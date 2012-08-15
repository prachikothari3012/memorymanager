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


#include	"BinaryTree.h"


#define	GET_FLAG(x)	((x&0x80000000)>>31)
#define	GET_SIZE(x)	(x&0x8FFFFFFF)


enum	TRAVERSE_DIRECTION
{
	DOWN_RIGHT,
	DOWN_LEFT,
	UP_RIGHT,
	UP_LEFT
};


boolean	isGreaterOrEqual(uint32 desired_size,uint32 disc_size)
{
	if(disc_size >= desired_size)
	{
		return	TRUE;
	}
	else
	{
		return	FALSE;
	}
}


boolean	init_binary_tree(BinaryTree *tree)
{
	if(!tree)
	{
		return FALSE;
	}

	tree->root = 0;
	tree->totalChunks = 0;
	tree->left_depth = 0;
	tree->right_depth = 0;
	tree->left_elem_count = 0;
	tree->right_elem_count = 0;
	tree->left_leaf_count = 0;
	tree->right_leaf_count = 0;

	return TRUE;
}


void	balance_tree(BinaryTree	*tree)
{
	/*
							A
						   / \
					      B	  E
						 / \   
						C   F  
					   /   / \
					  D	  J   G
					 /         \
					L		    I
				   /		   / 
				  M			  K	  

		diff = abs(left_depth - right_depth)

		diff => 2

		since left is the larger side we will traverse left by diff/2 element(s)

		Hence the new balanced tree is

						B
					   / \
					  C	  A
					 /	   \
				    D		E
	*/

	int32 diff;
	boolean	bLeftLarge,bRightLarge;
	if(!tree || !tree->root)
	{
		return;
	}

	diff	=	tree->right_depth - tree->left_depth;

	if(diff < 0)
	{
		diff = -diff;
	}

	if(diff <= 1)
	{
		return;
	}

	if(tree->right_depth > tree->left_depth)
	{
		bRightLarge = TRUE;
		bLeftLarge	=	FALSE;
	}
	else
	{
		bLeftLarge = TRUE;
		bRightLarge = FALSE;
	}

	diff >>= 1;

	if(bLeftLarge)
	{
		do
		{
			tree->root = tree->root->left;
			diff--;
		}while(diff);
	}

	if(bRightLarge)
	{
		do
		{
			tree->root->parent = tree->root->right;
			tree->root = tree->root->right;
			tree->root->parent = 0;
			diff--;
		}while(diff);
	}

}


void	insert_chunk(BinaryTree	*tree,MemoryChunk	*chunk)
{
	MemoryChunk	*node,*parent;
	uint32	insertion_depth;

	if(tree == 0 || chunk == 0)
	{
		return;
	}

	insertion_depth = 0;
	node = tree->root;
	parent = 0;


	while(node)
	{
		parent = node;
		if(GET_SIZE(chunk->size) > GET_SIZE(node->size))
		{
			node = node->right;
		}
		else
		{
			node = node->left;
		}

		insertion_depth++;
	}

	if(parent)
	{
		if(GET_SIZE(chunk->size) > GET_SIZE(parent->size))
		{
			parent->right = chunk;
		}
		else
		{
			parent->left = chunk;
		}

		chunk->left = 0;
		chunk->right = 0;
		chunk->parent = parent;

		tree->totalChunks ++;
	}
	else
	{	
		tree->root = chunk;
		chunk->left = 0;
		chunk->right = 0;
		chunk->parent = 0;
		tree->totalChunks = 1;
	}

	if(tree->totalChunks > 1)
	{
		if(GET_SIZE(chunk->size) > GET_SIZE(tree->root->size))
		{
			if(insertion_depth > tree->right_depth)
			{
				tree->right_depth = insertion_depth;
				tree->right_leaf_count = 1;
			}
			else if(insertion_depth == tree->right_depth)
			{
				tree->right_leaf_count++;
			}
		}
		else
		{
			if(insertion_depth > tree->left_depth)
			{
				tree->left_depth = insertion_depth;
				tree->left_leaf_count = 1;
			}
			else if(insertion_depth == tree->left_depth)
			{
				tree->left_leaf_count++;
			}
		}
	}

	balance_tree(tree);
}


