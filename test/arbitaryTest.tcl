package require argparse
package require tcltest
namespace import ::tcltest::*

const templateProcStr {proc templateProc {args} {
    set arguments [argparse @additionalArgs@ {
        @definitions@
    }]
    set resultDict {}
    if {$arguments ne {}} {
        set exclude {args resultDict exclude}
    } else {
        set exclude {args resultDict arguments exclude}
    }
    foreach locVar [info locals] {
        if {$locVar in $exclude} {
            continue
        } else {
            dict append resultDict $locVar [subst $[subst $locVar]]
        }
    }
    return $resultDict
}}

proc testTemplate {testName descr arguments definitionsStr inputStr refStr} {
    variable templateProcStr
    test $testName $descr -body {
        eval [string map [list @definitions@ $definitionsStr @additionalArgs@ $arguments] $templateProcStr]
        if {[catch {set result [templateProc {*}$inputStr]} errorStr]} {
            return $errorStr
        } else {
            return $result
        }
    } -result $refStr -cleanup {
        rename templateProc {}
    }
}


testTemplate helpTest-2 {} {-help {Description of procedure} -helpret} {
{-exact      -key match}
{-glob       -key match -default glob}
{-regexp     -key match}
{-sorted     -forbid {glob regexp}}
-all
-inline
-not
-start=
{-ascii      -key format -value text -default text}
{-dictionary -key format}
{-integer    -key format}
{-nocase     -key format}
{-real       -key format}
{-increasing -key order -require {sorted ascii} -default increasing}
{-decreasing -key order -require sorted}
{-bisect     -imply -sorted -forbid {all not}}
-index=
{-subindices -require index}
list
pattern} {-help} {Description of procedure. Can accepts unambiguous prefixes instead of switches
names. Accepts switches only before parameters.
    Switches:
        -exact - Forbids glob or regexp.
        -glob - Forbids exact or regexp.
        -regexp - Forbids exact or glob.
        -sorted - Forbids glob or regexp.
        -all
        -inline
        -not
        -start - Expects argument.
        -ascii - Forbids dictionary, integer, nocase or real.
        -dictionary - Forbids ascii, integer, nocase or real.
        -integer - Forbids ascii, dictionary, nocase or real.
        -nocase - Forbids ascii, dictionary, integer or real.
        -real - Forbids ascii, dictionary, integer or nocase.
        -increasing - Requires sorted. Forbids decreasing.
        -decreasing - Requires sorted. Forbids increasing.
        -bisect - Forbids all or not. Expects two arguments.
        -index - Expects argument.
        -subindices - Requires index.
        -help - Help switch, when provided, forces ignoring all other switches
            and parameters, prints the help message to stdout, and returns up to 2
            levels above the current level.
    Parameters:
        list
        pattern}



