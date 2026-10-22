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

बाह्य स्थिरांक अक्षर hex_digits[256];

शून्य vpip_hex_str_to_vec4(vvp_vector4_t&val, स्थिरांक अक्षर*str)
{
      अचिन्हित str_len = म_माप(str);
      अचिन्हित skip_chars = 0;
      स्थिरांक अक्षर*tstr = str;
	/* Find the number of non-numeric characters. */
      पर्यंत ((tstr = strpbrk(tstr, "-_"))) {
	    skip_chars += 1;
	    tstr += 1;
      }
      vvp_vector4_t tval(4*(str_len-skip_chars));
      skip_chars = 0;
      साठी (अचिन्हित idx = 0 ;  idx < tval.size() ;  idx += 1) {
	    अचिन्हित tmp;
	    अचिन्हित bit_off = idx%4;
	    अचिन्हित str_off = idx/4;
	    अक्षर ch;

	    प्रतिज्ञा (str_off+skip_chars < str_len);
	      /* Skip any "_" characters in the string. */
	    पर्यंत((ch = str[str_len-str_off-1-skip_chars]) == '_') {
		  skip_chars += 1;
		  प्रतिज्ञा (str_off+skip_chars < str_len);
	    }

	      /* If we find a "-" it must be at the head of the string. */
	    जर (ch == '-') प्रतिज्ञा(0);

	    निवड (ch) {
		स्थिती '0':
		स्थिती '1':
		स्थिती '2':
		स्थिती '3':
		स्थिती '4':
		स्थिती '5':
		स्थिती '6':
		स्थिती '7':
		स्थिती '8':
		स्थिती '9':
		  tmp = ch - '0';
		  tval.set_bit(idx, ((tmp>>bit_off)&1)? BIT4_1 : BIT4_0);
		  थांबा;
		स्थिती 'a':
		स्थिती 'b':
		स्थिती 'c':
		स्थिती 'd':
		स्थिती 'e':
		स्थिती 'f':
		  tmp = ch - 'a' + 10;
		  tval.set_bit(idx, ((tmp>>bit_off)&1)? BIT4_1 : BIT4_0);
		  थांबा;
		स्थिती 'A':
		स्थिती 'B':
		स्थिती 'C':
		स्थिती 'D':
		स्थिती 'E':
		स्थिती 'F':
		  tmp = ch - 'A' + 10;
		  tval.set_bit(idx, ((tmp>>bit_off)&1)? BIT4_1 : BIT4_0);
		  थांबा;
		स्थिती 'x':
		स्थिती 'X':
		  tval.set_bit(idx, BIT4_X);
		  थांबा;
		स्थिती 'z':
		स्थिती 'Z':
		  tval.set_bit(idx, BIT4_Z);
		  थांबा;
		पूर्वनिर्धारित:
		    /* Return "x" if there are invalid digits in the string. */
		  ख_लिखो(मानक_त्रुटि, "Warning: Invalid hex digit %c(%d) in "
		          "\"%s\".\n", ch, ch, str);
		  साठी (अचिन्हित jdx = 0 ;  jdx < val.size() ;  jdx += 1) {
			val.set_bit(jdx, BIT4_X);
		  }
		  परत;
	    }
      }

	/* Make a negative value when needed. */
      जर (str[0] == '-') {
	    tval.invert();
	    tval += (int64_t) 1;
      }

	/* Find the correct padding value. */
      vvp_bit4_t pad;
      निवड (tval.value(tval.size()-1)) {
	  स्थिती BIT4_X:  // Pad MSB 'x' with 'x'.
	    pad = BIT4_X;
	    थांबा;
	  स्थिती BIT4_Z:  // Pad MSB 'z' with 'z'.
	    pad = BIT4_Z;
	    थांबा;
	  स्थिती BIT4_1:  // If negative pad MSB '1' with '1'.
	    जर (str[0] == '-') {
		  pad = BIT4_1;
		  थांबा;
	    }
	    // fallthrough
	  पूर्वनिर्धारित:  // Everything else gets '0' padded.
	    pad = BIT4_0;
	    थांबा;
      }

	/* Copy the temporary value to the real value, padding if needed. */
      साठी (अचिन्हित idx = 0 ;  idx < val.size() ;  idx += 1) {
	    जर (idx < tval.size()) val.set_bit(idx, tval.value(idx));
	    अन्यथा val.set_bit(idx, pad);
      }
}

शून्य vpip_vec4_to_hex_str(स्थिरांक vvp_vector4_t&bits, अक्षर*buf, अचिन्हित nbuf)
{
      अचिन्हित slen = (bits.size() + 3) / 4;
      प्रतिज्ञा(slen < nbuf);

      buf[slen] = 0;

      अचिन्हित val = 0;
      साठी (अचिन्हित idx = 0 ;  idx < bits.size() ;  idx += 1) {
	    अचिन्हित vs = (idx%4) * 2;

	    निवड (bits.value(idx)) {
		स्थिती BIT4_0:
		  थांबा;
		स्थिती BIT4_1:
		  val |= 1 << vs;
		  थांबा;
		स्थिती BIT4_X:
		  val |= 2 << vs;
		  थांबा;
		स्थिती BIT4_Z:
		  val |= 3 << vs;
	    }

	    जर (vs == 6) {
		  slen -= 1;
		  buf[slen] = hex_digits[val];
		  val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      निवड (bits.size() % 4) {
	  स्थिती 1:
	    जर (val == 2) val = 170;
	    अन्यथा जर (val == 3) val = 255;
	    थांबा;
	  स्थिती 2:
	    जर (val == 10) val = 170;
	    अन्यथा जर (val == 15) val = 255;
	    थांबा;
	  स्थिती 3:
	    जर (val == 42) val = 170;
	    अन्यथा जर (val == 63) val = 255;
	    थांबा;
      }

      जर (slen > 0) {
	    slen -= 1;
	    buf[slen] = hex_digits[val];
      }
}
