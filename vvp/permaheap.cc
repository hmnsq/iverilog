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
      chunk_ptr_ = initial_chunk_.ಬೈಟುಗಳು;
      chunk_ಉಳಿದ = ಗಾತ್ರ(initial_chunk_);
      ರಾಶಿ_ಒಟ್ಟು_2 = chunk_ಉಳಿದ;
}

permaheap::~permaheap()
{
}

ಶೂನ್ಯ* permaheap::alloc(size_t size)
{
      ಪ್ರತಿಜ್ಞಾ(size <= CHUNK_ಗಾತ್ರ);

      ಆದರೆ (size > chunk_ಉಳಿದ) {
	    chunk_ptr_ = ::ಹೊಸ ಅಕ್ಷರ[CHUNK_ಗಾತ್ರ];
	    chunk_ಉಳಿದ = CHUNK_ಗಾತ್ರ;
	    ರಾಶಿ_ಒಟ್ಟು_2 += CHUNK_ಗಾತ್ರ;
      }

      ಪ್ರತಿಜ್ಞಾ( (size%ಗಾತ್ರ(ಶೂನ್ಯ*)) == 0 );

      ಶೂನ್ಯ*res = chunk_ptr_;
      chunk_ptr_ += size;
      chunk_ಉಳಿದ -= size;

      ಹಿಂತಿರುಗಿ res;
}
