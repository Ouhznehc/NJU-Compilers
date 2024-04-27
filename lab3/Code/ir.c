#include "ir.h"

/* 
Program:    
    | ExtDefList
*/
void translate_Program(syntax_t* node) { 
    return;
}

/*
ExtDefList:         
    | ExtDef ExtDefList                       
    | empty
*/ 
void translate_ExtDefList(syntax_t* node) {
    return;
}

/*
ExtDef:
    | Specifier ExtDecList SEMI
    | Specifier SEMI
    | Specifier FunDec CompSt
*/
void translate_ExtDef(syntax_t* node) {  
    return;
} 

/*
ExtDecList:
    | VarDec
    | VarDec COMMA ExtDecList
*/
void translate_ExtDecList(syntax_t* node) { 
    return;
} 

/*
Specifier:
    | TYPE
    | StructSpecifier
*/
void translate_Specifier(syntax_t* node) {  
    return;
} 

/*
StructSpecifier:
    | STRUCT OptTag LC DefList RC
    | STRUCT Tag
*/
void translate_StructSpecifier(syntax_t* node) { 
    return;
}

/*
OptTag:
    | ID
    | empty
*/
void translate_OptTag(syntax_t* node) {  
    return;
} 

/*
Tag:
    | ID
*/
void translate_Tag(syntax_t* node) { 
    return;
} 

/*
VarDec:
    | ID
    | VarDec LB INT RB
*/
void translate_VarDec(syntax_t* node) {  
    return;
} 

/*
FunDec:
    | ID LP VarList RP
    | ID LP RP
*/
void translate_FunDec(syntax_t* node) { 
    return;
} 

/*
VarList:
    | ParamDec COMMA VarList
    | ParamDec
*/
void translate_VarList(syntax_t* node) { 
    return;
} 

/*
ParamDec:
    | Specifier VarDec
*/
void translate_ParamDec(syntax_t* node) { 
    return;
} 

/* 
CompSt:
    | LC DefList StmtList RC
*/
void translate_CompSt(syntax_t* node) { 
    return;
}

/*
StmtList:
    | Stmt StmtList
    | empty
*/
void translate_StmtList(syntax_t* node) { 
    return;
} 

/*
Stmt:
    | Exp SEMI
    | CompSt
    | RETURN Exp SEMI
    | IF LP Exp RP Stmt ELSE Stmt
    | IF LP Exp RP Stmt
    | WHILE LP Exp RP Stmt
*/
void translate_Stmt(syntax_t* node) { 
    return;
}

/*
DefList:
    | Def DefList
    | empty
*/
void translate_DefList(syntax_t* node) { 
    return;
} 

/*
Def:
    | Specifier DecList SEMI
*/
void translate_Def(syntax_t* node) { 
    return;
} 

/*
DecList:
    | Dec
    | Dec COMMA DecList
*/
void translate_DecList(syntax_t* node) {  
    return;
} 

/*
Dec:
    | VarDec
    | VarDec ASSIGNOP Exp
*/
void translate_Dec(syntax_t* node) { 
 return;
} 



/*
Exp:
    | Exp ASSIGNOP Exp
    | Exp AND Exp
    | Exp OR Exp
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
void translate_Exp(syntax_t* node) { 
    return;
} 

/*
Args:
    | Exp COMMA Args
    | Exp
*/
void translate_Args(syntax_t* node) { 
    return;
} 

void display_ir_code(FILE* ir) {
    return;
}

void generate_ir(syntax_t* root, FILE* ir) {
    translate_Program(root);
    display_ir_code(ir);
    return;
}