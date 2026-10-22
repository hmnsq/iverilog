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

فراغ vpip_oct_str_to_vec4(vvp_vector4_t&val, ثابت حرف*str)
{
      unsigned str_len = strlen(str);
      unsigned skip_chars = 0;
      ثابت حرف*tstr = str;
	/* Find the number of non-numeric characters. */
      طالما ((tstr = strpbrk(tstr, "-_"))) {
	    skip_chars += 1;
	    tstr += 1;
      }
      vvp_vector4_t tval(3*(str_len-skip_chars));
      skip_chars = 0;
      لكل (unsigned idx = 0 ;  idx < tval.size() ;  idx += 1) {
	    unsigned tmp;
	    unsigned bit_off = idx%3;
	    unsigned str_off = idx/3;
	    حرف ch;

	    أكد(str_off+skip_chars < str_len);
	      /* Skip any "_" characters in the string. */
	    طالما ((ch = str[str_len-str_off-1-skip_chars]) == '_') {
		  skip_chars += 1;
		  أكد(str_off+skip_chars < str_len);
	    }

	      /* If we find a "-" it must be at the head of the string. */
	    𐤀𐤌 (ch == '-') أكد(0);

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
		  tval.set_bit(idx, ((tmp>>bit_off)&1)? BIT4_1 : BIT4_0);
		  اكسر;
		حالة 'x':
		حالة 'X':
		  tval.set_bit(idx, BIT4_X);
		  اكسر;
		حالة 'z':
		حالة 'Z':
		  tval.set_bit(idx, BIT4_Z);
		  اكسر;
		افتراضي:
		    /* Return "x" if there are invalid digits in the string. */
		  fprintf(stderr, "Warning: Invalid octal digit %c(%d) in "
		          "\"%s\".\n", ch, ch, str);
		  لكل (unsigned jdx = 0 ;  jdx < val.size() ;  jdx += 1) {
			val.set_bit(jdx, BIT4_X);
		  }
		  أرجع;
	    }
      }

	/* Make a negative value when needed. */
      𐤀𐤌 (str[0] == '-') {
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
	    𐤀𐤌 (str[0] == '-') {
		  pad = BIT4_1;
		  اكسر;
	    }
	    // fallthrough
	  افتراضي:  // Everything else gets '0' padded.
	    pad = BIT4_0;
	    اكسر;
      }

	/* Copy the temporary value to the real value, padding if needed. */
      لكل (unsigned idx = 0 ;  idx < val.size() ;  idx += 1) {
	    𐤀𐤌 (idx < tval.size()) val.set_bit(idx, tval.value(idx));
	    وإلا val.set_bit(idx, pad);
      }
}

فراغ vpip_vec4_to_oct_str(ثابت vvp_vector4_t&bits, حرف*buf, unsigned nbuf)
{
      unsigned slen = (bits.size() + 2) / 3;
      أكد(slen < nbuf);

      buf[slen] = 0;

      unsigned val = 0;
      لكل (unsigned idx = 0 ;  idx < bits.size() ;  idx += 1) {
	    unsigned vs = (idx%3) * 2;

	    بدل (bits.value(idx)) {
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

	    𐤀𐤌 (vs == 4) {
		  slen -= 1;
		  buf[slen] = oct_digits[val];
		  val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      بدل (bits.size() % 3) {
	  حالة 1:
	    𐤀𐤌 (val == 2) val = 42;
	    وإلا 𐤀𐤌 (val == 3) val = 63;
	    اكسر;
	  حالة 2:
	    𐤀𐤌 (val == 10) val = 42;
	    وإلا 𐤀𐤌 (val == 15) val = 63;
	    اكسر;
      }

      𐤀𐤌 (slen > 0) {
	    slen -= 1;
	    buf[slen] = oct_digits[val];
      }
}
