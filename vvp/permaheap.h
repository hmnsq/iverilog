#جے_بنا_تعریف IVL_permaheap_H
#تعریف_کرو IVL_permaheap_H
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
#جے_تعریف_شدہ __SUNPRO_CC
# include  <stddef.h>
#وإلا
# include  <cstddef>
#جے_انت

class permaheap {

    عام:
      explicit permaheap();
      ~permaheap();

      فراغ* alloc(size_t ناپ);

      size_t ڈھیر_کل() ثابت { أرجع ڈھیر_کل_2; }

    خاص:
      تعداد { INITIAL_CHUNK_ناپ = 512*1024, CHUNK_ناپ=256*1024 };

      اتحاد {
	    فراغ*align;
	    حرف بائٹاں[INITIAL_CHUNK_ناپ];
      } initial_chunk_;

      حرف*chunk_ptr_;
      size_t chunk_بچیا;
      size_t ڈھیر_کل_2;
};

#جے_انت /* IVL_permaheap_H */
