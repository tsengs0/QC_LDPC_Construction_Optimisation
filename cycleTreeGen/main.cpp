#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <map>
#include <vector>
#include <list>
#include "graph.h"
#include "qc_graph.h"

using namespace std;

int main(int argc, char **argv)
{
	Graph g;
	g.addEdge(1-1, 5-1,  1);
	g.addEdge(1-1, 6-1,  2);
	g.addEdge(1-1, 7-1,  3);
	g.addEdge(2-1, 5-1,  4);
	g.addEdge(2-1, 6-1,  5);
	g.addEdge(3-1, 6-1,  6);
	g.addEdge(3-1, 7-1,  7);
	g.addEdge(4-1, 5-1,  8);
	g.addEdge(4-1, 7-1,  9);
	g.addEdge(5-1, 1-1, -1);
	g.addEdge(5-1, 2-1, -4);
	g.addEdge(5-1, 4-1, -8);
	g.addEdge(6-1, 1-1, -2);
	g.addEdge(6-1, 2-1, -5);
	g.addEdge(6-1, 3-1, -6);
	g.addEdge(7-1, 1-1, -3);
	g.addEdge(7-1, 3-1, -7);
	g.addEdge(7-1, 4-1, -9);

	//cout << "Following is Depth First Traversal" << " (starting from vertex 2)" << endl;
	//g.DFS(1);
	
	QC_Graph qc(
		14, //target_girth_t, 
		4, //N_t, 
		1, //K_t, 
		3, //M_t, 
		9, // the number of nonzero element on the base graph
		9, //lift_degree_t, 
		&g //*base_graph_t
	);

	qc.init_weightCoefficientMatrix();
	for(_U32 var_node=0; var_node<qc.base_graph.N; var_node++) {
		qc.construct_cycleGenerate_tree(var_node, &(qc.cycleGenerate_tree[var_node]), 0, var_node, 0, 0); // the node number starts counting from one
		cout << "=======================================" << endl << "Number of node in the tree: " << qc.cycelTree_nodeNum[var_node] << endl;
		qc.assign_serialNumber(var_node);
		qc.traversal_cycleGenerate_tree(&(qc.cycleGenerate_tree[var_node]), 0);
		cout << "3-Tuple formed Table of Cycle Generating Tree " << var_node+1 << endl;
		cout << "-------------------" << endl;
		qc.cycleTree_3tuple_table[var_node].showTable();
		cout << "The number of entries: " << qc.cycleTree_3tuple_table[var_node].num << endl
		     << "-------------------" << endl;
		qc.build_cycleCandidate_list(var_node);
		qc.cycleCandidatePair_list[var_node].showTable();
		cout << "The number of entries: " << qc.cycleCandidatePair_list[var_node].num << endl
		     << "-------------------" << endl;
		 qc.build_weightCoefficientMatrix(var_node);
	}

	
	for(_U32 len=1; len<qc.half_girth-2+1; len++) {
		cout << "======================" <<endl;
		cout << endl << "A(" << (len+1)*2 << "):" << endl;
		qc.weightCoefficientMatrix[len-1].showMatrix();
	}

	cout << endl << endl;

	cout << endl;
	return 0;
}