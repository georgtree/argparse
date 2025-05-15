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


testTemplate multipleAliasesTest-1 {} {} {
-a|e|i=
-e|b
-c
r} {-w 1 -c} {element alias collision: e}

testTemplate multipleAliasesTest-2 {} {} {
-a|w|i=
-b
-c
r} {-w 1 -c} {i 1 r -c}

testTemplate multipleAliasesTest-3 {} {} {
-a|b|i=
-b
-c
r} {-a 1 -c} {collision of switch -i alias with the -b switch}

testTemplate multipleAliasesTest-4 {} {} {
{-i= -alias {a b}}
-b
-c
r} {-b 1 -c} {collision of switch -i alias with the -b switch}

testTemplate multipleAliasesTest-5 {} {} {
{-i= -alias {a b y} -required}
-uib
-cab
r} {-y 1 -cab 1} {i 1 r 1 cab {}}

testTemplate multipleAliasesTest-6 {} {} {
{-i= -alias {a b y} -required}
-uib
-cab
r} {-cab 1} {missing required switch: -a|b|y|i}

testTemplate multipleAliasesTest-7 {} {} {
{-i= -alias {a b y} -required}
{-uib -alias {po kl y}}
-cab
r} {-cab 1} {missing required switch: -a|b|y|i}
