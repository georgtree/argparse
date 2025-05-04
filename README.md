This is a fork of argparse project initially developed by Andy Goth <andrew.m.goth@gmail.com>.

See [original repository](https://core.tcl-lang.org/tcllib/timeline?r=amg-argparse), and 
[wiki page](https://wiki.tcl-lang.org/page/argparse).

## Table of content

  - [Table of content](#table-of-content)
  - [Synopsys](#synopsys)
  - [Description](#description)
  - [Quick Start](#quick-start)
  - [Concepts](#concepts)
    - [Argument](#argument)
    - [Definition](#definition)
    - [Global Switch](#global-switch)
    - [Element](#element)
    - [Element Switch](#element-switch)
    - [Name](#name)
    - [Key](#key)
    - [Switch](#switch)
    - [Parameter](#parameter)
    - [Value](#value)
    - [Alias](#alias)
    - [Shorthand flag](#shorthand-flag)
    - [Comment](#comment)
  - [Global Switches](#global-switches)
  - [Element Switches](#element-switches)
  - [Collecting unassigned arguments](#collecting-unassigned-arguments)
  - [Default values](#default-values)
  - [Forbid](#forbid)
  - [Reciprocal](#reciprocal)
  - [Imply](#imply)
  - [Validation](#validation)
  - [Passthrough and normalization](#passthrough-and-normalization)
  - [Upvar Elements](#upvar-elements)
  - [Argument Processing Sequence](#argument-processing-sequence)
  - [Return Value](#return-value)
  - [Author(s)](#author(s))


## Synopsys

``` text
package require Tcl 8.6
argparse ?-globalSwitch ...? ?--? definition ?arguments?
```

## Description

The `argparse` package provides a powerful argument parser command named `argparse` capable of flexibly processing and
validating many varieties of switches and parameters.

Tcl commands requiring more advanced argument parsing than provided by the standard `proc` command can be declared to
accept `args` (i.e. any number of arguments), then can call `argparse` to perform the real argument parsing.

In addition to Tcl command argument parsing, `argparse` is suitable for command line argument parsing, operating on the
value of the `::argv` global variable.

`argparse` may be applied to a variety of special purposes beyond standard argument parsing. For example, `argparse` can
parse custom variadic data structures formatted as lists of switches and/or parameters of a highly dynamic nature.
Another example: by calling `argparse` multiple times, it is possible to parse nested or multi-part argument lists in
which arguments to subsystems are embedded in passthrough switches or parameters.

## Quick Start

The `argparse` command may have many complex features, but it is not necessary to understand it in depth before using it
for the most common tasks. Its syntax is reasonably intuitive, so the best thing to do is see it in action before
reading the precise details on how it works.

Consider the following:

```tcl
package require argparse
proc greet {args} {
    argparse {
        {-salutation= -default hello}
        -modifier=
        -title
        subject
    }
    set msg $salutation
    if {[info exists title]} {
        set msg [string totitle $msg]
    }
    if {[info exists modifier]} {
        append msg ", " $modifier
    }
    append msg " " $subject
}
```

This example demonstrates many of the argparse core concepts and features. The greet command is defined to accept args.
When not explicitly given an argument list to parse, argparse parses the value of the args variable and stores the
results into local variables having the same [names](#name) as the various [elements](#element) of the
[definition](#definition).

Here, the definition is a list of four elements, named `salutation`, `modifier`, `title`, and `subject`. Because their
names are prefixed with `-`, `salutation`, `modifier`, and `title` are [switches](#switch), whereas `subject`, lacking a
`-` prefix, is a [parameter](#parameter). Two of the switches are given a `=` suffix, which means they each take an
[argument](#argument), whereas `title` does not. In addition to these [flag](#flag) characters, the `salutation` element
is surrounded with braces because it contains more list words used to further customize how it is handled. Namely, it
uses the `-default` [element switch](#element-switches) to set its default value to `hello`, in case `-salutation` is
not present in the argument list.

By default, switches are [optional](#optional) and parameters are [required](#required). `salutation`, `modifier`, and
`title`, being switches, are all optional, but since `salutation` has a default value, its variable's existence is
therefore guaranteed. Likewise, `subject`, being a parameter, is required, and its variable's existence is also
guaranteed. On the contrary, because `modifier` and `title` are optional and have no default value, it is necessary to
use `info exists` to confirm their variables' existence before attempting to read them. Because `title` does not accept
an argument, its variable's value (if the variable exists at all) is predefined to be empty string.

```tcl
greet world
```
```text
==> hello world
```

The first time `greet` is called, it is given only one argument, which is bound to the `subject` parameter. Normally,
switch arguments appear to the left of parameter arguments, and parameter arguments are bound first. Thus, the final
argument to `greet` is always bound to the `subject` parameter, even if it happens to resemble a switch, and is
therefore stored in the `subject` variable. Because their associated switches do not appear in the argument list, the
`salutation` variable is set to its default value (`hello`), and the `modifier` and `title` variables are unset due to
lacking a default.

```tcl
greet -salutation howdy world
```
```text
==> howdy world
```

The second time `greet` is called, it is given three arguments. As discussed above, the final argument (`world`) is
immediately stored into the `subject` variable before any switch processing occurs. Next, the remaining two arguments
are examined and determined to be the name and value of the `salutation` switch. Thus, the second argument (`howdy`) is
stored in the `salutation` variable. The `modifier` and `title` variables are unset.

```tcl
greet -title -mod "my dear" world
```
```text
==> Hello, my dear world
```

The third time `greet` is called, it is given four arguments. The first is `-title`, causing the title variable to be
set to empty string. The second is the name of the `modifier` switch. More precisely, it is an unambiguous prefix
thereof, i.e. an abbreviation. This causes the third argument (`my dear`) to be stored in the `modifier` variable, and
the final argument (`world`) is stored in the `subject` variable. As for the `salutation` variable, it is set to its
default (`hello`).

```tcl
greet -title
```
```text
==> hello -title
```

The fourth time `greet` is called, it is given one argument. Because the final argument is always bound to the `subject`
parameter, `subject` is set to `-title` even though there happens to be a switch with the same name. There are no
remaining arguments, so the switches are all handled according to defaults, just like in the first call to `greet`.

## Concepts

This section lists and explains the concepts and terminology employed by the `argparse` package. The concepts are
numerous and interrelated, so it may be necessary to read this section multiple times and to refer back to it while
reading the remainder of this document.

### Argument

The actual values passed to the `argparse` command to be parsed are known as arguments.

### Definition

The *definition* determines how the `argparse` command parses its arguments. Definitions are Tcl lists of any length,
each word of which is an [element](#element). Each definition contains a unique, non-empty name element consisting of
alphanumerics, underscores, and minus (not as the first character), then zero or more of the [element
switches](#element-switch)

The following example definition may conceivably be used by a command that stores a sequence of numbers into a variable.

``` tcl
{
    # {Optional sequence control switches}
    {-from= -default 1}
    {-to=   -default 10}
    {-step= -default 1}
    # {Required output list variable}
    listVar^
}
```

The example of such procedure is [source](https://stackoverflow.com/a/38436286/21306711):
```tcl
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
genNums -from 0 -to 10 -step 2 sequenceVar
puts $sequenceVar
```
```text
0.0 2.0 4.0 6.0 8.0
```

In this example procedure definition `{-prec= -default 1}` is added and defines precision of resulted sequence of
numbers. Instead of returning value via `[return]` command we save resulted list directly in variable of caller scope
`sequenceVar`.

### Global Switch

A *global switch* configures the overall operation of the `argparse` command. Global switches are optional initial
arguments to `argparse` and may only appear before the [definition](#definition) argument. Global switches may also be
embedded within special definition [elements](#element) whose [name](#name) is empty string.

### Element

A [definition](#definition) contains any number of *elements*. For the most part, each element defines either a
[switch](#switch) or a [parameter](#parameter). Elements may also be [comments](#comment) or [global
switches](#global-switches). An element is itself a Tcl list whose required first word is the name (with optional
shorthand aliases and flags) and whose optional subsequent words are element switches.

In addition to [switches](#switch) and [parameters](#parameter), elements may be [comments](#comment) or lists of 
[global switches](#global-switches).

The example definition shown above contains one parameter element, three switch elements, two comment elements, and no 
global switches. All switch and parameter elements in the example make use of shorthand flags.

One element from the above example is repeated here so it may be examined more closely.

``` tcl
{-from= -default 1}
```

The name of this element is from. It uses the `-` and `=` flags, as well as the `-default` element switch with argument 
`1`. The specific meaning of flags and element switches are described elsewhere in this document.

### Element Switch

An *element switch* configures a single [element](#element) in the [definition](#definition). Element switches are 
listed following the [name](#name) word of the definition element.

In the above example definition, each [switch](#switch) element explicitly uses the `-default` element switch. Due to 
use of the `-` and `=` shorthand [flags](#flag), each switch element also implicitly uses the `-switch` and `-argument`
element switches.

### Name

Aside from [aliases](#alias) and [flags](#flag), the first word of each [switch](#switch) or [parameter](#parameter) 
[element](#element) in the [definition](#definition) is the *name*.

Switch and parameter element names may not be used more than once within a definition.

If the name is `#`, the element is a [comment](#comment) and is ignored. If the name is empty string, the element is 
neither a switch nor a parameter and is instead a list of [global switches](#global-switches).

### Key

In addition to having a [name](#name), every [switch](#switch) and [parameter](#parameter) [definition](#definition) 
[element](#element) has a key. Unlike names, multiple elements may share the same key, subject to restrictions.

By default, the key defaults to the name, but it is possible to use the `-key` [element switch](#element-switch) to 
explicitly declare an element's key.

Keys determine the variable names or dict keys `argparse` uses to return the argument value of the switch or parameter.

Multiple elements may share the same `-key` value if they are switches, do not use `-argument` or `-catchall`, and do
not use `-default` for more than one element. Such elements are automatically are given `-forbid` constraints to prevent
them from being used simultaneously.  If such an element does not use `-boolean` or `-value`, the element name is used
as its default `-value`.

### Switch

Switch is the optional element, that could be made mandatory with `-required` [element switch](#element-switch). 
It could be defined either with shorthand `-` before name or with `-switch` [element switch](#element-switch).

Unambiguous prefixes of switch names are acceptable, unless the `-exact` switch is used. Switches in the argument list
normally begin with a single `-` but can also begin with `--` if the `-long` switch is used. Arguments to switches
normally appear as the list element following the switch, but if `-equalarg` is used, they may be supplied within the
switch element itself, delimited with an `=` character, e.g. `-switch=arg`.

When `-switch` and `-optional` are both used, `-catchall`, `-default`, and `-upvar` are disallowed.

### Parameter

Parameter is the mandatory [element](#element) that is provided without any preceding flags before name, and it's
position in the list of [definitions](#definition) is the same as it must be passed to `argparse` procedure.

In definition list it is also could be explicitly stated with `-parameter` [element switch](#element-switch). Parameters 
can be made optional with `-optional`. `-catchall` also makes parameters optional, unless `-required` is used, in which 
case at least one argument must be assigned to the parameter. Otherwise, using `-required` with `-parameter` has no 
effect.

### Value

Value is the argument that is passed into variable (or dictionary value) for particular parameter or switch if it is 
defined with `-argument` or shorthand `=` after switch name. `-optional`, `-required`, `-catchall`, and `-upvar` imply 
`-argument` when used with `-switch`.  Consequently, switches require an argument when any of the shorthand flag 
characters defined above are used, and it is not necessary to explicitly specify `=` if any of the other flag characters
are used.

If `-argument` is used, the value assigned to the switch's key is normally the next argument following the switch.  With
`-catchall`, the value assigned to the switch's key is instead the list of all remaining arguments. With `-optional`,
the following processing is applied:

 - If the switch is not present, the switch's key is omitted from the result.
 - If the switch is not the final argument, its value is a two-element list
  consisting of empty string and the argument following the switch.
 - If the switch is the final argument, its value is empty string.

We can also explicitly specify particular value which is assigned to switch key whet switch is presented is present with
`-value` after switch name in definition.

`-value` may not be used with `-argument`, `-optional`, `-required`, or `-catchall`.  `-value` normally defaults to 
empty string, except when `-boolean` is used (1 is the default `-value`) or multiple switches share the same `-key` (the
element name is the default `-value`).

### Alias

Alias is the alternative name of the switch element. It can be provided by `-alias ALIAS` element switch, or
before the base name with `|`  shorthand flag, but after `-` flag. For example:

``` tcl
{-variable= -alias var -required}
```

or

``` tcl
{-var|variable= -required}
```

### Shorthand flag

If neither `-switch` nor `-parameter` are used, a shorthand flag form is permitted. If the name is preceded by `-`, it 
is a switch; otherwise, it is a parameter. An alias may be written after `-`, then followed by `|` and the switch name.
The element name may be followed by any number of flag characters:

| Shorthand flag | Description                                  |
|:---------------|:---------------------------------------------|
| `=`            | Same as `-argument`; only valid for switches |
| `?`            | Same as `-optional`                          |
| `!`            | Same as `-required`                          |
| `*`            | Same as `-catchall`                          |
| `^`            | Same as `-upvar`                             |


### Comment

Comment is the element started with `#` at the start of definition in the definition list.


## Global Switches

| Switch                     | Description                                                      |
|:---------------------------|:-----------------------------------------------------------------|
| `-inline`                  | Return the result dict rather than setting caller variables      |
| `-exact`                   | Require exact switch name matches, and do not accept prefixes    |
| `-mixed`                   | Allow switches to appear after parameters                        |
| `-long`                    | Recognize `--switch` long option alternative syntax              |
| `-equalarg`                | Recognize `-switch=arg` inline argument alternative syntax       |
| `-normalize`               | Normalize switch syntax in passthrough result keys               |
| `-reciprocal`              | Every element's `-require` constraints are reciprocal            |
| `-level levelSpec`         | Every `-upvar` element's `[upvar]` level; defaults to 1          |
| `-template templateString` | Transform default element names using a substitution template    |
| `-pass passKey`            | Pass unrecognized elements through to a result key               |
| `-keep`                    | Do not unset omitted element variables; conflicts with `-inline` |
| `-boolean`                 | Treat switches as having `-boolean` wherever possible            |
| `-validate validDef`       | Define named validation expressions to be used by elements       |
| `-enum enumDef`            | Define named enumeration lists to be used by elements            |
| `-`                        | Force next argument to be interpreted as the definition list     |

## Element Switches

| Switch                     | Description                                                                           |
|:---------------------------|:--------------------------------------------------------------------------------------|
| `-switch`                  | Element is a switch; conflicts with `-parameter`                                      |
| `-parameter`               | Element is a parameter; conflicts with `-switch`                                      |
| `-alias aliasName`         | Alias name; requires `-switch`                                                        |
| `-ignore`                  | Element is omitted from result; conflicts with `-key` and `-pass`                     |
| `-key keyName`             | Override key name; not affected by `-template`                                        |
| `-pass keyName`            | Pass through to result key; not affected by `-template`                               |
| `-default value`           | Value if omitted; conflicts with `-required` and `-keep`                              |
| `-keep`                    | Do not unset if omitted; requires `-optional`; conflicts `-inline`                    |
| `-value value`             | Value if present; requires `-switch`; conflicts with `-argument`                      |
| `-boolean`                 | Equivalent to `-default 0 -value 1`                                                   |
| `-argument`                | Value is next argument following switch; requires `-switch`                           |
| `-optional`                | Switch value is optional, or parameter is optional                                    |
| `-required`                | Switch is required, or stop `-catchall` from implying `-optional`                     |
| `-catchall`                | Value is list of all otherwise unassigned arguments                                   |
| `-upvar`                   | Links caller variable; conflicts with `-inline` and `-catchall`                       |
| `-level levelSpec`         | This element's `[upvar]` level; requires `-upvar`                                     |
| `-standalone`              | If element is present, ignore `-required`, `-require`, and `-forbid`                  |
| `-require nameList`        | If element is present, other elements that must be present                            |
| `-forbid nameList`         | If element is present, other elements that must not be present                        |
| `-imply argList`           | If element is present, extra switch arguments; requires `-switch`                     |
| `-reciprocal`              | This element's `-require` is reciprocal; requires `-require`                          |
| `-validate validNameOrDef` | Name of validation expression, or inline validation definition                        |
| `-enum enumNameOrDef`      | Name of enumeration list, or inline enumeration definition                            |
| `-type typeName`           | Validate value according to type defined in [string is] command, requires `-argument` |

## Collecting unassigned arguments

With `-catchall` element switch, we can collects the rest of otherwise unassigned arguments to list and save to 
switch's key. Let's consider next example:

```tcl
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

applyOperator -op * -elements 1 2 3 4 5
```
```text
==> 120
```

This procedure make summation or product of all elements passed after `-elements` switch with `-catchall` element
switch. There is also an example of using `-enum` switch to [validate](#validation) possible values of argument to `-op`
switch.

At most one parameter may use `-catchall`.

## Default values

Element switch `-default` specifies the value to assign to element keys when the element is omitted. If `-default` is not
used, keys for omitted switches and parameters are omitted from the result, unless `-catchall` is used, in which case the
default value for -default is empty string.

## Validation

`-validate` and `-enum` provide [element](#element) [value](#value) validation.  The overall `-validate` and `-enum`
switches declare named validation expressions and enumeration lists, and the per-element `-validate` and `-enum`
switches select which validation expressions and enumeration lists are used on which elements.  The argument to the
overall `-validate` and `-enum` switches is a dict mapping from validation or enumeration name to validation expressions
or enumeration lists. The argument to a per-element `-validate` switch is a validation name or expression, and the
argument to a per-element `-enum` switch is an enumeration name or list.  An element may not use both `-validate` and
`-enum`.

A validation expression is an `[expr]` expression parameterized on a variable named arg which is replaced with the 
argument.  If the expression evaluates to `true`, the argument is accepted. Let's consider the example:

```tcl
proc exponentiation {args} {
    argparse {
        {-b!= -validate {[string is double $arg]}}
        {-n!= -validate {[string is double $arg]}}
    }
    return [expr {$b**$n}]
}

exponentiation -b 2 -n 4
```
```text
==> 16
```

This simple function return exponentiation of number `b` in `n`. Our simple tests verify that the arguments are strings
that could be considered as floating numbers, and only after that set the corresponding variables.

If wrong string is provided, a error message appears:

```tcl
exponentiation -b a -n 4
```
```{tclerr}
-b value "a" fails validation: [string is double $arg]
```


An enumeration list is a list of possible argument values.  If the argument appears in the enumeration list, the 
argument is accepted.  Unless `-exact` is used, if the argument is a prefix of exactly one element of the enumeration 
list, the argument is replaced with the enumeration list element.

Additional built-in validation of argument is done with help of [string is] command. All types (classes) are supported
except `true` and `false` class. To declare type of argument, the switch `-type` should be provided to definition
of switch or parameter with one of the argument: `alnum alpha ascii boolean control dict digit double graph integer list
lower print punct space upper wideinteger wordchar xdigit`. If you specify type for switch, the switch should be defined
with `-argument` (or `=` shorthand). Conflicts with `-upvar`, `-boolean` and `-enum`, combination of `-switch`, 
`-optional` and `type` is forbidden.

As an example, we can use previously proposed procedure:
```tcl
proc genNums {args} {
    argparse {
        # {Optional sequence control switches}
        {-from= -default 1 -type double}
        {-to=   -default 10 -type double}
        {-step= -default 1 -type double}
        {-prec= -default 1 -type double}
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
```

If we provide the wrong argument type to `genNums` procedure, the error occurs:

```tcl
genNums -from 0 -to 10.. -step 2 sequenceVar
```
```{tclerr}
-to value "10.." is not of the type double
```


## Forbid

If the presence of an element should be forbidden in combination with certain other elements, use the `-forbid` switch
followed by a list of element names. In next example, we can forbid using arguments that logically incompatible:
```tcl
proc sheduleEvent {args} {
    set arguments [argparse -inline\
            -validate [dict create date {[regexp {^(0[1-9]|[12][0-9]|3[01])-(0[1-9]|1[0-2])-\d{4}$} $arg]}\
                time {[regexp {^([01][0-9]|2[0-3]):[0-5][0-9](?::[0-5][0-9])?$} $arg]}] {
        {date -validate date}
        {time -validate time}
        {-allday -forbid {duration endtime}}
        {-duration= -forbid {allday endtime} -validate time}
        {-endtime= -forbid {allday duration} -validate time}
    }]
    if {![dict exists $arguments allday] && ![dict exists $arguments duration] && ![dict exists $arguments endtime]} {
        return -code error "One of the switch must be presented: -allday, -duration or -endtime"
    }
    return $arguments
}
```

This procedure provides template for sheduling event. As input, date in format DD-MM-YYYY and time in format HH:MM of 
the event provides as parameters, and the duration of the event could be provided in different mutually exclusively ways:

| switch name | description                                        |
|-------------|----------------------------------------------------|
| -allday     | select the duration of event to the end of the day |
| -duration   | set duration of the event in format HH:MM          |
| -endtime    | set end time of the event in format HH:MM          |

Additionaly, the validation expressions for the date format and time format regular expression is used. These expression
are supplied as  named expressions in dictionary argument to global switch `-validate`.

One of the switches `-allday`, `-duration` or `-endtime` must be presented, so additional checking is done after 
argument processing.

Normal operation:
```tcl
sheduleEvent -duration 01:30 20-12-2024 13:30
```
```text
==> duration 01:30 date 20-12-2024 time 13:30
```
Error issuing in case of providing conflicting switches:
```tcl
sheduleEvent -allday -duration 01:30 20-12-2024 13:30
```
```{tclerr}
-allday conflicts with -duration
```


## Reciprocal

Swtich with name `-reciprocal` is presented both as global switch and element switch. In global case all elements
with `-require` switch are reciprocal to names in argument to `-require` switch. Let's consider this definition:
```tcl
argparse -reciprocal {
    {-a= -require {b c}}
    -b
    -c=
}
```

With `-reciprocal` global switch, the `-b` and `-c` switches also require `-a` switch, because `-a` switch requires
them, so, it is equivalent to:
```tcl
argparse {
    {-a= -require {b c}}
    {-b -require a}
    {-c= -require a}
}
```

Element switch `-reciprocal` do the same thing but only for that particular element.

## Imply

`-imply` element switch allows to specify additional argument to this switch. Extra argument will be assigned
to different element specified in argument to `-imply` switch. For example:

```tcl
proc implyTest {args} {
    set arguments [argparse -inline {
        -a=
        {-b= -imply -c}
        -c=
    }]
    return $arguments
}
```

If we provide an additional argument to `-b`, it will be assigned to `-c` key:
```tcl
implyTest -a 1 -b 2 3
```
```text
==> a 1 b 2 c 3
```

If additional argument is not provided, the error is raised:
```tcl
implyTest -a 1 -b 2
```
```{tclerr}
-c requires an argument
```


And if `-b` switch with argument is not provided at all, `-c` is also considered omitted:
```tcl
implyTest -a 1
```
```text
==> a 1
```

## Passthrough and normalization

The per-element `-pass` switch causes the element argument or arguments to be appended to the value of the indicated
pass-through result key. Many elements may use the same pass-through key. If `-normalize` is used, switch arguments are
normalized to not use aliases, abbreviations, the `--` prefix, or the `=` argument delimiter; otherwise, switches will
be expressed the same way they appear in the original input. Furthermore, `-normalize` causes omitted switches that
accept arguments and have default values, as well as omitted parameters that have default values, to be explicitly
included in the pass-through key. If `-mixed` is used, pass-through keys will list all switches first before listing any
parameters. If the first parameter value for a pass-through key starts with `-`, its value will be preceded by `--` so
that it will not appear to be a switch. If no arguments are assigned to a pass-through key, its value will be empty
string. The intention is that the value of a pass-through key can be parsed again to get the original data, and if
`-normalize` is used, it will not be necessary to use `-mixed`, `-long`, `-equalarg`, `-alias`, or `-default` to get the
correct result. However, pathological use of `-default` can conflict with this goal. For example, if the first optional
parameter has no `-default` but the second one does, then parsing the result of `-normalize` can assign the default
value to the first parameter rather than the second.

The `[argparse]` `-pass` switch may be used to collect unrecognized arguments into a pass-through key, rather than
failing with an error. Normalization and unmixing will not be applied to these arguments because it is not possible to
reliably determine if they are switches or parameters. In particular, it is not known if an undefined switch expects an
argument.

## Upvar Elements

Elements with `-upvar` are special. Rather than having normal values, they are bound to caller variables using the
`[upvar]` command. `-upvar` conflicts with `-inline` because it is not possible to map a dict value to a variable. Due
to limitations of arrays and `[upvar]`, `-upvar` cannot be used with keys whose names resemble array elements. `-upvar`
conflicts with `-catchall` because the value must be a variable name, not a list. The combination `-switch -optional
-upvar` is disallowed for the same reason. If `-upvar` is used with switches or with optional parameters, `[info exists
KEY]` returns 1 both when the element is not present and when its value is the name of a nonexistent variable. To tell
the difference, check if `[info vars KEY]` returns an empty list; if so, the element is not present. Note that the
argument to `[info vars]` is a `[string match]` pattern, so it may be necessary to precede `*?[]\` characters with
backslashes.

## Argument Processing Sequence

Argument processing is performed in three stages: switch processing, parameter allocation, and parameter
assignment. Each argument processing stage and pass is performed left-to-right.

All switches must normally appear in the argument list before any parameters. Switch processing terminates with the
first argument (besides arguments to switches) that does not start with `-` (or `--`, if `-long` is used). The special
switch `--` can be used to force switch termination if the first parameter happens to start with `-`. If no switches are
defined, the first argument is known to be a parameter even if it starts with `-`.

When the `-mixed` switch is used, switch processing continues after encountering arguments that do not start with `-` or
`--`. This is convenient but may be ambiguous in cases where parameters look like switches.  To resolve ambiguity, the
special `--` switch terminates switch processing and forces all remaining arguments to be parameters.

When `-mixed` is not used, the required parameters are counted, then that number of arguments at the end of the argument
list are treated as parameters even if they begin with `-`. This avoids the need for `--` in many cases.

After switch processing, parameter allocation determines how many arguments to assign to each parameter.  Arguments
assigned to switches are not used in parameter processing. First, arguments are allocated to required parameters;
second, to optional, non-catchall parameters; and last to catchall parameters. Finally, each parameter is assigned the
allocated number of arguments.

## Return Value

`[argparse]` produces a set of keys and values. The keys are the names of caller variables into which the values are
stored, unless `-inline` is used, in which case the key-value pairs are returned as a dict.  The element names default
to the key names, unless overridden by `-key`, `-pass`, or `-template`. If both `-key` and `-pass` are used, two keys
are defined: one having the element value, the other having the pass-through elements. Unless `-keep` or `-inline` are
used, the caller variables for omitted switches and parameters are unset.

## Author(s)

Andy Goth <andrew.m.goth@gmail.com> - all code and most of documentation

George Yashin <georgtree@gmail.com> - some documentation and test suite
