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
ساكن فراغ draw_hex_جدول(فراغ)
{
      بینښه idx;

      چاپ_کړه("extern const char hex_digits[256] = {\n");
      لكل (idx = 0 ;  idx < 256 ;  idx += 1) {
	    بینښه cnt_z = 0, cnt_x = 0;
	    بینښه bv = 0, bdx;

	    لكل (bdx = 0 ;  bdx < 4 ;  bdx += 1) {
		  بدل ((idx >> (bdx * 2)) & 3) {
		      حالة 0:
			اكسر;
		      حالة 1:
			bv |= 1<<bdx;
			اكسر;
		      حالة 2:
			cnt_x += 1;
			اكسر;
		      حالة 3:
			cnt_z += 1;
			اكسر;
		  }
	    }

	    إذا (cnt_z == 4)
		  چاپ_کړه(" 'z',");

	    وإلا إذا (cnt_x == 4)
		  چاپ_کړه(" 'x',");

	    وإلا إذا ((cnt_z > 0) && (cnt_x == 0))
		  چاپ_کړه(" 'Z',");

	    وإلا إذا (cnt_x > 0)
		  چاپ_کړه(" 'X',");

	    وإلا
		  چاپ_کړه(" '%c',", (بینښه)"0123456789abcdef"[bv]);

	    إذا (((idx+1) % 8) == 0)
		  چاپ_کړه("\n");
      }
      چاپ_کړه("};\n");
}

ساكن فراغ draw_oct_جدول(فراغ)
{
      بینښه idx;

      چاپ_کړه("extern const char oct_digits[64] = {\n");
      لكل (idx = 0 ;  idx < 64 ;  idx += 1) {
	    بینښه cnt_z = 0, cnt_x = 0;
	    بینښه bv = 0, bdx;

	    لكل (bdx = 0 ;  bdx < 3 ;  bdx += 1) {
		  بدل ((idx >> (bdx * 2)) & 3) {
		      حالة 0:
			اكسر;
		      حالة 1:
			bv |= 1<<bdx;
			اكسر;
		      حالة 2:
			cnt_x += 1;
			اكسر;
		      حالة 3:
			cnt_z += 1;
			اكسر;
		  }
	    }

	    إذا (cnt_z == 3)
		  چاپ_کړه(" 'z',");

	    وإلا إذا (cnt_x == 3)
		  چاپ_کړه(" 'x',");

	    وإلا إذا ((cnt_z > 0) && (cnt_x == 0))
		  چاپ_کړه(" 'Z',");

	    وإلا إذا (cnt_x > 0)
		  چاپ_کړه(" 'X',");

	    وإلا
		  چاپ_کړه(" '%c',", (بینښه)"01234567"[bv]);

	    إذا (((idx+1) % 8) == 0)
		  چاپ_کړه("\n");
      }
      چاپ_کړه("};\n");
}

صحيح اصلي(فراغ)
{
      draw_hex_جدول();
      draw_oct_جدول();
      أرجع 0;
}
