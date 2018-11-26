#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"
#include "symtable.h"
#include "ir.h"
#include "type.h"

int sizeofchar=100;
void translate_ExtDefList(struct Node *root);
void translate_ExtDef(struct Node *root);
InterCodes translate_DefList(struct Node *root);
InterCodes translate_StmtList(struct Node *root);
void translate_Def(struct Node *root,InterCodes *icres);
InterCodes translate_DecList(struct Node *root);
void translate_Dec(struct Node *root,InterCodes *icres);
InterCodes translate_VarDec(struct Node *root);


InterCodes translate_exp(struct Node *root, Operand place);
InterCodes translate_Cond(struct Node *root, Operand label_true, Operand label_false);
InterCodes translate_Stmt(struct Node *root);
InterCodes translate_CompSt(struct Node *root);
InterCodes translate_Args(struct Node *root, OperandList* arg_list);
InterCodes translate_VarList(struct Node *root);
void translate_ParamDec(struct Node *root,InterCodes* icres);
void translate_FunDec(struct Node *root,InterCodes *icres);

void cleanhead(){
	while(iclist!=NULL){
		iclist=iclist->next;
	}
	iclist=NULL;
}
void  op_name2no(char *name,int *res) {
	if(strcmp(name, ">") == 0)
		*res=1;
	else if(strcmp(name, "<") == 0)
		*res=2;
	else if(strcmp(name, ">=") == 0)
		*res=3;
	else if(strcmp(name, "<=") == 0)
		*res=4;
	else if(strcmp(name, "==") == 0)
		*res=5;
	else if(strcmp(name, "!=") == 0)
		*res=6;
}

void op_no2name(int no,char *res) {
    if (no==1){strcpy(res, ">");}
    else if (no==2){strcpy(res, "<");}
    else if (no==3){strcpy(res, ">=");}
    else if (no==4){strcpy(res, "<=");}
    else if (no==5){strcpy(res, "==");}
    else if (no==6){strcpy(res, "!=");}
}

Operand new_op(int kind){
	Operand res = (Operand)malloc(sizeof(struct Operand_));
	if(kind==TEMP){
		res->kind = TEMP;
		res->u.temp_no = temp_index;
		temp_index ++;
		return res;
	}
	else if(kind==LABEL){
		res->kind = LABEL;
		res->u.label_no = label_index;
		label_index ++;
		return res;
	}
	else if(kind == VARIABLE1){
		res->kind = VARIABLE1;
		res->u.var_no = var_index;
		var_index ++;
		return res;
	}
	return res;
}

void init_ic1(int kind,Operand left,Operand right,InterCodes* res){
	(*res)->code.kind = kind;
	(*res)->code.u.assign.left = left;
  (*res)->code.u.assign.right = right;
	(*res)->next = NULL;
}
void init_ic2(int kind,Operand op1,Operand op2,Operand result,InterCodes* res){
	(*res)->code.kind = kind;
	(*res)->code.u.binop.op1 = op1;
	(*res)->code.u.binop.op2 = op2;
	(*res)->code.u.binop.result = result;
	(*res)->next = NULL;
}

void init_ic3(int kind,Operand op1,InterCodes* res){
	(*res)->code.kind = kind;
	(*res)->code.u.singleop.op = op1;
	(*res)->next = NULL;
}

void init_ic4(int kind,Operand op1,Operand op2,Operand result,int optype,InterCodes* res){
	(*res)->code.kind = kind;
	(*res)->code.u.ifop.op1 = op1;
	(*res)->code.u.ifop.op2 = op2;
	(*res)->code.u.ifop.result = result;
	(*res)->code.u.ifop.optype = optype;
	(*res)->next = NULL;
}

Operand init_operand1(int kind, int value) {
	Operand res = (Operand)malloc(sizeof(struct Operand_));
	res->kind = kind;
	if(kind == CONSTANT) {
		res->u.const_val = value;
	}
	else if(kind == NUMBER) {
		res->u.number = value;
	}
	return res;
}
Operand init_operand2(int kind, char *name) {
	Operand res = (Operand)malloc(sizeof(struct Operand_));
	res->kind = kind;
	if(kind == FUNCTION1) {
		res->u.func_name = (char*)malloc(sizeofchar);
		strcpy(res->u.func_name, name);
	}
	else if(kind == VARIABLE1) {
		res->u.var_name = (char*)malloc(sizeofchar);
		strcpy(res->u.var_name,name);
	}
	return res;
}


InterCodes strcatic(InterCodes ic1, InterCodes ic2,InterCodes *res) {
	if(ic1 == NULL)
		*res= ic2;
	else if(ic2 == NULL)
		*res= ic1;
	else{
		InterCodes head = ic1;
		for(head;head->next!=NULL;head=head->next);
		head->next = ic2;
		*res=ic1;
}

	return *res;
}



void linkic2(InterCodes c1,InterCodes c2,InterCodes* res) {
	*res=strcatic(c1, c2,&(*res));
}


void linkic3(InterCodes c1,InterCodes c2,InterCodes c3,InterCodes* res) {
	*res=strcatic(c1, c2,&(*res));
	*res=strcatic(c1, c3,&(*res));
}
void linkic4(InterCodes c1,InterCodes c2,InterCodes c3,InterCodes c4,InterCodes* res) {
	*res=strcatic(c1, c2,&(*res));
	*res=strcatic(c1, c3,&(*res));
	*res=strcatic(c1, c4,&(*res));
}

void linkic6(InterCodes c1,InterCodes c2,InterCodes c3,InterCodes c4,InterCodes c5,InterCodes c6,InterCodes* res) {
	*res=strcatic(c1, c2,&(*res));
	*res=strcatic(c1, c3,&(*res));
	*res=strcatic(c1, c4,&(*res));
	*res=strcatic(c1, c5,&(*res));
	*res=strcatic(c1, c6,&(*res));
}

void linkic7(InterCodes c1,InterCodes c2,InterCodes c3,InterCodes c4,InterCodes c5,InterCodes c6,InterCodes c7,InterCodes* res) {
	*res=strcatic(c1, c2,&(*res));
	*res=strcatic(c1, c3,&(*res));
	*res=strcatic(c1, c4,&(*res));
	*res=strcatic(c1, c5,&(*res));
	*res=strcatic(c1, c6,&(*res));
	*res=strcatic(c1, c7,&(*res));

}

