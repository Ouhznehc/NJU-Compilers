%{
    #include "lex.yy.c"
    #include "syntax.h"
%}

%union {
    syntax_t* syntax;
}


/* Tokens */
%token <syntax> SEMI
%token <syntax> COMMA
%token <syntax> ASSIGNOP
%token <syntax> RELOP
%token <syntax> PLUS
%token <syntax> MINUS
%token <syntax> STAR
%token <syntax> DIV
%token <syntax> AND
%token <syntax> OR
%token <syntax> DOT 
%token <syntax> NOT 
%token <syntax> TYPE
%token <syntax> LP
%token <syntax> RP
%token <syntax> LB
%token <syntax> RB
%token <syntax> LC
%token <syntax> RC
%token <syntax> STRUCT
%token <syntax> RETURN
%token <syntax> IF
%token <syntax> ELSE
%token <syntax> WHILE

%token <syntax> ID
%token <syntax> FLOAT
%token <syntax> INT

/* High-level Definitions */
%type <syntax> Program 
%type <syntax> ExtDefList 
%type <syntax> ExtDef 
%type <syntax> ExtDecList

/* Specifiers */
%type <syntax> Specifier 
%type <syntax> StructSpecifier 
%type <syntax> OptTag 
%type <syntax> Tag

/* Declarators */
%type <syntax> VarDec 
%type <syntax> FunDec 
%type <syntax> VarList 
%type <syntax> ParamDec

/* Statements */
%type <syntax> CompSt 
%type <syntax> StmtList 
%type <syntax> Stmt            

/* Local Definitions */
%type <syntax> DefList 
%type <syntax> Def 
%type <syntax> Dec 
%type <syntax> DecList

/* Expressions */
%type <syntax> Exp 
%type <syntax> Args

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%right ASSIGNOP
%left  OR
%left  AND
%left  RELOP
%left  PLUS MINUS
%left  STAR DIV
%right NOT
%left  DOT LB RB LP RP

%%



%%

