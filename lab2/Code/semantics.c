#include "semantics.h"



/* 
Program:    
    | ExtDefList
*/
void Program(syntax_t* node) { return; }

/*
ExtDefList:         
    | ExtDef ExtDefList                       
    | epsilon
*/ 
void ExtDefList(syntax_t* node) { return; }

/*
ExtDef:
    | Specifier ExtDecList SEMI
    | Specifier SEMI
    | Specifier FunDec CompSt
*/
void ExtDef(syntax_t* node) { return; } 

/*
ExtDecList:
    | VarDec
    | VarDec COMMA ExtDecList
*/
void ExtDecList(syntax_t* node) { return; } 

/*
Specifier:
    | TYPE
    | StructSpecifier
*/
void Specifier(syntax_t* node) { return; } 

/*
StructSpecifier:
    | STRUCT OptTag LC DefList RC
    | STRUCT Tag
*/
void StructSpecifier(syntax_t* node) { return; }

/*
OptTag:
    | ID
    | epsilon
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
    | epsilon
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
    | epsilon
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