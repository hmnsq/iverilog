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
بیرونی ثابت نویسه hex_digits[256];
بیرونی ثابت نویسه oct_digits[64];

/*
 * The string values need a result buf to hold the results. This
 * buffer can be reused for that purpose. Whenever I have a need, the
 * need_result_buf function makes sure that need can be met.
 */
تهی *need_result_buf(unsigned cnt, vpi_rbuf_t type)
{
      ایستا تهی*result_buf[2] = {0, 0};
      ایستا size_t result_buf_size[2] = {0, 0};

      اگر (type == RBUF_DEL) {
	    آزاد(result_buf[RBUF_VAL]);
	    result_buf[RBUF_VAL] = 0;
	    result_buf_size[RBUF_VAL] = 0;

	    آزاد(result_buf[RBUF_STR]);
	    result_buf[RBUF_STR] = 0;
	    result_buf_size[RBUF_STR] = 0;

	    بازگشت 0;
      }

      cnt = (cnt + 0x0fff) & ~0x0fff;

      اگر (result_buf_size[type] == 0) {
	    result_buf[type] = malloc(cnt);
	    result_buf_size[type] = cnt;
      } وگرنه اگر (result_buf_size[type] < cnt) {
	    result_buf[type] = realloc(result_buf[type], cnt);
	    result_buf_size[type] = cnt;
      }

      بازگشت result_buf[type];
}

نویسه *simple_set_rbuf_str(ثابت نویسه *s1)
{
      نویسه *res = static_cast<نویسه *>(need_result_buf(strlen(s1)+1, RBUF_STR));
      اگر (res) strcpy(res,s1);
      بازگشت res;
}

نویسه *generic_get_str(صحیح code, vpiHandle ref, ثابت نویسه *name, ثابت نویسه *index)
{
      size_t len = strlen(name) + 1;  /* include space for null termination */
      نویسه *bn = پوچ;
      منطقی is_bn_a_pkg = نادرست;
      اگر (code == vpiFullName) {
	    bn = strdup(vpi_get_str(code,ref));
	    size_t bn_len = strlen(bn);
	    is_bn_a_pkg = (bn_len >= 3) && (bn[bn_len-1] == ':') && (bn[bn_len-2] == ':');
	    len += bn_len;
	    اگر (! is_bn_a_pkg) len += 1; // include space for "." separator
      }
      اگر (index != پوچ) len += strlen(index) + 2;  /* include space for brackets */

      نویسه *res = static_cast<نویسه *>(need_result_buf(len, RBUF_STR));
      اگر (!res) {
	    آزاد(bn);
	    بازگشت پوچ;
      }
      *res=0;  /* start with nothing */

	/* if this works, I can make it more efficient later */
      اگر (bn != پوچ) {
	    strcat(res, bn);
	      // A package already has the "::" separator in the name
	    اگر (! is_bn_a_pkg) strcat(res, ".");
	    آزاد(bn);
      }
      strcat(res, name);
      اگر (index != پوچ) {
	    strcat(res, "[");
	    strcat(res, index);
	    strcat(res, "]");
      }
      بازگشت res;
}

ایستا vpiHandle fill_in_net4(ساختار __vpiSignal*obj, __vpiScope*scope,
                              ثابت نویسه*name, صحیح msb, صحیح lsb,
                              منطقی signed_flag, vvp_net_t*گره);

ایستا vpiHandle fill_in_var4(ساختار __vpiSignal*obj,
                              ثابت نویسه*name, صحیح msb, صحیح lsb,
                              منطقی signed_flag, vvp_net_t*گره)
{
	// Variable declarations are always resolved immediately,
	// so we can assume they belong in the current scope.
      بازگشت fill_in_net4(obj, vpip_peek_current_scope(),
			  name, msb, lsb, signed_flag, گره);
}

/*
 * The standard formatting/conversion routines.
 * They work with full or partial signals.
 */

ایستا تهی format_vpiBinStrVal(ثابت vvp_signal_مقدار*sig, صحیح base, unsigned wid,
                                s_vpi_مقدار*vp)
{
      نویسه *rbuf = static_cast<نویسه *>(need_result_buf(wid+1, RBUF_VAL));
      long end = base + (signed)wid;
      long offset = end - 1;
      long ssize = (signed)sig->مقدار_size();

      برای (long idx = base ;  idx < end ;  idx += 1) {
	    اگر (idx < 0 || idx >= ssize) {
                  rbuf[offset-idx] = 'x';
	    } وگرنه {
                  rbuf[offset-idx] = vvp_bit4_to_ascii(sig->value(idx));
	    }
      }
      rbuf[wid] = 0;

      vp->value.str = rbuf;
}

