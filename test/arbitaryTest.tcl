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


proc argProcSeq {args} {
    set arguments [argparse -inline -mixed {
        -a=
        -b=
        -c
        d
        e?
        f
        {g -catchall}
    }]
    return $arguments
}

puts [argProcSeq 10 -c -a 1 -- -2 3 4]
