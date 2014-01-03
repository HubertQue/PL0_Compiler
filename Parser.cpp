#include "parser.h"
#include "error.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <assert.h>

using std::string;
using std::vector;


//#define DEBUG(format,...) printf("File: "__FILE__", Line: %05d: "format"/n", __LINE__, ##__VA_ARGS__)  
//#define DEBUG(format,...) printf(format,##__VA_ARGS__)  

//#define LOG(s) std::cout<<"\t\t-----"<<s<<"()"<<std::endl //printf("%s()\n",s)
#define LOG(s) 

#define DEBUG_NEXT
//#define DEBUG_QUAT


/* Macro to test token type. */
#define IS(type) (current.get_type()== (type) )
#define NOT(type) (current.get_type()!= (type) )


/* Macro to print error message. */
#define ERROR(num,msg) error.error_message( num , msg , current.get_line_no())
#define ERROR2(num, msg1, msg2) error.error_message( num , msg1 , msg2, current.get_line_no())

#define CHECK_TYPE(dst, src) check_type(dst, src)

bool Parser::check_type(Symbol_item*dst, Symbol_item*src){
	if(dst->get_type()!=Symbol_item::ERROR
		&&src->get_type()!=Symbol_item::ERROR
		&&dst->get_type()!=src->get_type()){
		/* This is not used in function parameters check. So all type mismatches
			occur in expressions. If we treat it as weak-type language, no need to 
			generate ERROR of WARNING. */ 
		
		//	ERROR2(Error::TYPE_MISMATCH,dst->get_name()+"("+dst->get_type_name()+")",
		//	src->get_name()+"("+src->get_type_name()+")");
			return false;
	}
	return true;
}

/* Skip until find certain type. */
#define GOTO(type) while(NOT(type)) next()
#define ERROR_TO(num, msg, type) ERROR(num, msg); \
	GOTO(type)

/* Macro to test certain symbol and print error.
	Mainly for mismatch symbol error.
	It will not skip identifiers.*/
// #define EXPECT(type, sym) if IS(type)\
// 	next();\
// else {\
// 	ERROR2(Error::EXPECT_BEFORE, sym, current.get_value());	\
// 	if (!fresh || NOT(Token::IDEN))\
// 		next();\
// 	else\
// 		fresh = false;\
// }
//	if (current.get_type()>=Token::PLUS&&current.get_type()<=Token::RBRACE)\
//		next();	\
//}
//ERROR(Error::EXPECT_COMMON, sym )

/* Max times token will be kept. */
#define FRESH_NUM 3

#define EXPECT(type, sym) expect(type, sym)
void Parser:: expect(Token::Token_type type, std::string sym){
	if IS(type)
		next();
	else {
		ERROR2(Error::EXPECT_BEFORE, sym, current.get_value());	

		//next();

		/* Comment this will stop skipping. */
		if (fresh<=0|| IS(Token::EQL))
			next();
		else
			fresh--;
	}
}


/*
Get next token , skip all comment.
*/
void Parser::next(){
	static int lastline = 0;
	fresh = FRESH_NUM;
	current = tokenizer.next_token();

	while IS(Token::LBRACE){
		while(1){
			current = tokenizer.next_token();
			if(IS(Token::END_OF_FILE))
				break;
			if(IS(Token::RBRACE)){
				current = tokenizer.next_token();
				break;
			}
		}
	}

	if (lastline!=current.get_line_no()){
		lastline = current.get_line_no();
#ifdef DEBUG_NEXT
		std::cout<<endl<<lastline<<":\t";
#endif
	}
#ifdef DEBUG_NEXT
	if(IS(Token::CHARCON))
		std::cout<<"'"<<current.get_value()<<"' ";
	else if IS(Token::STRCON)
 		std::cout<<"\""<<current.get_value()<<"\" ";
	else
		std::cout<<current.get_value()<<" ";
#endif
}
/*
	Goto next line. ( Used to handle errors ).
*/
void Parser::next_line(){
	int lastline = current.get_line_no();
	while(current.get_line_no()==lastline)
		next();
}
/*
Generate middle code. 
*/
void Parser::gen(Quaternion::op opcode, Symbol_item * src1,
				 Symbol_item * src2, Symbol_item *dst){
#ifdef DEBUG_QUAT
	Quaternion(opcode, src1, src2, dst).print();
#endif
	middle_code.add(opcode, src1, src2, dst);
}

/*
Get pointer to the identifier. If not found, show error
but generate a temporary symbol as expected VAR kind and ERROR type.
This must NOT return NULL.
*/
Symbol_item * Parser::get(std::string iden){
	Symbol_item * item = symbol_table.search(iden);
	if(item!=NULL) return item;
	ERROR(Error	::UNDEFINED_REFERENCE, iden);
	item = symbol_table.get_temp(Symbol_item::VAR,Symbol_item::ERROR);
	assert(item!=NULL);
	return item;
}

