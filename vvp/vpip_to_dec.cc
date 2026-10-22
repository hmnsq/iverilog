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


#যদি SIZEOF_UNSIGNED_LONG * CHAR_BIT >= 64
#define BDIGITS 9
#define BASE 1000000000
#define BBITS 32
#define BMASK 0xffffffff
#অন্যথা
#যদি SIZEOF_UNSIGNED_LONG * CHAR_BIT >= 32
#define BDIGITS 4
#define BASE 10000
#define BBITS 16
#define BMASK 0xffff
#অন্যথা
#error apparent non-conforming word length
#endif
#endif

#define B_IS0(x)  ((x) == 0)
#define B_IS1(x)  ((x) == 1)
#define B_ISX(x)  ((x) == 2)
#define B_ISZ(x)  ((x) == 3)

/* Jump through some hoops so we don't have to malloc/free valv
 * on every call, and implement an optional malloc-less version. */
স্থির unsigned long *valv=রিক্ত;
স্থির unsigned পূর্ণাঙ্ক vlen_alloc=0;

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
স্থির অন্তঃস্থ শূন্য shift_in(unsigned পূর্ণাঙ্ক vlen, unsigned long val)
{
	unsigned পূর্ণাঙ্ক i;
	/* printf("shift in %u\n",val); */
	বাবে (i=0; i<vlen; i++) {
		val=(valv[i]<<BBITS)+val;
		valv[i]=val%BASE;
		val=val/BASE;
	}
	যদি (val!=0)
	      fprintf(stderr,"internal error: carry out %lu in " __FILE__ "\n",val);
}

/* Since BASE is a power of ten, conversion of each element of the
 * valv array to decimal is easy.  sprintf(buf,"%d",v) could be made
 * to work, I suppose, but for speed and control I prefer to write
 * the steps out longhand.
 */
স্থির অন্তঃস্থ পূর্ণাঙ্ক write_digits(unsigned long v, ৱর্ণ **buf,
                               unsigned পূর্ণাঙ্ক *nbuf, পূর্ণাঙ্ক zero_suppress)
{
	ৱর্ণ segment[BDIGITS];
	পূর্ণাঙ্ক i;
	বাবে (i=BDIGITS-1; i>=0; --i) {
		segment[i] = '0' + v%10;
		v=v/10;
	}
	বাবে (i=0; i<BDIGITS; ++i) {
		যদি (!(zero_suppress&=(segment[i]=='0'))) {
			*(*buf)++=segment[i]; --(*nbuf);
		}
	}
	ঘূৰাই zero_suppress;
}

#ifdef CHECK_WITH_VALGRIND
শূন্য dec_str_delete(শূন্য)
{
      মুক্ত(valv);
      valv = 0;
      vlen_alloc = 0;
}
#endif

unsigned vpip_vec4_to_dec_str(ধ্রুৱ vvp_vector4_t&vec4,
			      ৱর্ণ *buf, unsigned পূর্ণাঙ্ক nbuf,
			      পূর্ণাঙ্ক signed_flag)
{
      unsigned পূর্ণাঙ্ক idx, vlen;
      unsigned পূর্ণাঙ্ক mbits=vec4.size();   /* number of non-sign bits */
      unsigned গুনতি_x = 0, গুনতি_z = 0;

      unsigned long val=0;
      পূর্ণাঙ্ক comp=0;
      যদি (signed_flag) {
	    ৱিকল্প (vec4.value(vec4.size()-1)) {
		পক্ষ BIT4_X:
		  গুনতি_x += 1;
		  ভঙ্গ;
		পক্ষ BIT4_Z:
		  গুনতি_z += 1;
		  ভঙ্গ;
		পক্ষ BIT4_1:
		  comp=1;
		  ভঙ্গ;
		পক্ষ BIT4_0:
		  ভঙ্গ;
	    }
	    mbits -= 1;
      }
      প্রতিজ্ঞা(mbits<(UINT_MAX-92)/28);
      vlen = ((mbits*28+92)/93+BDIGITS-1)/BDIGITS;
	/* printf("vlen=%d\n",vlen); */

#define ALLOC_MARGIN 4
      যদি (!valv || vlen > vlen_alloc) {
	    যদি (valv) মুক্ত(valv);
	    valv = static_cast<unsigned long*>
	           (calloc(vlen+ALLOC_MARGIN, পরিমাণ (*valv)));
	    vlen_alloc=vlen+ALLOC_MARGIN;
      } অন্যথা {
	    memset(valv,0,vlen*পরিমাণ(valv[0]));
      }

      বাবে (idx = 0; idx < mbits; idx += 1) {
	      /* printf("%c ",bits[mbits-idx-1]); */
	    ৱিকল্প (vec4.value(mbits-idx-1)) {
		পক্ষ BIT4_Z:
		  গুনতি_z += 1;
		  ভঙ্গ;
		পক্ষ BIT4_X:
		  গুনতি_x += 1;
		  ভঙ্গ;
		পক্ষ BIT4_1:
		  যদি (! comp)
			val += 1;
		  ভঙ্গ;
		পক্ষ BIT4_0:
		  যদি (comp)
			val += 1;
		  ভঙ্গ;
	    }

	    যদি ((mbits-idx-1)%BBITS==0) {
		    /* make negative 2's complement, not 1's complement */
		  যদি (comp && idx==mbits-1) ++val;
		  shift_in(vlen,val);
		  val=0;
	    } অন্যথা {
		  val=val+val;
	    }
      }

	যদি (গুনতি_x == vec4.size()) {
	      buf[0] = 'x';
	      buf[1] = 0;
	} অন্যথা যদি (গুনতি_x > 0) {
	      buf[0] = 'X';
	      buf[1] = 0;
	} অন্যথা যদি (গুনতি_z == vec4.size()) {
	      buf[0] = 'z';
	      buf[1] = 0;
	} অন্যথা যদি (গুনতি_z > 0) {
	      buf[0] = 'Z';
	      buf[1] = 0;
	} অন্যথা {
	      পূর্ণাঙ্ক i;
	      পূর্ণাঙ্ক zero_suppress=1;
	      যদি (comp) {
		    *buf++='-';
		    nbuf--;
		      /* printf("-"); */
	      }
	      বাবে (i=vlen-1; i>=0; i--) {
		    zero_suppress = write_digits(valv[i],
						 &buf,&nbuf,zero_suppress);
		      /* printf(",%.4u",valv[i]); */
	      }
		/* Awkward special case, since we don't want to
		 * zero suppress down to nothing at all.  The only
		 * way we can still have zero_suppress on in the
		 * comp=1 case is if mbits==0, and therefore vlen==0.
		 * We represent 1'sb1 as "-1". */
	      যদি (zero_suppress) *buf++='0'+comp;
		/* printf("\n"); */
	      *buf='\0';
	}
	ঘূৰাই 0;
}

