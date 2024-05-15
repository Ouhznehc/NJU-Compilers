#include "mips.h"

const char* registers[32] = {
    "$zero",
    "$at",
    "$v0", "$v1",
    "$a0", "$a1", "$a2", "$a3",
    "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
    "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
    "$t8", "$t9",
    "$k0", "$k1",   
    "$gp",
    "$sp",
    "$fp",
    "$ra"
};

void init_var_space(FILE* fp) {
    fprintf(fp, "# temp var declaration\n");
    for (int i = 1; i <= tmp_no; i++) {
        fprintf(fp, "t%d: .word 0\n", i);
    }

    fprintf(fp, "# variable declaration\n");
    var_t* cur = varlist;
    while (cur != NULL) {
        if (cur->size != 4) fprintf(fp, "v%d: .space %d\n", cur->var->cons, cur->size);
        else fprintf(fp, "v%d: .word 0\n", cur->var->cons);
        cur = cur->next;
    }
    fprintf(fp, "\n");
}

void init_space(FILE* fp) {
    fprintf(fp, ".data\n");
    fprintf(fp, "# For implement convience, store both variable and temp in static area\n");
    fprintf(fp, "_prompt: .asciiz \"Enter an integer:\"\n");
    fprintf(fp, "_ret: .asciiz \"\\n\"\n");
    fprintf(fp, "\n");

    init_var_space(fp);

    fprintf(fp, ".globl main\n");
    fprintf(fp, "\n");

    fprintf(fp, ".text\n");
    fprintf(fp, "# Function definitions\n");
    fprintf(fp, "read:\n");
    fprintf(fp, "   li $v0, 4\n");
    fprintf(fp, "   la $a0, _prompt\n");
    fprintf(fp, "   syscall\n");
    fprintf(fp, "   li $v0, 5\n");
    fprintf(fp, "   syscall\n");
    fprintf(fp, "   jr $ra\n");
    fprintf(fp, "\n");

    fprintf(fp, "write:\n");
    fprintf(fp, "   li $v0, 1\n");
    fprintf(fp, "   syscall\n");
    fprintf(fp, "   li $v0, 4\n");
    fprintf(fp, "   li $a0, _ret\n");
    fprintf(fp, "   syscall\n");
    fprintf(fp, "   move $v0, $0\n");
    fprintf(fp, "   jr $ra\n");
    fprintf(fp, "\n");  

}

char* translate_arg(FILE* fp, arg_t* arg) {
    char* ret = malloc_safe(sizeof(char) * 64);
    switch (arg->kind){
        case ArgVar:

            break;
    
        default:
            assert(0);
    }
    return ret;
}


char* translate_ic(FILE* fp, ic_t* ic) {
    char* ret = malloc_safe(sizeof(char) * 64);
    char* result = translate_arg(fp, ic->result);
    char* arg1 = translate_arg(fp, ic->arg1);
    char* arg2 = translate_arg(fp, ic->arg2);

    return ret;
}

void display_asm(FILE* fp) {
    for(ir_t* cur = ir_head; cur; cur = cur->next)
        fprintf(fp, "%s\n", translate_ic(fp, cur->code));
}


void generate_asm(FILE* fp) {
    init_space(fp);
    display_asm(fp);
}