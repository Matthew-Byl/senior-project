/**
 */

#define _CPP_11_
#define _HOST_
#include <CLKernel.h>
#include <config.h>
#include <fstream>
using namespace std;

#define KERNEL_SOURCE "../trading.cl"

int main ( void )
{
	SocietiesConfig config = config_generate_default_configuration();
	config.num_threads = 20;
	config.num_agents = 2;
	config.num_resources = 20;
	config.menu_size = 5;
	config.num_trades = 1;

	string compiler_flags = config_generate_compiler_flags( config );

	// Open OpenCL kernel
	ifstream t( KERNEL_SOURCE );
	string src((std::istreambuf_iterator<char>(t)),
			   std::istreambuf_iterator<char>());
	CLKernel trading( "trading", src, compiler_flags );
	trading.setGlobalDimensions( config.num_threads, 1 );
	trading.setLocalDimensions( config.num_threads, 1 );

	// For each agent,
	// Out of 20 resources, create 4 ones we have too much
	//  of and would like to trade.
	cl_uint host_all_resources[40];
	for ( int i = 0; i < 40; i++ )
	{
		host_all_resources[i] = 8;
	}
	// Agent 0: resources 3, 8, 13, and 17 will be the least valuable.
	// Agent 1: resources 2, 4, 15, and 16 will be the least valuable
	host_all_resources[17] = 10;
	host_all_resources[20 + 15] = 10;

	cl_uint host_random_pairs[2] = { 0, 1 };

	CLUnitArgument all_resources( host_all_resources, 40 );
	CLUnitArgument random_pairs( host_random_pairs, 2 );
	CLUnitArgument config_buffer( "SocietiesConfig", config );
	
	trading(
		all_resources,
		random_pairs,
		config_buffer
		);

}
