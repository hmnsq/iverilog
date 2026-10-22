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
      chunk_ptr_ = initial_chunk_.بائٹاں;
      chunk_بچیا = حجم(initial_chunk_);
      ڈھیر_کل_2 = chunk_بچیا;
}

permaheap::~permaheap()
{
}

فراغ* permaheap::alloc(size_t size)
{
      أكد(size <= CHUNK_ناپ);

      إذا (size > chunk_بچیا) {
	    chunk_ptr_ = ::جديد حرف[CHUNK_ناپ];
	    chunk_بچیا = CHUNK_ناپ;
	    ڈھیر_کل_2 += CHUNK_ناپ;
      }

      أكد( (size%حجم(فراغ*)) == 0 );

      فراغ*res = chunk_ptr_;
      chunk_ptr_ += size;
      chunk_بچیا -= size;

      أرجع res;
}
