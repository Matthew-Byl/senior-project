#ifndef _CONFIG_H
#define _CONFIG_H

/**
 * Structure that holds all the tunable configuration data.
 *
 * Anything that cannot change at runtime should be stored
 *  here so that it can be tuned in the configuration file.
 */

#include "../util/types.h"

typedef struct {
	/* The number of agents */
	CL_INT num_agents;

	/* The number of resources */
	CL_INT num_resources;

	/* The D and n values for each resource.
	   Because of GPU limitations, there can be at
	   most 256 resources, so these can be a known size.
	*/
	CL_UCHAR resource_D[256];
	CL_UCHAR resource_n[256];

	CL_INT max_experience;
	CL_INT min_effort;
	CL_INT max_effort;
	
} SocietiesConfig;

SocietiesConfig config_generate_default_configuration( void );
int config_verify_configuration( SocietiesConfig config );

#endif
