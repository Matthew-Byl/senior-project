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
		SocietiesConfig &config );
	~ResourceExtraction();

	void extractResources();

private:
	CLUnitArgument resources;
	CLUnitArgument experiences;
	CLKernel *resource_extraction;
	SocietiesConfig &myConfig;
};

#endif
