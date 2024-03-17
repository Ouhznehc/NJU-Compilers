#ifndef __GRAMMAR_H__
#define __GRAMMAR_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>


enum {
    TOKEN_ID,
    TOKEN_TYPE,
    TOKEN_INT_DEC,
    TOKEN_INT_HEX,
    TOKEN_INT_OCT,
    TOKEN_CONST,
    TOKEN_INT,
    TOKEN_FLOAT,
    SYMBOL
};

union value_t {
    unsigned int   ival;
    float          fval;
    char           sval[64];
};
typedef union value_t value_t;

struct token_t {
    char name[64];
    int type;
    int lineno;
    union value_t value;
};
typedef struct token_t token_t;


struct symbol_t {
    char name[64];
    int lineno;
    int size;
    int type;
    union syntax_t** child; 
};
typedef struct symbol_t symbol_t;


union syntax_t {
    struct symbol_t symbol;
    struct token_t token;
};
typedef union syntax_t syntax_t;

syntax_t* new_token(char *name, int type, int lineno, value_t value);
syntax_t* new_symbol(char* name, int lineno, int size, ...);
void print_syntax_tree(syntax_t* node, int indent);


#endif