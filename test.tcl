package require argparse


proc genNums {args} {
    argparse {
        # {Optional sequence control switches}
        {-from= -default 1}
        {-to=   -default 10}
        {-step= -default 1}
        {-prec= -default 1}
        # {Required output list variable}
        listVar^
    }
    if {$step < 0} {
        set op ::tcl::mathop::>
    } else {
        set op ::tcl::mathop::<
    }
    for {set n $from} {[$op $n $to]} {set n [expr {$n + $step}]} {
        lappend listVar [format %.*f $prec $n]
    }
}
genNums -from 0 -to 10 -step 2 sequence
puts $sequence


proc applyOperator {args} {
    argparse {
        {-op= -enum {+ *}}
        {-elements= -catchall}
    }
    set result [lindex $elements 0]
    foreach element [lrange $elements 1 end] {
        set result [::tcl::mathop::$op $result $element]
    }
    return $result
}

puts [applyOperator -op * -elements 1 2 3 4 5]


proc exponentiation {args} {
    argparse {
        {-b!= -validate {[string is double $arg]}}
        {-n!= -validate {[string is double $arg]}}
    }
    return [expr {$b**$n}]
}

puts [exponentiation -b 2 -n 4]
