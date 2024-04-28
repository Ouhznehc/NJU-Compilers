#include "ir.h"

/* High-level Definitions */
void translate_Program(syntax_t* node); 
void translate_ExtDefList(syntax_t* node); 
void translate_ExtDef(syntax_t* node); 

/* Declarators */
arg_t* translate_VarDec(syntax_t* node);
void translate_FunDec(syntax_t* node); 

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
arg_t* translate_Exp(syntax_t* node); 
void translate_Args(syntax_t* node); 
void translate_Cond(syntax_t* node, arg_t* true_l, arg_t* false_l);

void* malloc_safe(int size) {
    void* ret = malloc(size);
    memset(ret, 0, size);
    return ret;
}

arg_t* new_arg(int kind, char* name, int cons) {
    arg_t* ret = malloc_safe(sizeof(arg_t));
    ret->kind = kind;
    switch (kind) {
        case ArgTmp: case ArgImm: case ArgLabel: case ArgSize:
            ret->cons = cons;
            break;
        case ArgFunc: case ArgRelop: case ArgVar: case ArgAddr:
            strcpy(ret->name, name);
            break;
        default: assert(0);
    }
    return ret;
}

ic_t* new_ic(int kind, ...) {
    ic_t* ret = malloc_safe(sizeof(ic_t));
    ret->op = kind;
    va_list args;
    va_start(args, kind);
    switch (kind) {
        case IcAssign: case IcCall: case IcMinus:
            ret->result = va_arg(args, arg_t*);
            ret->arg1 = va_arg(args, arg_t*);
            break;
        case IcAdd: case IcSub: case IcMul: case IcDiv: case IcDec:
            ret->result = va_arg(args, arg_t*);
            ret->arg1 = va_arg(args, arg_t*);
            ret->arg2 = va_arg(args, arg_t*);
            break;
        case IcLabel: case IcFunc: case IcGoto: case IcReturn: 
        case IcArg: case IcParam: case IcRead: case IcWrite:
            ret->result = va_arg(args, arg_t*);
            break;
        case IcBranch:
            ret->arg1 = va_arg(args, arg_t*);
            ret->relop = va_arg(args, arg_t*);
            ret->arg2 = va_arg(args, arg_t*);
            ret->result = va_arg(args, arg_t*);
            break;
    }
    return ret;
}

ir_t* new_ir(ic_t* ic) {
    ir_t* ret = malloc_safe(sizeof(ir_t));
    ret->code = ic;
    ret->next = NULL;
    return ret;
}

void insert_ir(ir_t* ir) {
    if (ir_head == NULL) {
        ir_head = ir_tail = ir;
        ir->next = NULL;
        return;
    }
    ir_tail->next = ir;
    ir_tail = ir;
    ir->next = NULL;
    return;
}

int calculate_size(type_t* type) {
    if (type == NULL) return 0;
    int ret = 0;
    switch (type->kind) {
        case Basic: return 4;
        case Array: return type->array.width * calculate_size(type->array.elem);
        case Struct:
            for (field_t* cur = type->record.field; cur; cur = cur->next)
                ret += calculate_size(cur->type);
            return ret;
        default: assert(0);
    }
}

/* 
Program:    
    | ExtDefList
*/
void translate_Program(syntax_t* node) { 
    assert(node != NULL);
    syntax_t** childs = node->symbol.child;
    translate_ExtDefList(childs[0]);
}

/*
ExtDefList:         
    | ExtDef ExtDefList                       
    | empty
*/ 
void translate_ExtDefList(syntax_t* node) {
    // ExtDefList -> empty
    if (node == NULL) return;

    syntax_t** childs = node->symbol.child;
    // ExtDefList -> ExtDef ExtDefList
    translate_ExtDef(childs[0]);
    translate_ExtDefList(childs[1]);
}

/*
ExtDef:
    | Specifier ExtDecList SEMI
    | Specifier SEMI
    | Specifier FunDec CompSt
*/
void translate_ExtDef(syntax_t* node) {  
    assert(node != NULL);
    syntax_t** childs = node->symbol.child;
    switch (node->symbol.rule) {
        // ExtDef -> Specifier ExtDecList SEMI
        case 1: return;
        // ExtDef -> Specifier SEMI 
        case 2: return;
        // ExtDef -> Specifier FunDec CompSt
        case 3:
            translate_FunDec(childs[1]);
            translate_CompSt(childs[2]);
            return;
        default: assert(0);
    }
} 

