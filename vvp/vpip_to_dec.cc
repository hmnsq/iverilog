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


#सचे SIZEOF_UNSIGNED_LONG * CHAR_BIT >= 64
#define BDIGITS 9
#define BASE 1000000000
#define BBITS 32
#define BMASK 0xffffffff
#अञ्ञथा
#सचे SIZEOF_UNSIGNED_LONG * CHAR_BIT >= 32
#define BDIGITS 4
#define BASE 10000
#define BBITS 16
#define BMASK 0xffff
#अञ्ञथा
#error apparent non-conforming word length
#endif
#endif

#define B_IS0(x)  ((x) == 0)
#define B_IS1(x)  ((x) == 1)
#define B_ISX(x)  ((x) == 2)
#define B_ISZ(x)  ((x) == 3)

/* Jump through some hoops so we don't have to malloc/free valv
 * on every call, and implement an optional malloc-less version. */
स्थिर unsigned long *valv=रिक्त;
स्थिर unsigned पूर्णाङ्क vlen_alloc=0;

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
स्थिर अन्तःस्थ शून्य shift_in(unsigned पूर्णाङ्क vlen, unsigned long val)
{
	unsigned पूर्णाङ्क i;
	/* printf("shift in %u\n",val); */
	अत्थाय (i=0; i<vlen; i++) {
		val=(valv[i]<<BBITS)+val;
		valv[i]=val%BASE;
		val=val/BASE;
	}
	सचे (val!=0)
	      fprintf(stderr,"internal error: carry out %lu in " __FILE__ "\n",val);
}

/* Since BASE is a power of ten, conversion of each element of the
 * valv array to decimal is easy.  sprintf(buf,"%d",v) could be made
 * to work, I suppose, but for speed and control I prefer to write
 * the steps out longhand.
 */
स्थिर अन्तःस्थ पूर्णाङ्क write_digits(unsigned long v, वर्ण **buf,
                               unsigned पूर्णाङ्क *nbuf, पूर्णाङ्क zero_suppress)
{
	वर्ण segment[BDIGITS];
	पूर्णाङ्क i;
	अत्थाय (i=BDIGITS-1; i>=0; --i) {
		segment[i] = '0' + v%10;
		v=v/10;
	}
	अत्थाय (i=0; i<BDIGITS; ++i) {
		सचे (!(zero_suppress&=(segment[i]=='0'))) {
			*(*buf)++=segment[i]; --(*nbuf);
		}
	}
	पटिनिवत्त zero_suppress;
}

#ifdef CHECK_WITH_VALGRIND
शून्य dec_str_delete(शून्य)
{
      मुक्त(valv);
      valv = 0;
      vlen_alloc = 0;
}
#endif

unsigned vpip_vec4_to_dec_str(ध्रुव vvp_vector4_t&vec4,
			      वर्ण *buf, unsigned पूर्णाङ्क nbuf,
			      पूर्णाङ्क signed_flag)
{
      unsigned पूर्णाङ्क idx, vlen;
      unsigned पूर्णाङ्क mbits=vec4.size();   /* number of non-sign bits */
      unsigned गणनम्_x = 0, गणनम्_z = 0;

      unsigned long val=0;
      पूर्णाङ्क comp=0;
      सचे (signed_flag) {
	    विकल्प (vec4.value(vec4.size()-1)) {
		पक्ष BIT4_X:
		  गणनम्_x += 1;
		  भङ्ग;
		पक्ष BIT4_Z:
		  गणनम्_z += 1;
		  भङ्ग;
		पक्ष BIT4_1:
		  comp=1;
		  भङ्ग;
		पक्ष BIT4_0:
		  भङ्ग;
	    }
	    mbits -= 1;
      }
      प्रतिज्ञा(mbits<(UINT_MAX-92)/28);
      vlen = ((mbits*28+92)/93+BDIGITS-1)/BDIGITS;
	/* printf("vlen=%d\n",vlen); */

#define ALLOC_MARGIN 4
      सचे (!valv || vlen > vlen_alloc) {
	    सचे (valv) मुक्त(valv);
	    valv = static_cast<unsigned long*>
	           (calloc(vlen+ALLOC_MARGIN, परिमाण (*valv)));
	    vlen_alloc=vlen+ALLOC_MARGIN;
      } अञ्ञथा {
	    memset(valv,0,vlen*परिमाण(valv[0]));
      }

      अत्थाय (idx = 0; idx < mbits; idx += 1) {
	      /* printf("%c ",bits[mbits-idx-1]); */
	    विकल्प (vec4.value(mbits-idx-1)) {
		पक्ष BIT4_Z:
		  गणनम्_z += 1;
		  भङ्ग;
		पक्ष BIT4_X:
		  गणनम्_x += 1;
		  भङ्ग;
		पक्ष BIT4_1:
		  सचे (! comp)
			val += 1;
		  भङ्ग;
		पक्ष BIT4_0:
		  सचे (comp)
			val += 1;
		  भङ्ग;
	    }

	    सचे ((mbits-idx-1)%BBITS==0) {
		    /* make negative 2's complement, not 1's complement */
		  सचे (comp && idx==mbits-1) ++val;
		  shift_in(vlen,val);
		  val=0;
	    } अञ्ञथा {
		  val=val+val;
	    }
      }

	सचे (गणनम्_x == vec4.size()) {
	      buf[0] = 'x';
	      buf[1] = 0;
	} अञ्ञथा सचे (गणनम्_x > 0) {
	      buf[0] = 'X';
	      buf[1] = 0;
	} अञ्ञथा सचे (गणनम्_z == vec4.size()) {
	      buf[0] = 'z';
	      buf[1] = 0;
	} अञ्ञथा सचे (गणनम्_z > 0) {
	      buf[0] = 'Z';
	      buf[1] = 0;
	} अञ्ञथा {
	      पूर्णाङ्क i;
	      पूर्णाङ्क zero_suppress=1;
	      सचे (comp) {
		    *buf++='-';
		    nbuf--;
		      /* printf("-"); */
	      }
	      अत्थाय (i=vlen-1; i>=0; i--) {
		    zero_suppress = write_digits(valv[i],
						 &buf,&nbuf,zero_suppress);
		      /* printf(",%.4u",valv[i]); */
	      }
		/* Awkward special case, since we don't want to
		 * zero suppress down to nothing at all.  The only
		 * way we can still have zero_suppress on in the
		 * comp=1 case is if mbits==0, and therefore vlen==0.
		 * We represent 1'sb1 as "-1". */
	      सचे (zero_suppress) *buf++='0'+comp;
		/* printf("\n"); */
	      *buf='\0';
	}
	पटिनिवत्त 0;
}

