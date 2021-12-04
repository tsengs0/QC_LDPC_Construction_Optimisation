#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <map>
#include <vector>
#include <list>
#include <string>
#include <sstream>
#include "graph.h"
#include "qc_graph.h"

using namespace std;

//#define _3_10_GIRTH_8
//#define MACKAY_REGULAR_504_1008
#define IEEE_802_16E_N2304_R50_Z96

#ifdef MACKAY_REGULAR_504_1008
	#define base_graph_row 6
	#define base_graph_col 12
	#define dc 6
	#define dv 3
#endif // MACKAY_REGULAR_504_1008

#ifdef IEEE_802_16E_N2304_R50_Z96
	#define base_graph_row 12
	#define base_graph_col 24
	#define dc_max 7
	#define dv_max 6
#endif // IEEE_802_16E_N2304_R50_Z96

int main(int argc, char **argv)
{
    int upperGirth = 8;
    string INPUT_FILENAME = "";
    string OUTPUT_FILENAME;
    
    if(argc != 7) {
    	cerr << "Usage:\ncycleTreeGen -file graphIn.txt -out out(default) -upperGirth 8(default)\n";
   		exit(1);
    }
    
    for (int i = 1; i + 1 < argc; ++i) {
        if (string(argv[i]) == "-file") {
            INPUT_FILENAME = argv[i + 1];
            ++i;
            continue;
        }
        if (string(argv[i]) == "-out") {
            OUTPUT_FILENAME = argv[i + 1];
            ++i;
            continue;
        }
        if (string(argv[i]) == "-upperGirth") {
            string strDep = argv[i + 1];
            stringstream sstrDep(strDep);
            sstrDep >> upperGirth;
            ++i;
            continue;
        }
    }


	Graph g;
#ifdef _3_10_GIRTH_8
	// Var-0
	g.addEdge(1-1, 11-1,  1); // VNU -> CNUs
	g.addEdge(1-1, 12-1,  2); // VNU -> CNUs
	g.addEdge(1-1, 13-1,  3); // VNU -> CNUs
	g.addEdge(11-1, 1-1, -1); // CNUs -> VNU
	g.addEdge(12-1, 1-1, -2); // CNUs -> VNU
	g.addEdge(13-1, 1-1, -3); // CNUs -> VNU
	// Var-1
	g.addEdge(2-1, 11-1,  4); // VNU -> CNUs
	g.addEdge(2-1, 12-1,  5); // VNU -> CNUs
	g.addEdge(2-1, 13-1,  6); // VNU -> CNUs
	g.addEdge(11-1, 2-1, -4); // CNUs -> VNU
	g.addEdge(12-1, 2-1, -5); // CNUs -> VNU
	g.addEdge(13-1, 2-1, -6); // CNUs -> VNU
	// Var-2
	g.addEdge(3-1, 11-1, 7); // VNU -> CNUs
	g.addEdge(3-1, 12-1, 8); // VNU -> CNUs
	g.addEdge(3-1, 13-1, 9); // VNU -> CNUs
	g.addEdge(11-1, 3-1, -7); // CNUs -> VNU
	g.addEdge(12-1, 3-1, -8); // CNUs -> VNU
	g.addEdge(13-1, 3-1, -9); // CNUs -> VNU
	// Var-3
	g.addEdge(4-1, 11-1, 10); // VNU -> CNUs
	g.addEdge(4-1, 12-1, 11); // VNU -> CNUs
	g.addEdge(4-1, 13-1, 12); // VNU -> CNUs
	g.addEdge(11-1, 4-1, -10); // CNUs -> VNU
	g.addEdge(12-1, 4-1, -11); // CNUs -> VNU
	g.addEdge(13-1, 4-1, -12); // CNUs -> VNU
	// Var-4
	g.addEdge(5-1, 11-1, 13); // VNU -> CNUs
	g.addEdge(5-1, 12-1, 14); // VNU -> CNUs
	g.addEdge(5-1, 13-1, 15); // VNU -> CNUs
	g.addEdge(11-1, 5-1,  -13); // CNUs -> VNU
	g.addEdge(12-1, 5-1,  -14); // CNUs -> VNU
	g.addEdge(13-1, 5-1,  -15); // CNUs -> VNU
	// Var-5
	g.addEdge(6-1, 11-1, 16); // VNU -> CNUs
	g.addEdge(6-1, 12-1, 17); // VNU -> CNUs
	g.addEdge(6-1, 13-1, 18); // VNU -> CNUs
	g.addEdge(11-1, 6-1, -16); // CNUs -> VNU
	g.addEdge(12-1, 6-1, -17); // CNUs -> VNU
	g.addEdge(13-1, 6-1, -18); // CNUs -> VNU
	// Var-6
	g.addEdge(7-1, 11-1, 19); // VNU -> CNUs
	g.addEdge(7-1, 12-1, 20); // VNU -> CNUs
	g.addEdge(7-1, 13-1, 21); // VNU -> CNUs
	g.addEdge(11-1, 7-1, -19); // CNUs -> VNU
	g.addEdge(12-1, 7-1, -20); // CNUs -> VNU
	g.addEdge(13-1, 7-1, -21); // CNUs -> VNU
	// Var-7
	g.addEdge(8-1, 11-1, 22); // VNU -> CNUs
	g.addEdge(8-1, 12-1, 23); // VNU -> CNUs
	g.addEdge(8-1, 13-1, 24); // VNU -> CNUs
	g.addEdge(11-1, 8-1, -22); // CNUs -> VNU
	g.addEdge(12-1, 8-1, -23); // CNUs -> VNU
	g.addEdge(13-1, 8-1, -24); // CNUs -> VNU
	// Var-8
	g.addEdge(9-1, 11-1, 25); // VNU -> CNUs
	g.addEdge(9-1, 12-1, 26); // VNU -> CNUs
	g.addEdge(9-1, 13-1, 27); // VNU -> CNUs
	g.addEdge(11-1, 9-1, -25); // CNUs -> VNU
	g.addEdge(12-1, 9-1, -26); // CNUs -> VNU
	g.addEdge(13-1, 9-1, -27); // CNUs -> VNU
	// Var-9
	g.addEdge(10-1, 11-1, 28); // VNU -> CNUs
	g.addEdge(10-1, 12-1, 29); // VNU -> CNUs
	g.addEdge(10-1, 13-1, 30); // VNU -> CNUs
	g.addEdge(11-1, 10-1, -28); // CNUs -> VNU
	g.addEdge(12-1, 10-1, -29); // CNUs -> VNU
	g.addEdge(13-1, 10-1, -30); // CNUs -> VNU
#endif // _3_10_GIRTH_8

#ifdef MACKAY_REGULAR_504_1008
	/*====================================================================================================*/
	// (3, 6)-regular base grpah for final construction of (1008, 504) protograh code
	unsigned int baseGraph_rowWise_connect[base_graph_row][dc] = {
		{1, 3, 5, 7, 9,  11},
		{2, 3, 6, 7, 10, 11},
		{1, 4, 6, 8, 10, 12},
		{2, 4, 5, 7, 9,  11},
		{2, 3, 6, 8, 9,  12},
		{1, 4, 5, 8, 10, 12}
	};

	int edge_label; edge_label=1; // to start labelling the edge from '1'
	for(unsigned int row_id=0; row_id<base_graph_row; row_id++) {
		for(unsigned col_id=0; col_id<dc; col_id++) {
			// A VNU to a CNU
			g.addEdge(baseGraph_rowWise_connect[row_id][col_id]-1, row_id+base_graph_col, edge_label);
			// to label the reversely directed edge (the same CNU to VNU) by negation of the same edge ID
			g.addEdge(row_id+base_graph_col, baseGraph_rowWise_connect[row_id][col_id]-1, -edge_label);
			edge_label += 1;
		}
	}
#endif // MACKAY_REGULAR_504_1008

#ifdef IEEE_802_16E_N2304_R50_Z96
	/*====================================================================================================*/
	// IEEE-802.16e (WiMax) standard code based on the [Zhang.2009]. 
	// The code structure is: (N=2304, R=0.5, Z=96) of G=V_a union V_b where |V_a|=12 and |V_b|=24.
	// [Zhang.2009] Zhang, Kai, Xinming Huang, and Zhongfeng Wang. "High-throughput layered decoder implementation for quasi-cyclic LDPC codes." IEEE Journal on Selected Areas in Communications 27.6 (2009): 985-994.
	unsigned int rowDegree_distribution[base_graph_row] = {6, 7, 7, 6, 6, 7, 6, 6, 7, 6, 6, 6};
	unsigned int nonzero_element[76] = {
		2, 3, 9, 10, 13, 14,
		2, 6, 7, 8, 12, 14, 15,
		4, 5, 6, 8, 12, 15, 16,
		1, 3, 9, 10, 16, 17,
		3, 7, 10, 11, 17, 18,
		5, 6, 8, 12, 13, 18, 19,
		3, 4, 10, 11, 19, 20,
		2, 3, 7, 10, 20, 21,
		1, 5, 6, 8, 12, 21, 22,
		6, 8, 11, 12, 22, 23,
		3, 4, 9, 10, 23, 24,
		1, 6, 8, 12, 13, 24
	};

	int edge_label; edge_label=1; // to start labelling the edge from '1'
	for(unsigned int row_id=0; row_id<base_graph_row; row_id++) {
		for(unsigned col_id=0; col_id<rowDegree_distribution[row_id]; col_id++) {
			// A VNU to a CNU
			g.addEdge(nonzero_element[edge_label-1]-1, row_id+base_graph_col, edge_label);
			// to label the reversely directed edge (the same CNU to VNU) by negation of the same edge ID
			g.addEdge(row_id+base_graph_col, nonzero_element[edge_label-1]-1, -edge_label);
			edge_label += 1;
		}
	}
#endif // IEEE_802_16E_N2304_R50_Z96

	/*====================================================================================================*/	
	QC_Graph qc(
		(_U32) upperGirth, //target_girth_t, 
		base_graph_col, //N_t, 
		base_graph_col-base_graph_row, //K_t, 
		base_graph_row, //M_t, 
	
	#ifdef MACKAY_REGULAR_504_1008
		base_graph_row*dc, // the number of nonzero element on the base graph
		84, //lift_degree_t, 
	#endif // MACKAY_REGULAR_504_1008

	#ifdef IEEE_802_16E_N2304_R50_Z96
		76, // the number of nonzero element on the base graph
		96, //lift_degree_t, 
	#endif // IEEE_802_16E_N2304_R50_Z96
		
		&g //*base_graph_t
	);
	/*---------------------------------------------------------------*/
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
		 cout << endl << "Finish building weightCoefficientMatrix for " << var_node << endl;
	}
	cout << "Finish building all weightCoefficientMatrices" << endl;
	for(_U32 len=1; len<qc.half_girth-1+1; len++) {
		qc.weightCoefficientMatrix[len-1].remove_redundancy_all();
		cout << "======================" <<endl;
		cout << endl << "A(" << (len+1)*2 << "), Number of voltage vectors:"
			 << qc.weightCoefficientMatrix[len-1].num << endl;

		qc.weightCoefficientMatrix[len-1].showMatrix(true);
	}

	cout << endl << endl;
	cout << endl;
	return 0;
}