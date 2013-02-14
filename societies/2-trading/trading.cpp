#include "trading.h"

#include <CLKernel.h>

#include <random>
#include <iostream>
using namespace std;

/// @XXX: make sture that random_seed isn't the same as some other class' random seed.
Trading::Trading(
	cl_uint *all_resources,
	SocietiesConfig &config,
	unsigned int random_seed
	)
	:
	myConfig( config ),
	myGenerator( random_seed )
{
	myRandomPairs = new cl_uint[config.num_agents];
	generate_random_pairs();

	for ( int i = 0; i < myConfig.num_agents; i++ )
	{
		cout << myRandomPairs[i] << " ";
	}
}

Trading::~Trading()
{
	delete[] myRandomPairs;
}

void Trading::generate_random_pairs()
{
	// Do an "inside-out" Knuth shuffle
	//  http://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle
	myRandomPairs[0] = 0;

	for ( int i = 1; i < myConfig.num_agents; i++ )
	{
		// 0 <= random_idx <= i
		int random_idx = myGenerator() % ( i + 1 );

		myRandomPairs[i] = myRandomPairs[random_idx];
		myRandomPairs[random_idx] = myRandomPairs[i];
	}
}

int main ( void )
{
	cl_uint resources;
	SocietiesConfig config = config_generate_default_configuration();

	Trading trading(
		&resources,
		config,
		42
		);
}
