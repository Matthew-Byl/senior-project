/**
 * For every pair of resources, determine whether
 *  it would be a mutually beneficial trade.
 *
 * In this file, each thread is a possible pair of
 *  resources. Therefore, the menu size can't be larger 
 *  than 16, since 16^2 = 256.
 * 
 * @author John Kloosterman
 * @date Feb. 2013
 */

#include "../util/max_min.cl"

// returns TRUE if this thread will be involved, FALSE otherwise.
//  It depends on the size of the menu.

/**
 * Determine which two resources this thread corresponds to.
 *
 * @param resource_1, resource_2
 *  Where to put the two resource numbers.
 * @param config
 *  The Societies configuration object.
 *
 * @return
 *  TRUE if the current thread corresponds to a
 *   pair to be considered as a potential trade,
 *   FALSE otherwise.
 */
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

/** 
 * Compute an agent's internal valuation of one
 *  resource against another.
 *
 * @param agent
 *  The index of the agent in question.
 * @param resource1, resource2
 *  The resources being considered.
 * @param all_resources
 *  The array of all agents' resources.
 * @param config
 *  The Societies configuration object.
 */
float valuation_internal_valuation(
	uint agent,
	uint resource1,
	uint resource2,
	__global uint *all_resources,
	__global SocietiesConfig *config
	)
{
	// Evaluate MU( outbound )
	int resource1_offset = ( agent * CONFIG_NUM_RESOURCES ) + resource1;
	uint resource1_amount = all_resources[resource1_offset];
	float resource1_valuation = mu(
		resource1_amount + 1,
		config->resource_D[resource1],
		config->resource_n[resource1]
		);

	// Evaluate MU( inboud )
	int resource2_offset = ( agent * CONFIG_NUM_RESOURCES ) + resource2;
	uint resource2_amount = all_resources[resource2_offset];
	float resource2_valuation = mu(
		resource2_amount + 1,
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

/**
 * Compute the @c CONFIG_NUM_TRADES best possible
 *  trades for @c agent_a made up of resources from
 *  @c menu_a and @c menu_b, and puts them in
 *  @c pairs.
 * 
 * @param agent_a
 *  The index of the agent in question.
 * @param menu_a, menu_b
 *  Agent A and B's menus.
 * @param pairs
 *  Where to put the CONFIG_NUM_TRADES top pairs.
 * @param internal_valuations_scratch
 *  Local memory, of size 
 *    sizeof( float ) * ( CONFIG_MENU_SIZE^2 )
 *  Note that CONFIG_MENU_SIZE^2 <= CONFIG_NUM_THREADS,
 *   because of restrictions checked by
 *   config_verify_configuration().
 * @param sort_tree
 *  Local memory, of size
 *    sizeof( uint ) * ( CONFIG_MENU_SIZE^2 / 2 )
 * @param indices_scratch
 *  Local memory, of size
 *    sizeof( uint ) * ( CONFIG_NUM_TRADES )
 * @param mask_scratch
 *  Local memory, of size
 *    sizeof( uchar ) * ( CONFIG_MENU_SIZE^2 )
 * @param config
 *  the Societies configuration object.
 */
void valuation_highest_trade_valuation_pairs(
	uint agent_a,
	__local uint *menu_a,
	__local uint *menu_b,
	__local uint2 *pairs,
	__global uint *all_resources,
	__local float *internal_valuations_scratch, // config_menu_size^2, which is <= num_threads
	__local uint *sort_tree,                    // config_menu_size^2 / 2
	__local uint *indices_scratch,              // config_num_trades
	__local uchar *mask_scratch,                // config_menu_size^2
	__global SocietiesConfig *config
	)
{
	size_t local_id = get_local_id( 0 );
	uint thread_index_1, thread_resource_1;
	uint thread_index_2, thread_resource_2;

	// Threads not in a pair still have to participate in
	//  synchronization and min/max.
	int is_pair = valuation_resources( &thread_index_1, &thread_index_2, config );
	thread_resource_1 = menu_a[thread_index_1];
	thread_resource_2 = menu_b[thread_index_2];

	printf( "Thread %d (is_pair %d): Resource %d and %d.\n", local_id, is_pair, thread_resource_1, thread_resource_2 );

	// This thread is involved in a pair.
	if ( is_pair )
	{
		internal_valuations_scratch[local_id] = valuation_internal_valuation(
			agent_a,
			thread_resource_1,
			thread_resource_2,
			all_resources,
			config
			);
	}
	barrier( CLK_LOCAL_MEM_FENCE );

	// Find the indices of the maximum num_trades pairs.
	n_max_indices(
		CONFIG_NUM_TRADES,
		internal_valuations_scratch,
		sort_tree,
		indices_scratch,
		mask_scratch
		);
	barrier( CLK_LOCAL_MEM_FENCE );

	// Turn those indices into pairs.
	for ( int i = 0; i < CONFIG_NUM_TRADES; i++ )
	{
		if ( local_id == indices_scratch[i] )
		{
			printf( "%d: Thread %d.\n", i, local_id );

			pairs[i].x = thread_resource_1;
			pairs[i].y = thread_resource_2;
		}
		barrier( CLK_LOCAL_MEM_FENCE );
	}
}
