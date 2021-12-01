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
    _U32 local_start;
    //_U32 local_end; 
    //_U32 cnt; cnt=node_num_cuDepth;
    local_start = 0; 
    //local_end = 0;//local_start+cnt-1;
    TreeNode **child_nodes = new TreeNode* [node_num_cuDepth]; // create the branch pointer for all child nodes
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
    }

    _U32 entry, local_cnt; local_cnt=0; entry=local_start;
    _U32 prev_entry, prev_t;
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
    				entry = (local_cnt == 0) ? local_start : (child_nodes[i] -> serial_number == prev_t) ? prev_entry : entry+1;
    				cycleTree_3tuple_table[rootNode].insertCycleTreeTupleTable( 
    					child_nodes[i] -> serial_number, 
    					vol,
    					child_nodes[i] -> next[branch_id].node_number,
    					child_nodes[i],
    					entry
    				);

    				prev_t = child_nodes[i] -> serial_number; prev_entry = entry;
    				local_start += 1; local_cnt += 1;
    				//local_end = local_start+local_cnt-1;
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

void QC_Graph::init_weightCoefficientMatrix()
{
	_U32 vector_col_num, length_search_num;
	vector_col_num = base_graph.nonzero_num;
	length_search_num = half_girth-1; // the length-2 does not exist
	weightCoefficientMatrix = new WeightCoefficientMatrix[length_search_num];
	for(_U32 len_id=0; len_id<length_search_num;len_id++) {
		weightCoefficientMatrix[len_id].col_num = vector_col_num;
		weightCoefficientMatrix[len_id].cycle_length = (len_id+2)*2;

		char outFilename[100];
		sprintf(outFilename, "weight_matrices/weightMatrix_%d.csv", weightCoefficientMatrix[len_id].cycle_length);
		weightCoefficientMatrix[len_id].weightCoefficientMatrix_fd.open(outFilename);
	}
}

void QC_Graph::build_weightCoefficientMatrix(_U32 rootNode)
{
	_U32 vector_col_num;
	vector_col_num = base_graph.nonzero_num;

	_U32 cycleCandidate_num = cycleCandidatePair_list[rootNode].num;
	for(_U32 entry_id=0; entry_id<cycleCandidate_num; entry_id++) {
		long long len_cnt, ji, jh; len_cnt=0;
		int *vector_temp = new int[vector_col_num]();
		ji = cycleCandidatePair_list[rootNode].candidate[entry_id].ji;
		jh = cycleCandidatePair_list[rootNode].candidate[entry_id].jh;

		// Backtracking from ji
		long long entry_ptr_ji; entry_ptr_ji = ji;
		while(entry_ptr_ji != -1) {
			//cout << cycleTree_3tuple_table[rootNode].tuple_3[entry_ptr_ji].v << "->";
			// voltage_index starts from 1 in order to avoid confusion of +0 and -0
			int vol_t; vol_t = cycleTree_3tuple_table[rootNode].tuple_3[entry_ptr_ji].e;
			vector_temp[abs(vol_t)-1] += (vol_t/abs(vol_t));
			entry_ptr_ji = cycleTree_3tuple_table[rootNode].tuple_3[entry_ptr_ji].parent_entryID;		
		}
		//cout << endl;
		// Backtracking from jh
		long long entry_ptr_jh = jh;
		while(!(entry_ptr_jh == -1 || entry_ptr_ji == entry_ptr_jh)) {
			//cout << cycleTree_3tuple_table[rootNode].tuple_3[entry_ptr_jh].v << "->";
			// voltage_index starts from 1 in order to avoid confusion of +0 and -0
			int vol_t; vol_t = cycleTree_3tuple_table[rootNode].tuple_3[entry_ptr_jh].e;
			vector_temp[abs(vol_t)-1] -= (vol_t/abs(vol_t));
			entry_ptr_jh = cycleTree_3tuple_table[rootNode].tuple_3[entry_ptr_jh].parent_entryID;		
		}
	
		len_cnt=0;
		for(_U32 col_id=0; col_id<vector_col_num; col_id++) {
			len_cnt = (vector_temp[col_id] != 0) ? len_cnt+abs(vector_temp[col_id]) : len_cnt;
		}
		

		if(weightCoefficientMatrix[(len_cnt/2)-2].isExist_all(vector_temp) == false)
			weightCoefficientMatrix[(len_cnt/2)-2].insert_vector(vector_temp, vector_col_num);
		//cout << "Cycle.Candidate_" << entry_id << ", len_cnt:" << len_cnt << ", index: " << (len_cnt/2)-2 << endl;
		//cout << "\n------------------------------" << endl;
	}
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
	num += 1;
}

_U32 WeightCoefficientMatrix::length_cal(_U32 vector_id)
{
	_U32 len_acc; len_acc=0;
	for(_U32 col_id=0; col_id<col_num; col_id++)
		len_acc += abs(acc_voltage_vector[vector_id][col_id]);
	return len_acc;
}

// To check if the incoming vector exists in the matrix
bool WeightCoefficientMatrix::isExist(int *vec_0, int *vec_1)
{
	/*
	bool isIdentical; isIdentical=false;
	for(_U32 col_id=0; col_id<col_num; col_id++) {
		if(abs(vec_in[col_id]) != abs(acc_voltage_vector[vector_id][col_id])) 
			return false;
		else 
			isIdentical = true;
	}
	return isIdentical;
	*/
	return 0;
}

