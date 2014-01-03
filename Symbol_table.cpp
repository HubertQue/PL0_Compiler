#include "symbol_table.h"
#include "error.h"
#include <sstream>

Symbol_table::Symbol_table(Symbol_item * _proc, Symbol_table * father, int _level ){
	proc = _proc;
	father_table = father;
	level = _level;
	args_size= 0;
	stack_size = 0;
}

Symbol_table:: ~Symbol_table(){
	SYMBOL_MAP::iterator s_iter = symbol_map.begin();
	while(s_iter!=symbol_map.end()){
		delete(s_iter->second);
		s_iter++;
	}
	TABLE_MAP::iterator t_iter = table_map.begin();
	while(t_iter!=table_map.end()){
		delete(t_iter->second);
		t_iter++;
	}
}

/* Put item in to symbol map, if already exists return false. */
bool Symbol_table::put_item(Symbol_item * item){
	if(get_item(item->get_name())!=NULL){
		return false;
	}
	symbol_map[item->get_name()]= item;
	item->set_level(level);
	seq_list.push_back(item);
	return true;
}

/* Find item in symbol map, if not found return NULL */
Symbol_item * Symbol_table::get_item(std:: string name){
	SYMBOL_MAP::iterator iter = symbol_map.find(name);
	if(iter!=symbol_map.end()) 
		return iter->second;
	else
		return NULL;
}


/* Get item which represent this procedure. */
Symbol_item * Symbol_table::get_proc_item(){
	return proc;
}
/* Find table in table map, if not found return NULL */
Symbol_table * Symbol_table::get_table(std:: string name){
	TABLE_MAP::iterator iter = table_map.find(name);
	if(iter!=table_map.end()) 
		return iter->second;
	else
		return NULL;
}

/* Get full name of table, which should be unique. */
std::string Symbol_table:: get_proc_name(){
	return proc->get_name();
}

int Symbol_table::get_level(){
	return level;
}
int Symbol_table::get_args_size(){
	return args_size;
}
int Symbol_table::get_stack_size(){
	return stack_size;
}


/* Get arguments of this procedure or function. */
std::vector<Symbol_item *> * Symbol_table:: get_arg_list(){
	std::vector<Symbol_item*> * args = new std::vector<Symbol_item *>();
	std::vector<Symbol_item*>::iterator iter = seq_list.begin();
	while(iter!=seq_list.end()&& ((*iter)->get_kind()==Symbol_item::PARA||(*iter)->get_kind()==Symbol_item::PARA_VAR)){
		args->push_back(*iter);
		iter++;
	}
	return args;
}

/* Insert new child table to Table Map ,
	return a pointer to the new table.  */
Symbol_table *  Symbol_table:: add_child(Symbol_item * _proc ){
	Symbol_table * child  = new  Symbol_table(_proc, this, level+1);
	table_map[_proc->get_name()] = child;
	return child;
}

/* Print content. */
void Symbol_table::print_table(std::ostream out){
	out<<"\n"<<get_proc_name()<<"\tLevel:"<<level<<"\tStack_size:"<<stack_size<<"\tArgu_size:"<<args_size;
	out<<endl;
//	std::cout<<"Table of "<<name<<"---------"<<endl;
//  SYMBOL_MAP::iterator s_iter = symbol_map.begin();
// 	while(s_iter!=symbol_map.end()){
// 		Symbol_item * item = s_iter->second;
// 		std::cout<<"\t"<<item->get_name()<<"\t"<<item->get_kind_name()<<"\t"
// 			<<item->get_type_name()<<"\t"<<item->get_value()<<"\t"<<item->get_size()<<endl;
// 		s_iter++;
// 	}
	std::vector<Symbol_item*>::iterator s_iter = seq_list.begin();
	while(s_iter!=seq_list.end()){
		Symbol_item * item = *s_iter;
		std::stringstream ss;
		if(item->get_offset()==0)
			ss<<"-";
		else
			ss<<item->get_offset();

		out<<"\t"<<item->get_name()<<"\t"<<item->get_kind_name()<<"\t"
			<<item->get_type_name()<<"\t"<<item->get_value()<<"\t"<<item->get_size()<<"\t"<<ss.str()<<endl;
  		s_iter++;
	}
	
	TABLE_MAP::iterator t_iter = table_map.begin();
	while(t_iter!=table_map.end()){
		t_iter->second->print_table(out);
		t_iter++;
	}
	
}


