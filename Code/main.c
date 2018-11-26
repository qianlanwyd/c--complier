#include  <stdio.h>
#include  "tree.h"
#include  "sdt.h"
#include  "syntax.tab.h"
#include  "ir.h"
extern int yyerror(char *msg);
extern int yyparse();
extern void yyrestart(FILE* f);
extern int yylineno;

int main(int argc,char** argv)
{
	//for(int i = 1;i < argc;i++){
		yylineno = 1;
		FILE* f = fopen(argv[1],"r");
		if(!f){
			perror(argv[1]);
			return 1;
		}
		if_print=1;
		yyrestart(f);
		//yydebug=1;
    		yyparse();
		//print_tree(root,0);
		init_tree(root,0);
		init_hash();
		init_structtable();
		sdt(root);
		translate_Program(root);
		char *filename=(char*)malloc(500);
		strcpy(filename,argv[2]);
		if(aerror==0&&serror==0){
		savefile(filename);
		//readfile(filename);
		}
		if(aerror){
			printf("Cannot translate: Code contains variables of multi-dimensional array type or parameters of array type.\n");
		}
		if(serror){
			printf("Cannot translate: Code contains variables or parameters of structure type.\n" );
		}
		cleanhead();
		fclose(f);
	//}
	return 0;
}