bool WeightCoefficientMatrix::isExist_all(int *vec_in)
{
	bool isIdentical; isIdentical=false;
	for(_U32 vector_id=0; vector_id<num; vector_id++) {
		for(_U32 col_id=0; col_id<col_num; col_id++) {
			if(abs(vec_in[col_id]) != abs(acc_voltage_vector[vector_id][col_id])) 
				return false;
			else 
				isIdentical = true;
		}

		if(isIdentical == true) break;
	}

	return isIdentical;
}

bool WeightCoefficientMatrix::remove_redundancy(_U32 base_vector_id)
{
	_U32 index_cnt; index_cnt = 0;
	std::vector< std::vector<int> > unremoved_matrix;

	// At first, to keep the referenced vector
	unremoved_matrix.push_back(acc_voltage_vector[base_vector_id]);

	bool isDirty = false;
	while(index_cnt < num) {
		if(index_cnt != base_vector_id) {
			//bool isIdentical; isIdentical = false;
			for(_U32 col_id=0; col_id<col_num; col_id++) {
				if(
					abs(acc_voltage_vector[index_cnt][col_id]) != 
					abs(acc_voltage_vector[base_vector_id][col_id])
				) {
					//isIdentical = false;
					unremoved_matrix.push_back(acc_voltage_vector[index_cnt]);
					break;
				}
				//else
					//isIdentical = true;
			}

			//if(isIdentical == true)
				//removed_indices.push_back(index_cnt);	
		}

		index_cnt += 1;
	}

	isDirty = (acc_voltage_vector.size() == unremoved_matrix.size()) ? false : true;
	acc_voltage_vector.swap(unremoved_matrix);
	std::vector< std::vector<int> >().swap(unremoved_matrix);
	return isDirty;
}

void WeightCoefficientMatrix::remove_redundancy_all()
{
	_U32 id; id = 0;
	bool isDirty;
	do {
		isDirty = remove_redundancy(id);
		id = (isDirty == true) ? 0 : id+1;
		//cout << "updated id: " << id << endl;
		//showMatrix();
		num = acc_voltage_vector.size();
	}while( id != (acc_voltage_vector.size()-1) );
}

bool WeightCoefficientMatrix::mismatch_check(_U32 base_vector_id)
{
	/*
	_U32 index_cnt; index_cnt = 0;
	std::vector< std::vector<int> > unremoved_matrix;

	// At first, to keep the referenced vector
	unremoved_matrix.push_back(acc_voltage_vector[base_vector_id]);

	_U32 curLen;
	bool isDirty = false;
	while(index_cnt < num) {
		if(index_cnt != base_vector_id) {
			curLen = length_cal(index_cnt);

			/////////////////////////////////////////////////////////
			//bool isIdentical; isIdentical = false;
			for(_U32 col_id=0; col_id<col_num; col_id++) {
				if(
					abs(acc_voltage_vector[index_cnt][col_id]) != 
					abs(acc_voltage_vector[base_vector_id][col_id])
				) {
					//isIdentical = false;
					unremoved_matrix.push_back(acc_voltage_vector[index_cnt]);
					break;
				}
				//else
					//isIdentical = true;
			}
			/////////////////////////////////////////////////////////////
			//if(isIdentical == true)
				//removed_indices.push_back(index_cnt);	
		}

		index_cnt += 1;
	}

	isDirty = (acc_voltage_vector.size() == unremoved_matrix.size()) ? false : true;
	acc_voltage_vector.swap(unremoved_matrix);
	std::vector< std::vector<int> >().swap(unremoved_matrix);
	return isDirty;
	*/
	return 0;
}

void WeightCoefficientMatrix::mismatch_check_all()
{
	/*
	_U32 id; id = 0;
	bool isDirty;
	do {
		isDirty = remove_redundancy(id);
		id = (isDirty == true) ? 0 : id+1;
		//cout << "updated id: " << id << endl;
		//showMatrix();
		num = acc_voltage_vector.size();
	}while( id != (acc_voltage_vector.size()-1) );
	*/
}

void WeightCoefficientMatrix::showMatrix(bool isFileOut)
{
	if(isFileOut == true) {
		for(_U32 vec_id=0; vec_id<num; vec_id++) {
			for(_U32 col_id=0; col_id<col_num; col_id++) {
				cout << acc_voltage_vector[vec_id][col_id] << "\t";

				weightCoefficientMatrix_fd << acc_voltage_vector[vec_id][col_id];
				if(col_id != col_num-1) 
					weightCoefficientMatrix_fd << ",";
			}
			cout << endl;
			weightCoefficientMatrix_fd << endl;
		}
		cout << "======================" << endl;
	}
	else {
		for(_U32 vec_id=0; vec_id<num; vec_id++) {
			for(_U32 col_id=0; col_id<col_num; col_id++) {
				cout << acc_voltage_vector[vec_id][col_id] << "\t";
			}
			cout << endl;
		}
		cout << "======================"
			 << endl;
	}
}