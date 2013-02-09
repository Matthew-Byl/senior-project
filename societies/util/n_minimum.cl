/**
 * Find the minimum n items in an array.
 *
 * @author John Kloosterman
 * @date Feb. 9, 2013
 */


/*
 Thought 1: use atomics.
  Even if we rad in a strange order, we know that no matter what, if we're
   larger than the values already there, we're not part of the n-minimum.
   If we are less, then we should atomically make sure we're less, and 
    swap ourselves in. (cmpxchng)
  This seems like worst-case performance might be really bad.
   We also have to do two operations atomically: change the index, and change the min.
   Or we could find the 5 min, and then after the fact determine what the indices are.
    -> this would work with atomic_min for integers, but not for floats.

 Thought 2: use some kind of merge sort-like algorithm. Make smaller lists of 5 mins,
   then merge them into a larger list of 5.

 Thought 3: make locks, and use Thought 1. Most of the time, the locks won't be used
   because the values are bigger than the 5 minimums.

 Thought 4: do 5 at a time, or have a shifting window or something: which is O(n) time.
*/

// Preconditions:
//  locks is the same size as mins, indices, locks; all threads participate.
//  locks = 0.
//
// There are no guarantees made about the order.
void n_minimum(
	int n,
	float value,
	__local float *mins,
	__local uchar *indices,
	volatile __local int *locks
	)
{
	size_t local_id = get_local_id( 0 );

	// Set mins to the largest value.
	if ( local_id < n )
		mins[local_id] = MAXFLOAT;
	mem_fence( CLK_LOCAL_MEM_FENCE );

	for ( int i = 0; i < n; i++ )
	{
		// Preliminary check: if we are greater than the value
		//  already there, we are guaranteed never to fill
		//  the spot.
		if ( value < mins[i] )
		{
			int been_placed = 0;

			// Acquire the lock. The lock = 0 when not held, so
			//  this will stop looping once someone else released it.
			while ( atomic_xchg( &locks[i], 1 ) );
			
			// Do the real check to see if we are less
			//  than the value already there. It could
			//  have changed under our noses.
			if ( value < mins[i] )
			{
				mins[i] = value;
				indices[i] = local_id;
				been_placed = 1;
			}

			// Release the lock.
			atomic_xchg( &locks[i], 0 );

			// Exit the loop if we put ourselves in one of the slots.
			if ( been_placed )
				break;
		}
	}
}