void operand2str(Operand op,char* res) {
			if(op == NULL){
				res=NULL;
				return ;
			}
      if (op->kind==TEMP) {	char *res2 = (char*)malloc(sizeofchar);res = strcat(res, "t");sprintf(res2, "%d", op->u.temp_no); res = strcat(res, res2);}
       else if (op->kind==VARIABLE1) {strcpy(res,op->u.var_name);}
       else if (op->kind==CONSTANT) {	char *res2 = (char*)malloc(sizeofchar);res = strcat(res, "#");sprintf(res2, "%d", op->u.const_val); res = strcat(res, res2);}
       else if (op->kind==LABEL) {	char *res2 = (char*)malloc(sizeofchar);res = strcat(res, "LABEL");sprintf(res2, "%d", op->u.label_no); res = strcat(res, res2);}
       else if (op->kind==FUNCTION1) {strcpy(res, op->u.func_name);}
       else if (op->kind==NUMBER) {	char *res2 = (char*)malloc(sizeofchar);sprintf(res2, "%d", op->u.number); res = strcat(res, res2); }
       else if (op->kind==REF) {res = strcat(res, "&");res = strcat(res, op->u.func_name);}
			 //printf("oooooooooooooot%s\n",res );


}







void ic2str(InterCodes ic,char *res) {
				struct InterCode code = ic->code;

        if (code.kind==icASSIGN){
					char* tl=(char*)malloc(sizeofchar);
					char* tr=(char*)malloc(sizeofchar);
					operand2str(code.u.assign.left,tl);
					operand2str(code.u.assign.right,tr);
					strcpy(res,tl);
					strcat(res," := ");
					strcat(res,tr);

				}
        else if (code.kind==icADD){
					char* r1=(char*)malloc(sizeofchar);
					char* o1=(char*)malloc(sizeofchar);
					char* o2=(char*)malloc(sizeofchar);
					operand2str(code.u.binop.result,r1);
					operand2str(code.u.binop.op1,o1);
					operand2str(code.u.binop.op2,o2);
					strcpy(res,r1);
					strcat(res," := ");
					strcat(res,o1);
					strcat(res," + ");
					strcat(res,o2);

				}
        else if (code.kind==icSUB){
					char* r1=(char*)malloc(sizeofchar);
					char* o1=(char*)malloc(sizeofchar);
					char* o2=(char*)malloc(sizeofchar);
					operand2str(code.u.binop.result,r1);
					operand2str(code.u.binop.op1,o1);
					operand2str(code.u.binop.op2,o2);
					strcpy(res,r1);
					strcat(res," := ");
					strcat(res,o1);
					strcat(res," - ");
					strcat(res,o2);
				}
        else if (code.kind==icMUL){
					char* r1=(char*)malloc(sizeofchar);
					char* o1=(char*)malloc(sizeofchar);
					char* o2=(char*)malloc(sizeofchar);
					operand2str(code.u.binop.result,r1);
					operand2str(code.u.binop.op1,o1);
					operand2str(code.u.binop.op2,o2);
					strcpy(res,r1);
					strcat(res," := ");
					strcat(res,o1);
					strcat(res," * ");
					strcat(res,o2);
				}
        else if (code.kind==icDIV){
					char* r1=(char*)malloc(sizeofchar);
					char* o1=(char*)malloc(sizeofchar);
					char* o2=(char*)malloc(sizeofchar);
					operand2str(code.u.binop.result,r1);
					operand2str(code.u.binop.op1,o1);
					operand2str(code.u.binop.op2,o2);
					strcpy(res,r1);
					strcat(res," := ");
					strcat(res,o1);
					strcat(res," / ");
					strcat(res,o2);
				}
        else if (code.kind==icLABEL){
					char* o=(char*)malloc(sizeofchar);
					operand2str(code.u.singleop.op,o);
					strcpy(res,"LABEL ");
					strcat(res,o);
					strcat(res," :");
				}
        else if (code.kind==icRETURN){
					char* o=(char*)malloc(sizeofchar);
					operand2str(code.u.singleop.op,o);
					strcpy(res, "RETURN ");
					strcat(res,o);
				}
        else if (code.kind==icGOTO){
					char* o=(char*)malloc(sizeofchar);
					operand2str(code.u.singleop.op,o);
					strcpy(res,"GOTO ");
					strcat(res,o);
				}
        else if (code.kind==icCALL){
					char* l=(char*)malloc(sizeofchar);
					char *r=(char*)malloc(sizeofchar);
					operand2str(code.u.assign.left,l);
					operand2str(code.u.assign.right,r);
					strcpy(res, l);
					strcat(res," := CALL ");
					strcat(res,r);
				}
        else if (code.kind==icREAD){
					char* o=(char*)malloc(sizeofchar);
					operand2str(code.u.singleop.op,o);
					strcpy(res,"READ ");
					strcat(res,o);
				}
        else if (code.kind==icWRITE){
					//printf("SSSSSSSSSSS\n\n\n\n\n" );
					char* o=(char*)malloc(sizeofchar);
					operand2str(code.u.singleop.op,o);
					strcpy(res, "WRITE ");
					strcat(res,o);
				}
        else if (code.kind==icIF){
					char*ss=(char*)malloc(sizeofchar);
					op_no2name(code.u.ifop.optype,ss);
					char *o1=(char*)malloc(sizeofchar);
					char *o2=(char*)malloc(sizeofchar);
					char *r=(char*)malloc(sizeofchar);
					operand2str(code.u.ifop.op1,o1);
					operand2str(code.u.ifop.op2,o2);
					operand2str(code.u.ifop.result,r);
					strcpy(res, "IF ");
					strcat(res,o1);
					strcat(res," ");
					strcat(res,ss);
					strcat(res," ");
					strcat(res,o2);
					strcat(res," GOTO ");
					strcat(res,r);
				}
        else if (code.kind==icFUNCTION){
					char* o=(char*)malloc(sizeofchar);
					operand2str(code.u.singleop.op,o);
					strcpy(res,"FUNCTION " );
					strcat(res,o);
					strcat(res," :");
				}
        else if (code.kind==icPARAM){
					char* o=(char*)malloc(sizeofchar);
					operand2str(code.u.singleop.op,o);
					strcpy(res,"PARAM ");
					strcat(res,o);
				}
        else if (code.kind==icARG){
					char* o=(char*)malloc(sizeofchar);
					operand2str(code.u.singleop.op,o);
					strcpy(res,"ARG ");
					strcat(res,o);
				}
        else if (code.kind==icRSTAR){
					char* l=(char*)malloc(sizeofchar);
					char *r=(char*)malloc(sizeofchar);
					operand2str(code.u.assign.left,l);
					operand2str(code.u.assign.right,r);
					strcpy(res,l);
					strcat(res," := *");
					strcat(res,r);
				}
        else if (code.kind==icDEC){
					char* l=(char*)malloc(sizeofchar);
					char *r=(char*)malloc(sizeofchar);
					operand2str(code.u.assign.left,l);
					operand2str(code.u.assign.right,r);
					strcpy(res, "DEC ");
					strcat(res,l);
					strcat(res," ");
					strcat(res,r);
				}
        else if (code.kind==icLSTAR){
					char* l=(char*)malloc(sizeofchar);
					char *r=(char*)malloc(sizeofchar);
					operand2str(code.u.assign.left,l);
					operand2str(code.u.assign.right,r);
					strcpy(res,"*");
					strcat(res,l);
					strcat(res," := ");
					strcat(res,r);
				}
        else if (code.kind==icREF){
					char* l=(char*)malloc(sizeofchar);
					char *r=(char*)malloc(sizeofchar);
					operand2str(code.u.assign.left,l);
					operand2str(code.u.assign.right,r);
					strcpy(res, l);
					strcat(res," := &");
					strcat(res,r);
				}

}




