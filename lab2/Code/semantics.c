#include "semantics.h"

void semantic_error(SemanticErrorType error, int lineno, char* msg) {
    switch (error) {
        case UNDEFINED_VAR:
            printf("Error type 1 at Line %d: Undefined variable '%s'.\n", lineno, msg);
            break;
        case UNDEFINED_FUNC:
            printf("Error type 2 at Line %d: Undefined function '%s'.\n", lineno, msg);
            break;
        case DUPLICATE_VAR:
            printf("Error type 3 at Line %d: Duplicate variable '%s'.\n", lineno, msg);
            break;
        case DUPLICATE_FUNC:
            printf("Error type 4 at Line %d: Duplicate function '%s'.\n", lineno, msg);
            break;
        case MISMATCHED_ASSIGN:
            printf("Error type 5 at Line %d: Type mismatch in assignment of '%s'.\n", lineno, msg);
            break;
        case RVALUE_ASSIGN:
            printf("Error type 6 at Line %d: Assignment to non-variable '%s'.\n", lineno, msg);
            break;
        case MISMATCHED_OP:
            printf("Error type 7 at Line %d: Operator applied to incompatible types '%s'.\n", lineno, msg);
            break;
        case MISMATCHED_RETURN:
            printf("Error type 8 at Line %d: Return type mismatch '%s'.\n", lineno, msg);
            break;
        case MISMATCHED_FUNC_ARG:
            printf("Error type 8 at Line %d: Function '%s' called with incorrect arguments.\n", lineno, msg);
            break;
        case NOT_A_ARRAY:
            printf("Error type 10 at Line %d: Variable '%s' is not an array.\n", lineno, msg);
            break;
        case NOT_A_FUNC:
            printf("Error type 11 at Line %d: Variable '%s' is not a function.\n", lineno, msg);
            break;
        case NOT_A_INDEX:
            printf("Error type 12 at Line %d: Array index in '%s' is not Int.\n", lineno, msg);
            break;
        case NOT_A_STRUCT:
            printf("Error type 13 at Line %d: Variable '%s' is not a struct.\n", lineno, msg);
            break;
        case UNDEFINED_FIELD:
            printf("Error type 14 at Line %d: Undefined field '%s' in Struct.\n", lineno, msg);
            break;
        case DUPLICATE_FIELD:
            printf("Error type 15 at Line %d: Duplicate field '%s' in Struct.\n", lineno, msg);
            break;
        case INITIALIZE_FIELD:
            printf("Error type 15 at Line %d: Field '%s' cannot be initialized directly.\n", lineno, msg);
            break;
        case DUPLICATED_STRUCT:
            printf("Error type 16 at Line %d: Struct name '%s' is duplicated.\n", lineno, msg);
            break;
        case UNDEFINED_STRUCT:
            printf("Error type 17 at Line %d: Undefined struct '%s'.\n", lineno, msg);
            break;
        case DECLARED_NOT_DEFINED_FUNC:
            printf("Error type 18 at Line %d: Function '%s' declared but not defined.\n", lineno, msg);
            break;
        case CONFLICT_FUNC:
            printf("Error type 19 at Line %d: Conflict between function declaration and definition '%s'.\n", lineno, msg);
            break;
        default:
            printf("Error at Line %d: Unknown semantic error.\n", lineno);
    }
}

bool symcmp(syntax_t* node, char* name) {
    return strcmp(node->symbol.name, name) == 0;
}


/* 
Program:    
    | ExtDefList
*/
void Program(syntax_t* node) { 
    syntax_t** childs = node->symbol.child;

    // Program -> ExtDefList
    ExtDefList(childs[0]);
 }

/*
ExtDefList:         
    | ExtDef ExtDefList                       
    | empty
*/ 
void ExtDefList(syntax_t* node) {
    // empty
    if (node == NULL) return;

    // ExtDefList ->  ExtDef ExtDefList
    syntax_t** childs = node->symbol.child;
    ExtDef(node->symbol.child[0]);
    ExtDefList(node->symbol.child[1]);
}

/*
ExtDef:
    | Specifier ExtDecList SEMI
    | Specifier SEMI
    | Specifier FunDec CompSt
*/
void ExtDef(syntax_t* node) { 
    assert(node != NULL);
    syntax_t** childs = node->symbol.child;

    type_t* specifier = Specifier(childs[0]);

    // ExtDef -> Specifier ExtDecList SEMI
    if (symcmp(childs[1], "ExtDecList")) {
        ExtDecList(childs[1]);
    }
    // ExtDef -> Specifier FunDec CompSt
    else if (symcmp(childs[1], "FunDec")) {
        FunDec(childs[1]);
        CompSt(childs[2]);
    }
    // ExtDef -> Specifier SEMI
    else {
        /* do nothing */
    }
} 

/*
ExtDecList:
    | VarDec
    | VarDec COMMA ExtDecList
*/
void ExtDecList(syntax_t* node) {
    assert(node != NULL);

} 

/*
Specifier:
    | TYPE
    | StructSpecifier
*/
type_t* Specifier(syntax_t* node) { return NULL; } 

/*
StructSpecifier:
    | STRUCT OptTag LC DefList RC
    | STRUCT Tag
*/
void StructSpecifier(syntax_t* node) { return; }

/*
OptTag:
    | ID
    | empty
*/
void OptTag(syntax_t* node) { return; } 

/*
Tag:
    | ID
*/
void Tag(syntax_t* node) { return; } 

/*
VarDec:
    | ID
    | VarDec LB INT RB
*/
void VarDec(syntax_t* node) { return; } 

/*
FunDec:
    | ID LP VarList RP
    | ID LP RP
*/
void FunDec(syntax_t* node) { return; } 

/*
VarList:
    | ParamDec COMMA VarList
    | ParamDec
*/
void VarList(syntax_t* node) { return; } 

/*
ParamDec:
    | Specifier VarDec
*/
void ParamDec(syntax_t* node) { return; } 

/* 
CompSt:
    | LC DefList StmtList RC
*/
void CompSt(syntax_t* node) { return; }

/*
StmtList:
    | Stmt StmtList
    | empty
*/
void StmtList(syntax_t* node) { return; } 

/*
Stmt:
    | Exp SEMI
    | CompSt
    | RETURN Exp SEMI
    | IF LP Exp RP Stmt ELSE Stmt
    | WHILE LP Exp RP Stmt
*/
void Stmt(syntax_t* node) { return; }

/*
DefList:
    | Def DefList
    | empty
*/
void DefList(syntax_t* node) { return; } 

/*
Def:
    | Specifier DecList SEMI
*/
void Def(syntax_t* node) { return; } 
/*
Dec:
    | VarDec
    | VarDec ASSIGNOP Exp
*/
void Dec(syntax_t* node) { return; } 

/*
DecList:
    | Dec
    | Dec COMMA DecList
*/
void DecList(syntax_t* node) { return; } 

/*
Exp:
    | Exp ASSIGNOP Exp
    | Exp AND Exp
    | Exp RELOP Exp
    | Exp PLUS Exp 
    | Exp MINUS Exp
    | Exp STAR Exp
    | Exp DIV Exp
    | LP Exp RP
    | MINUS Exp
    | NOT Exp
    | ID LP Args RP
    | ID LP RP
    | Exp LB Exp RB
    | Exp DOT ID
    | ID
    | INT
    | FLOAT
*/
void Exp(syntax_t* node) { return; } 

/*
Args:
    | Exp COMMA Args
    | Exp
*/
void Args(syntax_t* node) { return; } 