
#include "argparse.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tcl.h>


//***    SetGlobalSwitch function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * SetGlobalSwitch --
 *
 *      Sets the state of a global switch in the provided context. If the switch takes an argument, the argument is
 *      stored and its reference count is incremented.
 *
 * Parameters:
 *      GlobalSwitchesContext *ctx       - input: pointer to the context holding the state of global switches
 *      int globalSwitchId               - input: identifier of the global switch to set
 *      Tcl_Obj *value                   - input: optional argument value for the switch; can be NULL if the switch does
 *                                         not take an argument
 *
 * Results:
 *      The function modifies the `ctx` object by updating the global switch state and storing any associated value.
 *
 * Side Effects:
 *      - Updates the global switch state in the `ctx` context by setting the corresponding bit for `globalSwitchId`.
 *      - If the switch takes an argument and a value is provided, the function stores the value in the `ctx->values` 
 *        array and increments its reference count.
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
void SetGlobalSwitch(GlobalSwitchesContext *ctx, int globalSwitchId, Tcl_Obj *value) {
    ctx->globalSwitches |= (1 << globalSwitchId);
    if ((GLOBAL_SWITCH_TAKES_ARG_MASK & (1 << globalSwitchId)) && value) {
        ctx->values[globalSwitchId] = value;
        Tcl_IncrRefCount(value);
    }
}

//***    FreeGlobalSwitches function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * FreeGlobalSwitches --
 *
 *      Frees the resources associated with the global switches in the provided context. This function decrements the 
 *      reference counts of any stored arguments and resets the state of the global switches.
 *
 * Parameters:
 *      GlobalSwitchesContext *ctx       - input: pointer to the context holding the global switches and their values
 *
 * Results:
 *      None. The function frees resources in the `ctx` context and resets the global switch state.
 *
 * Side Effects:
 *      - Decrements the reference count of each value stored in the `ctx->values` array.
 *      - Sets each entry in `ctx->values` to `NULL` after decrementing the reference count.
 *      - Resets the global switches bitmask (`ctx->globalSwitches`) to zero.
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
void FreeGlobalSwitches(GlobalSwitchesContext *ctx) {
    for (int i = 0; i < GLOBAL_SWITCH_COUNT; ++i) {
        if (ctx->values[i]) {
            Tcl_DecrRefCount(ctx->values[i]);
            ctx->values[i] = NULL;
        }
    }
    ctx->globalSwitches = 0;
}

//** Initialization functions


//** Argument definition initialization and functions
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * InitArgumentDefinition --
 *
 *      Initializes an `ArgumentDefinition` structure by creating and setting up the necessary Tcl objects for managing
 *      argument definitions, aliases, order, switches, upvars, omitted switches, and catchall parameters. This function
 *      also increments the reference counts of the created Tcl objects to ensure proper memory management.
 *
 * Parameters:
 *      ArgumentDefinition *ctx        - input: pointer to the `ArgumentDefinition` structure to initialize
 *
 * Results:
 *      None. This function initializes the fields of the `ArgumentDefinition` structure with new Tcl objects.
 *
 * Side Effects:
 *      - Initializes the following fields in the `ArgumentDefinition` structure:
 *          - `defDict`: A dictionary for storing argument definitions.
 *          - `aliasesDict`: A dictionary for storing argument aliases.
 *          - `orderList`: A list for storing the order of parameters.
 *          - `switchesList`: A list for storing switches related to arguments.
 *          - `upvarsDict`: A dictionary for storing upvariable references.
 *          - `omittedDict`: A dictionary for storing omitted arguments.
 *          - `catchall`: A pointer for storing a catchall parameter (initialized to `NULL`).
 *      - Increments the reference counts of each Tcl object to manage memory properly.
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
void InitArgumentDefinition(ArgumentDefinition *ctx) {
    ctx->defDict = Tcl_NewDictObj();
    ctx->aliasesDict = Tcl_NewDictObj();
    ctx->orderList = Tcl_NewListObj(0, NULL);
    ctx->switchesList = Tcl_NewListObj(0, NULL);
    ctx->upvarsDict = Tcl_NewDictObj();
    ctx->omittedDict = Tcl_NewDictObj();
    ctx->catchall = NULL;
    Tcl_IncrRefCount(ctx->defDict);
    Tcl_IncrRefCount(ctx->aliasesDict);
    Tcl_IncrRefCount(ctx->orderList);
    Tcl_IncrRefCount(ctx->switchesList);
    Tcl_IncrRefCount(ctx->upvarsDict);
    Tcl_IncrRefCount(ctx->omittedDict);
}

/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * DeepCopyArgumentDefinition --
 *
 *      Creates a deep copy of the given `ArgumentDefinition` structure. This function allocates memory for a new 
 *      `ArgumentDefinition` and copies all relevant fields from the source structure. It ensures that all Tcl objects
 *      are properly duplicated, including nested dictionaries.
 *
 * Parameters:
 *      Tcl_Interp *interp             - input: the Tcl interpreter used for duplicating Tcl objects
 *      const ArgumentDefinition *src  - input: pointer to the `ArgumentDefinition` structure to copy
 *
 * Results:
 *      Returns a pointer to a newly allocated `ArgumentDefinition` structure that is a deep copy of the source 
 *      structure, or `NULL` if there is an error (e.g., memory allocation failure or invalid input).
 *
 * Side Effects:
 *      - Allocates memory for a new `ArgumentDefinition` structure.
 *      - Duplicates the dictionaries and lists from the source structure:
 *          - `defDict`: Duplicated using `DuplicateDictWithNestedDicts` (deep copy of dictionary).
 *          - `aliasesDict`, `orderList`, `switchesList`, `upvarsDict`, `omittedDict`: Duplicated using 
 *            `Tcl_DuplicateObj`.
 *          - `catchall`: Duplicated if not `NULL`.
 *      - If memory allocation fails, returns `NULL`.
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
ArgumentDefinition *DeepCopyArgumentDefinition(Tcl_Interp *interp, const ArgumentDefinition *src) {
    if (src == NULL)
        return NULL;

    ArgumentDefinition *copy = malloc(sizeof(ArgumentDefinition));
    if (copy == NULL)
        return NULL; // malloc failure
    copy->defDict = DuplicateDictWithNestedDicts(interp, src->defDict);
    copy->aliasesDict = Tcl_DuplicateObj(src->aliasesDict);
    copy->orderList = Tcl_DuplicateObj(src->orderList);
    copy->switchesList = Tcl_DuplicateObj(src->switchesList);
    copy->upvarsDict = Tcl_DuplicateObj(src->upvarsDict);
    copy->omittedDict = Tcl_DuplicateObj(src->omittedDict);
    copy->catchall = src->catchall ? Tcl_DuplicateObj(src->catchall) : NULL;
    return copy;
}

/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * FreeArgumentDefinition --
 *
 *      Frees the resources associated with an `ArgumentDefinition` structure. This function decrements the reference 
 *      counts of all Tcl objects in the structure and then frees the memory allocated for the `ArgumentDefinition`
 *      itself.
 *
 * Parameters:
 *      ArgumentDefinition *argDef     - input: pointer to the `ArgumentDefinition` structure to free
 *
 * Results:
 *      None. This function frees the resources associated with the `ArgumentDefinition` structure.
 *
 * Side Effects:
 *      - Decrements the reference counts for the following Tcl objects:
 *          - `defDict`: The dictionary of argument definitions.
 *          - `aliasesDict`: The dictionary of argument aliases.
 *          - `orderList`: The list representing the order of parameters.
 *          - `switchesList`: The list of switches.
 *          - `upvarsDict`: The dictionary of upvariables.
 *          - `omittedDict`: The dictionary of omitted arguments.
 *          - `catchall`: The catchall argument, if non-NULL.
 *      - Frees the memory allocated for the `ArgumentDefinition` structure after the Tcl objects are freed.
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
void FreeArgumentDefinition(ArgumentDefinition *argDef) {
    if (!argDef)
        return;
    if (argDef->defDict)
        Tcl_DecrRefCount(argDef->defDict);
    if (argDef->aliasesDict)
        Tcl_DecrRefCount(argDef->aliasesDict);
    if (argDef->orderList)
        Tcl_DecrRefCount(argDef->orderList);
    if (argDef->switchesList)
        Tcl_DecrRefCount(argDef->switchesList);
    if (argDef->upvarsDict)
        Tcl_DecrRefCount(argDef->upvarsDict);
    if (argDef->omittedDict)
        Tcl_DecrRefCount(argDef->omittedDict);
    if (argDef->catchall)
        Tcl_DecrRefCount(argDef->catchall);
    free(argDef);
}
//** Functions that evaluates Tcl commands
//***    EvalPrefixMatch function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * EvalPrefixMatch --
 *
 *      Call the Tcl `tcl::prefix match` command with optional flags and arguments to perform a prefix match.
 *
 * Parameters:
 *      Tcl_Interp *interp            - input: target interpreter
 *      Tcl_Obj *tableList            - input: pointer to Tcl_Obj with list of valid prefixes to match against
 *      Tcl_Obj *matchObj             - input: pointer to Tcl_Obj holding the string to match
 *      int useExact                  - input: if non-zero, include the `-exact` option in the command
 *      int useMessage                - input: if non-zero and messageObj is non-NULL, include `-message` option
 *      Tcl_Obj *messageObj           - input: optional message string for the `-message` flag; ignored if NULL
 *      int wantErrorMessage          - input: if non-zero, store the error message in *resultObjPtr on failure
 *      Tcl_Obj **resultObjPtr        - output: double pointer to store the result of the prefix match or error message
 *
 * Results:
 *      Code TCL_OK - stores pointer to Tcl_Obj holding the matching result in *resultObjPtr (reference count 
 *                    incremented)
 *      Code TCL_ERROR and wantErrorMessage == 1 - stores pointer to error message in *resultObjPtr (no ref count change)
 *      Code TCL_ERROR and wantErrorMessage == 0 - clears interpreter result; *resultObjPtr remains unchanged
 *
 * Side Effects:
 *      Reference count of the result object is incremented on success  
 *      All temporary Tcl_Obj arguments are created and properly reference-managed  
 *      Interpreter result is reset unless returning an error message
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
int EvalPrefixMatch(Tcl_Interp *interp, Tcl_Obj *tableList, Tcl_Obj *matchObj, int useExact, int useMessage,
                    Tcl_Obj *messageObj, int wantErrorMessage, Tcl_Obj **resultObjPtr) {
    Tcl_Obj *objv[8];
    int objc = 0;
    int code;
    objv[objc++] = Tcl_NewStringObj("tcl::prefix", -1);
    objv[objc++] = Tcl_NewStringObj("match", -1);
    if (useMessage && messageObj != NULL) {
        objv[objc++] = Tcl_NewStringObj("-message", -1);
        objv[objc++] = messageObj;
    }
    if (useExact) {
        objv[objc++] = Tcl_NewStringObj("-exact", -1);
    }
    objv[objc++] = tableList;
    objv[objc++] = matchObj;
    for (int i = 0; i < objc; i++) {
        Tcl_IncrRefCount(objv[i]);
    }
    code = Tcl_EvalObjv(interp, objc, objv, 0);
    if (code == TCL_OK) {
        if (resultObjPtr != NULL) {
            *resultObjPtr = Tcl_GetObjResult(interp);
            Tcl_IncrRefCount(*resultObjPtr);
            Tcl_ResetResult(interp);
        }
    } else {
        if (wantErrorMessage && resultObjPtr != NULL) {
            *resultObjPtr = Tcl_GetObjResult(interp);
        } else {
            Tcl_ResetResult(interp);
        }
    }
    for (int i = 0; i < objc; i++) {
        Tcl_DecrRefCount(objv[i]);
    }
    return code;
}

//***    EvalPrefixMatch function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * PrefixMatch --
 *
 *      Perform a prefix match against a list of strings using Tcl_GetIndexFromObj, optionally including flags and
 *      custom error messages. On success, returns the matched string as a Tcl_Obj.
 *
 * Parameters:
 *      Tcl_Interp *interp            - input: target interpreter
 *      const char **tableList        - input: NULL-terminated array of valid string prefixes to match against
 *      Tcl_Obj *matchObj             - input: pointer to Tcl_Obj holding the string to match
 *      int useExact                  - input: if non-zero, require exact match via TCL_EXACT flag
 *      int useMessage                - input: if non-zero and messageObj is non-NULL, use messageObj as error message
 *      char *messageObj              - input: optional custom error message (ignored if useMessage is 0 or NULL)
 *      int wantErrorMessage          - input: if non-zero, store error message in *resultObjPtr on failure
 *      Tcl_Obj **resultObjPtr        - output: double pointer to store result (matched string) or error message
 *
 * Results:
 *      Code TCL_OK - stores Tcl_Obj with matching string from tableList in *resultObjPtr (ref count incremented)
 *      Code TCL_ERROR and wantErrorMessage == 1 - stores pointer to error message in *resultObjPtr (no ref count change)
 *      Code TCL_ERROR and wantErrorMessage == 0 - clears interpreter result; *resultObjPtr remains unchanged
 *
 * Side Effects:
 *      Reference count of the returned result object is incremented on success
 *      Interpreter result is reset on success or when no error message is returned
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
int PrefixMatch(Tcl_Interp *interp, const char **tableList, Tcl_Obj *matchObj, int useExact, int useMessage,
                    char *messageObj, int wantErrorMessage, Tcl_Obj **resultObjPtr) {
    int code;
    int flags = 0;
    if (useExact) {
        flags = flags | TCL_EXACT;
    }
    int index;
    code = Tcl_GetIndexFromObj(interp, matchObj, tableList, messageObj, flags, &index);
    if (code == TCL_OK) {
        if (resultObjPtr != NULL) {
            *resultObjPtr = Tcl_NewStringObj(tableList[index], -1);
        }
        Tcl_ResetResult(interp);
    } else {
        if (wantErrorMessage && (resultObjPtr != NULL)) {
            *resultObjPtr = Tcl_GetObjResult(interp);
        } else {
            Tcl_ResetResult(interp);
        }
    }
    return code;
}

//***    EvalRegsubFirstMatch function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * EvalRegsubFirstMatch --
 *
 *      Perform a single substitution of the first match of a regular expression in the input string.
 *
 * Parameters:
 *      Tcl_Interp *interp           - input: target interpreter
 *      Tcl_RegExp regexp            - input: compiled regular expression to match
 *      Tcl_Obj *inputObj            - input: pointer to Tcl_Obj holding the original string
 *      Tcl_Obj *replacementObj      - input: pointer to Tcl_Obj holding the replacement string
 *      Tcl_Obj **resultObjPtr       - output: double pointer to Tcl_Obj that will store the resulting string
 *
 * Results:
 *      Code TCL_OK - if a match is found, stores the new string with replacement in **resultObjPtr (ref count
 *                    incremented); if no match, **resultObjPtr is set to inputObj without ref count change
 *      Code TCL_ERROR - error occurred during regular expression matching; **resultObjPtr is left unchanged
 *
 * Side Effects:
 *      If a match is found, a new Tcl_Obj is created and its reference count is incremented before returning  
 *      No changes to interpreter result; it is not reset or modified
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
int EvalRegsubFirstMatch(Tcl_Interp *interp, Tcl_RegExp regexp, Tcl_Obj *inputObj, Tcl_Obj *replacementObj,
                    Tcl_Obj **resultObjPtr) {
    int match;
    const char *matchStart = NULL, *matchEnd = NULL;
    const char *inputStr, *replaceStr;
    Tcl_Size inputLen, replaceLen, prefixLen, suffixLen;
    Tcl_Obj *resultObj = NULL;
    inputStr = Tcl_GetStringFromObj(inputObj, &inputLen);
    match = Tcl_RegExpExec(interp, regexp, inputStr, inputStr);
    if (match < 0) {
        return TCL_ERROR;
    } else if (match == 0) {
        *resultObjPtr = inputObj;
        return TCL_OK;
    }
    Tcl_RegExpRange(regexp, 0, &matchStart, &matchEnd);
    prefixLen = matchStart - inputStr;
    suffixLen = inputLen - (matchEnd - inputStr);
    resultObj = Tcl_NewObj();
    Tcl_IncrRefCount(resultObj);
    Tcl_AppendToObj(resultObj, inputStr, prefixLen);
    replaceStr = Tcl_GetStringFromObj(replacementObj, &replaceLen);
    Tcl_AppendToObj(resultObj, replaceStr, replaceLen);
    Tcl_AppendToObj(resultObj, matchEnd, suffixLen);
    *resultObjPtr = resultObj;
    return TCL_OK;
}

//***    EvalMatchRegexpGroups function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * EvalMatchRegexpGroups --
 *
 *      Execute a regular expression against a given string and extract all matched groups.
 *
 * Parameters:
 *      Tcl_Interp *interp           - input: target interpreter
 *      Tcl_RegExp regexp            - input: compiled regular expression object
 *      Tcl_Obj *textObj             - input: pointer to Tcl_Obj holding the input string to match
 *      ArgparseInterpCtx *interpCtx - input: interpreter context structure
 *      Tcl_Obj **resultListPtr      - output: double pointer to Tcl_Obj that will store the list of matched substrings
 *
 * Results:
 *      Code TCL_OK - stores pointer to Tcl_Obj holding a list of matched groups in **resultListPtr; reference count
 *                    of the list is incremented
 *      Code TCL_ERROR - regular expression did not match; resultListPtr is left unchanged
 *
 * Side Effects:
 *      Allocates and populates a new list object with matched substrings, including empty strings for unmatched groups
 *      Reference count of the result list is incremented before returning
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
int EvalMatchRegexpGroups(Tcl_Interp *interp, Tcl_RegExp regexp, Tcl_Obj *textObj, ArgparseInterpCtx *interpCtx, 
                          Tcl_Obj **resultListPtr) {
    Tcl_RegExpInfo info;
    const char *inputStr = Tcl_GetString(textObj);
    const char *start, *end;
    if (Tcl_RegExpExec(interp, regexp, inputStr, inputStr) != 1) {
        return TCL_ERROR;
    }
    Tcl_RegExpGetInfo(regexp, &info);
    Tcl_Obj *resultList = Tcl_NewListObj(0, NULL);
    Tcl_IncrRefCount(resultList);
    for (int i = 0; i <= info.nsubs; ++i) {
        if (info.matches[i].start >= 0 && info.matches[i].end >= info.matches[i].start) {
            Tcl_RegExpRange(regexp, i, &start, &end);
            Tcl_Obj *substr = Tcl_NewStringObj(start, end - start);
            Tcl_ListObjAppendElement(interp, resultList, substr);
        } else {
            Tcl_ListObjAppendElement(interp, resultList, Tcl_DuplicateObj(interpCtx->misc_emptyStrObj));
        }
    }
    *resultListPtr = resultList;
    return TCL_OK;
}

//***    EvalStringMap function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * EvalStringMap --
 *
 *      Call the Tcl `string map` command with a given list of mapping pairs and input string.
 *
 * Parameters:
 *      Tcl_Interp *interp       - input: target interpreter
 *      Tcl_Obj *mapListObj      - input: pointer to Tcl_Obj holding a flat list of string pairs for mapping
 *      Tcl_Obj *inputObj        - input: pointer to Tcl_Obj holding the input string to transform
 *      Tcl_Obj **resultObjPtr   - output: double pointer to Tcl_Obj that will store the result of the mapping operation
 *
 * Results:
 *      Code TCL_OK - stores pointer to Tcl_Obj holding the mapped result in **resultObjPtr and increments its
 *                    reference count
 *      Code TCL_ERROR - resultObjPtr is left unchanged; interpreter result is cleared after evaluation
 *
 * Side Effects:
 *      Reference count increment of **resultObjPtr if return code is TCL_OK  
 *      Interpreter result is reset after execution  
 *      Temporary reference counts are properly managed for created and intermediate Tcl_Obj values
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
int EvalStringMap(Tcl_Interp *interp, Tcl_Obj *mapListObj, Tcl_Obj *inputObj, Tcl_Obj **resultObjPtr) {
    Tcl_Obj *objv[4];
    int code;
    objv[0] = Tcl_NewStringObj("string", -1);
    objv[1] = Tcl_NewStringObj("map", -1);
    objv[2] = Tcl_NewListObj(0, NULL);
    objv[3] = inputObj;
    Tcl_Size mapLen;
    Tcl_Obj **mapElems;
    Tcl_ListObjGetElements(interp, mapListObj, &mapLen, &mapElems);
    for (int i = 0; i < mapLen; ++i) {
        Tcl_ListObjAppendElement(interp, objv[2], mapElems[i]);
    }
    for (int i = 0; i < 4; ++i) {
        Tcl_IncrRefCount(objv[i]);
    }
    Tcl_Obj *savedResult = Tcl_GetObjResult(interp);
    Tcl_IncrRefCount(savedResult);
    code = Tcl_EvalObjv(interp, 4, objv, 0);
    if (code == TCL_OK) {
        *resultObjPtr = Tcl_GetObjResult(interp);
        Tcl_IncrRefCount(*resultObjPtr);
    }
    Tcl_DecrRefCount(savedResult);
    for (int i = 0; i < 4; ++i) {
        Tcl_DecrRefCount(objv[i]);
    }
    Tcl_ResetResult(interp);
    return code;
}
//***    EvalLsort function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * EvalLsort --
 *
 *      Evaluate the Tcl `lsort` command on a given list object and return the sorted result.
 *
 * Parameters:
 *      Tcl_Interp *interp         - input: target interpreter
 *      Tcl_Obj *listObj           - input: pointer to Tcl_Obj holding the list to be sorted
 *      Tcl_Obj **resultObjPtr     - output: double pointer to store the sorted list result (if not NULL)
 *
 * Results:
 *      Code TCL_OK - stores pointer to Tcl_Obj holding the sorted list in *resultObjPtr (reference count incremented)
 *      Code TCL_ERROR - evaluation failed; *resultObjPtr is left unchanged
 *
 * Side Effects:
 *      Temporary Tcl_Obj for the command is created and properly reference-managed  
 *      Interpreter result is reset after evaluation  
 *      Reference count of result object is incremented if TCL_OK
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
int EvalLsort(Tcl_Interp *interp, Tcl_Obj *listObj, Tcl_Obj **resultObjPtr) {
    Tcl_Obj *objv[2];
    int code;
    objv[0] = Tcl_NewStringObj("lsort", -1);
    objv[1] = listObj;
    Tcl_IncrRefCount(objv[0]);
    Tcl_IncrRefCount(objv[1]);
    code = Tcl_EvalObjv(interp, 2, objv, 0);
    if (code == TCL_OK && resultObjPtr != NULL) {
        *resultObjPtr = Tcl_GetObjResult(interp);
        Tcl_IncrRefCount(*resultObjPtr);
    }
    Tcl_DecrRefCount(objv[0]);
    Tcl_DecrRefCount(objv[1]);
    Tcl_ResetResult(interp);
    return code;
}

//***    EvaluateAdjust function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * EvaluateAdjust --
 *
 *      Call the Tcl command `textutil::adjust::adjust` to adjust the input string to a specified length.
 *
 * Parameters:
 *      Tcl_Interp *interp        - input: target interpreter
 *      Tcl_Obj *stringObj        - input: pointer to Tcl_Obj holding the string to adjust
 *      int len                   - input: target length for adjustment (e.g., padding or trimming)
 *
 * Results:
 *      Returns a pointer to a Tcl_Obj containing the adjusted string; the reference count is incremented
 *
 * Side Effects:
 *      Allocates temporary Tcl_Obj values for the command and its arguments  
 *      Evaluates the constructed Tcl command in the interpreter  
 *      Interpreter result is reset after fetching the output
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
Tcl_Obj *EvaluateAdjust(Tcl_Interp *interp, Tcl_Obj *stringObj, int len) {
    Tcl_Obj *cmdList[4];
    cmdList[0] = Tcl_NewStringObj("textutil::adjust::adjust", -1);
    cmdList[1] = stringObj;
    cmdList[2] = Tcl_NewStringObj("-length", -1);
    cmdList[3] = Tcl_NewIntObj(len);
    Tcl_Obj *cmd = Tcl_NewListObj(4, cmdList);
    Tcl_EvalObjEx(interp, cmd, 0);
    Tcl_Obj *result = Tcl_GetObjResult(interp);
    Tcl_IncrRefCount(result);
    Tcl_ResetResult(interp);
    return result;
}

//***    EvaluateIndent function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * EvaluateIndent --
 *
 *      Call the Tcl command `textutil::adjust::indent` to indent each line of the input string.
 *
 * Parameters:
 *      Tcl_Interp *interp        - input: target interpreter
 *      Tcl_Obj *stringObj        - input: pointer to Tcl_Obj holding the string to indent
 *      Tcl_Obj *spacesObj        - input: Tcl_Obj representing the number of spaces to insert as indentation
 *      Tcl_Obj *skipObj          - input: Tcl_Obj representing the number of initial lines to skip indentation
 *
 * Results:
 *      Returns a pointer to a Tcl_Obj containing the indented string (reference count incremented),
 *      or NULL if command evaluation fails
 *
 * Side Effects:
 *      Constructs and evaluates a Tcl list command directly in the interpreter  
 *      Interpreter result is reset after the output is retrieved  
 *      Reference count of the returned result is incremented if successful
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
Tcl_Obj *EvaluateIndent(Tcl_Interp *interp, Tcl_Obj *stringObj, Tcl_Obj *spacesObj, Tcl_Obj *skipObj) {
    Tcl_Obj *cmdList[4];
    cmdList[0] = Tcl_NewStringObj("textutil::adjust::indent", -1);
    cmdList[1] = stringObj;
    cmdList[2] = spacesObj;
    cmdList[3] = skipObj;
    Tcl_Obj *cmd = Tcl_NewListObj(4, cmdList);
    if (Tcl_EvalObjEx(interp, cmd, TCL_EVAL_DIRECT) != TCL_OK) {
        return NULL;
    }
    Tcl_Obj *result = Tcl_GetObjResult(interp);
    Tcl_IncrRefCount(result);
    Tcl_ResetResult(interp);
    return result;
}

