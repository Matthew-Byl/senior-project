#include <gtk/gtk.h>

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
using namespace std;

#define SIZEX 100
#define SIZEY 150
#define PIXEL_BUFFER_SIZE SIZEX * SIZEY * 4
GdkPixbuf *gdk_pixel_buffer;
//unsigned char pixel_buffer[PIXEL_BUFFER_SIZE];
cl::Context context;
cl::CommandQueue queue;
cl::Kernel kernel;
cl::Buffer cl_pixel_buffer;
vector<cl::Device> devices;

void run_kernel()
{
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
//	run_kernel();
	gtk_image_set_from_pixbuf( GTK_IMAGE( data ), gdk_pixel_buffer );
}

int main( int argc, char *argv[] )
{
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

//	run_kernel();

	unsigned char *pb = gdk_pixbuf_get_pixels( gdk_pixel_buffer );
	printf( "%d\n", gdk_pixbuf_get_rowstride (gdk_pixel_buffer) );
	for ( int i = 0; i < SIZEX; i++ )
	{
		for ( int j = 0; j < SIZEY; j++ )
		{
			unsigned char *px = pb + ( SIZEX * j * 4 ) + ( i * 4 );

			px[0] = 0;
			px[1] = 0;
			px[2] = 255;
			px[3] = 255;
		}
	}

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

	gtk_container_add( GTK_CONTAINER( vbox ), image );
	gtk_container_add( GTK_CONTAINER( vbox ), button );
	gtk_container_add( GTK_CONTAINER( window ), vbox );

	gtk_widget_show_all( window );

	gtk_main();

	return 0;
}
