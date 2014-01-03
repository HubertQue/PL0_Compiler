#include "object_code.h"
#include "quaternion.h"
#include "utility.h"
#include <fstream>


Object_code::Object_code(Middle_code& _code, Root_symbol_table & _table):codes(_code),table(_table){
	current_table = _table.get_root_table();
};
namespace{

	const std::string exit_process = "ExitProcess";
	const std::string crt_printf= "crt_printf";
	const std::string crt_scanf= "crt_scanf";

	const std::string print_int= "offset print_int";
	const std::string print_char= "offset print_char";
	const std::string print_string= "offset print_string";
	const std::string scanf_int= "offset scanf_int";
	const std::string scanf_char= "offset scanf_char";

	const std::string eax= "EAX";
	const std::string ebx= "EBX";	// only used for array
	const std::string ecx= "ECX";	
	const std::string edx= "EDX";
	
	const std::string ebp= "EBP";
	const std::string esp= "ESP";	
	const std::string edi= "EDI";	
	const std::string esi= "ESI";

	const std::string start= "start";
	
}

/* Add instructions to vector. */
void Object_code::add(Instruction::op opcode , std::string dst, std::string src){
	ins.push_back(Instruction(opcode, dst, src));
}
void Object_code::add(Instruction::op opcode, std::string dst){
	ins.push_back(Instruction(opcode,dst));
}
void Object_code::add(Instruction::op opcode){
	ins.push_back(Instruction(opcode));
}


/* Output instructions .*/
void Object_code::print_ins(std::ostream & out){
	out<<"\n.CODE"<<std::endl;
	std::vector<Instruction>::iterator iter = ins.begin();
	while(iter!=ins.end()){
		iter->print(out);
		iter++;
	}
}
/* Output include head */
void Object_code::print_head(std::ostream &out){
	std::ifstream fs("head.txt");
	char ch;
	while(fs.get(ch))
		out << ch;
	fs.close();
}
/* Output data section */
void Object_code::print_data(std::ostream &out){
	out<<".DATA"<<std::endl;
	out<<"print_int    DB \"%d\",0"<<std::endl;
	out<<"print_char   DB \"%c\",0"<<std::endl;
	out<<"print_string DB \"%s\",0"<<std::endl;
	out<<"scanf_int    DB \"%d\",0"<<std::endl;
	out<<"scanf_char   DB \"%c\",0"<<std::endl;
	table.print_data(out);
}

/* Output entire file including head, data section and text section*/
void Object_code::print_file(std::ostream &out){
	out<<";-----------------"<<std::endl;
	print_head(out);
	print_data(out);
	print_ins(out);
}



	
void Object_code::translate(){
	std::vector<Quaternion*> & code = codes.get_codes();
	std::vector<Quaternion*>::iterator iter = code.begin();
	while(iter!=code.end()){
		switch((*iter)->opcode){
		case Quaternion::ASSIGN: handle_assign((*iter)->dst, (*iter)->src1); break;
		case Quaternion::ARR_LOAD: handle_arr_load((*iter)->src1, (*iter)->src2, (*iter)->dst); break;
//		case Quaternion::ARR_SAVE: handle_arr_save((*iter)->src1, (*iter)->src2, (*iter)->dst); break;
		case Quaternion::ADD:	 handle_add((*iter)->dst, (*iter)->src1, (*iter)->src2); break;			
		case Quaternion::SUB:    handle_sub((*iter)->dst, (*iter)->src1, (*iter)->src2); break;			
		case Quaternion::MUL:    handle_mul((*iter)->dst, (*iter)->src1, (*iter)->src2); break;			
		case Quaternion::DIV:    handle_div((*iter)->dst, (*iter)->src1, (*iter)->src2); break;
		
		case Quaternion::NEG:    handle_neg((*iter)->dst, (*iter)->src1); break;
		case Quaternion::INC:    handle_inc((*iter)->dst); break;
		case Quaternion::DEC:    handle_dec((*iter)->dst); break;

		case Quaternion::CMP:	 handle_cmp((*iter)->src1, (*iter)->src2); break;

		case Quaternion::PROC:   handle_proc((*iter)->dst); break;
		case Quaternion::END:	 handle_endp((*iter)->dst); break;
		case Quaternion::PUSH:	 handle_push((*iter)->dst); break;		
		case Quaternion::PUSH_VAR:	 handle_push_var((*iter)->dst); break;
		case Quaternion::CALL:   handle_call((*iter)->dst); break;
		case Quaternion::LABEL:  handle_label((*iter)->dst);break;
		case Quaternion::READ:	 handle_read((*iter)->dst); break;
		case Quaternion::WRITE:  handle_write((*iter)->dst);break;

		case Quaternion::JE:
		case Quaternion::JNE:
		case Quaternion::JG:
		case Quaternion::JGE:
		case Quaternion::JLE:
		case Quaternion::JL:
		case Quaternion::JMP:
			handle_j((*iter)->opcode,(*iter)->dst); break;

		default:

			break;
		}
		iter++;
	}
}

