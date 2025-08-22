#include <stdint.h>
#include <tcl.h>

typedef uint32_t bitmask_t;
#define BIT(n) ((bitmask_t)1u << (unsigned)(n))
#define BITMASK_OF(id) ((bitmask_t)1u << (unsigned)(id))

//** useful macroses
#define DICT_GET_IF_EXISTS(interp, dictObj, keyObj, valPtr)                                                            \
    ((Tcl_DictObjGet((interp), (dictObj), (keyObj), (valPtr)) == TCL_OK) && (*(valPtr) != NULL))

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

#define PRINT_REF_COUNT(obj_)                                                                                          \
    do {                                                                                                               \
        Tcl_Obj *const _obj = (obj_);                                                                                  \
        if (_obj != NULL) {                                                                                            \
            printf("Reference count of Tcl_Obj \"%s\": %d\n", Tcl_GetString(_obj), _obj->refCount);                    \
        } else {                                                                                                       \
            printf("Tcl_Obj is NULL\n");                                                                               \
        }                                                                                                              \
        fflush(stdout);                                                                                                \
    } while (0)

#define TCL_RETURN_IF_ERROR(expr)                                                                                      \
    do {                                                                                                               \
        if ((expr) != TCL_OK)                                                                                          \
            return TCL_ERROR;                                                                                          \
    } while (0)

#define TCL_RETURN_NULL_IF_ERROR(expr)                                                                             \
    do {                                                                                                               \
        if ((expr) != TCL_OK)                                                                                          \
            return NULL;                                                                                              \
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
    Tcl_Obj *list_allowedTypes;
    Tcl_Obj *list_templateSubstNames;
    Tcl_Obj *list_helpGenSubstNames; // cashed list objects
    Tcl_Obj *misc_emptyStrObj;
    Tcl_Obj *misc_presentSwitchObj;
    Tcl_Obj *misc_validateMsgStrObj;
    Tcl_Obj *misc_dashStrObj;
    Tcl_Obj *misc_doubleDashStrObj; // cashed miscellanious objects
    Tcl_Obj *elswitch_alias;
    Tcl_Obj *elswitch_argument;
    Tcl_Obj *elswitch_boolean;
    Tcl_Obj *elswitch_catchall;
    Tcl_Obj *elswitch_default;
    Tcl_Obj *elswitch_enum;
    Tcl_Obj *elswitch_forbid;
    Tcl_Obj *elswitch_ignore;
    Tcl_Obj *elswitch_imply;
    Tcl_Obj *elswitch_keep;
    Tcl_Obj *elswitch_key;
    Tcl_Obj *elswitch_level;
    Tcl_Obj *elswitch_optional;
    Tcl_Obj *elswitch_parameter;
    Tcl_Obj *elswitch_pass;
    Tcl_Obj *elswitch_reciprocal;
    Tcl_Obj *elswitch_require;
    Tcl_Obj *elswitch_required;
    Tcl_Obj *elswitch_standalone;
    Tcl_Obj *elswitch_switch;
    Tcl_Obj *elswitch_upvar;
    Tcl_Obj *elswitch_validate;
    Tcl_Obj *elswitch_value;
    Tcl_Obj *elswitch_type;
    Tcl_Obj *elswitch_allow;
    Tcl_Obj *elswitch_help;
    Tcl_Obj *elswitch_errormsg;
    Tcl_Obj *elswitch_hsuppress; // cashed element switches objects
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
    bitmask_t globalSwitches;             // bitmask of which global switch are present
    Tcl_Obj *values[GLOBAL_SWITCH_COUNT]; // arguments for value-carrying global switches
} GlobalSwitchesContext;
// global switches that require arguments
#define GLOBAL_SWITCH_TAKES_ARG_MASK                                                                                   \
    (BIT(GLOBAL_SWITCH_ENUM) | BIT(GLOBAL_SWITCH_LEVEL) | BIT(GLOBAL_SWITCH_PASS) | BIT(GLOBAL_SWITCH_TEMPLATE) |      \
     BIT(GLOBAL_SWITCH_VALIDATE) | BIT(GLOBAL_SWITCH_HELP) | BIT(GLOBAL_SWITCH_HELPLEVEL))
#define HAS_GLOBAL_SWITCH(pctx, id) ((((bitmask_t)((pctx)->globalSwitches)) & BITMASK_OF(id)) != (bitmask_t)0u)
#define GLOBAL_SWITCH_ARG(ctx, globalSwitchId) ((ctx)->values[globalSwitchId])

//** element switches declarations and definitions
#define INIT_LIST(name, strings, count)                                                                                \
    do {                                                                                                               \
        (name) = Tcl_NewListObj(0, NULL);                                                                              \
        for (int i = 0; i < (count); ++i) {                                                                            \
            TCL_RETURN_NULL_IF_ERROR(Tcl_ListObjAppendElement(NULL, (name), Tcl_NewStringObj((strings)[i], -1)));      \
        }                                                                                                              \
        Tcl_IncrRefCount((name));                                                                                      \
    } while (0)
#define FREE_LIST(name)                                                                                                \
    do {                                                                                                               \
        Tcl_DecrRefCount((name));                                                                                      \
        (name) = NULL;                                                                                                 \
    } while (0)
#define ELEMENT_SWITCH_COUNT_TYPES 19
static const char *allowedTypes[] = {
    "alnum", "alpha", "ascii", "boolean", "control", "dict",  "digit",       "double",   "graph", "integer",
    "list",  "lower", "print", "punct",   "space",   "upper", "wideinteger", "wordchar", "xdigit", NULL};

// Initialization macro
#define ELSWITCH(name, string)                                                                                         \
    do {                                                                                                               \
        (name) = Tcl_NewStringObj((string), -1);                                                                        \
        Tcl_IncrRefCount((name));                                                                                      \
    } while (0)

// Free macro
#define ELSWITCH_DECREF(name)                                                                                          \
    do {                                                                                                               \
        Tcl_DecrRefCount((name));                                                                                      \
        (name) = NULL;                                                                                                 \
    } while (0)


//** functions declaration

void SetGlobalSwitch(GlobalSwitchesContext *ctx, unsigned int globalSwitchId, Tcl_Obj *value);
void InitArgumentDefinition(ArgumentDefinition *ctx);
Tcl_Obj *EnumStrBuildObj(Tcl_Interp *interp, Tcl_Obj *nameObj, Tcl_Obj *optDict);
extern DLLEXPORT int Argparse_Init(Tcl_Interp *interp);
static int ArgparseCmdProc2(void *clientData, Tcl_Interp *interp, Tcl_Size objc, Tcl_Obj *const objv[]);
void FreeGlobalSwitches(GlobalSwitchesContext *ctx);
int EvalRegsubFirstMatch(Tcl_Interp *interp, Tcl_RegExp regexp, Tcl_Obj *inputObj, Tcl_Obj *replacementObj,
                         Tcl_Obj **resultObjPtr);
int InList(Tcl_Interp *interp, Tcl_Obj *itemObj, Tcl_Obj *listObj);
Tcl_Obj *ListRange(Tcl_Interp *interp, Tcl_Obj *listObj, Tcl_Size start, Tcl_Size end);
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
ArgumentDefinition *CreateAndCacheArgDef(Tcl_Interp *interp, ArgparseInterpCtx *interpCtx, Tcl_Obj *definition,
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
                    const char *message, int wantErrorMessage, Tcl_Obj **resultObjPtr);
int InListStringMatch(Tcl_Interp *interp, Tcl_Obj *itemObj, Tcl_Obj *listObj);
