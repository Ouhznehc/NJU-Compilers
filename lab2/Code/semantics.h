#ifndef __SEMANTICS_H__
#define __SEMANTICS_H__

#include "grammar.h"


typedef enum {
    UNDEFINED_VAR,              // Undefined Variable
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

void semantic_error(SemanticErrorType error, int lineno, char* msg);

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