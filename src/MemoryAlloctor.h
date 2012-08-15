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


#ifndef	MEMORY_ALLOCATOR_H_
#define	MEMORY_ALLOCATOR_H_

#include	"DataTypes.h"

// If N byte alignment is required then the minimum size to be allocated is N bytes


//		
//		____________________________________________________
//		|		|		|					|		|		|
//		|		|		|					|		|		|
//		|header	|Guard	|	size			|Guard	|Block	|
//		|		|Bytes	|	requested		|Bytes	|Start	|
//		|		|		|					|		|Address|
//		|_______|_______|___________________|_______|_______|
//
//


#define	GUARD_BYTES	20
#define	N_BYTES_ALIGNMENT	4


#define	FREE_CHUNK	0
#define	ALLOCATED_CHUNK	1


typedef	struct	_MemoryChunk
{
	uint32	size:30;
	uint32	flag:1;
	uint32	avl_flg:1;


	struct	_MemoryChunk	*left;
	struct	_MemoryChunk	*right;
	struct	_MemoryChunk	*parent;
}MemoryChunk;




//These functions are not the part of the API
MemoryChunk*	init_memory_chunk(void *ptr,uint32 size);
uint32	get_high_n_bytes_aligned_size(uint32 size);
uint32	get_low_n_bytes_aligned_size(uint32 size);
uint32	get_guard_bytes();
uint32	get_total_overhead_size();
boolean	is_chunk_size_adequate(uint32 size);
boolean	split_chunk(MemoryChunk	*chunk,uint32	desired_size,MemoryChunk	**left,MemoryChunk	**right);
void	insert_footer(MemoryChunk	*chunk);
uint32*	read_footer(MemoryChunk	*chunk);
void*	get_user_memory(MemoryChunk	*chunk);



#endif	//MEMORY_ALLOCATOR_H_
