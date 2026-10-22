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
#నిర్వచితమైతే CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#అయితే_ముగింపు
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


#ఒకవేళ SIZEOF_UNSIGNED_LONG * CHAR_BIT >= 64
#నిర్వచించు BDIGITS 9
#నిర్వచించు పునాది_3 1000000000
#నిర్వచించు BBITS 32
#నిర్వచించు BMASK 0xffffffff
#లేకపోతే
#ఒకవేళ SIZEOF_UNSIGNED_LONG * CHAR_BIT >= 32
#నిర్వచించు BDIGITS 4
#నిర్వచించు పునాది_3 10000
#నిర్వచించు BBITS 16
#నిర్వచించు BMASK 0xffff
#లేకపోతే
#దోషం apparent non-conforming మాట length
#అయితే_ముగింపు
#అయితే_ముగింపు

#నిర్వచించు B_IS0(x)  ((x) == 0)
#నిర్వచించు B_IS1(x)  ((x) == 1)
#నిర్వచించు B_ISX(x)  ((x) == 2)
#నిర్వచించు B_ISZ(x)  ((x) == 3)

/* Jump through some hoops so we don't have to malloc/free valv
 * on every call, and implement an optional malloc-less version. */
స్థిర గుర్తులేని దీర్ఘ *valv=ఖాళీ;
స్థిర గుర్తులేని పూర్ణాంకం vlen_alloc=0;

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
స్థిర అంతఃవరుస శూన్య జరుపు_లోపల(గుర్తులేని పూర్ణాంకం vlen, గుర్తులేని దీర్ఘ val)
{
	గుర్తులేని పూర్ణాంకం i;
	/* printf("shift in %u\n",val); */
	కొరకు (i=0; i<vlen; i++) {
		val=(valv[i]<<BBITS)+val;
		valv[i]=val%పునాది_3;
		val=val/పునాది_3;
	}
	ఒకవేళ (val!=0)
	      fprintf(stderr,"internal error: carry out %lu in " __FILE__ "\n",val);
}

/* Since BASE is a power of ten, conversion of each element of the
 * valv array to decimal is easy.  sprintf(buf,"%d",v) could be made
 * to work, I suppose, but for speed and control I prefer to write
 * the steps out longhand.
 */
