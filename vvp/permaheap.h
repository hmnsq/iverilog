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
#अन्यथा
# include  <cstddef>
#endif

class permaheap {

    सार्वजनिक:
      explicit permaheap();
      ~permaheap();

      शून्य* alloc(माप_प्रकार size);

      माप_प्रकार heap_total() स्थिरांक { परत heap_total_; }

    खाजगी:
      गणना { INITIAL_CHUNK_SIZE = 512*1024, CHUNK_SIZE=256*1024 };

      संघ {
	    शून्य*align;
	    अक्षर bytes[INITIAL_CHUNK_SIZE];
      } initial_chunk_;

      अक्षर*chunk_ptr_;
      माप_प्रकार chunk_remaining_;
      माप_प्रकार heap_total_;
};

#endif /* IVL_permaheap_H */
