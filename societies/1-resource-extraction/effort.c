/**
 * Functions to compute the effort needed to
 *  extract a resource.
 */

/**
 * Let @c max be the maximum effort to extract a resource.
 * Let @c min be the minimum effort to extract a resource.
 *
 * Then the effort to extract a resource given @c exp experience
 *  with that resource is:
 *
 * max - ((max - min)*e^(-2*sqrt(max))*e^(-(max - min)/2)*(exp/max))
 *
 * @param experience
 *  The amount of experience the agent has with a given
 *   resource.
 * @param max_experience
 *  The maximum amount of experience with a resource agents
 *   can have.
 * @param max_effort
 *  The maximum effort to extract a resource.
 * @param min_effort
 *  The minimum effort to extract a resource.
 *
 * @return
 *  The number of minutes the agent needs to extract that
 *   resource.
 */
float resource_effort( int experience, int max_experience, int max_effort, int min_effort )
{
	float term2 = 
		( max_effort - min_effort )
		* exp( -2 * sqrt( (float) max_effort ) )
		* exp( -( (float) max_effort - min_effort ) / 2 )
		* ( experience / max_experience );
	float effort = max_effort - term2;

	// No amount of experience allows the amount of effort to go
	//  beneath the minimum.
	if ( effort < min_effort )
		return min_effort;
	else
		return effort;
}
