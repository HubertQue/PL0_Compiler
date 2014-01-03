#ifndef OPTIMIZER_H
#define OPTIMIZER_H
#include "function_block.h"
#include "middle_code.h"

class Optimizer{
public:
	void build(Middle_code &);
	void DAG_optimize();
	void print_block(std::ostream &);
	void export_code(Middle_code &);
	~Optimizer();
private:
	std::vector<Function_block*> func_blocks;	

};

#endif