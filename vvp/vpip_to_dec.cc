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


#ᮚᮦᮔ᮪ SIZEOF_UNSIGNED_LONG * CHAR_BIT >= 64
#define BDIGITS 9
#define BASE 1000000000
#define BBITS 32
#define BMASK 0xffffffff
#ᮜᮤᮚ
#ᮚᮦᮔ᮪ SIZEOF_UNSIGNED_LONG * CHAR_BIT >= 32
#define BDIGITS 4
#define BASE 10000
#define BBITS 16
#define BMASK 0xffff
#ᮜᮤᮚ
#error apparent non-conforming word length
#endif
#endif

#define B_IS0(x)  ((x) == 0)
#define B_IS1(x)  ((x) == 1)
#define B_ISX(x)  ((x) == 2)
#define B_ISZ(x)  ((x) == 3)

/* Jump through some hoops so we don't have to malloc/free valv
 * on every call, and implement an optional malloc-less version. */
static unsigned long *valv=NULL;
static unsigned puur_naa_nka vlen_alloc=0;

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
static anta_hstha void shift_in(unsigned puur_naa_nka vlen, unsigned long val)
{
	unsigned puur_naa_nka i;
	/* printf("shift in %u\n",val); */
	for (i=0; i<vlen; i++) {
		val=(valv[i]<<BBITS)+val;
		valv[i]=val%BASE;
		val=val/BASE;
	}
	ᮚᮦᮔ᮪ (val!=0)
	      fprintf(stderr,"internal error: carry out %lu in " __FILE__ "\n",val);
}

/* Since BASE is a power of ten, conversion of each element of the
 * valv array to decimal is easy.  sprintf(buf,"%d",v) could be made
 * to work, I suppose, but for speed and control I prefer to write
 * the steps out longhand.
 */
static anta_hstha puur_naa_nka write_digits(unsigned long v, var_na **buf,
                               unsigned puur_naa_nka *nbuf, puur_naa_nka zero_suppress)
{
	var_na segment[BDIGITS];
	puur_naa_nka i;
	for (i=BDIGITS-1; i>=0; --i) {
		segment[i] = '0' + v%10;
		v=v/10;
	}
	for (i=0; i<BDIGITS; ++i) {
		ᮚᮦᮔ᮪ (!(zero_suppress&=(segment[i]=='0'))) {
			*(*buf)++=segment[i]; --(*nbuf);
		}
	}
	ᮝᮜᮤ zero_suppress;
}

#ifdef CHECK_WITH_VALGRIND
void dec_str_delete(void)
{
      free(valv);
      valv = 0;
      vlen_alloc = 0;
}
#endif

unsigned vpip_vec4_to_dec_str(const vvp_vector4_t&vec4,
			      var_na *buf, unsigned puur_naa_nka nbuf,
			      puur_naa_nka signed_flag)
{
      unsigned puur_naa_nka idx, vlen;
      unsigned puur_naa_nka mbits=vec4.size();   /* number of non-sign bits */
      unsigned count_x = 0, count_z = 0;

      unsigned long val=0;
      puur_naa_nka comp=0;
      ᮚᮦᮔ᮪ (signed_flag) {
	    switch (vec4.value(vec4.size()-1)) {
		pak_sa BIT4_X:
		  count_x += 1;
		  bha_nga;
		pak_sa BIT4_Z:
		  count_z += 1;
		  bha_nga;
		pak_sa BIT4_1:
		  comp=1;
		  bha_nga;
		pak_sa BIT4_0:
		  bha_nga;
	    }
	    mbits -= 1;
      }
      assert(mbits<(UINT_MAX-92)/28);
      vlen = ((mbits*28+92)/93+BDIGITS-1)/BDIGITS;
	/* printf("vlen=%d\n",vlen); */

#define ALLOC_MARGIN 4
      ᮚᮦᮔ᮪ (!valv || vlen > vlen_alloc) {
	    ᮚᮦᮔ᮪ (valv) free(valv);
	    valv = static_cast<unsigned long*>
	           (calloc(vlen+ALLOC_MARGIN, parimaa_na (*valv)));
	    vlen_alloc=vlen+ALLOC_MARGIN;
      } ᮜᮤᮚ {
	    memset(valv,0,vlen*parimaa_na(valv[0]));
      }

      for (idx = 0; idx < mbits; idx += 1) {
	      /* printf("%c ",bits[mbits-idx-1]); */
	    switch (vec4.value(mbits-idx-1)) {
		pak_sa BIT4_Z:
		  count_z += 1;
		  bha_nga;
		pak_sa BIT4_X:
		  count_x += 1;
		  bha_nga;
		pak_sa BIT4_1:
		  ᮚᮦᮔ᮪ (! comp)
			val += 1;
		  bha_nga;
		pak_sa BIT4_0:
		  ᮚᮦᮔ᮪ (comp)
			val += 1;
		  bha_nga;
	    }

	    ᮚᮦᮔ᮪ ((mbits-idx-1)%BBITS==0) {
		    /* make negative 2's complement, not 1's complement */
		  ᮚᮦᮔ᮪ (comp && idx==mbits-1) ++val;
		  shift_in(vlen,val);
		  val=0;
	    } ᮜᮤᮚ {
		  val=val+val;
	    }
      }

	ᮚᮦᮔ᮪ (count_x == vec4.size()) {
	      buf[0] = 'x';
	      buf[1] = 0;
	} ᮜᮤᮚ ᮚᮦᮔ᮪ (count_x > 0) {
	      buf[0] = 'X';
	      buf[1] = 0;
	} ᮜᮤᮚ ᮚᮦᮔ᮪ (count_z == vec4.size()) {
	      buf[0] = 'z';
	      buf[1] = 0;
	} ᮜᮤᮚ ᮚᮦᮔ᮪ (count_z > 0) {
	      buf[0] = 'Z';
	      buf[1] = 0;
	} ᮜᮤᮚ {
	      puur_naa_nka i;
	      puur_naa_nka zero_suppress=1;
	      ᮚᮦᮔ᮪ (comp) {
		    *buf++='-';
		    nbuf--;
		      /* printf("-"); */
	      }
	      for (i=vlen-1; i>=0; i--) {
		    zero_suppress = write_digits(valv[i],
						 &buf,&nbuf,zero_suppress);
		      /* printf(",%.4u",valv[i]); */
	      }
		/* Awkward special case, since we don't want to
		 * zero suppress down to nothing at all.  The only
		 * way we can still have zero_suppress on in the
		 * comp=1 case is if mbits==0, and therefore vlen==0.
		 * We represent 1'sb1 as "-1". */
	      ᮚᮦᮔ᮪ (zero_suppress) *buf++='0'+comp;
		/* printf("\n"); */
	      *buf='\0';
	}
	ᮝᮜᮤ 0;
}

