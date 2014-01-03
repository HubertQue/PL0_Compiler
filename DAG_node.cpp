#include "DAG_node.h"


DAG_node::DAG_node(Quaternion::op _opcode,Symbol_item* _item, DAG_node * _left, DAG_node * _right, int _id):opcode(_opcode),item(_item), left(_left), right(_right), id(_id){
	if(left!=NULL) left->parents++;
	if(right!=NULL) right->parents++;
	leaf = false;
	parents = 0;
}
DAG_node::DAG_node(Symbol_item * _item, int _id):item(_item),id(_id){
	opcode = Quaternion::NOP;
	left = NULL;
	right = NULL;
	parents = 0;
	leaf = true;
//	items.push_back(item);
}



bool DAG_node::ready(){
//	return parents.empty();
	return parents==0;
}

//DAG_node::add_parent(DAG_node * p){
//	parents.push_back(p);
//}

void DAG_node::print(std::ostream out){
	out<<"\t"<<id<<"\t";
	out<<Quaternion::get_op_name(opcode)<<"\t";
	if(left!=NULL) out<<left->id;
	out<<"\t";
	if(right!=NULL) out<<right->id;
	out<<"\t";
	if(item!=NULL) out<<item->get_name();
	std::vector<Symbol_item*>::iterator iter = items.begin();
	while(iter!=items.end()){
		out<<"\t";
		Symbol_item* item = *iter;
		if(item!=NULL) out<<item->get_name();
		iter++;
	}
	out<<std::endl;
}

Quaternion * DAG_node::to_code(){
	Symbol_item * src1 = NULL;
	Symbol_item * src2 = NULL;
	Symbol_item * dst = item;
	if (left!=NULL) src1 = left->item;
	if (right!=NULL) src2 = right->item;
	return new Quaternion(opcode,  src1, src2, dst);
}