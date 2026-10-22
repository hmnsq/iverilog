#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ IVL_permaheap_H
#ವ್ಯಾಖ್ಯಾನಿಸು IVL_permaheap_H
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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __SUNPRO_CC
# include  <stddef.h>
#ಇಲ್ಲದಿದ್ದರೆ
# include  <cstddef>
#ಆದರೆ_ಅಂತ್ಯ

class permaheap {

    ಸಾರ್ವಜನಿಕ:
      explicit permaheap();
      ~permaheap();

      ಶೂನ್ಯ* alloc(size_t ಗಾತ್ರ_2);

      size_t ರಾಶಿ_ಒಟ್ಟು() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ರಾಶಿ_ಒಟ್ಟು_2; }

    ಖಾಸಗಿ:
      ಎಣಿಕೆ { INITIAL_CHUNK_ಗಾತ್ರ = 512*1024, CHUNK_ಗಾತ್ರ=256*1024 };

      ಒಕ್ಕೂಟ {
	    ಶೂನ್ಯ*align;
	    ಅಕ್ಷರ ಬೈಟುಗಳು[INITIAL_CHUNK_ಗಾತ್ರ];
      } initial_chunk_;

      ಅಕ್ಷರ*chunk_ptr_;
      size_t chunk_ಉಳಿದ;
      size_t ರಾಶಿ_ಒಟ್ಟು_2;
};

#ಆದರೆ_ಅಂತ್ಯ /* IVL_permaheap_H */
