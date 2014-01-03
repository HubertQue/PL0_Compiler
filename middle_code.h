#ifndef MIDDLE_CODE_H
#define MIDDLE_CODE_H

#include "quaternion.h"
#include <vector>

class Middle_code{
public:
	void add(Quaternion::op opcode, Symbol_item * src1, Symbol_item * src2, Symbol_item *dst);
	void print_content(std::ostream out);
	std::vector<Quaternion*> & get_codes();
	~Middle_code();
private:
	std::vector<Quaternion*> codes;
};


#endif


















