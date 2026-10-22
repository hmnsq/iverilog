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


#ଯଦି SIZEOF_UNSIGNED_LONG * CHAR_BIT >= 64
#define BDIGITS 9
#define BASE 1000000000
#define BBITS 32
#define BMASK 0xffffffff
#ଅନ୍ୟଥା
#ଯଦି SIZEOF_UNSIGNED_LONG * CHAR_BIT >= 32
#define BDIGITS 4
#define BASE 10000
#define BBITS 16
#define BMASK 0xffff
#ଅନ୍ୟଥା
#error apparent non-conforming word length
#endif
#endif

#define B_IS0(x)  ((x) == 0)
#define B_IS1(x)  ((x) == 1)
#define B_ISX(x)  ((x) == 2)
#define B_ISZ(x)  ((x) == 3)

/* Jump through some hoops so we don't have to malloc/free valv
 * on every call, and implement an optional malloc-less version. */
ସ୍ଥିର unsigned long *valv=ଖାଲି;
ସ୍ଥିର unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା vlen_alloc=0;

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
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ shift_in(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା vlen, unsigned long val)
{
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
	/* printf("shift in %u\n",val); */
	ପାଇଁ (i=0; i<vlen; i++) {
		val=(valv[i]<<BBITS)+val;
		valv[i]=val%BASE;
		val=val/BASE;
	}
	ଯଦି (val!=0)
	      fprintf(stderr,"internal error: carry out %lu in " __FILE__ "\n",val);
}

/* Since BASE is a power of ten, conversion of each element of the
 * valv array to decimal is easy.  sprintf(buf,"%d",v) could be made
 * to work, I suppose, but for speed and control I prefer to write
 * the steps out longhand.
 */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା write_digits(unsigned long v, ଅକ୍ଷର **buf,
                               unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା *nbuf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା zero_suppress)
{
	ଅକ୍ଷର segment[BDIGITS];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
	ପାଇଁ (i=BDIGITS-1; i>=0; --i) {
		segment[i] = '0' + v%10;
		v=v/10;
	}
	ପାଇଁ (i=0; i<BDIGITS; ++i) {
		ଯଦି (!(zero_suppress&=(segment[i]=='0'))) {
			*(*buf)++=segment[i]; --(*nbuf);
		}
	}
	ଫେରନ୍ତୁ zero_suppress;
}

#ifdef CHECK_WITH_VALGRIND
ଶୂନ୍ୟ dec_str_delete(ଶୂନ୍ୟ)
{
      ମୁକ୍ତ(valv);
      valv = 0;
      vlen_alloc = 0;
}
#endif

