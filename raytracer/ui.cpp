#include <gtk/gtk.h>

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>

#define _HOST_
#include "objects.h"

#include <CLFunction.h>
#include <CLUnitIntArgument.h>

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
using namespace std;

#define SIZEX 700
#define SIZEY 700
#define PIXEL_BUFFER_SIZE SIZEX * SIZEY * 4
GdkPixbuf *gdk_pixel_buffer;
//unsigned char pixel_buffer[PIXEL_BUFFER_SIZE];
cl::Context context;
cl::CommandQueue queue;
cl::Kernel kernel;
cl::Buffer cl_pixel_buffer;
vector<cl::Device> devices;

void run_tests()
{
	CLContext context( 0, 0 );
    ifstream t("raytracer.cl");
    string src((std::istreambuf_iterator<char>(t)),
			   std::istreambuf_iterator<char>());

	CLFunction ray_direction( "get_ray_direction", src, context );
	CLUnitIntArgument x( context, 0 );
	CLUnitIntArgument num_x( context, 10 );
	CLUnitIntArgument y( context, 15 );
	CLUnitIntArgument num_y( context, 15 );
	cl_float3 result;
	
	ray_direction.addArgument( x );
	ray_direction.addArgument( num_x );
	ray_direction.addArgument( y );
	ray_direction.addArgument( num_y );

	result = ray_direction.run<cl_float3>( "float3" );

	printf( "Result: %f %f %f\n", result.s[0], result.s[1], result.s[2] );
}

void run_kernel()
{
#define NUM_OBJECTS 1
	Object objects[NUM_OBJECTS];
	Light light;
	cl_float3 camera;

	objects[0].colour.s[0] = 255;
	objects[0].colour.s[1] = 0;
	objects[0].colour.s[2] = 0;
	objects[0].type = SPHERE_TYPE;
	objects[0].position.s[0] = 0;
	objects[0].position.s[1] = 0;
	objects[0].position.s[2] = 0;
	objects[0].objects.sphere.radius = 0.5;
	
	light.position.s[0] = 10;
	light.position.s[1] = 10;
	light.position.s[2] = 10;

	camera.s[0] = 0;
	camera.s[1] = 0;
	camera.s[2] = -5;

	cl::Buffer cl_objects(
		context,
		CL_MEM_READ_ONLY,
		sizeof( Object ) * NUM_OBJECTS,
		NULL
	);
	queue.enqueueWriteBuffer(
		cl_objects,
		CL_TRUE,
		0,
		sizeof( Object ) * NUM_OBJECTS,
		objects,
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

	cl::NDRange globalWorkSize( SIZEX, SIZEY );
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
		sizeof(unsigned char) * PIXEL_BUFFER_SIZE,
		gdk_pixbuf_get_pixels( gdk_pixel_buffer ),
		NULL,
		NULL );

//	for ( int i = 0; i < PIXEL_BUFFER_SIZE; i++ )
//		printf( "%d ", gdk_pixbuf_get_pixels( gdk_pixel_buffer)[i] );

//	printf( "\n" );
}

static void clicked( GtkWidget *widget, gpointer data )
{
	clock_t start = clock();
	for ( int i = 0; i < 100000; i++ )
	{
		run_kernel();
//		gtk_image_set_from_pixbuf( GTK_IMAGE( data ), gdk_pixel_buffer );
	}
	clock_t end = clock();

	printf( "Took %d for 60.\n", end - start );
}

static void destroy( GtkWidget *widget,
                     gpointer   data )
{
    gtk_main_quit ();
}

int main( int argc, char *argv[] )
{
	run_tests();

	gtk_init( &argc, &argv );
	gdk_pixel_buffer = gdk_pixbuf_new( GDK_COLORSPACE_RGB, TRUE, 8, SIZEX, SIZEY );

	// Initialize OpenCL
	vector<cl::Platform> platforms;
	cl::Platform::get( &platforms );

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
		return 1;
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
		sizeof( unsigned char ) * PIXEL_BUFFER_SIZE,
		NULL
	);

	kernel.setArg(
		0,
		cl_pixel_buffer );

	run_kernel();


	// Initialize UI
	GtkWidget *window;
	GtkWidget *button;
	GtkWidget *image;
	GtkWidget *vbox;
	
	window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	button = gtk_button_new_with_label( "Hello, world!" );
	image = gtk_image_new_from_pixbuf( gdk_pixel_buffer );
	vbox = gtk_vbox_new( FALSE, 10 );

	g_signal_connect( button, "clicked", G_CALLBACK( clicked ), (gpointer) image );

	g_signal_connect (window, "destroy",
					  G_CALLBACK (destroy), NULL);

	gtk_container_add( GTK_CONTAINER( vbox ), image );
	gtk_container_add( GTK_CONTAINER( vbox ), button );
	gtk_container_add( GTK_CONTAINER( window ), vbox );

	gtk_widget_show_all( window );

	gtk_main();

	return 0;
}
