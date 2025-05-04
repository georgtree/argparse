# argparse.tcl --
#
# Feature-heavy argument parsing package
#
# Copyright (C) 2019 Andy Goth <andrew.m.goth@gmail.com>
# See the file "license.terms" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
package require Tcl 8.6-
package provide argparse 0.51
interp alias {} @ {} lindex
interp alias {} = {} expr
# argparse --
# Parses an argument list according to a definition list.  The result may be
# stored into caller variables or returned as a dict.
#
# Zero or more initial arguments are overall options:
#
# -inline        Only return the result dict, but do not set caller variables
# -keep          Do not unset omitted element variables; conflicts with -inline
# -exact         Require exact option name matches, and do not accept prefixes
# -mixed         Allow options to appear after parameters
# -long          Recognize "--option" long option alternative syntax
# -equal         Recognize "-option=arg" inline argument alternative syntax
# -normalize     Normalize option syntax in passthrough result keys
# -pass KEY      Pass unrecognized elements through to a result key
# -ignore        Ignore unrecognized elements; conflicts with -pass
# -argument      Unrecognized options require arguments; requires -pass/-ignore
# -optional      Unrecognized option arguments are optional; requires -argument
# -level LEVEL   Default value for element -level; defaults to 1
#
# The first non-option argument is the definition list.  The second argument is
# the argument list to be parsed, typically $args.
#
# Each definition list element is a list whose first word is a unique, non-empty
# name word consisting of alphanumerics, underscores, and minus (may not be the
# first character), followed by zero or more option words:
#
# -option        Element is an option; conflicts with -parameter
# -parameter     Element is a parameter; conflicts with -option
# -alias ALIAS   Alias name; requires -option
# -ignore        Element is omitted from result; conflicts with -key and -pass
# -key KEY       Override key name; not affected by -template
# -pass KEY      Pass through to result key; not affected by -template
# -default VAL   Value if omitted; conflicts with -required and -keep
# -keep          Do not unset if omitted; requires -optional; conflicts -inline
# -value VAL     Value if present; requires -option; conflicts with -argument
# -argument      Value is next argument following option; requires -option
# -optional      Option value is optional, or parameter is optional
# -required      Option is required, or stop -catchall from implying -optional
# -catchall      Value is list of all otherwise unassigned arguments
# -upvar         Links caller variable; conflicts with -inline and -catchall
# -level LEVEL   This element's [upvar] level; requires -upvar
# -standalone    If element is present, ignore -required, -require, and -forbid
# -require LIST  If element is present, other elements that must be present
# -forbid LIST   If element is present, other elements that must not be present
# -imply LIST    If element is present, extra option arguments; requires -option
# -reciprocal    This element's -require is reciprocal; requires -require
# -validate DEF  Name of validation command, or inline validation definition
# -enum DEF      Name of enumeration list, or inline enumeration definition
#
# If the definition list element's first word is empty, its subsequent words are
# overall options (-inline, -keep, -exact, -mixed, -long, -equal, -normalize,
# -pass, -ignore, -argument, -optional, -level) and/or the following options:
#
# -reciprocal    Every element's -require constraints are reciprocal
# -template TMP  Compute default element keys using a substitution template
# -validate DEF  Define named validation commands to be used by elements
# -enum DEF      Define named enumeration lists to be used by elements
#
# If the definition list element's first word is "#", the element is ignored as
# a comment.  Subsequent words in the element may contain any text at all.
#
# If neither -option nor -parameter are used, a shorthand form is permitted.  If
# the name is preceded by "-", it is an option; otherwise, it is a parameter.
# An alias may be written after "-", then followed by "|" and the option name.
# The element name may be followed by any number of flag characters:
#
# "="            Same as -argument; only valid for options
# "?"            Same as -optional
# "!"            Same as -required
# "*"            Same as -catchall
# "^"            Same as -upvar
#
# -value specifies the value to assign to keys when the element is present.  It
# cannot be used with -parameter, -argument, -optional, -required, or -catchall.
# -value defaults to 1, unless multiple options share the same -key, in which
# case the element name is the default -value.
#
# -default specifies the value to assign to keys when the element is omitted.
# If multiple options share the same -key, at most one of them may use -default,
# which specifies the value to assign when all of them are omitted.  The rest of
# this paragraph describes the case where -default is not used.  If -argument,
# -parameter, or -value is given, or if multiple options share the same -key,
# keys for omitted elements are not in the result.  If -catchall is used on a
# parameter, the default is empty.  Otherwise, the default is 0.
#
# At most one parameter may use -catchall.
#
# Multiple elements may share the same -key value if they are options, do not
# use -argument or -catchall, and do not use -default for more than one element.
# Such elements are automatically are given -forbid constraints to prevent them
# from being used simultaneously.  If such an element does not use -value, the
# element name is used as its default -value.
#
# -optional, -required, -catchall, and -upvar imply -argument when used with
# -option.  Consequently, options require an argument when any of the shorthand
# flag characters defined above are used, and it is not necessary to explicitly
# specify "=" if any of the other flag characters are used.
#
# If -argument is used, the value assigned to the option's key is normally the
# next argument following the option.  With -catchall, the value assigned to the
# option's key is instead the list of all remaining arguments.  With -optional,
# the following processing is applied:
#
# - If the option is not present, the option's key is omitted from the result.
# - If the option is not the final argument, its value is a list whose sole word
#   is the argument following the option.
# - If the option is the final argument, its value is empty.
#
# By default, options are optional and parameters are required.  Options can
# be made required with -required, and parameters can be made optional with
# -optional.  -catchall also makes parameters optional, unless -required is
# used, in which case at least one argument must be assigned to the parameter.
# Otherwise, using -required with -parameter has no effect.  -option -optional
# -required means the option must be present but may be the final argument.
#
# When -option and -optional are both used, -catchall, -default, and -upvar are
# disallowed.  -parameter -optional -required is also a disallowed combination.
#
# -validate and -enum provide element value validation.  The overall -validate
# and -enum options declare named validation commands and enumeration lists, and
# the per-element -validate and -enum options select which validation commands
# and enumeration lists are used on which elements.  The argument to the overall
# -validate and -enum options is a dict mapping from validation or enumeration
# names to validation command prefixes or enumeration lists.  The argument to a
# per-element -validate option is a validation name or command prefix, and the
# argument to a per-element -enum option is an enumeration name or list.  An
# element may not use both -validate and -enum.
#
# A validation command prefix is a list to which the argument is appended before
# being invoked.  In the case of -catchall, the option or parameter's whole list
# of arguments is appended as a single argument word.  The validation command's
# result is used in place of the argument, unless it throws an error.  Thus, a
# simple validator will return its argument if valid or throw if invalid, and a
# complex validator can transform its argument if need be.  The validator must
# return its argument verbatim if the argument is a possible return value, and a
# catchall element's validator must return a list.  The validation command may
# not [yield], except inside coroutines it started.
#
# An enumeration list is a list of possible argument values.  If the argument
# appears in the enumeration list, the argument is accepted.  Unless -exact is
# used, if the argument is a prefix of exactly one element of the enumeration
# list, the argument is replaced with the enumeration list element.
#
# Unless -exact is used, unambiguous prefixes of option names (not aliases) are
# acceptable.  Options in the argument list normally begin with a single "-" but
# can also begin with "--" if -long is used.  Arguments to options normally
# appear as the list word following the option, but if -equal is used, they may
# be supplied within the option element itself, delimited with an "=" character,
# e.g., "-option=arg".
#
# The per-element -pass option causes the element argument or arguments to be
# appended to the value of the indicated passthrough result key.  Elements may
# share a passthrough key.  -normalize causes option arguments to be normalized
# to not use aliases, abbreviations, the "--" prefix, or the "=" argument
# delimiter; otherwise, options will be expressed the same as they appear in the
# original input.  Furthermore, -normalize causes omitted options that accept
# arguments and have default values, as well as omitted parameters that have
# default values, to be explicitly included in the passthrough key.  If -mixed
# is used, passthrough keys will list all options first before listing any
# parameters.  If the first parameter value for a passthrough key starts with
# "-", its value will be preceded by "--" so it will not appear to be an option.
# If no arguments are assigned to a passthrough key, its value will be empty.
# The value of a passthrough key can be reparsed to get the original data, and
# if -normalize is used, it will not be necessary to use -mixed, -long, -equal,
# -alias, or -default to get the correct result.  Pathological use of -default
# can conflict with this goal.  For example, if the first optional parameter has
# no -default but the second one does, then parsing the result of -normalize can
# assign the default value to the first parameter rather than the second.
#
# The overall -pass option may be used to collect unrecognized arguments into a
# passthrough key, rather than failing with an error.  Arguments starting with
# "-", when not following "--", are assumed to be options that do not expect an
# argument, unless -argument is used.  For fine control, explicitly list all
# options that expect arguments, but configure them to have the same passthrough
# key as the overall passthrough.  If -equal is used, unknown options that are
# given arguments via the "-option=arg" syntax are assumed to expect arguments.
# Many uses of overall -pass should also use -exact, or else some options that
# may be expected to pass through instead may be allocated to a defined option
# whose name begins with that of the intended passthrough option.
#
# Overall -ignore is similar to overall -pass except it ignores the arguments
# that would otherwise have been put into a passthrough key.
#
# When using -imply, be careful to avoid creating an infinite loop.  Never allow
# an option to imply itself, neither directly nor indirectly.
#
# [args::parse] returns a dict.  By default, the element names are the keys,
# unless overridden by -key, -pass, or -template.  If both -key and -pass are
# used, two keys are defined: one having the element value, the other having the
# passthrough elements.  Unline -inline is used, the values are also stored into
# caller variables named the same as the keys.  Unless -keep or -inline are
# used, the caller variables for omitted options and parameters are unset.
#
# -template applies to elements using neither -key nor -pass.  Its value is a
# substitution template applied to element names to determine key names.  "%" in
# the template is replaced with the element name.  To protect "%" or "\" from
# replacement, precede it with "\".  One use for -template is to put the result
# in an array, e.g., with "-template arrayName(%)".
#
# Elements with -upvar are special.  Rather than having normal values, they are
# bound to caller variables using the [upvar] command.  -upvar conflicts with
# -inline because it is not possible to map a dict value to a variable.  Due to
# limitations of arrays and [upvar], -upvar cannot be used with keys whose names
# resemble array elements.  -upvar conflicts with -catchall because the value
# must be a variable name, not a list.  The combination -option -optional -upvar
# is disallowed for the same reason.  If -upvar is used with options or with
# optional parameters, [info exists KEY] returns 1 both when the element is not
# present and when its value is the name of a nonexistent variable.  To tell the
# difference, check if [info vars KEY] returns empty; if so, the element is not
# present.  Note that the argument to [info vars] is a [string match] pattern,
# so it is necessary to precede *?[]\ characters with backslashes.
#
# Argument processing is performed in three stages: option processing, parameter
# allocation, and parameter assignment.  Each argument processing stage and pass
# is performed left-to-right.
#
# All options must normally appear in the argument list before any parameters.
# Option processing terminates with the first argument (besides arguments to
# options) that does not start with "-" (or "--", if -long is used).  The
# special option "--" forces option termination, which is useful if the first
# parameter happens to start with "-".  If no options are defined, the first
# argument is known to be a parameter even if it starts with "-".
#
# When the -mixed option is used, option processing continues after encountering
# arguments that do not start with "-" or "--".  This is convenient but may be
# ambiguous in cases where parameters look like options.  To resolve ambiguity,
# the special "--" option terminates option processing and forces all remaining
# arguments to be parameters.
#
# When -mixed is not used, the required parameters are counted, then that number
# of arguments at the end of the argument list are treated as parameters even if
# they begin with "-".  This avoids the need for "--" in many cases.
#
# After option processing, parameter allocation determines how many arguments to
# assign to each parameter.  Arguments assigned to options are not used in
# parameter processing.  First, arguments are allocated to required parameters;
# second, to optional, non-catchall parameters; and last to catchall parameters.
# Finally, each parameter is assigned the allocated number of arguments.
proc ::argparse {args} {
### Common validation helper routine.
    set validateHelper {apply {{name opt args} {
        if {[dict exists $opt enum]} {
            set command [list tcl::prefix match -message "$name value"\
                                 {*}[if {[uplevel 1 {info exists exact}]} {list -exact}] [dict get $opt enum]]
            set args [lmap arg $args {{*}$command $arg}]
        } elseif {[dict exists $opt validate]} {
            foreach arg $args [list if [dict get $opt validate] {} else {
                return -code error -level 2 "$name value \"$arg\" fails [dict get $opt validateMsg]"
            }]
        }
        return $args
    }}}
### Type checking routine
    set typeChecker {apply {{name opt args} {
        if {[dict exists $opt type]} {
            foreach arg $args {
                if {![string is [dict get $opt type] -strict $arg]} {
                    return -code error -level 2 "$name value \"$arg\" is not of the type [dict get $opt type]"
                }
            }
        }
        return $args
    }}}
### Process arguments to argparse procedure
    set level 1
    set enum {}
    set validate {}
    set globalSwitches {-boolean -enum -equalarg -exact -inline -keep -level -long -mixed -normalize -pass\
                                -reciprocal -template -validate}
    for {set i 0} {$i<[llength $args]} {incr i} {
        if {[catch {regsub {^-} [tcl::prefix match -message switch $globalSwitches [@ $args $i]] {} switch} errorMsg]} {
            # Do not allow "--" or definition lists nested within the special
            # empty-string element containing extra overall switches.
            if {[info exists reparse]} {
                return -code error $msg
            }
            # Stop after "--" or at the first non-switch argument.
            if {[@ $args $i] eq {--}} {
                incr i
            }
            # Extract definition and args from the argument list, pulling from
            # the caller's args variable if the args parameter is omitted.
            switch [= {[llength $args]-$i}] {
            0 {
                break
            } 1 {
                set definition [@ $args end]
                set argv [uplevel 1 {::set args}]
            } 2 {
                set definition [@ $args end-1]
                set argv [@ $args end]
            } default {
                return -code error {too many arguments}
            }}
            # Convert any definition list elements named empty string to instead
            # be overall switches, and arrange to reparse those switches.  Also,
            # remove inline comments from the definition list.
            set args {}
            set reparse {}
            set i -1
            foreach elem $definition[set definition {}] {
                if {[@ $elem 0] eq "#"} {
                    if {[llength $elem] == 1} {
                        set comment {}
                    }
                } elseif {[info exists comment]} {
                    unset comment
                } elseif {[@ $elem 0] eq {}} {
                    lappend args {*}[lrange $elem 1 end]
                } else {
                    lappend definition $elem
                }
            }
        } elseif {$switch ni {enum level pass template validate}} {
            # Process switches with no arguments.
            set $switch {}
        } elseif {$i == [llength $args]-1} {
            return -code error "-$switch requires an argument"
        } else {
            # Process switches with arguments.
            set $switch [@ $args [incr i]]
        }
    }
    # Fail if no definition argument was supplied.
    if {![info exists definition]} {
        return -code error {missing required parameter: definition}
    }
    # Forbid using -inline and -keep at the same time.
    if {[info exists inline] && [info exists keep]} {
        return -code error {-inline and -keep conflict}
    }
### Parse element definition list.
    set def {}
    set aliases {}
    set order {}
    set switches {}
    set upvars {}
    set omitted {}
    foreach elem $definition {
####  Read element definition switches.
        set opt {}
        set defsSwitches {-alias -argument -boolean -catchall -default -enum -forbid -ignore -imply -keep -key -level\
                                  -optional -parameter -pass -reciprocal -require -required -standalone -switch -upvar\
                                  -validate -value -type}
        set defsSwitchesWArgs {alias default enum forbid imply key pass require validate value type}
        for {set i 1} {$i<[llength $elem]} {incr i} {
            if {[set switch [regsub {^-} [tcl::prefix match $defsSwitches [@ $elem $i]] {}]] ni $defsSwitchesWArgs} {
#####   Process switches without arguments.
                dict set opt $switch {}
            } elseif {$i == [llength $elem]-1} {
                return -code error "-$switch requires an argument"
            } else {
#####   Process switches with arguments.
                incr i
                dict set opt $switch [@ $elem $i]
            }
        }
####  Process the first element of the element definition.
        if {![llength $elem]} {
            return -code error {element definition cannot be empty}
        } elseif {[dict exists $opt switch] && [dict exists $opt parameter]} {
            return -code error {-switch and -parameter conflict}
        } elseif {[info exists inline] && [dict exists $opt keep]} {
            return -code error {-inline and -keep conflict}
        } elseif {![dict exists $opt switch] && ![dict exists $opt parameter]} {
            # If -switch and -parameter are not used, parse shorthand syntax.
            if {![regexp -expanded {
                ^(?:(-)             # Leading switch "-"
                (?:(\w[\w-]*)\|)?)? # Optional switch alias
                (\w[\w-]*)          # Switch or parameter name
                ([=?!*^]*)$         # Optional flags
            } [@ $elem 0] _ minus alias name flags]} {
                return -code error "bad element shorthand: [@ $elem 0]"
            }
            if {$minus ne {}} {
                dict set opt switch {}
            } else {
                dict set opt parameter {}
            }
            if {$alias ne {}} {
                dict set opt alias $alias
            }
            foreach flag [split $flags {}] {
                dict set opt [dict get {= argument ? optional ! required * catchall ^ upvar} $flag] {}
            }
        } elseif {![regexp {^\w[\w-]*$} [@ $elem 0]]} {
            return -code error "bad element name: [@ $elem 0]"
        } else {
            # If exactly one of -switch or -parameter is used, the first element
            # of the definition is the element name, with no processing applied.
            set name [@ $elem 0]
        }
####  Check for collisions.
        if {[dict exists $def $name]} {
            return -code error "element name collision: $name"
        }
        if {[dict exists $opt switch]} {
####  Add -argument switch if particular switches are presented
            # -optional, -required, -catchall, and -upvar imply -argument when
            # used with switches.
            foreach switch {optional required catchall upvar type} {
                if {[dict exists $opt $switch]} {
                    dict set opt argument {}
                }
            }
        } else {
####  Add -required switch for parameters
            # Parameters are required unless -catchall or -optional are used.
            if {([dict exists $opt catchall] || [dict exists $opt optional]) && ![dict exists $opt required]} {
                dict set opt optional {}
            } else {
                dict set opt required {}
            }
        }
####  Check requirements and conflicts.
        foreach {switch other} {reciprocal require   level upvar} {
            if {[dict exists $opt $switch] && ![dict exists $opt $other]} {
                return -code error "-$switch requires -$other"
            }
        }
        foreach {switch others} {
            parameter {alias boolean value argument imply}
            ignore    {key pass}
            required  {boolean default}
            argument  {boolean value}
            upvar     {boolean inline catchall}
            boolean   {default value}
            enum      validate
            type      {upvar boolean enum}
        } {
            if {[dict exists $opt $switch]} {
                foreach other $others {
                    if {[dict exists $opt $other]} {
                        return -code error "-$switch and -$other conflict"
                    }
                }
            }
        }
        if {[dict exists $opt upvar] && [info exists inline]} {
            return -code error {-upvar and -inline conflict}
        }
####  Check for disallowed combinations.
        foreach combination {
            {switch optional catchall}
            {switch optional upvar}
            {switch optional default}
            {switch optional boolean}
            {switch optional type}
            {parameter optional required}
        } {
            foreach switch [list {*}$combination {}] {
                if {$switch eq {}} {
                    return -code error "[join [lmap switch $combination {
                        string cat - $switch
                    }]] is a disallowed combination"
                } elseif {![dict exists $opt $switch]} {
                    break
                }
            }
        }
