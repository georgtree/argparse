TODO list by Andy Goth

- Add switch to put parameters before switches
  - Some Tcl commands work this way
  - Examples: [chan copy], [zlib gunzip], [clock], [registry broadcast]
- Remove `-boolean` 
  - Instead, act as if `-boolean` were always being passed to [argparse]
  - Remove `-keep` as well
  - Add an `-unset` switch to selectively change the behavior to unsetting a
    variable or omitting a dict key when the switch is not present
- Remove `-switch` and `-parameter`
  - Instead make `-` "shorthand" mandatory when declaring a switch
- Change from a result dict to a result array
  - Makes more complex validation expressions possible
  - If `-inline` is used, use [array get] to get the return value
- Allow validation expressions to see the result array, not just the switch or
  parameter being validated
  - Use [upvar] rather than [array get] and [array set] to expose the result
    array into the validation expression
  - Also use [upvar] to link the arg variable to the current value
  - Validation expressions can then be (ab)used to modify the results
  - Necessary to do validation expressions as a separate final pass
  - Still do enumeration immediately so their final result values will be
    reflected in the validation
- Allow enumerations to have validation expressions
  - No real reason why these two have to conflict
- Documentation
  - Instead of having a big comment, use doctools
- Help text generation
  - Parameter and switch descriptions given by per-element `-help` switch
  - Need overall `-help` switch to provide narrative description
  - May need special grouping elements to organize switches and parameters
  - Text formatting and word wrapping
- Automatic command usage error message
  - Takes the place of Tcl\_WrongNumArgs()
- Single-character switch clustering
  - Requires use of `-long` for switches with multi-character names
- Reimplement in C for performance reasons
  - No bytecoding needed
  - Continue to maintain Tcl version for reference and portability
  - Provide stubs-enabled C API
  - One function for parsing definition list
  - One function for parsing argument list against preparsed definition list
  - Custom Tcl\_Obj type for preparsed definition list
- Test suite
  - Possibly the most daunting part of the project

TODO list by George Yashin:

- More than one alias for switch, and global option to allow use alias name as a key
- Specify only allowed switches/parameters, not forbidden, e.g. whitelists
- Ability for validation expression to use others keys arguments
- Add -required option when use the same key with multiple names of switches, so particular must be presented.
- If only one switch presented, and the rest is pass to -pass key, if this switchh is provided not as the first element,
it can't find it
- Disable parameters processing, in that case we can provide only switches, the rest of elements goes to pass through key 
if specified
- Add logic in -require section: different combinations of elements could provide valid requirement. For example,
{-a= -require {(a && b) || (a && c)}}, so to fulfill the requirement, we can provide a and b together, or a and c together.
It could allow to demand one of many, for example {a||b||c}
