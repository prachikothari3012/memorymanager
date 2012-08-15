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


int	main()
{
	char	mem[1000];
	void *ptr1,*ptr2,*ptr3,*ptr4;

	MemoryPool	*pool;

	pool = create_memory_pool(mem,1000);

	ptr1 = allocate(pool,200);

	ptr2 = allocate(pool,210);

	ptr3	=	allocate(pool,220);

	ptr4	=	allocate(pool,320);


	deallocate(pool,ptr2);
	deallocate(pool,ptr3);


	return	0;
}
