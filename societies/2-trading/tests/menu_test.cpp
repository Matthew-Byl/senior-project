/**
 * Test program for resource effort curve.
 *
 * Creates a table of effort vs. experience, which
 *  can be charted to see if it matches the expected curve.
 */

#define _CPP_11_
#define _HOST_
#include <CLKernel.h>
#include <config.h>
#include <fstream>
using namespace std;

#define KERNEL_SOURCE "menu_test.cl"

int main ( void )
{
	cout << "Testing building the trading menus..." << endl << flush;

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
	CLKernel menu_tester( "menu_tester", src, compiler_flags );
	menu_tester.setGlobalDimensions( config.num_threads, 1 );
	menu_tester.setLocalDimensions( config.num_threads, 1 );

	// For each agent,
	// Out of 20 resources, create 4 ones we have too much
	//  of and would like to trade.
	cl_uint host_all_resources[40];
	for ( int i = 0; i < 40; i++ )
	{
		host_all_resources[i] = 0;
	}
	// Agent 0: resources 3, 8, 13, and 17 will be the least valuable.
	// Agent 1: resources 2, 4, 15, and 16 will be the least valuable
	host_all_resources[3] = 20;
	host_all_resources[8] = 3;
	host_all_resources[13] = 10;
	host_all_resources[17] = 7;
	host_all_resources[20 + 2] = 14;
	host_all_resources[20 + 4] = 6;
	host_all_resources[20 + 15] = 3;
	host_all_resources[20 + 16] = 100;

	// The fifth thing in the menu can be something essentially random.
	cl_uint host_menu_1[5];
	cl_uint host_menu_2[5];

	CLUnitArgument all_resources( host_all_resources, 40 );
	CLUnitArgument menu_1( host_menu_1, 5 );
	CLUnitArgument menu_2( host_menu_2, 5 );
	CLUnitArgument config_buffer( "SocietiesConfig", config );
	
	menu_tester(
		all_resources,
		menu_1,
		menu_2,
		config_buffer
		);

/*
	cout << "Menu 1: ";
	for ( int i = 0; i < 5; i++ )
		cout << host_menu_1[i] << " ";
	cout << endl;

	cout << "Menu 2: ";
	for ( int i = 0; i < 5; i++ )
		cout << host_menu_2[i] << " ";
	cout << endl;
*/

	assert( host_menu_1[0] == 3 );
	assert( host_menu_1[1] == 13 );
	assert( host_menu_1[2] == 17 );
	assert( host_menu_1[3] == 8 );
	assert( host_menu_2[0] == 16 );
	assert( host_menu_2[1] == 2 );
	assert( host_menu_2[2] == 4 );
	assert( host_menu_2[3] == 15 );

	cout << "All tests passed!" << endl << flush;
}
