# argparse

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

### Documentation

Documentation could be found [here](https://georgtree.github.io/argparse/).

## Author(s)

Andy Goth <andrew.m.goth@gmail.com> - all code and most of documentation

George Yashin <georgtree@gmail.com> - some documentation, test suite and new features, C implementation

Ashok P. Nadkarni <apnmbx-wits@yahoo.com> - fix of C implementation and valuable advices about the code architecture
