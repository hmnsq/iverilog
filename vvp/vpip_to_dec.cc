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
#ifdef CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#endif
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


#ಯದಿ SIZEOF_UNSIGNED_LONG * CHAR_BIT >= 64
#define BDIGITS 9
#define BASE 1000000000
#define BBITS 32
#define BMASK 0xffffffff
#ಅನ್ಯಥಾ
#ಯದಿ SIZEOF_UNSIGNED_LONG * CHAR_BIT >= 32
#define BDIGITS 4
#define BASE 10000
#define BBITS 16
#define BMASK 0xffff
#ಅನ್ಯಥಾ
#error apparent non-conforming word length
#endif
#endif

#define B_IS0(x)  ((x) == 0)
#define B_IS1(x)  ((x) == 1)
#define B_ISX(x)  ((x) == 2)
#define B_ISZ(x)  ((x) == 3)

/* Jump through some hoops so we don't have to malloc/free valv
 * on every call, and implement an optional malloc-less version. */
ಸ್ಥಿರ unsigned long *valv=ರಿಕ್ತ;
ಸ್ಥಿರ unsigned ಪೂರ್ಣಾಙ್ಕ vlen_alloc=0;

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
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ shift_in(unsigned ಪೂರ್ಣಾಙ್ಕ vlen, unsigned long val)
{
	unsigned ಪೂರ್ಣಾಙ್ಕ i;
	/* printf("shift in %u\n",val); */
	ಪ್ರತಿ (i=0; i<vlen; i++) {
		val=(valv[i]<<BBITS)+val;
		valv[i]=val%BASE;
		val=val/BASE;
	}
	ಯದಿ (val!=0)
	      fprintf(stderr,"internal error: carry out %lu in " __FILE__ "\n",val);
}

/* Since BASE is a power of ten, conversion of each element of the
 * valv array to decimal is easy.  sprintf(buf,"%d",v) could be made
 * to work, I suppose, but for speed and control I prefer to write
 * the steps out longhand.
 */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ write_digits(unsigned long v, ವರ್ಣ **buf,
                               unsigned ಪೂರ್ಣಾಙ್ಕ *nbuf, ಪೂರ್ಣಾಙ್ಕ zero_suppress)
{
	ವರ್ಣ segment[BDIGITS];
	ಪೂರ್ಣಾಙ್ಕ i;
	ಪ್ರತಿ (i=BDIGITS-1; i>=0; --i) {
		segment[i] = '0' + v%10;
		v=v/10;
	}
	ಪ್ರತಿ (i=0; i<BDIGITS; ++i) {
		ಯದಿ (!(zero_suppress&=(segment[i]=='0'))) {
			*(*buf)++=segment[i]; --(*nbuf);
		}
	}
	ಪ್ರತ್ಯಾಗಮ zero_suppress;
}

#ifdef CHECK_WITH_VALGRIND
ಶೂನ್ಯ dec_str_delete(ಶೂನ್ಯ)
{
      ಮುಕ್ತ(valv);
      valv = 0;
      vlen_alloc = 0;
}
#endif

