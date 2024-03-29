#include "trading.h"

#include <CLKernel.h>

#include <random>
#include <iostream>
#include <fstream>
using namespace std;

#define KERNEL_SOURCE "trading.cl"

/// @XXX: make sture that random_seed isn't the same as some other class' random seed.
Trading::Trading(
	cl_uint *all_resources,
	SocietiesConfig &config,
	unsigned int random_seed
	)
	:
	myRandomPairs( new cl_uint[config.num_agents] ),
	myConfig( config ),
	myGenerator( random_seed ),
	allResources( all_resources, config.num_agents * config.num_resources ),
	randomPairs( myRandomPairs, config.num_agents ),
	configBuffer( "SocietiesConfig", config )
{
	generate_random_pairs();

    ifstream t( KERNEL_SOURCE );
    string src((std::istreambuf_iterator<char>(t)),
               std::istreambuf_iterator<char>());

	string compiler_flags = config_generate_compiler_flags( config );
	trading_kernel = new CLKernel( "trading", src, compiler_flags );

	trading_kernel->setGlobalDimensions( config.num_threads, config.num_agents );
	trading_kernel->setLocalDimensions( config.num_threads, 1 );	

	for ( int i = 0; i < myConfig.num_agents; i++ )
	{
		cout << myRandomPairs[i] << " ";
	}
	cout << endl;
}

Trading::~Trading()
{
	delete[] myRandomPairs;
}

void Trading::trade()
{
	(*trading_kernel)(
		allResources,
		randomPairs,
		configBuffer
		);
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
		myRandomPairs[random_idx] = i;
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

	cout << config_generate_compiler_flags( config ) << endl;
}
