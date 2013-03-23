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

	ptr = local_malloc( 40, &state );
	local_free( ALLOC_SIZE, &state );

	ptr = local_malloc( ALLOC_SIZE + 20, &state );
	local_free( 60, &state );

	ptr = local_malloc( 80, &state );
	local_free( ALLOC_SIZE + 40, &state );
}
