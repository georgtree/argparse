package require SpiceGenTcl

set currentDir [file dirname [file normalize [info script]]]
global env
if {[string match -nocase *linux* $tcl_platform(os)]} {
    set nagelfarPath "/home/$env(USER)/tcl/nagelfar/nagelfar.tcl"
} elseif {[string match -nocase "*windows nt*" $tcl_platform(os)]} {
    set nagelfarPath "C:\\msys64\\home\\georgtree1\\nagelfar\\nagelfar.tcl"
}

set srcList {argparse.tcl}
set dbPath [file join $currentDir syntaxdb.tcl]
foreach file $srcList {
    lappend paths [file normalize [file join $currentDir .. {*}$file]]
}

puts [exec {*}[list tclsh $nagelfarPath -s $dbPath {*}$paths]]
