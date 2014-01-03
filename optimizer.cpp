#include "optimizer.h"

Optimizer::	~Optimizer(){
	std::vector<Function_block*>::iterator iter = func_blocks.begin();
	while(iter!=func_blocks.end()){
		delete(*iter);
		iter++;
	}
}
void Optimizer::build(Middle_code & mid_codes){
	std::vector<Quaternion*> & code = mid_codes.get_codes();
	std::vector<Quaternion*>::iterator iter = code.begin();
	std::vector<Quaternion*> tmp_codes;
	int count = 0;
	while(iter!=code.end()){
		tmp_codes.push_back(*iter);
		if((*iter)->opcode== Quaternion::END){	// If found end of a block.			
			Function_block * func_block = new Function_block(++count);
			func_block->build(tmp_codes);
			tmp_codes.clear();
			func_blocks.push_back(func_block);
		}
		iter++;
	}
}

void Optimizer::DAG_optimize(){
	std::vector<Function_block*>::iterator iter = func_blocks.begin();
	while(iter!=func_blocks.end()){
		(*iter)->DAG_optimize();
		iter++;
	}
}

void Optimizer::print_block(std::ostream & out){
	std::vector<Function_block*>::iterator iter = func_blocks.begin();
	while(iter!=func_blocks.end()){
		(*iter)->print_content(out);
		iter++;
	}
}

void Optimizer::export_code(Middle_code & mid){
	mid.get_codes().clear();
	std::vector<Function_block*>::iterator iter = func_blocks.begin();
	while(iter!=func_blocks.end()){
		(*iter)->export_code(mid);
		iter++;
	}
}

