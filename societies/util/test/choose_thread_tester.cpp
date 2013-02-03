/**
 * Tester for the functions that randomly choose
 *  a thread.
 *
 * @author John Kloosterman
 * @date Feb. 3, 2013
 */

#define _CPP_11_
#include <CLKernel.h>

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
using namespace std;

#define KERNEL_SOURCE "choose_thread_tester.cl"

int main()
{
	ifstream t( KERNEL_SOURCE );
    string src((std::istreambuf_iterator<char>(t)),
			   std::istreambuf_iterator<char>());

	CLKernel choose_thread_tester( "choose_thread_tester", src );
	choose_thread_tester.setGlobalDimensions( 256 );
	choose_thread_tester.setLocalDimensions( 256 );

	// Initialize some threads to enabled, others to disabled.
	cl_uchar host_enableds[256];
	for ( int i = 0; i < 256; i++ )
	{
		if ( i % 4 == 0 )
			host_enableds[i] = 1;
		else
			host_enableds[i] = 0;
	}
	CLUnitArgument enableds( host_enableds, 256 );

	cl_uchar host_chosen;
	CLUnitArgument chosen( &host_chosen, 1 );

	cl_ulong host_random_offset;
	srand( time( NULL ) );

	for ( int i = 0; i < 10000; i++ )
	{
		host_random_offset = rand();

		choose_thread_tester( enableds, chosen, host_random_offset );
		cout << (int) host_chosen << endl;
	}
}
