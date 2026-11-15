#!/usr/bin/env bash
set -euo pipefail

version=$1
root=$(pwd)
prefix="$root/_ci/runtime"
platform=unix
if [[ ${MSYSTEM:-} == UCRT64 ]]; then
    platform=win
fi
mkdir -p _ci/tcl-build _ci/argparse-build _ci/package
cd _ci/tcl-build
"$root/_ci/tcl/$platform/configure" --prefix="$prefix"
make -j2
make install
cd "$root/_ci/argparse-build"
"$root/configure" --with-tcl="$prefix/lib" --prefix="$prefix" --exec-prefix="$prefix"
make -j2

# Native Windows Tcl needs a Windows path; Tcl lists need braces around spaces.
modules="$root/_ci/tcllib/modules"
if [[ $platform == win ]]; then
    modules=$(cygpath -m "$modules")
fi
export TCLLIBPATH="{$modules}"
make test TCLLIBPATH="$TCLLIBPATH"

# Full distribution, including tests and the checked-in generated manual.
if [[ $platform == win ]]; then
    make dist-zip DIST_ROOT="$root/_ci/dist"
    for archive in "$root/_ci/dist/"*.zip; do
        cp "$archive" "$root/_ci/package/$(basename "$archive" .zip)-windows-ucrt64.zip"
    done
else
    make dist DIST_ROOT="$root/_ci/dist"
    for archive in "$root/_ci/dist/"*.tar.gz; do
        cp "$archive" "$root/_ci/package/$(basename "$archive" .tar.gz)-linux-gcc.tar.gz"
    done
fi
