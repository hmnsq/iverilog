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

ਖਾਲੀ vpip_bin_str_ਮੰਜ਼ਿਲ_vec4(vvp_vector4_t&vec4, ਅਚਲ ਅੱਖਰ*buf)
{
      ਅਚਲ ਅੱਖਰ*ebuf = buf + ਲੜੀ_ਲੰਬਾਈ(buf);
      ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0;
      ਬਿਨਾਨਿਸ਼ਾਨੀ skip_chars = 0;
      ਅਚਲ ਅੱਖਰ*tbuf = buf;
	/* Find the number of non-numeric characters. */
      ਜਦੋਂ ((tbuf = strpbrk(tbuf, "-_"))) {
	    skip_chars += 1;
	    tbuf += 1;
      }
      vvp_vector4_t tval(ਲੜੀ_ਲੰਬਾਈ(buf)-skip_chars);
      ਜਦੋਂ (ebuf > buf) {
	    ebuf -= 1;
	      /* Skip any "_" characters in the string. */
	    ਜਦੋਂ (*ebuf == '_') {
		  ebuf -= 1;
		  ਪ੍ਰਤਿਜ੍ਞਾ(ebuf > buf);
	    }

	      /*  If we find a "-" it must be at the head of the string. */
	    ਜੇ (*ebuf == '-') {
		  ਜੇ (ebuf != buf) ਪ੍ਰਤਿਜ੍ਞਾ(0);
		  ਰੋਕੋ;
	    }

	    ਪ੍ਰਤਿਜ੍ਞਾ(idx < tval.size());
	    ਚੋਣ (*ebuf) {
		ਹਾਲਤ '0':
		  tval.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_0);
		  ਰੋਕੋ;
		ਹਾਲਤ '1':
		  tval.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_1);
		  ਰੋਕੋ;
		ਹਾਲਤ 'x':
		ਹਾਲਤ 'X':
		  tval.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_X);
		  ਰੋਕੋ;
		ਹਾਲਤ 'z':
		ਹਾਲਤ 'Z':
		  tval.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_Z);
		  ਰੋਕੋ;
		ਮੂਲ:
		    /* Return "x" if there are invalid digits in the string. */
		  fprintf(stderr, "Warning: Invalid binary digit %c(%d) in "
		          "\"%s\".\n", *ebuf, *ebuf, buf);
		  ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ jdx = 0 ;  jdx < vec4.size() ;  jdx += 1) {
			vec4.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(jdx, BIT4_X);
		  }
		  ਵਾਪਸ;
	    }

	    idx += 1;
      }

	/* Make a negative value when needed. */
      ਜੇ (buf[0] == '-') {
	    tval.invert();
	    tval += (int64_t) 1;
      }

	/* Find the correct padding value. */
      vvp_bit4_t pad;
      ਚੋਣ (tval.value(tval.size()-1)) {
	  ਹਾਲਤ BIT4_X:  // Pad MSB 'x' with 'x'
	    pad = BIT4_X;
	    ਰੋਕੋ;
	  ਹਾਲਤ BIT4_Z:  // Pad MSB 'z' with 'z'
	    pad = BIT4_Z;
	    ਰੋਕੋ;
	  ਹਾਲਤ BIT4_1:  // If negative pad MSB '1' with '1'
	    ਜੇ (buf[0] == '-') {
		  pad = BIT4_1;
		  ਰੋਕੋ;
	    }
	    // fallthrough
	  ਮੂਲ:  // Everything else gets '0' padded/
	    pad = BIT4_0;
	    ਰੋਕੋ;
      }

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ jdx = 0 ;  jdx < vec4.size() ; jdx += 1) {
	    ਜੇ (jdx < tval.size()) vec4.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(jdx, tval.value(jdx));
	    ਨਹੀਂਤਾਂ vec4.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(jdx, pad);
      }
}
