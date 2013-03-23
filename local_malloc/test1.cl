/*
 * test0: simple allocation and deallocation.
 */

#include "local_malloc.h"

#define LOCAL_MALLOC_SIZE 200
#define ALLOC_SIZE 40

__kernel
void entry( void )
{
	__local char buffer[LOCAL_MALLOC_SIZE];
	LocalMallocState state;
	__local void *ptr;

	local_malloc_init( buffer, LOCAL_MALLOC_SIZE, &state );

	for ( int i = 0; i < 20; i++ )
	{
		if ( i % 2 )
		{
			ptr = local_malloc( 20, &state );
			local_free( 20, &state );
		}
		else
		{
			ptr = local_malloc( 40, &state );
			local_free( 40, &state );
		}
	}
}
