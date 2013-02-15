/**
 * Randomly choose one thread out of a set of 
 *  valid ones in the current workgroup.
 *
 * @author John Kloosterman
 * @date Feb. 3, 2013
 */

// Random number generator
#include <mwc64x.cl>

/*
 * How this works:
 *  (1) In scratch memory, create a counter of the number of valid options
 *      and a contiguous array of those valid options.
 *  (2) Thread 0 chooses a random integer between (0, # of valid options)
 *  (3) All the threads check to see if it was them. If so,
 *      they can do something.
 */

/*
 * Way to test: create histogram?
 */

/**
 * Preconditions: 
 *    *counter = 0
 *    scratch is of size at least sizeof( uint ) * 256
 *
 * Postconditions:
 *   there needs to be a local barrier before choose_thread_make_choice can be called.
 */
void choose_thread_add_to_options( 
	volatile __local int *counter, 
	__local uint *scratch
	)
{
	int idx = atomic_inc( counter );

	scratch[idx] = get_local_id( 0 );
}

/**
 * This should only be called by one thread.
 *   random_offset needs to be different per run and
 *     per workgroup.
 *
 * Preconditions:
 *   counter > 0
 *   rng_state has been initialized
 */
uint choose_thread_make_choice(
	volatile __local int *counter,
	__local uint *scratch,
	mwc64x_state_t *rng_state
	)
{
	if ( *counter == 0 )
	{
		// Error. It makes no sense to pick out of 
		//  0 options.
		printf( "choose_thread_make_choice: *counter == 0. Returning 0.\n" );
		return 0;
	}
	else if ( *counter == 1 )
	{
		return scratch[0];
	}
	else
	{
		// Choose a random integer between 0 and (counter - 1).
		uint random_integer = MWC64X_NextUint( rng_state );
		uint random_idx = random_integer % *counter;

		return scratch[random_idx];
	}
}
