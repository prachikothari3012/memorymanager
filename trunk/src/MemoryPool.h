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

#ifndef	MEMORY_POOL_H_
#define	MEMORY_POOL_H_

#include	"BinaryTree.h"

typedef	struct	_MemoryPool
{
	void	*mem_pool;
	uint32	mem_pool_size;
	MemoryChunk	*largestChunk;
	BinaryTree	freeChunkTree;
	BinaryTree	allocatedChunkTree;
}MemoryPool;


MemoryPool*	create_memory_pool(void	*ptr,uint32	size);
void*	allocate(MemoryPool	*pool,uint32 size);
void	deallocate(MemoryPool	*pool,void	*ptr);

MemoryChunk*	join_adjacent_free_chunks(MemoryPool*	pool,MemoryChunk *chunk);

#endif	//MEMORY_POOL_H_
