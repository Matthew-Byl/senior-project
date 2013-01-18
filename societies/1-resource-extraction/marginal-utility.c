/**
 * Functions to compute economic utility.
 */

/**
 * Computes MU(x) = (D/n) * x^((1/n) - 1 ).
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
	float C = (float) D / (float) n;

	float exponent = ( 1.0f / (float) x ) - 1.0f;
	return C * pow( (float) x, exponent );
}

/**
 * Compute the gain per minute for a given resource.
 *   See page 7 of the paper.
 *
 * @param x
 *  The number of units of the resource already owned.
 * @param e
 *  The agent's current effort to extract the resource.
 */
float gpm( int x, float e )
{
	
}