unsigned vpip_vec4_to_dec_str(ಧ್ರುವ vvp_vector4_t&vec4,
			      ವರ್ಣ *buf, unsigned ಪೂರ್ಣಾಙ್ಕ nbuf,
			      ಪೂರ್ಣಾಙ್ಕ signed_flag)
{
      unsigned ಪೂರ್ಣಾಙ್ಕ idx, vlen;
      unsigned ಪೂರ್ಣಾಙ್ಕ mbits=vec4.size();   /* number of non-sign bits */
      unsigned ಎಣಿಕೆ_x = 0, ಎಣಿಕೆ_z = 0;

      unsigned long val=0;
      ಪೂರ್ಣಾಙ್ಕ comp=0;
      ಯದಿ (signed_flag) {
	    ವಿಕಲ್ಪ (vec4.value(vec4.size()-1)) {
		ಪಕ್ಷ BIT4_X:
		  ಎಣಿಕೆ_x += 1;
		  ಭಙ್ಗ;
		ಪಕ್ಷ BIT4_Z:
		  ಎಣಿಕೆ_z += 1;
		  ಭಙ್ಗ;
		ಪಕ್ಷ BIT4_1:
		  comp=1;
		  ಭಙ್ಗ;
		ಪಕ್ಷ BIT4_0:
		  ಭಙ್ಗ;
	    }
	    mbits -= 1;
      }
      ಪ್ರತಿಜ್ಞಾ(mbits<(UINT_MAX-92)/28);
      vlen = ((mbits*28+92)/93+BDIGITS-1)/BDIGITS;
	/* printf("vlen=%d\n",vlen); */

#define ALLOC_MARGIN 4
      ಯದಿ (!valv || vlen > vlen_alloc) {
	    ಯದಿ (valv) ಮುಕ್ತ(valv);
	    valv = static_cast<unsigned long*>
	           (calloc(vlen+ALLOC_MARGIN, ಪರಿಮಾಣ (*valv)));
	    vlen_alloc=vlen+ALLOC_MARGIN;
      } ಅನ್ಯಥಾ {
	    memset(valv,0,vlen*ಪರಿಮಾಣ(valv[0]));
      }

      ಪ್ರತಿ (idx = 0; idx < mbits; idx += 1) {
	      /* printf("%c ",bits[mbits-idx-1]); */
	    ವಿಕಲ್ಪ (vec4.value(mbits-idx-1)) {
		ಪಕ್ಷ BIT4_Z:
		  ಎಣಿಕೆ_z += 1;
		  ಭಙ್ಗ;
		ಪಕ್ಷ BIT4_X:
		  ಎಣಿಕೆ_x += 1;
		  ಭಙ್ಗ;
		ಪಕ್ಷ BIT4_1:
		  ಯದಿ (! comp)
			val += 1;
		  ಭಙ್ಗ;
		ಪಕ್ಷ BIT4_0:
		  ಯದಿ (comp)
			val += 1;
		  ಭಙ್ಗ;
	    }

	    ಯದಿ ((mbits-idx-1)%BBITS==0) {
		    /* make negative 2's complement, not 1's complement */
		  ಯದಿ (comp && idx==mbits-1) ++val;
		  shift_in(vlen,val);
		  val=0;
	    } ಅನ್ಯಥಾ {
		  val=val+val;
	    }
      }

	ಯದಿ (ಎಣಿಕೆ_x == vec4.size()) {
	      buf[0] = 'x';
	      buf[1] = 0;
	} ಅನ್ಯಥಾ ಯದಿ (ಎಣಿಕೆ_x > 0) {
	      buf[0] = 'X';
	      buf[1] = 0;
	} ಅನ್ಯಥಾ ಯದಿ (ಎಣಿಕೆ_z == vec4.size()) {
	      buf[0] = 'z';
	      buf[1] = 0;
	} ಅನ್ಯಥಾ ಯದಿ (ಎಣಿಕೆ_z > 0) {
	      buf[0] = 'Z';
	      buf[1] = 0;
	} ಅನ್ಯಥಾ {
	      ಪೂರ್ಣಾಙ್ಕ i;
	      ಪೂರ್ಣಾಙ್ಕ zero_suppress=1;
	      ಯದಿ (comp) {
		    *buf++='-';
		    nbuf--;
		      /* printf("-"); */
	      }
	      ಪ್ರತಿ (i=vlen-1; i>=0; i--) {
		    zero_suppress = write_digits(valv[i],
						 &buf,&nbuf,zero_suppress);
		      /* printf(",%.4u",valv[i]); */
	      }
		/* Awkward special case, since we don't want to
		 * zero suppress down to nothing at all.  The only
		 * way we can still have zero_suppress on in the
		 * comp=1 case is if mbits==0, and therefore vlen==0.
		 * We represent 1'sb1 as "-1". */
	      ಯದಿ (zero_suppress) *buf++='0'+comp;
		/* printf("\n"); */
	      *buf='\0';
	}
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ vpip_dec_str_to_vec4(vvp_vector4_t&vec, ಧ್ರುವ ವರ್ಣ*buf)
{
	/* Support for [xX]_*. */
      ಯದಿ (buf[0] == 'x' || buf[0] == 'X') {
	    ಪ್ರತಿ (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vec.set_bit(idx, BIT4_X);
	    }
	    ಧ್ರುವ ವರ್ಣ*tbuf = buf+1;
	      /* See if this is a valid constant. */
	    ಯಾವತ್ (*tbuf) {
		  ಯದಿ (*tbuf != '_') {
			fprintf(stderr, "Warning: Invalid decimal \"x\" "
			                "value \"%s\".\n", buf);
			ಪ್ರತ್ಯಾಗಮ;
		  }
		  tbuf += 1;
	    }
	    ಪ್ರತ್ಯಾಗಮ;
      }

	/* Support for [zZ]_*. */
      ಯದಿ (buf[0] == 'z' || buf[0] == 'Z') {
	    ಧ್ರುವ ವರ್ಣ*tbuf = buf+1;
	      /* See if this is a valid constant, if not return "x". */
	    ಯಾವತ್ (*tbuf) {
		  ಯದಿ (*tbuf != '_') {
			fprintf(stderr, "Warning: Invalid decimal \"z\" "
			                "value \"%s\".\n", buf);
			ಪ್ರತಿ (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
			      vec.set_bit(idx, BIT4_X);
			}
			ಪ್ರತ್ಯಾಗಮ;
		  }
		  tbuf += 1;
	    }
	    ಪ್ರತಿ (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vec.set_bit(idx, BIT4_Z);
	    }
	    ಪ್ರತ್ಯಾಗಮ;
      }

	/* The str string is the decimal value with the least
	   significant digit first. This loop creates that string by
	   reversing the order of the buf string. For example, if the
	   input is "1234", str gets "4321". */
      unsigned slen = strlen(buf);
      ವರ್ಣ*str = ನೂತನ ವರ್ಣ[slen + 1];
      ತಾರ್ಕಿಕ is_negative = ಅಸತ್ಯ;
      ಪ್ರತಿ (unsigned idx = 0 ;  idx < slen ;  idx += 1) {
	    ಯದಿ (idx == slen-1 && buf[slen-idx-1] == '-') {
	          is_negative = ಸತ್ಯ;
		  slen--;
		  ಅನುವರ್ತ;
	    }
	    ಯಾವತ್ (buf[slen-idx-1] == '_') {
		  slen--;
	    }
	    ಯದಿ (isdigit(buf[slen-idx-1]))
		  str[idx] = buf[slen-idx-1];
            ಅನ್ಯಥಾ {
		    /* Return "x" if there are invalid digits in the string. */
		  fprintf(stderr, "Warning: Invalid decimal digit %c(%d) in "
		          "\"%s.\"\n", buf[slen-idx-1], buf[slen-idx-1], buf);
		  ಪ್ರತಿ (unsigned jdx = 0 ;  jdx < vec.size() ;  jdx += 1) {
			vec.set_bit(jdx, BIT4_X);
		  }
		  ಪ್ರತ್ಯಾಗಮ;
            }
      }

      str[slen] = 0;

      ಪ್ರತಿ (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
	    vvp_bit4_t val4 = BIT4_0;

	    ವಿಕಲ್ಪ (str[0]) {
		ಪಕ್ಷ '1':
		ಪಕ್ಷ '3':
		ಪಕ್ಷ '5':
		ಪಕ್ಷ '7':
		ಪಕ್ಷ '9':
		  val4 = BIT4_1;
		  ಭಙ್ಗ;
	    }

	    vec.set_bit(idx, val4);

	      /* Divide the str string by 2 in decimal. */
	    ವರ್ಣ*cp = str;
	    ಯಾವತ್ (*cp) {
		  unsigned val = cp[0] - '0';
		  ಯದಿ ((val&1) && (cp > str))
			cp[-1] += 5;

		  cp[0] = '0' + val/2;
		  cp += 1;
	    }

      }

      ಯದಿ (is_negative) {
            vec.invert();
            vec += (int64_t) 1;
      }

      delete[]str;
}
