%option yylineno

%{
    void report_error(const char* msg, char type);
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

DEC_INT             [1-9][0-9]*|0
OCT_INT             0[0-7]+
HEX_INT             0[xX][0-9a-fA-F]+

FLOAT_NRM           {DEC_INT}{DOT}{DIGIT}+
FLOAT_SCI           ({DIGIT}*{DOT}{DIGIT}+|{DIGIT}+{DOT})[eE][+-]?{DIGIT}+
FLOAT               {FLOAT_NRM}|{FLOAT_SCI}


ID                  {LETTER_}{CHAR}*
INT                 {DEC_INT}|{OCT_INT}|{HEX_INT}
INVALID_TOKEN       {DIGIT}{CHAR}+

COMMENT_LINE        \/\/
COMMENT_BLOCK       \/\*

%%
{SEMI}              { printf("<';', >\n"); }
{COMMA}             { printf("<',', >\n"); }
{ASSIGNOP}          { printf("<'=', >\n"); }
{RELOP}             { printf("<RELOP, %s>\n", yytext); }
{PLUS}              { printf("<'+', >\n"); }
{MINUS}             { printf("<'-', >\n"); }
{STAR}              { printf("<'*', >\n"); }
{DIV}               { printf("<'/', >\n"); }
{AND}               { printf("<'&&', >\n"); }
{OR}                { printf("<'||', >\n"); }
{DOT}               { printf("<'.', >\n"); }
{NOT}               { printf("<'!', >\n"); }
{TYPE}              { printf("<TYPE, %s>\n", yytext); }
{LP}                { printf("<'(', >\n"); }
{RP}                { printf("<')', >\n"); }
{LB}                { printf("<'[', >\n"); }
{RB}                { printf("<']', >\n"); }
{LC}                { printf("<'{', >\n"); }
{RC}                { printf("<'}', >\n"); }
{STRUCT}            { printf("<STRUCT, >\n"); }
{RETURN}            { printf("<RETURN, >\n"); }
{IF}                { printf("<IF, >\n"); }
{ELSE}              { printf("<ELSE, >\n"); }
{WHILE}             { printf("<WHILE, >\n"); }
{WS}                { /* no action and no return */ }

{ID}                { printf("<ID, %s>\n", yytext); }
{INT}               { printf("<INT, %s>\n", yytext); }
{FLOAT}             { printf("<FLOAT, %s>\n", yytext); }
{INVALID_TOKEN}     { report_error("Invalid token", 'A'); }

{COMMENT_LINE}      { char c = input(); while (c != '\n') c = input(); }
{COMMENT_BLOCK}     { 
                        char curr = input(), prev = curr; 
                        while (prev != '*' || curr != '/') {
                            prev = curr; curr = input();
                            if (!curr || curr == EOF) {
                                report_error("Invalid comment", 'A');
                                yyterminate();
                            }
                        } 
                    }

.                   { report_error("Mysterious character", 'A'); }
%%

void report_error(const char* msg, char type){
    printf("Error type %c at line %d: { %s, \'%s\' }\n", type, yylineno, msg, yytext);
}