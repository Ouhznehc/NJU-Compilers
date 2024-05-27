#include "mips.h"

int set_arg_pointer = 0;
int param_offset = 12;

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

void init_space(FILE* fp) {
    fprintf(fp, ".data\n");
    fprintf(fp, "_prompt: .asciiz \"Enter an integer:\"\n");
    fprintf(fp, "_ret: .asciiz \"\\n\"\n");
    fprintf(fp, "\n");

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
    fprintf(fp, "   la $a0, _ret\n");
    fprintf(fp, "   syscall\n");
    fprintf(fp, "   move $v0, $0\n");
    fprintf(fp, "   jr $ra\n");
    fprintf(fp, "\n");  

}

void load(FILE* fp, const char* reg, arg_t* arg) {
    switch (arg->kind) {
        case ArgTmp:
            fprintf(fp, "   lw %s, -%d($s4)\n", reg, tmp_offset[arg->cons]);
            break;
        case ArgVar:
            fprintf(fp, "   lw %s, -%d($s4)\n", reg, var_offset[arg->cons]);
            break;
        case ArgImm:
            fprintf(fp, "   li %s, %d\n", reg, arg->cons);
            break; 
        default:
            printf("Invalid Arg: %s\n", arg_to_string(arg));
            assert(0); 
    }
}

void store(FILE* fp, const char* reg, arg_t* arg) {
    switch (arg->kind) {
        case ArgTmp:
            fprintf(fp, "   sw %s, -%d($s4)\n", reg, tmp_offset[arg->cons]);
            break;
        case ArgVar:
            fprintf(fp, "   sw %s, -%d($s4)\n", reg, var_offset[arg->cons]);
            break;
        case ArgImm:
            break; 
        default:
            printf("Invalid Arg: %s\n", arg_to_string(arg));
            assert(0); 
    }
}

void load_two(FILE* fp, ic_t* ic) {
    load(fp, registers[16], ic->result);
    load(fp, registers[17], ic->arg1);
}

void load_three(FILE* fp, ic_t* ic) {
    load(fp, registers[16], ic->result);
    load(fp, registers[17], ic->arg1);
    load(fp, registers[18], ic->arg2);
}

int find_func_no(char* name) {
    for (int i = 0; i <= func_no; i++) {
        if (!strcmp(name, func_name[i])) return i;
    }
    assert(0);
}