void readfile(char *filename){
	 FILE *fp;
	 if(NULL == (fp = fopen(filename, "r")))
	 {
			 printf("error\n");
			 exit(1);
	 }

	 char ch;
	 while(EOF != (ch=fgetc(fp)))
	 {
			 printf("%c", ch);
	 }

	 fclose(fp);
}
void savefile(char *filename) {
	FILE *fp = fopen(filename, "w");
	InterCodes head = iclist;
	/*while(head != NULL) {
		char *temp =(char*)malloc(sizeof(char)*1000);
	  ic2str(head,temp);

		fprintf(fp, "%s\n", temp);
		head = head->next;
	}*/
	for(;head!=NULL;head=head->next){
		char *temp =(char*)malloc(sizeof(char)*1000);
	  ic2str(head,temp);
		fprintf(fp, "%s\n", temp);
	}
	fclose(fp);
}


int arg_is_list = 0;
int left_list = 0;
void check_if_array1(Node* t,int *res){
	if(t==NULL)return;
	if(strcmp(t->name,"ID")==0){
		int num=find_hash(t);
		if(num !=-1){
			if(Symtable[num].type->kind!=1 && *res==0){
				*res=1;
				//print_error(t,10);
			}
		}
	}

	int i=0;
	for(i;i<7;i++){
		if(t->child[i]!=NULL && strcmp(t->child[i]->name,"FLOAT")==0){
			//printf("%s\n",t->child[i]->value.text );
			//print_error(t->child[i],12);
		}
		if(t->child[i]!=NULL && (strcmp(t->child[i]->name,"Exp")==0 || strcmp(t->child[i]->name,"ID")==0) )
			check_if_array1(t->child[i],&(*res));
	}
}
Type get_type_of_exp1(Node* t)
{
	Type temp =NULL;
	//if(t==NULL)return;
	if(strcmp(t->name,"INT")==0){
		Type type = (Type)malloc(sizeof(struct Type_));
		type->kind = BASIC;
		type->u.basic = 0;
		return type;
	}
	if(strcmp(t->name,"FLOAT")==0){
		Type type = (Type)malloc(sizeof(struct Type_));
		type->kind = BASIC;
		type->u.basic = 1;
		return type;
	}
	if(strcmp(t->name,"ID")==0){
		int num=find_hash(t);
		//printf("find_hash: %d     %d\n",num ,Symtable[num].type->kind);
		if(num !=-1){
			//printf("in hear %d    %d\n",num,Symtable[num].type->kind );
			return(Symtable[num].type);
		}
		else{
			return NULL;
		}
	}
	if(t->child[0]!=NULL && strcmp(t->child[0]->name,"MINUS")==0){
		return get_type_of_exp1(t->child[1]);
	}
	if(t->child[0]!=NULL && strcmp(t->child[0]->name,"NOT")==0){
		return get_type_of_exp1(t->child[1]);
	}
	if(t->child[0]!=NULL && strcmp(t->child[0]->name,"LP")==0){
		return get_type_of_exp1(t->child[1]);
		}
	if(t->child[1]!=NULL && strcmp(t->child[1]->name,"PLUS")==0){
		return get_type_of_exp1(t->child[0]);

	}
	if(t->child[1]!=NULL && strcmp(t->child[1]->name,"MINUS")==0){
		return get_type_of_exp1(t->child[0]);
	}
	if(t->child[1]!=NULL && strcmp(t->child[1]->name,"STAR")==0){
		return get_type_of_exp1(t->child[0]);
	}
	if(t->child[1]!=NULL && strcmp(t->child[1]->name,"DIV")==0){
		return get_type_of_exp1(t->child[0]);
	}
	if(t->child[0]!=NULL && strcmp(t->child[0]->name,"ID")==0){
		return get_type_of_exp1(t->child[0]);
	}
	if(t->child[1]!=NULL && strcmp(t->child[1]->name,"DOT")==0){
		return get_type_of_exp1(t->child[2]);
	}
	if(t->child[1]!=NULL && strcmp(t->child[1]->name,"LB")==0){
		int i=0;
		while(strcmp(t->child[0]->name,"ID")!=0){
			i++;
			t=t->child[0];
		}
		//printf("i:%d\n",i);
		int loac=find_hash(t->child[0]);
		if(loac==-1){
			//print_error(t->child[0],1);
		}
		else{
			Type p1=(Type)malloc(sizeof(struct Type_));
			p1=Symtable[loac].type;
			int j=0;
			while(j<i){
				//printf("%d\n",p1->kind);
				if(p1->kind==1 && p1->u.array.elem!=NULL){
					p1=p1->u.array.elem;continue;
				}
				if(p1->kind==0 && (j+1)<i ){
					//printf("Error Type 18 at line %d: NullPointerException\n",t->child[0]->line);
					break;
				}
				j++;
			}
			return p1;
		}

	}
	if(t->child[0]!=NULL && strcmp(t->child[0]->name,"INT")==0){
		return get_type_of_exp1(t->child[0]);
	}
	if(t->child[0]!=NULL && strcmp(t->child[0]->name,"FLOAT")==0){
		return get_type_of_exp1(t->child[0]);
	}
	return temp;
}


