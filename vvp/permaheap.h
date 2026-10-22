#که_نه_تعریف_شوی IVL_permaheap_H
#تعریف_کړه IVL_permaheap_H
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
#که_تعریف_شوی __SUNPRO_CC
# include  <stddef.h>
#وإلا
# include  <cstddef>
#د_که_پای

class permaheap {

    عام:
      explicit permaheap();
      ~permaheap();

      فراغ* alloc(size_t اندازه);

      size_t ډېرۍ_ټول() ثابت { أرجع ډېرۍ_ټول_2; }

    خاص:
      تعداد { INITIAL_CHUNK_اندازه = 512*1024, CHUNK_اندازه=256*1024 };

      اتحاد {
	    فراغ*align;
	    حرف بایټونه[INITIAL_CHUNK_اندازه];
      } initial_chunk_;

      حرف*chunk_ptr_;
      size_t chunk_پاتې_شوی;
      size_t ډېرۍ_ټول_2;
};

#د_که_پای /* IVL_permaheap_H */
