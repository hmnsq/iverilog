/*
 * Copyright (c) 2001-2015 Stephen Williams (steve@icarus.com)
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

# include  <stdio.h>


/*
 * The hex_digits table is not a functor truth table per say, but a
 * map of a 4-vbit value to a hex digit. The table handles the display
 * of x, X, z, Z, etc.
 */
స్థిర శూన్య draw_hex_పట్టిక(శూన్య)
{
      గుర్తులేని idx;

      రాయి("extern const char hex_digits[256] = {\n");
      కొరకు (idx = 0 ;  idx < 256 ;  idx += 1) {
	    గుర్తులేని cnt_z = 0, cnt_x = 0;
	    గుర్తులేని bv = 0, bdx;

	    కొరకు (bdx = 0 ;  bdx < 4 ;  bdx += 1) {
		  ఎంపిక ((idx >> (bdx * 2)) & 3) {
		      స్థితి 0:
			ఆపు;
		      స్థితి 1:
			bv |= 1<<bdx;
			ఆపు;
		      స్థితి 2:
			cnt_x += 1;
			ఆపు;
		      స్థితి 3:
			cnt_z += 1;
			ఆపు;
		  }
	    }

	    ఒకవేళ (cnt_z == 4)
		  రాయి(" 'z',");

	    లేకపోతే ఒకవేళ (cnt_x == 4)
		  రాయి(" 'x',");

	    లేకపోతే ఒకవేళ ((cnt_z > 0) && (cnt_x == 0))
		  రాయి(" 'Z',");

	    లేకపోతే ఒకవేళ (cnt_x > 0)
		  రాయి(" 'X',");

	    లేకపోతే
		  రాయి(" '%c',", (గుర్తులేని)"0123456789abcdef"[bv]);

	    ఒకవేళ (((idx+1) % 8) == 0)
		  రాయి("\n");
      }
      రాయి("};\n");
}

స్థిర శూన్య draw_oct_పట్టిక(శూన్య)
{
      గుర్తులేని idx;

      రాయి("extern const char oct_digits[64] = {\n");
      కొరకు (idx = 0 ;  idx < 64 ;  idx += 1) {
	    గుర్తులేని cnt_z = 0, cnt_x = 0;
	    గుర్తులేని bv = 0, bdx;

	    కొరకు (bdx = 0 ;  bdx < 3 ;  bdx += 1) {
		  ఎంపిక ((idx >> (bdx * 2)) & 3) {
		      స్థితి 0:
			ఆపు;
		      స్థితి 1:
			bv |= 1<<bdx;
			ఆపు;
		      స్థితి 2:
			cnt_x += 1;
			ఆపు;
		      స్థితి 3:
			cnt_z += 1;
			ఆపు;
		  }
	    }

	    ఒకవేళ (cnt_z == 3)
		  రాయి(" 'z',");

	    లేకపోతే ఒకవేళ (cnt_x == 3)
		  రాయి(" 'x',");

	    లేకపోతే ఒకవేళ ((cnt_z > 0) && (cnt_x == 0))
		  రాయి(" 'Z',");

	    లేకపోతే ఒకవేళ (cnt_x > 0)
		  రాయి(" 'X',");

	    లేకపోతే
		  రాయి(" '%c',", (గుర్తులేని)"01234567"[bv]);

	    ఒకవేళ (((idx+1) % 8) == 0)
		  రాయి("\n");
      }
      రాయి("};\n");
}

పూర్ణాంకం ప్రధాన(శూన్య)
{
      draw_hex_పట్టిక();
      draw_oct_పట్టిక();
      తిరిగి 0;
}