//***    EvaluateStringToTitle function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * EvaluateStringToTitle --
 *
 *      Call the Tcl `string totitle` command to convert a string (or a range within it) to title case.
 *
 * Parameters:
 *      Tcl_Interp *interp          - input: target interpreter
 *      Tcl_Obj *stringObj          - input: pointer to Tcl_Obj holding the string to convert
 *      Tcl_Obj *firstIndexObj      - input: optional Tcl_Obj specifying the starting index for conversion (can be NULL)
 *      Tcl_Obj *lastIndexObj       - input: optional Tcl_Obj specifying the ending index for conversion (can be NULL)
 *
 * Results:
 *      Returns a pointer to a Tcl_Obj containing the title-cased result (reference count incremented)
 *
 * Side Effects:
 *      Builds and evaluates a Tcl command dynamically based on the presence of index arguments  
 *      Interpreter result is reset after the result is retrieved  
 *      Reference count of the returned Tcl_Obj is incremented
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
Tcl_Obj *EvaluateStringToTitle(Tcl_Interp *interp, Tcl_Obj *stringObj, Tcl_Obj *firstIndexObj, Tcl_Obj *lastIndexObj) {
    Tcl_Obj *cmdList[5];
    int count = 0;
    cmdList[count++] = Tcl_NewStringObj("string", -1);
    cmdList[count++] = Tcl_NewStringObj("totitle", -1);
    cmdList[count++] = stringObj;
    if (firstIndexObj != NULL) {
        cmdList[count++] = firstIndexObj;
        if (lastIndexObj != NULL) {
            cmdList[count++] = lastIndexObj;
        }
    }
    Tcl_Obj *cmd = Tcl_NewListObj(count, cmdList);
    Tcl_EvalObjEx(interp, cmd, TCL_EVAL_DIRECT);
    Tcl_Obj *result = Tcl_GetObjResult(interp);
    Tcl_IncrRefCount(result);
    Tcl_ResetResult(interp);
    return result;
}

//** List manipulation functions
//***    InList function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * InList --
 *
 *      Check whether a given string item exists in a Tcl list by performing a string comparison.
 *
 * Parameters:
 *      Tcl_Interp *interp        - input: target interpreter
 *      Tcl_Obj *itemObj          - input: pointer to Tcl_Obj holding the string to search for
 *      Tcl_Obj *listObj          - input: pointer to Tcl_Obj representing a list of string elements
 *
 * Results:
 *      Returns 1 if itemObj matches any element in listObj (using strcmp), otherwise returns 0
 *
 * Side Effects:
 *      None
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
int InList(Tcl_Interp *interp, Tcl_Obj *itemObj, Tcl_Obj *listObj) {
    Tcl_Obj **elemPtrs;
    Tcl_Size listLen;
    int result = 0;
    Tcl_ListObjGetElements(interp, listObj, &listLen, &elemPtrs);
    const char *itemStr = Tcl_GetString(itemObj);
    for (Tcl_Size i = 0; i < listLen; ++i) {
        const char *elemStr = Tcl_GetString(elemPtrs[i]);
        if (strcmp(itemStr, elemStr) == 0) {
            result = 1;
            break;
        }
    }
    return result;
}

//***    MergeTwoLists function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * MergeTwoLists --
 *
 *      Merge two Tcl list objects into a single flat list by appending all elements from both lists.
 *
 * Parameters:
 *      Tcl_Interp *interp        - input: target interpreter
 *      Tcl_Obj *list1            - input: pointer to Tcl_Obj representing the first list
 *      Tcl_Obj *list2            - input: pointer to Tcl_Obj representing the second list
 *
 * Results:
 *      Returns a new Tcl_Obj containing a flat list of all elements from list1 followed by list2.
 *      The returned object has a reference count of zero.
 *
 * Side Effects:
 *      None
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
Tcl_Obj *MergeTwoLists(Tcl_Interp *interp, Tcl_Obj *list1, Tcl_Obj *list2) {
    Tcl_Obj *result = Tcl_NewListObj(0, NULL); // Resulting flat list
    Tcl_Obj **elems;
    Tcl_Size len;
    Tcl_ListObjGetElements(interp, list1, &len, &elems);
    for (Tcl_Size i = 0; i < len; ++i) {
        Tcl_ListObjAppendElement(interp, result, elems[i]);
    }
    Tcl_ListObjGetElements(interp, list2, &len, &elems);
    for (Tcl_Size i = 0; i < len; ++i) {
        Tcl_ListObjAppendElement(interp, result, elems[i]);
    }
    return result;
}

//***    ListRange function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * ListRange --
 *
 *      Extract a subrange from a Tcl list object based on the specified start and end indices.
 *
 * Parameters:
 *      Tcl_Interp *interp        - input: target interpreter
 *      Tcl_Obj *listObj          - input: pointer to Tcl_Obj representing the source list
 *      Tcl_Size start            - input: starting index of the range (inclusive)
 *      Tcl_Size end              - input: ending index of the range (inclusive)
 *
 * Results:
 *      Returns a new Tcl_Obj containing a list of elements from index `start` to `end`.
 *      If indices are out of bounds or invalid (start > end), returns an empty list.
 *      The returned object has a reference count of zero.
 *
 * Side Effects:
 *      None
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
Tcl_Obj *ListRange(Tcl_Interp *interp, Tcl_Obj *listObj, Tcl_Size start, Tcl_Size end) {
    Tcl_Obj **elemPtrs;
    Tcl_Size listLen;
    Tcl_ListObjGetElements(interp, listObj, &listLen, &elemPtrs);
    // Normalize start and end indices
    if (start >= listLen) {
        start = listLen;
    }
    if (end >= listLen) {
        end = listLen - 1;
    }
    if (start > end || start >= listLen) {
        return Tcl_NewListObj(0, NULL);
    }
    Tcl_Size rangeLen = end - start + 1;
    return Tcl_NewListObj(rangeLen, &elemPtrs[start]);
}

//** SplitString function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * SplitString --
 *
 *      Split a Unicode string into a list of substrings using any of the characters from a set of separators.
 *      If no separators are provided, the string is split into individual characters.
 *
 * Parameters:
 *      Tcl_Interp *interp           - input: target interpreter
 *      Tcl_Obj *stringObj           - input: pointer to Tcl_Obj holding the string to split
 *      Tcl_Obj *sepCharsObj         - input: pointer to Tcl_Obj holding a set of separator characters; may be empty
 *      ArgparseInterpCtx *interpCtx - input: interpreter context structure
 *
 * Results:
 *      Returns a Tcl_Obj containing a list of substrings.  
 *      If `sepCharsObj` is empty, each character becomes a list element.  
 *      Adjacent or trailing separators produce empty string elements.  
 *      The returned object has a reference count of zero.
 *
 * Side Effects:
 *      None
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
Tcl_Obj *SplitString(Tcl_Interp *interp, Tcl_Obj *stringObj, Tcl_Obj *sepCharsObj, ArgparseInterpCtx *interpCtx) {
    Tcl_Size strLen, sepLen, start;
    const Tcl_UniChar *str = Tcl_GetUnicodeFromObj(stringObj, &strLen);
    const Tcl_UniChar *seps = Tcl_GetUnicodeFromObj(sepCharsObj, &sepLen);
    Tcl_Obj *listObj = Tcl_NewListObj(0, NULL);
    if (sepLen == 0) {
        /* Split to every character */
        for (Tcl_Size i = 0; i < strLen; ++i) {
            Tcl_ListObjAppendElement(interp, listObj, Tcl_GetRange(stringObj, i, i));
        }
        return listObj;
    }
    /* Split on any character in seps */
    start = 0;
    for (Tcl_Size i = 0; i < strLen; ++i) {
        int isSep = 0;
        for (int j = 0; j < sepLen; ++j) {
            if (str[i] == seps[j]) {
                isSep = 1;
                break;
            }
        }
        if (isSep) {
            /* Append substring [start..i-1] */
            if (i > start) {
                Tcl_ListObjAppendElement(interp, listObj, Tcl_GetRange(stringObj, start, i - 1));
            } else {
                /* Adjacent separators produce empty elements */
                Tcl_ListObjAppendElement(interp, listObj, Tcl_DuplicateObj(interpCtx->misc_emptyStrObj));
            }
            start = i + 1;
        }
    }
    /* Append last segment */
    if (start < strLen) {
        Tcl_ListObjAppendElement(interp, listObj, Tcl_GetRange(stringObj, start, strLen - 1));
    } else {
        /* Trailing separator => empty last element */
        Tcl_ListObjAppendElement(interp, listObj, Tcl_NewStringObj("", 0));
    }
    return listObj;
}

//** Dict manipulation functions
//***    DictKeyExists function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * DictKeyExists --
 *
 *      Check whether a specified key exists in a Tcl dictionary object.
 *
 * Parameters:
 *      Tcl_Interp *interp         - input: target interpreter
 *      Tcl_Obj *dictObj           - input: pointer to Tcl_Obj representing the dictionary
 *      Tcl_Obj *keyStr            - input: pointer to Tcl_Obj representing the key to check
 *
 * Results:
 *      Returns 1 if the key exists in the dictionary, 0 otherwise.
 *
 * Side Effects:
 *      None
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
static inline int DictKeyExists(Tcl_Interp *interp, Tcl_Obj *dictObj, Tcl_Obj *keyStr) {
    Tcl_Obj *valuePtr = NULL;
    return Tcl_DictObjGet(interp, dictObj, keyStr, &valuePtr) == TCL_OK && valuePtr != NULL;
}

//***    NestedDictKeyExists function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * NestedDictKeyExists --
 *
 *      Check whether a nested key exists within a dictionary-of-dictionaries structure.
 *
 * Parameters:
 *      Tcl_Interp *interp         - input: target interpreter
 *      Tcl_Obj *dictObj           - input: pointer to Tcl_Obj representing the outer dictionary
 *      Tcl_Obj *outerKey          - input: pointer to Tcl_Obj representing the key for the sub-dictionary
 *      Tcl_Obj *innerKey          - input: pointer to Tcl_Obj representing the key to check within the sub-dictionary
 *
 * Results:
 *      Returns 1 if both:
 *          - the outer dictionary contains `outerKey`, and
 *          - the corresponding sub-dictionary contains `innerKey`
 *      Returns 0 otherwise.
 *
 * Side Effects:
 *      None
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
int NestedDictKeyExists(Tcl_Interp *interp, Tcl_Obj *dictObj, Tcl_Obj *outerKey, Tcl_Obj *innerKey) {
    Tcl_Obj *subDict = NULL;
    if (Tcl_DictObjGet(interp, dictObj, outerKey, &subDict) != TCL_OK || subDict == NULL) {
        return 0;
    }
    Tcl_Obj *innerValue = NULL;
    if (Tcl_DictObjGet(interp, subDict, innerKey, &innerValue) != TCL_OK) {
        return 0;
    }
    return (innerValue != NULL);
}

//***    GetNestedDictValue function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * GetNestedDictValue --
 *
 *      Retrieve the value associated with a nested key from a dictionary-of-dictionaries structure.
 *
 * Parameters:
 *      Tcl_Interp *interp         - input: target interpreter
 *      Tcl_Obj *dictObj           - input: pointer to Tcl_Obj representing the outer dictionary
 *      Tcl_Obj *outerKey          - input: pointer to Tcl_Obj representing the key for the sub-dictionary
 *      Tcl_Obj *innerKey          - input: pointer to Tcl_Obj representing the key within the sub-dictionary
 *      Tcl_Obj **valuePtr         - output: pointer to store the resulting value (or NULL if not found)
 *
 * Results:
 *      Code TCL_OK - always returned; *valuePtr is set to:
 *          - the value if both keys exist,
 *          - NULL if either key is missing
 *
 * Side Effects:
 *      None
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
int GetNestedDictValue(Tcl_Interp *interp, Tcl_Obj *dictObj, Tcl_Obj *outerKey, Tcl_Obj *innerKey, Tcl_Obj **valuePtr) {
    Tcl_Obj *innerDict = NULL;
    Tcl_DictObjGet(interp, dictObj, outerKey, &innerDict);
    if (innerDict == NULL) {
        *valuePtr = NULL;
        return TCL_OK;
    }
    Tcl_Obj *value = NULL;
    Tcl_DictObjGet(interp, innerDict, innerKey, &value);
    *valuePtr = value;
    return TCL_OK;
}

//***    UnsetNestedDictKey function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * UnsetNestedDictKey --
 *
 *      Remove a key from a nested dictionary structure (dictionary-of-dictionaries).
 *
 * Parameters:
 *      Tcl_Interp *interp         - input: target interpreter
 *      Tcl_Obj *dictObj           - input: pointer to Tcl_Obj representing the outer dictionary
 *      Tcl_Obj *outerKey          - input: pointer to Tcl_Obj representing the key for the sub-dictionary
 *      Tcl_Obj *innerKey          - input: pointer to Tcl_Obj representing the key to remove from the sub-dictionary
 *
 * Results:
 *      Code TCL_OK - always returned, regardless of whether the keys existed
 *
 * Side Effects:
 *      If the sub-dictionary or outer dictionary is shared, it is duplicated before modification  
 *      The innerKey is removed from the sub-dictionary, and the updated sub-dictionary is re-inserted into dictObj
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
int UnsetNestedDictKey(Tcl_Interp *interp, Tcl_Obj *dictObj, Tcl_Obj *outerKey, Tcl_Obj *innerKey) {
    Tcl_Obj *nestedDict = NULL;
    Tcl_DictObjGet(interp, dictObj, outerKey, &nestedDict);
    if (nestedDict == NULL) {
        return TCL_OK;
    }
    if (Tcl_IsShared(nestedDict)) {
        nestedDict = Tcl_DuplicateObj(nestedDict);
    }
    Tcl_DictObjRemove(interp, nestedDict, innerKey);
    if (Tcl_IsShared(dictObj)) {
        dictObj = Tcl_DuplicateObj(dictObj);
    }
    Tcl_DictObjPut(interp, dictObj, outerKey, nestedDict);
    return TCL_OK;
}

//***    SetNestedDictKey function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * SetNestedDictKey --
 *
 *      Set a value in a nested dictionary structure (dictionary-of-dictionaries), creating intermediate dictionaries
 *      as needed.
 *
 * Parameters:
 *      Tcl_Interp *interp         - input: target interpreter
 *      Tcl_Obj *dictObj           - input: pointer to Tcl_Obj representing the outer dictionary
 *      Tcl_Obj *outerKey          - input: pointer to Tcl_Obj representing the key for the sub-dictionary
 *      Tcl_Obj *innerKey          - input: pointer to Tcl_Obj representing the key to set within the sub-dictionary
 *      Tcl_Obj *value             - input: pointer to Tcl_Obj holding the value to store
 *
 * Results:
 *      Code TCL_OK - value was successfully set
 *
 * Side Effects:
 *      If the sub-dictionary or outer dictionary is shared, it is duplicated before modification  
 *      A new sub-dictionary is created if the outerKey does not exist  
 *      The modified sub-dictionary is re-inserted into the outer dictionary under outerKey
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
int SetNestedDictKey(Tcl_Interp *interp, Tcl_Obj *dictObj, Tcl_Obj *outerKey, Tcl_Obj *innerKey, Tcl_Obj *value) {
    Tcl_Obj *nestedDict = NULL;
    Tcl_DictObjGet(interp, dictObj, outerKey, &nestedDict);
    if (nestedDict == NULL) {
        nestedDict = Tcl_NewDictObj();
    } else if (Tcl_IsShared(nestedDict)) {
        nestedDict = Tcl_DuplicateObj(nestedDict);
    }
    Tcl_DictObjPut(interp, nestedDict, innerKey, value);
    if (Tcl_IsShared(dictObj)) {
        dictObj = Tcl_DuplicateObj(dictObj);
    }
    Tcl_DictObjPut(interp, dictObj, outerKey, nestedDict);
    return TCL_OK;
}

//***    DictLappend function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * DictLappend --
 *
 *      Append a list of values to an existing list stored at a given key in a Tcl dictionary.  
 *      If the key does not exist, a new list is created and inserted.
 *
 * Parameters:
 *      Tcl_Interp *interp         - input: target interpreter
 *      Tcl_Obj *dictObjPtr        - input: pointer to Tcl_Obj representing the dictionary (duplicated if shared)
 *      Tcl_Obj *keyObj            - input: pointer to Tcl_Obj representing the dictionary key to update
 *      Tcl_Obj *valuesList        - input: pointer to Tcl_Obj representing the list of values to append
 *
 * Results:
 *      Code TCL_OK - values were successfully appended or inserted
 *
 * Side Effects:
 *      If `dictObjPtr` or the existing list at `keyObj` is shared, it is duplicated before modification  
 *      A new list is created if the key does not exist in the dictionary  
 *      The updated list is stored under `keyObj` in the dictionary
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
int DictLappend(Tcl_Interp *interp, Tcl_Obj *dictObjPtr, Tcl_Obj *keyObj, Tcl_Obj *valuesList) {
    Tcl_Obj *existingList = NULL;
    Tcl_Size listLen;
    Tcl_Obj **elements;
    if (Tcl_IsShared(dictObjPtr)) {
        dictObjPtr = Tcl_DuplicateObj(dictObjPtr);
    }
    Tcl_DictObjGet(interp, dictObjPtr, keyObj, &existingList);
    if (existingList == NULL) {
        existingList = Tcl_NewListObj(0, NULL);
    } else if (Tcl_IsShared(existingList)) {
        existingList = Tcl_DuplicateObj(existingList);
    }
    Tcl_ListObjGetElements(interp, valuesList, &listLen, &elements);
    for (Tcl_Size i = 0; i < listLen; i++) {
        Tcl_ListObjAppendElement(interp, existingList, elements[i]);
    }
    
    Tcl_DictObjPut(interp, dictObjPtr, keyObj, existingList);
    return TCL_OK;
}
//***    DictLappendElem function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * DictLappend --
 *
 *      Append a value to an existing list stored at a given key in a Tcl dictionary.  
 *      If the key does not exist, a new list is created and inserted.
 *
 * Parameters:
 *      Tcl_Interp *interp         - input: target interpreter
 *      Tcl_Obj *dictObjPtr        - input: pointer to Tcl_Obj representing the dictionary (duplicated if shared)
 *      Tcl_Obj *keyObj            - input: pointer to Tcl_Obj representing the dictionary key to update
 *      Tcl_Obj *valueObj          - input: pointer to Tcl_Obj representing the value to append
 *
 * Results:
 *      Code TCL_OK - values were successfully appended or inserted
 *
 * Side Effects:
 *      If `dictObjPtr` or the existing list at `keyObj` is shared, it is duplicated before modification  
 *      A new list is created if the key does not exist in the dictionary  
 *      The updated list is stored under `keyObj` in the dictionary
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
int DictLappendElem(Tcl_Interp *interp, Tcl_Obj *dictObjPtr, Tcl_Obj *keyObj, Tcl_Obj *valueObj) {
    Tcl_Obj *existingList = NULL;
    if (Tcl_IsShared(dictObjPtr)) {
        dictObjPtr = Tcl_DuplicateObj(dictObjPtr);
    }
    Tcl_DictObjGet(interp, dictObjPtr, keyObj, &existingList);
    if (existingList == NULL) {
        existingList = Tcl_NewListObj(0, NULL);
    } else if (Tcl_IsShared(existingList)) {
        existingList = Tcl_DuplicateObj(existingList);
    }
    Tcl_ListObjAppendElement(interp, existingList, valueObj);
    Tcl_DictObjPut(interp, dictObjPtr, keyObj, existingList);
    return TCL_OK;
}

//***    DictIncr function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * DictIncr --
 *
 *      Increment the integer value stored at a given key in a Tcl dictionary.
 *      If the key does not exist, it is initialized to zero before the increment is applied.
 *
 * Parameters:
 *      Tcl_Interp *interp         - input: target interpreter
 *      Tcl_Obj *dictObjPtr        - input: pointer to Tcl_Obj representing the dictionary (duplicated if shared)
 *      Tcl_Obj *keyObj            - input: pointer to Tcl_Obj representing the dictionary key to increment
 *      Tcl_Obj *countObj          - input: pointer to Tcl_Obj holding the integer amount to increment by
 *
 * Results:
 *      Code TCL_OK - value was successfully incremented and stored under `keyObj`
 *
 * Side Effects:
 *      If `dictObjPtr` is shared, it is duplicated before modification  
 *      If `keyObj` does not exist, it is treated as if it were zero  
 *      A new wide integer object is created and inserted into the dictionary  
 *      Temporary result object is reference-counted and safely released
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
int DictIncr(Tcl_Interp *interp, Tcl_Obj *dictObjPtr, Tcl_Obj *keyObj, Tcl_Obj *countObj) {
    Tcl_WideInt currentVal = 0;
    Tcl_WideInt incrVal = 0;
    Tcl_GetWideIntFromObj(interp, countObj, &incrVal);
    Tcl_Obj *currentObj = NULL;
    Tcl_DictObjGet(interp, dictObjPtr, keyObj, &currentObj);
    if (currentObj != NULL) {
        Tcl_GetWideIntFromObj(interp, currentObj, &currentVal);
    }
    Tcl_WideInt resultVal = currentVal + incrVal;
    Tcl_Obj *resultObj = Tcl_NewWideIntObj(resultVal);
    Tcl_IncrRefCount(resultObj);
    if (Tcl_IsShared(dictObjPtr)) {
        dictObjPtr = Tcl_DuplicateObj(dictObjPtr);
    }
    Tcl_DictObjPut(interp, dictObjPtr, keyObj, resultObj);
    Tcl_DecrRefCount(resultObj);
    return TCL_OK;
}

//***    DictKeys function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * DictKeys --
 *
 *      Extract all keys from a Tcl dictionary object and return them as an array of Tcl_Obj pointers.
 *
 * Parameters:
 *      Tcl_Interp *interp         - input: target interpreter
 *      Tcl_Obj *dictObj           - input: pointer to Tcl_Obj representing the dictionary
 *      Tcl_Size *keyCountPtr      - output: pointer to store the number of keys found
 *      Tcl_Obj ***keyObjsPtr      - output: pointer to array of Tcl_Obj* pointers representing the keys
 *                                   (memory allocated with malloc, caller is responsible for freeing)
 *
 * Results:
 *      Code TCL_OK - keys were successfully extracted; *keyCountPtr and *keyObjsPtr are set accordingly  
 *      Code TCL_ERROR - an error occurred (e.g., memory allocation failed); an error message is left in the interpreter
 *
 * Side Effects:
 *      Allocates memory for the key array via malloc or realloc  
 *      Caller must free the returned array using `free()`  
 *      If allocation fails, sets interpreter result to "out of memory"
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
int DictKeys(Tcl_Interp *interp, Tcl_Obj *dictObj, Tcl_Size *keyCountPtr, Tcl_Obj ***keyObjsPtr) {
    Tcl_DictSearch search;
    Tcl_Obj *key, *value;
    int done;
    Tcl_Size capacity = 16;
    Tcl_Size count = 0;
    Tcl_Obj **keys = (Tcl_Obj **)malloc(capacity * sizeof(Tcl_Obj *));
    if (keys == NULL) {
        Tcl_SetResult(interp, "out of memory", TCL_STATIC);
        return TCL_ERROR;
    }
    if (Tcl_DictObjFirst(interp, dictObj, &search, &key, &value, &done) != TCL_OK) {
        free(keys);
        return TCL_ERROR;
    }
    while (!done) {
        if (count == capacity) {
            // grow the array
            capacity *= 2;
            Tcl_Obj **newKeys = (Tcl_Obj **)realloc(keys, capacity * sizeof(Tcl_Obj *));
            if (newKeys == NULL) {
                Tcl_DictObjDone(&search);
                free(keys);
                Tcl_SetResult(interp, "out of memory", TCL_STATIC);
                return TCL_ERROR;
            }
            keys = newKeys;
        }
        keys[count++] = key;
        Tcl_DictObjNext(&search, &key, &value, &done);
    }
    Tcl_DictObjDone(&search);
    *keyCountPtr = count;
    *keyObjsPtr = keys;
    return TCL_OK;
}

