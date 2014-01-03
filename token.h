#ifndef TOKEN_H
#define TOKEN_H

#pragma warning(disable : 4786)
#include <iostream>
#include <string>
class Token{
public:
	
	enum Token_type
	{
		UNKNOWN,		//	unknown

		IDEN,		//	Identifier
		INTCON,		//	Integer
		REALCON,	//	Real number(Not used)
		CHARCON,	//	Charactor
		STRCON,		//	String

		/* Reserved words */
		CONSTTK,	//	"const"
		INTTK,		//	"int"
		FLOATTK,	//	"real"(Not used)
		CHARTK,		//	"char"
		VARTK,		//	"var"

		ARRAYTK,	//	"array"
		OFTK,		//	"of"	
		REPTTK,		//	"repeat"
		UNLTK,		//	"until"
		IFTK,		//	"if"

		THENTK,		//	"then"
		ELSETK,		//	"else"
		DOTK,		//	"do"
		WHILETK,	//	"while"(Not used)
		SWITCHTK,	//	"switch"(Not used)
		
		CASETK,		//	"case"(Not used)
		FORTK,		//	"for"
		TOTK,		//	"to"
		BYTK,		//	"by"(Not used)
		DOWNTOTK,	//	"downto"
		
		PROCETK,	//	"procedure"
		FUNCTK,		//	"function"
		READTK,		//	"read"
		WRITETK,	//	"write"
		CALLTK,		//	"call"(Not used)
		
		BEGINTK,	//	"begin"
		ENDTK,		//	"end"
		ODDTK,		//	"odd"(Not used)

		/* Operator */
		PLUS,		//	"+"
		MINU,		//	"-"
		MULT,		//	"*"
		DIV,		//	"/"
		LSS,		//	"<"
		LEQ,		//	"<="
		GRE,		//	">"	?
		GEQ,		//	">="
		EQL,		//	"="
		NEQ,		//	"<>"?
		ASSIGN,		//	":="
		SEMICN,		//	";"
		COMMA,		//	","
		PERIOD,		//	"."
		COLON,		//	":"
		QMARK,		//	"'"
		DQMARK,		//	"""
		LPARENT,	//	"("
		RPARENT,	//	")"
		LBRACK,		//	"["
		RBRACK,		//	"]"
		LBRACE,		//	"{"
		RBRACE,		//	"}"

		
		END_OF_FILE			//  EOF
	};

	Token_type get_type();
	std::string get_type_name();
	std::string get_value();
	int get_num();
	int get_line_no();
	Token& update(Token_type type, std::string value , int line);
private:

	Token_type type;
	std::string value;
	int num_value;
	int line_number;
};

#define TOKEN_TYPE Token::Token_type
#endif