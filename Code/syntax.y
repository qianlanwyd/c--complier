%{
#include <stdio.h>
#include "lex.yy.c"
#include "tree.h"

int yyerror(const char *msg){
	if_print=0;
	fprintf(stderr,"Error type B at Line %d: Column %d: %s\n",yylineno,yycolumn,msg);
}
%}

%union{
	int type_int;
	float type_float;
	double type_double;
	struct Node* node;
}
%locations
%token <node>INT
%token <node>FLOAT
%token <node>ADD SUB MUL DIV NOT TYPE LP RP LB RB LC RC STRUCT RETURN IF ELSE WHILE ID SEMI COMMA ASSIGNOP RELOP AND OR DOT

%type <node>  Program ExtDefList ExtDef ExtDecList Specifier VarDec
%type <node> StructSpecifier OptTag DefList Tag
%type <node> FunDec ParamDec VarList
%type <node> CompSt StmtList Stmt Exp
%type <node> Def DecList Dec
%type <node> Args



%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left ADD SUB
%left MUL DIV
%right NOT
%left LP RP LB RB DOT

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%error-verbose



%%
Program :ExtDefList{$$ =insert("Program",yytext,0,$1->line,1,$1);root=$$;}
	;
ExtDefList : ExtDef ExtDefList{$$ =insert("ExtDefList",yytext,0,$1->line,2,$1,$2);}
	|/* empty */{$$=NULL;}
	;
ExtDef :Specifier ExtDecList SEMI{$$ =insert("ExtDef",yytext,0,$1->line,3,$1,$2,$3);}
	|Specifier SEMI{$$ =insert("ExtDef",yytext,0,$1->line,2,$1,$2);}
	|Specifier FunDec CompSt{$$ =insert("ExtDef",yytext,0,$1->line,3,$1,$2,$3);} 
	;
ExtDecList :VarDec{$$ =insert("ExtDecList",yytext,0,$1->line,1,$1);}
	|VarDec COMMA ExtDecList{$$ =insert("ExtDecList",yytext,0,$1->line,3,$1,$2,$3);}
	;

Specifier :TYPE{$$ =insert("Specifier",yytext,0,$1->line,1,$1);}
	|StructSpecifier{$$ =insert("Specifier",yytext,0,$1->line,1,$1);}
	;
StructSpecifier :STRUCT OptTag LC DefList RC{$$ =insert("StructSpecifier",yytext,0,$1->line,5,$1,$2,$3,$4,$5);}
	|STRUCT Tag{$$ =insert("StructSpecifier",yytext,0,$1->line,2,$1,$2);}
	;
OptTag :ID{$$ =insert("OptTag",yytext,0,$1->line,1,$1);}
	|/* empty */{$$=NULL;}
	;
Tag :ID{$$ =insert("Tag",yytext,0,$1->line,1,$1);}
	;


VarDec :ID{$$ =insert("VarDec",yytext,0,$1->line,1,$1);}
	|VarDec LB INT RB{$$ =insert("VarDec",yytext,0,$1->line,4,$1,$2,$3,$4);}
	|VarDec LB error RB{yyerrok;}
	;
FunDec :ID LP VarList RP{$$ =insert("FunDec",yytext,0,$1->line,4,$1,$2,$3,$4);}
	|ID LP RP{$$ =insert("FunDec",yytext,0,$1->line,3,$1,$2,$3);}
	;
VarList :ParamDec COMMA VarList{$$ =insert("VarList",yytext,0,$1->line,3,$1,$2,$3);}
	|ParamDec{$$ =insert("VarList",yytext,0,$1->line,1,$1);}
	;
ParamDec :Specifier VarDec{$$ =insert("ParamDec",yytext,0,$1->line,2,$1,$2);}
	;


CompSt :LC DefList StmtList RC{$$ =insert("CompSt",yytext,0,$1->line,4,$1,$2,$3,$4);}
	;
StmtList :Stmt StmtList{$$ =insert("StmtList",yytext,0,$1->line,2,$1,$2);}
	|/* empty */{$$=NULL;}
	;
