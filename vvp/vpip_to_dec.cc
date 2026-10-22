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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#ਜੇ_ਅੰਤ
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


#ਜੇ SIZEOF_UNSIGNED_LONG * CHAR_BIT >= 64
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BDIGITS 9
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਅਧਾਰ_3 1000000000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BBITS 32
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BMASK 0xffffffff
#ਨਹੀਂਤਾਂ
#ਜੇ SIZEOF_UNSIGNED_LONG * CHAR_BIT >= 32
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BDIGITS 4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਅਧਾਰ_3 10000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BBITS 16
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BMASK 0xffff
#ਨਹੀਂਤਾਂ
#ਗਲਤੀ apparent non-conforming ਸ਼ਬਦ_2 length
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ B_IS0(x)  ((x) == 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ B_IS1(x)  ((x) == 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ B_ISX(x)  ((x) == 2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ B_ISZ(x)  ((x) == 3)

/* Jump through some hoops so we don't have to malloc/free valv
 * on every call, and implement an optional malloc-less version. */
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *valv=ਨਿਰਾ;
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ vlen_alloc=0;

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
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ ਖਿਸਕਾਓ_ਵਿੱਚ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ vlen, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ val)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ i;
	/* printf("shift in %u\n",val); */
	ਲਈ (i=0; i<vlen; i++) {
		val=(valv[i]<<BBITS)+val;
		valv[i]=val%ਅਧਾਰ_3;
		val=val/ਅਧਾਰ_3;
	}
	ਜੇ (val!=0)
	      fprintf(stderr,"internal error: carry out %lu in " __FILE__ "\n",val);
}

/* Since BASE is a power of ten, conversion of each element of the
 * valv array to decimal is easy.  sprintf(buf,"%d",v) could be made
 * to work, I suppose, but for speed and control I prefer to write
 * the steps out longhand.
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ write_digits(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ v, ਅੱਖਰ **buf,
                               ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ *nbuf, ਪੂਰਨ zero_suppress)
{
	ਅੱਖਰ segment[BDIGITS];
	ਪੂਰਨ i;
	ਲਈ (i=BDIGITS-1; i>=0; --i) {
		segment[i] = '0' + v%10;
		v=v/10;
	}
	ਲਈ (i=0; i<BDIGITS; ++i) {
		ਜੇ (!(zero_suppress&=(segment[i]=='0'))) {
			*(*buf)++=segment[i]; --(*nbuf);
		}
	}
	ਵਾਪਸ zero_suppress;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਖਾਲੀ dec_str_delete(ਖਾਲੀ)
{
      ਮੁਕਤ(valv);
      valv = 0;
      vlen_alloc = 0;
}
#ਜੇ_ਅੰਤ

ਬਿਨਾਨਿਸ਼ਾਨੀ vpip_vec4_ਮੰਜ਼ਿਲ_dec_str(ਅਚਲ vvp_vector4_t&vec4,
			      ਅੱਖਰ *buf, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ nbuf,
			      ਪੂਰਨ signed_flag)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ idx, vlen;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ mbits=vec4.size();   /* number of non-sign bits */
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਗਿਣਤੀ_x = 0, ਗਿਣਤੀ_z = 0;

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ val=0;
      ਪੂਰਨ comp=0;
      ਜੇ (signed_flag) {
	    ਚੋਣ (vec4.value(vec4.size()-1)) {
		ਹਾਲਤ BIT4_X:
		  ਗਿਣਤੀ_x += 1;
		  ਰੋਕੋ;
		ਹਾਲਤ BIT4_Z:
		  ਗਿਣਤੀ_z += 1;
		  ਰੋਕੋ;
		ਹਾਲਤ BIT4_1:
		  comp=1;
		  ਰੋਕੋ;
		ਹਾਲਤ BIT4_0:
		  ਰੋਕੋ;
	    }
	    mbits -= 1;
      }
      ਪ੍ਰਤਿਜ੍ਞਾ(mbits<(UINT_MAX-92)/28);
      vlen = ((mbits*28+92)/93+BDIGITS-1)/BDIGITS;
	/* printf("vlen=%d\n",vlen); */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALLOC_MARGIN 4
      ਜੇ (!valv || vlen > vlen_alloc) {
	    ਜੇ (valv) ਮੁਕਤ(valv);
	    valv = static_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ*>
	           (ਸਿਫਰ_ਸਮ੍ਰਿਤੀ_ਦਿਓ(vlen+ALLOC_MARGIN, ਆਕਾਰ (*valv)));
	    vlen_alloc=vlen+ALLOC_MARGIN;
      } ਨਹੀਂਤਾਂ {
	    memset(valv,0,vlen*ਆਕਾਰ(valv[0]));
      }

      ਲਈ (idx = 0; idx < mbits; idx += 1) {
	      /* printf("%c ",bits[mbits-idx-1]); */
	    ਚੋਣ (vec4.value(mbits-idx-1)) {
		ਹਾਲਤ BIT4_Z:
		  ਗਿਣਤੀ_z += 1;
		  ਰੋਕੋ;
		ਹਾਲਤ BIT4_X:
		  ਗਿਣਤੀ_x += 1;
		  ਰੋਕੋ;
		ਹਾਲਤ BIT4_1:
		  ਜੇ (! comp)
			val += 1;
		  ਰੋਕੋ;
		ਹਾਲਤ BIT4_0:
		  ਜੇ (comp)
			val += 1;
		  ਰੋਕੋ;
	    }

	    ਜੇ ((mbits-idx-1)%BBITS==0) {
		    /* make negative 2's complement, not 1's complement */
		  ਜੇ (comp && idx==mbits-1) ++val;
		  ਖਿਸਕਾਓ_ਵਿੱਚ(vlen,val);
		  val=0;
	    } ਨਹੀਂਤਾਂ {
		  val=val+val;
	    }
      }

	ਜੇ (ਗਿਣਤੀ_x == vec4.size()) {
	      buf[0] = 'x';
	      buf[1] = 0;
	} ਨਹੀਂਤਾਂ ਜੇ (ਗਿਣਤੀ_x > 0) {
	      buf[0] = 'X';
	      buf[1] = 0;
	} ਨਹੀਂਤਾਂ ਜੇ (ਗਿਣਤੀ_z == vec4.size()) {
	      buf[0] = 'z';
	      buf[1] = 0;
	} ਨਹੀਂਤਾਂ ਜੇ (ਗਿਣਤੀ_z > 0) {
	      buf[0] = 'Z';
	      buf[1] = 0;
	} ਨਹੀਂਤਾਂ {
	      ਪੂਰਨ i;
	      ਪੂਰਨ zero_suppress=1;
	      ਜੇ (comp) {
		    *buf++='-';
		    nbuf--;
		      /* printf("-"); */
	      }
	      ਲਈ (i=vlen-1; i>=0; i--) {
		    zero_suppress = write_digits(valv[i],
						 &buf,&nbuf,zero_suppress);
		      /* printf(",%.4u",valv[i]); */
	      }
		/* Awkward special case, since we don't want to
		 * zero suppress down to nothing at all.  The only
		 * way we can still have zero_suppress on in the
		 * comp=1 case is if mbits==0, and therefore vlen==0.
		 * We represent 1'sb1 as "-1". */
	      ਜੇ (zero_suppress) *buf++='0'+comp;
		/* printf("\n"); */
	      *buf='\0';
	}
	ਵਾਪਸ 0;
}

