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

व्योम vpip_bin_str_to_vec4(vvp_vector4_t&vec4, स्थिर अक्षर*buf)
{
      स्थिर अक्षर*ebuf = buf + म_माप(buf);
      अचिन्हित idx = 0;
      अचिन्हित skip_chars = 0;
      स्थिर अक्षर*tbuf = buf;
	/* Find the number of non-numeric characters. */
      जबतक ((tbuf = strpbrk(tbuf, "-_"))) {
	    skip_chars += 1;
	    tbuf += 1;
      }
      vvp_vector4_t tval(म_माप(buf)-skip_chars);
      जबतक (ebuf > buf) {
	    ebuf -= 1;
	      /* Skip any "_" characters in the string. */
	    जबतक (*ebuf == '_') {
		  ebuf -= 1;
		  निश्चित(ebuf > buf);
	    }

	      /*  If we find a "-" it must be at the head of the string. */
	    अगर (*ebuf == '-') {
		  अगर (ebuf != buf) निश्चित(0);
		  अवरोध;
	    }

	    निश्चित(idx < tval.size());
	    चयन (*ebuf) {
		हाल '0':
		  tval.set_bit(idx, BIT4_0);
		  अवरोध;
		हाल '1':
		  tval.set_bit(idx, BIT4_1);
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
		  ख_लिखो(मानक_त्रुटि, "Warning: Invalid binary digit %c(%d) in "
		          "\"%s\".\n", *ebuf, *ebuf, buf);
		  क्रम (अचिन्हित jdx = 0 ;  jdx < vec4.size() ;  jdx += 1) {
			vec4.set_bit(jdx, BIT4_X);
		  }
		  वापस;
	    }

	    idx += 1;
      }

	/* Make a negative value when needed. */
      अगर (buf[0] == '-') {
	    tval.invert();
	    tval += (int64_t) 1;
      }

	/* Find the correct padding value. */
      vvp_bit4_t pad;
      चयन (tval.value(tval.size()-1)) {
	  हाल BIT4_X:  // Pad MSB 'x' with 'x'
	    pad = BIT4_X;
	    अवरोध;
	  हाल BIT4_Z:  // Pad MSB 'z' with 'z'
	    pad = BIT4_Z;
	    अवरोध;
	  हाल BIT4_1:  // If negative pad MSB '1' with '1'
	    अगर (buf[0] == '-') {
		  pad = BIT4_1;
		  अवरोध;
	    }
	    // fallthrough
	  शेष:  // Everything else gets '0' padded/
	    pad = BIT4_0;
	    अवरोध;
      }

      क्रम (अचिन्हित jdx = 0 ;  jdx < vec4.size() ; jdx += 1) {
	    अगर (jdx < tval.size()) vec4.set_bit(jdx, tval.value(jdx));
	    अन्यथा vec4.set_bit(jdx, pad);
      }
}
