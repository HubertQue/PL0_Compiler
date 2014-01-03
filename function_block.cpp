#include "function_block.h"
#include <iostream>

Function_block::Function_block(int _id):id(_id){
	func_item=NULL;
}

Function_block::~Function_block(){
	std::vector<Basic_block*>::iterator iter = basic_blocks.begin();
	while(iter!=basic_blocks.end()){
		delete(*iter);
		iter++;
	}
}

void Function_block::build(std::vector<Quaternion*>& codes){
	std::vector<Quaternion*>::iterator iter = codes.begin();
	std::vector<Quaternion*> tmp_codes;
	func_item =((Symbol_table* ) (*iter)->dst)->get_proc_item();

	int count = 0;
	while(iter!=codes.end()){
		 Quaternion::op opcode = (*iter)->opcode;
	
		/* when found head of a basic block : LABEL , PUSH , PUSH_VAR , READ, WRITE 
		   build a new basic block and clear temp set, then push current code to temp set.
		*/
		 if( (opcode==Quaternion::LABEL || opcode == Quaternion::PUSH ||opcode == Quaternion::PUSH_VAR
			|| opcode == Quaternion::READ || opcode == Quaternion::WRITE ) ){
			//Save all tmp set as a new block			 
			if(!tmp_codes.empty()){
				Basic_block * b_block = new Basic_block(++count);
				b_block->build(tmp_codes);
				basic_blocks.push_back(b_block);
				tmp_codes.clear();
			}
			//Handle special block.
			while((*iter)->opcode == Quaternion::PUSH ||
				(*iter)->opcode == Quaternion::PUSH_VAR ||
				(*iter)->opcode == Quaternion::READ ||
				(*iter)->opcode == Quaternion::WRITE ||
				(*iter)->opcode == Quaternion::CALL ){
				// If READ or WRITE save as a new block;
				if((*iter)->opcode==Quaternion::READ ||
					(*iter)->opcode==Quaternion::WRITE||
					(*iter)->opcode == Quaternion::CALL ){
					while((*iter)->opcode == Quaternion::READ || 
						(*iter)->opcode == Quaternion::WRITE||
						(*iter)->opcode == Quaternion::CALL  ){
						tmp_codes.push_back(*iter);
						iter++; 
					}				
					Basic_block * b_block = new Basic_block(++count);
					b_block->build(tmp_codes);
					basic_blocks.push_back(b_block);
					tmp_codes.clear();					
				}
				// If PUSH or PUSH_VAR , save as a new block. 
				if ((*iter)->opcode == Quaternion::PUSH ||(*iter)->opcode == Quaternion::PUSH_VAR ){

					while((*iter)->opcode == Quaternion::PUSH ||
						(*iter)->opcode == Quaternion::PUSH_VAR ||
						(*iter)->opcode ==Quaternion::CALL ){
						tmp_codes.push_back(*iter);
						iter++; 
					}
					if ((*iter)->opcode ==Quaternion::ASSIGN && (*iter)->src1->get_kind()==Symbol_item::FUNC){	// include assign statement.				
						tmp_codes.push_back(*iter);	
						iter++;
					}
					// new block . 
					Basic_block * b_block = new Basic_block(++count);
					b_block->build(tmp_codes);
					basic_blocks.push_back(b_block);
					tmp_codes.clear();
				}
			}
			// push current code into temp set.
			tmp_codes.push_back(*iter);
			iter++;		
		 }
		 /* when found tail of a basic block , 
		   push current code to temp set and build a new basic block, lastly clear temp set.
		 */
		 else if( (opcode>= Quaternion::JE && opcode<=Quaternion::JMP)
			&& !tmp_codes.empty() ){
			Basic_block * b_block = new Basic_block(++count);
			tmp_codes.push_back(*iter);
			b_block->build(tmp_codes);			
			basic_blocks.push_back(b_block);
			tmp_codes.clear();
			iter++;
		 }else{
			tmp_codes.push_back(*iter);
			iter++;
		 }
	}// end while

	if(!tmp_codes.empty()){
		Basic_block * b_block = new Basic_block(++count);
		b_block->build(tmp_codes);
		basic_blocks.push_back(b_block);
	}
	build_in_out();
}

/* Private method called by build()*/
void Function_block::build_in_out(){
	std::vector<Basic_block*>::iterator iter = basic_blocks.begin();
	while(iter!=basic_blocks.end()){
		(*iter)->add_entry_exit(basic_blocks);
		iter++;
	}
}


void Function_block::print_content(std::ostream & out){
	out<<"Function block: "<<id<<std::endl;
	std::vector<Basic_block*>::iterator iter = basic_blocks.begin();
	while(iter!=basic_blocks.end()){
		(*iter)->print_content(out);
		iter++;
	}
}

void Function_block::DAG_optimize(){
	std::vector<Basic_block*>::iterator iter = basic_blocks.begin();
	while(iter!=basic_blocks.end()){
		(*iter)->DAG_optimize();
		iter++;
	}
}

void Function_block::export_code(Middle_code & mid_code){
	std::vector<Basic_block*>::iterator iter = basic_blocks.begin();
	while(iter!=basic_blocks.end()){
		(*iter)->export_code(mid_code);
		iter++;
	}
}