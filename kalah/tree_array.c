#include "tree_array.h"

#ifndef _OPENCL_
#include <math.h>
#endif

/**
 * Returns the index of the first child node of 
 *  a given node.
 * 
 * @param ary
 *  How many children each node in the tree has.
 * @param depth
 *  How many levels the tree has.
 */
int tree_array_first_child( int ary, int node )
{
	return ( node * ary ) + 1;
}

/**
 * Returns the index of the parent node of a 
 *  given node.
 *
 * @param ary
 *  How many children each node in the tree has.
 * @param depth
 *  How many levels the tree has.
 */
int tree_array_parent( int ary, int node )
{
	// Integer division rounds down.
	return ( node - 1 ) / ary;
}

/**
 * Finds the total number of nodes in a complete n-ary
 *  tree.
 *  Due to a theorem (https://ece.uwaterloo.ca/~dwharder/aads/LectureMaterials/4.07.NaryTrees.pdf),
 *    there are ( N^(k+1) - 1 ) / ( N - 1 ) nodes in a complete N-ary tree.
 *
 * @param ary
 *  How many children each node in the tree has.
 * @param depth
 *  How many levels the tree has.
*/
int tree_array_size( int ary, int depth )
{
    return 
		( (int) pow( (float) ary, depth + 1 ) - 1 ) 
		/ ( ary - 1 );
}
