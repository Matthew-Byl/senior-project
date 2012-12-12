#include <gtk/gtk.h>

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>

#define _HOST_
#include "objects.h"

#include <CLFunction.h>
#include <CLUnitIntArgument.h>

#include <png.h>

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
using namespace std;

#define SIZEX 1000
#define SIZEY 1000
#define PIXEL_BUFFER_SIZE SIZEX * SIZEY * 4
GdkPixbuf *gdk_pixel_buffer;
//unsigned char pixel_buffer[PIXEL_BUFFER_SIZE];
cl::Context context;
cl::CommandQueue queue;
cl::Kernel kernel;
cl::Buffer cl_pixel_buffer;
vector<cl::Device> devices;
cl_float3 camera_position;

GtkWidget *camera_x;
GtkWidget *camera_y;
GtkWidget *camera_z;
GtkWidget *light_x;
GtkWidget *light_y;
GtkWidget *light_z;
GtkWidget *image;

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

#define NUM_OBJECTS 1024
Object objects[NUM_OBJECTS];
void thousand_spheres()
{
	int num = 1;
	for ( int i = 0; i < 10; i++ )
	{
		for ( int j = 0; j < 10; j++ )
		{
			for ( int k = 0; k < 10; k++ )
			{
				objects[num].colour.s[0] = 100 + i * 10;
				objects[num].colour.s[1] = 100 + k * 10;
				objects[num].colour.s[2] = 100 + j * 10;
				objects[num].type = SPHERE_TYPE;
				objects[num].position.s[0] = i;
				objects[num].position.s[1] = j;
				objects[num].position.s[2] = k;
				objects[num].objects.sphere.radius = 0.2;

				num++;
			}
		}
	}	
}

void run_kernel()
{
	Light light;
	cl_float3 camera;

	objects[0].colour.s[0] = 255;
	objects[0].colour.s[1] = 255;
	objects[0].colour.s[2] = 0;
	objects[0].type = SPHERE_TYPE;
	objects[0].position.s[0] = gtk_range_get_value( GTK_RANGE( light_x ) );
	objects[0].position.s[1] = gtk_range_get_value( GTK_RANGE( light_y ) );
	objects[0].position.s[2] = gtk_range_get_value( GTK_RANGE( light_z ) );
	objects[0].objects.sphere.radius = 0.1;

	const int num_objects = 1001;
/*
	if ( NUM_OBJECTS > 2 )
	{
		objects[2].colour.s[0] = 2;
		objects[2].colour.s[1] = 2;
		objects[2].colour.s[2] = 2;
		objects[2].type = PLANE_TYPE;
		objects[2].position.s[0] = 0;
		objects[2].position.s[1] = 0;
		objects[2].position.s[2] = -1;
		objects[2].objects.plane.normal.s[0] = 0;
		objects[2].objects.plane.normal.s[1] = 0;
		objects[2].objects.plane.normal.s[2] = 1;	
	}
*/
/*
	int num = 3;
	for ( int i = 0; i < 10; i++ )
	{
		for ( int j = 0; j < 10; j++ )
		{
			for ( int k = 0; k < 10; k++ )
			{
				objects[num].colour.s[0] = 100 + i * 10;
				objects[num].colour.s[1] = 100 + k * 10;
				objects[num].colour.s[2] = 100 + j * 10;
				objects[num].type = SPHERE_TYPE;
				objects[num].position.s[0] = i;
				objects[num].position.s[1] = j;
				objects[num].position.s[2] = k;
				objects[num].objects.sphere.radius = 0.2;

				num++;
			}
		}
	}
*/

	light.position.s[0] = gtk_range_get_value( GTK_RANGE( light_x ) );
	light.position.s[1] = gtk_range_get_value( GTK_RANGE( light_y ) );
	light.position.s[2] = gtk_range_get_value( GTK_RANGE( light_z ) );

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
    // from http://zarb.org/~gc/html/libpng.html
	FILE *fp = fopen( "output.png", "wb" );
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_infop info_ptr = png_create_info_struct( png_ptr );

	png_init_io(png_ptr, fp);


	png_set_IHDR(png_ptr, info_ptr, SIZEX, SIZEY,
				 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
				 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);

	unsigned char *pixels = gdk_pixbuf_get_pixels( gdk_pixel_buffer );
	png_byte *row_pointers[SIZEY];
	for ( int i = 0; i < SIZEY; i++ )
	{
		row_pointers[i] = pixels + ( SIZEX * 4 * i );
	}

	png_write_image(png_ptr, row_pointers);
	png_write_end(png_ptr, NULL);
	fclose(fp);
}

