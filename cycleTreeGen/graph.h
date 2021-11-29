#ifndef __GRAPH_H
#define __GRAPH_H

#include <map>
#include <vector>
#include <list>

class Graph
{
	private:
		std::map<int, bool> visited;
		std::map<int, std::list<int>> adj;
		std::map<int, std::vector<int> > edge_voltage;

		friend class QC_Graph;

	public:
		// function to add an edge to graph
		void addEdge(int v, int w, int voltage);
	
		// DFS traversal of the vertices
		// reachable from v
		void DFS(int v);
		int getVoltage(int source_node, int sink_node);
};

#endif // __GRAPH_H