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
#که_تعریف_شوی CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#د_که_پای
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


#إذا SIZEOF_UNSIGNED_LONG * CHAR_BIT >= 64
#تعریف_کړه BDIGITS 9
#تعریف_کړه بنسټ_3 1000000000
#تعریف_کړه BBITS 32
#تعریف_کړه BMASK 0xffffffff
#وإلا
#إذا SIZEOF_UNSIGNED_LONG * CHAR_BIT >= 32
#تعریف_کړه BDIGITS 4
#تعریف_کړه بنسټ_3 10000
#تعریف_کړه BBITS 16
#تعریف_کړه BMASK 0xffff
#وإلا
#تېروتنه apparent non-conforming کلمه length
#د_که_پای
#د_که_پای

#تعریف_کړه B_IS0(x)  ((x) == 0)
#تعریف_کړه B_IS1(x)  ((x) == 1)
#تعریف_کړه B_ISX(x)  ((x) == 2)
#تعریف_کړه B_ISZ(x)  ((x) == 3)

/* Jump through some hoops so we don't have to malloc/free valv
 * on every call, and implement an optional malloc-less version. */
ساكن بینښه اوږد *valv=عدم;
ساكن بینښه صحيح vlen_alloc=0;

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
ساكن مضمن فراغ ښويدنه_دننه(بینښه صحيح vlen, بینښه اوږد val)
{
	بینښه صحيح i;
	/* printf("shift in %u\n",val); */
	لكل (i=0; i<vlen; i++) {
		val=(valv[i]<<BBITS)+val;
		valv[i]=val%بنسټ_3;
		val=val/بنسټ_3;
	}
	إذا (val!=0)
	      fprintf(stderr,"internal error: carry out %lu in " __FILE__ "\n",val);
}

/* Since BASE is a power of ten, conversion of each element of the
 * valv array to decimal is easy.  sprintf(buf,"%d",v) could be made
 * to work, I suppose, but for speed and control I prefer to write
 * the steps out longhand.
 */
ساكن مضمن صحيح write_digits(بینښه اوږد v, حرف **buf,
                               بینښه صحيح *nbuf, صحيح zero_suppress)
{
	حرف segment[BDIGITS];
	صحيح i;
	لكل (i=BDIGITS-1; i>=0; --i) {
		segment[i] = '0' + v%10;
		v=v/10;
	}
	لكل (i=0; i<BDIGITS; ++i) {
		إذا (!(zero_suppress&=(segment[i]=='0'))) {
			*(*buf)++=segment[i]; --(*nbuf);
		}
	}
	أرجع zero_suppress;
}

#که_تعریف_شوی CHECK_WITH_VALGRIND
فراغ dec_str_delete(فراغ)
{
      حرر(valv);
      valv = 0;
      vlen_alloc = 0;
}
#د_که_پای

بینښه vpip_vec4_منزل_dec_str(ثابت vvp_vector4_t&vec4,
			      حرف *buf, بینښه صحيح nbuf,
			      صحيح signed_flag)
{
      بینښه صحيح idx, vlen;
      بینښه صحيح mbits=vec4.size();   /* number of non-sign bits */
      بینښه شمېر_x = 0, شمېر_z = 0;

      بینښه اوږد val=0;
      صحيح comp=0;
      إذا (signed_flag) {
	    بدل (vec4.value(vec4.size()-1)) {
		حالة BIT4_X:
		  شمېر_x += 1;
		  اكسر;
		حالة BIT4_Z:
		  شمېر_z += 1;
		  اكسر;
		حالة BIT4_1:
		  comp=1;
		  اكسر;
		حالة BIT4_0:
		  اكسر;
	    }
	    mbits -= 1;
      }
      أكد(mbits<(UINT_MAX-92)/28);
      vlen = ((mbits*28+92)/93+BDIGITS-1)/BDIGITS;
	/* printf("vlen=%d\n",vlen); */

#تعریف_کړه ALLOC_MARGIN 4
      إذا (!valv || vlen > vlen_alloc) {
	    إذا (valv) حرر(valv);
	    valv = static_cast<بینښه اوږد*>
	           (حافظه_صفر_ورکړه(vlen+ALLOC_MARGIN, حجم (*valv)));
	    vlen_alloc=vlen+ALLOC_MARGIN;
      } وإلا {
	    memset(valv,0,vlen*حجم(valv[0]));
      }

      لكل (idx = 0; idx < mbits; idx += 1) {
	      /* printf("%c ",bits[mbits-idx-1]); */
	    بدل (vec4.value(mbits-idx-1)) {
		حالة BIT4_Z:
		  شمېر_z += 1;
		  اكسر;
		حالة BIT4_X:
		  شمېر_x += 1;
		  اكسر;
		حالة BIT4_1:
		  إذا (! comp)
			val += 1;
		  اكسر;
		حالة BIT4_0:
		  إذا (comp)
			val += 1;
		  اكسر;
	    }

	    إذا ((mbits-idx-1)%BBITS==0) {
		    /* make negative 2's complement, not 1's complement */
		  إذا (comp && idx==mbits-1) ++val;
		  ښويدنه_دننه(vlen,val);
		  val=0;
	    } وإلا {
		  val=val+val;
	    }
      }

	إذا (شمېر_x == vec4.size()) {
	      buf[0] = 'x';
	      buf[1] = 0;
	} وإلا إذا (شمېر_x > 0) {
	      buf[0] = 'X';
	      buf[1] = 0;
	} وإلا إذا (شمېر_z == vec4.size()) {
	      buf[0] = 'z';
	      buf[1] = 0;
	} وإلا إذا (شمېر_z > 0) {
	      buf[0] = 'Z';
	      buf[1] = 0;
	} وإلا {
	      صحيح i;
	      صحيح zero_suppress=1;
	      إذا (comp) {
		    *buf++='-';
		    nbuf--;
		      /* printf("-"); */
	      }
	      لكل (i=vlen-1; i>=0; i--) {
		    zero_suppress = write_digits(valv[i],
						 &buf,&nbuf,zero_suppress);
		      /* printf(",%.4u",valv[i]); */
	      }
		/* Awkward special case, since we don't want to
		 * zero suppress down to nothing at all.  The only
		 * way we can still have zero_suppress on in the
		 * comp=1 case is if mbits==0, and therefore vlen==0.
		 * We represent 1'sb1 as "-1". */
	      إذا (zero_suppress) *buf++='0'+comp;
		/* printf("\n"); */
	      *buf='\0';
	}
	أرجع 0;
}