शून्य vpip_dec_str_to_vec4(vvp_vector4_t&vec, ध्रुव वर्ण*buf)
{
	/* Support for [xX]_*. */
      सचे (buf[0] == 'x' || buf[0] == 'X') {
	    अत्थाय (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vec.set_bit(idx, BIT4_X);
	    }
	    ध्रुव वर्ण*tbuf = buf+1;
	      /* See if this is a valid constant. */
	    याव (*tbuf) {
		  सचे (*tbuf != '_') {
			fprintf(stderr, "Warning: Invalid decimal \"x\" "
			                "value \"%s\".\n", buf);
			पटिनिवत्त;
		  }
		  tbuf += 1;
	    }
	    पटिनिवत्त;
      }

	/* Support for [zZ]_*. */
      सचे (buf[0] == 'z' || buf[0] == 'Z') {
	    ध्रुव वर्ण*tbuf = buf+1;
	      /* See if this is a valid constant, if not return "x". */
	    याव (*tbuf) {
		  सचे (*tbuf != '_') {
			fprintf(stderr, "Warning: Invalid decimal \"z\" "
			                "value \"%s\".\n", buf);
			अत्थाय (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
			      vec.set_bit(idx, BIT4_X);
			}
			पटिनिवत्त;
		  }
		  tbuf += 1;
	    }
	    अत्थाय (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vec.set_bit(idx, BIT4_Z);
	    }
	    पटिनिवत्त;
      }

	/* The str string is the decimal value with the least
	   significant digit first. This loop creates that string by
	   reversing the order of the buf string. For example, if the
	   input is "1234", str gets "4321". */
      unsigned slen = strlen(buf);
      वर्ण*str = नूतन वर्ण[slen + 1];
      तार्किक is_negative = मिच्छा;
      अत्थाय (unsigned idx = 0 ;  idx < slen ;  idx += 1) {
	    सचे (idx == slen-1 && buf[slen-idx-1] == '-') {
	          is_negative = सच्च;
		  slen--;
		  अनुवर्त;
	    }
	    याव (buf[slen-idx-1] == '_') {
		  slen--;
	    }
	    सचे (isdigit(buf[slen-idx-1]))
		  str[idx] = buf[slen-idx-1];
            अञ्ञथा {
		    /* Return "x" if there are invalid digits in the string. */
		  fprintf(stderr, "Warning: Invalid decimal digit %c(%d) in "
		          "\"%s.\"\n", buf[slen-idx-1], buf[slen-idx-1], buf);
		  अत्थाय (unsigned jdx = 0 ;  jdx < vec.size() ;  jdx += 1) {
			vec.set_bit(jdx, BIT4_X);
		  }
		  पटिनिवत्त;
            }
      }

      str[slen] = 0;

      अत्थाय (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
	    vvp_bit4_t val4 = BIT4_0;

	    विकल्प (str[0]) {
		पक्ष '1':
		पक्ष '3':
		पक्ष '5':
		पक्ष '7':
		पक्ष '9':
		  val4 = BIT4_1;
		  भङ्ग;
	    }

	    vec.set_bit(idx, val4);

	      /* Divide the str string by 2 in decimal. */
	    वर्ण*cp = str;
	    याव (*cp) {
		  unsigned val = cp[0] - '0';
		  सचे ((val&1) && (cp > str))
			cp[-1] += 5;

		  cp[0] = '0' + val/2;
		  cp += 1;
	    }

      }

      सचे (is_negative) {
            vec.invert();
            vec += (int64_t) 1;
      }

      delete[]str;
}
