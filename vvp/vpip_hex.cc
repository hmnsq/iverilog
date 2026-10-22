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

బాహ్య స్థిరరాశి అక్షరం hex_digits[256];

శూన్య vpip_hex_str_గమ్యం_vec4(vvp_vector4_t&val, స్థిరరాశి అక్షరం*str)
{
      గుర్తులేని str_len = అక్షరశ్రేణి_పొడవు(str);
      గుర్తులేని skip_chars = 0;
      స్థిరరాశి అక్షరం*tstr = str;
	/* Find the number of non-numeric characters. */
      అయితే ((tstr = strpbrk(tstr, "-_"))) {
	    skip_chars += 1;
	    tstr += 1;
      }
      vvp_vector4_t tval(4*(str_len-skip_chars));
      skip_chars = 0;
      కొరకు (గుర్తులేని idx = 0 ;  idx < tval.size() ;  idx += 1) {
	    గుర్తులేని tmp;
	    గుర్తులేని బిట్టు_off = idx%4;
	    గుర్తులేని str_off = idx/4;
	    అక్షరం ch;

	    ప్రతిజ్ఞా (str_off+skip_chars < str_len);
	      /* Skip any "_" characters in the string. */
	    అయితే((ch = str[str_len-str_off-1-skip_chars]) == '_') {
		  skip_chars += 1;
		  ప్రతిజ్ఞా (str_off+skip_chars < str_len);
	    }

	      /* If we find a "-" it must be at the head of the string. */
	    ఒకవేళ (ch == '-') ప్రతిజ్ఞా(0);

	    ఎంపిక (ch) {
		స్థితి '0':
		స్థితి '1':
		స్థితి '2':
		స్థితి '3':
		స్థితి '4':
		స్థితి '5':
		స్థితి '6':
		స్థితి '7':
		స్థితి '8':
		స్థితి '9':
		  tmp = ch - '0';
		  tval.సమితి_బిట్టు(idx, ((tmp>>బిట్టు_off)&1)? BIT4_1 : BIT4_0);
		  ఆపు;
		స్థితి 'a':
		స్థితి 'b':
		స్థితి 'c':
		స్థితి 'd':
		స్థితి 'e':
		స్థితి 'f':
		  tmp = ch - 'a' + 10;
		  tval.సమితి_బిట్టు(idx, ((tmp>>బిట్టు_off)&1)? BIT4_1 : BIT4_0);
		  ఆపు;
		స్థితి 'A':
		స్థితి 'B':
		స్థితి 'C':
		స్థితి 'D':
		స్థితి 'E':
		స్థితి 'F':
		  tmp = ch - 'A' + 10;
		  tval.సమితి_బిట్టు(idx, ((tmp>>బిట్టు_off)&1)? BIT4_1 : BIT4_0);
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
		  fprintf(stderr, "Warning: Invalid hex digit %c(%d) in "
		          "\"%s\".\n", ch, ch, str);
		  కొరకు (గుర్తులేని jdx = 0 ;  jdx < val.size() ;  jdx += 1) {
			val.సమితి_బిట్టు(jdx, BIT4_X);
		  }
		  తిరిగి;
	    }
      }

	/* Make a negative value when needed. */
      ఒకవేళ (str[0] == '-') {
	    tval.invert();
	    tval += (int64_t) 1;
      }

	/* Find the correct padding value. */
      vvp_bit4_t pad;
      ఎంపిక (tval.value(tval.size()-1)) {
	  స్థితి BIT4_X:  // Pad MSB 'x' with 'x'.
	    pad = BIT4_X;
	    ఆపు;
	  స్థితి BIT4_Z:  // Pad MSB 'z' with 'z'.
	    pad = BIT4_Z;
	    ఆపు;
	  స్థితి BIT4_1:  // If negative pad MSB '1' with '1'.
	    ఒకవేళ (str[0] == '-') {
		  pad = BIT4_1;
		  ఆపు;
	    }
	    // fallthrough
	  అప్రమేయ:  // Everything else gets '0' padded.
	    pad = BIT4_0;
	    ఆపు;
      }

	/* Copy the temporary value to the real value, padding if needed. */
      కొరకు (గుర్తులేని idx = 0 ;  idx < val.size() ;  idx += 1) {
	    ఒకవేళ (idx < tval.size()) val.సమితి_బిట్టు(idx, tval.value(idx));
	    లేకపోతే val.సమితి_బిట్టు(idx, pad);
      }
}

శూన్య vpip_vec4_గమ్యం_hex_str(స్థిరరాశి vvp_vector4_t&బిట్లు, అక్షరం*buf, గుర్తులేని nbuf)
{
      గుర్తులేని slen = (బిట్లు.size() + 3) / 4;
      ప్రతిజ్ఞా(slen < nbuf);

      buf[slen] = 0;

      గుర్తులేని val = 0;
      కొరకు (గుర్తులేని idx = 0 ;  idx < బిట్లు.size() ;  idx += 1) {
	    గుర్తులేని vs = (idx%4) * 2;

	    ఎంపిక (బిట్లు.value(idx)) {
		స్థితి BIT4_0:
		  ఆపు;
		స్థితి BIT4_1:
		  val |= 1 << vs;
		  ఆపు;
		స్థితి BIT4_X:
		  val |= 2 << vs;
		  ఆపు;
		స్థితి BIT4_Z:
		  val |= 3 << vs;
	    }

	    ఒకవేళ (vs == 6) {
		  slen -= 1;
		  buf[slen] = hex_digits[val];
		  val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      ఎంపిక (బిట్లు.size() % 4) {
	  స్థితి 1:
	    ఒకవేళ (val == 2) val = 170;
	    లేకపోతే ఒకవేళ (val == 3) val = 255;
	    ఆపు;
	  స్థితి 2:
	    ఒకవేళ (val == 10) val = 170;
	    లేకపోతే ఒకవేళ (val == 15) val = 255;
	    ఆపు;
	  స్థితి 3:
	    ఒకవేళ (val == 42) val = 170;
	    లేకపోతే ఒకవేళ (val == 63) val = 255;
	    ఆపు;
      }

      ఒకవేళ (slen > 0) {
	    slen -= 1;
	    buf[slen] = hex_digits[val];
      }
}
