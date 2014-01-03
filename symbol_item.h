#ifndef SYMBOL_H
#define SYMBOL_H
#include <string>

#pragma warning(disable : 4786)

class Symbol_item
{
public:
	/*
		LEFT: kind--  VAR, PARA, PARA_VAR ( FUNC: only when returns );
			  type--	INT, CHAR
		
		RIGHT: kind--  CONST, VAR, PARA, PARA_VAR, TEMP, TEMP_CON 
			   type--  INT, CHAR
	*/
	enum symbol_kind{
		CONST,	// .data
		VAR,	// .data? or stack
		PARA,	// stack
		PARA_VAR,	// stack.
		ARRAY,	// .data? or stack. (Array must be variable in PL0.) 
		PROC,	// --
		FUNC,	// --
		LABEL,  // --
		TEMP,	// stack. Temporary variable
		TEMP_ADD,	// stack. Temporary address.(Only use in array. ) 
		TEMP_CON // immediate.   Temporary constant
	};
	enum symbol_type{
		VOID,
		INT,
		CHAR,
		STRING,
		ERROR
	};
	Symbol_item(std::string _name, symbol_kind _kind, symbol_type _type, int _value);
	std::string get_name();
	void set_name(std::string);
	symbol_kind get_kind();
	void set_kind(Symbol_item::symbol_kind);
	symbol_type get_type();
	void set_type(Symbol_item::symbol_type);
	int get_value();
	void set_value(int);
	int get_size();
	void set_size(int);
	int get_level();
	void set_level(int);

	std::string get_kind_name();
	std::string get_type_name();

	std::string get_str();
	void set_str(std::string);

	void set_offset(int);
	int get_offset();

private:
	/* Unique name in current level. */
	std::string name;
	symbol_kind	kind;
	symbol_type type;
	int value;
	int size;
	int level;

	
	/* Only for const string */
	std::string str;
	/* Used in code generation. Offset of base pointer(ebp) */
	int offset;
};


#endif