#ifndef __IR_H__
#define __IR_H__

int aerror;
int serror;
typedef struct Operand_* Operand;
typedef struct InterCodes_* InterCodes;
typedef struct OperandList_* OperandList;
typedef enum {
	false, true
}bool;

struct Operand_ {
	enum { TEMP,LABEL,VARIABLE1, CONSTANT,  FUNCTION1, NUMBER, REF} kind;
	union {
		int temp_no;
		int label_no;
		int var_no;
		int const_val;
		int number;
		char *func_name;
		char *var_name;
	} u;
};

struct OperandList_ {
	Operand op;
	struct OperandList_ *next;
};

struct InterCode {
	enum {icASSIGN, icADD, icSUB, icMUL, icDIV, icLABEL, icRETURN, icGOTO, icIF, icCALL, icREAD, icWRITE, icFUNCTION, icPARAM, icARG, icRSTAR, icDEC, icLSTAR, icREF} kind;
	union {
		struct {Operand right, left;} assign;
		struct {Operand result, op1, op2;} binop;
		struct {Operand op;} singleop;
		struct {Operand result, op1, op2; int optype;} ifop;
	} u;
};

struct InterCodes_ {
	struct InterCode code;
	InterCodes next;
};
int temp_index ;
int label_index ;
int var_index ;
InterCodes iclist;

void savefile(char *filename);
void readfile(char *filename);
void cleanhead();

void translate_Program(struct Node *root);
#endif


