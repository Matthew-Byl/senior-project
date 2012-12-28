#include "CLRenderer.h"
#include <iostream>
#include <fstream>
#include <string>

//#include <google/profiler.h>

using namespace std;

CLRenderer::CLRenderer( 
	string src, 
	unsigned char *pixel_buffer, 
	size_t width, 
	size_t height, 
	bool use_cpu )
	: myPixelBuffer( pixel_buffer ), 
	  myWidth( width ), 
	  myHeight( height ),
	  rayTrace( "raytrace", src, CLContext( 1, 0 ) ),
	  pixbuf_arg( "uchar", pixel_buffer, width * height * 4, false, true )
{
	rayTrace.setGlobalDimensions( myWidth, myHeight );
}

void CLRenderer::render( 
	Object *world, 
	int num_objects, 
	Light *lights, 
	int num_lights, 
	cl_float3 camera_position )
{
	size_t pixel_buffer_size = myWidth * myHeight * 4;

	CLUnitArgument world_arg( "Object", world, num_objects, true, false );
	CLUnitArgument lights_arg( "Light", lights, num_lights, true, false );

	rayTrace( 
		pixbuf_arg,
		world_arg,
		lights_arg,
		camera_position,
		num_objects,
		num_lights
		);
}
