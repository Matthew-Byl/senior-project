#ifndef _CALL_GRAPH_H
#define _CALL_GRAPH_H

#include <string>
#include <vector>
#include <map>

class CallGraph;

/*********/
class CallGraphAction {
public:
	virtual int call() = 0;
};

/**********/
class CallGraphFunction {
public:
	std::vector<CallGraphAction *> actions;
};

/*************/
class CallGraph {
public:
	void enter_function( std::string name );
	void malloc( int size );
	void free( int size );
	void call( std::string name );

	bool isDefined( std::string name );
	int maximum_alloc( std::string start_function );

private:
	CallGraphFunction *myCurrentFunction;
	std::vector<CallGraphFunction *> myFunctions;
	std::map<std::string, CallGraphFunction *> myFunctionMap;
};

/**************/
class CallGraphActionMalloc : public CallGraphAction {
public:
	CallGraphActionMalloc( int size ) 
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

class CallGraphActionFree : public CallGraphAction {
public:
	CallGraphActionFree( int size ) 
	: mySize( size )
		{
		};

	virtual int call()
		{
			return -mySize;
		}
private:
	int mySize;
};

/**********/
class CallGraphActionCall : public CallGraphAction {
public:
	CallGraphActionCall( std::string name, CallGraph &callGraph )
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
