#include "mips.h"

int set_arg_pointer = 0;

const char* registers[8] = {
    "eax", "ebx", "ecx", "edx",
    "esi", "edi", "ebp", "esp"
};

void init_space(FILE* fp) {
    fprintf(fp, "section .data\n");
    fprintf(fp, "_prompt db \"Enter an integer:\", 0\n");
    fprintf(fp, "_ret db 0xA, 0\n");
    fprintf(fp, "\n");

    fprintf(fp, "section .text\n");
    fprintf(fp, "global _start\n");
    fprintf(fp, "\n");

    fprintf(fp, "_start:\n");
    fprintf(fp, "\n");

    fprintf(fp, "read:\n");
    fprintf(fp, "   mov eax, 4\n");
    fprintf(fp, "   mov ebx, 1\n");
    fprintf(fp, "   mov ecx, _prompt\n");
    fprintf(fp, "   mov edx, 17\n");
    fprintf(fp, "   int 0x80\n");
    fprintf(fp, "   mov eax, 3\n");
    fprintf(fp, "   mov ebx, 0\n");
    fprintf(fp, "   mov ecx, _ret\n");
    fprintf(fp, "   mov edx, 4\n");
    fprintf(fp, "   int 0x80\n");
    fprintf(fp, "   ret\n");
    fprintf(fp, "\n");

    fprintf(fp, "write:\n");
    fprintf(fp, "   mov eax, 1\n");
    fprintf(fp, "   mov ebx, 1\n");
    fprintf(fp, "   int 0x80\n");
    fprintf(fp, "   mov eax, 4\n");
    fprintf(fp, "   mov ebx, 1\n");
    fprintf(fp, "   mov ecx, _ret\n");
    fprintf(fp, "   mov edx, 1\n");
    fprintf(fp, "   int 0x80\n");
    fprintf(fp, "   xor eax, eax\n");
    fprintf(fp, "   ret\n");
}

int arg_offset(arg_t* arg) {
    switch (arg->kind) {
        case ArgTmp:
            return tmp_offset[arg->cons];
        case ArgVar:
            return var_offset[arg->cons];
        default:
            printf("Invalid Arg: %s\n", arg_to_string(arg));
            assert(0);   
    }
}

void load(FILE* fp, const char* reg, arg_t* arg) {
    switch (arg->kind) {
        case ArgTmp:
        case ArgVar:
            fprintf(fp, "   mov %s, [ebp-%d]\n", reg, arg_offset(arg));
            break;
        case ArgImm:
            fprintf(fp, "   mov %s, %d\n", reg, arg->cons);
            break; 
        default:
            printf("Invalid Arg: %s\n", arg_to_string(arg));
            assert(0); 
    }
}

void store(FILE* fp, const char* reg, arg_t* arg) {
    switch (arg->kind) {
        case ArgTmp:
        case ArgVar:
            fprintf(fp, "   mov [ebp-%d], %s\n", arg_offset(arg), reg);
            break;
        case ArgImm:
            break; 
        default:
            printf("Invalid Arg: %s\n", arg_to_string(arg));
            assert(0); 
    }
}

void load_two(FILE* fp, ic_t* ic) {
    load(fp, registers[4], ic->result);
    load(fp, registers[5], ic->arg1);
}

void load_three(FILE* fp, ic_t* ic) {
    load(fp, registers[4], ic->result);
    load(fp, registers[5], ic->arg1);
    load(fp, registers[6], ic->arg2);
}

int find_func_no(char* name) {
    for (int i = 0; i <= func_no; i++) {
        if (!strcmp(name, func_name[i])) return i;
    }
    assert(0);
}

