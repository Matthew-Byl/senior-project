/*
 * test0: simple allocation and deallocation.
 */

#include "local_malloc.h"

#define LOCAL_MALLOC_SIZE 200
#define ALLOC_SIZE 40

__kernel
void entry( void )
{
	__local void *ptr;

	ptr = local_malloc( 40 );
	ptr = local_malloc( ALLOC_SIZE + 20 );
	local_free( 60 );
	local_free( ALLOC_SIZE );

	ptr = local_malloc( 80 );
	local_free( ALLOC_SIZE + 40 );
}
