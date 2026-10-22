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

শূন্য vpip_bin_str_to_vec4(vvp_vector4_t&vec4, ধ্রুবক অক্ষর*buf)
{
      ধ্রুবক অক্ষর*ebuf = buf + strlen(buf);
      unsigned idx = 0;
      unsigned skip_chars = 0;
      ধ্রুবক অক্ষর*tbuf = buf;
	/* Find the number of non-numeric characters. */
      যখন ((tbuf = strpbrk(tbuf, "-_"))) {
	    skip_chars += 1;
	    tbuf += 1;
      }
      vvp_vector4_t tval(strlen(buf)-skip_chars);
      যখন (ebuf > buf) {
	    ebuf -= 1;
	      /* Skip any "_" characters in the string. */
	    যখন (*ebuf == '_') {
		  ebuf -= 1;
		  প্রতিজ্ঞা(ebuf > buf);
	    }

	      /*  If we find a "-" it must be at the head of the string. */
	    যদি (*ebuf == '-') {
		  যদি (ebuf != buf) প্রতিজ্ঞা(0);
		  বিরতি;
	    }

	    প্রতিজ্ঞা(idx < tval.size());
	    নির্বাচন (*ebuf) {
		ক্ষেত্র '0':
		  tval.set_bit(idx, BIT4_0);
		  বিরতি;
		ক্ষেত্র '1':
		  tval.set_bit(idx, BIT4_1);
		  বিরতি;
		ক্ষেত্র 'x':
		ক্ষেত্র 'X':
		  tval.set_bit(idx, BIT4_X);
		  বিরতি;
		ক্ষেত্র 'z':
		ক্ষেত্র 'Z':
		  tval.set_bit(idx, BIT4_Z);
		  বিরতি;
		পূর্বনির্ধারিত:
		    /* Return "x" if there are invalid digits in the string. */
		  fprintf(stderr, "Warning: Invalid binary digit %c(%d) in "
		          "\"%s\".\n", *ebuf, *ebuf, buf);
		  জন্য (unsigned jdx = 0 ;  jdx < vec4.size() ;  jdx += 1) {
			vec4.set_bit(jdx, BIT4_X);
		  }
		  ফেরত;
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
      নির্বাচন (tval.value(tval.size()-1)) {
	  ক্ষেত্র BIT4_X:  // Pad MSB 'x' with 'x'
	    pad = BIT4_X;
	    বিরতি;
	  ক্ষেত্র BIT4_Z:  // Pad MSB 'z' with 'z'
	    pad = BIT4_Z;
	    বিরতি;
	  ক্ষেত্র BIT4_1:  // If negative pad MSB '1' with '1'
	    যদি (buf[0] == '-') {
		  pad = BIT4_1;
		  বিরতি;
	    }
	    // fallthrough
	  পূর্বনির্ধারিত:  // Everything else gets '0' padded/
	    pad = BIT4_0;
	    বিরতি;
      }

      জন্য (unsigned jdx = 0 ;  jdx < vec4.size() ; jdx += 1) {
	    যদি (jdx < tval.size()) vec4.set_bit(jdx, tval.value(jdx));
	    অন্যথা vec4.set_bit(jdx, pad);
      }
}