void translate_Program(struct Node *root) {
//	printf("%s\n",root->name);
	temp_index=1 ;
	label_index=1 ;
	var_index=1 ;
	aerror=0;
	serror=0;
	translate_ExtDefList(root->child[0]);
//	printInterCodes();
}

void translate_ExtDefList(struct Node *root) {
	if(root==NULL)return;
//	printf("%s\n",root->name);
	translate_ExtDef(root->child[0]);
	translate_ExtDefList(root->child[1]);
}

void translate_ExtDef(struct Node *root) {
	if(root==NULL)return;
	//printf("%s\n",root->name);

	if(strcmp(root->child[1]->name, "FunDec") == 0) {
		InterCodes code1=(InterCodes)malloc(sizeof(struct InterCodes_));
		translate_FunDec(root->child[1],&code1);
		if(iclist == NULL)
			iclist = code1;
		else {
			 InterCodes head1 = iclist;
			 for(head1;head1->next!=NULL;head1=head1->next);
				head1->next = code1;
		}
		InterCodes code2 = translate_CompSt(root->child[2]);
		if(NULL==iclist)
			iclist = code2;
		else {
			InterCodes head = iclist;
			for(head;head->next!=NULL;head=head->next);
			head->next = code2;
		}
	}
}


void translate_FunDec(struct Node *root,InterCodes* icres) {
	//printf("%s\n",root->name);
	InterCodes code1 = (InterCodes)malloc(sizeof(struct InterCodes_));
	init_ic3(icFUNCTION, init_operand2(FUNCTION1,root->child[0]->value.id_value),&code1);
	if(strcmp(root->child[2]->name, "RP") == 0) {
		*icres = code1;
	}
	else {
		InterCodes code2=translate_VarList(root->child[2]);
		InterCodes res1 = (InterCodes)malloc(sizeof(struct InterCodes_));
		linkic2( code1, code2,&res1);
		*icres=res1;
	}
}

InterCodes translate_VarList(struct Node *root) {
	//printf("%s\n",root->name);
	if(root->child[1] == NULL) {
		InterCodes code1=(InterCodes)malloc(sizeof(struct InterCodes_));
	  translate_ParamDec(root->child[0],&code1);
		return code1;
	}
	else {
		InterCodes code1=(InterCodes)malloc(sizeof(struct InterCodes_));
		translate_ParamDec(root->child[0],&code1);
		InterCodes code2=translate_VarList(root->child[2]);
		InterCodes res1 = (InterCodes)malloc(sizeof(struct InterCodes_));
		linkic2(code1,code2,&res1);
		return res1;
	}
}


void translate_ParamDec(struct Node *root,InterCodes *icres) {
	//printf("%s\n",root->name);
	int dim=0;
	struct Node *p2=root->child[1];
	while (strcmp(p2->name, "ID") != 0){
		p2=p2->child[0];
		dim++;
	}
	if(dim>2){
		aerror=1;
		//printf("sdddddddddd\n\n" );
		return ;
	}
	InterCodes ires31 = (InterCodes)malloc(sizeof(struct InterCodes_));
	init_ic3(icPARAM, init_operand2(VARIABLE1, p2->value.id_value),&ires31);
	*icres=ires31;
	//*icres =init_ic3(icPARAM, init_operand2(VARIABLE1, p2->value.id_value));
}

