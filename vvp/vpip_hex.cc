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

बाह्य स्थिर अक्षर hex_digits[256];

व्योम vpip_hex_str_to_vec4(vvp_vector4_t&val, स्थिर अक्षर*str)
{
      अचिन्हित str_len = म_माप(str);
      अचिन्हित skip_chars = 0;
      स्थिर अक्षर*tstr = str;
	/* Find the number of non-numeric characters. */
      जबतक ((tstr = strpbrk(tstr, "-_"))) {
	    skip_chars += 1;
	    tstr += 1;
      }
      vvp_vector4_t tval(4*(str_len-skip_chars));
      skip_chars = 0;
      क्रम (अचिन्हित idx = 0 ;  idx < tval.size() ;  idx += 1) {
	    अचिन्हित tmp;
	    अचिन्हित bit_off = idx%4;
	    अचिन्हित str_off = idx/4;
	    अक्षर ch;

	    निश्चित (str_off+skip_chars < str_len);
	      /* Skip any "_" characters in the string. */
	    जबतक((ch = str[str_len-str_off-1-skip_chars]) == '_') {
		  skip_chars += 1;
		  निश्चित (str_off+skip_chars < str_len);
	    }

	      /* If we find a "-" it must be at the head of the string. */
	    अगर (ch == '-') निश्चित(0);

	    चयन (ch) {
		हाल '0':
		हाल '1':
		हाल '2':
		हाल '3':
		हाल '4':
		हाल '5':
		हाल '6':
		हाल '7':
		हाल '8':
		हाल '9':
		  tmp = ch - '0';
		  tval.set_bit(idx, ((tmp>>bit_off)&1)? BIT4_1 : BIT4_0);
		  अवरोध;
		हाल 'a':
		हाल 'b':
		हाल 'c':
		हाल 'd':
		हाल 'e':
		हाल 'f':
		  tmp = ch - 'a' + 10;
		  tval.set_bit(idx, ((tmp>>bit_off)&1)? BIT4_1 : BIT4_0);
		  अवरोध;
		हाल 'A':
		हाल 'B':
		हाल 'C':
		हाल 'D':
		हाल 'E':
		हाल 'F':
		  tmp = ch - 'A' + 10;
		  tval.set_bit(idx, ((tmp>>bit_off)&1)? BIT4_1 : BIT4_0);
		  अवरोध;
		हाल 'x':
		हाल 'X':
		  tval.set_bit(idx, BIT4_X);
		  अवरोध;
		हाल 'z':
		हाल 'Z':
		  tval.set_bit(idx, BIT4_Z);
		  अवरोध;
		शेष:
		    /* Return "x" if there are invalid digits in the string. */
		  ख_लिखो(मानक_त्रुटि, "Warning: Invalid hex digit %c(%d) in "
		          "\"%s\".\n", ch, ch, str);
		  क्रम (अचिन्हित jdx = 0 ;  jdx < val.size() ;  jdx += 1) {
			val.set_bit(jdx, BIT4_X);
		  }
		  वापस;
	    }
      }

	/* Make a negative value when needed. */
      अगर (str[0] == '-') {
	    tval.invert();
	    tval += (int64_t) 1;
      }

	/* Find the correct padding value. */
      vvp_bit4_t pad;
      चयन (tval.value(tval.size()-1)) {
	  हाल BIT4_X:  // Pad MSB 'x' with 'x'.
	    pad = BIT4_X;
	    अवरोध;
	  हाल BIT4_Z:  // Pad MSB 'z' with 'z'.
	    pad = BIT4_Z;
	    अवरोध;
	  हाल BIT4_1:  // If negative pad MSB '1' with '1'.
	    अगर (str[0] == '-') {
		  pad = BIT4_1;
		  अवरोध;
	    }
	    // fallthrough
	  शेष:  // Everything else gets '0' padded.
	    pad = BIT4_0;
	    अवरोध;
      }

	/* Copy the temporary value to the real value, padding if needed. */
      क्रम (अचिन्हित idx = 0 ;  idx < val.size() ;  idx += 1) {
	    अगर (idx < tval.size()) val.set_bit(idx, tval.value(idx));
	    अन्यथा val.set_bit(idx, pad);
      }
}

व्योम vpip_vec4_to_hex_str(स्थिर vvp_vector4_t&bits, अक्षर*buf, अचिन्हित nbuf)
{
      अचिन्हित slen = (bits.size() + 3) / 4;
      निश्चित(slen < nbuf);

      buf[slen] = 0;

      अचिन्हित val = 0;
      क्रम (अचिन्हित idx = 0 ;  idx < bits.size() ;  idx += 1) {
	    अचिन्हित vs = (idx%4) * 2;

	    चयन (bits.value(idx)) {
		हाल BIT4_0:
		  अवरोध;
		हाल BIT4_1:
		  val |= 1 << vs;
		  अवरोध;
		हाल BIT4_X:
		  val |= 2 << vs;
		  अवरोध;
		हाल BIT4_Z:
		  val |= 3 << vs;
	    }

	    अगर (vs == 6) {
		  slen -= 1;
		  buf[slen] = hex_digits[val];
		  val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      चयन (bits.size() % 4) {
	  हाल 1:
	    अगर (val == 2) val = 170;
	    अन्यथा अगर (val == 3) val = 255;
	    अवरोध;
	  हाल 2:
	    अगर (val == 10) val = 170;
	    अन्यथा अगर (val == 15) val = 255;
	    अवरोध;
	  हाल 3:
	    अगर (val == 42) val = 170;
	    अन्यथा अगर (val == 63) val = 255;
	    अवरोध;
      }

      अगर (slen > 0) {
	    slen -= 1;
	    buf[slen] = hex_digits[val];
      }
}
