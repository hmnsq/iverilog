/*
 * Copyright (c) 2008-2026 Stephen Williams <steve@icarus.com>
 * Copyright (c) 2002 Larry Doolittle (larry@doolittle.boa.org)
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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#ಆದರೆ_ಅಂತ್ಯ
# include  <cstdio>
# include  <cstring>
# include  <climits>     /* for CHAR_BIT */
# include  <cstdlib>
# include  <cctype>
# include  <cassert>
# include  "ivl_alloc.h"

/* If you are allergic to malloc, you can set a stack memory allocation
 * here.  Otherwise, malloc() is used for the temporary array, so the
 * conversion length is unlimited. */
/* #define MAX_DIGITS 20 */


#ಆದರೆ SIZEOF_UNSIGNED_LONG * CHAR_BIT >= 64
#ವ್ಯಾಖ್ಯಾನಿಸು BDIGITS 9
#ವ್ಯಾಖ್ಯಾನಿಸು ತಳ_3 1000000000
#ವ್ಯಾಖ್ಯಾನಿಸು BBITS 32
#ವ್ಯಾಖ್ಯಾನಿಸು BMASK 0xffffffff
#ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ SIZEOF_UNSIGNED_LONG * CHAR_BIT >= 32
#ವ್ಯಾಖ್ಯಾನಿಸು BDIGITS 4
#ವ್ಯಾಖ್ಯಾನಿಸು ತಳ_3 10000
#ವ್ಯಾಖ್ಯಾನಿಸು BBITS 16
#ವ್ಯಾಖ್ಯಾನಿಸು BMASK 0xffff
#ಇಲ್ಲದಿದ್ದರೆ
#ದೋಷ apparent non-conforming ಶಬ್ದ length
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು B_IS0(x)  ((x) == 0)
#ವ್ಯಾಖ್ಯಾನಿಸು B_IS1(x)  ((x) == 1)
#ವ್ಯಾಖ್ಯಾನಿಸು B_ISX(x)  ((x) == 2)
#ವ್ಯಾಖ್ಯಾನಿಸು B_ISZ(x)  ((x) == 3)

/* Jump through some hoops so we don't have to malloc/free valv
 * on every call, and implement an optional malloc-less version. */
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *valv=ಖಾಲಿ;
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ vlen_alloc=0;

/* The program works by building a base BASE representation of the number
 * in the valv array.  BBITS bits of the number can be put in at a time.
 * Previous values of each valv element are always less than BASE, the
 * input val is less than or equal to 2^BBITS, so (valv[i]<<BBITS)+val
 * is guaranteed less than or equal to BASE<<BBITS, which is configured
 * less than ULONG_MAX.  When this number divided by BASE, to get the amount
 * propagated as a "carry" to the next array element, the result is again
 * less than or equal to 2^BBITS.  BBITS and BASE are configured above
 * to depend on the "unsigned long" length of the host, for efficiency.
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ ಸರಿಸು_ಒಳ(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ vlen, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ val)
{
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ i;
	/* printf("shift in %u\n",val); */
	ಗಾಗಿ (i=0; i<vlen; i++) {
		val=(valv[i]<<BBITS)+val;
		valv[i]=val%ತಳ_3;
		val=val/ತಳ_3;
	}
	ಆದರೆ (val!=0)
	      fprintf(stderr,"internal error: carry out %lu in " __FILE__ "\n",val);
}

