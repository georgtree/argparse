source ./argparse.tcl
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


testTemplate allowTest-1 {} {} {
{-a= -allow {c}}
-b=
-c=
-d=
-e=
{f -optional}
g} {-a 1 -c 4 4 5} {had 5 rest {-e 2 io p} cer 1 lab 4}

testTemplate allowTest-1 {} {} {
{-a= -allow {c}}
-b=
-c=
-d=
-e=
{f -optional}
g} {-a 1 -c 4 5} {had 5 rest {-e 2 io p} cer 1 lab 4}

testTemplate allowTest-1 {} {} {
{-a= -require {g}}
-b=
-c=
-d=
-e=
{f -optional}
g} {-a 1 5 5} {had 5 rest {-e 2 io p} cer 1 lab 4}
