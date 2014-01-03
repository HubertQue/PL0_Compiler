#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "token.h"
#include "error.h"

#include <string>
#include <map>
#include <fstream>
#define RESERVE_MAP std::map<std::string, Token::Token_type> 

class Tokenizer{
public:
	Token& next_token();
	Tokenizer(std::istream &_input, Error & _error);
private:
	Token current;
	int current_index;

	RESERVE_MAP reserve_map;
	std::istream& input;
	Error error;

	void init_reserve_map();
	Token& handle_digit(int);
	Token& handle_iden(int);
	Token& handle_char(int);
	Token& handle_string(int);
	Token& handle_mark(int);
};




#endif