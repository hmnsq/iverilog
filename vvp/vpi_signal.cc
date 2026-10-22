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
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#اگر_ختم
# include  <cmath>
# include  <iostream>
# include  <cstdio>
# include  <cstdlib>
# include  <climits>
# include  <cstring>
# include  <cassert>
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
# include  <valgrind/memcheck.h>
#اگر_ختم
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
خارجی ثابت حرف hex_digits[256];
خارجی ثابت حرف oct_digits[64];

/*
 * The string values need a result buf to hold the results. This
 * buffer can be reused for that purpose. Whenever I have a need, the
 * need_result_buf function makes sure that need can be met.
 */
خالی *need_نتیجہ_buf(غیرعلامتی cnt, vpi_rbuf_t type)
{
      ساکن خالی*نتیجہ_buf[2] = {0, 0};
      ساکن size_t نتیجہ_buf_سائز[2] = {0, 0};

      اگر (type == RBUF_DEL) {
	    آزاد(نتیجہ_buf[RBUF_VAL]);
	    نتیجہ_buf[RBUF_VAL] = 0;
	    نتیجہ_buf_سائز[RBUF_VAL] = 0;

	    آزاد(نتیجہ_buf[RBUF_STR]);
	    نتیجہ_buf[RBUF_STR] = 0;
	    نتیجہ_buf_سائز[RBUF_STR] = 0;

	    واپس 0;
      }

      cnt = (cnt + 0x0fff) & ~0x0fff;

      اگر (نتیجہ_buf_سائز[type] == 0) {
	    نتیجہ_buf[type] = حافظہ_دو(cnt);
	    نتیجہ_buf_سائز[type] = cnt;
      } ورنہ اگر (نتیجہ_buf_سائز[type] < cnt) {
	    نتیجہ_buf[type] = realloc(نتیجہ_buf[type], cnt);
	    نتیجہ_buf_سائز[type] = cnt;
      }

      واپس نتیجہ_buf[type];
}

حرف *simple_سیٹ_rbuf_str(ثابت حرف *s1)
{
      حرف *res = static_cast<حرف *>(need_نتیجہ_buf(لڑی_لمبائی(s1)+1, RBUF_STR));
      اگر (res) strcpy(res,s1);
      واپس res;
}

حرف *generic_get_str(صحیح code, vpiHandle ref, ثابت حرف *name, ثابت حرف *اشاریہ)
{
      size_t len = لڑی_لمبائی(name) + 1;  /* include space for null termination */
      حرف *bn = تہی;
      منطقی is_bn_a_pkg = جھوٹ;
      اگر (code == vpi_بھرا_Name) {
	    bn = strdup(vpi_get_str(code,ref));
	    size_t bn_len = لڑی_لمبائی(bn);
	    is_bn_a_pkg = (bn_len >= 3) && (bn[bn_len-1] == ':') && (bn[bn_len-2] == ':');
	    len += bn_len;
	    اگر (! is_bn_a_pkg) len += 1; // include space for "." separator
      }
      اگر (اشاریہ != تہی) len += لڑی_لمبائی(اشاریہ) + 2;  /* include space for brackets */

      حرف *res = static_cast<حرف *>(need_نتیجہ_buf(len, RBUF_STR));
      اگر (!res) {
	    آزاد(bn);
	    واپس تہی;
      }
      *res=0;  /* start with nothing */

	/* if this works, I can make it more efficient later */
      اگر (bn != تہی) {
	    strcat(res, bn);
	      // A package already has the "::" separator in the name
	    اگر (! is_bn_a_pkg) strcat(res, ".");
	    آزاد(bn);
      }
      strcat(res, name);
      اگر (اشاریہ != تہی) {
	    strcat(res, "[");
	    strcat(res, اشاریہ);
	    strcat(res, "]");
      }
      واپس res;
}

ساکن vpiHandle fill_اندر_net4(ساخت __vpiSignal*obj, __vpiScope*scope,
                              ثابت حرف*name, صحیح msb, صحیح lsb,
                              منطقی signed_flag, vvp_net_t*گرہ);

ساکن vpiHandle fill_اندر_var4(ساخت __vpiSignal*obj,
                              ثابت حرف*name, صحیح msb, صحیح lsb,
                              منطقی signed_flag, vvp_net_t*گرہ)
{
	// Variable declarations are always resolved immediately,
	// so we can assume they belong in the current scope.
      واپس fill_اندر_net4(obj, vpip_peek_current_scope(),
			  name, msb, lsb, signed_flag, گرہ);
}

/*
 * The standard formatting/conversion routines.
 * They work with full or partial signals.
 */

ساکن خالی format_vpiBinStrVal(ثابت vvp_signal_قدر*sig, صحیح بنیاد, غیرعلامتی wid,
                                s_vpi_قدر*vp)
{
      حرف *rbuf = static_cast<حرف *>(need_نتیجہ_buf(wid+1, RBUF_VAL));
      طویل انتہا = بنیاد + (علامتی)wid;
      طویل انحراف_2 = انتہا - 1;
      طویل ssize = (علامتی)sig->قدر_سائز();

      برائے (طویل idx = بنیاد ;  idx < انتہا ;  idx += 1) {
	    اگر (idx < 0 || idx >= ssize) {
                  rbuf[انحراف_2-idx] = 'x';
	    } ورنہ {
                  rbuf[انحراف_2-idx] = vvp_bit4_مقصد_ascii(sig->value(idx));
	    }
      }
      rbuf[wid] = 0;

      vp->value.str = rbuf;
}

