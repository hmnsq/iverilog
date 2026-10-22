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
#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#جيڪڏهن_پورو
# include  <cmath>
# include  <iostream>
# include  <cstdio>
# include  <cstdlib>
# include  <climits>
# include  <cstring>
# include  <cassert>
#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
# include  <valgrind/memcheck.h>
#جيڪڏهن_پورو
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
خارجي ثابت حرف hex_digits[256];
خارجي ثابت حرف oct_digits[64];

/*
 * The string values need a result buf to hold the results. This
 * buffer can be reused for that purpose. Whenever I have a need, the
 * need_result_buf function makes sure that need can be met.
 */
فراغ *need_نتيجو_buf(بنانشان cnt, vpi_rbuf_t type)
{
      ساكن فراغ*نتيجو_buf[2] = {0, 0};
      ساكن size_t نتيجو_buf_سائيز[2] = {0, 0};

      إذا (type == RBUF_DEL) {
	    حرر(نتيجو_buf[RBUF_VAL]);
	    نتيجو_buf[RBUF_VAL] = 0;
	    نتيجو_buf_سائيز[RBUF_VAL] = 0;

	    حرر(نتيجو_buf[RBUF_STR]);
	    نتيجو_buf[RBUF_STR] = 0;
	    نتيجو_buf_سائيز[RBUF_STR] = 0;

	    أرجع 0;
      }

      cnt = (cnt + 0x0fff) & ~0x0fff;

      إذا (نتيجو_buf_سائيز[type] == 0) {
	    نتيجو_buf[type] = يادگيري_ڏيو(cnt);
	    نتيجو_buf_سائيز[type] = cnt;
      } وإلا إذا (نتيجو_buf_سائيز[type] < cnt) {
	    نتيجو_buf[type] = realloc(نتيجو_buf[type], cnt);
	    نتيجو_buf_سائيز[type] = cnt;
      }

      أرجع نتيجو_buf[type];
}

حرف *simple_سيٽ_rbuf_str(ثابت حرف *s1)
{
      حرف *res = static_cast<حرف *>(need_نتيجو_buf(لڙي_ڊيگهه(s1)+1, RBUF_STR));
      إذا (res) strcpy(res,s1);
      أرجع res;
}

حرف *generic_get_str(صحيح code, vpiHandle ref, ثابت حرف *name, ثابت حرف *اشاريو)
{
      size_t len = لڙي_ڊيگهه(name) + 1;  /* include space for null termination */
      حرف *bn = عدم;
      منطقي is_bn_a_pkg = خطأ;
      إذا (code == vpi_ڀريل_Name) {
	    bn = strdup(vpi_get_str(code,ref));
	    size_t bn_len = لڙي_ڊيگهه(bn);
	    is_bn_a_pkg = (bn_len >= 3) && (bn[bn_len-1] == ':') && (bn[bn_len-2] == ':');
	    len += bn_len;
	    إذا (! is_bn_a_pkg) len += 1; // include space for "." separator
      }
      إذا (اشاريو != عدم) len += لڙي_ڊيگهه(اشاريو) + 2;  /* include space for brackets */

      حرف *res = static_cast<حرف *>(need_نتيجو_buf(len, RBUF_STR));
      إذا (!res) {
	    حرر(bn);
	    أرجع عدم;
      }
      *res=0;  /* start with nothing */

	/* if this works, I can make it more efficient later */
      إذا (bn != عدم) {
	    strcat(res, bn);
	      // A package already has the "::" separator in the name
	    إذا (! is_bn_a_pkg) strcat(res, ".");
	    حرر(bn);
      }
      strcat(res, name);
      إذا (اشاريو != عدم) {
	    strcat(res, "[");
	    strcat(res, اشاريو);
	    strcat(res, "]");
      }
      أرجع res;
}

ساكن vpiHandle fill_اندريون_net4(بنية __vpiSignal*obj, __vpiScope*scope,
                              ثابت حرف*name, صحيح msb, صحيح lsb,
                              منطقي signed_flag, vvp_net_t*ڳنڍ);

ساكن vpiHandle fill_اندريون_var4(بنية __vpiSignal*obj,
                              ثابت حرف*name, صحيح msb, صحيح lsb,
                              منطقي signed_flag, vvp_net_t*ڳنڍ)
{
	// Variable declarations are always resolved immediately,
	// so we can assume they belong in the current scope.
      أرجع fill_اندريون_net4(obj, vpip_peek_current_scope(),
			  name, msb, lsb, signed_flag, ڳنڍ);
}

/*
 * The standard formatting/conversion routines.
 * They work with full or partial signals.
 */

ساكن فراغ format_vpiBinStrVal(ثابت vvp_signal_قدر*sig, صحيح بنياد, بنانشان wid,
                                s_vpi_قدر*vp)
{
      حرف *rbuf = static_cast<حرف *>(need_نتيجو_buf(wid+1, RBUF_VAL));
      ڊگهو پڇاڙي = بنياد + (نشانوارو)wid;
      ڊگهو وٿي_2 = پڇاڙي - 1;
      ڊگهو ssize = (نشانوارو)sig->قدر_سائيز();

      لكل (ڊگهو idx = بنياد ;  idx < پڇاڙي ;  idx += 1) {
	    إذا (idx < 0 || idx >= ssize) {
                  rbuf[وٿي_2-idx] = 'x';
	    } وإلا {
                  rbuf[وٿي_2-idx] = vvp_bit4_منزل_ascii(sig->value(idx));
	    }
      }
      rbuf[wid] = 0;

      vp->value.str = rbuf;
}

