#include <stdio.h>
#include "syntax.tab.h"
#include "grammar.h"
#include "semantics.h"
extern FILE* yyin;
extern syntax_t* root;
int error_no = 0;

int main(int argc, char **argv) {   
    if (argc <= 1) return 1;
    // if (argc != 3) {
    //     printf("Usage: ./parser <source.cmm> <output.ir>\n");
    //     return 1;
    // }
    FILE* f = fopen(argv[1], "r");
    if (!f){
        perror(argv[1]);
        return 1;
    }
    // FILE* ir = fopen(argv[2], "w");
    // if (!ir) {
    //     perror(argv[2]);
    //     return 1;
    // }

    // lab1
    yyrestart(f);
    yyparse();
    // if(!error_no) print_syntax_tree(root, 0);

    // lab2
    if (!error_no) semantic_check(root);

    // lab3
    // if (!error_no) generate_ir(root, ir);

    return 0;
}
