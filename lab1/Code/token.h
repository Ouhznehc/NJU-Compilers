#ifndef __TOKEN_H__
#define __TOKEN_H__


#include <stdio.h>

enum {
    TOKEN_ID,
    TOKEN_INT_DEC,
    TOKEN_INT_HEX,
    TOKEN_INT_OCT,
    TOKEN_FLOAT_NRM,
    TOKEN_FLOAT_SCI,
    SYMBOL
};

struct token_t {
    const char *name;
    int type;
    int lineno;
    union {
        int   ival;
        float fval;
        char  id;
    };
    
    int size;
    struct token_t **child;
};
typedef struct token_t token_t;

token_t* new_token(char *name, int type, int lineno);
void update_token_tree(token_t *root, int size);

#endif