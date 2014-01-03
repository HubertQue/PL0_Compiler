#include <string>
#include <sstream>

using std::string;
using std::stringstream;

string to_string(int value){
	stringstream ss;
	ss<<value;
	return ss.str();
}

string to_string(char value){
	stringstream ss;
	ss<<value;
	return ss.str();
}

string to_string(char *value ){
	stringstream ss;
	ss<<value;
	return ss.str();	
}

string ebp_string(int offset){
	std::string addr= "";
	if(offset>=0)
		addr= "DWORD PTR [EBP+" +to_string(offset)+"]";
	else 
		addr= "DWORD PTR [EBP"  +to_string(offset)+"]";
	return addr;			
}

