//#define DEBUG

#ifdef DEBUG

#include "token.h"
#include "tokenizer.h"
#include "parser.h"
#include "root_symbol_table.h"
#include "object_code.h"
#include "utility.h"

#include <cstdlib>
#include <cstdio>

using namespace std;

void test_token(){
	Token token;
	token.update(Token::IDEN, "a", 4);
	cout<<""<<token.get_type_name()<<" "<<token.get_value()<<" "<<token.get_line_no()<<" "<<token.get_type()<<std::endl;
	cout<<token.get_line_no()<<endl;
	cout<<"end"<<endl;
}

void test_tokenizer(){
	//ifstream infile("test.txt");
	
	string file_name;
	cout<<"Type input file name :"<<endl;
	cin>> file_name;

	//istream * in = NULL;
	//if(file_name=="stdin")
	//	in = &cin;
	//else
	//	in = new ifstream(file_name.c_str());
	
	ifstream in(file_name.c_str());
	Error error;
	Tokenizer toke(in, error);
	ofstream outfile("11061159_token.txt");
	//ostream & out = cout;
	ostream & out = outfile;
	Token& current = toke.next_token();
	int id = 1;
	while(current.get_type()!=Token::END_OF_FILE){
		/*if(current.get_type()==Token::CHARCON)
			out<<id<<" "<<current.get_type_name()<<" \'"<<current.get_value()<<"\'"<<endl;
		else if(current.get_type()==Token::STRCON)
			out<<id<<" "<<current.get_type_name()<<" \""<<current.get_value()<<"\""<<endl;
		else*/
			out<<id<<" "<<current.get_type_name()<<" "<<current.get_value()<<endl;
			cout<<" "<<current.get_type_name()<<" "<<current.get_value()<<" "<<current.get_line_no()<<endl;
		
		id++;
		current = toke.next_token();
	}
	in.close();
	outfile.flush();
	outfile.close();
	cout<<"Finished"<<endl;


}
 
void test_parser(){

	//ifstream in("test_type.txt");
	//ifstream in("test_quat.txt");
	cout<<"Enter file:"<<endl;
	std::string file= "test_quat.txt";
	cin>>file;
	ifstream in(file.c_str());
	Error error;
	Tokenizer toke(in ,error);
	//Tokenizer toke(cin);
	Root_symbol_table symbol_table;
	Middle_code code; 
	Parser parser(toke, symbol_table, code, error);
	parser.parse();
	if(error.get_error_count()==0){
		cout<<"\n\nCompleted."<<error.get_error_count()<<" error(s), "<<error.get_warn_count()<<" warning(s)."<<endl;
		symbol_table.cal_offset(); 
		//symbol_table.print_content();
		symbol_table.rename();

		ofstream sym_out("sym.txt");
		symbol_table.print_content(sym_out);
		sym_out.close();
		symbol_table.print_content(cout);
		
		ofstream quat_out("quat.txt");
		code.print_content(quat_out);
		quat_out.close();
		code.print_content(cout);
		
		Object_code ins(code, symbol_table);
		ins.translate();
		ofstream out("test.asm");
		ins.print_file(out);		
		out.close();
		system("echo Assembling... && masm32\\bin\\ml.exe /c /coff test.asm && echo Linking... && masm32\\bin\\link.exe /SUBSYSTEM:CONSOLE /OPT:NOREF test.obj && echo Executing... && test.exe");
		cout<<endl;
	}else{
		cout<<"\n\nCompleted."<<error.get_error_count()<<" error(s), "<<error.get_warn_count()<<" warning(s). found. Check source code."<<endl;
	}

	in.close();
}

void test_symbol(){
	Root_symbol_table root;
	root.insert("a",Symbol_item::CONST,Symbol_item::INT,123);	
	root.insert("b",Symbol_item::VAR, Symbol_item::CHAR,'A');
	root.insert("b",Symbol_item::VAR, Symbol_item::CHAR,'A');
	root.insert("add", Symbol_item::PROC, Symbol_item::INT, 0);
	root.insert("a",Symbol_item::VAR, Symbol_item::CHAR,'B');
	root.insert("b",Symbol_item::VAR, Symbol_item::INT, 321);
	root.back();
	root.insert("abc",Symbol_item::CONST, Symbol_item::STRING, 0);

	root.print_content(cout);

}

void test_object_code(){
// 	Object_code code;
// 	code.add(Instruction::ADD, std::string("EAX"),std::string("EBX"));
// 	code.add(Instruction::PROC, std::string("_sum"), std::string(""));	
// 	code.add(Instruction::ENDP, std::string("_sum"), std::string(""));
// 	code.add(Instruction::LABEL, std::string("_L1"), std::string(""));
// 	code.add(Instruction::NOP, std::string(""), std::string(""));	
// 	code.add(Instruction::LEAVE, std::string(""), std::string(""));	
// 	code.print(cout);
}


#endif