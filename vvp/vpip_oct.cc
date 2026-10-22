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

خارجي ثابت حرف oct_digits[64];

فراغ vpip_oct_str_منزل_vec4(vvp_vector4_t&val, ثابت حرف*str)
{
      بنانشانی str_len = لڑی_لمبائی(str);
      بنانشانی skip_chars = 0;
      ثابت حرف*tstr = str;
	/* Find the number of non-numeric characters. */
      طالما ((tstr = strpbrk(tstr, "-_"))) {
	    skip_chars += 1;
	    tstr += 1;
      }
      vvp_vector4_t tval(3*(str_len-skip_chars));
      skip_chars = 0;
      لكل (بنانشانی idx = 0 ;  idx < tval.size() ;  idx += 1) {
	    بنانشانی tmp;
	    بنانشانی ثنائی_ہندسہ_off = idx%3;
	    بنانشانی str_off = idx/3;
	    حرف ch;

	    أكد(str_off+skip_chars < str_len);
	      /* Skip any "_" characters in the string. */
	    طالما ((ch = str[str_len-str_off-1-skip_chars]) == '_') {
		  skip_chars += 1;
		  أكد(str_off+skip_chars < str_len);
	    }

	      /* If we find a "-" it must be at the head of the string. */
	    إذا (ch == '-') أكد(0);

	    بدل (ch) {
		حالة '0':
		حالة '1':
		حالة '2':
		حالة '3':
		حالة '4':
		حالة '5':
		حالة '6':
		حالة '7':
		  tmp = ch - '0';
		  tval.سیٹ_ثنائی_ہندسہ(idx, ((tmp>>ثنائی_ہندسہ_off)&1)? BIT4_1 : BIT4_0);
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
		  fprintf(stderr, "Warning: Invalid octal digit %c(%d) in "
		          "\"%s\".\n", ch, ch, str);
		  لكل (بنانشانی jdx = 0 ;  jdx < val.size() ;  jdx += 1) {
			val.سیٹ_ثنائی_ہندسہ(jdx, BIT4_X);
		  }
		  أرجع;
	    }
      }

	/* Make a negative value when needed. */
      إذا (str[0] == '-') {
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
	    إذا (str[0] == '-') {
		  pad = BIT4_1;
		  اكسر;
	    }
	    // fallthrough
	  افتراضي:  // Everything else gets '0' padded.
	    pad = BIT4_0;
	    اكسر;
      }

	/* Copy the temporary value to the real value, padding if needed. */
      لكل (بنانشانی idx = 0 ;  idx < val.size() ;  idx += 1) {
	    إذا (idx < tval.size()) val.سیٹ_ثنائی_ہندسہ(idx, tval.value(idx));
	    وإلا val.سیٹ_ثنائی_ہندسہ(idx, pad);
      }
}

فراغ vpip_vec4_منزل_oct_str(ثابت vvp_vector4_t&بٹ, حرف*buf, بنانشانی nbuf)
{
      بنانشانی slen = (بٹ.size() + 2) / 3;
      أكد(slen < nbuf);

      buf[slen] = 0;

      بنانشانی val = 0;
      لكل (بنانشانی idx = 0 ;  idx < بٹ.size() ;  idx += 1) {
	    بنانشانی vs = (idx%3) * 2;

	    بدل (بٹ.value(idx)) {
		حالة BIT4_0:
		  اكسر;
		حالة BIT4_1:
		  val |= 1 << vs;
		  اكسر;
		حالة BIT4_X:
		  val |= 2 << vs;
		  اكسر;
		حالة BIT4_Z:
		  val |= 3 << vs;
	    }

	    إذا (vs == 4) {
		  slen -= 1;
		  buf[slen] = oct_digits[val];
		  val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      بدل (بٹ.size() % 3) {
	  حالة 1:
	    إذا (val == 2) val = 42;
	    وإلا إذا (val == 3) val = 63;
	    اكسر;
	  حالة 2:
	    إذا (val == 10) val = 42;
	    وإلا إذا (val == 15) val = 63;
	    اكسر;
      }

      إذا (slen > 0) {
	    slen -= 1;
	    buf[slen] = oct_digits[val];
      }
}
