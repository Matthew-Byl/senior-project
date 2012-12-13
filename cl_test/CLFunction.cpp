#include "CLFunction.h"
#include "KernelGenerator.h"
using namespace std;

CLFunction::CLFunction( std::string function, std::string kernel, CLContext &context )
	: myContext( context ), myFunction( function), myKernel( kernel )
{

}

void CLFunction::addArgument( CLUnitArgument argument )
{
	myArguments.push_back( argument );
}

void CLFunction::setArguments( const std::vector<CLUnitArgument> arguments )
{
	myArguments = arguments;
	cout << "Num arguments: " << arguments.size() << endl;
}
