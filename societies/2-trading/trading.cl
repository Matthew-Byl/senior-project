/**
 * Glue code to do trading.
 *
 * @author John Kloosterman
 * @date Feb. 12, 2013
 */

#include "menu.cl"
#include "valuation.cl"

// Find the best multiple of ratio:1.
uint trading_find_maximum_surplus( 
	uint ratio,
	uint agent,
	uint resource1,
	uint resource2,
	__global uint *all_resources,
	__global SocietiesConfig *config )
{
	// NUM_RESOURCES is a proxy for number of threads.
	__local float surpluses[NUM_THREADS];
	__local float sort_tree[NUM_THREADS / 2];
	int offset = 0;
	size_t local_id = get_local_id( 0 );

	// Is it faster to get only 1 thread to do this? Do we have to
	//  use global memory?
	int resource_offset = agent * num_resources;
	uint resource1_amount = all_resources[resource_offset + resource1];
	uint resource2_amount = all_resources[resource_offset + resource2];

	int maximum_found = FALSE;
	uint max_multiplier;
	while ( !maximum_found )
	{
		// The multiple of the ratio for this thread
		float resource1_gain = ratio * ( local_id + offset );

		// The amount of utility the agent gains by getting more resource1.
		float resource1_gain_utility = 
			u( resource1_amount + resource1_gain, config->D[resource1], config->n[resource1] )
			- u( resource1_amount, config->D[resource1], config->n[resource1] );

		// The amount of utility the agent loses by giving up more resource2.
		float resource2_loss = 1 * ( local_id + offset );
		float resource2_loss_utility = 
			u( resource2_amount - resource2_loss, config->D[resource2], config->n[resource2] )
			- u( resource2_amount, config->D[resource2], config->n[resource2] );
		
		// If we can't afford this trade, don't consider it.
		float surplus;
		if ( resource2_loss > resource2_amount )
			surplus = -1;
		else
			surplus = resource1_gain_utility - resource2_loss_utility;

		surpluses[local_id] = surplus;
		barrier( CLK_LOCAL_MEM_FENCE );
		
		// Find the maximum.
		max_multiplier = max_index( surpluses, sort_tree );

		// If the maximum is the last thread, we don't know if it was the true maximum.
		//  In that case, do another round, with the last thread being reconsidered.
		if ( max_multiplier == ( NUM_THREADS - 1 ) )
		{
			offset += NUM_THREADS - 2;
		}
		else
		{
			maximum_found = TRUE;
		}
	}

	return max_multiplier + offset;
}

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

	__local uint menu_1[MENU_SIZE];
	__local uint menu_2[MENU_SIZE];

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

	/// @XXX: maybe after this point, to avoid confusion, rename the agents
	///  "first mover" and "respondent".
	int a_is_first_mover = TRUE;
	for ( int i = 0; i < ( config->num_trades * 2); i++ )
	{
		uint first_mover;
		uint respondent;

		if ( a_is_first_mover )
		{
			first_mover = agent_a;
			respondent = agent_b;
		}
		else
		{
			first_mover = agent_b;
			respondent = agent_a;
		}

		__local uint resource_1, resource_2;
		// The first mover gets to choose its favourite pair.
		valuation_highest_trade_valuation(
			first_mover,
			menu_1, menu_2,
			&resource_1, &resource_2,
			all_resources,
			config
			);

		// Calculate agent A and B's internal valuations
		//  to see if the trade is beneficial and what
		//  the bargaining price will be.
		__local float first_mover_valuation;
		__local float respondent_valuation;
		if ( local_id == 0 )
		{
			first_mover_valuation = valuation_internal_valuation(
				first_mover,
				resource_1, resource_2,
				all_resources,
				config );
		}
		else if ( local_id == 1 )
		{
			respondent_valuation = valuation_internal_valuation(
				respondent,
				resource_2, resource_1,
				all_resources,
				config );
		}
		barrier( CLK_LOCAL_MEM_FENCE );

		// Is the trade mutually beneficial? Is this the right way to compute that?
		if ( first_mover_valuation < 1
			 || respondent_valuation < 1 )
		{
			// do something
		}

		// Bargaining price is the geometric mean of
		//  Agent A and B's internal valuations.
		float f_bargaining_price = sqrt( first_mover_valuation * respondent_valuation );
		// Since resources are possessed in integers, this gets
		//  rounded to nearest integer.
		int bargaining_price = (int) f_bargaining_price;

		/// @XXX: we don't consider what B can afford when we find maximum
		/// surplus.

		// First mover optimizes its maximum surplus. We assign one thread to each
		//  multiplier of more surplus. 
		uint maximum_surplus_multiplier = trading_find_maximum_surplus( 
			bargaining_price,
			first_mover,
			resource_1,
			resource_2,
			all_resources,
			config );

		// Respondent accepts only if it benefits from the trade. The respondent benefits when
		//  the utility of the resources received is greater than the loss of
		//  utility of the resources given up.
		if ( trading_respondent_benefits(...) )
		{
			trading_make_trade(
				first_mover,
				respondent,
				resource_1,
				resource_2,
				bargaining_price * maximum_surplus_multiplier
				);
		}
		/// @XXX: The correct type of fence depends on what trading_make_trade does.
		barrier( CLK_GLOBAL_MEM_FENCE );

		a_is_first_mover = !a_is_first_mover;
	}	
}
