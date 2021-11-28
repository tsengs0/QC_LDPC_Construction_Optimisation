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
	//if(edge_voltage[v].size() != 0) cout << " (" << edge_voltage[v][edge_cnt] << ") ";
	
	// Recur for all the vertices adjacent to vertex v
	list<int>::iterator i;
	
	for (i = adj[v].begin(); i != adj[v].end(); ++i) {
		if (!visited[*i])
			DFS(*i);	
	}
}