ساکن خالی format_vpiOctStrVal(ثابت vvp_signal_قدر*sig, صحیح بنیاد, غیرعلامتی wid,
                                s_vpi_قدر*vp)
{
      غیرعلامتی dwid = (wid + 2) / 3;
      حرف *rbuf = static_cast<حرف *>(need_نتیجہ_buf(dwid+1, RBUF_VAL));
      طویل انتہا = بنیاد + (علامتی)wid;
      طویل ssize = (علامتی)sig->قدر_سائز();
      غیرعلامتی val = 0;

      rbuf[dwid] = 0;
      برائے (طویل idx = بنیاد ;  idx < انتہا ;  idx += 1) {
	    غیرعلامتی ثنائی_ہندسہ = 0;
	    اگر (idx < 0 || idx >= ssize) {
                  ثنائی_ہندسہ = 2; // BIT4_X
	    } ورنہ {
                  انتخاب (sig->value(idx)) {
		      حالت BIT4_0:
			ثنائی_ہندسہ = 0;
			توڑو;
		      حالت BIT4_1:
			ثنائی_ہندسہ = 1;
			توڑو;
		      حالت BIT4_X:
			ثنائی_ہندسہ = 2;
			توڑو;
		      حالت BIT4_Z:
			ثنائی_ہندسہ = 3;
			توڑو;
                  }
	    }
	    val |= ثنائی_ہندسہ << 2*((idx-بنیاد) % 3);

	    اگر ((idx-بنیاد) % 3 == 2) {
		dwid -= 1;
		rbuf[dwid] = oct_digits[val];
		val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      انتخاب (wid % 3) {
	  حالت 1:
	    اگر (val == 2) val = 42;
	    ورنہ اگر (val == 3) val = 63;
	    توڑو;
	  حالت 2:
	    اگر (val == 10) val = 42;
	    ورنہ اگر (val == 15) val = 63;
	    توڑو;
      }

      اگر (dwid > 0) rbuf[0] = oct_digits[val];

      vp->value.str = rbuf;
}

ساکن خالی format_vpiHexStrVal(ثابت vvp_signal_قدر*sig, صحیح بنیاد, غیرعلامتی wid,
                                s_vpi_قدر*vp)
{
      غیرعلامتی dwid = (wid + 3) / 4;
      حرف *rbuf = static_cast<حرف *>(need_نتیجہ_buf(dwid+1, RBUF_VAL));
      طویل انتہا = بنیاد + (علامتی)wid;
      طویل ssize = (علامتی)sig->قدر_سائز();
      غیرعلامتی val = 0;

      rbuf[dwid] = 0;
      برائے (طویل idx = بنیاد ;  idx < انتہا ;  idx += 1) {
	    غیرعلامتی ثنائی_ہندسہ = 0;
	    اگر (idx < 0 || idx >= ssize) {
                  ثنائی_ہندسہ = 2; // BIT4_X
	    } ورنہ {
                  انتخاب (sig->value(idx)) {
		      حالت BIT4_0:
			ثنائی_ہندسہ = 0;
			توڑو;
		      حالت BIT4_1:
			ثنائی_ہندسہ = 1;
			توڑو;
		      حالت BIT4_X:
			ثنائی_ہندسہ = 2;
			توڑو;
		      حالت BIT4_Z:
			ثنائی_ہندسہ = 3;
			توڑو;
                  }
	    }
	    val |= ثنائی_ہندسہ << 2*((idx-بنیاد) % 4);

	    اگر ((idx-بنیاد) % 4 == 3) {
		dwid -= 1;
		rbuf[dwid] = hex_digits[val];
		val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      انتخاب (wid % 4) {
	  حالت 1:
	    اگر (val == 2) val = 170;
	    ورنہ اگر (val == 3) val = 255;
	    توڑو;
	  حالت 2:
	    اگر (val == 10) val = 170;
	    ورنہ اگر (val == 15) val = 255;
	    توڑو;
	  حالت 3:
	    اگر (val == 42) val = 170;
	    ورنہ اگر (val == 63) val = 255;
	    توڑو;
      }

      اگر (dwid > 0) rbuf[0] = hex_digits[val];

      vp->value.str = rbuf;
}

ساکن خالی format_vpiDecStrVal(ثابت vvp_signal_قدر*sig, صحیح بنیاد, غیرعلامتی wid,
                                صحیح signed_flag, s_vpi_قدر*vp)
{
      غیرعلامتی hwid = (sig->قدر_سائز()+2) / 3 + 1;
      حرف *rbuf = static_cast<حرف *>(need_نتیجہ_buf(hwid, RBUF_VAL));
      طویل ssize = (علامتی)sig->قدر_سائز();
      طویل انتہا = بنیاد + (علامتی)wid;

	/* Do we have an end outside of the real signal vector. */
      اگر (بنیاد < 0 || انتہا > ssize) {
	    منطقی all_x = سچ;
	    اگر (انتہا > ssize) انتہا = ssize;
	    اگر (بنیاد < 0) بنیاد = 0;
	    برائے (طویل idx = بنیاد ;  idx < انتہا ;  idx += 1) {
		  اگر (sig->value(idx) != BIT4_X) {
			all_x = جھوٹ;
			توڑو;
		  }
	    }

	    اگر (all_x) {
		  rbuf[0] = 'x';
	    } ورنہ {
		  rbuf[0] = 'X';
	    }
	    rbuf[1] = 0;

	    vp->value.str = rbuf;
	    واپس;
      }

      vvp_vector4_t vec4;
      اگر (بنیاد == 0 && انتہا == ssize) {
	    sig->vec4_قدر(vec4);
      } ورنہ {
	    vvp_vector4_t tmp;
	    sig->vec4_قدر(tmp);
	    vec4 = tmp.subvalue(بنیاد, wid);
      }

      vpip_vec4_مقصد_dec_str(vec4, rbuf, hwid, signed_flag);

      vp->value.str = rbuf;
}

ساکن خالی format_vpiIntVal(ثابت vvp_signal_قدر*sig, صحیح بنیاد, غیرعلامتی wid,
                             صحیح signed_flag, s_vpi_قدر*vp)
{
      vvp_vector4_t tmp;
      sig->vec4_قدر(tmp);
      vvp_vector4_t sub = tmp.subvalue(بنیاد, wid);

	// Normally, we'd be OK with just using long in the call to
	// vector4_to_value, but some compilers seem to take long as
	// distinct from int32_t AND int64_t. Since the condition is
	// constant, the compiler should eliminate the dead code.
      اگر (حجم(vp->value.integer) == حجم(int32_t)) {
	    int32_t val = 0;
	    vector4_مقصد_قدر(sub, val, signed_flag, جھوٹ);
	    vp->value.integer = val;
      } ورنہ {
	    أكد(حجم(vp->value.integer) == حجم(int64_t));
	    int64_t val = 0;
	    vector4_مقصد_قدر(sub, val, signed_flag, جھوٹ);
	    vp->value.integer = val;
      }
}

ساکن خالی format_vpiRealVal(ثابت vvp_signal_قدر*sig, صحیح بنیاد, غیرعلامتی wid,
                              صحیح signed_flag, s_vpi_قدر*vp)
{
      vvp_vector4_t vec4(wid);
      طویل ssize = (علامتی)sig->قدر_سائز();
      طویل انتہا = بنیاد + (علامتی)wid;
      اگر (انتہا > ssize) انتہا = ssize;

      برائے (طویل idx = (بنیاد < 0) ? 0 : بنیاد ;  idx < انتہا ;  idx += 1) {
	    vec4.سیٹ_ثنائی_ہندسہ(idx-بنیاد, sig->value(idx));
      }

      vp->value.real = 0.0;
      vector4_مقصد_قدر(vec4, vp->value.real, signed_flag);
}

ساکن خالی format_vpiStringVal(ثابت vvp_signal_قدر*sig, صحیح بنیاد, غیرعلامتی wid,
                                s_vpi_قدر*vp)
{
      /* The result will use a character for each 8 bits of the
	 vector. Add one extra character for the highest bits that
	 don't form an 8 bit group. */
      حرف *rbuf = static_cast<حرف *>(need_نتیجہ_buf(wid/8 + ((wid&7)!=0) + 1, RBUF_VAL));
      حرف *cp = rbuf;

      حرف tmp = 0;
      برائے (طویل idx = بنیاد+(علامتی)wid-1; idx >= بنیاد; idx -= 1) {
	    tmp <<= 1;

	    اگر (idx >=0 && idx < (علامتی)sig->قدر_سائز() &&
	        sig->value(idx) == BIT4_1) {
		   tmp |= 1;
	    }

	    اگر (((idx-بنیاد)&7)==0){
		  /* Skip leading nulls. */
		  اگر (tmp == 0 && cp == rbuf)
			جاری;

		  /* Nulls in the middle get turned into spaces. */
		  *cp++ = tmp ? tmp : ' ';
		  tmp = 0;
	    }
      }
      *cp++ = 0;

      vp->value.str = rbuf;
}

ساکن خالی format_vpiScalarVal(ثابت vvp_signal_قدر*sig, صحیح بنیاد,
                                s_vpi_قدر*vp)
{
      اگر (بنیاد >= 0 && بنیاد < (علامتی)sig->قدر_سائز()) {
	    انتخاب (sig->value(بنیاد)) {
		حالت BIT4_0:
		  vp->value.scalar = vpi0;
		  توڑو;
		حالت BIT4_1:
		  vp->value.scalar = vpi1;
		  توڑو;
		حالت BIT4_X: {
		  vvp_scalar_t strn = sig->scalar_قدر(بنیاد);
		  اگر (strn.strength0() == 1) vp->value.scalar = vpiH;
		  ورنہ اگر (strn.strength1() == 1) vp->value.scalar = vpiL;
		  ورنہ vp->value.scalar = vpiX;
		  توڑو;
		}
		حالت BIT4_Z:
		  vp->value.scalar = vpiZ;
		  توڑو;
	    }
      } ورنہ {
	    vp->value.scalar = vpiX;
      }
}

ساکن خالی format_vpiStrengthVal(ثابت vvp_signal_قدر*sig, صحیح بنیاد,
                                  غیرعلامتی wid, s_vpi_قدر*vp)
{
      طویل انتہا = بنیاد + (علامتی)wid;
      s_vpi_strengthval*op;

      op = static_cast<s_vpi_strengthval*>
           (need_نتیجہ_buf(wid * حجم(s_vpi_strengthval), RBUF_VAL));

      برائے (طویل idx = بنیاد ;  idx < انتہا ;  idx += 1) {
	    اگر (idx >=0 && idx < (علامتی)sig->قدر_سائز()) {
		  vvp_scalar_t val = sig->scalar_قدر(idx);

		  /* vvp_scalar_t strengths are 0-7, but the vpi strength
		     is bit0-bit7. This gets the vpi form of the strengths
		     from the vvp_scalar_t strengths. */
		  غیرعلامتی s0 = 1 << val.strength0();
		  غیرعلامتی s1 = 1 << val.strength1();

		  انتخاب (val.value()) {
		      حالت BIT4_0:
			op[idx-بنیاد].logic = vpi0;
			op[idx-بنیاد].s0 = s0|s1;
			op[idx-بنیاد].s1 = 0;
			توڑو;

		      حالت BIT4_1:
			op[idx-بنیاد].logic = vpi1;
			op[idx-بنیاد].s0 = 0;
			op[idx-بنیاد].s1 = s0|s1;
			توڑو;

		      حالت BIT4_X:
			op[idx-بنیاد].logic = vpiX;
			op[idx-بنیاد].s0 = s0;
			op[idx-بنیاد].s1 = s1;
			توڑو;

		      حالت BIT4_Z:
			op[idx-بنیاد].logic = vpiZ;
			op[idx-بنیاد].s0 = vpiHiZ;
			op[idx-بنیاد].s1 = vpiHiZ;
			توڑو;
		  }
	    } ورنہ {
		  op[idx-بنیاد].logic = vpiX;
		  op[idx-بنیاد].s0 = vpiStrongDrive;
		  op[idx-بنیاد].s1 = vpiStrongDrive;
	    }
      }

      vp->value.strength = op;
}

ساکن خالی format_vpi_سمتیہ_Val(ثابت vvp_signal_قدر*sig, صحیح بنیاد, غیرعلامتی wid,
                                s_vpi_قدر*vp)
{
      طویل انتہا = بنیاد + (علامتی)wid;
      غیرعلامتی صحیح obit = 0;
      غیرعلامتی hwid = (wid + 31)/32;

      s_vpi_vecval *op = static_cast<p_vpi_vecval>
                         (need_نتیجہ_buf(hwid * حجم(s_vpi_vecval), RBUF_VAL));
      vp->value.vector = op;

      op->aval = op->bval = 0;
      برائے (طویل idx = بنیاد ;  idx < انتہا ;  idx += 1) {
	    اگر (بنیاد >= 0 && بنیاد < (علامتی)sig->قدر_سائز()) {
		انتخاب (sig->value(idx)) {
		حالت BIT4_0:
		  op->aval &= ~(1 << obit);
		  op->bval &= ~(1 << obit);
		  توڑو;
		حالت BIT4_1:
		  op->aval |= (1 << obit);
		  op->bval &= ~(1 << obit);
		  توڑو;
		حالت BIT4_X:
		  op->aval |= (1 << obit);
		  op->bval |= (1 << obit);
		  توڑو;
		حالت BIT4_Z:
		  op->aval &= ~(1 << obit);
		  op->bval |= (1 << obit);
		  توڑو;
		}
	    } ورنہ {  /* BIT4_X */
		  op->aval |= (1 << obit);
		  op->bval |= (1 << obit);
	    }

	    obit++;
	    اگر (!(obit % 32)) {
		  op += 1;
		  اگر ((op - vp->value.vector) < (ptrdiff_t)hwid)
			op->aval = op->bval = 0;
		  obit = 0;
	    }
      }
}

/*
 * implement vpi_get for vpiReg objects.
 */
ساکن صحیح signal_get(صحیح code, vpiHandle ref)
{
      ساخت __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      أكد(rfp);

      انتخاب (code) {
	  حالت vpi_سطر_No:
	    واپس 0;  // Not implemented for now!

	  حالت vpiSigned:
	    واپس rfp->signed_flag != 0;

	  حالت vpiArray:
	    واپس rfp->is_netarray != 0;

	  حالت vpi_اشاریہ_2: // This only works while we have a single index.
	    اگر (rfp->is_netarray) {
		  s_vpi_قدر vp;
		  vp.format = vpiIntVal;
		  vpi_get_قدر(rfp->id.اشاریہ, &vp);
		  واپس vp.value.integer;
	    } ورنہ {
		  واپس vpiUndefined;
	    }

	  حالت vpi_سائز:
	    واپس rfp->width();

	  حالت vpiNetType:
	    اگر (ref->get_type_رمز()==vpiNet)
		  واپس vpiWire;
	    ورنہ
		  واپس vpiUndefined;

	  حالت vpi_بایاں_Range:
            واپس rfp->msb.get_قدر();

	  حالت vpi_دایاں_Range:
            واپس rfp->lsb.get_قدر();

	  حالت vpiScalar:
	    واپس (rfp->msb.get_قدر() == 0 && rfp->lsb.get_قدر() == 0);
	  حالت vpi_سمتیہ:
	    واپس (rfp->msb.get_قدر() != rfp->lsb.get_قدر());

          حالت vpiAutomatic:
            واپس vpip_scope(rfp)->is_automatic() ? 1  : 0;

#اگر_تعریف_شدہ BR916_STOPGAP_FIX
          حالت _vpiFromThr:
            واپس _vpiNoThr;
#اگر_ختم

	  طےشدہ:
	    fprintf(stderr, "VPI error: unknown signal_get property %d.\n",
	            code);
	    واپس vpiUndefined;
      }
}

/*
 * implement vpi_get64 for vpiReg objects.
 */
ساکن int64_t signal_get64(صحیح code, vpiHandle ref)
{
      ساخت __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      أكد(rfp);

      انتخاب (code) {
	    // This private property must return zero when undefined.
	  حالت _vpiNexusId:
	    اگر (rfp->msb.get_قدر() == rfp->lsb.get_قدر())
		  واپس reinterpret_cast<int64_t>(rfp->گرہ);
	    واپس 0;

	  طےشدہ:
	    fprintf(stderr, "VPI error: unknown signal_get64 property %d.\n",
	            code);
	    واپس vpiUndefined;
      }
}

ساکن حرف* signal_get_str(صحیح code, vpiHandle ref)
{
      ساخت __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      أكد(rfp);

      اگر (code == vpiFile) {  // Not implemented for now!
	    واپس simple_سیٹ_rbuf_str(file_نام[0]);
      }

      اگر ((code != vpiName) && (code != vpi_بھرا_Name)) واپس تہی;

      حرف *nm;
      ثابت حرف *ixs;
      اگر (rfp->is_netarray) {
	    nm = strdup(vpi_get_str(vpiName, rfp->within.والد));
	    s_vpi_قدر vp;
	    vp.format = vpiDecStrVal;
	    vpi_get_قدر(rfp->id.اشاریہ, &vp);
	    ixs = vp.value.str;  /* do I need to strdup() this? */
      } ورنہ {
	    nm = strdup(rfp->id.name);
	    ixs = تہی;
      }

	/* The scope information is added here for vpiFullName. */
      حرف *rbuf = generic_get_str(code, vpip_scope(rfp), nm, ixs);
      آزاد(nm);
      واپس rbuf;
}

ساکن vpiHandle signal_get_handle(صحیح code, vpiHandle ref)
{
      ساخت __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      أكد(rfp);

      انتخاب (code) {

	  حالت vpi_والد:
	    واپس rfp->is_netarray? rfp->within.والد : تہی;

	  حالت vpi_اشاریہ_2:
	    واپس rfp->is_netarray? rfp->id.اشاریہ : تہی;

	  حالت vpi_بایاں_Range:
	    واپس &rfp->msb;
	  حالت vpi_دایاں_Range:
	    واپس &rfp->lsb;

	  حالت vpiScope:
	    واپس vpip_scope(rfp);

	  حالت vpiModule:
	    واپس vpip_module(vpip_scope(rfp));
      }

      واپس 0;
}

ساکن vpiHandle signal_iterate(صحیح code, vpiHandle ref)
{
      ساخت __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      أكد(rfp);

      اگر (code == vpi_اشاریہ_2) {
	    واپس rfp->is_netarray ? rfp->id.اشاریہ->vpi_iterate(code) : تہی;
      }

      واپس 0;
}

خالی __vpiSignal::make_بٹ()
{
      أكد(بٹ == تہی);
      بٹ = نیا ساخت __vpiBit[width() + 1];

	// Make word[-1] point to the parent.
      بٹ->والد = یہ;
	// Now point to word[0].
      بٹ += 1;

	// Update each bit to point to the base
      ساخت __vpiBit*بنیاد = بٹ;
      برائے (غیرعلامتی idx = 0; idx < width(); idx += 1) {
	    بنیاد[idx].bit0 = بنیاد;
	    صحیح real_idx;
	    اگر (msb.get_قدر() >= lsb.get_قدر()) {
		  real_idx = idx + lsb.get_قدر();
	    } ورنہ {
		  real_idx = lsb.get_قدر() - idx;
	    }
	    بنیاد[idx].اشاریہ = نیا __vpiDecConst(real_idx);
      }
}

vpiHandle __vpiSignal::get_اشاریہ(صحیح idx)
{
	/* Check to see if the index is in range. */
      اگر (msb.get_قدر() >= lsb.get_قدر()) {
	    اگر ((idx > msb.get_قدر()) || (idx < lsb.get_قدر())) واپس 0;
      } ورنہ {
	    اگر ((idx < msb.get_قدر()) || (idx > lsb.get_قدر())) واپس 0;
      }

	/* Normalize the index */
      غیرعلامتی norm_idx;
      اگر (msb.get_قدر() >= lsb.get_قدر()) {
	    norm_idx = idx - lsb.get_قدر();
      } ورنہ {
	    norm_idx = lsb.get_قدر() - idx;
      }

      اگر (بٹ == تہی) make_بٹ();

      واپس &(بٹ[norm_idx].as_ثنائی_ہندسہ);
}

خالی __vpiSignal::get_ثنائی_ہندسہ_قدر(ثابت ساخت __vpiBit*ثنائی_ہندسہ, p_vpi_قدر vp)
{
      غیرعلامتی اشاریہ = ثنائی_ہندسہ->get_norm_اشاریہ();

      vvp_signal_قدر*vsig = dynamic_cast<vvp_signal_قدر*>(گرہ->fil);
      أكد(vsig);

      اگر (vp->format == vpiObjTypeVal) {
	    vp->format = vpiIntVal;
      }

      انتخاب (vp->format) {
	حالت vpiBinStrVal:
            format_vpiBinStrVal(vsig, اشاریہ, 1, vp);
	    توڑو;

	حالت vpiOctStrVal:
            format_vpiOctStrVal(vsig, اشاریہ, 1, vp);
	    توڑو;

	حالت vpiDecStrVal:
            format_vpiDecStrVal(vsig, اشاریہ, 1, جھوٹ, vp);
	    توڑو;

	حالت vpiHexStrVal:
            format_vpiHexStrVal(vsig, اشاریہ, 1, vp);
	    توڑو;

	حالت vpiStringVal:
            format_vpiStringVal(vsig, اشاریہ, 1, vp);
	    توڑو;

	حالت vpiIntVal:
            format_vpiIntVal(vsig, اشاریہ, 1, جھوٹ, vp);
	    توڑو;

	حالت vpiRealVal:
            format_vpiRealVal(vsig, اشاریہ, 1, جھوٹ, vp);
	    توڑو;

	حالت vpiScalarVal:
            format_vpiScalarVal(vsig, اشاریہ, vp);
	    توڑو;

        حالت vpiStrengthVal:
            format_vpiStrengthVal(vsig, اشاریہ, 1, vp);
            توڑو;

	حالت vpi_سمتیہ_Val:
            format_vpi_سمتیہ_Val(vsig, اشاریہ, 1, vp);
	    توڑو;

	طےشدہ:
	    fprintf(stderr, "internal error: get_value() "
		    "type %d is not implemented for bit "
		    "select %s[%d] in scope %s.\n",
		    (صحیح)vp->format, vpi_get_str(vpiName),
		    ثنائی_ہندسہ->get_اشاریہ(),
	            vpip_scope(یہ)->scope_name());
	    أكد(0);
      };
}

vpiHandle __vpiSignal::put_ثنائی_ہندسہ_قدر(ساخت __vpiBit*ثنائی_ہندسہ, p_vpi_قدر vp, صحیح flags)
{
      غیرعلامتی اشاریہ = ثنائی_ہندسہ->get_norm_اشاریہ();
      vvp_net_ptr_t dest(گرہ, 0);
      vvp_vector4_t val = vec4_from_vpi_قدر(vp, 1);

      اگر ((flags == vpiForceFlag) || (flags == vpiReleaseFlag)) {
	    fprintf(stderr, "Sorry: vpi_put_value() for %s does not "
	                    "currently support force/release.\n",
	                    ثنائی_ہندسہ->as_ثنائی_ہندسہ.vpi_get_str(vpi_بھرا_Name));
	    واپس تہی;
      }

      اگر ((get_type_رمز() == vpiNet) &&
          !dynamic_cast<vvp_island_پورٹ*>(گرہ->fun)) {
	    گرہ->send_vec4_pv(val, اشاریہ, width(),
	                            vthread_get_wt_context());
      } ورنہ {
	    vvp_send_vec4_pv(dest, val, اشاریہ, width(),
	                     vthread_get_wt_context());
      }

	// This is not a scheduled event so there is no event to return
      واپس تہی;
}

ساکن vpiHandle signal_اشاریہ(صحیح idx, vpiHandle ref)
{
      ساخت __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      أكد(rfp);

	/* We can only get the bit for a net or reg. */
      PLI_INT32 type = vpi_get(vpiType, ref);
      اگر ((type != vpiNet) && (type != vpiReg)) واپس 0;

      واپس rfp->get_اشاریہ(idx);
}

غیرعلامتی __vpiSignal::width(خالی) ثابت
{
      غیرعلامتی wid = (msb.get_قدر() >= lsb.get_قدر())
	    ? (msb.get_قدر() - lsb.get_قدر() + 1)
	    : (lsb.get_قدر() - msb.get_قدر() + 1);

      واپس wid;
}

/*
 * The get_value method reads the values of the functors and returns
 * the vector to the caller. This causes no side-effect, and reads the
 * variables like a %load would.
 */
ساکن خالی signal_get_قدر(vpiHandle ref, s_vpi_قدر*vp)
{
      ساخت __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      أكد(rfp);

      غیرعلامتی wid = rfp->width();

      vvp_signal_قدر*vsig = dynamic_cast<vvp_signal_قدر*>(rfp->گرہ->fil);
      أكد(vsig);

      انتخاب (vp->format) {

	  حالت vpiIntVal:
	    format_vpiIntVal(vsig, 0, wid, rfp->signed_flag, vp);
	    توڑو;

	  حالت vpiScalarVal:
	    format_vpiScalarVal(vsig, 0, vp);
	    توڑو;

	  حالت vpiStrengthVal:
	    format_vpiStrengthVal(vsig, 0, wid, vp);
	    توڑو;

	  حالت vpiBinStrVal:
	    format_vpiBinStrVal(vsig, 0, wid, vp);
	    توڑو;

	  حالت vpiHexStrVal:
	    format_vpiHexStrVal(vsig, 0, wid, vp);
	    توڑو;

	  حالت vpiOctStrVal:
	    format_vpiOctStrVal(vsig, 0, wid, vp);
	    توڑو;

	  حالت vpiDecStrVal:
	    format_vpiDecStrVal(vsig, 0, wid, rfp->signed_flag, vp);
	    توڑو;

	  حالت vpiStringVal:
	    format_vpiStringVal(vsig, 0, wid, vp);
	    توڑو;

	  حالت vpi_سمتیہ_Val:
	    format_vpi_سمتیہ_Val(vsig, 0, wid, vp);
	    توڑو;

	  حالت vpiRealVal:
	    format_vpiRealVal(vsig, 0, wid, rfp->signed_flag, vp);
	    توڑو;

	  حالت vpiObjTypeVal:
	    اگر (wid == 1) {
		  vp->format = vpiScalarVal;
		  format_vpiScalarVal(vsig, 0, vp);
	    } ورنہ {
		  vp->format = vpi_سمتیہ_Val;
		  format_vpi_سمتیہ_Val(vsig, 0, wid, vp);
	    }
	    توڑو;

	  طےشدہ:
	    fprintf(stderr, "vvp internal error: get_value: "
		    "value type %d not implemented."
		    " Signal is %s in scope %s\n",
		    (صحیح)vp->format, vpi_get_str(vpiName, ref),
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

ساکن vvp_vector4_t from_stringval(ثابت حرف*str, غیرعلامتی wid)
{
      غیرعلامتی idx;
      ثابت حرف*cp;

      cp = str + لڑی_لمبائی(str);
      idx = 0;

      vvp_vector4_t val(wid, BIT4_0);

      جب ((idx < wid) && (cp > str)) {
	    غیرعلامتی byte = *--cp;
	    صحیح ثنائی_ہندسہ;

	    برائے (ثنائی_ہندسہ = 0 ;  (ثنائی_ہندسہ < 8) && (idx < wid) ;  ثنائی_ہندسہ += 1) {
		  اگر (byte & 1)
			val.سیٹ_ثنائی_ہندسہ(idx, BIT4_1);

		  byte >>= 1;
		  idx += 1;
	    }
      }

      واپس val;
}

ساکن vpiHandle signal_put_قدر(vpiHandle ref, s_vpi_قدر*vp, صحیح flags)
{
      غیرعلامتی wid;
      ساخت __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      أكد(rfp);
      vvp_net_ptr_t dest(rfp->گرہ, 0);

      منطقی net_flag = ref->get_type_رمز()==vpiNet;

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value. Note that
	   if this net is attached to an island, we need to rerun
	   the calculations immediately so we can return the
	   released value. */
      اگر (flags == vpiReleaseFlag) {
	    أكد(rfp->گرہ->fil);
	    rfp->گرہ->fil->force_unlink();
	    rfp->گرہ->fil->release(dest, net_flag);
	    rfp->گرہ->fun->force_flag(سچ);
	    signal_get_قدر(ref, vp);
	    واپس ref;
      }

	/* Make a vvp_vector4_t vector to receive the translated value
	   that we are going to poke. This will get populated
	   differently depending on the format. */
      wid = (rfp->msb.get_قدر() >= rfp->lsb.get_قدر())
	    ? (rfp->msb.get_قدر() - rfp->lsb.get_قدر() + 1)
	    : (rfp->lsb.get_قدر() - rfp->msb.get_قدر() + 1);

      vvp_vector4_t val = vec4_from_vpi_قدر(vp, wid);

      اگر (flags == vpiForceFlag) {
	    vvp_vector2_t نقاب (vvp_vector2_t::FILL1, wid);
	    rfp->گرہ->force_vec4(val, نقاب);
      } ورنہ اگر (net_flag && !dynamic_cast<vvp_island_پورٹ*>(rfp->گرہ->fun)) {
	    rfp->گرہ->send_vec4(val, vthread_get_wt_context());
      } ورنہ {
	    vvp_send_vec4(dest, val, vthread_get_wt_context());
      }
      واپس ref;
}

vvp_vector4_t vec4_from_vpi_قدر(s_vpi_قدر*vp, غیرعلامتی wid)
{
      vvp_vector4_t val (wid, BIT4_0);

      انتخاب (vp->format) {

	  حالت vpiIntVal: {
		طویل vpi_val = vp->value.integer;
		برائے (غیرعلامتی idx = 0 ;  idx < wid ;  idx += 1) {
		      vvp_bit4_t ثنائی_ہندسہ = (vpi_val & 1) ? BIT4_1 : BIT4_0;
		      val.سیٹ_ثنائی_ہندسہ(idx, ثنائی_ہندسہ);
		      vpi_val >>= 1;
		}
		توڑو;
	  }

	  حالت vpi_سمتیہ_Val:
	    برائے (غیرعلامتی idx = 0 ;  idx < wid ;  idx += 1) {
		  غیرعلامتی طویل aval = vp->value.vector[idx/32].aval;
		  غیرعلامتی طویل bval = vp->value.vector[idx/32].bval;
		  aval >>= idx%32;
		  bval >>= idx%32;
		  صحیح bitmask = (aval&1) | ((bval<<1)&2);
		  vvp_bit4_t ثنائی_ہندسہ = scalar_مقصد_bit4(bitmask);
		  val.سیٹ_ثنائی_ہندسہ(idx, ثنائی_ہندسہ);
	    }
	    توڑو;
	  حالت vpiBinStrVal:
	    vpip_bin_str_مقصد_vec4(val, vp->value.str);
	    توڑو;
	  حالت vpiOctStrVal:
	    vpip_oct_str_مقصد_vec4(val, vp->value.str);
	    توڑو;
	  حالت vpiDecStrVal:
	    vpip_dec_str_مقصد_vec4(val, vp->value.str);
	    توڑو;
	  حالت vpiHexStrVal:
	    vpip_hex_str_مقصد_vec4(val, vp->value.str);
	    توڑو;
	  حالت vpiScalarVal:
	    val.سیٹ_ثنائی_ہندسہ(0, scalar_مقصد_bit4(vp->value.scalar));
	    توڑو;
	  حالت vpiStringVal:
	    val = from_stringval(vp->value.str, wid);
	    توڑو;
	  حالت vpiRealVal:
	    val = vvp_vector4_t(wid, vp->value.real);
	    توڑو;

	  طےشدہ:
	    fprintf(stderr, "vvp internal error: put_value: "
		    "value type %d not implemented here.\n",
		    (صحیح)vp->format);
	    أكد(0);

      }

      واپس val;
}

صحیح __vpiSignal::vpi_get(صحیح code)
{ واپس signal_get(code, یہ); }

int64_t __vpiSignal::vpi_get64(صحیح code)
{ واپس signal_get64(code, یہ); }

حرف* __vpiSignal::vpi_get_str(صحیح code)
{ واپس signal_get_str(code, یہ); }

خالی __vpiSignal::vpi_get_قدر(p_vpi_قدر val)
{ signal_get_قدر(یہ, val); }

vpiHandle __vpiSignal::vpi_put_قدر(p_vpi_قدر val, صحیح flags)
{ واپس signal_put_قدر(یہ, val, flags); }

vpiHandle __vpiSignal::vpi_handle(صحیح code)
{ واپس signal_get_handle(code, یہ); }

vpiHandle __vpiSignal::vpi_iterate(صحیح code)
{ واپس signal_iterate(code, یہ); }

vpiHandle __vpiSignal::vpi_اشاریہ(صحیح idx)
{ واپس signal_اشاریہ(idx, یہ); }

ساخت signal_reg : عوامی __vpiSignal {
      اندرونی signal_reg() { }
      صحیح get_type_رمز(خالی) ثابت override { واپس vpiReg; }
};

ساخت signal_integer : عوامی __vpiSignal {
      اندرونی signal_integer() { }
      صحیح get_type_رمز(خالی) ثابت override { واپس vpiIntegerVar; }
};

ساخت signal_net : عوامی __vpiSignal {
      اندرونی signal_net() { }
      صحیح get_type_رمز(خالی) ثابت override { واپس vpiNet; }
};

ساخت signal_byte : عوامی __vpiSignal {
      اندرونی signal_byte() { }
      صحیح get_type_رمز(خالی) ثابت override { واپس vpiByteVar; }
};

ساخت signal_bitvar : عوامی __vpiSignal {
      اندرونی signal_bitvar() { }
      صحیح get_type_رمز(خالی) ثابت override { واپس vpi_ثنائی_ہندسہ_Var; }
};

ساخت signal_shortint : عوامی __vpiSignal {
      اندرونی signal_shortint() { }
      صحیح get_type_رمز(خالی) ثابت override { واپس vpiShortIntVar; }
};

ساخت signal_int : عوامی __vpiSignal {
      اندرونی signal_int() { }
      صحیح get_type_رمز(خالی) ثابت override { واپس vpiIntVar; }
};

ساخت signal_longint : عوامی __vpiSignal {
      اندرونی signal_longint() { }
      صحیح get_type_رمز(خالی) ثابت override { واپس vpiLongIntVar; }
};


/*
 * Construct a vpiIntegerVar object. Indicate the type using a flag
 * to minimize the code modifications. Icarus implements integers
 * as 'reg signed [31:0]'.
 */
vpiHandle vpip_make_int4(ثابت حرف*name, صحیح msb, صحیح lsb, vvp_net_t*vec)
{
      __vpiSignal*obj = نیا signal_integer;
      واپس fill_اندر_var4(obj, name, msb, lsb, سچ, vec);
}

/*
 * Construct the two-state SystemVerilog variables.
 */
vpiHandle vpip_make_int2(ثابت حرف*name, صحیح msb, صحیح lsb, منطقی signed_flag,
                         vvp_net_t*vec)
{
      __vpiSignal*obj;

	// All unsigned 2-state variables are a vpiBitVar. All 2-state
	// variables with a non-zero lsb are also a vpiBitVar.
      اگر ((! signed_flag) || (lsb != 0) ) {
	    obj = نیا signal_bitvar;
      } ورنہ {
	      // These could also be bit declarations with matching
	      // information, but for now they get the apparent type.
	    انتخاب (msb) {
		حالت 7:
		  obj = نیا signal_byte;
		  توڑو;
		حالت 15:
		  obj = نیا signal_shortint;
		  توڑو;
		حالت 31:
		  obj = نیا signal_int;
		  توڑو;
		حالت 63:
		  obj = نیا signal_longint;
		  توڑو;
		طےشدہ:
		    // Every other type of bit vector is a vpiBitVar with
		    // array dimensions.
		  obj = نیا signal_bitvar;
		  توڑو;
	    }
      }

      واپس fill_اندر_var4(obj, name, msb, lsb, signed_flag, vec);
}

/*
 * Construct a vpiReg/vpiLogicVar object. It's like a net, except for the type.
 */
vpiHandle vpip_make_var4(ثابت حرف*name, صحیح msb, صحیح lsb,
			منطقی signed_flag, vvp_net_t*vec)
{
      __vpiSignal*obj = نیا signal_reg;
      واپس fill_اندر_var4(obj, name, msb, lsb, signed_flag, vec);
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
ساکن ساخت vpiSignal_plug **signal_pool = 0;
ساکن غیرعلامتی signal_pool_گنتی = 0;
ساکن غیرعلامتی طویل signal_گنتی = 0;
ساکن غیرعلامتی طویل signal_dels = 0;
#اگر_ختم

ساخت vpiSignal_plug {
      غیرعلامتی حرف space[حجم (ساخت __vpiSignal)];
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
      ساخت vpiSignal_plug *pool;
#اگر_ختم
};

خالی* __vpiSignal::عامل نیا(size_t siz)
{
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
      أكد(siz == حجم(ساخت vpiSignal_plug) - حجم(ساخت vpiSignal_plug *));
#ورنہ
      أكد(siz == حجم(ساخت vpiSignal_plug));
#اگر_ختم
      ساکن ساخت vpiSignal_plug*alloc_array = 0;
      ساکن غیرعلامتی alloc_اشاریہ = 0;
      ثابت غیرعلامتی alloc_گنتی = 512;

      اگر ((alloc_array == 0) || (alloc_اشاریہ == alloc_گنتی)) {
	    alloc_array = static_cast<ساخت vpiSignal_plug*>
	                  (صفر_حافظہ_دو(alloc_گنتی, حجم(ساخت vpiSignal_plug)));
	    alloc_اشاریہ = 0;
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
	    VALGRIND_MAKE_MEM_NOACCESS(alloc_array, alloc_گنتی *
	                                            حجم(ساخت vpiSignal_plug));
	    VALGRIND_CREATE_MEMPOOL(alloc_array, 0, 1);
	    signal_pool_گنتی += 1;
	    signal_pool = static_cast<vpiSignal_plug **>
	                  (realloc(signal_pool,
	                   signal_pool_گنتی*حجم(vpiSignal_plug **)));
	    signal_pool[signal_pool_گنتی-1] = alloc_array;
#اگر_ختم
      }

      ساخت vpiSignal_plug*cur = alloc_array + alloc_اشاریہ;
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
      VALGRIND_MEMPOOL_ALLOC(alloc_array, cur, حجم(ساخت vpiSignal_plug));
      cur->pool = alloc_array;
      signal_گنتی += 1;
#اگر_ختم
      alloc_اشاریہ += 1;
      واپس cur;
}

خالی __vpiSignal::عامل حذف(خالی*)
{
      أكد(0);
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
خالی signal_delete(vpiHandle شے)
{
      ساخت __vpiSignal *obj = static_cast<__vpiSignal *> (شے);
      أكد(obj->گرہ->fil);
      obj->گرہ->fil->clear_all_callbacks();
      vvp_net_delete(obj->گرہ);
      اگر (obj->بٹ) {
	    برائے (غیرعلامتی idx=0; idx<obj->width(); idx+=1) {
		  حذف obj->بٹ[idx].اشاریہ;
	    }
	    obj->بٹ -= 1;
	    حذف [] obj->بٹ;
      }
      signal_dels += 1;
      VALGRIND_MEMPOOL_FREE(reinterpret_cast<vpiSignal_plug *>(obj)->pool, obj);
}

خالی signal_pool_delete()
{
      اگر (RUNNING_ON_VALGRIND && (signal_گنتی != signal_dels)) {
	    fflush(تہی);
	    VALGRIND_PRINTF("Error: vvp missed deleting %ld of %lu signal(s).",
	                    (طویل) signal_گنتی - signal_dels, signal_گنتی);
      }

      برائے (غیرعلامتی idx = 0; idx < signal_pool_گنتی; idx += 1) {
	    VALGRIND_DESTROY_MEMPOOL(signal_pool[idx]);
	    آزاد(signal_pool[idx]);
      }

      آزاد(signal_pool);
      signal_pool = 0;
      signal_pool_گنتی = 0;
}
#اگر_ختم

/*
 * Construct a vpiNet object. Give the object specified dimensions,
 * and point to the specified functor for the lsb.
 *
 * The name is the PLI name for the object. If it is an array it is
 * <name>[<index>].
 */
ساکن vpiHandle fill_اندر_net4(ساخت __vpiSignal*obj, __vpiScope*scope,
			      ثابت حرف*name, صحیح msb, صحیح lsb,
			      منطقی signed_flag, vvp_net_t*گرہ)
{
      obj->id.name = name? vpip_name_string(name) : تہی;
      obj->msb = __vpiDecConst(msb);
      obj->lsb = __vpiDecConst(lsb);
      obj->signed_flag = signed_flag? 1 : 0;
      obj->is_netarray = 0;
      obj->گرہ = گرہ;

	// Place this object within a scope. If this object is
	// attached to an array, then this value will be replaced with
	// the handle to the parent.
      obj->within.scope = scope;

      گنتی_vpi_nets += 1;

      واپس obj;
}

vpiHandle vpip_make_net4(__vpiScope*scope,
			 ثابت حرف*name, صحیح msb, صحیح lsb,
			 منطقی signed_flag, vvp_net_t*گرہ)
{
      ساخت __vpiSignal*obj = نیا signal_net;
      واپس fill_اندر_net4(obj, scope, name, msb, lsb, signed_flag, گرہ);
}

ساکن صحیح PV_get_بنیاد(ساخت __vpiPV*rfp)
{
	/* We return from the symbol base if it is defined. */
      اگر (rfp->sbase != 0) {
	    s_vpi_قدر val;
	      /* Check to see if the value is defined. */
	    val.format = vpi_سمتیہ_Val;
	    vpi_get_قدر(rfp->sbase, &val);
	    صحیح الفاظ = (vpi_get(vpi_سائز, rfp->sbase)-1)/32 + 1;
	    برائے(صحیح idx = 0; idx < الفاظ; idx += 1) {
		    /* Return INT_MIN to indicate an X base. */
		  اگر (val.value.vector[idx].bval != 0) واپس INT_MIN;
	    }
	      /* The value is defined so get and return it. */
	    val.format = vpiIntVal;
	    vpi_get_قدر(rfp->sbase, &val);
	    واپس val.value.integer;
      }

	/* If the width is zero then tbase is the constant. */
      واپس rfp->tbase;
}

ساکن صحیح PV_get(صحیح code, vpiHandle ref)
{
      ساخت __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      أكد(rfp);

      صحیح rval = 0;
      انتخاب (code) {
	حالت vpi_سطر_No:
	    واپس 0;  // Not implemented for now!

	حالت vpiSigned:
	    واپس 0;  // A part/bit select is always unsigned!

	حالت vpi_سائز:
	    واپس rfp->width;

	  /* This is like the &A<> in array.cc. */
	حالت vpiConstantSelect:
	    واپس rfp->sbase == 0;

	حالت vpi_بایاں_Range:
            rval += rfp->width - 1;
	    // fallthrough
	حالت vpi_دایاں_Range: {
	    صحیح بائیں = vpi_get(vpi_بایاں_Range, rfp->والد);
	    صحیح دائیں = vpi_get(vpi_دایاں_Range, rfp->والد);
	    rval += PV_get_بنیاد(rfp);
	    واپس بائیں >= دائیں ? دائیں + rval : دائیں - rval;
	}

        حالت vpiAutomatic:
            واپس vpi_get(vpiAutomatic, rfp->والد);

#اگر defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
        حالت _vpiFromThr:
            واپس _vpi_at_PV;
#اگر_ختم

	طےشدہ:
	    fprintf(stderr, "PV_get: property %d is unknown\n", code);
      }

      واپس 0;
}

ساکن حرف* PV_get_str(صحیح code, vpiHandle ref)
{
      ساخت __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      أكد(rfp);

      انتخاب (code) {
	حالت vpiFile:  // Not implemented for now!
	    واپس simple_سیٹ_rbuf_str(file_نام[0]);

	حالت vpiName:
	حالت vpi_بھرا_Name: {
	    ثابت حرف*nm = vpi_get_str(code, rfp->والد);
	    size_t len = 256+لڑی_لمبائی(nm);
	    حرف *بھرا = static_cast<حرف *>(حافظہ_دو(len));
	    snprintf(بھرا, len, "%s[%d:%d]", nm,
	                                     (صحیح)vpi_get(vpi_بایاں_Range, ref),
	                                     (صحیح)vpi_get(vpi_دایاں_Range, ref));
	    بھرا[len-1] = 0;
	    حرف *res = simple_سیٹ_rbuf_str(بھرا);
	    آزاد(بھرا);
	    واپس res;
	}

	طےشدہ:
	    fprintf(stderr, "PV_get_str: property %d is unknown.\n", code);
      }

      واپس 0;
}

ساکن خالی PV_get_قدر(vpiHandle ref, p_vpi_قدر vp)
{
      ساخت __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      أكد(rfp);

      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*>(rfp->net->fil);
      أكد(sig);

      انتخاب (vp->format) {

	  حالت vpiIntVal:
	    format_vpiIntVal(sig, PV_get_بنیاد(rfp), rfp->width, 0, vp);
	    توڑو;

	  حالت vpiBinStrVal:
	    format_vpiBinStrVal(sig, PV_get_بنیاد(rfp), rfp->width, vp);
	    توڑو;

	  حالت vpiOctStrVal:
	    format_vpiOctStrVal(sig, PV_get_بنیاد(rfp), rfp->width, vp);
	    توڑو;

	  حالت vpiHexStrVal:
	    format_vpiHexStrVal(sig, PV_get_بنیاد(rfp), rfp->width, vp);
	    توڑو;

	  حالت vpiDecStrVal:
	    format_vpiDecStrVal(sig, PV_get_بنیاد(rfp), rfp->width, 0, vp);
	    توڑو;

	  حالت vpiStringVal:
	    format_vpiStringVal(sig, PV_get_بنیاد(rfp), rfp->width, vp);
	    توڑو;

	  حالت vpiScalarVal:
	    format_vpiScalarVal(sig, PV_get_بنیاد(rfp), vp);
	    توڑو;

	  حالت vpiStrengthVal:
	    format_vpiStrengthVal(sig, PV_get_بنیاد(rfp), rfp->width, vp);
	    توڑو;

	  حالت vpi_سمتیہ_Val:
	    format_vpi_سمتیہ_Val(sig, PV_get_بنیاد(rfp), rfp->width, vp);
	    توڑو;

	  حالت vpiRealVal:
	    format_vpiRealVal(sig, PV_get_بنیاد(rfp), rfp->width, 0, vp);
	    توڑو;

	  طےشدہ:
	    fprintf(stderr, "vvp internal error: PV_get_value: "
		    "value type %d not implemented. Signal is %s.\n",
		    (صحیح)vp->format, vpi_get_str(vpi_بھرا_Name, rfp->والد));
	    أكد(0);
      }
}

ساکن vpiHandle PV_put_قدر(vpiHandle ref, p_vpi_قدر vp, صحیح flags)
{
      ساخت __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      أكد(rfp);
      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*>(rfp->net->fil);
      أكد(sig);

      غیرعلامتی sig_سائز = sig->قدر_سائز();
      غیرعلامتی width = rfp->width;
      صحیح بنیاد = PV_get_بنیاد(rfp);
      اگر (بنیاد >= (علامتی) sig_سائز) واپس 0;
      اگر (بنیاد + (علامتی) width < 0) واپس 0;

      vvp_vector4_t val;
      اگر (flags != vpiReleaseFlag) {
	    val = vec4_from_vpi_قدر(vp, width);
      }

	/*
	 * If the base is less than zero then trim off any unneeded
	 * lower bits.
	 */
      اگر (بنیاد < 0) {
	    width += بنیاد;
	    اگر (flags != vpiReleaseFlag) {
		  val = val.subvalue(-بنیاد, width);
	    }
	    بنیاد = 0;
      }

	/*
	 * If the value is wider than the signal then trim off any
	 * unneeded upper bits.
	 */
      اگر (بنیاد+width > sig_سائز) {
	    width = sig_سائز - بنیاد;
	    اگر (flags != vpiReleaseFlag) {
		  val = val.subvalue(0, width);
	    }
      }

      أكد(rfp->والد);
      منطقی net_flag = rfp->والد->get_type_رمز()==vpiNet;
      منطقی بھرا_sig = بنیاد == 0 && width == sig_سائز;

      vvp_net_ptr_t dest(rfp->net, 0);

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value.  Note that
	   if this net is attached to an island, we need to rerun
	   the calculations immediately so we can return the
	   released value.*/
      اگر (flags == vpiReleaseFlag) {
	    أكد(rfp->net->fil);
	      // XXXX Can't really do this if this is a partial release?
	    rfp->net->fil->force_unlink();
	    اگر (بھرا_sig) {
		  rfp->net->fil->release(dest, net_flag);
	    } ورنہ {
		  rfp->net->fil->release_pv(dest, بنیاد, width, net_flag);
	    }
	    rfp->net->fun->force_flag(سچ);
	    PV_get_قدر(ref, vp);
	    واپس ref;
      }

      اگر (flags == vpiForceFlag) {
	    اگر (بھرا_sig) {
		  vvp_vector2_t نقاب (vvp_vector2_t::FILL1, sig_سائز);
		  rfp->net->force_vec4(val, نقاب);
	    } ورنہ {
		  vvp_vector2_t نقاب (vvp_vector2_t::FILL0, sig_سائز);
		  برائے (غیرعلامتی idx = 0 ; idx < width ; idx += 1)
		        نقاب.سیٹ_ثنائی_ہندسہ(بنیاد+idx, 1);

		  vvp_vector4_t tmp (sig_سائز, BIT4_Z);

		    // vvp_net_t::force_vec4 propagates all the bits of the
		    // forced vector value, regardless of the mask. This
		    // ensures the unforced bits retain their current value.
		  sig->vec4_قدر(tmp);

		  tmp.سیٹ_vec(بنیاد, val);
		  rfp->net->force_vec4(tmp, نقاب);
	    }
      } ورنہ اگر (net_flag && !dynamic_cast<vvp_island_پورٹ*>(rfp->net->fun)) {
	    اگر (بھرا_sig) {
		  rfp->net->send_vec4(val, vthread_get_wt_context());
	    } ورنہ {
		  rfp->net->send_vec4_pv(val, بنیاد, sig_سائز,
		                         vthread_get_wt_context());
	    }
      } ورنہ {
	    اگر (بھرا_sig) {
		  vvp_send_vec4(dest, val, vthread_get_wt_context());
	    } ورنہ {
		  vvp_send_vec4_pv(dest, val, بنیاد, sig_سائز,
	                           vthread_get_wt_context());
	    }
      }
      واپس 0;
}

ساکن vpiHandle PV_get_handle(صحیح code, vpiHandle ref)
{
      ساخت __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      أكد(rfp);

      انتخاب (code) {
	  حالت vpi_والد:
	    واپس rfp->والد;

	  حالت vpiScope:
	    واپس vpi_handle(vpiScope, rfp->والد);

	  حالت vpiModule:
	    واپس vpi_handle(vpiModule, rfp->والد);
      }

      واپس 0;
}

اندرونی __vpiPV::__vpiPV()
{ }

صحیح __vpiPV::get_type_رمز(خالی) ثابت
{ واپس vpiPartSelect; }

صحیح __vpiPV::vpi_get(صحیح code)
{ واپس PV_get(code, یہ); }

حرف* __vpiPV::vpi_get_str(صحیح code)
{ واپس PV_get_str(code, یہ); }

خالی __vpiPV::vpi_get_قدر(p_vpi_قدر val)
{ PV_get_قدر(یہ, val); }

vpiHandle __vpiPV::vpi_put_قدر(p_vpi_قدر val, صحیح flags)
{ واپس PV_put_قدر(یہ, val, flags); }

vpiHandle __vpiPV::vpi_handle(صحیح code)
{ واپس PV_get_handle(code, یہ); }

vpiHandle vpip_make_PV(حرف*var, صحیح بنیاد, صحیح width)
{
      ساخت __vpiPV*obj = نیا __vpiPV;
      compile_vpi_lookup(&obj->والد, strdup(var));
      obj->sbase = 0;
      obj->tbase = بنیاد;
      obj->width = (غیرعلامتی) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      واپس obj;
}

vpiHandle vpip_make_PV(حرف*var, حرف*علامت, صحیح width)
{
      ساخت __vpiPV*obj = نیا __vpiPV;
      compile_vpi_lookup(&obj->والد, strdup(var));
      compile_vpi_lookup(&obj->sbase, علامت);
      obj->tbase = 0;
      obj->width = (غیرعلامتی) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      واپس obj;
}

vpiHandle vpip_make_PV(حرف*var, vpiHandle handle, صحیح width)
{
      ساخت __vpiPV*obj = نیا __vpiPV;
      compile_vpi_lookup(&obj->والد, strdup(var));
      obj->sbase = handle;
      obj->tbase = 0;
      obj->width = (غیرعلامتی) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      واپس obj;
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
خالی PV_delete(vpiHandle شے)
{
      ساخت __vpiPV *obj = dynamic_cast<__vpiPV*>(شے);
      اگر (obj->sbase) {
	    انتخاب (obj->sbase->get_type_رمز()) {
		حالت vpi_حافظہ_لفظ:
		  اگر (vpi_get(_vpiFromThr, obj->sbase) == _vpi_at_A) {
			A_delete(obj->sbase);
		  }
		  توڑو;
		حالت vpiPartSelect:
		  أكد(vpi_get(_vpiFromThr, obj->sbase) == _vpi_at_PV);
		  PV_delete(obj->sbase);
		  توڑو;
	    }
      }
      أكد(obj->net->fil);
      obj->net->fil->clear_all_callbacks();
      حذف obj;
}
#اگر_ختم
