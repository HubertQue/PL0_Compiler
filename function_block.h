#ifndef FUNCTION_BLOCK_H
#define FUNCTION_BLOCK_H
#include "basic_block.h"
#include "quaternion.h"
#include "symbol_table.h"

class Function_block{
public:	
	void build(std::vector<Quaternion*>&);
	void print_content(std::ostream &);
	void DAG_optimize();

	void export_code(Middle_code &);
	Function_block(int);
	~Function_block();
private:
	void build_in_out();

	std::vector<Basic_block*> basic_blocks;
	Symbol_item* func_item;
	int id;
};
#endif