ایستا تهی format_vpiOctStrVal(ثابت vvp_signal_مقدار*sig, صحیح base, unsigned wid,
                                s_vpi_مقدار*vp)
{
      unsigned dwid = (wid + 2) / 3;
      نویسه *rbuf = static_cast<نویسه *>(need_result_buf(dwid+1, RBUF_VAL));
      long end = base + (signed)wid;
      long ssize = (signed)sig->مقدار_size();
      unsigned val = 0;

      rbuf[dwid] = 0;
      برای (long idx = base ;  idx < end ;  idx += 1) {
	    unsigned bit = 0;
	    اگر (idx < 0 || idx >= ssize) {
                  bit = 2; // BIT4_X
	    } وگرنه {
                  انتخاب (sig->value(idx)) {
		      حالت BIT4_0:
			bit = 0;
			بشکن;
		      حالت BIT4_1:
			bit = 1;
			بشکن;
		      حالت BIT4_X:
			bit = 2;
			بشکن;
		      حالت BIT4_Z:
			bit = 3;
			بشکن;
                  }
	    }
	    val |= bit << 2*((idx-base) % 3);

	    اگر ((idx-base) % 3 == 2) {
		dwid -= 1;
		rbuf[dwid] = oct_digits[val];
		val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      انتخاب (wid % 3) {
	  حالت 1:
	    اگر (val == 2) val = 42;
	    وگرنه اگر (val == 3) val = 63;
	    بشکن;
	  حالت 2:
	    اگر (val == 10) val = 42;
	    وگرنه اگر (val == 15) val = 63;
	    بشکن;
      }

      اگر (dwid > 0) rbuf[0] = oct_digits[val];

      vp->value.str = rbuf;
}

ایستا تهی format_vpiHexStrVal(ثابت vvp_signal_مقدار*sig, صحیح base, unsigned wid,
                                s_vpi_مقدار*vp)
{
      unsigned dwid = (wid + 3) / 4;
      نویسه *rbuf = static_cast<نویسه *>(need_result_buf(dwid+1, RBUF_VAL));
      long end = base + (signed)wid;
      long ssize = (signed)sig->مقدار_size();
      unsigned val = 0;

      rbuf[dwid] = 0;
      برای (long idx = base ;  idx < end ;  idx += 1) {
	    unsigned bit = 0;
	    اگر (idx < 0 || idx >= ssize) {
                  bit = 2; // BIT4_X
	    } وگرنه {
                  انتخاب (sig->value(idx)) {
		      حالت BIT4_0:
			bit = 0;
			بشکن;
		      حالت BIT4_1:
			bit = 1;
			بشکن;
		      حالت BIT4_X:
			bit = 2;
			بشکن;
		      حالت BIT4_Z:
			bit = 3;
			بشکن;
                  }
	    }
	    val |= bit << 2*((idx-base) % 4);

	    اگر ((idx-base) % 4 == 3) {
		dwid -= 1;
		rbuf[dwid] = hex_digits[val];
		val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      انتخاب (wid % 4) {
	  حالت 1:
	    اگر (val == 2) val = 170;
	    وگرنه اگر (val == 3) val = 255;
	    بشکن;
	  حالت 2:
	    اگر (val == 10) val = 170;
	    وگرنه اگر (val == 15) val = 255;
	    بشکن;
	  حالت 3:
	    اگر (val == 42) val = 170;
	    وگرنه اگر (val == 63) val = 255;
	    بشکن;
      }

      اگر (dwid > 0) rbuf[0] = hex_digits[val];

      vp->value.str = rbuf;
}

ایستا تهی format_vpiDecStrVal(ثابت vvp_signal_مقدار*sig, صحیح base, unsigned wid,
                                صحیح signed_flag, s_vpi_مقدار*vp)
{
      unsigned hwid = (sig->مقدار_size()+2) / 3 + 1;
      نویسه *rbuf = static_cast<نویسه *>(need_result_buf(hwid, RBUF_VAL));
      long ssize = (signed)sig->مقدار_size();
      long end = base + (signed)wid;

	/* Do we have an end outside of the real signal vector. */
      اگر (base < 0 || end > ssize) {
	    منطقی all_x = درست;
	    اگر (end > ssize) end = ssize;
	    اگر (base < 0) base = 0;
	    برای (long idx = base ;  idx < end ;  idx += 1) {
		  اگر (sig->value(idx) != BIT4_X) {
			all_x = نادرست;
			بشکن;
		  }
	    }

	    اگر (all_x) {
		  rbuf[0] = 'x';
	    } وگرنه {
		  rbuf[0] = 'X';
	    }
	    rbuf[1] = 0;

	    vp->value.str = rbuf;
	    بازگشت;
      }

      vvp_vector4_t vec4;
      اگر (base == 0 && end == ssize) {
	    sig->vec4_مقدار(vec4);
      } وگرنه {
	    vvp_vector4_t tmp;
	    sig->vec4_مقدار(tmp);
	    vec4 = tmp.subvalue(base, wid);
      }

      vpip_vec4_to_dec_str(vec4, rbuf, hwid, signed_flag);

      vp->value.str = rbuf;
}

ایستا تهی format_vpiIntVal(ثابت vvp_signal_مقدار*sig, صحیح base, unsigned wid,
                             صحیح signed_flag, s_vpi_مقدار*vp)
{
      vvp_vector4_t tmp;
      sig->vec4_مقدار(tmp);
      vvp_vector4_t sub = tmp.subvalue(base, wid);

	// Normally, we'd be OK with just using long in the call to
	// vector4_to_value, but some compilers seem to take long as
	// distinct from int32_t AND int64_t. Since the condition is
	// constant, the compiler should eliminate the dead code.
      اگر (اندازه(vp->value.integer) == اندازه(int32_t)) {
	    int32_t val = 0;
	    vector4_to_مقدار(sub, val, signed_flag, نادرست);
	    vp->value.integer = val;
      } وگرنه {
	    أكد(اندازه(vp->value.integer) == اندازه(int64_t));
	    int64_t val = 0;
	    vector4_to_مقدار(sub, val, signed_flag, نادرست);
	    vp->value.integer = val;
      }
}

ایستا تهی format_vpiRealVal(ثابت vvp_signal_مقدار*sig, صحیح base, unsigned wid,
                              صحیح signed_flag, s_vpi_مقدار*vp)
{
      vvp_vector4_t vec4(wid);
      long ssize = (signed)sig->مقدار_size();
      long end = base + (signed)wid;
      اگر (end > ssize) end = ssize;

      برای (long idx = (base < 0) ? 0 : base ;  idx < end ;  idx += 1) {
	    vec4.set_bit(idx-base, sig->value(idx));
      }

      vp->value.real = 0.0;
      vector4_to_مقدار(vec4, vp->value.real, signed_flag);
}

ایستا تهی format_vpiStringVal(ثابت vvp_signal_مقدار*sig, صحیح base, unsigned wid,
                                s_vpi_مقدار*vp)
{
      /* The result will use a character for each 8 bits of the
	 vector. Add one extra character for the highest bits that
	 don't form an 8 bit group. */
      نویسه *rbuf = static_cast<نویسه *>(need_result_buf(wid/8 + ((wid&7)!=0) + 1, RBUF_VAL));
      نویسه *cp = rbuf;

      نویسه tmp = 0;
      برای (long idx = base+(signed)wid-1; idx >= base; idx -= 1) {
	    tmp <<= 1;

	    اگر (idx >=0 && idx < (signed)sig->مقدار_size() &&
	        sig->value(idx) == BIT4_1) {
		   tmp |= 1;
	    }

	    اگر (((idx-base)&7)==0){
		  /* Skip leading nulls. */
		  اگر (tmp == 0 && cp == rbuf)
			ادامه;

		  /* Nulls in the middle get turned into spaces. */
		  *cp++ = tmp ? tmp : ' ';
		  tmp = 0;
	    }
      }
      *cp++ = 0;

      vp->value.str = rbuf;
}

ایستا تهی format_vpiScalarVal(ثابت vvp_signal_مقدار*sig, صحیح base,
                                s_vpi_مقدار*vp)
{
      اگر (base >= 0 && base < (signed)sig->مقدار_size()) {
	    انتخاب (sig->value(base)) {
		حالت BIT4_0:
		  vp->value.scalar = vpi0;
		  بشکن;
		حالت BIT4_1:
		  vp->value.scalar = vpi1;
		  بشکن;
		حالت BIT4_X: {
		  vvp_scalar_t strn = sig->scalar_مقدار(base);
		  اگر (strn.strength0() == 1) vp->value.scalar = vpiH;
		  وگرنه اگر (strn.strength1() == 1) vp->value.scalar = vpiL;
		  وگرنه vp->value.scalar = vpiX;
		  بشکن;
		}
		حالت BIT4_Z:
		  vp->value.scalar = vpiZ;
		  بشکن;
	    }
      } وگرنه {
	    vp->value.scalar = vpiX;
      }
}

ایستا تهی format_vpiStrengthVal(ثابت vvp_signal_مقدار*sig, صحیح base,
                                  unsigned wid, s_vpi_مقدار*vp)
{
      long end = base + (signed)wid;
      s_vpi_strengthval*op;

      op = static_cast<s_vpi_strengthval*>
           (need_result_buf(wid * اندازه(s_vpi_strengthval), RBUF_VAL));

      برای (long idx = base ;  idx < end ;  idx += 1) {
	    اگر (idx >=0 && idx < (signed)sig->مقدار_size()) {
		  vvp_scalar_t val = sig->scalar_مقدار(idx);

		  /* vvp_scalar_t strengths are 0-7, but the vpi strength
		     is bit0-bit7. This gets the vpi form of the strengths
		     from the vvp_scalar_t strengths. */
		  unsigned s0 = 1 << val.strength0();
		  unsigned s1 = 1 << val.strength1();

		  انتخاب (val.value()) {
		      حالت BIT4_0:
			op[idx-base].logic = vpi0;
			op[idx-base].s0 = s0|s1;
			op[idx-base].s1 = 0;
			بشکن;

		      حالت BIT4_1:
			op[idx-base].logic = vpi1;
			op[idx-base].s0 = 0;
			op[idx-base].s1 = s0|s1;
			بشکن;

		      حالت BIT4_X:
			op[idx-base].logic = vpiX;
			op[idx-base].s0 = s0;
			op[idx-base].s1 = s1;
			بشکن;

		      حالت BIT4_Z:
			op[idx-base].logic = vpiZ;
			op[idx-base].s0 = vpiHiZ;
			op[idx-base].s1 = vpiHiZ;
			بشکن;
		  }
	    } وگرنه {
		  op[idx-base].logic = vpiX;
		  op[idx-base].s0 = vpiStrongDrive;
		  op[idx-base].s1 = vpiStrongDrive;
	    }
      }

      vp->value.strength = op;
}

ایستا تهی format_vpiVectorVal(ثابت vvp_signal_مقدار*sig, صحیح base, unsigned wid,
                                s_vpi_مقدار*vp)
{
      long end = base + (signed)wid;
      unsigned صحیح obit = 0;
      unsigned hwid = (wid + 31)/32;

      s_vpi_vecval *op = static_cast<p_vpi_vecval>
                         (need_result_buf(hwid * اندازه(s_vpi_vecval), RBUF_VAL));
      vp->value.vector = op;

      op->aval = op->bval = 0;
      برای (long idx = base ;  idx < end ;  idx += 1) {
	    اگر (base >= 0 && base < (signed)sig->مقدار_size()) {
		انتخاب (sig->value(idx)) {
		حالت BIT4_0:
		  op->aval &= ~(1 << obit);
		  op->bval &= ~(1 << obit);
		  بشکن;
		حالت BIT4_1:
		  op->aval |= (1 << obit);
		  op->bval &= ~(1 << obit);
		  بشکن;
		حالت BIT4_X:
		  op->aval |= (1 << obit);
		  op->bval |= (1 << obit);
		  بشکن;
		حالت BIT4_Z:
		  op->aval &= ~(1 << obit);
		  op->bval |= (1 << obit);
		  بشکن;
		}
	    } وگرنه {  /* BIT4_X */
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
ایستا صحیح signal_get(صحیح code, vpiHandle ref)
{
      ساختار __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      أكد(rfp);

      انتخاب (code) {
	  حالت vpi_سطر_No:
	    بازگشت 0;  // Not implemented for now!

	  حالت vpiSigned:
	    بازگشت rfp->signed_flag != 0;

	  حالت vpiArray:
	    بازگشت rfp->is_netarray != 0;

	  حالت vpiIndex: // This only works while we have a single index.
	    اگر (rfp->is_netarray) {
		  s_vpi_مقدار vp;
		  vp.format = vpiIntVal;
		  vpi_get_مقدار(rfp->id.index, &vp);
		  بازگشت vp.value.integer;
	    } وگرنه {
		  بازگشت vpiUndefined;
	    }

	  حالت vpiSize:
	    بازگشت rfp->width();

	  حالت vpiNetType:
	    اگر (ref->get_type_code()==vpiNet)
		  بازگشت vpiWire;
	    وگرنه
		  بازگشت vpiUndefined;

	  حالت vpi_چپ_Range:
            بازگشت rfp->msb.get_مقدار();

	  حالت vpi_راست_Range:
            بازگشت rfp->lsb.get_مقدار();

	  حالت vpiScalar:
	    بازگشت (rfp->msb.get_مقدار() == 0 && rfp->lsb.get_مقدار() == 0);
	  حالت vpiVector:
	    بازگشت (rfp->msb.get_مقدار() != rfp->lsb.get_مقدار());

          حالت vpiAutomatic:
            بازگشت vpip_scope(rfp)->is_automatic() ? 1  : 0;

#ifdef BR916_STOPGAP_FIX
          حالت _vpiFromThr:
            بازگشت _vpiNoThr;
#endif

	  پیشفرض:
	    fprintf(stderr, "VPI error: unknown signal_get property %d.\n",
	            code);
	    بازگشت vpiUndefined;
      }
}

/*
 * implement vpi_get64 for vpiReg objects.
 */
ایستا int64_t signal_get64(صحیح code, vpiHandle ref)
{
      ساختار __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      أكد(rfp);

      انتخاب (code) {
	    // This private property must return zero when undefined.
	  حالت _vpiNexusId:
	    اگر (rfp->msb.get_مقدار() == rfp->lsb.get_مقدار())
		  بازگشت reinterpret_cast<int64_t>(rfp->گره);
	    بازگشت 0;

	  پیشفرض:
	    fprintf(stderr, "VPI error: unknown signal_get64 property %d.\n",
	            code);
	    بازگشت vpiUndefined;
      }
}

ایستا نویسه* signal_get_str(صحیح code, vpiHandle ref)
{
      ساختار __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      أكد(rfp);

      اگر (code == vpiFile) {  // Not implemented for now!
	    بازگشت simple_set_rbuf_str(file_names[0]);
      }

      اگر ((code != vpiName) && (code != vpiFullName)) بازگشت پوچ;

      نویسه *nm;
      ثابت نویسه *ixs;
      اگر (rfp->is_netarray) {
	    nm = strdup(vpi_get_str(vpiName, rfp->within.parent));
	    s_vpi_مقدار vp;
	    vp.format = vpiDecStrVal;
	    vpi_get_مقدار(rfp->id.index, &vp);
	    ixs = vp.value.str;  /* do I need to strdup() this? */
      } وگرنه {
	    nm = strdup(rfp->id.name);
	    ixs = پوچ;
      }

	/* The scope information is added here for vpiFullName. */
      نویسه *rbuf = generic_get_str(code, vpip_scope(rfp), nm, ixs);
      آزاد(nm);
      بازگشت rbuf;
}

ایستا vpiHandle signal_get_handle(صحیح code, vpiHandle ref)
{
      ساختار __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      أكد(rfp);

      انتخاب (code) {

	  حالت vpiParent:
	    بازگشت rfp->is_netarray? rfp->within.parent : پوچ;

	  حالت vpiIndex:
	    بازگشت rfp->is_netarray? rfp->id.index : پوچ;

	  حالت vpi_چپ_Range:
	    بازگشت &rfp->msb;
	  حالت vpi_راست_Range:
	    بازگشت &rfp->lsb;

	  حالت vpiScope:
	    بازگشت vpip_scope(rfp);

	  حالت vpiModule:
	    بازگشت vpip_module(vpip_scope(rfp));
      }

      بازگشت 0;
}

ایستا vpiHandle signal_iterate(صحیح code, vpiHandle ref)
{
      ساختار __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      أكد(rfp);

      اگر (code == vpiIndex) {
	    بازگشت rfp->is_netarray ? rfp->id.index->vpi_iterate(code) : پوچ;
      }

      بازگشت 0;
}

تهی __vpiSignal::make_bits()
{
      أكد(bits == پوچ);
      bits = جدید ساختار __vpiBit[width() + 1];

	// Make word[-1] point to the parent.
      bits->parent = این;
	// Now point to word[0].
      bits += 1;

	// Update each bit to point to the base
      ساختار __vpiBit*base = bits;
      برای (unsigned idx = 0; idx < width(); idx += 1) {
	    base[idx].bit0 = base;
	    صحیح real_idx;
	    اگر (msb.get_مقدار() >= lsb.get_مقدار()) {
		  real_idx = idx + lsb.get_مقدار();
	    } وگرنه {
		  real_idx = lsb.get_مقدار() - idx;
	    }
	    base[idx].index = جدید __vpiDecConst(real_idx);
      }
}

vpiHandle __vpiSignal::get_index(صحیح idx)
{
	/* Check to see if the index is in range. */
      اگر (msb.get_مقدار() >= lsb.get_مقدار()) {
	    اگر ((idx > msb.get_مقدار()) || (idx < lsb.get_مقدار())) بازگشت 0;
      } وگرنه {
	    اگر ((idx < msb.get_مقدار()) || (idx > lsb.get_مقدار())) بازگشت 0;
      }

	/* Normalize the index */
      unsigned norm_idx;
      اگر (msb.get_مقدار() >= lsb.get_مقدار()) {
	    norm_idx = idx - lsb.get_مقدار();
      } وگرنه {
	    norm_idx = lsb.get_مقدار() - idx;
      }

      اگر (bits == پوچ) make_bits();

      بازگشت &(bits[norm_idx].as_bit);
}

تهی __vpiSignal::get_bit_مقدار(ثابت ساختار __vpiBit*bit, p_vpi_مقدار vp)
{
      unsigned index = bit->get_norm_index();

      vvp_signal_مقدار*vsig = dynamic_cast<vvp_signal_مقدار*>(گره->fil);
      أكد(vsig);

      اگر (vp->format == vpiObjTypeVal) {
	    vp->format = vpiIntVal;
      }

      انتخاب (vp->format) {
	حالت vpiBinStrVal:
            format_vpiBinStrVal(vsig, index, 1, vp);
	    بشکن;

	حالت vpiOctStrVal:
            format_vpiOctStrVal(vsig, index, 1, vp);
	    بشکن;

	حالت vpiDecStrVal:
            format_vpiDecStrVal(vsig, index, 1, نادرست, vp);
	    بشکن;

	حالت vpiHexStrVal:
            format_vpiHexStrVal(vsig, index, 1, vp);
	    بشکن;

	حالت vpiStringVal:
            format_vpiStringVal(vsig, index, 1, vp);
	    بشکن;

	حالت vpiIntVal:
            format_vpiIntVal(vsig, index, 1, نادرست, vp);
	    بشکن;

	حالت vpiRealVal:
            format_vpiRealVal(vsig, index, 1, نادرست, vp);
	    بشکن;

	حالت vpiScalarVal:
            format_vpiScalarVal(vsig, index, vp);
	    بشکن;

        حالت vpiStrengthVal:
            format_vpiStrengthVal(vsig, index, 1, vp);
            بشکن;

	حالت vpiVectorVal:
            format_vpiVectorVal(vsig, index, 1, vp);
	    بشکن;

	پیشفرض:
	    fprintf(stderr, "internal error: get_value() "
		    "type %d is not implemented for bit "
		    "select %s[%d] in scope %s.\n",
		    (صحیح)vp->format, vpi_get_str(vpiName),
		    bit->get_index(),
	            vpip_scope(این)->scope_name());
	    أكد(0);
      };
}

vpiHandle __vpiSignal::put_bit_مقدار(ساختار __vpiBit*bit, p_vpi_مقدار vp, صحیح flags)
{
      unsigned index = bit->get_norm_index();
      vvp_net_ptr_t dest(گره, 0);
      vvp_vector4_t val = vec4_from_vpi_مقدار(vp, 1);

      اگر ((flags == vpiForceFlag) || (flags == vpiReleaseFlag)) {
	    fprintf(stderr, "Sorry: vpi_put_value() for %s does not "
	                    "currently support force/release.\n",
	                    bit->as_bit.vpi_get_str(vpiFullName));
	    بازگشت پوچ;
      }

      اگر ((get_type_code() == vpiNet) &&
          !dynamic_cast<vvp_island_port*>(گره->fun)) {
	    گره->send_vec4_pv(val, index, width(),
	                            vthread_get_wt_context());
      } وگرنه {
	    vvp_send_vec4_pv(dest, val, index, width(),
	                     vthread_get_wt_context());
      }

	// This is not a scheduled event so there is no event to return
      بازگشت پوچ;
}

ایستا vpiHandle signal_index(صحیح idx, vpiHandle ref)
{
      ساختار __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      أكد(rfp);

	/* We can only get the bit for a net or reg. */
      PLI_INT32 type = vpi_get(vpiType, ref);
      اگر ((type != vpiNet) && (type != vpiReg)) بازگشت 0;

      بازگشت rfp->get_index(idx);
}

unsigned __vpiSignal::width(تهی) ثابت
{
      unsigned wid = (msb.get_مقدار() >= lsb.get_مقدار())
	    ? (msb.get_مقدار() - lsb.get_مقدار() + 1)
	    : (lsb.get_مقدار() - msb.get_مقدار() + 1);

      بازگشت wid;
}

/*
 * The get_value method reads the values of the functors and returns
 * the vector to the caller. This causes no side-effect, and reads the
 * variables like a %load would.
 */
ایستا تهی signal_get_مقدار(vpiHandle ref, s_vpi_مقدار*vp)
{
      ساختار __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      أكد(rfp);

      unsigned wid = rfp->width();

      vvp_signal_مقدار*vsig = dynamic_cast<vvp_signal_مقدار*>(rfp->گره->fil);
      أكد(vsig);

      انتخاب (vp->format) {

	  حالت vpiIntVal:
	    format_vpiIntVal(vsig, 0, wid, rfp->signed_flag, vp);
	    بشکن;

	  حالت vpiScalarVal:
	    format_vpiScalarVal(vsig, 0, vp);
	    بشکن;

	  حالت vpiStrengthVal:
	    format_vpiStrengthVal(vsig, 0, wid, vp);
	    بشکن;

	  حالت vpiBinStrVal:
	    format_vpiBinStrVal(vsig, 0, wid, vp);
	    بشکن;

	  حالت vpiHexStrVal:
	    format_vpiHexStrVal(vsig, 0, wid, vp);
	    بشکن;

	  حالت vpiOctStrVal:
	    format_vpiOctStrVal(vsig, 0, wid, vp);
	    بشکن;

	  حالت vpiDecStrVal:
	    format_vpiDecStrVal(vsig, 0, wid, rfp->signed_flag, vp);
	    بشکن;

	  حالت vpiStringVal:
	    format_vpiStringVal(vsig, 0, wid, vp);
	    بشکن;

	  حالت vpiVectorVal:
	    format_vpiVectorVal(vsig, 0, wid, vp);
	    بشکن;

	  حالت vpiRealVal:
	    format_vpiRealVal(vsig, 0, wid, rfp->signed_flag, vp);
	    بشکن;

	  حالت vpiObjTypeVal:
	    اگر (wid == 1) {
		  vp->format = vpiScalarVal;
		  format_vpiScalarVal(vsig, 0, vp);
	    } وگرنه {
		  vp->format = vpiVectorVal;
		  format_vpiVectorVal(vsig, 0, wid, vp);
	    }
	    بشکن;

	  پیشفرض:
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

ایستا vvp_vector4_t from_stringval(ثابت نویسه*str, unsigned wid)
{
      unsigned idx;
      ثابت نویسه*cp;

      cp = str + strlen(str);
      idx = 0;

      vvp_vector4_t val(wid, BIT4_0);

      وقتی ((idx < wid) && (cp > str)) {
	    unsigned byte = *--cp;
	    صحیح bit;

	    برای (bit = 0 ;  (bit < 8) && (idx < wid) ;  bit += 1) {
		  اگر (byte & 1)
			val.set_bit(idx, BIT4_1);

		  byte >>= 1;
		  idx += 1;
	    }
      }

      بازگشت val;
}

ایستا vpiHandle signal_put_مقدار(vpiHandle ref, s_vpi_مقدار*vp, صحیح flags)
{
      unsigned wid;
      ساختار __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      أكد(rfp);
      vvp_net_ptr_t dest(rfp->گره, 0);

      منطقی net_flag = ref->get_type_code()==vpiNet;

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value. Note that
	   if this net is attached to an island, we need to rerun
	   the calculations immediately so we can return the
	   released value. */
      اگر (flags == vpiReleaseFlag) {
	    أكد(rfp->گره->fil);
	    rfp->گره->fil->force_unlink();
	    rfp->گره->fil->release(dest, net_flag);
	    rfp->گره->fun->force_flag(درست);
	    signal_get_مقدار(ref, vp);
	    بازگشت ref;
      }

	/* Make a vvp_vector4_t vector to receive the translated value
	   that we are going to poke. This will get populated
	   differently depending on the format. */
      wid = (rfp->msb.get_مقدار() >= rfp->lsb.get_مقدار())
	    ? (rfp->msb.get_مقدار() - rfp->lsb.get_مقدار() + 1)
	    : (rfp->lsb.get_مقدار() - rfp->msb.get_مقدار() + 1);

      vvp_vector4_t val = vec4_from_vpi_مقدار(vp, wid);

      اگر (flags == vpiForceFlag) {
	    vvp_vector2_t mask (vvp_vector2_t::FILL1, wid);
	    rfp->گره->force_vec4(val, mask);
      } وگرنه اگر (net_flag && !dynamic_cast<vvp_island_port*>(rfp->گره->fun)) {
	    rfp->گره->send_vec4(val, vthread_get_wt_context());
      } وگرنه {
	    vvp_send_vec4(dest, val, vthread_get_wt_context());
      }
      بازگشت ref;
}

vvp_vector4_t vec4_from_vpi_مقدار(s_vpi_مقدار*vp, unsigned wid)
{
      vvp_vector4_t val (wid, BIT4_0);

      انتخاب (vp->format) {

	  حالت vpiIntVal: {
		long vpi_val = vp->value.integer;
		برای (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		      vvp_bit4_t bit = (vpi_val & 1) ? BIT4_1 : BIT4_0;
		      val.set_bit(idx, bit);
		      vpi_val >>= 1;
		}
		بشکن;
	  }

	  حالت vpiVectorVal:
	    برای (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		  unsigned long aval = vp->value.vector[idx/32].aval;
		  unsigned long bval = vp->value.vector[idx/32].bval;
		  aval >>= idx%32;
		  bval >>= idx%32;
		  صحیح bitmask = (aval&1) | ((bval<<1)&2);
		  vvp_bit4_t bit = scalar_to_bit4(bitmask);
		  val.set_bit(idx, bit);
	    }
	    بشکن;
	  حالت vpiBinStrVal:
	    vpip_bin_str_to_vec4(val, vp->value.str);
	    بشکن;
	  حالت vpiOctStrVal:
	    vpip_oct_str_to_vec4(val, vp->value.str);
	    بشکن;
	  حالت vpiDecStrVal:
	    vpip_dec_str_to_vec4(val, vp->value.str);
	    بشکن;
	  حالت vpiHexStrVal:
	    vpip_hex_str_to_vec4(val, vp->value.str);
	    بشکن;
	  حالت vpiScalarVal:
	    val.set_bit(0, scalar_to_bit4(vp->value.scalar));
	    بشکن;
	  حالت vpiStringVal:
	    val = from_stringval(vp->value.str, wid);
	    بشکن;
	  حالت vpiRealVal:
	    val = vvp_vector4_t(wid, vp->value.real);
	    بشکن;

	  پیشفرض:
	    fprintf(stderr, "vvp internal error: put_value: "
		    "value type %d not implemented here.\n",
		    (صحیح)vp->format);
	    أكد(0);

      }

      بازگشت val;
}

صحیح __vpiSignal::vpi_get(صحیح code)
{ بازگشت signal_get(code, این); }

int64_t __vpiSignal::vpi_get64(صحیح code)
{ بازگشت signal_get64(code, این); }

نویسه* __vpiSignal::vpi_get_str(صحیح code)
{ بازگشت signal_get_str(code, این); }

تهی __vpiSignal::vpi_get_مقدار(p_vpi_مقدار val)
{ signal_get_مقدار(این, val); }

vpiHandle __vpiSignal::vpi_put_مقدار(p_vpi_مقدار val, صحیح flags)
{ بازگشت signal_put_مقدار(این, val, flags); }

vpiHandle __vpiSignal::vpi_handle(صحیح code)
{ بازگشت signal_get_handle(code, این); }

vpiHandle __vpiSignal::vpi_iterate(صحیح code)
{ بازگشت signal_iterate(code, این); }

vpiHandle __vpiSignal::vpi_index(صحیح idx)
{ بازگشت signal_index(idx, این); }

ساختار signal_reg : عمومی __vpiSignal {
      درونخطی signal_reg() { }
      صحیح get_type_code(تهی) ثابت override { بازگشت vpiReg; }
};

ساختار signal_integer : عمومی __vpiSignal {
      درونخطی signal_integer() { }
      صحیح get_type_code(تهی) ثابت override { بازگشت vpiIntegerVar; }
};

ساختار signal_net : عمومی __vpiSignal {
      درونخطی signal_net() { }
      صحیح get_type_code(تهی) ثابت override { بازگشت vpiNet; }
};

ساختار signal_byte : عمومی __vpiSignal {
      درونخطی signal_byte() { }
      صحیح get_type_code(تهی) ثابت override { بازگشت vpiByteVar; }
};

ساختار signal_bitvar : عمومی __vpiSignal {
      درونخطی signal_bitvar() { }
      صحیح get_type_code(تهی) ثابت override { بازگشت vpiBitVar; }
};

ساختار signal_shortint : عمومی __vpiSignal {
      درونخطی signal_shortint() { }
      صحیح get_type_code(تهی) ثابت override { بازگشت vpiShortIntVar; }
};

ساختار signal_int : عمومی __vpiSignal {
      درونخطی signal_int() { }
      صحیح get_type_code(تهی) ثابت override { بازگشت vpiIntVar; }
};

ساختار signal_longint : عمومی __vpiSignal {
      درونخطی signal_longint() { }
      صحیح get_type_code(تهی) ثابت override { بازگشت vpiLongIntVar; }
};


/*
 * Construct a vpiIntegerVar object. Indicate the type using a flag
 * to minimize the code modifications. Icarus implements integers
 * as 'reg signed [31:0]'.
 */
vpiHandle vpip_make_int4(ثابت نویسه*name, صحیح msb, صحیح lsb, vvp_net_t*vec)
{
      __vpiSignal*obj = جدید signal_integer;
      بازگشت fill_in_var4(obj, name, msb, lsb, درست, vec);
}

/*
 * Construct the two-state SystemVerilog variables.
 */
vpiHandle vpip_make_int2(ثابت نویسه*name, صحیح msb, صحیح lsb, منطقی signed_flag,
                         vvp_net_t*vec)
{
      __vpiSignal*obj;

	// All unsigned 2-state variables are a vpiBitVar. All 2-state
	// variables with a non-zero lsb are also a vpiBitVar.
      اگر ((! signed_flag) || (lsb != 0) ) {
	    obj = جدید signal_bitvar;
      } وگرنه {
	      // These could also be bit declarations with matching
	      // information, but for now they get the apparent type.
	    انتخاب (msb) {
		حالت 7:
		  obj = جدید signal_byte;
		  بشکن;
		حالت 15:
		  obj = جدید signal_shortint;
		  بشکن;
		حالت 31:
		  obj = جدید signal_int;
		  بشکن;
		حالت 63:
		  obj = جدید signal_longint;
		  بشکن;
		پیشفرض:
		    // Every other type of bit vector is a vpiBitVar with
		    // array dimensions.
		  obj = جدید signal_bitvar;
		  بشکن;
	    }
      }

      بازگشت fill_in_var4(obj, name, msb, lsb, signed_flag, vec);
}

/*
 * Construct a vpiReg/vpiLogicVar object. It's like a net, except for the type.
 */
vpiHandle vpip_make_var4(ثابت نویسه*name, صحیح msb, صحیح lsb,
			منطقی signed_flag, vvp_net_t*vec)
{
      __vpiSignal*obj = جدید signal_reg;
      بازگشت fill_in_var4(obj, name, msb, lsb, signed_flag, vec);
}

#ifdef CHECK_WITH_VALGRIND
ایستا ساختار vpiSignal_plug **signal_pool = 0;
ایستا unsigned signal_pool_شمار = 0;
ایستا unsigned long signal_شمار = 0;
ایستا unsigned long signal_dels = 0;
#endif

ساختار vpiSignal_plug {
      unsigned نویسه space[اندازه (ساختار __vpiSignal)];
#ifdef CHECK_WITH_VALGRIND
      ساختار vpiSignal_plug *pool;
#endif
};

تهی* __vpiSignal::operator جدید(size_t siz)
{
#ifdef CHECK_WITH_VALGRIND
      أكد(siz == اندازه(ساختار vpiSignal_plug) - اندازه(ساختار vpiSignal_plug *));
#وگرنه
      أكد(siz == اندازه(ساختار vpiSignal_plug));
#endif
      ایستا ساختار vpiSignal_plug*alloc_array = 0;
      ایستا unsigned alloc_index = 0;
      ثابت unsigned alloc_شمار = 512;

      اگر ((alloc_array == 0) || (alloc_index == alloc_شمار)) {
	    alloc_array = static_cast<ساختار vpiSignal_plug*>
	                  (calloc(alloc_شمار, اندازه(ساختار vpiSignal_plug)));
	    alloc_index = 0;
#ifdef CHECK_WITH_VALGRIND
	    VALGRIND_MAKE_MEM_NOACCESS(alloc_array, alloc_شمار *
	                                            اندازه(ساختار vpiSignal_plug));
	    VALGRIND_CREATE_MEMPOOL(alloc_array, 0, 1);
	    signal_pool_شمار += 1;
	    signal_pool = static_cast<vpiSignal_plug **>
	                  (realloc(signal_pool,
	                   signal_pool_شمار*اندازه(vpiSignal_plug **)));
	    signal_pool[signal_pool_شمار-1] = alloc_array;
#endif
      }

      ساختار vpiSignal_plug*cur = alloc_array + alloc_index;
#ifdef CHECK_WITH_VALGRIND
      VALGRIND_MEMPOOL_ALLOC(alloc_array, cur, اندازه(ساختار vpiSignal_plug));
      cur->pool = alloc_array;
      signal_شمار += 1;
#endif
      alloc_index += 1;
      بازگشت cur;
}

تهی __vpiSignal::operator delete(تهی*)
{
      أكد(0);
}

#ifdef CHECK_WITH_VALGRIND
تهی signal_delete(vpiHandle item)
{
      ساختار __vpiSignal *obj = static_cast<__vpiSignal *> (item);
      أكد(obj->گره->fil);
      obj->گره->fil->clear_all_callbacks();
      vvp_net_delete(obj->گره);
      اگر (obj->bits) {
	    برای (unsigned idx=0; idx<obj->width(); idx+=1) {
		  delete obj->bits[idx].index;
	    }
	    obj->bits -= 1;
	    delete [] obj->bits;
      }
      signal_dels += 1;
      VALGRIND_MEMPOOL_FREE(reinterpret_cast<vpiSignal_plug *>(obj)->pool, obj);
}

تهی signal_pool_delete()
{
      اگر (RUNNING_ON_VALGRIND && (signal_شمار != signal_dels)) {
	    fflush(پوچ);
	    VALGRIND_PRINTF("Error: vvp missed deleting %ld of %lu signal(s).",
	                    (long) signal_شمار - signal_dels, signal_شمار);
      }

      برای (unsigned idx = 0; idx < signal_pool_شمار; idx += 1) {
	    VALGRIND_DESTROY_MEMPOOL(signal_pool[idx]);
	    آزاد(signal_pool[idx]);
      }

      آزاد(signal_pool);
      signal_pool = 0;
      signal_pool_شمار = 0;
}
#endif

/*
 * Construct a vpiNet object. Give the object specified dimensions,
 * and point to the specified functor for the lsb.
 *
 * The name is the PLI name for the object. If it is an array it is
 * <name>[<index>].
 */
ایستا vpiHandle fill_in_net4(ساختار __vpiSignal*obj, __vpiScope*scope,
			      ثابت نویسه*name, صحیح msb, صحیح lsb,
			      منطقی signed_flag, vvp_net_t*گره)
{
      obj->id.name = name? vpip_name_string(name) : پوچ;
      obj->msb = __vpiDecConst(msb);
      obj->lsb = __vpiDecConst(lsb);
      obj->signed_flag = signed_flag? 1 : 0;
      obj->is_netarray = 0;
      obj->گره = گره;

	// Place this object within a scope. If this object is
	// attached to an array, then this value will be replaced with
	// the handle to the parent.
      obj->within.scope = scope;

      شمار_vpi_nets += 1;

      بازگشت obj;
}

vpiHandle vpip_make_net4(__vpiScope*scope,
			 ثابت نویسه*name, صحیح msb, صحیح lsb,
			 منطقی signed_flag, vvp_net_t*گره)
{
      ساختار __vpiSignal*obj = جدید signal_net;
      بازگشت fill_in_net4(obj, scope, name, msb, lsb, signed_flag, گره);
}

ایستا صحیح PV_get_base(ساختار __vpiPV*rfp)
{
	/* We return from the symbol base if it is defined. */
      اگر (rfp->sbase != 0) {
	    s_vpi_مقدار val;
	      /* Check to see if the value is defined. */
	    val.format = vpiVectorVal;
	    vpi_get_مقدار(rfp->sbase, &val);
	    صحیح واژگان = (vpi_get(vpiSize, rfp->sbase)-1)/32 + 1;
	    برای(صحیح idx = 0; idx < واژگان; idx += 1) {
		    /* Return INT_MIN to indicate an X base. */
		  اگر (val.value.vector[idx].bval != 0) بازگشت INT_MIN;
	    }
	      /* The value is defined so get and return it. */
	    val.format = vpiIntVal;
	    vpi_get_مقدار(rfp->sbase, &val);
	    بازگشت val.value.integer;
      }

	/* If the width is zero then tbase is the constant. */
      بازگشت rfp->tbase;
}

ایستا صحیح PV_get(صحیح code, vpiHandle ref)
{
      ساختار __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      أكد(rfp);

      صحیح rval = 0;
      انتخاب (code) {
	حالت vpi_سطر_No:
	    بازگشت 0;  // Not implemented for now!

	حالت vpiSigned:
	    بازگشت 0;  // A part/bit select is always unsigned!

	حالت vpiSize:
	    بازگشت rfp->width;

	  /* This is like the &A<> in array.cc. */
	حالت vpiConstantSelect:
	    بازگشت rfp->sbase == 0;

	حالت vpi_چپ_Range:
            rval += rfp->width - 1;
	    // fallthrough
	حالت vpi_راست_Range: {
	    صحیح چپ = vpi_get(vpi_چپ_Range, rfp->parent);
	    صحیح راست = vpi_get(vpi_راست_Range, rfp->parent);
	    rval += PV_get_base(rfp);
	    بازگشت چپ >= راست ? راست + rval : راست - rval;
	}

        حالت vpiAutomatic:
            بازگشت vpi_get(vpiAutomatic, rfp->parent);

#اگر defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
        حالت _vpiFromThr:
            بازگشت _vpi_at_PV;
#endif

	پیشفرض:
	    fprintf(stderr, "PV_get: property %d is unknown\n", code);
      }

      بازگشت 0;
}

ایستا نویسه* PV_get_str(صحیح code, vpiHandle ref)
{
      ساختار __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      أكد(rfp);

      انتخاب (code) {
	حالت vpiFile:  // Not implemented for now!
	    بازگشت simple_set_rbuf_str(file_names[0]);

	حالت vpiName:
	حالت vpiFullName: {
	    ثابت نویسه*nm = vpi_get_str(code, rfp->parent);
	    size_t len = 256+strlen(nm);
	    نویسه *full = static_cast<نویسه *>(malloc(len));
	    snprintf(full, len, "%s[%d:%d]", nm,
	                                     (صحیح)vpi_get(vpi_چپ_Range, ref),
	                                     (صحیح)vpi_get(vpi_راست_Range, ref));
	    full[len-1] = 0;
	    نویسه *res = simple_set_rbuf_str(full);
	    آزاد(full);
	    بازگشت res;
	}

	پیشفرض:
	    fprintf(stderr, "PV_get_str: property %d is unknown.\n", code);
      }

      بازگشت 0;
}

ایستا تهی PV_get_مقدار(vpiHandle ref, p_vpi_مقدار vp)
{
      ساختار __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      أكد(rfp);

      vvp_signal_مقدار*sig = dynamic_cast<vvp_signal_مقدار*>(rfp->net->fil);
      أكد(sig);

      انتخاب (vp->format) {

	  حالت vpiIntVal:
	    format_vpiIntVal(sig, PV_get_base(rfp), rfp->width, 0, vp);
	    بشکن;

	  حالت vpiBinStrVal:
	    format_vpiBinStrVal(sig, PV_get_base(rfp), rfp->width, vp);
	    بشکن;

	  حالت vpiOctStrVal:
	    format_vpiOctStrVal(sig, PV_get_base(rfp), rfp->width, vp);
	    بشکن;

	  حالت vpiHexStrVal:
	    format_vpiHexStrVal(sig, PV_get_base(rfp), rfp->width, vp);
	    بشکن;

	  حالت vpiDecStrVal:
	    format_vpiDecStrVal(sig, PV_get_base(rfp), rfp->width, 0, vp);
	    بشکن;

	  حالت vpiStringVal:
	    format_vpiStringVal(sig, PV_get_base(rfp), rfp->width, vp);
	    بشکن;

	  حالت vpiScalarVal:
	    format_vpiScalarVal(sig, PV_get_base(rfp), vp);
	    بشکن;

	  حالت vpiStrengthVal:
	    format_vpiStrengthVal(sig, PV_get_base(rfp), rfp->width, vp);
	    بشکن;

	  حالت vpiVectorVal:
	    format_vpiVectorVal(sig, PV_get_base(rfp), rfp->width, vp);
	    بشکن;

	  حالت vpiRealVal:
	    format_vpiRealVal(sig, PV_get_base(rfp), rfp->width, 0, vp);
	    بشکن;

	  پیشفرض:
	    fprintf(stderr, "vvp internal error: PV_get_value: "
		    "value type %d not implemented. Signal is %s.\n",
		    (صحیح)vp->format, vpi_get_str(vpiFullName, rfp->parent));
	    أكد(0);
      }
}

ایستا vpiHandle PV_put_مقدار(vpiHandle ref, p_vpi_مقدار vp, صحیح flags)
{
      ساختار __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      أكد(rfp);
      vvp_signal_مقدار*sig = dynamic_cast<vvp_signal_مقدار*>(rfp->net->fil);
      أكد(sig);

      unsigned sig_size = sig->مقدار_size();
      unsigned width = rfp->width;
      صحیح base = PV_get_base(rfp);
      اگر (base >= (signed) sig_size) بازگشت 0;
      اگر (base + (signed) width < 0) بازگشت 0;

      vvp_vector4_t val;
      اگر (flags != vpiReleaseFlag) {
	    val = vec4_from_vpi_مقدار(vp, width);
      }

	/*
	 * If the base is less than zero then trim off any unneeded
	 * lower bits.
	 */
      اگر (base < 0) {
	    width += base;
	    اگر (flags != vpiReleaseFlag) {
		  val = val.subvalue(-base, width);
	    }
	    base = 0;
      }

	/*
	 * If the value is wider than the signal then trim off any
	 * unneeded upper bits.
	 */
      اگر (base+width > sig_size) {
	    width = sig_size - base;
	    اگر (flags != vpiReleaseFlag) {
		  val = val.subvalue(0, width);
	    }
      }

      أكد(rfp->parent);
      منطقی net_flag = rfp->parent->get_type_code()==vpiNet;
      منطقی full_sig = base == 0 && width == sig_size;

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
	    اگر (full_sig) {
		  rfp->net->fil->release(dest, net_flag);
	    } وگرنه {
		  rfp->net->fil->release_pv(dest, base, width, net_flag);
	    }
	    rfp->net->fun->force_flag(درست);
	    PV_get_مقدار(ref, vp);
	    بازگشت ref;
      }

      اگر (flags == vpiForceFlag) {
	    اگر (full_sig) {
		  vvp_vector2_t mask (vvp_vector2_t::FILL1, sig_size);
		  rfp->net->force_vec4(val, mask);
	    } وگرنه {
		  vvp_vector2_t mask (vvp_vector2_t::FILL0, sig_size);
		  برای (unsigned idx = 0 ; idx < width ; idx += 1)
		        mask.set_bit(base+idx, 1);

		  vvp_vector4_t tmp (sig_size, BIT4_Z);

		    // vvp_net_t::force_vec4 propagates all the bits of the
		    // forced vector value, regardless of the mask. This
		    // ensures the unforced bits retain their current value.
		  sig->vec4_مقدار(tmp);

		  tmp.set_vec(base, val);
		  rfp->net->force_vec4(tmp, mask);
	    }
      } وگرنه اگر (net_flag && !dynamic_cast<vvp_island_port*>(rfp->net->fun)) {
	    اگر (full_sig) {
		  rfp->net->send_vec4(val, vthread_get_wt_context());
	    } وگرنه {
		  rfp->net->send_vec4_pv(val, base, sig_size,
		                         vthread_get_wt_context());
	    }
      } وگرنه {
	    اگر (full_sig) {
		  vvp_send_vec4(dest, val, vthread_get_wt_context());
	    } وگرنه {
		  vvp_send_vec4_pv(dest, val, base, sig_size,
	                           vthread_get_wt_context());
	    }
      }
      بازگشت 0;
}

ایستا vpiHandle PV_get_handle(صحیح code, vpiHandle ref)
{
      ساختار __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      أكد(rfp);

      انتخاب (code) {
	  حالت vpiParent:
	    بازگشت rfp->parent;

	  حالت vpiScope:
	    بازگشت vpi_handle(vpiScope, rfp->parent);

	  حالت vpiModule:
	    بازگشت vpi_handle(vpiModule, rfp->parent);
      }

      بازگشت 0;
}

درونخطی __vpiPV::__vpiPV()
{ }

صحیح __vpiPV::get_type_code(تهی) ثابت
{ بازگشت vpiPartSelect; }

صحیح __vpiPV::vpi_get(صحیح code)
{ بازگشت PV_get(code, این); }

نویسه* __vpiPV::vpi_get_str(صحیح code)
{ بازگشت PV_get_str(code, این); }

تهی __vpiPV::vpi_get_مقدار(p_vpi_مقدار val)
{ PV_get_مقدار(این, val); }

vpiHandle __vpiPV::vpi_put_مقدار(p_vpi_مقدار val, صحیح flags)
{ بازگشت PV_put_مقدار(این, val, flags); }

vpiHandle __vpiPV::vpi_handle(صحیح code)
{ بازگشت PV_get_handle(code, این); }

vpiHandle vpip_make_PV(نویسه*var, صحیح base, صحیح width)
{
      ساختار __vpiPV*obj = جدید __vpiPV;
      compile_vpi_lookup(&obj->parent, strdup(var));
      obj->sbase = 0;
      obj->tbase = base;
      obj->width = (unsigned) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      بازگشت obj;
}

vpiHandle vpip_make_PV(نویسه*var, نویسه*نماد, صحیح width)
{
      ساختار __vpiPV*obj = جدید __vpiPV;
      compile_vpi_lookup(&obj->parent, strdup(var));
      compile_vpi_lookup(&obj->sbase, نماد);
      obj->tbase = 0;
      obj->width = (unsigned) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      بازگشت obj;
}

vpiHandle vpip_make_PV(نویسه*var, vpiHandle handle, صحیح width)
{
      ساختار __vpiPV*obj = جدید __vpiPV;
      compile_vpi_lookup(&obj->parent, strdup(var));
      obj->sbase = handle;
      obj->tbase = 0;
      obj->width = (unsigned) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      بازگشت obj;
}

#ifdef CHECK_WITH_VALGRIND
تهی PV_delete(vpiHandle item)
{
      ساختار __vpiPV *obj = dynamic_cast<__vpiPV*>(item);
      اگر (obj->sbase) {
	    انتخاب (obj->sbase->get_type_code()) {
		حالت vpiMemoryWord:
		  اگر (vpi_get(_vpiFromThr, obj->sbase) == _vpi_at_A) {
			A_delete(obj->sbase);
		  }
		  بشکن;
		حالت vpiPartSelect:
		  أكد(vpi_get(_vpiFromThr, obj->sbase) == _vpi_at_PV);
		  PV_delete(obj->sbase);
		  بشکن;
	    }
      }
      أكد(obj->net->fil);
      obj->net->fil->clear_all_callbacks();
      delete obj;
}
#endif
