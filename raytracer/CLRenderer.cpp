#include "CLRenderer.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

CLRenderer::CLRenderer( unsigned char *pixel_buffer, size_t width, size_t height, bool use_cpu )
	: myPixelBuffer( pixel_buffer ), myWidth( width ), myHeight( height )
{
	size_t pixel_buffer_size = width * height * 4;

	// Initialize OpenCL platform and devices.
	// We choose the first platform, and its first GPU device.
    vector<cl::Platform> platforms;
	cl::Platform::get( &platforms );

	if ( use_cpu )
		platforms[0].getDevices( CL_DEVICE_TYPE_CPU, &devices );
	else
		platforms[0].getDevices( CL_DEVICE_TYPE_GPU, &devices );

    context = cl::Context( devices, NULL, NULL, NULL );
    queue = cl::CommandQueue( context, devices[0], 0 );

	// Read OpenCL kernel from a file.
    ifstream t("raytracer.cl");
    string src((std::istreambuf_iterator<char>(t)),
			   std::istreambuf_iterator<char>());

	// Compile kernel.
	cl::Program::Sources sources(
        1,
        std::pair<const char *, int>( src.c_str(), src.length() + 1 )
        );

	cl::Program program(
		context,
        sources );

    try {
        program.build( devices );
    } catch ( cl::Error err ) {
		std::cout << "Build Status: " << program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(devices[0]) << std::endl;
		std::cout << "Build Options:\t" << program.getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(devices[0]) << std::endl;
		std::cout << "Build Log:\t " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]) << std::endl;
		throw new std::exception();
    }

    kernel = cl::Kernel(
		program,
        "raytrace" );

	// Initialize OpenCL pixel buffer.
    cl_pixel_buffer = cl::Buffer(
		context,
        CL_MEM_READ_WRITE,
        sizeof( unsigned char ) * pixel_buffer_size,
        NULL
		);

    kernel.setArg(
        0,
        cl_pixel_buffer );
}

void CLRenderer::render( 
	Object *world, 
	int num_objects, 
	Light *lights, 
	int num_lights, 
	cl_float3 camera_position )
{
    cl_float3 camera;
	size_t pixel_buffer_size = myWidth * myHeight * 4;

	// Objects
	cl::Buffer cl_objects(
        context,
        CL_MEM_READ_ONLY,
        sizeof( Object ) * num_objects,
        NULL
		);
    queue.enqueueWriteBuffer(
        cl_objects,
        CL_TRUE,
		0,
        sizeof( Object ) * num_objects,
        world,
        NULL,
        NULL
		);
    kernel.setArg(
        1,
        cl_objects
		);

	// Lights
	cl::Buffer cl_lights(
        context,
        CL_MEM_READ_ONLY,
        sizeof( Light ) * num_lights,
        NULL
        );
    queue.enqueueWriteBuffer(
        cl_lights,
        CL_TRUE,
		0,
        sizeof( Light ) * num_lights,
        lights,
        NULL,
        NULL
		);
    kernel.setArg(
        2,
        cl_lights
		);

	// Camera position
	cl::Buffer cl_camera(
        context,
        CL_MEM_READ_ONLY,
        sizeof( cl_float3 ),
        NULL
        );
    queue.enqueueWriteBuffer(
        cl_camera,
        CL_TRUE,
		0,
        sizeof( camera_position ),
        &camera_position,
        NULL,
        NULL
		);
    kernel.setArg(
        3,
        cl_camera
	);

	// Number of objects
	cl::Buffer cl_num_objects(
        context,
        CL_MEM_READ_ONLY,
        sizeof( cl_int ),
        NULL
    );
    queue.enqueueWriteBuffer(
        cl_num_objects,
        CL_TRUE,
        0,
        sizeof( num_objects ),
        &num_objects,
        NULL,
        NULL
	);
    kernel.setArg(
        4,
        cl_num_objects
	);

	// Number of lights
	cl::Buffer cl_num_lights(
        context,
        CL_MEM_READ_ONLY,
        sizeof( cl_int ),
        NULL
    );
    queue.enqueueWriteBuffer(
        cl_num_lights,
        CL_TRUE,
        0,
        sizeof( num_lights ),
        &num_lights,
        NULL,
        NULL
	);
    kernel.setArg(
        5,
        cl_num_lights
	);

	cl::NDRange globalWorkSize( myWidth, myHeight );
    queue.enqueueNDRangeKernel(
        kernel,
        cl::NullRange,
        globalWorkSize,
        cl::NullRange,
        NULL,
        NULL );

    queue.enqueueReadBuffer(
        cl_pixel_buffer,
        CL_TRUE,
        0,
        sizeof(unsigned char) * pixel_buffer_size,
        myPixelBuffer,
        NULL,
        NULL );
}
