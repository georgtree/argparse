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

# Package only argparse, without bundling the Tcl runtime or Tcllib.
make install-binaries
cp -R "$prefix/lib/argparse"* "$root/_ci/package/"
for package in "$root/_ci/package/argparse"*; do
    cp "$root/argparse.tcl" "$package/"
done
cp "$root/LICENSE" "$root/_ci/package/"
printf 'Tcl %s; %s\n' "$version" "${MSYSTEM:-Linux}" > "$root/_ci/package/build.txt"
