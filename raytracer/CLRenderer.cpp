#include "CLRenderer.h"
#include <iostream>
#include <fstream>
#include <string>

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
	  rayTrace( "raytrace", src ),
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

	CLArgument world_arg( "Object", world, num_objects, true, false );
	CLArgument lights_arg( "Light", lights, num_lights, true, false );

	vector<CLArgument> args;
	args.push_back( pixbuf_arg );
	args.push_back( world_arg );
	args.push_back( lights_arg );
	args.push_back( camera_position );
	args.push_back( num_objects );
	args.push_back( num_lights );
	
	rayTrace( args );
}
