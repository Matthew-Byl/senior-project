#ifndef _CL_KERNEL
#define _CL_KERNEL

#include "CLFunction.h"

class CLKernel : public CLFunction<void>
{
public:
	CLKernel( std::string function,
			  std::string kernel,
			  const CLContext context = CLContext() )
		: CLFunction( function, kernel, context ), 
		globalDimensionsSet( false ),
		globalOffsetSet( false ),
		localDimensionsSet( false )
		{ };
	virtual ~CLKernel() { };

	void setGlobalDimensions( int dim1 );
	void setGlobalDimensions( int dim1, int dim2 );
	void setGlobalDimensions( int dim1, int dim2, int dim3 );

	void setGlobalOffset( int dim1 );
	void setGlobalOffset( int dim1, int dim2 );
	void setGlobalOffset( int dim1, int dim2, int dim3 );

	void setLocalDimensions( int dim1 );
	void setLocalDimensions( int dim1, int dim2 );
	void setLocalDimensions( int dim1, int dim2, int dim3 );

	virtual void run();

private:
	bool globalDimensionsSet;
	bool globalOffsetSet;
	bool localDimensionsSet;
	std::vector<int> globalDimensions;
	std::vector<int> globalOffset;
	std::vector<int> localDimensions;
};

#endif
