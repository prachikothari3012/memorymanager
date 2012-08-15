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


#include	"MemoryPool.h"



uint32	getRequiredPoolSize(uint32	size)
{
	size = get_high_n_bytes_aligned_size(size);
	return	(sizeof(MemoryPool) + size);
}


MemoryPool*	create_memory_pool(void	*ptr,uint32	size)
{
	MemoryPool	*pool;
	uint8	*chunk_start_ptr;
	MemoryChunk	*chunk;

	if(ptr == 0 || size == 0)
	{
		return	0;
	}

	if(size < sizeof(MemoryPool))
	{
		return	0;
	}

	if(!is_chunk_size_adequate(size - sizeof(MemoryPool)))
	{
		return	0;
	}

	pool = (MemoryPool*)ptr;

	pool->mem_pool = ptr;
	pool->mem_pool_size = size;

	init_binary_tree(&pool->freeChunkTree);
	init_binary_tree(&pool->allocatedChunkTree);

	chunk_start_ptr =	(uint8*)ptr;

	chunk_start_ptr += sizeof(MemoryPool);

	chunk	=	init_memory_chunk((void*)chunk_start_ptr,size-sizeof(MemoryPool));

	pool->largestChunk	=	chunk;

	insert_chunk(&pool->freeChunkTree,chunk);

	return	pool;
}


void	chunk_availability_notification(MemoryChunk	*chunk,uint32 *flg,uint32 desired_size)
{
	if(desired_size*2 <= chunk->size)
	{
		*flg = ABORT_SEARCH;
	}
	else
	{
		*flg = CONTINUE_SEARCH;
	}
}

MemoryChunk*	find_appropriate_chunk(MemoryPool*	pool,uint32 size)
{
	MemoryChunk	*chunk = get_bigger_chunk(&pool->freeChunkTree,size,chunk_availability_notification);
	
	return	chunk;
}


void*	allocate(MemoryPool	*pool,uint32 size)
{
	MemoryChunk*	chunk;
	MemoryChunk	*leftChunk,*rightChunk;

	if(pool == 0 || size == 0)
	{
		return	0;
	}

	chunk = find_appropriate_chunk(pool,size);

	remove_chunk(&pool->freeChunkTree,chunk);

	leftChunk = 0;
	rightChunk = 0;

	if(split_chunk(chunk,size,&leftChunk,&rightChunk))
	{
		leftChunk->flag = ALLOCATED_CHUNK;
		insert_chunk(&pool->allocatedChunkTree,leftChunk);

		if(rightChunk)
		{
			rightChunk->flag = FREE_CHUNK;
			insert_chunk(&pool->freeChunkTree,rightChunk);
		}

		return	get_user_memory(leftChunk);
	}
	else
	{
		return	0;
	}



}

void	deallocate(MemoryPool	*pool,void	*ptr)
{
	MemoryChunk *chunk = (MemoryChunk*)((uint8*)ptr - sizeof(MemoryChunk) - get_guard_bytes());

	remove_chunk(&pool->allocatedChunkTree,chunk);

	chunk = join_adjacent_free_chunks(pool,chunk);

	chunk->flag = FREE_CHUNK;

	insert_chunk(&pool->freeChunkTree,chunk);

}


MemoryChunk*	join_adjacent_free_chunks(MemoryPool*	pool,MemoryChunk *chunk)
{
	boolean	loop;
	MemoryChunk *adjacent_chunk;
	uint8	*ptr;

	if(chunk == 0 || pool == 0)
	{
		return 0;
	}

	//first join the left sided chunks
	

	for(loop = TRUE;loop==TRUE;)
	{
		if((void*)chunk <= (void*)pool->mem_pool)
		{
			loop = FALSE;
			continue;
		}

		adjacent_chunk = (MemoryChunk*)(*((uint32*)((uint8*)chunk - sizeof(void*))));

		if(adjacent_chunk->flag == ALLOCATED_CHUNK)
		{
			loop = FALSE;
			continue;
		}

		remove_chunk(&pool->freeChunkTree,adjacent_chunk);

		init_memory_chunk(adjacent_chunk,
						adjacent_chunk->size + chunk->size + (get_total_overhead_size()*2));

		chunk	=	adjacent_chunk;
	}


	//then join the right sided chunks
	for(loop = TRUE;loop==TRUE;)
	{
		if((uint32)((uint8*)chunk + get_total_overhead_size() + chunk->size) >= (uint32)((uint8*)pool->mem_pool + pool->mem_pool_size))
		{
			loop = FALSE;
			continue;
		}

		ptr = (uint8*)chunk;

		ptr += chunk->size;
		ptr += get_total_overhead_size();

		adjacent_chunk = (MemoryChunk*)ptr;
		
		if(adjacent_chunk->flag == ALLOCATED_CHUNK)
		{
			loop = FALSE;
			continue;
		}

		remove_chunk(&pool->freeChunkTree,adjacent_chunk);

		init_memory_chunk(chunk,
						adjacent_chunk->size + chunk->size + (get_total_overhead_size()*2));

	}

	return chunk;

}

