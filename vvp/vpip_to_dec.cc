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


#எனில் SIZEOF_UNSIGNED_LONG * CHAR_BIT >= 64
#define BDIGITS 9
#define BASE 1000000000
#define BBITS 32
#define BMASK 0xffffffff
#இல்லையெனில்
#எனில் SIZEOF_UNSIGNED_LONG * CHAR_BIT >= 32
#define BDIGITS 4
#define BASE 10000
#define BBITS 16
#define BMASK 0xffff
#இல்லையெனில்
#error apparent non-conforming word length
#endif
#endif

#define B_IS0(x)  ((x) == 0)
#define B_IS1(x)  ((x) == 1)
#define B_ISX(x)  ((x) == 2)
#define B_ISZ(x)  ((x) == 3)

/* Jump through some hoops so we don't have to malloc/free valv
 * on every call, and implement an optional malloc-less version. */
நிலையான unsigned long *valv=வெற்று;
நிலையான unsigned முழு vlen_alloc=0;

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
நிலையான உள்வரி வெறுமை shift_in(unsigned முழு vlen, unsigned long val)
{
	unsigned முழு i;
	/* printf("shift in %u\n",val); */
	க்கு (i=0; i<vlen; i++) {
		val=(valv[i]<<BBITS)+val;
		valv[i]=val%BASE;
		val=val/BASE;
	}
	எனில் (val!=0)
	      fprintf(stderr,"internal error: carry out %lu in " __FILE__ "\n",val);
}

/* Since BASE is a power of ten, conversion of each element of the
 * valv array to decimal is easy.  sprintf(buf,"%d",v) could be made
 * to work, I suppose, but for speed and control I prefer to write
 * the steps out longhand.
 */
நிலையான உள்வரி முழு write_digits(unsigned long v, எழுத்து **buf,
                               unsigned முழு *nbuf, முழு zero_suppress)
{
	எழுத்து segment[BDIGITS];
	முழு i;
	க்கு (i=BDIGITS-1; i>=0; --i) {
		segment[i] = '0' + v%10;
		v=v/10;
	}
	க்கு (i=0; i<BDIGITS; ++i) {
		எனில் (!(zero_suppress&=(segment[i]=='0'))) {
			*(*buf)++=segment[i]; --(*nbuf);
		}
	}
	திரும்பு zero_suppress;
}

#ifdef CHECK_WITH_VALGRIND
வெறுமை dec_str_delete(வெறுமை)
{
      விடுவி(valv);
      valv = 0;
      vlen_alloc = 0;
}
#endif

unsigned vpip_vec4_to_dec_str(மாறிலி vvp_vector4_t&vec4,
			      எழுத்து *buf, unsigned முழு nbuf,
			      முழு signed_flag)
{
      unsigned முழு idx, vlen;
      unsigned முழு mbits=vec4.size();   /* number of non-sign bits */
      unsigned மொத்தம்_x = 0, மொத்தம்_z = 0;

      unsigned long val=0;
      முழு comp=0;
      எனில் (signed_flag) {
	    தேர்வு (vec4.value(vec4.size()-1)) {
		நிலை BIT4_X:
		  மொத்தம்_x += 1;
		  நிறுத்து;
		நிலை BIT4_Z:
		  மொத்தம்_z += 1;
		  நிறுத்து;
		நிலை BIT4_1:
		  comp=1;
		  நிறுத்து;
		நிலை BIT4_0:
		  நிறுத்து;
	    }
	    mbits -= 1;
      }
      ப்ரதிஜ்ஞா(mbits<(UINT_MAX-92)/28);
      vlen = ((mbits*28+92)/93+BDIGITS-1)/BDIGITS;
	/* printf("vlen=%d\n",vlen); */

#define ALLOC_MARGIN 4
      எனில் (!valv || vlen > vlen_alloc) {
	    எனில் (valv) விடுவி(valv);
	    valv = static_cast<unsigned long*>
	           (calloc(vlen+ALLOC_MARGIN, அளவு (*valv)));
	    vlen_alloc=vlen+ALLOC_MARGIN;
      } இல்லையெனில் {
	    memset(valv,0,vlen*அளவு(valv[0]));
      }

      க்கு (idx = 0; idx < mbits; idx += 1) {
	      /* printf("%c ",bits[mbits-idx-1]); */
	    தேர்வு (vec4.value(mbits-idx-1)) {
		நிலை BIT4_Z:
		  மொத்தம்_z += 1;
		  நிறுத்து;
		நிலை BIT4_X:
		  மொத்தம்_x += 1;
		  நிறுத்து;
		நிலை BIT4_1:
		  எனில் (! comp)
			val += 1;
		  நிறுத்து;
		நிலை BIT4_0:
		  எனில் (comp)
			val += 1;
		  நிறுத்து;
	    }

	    எனில் ((mbits-idx-1)%BBITS==0) {
		    /* make negative 2's complement, not 1's complement */
		  எனில் (comp && idx==mbits-1) ++val;
		  shift_in(vlen,val);
		  val=0;
	    } இல்லையெனில் {
		  val=val+val;
	    }
      }

	எனில் (மொத்தம்_x == vec4.size()) {
	      buf[0] = 'x';
	      buf[1] = 0;
	} இல்லையெனில் எனில் (மொத்தம்_x > 0) {
	      buf[0] = 'X';
	      buf[1] = 0;
	} இல்லையெனில் எனில் (மொத்தம்_z == vec4.size()) {
	      buf[0] = 'z';
	      buf[1] = 0;
	} இல்லையெனில் எனில் (மொத்தம்_z > 0) {
	      buf[0] = 'Z';
	      buf[1] = 0;
	} இல்லையெனில் {
	      முழு i;
	      முழு zero_suppress=1;
	      எனில் (comp) {
		    *buf++='-';
		    nbuf--;
		      /* printf("-"); */
	      }
	      க்கு (i=vlen-1; i>=0; i--) {
		    zero_suppress = write_digits(valv[i],
						 &buf,&nbuf,zero_suppress);
		      /* printf(",%.4u",valv[i]); */
	      }
		/* Awkward special case, since we don't want to
		 * zero suppress down to nothing at all.  The only
		 * way we can still have zero_suppress on in the
		 * comp=1 case is if mbits==0, and therefore vlen==0.
		 * We represent 1'sb1 as "-1". */
	      எனில் (zero_suppress) *buf++='0'+comp;
		/* printf("\n"); */
	      *buf='\0';
	}
	திரும்பு 0;
}

