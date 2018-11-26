
%{
	#include "syntax.tab.h"
	#include "tree.h"
	int columnno=0;
	int lineno=1;
	int yycolumn=0;
	#define YY_USER_ACTION \
		yylloc.first_line = yylloc.last_line = yylineno; \
		yylloc.first_column = yycolumn; \
		yylloc.last_column = yycolumn + yyleng - 1; \
		yycolumn += yyleng;
%}
%option yylineno

LINEBREAK	\n
TAB		\t
ENTER		\r
SPACE		" "	
DIGIT		[0-9]
LETTER 		[_a-zA-Z]
DECINT		0|([1-9][0-9]*)
OCTINT		0(0|[1-7][0-7]*)
HEXINT		0[xX](0|[1-9a-fA-F][0-9a-fA-F]*)
ID		{LETTER}({LETTER}|{DIGIT})*
FLOAT		{DIGIT}+\.{DIGIT}+
INT		{DECINT}|{OCTINT}|{HEXINT}
SEMI		";"
COMMA		","
ASSIGNOP 	"="
RELOP		">"|"<"|">="|"<="|"=="|"!="
ADD		"+"
SUB		"-"
MUL		"*"
DIV		"/"
AND		"&&"
OR		"||"
DOT		"."
NOT		"!"
TYPE		"int"|"float"
LP 		"("
RP 		")"
LB 		"["
RB 		"]"
LC 		"{"
RC 		"}"
STRUCT 		"struct"
RETURN 		"return"
IF 		"if"
ELSE 		"else"
WHILE 		"while"	
	


%%

{TAB}|{ENTER}|{SPACE}	{} 
{LINEBREAK}		{lineno++;yycolumn=1;}
{STRUCT}		{yylval.node=insert("STRUCT",yytext,1,yylineno,0);return STRUCT;}
{RETURN}		{yylval.node=insert("RETURN",yytext,1,yylineno,0);return RETURN;}
{IF}			{yylval.node=insert("IF",yytext,1,yylineno,0);return IF;}
{ELSE}			{yylval.node=insert("ELSE",yytext,1,yylineno,0);return ELSE;}
{WHILE}			{yylval.node=insert("WHILE",yytext,1,yylineno,0);return WHILE;} 
{TYPE}			{yylval.node=insert("TYPE",yytext,2,yylineno,0);return TYPE;}
{FLOAT}			{yylval.node=insert("FLOAT",yytext,5,yylineno,0);return FLOAT;}
{INT}			{yylval.node=insert("INT",yytext,4,yylineno,0);return INT;}
{ID}			{yylval.node=insert("ID",yytext,3,yylineno,0);return ID;}
{SEMI}			{yylval.node=insert("SEMI",yytext,1,yylineno,0);return SEMI;}
{COMMA}			{yylval.node=insert("COMMA",yytext,1,yylineno,0);return COMMA;}
{RELOP}			{yylval.node=insert("RELOP",yytext,1,yylineno,0);return RELOP;}
{ASSIGNOP}		{yylval.node=insert("ASSIGNOP",yytext,1,yylineno,0);return ASSIGNOP;}
{ADD}			{yylval.node=insert("PLUS",yytext,1,yylineno,0);return ADD;}
{SUB}			{yylval.node=insert("MINUS",yytext,1,yylineno,0);return SUB;}
{MUL}			{yylval.node=insert("STAR",yytext,1,yylineno,0);return MUL;}
{DIV}			{yylval.node=insert("DIV",yytext,1,yylineno,0);return DIV;}
{AND}			{yylval.node=insert("AND",yytext,1,yylineno,0);return AND;}
{OR}			{yylval.node=insert("OR",yytext,1,yylineno,0);return OR;}
{DOT}			{yylval.node=insert("DOT",yytext,1,yylineno,0);return DOT;}
{NOT}			{yylval.node=insert("NOT",yytext,1,yylineno,0);return NOT;}
{LP}			{yylval.node=insert("LP",yytext,1,yylineno,0);return LP;}
{RP}			{yylval.node=insert("RP",yytext,1,yylineno,0);return RP;}
{LB}			{yylval.node=insert("LB",yytext,1,yylineno,0);return LB;}
{RB}			{yylval.node=insert("RB",yytext,1,yylineno,0);return RB;}
{LC}			{yylval.node=insert("LC",yytext,1,yylineno,0);return LC;}
{RC}			{yylval.node=insert("RC",yytext,1,yylineno,0);return RC;}

.		{if_print=0;printf("Error type A at Line %d Column %d: Mysterious character \'%s\'.\n", lineno, yycolumn, yytext);}
%%


