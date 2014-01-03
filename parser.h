#ifndef PARSER_H
#define PARSER_H
#include "tokenizer.h"
#include "root_symbol_table.h"
#include "middle_code.h"
#include "error.h"
#include <vector>
class Parser
{ 
public:
	Parser(Tokenizer& _tokenizer, Root_symbol_table & _symbol_talbe, Middle_code & _middle_code, Error& _error):tokenizer(_tokenizer), symbol_table(_symbol_talbe), middle_code(_middle_code), error(_error){
	};
	void parse();

private:
	Token current;
	Tokenizer& tokenizer;
	Root_symbol_table& symbol_table;
	Middle_code& middle_code;
	Error& error;

	void next();
	int fresh;
	void expect(Token::Token_type, std::string);
	void next_line();
	void gen(Quaternion::op , Symbol_item * , Symbol_item * , Symbol_item *);
	Symbol_item * get(std::string);
	bool check_type(Symbol_item*, Symbol_item*);

	/* Gets next token before calling the method , which also
		means it often gets next token in the end of a method. */
	void program();
	void const_head();
	void const_def();
	void var_head();
	void var_def();
	void var_type(std::vector<std::string> *);


	void proc_head();
	void arg_list();
	void func_head();


	/* Compound statement */
	void com_stat();	
	void stat();
	
	void assign_stat(std::string);
	void if_stat();
	Token::Token_type condition();
	void repeat_stat();
	void for_stat();
	void write_stat();
	void read_stat();
	void call_proc_stat(std::string);
	Symbol_item *  call_func_stat(Symbol_item *);
	Symbol_item * call_list(Symbol_item*);

	Symbol_item * expr();
	Symbol_item *  item();
	Symbol_item *  factor();

};





#endif