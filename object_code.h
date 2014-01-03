#ifndef OBJECT_CODE_H
#define OBJECT_CODE_H
#include "instruction.h"
#include "middle_code.h"
#include "root_symbol_table.h"

#include <vector>

class Object_code{
public:
	void print_ins(std::ostream &);
	void print_data(std::ostream &);
	void print_head(std::ostream &);
	void print_file(std::ostream &);
	void translate();
	Object_code(Middle_code&, Root_symbol_table&);
	
private:
	std::vector<Instruction> ins;	
	void add(Instruction::op, std::string, std::string);
	void add(Instruction::op, std::string);
	void add(Instruction::op);
	
	Middle_code & codes;
	Root_symbol_table & table;		// Only print_data() is used here.

	Symbol_table * current_table;	// Provides level information 

	std::string get_ref(Symbol_item*);
	void handle_assign(Symbol_item*, Symbol_item*);
	void handle_arr_save(Symbol_item*, Symbol_item*,Symbol_item*);
	void handle_arr_load(Symbol_item*, Symbol_item*,Symbol_item*);
	void handle_add(Symbol_item*, Symbol_item*,Symbol_item*);
	void handle_sub(Symbol_item*, Symbol_item*,Symbol_item*);
	void handle_mul(Symbol_item*, Symbol_item*,Symbol_item*);
	void handle_div(Symbol_item*, Symbol_item*,Symbol_item*);
	void handle_neg(Symbol_item*, Symbol_item*);
	void handle_inc(Symbol_item*);
	void handle_dec(Symbol_item*);
	void handle_cmp(Symbol_item*,Symbol_item*);
	void handle_j(Quaternion::op, Symbol_item*);
	void handle_push(Symbol_item*);
	void handle_push_var(Symbol_item* );
	void handle_call(Symbol_item*);
	void handle_label(Symbol_item*);
	void handle_proc(Symbol_item*);
	void handle_func(Symbol_item*);
	void handle_endp(Symbol_item*);
	void handle_write(Symbol_item*);
	void handle_read(Symbol_item*);


};



#endif