শূন্য vpip_dec_str_to_vec4(vvp_vector4_t&vec, ধ্রুৱ ৱর্ণ*buf)
{
	/* Support for [xX]_*. */
      যদি (buf[0] == 'x' || buf[0] == 'X') {
	    বাবে (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vec.set_bit(idx, BIT4_X);
	    }
	    ধ্রুৱ ৱর্ণ*tbuf = buf+1;
	      /* See if this is a valid constant. */
	    যেতিয়ালৈ (*tbuf) {
		  যদি (*tbuf != '_') {
			fprintf(stderr, "Warning: Invalid decimal \"x\" "
			                "value \"%s\".\n", buf);
			ঘূৰাই;
		  }
		  tbuf += 1;
	    }
	    ঘূৰাই;
      }

	/* Support for [zZ]_*. */
      যদি (buf[0] == 'z' || buf[0] == 'Z') {
	    ধ্রুৱ ৱর্ণ*tbuf = buf+1;
	      /* See if this is a valid constant, if not return "x". */
	    যেতিয়ালৈ (*tbuf) {
		  যদি (*tbuf != '_') {
			fprintf(stderr, "Warning: Invalid decimal \"z\" "
			                "value \"%s\".\n", buf);
			বাবে (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
			      vec.set_bit(idx, BIT4_X);
			}
			ঘূৰাই;
		  }
		  tbuf += 1;
	    }
	    বাবে (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vec.set_bit(idx, BIT4_Z);
	    }
	    ঘূৰাই;
      }

	/* The str string is the decimal value with the least
	   significant digit first. This loop creates that string by
	   reversing the order of the buf string. For example, if the
	   input is "1234", str gets "4321". */
      unsigned slen = strlen(buf);
      ৱর্ণ*str = নূতন ৱর্ণ[slen + 1];
      তার্কিক is_negative = মিছা;
      বাবে (unsigned idx = 0 ;  idx < slen ;  idx += 1) {
	    যদি (idx == slen-1 && buf[slen-idx-1] == '-') {
	          is_negative = সঁচা;
		  slen--;
		  অনুৱর্ত;
	    }
	    যেতিয়ালৈ (buf[slen-idx-1] == '_') {
		  slen--;
	    }
	    যদি (isdigit(buf[slen-idx-1]))
		  str[idx] = buf[slen-idx-1];
            অন্যথা {
		    /* Return "x" if there are invalid digits in the string. */
		  fprintf(stderr, "Warning: Invalid decimal digit %c(%d) in "
		          "\"%s.\"\n", buf[slen-idx-1], buf[slen-idx-1], buf);
		  বাবে (unsigned jdx = 0 ;  jdx < vec.size() ;  jdx += 1) {
			vec.set_bit(jdx, BIT4_X);
		  }
		  ঘূৰাই;
            }
      }

      str[slen] = 0;

      বাবে (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
	    vvp_bit4_t val4 = BIT4_0;

	    ৱিকল্প (str[0]) {
		পক্ষ '1':
		পক্ষ '3':
		পক্ষ '5':
		পক্ষ '7':
		পক্ষ '9':
		  val4 = BIT4_1;
		  ভঙ্গ;
	    }

	    vec.set_bit(idx, val4);

	      /* Divide the str string by 2 in decimal. */
	    ৱর্ণ*cp = str;
	    যেতিয়ালৈ (*cp) {
		  unsigned val = cp[0] - '0';
		  যদি ((val&1) && (cp > str))
			cp[-1] += 5;

		  cp[0] = '0' + val/2;
		  cp += 1;
	    }

      }

      যদি (is_negative) {
            vec.invert();
            vec += (int64_t) 1;
      }

      delete[]str;
}
