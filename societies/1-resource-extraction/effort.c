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
 * This is a Gompertz curve y(t) with:
 *   a = ( max - min ) [upper asymptote]
 *   b = -2 * sqrt( max ) [y displacement]
 *   c = -( max - min ) / ( 2 * max ) [growth rate]
 * See the formula on
 *   http://en.wikipedia.org/w/index.php?title=Gompertz_function&oldid=534656748
 * 
 * Prof. Haarsma wrote:
 *   I realized that I prefer the Gompertz curve 
 *   to the generalized logistic curve for the 
 *   purpose of representing agents learning 
 *   and gaining efficiency through experience.
 *   With the Gompertz curve, as agents gain efficiency, 
 *   the agents initially make small gains in efficiency 
 *   (the initial flat part of the curve), but pretty 
 *   soon start making rapid gains in efficiency.  
 *   But once the curve starts to go flat again when 
 *   the agents have lots of experience, agents continue 
 *   to gain efficiency slowly, and approach the asymptote 
 *   of maximum efficiency in a nice, gradual way.
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
	float f_experience = experience;
	float f_max_experience = max_experience;
	float f_min_effort = min_effort;
	float f_max_effort = max_effort;

	float effort = 
		f_max_effort -
		( ( f_max_effort - f_min_effort )
		  * exp( -sqrt(f_max_effort) * 2 
				* exp( -(f_max_effort - f_min_effort)
					   * f_experience / f_max_experience ) ) );

		return effort;
}
