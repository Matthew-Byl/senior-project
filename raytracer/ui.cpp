#include <gtk/gtk.h>

#define __OPENCL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>

#include <string>
#include <vector>
#include <fstream>
using namespace std;

unsigned char pixel_buffer[500*500];
cl::Context context;
cl::CommandQueue queue;
cl::Kernel kernel;
vector<cl::Device> devices;

void run_kernel()
{
	
}

int main( int argc, char *argv[] )
{
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

	program.build(
		devices,
		NULL,
		NULL,
		NULL );

	kernel = cl::Kernel(
		program,
		"raytrace" );

	// Initialize UI
	GtkWidget *window;
	GtkWidget *button;

	gtk_init( &argc, &argv );
	
	window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	button = gtk_button_new_with_label( "Hello, world!" );

	gtk_container_add( GTK_CONTAINER( window ), button );
	gtk_widget_show( button );
	gtk_widget_show( window );

	gtk_main();

	return 0;
}
