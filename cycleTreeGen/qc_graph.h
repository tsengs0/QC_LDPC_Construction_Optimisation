#ifndef __QC_GRAPH_H
#define __QC_GRAPH_H

#include <vector>
#include "graph.h"

typedef unsigned int _U32;
typedef struct Base_Graph_t {
	_U32 N;
	_U32 K;
	_U32 M;
	_U32 lift_degree;
	Graph *graph_matrix;
} BaseGraph;

typedef struct CycleTree_tuple_3_t {
	_U32 t; // serial number
	int e; // voltage w_e
	_U32 v; // node number of terminating node of the corresponding edge
} CycleTree_tuple_3;

class CycleTree_3tuple_table {
	private:
		std::vector<CycleTree_tuple_3_t> tuple_3;
		_U32 num; // the nubmer of entries in the table

	public:
		CycleTree_3tuple_table();
		void insertCycleTreeTupleTable(_U32 t, int e, _U32 v);
		void showTable();
};

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

class QC_Graph {
	public:
		BaseGraph base_graph;
		_U32 target_girth, half_girth;
		_U32 *cycelTree_nodeNum;
		TreeNode *cycleGenerate_tree;
		CycleTree_3tuple_table *cycleTree_3tuple_table;
		
		QC_Graph(_U32 target_girth_t, _U32 N_t, _U32 K_t, _U32 M_t, _U32 lift_degree_t, Graph *base_graph_t);
		_U32 construct_cycleGenerate_tree(_U32 rootNode, TreeNode *curNode, _U32 prev_node, _U32 node_id, _U32 depth, _U32 traversedNode_cnt);
		void assign_serialNumber(_U32 rootNode);
		void traversal_cycleGenerate_tree(TreeNode *curNode, _U32 depth);
};

#endif // __QC_GRAPH_H