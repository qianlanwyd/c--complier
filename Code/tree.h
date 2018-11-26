#ifndef TREE_H
#define TREE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdarg.h>



typedef struct Node{
	char *name;
	int line;
	int type;
	union value{
		char *text;
		char *type_name;
		char *id_value;
		int int_value;
		float float_value;
	}value;
	struct Node* child[7];
}Node;

int if_print;
struct Node*	root;
struct Node* insert(char *curname,char *value,int p,int line,int num,...);
void print_tree(Node *h,int dep);
void init_tree(Node *h,int dep);

#endif
