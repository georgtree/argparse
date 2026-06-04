package require tmdoc
package require fileutil

set currentDir [file normalize [file dirname [info script]]]

::tmdoc::tmdoc [file join $currentDir README.tmd] [file join $currentDir .. README.md]

proc processMd {fileContents} {
    return [string map {\{tclcode\} tcl \{tclout\} text} $fileContents]
}
fileutil::updateInPlace [file join $currentDir .. README.md] processMd

# nroff generating
set file [file join $currentDir .. README.md]
if {$tcl_platform(platform) eq {windows}} {
    exec md2man-roff.cmd [file join $currentDir .. README.md] > [file join $currentDir argparse.n]
} else {
    exec md2man-roff [file join $currentDir .. README.md] > [file join $currentDir argparse.n]
}
