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

ବାହ୍ୟ ଅଚଳ ଅକ୍ଷର hex_digits[256];

ଶୂନ୍ୟ vpip_hex_str_to_vec4(vvp_vector4_t&val, ଅଚଳ ଅକ୍ଷର*str)
{
      unsigned str_len = strlen(str);
      unsigned skip_chars = 0;
      ଅଚଳ ଅକ୍ଷର*tstr = str;
	/* Find the number of non-numeric characters. */
      ଯେତେବେଳେ ((tstr = strpbrk(tstr, "-_"))) {
	    skip_chars += 1;
	    tstr += 1;
      }
      vvp_vector4_t tval(4*(str_len-skip_chars));
      skip_chars = 0;
      ପାଇଁ (unsigned idx = 0 ;  idx < tval.size() ;  idx += 1) {
	    unsigned tmp;
	    unsigned bit_off = idx%4;
	    unsigned str_off = idx/4;
	    ଅକ୍ଷର ch;

	    ପ୍ରତିଜ୍ଞା (str_off+skip_chars < str_len);
	      /* Skip any "_" characters in the string. */
	    ଯେତେବେଳେ((ch = str[str_len-str_off-1-skip_chars]) == '_') {
		  skip_chars += 1;
		  ପ୍ରତିଜ୍ଞା (str_off+skip_chars < str_len);
	    }

	      /* If we find a "-" it must be at the head of the string. */
	    ଯଦି (ch == '-') ପ୍ରତିଜ୍ଞା(0);

	    ବାଛ (ch) {
		ସ୍ଥିତି '0':
		ସ୍ଥିତି '1':
		ସ୍ଥିତି '2':
		ସ୍ଥିତି '3':
		ସ୍ଥିତି '4':
		ସ୍ଥିତି '5':
		ସ୍ଥିତି '6':
		ସ୍ଥିତି '7':
		ସ୍ଥିତି '8':
		ସ୍ଥିତି '9':
		  tmp = ch - '0';
		  tval.set_bit(idx, ((tmp>>bit_off)&1)? BIT4_1 : BIT4_0);
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି 'a':
		ସ୍ଥିତି 'b':
		ସ୍ଥିତି 'c':
		ସ୍ଥିତି 'd':
		ସ୍ଥିତି 'e':
		ସ୍ଥିତି 'f':
		  tmp = ch - 'a' + 10;
		  tval.set_bit(idx, ((tmp>>bit_off)&1)? BIT4_1 : BIT4_0);
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି 'A':
		ସ୍ଥିତି 'B':
		ସ୍ଥିତି 'C':
		ସ୍ଥିତି 'D':
		ସ୍ଥିତି 'E':
		ସ୍ଥିତି 'F':
		  tmp = ch - 'A' + 10;
		  tval.set_bit(idx, ((tmp>>bit_off)&1)? BIT4_1 : BIT4_0);
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି 'x':
		ସ୍ଥିତି 'X':
		  tval.set_bit(idx, BIT4_X);
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି 'z':
		ସ୍ଥିତି 'Z':
		  tval.set_bit(idx, BIT4_Z);
		  ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		    /* Return "x" if there are invalid digits in the string. */
		  fprintf(stderr, "Warning: Invalid hex digit %c(%d) in "
		          "\"%s\".\n", ch, ch, str);
		  ପାଇଁ (unsigned jdx = 0 ;  jdx < val.size() ;  jdx += 1) {
			val.set_bit(jdx, BIT4_X);
		  }
		  ଫେରନ୍ତୁ;
	    }
      }

	/* Make a negative value when needed. */
      ଯଦି (str[0] == '-') {
	    tval.invert();
	    tval += (int64_t) 1;
      }

	/* Find the correct padding value. */
      vvp_bit4_t pad;
      ବାଛ (tval.value(tval.size()-1)) {
	  ସ୍ଥିତି BIT4_X:  // Pad MSB 'x' with 'x'.
	    pad = BIT4_X;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି BIT4_Z:  // Pad MSB 'z' with 'z'.
	    pad = BIT4_Z;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି BIT4_1:  // If negative pad MSB '1' with '1'.
	    ଯଦି (str[0] == '-') {
		  pad = BIT4_1;
		  ଭାଙ୍ଗ;
	    }
	    // fallthrough
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:  // Everything else gets '0' padded.
	    pad = BIT4_0;
	    ଭାଙ୍ଗ;
      }

	/* Copy the temporary value to the real value, padding if needed. */
      ପାଇଁ (unsigned idx = 0 ;  idx < val.size() ;  idx += 1) {
	    ଯଦି (idx < tval.size()) val.set_bit(idx, tval.value(idx));
	    ଅନ୍ୟଥା val.set_bit(idx, pad);
      }
}

ଶୂନ୍ୟ vpip_vec4_to_hex_str(ଅଚଳ vvp_vector4_t&bits, ଅକ୍ଷର*buf, unsigned nbuf)
{
      unsigned slen = (bits.size() + 3) / 4;
      ପ୍ରତିଜ୍ଞା(slen < nbuf);

      buf[slen] = 0;

      unsigned val = 0;
      ପାଇଁ (unsigned idx = 0 ;  idx < bits.size() ;  idx += 1) {
	    unsigned vs = (idx%4) * 2;

	    ବାଛ (bits.value(idx)) {
		ସ୍ଥିତି BIT4_0:
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି BIT4_1:
		  val |= 1 << vs;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି BIT4_X:
		  val |= 2 << vs;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି BIT4_Z:
		  val |= 3 << vs;
	    }

	    ଯଦି (vs == 6) {
		  slen -= 1;
		  buf[slen] = hex_digits[val];
		  val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      ବାଛ (bits.size() % 4) {
	  ସ୍ଥିତି 1:
	    ଯଦି (val == 2) val = 170;
	    ଅନ୍ୟଥା ଯଦି (val == 3) val = 255;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 2:
	    ଯଦି (val == 10) val = 170;
	    ଅନ୍ୟଥା ଯଦି (val == 15) val = 255;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 3:
	    ଯଦି (val == 42) val = 170;
	    ଅନ୍ୟଥା ଯଦି (val == 63) val = 255;
	    ଭାଙ୍ଗ;
      }

      ଯଦି (slen > 0) {
	    slen -= 1;
	    buf[slen] = hex_digits[val];
      }
}
