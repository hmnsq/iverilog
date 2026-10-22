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

बाह्य ध्रुव वर्ण hex_digits[256];

शून्य vpip_hex_str_to_vec4(vvp_vector4_t&val, ध्रुव वर्ण*str)
{
      unsigned str_len = strlen(str);
      unsigned skip_chars = 0;
      ध्रुव वर्ण*tstr = str;
	/* Find the number of non-numeric characters. */
      जाव ((tstr = strpbrk(tstr, "-_"))) {
	    skip_chars += 1;
	    tstr += 1;
      }
      vvp_vector4_t tval(4*(str_len-skip_chars));
      skip_chars = 0;
      प्रति (unsigned idx = 0 ;  idx < tval.size() ;  idx += 1) {
	    unsigned tmp;
	    unsigned bit_off = idx%4;
	    unsigned str_off = idx/4;
	    वर्ण ch;

	    प्रतिज्ञा (str_off+skip_chars < str_len);
	      /* Skip any "_" characters in the string. */
	    जाव((ch = str[str_len-str_off-1-skip_chars]) == '_') {
		  skip_chars += 1;
		  प्रतिज्ञा (str_off+skip_chars < str_len);
	    }

	      /* If we find a "-" it must be at the head of the string. */
	    जइ (ch == '-') प्रतिज्ञा(0);

	    विकल्प (ch) {
		पक्ष '0':
		पक्ष '1':
		पक्ष '2':
		पक्ष '3':
		पक्ष '4':
		पक्ष '5':
		पक्ष '6':
		पक्ष '7':
		पक्ष '8':
		पक्ष '9':
		  tmp = ch - '0';
		  tval.set_bit(idx, ((tmp>>bit_off)&1)? BIT4_1 : BIT4_0);
		  भङ्ग;
		पक्ष 'a':
		पक्ष 'b':
		पक्ष 'c':
		पक्ष 'd':
		पक्ष 'e':
		पक्ष 'f':
		  tmp = ch - 'a' + 10;
		  tval.set_bit(idx, ((tmp>>bit_off)&1)? BIT4_1 : BIT4_0);
		  भङ्ग;
		पक्ष 'A':
		पक्ष 'B':
		पक्ष 'C':
		पक्ष 'D':
		पक्ष 'E':
		पक्ष 'F':
		  tmp = ch - 'A' + 10;
		  tval.set_bit(idx, ((tmp>>bit_off)&1)? BIT4_1 : BIT4_0);
		  भङ्ग;
		पक्ष 'x':
		पक्ष 'X':
		  tval.set_bit(idx, BIT4_X);
		  भङ्ग;
		पक्ष 'z':
		पक्ष 'Z':
		  tval.set_bit(idx, BIT4_Z);
		  भङ्ग;
		स्वभाव:
		    /* Return "x" if there are invalid digits in the string. */
		  fprintf(stderr, "Warning: Invalid hex digit %c(%d) in "
		          "\"%s\".\n", ch, ch, str);
		  प्रति (unsigned jdx = 0 ;  jdx < val.size() ;  jdx += 1) {
			val.set_bit(jdx, BIT4_X);
		  }
		  प्रत्यागम;
	    }
      }

	/* Make a negative value when needed. */
      जइ (str[0] == '-') {
	    tval.invert();
	    tval += (int64_t) 1;
      }

	/* Find the correct padding value. */
      vvp_bit4_t pad;
      विकल्प (tval.value(tval.size()-1)) {
	  पक्ष BIT4_X:  // Pad MSB 'x' with 'x'.
	    pad = BIT4_X;
	    भङ्ग;
	  पक्ष BIT4_Z:  // Pad MSB 'z' with 'z'.
	    pad = BIT4_Z;
	    भङ्ग;
	  पक्ष BIT4_1:  // If negative pad MSB '1' with '1'.
	    जइ (str[0] == '-') {
		  pad = BIT4_1;
		  भङ्ग;
	    }
	    // fallthrough
	  स्वभाव:  // Everything else gets '0' padded.
	    pad = BIT4_0;
	    भङ्ग;
      }

	/* Copy the temporary value to the real value, padding if needed. */
      प्रति (unsigned idx = 0 ;  idx < val.size() ;  idx += 1) {
	    जइ (idx < tval.size()) val.set_bit(idx, tval.value(idx));
	    अण्णहा val.set_bit(idx, pad);
      }
}

शून्य vpip_vec4_to_hex_str(ध्रुव vvp_vector4_t&bits, वर्ण*buf, unsigned nbuf)
{
      unsigned slen = (bits.size() + 3) / 4;
      प्रतिज्ञा(slen < nbuf);

      buf[slen] = 0;

      unsigned val = 0;
      प्रति (unsigned idx = 0 ;  idx < bits.size() ;  idx += 1) {
	    unsigned vs = (idx%4) * 2;

	    विकल्प (bits.value(idx)) {
		पक्ष BIT4_0:
		  भङ्ग;
		पक्ष BIT4_1:
		  val |= 1 << vs;
		  भङ्ग;
		पक्ष BIT4_X:
		  val |= 2 << vs;
		  भङ्ग;
		पक्ष BIT4_Z:
		  val |= 3 << vs;
	    }

	    जइ (vs == 6) {
		  slen -= 1;
		  buf[slen] = hex_digits[val];
		  val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      विकल्प (bits.size() % 4) {
	  पक्ष 1:
	    जइ (val == 2) val = 170;
	    अण्णहा जइ (val == 3) val = 255;
	    भङ्ग;
	  पक्ष 2:
	    जइ (val == 10) val = 170;
	    अण्णहा जइ (val == 15) val = 255;
	    भङ्ग;
	  पक्ष 3:
	    जइ (val == 42) val = 170;
	    अण्णहा जइ (val == 63) val = 255;
	    भङ्ग;
      }

      जइ (slen > 0) {
	    slen -= 1;
	    buf[slen] = hex_digits[val];
      }
}
