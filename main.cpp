#include "token.h"
#include "tokenizer.h"
#include "parser.h"
#include "root_symbol_table.h" 
#include "object_code.h"
#include "utility.h"
#include "optimizer.h"

#include <cstdlib>

using namespace std;

void compile(){
	
	/* Input test file path. */
	cout<<"Enter file path (No SPACE supported in the string):"<<endl;

	std::string file= "";
	cin>>file;


	ifstream in(file.c_str());
	if(!in){
		cout<<"Can not open file "<<file<<endl;
		return;
	}

	/* Initial all components. */
	Error error;
	Tokenizer toke(in ,error);
	Root_symbol_table symbol_table;
	Middle_code code; 
	Parser parser(toke, symbol_table, code, error);
	Optimizer optimizer;

	parser.parse();
	

	if(error.get_error_count()==0){
		cout<<"\n\nCompleted."<<error.get_error_count()<<" error(s), "<<error.get_warn_count()<<" warning(s)."<<endl;
		
		
		/* Print symbol table. */
		ofstream sym_out("sym.txt");
		symbol_table.print_content(sym_out);
		sym_out.close();
		symbol_table.print_content(cout);
		
		/* Print intermediate code. */
		ofstream quat_out("quat.txt");
		code.print_content(quat_out);
		quat_out.close();
		code.print_content(cout);

		/* Optimize */
// 		optimizer.build(code);
// 		optimizer.print_block(std::cout);
// 		optimizer.DAG_optimize();
// 		optimizer.export_code(code);

		/* Generate object code. */
		symbol_table.cal_offset(); 
		symbol_table.rename();
		Object_code ins(code, symbol_table);
		ins.translate();
		ofstream out("test.asm");
		ins.print_file(out);		
		out.close();


		/* Assembly , link and run. */
		system("echo Assembling... && masm32\\bin\\ml.exe /c /coff test.asm && echo Linking... && masm32\\bin\\link.exe /SUBSYSTEM:CONSOLE /OPT:NOREF test.obj && echo Executing... && test.exe");
		cout<<endl;

	}else{
		cout<<"\n\nCompleted."<<error.get_error_count()<<" error(s), "<<error.get_warn_count()<<" warning(s)  found. Check source code."<<endl;
	}	
	in.close();
}

void test_one(std::string file){			
	ifstream in(file.c_str());
	if(!in){
		cout<<"Can not open file "<<file<<endl;
		return;
	}
	Error error;
	Tokenizer toke(in ,error);
	Root_symbol_table symbol_table;
	Middle_code code; 
	Parser parser(toke, symbol_table, code, error);	
	parser.parse();		
	
	if(error.get_error_count()==0){
		cout<<"\n\nCompleted."<<error.get_error_count()<<" error(s), "<<error.get_warn_count()<<" warning(s)."<<endl;
		
		/* Optimize */
		Optimizer optimizer;
	//	code.print_content(std::cout);
		optimizer.build(code);
//		optimizer.print_block(std::cout);
		optimizer.DAG_optimize();
//		optimizer.print_block(std::cout);
		optimizer.export_code(code);		
	//	code.print_content(std::cout);

		/* Generate object code. */
		symbol_table.cal_offset(); 
		symbol_table.rename();	
		Object_code ins(code, symbol_table);
		ins.translate();
		ofstream out("test.asm");
		ins.print_file(out);		
		out.close();			
		/* Assembly , link and run. */
		system("echo Assembling... && masm32\\bin\\ml.exe /c /coff test.asm && echo Linking... && masm32\\bin\\link.exe /SUBSYSTEM:CONSOLE /OPT:NOREF test.obj && echo Executing... &&echo # >> result.txt && test.exe && test.exe >> result.txt ");
		cout<<endl;			
	}else{
		cout<<"\n\nCompleted."<<error.get_error_count()<<" error(s), "<<error.get_warn_count()<<" warning(s)  found. Check source code."<<endl;
	}	
		in.close();
}
void test_all(){
	std::vector<std::string> files;
	files.push_back("test_cqd/t1.pas");//112624
	files.push_back("test_cqd/t2.pas");//64321
	files.push_back("test_cqd/t3.pas");//2
	files.push_back("test_cqd/t4.pas");//120
	files.push_back("test_cqd/t5.pas");//236022
	files.push_back("test_cqd/t7.pas");//2368111314
	files.push_back("test_cqd/t8.pas");//0123456789
	files.push_back("test_cqd/t9.pas");//01234567897
	files.push_back("test_cqd/t10.pas");//01234567897
	files.push_back("test_cqd/t11.pas");//64321
	files.push_back("test_cqd/t12.pas");//9
	files.push_back("test_cqd/t13.pas");//222250502222495022224851	
	files.push_back("test_cqd/expr.txt");//8110
	std::vector<std::string>::iterator iter = files.begin();
	system("echo ~ > result.txt");
	while(iter!=files.end()){
		test_one(*iter);		
		iter++;
	}
}

void test_me(){
	std::vector<std::string> files;
	files.push_back("test_me/1.txt");//112624
	files.push_back("test_me/2.txt");//112624
	files.push_back("test_me/3.txt");//112624
	files.push_back("test_me/4.txt");//112624
	files.push_back("test_me/5.txt");//112624
	std::vector<std::string>::iterator iter = files.begin();
	system("echo ~ > result.txt");
	while(iter!=files.end()){
		test_one(*iter);		
		iter++;
	}
}

#define  DEBUG

int main(){
#ifdef DEBUG	
//	test_all();
//	test_me();
// 	test_one("test_cqd/t5.pas");
	test_one("test_me/5.txt");
//	test_one("test_me/dag.txt");
#else
	while(1){
		compile();
	}
#endif
	return 0;
}
