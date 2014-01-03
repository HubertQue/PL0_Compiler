#include "basic_block.h"
#include "symbol_table.h"
#include "DAG_graph.h"
#include <iostream>

Basic_block::Basic_block(int _id):id(_id){

}

void Basic_block::build(std::vector<Quaternion*>& _codes){
	std::vector<Quaternion*>::iterator iter = _codes.begin();
	while(iter!= _codes.end()){
		codes.push_back(*iter);
		iter++;
	}
}

void Basic_block:: add_entry_exit(std::vector<Basic_block*> & blocks){
	std::vector<Basic_block*>::iterator iter = blocks.begin();
	if (blocks.size()==1) return;
	Quaternion * cur_head = get_head();
	Quaternion * cur_tail = get_tail();
	while(iter!=blocks.end()){
		if((*iter)==this){
			if(iter==blocks.begin()&&iter+1!=blocks.end()){
				if(cur_tail->opcode!=Quaternion::JMP)	// JMP skip next block.
					out_blocks.push_back(*(iter+1));
			}
			else if(iter+1==blocks.end()){
				if((*(iter-1))->get_tail()->opcode!=Quaternion::JMP)
					in_blocks.push_back(*(iter-1));// as iter!=begin, iter -1 must exist.
			}else{
				if((*(iter-1))->get_tail()->opcode!=Quaternion::JMP)
					in_blocks.push_back(*(iter-1));				
				if(cur_tail->opcode!=Quaternion::JMP)
					out_blocks.push_back(*(iter+1));
			}
		}
		if(cur_head->opcode==Quaternion::LABEL){			
			Quaternion * tail = (*iter)->get_tail();
			if (tail->opcode >= Quaternion::JE && tail->opcode <= Quaternion::JMP&&tail->dst== cur_head->dst){
				in_blocks.push_back(*iter);
			}
		}
		if(cur_tail->opcode >= Quaternion::JE && cur_tail->opcode <= Quaternion::JMP){
			Quaternion * head = (*iter)->get_head();
			if(head->dst==cur_tail->dst){
				out_blocks.push_back(*iter);
			}
		}

		iter++;
	}
}
Quaternion* Basic_block:: get_head(){
	return codes[0];
}
Quaternion* Basic_block:: get_tail(){
	return codes[codes.size()-1];
}

void Basic_block::print_content(std::ostream & out){
	out<<"Basic block: "<<id<<" in: ";
	std::vector<Basic_block*>::iterator b_iter = in_blocks.begin();
	while(b_iter!=in_blocks.end()){
		out<<(*b_iter)->id<<" ";		
		b_iter++;
	}
	out<<" out: ";

	b_iter= out_blocks.begin();
	while(b_iter!=out_blocks.end()){
		out<<(*b_iter)->id<<" ";		
		b_iter++;
	}
	out<<std::endl;

	std::vector<Quaternion*>::iterator iter = codes.begin();
	while(iter!= codes.end()){
		(*iter)->print(out);
		iter++;
	}
}

void Basic_block::DAG_optimize(){
	print_content(std::cout);
	DAG_graph dag(codes);
	dag.build();
//	dag.regenerate();
	print_content(std::cout);
}

void Basic_block::export_code(Middle_code & mid){
	std::vector<Quaternion*>& mid_code = mid.get_codes();
	std::vector<Quaternion*>::iterator iter = codes.begin();
	while(iter!= codes.end()){
		mid_code.push_back(*iter);
		iter++;
	}
}