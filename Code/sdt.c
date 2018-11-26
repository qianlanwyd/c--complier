#include <string.h>
#include "sdt.h"
#include "tree.h"
#include "symtable.h"
#include "type.h"

void init_hash();
void init_structtable();
void print_struct(FieldList t);
void print_error(Node* t,int kind);
void print_error9(char* s,int line,Type* p1,Type* p2,int n1,int n2);
void print_error16(Node* t,char* s);
void Program_(Node* t);//Program ->ExtDefList
void ExtDefList_(Node* t);//ExtDefList -> ExtDef ExtDefList (以下相同)
void ExtDef_(Node* t);
Type Specifier_(Node* t);
Type StructSpecifier_(Node* t);
void StructDefList_(Node* t,FieldList* pre);
void StructDef_(Node* t,FieldList* pre);
void StructDecList_(Node* t,Type type,FieldList* pre);
void StructDec_(Node* t,Type type,FieldList* pre);
void StructVarDec_(Node* t,Type type,FieldList* pre);
void ExtDecList_(Node* t,Type type);
void VarDec_(Node* t,Type type);
void VarDec_Array(Node* t,Type type,Type next);
void FunDec_(Node* t,Type type);
void VarList_(Node* t,Type* p,int* cur);
void CompSt_(Node* t);
void StmtList_(Node* t);
void Stmt_(Node* t);
void DefList_(Node* t);
void DecList_(Node* t,Type type);
void Def_(Node* t);
void Dec_(Node* t,Type type);
void Exp_(Node* t);
Type get_type_of_exp(Node* t);
void can_be_assigned(Node* t,int* res);
void check_return(Node* t,Type type);
void check_field(Node* t,char* name);
int type_equal(Type t1,Type t2);
int struct_equal(FieldList f1,FieldList f2);
void count_arg(Node* t,int* res);
void count_varlist(Node* t,int* res);
void fill_arg(Node* t,int* cur,Type* type);
void func_equal(Node* t,Type *type,int num);
void check_if_array(Node* t,int* res);
int find_field(FieldList *f,char* name);
void check_if_array(Node* t,int *res){
	if(t==NULL)return;
	if(strcmp(t->name,"ID")==0){
		int num=find_hash(t);
		if(num !=-1){
			if(Symtable[num].type->kind!=1 && *res==0){
				*res=1;
				print_error(t,10);
			}
		}
	}

	int i=0;
	for(i;i<7;i++){
		if(t->child[i]!=NULL && strcmp(t->child[i]->name,"FLOAT")==0){
			//printf("%s\n",t->child[i]->value.text );
			print_error(t->child[i],12);
		}
		if(t->child[i]!=NULL && (strcmp(t->child[i]->name,"Exp")==0 || strcmp(t->child[i]->name,"ID")==0) )
			check_if_array(t->child[i],&(*res));
	}
}
void func_equal(Node* t,Type *type,int num){
	if(t==NULL)return;
	int line=t->line;
	int loac=find_hash(t);
	if(loac==-1){
		print_error(t,2);
	}
	//printf("funcequal\n");
	//printf("%d %d\n",Symtable[loac].arg_num,num);
	if(Symtable[loac].arg_num!=num){
		print_error9(Symtable[loac].name,line,Symtable[loac].argument,type,Symtable[loac].arg_num,num);
		return;
	}
	//printf("funcequal2 %s %d\n",t->value.text,num);
	int i=0;
	for(i;i<num;i++){
		if(NULL==type[i])return;
		if(NULL==Symtable[loac].argument){
			print_error9(Symtable[loac].name,line,Symtable[loac].argument,type,Symtable[loac].arg_num,num);
			return;
		}
		else if(type_equal(Symtable[loac].argument[i],type[i])==-1){
			print_error9(Symtable[loac].name,line,Symtable[loac].argument,type,Symtable[loac].arg_num,num);
			return;
		}


	}
	//printf("funcequal3\n");
}
void count_varlist(Node* t,int* res){
	if(t==NULL)return;
	if(t->child[0]!=NULL && strcmp(t->child[0]->name,"ParamDec")==0){
		*res=*res+1;
		count_varlist(t->child[0],&(*res));
	}
	if(t->child[2]!=NULL && strcmp(t->child[2]->name,"VarList")==0){
		count_varlist(t->child[2],&(*res));
	}

}
void count_arg(Node* t,int* res){
	if(t==NULL)return;
	*res=*res+1;
	if(t->child[2]!=NULL && strcmp(t->child[2]->name,"Args")==0)
		count_arg(t->child[2],&(*res));

}
void fill_arg(Node* t,int* cur,Type* type){
	if(t==NULL)return;
	if(t->child[2]!=NULL && strcmp(t->child[2]->name,"Args")==0){
		fill_arg(t->child[2],&(*cur),type);
	}
	if(t->child[0]!=NULL && strcmp(t->child[0]->name,"Exp")==0){
		//Type res=get_type_of_exp(t->child[0]);
		/*if(res->kind!=2){
			type[*cur]=get_type_of_exp(t->child[0]);
			*cur=*cur-1;
		}
		else{
			type[*cur]=Specifier_(t->child[0]);
			*cur=*cur-1;
		}*/
		type[*cur]=get_type_of_exp(t->child[0]);
		*cur=*cur-1;
	}

}

