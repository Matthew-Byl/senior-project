#ifndef _RESOURCE_EXTRACTION_H
#define _RESOURCE_EXTRACTION_H

#define _HOST_
#include <CLKernel.h>
#include <config.h>

class ResourceExtraction
{
public:
	ResourceExtraction( 
		cl_uint *all_resources,
		cl_uint *all_experiences,
		SocietiesConfig &config,
		unsigned int random_seed );
	~ResourceExtraction();

	void extractResources();

private:
	void generateRandomOffsets();

	cl_ulong *myRandomOffsets;
	CLUnitArgument resources;
	CLUnitArgument experiences;
	CLUnitArgument random_offsets;
	CLUnitArgument config;
	CLKernel *resource_extraction;
	SocietiesConfig &myConfig;
	unsigned int mySeed;
};

#endif