//** CheckAliasesAreUnique function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * CheckAliasesAreUnique --
 *
 *      Verify that all aliases listed in an option dictionary do not already exist in a given alias dictionary.
 *
 * Parameters:
 *      Tcl_Interp *interp           - input: target interpreter
 *      Tcl_Obj *aliasesDict         - input: pointer to Tcl_Obj representing the dictionary of known aliases
 *      Tcl_Obj *optDict             - input: pointer to Tcl_Obj containing the option dictionary with an `-alias` entry
 *      ArgparseInterpCtx *interpCtx - input: interpreter context structure
 *
 * Results:
 *      Returns 1 if all aliases in `-alias` list are unique (not present in aliasesDict),  
 *      Returns 0 if any alias is already defined
 *
 * Side Effects:
 *      None
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
int CheckAliasesAreUnique(Tcl_Interp *interp, Tcl_Obj *aliasesDict, Tcl_Obj *optDict, ArgparseInterpCtx *interpCtx) {
    Tcl_Obj *aliasList;
    Tcl_DictObjGet(interp, optDict, interpCtx->elswitch_alias, &aliasList);
    Tcl_Obj **aliasElems;
    Tcl_Size aliasCount;
    Tcl_ListObjGetElements(interp, aliasList, &aliasCount, &aliasElems);
    for (Tcl_Size i = 0; i < aliasCount; ++i) {
        if (DictKeyExists(interp, aliasesDict, aliasElems[i])) {
            return 0;
        }
    }
    return 1;
}

//** String building functions

//***    BuildAliasJoinString function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * BuildAliasJoinString --
 *
 *      Construct a display string that joins all aliases from an option dictionary with the primary name.
 *      The result is formatted as "-alias1|alias2|...|name".
 *
 * Parameters:
 *      Tcl_Interp *interp           - input: target interpreter
 *      Tcl_Obj *optDict             - input: pointer to Tcl_Obj containing the option dictionary with an `-alias` list
 *      Tcl_Obj *name                - input: pointer to Tcl_Obj representing the primary name to append
 *      ArgparseInterpCtx *interpCtx - input: interpreter context structure
 *
 * Results:
 *      Returns a Tcl_Obj containing the constructed string. The returned object has a reference count of zero.
 *
 * Side Effects:
 *      None
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
Tcl_Obj *BuildAliasJoinString(Tcl_Interp *interp, Tcl_Obj *optDict, Tcl_Obj *name, ArgparseInterpCtx *interpCtx) {
    Tcl_Obj *aliasList = NULL;
    Tcl_DictObjGet(interp, optDict, interpCtx->elswitch_alias, &aliasList);
    Tcl_Obj **aliasElems;
    Tcl_Size aliasCount;
    Tcl_ListObjGetElements(interp, aliasList, &aliasCount, &aliasElems);
    Tcl_Obj *resultObj = Tcl_DuplicateObj(interpCtx->misc_dashStrObj);
    for (Tcl_Size i = 0; i < aliasCount; ++i) {
        Tcl_AppendObjToObj(resultObj, aliasElems[i]);
        Tcl_AppendToObj(resultObj, "|", 1);
    }
    Tcl_AppendObjToObj(resultObj, name);
    return resultObj;
}

//***    BuildAliasJoinString function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * BuildAllowedTypesSummary --
 *
 *      Create a human-readable summary string from a list of allowed types, using commas and "or" as separators.
 *      For example, the list {"int" "float" "string"} becomes "int, float or string".
 *
 * Parameters:
 *      Tcl_Interp *interp         - input: target interpreter
 *      Tcl_Obj *allowedTypes      - input: pointer to Tcl_Obj representing a list of allowed type names
 *
 * Results:
 *      Returns a Tcl_Obj containing the formatted summary string. The returned object has a reference count of zero.
 *
 * Side Effects:
 *      None
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
Tcl_Obj *BuildAllowedTypesSummary(Tcl_Interp *interp, Tcl_Obj *allowedTypes) {
    Tcl_Obj *tail = NULL;
    Tcl_Size allowedTypesLen;
    Tcl_ListObjLength(interp, allowedTypes, &allowedTypesLen);
    Tcl_Obj *rangedList = ListRange(interp, allowedTypes, 0, allowedTypesLen-2);
    Tcl_Obj *head = JoinListWithSeparator(interp, rangedList, ", ");
    Tcl_ListObjIndex(interp, allowedTypes, allowedTypesLen - 1, &tail);
    return Tcl_ObjPrintf("%s or %s", Tcl_GetString(head), Tcl_GetString(tail));
}

//***    EnumStrBuildObj function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * EnumStrBuildObj --
 *
 *      Construct a human-readable string from a list of values stored under a named key in an option dictionary.
 *      Formats the list as: "A", "A or B", or "A, B, ..., or Z" depending on the number of elements.
 *
 * Parameters:
 *      Tcl_Interp *interp         - input: target interpreter
 *      Tcl_Obj *nameObj           - input: pointer to Tcl_Obj representing the key to look up in the option dictionary
 *      Tcl_Obj *optDict           - input: pointer to Tcl_Obj representing the option dictionary
 *
 * Results:
 *      Returns a Tcl_Obj containing the formatted string summary.
 *      The returned object has a reference count of zero.
 *
 * Side Effects:
 *      None
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
Tcl_Obj *EnumStrBuildObj(Tcl_Interp *interp, Tcl_Obj *nameObj, Tcl_Obj *optDict) {
    Tcl_Obj *valList;
    Tcl_DictObjGet(interp, optDict, nameObj, &valList);
    Tcl_Size len;
    Tcl_ListObjLength(interp, valList, &len);
    Tcl_Obj *resultStr = NULL;
    if (len >= 3) {
        resultStr = Tcl_NewStringObj("", 0);
        for (Tcl_Size i = 0; i < len - 1; ++i) {
            Tcl_Obj *elem;
            Tcl_ListObjIndex(interp, valList, i, &elem);
            if (i > 0) Tcl_AppendToObj(resultStr, ", ", -1);
            Tcl_AppendObjToObj(resultStr, elem);
        }
        Tcl_Obj *lastElem;
        Tcl_ListObjIndex(interp, valList, len - 1, &lastElem);
        Tcl_AppendToObj(resultStr, " or ", -1);
        Tcl_AppendObjToObj(resultStr, lastElem);
    } else if (len == 2) {
        Tcl_Obj *first, *second;
        Tcl_ListObjIndex(interp, valList, 0, &first);
        Tcl_ListObjIndex(interp, valList, 1, &second);
        resultStr = Tcl_NewStringObj("", 0);
        Tcl_AppendObjToObj(resultStr, first);
        Tcl_AppendToObj(resultStr, " or ", -1);
        Tcl_AppendObjToObj(resultStr, second);

    } else if (len == 1) {
        Tcl_Obj *only;
        Tcl_ListObjIndex(interp, valList, 0, &only);
        resultStr = Tcl_DuplicateObj(only);
    } else {
        return Tcl_NewStringObj("", 0);
    }
    return resultStr;
}

//***    BuildBadSwitchError function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * BuildBadSwitchError --
 *
 *      Construct a formatted error message for an unrecognized switch argument, listing valid alternatives.
 *      The list of valid switches is joined into a human-readable string with appropriate punctuation.
 *
 * Parameters:
 *      Tcl_Interp *interp         - input: target interpreter
 *      Tcl_Obj *argObj            - input: pointer to Tcl_Obj holding the unrecognized switch value
 *      Tcl_Obj *switchesList      - input: pointer to Tcl_Obj representing a list of valid switch strings
 *
 * Results:
 *      Returns a Tcl_Obj containing the complete error message in the format:
 *          bad switch "X": must be A, B, or C
 *      The returned object has a reference count of zero.
 *
 * Side Effects:
 *      None
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
Tcl_Obj *BuildBadSwitchError(Tcl_Interp *interp, Tcl_Obj *argObj, Tcl_Obj *switchesList) {
    Tcl_Size len;
    const char *separator;
    Tcl_Obj *editableList = Tcl_DuplicateObj(switchesList);
    Tcl_ListObjLength(interp, editableList, &len);
    if (len > 1) {
        Tcl_Obj *lastElem = NULL;
        Tcl_ListObjIndex(interp, editableList, len - 1, &lastElem);
        Tcl_Obj *newLast = Tcl_ObjPrintf("or %s", Tcl_GetString(lastElem));
        Tcl_ListObjReplace(interp, editableList, len - 1, 1, 1, &newLast);
    }
    if (len > 2) {
        separator = ", ";
    } else {
        separator = " ";
    }
    Tcl_Obj *joinedSwitches = JoinListWithSeparator(interp, editableList, separator);
    Tcl_Obj *errMsg =
        Tcl_ObjPrintf("bad switch \"%s\": must be %s", Tcl_GetString(argObj), Tcl_GetString(joinedSwitches));
    return errMsg;
}

//***    BuildMissingSwitchesError function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * BuildMissingSwitchesError --
 *
 *      Construct an error message indicating one or more required switches are missing.
 *      The list is formatted as a human-readable string using proper punctuation and conjunctions.
 *
 * Parameters:
 *      Tcl_Interp *interp         - input: target interpreter
 *      Tcl_Obj *missingList       - input: pointer to Tcl_Obj representing a list of missing switch names
 *
 * Results:
 *      Returns a Tcl_Obj containing the formatted error message in the format:
 *          missing required switch: X
 *          or
 *          missing required switches: A, B, and C
 *      The returned object has a reference count of zero.
 *
 * Side Effects:
 *      If the input list is shared, it is duplicated before modification  
 *      The last element is modified to include "and" if more than one element is present  
 *      The result is assembled using a context-aware separator string
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
Tcl_Obj *BuildMissingSwitchesError(Tcl_Interp *interp, Tcl_Obj *missingList) {
    Tcl_Size len;
    const char *separator;
    Tcl_ListObjLength(interp, missingList, &len);
    if (Tcl_IsShared(missingList)) {
        missingList = Tcl_DuplicateObj(missingList);
    }
    if (len > 1) {
        Tcl_Obj *last = NULL;
        Tcl_ListObjIndex(interp, missingList, len - 1, &last);
        Tcl_Obj *newLast = Tcl_ObjPrintf("and %s", Tcl_GetString(last));
        Tcl_ListObjReplace(interp, missingList, len - 1, 1, 1, &newLast);
    }
    if (len > 2) {
        separator = ", ";
    } else {
        separator = " ";
    }
    Tcl_Obj *joined = JoinListWithSeparator(interp, missingList, separator);
    Tcl_Obj *err = Tcl_ObjPrintf("missing required switch%s: %s", (len > 1 ? "es" : ""), Tcl_GetString(joined));
    return err;
}

//***    BuildMissingParameterError function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * BuildMissingParameterError --
 *
 *      Construct an error message indicating one or more required parameters are missing.
 *      Formats the list with appropriate punctuation and grammar, using "and" before the final element.
 *
 * Parameters:
 *      Tcl_Interp *interp         - input: target interpreter
 *      Tcl_Obj *missingList       - input: pointer to Tcl_Obj representing a list of missing parameter names
 *
 * Results:
 *      Returns a Tcl_Obj containing the formatted error message. The message has the form:
 *          "missing required parameter: X"
 *          or
 *          "missing required parameters: A, B, and C"
 *      The returned object has a reference count of zero.
 *
 * Side Effects:
 *      If `missingList` is shared, it is duplicated before modification  
 *      The last element is prefixed with "and" if there is more than one missing parameter  
 *      The result string includes pluralization based on the list length
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
Tcl_Obj *BuildMissingParameterError(Tcl_Interp *interp, Tcl_Obj *missingList) {
    Tcl_Size len;
    const char *separator;
    Tcl_ListObjLength(interp, missingList, &len);
    if (len > 1) {
        Tcl_Obj *lastElem = NULL;
        Tcl_ListObjIndex(interp, missingList, len - 1, &lastElem);
        Tcl_Obj *replacement = Tcl_ObjPrintf("and %s", Tcl_GetString(lastElem));
        if (Tcl_IsShared(missingList)) {
            missingList = Tcl_DuplicateObj(missingList);
        }
        Tcl_ListObjReplace(interp, missingList, len - 1, 1, 1, &replacement);
    }
    if (len > 2) {
        separator = ", ";
    } else {
        separator = " ";
    }
    Tcl_Obj *joinedMissing = JoinListWithSeparator(interp, missingList, separator);
    Tcl_Obj *msg = Tcl_NewStringObj("missing required parameter", -1);
    if (len > 1) {
        Tcl_AppendToObj(msg, "s", -1);
    }
    Tcl_AppendToObj(msg, ": ", -1);
    Tcl_AppendObjToObj(msg, joinedMissing);
    return msg;
}

//***    JoinListWithSeparator function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * JoinListWithSeparator --
 *
 *      Join elements of a Tcl list into a single string using a specified separator string.
 *
 * Parameters:
 *      Tcl_Interp *interp         - input: target interpreter
 *      Tcl_Obj *listObj           - input: pointer to Tcl_Obj representing the list to join
 *      const char *separator      - input: C-string used to separate elements in the result
 *
 * Results:
 *      Returns a Tcl_Obj containing the joined string.  
 *      If the list is empty, returns an empty string object.  
 *      The returned object has a reference count of zero.
 *
 * Side Effects:
 *      None
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
Tcl_Obj *JoinListWithSeparator(Tcl_Interp *interp, Tcl_Obj *listObj, const char *separator) {
    Tcl_Size listLen;
    Tcl_Obj **elemPtrs;
    Tcl_ListObjGetElements(interp, listObj, &listLen, &elemPtrs);
    if (listLen == 0) {
        return Tcl_NewStringObj("", 0);
    }
    Tcl_Obj *result = Tcl_NewObj();
    for (Tcl_Size i = 0; i < listLen; ++i) {
        if (i > 0) {
            Tcl_AppendToObj(result, separator, -1);
        }
        const char *elemStr = Tcl_GetString(elemPtrs[i]);
        Tcl_AppendToObj(result, elemStr, -1);
    }
    return result;
}

//***    JoinWithEmptySeparator function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * JoinWithEmptySeparator --
 *
 *      Concatenate all elements of a Tcl list into a single string with no separator between them.
 *
 * Parameters:
 *      Tcl_Interp *interp         - input: target interpreter
 *      Tcl_Obj *listObj           - input: pointer to Tcl_Obj representing the list of elements to join
 *
 * Results:
 *      Returns a Tcl_Obj containing the concatenated string.  
 *      The returned object has a reference count of zero.
 *
 * Side Effects:
 *      None
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
Tcl_Obj *JoinWithEmptySeparator(Tcl_Interp *interp, Tcl_Obj *listObj) {
    Tcl_Obj **elemPtrs;
    Tcl_Size len;
    Tcl_ListObjGetElements(interp, listObj, &len, &elemPtrs);
    Tcl_Obj *result = Tcl_NewObj();
    for (Tcl_Size i = 0; i < len; ++i) {
        Tcl_AppendObjToObj(result, elemPtrs[i]);
    }
    return result;
}

