/**
 * For every pair of resources, determine whether
 *  it would be a mutually beneficial trade.
 *
 * In this file, each thread is a possible pair of
 *  resources. Therefore, the menu size can't be larger 
 *  than 16, since 16^2 = 256.
 */

// returns TRUE if this thread will be involved, FALSE otherwise.
//  It depends on the size of the menu.
int valuation_resources( 
	int *resource_1,
	int *resource_2,
	__global SocietiesConfig *config )
{
	size_t local_id = get_local_id( 0 );
	int max_thread_needed = ( config->menu_size * config->menu_size ) - 1;

	if ( local_id > max_thread_needed )
		return FALSE;

	(*resource_1) = local_id / config->menu_size;
	(*resource_2) = local_id % config->menu_size;
}

float valuation_internal_valuation(
	uint agent,
	uint resource1,
	uint resource2,
	__global uint *all_resources,
	__global SocietiesConfig *config,
	)
{
	// Evaluate MU( outbound )
	size_t resource1_offset = ( agent_a * config->num_resources ) + resource1;
	uint resource1_amount = all_resources[resource1_offset];
	float resource1_valuation = mu(
		resource1_amount,
		config->D[resource1],
		config->n[resource1]
		);

	// Evaluate MU( inboud )
	size_t resource2_offset = ( agent_a * config->num_resources ) + resource2;
	uint resource2_amount = all_resources[resource2_offset];
	float resource2_valuation = mu(
		resource2_amount,
		config->D[resource2],
		config->n[resource2]
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
void valuation_highest_trade_valuation(
	uint agent_a,
	uint agent_b,
	__local uint *menu_a,
	__local uint *menu_b,
	__local uchar *beneficial_trade_mask,
	__global SocietiesConfig *config
	)
{
	
}