static void destroy( GtkWidget *widget,
                     gpointer   data )
{
    gtk_main_quit ();
}

static void move_camera( GtkWidget *widget,
						 gpointer   data )
{
	gfloat x = gtk_range_get_value( GTK_RANGE( camera_x ) );
	gfloat y = gtk_range_get_value( GTK_RANGE( camera_y ) );
	gfloat z = gtk_range_get_value( GTK_RANGE( camera_z ) );

	camera_position.s[0] = x;
	camera_position.s[1] = y;
	camera_position.s[2] = z;

	run_kernel();
	gtk_image_set_from_pixbuf( GTK_IMAGE( image ), gdk_pixel_buffer );
}


int main( int argc, char *argv[] )
{
	thousand_spheres();

	camera_position.s[0] = 0;
	camera_position.s[1] = 0;
	camera_position.s[2] = 0;

//	run_tests();

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

	// Initialize UI
	GtkWidget *window;
	GtkWidget *button;
	GtkWidget *vbox;
	GtkWidget *hbox;
	
	window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	button = gtk_button_new_with_label( "Hello, world!" );


	light_x = gtk_hscale_new_with_range(
		-30,
		30,
		0.1 );
	gtk_range_set_value( GTK_RANGE( light_x ), 0 );
	g_signal_connect( light_x, "value-changed", G_CALLBACK( move_camera ), NULL );

	light_y = gtk_hscale_new_with_range(
		-30,
		30,
		0.1 );
	gtk_range_set_value( GTK_RANGE( light_y ), 0 );
	g_signal_connect( light_y, "value-changed", G_CALLBACK( move_camera ), NULL );

	light_z = gtk_hscale_new_with_range(
		-30,
		30,
		0.1 );
	gtk_range_set_value( GTK_RANGE( light_z ), 0 );
	g_signal_connect( light_z, "value-changed", G_CALLBACK( move_camera ), NULL );


	run_kernel();
	image = gtk_image_new_from_pixbuf( gdk_pixel_buffer );
	vbox = gtk_vbox_new( FALSE, 10 );
	gtk_widget_set_size_request( vbox, 200, -1 );

	hbox = gtk_hbox_new( FALSE, 10 );

	camera_x = gtk_hscale_new_with_range(
		-30,
		30,
		0.1 );
	gtk_range_set_value( GTK_RANGE( camera_x ), 0 );
	g_signal_connect( camera_x, "value-changed", G_CALLBACK( move_camera ), NULL );

	camera_y = gtk_hscale_new_with_range(
		-30,
		30,
		0.1 );
	gtk_range_set_value( GTK_RANGE( camera_y ), 0 );
	g_signal_connect( camera_y, "value-changed", G_CALLBACK( move_camera ), NULL );

	camera_z = gtk_hscale_new_with_range(
		-30,
		30,
		0.1 );
	gtk_range_set_value( GTK_RANGE( camera_z ), 0 );
	g_signal_connect( camera_z, "value-changed", G_CALLBACK( move_camera ), NULL );


	g_signal_connect( button, "clicked", G_CALLBACK( clicked ), (gpointer) image );
	g_signal_connect (window, "destroy", G_CALLBACK (destroy), NULL);

	gtk_container_add( GTK_CONTAINER( hbox ), image );
	gtk_container_add( GTK_CONTAINER( vbox ), button );
	gtk_container_add( GTK_CONTAINER( vbox ), camera_x );
	gtk_container_add( GTK_CONTAINER( vbox ), camera_y );
	gtk_container_add( GTK_CONTAINER( vbox ), camera_z );
	gtk_container_add( GTK_CONTAINER( vbox ), light_x );
	gtk_container_add( GTK_CONTAINER( vbox ), light_y );
	gtk_container_add( GTK_CONTAINER( vbox ), light_z );
	gtk_container_add( GTK_CONTAINER( hbox ), vbox );
	gtk_container_add( GTK_CONTAINER( window ), hbox );

	gtk_widget_show_all( window );

	gtk_main();

	return 0;
}