void check_return(Node* t,Type type){
	//printf("here: %d\n",type->u.basic);
	if(t==NULL)return;
	int i=0;
	for(i;i<7;i++){
		if(t->child[i]!=NULL && strcmp(t->child[i]->name,"Stmt")==0){
				check_return(t->child[i],type);
		}
		if(t->child[i]!=NULL && strcmp(t->child[i]->name,"StmtList")==0){
				check_return(t->child[i],type);
		}
		if(t->child[i]!=NULL && strcmp(t->child[i]->name,"CompSt")==0){
				check_return(t->child[i],type);
		}
	}
	if(t->child[0]!=NULL && strcmp(t->child[0]->name,"RETURN")==0){
		Type t1 = (Type)malloc(sizeof(struct Type_));
		t1=get_type_of_exp(t->child[1]);
		if(type_equal(t1,type)!=0 && t1!=NULL && type!=NULL)
			print_error(t,8);
	}
}
void can_be_assigned(Node* t,int* res){
	if(t==NULL)return;
	if(t->child[0]!=NULL && strcmp(t->child[0]->name,"ID")==0)
		*res=0;

	if(t->child[1]!=NULL && strcmp(t->child[1]->name,"LB")==0)
		*res=0;
	if(t->child[1]!=NULL && strcmp(t->child[1]->name,"DOT")==0)
		*res=0;
}
Type get_type_of_exp(Node* t)
{
	//printf("type exp%s\n",t->name);
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
			//printf("null\n");
			return NULL;
		}
	}
	if(t->child[0]!=NULL && strcmp(t->child[0]->name,"MINUS")==0){
		return get_type_of_exp(t->child[1]);
	}
	if(t->child[0]!=NULL && strcmp(t->child[0]->name,"NOT")==0){
		return get_type_of_exp(t->child[1]);
	}
	if(t->child[0]!=NULL && strcmp(t->child[0]->name,"LP")==0){
		return get_type_of_exp(t->child[1]);
		}
	if(t->child[1]!=NULL && strcmp(t->child[1]->name,"PLUS")==0){
		return get_type_of_exp(t->child[0]);

	}
	if(t->child[1]!=NULL && strcmp(t->child[1]->name,"MINUS")==0){
		return get_type_of_exp(t->child[0]);
	}
	if(t->child[1]!=NULL && strcmp(t->child[1]->name,"STAR")==0){
		return get_type_of_exp(t->child[0]);
	}
	if(t->child[1]!=NULL && strcmp(t->child[1]->name,"DIV")==0){
		return get_type_of_exp(t->child[0]);
	}
	if(t->child[0]!=NULL && strcmp(t->child[0]->name,"ID")==0){
		return get_type_of_exp(t->child[0]);
	}
	if(t->child[1]!=NULL && strcmp(t->child[1]->name,"DOT")==0){
		return get_type_of_exp(t->child[2]);
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
			print_error(t->child[0],1);
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
					printf("Error Type 18 at line %d: NullPointerException\n",t->child[0]->line);
					break;
				}
				j++;
			}
			return p1;
		}

	}
	if(t->child[0]!=NULL && strcmp(t->child[0]->name,"INT")==0){
		return get_type_of_exp(t->child[0]);
	}
	if(t->child[0]!=NULL && strcmp(t->child[0]->name,"FLOAT")==0){
		return get_type_of_exp(t->child[0]);
	}
	return temp;
}

