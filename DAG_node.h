#include "quaternion.h"
#include <iostream>
#include <vector>


class DAG_node{
public:
	Quaternion::op	opcode;
	DAG_node * left;
	DAG_node * right;
	bool leaf;

	int parents;
	int id;
	bool ready();
	void print(std::ostream out);
	std::vector<Symbol_item *> items;
	Quaternion * to_code();
	DAG_node(Symbol_item *, int);
	DAG_node(Quaternion::op,Symbol_item*,  DAG_node *, DAG_node*, int);

	Symbol_item * item;
private:
	
//	void add_parent(DAG_node *);
//	std::vector<DAG_node *> parents;
};




















