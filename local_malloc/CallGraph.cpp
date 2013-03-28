// Mar. 22, 2013

#include "CallGraph.h"
#include <iostream>
using namespace std;

void CallGraph::enter_function( string name )
{
	CallGraphFunction *function = new CallGraphFunction;
	myFunctions.push_back( function );
	myFunctionMap.insert( make_pair<string,CallGraphFunction*>( name, function ) );
	myCurrentFunction = function;
}

void CallGraph::malloc( int size )
{
	myCurrentFunction->actions.push_back(
		new CallGraphActionMalloc( size )
		);
}

void CallGraph::free( int size )
{
	myCurrentFunction->actions.push_back(
		new CallGraphActionFree( size )
		);
}

void CallGraph::call( string name )
{
	myCurrentFunction->actions.push_back(
		new CallGraphActionCall( name, *this )
		);
}

int CallGraph::maximum_alloc( string start_function )
{
	CallGraphFunction *function = myFunctionMap[start_function];

	if ( function == NULL )
	{
		cout << "Ignoring unknown function " << start_function << endl;
		return 0;
	}
	
	int current_allocation = 0;
	int peak = 0;

	for ( int i = 0; i < function->actions.size(); i++ )
	{
		CallGraphAction *action = function->actions[i];
		int action_usage = action->call();

		current_allocation += action_usage;
		if ( current_allocation > peak )
			peak = current_allocation;
	}

	return peak;
}

bool CallGraph::isDefined( string name )
{
	return ( myFunctionMap.find( name ) != myFunctionMap.end() );
}
