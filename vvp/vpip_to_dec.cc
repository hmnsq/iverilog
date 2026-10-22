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
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#اگر_ختم
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


#اگر SIZEOF_UNSIGNED_LONG * CHAR_BIT >= 64
#تعریف_کرو BDIGITS 9
#تعریف_کرو بنیاد_3 1000000000
#تعریف_کرو BBITS 32
#تعریف_کرو BMASK 0xffffffff
#ورنہ
#اگر SIZEOF_UNSIGNED_LONG * CHAR_BIT >= 32
#تعریف_کرو BDIGITS 4
#تعریف_کرو بنیاد_3 10000
#تعریف_کرو BBITS 16
#تعریف_کرو BMASK 0xffff
#ورنہ
#غلطی apparent non-conforming لفظ length
#اگر_ختم
#اگر_ختم

#تعریف_کرو B_IS0(x)  ((x) == 0)
#تعریف_کرو B_IS1(x)  ((x) == 1)
#تعریف_کرو B_ISX(x)  ((x) == 2)
#تعریف_کرو B_ISZ(x)  ((x) == 3)

/* Jump through some hoops so we don't have to malloc/free valv
 * on every call, and implement an optional malloc-less version. */
ساکن غیرعلامتی طویل *valv=تہی;
ساکن غیرعلامتی صحیح vlen_alloc=0;

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
ساکن اندرونی خالی سرکاؤ_اندر(غیرعلامتی صحیح vlen, غیرعلامتی طویل val)
{
	غیرعلامتی صحیح i;
	/* printf("shift in %u\n",val); */
	برائے (i=0; i<vlen; i++) {
		val=(valv[i]<<BBITS)+val;
		valv[i]=val%بنیاد_3;
		val=val/بنیاد_3;
	}
	اگر (val!=0)
	      fprintf(stderr,"internal error: carry out %lu in " __FILE__ "\n",val);
}

/* Since BASE is a power of ten, conversion of each element of the
 * valv array to decimal is easy.  sprintf(buf,"%d",v) could be made
 * to work, I suppose, but for speed and control I prefer to write
 * the steps out longhand.
 */
