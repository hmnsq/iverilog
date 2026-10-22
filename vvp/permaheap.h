#यदि_अपरिभाषितम् IVL_permaheap_H
#परिभाषय IVL_permaheap_H
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
#यदि_परिभाषितम् __SUNPRO_CC
# include  <stddef.h>
#अन्यथा
# include  <cstddef>
#यदि_अन्तः

class permaheap {

    सार्वजनिक:
      explicit permaheap();
      ~permaheap();

      शून्य* alloc(size_t आकार);

      size_t heap_योग() ध्रुव { प्रत्यागम heap_योग_2; }

    गुप्त:
      गणना { INITIAL_CHUNK_आकार = 512*1024, CHUNK_आकार=256*1024 };

      सङ्घ {
	    शून्य*align;
	    वर्ण अष्टकानि[INITIAL_CHUNK_आकार];
      } initial_chunk_;

      वर्ण*chunk_ptr_;
      size_t chunk_remaining_;
      size_t heap_योग_2;
};

#यदि_अन्तः /* IVL_permaheap_H */
