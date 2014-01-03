#ifndef INSTRUCTION_H
#define INSTRUCTION_H
#include <string>
#include <iostream>

class Instruction{
public:
	enum op {
		MOV,
		ADD,
		SUB,
		IMUL,

		CDQ,	// for idiv
		IDIV,
		
		LEA,	// For var para.

		NEG,
		INC,
		DEC,

		CMP,
		JE,
		JNE,
		JG,
		JGE,
		JL,
		JLE,
		JMP,

		PUSH,
		POP,
		CALL,
		LEAVE,
		RET,	// or retn ?
		
		LABEL,
		PROC,
		ENDP,
		
		END, 
		NOP
	};

	op opcode;
	std::string src1;
	std::string src2;
	Instruction(op, std::string, std::string);
	Instruction(op, std::string);	
	Instruction(op);
	void print(std::ostream &);
private:
	std::string get_op();
};





#endif