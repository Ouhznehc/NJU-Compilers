#ifndef __SEMANTICS_H__
#define __SEMANTICS_H__

#include "grammar.h"


typedef enum {
    UNDEFINED_VAR               = 1,    // Undefined Variable
    UNDEFINED_FUNC              = 2,    // Undefined Function
    DUPLICATE_VAR               = 3,    // Duplicate Variable
    DUPLICATE_FUNC              = 4,    // Duplicate Function
    MISMATCHED_ASSIGN           = 5,    // Type mismatched for assignment.
    RVALUE_ASSIGN               = 6,    // The left-hand side of an assignment must be a variable.
    MISMATCHED_OP               = 7,    // Type mismatched for operands.
    MISMATCHED_RETURN           = 8,    // Type mismatched for return.
    MISMATCHED_FUNC_ARG         = 9,    // Function is not applicable for arguments
    NOT_A_ARRAY                 = 10,   // Variable is not a Array
    NOT_A_FUNC                  = 11,   // Variable is not a Function
    NOT_A_INDEX                 = 12,   // Array index is not a Integer
    NOT_A_STRUCT                = 13,   // Illegal use of "."(Variable is not a Struct)
    UNDEFINED_FIELD             = 14,   // Struct field undefined
    DUPLICATE_FIELD             = 15,   // Duplicate field
    INITIALIZE_FIELD            = 15,   // Initialize field
    DUPLICATED_STRUCT           = 16,   // Duplicated Struct name
    UNDEFINED_STRUCT            = 17,   // Undefined Struct
    DECLARED_NOT_DEFINED_FUNC   = 18,   // Function declared but not defined
    CONFLICTING_FUNC            = 19,   // Conflicting between Function declaration and definition
    
} SemanticErrorType;


/* High-level Definitions */
void Program(syntax_t* node); 
void ExtDefList(syntax_t* node); 
void ExtDef(syntax_t* node); 
void ExtDecList(syntax_t* node); 

/* Specifiers */
void Specifier(syntax_t* node); 
void StructSpecifier(syntax_t* node); 
void OptTag(syntax_t* node); 
void Tag(syntax_t* node); 

/* Declarators */
void VarDec(syntax_t* node); 
void FunDec(syntax_t* node); 
void VarList(syntax_t* node); 
void ParamDec(syntax_t* node); 

/* Statements */
void CompSt(syntax_t* node); 
void StmtList(syntax_t* node); 
void Stmt(syntax_t* node);

/* Local Definitions */
void DefList(syntax_t* node); 
void Def(syntax_t* node); 
void Dec(syntax_t* node); 
void DecList(syntax_t* node); 

/* Expressions */
void Exp(syntax_t* node); 
void Args(syntax_t* node); 

#endif