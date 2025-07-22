
#include <tcl.h>

//** useful macroses
#define DICT_GET_IF_EXISTS(interp, dictObj, keyObj, valPtr)                                                            \
    (Tcl_DictObjGet((interp), (dictObj), (keyObj), (valPtr)) == TCL_OK && *(valPtr) != NULL)

#define SAFE_DECR_REF_AND_NULL(objPtr)                                                                                 \
    do {                                                                                                               \
        if ((objPtr) != NULL) {                                                                                        \
            Tcl_DecrRefCount(objPtr);                                                                                  \
            (objPtr) = NULL;                                                                                           \
        }                                                                                                              \
    } while (0)

#define SAFE_DECR_REF(objPtr)                                                                                          \
    do {                                                                                                               \
        if ((objPtr) != NULL) {                                                                                        \
            Tcl_DecrRefCount(objPtr);                                                                                  \
        }                                                                                                              \
    } while (0)

#define SAFE_FREE_GLOBAL_SWITCHES(ctx)                                                                                 \
    do {                                                                                                               \
        if ((ctx) != NULL) {                                                                                           \
            FreeGlobalSwitches(ctx);                                                                                   \
        }                                                                                                              \
    } while (0)

#define PRINT_REF_COUNT(obj)                                                                                           \
    do {                                                                                                               \
        if (obj != NULL) {                                                                                             \
            printf("Reference count of Tcl_Obj \"%s\": %ld\n", Tcl_GetString(obj), obj->refCount);                     \
        } else {                                                                                                       \
            printf("Tcl_Obj is NULL\n");                                                                               \
        }                                                                                                              \
        fflush(stdout);                                                                                                \
    } while (0)

//** arguments definition structure
typedef struct {
    Tcl_Obj *defDict;
    Tcl_Obj *aliasesDict;
    Tcl_Obj *orderList;
    Tcl_Obj *switchesList;
    Tcl_Obj *upvarsDict;
    Tcl_Obj *omittedDict;
    Tcl_Obj *catchall;
} ArgumentDefinition;

//** A per-interpreter context for argparse.
typedef struct {
    Tcl_HashTable argDefHashTable; // arguments definition hash table
    Tcl_Obj *list_allowedTypes, *list_templateSubstNames, *list_helpGenSubstNames; // cashed list objects
    Tcl_Obj *misc_emptyStrObj, *misc_presentSwitchObj, *misc_validateMsgStrObj, *misc_dashStrObj,
        *misc_doubleDashStrObj; // cashed miscellanious objects
    Tcl_Obj *elswitch_alias, *elswitch_argument, *elswitch_boolean, *elswitch_catchall, *elswitch_default,
        *elswitch_enum, *elswitch_forbid, *elswitch_ignore, *elswitch_imply, *elswitch_keep, *elswitch_key,
        *elswitch_level, *elswitch_optional, *elswitch_parameter, *elswitch_pass, *elswitch_reciprocal,
        *elswitch_require, *elswitch_required, *elswitch_standalone, *elswitch_switch, *elswitch_upvar,
        *elswitch_validate, *elswitch_value, *elswitch_type, *elswitch_allow, *elswitch_help, *elswitch_errormsg,
        *elswitch_hsuppress; // cashed element switches objects
} ArgparseInterpCtx;

//** global switches declarations and definitions. Order MUST match globaSwitches[]
enum GlobalSwitchId {
    GLOBAL_SWITCH_BOOLEAN = 0,
    GLOBAL_SWITCH_ENUM,
    GLOBAL_SWITCH_EQUALARG,
    GLOBAL_SWITCH_EXACT,
    GLOBAL_SWITCH_INLINE,
    GLOBAL_SWITCH_KEEP,
    GLOBAL_SWITCH_LEVEL,
    GLOBAL_SWITCH_LONG,
    GLOBAL_SWITCH_MIXED,
    GLOBAL_SWITCH_NORMALIZE,
    GLOBAL_SWITCH_PASS,
    GLOBAL_SWITCH_RECIPROCAL,
    GLOBAL_SWITCH_TEMPLATE,
    GLOBAL_SWITCH_VALIDATE,
    GLOBAL_SWITCH_HELP,
    GLOBAL_SWITCH_HELPLEVEL,
    GLOBAL_SWITCH_PFIRST,
    GLOBAL_SWITCH_HELPRET,
    GLOBAL_SWITCH_COUNT // total number of global switches
};
static const char *globalSwitches[] = {"-boolean",   "-enum",       "-equalarg", "-exact",    "-inline",
                                       "-keep",      "-level",      "-long",     "-mixed",    "-normalize",
                                       "-pass",      "-reciprocal", "-template", "-validate", "-help",
                                       "-helplevel", "-pfirst",     "-helpret",  NULL};
