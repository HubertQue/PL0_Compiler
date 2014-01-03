#include "middle_code.h"
#include <iostream>


void Middle_code::add(Quaternion::op opcode, Symbol_item * src1,
				 Symbol_item * src2, Symbol_item *dst)
{	
	codes.push_back(new Quaternion(opcode, src1, src2, dst));
}

void Middle_code::print_content(std::ostream out){
	std::vector<Quaternion*>::iterator iter = codes.begin();
	out<<"\n----------------\n\tOP\tSRC1\tSRC2\tDST"<<std::endl;
	while(iter!=codes.end()){
		(*iter)->print(out);
		iter++;
	}
}


Middle_code::~Middle_code(){
	std::vector<Quaternion*>::iterator iter = codes.begin();
	while(iter!=codes.end()){
		delete (*iter);
		iter++;
	}
}


/* Used for translate and optimize. */
std::vector<Quaternion* > & Middle_code::get_codes(){
	return codes;
}