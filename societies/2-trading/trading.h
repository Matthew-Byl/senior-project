#ifndef _TRADING_H
#define _TRADING_H

class Trading
{
public:
	Trading( 
		cl_uint *all_resources,
		SocietiesConfig &config,
		unsigned int random_seed
		);

private:
	cl_uint *myRandomPairs;
	std::minstd_rand0 myGenerator;
};

#endif
