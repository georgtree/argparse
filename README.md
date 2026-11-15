# argparse

[![Tests: Linux, Windows UCRT64 and MSVC](https://img.shields.io/github/actions/workflow/status/georgtree/argparse/tests.yml?branch=main&label=Tests%3A%20Linux%2C%20Windows%20UCRT64%20and%20MSVC)](https://github.com/georgtree/argparse/actions/workflows/tests.yml)

Tests cover **Linux · Windows UCRT64 · Windows MSVC**, with both C and Tcl implementations on Tcl 9.1.

This is a fork of argparse project initially developed by Andy Goth <andrew.m.goth@gmail.com>.

See [original repository](https://core.tcl-lang.org/tcllib/timeline?r=amg-argparse), and 
[wiki page](https://wiki.tcl-lang.org/page/argparse).

Starting from 0.6 version, the base implementation is in C. Tcl-only version is still availible and will be availible in
future with the same feature set. C version in order of magnitude faster than Tcl-only version due to cashing of the
parsed once argument definition.

## Installation

You have two ways: install from git repo (convinient for Linux users) or from archive package release.

Additionaly, package was added into "batteries included" Magicsplat 
[distribution](https://www.magicsplat.com/tcl-installer/index.html#packages) of Tcl8.6 and Tcl9.0.

### From git repo

To install default C implementation, run following commands:
- `git clone https://github.com/georgtree/argparse.git`
- `./configure`
- `sudo make install`

Both installation modes install the same manpages and HTML documentation, including assets. Manpages use
`$(mandir)/mann`; HTML uses `$(datadir)/argparse<version>/doc` (normally under the configured `--prefix`).  Both
destinations honor `DESTDIR` for staged installation.

Run `make test` to execute the complete suite once with the C implementation and once with the Tcl implementation, in
separate processes. In a `--enable-tcl-only` build it runs the Tcl implementation only. `make test-c` and `make
test-tcl` select one backend explicitly; `test-c` reports an error in a Tcl-only build.

Both runs use the same tests and expected results. Temporary package indexes select the requested backend in test
subprocesses and nested interpreters, without requiring installation or changing the build's `pkgIndex.tcl`. Both
backends are attempted even if one fails, and `make test` fails if either suite fails.

Standard tcltest filters apply to both runs, for example:

```sh
make test TESTFLAGS='-match exclusiveTest-*'
make test-c TESTFLAGS='-singleproc 1'
```

To run the Tcl suite directly, use `tclsh test/all.tcl`. To select a built C library, use `tclsh test/all.tcl -backend c
-packagefile /path/to/libargparse.so`, substituting the actual library filename. Backend options precede ordinary
tcltest options.

`test/memory.test` checks that repeated parsing releases input values in both variable-setting and `-inline` modes,
including failed calls. Reference-count checks use `::tcl::unsupported::representation` when available. Additional live
allocation checks run when Tcl was built with `--enable-symbols=mem`; they are skipped on ordinary builds. These checks
measure live Tcl allocations, not process RSS, which may remain high because the allocator reuses freed memory.  Scalar
regression cases cover positional parameters, mixed parsing, enum prefixes, and switch values, detecting leaked
argument-name copies even when input values themselves are correctly released.

Run `sudo make uninstall` to remove the package, its HTML documentation, and its manpages. `make uninstall-doc` removes
only the documentation. Both targets honor `DESTDIR` and the configured installation paths; unrelated manpages and other
packages' documentation are preserved.

To install Tcl-only version, the sequence is the same, except the flag `--enable-tcl-only` provided to `./configure`
script.

### Windows MSVC build

For a local MSVC build, use an **x64 Native Tools Command Prompt for VS 2022** with Tcl 9 installed, and run from
`win/`:

```bat
nmake /f makefile.vc INSTALLDIR=C:\Tcl
set TCLLIBPATH={C:/src/tcllib/modules}
nmake /f makefile.vc INSTALLDIR=C:\Tcl test
nmake /f makefile.vc INSTALLDIR=C:\Tcl install
```

The test target attempts both backends and fails if either fails. `TESTFLAGS` accepts the usual tcltest filters. The
MSVC build uses C11 support (Visual Studio 2019 16.8 or newer). Use a Tcl installation built for the same architecture.

`INSTALLDIR` names the installation root, for example `C:\tcltk\9.1-msvc`, not its `lib` subdirectory. Bundled build
helpers do not replace Tcl headers or its MSVC stub library (`lib\tclstub.lib` for Tcl 9). Use an MSVC-built Tcl for
this route; the UCRT64 workflow remains a separate MinGW build.

### Distribution archives

`make dist` creates `argparse<version>.tar.gz` and `argparse<version>.zip` under `/tmp/dist`. Override the destination
with `make dist DIST_ROOT=/path/to/dist`.

For MSVC, run `nmake /f makefile.vc INSTALLDIR=C:\Tcl dist` from `win/`.  Archives default to `$(OUT_DIR)\dist`; set
`DIST_ROOT=C:\path\to\dist` to override.  This target uses Windows `tar.exe` to create both archive formats.

All distributions include the active build's package index, Tcl source, README, license, complete test suite, and
checked-in generated HTML and nroff manuals with their assets. Native builds also include the compiled library. Tcl and
Tcllib are not bundled. Regenerate and commit the manual before packaging if its contents have changed.

The manual build workflow uploads the full `.tar.gz` distribution for Linux and `.zip` distributions for UCRT64 and
MSVC, inside the corresponding workflow artifacts. These archives can also be attached to a GitHub release.

### GitHub Actions

Every push runs the complete test suite with both the C and Tcl implementations on Tcl 9.1.0 in three configurations:
Linux GCC, Windows MSYS2 UCRT64, and Windows MSVC (x64).

The **Build packages** workflow uses the same three configurations and uploads one package artifact per
configuration. It runs only manually through **Actions → Build packages → Run workflow**.

### Documentation

Documentation could be found [here](https://georgtree.github.io/argparse/).

## Author(s)

Andy Goth <andrew.m.goth@gmail.com> - all code and most of documentation

George Yashin <georgtree@gmail.com> - some documentation, test suite and new features, C implementation

Ashok P. Nadkarni <apnmbx-wits@yahoo.com> - fix of C implementation and valuable advices about the code architecture

