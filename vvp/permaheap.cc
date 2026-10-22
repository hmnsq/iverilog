/*
 * Copyright (c) 2009-2010 Stephen Williams (steve@icarus.com)
 *
 *    This source code is free software; you can redistribute it
 *    and/or modify it in source code form under the terms of the GNU
 *    General Public License as published by the Free Software
 *    Foundation; either version 2 of the License, or (at your option)
 *    any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

# include  "permaheap.h"
# include  <cassert>

permaheap::permaheap()
{
      chunk_ptr_ = initial_chunk_.bytes;
      chunk_remaining_ = माप(initial_chunk_);
      heap_योग_2 = chunk_remaining_;
}

permaheap::~permaheap()
{
}

व्योम* permaheap::alloc(माप_प्रकार size)
{
      निश्चित(size <= CHUNK_आकार);

      अगर (size > chunk_remaining_) {
	    chunk_ptr_ = ::नया अक्षर[CHUNK_आकार];
	    chunk_remaining_ = CHUNK_आकार;
	    heap_योग_2 += CHUNK_आकार;
      }

      निश्चित( (size%माप(व्योम*)) == 0 );

      व्योम*res = chunk_ptr_;
      chunk_ptr_ += size;
      chunk_remaining_ -= size;

      वापस res;
}