typedef struct {
    int globalSwitches;                   // bitmask of which global switch are present
    Tcl_Obj *values[GLOBAL_SWITCH_COUNT]; // arguments for value-carrying global switches
} GlobalSwitchesContext;
// global switches that require arguments
#define GLOBAL_SWITCH_TAKES_ARG_MASK                                                                                   \
    ((1 << GLOBAL_SWITCH_ENUM) | (1 << GLOBAL_SWITCH_LEVEL) | (1 << GLOBAL_SWITCH_PASS) |                              \
     (1 << GLOBAL_SWITCH_TEMPLATE) | (1 << GLOBAL_SWITCH_VALIDATE) | (1 << GLOBAL_SWITCH_HELP) |                       \
     (1 << GLOBAL_SWITCH_HELPLEVEL))
#define HAS_GLOBAL_SWITCH(ctx, globalSwitchId) (((ctx)->globalSwitches & (1 << (globalSwitchId))) != 0)
#define GLOBAL_SWITCH_ARG(ctx, globalSwitchId) ((ctx)->values[globalSwitchId])

//** element switches declarations and definitions
#define ELEMENT_SWITCH_COUNT 28
static const char *elementSwitches[] = {
    "-alias",    "-argument",   "-boolean", "-catchall", "-default",    "-enum",     "-forbid",
    "-ignore",   "-imply",      "-keep",    "-key",      "-level",      "-optional", "-parameter",
    "-pass",     "-reciprocal", "-require", "-required", "-standalone", "-switch",   "-upvar",
    "-validate", "-value",      "-type",    "-allow",    "-help",       "-errormsg", "-hsuppress", NULL};

#define INIT_LIST(name, strings, count)                                                                                \
    do {                                                                                                               \
        (name) = Tcl_NewListObj(0, NULL);                                                                              \
        for (int i = 0; i < (count); ++i) {                                                                            \
            Tcl_ListObjAppendElement(NULL, (name), Tcl_NewStringObj((strings)[i], -1));                                \
        }                                                                                                              \
        Tcl_IncrRefCount((name));                                                                                      \
    } while (0)
#define FREE_LIST(name)                                                                                                \
    do {                                                                                                               \
        Tcl_DecrRefCount((name));                                                                                      \
        (name) = NULL;                                                                                                 \
    } while (0)
#define ELEMENT_SWITCH_COUNT_WARGS 14
static const char *elementSwitchesWithArgsNames[] = {
    "alias",   "default",  "enum",  "forbid", "imply", "key",  "pass",
    "require", "validate", "value", "type",   "allow", "help", "errormsg", NULL};
#define ELEMENT_SWITCH_COUNT_TYPES 19
static const char *allowedTypes[] = {
    "alnum", "alpha", "ascii", "boolean", "control", "dict",  "digit",       "double",   "graph", "integer",
    "list",  "lower", "print", "punct",   "space",   "upper", "wideinteger", "wordchar", "xdigit", NULL};
#define TEMPLATE_SUBST_COUNT 5
static const char *templateSubstNames[TEMPLATE_SUBST_COUNT] = {"\\\\\\\\", "\\\\", "\\\\%", "%", "%"};
#define HELP_GEN_SUBST_COUNT 4
static const char *helpGenSubstNames[HELP_GEN_SUBST_COUNT] = {",;", ";", ",.", "."};