####  Replace -boolean with "-default 0 -value 1".
        if {([info exists boolean] && [dict exists $opt switch] && ![dict exists $opt argument]\
                     && ![dict exists $opt upvar] && ![dict exists $opt default] && ![dict exists $opt value]\
                     && ![dict exists $opt required]) || [dict exists $opt boolean]} {
            dict set opt default 0
            dict set opt value 1
        }
####  Insert default -level if -upvar is used.
        if {[dict exists $opt upvar] && ![dict exists $opt level]} {
            dict set opt level $level
        }
####  Compute default output key if -ignore, -key, and -pass aren't used.
        if {![dict exists $opt ignore] && ![dict exists $opt key] && ![dict exists $opt pass]} {
            if {[info exists template]} {
                dict set opt key [string map [list \\\\ \\ \\% % % $name] $template]
            } else {
                dict set opt key $name
            }
        }
        if {[dict exists $opt parameter]} {
####  Keep track of parameter order.
            lappend order $name
####  Forbid more than one catchall parameter.
            if {[dict exists $opt catchall]} {
                if {[info exists catchall]} {
                    return -code error "multiple catchall parameters: $catchall and $name"
                } else {
                    set catchall $name
                }
            }
        } elseif {![dict exists $opt alias]} {
####  Build list of switches.
            lappend switches -$name
        } elseif {![regexp {^\w[\w-]*$} [dict get $opt alias]]} {
            return -code error "bad alias: [dict get $opt alias]"
        } elseif {[dict exists $aliases [dict get $opt alias]]} {
            return -code error "element alias collision: [dict get $opt alias]"
        } else {
####  Build list of switches (with aliases), and link switch aliases.
            dict set aliases [dict get $opt alias] $name
            lappend switches -[dict get $opt alias]|$name
        }