int type_equal(Type type1,Type type2){//==:0 !=: -1
	if(type1 == NULL && type2 == NULL)
		return 0;
	else if(type1 == NULL || type2 == NULL)
	{
		return -1;
	}
	if(type1->kind != type2->kind){
		return -1;
	}
	switch(type1->kind) {
		case 0:if(type1->u.basic != type2->u.basic){
						//printf("basic %d  %d\n",type1->u.basic,type2->u.basic );
				   	return -1;
				 }
			   break;
		case 1:if(type_equal(type1->u.array.elem, type2->u.array.elem)==-1)
				   return -1;
			   break;
		case 2:{
			int res= struct_equal(type1->u.structure,type2->u.structure);
		//	printf("check struct:%d\n",struct_equal(type1->u.structure,type2->u.structure) );
			return res;
		}
			   break;
	}
	return 0;
}
int struct_equal(FieldList f1,FieldList f2){
	//if(f1==NULL && f2!=NULL)return -1;;
	while(f1!=NULL){
		//printf("in list:%d   %d\n",f1->type->kind,f2->type->kind );
		if(f2==NULL){
			return -1;
		}
		if(f1->type->kind==2){
			if(f2->type->kind!=2)return-1;
			else{
				int res=struct_equal(f1->type->u.structure,f2->type->u.structure);
				if(res==-1)return -1;
			}
		}
		else{
			if(f2->type->kind==2){
				return -1;
			}
			else{
				if(type_equal(f1->type,f2->type)==-1)return -1;
			}
		}
		f1=f1->tail;
		f2=f2->tail;
	}
	return 0;
}

void init_hash(){
	int i;
	for(i=0;i<hash_num;i++){
		Symtable[i].name = (char*)malloc(sizeof(char)*50);
		strcpy(Symtable[i].name,"null");
	}
}
void init_structtable(){
	int i;
	for(i=0;i<hash_num;i++){
		Structtable[i].used=0;
	}
}
void Program_(Node* t){
	if(t==NULL)return;
	insert_read();
	insert_write();
	//printf("out\n");
	if(t->child[0]!=NULL)
		ExtDefList_(t->child[0]);
}
void ExtDefList_(Node* t){
	if(t==NULL)return;
	if(t->child[0]!=NULL)
		ExtDef_(t->child[0]);
	if(t->child[1]!=NULL)
		ExtDefList_(t->child[1]);
}

void get_Opt(Node* t,char* res){
	if(t==NULL)return;
	if(t->child[0]!=NULL && strcmp(t->child[0]->name,"StructSpecifier")==0){
		get_Opt(t->child[0],res);
	}
	if(t->child[1]!=NULL && strcmp(t->child[1]->name,"OptTag")==0){
		get_Opt(t->child[1],res);
	}
	if(t->child[0]!=NULL && strcmp(t->child[0]->name,"ID")==0){
		strcpy(res,t->child[0]->value.text);
	}
}

void ExtDef_(Node* t){
	if(t==NULL)return;
	if(t->child[0]==NULL)return;
	Type type=get_type_of_sepecifier(t->child[0]);
	if(type->kind==2){
		Type res=(Type)malloc(sizeof(struct Type_));
		res=Specifier_(t->child[0]);
		char *opt=(char*)malloc(50);;
		get_Opt(t->child[0],opt);
		if(strlen(opt)!=0){
			if(find_struct(opt)==-1){
				insert_struct(opt,res);
			}
			else{
				print_error16(t->child[0],opt);
			}
		}
		//print_struct(res->u.structure);
		type=res;
	}
	if(t->child[1]!=NULL && strcmp(t->child[1]->name,"ExtDecList")==0){
		ExtDecList_(t->child[1],type);
	}
	if(t->child[1]!=NULL && strcmp(t->child[1]->name,"FunDec")==0){
		FunDec_(t->child[1],type);
		CompSt_(t->child[2]);
		check_return(t->child[2],type);
	}

}


void print_struct(FieldList t){
	if(t==NULL)return;
	/*if(t->name!=NULL)
		printf("p:%s\n",t->name);*/
	if(t->type->kind==2){
		print_struct(t->type->u.structure);
	}
	if(t->tail!=NULL){
		print_struct(t->tail);
	}
}

