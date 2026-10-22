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
      chunk_remaining_ = আকার(initial_chunk_);
      heap_যোগ_2 = chunk_remaining_;
}

permaheap::~permaheap()
{
}

শূন্য* permaheap::alloc(size_t size)
{
      প্রতিজ্ঞা(size <= CHUNK_আকার);

      যদি (size > chunk_remaining_) {
	    chunk_ptr_ = ::নতুন অক্ষর[CHUNK_আকার];
	    chunk_remaining_ = CHUNK_আকার;
	    heap_যোগ_2 += CHUNK_আকার;
      }

      প্রতিজ্ঞা( (size%আকার(শূন্য*)) == 0 );

      শূন্য*res = chunk_ptr_;
      chunk_ptr_ += size;
      chunk_remaining_ -= size;

      ফেরত res;
}
