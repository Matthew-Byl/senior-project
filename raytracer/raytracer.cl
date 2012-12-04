#define NUM_CHANNELS 4

__kernel void raytrace( __global uchar4 *pixbuf )
{
	size_t x = get_global_id( 0 );
	size_t y = get_global_id( 1 );

	// Important for stride.
	size_t num_x = get_global_size( 0 );
	
	// set to red.
	__global uchar4 *pixel = pixbuf + ( y * num_x ) + x;
//	printf( "x, y: %d %d Pixel address: %d %d\n", x, y, pixel, num_y );

	if ( x % 2 == 0 )
		(*pixel).x = 0;
	else
		(*pixel).x = 255;
	(*pixel).y = 255;
	(*pixel).z = 0;

	// 255 is no transparency.
	(*pixel).w = 255;
}
