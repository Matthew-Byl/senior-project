/**
 * A simple OpenCL kernel that uses local_malloc()
 *  to allocate workgroup scratch memory.
 *
 * @author John Kloosterman
 * @date April 10, 2013
 */

#define SIZEOF_INT 4
#define NUM_THREADS 256

int function( int i )
{
	int ret;

	__local int *useless_ptr = local_malloc( SIZEOF_INT );
	*useless_ptr = i * 2;
	ret = *useless_ptr;
	local_free( SIZEOF_INT );

	return ret;
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
	__local int local_sum;

	// Initialize the sum.
	if ( local_id == 0 )
		local_sum = 0;
	barrier( CLK_LOCAL_MEM_FENCE );

	// Allocate scratch __local memory.
	__local int *values = local_malloc( NUM_THREADS * SIZEOF_INT );

	// Run the function at our thread's location.
	values[local_id] = function( *range_start + local_id );

	// Find the sum using atomics.
	atomic_add( &local_sum, values[local_id] );

	// Free scratch memory.
	local_free( NUM_THREADS * SIZEOF_INT );
	
	// Copy the result to global memory.
	if ( local_id == 0 )
		*sum = local_sum;
}
