%option yylineno

%{
    #include "syntax.tab.h"
    #include "grammar.h"
    extern int error_no;

    void report_error(const char* msg);
    value_t new_value(char* name, int type);

    int yycolumn = 1;
    #define YY_USER_ACTION \
        yylloc.first_line = yylloc.last_line = yylineno; \
        yylloc.first_column = yycolumn; \
        yylloc.last_column = yycolumn + yyleng - 1; \
        yycolumn += yyleng;

%}

/*Tokens*/
SEMI                ;
COMMA               ,
ASSIGNOP            =
RELOP               <|>|<=|>=|==|!=
PLUS                \+
MINUS               -
STAR                \*
DIV                 \/
AND                 &&
OR                  \|\|
DOT                 \.
NOT                 !
TYPE                int|float
LP                  \(
RP                  \)
LB                  \[
RB                  \]
LC                  \{
RC                  \}
STRUCT              struct
RETURN              return
IF                  if
ELSE                else
WHILE               while

DIGIT               [0-9]
LETTER              [a-zA-Z]
LETTER_             {LETTER}|_
CHAR                {LETTER_}|{DIGIT}
DELIM               [ \r\n\t]
WS                  {DELIM}+
NEW_LINE            \n

DEC_INT             [1-9][0-9]*|0
OCT_INT             0[0-7]+
HEX_INT             0[xX][0-9a-fA-F]+

FLOAT_NRM           {DEC_INT}{DOT}{DIGIT}+
FLOAT_SCI           ({DIGIT}*{DOT}{DIGIT}+|{DIGIT}+{DOT})[eE][+-]?{DIGIT}+
FLOAT               {FLOAT_NRM}|{FLOAT_SCI}

ID                  {LETTER_}{CHAR}*

COMMENT_LINE        \/\/
COMMENT_BLOCK       \/\*

%%
{NEW_LINE}          { yycolumn = 1; }
{SEMI}              { value_t value = new_value(yytext, TOKEN_CONST); yylval.syntax = new_token("SEMI", TOKEN_CONST, yylineno, value); return SEMI; }
{COMMA}             { value_t value = new_value(yytext, TOKEN_CONST); yylval.syntax = new_token("COMMA", TOKEN_CONST, yylineno, value); return COMMA; }
{ASSIGNOP}          { value_t value = new_value(yytext, TOKEN_CONST); yylval.syntax = new_token("ASSIGNOP", TOKEN_CONST, yylineno, value); return ASSIGNOP; }
{RELOP}             { value_t value = new_value(yytext, TOKEN_CONST); yylval.syntax = new_token("RELOP", TOKEN_CONST, yylineno, value); return RELOP; }
{PLUS}              { value_t value = new_value(yytext, TOKEN_CONST); yylval.syntax = new_token("PLUS", TOKEN_CONST, yylineno, value); return PLUS; }
{MINUS}             { value_t value = new_value(yytext, TOKEN_CONST); yylval.syntax = new_token("MINUS", TOKEN_CONST, yylineno, value); return MINUS; }
{STAR}              { value_t value = new_value(yytext, TOKEN_CONST); yylval.syntax = new_token("STAR", TOKEN_CONST, yylineno, value); return STAR; }
{DIV}               { value_t value = new_value(yytext, TOKEN_CONST); yylval.syntax = new_token("DIV", TOKEN_CONST, yylineno, value); return DIV; }
{AND}               { value_t value = new_value(yytext, TOKEN_CONST); yylval.syntax = new_token("AND", TOKEN_CONST, yylineno, value); return AND; }
{OR}                { value_t value = new_value(yytext, TOKEN_CONST); yylval.syntax = new_token("OR", TOKEN_CONST, yylineno, value); return OR; }
{DOT}               { value_t value = new_value(yytext, TOKEN_CONST); yylval.syntax = new_token("DOT", TOKEN_CONST, yylineno, value); return DOT; }
{NOT}               { value_t value = new_value(yytext, TOKEN_CONST); yylval.syntax = new_token("NOT", TOKEN_CONST, yylineno, value); return NOT; }

{TYPE}              { value_t value = new_value(yytext, TOKEN_TYPE); yylval.syntax = new_token("TYPE", TOKEN_TYPE, yylineno, value); return TYPE; }

