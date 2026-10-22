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

ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ hex_digits[256];

ಶೂನ್ಯ vpip_hex_str_ಗಮ್ಯ_vec4(vvp_vector4_t&val, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*str)
{
      ಚಿಹ್ನೆರಹಿತ str_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(str);
      ಚಿಹ್ನೆರಹಿತ skip_chars = 0;
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*tstr = str;
	/* Find the number of non-numeric characters. */
      ಆಗ ((tstr = strpbrk(tstr, "-_"))) {
	    skip_chars += 1;
	    tstr += 1;
      }
      vvp_vector4_t tval(4*(str_len-skip_chars));
      skip_chars = 0;
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < tval.size() ;  idx += 1) {
	    ಚಿಹ್ನೆರಹಿತ tmp;
	    ಚಿಹ್ನೆರಹಿತ ಬಿಟ್_off = idx%4;
	    ಚಿಹ್ನೆರಹಿತ str_off = idx/4;
	    ಅಕ್ಷರ ch;

	    ಪ್ರತಿಜ್ಞಾ (str_off+skip_chars < str_len);
	      /* Skip any "_" characters in the string. */
	    ಆಗ((ch = str[str_len-str_off-1-skip_chars]) == '_') {
		  skip_chars += 1;
		  ಪ್ರತಿಜ್ಞಾ (str_off+skip_chars < str_len);
	    }

	      /* If we find a "-" it must be at the head of the string. */
	    ಆದರೆ (ch == '-') ಪ್ರತಿಜ್ಞಾ(0);

	    ಆಯ್ಕೆ (ch) {
		ಸ್ಥಿತಿ '0':
		ಸ್ಥಿತಿ '1':
		ಸ್ಥಿತಿ '2':
		ಸ್ಥಿತಿ '3':
		ಸ್ಥಿತಿ '4':
		ಸ್ಥಿತಿ '5':
		ಸ್ಥಿತಿ '6':
		ಸ್ಥಿತಿ '7':
		ಸ್ಥಿತಿ '8':
		ಸ್ಥಿತಿ '9':
		  tmp = ch - '0';
		  tval.ಗಣ_ಬಿಟ್(idx, ((tmp>>ಬಿಟ್_off)&1)? BIT4_1 : BIT4_0);
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'a':
		ಸ್ಥಿತಿ 'b':
		ಸ್ಥಿತಿ 'c':
		ಸ್ಥಿತಿ 'd':
		ಸ್ಥಿತಿ 'e':
		ಸ್ಥಿತಿ 'f':
		  tmp = ch - 'a' + 10;
		  tval.ಗಣ_ಬಿಟ್(idx, ((tmp>>ಬಿಟ್_off)&1)? BIT4_1 : BIT4_0);
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'A':
		ಸ್ಥಿತಿ 'B':
		ಸ್ಥಿತಿ 'C':
		ಸ್ಥಿತಿ 'D':
		ಸ್ಥಿತಿ 'E':
		ಸ್ಥಿತಿ 'F':
		  tmp = ch - 'A' + 10;
		  tval.ಗಣ_ಬಿಟ್(idx, ((tmp>>ಬಿಟ್_off)&1)? BIT4_1 : BIT4_0);
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'x':
		ಸ್ಥಿತಿ 'X':
		  tval.ಗಣ_ಬಿಟ್(idx, BIT4_X);
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'z':
		ಸ್ಥಿತಿ 'Z':
		  tval.ಗಣ_ಬಿಟ್(idx, BIT4_Z);
		  ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
		    /* Return "x" if there are invalid digits in the string. */
		  fprintf(stderr, "Warning: Invalid hex digit %c(%d) in "
		          "\"%s\".\n", ch, ch, str);
		  ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ jdx = 0 ;  jdx < val.size() ;  jdx += 1) {
			val.ಗಣ_ಬಿಟ್(jdx, BIT4_X);
		  }
		  ಹಿಂತಿರುಗಿ;
	    }
      }

	/* Make a negative value when needed. */
      ಆದರೆ (str[0] == '-') {
	    tval.invert();
	    tval += (int64_t) 1;
      }

	/* Find the correct padding value. */
      vvp_bit4_t pad;
      ಆಯ್ಕೆ (tval.value(tval.size()-1)) {
	  ಸ್ಥಿತಿ BIT4_X:  // Pad MSB 'x' with 'x'.
	    pad = BIT4_X;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ BIT4_Z:  // Pad MSB 'z' with 'z'.
	    pad = BIT4_Z;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ BIT4_1:  // If negative pad MSB '1' with '1'.
	    ಆದರೆ (str[0] == '-') {
		  pad = BIT4_1;
		  ನಿಲ್ಲಿಸು;
	    }
	    // fallthrough
	  ಪೂರ್ವನಿಯೋಜಿತ:  // Everything else gets '0' padded.
	    pad = BIT4_0;
	    ನಿಲ್ಲಿಸು;
      }

	/* Copy the temporary value to the real value, padding if needed. */
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < val.size() ;  idx += 1) {
	    ಆದರೆ (idx < tval.size()) val.ಗಣ_ಬಿಟ್(idx, tval.value(idx));
	    ಇಲ್ಲದಿದ್ದರೆ val.ಗಣ_ಬಿಟ್(idx, pad);
      }
}

ಶೂನ್ಯ vpip_vec4_ಗಮ್ಯ_hex_str(ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್ಟುಗಳು, ಅಕ್ಷರ*buf, ಚಿಹ್ನೆರಹಿತ nbuf)
{
      ಚಿಹ್ನೆರಹಿತ slen = (ಬಿಟ್ಟುಗಳು.size() + 3) / 4;
      ಪ್ರತಿಜ್ಞಾ(slen < nbuf);

      buf[slen] = 0;

      ಚಿಹ್ನೆರಹಿತ val = 0;
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < ಬಿಟ್ಟುಗಳು.size() ;  idx += 1) {
	    ಚಿಹ್ನೆರಹಿತ vs = (idx%4) * 2;

	    ಆಯ್ಕೆ (ಬಿಟ್ಟುಗಳು.value(idx)) {
		ಸ್ಥಿತಿ BIT4_0:
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BIT4_1:
		  val |= 1 << vs;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BIT4_X:
		  val |= 2 << vs;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BIT4_Z:
		  val |= 3 << vs;
	    }

	    ಆದರೆ (vs == 6) {
		  slen -= 1;
		  buf[slen] = hex_digits[val];
		  val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      ಆಯ್ಕೆ (ಬಿಟ್ಟುಗಳು.size() % 4) {
	  ಸ್ಥಿತಿ 1:
	    ಆದರೆ (val == 2) val = 170;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (val == 3) val = 255;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 2:
	    ಆದರೆ (val == 10) val = 170;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (val == 15) val = 255;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 3:
	    ಆದರೆ (val == 42) val = 170;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (val == 63) val = 255;
	    ನಿಲ್ಲಿಸು;
      }

      ಆದರೆ (slen > 0) {
	    slen -= 1;
	    buf[slen] = hex_digits[val];
      }
}