ਖਾਲੀ vpip_dec_str_ਮੰਜ਼ਿਲ_vec4(vvp_vector4_t&vec, ਅਚਲ ਅੱਖਰ*buf)
{
	/* Support for [xX]_*. */
      ਜੇ (buf[0] == 'x' || buf[0] == 'X') {
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vec.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_X);
	    }
	    ਅਚਲ ਅੱਖਰ*tbuf = buf+1;
	      /* See if this is a valid constant. */
	    ਜਦੋਂ (*tbuf) {
		  ਜੇ (*tbuf != '_') {
			fprintf(stderr, "Warning: Invalid decimal \"x\" "
			                "value \"%s\".\n", buf);
			ਵਾਪਸ;
		  }
		  tbuf += 1;
	    }
	    ਵਾਪਸ;
      }

	/* Support for [zZ]_*. */
      ਜੇ (buf[0] == 'z' || buf[0] == 'Z') {
	    ਅਚਲ ਅੱਖਰ*tbuf = buf+1;
	      /* See if this is a valid constant, if not return "x". */
	    ਜਦੋਂ (*tbuf) {
		  ਜੇ (*tbuf != '_') {
			fprintf(stderr, "Warning: Invalid decimal \"z\" "
			                "value \"%s\".\n", buf);
			ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < vec.size() ;  idx += 1) {
			      vec.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_X);
			}
			ਵਾਪਸ;
		  }
		  tbuf += 1;
	    }
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vec.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_Z);
	    }
	    ਵਾਪਸ;
      }

	/* The str string is the decimal value with the least
	   significant digit first. This loop creates that string by
	   reversing the order of the buf string. For example, if the
	   input is "1234", str gets "4321". */
      ਬਿਨਾਨਿਸ਼ਾਨੀ slen = ਲੜੀ_ਲੰਬਾਈ(buf);
      ਅੱਖਰ*str = ਨਵਾਂ ਅੱਖਰ[slen + 1];
      ਤਾਰਕਿਕ is_negative = ਝੂਠ;
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < slen ;  idx += 1) {
	    ਜੇ (idx == slen-1 && buf[slen-idx-1] == '-') {
	          is_negative = ਸੱਚ;
		  slen--;
		  ਜਾਰੀ;
	    }
	    ਜਦੋਂ (buf[slen-idx-1] == '_') {
		  slen--;
	    }
	    ਜੇ (ਅੰਕ_ਹੈ(buf[slen-idx-1]))
		  str[idx] = buf[slen-idx-1];
            ਨਹੀਂਤਾਂ {
		    /* Return "x" if there are invalid digits in the string. */
		  fprintf(stderr, "Warning: Invalid decimal digit %c(%d) in "
		          "\"%s.\"\n", buf[slen-idx-1], buf[slen-idx-1], buf);
		  ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ jdx = 0 ;  jdx < vec.size() ;  jdx += 1) {
			vec.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(jdx, BIT4_X);
		  }
		  ਵਾਪਸ;
            }
      }

      str[slen] = 0;

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < vec.size() ;  idx += 1) {
	    vvp_bit4_t val4 = BIT4_0;

	    ਚੋਣ (str[0]) {
		ਹਾਲਤ '1':
		ਹਾਲਤ '3':
		ਹਾਲਤ '5':
		ਹਾਲਤ '7':
		ਹਾਲਤ '9':
		  val4 = BIT4_1;
		  ਰੋਕੋ;
	    }

	    vec.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, val4);

	      /* Divide the str string by 2 in decimal. */
	    ਅੱਖਰ*cp = str;
	    ਜਦੋਂ (*cp) {
		  ਬਿਨਾਨਿਸ਼ਾਨੀ val = cp[0] - '0';
		  ਜੇ ((val&1) && (cp > str))
			cp[-1] += 5;

		  cp[0] = '0' + val/2;
		  cp += 1;
	    }

      }

      ਜੇ (is_negative) {
            vec.invert();
            vec += (int64_t) 1;
      }

      ਮਿਟਾਓ[]str;
}
