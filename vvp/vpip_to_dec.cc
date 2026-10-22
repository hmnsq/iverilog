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


#എങ്കിൽ SIZEOF_UNSIGNED_LONG * CHAR_BIT >= 64
#define BDIGITS 9
#define BASE 1000000000
#define BBITS 32
#define BMASK 0xffffffff
#അല്ലെങ്കിൽ
#എങ്കിൽ SIZEOF_UNSIGNED_LONG * CHAR_BIT >= 32
#define BDIGITS 4
#define BASE 10000
#define BBITS 16
#define BMASK 0xffff
#അല്ലെങ്കിൽ
#error apparent non-conforming word length
#endif
#endif

#define B_IS0(x)  ((x) == 0)
#define B_IS1(x)  ((x) == 1)
#define B_ISX(x)  ((x) == 2)
#define B_ISZ(x)  ((x) == 3)

/* Jump through some hoops so we don't have to malloc/free valv
 * on every call, and implement an optional malloc-less version. */
സ്ഥിതം unsigned long *valv=ശൂന്യ;
സ്ഥിതം unsigned പൂർണ്ണസംഖ്യ vlen_alloc=0;

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
സ്ഥിതം വരിയിൽ ശൂന്യം shift_in(unsigned പൂർണ്ണസംഖ്യ vlen, unsigned long val)
{
	unsigned പൂർണ്ണസംഖ്യ i;
	/* printf("shift in %u\n",val); */
	വേണ്ടി (i=0; i<vlen; i++) {
		val=(valv[i]<<BBITS)+val;
		valv[i]=val%BASE;
		val=val/BASE;
	}
	എങ്കിൽ (val!=0)
	      fprintf(stderr,"internal error: carry out %lu in " __FILE__ "\n",val);
}

/* Since BASE is a power of ten, conversion of each element of the
 * valv array to decimal is easy.  sprintf(buf,"%d",v) could be made
 * to work, I suppose, but for speed and control I prefer to write
 * the steps out longhand.
 */
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ write_digits(unsigned long v, അക്ഷരം **buf,
                               unsigned പൂർണ്ണസംഖ്യ *nbuf, പൂർണ്ണസംഖ്യ zero_suppress)
{
	അക്ഷരം segment[BDIGITS];
	പൂർണ്ണസംഖ്യ i;
	വേണ്ടി (i=BDIGITS-1; i>=0; --i) {
		segment[i] = '0' + v%10;
		v=v/10;
	}
	വേണ്ടി (i=0; i<BDIGITS; ++i) {
		എങ്കിൽ (!(zero_suppress&=(segment[i]=='0'))) {
			*(*buf)++=segment[i]; --(*nbuf);
		}
	}
	തിരികെ zero_suppress;
}

#ifdef CHECK_WITH_VALGRIND
ശൂന്യം dec_str_delete(ശൂന്യം)
{
      സ്വതന്ത്രം(valv);
      valv = 0;
      vlen_alloc = 0;
}
#endif

