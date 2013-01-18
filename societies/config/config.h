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
	   This needs to be an array of size
	   num_resources, but we don't know num_resources
	   at compile time. They also can't be pointers, because
	   they won't be part of the structure.
	*/
	CL_UCHAR resource_D;
	CL_UCHAR resource_n;
} SocietiesConfig;

SocietiesConfig config_generate_default_configuration( void );
int config_verify_configuration( SocietiesConfig config );

#endif
