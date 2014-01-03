#include "symbol_item.h"

Symbol_item::Symbol_item(std::string _name, Symbol_item::symbol_kind _kind,
						 Symbol_item::symbol_type _type, int _value): name(_name),kind(_kind), type(_type),value(_value)
{
// 	if(_type==symbol_type::INT)
// 		size = 4;
// 	else if(_type==symbol_type::CHAR)
// 		size = 1;
// 	else 
	if(_type==symbol_type::STRING)
		size = _name.size();
	else if(_kind!=symbol_kind::ARRAY){
		size = 1;
	}
	level = 0;
	str= _name;
	offset = 0;

}

std::string Symbol_item:: get_name(){
	return name;
}
void Symbol_item:: set_name(std::string _name){
	name = _name;
}
Symbol_item::symbol_kind  Symbol_item::get_kind(){
	return kind;
}
void Symbol_item:: set_kind(Symbol_item::symbol_kind _kind){
	kind = _kind;
}
Symbol_item::symbol_type Symbol_item:: get_type(){
	return type;
}
void Symbol_item:: set_type(Symbol_item::symbol_type _type){
	type = _type;
}
int Symbol_item::get_value(){
	return value;
}
void Symbol_item::set_value(int _value){
	value = _value;
}

/* Return real size in memory*/
/*********************************************
	To make it easier , 
	save char as 4 byte.

	This may change at last.
**********************************************/
int Symbol_item::get_size(){
//	if(type== Symbol_item::CHAR&&kind!=Symbol_item::TMEP_ADD)
//		return size;
//	else
//
	if(type==Symbol_item::STRING)
		return size;
	else
		return size *4;
}

/* Only for array. */
void Symbol_item::set_size(int _size){
	size = _size;
}

int Symbol_item::get_level(){
	return level;
}
void Symbol_item::set_level(int _level){
	level = _level;
}

std::string Symbol_item::get_str(){
	return str;
}
void Symbol_item::set_str(std::string _str){
	str= _str;
}
void Symbol_item::set_offset(int _offset){
	offset = _offset;
}
int Symbol_item::get_offset(){
	return offset;
}

std::string Symbol_item::get_type_name(){
	std::string type_name=  "UNKNOWN";
	switch(type){
	case Symbol_item::	VOID: type_name = "VOID"; break;
	case Symbol_item::	INT: type_name = "INT"; break;
	case Symbol_item::	CHAR: type_name = "CHAR"; break;
	case Symbol_item::	STRING: type_name = "STRING"; break;
	case Symbol_item::  ERROR: type_name= "ERROR"; break;
		default: type_name = "UNKNOWN" ; break;
	}
	return type_name;
}

std::string Symbol_item::get_kind_name(){
	std::string kind_name=  "UNKNOWN";
	switch(kind){
	case Symbol_item::	CONST: kind_name = "CONST"; break;
	case Symbol_item::	VAR: kind_name = "VAR"; break;
	case Symbol_item::	PARA: kind_name = "PARA"; break;
	case Symbol_item::	PARA_VAR: kind_name = "PAR_VAR"; break;
	case Symbol_item::	ARRAY: kind_name = "ARRAY"; break;
	case Symbol_item::	PROC: kind_name = "PROC"; break;
	case Symbol_item::	FUNC: kind_name = "FUNC"; break;
	case Symbol_item::	LABEL: kind_name = "LABEL"; break;
	case Symbol_item::	TEMP: kind_name = "TEMP"; break;
	case Symbol_item:: TEMP_CON :kind_name = "TMP_CON"; break;
	case Symbol_item:: TEMP_ADD: kind_name = "TMP_ADD"; break;
	default : kind_name = "UNKNOWN" ; break;
		}
	return kind_name;
}


