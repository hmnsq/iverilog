/*
 * Copyright (c) 2001-2026 Stephen Williams (steve@icarus.com)
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

/*
 * vpiReg handles are handled here. These objects represent vectors of
 * .var objects that can be manipulated by the VPI module.
 */

# include  "compile.h"
# include  "vpi_priv.h"
# include  "vvp_net_sig.h"
# include  "vvp_island.h"
# include  "schedule.h"
# include  "statistics.h"
# include  "config.h"
#ifdef CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#endif
# include  <cmath>
# include  <iostream>
# include  <cstdio>
# include  <cstdlib>
# include  <climits>
# include  <cstring>
# include  <cassert>
#ifdef CHECK_WITH_VALGRIND
# include  <valgrind/memcheck.h>
#endif
# include  "ivl_alloc.h"

/*
 * Hex digits that represent 4-value bits of Verilog are not as
 * trivially obvious to display as if the bits were the usual 2-value
 * bits. So, although it is possible to write a function that
 * generates a correct character for 4*4-value bits, it is easier to
 * just perform the lookup in a table. This only takes 256 bytes,
 * which is not many executable instructions:-)
 *
 * The table is calculated at compile time, therefore, by the
 * draw_tt.c program.
 */
বাহ্যিক ধ্রুবক অক্ষর hex_digits[256];
বাহ্যিক ধ্রুবক অক্ষর oct_digits[64];

/*
 * The string values need a result buf to hold the results. This
 * buffer can be reused for that purpose. Whenever I have a need, the
 * need_result_buf function makes sure that need can be met.
 */
শূন্য *need_ফলাফল_buf(unsigned cnt, vpi_rbuf_t type)
{
      স্থির শূন্য*ফলাফল_buf[2] = {0, 0};
      স্থির size_t ফলাফল_buf_আকার[2] = {0, 0};

      যদি (type == RBUF_DEL) {
	    মুক্ত(ফলাফল_buf[RBUF_VAL]);
	    ফলাফল_buf[RBUF_VAL] = 0;
	    ফলাফল_buf_আকার[RBUF_VAL] = 0;

	    মুক্ত(ফলাফল_buf[RBUF_STR]);
	    ফলাফল_buf[RBUF_STR] = 0;
	    ফলাফল_buf_আকার[RBUF_STR] = 0;

	    ফেরত 0;
      }

      cnt = (cnt + 0x0fff) & ~0x0fff;

      যদি (ফলাফল_buf_আকার[type] == 0) {
	    ফলাফল_buf[type] = malloc(cnt);
	    ফলাফল_buf_আকার[type] = cnt;
      } অন্যথা যদি (ফলাফল_buf_আকার[type] < cnt) {
	    ফলাফল_buf[type] = realloc(ফলাফল_buf[type], cnt);
	    ফলাফল_buf_আকার[type] = cnt;
      }

      ফেরত ফলাফল_buf[type];
}

অক্ষর *simple_set_rbuf_str(ধ্রুবক অক্ষর *s1)
{
      অক্ষর *res = static_cast<অক্ষর *>(need_ফলাফল_buf(strlen(s1)+1, RBUF_STR));
      যদি (res) strcpy(res,s1);
      ফেরত res;
}

অক্ষর *generic_get_str(পূর্ণসংখ্যা code, vpiHandle ref, ধ্রুবক অক্ষর *name, ধ্রুবক অক্ষর *সূচক)
{
      size_t len = strlen(name) + 1;  /* include space for null termination */
      অক্ষর *bn = ফাঁকা;
      যৌক্তিক is_bn_a_pkg = মিথ্যা;
      যদি (code == vpiFullName) {
	    bn = strdup(vpi_get_str(code,ref));
	    size_t bn_len = strlen(bn);
	    is_bn_a_pkg = (bn_len >= 3) && (bn[bn_len-1] == ':') && (bn[bn_len-2] == ':');
	    len += bn_len;
	    যদি (! is_bn_a_pkg) len += 1; // include space for "." separator
      }
      যদি (সূচক != ফাঁকা) len += strlen(সূচক) + 2;  /* include space for brackets */

      অক্ষর *res = static_cast<অক্ষর *>(need_ফলাফল_buf(len, RBUF_STR));
      যদি (!res) {
	    মুক্ত(bn);
	    ফেরত ফাঁকা;
      }
      *res=0;  /* start with nothing */

	/* if this works, I can make it more efficient later */
      যদি (bn != ফাঁকা) {
	    strcat(res, bn);
	      // A package already has the "::" separator in the name
	    যদি (! is_bn_a_pkg) strcat(res, ".");
	    মুক্ত(bn);
      }
      strcat(res, name);
      যদি (সূচক != ফাঁকা) {
	    strcat(res, "[");
	    strcat(res, সূচক);
	    strcat(res, "]");
      }
      ফেরত res;
}

স্থির vpiHandle fill_in_net4(কাঠামো __vpiSignal*obj, __vpiScope*scope,
                              ধ্রুবক অক্ষর*name, পূর্ণসংখ্যা msb, পূর্ণসংখ্যা lsb,
                              যৌক্তিক signed_flag, vvp_net_t*গ্রন্থি);

স্থির vpiHandle fill_in_var4(কাঠামো __vpiSignal*obj,
                              ধ্রুবক অক্ষর*name, পূর্ণসংখ্যা msb, পূর্ণসংখ্যা lsb,
                              যৌক্তিক signed_flag, vvp_net_t*গ্রন্থি)
{
	// Variable declarations are always resolved immediately,
	// so we can assume they belong in the current scope.
      ফেরত fill_in_net4(obj, vpip_peek_current_scope(),
			  name, msb, lsb, signed_flag, গ্রন্থি);
}

/*
 * The standard formatting/conversion routines.
 * They work with full or partial signals.
 */

স্থির শূন্য format_vpiBinStrVal(ধ্রুবক vvp_signal_মান*sig, পূর্ণসংখ্যা base, unsigned wid,
                                s_vpi_মান*vp)
{
      অক্ষর *rbuf = static_cast<অক্ষর *>(need_ফলাফল_buf(wid+1, RBUF_VAL));
      long end = base + (signed)wid;
      long offset = end - 1;
      long ssize = (signed)sig->মান_আকার();

      জন্য (long idx = base ;  idx < end ;  idx += 1) {
	    যদি (idx < 0 || idx >= ssize) {
                  rbuf[offset-idx] = 'x';
	    } অন্যথা {
                  rbuf[offset-idx] = vvp_bit4_to_ascii(sig->value(idx));
	    }
      }
      rbuf[wid] = 0;

      vp->value.str = rbuf;
}