/*
Start parse. 
*/
void Parser::parse(){
	LOG("parse");
	
	next();
	program();
	if(NOT(Token::PERIOD))
		ERROR(Error::EXPECT_END, current.get_value());
}

/*
<程序>     ::=  <分程序>.
<分程序>  ::=   [<常量说明部分>][<变量说明部分>]{[<过程说明部分>]| [<函数说明部分>]}<复合语句>
<过程说明部分>     ::=  <过程首部><分程序>;
<函数说明部分>     ::=  <函数首部><分程序>;
*/
 void Parser::program(){	
	LOG("program");

	if(IS(Token::CONSTTK))
		const_head();
	if(IS(Token::VARTK))
		var_head();
	while(IS(Token::PROCETK)||IS(Token::FUNCTK)){
		if(IS(Token::PROCETK)){
			proc_head();
			program();
			/* Symbol table point step into the PROC when inserting a PROC into it , 
			 now Symbol table pointer will back to current block. */
			symbol_table.back();
			//gen(Quaternion::END, NULL, NULL, NULL);

			EXPECT(Token::SEMICN, ";");
		}
		else{
			func_head();
			program();
			EXPECT(Token::SEMICN, ";");
			symbol_table.back();
			//gen(Quaternion::END, NULL, NULL, NULL);
		}
	}
	gen(Quaternion::PROC, NULL, NULL,(Symbol_item*) symbol_table.get_current_table());
	com_stat();
	gen(Quaternion::END, NULL, NULL,(Symbol_item*) symbol_table.get_current_table());
}
/*
<常量说明部分>     ::=  const<常量定义>{,<常量定义>};
*/
void Parser::const_head(){
	LOG("const_head");
	/* Current token must be "const"  */
	next();
	if IS(Token::IDEN){
		while(1){
			const_def();
			if IS(Token::COMMA){
				next();
			}else
				break;
		}	
		EXPECT(Token::SEMICN, ";");
	}
	else
		ERROR(Error::INVALID_IDEN, current.get_value());
}
/*
<常量定义>         ::=  <标识符>＝ <常量>
<常量>             ::=  [+| -] <无符号整数>|<字符>
*/
void Parser::const_def(){	
	LOG("const_def");
	string name = current.get_value();	/* Save identifier name. */
	next();
	if (IS(Token::EQL) || IS(Token::ASSIGN)){
		if IS(Token::ASSIGN)
			ERROR(Error::EXPECT_EQL, current.get_value());		
		next();

		bool minus = false ; // false + true -
		
		/* If is int or + or - */
		if(IS(Token::INTCON)||IS(Token::PLUS)||IS(Token::MINU)){
			if (IS(Token::MINU)|| IS(Token::PLUS)){
				if IS(Token::MINU) 
					minus = true;
				next();
			}
			if IS(Token::INTCON){
				if(!symbol_table.insert(name, Symbol_item::CONST,Symbol_item::INT, minus ? - current.get_num(): current.get_num()))
					ERROR(Error::DUPLICATED_DEFINITION, name);				
				next();
			}else
				ERROR2(Error::INVALID_VALUE,current.get_value(), "const");
			
		}		
		else if IS(Token::CHARCON){
			if(!symbol_table.insert(name, Symbol_item::CONST,Symbol_item::CHAR, current.get_num()))
			ERROR2(Error::INVALID_VALUE,current.get_value(), "const");
			next();
		}
		else 
			ERROR2(Error::INVALID_VALUE,current.get_value(), "const");

		
	}
	else {
		ERROR(Error::EXPECT_EQL, current.get_value());
	}
}
/*
<变量说明部分>     ::=  var <变量说明> ; {<变量说明>;}
*/
void Parser::var_head(){
	LOG("var_head");
	/* Current must be "var". */
	next();
	while(1){
		if IS(Token::IDEN)
			var_def();
		else{
			ERROR(Error::INVALID_IDEN, current.get_value());
		}
		if IS(Token::IDEN){
			continue;
		}else
			break;
	}
}
/*
<变量说明>              ::=  <标识符>{, <标识符>} :  <类型>
*/
void Parser::var_def(){	
	LOG("var_def");
	std::vector<std::string> * var_name = new std::vector<std::string>();
	while(1){
		if IS(Token::IDEN)			
			var_name->push_back(current.get_value()); /* Save identifier name . */
		else
			ERROR(Error::INVALID_IDEN,current.get_value());
		next();
		if IS(Token::COMMA){
			next();
		}else
			break;
	}
	if IS(Token::COLON){
		next();
		var_type(var_name);
	}else
		ERROR2(Error::EXPECT_BEFORE, ":", current.get_value());
		//ERROR(Error::EXPECT_COLON, current.get_value());

	EXPECT(Token::SEMICN, ";");

}
/*
<类型>                      ::=   <基本类型>|array'['<无符号整数>']' of <基本类型>
<基本类型>              ::=   integer | char
*/
void Parser::var_type(std::vector<std::string> * var_name){
	LOG("var_type");
	std::vector<std::string>::iterator iter = var_name->begin();
	Symbol_item::symbol_kind kind = Symbol_item::VAR;
	Symbol_item::symbol_type type = Symbol_item::INT;
	int size= 1;

	if IS(Token::CHARTK){
		type = Symbol_item::CHAR;
		next();
	}else if IS (Token::INTTK){
		type = Symbol_item::INT;
		next();
	}else if IS (Token::ARRAYTK){
		kind = Symbol_item::ARRAY;
		next();
		EXPECT(Token::LBRACK, "[");

		if IS(Token::INTCON){
			size = current.get_num();
			next();
		}
		else
			ERROR2(Error::INVALID_TYPE,current.get_value(),  "size of array");

		EXPECT(Token::RBRACK, "]");
		EXPECT(Token::OFTK, "of");
		
		if IS(Token::CHARTK){
			type = Symbol_item::CHAR;
			next();
		}else if IS(Token::INTTK){
			type = Symbol_item::INT;			
			next();
		}else
			ERROR2(Error::INVALID_TYPE,current.get_value(), "array");
		
	}else
		ERROR2(Error::INVALID_TYPE,current.get_value(), "var");
	
	while(iter!=var_name->end()){
		Symbol_item * iden = symbol_table.insert(*iter, kind,type, 0);
		if (iden)
			iden->set_size(size);
		else
			ERROR(Error::DUPLICATED_DEFINITION, *iter);
		iter++;
		}

	delete(var_name);
}

