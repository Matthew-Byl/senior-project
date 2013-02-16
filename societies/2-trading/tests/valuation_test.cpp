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
	host_all_resources[20 + 9] = 40;

	cl_uint host_a_gives;
	cl_uint host_a_receives;

	CLUnitArgument all_resources( host_all_resources, 40 );
	CLUnitArgument a_gives( &host_a_gives, 1 );
	CLUnitArgument a_receives( &host_a_receives, 1 );
	CLUnitArgument config_buffer( "SocietiesConfig", config );
	
	valuation_tester(
		all_resources,
		a_gives,
		a_receives,
		config_buffer
		);

	cout << "Gives: " << host_a_gives << endl;
	cout << "Receives: " << host_a_receives << endl;
}
