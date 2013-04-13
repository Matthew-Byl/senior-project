/*
 * test0: simple allocation and deallocation.
 */

#define LOCAL_MALLOC_SIZE 200
#define ALLOC_SIZE 40

void some_function( int i )
{
	__local void *ptr = local_malloc( 20 );
}

void another_function( void )
{
	__local void *ptr = local_malloc( 20 );
}

__kernel
void entry( void )
{
	int i = get_local_id( 0 );

	some_function( 10 );
	another_function();
}
