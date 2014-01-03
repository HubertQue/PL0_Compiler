#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H
#include "symbol_item.h"
#include <vector>
#include <map>

class Symbol_table{
public:
	Symbol_table * father_table;

	Symbol_table(Symbol_item * proc, Symbol_table * father, int level );
	~Symbol_table();
	
	Symbol_item * get_item (std::string name);
	bool put_item (Symbol_item * item);	
	Symbol_table * add_child(Symbol_item * proc);
	Symbol_table * get_table(std:: string name);
	void print_table(std::ostream out);
	std::vector<Symbol_item *> * get_arg_list();	
	Symbol_item * get_proc_item();
	std::string get_proc_name();

	// Method below should only be called after Parsing.

	// Calculate offset of VAR , PARA, PARA_VAR , ARRAY based on base pointer(ebp). 
	void cal_offset();
	// Rename identifiers.
	void rename();	
	// Print .data for asm file.
	void print_data(std::ostream);
	int get_stack_size();
	int get_args_size();
	int get_level();

private:
	typedef std::map<std::string, Symbol_item * > SYMBOL_MAP;
	typedef std::map<std::string, Symbol_table * > TABLE_MAP;
	
	//Item which represents this table.
	Symbol_item * proc;
	std::vector<Symbol_item * > seq_list; // Sequence list to save all local variables to calculate offset.
	SYMBOL_MAP symbol_map;
	TABLE_MAP table_map;
	
	int level;	// 0 for root .
	// Data size in stack ( offset esp should subtract). Reset every time cal_offset() method is called. 
	int stack_size;	
	// Sum of arguments size and display size in stack (size should be added to esp after function return)
	int args_size;
};


#endif