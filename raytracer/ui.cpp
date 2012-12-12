#include "CLRenderer.h"

#include <gtk/gtk.h>

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
cl_float3 camera_position;

GtkWidget *light_x;
GtkWidget *light_y;
GtkWidget *light_z;
GtkWidget *image;

CLRenderer *renderer;

#define NUM_OBJECTS 1024
Object objects[NUM_OBJECTS];
void thousand_spheres()
{
	int num = 2;
	for ( int i = 0; i < 5; i++ )
	{
		for ( int j = 0; j < 5; j++ )
		{
			for ( int k = 0; k < 5; k++ )
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

void run_kernel()
{
	Light light;
	const int num_objects = 126;

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
	objects[1].objects.plane.normal.s[1] = 1;
	objects[1].objects.plane.normal.s[2] = 1;	

	// The diffuse light
	light.position.s[0] = gtk_range_get_value( GTK_RANGE( light_x ) );
	light.position.s[1] = gtk_range_get_value( GTK_RANGE( light_y ) );
	light.position.s[2] = gtk_range_get_value( GTK_RANGE( light_z ) );

	// Render the scene.
	renderer->render( objects, num_objects, &light, 1, camera_position );
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
	run_kernel();
	gtk_image_set_from_pixbuf( GTK_IMAGE( image ), gdk_pixel_buffer );
}

bool dragging = false;
float drag_x, drag_y;
float prev_x, prev_y;
int drag_frame = -1;
static void motion_notify( GtkWidget *widget, GdkEvent *event, gpointer user_data )
{
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
			drag_frame++;
			if ( drag_frame % 5 != 0 )
				return;

			camera_position.s[1] = prev_x + 0.01 * ( motion->x - drag_x );
			camera_position.s[2] = prev_y + 0.01 * ( motion->y - drag_y );

			run_kernel();
			gtk_image_set_from_pixbuf( GTK_IMAGE( image ), gdk_pixel_buffer );
		}
	}
	else
	{
		dragging = false;
	}
}

static void scroll( GtkWidget *widget, GdkEvent *event, gpointer user_data )
{
	GdkEventScroll *scroll = (GdkEventScroll *) event;

	if ( scroll->direction == GDK_SCROLL_UP )
	{
		camera_position.s[0] += 0.1;
	}
	else if ( scroll->direction == GDK_SCROLL_DOWN )
	{
		camera_position.s[0] -= 0.1;
	}

	run_kernel();
	gtk_image_set_from_pixbuf( GTK_IMAGE( image ), gdk_pixel_buffer );
}

int main( int argc, char *argv[] )
{
	thousand_spheres();

	camera_position.s[0] = 0;
	camera_position.s[1] = 0;
	camera_position.s[2] = 0;

	gtk_init( &argc, &argv );
	gdk_pixel_buffer = gdk_pixbuf_new( GDK_COLORSPACE_RGB, TRUE, 8, SIZEX, SIZEY );

	renderer = new CLRenderer( gdk_pixbuf_get_pixels( gdk_pixel_buffer ), SIZEX, SIZEY );

	// Initialize UI
	GtkWidget *window;
	GtkWidget *button;
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *eventBox;
	
	window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	button = gtk_button_new_with_label( "Save as PNG" );

	light_x = gtk_hscale_new_with_range(
		-30,
		30,
		0.01 );
	gtk_range_set_value( GTK_RANGE( light_x ), 0 );
	g_signal_connect( light_x, "value-changed", G_CALLBACK( move_camera ), NULL );

	light_y = gtk_hscale_new_with_range(
		-30,
		30,
		0.01 );
	gtk_range_set_value( GTK_RANGE( light_y ), 0 );
	g_signal_connect( light_y, "value-changed", G_CALLBACK( move_camera ), NULL );

	light_z = gtk_hscale_new_with_range(
		-30,
		30,
		0.01 );
	gtk_range_set_value( GTK_RANGE( light_z ), 0 );
	g_signal_connect( light_z, "value-changed", G_CALLBACK( move_camera ), NULL );


	run_kernel();
	image = gtk_image_new_from_pixbuf( gdk_pixel_buffer );
	vbox = gtk_vbox_new( FALSE, 10 );
	gtk_widget_set_size_request( vbox, 200, -1 );

	hbox = gtk_hbox_new( FALSE, 10 );

	g_signal_connect( button, "clicked", G_CALLBACK( clicked ), (gpointer) image );
	g_signal_connect (window, "destroy", G_CALLBACK (destroy), NULL);

	eventBox = gtk_event_box_new();
	gtk_widget_set_events( eventBox, GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK );
	g_signal_connect (eventBox, "motion-notify-event", G_CALLBACK (motion_notify), NULL);
	g_signal_connect (eventBox, "scroll-event", G_CALLBACK (scroll), NULL);

	gtk_container_add( GTK_CONTAINER( eventBox ), image );
	gtk_container_add( GTK_CONTAINER( hbox ), eventBox );
	gtk_container_add( GTK_CONTAINER( vbox ), button );
	gtk_container_add( GTK_CONTAINER( vbox ), light_x );
	gtk_container_add( GTK_CONTAINER( vbox ), light_y );
	gtk_container_add( GTK_CONTAINER( vbox ), light_z );
	gtk_container_add( GTK_CONTAINER( hbox ), vbox );
	gtk_container_add( GTK_CONTAINER( window ), hbox );

//	gtk_widget_set_double_buffered( image, FALSE );

	gtk_widget_show_all( window );

	gtk_main();

	return 0;
}
