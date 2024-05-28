#include <stdio.h>
#include "syntax.tab.h"
#include "grammar.h"
#include "semantics.h"
#include "ir.h"
#include "mips.h"

extern FILE* yyin;
extern syntax_t* root;
int error_no = 0;

int main(int argc, char **argv) {   
    if (argc <= 1) return 1;
    if (argc != 4) {
        printf("Usage: ./parser <source.cmm> <output.asm> <output.ir>\n");
        return 1;
    }
    FILE* src_file = fopen(argv[1], "r");
    if (!src_file){
        perror(argv[1]);
        return 1;
    }
    FILE* asm_file = fopen(argv[2], "w");
    if (!asm_file) {
        perror(argv[2]);
        return 1;
    }
    FILE* ir_file = fopen(argv[3], "w");
    if (!ir_file) {
        perror(argv[2]);
        return 1;
    }


    // lab1
    yyrestart(src_file);
    yyparse();
    // if(!error_no) print_syntax_tree(root, 0);

    // lab2
    if (!error_no) semantic_check(root);

    // lab3
    if (!error_no) generate_ir(root, ir_file);

    // if (!error_no) generate_asm(asm_file);

    return 0;
}