స్థిర అంతఃవరుస పూర్ణాంకం write_digits(గుర్తులేని దీర్ఘ v, అక్షరం **buf,
                               గుర్తులేని పూర్ణాంకం *nbuf, పూర్ణాంకం zero_suppress)
{
	అక్షరం segment[BDIGITS];
	పూర్ణాంకం i;
	కొరకు (i=BDIGITS-1; i>=0; --i) {
		segment[i] = '0' + v%10;
		v=v/10;
	}
	కొరకు (i=0; i<BDIGITS; ++i) {
		ఒకవేళ (!(zero_suppress&=(segment[i]=='0'))) {
			*(*buf)++=segment[i]; --(*nbuf);
		}
	}
	తిరిగి zero_suppress;
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
శూన్య dec_str_delete(శూన్య)
{
      విడుదల(valv);
      valv = 0;
      vlen_alloc = 0;
}
#అయితే_ముగింపు

గుర్తులేని vpip_vec4_గమ్యం_dec_str(స్థిరరాశి vvp_vector4_t&vec4,
			      అక్షరం *buf, గుర్తులేని పూర్ణాంకం nbuf,
			      పూర్ణాంకం signed_flag)
{
      గుర్తులేని పూర్ణాంకం idx, vlen;
      గుర్తులేని పూర్ణాంకం mbits=vec4.size();   /* number of non-sign bits */
      గుర్తులేని లెక్క_x = 0, లెక్క_z = 0;

      గుర్తులేని దీర్ఘ val=0;
      పూర్ణాంకం comp=0;
      ఒకవేళ (signed_flag) {
	    ఎంపిక (vec4.value(vec4.size()-1)) {
		స్థితి BIT4_X:
		  లెక్క_x += 1;
		  ఆపు;
		స్థితి BIT4_Z:
		  లెక్క_z += 1;
		  ఆపు;
		స్థితి BIT4_1:
		  comp=1;
		  ఆపు;
		స్థితి BIT4_0:
		  ఆపు;
	    }
	    mbits -= 1;
      }
      ప్రతిజ్ఞా(mbits<(UINT_MAX-92)/28);
      vlen = ((mbits*28+92)/93+BDIGITS-1)/BDIGITS;
	/* printf("vlen=%d\n",vlen); */

#నిర్వచించు ALLOC_MARGIN 4
      ఒకవేళ (!valv || vlen > vlen_alloc) {
	    ఒకవేళ (valv) విడుదల(valv);
	    valv = static_cast<గుర్తులేని దీర్ఘ*>
	           (సున్నా_జ్ఞాపకం_ఇవ్వు(vlen+ALLOC_MARGIN, పరిమాణం (*valv)));
	    vlen_alloc=vlen+ALLOC_MARGIN;
      } లేకపోతే {
	    memset(valv,0,vlen*పరిమాణం(valv[0]));
      }

      కొరకు (idx = 0; idx < mbits; idx += 1) {
	      /* printf("%c ",bits[mbits-idx-1]); */
	    ఎంపిక (vec4.value(mbits-idx-1)) {
		స్థితి BIT4_Z:
		  లెక్క_z += 1;
		  ఆపు;
		స్థితి BIT4_X:
		  లెక్క_x += 1;
		  ఆపు;
		స్థితి BIT4_1:
		  ఒకవేళ (! comp)
			val += 1;
		  ఆపు;
		స్థితి BIT4_0:
		  ఒకవేళ (comp)
			val += 1;
		  ఆపు;
	    }

	    ఒకవేళ ((mbits-idx-1)%BBITS==0) {
		    /* make negative 2's complement, not 1's complement */
		  ఒకవేళ (comp && idx==mbits-1) ++val;
		  జరుపు_లోపల(vlen,val);
		  val=0;
	    } లేకపోతే {
		  val=val+val;
	    }
      }

	ఒకవేళ (లెక్క_x == vec4.size()) {
	      buf[0] = 'x';
	      buf[1] = 0;
	} లేకపోతే ఒకవేళ (లెక్క_x > 0) {
	      buf[0] = 'X';
	      buf[1] = 0;
	} లేకపోతే ఒకవేళ (లెక్క_z == vec4.size()) {
	      buf[0] = 'z';
	      buf[1] = 0;
	} లేకపోతే ఒకవేళ (లెక్క_z > 0) {
	      buf[0] = 'Z';
	      buf[1] = 0;
	} లేకపోతే {
	      పూర్ణాంకం i;
	      పూర్ణాంకం zero_suppress=1;
	      ఒకవేళ (comp) {
		    *buf++='-';
		    nbuf--;
		      /* printf("-"); */
	      }
	      కొరకు (i=vlen-1; i>=0; i--) {
		    zero_suppress = write_digits(valv[i],
						 &buf,&nbuf,zero_suppress);
		      /* printf(",%.4u",valv[i]); */
	      }
		/* Awkward special case, since we don't want to
		 * zero suppress down to nothing at all.  The only
		 * way we can still have zero_suppress on in the
		 * comp=1 case is if mbits==0, and therefore vlen==0.
		 * We represent 1'sb1 as "-1". */
	      ఒకవేళ (zero_suppress) *buf++='0'+comp;
		/* printf("\n"); */
	      *buf='\0';
	}
	తిరిగి 0;
}

శూన్య vpip_dec_str_గమ్యం_vec4(vvp_vector4_t&vec, స్థిరరాశి అక్షరం*buf)
{
	/* Support for [xX]_*. */
      ఒకవేళ (buf[0] == 'x' || buf[0] == 'X') {
	    కొరకు (గుర్తులేని idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vec.సమితి_బిట్టు(idx, BIT4_X);
	    }
	    స్థిరరాశి అక్షరం*tbuf = buf+1;
	      /* See if this is a valid constant. */
	    అయితే (*tbuf) {
		  ఒకవేళ (*tbuf != '_') {
			fprintf(stderr, "Warning: Invalid decimal \"x\" "
			                "value \"%s\".\n", buf);
			తిరిగి;
		  }
		  tbuf += 1;
	    }
	    తిరిగి;
      }

	/* Support for [zZ]_*. */
      ఒకవేళ (buf[0] == 'z' || buf[0] == 'Z') {
	    స్థిరరాశి అక్షరం*tbuf = buf+1;
	      /* See if this is a valid constant, if not return "x". */
	    అయితే (*tbuf) {
		  ఒకవేళ (*tbuf != '_') {
			fprintf(stderr, "Warning: Invalid decimal \"z\" "
			                "value \"%s\".\n", buf);
			కొరకు (గుర్తులేని idx = 0 ;  idx < vec.size() ;  idx += 1) {
			      vec.సమితి_బిట్టు(idx, BIT4_X);
			}
			తిరిగి;
		  }
		  tbuf += 1;
	    }
	    కొరకు (గుర్తులేని idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vec.సమితి_బిట్టు(idx, BIT4_Z);
	    }
	    తిరిగి;
      }

	/* The str string is the decimal value with the least
	   significant digit first. This loop creates that string by
	   reversing the order of the buf string. For example, if the
	   input is "1234", str gets "4321". */
      గుర్తులేని slen = అక్షరశ్రేణి_పొడవు(buf);
      అక్షరం*str = కొత్త అక్షరం[slen + 1];
      తార్కిక is_negative = అబద్ధం;
      కొరకు (గుర్తులేని idx = 0 ;  idx < slen ;  idx += 1) {
	    ఒకవేళ (idx == slen-1 && buf[slen-idx-1] == '-') {
	          is_negative = నిజం;
		  slen--;
		  కొనసాగు;
	    }
	    అయితే (buf[slen-idx-1] == '_') {
		  slen--;
	    }
	    ఒకవేళ (అంకెనా(buf[slen-idx-1]))
		  str[idx] = buf[slen-idx-1];
            లేకపోతే {
		    /* Return "x" if there are invalid digits in the string. */
		  fprintf(stderr, "Warning: Invalid decimal digit %c(%d) in "
		          "\"%s.\"\n", buf[slen-idx-1], buf[slen-idx-1], buf);
		  కొరకు (గుర్తులేని jdx = 0 ;  jdx < vec.size() ;  jdx += 1) {
			vec.సమితి_బిట్టు(jdx, BIT4_X);
		  }
		  తిరిగి;
            }
      }

      str[slen] = 0;

      కొరకు (గుర్తులేని idx = 0 ;  idx < vec.size() ;  idx += 1) {
	    vvp_bit4_t val4 = BIT4_0;

	    ఎంపిక (str[0]) {
		స్థితి '1':
		స్థితి '3':
		స్థితి '5':
		స్థితి '7':
		స్థితి '9':
		  val4 = BIT4_1;
		  ఆపు;
	    }

	    vec.సమితి_బిట్టు(idx, val4);

	      /* Divide the str string by 2 in decimal. */
	    అక్షరం*cp = str;
	    అయితే (*cp) {
		  గుర్తులేని val = cp[0] - '0';
		  ఒకవేళ ((val&1) && (cp > str))
			cp[-1] += 5;

		  cp[0] = '0' + val/2;
		  cp += 1;
	    }

      }

      ఒకవేళ (is_negative) {
            vec.invert();
            vec += (int64_t) 1;
      }

      తొలగించు[]str;
}
