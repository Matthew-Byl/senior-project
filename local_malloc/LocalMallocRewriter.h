/**
 * LocalMallocAnalyzer takes OpenCL source code
 *  in memory and makes the local_malloc code
 *  word properly.
 *
 * @author John Kloosterman
 * @date March 23, 2013
 */

#include <string>

class LocalMallocRewriter {
public:
	LocalMallocRewriter( std::string src );
	~LocalMallocRewriter();

	/**
	 * Get rewritten source code.
	 *
	 * @param entry
	 *  The name of the kernel to process.
	 * @return
	 *  Rewritten source code.
	 */
	std::string rewrite( std::string entry);

private:
	std::string mySrc;
	char *myTempFileName;
};
