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

#define KERNEL_SOURCE "../effort.c"
#define OUTPUT_FILE "effort-curve.out"

int main ( void )
{
	const cl_int min_effort = 3;
	const cl_int max_effort = 0;
	const cl_int max_experience = 600;

	// Open output file.
	fstream out( OUTPUT_FILE, fstream::out );
	out << "Experience" << "\t" << "Effort" << endl;

	// Open OpenCL kernel
	ifstream t( KERNEL_SOURCE );
	string src((std::istreambuf_iterator<char>(t)),
			   std::istreambuf_iterator<char>());
	CLFunction<cl_float> resource_effort( "resource_effort", src );

	// Compute efforts.
	for ( cl_int experience = 0; experience <= max_experience; experience++ )
	{
		cl_int effort = 
			resource_effort( 
				experience,
				max_experience,
				min_effort,
				max_effort
				);

		out << experience << "\t" << effort << endl;
	}

	out.close();
}
