#ifndef QUATERNION_H
#define QUATERNION_H
#include "symbol_item.h"
class Quaternion{
public:
	enum op{
		ASSIGN,
		//ARR_SAVE,	// SRC1[SRC2]=DST
		ARR_LOAD,	// DST=&SRC1[SRC2]	DST is the address.
		
		ADD,
		SUB,
		MUL,
		DIV,
		NEG,
		INC,	// ++
		DEC,	//	--

		CMP,

		JE,
		JNE,
		JG,
		JGE,
		JL,
		JLE,
		JMP,
		
		PUSH,
		PUSH_VAR,

		CALL,

		LABEL, 
		READ,
		WRITE,

		PROC,	// PROC null null table* (saved table* here)
		END,

		NOP,

	};

	Quaternion(op, Symbol_item* , Symbol_item* , Symbol_item*);
	void print(std::ostream);

	op opcode;
	Symbol_item * dst;
	Symbol_item * src1;
	Symbol_item * src2;

	static std::string get_op_name(Quaternion::op);
private:
};
#endif