Type Specifier_(Node* t){
//	printf("%s\n",t->name );
	Type res=(Type)malloc(sizeof(struct Type_));
	if(strcmp(t->child[0]->name,"StructSpecifier")==0){
		res=StructSpecifier_(t->child[0]);
	}
	return res;
}
Type StructSpecifier_(Node* t){
	//printf("%s\n",t->name );
	if(t->child[1]!=NULL && strcmp(t->child[1]->name,"Tag")==0){
		if(find_struct(t->child[1]->child[0]->value.text)==-1){
			print_error(t->child[1]->child[0],17);
			Type res=NULL;
			return res;
		}
		else{
			int loac=find_struct(t->child[1]->child[0]->value.text);
			int test=insert_hash_var(t->child[1]->child[0],Structtable[loac].type,0);
			return Structtable[loac].type;
		}
	}
	else{
		Type res=(Type)malloc(sizeof(struct Type_));
		res->kind=STRUCTURE;
		res->u.structure=(FieldList)malloc(sizeof(struct FieldList_));
		if(t->child[1]!=NULL&&t->child[1]->child[0]!=NULL){//need insert hash
			//printf("has name:%s\n",t->child[1]->child[0]->value.text );
			FieldList field=NULL;
			StructDefList_(t->child[3],&field);
			res->u.structure=field;
		}
		else{//no need insert hash
			//printf("has no name\n");
			FieldList field=NULL;
			StructDefList_(t->child[3],&field);
			res->u.structure=field;
		}
		return res;
	}
}
void StructDefList_(Node* t,FieldList* pre){
	//printf("in structdeflist: %s\n", t->name);
	/*if((*pre)=NULL){
		(*pre)=(FieldList)malloc(sizeof(struct FieldList_));
	}*/
	if(t->child[0]!=NULL &&strcmp(t->child[0]->name,"Def")==0){
			StructDef_(t->child[0],pre);
	}
	if(t->child[1]!=NULL &&strcmp(t->child[1]->name,"DefList")==0){
			StructDefList_(t->child[1],pre);
	}
}

void StructDef_(Node* t,FieldList* pre){
	Type type=get_type_of_sepecifier(t->child[0]);
	if(type->kind!=2){
		StructDecList_(t->child[1],type,pre);
	}
	else{
		type=Specifier_(t->child[0]);
		/*FieldList t1=(FieldList)malloc(sizeof(struct FieldList_));
		t1=type->u.structure;
		while(t1!=NULL){
			printf("t1test:%s\n",t1->name );
			t1=t1->tail;
		}*/
		StructDecList_(t->child[1],type,pre);
	}
}

void StructDecList_(Node* t,Type type,FieldList* pre){
	//printf("%s\n", t->name);
	if(t->child[0]!=NULL && strcmp(t->child[0]->name,"Dec")==0){
		StructDec_(t->child[0],type,pre);
	}
	if(t->child[2]!=NULL && strcmp(t->child[2]->name,"DecList")==0){
		StructDecList_(t->child[2],type,pre);
	}
}
void StructDec_(Node* t,Type type,FieldList* pre){
	//printf("%s\n",t->name );
	if(t->child[2]==NULL){
		StructVarDec_(t->child[0],type,pre);
	}
	else{
		printf("Error type 15 at Line %d: \"%s\" can not be assigned.\n",t->line,t->child[0]->value.text );
	}
}
void StructVarDec_(Node* t,Type type,FieldList* pre){
	//printf("%s\n",t->name );
	if(t->child[0]!=NULL && strcmp(t->child[0]->name,"ID")==0){
		if((*pre)==NULL){
			(*pre)=(FieldList)malloc(sizeof(struct FieldList_));
			(*pre)->name=(char*)malloc(50);
			(*pre)->type=(Type)malloc(sizeof(struct Type_));
			strcpy((*pre)->name,t->child[0]->value.text);
			(*pre)->type=type;
		}
		else{
			if(find_field(pre,t->child[0]->value.text)==-1){
				printf("Error type 15 at Line %d: Redefined field \"%s\".\n",t->line,t->child[0]->value.text );
			}
			FieldList next=(FieldList)malloc(sizeof(struct FieldList_));
			next->name=(char*)malloc(50);
			next->type=(Type)malloc(sizeof(struct Type_));
			strcpy(next->name,t->child[0]->value.text);
			next->type=type;
			next->tail=(*pre);
			(*pre)=next;
		}
	}
	else if(t->child[0]!=NULL && strcmp(t->child[0]->name,"VarDec")==0){
		Type type1 = (Type)malloc(sizeof(struct Type_));
		type1=type;
		Type next=NULL;
		while(strcmp(t->child[0]->name,"ID")!=0){
			next = (Type)malloc(sizeof(struct Type_));
			next->kind = 1;
			next->u.array.elem = type1;
		  next->u.array.size = t->child[2]->value.int_value;
			type1 = next;
			t=t->child[0];
		}
		if((*pre)==NULL){
			(*pre)=(FieldList)malloc(sizeof(struct FieldList_));
			(*pre)->name=(char*)malloc(50);
			(*pre)->type=(Type)malloc(sizeof(struct Type_));
			strcpy((*pre)->name,t->child[0]->value.text);
			(*pre)->type=type1;
		}
		else{
			if(find_field(pre,t->child[0]->value.text)==-1){
				printf("Error type 15 at Line %d: Redefined field \"%s\".\n",t->line,t->child[0]->value.text );
			}
			FieldList next=(FieldList)malloc(sizeof(struct FieldList_));
			next->name=(char*)malloc(50);
			next->type=(Type)malloc(sizeof(struct Type_));
			strcpy(next->name,t->child[0]->value.text);
			next->type=type1;
			next->tail=(*pre);
			(*pre)=next;
		}
	}
}
int find_field(FieldList* f,char* name){
	FieldList res=(FieldList)malloc(sizeof(struct FieldList_));
	res=*f;
	while(res!=NULL){
		if(strcmp(res->name,name)==0){
			return -1;
		}
		res=res->tail;
	}
}
void ExtDecList_(Node*t,Type type){
	//printf("%s \n",t->name);
	if(t==NULL)return;
	int i=0;
	for(i;i<7;i++){
		if(t->child[i]!=NULL && strcmp(t->child[i]->name,"VarDec")==0){
			VarDec_(t->child[i],type);

		}
		if(t->child[i]!=NULL && strcmp(t->child[i]->name,"ExtDecList")==0){
			ExtDecList_(t->child[i],type);

		}

	}
}




