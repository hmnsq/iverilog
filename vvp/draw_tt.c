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
நிலையான வெறுமை draw_hex_table(வெறுமை)
{
      unsigned idx;

      printf("extern const char hex_digits[256] = {\n");
      க்கு (idx = 0 ;  idx < 256 ;  idx += 1) {
	    unsigned cnt_z = 0, cnt_x = 0;
	    unsigned bv = 0, bdx;

	    க்கு (bdx = 0 ;  bdx < 4 ;  bdx += 1) {
		  தேர்வு ((idx >> (bdx * 2)) & 3) {
		      நிலை 0:
			நிறுத்து;
		      நிலை 1:
			bv |= 1<<bdx;
			நிறுத்து;
		      நிலை 2:
			cnt_x += 1;
			நிறுத்து;
		      நிலை 3:
			cnt_z += 1;
			நிறுத்து;
		  }
	    }

	    எனில் (cnt_z == 4)
		  printf(" 'z',");

	    இல்லையெனில் எனில் (cnt_x == 4)
		  printf(" 'x',");

	    இல்லையெனில் எனில் ((cnt_z > 0) && (cnt_x == 0))
		  printf(" 'Z',");

	    இல்லையெனில் எனில் (cnt_x > 0)
		  printf(" 'X',");

	    இல்லையெனில்
		  printf(" '%c',", (unsigned)"0123456789abcdef"[bv]);

	    எனில் (((idx+1) % 8) == 0)
		  printf("\n");
      }
      printf("};\n");
}

நிலையான வெறுமை draw_oct_table(வெறுமை)
{
      unsigned idx;

      printf("extern const char oct_digits[64] = {\n");
      க்கு (idx = 0 ;  idx < 64 ;  idx += 1) {
	    unsigned cnt_z = 0, cnt_x = 0;
	    unsigned bv = 0, bdx;

	    க்கு (bdx = 0 ;  bdx < 3 ;  bdx += 1) {
		  தேர்வு ((idx >> (bdx * 2)) & 3) {
		      நிலை 0:
			நிறுத்து;
		      நிலை 1:
			bv |= 1<<bdx;
			நிறுத்து;
		      நிலை 2:
			cnt_x += 1;
			நிறுத்து;
		      நிலை 3:
			cnt_z += 1;
			நிறுத்து;
		  }
	    }

	    எனில் (cnt_z == 3)
		  printf(" 'z',");

	    இல்லையெனில் எனில் (cnt_x == 3)
		  printf(" 'x',");

	    இல்லையெனில் எனில் ((cnt_z > 0) && (cnt_x == 0))
		  printf(" 'Z',");

	    இல்லையெனில் எனில் (cnt_x > 0)
		  printf(" 'X',");

	    இல்லையெனில்
		  printf(" '%c',", (unsigned)"01234567"[bv]);

	    எனில் (((idx+1) % 8) == 0)
		  printf("\n");
      }
      printf("};\n");
}

முழு முதன்மை(வெறுமை)
{
      draw_hex_table();
      draw_oct_table();
      திரும்பு 0;
}