/*
	Get reference(address) of item. 
	For const:
		return const_name@table_name(int or char) or offset()

*/
std::string Object_code::get_ref(Symbol_item* item){
	if(item->get_kind()==Symbol_item::CONST){
		if(item->get_type()==Symbol_item::STRING){
			return "offset "+item->get_name();
		}else
			return item->get_name();
	}else if(item->get_kind()==Symbol_item::VAR||
		item->get_kind()==Symbol_item::PARA||
		item->get_kind()==Symbol_item::PARA_VAR||
		item->get_kind()==Symbol_item::ARRAY){
		if(item->get_level()==0)	// para and para_var will not be in root proc.
			return item->get_name();
		else if (item->get_level()==current_table->get_level()){
				
			/* if para_var, the para will be a address. */
			if(item->get_kind()==Symbol_item::PARA_VAR){
				add(Instruction::MOV, edi, _EBP(item->get_offset()));
				return "DWORD PTR ["+edi+"]";
			}else
				return _EBP(item->get_offset());
		}else {
			/* Then item level must be higher than current level.
			   And current display region must contain pointer to 
			   item level, which is guaranteed by Parser. */
				
				add(Instruction::MOV, edi , _EBP(4+4*item->get_level()));
			
				int off_set = item->get_offset();
				std::string addr= "";
				if(off_set>=0)
					addr= "DWORD PTR ["+edi+"+"+_S(off_set)+"]";
				else 
					addr= "DWORD PTR ["+edi+_S(off_set)+"]";
				
				/* if para_var, the para will be a address. */
				if(item->get_kind()==Symbol_item::PARA_VAR){
					add(Instruction::MOV, edi, addr);
					return "DWORD PTR ["+edi+"]";
				}else
					return addr;
			
		}			
	}else if(item->get_kind()==Symbol_item::TEMP){
		return _EBP(item->get_offset());	
	}else if(item->get_kind()==Symbol_item::TEMP_ADD){
		add(Instruction::MOV, ebx, _EBP(item->get_offset()));
		return "DWORD PTR["+ebx+"]";
	}	
	else if(item->get_kind()==Symbol_item::TEMP_CON){
		return _S(item->get_value());
	}else if(item->get_kind()==Symbol_item::LABEL)
		return item->get_name();
	else if(item->get_kind()==Symbol_item::FUNC)
		return eax;
	else 
		return item->get_name()+"(ERROR in get_ref)";
}
void Object_code::handle_assign(Symbol_item*dst, Symbol_item*src){
	/* Return value. */
	if(src->get_kind()==Symbol_item::FUNC){
		add(Instruction::MOV, get_ref(dst), eax);
	}
	/* Return assign. */
	else if(dst->get_kind()==Symbol_item::FUNC){
		add(Instruction::MOV, eax, get_ref(src));
		/* Function_level = item_level+1.  */
		if(dst->get_level()+1==current_table->get_level()){	// assign in its own function block.
			add(Instruction::MOV, _EBP(dst->get_offset()), eax);
		}else{
			add(Instruction::MOV, edi , _EBP(4+4*(dst->get_level()+1)));			
			add(Instruction::MOV, "DWORD PTR ["+edi+_S(dst->get_offset())+"]", eax);
		}	
			
	}else{
		add(Instruction::MOV, eax, get_ref(src));
		add(Instruction::MOV, get_ref(dst), eax);
	}	
}
//void Object_code::handle_arr_save(Symbol_item*arr, Symbol_item*idx,Symbol_item*value){
//	add(Instruction::LEA, edx, get_ref(arr));
//	add(Instruction::MOV, ecx, get_ref(idx));
//	add(Instruction::MOV, eax, get_ref(value));
//	add(Instruction::MOV, "DWORD PTR ["+edx+"+"+ecx+"*4]", eax);
//}
void Object_code::handle_arr_load(Symbol_item*arr, Symbol_item*idx,Symbol_item*addr){	
	add(Instruction::LEA, edx, get_ref(arr));
	add(Instruction::MOV, ecx, get_ref(idx));
	add(Instruction::LEA, edx, "DWORD PTR ["+edx+"+"+ecx+"*4]");
	add(Instruction::MOV, _EBP(addr->get_offset()), edx);
}
void Object_code::handle_add(Symbol_item*dst, Symbol_item*src1,Symbol_item*src2){	
	add(Instruction::MOV, eax, get_ref(src1));
	add(Instruction::ADD, eax, get_ref(src2));
	add(Instruction::MOV, get_ref(dst), eax);
}
void Object_code::handle_sub(Symbol_item*dst, Symbol_item*src1,Symbol_item*src2){	
	add(Instruction::MOV, eax, get_ref(src1));
	add(Instruction::SUB, eax, get_ref(src2));
	add(Instruction::MOV, get_ref(dst), eax);
}
void Object_code::handle_mul(Symbol_item*dst, Symbol_item*src1,Symbol_item*src2){	
	add(Instruction::MOV, eax, get_ref(src1));
	add(Instruction::IMUL, eax, get_ref(src2));
	add(Instruction::MOV, get_ref(dst), eax);
}
void Object_code::handle_div(Symbol_item*dst, Symbol_item*src1,Symbol_item*src2){
	add(Instruction::MOV, eax, get_ref(src1));
//	add(Instruction::MOV, ecx, get_ref(src2));
//	add(Instruction::IDIV, ecx);
	std::string divisor;
	if(src2->get_kind()==Symbol_item::TEMP_CON){
		add(Instruction::MOV, ecx, get_ref(src2));
		divisor= ecx;
	}else
		divisor= get_ref(src2);
	add(Instruction::CDQ);
	add(Instruction::IDIV, divisor);
	add(Instruction::MOV, get_ref(dst), eax);
}
void Object_code::handle_neg(Symbol_item*dst, Symbol_item*src){
	add(Instruction::MOV, eax, get_ref(src));
	add(Instruction::NEG, eax);
	add(Instruction::MOV, get_ref(dst),eax);

}
void Object_code::handle_inc(Symbol_item*dst){
	add(Instruction::INC, get_ref(dst));
}
void Object_code::handle_dec(Symbol_item*dst){
	add(Instruction::DEC, get_ref(dst));

}
void Object_code::handle_cmp(Symbol_item* src1,Symbol_item* src2){
	add(Instruction::MOV, eax, get_ref(src1));
	add(Instruction::CMP, eax, get_ref(src2));
}
void Object_code::handle_j(Quaternion::op j_code, Symbol_item* lab){
	switch(j_code){
	case Quaternion::JE: add(Instruction::JE,get_ref(lab)); break;
	case Quaternion::JNE: add(Instruction::JNE,get_ref(lab));break;
	case Quaternion::JG: add(Instruction::JG,get_ref(lab));break;
	case Quaternion::JGE: add(Instruction::JGE,get_ref(lab));break;
	case Quaternion::JL: add(Instruction::JL,get_ref(lab));break;
	case Quaternion::JLE: add(Instruction::JLE,get_ref(lab));break;
	case Quaternion::JMP: add(Instruction::JMP,get_ref(lab));break;
	default:
		add(Instruction::JMP, "-----ERROR");
		break;
	}
}
void Object_code::handle_push(Symbol_item* dst){
	add(Instruction::PUSH, get_ref(dst));
}
void Object_code::handle_push_var(Symbol_item* dst){
	if(dst->get_kind()==Symbol_item::TEMP_ADD){
		add(Instruction::PUSH,_EBP(dst->get_offset()));
	}		
	else{
		add(Instruction::LEA,esi, get_ref(dst));
		add(Instruction::PUSH,esi);
	}
}
/*  */