####  Map from upvar keys back to element names, and forbid collisions.
        if {[dict exists $opt upvar] && [dict exists $opt key]} {
            if {[dict exists $upvars [dict get $opt key]]} {
                return -code error "multiple upvars to the same variable: [dict get $upvars [dict get $opt key]] $name"
            }
            dict set upvars [dict get $opt key] $name
        }
####  Look up named enumeration lists and validation expressions.
        if {[dict exists $opt enum] && [dict exists $enum [dict get $opt enum]]} {
            dict set opt enum [dict get $enum [dict get $opt enum]]
        } elseif {[dict exists $opt validate]} {
            if {[dict exists $validate [dict get $opt validate]]} {
                dict set opt validateMsg "[dict get $opt validate] validation"
                dict set opt validate [dict get $validate [dict get $opt validate]]
            } else {
                dict set opt validateMsg "validation: [dict get $opt validate]"
            }
        }
####  Check for allowed arguments to -type
        if {[dict exists $opt type]} {
            set allowedTypes {alnum alpha ascii boolean control dict digit double graph integer list lower print punct\
                                      space upper wideinteger wordchar xdigit}
            if {[dict get $opt type] ni $allowedTypes} {
                return -code error "type [dict get $opt type] is not in the list of allowed types, must be\
                        [join [lrange $allowedTypes 0 end-1] {, }] or [@ $allowedTypes end]"
            }
        }
