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

	/* The number of days in the simulation */
	CL_INT num_days;

	/* The number of minutes in a day. */
	CL_INT num_minutes;

	/* The D and n values for each resource.
	   Because of GPU limitations, there can be at
	   most 256 resources, so these can be a known size.
	*/
	CL_UCHAR resource_D[256];
	CL_UCHAR resource_n[256];

	/* How close to the maximum gain per minute a
	   resource needs to be in order to be collected. */
	CL_FLOAT resource_epsilon;

	CL_INT max_experience;
	CL_INT min_effort;
	CL_INT max_effort;

	/* The experience penalty for not collecting a resources
	   on a given day. */
	CL_INT idle_penalty;
	
} SocietiesConfig;

#ifdef _HOST
SocietiesConfig config_generate_default_configuration( void );
bool config_verify_configuration( SocietiesConfig config );
#endif

#endif