// Initialization macro
#define ELSWITCH(name, string)                                                                                         \
    do {                                                                                                               \
        (name) = Tcl_NewStringObj(#string, -1);                                                                        \
        Tcl_IncrRefCount((name));                                                                                      \
    } while (0)

// Free macro
#define ELSWITCH_DECREF(name)                                                                                          \
    do {                                                                                                               \
        Tcl_DecrRefCount((name));                                                                                      \
        (name) = NULL;                                                                                                 \
    } while (0)

//** static string arrays
#define ELEMENT_SWITCH_COUNT_IMPLY_ARG 5
static const char *elementSwitchesImplyElementArg[] = {"optional", "required", "catchall", "upvar", "type"};
#define ELEMENT_SWITCH_COUNT_CONFLICT 9
static const char *conflictSwitches[] = {"parameter", "ignore", "required", "argument", "upvar",
                                         "boolean",   "enum",   "type",     "allow"};
static const char *cSwRow1[] = {"alias", "boolean", "value", "argument", "imply"};
static const char *cSwRow2[] = {"key", "pass"};
static const char *cSwRow3[] = {"boolean", "default"};
static const char *cSwRow4[] = {"boolean", "value"};
static const char *cSwRow5[] = {"boolean", "inline", "catchall"};
static const char *cSwRow6[] = {"default", "value"};
static const char *cSwRow7[] = {"validate"};
static const char *cSwRow8[] = {"upvar", "boolean", "enum"};
static const char *cSwRow9[] = {"forbid"};
static const char **conflictSwitchesRows[] = {cSwRow1, cSwRow2, cSwRow3, cSwRow4, cSwRow5,
                                              cSwRow6, cSwRow7, cSwRow8, cSwRow9};
static const int cSwRowSizes[] = {5, 2, 2, 2, 3, 2, 1, 3, 1};
#define ELEMENT_SWITCH_COUNT_DISALLOW 6
static const char *disSwRow1[] = {"switch", "optional", "catchall"};
static const char *disSwRow2[] = {"switch", "optional", "upvar"};
static const char *disSwRow3[] = {"switch", "optional", "default"};
static const char *disSwRow4[] = {"switch", "optional", "boolean"};
static const char *disSwRow5[] = {"switch", "optional", "type"};
static const char *disSwRow6[] = {"parameter", "optional", "required"};
static const char **disallowedSwitchesRows[] = {disSwRow1, disSwRow2, disSwRow3, disSwRow4, disSwRow5, disSwRow6};
#define ELEMENT_SWITCH_COUNT_REQPAIRS 3
static const char *requireSwitchesPair0[ELEMENT_SWITCH_COUNT_REQPAIRS] = {"reciprocal", "level", "errormsg"};
static const char *requireSwitchesPair1[ELEMENT_SWITCH_COUNT_REQPAIRS] = {"require", "upvar", "validate"};
#define ELEMENT_SWITCH_COUNT_CONSTRAINTS 3
static const char *elemSwConstraints[ELEMENT_SWITCH_COUNT_CONSTRAINTS] = {"require", "forbid", "allow"};

//** functions declaration

extern DLLEXPORT int Argparse_c_Init(Tcl_Interp *interp);
static int ArgparseCmdProc2(void *clientData, Tcl_Interp *interp, Tcl_Size objc, Tcl_Obj *const objv[]);
void FreeGlobalSwitches(GlobalSwitchesContext *ctx);
int EvalPrefixMatch(Tcl_Interp *interp, Tcl_Obj *tableList, Tcl_Obj *matchObj, int useExact, int useMessage,
                    Tcl_Obj *messageObj, int wantErrorMessage, Tcl_Obj **resultObjPtr);
int EvalRegsubFirstMatch(Tcl_Interp *interp, Tcl_RegExp regexp, Tcl_Obj *inputObj, Tcl_Obj *replacementObj,
                         Tcl_Obj **resultObjPtr);
int InList(Tcl_Interp *interp, Tcl_Obj *itemObj, Tcl_Obj *listObj);
Tcl_Obj *ListRange(Tcl_Interp *interp, Tcl_Obj *listObj, Tcl_Size start, Tcl_Size end);
int GetArgsFromCaller(Tcl_Interp *interp, int *argcPtr, Tcl_Obj ***argvPtr);
int ParseElementDefinitions(Tcl_Interp *interp, GlobalSwitchesContext *ctx, Tcl_Obj *definition,
                            ArgumentDefinition *argCtx, ArgparseInterpCtx *interpCtx);
static inline int DictKeyExists(Tcl_Interp *interp, Tcl_Obj *dictObj, Tcl_Obj *keyStr);
int NestedDictKeyExists(Tcl_Interp *interp, Tcl_Obj *dictObj, Tcl_Obj *outerKey, Tcl_Obj *innerKey);
Tcl_Obj *SplitString(Tcl_Interp *interp, Tcl_Obj *stringObj, Tcl_Obj *sepCharsObj, ArgparseInterpCtx *interpCtx);
int EvalStringMap(Tcl_Interp *interp, Tcl_Obj *mapListObj, Tcl_Obj *inputObj, Tcl_Obj **resultObjPtr);
Tcl_Obj *BuildAliasJoinString(Tcl_Interp *interp, Tcl_Obj *optDict, Tcl_Obj *name, ArgparseInterpCtx *interpCtx);
int CheckAliasesAreUnique(Tcl_Interp *interp, Tcl_Obj *aliasesDict, Tcl_Obj *optDict, ArgparseInterpCtx *interpCtx);
Tcl_Obj *BuildAllowedTypesSummary(Tcl_Interp *interp, Tcl_Obj *allowedTypes);
int DictKeys(Tcl_Interp *interp, Tcl_Obj *dictObj, Tcl_Size *keyCountPtr, Tcl_Obj ***keyObjsPtr);
Tcl_Obj *MergeTwoLists(Tcl_Interp *interp, Tcl_Obj *list1, Tcl_Obj *list2);
int EvalLsort(Tcl_Interp *interp, Tcl_Obj *listObj, Tcl_Obj **resultObjPtr);
int GetNestedDictValue(Tcl_Interp *interp, Tcl_Obj *dictObj, Tcl_Obj *outerKey, Tcl_Obj *innerKey, Tcl_Obj **valuePtr);
int ValidateHelper(Tcl_Interp *interp, GlobalSwitchesContext *ctx, Tcl_Obj *nameObj, Tcl_Obj *optDictObj,
                   Tcl_Obj *argObj, ArgparseInterpCtx *interpCtx, int listFlag, Tcl_Obj **resultPtr);
int TypeChecker(Tcl_Interp *interp, Tcl_Obj *nameObj, Tcl_Obj *optDictObj, Tcl_Obj *argObj,
                ArgparseInterpCtx *interpCtx, int listFlag, Tcl_Obj **resultPtr);
int DictLappend(Tcl_Interp *interp, Tcl_Obj *dictObjPtr, Tcl_Obj *keyObj, Tcl_Obj *valuesList);
int DictLappendElem(Tcl_Interp *interp, Tcl_Obj *dictObjPtr, Tcl_Obj *keyObj, Tcl_Obj *valueObj);
Tcl_Obj *BuildMissingSwitchesError(Tcl_Interp *interp, Tcl_Obj *missingList);
Tcl_Obj *BuildMissingParameterError(Tcl_Interp *interp, Tcl_Obj *missingList);
int DictIncr(Tcl_Interp *interp, Tcl_Obj *dictObjPtr, Tcl_Obj *keyObj, Tcl_Obj *countObj);
Tcl_Obj *BuildBadSwitchError(Tcl_Interp *interp, Tcl_Obj *argObj, Tcl_Obj *switchesList);
int EvalMatchRegexpGroups(Tcl_Interp *interp, Tcl_RegExp regexp, Tcl_Obj *textObj, ArgparseInterpCtx *interpCtx,
                          Tcl_Obj **resultListPtr);
static Tcl_Obj *GenerateGlobalSwitchesKey(const GlobalSwitchesContext *ctx);
ArgumentDefinition *CreateAndCacheArgDef(Tcl_Interp *interp, ArgparseInterpCtx *, Tcl_Obj *definition,
                                         GlobalSwitchesContext *ctx, const char *key);
static void CleanupAllArgumentDefinitions(ArgparseInterpCtx *argparseInterpCtx);
ArgumentDefinition *DeepCopyArgumentDefinition(Tcl_Interp *interp, const ArgumentDefinition *src);
Tcl_Obj *DuplicateDictWithNestedDicts(Tcl_Interp *interp, Tcl_Obj *dictObj);
void FreeArgumentDefinition(ArgumentDefinition *argDef);
Tcl_Obj *JoinListWithSeparator(Tcl_Interp *interp, Tcl_Obj *listObj, const char *separator);
Tcl_Obj *JoinWithEmptySeparator(Tcl_Interp *interp, Tcl_Obj *listObj);
Tcl_Obj *EvaluateAdjust(Tcl_Interp *interp, Tcl_Obj *helpObj, int len);
Tcl_Obj *EvaluateIndent(Tcl_Interp *interp, Tcl_Obj *stringObj, Tcl_Obj *spacesObj, Tcl_Obj *skipObj);
Tcl_Obj *EvaluateStringToTitle(Tcl_Interp *interp, Tcl_Obj *stringObj, Tcl_Obj *firstIndexObj, Tcl_Obj *lastIndexObj);
Tcl_Obj *BuildHelpMessage(Tcl_Interp *interp, GlobalSwitchesContext *ctx, ArgumentDefinition *argDefCtx,
                          Tcl_Obj *helpLevel, ArgparseInterpCtx *interpCtx);
int PrefixMatch(Tcl_Interp *interp, const char **tableList, Tcl_Obj *matchObj, int useExact, int useMessage,
                    char *messageObj, int wantErrorMessage, Tcl_Obj **resultObjPtr);
