#include "quaternion.h"
#include "symbol_table.h"
#include <iostream>
#include <sstream>
#include <string>
Quaternion::Quaternion(Quaternion::op _opcode, Symbol_item * _src1, Symbol_item* _src2, Symbol_item * _dst){
	opcode = _opcode;
	dst = _dst;
	src1= _src1;
	src2 = _src2;
}

std::string Quaternion::get_op_name(Quaternion::op opcode){
	
	std::string op_name= "";
	switch(opcode){	
	case ASSIGN:	op_name= "ASSIGN"; break;
	case ARR_LOAD:	op_name= "ARR_LOAD"; break;
		// 	case ARR_SAVE:	op_name= "ARR_SAVE"; break;
		
	case ADD:		op_name= "ADD"; break;
	case SUB:		op_name= "SUB"; break;
	case MUL:		op_name= "MUL"; break;
	case DIV:		op_name= "DIV"; break;
	case NEG:		op_name= "NEG"; break;
	case INC:		op_name= "INC"; break;
	case DEC:		op_name= "DEC"; break;
		
	case CMP:		op_name= "CMP"; break;
	case JE:		op_name= "JE";  break;
	case JNE:		op_name= "JNE"; break;
	case JG:		op_name= "JG";  break;
	case JGE:		op_name= "JGE"; break;
	case JL:		op_name= "JL";	break;
	case JLE:		op_name= "JLE"; break;
		
	case JMP:		op_name= "JMP"; break;
		
	case PUSH_VAR:		op_name= "PUSH_VAR"; break;
	case PUSH:		op_name= "PUSH"; break;
	case CALL:		op_name= "CALL"; break;
		
	case LABEL:		op_name= "LABEL"; break; 
	case READ:		op_name = "READ"; break;
	case WRITE:		op_name = "WRITE"; break;
		
	case PROC:		op_name = "PROC"; break;
	case END:		op_name = "END"; break;
		
	case NOP:		op_name = "NOP"; break;
		
	default:
		break;
		
	}
	return op_name;
}

void Quaternion::print(std::ostream out){
	std::string op_name = get_op_name(opcode);
	std::stringstream ins;
	ins<<"\t"<<op_name<<"\t";


	if(src1==NULL)
		ins<<"-\t";
	else if(src1->get_kind()==Symbol_item::TEMP_CON)
		ins<<src1->get_value()<<"\t";
	else if(src1->get_type()==Symbol_item::STRING)
		ins<<"\""<<src1->get_str()<<"\"\t";
	else 
		ins<<src1->get_name()<<"\t";

	if(src2==NULL)
		ins<<"-\t";
	else if(src2->get_kind()==Symbol_item::TEMP_CON)
		ins<<src2->get_value()<<"\t";
	else if(src2->get_type()==Symbol_item::STRING)
		ins<<"\""<<src2->get_str()<<"\"\t";
	else 
		ins<<src2->get_name()<<"\t";

	if(opcode==PROC||opcode==END){
		ins<< ((Symbol_table *)dst)->get_proc_name()<<"\t";
	}
	else if(dst==NULL)
		ins<<"-\t";
	else if(dst->get_kind()==Symbol_item::TEMP_CON)
		ins<<dst->get_value()<<"\t";
	else if(dst->get_type()==Symbol_item::STRING)
		ins<<"\""<<dst->get_str()<<"\"\t";
	else 
		ins<<dst->get_name()<<"\t";
	/*OP SRC1 SRC2 DST */
	out<<ins.str()<<std::endl;

}



















