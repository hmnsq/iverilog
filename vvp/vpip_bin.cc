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

শূন্য vpip_bin_str_to_vec4(vvp_vector4_t&vec4, ধ্রুৱ ৱর্ণ*buf)
{
      ধ্রুৱ ৱর্ণ*ebuf = buf + strlen(buf);
      unsigned idx = 0;
      unsigned skip_chars = 0;
      ধ্রুৱ ৱর্ণ*tbuf = buf;
	/* Find the number of non-numeric characters. */
      যেতিয়ালৈ ((tbuf = strpbrk(tbuf, "-_"))) {
	    skip_chars += 1;
	    tbuf += 1;
      }
      vvp_vector4_t tval(strlen(buf)-skip_chars);
      যেতিয়ালৈ (ebuf > buf) {
	    ebuf -= 1;
	      /* Skip any "_" characters in the string. */
	    যেতিয়ালৈ (*ebuf == '_') {
		  ebuf -= 1;
		  প্রতিজ্ঞা(ebuf > buf);
	    }

	      /*  If we find a "-" it must be at the head of the string. */
	    যদি (*ebuf == '-') {
		  যদি (ebuf != buf) প্রতিজ্ঞা(0);
		  ভঙ্গ;
	    }

	    প্রতিজ্ঞা(idx < tval.size());
	    ৱিকল্প (*ebuf) {
		পক্ষ '0':
		  tval.set_bit(idx, BIT4_0);
		  ভঙ্গ;
		পক্ষ '1':
		  tval.set_bit(idx, BIT4_1);
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
		  fprintf(stderr, "Warning: Invalid binary digit %c(%d) in "
		          "\"%s\".\n", *ebuf, *ebuf, buf);
		  বাবে (unsigned jdx = 0 ;  jdx < vec4.size() ;  jdx += 1) {
			vec4.set_bit(jdx, BIT4_X);
		  }
		  ঘূৰাই;
	    }

	    idx += 1;
      }

	/* Make a negative value when needed. */
      যদি (buf[0] == '-') {
	    tval.invert();
	    tval += (int64_t) 1;
      }

	/* Find the correct padding value. */
      vvp_bit4_t pad;
      ৱিকল্প (tval.value(tval.size()-1)) {
	  পক্ষ BIT4_X:  // Pad MSB 'x' with 'x'
	    pad = BIT4_X;
	    ভঙ্গ;
	  পক্ষ BIT4_Z:  // Pad MSB 'z' with 'z'
	    pad = BIT4_Z;
	    ভঙ্গ;
	  পক্ষ BIT4_1:  // If negative pad MSB '1' with '1'
	    যদি (buf[0] == '-') {
		  pad = BIT4_1;
		  ভঙ্গ;
	    }
	    // fallthrough
	  স্ৱভাৱ:  // Everything else gets '0' padded/
	    pad = BIT4_0;
	    ভঙ্গ;
      }

      বাবে (unsigned jdx = 0 ;  jdx < vec4.size() ; jdx += 1) {
	    যদি (jdx < tval.size()) vec4.set_bit(jdx, tval.value(jdx));
	    অন্যথা vec4.set_bit(jdx, pad);
      }
}
