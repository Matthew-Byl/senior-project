/**
 * Test program for valuation
 */

#define _CPP_11_
#define _HOST_
#include <CLKernel.h>
#include <config.h>
#include <fstream>
using namespace std;

#define KERNEL_SOURCE "valuation_test.cl"

int main ( void )
{
	cout << "Testing trade valuations..." << endl << flush;

	SocietiesConfig config = config_generate_default_configuration();
	config.num_threads = 20;
	config.num_agents = 2;
	config.num_resources = 20;
	config.menu_size = 5;
	config.num_trades = 5;

	string compiler_flags = config_generate_compiler_flags( config );

	// Open OpenCL kernel
	ifstream t( KERNEL_SOURCE );
	string src((std::istreambuf_iterator<char>(t)),
			   std::istreambuf_iterator<char>());
	CLKernel valuation_tester( "valuation_test", src, compiler_flags );
	valuation_tester.setGlobalDimensions( config.num_threads, 1 );
	valuation_tester.setLocalDimensions( config.num_threads, 1 );

	// For each agent,
	// Out of 20 resources, create 4 ones we have too much
	//  of and would like to trade.
	cl_uint host_all_resources[40];
	for ( int i = 0; i < 40; i++ )
	{
		host_all_resources[i] = 40 - i;
	}
	// Agent 0: resources 3, 8, 13, and 17 will be the least valuable.
	// Agent 1: resources 2, 4, 15, and 16 will be the least valuable
	host_all_resources[4] = 50;
 	host_all_resources[5] = 60;
 	host_all_resources[9] = 30;
	host_all_resources[20 + 9] = 40;

	cl_uint2 host_pairs[5];

	CLUnitArgument all_resources( host_all_resources, 40 );
	CLUnitArgument pairs( host_pairs, 5 );
	CLUnitArgument config_buffer( "SocietiesConfig", config );
	
	valuation_tester(
		all_resources,
		pairs,
		config_buffer
		);

	for ( int i = 0; i < 5; i++ )
	{
		cout << "Pair " << i << " resources: " << host_pairs[i].s[0] << " " << host_pairs[i].s[1] << endl;
	}
}
