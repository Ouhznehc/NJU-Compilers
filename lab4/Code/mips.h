#ifndef __MIPS_H__
#define __MIPS_H__

#include "ir.h"

extern ir_t *ir_head, *ir_tail;
extern var_t* varlist;
extern int tmp_no;

extern int func_no;
extern char* func_name[1024];
extern int func_size[1024];
extern int var_offset[1024];
extern int tmp_offset[1024];


void generate_asm(FILE* fp);

#endif