InterCodes translate_Exp(struct Node *root, Operand place) {
	//printf("%s\n",root->name);
	if(strcmp(root->child[0]->name, "INT") == 0) {
		InterCodes ires1 = (InterCodes)malloc(sizeof(struct InterCodes_));
		init_ic1(icASSIGN,place, init_operand1(CONSTANT, root->child[0]->value.int_value),&ires1);
		return ires1;
	}
	else if(strcmp(root->child[0]->name, "ID") == 0) {
		if(root->child[1] == NULL) {
			InterCodes ires1 = (InterCodes)malloc(sizeof(struct InterCodes_));
			init_ic1(icASSIGN,place, init_operand2(VARIABLE1, root->child[0]->value.id_value),&ires1);
			return ires1;
		}
		else {
			if(strcmp(root->child[2]->name, "RP") == 0) {
				if(strcmp(root->child[0]->value.id_value, "read") == 0){
					InterCodes ires31 = (InterCodes)malloc(sizeof(struct InterCodes_));
					init_ic3(icREAD,place,&ires31);
					return ires31;
					//return init_ic3(icREAD, place);
				}
				InterCodes ires1 = (InterCodes)malloc(sizeof(struct InterCodes_));
				init_ic1(icCALL, place, init_operand2(FUNCTION1, root->child[0]->value.id_value),&ires1);
				return ires1;
			}
			else {
				//function para list
				OperandList arg_list = NULL;
				InterCodes code1 = translate_Args(root->child[2], &arg_list);
				if(strcmp(root->child[0]->value.id_value, "write") == 0){
					InterCodes res21 = (InterCodes)malloc(sizeof(struct InterCodes_));
					InterCodes ires31 = (InterCodes)malloc(sizeof(struct InterCodes_));
					init_ic3(icWRITE, arg_list->op,&ires31);
					linkic2(code1, ires31,&res21);
					return res21;
				}
				else {
					OperandList head = arg_list;
					InterCodes code2 = NULL;
					while(head != NULL) {
						InterCodes res21 = (InterCodes)malloc(sizeof(struct InterCodes_));
						InterCodes ires31 = (InterCodes)malloc(sizeof(struct InterCodes_));
						init_ic3(icARG, head->op,&ires31);
						linkic2( code2, ires31,&res21);
						code2=res21;
						head = head->next;
					}
					if(place == NULL) {
						Operand t1 = new_op(TEMP);
						InterCodes res31 = (InterCodes)malloc(sizeof(struct InterCodes_));
						InterCodes ires1 = (InterCodes)malloc(sizeof(struct InterCodes_));
						init_ic1(icCALL, t1, init_operand2(FUNCTION1, root->child[0]->value.id_value),&ires1);
						linkic3( code1, code2, ires1,&res31);
						return res31;
					}
					else
					{
						InterCodes res31 = (InterCodes)malloc(sizeof(struct InterCodes_));
						InterCodes ires1 = (InterCodes)malloc(sizeof(struct InterCodes_));
						init_ic1(icCALL, place, init_operand2(FUNCTION1, root->child[0]->value.id_value),&ires1);
						linkic3( code1, code2, ires1,&res31);
						return res31;
					}
				}
			}
		}
	}
	else if(strcmp(root->child[0]->name, "MINUS") == 0) {
		Operand t1 = new_op(TEMP);
		InterCodes code1 = translate_Exp(root->child[1], t1);
		Operand op1 = init_operand1(CONSTANT, 0);
		InterCodes code2 = (InterCodes)malloc(sizeof(struct InterCodes_));
		init_ic2(icSUB, op1, t1, place,&code2);
		InterCodes res21 = (InterCodes)malloc(sizeof(struct InterCodes_));
		linkic2( code1, code2,&res21);
		return res21;
	}
	else if(strcmp(root->child[0]->name, "Exp") == 0) {
		struct Node *p2 = root->child[1];
		if(strcmp(root->child[1]->name, "PLUS") == 0) {
			Operand t1 = new_op(TEMP);
			Operand t2 = new_op(TEMP);
			InterCodes code3 = (InterCodes)malloc(sizeof(struct InterCodes_));
			InterCodes code1 = translate_Exp(root->child[0], t1);
			InterCodes code2 = translate_Exp(root->child[2], t2);
			init_ic2(icADD, t1, t2, place,&code3);
			InterCodes res31 = (InterCodes)malloc(sizeof(struct InterCodes_));
			linkic3( code1, code2, code3,&res31);
			return res31;
		}
		else if(strcmp(root->child[1]->name, "MINUS") == 0) {
			Operand t1 = new_op(TEMP);
			Operand t2 = new_op(TEMP);
			InterCodes code3 = (InterCodes)malloc(sizeof(struct InterCodes_));
			InterCodes code1 = translate_Exp(root->child[0], t1);
			InterCodes code2 = translate_Exp(root->child[2], t2);
			init_ic2(icSUB, t1, t2, place,&code3);
			InterCodes res31 = (InterCodes)malloc(sizeof(struct InterCodes_));
			linkic3( code1, code2, code3,&res31);
			return res31;
		}
		else if(strcmp(root->child[1]->name, "STAR") == 0) {
			Operand t1 = new_op(TEMP);
			Operand t2 = new_op(TEMP);
			InterCodes code3 = (InterCodes)malloc(sizeof(struct InterCodes_));
			InterCodes code1 = translate_Exp(root->child[0], t1);
			InterCodes code2 = translate_Exp(root->child[2], t2);
			init_ic2(icMUL, t1, t2, place,&code3);
			InterCodes res31 = (InterCodes)malloc(sizeof(struct InterCodes_));
			linkic3( code1, code2, code3,&res31);
			return res31;
		}
		else if(strcmp(root->child[1]->name, "DIV") == 0) {
			Operand t1 = new_op(TEMP);
			Operand t2 = new_op(TEMP);
			InterCodes code3 = (InterCodes)malloc(sizeof(struct InterCodes_));
			InterCodes code1 = translate_Exp(root->child[0], t1);
			InterCodes code2 = translate_Exp(root->child[2], t2);
			init_ic2(icDIV, t1, t2, place,&code3);
			InterCodes res31 = (InterCodes)malloc(sizeof(struct InterCodes_));
			linkic3( code1, code2, code3,&res31);
			return res31;
		}
		else if(strcmp(root->child[1]->name, "RELOP") == 0 || strcmp(root->child[1]->name, "AND") == 0 || strcmp(root->child[1]->name, "OR") == 0) {
			Operand label1 = new_op(LABEL);
			Operand label2 = new_op(LABEL);
			InterCodes code0 = (InterCodes)malloc(sizeof(struct InterCodes_));
			InterCodes code2 = (InterCodes)malloc(sizeof(struct InterCodes_));
			init_ic1(icASSIGN,place, init_operand1(CONSTANT, 0),&code0);
			InterCodes code1 = translate_Cond(root, label1, label2);
			init_ic3(icLABEL, label1,&code2);
			InterCodes res21 = (InterCodes)malloc(sizeof(struct InterCodes_));
			InterCodes ires1 = (InterCodes)malloc(sizeof(struct InterCodes_));
			init_ic1(icASSIGN,place, init_operand1(CONSTANT, 1),&ires1);
			linkic2( code2, ires1,&res21);
			code2=res21;
			InterCodes res41 = (InterCodes)malloc(sizeof(struct InterCodes_));
			InterCodes ires31 = (InterCodes)malloc(sizeof(struct InterCodes_));
			init_ic3(icLABEL, label2,&ires31);
			linkic4( code0, code1, code2, ires31,&res41);
			return res41;
		}
		else if(strcmp(root->child[1]->name, "ASSIGNOP") == 0) {
			Operand t1 = new_op(TEMP);
			InterCodes code1 = translate_Exp(root->child[2], t1);
			InterCodes code2 = NULL;
			Operand var = NULL;
			if(strcmp(root->child[0]->child[0]->name, "ID") == 0) {
				var = init_operand2(VARIABLE1, root->child[0]->child[0]->value.id_value);
				InterCodes code2 = (InterCodes)malloc(sizeof(struct InterCodes_));
				init_ic1(icASSIGN, var, t1,&code2);
				InterCodes res21 = (InterCodes)malloc(sizeof(struct InterCodes_));
				linkic2(code1,code2,&res21);
				return res21;
			}
			else if(strcmp(root->child[0]->child[0]->name, "Exp") == 0) {	//list
				Operand t2 = new_op(TEMP);
				left_list = 1;
				InterCodes code3 = translate_Exp(root->child[0], t2);
				left_list = 0;
				InterCodes code2 = (InterCodes)malloc(sizeof(struct InterCodes_));
			  init_ic1(icLSTAR, t2, t1,&code2);
				InterCodes res31 = (InterCodes)malloc(sizeof(struct InterCodes_));
				linkic3( code1, code2, code3,&res31);
				return res31;
			}
		}
		else if(strcmp(root->child[1]->name, "LB") == 0) {
			struct Node *lp = root->child[0]->child[0];
			int dim=0;
			while(strcmp(lp->name, "ID") != 0){
				dim++;
				lp = lp->child[0];
			}
			//printf("exp dim:%d\n",dim );
			if(dim>0){
				aerror=1;
				return NULL;
			}
			Operand var = init_operand2(VARIABLE1, lp->value.id_value);
			Type type = get_type_of_exp1(lp);
			int size = 1;
			InterCodes code = NULL;
			Type type_copy = type;
			Type ttype = type_copy;
			ttype = ttype->u.array.elem;
			size *= ttype->u.array.size;
			struct Node *pp = root->child[2];
			Operand t1 = new_op(TEMP);
			InterCodes code1 = translate_Exp(root->child[2], t1);
			InterCodes code2 = (InterCodes)malloc(sizeof(struct InterCodes_));
			init_ic2(icMUL, t1, init_operand1(CONSTANT, (size*4)), t1,&code2);
			InterCodes res41 = (InterCodes)malloc(sizeof(struct InterCodes_));
			InterCodes icres22 = (InterCodes)malloc(sizeof(struct InterCodes_));
			init_ic2(icADD, place, t1, place,&icres22);
			linkic4( code, code1, code2, icres22,&res41);
			code=res41;
			type = type->u.array.elem;
			Operand t2 = new_op(TEMP);
			if(left_list == 0) {
					InterCodes res42 = (InterCodes)malloc(sizeof(struct InterCodes_));
					InterCodes ires1 = (InterCodes)malloc(sizeof(struct InterCodes_));
					InterCodes ires2 = (InterCodes)malloc(sizeof(struct InterCodes_));
					init_ic1( icASSIGN,place, init_operand1(CONSTANT, 0),&ires1);
					init_ic1(icRSTAR, place, place,&ires2);
					InterCodes icres23 = (InterCodes)malloc(sizeof(struct InterCodes_));
					init_ic2(icADD, place, init_operand2(VARIABLE1, lp->value.id_value), place,&icres23);
					linkic4( ires1, code, icres23, ires2,&res42);
					return res42;
			}
			else {
				InterCodes res32 = (InterCodes)malloc(sizeof(struct InterCodes_));
				InterCodes ires1 = (InterCodes)malloc(sizeof(struct InterCodes_));
				init_ic1( icASSIGN,place, init_operand1(CONSTANT, 0),&ires1);
				InterCodes icres23 = (InterCodes)malloc(sizeof(struct InterCodes_));
				init_ic2(icADD, place, init_operand2(VARIABLE1, lp->value.id_value), place,&icres23);
				linkic3( ires1, code, icres23,&res32);
				return res32;
			}
		}
	}
	else if(strcmp(root->child[0]->name, "NOT") == 0) {
		Operand label1 = new_op(LABEL);
		Operand label2 = new_op(LABEL);
		InterCodes code0 = (InterCodes)malloc(sizeof(struct InterCodes_));
		InterCodes ires1 = (InterCodes)malloc(sizeof(struct InterCodes_));
		InterCodes code2 = (InterCodes)malloc(sizeof(struct InterCodes_));
		init_ic1(icASSIGN, place, init_operand1(CONSTANT, 0),&code0);
		InterCodes code1 = translate_Cond(root, label1, label2);
		init_ic3(icLABEL, label1,&code2);
		InterCodes res21 = (InterCodes)malloc(sizeof(struct InterCodes_));
		init_ic1( icASSIGN,place, init_operand1(CONSTANT, 1),&ires1);
		linkic2( code2, ires1,&res21);
		code2=res21;
		InterCodes res42 = (InterCodes)malloc(sizeof(struct InterCodes_));
		InterCodes ires31 = (InterCodes)malloc(sizeof(struct InterCodes_));
		init_ic3(icLABEL, label2,&ires31);
		linkic4( code0, code1, code2, ires31,&res42);
		return res42;
	}
	else if(strcmp(root->child[0]->name, "LP") == 0) {
		return translate_Exp(root->child[1], place);
	}
	return NULL;
}
InterCodes translate_Stmt(struct Node *root) {
	//printf("%s\n",root->name);
	if(strcmp(root->child[0]->name, "Exp") == 0) {
		return translate_Exp(root->child[0], NULL);
	}
	else if(strcmp(root->child[0]->name, "CompSt") == 0) {
		return translate_CompSt(root->child[0]);
	}
	else if(strcmp(root->child[0]->name, "RETURN") == 0) {
		Operand t1 = new_op(TEMP);
		InterCodes code1 = translate_Exp(root->child[1], t1);
		InterCodes code2 = (InterCodes)malloc(sizeof(struct InterCodes_));
	  init_ic3(icRETURN, t1,&code2);
		InterCodes res1 = (InterCodes)malloc(sizeof(struct InterCodes_));
		linkic2(code1,code2,&res1);
		return res1;
	}
	else if(strcmp(root->child[0]->name, "WHILE") == 0) {
		Operand label1 = new_op(LABEL);
		Operand label2 = new_op(LABEL);
		Operand label3 = new_op(LABEL);
		InterCodes code1 = translate_Cond( root->child[2], label2, label3);
		InterCodes code2 = translate_Stmt( root->child[4]);
		InterCodes res61 = (InterCodes)malloc(sizeof(struct InterCodes_));
		InterCodes ires31 = (InterCodes)malloc(sizeof(struct InterCodes_));
		InterCodes ires32 = (InterCodes)malloc(sizeof(struct InterCodes_));
		InterCodes ires33 = (InterCodes)malloc(sizeof(struct InterCodes_));
		InterCodes ires34 = (InterCodes)malloc(sizeof(struct InterCodes_));
		init_ic3(icLABEL, label1,&ires31);
		init_ic3(icLABEL, label2,&ires32);
		init_ic3(icGOTO, label1,&ires33);
		init_ic3(icLABEL, label3,&ires34);
		linkic6( ires31, code1, ires32, code2, ires33, ires34,&res61);
		return res61;
	}
	else if(strcmp(root->child[0]->name, "IF") == 0) {
		if(root->child[5] == NULL) {
			Operand label1 = new_op(LABEL);
			Operand label2 = new_op(LABEL);

			InterCodes code1 = translate_Cond(root->child[2], label1, label2);
			InterCodes code2 = translate_Stmt(root->child[4]);
			InterCodes res41 = (InterCodes)malloc(sizeof(struct InterCodes_));
			InterCodes ires31 = (InterCodes)malloc(sizeof(struct InterCodes_));
			InterCodes ires32 = (InterCodes)malloc(sizeof(struct InterCodes_));
			init_ic3(icLABEL, label1,&ires31);
			init_ic3(icLABEL, label2,&ires32);
			linkic4( code1, ires31, code2, ires32,&res41);
			return res41;
		}
		else {
			Operand label1 = new_op(LABEL);
			Operand label2 = new_op(LABEL);
			Operand label3 = new_op(LABEL);
			InterCodes code1 = translate_Cond(root->child[2], label1, label2);
			InterCodes code2 = translate_Stmt(root->child[4]);
			InterCodes code3 = translate_Stmt(root->child[6]);
			InterCodes res71 = (InterCodes)malloc(sizeof(struct InterCodes_));
			InterCodes ires31 = (InterCodes)malloc(sizeof(struct InterCodes_));
			InterCodes ires32 = (InterCodes)malloc(sizeof(struct InterCodes_));
			InterCodes ires33 = (InterCodes)malloc(sizeof(struct InterCodes_));
			InterCodes ires34 = (InterCodes)malloc(sizeof(struct InterCodes_));
			init_ic3(icLABEL, label1,&ires31);
			init_ic3(icGOTO, label3,&ires32);
			init_ic3(icLABEL, label2,&ires33);
			init_ic3(icLABEL, label3,&ires34);
			linkic7( code1, ires31, code2, ires32, ires33, code3, ires34,&res71);
			return res71;
		}
	}
	return NULL;
}

