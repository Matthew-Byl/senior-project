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

#define KERNEL_SOURCE "../utility.cl"
#define OUTPUT_FILE "utility.out"

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
	CLFunction<cl_float> u( "u", src );

	// Compute efforts.
	for ( cl_int x = 0; x < 1000; x++ )
	{
		cl_float utility =
			u( x, D, n );

		out << x << "\t" << utility << endl;
	}

	out.close();
}
