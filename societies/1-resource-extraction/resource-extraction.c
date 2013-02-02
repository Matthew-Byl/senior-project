/**
 * Simulate one day's worth of collecting resources.
 *
 * Approaches:
 *  -Loop for each minute
 *  -Loop that stores time left and what we got. (fewer iterations)
 */

// local ids: 1-dimensional: 1 per resource.
// Global ids: resources x agents

#if 0
void compute_gain_per_minute( 
	__local int *resources, 
	__local int *experiences,
	__local float *gains_per_minute,
	__constant SocietiesConfig *config
	)
{
	size_t local_id = get_local_id( 0 );

	float effort = resource_effort(
		experiences[local_id],
		config->max_experience,
		config->min_effort,
		config->max_effort
		);

	gains_per_minute[local_id] = gpm( 
		resources[local_id],
		config->resource_D[local_id],
		config->resource_n[local_id],
		effort
		);
}
#endif

// The idea is that we keep a sorted list, which makes the
//  maximum operation trivial. The resource that is harvested
//  then just needs to re-find its position in the list, and the
//  others need to increment their positions by one.
//
// Except this is slow because we only need the first few largest ones.
//  Finding the max is faster than sorting.
//
// Sort_tree is n/2 items long, and has to be a power of 2.

void max_gains_per_minute_first_pass(
	__local float *gains_per_minute,
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
			if ( gains_per_minute[real_idx] > gains_per_minute[real_idx + 1] )
				sort_tree[local_id] = real_idx;
			else
				sort_tree[local_id] = real_idx + 1;
		}
	}

	// Before proceeding, make sure memory is consistent.
	barrier( CLK_LOCAL_MEM_FENCE );
}	

void max_gains_per_minute(
	__local float *gains_per_minute,
	__local uchar *sort_tree
	)
{
	size_t local_id = get_local_id( 0 );

	// Pass 1: put the indices of the maximum of every other element
	//  into the sort tree.
	
}

#if 0
void check_is_within_epsilon(
	int max_gain_per_minute,
	__local float *gains_per_minute,
	__local uchar *within_epsilons,
	__constant SocietiesConfig *config
	)
{
	
}

// The __local arguments in the kernel allow us to dynamically
//  allocate their size.
__kernel void resource_extraction(
	__global int *all_resources,
	__global int *all_experiences,
	__local int *resources,
	__local int *experiences,
	__local float *gains_per_minute,
	__local uchar *within_epsilons,
	__local uchar *sort_tree,
	__constant SocietiesConfig *config
	)
{
	size_t local_id = get_local_id( 0 );
	size_t agent_offset = get_global_id( 1 ) * config->num_agents;

	// Copy our chunk to local memory.
	resources[local_id] = all_resources[agent_offset + local_id];
	experiences[local_id] = all_experiences[agent_offset + local_id];
	mem_fence( CLK_LOCAL_MEM_FENCE );

	

	// Copy our chunk back to global memory.
	all_resources[agent_offset + local_id] = resources[local_id];
	all_experiences[agent_offset + local_id] = experiences[local_id];
//	mem_fence( CLK_LOCAL_MEM_FENCE );
}

#endif

/********* Testing shims *********************/

__kernel void max_gain_per_minute_first_pass_tester(
	__global float *global_gains_per_minute,
	__global uchar *global_sort_tree
	)
{
	// There are at most 256 threads in the workgroup, so allocate
	//  memory for the max. It doesn't hurt if the actual size is less.
	__local float gains_per_minute[256];
	__local uchar sort_tree[128];

	size_t local_id = get_local_id( 0 );
	gains_per_minute[local_id] = global_gains_per_minute[local_id];
	barrier( CLK_LOCAL_MEM_FENCE );

	max_gains_per_minute_first_pass( gains_per_minute, sort_tree );
	barrier( CLK_LOCAL_MEM_FENCE );
	
	size_t local_size = get_local_size( 0 );
	if ( local_id < ( local_size / 2 ) )
		global_sort_tree[local_id] = sort_tree[local_id];
	barrier( CLK_LOCAL_MEM_FENCE );
}