/*
VarDec:
    | ID
    | VarDec LB INT RB
*/
arg_t* translate_VarDec(syntax_t* node) {
    assert(node != NULL);
    syntax_t** childs = node->symbol.child;
    arg_t* ret = new_arg(ArgVar, childs[0]->name, 0);
    item_t* var = FindScopeItem(childs[0]->name);
    arg_t* size = new_arg(ArgSize, NULL, calculate_size(var->type));
    switch (node->symbol.rule) {
        // VarDec -> ID
        case 1:
            assert(var->type->kind != FuncDec && var->type->kind != FuncDef);
            if(var->type->kind == Basic) return ret;
            else insert_ir(new_ic(IcDec, ret, size));
            return ret;
        // VarDec LB INT RB
        case 2:
            return translate_VarDec(childs[0]);
        default: assert(0);
    }
} 

/*
FunDec:
    | ID LP VarList RP
    | ID LP RP
*/
void translate_FunDec(syntax_t* node) { 
    assert(node != NULL);
    syntax_t** childs = node->symbol.child;
    
    arg_t* func = new_arg(ArgFunc, childs[0]->name, 0);
    insert_ir(new_ic(IcFunc, func));

    type_t* type = FindScopeItem(childs[0]->name)->type;

    assert(type->kind == FuncDef);
    for (field_t* cur = type->function.argv; cur; cur = cur->next) {
        int kind = cur->type->kind != Basic ? ArgAddr : ArgVar;
        arg_t* param = new_arg(kind, cur->name, 0);
        insert_ir(new_ic(IcParam, param));
    }
    return;
} 

/* 
CompSt:
    | LC DefList StmtList RC
*/
void translate_CompSt(syntax_t* node) { 
    assert(node != NULL);
    syntax_t** childs = node->symbol.child;
    translate_DefList(childs[1]);
    translate_StmtList(childs[2]);
    return;
}

/*
StmtList:
    | Stmt StmtList
    | empty
*/
void translate_StmtList(syntax_t* node) { 
    if (node == NULL) return;
    syntax_t** childs = node->symbol.child;
    translate_Stmt(childs[0]);
    translate_StmtList(childs[1]);
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
    assert(node != NULL);
    syntax_t** childs = node->symbol.child;

    int rule = node->symbol.rule;

    // Stmt -> Exp SEMI
    if (rule == 1) {
        translate_Exp(childs[0]);
    }
    // Stmt -> CompSt
    else if (rule == 2) {
        translate_CompSt(childs[0]);
    }
    // Stmt -> RETURN Exp SEMI
    else if (rule == 3) {
        arg_t* exp = translate_Exp(childs[1]);
        insert_ir(new_ic(IcReturn, exp));
    }
    // Stmt -> IF LP Exp RP Stmt ELSE Stmt
    else if (rule == 4) {
        arg_t* label1 = new_arg(ArgLabel, NULL, ++label_no);
        arg_t* label2 = new_arg(ArgLabel, NULL, ++label_no);
        arg_t* label3 = new_arg(ArgLabel, NULL, ++label_no);
        translate_Cond(childs[2], label1, label2);
        insert_ir(new_ic(IcLabel, label1));
        translate_Stmt(childs[4]);
        insert_ir(new_ic(IcGoto, label3));
        insert_ir(new_ic(IcLabel, label3)); 
        translate_Stmt(childs[6]);
        insert_ir(new_ic(IcLabel, label3));       
    }
    // Stmt -> IF LP Exp RP Stmt
    else if (rule == 5) {
        arg_t* label1 = new_arg(ArgLabel, NULL, ++label_no);
        arg_t* label2 = new_arg(ArgLabel, NULL, ++label_no);
        translate_Cond(childs[2], label1, label2);
        insert_ir(new_ic(IcLabel, label1));
        translate_Stmt(childs[4]);
        insert_ir(new_ic(IcLabel, label2));
    }
    // Stmt -> WHILE LP Exp RP Stmt
    else if (rule == 6) {
        arg_t* label1 = new_arg(ArgLabel, NULL, ++label_no);
        arg_t* label2 = new_arg(ArgLabel, NULL, ++label_no);
        arg_t* label3 = new_arg(ArgLabel, NULL, ++label_no);
        insert_ir(new_ic(IcLabel, label1));
        translate_Cond(childs[2], label2, label3);
        insert_ir(new_ic(IcLabel, label2));
        translate_Stmt(childs[4]);
        insert_ir(new_ic(IcGoto, label1));
        insert_ir(new_ic(IcLabel, label3));       
    }
    else assert(0);

    return;
}

