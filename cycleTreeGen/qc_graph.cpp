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
	cycelTree_nodeNum = new _U32[base_graph.N];
	cycleGenerate_tree = new TreeNode[base_graph.N];

	cycleTree_3tuple_table = new CycleTree_3tuple_table [base_graph.N];
}

_U32 QC_Graph::construct_cycleGenerate_tree(_U32 rootNode, TreeNode *curNode, _U32 prev_node, _U32 node_id, _U32 depth, _U32 traversedNode_cnt)
{
	_U32 temp; temp = traversedNode_cnt;
	if(node_id < rootNode) return temp;

	if(depth <= half_girth) {
		_U32 edge_cnt; edge_cnt = 0;
		
		if(traversedNode_cnt == 0) {
			curNode -> insertNode(curNode, node_id, 0, base_graph.graph_matrix -> edge_voltage[node_id][edge_cnt]);
			cycelTree_nodeNum[rootNode] += 1;
		}
		else {
			TreeNode temp; 
			curNode -> insertNode(&temp, node_id, 0, base_graph.graph_matrix -> edge_voltage[node_id][edge_cnt]);
			cycelTree_nodeNum[rootNode] += 1;

			curNode -> next.push_back(temp);
			curNode = &(curNode -> next.back());
		}
		
		list<int>::iterator i;
		for (i = base_graph.graph_matrix -> adj[curNode->node_number].begin(); i != base_graph.graph_matrix -> adj[curNode->node_number].end(); ++i) {
			if(prev_node != _U32 (*i)) {
				temp = construct_cycleGenerate_tree(rootNode, curNode, node_id, *i, depth+1, temp+1);
			}
			edge_cnt += 1;
		}
	}

	return temp;
}

void QC_Graph::assign_serialNumber(_U32 rootNode)
{
	_U32 node_num_cuDepth; // the number of nodes at current depth
    TreeNode *root_ptr; root_ptr = &cycleGenerate_tree[rootNode];
    root_ptr -> serial_number = 0;
    node_num_cuDepth = root_ptr -> next.size();

    _U32 node_cnt; node_cnt = 1;

    TreeNode **child_nodes = new TreeNode* [node_num_cuDepth]; // create the branch pointer for all child nodes
    for(_U32 i=0; i<node_num_cuDepth; i++) {
    	child_nodes[i] = &(root_ptr -> next[i]);

    	int vol = base_graph.graph_matrix -> getVoltage(root_ptr->node_number, root_ptr->next[i].node_number);
    	cycleTree_3tuple_table[rootNode].insertCycleTreeTupleTable( 
    		root_ptr -> serial_number, 
    		vol,
    		root_ptr -> next[i].node_number
    	);
    }

    for(_U32 depth=0; depth<half_girth; depth++) {
    	_U32 node_num_nextDepth; node_num_nextDepth=0; 
    	// Assigning the serial number to current parent node
    	for(_U32 i=0; i<node_num_cuDepth; i++) {
    		child_nodes[i] -> serial_number = node_cnt++;
    		node_num_nextDepth += child_nodes[i] -> next.size();

    		// To build the cycle-generating tree table
    		if(depth != half_girth-1) { // the leaf nodes are excluded since there is no associated terminating node
    			_U32 child_branch_num = child_nodes[i] -> next.size();
    			for(_U32 branch_id=0; branch_id<child_branch_num; branch_id++) {
    				int vol = base_graph.graph_matrix -> getVoltage(child_nodes[i]->node_number, child_nodes[i]->next[branch_id].node_number);
    				cycleTree_3tuple_table[rootNode].insertCycleTreeTupleTable( 
    					child_nodes[i] -> serial_number, 
    					vol,
    					child_nodes[i] -> next[branch_id].node_number
    				);
    			}
    		}
    	}

    	TreeNode **temp = new TreeNode* [node_num_nextDepth];
    	node_num_nextDepth = 0;
    	for(_U32 i=0; i<node_num_cuDepth; i++) {
    		_U32 branch_num_temp = child_nodes[i] -> next.size();
    		for(_U32 j=0; j<branch_num_temp; j++) {
    			temp[node_num_nextDepth+j] = &(child_nodes[i] -> next[j]);
    		}
    		node_num_nextDepth += branch_num_temp;
    	}


    	node_num_cuDepth = node_num_nextDepth;
    	delete [] child_nodes;
    	child_nodes = new TreeNode* [node_num_cuDepth];
    	for(_U32 i=0; i<node_num_cuDepth; i++) child_nodes[i] = temp[i];
    }
}

void QC_Graph::traversal_cycleGenerate_tree(TreeNode *curNode, _U32 depth)
{
	cout << curNode -> node_number << " (serialID: " << curNode -> serial_number  << ") -> ";
	for(_U32 i = 0; i<curNode -> next.size(); i++) {
		if(i != 0) {
			for(_U32 j=0; j<depth+1; j++)
				cout << "                  ";
		}	
		traversal_cycleGenerate_tree(&(curNode -> next[i]), depth+1);
	}
	cout << endl;	
}

TreeNode::TreeNode()
{

}

void TreeNode::insertNode(TreeNode *ptr, _U32 node_id, _U32 serial_id, int voltage_t)
{
	ptr -> node_number = node_id;
	ptr -> serial_number = serial_id;
	ptr -> voltage = voltage_t;
}

CycleTree_3tuple_table::CycleTree_3tuple_table()
{
	num = 0; // to reset the capacity status to zero for all tables
}

void CycleTree_3tuple_table::insertCycleTreeTupleTable(_U32 t, int e, _U32 v)
{ 
	tuple_3.push_back({t, e, v});
}

void CycleTree_3tuple_table::showTable()
{
	for(_U32 it=0; it<tuple_3.size(); it++)
		cout << "(" << tuple_3[it].t << ", " << tuple_3[it].e << ", " << tuple_3[it].v << ")" << endl;
}