/* Since BASE is a power of ten, conversion of each element of the
 * valv array to decimal is easy.  sprintf(buf,"%d",v) could be made
 * to work, I suppose, but for speed and control I prefer to write
 * the steps out longhand.
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ write_digits(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ v, ಅಕ್ಷರ **buf,
                               ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ *nbuf, ಪೂರ್ಣಾಂಕ zero_suppress)
{
	ಅಕ್ಷರ segment[BDIGITS];
	ಪೂರ್ಣಾಂಕ i;
	ಗಾಗಿ (i=BDIGITS-1; i>=0; --i) {
		segment[i] = '0' + v%10;
		v=v/10;
	}
	ಗಾಗಿ (i=0; i<BDIGITS; ++i) {
		ಆದರೆ (!(zero_suppress&=(segment[i]=='0'))) {
			*(*buf)++=segment[i]; --(*nbuf);
		}
	}
	ಹಿಂತಿರುಗಿ zero_suppress;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಶೂನ್ಯ dec_str_delete(ಶೂನ್ಯ)
{
      ಬಿಡುಗಡೆ(valv);
      valv = 0;
      vlen_alloc = 0;
}
#ಆದರೆ_ಅಂತ್ಯ

ಚಿಹ್ನೆರಹಿತ vpip_vec4_ಗಮ್ಯ_dec_str(ಸ್ಥಿರಾಂಕ vvp_vector4_t&vec4,
			      ಅಕ್ಷರ *buf, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ nbuf,
			      ಪೂರ್ಣಾಂಕ signed_flag)
{
      ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ idx, vlen;
      ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ mbits=vec4.size();   /* number of non-sign bits */
      ಚಿಹ್ನೆರಹಿತ ಎಣಿಕೆ_x = 0, ಎಣಿಕೆ_z = 0;

      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ val=0;
      ಪೂರ್ಣಾಂಕ comp=0;
      ಆದರೆ (signed_flag) {
	    ಆಯ್ಕೆ (vec4.value(vec4.size()-1)) {
		ಸ್ಥಿತಿ BIT4_X:
		  ಎಣಿಕೆ_x += 1;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BIT4_Z:
		  ಎಣಿಕೆ_z += 1;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BIT4_1:
		  comp=1;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BIT4_0:
		  ನಿಲ್ಲಿಸು;
	    }
	    mbits -= 1;
      }
      ಪ್ರತಿಜ್ಞಾ(mbits<(UINT_MAX-92)/28);
      vlen = ((mbits*28+92)/93+BDIGITS-1)/BDIGITS;
	/* printf("vlen=%d\n",vlen); */

