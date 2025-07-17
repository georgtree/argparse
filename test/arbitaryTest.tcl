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


testTemplate helpTest-2 {} {-inline -pfirst} {
    {-model= -required -help {Name of the model}}
    {-area= -help {Emitter scale factor}}
    {-areac= -help {Collector scale factor}}
    {-areab= -help {Base scale factor}}
    {-m= -help {Multiplier of area and perimeter}}
    {-ic= -validate {[llength $arg]==2} -help {Initial conditions for vds and vgs, in form of two element\
                                                       list}}
    {-temp= -forbid {dtemp} -help {Device temperature}}
    {-dtemp= -forbid {temp} -help {Temperature offset}}
    {-ns= -help {Name of node connected to substrate pin}}
    {-tj= -require {ns} -help {Name of node connected to thermal pin}}
    {-off -boolean -help {Initial state}}
    {name -help {Name of the device without first-letter designator}}
    {nc -help {Name of node connected to collector pin}}
    {nb -help {Name of node connected to base pin}}
    {ne -help {Name of node connected to emitter pin}}} {1 netc netb nete -model bjtmod -ns nets -tj nettj -area 1e-3 -temp 25 -ic {1 2}} {-exact conflicts with -regexp}

