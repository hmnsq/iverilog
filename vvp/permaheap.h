#నిర్వచితం_కాకపోతే IVL_permaheap_H
#నిర్వచించు IVL_permaheap_H
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
#నిర్వచితమైతే __SUNPRO_CC
# include  <stddef.h>
#లేకపోతే
# include  <cstddef>
#అయితే_ముగింపు

class permaheap {

    బహిరంగ:
      explicit permaheap();
      ~permaheap();

      శూన్య* alloc(size_t కొలత);

      size_t కుప్ప_మొత్తం() స్థిరరాశి { తిరిగి కుప్ప_మొత్తం_2; }

    అంతరంగ:
      గణన { INITIAL_CHUNK_కొలత = 512*1024, CHUNK_కొలత=256*1024 };

      సంఘం {
	    శూన్య*align;
	    అక్షరం బైట్లు[INITIAL_CHUNK_కొలత];
      } initial_chunk_;

      అక్షరం*chunk_ptr_;
      size_t chunk_మిగిలిన;
      size_t కుప్ప_మొత్తం_2;
};

#అయితే_ముగింపు /* IVL_permaheap_H */
