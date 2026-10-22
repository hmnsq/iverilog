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

خارجی ثابت حرف oct_digits[64];

خالی vpip_oct_str_مقصد_vec4(vvp_vector4_t&val, ثابت حرف*str)
{
      غیرعلامتی str_len = لڑی_لمبائی(str);
      غیرعلامتی skip_chars = 0;
      ثابت حرف*tstr = str;
	/* Find the number of non-numeric characters. */
      جب ((tstr = strpbrk(tstr, "-_"))) {
	    skip_chars += 1;
	    tstr += 1;
      }
      vvp_vector4_t tval(3*(str_len-skip_chars));
      skip_chars = 0;
      برائے (غیرعلامتی idx = 0 ;  idx < tval.size() ;  idx += 1) {
	    غیرعلامتی tmp;
	    غیرعلامتی ثنائی_ہندسہ_off = idx%3;
	    غیرعلامتی str_off = idx/3;
	    حرف ch;

	    أكد(str_off+skip_chars < str_len);
	      /* Skip any "_" characters in the string. */
	    جب ((ch = str[str_len-str_off-1-skip_chars]) == '_') {
		  skip_chars += 1;
		  أكد(str_off+skip_chars < str_len);
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
		  tmp = ch - '0';
		  tval.سیٹ_ثنائی_ہندسہ(idx, ((tmp>>ثنائی_ہندسہ_off)&1)? BIT4_1 : BIT4_0);
		  توڑو;
		حالت 'x':
		حالت 'X':
		  tval.سیٹ_ثنائی_ہندسہ(idx, BIT4_X);
		  توڑو;
		حالت 'z':
		حالت 'Z':
		  tval.سیٹ_ثنائی_ہندسہ(idx, BIT4_Z);
		  توڑو;
		طےشدہ:
		    /* Return "x" if there are invalid digits in the string. */
		  fprintf(stderr, "Warning: Invalid octal digit %c(%d) in "
		          "\"%s\".\n", ch, ch, str);
		  برائے (غیرعلامتی jdx = 0 ;  jdx < val.size() ;  jdx += 1) {
			val.سیٹ_ثنائی_ہندسہ(jdx, BIT4_X);
		  }
		  واپس;
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
	  حالت BIT4_X:  // Pad MSB 'x' with 'x'
	    pad = BIT4_X;
	    توڑو;
	  حالت BIT4_Z:  // Pad MSB 'z' with 'z'
	    pad = BIT4_Z;
	    توڑو;
	  حالت BIT4_1:  // If negative pad MSB '1' with '1'
	    اگر (str[0] == '-') {
		  pad = BIT4_1;
		  توڑو;
	    }
	    // fallthrough
	  طےشدہ:  // Everything else gets '0' padded.
	    pad = BIT4_0;
	    توڑو;
      }

	/* Copy the temporary value to the real value, padding if needed. */
      برائے (غیرعلامتی idx = 0 ;  idx < val.size() ;  idx += 1) {
	    اگر (idx < tval.size()) val.سیٹ_ثنائی_ہندسہ(idx, tval.value(idx));
	    ورنہ val.سیٹ_ثنائی_ہندسہ(idx, pad);
      }
}

خالی vpip_vec4_مقصد_oct_str(ثابت vvp_vector4_t&بٹ, حرف*buf, غیرعلامتی nbuf)
{
      غیرعلامتی slen = (بٹ.size() + 2) / 3;
      أكد(slen < nbuf);

      buf[slen] = 0;

      غیرعلامتی val = 0;
      برائے (غیرعلامتی idx = 0 ;  idx < بٹ.size() ;  idx += 1) {
	    غیرعلامتی vs = (idx%3) * 2;

	    انتخاب (بٹ.value(idx)) {
		حالت BIT4_0:
		  توڑو;
		حالت BIT4_1:
		  val |= 1 << vs;
		  توڑو;
		حالت BIT4_X:
		  val |= 2 << vs;
		  توڑو;
		حالت BIT4_Z:
		  val |= 3 << vs;
	    }

	    اگر (vs == 4) {
		  slen -= 1;
		  buf[slen] = oct_digits[val];
		  val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      انتخاب (بٹ.size() % 3) {
	  حالت 1:
	    اگر (val == 2) val = 42;
	    ورنہ اگر (val == 3) val = 63;
	    توڑو;
	  حالت 2:
	    اگر (val == 10) val = 42;
	    ورنہ اگر (val == 15) val = 63;
	    توڑو;
      }

      اگر (slen > 0) {
	    slen -= 1;
	    buf[slen] = oct_digits[val];
      }
}