வெறுமை vpip_dec_str_to_vec4(vvp_vector4_t&vec, மாறிலி எழுத்து*buf)
{
	/* Support for [xX]_*. */
      எனில் (buf[0] == 'x' || buf[0] == 'X') {
	    க்கு (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vec.set_bit(idx, BIT4_X);
	    }
	    மாறிலி எழுத்து*tbuf = buf+1;
	      /* See if this is a valid constant. */
	    போது (*tbuf) {
		  எனில் (*tbuf != '_') {
			fprintf(stderr, "Warning: Invalid decimal \"x\" "
			                "value \"%s\".\n", buf);
			திரும்பு;
		  }
		  tbuf += 1;
	    }
	    திரும்பு;
      }

	/* Support for [zZ]_*. */
      எனில் (buf[0] == 'z' || buf[0] == 'Z') {
	    மாறிலி எழுத்து*tbuf = buf+1;
	      /* See if this is a valid constant, if not return "x". */
	    போது (*tbuf) {
		  எனில் (*tbuf != '_') {
			fprintf(stderr, "Warning: Invalid decimal \"z\" "
			                "value \"%s\".\n", buf);
			க்கு (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
			      vec.set_bit(idx, BIT4_X);
			}
			திரும்பு;
		  }
		  tbuf += 1;
	    }
	    க்கு (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vec.set_bit(idx, BIT4_Z);
	    }
	    திரும்பு;
      }

	/* The str string is the decimal value with the least
	   significant digit first. This loop creates that string by
	   reversing the order of the buf string. For example, if the
	   input is "1234", str gets "4321". */
      unsigned slen = strlen(buf);
      எழுத்து*str = புதிய எழுத்து[slen + 1];
      தருக்க is_negative = பொய்;
      க்கு (unsigned idx = 0 ;  idx < slen ;  idx += 1) {
	    எனில் (idx == slen-1 && buf[slen-idx-1] == '-') {
	          is_negative = உண்மை;
		  slen--;
		  தொடர்;
	    }
	    போது (buf[slen-idx-1] == '_') {
		  slen--;
	    }
	    எனில் (isdigit(buf[slen-idx-1]))
		  str[idx] = buf[slen-idx-1];
            இல்லையெனில் {
		    /* Return "x" if there are invalid digits in the string. */
		  fprintf(stderr, "Warning: Invalid decimal digit %c(%d) in "
		          "\"%s.\"\n", buf[slen-idx-1], buf[slen-idx-1], buf);
		  க்கு (unsigned jdx = 0 ;  jdx < vec.size() ;  jdx += 1) {
			vec.set_bit(jdx, BIT4_X);
		  }
		  திரும்பு;
            }
      }

      str[slen] = 0;

      க்கு (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
	    vvp_bit4_t val4 = BIT4_0;

	    தேர்வு (str[0]) {
		நிலை '1':
		நிலை '3':
		நிலை '5':
		நிலை '7':
		நிலை '9':
		  val4 = BIT4_1;
		  நிறுத்து;
	    }

	    vec.set_bit(idx, val4);

	      /* Divide the str string by 2 in decimal. */
	    எழுத்து*cp = str;
	    போது (*cp) {
		  unsigned val = cp[0] - '0';
		  எனில் ((val&1) && (cp > str))
			cp[-1] += 5;

		  cp[0] = '0' + val/2;
		  cp += 1;
	    }

      }

      எனில் (is_negative) {
            vec.invert();
            vec += (int64_t) 1;
      }

      delete[]str;
}
