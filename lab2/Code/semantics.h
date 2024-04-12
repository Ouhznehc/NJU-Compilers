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




void semantic_error(SemanticErrorType error, int lineno, char* msg);
type_t* new_type(SemanticBasicType kind, ...);

/* High-level Definitions */
void Program(syntax_t* node); 
void ExtDefList(syntax_t* node); 
void ExtDef(syntax_t* node); 
void ExtDecList(syntax_t* node, type_t* specifier); 

/* Specifiers */
type_t* Specifier(syntax_t* node); 
type_t* StructSpecifier(syntax_t* node); 
type_t* OptTag(syntax_t* node); 
type_t* Tag(syntax_t* node); 

/* Declarators */
item_t* VarDec(syntax_t* node, type_t* specifier); 
void FunDec(syntax_t* node, type_t* specifier, int type); 
void VarList(syntax_t* node, item_t* func); 
item_t* ParamDec(syntax_t* node); 

/* Statements */
void CompSt(syntax_t* node, type_t* specifier); 
void StmtList(syntax_t* node, type_t* specifier); 
void Stmt(syntax_t* node, type_t* specifier);

/* Local Definitions */
void DefList(syntax_t* node, type_t* record); 
void Def(syntax_t* node, type_t* record); 
void Dec(syntax_t* node, type_t* specifier, type_t* record); 
void DecList(syntax_t* node, type_t* specifier, type_t* record); 

/* Expressions */
type_t* Exp(syntax_t* node); 
void Args(syntax_t* node, item_t* func); 

void check_func_definition();

#endif