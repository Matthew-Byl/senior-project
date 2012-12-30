#ifndef _TREE_ARRAY_H
#define _TREE_ARRAY_H

/*
 * Helper functions for storing trees in arrays.
 */

int tree_array_first_child( int ary, int node );
int tree_array_parent( int ary, int node );
int tree_array_size( int ary, int depth );

#endif
