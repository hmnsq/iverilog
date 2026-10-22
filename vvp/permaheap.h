#اگر_غیر_تعریف_شدہ IVL_permaheap_H
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
#اگر_تعریف_شدہ __SUNPRO_CC
# include  <stddef.h>
#ورنہ
# include  <cstddef>
#اگر_ختم

class permaheap {

    عوامی:
      explicit permaheap();
      ~permaheap();

      خالی* alloc(size_t سائز);

      size_t ڈھیر_کل() ثابت { واپس ڈھیر_کل_2; }

    نجی:
      شمار { INITIAL_CHUNK_سائز = 512*1024, CHUNK_سائز=256*1024 };

      اتحاد {
	    خالی*align;
	    حرف بائٹیں[INITIAL_CHUNK_سائز];
      } initial_chunk_;

      حرف*chunk_ptr_;
      size_t chunk_بقایا;
      size_t ڈھیر_کل_2;
};

#اگر_ختم /* IVL_permaheap_H */
