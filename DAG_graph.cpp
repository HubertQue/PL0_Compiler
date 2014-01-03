#include "DAG_graph.h"


DAG_graph::DAG_graph(std::vector<Quaternion*> & _codes): codes(_codes){
	head = NULL;
	tail = NULL;
	count = 0;
}

void DAG_graph::build(){
	std::vector<Quaternion *>::iterator iter = codes.begin();

	/* Skip block which contains items below. */
	while(iter!= codes.end()){
		Quaternion::op opcode = (*iter)->opcode;
		if( opcode==Quaternion::ARR_LOAD||
			opcode==Quaternion::PUSH||
			opcode==Quaternion::PUSH_VAR||
			opcode == Quaternion::CALL ||
			opcode==Quaternion::READ||
			opcode==Quaternion::WRITE)
			return ;	
		iter++;
	}

	/* Set head and tail code. */
	iter = codes.begin();
	while(iter != codes.end()){
		if(iter == codes.begin()&&
			((*iter)->opcode == Quaternion::PROC||
			(*iter)->opcode== Quaternion::LABEL)){
				head = *iter;
			
		}else if(iter+1 == codes.end()&&
			((*iter)->opcode == Quaternion::END||
			((*iter)->opcode >= Quaternion::JE && (*iter)->opcode <=Quaternion::JMP))){
				tail = *iter;
		}else{
			add_node(*iter);
		}
		iter++;
	}

// 	Now i did this when set node map. 
// 	/*  Add items to node and set value to each nodes. */
// 	std::map<Symbol_item *  ,DAG_node *>::iterator m_iter = node_map.begin();
// 	while(m_iter!= node_map.end()){
// 		Symbol_item * item= m_iter->first;
// 		DAG_node * node = m_iter->second;
// 		node->items.push_back(item);
// 		/* Maybe useless. */
// 		if (node->item == NULL) node->item = item;
// 		m_iter++;
// 	}

	print_content(std::cout);
	regenerate();
	
}

void DAG_graph::add_node(Quaternion * code){
	DAG_node * left = NULL;
	DAG_node * right= NULL;
	if(code->src1!=NULL){
		left = find_node(code->src1);
		set_map(code->src1, left);
	}
	if(code->src2!=NULL){
		right = find_node(code->src2);
		set_map(code->src2, right);	
	}

	if(code->opcode==Quaternion::ASSIGN){
		set_map(code->dst, left);
	}else{
		DAG_node * node = find_node(code->opcode,code->dst, left, right);
		set_map(code->dst, node);
	}	
}

/* Find node in node_map, if not found , generate a leaf node. */
DAG_node * DAG_graph::find_node(Symbol_item * item){
	std::map<Symbol_item *  ,DAG_node *>::iterator iter = node_map.find(item);
	if(iter != node_map.end()){
		return iter->second;
	}
	/* For constant find by value. */
	if(item->get_kind()==Symbol_item::TEMP_CON||item->get_kind()==Symbol_item::CONST){
		std::map<Symbol_item *  ,DAG_node *>::iterator m_iter = node_map.begin();
		while(m_iter!= node_map.end()){
			Symbol_item * tmp_item= m_iter->first;
			DAG_node * node = m_iter->second;
			if(test_equal(tmp_item, item))
				return node;
			m_iter++;
		}
	}

	DAG_node * node = new DAG_node(item, ++count);
	node_list.push_back(node);
	return node;
}

/* Find node in node_list, if not found , generate a node. */
DAG_node * DAG_graph::find_node(Quaternion::op opcode, Symbol_item * item ,  DAG_node * left, DAG_node * right){
	std::vector<DAG_node *>::iterator iter= node_list.begin();
	while(iter != node_list.end()){
		if(!(*iter)->leaf&&(*iter)->opcode == opcode && (*iter)->left == left&& (*iter)->right== right){
			return *iter;
		}
		if(!(*iter)->leaf&&(*iter)->opcode == opcode &&		// for + or * the operands can swap.
			(opcode==Quaternion::ADD ||opcode==Quaternion::MUL) 
			&& (*iter)->left == right&& (*iter)->right== left){
			return *iter;
		}
		iter++;
	}	
	//New node ;
	DAG_node * node = new DAG_node(opcode,item,  left, right , ++count);
	node_list.push_back(node);
	return node;
	
}