Stmt :Exp SEMI{$$ =insert("Stmt",yytext,0,$1->line,2,$1,$2);}
	|CompSt{$$ =insert("Stmt",yytext,0,$1->line,1,$1);}
	|RETURN Exp SEMI{$$ =insert("Stmt",yytext,0,$1->line,3,$1,$2,$3);}
	|IF LP Exp RP Stmt %prec LOWER_THAN_ELSE{$$ =insert("Stmt",yytext,0,$1->line,5,$1,$2,$3,$4,$5);}
	|IF LP Exp RP Stmt ELSE Stmt{$$ =insert("Stmt",yytext,0,$1->line,7,$1,$2,$3,$4,$5,$6,$7);}
	|WHILE LP Exp RP Stmt{$$ =insert("Stmt",yytext,0,$1->line,5,$1,$2,$3,$4,$5);}
	;

DefList :Def DefList{$$ =insert("DefList",yytext,0,$1->line,2,$1,$2);}
	|/* empty */{$$=NULL;}
	;
Def :Specifier DecList SEMI{$$ =insert("Def",yytext,0,$1->line,3,$1,$2,$3);}
	|error SEMI{yyerrok;}
	;
DecList :Dec{$$ =insert("DecList",yytext,0,$1->line,1,$1);}
	|Dec COMMA DecList{$$ =insert("DecList",yytext,0,$1->line,3,$1,$2,$3);} 
	;
Dec :VarDec{$$ =insert("Dec",yytext,0,$1->line,1,$1);}
	|VarDec ASSIGNOP Exp{$$ =insert("Dec",yytext,0,$1->line,3,$1,$2,$3);}
	;

Exp :Exp ASSIGNOP Exp{$$ =insert("Exp",yytext,0,$1->line,3,$1,$2,$3);}
	|Exp AND Exp{$$ =insert("Exp",yytext,0,$1->line,3,$1,$2,$3);}
	|Exp OR Exp{$$ =insert("Exp",yytext,0,$1->line,3,$1,$2,$3);}
	|Exp RELOP Exp{$$ =insert("Exp",yytext,0,$1->line,3,$1,$2,$3);}
	|Exp ADD Exp{$$ =insert("Exp",yytext,0,$1->line,3,$1,$2,$3);}
	|Exp SUB Exp{$$ =insert("Exp",yytext,0,$1->line,3,$1,$2,$3);}
	|Exp MUL Exp{$$ =insert("Exp",yytext,0,$1->line,3,$1,$2,$3);}
	|Exp DIV Exp{$$ =insert("Exp",yytext,0,$1->line,3,$1,$2,$3);}
	|LP Exp RP{$$ =insert("Exp",yytext,0,$1->line,3,$1,$2,$3);}
	|SUB Exp{$$ =insert("Exp",yytext,0,$1->line,2,$1,$2);}
	|NOT Exp{$$ =insert("Exp",yytext,0,$1->line,2,$1,$2);}
	|ID LP Args RP{$$ =insert("Exp",yytext,0,$1->line,4,$1,$2,$3,$4);}
	|ID LP RP{$$ =insert("Exp",yytext,0,$1->line,3,$1,$2,$3);}
	|Exp LB Exp RB{$$ =insert("Exp",yytext,0,$1->line,4,$1,$2,$3,$4);}
	|Exp DOT ID{$$ =insert("Exp",yytext,0,$1->line,3,$1,$2,$3);}
	|ID{$$ =insert("Exp",yytext,0,$1->line,1,$1);}
	|INT{$$ =insert("Exp",yytext,0,$1->line,1,$1);}
	|FLOAT{$$ =insert("Exp",yytext,0,$1->line,1,$1);}
	|LP error RP{yyerrok;}
	|RB error RB{yyerrok;} 
	;

Args :Exp COMMA Args{$$ =insert("Args",yytext,0,$1->line,3,$1,$2,$3);}
	|Exp{$$ =insert("Args",yytext,0,$1->line,1,$1);}
	|error COMMA Args{yyerrok;} 
	;

%%



