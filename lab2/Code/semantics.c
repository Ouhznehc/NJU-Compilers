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
    assert(kind == Basic || kind == Array || kind == Struct || kind == Function);

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
        case Function:
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

enum {CurScope, AllScope};
list_t* FindScopeItem(list_t** stack, int layer, char* name, int mode) {
    list_t* cur = stack[layer];
    switch (mode) {
        case CurScope:
            while (cur != NULL) {
                if (!strcmp(cur->name, name)) return cur;
                cur = cur->next;
            }
            return NULL;
        case AllScope:
            for (int i = layer; i >= 0; i--){
                list_t* ret = FindScopeItem(stack, i, name, CurScope);
                if(ret != NULL) return ret;
            }
            return NULL;
        default:
            assert(0);
    }
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
        FunDec(childs[1], specifier);
        CompSt(childs[2], specifier);
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

    syntax_t** childs = node->symbol.child;
    VarDec(childs[0]);
    // ExtDecList -> VarDec COMMA ExtDecList
    if (symcmp(childs[2], "ExtDecList")) {
        ExtDecList(childs[2]);
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
        list_t* item = FindScopeItem(StructScope, StructTop, record->record.name, CurScope);
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
    list_t* item = FindScopeItem(StructScope, StructTop, childs[0]->token.value.sval, CurScope);
    if(item == NULL) 
        semantic_error(UNDEFINED_STRUCT, childs[0]->lineno, childs[0]->token.value.sval);
    else {
        assert(!strcmp(item->name,item->type.record.name));
        ret = new_type(Struct, item->name, item->type.record.field);
    }
} 

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
void FunDec(syntax_t* node, type_t* specifier) { return; } 

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
void DefList(syntax_t* node, type_t* record) { return; } 

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