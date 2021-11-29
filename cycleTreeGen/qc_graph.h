#ifndef __QC_GRAPH_H
#define __QC_GRAPH_H

#include <vector>
#include "graph.h"

typedef unsigned int _U32;
typedef struct Base_Graph_t {
	_U32 N;
	_U32 K;
	_U32 M;
	_U32 nonzero_num; // the number of nonzero element on the base graph
	_U32 lift_degree;
	Graph *graph_matrix;
} BaseGraph;

class TreeNode {
	public:
		TreeNode *parent;
		std::vector<TreeNode> next;
		TreeNode *head;
		_U32 node_number, serial_number;
		int voltage;

		TreeNode();
		void insertNode(TreeNode *ptr, _U32 node_id, _U32 serial_id, int voltage_t);
};

typedef struct CycleTree_tuple_3_t {
	_U32 t; // serial number
	int e; // voltage w_e
	_U32 v; // node number of terminating node of the corresponding edge

	TreeNode *parent; // additional pointer for ease of backtracking process later
	long long parent_entryID;
} CycleTree_tuple_3;

class CycleTree_3tuple_table {
	public:
		std::vector<CycleTree_tuple_3_t> tuple_3;
		_U32 num; // the nubmer of entries in the table
		CycleTree_3tuple_table();
		void insertCycleTreeTupleTable(_U32 t, int e, _U32 v, TreeNode *parent, long long parent_entryID);
		bool isEqual_entries(_U32 ji, _U32 jh);
		void showTable();
};

typedef struct CycleCandidatePair_t {
	_U32 ji;
	_U32 jh;
} CycleCandidatePair;

class CycleCandidatePair_list {
	public:
		std::vector<CycleCandidatePair> candidate;
		_U32 num; // the nubmer of entries in the table
		CycleCandidatePair_list();
		void insertCycleCandidatePair(_U32 ji, _U32 jh);
		void showTable();
};

class WeightCoefficientMatrix {
	public:
		_U32 num;
		_U32 col_num;
		_U32 cycle_length;
		std::vector< std::vector<int> > acc_voltage_vector;
		WeightCoefficientMatrix();
		void insert_vector(int *vec_in, _U32 vec_num);
		void remove_redundancy(_U32 base_vector_id);
		void showMatrix();
};

class QC_Graph {
	public:
		BaseGraph base_graph;
		_U32 target_girth, half_girth;
		_U32 *cycelTree_nodeNum;
		TreeNode *cycleGenerate_tree;
		CycleTree_3tuple_table *cycleTree_3tuple_table;
		CycleCandidatePair_list *cycleCandidatePair_list;
		WeightCoefficientMatrix *weightCoefficientMatrix;
		
		QC_Graph(_U32 target_girth_t, _U32 N_t, _U32 K_t, _U32 M_t, _U32 nonzero_num_t,_U32 lift_degree_t, Graph *base_graph_t);
		_U32 construct_cycleGenerate_tree(_U32 rootNode, TreeNode *curNode, _U32 prev_node, _U32 node_id, _U32 depth, _U32 traversedNode_cnt);
		void assign_serialNumber(_U32 rootNode);
		void traversal_cycleGenerate_tree(TreeNode *curNode, _U32 depth);
		void build_cycleCandidate_list(_U32 rootNode);
		void init_weightCoefficientMatrix();
		void build_weightCoefficientMatrix(_U32 rootNode);
};
#endif // __QC_GRAPH_H