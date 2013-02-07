/**
 * Host code to drive the resource extraction
 *  stage of Societies.
 *
 * @date Feb. 7, 2013
 * @author John Kloosterman
 */

#define _HOST_
#define _CPP_11_
#include <CL/cl.hpp>
#include <CLKernel.h>
#include <config.h>
#include <fstream>
#include <random>
#include "resource-extraction.h"
using namespace std;

#define KERNEL_SOURCE "resource-extraction.cl"

/**
 * Perform one day of resource extraction.
 */
ResourceExtraction::ResourceExtraction(
	cl_uint *all_resources,
	cl_uint *all_experiences,
	SocietiesConfig &config,
	unsigned int random_seed
	)
	: 
	myRandomOffsets( new cl_ulong[config.num_agents] ),
	resources( all_resources, config.num_agents * config.num_resources ),
	experiences( all_experiences, config.num_agents * config.num_resources ),
	random_offsets( myRandomOffsets, config.num_agents, false, false ),
	config( "SocietiesConfig", &myConfig, 1, false, false ),
	myConfig( config ),
	mySeed( random_seed )
{
	ifstream t( KERNEL_SOURCE );
    string src((std::istreambuf_iterator<char>(t)),
               std::istreambuf_iterator<char>());

	resource_extraction = new CLKernel( "resource_extraction", src );
	resource_extraction->setGlobalDimensions( config.num_resources, config.num_agents );
	resource_extraction->setLocalDimensions( config.num_resources, 1 );

	resource_extraction->setLocalArgument( 4, sizeof( cl_uint ) * config.num_resources );
	resource_extraction->setLocalArgument( 5, sizeof( cl_uint ) * config.num_resources );
	resource_extraction->setLocalArgument( 6, sizeof( cl_float ) * config.num_resources );
	resource_extraction->setLocalArgument( 7, sizeof( cl_uchar ) * config.num_resources );
}

ResourceExtraction::~ResourceExtraction()
{
	delete myRandomOffsets;
	delete resource_extraction;
}

void ResourceExtraction::generateRandomOffsets()
{
	std::minstd_rand0 generator( mySeed );

	for ( int i = 0; i < myConfig.num_agents; i++ )
	{
		myRandomOffsets[i] = generator();
	}
}

void ResourceExtraction::extractResources()
{
	generateRandomOffsets();
	(*resource_extraction)( 
		resources, 
		experiences,
		random_offsets,
		config
		);
}

int main ( void )
{
	SocietiesConfig config;
	config = config_generate_default_configuration();

	cl_uint all_resources[1000*256];
	cl_uint all_experiences[1000*256];

	ResourceExtraction re( 
		all_resources, 
		all_experiences,
		config,
		42 );
	re.extractResources();
}
