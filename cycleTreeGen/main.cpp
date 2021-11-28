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
	g.addEdge(1, 5, 1);
	g.addEdge(1, 6, 2);
	g.addEdge(1, 7, 3);
	g.addEdge(2, 5, 4);
	g.addEdge(2, 6, 5);
	g.addEdge(3, 6, 6);
	g.addEdge(3, 7, 7);
	g.addEdge(4, 5, 8);
	g.addEdge(4, 7, 9);
	g.addEdge(5, 1, -1);
	g.addEdge(5, 2, -4);
	g.addEdge(5, 4, -8);
	g.addEdge(6, 1, -2);
	g.addEdge(6, 2, -5);
	g.addEdge(6, 3, -6);
	g.addEdge(7, 1, -3);
	g.addEdge(7, 3, -7);
	g.addEdge(7, 4, -9);

	//cout << "Following is Depth First Traversal" << " (starting from vertex 2)" << endl;
	//g.DFS(1);
	
	_U32 traversedNode_cnt;
	QC_Graph qc(8, 4, 1, 3, 9, &g);
	//qc.base_graph.graph_matrix -> DFS(1);
	traversedNode_cnt = qc.construct_cycleGenerate_tree(&(qc.cycleGenerate_tree), 0, 1, 0, 0);
	cout << "=======================================" << endl << "Number of node in the tree: " << traversedNode_cnt << endl;
	//qc.traversal_cycleGenerate_tree(0, 1, 0);
	qc.traversal_cycleGenerate_tree(&(qc.cycleGenerate_tree));
	cout << "=======================================" << endl;
	cout << "(Node_" << qc.cycleGenerate_tree.node_number << ").size: " << qc.cycleGenerate_tree.next.size() << endl;
	cout << "(Node_" << qc.cycleGenerate_tree.next[0].node_number << ").size: " << qc.cycleGenerate_tree.next[0].next.size() << endl;
	cout << "(Node_" << qc.cycleGenerate_tree.next[0].next[0].node_number << ").size: " << qc.cycleGenerate_tree.next[0].next[0].next.size() << endl;
	cout << "(Node_" << qc.cycleGenerate_tree.next[0].next[1].node_number << ").size: " << qc.cycleGenerate_tree.next[0].next[1].next.size() << endl;
	cout << "(Node_" << qc.cycleGenerate_tree.next[0].next[0].next[0].node_number << ").size: " << qc.cycleGenerate_tree.next[0].next[0].next[0].next.size() << endl;
	cout << "(Node_" << qc.cycleGenerate_tree.next[0].next[1].next[0].node_number << ").size: " << qc.cycleGenerate_tree.next[0].next[1].next[0].next.size() << endl;
	cout << "(Node_" << qc.cycleGenerate_tree.next[0].next[0].next[0].next[0].node_number << ").size: " << qc.cycleGenerate_tree.next[0].next[0].next[0].next[0].next.size() << endl;
	cout << "(Node_" << qc.cycleGenerate_tree.next[0].next[0].next[0].next[1].node_number << ").size: " << qc.cycleGenerate_tree.next[0].next[0].next[0].next[1].next.size() << endl;
	cout << "(Node_" << qc.cycleGenerate_tree.next[0].next[1].next[0].next[0].node_number << ").size: " << qc.cycleGenerate_tree.next[0].next[1].next[0].next[0].next.size() << endl;
	cout << "(Node_" << qc.cycleGenerate_tree.next[0].next[1].next[0].next[1].node_number << ").size: " << qc.cycleGenerate_tree.next[0].next[1].next[0].next[1].next.size() << endl;
	cout << "=======================================" << endl;
	cout << "(Node_" << qc.cycleGenerate_tree.next[1].node_number << ").size: "                         << qc.cycleGenerate_tree.next[1].next.size() << endl;
	cout << "(Node_" << qc.cycleGenerate_tree.next[1].next[0].node_number << ").size: "                 << qc.cycleGenerate_tree.next[1].next[0].next.size() << endl;
	cout << "(Node_" << qc.cycleGenerate_tree.next[1].next[1].node_number << ").size: "                 << qc.cycleGenerate_tree.next[1].next[1].next.size() << endl;
	cout << "(Node_" << qc.cycleGenerate_tree.next[1].next[0].next[0].node_number << ").size: "         << qc.cycleGenerate_tree.next[1].next[0].next[0].next.size() << endl;
	cout << "(Node_" << qc.cycleGenerate_tree.next[1].next[1].next[0].node_number << ").size: "         << qc.cycleGenerate_tree.next[1].next[1].next[0].next.size() << endl;
	cout << "(Node_" << qc.cycleGenerate_tree.next[1].next[0].next[0].next[0].node_number << ").size: " << qc.cycleGenerate_tree.next[1].next[0].next[0].next[0].next.size() << endl;
	cout << "(Node_" << qc.cycleGenerate_tree.next[1].next[0].next[0].next[1].node_number << ").size: " << qc.cycleGenerate_tree.next[1].next[0].next[0].next[1].next.size() << endl;
	cout << "(Node_" << qc.cycleGenerate_tree.next[1].next[1].next[0].next[0].node_number << ").size: " << qc.cycleGenerate_tree.next[1].next[1].next[0].next[0].next.size() << endl;
	cout << "(Node_" << qc.cycleGenerate_tree.next[1].next[1].next[0].next[1].node_number << ").size: " << qc.cycleGenerate_tree.next[1].next[1].next[0].next[1].next.size() << endl;
	cout << "=======================================" << endl;
	cout << "(Node_" << qc.cycleGenerate_tree.next[2].node_number << ").size: "                         << qc.cycleGenerate_tree.next[2].next.size() << endl;
	cout << "(Node_" << qc.cycleGenerate_tree.next[2].next[0].node_number << ").size: "                 << qc.cycleGenerate_tree.next[2].next[0].next.size() << endl;
	cout << "(Node_" << qc.cycleGenerate_tree.next[2].next[1].node_number << ").size: "                 << qc.cycleGenerate_tree.next[2].next[1].next.size() << endl;
	cout << "(Node_" << qc.cycleGenerate_tree.next[2].next[0].next[0].node_number << ").size: "         << qc.cycleGenerate_tree.next[2].next[0].next[0].next.size() << endl;
	cout << "(Node_" << qc.cycleGenerate_tree.next[2].next[1].next[0].node_number << ").size: "         << qc.cycleGenerate_tree.next[2].next[1].next[0].next.size() << endl;
	cout << "(Node_" << qc.cycleGenerate_tree.next[2].next[0].next[0].next[0].node_number << ").size: " << qc.cycleGenerate_tree.next[2].next[0].next[0].next[0].next.size() << endl;
	cout << "(Node_" << qc.cycleGenerate_tree.next[2].next[0].next[0].next[1].node_number << ").size: " << qc.cycleGenerate_tree.next[2].next[0].next[0].next[1].next.size() << endl;
	cout << "(Node_" << qc.cycleGenerate_tree.next[2].next[1].next[0].next[0].node_number << ").size: " << qc.cycleGenerate_tree.next[2].next[1].next[0].next[0].next.size() << endl;
	cout << "(Node_" << qc.cycleGenerate_tree.next[2].next[1].next[0].next[1].node_number << ").size: " << qc.cycleGenerate_tree.next[2].next[1].next[0].next[1].next.size() << endl;


	cout << endl;
	return 0;
}