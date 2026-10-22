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
ਸਥਿਰ ਖਾਲੀ draw_hex_ਸਾਰਣੀ(ਖਾਲੀ)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ idx;

      ਲਿਖੋ("extern const char hex_digits[256] = {\n");
      ਲਈ (idx = 0 ;  idx < 256 ;  idx += 1) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ cnt_z = 0, cnt_x = 0;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ bv = 0, bdx;

	    ਲਈ (bdx = 0 ;  bdx < 4 ;  bdx += 1) {
		  ਚੋਣ ((idx >> (bdx * 2)) & 3) {
		      ਹਾਲਤ 0:
			ਰੋਕੋ;
		      ਹਾਲਤ 1:
			bv |= 1<<bdx;
			ਰੋਕੋ;
		      ਹਾਲਤ 2:
			cnt_x += 1;
			ਰੋਕੋ;
		      ਹਾਲਤ 3:
			cnt_z += 1;
			ਰੋਕੋ;
		  }
	    }

	    ਜੇ (cnt_z == 4)
		  ਲਿਖੋ(" 'z',");

	    ਨਹੀਂਤਾਂ ਜੇ (cnt_x == 4)
		  ਲਿਖੋ(" 'x',");

	    ਨਹੀਂਤਾਂ ਜੇ ((cnt_z > 0) && (cnt_x == 0))
		  ਲਿਖੋ(" 'Z',");

	    ਨਹੀਂਤਾਂ ਜੇ (cnt_x > 0)
		  ਲਿਖੋ(" 'X',");

	    ਨਹੀਂਤਾਂ
		  ਲਿਖੋ(" '%c',", (ਬਿਨਾਨਿਸ਼ਾਨੀ)"0123456789abcdef"[bv]);

	    ਜੇ (((idx+1) % 8) == 0)
		  ਲਿਖੋ("\n");
      }
      ਲਿਖੋ("};\n");
}

ਸਥਿਰ ਖਾਲੀ draw_oct_ਸਾਰਣੀ(ਖਾਲੀ)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ idx;

      ਲਿਖੋ("extern const char oct_digits[64] = {\n");
      ਲਈ (idx = 0 ;  idx < 64 ;  idx += 1) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ cnt_z = 0, cnt_x = 0;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ bv = 0, bdx;

	    ਲਈ (bdx = 0 ;  bdx < 3 ;  bdx += 1) {
		  ਚੋਣ ((idx >> (bdx * 2)) & 3) {
		      ਹਾਲਤ 0:
			ਰੋਕੋ;
		      ਹਾਲਤ 1:
			bv |= 1<<bdx;
			ਰੋਕੋ;
		      ਹਾਲਤ 2:
			cnt_x += 1;
			ਰੋਕੋ;
		      ਹਾਲਤ 3:
			cnt_z += 1;
			ਰੋਕੋ;
		  }
	    }

	    ਜੇ (cnt_z == 3)
		  ਲਿਖੋ(" 'z',");

	    ਨਹੀਂਤਾਂ ਜੇ (cnt_x == 3)
		  ਲਿਖੋ(" 'x',");

	    ਨਹੀਂਤਾਂ ਜੇ ((cnt_z > 0) && (cnt_x == 0))
		  ਲਿਖੋ(" 'Z',");

	    ਨਹੀਂਤਾਂ ਜੇ (cnt_x > 0)
		  ਲਿਖੋ(" 'X',");

	    ਨਹੀਂਤਾਂ
		  ਲਿਖੋ(" '%c',", (ਬਿਨਾਨਿਸ਼ਾਨੀ)"01234567"[bv]);

	    ਜੇ (((idx+1) % 8) == 0)
		  ਲਿਖੋ("\n");
      }
      ਲਿਖੋ("};\n");
}

ਪੂਰਨ ਮੁੱਖ(ਖਾਲੀ)
{
      draw_hex_ਸਾਰਣੀ();
      draw_oct_ਸਾਰਣੀ();
      ਵਾਪਸ 0;
}
