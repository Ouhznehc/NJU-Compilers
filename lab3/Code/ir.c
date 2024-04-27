#include "ir.h"

/* High-level Definitions */
void translate_Program(syntax_t* node); 
void translate_ExtDefList(syntax_t* node); 
void translate_ExtDef(syntax_t* node); 
void translate_ExtDecList(syntax_t* node); 

/* Specifiers */
void translate_Specifier(syntax_t* node); 
void translate_StructSpecifier(syntax_t* node); 
void translate_OptTag(syntax_t* node); 
void translate_Tag(syntax_t* node); 

/* Declarators */
void translate_VarDec(syntax_t* node); 
void translate_FunDec(syntax_t* node); 
void translate_VarList(syntax_t* node); 
void translate_ParamDec(syntax_t* node); 

/* Statements */
void translate_CompSt(syntax_t* node); 
void translate_StmtList(syntax_t* node); 
void translate_Stmt(syntax_t* node);

/* Local Definitions */
void translate_DefList(syntax_t* node); 
void translate_Def(syntax_t* node); 
void translate_Dec(syntax_t* node); 
void translate_DecList(syntax_t* node); 

/* Expressions */
void translate_Exp(syntax_t* node); 
void translate_Args(syntax_t* node); 

void* malloc_safe(int size) {
    void* ret = malloc(size);
    memset(ret, 0, size);
    return ret;
}

arg_t* new_arg(int kind, char* name, int cons) {
    arg_t* ret = malloc_safe(sizeof(arg_t));
    ret->kind = kind;
    switch (kind) {
        case ArgVar:
        case ArgTmp:
        case ArgImm:
        case ArgLabel:
        case ArgSize:
            ret->cons = cons;
        case ArgFunc:
        case ArgRelop:
        case ArgRef:
        case ArgDeref:
            strcpy(ret->name, name);
        default: assert(0);
    }
    return ret;
}



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

char* display_arg(arg_t* arg) {
    if (arg == NULL) return NULL;
    char* ret = malloc_safe(sizeof(char) * 64);
    switch (arg->kind) {
        case ArgVar:        sprintf(ret, "v%d", arg->cons); break;
        case ArgTmp:        sprintf(ret, "t%d", arg->cons); break;
        case ArgLabel:      sprintf(ret, "label%d", arg->cons); break;
        case ArgFunc:       sprintf(ret, "%s", arg->name); break;
        case ArgRelop:      sprintf(ret, "%s", arg->name); break;
        case ArgSize:       sprintf(ret, "%d", arg->cons); break;
        case ArgRef:        sprintf(ret, "&%s", arg->name); break;
        case ArgDeref:      sprintf(ret, "*%s", arg->name); break;
        default: assert(0);
    }
    return ret;
}
char* display_ic(ic_t* ic) {
    char* ret = malloc_safe(sizeof(char) * 64);
    char* result = display_arg(ic->result);
    char* arg1 = display_arg(ic->arg1);
    char* arg2 = display_arg(ic->arg2);
    char* relop = display_arg(ic->relop);
    switch (ic->op) {
        case IcLabel:       sprintf(ret, "LABEL %s :\n", result);
        case IcFunc:        sprintf(ret, "FUNCTION %s :\n", result);
        case IcAssign:      sprintf(ret, "%s := %s\n", result, arg1);
        case IcAdd:         sprintf(ret, "%s := %s + %s\n", result, arg1, arg2);
        case IcSub:         sprintf(ret, "%s := %s - %s\n", result, arg1, arg2);
        case IcMul:         sprintf(ret, "%s := %s * %s\n", result, arg1, arg2);
        case IcDiv:         sprintf(ret, "%s := %s / %s\n", result, arg1, arg2);
        case IcGoto:        sprintf(ret, "GOTO %s\n", result);
        case IcBranch:      sprintf(ret, "IF %s %s %s GOTO %s\n", arg1, relop, arg2, result);
        case IcReturn:      sprintf(ret, "RETURN %s\n", result);
        case IcArg:         sprintf(ret, "ARG %s\n", result);
        case IcCall:        sprintf(ret, "%s := CALL %s\n", result, arg1);
        case IcParam:       sprintf(ret, "PARAM %s\n", result);
        case IcRead:        sprintf(ret, "READ %s\n", result);
        case IcWrite:       sprintf(ret, "WRITE %s\n", result);
        default: assert(0);
    }
    return ret;
}

void display_ir(FILE* ir) {
    ir_t* cur = ir_head;
    while (cur != NULL) {
        fprintf(ir, "%s", display_ic(cur->code));
        cur = cur->next;
    }
}


void generate_ir(syntax_t* root, FILE* ir) {
    translate_Program(root);
    display_ir(ir);
    return;
}