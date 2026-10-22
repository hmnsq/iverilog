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

శూన్య vpip_bin_str_గమ్యం_vec4(vvp_vector4_t&vec4, స్థిరరాశి అక్షరం*buf)
{
      స్థిరరాశి అక్షరం*ebuf = buf + అక్షరశ్రేణి_పొడవు(buf);
      గుర్తులేని idx = 0;
      గుర్తులేని skip_chars = 0;
      స్థిరరాశి అక్షరం*tbuf = buf;
	/* Find the number of non-numeric characters. */
      అయితే ((tbuf = strpbrk(tbuf, "-_"))) {
	    skip_chars += 1;
	    tbuf += 1;
      }
      vvp_vector4_t tval(అక్షరశ్రేణి_పొడవు(buf)-skip_chars);
      అయితే (ebuf > buf) {
	    ebuf -= 1;
	      /* Skip any "_" characters in the string. */
	    అయితే (*ebuf == '_') {
		  ebuf -= 1;
		  ప్రతిజ్ఞా(ebuf > buf);
	    }

	      /*  If we find a "-" it must be at the head of the string. */
	    ఒకవేళ (*ebuf == '-') {
		  ఒకవేళ (ebuf != buf) ప్రతిజ్ఞా(0);
		  ఆపు;
	    }

	    ప్రతిజ్ఞా(idx < tval.size());
	    ఎంపిక (*ebuf) {
		స్థితి '0':
		  tval.సమితి_బిట్టు(idx, BIT4_0);
		  ఆపు;
		స్థితి '1':
		  tval.సమితి_బిట్టు(idx, BIT4_1);
		  ఆపు;
		స్థితి 'x':
		స్థితి 'X':
		  tval.సమితి_బిట్టు(idx, BIT4_X);
		  ఆపు;
		స్థితి 'z':
		స్థితి 'Z':
		  tval.సమితి_బిట్టు(idx, BIT4_Z);
		  ఆపు;
		అప్రమేయ:
		    /* Return "x" if there are invalid digits in the string. */
		  fprintf(stderr, "Warning: Invalid binary digit %c(%d) in "
		          "\"%s\".\n", *ebuf, *ebuf, buf);
		  కొరకు (గుర్తులేని jdx = 0 ;  jdx < vec4.size() ;  jdx += 1) {
			vec4.సమితి_బిట్టు(jdx, BIT4_X);
		  }
		  తిరిగి;
	    }

	    idx += 1;
      }

	/* Make a negative value when needed. */
      ఒకవేళ (buf[0] == '-') {
	    tval.invert();
	    tval += (int64_t) 1;
      }

	/* Find the correct padding value. */
      vvp_bit4_t pad;
      ఎంపిక (tval.value(tval.size()-1)) {
	  స్థితి BIT4_X:  // Pad MSB 'x' with 'x'
	    pad = BIT4_X;
	    ఆపు;
	  స్థితి BIT4_Z:  // Pad MSB 'z' with 'z'
	    pad = BIT4_Z;
	    ఆపు;
	  స్థితి BIT4_1:  // If negative pad MSB '1' with '1'
	    ఒకవేళ (buf[0] == '-') {
		  pad = BIT4_1;
		  ఆపు;
	    }
	    // fallthrough
	  అప్రమేయ:  // Everything else gets '0' padded/
	    pad = BIT4_0;
	    ఆపు;
      }

      కొరకు (గుర్తులేని jdx = 0 ;  jdx < vec4.size() ; jdx += 1) {
	    ఒకవేళ (jdx < tval.size()) vec4.సమితి_బిట్టు(jdx, tval.value(jdx));
	    లేకపోతే vec4.సమితి_బిట్టు(jdx, pad);
      }
}
