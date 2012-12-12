/**
 * Host interface for OpenCL raytracing renderer.
 *
 * @author John Kloosterman for CS352 at Calvin College
 * @date Dec. 12, 2012
 */

#ifndef _CL_RENDERER_H
#define _CL_RENDERER_H

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>

#define _HOST_
#include "objects.h"

#include <vector>

class CLRenderer
{
public:
	CLRenderer( unsigned char *pixel_buffer, size_t width, size_t height, bool use_cpu );
	void render( Object *world, int num_objects, Light *lights, int num_lights, cl_float3 camera_position );

private:
	size_t myWidth, myHeight;
	unsigned char *myPixelBuffer;
	cl::Context context;
	cl::CommandQueue queue;
	cl::Kernel kernel;
	cl::Buffer cl_pixel_buffer;
	std::vector<cl::Device> devices;
};

#endif
