/**
 * local_malloc: Allow for allocation of workgroup-local
 *  scratch memory.
 *
 * @author John Kloosterman
 * @date March 23, 2012
 */

#ifndef _LOCAL_MALLOC_H
#define _LOCAL_MALLOC_H

typedef struct {
	size_t offset;
	size_t max_size;
	__local char *buffer;
} LocalMallocState;

void local_malloc_init( 
	__local char *buffer, 
	size_t max_size,
	LocalMallocState *state )
{
	state->offset = 0;
	state->max_size = max_size;
	state->buffer = buffer;
}

/**
 * This needs to be callable by all threads, because
 *  a pointer of the form
 *    __local int *ptr;
 *  is stored in private memory, not local memory.
 */
__local void *local_malloc( size_t size, LocalMallocState *state )
{
	state->offset += size;

	if ( state->offset > state->max_size )
	{
		// Is there a way to give better diagnostic information?
		printf( "local_malloc: buffer overflow.\n" );
	}

	return state->buffer + state->offset;
}

void local_free( size_t size, LocalMallocState *state )
{
	state->offset -= size;
}

#endif
