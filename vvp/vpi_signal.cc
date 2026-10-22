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
बाह्य ध्रुव वर्ण hex_digits[256];
बाह्य ध्रुव वर्ण oct_digits[64];

/*
 * The string values need a result buf to hold the results. This
 * buffer can be reused for that purpose. Whenever I have a need, the
 * need_result_buf function makes sure that need can be met.
 */
शून्य *need_result_buf(unsigned cnt, vpi_rbuf_t type)
{
      स्थिर शून्य*result_buf[2] = {0, 0};
      स्थिर size_t result_buf_size[2] = {0, 0};

      सचे (type == RBUF_DEL) {
	    मुक्त(result_buf[RBUF_VAL]);
	    result_buf[RBUF_VAL] = 0;
	    result_buf_size[RBUF_VAL] = 0;

	    मुक्त(result_buf[RBUF_STR]);
	    result_buf[RBUF_STR] = 0;
	    result_buf_size[RBUF_STR] = 0;

	    पटिनिवत्त 0;
      }

      cnt = (cnt + 0x0fff) & ~0x0fff;

      सचे (result_buf_size[type] == 0) {
	    result_buf[type] = malloc(cnt);
	    result_buf_size[type] = cnt;
      } अञ्ञथा सचे (result_buf_size[type] < cnt) {
	    result_buf[type] = realloc(result_buf[type], cnt);
	    result_buf_size[type] = cnt;
      }

      पटिनिवत्त result_buf[type];
}

वर्ण *simple_set_rbuf_str(ध्रुव वर्ण *s1)
{
      वर्ण *res = static_cast<वर्ण *>(need_result_buf(strlen(s1)+1, RBUF_STR));
      सचे (res) strcpy(res,s1);
      पटिनिवत्त res;
}

वर्ण *generic_get_str(पूर्णाङ्क code, vpiHandle ref, ध्रुव वर्ण *name, ध्रुव वर्ण *index)
{
      size_t len = strlen(name) + 1;  /* include space for null termination */
      वर्ण *bn = रिक्त;
      तार्किक is_bn_a_pkg = मिच्छा;
      सचे (code == vpiFullName) {
	    bn = strdup(vpi_get_str(code,ref));
	    size_t bn_len = strlen(bn);
	    is_bn_a_pkg = (bn_len >= 3) && (bn[bn_len-1] == ':') && (bn[bn_len-2] == ':');
	    len += bn_len;
	    सचे (! is_bn_a_pkg) len += 1; // include space for "." separator
      }
      सचे (index != रिक्त) len += strlen(index) + 2;  /* include space for brackets */

      वर्ण *res = static_cast<वर्ण *>(need_result_buf(len, RBUF_STR));
      सचे (!res) {
	    मुक्त(bn);
	    पटिनिवत्त रिक्त;
      }
      *res=0;  /* start with nothing */

	/* if this works, I can make it more efficient later */
      सचे (bn != रिक्त) {
	    strcat(res, bn);
	      // A package already has the "::" separator in the name
	    सचे (! is_bn_a_pkg) strcat(res, ".");
	    मुक्त(bn);
      }
      strcat(res, name);
      सचे (index != रिक्त) {
	    strcat(res, "[");
	    strcat(res, index);
	    strcat(res, "]");
      }
      पटिनिवत्त res;
}

स्थिर vpiHandle fill_in_net4(संरचना __vpiSignal*obj, __vpiScope*scope,
                              ध्रुव वर्ण*name, पूर्णाङ्क msb, पूर्णाङ्क lsb,
                              तार्किक signed_flag, vvp_net_t*ग्रन्थिः);

स्थिर vpiHandle fill_in_var4(संरचना __vpiSignal*obj,
                              ध्रुव वर्ण*name, पूर्णाङ्क msb, पूर्णाङ्क lsb,
                              तार्किक signed_flag, vvp_net_t*ग्रन्थिः)
{
	// Variable declarations are always resolved immediately,
	// so we can assume they belong in the current scope.
      पटिनिवत्त fill_in_net4(obj, vpip_peek_current_scope(),
			  name, msb, lsb, signed_flag, ग्रन्थिः);
}

/*
 * The standard formatting/conversion routines.
 * They work with full or partial signals.
 */

स्थिर शून्य format_vpiBinStrVal(ध्रुव vvp_signal_मानम्*sig, पूर्णाङ्क base, unsigned wid,
                                s_vpi_मानम्*vp)
{
      वर्ण *rbuf = static_cast<वर्ण *>(need_result_buf(wid+1, RBUF_VAL));
      long end = base + (signed)wid;
      long offset = end - 1;
      long ssize = (signed)sig->मानम्_size();

      अत्थाय (long idx = base ;  idx < end ;  idx += 1) {
	    सचे (idx < 0 || idx >= ssize) {
                  rbuf[offset-idx] = 'x';
	    } अञ्ञथा {
                  rbuf[offset-idx] = vvp_bit4_to_ascii(sig->value(idx));
	    }
      }
      rbuf[wid] = 0;

      vp->value.str = rbuf;
}

