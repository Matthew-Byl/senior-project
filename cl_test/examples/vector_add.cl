/**
 * vector_add.cl: add two arrays of floats together
 *  into a third array.
 *
 * @author John Kloosterman
 * @date April 9, 2013
 */

__kernel void
vector_add(
	__global float *a,
	__global float *b,
	__global float *result
	)
{
	size_t global_id = get_global_id( 0 );

	result[global_id] = a[global_id] + b[global_id];
}
