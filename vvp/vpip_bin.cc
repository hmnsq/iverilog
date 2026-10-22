/*
 * Copyright (c) 2002-2013 Stephen Williams (steve@icarus.com)
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

# include  "config.h"
# include  "vpi_priv.h"
# include  <cstdio>
# include  <cstring>
# include  <climits>
# include  <cstdlib>
# include  <cassert>

فراغ vpip_bin_str_منزل_vec4(vvp_vector4_t&vec4, ثابت حرف*buf)
{
      ثابت حرف*ebuf = buf + لڑی_لمبائی(buf);
      بنانشانی idx = 0;
      بنانشانی skip_chars = 0;
      ثابت حرف*tbuf = buf;
	/* Find the number of non-numeric characters. */
      طالما ((tbuf = strpbrk(tbuf, "-_"))) {
	    skip_chars += 1;
	    tbuf += 1;
      }
      vvp_vector4_t tval(لڑی_لمبائی(buf)-skip_chars);
      طالما (ebuf > buf) {
	    ebuf -= 1;
	      /* Skip any "_" characters in the string. */
	    طالما (*ebuf == '_') {
		  ebuf -= 1;
		  أكد(ebuf > buf);
	    }

	      /*  If we find a "-" it must be at the head of the string. */
	    إذا (*ebuf == '-') {
		  إذا (ebuf != buf) أكد(0);
		  اكسر;
	    }

	    أكد(idx < tval.size());
	    بدل (*ebuf) {
		حالة '0':
		  tval.سیٹ_ثنائی_ہندسہ(idx, BIT4_0);
		  اكسر;
		حالة '1':
		  tval.سیٹ_ثنائی_ہندسہ(idx, BIT4_1);
		  اكسر;
		حالة 'x':
		حالة 'X':
		  tval.سیٹ_ثنائی_ہندسہ(idx, BIT4_X);
		  اكسر;
		حالة 'z':
		حالة 'Z':
		  tval.سیٹ_ثنائی_ہندسہ(idx, BIT4_Z);
		  اكسر;
		افتراضي:
		    /* Return "x" if there are invalid digits in the string. */
		  fprintf(stderr, "Warning: Invalid binary digit %c(%d) in "
		          "\"%s\".\n", *ebuf, *ebuf, buf);
		  لكل (بنانشانی jdx = 0 ;  jdx < vec4.size() ;  jdx += 1) {
			vec4.سیٹ_ثنائی_ہندسہ(jdx, BIT4_X);
		  }
		  أرجع;
	    }

	    idx += 1;
      }

	/* Make a negative value when needed. */
      إذا (buf[0] == '-') {
	    tval.invert();
	    tval += (int64_t) 1;
      }

	/* Find the correct padding value. */
      vvp_bit4_t pad;
      بدل (tval.value(tval.size()-1)) {
	  حالة BIT4_X:  // Pad MSB 'x' with 'x'
	    pad = BIT4_X;
	    اكسر;
	  حالة BIT4_Z:  // Pad MSB 'z' with 'z'
	    pad = BIT4_Z;
	    اكسر;
	  حالة BIT4_1:  // If negative pad MSB '1' with '1'
	    إذا (buf[0] == '-') {
		  pad = BIT4_1;
		  اكسر;
	    }
	    // fallthrough
	  افتراضي:  // Everything else gets '0' padded/
	    pad = BIT4_0;
	    اكسر;
      }

      لكل (بنانشانی jdx = 0 ;  jdx < vec4.size() ; jdx += 1) {
	    إذا (jdx < tval.size()) vec4.سیٹ_ثنائی_ہندسہ(jdx, tval.value(jdx));
	    وإلا vec4.سیٹ_ثنائی_ہندسہ(jdx, pad);
      }
}
