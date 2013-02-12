/**
 * Glue code to do trading.
 *
 * @author John Kloosterman
 * @date Feb. 12, 2013
 */

#include "menu.cl"
#include "valuation.cl"

__kernel void trading(
	__global uint *all_resources,
	__global uint *random_pairs,
	__global SocietiesConfig *config,
	)
{
	size pair_id = get_global_id( 1 );
	int pair_offset = 2 * pair_id;
	uint agent_a = random_pairs[pair_offset];
	uint agent_b = random_pairs[pair_offset + 1];

	__local uint menu_a[MENU_SIZE];
	__local uint menu_b[MENU_SIZE];

	// Generate the menus.
	menu_create_menus(
		agent_a,
		agent_b,
		all_resources,
		menu_a,
		menu_b,
		config
		);
	barrier( CLK_LOCAL_MEM_FENCE );

	__local uint resource_a, resource_b;
	// Agent A is the first mover and gets to pick its
	//  favourite trade.
	valuation_highest_trade_valuation(
		agent_a,
		menu_a, menu_b,
		&resource_a, &resource_b,
		all_resources,
		config
		);

	// Calculate agent A and B's internal valuations
	//  to see if the trade is beneficial and what
	//  the bargaining price will be.
	__local float agent_a_valuation;
	__local float agent_b_valuation;
	if ( local_id == 0 )
	{
		agent_a_valuation = valuation_internal_valuation(
			agent_a,
			resource_a, resource_b,
			all_resources,
			config );
	}
	else if ( local_id == 1 )
	{
		agent_b_valuation = valuation_internal_valuation(
			agent_b,
			resource_b, resource_a,
			all_resources,
			config );
	}
	barrier( CLK_LOCAL_MEM_FENCE );

	// Is the trade mutually beneficial? Is this the right way to compute that?
	/*
	if ( agent_a_valuation < 1
		 || agent_b_valuation < 1 )
	{
		// do something
	}
	*/

	// Bargaining price is the geometric mean of
	//  Agent A and B's internal valuations.
	float f_bargaining_price = sqrt( agent_a_valuation * agent_b_valuation );
	// Since resources are possessed in integers, this gets
	//  rounded to nearest integer.
	int bargaining_price = (int) f_bargaining_price;

	// A optimizes its maximum surplus. We assign one thread to each
	//  multiplier of more surplus. We want to find the first one
	//  whose first difference is negative.
	
}
