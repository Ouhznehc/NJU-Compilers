%option yylineno

%{
    void report_error(const char* msg, char type);
%}

/*Tokens*/
DIGIT                [0-9]
LETTER               [a-zA-Z]
LETTER_              {LETTER}|_
CHAR                 {LETTER_}|DIGIT
DELIM                [ \r\n\t]



DEC_INT              [1-9][0-9]*|0
OCT_INT              0[0-7]+
HEX_INT              0[xX][0-9a-fA-F]+

ID                   {LETTER_}{CHAR}*
INT                  {DEC_INT}|{OCT_INT}|{HEX_INT}
INVALID_TOKEN        {DIGIT}{CHAR}+

SEMI                 ;
COMMA                ,
ASSIGNOP             =
RELOP                <|>|<=|>=|==|!=
PLUS                 \+
MINUS                -
STAR                 \*
DIV                  \/
AND                  &&
OR                   \|\|
DOT                  \.
NOT                  !
TYPE                 int|float
LP                   \(
RP                   \)
LB                   \[
RB                   \]
LC                   \{
RC                   \}
STRUCT               struct
RETURN               return
IF                   if
ELSE                 else
WHILE                while

%%
{SEMI}               { printf("<';', >\n"); }
{COMMA}              { printf("<',', >\n"); }
{ASSIGNOP}           { printf("<'=', >\n"); }
{RELOP}              { printf("<RELOP, %s>\n", yytext); }
{PLUS}               { printf("<'+', >\n"); }
{MINUS}              { printf("<'-', >\n"); }
{STAR}               { printf("<'*', >\n"); }
{DIV}                { printf("<'/', >\n"); }
{AND}                { printf("<'&&', >\n"); }
{OR}                 { printf("<'||', >\n"); }
{DOT}                { printf("<'.', >\n"); }
{NOT}                { printf("<'!', >\n"); }
{TYPE}               { printf("<TYPE, %s>\n", yytext); }
{LP}                 { printf("<'(', >\n"); }
{RP}                 { printf("<')', >\n"); }
{LB}                 { printf("<'[', >\n"); }
{RB}                 { printf("<']', >\n"); }
{LC}                 { printf("<'{', >\n"); }
{RC}                 { printf("<'}', >\n"); }
{STRUCT}             { printf("<STRUCT, >\n"); }
{RETURN}             { printf("<RETURN, >\n"); }
{IF}                 { printf("<IF, >\n"); }
{ELSE}               { printf("<ELSE, >\n"); }
{WHILE}              { printf("<WHILE, >\n"); }
{DELIM}              { /*pass*/ }

{ID}                 { printf("<ID, %s>\n", yytext); }
{INT}                { printf("<INT, %s>\n", yytext); }
{INVALID_TOKEN}      { report_error("Invalid token", 'A'); }
.                    { report_error("Mysterious character", 'A'); }
%%

void report_error(const char* msg, char type){
    printf("Error type %c at line %d: { %s, \'%s\' }\n", type, yylineno, msg, yytext);
}