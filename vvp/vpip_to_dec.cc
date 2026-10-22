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


#जर SIZEOF_UNSIGNED_LONG * अक्षर_बिट >= 64
#define BDIGITS 9
#define BASE 1000000000
#define BBITS 32
#define BMASK 0xffffffff
#अन्यथा
#जर SIZEOF_UNSIGNED_LONG * अक्षर_बिट >= 32
#define BDIGITS 4
#define BASE 10000
#define BBITS 16
#define BMASK 0xffff
#अन्यथा
#error apparent non-conforming word length
#endif
#endif

#define B_IS0(x)  ((x) == 0)
#define B_IS1(x)  ((x) == 1)
#define B_ISX(x)  ((x) == 2)
#define B_ISZ(x)  ((x) == 3)

/* Jump through some hoops so we don't have to malloc/free valv
 * on every call, and implement an optional malloc-less version. */
स्थिर अचिन्हित दीर्घ *valv=रिक्त;
स्थिर अचिन्हित पूर्णांक vlen_alloc=0;

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
स्थिर ओळीत शून्य shift_in(अचिन्हित पूर्णांक vlen, अचिन्हित दीर्घ val)
{
	अचिन्हित पूर्णांक i;
	/* printf("shift in %u\n",val); */
	साठी (i=0; i<vlen; i++) {
		val=(valv[i]<<BBITS)+val;
		valv[i]=val%BASE;
		val=val/BASE;
	}
	जर (val!=0)
	      ख_लिखो(मानक_त्रुटि,"internal error: carry out %lu in " __FILE__ "\n",val);
}

/* Since BASE is a power of ten, conversion of each element of the
 * valv array to decimal is easy.  sprintf(buf,"%d",v) could be made
 * to work, I suppose, but for speed and control I prefer to write
 * the steps out longhand.
 */
स्थिर ओळीत पूर्णांक write_digits(अचिन्हित दीर्घ v, अक्षर **buf,
                               अचिन्हित पूर्णांक *nbuf, पूर्णांक zero_suppress)
{
	अक्षर segment[BDIGITS];
	पूर्णांक i;
	साठी (i=BDIGITS-1; i>=0; --i) {
		segment[i] = '0' + v%10;
		v=v/10;
	}
	साठी (i=0; i<BDIGITS; ++i) {
		जर (!(zero_suppress&=(segment[i]=='0'))) {
			*(*buf)++=segment[i]; --(*nbuf);
		}
	}
	परत zero_suppress;
}

#ifdef CHECK_WITH_VALGRIND
शून्य dec_str_delete(शून्य)
{
      मुक्त(valv);
      valv = 0;
      vlen_alloc = 0;
}
#endif

अचिन्हित vpip_vec4_to_dec_str(स्थिरांक vvp_vector4_t&vec4,
			      अक्षर *buf, अचिन्हित पूर्णांक nbuf,
			      पूर्णांक signed_flag)
{
      अचिन्हित पूर्णांक idx, vlen;
      अचिन्हित पूर्णांक mbits=vec4.size();   /* number of non-sign bits */
      अचिन्हित मोजणी_x = 0, मोजणी_z = 0;

      अचिन्हित दीर्घ val=0;
      पूर्णांक comp=0;
      जर (signed_flag) {
	    निवड (vec4.value(vec4.size()-1)) {
		स्थिती BIT4_X:
		  मोजणी_x += 1;
		  थांबा;
		स्थिती BIT4_Z:
		  मोजणी_z += 1;
		  थांबा;
		स्थिती BIT4_1:
		  comp=1;
		  थांबा;
		स्थिती BIT4_0:
		  थांबा;
	    }
	    mbits -= 1;
      }
      प्रतिज्ञा(mbits<(अच_पूर्णांक_उच्च-92)/28);
      vlen = ((mbits*28+92)/93+BDIGITS-1)/BDIGITS;
	/* printf("vlen=%d\n",vlen); */

#define ALLOC_MARGIN 4
      जर (!valv || vlen > vlen_alloc) {
	    जर (valv) मुक्त(valv);
	    valv = static_cast<अचिन्हित दीर्घ*>
	           (सुस्मृति(vlen+ALLOC_MARGIN, आकार (*valv)));
	    vlen_alloc=vlen+ALLOC_MARGIN;
      } अन्यथा {
	    स_रखो(valv,0,vlen*आकार(valv[0]));
      }

      साठी (idx = 0; idx < mbits; idx += 1) {
	      /* printf("%c ",bits[mbits-idx-1]); */
	    निवड (vec4.value(mbits-idx-1)) {
		स्थिती BIT4_Z:
		  मोजणी_z += 1;
		  थांबा;
		स्थिती BIT4_X:
		  मोजणी_x += 1;
		  थांबा;
		स्थिती BIT4_1:
		  जर (! comp)
			val += 1;
		  थांबा;
		स्थिती BIT4_0:
		  जर (comp)
			val += 1;
		  थांबा;
	    }

	    जर ((mbits-idx-1)%BBITS==0) {
		    /* make negative 2's complement, not 1's complement */
		  जर (comp && idx==mbits-1) ++val;
		  shift_in(vlen,val);
		  val=0;
	    } अन्यथा {
		  val=val+val;
	    }
      }

	जर (मोजणी_x == vec4.size()) {
	      buf[0] = 'x';
	      buf[1] = 0;
	} अन्यथा जर (मोजणी_x > 0) {
	      buf[0] = 'X';
	      buf[1] = 0;
	} अन्यथा जर (मोजणी_z == vec4.size()) {
	      buf[0] = 'z';
	      buf[1] = 0;
	} अन्यथा जर (मोजणी_z > 0) {
	      buf[0] = 'Z';
	      buf[1] = 0;
	} अन्यथा {
	      पूर्णांक i;
	      पूर्णांक zero_suppress=1;
	      जर (comp) {
		    *buf++='-';
		    nbuf--;
		      /* printf("-"); */
	      }
	      साठी (i=vlen-1; i>=0; i--) {
		    zero_suppress = write_digits(valv[i],
						 &buf,&nbuf,zero_suppress);
		      /* printf(",%.4u",valv[i]); */
	      }
		/* Awkward special case, since we don't want to
		 * zero suppress down to nothing at all.  The only
		 * way we can still have zero_suppress on in the
		 * comp=1 case is if mbits==0, and therefore vlen==0.
		 * We represent 1'sb1 as "-1". */
	      जर (zero_suppress) *buf++='0'+comp;
		/* printf("\n"); */
	      *buf='\0';
	}
	परत 0;
}

