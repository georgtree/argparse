package require tcltest
namespace import ::tcltest::*
package require argparse
set currentDir [file normalize [file dirname [info script]]]
configure {*}$argv -testdir $currentDir
runAllTests