####  Save element definition.
        dict set def $name $opt
####  Prepare to identify omitted elements.
        dict set omitted $name {}
    }
### Process constraints and shared key logic.
    dict for {name opt} $def {
####  Verify constraint references.
        foreach constraint {require forbid} {
            if {[dict exists $opt $constraint]} {
                foreach otherName [dict get $opt $constraint] {
                    if {![dict exists $def $otherName]} {
                        return -code error "$name -$constraint references undefined element: $otherName"
                    }
                }
            }
        }
####  Create reciprocal requirements.
        if {([info exists reciprocal] || [dict exists $opt reciprocal]) && [dict exists $opt require]} {
            foreach other [dict get $opt require] {
                dict update def $other otherOpt {
                    dict lappend otherOpt require $name
                }
            }
        }
####  Perform shared key logic.
        if {[dict exists $opt key]} {
            dict for {otherName otherOpt} $def {
                if {$name ne $otherName && [dict exists $otherOpt key] && ([dict get $otherOpt key] eq\
                                                                                   [dict get $opt key])} {
#####   Limit when shared keys may be used.
                    if {[dict exists $opt parameter]} {
                        return -code error "$name cannot be a parameter because it shares a key with $otherName"
                    } elseif {[dict exists $opt argument]} {
                        return -code error "$name cannot use -argument because it shares a key with $otherName"
                    } elseif {[dict exists $opt catchall]} {
                        return -code error "$name cannot use -catchall because it shares a key with $otherName"
                    } elseif {[dict exists $opt default] && [dict exists $otherOpt default]} {
                        return -code error "$name and $otherName cannot both use -default because they share a key"
                    }
#####   Create forbid constraints on shared keys.
                    if {![dict exists $otherOpt forbid] || ($name ni [dict get $otherOpt forbid])} {
                        dict update def $otherName otherOpt {
                            dict lappend otherOpt forbid $name
                        }
                    }
#####   Set default -value for shared keys.
                    if {![dict exists $opt value]} {
                        dict set def $name value $name
                    }
                }
            }
        }
    }
