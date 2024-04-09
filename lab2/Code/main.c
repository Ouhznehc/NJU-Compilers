#include <stdio.h>
#include "syntax.tab.h"
#include "grammar.h"
#include "semantics.h"
extern FILE* yyin;
extern syntax_t* root;
int error_no = 0;

int main(int argc, char **argv) {
    SemanticErrorType error = INITIALIZE_FIELD;
    if(error == DUPLICATE_FIELD) {printf("fuck\n");}
    else {printf("ok\n");}
    return 0;    
    
    if (argc <= 1) return 1;
    FILE* f = fopen(argv[1], "r");
    if (!f){
        perror(argv[1]);
        return 1;
    }
    yyrestart(f);
    yyparse();
    if(!error_no) print_syntax_tree(root, 0);
    return 0;
}