void Object_code::handle_call(Symbol_item* dst){
	/* Push display region. */
	/* Attention:
		display region should be pushed from stack bottom to top.*/
	int call_level = dst->get_level()+1;	// Call level equals item_level+1
	if(call_level==1){		
		add(Instruction::CALL, dst->get_name());
	}
	else{
		int i;
		if(call_level==current_table->get_level()+1){
			add(Instruction::PUSH, ebp);
			for(i=call_level-3;i>=0;i--){
				add(Instruction::PUSH, _EBP(8+i*4));
			}
			/* Sum happens to be call_level-1. */
		}else{
			for(i= call_level-2;i>=0;i--){
				add(Instruction::PUSH, _EBP(8+i*4));
			}
		}
		add(Instruction::CALL, dst->get_name());
	}		
}

void Object_code::handle_label(Symbol_item* lab){
	add(Instruction::LABEL, get_ref(lab));
}
void Object_code::handle_proc(Symbol_item* proc){
	Symbol_table * table = (Symbol_table *)proc;
	/* This decides current level!!! */
	current_table = table;
	
	/* If it is root proc*/	
	if(table->get_level()==0){
		add(Instruction::LABEL,start );
		add(Instruction::PUSH,ebp);
		add(Instruction::MOV, ebp, esp);
		if(table->get_stack_size()!=0)
		add(Instruction::SUB, esp, _S(table->get_stack_size()));
	}else{
		add(Instruction::PROC,table->get_proc_name());
		add(Instruction::PUSH,ebp);
		add(Instruction::MOV, ebp, esp);
		if(table->get_stack_size()!=0)
		add(Instruction::SUB, esp, _S(table->get_stack_size()));
	}
}
void Object_code::handle_endp(Symbol_item* proc){
	Symbol_table * table = (Symbol_table *)proc;
	/* If it is root proc*/
	if(table->get_level()==0){		
		add(Instruction::PUSH, _S(0));
		add(Instruction::CALL, exit_process);
		add(Instruction::LEAVE);
		add(Instruction::RET,_S(0));
		add(Instruction::END, start);
	}else{
		if(table->get_proc_item()->get_kind()==Symbol_item::FUNC){			
			add(Instruction::MOV, eax,_EBP(table->get_proc_item()->get_offset()));
		}
		add(Instruction::LEAVE);
		add(Instruction::RET, _S(table->get_args_size()));		
		add(Instruction::ENDP,table->get_proc_name());
	}
}

void Object_code::handle_write(Symbol_item* dst){	
	add(Instruction::PUSH, get_ref(dst));
	if(dst->get_type()==Symbol_item::INT){
		add(Instruction::PUSH, print_int);
	}else if(dst->get_type()==Symbol_item::CHAR){
		add(Instruction::PUSH, print_char);
	}else if(dst->get_type()==Symbol_item::STRING){
		add(Instruction::PUSH, print_string);
	}
	add(Instruction::CALL, crt_printf);
	add(Instruction::ADD, esp, _S(8));
}

void Object_code::handle_read(Symbol_item*dst){	
	add(Instruction::LEA,eax, get_ref(dst));
	add(Instruction::PUSH, eax);
	if(dst->get_type()==Symbol_item::INT){
		add(Instruction::PUSH, scanf_int);
	}else if(dst->get_type()==Symbol_item::CHAR){
		add(Instruction::PUSH, scanf_char);
	}
	add(Instruction::CALL, crt_scanf);
	add(Instruction::ADD, esp, _S(8));
}