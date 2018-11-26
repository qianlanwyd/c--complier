#ifndef __SYMTABLE_H__
#define __SYMTABLE_H__
#include "type.h"
#define hash_num  16384


typedef struct Symbol_
{
	char* name;
	Type type;
	enum {VARIABLE,FUNCTION} kind;
	Type* argument;
	int arg_num;
}Symbol;

typedef struct StructTable_
{
	char *optname;
	Type type;
	int used;
}StructTable;
struct Symbol_  Symtable[hash_num];
struct StructTable_  Structtable[hash_num];
int insert_hash_var(Node* t,Type type,int kind);
int insert_hash_fun1(Node* t,Type type,int kind);
int insert_hash_fun2(Node* t,Type type,int kind);
int insert_read();
int insert_write();
int find_hash(Node* t);
int insert_struct(char* optname,Type type);
int find_struct(char* optname);
#endif
