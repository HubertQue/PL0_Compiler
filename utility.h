#ifndef UTILITY_H
#define UTILITY_H

#pragma warning(disable : 4786)
//#define is_valid(c) ((c>='0'&&c<='9')||(c>='a'&&c<='z')||(c>='A'&&c<='Z')||c=='[')
#include <string>

#define is_digit(c) (c>='0'&&c<='9')
#define is_alpha(c) ((c>='a'&&c<='z')||(c>='A'&&c<='Z'))
#define is_space(c) (c==' '||c=='\n'||c=='\r'||c=='\t'||c=='\v'||c=='\f')
#define is_qoute(c) (c=='\'')
#define is_char(c)	(is_digit(c)||is_alpha(c))


#define is_dqoute(c) (c=='\"')
#define is_string(c) (c>=32&&c<=126&&c!=34)	/* All print char without '\"' */

std::string to_string(int);
std::string to_string(char);
std::string to_string(char *);
std::string ebp_string(int);
#define _S(str) to_string(str)
#define _EBP(offset) ebp_string(offset)


#endif