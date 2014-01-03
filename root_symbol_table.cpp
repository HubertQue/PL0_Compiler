#include "root_symbol_table.h"
#include "symbol_table.h"
#include <iostream>
#include <assert.h>
#include <sstream>

Root_symbol_table:: Root_symbol_table(){
	Symbol_item * root = new Symbol_item("main@",Symbol_item::PROC, Symbol_item::VOID, 0);
	root_table = new Symbol_table(root, NULL, 0);
	current_table = root_table;
}

Root_symbol_table:: ~Root_symbol_table(){
	if(root_table!=NULL) delete root_table;
}

Symbol_table *  Root_symbol_table::get_root_table(){
	return root_table;
}
Symbol_table * Root_symbol_table::get_current_table(){
	return current_table;
}

/* Search along father up to the root.
   Return NULL if not found . */
Symbol_item *Root_symbol_table:: search(std::string name){
	Symbol_table * tmp = current_table;
	Symbol_item * item = NULL;
	while(tmp!=NULL){
		item = tmp->get_item(name);
		if(item!=NULL) return item;
		tmp = tmp->father_table;
	}
	return item;
}

/* Insert new Symbol to Symbol Table if no
   item with same name already exists.   */
Symbol_item * Root_symbol_table:: insert(std::string name, Symbol_item::symbol_kind kind, Symbol_item::symbol_type type, int value){
	
	/* Then insert symbol item into current table */
	Symbol_item * item = NULL; 
	item = new Symbol_item(name, kind, type, value);
	if(! current_table->put_item(item))
		return NULL;
	
	/* If inserting a PROC of FUNC , create a new Symbol Table
		and change current_table pointer to point to it .*/
	if(kind== Symbol_item::FUNC || kind==Symbol_item::PROC){
		current_table =  current_table->add_child(item);
	}
	return item;
}

/* Search table recursively. ONLY used in get_arg_list.
 This should never return NULL.*/
Symbol_table * Root_symbol_table::search_table(std::string name){
	Symbol_table * tmp = current_table;
	Symbol_table * table = NULL;
	while(tmp!=NULL){
		table  = tmp->get_table(name);
		if(table !=NULL) break ;
		tmp = tmp->father_table;
	}
	assert(table!=NULL); //This should not return NULL
	return table;
}

/* Find function. ONLY used to check return-assign expression. */
bool Root_symbol_table::find_function(std::string name){
	Symbol_table * tmp  = current_table;
	while(tmp!=NULL){
		if(tmp->get_proc_name()==name)
			return true;
		tmp = tmp->father_table;
	}
	return false;
}

std::vector<Symbol_item *> *  Root_symbol_table::get_arg_list(std::string id){
	Symbol_table * table = search_table(id);
	return table->get_arg_list();
}
void Root_symbol_table::back(){
	if(current_table->father_table!=NULL)
		current_table = current_table->father_table;
}


/*
	Generate a temp symbol. 
	KIND: TEMP, TEMP_CON
	TYPE: INT, CHAR;
*/
Symbol_item* Root_symbol_table::get_temp(Symbol_item::symbol_kind kind , Symbol_item::symbol_type type){
	Symbol_item *temp =NULL;
	static int count = 1;
	std::stringstream name;
	name<<"_t"<<count;
	temp = new Symbol_item(name.str(), kind , type, 0);
	current_table->put_item(temp);
	count++;
	assert(temp!=NULL);
	return temp;
}

Symbol_item * Root_symbol_table::get_label(){
	static  int count = 1;
	std::stringstream name;
	name<<"_L"<<count;
	
	Symbol_item * label =NULL;
	label = new Symbol_item(name.str(),Symbol_item::LABEL, Symbol_item::VOID, count );
	current_table->put_item(label);
	count++;
	assert(label!=NULL);
	return label;	
}

std::string Root_symbol_table::get_current_name(){
	return current_table->get_proc_name();
}

void Root_symbol_table::print_content(std::ostream out){
	out<<"\n----------------\n\tNAME\tKIND\tTYPE\tVALUE\tSIZE\tOFFSET"<<std::endl;
	root_table->print_table(out);
}
void Root_symbol_table::cal_offset(){
	root_table->cal_offset();
}
void Root_symbol_table::rename(){
	root_table->rename();
}
void Root_symbol_table::print_data(std::ostream & out){
	root_table->print_data(out);
}