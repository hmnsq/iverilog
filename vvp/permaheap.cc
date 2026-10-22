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
      chunk_ptr_ = initial_chunk_.బైట్లు;
      chunk_మిగిలిన = పరిమాణం(initial_chunk_);
      కుప్ప_మొత్తం_2 = chunk_మిగిలిన;
}

permaheap::~permaheap()
{
}

శూన్య* permaheap::alloc(size_t size)
{
      ప్రతిజ్ఞా(size <= CHUNK_కొలత);

      ఒకవేళ (size > chunk_మిగిలిన) {
	    chunk_ptr_ = ::కొత్త అక్షరం[CHUNK_కొలత];
	    chunk_మిగిలిన = CHUNK_కొలత;
	    కుప్ప_మొత్తం_2 += CHUNK_కొలత;
      }

      ప్రతిజ్ఞా( (size%పరిమాణం(శూన్య*)) == 0 );

      శూన్య*res = chunk_ptr_;
      chunk_ptr_ += size;
      chunk_మిగిలిన -= size;

      తిరిగి res;
}
