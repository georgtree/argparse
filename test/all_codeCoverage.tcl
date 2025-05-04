package require tcltest
namespace import ::tcltest::*
package require argparse
set currentDir [file normalize [file dirname [info script]]]
source [file join $currentDir .. argparse.tcl]
source [file join $currentDir argparse.test]
