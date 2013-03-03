/**
 * Stage 3: device invention
 *
 * @author John Kloosterman
 * @date Mar. 2, 2013
 */

#include "../util/max_min.cl"

/**
 * Pick the six resources for device
 *  invention: the two for which the agent
 *  has the most experience, and 4 chosen
 *  at random.
 *
 * @param resources
 *  Where to put the 6 resources chosen.
 * @param experiences
 *  This agent's experiences collecting each
 *  resource.
 */
void invention_pick_resources(
	__local uint *resources,
	__local uint *experiences,
	__local float *float_scratch, // size num_resources
	__local uint *sort_tree_scratch,
	__local uint *mask_scratch,
    mwc64x_state_t *rng_state
	)
{
	size_t local_id = get_local_id( 0 );

	// Find the indices of the two highest experiences.
	// Irritatingly, our util functions require floats,
	//  so we have to convert all the experiences to
	//  floats first.
	float_scratch[local_id] = (float) experiences[local_id];
	barrier( CLK_LOCAL_MEM_FENCE );

	n_max_indices( 
		2,
		float_scratch,
		resources,
		mask_scratch
		);
	// Now resources[0,1] have the two resources with the
	//  most experience.

	if ( local_id == 0 )
	{
		for ( int i = 2; i < 6; i++ )
		{
			uint random_integer = MWC64X_NextUint( rng_state );
			resources[i] = random_integer % CONFIG_NUM_RESOURCES;
		}
	}
	barrier( CLK_LOCAL_MEM_FENCE );
}
