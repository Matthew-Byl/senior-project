/**
 * Functions to compute the "menu" of the least
 *  valuable resources that each of the 2 agents
 *  wants to trade.
 *
 * In this file, a thread is a resource, and does
 *  computation for both trading agents.
 */

#include "../1-resource-extraction/utility.cl"

/**
 * From the paper: "the value of a resource is simply
 *  the MU of the last unit of each resource they hold.
 */
float menu_resource_value(
	__local uint *agent_resources,
	__local float *valuations,
	__global SocietiesConfig *config )
{
	size_t local_id = get_local_id( 0 );

	valuations[local_id] = mu( 
		agent_resources[local_id],
		config->D[local_id],
		config->n[local_id]
	);
}

void menu_create_menus(
	uint agent_a,
	uint agent_b,
	__global uint *all_resources,
	__local uint *agent_a_menu,
	__local uint *agent_b_menu,
	__local uint *resources_scratch,  // Needs to be at least config->num_resources long.
	__local uchar *sort_tree_scratch, // 0.5 * config->num_resources
	__local uchar *mask_scratch,      // config->num_resources
	__local float *valuations_scratch, // config->num_resources
	__global SocietiesConfig *config
	)
{
	size_t local_id = get_local_id( 0 );
	size_t resource_offset;

	// Copy agent A's resources to local memory.
	resource_offset = ( agent_a * config->num_resources ) + local_id;
	resources_scratch[local_id] = all_resources[resource_offset];
	barrier( CLK_LOCAL_MEM_FENCE );
	
	// Find the value of each resource.
	menu_resource_value( resources_scratch, valuations_scratch, config );

	// Find the minimum config->menu_size ones.
	n_min_indices(
		config->menu_size,
		valuations_scratch,
		sort_tree_scratch,
		agent_a_menu,
		mask_scratch
		);

	// Do the same for agent B.
	// Copy agent B's resources to local memory.
	resource_offset = ( agent_b * config->num_resources ) + local_id;
	resources_scratch[local_id] = all_resources[resource_offset];
	barrier( CLK_LOCAL_MEM_FENCE );
	
	// Find the value of each resource.
	menu_resource_value( resources_scratch, valuations_scratch, config );

	// Find the minimum config->menu_size ones.
	n_min_indices(
		config->menu_size,
		valuations_scratch,
		sort_tree_scratch,
		agent_b_menu,
		mask_scratch
		);
}
