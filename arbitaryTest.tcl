source ./argparse.tcl

proc genNums {args} {
    argparse {
        # {Optional sequence control switches}
        {-in= -required}
        {-from= -default 1}
        {-to=   -default 10}
        {-step= -default 1}
        {-prec= -default 1}
        {-rest -catchall}
        # {Required output list variable}
    }
    puts $rest
}
genNums -from 0 -to 10 -step 2 -a 6 -b 9 -in 1
