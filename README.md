This is a fork of argparse project initially developed by Andy Goth <andrew.m.goth@gmail.com>.

See [original repository](https://core.tcl-lang.org/tcllib/timeline?r=amg-argparse), and 
[wiki page](https://wiki.tcl-lang.org/page/argparse).

## Table of content

  - [Synopsys](#synopsys)
  - [Description](#description)
  - [Quick Start](#quick-start)
  - [Concepts](#concepts)
    - [Definition](#definition)
    - [Element](#element)
    - [Name](#name)
    - [Key](#key)
    - [Alias](#alias)
    - [Flag](#flag)
    - [Global Switch](#global-switch)
    - [Element Switch](#element-switch)
    - [Switch](#switch)
    - [Parameter](#parameter)
    - [Comment](#comment)
    - [Argument](#argument)
    - [Required](#required)
    - [Optional](#optional)
  - [Global Switches](#global-switches)
  - [Element Switches](#element-switches)
  - [Shorthand](#shorthand)
    - [Aliases](#aliases)
    - [Flags](#flags)
  - [Validation](#validation)
  - [Passthrough](#passthrough)
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

The `argparse` package provides a powerful argument parser command named `argparse` capable of flexibly processing
and validating many varieties of switches and parameters.

Tcl commands requiring more advanced argument parsing than provided by the standard `proc` command can be declared to 
accept `args` (i.e. any number of arguments), then can call `argparse` to perform the real argument parsing.

In addition to Tcl command argument parsing, `argparse` is suitable for command line argument parsing, operating on
the value of the `::argv` global variable.

`argparse` may be applied to a variety of special purposes beyond standard argument parsing. For example, `argparse`
can parse custom variadic data structures formatted as lists of switches and/or parameters of a highly dynamic nature. 
Another example: by calling `argparse` multiple times, it is possible to parse nested or multi-part argument lists in 
which arguments to subsystems are embedded in passthrough switches or parameters.

## Quick Start

The `argparse` command may have many complex features, but it is not necessary to understand it in depth before using
it for the most common tasks. Its syntax is reasonably intuitive, so the best thing to do is see it in action before 
reading the precise details on how it works.

Consider the following:

``` tcl
% proc greet {args} {
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
% greet world
hello world
% greet -salutation howdy world
howdy world
% greet -title -mod "my dear" world
Hello, my dear world
% greet -title
hello -title
```

This example demonstrates many of the argparse core concepts and features. The greet command is defined to accept args. 
When not explicitly given an argument list to parse, argparse parses the value of the args variable and stores the 
results into local variables having the same [names](#name) as the various [elements](#element) of the 
[definition](#definition).

Here, the definition is a list of four elements, named `salutation`, `modifier`, `title`, and `subject`. Because their 
names are prefixed with `-`, `salutation`, `modifier`, and `title` are [switches](#switch), whereas `subject`, lacking
a `-` prefix, is a [parameter](#parameter). Two of the switches are given a `=` suffix, which means they each take an 
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

``` tcl
% greet world
hello world
```

The first time `greet` is called, it is given only one argument, which is bound to the `subject` parameter. Normally, 
switch arguments appear to the left of parameter arguments, and parameter arguments are bound first. Thus, the final 
argument to `greet` is always bound to the `subject` parameter, even if it happens to resemble a switch, and is 
therefore stored in the `subject` variable. Because their associated switches do not appear in the argument list, the 
`salutation` variable is set to its default value (`hello`), and the `modifier` and `title` variables are unset due to 
lacking a default.

``` tcl
% greet -salutation howdy world
howdy world
```

The second time `greet` is called, it is given three arguments. As discussed above, the final argument (`world`) is 
immediately stored into the `subject` variable before any switch processing occurs. Next, the remaining two arguments 
are examined and determined to be the name and value of the `salutation` switch. Thus, the second argument (`howdy`) is 
stored in the `salutation` variable. The `modifier` and `title` variables are unset.

``` tcl
% greet -title -mod "my dear" world
Hello, my dear world
```

The third time `greet` is called, it is given four arguments. The first is `-title`, causing the title variable to be 
set to empty string. The second is the name of the `modifier` switch. More precisely, it is an unambiguous prefix 
thereof, i.e. an abbreviation. This causes the third argument (`my dear`) to be stored in the `modifier` variable, and 
the final argument (`world`) is stored in the `subject` variable. As for the `salutation` variable, it is set to its 
default (`hello`).

``` tcl
% greet -title
hello -title
```

The fourth time `greet` is called, it is given one argument. Because the final argument is always bound to the `subject` 
parameter, `subject` is set to `-title` even though there happens to be a switch with the same name. There are no 
remaining arguments, so the switches are all handled according to defaults, just like in the first call to `greet`.

## Concepts

This section lists and explains the concepts and terminology employed by the `argparse` package. The concepts are 
numerous and interrelated, so it may be necessary to read this section multiple times and to refer back to it while 
reading the remainder of this document.

### Definition

The *definition* determines how the `argparse` command parses its arguments. Definitions are Tcl lists of any length, 
each word of which is an [element](#element).

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

### Element

A [definition](#definition) contains any number of *elements*. For the most part, each element defines either a 
[switch](#switch) or a [parameter](#parameter). Elements may also be [comments](#comment) or 
[global switches](#global-switches). An element is itself a Tcl list whose required first word is the name (with optional 
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

### Alias

### Flag

### Global Switch

A *global switch* configures the overall operation of the `argparse` command. Global switches are optional initial 
arguments to `argparse` and may only appear before the [definition](#definition) argument. Global switches may also be 
embedded within special definition [elements](#element) whose [name](#name) is empty string.

### Element Switch

An *element switch* configures a single [element](#element) in the [definition](#definition). Element switches are 
listed following the the [name](#name) word of the definition element.

In the above example definition, each [switch](#switch) element explicitly uses the `-default` element switch. Due to 
use of the `-` and `=` shorthand [flags](#flag), each switch element also implicitly uses the `-switch` and `-argument`
element switches.

### Switch

### Parameter

### Comment

### Argument

The actual values passed to the `argparse` command to be parsed are known as arguments.

### Required

### Optional


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

| Switch                     | Description                                                          |
|:---------------------------|:---------------------------------------------------------------------|
| `-switch`                  | Element is a switch; conflicts with `-parameter`                     |
| `-parameter`               | Element is a parameter; conflicts with `-switch`                     |
| `-alias aliasName`         | Alias name; requires `-switch`                                       |
| `-ignore`                  | Element is omitted from result; conflicts with `-key` and `-pass`    |
| `-key keyName`             | Override key name; not affected by `-template`                       |
| `-pass keyName`            | Pass through to result key; not affected by `-template`              |
| `-default value`           | Value if omitted; conflicts with `-required` and `-keep`             |
| `-keep`                    | Do not unset if omitted; requires `-optional`; conflicts `-inline`   |
| `-value value`             | Value if present; requires `-switch`; conflicts with `-argument`     |
| `-boolean`                 | Equivalent to `-default 0 -value 1`                                  |
| `-argument`                | Value is next argument following switch; requires `-switch`          |
| `-optional`                | Switch value is optional, or parameter is optional                   |
| `-required`                | Switch is required, or stop `-catchall` from implying `-optional`    |
| `-catchall`                | Value is list of all otherwise unassigned arguments                  |
| `-upvar`                   | Links caller variable; conflicts with `-inline` and `-catchall`      |
| `-level levelSpec`         | This element's `[upvar]` level; requires `-upvar`                    |
| `-standalone`              | If element is present, ignore `-required`, `-require`, and `-forbid` |
| `-require nameList`        | If element is present, other elements that must be present           |
| `-forbid nameList`         | If element is present, other elements that must not be present       |
| `-imply argList`           | If element is present, extra switch arguments; requires `-switch`    |
| `-reciprocal`              | This element's `-require` is reciprocal; requires `-require`         |
| `-validate validNameOrDef` | Name of validation expression, or inline validation definition       |
| `-enum enumNameOrDef`      | Name of enumeration list, or inline enumeration definition           |


## Shorthand

### Aliases

### Flags

## Validation

## Passthrough

## Upvar Elements

## Argument Processing Sequence

## Return Value

## Author(s)

Andy Goth <andrew.m.goth@gmail.com> - all code and most part of documentation

George Yashin <georgtree@gmail.com> - some documentation and test suite
