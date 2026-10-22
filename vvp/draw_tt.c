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
स्थिर शून्य draw_hex_table(शून्य)
{
      अचिन्हित idx;

      म_लिखो("extern const char hex_digits[256] = {\n");
      प्रति (idx = 0 ;  idx < 256 ;  idx += 1) {
	    अचिन्हित cnt_z = 0, cnt_x = 0;
	    अचिन्हित bv = 0, bdx;

	    प्रति (bdx = 0 ;  bdx < 4 ;  bdx += 1) {
		  विकल्प ((idx >> (bdx * 2)) & 3) {
		      पक्ष 0:
			भङ्ग;
		      पक्ष 1:
			bv |= 1<<bdx;
			भङ्ग;
		      पक्ष 2:
			cnt_x += 1;
			भङ्ग;
		      पक्ष 3:
			cnt_z += 1;
			भङ्ग;
		  }
	    }

	    यदि (cnt_z == 4)
		  म_लिखो(" 'z',");

	    अन्यथा यदि (cnt_x == 4)
		  म_लिखो(" 'x',");

	    अन्यथा यदि ((cnt_z > 0) && (cnt_x == 0))
		  म_लिखो(" 'Z',");

	    अन्यथा यदि (cnt_x > 0)
		  म_लिखो(" 'X',");

	    अन्यथा
		  म_लिखो(" '%c',", (अचिन्हित)"0123456789abcdef"[bv]);

	    यदि (((idx+1) % 8) == 0)
		  म_लिखो("\n");
      }
      म_लिखो("};\n");
}

स्थिर शून्य draw_oct_table(शून्य)
{
      अचिन्हित idx;

      म_लिखो("extern const char oct_digits[64] = {\n");
      प्रति (idx = 0 ;  idx < 64 ;  idx += 1) {
	    अचिन्हित cnt_z = 0, cnt_x = 0;
	    अचिन्हित bv = 0, bdx;

	    प्रति (bdx = 0 ;  bdx < 3 ;  bdx += 1) {
		  विकल्प ((idx >> (bdx * 2)) & 3) {
		      पक्ष 0:
			भङ्ग;
		      पक्ष 1:
			bv |= 1<<bdx;
			भङ्ग;
		      पक्ष 2:
			cnt_x += 1;
			भङ्ग;
		      पक्ष 3:
			cnt_z += 1;
			भङ्ग;
		  }
	    }

	    यदि (cnt_z == 3)
		  म_लिखो(" 'z',");

	    अन्यथा यदि (cnt_x == 3)
		  म_लिखो(" 'x',");

	    अन्यथा यदि ((cnt_z > 0) && (cnt_x == 0))
		  म_लिखो(" 'Z',");

	    अन्यथा यदि (cnt_x > 0)
		  म_लिखो(" 'X',");

	    अन्यथा
		  म_लिखो(" '%c',", (अचिन्हित)"01234567"[bv]);

	    यदि (((idx+1) % 8) == 0)
		  म_लिखो("\n");
      }
      म_लिखो("};\n");
}

पूर्णाङ्क main(शून्य)
{
      draw_hex_table();
      draw_oct_table();
      प्रत्यागम 0;
}
