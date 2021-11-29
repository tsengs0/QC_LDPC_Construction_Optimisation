#include <iostream>
#include "qc_graph.h"

using namespace std;

QC_Graph::QC_Graph(_U32 target_girth_t, _U32 N_t, _U32 K_t, _U32 M_t, _U32 nonzero_num_t, _U32 lift_degree_t, Graph *base_graph_t)
{
	target_girth = target_girth_t;
	half_girth = target_girth / 2;
	base_graph.N = N_t;
	base_graph.K = K_t;
	base_graph.M = M_t;
	base_graph.nonzero_num = nonzero_num_t;
	base_graph.lift_degree = lift_degree_t;
	base_graph.graph_matrix = base_graph_t;
	cycelTree_nodeNum = new _U32[base_graph.N];
	cycleGenerate_tree = new TreeNode[base_graph.N];
	cycleTree_3tuple_table = new CycleTree_3tuple_table[base_graph.N];
	cycleCandidatePair_list = new CycleCandidatePair_list[base_graph.N];
}

_U32 QC_Graph::construct_cycleGenerate_tree(_U32 rootNode, TreeNode *curNode, _U32 prev_node, _U32 node_id, _U32 depth, _U32 traversedNode_cnt)
{
	_U32 temp; temp = traversedNode_cnt;
	if(node_id < rootNode) return temp;

	if(depth <= half_girth) {
		_U32 edge_cnt; edge_cnt = 0;
		
		if(traversedNode_cnt == 0) {
			curNode -> insertNode(curNode, node_id, 0, base_graph.graph_matrix -> edge_voltage[node_id][edge_cnt]);
			curNode -> parent = NULL;
			cycelTree_nodeNum[rootNode] += 1;
		}
		else {
			TreeNode temp; 
			curNode -> insertNode(&temp, node_id, 0, base_graph.graph_matrix -> edge_voltage[node_id][edge_cnt]);
			cycelTree_nodeNum[rootNode] += 1;

			curNode -> next.push_back(temp);
			curNode -> next.back().parent = curNode;
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
    long long **parent_entryID, **parent_entryID_local; parent_entryID = new long long* [node_num_cuDepth];
    for(_U32 i=0; i<node_num_cuDepth; i++) {
    	child_nodes[i] = &(root_ptr -> next[i]);

    	int vol = base_graph.graph_matrix -> getVoltage(root_ptr->node_number, root_ptr->next[i].node_number);
    	cycleTree_3tuple_table[rootNode].insertCycleTreeTupleTable( 
    		root_ptr -> serial_number, 
    		vol,
    		root_ptr -> next[i].node_number,
    		root_ptr,
    		-1
    	);

    	parent_entryID[i] = new long long[ child_nodes[i] -> next.size() ];
    	for(_U32 temp=0; temp<child_nodes[i] -> next.size(); temp++) parent_entryID[i][temp] = cycleTree_3tuple_table[rootNode].num-1;
    }

    for(_U32 depth=0; depth<half_girth; depth++) {
    	_U32 node_num_nextDepth; node_num_nextDepth=0; 

		_U32 precal_branch_num; precal_branch_num=0;
		for(_U32 i=0; i<node_num_cuDepth; i++)
			for(_U32 branch_id=0; branch_id<child_branch_num; branch_id++)
				precal_branch_num += 1;
		parent_entryID_local = new long long* [precal_branch_num];

    	// Assigning the serial number to current parent node
    	precal_branch_num=0;
    	for(_U32 i=0; i<node_num_cuDepth; i++) {
    		child_nodes[i] -> serial_number = node_cnt++;
    		node_num_nextDepth += child_nodes[i] -> next.size();

    		// To build the cycle-generating tree table
    		if(depth != half_girth-1) { // the leaf nodes are excluded since there is no associated terminating node
    			_U32 child_branch_num = child_nodes[i] -> next.size();
    			_U32 temp_cnt; temp_cnt=0;
    			precal_branch_num[precal_branch_num] = new long long[child_branch_num];
    			for(_U32 branch_id=0; branch_id<child_branch_num; branch_id++) {
    				int vol = base_graph.graph_matrix -> getVoltage(child_nodes[i]->node_number, child_nodes[i]->next[branch_id].node_number);
    				cycleTree_3tuple_table[rootNode].insertCycleTreeTupleTable( 
    					child_nodes[i] -> serial_number, 
    					vol,
    					child_nodes[i] -> next[branch_id].node_number,
    					child_nodes[i],
    					parent_entryID[i][branch_id]
    				);
    				precal_branch_num[precal_branch_num][branch_id] = cycleTree_3tuple_table[rootNode].num-1;
    				precal_branch_num += 1;
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

    	parent_entryID = new _U32* [node_num_nextDepth];
    	node_num_cuDepth = node_num_nextDepth;
    	delete [] child_nodes;
    	child_nodes = new TreeNode* [node_num_cuDepth];
    	for(_U32 i=0; i<node_num_cuDepth; i++) {
    		parent_entryID[i] = new _U32[ temp[i] -> next.size() ];
    		for(_U32 temp = 0; temp<temp[i] -> next.size(); temp++)

    		child_nodes[i] = temp[i];
    	}
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

void QC_Graph::build_cycleCandidate_list(_U32 rootNode)
{
	//cycleCandidatePair_list.insertCycleCandidatePair()
	_U32 search_num = cycleTree_3tuple_table[rootNode].num-1;
	for(_U32 entry_id=1; entry_id<search_num; entry_id++) {
		for(_U32 comp_id=0; comp_id<entry_id; comp_id++) {
			if(cycleTree_3tuple_table[rootNode].isEqual_entries(entry_id, comp_id) == true)
				cycleCandidatePair_list[rootNode].insertCycleCandidatePair(entry_id, comp_id);
		}
	}
}

void QC_Graph::build_weightCoefficientMatrix(_U32 rootNode)
{
	/*
	_U32 vector_col_num, length_search_num;
	vector_col_num = base_graph.nonzero_num;
	length_search_num = half_girth-2; // the length-2 does not exist
	weightCoefficientMatrix = new WeightCoefficientMatrix[length_search_num];
	for(_U32 len_id=0; len_id<length_search_num;len_id++) {
		weightCoefficientMatrix[len_id].col_num = vector_col_num;
		weightCoefficientMatrix[len_id].cycle_length = (len_id+2)*2;
	}

	_U32 cycleCandidate_num = cycleCandidatePair_list[rootNode].num;
	for(_U32 entry_id=0; entry_id<cycleCandidate_num; entry_id++) {
		_U32 len_cnt, ji, jh; len_cnt=1;
		int *vector_temp = new int[vector_col_num]();
		ji = cycleCandidatePair_list[rootNode].candidate[entry_id].ji;
		jh = cycleCandidatePair_list[rootNode].candidate[entry_id].jh;

		// Backtracking from ji
		int entry_ptr, prev_ptr; entry_ptr = ji; prev_ptr = 900000; // dummy value
		while(!(entry_ptr == -1)) {
			cout << cycleTree_3tuple_table[rootNode].tuple_3[entry_ptr].v << "->";
			// voltage_index starts from 1 in order to avoid confusion of +0 and -0
			int vol_t; vol_t = cycleTree_3tuple_table[rootNode].tuple_3[entry_ptr].e;
			vector_temp[abs(vol_t)-1] += (vol_t/abs(vol_t));
		 	_U32 dummy; dummy = cycleTree_3tuple_table[rootNode].tuple_3[entry_ptr].t;
			prev_ptr = entry_ptr;
			entry_ptr = dummy-1;//(dummy == 0) ? 0 : dummy-1;
			
			len_cnt += 1; 
			//cout << "len_cnt: " << len_cnt << endl;
		}
		cout << endl;
		for(int aaa=0; aaa<vector_col_num; aaa++) cout << "["<< aaa <<"]:" << vector_temp[aaa] << "\t"; cout << endl;
		cout << "\n------------------------------" << endl;
		// Backtracking from jh
		entry_ptr = jh; prev_ptr = 900000; // dummy value
		while(!(entry_ptr == -1)) {
			 //cout << "@" << entry_ptr << endl;
			cout << cycleTree_3tuple_table[rootNode].tuple_3[entry_ptr].v << "->";
			 //cout << "!" << entry_ptr << endl;
			// voltage_index starts from 1 in order to avoid confusion of +0 and -0
			int vol_t; vol_t = cycleTree_3tuple_table[rootNode].tuple_3[entry_ptr].e;
			 //cout << "?" << entry_ptr << endl;
			vector_temp[abs(vol_t)-1] -= (vol_t/abs(vol_t));
			 //cout << "@" << entry_ptr << endl;
			_U32 dummy; dummy = cycleTree_3tuple_table[rootNode].tuple_3[entry_ptr].t;
			 //cout << "#" << entry_ptr << endl;
			prev_ptr = entry_ptr;
			entry_ptr = dummy-1;//(dummy == 0) ? 0 : dummy-1;
			
			 //cout << "*" << entry_ptr << endl;
			len_cnt += 1;
			//cout << "len_cnt: " << len_cnt << endl;
		}
		cout << endl;
		for(int aaa=0; aaa<vector_col_num; aaa++) cout << "["<< aaa <<"]:" << vector_temp[aaa] << "\t"; cout << endl;
		cout << "len_cnt:" << len_cnt << ", index: " << (len_cnt/2)-2 << endl;
		weightCoefficientMatrix[(len_cnt/2)-2].insert_vector(vector_temp, vector_col_num);
	}
	*/
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

void CycleTree_3tuple_table::insertCycleTreeTupleTable(_U32 t, int e, _U32 v, TreeNode *parent, long long parent_entryID)
{ 
	tuple_3.push_back({t, e, v, parent, parent_entryID});
	num += 1;
}

_U32 CycleTree_3tuple_table::entryQuery(_U32 t, _U32 v)
{
	_U32 cnt; cnt=0;
	while(1) {
		if(tuple_3.t == t && tuple_3.v == v)
			return cnt;
		else
			cnt += 1;
	}
}

bool CycleTree_3tuple_table::isEqual_entries(_U32 ji, _U32 jh)
{
	return (tuple_3[ji].v == tuple_3[jh].v) ? true : false;
}

void CycleTree_3tuple_table::showTable()
{
	for(_U32 it=0; it<tuple_3.size(); it++)
		cout << "Parent:" << tuple_3[it].parent_entryID << ", (" << tuple_3[it].t << ", " << tuple_3[it].e << ", " << tuple_3[it].v << ")" << endl;
}

CycleCandidatePair_list::CycleCandidatePair_list()
{
	num = 0;
}

void CycleCandidatePair_list::insertCycleCandidatePair(_U32 ji, _U32 jh)
{
	candidate.push_back({ji, jh});
	num += 1;
}

void CycleCandidatePair_list::showTable()
{
	for(_U32 it=0; it<candidate.size(); it++)
		cout << "(" << candidate[it].ji << ", " << candidate[it].jh << ")" << endl;	
}

WeightCoefficientMatrix::WeightCoefficientMatrix()
{
	num = 0;
}

void WeightCoefficientMatrix::insert_vector(int *vec_in, _U32 vec_num)
{
	std::vector<int> temp;
	for(_U32 vec_id=0; vec_id<vec_num; vec_id++)
		temp.push_back(vec_in[vec_id]);
	acc_voltage_vector.push_back(temp);
}

void WeightCoefficientMatrix::remove_redundancy(_U32 base_vector_id)
{

}

void WeightCoefficientMatrix::showMatrix()
{

}