स्थिर शून्य format_vpiOctStrVal(ध्रुव vvp_signal_मानम्*sig, पूर्णाङ्क base, unsigned wid,
                                s_vpi_मानम्*vp)
{
      unsigned dwid = (wid + 2) / 3;
      वर्ण *rbuf = static_cast<वर्ण *>(need_result_buf(dwid+1, RBUF_VAL));
      long end = base + (signed)wid;
      long ssize = (signed)sig->मानम्_size();
      unsigned val = 0;

      rbuf[dwid] = 0;
      अत्थाय (long idx = base ;  idx < end ;  idx += 1) {
	    unsigned bit = 0;
	    सचे (idx < 0 || idx >= ssize) {
                  bit = 2; // BIT4_X
	    } अञ्ञथा {
                  विकल्प (sig->value(idx)) {
		      पक्ष BIT4_0:
			bit = 0;
			भङ्ग;
		      पक्ष BIT4_1:
			bit = 1;
			भङ्ग;
		      पक्ष BIT4_X:
			bit = 2;
			भङ्ग;
		      पक्ष BIT4_Z:
			bit = 3;
			भङ्ग;
                  }
	    }
	    val |= bit << 2*((idx-base) % 3);

	    सचे ((idx-base) % 3 == 2) {
		dwid -= 1;
		rbuf[dwid] = oct_digits[val];
		val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      विकल्प (wid % 3) {
	  पक्ष 1:
	    सचे (val == 2) val = 42;
	    अञ्ञथा सचे (val == 3) val = 63;
	    भङ्ग;
	  पक्ष 2:
	    सचे (val == 10) val = 42;
	    अञ्ञथा सचे (val == 15) val = 63;
	    भङ्ग;
      }

      सचे (dwid > 0) rbuf[0] = oct_digits[val];

      vp->value.str = rbuf;
}

स्थिर शून्य format_vpiHexStrVal(ध्रुव vvp_signal_मानम्*sig, पूर्णाङ्क base, unsigned wid,
                                s_vpi_मानम्*vp)
{
      unsigned dwid = (wid + 3) / 4;
      वर्ण *rbuf = static_cast<वर्ण *>(need_result_buf(dwid+1, RBUF_VAL));
      long end = base + (signed)wid;
      long ssize = (signed)sig->मानम्_size();
      unsigned val = 0;

      rbuf[dwid] = 0;
      अत्थाय (long idx = base ;  idx < end ;  idx += 1) {
	    unsigned bit = 0;
	    सचे (idx < 0 || idx >= ssize) {
                  bit = 2; // BIT4_X
	    } अञ्ञथा {
                  विकल्प (sig->value(idx)) {
		      पक्ष BIT4_0:
			bit = 0;
			भङ्ग;
		      पक्ष BIT4_1:
			bit = 1;
			भङ्ग;
		      पक्ष BIT4_X:
			bit = 2;
			भङ्ग;
		      पक्ष BIT4_Z:
			bit = 3;
			भङ्ग;
                  }
	    }
	    val |= bit << 2*((idx-base) % 4);

	    सचे ((idx-base) % 4 == 3) {
		dwid -= 1;
		rbuf[dwid] = hex_digits[val];
		val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      विकल्प (wid % 4) {
	  पक्ष 1:
	    सचे (val == 2) val = 170;
	    अञ्ञथा सचे (val == 3) val = 255;
	    भङ्ग;
	  पक्ष 2:
	    सचे (val == 10) val = 170;
	    अञ्ञथा सचे (val == 15) val = 255;
	    भङ्ग;
	  पक्ष 3:
	    सचे (val == 42) val = 170;
	    अञ्ञथा सचे (val == 63) val = 255;
	    भङ्ग;
      }

      सचे (dwid > 0) rbuf[0] = hex_digits[val];

      vp->value.str = rbuf;
}

स्थिर शून्य format_vpiDecStrVal(ध्रुव vvp_signal_मानम्*sig, पूर्णाङ्क base, unsigned wid,
                                पूर्णाङ्क signed_flag, s_vpi_मानम्*vp)
{
      unsigned hwid = (sig->मानम्_size()+2) / 3 + 1;
      वर्ण *rbuf = static_cast<वर्ण *>(need_result_buf(hwid, RBUF_VAL));
      long ssize = (signed)sig->मानम्_size();
      long end = base + (signed)wid;

	/* Do we have an end outside of the real signal vector. */
      सचे (base < 0 || end > ssize) {
	    तार्किक all_x = सच्च;
	    सचे (end > ssize) end = ssize;
	    सचे (base < 0) base = 0;
	    अत्थाय (long idx = base ;  idx < end ;  idx += 1) {
		  सचे (sig->value(idx) != BIT4_X) {
			all_x = मिच्छा;
			भङ्ग;
		  }
	    }

	    सचे (all_x) {
		  rbuf[0] = 'x';
	    } अञ्ञथा {
		  rbuf[0] = 'X';
	    }
	    rbuf[1] = 0;

	    vp->value.str = rbuf;
	    पटिनिवत्त;
      }

      vvp_vector4_t vec4;
      सचे (base == 0 && end == ssize) {
	    sig->vec4_मानम्(vec4);
      } अञ्ञथा {
	    vvp_vector4_t tmp;
	    sig->vec4_मानम्(tmp);
	    vec4 = tmp.subvalue(base, wid);
      }

      vpip_vec4_to_dec_str(vec4, rbuf, hwid, signed_flag);

      vp->value.str = rbuf;
}

स्थिर शून्य format_vpiIntVal(ध्रुव vvp_signal_मानम्*sig, पूर्णाङ्क base, unsigned wid,
                             पूर्णाङ्क signed_flag, s_vpi_मानम्*vp)
{
      vvp_vector4_t tmp;
      sig->vec4_मानम्(tmp);
      vvp_vector4_t sub = tmp.subvalue(base, wid);

	// Normally, we'd be OK with just using long in the call to
	// vector4_to_value, but some compilers seem to take long as
	// distinct from int32_t AND int64_t. Since the condition is
	// constant, the compiler should eliminate the dead code.
      सचे (परिमाण(vp->value.integer) == परिमाण(int32_t)) {
	    int32_t val = 0;
	    vector4_to_मानम्(sub, val, signed_flag, मिच्छा);
	    vp->value.integer = val;
      } अञ्ञथा {
	    प्रतिज्ञा(परिमाण(vp->value.integer) == परिमाण(int64_t));
	    int64_t val = 0;
	    vector4_to_मानम्(sub, val, signed_flag, मिच्छा);
	    vp->value.integer = val;
      }
}

स्थिर शून्य format_vpiRealVal(ध्रुव vvp_signal_मानम्*sig, पूर्णाङ्क base, unsigned wid,
                              पूर्णाङ्क signed_flag, s_vpi_मानम्*vp)
{
      vvp_vector4_t vec4(wid);
      long ssize = (signed)sig->मानम्_size();
      long end = base + (signed)wid;
      सचे (end > ssize) end = ssize;

      अत्थाय (long idx = (base < 0) ? 0 : base ;  idx < end ;  idx += 1) {
	    vec4.set_bit(idx-base, sig->value(idx));
      }

      vp->value.real = 0.0;
      vector4_to_मानम्(vec4, vp->value.real, signed_flag);
}

स्थिर शून्य format_vpiStringVal(ध्रुव vvp_signal_मानम्*sig, पूर्णाङ्क base, unsigned wid,
                                s_vpi_मानम्*vp)
{
      /* The result will use a character for each 8 bits of the
	 vector. Add one extra character for the highest bits that
	 don't form an 8 bit group. */
      वर्ण *rbuf = static_cast<वर्ण *>(need_result_buf(wid/8 + ((wid&7)!=0) + 1, RBUF_VAL));
      वर्ण *cp = rbuf;

      वर्ण tmp = 0;
      अत्थाय (long idx = base+(signed)wid-1; idx >= base; idx -= 1) {
	    tmp <<= 1;

	    सचे (idx >=0 && idx < (signed)sig->मानम्_size() &&
	        sig->value(idx) == BIT4_1) {
		   tmp |= 1;
	    }

	    सचे (((idx-base)&7)==0){
		  /* Skip leading nulls. */
		  सचे (tmp == 0 && cp == rbuf)
			अनुवर्त;

		  /* Nulls in the middle get turned into spaces. */
		  *cp++ = tmp ? tmp : ' ';
		  tmp = 0;
	    }
      }
      *cp++ = 0;

      vp->value.str = rbuf;
}

स्थिर शून्य format_vpiScalarVal(ध्रुव vvp_signal_मानम्*sig, पूर्णाङ्क base,
                                s_vpi_मानम्*vp)
{
      सचे (base >= 0 && base < (signed)sig->मानम्_size()) {
	    विकल्प (sig->value(base)) {
		पक्ष BIT4_0:
		  vp->value.scalar = vpi0;
		  भङ्ग;
		पक्ष BIT4_1:
		  vp->value.scalar = vpi1;
		  भङ्ग;
		पक्ष BIT4_X: {
		  vvp_scalar_t strn = sig->scalar_मानम्(base);
		  सचे (strn.strength0() == 1) vp->value.scalar = vpiH;
		  अञ्ञथा सचे (strn.strength1() == 1) vp->value.scalar = vpiL;
		  अञ्ञथा vp->value.scalar = vpiX;
		  भङ्ग;
		}
		पक्ष BIT4_Z:
		  vp->value.scalar = vpiZ;
		  भङ्ग;
	    }
      } अञ्ञथा {
	    vp->value.scalar = vpiX;
      }
}

स्थिर शून्य format_vpiStrengthVal(ध्रुव vvp_signal_मानम्*sig, पूर्णाङ्क base,
                                  unsigned wid, s_vpi_मानम्*vp)
{
      long end = base + (signed)wid;
      s_vpi_strengthval*op;

      op = static_cast<s_vpi_strengthval*>
           (need_result_buf(wid * परिमाण(s_vpi_strengthval), RBUF_VAL));

      अत्थाय (long idx = base ;  idx < end ;  idx += 1) {
	    सचे (idx >=0 && idx < (signed)sig->मानम्_size()) {
		  vvp_scalar_t val = sig->scalar_मानम्(idx);

		  /* vvp_scalar_t strengths are 0-7, but the vpi strength
		     is bit0-bit7. This gets the vpi form of the strengths
		     from the vvp_scalar_t strengths. */
		  unsigned s0 = 1 << val.strength0();
		  unsigned s1 = 1 << val.strength1();

		  विकल्प (val.value()) {
		      पक्ष BIT4_0:
			op[idx-base].logic = vpi0;
			op[idx-base].s0 = s0|s1;
			op[idx-base].s1 = 0;
			भङ्ग;

		      पक्ष BIT4_1:
			op[idx-base].logic = vpi1;
			op[idx-base].s0 = 0;
			op[idx-base].s1 = s0|s1;
			भङ्ग;

		      पक्ष BIT4_X:
			op[idx-base].logic = vpiX;
			op[idx-base].s0 = s0;
			op[idx-base].s1 = s1;
			भङ्ग;

		      पक्ष BIT4_Z:
			op[idx-base].logic = vpiZ;
			op[idx-base].s0 = vpiHiZ;
			op[idx-base].s1 = vpiHiZ;
			भङ्ग;
		  }
	    } अञ्ञथा {
		  op[idx-base].logic = vpiX;
		  op[idx-base].s0 = vpiStrongDrive;
		  op[idx-base].s1 = vpiStrongDrive;
	    }
      }

      vp->value.strength = op;
}

स्थिर शून्य format_vpiVectorVal(ध्रुव vvp_signal_मानम्*sig, पूर्णाङ्क base, unsigned wid,
                                s_vpi_मानम्*vp)
{
      long end = base + (signed)wid;
      unsigned पूर्णाङ्क obit = 0;
      unsigned hwid = (wid + 31)/32;

      s_vpi_vecval *op = static_cast<p_vpi_vecval>
                         (need_result_buf(hwid * परिमाण(s_vpi_vecval), RBUF_VAL));
      vp->value.vector = op;

      op->aval = op->bval = 0;
      अत्थाय (long idx = base ;  idx < end ;  idx += 1) {
	    सचे (base >= 0 && base < (signed)sig->मानम्_size()) {
		विकल्प (sig->value(idx)) {
		पक्ष BIT4_0:
		  op->aval &= ~(1 << obit);
		  op->bval &= ~(1 << obit);
		  भङ्ग;
		पक्ष BIT4_1:
		  op->aval |= (1 << obit);
		  op->bval &= ~(1 << obit);
		  भङ्ग;
		पक्ष BIT4_X:
		  op->aval |= (1 << obit);
		  op->bval |= (1 << obit);
		  भङ्ग;
		पक्ष BIT4_Z:
		  op->aval &= ~(1 << obit);
		  op->bval |= (1 << obit);
		  भङ्ग;
		}
	    } अञ्ञथा {  /* BIT4_X */
		  op->aval |= (1 << obit);
		  op->bval |= (1 << obit);
	    }

	    obit++;
	    सचे (!(obit % 32)) {
		  op += 1;
		  सचे ((op - vp->value.vector) < (ptrdiff_t)hwid)
			op->aval = op->bval = 0;
		  obit = 0;
	    }
      }
}

/*
 * implement vpi_get for vpiReg objects.
 */
स्थिर पूर्णाङ्क signal_get(पूर्णाङ्क code, vpiHandle ref)
{
      संरचना __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      प्रतिज्ञा(rfp);

      विकल्प (code) {
	  पक्ष vpi_पङ्क्तिः_No:
	    पटिनिवत्त 0;  // Not implemented for now!

	  पक्ष vpiSigned:
	    पटिनिवत्त rfp->signed_flag != 0;

	  पक्ष vpiArray:
	    पटिनिवत्त rfp->is_netarray != 0;

	  पक्ष vpiIndex: // This only works while we have a single index.
	    सचे (rfp->is_netarray) {
		  s_vpi_मानम् vp;
		  vp.format = vpiIntVal;
		  vpi_get_मानम्(rfp->id.index, &vp);
		  पटिनिवत्त vp.value.integer;
	    } अञ्ञथा {
		  पटिनिवत्त vpiUndefined;
	    }

	  पक्ष vpiSize:
	    पटिनिवत्त rfp->width();

	  पक्ष vpiNetType:
	    सचे (ref->get_type_code()==vpiNet)
		  पटिनिवत्त vpiWire;
	    अञ्ञथा
		  पटिनिवत्त vpiUndefined;

	  पक्ष vpi_वामः_Range:
            पटिनिवत्त rfp->msb.get_मानम्();

	  पक्ष vpi_दक्षिणः_Range:
            पटिनिवत्त rfp->lsb.get_मानम्();

	  पक्ष vpiScalar:
	    पटिनिवत्त (rfp->msb.get_मानम्() == 0 && rfp->lsb.get_मानम्() == 0);
	  पक्ष vpiVector:
	    पटिनिवत्त (rfp->msb.get_मानम्() != rfp->lsb.get_मानम्());

          पक्ष vpiAutomatic:
            पटिनिवत्त vpip_scope(rfp)->is_automatic() ? 1  : 0;

#ifdef BR916_STOPGAP_FIX
          पक्ष _vpiFromThr:
            पटिनिवत्त _vpiNoThr;
#endif

	  स्वभाव:
	    fprintf(stderr, "VPI error: unknown signal_get property %d.\n",
	            code);
	    पटिनिवत्त vpiUndefined;
      }
}

/*
 * implement vpi_get64 for vpiReg objects.
 */
स्थिर int64_t signal_get64(पूर्णाङ्क code, vpiHandle ref)
{
      संरचना __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      प्रतिज्ञा(rfp);

      विकल्प (code) {
	    // This private property must return zero when undefined.
	  पक्ष _vpiNexusId:
	    सचे (rfp->msb.get_मानम्() == rfp->lsb.get_मानम्())
		  पटिनिवत्त reinterpret_cast<int64_t>(rfp->ग्रन्थिः);
	    पटिनिवत्त 0;

	  स्वभाव:
	    fprintf(stderr, "VPI error: unknown signal_get64 property %d.\n",
	            code);
	    पटिनिवत्त vpiUndefined;
      }
}

स्थिर वर्ण* signal_get_str(पूर्णाङ्क code, vpiHandle ref)
{
      संरचना __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      प्रतिज्ञा(rfp);

      सचे (code == vpiFile) {  // Not implemented for now!
	    पटिनिवत्त simple_set_rbuf_str(file_names[0]);
      }

      सचे ((code != vpiName) && (code != vpiFullName)) पटिनिवत्त रिक्त;

      वर्ण *nm;
      ध्रुव वर्ण *ixs;
      सचे (rfp->is_netarray) {
	    nm = strdup(vpi_get_str(vpiName, rfp->within.parent));
	    s_vpi_मानम् vp;
	    vp.format = vpiDecStrVal;
	    vpi_get_मानम्(rfp->id.index, &vp);
	    ixs = vp.value.str;  /* do I need to strdup() this? */
      } अञ्ञथा {
	    nm = strdup(rfp->id.name);
	    ixs = रिक्त;
      }

	/* The scope information is added here for vpiFullName. */
      वर्ण *rbuf = generic_get_str(code, vpip_scope(rfp), nm, ixs);
      मुक्त(nm);
      पटिनिवत्त rbuf;
}

स्थिर vpiHandle signal_get_handle(पूर्णाङ्क code, vpiHandle ref)
{
      संरचना __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      प्रतिज्ञा(rfp);

      विकल्प (code) {

	  पक्ष vpiParent:
	    पटिनिवत्त rfp->is_netarray? rfp->within.parent : रिक्त;

	  पक्ष vpiIndex:
	    पटिनिवत्त rfp->is_netarray? rfp->id.index : रिक्त;

	  पक्ष vpi_वामः_Range:
	    पटिनिवत्त &rfp->msb;
	  पक्ष vpi_दक्षिणः_Range:
	    पटिनिवत्त &rfp->lsb;

	  पक्ष vpiScope:
	    पटिनिवत्त vpip_scope(rfp);

	  पक्ष vpiModule:
	    पटिनिवत्त vpip_module(vpip_scope(rfp));
      }

      पटिनिवत्त 0;
}

स्थिर vpiHandle signal_iterate(पूर्णाङ्क code, vpiHandle ref)
{
      संरचना __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      प्रतिज्ञा(rfp);

      सचे (code == vpiIndex) {
	    पटिनिवत्त rfp->is_netarray ? rfp->id.index->vpi_iterate(code) : रिक्त;
      }

      पटिनिवत्त 0;
}

शून्य __vpiSignal::make_bits()
{
      प्रतिज्ञा(bits == रिक्त);
      bits = नूतन संरचना __vpiBit[width() + 1];

	// Make word[-1] point to the parent.
      bits->parent = एतत्;
	// Now point to word[0].
      bits += 1;

	// Update each bit to point to the base
      संरचना __vpiBit*base = bits;
      अत्थाय (unsigned idx = 0; idx < width(); idx += 1) {
	    base[idx].bit0 = base;
	    पूर्णाङ्क real_idx;
	    सचे (msb.get_मानम्() >= lsb.get_मानम्()) {
		  real_idx = idx + lsb.get_मानम्();
	    } अञ्ञथा {
		  real_idx = lsb.get_मानम्() - idx;
	    }
	    base[idx].index = नूतन __vpiDecConst(real_idx);
      }
}

vpiHandle __vpiSignal::get_index(पूर्णाङ्क idx)
{
	/* Check to see if the index is in range. */
      सचे (msb.get_मानम्() >= lsb.get_मानम्()) {
	    सचे ((idx > msb.get_मानम्()) || (idx < lsb.get_मानम्())) पटिनिवत्त 0;
      } अञ्ञथा {
	    सचे ((idx < msb.get_मानम्()) || (idx > lsb.get_मानम्())) पटिनिवत्त 0;
      }

	/* Normalize the index */
      unsigned norm_idx;
      सचे (msb.get_मानम्() >= lsb.get_मानम्()) {
	    norm_idx = idx - lsb.get_मानम्();
      } अञ्ञथा {
	    norm_idx = lsb.get_मानम्() - idx;
      }

      सचे (bits == रिक्त) make_bits();

      पटिनिवत्त &(bits[norm_idx].as_bit);
}

शून्य __vpiSignal::get_bit_मानम्(ध्रुव संरचना __vpiBit*bit, p_vpi_मानम् vp)
{
      unsigned index = bit->get_norm_index();

      vvp_signal_मानम्*vsig = dynamic_cast<vvp_signal_मानम्*>(ग्रन्थिः->fil);
      प्रतिज्ञा(vsig);

      सचे (vp->format == vpiObjTypeVal) {
	    vp->format = vpiIntVal;
      }

      विकल्प (vp->format) {
	पक्ष vpiBinStrVal:
            format_vpiBinStrVal(vsig, index, 1, vp);
	    भङ्ग;

	पक्ष vpiOctStrVal:
            format_vpiOctStrVal(vsig, index, 1, vp);
	    भङ्ग;

	पक्ष vpiDecStrVal:
            format_vpiDecStrVal(vsig, index, 1, मिच्छा, vp);
	    भङ्ग;

	पक्ष vpiHexStrVal:
            format_vpiHexStrVal(vsig, index, 1, vp);
	    भङ्ग;

	पक्ष vpiStringVal:
            format_vpiStringVal(vsig, index, 1, vp);
	    भङ्ग;

	पक्ष vpiIntVal:
            format_vpiIntVal(vsig, index, 1, मिच्छा, vp);
	    भङ्ग;

	पक्ष vpiRealVal:
            format_vpiRealVal(vsig, index, 1, मिच्छा, vp);
	    भङ्ग;

	पक्ष vpiScalarVal:
            format_vpiScalarVal(vsig, index, vp);
	    भङ्ग;

        पक्ष vpiStrengthVal:
            format_vpiStrengthVal(vsig, index, 1, vp);
            भङ्ग;

	पक्ष vpiVectorVal:
            format_vpiVectorVal(vsig, index, 1, vp);
	    भङ्ग;

	स्वभाव:
	    fprintf(stderr, "internal error: get_value() "
		    "type %d is not implemented for bit "
		    "select %s[%d] in scope %s.\n",
		    (पूर्णाङ्क)vp->format, vpi_get_str(vpiName),
		    bit->get_index(),
	            vpip_scope(एतत्)->scope_name());
	    प्रतिज्ञा(0);
      };
}

vpiHandle __vpiSignal::put_bit_मानम्(संरचना __vpiBit*bit, p_vpi_मानम् vp, पूर्णाङ्क flags)
{
      unsigned index = bit->get_norm_index();
      vvp_net_ptr_t dest(ग्रन्थिः, 0);
      vvp_vector4_t val = vec4_from_vpi_मानम्(vp, 1);

      सचे ((flags == vpiForceFlag) || (flags == vpiReleaseFlag)) {
	    fprintf(stderr, "Sorry: vpi_put_value() for %s does not "
	                    "currently support force/release.\n",
	                    bit->as_bit.vpi_get_str(vpiFullName));
	    पटिनिवत्त रिक्त;
      }

      सचे ((get_type_code() == vpiNet) &&
          !dynamic_cast<vvp_island_port*>(ग्रन्थिः->fun)) {
	    ग्रन्थिः->send_vec4_pv(val, index, width(),
	                            vthread_get_wt_context());
      } अञ्ञथा {
	    vvp_send_vec4_pv(dest, val, index, width(),
	                     vthread_get_wt_context());
      }

	// This is not a scheduled event so there is no event to return
      पटिनिवत्त रिक्त;
}

स्थिर vpiHandle signal_index(पूर्णाङ्क idx, vpiHandle ref)
{
      संरचना __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      प्रतिज्ञा(rfp);

	/* We can only get the bit for a net or reg. */
      PLI_INT32 type = vpi_get(vpiType, ref);
      सचे ((type != vpiNet) && (type != vpiReg)) पटिनिवत्त 0;

      पटिनिवत्त rfp->get_index(idx);
}

unsigned __vpiSignal::width(शून्य) ध्रुव
{
      unsigned wid = (msb.get_मानम्() >= lsb.get_मानम्())
	    ? (msb.get_मानम्() - lsb.get_मानम्() + 1)
	    : (lsb.get_मानम्() - msb.get_मानम्() + 1);

      पटिनिवत्त wid;
}

/*
 * The get_value method reads the values of the functors and returns
 * the vector to the caller. This causes no side-effect, and reads the
 * variables like a %load would.
 */
स्थिर शून्य signal_get_मानम्(vpiHandle ref, s_vpi_मानम्*vp)
{
      संरचना __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      प्रतिज्ञा(rfp);

      unsigned wid = rfp->width();

      vvp_signal_मानम्*vsig = dynamic_cast<vvp_signal_मानम्*>(rfp->ग्रन्थिः->fil);
      प्रतिज्ञा(vsig);

      विकल्प (vp->format) {

	  पक्ष vpiIntVal:
	    format_vpiIntVal(vsig, 0, wid, rfp->signed_flag, vp);
	    भङ्ग;

	  पक्ष vpiScalarVal:
	    format_vpiScalarVal(vsig, 0, vp);
	    भङ्ग;

	  पक्ष vpiStrengthVal:
	    format_vpiStrengthVal(vsig, 0, wid, vp);
	    भङ्ग;

	  पक्ष vpiBinStrVal:
	    format_vpiBinStrVal(vsig, 0, wid, vp);
	    भङ्ग;

	  पक्ष vpiHexStrVal:
	    format_vpiHexStrVal(vsig, 0, wid, vp);
	    भङ्ग;

	  पक्ष vpiOctStrVal:
	    format_vpiOctStrVal(vsig, 0, wid, vp);
	    भङ्ग;

	  पक्ष vpiDecStrVal:
	    format_vpiDecStrVal(vsig, 0, wid, rfp->signed_flag, vp);
	    भङ्ग;

	  पक्ष vpiStringVal:
	    format_vpiStringVal(vsig, 0, wid, vp);
	    भङ्ग;

	  पक्ष vpiVectorVal:
	    format_vpiVectorVal(vsig, 0, wid, vp);
	    भङ्ग;

	  पक्ष vpiRealVal:
	    format_vpiRealVal(vsig, 0, wid, rfp->signed_flag, vp);
	    भङ्ग;

	  पक्ष vpiObjTypeVal:
	    सचे (wid == 1) {
		  vp->format = vpiScalarVal;
		  format_vpiScalarVal(vsig, 0, vp);
	    } अञ्ञथा {
		  vp->format = vpiVectorVal;
		  format_vpiVectorVal(vsig, 0, wid, vp);
	    }
	    भङ्ग;

	  स्वभाव:
	    fprintf(stderr, "vvp internal error: get_value: "
		    "value type %d not implemented."
		    " Signal is %s in scope %s\n",
		    (पूर्णाङ्क)vp->format, vpi_get_str(vpiName, ref),
		    vpip_scope(rfp)->scope_name());
	    प्रतिज्ञा(0);
      }
}

/*
 * The put_value method writes the value into the vector, and returns
 * the affected ref. This operation works much like the %set or
 * %assign instructions and causes all the side-effects that the
 * equivalent instruction would cause.
 */

स्थिर vvp_vector4_t from_stringval(ध्रुव वर्ण*str, unsigned wid)
{
      unsigned idx;
      ध्रुव वर्ण*cp;

      cp = str + strlen(str);
      idx = 0;

      vvp_vector4_t val(wid, BIT4_0);

      याव ((idx < wid) && (cp > str)) {
	    unsigned byte = *--cp;
	    पूर्णाङ्क bit;

	    अत्थाय (bit = 0 ;  (bit < 8) && (idx < wid) ;  bit += 1) {
		  सचे (byte & 1)
			val.set_bit(idx, BIT4_1);

		  byte >>= 1;
		  idx += 1;
	    }
      }

      पटिनिवत्त val;
}

स्थिर vpiHandle signal_put_मानम्(vpiHandle ref, s_vpi_मानम्*vp, पूर्णाङ्क flags)
{
      unsigned wid;
      संरचना __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      प्रतिज्ञा(rfp);
      vvp_net_ptr_t dest(rfp->ग्रन्थिः, 0);

      तार्किक net_flag = ref->get_type_code()==vpiNet;

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value. Note that
	   if this net is attached to an island, we need to rerun
	   the calculations immediately so we can return the
	   released value. */
      सचे (flags == vpiReleaseFlag) {
	    प्रतिज्ञा(rfp->ग्रन्थिः->fil);
	    rfp->ग्रन्थिः->fil->force_unlink();
	    rfp->ग्रन्थिः->fil->release(dest, net_flag);
	    rfp->ग्रन्थिः->fun->force_flag(सच्च);
	    signal_get_मानम्(ref, vp);
	    पटिनिवत्त ref;
      }

	/* Make a vvp_vector4_t vector to receive the translated value
	   that we are going to poke. This will get populated
	   differently depending on the format. */
      wid = (rfp->msb.get_मानम्() >= rfp->lsb.get_मानम्())
	    ? (rfp->msb.get_मानम्() - rfp->lsb.get_मानम्() + 1)
	    : (rfp->lsb.get_मानम्() - rfp->msb.get_मानम्() + 1);

      vvp_vector4_t val = vec4_from_vpi_मानम्(vp, wid);

      सचे (flags == vpiForceFlag) {
	    vvp_vector2_t mask (vvp_vector2_t::FILL1, wid);
	    rfp->ग्रन्थिः->force_vec4(val, mask);
      } अञ्ञथा सचे (net_flag && !dynamic_cast<vvp_island_port*>(rfp->ग्रन्थिः->fun)) {
	    rfp->ग्रन्थिः->send_vec4(val, vthread_get_wt_context());
      } अञ्ञथा {
	    vvp_send_vec4(dest, val, vthread_get_wt_context());
      }
      पटिनिवत्त ref;
}

vvp_vector4_t vec4_from_vpi_मानम्(s_vpi_मानम्*vp, unsigned wid)
{
      vvp_vector4_t val (wid, BIT4_0);

      विकल्प (vp->format) {

	  पक्ष vpiIntVal: {
		long vpi_val = vp->value.integer;
		अत्थाय (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		      vvp_bit4_t bit = (vpi_val & 1) ? BIT4_1 : BIT4_0;
		      val.set_bit(idx, bit);
		      vpi_val >>= 1;
		}
		भङ्ग;
	  }

	  पक्ष vpiVectorVal:
	    अत्थाय (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		  unsigned long aval = vp->value.vector[idx/32].aval;
		  unsigned long bval = vp->value.vector[idx/32].bval;
		  aval >>= idx%32;
		  bval >>= idx%32;
		  पूर्णाङ्क bitmask = (aval&1) | ((bval<<1)&2);
		  vvp_bit4_t bit = scalar_to_bit4(bitmask);
		  val.set_bit(idx, bit);
	    }
	    भङ्ग;
	  पक्ष vpiBinStrVal:
	    vpip_bin_str_to_vec4(val, vp->value.str);
	    भङ्ग;
	  पक्ष vpiOctStrVal:
	    vpip_oct_str_to_vec4(val, vp->value.str);
	    भङ्ग;
	  पक्ष vpiDecStrVal:
	    vpip_dec_str_to_vec4(val, vp->value.str);
	    भङ्ग;
	  पक्ष vpiHexStrVal:
	    vpip_hex_str_to_vec4(val, vp->value.str);
	    भङ्ग;
	  पक्ष vpiScalarVal:
	    val.set_bit(0, scalar_to_bit4(vp->value.scalar));
	    भङ्ग;
	  पक्ष vpiStringVal:
	    val = from_stringval(vp->value.str, wid);
	    भङ्ग;
	  पक्ष vpiRealVal:
	    val = vvp_vector4_t(wid, vp->value.real);
	    भङ्ग;

	  स्वभाव:
	    fprintf(stderr, "vvp internal error: put_value: "
		    "value type %d not implemented here.\n",
		    (पूर्णाङ्क)vp->format);
	    प्रतिज्ञा(0);

      }

      पटिनिवत्त val;
}

पूर्णाङ्क __vpiSignal::vpi_get(पूर्णाङ्क code)
{ पटिनिवत्त signal_get(code, एतत्); }

int64_t __vpiSignal::vpi_get64(पूर्णाङ्क code)
{ पटिनिवत्त signal_get64(code, एतत्); }

वर्ण* __vpiSignal::vpi_get_str(पूर्णाङ्क code)
{ पटिनिवत्त signal_get_str(code, एतत्); }

शून्य __vpiSignal::vpi_get_मानम्(p_vpi_मानम् val)
{ signal_get_मानम्(एतत्, val); }

vpiHandle __vpiSignal::vpi_put_मानम्(p_vpi_मानम् val, पूर्णाङ्क flags)
{ पटिनिवत्त signal_put_मानम्(एतत्, val, flags); }

vpiHandle __vpiSignal::vpi_handle(पूर्णाङ्क code)
{ पटिनिवत्त signal_get_handle(code, एतत्); }

vpiHandle __vpiSignal::vpi_iterate(पूर्णाङ्क code)
{ पटिनिवत्त signal_iterate(code, एतत्); }

vpiHandle __vpiSignal::vpi_index(पूर्णाङ्क idx)
{ पटिनिवत्त signal_index(idx, एतत्); }

संरचना signal_reg : सार्वजनिक __vpiSignal {
      अन्तःस्थ signal_reg() { }
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override { पटिनिवत्त vpiReg; }
};

संरचना signal_integer : सार्वजनिक __vpiSignal {
      अन्तःस्थ signal_integer() { }
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override { पटिनिवत्त vpiIntegerVar; }
};

संरचना signal_net : सार्वजनिक __vpiSignal {
      अन्तःस्थ signal_net() { }
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override { पटिनिवत्त vpiNet; }
};

संरचना signal_byte : सार्वजनिक __vpiSignal {
      अन्तःस्थ signal_byte() { }
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override { पटिनिवत्त vpiByteVar; }
};

संरचना signal_bitvar : सार्वजनिक __vpiSignal {
      अन्तःस्थ signal_bitvar() { }
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override { पटिनिवत्त vpiBitVar; }
};

संरचना signal_shortint : सार्वजनिक __vpiSignal {
      अन्तःस्थ signal_shortint() { }
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override { पटिनिवत्त vpiShortIntVar; }
};

संरचना signal_int : सार्वजनिक __vpiSignal {
      अन्तःस्थ signal_int() { }
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override { पटिनिवत्त vpiIntVar; }
};

संरचना signal_longint : सार्वजनिक __vpiSignal {
      अन्तःस्थ signal_longint() { }
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override { पटिनिवत्त vpiLongIntVar; }
};


/*
 * Construct a vpiIntegerVar object. Indicate the type using a flag
 * to minimize the code modifications. Icarus implements integers
 * as 'reg signed [31:0]'.
 */
vpiHandle vpip_make_int4(ध्रुव वर्ण*name, पूर्णाङ्क msb, पूर्णाङ्क lsb, vvp_net_t*vec)
{
      __vpiSignal*obj = नूतन signal_integer;
      पटिनिवत्त fill_in_var4(obj, name, msb, lsb, सच्च, vec);
}

/*
 * Construct the two-state SystemVerilog variables.
 */
vpiHandle vpip_make_int2(ध्रुव वर्ण*name, पूर्णाङ्क msb, पूर्णाङ्क lsb, तार्किक signed_flag,
                         vvp_net_t*vec)
{
      __vpiSignal*obj;

	// All unsigned 2-state variables are a vpiBitVar. All 2-state
	// variables with a non-zero lsb are also a vpiBitVar.
      सचे ((! signed_flag) || (lsb != 0) ) {
	    obj = नूतन signal_bitvar;
      } अञ्ञथा {
	      // These could also be bit declarations with matching
	      // information, but for now they get the apparent type.
	    विकल्प (msb) {
		पक्ष 7:
		  obj = नूतन signal_byte;
		  भङ्ग;
		पक्ष 15:
		  obj = नूतन signal_shortint;
		  भङ्ग;
		पक्ष 31:
		  obj = नूतन signal_int;
		  भङ्ग;
		पक्ष 63:
		  obj = नूतन signal_longint;
		  भङ्ग;
		स्वभाव:
		    // Every other type of bit vector is a vpiBitVar with
		    // array dimensions.
		  obj = नूतन signal_bitvar;
		  भङ्ग;
	    }
      }

      पटिनिवत्त fill_in_var4(obj, name, msb, lsb, signed_flag, vec);
}

/*
 * Construct a vpiReg/vpiLogicVar object. It's like a net, except for the type.
 */
vpiHandle vpip_make_var4(ध्रुव वर्ण*name, पूर्णाङ्क msb, पूर्णाङ्क lsb,
			तार्किक signed_flag, vvp_net_t*vec)
{
      __vpiSignal*obj = नूतन signal_reg;
      पटिनिवत्त fill_in_var4(obj, name, msb, lsb, signed_flag, vec);
}

#ifdef CHECK_WITH_VALGRIND
स्थिर संरचना vpiSignal_plug **signal_pool = 0;
स्थिर unsigned signal_pool_गणनम् = 0;
स्थिर unsigned long signal_गणनम् = 0;
स्थिर unsigned long signal_dels = 0;
#endif

संरचना vpiSignal_plug {
      unsigned वर्ण space[परिमाण (संरचना __vpiSignal)];
#ifdef CHECK_WITH_VALGRIND
      संरचना vpiSignal_plug *pool;
#endif
};

शून्य* __vpiSignal::operator नूतन(size_t siz)
{
#ifdef CHECK_WITH_VALGRIND
      प्रतिज्ञा(siz == परिमाण(संरचना vpiSignal_plug) - परिमाण(संरचना vpiSignal_plug *));
#अञ्ञथा
      प्रतिज्ञा(siz == परिमाण(संरचना vpiSignal_plug));
#endif
      स्थिर संरचना vpiSignal_plug*alloc_array = 0;
      स्थिर unsigned alloc_index = 0;
      ध्रुव unsigned alloc_गणनम् = 512;

      सचे ((alloc_array == 0) || (alloc_index == alloc_गणनम्)) {
	    alloc_array = static_cast<संरचना vpiSignal_plug*>
	                  (calloc(alloc_गणनम्, परिमाण(संरचना vpiSignal_plug)));
	    alloc_index = 0;
#ifdef CHECK_WITH_VALGRIND
	    VALGRIND_MAKE_MEM_NOACCESS(alloc_array, alloc_गणनम् *
	                                            परिमाण(संरचना vpiSignal_plug));
	    VALGRIND_CREATE_MEMPOOL(alloc_array, 0, 1);
	    signal_pool_गणनम् += 1;
	    signal_pool = static_cast<vpiSignal_plug **>
	                  (realloc(signal_pool,
	                   signal_pool_गणनम्*परिमाण(vpiSignal_plug **)));
	    signal_pool[signal_pool_गणनम्-1] = alloc_array;
#endif
      }

      संरचना vpiSignal_plug*cur = alloc_array + alloc_index;
#ifdef CHECK_WITH_VALGRIND
      VALGRIND_MEMPOOL_ALLOC(alloc_array, cur, परिमाण(संरचना vpiSignal_plug));
      cur->pool = alloc_array;
      signal_गणनम् += 1;
#endif
      alloc_index += 1;
      पटिनिवत्त cur;
}

शून्य __vpiSignal::operator delete(शून्य*)
{
      प्रतिज्ञा(0);
}

#ifdef CHECK_WITH_VALGRIND
शून्य signal_delete(vpiHandle item)
{
      संरचना __vpiSignal *obj = static_cast<__vpiSignal *> (item);
      प्रतिज्ञा(obj->ग्रन्थिः->fil);
      obj->ग्रन्थिः->fil->clear_all_callbacks();
      vvp_net_delete(obj->ग्रन्थिः);
      सचे (obj->bits) {
	    अत्थाय (unsigned idx=0; idx<obj->width(); idx+=1) {
		  delete obj->bits[idx].index;
	    }
	    obj->bits -= 1;
	    delete [] obj->bits;
      }
      signal_dels += 1;
      VALGRIND_MEMPOOL_FREE(reinterpret_cast<vpiSignal_plug *>(obj)->pool, obj);
}

शून्य signal_pool_delete()
{
      सचे (RUNNING_ON_VALGRIND && (signal_गणनम् != signal_dels)) {
	    fflush(रिक्त);
	    VALGRIND_PRINTF("Error: vvp missed deleting %ld of %lu signal(s).",
	                    (long) signal_गणनम् - signal_dels, signal_गणनम्);
      }

      अत्थाय (unsigned idx = 0; idx < signal_pool_गणनम्; idx += 1) {
	    VALGRIND_DESTROY_MEMPOOL(signal_pool[idx]);
	    मुक्त(signal_pool[idx]);
      }

      मुक्त(signal_pool);
      signal_pool = 0;
      signal_pool_गणनम् = 0;
}
#endif

/*
 * Construct a vpiNet object. Give the object specified dimensions,
 * and point to the specified functor for the lsb.
 *
 * The name is the PLI name for the object. If it is an array it is
 * <name>[<index>].
 */
स्थिर vpiHandle fill_in_net4(संरचना __vpiSignal*obj, __vpiScope*scope,
			      ध्रुव वर्ण*name, पूर्णाङ्क msb, पूर्णाङ्क lsb,
			      तार्किक signed_flag, vvp_net_t*ग्रन्थिः)
{
      obj->id.name = name? vpip_name_string(name) : रिक्त;
      obj->msb = __vpiDecConst(msb);
      obj->lsb = __vpiDecConst(lsb);
      obj->signed_flag = signed_flag? 1 : 0;
      obj->is_netarray = 0;
      obj->ग्रन्थिः = ग्रन्थिः;

	// Place this object within a scope. If this object is
	// attached to an array, then this value will be replaced with
	// the handle to the parent.
      obj->within.scope = scope;

      गणनम्_vpi_nets += 1;

      पटिनिवत्त obj;
}

vpiHandle vpip_make_net4(__vpiScope*scope,
			 ध्रुव वर्ण*name, पूर्णाङ्क msb, पूर्णाङ्क lsb,
			 तार्किक signed_flag, vvp_net_t*ग्रन्थिः)
{
      संरचना __vpiSignal*obj = नूतन signal_net;
      पटिनिवत्त fill_in_net4(obj, scope, name, msb, lsb, signed_flag, ग्रन्थिः);
}

स्थिर पूर्णाङ्क PV_get_base(संरचना __vpiPV*rfp)
{
	/* We return from the symbol base if it is defined. */
      सचे (rfp->sbase != 0) {
	    s_vpi_मानम् val;
	      /* Check to see if the value is defined. */
	    val.format = vpiVectorVal;
	    vpi_get_मानम्(rfp->sbase, &val);
	    पूर्णाङ्क शब्दाः = (vpi_get(vpiSize, rfp->sbase)-1)/32 + 1;
	    अत्थाय(पूर्णाङ्क idx = 0; idx < शब्दाः; idx += 1) {
		    /* Return INT_MIN to indicate an X base. */
		  सचे (val.value.vector[idx].bval != 0) पटिनिवत्त INT_MIN;
	    }
	      /* The value is defined so get and return it. */
	    val.format = vpiIntVal;
	    vpi_get_मानम्(rfp->sbase, &val);
	    पटिनिवत्त val.value.integer;
      }

	/* If the width is zero then tbase is the constant. */
      पटिनिवत्त rfp->tbase;
}

स्थिर पूर्णाङ्क PV_get(पूर्णाङ्क code, vpiHandle ref)
{
      संरचना __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      प्रतिज्ञा(rfp);

      पूर्णाङ्क rval = 0;
      विकल्प (code) {
	पक्ष vpi_पङ्क्तिः_No:
	    पटिनिवत्त 0;  // Not implemented for now!

	पक्ष vpiSigned:
	    पटिनिवत्त 0;  // A part/bit select is always unsigned!

	पक्ष vpiSize:
	    पटिनिवत्त rfp->width;

	  /* This is like the &A<> in array.cc. */
	पक्ष vpiConstantSelect:
	    पटिनिवत्त rfp->sbase == 0;

	पक्ष vpi_वामः_Range:
            rval += rfp->width - 1;
	    // fallthrough
	पक्ष vpi_दक्षिणः_Range: {
	    पूर्णाङ्क वामः = vpi_get(vpi_वामः_Range, rfp->parent);
	    पूर्णाङ्क दक्षिणः = vpi_get(vpi_दक्षिणः_Range, rfp->parent);
	    rval += PV_get_base(rfp);
	    पटिनिवत्त वामः >= दक्षिणः ? दक्षिणः + rval : दक्षिणः - rval;
	}

        पक्ष vpiAutomatic:
            पटिनिवत्त vpi_get(vpiAutomatic, rfp->parent);

#सचे defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
        पक्ष _vpiFromThr:
            पटिनिवत्त _vpi_at_PV;
#endif

	स्वभाव:
	    fprintf(stderr, "PV_get: property %d is unknown\n", code);
      }

      पटिनिवत्त 0;
}

स्थिर वर्ण* PV_get_str(पूर्णाङ्क code, vpiHandle ref)
{
      संरचना __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      प्रतिज्ञा(rfp);

      विकल्प (code) {
	पक्ष vpiFile:  // Not implemented for now!
	    पटिनिवत्त simple_set_rbuf_str(file_names[0]);

	पक्ष vpiName:
	पक्ष vpiFullName: {
	    ध्रुव वर्ण*nm = vpi_get_str(code, rfp->parent);
	    size_t len = 256+strlen(nm);
	    वर्ण *full = static_cast<वर्ण *>(malloc(len));
	    snprintf(full, len, "%s[%d:%d]", nm,
	                                     (पूर्णाङ्क)vpi_get(vpi_वामः_Range, ref),
	                                     (पूर्णाङ्क)vpi_get(vpi_दक्षिणः_Range, ref));
	    full[len-1] = 0;
	    वर्ण *res = simple_set_rbuf_str(full);
	    मुक्त(full);
	    पटिनिवत्त res;
	}

	स्वभाव:
	    fprintf(stderr, "PV_get_str: property %d is unknown.\n", code);
      }

      पटिनिवत्त 0;
}

स्थिर शून्य PV_get_मानम्(vpiHandle ref, p_vpi_मानम् vp)
{
      संरचना __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      प्रतिज्ञा(rfp);

      vvp_signal_मानम्*sig = dynamic_cast<vvp_signal_मानम्*>(rfp->net->fil);
      प्रतिज्ञा(sig);

      विकल्प (vp->format) {

	  पक्ष vpiIntVal:
	    format_vpiIntVal(sig, PV_get_base(rfp), rfp->width, 0, vp);
	    भङ्ग;

	  पक्ष vpiBinStrVal:
	    format_vpiBinStrVal(sig, PV_get_base(rfp), rfp->width, vp);
	    भङ्ग;

	  पक्ष vpiOctStrVal:
	    format_vpiOctStrVal(sig, PV_get_base(rfp), rfp->width, vp);
	    भङ्ग;

	  पक्ष vpiHexStrVal:
	    format_vpiHexStrVal(sig, PV_get_base(rfp), rfp->width, vp);
	    भङ्ग;

	  पक्ष vpiDecStrVal:
	    format_vpiDecStrVal(sig, PV_get_base(rfp), rfp->width, 0, vp);
	    भङ्ग;

	  पक्ष vpiStringVal:
	    format_vpiStringVal(sig, PV_get_base(rfp), rfp->width, vp);
	    भङ्ग;

	  पक्ष vpiScalarVal:
	    format_vpiScalarVal(sig, PV_get_base(rfp), vp);
	    भङ्ग;

	  पक्ष vpiStrengthVal:
	    format_vpiStrengthVal(sig, PV_get_base(rfp), rfp->width, vp);
	    भङ्ग;

	  पक्ष vpiVectorVal:
	    format_vpiVectorVal(sig, PV_get_base(rfp), rfp->width, vp);
	    भङ्ग;

	  पक्ष vpiRealVal:
	    format_vpiRealVal(sig, PV_get_base(rfp), rfp->width, 0, vp);
	    भङ्ग;

	  स्वभाव:
	    fprintf(stderr, "vvp internal error: PV_get_value: "
		    "value type %d not implemented. Signal is %s.\n",
		    (पूर्णाङ्क)vp->format, vpi_get_str(vpiFullName, rfp->parent));
	    प्रतिज्ञा(0);
      }
}

स्थिर vpiHandle PV_put_मानम्(vpiHandle ref, p_vpi_मानम् vp, पूर्णाङ्क flags)
{
      संरचना __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      प्रतिज्ञा(rfp);
      vvp_signal_मानम्*sig = dynamic_cast<vvp_signal_मानम्*>(rfp->net->fil);
      प्रतिज्ञा(sig);

      unsigned sig_size = sig->मानम्_size();
      unsigned width = rfp->width;
      पूर्णाङ्क base = PV_get_base(rfp);
      सचे (base >= (signed) sig_size) पटिनिवत्त 0;
      सचे (base + (signed) width < 0) पटिनिवत्त 0;

      vvp_vector4_t val;
      सचे (flags != vpiReleaseFlag) {
	    val = vec4_from_vpi_मानम्(vp, width);
      }

	/*
	 * If the base is less than zero then trim off any unneeded
	 * lower bits.
	 */
      सचे (base < 0) {
	    width += base;
	    सचे (flags != vpiReleaseFlag) {
		  val = val.subvalue(-base, width);
	    }
	    base = 0;
      }

	/*
	 * If the value is wider than the signal then trim off any
	 * unneeded upper bits.
	 */
      सचे (base+width > sig_size) {
	    width = sig_size - base;
	    सचे (flags != vpiReleaseFlag) {
		  val = val.subvalue(0, width);
	    }
      }

      प्रतिज्ञा(rfp->parent);
      तार्किक net_flag = rfp->parent->get_type_code()==vpiNet;
      तार्किक full_sig = base == 0 && width == sig_size;

      vvp_net_ptr_t dest(rfp->net, 0);

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value.  Note that
	   if this net is attached to an island, we need to rerun
	   the calculations immediately so we can return the
	   released value.*/
      सचे (flags == vpiReleaseFlag) {
	    प्रतिज्ञा(rfp->net->fil);
	      // XXXX Can't really do this if this is a partial release?
	    rfp->net->fil->force_unlink();
	    सचे (full_sig) {
		  rfp->net->fil->release(dest, net_flag);
	    } अञ्ञथा {
		  rfp->net->fil->release_pv(dest, base, width, net_flag);
	    }
	    rfp->net->fun->force_flag(सच्च);
	    PV_get_मानम्(ref, vp);
	    पटिनिवत्त ref;
      }

      सचे (flags == vpiForceFlag) {
	    सचे (full_sig) {
		  vvp_vector2_t mask (vvp_vector2_t::FILL1, sig_size);
		  rfp->net->force_vec4(val, mask);
	    } अञ्ञथा {
		  vvp_vector2_t mask (vvp_vector2_t::FILL0, sig_size);
		  अत्थाय (unsigned idx = 0 ; idx < width ; idx += 1)
		        mask.set_bit(base+idx, 1);

		  vvp_vector4_t tmp (sig_size, BIT4_Z);

		    // vvp_net_t::force_vec4 propagates all the bits of the
		    // forced vector value, regardless of the mask. This
		    // ensures the unforced bits retain their current value.
		  sig->vec4_मानम्(tmp);

		  tmp.set_vec(base, val);
		  rfp->net->force_vec4(tmp, mask);
	    }
      } अञ्ञथा सचे (net_flag && !dynamic_cast<vvp_island_port*>(rfp->net->fun)) {
	    सचे (full_sig) {
		  rfp->net->send_vec4(val, vthread_get_wt_context());
	    } अञ्ञथा {
		  rfp->net->send_vec4_pv(val, base, sig_size,
		                         vthread_get_wt_context());
	    }
      } अञ्ञथा {
	    सचे (full_sig) {
		  vvp_send_vec4(dest, val, vthread_get_wt_context());
	    } अञ्ञथा {
		  vvp_send_vec4_pv(dest, val, base, sig_size,
	                           vthread_get_wt_context());
	    }
      }
      पटिनिवत्त 0;
}

स्थिर vpiHandle PV_get_handle(पूर्णाङ्क code, vpiHandle ref)
{
      संरचना __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      प्रतिज्ञा(rfp);

      विकल्प (code) {
	  पक्ष vpiParent:
	    पटिनिवत्त rfp->parent;

	  पक्ष vpiScope:
	    पटिनिवत्त vpi_handle(vpiScope, rfp->parent);

	  पक्ष vpiModule:
	    पटिनिवत्त vpi_handle(vpiModule, rfp->parent);
      }

      पटिनिवत्त 0;
}

अन्तःस्थ __vpiPV::__vpiPV()
{ }

पूर्णाङ्क __vpiPV::get_type_code(शून्य) ध्रुव
{ पटिनिवत्त vpiPartSelect; }

पूर्णाङ्क __vpiPV::vpi_get(पूर्णाङ्क code)
{ पटिनिवत्त PV_get(code, एतत्); }

वर्ण* __vpiPV::vpi_get_str(पूर्णाङ्क code)
{ पटिनिवत्त PV_get_str(code, एतत्); }

शून्य __vpiPV::vpi_get_मानम्(p_vpi_मानम् val)
{ PV_get_मानम्(एतत्, val); }

vpiHandle __vpiPV::vpi_put_मानम्(p_vpi_मानम् val, पूर्णाङ्क flags)
{ पटिनिवत्त PV_put_मानम्(एतत्, val, flags); }

vpiHandle __vpiPV::vpi_handle(पूर्णाङ्क code)
{ पटिनिवत्त PV_get_handle(code, एतत्); }

vpiHandle vpip_make_PV(वर्ण*var, पूर्णाङ्क base, पूर्णाङ्क width)
{
      संरचना __vpiPV*obj = नूतन __vpiPV;
      compile_vpi_lookup(&obj->parent, strdup(var));
      obj->sbase = 0;
      obj->tbase = base;
      obj->width = (unsigned) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      पटिनिवत्त obj;
}

vpiHandle vpip_make_PV(वर्ण*var, वर्ण*चिह्नम्, पूर्णाङ्क width)
{
      संरचना __vpiPV*obj = नूतन __vpiPV;
      compile_vpi_lookup(&obj->parent, strdup(var));
      compile_vpi_lookup(&obj->sbase, चिह्नम्);
      obj->tbase = 0;
      obj->width = (unsigned) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      पटिनिवत्त obj;
}

vpiHandle vpip_make_PV(वर्ण*var, vpiHandle handle, पूर्णाङ्क width)
{
      संरचना __vpiPV*obj = नूतन __vpiPV;
      compile_vpi_lookup(&obj->parent, strdup(var));
      obj->sbase = handle;
      obj->tbase = 0;
      obj->width = (unsigned) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      पटिनिवत्त obj;
}

#ifdef CHECK_WITH_VALGRIND
शून्य PV_delete(vpiHandle item)
{
      संरचना __vpiPV *obj = dynamic_cast<__vpiPV*>(item);
      सचे (obj->sbase) {
	    विकल्प (obj->sbase->get_type_code()) {
		पक्ष vpiMemoryWord:
		  सचे (vpi_get(_vpiFromThr, obj->sbase) == _vpi_at_A) {
			A_delete(obj->sbase);
		  }
		  भङ्ग;
		पक्ष vpiPartSelect:
		  प्रतिज्ञा(vpi_get(_vpiFromThr, obj->sbase) == _vpi_at_PV);
		  PV_delete(obj->sbase);
		  भङ्ग;
	    }
      }
      प्रतिज्ञा(obj->net->fil);
      obj->net->fil->clear_all_callbacks();
      delete obj;
}
#endif
