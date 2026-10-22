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

ಬಾಹ್ಯ ಧ್ರುವ ವರ್ಣ hex_digits[256];

ಶೂನ್ಯ vpip_hex_str_to_vec4(vvp_vector4_t&val, ಧ್ರುವ ವರ್ಣ*str)
{
      unsigned str_len = strlen(str);
      unsigned skip_chars = 0;
      ಧ್ರುವ ವರ್ಣ*tstr = str;
	/* Find the number of non-numeric characters. */
      ಯಾವತ್ ((tstr = strpbrk(tstr, "-_"))) {
	    skip_chars += 1;
	    tstr += 1;
      }
      vvp_vector4_t tval(4*(str_len-skip_chars));
      skip_chars = 0;
      ಪ್ರತಿ (unsigned idx = 0 ;  idx < tval.size() ;  idx += 1) {
	    unsigned tmp;
	    unsigned bit_off = idx%4;
	    unsigned str_off = idx/4;
	    ವರ್ಣ ch;

	    ಪ್ರತಿಜ್ಞಾ (str_off+skip_chars < str_len);
	      /* Skip any "_" characters in the string. */
	    ಯಾವತ್((ch = str[str_len-str_off-1-skip_chars]) == '_') {
		  skip_chars += 1;
		  ಪ್ರತಿಜ್ಞಾ (str_off+skip_chars < str_len);
	    }

	      /* If we find a "-" it must be at the head of the string. */
	    ಯದಿ (ch == '-') ಪ್ರತಿಜ್ಞಾ(0);

	    ವಿಕಲ್ಪ (ch) {
		ಪಕ್ಷ '0':
		ಪಕ್ಷ '1':
		ಪಕ್ಷ '2':
		ಪಕ್ಷ '3':
		ಪಕ್ಷ '4':
		ಪಕ್ಷ '5':
		ಪಕ್ಷ '6':
		ಪಕ್ಷ '7':
		ಪಕ್ಷ '8':
		ಪಕ್ಷ '9':
		  tmp = ch - '0';
		  tval.set_bit(idx, ((tmp>>bit_off)&1)? BIT4_1 : BIT4_0);
		  ಭಙ್ಗ;
		ಪಕ್ಷ 'a':
		ಪಕ್ಷ 'b':
		ಪಕ್ಷ 'c':
		ಪಕ್ಷ 'd':
		ಪಕ್ಷ 'e':
		ಪಕ್ಷ 'f':
		  tmp = ch - 'a' + 10;
		  tval.set_bit(idx, ((tmp>>bit_off)&1)? BIT4_1 : BIT4_0);
		  ಭಙ್ಗ;
		ಪಕ್ಷ 'A':
		ಪಕ್ಷ 'B':
		ಪಕ್ಷ 'C':
		ಪಕ್ಷ 'D':
		ಪಕ್ಷ 'E':
		ಪಕ್ಷ 'F':
		  tmp = ch - 'A' + 10;
		  tval.set_bit(idx, ((tmp>>bit_off)&1)? BIT4_1 : BIT4_0);
		  ಭಙ್ಗ;
		ಪಕ್ಷ 'x':
		ಪಕ್ಷ 'X':
		  tval.set_bit(idx, BIT4_X);
		  ಭಙ್ಗ;
		ಪಕ್ಷ 'z':
		ಪಕ್ಷ 'Z':
		  tval.set_bit(idx, BIT4_Z);
		  ಭಙ್ಗ;
		ಸ್ವಭಾವ:
		    /* Return "x" if there are invalid digits in the string. */
		  fprintf(stderr, "Warning: Invalid hex digit %c(%d) in "
		          "\"%s\".\n", ch, ch, str);
		  ಪ್ರತಿ (unsigned jdx = 0 ;  jdx < val.size() ;  jdx += 1) {
			val.set_bit(jdx, BIT4_X);
		  }
		  ಪ್ರತ್ಯಾಗಮ;
	    }
      }

	/* Make a negative value when needed. */
      ಯದಿ (str[0] == '-') {
	    tval.invert();
	    tval += (int64_t) 1;
      }

	/* Find the correct padding value. */
      vvp_bit4_t pad;
      ವಿಕಲ್ಪ (tval.value(tval.size()-1)) {
	  ಪಕ್ಷ BIT4_X:  // Pad MSB 'x' with 'x'.
	    pad = BIT4_X;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ BIT4_Z:  // Pad MSB 'z' with 'z'.
	    pad = BIT4_Z;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ BIT4_1:  // If negative pad MSB '1' with '1'.
	    ಯದಿ (str[0] == '-') {
		  pad = BIT4_1;
		  ಭಙ್ಗ;
	    }
	    // fallthrough
	  ಸ್ವಭಾವ:  // Everything else gets '0' padded.
	    pad = BIT4_0;
	    ಭಙ್ಗ;
      }

	/* Copy the temporary value to the real value, padding if needed. */
      ಪ್ರತಿ (unsigned idx = 0 ;  idx < val.size() ;  idx += 1) {
	    ಯದಿ (idx < tval.size()) val.set_bit(idx, tval.value(idx));
	    ಅನ್ಯಥಾ val.set_bit(idx, pad);
      }
}

ಶೂನ್ಯ vpip_vec4_to_hex_str(ಧ್ರುವ vvp_vector4_t&bits, ವರ್ಣ*buf, unsigned nbuf)
{
      unsigned slen = (bits.size() + 3) / 4;
      ಪ್ರತಿಜ್ಞಾ(slen < nbuf);

      buf[slen] = 0;

      unsigned val = 0;
      ಪ್ರತಿ (unsigned idx = 0 ;  idx < bits.size() ;  idx += 1) {
	    unsigned vs = (idx%4) * 2;

	    ವಿಕಲ್ಪ (bits.value(idx)) {
		ಪಕ್ಷ BIT4_0:
		  ಭಙ್ಗ;
		ಪಕ್ಷ BIT4_1:
		  val |= 1 << vs;
		  ಭಙ್ಗ;
		ಪಕ್ಷ BIT4_X:
		  val |= 2 << vs;
		  ಭಙ್ಗ;
		ಪಕ್ಷ BIT4_Z:
		  val |= 3 << vs;
	    }

	    ಯದಿ (vs == 6) {
		  slen -= 1;
		  buf[slen] = hex_digits[val];
		  val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      ವಿಕಲ್ಪ (bits.size() % 4) {
	  ಪಕ್ಷ 1:
	    ಯದಿ (val == 2) val = 170;
	    ಅನ್ಯಥಾ ಯದಿ (val == 3) val = 255;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ 2:
	    ಯದಿ (val == 10) val = 170;
	    ಅನ್ಯಥಾ ಯದಿ (val == 15) val = 255;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ 3:
	    ಯದಿ (val == 42) val = 170;
	    ಅನ್ಯಥಾ ಯದಿ (val == 63) val = 255;
	    ಭಙ್ಗ;
      }

      ಯದಿ (slen > 0) {
	    slen -= 1;
	    buf[slen] = hex_digits[val];
      }
}
