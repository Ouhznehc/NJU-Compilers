%{
    #include "lex.yy.c"
    #include "grammar.h"

    syntax_t *root;
    extern int error_no;

    void yyerror(char const *msg){
        error_no = 1;
        printf("Error type B at line %d: %s.\n", yylineno, msg);
    }
%}

%union {
    union syntax_t* syntax;
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

/*High-level Definitions*/
Program : ExtDefList { $$ = new_symbol("Program", @$.first_line, 1, $1); root = $$; }
	;
ExtDefList : ExtDef ExtDefList { $$ = new_symbol("ExtDefList", @$.first_line, 2, $1, $2); }
	| /*empty*/ { $$ = NULL; }
	| error RC {}
	;
ExtDef : Specifier ExtDecList SEMI { $$ = new_symbol("ExtDef", @$.first_line, 3, $1, $2, $3); }
	| Specifier SEMI { $$ = new_symbol("ExtDef", @$.first_line, 2, $1, $2); }
	| Specifier FunDec CompSt { $$ = new_symbol("ExtDef", @$.first_line, 3, $1, $2, $3); }
	| Specifier error LC {}
	;
ExtDecList : VarDec { $$ = new_symbol("ExtDecList", @$.first_line, 1, $1); }
	| VarDec COMMA ExtDecList { $$ = new_symbol("ExtDecList", @$.first_line, 3, $1, $2, $3); }
	;


/*Specifiers*/
Specifier : TYPE { $$ = new_symbol("Specifier", @$.first_line, 1, $1); }
	| StructSpecifier { $$ = new_symbol("Specifier", @$.first_line, 1, $1); }
	;
StructSpecifier : STRUCT OptTag LC DefList RC { $$ = new_symbol("StructSpecifier", @$.first_line, 5, $1, $2, $3, $4, $5); }
	| STRUCT Tag { $$ = new_symbol("StructSpecifier", @$.first_line, 2, $1, $2); }
	| STRUCT error RC {}
	;
OptTag : ID { $$ = new_symbol("OptTag", @$.first_line, 1, $1); }
	| /*empty*/ { $$ = NULL; }
	;
Tag : ID { $$ = new_symbol("Tag", @$.first_line, 1, $1); }
	;


/*Declarators*/
VarDec : ID { $$ = new_symbol("VarDec", @$.first_line, 1, $1); }
	| VarDec LB INT RB { $$ = new_symbol("VarDec", @$.first_line, 4, $1, $2, $3, $4); }
	| error RB {}
	;
FunDec : ID LP VarList RP { $$ = new_symbol("FunDec", @$.first_line, 4, $1, $2, $3, $4); }
	| ID LP RP { $$ = new_symbol("FunDec", @$.first_line, 3, $1, $2, $3); }
	| error RP {}
	;
VarList : ParamDec COMMA VarList { $$ = new_symbol("VarList", @$.first_line, 3, $1, $2, $3); }
	| ParamDec { $$ = new_symbol("VarList", @$.first_line, 1, $1); }
	| error COMMA {}
	;
ParamDec : Specifier VarDec { $$ = new_symbol("ParamDec", @$.first_line, 2, $1, $2); }
	;


/*Statements*/
CompSt : LC DefList StmtList RC { $$ = new_symbol("CompSt", @$.first_line, 4, $1, $2, $3, $4); }
	| error LC {}
	;
StmtList : Stmt StmtList  { $$ = new_symbol("StmtList", @$.first_line, 2, $1, $2); }
	| /*empty*/ { $$ = NULL; }
	;
Stmt : Exp SEMI  { $$ = new_symbol("Stmt", @$.first_line, 2, $1, $2); }
	| CompSt  { $$ = new_symbol("Stmt", @$.first_line, 1, $1); }
	| RETURN Exp SEMI  { $$ = new_symbol("Stmt", @$.first_line, 3, $1, $2, $3); }
	| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE { $$ = new_symbol("Stmt", @$.first_line, 5, $1, $2, $3, $4, $5); }
	| IF LP Exp RP Stmt ELSE Stmt { $$ = new_symbol("Stmt", @$.first_line, 7, $1, $2, $3, $4, $5, $6, $7); }
	| WHILE LP Exp RP Stmt  { $$ = new_symbol("Stmt", @$.first_line, 5, $1, $2, $3, $4, $5); }
	| error SEMI {}
	| Exp error SEMI {}
	;


/*Local Definitions*/
DefList : Def DefList  { $$ = new_symbol("DefList", @$.first_line, 2, $1, $2); }
	| /*empty*/{ $$ = NULL; }
	;
Def : Specifier DecList SEMI { $$ = new_symbol("Def", @$.first_line, 3, $1, $2, $3); }
	| Specifier error SEMI {}
	;
DecList : Dec { $$ = new_symbol("DecList", @$.first_line, 1, $1); } 
	| Dec COMMA DecList { $$ = new_symbol("DecList", @$.first_line, 3, $1, $2, $3); } 
	| error COMMA {}
	;
Dec : VarDec { $$ = new_symbol("Dec", @$.first_line, 1, $1); } 
	| VarDec ASSIGNOP Exp { $$ = new_symbol("Dec", @$.first_line, 3, $1, $2, $3); }
	| error RP {}
	;


/*Expressions*/
Exp : Exp ASSIGNOP Exp  { $$ = new_symbol("Exp", @$.first_line, 3, $1, $2, $3); }
	| Exp AND Exp  { $$ = new_symbol("Exp", @$.first_line, 3, $1, $2, $3); }
	| Exp OR Exp  { $$ = new_symbol("Exp", @$.first_line, 3, $1, $2, $3); }
	| Exp RELOP Exp { $$ = new_symbol("Exp", @$.first_line, 3, $1, $2, $3); }
	| Exp PLUS Exp  { $$ = new_symbol("Exp", @$.first_line, 3, $1, $2, $3); }
	| Exp MINUS Exp  { $$ = new_symbol("Exp", @$.first_line, 3, $1, $2, $3); }
	| Exp STAR Exp  { $$ = new_symbol("Exp", @$.first_line, 3, $1, $2, $3); }
	| Exp DIV Exp  { $$ = new_symbol("Exp", @$.first_line, 3, $1, $2, $3); }
	| LP Exp RP { $$ = new_symbol("Exp", @$.first_line, 3, $1, $2, $3); }
	| MINUS Exp { $$ = new_symbol("Exp", @$.first_line, 2, $1, $2); }
	| NOT Exp  { $$ = new_symbol("Exp", @$.first_line, 2, $1, $2); }
	| ID LP Args RP { $$ = new_symbol("Exp", @$.first_line, 4, $1, $2, $3, $4); }
	| ID LP RP { $$ = new_symbol("Exp", @$.first_line, 3, $1, $2, $3); }
	| Exp LB Exp RB { $$ = new_symbol("Exp", @$.first_line, 4, $1, $2, $3, $4); }
	| Exp DOT ID { $$ = new_symbol("Exp", @$.first_line, 3, $1, $2, $3); }
	| ID  { $$ = new_symbol("Exp", @$.first_line, 1, $1); }
	| INT  { $$ = new_symbol("Exp", @$.first_line, 1, $1); }
	| FLOAT  { $$ = new_symbol("Exp", @$.first_line, 1, $1); }
	| Exp LB error RB {}
	| error RP {}
	;
Args : Exp COMMA Args { $$ = new_symbol("Args", @$.first_line, 3, $1, $2, $3); }
	| Exp { $$ = new_symbol("Args", @$.first_line, 1, $1); } 
	;

%%

