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

type_t* new_type(SemanticBasicType kind, ...) {
    type_t* ret = malloc(sizeof(type_t));
    memset(ret, 0, sizeof(type_t));

    ret->kind = kind;
    assert(kind == Basic || kind == Array || kind == Struct || kind == FuncDec || kind == FuncDef);

    va_list args;
    va_start(args, kind);
    switch (kind) {
        case Basic:
            ret->basic.type = va_arg(args, SemanticBasicType);
            break;
        case Array:
            ret->array.elem = va_arg(args, type_t*);
            ret->array.width = va_arg(args, int);
            break;
        case Struct:
            strcpy(ret->record.name, va_arg(args, char*));
            ret->record.field = va_arg(args, field_t*);  
            break;
        case FuncDef:
        case FuncDec:
            strcpy(ret->function.name, va_arg(args, char*));
            ret->function.lineno = va_arg(args, int);
            ret->function.argc = va_arg(args, int);
            ret->function.argv = va_arg(args, field_t*);
            ret->function.ret = va_arg(args, type_t*);
            break;
        default:
            assert(0);
    }
    va_end(args);
    return ret;
}

bool symcmp(syntax_t* node, char* name) {
    if (node == NULL) return false;
    return strcmp(node->name, name) == 0;
}

bool typecmp(type_t* t1, type_t* t2) {
    if (t1 == NULL || t2 == NULL) return true;
    if (t1->kind != t2->kind) return false;
    field_t* cur1, *cur2;
    switch (t1->kind) {
        case Basic:
            return t1->basic.type == t2->basic.type;
        case Array:
            return typecmp(t1->array.elem, t2->array.elem);
        case Struct:
            cur1 = t1->record.field;
            cur2 = t2->record.field;
            while (cur1 != NULL && cur2 != NULL){
                if(!typecmp(cur1->type, cur2->type)) return false;
                cur1 = cur1->next;
                cur2 = cur2->next;
            }
            if(cur1 == NULL && cur2 == NULL) return true;
            return false;
        case FuncDec:
        case FuncDef:
            if (!strcmp(t1->function.name, t2->function.name)) return false;
            if (t1->function.argc != t2->function.argc) return false;
            cur1 = t1->function.argv;
            cur2 = t2->function.argv;
            while (cur1 != NULL && cur2 != NULL){
                if(!typecmp(cur1->type, cur2->type)) return false;
                cur1 = cur1->next;
                cur2 = cur2->next;
            }
            return true;

    }
}

bool contains_field(type_t* type, char* name) {
    assert(type->kind == Struct || type->kind == FuncDef);
    field_t* cur = type->kind == Struct ? type->record.field : type->function.argv;
    while (cur != NULL) {
        if (!strcmp(cur->name, name)) return true;
        cur = cur->next;
    }
    return false;
}

enum {CurScope, AllScope};
item_t* NewScopeItem(char* name, type_t* type) {
    item_t* item = malloc(sizeof(item_t));
    strcpy(item->name, name);
    item->type = type;
    item->next = NULL;
    return item;
}
item_t* FindScopeItem(item_t** stack, int layer, char* name, int mode) {
    item_t* cur = stack[layer];
    switch (mode) {
        case CurScope:
            while (cur != NULL) {
                if (!strcmp(cur->name, name)) return cur;
                cur = cur->next;
            }
            return NULL;
        case AllScope:
            for (int i = layer; i >= 0; i--){
                item_t* ret = FindScopeItem(stack, i, name, CurScope);
                if(ret != NULL) return ret;
            }
            return NULL;
        default:
            assert(0);
    }
}
item_t* FindScopeItemWithType(item_t** stack, int layer, char* name, int kind, int mode) {
    item_t* cur = stack[layer];
    switch (mode) {
        case CurScope:
            while (cur != NULL) {
                if (!strcmp(cur->name, name) && cur->type->kind == kind) return cur;
                cur = cur->next;
            }
            return NULL;
        case AllScope:
            for (int i = layer; i >= 0; i--){
                item_t* ret = FindScopeItemWithType(stack, i, name, kind, CurScope);
                if(ret != NULL) return ret;
            }
            return NULL;
        default:
            assert(0);
    }
}
void InsertScopeItem(item_t** stack, int layer, item_t* item) {
    item->next = stack[layer];
    stack[layer] = item;
}

