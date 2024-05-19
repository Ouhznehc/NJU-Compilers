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

void store(FILE* fp, char* reg, char* addr) {
    fprintf(fp, "   sw %s, %s\n", reg, addr);
}

void load(FILE* fp, char* reg, char* addr) {
    fprintf(fp, "   lw %s, %s\n", reg, addr);
}

// only use 3 registers:
// result -> $s0($16)
// arg1 -> $s1($17)
// arg2 -> $s2($18)
void translate_ic(FILE* fp, ic_t* ic) {
    fprintf(fp, "# %s", ic_to_string(ic));
    switch (ic->op) {
        case IcLabel:
            fprintf(fp, "label%d:\n", ic->result->cons);
            break;
        case IcFunc:
            fprintf(fp, "\n%s:\n", ic->result->name);
            break;
        case IcAssign:
            load(fp, arg_to_string(ic->result), registers[16]);
            load(fp, arg_to_string(ic->arg1), registers[17]);
            fprintf(fp, "   move $s0, $s1\n");
            store(fp, registers[16], arg_to_string(ic->result));
            store(fp, registers[17], arg_to_string(ic->arg1));  
            break;   
    }
}

void display_asm(FILE* fp) {
    for(ir_t* cur = ir_head; cur; cur = cur->next)
        translate_ic(fp, cur->code);
}


void generate_asm(FILE* fp) {
    init_space(fp);
    display_asm(fp);
}