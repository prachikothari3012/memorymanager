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


#include	"MemoryAlloctor.h"


uint32	get_high_n_bytes_aligned_size(uint32 size)
{
	uint32 quo = size/N_BYTES_ALIGNMENT;
	uint32 rem = size - (quo*N_BYTES_ALIGNMENT);

	if(rem == 0)
	{
		return	size;
	}
	else
	{
		return	(size) + (N_BYTES_ALIGNMENT-rem);
	}
}

uint32	get_low_n_bytes_aligned_size(uint32 size)
{
	uint32 quo = size/N_BYTES_ALIGNMENT;
	uint32 rem = size - (quo*N_BYTES_ALIGNMENT);

	if(rem == 0)
	{
		return	size;
	}
	else
	{
		return	(quo*N_BYTES_ALIGNMENT);
	}
}

uint32	get_guard_bytes()
{
	uint32 g_bytes = GUARD_BYTES;

	if(g_bytes < 0)
	{
		g_bytes = 0;
	}

	g_bytes	=	get_high_n_bytes_aligned_size(g_bytes);

	return	g_bytes;
}

uint32	get_total_overhead_size()
{
	uint32 guard_bytes = get_guard_bytes();
						//the header size	//guard bytes on both ends		//the last 4 bytes to store chunk start address
	uint32	overhead = sizeof(MemoryChunk) + (guard_bytes*2)			+ sizeof(void*);

	return	overhead;
}

boolean	is_chunk_size_adequate(uint32 size)
{
	uint32 min_req_size = get_total_overhead_size();

	min_req_size += N_BYTES_ALIGNMENT;

	if(min_req_size > size)
	{
		return	FALSE;
	}
	else
	{
		return	TRUE;
	}
}

MemoryChunk*	init_memory_chunk(void *ptr,uint32 size)
{
	MemoryChunk	*chunk;

	if(ptr == 0 || size == 0)
	{
		return	0;
	}

	if(!is_chunk_size_adequate(size))
	{
		return	0;
	}

	chunk = (MemoryChunk*)ptr;
	chunk->flag = FREE_CHUNK;
	chunk->avl_flg = 0;
	chunk->size = size - get_total_overhead_size();

	*((uint32*)&(((uint8*)ptr)[size-sizeof(void*)]))	=	(uint32)ptr;

	return	chunk;
}


void	insert_footer(MemoryChunk	*chunk)
{
	*((uint32*)&(((uint8*)chunk)[chunk->size + get_total_overhead_size() - sizeof(void*)]))	=	(uint32)chunk;
}

boolean	split_chunk(MemoryChunk	*chunk,uint32	desired_size,MemoryChunk	**left,MemoryChunk	**right)
{
	void	*ptr;
	uint32	ognl_size;

	if(chunk == 0 || chunk->size < desired_size)
	{
		return	FALSE;
	}

	desired_size = get_high_n_bytes_aligned_size(desired_size);

	//now let us if splitting can actually generate another usable chunk or not
	if(!is_chunk_size_adequate(chunk->size - desired_size))
	{
		//no split
		*left = chunk;
		*right = 0;
		return	TRUE;
	}

	//split the chunk

	ognl_size = chunk->size;

	chunk->size = desired_size;

	insert_footer(chunk);

	*left = chunk;

	ptr = (void*)(&(((uint8*)chunk)[chunk->size + get_total_overhead_size()]));

	init_memory_chunk(ptr,ognl_size - desired_size);

	*right = (MemoryChunk*)ptr;

	(*right)->left = (*left)->left;
	(*right)->right = (*left)->right;
	(*right)->parent = (*left)->parent;

	insert_footer(*right);

	return	TRUE;
}

void*	get_user_memory(MemoryChunk	*chunk)
{
	uint8*	ptr = (uint8*)chunk;

	return	(void*)(ptr + sizeof(MemoryChunk) + get_guard_bytes());
}