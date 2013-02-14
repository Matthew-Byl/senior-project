/**
 * For every pair of resources, determine whether
 *  it would be a mutually beneficial trade.
 *
 * In this file, each thread is a possible pair of
 *  resources. Therefore, the menu size can't be larger 
 *  than 16, since 16^2 = 256.
 */

#include "../util/max_min.cl"

// returns TRUE if this thread will be involved, FALSE otherwise.
//  It depends on the size of the menu.
int valuation_resources( 
	uint *resource_1,
	uint *resource_2,
	__global SocietiesConfig *config )
{
	size_t local_id = get_local_id( 0 );
	int max_thread_needed = ( config->menu_size * config->menu_size ) - 1;

	if ( local_id > max_thread_needed )
		return FALSE;

	(*resource_1) = local_id / config->menu_size;
	(*resource_2) = local_id % config->menu_size;

	return TRUE;
}

float valuation_internal_valuation(
	uint agent,
	uint resource1,
	uint resource2,
	__global uint *all_resources,
	__global SocietiesConfig *config
	)
{
	// Evaluate MU( outbound )
	size_t resource1_offset = ( agent * config->num_resources ) + resource1;
	uint resource1_amount = all_resources[resource1_offset];
	float resource1_valuation = mu(
		resource1_amount,
		config->resource_D[resource1],
		config->resource_n[resource1]
		);

	// Evaluate MU( inboud )
	size_t resource2_offset = ( agent * config->num_resources ) + resource2;
	uint resource2_amount = all_resources[resource2_offset];
	float resource2_valuation = mu(
		resource2_amount,
		config->resource_D[resource2],
		config->resource_n[resource2]
		);

	return resource2_valuation / resource1_valuation;
}

// I don't know how to check. And even if a trade is stupid for
//  both sides, it might not be any slower for us to discover
//  that later.
int valuation_trade_beneficial(
	float agent_a_valuation,
	float agent_b_valuation
	)
{
	return TRUE;
}

// menu is an array of resource #s.
// menus need to already have been computed.
// The resource that a wants to trade will be put in resource_a
//  and resource that a wants to receive will be put in resource_b
void valuation_highest_trade_valuation(
	uint agent_a,
	__local uint *menu_a,
	__local uint *menu_b,
	__local uint *resource_a,
	__local uint *resource_b,
	__global uint *all_resources,
	__global SocietiesConfig *config
	)
{
	size_t local_id = get_local_id( 0 );
	uint thread_index_1, thread_resource_1;
	uint thread_index_2, thread_resource_2;
	__local float internal_valuations[CONFIG_MENU_SIZE * CONFIG_MENU_SIZE];

	// Threads not in a pair still have to participate in
	//  synchronization and min/max.
	int is_pair = valuation_resources( &thread_index_1, &thread_index_2, config );
	thread_resource_1 = menu_a[thread_index1];
	thread_resource_2 = menu_b[thread_index2];

	// This thread is involved in a pair.
	if ( is_pair )
	{
		internal_valuations[local_id] = valuation_internal_valuation(
			agent_a,
			thread_resource_1,
			thread_resource_2,
			all_resources,
			config
			);
	}
	barrier( CLK_LOCAL_MEM_FENCE );

	// Find the maximum.
	__local uchar sort_tree[(CONFIG_MENU_SIZE * CONFIG_MENU_SIZE) / 2];
	uint highest_valuation;
	if ( is_pair )
	{
		highest_valuation = max_index(
			internal_valuations,
			sort_tree
			);
	}

	if ( highest_valuation == local_id )
	{
		(*resource_a) = thread_resource_1;
		(*resource_b) = thread_resource_2;
	}
	barrier( CLK_LOCAL_MEM_FENCE );
}