/*

<过程首部>         ::=   procedure<标识符>'('[<形式参数表>]')';
*/
void Parser::proc_head(){
	LOG("proc_head");
	/* Current must be "procedure". */
	next(); 
	if IS(Token::IDEN){
		Symbol_item* proc = symbol_table.insert(current.get_value(),Symbol_item::PROC, Symbol_item::VOID,0);
		if(proc==NULL)	ERROR(Error::DUPLICATED_DEFINITION, current.get_value());
// 		else
// 			gen(Quaternion::PROC, NULL, NULL, proc);
		next();

		EXPECT(Token::LPARENT, "(");
		
		if (IS (Token::VARTK) || IS(Token::IDEN))
			arg_list();

		EXPECT(Token::RPARENT, ")");
		EXPECT(Token::SEMICN, ";");
	}
	else
		ERROR(Error::INVALID_IDEN, current.get_value());

}
/*
<函数首部>         ::=   function <标识符>'('[<形式参数表>]')': <基本类型>;
*/
void Parser::func_head(){
	LOG("func_head");
	/* Current must be "function". */
	next();
	if IS(Token::IDEN){
		Symbol_item * item = NULL;
		item=symbol_table.insert(current.get_value(),Symbol_item::FUNC, Symbol_item::VOID,0);
		if(item==NULL )	ERROR(Error::DUPLICATED_DEFINITION, current.get_value());
// 		else
// 			gen(Quaternion::FUNC, NULL, NULL, item);
		next();
		EXPECT(Token::LPARENT, "(");
		if (IS (Token::VARTK) || IS(Token::IDEN))
			arg_list();
		EXPECT(Token::RPARENT, ")");
		EXPECT(Token::COLON, ":");
		
		// Return value 
		Symbol_item::symbol_type type = Symbol_item::INT;
		if IS(Token::INTTK){
			type = Symbol_item::INT ;
			next();
		}
		else if IS(Token::CHARTK){
			type = Symbol_item::CHAR ;			
			next();
		}
		else
			ERROR2(Error::INVALID_TYPE, current.get_value(), "function");
		
		// Insert function type back. 
		if(item!=NULL)
			item->set_type(type);

		EXPECT(Token::SEMICN, ";");
	}
}
/*
 <形式参数表>       ::=   [var] <标识符>{, <标识符>}: <基本类型>{; <形式参数表>}
*/
 void Parser::arg_list(){
	LOG("arg_list");
	// [var]
	Symbol_item::symbol_kind kind = Symbol_item::PARA;
	if IS(Token::VARTK){
		kind = Symbol_item::PARA_VAR;
		next();
	}else
		kind = Symbol_item::PARA;
	// identifiers 
	std::vector<std::string> args;
	while(1){
		if IS(Token::IDEN){
			args.push_back(current.get_value());
			next();
		}else
			ERROR(Error::INVALID_IDEN,current.get_value());
		if IS(Token::COMMA){
			next();
		}else
			break;
	}
	// :
	EXPECT(Token::COLON, ":");
	
	// type
	Symbol_item::symbol_type type = Symbol_item::INT;
	if IS(Token::INTTK){
		type = Symbol_item::INT;
		next();
	}
	else if IS(Token::CHARTK){
		type = Symbol_item::CHAR;
		next();
	}
	else
		ERROR2(Error::INVALID_TYPE, current.get_value(), "parameter");
	
	// Insert into symbol table.
	std::vector<std::string>::iterator iter = args.begin();
	while(iter!=args.end()){
		if(!symbol_table.insert(*iter,kind,type, 0))
			ERROR(Error::DUPLICATED_DEFINITION, *iter);
		iter++;
	}

	if IS(Token::SEMICN){
		next();
		arg_list();
	}
		
 }



