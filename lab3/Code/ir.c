#include "ir.h"


ir_t *ir_head, *ir_tail;
int var_no = 0, tmp_no = 0, label_no = 0;
var_t* VarList;

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

#define min(a, b) ( ((a) < (b)) ? (a) : (b) ) 

void* malloc_safe(int size) {
    void* ret = malloc(size);
    memset(ret, 0, size);
    return ret;
}

void insert_var(arg_t* var) {
    var_t* item = malloc_safe(sizeof(var_t));
    item->var = var;
    item->next = VarList;
    VarList = item;
}

arg_t* find_var(char* name) {
    var_t* cur = VarList;
    
    while (cur != NULL) {
        if(!strcmp(cur->var->name, name)) return cur->var;
        cur = cur->next;
    }
    assert(0);
}

arg_t* new_arg(int kind, char* name, int cons, bool is_addr) {
    arg_t* ret = malloc_safe(sizeof(arg_t));
    ret->kind = kind;
    ret->is_addr = is_addr;
    switch (kind) {
        case ArgTmp: case ArgImm: case ArgLabel: case ArgSize:
            ret->cons = cons;
            break;
        case ArgFunc: case ArgRelop: case ArgVar:
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
        case IcLeftStar: case IcRightStar: case IcRef:
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


void insert_ir(ic_t* ic) {
    ir_t* ir = new_ir(ic);
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

arg_t* deref(arg_t* arg) {
    if(!arg->is_addr) return arg;
    arg_t* tmp = new_arg(ArgTmp, NULL, ++tmp_no, false);
    insert_ir(new_ic(IcRightStar, tmp, arg));
    return tmp;
}
arg_t* ref(arg_t* arg) {
    if(arg->is_addr) return arg;
    arg_t* tmp = new_arg(ArgTmp, NULL, ++tmp_no, true);
    insert_ir(new_ic(IcRef, tmp, arg));
    return tmp;
}

void insert_assign_ir(arg_t* left, arg_t* right) {
    arg_t* tmp = deref(right);
    if (left->is_addr) insert_ir(new_ic(IcLeftStar, left, tmp));
    else insert_ir(new_ic(IcAssign, left, tmp));
}

arg_t* fetch_addr(arg_t* base, int offset) {
    arg_t* tmp = new_arg(ArgTmp, NULL, ++tmp_no, true);
    if (!base->is_addr) insert_ir(new_ic(IcRef, tmp, base));
    else insert_ir(new_ic(IcAssign, tmp, base));
    arg_t* off = new_arg(ArgImm, NULL, offset, false);
    arg_t* ret = new_arg(ArgTmp, NULL, ++tmp_no, true);
    insert_ir(new_ic(IcAdd, ret, tmp, off));
    return ret;
}

void insert_assign_recursively(arg_t* dst, arg_t* src, int size) {
    for (int i = 0; i < size; i += 4) {
        arg_t* left = fetch_addr(dst, i);
        arg_t* right = fetch_addr(src, i);
        insert_assign_ir(left, right);
    }
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
    arg_t* ret = new_arg(ArgVar, childs[0]->token.value.sval, 0, false);
    item_t* var = FindScopeItem(childs[0]->token.value.sval);
    arg_t* size = new_arg(ArgSize, NULL, calculate_size(var->type), false);
    switch (node->symbol.rule) {
        // VarDec -> ID
        case 1:
            assert(var->type->kind != FuncDec && var->type->kind != FuncDef);
            if(var->type->kind != Basic) insert_ir(new_ic(IcDec, ret, size));
            insert_var(ret);
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
    
    arg_t* func = new_arg(ArgFunc, childs[0]->token.value.sval, 0, false);
    insert_ir(new_ic(IcFunc, func));

    type_t* type = FindScopeItem(childs[0]->token.value.sval)->type;
    assert(type->kind == FuncDef);

    for (field_t* cur = type->function.argv; cur; cur = cur->next) {
        int is_addr = cur->type->kind != Basic ? true : false;
        arg_t* param = new_arg(ArgVar, cur->name, 0, is_addr);
        insert_ir(new_ic(IcParam, param));
        insert_var(param);
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
        arg_t* label1 = new_arg(ArgLabel, NULL, ++label_no, false);
        arg_t* label2 = new_arg(ArgLabel, NULL, ++label_no, false);
        arg_t* label3 = new_arg(ArgLabel, NULL, ++label_no, false);
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
        arg_t* label1 = new_arg(ArgLabel, NULL, ++label_no, false);
        arg_t* label2 = new_arg(ArgLabel, NULL, ++label_no, false);
        translate_Cond(childs[2], label1, label2);
        insert_ir(new_ic(IcLabel, label1));
        translate_Stmt(childs[4]);
        insert_ir(new_ic(IcLabel, label2));
    }
    // Stmt -> WHILE LP Exp RP Stmt
    else if (rule == 6) {
        arg_t* label1 = new_arg(ArgLabel, NULL, ++label_no, false);
        arg_t* label2 = new_arg(ArgLabel, NULL, ++label_no, false);
        arg_t* label3 = new_arg(ArgLabel, NULL, ++label_no, false);
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
    assert(node != NULL);
    syntax_t** childs = node->symbol.child;
    translate_DecList(childs[1]);
} 

/*
DecList:
    | Dec
    | Dec COMMA DecList
*/
void translate_DecList(syntax_t* node) {  
    assert(node != NULL);
    syntax_t** childs = node->symbol.child;
    switch (node->symbol.rule) {
        case 1:
            translate_Dec(childs[0]); return;
        case 2:
            translate_Dec(childs[0]);
            translate_DecList(childs[2]); 
            return;
        default: assert(0);
    }
    return;
} 

/*
Dec:
    | VarDec
    | VarDec ASSIGNOP Exp
*/
void translate_Dec(syntax_t* node) { 
    assert(node != NULL);
    syntax_t** childs = node->symbol.child;
    int rule = node->symbol.rule;
    // Dec -> VarDec
    if (rule == 1) {
        translate_VarDec(childs[0]);
    }
    // Dec -> VarDec ASSIGNOP Exp
    else if (rule == 2) {
        arg_t* left = translate_VarDec(childs[0]);
        arg_t* right = translate_Exp(childs[2]);
        insert_assign_ir(left, right);
    }
    else assert(0);
    return;
} 



/*
Exp:
    | Exp ASSIGNOP Exp
    | Exp AND Exp
    | Exp OR Exp
    | Exp RELOP Exp
    | NOT Exp
    | Exp PLUS Exp 
    | Exp MINUS Exp
    | Exp STAR Exp
    | Exp DIV Exp
    | LP Exp RP
    | MINUS Exp
    | ID LP Args RP
    | ID LP RP
    | Exp LB Exp RB
    | Exp DOT ID
    | ID
    | INT
    | FLOAT
*/
arg_t* translate_Exp(syntax_t* node) { 
    assert(node != NULL);
    syntax_t** childs = node->symbol.child;

    int rule = node->symbol.rule;
    // Exp -> Exp ASSIGNOP Exp
    if (rule == 1) {
        type_t* exp1 = Exp(childs[0]);
        type_t* exp2 = Exp(childs[2]);
        arg_t* arg1 = translate_Exp(childs[0]);
        arg_t* arg2 = translate_Exp(childs[2]);
        // ASSIGNOP for Array or Struct recursively
        if (exp1->kind != Basic) {
            int size = min(calculate_size(exp1), calculate_size(exp2));
            insert_assign_recursively(arg1, arg2, size);
        }
        else insert_assign_ir(arg1, arg2);
        return arg1;
    }
    // Exp -> Exp AND Exp
    // Exp -> Exp OR Exp
    // Exp -> Exp RELOP Exp
    // Exp -> NOT Exp
    else if (rule == 2 || rule == 3 || rule == 4 || rule == 5) {
        arg_t* label1 = new_arg(ArgLabel, NULL, ++label_no, false);
        arg_t* label2 = new_arg(ArgLabel, NULL, ++label_no, false);
        arg_t* ret = new_arg(ArgTmp, NULL, ++tmp_no, false);

        insert_ir(new_ic(IcAssign, ret, new_arg(ArgImm, NULL, 0, false)));
        translate_Cond(node, label1, label2);
        insert_ir(new_ic(IcLabel, label1));
        insert_ir(new_ic(IcAssign, ret, new_arg(ArgImm, NULL, 1, false)));
        insert_ir(new_ic(IcLabel, label2));
        return ret;
    }
    // Exp -> Exp PLUS Exp
    // Exp -> Exp MINUS Exp
    // Exp -> Exp STAR Exp
    // Exp -> Exp DIV Exp 
    else if (rule == 6 || rule == 7 || rule == 8 || rule == 9) {
        arg_t* exp1 = translate_Exp(childs[0]);
        arg_t* exp2 = translate_Exp(childs[2]);
        arg_t* ret = new_arg(ArgTmp, NULL, ++tmp_no, false);
        exp1 = deref(exp1);
        exp2 = deref(exp2);
        insert_ir(new_ic(rule, ret, exp1, exp2));
        return ret;
    }
    // Exp -> LP Exp RP
    else if (rule == 10) {
        return translate_Exp(childs[1]);
    }
    // Exp -> MINUS Exp
    else if (rule == 11) {
        arg_t* exp = translate_Exp(childs[1]);
        exp = deref(exp);
        arg_t* ret = new_arg(ArgTmp, NULL, ++tmp_no, false);
        insert_ir(new_ic(IcMinus, ret, exp));
        return ret;
    }
    // Exp -> ID LP Args RP
    else if (rule == 12) {
        if(!strcmp(childs[0]->token.value.sval, "write")) {

            assert(childs[2]->symbol.rule = 2);
            syntax_t* child = childs[2]->symbol.child[0];
            arg_t* arg = translate_Exp(child);
            arg = deref(arg);
            insert_ir(new_ic(IcWrite, arg));
            return NULL;
        }
        arg_t* func = new_arg(ArgFunc, childs[0]->token.value.sval, 0, false);
        arg_t* ret = new_arg(ArgTmp, NULL, ++tmp_no, false);
        translate_Args(childs[2]);
        insert_ir(new_ic(IcCall, ret, func));
        return ret;
    }
    // Exp -> ID LP RP
    else if (rule == 13) {
        arg_t* func = new_arg(ArgFunc, childs[0]->token.value.sval, 0, false);
        arg_t* ret = new_arg(ArgTmp, NULL, ++tmp_no, false);
        if(!strcmp(childs[0]->token.value.sval, "read")) {
            insert_ir(new_ic(IcRead, ret));
            return ret;
        }
        insert_ir(new_ic(IcCall, ret, func));
        return ret;
    }
    // Exp -> Exp LB Exp RB
    else if (rule == 14) {
        arg_t* exp1 = translate_Exp(childs[0]);
        arg_t* exp2 = translate_Exp(childs[2]);
        arg_t* ret = new_arg(ArgTmp, NULL, ++tmp_no, true);
        exp1 = ref(exp1);
        exp2 = deref(exp2);
        type_t* array = Exp(childs[0]);
        arg_t* size = new_arg(ArgImm, NULL, calculate_size(array), false);
        arg_t* offset = new_arg(ArgTmp, NULL, ++tmp_no, false);
        insert_ir(new_ic(IcMul, offset, size, exp2));
        insert_ir(new_ic(IcAdd, ret, exp1, offset));
        return ret;
    }
    // Exp -> Exp DOT ID
    else if (rule == 15) {
        arg_t* exp = translate_Exp(childs[0]);
        arg_t* ret = new_arg(ArgTmp, NULL, ++tmp_no, true);
        type_t* record = Exp(childs[0]);
        exp = ref(exp);
        int size = 0;
        for (field_t* cur = record->record.field; cur; cur = cur->next) {
            if (!strcmp(cur->name, childs[2]->token.value.sval)) break;
            size += calculate_size(cur->type);
        }
        arg_t* offset = new_arg(ArgImm, NULL, size, false);
        insert_ir(new_ic(IcAdd, ret, exp, offset));
        return ret;
    }
    // Exp -> ID
    else if (rule == 16) {
        return find_var(childs[0]->token.value.sval);
    }
    // Exp -> INT
    else if (rule == 17) {
        arg_t* ret = new_arg(ArgImm, NULL, childs[0]->token.value.ival, false);
        return ret;
    }
    // Exp -> FLOAT
    else if (rule == 18) {
        assert(0);
    }
    else assert(0);
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
        case ArgImm:        sprintf(ret, "#%d", arg->cons); break;
        case ArgLabel:      sprintf(ret, "label%d", arg->cons); break;
        case ArgFunc:       sprintf(ret, "%s", arg->name); break;
        case ArgRelop:      sprintf(ret, "%s", arg->name); break;
        case ArgSize:       sprintf(ret, "%d", arg->cons); break;
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
        case IcLeftStar:    sprintf(ret, "*%s := %s\n", result, arg1); break;
        case IcRightStar:   sprintf(ret, "%s := *%s\n", result, arg1); break;
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