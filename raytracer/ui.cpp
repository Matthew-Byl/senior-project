/**
 * OpenCL raytracer GTK+ UI code.
 *
 * @author John Kloosterman for CS352 at Calvin College
 * @date Dec. 12, 2012
 */

#include "CLRenderer.h"

#include <gtk/gtk.h>
#include <png.h>

#include <google/profiler.h>

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
using namespace std;

// The size of the rendered scene. Right now, this has
//  to be square or there will be distortion.
#define SIZEX 1000
#define SIZEY 1000
#define PIXEL_BUFFER_SIZE SIZEX * SIZEY * 4

// The pixel buffer that is drawn onto the screen
//  and rendered into.
GdkPixbuf *gdk_pixel_buffer;

// The current camera position
cl_float3 camera_position;

GtkWidget *light_x;
GtkWidget *light_y;
GtkWidget *light_z;
GtkWidget *image;

// The renderer object that will render the scene
CLRenderer *renderer;

// The objects in the scene
#define NUM_OBJECTS 1024
Object objects[NUM_OBJECTS];

#define NUM_SPHERES 6

/**
 * Initially prepare the scene.
 */
void setup_scene()
{
	// objects[0] is the sphere that follows
	//  the light around, that will be filled
	//  in when the scene is rendered.

	// Plane
	objects[1].colour.s[0] = 235;
	objects[1].colour.s[1] = 206;
	objects[1].colour.s[2] = 198;
	objects[1].colour.s[3] = 0;
	objects[1].type = PLANE_TYPE;
	objects[1].position.s[0] = 0;
	objects[1].position.s[1] = 0;
	objects[1].position.s[2] = -1;
	objects[1].objects.plane.normal.s[0] = 0;
	objects[1].objects.plane.normal.s[1] = 0;
	objects[1].objects.plane.normal.s[2] = 1;	

	int num = 2;
	for ( int i = 0; i < NUM_SPHERES; i++ )
	{
		for ( int j = 0; j < NUM_SPHERES; j++ )
		{
			for ( int k = 0; k < NUM_SPHERES; k++ )
			{
				objects[num].colour.s[0] = 100 + i * 10;
				objects[num].colour.s[1] = 100 + k * 10;
				objects[num].colour.s[2] = 100 + j * 10;

				if ( k == 3 )
					objects[num].colour.s[3] = 1;
				else
					objects[num].colour.s[3] = 0;

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

/**
 * Run the OpenCL renderer to redraw the scene.
 */
void run_kernel()
{
	Light light;
	const int num_objects = ( NUM_SPHERES * NUM_SPHERES * NUM_SPHERES ) + 2;

	// Sphere that follows light
	objects[0].colour.s[0] = 253;
	objects[0].colour.s[1] = 204;
	objects[0].colour.s[2] = 135;
	objects[0].colour.s[3] = 0;
	objects[0].type = SPHERE_TYPE;
	objects[0].position.s[0] = gtk_range_get_value( GTK_RANGE( light_x ) );
	objects[0].position.s[1] = gtk_range_get_value( GTK_RANGE( light_y ) );
	objects[0].position.s[2] = gtk_range_get_value( GTK_RANGE( light_z ) );
	objects[0].objects.sphere.radius = 0.1;

	// The diffuse light
	light.position.s[0] = gtk_range_get_value( GTK_RANGE( light_x ) );
	light.position.s[1] = gtk_range_get_value( GTK_RANGE( light_y ) );
	light.position.s[2] = gtk_range_get_value( GTK_RANGE( light_z ) );

	// Render the scene.
	renderer->render( objects, num_objects, &light, 1, camera_position );
	gtk_image_set_from_pixbuf( GTK_IMAGE( image ), gdk_pixel_buffer );
}

/**
 * Save the current image as a PNG.
 *
 * Code based off of example at
 *   http://zarb.org/~gc/html/libpng.html
 */
static void png_button_clicked( GtkWidget *widget, gpointer data )
{
	FILE *fp = fopen( "output.png", "wb" );
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_infop info_ptr = png_create_info_struct( png_ptr );

	png_init_io(png_ptr, fp);

	png_set_IHDR(png_ptr, info_ptr, SIZEX, SIZEY,
				 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
				 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);

	// libpng likes to have pointers to rows of pixels.
	// Happily oblige, however silly because we have all our
	//  pixels in a continuous buffer.
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

/**
 * Exit the program when the window is closed.
 */
static void destroy( GtkWidget *widget,
                     gpointer   data )
{
//	ProfilerStop();
    gtk_main_quit ();
}

bool in_handler = false;
/**
 * Redraw the scene when the sliders are moved.
 */
static void redraw_callback( GtkWidget *widget,
							 gpointer   data )
{
	if ( in_handler )
		return;
	in_handler = true;

	run_kernel();

	while ( gtk_events_pending() )
		gtk_main_iteration();

	in_handler = false;
}

/**
 * Move the x and y position of the camera
 *  based on mouse dragging.
 */
bool dragging = false;
float drag_x, drag_y;
float prev_x, prev_y;
int drag_frame = -1;
static void motion_notify( GtkWidget *widget, GdkEvent *event, gpointer user_data )
{
	if ( in_handler )
		return;
	in_handler = true;

	GdkEventMotion *motion = (GdkEventMotion *) event;

	if ( motion->state & GDK_BUTTON1_MASK )
	{
		if (!dragging )
		{
			prev_x = camera_position.s[1];
			prev_y = camera_position.s[2];
			drag_x = motion->x;
			drag_y = motion->y;
			dragging = true;
		}
		else
		{
			camera_position.s[1] = prev_x + 0.01 * ( motion->x - drag_x );
			camera_position.s[2] = prev_y + 0.01 * ( motion->y - drag_y );

			run_kernel();

			while ( gtk_events_pending() )
				gtk_main_iteration();
		}
	}
	else
	{
		dragging = false;
	}

	in_handler = false;
}

/**
 * Move the z coordinate of the camera based
 *  on mouse wheel scrolling.
 */
static void scroll( GtkWidget *widget, GdkEvent *event, gpointer user_data )
{
	if ( in_handler )
		return;
	in_handler = true;

	GdkEventScroll *scroll = (GdkEventScroll *) event;

	if ( scroll->direction == GDK_SCROLL_UP )
	{
		camera_position.s[0] += 0.3;
	}
	else if ( scroll->direction == GDK_SCROLL_DOWN )
	{
		camera_position.s[0] -= 0.3;
	}

	run_kernel();
	
	// This, along with in_handler is a trick to get rid of
	//  any mouse move or scroll events that are on GTK+s
	//  event queue. If we don't purge them, then we will
	//  render frames that we have no intent of displaying,
	//  and the GPU gets behind.
	while ( gtk_events_pending() )
		gtk_main_iteration();


	in_handler = false;
}

int main( int argc, char *argv[] )
{
//	ProfilerStart( "raytracer.prof" );

	// Set up scene.
	setup_scene();

	// Initialize camera position
	camera_position.s[0] = 0;
	camera_position.s[1] = 0;
	camera_position.s[2] = 0;

	// Initialize GTK+ and the pixel buffer
	gtk_init( &argc, &argv );
	gdk_pixel_buffer = gdk_pixbuf_new( GDK_COLORSPACE_RGB, TRUE, 8, SIZEX, SIZEY );

	// Initialize OpenCL renderer
	bool use_cpu = false;
	if( argc == 2 && strcmp( argv[1], "-cpu" ) == 0 )
		use_cpu = true;

	ifstream t("raytracer.cl");
    string src((std::istreambuf_iterator<char>(t)),
			   std::istreambuf_iterator<char>());

	renderer = new CLRenderer( src, gdk_pixbuf_get_pixels( gdk_pixel_buffer ), SIZEX, SIZEY, use_cpu );

	// Initialize UI
	GtkWidget *window;
	GtkWidget *button;
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *eventBox;
	
	window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	button = gtk_button_new_with_label( "Save as PNG" );

	// Sliders for light position
	light_x = gtk_hscale_new_with_range(
		-30,
		30,
		0.001 );
	gtk_range_set_value( GTK_RANGE( light_x ), 0 );
	g_signal_connect( light_x, "value-changed", G_CALLBACK( redraw_callback ), NULL );

	light_y = gtk_hscale_new_with_range(
		-30,
		30,
		0.001 );
	gtk_range_set_value( GTK_RANGE( light_y ), 0 );
	g_signal_connect( light_y, "value-changed", G_CALLBACK( redraw_callback ), NULL );

	light_z = gtk_hscale_new_with_range(
		-30,
		30,
		0.001 );
	gtk_range_set_value( GTK_RANGE( light_z ), 0 );
	g_signal_connect( light_z, "value-changed", G_CALLBACK( redraw_callback ), NULL );

	// Horizontal layout
	vbox = gtk_vbox_new( FALSE, 10 );
	gtk_widget_set_size_request( vbox, 400, 200 );
	hbox = gtk_hbox_new( FALSE, 10 );

	// Draw the initial image.
	image = gtk_image_new_from_pixbuf( gdk_pixel_buffer );
	eventBox = gtk_event_box_new();
	gtk_container_add( GTK_CONTAINER( eventBox ), image );
	gtk_container_add( GTK_CONTAINER( hbox ), eventBox );

	run_kernel();

	// Vertical layout inside right side.
	gtk_container_add( GTK_CONTAINER( vbox ), button );
	gtk_container_add( GTK_CONTAINER( vbox ),
					   gtk_label_new( "Coordinates of light source:" ) );
	gtk_container_add( GTK_CONTAINER( vbox ), light_x );
	gtk_container_add( GTK_CONTAINER( vbox ), light_y );
	gtk_container_add( GTK_CONTAINER( vbox ), light_z );
	gtk_container_add( GTK_CONTAINER( hbox ), vbox );

	// Save a PNG when the button clicked
	g_signal_connect( button, "clicked", G_CALLBACK( png_button_clicked ), NULL );

	// Exit when the window is destroyed
	g_signal_connect( window, "destroy", G_CALLBACK( destroy ), NULL);

	// Connect signals for mouse move and scroll events
	gtk_widget_set_events( eventBox, GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK );
	g_signal_connect (eventBox, "motion-notify-event", G_CALLBACK (motion_notify), NULL);
	g_signal_connect (eventBox, "scroll-event", G_CALLBACK (scroll), NULL);

	// Add everything to the window, and show it all.
	gtk_container_add( GTK_CONTAINER( window ), hbox );
	gtk_widget_show_all( window );
	gtk_main();

	return 0;
}
