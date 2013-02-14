#ifndef _TRADING_H
#define _TRADING_H

#define _HOST_
#define _CPP_11_
#include <CLKernel.h>
#include <config.h>
#include <random>

class Trading
{
public:
	Trading( 
		cl_uint *all_resources,
		SocietiesConfig &config,
		unsigned int random_seed
		);

	~Trading();

	void trade();

private:
	void generate_random_pairs();

	cl_uint *myRandomPairs;
	SocietiesConfig myConfig;
	std::minstd_rand0 myGenerator;

	CLKernel *trading_kernel;
	CLUnitArgument allResources;
	CLUnitArgument randomPairs;
	CLUnitArgument configBuffer;
};

#endif