/*
<复合语句>       ::=  begin<语句>{; <语句>}end
*/
 void Parser::com_stat(){
	LOG("com_stat");
	EXPECT(Token::BEGINTK, "begin");
	stat();
	if IS(Token::ENDTK){
		next();
		return;
	}
	while (1){			
			EXPECT(Token::SEMICN, ";");
			stat();
			if IS(Token::ENDTK) {
				next();
				return;
			}
			if IS(Token::END_OF_FILE){
				ERROR(Error::MESSAGE, "Unexpected end of file.\n");
				return ;
			}
	}
 }

/*<语句>    ::=  <赋值语句>|<条件语句>|<repeat循环语句>|<过程调用语句>|<复合语句>|<读语句>|<写语句>|<for循环语句>|<空>*/

void Parser::stat(){
	LOG("stat");
	if IS(Token::IDEN){
		std::string id= current.get_value();
		next();
		if IS(Token::LPARENT)	// ( 
			call_proc_stat(id);
		else //if (IS(Token::ASSIGN)||IS(Token::LBRACK))	// := or [
			assign_stat(id);
		
	}
	else if IS(Token::IFTK)
		if_stat();
	else if IS(Token::REPTTK)
		repeat_stat();
	else if IS(Token::FORTK)
		for_stat();
	else if IS(Token::WRITETK)
		write_stat();		
	else if IS(Token::READTK)
		read_stat();	
	else if IS(Token::BEGINTK)
		com_stat();
	else if (IS(Token::SEMICN)||IS(Token::ENDTK))
		;
	else
		ERROR(Error::MESSAGE,"Unexpected word "+current.get_value()+" at begining of statement" );

	/* else continue. as statement may be blank .*/
	//GOTO(Token::ENDTK);
}

/*<赋值语句>      ::=  <标识符> := <表达式>| <函数标识符> := <表达式>|<标识符>'['<表达式>']':= <表达式>
	Combine <标识符> := <表达式>| <函数标识符> := <表达式> to <标识符> := <表达式>
*/
/* 
Para:
	id: name of identifier to be assigned.
*/
void Parser::assign_stat(std::string id){
	LOG("assign_stat");
	Symbol_item * dst = get(id);
	Symbol_item * src1 = NULL;	
	Symbol_item * src2 = NULL;

	/* Current token must be ":=" or "[" . */
	if IS(Token::LBRACK){		
		next();		
		src1 = expr();
		if(dst!=NULL && dst->get_kind()!=Symbol_item::ARRAY)
			ERROR2(Error::NOT_TYPE, dst->get_name(), "ARRAY");
			/* Report index out of bound error. */
		else if((src1->get_kind()==Symbol_item::CONST||src1->get_kind()==Symbol_item::TEMP_CON)
			&&(src1->get_value()<0||src1->get_value()>=dst->get_size()/4)){
			std::stringstream s;
			s<<"Array "<<dst->get_name()<<"[0.."<<(dst->get_size()/4-1)<<"] , invalid index "<<src1->get_value()<<" ";
			ERROR(Error::OUT_OF_BOUND,s.str());
		}
			
		EXPECT(Token::RBRACK, "]");
		EXPECT(Token::ASSIGN, ":=");
		src2 = expr();
		CHECK_TYPE(dst, src2);
		Symbol_item* addr = symbol_table.get_temp(Symbol_item::TEMP_ADD,dst->get_type());
		gen(Quaternion::ARR_LOAD,dst, src1, addr);
		gen(Quaternion::ASSIGN,src2,NULL, addr );
	}else{
			EXPECT(Token::ASSIGN, ":=");
			src1 = expr();
			if(dst!=NULL&&(dst->get_kind()==Symbol_item::VAR||
				dst->get_kind()==Symbol_item::PARA||
				dst->get_kind()==Symbol_item::PARA_VAR)){
				CHECK_TYPE(dst, src1);
				gen(Quaternion::ASSIGN, src1, NULL, dst);
			}else if(dst!=NULL&&(dst->get_kind()==Symbol_item::FUNC)){
				if( !symbol_table.find_function(dst->get_name())){
					ERROR(Error::MESSAGE, "Function "+dst->get_name()+" can not be assigned here.");
				}else{
					CHECK_TYPE(dst, src1);
					gen(Quaternion::ASSIGN, src1, NULL, dst);
				}
			}			
			else
				ERROR2(Error::INVALID_TYPE, dst->get_name(), "Assign statement");
	}

}
/*
<关系运算符>     ::=  <  |  <=  |  >  |  >=   |  =  |  <>
<条件语句>       ::=  if<条件>then<语句> | if<条件>then<语句>else<语句>
*/
Token::Token_type Parser::condition(){
	LOG("condition");
	// Condition
	Symbol_item* src1 = expr();
	Token::Token_type op = Token::UNKNOWN;
	if (IS(Token::LSS)||IS(Token::LEQ)||IS(Token::GRE)
		||IS(Token::GEQ)||IS(Token::EQL)||IS(Token::NEQ)){
		op= current.get_type();
		next();		
		Symbol_item* src2 = expr();
		CHECK_TYPE(src1, src2);
		gen(Quaternion::CMP, src1, src2, NULL);

	}else
		ERROR2(Error::EXPECT_BEFORE, ">,<,=,>=,<= or <>", current.get_value());
	return op;
}