### Handle default pass-through switch by creating a dummy element.
    if {[info exists pass]} {
        dict set def {} pass $pass
    }
### Force required parameters to bypass switch logic.
    set end [= {[llength $argv]-1}]
    if {![info exists mixed]} {
        foreach name $order {
            if {[dict exists $def $name required]} {
                incr end -1
            }
        }
    }
    set force [lreplace $argv 0 $end]
    set argv [lrange $argv 0 $end]
### Perform switch logic.
    set result {}
    set missing {}
    if {$switches ne {}} {
####  Build regular expression to match switches.
        set re ^-
        if {[info exists long]} {
            append re -?
        }
        append re {(\w[\w-]*)}
        if {[info exists equalarg]} {
            append re (?:(=)(.*))?
        } else {
            append re ()()
        }
        append re $
####  Process switches, and build the list of parameter arguments.
        set params {}
        while {$argv ne {}} {
#####   Check if this argument appears to be a switch.
            set argv [lassign $argv arg]
            if {[regexp $re $arg _ name equal val]} {
                # This appears to be a switch.  Fall through to the handler.
            } elseif {$arg eq {--}} {
                # If this is the special "--" switch to end all switches, all
                # remaining arguments are parameters.
                set params $argv
                break
            } elseif {[info exists mixed]} {
                # If -mixed is used and this is not a switch, it is a parameter.
                # Add it to the parameter list, then go to the next argument.
                lappend params $arg
                continue
            } else {
                # If this is not a switch, terminate switch processing, and
                # process this and all remaining arguments as parameters.
                set params [linsert $argv 0 $arg]
                break
            }
#####   Process switch aliases.
            if {[dict exists $aliases $name]} {
                set name [dict get $aliases $name]
            }
#####   Preliminary guess for the normalized switch name.
            set normal -$name
#####   Perform switch name lookup.
            if {[dict exists $def $name switch]} {
                # Exact match.  No additional lookup needed.
            } elseif {![info exists exact] &&\
                              ![catch {tcl::prefix match -message switch\
                                               [lmap {key data} $def {
                                                   if {[dict exists $data switch]} {
                                                       set key
                                                   } else {
                                                       continue
                                                   }
                                               }] $name
                              } name]} {
                # Use the switch whose prefix unambiguously matches.
                set normal -$name
            } elseif {[dict exists $def {}]} {
                # Use default pass-through if defined.
                set name {}
            } else {
                # Fail if this is an invalid switch.
                set switches [lsort $switches]
                if {[llength $switches]>1} {
                    lset switches end "or [@ $switches end]"
                }
                set switches [join $switches {*}[if {[llength $switches]>2} {list ", "}]]
                return -code error "bad switch \"$arg\": must be $switches"
            }
#####   If the switch is standalone, ignore all constraints.
            if {[dict exists $def $name standalone]} {
                foreach other [dict keys $def] {
                    dict unset def $other required
                    dict unset def $other require
                    dict unset def $other forbid
                    if {[dict exists $def $other parameter]} {
                        dict set def $other optional {}
                    }
                }
            }
#####   Keep track of which elements are present.
            dict set def $name present {}
            # If the switch value was set using -switch=value notation, insert
            # the value into the argument list to be handled below.
            if {$equal eq {=}} {
                set argv [linsert $argv 0 $val]
            }
#####   Load key and pass into local variables for easy access.
            unset -nocomplain key pass
            foreach var {key pass} {
                if {[dict exists $def $name $var]} {
                    set $var [dict get $def $name $var]
                }
            }
#####   Keep track of which switches have been seen.
            dict unset omitted $name
#####   Validate switch arguments and store values into the result dict.
            if {[dict exists $def $name catchall]} {
                # The switch is catchall, so store all remaining arguments.
                set argv [{*}$validateHelper $normal [dict get $def $name] {*}$argv]
                set argv [{*}$typeChecker $normal [dict get $def $name] {*}$argv]
                if {[info exists key]} {
                    dict set result $key $argv
                }
                if {[info exists pass]} {
                    if {[info exists normalize]} {
                        dict lappend result $pass $normal {*}$argv
                    } else {
                        dict lappend result $pass $arg {*}$argv
                    }
                }
                break
            } elseif {![dict exists $def $name argument]} {
                # The switch expects no arguments.
                if {$equal eq {=}} {
                    return -code error "$normal doesn't allow an argument"
                }
                if {[info exists key]} {
                    if {[dict exists $def $name value]} {
                        dict set result $key [dict get $def $name value]
                    } else {
                        dict set result $key {}
                    }
                }
                if {[info exists pass]} {
                    if {[info exists normalize]} {
                        dict lappend result $pass $normal
                    } else {
                        dict lappend result $pass $arg
                    }
                }
            } elseif {$argv ne {}} {
                # The switch was given the expected argument.
                set argv0 [@ [{*}$validateHelper $normal [dict get $def $name] [@ $argv 0]] 0]
                set argv0 [@ [{*}$typeChecker $normal [dict get $def $name] [@ $argv 0]] 0]
                if {[info exists key]} {
                    if {[dict exists $def $name optional]} {
                        dict set result $key [list {} $argv0]
                    } else {
                        dict set result $key $argv0
                    }
                }
                if {[info exists pass]} {
                    if {[info exists normalize]} {
                        dict lappend result $pass $normal $argv0
                    } elseif {$equal eq {=}} {
                        dict lappend result $pass $arg
                    } else {
                        dict lappend result $pass $arg [@ $argv 0]
                    }
                }
                set argv [lrange $argv 1 end]
            } else {
                # The switch was not given the expected argument.
                if {![dict exists $def $name optional]} {
                    return -code error "$normal requires an argument"
                }
                if {[info exists key]} {
                    dict set result $key {}
                }
                if {[info exists pass]} {
                    if {[info exists normalize]} {
                        dict lappend result $pass $normal
                    } else {
                        dict lappend result $pass $arg
                    }
                }
            }

            # Insert this switch's implied arguments into the argument list.
            if {[dict exists $def $name imply]} {
                set argv [concat [dict get $def $name imply] $argv]
                dict unset def $name imply
            }
        }
#####   Build list of missing required switches.
        dict for {name opt} $def {
            if {[dict exists $opt switch] && ![dict exists $opt present] && [dict exists $opt required]} {
                if {[dict exists $opt alias]} {
                    lappend missing -[dict get $opt alias]|$name
                } else {
                    lappend missing -$name
                }
            }
        }
#####   Fail if at least one required switch is missing.
        if {$missing ne {}} {
            set missing [lsort $missing]
            if {[llength $missing]>1} {
                lset missing end "and [@ $missing end]"
            }
            set missing [join $missing {*}[if {[llength $missing]>2} {list ", "}]]
            return -code error [string cat "missing required switch"\
                    {*}[if {[llength $missing]>1} {list es}] ": " $missing]
        }
    } else {
        # If no switches are defined, bypass the switch logic and process all
        # arguments using the parameter logic.
        set params $argv
    }
