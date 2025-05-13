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


proc sheduleEvent {args} {
    set arguments [argparse -inline\
            -validate [dict create date {[regexp {^(0[1-9]|[12][0-9]|3[01])-(0[1-9]|1[0-2])-\d{4}$} $arg]}\
                time {[regexp {^([01][0-9]|2[0-3]):[0-5][0-9](?::[0-5][0-9])?$} $arg]}] {
        {date -validate date}
        {time -validate time}
        {-allday -allow {date time}}
        {-duration= -allow {date time} -validate time}
        {-endtime= -allow {date time} -validate time}
    }]
    if {![dict exists $arguments allday] && ![dict exists $arguments duration] && ![dict exists $arguments endtime]} {
        return -code error "One of the switch must be presented: -allday, -duration or -endtime"
    }
    return $arguments
}
sheduleEvent
