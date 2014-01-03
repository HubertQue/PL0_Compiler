#ifndef BASIC_BLOCK_H
#define BASIC_BLOCK_H

#include "quaternion.h"
#include "middle_code.h"

#include <vector>

class Basic_block{
public:
	void build(std::vector<Quaternion*>&);
	void add_entry_exit(std::vector<Basic_block*> &);
	void print_content(std::ostream &);
	void DAG_optimize();
	void export_code(Middle_code &);
	
	/* Mainly used to build in-out */
	Quaternion* get_head();
	Quaternion* get_tail();

	Basic_block(int);
	
	int id;
private:
	std::vector<Quaternion*> codes;
	std::vector<Basic_block*> in_blocks;
	std::vector<Basic_block*> out_blocks;
};
#endif