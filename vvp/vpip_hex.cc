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

ਬਾਹਰੀ ਅਚਲ ਅੱਖਰ hex_digits[256];

ਖਾਲੀ vpip_hex_str_ਮੰਜ਼ਿਲ_vec4(vvp_vector4_t&val, ਅਚਲ ਅੱਖਰ*str)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ str_len = ਲੜੀ_ਲੰਬਾਈ(str);
      ਬਿਨਾਨਿਸ਼ਾਨੀ skip_chars = 0;
      ਅਚਲ ਅੱਖਰ*tstr = str;
	/* Find the number of non-numeric characters. */
      ਜਦੋਂ ((tstr = strpbrk(tstr, "-_"))) {
	    skip_chars += 1;
	    tstr += 1;
      }
      vvp_vector4_t tval(4*(str_len-skip_chars));
      skip_chars = 0;
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < tval.size() ;  idx += 1) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ tmp;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਦੋਆਧਾਰੀ_ਅੰਕ_off = idx%4;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ str_off = idx/4;
	    ਅੱਖਰ ch;

	    ਪ੍ਰਤਿਜ੍ਞਾ (str_off+skip_chars < str_len);
	      /* Skip any "_" characters in the string. */
	    ਜਦੋਂ((ch = str[str_len-str_off-1-skip_chars]) == '_') {
		  skip_chars += 1;
		  ਪ੍ਰਤਿਜ੍ਞਾ (str_off+skip_chars < str_len);
	    }

	      /* If we find a "-" it must be at the head of the string. */
	    ਜੇ (ch == '-') ਪ੍ਰਤਿਜ੍ਞਾ(0);

	    ਚੋਣ (ch) {
		ਹਾਲਤ '0':
		ਹਾਲਤ '1':
		ਹਾਲਤ '2':
		ਹਾਲਤ '3':
		ਹਾਲਤ '4':
		ਹਾਲਤ '5':
		ਹਾਲਤ '6':
		ਹਾਲਤ '7':
		ਹਾਲਤ '8':
		ਹਾਲਤ '9':
		  tmp = ch - '0';
		  tval.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, ((tmp>>ਦੋਆਧਾਰੀ_ਅੰਕ_off)&1)? BIT4_1 : BIT4_0);
		  ਰੋਕੋ;
		ਹਾਲਤ 'a':
		ਹਾਲਤ 'b':
		ਹਾਲਤ 'c':
		ਹਾਲਤ 'd':
		ਹਾਲਤ 'e':
		ਹਾਲਤ 'f':
		  tmp = ch - 'a' + 10;
		  tval.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, ((tmp>>ਦੋਆਧਾਰੀ_ਅੰਕ_off)&1)? BIT4_1 : BIT4_0);
		  ਰੋਕੋ;
		ਹਾਲਤ 'A':
		ਹਾਲਤ 'B':
		ਹਾਲਤ 'C':
		ਹਾਲਤ 'D':
		ਹਾਲਤ 'E':
		ਹਾਲਤ 'F':
		  tmp = ch - 'A' + 10;
		  tval.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, ((tmp>>ਦੋਆਧਾਰੀ_ਅੰਕ_off)&1)? BIT4_1 : BIT4_0);
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
		  fprintf(stderr, "Warning: Invalid hex digit %c(%d) in "
		          "\"%s\".\n", ch, ch, str);
		  ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ jdx = 0 ;  jdx < val.size() ;  jdx += 1) {
			val.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(jdx, BIT4_X);
		  }
		  ਵਾਪਸ;
	    }
      }

	/* Make a negative value when needed. */
      ਜੇ (str[0] == '-') {
	    tval.invert();
	    tval += (int64_t) 1;
      }

	/* Find the correct padding value. */
      vvp_bit4_t pad;
      ਚੋਣ (tval.value(tval.size()-1)) {
	  ਹਾਲਤ BIT4_X:  // Pad MSB 'x' with 'x'.
	    pad = BIT4_X;
	    ਰੋਕੋ;
	  ਹਾਲਤ BIT4_Z:  // Pad MSB 'z' with 'z'.
	    pad = BIT4_Z;
	    ਰੋਕੋ;
	  ਹਾਲਤ BIT4_1:  // If negative pad MSB '1' with '1'.
	    ਜੇ (str[0] == '-') {
		  pad = BIT4_1;
		  ਰੋਕੋ;
	    }
	    // fallthrough
	  ਮੂਲ:  // Everything else gets '0' padded.
	    pad = BIT4_0;
	    ਰੋਕੋ;
      }

	/* Copy the temporary value to the real value, padding if needed. */
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < val.size() ;  idx += 1) {
	    ਜੇ (idx < tval.size()) val.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, tval.value(idx));
	    ਨਹੀਂਤਾਂ val.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, pad);
      }
}

ਖਾਲੀ vpip_vec4_ਮੰਜ਼ਿਲ_hex_str(ਅਚਲ vvp_vector4_t&ਬਿੱਟ, ਅੱਖਰ*buf, ਬਿਨਾਨਿਸ਼ਾਨੀ nbuf)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ slen = (ਬਿੱਟ.size() + 3) / 4;
      ਪ੍ਰਤਿਜ੍ਞਾ(slen < nbuf);

      buf[slen] = 0;

      ਬਿਨਾਨਿਸ਼ਾਨੀ val = 0;
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < ਬਿੱਟ.size() ;  idx += 1) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ vs = (idx%4) * 2;

	    ਚੋਣ (ਬਿੱਟ.value(idx)) {
		ਹਾਲਤ BIT4_0:
		  ਰੋਕੋ;
		ਹਾਲਤ BIT4_1:
		  val |= 1 << vs;
		  ਰੋਕੋ;
		ਹਾਲਤ BIT4_X:
		  val |= 2 << vs;
		  ਰੋਕੋ;
		ਹਾਲਤ BIT4_Z:
		  val |= 3 << vs;
	    }

	    ਜੇ (vs == 6) {
		  slen -= 1;
		  buf[slen] = hex_digits[val];
		  val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      ਚੋਣ (ਬਿੱਟ.size() % 4) {
	  ਹਾਲਤ 1:
	    ਜੇ (val == 2) val = 170;
	    ਨਹੀਂਤਾਂ ਜੇ (val == 3) val = 255;
	    ਰੋਕੋ;
	  ਹਾਲਤ 2:
	    ਜੇ (val == 10) val = 170;
	    ਨਹੀਂਤਾਂ ਜੇ (val == 15) val = 255;
	    ਰੋਕੋ;
	  ਹਾਲਤ 3:
	    ਜੇ (val == 42) val = 170;
	    ਨਹੀਂਤਾਂ ਜੇ (val == 63) val = 255;
	    ਰੋਕੋ;
      }

      ਜੇ (slen > 0) {
	    slen -= 1;
	    buf[slen] = hex_digits[val];
      }
}
