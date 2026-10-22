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

ଶୂନ୍ୟ vpip_bin_str_to_vec4(vvp_vector4_t&vec4, ଅଚଳ ଅକ୍ଷର*buf)
{
      ଅଚଳ ଅକ୍ଷର*ebuf = buf + strlen(buf);
      unsigned idx = 0;
      unsigned skip_chars = 0;
      ଅଚଳ ଅକ୍ଷର*tbuf = buf;
	/* Find the number of non-numeric characters. */
      ଯେତେବେଳେ ((tbuf = strpbrk(tbuf, "-_"))) {
	    skip_chars += 1;
	    tbuf += 1;
      }
      vvp_vector4_t tval(strlen(buf)-skip_chars);
      ଯେତେବେଳେ (ebuf > buf) {
	    ebuf -= 1;
	      /* Skip any "_" characters in the string. */
	    ଯେତେବେଳେ (*ebuf == '_') {
		  ebuf -= 1;
		  ପ୍ରତିଜ୍ଞା(ebuf > buf);
	    }

	      /*  If we find a "-" it must be at the head of the string. */
	    ଯଦି (*ebuf == '-') {
		  ଯଦି (ebuf != buf) ପ୍ରତିଜ୍ଞା(0);
		  ଭାଙ୍ଗ;
	    }

	    ପ୍ରତିଜ୍ଞା(idx < tval.size());
	    ବାଛ (*ebuf) {
		ସ୍ଥିତି '0':
		  tval.set_bit(idx, BIT4_0);
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି '1':
		  tval.set_bit(idx, BIT4_1);
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
		  fprintf(stderr, "Warning: Invalid binary digit %c(%d) in "
		          "\"%s\".\n", *ebuf, *ebuf, buf);
		  ପାଇଁ (unsigned jdx = 0 ;  jdx < vec4.size() ;  jdx += 1) {
			vec4.set_bit(jdx, BIT4_X);
		  }
		  ଫେରନ୍ତୁ;
	    }

	    idx += 1;
      }

	/* Make a negative value when needed. */
      ଯଦି (buf[0] == '-') {
	    tval.invert();
	    tval += (int64_t) 1;
      }

	/* Find the correct padding value. */
      vvp_bit4_t pad;
      ବାଛ (tval.value(tval.size()-1)) {
	  ସ୍ଥିତି BIT4_X:  // Pad MSB 'x' with 'x'
	    pad = BIT4_X;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି BIT4_Z:  // Pad MSB 'z' with 'z'
	    pad = BIT4_Z;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି BIT4_1:  // If negative pad MSB '1' with '1'
	    ଯଦି (buf[0] == '-') {
		  pad = BIT4_1;
		  ଭାଙ୍ଗ;
	    }
	    // fallthrough
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:  // Everything else gets '0' padded/
	    pad = BIT4_0;
	    ଭାଙ୍ଗ;
      }

      ପାଇଁ (unsigned jdx = 0 ;  jdx < vec4.size() ; jdx += 1) {
	    ଯଦି (jdx < tval.size()) vec4.set_bit(jdx, tval.value(jdx));
	    ଅନ୍ୟଥା vec4.set_bit(jdx, pad);
      }
}
