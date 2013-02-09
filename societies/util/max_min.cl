/**
 * Efficient max and min functions for OpenCL, all running in lg(n)
 *  time with n threads.
 *
 * Conceptually, this works like making a heap. But since on the GPU
 *  it would take as long to reheapify as just build the heap again
 *  from scratch, we save over 1/2 the __local memory space by
 *  building a tree-like structure.
 *
 * @author John Kloosterman
 * @date Feb. 9, 2013
 */

typedef enum {
	MAX,
	MIN
} MaxMinType;

void max_min_first_pass(
	MaxMinType type,
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
			if ( type == MAX
				 && values[real_idx] > values[real_idx + 1] )
			{
				sort_tree[local_id] = real_idx;
			}
			else if ( type == MIN
					  && values[real_idx] < values[real_idx + 1] )
			{
				sort_tree[local_id] = real_idx;
			}
			else
			{
				sort_tree[local_id] = real_idx + 1;
			}
		}
	}

	// Before proceeding, make sure memory is consistent.
	barrier( CLK_LOCAL_MEM_FENCE );
}	

void max_min_second_pass(
	MaxMinType type,
	__local float *values,
	__local uchar *sort_tree
	)
{
	size_t local_id = get_local_id( 0 );
	size_t local_size = get_local_size( 0 );
	size_t sort_tree_size = ( local_size + 1 ) / 2; // The + 1 means we round up when dividing.

	// How far apart the two values that are being compared are.
	int stride = 1;
	int power_of_two = 2;

	// ( sort_tree_size * 2 ) forces one extra iteration,
	//  which is necessary when the sort tree is not a
	//  power-of-two size.
	while ( power_of_two <= ( sort_tree_size * 2 ) )
	{
		if ( ( local_id < sort_tree_size )
			 && ( ( local_id % power_of_two ) == 0 ) )
		{
			if ( ( local_id + stride ) >= sort_tree_size )
			{
				// If the comparison needs a value beyond the
				//  size of the sort tree, keep the current value.
			}
			else if ( type == MAX
					  && values[sort_tree[local_id + stride]] > values[sort_tree[local_id]] )
			{
				// Otherwise, make the sort tree at this location equal
				//  to the larger of the two values we are comparing.
				sort_tree[local_id] = sort_tree[local_id + stride];
			}
			else if ( type == MIN
					  && values[sort_tree[local_id + stride]] < values[sort_tree[local_id]] )
			{
				// Otherwise, make the sort tree at this location equal
				//  to the larger of the two values we are comparing.
				sort_tree[local_id] = sort_tree[local_id + stride];
			}
		}
		
		stride *= 2;
		power_of_two *= 2;
		
		barrier( CLK_LOCAL_MEM_FENCE );
	}
}


uchar max_min(
	MaxMinType type,
	__local float *values,
	__local uchar *sort_tree
	)
{
	// Pass 1: put the indices of the maximum of every other element
	//  into the sort tree.
	max_min_first_pass( type, values, sort_tree );
	max_min_second_pass( type, values, sort_tree );

	return sort_tree[0];
}

uchar max_index(
	__local float *values,
	__local uchar *sort_tree
	)
{
	return max_min( MAX, values, sort_tree );
}

uchar min_index(
	__local float *values,
	__local uchar *sort_tree
	)
{
	return max_min( MIN, values, sort_tree );
}
