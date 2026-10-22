#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ IVL_permaheap_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IVL_permaheap_H
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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __SUNPRO_CC
# include  <stddef.h>
#ਨਹੀਂਤਾਂ
# include  <cstddef>
#ਜੇ_ਅੰਤ

class permaheap {

    ਜਨਤਕ:
      explicit permaheap();
      ~permaheap();

      ਖਾਲੀ* alloc(size_t ਮਾਪ);

      size_t ਢੇਰ_ਕੁੱਲ() ਅਚਲ { ਵਾਪਸ ਢੇਰ_ਕੁੱਲ_2; }

    ਨਿਜੀ:
      ਗਿਣਤੀ { INITIAL_CHUNK_ਮਾਪ = 512*1024, CHUNK_ਮਾਪ=256*1024 };

      ਸੰਘ {
	    ਖਾਲੀ*align;
	    ਅੱਖਰ ਬਾਈਟਾਂ[INITIAL_CHUNK_ਮਾਪ];
      } initial_chunk_;

      ਅੱਖਰ*chunk_ptr_;
      size_t chunk_ਬਚਿਆ;
      size_t ਢੇਰ_ਕੁੱਲ_2;
};

#ਜੇ_ਅੰਤ /* IVL_permaheap_H */