void VarDec_(Node* t,Type type){
	//printf("%s \n",t->name);
	if(t==NULL)return;
	if(t->child[0]==NULL)return;
	if(strcmp(t->child[0]->name,"ID")==0){
		if(find_hash(t->child[0])==-1){
			insert_hash_var(t->child[0],type,0);
		}
		else{
			print_error(t->child[0],3);
		}
	}
	if(strcmp(t->child[0]->name,"VarDec")==0){
		Type type1 = (Type)malloc(sizeof(struct Type_));
		type1=type;
		Type next=NULL;
		while(strcmp(t->child[0]->name,"ID")!=0){
			//printf("innnnn\n" );
			next = (Type)malloc(sizeof(struct Type_));
			next->kind = 1;
			next->u.array.elem = type1;
		  next->u.array.size = t->child[2]->value.int_value;
			type1 = next;
			t=t->child[0];
		}
		if(find_hash(t->child[0])==-1){
			insert_hash_var(t->child[0],type1,0);
		}
		else{
			print_error(t->child[0],3);
		}
		/*Type type2 = (Type)malloc(sizeof(struct Type_));
		type2=type1;
		while(type2!=NULL){
			//printf("ssss\n" );
			if(type2->kind==1)printf("[%d]\n",type2->u.array.size);
			if(type2->kind==0){
				printf("[%d]\n",type2->kind);
				break;
			}
			type2=type2->u.array.elem;
		}*/
	}
}

void FunDec_(Node* t,Type type){
//	printf("%s  %d\n ",t->name,type->kind);
	if(t==NULL)return;
	if(t->child[0]==NULL)return;
	if(strcmp(t->child[0]->name,"ID")==0 && strcmp(t->child[2]->name,"RP")==0){
		if(find_hash(t->child[0])==-1){
			insert_hash_fun1(t->child[0],type,1);
		}
		else{
			print_error(t->child[0],4);
		}
	}
	else{
		if(find_hash(t->child[0])==-1){
			insert_hash_fun2(t->child[0],type,1);
		}
		else{
			print_error(t->child[0],4);
		}
		int var_num=0;
		count_varlist(t->child[2],&var_num);
		int loac=find_hash(t->child[0]);
		if(loac==-1){
			print_error(t->child[0],1);
		}
		else{
			Symtable[loac].arg_num=var_num;
			Symtable[loac].argument= (Type*)malloc(sizeof(Type) * var_num);
			int cur_num=0;
			VarList_(t->child[2],Symtable[loac].argument,&cur_num);
			cur_num=0;
		}
		//for(cur_num;cur_num<var_num;cur_num++)
			//printf("%d...\n",Symtable[loac].argument[cur_num]->u.basic);

	}
}

void VarList_(Node* t,Type* p,int* cur){
	//printf("%s \n",t->name);
	if(t==NULL)return;
	if(t->child[0]==NULL)return;
	if(strcmp(t->child[0]->name,"Specifier")==0){
		Type res=get_type_of_sepecifier(t->child[0]);
		if(res->kind!=2){
			p[*cur]=get_type_of_sepecifier(t->child[0]);
			VarDec_(t->child[1],p[(*cur)]);
			*cur=*cur+1;
			VarList_(t->child[0],p,&(*cur));
		}
		else{
			Type res=(Type)malloc(sizeof(struct Type_));
			res=Specifier_(t->child[0]);
			p[*cur]=res;
			VarDec_(t->child[1],p[(*cur)]);
			*cur=*cur+1;
			VarList_(t->child[0],p,&(*cur));
		}
	}
	if(strcmp(t->child[0]->name,"ParamDec")==0){
		VarList_(t->child[0],p,&(*cur));
	}
	if(t->child[2]!=NULL && strcmp(t->child[2]->name,"VarList")==0){
		VarList_(t->child[2],p,&(*cur));
	}
}

