#ifndef __MIPS_H__
#define __MIPS_H__

#include "ir.h"

extern ir_t *ir_head, *ir_tail;
extern var_t* varlist;
extern int tmp_no;

void generate_asm(FILE* fp);

#endif