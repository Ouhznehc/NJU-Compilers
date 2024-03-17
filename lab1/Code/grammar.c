#include "grammar.h"

syntax_t* new_token(char *name, int type, int lineno, value_t value) {
    syntax_t* ret = malloc(sizeof(syntax_t));
    strcpy(ret->token.name, name);
    ret->token.type = type;
    ret->token.lineno = lineno;

    switch (type) {
        case TOKEN_INT:
            ret->token.value.ival = value.ival;
            break;
        case TOKEN_FLOAT:
            ret->token.value.fval = value.fval;
            break;
        case TOKEN_ID:
        case TOKEN_TYPE:
            strcpy(ret->token.value.sval, value.sval);
            break;
        default: break;
    }
    return ret;
}

syntax_t* new_symbol(char* name, int lineno, int size, ...){
    syntax_t* ret = malloc(sizeof(syntax_t));
    strcpy(ret->symbol.name, name);
    ret->symbol.lineno = lineno;

    ret->symbol.size = size;
    ret->symbol.child = malloc(size * sizeof(syntax_t*));
    va_list args;
    va_start(args, size);
    for (int i = 0; i < size; i++){
        syntax_t* child = va_arg(args, syntax_t*);
        ret->symbol.child[i] = child;
    }    
    va_end(args);

    return ret;
}