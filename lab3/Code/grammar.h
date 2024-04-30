#ifndef __GRAMMAR_H__
#define __GRAMMAR_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
extern int error_no;


enum {
    TOKEN_ID,
    TOKEN_TYPE,
    TOKEN_INT_DEC,
    TOKEN_INT_HEX,
    TOKEN_INT_OCT,
    TOKEN_RELOP,
    TOKEN_CONST,
    TOKEN_INT,
    TOKEN_FLOAT,
    SYMBOL,
    TOKEN
};

union value_t {
    unsigned int   ival;
    float          fval;
    char           sval[64];
};
typedef union value_t value_t;

struct token_t {
    int type;
    union value_t value;
};
typedef struct token_t token_t;


struct symbol_t {
    int size;
    int rule;
    struct syntax_t* child[8]; 
};
typedef struct symbol_t symbol_t;


struct syntax_t {
    union{
        struct symbol_t symbol;
        struct token_t token;
    };
    char name[64];
    int lineno;
    int type;
};
typedef struct syntax_t syntax_t;

syntax_t* new_token(char *name, int type, int lineno, value_t value);
syntax_t* new_symbol(char* name, int rule, int lineno, int size, ...);
void print_syntax_tree(syntax_t* node, int indent);


#endif