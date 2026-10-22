/*
 * Copyright (c) 2002-2018 Stephen Williams (steve@icarus.com)
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
# include  <climits>     /* for CHAR_BIT */
# include  <cstdlib>
# include  <cassert>

خارجي ثابت حرف hex_digits[256];

فراغ vpip_hex_str_منزل_vec4(vvp_vector4_t&val, ثابت حرف*str)
{
      بینښه str_len = مزي_اوږدوالی(str);
      بینښه skip_chars = 0;
      ثابت حرف*tstr = str;
	/* Find the number of non-numeric characters. */
      طالما ((tstr = strpbrk(tstr, "-_"))) {
	    skip_chars += 1;
	    tstr += 1;
      }
      vvp_vector4_t tval(4*(str_len-skip_chars));
      skip_chars = 0;
      لكل (بینښه idx = 0 ;  idx < tval.size() ;  idx += 1) {
	    بینښه tmp;
	    بینښه بټ_off = idx%4;
	    بینښه str_off = idx/4;
	    حرف ch;

	    أكد (str_off+skip_chars < str_len);
	      /* Skip any "_" characters in the string. */
	    طالما((ch = str[str_len-str_off-1-skip_chars]) == '_') {
		  skip_chars += 1;
		  أكد (str_off+skip_chars < str_len);
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
		حالة '8':
		حالة '9':
		  tmp = ch - '0';
		  tval.سېټ_بټ(idx, ((tmp>>بټ_off)&1)? BIT4_1 : BIT4_0);
		  اكسر;
		حالة 'a':
		حالة 'b':
		حالة 'c':
		حالة 'd':
		حالة 'e':
		حالة 'f':
		  tmp = ch - 'a' + 10;
		  tval.سېټ_بټ(idx, ((tmp>>بټ_off)&1)? BIT4_1 : BIT4_0);
		  اكسر;
		حالة 'A':
		حالة 'B':
		حالة 'C':
		حالة 'D':
		حالة 'E':
		حالة 'F':
		  tmp = ch - 'A' + 10;
		  tval.سېټ_بټ(idx, ((tmp>>بټ_off)&1)? BIT4_1 : BIT4_0);
		  اكسر;
		حالة 'x':
		حالة 'X':
		  tval.سېټ_بټ(idx, BIT4_X);
		  اكسر;
		حالة 'z':
		حالة 'Z':
		  tval.سېټ_بټ(idx, BIT4_Z);
		  اكسر;
		افتراضي:
		    /* Return "x" if there are invalid digits in the string. */
		  fprintf(stderr, "Warning: Invalid hex digit %c(%d) in "
		          "\"%s\".\n", ch, ch, str);
		  لكل (بینښه jdx = 0 ;  jdx < val.size() ;  jdx += 1) {
			val.سېټ_بټ(jdx, BIT4_X);
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
	  حالة BIT4_X:  // Pad MSB 'x' with 'x'.
	    pad = BIT4_X;
	    اكسر;
	  حالة BIT4_Z:  // Pad MSB 'z' with 'z'.
	    pad = BIT4_Z;
	    اكسر;
	  حالة BIT4_1:  // If negative pad MSB '1' with '1'.
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
      لكل (بینښه idx = 0 ;  idx < val.size() ;  idx += 1) {
	    إذا (idx < tval.size()) val.سېټ_بټ(idx, tval.value(idx));
	    وإلا val.سېټ_بټ(idx, pad);
      }
}

فراغ vpip_vec4_منزل_hex_str(ثابت vvp_vector4_t&بټونه, حرف*buf, بینښه nbuf)
{
      بینښه slen = (بټونه.size() + 3) / 4;
      أكد(slen < nbuf);

      buf[slen] = 0;

      بینښه val = 0;
      لكل (بینښه idx = 0 ;  idx < بټونه.size() ;  idx += 1) {
	    بینښه vs = (idx%4) * 2;

	    بدل (بټونه.value(idx)) {
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

	    إذا (vs == 6) {
		  slen -= 1;
		  buf[slen] = hex_digits[val];
		  val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      بدل (بټونه.size() % 4) {
	  حالة 1:
	    إذا (val == 2) val = 170;
	    وإلا إذا (val == 3) val = 255;
	    اكسر;
	  حالة 2:
	    إذا (val == 10) val = 170;
	    وإلا إذا (val == 15) val = 255;
	    اكسر;
	  حالة 3:
	    إذا (val == 42) val = 170;
	    وإلا إذا (val == 63) val = 255;
	    اكسر;
      }

      إذا (slen > 0) {
	    slen -= 1;
	    buf[slen] = hex_digits[val];
      }
}
