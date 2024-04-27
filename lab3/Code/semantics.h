#ifndef __SEMANTICS_H__
#define __SEMANTICS_H__

#include "grammar.h"
#include <stdbool.h>

typedef enum {
    UNDEFINED_VAR = 1,              // Undefined Variable
    UNDEFINED_FUNC,             // Undefined Function
    DUPLICATE_VAR,              // Duplicate Variable
    DUPLICATE_FUNC,             // Duplicate Function
    MISMATCHED_ASSIGN,          // Type mismatched for assignment.
    RVALUE_ASSIGN,              // The left-hand side of an assignment must be a variable.
    MISMATCHED_OP,              // Type mismatched for operands.
    MISMATCHED_RETURN,          // Type mismatched for return.
    MISMATCHED_FUNC_ARG,        // Function is not applicable for arguments
    NOT_A_ARRAY,                // Variable is not a Array
    NOT_A_FUNC,                 // Variable is not a Function
    NOT_A_INDEX,                // Array index is not Int
    NOT_A_STRUCT,               // Illegal use of "."(Variable is not a Struct)
    UNDEFINED_FIELD,            // Struct field undefined
    DUPLICATE_FIELD,            // Struct field duplicate
    INITIALIZE_FIELD,           // Initialize Struct field
    DUPLICATED_STRUCT,          // Duplicated Struct name
    UNDEFINED_STRUCT,           // Undefined Struct name
    DECLARED_NOT_DEFINED_FUNC,  // Function declared but not defined
    CONFLICT_FUNC,           // Conflict between Function declaration and definition
} SemanticErrorType;

typedef enum _SemanticVarType {
    Basic,
    Array,
    Struct,
    FuncDec,
    FuncDef
} SemanticVarType;

typedef enum _SemanticBasicType {
    Int, 
    Float,
} SemanticBasicType;

struct array_t {
    struct type_t* elem;
    int width;
};
typedef struct array_t array_t;

struct basic_t {
    SemanticBasicType type;
};
typedef struct basic_t basic_t;

struct record_t {
    char name[64];
    struct field_t* field;
};
typedef struct record_t record_t;

struct func_t {
    char name[64];
    int lineno;
    int argc;
    struct field_t* argv;
    struct type_t* ret;
};
typedef struct func_t func_t; 


//only record_t need a name such as: 'example' in ```struct example {int a;}```.
struct type_t {
    SemanticVarType kind;
    union {
        struct basic_t basic;
        struct array_t array;
        struct record_t record;
        struct func_t function;
    };
};
typedef struct type_t type_t;


struct field_t {
    char name[64];
    struct type_t* type;
    struct field_t* next; 
};
typedef struct field_t field_t;
typedef field_t item_t;

type_t* new_type(SemanticBasicType kind, ...);
void semantic_check(syntax_t* root);


#endif