فراغ vpip_dec_str_منزل_vec4(vvp_vector4_t&vec, ثابت حرف*buf)
{
	/* Support for [xX]_*. */
      إذا (buf[0] == 'x' || buf[0] == 'X') {
	    لكل (بینښه idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vec.سېټ_بټ(idx, BIT4_X);
	    }
	    ثابت حرف*tbuf = buf+1;
	      /* See if this is a valid constant. */
	    طالما (*tbuf) {
		  إذا (*tbuf != '_') {
			fprintf(stderr, "Warning: Invalid decimal \"x\" "
			                "value \"%s\".\n", buf);
			أرجع;
		  }
		  tbuf += 1;
	    }
	    أرجع;
      }

	/* Support for [zZ]_*. */
      إذا (buf[0] == 'z' || buf[0] == 'Z') {
	    ثابت حرف*tbuf = buf+1;
	      /* See if this is a valid constant, if not return "x". */
	    طالما (*tbuf) {
		  إذا (*tbuf != '_') {
			fprintf(stderr, "Warning: Invalid decimal \"z\" "
			                "value \"%s\".\n", buf);
			لكل (بینښه idx = 0 ;  idx < vec.size() ;  idx += 1) {
			      vec.سېټ_بټ(idx, BIT4_X);
			}
			أرجع;
		  }
		  tbuf += 1;
	    }
	    لكل (بینښه idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vec.سېټ_بټ(idx, BIT4_Z);
	    }
	    أرجع;
      }

	/* The str string is the decimal value with the least
	   significant digit first. This loop creates that string by
	   reversing the order of the buf string. For example, if the
	   input is "1234", str gets "4321". */
      بینښه slen = مزي_اوږدوالی(buf);
      حرف*str = جديد حرف[slen + 1];
      منطقي is_negative = خطأ;
      لكل (بینښه idx = 0 ;  idx < slen ;  idx += 1) {
	    إذا (idx == slen-1 && buf[slen-idx-1] == '-') {
	          is_negative = ریښتیا;
		  slen--;
		  استمر;
	    }
	    طالما (buf[slen-idx-1] == '_') {
		  slen--;
	    }
	    إذا (که_عدد(buf[slen-idx-1]))
		  str[idx] = buf[slen-idx-1];
            وإلا {
		    /* Return "x" if there are invalid digits in the string. */
		  fprintf(stderr, "Warning: Invalid decimal digit %c(%d) in "
		          "\"%s.\"\n", buf[slen-idx-1], buf[slen-idx-1], buf);
		  لكل (بینښه jdx = 0 ;  jdx < vec.size() ;  jdx += 1) {
			vec.سېټ_بټ(jdx, BIT4_X);
		  }
		  أرجع;
            }
      }

      str[slen] = 0;

      لكل (بینښه idx = 0 ;  idx < vec.size() ;  idx += 1) {
	    vvp_bit4_t val4 = BIT4_0;

	    بدل (str[0]) {
		حالة '1':
		حالة '3':
		حالة '5':
		حالة '7':
		حالة '9':
		  val4 = BIT4_1;
		  اكسر;
	    }

	    vec.سېټ_بټ(idx, val4);

	      /* Divide the str string by 2 in decimal. */
	    حرف*cp = str;
	    طالما (*cp) {
		  بینښه val = cp[0] - '0';
		  إذا ((val&1) && (cp > str))
			cp[-1] += 5;

		  cp[0] = '0' + val/2;
		  cp += 1;
	    }

      }

      إذا (is_negative) {
            vec.invert();
            vec += (int64_t) 1;
      }

      ړنګول[]str;
}
