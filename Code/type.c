#include <string.h>
#include "type.h"


Type get_type_of_sepecifier(Node* t){
	t=t->child[0];
	if (strcmp(t->name,"TYPE") == 0)	//	int|float
	{	
		Type type = (Type)malloc(sizeof(struct Type_));
		type->kind = BASIC;
		if (strcmp(t->value.text,"int") == 0)
			type->u.basic = 0;
		else
			type->u.basic = 1;
		return type;	
	}
	if(strcmp(t->name,"StructSpecifier") == 0)//struct
	{
		Type type = (Type)malloc(sizeof(struct Type_));
		type->kind = STRUCTURE;
		return type;
	}
}

