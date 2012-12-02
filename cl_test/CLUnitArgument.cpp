#include "CLUnitArgument.h"
#include <iostream>
using namespace std;

cl::Buffer &CLUnitArgument::getBuffer()
{
	cout << "Abstract called." << endl << flush;
	abort();
	return * new cl::Buffer();
}

std::string CLUnitArgument::getType()
{
	cout << "Abstract called." << endl << flush;
	abort();
	return "Override me!";
}
