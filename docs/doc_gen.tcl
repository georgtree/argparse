package require tmdoc
package require fileutil

set currentDir [file normalize [file dirname [info script]]]

::tmdoc::tmdoc [file join $currentDir README.tmd] [file join $currentDir .. README.md]

proc processMd {fileContents} {
    return [string map {tclcode tcl tclout text} $fileContents]
}
fileutil::updateInPlace [file join $currentDir .. README.md] processMd

# nroff generating
set file [file join $currentDir .. README.md]
exec pandoc -s -t man [file join $currentDir .. README.md] -o [file join $currentDir argparse.n]