//***    BuildHelpMessage function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * BuildHelpMessage --
 *
 *      Generate a formatted help message for a command-line parser based on global parsing switches and
 *      argument definitions. The message includes general behavior notes, per-switch and per-parameter descriptions,
 *      as well as detailed constraints (e.g., required relationships, default values, type hints).
 *
 * Parameters:
 *      Tcl_Interp *interp             - input: target interpreter
 *      GlobalSwitchesContext *ctx     - input: pointer to global parsing context, aka global switches (holds enabled
 *                                       parsing behaviors)
 *      ArgumentDefinition *argDefCtx  - input: pointer to argument definition context (contains metadata for each
 *                                       element)
 *      Tcl_Obj *helpLevel             - input: pointer to Tcl_Obj representing the help message depth level for `-help`
 *                                       behavior
 *      ArgparseInterpCtx *interpCtx   - input: interpreter context structure
 *
 * Results:
 *      Returns a Tcl_Obj containing the full help message as a formatted multi-line string.  
 *      The returned object has a reference count of zero.
 *
 * Side Effects:
 *      - Builds intermediate Tcl_Obj lists and strings to describe behavior and argument constraints
 *      - Performs alignment, indentation, and prefix adjustment using `textutil::adjust` utilities
 *      - Applies final substitutions via `tcl::prefix string map` (EvalStringMap) with contextual placeholders
 *
 * Notes:
 *      - Skips any options marked with `-hsuppress`
 *      - Switch descriptions are indented and grouped under a "Switches:" header
 *      - Parameter descriptions are grouped under a "Parameters:" header
 *      - If no switches or parameters are defined, only the `-help` description or top-level help is returned
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
Tcl_Obj *BuildHelpMessage(Tcl_Interp *interp, GlobalSwitchesContext *ctx, ArgumentDefinition *argDefCtx,
                          Tcl_Obj *helpLevel, ArgparseInterpCtx *interpCtx) {
    Tcl_Obj *providedHelp = Tcl_DuplicateObj(interpCtx->misc_emptyStrObj);
    Tcl_Obj *description = Tcl_NewListObj(0, NULL);
    if (Tcl_GetCharLength(GLOBAL_SWITCH_ARG(ctx, GLOBAL_SWITCH_HELP)) > 0) {
        providedHelp = EvaluateAdjust(interp, GLOBAL_SWITCH_ARG(ctx, GLOBAL_SWITCH_HELP), 80);
        Tcl_AppendStringsToObj(providedHelp, ".", (char *)NULL);
        Tcl_ListObjAppendElement(interp, description, providedHelp);
    }
    if (HAS_GLOBAL_SWITCH(ctx, GLOBAL_SWITCH_EXACT)) {
        Tcl_ListObjAppendElement(interp, description,
                                 Tcl_NewStringObj("Doesn't accept prefixes instead of switches names.", -1));
    } else {
        Tcl_ListObjAppendElement(interp, description,
                                 Tcl_NewStringObj("Can accepts unambiguous prefixes instead of switches names.", -1));
    }
    if (HAS_GLOBAL_SWITCH(ctx, GLOBAL_SWITCH_MIXED)) {
        Tcl_ListObjAppendElement(interp, description,
                                 Tcl_NewStringObj("Allows switches to appear after parameters.", -1));
    } else if (!HAS_GLOBAL_SWITCH(ctx, GLOBAL_SWITCH_PFIRST)) {
        Tcl_ListObjAppendElement(interp, description, Tcl_NewStringObj("Accepts switches only before parameters.", -1));
    }
    if (HAS_GLOBAL_SWITCH(ctx, GLOBAL_SWITCH_PFIRST)) {
        Tcl_ListObjAppendElement(interp, description,
                                 Tcl_NewStringObj("Required parameters must appear before switches.", -1));
    }
    if (HAS_GLOBAL_SWITCH(ctx, GLOBAL_SWITCH_LONG)) {
        Tcl_ListObjAppendElement(interp, description,
                                 Tcl_NewStringObj("Recognizes --switch long option alternative syntax.", -1));
    }
    if (HAS_GLOBAL_SWITCH(ctx, GLOBAL_SWITCH_EQUALARG)) {
        Tcl_ListObjAppendElement(interp, description,
                                 Tcl_NewStringObj("Recognizes -switch=arg inline argument alternative syntax.", -1));
    }
    Tcl_Obj *descriptionSwitches = Tcl_NewListObj(0, NULL);
    Tcl_Obj *descriptionParameters = Tcl_NewListObj(0, NULL);
    Tcl_DictSearch search;
    Tcl_Obj *name = NULL, *opt = NULL;
    int done;
    Tcl_DictObjFirst(interp, argDefCtx->defDict, &search, &name, &opt, &done);
    while (!done) {
        Tcl_Obj *elementDescr = Tcl_NewListObj(0, NULL);
        Tcl_Obj *constraints = Tcl_NewListObj(0, NULL);
        Tcl_Obj *combined = Tcl_NewListObj(0, NULL);
        Tcl_Obj *type = NULL;
        // basic element string building
        if (DictKeyExists(interp, opt, interpCtx->elswitch_hsuppress)) {
            Tcl_DictObjNext(&search, &name, &opt, &done);
            continue;
        }
        if (DictKeyExists(interp, opt, interpCtx->elswitch_switch)) {
            if (DictKeyExists(interp, opt, interpCtx->elswitch_required)) {
                Tcl_ListObjAppendElement(interp, elementDescr, Tcl_NewStringObj("required,", -1));
            } else if (DictKeyExists(interp, opt, interpCtx->elswitch_boolean)) {
                Tcl_ListObjAppendElement(interp, elementDescr, Tcl_NewStringObj("boolean,", -1));
            }
            if (DictKeyExists(interp, opt, interpCtx->elswitch_argument)) {
                if (DictKeyExists(interp, opt, interpCtx->elswitch_optional)) {
                    Tcl_ListObjAppendElement(interp, elementDescr, Tcl_NewStringObj("expects", -1));
                    Tcl_ListObjAppendElement(interp, elementDescr, Tcl_NewStringObj("optional", -1));
                    Tcl_ListObjAppendElement(interp, elementDescr, Tcl_NewStringObj("argument", -1));
                } else {
                    Tcl_ListObjAppendElement(interp, elementDescr, Tcl_NewStringObj("expects", -1));
                    Tcl_ListObjAppendElement(interp, elementDescr, Tcl_NewStringObj("argument", -1));
                }
            }
            type = Tcl_NewStringObj("switch", -1);
        } else {
            if (DictKeyExists(interp, opt, interpCtx->elswitch_optional)) {
                Tcl_ListObjAppendElement(interp, elementDescr, Tcl_NewStringObj("optional", -1));
            }
            type = Tcl_NewStringObj("parameter", -1);
        }
        // element constraints string building
        if (DictKeyExists(interp, opt, interpCtx->elswitch_require)) {
            Tcl_Obj *enumStr = Tcl_NewListObj(0, NULL);
            Tcl_ListObjAppendElement(interp, enumStr, Tcl_NewStringObj("Requires ", -1));
            Tcl_ListObjAppendElement(interp, enumStr, EnumStrBuildObj(interp, interpCtx->elswitch_require, opt));
            Tcl_ListObjAppendElement(interp, enumStr, Tcl_NewStringObj(".", -1));
            Tcl_ListObjAppendElement(interp, constraints, JoinWithEmptySeparator(interp, enumStr));
        } else if (DictKeyExists(interp, opt, interpCtx->elswitch_allow)) {
            Tcl_Obj *enumStr = Tcl_NewListObj(0, NULL);
            Tcl_ListObjAppendElement(interp, enumStr, Tcl_NewStringObj("Allows ", -1));
            Tcl_ListObjAppendElement(interp, enumStr, EnumStrBuildObj(interp, interpCtx->elswitch_allow, opt));
            Tcl_ListObjAppendElement(interp, enumStr, Tcl_NewStringObj(".", -1));
            Tcl_ListObjAppendElement(interp, constraints, JoinWithEmptySeparator(interp, enumStr));
        }
        if (DictKeyExists(interp, opt, interpCtx->elswitch_forbid)) {
            Tcl_Obj *enumStr = Tcl_NewListObj(0, NULL);
            Tcl_ListObjAppendElement(interp, enumStr, Tcl_NewStringObj("Forbids ", -1));
            Tcl_ListObjAppendElement(interp, enumStr, EnumStrBuildObj(interp, interpCtx->elswitch_forbid, opt));
            Tcl_ListObjAppendElement(interp, enumStr, Tcl_NewStringObj(".", -1));
            Tcl_ListObjAppendElement(interp, constraints, JoinWithEmptySeparator(interp, enumStr));
        }
        // element description building
        Tcl_Size elementDescrLen;
        Tcl_ListObjLength(interp, elementDescr, &elementDescrLen);
        if (elementDescrLen > 0) {
            Tcl_Obj *lastElem = NULL;
            Tcl_ListObjIndex(interp, elementDescr, elementDescrLen - 1, &lastElem);
            Tcl_AppendStringsToObj(lastElem, ".", (char *)NULL);
            Tcl_ListObjAppendElement(interp, combined,
                                     EvaluateStringToTitle(interp, JoinListWithSeparator(interp, elementDescr, " "),
                                                           Tcl_NewIntObj(0), Tcl_NewIntObj(2)));
        }
        Tcl_Obj *helpLoc = NULL;
        if (DICT_GET_IF_EXISTS(interp, opt, interpCtx->elswitch_help, &helpLoc)) {
            Tcl_Obj *helpStringLoc = Tcl_NewListObj(0, NULL);
            Tcl_ListObjAppendElement(interp, helpStringLoc, helpLoc);
            Tcl_ListObjAppendElement(interp, helpStringLoc, Tcl_NewStringObj(".", -1));
            Tcl_ListObjAppendElement(interp, combined, JoinWithEmptySeparator(interp, helpStringLoc));
        }
        Tcl_Size constraintsLen;
        Tcl_ListObjLength(interp, constraints, &constraintsLen);
        if (constraintsLen > 0) {
            combined = MergeTwoLists(interp, combined, constraints);
        }
        Tcl_Obj *defaultLoc = NULL;
        if (DICT_GET_IF_EXISTS(interp, opt, interpCtx->elswitch_default, &defaultLoc) &&
            DictKeyExists(interp, opt, interpCtx->elswitch_argument)) {
            Tcl_Obj *defaultStringLoc = Tcl_NewListObj(0, NULL);
            Tcl_ListObjAppendElement(interp, defaultStringLoc, Tcl_NewStringObj("Default value is ", -1));
            Tcl_ListObjAppendElement(interp, defaultStringLoc, defaultLoc);
            Tcl_ListObjAppendElement(interp, defaultStringLoc, Tcl_NewStringObj(".", -1));
            Tcl_ListObjAppendElement(interp, combined, JoinWithEmptySeparator(interp, defaultStringLoc));
        }
        Tcl_Obj *aliasLoc = NULL;
        if (DICT_GET_IF_EXISTS(interp, opt, interpCtx->elswitch_alias, &aliasLoc)) {
            Tcl_Size aliasLocLen;
            Tcl_ListObjLength(interp, aliasLoc, &aliasLocLen);
            if (aliasLocLen > 1) {
                Tcl_Obj *enumStr = Tcl_NewListObj(0, NULL);
                Tcl_ListObjAppendElement(interp, enumStr, Tcl_NewStringObj("Aliases are ", -1));
                Tcl_ListObjAppendElement(interp, enumStr, EnumStrBuildObj(interp, interpCtx->elswitch_alias, opt));
                Tcl_ListObjAppendElement(interp, enumStr, Tcl_NewStringObj(".", -1));
                Tcl_ListObjAppendElement(interp, combined, JoinWithEmptySeparator(interp, enumStr));
            } else {
                Tcl_Obj *enumStr = Tcl_NewListObj(0, NULL);
                Tcl_ListObjAppendElement(interp, enumStr, Tcl_NewStringObj("Alias is ", -1));
                Tcl_ListObjAppendElement(interp, enumStr, aliasLoc);
                Tcl_ListObjAppendElement(interp, enumStr, Tcl_NewStringObj(".", -1));
                Tcl_ListObjAppendElement(interp, combined, JoinWithEmptySeparator(interp, enumStr));
            }
        }
        if (DictKeyExists(interp, opt, interpCtx->elswitch_catchall)) {
            Tcl_ListObjAppendElement(interp, combined, Tcl_NewStringObj("Collects unassigned arguments.", -1));
        }
        if (DictKeyExists(interp, opt, interpCtx->elswitch_upvar)) {
            Tcl_ListObjAppendElement(interp, combined, Tcl_NewStringObj("Links caller variable.", -1));
        }
        Tcl_Obj *typeLoc;
        if (DICT_GET_IF_EXISTS(interp, opt, interpCtx->elswitch_type, &typeLoc)) {
            Tcl_Obj *typeStr = Tcl_NewListObj(0, NULL);
            Tcl_ListObjAppendElement(interp, typeStr, Tcl_NewStringObj("Type ", -1));
            Tcl_ListObjAppendElement(interp, typeStr, typeLoc);
            Tcl_ListObjAppendElement(interp, typeStr, Tcl_NewStringObj(".", -1));
            Tcl_ListObjAppendElement(interp, combined, JoinWithEmptySeparator(interp, typeStr));
        }
        if (DictKeyExists(interp, opt, interpCtx->elswitch_enum)) {
            Tcl_Obj *enumStr = Tcl_NewListObj(0, NULL);
            Tcl_ListObjAppendElement(interp, enumStr, Tcl_NewStringObj("Value must be one of: ", -1));
            Tcl_ListObjAppendElement(interp, enumStr, EnumStrBuildObj(interp, interpCtx->elswitch_enum, opt));
            Tcl_ListObjAppendElement(interp, enumStr, Tcl_NewStringObj(".", -1));
            Tcl_ListObjAppendElement(interp, combined, JoinWithEmptySeparator(interp, enumStr));
        }
        if (DictKeyExists(interp, opt, interpCtx->elswitch_imply)) {
            Tcl_ListObjAppendElement(interp, combined, Tcl_NewStringObj("Expects two arguments.", -1));
        }
        if (strcmp("switch", Tcl_GetString(type)) == 0) {
            Tcl_Size combinedLen;
            Tcl_ListObjLength(interp, combined, &combinedLen);
            if (combinedLen > 0) {
                Tcl_Obj *combinedStr = Tcl_NewListObj(0, NULL);
                Tcl_ListObjAppendElement(interp, combinedStr, Tcl_DuplicateObj(interpCtx->misc_dashStrObj));
                Tcl_ListObjAppendElement(interp, combinedStr, name);
                Tcl_ListObjAppendElement(interp, combinedStr, Tcl_NewStringObj(" - ", -1));
                Tcl_ListObjAppendElement(interp, combinedStr, JoinListWithSeparator(interp, combined, " "));
                combined = JoinWithEmptySeparator(interp, combinedStr);
            } else {
                Tcl_Obj *combinedStr = Tcl_NewListObj(0, NULL);
                Tcl_ListObjAppendElement(interp, combinedStr, Tcl_DuplicateObj(interpCtx->misc_dashStrObj));
                Tcl_ListObjAppendElement(interp, combinedStr, name);
                combined = JoinWithEmptySeparator(interp, combinedStr);
            }
            Tcl_Obj *descrSwitchesStr = NULL;
            descrSwitchesStr = EvaluateAdjust(interp, combined, 72);
            descrSwitchesStr = EvaluateIndent(interp, descrSwitchesStr, Tcl_NewStringObj("    ", -1), Tcl_NewIntObj(1));

            descrSwitchesStr =
                EvaluateIndent(interp, descrSwitchesStr, Tcl_NewStringObj("        ", -1), Tcl_NewIntObj(0));

            Tcl_ListObjAppendElement(interp, descriptionSwitches, descrSwitchesStr);
        } else {
            Tcl_Size combinedLen;
            Tcl_ListObjLength(interp, combined, &combinedLen);
            if (combinedLen > 0) {
                Tcl_Obj *combinedStr = Tcl_NewListObj(0, NULL);
                Tcl_ListObjAppendElement(interp, combinedStr, name);
                Tcl_ListObjAppendElement(interp, combinedStr, Tcl_NewStringObj(" - ", -1));
                Tcl_ListObjAppendElement(interp, combinedStr, JoinListWithSeparator(interp, combined, " "));
                combined = JoinWithEmptySeparator(interp, combinedStr);
            } else {
                Tcl_Obj *combinedStr = Tcl_NewListObj(0, NULL);
                Tcl_ListObjAppendElement(interp, combinedStr, name);
                combined = JoinWithEmptySeparator(interp, combinedStr);
            }
            Tcl_Obj *descrParametersStr = NULL;
            descrParametersStr = EvaluateAdjust(interp, combined, 72);
            descrParametersStr =
                EvaluateIndent(interp, descrParametersStr, Tcl_NewStringObj("    ", -1), Tcl_NewIntObj(1));
            descrParametersStr =
                EvaluateIndent(interp, descrParametersStr, Tcl_NewStringObj("        ", -1), Tcl_NewIntObj(0));
            Tcl_ListObjAppendElement(interp, descriptionParameters, descrParametersStr);
        }
        Tcl_DictObjNext(&search, &name, &opt, &done);
    }
    Tcl_DictObjDone(&search);
    Tcl_Obj *helpListLoc = Tcl_NewListObj(0, NULL);
    Tcl_Obj *descrSwitchesStr = NULL;
    Tcl_ListObjAppendElement(
        interp, helpListLoc,
        Tcl_NewStringObj("-help - Help switch, when provided, forces ignoring all other switches and "
                         "parameters, prints the help message to stdout, and returns up to ",
                         -1));
    Tcl_ListObjAppendElement(interp, helpListLoc, helpLevel);
    Tcl_ListObjAppendElement(interp, helpListLoc, Tcl_NewStringObj(" levels above the current level.", -1));
    descrSwitchesStr = JoinWithEmptySeparator(interp, helpListLoc);
    descrSwitchesStr = EvaluateAdjust(interp, descrSwitchesStr, 72);
    descrSwitchesStr = EvaluateIndent(interp, descrSwitchesStr, Tcl_NewStringObj("    ", -1), Tcl_NewIntObj(1));
    descrSwitchesStr = EvaluateIndent(interp, descrSwitchesStr, Tcl_NewStringObj("        ", -1), Tcl_NewIntObj(0));
    Tcl_ListObjAppendElement(interp, descriptionSwitches, descrSwitchesStr);
    description = EvaluateAdjust(interp, JoinListWithSeparator(interp, description, " "), 80);
    Tcl_Size descrSwitchesLen, descrParametersLen;
    Tcl_ListObjLength(interp, descriptionSwitches, &descrSwitchesLen);
    Tcl_ListObjLength(interp, descriptionParameters, &descrParametersLen);
    Tcl_Obj *finalDescrList = Tcl_NewListObj(0, NULL);
    Tcl_IncrRefCount(finalDescrList);
    if ((descrSwitchesLen > 0) && (descrParametersLen > 0)) {
        Tcl_ListObjAppendElement(interp, finalDescrList, description);
        Tcl_ListObjAppendElement(
            interp, finalDescrList,
            EvaluateIndent(interp, Tcl_NewStringObj("Switches:", -1), Tcl_NewStringObj("    ", -1), Tcl_NewIntObj(0)));
        finalDescrList = MergeTwoLists(interp, finalDescrList, descriptionSwitches);

        Tcl_ListObjAppendElement(interp, finalDescrList,
                                 EvaluateIndent(interp, Tcl_NewStringObj("Parameters:", -1),
                                                Tcl_NewStringObj("    ", -1), Tcl_NewIntObj(0)));
        finalDescrList = MergeTwoLists(interp, finalDescrList, descriptionParameters);
        Tcl_Obj *resultObj = NULL;
        EvalStringMap(interp, interpCtx->list_helpGenSubstNames, JoinListWithSeparator(interp, finalDescrList, "\n"),
                      &resultObj);
        finalDescrList = EvaluateStringToTitle(interp, resultObj, Tcl_NewIntObj(0), Tcl_NewIntObj(1));
    } else if (descrSwitchesLen > 0) {
        Tcl_ListObjAppendElement(interp, finalDescrList, description);
        Tcl_ListObjAppendElement(
            interp, finalDescrList,
            EvaluateIndent(interp, Tcl_NewStringObj("Switches:", -1), Tcl_NewStringObj("    ", -1), Tcl_NewIntObj(0)));
        finalDescrList = MergeTwoLists(interp, finalDescrList, descriptionSwitches);
        Tcl_Obj *resultObj = NULL;
        EvalStringMap(interp, interpCtx->list_helpGenSubstNames, JoinListWithSeparator(interp, finalDescrList, "\n"),
                      &resultObj);
        finalDescrList = EvaluateStringToTitle(interp, resultObj, Tcl_NewIntObj(0), Tcl_NewIntObj(1));
    } else if (descrParametersLen > 0) {
        Tcl_ListObjAppendElement(interp, finalDescrList, description);
        Tcl_ListObjAppendElement(interp, finalDescrList,
                                 EvaluateIndent(interp, Tcl_NewStringObj("Parameters:", -1),
                                                Tcl_NewStringObj("    ", -1), Tcl_NewIntObj(0)));
        finalDescrList = MergeTwoLists(interp, finalDescrList, descriptionParameters);
        Tcl_Obj *resultObj = NULL;
        EvalStringMap(interp, interpCtx->list_helpGenSubstNames, JoinListWithSeparator(interp, finalDescrList, "\n"),
                      &resultObj);
        finalDescrList = EvaluateStringToTitle(interp, resultObj, Tcl_NewIntObj(0), Tcl_NewIntObj(1));
    } else {
        Tcl_Obj *resultObj = NULL;
        EvalStringMap(interp, interpCtx->list_helpGenSubstNames, providedHelp, &resultObj);
        finalDescrList = EvaluateStringToTitle(interp, resultObj, Tcl_NewIntObj(0), Tcl_NewIntObj(1));
    }
    return finalDescrList;
}

//** Validating helper functions
//***    ValidateHelper function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * ValidateHelper --
 *
 *      Validates argument values for a given switch or parameter using either:
 *      - an `-enum` list with optional prefix matching, or
 *      - a `-validate` expression with variable substitution.
 *
 *      If neither `-enum` nor `-validate` is present, the input list is returned as-is.
 *
 * Parameters:
 *      Tcl_Interp *interp             - input: target interpreter
 *      GlobalSwitchesContext *ctx     - input: pointer to global switches context (used to determine prefix behavior)
 *      Tcl_Obj *nameObj               - input: Tcl_Obj representing the name of the switch/parameter being validated
 *      Tcl_Obj *optDictObj            - input: Tcl_Obj representing the dictionary containing the option definition
 *      Tcl_Obj *argObj                - input: Tcl_Obj argument to validate
 *      ArgparseInterpCtx *interpCtx   - input: interpreter context structure
 *      int listFlag                   - input: if 1, argument expected to be a list
 *      Tcl_Obj **resultPtr            - output: pointer to the list of validated (and possibly substituted) values
 *
 * Results:
 *      Code TCL_OK - validation passed; *resultPtr is set to a list of validated arguments  
 *      Code TCL_ERROR - validation failed or a Tcl error occurred; result is stored in the interpreter result
 *
 * Side Effects:
 *      - May call `EvalPrefixMatch()` if `-enum` is present  
 *      - May evaluate a Tcl expression using `Tcl_ExprBooleanObj` for custom validation logic via `-validate`  
 *      - Temporary Tcl variables `arg`, `name`, and `opt` are set/unset during validation  
 *      - If `-errormsg` is defined, it is used as the error message with variable substitution (`Tcl_SubstObj`)  
 *      - If `-validateMsg` is defined, it is included in the error message on failure
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
int ValidateHelper(Tcl_Interp *interp, GlobalSwitchesContext *ctx, Tcl_Obj *nameObj, Tcl_Obj *optDictObj,
                   Tcl_Obj *argObj, ArgparseInterpCtx *interpCtx, int listFlag, Tcl_Obj **resultPtr) {
    int enumExists = 0;
    Tcl_Obj *enumList = NULL;
    int validateExists = 0;
    Tcl_Obj *validateCmd = NULL;
    Tcl_Obj *errormsgObj = NULL;
    Tcl_Obj *validateMsgObj = NULL;
    Tcl_Obj *enumPrefixListObj = NULL;
    if (Tcl_IsShared(nameObj)) {
        nameObj = Tcl_DuplicateObj(nameObj);
    }
    if (DICT_GET_IF_EXISTS(interp, optDictObj, interpCtx->elswitch_enum, &enumList)) {
        enumExists = 1;
    }
    if (DICT_GET_IF_EXISTS(interp, optDictObj, interpCtx->elswitch_validate, &validateCmd)) {
        validateExists = 1;
    }
    Tcl_DictObjGet(interp, optDictObj, interpCtx->misc_validateMsgStrObj, &validateMsgObj);
    if (enumExists && listFlag) {
        enumPrefixListObj = Tcl_NewListObj(0, NULL);
    }
    if (listFlag) {
        Tcl_Size argc;
        Tcl_Obj **argv;
        Tcl_ListObjGetElements(interp, argObj, &argc, &argv);
        for (Tcl_Size i = 0; i < argc; ++i) {
            if (enumExists) {
                Tcl_Obj *messageStr = Tcl_ObjPrintf("%s value", Tcl_GetString(nameObj));
                Tcl_IncrRefCount(messageStr);
                int useExact = 0;
                if (HAS_GLOBAL_SWITCH(ctx, GLOBAL_SWITCH_EXACT)) {
                    useExact = 1;
                }
                Tcl_Obj *prefixResult = NULL;
                int code = EvalPrefixMatch(interp, enumList, argv[i], useExact, 1, messageStr, 1, &prefixResult);
                if (code != TCL_OK) {
                    Tcl_DecrRefCount(messageStr);
                    return TCL_ERROR;
                }
                Tcl_DecrRefCount(messageStr);
                Tcl_ListObjAppendElement(interp, enumPrefixListObj, prefixResult);
            } else if (validateExists) {
                Tcl_ObjSetVar2(interp, Tcl_NewStringObj("opt", -1), NULL, optDictObj, 0);
                Tcl_ObjSetVar2(interp, Tcl_NewStringObj("name", -1), NULL, nameObj, 0);
                Tcl_ObjSetVar2(interp, Tcl_NewStringObj("arg", -1), NULL, argv[i], 0);
                int result = 0;
                int code = Tcl_ExprBooleanObj(interp, validateCmd, &result);
                if (code == TCL_ERROR || !result) {
                    if (DICT_GET_IF_EXISTS(interp, optDictObj, interpCtx->elswitch_errormsg, &errormsgObj)) {
                        Tcl_Obj *substErr = Tcl_DuplicateObj(errormsgObj);
                        Tcl_IncrRefCount(substErr);

                        Tcl_Obj *substErrResult =
                            Tcl_SubstObj(interp, substErr, TCL_SUBST_VARIABLES | TCL_SUBST_COMMANDS);
                        Tcl_DecrRefCount(substErr);
                        if (substErrResult != NULL) {
                            Tcl_SetObjResult(interp, substErrResult);
                            return TCL_ERROR;
                        } else {
                            Tcl_SetObjResult(interp, Tcl_GetObjResult(interp));
                            return TCL_ERROR;
                        }
                    }
                    Tcl_Obj *errMsg =
                        Tcl_ObjPrintf("%s value \"%s\" fails %s", Tcl_GetString(nameObj), Tcl_GetString(argv[i]),
                                      validateMsgObj ? Tcl_GetString(validateMsgObj) : "validation");
                    Tcl_UnsetVar(interp, "arg", 0);
                    Tcl_UnsetVar(interp, "name", 0);
                    Tcl_UnsetVar(interp, "opt", 0);
                    Tcl_SetObjResult(interp, errMsg);
                    return TCL_ERROR;
                }
                Tcl_UnsetVar(interp, "arg", 0);
                Tcl_UnsetVar(interp, "name", 0);
                Tcl_UnsetVar(interp, "opt", 0);
            }
        }
    } else {
        if (enumExists) {
            Tcl_Obj *messageStr = Tcl_ObjPrintf("%s value", Tcl_GetString(nameObj));
            Tcl_IncrRefCount(messageStr);
            int useExact = 0;
            if (HAS_GLOBAL_SWITCH(ctx, GLOBAL_SWITCH_EXACT)) {
                useExact = 1;
            }
            Tcl_Obj *prefixResult = NULL;
            int code = EvalPrefixMatch(interp, enumList, argObj, useExact, 1, messageStr, 1, &prefixResult);
            if (code != TCL_OK) {
                Tcl_DecrRefCount(messageStr);
                return TCL_ERROR;
            }
            Tcl_DecrRefCount(messageStr);
            *resultPtr = prefixResult;
            return TCL_OK;
        } else if (validateExists) {
            Tcl_ObjSetVar2(interp, Tcl_NewStringObj("opt", -1), NULL, optDictObj, 0);
            Tcl_ObjSetVar2(interp, Tcl_NewStringObj("name", -1), NULL, nameObj, 0);
            Tcl_ObjSetVar2(interp, Tcl_NewStringObj("arg", -1), NULL, argObj, 0);
            int result = 0;
            int code = Tcl_ExprBooleanObj(interp, validateCmd, &result);
            if (code == TCL_ERROR || !result) {
                if (DICT_GET_IF_EXISTS(interp, optDictObj, interpCtx->elswitch_errormsg, &errormsgObj)) {
                    Tcl_Obj *substErr = Tcl_DuplicateObj(errormsgObj);
                    Tcl_IncrRefCount(substErr);

                    Tcl_Obj *substErrResult = Tcl_SubstObj(interp, substErr, TCL_SUBST_VARIABLES | TCL_SUBST_COMMANDS);
                    Tcl_DecrRefCount(substErr);
                    if (substErrResult != NULL) {
                        Tcl_SetObjResult(interp, substErrResult);
                        return TCL_ERROR;
                    } else {
                        Tcl_SetObjResult(interp, Tcl_GetObjResult(interp));
                        return TCL_ERROR;
                    }
                }
                Tcl_Obj *errMsg =
                    Tcl_ObjPrintf("%s value \"%s\" fails %s", Tcl_GetString(nameObj), Tcl_GetString(argObj),
                                  validateMsgObj ? Tcl_GetString(validateMsgObj) : "validation");
                Tcl_UnsetVar(interp, "arg", 0);
                Tcl_UnsetVar(interp, "name", 0);
                Tcl_UnsetVar(interp, "opt", 0);
                Tcl_SetObjResult(interp, errMsg);
                return TCL_ERROR;
            }
            Tcl_UnsetVar(interp, "arg", 0);
            Tcl_UnsetVar(interp, "name", 0);
            Tcl_UnsetVar(interp, "opt", 0);
        }
    }
    if (enumExists && listFlag) {
        *resultPtr = enumPrefixListObj;
    } else {
        *resultPtr = argObj;
    }
    return TCL_OK;
}

//***    TypeChecker function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * TypeChecker --
 *
 *      Validate a list of argument values against a specified type declared in the option dictionary.
 *      Supports built-in types ("int", "double", "boolean", "digit") as well as extended Tcl `string is` types.
 *
 * Parameters:
 *      Tcl_Interp *interp           - input: target interpreter
 *      Tcl_Obj *nameObj             - input: pointer to Tcl_Obj representing the name of the argument or option
 *      Tcl_Obj *optDictObj          - input: pointer to Tcl_Obj dictionary that may contain the `-type` key
 *      Tcl_Obj *argObj              - input: pointer to Tcl_Obj list of argument values to validate
 *      ArgparseInterpCtx *interpCtx - input: interpreter context structure
 *      int listFlag                 - input: if 1, argument expected to be a list
 *      Tcl_Obj **resultPtr          - output: optional pointer to store the original `argObj` if validation passes
 *
 * Results:
 *      Code TCL_OK - all values match the specified type, or no `-type` key is defined  
 *      Code TCL_ERROR - a value does not match the declared type; sets interpreter result with an error message
 *
 * Side Effects:
 *      - If `-type` is set and does not match a built-in handler, falls back to evaluating `string is <type> -strict`  
 *      - Interpreter result is set with a descriptive error message on failure  
 *      - Error code is set to `"TCL", "TYPE", "MISMATCH"` on type mismatch
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
int TypeChecker(Tcl_Interp *interp, Tcl_Obj *nameObj, Tcl_Obj *optDictObj, Tcl_Obj *argObj,
                ArgparseInterpCtx *interpCtx, int listFlag, Tcl_Obj **resultPtr) {
    Tcl_Obj *typeObj = NULL;
    Tcl_DictObjGet(interp, optDictObj, interpCtx->elswitch_type, &typeObj);
    if (typeObj != NULL) {
        const char *typeStr = Tcl_GetString(typeObj);
        if (listFlag) {
            Tcl_Size argc;
            Tcl_Obj **argv;
            Tcl_ListObjGetElements(interp, argObj, &argc, &argv);
            for (Tcl_Size i = 0; i < argc; ++i) {
                int isValid = 0;
                if (strcmp(typeStr, "integer") == 0) {
                    int dummy;
                    isValid = (Tcl_GetIntFromObj(interp, argv[i], &dummy) == TCL_OK);
                } else if (strcmp(typeStr, "double") == 0) {
                    double dummy;
                    isValid = (Tcl_GetDoubleFromObj(interp, argv[i], &dummy) == TCL_OK);
                } else if (strcmp(typeStr, "digit") == 0) {
                    isValid = Tcl_StringMatch(Tcl_GetString(argv[i]), "[0-9]*");
                } else if (strcmp(typeStr, "boolean") == 0) {
                    int dummy;
                    isValid = (Tcl_GetBooleanFromObj(interp, argv[i], &dummy) == TCL_OK);
                } else {
                    // Fall back to string is type -strict (slower)
                    Tcl_Obj *cmd[5];
                    cmd[0] = Tcl_NewStringObj("string", -1);
                    cmd[1] = Tcl_NewStringObj("is", -1);
                    cmd[2] = typeObj;
                    cmd[3] = Tcl_NewStringObj("-strict", -1);
                    cmd[4] = argv[i];
                    for (int j = 0; j < 5; ++j)
                        Tcl_IncrRefCount(cmd[j]);
                    if (Tcl_EvalObjv(interp, 5, cmd, 0) != TCL_OK) {
                        for (int j = 0; j < 5; ++j)
                            Tcl_DecrRefCount(cmd[j]);
                        return TCL_ERROR;
                    }
                    Tcl_GetBooleanFromObj(interp, Tcl_GetObjResult(interp), &isValid);
                    Tcl_ResetResult(interp);
                    for (int j = 0; j < 5; ++j)
                        Tcl_DecrRefCount(cmd[j]);
                }
                if (!isValid) {
                    Tcl_Obj *errMsg = Tcl_ObjPrintf("%s value \"%s\" is not of the type %s", Tcl_GetString(nameObj),
                                                    Tcl_GetString(argv[i]), typeStr);
                    Tcl_SetObjResult(interp, errMsg);
                    return TCL_ERROR;
                }
            }
        } else {
            int isValid = 0;
            if (strcmp(typeStr, "integer") == 0) {
                int dummy;
                isValid = (Tcl_GetIntFromObj(interp, argObj, &dummy) == TCL_OK);
            } else if (strcmp(typeStr, "double") == 0) {
                double dummy;
                isValid = (Tcl_GetDoubleFromObj(interp, argObj, &dummy) == TCL_OK);
            } else if (strcmp(typeStr, "digit") == 0) {
                isValid = Tcl_StringMatch(Tcl_GetString(argObj), "[0-9]*");
            } else if (strcmp(typeStr, "boolean") == 0) {
                int dummy;
                isValid = (Tcl_GetBooleanFromObj(interp, argObj, &dummy) == TCL_OK);
            } else {
                // Fall back to string is type -strict (slower)
                Tcl_Obj *cmd[5];
                cmd[0] = Tcl_NewStringObj("string", -1);
                cmd[1] = Tcl_NewStringObj("is", -1);
                cmd[2] = typeObj;
                cmd[3] = Tcl_NewStringObj("-strict", -1);
                cmd[4] = argObj;
                for (int j = 0; j < 5; ++j)
                    Tcl_IncrRefCount(cmd[j]);
                if (Tcl_EvalObjv(interp, 5, cmd, 0) != TCL_OK) {
                    for (int j = 0; j < 5; ++j)
                        Tcl_DecrRefCount(cmd[j]);
                    return TCL_ERROR;
                }
                Tcl_GetBooleanFromObj(interp, Tcl_GetObjResult(interp), &isValid);
                Tcl_ResetResult(interp);
                for (int j = 0; j < 5; ++j)
                    Tcl_DecrRefCount(cmd[j]);
            }
            if (!isValid) {
                Tcl_Obj *errMsg = Tcl_ObjPrintf("%s value \"%s\" is not of the type %s", Tcl_GetString(nameObj),
                                                Tcl_GetString(argObj), typeStr);
                Tcl_SetObjResult(interp, errMsg);
                return TCL_ERROR;
            }
        }
    }
    *resultPtr = argObj;
    return TCL_OK;
}

