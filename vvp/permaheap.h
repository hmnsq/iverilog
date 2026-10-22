#अगर_अघोषित IVL_permaheap_H
#घोषणा IVL_permaheap_H
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
#अगर_घोषित __SUNPRO_CC
# include  <stddef.h>
#अन्यथा
# include  <cstddef>
#पूर्ण_अगर

class permaheap {

    खुला:
      explicit permaheap();
      ~permaheap();

      व्योम* alloc(माप_प्रकार आकार);

      माप_प्रकार heap_योग() स्थिर { वापस heap_योग_2; }

    निजी:
      क्रमागत { INITIAL_CHUNK_आकार = 512*1024, CHUNK_आकार=256*1024 };

      जोड़ {
	    व्योम*align;
	    अक्षर bytes[INITIAL_CHUNK_आकार];
      } initial_chunk_;

      अक्षर*chunk_ptr_;
      माप_प्रकार chunk_remaining_;
      माप_प्रकार heap_योग_2;
};

#पूर्ण_अगर /* IVL_permaheap_H */
