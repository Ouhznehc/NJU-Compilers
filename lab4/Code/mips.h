#ifndef __MIPS_H__
#define __MIPS_H__

#include "ir.h"

extern ir_t *ir_head, *ir_tail;
extern var_t* varlist;
extern int tmp_no;

typedef struct arglist_t arglist_t;
struct arglist_t {
    arg_t* arg;
    arglist_t* next;
};


void generate_asm(FILE* fp);

#endif