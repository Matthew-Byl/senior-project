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
	size_t sort_tree_size = ( local_size + 1 ) / 2; // The +1 means we round up.

	if ( local_id < sort_tree_size )
	{
		// The index in @gains_per_minute this thread is examining.
		int real_idx = local_id * 2;

		// If there is an odd number of threads, don't read past
		//  the end of the array.
		if ( ( real_idx + 1 ) >= local_size )
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

void max_index_second_pass(
	__local float *values,
	__local uchar *sort_tree
	)
{
	size_t local_id = get_local_id( 0 );
	size_t local_size = get_local_size( 0 );
	size_t sort_tree_size = ( local_size + 1 ) / 2; // The +1 means we round up.

	int stride = 1;
	int power_of_two = 2;

	// We want only the threads that put together the sort tree.
	while ( power_of_two <= sort_tree_size )
	{
		if ( ( local_id < sort_tree_size )
			 && ( ( local_id % power_of_two ) == 0 ) )
		{

			printf( "Thread %d!\n", local_id );

			if ( ( local_id + stride ) >= sort_tree_size )
			{
				// do nothing; keep the current value.
			}
			else if ( values[sort_tree[local_id + stride]] > values[sort_tree[local_id]] )
			{
				sort_tree[local_id] = sort_tree[local_id + stride];
			}
		}
		
		stride *= 2;
		power_of_two *= 2;
		
		barrier( CLK_LOCAL_MEM_FENCE );
	}
}


uchar max_index(
	__local float *values,
	__local uchar *sort_tree
	)
{
	// Pass 1: put the indices of the maximum of every other element
	//  into the sort tree.
	max_index_first_pass( values, sort_tree );

	max_index_second_pass( values, sort_tree );

	return sort_tree[0];
}
