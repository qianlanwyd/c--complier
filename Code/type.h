#ifndef __TYPE_H__
#define __TYPE_H__

#include "tree.h"
typedef struct Type_* Type;
typedef struct FieldList_* FieldList;



struct Type_ {
	enum {BASIC, ARRAY, STRUCTURE} kind;
	union {
		int basic;
		struct {
			Type elem;
			int size;
		} array;
		FieldList structure;
	} u;
	//FieldList head;
};

struct FieldList_ {
	char *name;
	Type type;
	FieldList tail;
};

Type get_type_of_sepecifier(Node* t);
#endif
