#define NUM_CHANNELS 4

__kernel void raytrace( __global uchar4 *pixbuf )
{
	size_t x = get_global_id( 0 );
	size_t y = get_global_id( 1 );

	// Important for stride.
	size_t num_y = get_global_size( 1 ) - 1;
	
	// set to red.
	__global uchar4 *pixel = pixbuf + ( y * num_y ) + x;
//	printf( "x, y: %d %d Pixel address: %d %d\n", x, y, pixel, num_y );

	if ( x % 2 == 0 )
		(*pixel).x = 255;
	else
		(*pixel).x = 0;		

	(*pixel).y = 0;
	(*pixel).z = 0;
	(*pixel).w = 0;
}
