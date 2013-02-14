#ifndef _TRADING_H
#define _TRADING_H

#define _HOST_
#include <CL/cl.hpp>
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

private:
	void generate_random_pairs();

	cl_uint *myRandomPairs;
	SocietiesConfig myConfig;
	std::minstd_rand0 myGenerator;
};

#endif
