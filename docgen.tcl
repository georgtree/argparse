package require tmdoc
package require fileutil

set currentDir [file normalize [file dirname [info script]]]

::tmdoc::tmdoc [file join $currentDir README.tmd] README.md

proc processMd {fileContents} {
    return [string map {\{tclcode\} tcl \{tclout\} text} $fileContents]
}
fileutil::updateInPlace [file join $currentDir README.md] processMd