### Allocate one argument to each required parameter, including catchalls.
    set alloc {}
    lappend params {*}$force
    set count [llength $params]
    set i 0
    foreach name $order {
        if {[dict exists $def $name required]} {
            if {$count} {
                dict set alloc $name 1
                dict unset omitted $name
                incr count -1
            } else {
                lappend missing $name
            }
        }
        incr i
    }
### Fail if at least one required parameter is missing.
    if {$missing ne {}} {
        if {[llength $missing]>1} {
            lset missing end "and [@ $missing end]"
        }
        return -code error [string cat "missing required parameter" {*}[if {[llength $missing]>1} {list s}] ": "\
                                    [join $missing {*}[if {[llength $missing]>2} {list ", "}]]]
    }
### Try to allocate one argument to each optional, non-catchall parameter, until there are no arguments left.
    if {$count} {
        foreach name $order {
            if {![dict exists $def $name required] && ![dict exists $def $name catchall]} {
                dict set alloc $name 1
                dict unset omitted $name
                if {![incr count -1]} {
                    break
                }
            }
        }
    }
### Process excess arguments.
    if {$count} {
        if {[info exists catchall]} {
            # Allocate remaining arguments to the catchall parameter.
            dict incr alloc $catchall $count
            dict unset omitted $catchall
        } elseif {[dict exists $def {}]} {
            # If there is no catchall parameter, instead allocate to the default
            # pass-through result key.
            lappend order {}
            dict set alloc {} $count
        } else {
            return -code error {too many arguments}
        }
    }
