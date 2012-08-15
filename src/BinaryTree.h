/* 
Cross Platform Memory Manager	v 1.0.0

Copyright (c) 2009-2010 Pradip (dotB) Bhattacharya

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 3, 29 June 2007 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA <http://fsf.org/>

Please feel free to contact me via e-mail: pradip.bhattacharya@gmail.com
*/


#ifndef	BINARY_TREE
#define	BINARY_TREE


#include	"MemoryAlloctor.h"


#define	ABORT_SEARCH	0
#define	CONTINUE_SEARCH	1

typedef	void(*tree_search_callback)(MemoryChunk*,uint32 *flag,uint32 desired_size);


//Self balanced binary tree
// This tree is going to maintain almost equal depth on both sides
typedef	struct	_BinaryTree
{
	MemoryChunk	*root;
	uint32	totalChunks;
	uint32	right_depth;
	uint32	left_depth;
	uint32	right_elem_count;
	uint32	left_elem_count;
	uint32	right_leaf_count;
	uint32	left_leaf_count;
}BinaryTree;


void	insert_chunk(BinaryTree	*tree,MemoryChunk	*chunk);
void	remove_chunk(BinaryTree	*tree,MemoryChunk	*chunk);
boolean	init_binary_tree(BinaryTree *tree);
MemoryChunk*	get_closet_chunk(BinaryTree	*tree,uint32	size,tree_search_callback search_fn);
MemoryChunk*	get_bigger_chunk(BinaryTree	*tree,uint32	size,tree_search_callback search_fn);


#endif	//BINARY_TREE
