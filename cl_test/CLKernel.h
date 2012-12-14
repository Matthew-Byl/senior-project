#ifndef _CL_KERNEL
#define _CL_KERNEL

#include "CLFunction.h"

class CLKernel : public CLFunction<void>
{
public:
	CLKernel( std::string function,
			  std::string kernel,
			  const CLContext context = CLContext() )
		: CLFunction( function, kernel, context ), dimensionsSet( false )
		{ };
	virtual ~CLKernel() { };

	void setDimensions( int dim1 );
	void setDimensions( int dim1, int dim2 );
	void setDimensions( int dim1, int dim2, int dim3 );

	virtual void run();

private:
	bool dimensionsSet;
	std::vector<int> dimensions;
};

#endif
