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
#include "resource-extraction.h"
using namespace std;

#define KERNEL_SOURCE "resource-extraction.cl"

/**
 * Perform one day of resource extraction.
 */
ResourceExtraction::ResourceExtraction(
	cl_uint *all_resources,
	cl_uint *all_experiences,
	SocietiesConfig &config
	)
	: 
	resources( all_resources, config.num_agents * config.num_resources ),
	experiences( all_experiences, config.num_agents * config.num_resources ),
	myConfig( config )
{
	ifstream t( KERNEL_SOURCE );
    string src((std::istreambuf_iterator<char>(t)),
               std::istreambuf_iterator<char>());

	resource_extraction = new CLKernel( "resource_extraction", src );
	resource_extraction->setGlobalDimensions( config.num_resources, config.num_agents );
	resource_extraction->setLocalDimensions( config.num_resources, 1 );
}

ResourceExtraction::~ResourceExtraction()
{
	delete resource_extraction;
}

void ResourceExtraction::extractResources()
{
	(*resource_extraction)( resources, experiences );
}

int main ( void )
{
	SocietiesConfig config;
	config = config_generate_default_configuration();

	cl_uint all_resources[1000*256];
	cl_uint all_experiences[1000*256];

	ResourceExtraction re( all_resources, all_experiences, config );
	re.extractResources();
}