void DAG_graph::regenerate(){
	codes.clear();
	if(head!=NULL) codes.push_back(head);
	
	std::vector<DAG_node *> tmp_node_list = node_list;
	std::vector<DAG_node *> stack;
	while(!tmp_node_list.empty()){
		/* Add nodes to stack. */
		std::vector<DAG_node *>::reverse_iterator iter= tmp_node_list.rbegin();
		bool flag = true;
		while(iter!=tmp_node_list.rend()){
			DAG_node * node = *iter;
			if(node->ready()){
				stack.push_back(node);
				if(node->left!=NULL) node->left->parents--;
				if(node->right!=NULL) node->right->parents--;
				tmp_node_list.erase((++iter).base());
				flag = false;
				break;
			}else
				iter++;
		}
		if(flag){
			std::cerr<<"DAG graph export warning: Circle found."<<std::endl;
			break;
		}
	}

	std::vector<DAG_node*>::reverse_iterator r_iter = stack.rbegin();
	while(r_iter!= stack.rend()){
		DAG_node * node = *r_iter;	
		release_leaf(codes, node->item);
		r_iter++;
	}

	r_iter = stack.rbegin();
	while(r_iter!= stack.rend()){
		DAG_node * node = *r_iter;
		std::cout<<node->id<<"\t";
		if(!node->leaf){
			codes.push_back(node->to_code());
		}
		std::vector<Symbol_item *>::iterator iter = node->items.begin();
		while(iter!=node->items.end()){
			/* Assign value to other items of this node.( expect for constant value ) */
			if(*iter!=node->item){
				if ( (*iter)->get_kind()!=Symbol_item::TEMP_CON&&(*iter)->get_kind()!=Symbol_item::CONST){
					codes.push_back(new Quaternion(Quaternion::ASSIGN, node->item,NULL, *iter));
				}
			}
				
			iter++;
		}		
		
		r_iter++;
	}
	std::cout<<"\n";

	if(tail!=NULL) codes.push_back(tail);
}

void DAG_graph::print_content(std::ostream & out){
	std::map<Symbol_item *  ,DAG_node *>::iterator iter = node_map.begin();
	out<<"Node Map:\n\tVAR\tNODE\t OP"<<std::endl;
	while(iter!= node_map.end()){
		Symbol_item * item= iter->first;
		DAG_node * node = iter->second;
		out<<"\t"<< (item!=NULL?item->get_name():"" )<<"\t"<<node->id<<"\t"<< Quaternion::get_op_name(node->opcode)<<std::endl;
		iter++;
	}
	
	std::vector<DAG_node *>::iterator l_iter= node_list.begin();
	out<<"Node List:\n\tNODE\tOP\tLEFT\tRIGHT\tITEM\tITEMs"<<std::endl;
	while(l_iter!=node_list.end()){
		(*l_iter)->print(out);
		l_iter++;
	}
}

bool DAG_graph::test_equal(Symbol_item* a , Symbol_item * b){
	if(a==b) return true;
	/* For constants , test whether value equals. */
	if((a->get_kind()==Symbol_item::CONST||a->get_kind()==Symbol_item::TEMP_CON)
		&& (b->get_kind()==Symbol_item::CONST||b->get_kind()==Symbol_item::TEMP_CON)){
		if(a->get_value()==b->get_value())
			return true;
	}

	return false;
}

void DAG_graph:: release_leaf(std::vector<Quaternion*> & codes, Symbol_item * item){
	std::vector<DAG_node *>::iterator iter = node_list.begin();

	while(iter!=node_list.end()){
		DAG_node * node = *iter;
		if(node->leaf&&node_map[item]!=node){
			if(node->item==item && !node->items.empty()){
				std::vector<Symbol_item* >::iterator i_iter = node->items.begin();				
				node->item = *i_iter;
				node->items.erase(i_iter);
				release_leaf(codes, node->item);
				codes.push_back(new Quaternion(Quaternion::ASSIGN, item, NULL, node->item));				
			}
		}
		iter++;
	}
}

void DAG_graph:: set_map(Symbol_item * item , DAG_node * node ){
	std::map<Symbol_item *  ,DAG_node *>::iterator iter = node_map.find(item);
	
	/* If not found, set map , add item to node. */
	if(iter == node_map.end()){
		node_map[item]= node;
		node->items.push_back(item);
		return;
	}
	/* If found itself, just return .*/
	else if (iter->second == node){
		return;
	}
	/* Else delete item from that node , set map , add item to this node.*/
	else{		
		std::vector<Symbol_item *> ::iterator it = iter->second->items.begin();
		while(it!=iter->second->items.end()){
			if(*it==item) it= iter->second->items.erase(it);
			else
				it++;
		}
		node_map[item]= node;
		node->items.push_back(item);
	}
}
