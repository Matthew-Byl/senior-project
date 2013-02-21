/**
 * Functions to compute the "menu" of the least
 *  valuable resources that each of the 2 agents
 *  wants to trade.
 *
 * In this file, a thread is a resource, and does
 *  computation for both trading agents.
 */

#include "../config/config.h"
#include "../1-resource-extraction/utility.cl"
#include "../util/max_min.cl"

/**
 * From the paper: "the value of a resource is simply
 *  the MU of the last unit of each resource they hold.
 *
 * I change this, because the MU of a resource we have
 *  nothing of is undefined. I use the marginal utility of
 *  the next unit of the resource.
 *
 * This has the side effect of making it really valuable to
 *  have at least 1 of something. But that might be OK.
 *
 * @param agent_resources
 *  The array to read the amount of resources the agent owns.
 * @param valuations
 *  An array to put the valuation of each resource.
 * @param config
 *  The societies config object.
 */
void menu_resource_value(
	__local uint *agent_resources,
	__local float *valuations,
	__global SocietiesConfig *config )
{
	size_t local_id = get_local_id( 0 );

	valuations[local_id] = mu( 
		agent_resources[local_id] + 1,
		config->resource_D[local_id],
		config->resource_n[local_id]
	);
}

/** 
 * Create the menus of the @c CONFIG_MENU_SIZE
 *  least valuable resources two agents own.
 *
 * @param agent_a, agent_b
 *  The agents to compute the menus for.
 * @param all_resources
 *  The array of all agents' owned resources.
 * @param agent_a_menu, agent_b_menu
 *  Arrays of size CONFIG_MENU_SIZE to put
 *  the menus for the agents.
 * @param resources_scratch
 *  Local memory of at least size
 *    sizeof( uint ) * CONFIG_NUM_RESOURCES
 * @param sort_tree_scratch
 *  Local memory of at least size
 *    sizeof( uint ) * ( CONFIG_NUM_RESOURCES / 2 )
 * @param mask_scratch
 *  Local memory of at least size
 *    sizeof( uchar ) * CONFIG_NUM_RESOURCES
 * @param valuations_scratch
 *  Local memory of at least size
 *    sizeof( float ) * CONFIG_NUM_RESOURCES
 * @param config
 *  The societies configuration object.
 */
void menu_create_menus(
	uint agent_a,
	uint agent_b,
	__global uint *all_resources,
	__local uint *agent_a_menu,
	__local uint *agent_b_menu,
	__local uint *resources_scratch, // num_resources
	__local uint *sort_tree_scratch, // num_resources / 2
	__local uchar *mask_scratch,     // num_resources
	__local float *valuations_scratch, // num_resources
	__global SocietiesConfig *config
	)
{
	size_t local_id = get_local_id( 0 );
	size_t resource_offset;

	// Copy agent A's resources to local memory.
	resource_offset = ( agent_a * CONFIG_NUM_RESOURCES ) + local_id;
	resources_scratch[local_id] = all_resources[resource_offset];
	barrier( CLK_LOCAL_MEM_FENCE );
	
//	printf( "Agent A has %d of resource %d.\n", resources_scratch[local_id], local_id );

	// Find the value of each resource.
	menu_resource_value( resources_scratch, valuations_scratch, config );
	barrier( CLK_LOCAL_MEM_FENCE );
//	printf( "Resource %d has value %f.\n", local_id, valuations_scratch[local_id] );

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
	resource_offset = ( agent_b * CONFIG_NUM_RESOURCES ) + local_id;
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
