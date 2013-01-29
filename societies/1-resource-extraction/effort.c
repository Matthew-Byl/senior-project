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
 * @param min_effort
 *  The minimum effort to extract a resource.
 * @param max_effort
 *  The maximum effort to extract a resource.
 *
 * @return
 *  The number of minutes the agent needs to extract that
 *   resource.
 */
float resource_effort( int experience, int max_experience, int min_effort, int max_effort )
{
	float f_experience = (float) experience;
	float f_max_experience = (float) max_experience;
	float f_min_effort = (float) min_effort;
	float f_max_effort = (float) max_effort;

	float effort =
		f_max_effort -
		( f_max_effort - f_min_effort )
		* exp( -sqrt( f_max_effort ) * 2
			   * exp( -(f_max_effort - f_min_effort )
					  * f_experience / max_experience ) );

	// No amount of experience allows the amount of effort to go
	//  beneath the minimum.
//	if ( min_effort > effort )
//		return min_effort;
//	else
		return effort;
}
