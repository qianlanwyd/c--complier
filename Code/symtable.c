#include "type.h"
#include "symtable.h"
#include <string.h>

unsigned int hash_pjw(char* name)	//size = 16384
{
	unsigned int val = 0, i;
	for (; *name; ++name)
	{
		val = (val << 2) + *name;
		if (i = val & ~0x3fff) 
			val = (val ^ (i >> 12)) & 0x3fff;
	}
	return val;
}



int insert_hash_var(Node* t,Type type,int kind){
	unsigned int i = hash_pjw(t->value.text);
	unsigned int start=i;
	//printf("insert%s%d\n",t->value.text,i);
	Symbol temp=Symtable[i];
	if(strcmp(temp.name,"null")==0){
		strcpy(Symtable[i].name,t->value.text);
		Symtable[i].type=type;
		Symtable[i].kind=kind;
		//printf("insert at%d\n",i);
		return i;
	}
	while(strcmp(temp.name,"null")!=0)	
	{	
		if(i<hash_num)
			i++;	
		else
			i=0;
		if(i==start)break;
		temp=Symtable[i];
		if(strcmp(temp.name,"null")==0){
			strcpy(Symtable[i].name,t->value.text);
			Symtable[i].type=type;
			Symtable[i].kind=kind;
			return i;
		}
		
	}
	return -1;

}

int insert_hash_fun1(Node* t,Type type,int kind){//func()
	unsigned int i = hash_pjw(t->value.text);
	unsigned int start=i;
	//printf("insert%s%d\n",t->value.text,i);
	Symbol temp=Symtable[i];
	if(strcmp(temp.name,"null")==0){
		strcpy(Symtable[i].name,t->value.text);
		Symtable[i].type=type;
		Symtable[i].kind=kind;
		Symtable[i].arg_num=0;
		//printf("insert at%d\n",i);
		return i;
	}
	while(strcmp(temp.name,"null")!=0)	
	{	
		if(i<hash_num)
			i++;	
		else
			i=0;
		if(i==start)break;
		temp=Symtable[i];
		if(strcmp(temp.name,"null")==0){
			strcpy(Symtable[i].name,t->value.text);
			Symtable[i].type=type;
			Symtable[i].kind=kind;
			Symtable[i].arg_num=0;
			return i;
		}
		
	}
	return -1;

}
int insert_hash_fun2(Node* t,Type type,int kind){//func(int,float,...)
	int num=0;
	Node* h=t->child[2];
	while(h!=NULL){
		h=h->child[2];
		num++;
	}
	unsigned int i = hash_pjw(t->value.text);
	unsigned int start=i;
	//printf("insert%s%d\n",t->value.text,i);
	Symbol temp=Symtable[i];
	if(strcmp(temp.name,"null")==0){
		strcpy(Symtable[i].name,t->value.text);
		Symtable[i].type=type;
		Symtable[i].kind=kind;
		Symtable[i].arg_num=num;
		return i;
	}
	while(strcmp(temp.name,"null")!=0)	
	{	
		if(i<hash_num)
			i++;	
		else
			i=0;
		if(i==start)break;
		temp=Symtable[i];
		if(strcmp(temp.name,"null")==0){
			strcpy(Symtable[i].name,t->value.text);
			Symtable[i].type=type;
			Symtable[i].kind=kind;
			Symtable[i].arg_num=num;
			return i;
		}
		
	}
	return -1;

}
int find_hash(Node* t){
	unsigned int i = hash_pjw(t->value.text);
	unsigned int start=i;
	//printf("find i %s%d\n",t->value.text,i);
	Symbol temp=Symtable[i];
	if(strcmp(temp.name,t->value.text)==0){
		//printf("find:%d\n",i);
		return i;
	}
	while(strcmp(temp.name,t->value.text)!=0)	
	{	
		if(i<hash_num)
			i++;	
		else
			i=0;
		if(i==start)break;
		if(strcmp(temp.name,"null")==0)break;
		temp=Symtable[i];
		if(strcmp(temp.name,t->value.text)==0){
			//printf("find:%d\n",i);
			return i;
		}
		
	}
	return -1;
}

int insert_struct(char* optname,Type type){
	int i;
	for(i=0;i<hash_num;i++){
		if(Structtable[i].used==0){
			Structtable[i].used=1;
			Structtable[i].type=(Type)malloc(sizeof(struct Type_));
			Structtable[i].optname=(char*)malloc(50);
			Structtable[i].type=type;
			strcpy(Structtable[i].optname,optname);
			//printf("insert: %s\n",Structtable[i].optname);
			return i;
		}	
	}
}
int find_struct(char* optname){
	int i;
	for(i=0;i<hash_num;i++){
		if(Structtable[i].used==1 && strcmp(optname,Structtable[i].optname)==0){
			//printf("find: %s\n",Structtable[i].optname);
			return i;
		}
		if(Structtable[i].used==0){
			//WSprintf("no find at: %d  %s\n",i,optname);
			return -1;
		}	
	}
}
int insert_read(){
	Type type = (Type)malloc(sizeof(struct Type_));
	type->kind = BASIC;
	type->u.basic = 0;
	char *r=(char*)malloc(50);
	strcpy(r,"read");
	unsigned int i = hash_pjw(r);
	unsigned int start=i;
	//printf("insert%s%d\n",r,i);
	Symbol temp=Symtable[i];
	if(strcmp(temp.name,"null")==0){
		strcpy(Symtable[i].name,r);
		Symtable[i].type=type;
		Symtable[i].kind=FUNCTION;
		Symtable[i].arg_num=0;
		//printf("insert at%d\n",i);
		return i;
	}
	while(strcmp(temp.name,"null")!=0)	
	{	
		if(i<hash_num)
			i++;	
		else
			i=0;
		if(i==start)break;
		temp=Symtable[i];
		if(strcmp(temp.name,"null")==0){
			strcpy(Symtable[i].name,r);
			Symtable[i].type=type;
			Symtable[i].kind=FUNCTION;
			Symtable[i].arg_num=0;
			return i;
		}
		
	}
	return -1;

}
int insert_write(){
	Type type = (Type)malloc(sizeof(struct Type_));
	type->kind = BASIC;
	type->u.basic = 0;
	char *wr=(char*)malloc(50);;
	strcpy(wr,"write");
	unsigned int i = hash_pjw(wr);
	unsigned int start=i;
	//printf("insert%s%d\n",wr,i);
	Symbol temp=Symtable[i];
	if(strcmp(temp.name,"null")==0){
		strcpy(Symtable[i].name,wr);
		Symtable[i].type=type;
		Symtable[i].kind=FUNCTION;
		Symtable[i].arg_num=1;
		Symtable[i].argument= (Type*)malloc(sizeof(Type) * 1);
		Symtable[i].argument[0]=type;
		//printf("insert at%d\n",i);
		return i;
	}
	while(strcmp(temp.name,"null")!=0)	
	{	
		if(i<hash_num)
			i++;	
		else
			i=0;
		if(i==start)break;
		temp=Symtable[i];
		if(strcmp(temp.name,"null")==0){
			strcpy(Symtable[i].name,wr);
			Symtable[i].type=type;
			Symtable[i].kind=FUNCTION;
			Symtable[i].arg_num=1;
			Symtable[i].argument= (Type*)malloc(sizeof(Type) * 1);
			Symtable[i].argument[0]=type;
			return i;
		}
		
	}
	return -1;
}