//** Argument definition manipulating functions
//***    ParseElementDefinitions function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * ParseElementDefinitions --
 *
 *      Parse a list of element definitions (switches and parameters) and populate the argument definition context
 *      with metadata for each element, including validation rules, constraints, aliases, and derived attributes.
 *      This function supports both full (`-switch`, `-parameter`, etc.) and shorthand notation.
 *
 * Parameters:
 *      Tcl_Interp *interp             - input: target interpreter
 *      GlobalSwitchesContext *ctx     - input: pointer to global parsing switches context
 *      Tcl_Obj *definition            - input: list of element definitions, each being a list of the form:
 *                                       {name -switch -alias ...} or shorthand e.g., {-a|alias:name!?}
 *      ArgumentDefinition *argCtx     - input/output: structure that accumulates parsed switch/parameter definitions
 *      ArgparseInterpCtx *interpCtx   - input: interpreter context structure
 *
 * Results:
 *      Code TCL_OK - all element definitions were successfully parsed and validated  
 *      Code TCL_ERROR - an error occurred during parsing or validation; interpreter result is set with a message
 *
 * Side Effects:
 *      - Populates:
 *          - `argCtx->defDict`: main dictionary mapping element names to option metadata
 *          - `argCtx->aliasesDict`: maps aliases to canonical switch names
 *          - `argCtx->orderList`: ordered list of parameter names
 *          - `argCtx->switchesList`: flat list of defined switches (including aliases)
 *          - `argCtx->upvarsDict`: maps upvar target variables to element names
 *          - `argCtx->omittedDict`: initially includes all element names (used to track unassigned inputs)
 *      - Performs validation checks:
 *          - Mutual exclusivity and required pair switches
 *          - Shorthand flags (e.g., `!`, `?`, `=`, `*`, `^`) are expanded into standard switches
 *          - Auto-inference of certain switches (e.g., `-argument` implied by `-optional`)
 *          - Conflicts between global options like `-inline` and element-specific options like `-keep`
 *          - Checks for duplicate names, aliases, and upvar collisions
 *      - Uses prefix matching for element switches via `EvalPrefixMatch`
 *      - Expands enums and validators from global references if enabled (via `-enum` or `-validate`)
 *      - Rewrites `-boolean` into `-default 0 -value 1`
 *      - Applies substitution templates for implicit `-key` names using `-template`
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
int ParseElementDefinitions(Tcl_Interp *interp, GlobalSwitchesContext *ctx, Tcl_Obj *definition,
                            ArgumentDefinition *argCtx, ArgparseInterpCtx *interpCtx) {
    Tcl_RegExp regexpShorthand = Tcl_GetRegExpFromObj(
        interp, Tcl_NewStringObj("^(?:(-)(?:(.*)\\|)?)?(\\w[\\w-]*)([=?!*^]*)$", -1), TCL_REG_ADVANCED);
    Tcl_RegExp regexpSwitchName = Tcl_GetRegExpFromObj(interp, Tcl_NewStringObj("^\\w[\\w-]*$", -1), TCL_REG_ADVANCED);
    Tcl_RegExp regexpSwitchAlias =
        Tcl_GetRegExpFromObj(interp, Tcl_NewStringObj("^\\w[\\w-]*( \\w[\\w-]*)*$", -1), TCL_REG_ADVANCED);
    Tcl_Obj *prefixResult = NULL;
    if (argCtx->catchall != NULL) {
        argCtx->catchall = NULL;
    }
//****       Read element definition switches
    Tcl_Size defListLen;
    Tcl_Obj **defListElems;
    if (Tcl_ListObjGetElements(interp, definition, &defListLen, &defListElems) != TCL_OK) {
        Tcl_SetObjResult(interp, Tcl_NewStringObj("error getting definition list", -1));
        return TCL_ERROR;
    }
    for (Tcl_Size i = 0; i < defListLen; ++i) {
        Tcl_Size elemListLen;
        Tcl_Obj **elemListElems;
        Tcl_Obj *switchName = NULL;
        if (Tcl_ListObjGetElements(interp, defListElems[i], &elemListLen, &elemListElems) != TCL_OK) {
            Tcl_SetObjResult(interp, Tcl_NewStringObj("error getting element definition list", -1));
            return TCL_ERROR;
        }
        Tcl_Obj *optDict = Tcl_NewDictObj();
        for (Tcl_Size j = 1; j < elemListLen; ++j) {
            if (PrefixMatch(interp, elementSwitches, elemListElems[j], 1, 1, "option", 1, &prefixResult) == TCL_ERROR) {
                Tcl_SetObjResult(interp, prefixResult);
                return TCL_ERROR;
            }
            Tcl_Size len;
            const char *str = Tcl_GetStringFromObj(prefixResult, &len);
            SAFE_DECR_REF_AND_NULL(prefixResult);
            switchName = Tcl_NewStringObj(str + 1, len - 1);
            if (PrefixMatch(interp, elementSwitchesWithArgsNames, switchName, 1, 0, NULL, 0, NULL) == TCL_ERROR) {
//****       Process switches without arguments
                Tcl_DictObjPut(interp, optDict, switchName, interpCtx->misc_emptyStrObj);
            } else if (j == (elemListLen - 1)) {
                Tcl_Obj *msg = Tcl_DuplicateObj(interpCtx->misc_dashStrObj);
                Tcl_AppendStringsToObj(msg, Tcl_GetString(switchName), " requires an argument", NULL);
                Tcl_SetObjResult(interp, msg);
                return TCL_ERROR;
            } else {
//****       Process switches with arguments
                j++;
                Tcl_DictObjPut(interp, optDict, switchName, elemListElems[j]);
            }
        }
//****       Process the first element of the element definition
        Tcl_Obj *name = NULL;
        Tcl_Obj *matchList = NULL;
        if (elemListLen == 0) {
            Tcl_SetObjResult(interp, Tcl_NewStringObj("element definition cannot be empty", -1));
            return TCL_ERROR;
        } else if (DictKeyExists(interp, optDict, interpCtx->elswitch_switch) &&
                   DictKeyExists(interp, optDict, interpCtx->elswitch_parameter)) {
            Tcl_SetObjResult(interp, Tcl_NewStringObj("-switch and -parameter conflict", -1));
            return TCL_ERROR;
        } else if (HAS_GLOBAL_SWITCH(ctx, GLOBAL_SWITCH_INLINE) &&
                   DictKeyExists(interp, optDict, interpCtx->elswitch_keep)) {
            Tcl_SetObjResult(interp, Tcl_NewStringObj("-inline and -keep conflict", -1));
            return TCL_ERROR;
        } else if (!DictKeyExists(interp, optDict, interpCtx->elswitch_switch) &&
                   !DictKeyExists(interp, optDict, interpCtx->elswitch_parameter)) {
            /* if -switch and -parameter are not used, parse shorthand syntax */
            Tcl_Obj *minus = NULL;
            Tcl_Obj *alias = NULL;
            Tcl_Obj *flags = NULL;
            if (EvalMatchRegexpGroups(interp, regexpShorthand, elemListElems[0], interpCtx, &matchList) == TCL_ERROR) {
                Tcl_Obj *msg = Tcl_NewStringObj("bad element shorthand: ", -1);
                Tcl_AppendStringsToObj(msg, Tcl_GetString(elemListElems[0]), NULL);
                Tcl_SetObjResult(interp, msg);
                return TCL_ERROR;
            }
            Tcl_ListObjIndex(interp, matchList, 1, &minus);
            Tcl_ListObjIndex(interp, matchList, 2, &alias);
            Tcl_ListObjIndex(interp, matchList, 3, &name);
            Tcl_ListObjIndex(interp, matchList, 4, &flags);
            if (Tcl_GetCharLength(minus) > 0) {
                Tcl_DictObjPut(interp, optDict, interpCtx->elswitch_switch, interpCtx->misc_emptyStrObj);
            } else {
                Tcl_DictObjPut(interp, optDict, interpCtx->elswitch_parameter, interpCtx->misc_emptyStrObj);
            }
            if (Tcl_GetCharLength(alias) > 0) {
                Tcl_DictObjPut(interp, optDict, interpCtx->elswitch_alias,
                               SplitString(interp, alias, Tcl_NewStringObj("|", -1), interpCtx));
            }
            Tcl_Obj *flagsList = SplitString(interp, flags, Tcl_NewStringObj("", 0), interpCtx);
            Tcl_Size flagsListLen;
            Tcl_Obj **flagsListElems;
            Tcl_ListObjGetElements(interp, flagsList, &flagsListLen, &flagsListElems);
            for (Tcl_Size k = 0; k < flagsListLen; ++k) {
                if (strcmp(Tcl_GetString(flagsListElems[k]), "=") == 0) {
                    Tcl_DictObjPut(interp, optDict, interpCtx->elswitch_argument, interpCtx->misc_emptyStrObj);
                } else if (strcmp(Tcl_GetString(flagsListElems[k]), "?") == 0) {
                    Tcl_DictObjPut(interp, optDict, interpCtx->elswitch_optional, interpCtx->misc_emptyStrObj);
                } else if (strcmp(Tcl_GetString(flagsListElems[k]), "!") == 0) {
                    Tcl_DictObjPut(interp, optDict, interpCtx->elswitch_required, interpCtx->misc_emptyStrObj);
                } else if (strcmp(Tcl_GetString(flagsListElems[k]), "*") == 0) {
                    Tcl_DictObjPut(interp, optDict, interpCtx->elswitch_catchall, interpCtx->misc_emptyStrObj);
                } else if (strcmp(Tcl_GetString(flagsListElems[k]), "^") == 0) {
                    Tcl_DictObjPut(interp, optDict, interpCtx->elswitch_upvar, interpCtx->misc_emptyStrObj);
                }
            }
            SAFE_DECR_REF(minus);
            SAFE_DECR_REF(flags);
            SAFE_DECR_REF(alias);
            SAFE_DECR_REF(flagsList);
        } else if (EvalMatchRegexpGroups(interp, regexpSwitchName, elemListElems[0], interpCtx, &matchList) ==
                   TCL_ERROR) {
            Tcl_Obj *msg = Tcl_NewStringObj("bad element name: ", -1);
            Tcl_AppendStringsToObj(msg, Tcl_GetString(elemListElems[0]), NULL);
            Tcl_SetObjResult(interp, msg);
            return TCL_ERROR;
        } else {
            name = elemListElems[0];
        }

//****       Check for collisions
        if (DictKeyExists(interp, argCtx->defDict, name)) {
            Tcl_Obj *msg = Tcl_NewStringObj("element name collision: ", -1);
            Tcl_AppendStringsToObj(msg, Tcl_GetString(name), NULL);
            Tcl_SetObjResult(interp, msg);
            return TCL_ERROR;
        }
        if (DictKeyExists(interp, optDict, interpCtx->elswitch_switch)) {
//****       Add -argument switch if particular switches are presented
            /* -optional, -required, -catchall, -upvar and -type imply -argument when used with switches */
            for (Tcl_Size l = 0; l < ELEMENT_SWITCH_COUNT_IMPLY_ARG; ++l) {
                if (DictKeyExists(interp, optDict, Tcl_NewStringObj(elementSwitchesImplyElementArg[l], -1))) {
                    Tcl_DictObjPut(interp, optDict, interpCtx->elswitch_argument, interpCtx->misc_emptyStrObj);
                }
            }
        } else {
//****       Add -required switch for parameters
            if ((DictKeyExists(interp, optDict, interpCtx->elswitch_catchall) ||
                 DictKeyExists(interp, optDict, interpCtx->elswitch_optional)) &&
                !DictKeyExists(interp, optDict, interpCtx->elswitch_required)) {
                Tcl_DictObjPut(interp, optDict, interpCtx->elswitch_optional, interpCtx->misc_emptyStrObj);
            } else {
                Tcl_DictObjPut(interp, optDict, interpCtx->elswitch_required, interpCtx->misc_emptyStrObj);
            }
        }
//****       Check requirements and conflicts
        for (Tcl_Size m = 0; m < ELEMENT_SWITCH_COUNT_REQPAIRS; ++m) {
            if (DictKeyExists(interp, optDict, Tcl_NewStringObj(requireSwitchesPair0[m], -1)) &&
                !DictKeyExists(interp, optDict, Tcl_NewStringObj(requireSwitchesPair1[m], -1))) {
                Tcl_Obj *msg = Tcl_DuplicateObj(interpCtx->misc_dashStrObj);
                Tcl_AppendStringsToObj(msg, requireSwitchesPair0[m], " requires -", requireSwitchesPair1[m], NULL);
                Tcl_SetObjResult(interp, msg);
                return TCL_ERROR;
            }
        }
        for (int n = 0; n < ELEMENT_SWITCH_COUNT_CONFLICT; ++n) {
            if (DictKeyExists(interp, optDict, Tcl_NewStringObj(conflictSwitches[n], -1))) {
                for (int p = 0; p < cSwRowSizes[n]; ++p) {
                    if (DictKeyExists(interp, optDict, Tcl_NewStringObj(conflictSwitchesRows[n][p], -1))) {
                        Tcl_Obj *msg = Tcl_DuplicateObj(interpCtx->misc_dashStrObj);
                        Tcl_AppendStringsToObj(msg, conflictSwitches[n], " and -", conflictSwitchesRows[n][p],
                                               " conflict", NULL);
                        Tcl_SetObjResult(interp, msg);
                        return TCL_ERROR;
                    }
                }
            }
        }
        if (HAS_GLOBAL_SWITCH(ctx, GLOBAL_SWITCH_INLINE) && DictKeyExists(interp, optDict, interpCtx->elswitch_upvar)) {
            Tcl_SetObjResult(interp, Tcl_NewStringObj("-upvar and -inline conflict", -1));
            return TCL_ERROR;
        }
//****       Check for disallowed combinations
        for (int n = 0; n < ELEMENT_SWITCH_COUNT_DISALLOW; ++n) {
            Tcl_Obj *firstSwitch = Tcl_NewStringObj(disallowedSwitchesRows[n][0], -1);
            Tcl_Obj *secondSwitch = Tcl_NewStringObj(disallowedSwitchesRows[n][1], -1);
            Tcl_Obj *thirdSwitch = Tcl_NewStringObj(disallowedSwitchesRows[n][2], -1);
            if (DictKeyExists(interp, optDict, firstSwitch) && DictKeyExists(interp, optDict, secondSwitch) &&
                DictKeyExists(interp, optDict, thirdSwitch)) {
                Tcl_Obj *msg = Tcl_DuplicateObj(interpCtx->misc_dashStrObj);
                Tcl_AppendStringsToObj(msg, disallowedSwitchesRows[n][0], " -", disallowedSwitchesRows[n][1], " -",
                                       disallowedSwitchesRows[n][2], " is a disallowed combination", NULL);
                Tcl_SetObjResult(interp, msg);
                return TCL_ERROR;
            }
        }
//****       Replace -boolean with "-default 0 -value 1"
        if ((HAS_GLOBAL_SWITCH(ctx, GLOBAL_SWITCH_BOOLEAN) &&
             DictKeyExists(interp, optDict, interpCtx->elswitch_switch) &&
             !DictKeyExists(interp, optDict, interpCtx->elswitch_argument) &&
             !DictKeyExists(interp, optDict, interpCtx->elswitch_upvar) &&
             !DictKeyExists(interp, optDict, interpCtx->elswitch_default) &&
             !DictKeyExists(interp, optDict, interpCtx->elswitch_value) &&
             !DictKeyExists(interp, optDict, interpCtx->elswitch_required)) ||
            DictKeyExists(interp, optDict, interpCtx->elswitch_boolean)) {
            Tcl_DictObjPut(interp, optDict, interpCtx->elswitch_default, Tcl_NewStringObj("0", -1));
            Tcl_DictObjPut(interp, optDict, interpCtx->elswitch_value, Tcl_NewStringObj("1", -1));
        }
//****       Insert default -level if -upvar is used
        if (DictKeyExists(interp, optDict, interpCtx->elswitch_upvar) &&
            !DictKeyExists(interp, optDict, interpCtx->elswitch_level)) {
            if (HAS_GLOBAL_SWITCH(ctx, GLOBAL_SWITCH_LEVEL)) {
                Tcl_DictObjPut(interp, optDict, interpCtx->elswitch_level, GLOBAL_SWITCH_ARG(ctx, GLOBAL_SWITCH_LEVEL));
            } else {
                Tcl_DictObjPut(interp, optDict, interpCtx->elswitch_level, Tcl_NewStringObj("1", -1));
            }
        }
//****       Compute default output key if -ignore, -key, and -pass aren't used
        if (!DictKeyExists(interp, optDict, interpCtx->elswitch_ignore) &&
            !DictKeyExists(interp, optDict, interpCtx->elswitch_key) &&
            !DictKeyExists(interp, optDict, interpCtx->elswitch_pass)) {
            if (HAS_GLOBAL_SWITCH(ctx, GLOBAL_SWITCH_TEMPLATE)) {
                Tcl_Obj *templateMapList = Tcl_DuplicateObj(interpCtx->list_templateSubstNames);
                Tcl_Obj *resultObj = NULL;
                Tcl_ListObjAppendElement(interp, templateMapList, name);
                EvalStringMap(interp, templateMapList, GLOBAL_SWITCH_ARG(ctx, GLOBAL_SWITCH_TEMPLATE), &resultObj);
                Tcl_DictObjPut(interp, optDict, interpCtx->elswitch_key, resultObj);
                SAFE_DECR_REF(templateMapList);
            } else {
                Tcl_DictObjPut(interp, optDict, interpCtx->elswitch_key, name);
            }
        }
//****       Build parameter and switches definition lists
        Tcl_Obj *aliasVal = NULL;
        Tcl_Obj *matchListDummy = NULL;
        if (DictKeyExists(interp, optDict, interpCtx->elswitch_alias)) {
            Tcl_DictObjGet(interp, optDict, interpCtx->elswitch_alias, &aliasVal);
        }
        if (DictKeyExists(interp, optDict, interpCtx->elswitch_parameter)) {
//*****          Keep track of parameter order
            Tcl_ListObjAppendElement(interp, argCtx->orderList, name);
            if (DictKeyExists(interp, optDict, interpCtx->elswitch_catchall)) {
                if (argCtx->catchall != NULL) {
                    Tcl_Obj *msg = Tcl_NewStringObj("multiple catchall parameters: ", -1);
                    Tcl_AppendStringsToObj(msg, Tcl_GetString(argCtx->catchall), " and ", Tcl_GetString(name), NULL);
                    Tcl_SetObjResult(interp, msg);
                    return TCL_ERROR;
                } else {
                    argCtx->catchall = name;
                }
            }
        } else if (!DictKeyExists(interp, optDict, interpCtx->elswitch_alias)) {
//*****          Build list of switches
            Tcl_Obj *switchVal = Tcl_DuplicateObj(interpCtx->misc_dashStrObj);
            Tcl_AppendObjToObj(switchVal, name);
            Tcl_ListObjAppendElement(interp, argCtx->switchesList, switchVal);
        } else if (EvalMatchRegexpGroups(interp, regexpSwitchAlias, aliasVal, interpCtx, &matchListDummy) ==
                   TCL_ERROR) {
            Tcl_Obj *msg = Tcl_NewStringObj("bad alias: ", -1);
            Tcl_AppendStringsToObj(msg, Tcl_GetString(aliasVal), NULL);
            Tcl_SetObjResult(interp, msg);
            return TCL_ERROR;
        } else if (!CheckAliasesAreUnique(interp, argCtx->aliasesDict, optDict, interpCtx)) {
            Tcl_Obj *msg = Tcl_NewStringObj("element alias collision: ", -1);
            Tcl_AppendStringsToObj(msg, Tcl_GetString(aliasVal), NULL);
            Tcl_SetObjResult(interp, msg);
            return TCL_ERROR;
        } else {
//*****          Build list of switches (with aliases), and link switch aliases
            Tcl_Size aliasValListLen;
            Tcl_Obj **aliasValListElems;
            if (Tcl_ListObjGetElements(interp, aliasVal, &aliasValListLen, &aliasValListElems) != TCL_OK) {
                Tcl_SetObjResult(interp, Tcl_NewStringObj("error getting aliases list", -1));
                return TCL_ERROR;
            }
            for (Tcl_Size f = 0; f < aliasValListLen; ++f) {
                Tcl_DictObjPut(interp, argCtx->aliasesDict, aliasValListElems[f], name);
            }
            Tcl_ListObjAppendElement(interp, argCtx->switchesList, BuildAliasJoinString(interp, optDict, name, interpCtx));
        }
//*****          Check for collision between alias and other switch name
        Tcl_Obj *switchValCollision = NULL;
        if (DICT_GET_IF_EXISTS(interp, argCtx->aliasesDict, name, &switchValCollision)) {
            Tcl_Obj *msg = Tcl_NewStringObj("collision of switch -", -1);
            Tcl_AppendStringsToObj(msg, Tcl_GetString(switchValCollision), " alias with the -", Tcl_GetString(name),
                                   " switch", NULL);
            Tcl_SetObjResult(interp, msg);
            return TCL_ERROR;
        }
//*****          Map from upvar keys back to element names, and forbid collisions
        Tcl_Obj *keyVal = NULL;
        if (DictKeyExists(interp, optDict, interpCtx->elswitch_upvar) &&
            DICT_GET_IF_EXISTS(interp, optDict, interpCtx->elswitch_key, &keyVal)) {
            Tcl_Obj *upvarVal = NULL;
            if (DICT_GET_IF_EXISTS(interp, argCtx->upvarsDict, keyVal, &upvarVal)) {
                Tcl_Obj *msg = Tcl_NewStringObj("multiple upvars to the same variable: ", -1);
                Tcl_AppendStringsToObj(msg, Tcl_GetString(upvarVal), " ", Tcl_GetString(name), NULL);
                Tcl_SetObjResult(interp, msg);
                return TCL_ERROR;
            }
            Tcl_DictObjPut(interp, argCtx->upvarsDict, keyVal, name);
        }
//*****          Look up named enumeration lists and validation expressions
        Tcl_Obj *elemEnumVal = NULL;
        Tcl_Obj *elemValidateVal = NULL;
        if (DICT_GET_IF_EXISTS(interp, optDict, interpCtx->elswitch_enum, &elemEnumVal) &&
            HAS_GLOBAL_SWITCH(ctx, GLOBAL_SWITCH_ENUM)) {
            Tcl_Obj *globEnumVal = NULL;
            if (DICT_GET_IF_EXISTS(interp, GLOBAL_SWITCH_ARG(ctx, GLOBAL_SWITCH_ENUM), elemEnumVal, &globEnumVal)) {
                Tcl_DictObjPut(interp, optDict, interpCtx->elswitch_enum, globEnumVal);
            }
        } else if (DICT_GET_IF_EXISTS(interp, optDict, interpCtx->elswitch_validate, &elemValidateVal)) {
            if (HAS_GLOBAL_SWITCH(ctx, GLOBAL_SWITCH_VALIDATE)) {
                Tcl_Obj *globValidateVal = NULL;
                if (DICT_GET_IF_EXISTS(interp, GLOBAL_SWITCH_ARG(ctx, GLOBAL_SWITCH_VALIDATE), elemValidateVal,
                                       &globValidateVal)) {
                    Tcl_Obj *objv[] = {
                        elemValidateVal,
                        Tcl_NewStringObj("validation", -1),
                    };
                    Tcl_DictObjPut(interp, optDict, Tcl_NewStringObj("validateMsg", -1), Tcl_ConcatObj(2, objv));
                    Tcl_DictObjPut(interp, optDict, interpCtx->elswitch_validate, globValidateVal);
                } else {
                    Tcl_Obj *objv[] = {
                        Tcl_NewStringObj("validation:", -1),
                        elemValidateVal,
                    };
                    Tcl_DictObjPut(interp, optDict, Tcl_NewStringObj("validateMsg", -1), Tcl_ConcatObj(2, objv));
                }
            } else {
                Tcl_Obj *objv[] = {
                    Tcl_NewStringObj("validation:", -1),
                    elemValidateVal,
                };
                Tcl_DictObjPut(interp, optDict, Tcl_NewStringObj("validateMsg", -1), Tcl_ConcatObj(2, objv));
            }
        }
//*****          Check for allowed arguments to -type
        Tcl_Obj *typeVal = NULL;
        if (DICT_GET_IF_EXISTS(interp, optDict, interpCtx->elswitch_type, &typeVal)) {
            if (PrefixMatch(interp, allowedTypes, typeVal, 1, 0, NULL, 0, NULL) == TCL_ERROR) {
                Tcl_Obj *msg = Tcl_DuplicateObj(interpCtx->elswitch_type);
                Tcl_AppendStringsToObj(
                    msg, " ", Tcl_GetString(typeVal), " is not in the list of allowed types, must be ",
                    Tcl_GetString(BuildAllowedTypesSummary(interp, interpCtx->list_allowedTypes)), NULL);
                Tcl_SetObjResult(interp, msg);
                return TCL_ERROR;
            }
        }
//****       Save element definition
        Tcl_DictObjPut(interp, argCtx->defDict, name, optDict);
//****       Prepare to identify omitted elements
        Tcl_DictObjPut(interp, argCtx->omittedDict, name, interpCtx->misc_emptyStrObj);
    }
    return TCL_OK;
}