/* Calculate offset of VAR , PARA, PARA_VAR , ARRAY based on base pointer(ebp).
   Sequence list of symbol item is used here. 

   STACK STRUCTURE
-------------------------------
   arg_1			push arg_1
   arg_2			push arg_2
   бн
   arg_n			push arg_n
   return_addr		call proc
   pre_bp			push ebp ;  mov ebp, esp ;	sub esp,stack_size
   return_value		//if it is a function , return value saved here.
   var_1			var 1
   var_2			var 2
   бн	
   var_n			var n
					
					leave
					retn arg_size
-------------------------------

*/
void Symbol_table:: cal_offset(){
	std::vector<Symbol_item*>::iterator iter = seq_list.begin();

	if(level==0 ){
		/* Root proc only save tmp in stack.*/
		int cur_offset= 0;
		while(iter!=seq_list.end()){
			if((*iter)->get_kind()==Symbol_item::TEMP||
				(*iter)->get_kind()==Symbol_item::TEMP_ADD){
				cur_offset-= (*iter)->get_size();				
				(*iter)->set_offset(cur_offset);
			}else if((*iter)->get_kind()==Symbol_item::FUNC){
				(*iter)->set_offset(-(*iter)->get_size());
			}
			iter++;
		}
		stack_size = -cur_offset;
		args_size = 0;		
	}else{
	
	/* Calculate sum of argument size.*/
	int arg_size= 0;
	while(iter!=seq_list.end()){
		if((*iter)->get_kind()==Symbol_item::PARA_VAR||(*iter)->get_kind()==Symbol_item::PARA){
			arg_size+= (*iter)->get_size();
		}else if((*iter)->get_kind()==Symbol_item::FUNC){
			(*iter)->set_offset(-(*iter)->get_size());
		}
		iter++;
	}
	

	int cur_offset; // Current offset. 
	int display_size = 0;
	if(level>1)
		display_size = (level -1) * 4;

	// offset 0~4 for ebp, offset 4~8 used to save return address , 8~ 4*level+4 for display.	
	cur_offset= arg_size+8+ display_size; 
	// Include arguments and display pointer.
	args_size= arg_size + display_size;
	
	iter= seq_list.begin();
	while(iter!=seq_list.end()){
		if((*iter)->get_kind()==Symbol_item::PARA_VAR||(*iter)->get_kind()==Symbol_item::PARA){
			cur_offset-= (*iter)->get_size();	// increate offset		
			(*iter)->set_offset(cur_offset);	// save offset.
		}
		iter++;
	}
	if(proc->get_kind()==Symbol_item::PROC)
		cur_offset= 0;		// offset 0 for ebp
	else// then must be FUNCTION
		cur_offset = -4;	// offset -4~0 for return value. 
	
	iter= seq_list.begin();	
	while(iter!=seq_list.end()){
		if((*iter)->get_kind()==Symbol_item::VAR||
			(*iter)->get_kind()==Symbol_item::TEMP||			
			(*iter)->get_kind()==Symbol_item::TEMP_ADD||
			(*iter)->get_kind()==Symbol_item::ARRAY){			
			cur_offset-= (*iter)->get_size();
			(*iter)->set_offset(cur_offset);
		}
		iter++;
	}
	stack_size = -cur_offset;
	}

	/* Calculate offset recursively*/
	TABLE_MAP::iterator t_iter = table_map.begin();
	while(t_iter!=table_map.end()){
		t_iter->second->cal_offset();
		t_iter++;
	}

}

/* Rename global variable and const identifier. */
void Symbol_table::rename(){
	std::vector<Symbol_item*>::iterator iter = seq_list.begin();
	while(iter!=seq_list.end()){
		if((*iter)->get_kind()==Symbol_item::CONST||
			(*iter)->get_kind()==Symbol_item::PROC||
			(*iter)->get_kind()==Symbol_item::FUNC){
			/*
			IMPORTANT!! RENAME.
			const rename:	item_name@proc_name
			It is used in object code generation.
			*/
			std::stringstream ss;
			ss<<(*iter)->get_name()<<"@"<<proc->get_name();
			(*iter)->set_name(ss.str());			
		}
		iter++;
	}
	/* Only rename variable in root .*/
	if(level==0){
		iter = seq_list.begin();
		while(iter!=seq_list.end()){
			if((*iter)->get_kind()==Symbol_item::VAR||(*iter)->get_kind()==Symbol_item::ARRAY){
				/*
				IMPORTANT!!
				variable rename:	_item_name
				It is used in object code generation.
				*/
				std::stringstream ss;
				ss<<(*iter)->get_name()<<"$";
				(*iter)->set_name(ss.str());
			}
			iter++;
		}
	}
	/* Rename recursively*/
	TABLE_MAP::iterator t_iter = table_map.begin();
	while(t_iter!=table_map.end()){
		t_iter->second->rename();
		t_iter++;
	}

	
}

/*********************************************
	To make it easier , 
	save char as 4 byte.

	This may change at last.
**********************************************/
void Symbol_table::print_data(std::ostream out){
	std::vector<Symbol_item*>::iterator iter = seq_list.begin();
	while(iter!=seq_list.end()){
		if((*iter)->get_kind()==Symbol_item::CONST){
			out<<(*iter)->get_name()<<" ";
			if((*iter)->get_type()==Symbol_item::CHAR)
				out<<"DWORD "<<(*iter)->get_value()<<std::endl;
				//out<<"DB "<<(*iter)->get_value()<<std::endl;
			else if((*iter)->get_type()==Symbol_item::INT)
				out<<"DWORD "<<(*iter)->get_value()<<std::endl;
			else
				out<<" DB \""<<(*iter)->get_str()<<"\",0"<<std::endl;
		}
		iter++;
	}
	/* Print data recursively*/
	TABLE_MAP::iterator t_iter = table_map.begin();
	while(t_iter!=table_map.end()){
		t_iter->second->print_data(out);
		t_iter++;
	}
	
	/* Only variable in root will allocate.*/
	if(level==0){
		out<<"\n.DATA?"<<std::endl;
		iter = seq_list.begin();
		while(iter!=seq_list.end()){
			if((*iter)->get_kind()==Symbol_item::VAR){
				out<<(*iter)->get_name()<<" ";
				if((*iter)->get_type()==Symbol_item::CHAR)
				//	out<<"DB ?\n";
					out<<"DWORD ?\n";
				else 
					out<<"DWORD ?\n";
			}else if((*iter)->get_kind()==Symbol_item::ARRAY){
				out<<(*iter)->get_name()<<" ";
				if((*iter)->get_type()==Symbol_item::CHAR)
					//	out<<"DB "<<(*iter)->get_size()<<" DUP(?)\n";
					out<<"DWORD "<<(*iter)->get_size()/4<<" DUP(?)\n";
				else 
					out<<"DWORD "<<(*iter)->get_size()/4<<" DUP(?)\n";
			}
			iter++;
		}
	}
}