/*
DefList:
    | Def DefList
    | empty
*/
void translate_DefList(syntax_t* node) { 
    if (node == NULL) return;
    syntax_t** childs = node->symbol.child;
    translate_Def(childs[0]);
    translate_DefList(childs[1]);
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
arg_t* translate_Exp(syntax_t* node) { 
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

void translate_Cond(syntax_t* node, arg_t* true_l, arg_t* false_l) {
    return;
}

char* arg_to_string(arg_t* arg) {
    if (arg == NULL) return NULL;
    char* ret = malloc_safe(sizeof(char) * 64);
    switch (arg->kind) {
        case ArgVar:        sprintf(ret, "%s", arg->name); break;
        case ArgTmp:        sprintf(ret, "t%d", arg->cons); break;
        case ArgLabel:      sprintf(ret, "label%d", arg->cons); break;
        case ArgFunc:       sprintf(ret, "%s", arg->name); break;
        case ArgRelop:      sprintf(ret, "%s", arg->name); break;
        case ArgSize:       sprintf(ret, "%d", arg->cons); break;
        case ArgAddr:       sprintf(ret, "%s", arg->name); break;
        default: assert(0);
    }
    return ret;
}
char* ic_to_string(ic_t* ic) {
    char* ret = malloc_safe(sizeof(char) * 64);
    char* result = arg_to_string(ic->result);
    char* arg1 = arg_to_string(ic->arg1);
    char* arg2 = arg_to_string(ic->arg2);
    char* relop = arg_to_string(ic->relop);
    switch (ic->op) {
        case IcLabel:       sprintf(ret, "LABEL %s :\n", result); break;
        case IcFunc:        sprintf(ret, "FUNCTION %s :\n", result); break;
        case IcAssign:      sprintf(ret, "%s := %s\n", result, arg1); break;
        case IcAdd:         sprintf(ret, "%s := %s + %s\n", result, arg1, arg2); break;
        case IcSub:         sprintf(ret, "%s := %s - %s\n", result, arg1, arg2); break;
        case IcMul:         sprintf(ret, "%s := %s * %s\n", result, arg1, arg2); break;
        case IcDiv:         sprintf(ret, "%s := %s / %s\n", result, arg1, arg2); break;
        case IcRef:         sprintf(ret, "%s := &%s \n", result, arg1); break;
        case IcDeref:       sprintf(ret, "*%s := %s\n", result, arg1); break;
        case IcGoto:        sprintf(ret, "GOTO %s\n", result); break;
        case IcBranch:      sprintf(ret, "IF %s %s %s GOTO %s\n", arg1, relop, arg2, result); break;
        case IcReturn:      sprintf(ret, "RETURN %s\n", result); break;
        case IcArg:         sprintf(ret, "ARG %s\n", result); break;
        case IcCall:        sprintf(ret, "%s := CALL %s\n", result, arg1); break;
        case IcParam:       sprintf(ret, "PARAM %s\n", result); break;
        case IcRead:        sprintf(ret, "READ %s\n", result); break;
        case IcWrite:       sprintf(ret, "WRITE %s\n", result); break;
        case IcDec:         sprintf(ret, "DEC %s %s\n", result, arg1); break;
        case IcMinus:       sprintf(ret, "%s := #0 - %s\n", result, arg1); break;
        default: assert(0);
    }
    return ret;
}

void display_ir(FILE* ir) {
    ir_t* cur = ir_head;
    while (cur != NULL) {
        fprintf(ir, "%s", ic_to_string(cur->code));
        cur = cur->next;
    }
}


void generate_ir(syntax_t* root, FILE* ir) {
    translate_Program(root);
    display_ir(ir);
    return;
}