### Check constraints.
    dict for {name opt} $def {
        if {[dict exists $opt present]} {
            foreach {match condition description} {1 require requires 0 forbid {conflicts with}} {
                if {[dict exists $opt $condition]} {
                    foreach otherName [dict get $opt $condition] {
                        if {[dict exists $def $otherName present]!=$match} {
                            foreach var {name otherName} {
                                if {[dict exists $def [set $var] switch]} {
                                    set $var -[set $var]
                                }
                            }
                            return -code error "$name $description $otherName"
                        }
                    }
                }
            }
        }
    }
### If normalization is enabled, explicitly store into the pass-through keys
    # all omitted switches that have a pass-through key, accept an argument, and
    # have a default value.
    if {[info exists normalize]} {
        dict for {name opt} $def {
            if {[dict exists $opt switch] && [dict exists $opt pass] && [dict exists $opt argument] &&\
                        [dict exists $opt default] && [dict exists $omitted $name]} {
                dict lappend result [dict get $opt pass] -$name [dict get $opt default]
            }
        }
    }
### Validate parameters and store in result dict.
    set i 0
    foreach name $order {
        set opt [dict get $def $name]
        if {[dict exists $alloc $name]} {
            if {![dict exists $opt catchall] && $name ne {}} {
                set val [@ [{*}$validateHelper $name $opt [@ $params $i]] 0]
                set val [@ [{*}$typeChecker $name $opt [@ $params $i]] 0]
                if {[dict exists $opt pass]} {
                    if {([string index $val 0] eq {-}) && ![dict exists $result [dict get $opt pass]]} {
                        dict lappend result [dict get $opt pass] --
                    }
                    dict lappend result [dict get $opt pass] $val
                }
                incr i
            } else {
                set step [dict get $alloc $name]
                set val [lrange $params $i [= {$i+$step-1}]]
                if {$name ne {}} {
                    set val [{*}$validateHelper $name $opt {*}$val]
                    set val [{*}$typeChecker $name $opt {*}$val]
                }
                if {[dict exists $opt pass]} {
                    if {([string index [@ $val 0] 0] eq {-}) && ![dict exists $result [dict get $opt pass]]} {
                        dict lappend result [dict get $opt pass] --
                    }
                    dict lappend result [dict get $opt pass] {*}$val
                }
                incr i $step
            }
            if {[dict exists $opt key]} {
                dict set result [dict get $opt key] $val
            }
        } elseif {[info exists normalize] && [dict exists $opt default] && [dict exists $opt pass]} {
            # If normalization is enabled and this omitted parameter has both a
            # default value and a pass-through key, explicitly store the default
            # value in the pass-through key, located in the correct position so
            # that it can be recognized again later.
            if {([string index [dict get $opt default] 0] eq {-}) && ![dict exists $result [dict get $opt pass]]} {
                dict lappend result [dict get $opt pass] --
            }
            dict lappend result [dict get $opt pass] [dict get $opt default]
        }
    }
