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
#यदि_परिभाषितम् CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#यदि_अन्तः
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


#यदि SIZEOF_UNSIGNED_LONG * CHAR_BIT >= 64
#परिभाषय BDIGITS 9
#परिभाषय आधारः_3 1000000000
#परिभाषय BBITS 32
#परिभाषय BMASK 0xffffffff
#अन्यथा
#यदि SIZEOF_UNSIGNED_LONG * CHAR_BIT >= 32
#परिभाषय BDIGITS 4
#परिभाषय आधारः_3 10000
#परिभाषय BBITS 16
#परिभाषय BMASK 0xffff
#अन्यथा
#दोषः apparent non-conforming word length
#यदि_अन्तः
#यदि_अन्तः

#परिभाषय B_IS0(x)  ((x) == 0)
#परिभाषय B_IS1(x)  ((x) == 1)
#परिभाषय B_ISX(x)  ((x) == 2)
#परिभाषय B_ISZ(x)  ((x) == 3)

/* Jump through some hoops so we don't have to malloc/free valv
 * on every call, and implement an optional malloc-less version. */
स्थिर अचिह्नित दीर्घ *valv=रिक्त;
स्थिर अचिह्नित पूर्णाङ्क vlen_alloc=0;

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
स्थिर अन्तःस्थ शून्य सरणम्_अन्तर्(अचिह्नित पूर्णाङ्क vlen, अचिह्नित दीर्घ val)
{
	अचिह्नित पूर्णाङ्क i;
	/* printf("shift in %u\n",val); */
	प्रति (i=0; i<vlen; i++) {
		val=(valv[i]<<BBITS)+val;
		valv[i]=val%आधारः_3;
		val=val/आधारः_3;
	}
	यदि (val!=0)
	      fprintf(stderr,"internal error: carry out %lu in " __FILE__ "\n",val);
}

/* Since BASE is a power of ten, conversion of each element of the
 * valv array to decimal is easy.  sprintf(buf,"%d",v) could be made
 * to work, I suppose, but for speed and control I prefer to write
 * the steps out longhand.
 */
स्थिर अन्तःस्थ पूर्णाङ्क write_digits(अचिह्नित दीर्घ v, वर्ण **buf,
                               अचिह्नित पूर्णाङ्क *nbuf, पूर्णाङ्क zero_suppress)
{
	वर्ण segment[BDIGITS];
	पूर्णाङ्क i;
	प्रति (i=BDIGITS-1; i>=0; --i) {
		segment[i] = '0' + v%10;
		v=v/10;
	}
	प्रति (i=0; i<BDIGITS; ++i) {
		यदि (!(zero_suppress&=(segment[i]=='0'))) {
			*(*buf)++=segment[i]; --(*nbuf);
		}
	}
	प्रत्यागम zero_suppress;
}

#यदि_परिभाषितम् CHECK_WITH_VALGRIND
शून्य dec_str_delete(शून्य)
{
      मुक्त(valv);
      valv = 0;
      vlen_alloc = 0;
}
#यदि_अन्तः

