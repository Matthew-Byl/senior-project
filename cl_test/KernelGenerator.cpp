#include "KernelGenerator.h"
#include <iostream>
using namespace std;


KernelGenerator::KernelGenerator( std::string function, std::vector<CLUnitArgument> &arguments )
	: myFunction(function), myArguments( arguments )
{

}


string KernelGenerator::generate()
{
	string ret = "__kernel run" + myFunction;
	ret += "( ";

	int i = 0;
	for ( auto it : myArguments )
	{
		// global *someType arg3,
		ret += "__global *" + it + " arg" + to_string( i ) + ", ";
		i++;
	}

	// take off last comma.
	ret.erase( ret.length() - 2, 1 );

	ret += ")";
	ret += "\n";
	ret += "{";
	ret += "\n";	

	ret += "\t" + myFunction + "( ";
	i = 0;
	for ( auto it : myArgumentTypes )
	{
		ret += "arg" + to_string( i ) + ", ";
		i++;
	}	

	// take off last comma.
	ret.erase( ret.length() - 2, 1 );

	ret += ")";
	ret += "\n";

	ret += "}";

	cout << ret << endl;
	return ret;
}

/*
int main ( void )
{
	KernelGenerator kg( "myFavouriteFunction" );
	kg.addArgument( "Sphere" );
	kg.addArgument( "SomeType" );
	kg.generate();
}
*/