InterCodes translate_CompSt(struct Node *root) {
	//printf("%s\n",root->name);
	InterCodes code1=translate_DefList(root->child[1]);
	InterCodes code2 = translate_StmtList(root->child[2]);
	InterCodes res1 = (InterCodes)malloc(sizeof(struct InterCodes_));
	linkic2(code1,code2,&res1);
	return res1;
}

InterCodes translate_DefList(struct Node *root) {
	if(root== NULL) {
		return NULL;
	}
	//printf("%s\n",root->name);
	InterCodes code1=(InterCodes)malloc(sizeof(struct InterCodes_));
	translate_Def(root->child[0],&code1);
	InterCodes code2 = translate_DefList(root->child[1]);
	InterCodes res1 = (InterCodes)malloc(sizeof(struct InterCodes_));
	linkic2(code1,code2,&res1);
	return res1;
}

InterCodes translate_StmtList(struct Node *root) {
	if(root==NULL)
		return NULL;
//	printf("%s\n",root->name);
	InterCodes code1 = translate_Stmt(root->child[0]);
	InterCodes code2 = translate_StmtList(root->child[1]);
	InterCodes res1 = (InterCodes)malloc(sizeof(struct InterCodes_));
	linkic2(code1,code2,&res1);
	return res1;
}
void check_struct(struct Node *root){
	//printf("ssss%s\n",root->name);
	if(strcmp(root->name,"StructSpecifier")==0){
		serror=1;
		//printf("struct\n\n");
		return;
	}
	if(root->child[0]!=NULL){
		check_struct(root->child[0]);

	}

}
void translate_Def(struct Node *root,InterCodes *icres) {
	//printf("%s\n",root->name);
	check_struct(root);
	*icres= translate_DecList(root->child[1]);
}

