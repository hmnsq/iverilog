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
      chunk_remaining_ = परिमाण(initial_chunk_);
      heap_total_ = chunk_remaining_;
}

permaheap::~permaheap()
{
}

शून्य* permaheap::alloc(size_t size)
{
      प्रतिज्ञा(size <= CHUNK_SIZE);

      यदि (size > chunk_remaining_) {
	    chunk_ptr_ = ::नूतन वर्ण[CHUNK_SIZE];
	    chunk_remaining_ = CHUNK_SIZE;
	    heap_total_ += CHUNK_SIZE;
      }

      प्रतिज्ञा( (size%परिमाण(शून्य*)) == 0 );

      शून्य*res = chunk_ptr_;
      chunk_ptr_ += size;
      chunk_remaining_ -= size;

      प्रत्यागम res;
}