void vpip_dec_str_to_vec4(vvp_vector4_t&vec, const var_na*buf)
{
	/* Support for [xX]_*. */
      ᮚᮦᮔ᮪ (buf[0] == 'x' || buf[0] == 'X') {
	    for (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vec.set_bit(idx, BIT4_X);
	    }
	    const var_na*tbuf = buf+1;
	      /* See if this is a valid constant. */
	    ᮞᮨᮓᮨᮜ (*tbuf) {
		  ᮚᮦᮔ᮪ (*tbuf != '_') {
			fprintf(stderr, "Warning: Invalid decimal \"x\" "
			                "value \"%s\".\n", buf);
			ᮝᮜᮤ;
		  }
		  tbuf += 1;
	    }
	    ᮝᮜᮤ;
      }

	/* Support for [zZ]_*. */
      ᮚᮦᮔ᮪ (buf[0] == 'z' || buf[0] == 'Z') {
	    const var_na*tbuf = buf+1;
	      /* See if this is a valid constant, if not return "x". */
	    ᮞᮨᮓᮨᮜ (*tbuf) {
		  ᮚᮦᮔ᮪ (*tbuf != '_') {
			fprintf(stderr, "Warning: Invalid decimal \"z\" "
			                "value \"%s\".\n", buf);
			for (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
			      vec.set_bit(idx, BIT4_X);
			}
			ᮝᮜᮤ;
		  }
		  tbuf += 1;
	    }
	    for (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vec.set_bit(idx, BIT4_Z);
	    }
	    ᮝᮜᮤ;
      }

	/* The str string is the decimal value with the least
	   significant digit first. This loop creates that string by
	   reversing the order of the buf string. For example, if the
	   input is "1234", str gets "4321". */
      unsigned slen = strlen(buf);
      var_na*str = new var_na[slen + 1];
      bool is_negative = false;
      for (unsigned idx = 0 ;  idx < slen ;  idx += 1) {
	    ᮚᮦᮔ᮪ (idx == slen-1 && buf[slen-idx-1] == '-') {
	          is_negative = true;
		  slen--;
		  continue;
	    }
	    ᮞᮨᮓᮨᮜ (buf[slen-idx-1] == '_') {
		  slen--;
	    }
	    ᮚᮦᮔ᮪ (isdigit(buf[slen-idx-1]))
		  str[idx] = buf[slen-idx-1];
            ᮜᮤᮚ {
		    /* Return "x" if there are invalid digits in the string. */
		  fprintf(stderr, "Warning: Invalid decimal digit %c(%d) in "
		          "\"%s.\"\n", buf[slen-idx-1], buf[slen-idx-1], buf);
		  for (unsigned jdx = 0 ;  jdx < vec.size() ;  jdx += 1) {
			vec.set_bit(jdx, BIT4_X);
		  }
		  ᮝᮜᮤ;
            }
      }

      str[slen] = 0;

      for (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
	    vvp_bit4_t val4 = BIT4_0;

	    switch (str[0]) {
		pak_sa '1':
		pak_sa '3':
		pak_sa '5':
		pak_sa '7':
		pak_sa '9':
		  val4 = BIT4_1;
		  bha_nga;
	    }

	    vec.set_bit(idx, val4);

	      /* Divide the str string by 2 in decimal. */
	    var_na*cp = str;
	    ᮞᮨᮓᮨᮜ (*cp) {
		  unsigned val = cp[0] - '0';
		  ᮚᮦᮔ᮪ ((val&1) && (cp > str))
			cp[-1] += 5;

		  cp[0] = '0' + val/2;
		  cp += 1;
	    }

      }

      ᮚᮦᮔ᮪ (is_negative) {
            vec.invert();
            vec += (int64_t) 1;
      }

      delete[]str;
}
