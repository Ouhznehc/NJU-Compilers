#include "grammar.h"

syntax_t* new_token(char *name, int type, int lineno, value_t value) {
    syntax_t* ret = malloc(sizeof(syntax_t));
    ret->type = TOKEN;

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
    ret->type = SYMBOL;

    strcpy(ret->symbol.name, name);
    ret->symbol.lineno = lineno;

    ret->symbol.size = size;
    if (size == 0) ret->symbol.child = NULL;
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

void print_syntax_tree(syntax_t* node, int indent) {
    if (node == NULL) return;
    for (int i = 0; i < indent; i++) printf("  ");

    if(node->type == TOKEN) {
        if (node->token.type == TOKEN_ID || node->token.type == TOKEN_TYPE) printf("%s: %s", node->token.name, node->token.value.sval);
        else if (node->token.type == TOKEN_INT) printf("%s: %u", node->token.name, node->token.value.ival);
        else if (node->token.type == TOKEN_FLOAT) printf("%s: %f", node->token.name, node->token.value.fval);
        else printf("%s", node->token.name);
        printf("\n");
        return;
    }

    assert(node->type == SYMBOL);
    
    printf("%s (%d)", node->symbol.name, node->symbol.lineno);
    printf("\n");

    for (int i = 0; i < node->symbol.size; i++)
        print_syntax_tree(node->symbol.child[i], indent + 1);

    return;
}