void CompSt_(Node* t){
	//printf("%s \n",t->name);
	if(t==NULL)return;
	if(t->child[0]==NULL)return;
	//printf("here\n");
	if(t->child[1]!=NULL)
		DefList_(t->child[1]);
	if(t->child[2]!=NULL){
		StmtList_(t->child[2]);
	}
}

void StmtList_(Node* t){
	if(t==NULL)return;
	if(t->child[0]==NULL)return;
	Stmt_(t->child[0]);
	if(t->child[1]!=NULL)
		StmtList_(t->child[1]);
}
void Stmt_(Node* t){
	if(t==NULL)return;
	if(t->child[0]==NULL)return;
	if(t->child[0]!=NULL && strcmp(t->child[0]->name,"Exp")==0){
		//printf("in\n" );
		Exp_(t->child[0]);
		//printf("out\n" );

	}
	if(t->child[0]!=NULL &&strcmp(t->child[0]->name,"CompSt")==0){
		CompSt_(t->child[0]);
	}
	if(t->child[1]!=NULL &&strcmp(t->child[1]->name,"Exp")==0){
		Exp_(t->child[1]);
	}
	if(t->child[2]!=NULL &&strcmp(t->child[2]->name,"Exp")==0){
		Exp_(t->child[2]);
	}
	if(t->child[4]!=NULL &&strcmp(t->child[4]->name,"Stmt")==0){
		Stmt_(t->child[4]);
	}
	if(t->child[6]!=NULL &&strcmp(t->child[6]->name,"Stmt")==0){
		Stmt_(t->child[6]);
	}
}
void DefList_(Node* t){
	if(t==NULL)return;
	if(t->child[0]==NULL)return;
	if(t->child[0]!=NULL){
		Def_(t->child[0]);
	}
	if(t->child[1]!=NULL)
		DefList_(t->child[1]);
}

void DecList_(Node* t,Type type){
	if(t==NULL)return;
	if(t->child[0]==NULL)return;
	if(t->child[0]!=NULL){
		Node* t1=t->child[0];
		Dec_(t1,type);
	}
	if(t->child[2]!=NULL){
		DecList_(t->child[2],type);
	}

}

void Def_(Node* t){
	//printf("%s\n ",t->name);
	if(t==NULL)return;
	if(t->child[0]==NULL)return;
	Type type;
	if(t->child[0]!=NULL){
		type=get_type_of_sepecifier(t->child[0]);
		//printf("type:%d\n",type->u.basic );
	}

	if(type->kind==2){
		type=Specifier_(t->child[0]);
	}
	if(t->child[1]!=NULL){
		t=t->child[1];
		DecList_(t,type);
	}
}



void Dec_(Node* t,Type type){
	//printf("%s \n",t->name);
	if(t==NULL)return;
	if(t->child[0]!=NULL){
		VarDec_(t->child[0],type);
	}
	if(t->child[2]!=NULL){
		Exp_(t->child[2]);
	}
}



