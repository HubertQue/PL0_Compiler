#ifndef ROOT_SYMBOL_TABLE_H
#define ROOT_SYMBOL_TABLE_H
#include "symbol_table.h"
#include <vector>
#include <map>

typedef std::map<std::string, Symbol_item> SYMBOL_MAP;
class Root_symbol_table{
public:
	Root_symbol_table();
	~Root_symbol_table();

	Symbol_item * search(std::string);
	Symbol_item * insert(std::string name, Symbol_item::symbol_kind kind, Symbol_item::symbol_type type, int value);
	void back();
	void print_content(std::ostream out);

	Symbol_item * get_temp(Symbol_item::symbol_kind kind, Symbol_item::symbol_type type);
	Symbol_item * get_label();	

	bool Root_symbol_table::find_function(std::string);
	std::vector<Symbol_item *> * get_arg_list(std::string);
	std::string get_current_name();
	Symbol_table * get_root_table();
	Symbol_table * get_current_table();


	// Used in object code generation.
	void cal_offset();
	// !!! After called this. search() method will not work. As the map use previous name as index.
	void rename();
	void print_data(std::ostream &);
private:

	
	Symbol_table * search_table(std::string);
	Symbol_table * current_table ;
	Symbol_table * root_table ;
};


#endif