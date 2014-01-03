#include "token.h"
#include <sstream>
TOKEN_TYPE Token:: get_type(){
	return type;
}

std::string Token:: get_value(){
	return value;
}

int Token:: get_num(){
	return num_value;
}

int Token:: get_line_no(){
	return line_number;
}


Token& Token:: update(TOKEN_TYPE _type, std::string _value, int _line_number){
	type = _type;
	value = _value;
	line_number = _line_number;
	
	if(type==Token::INTCON) 
		std::stringstream(_value) >> num_value;
	else if( _value.length()==1 ){
		char c;
		std::stringstream(_value) >> c;
		num_value = c;
	}
	else
		num_value= -1;
	return *this;
}

std::string Token:: get_type_name(){
	switch(type){
		case  IDEN: return std::string("IDEN"); break;
		case  INTCON: return std::string("INTCON"); break;
		case  REALCON: return std::string("REALCON"); break;
		case  CHARCON: return std::string("CHARCON"); break;
		case  STRCON: return std::string("STRCON"); break;
		case  CONSTTK: return std::string("CONSTTK"); break;
		case  INTTK: return std::string("INTTK"); break;
		case  FLOATTK: return std::string("FLOATTK"); break;
		case  CHARTK: return std::string("CHARTK"); break;
		case  VARTK: return std::string("VARTK"); break;
		case  ARRAYTK: return std::string("ARRAYTK"); break;
		case  OFTK: return std::string("OFTK"); break;
		case  REPTTK: return std::string("REPTTK"); break;
		case  UNLTK: return std::string("UNLTK"); break;
		case  IFTK: return std::string("IFTK"); break;
		case  THENTK: return std::string("THENTK"); break;
		case  ELSETK: return std::string("ELSETK"); break;
		case  DOTK: return std::string("DOTK"); break;
		case  WHILETK: return std::string("WHILETK"); break;
		case  SWITCHTK: return std::string("SWITCHTK"); break;
		case  CASETK: return std::string("CASETK"); break;
		case  FORTK: return std::string("FORTK"); break;
		case  TOTK: return std::string("TOTK"); break;
		case  BYTK: return std::string("BYTK"); break;
		case  DOWNTOTK: return std::string("DOWNTOTK"); break;
		case  PROCETK: return std::string("PROCETK"); break;
		case  FUNCTK: return std::string("FUNCTK"); break;
		case  READTK: return std::string("READTK"); break;
		case  WRITETK: return std::string("WRITETK"); break;
		case  CALLTK: return std::string("CALLTK"); break;
		case  BEGINTK: return std::string("BEGINTK"); break;
		case  ENDTK: return std::string("ENDTK"); break;
		case  ODDTK: return std::string("ODDTK"); break;
		case  PLUS: return std::string("PLUS"); break;
		case  MINU: return std::string("MINU"); break;
		case  MULT: return std::string("MULT"); break;
		case  DIV: return std::string("DIV"); break;
		case  LSS: return std::string("LSS"); break;
		case  LEQ: return std::string("LEQ"); break;
		case  GRE: return std::string("GRE"); break;
		case  GEQ: return std::string("GEQ"); break;
		case  EQL: return std::string("EQL"); break;
		case  NEQ: return std::string("NEQ"); break;
		case  ASSIGN: return std::string("ASSIGN"); break;
		case  SEMICN: return std::string("SEMICN"); break;
		case  COMMA: return std::string("COMMA"); break;
		case  PERIOD: return std::string("PERIOD"); break;
		case  COLON: return std::string("COLON"); break;
		case  QMARK: return std::string("QMARK"); break;
		case  DQMARK: return std::string("DQMARK"); break;
		case  LPARENT: return std::string("LPARENT"); break;
		case  RPARENT: return std::string("RPARENT"); break;
		case  LBRACK: return std::string("LBRACK"); break;
		case  RBRACK: return std::string("RBRACK"); break;
		case  LBRACE: return std::string("LBRACE"); break;
		case  RBRACE: return std::string("RBRACE"); break;
		default:	return std::string("unknonw"); break;
	}
}