#ifndef ERROR_H
#define ERROR_H

#pragma warning(disable : 4786)

#include <string>
#include <iostream>

using std::cerr;
using std::string;
using std::endl;

class Error{
public:
	enum Error_type{
		UNEXPECTED_CHAR,
		EXPECT_QUOTE,
		EXPECT_DQUOTE,
		EXPECT_END,
		EXPECT_EQL,
		
		DUPLICATED_DEFINITION,
		UNDEFINED_REFERENCE,
		NOT_ARRAY,
		NOT_PROC,
		
		INVALID_IDEN, 
		INVALID_VALUE, 
		INVALID_TYPE, 
		TYPE_MISMATCH,

		PARAS_MISMATH,
		RETURN_ERROR,
		PARA_VAR_ERROR,

		EXPECT_BEFORE,
		EXPECT_AFTER,

		NOT_TYPE,

		OUT_OF_BOUND,

		MESSAGE
	};
	Error();
	void error_message(Error_type err, string msg, int line);
	void error_message(Error_type err, string msg1, string msg2, int line);
	int get_error_count();
	int get_warn_count();
private:
	int error_count;
	int warn_count;
};


#endif