স্থির শূন্য format_vpiOctStrVal(ধ্রুবক vvp_signal_মান*sig, পূর্ণসংখ্যা base, unsigned wid,
                                s_vpi_মান*vp)
{
      unsigned dwid = (wid + 2) / 3;
      অক্ষর *rbuf = static_cast<অক্ষর *>(need_ফলাফল_buf(dwid+1, RBUF_VAL));
      long end = base + (signed)wid;
      long ssize = (signed)sig->মান_আকার();
      unsigned val = 0;

      rbuf[dwid] = 0;
      জন্য (long idx = base ;  idx < end ;  idx += 1) {
	    unsigned bit = 0;
	    যদি (idx < 0 || idx >= ssize) {
                  bit = 2; // BIT4_X
	    } অন্যথা {
                  নির্বাচন (sig->value(idx)) {
		      ক্ষেত্র BIT4_0:
			bit = 0;
			বিরতি;
		      ক্ষেত্র BIT4_1:
			bit = 1;
			বিরতি;
		      ক্ষেত্র BIT4_X:
			bit = 2;
			বিরতি;
		      ক্ষেত্র BIT4_Z:
			bit = 3;
			বিরতি;
                  }
	    }
	    val |= bit << 2*((idx-base) % 3);

	    যদি ((idx-base) % 3 == 2) {
		dwid -= 1;
		rbuf[dwid] = oct_digits[val];
		val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      নির্বাচন (wid % 3) {
	  ক্ষেত্র 1:
	    যদি (val == 2) val = 42;
	    অন্যথা যদি (val == 3) val = 63;
	    বিরতি;
	  ক্ষেত্র 2:
	    যদি (val == 10) val = 42;
	    অন্যথা যদি (val == 15) val = 63;
	    বিরতি;
      }

      যদি (dwid > 0) rbuf[0] = oct_digits[val];

      vp->value.str = rbuf;
}

স্থির শূন্য format_vpiHexStrVal(ধ্রুবক vvp_signal_মান*sig, পূর্ণসংখ্যা base, unsigned wid,
                                s_vpi_মান*vp)
{
      unsigned dwid = (wid + 3) / 4;
      অক্ষর *rbuf = static_cast<অক্ষর *>(need_ফলাফল_buf(dwid+1, RBUF_VAL));
      long end = base + (signed)wid;
      long ssize = (signed)sig->মান_আকার();
      unsigned val = 0;

      rbuf[dwid] = 0;
      জন্য (long idx = base ;  idx < end ;  idx += 1) {
	    unsigned bit = 0;
	    যদি (idx < 0 || idx >= ssize) {
                  bit = 2; // BIT4_X
	    } অন্যথা {
                  নির্বাচন (sig->value(idx)) {
		      ক্ষেত্র BIT4_0:
			bit = 0;
			বিরতি;
		      ক্ষেত্র BIT4_1:
			bit = 1;
			বিরতি;
		      ক্ষেত্র BIT4_X:
			bit = 2;
			বিরতি;
		      ক্ষেত্র BIT4_Z:
			bit = 3;
			বিরতি;
                  }
	    }
	    val |= bit << 2*((idx-base) % 4);

	    যদি ((idx-base) % 4 == 3) {
		dwid -= 1;
		rbuf[dwid] = hex_digits[val];
		val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      নির্বাচন (wid % 4) {
	  ক্ষেত্র 1:
	    যদি (val == 2) val = 170;
	    অন্যথা যদি (val == 3) val = 255;
	    বিরতি;
	  ক্ষেত্র 2:
	    যদি (val == 10) val = 170;
	    অন্যথা যদি (val == 15) val = 255;
	    বিরতি;
	  ক্ষেত্র 3:
	    যদি (val == 42) val = 170;
	    অন্যথা যদি (val == 63) val = 255;
	    বিরতি;
      }

      যদি (dwid > 0) rbuf[0] = hex_digits[val];

      vp->value.str = rbuf;
}

স্থির শূন্য format_vpiDecStrVal(ধ্রুবক vvp_signal_মান*sig, পূর্ণসংখ্যা base, unsigned wid,
                                পূর্ণসংখ্যা signed_flag, s_vpi_মান*vp)
{
      unsigned hwid = (sig->মান_আকার()+2) / 3 + 1;
      অক্ষর *rbuf = static_cast<অক্ষর *>(need_ফলাফল_buf(hwid, RBUF_VAL));
      long ssize = (signed)sig->মান_আকার();
      long end = base + (signed)wid;

	/* Do we have an end outside of the real signal vector. */
      যদি (base < 0 || end > ssize) {
	    যৌক্তিক all_x = সত্য;
	    যদি (end > ssize) end = ssize;
	    যদি (base < 0) base = 0;
	    জন্য (long idx = base ;  idx < end ;  idx += 1) {
		  যদি (sig->value(idx) != BIT4_X) {
			all_x = মিথ্যা;
			বিরতি;
		  }
	    }

	    যদি (all_x) {
		  rbuf[0] = 'x';
	    } অন্যথা {
		  rbuf[0] = 'X';
	    }
	    rbuf[1] = 0;

	    vp->value.str = rbuf;
	    ফেরত;
      }

      vvp_vector4_t vec4;
      যদি (base == 0 && end == ssize) {
	    sig->vec4_মান(vec4);
      } অন্যথা {
	    vvp_vector4_t tmp;
	    sig->vec4_মান(tmp);
	    vec4 = tmp.subvalue(base, wid);
      }

      vpip_vec4_to_dec_str(vec4, rbuf, hwid, signed_flag);

      vp->value.str = rbuf;
}

স্থির শূন্য format_vpiIntVal(ধ্রুবক vvp_signal_মান*sig, পূর্ণসংখ্যা base, unsigned wid,
                             পূর্ণসংখ্যা signed_flag, s_vpi_মান*vp)
{
      vvp_vector4_t tmp;
      sig->vec4_মান(tmp);
      vvp_vector4_t sub = tmp.subvalue(base, wid);

	// Normally, we'd be OK with just using long in the call to
	// vector4_to_value, but some compilers seem to take long as
	// distinct from int32_t AND int64_t. Since the condition is
	// constant, the compiler should eliminate the dead code.
      যদি (আকার(vp->value.integer) == আকার(int32_t)) {
	    int32_t val = 0;
	    vector4_to_মান(sub, val, signed_flag, মিথ্যা);
	    vp->value.integer = val;
      } অন্যথা {
	    প্রতিজ্ঞা(আকার(vp->value.integer) == আকার(int64_t));
	    int64_t val = 0;
	    vector4_to_মান(sub, val, signed_flag, মিথ্যা);
	    vp->value.integer = val;
      }
}

স্থির শূন্য format_vpiRealVal(ধ্রুবক vvp_signal_মান*sig, পূর্ণসংখ্যা base, unsigned wid,
                              পূর্ণসংখ্যা signed_flag, s_vpi_মান*vp)
{
      vvp_vector4_t vec4(wid);
      long ssize = (signed)sig->মান_আকার();
      long end = base + (signed)wid;
      যদি (end > ssize) end = ssize;

      জন্য (long idx = (base < 0) ? 0 : base ;  idx < end ;  idx += 1) {
	    vec4.set_bit(idx-base, sig->value(idx));
      }

      vp->value.real = 0.0;
      vector4_to_মান(vec4, vp->value.real, signed_flag);
}

স্থির শূন্য format_vpiStringVal(ধ্রুবক vvp_signal_মান*sig, পূর্ণসংখ্যা base, unsigned wid,
                                s_vpi_মান*vp)
{
      /* The result will use a character for each 8 bits of the
	 vector. Add one extra character for the highest bits that
	 don't form an 8 bit group. */
      অক্ষর *rbuf = static_cast<অক্ষর *>(need_ফলাফল_buf(wid/8 + ((wid&7)!=0) + 1, RBUF_VAL));
      অক্ষর *cp = rbuf;

      অক্ষর tmp = 0;
      জন্য (long idx = base+(signed)wid-1; idx >= base; idx -= 1) {
	    tmp <<= 1;

	    যদি (idx >=0 && idx < (signed)sig->মান_আকার() &&
	        sig->value(idx) == BIT4_1) {
		   tmp |= 1;
	    }

	    যদি (((idx-base)&7)==0){
		  /* Skip leading nulls. */
		  যদি (tmp == 0 && cp == rbuf)
			চালু;

		  /* Nulls in the middle get turned into spaces. */
		  *cp++ = tmp ? tmp : ' ';
		  tmp = 0;
	    }
      }
      *cp++ = 0;

      vp->value.str = rbuf;
}

স্থির শূন্য format_vpiScalarVal(ধ্রুবক vvp_signal_মান*sig, পূর্ণসংখ্যা base,
                                s_vpi_মান*vp)
{
      যদি (base >= 0 && base < (signed)sig->মান_আকার()) {
	    নির্বাচন (sig->value(base)) {
		ক্ষেত্র BIT4_0:
		  vp->value.scalar = vpi0;
		  বিরতি;
		ক্ষেত্র BIT4_1:
		  vp->value.scalar = vpi1;
		  বিরতি;
		ক্ষেত্র BIT4_X: {
		  vvp_scalar_t strn = sig->scalar_মান(base);
		  যদি (strn.strength0() == 1) vp->value.scalar = vpiH;
		  অন্যথা যদি (strn.strength1() == 1) vp->value.scalar = vpiL;
		  অন্যথা vp->value.scalar = vpiX;
		  বিরতি;
		}
		ক্ষেত্র BIT4_Z:
		  vp->value.scalar = vpiZ;
		  বিরতি;
	    }
      } অন্যথা {
	    vp->value.scalar = vpiX;
      }
}

স্থির শূন্য format_vpiStrengthVal(ধ্রুবক vvp_signal_মান*sig, পূর্ণসংখ্যা base,
                                  unsigned wid, s_vpi_মান*vp)
{
      long end = base + (signed)wid;
      s_vpi_strengthval*op;

      op = static_cast<s_vpi_strengthval*>
           (need_ফলাফল_buf(wid * আকার(s_vpi_strengthval), RBUF_VAL));

      জন্য (long idx = base ;  idx < end ;  idx += 1) {
	    যদি (idx >=0 && idx < (signed)sig->মান_আকার()) {
		  vvp_scalar_t val = sig->scalar_মান(idx);

		  /* vvp_scalar_t strengths are 0-7, but the vpi strength
		     is bit0-bit7. This gets the vpi form of the strengths
		     from the vvp_scalar_t strengths. */
		  unsigned s0 = 1 << val.strength0();
		  unsigned s1 = 1 << val.strength1();

		  নির্বাচন (val.value()) {
		      ক্ষেত্র BIT4_0:
			op[idx-base].logic = vpi0;
			op[idx-base].s0 = s0|s1;
			op[idx-base].s1 = 0;
			বিরতি;

		      ক্ষেত্র BIT4_1:
			op[idx-base].logic = vpi1;
			op[idx-base].s0 = 0;
			op[idx-base].s1 = s0|s1;
			বিরতি;

		      ক্ষেত্র BIT4_X:
			op[idx-base].logic = vpiX;
			op[idx-base].s0 = s0;
			op[idx-base].s1 = s1;
			বিরতি;

		      ক্ষেত্র BIT4_Z:
			op[idx-base].logic = vpiZ;
			op[idx-base].s0 = vpiHiZ;
			op[idx-base].s1 = vpiHiZ;
			বিরতি;
		  }
	    } অন্যথা {
		  op[idx-base].logic = vpiX;
		  op[idx-base].s0 = vpiStrongDrive;
		  op[idx-base].s1 = vpiStrongDrive;
	    }
      }

      vp->value.strength = op;
}

স্থির শূন্য format_vpiVectorVal(ধ্রুবক vvp_signal_মান*sig, পূর্ণসংখ্যা base, unsigned wid,
                                s_vpi_মান*vp)
{
      long end = base + (signed)wid;
      unsigned পূর্ণসংখ্যা obit = 0;
      unsigned hwid = (wid + 31)/32;

      s_vpi_vecval *op = static_cast<p_vpi_vecval>
                         (need_ফলাফল_buf(hwid * আকার(s_vpi_vecval), RBUF_VAL));
      vp->value.vector = op;

      op->aval = op->bval = 0;
      জন্য (long idx = base ;  idx < end ;  idx += 1) {
	    যদি (base >= 0 && base < (signed)sig->মান_আকার()) {
		নির্বাচন (sig->value(idx)) {
		ক্ষেত্র BIT4_0:
		  op->aval &= ~(1 << obit);
		  op->bval &= ~(1 << obit);
		  বিরতি;
		ক্ষেত্র BIT4_1:
		  op->aval |= (1 << obit);
		  op->bval &= ~(1 << obit);
		  বিরতি;
		ক্ষেত্র BIT4_X:
		  op->aval |= (1 << obit);
		  op->bval |= (1 << obit);
		  বিরতি;
		ক্ষেত্র BIT4_Z:
		  op->aval &= ~(1 << obit);
		  op->bval |= (1 << obit);
		  বিরতি;
		}
	    } অন্যথা {  /* BIT4_X */
		  op->aval |= (1 << obit);
		  op->bval |= (1 << obit);
	    }

	    obit++;
	    যদি (!(obit % 32)) {
		  op += 1;
		  যদি ((op - vp->value.vector) < (ptrdiff_t)hwid)
			op->aval = op->bval = 0;
		  obit = 0;
	    }
      }
}

/*
 * implement vpi_get for vpiReg objects.
 */
স্থির পূর্ণসংখ্যা signal_get(পূর্ণসংখ্যা code, vpiHandle ref)
{
      কাঠামো __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      প্রতিজ্ঞা(rfp);

      নির্বাচন (code) {
	  ক্ষেত্র vpi_সারি_No:
	    ফেরত 0;  // Not implemented for now!

	  ক্ষেত্র vpiSigned:
	    ফেরত rfp->signed_flag != 0;

	  ক্ষেত্র vpiArray:
	    ফেরত rfp->is_netarray != 0;

	  ক্ষেত্র vpi_সূচক_2: // This only works while we have a single index.
	    যদি (rfp->is_netarray) {
		  s_vpi_মান vp;
		  vp.format = vpiIntVal;
		  vpi_get_মান(rfp->id.সূচক, &vp);
		  ফেরত vp.value.integer;
	    } অন্যথা {
		  ফেরত vpiUndefined;
	    }

	  ক্ষেত্র vpi_আকার:
	    ফেরত rfp->width();

	  ক্ষেত্র vpiNetType:
	    যদি (ref->get_type_code()==vpiNet)
		  ফেরত vpiWire;
	    অন্যথা
		  ফেরত vpiUndefined;

	  ক্ষেত্র vpi_বাম_Range:
            ফেরত rfp->msb.get_মান();

	  ক্ষেত্র vpi_ডান_Range:
            ফেরত rfp->lsb.get_মান();

	  ক্ষেত্র vpiScalar:
	    ফেরত (rfp->msb.get_মান() == 0 && rfp->lsb.get_মান() == 0);
	  ক্ষেত্র vpiVector:
	    ফেরত (rfp->msb.get_মান() != rfp->lsb.get_মান());

          ক্ষেত্র vpiAutomatic:
            ফেরত vpip_scope(rfp)->is_automatic() ? 1  : 0;

#ifdef BR916_STOPGAP_FIX
          ক্ষেত্র _vpiFromThr:
            ফেরত _vpiNoThr;
#endif

	  পূর্বনির্ধারিত:
	    fprintf(stderr, "VPI error: unknown signal_get property %d.\n",
	            code);
	    ফেরত vpiUndefined;
      }
}

/*
 * implement vpi_get64 for vpiReg objects.
 */
স্থির int64_t signal_get64(পূর্ণসংখ্যা code, vpiHandle ref)
{
      কাঠামো __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      প্রতিজ্ঞা(rfp);

      নির্বাচন (code) {
	    // This private property must return zero when undefined.
	  ক্ষেত্র _vpiNexusId:
	    যদি (rfp->msb.get_মান() == rfp->lsb.get_মান())
		  ফেরত reinterpret_cast<int64_t>(rfp->গ্রন্থি);
	    ফেরত 0;

	  পূর্বনির্ধারিত:
	    fprintf(stderr, "VPI error: unknown signal_get64 property %d.\n",
	            code);
	    ফেরত vpiUndefined;
      }
}

স্থির অক্ষর* signal_get_str(পূর্ণসংখ্যা code, vpiHandle ref)
{
      কাঠামো __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      প্রতিজ্ঞা(rfp);

      যদি (code == vpiFile) {  // Not implemented for now!
	    ফেরত simple_set_rbuf_str(file_names[0]);
      }

      যদি ((code != vpiName) && (code != vpiFullName)) ফেরত ফাঁকা;

      অক্ষর *nm;
      ধ্রুবক অক্ষর *ixs;
      যদি (rfp->is_netarray) {
	    nm = strdup(vpi_get_str(vpiName, rfp->within.parent));
	    s_vpi_মান vp;
	    vp.format = vpiDecStrVal;
	    vpi_get_মান(rfp->id.সূচক, &vp);
	    ixs = vp.value.str;  /* do I need to strdup() this? */
      } অন্যথা {
	    nm = strdup(rfp->id.name);
	    ixs = ফাঁকা;
      }

	/* The scope information is added here for vpiFullName. */
      অক্ষর *rbuf = generic_get_str(code, vpip_scope(rfp), nm, ixs);
      মুক্ত(nm);
      ফেরত rbuf;
}

স্থির vpiHandle signal_get_handle(পূর্ণসংখ্যা code, vpiHandle ref)
{
      কাঠামো __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      প্রতিজ্ঞা(rfp);

      নির্বাচন (code) {

	  ক্ষেত্র vpiParent:
	    ফেরত rfp->is_netarray? rfp->within.parent : ফাঁকা;

	  ক্ষেত্র vpi_সূচক_2:
	    ফেরত rfp->is_netarray? rfp->id.সূচক : ফাঁকা;

	  ক্ষেত্র vpi_বাম_Range:
	    ফেরত &rfp->msb;
	  ক্ষেত্র vpi_ডান_Range:
	    ফেরত &rfp->lsb;

	  ক্ষেত্র vpiScope:
	    ফেরত vpip_scope(rfp);

	  ক্ষেত্র vpiModule:
	    ফেরত vpip_module(vpip_scope(rfp));
      }

      ফেরত 0;
}

স্থির vpiHandle signal_iterate(পূর্ণসংখ্যা code, vpiHandle ref)
{
      কাঠামো __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      প্রতিজ্ঞা(rfp);

      যদি (code == vpi_সূচক_2) {
	    ফেরত rfp->is_netarray ? rfp->id.সূচক->vpi_iterate(code) : ফাঁকা;
      }

      ফেরত 0;
}

শূন্য __vpiSignal::make_bits()
{
      প্রতিজ্ঞা(bits == ফাঁকা);
      bits = নতুন কাঠামো __vpiBit[width() + 1];

	// Make word[-1] point to the parent.
      bits->parent = এই;
	// Now point to word[0].
      bits += 1;

	// Update each bit to point to the base
      কাঠামো __vpiBit*base = bits;
      জন্য (unsigned idx = 0; idx < width(); idx += 1) {
	    base[idx].bit0 = base;
	    পূর্ণসংখ্যা real_idx;
	    যদি (msb.get_মান() >= lsb.get_মান()) {
		  real_idx = idx + lsb.get_মান();
	    } অন্যথা {
		  real_idx = lsb.get_মান() - idx;
	    }
	    base[idx].সূচক = নতুন __vpiDecConst(real_idx);
      }
}

vpiHandle __vpiSignal::get_সূচক(পূর্ণসংখ্যা idx)
{
	/* Check to see if the index is in range. */
      যদি (msb.get_মান() >= lsb.get_মান()) {
	    যদি ((idx > msb.get_মান()) || (idx < lsb.get_মান())) ফেরত 0;
      } অন্যথা {
	    যদি ((idx < msb.get_মান()) || (idx > lsb.get_মান())) ফেরত 0;
      }

	/* Normalize the index */
      unsigned norm_idx;
      যদি (msb.get_মান() >= lsb.get_মান()) {
	    norm_idx = idx - lsb.get_মান();
      } অন্যথা {
	    norm_idx = lsb.get_মান() - idx;
      }

      যদি (bits == ফাঁকা) make_bits();

      ফেরত &(bits[norm_idx].as_bit);
}

শূন্য __vpiSignal::get_bit_মান(ধ্রুবক কাঠামো __vpiBit*bit, p_vpi_মান vp)
{
      unsigned সূচক = bit->get_norm_সূচক();

      vvp_signal_মান*vsig = dynamic_cast<vvp_signal_মান*>(গ্রন্থি->fil);
      প্রতিজ্ঞা(vsig);

      যদি (vp->format == vpiObjTypeVal) {
	    vp->format = vpiIntVal;
      }

      নির্বাচন (vp->format) {
	ক্ষেত্র vpiBinStrVal:
            format_vpiBinStrVal(vsig, সূচক, 1, vp);
	    বিরতি;

	ক্ষেত্র vpiOctStrVal:
            format_vpiOctStrVal(vsig, সূচক, 1, vp);
	    বিরতি;

	ক্ষেত্র vpiDecStrVal:
            format_vpiDecStrVal(vsig, সূচক, 1, মিথ্যা, vp);
	    বিরতি;

	ক্ষেত্র vpiHexStrVal:
            format_vpiHexStrVal(vsig, সূচক, 1, vp);
	    বিরতি;

	ক্ষেত্র vpiStringVal:
            format_vpiStringVal(vsig, সূচক, 1, vp);
	    বিরতি;

	ক্ষেত্র vpiIntVal:
            format_vpiIntVal(vsig, সূচক, 1, মিথ্যা, vp);
	    বিরতি;

	ক্ষেত্র vpiRealVal:
            format_vpiRealVal(vsig, সূচক, 1, মিথ্যা, vp);
	    বিরতি;

	ক্ষেত্র vpiScalarVal:
            format_vpiScalarVal(vsig, সূচক, vp);
	    বিরতি;

        ক্ষেত্র vpiStrengthVal:
            format_vpiStrengthVal(vsig, সূচক, 1, vp);
            বিরতি;

	ক্ষেত্র vpiVectorVal:
            format_vpiVectorVal(vsig, সূচক, 1, vp);
	    বিরতি;

	পূর্বনির্ধারিত:
	    fprintf(stderr, "internal error: get_value() "
		    "type %d is not implemented for bit "
		    "select %s[%d] in scope %s.\n",
		    (পূর্ণসংখ্যা)vp->format, vpi_get_str(vpiName),
		    bit->get_সূচক(),
	            vpip_scope(এই)->scope_name());
	    প্রতিজ্ঞা(0);
      };
}

vpiHandle __vpiSignal::put_bit_মান(কাঠামো __vpiBit*bit, p_vpi_মান vp, পূর্ণসংখ্যা flags)
{
      unsigned সূচক = bit->get_norm_সূচক();
      vvp_net_ptr_t dest(গ্রন্থি, 0);
      vvp_vector4_t val = vec4_from_vpi_মান(vp, 1);

      যদি ((flags == vpiForceFlag) || (flags == vpiReleaseFlag)) {
	    fprintf(stderr, "Sorry: vpi_put_value() for %s does not "
	                    "currently support force/release.\n",
	                    bit->as_bit.vpi_get_str(vpiFullName));
	    ফেরত ফাঁকা;
      }

      যদি ((get_type_code() == vpiNet) &&
          !dynamic_cast<vvp_island_port*>(গ্রন্থি->fun)) {
	    গ্রন্থি->send_vec4_pv(val, সূচক, width(),
	                            vthread_get_wt_context());
      } অন্যথা {
	    vvp_send_vec4_pv(dest, val, সূচক, width(),
	                     vthread_get_wt_context());
      }

	// This is not a scheduled event so there is no event to return
      ফেরত ফাঁকা;
}

স্থির vpiHandle signal_সূচক(পূর্ণসংখ্যা idx, vpiHandle ref)
{
      কাঠামো __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      প্রতিজ্ঞা(rfp);

	/* We can only get the bit for a net or reg. */
      PLI_INT32 type = vpi_get(vpiType, ref);
      যদি ((type != vpiNet) && (type != vpiReg)) ফেরত 0;

      ফেরত rfp->get_সূচক(idx);
}

unsigned __vpiSignal::width(শূন্য) ধ্রুবক
{
      unsigned wid = (msb.get_মান() >= lsb.get_মান())
	    ? (msb.get_মান() - lsb.get_মান() + 1)
	    : (lsb.get_মান() - msb.get_মান() + 1);

      ফেরত wid;
}

/*
 * The get_value method reads the values of the functors and returns
 * the vector to the caller. This causes no side-effect, and reads the
 * variables like a %load would.
 */
স্থির শূন্য signal_get_মান(vpiHandle ref, s_vpi_মান*vp)
{
      কাঠামো __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      প্রতিজ্ঞা(rfp);

      unsigned wid = rfp->width();

      vvp_signal_মান*vsig = dynamic_cast<vvp_signal_মান*>(rfp->গ্রন্থি->fil);
      প্রতিজ্ঞা(vsig);

      নির্বাচন (vp->format) {

	  ক্ষেত্র vpiIntVal:
	    format_vpiIntVal(vsig, 0, wid, rfp->signed_flag, vp);
	    বিরতি;

	  ক্ষেত্র vpiScalarVal:
	    format_vpiScalarVal(vsig, 0, vp);
	    বিরতি;

	  ক্ষেত্র vpiStrengthVal:
	    format_vpiStrengthVal(vsig, 0, wid, vp);
	    বিরতি;

	  ক্ষেত্র vpiBinStrVal:
	    format_vpiBinStrVal(vsig, 0, wid, vp);
	    বিরতি;

	  ক্ষেত্র vpiHexStrVal:
	    format_vpiHexStrVal(vsig, 0, wid, vp);
	    বিরতি;

	  ক্ষেত্র vpiOctStrVal:
	    format_vpiOctStrVal(vsig, 0, wid, vp);
	    বিরতি;

	  ক্ষেত্র vpiDecStrVal:
	    format_vpiDecStrVal(vsig, 0, wid, rfp->signed_flag, vp);
	    বিরতি;

	  ক্ষেত্র vpiStringVal:
	    format_vpiStringVal(vsig, 0, wid, vp);
	    বিরতি;

	  ক্ষেত্র vpiVectorVal:
	    format_vpiVectorVal(vsig, 0, wid, vp);
	    বিরতি;

	  ক্ষেত্র vpiRealVal:
	    format_vpiRealVal(vsig, 0, wid, rfp->signed_flag, vp);
	    বিরতি;

	  ক্ষেত্র vpiObjTypeVal:
	    যদি (wid == 1) {
		  vp->format = vpiScalarVal;
		  format_vpiScalarVal(vsig, 0, vp);
	    } অন্যথা {
		  vp->format = vpiVectorVal;
		  format_vpiVectorVal(vsig, 0, wid, vp);
	    }
	    বিরতি;

	  পূর্বনির্ধারিত:
	    fprintf(stderr, "vvp internal error: get_value: "
		    "value type %d not implemented."
		    " Signal is %s in scope %s\n",
		    (পূর্ণসংখ্যা)vp->format, vpi_get_str(vpiName, ref),
		    vpip_scope(rfp)->scope_name());
	    প্রতিজ্ঞা(0);
      }
}

/*
 * The put_value method writes the value into the vector, and returns
 * the affected ref. This operation works much like the %set or
 * %assign instructions and causes all the side-effects that the
 * equivalent instruction would cause.
 */

স্থির vvp_vector4_t from_stringval(ধ্রুবক অক্ষর*str, unsigned wid)
{
      unsigned idx;
      ধ্রুবক অক্ষর*cp;

      cp = str + strlen(str);
      idx = 0;

      vvp_vector4_t val(wid, BIT4_0);

      যখন ((idx < wid) && (cp > str)) {
	    unsigned byte = *--cp;
	    পূর্ণসংখ্যা bit;

	    জন্য (bit = 0 ;  (bit < 8) && (idx < wid) ;  bit += 1) {
		  যদি (byte & 1)
			val.set_bit(idx, BIT4_1);

		  byte >>= 1;
		  idx += 1;
	    }
      }

      ফেরত val;
}

স্থির vpiHandle signal_put_মান(vpiHandle ref, s_vpi_মান*vp, পূর্ণসংখ্যা flags)
{
      unsigned wid;
      কাঠামো __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      প্রতিজ্ঞা(rfp);
      vvp_net_ptr_t dest(rfp->গ্রন্থি, 0);

      যৌক্তিক net_flag = ref->get_type_code()==vpiNet;

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value. Note that
	   if this net is attached to an island, we need to rerun
	   the calculations immediately so we can return the
	   released value. */
      যদি (flags == vpiReleaseFlag) {
	    প্রতিজ্ঞা(rfp->গ্রন্থি->fil);
	    rfp->গ্রন্থি->fil->force_unlink();
	    rfp->গ্রন্থি->fil->release(dest, net_flag);
	    rfp->গ্রন্থি->fun->force_flag(সত্য);
	    signal_get_মান(ref, vp);
	    ফেরত ref;
      }

	/* Make a vvp_vector4_t vector to receive the translated value
	   that we are going to poke. This will get populated
	   differently depending on the format. */
      wid = (rfp->msb.get_মান() >= rfp->lsb.get_মান())
	    ? (rfp->msb.get_মান() - rfp->lsb.get_মান() + 1)
	    : (rfp->lsb.get_মান() - rfp->msb.get_মান() + 1);

      vvp_vector4_t val = vec4_from_vpi_মান(vp, wid);

      যদি (flags == vpiForceFlag) {
	    vvp_vector2_t mask (vvp_vector2_t::FILL1, wid);
	    rfp->গ্রন্থি->force_vec4(val, mask);
      } অন্যথা যদি (net_flag && !dynamic_cast<vvp_island_port*>(rfp->গ্রন্থি->fun)) {
	    rfp->গ্রন্থি->send_vec4(val, vthread_get_wt_context());
      } অন্যথা {
	    vvp_send_vec4(dest, val, vthread_get_wt_context());
      }
      ফেরত ref;
}

vvp_vector4_t vec4_from_vpi_মান(s_vpi_মান*vp, unsigned wid)
{
      vvp_vector4_t val (wid, BIT4_0);

      নির্বাচন (vp->format) {

	  ক্ষেত্র vpiIntVal: {
		long vpi_val = vp->value.integer;
		জন্য (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		      vvp_bit4_t bit = (vpi_val & 1) ? BIT4_1 : BIT4_0;
		      val.set_bit(idx, bit);
		      vpi_val >>= 1;
		}
		বিরতি;
	  }

	  ক্ষেত্র vpiVectorVal:
	    জন্য (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		  unsigned long aval = vp->value.vector[idx/32].aval;
		  unsigned long bval = vp->value.vector[idx/32].bval;
		  aval >>= idx%32;
		  bval >>= idx%32;
		  পূর্ণসংখ্যা bitmask = (aval&1) | ((bval<<1)&2);
		  vvp_bit4_t bit = scalar_to_bit4(bitmask);
		  val.set_bit(idx, bit);
	    }
	    বিরতি;
	  ক্ষেত্র vpiBinStrVal:
	    vpip_bin_str_to_vec4(val, vp->value.str);
	    বিরতি;
	  ক্ষেত্র vpiOctStrVal:
	    vpip_oct_str_to_vec4(val, vp->value.str);
	    বিরতি;
	  ক্ষেত্র vpiDecStrVal:
	    vpip_dec_str_to_vec4(val, vp->value.str);
	    বিরতি;
	  ক্ষেত্র vpiHexStrVal:
	    vpip_hex_str_to_vec4(val, vp->value.str);
	    বিরতি;
	  ক্ষেত্র vpiScalarVal:
	    val.set_bit(0, scalar_to_bit4(vp->value.scalar));
	    বিরতি;
	  ক্ষেত্র vpiStringVal:
	    val = from_stringval(vp->value.str, wid);
	    বিরতি;
	  ক্ষেত্র vpiRealVal:
	    val = vvp_vector4_t(wid, vp->value.real);
	    বিরতি;

	  পূর্বনির্ধারিত:
	    fprintf(stderr, "vvp internal error: put_value: "
		    "value type %d not implemented here.\n",
		    (পূর্ণসংখ্যা)vp->format);
	    প্রতিজ্ঞা(0);

      }

      ফেরত val;
}

পূর্ণসংখ্যা __vpiSignal::vpi_get(পূর্ণসংখ্যা code)
{ ফেরত signal_get(code, এই); }

int64_t __vpiSignal::vpi_get64(পূর্ণসংখ্যা code)
{ ফেরত signal_get64(code, এই); }

অক্ষর* __vpiSignal::vpi_get_str(পূর্ণসংখ্যা code)
{ ফেরত signal_get_str(code, এই); }

শূন্য __vpiSignal::vpi_get_মান(p_vpi_মান val)
{ signal_get_মান(এই, val); }

vpiHandle __vpiSignal::vpi_put_মান(p_vpi_মান val, পূর্ণসংখ্যা flags)
{ ফেরত signal_put_মান(এই, val, flags); }

vpiHandle __vpiSignal::vpi_handle(পূর্ণসংখ্যা code)
{ ফেরত signal_get_handle(code, এই); }

vpiHandle __vpiSignal::vpi_iterate(পূর্ণসংখ্যা code)
{ ফেরত signal_iterate(code, এই); }

vpiHandle __vpiSignal::vpi_সূচক(পূর্ণসংখ্যা idx)
{ ফেরত signal_সূচক(idx, এই); }

কাঠামো signal_reg : সর্বজনীন __vpiSignal {
      অন্তঃরেখ signal_reg() { }
      পূর্ণসংখ্যা get_type_code(শূন্য) ধ্রুবক override { ফেরত vpiReg; }
};

কাঠামো signal_integer : সর্বজনীন __vpiSignal {
      অন্তঃরেখ signal_integer() { }
      পূর্ণসংখ্যা get_type_code(শূন্য) ধ্রুবক override { ফেরত vpiIntegerVar; }
};

কাঠামো signal_net : সর্বজনীন __vpiSignal {
      অন্তঃরেখ signal_net() { }
      পূর্ণসংখ্যা get_type_code(শূন্য) ধ্রুবক override { ফেরত vpiNet; }
};

কাঠামো signal_byte : সর্বজনীন __vpiSignal {
      অন্তঃরেখ signal_byte() { }
      পূর্ণসংখ্যা get_type_code(শূন্য) ধ্রুবক override { ফেরত vpiByteVar; }
};

কাঠামো signal_bitvar : সর্বজনীন __vpiSignal {
      অন্তঃরেখ signal_bitvar() { }
      পূর্ণসংখ্যা get_type_code(শূন্য) ধ্রুবক override { ফেরত vpiBitVar; }
};

কাঠামো signal_shortint : সর্বজনীন __vpiSignal {
      অন্তঃরেখ signal_shortint() { }
      পূর্ণসংখ্যা get_type_code(শূন্য) ধ্রুবক override { ফেরত vpiShortIntVar; }
};

কাঠামো signal_int : সর্বজনীন __vpiSignal {
      অন্তঃরেখ signal_int() { }
      পূর্ণসংখ্যা get_type_code(শূন্য) ধ্রুবক override { ফেরত vpiIntVar; }
};

কাঠামো signal_longint : সর্বজনীন __vpiSignal {
      অন্তঃরেখ signal_longint() { }
      পূর্ণসংখ্যা get_type_code(শূন্য) ধ্রুবক override { ফেরত vpiLongIntVar; }
};


/*
 * Construct a vpiIntegerVar object. Indicate the type using a flag
 * to minimize the code modifications. Icarus implements integers
 * as 'reg signed [31:0]'.
 */
vpiHandle vpip_make_int4(ধ্রুবক অক্ষর*name, পূর্ণসংখ্যা msb, পূর্ণসংখ্যা lsb, vvp_net_t*vec)
{
      __vpiSignal*obj = নতুন signal_integer;
      ফেরত fill_in_var4(obj, name, msb, lsb, সত্য, vec);
}

/*
 * Construct the two-state SystemVerilog variables.
 */
vpiHandle vpip_make_int2(ধ্রুবক অক্ষর*name, পূর্ণসংখ্যা msb, পূর্ণসংখ্যা lsb, যৌক্তিক signed_flag,
                         vvp_net_t*vec)
{
      __vpiSignal*obj;

	// All unsigned 2-state variables are a vpiBitVar. All 2-state
	// variables with a non-zero lsb are also a vpiBitVar.
      যদি ((! signed_flag) || (lsb != 0) ) {
	    obj = নতুন signal_bitvar;
      } অন্যথা {
	      // These could also be bit declarations with matching
	      // information, but for now they get the apparent type.
	    নির্বাচন (msb) {
		ক্ষেত্র 7:
		  obj = নতুন signal_byte;
		  বিরতি;
		ক্ষেত্র 15:
		  obj = নতুন signal_shortint;
		  বিরতি;
		ক্ষেত্র 31:
		  obj = নতুন signal_int;
		  বিরতি;
		ক্ষেত্র 63:
		  obj = নতুন signal_longint;
		  বিরতি;
		পূর্বনির্ধারিত:
		    // Every other type of bit vector is a vpiBitVar with
		    // array dimensions.
		  obj = নতুন signal_bitvar;
		  বিরতি;
	    }
      }

      ফেরত fill_in_var4(obj, name, msb, lsb, signed_flag, vec);
}

/*
 * Construct a vpiReg/vpiLogicVar object. It's like a net, except for the type.
 */
vpiHandle vpip_make_var4(ধ্রুবক অক্ষর*name, পূর্ণসংখ্যা msb, পূর্ণসংখ্যা lsb,
			যৌক্তিক signed_flag, vvp_net_t*vec)
{
      __vpiSignal*obj = নতুন signal_reg;
      ফেরত fill_in_var4(obj, name, msb, lsb, signed_flag, vec);
}

#ifdef CHECK_WITH_VALGRIND
স্থির কাঠামো vpiSignal_plug **signal_pool = 0;
স্থির unsigned signal_pool_গুনতি = 0;
স্থির unsigned long signal_গুনতি = 0;
স্থির unsigned long signal_dels = 0;
#endif

কাঠামো vpiSignal_plug {
      unsigned অক্ষর space[আকার (কাঠামো __vpiSignal)];
#ifdef CHECK_WITH_VALGRIND
      কাঠামো vpiSignal_plug *pool;
#endif
};

শূন্য* __vpiSignal::operator নতুন(size_t siz)
{
#ifdef CHECK_WITH_VALGRIND
      প্রতিজ্ঞা(siz == আকার(কাঠামো vpiSignal_plug) - আকার(কাঠামো vpiSignal_plug *));
#অন্যথা
      প্রতিজ্ঞা(siz == আকার(কাঠামো vpiSignal_plug));
#endif
      স্থির কাঠামো vpiSignal_plug*alloc_array = 0;
      স্থির unsigned alloc_সূচক = 0;
      ধ্রুবক unsigned alloc_গুনতি = 512;

      যদি ((alloc_array == 0) || (alloc_সূচক == alloc_গুনতি)) {
	    alloc_array = static_cast<কাঠামো vpiSignal_plug*>
	                  (calloc(alloc_গুনতি, আকার(কাঠামো vpiSignal_plug)));
	    alloc_সূচক = 0;
#ifdef CHECK_WITH_VALGRIND
	    VALGRIND_MAKE_MEM_NOACCESS(alloc_array, alloc_গুনতি *
	                                            আকার(কাঠামো vpiSignal_plug));
	    VALGRIND_CREATE_MEMPOOL(alloc_array, 0, 1);
	    signal_pool_গুনতি += 1;
	    signal_pool = static_cast<vpiSignal_plug **>
	                  (realloc(signal_pool,
	                   signal_pool_গুনতি*আকার(vpiSignal_plug **)));
	    signal_pool[signal_pool_গুনতি-1] = alloc_array;
#endif
      }

      কাঠামো vpiSignal_plug*cur = alloc_array + alloc_সূচক;
#ifdef CHECK_WITH_VALGRIND
      VALGRIND_MEMPOOL_ALLOC(alloc_array, cur, আকার(কাঠামো vpiSignal_plug));
      cur->pool = alloc_array;
      signal_গুনতি += 1;
#endif
      alloc_সূচক += 1;
      ফেরত cur;
}

শূন্য __vpiSignal::operator delete(শূন্য*)
{
      প্রতিজ্ঞা(0);
}

#ifdef CHECK_WITH_VALGRIND
শূন্য signal_delete(vpiHandle item)
{
      কাঠামো __vpiSignal *obj = static_cast<__vpiSignal *> (item);
      প্রতিজ্ঞা(obj->গ্রন্থি->fil);
      obj->গ্রন্থি->fil->clear_all_callbacks();
      vvp_net_delete(obj->গ্রন্থি);
      যদি (obj->bits) {
	    জন্য (unsigned idx=0; idx<obj->width(); idx+=1) {
		  delete obj->bits[idx].সূচক;
	    }
	    obj->bits -= 1;
	    delete [] obj->bits;
      }
      signal_dels += 1;
      VALGRIND_MEMPOOL_FREE(reinterpret_cast<vpiSignal_plug *>(obj)->pool, obj);
}

শূন্য signal_pool_delete()
{
      যদি (RUNNING_ON_VALGRIND && (signal_গুনতি != signal_dels)) {
	    fflush(ফাঁকা);
	    VALGRIND_PRINTF("Error: vvp missed deleting %ld of %lu signal(s).",
	                    (long) signal_গুনতি - signal_dels, signal_গুনতি);
      }

      জন্য (unsigned idx = 0; idx < signal_pool_গুনতি; idx += 1) {
	    VALGRIND_DESTROY_MEMPOOL(signal_pool[idx]);
	    মুক্ত(signal_pool[idx]);
      }

      মুক্ত(signal_pool);
      signal_pool = 0;
      signal_pool_গুনতি = 0;
}
#endif

/*
 * Construct a vpiNet object. Give the object specified dimensions,
 * and point to the specified functor for the lsb.
 *
 * The name is the PLI name for the object. If it is an array it is
 * <name>[<index>].
 */
স্থির vpiHandle fill_in_net4(কাঠামো __vpiSignal*obj, __vpiScope*scope,
			      ধ্রুবক অক্ষর*name, পূর্ণসংখ্যা msb, পূর্ণসংখ্যা lsb,
			      যৌক্তিক signed_flag, vvp_net_t*গ্রন্থি)
{
      obj->id.name = name? vpip_name_string(name) : ফাঁকা;
      obj->msb = __vpiDecConst(msb);
      obj->lsb = __vpiDecConst(lsb);
      obj->signed_flag = signed_flag? 1 : 0;
      obj->is_netarray = 0;
      obj->গ্রন্থি = গ্রন্থি;

	// Place this object within a scope. If this object is
	// attached to an array, then this value will be replaced with
	// the handle to the parent.
      obj->within.scope = scope;

      গুনতি_vpi_nets += 1;

      ফেরত obj;
}

vpiHandle vpip_make_net4(__vpiScope*scope,
			 ধ্রুবক অক্ষর*name, পূর্ণসংখ্যা msb, পূর্ণসংখ্যা lsb,
			 যৌক্তিক signed_flag, vvp_net_t*গ্রন্থি)
{
      কাঠামো __vpiSignal*obj = নতুন signal_net;
      ফেরত fill_in_net4(obj, scope, name, msb, lsb, signed_flag, গ্রন্থি);
}

স্থির পূর্ণসংখ্যা PV_get_base(কাঠামো __vpiPV*rfp)
{
	/* We return from the symbol base if it is defined. */
      যদি (rfp->sbase != 0) {
	    s_vpi_মান val;
	      /* Check to see if the value is defined. */
	    val.format = vpiVectorVal;
	    vpi_get_মান(rfp->sbase, &val);
	    পূর্ণসংখ্যা শব্দগুলি = (vpi_get(vpi_আকার, rfp->sbase)-1)/32 + 1;
	    জন্য(পূর্ণসংখ্যা idx = 0; idx < শব্দগুলি; idx += 1) {
		    /* Return INT_MIN to indicate an X base. */
		  যদি (val.value.vector[idx].bval != 0) ফেরত INT_MIN;
	    }
	      /* The value is defined so get and return it. */
	    val.format = vpiIntVal;
	    vpi_get_মান(rfp->sbase, &val);
	    ফেরত val.value.integer;
      }

	/* If the width is zero then tbase is the constant. */
      ফেরত rfp->tbase;
}

স্থির পূর্ণসংখ্যা PV_get(পূর্ণসংখ্যা code, vpiHandle ref)
{
      কাঠামো __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      প্রতিজ্ঞা(rfp);

      পূর্ণসংখ্যা rval = 0;
      নির্বাচন (code) {
	ক্ষেত্র vpi_সারি_No:
	    ফেরত 0;  // Not implemented for now!

	ক্ষেত্র vpiSigned:
	    ফেরত 0;  // A part/bit select is always unsigned!

	ক্ষেত্র vpi_আকার:
	    ফেরত rfp->width;

	  /* This is like the &A<> in array.cc. */
	ক্ষেত্র vpiConstantSelect:
	    ফেরত rfp->sbase == 0;

	ক্ষেত্র vpi_বাম_Range:
            rval += rfp->width - 1;
	    // fallthrough
	ক্ষেত্র vpi_ডান_Range: {
	    পূর্ণসংখ্যা বাম = vpi_get(vpi_বাম_Range, rfp->parent);
	    পূর্ণসংখ্যা ডান = vpi_get(vpi_ডান_Range, rfp->parent);
	    rval += PV_get_base(rfp);
	    ফেরত বাম >= ডান ? ডান + rval : ডান - rval;
	}

        ক্ষেত্র vpiAutomatic:
            ফেরত vpi_get(vpiAutomatic, rfp->parent);

#যদি defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
        ক্ষেত্র _vpiFromThr:
            ফেরত _vpi_at_PV;
#endif

	পূর্বনির্ধারিত:
	    fprintf(stderr, "PV_get: property %d is unknown\n", code);
      }

      ফেরত 0;
}

স্থির অক্ষর* PV_get_str(পূর্ণসংখ্যা code, vpiHandle ref)
{
      কাঠামো __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      প্রতিজ্ঞা(rfp);

      নির্বাচন (code) {
	ক্ষেত্র vpiFile:  // Not implemented for now!
	    ফেরত simple_set_rbuf_str(file_names[0]);

	ক্ষেত্র vpiName:
	ক্ষেত্র vpiFullName: {
	    ধ্রুবক অক্ষর*nm = vpi_get_str(code, rfp->parent);
	    size_t len = 256+strlen(nm);
	    অক্ষর *full = static_cast<অক্ষর *>(malloc(len));
	    snprintf(full, len, "%s[%d:%d]", nm,
	                                     (পূর্ণসংখ্যা)vpi_get(vpi_বাম_Range, ref),
	                                     (পূর্ণসংখ্যা)vpi_get(vpi_ডান_Range, ref));
	    full[len-1] = 0;
	    অক্ষর *res = simple_set_rbuf_str(full);
	    মুক্ত(full);
	    ফেরত res;
	}

	পূর্বনির্ধারিত:
	    fprintf(stderr, "PV_get_str: property %d is unknown.\n", code);
      }

      ফেরত 0;
}

স্থির শূন্য PV_get_মান(vpiHandle ref, p_vpi_মান vp)
{
      কাঠামো __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      প্রতিজ্ঞা(rfp);

      vvp_signal_মান*sig = dynamic_cast<vvp_signal_মান*>(rfp->net->fil);
      প্রতিজ্ঞা(sig);

      নির্বাচন (vp->format) {

	  ক্ষেত্র vpiIntVal:
	    format_vpiIntVal(sig, PV_get_base(rfp), rfp->width, 0, vp);
	    বিরতি;

	  ক্ষেত্র vpiBinStrVal:
	    format_vpiBinStrVal(sig, PV_get_base(rfp), rfp->width, vp);
	    বিরতি;

	  ক্ষেত্র vpiOctStrVal:
	    format_vpiOctStrVal(sig, PV_get_base(rfp), rfp->width, vp);
	    বিরতি;

	  ক্ষেত্র vpiHexStrVal:
	    format_vpiHexStrVal(sig, PV_get_base(rfp), rfp->width, vp);
	    বিরতি;

	  ক্ষেত্র vpiDecStrVal:
	    format_vpiDecStrVal(sig, PV_get_base(rfp), rfp->width, 0, vp);
	    বিরতি;

	  ক্ষেত্র vpiStringVal:
	    format_vpiStringVal(sig, PV_get_base(rfp), rfp->width, vp);
	    বিরতি;

	  ক্ষেত্র vpiScalarVal:
	    format_vpiScalarVal(sig, PV_get_base(rfp), vp);
	    বিরতি;

	  ক্ষেত্র vpiStrengthVal:
	    format_vpiStrengthVal(sig, PV_get_base(rfp), rfp->width, vp);
	    বিরতি;

	  ক্ষেত্র vpiVectorVal:
	    format_vpiVectorVal(sig, PV_get_base(rfp), rfp->width, vp);
	    বিরতি;

	  ক্ষেত্র vpiRealVal:
	    format_vpiRealVal(sig, PV_get_base(rfp), rfp->width, 0, vp);
	    বিরতি;

	  পূর্বনির্ধারিত:
	    fprintf(stderr, "vvp internal error: PV_get_value: "
		    "value type %d not implemented. Signal is %s.\n",
		    (পূর্ণসংখ্যা)vp->format, vpi_get_str(vpiFullName, rfp->parent));
	    প্রতিজ্ঞা(0);
      }
}

স্থির vpiHandle PV_put_মান(vpiHandle ref, p_vpi_মান vp, পূর্ণসংখ্যা flags)
{
      কাঠামো __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      প্রতিজ্ঞা(rfp);
      vvp_signal_মান*sig = dynamic_cast<vvp_signal_মান*>(rfp->net->fil);
      প্রতিজ্ঞা(sig);

      unsigned sig_আকার = sig->মান_আকার();
      unsigned width = rfp->width;
      পূর্ণসংখ্যা base = PV_get_base(rfp);
      যদি (base >= (signed) sig_আকার) ফেরত 0;
      যদি (base + (signed) width < 0) ফেরত 0;

      vvp_vector4_t val;
      যদি (flags != vpiReleaseFlag) {
	    val = vec4_from_vpi_মান(vp, width);
      }

	/*
	 * If the base is less than zero then trim off any unneeded
	 * lower bits.
	 */
      যদি (base < 0) {
	    width += base;
	    যদি (flags != vpiReleaseFlag) {
		  val = val.subvalue(-base, width);
	    }
	    base = 0;
      }

	/*
	 * If the value is wider than the signal then trim off any
	 * unneeded upper bits.
	 */
      যদি (base+width > sig_আকার) {
	    width = sig_আকার - base;
	    যদি (flags != vpiReleaseFlag) {
		  val = val.subvalue(0, width);
	    }
      }

      প্রতিজ্ঞা(rfp->parent);
      যৌক্তিক net_flag = rfp->parent->get_type_code()==vpiNet;
      যৌক্তিক full_sig = base == 0 && width == sig_আকার;

      vvp_net_ptr_t dest(rfp->net, 0);

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value.  Note that
	   if this net is attached to an island, we need to rerun
	   the calculations immediately so we can return the
	   released value.*/
      যদি (flags == vpiReleaseFlag) {
	    প্রতিজ্ঞা(rfp->net->fil);
	      // XXXX Can't really do this if this is a partial release?
	    rfp->net->fil->force_unlink();
	    যদি (full_sig) {
		  rfp->net->fil->release(dest, net_flag);
	    } অন্যথা {
		  rfp->net->fil->release_pv(dest, base, width, net_flag);
	    }
	    rfp->net->fun->force_flag(সত্য);
	    PV_get_মান(ref, vp);
	    ফেরত ref;
      }

      যদি (flags == vpiForceFlag) {
	    যদি (full_sig) {
		  vvp_vector2_t mask (vvp_vector2_t::FILL1, sig_আকার);
		  rfp->net->force_vec4(val, mask);
	    } অন্যথা {
		  vvp_vector2_t mask (vvp_vector2_t::FILL0, sig_আকার);
		  জন্য (unsigned idx = 0 ; idx < width ; idx += 1)
		        mask.set_bit(base+idx, 1);

		  vvp_vector4_t tmp (sig_আকার, BIT4_Z);

		    // vvp_net_t::force_vec4 propagates all the bits of the
		    // forced vector value, regardless of the mask. This
		    // ensures the unforced bits retain their current value.
		  sig->vec4_মান(tmp);

		  tmp.set_vec(base, val);
		  rfp->net->force_vec4(tmp, mask);
	    }
      } অন্যথা যদি (net_flag && !dynamic_cast<vvp_island_port*>(rfp->net->fun)) {
	    যদি (full_sig) {
		  rfp->net->send_vec4(val, vthread_get_wt_context());
	    } অন্যথা {
		  rfp->net->send_vec4_pv(val, base, sig_আকার,
		                         vthread_get_wt_context());
	    }
      } অন্যথা {
	    যদি (full_sig) {
		  vvp_send_vec4(dest, val, vthread_get_wt_context());
	    } অন্যথা {
		  vvp_send_vec4_pv(dest, val, base, sig_আকার,
	                           vthread_get_wt_context());
	    }
      }
      ফেরত 0;
}

স্থির vpiHandle PV_get_handle(পূর্ণসংখ্যা code, vpiHandle ref)
{
      কাঠামো __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      প্রতিজ্ঞা(rfp);

      নির্বাচন (code) {
	  ক্ষেত্র vpiParent:
	    ফেরত rfp->parent;

	  ক্ষেত্র vpiScope:
	    ফেরত vpi_handle(vpiScope, rfp->parent);

	  ক্ষেত্র vpiModule:
	    ফেরত vpi_handle(vpiModule, rfp->parent);
      }

      ফেরত 0;
}

অন্তঃরেখ __vpiPV::__vpiPV()
{ }

পূর্ণসংখ্যা __vpiPV::get_type_code(শূন্য) ধ্রুবক
{ ফেরত vpiPartSelect; }

পূর্ণসংখ্যা __vpiPV::vpi_get(পূর্ণসংখ্যা code)
{ ফেরত PV_get(code, এই); }

অক্ষর* __vpiPV::vpi_get_str(পূর্ণসংখ্যা code)
{ ফেরত PV_get_str(code, এই); }

শূন্য __vpiPV::vpi_get_মান(p_vpi_মান val)
{ PV_get_মান(এই, val); }

vpiHandle __vpiPV::vpi_put_মান(p_vpi_মান val, পূর্ণসংখ্যা flags)
{ ফেরত PV_put_মান(এই, val, flags); }

vpiHandle __vpiPV::vpi_handle(পূর্ণসংখ্যা code)
{ ফেরত PV_get_handle(code, এই); }

vpiHandle vpip_make_PV(অক্ষর*var, পূর্ণসংখ্যা base, পূর্ণসংখ্যা width)
{
      কাঠামো __vpiPV*obj = নতুন __vpiPV;
      compile_vpi_lookup(&obj->parent, strdup(var));
      obj->sbase = 0;
      obj->tbase = base;
      obj->width = (unsigned) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      ফেরত obj;
}

vpiHandle vpip_make_PV(অক্ষর*var, অক্ষর*প্রতীক, পূর্ণসংখ্যা width)
{
      কাঠামো __vpiPV*obj = নতুন __vpiPV;
      compile_vpi_lookup(&obj->parent, strdup(var));
      compile_vpi_lookup(&obj->sbase, প্রতীক);
      obj->tbase = 0;
      obj->width = (unsigned) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      ফেরত obj;
}

vpiHandle vpip_make_PV(অক্ষর*var, vpiHandle handle, পূর্ণসংখ্যা width)
{
      কাঠামো __vpiPV*obj = নতুন __vpiPV;
      compile_vpi_lookup(&obj->parent, strdup(var));
      obj->sbase = handle;
      obj->tbase = 0;
      obj->width = (unsigned) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      ফেরত obj;
}

#ifdef CHECK_WITH_VALGRIND
শূন্য PV_delete(vpiHandle item)
{
      কাঠামো __vpiPV *obj = dynamic_cast<__vpiPV*>(item);
      যদি (obj->sbase) {
	    নির্বাচন (obj->sbase->get_type_code()) {
		ক্ষেত্র vpiMemoryWord:
		  যদি (vpi_get(_vpiFromThr, obj->sbase) == _vpi_at_A) {
			A_delete(obj->sbase);
		  }
		  বিরতি;
		ক্ষেত্র vpiPartSelect:
		  প্রতিজ্ঞা(vpi_get(_vpiFromThr, obj->sbase) == _vpi_at_PV);
		  PV_delete(obj->sbase);
		  বিরতি;
	    }
      }
      প্রতিজ্ঞা(obj->net->fil);
      obj->net->fil->clear_all_callbacks();
      delete obj;
}
#endif
