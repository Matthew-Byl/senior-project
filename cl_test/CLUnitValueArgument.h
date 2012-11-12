#ifndef _CL_UNIT_VALUE_ARGUMENT_H
#define _CL_UNIT_VALUE_ARGUMENT_H

#include "CLUnitArgument.h"

template<class T>
class CLUnitValueArgument : public CLUnitArgument
{
public:
	CLUnitValueArgument( T value );
    virtual cl::Buffer getBuffer();
    virtual std::string getType();

private:
	T myValue;
};

template<class T>
CLUnitValueArgument<T>::CLUnitValueArgument( T value )
: myValue( value )
{

}

template<>
std::string CLUnitValueArgument<cl_int>::getType()
{
	return "int";
}

template<>
std::string CLUnitValueArgument<cl_float>::getType()
{
	return "float";
}

#endif
