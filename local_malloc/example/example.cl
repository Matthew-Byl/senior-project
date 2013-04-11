/**
 * A simple OpenCL kernel that uses local_malloc()
 *  to allocate workgroup scratch memory.
 *
 * @author John Kloosterman
 * @date April 10, 2013
 */

#include "local_malloc.h"

#define SIZEOF_INT 4
#define NUM_THREADS 256

int function( int i )
{
	return i * 2;
}

__kernel
void
function_sum(
	__global int *range_start,
	__global int *sum
	)
{
	size_t num_threads = get_local_size( 0 );
	size_t local_id = get_local_id( 0 );
	__local int minimum;

	// Initialize the minimum value.
	if ( local_id == 0 )
		minimum = 32768;
	barrier( CLK_LOCAL_MEM_FENCE );

	// Allocate scratch __local memory.
	__local int *values = local_malloc( NUM_THREADS * SIZEOF_INT );

	// Run the function at our thread's location.
	values[local_id] = function( *range_start + local_id );

	// Find the minimum using atomics.
	atomic_min( &minimum, values[local_id] )

	// Free scratch memory.
	local_free( NUM_THREADS * SIZEOF_INT, values );
}

/// @todo: find something less stupid to do.