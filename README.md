# argparse

This is a fork of argparse project initially developed by Andy Goth <andrew.m.goth@gmail.com>.

See [original repository](https://core.tcl-lang.org/tcllib/timeline?r=amg-argparse), and 
[wiki page](https://wiki.tcl-lang.org/page/argparse).

Starting from 0.6 version, the base implementation is in C. Tcl-only version is still availible and will be availible
in future with the same feature set. C version in order of magnitude faster than Tcl-only version due to cashing of
the parsed once argument definition.

## Installation

You have two ways: install from git repo (convinient for Linux users) or from archive package release.

Additionaly, package was added into "batteries included" Magicsplat 
[distribution](https://www.magicsplat.com/tcl-installer/index.html#packages) of Tcl8.6 and Tcl9.0.

### From git repo

To install default C implementation, run following commands:
- `git clone https://github.com/georgtree/argparse.git`
- `./configure`
- `sudo make install`

During installation manpages are also installed.

For test package in place run `make test`.

For package uninstall run `sudo make uninstall`.

To install Tcl-only version, the sequence is the same, except the flag `--enable-tcl-only` provided to `./configure`
script.

### Documentation

Documentation could be found [here](https://georgtree.github.io/argparse/).

## Author(s)

Andy Goth <andrew.m.goth@gmail.com> - all code and most of documentation

George Yashin <georgtree@gmail.com> - some documentation, test suite and new features, C implementation

Ashok P. Nadkarni <apnmbx-wits@yahoo.com> - fix of C implementation and valuable advices about the code architecture