ساكن فراغ format_vpiOctStrVal(ثابت vvp_signal_قدر*sig, صحيح بنياد, بنانشان wid,
                                s_vpi_قدر*vp)
{
      بنانشان dwid = (wid + 2) / 3;
      حرف *rbuf = static_cast<حرف *>(need_نتيجو_buf(dwid+1, RBUF_VAL));
      ڊگهو پڇاڙي = بنياد + (نشانوارو)wid;
      ڊگهو ssize = (نشانوارو)sig->قدر_سائيز();
      بنانشان val = 0;

      rbuf[dwid] = 0;
      لكل (ڊگهو idx = بنياد ;  idx < پڇاڙي ;  idx += 1) {
	    بنانشان ٻٽو_انگ = 0;
	    إذا (idx < 0 || idx >= ssize) {
                  ٻٽو_انگ = 2; // BIT4_X
	    } وإلا {
                  بدل (sig->value(idx)) {
		      حالة BIT4_0:
			ٻٽو_انگ = 0;
			اكسر;
		      حالة BIT4_1:
			ٻٽو_انگ = 1;
			اكسر;
		      حالة BIT4_X:
			ٻٽو_انگ = 2;
			اكسر;
		      حالة BIT4_Z:
			ٻٽو_انگ = 3;
			اكسر;
                  }
	    }
	    val |= ٻٽو_انگ << 2*((idx-بنياد) % 3);

	    إذا ((idx-بنياد) % 3 == 2) {
		dwid -= 1;
		rbuf[dwid] = oct_digits[val];
		val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      بدل (wid % 3) {
	  حالة 1:
	    إذا (val == 2) val = 42;
	    وإلا إذا (val == 3) val = 63;
	    اكسر;
	  حالة 2:
	    إذا (val == 10) val = 42;
	    وإلا إذا (val == 15) val = 63;
	    اكسر;
      }

      إذا (dwid > 0) rbuf[0] = oct_digits[val];

      vp->value.str = rbuf;
}

ساكن فراغ format_vpiHexStrVal(ثابت vvp_signal_قدر*sig, صحيح بنياد, بنانشان wid,
                                s_vpi_قدر*vp)
{
      بنانشان dwid = (wid + 3) / 4;
      حرف *rbuf = static_cast<حرف *>(need_نتيجو_buf(dwid+1, RBUF_VAL));
      ڊگهو پڇاڙي = بنياد + (نشانوارو)wid;
      ڊگهو ssize = (نشانوارو)sig->قدر_سائيز();
      بنانشان val = 0;

      rbuf[dwid] = 0;
      لكل (ڊگهو idx = بنياد ;  idx < پڇاڙي ;  idx += 1) {
	    بنانشان ٻٽو_انگ = 0;
	    إذا (idx < 0 || idx >= ssize) {
                  ٻٽو_انگ = 2; // BIT4_X
	    } وإلا {
                  بدل (sig->value(idx)) {
		      حالة BIT4_0:
			ٻٽو_انگ = 0;
			اكسر;
		      حالة BIT4_1:
			ٻٽو_انگ = 1;
			اكسر;
		      حالة BIT4_X:
			ٻٽو_انگ = 2;
			اكسر;
		      حالة BIT4_Z:
			ٻٽو_انگ = 3;
			اكسر;
                  }
	    }
	    val |= ٻٽو_انگ << 2*((idx-بنياد) % 4);

	    إذا ((idx-بنياد) % 4 == 3) {
		dwid -= 1;
		rbuf[dwid] = hex_digits[val];
		val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      بدل (wid % 4) {
	  حالة 1:
	    إذا (val == 2) val = 170;
	    وإلا إذا (val == 3) val = 255;
	    اكسر;
	  حالة 2:
	    إذا (val == 10) val = 170;
	    وإلا إذا (val == 15) val = 255;
	    اكسر;
	  حالة 3:
	    إذا (val == 42) val = 170;
	    وإلا إذا (val == 63) val = 255;
	    اكسر;
      }

      إذا (dwid > 0) rbuf[0] = hex_digits[val];

      vp->value.str = rbuf;
}

ساكن فراغ format_vpiDecStrVal(ثابت vvp_signal_قدر*sig, صحيح بنياد, بنانشان wid,
                                صحيح signed_flag, s_vpi_قدر*vp)
{
      بنانشان hwid = (sig->قدر_سائيز()+2) / 3 + 1;
      حرف *rbuf = static_cast<حرف *>(need_نتيجو_buf(hwid, RBUF_VAL));
      ڊگهو ssize = (نشانوارو)sig->قدر_سائيز();
      ڊگهو پڇاڙي = بنياد + (نشانوارو)wid;

	/* Do we have an end outside of the real signal vector. */
      إذا (بنياد < 0 || پڇاڙي > ssize) {
	    منطقي all_x = سچ;
	    إذا (پڇاڙي > ssize) پڇاڙي = ssize;
	    إذا (بنياد < 0) بنياد = 0;
	    لكل (ڊگهو idx = بنياد ;  idx < پڇاڙي ;  idx += 1) {
		  إذا (sig->value(idx) != BIT4_X) {
			all_x = خطأ;
			اكسر;
		  }
	    }

	    إذا (all_x) {
		  rbuf[0] = 'x';
	    } وإلا {
		  rbuf[0] = 'X';
	    }
	    rbuf[1] = 0;

	    vp->value.str = rbuf;
	    أرجع;
      }

      vvp_vector4_t vec4;
      إذا (بنياد == 0 && پڇاڙي == ssize) {
	    sig->vec4_قدر(vec4);
      } وإلا {
	    vvp_vector4_t tmp;
	    sig->vec4_قدر(tmp);
	    vec4 = tmp.subvalue(بنياد, wid);
      }

      vpip_vec4_منزل_dec_str(vec4, rbuf, hwid, signed_flag);

      vp->value.str = rbuf;
}

ساكن فراغ format_vpiIntVal(ثابت vvp_signal_قدر*sig, صحيح بنياد, بنانشان wid,
                             صحيح signed_flag, s_vpi_قدر*vp)
{
      vvp_vector4_t tmp;
      sig->vec4_قدر(tmp);
      vvp_vector4_t sub = tmp.subvalue(بنياد, wid);

	// Normally, we'd be OK with just using long in the call to
	// vector4_to_value, but some compilers seem to take long as
	// distinct from int32_t AND int64_t. Since the condition is
	// constant, the compiler should eliminate the dead code.
      إذا (حجم(vp->value.integer) == حجم(int32_t)) {
	    int32_t val = 0;
	    vector4_منزل_قدر(sub, val, signed_flag, خطأ);
	    vp->value.integer = val;
      } وإلا {
	    أكد(حجم(vp->value.integer) == حجم(int64_t));
	    int64_t val = 0;
	    vector4_منزل_قدر(sub, val, signed_flag, خطأ);
	    vp->value.integer = val;
      }
}

ساكن فراغ format_vpiRealVal(ثابت vvp_signal_قدر*sig, صحيح بنياد, بنانشان wid,
                              صحيح signed_flag, s_vpi_قدر*vp)
{
      vvp_vector4_t vec4(wid);
      ڊگهو ssize = (نشانوارو)sig->قدر_سائيز();
      ڊگهو پڇاڙي = بنياد + (نشانوارو)wid;
      إذا (پڇاڙي > ssize) پڇاڙي = ssize;

      لكل (ڊگهو idx = (بنياد < 0) ? 0 : بنياد ;  idx < پڇاڙي ;  idx += 1) {
	    vec4.سيٽ_ٻٽو_انگ(idx-بنياد, sig->value(idx));
      }

      vp->value.real = 0.0;
      vector4_منزل_قدر(vec4, vp->value.real, signed_flag);
}

ساكن فراغ format_vpiStringVal(ثابت vvp_signal_قدر*sig, صحيح بنياد, بنانشان wid,
                                s_vpi_قدر*vp)
{
      /* The result will use a character for each 8 bits of the
	 vector. Add one extra character for the highest bits that
	 don't form an 8 bit group. */
      حرف *rbuf = static_cast<حرف *>(need_نتيجو_buf(wid/8 + ((wid&7)!=0) + 1, RBUF_VAL));
      حرف *cp = rbuf;

      حرف tmp = 0;
      لكل (ڊگهو idx = بنياد+(نشانوارو)wid-1; idx >= بنياد; idx -= 1) {
	    tmp <<= 1;

	    إذا (idx >=0 && idx < (نشانوارو)sig->قدر_سائيز() &&
	        sig->value(idx) == BIT4_1) {
		   tmp |= 1;
	    }

	    إذا (((idx-بنياد)&7)==0){
		  /* Skip leading nulls. */
		  إذا (tmp == 0 && cp == rbuf)
			استمر;

		  /* Nulls in the middle get turned into spaces. */
		  *cp++ = tmp ? tmp : ' ';
		  tmp = 0;
	    }
      }
      *cp++ = 0;

      vp->value.str = rbuf;
}

ساكن فراغ format_vpiScalarVal(ثابت vvp_signal_قدر*sig, صحيح بنياد,
                                s_vpi_قدر*vp)
{
      إذا (بنياد >= 0 && بنياد < (نشانوارو)sig->قدر_سائيز()) {
	    بدل (sig->value(بنياد)) {
		حالة BIT4_0:
		  vp->value.scalar = vpi0;
		  اكسر;
		حالة BIT4_1:
		  vp->value.scalar = vpi1;
		  اكسر;
		حالة BIT4_X: {
		  vvp_scalar_t strn = sig->scalar_قدر(بنياد);
		  إذا (strn.strength0() == 1) vp->value.scalar = vpiH;
		  وإلا إذا (strn.strength1() == 1) vp->value.scalar = vpiL;
		  وإلا vp->value.scalar = vpiX;
		  اكسر;
		}
		حالة BIT4_Z:
		  vp->value.scalar = vpiZ;
		  اكسر;
	    }
      } وإلا {
	    vp->value.scalar = vpiX;
      }
}

ساكن فراغ format_vpiStrengthVal(ثابت vvp_signal_قدر*sig, صحيح بنياد,
                                  بنانشان wid, s_vpi_قدر*vp)
{
      ڊگهو پڇاڙي = بنياد + (نشانوارو)wid;
      s_vpi_strengthval*op;

      op = static_cast<s_vpi_strengthval*>
           (need_نتيجو_buf(wid * حجم(s_vpi_strengthval), RBUF_VAL));

      لكل (ڊگهو idx = بنياد ;  idx < پڇاڙي ;  idx += 1) {
	    إذا (idx >=0 && idx < (نشانوارو)sig->قدر_سائيز()) {
		  vvp_scalar_t val = sig->scalar_قدر(idx);

		  /* vvp_scalar_t strengths are 0-7, but the vpi strength
		     is bit0-bit7. This gets the vpi form of the strengths
		     from the vvp_scalar_t strengths. */
		  بنانشان s0 = 1 << val.strength0();
		  بنانشان s1 = 1 << val.strength1();

		  بدل (val.value()) {
		      حالة BIT4_0:
			op[idx-بنياد].logic = vpi0;
			op[idx-بنياد].s0 = s0|s1;
			op[idx-بنياد].s1 = 0;
			اكسر;

		      حالة BIT4_1:
			op[idx-بنياد].logic = vpi1;
			op[idx-بنياد].s0 = 0;
			op[idx-بنياد].s1 = s0|s1;
			اكسر;

		      حالة BIT4_X:
			op[idx-بنياد].logic = vpiX;
			op[idx-بنياد].s0 = s0;
			op[idx-بنياد].s1 = s1;
			اكسر;

		      حالة BIT4_Z:
			op[idx-بنياد].logic = vpiZ;
			op[idx-بنياد].s0 = vpiHiZ;
			op[idx-بنياد].s1 = vpiHiZ;
			اكسر;
		  }
	    } وإلا {
		  op[idx-بنياد].logic = vpiX;
		  op[idx-بنياد].s0 = vpiStrongDrive;
		  op[idx-بنياد].s1 = vpiStrongDrive;
	    }
      }

      vp->value.strength = op;
}

ساكن فراغ format_vpi_ويڪٽر_Val(ثابت vvp_signal_قدر*sig, صحيح بنياد, بنانشان wid,
                                s_vpi_قدر*vp)
{
      ڊگهو پڇاڙي = بنياد + (نشانوارو)wid;
      بنانشان صحيح obit = 0;
      بنانشان hwid = (wid + 31)/32;

      s_vpi_vecval *op = static_cast<p_vpi_vecval>
                         (need_نتيجو_buf(hwid * حجم(s_vpi_vecval), RBUF_VAL));
      vp->value.vector = op;

      op->aval = op->bval = 0;
      لكل (ڊگهو idx = بنياد ;  idx < پڇاڙي ;  idx += 1) {
	    إذا (بنياد >= 0 && بنياد < (نشانوارو)sig->قدر_سائيز()) {
		بدل (sig->value(idx)) {
		حالة BIT4_0:
		  op->aval &= ~(1 << obit);
		  op->bval &= ~(1 << obit);
		  اكسر;
		حالة BIT4_1:
		  op->aval |= (1 << obit);
		  op->bval &= ~(1 << obit);
		  اكسر;
		حالة BIT4_X:
		  op->aval |= (1 << obit);
		  op->bval |= (1 << obit);
		  اكسر;
		حالة BIT4_Z:
		  op->aval &= ~(1 << obit);
		  op->bval |= (1 << obit);
		  اكسر;
		}
	    } وإلا {  /* BIT4_X */
		  op->aval |= (1 << obit);
		  op->bval |= (1 << obit);
	    }

	    obit++;
	    إذا (!(obit % 32)) {
		  op += 1;
		  إذا ((op - vp->value.vector) < (ptrdiff_t)hwid)
			op->aval = op->bval = 0;
		  obit = 0;
	    }
      }
}

/*
 * implement vpi_get for vpiReg objects.
 */
ساكن صحيح signal_get(صحيح code, vpiHandle ref)
{
      بنية __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      أكد(rfp);

      بدل (code) {
	  حالة vpi_سٽ_No:
	    أرجع 0;  // Not implemented for now!

	  حالة vpiSigned:
	    أرجع rfp->signed_flag != 0;

	  حالة vpiArray:
	    أرجع rfp->is_netarray != 0;

	  حالة vpi_اشاريو_2: // This only works while we have a single index.
	    إذا (rfp->is_netarray) {
		  s_vpi_قدر vp;
		  vp.format = vpiIntVal;
		  vpi_get_قدر(rfp->id.اشاريو, &vp);
		  أرجع vp.value.integer;
	    } وإلا {
		  أرجع vpiUndefined;
	    }

	  حالة vpi_سائيز:
	    أرجع rfp->width();

	  حالة vpiNetType:
	    إذا (ref->get_type_ڪوڊ()==vpiNet)
		  أرجع vpiWire;
	    وإلا
		  أرجع vpiUndefined;

	  حالة vpi_کاٻو_Range:
            أرجع rfp->msb.get_قدر();

	  حالة vpi_ساڄو_Range:
            أرجع rfp->lsb.get_قدر();

	  حالة vpiScalar:
	    أرجع (rfp->msb.get_قدر() == 0 && rfp->lsb.get_قدر() == 0);
	  حالة vpi_ويڪٽر:
	    أرجع (rfp->msb.get_قدر() != rfp->lsb.get_قدر());

          حالة vpiAutomatic:
            أرجع vpip_scope(rfp)->is_automatic() ? 1  : 0;

#جيڪڏهن_وصف_ٿيل BR916_STOPGAP_FIX
          حالة _vpiFromThr:
            أرجع _vpiNoThr;
#جيڪڏهن_پورو

	  افتراضي:
	    fprintf(stderr, "VPI error: unknown signal_get property %d.\n",
	            code);
	    أرجع vpiUndefined;
      }
}

/*
 * implement vpi_get64 for vpiReg objects.
 */
ساكن int64_t signal_get64(صحيح code, vpiHandle ref)
{
      بنية __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      أكد(rfp);

      بدل (code) {
	    // This private property must return zero when undefined.
	  حالة _vpiNexusId:
	    إذا (rfp->msb.get_قدر() == rfp->lsb.get_قدر())
		  أرجع reinterpret_cast<int64_t>(rfp->ڳنڍ);
	    أرجع 0;

	  افتراضي:
	    fprintf(stderr, "VPI error: unknown signal_get64 property %d.\n",
	            code);
	    أرجع vpiUndefined;
      }
}

ساكن حرف* signal_get_str(صحيح code, vpiHandle ref)
{
      بنية __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      أكد(rfp);

      إذا (code == vpiFile) {  // Not implemented for now!
	    أرجع simple_سيٽ_rbuf_str(file_نالا[0]);
      }

      إذا ((code != vpiName) && (code != vpi_ڀريل_Name)) أرجع عدم;

      حرف *nm;
      ثابت حرف *ixs;
      إذا (rfp->is_netarray) {
	    nm = strdup(vpi_get_str(vpiName, rfp->within.والد));
	    s_vpi_قدر vp;
	    vp.format = vpiDecStrVal;
	    vpi_get_قدر(rfp->id.اشاريو, &vp);
	    ixs = vp.value.str;  /* do I need to strdup() this? */
      } وإلا {
	    nm = strdup(rfp->id.name);
	    ixs = عدم;
      }

	/* The scope information is added here for vpiFullName. */
      حرف *rbuf = generic_get_str(code, vpip_scope(rfp), nm, ixs);
      حرر(nm);
      أرجع rbuf;
}

ساكن vpiHandle signal_get_handle(صحيح code, vpiHandle ref)
{
      بنية __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      أكد(rfp);

      بدل (code) {

	  حالة vpi_والد:
	    أرجع rfp->is_netarray? rfp->within.والد : عدم;

	  حالة vpi_اشاريو_2:
	    أرجع rfp->is_netarray? rfp->id.اشاريو : عدم;

	  حالة vpi_کاٻو_Range:
	    أرجع &rfp->msb;
	  حالة vpi_ساڄو_Range:
	    أرجع &rfp->lsb;

	  حالة vpiScope:
	    أرجع vpip_scope(rfp);

	  حالة vpiModule:
	    أرجع vpip_module(vpip_scope(rfp));
      }

      أرجع 0;
}

ساكن vpiHandle signal_iterate(صحيح code, vpiHandle ref)
{
      بنية __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      أكد(rfp);

      إذا (code == vpi_اشاريو_2) {
	    أرجع rfp->is_netarray ? rfp->id.اشاريو->vpi_iterate(code) : عدم;
      }

      أرجع 0;
}

فراغ __vpiSignal::make_بٽ()
{
      أكد(بٽ == عدم);
      بٽ = جديد بنية __vpiBit[width() + 1];

	// Make word[-1] point to the parent.
      بٽ->والد = هذا;
	// Now point to word[0].
      بٽ += 1;

	// Update each bit to point to the base
      بنية __vpiBit*بنياد = بٽ;
      لكل (بنانشان idx = 0; idx < width(); idx += 1) {
	    بنياد[idx].bit0 = بنياد;
	    صحيح real_idx;
	    إذا (msb.get_قدر() >= lsb.get_قدر()) {
		  real_idx = idx + lsb.get_قدر();
	    } وإلا {
		  real_idx = lsb.get_قدر() - idx;
	    }
	    بنياد[idx].اشاريو = جديد __vpiDecConst(real_idx);
      }
}

vpiHandle __vpiSignal::get_اشاريو(صحيح idx)
{
	/* Check to see if the index is in range. */
      إذا (msb.get_قدر() >= lsb.get_قدر()) {
	    إذا ((idx > msb.get_قدر()) || (idx < lsb.get_قدر())) أرجع 0;
      } وإلا {
	    إذا ((idx < msb.get_قدر()) || (idx > lsb.get_قدر())) أرجع 0;
      }

	/* Normalize the index */
      بنانشان norm_idx;
      إذا (msb.get_قدر() >= lsb.get_قدر()) {
	    norm_idx = idx - lsb.get_قدر();
      } وإلا {
	    norm_idx = lsb.get_قدر() - idx;
      }

      إذا (بٽ == عدم) make_بٽ();

      أرجع &(بٽ[norm_idx].as_ٻٽو_انگ);
}

فراغ __vpiSignal::get_ٻٽو_انگ_قدر(ثابت بنية __vpiBit*ٻٽو_انگ, p_vpi_قدر vp)
{
      بنانشان اشاريو = ٻٽو_انگ->get_norm_اشاريو();

      vvp_signal_قدر*vsig = dynamic_cast<vvp_signal_قدر*>(ڳنڍ->fil);
      أكد(vsig);

      إذا (vp->format == vpiObjTypeVal) {
	    vp->format = vpiIntVal;
      }

      بدل (vp->format) {
	حالة vpiBinStrVal:
            format_vpiBinStrVal(vsig, اشاريو, 1, vp);
	    اكسر;

	حالة vpiOctStrVal:
            format_vpiOctStrVal(vsig, اشاريو, 1, vp);
	    اكسر;

	حالة vpiDecStrVal:
            format_vpiDecStrVal(vsig, اشاريو, 1, خطأ, vp);
	    اكسر;

	حالة vpiHexStrVal:
            format_vpiHexStrVal(vsig, اشاريو, 1, vp);
	    اكسر;

	حالة vpiStringVal:
            format_vpiStringVal(vsig, اشاريو, 1, vp);
	    اكسر;

	حالة vpiIntVal:
            format_vpiIntVal(vsig, اشاريو, 1, خطأ, vp);
	    اكسر;

	حالة vpiRealVal:
            format_vpiRealVal(vsig, اشاريو, 1, خطأ, vp);
	    اكسر;

	حالة vpiScalarVal:
            format_vpiScalarVal(vsig, اشاريو, vp);
	    اكسر;

        حالة vpiStrengthVal:
            format_vpiStrengthVal(vsig, اشاريو, 1, vp);
            اكسر;

	حالة vpi_ويڪٽر_Val:
            format_vpi_ويڪٽر_Val(vsig, اشاريو, 1, vp);
	    اكسر;

	افتراضي:
	    fprintf(stderr, "internal error: get_value() "
		    "type %d is not implemented for bit "
		    "select %s[%d] in scope %s.\n",
		    (صحيح)vp->format, vpi_get_str(vpiName),
		    ٻٽو_انگ->get_اشاريو(),
	            vpip_scope(هذا)->scope_name());
	    أكد(0);
      };
}

vpiHandle __vpiSignal::put_ٻٽو_انگ_قدر(بنية __vpiBit*ٻٽو_انگ, p_vpi_قدر vp, صحيح flags)
{
      بنانشان اشاريو = ٻٽو_انگ->get_norm_اشاريو();
      vvp_net_ptr_t dest(ڳنڍ, 0);
      vvp_vector4_t val = vec4_from_vpi_قدر(vp, 1);

      إذا ((flags == vpiForceFlag) || (flags == vpiReleaseFlag)) {
	    fprintf(stderr, "Sorry: vpi_put_value() for %s does not "
	                    "currently support force/release.\n",
	                    ٻٽو_انگ->as_ٻٽو_انگ.vpi_get_str(vpi_ڀريل_Name));
	    أرجع عدم;
      }

      إذا ((get_type_ڪوڊ() == vpiNet) &&
          !dynamic_cast<vvp_island_پورٽ*>(ڳنڍ->fun)) {
	    ڳنڍ->send_vec4_pv(val, اشاريو, width(),
	                            vthread_get_wt_context());
      } وإلا {
	    vvp_send_vec4_pv(dest, val, اشاريو, width(),
	                     vthread_get_wt_context());
      }

	// This is not a scheduled event so there is no event to return
      أرجع عدم;
}

ساكن vpiHandle signal_اشاريو(صحيح idx, vpiHandle ref)
{
      بنية __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      أكد(rfp);

	/* We can only get the bit for a net or reg. */
      PLI_INT32 type = vpi_get(vpiType, ref);
      إذا ((type != vpiNet) && (type != vpiReg)) أرجع 0;

      أرجع rfp->get_اشاريو(idx);
}

بنانشان __vpiSignal::width(فراغ) ثابت
{
      بنانشان wid = (msb.get_قدر() >= lsb.get_قدر())
	    ? (msb.get_قدر() - lsb.get_قدر() + 1)
	    : (lsb.get_قدر() - msb.get_قدر() + 1);

      أرجع wid;
}

/*
 * The get_value method reads the values of the functors and returns
 * the vector to the caller. This causes no side-effect, and reads the
 * variables like a %load would.
 */
ساكن فراغ signal_get_قدر(vpiHandle ref, s_vpi_قدر*vp)
{
      بنية __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      أكد(rfp);

      بنانشان wid = rfp->width();

      vvp_signal_قدر*vsig = dynamic_cast<vvp_signal_قدر*>(rfp->ڳنڍ->fil);
      أكد(vsig);

      بدل (vp->format) {

	  حالة vpiIntVal:
	    format_vpiIntVal(vsig, 0, wid, rfp->signed_flag, vp);
	    اكسر;

	  حالة vpiScalarVal:
	    format_vpiScalarVal(vsig, 0, vp);
	    اكسر;

	  حالة vpiStrengthVal:
	    format_vpiStrengthVal(vsig, 0, wid, vp);
	    اكسر;

	  حالة vpiBinStrVal:
	    format_vpiBinStrVal(vsig, 0, wid, vp);
	    اكسر;

	  حالة vpiHexStrVal:
	    format_vpiHexStrVal(vsig, 0, wid, vp);
	    اكسر;

	  حالة vpiOctStrVal:
	    format_vpiOctStrVal(vsig, 0, wid, vp);
	    اكسر;

	  حالة vpiDecStrVal:
	    format_vpiDecStrVal(vsig, 0, wid, rfp->signed_flag, vp);
	    اكسر;

	  حالة vpiStringVal:
	    format_vpiStringVal(vsig, 0, wid, vp);
	    اكسر;

	  حالة vpi_ويڪٽر_Val:
	    format_vpi_ويڪٽر_Val(vsig, 0, wid, vp);
	    اكسر;

	  حالة vpiRealVal:
	    format_vpiRealVal(vsig, 0, wid, rfp->signed_flag, vp);
	    اكسر;

	  حالة vpiObjTypeVal:
	    إذا (wid == 1) {
		  vp->format = vpiScalarVal;
		  format_vpiScalarVal(vsig, 0, vp);
	    } وإلا {
		  vp->format = vpi_ويڪٽر_Val;
		  format_vpi_ويڪٽر_Val(vsig, 0, wid, vp);
	    }
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "vvp internal error: get_value: "
		    "value type %d not implemented."
		    " Signal is %s in scope %s\n",
		    (صحيح)vp->format, vpi_get_str(vpiName, ref),
		    vpip_scope(rfp)->scope_name());
	    أكد(0);
      }
}

/*
 * The put_value method writes the value into the vector, and returns
 * the affected ref. This operation works much like the %set or
 * %assign instructions and causes all the side-effects that the
 * equivalent instruction would cause.
 */

ساكن vvp_vector4_t from_stringval(ثابت حرف*str, بنانشان wid)
{
      بنانشان idx;
      ثابت حرف*cp;

      cp = str + لڙي_ڊيگهه(str);
      idx = 0;

      vvp_vector4_t val(wid, BIT4_0);

      طالما ((idx < wid) && (cp > str)) {
	    بنانشان byte = *--cp;
	    صحيح ٻٽو_انگ;

	    لكل (ٻٽو_انگ = 0 ;  (ٻٽو_انگ < 8) && (idx < wid) ;  ٻٽو_انگ += 1) {
		  إذا (byte & 1)
			val.سيٽ_ٻٽو_انگ(idx, BIT4_1);

		  byte >>= 1;
		  idx += 1;
	    }
      }

      أرجع val;
}

ساكن vpiHandle signal_put_قدر(vpiHandle ref, s_vpi_قدر*vp, صحيح flags)
{
      بنانشان wid;
      بنية __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      أكد(rfp);
      vvp_net_ptr_t dest(rfp->ڳنڍ, 0);

      منطقي net_flag = ref->get_type_ڪوڊ()==vpiNet;

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value. Note that
	   if this net is attached to an island, we need to rerun
	   the calculations immediately so we can return the
	   released value. */
      إذا (flags == vpiReleaseFlag) {
	    أكد(rfp->ڳنڍ->fil);
	    rfp->ڳنڍ->fil->force_unlink();
	    rfp->ڳنڍ->fil->release(dest, net_flag);
	    rfp->ڳنڍ->fun->force_flag(سچ);
	    signal_get_قدر(ref, vp);
	    أرجع ref;
      }

	/* Make a vvp_vector4_t vector to receive the translated value
	   that we are going to poke. This will get populated
	   differently depending on the format. */
      wid = (rfp->msb.get_قدر() >= rfp->lsb.get_قدر())
	    ? (rfp->msb.get_قدر() - rfp->lsb.get_قدر() + 1)
	    : (rfp->lsb.get_قدر() - rfp->msb.get_قدر() + 1);

      vvp_vector4_t val = vec4_from_vpi_قدر(vp, wid);

      إذا (flags == vpiForceFlag) {
	    vvp_vector2_t نقاب (vvp_vector2_t::FILL1, wid);
	    rfp->ڳنڍ->force_vec4(val, نقاب);
      } وإلا إذا (net_flag && !dynamic_cast<vvp_island_پورٽ*>(rfp->ڳنڍ->fun)) {
	    rfp->ڳنڍ->send_vec4(val, vthread_get_wt_context());
      } وإلا {
	    vvp_send_vec4(dest, val, vthread_get_wt_context());
      }
      أرجع ref;
}

vvp_vector4_t vec4_from_vpi_قدر(s_vpi_قدر*vp, بنانشان wid)
{
      vvp_vector4_t val (wid, BIT4_0);

      بدل (vp->format) {

	  حالة vpiIntVal: {
		ڊگهو vpi_val = vp->value.integer;
		لكل (بنانشان idx = 0 ;  idx < wid ;  idx += 1) {
		      vvp_bit4_t ٻٽو_انگ = (vpi_val & 1) ? BIT4_1 : BIT4_0;
		      val.سيٽ_ٻٽو_انگ(idx, ٻٽو_انگ);
		      vpi_val >>= 1;
		}
		اكسر;
	  }

	  حالة vpi_ويڪٽر_Val:
	    لكل (بنانشان idx = 0 ;  idx < wid ;  idx += 1) {
		  بنانشان ڊگهو aval = vp->value.vector[idx/32].aval;
		  بنانشان ڊگهو bval = vp->value.vector[idx/32].bval;
		  aval >>= idx%32;
		  bval >>= idx%32;
		  صحيح bitmask = (aval&1) | ((bval<<1)&2);
		  vvp_bit4_t ٻٽو_انگ = scalar_منزل_bit4(bitmask);
		  val.سيٽ_ٻٽو_انگ(idx, ٻٽو_انگ);
	    }
	    اكسر;
	  حالة vpiBinStrVal:
	    vpip_bin_str_منزل_vec4(val, vp->value.str);
	    اكسر;
	  حالة vpiOctStrVal:
	    vpip_oct_str_منزل_vec4(val, vp->value.str);
	    اكسر;
	  حالة vpiDecStrVal:
	    vpip_dec_str_منزل_vec4(val, vp->value.str);
	    اكسر;
	  حالة vpiHexStrVal:
	    vpip_hex_str_منزل_vec4(val, vp->value.str);
	    اكسر;
	  حالة vpiScalarVal:
	    val.سيٽ_ٻٽو_انگ(0, scalar_منزل_bit4(vp->value.scalar));
	    اكسر;
	  حالة vpiStringVal:
	    val = from_stringval(vp->value.str, wid);
	    اكسر;
	  حالة vpiRealVal:
	    val = vvp_vector4_t(wid, vp->value.real);
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "vvp internal error: put_value: "
		    "value type %d not implemented here.\n",
		    (صحيح)vp->format);
	    أكد(0);

      }

      أرجع val;
}

صحيح __vpiSignal::vpi_get(صحيح code)
{ أرجع signal_get(code, هذا); }

int64_t __vpiSignal::vpi_get64(صحيح code)
{ أرجع signal_get64(code, هذا); }

حرف* __vpiSignal::vpi_get_str(صحيح code)
{ أرجع signal_get_str(code, هذا); }

فراغ __vpiSignal::vpi_get_قدر(p_vpi_قدر val)
{ signal_get_قدر(هذا, val); }

vpiHandle __vpiSignal::vpi_put_قدر(p_vpi_قدر val, صحيح flags)
{ أرجع signal_put_قدر(هذا, val, flags); }

vpiHandle __vpiSignal::vpi_handle(صحيح code)
{ أرجع signal_get_handle(code, هذا); }

vpiHandle __vpiSignal::vpi_iterate(صحيح code)
{ أرجع signal_iterate(code, هذا); }

vpiHandle __vpiSignal::vpi_اشاريو(صحيح idx)
{ أرجع signal_اشاريو(idx, هذا); }

بنية signal_reg : عام __vpiSignal {
      مضمن signal_reg() { }
      صحيح get_type_ڪوڊ(فراغ) ثابت override { أرجع vpiReg; }
};

بنية signal_integer : عام __vpiSignal {
      مضمن signal_integer() { }
      صحيح get_type_ڪوڊ(فراغ) ثابت override { أرجع vpiIntegerVar; }
};

بنية signal_net : عام __vpiSignal {
      مضمن signal_net() { }
      صحيح get_type_ڪوڊ(فراغ) ثابت override { أرجع vpiNet; }
};

بنية signal_byte : عام __vpiSignal {
      مضمن signal_byte() { }
      صحيح get_type_ڪوڊ(فراغ) ثابت override { أرجع vpiByteVar; }
};

بنية signal_bitvar : عام __vpiSignal {
      مضمن signal_bitvar() { }
      صحيح get_type_ڪوڊ(فراغ) ثابت override { أرجع vpi_ٻٽو_انگ_Var; }
};

بنية signal_shortint : عام __vpiSignal {
      مضمن signal_shortint() { }
      صحيح get_type_ڪوڊ(فراغ) ثابت override { أرجع vpiShortIntVar; }
};

بنية signal_int : عام __vpiSignal {
      مضمن signal_int() { }
      صحيح get_type_ڪوڊ(فراغ) ثابت override { أرجع vpiIntVar; }
};

بنية signal_longint : عام __vpiSignal {
      مضمن signal_longint() { }
      صحيح get_type_ڪوڊ(فراغ) ثابت override { أرجع vpiLongIntVar; }
};


/*
 * Construct a vpiIntegerVar object. Indicate the type using a flag
 * to minimize the code modifications. Icarus implements integers
 * as 'reg signed [31:0]'.
 */
vpiHandle vpip_make_int4(ثابت حرف*name, صحيح msb, صحيح lsb, vvp_net_t*vec)
{
      __vpiSignal*obj = جديد signal_integer;
      أرجع fill_اندريون_var4(obj, name, msb, lsb, سچ, vec);
}

/*
 * Construct the two-state SystemVerilog variables.
 */
vpiHandle vpip_make_int2(ثابت حرف*name, صحيح msb, صحيح lsb, منطقي signed_flag,
                         vvp_net_t*vec)
{
      __vpiSignal*obj;

	// All unsigned 2-state variables are a vpiBitVar. All 2-state
	// variables with a non-zero lsb are also a vpiBitVar.
      إذا ((! signed_flag) || (lsb != 0) ) {
	    obj = جديد signal_bitvar;
      } وإلا {
	      // These could also be bit declarations with matching
	      // information, but for now they get the apparent type.
	    بدل (msb) {
		حالة 7:
		  obj = جديد signal_byte;
		  اكسر;
		حالة 15:
		  obj = جديد signal_shortint;
		  اكسر;
		حالة 31:
		  obj = جديد signal_int;
		  اكسر;
		حالة 63:
		  obj = جديد signal_longint;
		  اكسر;
		افتراضي:
		    // Every other type of bit vector is a vpiBitVar with
		    // array dimensions.
		  obj = جديد signal_bitvar;
		  اكسر;
	    }
      }

      أرجع fill_اندريون_var4(obj, name, msb, lsb, signed_flag, vec);
}

/*
 * Construct a vpiReg/vpiLogicVar object. It's like a net, except for the type.
 */
vpiHandle vpip_make_var4(ثابت حرف*name, صحيح msb, صحيح lsb,
			منطقي signed_flag, vvp_net_t*vec)
{
      __vpiSignal*obj = جديد signal_reg;
      أرجع fill_اندريون_var4(obj, name, msb, lsb, signed_flag, vec);
}

#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
ساكن بنية vpiSignal_plug **signal_pool = 0;
ساكن بنانشان signal_pool_ڳڻپ = 0;
ساكن بنانشان ڊگهو signal_ڳڻپ = 0;
ساكن بنانشان ڊگهو signal_dels = 0;
#جيڪڏهن_پورو

بنية vpiSignal_plug {
      بنانشان حرف space[حجم (بنية __vpiSignal)];
#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
      بنية vpiSignal_plug *pool;
#جيڪڏهن_پورو
};

فراغ* __vpiSignal::عامل جديد(size_t siz)
{
#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
      أكد(siz == حجم(بنية vpiSignal_plug) - حجم(بنية vpiSignal_plug *));
#وإلا
      أكد(siz == حجم(بنية vpiSignal_plug));
#جيڪڏهن_پورو
      ساكن بنية vpiSignal_plug*alloc_array = 0;
      ساكن بنانشان alloc_اشاريو = 0;
      ثابت بنانشان alloc_ڳڻپ = 512;

      إذا ((alloc_array == 0) || (alloc_اشاريو == alloc_ڳڻپ)) {
	    alloc_array = static_cast<بنية vpiSignal_plug*>
	                  (صفر_يادگيري_ڏيو(alloc_ڳڻپ, حجم(بنية vpiSignal_plug)));
	    alloc_اشاريو = 0;
#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
	    VALGRIND_MAKE_MEM_NOACCESS(alloc_array, alloc_ڳڻپ *
	                                            حجم(بنية vpiSignal_plug));
	    VALGRIND_CREATE_MEMPOOL(alloc_array, 0, 1);
	    signal_pool_ڳڻپ += 1;
	    signal_pool = static_cast<vpiSignal_plug **>
	                  (realloc(signal_pool,
	                   signal_pool_ڳڻپ*حجم(vpiSignal_plug **)));
	    signal_pool[signal_pool_ڳڻپ-1] = alloc_array;
#جيڪڏهن_پورو
      }

      بنية vpiSignal_plug*cur = alloc_array + alloc_اشاريو;
#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
      VALGRIND_MEMPOOL_ALLOC(alloc_array, cur, حجم(بنية vpiSignal_plug));
      cur->pool = alloc_array;
      signal_ڳڻپ += 1;
#جيڪڏهن_پورو
      alloc_اشاريو += 1;
      أرجع cur;
}

فراغ __vpiSignal::عامل ميٽيو(فراغ*)
{
      أكد(0);
}

#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
فراغ signal_delete(vpiHandle شي)
{
      بنية __vpiSignal *obj = static_cast<__vpiSignal *> (شي);
      أكد(obj->ڳنڍ->fil);
      obj->ڳنڍ->fil->clear_all_callbacks();
      vvp_net_delete(obj->ڳنڍ);
      إذا (obj->بٽ) {
	    لكل (بنانشان idx=0; idx<obj->width(); idx+=1) {
		  ميٽيو obj->بٽ[idx].اشاريو;
	    }
	    obj->بٽ -= 1;
	    ميٽيو [] obj->بٽ;
      }
      signal_dels += 1;
      VALGRIND_MEMPOOL_FREE(reinterpret_cast<vpiSignal_plug *>(obj)->pool, obj);
}

فراغ signal_pool_delete()
{
      إذا (RUNNING_ON_VALGRIND && (signal_ڳڻپ != signal_dels)) {
	    fflush(عدم);
	    VALGRIND_PRINTF("Error: vvp missed deleting %ld of %lu signal(s).",
	                    (ڊگهو) signal_ڳڻپ - signal_dels, signal_ڳڻپ);
      }

      لكل (بنانشان idx = 0; idx < signal_pool_ڳڻپ; idx += 1) {
	    VALGRIND_DESTROY_MEMPOOL(signal_pool[idx]);
	    حرر(signal_pool[idx]);
      }

      حرر(signal_pool);
      signal_pool = 0;
      signal_pool_ڳڻپ = 0;
}
#جيڪڏهن_پورو

/*
 * Construct a vpiNet object. Give the object specified dimensions,
 * and point to the specified functor for the lsb.
 *
 * The name is the PLI name for the object. If it is an array it is
 * <name>[<index>].
 */
ساكن vpiHandle fill_اندريون_net4(بنية __vpiSignal*obj, __vpiScope*scope,
			      ثابت حرف*name, صحيح msb, صحيح lsb,
			      منطقي signed_flag, vvp_net_t*ڳنڍ)
{
      obj->id.name = name? vpip_name_string(name) : عدم;
      obj->msb = __vpiDecConst(msb);
      obj->lsb = __vpiDecConst(lsb);
      obj->signed_flag = signed_flag? 1 : 0;
      obj->is_netarray = 0;
      obj->ڳنڍ = ڳنڍ;

	// Place this object within a scope. If this object is
	// attached to an array, then this value will be replaced with
	// the handle to the parent.
      obj->within.scope = scope;

      ڳڻپ_vpi_nets += 1;

      أرجع obj;
}

vpiHandle vpip_make_net4(__vpiScope*scope,
			 ثابت حرف*name, صحيح msb, صحيح lsb,
			 منطقي signed_flag, vvp_net_t*ڳنڍ)
{
      بنية __vpiSignal*obj = جديد signal_net;
      أرجع fill_اندريون_net4(obj, scope, name, msb, lsb, signed_flag, ڳنڍ);
}

ساكن صحيح PV_get_بنياد(بنية __vpiPV*rfp)
{
	/* We return from the symbol base if it is defined. */
      إذا (rfp->sbase != 0) {
	    s_vpi_قدر val;
	      /* Check to see if the value is defined. */
	    val.format = vpi_ويڪٽر_Val;
	    vpi_get_قدر(rfp->sbase, &val);
	    صحيح لفظ = (vpi_get(vpi_سائيز, rfp->sbase)-1)/32 + 1;
	    لكل(صحيح idx = 0; idx < لفظ; idx += 1) {
		    /* Return INT_MIN to indicate an X base. */
		  إذا (val.value.vector[idx].bval != 0) أرجع INT_MIN;
	    }
	      /* The value is defined so get and return it. */
	    val.format = vpiIntVal;
	    vpi_get_قدر(rfp->sbase, &val);
	    أرجع val.value.integer;
      }

	/* If the width is zero then tbase is the constant. */
      أرجع rfp->tbase;
}

ساكن صحيح PV_get(صحيح code, vpiHandle ref)
{
      بنية __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      أكد(rfp);

      صحيح rval = 0;
      بدل (code) {
	حالة vpi_سٽ_No:
	    أرجع 0;  // Not implemented for now!

	حالة vpiSigned:
	    أرجع 0;  // A part/bit select is always unsigned!

	حالة vpi_سائيز:
	    أرجع rfp->width;

	  /* This is like the &A<> in array.cc. */
	حالة vpiConstantSelect:
	    أرجع rfp->sbase == 0;

	حالة vpi_کاٻو_Range:
            rval += rfp->width - 1;
	    // fallthrough
	حالة vpi_ساڄو_Range: {
	    صحيح کاٻي = vpi_get(vpi_کاٻو_Range, rfp->والد);
	    صحيح ساڄي = vpi_get(vpi_ساڄو_Range, rfp->والد);
	    rval += PV_get_بنياد(rfp);
	    أرجع کاٻي >= ساڄي ? ساڄي + rval : ساڄي - rval;
	}

        حالة vpiAutomatic:
            أرجع vpi_get(vpiAutomatic, rfp->والد);

#إذا defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
        حالة _vpiFromThr:
            أرجع _vpi_at_PV;
#جيڪڏهن_پورو

	افتراضي:
	    fprintf(stderr, "PV_get: property %d is unknown\n", code);
      }

      أرجع 0;
}

ساكن حرف* PV_get_str(صحيح code, vpiHandle ref)
{
      بنية __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      أكد(rfp);

      بدل (code) {
	حالة vpiFile:  // Not implemented for now!
	    أرجع simple_سيٽ_rbuf_str(file_نالا[0]);

	حالة vpiName:
	حالة vpi_ڀريل_Name: {
	    ثابت حرف*nm = vpi_get_str(code, rfp->والد);
	    size_t len = 256+لڙي_ڊيگهه(nm);
	    حرف *ڀريل = static_cast<حرف *>(يادگيري_ڏيو(len));
	    snprintf(ڀريل, len, "%s[%d:%d]", nm,
	                                     (صحيح)vpi_get(vpi_کاٻو_Range, ref),
	                                     (صحيح)vpi_get(vpi_ساڄو_Range, ref));
	    ڀريل[len-1] = 0;
	    حرف *res = simple_سيٽ_rbuf_str(ڀريل);
	    حرر(ڀريل);
	    أرجع res;
	}

	افتراضي:
	    fprintf(stderr, "PV_get_str: property %d is unknown.\n", code);
      }

      أرجع 0;
}

ساكن فراغ PV_get_قدر(vpiHandle ref, p_vpi_قدر vp)
{
      بنية __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      أكد(rfp);

      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*>(rfp->net->fil);
      أكد(sig);

      بدل (vp->format) {

	  حالة vpiIntVal:
	    format_vpiIntVal(sig, PV_get_بنياد(rfp), rfp->width, 0, vp);
	    اكسر;

	  حالة vpiBinStrVal:
	    format_vpiBinStrVal(sig, PV_get_بنياد(rfp), rfp->width, vp);
	    اكسر;

	  حالة vpiOctStrVal:
	    format_vpiOctStrVal(sig, PV_get_بنياد(rfp), rfp->width, vp);
	    اكسر;

	  حالة vpiHexStrVal:
	    format_vpiHexStrVal(sig, PV_get_بنياد(rfp), rfp->width, vp);
	    اكسر;

	  حالة vpiDecStrVal:
	    format_vpiDecStrVal(sig, PV_get_بنياد(rfp), rfp->width, 0, vp);
	    اكسر;

	  حالة vpiStringVal:
	    format_vpiStringVal(sig, PV_get_بنياد(rfp), rfp->width, vp);
	    اكسر;

	  حالة vpiScalarVal:
	    format_vpiScalarVal(sig, PV_get_بنياد(rfp), vp);
	    اكسر;

	  حالة vpiStrengthVal:
	    format_vpiStrengthVal(sig, PV_get_بنياد(rfp), rfp->width, vp);
	    اكسر;

	  حالة vpi_ويڪٽر_Val:
	    format_vpi_ويڪٽر_Val(sig, PV_get_بنياد(rfp), rfp->width, vp);
	    اكسر;

	  حالة vpiRealVal:
	    format_vpiRealVal(sig, PV_get_بنياد(rfp), rfp->width, 0, vp);
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "vvp internal error: PV_get_value: "
		    "value type %d not implemented. Signal is %s.\n",
		    (صحيح)vp->format, vpi_get_str(vpi_ڀريل_Name, rfp->والد));
	    أكد(0);
      }
}

ساكن vpiHandle PV_put_قدر(vpiHandle ref, p_vpi_قدر vp, صحيح flags)
{
      بنية __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      أكد(rfp);
      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*>(rfp->net->fil);
      أكد(sig);

      بنانشان sig_سائيز = sig->قدر_سائيز();
      بنانشان width = rfp->width;
      صحيح بنياد = PV_get_بنياد(rfp);
      إذا (بنياد >= (نشانوارو) sig_سائيز) أرجع 0;
      إذا (بنياد + (نشانوارو) width < 0) أرجع 0;

      vvp_vector4_t val;
      إذا (flags != vpiReleaseFlag) {
	    val = vec4_from_vpi_قدر(vp, width);
      }

	/*
	 * If the base is less than zero then trim off any unneeded
	 * lower bits.
	 */
      إذا (بنياد < 0) {
	    width += بنياد;
	    إذا (flags != vpiReleaseFlag) {
		  val = val.subvalue(-بنياد, width);
	    }
	    بنياد = 0;
      }

	/*
	 * If the value is wider than the signal then trim off any
	 * unneeded upper bits.
	 */
      إذا (بنياد+width > sig_سائيز) {
	    width = sig_سائيز - بنياد;
	    إذا (flags != vpiReleaseFlag) {
		  val = val.subvalue(0, width);
	    }
      }

      أكد(rfp->والد);
      منطقي net_flag = rfp->والد->get_type_ڪوڊ()==vpiNet;
      منطقي ڀريل_sig = بنياد == 0 && width == sig_سائيز;

      vvp_net_ptr_t dest(rfp->net, 0);

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value.  Note that
	   if this net is attached to an island, we need to rerun
	   the calculations immediately so we can return the
	   released value.*/
      إذا (flags == vpiReleaseFlag) {
	    أكد(rfp->net->fil);
	      // XXXX Can't really do this if this is a partial release?
	    rfp->net->fil->force_unlink();
	    إذا (ڀريل_sig) {
		  rfp->net->fil->release(dest, net_flag);
	    } وإلا {
		  rfp->net->fil->release_pv(dest, بنياد, width, net_flag);
	    }
	    rfp->net->fun->force_flag(سچ);
	    PV_get_قدر(ref, vp);
	    أرجع ref;
      }

      إذا (flags == vpiForceFlag) {
	    إذا (ڀريل_sig) {
		  vvp_vector2_t نقاب (vvp_vector2_t::FILL1, sig_سائيز);
		  rfp->net->force_vec4(val, نقاب);
	    } وإلا {
		  vvp_vector2_t نقاب (vvp_vector2_t::FILL0, sig_سائيز);
		  لكل (بنانشان idx = 0 ; idx < width ; idx += 1)
		        نقاب.سيٽ_ٻٽو_انگ(بنياد+idx, 1);

		  vvp_vector4_t tmp (sig_سائيز, BIT4_Z);

		    // vvp_net_t::force_vec4 propagates all the bits of the
		    // forced vector value, regardless of the mask. This
		    // ensures the unforced bits retain their current value.
		  sig->vec4_قدر(tmp);

		  tmp.سيٽ_vec(بنياد, val);
		  rfp->net->force_vec4(tmp, نقاب);
	    }
      } وإلا إذا (net_flag && !dynamic_cast<vvp_island_پورٽ*>(rfp->net->fun)) {
	    إذا (ڀريل_sig) {
		  rfp->net->send_vec4(val, vthread_get_wt_context());
	    } وإلا {
		  rfp->net->send_vec4_pv(val, بنياد, sig_سائيز,
		                         vthread_get_wt_context());
	    }
      } وإلا {
	    إذا (ڀريل_sig) {
		  vvp_send_vec4(dest, val, vthread_get_wt_context());
	    } وإلا {
		  vvp_send_vec4_pv(dest, val, بنياد, sig_سائيز,
	                           vthread_get_wt_context());
	    }
      }
      أرجع 0;
}

ساكن vpiHandle PV_get_handle(صحيح code, vpiHandle ref)
{
      بنية __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      أكد(rfp);

      بدل (code) {
	  حالة vpi_والد:
	    أرجع rfp->والد;

	  حالة vpiScope:
	    أرجع vpi_handle(vpiScope, rfp->والد);

	  حالة vpiModule:
	    أرجع vpi_handle(vpiModule, rfp->والد);
      }

      أرجع 0;
}

مضمن __vpiPV::__vpiPV()
{ }

صحيح __vpiPV::get_type_ڪوڊ(فراغ) ثابت
{ أرجع vpiPartSelect; }

صحيح __vpiPV::vpi_get(صحيح code)
{ أرجع PV_get(code, هذا); }

حرف* __vpiPV::vpi_get_str(صحيح code)
{ أرجع PV_get_str(code, هذا); }

فراغ __vpiPV::vpi_get_قدر(p_vpi_قدر val)
{ PV_get_قدر(هذا, val); }

vpiHandle __vpiPV::vpi_put_قدر(p_vpi_قدر val, صحيح flags)
{ أرجع PV_put_قدر(هذا, val, flags); }

vpiHandle __vpiPV::vpi_handle(صحيح code)
{ أرجع PV_get_handle(code, هذا); }

vpiHandle vpip_make_PV(حرف*var, صحيح بنياد, صحيح width)
{
      بنية __vpiPV*obj = جديد __vpiPV;
      compile_vpi_lookup(&obj->والد, strdup(var));
      obj->sbase = 0;
      obj->tbase = بنياد;
      obj->width = (بنانشان) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      أرجع obj;
}

vpiHandle vpip_make_PV(حرف*var, حرف*نشان, صحيح width)
{
      بنية __vpiPV*obj = جديد __vpiPV;
      compile_vpi_lookup(&obj->والد, strdup(var));
      compile_vpi_lookup(&obj->sbase, نشان);
      obj->tbase = 0;
      obj->width = (بنانشان) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      أرجع obj;
}

vpiHandle vpip_make_PV(حرف*var, vpiHandle handle, صحيح width)
{
      بنية __vpiPV*obj = جديد __vpiPV;
      compile_vpi_lookup(&obj->والد, strdup(var));
      obj->sbase = handle;
      obj->tbase = 0;
      obj->width = (بنانشان) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      أرجع obj;
}

#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
فراغ PV_delete(vpiHandle شي)
{
      بنية __vpiPV *obj = dynamic_cast<__vpiPV*>(شي);
      إذا (obj->sbase) {
	    بدل (obj->sbase->get_type_ڪوڊ()) {
		حالة vpi_يادگيري_لفظ:
		  إذا (vpi_get(_vpiFromThr, obj->sbase) == _vpi_at_A) {
			A_delete(obj->sbase);
		  }
		  اكسر;
		حالة vpiPartSelect:
		  أكد(vpi_get(_vpiFromThr, obj->sbase) == _vpi_at_PV);
		  PV_delete(obj->sbase);
		  اكسر;
	    }
      }
      أكد(obj->net->fil);
      obj->net->fil->clear_all_callbacks();
      ميٽيو obj;
}
#جيڪڏهن_پورو
