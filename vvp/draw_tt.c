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
ساکن خالی draw_hex_جدول(خالی)
{
      غیرعلامتی idx;

      لکھو("extern const char hex_digits[256] = {\n");
      برائے (idx = 0 ;  idx < 256 ;  idx += 1) {
	    غیرعلامتی cnt_z = 0, cnt_x = 0;
	    غیرعلامتی bv = 0, bdx;

	    برائے (bdx = 0 ;  bdx < 4 ;  bdx += 1) {
		  انتخاب ((idx >> (bdx * 2)) & 3) {
		      حالت 0:
			توڑو;
		      حالت 1:
			bv |= 1<<bdx;
			توڑو;
		      حالت 2:
			cnt_x += 1;
			توڑو;
		      حالت 3:
			cnt_z += 1;
			توڑو;
		  }
	    }

	    اگر (cnt_z == 4)
		  لکھو(" 'z',");

	    ورنہ اگر (cnt_x == 4)
		  لکھو(" 'x',");

	    ورنہ اگر ((cnt_z > 0) && (cnt_x == 0))
		  لکھو(" 'Z',");

	    ورنہ اگر (cnt_x > 0)
		  لکھو(" 'X',");

	    ورنہ
		  لکھو(" '%c',", (غیرعلامتی)"0123456789abcdef"[bv]);

	    اگر (((idx+1) % 8) == 0)
		  لکھو("\n");
      }
      لکھو("};\n");
}

ساکن خالی draw_oct_جدول(خالی)
{
      غیرعلامتی idx;

      لکھو("extern const char oct_digits[64] = {\n");
      برائے (idx = 0 ;  idx < 64 ;  idx += 1) {
	    غیرعلامتی cnt_z = 0, cnt_x = 0;
	    غیرعلامتی bv = 0, bdx;

	    برائے (bdx = 0 ;  bdx < 3 ;  bdx += 1) {
		  انتخاب ((idx >> (bdx * 2)) & 3) {
		      حالت 0:
			توڑو;
		      حالت 1:
			bv |= 1<<bdx;
			توڑو;
		      حالت 2:
			cnt_x += 1;
			توڑو;
		      حالت 3:
			cnt_z += 1;
			توڑو;
		  }
	    }

	    اگر (cnt_z == 3)
		  لکھو(" 'z',");

	    ورنہ اگر (cnt_x == 3)
		  لکھو(" 'x',");

	    ورنہ اگر ((cnt_z > 0) && (cnt_x == 0))
		  لکھو(" 'Z',");

	    ورنہ اگر (cnt_x > 0)
		  لکھو(" 'X',");

	    ورنہ
		  لکھو(" '%c',", (غیرعلامتی)"01234567"[bv]);

	    اگر (((idx+1) % 8) == 0)
		  لکھو("\n");
      }
      لکھو("};\n");
}

صحیح مین(خالی)
{
      draw_hex_جدول();
      draw_oct_جدول();
      واپس 0;
}
