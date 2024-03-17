#include "syntax.h"

syntax_t* new_token(char *name, int type, int lineno, value_t value) {
    token_t* ret = malloc(sizeof(token_t));
    strcpy(ret->name, name);
    ret->type = type;
    ret->lineno = lineno;

    switch (type) {
        case TOKEN_INT:
            ret->ival = value.ival;
            break;
        case TOKEN_FLOAT:
            ret->fval = value.fval;
            break;
        case TOKEN_ID, TOKEN_TYPE:
            strcpy(ret->sval, value.sval);
            break;
        default: break;
    }
    ret->value = value;
    return ret;
}

syntax_t* new_symbol(char* name, int lineno, int size, ...){
    symbol_t* ret = malloc(sizeof(symbol_t));
    strcpy(ret->name, name);
    ret->lineno = lineno;

    ret->size = size;
    ret->child = malloc(size * sizeof(syntax_t*));
    va_list args;
    va_start(args, size);
    for (int i = 0; i < size; i++){
        syntax_t* child = va_arg(args, syntax_t*);
        ret->child[i] = child;
    }    
    va_end(args);

    return ret;
}