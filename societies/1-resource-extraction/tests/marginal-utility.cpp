/**
 * Test program for resource effort curve.
 *
 * Creates a table of effort vs. experience, which
 *  can be charted to see if it matches the expected curve.
 */

#define _CPP_11_
#include <CLFunction.h>
#include <fstream>
using namespace std;

#define KERNEL_SOURCE "../utility.c"
#define OUTPUT_FILE "marginal-utility.out"

int main ( void )
{
	const cl_int D = 3;
	const cl_int n = 4;

	// Open output file.
	fstream out( OUTPUT_FILE, fstream::out );

	// Open OpenCL kernel
	ifstream t( KERNEL_SOURCE );
	string src((std::istreambuf_iterator<char>(t)),
			   std::istreambuf_iterator<char>());
	CLFunction<cl_float> mu( "mu", src );

	// Compute efforts.
	for ( cl_int x = 0; x < 100; x++ )
	{
		cl_float marginal_utility =
			mu( x, D, n );

		out << x << "\t" << marginal_utility << endl;
	}

	out.close();
}
