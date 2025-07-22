package require argparse 0.60
package require argparse_c 




proc lsearch_ {args} {
    argparse -inline {
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
        {-increasing -key order -require sorted -default increasing}
        {-decreasing -key order -require sorted}
        {-bisect     -imply -sorted -forbid {all not}}
        -index=
        {-subindices -require index}
        list
        pattern
    }
}

timerate -calibrate {}
puts {lsearch times:}
puts [timerate {lsearch_ -inline -start 1 -exact -bisect {a b c} b}]

proc lsort_ {args} {
    argparse -inline {
        {-ascii      -key sort -value text -default text}
        {-dictionary -key sort}
        {-integer    -key sort}
        {-real       -key sort}
        {-command=   -forbid {ascii dictionary integer real}}
        {-increasing -key order -default increasing}
        {-decreasing -key order}
        -indices
        -index=
        -stride=
        -nocase
        -unique
        list
    }
}


timerate -calibrate {}
puts {lsort times:}
puts [timerate {lsort_ -increasing -real {2.0 1.0}}]


#time "bar 1000000"