void	remove_chunk(BinaryTree	*tree,MemoryChunk	*chunk)
{
	MemoryChunk	*left,*right,*parent,*newNode;
	boolean	bLeafNode;

	if(tree->root == 0 || tree->totalChunks == 0 || chunk == 0)
	{
		return;
	}

	/*remove the chunk and give priority to the right side*/

	left	=	chunk->left;
	right	=	chunk->right;
	parent	=	chunk->parent;
	newNode	=	0;

	if(!chunk->left && !chunk->right)
	{
		bLeafNode = TRUE;
	}
	else
	{
		bLeafNode = FALSE;
	}

	/*relocate left part (if possible)*/

	if(right == 0)
	{
		newNode = left;
	}
	else
	{
		newNode	=	right;
	}

	if(left && right)
	{
		MemoryChunk	*ptr,*prev;

		ptr = right;
		prev = 0;

		while(ptr)
		{
			prev = ptr;
			ptr = ptr->left;
		}

		prev->left = left;
		left->parent = prev;
	}

	if(parent)
	{
		if(GET_SIZE(chunk->size) > GET_SIZE(parent->size))
		{
			parent->right = newNode;
		}
		else
		{
			parent->left = newNode;
		}

		if(newNode)
		{
			newNode->parent = parent;
		}
	}
	else
	{
		tree->root = newNode;
		if(newNode)
		{
			newNode->parent = 0;
		}
	}

	tree->totalChunks--;

	if(bLeafNode && tree->root)
	{
		if(GET_SIZE(chunk->size) > GET_SIZE(tree->root->size))
		{
			tree->right_leaf_count--;
		}
		else
		{
			tree->right_leaf_count--;
		}
		balance_tree(tree);
	}
}


void	traverse_binary_tree(BinaryTree	*tree)
{
	MemoryChunk	*node;


	node = tree->root;

	while(node)
	{
		
	}
}


MemoryChunk*	get_closet_chunk(BinaryTree	*tree,uint32	size,tree_search_callback search_fn)
{
	MemoryChunk	*chunk;
	MemoryChunk	*node;

	/*the callback function gets called when search_fn returns TRUE*/
	
	if(tree == 0 || tree->root == 0 || search_fn == 0)
	{
		return 0;
	}

	chunk	=	0;

	node = tree->root;

	while(node)
	{
		if(GET_SIZE(node->size)>= size)
		{
			uint32	flg = CONTINUE_SEARCH;

			search_fn(node,&flg,size);

			chunk = node;

			if(flg == ABORT_SEARCH)
			{
				break;
			}
		}

		if(size < GET_SIZE(node->size))
		{
			node = node->left;
		}
		else
		{
			node = node->right;
		}
	}

	return	chunk;
}


MemoryChunk*	get_bigger_chunk(BinaryTree	*tree,uint32	size,tree_search_callback search_fn)
{
	MemoryChunk	*chunk;
	MemoryChunk	*node;
	MemoryChunk	*n;

	/*the callback function gets called when search_fn returns TRUE*/

	if(tree == 0 || tree->root == 0 || search_fn == 0)
	{
		return 0;
	}
	
	chunk	=	0;
	node = tree->root;

	while(node)
	{
		if(GET_SIZE(node->size)>= size)
		{
			uint32	flg = CONTINUE_SEARCH;

			search_fn(node,&flg,size);

			chunk = node;

			if(flg == ABORT_SEARCH)
			{
				break;
			}
		}

		n = node;
		node = node->right;

		if(node == 0)
		{
			node = n->left;
		}

		if(node == 0)
		{
			node = n->parent;
			if(node)
			{
				node = node->left;
			}

			if(node == n)
			{
				node = 0;
			}
		}

	}

	return	chunk;
}
