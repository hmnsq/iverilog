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

ബാഹ്യ സ്ഥിരം അക്ഷരം hex_digits[256];

ശൂന്യം vpip_hex_str_to_vec4(vvp_vector4_t&val, സ്ഥിരം അക്ഷരം*str)
{
      unsigned str_len = strlen(str);
      unsigned skip_chars = 0;
      സ്ഥിരം അക്ഷരം*tstr = str;
	/* Find the number of non-numeric characters. */
      ആകുമ്പോൾ ((tstr = strpbrk(tstr, "-_"))) {
	    skip_chars += 1;
	    tstr += 1;
      }
      vvp_vector4_t tval(4*(str_len-skip_chars));
      skip_chars = 0;
      വേണ്ടി (unsigned idx = 0 ;  idx < tval.size() ;  idx += 1) {
	    unsigned tmp;
	    unsigned bit_off = idx%4;
	    unsigned str_off = idx/4;
	    അക്ഷരം ch;

	    പ്രതിജ്ഞാ (str_off+skip_chars < str_len);
	      /* Skip any "_" characters in the string. */
	    ആകുമ്പോൾ((ch = str[str_len-str_off-1-skip_chars]) == '_') {
		  skip_chars += 1;
		  പ്രതിജ്ഞാ (str_off+skip_chars < str_len);
	    }

	      /* If we find a "-" it must be at the head of the string. */
	    എങ്കിൽ (ch == '-') പ്രതിജ്ഞാ(0);

	    തിരഞ്ഞെടുപ്പ് (ch) {
		അവസ്ഥ '0':
		അവസ്ഥ '1':
		അവസ്ഥ '2':
		അവസ്ഥ '3':
		അവസ്ഥ '4':
		അവസ്ഥ '5':
		അവസ്ഥ '6':
		അവസ്ഥ '7':
		അവസ്ഥ '8':
		അവസ്ഥ '9':
		  tmp = ch - '0';
		  tval.set_bit(idx, ((tmp>>bit_off)&1)? BIT4_1 : BIT4_0);
		  നിർത്തുക;
		അവസ്ഥ 'a':
		അവസ്ഥ 'b':
		അവസ്ഥ 'c':
		അവസ്ഥ 'd':
		അവസ്ഥ 'e':
		അവസ്ഥ 'f':
		  tmp = ch - 'a' + 10;
		  tval.set_bit(idx, ((tmp>>bit_off)&1)? BIT4_1 : BIT4_0);
		  നിർത്തുക;
		അവസ്ഥ 'A':
		അവസ്ഥ 'B':
		അവസ്ഥ 'C':
		അവസ്ഥ 'D':
		അവസ്ഥ 'E':
		അവസ്ഥ 'F':
		  tmp = ch - 'A' + 10;
		  tval.set_bit(idx, ((tmp>>bit_off)&1)? BIT4_1 : BIT4_0);
		  നിർത്തുക;
		അവസ്ഥ 'x':
		അവസ്ഥ 'X':
		  tval.set_bit(idx, BIT4_X);
		  നിർത്തുക;
		അവസ്ഥ 'z':
		അവസ്ഥ 'Z':
		  tval.set_bit(idx, BIT4_Z);
		  നിർത്തുക;
		സ്ഥിരസ്ഥിതി:
		    /* Return "x" if there are invalid digits in the string. */
		  fprintf(stderr, "Warning: Invalid hex digit %c(%d) in "
		          "\"%s\".\n", ch, ch, str);
		  വേണ്ടി (unsigned jdx = 0 ;  jdx < val.size() ;  jdx += 1) {
			val.set_bit(jdx, BIT4_X);
		  }
		  തിരികെ;
	    }
      }

	/* Make a negative value when needed. */
      എങ്കിൽ (str[0] == '-') {
	    tval.invert();
	    tval += (int64_t) 1;
      }

	/* Find the correct padding value. */
      vvp_bit4_t pad;
      തിരഞ്ഞെടുപ്പ് (tval.value(tval.size()-1)) {
	  അവസ്ഥ BIT4_X:  // Pad MSB 'x' with 'x'.
	    pad = BIT4_X;
	    നിർത്തുക;
	  അവസ്ഥ BIT4_Z:  // Pad MSB 'z' with 'z'.
	    pad = BIT4_Z;
	    നിർത്തുക;
	  അവസ്ഥ BIT4_1:  // If negative pad MSB '1' with '1'.
	    എങ്കിൽ (str[0] == '-') {
		  pad = BIT4_1;
		  നിർത്തുക;
	    }
	    // fallthrough
	  സ്ഥിരസ്ഥിതി:  // Everything else gets '0' padded.
	    pad = BIT4_0;
	    നിർത്തുക;
      }

	/* Copy the temporary value to the real value, padding if needed. */
      വേണ്ടി (unsigned idx = 0 ;  idx < val.size() ;  idx += 1) {
	    എങ്കിൽ (idx < tval.size()) val.set_bit(idx, tval.value(idx));
	    അല്ലെങ്കിൽ val.set_bit(idx, pad);
      }
}

ശൂന്യം vpip_vec4_to_hex_str(സ്ഥിരം vvp_vector4_t&bits, അക്ഷരം*buf, unsigned nbuf)
{
      unsigned slen = (bits.size() + 3) / 4;
      പ്രതിജ്ഞാ(slen < nbuf);

      buf[slen] = 0;

      unsigned val = 0;
      വേണ്ടി (unsigned idx = 0 ;  idx < bits.size() ;  idx += 1) {
	    unsigned vs = (idx%4) * 2;

	    തിരഞ്ഞെടുപ്പ് (bits.value(idx)) {
		അവസ്ഥ BIT4_0:
		  നിർത്തുക;
		അവസ്ഥ BIT4_1:
		  val |= 1 << vs;
		  നിർത്തുക;
		അവസ്ഥ BIT4_X:
		  val |= 2 << vs;
		  നിർത്തുക;
		അവസ്ഥ BIT4_Z:
		  val |= 3 << vs;
	    }

	    എങ്കിൽ (vs == 6) {
		  slen -= 1;
		  buf[slen] = hex_digits[val];
		  val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      തിരഞ്ഞെടുപ്പ് (bits.size() % 4) {
	  അവസ്ഥ 1:
	    എങ്കിൽ (val == 2) val = 170;
	    അല്ലെങ്കിൽ എങ്കിൽ (val == 3) val = 255;
	    നിർത്തുക;
	  അവസ്ഥ 2:
	    എങ്കിൽ (val == 10) val = 170;
	    അല്ലെങ്കിൽ എങ്കിൽ (val == 15) val = 255;
	    നിർത്തുക;
	  അവസ്ഥ 3:
	    എങ്കിൽ (val == 42) val = 170;
	    അല്ലെങ്കിൽ എങ്കിൽ (val == 63) val = 255;
	    നിർത്തുക;
      }

      എങ്കിൽ (slen > 0) {
	    slen -= 1;
	    buf[slen] = hex_digits[val];
      }
}
