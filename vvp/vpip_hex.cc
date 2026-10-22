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

بیرونی ثابت نویسه hex_digits[256];

تهی vpip_hex_str_to_vec4(vvp_vector4_t&val, ثابت نویسه*str)
{
      unsigned str_len = strlen(str);
      unsigned skip_chars = 0;
      ثابت نویسه*tstr = str;
	/* Find the number of non-numeric characters. */
      وقتی ((tstr = strpbrk(tstr, "-_"))) {
	    skip_chars += 1;
	    tstr += 1;
      }
      vvp_vector4_t tval(4*(str_len-skip_chars));
      skip_chars = 0;
      برای (unsigned idx = 0 ;  idx < tval.size() ;  idx += 1) {
	    unsigned tmp;
	    unsigned bit_off = idx%4;
	    unsigned str_off = idx/4;
	    نویسه ch;

	    أكد (str_off+skip_chars < str_len);
	      /* Skip any "_" characters in the string. */
	    وقتی((ch = str[str_len-str_off-1-skip_chars]) == '_') {
		  skip_chars += 1;
		  أكد (str_off+skip_chars < str_len);
	    }

	      /* If we find a "-" it must be at the head of the string. */
	    اگر (ch == '-') أكد(0);

	    انتخاب (ch) {
		حالت '0':
		حالت '1':
		حالت '2':
		حالت '3':
		حالت '4':
		حالت '5':
		حالت '6':
		حالت '7':
		حالت '8':
		حالت '9':
		  tmp = ch - '0';
		  tval.set_bit(idx, ((tmp>>bit_off)&1)? BIT4_1 : BIT4_0);
		  بشکن;
		حالت 'a':
		حالت 'b':
		حالت 'c':
		حالت 'd':
		حالت 'e':
		حالت 'f':
		  tmp = ch - 'a' + 10;
		  tval.set_bit(idx, ((tmp>>bit_off)&1)? BIT4_1 : BIT4_0);
		  بشکن;
		حالت 'A':
		حالت 'B':
		حالت 'C':
		حالت 'D':
		حالت 'E':
		حالت 'F':
		  tmp = ch - 'A' + 10;
		  tval.set_bit(idx, ((tmp>>bit_off)&1)? BIT4_1 : BIT4_0);
		  بشکن;
		حالت 'x':
		حالت 'X':
		  tval.set_bit(idx, BIT4_X);
		  بشکن;
		حالت 'z':
		حالت 'Z':
		  tval.set_bit(idx, BIT4_Z);
		  بشکن;
		پیشفرض:
		    /* Return "x" if there are invalid digits in the string. */
		  fprintf(stderr, "Warning: Invalid hex digit %c(%d) in "
		          "\"%s\".\n", ch, ch, str);
		  برای (unsigned jdx = 0 ;  jdx < val.size() ;  jdx += 1) {
			val.set_bit(jdx, BIT4_X);
		  }
		  بازگشت;
	    }
      }

	/* Make a negative value when needed. */
      اگر (str[0] == '-') {
	    tval.invert();
	    tval += (int64_t) 1;
      }

	/* Find the correct padding value. */
      vvp_bit4_t pad;
      انتخاب (tval.value(tval.size()-1)) {
	  حالت BIT4_X:  // Pad MSB 'x' with 'x'.
	    pad = BIT4_X;
	    بشکن;
	  حالت BIT4_Z:  // Pad MSB 'z' with 'z'.
	    pad = BIT4_Z;
	    بشکن;
	  حالت BIT4_1:  // If negative pad MSB '1' with '1'.
	    اگر (str[0] == '-') {
		  pad = BIT4_1;
		  بشکن;
	    }
	    // fallthrough
	  پیشفرض:  // Everything else gets '0' padded.
	    pad = BIT4_0;
	    بشکن;
      }

	/* Copy the temporary value to the real value, padding if needed. */
      برای (unsigned idx = 0 ;  idx < val.size() ;  idx += 1) {
	    اگر (idx < tval.size()) val.set_bit(idx, tval.value(idx));
	    وگرنه val.set_bit(idx, pad);
      }
}

تهی vpip_vec4_to_hex_str(ثابت vvp_vector4_t&bits, نویسه*buf, unsigned nbuf)
{
      unsigned slen = (bits.size() + 3) / 4;
      أكد(slen < nbuf);

      buf[slen] = 0;

      unsigned val = 0;
      برای (unsigned idx = 0 ;  idx < bits.size() ;  idx += 1) {
	    unsigned vs = (idx%4) * 2;

	    انتخاب (bits.value(idx)) {
		حالت BIT4_0:
		  بشکن;
		حالت BIT4_1:
		  val |= 1 << vs;
		  بشکن;
		حالت BIT4_X:
		  val |= 2 << vs;
		  بشکن;
		حالت BIT4_Z:
		  val |= 3 << vs;
	    }

	    اگر (vs == 6) {
		  slen -= 1;
		  buf[slen] = hex_digits[val];
		  val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      انتخاب (bits.size() % 4) {
	  حالت 1:
	    اگر (val == 2) val = 170;
	    وگرنه اگر (val == 3) val = 255;
	    بشکن;
	  حالت 2:
	    اگر (val == 10) val = 170;
	    وگرنه اگر (val == 15) val = 255;
	    بشکن;
	  حالت 3:
	    اگر (val == 42) val = 170;
	    وگرنه اگر (val == 63) val = 255;
	    بشکن;
      }

      اگر (slen > 0) {
	    slen -= 1;
	    buf[slen] = hex_digits[val];
      }
}