/*
<条件>           ::=  <表达式><关系运算符><表达式>
*/
void Parser::if_stat(){
	LOG("if_stat");
	//Current token must be "if"
	next();
	Symbol_item * l1 = symbol_table.get_label();

	Token::Token_type op = condition();
	switch(op){
	case Token::LSS: gen(Quaternion::JGE,NULL,NULL, l1); break;
	case Token::LEQ: gen(Quaternion::JG, NULL,NULL, l1); break;
	case Token::GEQ: gen(Quaternion::JL, NULL, NULL, l1); break;
	case Token::GRE: gen(Quaternion::JLE, NULL, NULL, l1); break;
	case Token::EQL: gen(Quaternion::JNE, NULL, NULL, l1); break;
	case Token::NEQ: gen(Quaternion::JE, NULL, NULL, l1); break;
	default: 
		ERROR(Error::MESSAGE, "Invaild condition.");
	}
	
	EXPECT(Token::THENTK, "then");
	stat();// true_statement
	if IS(Token::ELSETK){
		/*
			condition;
			false JMP L1;
			true_statement;
			JMP L2
		L1:
			false_statement;
		L2:			
		*/
		Symbol_item * l2 = symbol_table.get_label();	
		gen(Quaternion::JMP, NULL, NULL, l2);
		gen(Quaternion::LABEL, NULL, NULL, l1);
		next();
		stat();// false_statement
		gen(Quaternion::LABEL, NULL, NULL, l2);
	}else{
		gen(Quaternion::LABEL, NULL, NULL, l1);	
	}

}
/*
<repeat循环语句>  ::=  repeat <语句>until<条件>
*/

 void Parser::repeat_stat(){
	LOG("repeat_stat");
	//Current token must be repeat.
	next();
	/*
	L1:
		do_statement;
		false JMP L1;
	*/
	Symbol_item * l1 = symbol_table.get_label();
	gen(Quaternion::LABEL, NULL, NULL, l1);
	stat();
	EXPECT(Token::UNLTK, "until");
	Token::Token_type op =  condition();
	switch(op){
	case Token::LSS: gen(Quaternion::JGE,NULL,NULL, l1); break;
	case Token::LEQ: gen(Quaternion::JG, NULL,NULL, l1); break;
	case Token::GEQ: gen(Quaternion::JL, NULL, NULL, l1); break;
	case Token::GRE: gen(Quaternion::JLE, NULL, NULL, l1); break;
	case Token::EQL: gen(Quaternion::JNE, NULL, NULL, l1); break;
	case Token::NEQ: gen(Quaternion::JE, NULL, NULL, l1); break;
	default: 
		ERROR(Error::MESSAGE, "Invaild condition.");
	}
	
}
/*
<for循环语句>     ::=  for <标识符> := <表达式> (to|downto) <表达式> do <语句>
*/
 void Parser::for_stat(){
	LOG("for_stat");
	//Current token must be for
	next();
	if IS(Token::IDEN){
		Symbol_item * id = get(current.get_value());
		if(!(id->get_kind()==Symbol_item::VAR||id->get_kind()==Symbol_item::PARA||id->get_kind()==Symbol_item::PARA_VAR)){
			ERROR2(Error::INVALID_TYPE, id->get_name(), "Assign statement");
		}
		next();
		EXPECT(Token::ASSIGN, ":=");
		Symbol_item* start = expr();
		if(id!=NULL){ 
			CHECK_TYPE(id, start);
			gen(Quaternion::ASSIGN, start, NULL,id );
		}
		
		bool inc = true;
		if (IS(Token::DOWNTOTK)||IS(Token::TOTK)){
			if IS(Token::DOWNTOTK) inc = false;
			next();
		}else
			ERROR2(Error::EXPECT_BEFORE, " to or downto ", current.get_value());
		/* Cal expression value first. */
		Symbol_item* end = expr();

		Symbol_item* l1 = symbol_table.get_label();
		Symbol_item* l2 = symbol_table.get_label();
		gen(Quaternion::JMP, NULL, NULL, l1);
		gen(Quaternion::LABEL, NULL, NULL, l2);
		
		EXPECT(Token::DOTK, "do");
		stat();
		if(inc) gen(Quaternion::INC,NULL, NULL, id);
		else gen(Quaternion::DEC, NULL, NULL, id);
		gen(Quaternion::LABEL, NULL, NULL, l1);
		CHECK_TYPE(id, end);
		gen(Quaternion::CMP, id, end, NULL);
		if(inc)
			gen(Quaternion::JLE, NULL, NULL, l2);
		else
			gen(Quaternion::JGE,NULL, NULL, l2);

		/*
			init;
			JMP l1;
		L2:
			do_statement;
			DEC/INC;
		L1:
			CMP;
			JNE L2;
		*/

	}else
		ERROR(Error::INVALID_IDEN, current.get_value());
}
/*
<写语句>  ::=   write'(' <字符串>,<表达式> ')'|write'(' <字符串>')'|write'('<表达式>')'
 */
