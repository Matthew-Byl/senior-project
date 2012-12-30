#include "tree_array.h"

#ifndef _OPENCL_
#include <math.h>
#endif

int tree_array_first_child( int ary, int node )
{
	return ( node * ary ) + 1;
}

int tree_array_parent( int ary, int node )
{
	// Integer division rounds down.
	return ( node - 1 ) / ary;
}

/*
  Due to a theorem (https://ece.uwaterloo.ca/~dwharder/aads/LectureMaterials/4.07.NaryTrees.pdf),         
  there are ( N^(k+1) - 1 ) / ( N - 1 ) nodes in a complete N-ary tree.
*/
int tree_array_size( int ary, int depth )
{
    return 
		( (int) pow( (float) ary, depth + 1 ) - 1 ) 
		/ ( ary - 1 );
}
