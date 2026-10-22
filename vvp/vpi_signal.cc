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
خارجي ثابت حرف hex_digits[256];
خارجي ثابت حرف oct_digits[64];

/*
 * The string values need a result buf to hold the results. This
 * buffer can be reused for that purpose. Whenever I have a need, the
 * need_result_buf function makes sure that need can be met.
 */
فراغ *need_result_buf(unsigned cnt, vpi_rbuf_t type)
{
      ساكن فراغ*result_buf[2] = {0, 0};
      ساكن size_t result_buf_size[2] = {0, 0};

      إذا (type == RBUF_DEL) {
	    حرر(result_buf[RBUF_VAL]);
	    result_buf[RBUF_VAL] = 0;
	    result_buf_size[RBUF_VAL] = 0;

	    حرر(result_buf[RBUF_STR]);
	    result_buf[RBUF_STR] = 0;
	    result_buf_size[RBUF_STR] = 0;

	    أرجع 0;
      }

      cnt = (cnt + 0x0fff) & ~0x0fff;

      إذا (result_buf_size[type] == 0) {
	    result_buf[type] = malloc(cnt);
	    result_buf_size[type] = cnt;
      } وإلا إذا (result_buf_size[type] < cnt) {
	    result_buf[type] = realloc(result_buf[type], cnt);
	    result_buf_size[type] = cnt;
      }

      أرجع result_buf[type];
}

حرف *simple_set_rbuf_str(ثابت حرف *s1)
{
      حرف *res = static_cast<حرف *>(need_result_buf(strlen(s1)+1, RBUF_STR));
      إذا (res) strcpy(res,s1);
      أرجع res;
}

حرف *generic_get_str(صحيح code, vpiHandle ref, ثابت حرف *name, ثابت حرف *index)
{
      size_t len = strlen(name) + 1;  /* include space for null termination */
      حرف *bn = عدم;
      منطقي is_bn_a_pkg = خطأ;
      إذا (code == vpiFullName) {
	    bn = strdup(vpi_get_str(code,ref));
	    size_t bn_len = strlen(bn);
	    is_bn_a_pkg = (bn_len >= 3) && (bn[bn_len-1] == ':') && (bn[bn_len-2] == ':');
	    len += bn_len;
	    إذا (! is_bn_a_pkg) len += 1; // include space for "." separator
      }
      إذا (index != عدم) len += strlen(index) + 2;  /* include space for brackets */

      حرف *res = static_cast<حرف *>(need_result_buf(len, RBUF_STR));
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
      إذا (index != عدم) {
	    strcat(res, "[");
	    strcat(res, index);
	    strcat(res, "]");
      }
      أرجع res;
}

ساكن vpiHandle fill_in_net4(بنية __vpiSignal*obj, __vpiScope*scope,
                              ثابت حرف*name, صحيح msb, صحيح lsb,
                              منطقي signed_flag, vvp_net_t*كلي);

ساكن vpiHandle fill_in_var4(بنية __vpiSignal*obj,
                              ثابت حرف*name, صحيح msb, صحيح lsb,
                              منطقي signed_flag, vvp_net_t*كلي)
{
	// Variable declarations are always resolved immediately,
	// so we can assume they belong in the current scope.
      أرجع fill_in_net4(obj, vpip_peek_current_scope(),
			  name, msb, lsb, signed_flag, كلي);
}

/*
 * The standard formatting/conversion routines.
 * They work with full or partial signals.
 */

ساكن فراغ format_vpiBinStrVal(ثابت vvp_signal_درج*sig, صحيح base, unsigned wid,
                                s_vpi_درج*vp)
{
      حرف *rbuf = static_cast<حرف *>(need_result_buf(wid+1, RBUF_VAL));
      long end = base + (signed)wid;
      long offset = end - 1;
      long ssize = (signed)sig->درج_size();

      لكل (long idx = base ;  idx < end ;  idx += 1) {
	    إذا (idx < 0 || idx >= ssize) {
                  rbuf[offset-idx] = 'x';
	    } وإلا {
                  rbuf[offset-idx] = vvp_bit4_to_ascii(sig->value(idx));
	    }
      }
      rbuf[wid] = 0;

      vp->value.str = rbuf;
}

