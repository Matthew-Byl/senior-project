#ifndef _CALL_GRAPH_H
#define _CALL_GRAPH_H

#include <string>
#include <vector>
#include <map>

class CallGraph;

/*********/
class CallGraphEdge {
public:
	virtual int call() = 0;
};

/**********/
class CallGraphNode {
public:
	std::vector<CallGraphEdge *> edges;
};

/*************/
class CallGraph {
public:
	void enter_function( std::string name );
	void malloc( int size );
//	void free( int size );
	void call( std::string name );

	int maximum_alloc( std::string start_function );

private:
	CallGraphNode *myCurrentNode;
	std::vector<CallGraphNode *> myNodes;
	std::map<std::string, CallGraphNode *> myNodeMap;
};

/**************/
class CallGraphEdgeMalloc : public CallGraphEdge {
public:
	CallGraphEdgeMalloc( int size ) 
	: mySize( size )
		{
		};

	virtual int call()
		{
			return mySize;
		}
private:
	int mySize;
};

/**********/
class CallGraphEdgeCall : public CallGraphEdge {
public:
	CallGraphEdgeCall( std::string name, CallGraph &callGraph )
		: myName( name ),
		  myCallGraph( callGraph )
		{
		};

	virtual int call()
		{
			return myCallGraph.maximum_alloc( myName );
		}	

private:
	std::string myName;
	CallGraph &myCallGraph;
};


#endif
