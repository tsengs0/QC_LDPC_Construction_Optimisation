#include <iostream>
#include "qc_graph.h"

using namespace std;

QC_Graph::QC_Graph(_U32 target_girth_t, _U32 N_t, _U32 K_t, _U32 M_t, _U32 lift_degree_t, Graph *base_graph_t)
{
	target_girth = target_girth_t;
	half_girth = target_girth / 2;
	base_graph.N = N_t;
	base_graph.K = K_t;
	base_graph.M = M_t;
	base_graph.lift_degree = lift_degree_t;
	base_graph.graph_matrix = base_graph_t;
}

/*
void QC_Graph::construct_cycleGenerate_tree(_U32 prev_node, _U32 node_id, _U32 depth)
{
	if(depth <= half_girth) {
		_U32 edge_cnt; edge_cnt = 0;

		list<int>::iterator i;
		for (i = base_graph.graph_matrix -> adj[node_id].begin(); i != base_graph.graph_matrix -> adj[node_id].end(); ++i) {
			if(prev_node != _U32 (*i)) {
				construct_cycleGenerate_tree(node_id, *i, depth+1);
				cycleGenerate_tree.addEdge(node_id, *i, base_graph.graph_matrix -> edge_voltage[node_id][edge_cnt]);
				cout << "Node_id: " << node_id << ", sink_node: " << *i << ", Vol: " << base_graph.graph_matrix -> edge_voltage[node_id][edge_cnt] << endl;
			}
			cout << endl;
			edge_cnt += 1;	
		}
	}
}

void QC_Graph::traversal_cycleGenerate_tree(_U32 prev_node, _U32 node_id, _U32 depth)
{
	if(depth <= half_girth) {
		_U32 edge_cnt; edge_cnt = 0;
	
		cout << node_id << " -> ";
		list<int>::iterator i;
		for (i = cycleGenerate_tree.adj[node_id].begin(); i != cycleGenerate_tree.adj[node_id].end(); ++i) {
			if(prev_node != _U32 (*i)) {
				traversal_cycleGenerate_tree(node_id, *i, depth+1);
			}
			edge_cnt += 1;	
		}
		cout << endl;
	}
}
*/
_U32 QC_Graph::construct_cycleGenerate_tree(TreeNode *curNode, _U32 prev_node, _U32 node_id, _U32 depth, _U32 traversedNode_cnt)
{
	_U32 temp; temp = traversedNode_cnt;
	if(node_id == 1) return temp;

	if(depth <= half_girth) {
		_U32 edge_cnt; edge_cnt = 0;
		
		if(traversedNode_cnt == 0) {
			curNode -> node_number = node_id;//insertNode(node_id, 0, base_graph.graph_matrix -> edge_voltage[node_id][edge_cnt]);
			curNode -> serial_number = 0;
			curNode -> voltage = base_graph.graph_matrix -> edge_voltage[node_id][edge_cnt];
			cout << "1) Node:" << node_id << endl;
		}
		else {
			TreeNode temp; 
			temp.node_number = node_id;
			temp.serial_number = 0;
			temp.voltage = base_graph.graph_matrix -> edge_voltage[node_id][edge_cnt];

			curNode -> next.push_back(temp);
			cout << "2) " << curNode -> node_number << "-" << node_id << endl;
			curNode = &(curNode -> next.back());
		}
		cout << "(Node_" << curNode->node_number << ").size: " << curNode->next.size() << endl;
		
		list<int>::iterator i;
		for (i = base_graph.graph_matrix -> adj[curNode->node_number].begin(); i != base_graph.graph_matrix -> adj[curNode->node_number].end(); ++i) {
			if(prev_node != _U32 (*i)) {
				//if(curNode -> next.size() == 0) // the first successor to be added
					temp = construct_cycleGenerate_tree(curNode, node_id, *i, depth+1, temp+1);
				//else
				//	temp = construct_cycleGenerate_tree(&(curNode -> next.back()), node_id, *i, depth+1, temp+1);
				cout << "Node_id: " << curNode -> node_number << ", sink_node: " << *i << ", Vol: " << base_graph.graph_matrix -> edge_voltage[node_id][edge_cnt] << endl;
				//curNode -> insertNode(*i, 0, base_graph.graph_matrix -> edge_voltage[node_id][edge_cnt]);
				//cout << "Node_id: " << node_id << ", sink_node: " << *i << ", Vol: " << base_graph.graph_matrix -> edge_voltage[node_id][edge_cnt] << endl;
				
			}
			cout << endl;
			edge_cnt += 1;
		}
	}

	return temp;
}

void QC_Graph::traversal_cycleGenerate_tree(TreeNode *curNode, _U32 depth)
{
	cout << curNode -> node_number << " -> ";
	for(_U32 i = 0; i<curNode -> next.size(); i++) {
		if(i != 0) {
			for(_U32 j=0; j<depth+1; j++)
				cout << "     ";
		}	
		traversal_cycleGenerate_tree(&(curNode -> next[i]), depth+1);
	}
	cout << endl;	
}

TreeNode::TreeNode()
{

}

void TreeNode::insertNode(_U32 node_id, _U32 serial_id, _U32 voltage_t)
{
	TreeNode temp; 
	temp.node_number = node_id;
	temp.serial_number = serial_id;
	temp.voltage = voltage_t;

	next.push_back(temp);
	cout << node_number << "===> after added " << node_id << " the size is "<< next.size() << endl;
	//head -> next.push_back(temp);
	//next.back().parent = head;
	//head = &next.back();
}