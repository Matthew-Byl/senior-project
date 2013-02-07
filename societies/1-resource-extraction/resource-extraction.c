/**
 * Simulate one day's worth of collecting resources.
 *
 * Approaches:
 *  -Loop that stores time left and what we got. (fewer iterations)
 */

#include <config.h>
#include <mwc64x.cl>
#include <choose_thread.c>
#include <max_index.c>
#include "effort.c"
#include "resource_extraction.c"

// local ids: 1-dimensional: 1 per resource.
// Global ids: resources x agents

/**
 * Compute the gain per minute of collecting this resource.
 */
void compute_gain_per_minute( 
	__local uint *resources, 
	__local uint *experiences,
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

/**
 * Determine whether this resource is within an epsilon
 *  of the resource with the best gain per minute.
 */
int is_within_epsilon(
	uchar max_gpm_index,
	__local float *gains_per_minute,
	__constant SocietiesConfig *config
	)
{
	size_t local_id = get_local_id( 0 );

	float difference = 
		gains_per_minute[max_gpm_index] 
		- gains_per_minute[local_id];

	if ( fabs( difference ) <= config->resource_epsilon )
		return TRUE;
	else
		return FALSE;
}

/**
 * If the agent does not collect a given resource in a day,
 *  decrease its experience with that resource by
 *  config->idle_penalty.
 */
void take_experience_penalty(
	__global uint *all_resources,
	__local uint *resources,
	__local uint *experiences,
	__constant SocietiesConfig *config
	)
{
    size_t local_id = get_local_id( 0 );

	// If we did not collect this resource today, take the
	//  penalty for being idle.
	if ( all_resources[local_id] == resources[local_id] )
	{
		if ( experiences[local_id] < config->idle_penalty )
			experiences[local_id] = 0;
		else
			experiences[local_id] -= config->idle_penalty;
	}
}

/**
 * Clamp the amount of experience the agent has
 *  with a resource to the maximum in config->max_experience.
 */
void clamp_experience(
	__local uint *experiences,
	__constant SocietiesConfig *config
	)
{
	size_t local_id = get_local_id( 0 );

	if ( experiences[local_id] > config->max_experience )
		experiences[local_id] = config->max_experience;
}

// The __local arguments in the kernel allow us to dynamically
//  allocate their size.
__kernel void resource_extraction(
	__global uint *all_resources,
	__global uint *all_experiences,
	__global ulong *rng_base_offsets,
	__local uint *resources,
	__local uint *experiences,
	__local float *gains_per_minute,
	__local uchar *scratch,  // 255 * sizeof( uchar )
	__constant SocietiesConfig *config
	)
{
	__local int counter;
	__local uchar chosen_resource;

	size_t local_id = get_local_id( 0 );
	size_t agent_offset = get_global_id( 1 ) * config->num_agents;

	// Initialize the RNG.
	mwc64x_state_t rng_state;
	// The documentation ( http://cas.ee.ic.ac.uk/people/dt10/research/rngs-gpu-mwc64x.html )
	//  says that 2^40 is a good perStreamOffset if we don't know one.
	// 2^40 = 1099511627776
    MWC64X_SeedStreams(
		&rng, 
		rng_base_offsets[get_global_id( 1 )], 
		1099511627776 
	);

	// Copy our chunk to local memory.
	resources[local_id] = all_resources[agent_offset + local_id];
	experiences[local_id] = all_experiences[agent_offset + local_id];
	mem_fence( CLK_LOCAL_MEM_FENCE );

	// Compute gain per minute for all resources.
	compute_gain_per_minute( resources, experiences, gains_per_minute, config );

	int minutes_used = 0;
	while ( minutes_used < config->num_minutes )
	{
		// Find the resource with maximum gain per minute.
		uchar max_gpm_index = max_index( gains_per_minute, scratch );

		// Set the random selection counter to 0.
		if ( local_id == 0 )
			counter = 0;
		barrier( CLK_LOCAL_MEM_FENCE );

		// If this thread's resources is within an epsilon of 
		//  that maximum, add it to the list of threads that can
		//  be randomly chosen from.
		if ( is_within_epsilon( max_gpm_index, gains_per_minute, config ) )
		{
			choose_thread_add_to_options( counter, scratch );
		}
		barrier( CLK_LOCAL_MEM_FENCE );

		// Choose a resource.
		if ( local_id == 0 )
		{
			chosen_resource = choose_thread_make_choice( counter, scratch, rng_state );
		}
		barrier( CLK_LOCAL_MEM_FENCE );

		// Collect one of that resource and gain experience with it.
		if ( local_id == chosen_resource )
		{
			minutes_used += resource_effort(
				experiences[local_id],
				config->max_experience,
				config->min_effort,
				config->max_effort
				);

			resources[local_id]++;
			experiences[local_id]++;
		}
		barrier( CLK_LOCAL_MEM_FENCE );
	}

	// Take an experience penalty if a resource was never collected.
	take_experience_penalty( all_resources, resources, experiences, config );

	// Ensure experience is below the maximum allowed.
	clamp_experiences( experiences, config );
	
	// Copy data back to global memory.
	all_resources[agent_offset + local_id] = resources[local_id];
	all_experiences[agent_offset + local_id] = experiences[local_id];
}