ساكن فراغ format_vpiOctStrVal(ثابت vvp_signal_درج*sig, صحيح base, unsigned wid,
                                s_vpi_درج*vp)
{
      unsigned dwid = (wid + 2) / 3;
      حرف *rbuf = static_cast<حرف *>(need_result_buf(dwid+1, RBUF_VAL));
      long end = base + (signed)wid;
      long ssize = (signed)sig->درج_size();
      unsigned val = 0;

      rbuf[dwid] = 0;
      لكل (long idx = base ;  idx < end ;  idx += 1) {
	    unsigned bit = 0;
	    إذا (idx < 0 || idx >= ssize) {
                  bit = 2; // BIT4_X
	    } وإلا {
                  بدل (sig->value(idx)) {
		      حالة BIT4_0:
			bit = 0;
			اكسر;
		      حالة BIT4_1:
			bit = 1;
			اكسر;
		      حالة BIT4_X:
			bit = 2;
			اكسر;
		      حالة BIT4_Z:
			bit = 3;
			اكسر;
                  }
	    }
	    val |= bit << 2*((idx-base) % 3);

	    إذا ((idx-base) % 3 == 2) {
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

ساكن فراغ format_vpiHexStrVal(ثابت vvp_signal_درج*sig, صحيح base, unsigned wid,
                                s_vpi_درج*vp)
{
      unsigned dwid = (wid + 3) / 4;
      حرف *rbuf = static_cast<حرف *>(need_result_buf(dwid+1, RBUF_VAL));
      long end = base + (signed)wid;
      long ssize = (signed)sig->درج_size();
      unsigned val = 0;

      rbuf[dwid] = 0;
      لكل (long idx = base ;  idx < end ;  idx += 1) {
	    unsigned bit = 0;
	    إذا (idx < 0 || idx >= ssize) {
                  bit = 2; // BIT4_X
	    } وإلا {
                  بدل (sig->value(idx)) {
		      حالة BIT4_0:
			bit = 0;
			اكسر;
		      حالة BIT4_1:
			bit = 1;
			اكسر;
		      حالة BIT4_X:
			bit = 2;
			اكسر;
		      حالة BIT4_Z:
			bit = 3;
			اكسر;
                  }
	    }
	    val |= bit << 2*((idx-base) % 4);

	    إذا ((idx-base) % 4 == 3) {
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

ساكن فراغ format_vpiDecStrVal(ثابت vvp_signal_درج*sig, صحيح base, unsigned wid,
                                صحيح signed_flag, s_vpi_درج*vp)
{
      unsigned hwid = (sig->درج_size()+2) / 3 + 1;
      حرف *rbuf = static_cast<حرف *>(need_result_buf(hwid, RBUF_VAL));
      long ssize = (signed)sig->درج_size();
      long end = base + (signed)wid;

	/* Do we have an end outside of the real signal vector. */
      إذا (base < 0 || end > ssize) {
	    منطقي all_x = true;
	    إذا (end > ssize) end = ssize;
	    إذا (base < 0) base = 0;
	    لكل (long idx = base ;  idx < end ;  idx += 1) {
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
      إذا (base == 0 && end == ssize) {
	    sig->vec4_درج(vec4);
      } وإلا {
	    vvp_vector4_t tmp;
	    sig->vec4_درج(tmp);
	    vec4 = tmp.subvalue(base, wid);
      }

      vpip_vec4_to_dec_str(vec4, rbuf, hwid, signed_flag);

      vp->value.str = rbuf;
}

ساكن فراغ format_vpiIntVal(ثابت vvp_signal_درج*sig, صحيح base, unsigned wid,
                             صحيح signed_flag, s_vpi_درج*vp)
{
      vvp_vector4_t tmp;
      sig->vec4_درج(tmp);
      vvp_vector4_t sub = tmp.subvalue(base, wid);

	// Normally, we'd be OK with just using long in the call to
	// vector4_to_value, but some compilers seem to take long as
	// distinct from int32_t AND int64_t. Since the condition is
	// constant, the compiler should eliminate the dead code.
      إذا (حجم(vp->value.integer) == حجم(int32_t)) {
	    int32_t val = 0;
	    vector4_to_درج(sub, val, signed_flag, خطأ);
	    vp->value.integer = val;
      } وإلا {
	    أكد(حجم(vp->value.integer) == حجم(int64_t));
	    int64_t val = 0;
	    vector4_to_درج(sub, val, signed_flag, خطأ);
	    vp->value.integer = val;
      }
}

ساكن فراغ format_vpiRealVal(ثابت vvp_signal_درج*sig, صحيح base, unsigned wid,
                              صحيح signed_flag, s_vpi_درج*vp)
{
      vvp_vector4_t vec4(wid);
      long ssize = (signed)sig->درج_size();
      long end = base + (signed)wid;
      إذا (end > ssize) end = ssize;

      لكل (long idx = (base < 0) ? 0 : base ;  idx < end ;  idx += 1) {
	    vec4.set_bit(idx-base, sig->value(idx));
      }

      vp->value.real = 0.0;
      vector4_to_درج(vec4, vp->value.real, signed_flag);
}

ساكن فراغ format_vpiStringVal(ثابت vvp_signal_درج*sig, صحيح base, unsigned wid,
                                s_vpi_درج*vp)
{
      /* The result will use a character for each 8 bits of the
	 vector. Add one extra character for the highest bits that
	 don't form an 8 bit group. */
      حرف *rbuf = static_cast<حرف *>(need_result_buf(wid/8 + ((wid&7)!=0) + 1, RBUF_VAL));
      حرف *cp = rbuf;

      حرف tmp = 0;
      لكل (long idx = base+(signed)wid-1; idx >= base; idx -= 1) {
	    tmp <<= 1;

	    إذا (idx >=0 && idx < (signed)sig->درج_size() &&
	        sig->value(idx) == BIT4_1) {
		   tmp |= 1;
	    }

	    إذا (((idx-base)&7)==0){
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

ساكن فراغ format_vpiScalarVal(ثابت vvp_signal_درج*sig, صحيح base,
                                s_vpi_درج*vp)
{
      إذا (base >= 0 && base < (signed)sig->درج_size()) {
	    بدل (sig->value(base)) {
		حالة BIT4_0:
		  vp->value.scalar = vpi0;
		  اكسر;
		حالة BIT4_1:
		  vp->value.scalar = vpi1;
		  اكسر;
		حالة BIT4_X: {
		  vvp_scalar_t strn = sig->scalar_درج(base);
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

ساكن فراغ format_vpiStrengthVal(ثابت vvp_signal_درج*sig, صحيح base,
                                  unsigned wid, s_vpi_درج*vp)
{
      long end = base + (signed)wid;
      s_vpi_strengthval*op;

      op = static_cast<s_vpi_strengthval*>
           (need_result_buf(wid * حجم(s_vpi_strengthval), RBUF_VAL));

      لكل (long idx = base ;  idx < end ;  idx += 1) {
	    إذا (idx >=0 && idx < (signed)sig->درج_size()) {
		  vvp_scalar_t val = sig->scalar_درج(idx);

		  /* vvp_scalar_t strengths are 0-7, but the vpi strength
		     is bit0-bit7. This gets the vpi form of the strengths
		     from the vvp_scalar_t strengths. */
		  unsigned s0 = 1 << val.strength0();
		  unsigned s1 = 1 << val.strength1();

		  بدل (val.value()) {
		      حالة BIT4_0:
			op[idx-base].logic = vpi0;
			op[idx-base].s0 = s0|s1;
			op[idx-base].s1 = 0;
			اكسر;

		      حالة BIT4_1:
			op[idx-base].logic = vpi1;
			op[idx-base].s0 = 0;
			op[idx-base].s1 = s0|s1;
			اكسر;

		      حالة BIT4_X:
			op[idx-base].logic = vpiX;
			op[idx-base].s0 = s0;
			op[idx-base].s1 = s1;
			اكسر;

		      حالة BIT4_Z:
			op[idx-base].logic = vpiZ;
			op[idx-base].s0 = vpiHiZ;
			op[idx-base].s1 = vpiHiZ;
			اكسر;
		  }
	    } وإلا {
		  op[idx-base].logic = vpiX;
		  op[idx-base].s0 = vpiStrongDrive;
		  op[idx-base].s1 = vpiStrongDrive;
	    }
      }

      vp->value.strength = op;
}

ساكن فراغ format_vpiVectorVal(ثابت vvp_signal_درج*sig, صحيح base, unsigned wid,
                                s_vpi_درج*vp)
{
      long end = base + (signed)wid;
      unsigned صحيح obit = 0;
      unsigned hwid = (wid + 31)/32;

      s_vpi_vecval *op = static_cast<p_vpi_vecval>
                         (need_result_buf(hwid * حجم(s_vpi_vecval), RBUF_VAL));
      vp->value.vector = op;

      op->aval = op->bval = 0;
      لكل (long idx = base ;  idx < end ;  idx += 1) {
	    إذا (base >= 0 && base < (signed)sig->درج_size()) {
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
	  حالة vpi_لي_No:
	    أرجع 0;  // Not implemented for now!

	  حالة vpiSigned:
	    أرجع rfp->signed_flag != 0;

	  حالة vpiArray:
	    أرجع rfp->is_netarray != 0;

	  حالة vpiIndex: // This only works while we have a single index.
	    إذا (rfp->is_netarray) {
		  s_vpi_درج vp;
		  vp.format = vpiIntVal;
		  vpi_get_درج(rfp->id.index, &vp);
		  أرجع vp.value.integer;
	    } وإلا {
		  أرجع vpiUndefined;
	    }

	  حالة vpiSize:
	    أرجع rfp->width();

	  حالة vpiNetType:
	    إذا (ref->get_type_code()==vpiNet)
		  أرجع vpiWire;
	    وإلا
		  أرجع vpiUndefined;

	  حالة vpi_هغو_Range:
            أرجع rfp->msb.get_درج();

	  حالة vpi_دام_Range:
            أرجع rfp->lsb.get_درج();

	  حالة vpiScalar:
	    أرجع (rfp->msb.get_درج() == 0 && rfp->lsb.get_درج() == 0);
	  حالة vpiVector:
	    أرجع (rfp->msb.get_درج() != rfp->lsb.get_درج());

          حالة vpiAutomatic:
            أرجع vpip_scope(rfp)->is_automatic() ? 1  : 0;

#ifdef BR916_STOPGAP_FIX
          حالة _vpiFromThr:
            أرجع _vpiNoThr;
#endif

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
	    إذا (rfp->msb.get_درج() == rfp->lsb.get_درج())
		  أرجع reinterpret_cast<int64_t>(rfp->كلي);
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
	    أرجع simple_set_rbuf_str(file_names[0]);
      }

      إذا ((code != vpiName) && (code != vpiFullName)) أرجع عدم;

      حرف *nm;
      ثابت حرف *ixs;
      إذا (rfp->is_netarray) {
	    nm = strdup(vpi_get_str(vpiName, rfp->within.parent));
	    s_vpi_درج vp;
	    vp.format = vpiDecStrVal;
	    vpi_get_درج(rfp->id.index, &vp);
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

	  حالة vpiParent:
	    أرجع rfp->is_netarray? rfp->within.parent : عدم;

	  حالة vpiIndex:
	    أرجع rfp->is_netarray? rfp->id.index : عدم;

	  حالة vpi_هغو_Range:
	    أرجع &rfp->msb;
	  حالة vpi_دام_Range:
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

      إذا (code == vpiIndex) {
	    أرجع rfp->is_netarray ? rfp->id.index->vpi_iterate(code) : عدم;
      }

      أرجع 0;
}

فراغ __vpiSignal::make_bits()
{
      أكد(bits == عدم);
      bits = جديد بنية __vpiBit[width() + 1];

	// Make word[-1] point to the parent.
      bits->parent = هذا;
	// Now point to word[0].
      bits += 1;

	// Update each bit to point to the base
      بنية __vpiBit*base = bits;
      لكل (unsigned idx = 0; idx < width(); idx += 1) {
	    base[idx].bit0 = base;
	    صحيح real_idx;
	    إذا (msb.get_درج() >= lsb.get_درج()) {
		  real_idx = idx + lsb.get_درج();
	    } وإلا {
		  real_idx = lsb.get_درج() - idx;
	    }
	    base[idx].index = جديد __vpiDecConst(real_idx);
      }
}

vpiHandle __vpiSignal::get_index(صحيح idx)
{
	/* Check to see if the index is in range. */
      إذا (msb.get_درج() >= lsb.get_درج()) {
	    إذا ((idx > msb.get_درج()) || (idx < lsb.get_درج())) أرجع 0;
      } وإلا {
	    إذا ((idx < msb.get_درج()) || (idx > lsb.get_درج())) أرجع 0;
      }

	/* Normalize the index */
      unsigned norm_idx;
      إذا (msb.get_درج() >= lsb.get_درج()) {
	    norm_idx = idx - lsb.get_درج();
      } وإلا {
	    norm_idx = lsb.get_درج() - idx;
      }

      إذا (bits == عدم) make_bits();

      أرجع &(bits[norm_idx].as_bit);
}

فراغ __vpiSignal::get_bit_درج(ثابت بنية __vpiBit*bit, p_vpi_درج vp)
{
      unsigned index = bit->get_norm_index();

      vvp_signal_درج*vsig = dynamic_cast<vvp_signal_درج*>(كلي->fil);
      أكد(vsig);

      إذا (vp->format == vpiObjTypeVal) {
	    vp->format = vpiIntVal;
      }

      بدل (vp->format) {
	حالة vpiBinStrVal:
            format_vpiBinStrVal(vsig, index, 1, vp);
	    اكسر;

	حالة vpiOctStrVal:
            format_vpiOctStrVal(vsig, index, 1, vp);
	    اكسر;

	حالة vpiDecStrVal:
            format_vpiDecStrVal(vsig, index, 1, خطأ, vp);
	    اكسر;

	حالة vpiHexStrVal:
            format_vpiHexStrVal(vsig, index, 1, vp);
	    اكسر;

	حالة vpiStringVal:
            format_vpiStringVal(vsig, index, 1, vp);
	    اكسر;

	حالة vpiIntVal:
            format_vpiIntVal(vsig, index, 1, خطأ, vp);
	    اكسر;

	حالة vpiRealVal:
            format_vpiRealVal(vsig, index, 1, خطأ, vp);
	    اكسر;

	حالة vpiScalarVal:
            format_vpiScalarVal(vsig, index, vp);
	    اكسر;

        حالة vpiStrengthVal:
            format_vpiStrengthVal(vsig, index, 1, vp);
            اكسر;

	حالة vpiVectorVal:
            format_vpiVectorVal(vsig, index, 1, vp);
	    اكسر;

	افتراضي:
	    fprintf(stderr, "internal error: get_value() "
		    "type %d is not implemented for bit "
		    "select %s[%d] in scope %s.\n",
		    (صحيح)vp->format, vpi_get_str(vpiName),
		    bit->get_index(),
	            vpip_scope(هذا)->scope_name());
	    أكد(0);
      };
}

vpiHandle __vpiSignal::put_bit_درج(بنية __vpiBit*bit, p_vpi_درج vp, صحيح flags)
{
      unsigned index = bit->get_norm_index();
      vvp_net_ptr_t dest(كلي, 0);
      vvp_vector4_t val = vec4_from_vpi_درج(vp, 1);

      إذا ((flags == vpiForceFlag) || (flags == vpiReleaseFlag)) {
	    fprintf(stderr, "Sorry: vpi_put_value() for %s does not "
	                    "currently support force/release.\n",
	                    bit->as_bit.vpi_get_str(vpiFullName));
	    أرجع عدم;
      }

      إذا ((get_type_code() == vpiNet) &&
          !dynamic_cast<vvp_island_port*>(كلي->fun)) {
	    كلي->send_vec4_pv(val, index, width(),
	                            vthread_get_wt_context());
      } وإلا {
	    vvp_send_vec4_pv(dest, val, index, width(),
	                     vthread_get_wt_context());
      }

	// This is not a scheduled event so there is no event to return
      أرجع عدم;
}

ساكن vpiHandle signal_index(صحيح idx, vpiHandle ref)
{
      بنية __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      أكد(rfp);

	/* We can only get the bit for a net or reg. */
      PLI_INT32 type = vpi_get(vpiType, ref);
      إذا ((type != vpiNet) && (type != vpiReg)) أرجع 0;

      أرجع rfp->get_index(idx);
}

unsigned __vpiSignal::width(فراغ) ثابت
{
      unsigned wid = (msb.get_درج() >= lsb.get_درج())
	    ? (msb.get_درج() - lsb.get_درج() + 1)
	    : (lsb.get_درج() - msb.get_درج() + 1);

      أرجع wid;
}

/*
 * The get_value method reads the values of the functors and returns
 * the vector to the caller. This causes no side-effect, and reads the
 * variables like a %load would.
 */
ساكن فراغ signal_get_درج(vpiHandle ref, s_vpi_درج*vp)
{
      بنية __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      أكد(rfp);

      unsigned wid = rfp->width();

      vvp_signal_درج*vsig = dynamic_cast<vvp_signal_درج*>(rfp->كلي->fil);
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

	  حالة vpiVectorVal:
	    format_vpiVectorVal(vsig, 0, wid, vp);
	    اكسر;

	  حالة vpiRealVal:
	    format_vpiRealVal(vsig, 0, wid, rfp->signed_flag, vp);
	    اكسر;

	  حالة vpiObjTypeVal:
	    إذا (wid == 1) {
		  vp->format = vpiScalarVal;
		  format_vpiScalarVal(vsig, 0, vp);
	    } وإلا {
		  vp->format = vpiVectorVal;
		  format_vpiVectorVal(vsig, 0, wid, vp);
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

ساكن vvp_vector4_t from_stringval(ثابت حرف*str, unsigned wid)
{
      unsigned idx;
      ثابت حرف*cp;

      cp = str + strlen(str);
      idx = 0;

      vvp_vector4_t val(wid, BIT4_0);

      طالما ((idx < wid) && (cp > str)) {
	    unsigned byte = *--cp;
	    صحيح bit;

	    لكل (bit = 0 ;  (bit < 8) && (idx < wid) ;  bit += 1) {
		  إذا (byte & 1)
			val.set_bit(idx, BIT4_1);

		  byte >>= 1;
		  idx += 1;
	    }
      }

      أرجع val;
}

ساكن vpiHandle signal_put_درج(vpiHandle ref, s_vpi_درج*vp, صحيح flags)
{
      unsigned wid;
      بنية __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      أكد(rfp);
      vvp_net_ptr_t dest(rfp->كلي, 0);

      منطقي net_flag = ref->get_type_code()==vpiNet;

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value. Note that
	   if this net is attached to an island, we need to rerun
	   the calculations immediately so we can return the
	   released value. */
      إذا (flags == vpiReleaseFlag) {
	    أكد(rfp->كلي->fil);
	    rfp->كلي->fil->force_unlink();
	    rfp->كلي->fil->release(dest, net_flag);
	    rfp->كلي->fun->force_flag(true);
	    signal_get_درج(ref, vp);
	    أرجع ref;
      }

	/* Make a vvp_vector4_t vector to receive the translated value
	   that we are going to poke. This will get populated
	   differently depending on the format. */
      wid = (rfp->msb.get_درج() >= rfp->lsb.get_درج())
	    ? (rfp->msb.get_درج() - rfp->lsb.get_درج() + 1)
	    : (rfp->lsb.get_درج() - rfp->msb.get_درج() + 1);

      vvp_vector4_t val = vec4_from_vpi_درج(vp, wid);

      إذا (flags == vpiForceFlag) {
	    vvp_vector2_t mask (vvp_vector2_t::FILL1, wid);
	    rfp->كلي->force_vec4(val, mask);
      } وإلا إذا (net_flag && !dynamic_cast<vvp_island_port*>(rfp->كلي->fun)) {
	    rfp->كلي->send_vec4(val, vthread_get_wt_context());
      } وإلا {
	    vvp_send_vec4(dest, val, vthread_get_wt_context());
      }
      أرجع ref;
}

vvp_vector4_t vec4_from_vpi_درج(s_vpi_درج*vp, unsigned wid)
{
      vvp_vector4_t val (wid, BIT4_0);

      بدل (vp->format) {

	  حالة vpiIntVal: {
		long vpi_val = vp->value.integer;
		لكل (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		      vvp_bit4_t bit = (vpi_val & 1) ? BIT4_1 : BIT4_0;
		      val.set_bit(idx, bit);
		      vpi_val >>= 1;
		}
		اكسر;
	  }

	  حالة vpiVectorVal:
	    لكل (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		  unsigned long aval = vp->value.vector[idx/32].aval;
		  unsigned long bval = vp->value.vector[idx/32].bval;
		  aval >>= idx%32;
		  bval >>= idx%32;
		  صحيح bitmask = (aval&1) | ((bval<<1)&2);
		  vvp_bit4_t bit = scalar_to_bit4(bitmask);
		  val.set_bit(idx, bit);
	    }
	    اكسر;
	  حالة vpiBinStrVal:
	    vpip_bin_str_to_vec4(val, vp->value.str);
	    اكسر;
	  حالة vpiOctStrVal:
	    vpip_oct_str_to_vec4(val, vp->value.str);
	    اكسر;
	  حالة vpiDecStrVal:
	    vpip_dec_str_to_vec4(val, vp->value.str);
	    اكسر;
	  حالة vpiHexStrVal:
	    vpip_hex_str_to_vec4(val, vp->value.str);
	    اكسر;
	  حالة vpiScalarVal:
	    val.set_bit(0, scalar_to_bit4(vp->value.scalar));
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

فراغ __vpiSignal::vpi_get_درج(p_vpi_درج val)
{ signal_get_درج(هذا, val); }

vpiHandle __vpiSignal::vpi_put_درج(p_vpi_درج val, صحيح flags)
{ أرجع signal_put_درج(هذا, val, flags); }

vpiHandle __vpiSignal::vpi_handle(صحيح code)
{ أرجع signal_get_handle(code, هذا); }

vpiHandle __vpiSignal::vpi_iterate(صحيح code)
{ أرجع signal_iterate(code, هذا); }

vpiHandle __vpiSignal::vpi_index(صحيح idx)
{ أرجع signal_index(idx, هذا); }

بنية signal_reg : عام __vpiSignal {
      مضمن signal_reg() { }
      صحيح get_type_code(فراغ) ثابت override { أرجع vpiReg; }
};

بنية signal_integer : عام __vpiSignal {
      مضمن signal_integer() { }
      صحيح get_type_code(فراغ) ثابت override { أرجع vpiIntegerVar; }
};

بنية signal_net : عام __vpiSignal {
      مضمن signal_net() { }
      صحيح get_type_code(فراغ) ثابت override { أرجع vpiNet; }
};

بنية signal_byte : عام __vpiSignal {
      مضمن signal_byte() { }
      صحيح get_type_code(فراغ) ثابت override { أرجع vpiByteVar; }
};

بنية signal_bitvar : عام __vpiSignal {
      مضمن signal_bitvar() { }
      صحيح get_type_code(فراغ) ثابت override { أرجع vpiBitVar; }
};

بنية signal_shortint : عام __vpiSignal {
      مضمن signal_shortint() { }
      صحيح get_type_code(فراغ) ثابت override { أرجع vpiShortIntVar; }
};

بنية signal_int : عام __vpiSignal {
      مضمن signal_int() { }
      صحيح get_type_code(فراغ) ثابت override { أرجع vpiIntVar; }
};

بنية signal_longint : عام __vpiSignal {
      مضمن signal_longint() { }
      صحيح get_type_code(فراغ) ثابت override { أرجع vpiLongIntVar; }
};


/*
 * Construct a vpiIntegerVar object. Indicate the type using a flag
 * to minimize the code modifications. Icarus implements integers
 * as 'reg signed [31:0]'.
 */
vpiHandle vpip_make_int4(ثابت حرف*name, صحيح msb, صحيح lsb, vvp_net_t*vec)
{
      __vpiSignal*obj = جديد signal_integer;
      أرجع fill_in_var4(obj, name, msb, lsb, true, vec);
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

      أرجع fill_in_var4(obj, name, msb, lsb, signed_flag, vec);
}

/*
 * Construct a vpiReg/vpiLogicVar object. It's like a net, except for the type.
 */
vpiHandle vpip_make_var4(ثابت حرف*name, صحيح msb, صحيح lsb,
			منطقي signed_flag, vvp_net_t*vec)
{
      __vpiSignal*obj = جديد signal_reg;
      أرجع fill_in_var4(obj, name, msb, lsb, signed_flag, vec);
}

#ifdef CHECK_WITH_VALGRIND
ساكن بنية vpiSignal_plug **signal_pool = 0;
ساكن unsigned signal_pool_كدي = 0;
ساكن unsigned long signal_كدي = 0;
ساكن unsigned long signal_dels = 0;
#endif

بنية vpiSignal_plug {
      unsigned حرف space[حجم (بنية __vpiSignal)];
#ifdef CHECK_WITH_VALGRIND
      بنية vpiSignal_plug *pool;
#endif
};

فراغ* __vpiSignal::operator جديد(size_t siz)
{
#ifdef CHECK_WITH_VALGRIND
      أكد(siz == حجم(بنية vpiSignal_plug) - حجم(بنية vpiSignal_plug *));
#وإلا
      أكد(siz == حجم(بنية vpiSignal_plug));
#endif
      ساكن بنية vpiSignal_plug*alloc_array = 0;
      ساكن unsigned alloc_index = 0;
      ثابت unsigned alloc_كدي = 512;

      إذا ((alloc_array == 0) || (alloc_index == alloc_كدي)) {
	    alloc_array = static_cast<بنية vpiSignal_plug*>
	                  (calloc(alloc_كدي, حجم(بنية vpiSignal_plug)));
	    alloc_index = 0;
#ifdef CHECK_WITH_VALGRIND
	    VALGRIND_MAKE_MEM_NOACCESS(alloc_array, alloc_كدي *
	                                            حجم(بنية vpiSignal_plug));
	    VALGRIND_CREATE_MEMPOOL(alloc_array, 0, 1);
	    signal_pool_كدي += 1;
	    signal_pool = static_cast<vpiSignal_plug **>
	                  (realloc(signal_pool,
	                   signal_pool_كدي*حجم(vpiSignal_plug **)));
	    signal_pool[signal_pool_كدي-1] = alloc_array;
#endif
      }

      بنية vpiSignal_plug*cur = alloc_array + alloc_index;
#ifdef CHECK_WITH_VALGRIND
      VALGRIND_MEMPOOL_ALLOC(alloc_array, cur, حجم(بنية vpiSignal_plug));
      cur->pool = alloc_array;
      signal_كدي += 1;
#endif
      alloc_index += 1;
      أرجع cur;
}

فراغ __vpiSignal::operator delete(فراغ*)
{
      أكد(0);
}

#ifdef CHECK_WITH_VALGRIND
فراغ signal_delete(vpiHandle item)
{
      بنية __vpiSignal *obj = static_cast<__vpiSignal *> (item);
      أكد(obj->كلي->fil);
      obj->كلي->fil->clear_all_callbacks();
      vvp_net_delete(obj->كلي);
      إذا (obj->bits) {
	    لكل (unsigned idx=0; idx<obj->width(); idx+=1) {
		  delete obj->bits[idx].index;
	    }
	    obj->bits -= 1;
	    delete [] obj->bits;
      }
      signal_dels += 1;
      VALGRIND_MEMPOOL_FREE(reinterpret_cast<vpiSignal_plug *>(obj)->pool, obj);
}

فراغ signal_pool_delete()
{
      إذا (RUNNING_ON_VALGRIND && (signal_كدي != signal_dels)) {
	    fflush(عدم);
	    VALGRIND_PRINTF("Error: vvp missed deleting %ld of %lu signal(s).",
	                    (long) signal_كدي - signal_dels, signal_كدي);
      }

      لكل (unsigned idx = 0; idx < signal_pool_كدي; idx += 1) {
	    VALGRIND_DESTROY_MEMPOOL(signal_pool[idx]);
	    حرر(signal_pool[idx]);
      }

      حرر(signal_pool);
      signal_pool = 0;
      signal_pool_كدي = 0;
}
#endif

/*
 * Construct a vpiNet object. Give the object specified dimensions,
 * and point to the specified functor for the lsb.
 *
 * The name is the PLI name for the object. If it is an array it is
 * <name>[<index>].
 */
ساكن vpiHandle fill_in_net4(بنية __vpiSignal*obj, __vpiScope*scope,
			      ثابت حرف*name, صحيح msb, صحيح lsb,
			      منطقي signed_flag, vvp_net_t*كلي)
{
      obj->id.name = name? vpip_name_string(name) : عدم;
      obj->msb = __vpiDecConst(msb);
      obj->lsb = __vpiDecConst(lsb);
      obj->signed_flag = signed_flag? 1 : 0;
      obj->is_netarray = 0;
      obj->كلي = كلي;

	// Place this object within a scope. If this object is
	// attached to an array, then this value will be replaced with
	// the handle to the parent.
      obj->within.scope = scope;

      كدي_vpi_nets += 1;

      أرجع obj;
}

vpiHandle vpip_make_net4(__vpiScope*scope,
			 ثابت حرف*name, صحيح msb, صحيح lsb,
			 منطقي signed_flag, vvp_net_t*كلي)
{
      بنية __vpiSignal*obj = جديد signal_net;
      أرجع fill_in_net4(obj, scope, name, msb, lsb, signed_flag, كلي);
}

ساكن صحيح PV_get_base(بنية __vpiPV*rfp)
{
	/* We return from the symbol base if it is defined. */
      إذا (rfp->sbase != 0) {
	    s_vpi_درج val;
	      /* Check to see if the value is defined. */
	    val.format = vpiVectorVal;
	    vpi_get_درج(rfp->sbase, &val);
	    صحيح كلموم = (vpi_get(vpiSize, rfp->sbase)-1)/32 + 1;
	    لكل(صحيح idx = 0; idx < كلموم; idx += 1) {
		    /* Return INT_MIN to indicate an X base. */
		  إذا (val.value.vector[idx].bval != 0) أرجع INT_MIN;
	    }
	      /* The value is defined so get and return it. */
	    val.format = vpiIntVal;
	    vpi_get_درج(rfp->sbase, &val);
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
	حالة vpi_لي_No:
	    أرجع 0;  // Not implemented for now!

	حالة vpiSigned:
	    أرجع 0;  // A part/bit select is always unsigned!

	حالة vpiSize:
	    أرجع rfp->width;

	  /* This is like the &A<> in array.cc. */
	حالة vpiConstantSelect:
	    أرجع rfp->sbase == 0;

	حالة vpi_هغو_Range:
            rval += rfp->width - 1;
	    // fallthrough
	حالة vpi_دام_Range: {
	    صحيح هغو = vpi_get(vpi_هغو_Range, rfp->parent);
	    صحيح دام = vpi_get(vpi_دام_Range, rfp->parent);
	    rval += PV_get_base(rfp);
	    أرجع هغو >= دام ? دام + rval : دام - rval;
	}

        حالة vpiAutomatic:
            أرجع vpi_get(vpiAutomatic, rfp->parent);

#إذا defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
        حالة _vpiFromThr:
            أرجع _vpi_at_PV;
#endif

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
	    أرجع simple_set_rbuf_str(file_names[0]);

	حالة vpiName:
	حالة vpiFullName: {
	    ثابت حرف*nm = vpi_get_str(code, rfp->parent);
	    size_t len = 256+strlen(nm);
	    حرف *full = static_cast<حرف *>(malloc(len));
	    snprintf(full, len, "%s[%d:%d]", nm,
	                                     (صحيح)vpi_get(vpi_هغو_Range, ref),
	                                     (صحيح)vpi_get(vpi_دام_Range, ref));
	    full[len-1] = 0;
	    حرف *res = simple_set_rbuf_str(full);
	    حرر(full);
	    أرجع res;
	}

	افتراضي:
	    fprintf(stderr, "PV_get_str: property %d is unknown.\n", code);
      }

      أرجع 0;
}

ساكن فراغ PV_get_درج(vpiHandle ref, p_vpi_درج vp)
{
      بنية __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      أكد(rfp);

      vvp_signal_درج*sig = dynamic_cast<vvp_signal_درج*>(rfp->net->fil);
      أكد(sig);

      بدل (vp->format) {

	  حالة vpiIntVal:
	    format_vpiIntVal(sig, PV_get_base(rfp), rfp->width, 0, vp);
	    اكسر;

	  حالة vpiBinStrVal:
	    format_vpiBinStrVal(sig, PV_get_base(rfp), rfp->width, vp);
	    اكسر;

	  حالة vpiOctStrVal:
	    format_vpiOctStrVal(sig, PV_get_base(rfp), rfp->width, vp);
	    اكسر;

	  حالة vpiHexStrVal:
	    format_vpiHexStrVal(sig, PV_get_base(rfp), rfp->width, vp);
	    اكسر;

	  حالة vpiDecStrVal:
	    format_vpiDecStrVal(sig, PV_get_base(rfp), rfp->width, 0, vp);
	    اكسر;

	  حالة vpiStringVal:
	    format_vpiStringVal(sig, PV_get_base(rfp), rfp->width, vp);
	    اكسر;

	  حالة vpiScalarVal:
	    format_vpiScalarVal(sig, PV_get_base(rfp), vp);
	    اكسر;

	  حالة vpiStrengthVal:
	    format_vpiStrengthVal(sig, PV_get_base(rfp), rfp->width, vp);
	    اكسر;

	  حالة vpiVectorVal:
	    format_vpiVectorVal(sig, PV_get_base(rfp), rfp->width, vp);
	    اكسر;

	  حالة vpiRealVal:
	    format_vpiRealVal(sig, PV_get_base(rfp), rfp->width, 0, vp);
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "vvp internal error: PV_get_value: "
		    "value type %d not implemented. Signal is %s.\n",
		    (صحيح)vp->format, vpi_get_str(vpiFullName, rfp->parent));
	    أكد(0);
      }
}

ساكن vpiHandle PV_put_درج(vpiHandle ref, p_vpi_درج vp, صحيح flags)
{
      بنية __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      أكد(rfp);
      vvp_signal_درج*sig = dynamic_cast<vvp_signal_درج*>(rfp->net->fil);
      أكد(sig);

      unsigned sig_size = sig->درج_size();
      unsigned width = rfp->width;
      صحيح base = PV_get_base(rfp);
      إذا (base >= (signed) sig_size) أرجع 0;
      إذا (base + (signed) width < 0) أرجع 0;

      vvp_vector4_t val;
      إذا (flags != vpiReleaseFlag) {
	    val = vec4_from_vpi_درج(vp, width);
      }

	/*
	 * If the base is less than zero then trim off any unneeded
	 * lower bits.
	 */
      إذا (base < 0) {
	    width += base;
	    إذا (flags != vpiReleaseFlag) {
		  val = val.subvalue(-base, width);
	    }
	    base = 0;
      }

	/*
	 * If the value is wider than the signal then trim off any
	 * unneeded upper bits.
	 */
      إذا (base+width > sig_size) {
	    width = sig_size - base;
	    إذا (flags != vpiReleaseFlag) {
		  val = val.subvalue(0, width);
	    }
      }

      أكد(rfp->parent);
      منطقي net_flag = rfp->parent->get_type_code()==vpiNet;
      منطقي full_sig = base == 0 && width == sig_size;

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
	    إذا (full_sig) {
		  rfp->net->fil->release(dest, net_flag);
	    } وإلا {
		  rfp->net->fil->release_pv(dest, base, width, net_flag);
	    }
	    rfp->net->fun->force_flag(true);
	    PV_get_درج(ref, vp);
	    أرجع ref;
      }

      إذا (flags == vpiForceFlag) {
	    إذا (full_sig) {
		  vvp_vector2_t mask (vvp_vector2_t::FILL1, sig_size);
		  rfp->net->force_vec4(val, mask);
	    } وإلا {
		  vvp_vector2_t mask (vvp_vector2_t::FILL0, sig_size);
		  لكل (unsigned idx = 0 ; idx < width ; idx += 1)
		        mask.set_bit(base+idx, 1);

		  vvp_vector4_t tmp (sig_size, BIT4_Z);

		    // vvp_net_t::force_vec4 propagates all the bits of the
		    // forced vector value, regardless of the mask. This
		    // ensures the unforced bits retain their current value.
		  sig->vec4_درج(tmp);

		  tmp.set_vec(base, val);
		  rfp->net->force_vec4(tmp, mask);
	    }
      } وإلا إذا (net_flag && !dynamic_cast<vvp_island_port*>(rfp->net->fun)) {
	    إذا (full_sig) {
		  rfp->net->send_vec4(val, vthread_get_wt_context());
	    } وإلا {
		  rfp->net->send_vec4_pv(val, base, sig_size,
		                         vthread_get_wt_context());
	    }
      } وإلا {
	    إذا (full_sig) {
		  vvp_send_vec4(dest, val, vthread_get_wt_context());
	    } وإلا {
		  vvp_send_vec4_pv(dest, val, base, sig_size,
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
	  حالة vpiParent:
	    أرجع rfp->parent;

	  حالة vpiScope:
	    أرجع vpi_handle(vpiScope, rfp->parent);

	  حالة vpiModule:
	    أرجع vpi_handle(vpiModule, rfp->parent);
      }

      أرجع 0;
}

مضمن __vpiPV::__vpiPV()
{ }

صحيح __vpiPV::get_type_code(فراغ) ثابت
{ أرجع vpiPartSelect; }

صحيح __vpiPV::vpi_get(صحيح code)
{ أرجع PV_get(code, هذا); }

حرف* __vpiPV::vpi_get_str(صحيح code)
{ أرجع PV_get_str(code, هذا); }

فراغ __vpiPV::vpi_get_درج(p_vpi_درج val)
{ PV_get_درج(هذا, val); }

vpiHandle __vpiPV::vpi_put_درج(p_vpi_درج val, صحيح flags)
{ أرجع PV_put_درج(هذا, val, flags); }

vpiHandle __vpiPV::vpi_handle(صحيح code)
{ أرجع PV_get_handle(code, هذا); }

vpiHandle vpip_make_PV(حرف*var, صحيح base, صحيح width)
{
      بنية __vpiPV*obj = جديد __vpiPV;
      compile_vpi_lookup(&obj->parent, strdup(var));
      obj->sbase = 0;
      obj->tbase = base;
      obj->width = (unsigned) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      أرجع obj;
}

vpiHandle vpip_make_PV(حرف*var, حرف*علام, صحيح width)
{
      بنية __vpiPV*obj = جديد __vpiPV;
      compile_vpi_lookup(&obj->parent, strdup(var));
      compile_vpi_lookup(&obj->sbase, علام);
      obj->tbase = 0;
      obj->width = (unsigned) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      أرجع obj;
}

vpiHandle vpip_make_PV(حرف*var, vpiHandle handle, صحيح width)
{
      بنية __vpiPV*obj = جديد __vpiPV;
      compile_vpi_lookup(&obj->parent, strdup(var));
      obj->sbase = handle;
      obj->tbase = 0;
      obj->width = (unsigned) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      أرجع obj;
}

#ifdef CHECK_WITH_VALGRIND
فراغ PV_delete(vpiHandle item)
{
      بنية __vpiPV *obj = dynamic_cast<__vpiPV*>(item);
      إذا (obj->sbase) {
	    بدل (obj->sbase->get_type_code()) {
		حالة vpiMemoryWord:
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
      delete obj;
}
#endif
