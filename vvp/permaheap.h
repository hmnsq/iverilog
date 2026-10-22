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
#അല്ലെങ്കിൽ
# include  <cstddef>
#endif

class permaheap {

    പൊതു:
      explicit permaheap();
      ~permaheap();

      ശൂന്യം* alloc(size_t size);

      size_t heap_total() സ്ഥിരം { തിരികെ heap_total_; }

    സ്വകാര്യ:
      എണ്ണൽ { INITIAL_CHUNK_SIZE = 512*1024, CHUNK_SIZE=256*1024 };

      സംഘം {
	    ശൂന്യം*align;
	    അക്ഷരം bytes[INITIAL_CHUNK_SIZE];
      } initial_chunk_;

      അക്ഷരം*chunk_ptr_;
      size_t chunk_remaining_;
      size_t heap_total_;
};

#endif /* IVL_permaheap_H */
