#ifndef _MAX_MIN_CL
#define _MAX_MIN_CL

/**
 * Efficient max and min functions for OpenCL, all running in lg(n)
 *  time with n threads.
 *
 * Conceptually, this works like making a heap. But since on the GPU
 *  it would take as long to reheapify as just build the heap again
 *  from scratch, we save over 1/2 the __local memory space by
 *  building a tree-like structure that doesn't survive intact afterwards.
 *
 * @author John Kloosterman
 * @date Feb. 9, 2013
 */

#ifndef NULL
#define NULL ((void *) 0)
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

typedef enum {
	MAX,
	MIN
} MaxMinType;

/**
 * Create the first level of the sort tree by
 *  comparing adjacent items and putting the 
 *  index of the larger/smaller in the sort tree.
 *
 * @param type
 *  MAX or MIN
 * @param values
 *  The values the max/min is being taken of. It
 *   must have as many elements as threads calling
 *   this function.
 * @param sort_tree
 *  The location of scratch memory for the sort heap.
 *   It must have n / 2 elements, if n is the number
 *   of threads calling this function.
 * @param use_mask
 *  Whether or not to use @c value_mask to remove elements
 *   from consideration.
 * @param value_mask
 *  The ith element in @c values will not be considered as
 *   a candidate for max/min if value_mask[i] is TRUE.
 */
void max_min_first_pass(
	MaxMinType type,
	__local float *values,
	__local uint *sort_tree,
	int use_mask,
	__local uchar *value_mask
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
		// It's OK if this is a masked value, we'll catch it in the
		//  next phase.
		if ( ( real_idx + 1 ) >= local_size )
			sort_tree[local_id] = real_idx;
		else
		{
			// Masking: all that's important is that a masked value can't
			//  win over an unmasked one. So if one is masked, choose the other.
			if ( use_mask
				 && ( value_mask[real_idx] || value_mask[real_idx + 1] ) )
			{
				if ( value_mask[real_idx] )
					sort_tree[local_id] = real_idx + 1;
				else // value_mask[real_idx + 1]
					sort_tree[local_id] = real_idx;
			}
			else if ( ( type == MAX
						&& values[real_idx] > values[real_idx + 1] )
					  ||
					  ( type == MIN
						&& values[real_idx] < values[real_idx + 1] ) )
			{
				// Put the index of the maximum of the two values
				//  in this thread's index in @sort_tree.
				sort_tree[local_id] = real_idx;
			}
			else
			{
				// If MAX, this meant that values[real_idx + 1] was larger
				// If MIN
				sort_tree[local_id] = real_idx + 1;
			}
		}
	}

	// Before proceeding, make sure memory is consistent.
	barrier( CLK_LOCAL_MEM_FENCE );
}	

/**
 * Find the minimum value from the indices already placed
 *  in @c sort_tree.
 *
 * @param type
 *  MAX or MIN
 * @param values
 *  The values the max/min is being taken of. It
 *   must have as many elements as threads calling
 *   this function.
 * @param sort_tree
 *  The location of scratch memory for the sort heap.
 *   It must have n / 2 elements, if n is the number
 *   of threads calling this function.
 * @param use_mask
 *  Whether or not to use @c value_mask to remove elements
 *   from consideration.
 * @param value_mask
 *  The ith element in @c values will not be considered as
 *   a candidate for max/min if value_mask[i] is TRUE. It must
 *   have the same number of elements as @c values.
 */
void max_min_second_pass(
	MaxMinType type,
	__local float *values,
	__local uint *sort_tree,
	int use_mask,
	__local uchar *value_mask
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
			else if ( use_mask
					  && value_mask[sort_tree[local_id + stride]] )
			{
				// If the other value is masked, keep
				//  the current value.
			}
			else if ( use_mask
					  && value_mask[sort_tree[local_id]] )
			{
				// If this value is masked, take the other value.
				sort_tree[local_id] = sort_tree[local_id + stride];
			} 
			else if ( type == MAX
					  && values[sort_tree[local_id + stride]] > values[sort_tree[local_id]] )
			{
				// Make the sort tree at this location equal
				//  to the larger of the two values we are comparing.
				sort_tree[local_id] = sort_tree[local_id + stride];
			}
			else if ( type == MIN
					  && values[sort_tree[local_id + stride]] < values[sort_tree[local_id]] )
			{
				// Make the sort tree at this location equal
				//  to the smaller of the two values we are comparing.
				sort_tree[local_id] = sort_tree[local_id + stride];
			}
		}
		
		stride *= 2;
		power_of_two *= 2;
		
		barrier( CLK_LOCAL_MEM_FENCE );
	}
}

// Precondition: there is at least one value that is not masked.

/**
 * Find the index of the maximum/minimum value in
 *  @values.
 *
 * @param type
 *  MAX or MIN
 * @param values
 *  The values the max/min is being taken of. It
 *   must have as many elements as threads calling
 *   this function.
 * @param sort_tree
 *  The location of scratch memory for the sort heap.
 *   It must have n / 2 elements, if n is the number
 *   of threads calling this function.
 * @param use_mask
 *  Whether or not to use @c value_mask to remove elements
 *   from consideration.
 * @param value_mask
 *  The ith element in @c values will not be considered as
 *   a candidate for max/min if value_mask[i] is TRUE. It must
 *   have the same number of elements as @c values.
 */