void Parser::write_stat(){
	LOG("write_stat");
	//Current token must be "write"
	next();
	EXPECT(Token::LPARENT, "(");
	
	if IS(Token::STRCON){
		// Generate const string.
		Symbol_item * tmp = symbol_table.get_temp(Symbol_item::CONST, Symbol_item::STRING);
		tmp->set_str(current.get_value());
		gen(Quaternion::WRITE, NULL, NULL, tmp);

		next();
		if IS(Token::COMMA){
			next();
			Symbol_item* arg = expr();			
			gen(Quaternion::WRITE, NULL, NULL, arg);
		}
		// else blank expr may not exist. 
	}else{
		Symbol_item* arg = expr();			
		gen(Quaternion::WRITE, NULL, NULL, arg);
	}
	
	
	EXPECT(Token::RPARENT, ")");
}
/*
<读语句>     ::=  read'('<标识符>{,<标识符>}')'
*/
void Parser::read_stat(){
	LOG("read_stat");
	// Current token must be "read"
	next();
	EXPECT(Token::LPARENT, "(");
	std::vector<std::string> args;
	while(1){
		if IS(Token::IDEN){
			// save the identifier
			args.push_back(current.get_value());
			next();
		}else
			ERROR(Error::INVALID_IDEN, current.get_value());
		if IS(Token::COMMA)
			next();
		else
			break;
	}
	std::vector<std::string>::iterator iter = args.begin();
	while(iter!=args.end()){
		Symbol_item * dst = get(*iter);
		if(dst!=NULL){
			if((dst->get_kind()==Symbol_item::VAR||dst->get_kind()==Symbol_item::PARA||dst->get_kind()==Symbol_item::PARA_VAR)
				&&(dst->get_type()==Symbol_item::INT||dst->get_type()==Symbol_item::CHAR)){
				gen(Quaternion::READ,NULL, NULL, dst);
			}else{
				ERROR2(Error::INVALID_TYPE, dst->get_name(), "Read statement");
			}
		}
		iter++;
	}

	EXPECT(Token::RPARENT, ")");
	
}

