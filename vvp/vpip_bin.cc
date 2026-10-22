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

வெறுமை vpip_bin_str_to_vec4(vvp_vector4_t&vec4, மாறிலி எழுத்து*buf)
{
      மாறிலி எழுத்து*ebuf = buf + strlen(buf);
      unsigned idx = 0;
      unsigned skip_chars = 0;
      மாறிலி எழுத்து*tbuf = buf;
	/* Find the number of non-numeric characters. */
      போது ((tbuf = strpbrk(tbuf, "-_"))) {
	    skip_chars += 1;
	    tbuf += 1;
      }
      vvp_vector4_t tval(strlen(buf)-skip_chars);
      போது (ebuf > buf) {
	    ebuf -= 1;
	      /* Skip any "_" characters in the string. */
	    போது (*ebuf == '_') {
		  ebuf -= 1;
		  ப்ரதிஜ்ஞா(ebuf > buf);
	    }

	      /*  If we find a "-" it must be at the head of the string. */
	    எனில் (*ebuf == '-') {
		  எனில் (ebuf != buf) ப்ரதிஜ்ஞா(0);
		  நிறுத்து;
	    }

	    ப்ரதிஜ்ஞா(idx < tval.size());
	    தேர்வு (*ebuf) {
		நிலை '0':
		  tval.set_bit(idx, BIT4_0);
		  நிறுத்து;
		நிலை '1':
		  tval.set_bit(idx, BIT4_1);
		  நிறுத்து;
		நிலை 'x':
		நிலை 'X':
		  tval.set_bit(idx, BIT4_X);
		  நிறுத்து;
		நிலை 'z':
		நிலை 'Z':
		  tval.set_bit(idx, BIT4_Z);
		  நிறுத்து;
		இயல்பு:
		    /* Return "x" if there are invalid digits in the string. */
		  fprintf(stderr, "Warning: Invalid binary digit %c(%d) in "
		          "\"%s\".\n", *ebuf, *ebuf, buf);
		  க்கு (unsigned jdx = 0 ;  jdx < vec4.size() ;  jdx += 1) {
			vec4.set_bit(jdx, BIT4_X);
		  }
		  திரும்பு;
	    }

	    idx += 1;
      }

	/* Make a negative value when needed. */
      எனில் (buf[0] == '-') {
	    tval.invert();
	    tval += (int64_t) 1;
      }

	/* Find the correct padding value. */
      vvp_bit4_t pad;
      தேர்வு (tval.value(tval.size()-1)) {
	  நிலை BIT4_X:  // Pad MSB 'x' with 'x'
	    pad = BIT4_X;
	    நிறுத்து;
	  நிலை BIT4_Z:  // Pad MSB 'z' with 'z'
	    pad = BIT4_Z;
	    நிறுத்து;
	  நிலை BIT4_1:  // If negative pad MSB '1' with '1'
	    எனில் (buf[0] == '-') {
		  pad = BIT4_1;
		  நிறுத்து;
	    }
	    // fallthrough
	  இயல்பு:  // Everything else gets '0' padded/
	    pad = BIT4_0;
	    நிறுத்து;
      }

      க்கு (unsigned jdx = 0 ;  jdx < vec4.size() ; jdx += 1) {
	    எனில் (jdx < tval.size()) vec4.set_bit(jdx, tval.value(jdx));
	    இல்லையெனில் vec4.set_bit(jdx, pad);
      }
}