### Create default values for missing elements.
    dict for {name opt} $def {
        if {[dict exists $opt key] && ![dict exists $result [dict get $opt key]]} {
            if {[dict exists $opt default]} {
                dict set result [dict get $opt key] [dict get $opt default]
            } elseif {[dict exists $opt catchall]} {
                dict set result [dict get $opt key] {}
            }
        }
        if {[dict exists $opt pass] && ![dict exists $result [dict get $opt pass]]} {
            dict set result [dict get $opt pass] {}
        }
    }
    if {[info exists inline]} {
####  Return result dict.
        return $result
    } else {
####  Unless -keep was used, unset caller variables for omitted elements.
        if {![info exists keep]} {
            dict for {name val} $omitted {
                set opt [dict get $def $name]
                if {![dict exists $opt keep] && [dict exists $opt key] && ![dict exists $result [dict get $opt key]]} {
                    uplevel 1 [list ::unset -nocomplain [dict get $opt key]]
                }
            }
        }
####  Process results.
        dict for {key val} $result {
            if {[dict exists $upvars $key]} {
                # If this element uses -upvar, link to the named variable.
                uplevel 1 [list ::upvar [dict get $def [dict get $upvars $key] level] $val $key]
            } else {
                # Store result into caller variables.
                uplevel 1 [list ::set $key $val]
            }
        }
    }
}