{LP}                { value_t value = new_value(yytext, TOKEN_CONST); yylval.syntax = new_token("LP", TOKEN_CONST, yylineno, value); return LP; }
{RP}                { value_t value = new_value(yytext, TOKEN_CONST); yylval.syntax = new_token("RP", TOKEN_CONST, yylineno, value); return RP; }
{LB}                { value_t value = new_value(yytext, TOKEN_CONST); yylval.syntax = new_token("LB", TOKEN_CONST, yylineno, value); return LB; }
{RB}                { value_t value = new_value(yytext, TOKEN_CONST); yylval.syntax = new_token("RB", TOKEN_CONST, yylineno, value); return RB; }
{LC}                { value_t value = new_value(yytext, TOKEN_CONST); yylval.syntax = new_token("LC", TOKEN_CONST, yylineno, value); return LC; }
{RC}                { value_t value = new_value(yytext, TOKEN_CONST); yylval.syntax = new_token("RC", TOKEN_CONST, yylineno, value); return RC; }
{STRUCT}            { value_t value = new_value(yytext, TOKEN_CONST); yylval.syntax = new_token("STRUCT", TOKEN_CONST, yylineno, value); return STRUCT; }
{RETURN}            { value_t value = new_value(yytext, TOKEN_CONST); yylval.syntax = new_token("RETURN", TOKEN_CONST, yylineno, value); return RETURN; }
{IF}                { value_t value = new_value(yytext, TOKEN_CONST); yylval.syntax = new_token("IF", TOKEN_CONST, yylineno, value); return IF; }
{ELSE}              { value_t value = new_value(yytext, TOKEN_CONST); yylval.syntax = new_token("ELSE", TOKEN_CONST, yylineno, value); return ELSE; }
{WHILE}             { value_t value = new_value(yytext, TOKEN_CONST); yylval.syntax = new_token("WHILE", TOKEN_CONST, yylineno, value); return WHILE; }
{WS}                { /* no action and no return */ }

{ID}                { value_t value = new_value(yytext, TOKEN_ID); yylval.syntax = new_token("ID", TOKEN_ID, yylineno, value); return ID; }

{DEC_INT}           { value_t value = new_value(yytext, TOKEN_INT_DEC); yylval.syntax = new_token("INT", TOKEN_INT, yylineno, value); return INT; }
{OCT_INT}           { value_t value = new_value(yytext, TOKEN_INT_OCT); yylval.syntax = new_token("INT", TOKEN_INT, yylineno, value); return INT; }
{HEX_INT}           { value_t value = new_value(yytext, TOKEN_INT_HEX); yylval.syntax = new_token("INT", TOKEN_INT, yylineno, value); return INT; }

{FLOAT}             { value_t value = new_value(yytext, TOKEN_FLOAT); yylval.syntax = new_token("FLOAT", TOKEN_FLOAT, yylineno, value); return FLOAT; }

{COMMENT_LINE}      { char c = input(); while (c != '\n') c = input(); }
{COMMENT_BLOCK}     { 
                        char curr = input(), prev = curr; 
                        while (prev != '*' || curr != '/') {
                            prev = curr; curr = input();
                            if (!curr || curr == EOF)
                                report_error("Invalid comment");
                                break;
                        } 
                    }

.                   { report_error("Mysterious character"); }
%%

void report_error(const char* msg) {
    error_no = 1;
    printf("Error type A at line %d: { %s, \'%s\' }\n", yylineno, msg, yytext);
}


value_t new_value(char* name, int type) {
    value_t ret;
    switch (type) {
        case TOKEN_INT_DEC:
            ret.ival = strtol(name, NULL, 10);
            break;
        case TOKEN_INT_OCT:
            ret.ival = strtol(name, NULL, 8);
            break;
        case TOKEN_INT_HEX:
            ret.ival = strtol(name, NULL, 16);
            break;
        case TOKEN_FLOAT:
            ret.fval = strtof(name, NULL); 
            break;
        case TOKEN_TYPE:
        case TOKEN_ID:
            strcpy(ret.sval, name);
            break;
        default:
            memset(&ret, 0, sizeof(ret)); 
            break;
    }
    return ret;
}