enum {VarStack, StructStack};
void StackPush(int type) {
    switch (type) {
        case VarStack:
            VarTop++;
            VarScope[VarTop] = NULL;
            break;
        case StructStack:
            StructTop++;
            StructScope[StructTop] = NULL;
            break;
        default:
            assert(0);
    }
}
void StackPop(int type) {
    switch (type) {
        case VarStack:
            VarScope[VarTop] = NULL;
            VarTop--;
            break;
        case StructStack:
            StructScope[StructTop] = NULL;
            StructTop--;
            break;
        default:
            assert(0);
    }
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
    ExtDef(childs[0]);
    ExtDefList(childs[1]);
}

/*
ExtDef:
    | Specifier ExtDecList SEMI
    | Specifier SEMI
    | Specifier FunDec CompSt
    | Specifier FunDec SEMI
*/
void ExtDef(syntax_t* node) { 
    assert(node != NULL);
    syntax_t** childs = node->symbol.child;

    type_t* specifier = Specifier(childs[0]);

    // ExtDef -> Specifier ExtDecList SEMI
    if (symcmp(childs[1], "ExtDecList")) {
        ExtDecList(childs[1], specifier);
    }
    else if (symcmp(childs[1], "FunDec")) {
        // ExtDef -> Specifier FunDec CompSt
        if (symcmp(childs[2], "CompSt")) {
            FunDec(childs[1], specifier, FuncDef);
            CompSt(childs[2], specifier);
        }
        // ExtDef -> Specifier FunDec SEMI
        else if (symcmp(childs[2], "SEMI")) {
            FunDec(childs[1], specifier, FuncDec);
        }
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
void ExtDecList(syntax_t* node, type_t* specifier) {
    assert(node != NULL);

    syntax_t** childs = node->symbol.child;
    VarDec(childs[0], specifier);
    // ExtDecList -> VarDec COMMA ExtDecList
    if (symcmp(childs[2], "ExtDecList")) {
        ExtDecList(childs[2], specifier);
    }
} 

/*
Specifier:
    | TYPE
    | StructSpecifier
*/
type_t* Specifier(syntax_t* node) { 
    assert(node != NULL);

    syntax_t** childs = node->symbol.child;
    // Specifier -> TYPE
    if (symcmp(childs[0], "TYPE")) {
        if (childs[0]->token.type == TOKEN_FLOAT) return new_type(Basic, Float);
        else if (childs[0]->token.type == TOKEN_INT) return new_type(Basic, Int);
        else assert(0);
    }
    else if (symcmp(childs[0], "StructSpecifier")) {
        return StructSpecifier(childs[0]);
    }
} 

/*
StructSpecifier:
    | STRUCT OptTag LC DefList RC
    | STRUCT Tag
*/
type_t* StructSpecifier(syntax_t* node) {
    assert(node != NULL);

    syntax_t** childs = node->symbol.child;
    // StructSpecifier -> STRUCT Tag
    if (symcmp(childs[1], "Tag")) {
        return Tag(childs[1]);
    }
    // StrucSpecifier -> STRUCT OptTag LC DefList RC
    // Note: OptTag may be NULL, we can't use symcmp(childs[1], "OptTag")
    else {
        type_t* record = OptTag(childs[1]);
        item_t* item = FindScopeItem(StructScope, StructTop, record->record.name, CurScope);
        if (item != NULL) {
            // must not be anonymous struct
            assert(childs[1] != NULL);
            semantic_error(DUPLICATED_STRUCT, childs[1]->lineno, record->record.name);
        }
        else {
            StackPush(VarStack);
            StackPush(StructStack);
            DefList(childs[3], record);
            StackPop(VarStack);
            StackPop(StructStack);
            item_t* item = NewScopeItem(record->record.name, record);
            InsertScopeItem(StructScope, StructTop, item);
            return record;
        }
    }
}

/*
OptTag:
    | ID
    | empty
*/
type_t* OptTag(syntax_t* node) { 
    // anonymous struct: manually assign a name, which starts with a number, 
    // so its name is impossible to conflict with another non-anonymous struct 
    if (node == NULL) {
        char name[64] = {0};
        sprintf(name, "%d_anonymous_struct", AnonymousStruct);
        return new_type(Struct, name, NULL); 
    }
    else {
        char* name = node->symbol.child[0]->token.value.sval;
        return new_type(Struct, name, NULL);
    }
} 

/*
Tag:
    | ID
*/
type_t* Tag(syntax_t* node) {
    assert(node != NULL);
    type_t* ret = NULL;
    syntax_t** childs = node->symbol.child;
    item_t* item = FindScopeItem(StructScope, StructTop, childs[0]->token.value.sval, CurScope);
    if(item == NULL) 
        semantic_error(UNDEFINED_STRUCT, childs[0]->lineno, childs[0]->token.value.sval);
    else {
        assert(!strcmp(item->name,item->type->record.name));
        ret = new_type(Struct, item->name, item->type->record.field);
    }
} 

/*
VarDec:
    | ID
    | VarDec LB INT RB
*/
item_t* VarDec(syntax_t* node, type_t* specifier) { 
    assert(node != NULL);
    syntax_t** childs = node->symbol.child;
    // VarDec -> ID
    if (symcmp(childs[0], "ID")) {
        return NewScopeItem(childs[0]->token.value.sval, specifier);
    }
    // VarDec -> VarDec LB INT RB
    else if (symcmp(childs[0], "VarDec")) {
        type_t* array = new_type(Array, specifier, childs[2]->token.value.ival);
        return VarDec(childs[0], array);
    }
    else assert(0);
} 

/*
FunDec:
    | ID LP VarList RP
    | ID LP RP
*/
// NOTE: type -> {FuncDec, FuncDef}
void FunDec(syntax_t* node, type_t* specifier, int type) { 
    assert(node != NULL);

    syntax_t** childs = node->symbol.child;
    item_t* func = NewScopeItem(
        childs[0]->token.value.sval,
        new_type(type, childs[0]->token.value.sval, childs[0]->lineno, 0, NULL, specifier) 
    );
    // FuncDec -> ID LP VarList RP
    if(symcmp(childs[2], "VarList")) {
        VarList(childs[2], func);
    }
    // FuncDec -> ID LP RP
    else {
        /*do nothing*/
    }
    item_t* funcDec = FindScopeItemWithType(VarScope, VarTop, func->name, FuncDec, CurScope);
    item_t* funcDef = FindScopeItemWithType(VarScope, VarTop, func->name, FuncDef, CurScope);
    type_t* funcDecType = funcDec == NULL ? NULL : funcDec->type;
    type_t* funcDefType = funcDef == NULL ? NULL : funcDef->type;
    if (type == FuncDec) {
        if(!typecmp(funcDecType, func->type) || !typecmp(funcDefType, func->type)) {
            semantic_error(CONFLICT_FUNC, func->type->function.lineno, func->name); 
            return;
        }
    }
    else if (type == FuncDef){
        if(funcDef != NULL) {
            semantic_error(DUPLICATE_FUNC, func->type->function.lineno, func->name);
            return;
        }
        if(!typecmp(funcDecType, func->type)) {
            semantic_error(CONFLICT_FUNC, func->type->function.lineno, func->name); 
            return;
        }
    }
    else assert(0);

    if (type == FuncDec && funcDec == NULL) InsertScopeItem(VarScope, VarTop, func);
    else if (type == FuncDef) InsertScopeItem(VarScope, VarTop, func);
    else return;
} 

/*
VarList:
    | ParamDec COMMA VarList
    | ParamDec
*/
void VarList(syntax_t* node, item_t* func) {
    assert(node != NULL);
    StackPush(VarStack);

} 

/*
ParamDec:
    | Specifier VarDec
*/
item_t* ParamDec(syntax_t* node) { return NULL; } 

/* 
CompSt:
    | LC DefList StmtList RC
*/
void CompSt(syntax_t* node, type_t* specifier) { return; }

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
void DefList(syntax_t* node, type_t* record) { 

} 

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