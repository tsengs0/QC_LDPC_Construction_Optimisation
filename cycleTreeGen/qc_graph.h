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

class TreeNode {
	public:
		TreeNode *parent;
		std::vector<TreeNode> next;
		TreeNode *head;
		_U32 node_number, serial_number, voltage;

		TreeNode();
		void insertNode(_U32 node_id, _U32 serial_id, _U32 voltage_t);
};

/*
typedef struct CycleGenerateTree_t {
	Graph tree;
	std::ve
} CycleGenerateTree;
*/
class QC_Graph {
	public:
		BaseGraph base_graph;
		_U32 target_girth, half_girth;
		TreeNode cycleGenerate_tree;

		
		QC_Graph(_U32 target_girth_t, _U32 N_t, _U32 K_t, _U32 M_t, _U32 lift_degree_t, Graph *base_graph_t);
		//void construct_cycleGenerate_tree(_U32 prev_node, _U32 node_id, _U32 depth);
		_U32 construct_cycleGenerate_tree(TreeNode *curNode, _U32 prev_node, _U32 node_id, _U32 depth, _U32 traversedNode_cnt);
		//void traversal_cycleGenerate_tree(_U32 prev_node, _U32 node_id, _U32 depth);
		void traversal_cycleGenerate_tree(TreeNode *curNode, _U32 depth);
};

#endif // __QC_GRAPH_H