#ವ್ಯಾಖ್ಯಾನಿಸು ALLOC_MARGIN 4
      ಆದರೆ (!valv || vlen > vlen_alloc) {
	    ಆದರೆ (valv) ಬಿಡುಗಡೆ(valv);
	    valv = static_cast<ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ*>
	           (ಸೊನ್ನೆ_ಸ್ಮರಣೆ_ಕೊಡು(vlen+ALLOC_MARGIN, ಗಾತ್ರ (*valv)));
	    vlen_alloc=vlen+ALLOC_MARGIN;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    memset(valv,0,vlen*ಗಾತ್ರ(valv[0]));
      }

      ಗಾಗಿ (idx = 0; idx < mbits; idx += 1) {
	      /* printf("%c ",bits[mbits-idx-1]); */
	    ಆಯ್ಕೆ (vec4.value(mbits-idx-1)) {
		ಸ್ಥಿತಿ BIT4_Z:
		  ಎಣಿಕೆ_z += 1;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BIT4_X:
		  ಎಣಿಕೆ_x += 1;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BIT4_1:
		  ಆದರೆ (! comp)
			val += 1;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BIT4_0:
		  ಆದರೆ (comp)
			val += 1;
		  ನಿಲ್ಲಿಸು;
	    }

	    ಆದರೆ ((mbits-idx-1)%BBITS==0) {
		    /* make negative 2's complement, not 1's complement */
		  ಆದರೆ (comp && idx==mbits-1) ++val;
		  ಸರಿಸು_ಒಳ(vlen,val);
		  val=0;
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  val=val+val;
	    }
      }

	ಆದರೆ (ಎಣಿಕೆ_x == vec4.size()) {
	      buf[0] = 'x';
	      buf[1] = 0;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಎಣಿಕೆ_x > 0) {
	      buf[0] = 'X';
	      buf[1] = 0;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಎಣಿಕೆ_z == vec4.size()) {
	      buf[0] = 'z';
	      buf[1] = 0;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಎಣಿಕೆ_z > 0) {
	      buf[0] = 'Z';
	      buf[1] = 0;
	} ಇಲ್ಲದಿದ್ದರೆ {
	      ಪೂರ್ಣಾಂಕ i;
	      ಪೂರ್ಣಾಂಕ zero_suppress=1;
	      ಆದರೆ (comp) {
		    *buf++='-';
		    nbuf--;
		      /* printf("-"); */
	      }
	      ಗಾಗಿ (i=vlen-1; i>=0; i--) {
		    zero_suppress = write_digits(valv[i],
						 &buf,&nbuf,zero_suppress);
		      /* printf(",%.4u",valv[i]); */
	      }
		/* Awkward special case, since we don't want to
		 * zero suppress down to nothing at all.  The only
		 * way we can still have zero_suppress on in the
		 * comp=1 case is if mbits==0, and therefore vlen==0.
		 * We represent 1'sb1 as "-1". */
	      ಆದರೆ (zero_suppress) *buf++='0'+comp;
		/* printf("\n"); */
	      *buf='\0';
	}
	ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ vpip_dec_str_ಗಮ್ಯ_vec4(vvp_vector4_t&vec, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*buf)
{
	/* Support for [xX]_*. */
      ಆದರೆ (buf[0] == 'x' || buf[0] == 'X') {
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vec.ಗಣ_ಬಿಟ್(idx, BIT4_X);
	    }
	    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*tbuf = buf+1;
	      /* See if this is a valid constant. */
	    ಆಗ (*tbuf) {
		  ಆದರೆ (*tbuf != '_') {
			fprintf(stderr, "Warning: Invalid decimal \"x\" "
			                "value \"%s\".\n", buf);
			ಹಿಂತಿರುಗಿ;
		  }
		  tbuf += 1;
	    }
	    ಹಿಂತಿರುಗಿ;
      }

	/* Support for [zZ]_*. */
      ಆದರೆ (buf[0] == 'z' || buf[0] == 'Z') {
	    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*tbuf = buf+1;
	      /* See if this is a valid constant, if not return "x". */
	    ಆಗ (*tbuf) {
		  ಆದರೆ (*tbuf != '_') {
			fprintf(stderr, "Warning: Invalid decimal \"z\" "
			                "value \"%s\".\n", buf);
			ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < vec.size() ;  idx += 1) {
			      vec.ಗಣ_ಬಿಟ್(idx, BIT4_X);
			}
			ಹಿಂತಿರುಗಿ;
		  }
		  tbuf += 1;
	    }
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vec.ಗಣ_ಬಿಟ್(idx, BIT4_Z);
	    }
	    ಹಿಂತಿರುಗಿ;
      }

	/* The str string is the decimal value with the least
	   significant digit first. This loop creates that string by
	   reversing the order of the buf string. For example, if the
	   input is "1234", str gets "4321". */
      ಚಿಹ್ನೆರಹಿತ slen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(buf);
      ಅಕ್ಷರ*str = ಹೊಸ ಅಕ್ಷರ[slen + 1];
      ತಾರ್ಕಿಕ is_negative = ಸುಳ್ಳು;
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < slen ;  idx += 1) {
	    ಆದರೆ (idx == slen-1 && buf[slen-idx-1] == '-') {
	          is_negative = ನಿಜ;
		  slen--;
		  ಮುಂದುವರಿ;
	    }
	    ಆಗ (buf[slen-idx-1] == '_') {
		  slen--;
	    }
	    ಆದರೆ (ಅಂಕಿಯೇ(buf[slen-idx-1]))
		  str[idx] = buf[slen-idx-1];
            ಇಲ್ಲದಿದ್ದರೆ {
		    /* Return "x" if there are invalid digits in the string. */
		  fprintf(stderr, "Warning: Invalid decimal digit %c(%d) in "
		          "\"%s.\"\n", buf[slen-idx-1], buf[slen-idx-1], buf);
		  ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ jdx = 0 ;  jdx < vec.size() ;  jdx += 1) {
			vec.ಗಣ_ಬಿಟ್(jdx, BIT4_X);
		  }
		  ಹಿಂತಿರುಗಿ;
            }
      }

      str[slen] = 0;

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < vec.size() ;  idx += 1) {
	    vvp_bit4_t val4 = BIT4_0;

	    ಆಯ್ಕೆ (str[0]) {
		ಸ್ಥಿತಿ '1':
		ಸ್ಥಿತಿ '3':
		ಸ್ಥಿತಿ '5':
		ಸ್ಥಿತಿ '7':
		ಸ್ಥಿತಿ '9':
		  val4 = BIT4_1;
		  ನಿಲ್ಲಿಸು;
	    }

	    vec.ಗಣ_ಬಿಟ್(idx, val4);

	      /* Divide the str string by 2 in decimal. */
	    ಅಕ್ಷರ*cp = str;
	    ಆಗ (*cp) {
		  ಚಿಹ್ನೆರಹಿತ val = cp[0] - '0';
		  ಆದರೆ ((val&1) && (cp > str))
			cp[-1] += 5;

		  cp[0] = '0' + val/2;
		  cp += 1;
	    }

      }

      ಆದರೆ (is_negative) {
            vec.invert();
            vec += (int64_t) 1;
      }

      ಅಳಿಸು[]str;
}