//***    CreateAndCacheArgDef function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * CreateAndCacheArgDef --
 *
 *      Create and cache an `ArgumentDefinition` structure by parsing a given element definition list.
 *      If an entry with the same key already exists in the internal hash table, the cached definition is returned.
 *
 * Parameters:
 *      Tcl_Interp *interp             - input: target interpreter
 *      ArgparseInterpCtx *interpCtx   - input: per-interpreter context containing the hash table
 *      Tcl_Obj *definition            - input: Tcl list object containing switch/parameter definitions
 *      GlobalSwitchesContext *ctx     - input: pointer to global parsing context
 *      const char *key                - input: key string used to cache the parsed result in the internal hash table
 *
 * Results:
 *      Returns a pointer to a fully populated `ArgumentDefinition` structure.  
 *      If a definition already exists for the given key, it is returned immediately.  
 *      Returns NULL and sets an error in the interpreter on failure (e.g., parse error or allocation failure).
 *
 * Side Effects:
 *      - Allocates and initializes a new `ArgumentDefinition` structure if not cached
 *      - Stores the result in `argDefHashTable` in the per-interpreter context using the provided key
 *      - On error, cleans up and removes the partially inserted hash entry
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
ArgumentDefinition *CreateAndCacheArgDef(Tcl_Interp *interp, ArgparseInterpCtx *interpCtx, Tcl_Obj *definition,
                                         GlobalSwitchesContext *ctx, const char *key) {
    Tcl_HashEntry *entry;
    int isNew;
    entry = Tcl_CreateHashEntry(&interpCtx->argDefHashTable, key, &isNew);
    if (!isNew) {
        // already cached
        return (ArgumentDefinition *)Tcl_GetHashValue(entry);
    }
    // Allocate and init
    ArgumentDefinition *argDef = malloc(sizeof(ArgumentDefinition));
    if (!argDef) {
        Tcl_SetResult(interp, "Failed to allocate memory for ArgumentDefinition", TCL_STATIC);
        Tcl_DeleteHashEntry(entry);
        return NULL;
    }
    InitArgumentDefinition(argDef);
    // Call parsing function to populate the fields
    int defParseCode = ParseElementDefinitions(interp, ctx, definition, argDef, interpCtx);
    if (defParseCode != TCL_OK) {
        // Cleanup partially initialized struct on error
        FreeArgumentDefinition(argDef);
        Tcl_DeleteHashEntry(entry);
        return NULL;
    }
    // Store it
    Tcl_SetHashValue(entry, argDef);
    return argDef;
}

//***    CleanupAllArgumentDefinitions function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * CleanupAllArgumentDefinitions --
 *
 *      Free all cached `ArgumentDefinition` structures stored in the per-interpreter context.
 *      Intended to be called during interpreter finalization or library shutdown.
 *
 * Parameters:
 *      ArgparseInterpCtx *interpCtx - input: interpreter context structure
 *
 * Results:
 *      None
 *
 * Side Effects:
 *      - Iterates over all entries in `argDefHashTable` in the provided `ArgparseInterpCtx`
 *      - Calls `FreeArgumentDefinition` on each stored structure to release all associated memory and Tcl objects  
 *      - Deletes the hash table to fully release all resources
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
static void CleanupAllArgumentDefinitions(ArgparseInterpCtx *interpCtx) {
    Tcl_HashSearch search;
    Tcl_HashEntry *entry = Tcl_FirstHashEntry(&interpCtx->argDefHashTable, &search);
    while (entry != NULL) {
        ArgumentDefinition *argDef = (ArgumentDefinition *)Tcl_GetHashValue(entry);
        // free Tcl objects and other cleanup inside argDef
        FreeArgumentDefinition(argDef);
        entry = Tcl_NextHashEntry(&search);
    }
    Tcl_DeleteHashTable(&interpCtx->argDefHashTable);
}

//***    GenerateGlobalSwitchesKey function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * GenerateGlobalSwitchesKey --
 *
 *      Generates a Tcl_Obj key that represents the state of global switches, including any arguments they may have.
 *
 * Parameters:
 *      const GlobalSwitchesContext *ctx - input: pointer to the context containing the state of the global switches
 *
 * Results:
 *      Returns a Tcl_Obj that represents the key for the global switches. This key includes the global switch value and
 *      any arguments associated with switches that take arguments, formatted as `switchName=value;`.
 *      The returned Tcl_Obj has an incremented reference count.
 *
 * Side Effects:
 *      A new Tcl_Obj is created and populated with the global switch key.
 *      The reference count of the generated Tcl_Obj is incremented before returning.
 *      The function does not modify the input context `ctx`.
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
static Tcl_Obj *GenerateGlobalSwitchesKey(const GlobalSwitchesContext *ctx) {
    Tcl_Obj *keyObj = Tcl_NewObj();
    Tcl_AppendPrintfToObj(keyObj, "%d:", ctx->globalSwitches);
    for (int i = 0; i < GLOBAL_SWITCH_COUNT; ++i) {
        if ((GLOBAL_SWITCH_TAKES_ARG_MASK & (1 << i)) && HAS_GLOBAL_SWITCH(ctx, i)) {
            Tcl_Obj *argObj = ctx->values[i];
            if (argObj != NULL) {
                const char *argStr = Tcl_GetString(argObj);
                Tcl_AppendPrintfToObj(keyObj, "%s=%s;", 1+globalSwitches[i], argStr);
            }
        }
    }
    return keyObj;
}

//***    DuplicateDictWithNestedDicts function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * DuplicateDictWithNestedDicts --
 *
 *      Creates a deep copy of a Tcl dictionary object, including nested dictionaries, recursively duplicating all 
 *      dictionary values.
 *
 * Parameters:
 *      Tcl_Interp *interp          - input: target interpreter, used for dictionary operations and error handling
 *      Tcl_Obj *dictObj            - input: pointer to the Tcl dictionary object to be duplicated
 *
 * Results:
 *      Returns a new Tcl_Obj that is a deep copy of the input dictionary `dictObj`. If the dictionary contains nested
 *      dictionaries, they are also recursively duplicated.
 *      The returned Tcl_Obj has an incremented reference count.
 *
 * Side Effects:
 *      A new Tcl_Obj is created and populated with a deep copy of the original dictionary, including nested
 *      dictionaries.
 *      The reference count of the returned object is incremented before returning.
 *      Temporary memory for keys is allocated and freed within the function.
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
Tcl_Obj *DuplicateDictWithNestedDicts(Tcl_Interp *interp, Tcl_Obj *dictObj) {
    if (dictObj == NULL)
        return NULL;
    Tcl_Obj *newDict = Tcl_DuplicateObj(dictObj);
    Tcl_IncrRefCount(newDict);
    Tcl_Size keyCount = 0;
    Tcl_Obj **keys = NULL;
    if (DictKeys(interp, newDict, &keyCount, &keys) != TCL_OK) {
        return newDict;
    }
    for (Tcl_Size i = 0; i < keyCount; i++) {
        Tcl_Obj *key = keys[i];
        Tcl_Obj *val;
        if (Tcl_DictObjGet(interp, newDict, key, &val) == TCL_OK && val != NULL) {
            const Tcl_ObjType *typePtr = val->typePtr;
            if (typePtr != NULL && strcmp(typePtr->name, "dict") == 0) {
                Tcl_Obj *dupVal = DuplicateDictWithNestedDicts(interp, val);
                Tcl_DictObjPut(interp, newDict, key, dupVal);
                Tcl_DecrRefCount(dupVal);
            }
        }
    }
    free(keys);
    return newDict;
}

//** FreeArgparseInterpCtx
/*
 * FreeArgparseInterpCtx --
 *
 *     Frees the per-interpreter argparse context
 *
 * Parameters:
 *      ArgparseInterpCtx *interpCtx - input: interpreter context structure Must not be NULL.
 *
 * Side-effects:
 *     The memory for the context as well as its contained elements, such
 *     as hash tables are freed.
 */
static void FreeArgparseInterpCtx(void *clientData) {
    ArgparseInterpCtx *interpCtx = (ArgparseInterpCtx *)clientData;
    CleanupAllArgumentDefinitions(interpCtx);
    FREE_LIST(interpCtx->list_allowedTypes);
    FREE_LIST(interpCtx->list_templateSubstNames);
    FREE_LIST(interpCtx->list_helpGenSubstNames);
    Tcl_DecrRefCount(interpCtx->misc_emptyStrObj);
    Tcl_DecrRefCount(interpCtx->misc_presentSwitchObj);
    Tcl_DecrRefCount(interpCtx->misc_validateMsgStrObj);
    Tcl_DecrRefCount(interpCtx->misc_dashStrObj);
    Tcl_DecrRefCount(interpCtx->misc_doubleDashStrObj);
    Tcl_DecrRefCount(interpCtx->elswitch_alias);
    Tcl_DecrRefCount(interpCtx->elswitch_argument);
    Tcl_DecrRefCount(interpCtx->elswitch_boolean);
    Tcl_DecrRefCount(interpCtx->elswitch_catchall);
    Tcl_DecrRefCount(interpCtx->elswitch_default);
    Tcl_DecrRefCount(interpCtx->elswitch_enum);
    Tcl_DecrRefCount(interpCtx->elswitch_forbid);
    Tcl_DecrRefCount(interpCtx->elswitch_ignore);
    Tcl_DecrRefCount(interpCtx->elswitch_imply);
    Tcl_DecrRefCount(interpCtx->elswitch_keep);
    Tcl_DecrRefCount(interpCtx->elswitch_key);
    Tcl_DecrRefCount(interpCtx->elswitch_level);
    Tcl_DecrRefCount(interpCtx->elswitch_optional);
    Tcl_DecrRefCount(interpCtx->elswitch_parameter);
    Tcl_DecrRefCount(interpCtx->elswitch_pass);
    Tcl_DecrRefCount(interpCtx->elswitch_reciprocal);
    Tcl_DecrRefCount(interpCtx->elswitch_require);
    Tcl_DecrRefCount(interpCtx->elswitch_required);
    Tcl_DecrRefCount(interpCtx->elswitch_standalone);
    Tcl_DecrRefCount(interpCtx->elswitch_switch);
    Tcl_DecrRefCount(interpCtx->elswitch_upvar);
    Tcl_DecrRefCount(interpCtx->elswitch_validate);
    Tcl_DecrRefCount(interpCtx->elswitch_value);
    Tcl_DecrRefCount(interpCtx->elswitch_type);
    Tcl_DecrRefCount(interpCtx->elswitch_allow);
    Tcl_DecrRefCount(interpCtx->elswitch_help);
    Tcl_DecrRefCount(interpCtx->elswitch_errormsg);
    Tcl_DecrRefCount(interpCtx->elswitch_hsuppress);
    Tcl_Free(interpCtx);
}

//** InitArgparseInterpCtx function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * InitArgparseInterpCtx --
 *
 *     Allocates and initializes the per-interpreter context used by argparse.
 *     This is passed as the clientData parameter to argparse command implementation.
 *
 * Parameters:
 *      Tcl_Interp *interp - input: target interpreter to initialize the extension within
 *
 * Results:
 *     Returns pointer to initialized context. This is eventually freed by
 *     FreeArgparseInterpCtx when the command is deleted. Returns NULL on failure.
 */
static ArgparseInterpCtx *InitArgparseInterpCtx(Tcl_Interp *interp) {
    ArgparseInterpCtx *interpCtx = Tcl_AttemptAlloc(sizeof(*interpCtx));
    if (interpCtx == NULL) {
        if (interp) {
            Tcl_SetResult(interp, "Could not allocate memory for argparse context", TCL_STATIC);
        }
        return NULL;
    }
    Tcl_InitHashTable(&interpCtx->argDefHashTable, TCL_STRING_KEYS);
    INIT_LIST(interpCtx->list_allowedTypes, allowedTypes, ELEMENT_SWITCH_COUNT_TYPES);
    INIT_LIST(interpCtx->list_templateSubstNames, templateSubstNames, TEMPLATE_SUBST_COUNT);
    INIT_LIST(interpCtx->list_helpGenSubstNames, helpGenSubstNames, HELP_GEN_SUBST_COUNT);
    interpCtx->misc_emptyStrObj = Tcl_NewStringObj("", -1);
    Tcl_IncrRefCount(interpCtx->misc_emptyStrObj);
    interpCtx->misc_presentSwitchObj = Tcl_NewStringObj("present", -1);
    Tcl_IncrRefCount(interpCtx->misc_presentSwitchObj);
    interpCtx->misc_validateMsgStrObj = Tcl_NewStringObj("validateMsg", -1);
    Tcl_IncrRefCount(interpCtx->misc_validateMsgStrObj);
    interpCtx->misc_dashStrObj = Tcl_NewStringObj("-", -1);
    Tcl_IncrRefCount(interpCtx->misc_dashStrObj);
    interpCtx->misc_doubleDashStrObj = Tcl_NewStringObj("--", -1);
    Tcl_IncrRefCount(interpCtx->misc_doubleDashStrObj);
    ELSWITCH(interpCtx->elswitch_alias, alias);
    ELSWITCH(interpCtx->elswitch_argument, argument);
    ELSWITCH(interpCtx->elswitch_boolean, boolean);
    ELSWITCH(interpCtx->elswitch_catchall, catchall);
    ELSWITCH(interpCtx->elswitch_default, default);
    ELSWITCH(interpCtx->elswitch_enum, enum);
    ELSWITCH(interpCtx->elswitch_forbid, forbid);
    ELSWITCH(interpCtx->elswitch_ignore, ignore);
    ELSWITCH(interpCtx->elswitch_imply, imply);
    ELSWITCH(interpCtx->elswitch_keep, keep);
    ELSWITCH(interpCtx->elswitch_key, key);
    ELSWITCH(interpCtx->elswitch_level, level);
    ELSWITCH(interpCtx->elswitch_optional, optional);
    ELSWITCH(interpCtx->elswitch_parameter, parameter);
    ELSWITCH(interpCtx->elswitch_pass, pass);
    ELSWITCH(interpCtx->elswitch_reciprocal, reciprocal);
    ELSWITCH(interpCtx->elswitch_require, require);
    ELSWITCH(interpCtx->elswitch_required, required);
    ELSWITCH(interpCtx->elswitch_standalone, standalone);
    ELSWITCH(interpCtx->elswitch_switch, switch);
    ELSWITCH(interpCtx->elswitch_upvar, upvar);
    ELSWITCH(interpCtx->elswitch_validate, validate);
    ELSWITCH(interpCtx->elswitch_value, value);
    ELSWITCH(interpCtx->elswitch_type, type);
    ELSWITCH(interpCtx->elswitch_allow, allow);
    ELSWITCH(interpCtx->elswitch_help, help);
    ELSWITCH(interpCtx->elswitch_errormsg, errormsg);
    ELSWITCH(interpCtx->elswitch_hsuppress, hsuppress);
    return interpCtx;
}

//** Package initialization function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * Argparse_c_Init --
 *
 *      Initializes the Argparse C extension for Tcl. This function performs various setup tasks including initializing
 *      necessary stubs, providing the package, initializing static objects, and registering commands.
 *
 * Parameters:
 *      Tcl_Interp *interp            - input: target interpreter to initialize the extension within
 *
 * Results:
 *      Returns TCL_OK on success or TCL_ERROR on failure.
 *
 * Side Effects:
 *      Initializes static objects and registers several commands with the Tcl interpreter:
 *      - Registers the "argparse_c" command with a corresponding command procedure.
 *      - Registers cleanup functions to be called when the interpreter is deleted.
 *      - Initializes various static data structures including hash tables and objects.
 *      - Provides the package information to the interpreter.
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
extern DLLEXPORT int Argparse_Init(Tcl_Interp *interp) {
    if (Tcl_InitStubs(interp, "8.6-10.0", 0) == NULL) {
        return TCL_ERROR;
    }
    /* Provide the current package */
    if (Tcl_PkgProvideEx(interp, PACKAGE_NAME, PACKAGE_VERSION, NULL) != TCL_OK) {
        return TCL_ERROR;
    }
    /* Initialize interpreter context */
    ArgparseInterpCtx *interpCtx = InitArgparseInterpCtx(interp);
    if (interpCtx == NULL) {
        return TCL_ERROR;
    }
    /* Register commands */
    Tcl_CreateObjCommand2(interp, "argparse", (Tcl_ObjCmdProc2 *)ArgparseCmdProc2, interpCtx, FreeArgparseInterpCtx);
    return TCL_OK;
}