अचिह्नित vpip_vec4_गन्तव्यम्_dec_str(ध्रुव vvp_vector4_t&vec4,
			      वर्ण *buf, अचिह्नित पूर्णाङ्क nbuf,
			      पूर्णाङ्क signed_flag)
{
      अचिह्नित पूर्णाङ्क idx, vlen;
      अचिह्नित पूर्णाङ्क mbits=vec4.size();   /* number of non-sign bits */
      अचिह्नित गणनम्_x = 0, गणनम्_z = 0;

      अचिह्नित दीर्घ val=0;
      पूर्णाङ्क comp=0;
      यदि (signed_flag) {
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

#परिभाषय ALLOC_MARGIN 4
      यदि (!valv || vlen > vlen_alloc) {
	    यदि (valv) मुक्त(valv);
	    valv = static_cast<अचिह्नित दीर्घ*>
	           (शून्यस्मृतिं_देहि(vlen+ALLOC_MARGIN, परिमाण (*valv)));
	    vlen_alloc=vlen+ALLOC_MARGIN;
      } अन्यथा {
	    स्मृतिं_पूरय(valv,0,vlen*परिमाण(valv[0]));
      }

      प्रति (idx = 0; idx < mbits; idx += 1) {
	      /* printf("%c ",bits[mbits-idx-1]); */
	    विकल्प (vec4.value(mbits-idx-1)) {
		पक्ष BIT4_Z:
		  गणनम्_z += 1;
		  भङ्ग;
		पक्ष BIT4_X:
		  गणनम्_x += 1;
		  भङ्ग;
		पक्ष BIT4_1:
		  यदि (! comp)
			val += 1;
		  भङ्ग;
		पक्ष BIT4_0:
		  यदि (comp)
			val += 1;
		  भङ्ग;
	    }

	    यदि ((mbits-idx-1)%BBITS==0) {
		    /* make negative 2's complement, not 1's complement */
		  यदि (comp && idx==mbits-1) ++val;
		  सरणम्_अन्तर्(vlen,val);
		  val=0;
	    } अन्यथा {
		  val=val+val;
	    }
      }

	यदि (गणनम्_x == vec4.size()) {
	      buf[0] = 'x';
	      buf[1] = 0;
	} अन्यथा यदि (गणनम्_x > 0) {
	      buf[0] = 'X';
	      buf[1] = 0;
	} अन्यथा यदि (गणनम्_z == vec4.size()) {
	      buf[0] = 'z';
	      buf[1] = 0;
	} अन्यथा यदि (गणनम्_z > 0) {
	      buf[0] = 'Z';
	      buf[1] = 0;
	} अन्यथा {
	      पूर्णाङ्क i;
	      पूर्णाङ्क zero_suppress=1;
	      यदि (comp) {
		    *buf++='-';
		    nbuf--;
		      /* printf("-"); */
	      }
	      प्रति (i=vlen-1; i>=0; i--) {
		    zero_suppress = write_digits(valv[i],
						 &buf,&nbuf,zero_suppress);
		      /* printf(",%.4u",valv[i]); */
	      }
		/* Awkward special case, since we don't want to
		 * zero suppress down to nothing at all.  The only
		 * way we can still have zero_suppress on in the
		 * comp=1 case is if mbits==0, and therefore vlen==0.
		 * We represent 1'sb1 as "-1". */
	      यदि (zero_suppress) *buf++='0'+comp;
		/* printf("\n"); */
	      *buf='\0';
	}
	प्रत्यागम 0;
}

शून्य vpip_dec_str_गन्तव्यम्_vec4(vvp_vector4_t&vec, ध्रुव वर्ण*buf)
{
	/* Support for [xX]_*. */
      यदि (buf[0] == 'x' || buf[0] == 'X') {
	    प्रति (अचिह्नित idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vec.समुच्चयः_bit(idx, BIT4_X);
	    }
	    ध्रुव वर्ण*tbuf = buf+1;
	      /* See if this is a valid constant. */
	    यावत् (*tbuf) {
		  यदि (*tbuf != '_') {
			fprintf(stderr, "Warning: Invalid decimal \"x\" "
			                "value \"%s\".\n", buf);
			प्रत्यागम;
		  }
		  tbuf += 1;
	    }
	    प्रत्यागम;
      }

	/* Support for [zZ]_*. */
      यदि (buf[0] == 'z' || buf[0] == 'Z') {
	    ध्रुव वर्ण*tbuf = buf+1;
	      /* See if this is a valid constant, if not return "x". */
	    यावत् (*tbuf) {
		  यदि (*tbuf != '_') {
			fprintf(stderr, "Warning: Invalid decimal \"z\" "
			                "value \"%s\".\n", buf);
			प्रति (अचिह्नित idx = 0 ;  idx < vec.size() ;  idx += 1) {
			      vec.समुच्चयः_bit(idx, BIT4_X);
			}
			प्रत्यागम;
		  }
		  tbuf += 1;
	    }
	    प्रति (अचिह्नित idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vec.समुच्चयः_bit(idx, BIT4_Z);
	    }
	    प्रत्यागम;
      }

	/* The str string is the decimal value with the least
	   significant digit first. This loop creates that string by
	   reversing the order of the buf string. For example, if the
	   input is "1234", str gets "4321". */
      अचिह्नित slen = वाक्यदैर्घ्यम्(buf);
      वर्ण*str = नूतन वर्ण[slen + 1];
      तार्किक is_negative = असत्य;
      प्रति (अचिह्नित idx = 0 ;  idx < slen ;  idx += 1) {
	    यदि (idx == slen-1 && buf[slen-idx-1] == '-') {
	          is_negative = सत्य;
		  slen--;
		  अनुवर्त;
	    }
	    यावत् (buf[slen-idx-1] == '_') {
		  slen--;
	    }
	    यदि (अङ्कः_वा(buf[slen-idx-1]))
		  str[idx] = buf[slen-idx-1];
            अन्यथा {
		    /* Return "x" if there are invalid digits in the string. */
		  fprintf(stderr, "Warning: Invalid decimal digit %c(%d) in "
		          "\"%s.\"\n", buf[slen-idx-1], buf[slen-idx-1], buf);
		  प्रति (अचिह्नित jdx = 0 ;  jdx < vec.size() ;  jdx += 1) {
			vec.समुच्चयः_bit(jdx, BIT4_X);
		  }
		  प्रत्यागम;
            }
      }

      str[slen] = 0;

      प्रति (अचिह्नित idx = 0 ;  idx < vec.size() ;  idx += 1) {
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

	    vec.समुच्चयः_bit(idx, val4);

	      /* Divide the str string by 2 in decimal. */
	    वर्ण*cp = str;
	    यावत् (*cp) {
		  अचिह्नित val = cp[0] - '0';
		  यदि ((val&1) && (cp > str))
			cp[-1] += 5;

		  cp[0] = '0' + val/2;
		  cp += 1;
	    }

      }

      यदि (is_negative) {
            vec.invert();
            vec += (int64_t) 1;
      }

      विलोपय[]str;
}