// 使用寄存器：
// result -> esi
// arg1 -> edi
// arg2 -> ebx
// arg pointer -> edx
// param counter -> ecx
// eax, ebp, esp
void translate_ic(FILE* fp, ic_t* ic) {
    switch (ic->op) {
        case IcAdd:
            fprintf(fp, "   ; %s", ic_to_string(ic));
            load_three(fp, ic);
            fprintf(fp, "   add esi, edi, ebx\n");
            store(fp, registers[4], ic->result);
            break;
        case IcSub:
            fprintf(fp, "   ; %s", ic_to_string(ic));
            load_three(fp, ic);
            fprintf(fp, "   sub esi, edi, ebx\n");
            store(fp, registers[4], ic->result);
            break;
        case IcMul:
            fprintf(fp, "   ; %s", ic_to_string(ic));
            load_three(fp, ic);
            fprintf(fp, "   imul esi, edi, ebx\n");
            store(fp, registers[4], ic->result);
            break;
        case IcDiv:
            fprintf(fp, "   ; %s", ic_to_string(ic));
            load_three(fp, ic);
            fprintf(fp, "   mov eax, edi\n");
            fprintf(fp, "   cdq\n");
            fprintf(fp, "   idiv ebx\n");
            fprintf(fp, "   mov esi, eax\n");
            store(fp, registers[4], ic->result);
            break;
        case IcDec:
            break;
        case IcLabel:
            fprintf(fp, "label%d:\n", ic->result->cons);
            break;
        case IcFunc:
            fprintf(fp, "\n");
            fprintf(fp, "%s:\n", ic->result->name);
            fprintf(fp, "   push ebp\n");
            fprintf(fp, "   mov ebp, esp\n");
            int func = find_func_no(ic->result->name);
            fprintf(fp, "   sub esp, %d\n", func_size[func]);
            break;
        case IcReturn:
            fprintf(fp, "   ; %s", ic_to_string(ic));
            load(fp, registers[0], ic->result);
            fprintf(fp, "   mov esp, ebp\n");
            fprintf(fp, "   pop ebp\n");
            fprintf(fp, "   ret\n");
            break;
        case IcGoto:
            fprintf(fp, "   jmp %s\n", arg_to_string(ic->result));
            break;
        case IcArg:
            fprintf(fp, "   ; %s", ic_to_string(ic));
            if (!set_arg_pointer) {
                set_arg_pointer = 1;
                fprintf(fp, "   mov edx, esp\n");
            }
            fprintf(fp, "   sub esp, 4\n");
            load(fp, registers[4], ic->result);
            fprintf(fp, "   mov [esp], esi\n");
            break;
        case IcParam:
            fprintf(fp, "   ; %s", ic_to_string(ic));
            fprintf(fp, "   lea ecx, [ebp + ecx]\n");
            fprintf(fp, "   mov esi, [ecx]\n");
            fprintf(fp, "   add ecx, 4\n");
            store(fp, registers[4], ic->result);
            break;  
        case IcRead:
            fprintf(fp, "   ; %s", ic_to_string(ic));
            fprintf(fp, "   sub esp, 4\n");
            fprintf(fp, "   push eax\n");
            fprintf(fp, "   call read\n");
            fprintf(fp, "   pop eax\n");
            fprintf(fp, "   add esp, 4\n");
            store(fp, registers[0], ic->result);
            break;
        case IcWrite:
            fprintf(fp, "   ; %s", ic_to_string(ic));
            load(fp, registers[1], ic->result);
            fprintf(fp, "   sub esp, 4\n");
            fprintf(fp, "   push eax\n");
            fprintf(fp, "   call write\n");
            fprintf(fp, "   pop eax\n");
            fprintf(fp, "   add esp, 4\n");
            break;
        case IcAssign:
            fprintf(fp, "   ; %s", ic_to_string(ic));
            load_two(fp, ic);
            fprintf(fp, "   mov esi, edi\n");
            store(fp, registers[4], ic->result);
            break;   
        case IcCall:
            fprintf(fp, "   ; %s", ic_to_string(ic));
            if (!set_arg_pointer) {
                fprintf(fp, "   mov edx, esp\n");
            }
            set_arg_pointer = 0;
            fprintf(fp, "   mov ecx, 12\n");
            fprintf(fp, "   sub esp, 12\n");
            fprintf(fp, "   push eax\n");
            fprintf(fp, "   push edx\n");
            fprintf(fp, "   push ebp\n");
            fprintf(fp, "   call %s\n", ic->arg1->name);
            fprintf(fp, "   pop ebp\n");
            fprintf(fp, "   pop edx\n");
            fprintf(fp, "   pop eax\n");
            fprintf(fp, "   mov esp, edx\n");
            store(fp, registers[0], ic->result);
            break;
        case IcMinus:
            fprintf(fp, "   ; %s", ic_to_string(ic));
            load_two(fp, ic);
            fprintf(fp, "   neg edi\n");
            store(fp, registers[4], ic->result);
            break;
        case IcLeftStar:
            fprintf(fp, "   ; %s", ic_to_string(ic));
            load_two(fp, ic);
            fprintf(fp, "   mov [esi], edi\n");
            break;
        case IcRightStar:
            fprintf(fp, "   ; %s", ic_to_string(ic));
            load_two(fp, ic);
            fprintf(fp, "   mov esi, [edi]\n");
            store(fp, registers[4], ic->result);
            break; 
        case IcRef:
            fprintf(fp, "   ; %s", ic_to_string(ic));
            fprintf(fp, "   lea esi, [ebp-%d]\n", arg_offset(ic->arg1));
            store(fp, registers[4], ic->result);
            break;     
        case IcBranch:
            fprintf(fp, "   ; %s", ic_to_string(ic));
            load(fp, registers[5], ic->arg1);
            load(fp, registers[6], ic->arg2);
            if (!strcmp(ic->relop->name, "==")) {
                fprintf(fp, "   je %s\n", arg_to_string(ic->result));
            }
            else if (!strcmp(ic->relop->name, "!=")) {
                fprintf(fp, "   jne %s\n", arg_to_string(ic->result));
            }
            else if (!strcmp(ic->relop->name, ">")) {
                fprintf(fp, "   jg %s\n", arg_to_string(ic->result));
            }
            else if (!strcmp(ic->relop->name, "<")) {
                fprintf(fp, "   jl %s\n", arg_to_string(ic->result));
            }
            else if (!strcmp(ic->relop->name, ">=")) {
                fprintf(fp, "   jge %s\n", arg_to_string(ic->result));
            }
            else if (!strcmp(ic->relop->name, "<=")) {
                fprintf(fp, "   jle %s\n", arg_to_string(ic->result));
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