void Exp_(Node* t){
	//printf("%s \n",t->name);
	if(t==NULL)return;
	if(t->child[0]==NULL)return;
	if(strcmp(t->child[0]->name,"int")==0)return;
	if(strcmp(t->child[0]->name,"FLOAT")==0)return;
	if(strcmp(t->child[0]->name,"ID")==0 &&t->child[1]==NULL){
		if(find_hash(t->child[0])==-1){
			print_error(t->child[0],1);
		}
	}
	else if(strcmp(t->child[0]->name,"ID")==0 &&t->child[1]!=NULL){
		int loac=find_hash(t->child[0]);
		//printf("out1\n");
		if(loac==-1){
			print_error(t->child[0],2);
		}
		else if(Symtable[loac].kind==0){
			print_error(t->child[0],11);
		}
		else{
			int arg_num=0;
			if(t->child[2]!=NULL && strcmp(t->child[2]->name,"RP")!=0){
				count_arg(t->child[2],&arg_num);
			}
			//printf("args:%d\n",arg_num );
			Type* args=(Type*)malloc(sizeof(Type) * arg_num);
			int cur=arg_num-1;
			if(arg_num>0 && t->child[2]!=NULL && strcmp(t->child[2]->name,"RP")!=0){
				fill_arg(t->child[2],&cur,args);
			}
			//printf("out1 %d\n",cur);
			func_equal(t->child[0],args,arg_num);
			//printf("out2\n");

		}
	}
	if(t->child[1]!=NULL && strcmp(t->child[1]->name,"ASSIGNOP")==0){
		Exp_(t->child[0]);
		Exp_(t->child[2]);
		int res=-1;
		can_be_assigned(t->child[0],&res);
		if(res==-1&&t!=NULL){
			print_error(t,6);
		}
		Type t1 = (Type)malloc(sizeof(struct Type_));
		Type t2 = (Type)malloc(sizeof(struct Type_));
		t1=get_type_of_exp(t->child[0]);
		t2=get_type_of_exp(t->child[2]);
		if(t1!=NULL && t2!=NULL &&type_equal(t1,t2)!=0 )
			print_error(t,5) ;
	}
	if(t->child[1]!=NULL && strcmp(t->child[1]->name,"PLUS")==0){
		Exp_(t->child[0]);
		Exp_(t->child[2]);

		Type t1 = (Type)malloc(sizeof(struct Type_));
		Type t2 = (Type)malloc(sizeof(struct Type_));
		t1=get_type_of_exp(t->child[0]);
		t2=get_type_of_exp(t->child[2]);
		if(type_equal(t1,t2)!=0 && t1!=NULL && t2!=NULL)
			print_error(t,7);
	}
	if(t->child[1]!=NULL && strcmp(t->child[1]->name,"MINUS")==0){
		Exp_(t->child[0]);
		Exp_(t->child[2]);
		Type t1 = (Type)malloc(sizeof(struct Type_));
		Type t2 = (Type)malloc(sizeof(struct Type_));
		t1=get_type_of_exp(t->child[0]);
		t2=get_type_of_exp(t->child[2]);
		if(type_equal(t1,t2)!=0 && t1!=NULL && t2!=NULL)
			print_error(t,7);
	}
	if(t->child[1]!=NULL && strcmp(t->child[1]->name,"STAR")==0){
		Exp_(t->child[0]);
		Exp_(t->child[2]);
		Type t1 = (Type)malloc(sizeof(struct Type_));
		Type t2 = (Type)malloc(sizeof(struct Type_));
		t1=get_type_of_exp(t->child[0]);
		t2=get_type_of_exp(t->child[2]);
		if(type_equal(t1,t2)!=0 && t1!=NULL && t2!=NULL)
			print_error(t,7);
	}
	if(t->child[1]!=NULL && strcmp(t->child[1]->name,"DIV")==0){
		Exp_(t->child[0]);
		Exp_(t->child[2]);
		Type t1 = (Type)malloc(sizeof(struct Type_));
		Type t2 = (Type)malloc(sizeof(struct Type_));
		t1=get_type_of_exp(t->child[0]);
		t2=get_type_of_exp(t->child[2]);
		if(type_equal(t1,t2)!=0 && t1!=NULL && t2!=NULL)
			print_error(t,7);
	}
	if(t->child[1]!=NULL && strcmp(t->child[1]->name,"LB")==0){
		int res=0;
		check_if_array(t,&res);
	}
	if(t->child[1]!=NULL && strcmp(t->child[1]->name,"AND")==0){
		Exp_(t->child[0]);
		Exp_(t->child[2]);
	}
	if(t->child[1]!=NULL && strcmp(t->child[1]->name,"OR")==0){
		Exp_(t->child[0]);
		Exp_(t->child[2]);
	}
	if(t->child[1]!=NULL && strcmp(t->child[1]->name,"RELOP")==0){
		Exp_(t->child[0]);
		Exp_(t->child[2]);
	}
	if(t->child[0]!=NULL && strcmp(t->child[0]->name,"LP")==0){
		Exp_(t->child[1]);
	}
	if(t->child[0]!=NULL && strcmp(t->child[0]->name,"MINUS")==0){
		Exp_(t->child[1]);
	}
	if(t->child[0]!=NULL && strcmp(t->child[0]->name,"NOT")==0){
		Exp_(t->child[1]);
	}
	if(t->child[1]!=NULL && strcmp(t->child[1]->name,"DOT")==0){
		Type t1 = (Type)malloc(sizeof(struct Type_));
		t1=get_type_of_exp(t->child[0]);
		if(t1==NULL || t1->kind!=2){
			print_error(t->child[1],13);
		}
		if(t->child[2]!=NULL && strcmp(t->child[2]->name,"ID")==0){
			//printf("jjjjjjjj  %s\n" ,t->child[2]->value.text);
			check_field(t->child[0],t->child[2]->value.text);
		}
		Exp_(t->child[0]);
	}
}
void check_field(Node* t,char* name){
	if(t==NULL)return;
	char* optname=(char*)malloc(50);
	if(t->child[0]!=NULL){
		strcpy(optname,t->child[0]->value.text);
	}
	int loac=find_hash(t->child[0]);
	if(loac==-1){
		print_error(t->child[2],17);
	}
	else{
		FieldList res=Symtable[loac].type->u.structure;
		int has_f=0;
		while(res!=NULL){
			if(strcmp(res->name,name)==0){
				has_f=1;
				break;
			}
			res=res->tail;
		}
		if(has_f==0){
			printf("Error type 14 at Line %d: Non-existent field \"%s\".\n",t->line,name);
		}
	}
 }