InterCodes translate_DecList(struct Node *root) {
	//printf("%s\n",root->name);
	if(root->child[1] == NULL) {
		InterCodes code1 =(InterCodes)malloc(sizeof(struct InterCodes_));
		translate_Dec(root->child[0],&code1);
		return code1;
	}
	else {
		InterCodes code1=(InterCodes)malloc(sizeof(struct InterCodes_)) ;
		 translate_Dec(root->child[0],&code1);
		InterCodes code2 = translate_DecList(root->child[2]);
		InterCodes res1 = (InterCodes)malloc(sizeof(struct InterCodes_));
		linkic2(code1,code2,&res1);
		return res1;
	}
}

void translate_Dec(struct Node *root,InterCodes *icres) {
	//printf("%s\n",root->name);
	if(root->child[1] == NULL) {
		*icres= translate_VarDec(root->child[0]);
	}
	else {
		//printf("in\n");
		struct Node *lp = root->child[0];
		int dim=0;
		while(strcmp(lp->name, "ID") != 0){
			lp = lp->child[0];
			dim++;
		}
		//printf("dim:%d\n", dim);
		Operand place = new_op(TEMP);
		InterCodes code2 = translate_Exp(root->child[2], place);
		//printf("indec\n");
		//printInterCodes(code2);
		//return code2;
		InterCodes res1 = (InterCodes)malloc(sizeof(struct InterCodes_));
		InterCodes ires1 = (InterCodes)malloc(sizeof(struct InterCodes_));
		init_ic1(icASSIGN, init_operand2(VARIABLE1, lp->value.id_value), place,&ires1);
		linkic2(code2, ires1,&res1);
		*icres= res1;
	}
}
InterCodes translate_VarDec(struct Node *root) {
	//printf("%s\n",root->name);
	struct Node *p1 = root->child[0];
	if(strcmp(root->child[0]->name, "ID") == 0) {
	}
	else {	//list
		int dim1=0;
		while(strcmp(p1->name, "ID") != 0){
			p1 = p1->child[0];
			dim1++;
		}
		//printf("dim1:%d\n",dim1 );
		if(dim1>1){
			aerror=1;
			//printf("vadec dim:%d\n",dim1 );
			//printf("ssssssssssss\n\n" );
			return NULL;
		}
		Type type = get_type_of_exp1(p1);
		int size = 1;
		int dim=0;
		while(type->kind != BASIC) {
			size *= type->u.array.size;
			type = type->u.array.elem;
			dim++;
		}
		if(dim>1){
			//printf("eerrrrr\n" );
			aerror=1;
			return NULL;
		}
		//printf("dim2:%d\n", dim);
		Operand t1 = new_op(TEMP);
		InterCodes res1 = (InterCodes)malloc(sizeof(struct InterCodes_));
		InterCodes ires1 = (InterCodes)malloc(sizeof(struct InterCodes_));
		InterCodes ires2 = (InterCodes)malloc(sizeof(struct InterCodes_));
		init_ic1( icDEC,t1, init_operand1(NUMBER, size*4),&ires1);
		init_ic1(icREF,init_operand2(VARIABLE1, p1->value.id_value), t1,&ires2);
		linkic2(ires1, ires2,&res1);
		return res1;
	}
	return NULL;
}

