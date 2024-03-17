#ifndef __SYNTAX_H__
#define __SYNTAX_H__


#include <stdio.h>

enum {
    TOKEN_ID,
    TOKEN_TYPE,
    TOKEN_INT_DEC,
    TOKEN_INT_HEX,
    TOKEN_INT_OCT,
    TOKEN_FLOAT_NRM,
    TOKEN_FLOAT_SCI,
    TOKEN_CONST,
    TOKEN_INT,
    TOKEN_FLOAT
};

union value_t {
    int   ival;
    float fval;
    char  sval[64];
};
typedef union value_t value_t;

struct token_t {
    const char name[64];
    int type;
    int lineno;
    union value_t value;
};
typedef struct token_t token_t;


struct symbol_t {
    const char name[64];
    int lineno;
    int size;
    union syntax_t** child; 
};
typedef struct symbol_t symbol_t;


union syntax_t {
    struct symbol_t symbol;
    struct token_t token;
};
typedef union syntax_t syntax_t;

token_t* new_token(char *name, int type, int lineno, value_t value);
symbol_t* new_symbol(char* name, int lineno, int size, ...);

#endif