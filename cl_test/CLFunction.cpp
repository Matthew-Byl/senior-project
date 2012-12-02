#include "CLFunction.h"
#include "KernelGenerator.h"
using namespace std;

CLFunction::CLFunction( std::string function, std::string kernel, CLContext &context )
	: myContext( context ), myFunction( function), myKernel( kernel )
{

}

void CLFunction::addArgument( CLUnitArgument &argument )
{
	myArguments.push_back( &argument );
}

void CLFunction::run()
{
	KernelGenerator generator( myFunction, myArguments );
	string src( myKernel + "\n\n" + generator.generate() );

	cout << "FULL SOURCE" << endl;
	cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
	cout << src << endl;
	cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;

	cl::Program program = myContext.buildProgram( src );
	cl::Kernel kernel(
		program,
		generator.getKernelFunction().c_str()
	);
}

