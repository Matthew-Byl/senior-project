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

