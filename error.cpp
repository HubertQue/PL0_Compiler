#include "error.h"
#define MAX_ERROR 10

#define ERROR_MSG error_count++;if(error_count<MAX_ERROR) cerr<<"\nError "<<err<<" in line "<<line
#define WARN_MSG warn_count++; cerr<<"\nWarning "<<err<<" in line "<<line


Error::Error(){
	error_count= 0;
	warn_count=0;
}

void Error::error_message(Error_type err, string msg, int line){
	switch (err)
	{
	/* In tokenizer. */
	case UNEXPECTED_CHAR: ERROR_MSG<<" : Unexpected char ["<<msg<<"] here."<<endl; break;
	case EXPECT_QUOTE:	ERROR_MSG<<" : Expect \' after ["<<msg<<"] here."<<endl; break;
	case EXPECT_DQUOTE: ERROR_MSG<<" : Expect \" after ["<<msg<<"] here."<<endl; break;
	
	/* Syntax error. */	
	case EXPECT_EQL: ERROR_MSG <<" : Expect = at ["<<msg<<"] ."<<endl; break;
	case EXPECT_END: ERROR_MSG <<" : Expect . to end the block ."<<endl; break;
//	case EXPECT_SIMI: ERROR_MSG <<" : Expect ; at  ["<<msg<<"] " <<endl; break;
//	case EXPECT_COLON: ERROR_MSG <<" : Expect : at ["<<msg<<"] " <<endl; break;
//	case EXPECT_COMMON: ERROR_MSG<<" : Expect ["<<msg<<"] here. "<< endl;
		
	/* Semantic error. */	
	case DUPLICATED_DEFINITION: ERROR_MSG<<" : Duplicated definition. ["<<msg<<"] already exists in this block. "<<endl; break;
	case INVALID_IDEN: ERROR_MSG<<" : Invalid indentifier name ["<<msg<<"]. "<<endl; break;
	case UNDEFINED_REFERENCE: ERROR_MSG<<" : Undefined referrence ["<<msg<<"]. "<<endl; break;
		
	case OUT_OF_BOUND:WARN_MSG<<" : Out of bound :"<<msg<<endl;break;
	case RETURN_ERROR: ERROR_MSG<<" : "<<msg<<endl;break;
	case PARA_VAR_ERROR: ERROR_MSG<<" : "<<msg<<endl;break;

	case MESSAGE: ERROR_MSG<<" : "<<msg<<endl; break;
	default:
		ERROR_MSG<<" : Error message: ["<<msg<<"] ."<<endl;
		break;
	}
}


void Error:: error_message(Error_type err, string msg1, string msg2, int line){
	switch(err)
	{
	/* Syntax error. */
	case EXPECT_BEFORE: ERROR_MSG<<" : Expect ["<<msg1<<"] before ["<<msg2<<"] ."<<endl; break;
	case EXPECT_AFTER: ERROR_MSG<<" : Expect ["<<msg1<<"] after ["<<msg2<<"] ."<<endl; break;

	/* Semantic error. */
	case INVALID_TYPE: ERROR_MSG<<" : Invalid type ["<<msg1<<"] for ["<<msg2<<"] ."<<endl; break;
	case INVALID_VALUE: ERROR_MSG<<" : Invalid value ["<<msg1<<"] for ["<<msg2<<"] ."<<endl; break;

	case NOT_TYPE: ERROR_MSG<<" : ["<<msg1<<"] is not "<<msg2<<" type. "<<endl; break; 
		
	case TYPE_MISMATCH:WARN_MSG<<" : Type mismatch:  ["<<msg1<<"] vs ["<<msg2<<"] . "<<endl; break;

	case PARAS_MISMATH: ERROR_MSG<<" : Parameter list mismatch: Require "<<msg1<<" not "<<msg2<<" ." <<endl; break;

	case MESSAGE:		ERROR_MSG<<" :"<<msg1<<msg2<<endl; break;

	default:
		ERROR_MSG<<" :Error message: ["<<msg1<<"] ["<<msg2<<"] ."<<endl;
		break;
	}
}

int Error::get_error_count(){
	return error_count;
}

int Error::get_warn_count(){
	return warn_count;
}