/*
 <过程调用语句>      ::=  <标识符>'('[<实在参数表>]')'
*/
 void Parser::call_proc_stat(std::string id){
	LOG("call_proc_stat");
	Symbol_item * proc = get(id);
	if(proc==NULL) return ; // Or it will crash as null pointer. 
	if(proc!=NULL && proc->get_kind()!=Symbol_item::PROC){
		ERROR2(Error::NOT_TYPE, proc->get_name(), "PROCEDURE");
		return ;
	}
	// Current token must be "("
	next();
	call_list(proc);
}
/*
  <函数调用语句>      ::=  <标识符>'('[<实在参数表>]')'
*/
Symbol_item * Parser::call_func_stat(Symbol_item * id){
	LOG("call_func_stat");	
	Symbol_item * func = id;
	if(func==NULL) return symbol_table.get_temp(Symbol_item::TEMP, Symbol_item::ERROR); // Or it will crash as null pointer. 
	if(func!=NULL && func->get_kind()!=Symbol_item::FUNC){
		ERROR2(Error::NOT_TYPE, func->get_name(), "FUNCTION");
		return symbol_table.get_temp(Symbol_item::TEMP, Symbol_item::ERROR);
	}
	// Current token must be "("
	// Almost same as call_proc_stat()
	next();	
	Symbol_item* tmp = call_list(func);

	/*Save return value to a tmp var to avoid eax reg conflict.*/
	gen(Quaternion::ASSIGN,func, NULL, tmp);
	return tmp;
}
/*
<实在参数表>        ::=  <实在参数> {, <实在参数>}
 <实在参数>          ::=  <表达式>
*/
Symbol_item * Parser::call_list(Symbol_item* func){
	std::vector<Symbol_item*> * args_list = symbol_table.get_arg_list(func->get_name());
	std::vector<Symbol_item*> args;
	/* Save all parameters. */
	if IS(Token::RPARENT){  // ()
		next();		
	}else{					// (a*b, c)
		args.push_back(expr());
		while IS(Token::COMMA){
			next();
			args.push_back(expr());
		}
		EXPECT(Token::RPARENT, ")");
	}

	/* Check type and address passing(var_para). */
	std::vector<Symbol_item*>::iterator iter_list = args_list->begin();
	std::vector<Symbol_item*>::iterator iter = args.begin();
	while(1){
		if(iter_list==args_list->end()&&iter==args.end())	// End at same time , Match.
			break;
		else if( iter_list!=args_list->end() && iter!=args.end() && //Must check null before this.
			(*iter_list)->get_kind()==Symbol_item::PARA_VAR &&		// Check variable parameter.
			((*iter)->get_kind()!=Symbol_item::VAR&&
			(*iter)->get_kind()!=Symbol_item::PARA&&
			(*iter)->get_kind()!=Symbol_item::PARA_VAR&&
			(*iter)->get_kind()!=Symbol_item::TEMP_ADD)
			){
			int n= iter-args.begin()+1;
			std::stringstream buf;
			buf<<"Only variable type accepted : at No."<<n<<" parameter of "<<func->get_kind_name()<<" "<<func->get_name()<<"()";
			ERROR(Error::PARA_VAR_ERROR, buf.str());
			break;
		}
		else if((iter_list==args_list->end()||iter==args.end()) // Count Mismatch
		//	||(*iter_list)->get_type()!=(*iter)->get_type() 	// Type Mismatch Warning. 12/13 Ignored here.
			){		
			std::stringstream buf;
			buf<<func->get_name()<<"( ";
			iter_list= args_list->begin();
			while(iter_list!=args_list->end()){
				buf<<(*iter_list)->get_type_name()<<" ";
				iter_list++;
			}
			buf<<")";
			std::string req = buf.str();
			//buf.clear(); It clear state bit not content.
			buf.str("");
			buf<<"( ";
			iter= args.begin();
			while(iter!=args.end()){
				buf<<(*iter)->get_type_name()<<" ";
				iter++;
			}
			buf<<")";
			std::string err= buf.str();
			
			ERROR2(Error::PARAS_MISMATH, req, err);
			break;
		}
		iter_list++;
		iter++;
	}
	/* Push parameters into stack and call function. */
	iter = args.begin();
	iter_list= args_list->begin();
	while(iter!=args.end()){
		if(iter_list!=args_list->end()){ 
			if((*iter_list)->get_kind()==Symbol_item::PARA_VAR)
				gen(Quaternion::PUSH_VAR, NULL, NULL, *iter);
			else
				gen(Quaternion::PUSH, NULL, NULL, *iter);
			
			iter_list++;
		}
		iter++;
	}	
	gen(Quaternion::CALL, NULL, NULL, func);
	
	/* If this is a function , return its return-value */
	if(func->get_kind()==Symbol_item::FUNC){
		Symbol_item* tmp = symbol_table.get_temp(Symbol_item::TEMP, func->get_type());
		return tmp;
	}else
		return NULL;
}

