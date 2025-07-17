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


testTemplate typeTest-8 {} {} {
{-a= -type alpha}
{-b= -type boolean}
{-nterms= -default 3 -type integer -validate {$arg > 0}\
                 }} {-a y -b False -nterms 10.5} {a y b False c 10.5}