void sdt(Node* t){
	if(t==NULL)return;
	Program_(t);
}


void print_error(Node* t,int kind){
	if(t==NULL)return;
	switch(kind){
		case 1:printf("Error type %d at Line %d:Undefined variable \"%s\".\n",kind,t->line,t->value.text);break;
		case 2:printf("Error type %d at Line %d:Undefined function \"%s\".\n",kind,t->line,t->value.text);break;
		case 3:printf("Error type %d at Line %d:Redefined variable \"%s\".\n",kind,t->line,t->value.text);break;
		case 4:printf("Error type %d at Line %d:Redefined function \"%s\".\n",kind,t->line,t->value.text);break;
		case 5:printf("Error type %d at Line %d:Type mismatched for assignment.\n",kind,t->line);break;
		case 6:printf("Error type %d at Line %d:The left-hand side of an assignment must be a variable.\n",kind,t->line);break;
		case 7:printf("Error type %d at Line %d:Type mismatched for oprands.\n",kind,t->line);break;
		case 8:printf("Error type %d at Line %d:Type mismatched for return.\n",kind,t->line);break;
		case 10:printf("Error type %d at Line %d: \"%s\"is not an array.\n",kind,t->line,t->value.text);break;
		case 11:printf("Error type %d at Line %d: \"%s\"is not an function.\n",kind,t->line,t->value.text);break;
		case 12:printf("Error type %d at Line %d: \"%s\"is not an integer.\n",kind,t->line,t->value.text);break;
		case 13:printf("Error type %d at Line %d: Illegal use of \"%s\".\n",kind,t->line,t->value.text);break;
		case 17:printf("Error type %d at Line %d: Undefined structure \"%s\".\n",kind,t->line,t->value.text);break;
	}
	//printf("out\n");
}
void print_error9(char* s,int line,Type* p1,Type* p2,int n1,int n2){
	printf("Error type 9  Line %d: Function \"",line);
	printf("%s(",s);
	if(n1!=0){
		int i=0;
		for(i;i<n1-1;i++){
			if(p1[i]->kind==0 && p1[i]->u.basic==0)
				printf("int, ");
			if(p1[i]->kind==0 && p1[i]->u.basic==1)
				printf("float, ");
			if(p1[i]->kind==1)
				printf("array, ");
			if(p1[i]->kind==2)
			  printf("struct, ");
		}
		if(p1[n1-1]->kind==0 && p1[n1-1]->u.basic==0)
				printf("int");
		if(p1[n1-1]->kind==0 && p1[n1-1]->u.basic==1)
				printf("float");
	  if(p1[i]->kind==2)
			 printf("array, ");
		if(p1[i]->kind==2)
				printf("struct");
	}
	printf(")\" is not applicable for arguments \"(");
	if(n2!=0){
		int i=0;
		for(i;i<n2-1;i++){
			if(p2[i]->kind==0 && p2[i]->u.basic==0)
				printf("int, ");
			if(p2[i]->kind==0 && p2[i]->u.basic==1)
				printf("float, ");
			if(p2[i]->kind==1)
				printf("array, ");
			if(p2[i]->kind==2)
				printf("struct, ");

		}
		if(p2[n2-1]->kind==0 && p2[n2-1]->u.basic==0)
				printf("int");
		if(p2[n2-1]->kind==0 && p2[n2-1]->u.basic==1)
				printf("float");
		if(p2[i]->kind==1)
				printf("array");
		if(p2[i]->kind==2)
				printf("struct");
	}
	printf(")\".\n");

}
void print_error16(Node* t,char* s){
	printf("Error type 16 at Line %d: Duplicated name \"%s\".\n",t->line,s );
}

