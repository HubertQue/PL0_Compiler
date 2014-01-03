#include "DAG_node.h"
#include "basic_block.h"
#include "quaternion.h"

#include <map>
#include <vector>


class DAG_graph{
public:
	DAG_graph(std::vector<Quaternion*>& codes);
	void build();
	void regenerate();
	void print_content(std::ostream &);

private:
	
	bool test_equal(Symbol_item* , Symbol_item*);
	void set_map(Symbol_item*  , DAG_node*);
	void release_leaf(std::vector<Quaternion*> & nodes, Symbol_item * );

	void add_node(Quaternion *);
	
	DAG_node * find_node(Symbol_item *);
	DAG_node * find_node(Quaternion ::op,Symbol_item*, DAG_node * left, DAG_node *right);

	std::map<Symbol_item *, DAG_node *> node_map;	// All nodes including leaves.

	std::vector<DAG_node * > node_list;	// Sequential nodes.

	std::vector<Quaternion*>& codes;

	Quaternion * head;
	Quaternion * tail;

	int count; // for nodes.
};
