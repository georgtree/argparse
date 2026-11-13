# Select one backend for this process, its test-file subprocesses, and any
# interpreters created by tests. All backends run the same .test files.
set currentDir [file normalize [file dirname [info script]]]
set backend tcl
set packageFile [file join $currentDir .. argparse.tcl]

while {[llength $argv] && ([lindex $argv 0] in {-backend -packagefile})} {
    set option [lindex $argv 0]
    if {[llength $argv] < 2} {
        return -code error "$option requires an argument"
    }
    if {$option eq {-backend}} {
        set backend [lindex $argv 1]
    } else {
        set packageFile [lindex $argv 1]
    }
    set argv [lrange $argv 2 end]
}
if {$backend ni {c tcl}} {
    return -code error "bad backend \"$backend\": must be c or tcl"
}

set packageFile [file normalize $packageFile]
if {![file isfile $packageFile]} {
    return -code error "backend file does not exist: $packageFile"
}

if {$backend eq {c}} {
    set loadCommand [list load $packageFile Argparse]
} else {
    set loadCommand [list source $packageFile]
}

{*}$loadCommand
set version [package require argparse]
if {([info procs ::argparse] ne {}) != ($backend eq "tcl")} {
    return -code error "requested $backend backend was not loaded"
}
puts "Testing argparse $version ($backend backend): $packageFile"

# A private package index makes package require select the same backend even in
# fresh interpreters. Tcl-list quoting preserves paths containing spaces.
set channel [file tempfile anchor]
close $channel
set packageDir ${anchor}.d
try {
    file mkdir $packageDir
    set channel [open [file join $packageDir pkgIndex.tcl] w]
    try {
        puts $channel [list package ifneeded argparse $version $loadCommand]
    } finally {
        close $channel
    }
    set auto_path [linsert $auto_path 0 $packageDir]
    set ::env(TCLLIBPATH) $auto_path
    # Initialize tcltest after the environment is set, so cleanupTests preserves
    # backend selection between files in -singleproc mode too.
    package require tcltest
    namespace import ::tcltest::*
    configure {*}$argv -testdir $currentDir
    set failed [runAllTests]
} finally {
    file delete -force $packageDir $anchor
}

exit $failed