unsigned vpip_vec4_to_dec_str(സ്ഥിരം vvp_vector4_t&vec4,
			      അക്ഷരം *buf, unsigned പൂർണ്ണസംഖ്യ nbuf,
			      പൂർണ്ണസംഖ്യ signed_flag)
{
      unsigned പൂർണ്ണസംഖ്യ idx, vlen;
      unsigned പൂർണ്ണസംഖ്യ mbits=vec4.size();   /* number of non-sign bits */
      unsigned എണ്ണം_x = 0, എണ്ണം_z = 0;

      unsigned long val=0;
      പൂർണ്ണസംഖ്യ comp=0;
      എങ്കിൽ (signed_flag) {
	    തിരഞ്ഞെടുപ്പ് (vec4.value(vec4.size()-1)) {
		അവസ്ഥ BIT4_X:
		  എണ്ണം_x += 1;
		  നിർത്തുക;
		അവസ്ഥ BIT4_Z:
		  എണ്ണം_z += 1;
		  നിർത്തുക;
		അവസ്ഥ BIT4_1:
		  comp=1;
		  നിർത്തുക;
		അവസ്ഥ BIT4_0:
		  നിർത്തുക;
	    }
	    mbits -= 1;
      }
      പ്രതിജ്ഞാ(mbits<(UINT_MAX-92)/28);
      vlen = ((mbits*28+92)/93+BDIGITS-1)/BDIGITS;
	/* printf("vlen=%d\n",vlen); */

#define ALLOC_MARGIN 4
      എങ്കിൽ (!valv || vlen > vlen_alloc) {
	    എങ്കിൽ (valv) സ്വതന്ത്രം(valv);
	    valv = static_cast<unsigned long*>
	           (calloc(vlen+ALLOC_MARGIN, വലുപ്പം (*valv)));
	    vlen_alloc=vlen+ALLOC_MARGIN;
      } അല്ലെങ്കിൽ {
	    memset(valv,0,vlen*വലുപ്പം(valv[0]));
      }

      വേണ്ടി (idx = 0; idx < mbits; idx += 1) {
	      /* printf("%c ",bits[mbits-idx-1]); */
	    തിരഞ്ഞെടുപ്പ് (vec4.value(mbits-idx-1)) {
		അവസ്ഥ BIT4_Z:
		  എണ്ണം_z += 1;
		  നിർത്തുക;
		അവസ്ഥ BIT4_X:
		  എണ്ണം_x += 1;
		  നിർത്തുക;
		അവസ്ഥ BIT4_1:
		  എങ്കിൽ (! comp)
			val += 1;
		  നിർത്തുക;
		അവസ്ഥ BIT4_0:
		  എങ്കിൽ (comp)
			val += 1;
		  നിർത്തുക;
	    }

	    എങ്കിൽ ((mbits-idx-1)%BBITS==0) {
		    /* make negative 2's complement, not 1's complement */
		  എങ്കിൽ (comp && idx==mbits-1) ++val;
		  shift_in(vlen,val);
		  val=0;
	    } അല്ലെങ്കിൽ {
		  val=val+val;
	    }
      }

	എങ്കിൽ (എണ്ണം_x == vec4.size()) {
	      buf[0] = 'x';
	      buf[1] = 0;
	} അല്ലെങ്കിൽ എങ്കിൽ (എണ്ണം_x > 0) {
	      buf[0] = 'X';
	      buf[1] = 0;
	} അല്ലെങ്കിൽ എങ്കിൽ (എണ്ണം_z == vec4.size()) {
	      buf[0] = 'z';
	      buf[1] = 0;
	} അല്ലെങ്കിൽ എങ്കിൽ (എണ്ണം_z > 0) {
	      buf[0] = 'Z';
	      buf[1] = 0;
	} അല്ലെങ്കിൽ {
	      പൂർണ്ണസംഖ്യ i;
	      പൂർണ്ണസംഖ്യ zero_suppress=1;
	      എങ്കിൽ (comp) {
		    *buf++='-';
		    nbuf--;
		      /* printf("-"); */
	      }
	      വേണ്ടി (i=vlen-1; i>=0; i--) {
		    zero_suppress = write_digits(valv[i],
						 &buf,&nbuf,zero_suppress);
		      /* printf(",%.4u",valv[i]); */
	      }
		/* Awkward special case, since we don't want to
		 * zero suppress down to nothing at all.  The only
		 * way we can still have zero_suppress on in the
		 * comp=1 case is if mbits==0, and therefore vlen==0.
		 * We represent 1'sb1 as "-1". */
	      എങ്കിൽ (zero_suppress) *buf++='0'+comp;
		/* printf("\n"); */
	      *buf='\0';
	}
	തിരികെ 0;
}