//** Argparse command function
/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 * ArgparseCmdProc2 --
 *
 *      The command function for the "argparse_c" Tcl command. This function processes the command-line arguments, 
 *      manages global switches, parses argument definitions, checks for conflicts, validates arguments, and processes 
 *      parameters based on the specified constraints and requirements.
 *
 * Parameters:
 *      void *clientData              - input: client data, typically not used in this function (can be set to NULL)
 *      Tcl_Interp *interp            - input: the Tcl interpreter in which the command is executed
 *      Tcl_Size objc                 - input: the number of arguments passed to the command
 *      Tcl_Obj *const objv[]         - input: array of Tcl_Obj pointers representing the command arguments
 *
 * Results:
 *      Returns TCL_OK on successful processing, TCL_ERROR on failure.
 *
 * Side Effects:
 *      - Modifies global switch contexts and argument definitions based on command input.
 *      - Registers and modifies various argument definitions.
 *      - Constructs and validates argument lists, including checking for conflicts between switches and parameters.
 *      - Sets interpreter results with error messages if invalid arguments are encountered.
 *      - Modifies or creates Tcl variables based on parsed argument values.
 *      - Cleans up memory and data structures on error.
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
static int ArgparseCmdProc2(void *clientData, Tcl_Interp *interp, Tcl_Size objc, Tcl_Obj *const objv[]) {
    GlobalSwitchesContext ctx = {0};
    Tcl_Obj *definition = NULL;                                     // stores input definition list
    Tcl_Obj *argv = NULL;                                           // stores arguments to target procedure
    ArgumentDefinition *argDefCtx = NULL;                           // arguments definition structure
    ArgparseInterpCtx *interpCtx = (ArgparseInterpCtx *)clientData; // per-interpreter context structure

//***    Process arguments to argparse procedure
    Tcl_Size defListLen;
    Tcl_Obj **defListElems;
    Tcl_Size i;
    for (i = 1; i < objc; ++i) {
        int globalSwitchId;
        if (Tcl_GetIndexFromObj(NULL, objv[i], globalSwitches, "option", 0, &globalSwitchId) != TCL_OK) {
            break;
        }
        // handle flags that take arguments
        if (GLOBAL_SWITCH_TAKES_ARG_MASK & (1 << globalSwitchId)) {
            if (i + 1 >= objc) {
                Tcl_SetObjResult(interp, Tcl_ObjPrintf("Missing argument for %s", globalSwitches[globalSwitchId]));
                goto cleanupOnError;
            }
            SetGlobalSwitch(&ctx, globalSwitchId, objv[i + 1]);
            i++; // skip value
        }
        else {
            SetGlobalSwitch(&ctx, globalSwitchId, NULL);
        }
    }
    /* End of global options. */

    if (i < objc && strcmp(Tcl_GetString(objv[i]), "--") == 0) {
	/* Explicit end of global options marker. Skip it */
        i++;
    }
    switch (objc - i) {
    case 0:
        Tcl_SetObjResult(interp, Tcl_NewStringObj("missing required parameter: definition", -1));
        goto cleanupOnError;
    case 1:
        /* if args is omitted, pulled it from the caller args variable */
        argv = Tcl_GetVar2Ex(interp, "args", NULL, 0);
        if (argv == NULL) {
            Tcl_SetObjResult(interp, Tcl_NewStringObj("Variable 'args' not found", -1));
            goto cleanupOnError;
        }
        break;
    case 2:
        argv = objv[objc - 1];
        break;
    default:
        Tcl_SetObjResult(interp, Tcl_NewStringObj("too many arguments", -1));
        goto cleanupOnError;
    };
    /* Verify argv is a list before further processing - Bug #17 */
    Tcl_Size lenArgv;
    if (Tcl_ListObjLength(interp, argv, &lenArgv) != TCL_OK) {
        goto cleanupOnError;
    }
    /* pre-process definition list */
    if (Tcl_ListObjGetElements(interp, objv[i], &defListLen, &defListElems) != TCL_OK) {
        goto cleanupOnError;
    }
    definition = Tcl_NewListObj(defListLen, NULL); /* Preallocate storage */
    int commentFlag = 0;
    for (Tcl_Size j = 0; j < defListLen; ++j) {
        Tcl_Size elemListLen;
        Tcl_Obj **elemListElems;
        if (Tcl_ListObjGetElements(interp, defListElems[j], &elemListLen, &elemListElems) != TCL_OK) {
            Tcl_SetObjResult(interp, Tcl_NewStringObj("error getting element definition list", -1));
            goto cleanupOnError;
        }
	if (elemListLen == 0) {
            Tcl_SetResult(interp, "element definition cannot be empty", TCL_STATIC);
            goto cleanupOnError;
        }
        if (strcmp(Tcl_GetString(elemListElems[0]), "#") == 0) {
            if (elemListLen == 1) {
                commentFlag = 1;
            }
        } else if (commentFlag == 1) {
            commentFlag = 0;
        } else {
            Tcl_ListObjAppendElement(interp, definition, defListElems[j]);
        }
    }

    if (HAS_GLOBAL_SWITCH(&ctx, GLOBAL_SWITCH_INLINE) && HAS_GLOBAL_SWITCH(&ctx, GLOBAL_SWITCH_KEEP)) {
        Tcl_SetObjResult(interp, Tcl_NewStringObj("-inline and -keep conflict", -1));
        goto cleanupOnError;
    }
    if (HAS_GLOBAL_SWITCH(&ctx, GLOBAL_SWITCH_MIXED) && HAS_GLOBAL_SWITCH(&ctx, GLOBAL_SWITCH_PFIRST)) {
        Tcl_SetObjResult(interp, Tcl_NewStringObj("-mixed and -pfirst conflict", -1));
        goto cleanupOnError;
    }

//***    Parse element definition list and cash it, or used cashed version
    const char *keyArgDefinition = Tcl_GetString(definition);
    const char *keyStr = Tcl_GetString(GenerateGlobalSwitchesKey(&ctx));
    keyArgDefinition = Tcl_GetString(Tcl_ObjPrintf("%s %s",keyArgDefinition, keyStr));
    Tcl_HashEntry *entry = Tcl_FindHashEntry(&interpCtx->argDefHashTable, keyArgDefinition);
    ArgumentDefinition *cashedArgDefCtx = NULL;
    if (entry == NULL) {
        // no cashed definition
        cashedArgDefCtx = CreateAndCacheArgDef(interp, interpCtx, definition, &ctx, keyArgDefinition);
        if (!cashedArgDefCtx) {
            goto cleanupOnError;
        }
    } else {
        // use cashed definition
        cashedArgDefCtx = (ArgumentDefinition *)Tcl_GetHashValue(entry);
    }
    argDefCtx = DeepCopyArgumentDefinition(interp, cashedArgDefCtx);

//***    Process constraints and shared key logic
    Tcl_Obj **keyObjs;
    Tcl_Size keyCount;
    DictKeys(interp, argDefCtx->defDict, &keyCount, &keyObjs);
    for (Tcl_Size idx = 0; idx < keyCount; ++idx) {
        Tcl_Obj *name = keyObjs[idx];
        Tcl_Obj *opt = NULL;
        Tcl_DictObjGet(interp, argDefCtx->defDict, name, &opt);
//****       Verify constraint references
        for (int i = 0; i < ELEMENT_SWITCH_COUNT_CONSTRAINTS; ++i) {
            Tcl_Obj *constraint = Tcl_NewStringObj(elemSwConstraints[i], -1);
            Tcl_Obj *otherNameList = NULL;
            if (DICT_GET_IF_EXISTS(interp, opt, constraint, &otherNameList)) {
                Tcl_Size otherNameListLen;
                Tcl_Obj **otherNameListElems;
                Tcl_ListObjGetElements(interp, otherNameList, &otherNameListLen, &otherNameListElems);
                for (Tcl_Size j = 0; j < otherNameListLen; ++j) {
                    Tcl_Obj *otherName = otherNameListElems[j];
                    if (!DictKeyExists(interp, argDefCtx->defDict, otherName)) {
                        Tcl_Obj *msg = Tcl_DuplicateObj(name);
                        Tcl_AppendStringsToObj(msg, " -", Tcl_GetString(constraint),
                                               " references undefined element: ", Tcl_GetString(otherName), NULL);
                        Tcl_SetObjResult(interp, msg);
                        free(keyObjs);
                        goto cleanupOnError;
                    }
                }
            }
        }
//****       Create reciprocal requirements
        Tcl_Obj *otherList = NULL;
        if ((HAS_GLOBAL_SWITCH(&ctx, GLOBAL_SWITCH_RECIPROCAL) || DictKeyExists(interp, opt, interpCtx->elswitch_reciprocal)) &&
            DICT_GET_IF_EXISTS(interp, opt, interpCtx->elswitch_require, &otherList)) {
            Tcl_Size otherListLen;
            Tcl_Obj **otherListElems;
            Tcl_ListObjGetElements(interp, otherList, &otherListLen, &otherListElems);
            for (Tcl_Size j = 0; j < otherListLen; ++j) {
                Tcl_Obj *other = otherListElems[j];
                Tcl_Obj *otherOpt = NULL;
                Tcl_DictObjGet(interp, argDefCtx->defDict, other, &otherOpt);
                if (otherOpt == NULL) {
                    otherOpt = Tcl_NewDictObj();
                } else {
                    otherOpt = Tcl_DuplicateObj(otherOpt);
                }
                Tcl_IncrRefCount(otherOpt);
                Tcl_Obj *requireList = NULL;
                Tcl_DictObjGet(interp, otherOpt, interpCtx->elswitch_require, &requireList);
                if (requireList == NULL) {
                    requireList = Tcl_NewListObj(0, NULL);
                } else {
                    requireList = Tcl_DuplicateObj(requireList);
                }
                Tcl_ListObjAppendElement(interp, requireList, name);
                Tcl_DictObjPut(interp, otherOpt, interpCtx->elswitch_require, requireList);
                Tcl_DictObjPut(interp, argDefCtx->defDict, other, otherOpt);
                Tcl_DecrRefCount(otherOpt);
            }
        }
//****       Perform shared key logic
        Tcl_Obj *optKey = NULL;
        Tcl_Obj *otherOptKey = NULL;
        if (DICT_GET_IF_EXISTS(interp, opt, interpCtx->elswitch_key, &optKey)) {
            for (Tcl_Size j = 0; j < keyCount; ++j) {
                Tcl_Obj *otherName = keyObjs[j];
                Tcl_Obj *otherOpt = NULL;
                if (Tcl_DictObjGet(interp, argDefCtx->defDict, otherName, &otherOpt) != TCL_OK || otherOpt == NULL) {
                    continue;
                }
                if ((strcmp(Tcl_GetString(name), Tcl_GetString(otherName)) != 0) &&
                    DICT_GET_IF_EXISTS(interp, otherOpt, interpCtx->elswitch_key, &otherOptKey)) {
                    if (strcmp(Tcl_GetString(optKey), Tcl_GetString(otherOptKey)) == 0) {
//*****          Limit when shared keys may be used
                        if (DictKeyExists(interp, opt, interpCtx->elswitch_parameter)) {
                            Tcl_Obj *msg = Tcl_DuplicateObj(name);
                            Tcl_AppendStringsToObj(msg, " cannot be a parameter because it shares a key with ",
                                                   Tcl_GetString(otherName), NULL);
                            Tcl_SetObjResult(interp, msg);
                            free(keyObjs);
                            goto cleanupOnError;
                        } else if (DictKeyExists(interp, opt, interpCtx->elswitch_argument)) {
                            Tcl_Obj *msg = Tcl_DuplicateObj(name);
                            Tcl_AppendStringsToObj(msg, " cannot use -argument because it shares a key with ",
                                                   Tcl_GetString(otherName), NULL);
                            Tcl_SetObjResult(interp, msg);
                            free(keyObjs);
                            goto cleanupOnError;
                        } else if (DictKeyExists(interp, opt, interpCtx->elswitch_catchall)) {
                            Tcl_Obj *msg = Tcl_DuplicateObj(name);
                            Tcl_AppendStringsToObj(msg, " cannot use -catchall because it shares a key with ",
                                                   Tcl_GetString(otherName), NULL);
                            Tcl_SetObjResult(interp, msg);
                            free(keyObjs);
                            goto cleanupOnError;
                        } else if (DictKeyExists(interp, opt, interpCtx->elswitch_default) &&
                                   DictKeyExists(interp, otherOpt, interpCtx->elswitch_default)) {
                            Tcl_Obj *msg = Tcl_DuplicateObj(name);
                            Tcl_AppendStringsToObj(msg, " and ", Tcl_GetString(otherName),
                                                   " cannot both use -default because they share a key", NULL);
                            Tcl_SetObjResult(interp, msg);
                            free(keyObjs);
                            goto cleanupOnError;
                        }
//*****          Create forbid constraints on shared keys
                        int nameInForbidList = 0;
                        Tcl_Obj *otherOptForbidList = NULL;
                        if (DICT_GET_IF_EXISTS(interp, otherOpt, interpCtx->elswitch_forbid, &otherOptForbidList)) {
                            nameInForbidList = InList(interp, name, otherOptForbidList);
                        }
                        if (!DictKeyExists(interp, otherOpt, interpCtx->elswitch_forbid) || !nameInForbidList) {
                            Tcl_Obj *otherOpt = NULL;
                            Tcl_DictObjGet(interp, argDefCtx->defDict, otherName, &otherOpt);
                            if (otherOpt == NULL) {
                                otherOpt = Tcl_NewDictObj();
                            } else {
                                otherOpt = Tcl_DuplicateObj(otherOpt);
                            }
                            Tcl_IncrRefCount(otherOpt);
                            DictLappendElem(interp, otherOpt, interpCtx->elswitch_forbid, name);
                            Tcl_DictObjPut(interp, argDefCtx->defDict, otherName, otherOpt);
                            Tcl_DecrRefCount(otherOpt);
                        }
//*****          Set default -value for shared keys
                        if (!DictKeyExists(interp, opt, interpCtx->elswitch_value)) {
                            Tcl_Obj *valueDict = NULL;
                            Tcl_DictObjGet(interp, argDefCtx->defDict, name, &valueDict);
                            if (valueDict == NULL) {
                                valueDict = Tcl_NewDictObj();
                            }
                            Tcl_DictObjPut(interp, valueDict, interpCtx->elswitch_value, name);
                            Tcl_DictObjPut(interp, argDefCtx->defDict, name, valueDict);
                        }
                    }
                }
            }
        }
    }
    free(keyObjs);
//****       Build help string
    if (HAS_GLOBAL_SWITCH(&ctx, GLOBAL_SWITCH_HELP)) {
        if (Tcl_PkgRequire(interp, "textutil::adjust", "0", 0) == NULL) {
            goto cleanupOnError;
        }
        int helpSwitchPresented;
        if (HAS_GLOBAL_SWITCH(&ctx, GLOBAL_SWITCH_LONG)) {
            helpSwitchPresented = InList(interp, Tcl_NewStringObj("--help", -1), argv);
        } else {
            helpSwitchPresented = InList(interp, Tcl_NewStringObj("-help", -1), argv);
        }
        if (helpSwitchPresented) {
            Tcl_Obj *helpLevel = NULL;
            if (!HAS_GLOBAL_SWITCH(&ctx, GLOBAL_SWITCH_HELPLEVEL)) {
                helpLevel = Tcl_NewStringObj("2", -1);
            } else {
                helpLevel = GLOBAL_SWITCH_ARG(&ctx, GLOBAL_SWITCH_HELPLEVEL);
            }
            Tcl_Obj *finalDescrList = BuildHelpMessage(interp, &ctx, argDefCtx, helpLevel, interpCtx);
            if (HAS_GLOBAL_SWITCH(&ctx, GLOBAL_SWITCH_HELPRET)) {
                Tcl_Obj *optionsDict = Tcl_NewDictObj();
                Tcl_DictObjPut(interp, optionsDict, Tcl_NewStringObj("-level", -1), helpLevel);
                Tcl_SetObjResult(interp, finalDescrList);
                Tcl_SetReturnOptions(interp, optionsDict);
                return TCL_RETURN;
            } else {
                Tcl_Channel stdoutChan = Tcl_GetStdChannel(TCL_STDOUT);
                if (stdoutChan != NULL) {
                    Tcl_WriteChars(stdoutChan, Tcl_GetString(finalDescrList), -1);
                    Tcl_Flush(stdoutChan);
                }
                Tcl_ResetResult(interp);
                Tcl_Obj *optionsDict = Tcl_NewDictObj();
                Tcl_DictObjPut(interp, optionsDict, Tcl_NewStringObj("-level", -1), helpLevel);
                Tcl_SetReturnOptions(interp, optionsDict);
                return TCL_RETURN;
            }
        }

    }
//***    Handle default pass-through switch by creating a dummy element
    if (HAS_GLOBAL_SWITCH(&ctx, GLOBAL_SWITCH_PASS)) {
        Tcl_Obj *valueDict = NULL;
        valueDict = Tcl_NewDictObj();
        Tcl_DictObjPut(interp, valueDict, interpCtx->elswitch_pass, GLOBAL_SWITCH_ARG(&ctx, GLOBAL_SWITCH_PASS));
        Tcl_DictObjPut(interp, argDefCtx->defDict, Tcl_DuplicateObj(interpCtx->misc_emptyStrObj), valueDict);
    }
//***    Reorder parameters to have required parameters first if -pfirst global switch is specified
    if (HAS_GLOBAL_SWITCH(&ctx, GLOBAL_SWITCH_PFIRST)) {
        Tcl_Obj *orderReq = Tcl_NewListObj(0, NULL);
        Tcl_Obj *orderOpt = Tcl_NewListObj(0, NULL);
        Tcl_Size orderListLen;
        Tcl_Obj **orderListElems;
        Tcl_ListObjGetElements(interp, argDefCtx->orderList, &orderListLen, &orderListElems);
        for (Tcl_Size j = 0; j < orderListLen; ++j) {
            Tcl_Obj *name = orderListElems[j];
            Tcl_Obj *innerDict = NULL;
            Tcl_DictObjGet(interp, argDefCtx->defDict, name, &innerDict);
            if (DictKeyExists(interp, innerDict, interpCtx->elswitch_required)) {
                Tcl_ListObjAppendElement(interp, orderReq, name);
            } else {
                Tcl_ListObjAppendElement(interp, orderOpt, name);
            }
        }
        Tcl_Size lenOrderReq, lenOrderOpt;
        Tcl_ListObjLength(interp, orderReq, &lenOrderReq);
        Tcl_ListObjLength(interp, orderOpt, &lenOrderOpt);
        if ((lenOrderReq > 0) && (lenOrderOpt > 0)) {
            Tcl_DecrRefCount(argDefCtx->orderList);
            argDefCtx->orderList = MergeTwoLists(interp, orderReq, orderOpt);
        } else if (lenOrderReq > 0) {
            Tcl_DecrRefCount(argDefCtx->orderList);
            argDefCtx->orderList = orderReq;
        } else if (lenOrderOpt > 0) {
            Tcl_DecrRefCount(argDefCtx->orderList);
            argDefCtx->orderList = orderOpt;
        }
    }

//***    Force required parameters to bypass switch logic
    Tcl_Size end, start;
    Tcl_Obj *force = Tcl_DuplicateObj(argv);
    end = lenArgv - 1;
    start = 0;
    if (!HAS_GLOBAL_SWITCH(&ctx, GLOBAL_SWITCH_MIXED)) {
        if (HAS_GLOBAL_SWITCH(&ctx, GLOBAL_SWITCH_PFIRST)) {
            Tcl_Size orderListLen;
            Tcl_Obj **orderListElems;
            Tcl_ListObjGetElements(interp, argDefCtx->orderList, &orderListLen, &orderListElems);
            for (Tcl_Size j = 0; j < orderListLen; ++j) {
                Tcl_Obj *name = orderListElems[j];
                Tcl_Obj *innerDict = NULL;
                Tcl_DictObjGet(interp, argDefCtx->defDict, name, &innerDict);
                if (DictKeyExists(interp, innerDict, interpCtx->elswitch_required)) {
                    start++;
                }
            }
            Tcl_ListObjReplace(interp, force, start, lenArgv - start, 0, NULL);
            argv = ListRange(interp, argv, start, end);
        } else {
            Tcl_Size orderListLen;
            Tcl_Obj **orderListElems;
            Tcl_ListObjGetElements(interp, argDefCtx->orderList, &orderListLen, &orderListElems);
            for (Tcl_Size j = 0; j < orderListLen; ++j) {
                Tcl_Obj *name = orderListElems[j];
                Tcl_Obj *innerDict = NULL;
                Tcl_DictObjGet(interp, argDefCtx->defDict, name, &innerDict);
                if (DictKeyExists(interp, innerDict, interpCtx->elswitch_required)) {
                    end--;
                }
            }
            Tcl_ListObjReplace(interp, force, 0, end + 1, 0, NULL);
            argv = ListRange(interp, argv, 0, end);
        }
    } else {
        Tcl_ListObjReplace(interp, force, start, end - start + 1, 0, NULL);
        argv = ListRange(interp, argv, start, end);
    }

//***    Perform switch logic
    Tcl_Obj *resultDict = Tcl_NewDictObj();
    Tcl_Obj *missingList = Tcl_NewListObj(0, NULL);
    Tcl_Obj *params = NULL;
    Tcl_Size lenSwitchesList;
    Tcl_ListObjLength(interp, argDefCtx->switchesList, &lenSwitchesList);
    if (lenSwitchesList > 0) {
//****       Build regular expression to match switches.
        Tcl_Obj *re = Tcl_NewStringObj("^-", -1);
        if (HAS_GLOBAL_SWITCH(&ctx, GLOBAL_SWITCH_LONG)) {
            Tcl_AppendStringsToObj(re, "-?", NULL);
        }
        Tcl_AppendStringsToObj(re, "(\\w[\\w-]*)", NULL);
        if (HAS_GLOBAL_SWITCH(&ctx, GLOBAL_SWITCH_EQUALARG)) {
            Tcl_AppendStringsToObj(re, "(?:(=)(.*))?", NULL);
        } else {
            Tcl_AppendStringsToObj(re, "()()", NULL);
        }
        Tcl_AppendStringsToObj(re, "$", NULL);
        Tcl_RegExp regexpRe = Tcl_GetRegExpFromObj(interp, re, TCL_REG_ADVANCED);

//****       Process switches, and build the list of parameter arguments
        Tcl_Size argc;
        params = Tcl_NewListObj(0, NULL);
        while (1) {
            Tcl_ListObjLength(interp, argv, &argc);
            if (argc == 0) {
                break;
            }
//*****          Check if this argument appears to be a switch
            // emulate `set argv [lassign $argv arg]`
            Tcl_Obj *arg = NULL;
            if (Tcl_ListObjIndex(interp, argv, 0, &arg) != TCL_OK || arg == NULL) {
                goto cleanupOnError;
            }
            Tcl_IncrRefCount(arg); // we own it now
            if (Tcl_IsShared(argv)) {
                argv = Tcl_DuplicateObj(argv);
            }
            Tcl_ListObjReplace(interp, argv, 0, 1, 0, NULL); // remove first item
            Tcl_Obj *matchList = NULL, *name = NULL, *equal = NULL, *val = NULL;
            if (EvalMatchRegexpGroups(interp, regexpRe, arg, interpCtx, &matchList) == TCL_OK) {
                Tcl_ListObjIndex(interp, matchList, 1, &name);
                Tcl_ListObjIndex(interp, matchList, 2, &equal);
                Tcl_ListObjIndex(interp, matchList, 3, &val);
            } else if (strcmp(Tcl_GetString(arg), "--") == 0) {
                params = argv;
                Tcl_DecrRefCount(arg);
                break;
            } else if (HAS_GLOBAL_SWITCH(&ctx, GLOBAL_SWITCH_MIXED) || HAS_GLOBAL_SWITCH(&ctx, GLOBAL_SWITCH_PFIRST)) {
                Tcl_ListObjAppendElement(interp, params, arg);
                Tcl_DecrRefCount(arg);
                continue;
            } else {
                Tcl_Obj *elems[1] = {arg};
                if (Tcl_IsShared(argv)) {
                    argv = Tcl_DuplicateObj(argv);
                }
                Tcl_ListObjReplace(interp, argv, 0, 0, 1, elems);
                params = argv;
                Tcl_DecrRefCount(arg);
                break;
            }
            Tcl_DecrRefCount(arg); // Final cleanup if no break/continue occurred

//*****          Process switch aliases
            Tcl_Obj *aliasName = NULL;
            if (DICT_GET_IF_EXISTS(interp, argDefCtx->aliasesDict, name, &aliasName)) {
                name = aliasName;
            }
//*****          Preliminary guess for the normalized switch name
            Tcl_Obj *normal = Tcl_DuplicateObj(interpCtx->misc_dashStrObj);
            Tcl_AppendStringsToObj(normal, Tcl_GetString(name), NULL);
            Tcl_IncrRefCount(normal);
//*****          Perform switch name lookup
            Tcl_DictSearch search;
            Tcl_Obj *key = NULL, *data = NULL;
            Tcl_Obj *matchList1 = Tcl_NewListObj(0, NULL);
            int done;
            Tcl_DictObjFirst(interp, argDefCtx->defDict, &search, &key, &data, &done);
            while (!done) {
                if (DictKeyExists(interp, data, interpCtx->elswitch_switch)) {
                    Tcl_ListObjAppendElement(interp, matchList1, key);
                } else {
                    Tcl_DictObjNext(&search, &key, &data, &done);
                    continue;
                }
                Tcl_DictObjNext(&search, &key, &data, &done);
            }
            Tcl_DictObjDone(&search);
            Tcl_Obj *prefixName = NULL;
            int prefixCode =
                EvalPrefixMatch(interp, matchList1, name, 0, 1, Tcl_NewStringObj("switch", -1), 0, &prefixName);
            if (NestedDictKeyExists(interp, argDefCtx->defDict, name, interpCtx->elswitch_switch)) {
                // Exact match.  No additional lookup needed
                SAFE_DECR_REF(prefixName);
            } else if (!HAS_GLOBAL_SWITCH(&ctx, GLOBAL_SWITCH_EXACT) && prefixCode == TCL_OK) {
                // Use the switch whose prefix unambiguously matches
                name = prefixName;
                normal = Tcl_DuplicateObj(interpCtx->misc_dashStrObj);
                Tcl_AppendStringsToObj(normal, Tcl_GetString(name), NULL);
            } else if (DictKeyExists(interp, argDefCtx->defDict, interpCtx->misc_emptyStrObj)) {
                // Use default pass-through if defined
                name = interpCtx->misc_emptyStrObj;
                SAFE_DECR_REF(prefixName);
            } else {
                // Fail if this is an invalid switch
                Tcl_Obj *result = NULL;
                EvalLsort(interp, argDefCtx->switchesList, &result);
                argDefCtx->switchesList = result;
                Tcl_SetObjResult(interp, BuildBadSwitchError(interp, arg, argDefCtx->switchesList));
                goto cleanupOnError;
            }
//*****          If the switch is standalone, ignore all constraints
            if (NestedDictKeyExists(interp, argDefCtx->defDict, name, interpCtx->elswitch_standalone)) {
                Tcl_Obj **keyObjs;
                Tcl_Size keyCount;
                DictKeys(interp, argDefCtx->defDict, &keyCount, &keyObjs);
                for (Tcl_Size idx = 0; idx < keyCount; ++idx) {
                    Tcl_Obj *other = keyObjs[idx];
                    UnsetNestedDictKey(interp, argDefCtx->defDict, other, interpCtx->elswitch_required);
                    UnsetNestedDictKey(interp, argDefCtx->defDict, other, interpCtx->elswitch_require);
                    UnsetNestedDictKey(interp, argDefCtx->defDict, other, interpCtx->elswitch_forbid);
                    UnsetNestedDictKey(interp, argDefCtx->defDict, other, interpCtx->elswitch_allow);
                    if (NestedDictKeyExists(interp, argDefCtx->defDict, other, interpCtx->elswitch_parameter)) {
                        SetNestedDictKey(interp, argDefCtx->defDict, other, interpCtx->elswitch_optional,
                                         interpCtx->misc_emptyStrObj);
                    }
                }
                free(keyObjs);
            }
//*****          Keep track of which elements are present
            SetNestedDictKey(interp, argDefCtx->defDict, name, interpCtx->misc_presentSwitchObj,
                             interpCtx->misc_emptyStrObj);
            if (strcmp(Tcl_GetString(equal), "=") == 0) {
                Tcl_Obj *elems[1] = {val};
                if (Tcl_IsShared(argv)) {
                    argv = Tcl_DuplicateObj(argv);
                }
                Tcl_ListObjReplace(interp, argv, 0, 0, 1, elems);
            }
//*****          Load key and pass into local variables for easy access
            Tcl_Obj *keyLoc = NULL;
            Tcl_Obj *passLoc = NULL;
            if (NestedDictKeyExists(interp, argDefCtx->defDict, name, interpCtx->elswitch_key)) {
                GetNestedDictValue(interp, argDefCtx->defDict, name, interpCtx->elswitch_key, &keyLoc);
            }
            if (NestedDictKeyExists(interp, argDefCtx->defDict, name, interpCtx->elswitch_pass)) {
                GetNestedDictValue(interp, argDefCtx->defDict, name, interpCtx->elswitch_pass, &passLoc);
            }
//*****          Keep track of which switches have been seen
            Tcl_DictObjRemove(interp, argDefCtx->omittedDict, name);
//*****          Validate switch arguments and store values into the result dict
            Tcl_Size argvLen;
            Tcl_ListObjLength(interp, argv, &argvLen);
            if (NestedDictKeyExists(interp, argDefCtx->defDict, name, interpCtx->elswitch_catchall)) {
                // The switch is catchall, so store all remaining arguments.
                // validate
                Tcl_Obj *resultList = NULL;
                Tcl_Obj *elementDefDict = NULL;
                Tcl_DictObjGet(interp, argDefCtx->defDict, name, &elementDefDict);
                if (ValidateHelper(interp, &ctx, normal, elementDefDict, argv, interpCtx, 1, &resultList)
                    != TCL_OK) {
                    Tcl_DecrRefCount(normal);
                    goto cleanupOnError;
                }
                argv = resultList;
                // type check
                if (TypeChecker(interp, normal, elementDefDict, argv, interpCtx, 1, &resultList) != TCL_OK) {
                    Tcl_DecrRefCount(normal);
                    goto cleanupOnError;
                }
                argv = resultList;
                if (keyLoc != NULL) {
                    Tcl_DictObjPut(interp, resultDict, keyLoc, argv);
                }
                if (passLoc != NULL) {
                    if (HAS_GLOBAL_SWITCH(&ctx, GLOBAL_SWITCH_NORMALIZE)) {
                        DictLappend(interp, resultDict, passLoc, MergeTwoLists(interp, normal, argv));
                    } else {
                        DictLappend(interp, resultDict, passLoc, MergeTwoLists(interp, arg, argv));
                    }
                }
                break;
            } else if (!NestedDictKeyExists(interp, argDefCtx->defDict, name, interpCtx->elswitch_argument)) {
                // The switch expects no arguments.
                if (strcmp(Tcl_GetString(equal), "=") == 0) {
                    Tcl_Obj *msg = Tcl_DuplicateObj(normal);
                    Tcl_AppendStringsToObj(msg, " doesn't allow an argument", NULL);
                    Tcl_SetObjResult(interp, msg);
                    Tcl_DecrRefCount(normal);
                    goto cleanupOnError;
                }
                if (keyLoc != NULL) {
                    if (NestedDictKeyExists(interp, argDefCtx->defDict, name, interpCtx->elswitch_value)) {
                        Tcl_Obj *valueLoc = NULL;
                        GetNestedDictValue(interp, argDefCtx->defDict, name, interpCtx->elswitch_value, &valueLoc);
                        Tcl_DictObjPut(interp, resultDict, keyLoc, valueLoc);
                    } else {
                        Tcl_DictObjPut(interp, resultDict, keyLoc, interpCtx->misc_emptyStrObj);
                    }
                }
                if (passLoc != NULL) {
                    if (HAS_GLOBAL_SWITCH(&ctx, GLOBAL_SWITCH_NORMALIZE)) {
                        DictLappendElem(interp, resultDict, passLoc, normal);
                    } else {
                        DictLappendElem(interp, resultDict, passLoc, arg);
                    }
                }
            } else if (argvLen > 0) {
                // The switch was given the expected argument.
                Tcl_Obj *resultList = NULL;
                Tcl_Obj *argLoc = NULL;
                Tcl_Obj *elementDefDict = NULL;
                Tcl_DictObjGet(interp, argDefCtx->defDict, name, &elementDefDict);
                Tcl_ListObjIndex(interp, argv, 0, &argLoc);
                Tcl_IncrRefCount(elementDefDict);
                if (ValidateHelper(interp, &ctx, normal, elementDefDict, argLoc, interpCtx, 0, &resultList)
                    != TCL_OK) {
                    Tcl_DecrRefCount(elementDefDict);
                    Tcl_DecrRefCount(normal);
                    goto cleanupOnError;
                }
                argLoc = resultList;
                if (TypeChecker(interp, normal, elementDefDict, argLoc, interpCtx, 0, &resultList) != TCL_OK) {
                    Tcl_DecrRefCount(elementDefDict);
                    Tcl_DecrRefCount(normal);
                    goto cleanupOnError;
                }
                if (keyLoc != NULL) {
                    if (NestedDictKeyExists(interp, argDefCtx->defDict, name, interpCtx->elswitch_optional)) {
                        Tcl_Obj *list = Tcl_NewListObj(0, NULL);
                        Tcl_Obj *emptyList = Tcl_NewListObj(0, NULL);
                        Tcl_ListObjAppendElement(interp, list, emptyList);
                        Tcl_ListObjAppendElement(interp, list, resultList);
                        Tcl_DictObjPut(interp, resultDict, keyLoc, list);
                    } else {
                        Tcl_DictObjPut(interp, resultDict, keyLoc, resultList);
                    }
                }
                if (passLoc != NULL) {
                    if (HAS_GLOBAL_SWITCH(&ctx, GLOBAL_SWITCH_NORMALIZE)) {
                        Tcl_Obj *objv[2];
                        objv[0] = normal;
                        objv[1] = resultList;
                        DictLappend(interp, resultDict, passLoc, Tcl_NewListObj(2, objv));
                    } else if (strcmp(Tcl_GetString(equal), "=") == 0) {
                        DictLappendElem(interp, resultDict, passLoc, arg);
                    } else {
                        Tcl_Obj *objv[2];
                        objv[0] = arg;
                        objv[1] = argLoc;
                        DictLappend(interp, resultDict, passLoc, Tcl_NewListObj(2, objv));
                    }
                }
                argv = ListRange(interp, argv, 1, end);
                Tcl_DecrRefCount(elementDefDict);
            } else {
                // The switch was not given the expected argument
                if (!NestedDictKeyExists(interp, argDefCtx->defDict, name, interpCtx->elswitch_optional)) {
                    Tcl_Obj *msg = Tcl_DuplicateObj(normal);
                    Tcl_AppendStringsToObj(msg, " requires an argument", NULL);
                    Tcl_SetObjResult(interp, msg);
                    goto cleanupOnError;
                }
                if (keyLoc != NULL) {
                    Tcl_DictObjPut(interp, resultDict, keyLoc, interpCtx->misc_emptyStrObj);
                }
                if (passLoc != NULL) {
                    if (HAS_GLOBAL_SWITCH(&ctx, GLOBAL_SWITCH_NORMALIZE)) {
                        DictLappendElem(interp, resultDict, passLoc, normal);
                    } else {
                        DictLappendElem(interp, resultDict, passLoc, arg);
                    }
                }
            }
            // Insert this switch's implied arguments into the argument list
            if (NestedDictKeyExists(interp, argDefCtx->defDict, name, interpCtx->elswitch_imply)) {
                Tcl_Obj *implyName = NULL;
                GetNestedDictValue(interp, argDefCtx->defDict, name, interpCtx->elswitch_imply, &implyName);
                Tcl_Size valueLen;
                Tcl_Obj **valueElems;
                Tcl_ListObjGetElements(interp, implyName, &valueLen, &valueElems);
                if (Tcl_IsShared(argv)) {
                    argv = Tcl_DuplicateObj(argv);
                }
                Tcl_ListObjReplace(interp, argv, 0, 0, valueLen, valueElems);
                UnsetNestedDictKey(interp, argDefCtx->defDict, name, interpCtx->elswitch_imply);
            }
        }
//*****          Build list of missing required switches
        Tcl_DictSearch search;
        Tcl_Obj *name = NULL, *opt = NULL;
        int done;
        Tcl_DictObjFirst(interp, argDefCtx->defDict, &search, &name, &opt, &done);
        while (!done) {
            if (DictKeyExists(interp, opt, interpCtx->elswitch_switch) &&
                !DictKeyExists(interp, opt, interpCtx->misc_presentSwitchObj) &&
                DictKeyExists(interp, opt, interpCtx->elswitch_required)) {
                if (DictKeyExists(interp, opt, interpCtx->elswitch_alias)) {
                    Tcl_ListObjAppendElement(interp, missingList, BuildAliasJoinString(interp, opt, name, interpCtx));
                } else {
                    Tcl_Obj *dashName = Tcl_DuplicateObj(interpCtx->misc_dashStrObj);
                    Tcl_AppendStringsToObj(dashName, Tcl_GetString(name), NULL);
                    Tcl_ListObjAppendElement(interp, missingList, dashName);
                }
            }
            Tcl_DictObjNext(&search, &name, &opt, &done);
        }
        Tcl_DictObjDone(&search);
//*****          Fail if at least one required switch is missing
        Tcl_Size missingListLen;
        Tcl_ListObjLength(interp, missingList, &missingListLen);
        if (missingListLen > 0) {
            Tcl_Obj *result = NULL;
            EvalLsort(interp, missingList, &result);
            Tcl_IncrRefCount(result);
            missingList = result;
            Tcl_SetObjResult(interp, BuildMissingSwitchesError(interp, missingList));
            goto cleanupOnError;
        }
    } else {
        // If no switches are defined, bypass the switch logic and process all arguments using the parameter logic.
        params = argv;
    }
//***    Allocate one argument to each required parameter, including catchalls
    Tcl_Obj *allocDict = Tcl_NewDictObj();
    if (HAS_GLOBAL_SWITCH(&ctx, GLOBAL_SWITCH_PFIRST)) {
        Tcl_Size forceLen;
        Tcl_Obj **forceElems;
        Tcl_ListObjGetElements(interp, force, &forceLen, &forceElems);
        if (Tcl_IsShared(params)) {
            params = Tcl_DuplicateObj(params);
        }
        Tcl_ListObjReplace(interp, params, 0, 0, forceLen, forceElems);
    } else {
        params = MergeTwoLists(interp, params, force);
    }
    Tcl_Size count;
    Tcl_ListObjLength(interp, params, &count);
    Tcl_Size orderListLen;
    Tcl_Obj **orderListElems;
    Tcl_ListObjGetElements(interp, argDefCtx->orderList, &orderListLen, &orderListElems);
    for (Tcl_Size j = 0; j < orderListLen; ++j) {
        Tcl_Obj *name = orderListElems[j];
        if (NestedDictKeyExists(interp, argDefCtx->defDict, name, interpCtx->elswitch_required)) {
            if (count > 0) {
                Tcl_DictObjPut(interp, allocDict, name, Tcl_NewWideIntObj(1));
                Tcl_DictObjRemove(interp, argDefCtx->omittedDict, name);
                SetNestedDictKey(interp, argDefCtx->defDict, name, interpCtx->misc_presentSwitchObj,
                                 interpCtx->misc_emptyStrObj);
                count--;
            } else {
                Tcl_ListObjAppendElement(interp, missingList, name);
            }
        }
    }
//***    Fail if at least one required parameter is missing
    Tcl_Size missingListLen;
    Tcl_ListObjLength(interp, missingList, &missingListLen);
    if (missingListLen > 0) {
        Tcl_SetObjResult(interp, BuildMissingParameterError(interp, missingList));
        goto cleanupOnError;
    }
//***    Try to allocate one argument to each optional, non-catchall parameter, until there are no arguments left
    if (count > 0) {
        for (Tcl_Size j = 0; j < orderListLen; ++j) {
            Tcl_Obj *name = orderListElems[j];
            if (!NestedDictKeyExists(interp, argDefCtx->defDict, name, interpCtx->elswitch_required) &&
                !NestedDictKeyExists(interp, argDefCtx->defDict, name, interpCtx->elswitch_catchall)) {
                Tcl_DictObjPut(interp, allocDict, name, Tcl_NewWideIntObj(1));
                Tcl_DictObjRemove(interp, argDefCtx->omittedDict, name);
                SetNestedDictKey(interp, argDefCtx->defDict, name, interpCtx->misc_presentSwitchObj,
                                 interpCtx->misc_emptyStrObj);
                if (--count == 0) {
                    break;
                }
            }
        }
    }
//***    Process excess arguments
    if (count > 0) {
        if (argDefCtx->catchall != NULL) {
            // Allocate remaining arguments to the catchall parameter
            Tcl_Obj *catchallName = argDefCtx->catchall;
            DictIncr(interp, allocDict, catchallName, Tcl_NewWideIntObj(count));
            Tcl_DictObjRemove(interp, argDefCtx->omittedDict, catchallName);
        } else if (DictKeyExists(interp, argDefCtx->defDict, interpCtx->misc_emptyStrObj)) {
            // If there is no catchall parameter, instead allocate to the default pass-through result key.
            Tcl_ListObjAppendElement(interp, argDefCtx->orderList, interpCtx->misc_emptyStrObj);
            Tcl_DictObjPut(interp, allocDict, interpCtx->misc_emptyStrObj, Tcl_NewWideIntObj(count));
        } else {
            Tcl_SetObjResult(interp, Tcl_NewStringObj("too many arguments", -1));
            goto cleanupOnError;
        }
    }
//***    Check constraints
    Tcl_DictSearch search;
    Tcl_Obj *name = NULL, *opt = NULL;
    int done;
    Tcl_DictObjFirst(interp, argDefCtx->defDict, &search, &name, &opt, &done);
    while (!done) {
        if (DictKeyExists(interp, opt, interpCtx->misc_presentSwitchObj)) {
            int match = 1;
            Tcl_Obj *condition = Tcl_NewStringObj("require", -1);
            Tcl_Obj *description = Tcl_NewStringObj("requires", -1);
            Tcl_Obj *otherNameList = NULL;
            if (DICT_GET_IF_EXISTS(interp, opt, condition, &otherNameList)) {
                Tcl_Size otherNameListLen;
                Tcl_Obj **otherNameListElems;
                Tcl_ListObjGetElements(interp, otherNameList, &otherNameListLen, &otherNameListElems);
                for (Tcl_Size j = 0; j < otherNameListLen; ++j) {
                    Tcl_Obj *otherName = otherNameListElems[j];
                    if (NestedDictKeyExists(interp, argDefCtx->defDict, otherName, interpCtx->misc_presentSwitchObj) !=
                        match) {
                        Tcl_Obj *dashName = NULL;
                        Tcl_Obj *dashOtherName = NULL;
                        if (NestedDictKeyExists(interp, argDefCtx->defDict, name, interpCtx->elswitch_switch)) {
                            dashName = Tcl_DuplicateObj(interpCtx->misc_dashStrObj);
                            Tcl_AppendStringsToObj(dashName, Tcl_GetString(name), NULL);
                        } else {
                            dashName = name;
                        }
                        if (NestedDictKeyExists(interp, argDefCtx->defDict, otherName, interpCtx->elswitch_switch)) {
                            dashOtherName = Tcl_DuplicateObj(interpCtx->misc_dashStrObj);
                            Tcl_AppendStringsToObj(dashOtherName, Tcl_GetString(otherName), NULL);
                        } else {
                            dashOtherName = otherName;
                        }
                        Tcl_Obj *msg = Tcl_DuplicateObj(dashName);
                        Tcl_AppendStringsToObj(msg, " ", Tcl_GetString(description), " ", Tcl_GetString(dashOtherName),
                                               NULL);
                        Tcl_SetObjResult(interp, msg);
                        goto cleanupOnError;
                    }
                }
            }
            match = 0;
            condition = Tcl_NewStringObj("forbid", -1);
            description = Tcl_NewStringObj("conflicts with", -1);
            if (DICT_GET_IF_EXISTS(interp, opt, condition, &otherNameList)) {
                Tcl_Size otherNameListLen;
                Tcl_Obj **otherNameListElems;
                Tcl_ListObjGetElements(interp, otherNameList, &otherNameListLen, &otherNameListElems);
                for (Tcl_Size j = 0; j < otherNameListLen; ++j) {
                    Tcl_Obj *otherName = otherNameListElems[j];
                    if (NestedDictKeyExists(interp, argDefCtx->defDict, otherName, interpCtx->misc_presentSwitchObj) !=
                        match) {
                        Tcl_Obj *dashName = NULL;
                        Tcl_Obj *dashOtherName = NULL;
                        if (NestedDictKeyExists(interp, argDefCtx->defDict, name, interpCtx->elswitch_switch)) {
                            dashName = Tcl_DuplicateObj(interpCtx->misc_dashStrObj);
                            Tcl_AppendStringsToObj(dashName, Tcl_GetString(name), NULL);
                        } else {
                            dashName = name;
                        }
                        if (NestedDictKeyExists(interp, argDefCtx->defDict, otherName, interpCtx->elswitch_switch)) {
                            dashOtherName = Tcl_DuplicateObj(interpCtx->misc_dashStrObj);
                            Tcl_AppendStringsToObj(dashOtherName, Tcl_GetString(otherName), NULL);
                        } else {
                            dashOtherName = otherName;
                        }
                        Tcl_Obj *msg = Tcl_DuplicateObj(dashName);
                        Tcl_AppendStringsToObj(msg, " ", Tcl_GetString(description), " ", Tcl_GetString(dashOtherName),
                                               NULL);
                        Tcl_SetObjResult(interp, msg);
                        goto cleanupOnError;
                    }
                }
            }
        }
        Tcl_DictObjNext(&search, &name, &opt, &done);
    }
    Tcl_DictObjDone(&search);

    Tcl_Obj *presentedNames = Tcl_NewListObj(0, NULL);
    Tcl_DictObjFirst(interp, argDefCtx->defDict, &search, &name, &opt, &done);
    while (!done) {
        if (DictKeyExists(interp, opt, interpCtx->misc_presentSwitchObj)) {
            Tcl_ListObjAppendElement(interp, presentedNames, name);
        }
        Tcl_DictObjNext(&search, &name, &opt, &done);
    }
    Tcl_DictObjDone(&search);

    Tcl_DictObjFirst(interp, argDefCtx->defDict, &search, &name, &opt, &done);
    while (!done) {
        if (DictKeyExists(interp, opt, interpCtx->misc_presentSwitchObj)) {
            Tcl_Obj *allowedNames = NULL;
             if (DICT_GET_IF_EXISTS(interp, opt, interpCtx->elswitch_allow, &allowedNames)) {
                Tcl_Size presentedNamesListLen;
                Tcl_Obj **presentedNamesListElems;
                Tcl_ListObjGetElements(interp, presentedNames, &presentedNamesListLen, &presentedNamesListElems);
                for (Tcl_Size j = 0; j < presentedNamesListLen; ++j) {
                    Tcl_Obj *presentedName = presentedNamesListElems[j];
                    if (strcmp(Tcl_GetString(presentedName), Tcl_GetString(name)) == 0) {
                        continue;
                    }
                    if (!InList(interp, presentedName, allowedNames)) {
                        Tcl_Obj *msg = Tcl_DuplicateObj(name);
                        Tcl_AppendStringsToObj(msg, " doesn't allow ", Tcl_GetString(presentedName), NULL);
                        Tcl_SetObjResult(interp, msg);
                        goto cleanupOnError;
                    }
                }
            }
        }
        Tcl_DictObjNext(&search, &name, &opt, &done);
    }
    Tcl_DictObjDone(&search);

//***    If normalization is enabled, explicitly store into the pass-through keys
    // all omitted switches that have a pass-through key, accept an argument, and have a default value
    if (HAS_GLOBAL_SWITCH(&ctx, GLOBAL_SWITCH_NORMALIZE)) {
        Tcl_DictSearch search;
        Tcl_Obj *name = NULL, *opt = NULL;
        int done;
        Tcl_DictObjFirst(interp, argDefCtx->defDict, &search, &name, &opt, &done);
        while (!done) {
            Tcl_Obj *switchLoc = NULL;
            Tcl_Obj *passLoc = NULL;
            Tcl_Obj *argLoc = NULL;
            Tcl_Obj *defaultLoc = NULL;
            Tcl_Obj *nameLoc = NULL;
            if (DICT_GET_IF_EXISTS(interp, opt, interpCtx->elswitch_switch, &switchLoc) && 
                DICT_GET_IF_EXISTS(interp, opt, interpCtx->elswitch_pass, &passLoc) &&
                DICT_GET_IF_EXISTS(interp, opt, interpCtx->elswitch_argument, &argLoc) &&
                DICT_GET_IF_EXISTS(interp, opt, interpCtx->elswitch_default, &defaultLoc) &&
                DICT_GET_IF_EXISTS(interp, argDefCtx->omittedDict, name, &nameLoc)) {
                nameLoc = Tcl_DuplicateObj(interpCtx->misc_dashStrObj);
                Tcl_AppendStringsToObj(nameLoc, Tcl_GetString(name), NULL);
                Tcl_Obj *objv[2];
                objv[0] = nameLoc;
                objv[1] = defaultLoc;
                DictLappend(interp, resultDict, passLoc, Tcl_NewListObj(2, objv));
            }
            Tcl_DictObjNext(&search, &name, &opt, &done);
        }
        Tcl_DictObjDone(&search);
    }
//***    Validate parameters and store in result dict
    Tcl_ListObjGetElements(interp, argDefCtx->orderList, &orderListLen, &orderListElems);
    Tcl_Size indx = 0;
    for (Tcl_Size j = 0; j < orderListLen; ++j) {
        Tcl_Obj *name = orderListElems[j];
        Tcl_Obj *opt = NULL;
        Tcl_DictObjGet(interp, argDefCtx->defDict, name, &opt);
        Tcl_Obj *passLoc = NULL;
        Tcl_Obj *defaultLoc = NULL;
        if (DictKeyExists(interp, allocDict, name)) {
            Tcl_Obj *val = NULL;
            Tcl_Size nameLen;
            Tcl_ListObjLength(interp, name, &nameLen);
            if (!DictKeyExists(interp, opt, interpCtx->elswitch_catchall) && (nameLen > 0)) {
                Tcl_Obj *parami = NULL;
                Tcl_Obj *resultList = NULL;
                Tcl_ListObjIndex(interp, params, indx, &parami);
                Tcl_IncrRefCount(parami);
                if (ValidateHelper(interp, &ctx, name, opt, parami, interpCtx, 0, &resultList) != TCL_OK) {
                    Tcl_DecrRefCount(parami);
                    goto cleanupOnError;
                }
                val = resultList;
                if (TypeChecker(interp, name, opt, val, interpCtx, 0, &resultList) != TCL_OK) {
                    Tcl_DecrRefCount(parami);
                    goto cleanupOnError;
                }
                val = resultList;
                Tcl_Obj *passLoc = NULL;
                if (DICT_GET_IF_EXISTS(interp, opt, interpCtx->elswitch_pass, &passLoc)) {
                    char *str = Tcl_GetString(val);
                    if ((str[0] == '-') && !DictKeyExists(interp, resultDict, passLoc)) {
                        DictLappendElem(interp, resultDict, passLoc, interpCtx->misc_doubleDashStrObj);
                    }
                    
                    DictLappendElem(interp, resultDict, passLoc, val);
                }
                indx++;
                Tcl_DecrRefCount(parami);
            } else {
                Tcl_Obj *allocValLoc = NULL;
                Tcl_DictObjGet(interp, allocDict, name, &allocValLoc);
                Tcl_WideInt step;
                Tcl_GetWideIntFromObj(interp, allocValLoc, &step);
                val = ListRange(interp, params, indx, indx + step - 1);
                if (nameLen > 0) {
                    Tcl_Obj *resultList = NULL;
                    if (ValidateHelper(interp, &ctx, name, opt, val, interpCtx, 1, &resultList) != TCL_OK) {
                        Tcl_DecrRefCount(val);
                        goto cleanupOnError;
                    }
                    val = resultList;
                    if (TypeChecker(interp, name, opt, val, interpCtx, 1, &resultList) != TCL_OK) {
                        Tcl_DecrRefCount(val);
                        goto cleanupOnError;
                    }
                    val = resultList;
                }
                if (DictKeyExists(interp, opt, interpCtx->elswitch_pass)) {
                    Tcl_Obj *val0 = NULL;
                    Tcl_ListObjIndex(interp, val, 0, &val0);
                    Tcl_Obj *passLoc = NULL;
                    Tcl_DictObjGet(interp, opt, interpCtx->elswitch_pass, &passLoc);
                    char *str = Tcl_GetString(val0);
                    if ((str[0] == '-') && !DictKeyExists(interp, resultDict, passLoc)) {
                        DictLappendElem(interp, resultDict, passLoc, interpCtx->misc_doubleDashStrObj);
                    }
                    DictLappend(interp, resultDict, passLoc, val);
                }
                indx = indx + step;
            }
            Tcl_Obj *keyLoc = NULL;
            if (DICT_GET_IF_EXISTS(interp, opt, interpCtx->elswitch_key, &keyLoc)) {
                Tcl_DictObjPut(interp, resultDict, keyLoc, val);
            }
        } else if (HAS_GLOBAL_SWITCH(&ctx, GLOBAL_SWITCH_NORMALIZE) &&
                   DICT_GET_IF_EXISTS(interp, opt, interpCtx->elswitch_default, &defaultLoc) &&
                   DICT_GET_IF_EXISTS(interp, opt, interpCtx->elswitch_pass, &passLoc)) {
            char *str = Tcl_GetString(defaultLoc);
            if ((str[0] == '-') && !DictKeyExists(interp, resultDict, passLoc)) {
                DictLappendElem(interp, resultDict, passLoc, interpCtx->misc_doubleDashStrObj);
            }
            DictLappendElem(interp, resultDict, passLoc, defaultLoc);
        }
    }
//***    Create default values for missing elements
    Tcl_DictObjFirst(interp, argDefCtx->defDict, &search, &name, &opt, &done);
    while (!done) {
        Tcl_Obj *keyLoc = NULL;
        if (DICT_GET_IF_EXISTS(interp, opt, interpCtx->elswitch_key, &keyLoc)) {
            if (!DictKeyExists(interp, resultDict, keyLoc)) {
                Tcl_Obj *defaultLoc;
                if (DICT_GET_IF_EXISTS(interp, opt, interpCtx->elswitch_default, &defaultLoc)) {
                    Tcl_DictObjPut(interp, resultDict, keyLoc, defaultLoc);
                } else if (DictKeyExists(interp, opt, interpCtx->elswitch_catchall)) {
                    Tcl_DictObjPut(interp, resultDict, keyLoc, interpCtx->misc_emptyStrObj);
                }
            }
        }
        Tcl_Obj *passLoc = NULL;
        if (DICT_GET_IF_EXISTS(interp, opt, interpCtx->elswitch_pass, &passLoc)) {
            if (!DictKeyExists(interp, resultDict, passLoc)) {
                Tcl_DictObjPut(interp, resultDict, passLoc, interpCtx->misc_emptyStrObj);
            }
        }
        Tcl_DictObjNext(&search, &name, &opt, &done);
    }
    Tcl_DictObjDone(&search);
    if (HAS_GLOBAL_SWITCH(&ctx, GLOBAL_SWITCH_INLINE)) {
//****       Return result dict
        Tcl_SetObjResult(interp, resultDict);
        FreeGlobalSwitches(&ctx);
        FreeArgumentDefinition(argDefCtx);
        return TCL_OK;
    } else {
//****       Unless -keep was used, unset caller variables for omitted elements
        if (!HAS_GLOBAL_SWITCH(&ctx, GLOBAL_SWITCH_KEEP)) {
            Tcl_DictSearch search;
            Tcl_Obj *name = NULL, *val = NULL;
            int done;
            Tcl_DictObjFirst(interp, argDefCtx->omittedDict, &search, &name, &val, &done);
            while (!done) {
                Tcl_Obj *opt = NULL;
                Tcl_DictObjGet(interp, argDefCtx->defDict, name, &opt);
                Tcl_Obj *keyLoc = NULL;
                if (DICT_GET_IF_EXISTS(interp, opt, interpCtx->elswitch_key, &keyLoc)) {
                    if (!DictKeyExists(interp, opt, interpCtx->elswitch_keep) && !DictKeyExists(interp, resultDict, keyLoc)) {
                        Tcl_UnsetVar(interp, Tcl_GetString(keyLoc), 0);
                    }
                }
                Tcl_DictObjNext(&search, &name, &val, &done);
            }
            Tcl_DictObjDone(&search);
        }
//****       Process results
        Tcl_DictSearch search;
        Tcl_Obj *key = NULL, *val = NULL;
        int done;
        Tcl_DictObjFirst(interp, resultDict, &search, &key, &val, &done);
        while (!done) {
            Tcl_Obj *upvarKeyLoc = NULL;
            if (DICT_GET_IF_EXISTS(interp, argDefCtx->upvarsDict, key, &upvarKeyLoc)) {
                Tcl_Obj *levelLoc = NULL;
                GetNestedDictValue(interp, argDefCtx->defDict, upvarKeyLoc, interpCtx->elswitch_level, &levelLoc);
                if (Tcl_UpVar(interp, Tcl_GetString(levelLoc), Tcl_GetString(val), Tcl_GetString(key), 0) == TCL_ERROR) {
                    goto cleanupOnError;
                }
            } else {
                Tcl_ObjSetVar2(interp, key, NULL, val, 0);
            }
            Tcl_DictObjNext(&search, &key, &val, &done);
        }
        Tcl_DictObjDone(&search);
    }
    FreeGlobalSwitches(&ctx);
    FreeArgumentDefinition(argDefCtx);
    return TCL_OK;


cleanupOnError:
    SAFE_DECR_REF(definition);
    FreeGlobalSwitches(&ctx);
    if (argDefCtx != NULL) {
        FreeArgumentDefinition(argDefCtx);
    }
    return TCL_ERROR; 
}
