/**
 * Functions to compute economic utility.
 */

/**
 * Computes the utility of a resource when posessing
 *  x units of that resource, using the formula
 *     U(x) = D * x^( 1/n ).
 *  See page 5 of the paper.
 *
 * @param x
 *  The number of units of the resource already owned.
 * @param D
 *  The level of utility for the resource.
 * @param n
 *  The diminishing marginal utility for the resource.
 */
float u( int x, int D, int n )
{
	float f_x = x;
	float f_D = D;
	float f_n = n;

	return f_D * pow( f_x, 1.0f / f_n );
}

/**
 * Computes the marginal utility of the next unit of
 *  a resource, given that x units are already owned,
 *  using the formula
 *     MU(x) = U(k) - U(k-1)
 *  See page 5 of the paper.
 *
 * @param x
 *  The number of units of the resource already owned.
 * @param D
 *  The level of utility for the resource.
 * @param n
 *  The diminishing marginal utility for the resource.
 */
float mu( int x, int D, int n )
{
	return u( x, D, n ) - u( x - 1, D, n );
}

/**
 * Compute the gain per minute for a given resource.
 *   See page 7 of the paper.
 *
 * @param x
 *  The number of units of the resource already owned.
 * @param D
 *  The level of utility for the resource.
 * @param n
 *  The diminishing marginal utility for the resource.
 * @param e
 *  The agent's current effort to extract the resource.
 */
float gpm( int x, int D, int n, float e )
{
	return mu( x + 1, D, n ) / e;
}
