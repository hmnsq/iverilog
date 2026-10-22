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

ಶೂನ್ಯ vpip_bin_str_ಗಮ್ಯ_vec4(vvp_vector4_t&vec4, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*buf)
{
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*ebuf = buf + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(buf);
      ಚಿಹ್ನೆರಹಿತ idx = 0;
      ಚಿಹ್ನೆರಹಿತ skip_chars = 0;
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*tbuf = buf;
	/* Find the number of non-numeric characters. */
      ಆಗ ((tbuf = strpbrk(tbuf, "-_"))) {
	    skip_chars += 1;
	    tbuf += 1;
      }
      vvp_vector4_t tval(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(buf)-skip_chars);
      ಆಗ (ebuf > buf) {
	    ebuf -= 1;
	      /* Skip any "_" characters in the string. */
	    ಆಗ (*ebuf == '_') {
		  ebuf -= 1;
		  ಪ್ರತಿಜ್ಞಾ(ebuf > buf);
	    }

	      /*  If we find a "-" it must be at the head of the string. */
	    ಆದರೆ (*ebuf == '-') {
		  ಆದರೆ (ebuf != buf) ಪ್ರತಿಜ್ಞಾ(0);
		  ನಿಲ್ಲಿಸು;
	    }

	    ಪ್ರತಿಜ್ಞಾ(idx < tval.size());
	    ಆಯ್ಕೆ (*ebuf) {
		ಸ್ಥಿತಿ '0':
		  tval.ಗಣ_ಬಿಟ್(idx, BIT4_0);
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ '1':
		  tval.ಗಣ_ಬಿಟ್(idx, BIT4_1);
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'x':
		ಸ್ಥಿತಿ 'X':
		  tval.ಗಣ_ಬಿಟ್(idx, BIT4_X);
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'z':
		ಸ್ಥಿತಿ 'Z':
		  tval.ಗಣ_ಬಿಟ್(idx, BIT4_Z);
		  ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
		    /* Return "x" if there are invalid digits in the string. */
		  fprintf(stderr, "Warning: Invalid binary digit %c(%d) in "
		          "\"%s\".\n", *ebuf, *ebuf, buf);
		  ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ jdx = 0 ;  jdx < vec4.size() ;  jdx += 1) {
			vec4.ಗಣ_ಬಿಟ್(jdx, BIT4_X);
		  }
		  ಹಿಂತಿರುಗಿ;
	    }

	    idx += 1;
      }

	/* Make a negative value when needed. */
      ಆದರೆ (buf[0] == '-') {
	    tval.invert();
	    tval += (int64_t) 1;
      }

	/* Find the correct padding value. */
      vvp_bit4_t pad;
      ಆಯ್ಕೆ (tval.value(tval.size()-1)) {
	  ಸ್ಥಿತಿ BIT4_X:  // Pad MSB 'x' with 'x'
	    pad = BIT4_X;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ BIT4_Z:  // Pad MSB 'z' with 'z'
	    pad = BIT4_Z;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ BIT4_1:  // If negative pad MSB '1' with '1'
	    ಆದರೆ (buf[0] == '-') {
		  pad = BIT4_1;
		  ನಿಲ್ಲಿಸು;
	    }
	    // fallthrough
	  ಪೂರ್ವನಿಯೋಜಿತ:  // Everything else gets '0' padded/
	    pad = BIT4_0;
	    ನಿಲ್ಲಿಸು;
      }

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ jdx = 0 ;  jdx < vec4.size() ; jdx += 1) {
	    ಆದರೆ (jdx < tval.size()) vec4.ಗಣ_ಬಿಟ್(jdx, tval.value(jdx));
	    ಇಲ್ಲದಿದ್ದರೆ vec4.ಗಣ_ಬಿಟ್(jdx, pad);
      }
}
