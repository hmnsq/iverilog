#ifndef IVL_permaheap_H
#define IVL_permaheap_H
/*
 * Copyright (c) 2009-2014 Stephen Williams (steve@icarus.com)
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

// The SunPro C++ compiler is broken and does not define size_t in cstddef.
#ifdef __SUNPRO_CC
# include  <stddef.h>
#وگرنه
# include  <cstddef>
#endif

class permaheap {

    عمومی:
      explicit permaheap();
      ~permaheap();

      تهی* alloc(size_t size);

      size_t heap_total() ثابت { بازگشت heap_total_; }

    خصوصی:
      شمارش { INITIAL_CHUNK_SIZE = 512*1024, CHUNK_SIZE=256*1024 };

      اجتماع {
	    تهی*align;
	    نویسه bytes[INITIAL_CHUNK_SIZE];
      } initial_chunk_;

      نویسه*chunk_ptr_;
      size_t chunk_remaining_;
      size_t heap_total_;
};

#endif /* IVL_permaheap_H */
