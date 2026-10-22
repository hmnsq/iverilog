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
static void draw_hex_table(void)
{
      unsigned idx;

      printf("extern const char hex_digits[256] = {\n");
      for (idx = 0 ;  idx < 256 ;  idx += 1) {
	    unsigned cnt_z = 0, cnt_x = 0;
	    unsigned bv = 0, bdx;

	    for (bdx = 0 ;  bdx < 4 ;  bdx += 1) {
		  switch ((idx >> (bdx * 2)) & 3) {
		      pak_sa 0:
			bha_nga;
		      pak_sa 1:
			bv |= 1<<bdx;
			bha_nga;
		      pak_sa 2:
			cnt_x += 1;
			bha_nga;
		      pak_sa 3:
			cnt_z += 1;
			bha_nga;
		  }
	    }

	    ᬬᬾᬦ᭄ (cnt_z == 4)
		  printf(" 'z',");

	    ᬮᬶᬬ ᬬᬾᬦ᭄ (cnt_x == 4)
		  printf(" 'x',");

	    ᬮᬶᬬ ᬬᬾᬦ᭄ ((cnt_z > 0) && (cnt_x == 0))
		  printf(" 'Z',");

	    ᬮᬶᬬ ᬬᬾᬦ᭄ (cnt_x > 0)
		  printf(" 'X',");

	    ᬮᬶᬬ
		  printf(" '%c',", (unsigned)"0123456789abcdef"[bv]);

	    ᬬᬾᬦ᭄ (((idx+1) % 8) == 0)
		  printf("\n");
      }
      printf("};\n");
}

static void draw_oct_table(void)
{
      unsigned idx;

      printf("extern const char oct_digits[64] = {\n");
      for (idx = 0 ;  idx < 64 ;  idx += 1) {
	    unsigned cnt_z = 0, cnt_x = 0;
	    unsigned bv = 0, bdx;

	    for (bdx = 0 ;  bdx < 3 ;  bdx += 1) {
		  switch ((idx >> (bdx * 2)) & 3) {
		      pak_sa 0:
			bha_nga;
		      pak_sa 1:
			bv |= 1<<bdx;
			bha_nga;
		      pak_sa 2:
			cnt_x += 1;
			bha_nga;
		      pak_sa 3:
			cnt_z += 1;
			bha_nga;
		  }
	    }

	    ᬬᬾᬦ᭄ (cnt_z == 3)
		  printf(" 'z',");

	    ᬮᬶᬬ ᬬᬾᬦ᭄ (cnt_x == 3)
		  printf(" 'x',");

	    ᬮᬶᬬ ᬬᬾᬦ᭄ ((cnt_z > 0) && (cnt_x == 0))
		  printf(" 'Z',");

	    ᬮᬶᬬ ᬬᬾᬦ᭄ (cnt_x > 0)
		  printf(" 'X',");

	    ᬮᬶᬬ
		  printf(" '%c',", (unsigned)"01234567"[bv]);

	    ᬬᬾᬦ᭄ (((idx+1) % 8) == 0)
		  printf("\n");
      }
      printf("};\n");
}

puur_naa_nka main(void)
{
      draw_hex_table();
      draw_oct_table();
      ᬯᬮᬶ 0;
}
