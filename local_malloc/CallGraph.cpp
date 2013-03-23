// Mar. 22, 2013

#include "CallGraph.h"
using namespace std;

void CallGraph::enter_function( string name )
{
	CallGraphNode *node = new CallGraphNode;
	myNodes.push_back( node );
	myNodeMap.insert( make_pair<string,CallGraphNode*>( name, node ) );
	myCurrentNode = node;
}

void CallGraph::malloc( int size )
{
	myCurrentNode->edges.push_back(
		new CallGraphEdgeMalloc( size )
		);
}

void CallGraph::call( string name )
{
	myCurrentNode->edges.push_back(
		new CallGraphEdgeCall( name, *this )
		);
}

int CallGraph::maximum_alloc( string start_function )
{
	CallGraphNode *node = myNodeMap[start_function];


	// This isn't correct. Figure out the right algorithm
	//  in the morning.
	
	int max = 0;
	for ( int i = 0; i < node->edges.size(); i++ )
	{
		CallGraphEdge *edge = node->edges[i];
		int edge_usage = edge->call();

		if ( edge_usage > max )
			max = edge_usage;
	}

	return max;
}

