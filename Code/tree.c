#include "tree.h"

struct Node* insert(char *curname,char *value,int p,int line,int num,...){
	struct Node* cur = (struct Node*)malloc(sizeof(struct Node));
	cur->name=curname;
	cur->type=p;
	cur->line=line;
	char *id = malloc(sizeof(char)*40);
	strcpy(id,value);
	cur->value.text=id;
	int j=0;
	for(j;j<7;j++){
		(cur->child)[j] = NULL;
	}
	if(num<=0){
		return cur;
	}
	va_list ap;
	va_start(ap,num);
	int i=0;
	for(i;i<num;i++){
		(cur->child)[i] = (Node *)va_arg(ap,Node *);
	}
	va_end(ap);
	return  cur;
	//printf("%s: %d\n",cur->name,cur->line);
}


void print_tree(Node *h,int dep){
	if(h==NULL)return;
	int i=0;
	for(i;i<dep;i++)
		printf("  ");
	if(h->type==0)
		printf("%s: (%d)\n",h->name,h->line);
	else if(h->type==1)
		printf("%s \n",h->name);
	else if(h->type==2){
		char *temp = malloc(sizeof(char)*40);
		strcpy(temp,h->value.text);
		h->value.type_name=temp;
		printf("%s: %s\n",h->name,h->value.type_name);
	}
	else if(h->type==3){
		char *temp = malloc(sizeof(char)*40);
		strcpy(temp,h->value.text);
		h->value.id_value=temp;
		printf("%s: %s\n",h->name,h->value.id_value);
	}
	else if(h->type==4){
		char *temp = malloc(sizeof(char)*40);
		strcpy(temp,h->value.text);
		sscanf(temp,"%i",&(h->value.int_value));
		printf("%s: %d\n",h->name,h->value.int_value);
	}
	else if(h->type==5){
		char *temp = malloc(sizeof(char)*40);
		strcpy(temp,h->value.text);
		h->value.float_value=atof(temp);
		printf("%s: %f\n",h->name,h->value.float_value);
	}
	for(i=0;i<7;i++){
		print_tree(h->child[i],dep+1);
	}
	
}
void init_tree(Node *h,int dep){
	if(h==NULL)return;
	int i=0;
	if(h->type==2){
		char *temp = malloc(sizeof(char)*40);
		strcpy(temp,h->value.text);
		h->value.type_name=temp;
		//printf("%s: %s\n",h->name,h->value.type_name);
	}
	else if(h->type==3){
		char *temp = malloc(sizeof(char)*40);
		strcpy(temp,h->value.text);
		h->value.id_value=temp;
		//printf("%s: %s\n",h->name,h->value.id_value);
	}
	else if(h->type==4){
		char *temp = malloc(sizeof(char)*40);
		strcpy(temp,h->value.text);
		sscanf(temp,"%i",&(h->value.int_value));
		//printf("%s: %d\n",h->name,h->value.int_value);
	}
	else if(h->type==5){
		char *temp = malloc(sizeof(char)*40);
		strcpy(temp,h->value.text);
		h->value.float_value=atof(temp);
		//printf("%s: %f\n",h->name,h->value.float_value);
	}
	for(i=0;i<7;i++){
		init_tree(h->child[i],dep+1);
	}
	
}