// only use 5 registers:
// result -> $s0($16)
// arg1 -> $s1($17)
// arg2 -> $s2($18)
// $v0($2)
// $ra($31)
// $s3(arg pointer)
// $s4(frame pointer)
/*
┌────────────────────────────┐
│                            │
├────────────────────────────┤
│            arg1            │
├────────────────────────────┤
│            arg2            │
├────────────────────────────┤
│            arg3            │
├────────────────────────────┤
│             .              │
│             .              │
├────────────────────────────┤
│      frame pointer(old)    │
├────────────────────────────┤
│       arg  pointer(old)    │
├────────────────────────────┤
│      return address        │
├────────────────────────────┤
│                            │
│     new func variables     │
│                            │
├────────────────────────────┤
│                            │
│                            │
└────────────────────────────┘
*/
void translate_ic(FILE* fp, ic_t* ic) {
    switch (ic->op) {
        case IcAdd:
            fprintf(fp, "   # %s", ic_to_string(ic));
            load_three(fp, ic);
            fprintf(fp, "   add $s0, $s1, $s2\n");
            store(fp, registers[16], ic->result);
            break;
        case IcSub:
            fprintf(fp, "   # %s", ic_to_string(ic));
            load_three(fp, ic);
            fprintf(fp, "   sub $s0, $s1, $s2\n");
            store(fp, registers[16], ic->result);
            break;
        case IcMul:
            fprintf(fp, "   # %s", ic_to_string(ic));
            load_three(fp, ic);
            fprintf(fp, "   mul $s0, $s1, $s2\n");
            store(fp, registers[16], ic->result);
            break;
        case IcDiv:
            fprintf(fp, "   # %s", ic_to_string(ic));
            load_three(fp, ic);
            fprintf(fp, "   div $s1, $s2\n");
            fprintf(fp, "   mflo $s0\n");           
            store(fp, registers[16], ic->result);
            break;
        case IcDec:
            break;
        case IcLabel:
            fprintf(fp, "label%d:\n", ic->result->cons);
            break;
        case IcFunc:
            fprintf(fp, "%s:\n", ic->result->name);
            // update frame pointer to the begin of new func
            fprintf(fp, "   move $s4, $sp\n");
            // allocate space for variables
            int func = find_func_no(ic->result->name);
            fprintf(fp, "   addi $sp, $sp, -%d\n", func_size[func]);
            break;
        case IcReturn:
            fprintf(fp, "   # %s", ic_to_string(ic));
            load(fp, registers[2], ic->result);
            fprintf(fp, "   jr $ra\n");
            break;
        case IcGoto:
            fprintf(fp, "   j %s\n", arg_to_string(ic->result));
            break;
        case IcArg:
            if (!set_arg_pointer) {
                set_arg_pointer = 1;
                fprintf(fp, "   move $s3, $sp\n");
            }
            fprintf(fp, "   addi $sp, $sp, -4\n");
            load(fp, registers[16], ic->result);
            fprintf(fp, "	sw $s0, 0($sp)\n");
            break;
        case IcParam:
            fprintf(fp, "   lw $s0, %d($s4)\n", param_offset);
            param_offset += 4;
            store(fp, registers[16], ic->result);
            break;
        case IcRead:
            fprintf(fp, "   # %s", ic_to_string(ic));
            fprintf(fp, "   addi $sp, $sp, -4\n");
            fprintf(fp, "	sw $ra, 0($sp)\n");
            fprintf(fp, "	jal read\n");
            fprintf(fp, "	lw $ra, 0($sp)\n");
            fprintf(fp, "	addi $sp, $sp, 4\n");
            store(fp, registers[2], ic->result);
            break;
        case IcWrite:
            fprintf(fp, "   # %s", ic_to_string(ic));
            load(fp, registers[4], ic->result);
            fprintf(fp, "   addi $sp, $sp, -4\n");
            fprintf(fp, "   sw $ra, 0($sp)\n");
            fprintf(fp, "   jal write\n");
            fprintf(fp, "   lw $ra, 0($sp)\n");
            fprintf(fp, "	addi $sp, $sp, 4\n");
            break;
        case IcAssign:
            fprintf(fp, "   # %s", ic_to_string(ic));
            load_two(fp, ic);
            fprintf(fp, "   move $s0, $s1\n");
            store(fp, registers[16], ic->result);
            break;   
        case IcCall:
            fprintf(fp, "   addi $sp, $sp, -12\n");
            fprintf(fp, "   sw $ra, 0($sp)\n");
            fprintf(fp, "   sw $s3, 4($sp)\n");
            fprintf(fp, "   sw $s4, 8($sp)\n");
            fprintf(fp, "   jal %s\n", ic->arg1->name);
            store(fp, registers[2], ic->result);
            fprintf(fp, "   lw $ra, 0($s4)\n");
            fprintf(fp, "   lw $s3, 4($s4)\n");
            fprintf(fp, "   lw $s4, 8($s4)\n");
            fprintf(fp, "   move $sp, $s3\n");
            break;
        case IcMinus:
            fprintf(fp, "   # %s", ic_to_string(ic));
            load_two(fp, ic);
            fprintf(fp, "   sub $s0, $zero, $s1\n");
            store(fp, registers[16], ic->result);
            break;
        case IcLeftStar:
            fprintf(fp, "   # %s", ic_to_string(ic));
            load_two(fp, ic);
            fprintf(fp, "   sw $s1, 0($s0)\n");
            break;
        case IcRightStar:
            fprintf(fp, "   # %s", ic_to_string(ic));
            load_two(fp, ic);
            fprintf(fp, "   lw $s0, 0($s1)\n");
            store(fp,registers[16], ic->result);
            break; 
        case IcRef:
            fprintf(fp, "   # %s", ic_to_string(ic));
            fprintf(fp, "   la $s0, %s\n", arg_to_string(ic->arg1));
            store(fp, registers[16], ic->result);
            break;     
        case IcBranch:
            fprintf(fp, "   # %s", ic_to_string(ic));
            load(fp, registers[17], ic->arg1);
            load(fp, registers[18], ic->arg2);
            if (!strcmp(ic->relop->name, "==")) {
                fprintf(fp, "   beq $s1, $s2, %s\n", arg_to_string(ic->result));
            }
            else if (!strcmp(ic->relop->name, "!=")) {
                fprintf(fp, "   bne $s1, $s2, %s\n", arg_to_string(ic->result));
            }
            else if (!strcmp(ic->relop->name, ">")) {
                fprintf(fp, "   bgt $s1, $s2, %s\n", arg_to_string(ic->result));
            }
            else if (!strcmp(ic->relop->name, "<")) {
                fprintf(fp, "   blt $s1, $s2, %s\n", arg_to_string(ic->result));
            }
            else if (!strcmp(ic->relop->name, ">=")) {
                fprintf(fp, "   bge $s1, $s2, %s\n", arg_to_string(ic->result));
            }
            else if (!strcmp(ic->relop->name, "<=")) {
                fprintf(fp, "   ble $s1, $s2, %s\n", arg_to_string(ic->result));
            }
            break; 
        default:
            printf("Invalid Intercode: %s", ic_to_string(ic));
            assert(0);
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