/*
<表达式>        ::=  [+|-]<项>{<加法运算符><项>}

If a temporary variable generates, save all results into it.
This will save count of temporary variables.
*/
Symbol_item * Parser::expr(){
	LOG("expr");
	bool minus = false; 
	if (IS(Token::PLUS)||IS(Token::MINU)){
		if IS (Token::MINU)
			minus = true;
		next();
	}	
	Symbol_item * it = item();
	Symbol_item * tmp = NULL;
	if(minus){
		tmp = symbol_table.get_temp(Symbol_item::TEMP, it->get_type());
		gen(Quaternion::NEG, it, NULL, tmp);
		it = tmp ;
	} 
	while (IS(Token::PLUS)||IS(Token::MINU)){
		Quaternion::op opcode = IS(Token::PLUS)? Quaternion::ADD:Quaternion::SUB;
		next();
		//Symbol_item * tmp = symbol_table.get_temp();
		Symbol_item * it2 = item();
	//	if(tmp==NULL){
			tmp= symbol_table.get_temp(Symbol_item::TEMP, it->get_type());
			if(!CHECK_TYPE(it, it2))
				tmp->set_type(Symbol_item::INT);
			gen(opcode, it, it2, tmp);
			it= tmp; // copy tmp to it

	//	}else{
	//		if(!CHECK_TYPE(tmp, it2))
	//			tmp->set_type(Symbol_item::INT);			
	//		gen(opcode, tmp, it2, tmp);
	//	}
	}
//	if(tmp==NULL)
		return it;
//	else
//		return tmp;
}
/*
<项>            ::=  <因子>{<乘法运算符><因子>}
If a temporary variable generates, save all results into it.
This will save count of temporary variables.
*/
Symbol_item *  Parser::item(){
	LOG("item");
	Symbol_item * f1  = factor();
	Symbol_item * tmp = NULL;
	while (IS(Token::MULT)|| IS(Token::DIV)){
		Quaternion::op opcode = IS(Token::MULT)? Quaternion::MUL:Quaternion::DIV;
		next();
		Symbol_item * f2 = factor();
//		if(tmp==NULL){
			tmp = symbol_table.get_temp(Symbol_item::TEMP, f1->get_type());
			if(!CHECK_TYPE(f1,f2))
				tmp->set_type(Symbol_item::INT);
			gen(opcode, f1, f2, tmp);
			f1 = tmp; // copy tmp to f1.
//		}else{
//			if(!CHECK_TYPE(tmp,f2))
//				tmp->set_type(Symbol_item::INT);
//			gen(opcode,tmp, f2, tmp);
//		}
	}
//	if(tmp==NULL)
		return f1;
//	else
//		return tmp;
}
/*
<因子>  ::=  <标识符>|<标识符>'['<表达式>']'|<无符号整数>|'('<表达式>')' | <函数调用语句>
*/
Symbol_item *  Parser::factor(){
	LOG("factor");
	if IS(Token::LPARENT){	//	(2*3)
		next();
		Symbol_item * ex = expr();
		EXPECT(Token::RPARENT, ")");
		return ex;
	}else if IS(Token::INTCON){	// 123
		Symbol_item * tmp = symbol_table.get_temp(Symbol_item::TEMP_CON, Symbol_item::INT);
		tmp->set_value(current.get_num());
		next();
		return tmp;
	}else if IS(Token::IDEN){
		Symbol_item * id = get(current.get_value());
		next();
		if IS(Token::LBRACK){
			// a[2]
			if (id->get_kind()!=Symbol_item::ARRAY)
				ERROR2(Error::NOT_TYPE, id->get_name(), "ARRAY");
			next();
			Symbol_item * idx = expr();

			if((idx->get_kind()==Symbol_item::CONST||idx->get_kind()==Symbol_item::TEMP_CON)
				&&(idx->get_value()<0||idx->get_value()>=id->get_size()/4)){
				std::stringstream s;
				s<<"Array "<<id->get_name()<<"[0.."<<(id->get_size()/4-1)<<"] , invalid index "<<idx->get_value()<<" ";
				ERROR(Error::OUT_OF_BOUND,s.str());
		}

			/*Save address to a tmp var. */
			Symbol_item * tmp = symbol_table.get_temp(Symbol_item::TEMP_ADD,id->get_type());
			gen(Quaternion::ARR_LOAD, id, idx, tmp);			
			EXPECT(Token::RBRACK, "]");
			return tmp;
		} else if IS(Token::LPARENT){
			// no next ! 			
			Symbol_item * func = call_func_stat(id);
			return func;
		} else if (id!= NULL){
			if (id->get_kind()==Symbol_item::ARRAY)
				ERROR2(Error::EXPECT_AFTER,"[",  id->get_name());
			else if(id->get_kind()==Symbol_item::FUNC)
				ERROR2(Error::EXPECT_AFTER, "(", id->get_name()); 
		}
		return id;
	}else{
		ERROR(Error::MESSAGE, "Invalid expression. ");
		return symbol_table.get_temp(Symbol_item::TEMP, Symbol_item::ERROR);
	}
}