unsigned vpip_vec4_to_dec_str(ଅଚଳ vvp_vector4_t&vec4,
			      ଅକ୍ଷର *buf, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା nbuf,
			      ପୂର୍ଣ୍ଣସଂଖ୍ୟା signed_flag)
{
      unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, vlen;
      unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା mbits=vec4.size();   /* number of non-sign bits */
      unsigned ଗଣନା_x = 0, ଗଣନା_z = 0;

      unsigned long val=0;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା comp=0;
      ଯଦି (signed_flag) {
	    ବାଛ (vec4.value(vec4.size()-1)) {
		ସ୍ଥିତି BIT4_X:
		  ଗଣନା_x += 1;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି BIT4_Z:
		  ଗଣନା_z += 1;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି BIT4_1:
		  comp=1;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି BIT4_0:
		  ଭାଙ୍ଗ;
	    }
	    mbits -= 1;
      }
      ପ୍ରତିଜ୍ଞା(mbits<(UINT_MAX-92)/28);
      vlen = ((mbits*28+92)/93+BDIGITS-1)/BDIGITS;
	/* printf("vlen=%d\n",vlen); */

#define ALLOC_MARGIN 4
      ଯଦି (!valv || vlen > vlen_alloc) {
	    ଯଦି (valv) ମୁକ୍ତ(valv);
	    valv = static_cast<unsigned long*>
	           (calloc(vlen+ALLOC_MARGIN, ଆକାର (*valv)));
	    vlen_alloc=vlen+ALLOC_MARGIN;
      } ଅନ୍ୟଥା {
	    memset(valv,0,vlen*ଆକାର(valv[0]));
      }

      ପାଇଁ (idx = 0; idx < mbits; idx += 1) {
	      /* printf("%c ",bits[mbits-idx-1]); */
	    ବାଛ (vec4.value(mbits-idx-1)) {
		ସ୍ଥିତି BIT4_Z:
		  ଗଣନା_z += 1;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି BIT4_X:
		  ଗଣନା_x += 1;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି BIT4_1:
		  ଯଦି (! comp)
			val += 1;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି BIT4_0:
		  ଯଦି (comp)
			val += 1;
		  ଭାଙ୍ଗ;
	    }

	    ଯଦି ((mbits-idx-1)%BBITS==0) {
		    /* make negative 2's complement, not 1's complement */
		  ଯଦି (comp && idx==mbits-1) ++val;
		  shift_in(vlen,val);
		  val=0;
	    } ଅନ୍ୟଥା {
		  val=val+val;
	    }
      }

	ଯଦି (ଗଣନା_x == vec4.size()) {
	      buf[0] = 'x';
	      buf[1] = 0;
	} ଅନ୍ୟଥା ଯଦି (ଗଣନା_x > 0) {
	      buf[0] = 'X';
	      buf[1] = 0;
	} ଅନ୍ୟଥା ଯଦି (ଗଣନା_z == vec4.size()) {
	      buf[0] = 'z';
	      buf[1] = 0;
	} ଅନ୍ୟଥା ଯଦି (ଗଣନା_z > 0) {
	      buf[0] = 'Z';
	      buf[1] = 0;
	} ଅନ୍ୟଥା {
	      ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
	      ପୂର୍ଣ୍ଣସଂଖ୍ୟା zero_suppress=1;
	      ଯଦି (comp) {
		    *buf++='-';
		    nbuf--;
		      /* printf("-"); */
	      }
	      ପାଇଁ (i=vlen-1; i>=0; i--) {
		    zero_suppress = write_digits(valv[i],
						 &buf,&nbuf,zero_suppress);
		      /* printf(",%.4u",valv[i]); */
	      }
		/* Awkward special case, since we don't want to
		 * zero suppress down to nothing at all.  The only
		 * way we can still have zero_suppress on in the
		 * comp=1 case is if mbits==0, and therefore vlen==0.
		 * We represent 1'sb1 as "-1". */
	      ଯଦି (zero_suppress) *buf++='0'+comp;
		/* printf("\n"); */
	      *buf='\0';
	}
	ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ vpip_dec_str_to_vec4(vvp_vector4_t&vec, ଅଚଳ ଅକ୍ଷର*buf)
{
	/* Support for [xX]_*. */
      ଯଦି (buf[0] == 'x' || buf[0] == 'X') {
	    ପାଇଁ (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vec.set_bit(idx, BIT4_X);
	    }
	    ଅଚଳ ଅକ୍ଷର*tbuf = buf+1;
	      /* See if this is a valid constant. */
	    ଯେତେବେଳେ (*tbuf) {
		  ଯଦି (*tbuf != '_') {
			fprintf(stderr, "Warning: Invalid decimal \"x\" "
			                "value \"%s\".\n", buf);
			ଫେରନ୍ତୁ;
		  }
		  tbuf += 1;
	    }
	    ଫେରନ୍ତୁ;
      }

	/* Support for [zZ]_*. */
      ଯଦି (buf[0] == 'z' || buf[0] == 'Z') {
	    ଅଚଳ ଅକ୍ଷର*tbuf = buf+1;
	      /* See if this is a valid constant, if not return "x". */
	    ଯେତେବେଳେ (*tbuf) {
		  ଯଦି (*tbuf != '_') {
			fprintf(stderr, "Warning: Invalid decimal \"z\" "
			                "value \"%s\".\n", buf);
			ପାଇଁ (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
			      vec.set_bit(idx, BIT4_X);
			}
			ଫେରନ୍ତୁ;
		  }
		  tbuf += 1;
	    }
	    ପାଇଁ (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vec.set_bit(idx, BIT4_Z);
	    }
	    ଫେରନ୍ତୁ;
      }

	/* The str string is the decimal value with the least
	   significant digit first. This loop creates that string by
	   reversing the order of the buf string. For example, if the
	   input is "1234", str gets "4321". */
      unsigned slen = strlen(buf);
      ଅକ୍ଷର*str = ନୂତନ ଅକ୍ଷର[slen + 1];
      ତାର୍କିକ is_negative = ମିଥ୍ୟା;
      ପାଇଁ (unsigned idx = 0 ;  idx < slen ;  idx += 1) {
	    ଯଦି (idx == slen-1 && buf[slen-idx-1] == '-') {
	          is_negative = ସତ୍ୟ;
		  slen--;
		  ଜାରି;
	    }
	    ଯେତେବେଳେ (buf[slen-idx-1] == '_') {
		  slen--;
	    }
	    ଯଦି (isdigit(buf[slen-idx-1]))
		  str[idx] = buf[slen-idx-1];
            ଅନ୍ୟଥା {
		    /* Return "x" if there are invalid digits in the string. */
		  fprintf(stderr, "Warning: Invalid decimal digit %c(%d) in "
		          "\"%s.\"\n", buf[slen-idx-1], buf[slen-idx-1], buf);
		  ପାଇଁ (unsigned jdx = 0 ;  jdx < vec.size() ;  jdx += 1) {
			vec.set_bit(jdx, BIT4_X);
		  }
		  ଫେରନ୍ତୁ;
            }
      }

      str[slen] = 0;

      ପାଇଁ (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
	    vvp_bit4_t val4 = BIT4_0;

	    ବାଛ (str[0]) {
		ସ୍ଥିତି '1':
		ସ୍ଥିତି '3':
		ସ୍ଥିତି '5':
		ସ୍ଥିତି '7':
		ସ୍ଥିତି '9':
		  val4 = BIT4_1;
		  ଭାଙ୍ଗ;
	    }

	    vec.set_bit(idx, val4);

	      /* Divide the str string by 2 in decimal. */
	    ଅକ୍ଷର*cp = str;
	    ଯେତେବେଳେ (*cp) {
		  unsigned val = cp[0] - '0';
		  ଯଦି ((val&1) && (cp > str))
			cp[-1] += 5;

		  cp[0] = '0' + val/2;
		  cp += 1;
	    }

      }

      ଯଦି (is_negative) {
            vec.invert();
            vec += (int64_t) 1;
      }

      delete[]str;
}
