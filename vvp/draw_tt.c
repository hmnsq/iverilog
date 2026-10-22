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
ಸ್ಥಿರ ಶೂನ್ಯ draw_hex_ಕೋಷ್ಟಕ(ಶೂನ್ಯ)
{
      ಚಿಹ್ನೆರಹಿತ idx;

      ಬರೆ("extern const char hex_digits[256] = {\n");
      ಗಾಗಿ (idx = 0 ;  idx < 256 ;  idx += 1) {
	    ಚಿಹ್ನೆರಹಿತ cnt_z = 0, cnt_x = 0;
	    ಚಿಹ್ನೆರಹಿತ bv = 0, bdx;

	    ಗಾಗಿ (bdx = 0 ;  bdx < 4 ;  bdx += 1) {
		  ಆಯ್ಕೆ ((idx >> (bdx * 2)) & 3) {
		      ಸ್ಥಿತಿ 0:
			ನಿಲ್ಲಿಸು;
		      ಸ್ಥಿತಿ 1:
			bv |= 1<<bdx;
			ನಿಲ್ಲಿಸು;
		      ಸ್ಥಿತಿ 2:
			cnt_x += 1;
			ನಿಲ್ಲಿಸು;
		      ಸ್ಥಿತಿ 3:
			cnt_z += 1;
			ನಿಲ್ಲಿಸು;
		  }
	    }

	    ಆದರೆ (cnt_z == 4)
		  ಬರೆ(" 'z',");

	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cnt_x == 4)
		  ಬರೆ(" 'x',");

	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((cnt_z > 0) && (cnt_x == 0))
		  ಬರೆ(" 'Z',");

	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cnt_x > 0)
		  ಬರೆ(" 'X',");

	    ಇಲ್ಲದಿದ್ದರೆ
		  ಬರೆ(" '%c',", (ಚಿಹ್ನೆರಹಿತ)"0123456789abcdef"[bv]);

	    ಆದರೆ (((idx+1) % 8) == 0)
		  ಬರೆ("\n");
      }
      ಬರೆ("};\n");
}

ಸ್ಥಿರ ಶೂನ್ಯ draw_oct_ಕೋಷ್ಟಕ(ಶೂನ್ಯ)
{
      ಚಿಹ್ನೆರಹಿತ idx;

      ಬರೆ("extern const char oct_digits[64] = {\n");
      ಗಾಗಿ (idx = 0 ;  idx < 64 ;  idx += 1) {
	    ಚಿಹ್ನೆರಹಿತ cnt_z = 0, cnt_x = 0;
	    ಚಿಹ್ನೆರಹಿತ bv = 0, bdx;

	    ಗಾಗಿ (bdx = 0 ;  bdx < 3 ;  bdx += 1) {
		  ಆಯ್ಕೆ ((idx >> (bdx * 2)) & 3) {
		      ಸ್ಥಿತಿ 0:
			ನಿಲ್ಲಿಸು;
		      ಸ್ಥಿತಿ 1:
			bv |= 1<<bdx;
			ನಿಲ್ಲಿಸು;
		      ಸ್ಥಿತಿ 2:
			cnt_x += 1;
			ನಿಲ್ಲಿಸು;
		      ಸ್ಥಿತಿ 3:
			cnt_z += 1;
			ನಿಲ್ಲಿಸು;
		  }
	    }

	    ಆದರೆ (cnt_z == 3)
		  ಬರೆ(" 'z',");

	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cnt_x == 3)
		  ಬರೆ(" 'x',");

	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((cnt_z > 0) && (cnt_x == 0))
		  ಬರೆ(" 'Z',");

	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cnt_x > 0)
		  ಬರೆ(" 'X',");

	    ಇಲ್ಲದಿದ್ದರೆ
		  ಬರೆ(" '%c',", (ಚಿಹ್ನೆರಹಿತ)"01234567"[bv]);

	    ಆದರೆ (((idx+1) % 8) == 0)
		  ಬರೆ("\n");
      }
      ಬರೆ("};\n");
}

ಪೂರ್ಣಾಂಕ ಮುಖ್ಯ(ಶೂನ್ಯ)
{
      draw_hex_ಕೋಷ್ಟಕ();
      draw_oct_ಕೋಷ್ಟಕ();
      ಹಿಂತಿರುಗಿ 0;
}