InterCodes translate_Args(struct Node *root, OperandList* arg_list) {
	//printf("AAAAAAAAAAAAAAAAAA\n\n" );
	if(root->child[1]== NULL) {
		Operand t1 = new_op(TEMP);
		arg_is_list = 1;
		InterCodes code1 = translate_Exp(root->child[0], t1);
		arg_is_list = 0;

		OperandList res = (OperandList)malloc(sizeof(struct OperandList_));
		res->op = t1;
		res->next = NULL;
		bool arg_null=(*arg_list == NULL);
		if(arg_null) {
			*arg_list = res;
		}
		else {
			res->next = *arg_list;
			*arg_list = res;
		}


		return code1;
	}
	else {
		Operand t1 = new_op(TEMP);
		arg_is_list = 1;
		InterCodes code1 = translate_Exp(root->child[0], t1);
		arg_is_list = 0;
		OperandList res = (OperandList)malloc(sizeof(struct OperandList_));
		res->op = t1;
		res->next = NULL;
		bool arg_null=(*arg_list == NULL);
		if(arg_null) {
			*arg_list = res;
		}
		else {
			res->next = *arg_list;
			*arg_list = res;
		}
		InterCodes code2 = translate_Args(root->child[2], arg_list);
		InterCodes res1 = (InterCodes)malloc(sizeof(struct InterCodes_));
		linkic2(code1,code2,&res1);
		return res1;
	}
}

InterCodes translate_Cond(struct Node *root, Operand label_true, Operand label_false) {
	//struct Node *p1 = root->child[0];
	if(strcmp(root->child[0]->name, "Exp") == 0) {
		if(strcmp(root->child[1]->name, "RELOP") == 0) {
			Operand t1 = new_op(TEMP);
			Operand t2 = new_op(TEMP);
			InterCodes code1 = translate_Exp(root->child[0], t1);
			InterCodes code2 = translate_Exp(root->child[2], t2);
			int optype = 0;
			op_name2no(root->child[1]->value.text,&optype);
			InterCodes code3 = (InterCodes)malloc(sizeof(struct InterCodes_));
			init_ic4(icIF, t1, t2, label_true, optype,&code3);
			InterCodes res41 = (InterCodes)malloc(sizeof(struct InterCodes_));
			InterCodes ires31 = (InterCodes)malloc(sizeof(struct InterCodes_));
			init_ic3(icGOTO, label_false,&ires31);
			linkic4( code1, code2, code3, ires31,&res41);
			return res41;
		}
		else if(strcmp(root->child[1]->name, "AND") == 0) {
			Operand label1 = new_op(LABEL);
			InterCodes code1 = translate_Cond(root->child[0], label1, label_false);
			InterCodes code2 = translate_Cond(root->child[2], label_true, label_false);
			InterCodes res31 = (InterCodes)malloc(sizeof(struct InterCodes_));
			InterCodes ires31 = (InterCodes)malloc(sizeof(struct InterCodes_));
			init_ic3(icLABEL, label1,&ires31);
			linkic3( code1, ires31, code2,&res31);
			return res31;
		}
		else if(strcmp(root->child[1]->name, "OR") == 0) {
			Operand label1 = new_op(LABEL);
			InterCodes code1 = translate_Cond(root->child[0], label_true, label1);
			InterCodes code2 = translate_Cond(root->child[2], label_true, label_false);
			InterCodes res31 = (InterCodes)malloc(sizeof(struct InterCodes_));
			InterCodes ires31 = (InterCodes)malloc(sizeof(struct InterCodes_));
			init_ic3(icLABEL, label1,&ires31);
			linkic3( code1, ires31, code2,&res31);
			return res31;
		}
	}
	else if(strcmp(root->child[0]->name, "NOT") == 0) {
		return translate_Cond(root->child[1], label_false, label_true);
	}
	else {
		Operand t1 = new_op(TEMP);
		InterCodes code1 = translate_Exp(root, t1);
		int optype=0;
		op_name2no("!=",&optype);
		InterCodes code2 = (InterCodes)malloc(sizeof(struct InterCodes_));
		init_ic4(icIF, t1, init_operand1(CONSTANT, 0), label_true, optype,&code2);
		InterCodes res32 = (InterCodes)malloc(sizeof(struct InterCodes_));
		InterCodes ires31 = (InterCodes)malloc(sizeof(struct InterCodes_));
		init_ic3(icGOTO, label_false,&ires31);
		linkic3( code1, code2, ires31,&res32);
		return res32;
	}
}