uint max_min(
	MaxMinType type,
	__local float *values,
	__local uint *sort_tree,
	int use_mask,
	__local uchar *value_mask
	)
{
	max_min_first_pass( type, values, sort_tree, use_mask, value_mask );
	max_min_second_pass( type, values, sort_tree, use_mask, value_mask );

	return sort_tree[0];
}

/** 
 * Find the index of the maximum value in @c values.
 *
 * @param values
 *  The values the max is being taken of. It
 *   must have as many elements as threads calling
 *   this function.
 * @param sort_tree
 *  The location of scratch memory for the sort heap.
 *   It must have n / 2 elements, if n is the number
 *   of threads calling this function.
 * 
 * @return
 *  The index of the maximum value in @c values.
 */
uint max_index(
	__local float *values,
	__local uint *sort_tree
	)
{
	return max_min( MAX, values, sort_tree, 0, NULL );
}

/** 
 * Find the index of the minimum value in @c values.
 *
 * @param values
 *  The values the min is being taken of. It
 *   must have as many elements as threads calling
 *   this function.
 * @param sort_tree
 *  The location of scratch memory for the sort heap.
 *   It must have n / 2 elements, if n is the number
 *   of threads calling this function.
 *
 * @return
 *  The index of the maximum value in @c values.
 */
uint min_index(
	__local float *values,
	__local uint *sort_tree
	)
{
	return max_min( MIN, values, sort_tree, 0, NULL );
}

/**
 * Find the indices of the @c n maximum/minimum 
 *  values in @c values.
 *
 * @param n
 *  How many maximum/minimum indices to find.
 * @param type
 *  MAX or MIN
 * @param values
 *  The values the min is being taken of. It
 *   must have as many elements as threads calling
 *   this function.
 * @param sort_tree
 *  The location of scratch memory for the sort heap.
 *   It must have n / 2 elements, if n is the number
 *   of threads calling this function.
 * @param results
 *  Where to put the @c n indices. Must have @c n elements.
 * @param mask
 *  Scratch space to create a mask in. It must
 *   have the same number of elements as @c values.
 */
void n_max_min_indices(
	uint n,
	MaxMinType type,
	__local float *values,
	__local uint *sort_tree, // 1/2 the size of values
	__local uint *results,   // size n
	__local uchar *mask       // the size of values
	)
{
	size_t local_id = get_local_id( 0 );

	mask[local_id] = FALSE;
	barrier( CLK_LOCAL_MEM_FENCE );

	for ( int i = 0; i < n; i++ )
	{
		uchar val = max_min(
			type,
			values,
			sort_tree,
			TRUE,
			mask
			);

		if ( local_id == 0 )
		{
			results[i] = val;
		}
		if ( local_id == val )
		{
			// Don't reuse this value.
			mask[local_id] = TRUE;
		}
		barrier( CLK_LOCAL_MEM_FENCE );
	}
}

/**
 * Find the indices of the @c n maximum
 *  values in @c values.
 *
 * @param n
 *  How many maximum indices to find.
 * @param values
 *  The values the min is being taken of. It
 *   must have as many elements as threads calling
 *   this function.
 * @param sort_tree
 *  The location of scratch memory for the sort heap.
 *   It must have n / 2 elements, if n is the number
 *   of threads calling this function.
 * @param results
 *  Where to put the @c n indices. Must have @c n elements.
 * @param mask
 *  Scratch space to create a mask in. It must
 *   have the same number of elements as @c values.
 */
void n_max_indices(
	uchar n,
	__local float *values,
	__local uint *sort_tree, // 1/2 the size of values
	__local uint *results,   // size n
	__local uchar *mask       // the size of values
	)
{
	return n_max_min_indices( 
		n,
		MAX,
		values,
		sort_tree,
		results,
		mask
		);
}

/**
 * Find the indices of the @c n minimum
 *  values in @c values.
 *
 * @param n
 *  How many minimum indices to find.
 * @param values
 *  The values the min is being taken of. It
 *   must have as many elements as threads calling
 *   this function.
 * @param sort_tree
 *  The location of scratch memory for the sort heap.
 *   It must have n / 2 elements, if n is the number
 *   of threads calling this function.
 * @param results
 *  Where to put the @c n indices. Must have @c n elements.
 * @param mask
 *  Scratch space to create a mask in. It must
 *   have the same number of elements as @c values.
 */
void n_min_indices(
	uchar n,
	__local float *values,
	__local uint *sort_tree, // 1/2 the size of values
	__local uint *results,   // size n
	__local uchar *mask       // the size of values
	)
{
	return n_max_min_indices( 
		n,
		MIN,
		values,
		sort_tree,
		results,
		mask
		);
}


#endif