ساکن اندرونی صحیح write_digits(غیرعلامتی طویل v, حرف **buf,
                               غیرعلامتی صحیح *nbuf, صحیح zero_suppress)
{
	حرف segment[BDIGITS];
	صحیح i;
	برائے (i=BDIGITS-1; i>=0; --i) {
		segment[i] = '0' + v%10;
		v=v/10;
	}
	برائے (i=0; i<BDIGITS; ++i) {
		اگر (!(zero_suppress&=(segment[i]=='0'))) {
			*(*buf)++=segment[i]; --(*nbuf);
		}
	}
	واپس zero_suppress;
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
خالی dec_str_delete(خالی)
{
      آزاد(valv);
      valv = 0;
      vlen_alloc = 0;
}
#اگر_ختم

غیرعلامتی vpip_vec4_مقصد_dec_str(ثابت vvp_vector4_t&vec4,
			      حرف *buf, غیرعلامتی صحیح nbuf,
			      صحیح signed_flag)
{
      غیرعلامتی صحیح idx, vlen;
      غیرعلامتی صحیح mbits=vec4.size();   /* number of non-sign bits */
      غیرعلامتی گنتی_x = 0, گنتی_z = 0;

      غیرعلامتی طویل val=0;
      صحیح comp=0;
      اگر (signed_flag) {
	    انتخاب (vec4.value(vec4.size()-1)) {
		حالت BIT4_X:
		  گنتی_x += 1;
		  توڑو;
		حالت BIT4_Z:
		  گنتی_z += 1;
		  توڑو;
		حالت BIT4_1:
		  comp=1;
		  توڑو;
		حالت BIT4_0:
		  توڑو;
	    }
	    mbits -= 1;
      }
      أكد(mbits<(UINT_MAX-92)/28);
      vlen = ((mbits*28+92)/93+BDIGITS-1)/BDIGITS;
	/* printf("vlen=%d\n",vlen); */

#تعریف_کرو ALLOC_MARGIN 4
      اگر (!valv || vlen > vlen_alloc) {
	    اگر (valv) آزاد(valv);
	    valv = static_cast<غیرعلامتی طویل*>
	           (صفر_حافظہ_دو(vlen+ALLOC_MARGIN, حجم (*valv)));
	    vlen_alloc=vlen+ALLOC_MARGIN;
      } ورنہ {
	    memset(valv,0,vlen*حجم(valv[0]));
      }

      برائے (idx = 0; idx < mbits; idx += 1) {
	      /* printf("%c ",bits[mbits-idx-1]); */
	    انتخاب (vec4.value(mbits-idx-1)) {
		حالت BIT4_Z:
		  گنتی_z += 1;
		  توڑو;
		حالت BIT4_X:
		  گنتی_x += 1;
		  توڑو;
		حالت BIT4_1:
		  اگر (! comp)
			val += 1;
		  توڑو;
		حالت BIT4_0:
		  اگر (comp)
			val += 1;
		  توڑو;
	    }

	    اگر ((mbits-idx-1)%BBITS==0) {
		    /* make negative 2's complement, not 1's complement */
		  اگر (comp && idx==mbits-1) ++val;
		  سرکاؤ_اندر(vlen,val);
		  val=0;
	    } ورنہ {
		  val=val+val;
	    }
      }

	اگر (گنتی_x == vec4.size()) {
	      buf[0] = 'x';
	      buf[1] = 0;
	} ورنہ اگر (گنتی_x > 0) {
	      buf[0] = 'X';
	      buf[1] = 0;
	} ورنہ اگر (گنتی_z == vec4.size()) {
	      buf[0] = 'z';
	      buf[1] = 0;
	} ورنہ اگر (گنتی_z > 0) {
	      buf[0] = 'Z';
	      buf[1] = 0;
	} ورنہ {
	      صحیح i;
	      صحیح zero_suppress=1;
	      اگر (comp) {
		    *buf++='-';
		    nbuf--;
		      /* printf("-"); */
	      }
	      برائے (i=vlen-1; i>=0; i--) {
		    zero_suppress = write_digits(valv[i],
						 &buf,&nbuf,zero_suppress);
		      /* printf(",%.4u",valv[i]); */
	      }
		/* Awkward special case, since we don't want to
		 * zero suppress down to nothing at all.  The only
		 * way we can still have zero_suppress on in the
		 * comp=1 case is if mbits==0, and therefore vlen==0.
		 * We represent 1'sb1 as "-1". */
	      اگر (zero_suppress) *buf++='0'+comp;
		/* printf("\n"); */
	      *buf='\0';
	}
	واپس 0;
}

خالی vpip_dec_str_مقصد_vec4(vvp_vector4_t&vec, ثابت حرف*buf)
{
	/* Support for [xX]_*. */
      اگر (buf[0] == 'x' || buf[0] == 'X') {
	    برائے (غیرعلامتی idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vec.سیٹ_ثنائی_ہندسہ(idx, BIT4_X);
	    }
	    ثابت حرف*tbuf = buf+1;
	      /* See if this is a valid constant. */
	    جب (*tbuf) {
		  اگر (*tbuf != '_') {
			fprintf(stderr, "Warning: Invalid decimal \"x\" "
			                "value \"%s\".\n", buf);
			واپس;
		  }
		  tbuf += 1;
	    }
	    واپس;
      }

	/* Support for [zZ]_*. */
      اگر (buf[0] == 'z' || buf[0] == 'Z') {
	    ثابت حرف*tbuf = buf+1;
	      /* See if this is a valid constant, if not return "x". */
	    جب (*tbuf) {
		  اگر (*tbuf != '_') {
			fprintf(stderr, "Warning: Invalid decimal \"z\" "
			                "value \"%s\".\n", buf);
			برائے (غیرعلامتی idx = 0 ;  idx < vec.size() ;  idx += 1) {
			      vec.سیٹ_ثنائی_ہندسہ(idx, BIT4_X);
			}
			واپس;
		  }
		  tbuf += 1;
	    }
	    برائے (غیرعلامتی idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vec.سیٹ_ثنائی_ہندسہ(idx, BIT4_Z);
	    }
	    واپس;
      }

	/* The str string is the decimal value with the least
	   significant digit first. This loop creates that string by
	   reversing the order of the buf string. For example, if the
	   input is "1234", str gets "4321". */
      غیرعلامتی slen = لڑی_لمبائی(buf);
      حرف*str = نیا حرف[slen + 1];
      منطقی is_negative = جھوٹ;
      برائے (غیرعلامتی idx = 0 ;  idx < slen ;  idx += 1) {
	    اگر (idx == slen-1 && buf[slen-idx-1] == '-') {
	          is_negative = سچ;
		  slen--;
		  جاری;
	    }
	    جب (buf[slen-idx-1] == '_') {
		  slen--;
	    }
	    اگر (ہندسہ_ہے(buf[slen-idx-1]))
		  str[idx] = buf[slen-idx-1];
            ورنہ {
		    /* Return "x" if there are invalid digits in the string. */
		  fprintf(stderr, "Warning: Invalid decimal digit %c(%d) in "
		          "\"%s.\"\n", buf[slen-idx-1], buf[slen-idx-1], buf);
		  برائے (غیرعلامتی jdx = 0 ;  jdx < vec.size() ;  jdx += 1) {
			vec.سیٹ_ثنائی_ہندسہ(jdx, BIT4_X);
		  }
		  واپس;
            }
      }

      str[slen] = 0;

      برائے (غیرعلامتی idx = 0 ;  idx < vec.size() ;  idx += 1) {
	    vvp_bit4_t val4 = BIT4_0;

	    انتخاب (str[0]) {
		حالت '1':
		حالت '3':
		حالت '5':
		حالت '7':
		حالت '9':
		  val4 = BIT4_1;
		  توڑو;
	    }

	    vec.سیٹ_ثنائی_ہندسہ(idx, val4);

	      /* Divide the str string by 2 in decimal. */
	    حرف*cp = str;
	    جب (*cp) {
		  غیرعلامتی val = cp[0] - '0';
		  اگر ((val&1) && (cp > str))
			cp[-1] += 5;

		  cp[0] = '0' + val/2;
		  cp += 1;
	    }

      }

      اگر (is_negative) {
            vec.invert();
            vec += (int64_t) 1;
      }

      حذف[]str;
}
