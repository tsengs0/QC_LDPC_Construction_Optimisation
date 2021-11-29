#include <bits/stdc++.h>
#include "graph.h"

using namespace std;

void Graph::addEdge(int v, int w, int voltage)
{
	adj[v].push_back(w); // Add w to v’s list.
	edge_voltage[v].push_back(voltage); // add a wighe/voltage onto the corresponding edge
}

void Graph::DFS(int v)
{
	visited[v] = true;
	cout << v << " ";

	// Recur for all the vertices adjacent to vertex v
	list<int>::iterator i;
	
	for (i = adj[v].begin(); i != adj[v].end(); ++i) {
		if (!visited[*i])
			DFS(*i);	
	}
}

int Graph::getVoltage(int source_node, int sink_node)
{
    // to find the edge order with right terminating point
    unsigned int list_cnt; list<int>::iterator i; list_cnt = 0;
	int vol;
	for (i = adj[source_node].begin(); i != adj[source_node].end(); ++i) {
		if((*i) == sink_node) {
			vol = edge_voltage[source_node][list_cnt];
			break;
		}
		list_cnt += 1;
	}

    return vol;
}