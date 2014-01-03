#include "instruction.h"
using::std::endl;

Instruction::Instruction(op _opcode, std::string _src1, std::string _src2):opcode(_opcode),src1(_src1),src2(_src2){
	// Blank
}	
Instruction::Instruction(op _opcode, std::string _src1):opcode(_opcode),src1(_src1){
	src2= "";
}
Instruction::Instruction(op _opcode):opcode(_opcode){
	src1= "";
	src2= "";
}

void Instruction::print(std::ostream & out){
	std::string tmp_src1= src1;
	if(src2!="") 
		tmp_src1= src1+",";

	switch(opcode){
	/* Special operation */
	case Instruction::LABEL: out<<src1<<":"<<endl; break;

	case Instruction::PROC:	out<<src1<<"\tPROC"<<endl; break;
	case Instruction::ENDP: out<<src1<<"\tENDP\n"<<endl; break;
	case Instruction::END:  out<<"END "<<src1<<endl; break;

	default:
		out<<"\t"<<get_op()<<" "<<tmp_src1<<" "<<src2<<endl; break;

// 	/* 2 operands */
// 	case Instruction::MOV:		
// 	case Instruction::ADD:
// 	case Instruction::IMUL:
// 	case Instruction::LEA:
// 	case Instruction::CMP:
// 	
// 	/* 1 operand */
// 	case Instruction::CDQ:
// 	case Instruction::IDIV:
// 	case Instruction::NEG:
// 	case Instruction::INC:
// 	case Instruction::DEC:
// 	case Instruction::JE:
// 	case Instruction::JNE:
// 	case Instruction::JG:
// 	case Instruction::JGE:
// 	case Instruction::JL:
// 	case Instruction::JLE:
// 	case Instruction::JMP:
// 	case Instruction::PUSH:
// 	case Instruction::CALL:
// 	case Instruction::RET:
// 		case 
// 
// 	/* 0 operand */
// 	case Instruction::LEAVE:
// 	case Instruction::NOP:
	}
}

std::string Instruction:: get_op(){
	switch(opcode){
	case Instruction::MOV: return std::string("MOV"); break;
	case Instruction::ADD: return std::string("ADD"); break;
	case Instruction::SUB: return std::string("SUB"); break;
	case Instruction::IMUL: return std::string("IMUL"); break;
	case Instruction::CDQ: return std::string("CDQ"); break;
	case Instruction::IDIV: return std::string("IDIV"); break;
	
	case Instruction::LEA: return std::string("LEA"); break;
	case Instruction::NEG: return std::string("NEG"); break;
	case Instruction::INC: return std::string("INC"); break;
	case Instruction::DEC: return std::string("DEC"); break;

	case Instruction::CMP: return std::string("CMP"); break;
	case Instruction::JE: return std::string("JE"); break;
	case Instruction::JNE: return std::string("JNE"); break;
	case Instruction::JG: return std::string("JG"); break;
	case Instruction::JGE: return std::string("JGE"); break;
	case Instruction::JL: return std::string("JL"); break;
	case Instruction::JLE: return std::string("JLE"); break;
	case Instruction::JMP: return std::string("JMP"); break;

	case Instruction::PUSH: return std::string("PUSH"); break;
	case Instruction::POP: return std::string("POP"); break;
	case Instruction::CALL: return std::string("CALL"); break;
	case Instruction::LEAVE: return std::string("LEAVE"); break;
	case Instruction::RET: return std::string("RET"); break;

	case Instruction::LABEL: return std::string("LABEL"); break;
	case Instruction::PROC: return std::string("PROC"); break;
	case Instruction::ENDP: return std::string("ENDP"); break;
		
	case Instruction::END: return std::string("END"); break;
	case Instruction::NOP: return std::string("NOP"); break;

	default:
		return std::string("-----INVALID");
	}
}