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

void load(FILE* fp, arg_t* arg, const char* reg) {
    switch (arg->kind) {
        case ArgTmp:
            fprintf(fp, "   lw %s, t%d\n", reg, arg->cons);
            break;
        case ArgVar:
            fprintf(fp, "   lw %s, v%d\n", reg, arg->cons);
            break;
        case ArgImm:
            fprintf(fp, "   li %s, %d\n", reg, arg->cons);
            break; 
        default:
            assert(0); 
    }
}

void store(FILE* fp, const char* reg, arg_t* arg) {
    switch (arg->kind) {
        case ArgTmp:
            fprintf(fp, "   sw %s, t%d\n", reg, arg->cons);
            break;
        case ArgVar:
            fprintf(fp, "   sw %s, v%d\n", reg, arg->cons);
            break;
        case ArgImm:
            break; 
        default:
            assert(0); 
    }
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
            fprintf(fp, "   # %s\n", ic_to_string(ic));
            load(fp, ic->result, registers[16]);
            load(fp, ic->arg1, registers[17]);
            fprintf(fp, "   move $s0, $s1\n");
            store(fp, registers[16], ic->result);
            store(fp, registers[17], ic->arg1);  
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