#include "CLRenderer.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

CLRenderer::CLRenderer( unsigned char *pixel_buffer, size_t width, size_t height )
	: myPixelBuffer( pixel_buffer ), myWidth( width ), myHeight( height )
{
    vector<cl::Platform> platforms;
	cl::Platform::get( &platforms );
	size_t pixel_buffer_size = width * height * 4;

    platforms[0].getDevices( CL_DEVICE_TYPE_GPU, &devices );

    context = cl::Context( devices, NULL, NULL, NULL );
    queue = cl::CommandQueue( context, devices[0], 0 );

    ifstream t("raytracer.cl");
    string src((std::istreambuf_iterator<char>(t)),
			   std::istreambuf_iterator<char>());

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

	std::cout << "Build Status: " << program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(devices[0]) << std::endl;
	std::cout << "Build Options:\t" << program.getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(devices[0]) << std::endl;
	std::cout << "Build Log:\t " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]) << std::endl;


    kernel = cl::Kernel(
		program,
        "raytrace" );

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

void CLRenderer::render( Object *world, int num_objects, cl_float3 camera_position )
{
    Light light;
    cl_float3 camera;
	size_t pixel_buffer_size = myWidth * myHeight * 4;

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

	cl::Buffer cl_lights(
        context,
        CL_MEM_READ_ONLY,
        sizeof( light ),
        NULL
        );
    queue.enqueueWriteBuffer(
        cl_lights,
        CL_TRUE,
		0,
        sizeof( light ),
        &light,
        NULL,
        NULL
		);
    kernel.setArg(
        2,
        cl_lights
		);

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
