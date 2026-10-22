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

வெளி மாறிலி எழுத்து hex_digits[256];

வெறுமை vpip_hex_str_to_vec4(vvp_vector4_t&val, மாறிலி எழுத்து*str)
{
      unsigned str_len = strlen(str);
      unsigned skip_chars = 0;
      மாறிலி எழுத்து*tstr = str;
	/* Find the number of non-numeric characters. */
      போது ((tstr = strpbrk(tstr, "-_"))) {
	    skip_chars += 1;
	    tstr += 1;
      }
      vvp_vector4_t tval(4*(str_len-skip_chars));
      skip_chars = 0;
      க்கு (unsigned idx = 0 ;  idx < tval.size() ;  idx += 1) {
	    unsigned tmp;
	    unsigned bit_off = idx%4;
	    unsigned str_off = idx/4;
	    எழுத்து ch;

	    ப்ரதிஜ்ஞா (str_off+skip_chars < str_len);
	      /* Skip any "_" characters in the string. */
	    போது((ch = str[str_len-str_off-1-skip_chars]) == '_') {
		  skip_chars += 1;
		  ப்ரதிஜ்ஞா (str_off+skip_chars < str_len);
	    }

	      /* If we find a "-" it must be at the head of the string. */
	    எனில் (ch == '-') ப்ரதிஜ்ஞா(0);

	    தேர்வு (ch) {
		நிலை '0':
		நிலை '1':
		நிலை '2':
		நிலை '3':
		நிலை '4':
		நிலை '5':
		நிலை '6':
		நிலை '7':
		நிலை '8':
		நிலை '9':
		  tmp = ch - '0';
		  tval.set_bit(idx, ((tmp>>bit_off)&1)? BIT4_1 : BIT4_0);
		  நிறுத்து;
		நிலை 'a':
		நிலை 'b':
		நிலை 'c':
		நிலை 'd':
		நிலை 'e':
		நிலை 'f':
		  tmp = ch - 'a' + 10;
		  tval.set_bit(idx, ((tmp>>bit_off)&1)? BIT4_1 : BIT4_0);
		  நிறுத்து;
		நிலை 'A':
		நிலை 'B':
		நிலை 'C':
		நிலை 'D':
		நிலை 'E':
		நிலை 'F':
		  tmp = ch - 'A' + 10;
		  tval.set_bit(idx, ((tmp>>bit_off)&1)? BIT4_1 : BIT4_0);
		  நிறுத்து;
		நிலை 'x':
		நிலை 'X':
		  tval.set_bit(idx, BIT4_X);
		  நிறுத்து;
		நிலை 'z':
		நிலை 'Z':
		  tval.set_bit(idx, BIT4_Z);
		  நிறுத்து;
		இயல்பு:
		    /* Return "x" if there are invalid digits in the string. */
		  fprintf(stderr, "Warning: Invalid hex digit %c(%d) in "
		          "\"%s\".\n", ch, ch, str);
		  க்கு (unsigned jdx = 0 ;  jdx < val.size() ;  jdx += 1) {
			val.set_bit(jdx, BIT4_X);
		  }
		  திரும்பு;
	    }
      }

	/* Make a negative value when needed. */
      எனில் (str[0] == '-') {
	    tval.invert();
	    tval += (int64_t) 1;
      }

	/* Find the correct padding value. */
      vvp_bit4_t pad;
      தேர்வு (tval.value(tval.size()-1)) {
	  நிலை BIT4_X:  // Pad MSB 'x' with 'x'.
	    pad = BIT4_X;
	    நிறுத்து;
	  நிலை BIT4_Z:  // Pad MSB 'z' with 'z'.
	    pad = BIT4_Z;
	    நிறுத்து;
	  நிலை BIT4_1:  // If negative pad MSB '1' with '1'.
	    எனில் (str[0] == '-') {
		  pad = BIT4_1;
		  நிறுத்து;
	    }
	    // fallthrough
	  இயல்பு:  // Everything else gets '0' padded.
	    pad = BIT4_0;
	    நிறுத்து;
      }

	/* Copy the temporary value to the real value, padding if needed. */
      க்கு (unsigned idx = 0 ;  idx < val.size() ;  idx += 1) {
	    எனில் (idx < tval.size()) val.set_bit(idx, tval.value(idx));
	    இல்லையெனில் val.set_bit(idx, pad);
      }
}

வெறுமை vpip_vec4_to_hex_str(மாறிலி vvp_vector4_t&bits, எழுத்து*buf, unsigned nbuf)
{
      unsigned slen = (bits.size() + 3) / 4;
      ப்ரதிஜ்ஞா(slen < nbuf);

      buf[slen] = 0;

      unsigned val = 0;
      க்கு (unsigned idx = 0 ;  idx < bits.size() ;  idx += 1) {
	    unsigned vs = (idx%4) * 2;

	    தேர்வு (bits.value(idx)) {
		நிலை BIT4_0:
		  நிறுத்து;
		நிலை BIT4_1:
		  val |= 1 << vs;
		  நிறுத்து;
		நிலை BIT4_X:
		  val |= 2 << vs;
		  நிறுத்து;
		நிலை BIT4_Z:
		  val |= 3 << vs;
	    }

	    எனில் (vs == 6) {
		  slen -= 1;
		  buf[slen] = hex_digits[val];
		  val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      தேர்வு (bits.size() % 4) {
	  நிலை 1:
	    எனில் (val == 2) val = 170;
	    இல்லையெனில் எனில் (val == 3) val = 255;
	    நிறுத்து;
	  நிலை 2:
	    எனில் (val == 10) val = 170;
	    இல்லையெனில் எனில் (val == 15) val = 255;
	    நிறுத்து;
	  நிலை 3:
	    எனில் (val == 42) val = 170;
	    இல்லையெனில் எனில் (val == 63) val = 255;
	    நிறுத்து;
      }

      எனில் (slen > 0) {
	    slen -= 1;
	    buf[slen] = hex_digits[val];
      }
}
