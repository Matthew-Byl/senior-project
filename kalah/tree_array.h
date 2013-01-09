#ifndef _TREE_ARRAY_H
#define _TREE_ARRAY_H

/*
 * Helper functions for storing n-ary trees in arrays.
 *
 * In general, for an n-ary tree, the children of the node
 *  with index k are stored in indices n*k through n*k + (n - 1).
 *
 * @author John Kloosterman
 * @date December 2012
 */

int tree_array_first_child( int ary, int node );
int tree_array_parent( int ary, int node );
int tree_array_size( int ary, int depth );

#endif
