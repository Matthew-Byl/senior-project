/**
 * LocalMallocAnalyzer takes OpenCL source code
 *  in memory and makes the local_malloc code
 *  word properly.
 *
 * @author John Kloosterman
 * @date March 23, 2013
 */

/* What we all have to do:
   (0) Rewrite function calls to make it easier
       on the programmer. This has to be done here to make
	   the source code syntactically valid.
	     --> could use a different header with prototypes
		     without the malloc object.
       To do this:
	     a) Track which functions call malloc/free.
		 b) Propagate them up the call graph.
		 c) Make a list of function prototypes to rewrite.
   (1) Do static analysis to determine how big the buffer
       needs to be.
   (2) Put in the calls to allocate the buffer and call
       local_malloc_init().
*/

class LocalMallocAnalyzer {
public:
	LocalMallocAnalyzer( std::string src );

private:
	
};
