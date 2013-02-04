// The idea is that we keep a sorted list, which makes the
//  maximum operation trivial. The resource that is harvested
//  then just needs to re-find its position in the list, and the
//  others need to increment their positions by one.
//
// Except this is slow because we only need the first few largest ones.
//  Finding the max is faster than sorting.
//
// Sort_tree is n/2 items long, and has to be a power of 2.

void max_index_first_pass(
	__local float *values,
	__local uchar *sort_tree
	)
{
	size_t local_id = get_local_id( 0 );
	size_t local_size = get_local_size( 0 );

	if ( local_id < ( local_size / 2 ) )
	{
		// The index in @gains_per_minute this thread is examining.
		int real_idx = local_id * 2;

		// If there is an odd number of threads, don't read past
		//  the end of the array.
		if ( ( real_idx + 1 ) > local_size )
			sort_tree[local_id] = real_idx;
		else
		{
			// Put the index of the maximum of the two values
			//  in this thread's index in @sort_tree.
			if ( values[real_idx] > values[real_idx + 1] )
				sort_tree[local_id] = real_idx;
			else
				sort_tree[local_id] = real_idx + 1;
		}
	}

	// Before proceeding, make sure memory is consistent.
	barrier( CLK_LOCAL_MEM_FENCE );
}	

void max_index(
	__local float *values,
	__local uchar *sort_tree
	)
{
	size_t local_id = get_local_id( 0 );

	// Pass 1: put the indices of the maximum of every other element
	//  into the sort tree.
	
}