ശൂന്യം vpip_dec_str_to_vec4(vvp_vector4_t&vec, സ്ഥിരം അക്ഷരം*buf)
{
	/* Support for [xX]_*. */
      എങ്കിൽ (buf[0] == 'x' || buf[0] == 'X') {
	    വേണ്ടി (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vec.set_bit(idx, BIT4_X);
	    }
	    സ്ഥിരം അക്ഷരം*tbuf = buf+1;
	      /* See if this is a valid constant. */
	    ആകുമ്പോൾ (*tbuf) {
		  എങ്കിൽ (*tbuf != '_') {
			fprintf(stderr, "Warning: Invalid decimal \"x\" "
			                "value \"%s\".\n", buf);
			തിരികെ;
		  }
		  tbuf += 1;
	    }
	    തിരികെ;
      }

	/* Support for [zZ]_*. */
      എങ്കിൽ (buf[0] == 'z' || buf[0] == 'Z') {
	    സ്ഥിരം അക്ഷരം*tbuf = buf+1;
	      /* See if this is a valid constant, if not return "x". */
	    ആകുമ്പോൾ (*tbuf) {
		  എങ്കിൽ (*tbuf != '_') {
			fprintf(stderr, "Warning: Invalid decimal \"z\" "
			                "value \"%s\".\n", buf);
			വേണ്ടി (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
			      vec.set_bit(idx, BIT4_X);
			}
			തിരികെ;
		  }
		  tbuf += 1;
	    }
	    വേണ്ടി (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vec.set_bit(idx, BIT4_Z);
	    }
	    തിരികെ;
      }

	/* The str string is the decimal value with the least
	   significant digit first. This loop creates that string by
	   reversing the order of the buf string. For example, if the
	   input is "1234", str gets "4321". */
      unsigned slen = strlen(buf);
      അക്ഷരം*str = പുതിയ അക്ഷരം[slen + 1];
      തർക്കം is_negative = അസത്യം;
      വേണ്ടി (unsigned idx = 0 ;  idx < slen ;  idx += 1) {
	    എങ്കിൽ (idx == slen-1 && buf[slen-idx-1] == '-') {
	          is_negative = സത്യം;
		  slen--;
		  തുടരുക;
	    }
	    ആകുമ്പോൾ (buf[slen-idx-1] == '_') {
		  slen--;
	    }
	    എങ്കിൽ (isdigit(buf[slen-idx-1]))
		  str[idx] = buf[slen-idx-1];
            അല്ലെങ്കിൽ {
		    /* Return "x" if there are invalid digits in the string. */
		  fprintf(stderr, "Warning: Invalid decimal digit %c(%d) in "
		          "\"%s.\"\n", buf[slen-idx-1], buf[slen-idx-1], buf);
		  വേണ്ടി (unsigned jdx = 0 ;  jdx < vec.size() ;  jdx += 1) {
			vec.set_bit(jdx, BIT4_X);
		  }
		  തിരികെ;
            }
      }

      str[slen] = 0;

      വേണ്ടി (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
	    vvp_bit4_t val4 = BIT4_0;

	    തിരഞ്ഞെടുപ്പ് (str[0]) {
		അവസ്ഥ '1':
		അവസ്ഥ '3':
		അവസ്ഥ '5':
		അവസ്ഥ '7':
		അവസ്ഥ '9':
		  val4 = BIT4_1;
		  നിർത്തുക;
	    }

	    vec.set_bit(idx, val4);

	      /* Divide the str string by 2 in decimal. */
	    അക്ഷരം*cp = str;
	    ആകുമ്പോൾ (*cp) {
		  unsigned val = cp[0] - '0';
		  എങ്കിൽ ((val&1) && (cp > str))
			cp[-1] += 5;

		  cp[0] = '0' + val/2;
		  cp += 1;
	    }

      }

      എങ്കിൽ (is_negative) {
            vec.invert();
            vec += (int64_t) 1;
      }

      delete[]str;
}