शून्य vpip_dec_str_to_vec4(vvp_vector4_t&vec, स्थिरांक अक्षर*buf)
{
	/* Support for [xX]_*. */
      जर (buf[0] == 'x' || buf[0] == 'X') {
	    साठी (अचिन्हित idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vec.set_bit(idx, BIT4_X);
	    }
	    स्थिरांक अक्षर*tbuf = buf+1;
	      /* See if this is a valid constant. */
	    पर्यंत (*tbuf) {
		  जर (*tbuf != '_') {
			ख_लिखो(मानक_त्रुटि, "Warning: Invalid decimal \"x\" "
			                "value \"%s\".\n", buf);
			परत;
		  }
		  tbuf += 1;
	    }
	    परत;
      }

	/* Support for [zZ]_*. */
      जर (buf[0] == 'z' || buf[0] == 'Z') {
	    स्थिरांक अक्षर*tbuf = buf+1;
	      /* See if this is a valid constant, if not return "x". */
	    पर्यंत (*tbuf) {
		  जर (*tbuf != '_') {
			ख_लिखो(मानक_त्रुटि, "Warning: Invalid decimal \"z\" "
			                "value \"%s\".\n", buf);
			साठी (अचिन्हित idx = 0 ;  idx < vec.size() ;  idx += 1) {
			      vec.set_bit(idx, BIT4_X);
			}
			परत;
		  }
		  tbuf += 1;
	    }
	    साठी (अचिन्हित idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vec.set_bit(idx, BIT4_Z);
	    }
	    परत;
      }

	/* The str string is the decimal value with the least
	   significant digit first. This loop creates that string by
	   reversing the order of the buf string. For example, if the
	   input is "1234", str gets "4321". */
      अचिन्हित slen = म_माप(buf);
      अक्षर*str = नवीन अक्षर[slen + 1];
      तार्किक is_negative = असत्य;
      साठी (अचिन्हित idx = 0 ;  idx < slen ;  idx += 1) {
	    जर (idx == slen-1 && buf[slen-idx-1] == '-') {
	          is_negative = सत्य;
		  slen--;
		  सुरूठेवा;
	    }
	    पर्यंत (buf[slen-idx-1] == '_') {
		  slen--;
	    }
	    जर (है_अंक(buf[slen-idx-1]))
		  str[idx] = buf[slen-idx-1];
            अन्यथा {
		    /* Return "x" if there are invalid digits in the string. */
		  ख_लिखो(मानक_त्रुटि, "Warning: Invalid decimal digit %c(%d) in "
		          "\"%s.\"\n", buf[slen-idx-1], buf[slen-idx-1], buf);
		  साठी (अचिन्हित jdx = 0 ;  jdx < vec.size() ;  jdx += 1) {
			vec.set_bit(jdx, BIT4_X);
		  }
		  परत;
            }
      }

      str[slen] = 0;

      साठी (अचिन्हित idx = 0 ;  idx < vec.size() ;  idx += 1) {
	    vvp_bit4_t val4 = BIT4_0;

	    निवड (str[0]) {
		स्थिती '1':
		स्थिती '3':
		स्थिती '5':
		स्थिती '7':
		स्थिती '9':
		  val4 = BIT4_1;
		  थांबा;
	    }

	    vec.set_bit(idx, val4);

	      /* Divide the str string by 2 in decimal. */
	    अक्षर*cp = str;
	    पर्यंत (*cp) {
		  अचिन्हित val = cp[0] - '0';
		  जर ((val&1) && (cp > str))
			cp[-1] += 5;

		  cp[0] = '0' + val/2;
		  cp += 1;
	    }

      }

      जर (is_negative) {
            vec.invert();
            vec += (int64_t) 1;
      }

      delete[]str;
}
