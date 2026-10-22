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

extern const var_na hex_digits[256];

void vpip_hex_str_to_vec4(vvp_vector4_t&val, const var_na*str)
{
      unsigned str_len = strlen(str);
      unsigned skip_chars = 0;
      const var_na*tstr = str;
	/* Find the number of non-numeric characters. */
      ᮞᮨᮓᮨᮜ ((tstr = strpbrk(tstr, "-_"))) {
	    skip_chars += 1;
	    tstr += 1;
      }
      vvp_vector4_t tval(4*(str_len-skip_chars));
      skip_chars = 0;
      for (unsigned idx = 0 ;  idx < tval.size() ;  idx += 1) {
	    unsigned tmp;
	    unsigned bit_off = idx%4;
	    unsigned str_off = idx/4;
	    var_na ch;

	    assert (str_off+skip_chars < str_len);
	      /* Skip any "_" characters in the string. */
	    ᮞᮨᮓᮨᮜ((ch = str[str_len-str_off-1-skip_chars]) == '_') {
		  skip_chars += 1;
		  assert (str_off+skip_chars < str_len);
	    }

	      /* If we find a "-" it must be at the head of the string. */
	    ᮚᮦᮔ᮪ (ch == '-') assert(0);

	    switch (ch) {
		pak_sa '0':
		pak_sa '1':
		pak_sa '2':
		pak_sa '3':
		pak_sa '4':
		pak_sa '5':
		pak_sa '6':
		pak_sa '7':
		pak_sa '8':
		pak_sa '9':
		  tmp = ch - '0';
		  tval.set_bit(idx, ((tmp>>bit_off)&1)? BIT4_1 : BIT4_0);
		  bha_nga;
		pak_sa 'a':
		pak_sa 'b':
		pak_sa 'c':
		pak_sa 'd':
		pak_sa 'e':
		pak_sa 'f':
		  tmp = ch - 'a' + 10;
		  tval.set_bit(idx, ((tmp>>bit_off)&1)? BIT4_1 : BIT4_0);
		  bha_nga;
		pak_sa 'A':
		pak_sa 'B':
		pak_sa 'C':
		pak_sa 'D':
		pak_sa 'E':
		pak_sa 'F':
		  tmp = ch - 'A' + 10;
		  tval.set_bit(idx, ((tmp>>bit_off)&1)? BIT4_1 : BIT4_0);
		  bha_nga;
		pak_sa 'x':
		pak_sa 'X':
		  tval.set_bit(idx, BIT4_X);
		  bha_nga;
		pak_sa 'z':
		pak_sa 'Z':
		  tval.set_bit(idx, BIT4_Z);
		  bha_nga;
		default:
		    /* Return "x" if there are invalid digits in the string. */
		  fprintf(stderr, "Warning: Invalid hex digit %c(%d) in "
		          "\"%s\".\n", ch, ch, str);
		  for (unsigned jdx = 0 ;  jdx < val.size() ;  jdx += 1) {
			val.set_bit(jdx, BIT4_X);
		  }
		  ᮝᮜᮤ;
	    }
      }

	/* Make a negative value when needed. */
      ᮚᮦᮔ᮪ (str[0] == '-') {
	    tval.invert();
	    tval += (int64_t) 1;
      }

	/* Find the correct padding value. */
      vvp_bit4_t pad;
      switch (tval.value(tval.size()-1)) {
	  pak_sa BIT4_X:  // Pad MSB 'x' with 'x'.
	    pad = BIT4_X;
	    bha_nga;
	  pak_sa BIT4_Z:  // Pad MSB 'z' with 'z'.
	    pad = BIT4_Z;
	    bha_nga;
	  pak_sa BIT4_1:  // If negative pad MSB '1' with '1'.
	    ᮚᮦᮔ᮪ (str[0] == '-') {
		  pad = BIT4_1;
		  bha_nga;
	    }
	    // fallthrough
	  default:  // Everything else gets '0' padded.
	    pad = BIT4_0;
	    bha_nga;
      }

	/* Copy the temporary value to the real value, padding if needed. */
      for (unsigned idx = 0 ;  idx < val.size() ;  idx += 1) {
	    ᮚᮦᮔ᮪ (idx < tval.size()) val.set_bit(idx, tval.value(idx));
	    ᮜᮤᮚ val.set_bit(idx, pad);
      }
}

void vpip_vec4_to_hex_str(const vvp_vector4_t&bits, var_na*buf, unsigned nbuf)
{
      unsigned slen = (bits.size() + 3) / 4;
      assert(slen < nbuf);

      buf[slen] = 0;

      unsigned val = 0;
      for (unsigned idx = 0 ;  idx < bits.size() ;  idx += 1) {
	    unsigned vs = (idx%4) * 2;

	    switch (bits.value(idx)) {
		pak_sa BIT4_0:
		  bha_nga;
		pak_sa BIT4_1:
		  val |= 1 << vs;
		  bha_nga;
		pak_sa BIT4_X:
		  val |= 2 << vs;
		  bha_nga;
		pak_sa BIT4_Z:
		  val |= 3 << vs;
	    }

	    ᮚᮦᮔ᮪ (vs == 6) {
		  slen -= 1;
		  buf[slen] = hex_digits[val];
		  val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      switch (bits.size() % 4) {
	  pak_sa 1:
	    ᮚᮦᮔ᮪ (val == 2) val = 170;
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (val == 3) val = 255;
	    bha_nga;
	  pak_sa 2:
	    ᮚᮦᮔ᮪ (val == 10) val = 170;
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (val == 15) val = 255;
	    bha_nga;
	  pak_sa 3:
	    ᮚᮦᮔ᮪ (val == 42) val = 170;
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (val == 63) val = 255;
	    bha_nga;
      }

      ᮚᮦᮔ᮪ (slen > 0) {
	    slen -= 1;
	    buf[slen] = hex_digits[val];
      }
}
