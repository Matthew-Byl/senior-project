/**
 * Functions to create and validate Societies
 *  config structs.
 *
 * @author John Kloosterman
 * @date Feb. 7, 2013
 */

#include "config.h"

SocietiesConfig config_generate_default_configuration( void )
{
	SocietiesConfig config;

	config.num_agents = 5;
	config.num_resources = 128;
	config.num_days = 50;
	config.num_minutes = 6000;

	for ( int i = 0; i < 256; i++ )
	{
		config.resource_D[i] = 10;
		config.resource_n[i] = 10;
	}

	config.resource_epsilon = 1;
	config.max_experience = 200;
	config.min_effort = 3;
	config.max_effort = 9;

	return config;
}

bool config_verify_configuration( SocietiesConfig config )
{
	return true;
}
