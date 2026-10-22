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

বাহ্য ধ্রুৱ ৱর্ণ hex_digits[256];

শূন্য vpip_hex_str_to_vec4(vvp_vector4_t&val, ধ্রুৱ ৱর্ণ*str)
{
      unsigned str_len = strlen(str);
      unsigned skip_chars = 0;
      ধ্রুৱ ৱর্ণ*tstr = str;
	/* Find the number of non-numeric characters. */
      যেতিয়ালৈ ((tstr = strpbrk(tstr, "-_"))) {
	    skip_chars += 1;
	    tstr += 1;
      }
      vvp_vector4_t tval(4*(str_len-skip_chars));
      skip_chars = 0;
      বাবে (unsigned idx = 0 ;  idx < tval.size() ;  idx += 1) {
	    unsigned tmp;
	    unsigned bit_off = idx%4;
	    unsigned str_off = idx/4;
	    ৱর্ণ ch;

	    প্রতিজ্ঞা (str_off+skip_chars < str_len);
	      /* Skip any "_" characters in the string. */
	    যেতিয়ালৈ((ch = str[str_len-str_off-1-skip_chars]) == '_') {
		  skip_chars += 1;
		  প্রতিজ্ঞা (str_off+skip_chars < str_len);
	    }

	      /* If we find a "-" it must be at the head of the string. */
	    যদি (ch == '-') প্রতিজ্ঞা(0);

	    ৱিকল্প (ch) {
		পক্ষ '0':
		পক্ষ '1':
		পক্ষ '2':
		পক্ষ '3':
		পক্ষ '4':
		পক্ষ '5':
		পক্ষ '6':
		পক্ষ '7':
		পক্ষ '8':
		পক্ষ '9':
		  tmp = ch - '0';
		  tval.set_bit(idx, ((tmp>>bit_off)&1)? BIT4_1 : BIT4_0);
		  ভঙ্গ;
		পক্ষ 'a':
		পক্ষ 'b':
		পক্ষ 'c':
		পক্ষ 'd':
		পক্ষ 'e':
		পক্ষ 'f':
		  tmp = ch - 'a' + 10;
		  tval.set_bit(idx, ((tmp>>bit_off)&1)? BIT4_1 : BIT4_0);
		  ভঙ্গ;
		পক্ষ 'A':
		পক্ষ 'B':
		পক্ষ 'C':
		পক্ষ 'D':
		পক্ষ 'E':
		পক্ষ 'F':
		  tmp = ch - 'A' + 10;
		  tval.set_bit(idx, ((tmp>>bit_off)&1)? BIT4_1 : BIT4_0);
		  ভঙ্গ;
		পক্ষ 'x':
		পক্ষ 'X':
		  tval.set_bit(idx, BIT4_X);
		  ভঙ্গ;
		পক্ষ 'z':
		পক্ষ 'Z':
		  tval.set_bit(idx, BIT4_Z);
		  ভঙ্গ;
		স্ৱভাৱ:
		    /* Return "x" if there are invalid digits in the string. */
		  fprintf(stderr, "Warning: Invalid hex digit %c(%d) in "
		          "\"%s\".\n", ch, ch, str);
		  বাবে (unsigned jdx = 0 ;  jdx < val.size() ;  jdx += 1) {
			val.set_bit(jdx, BIT4_X);
		  }
		  ঘূৰাই;
	    }
      }

	/* Make a negative value when needed. */
      যদি (str[0] == '-') {
	    tval.invert();
	    tval += (int64_t) 1;
      }

	/* Find the correct padding value. */
      vvp_bit4_t pad;
      ৱিকল্প (tval.value(tval.size()-1)) {
	  পক্ষ BIT4_X:  // Pad MSB 'x' with 'x'.
	    pad = BIT4_X;
	    ভঙ্গ;
	  পক্ষ BIT4_Z:  // Pad MSB 'z' with 'z'.
	    pad = BIT4_Z;
	    ভঙ্গ;
	  পক্ষ BIT4_1:  // If negative pad MSB '1' with '1'.
	    যদি (str[0] == '-') {
		  pad = BIT4_1;
		  ভঙ্গ;
	    }
	    // fallthrough
	  স্ৱভাৱ:  // Everything else gets '0' padded.
	    pad = BIT4_0;
	    ভঙ্গ;
      }

	/* Copy the temporary value to the real value, padding if needed. */
      বাবে (unsigned idx = 0 ;  idx < val.size() ;  idx += 1) {
	    যদি (idx < tval.size()) val.set_bit(idx, tval.value(idx));
	    অন্যথা val.set_bit(idx, pad);
      }
}

শূন্য vpip_vec4_to_hex_str(ধ্রুৱ vvp_vector4_t&bits, ৱর্ণ*buf, unsigned nbuf)
{
      unsigned slen = (bits.size() + 3) / 4;
      প্রতিজ্ঞা(slen < nbuf);

      buf[slen] = 0;

      unsigned val = 0;
      বাবে (unsigned idx = 0 ;  idx < bits.size() ;  idx += 1) {
	    unsigned vs = (idx%4) * 2;

	    ৱিকল্প (bits.value(idx)) {
		পক্ষ BIT4_0:
		  ভঙ্গ;
		পক্ষ BIT4_1:
		  val |= 1 << vs;
		  ভঙ্গ;
		পক্ষ BIT4_X:
		  val |= 2 << vs;
		  ভঙ্গ;
		পক্ষ BIT4_Z:
		  val |= 3 << vs;
	    }

	    যদি (vs == 6) {
		  slen -= 1;
		  buf[slen] = hex_digits[val];
		  val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      ৱিকল্প (bits.size() % 4) {
	  পক্ষ 1:
	    যদি (val == 2) val = 170;
	    অন্যথা যদি (val == 3) val = 255;
	    ভঙ্গ;
	  পক্ষ 2:
	    যদি (val == 10) val = 170;
	    অন্যথা যদি (val == 15) val = 255;
	    ভঙ্গ;
	  পক্ষ 3:
	    যদি (val == 42) val = 170;
	    অন্যথা যদি (val == 63) val = 255;
	    ভঙ্গ;
      }

      যদি (slen > 0) {
	    slen -= 1;
	    buf[slen] = hex_digits[val];
      }
}
