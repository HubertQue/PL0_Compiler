#include "tokenizer.h"
#include "utility.h"
#include "error.h"

using std::string;

Tokenizer:: Tokenizer(std::istream& _input, Error& _error):input(_input), error(_error){
	init_reserve_map();
	current.update(Token::UNKNOWN, "", 0);
	current_index = 1;

}

Token& Tokenizer:: next_token(){
	int c;
	/*	Skip space	*/
	c= input.get();
	while(is_space(c)){
		if(c=='\n') current_index++;
		c= input.get();
	}
	if(c==EOF) return current.update(Token::END_OF_FILE, "", current_index);
	else if(is_digit(c)) return handle_digit(c);
	else if(is_alpha(c)) return handle_iden(c);
	else if(is_qoute(c)) return handle_char(c);
	else if(is_dqoute(c)) return handle_string(c);
	else return handle_mark(c);
}

Token& Tokenizer:: handle_digit(int c){
	string digit="";
	while(is_digit(c)){
		digit+=char(c);
		c= input.get();
	}
	input.putback(c);
	current.update(Token::INTCON, digit, current_index);
	return current;
}

Token& Tokenizer:: handle_iden(int c){
	string iden="";
	while(is_alpha(c)||is_digit(c)){
		iden+=char(c);
		c = input.get();
	}
	input.putback(c);
	RESERVE_MAP ::iterator iter = reserve_map.find(iden);
	if(iter!=reserve_map.end()){
		return current.update(iter->second,iden, current_index);
	}else
		return current.update(Token::IDEN, iden, current_index);
}

Token& Tokenizer:: handle_char(int c){
	c= input.get();
	if(is_char(c)) {
		int tmp = c;
		current.update(Token::CHARCON, string("")+char(c), current_index);
		c= input.get();
		if(!is_qoute(c)){
			input.putback(c);
			error.error_message(Error::EXPECT_QUOTE, string("")+char(tmp), current_index);
		}
		return current;
	}else{
		input.putback(c);
			error.error_message(Error::UNEXPECTED_CHAR, string("")+char(c), current_index);
		return current.update(Token::QMARK, string("")+char(c), current_index);
	}

}
Token& Tokenizer:: handle_string(int c){
	c= input.get();
	string str= "";
	while(is_string(c)){
		str+=char(c);
		c= input.get();
	}
	if(is_dqoute(c)){
		return current.update(Token::STRCON, str, current_index);
	}else{
		input.putback(c);
		error.error_message(Error::EXPECT_DQUOTE, str, current_index);
		return current.update(Token::STRCON, str, current_index);
	}

}
Token& Tokenizer:: handle_mark(int c){
	int tmp;
	switch (c)
	{
	case '+': return current.update(Token::PLUS, string("")+char(c), current_index);
	case '-': return current.update(Token::MINU, string("")+char(c), current_index);
	case '*': return current.update(Token::MULT, string("")+char(c), current_index);
	case '/': return current.update(Token::DIV,  string("")+char(c), current_index);
	case '<':
		tmp = input.get();
		if(tmp=='=') return current.update(Token::LEQ, "<=", current_index);
		else if(tmp=='>') return current.update(Token::NEQ , "<>", current_index); 
		else{
			input.putback(tmp);
			return current.update(Token::LSS, string("")+char(c), current_index);
		}
	case '>':
		tmp = input.get();
		if(tmp=='=') return current.update(Token::GEQ , ">=", current_index);
		else
		{
			input.putback(tmp);
			return current.update(Token::GRE , string("")+char(c) , current_index);
		}
	case '=': return current.update(Token::EQL,  string("")+char(c), current_index);
	case ':': 
		tmp = input.get();
		if(tmp=='=') return current.update(Token::ASSIGN , ":=", current_index);
		else
		{
			input.putback(tmp);
			return current.update(Token::COLON,  string("")+char(c), current_index);
		}
	case ';': return current.update(Token::SEMICN,  string("")+char(c), current_index);
	case ',': return current.update(Token::COMMA, string("")+char(c), current_index);
	case '.': return current.update(Token::PERIOD, string("")+char(c), current_index);

	case '(': return current.update(Token::LPARENT, string("")+char(c), current_index);
	case ')': return current.update(Token::RPARENT, string("")+char(c), current_index);	

	case '[': return current.update(Token::LBRACK, string("")+char(c), current_index);	
	case ']': return current.update(Token::RBRACK, string("")+char(c), current_index);	

	case '{': return current.update(Token::LBRACE, string("")+char(c), current_index);			
	case '}': return current.update(Token::RBRACE, string("")+char(c), current_index);

	default:
		error.error_message(Error::UNEXPECTED_CHAR, string("")+char(c), current_index);
		return next_token();
	}
}

/*
	Initialize reserved words map which contains all preserved words. 
*/
void Tokenizer:: init_reserve_map(){
	reserve_map["const"]= Token::CONSTTK;
	reserve_map["integer"]= Token::INTTK;
	reserve_map["char"] = Token::CHARTK;
	reserve_map["var"] = Token::VARTK;

	reserve_map["array"] = Token::ARRAYTK;
	reserve_map["of"] = Token::OFTK;
	reserve_map["repeat"] = Token::REPTTK;
	reserve_map["until"] = Token::UNLTK;
	reserve_map["if"] = Token::IFTK;

	reserve_map["then"] = Token::THENTK;
	reserve_map["else"] = Token::ELSETK;

	reserve_map["for"] = Token::FORTK;
	reserve_map["to"] = Token::TOTK;
	reserve_map["downto"] = Token::DOWNTOTK;
	reserve_map["do"] = Token::DOTK;
	
	reserve_map["procedure"] = Token::PROCETK;	
	reserve_map["function"] = Token::FUNCTK;
	reserve_map["read"] = Token::READTK;
	reserve_map["write"] = Token::WRITETK;

	reserve_map["begin"] = Token::BEGINTK;	
	reserve_map["end"] = Token::ENDTK;	
}