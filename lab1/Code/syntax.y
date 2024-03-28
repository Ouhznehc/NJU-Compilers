%locations

%{
    #include "lex.yy.c"
    #include "grammar.h"

    syntax_t *root;
    extern int error_no;
	extern int error_lineno;

	extern int yylineno;

    void yyerror(char const *msg){
		if (yylineno == error_lineno) return;
		error_lineno = yylineno;
        error_no = 1;
        printf("Error type B at line %d: %s.\n", yylineno, msg);
    }
	
%}

%union {
    struct syntax_t* syntax;
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
Program 
	: ExtDefList { 
                        $$ = new_symbol("Program", @$.first_line, 1, $1); root = $$; 
						/* empty file */
						if ($1 == NULL) $$->symbol.lineno = yylineno;
                     }
	| error { $$ = NULL; }
	;
ExtDefList 
	: ExtDef ExtDefList { $$ = new_symbol("ExtDefList", @$.first_line, 2, $1, $2); }
	| /*empty*/ { $$ = NULL; }
	| error { $$ = NULL; }
	;
ExtDef 
	: Specifier ExtDecList SEMI { $$ = new_symbol("ExtDef", @$.first_line, 3, $1, $2, $3); }
	| Specifier SEMI { $$ = new_symbol("ExtDef", @$.first_line, 2, $1, $2); }
	| Specifier FunDec CompSt { $$ = new_symbol("ExtDef", @$.first_line, 3, $1, $2, $3); }
	| error { $$ = NULL; }
	;
ExtDecList 
	: VarDec { $$ = new_symbol("ExtDecList", @$.first_line, 1, $1); }
	| VarDec COMMA ExtDecList { $$ = new_symbol("ExtDecList", @$.first_line, 3, $1, $2, $3); }
	| error { $$ = NULL; }
	;


/*Specifiers*/
Specifier 
	: TYPE { $$ = new_symbol("Specifier", @$.first_line, 1, $1); }
	| StructSpecifier { $$ = new_symbol("Specifier", @$.first_line, 1, $1); }
	| error { $$ = NULL; }
	;
StructSpecifier 
	: STRUCT OptTag LC DefList RC { $$ = new_symbol("StructSpecifier", @$.first_line, 5, $1, $2, $3, $4, $5); }
	| STRUCT Tag { $$ = new_symbol("StructSpecifier", @$.first_line, 2, $1, $2); }
	| error { $$ = NULL; }
	;
OptTag 
	: ID { $$ = new_symbol("OptTag", @$.first_line, 1, $1); }
	| /*empty*/ { $$ = NULL; }
	| error { $$ = NULL; }
	;
Tag 
	: ID { $$ = new_symbol("Tag", @$.first_line, 1, $1); }
	;


/*Declarators*/
VarDec 
	: ID { $$ = new_symbol("VarDec", @$.first_line, 1, $1); }
	| VarDec LB INT RB { $$ = new_symbol("VarDec", @$.first_line, 4, $1, $2, $3, $4); }
	| error { $$ = NULL; }
	;
FunDec 
	: ID LP VarList RP { $$ = new_symbol("FunDec", @$.first_line, 4, $1, $2, $3, $4); }
	| ID LP RP { $$ = new_symbol("FunDec", @$.first_line, 3, $1, $2, $3); }
	| error { $$ = NULL; }
	;
VarList 
	: ParamDec COMMA VarList { $$ = new_symbol("VarList", @$.first_line, 3, $1, $2, $3); }
	| ParamDec { $$ = new_symbol("VarList", @$.first_line, 1, $1); }
	| error { $$ = NULL; }
	;
ParamDec 
	: Specifier VarDec { $$ = new_symbol("ParamDec", @$.first_line, 2, $1, $2); }
	| error { $$ = NULL; }
	;


/*Statements*/
CompSt 
	: LC DefList StmtList RC { $$ = new_symbol("CompSt", @$.first_line, 4, $1, $2, $3, $4); }
	| error { $$ = NULL; }
	;
StmtList 
	: Stmt StmtList  { $$ = new_symbol("StmtList", @$.first_line, 2, $1, $2); }
	| /*empty*/ { $$ = NULL; }
	| error { $$ = NULL; }
	;
Stmt 
	: Exp SEMI  { $$ = new_symbol("Stmt", @$.first_line, 2, $1, $2); }
	| CompSt  { $$ = new_symbol("Stmt", @$.first_line, 1, $1); }
	| RETURN Exp SEMI  { $$ = new_symbol("Stmt", @$.first_line, 3, $1, $2, $3); }
	| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE { $$ = new_symbol("Stmt", @$.first_line, 5, $1, $2, $3, $4, $5); }
	| IF LP Exp RP Stmt ELSE Stmt { $$ = new_symbol("Stmt", @$.first_line, 7, $1, $2, $3, $4, $5, $6, $7); }
	| WHILE LP Exp RP Stmt  { $$ = new_symbol("Stmt", @$.first_line, 5, $1, $2, $3, $4, $5); }
	| error { $$ = NULL; }
	;


/*Local Definitions*/
DefList 
	: Def DefList  { $$ = new_symbol("DefList", @$.first_line, 2, $1, $2); }
	| /*empty*/{ $$ = NULL; }
	| error { $$ = NULL; }
	;
Def 
	: Specifier DecList SEMI { $$ = new_symbol("Def", @$.first_line, 3, $1, $2, $3); }
	| error { $$ = NULL; }
	;
DecList 
	: Dec { $$ = new_symbol("DecList", @$.first_line, 1, $1); } 
	| Dec COMMA DecList { $$ = new_symbol("DecList", @$.first_line, 3, $1, $2, $3); } 
	| error { $$ = NULL; }
	;
Dec 
	: VarDec { $$ = new_symbol("Dec", @$.first_line, 1, $1); } 
	| VarDec ASSIGNOP Exp { $$ = new_symbol("Dec", @$.first_line, 3, $1, $2, $3); }
	| error { $$ = NULL; }
	;


/*Expressions*/
Exp 
	: Exp ASSIGNOP Exp  { $$ = new_symbol("Exp", @$.first_line, 3, $1, $2, $3); }
	| Exp AND Exp  { $$ = new_symbol("Exp", @$.first_line, 3, $1, $2, $3); }
	| Exp OR Exp  { $$ = new_symbol("Exp", @$.first_line, 3, $1, $2, $3); }
	| Exp RELOP Exp { $$ = new_symbol("Exp", @$.first_line, 3, $1, $2, $3); }
	| Exp PLUS Exp  { $$ = new_symbol("Exp", @$.first_line, 3, $1, $2, $3); }
	| Exp MINUS Exp  { $$ = new_symbol("Exp", @$.first_line, 3, $1, $2, $3); }
	| Exp STAR Exp  { $$ = new_symbol("Exp", @$.first_line, 3, $1, $2, $3); }
	| Exp DIV Exp  { $$ = new_symbol("Exp", @$.first_line, 3, $1, $2, $3); }
	| LP Exp RP { $$ = new_symbol("Exp", @$.first_line, 3, $1, $2, $3); }
	| MINUS Exp %prec NOT { $$ = new_symbol("Exp", @$.first_line, 2, $1, $2); }
	| NOT Exp  { $$ = new_symbol("Exp", @$.first_line, 2, $1, $2); }
	| ID LP Args RP { $$ = new_symbol("Exp", @$.first_line, 4, $1, $2, $3, $4); }
	| ID LP RP { $$ = new_symbol("Exp", @$.first_line, 3, $1, $2, $3); }
	| Exp LB Exp RB { $$ = new_symbol("Exp", @$.first_line, 4, $1, $2, $3, $4); }
	| Exp DOT ID { $$ = new_symbol("Exp", @$.first_line, 3, $1, $2, $3); }
	| ID  { $$ = new_symbol("Exp", @$.first_line, 1, $1); }
	| INT  { $$ = new_symbol("Exp", @$.first_line, 1, $1); }
	| FLOAT  { $$ = new_symbol("Exp", @$.first_line, 1, $1); }
	| error { $$ = NULL; }
	;
Args 
	: Exp COMMA Args { $$ = new_symbol("Args", @$.first_line, 3, $1, $2, $3); }
	| Exp { $$ = new_symbol("Args", @$.first_line, 1, $1); } 
	| error { $$ = NULL; }
	;

%%

