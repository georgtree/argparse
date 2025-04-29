global env
if {[string match -nocase *linux* $tcl_platform(os)]} {
    set nagelfarPath "/home/$env(USER)/tcl/nagelfar/nagelfar.tcl"
} elseif {[string match -nocase "*windows nt*" $tcl_platform(os)]} {
    set nagelfarPath "C:\\msys64\\home\\georgtree1\\nagelfar\\nagelfar.tcl"
}

set currentDir [file dirname [file normalize [info script]]]
set srcList {argparse.tcl}
foreach file $srcList {
    exec tclsh $nagelfarPath -instrument [file join $currentDir {*}$file]
}
foreach file $srcList {
    file rename [file join $currentDir [lindex $file 0]] [file join $currentDir [lindex $file 0]_orig]
    file rename [file join $currentDir  [lindex $file 0]_i] [file join $currentDir [lindex $file 0]]
}
exec tclsh [file join $currentDir argparse.test]
foreach file $srcList {
    file rename [file join $currentDir [lindex $file 0]] [file join $currentDir [lindex $file 0]_i]
    file rename [file join $currentDir [lindex $file 0]_orig] [file join $currentDir [lindex $file 0]]
}

set coveredSum 0
set totalSum 0
foreach file $srcList {
    set result [exec tclsh $nagelfarPath -markup [file join $currentDir {*}$file]]
    lappend results $result
    if {[regexp {(\d+)/(\d+)\s+(\d+(\.\d+)?)%} $result match num1 num2 num3]} {
        set coveredSum [expr {$num1+$coveredSum}]
        set totalSum [expr {$num2+$totalSum}]
    }
}

foreach file $srcList {
    exec eskil -noparse [file join ${currentDir} [lindex $file 0]] [file join $currentDir [lindex $file 0]_m]
}
puts [join $results "\n"]
puts "Covered $coveredSum of $totalSum branches, percentage is [expr {double($coveredSum)/double($totalSum)*100}]%"

foreach file $srcList {
    file delete [file join $currentDir [lindex $file 0]_i]
    file delete [file join $currentDir [lindex $file 0]_log]
    file delete [file join $currentDir [lindex $file 0]_m]
}
