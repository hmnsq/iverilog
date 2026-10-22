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
बाह्य स्थिरांक अक्षर hex_digits[256];
बाह्य स्थिरांक अक्षर oct_digits[64];

/*
 * The string values need a result buf to hold the results. This
 * buffer can be reused for that purpose. Whenever I have a need, the
 * need_result_buf function makes sure that need can be met.
 */
शून्य *need_result_buf(अचिन्हित cnt, vpi_rbuf_t type)
{
      स्थिर शून्य*result_buf[2] = {0, 0};
      स्थिर माप_प्रकार result_buf_size[2] = {0, 0};

      जर (type == RBUF_DEL) {
	    मुक्त(result_buf[RBUF_VAL]);
	    result_buf[RBUF_VAL] = 0;
	    result_buf_size[RBUF_VAL] = 0;

	    मुक्त(result_buf[RBUF_STR]);
	    result_buf[RBUF_STR] = 0;
	    result_buf_size[RBUF_STR] = 0;

	    परत 0;
      }

      cnt = (cnt + 0x0fff) & ~0x0fff;

      जर (result_buf_size[type] == 0) {
	    result_buf[type] = दो_स्मृति(cnt);
	    result_buf_size[type] = cnt;
      } अन्यथा जर (result_buf_size[type] < cnt) {
	    result_buf[type] = पुनः_स्मृति(result_buf[type], cnt);
	    result_buf_size[type] = cnt;
      }

      परत result_buf[type];
}

अक्षर *simple_set_rbuf_str(स्थिरांक अक्षर *s1)
{
      अक्षर *res = static_cast<अक्षर *>(need_result_buf(म_माप(s1)+1, RBUF_STR));
      जर (res) म_नकल(res,s1);
      परत res;
}

अक्षर *generic_get_str(पूर्णांक code, vpiHandle ref, स्थिरांक अक्षर *name, स्थिरांक अक्षर *index)
{
      माप_प्रकार len = म_माप(name) + 1;  /* include space for null termination */
      अक्षर *bn = रिक्त;
      तार्किक is_bn_a_pkg = असत्य;
      जर (code == vpiFullName) {
	    bn = strdup(vpi_get_str(code,ref));
	    माप_प्रकार bn_len = म_माप(bn);
	    is_bn_a_pkg = (bn_len >= 3) && (bn[bn_len-1] == ':') && (bn[bn_len-2] == ':');
	    len += bn_len;
	    जर (! is_bn_a_pkg) len += 1; // include space for "." separator
      }
      जर (index != रिक्त) len += म_माप(index) + 2;  /* include space for brackets */

      अक्षर *res = static_cast<अक्षर *>(need_result_buf(len, RBUF_STR));
      जर (!res) {
	    मुक्त(bn);
	    परत रिक्त;
      }
      *res=0;  /* start with nothing */

	/* if this works, I can make it more efficient later */
      जर (bn != रिक्त) {
	    म_जोड़ो(res, bn);
	      // A package already has the "::" separator in the name
	    जर (! is_bn_a_pkg) म_जोड़ो(res, ".");
	    मुक्त(bn);
      }
      म_जोड़ो(res, name);
      जर (index != रिक्त) {
	    म_जोड़ो(res, "[");
	    म_जोड़ो(res, index);
	    म_जोड़ो(res, "]");
      }
      परत res;
}

स्थिर vpiHandle fill_in_net4(संरचना __vpiSignal*obj, __vpiScope*scope,
                              स्थिरांक अक्षर*name, पूर्णांक msb, पूर्णांक lsb,
                              तार्किक signed_flag, vvp_net_t*गाठ);

स्थिर vpiHandle fill_in_var4(संरचना __vpiSignal*obj,
                              स्थिरांक अक्षर*name, पूर्णांक msb, पूर्णांक lsb,
                              तार्किक signed_flag, vvp_net_t*गाठ)
{
	// Variable declarations are always resolved immediately,
	// so we can assume they belong in the current scope.
      परत fill_in_net4(obj, vpip_peek_current_scope(),
			  name, msb, lsb, signed_flag, गाठ);
}

/*
 * The standard formatting/conversion routines.
 * They work with full or partial signals.
 */

स्थिर शून्य format_vpiBinStrVal(स्थिरांक vvp_signal_मूल्य*sig, पूर्णांक base, अचिन्हित wid,
                                s_vpi_मूल्य*vp)
{
      अक्षर *rbuf = static_cast<अक्षर *>(need_result_buf(wid+1, RBUF_VAL));
      दीर्घ end = base + (चिन्हित)wid;
      दीर्घ offset = end - 1;
      दीर्घ ssize = (चिन्हित)sig->मूल्य_size();

      साठी (दीर्घ idx = base ;  idx < end ;  idx += 1) {
	    जर (idx < 0 || idx >= ssize) {
                  rbuf[offset-idx] = 'x';
	    } अन्यथा {
                  rbuf[offset-idx] = vvp_bit4_to_ascii(sig->value(idx));
	    }
      }
      rbuf[wid] = 0;

      vp->value.str = rbuf;
}

स्थिर शून्य format_vpiOctStrVal(स्थिरांक vvp_signal_मूल्य*sig, पूर्णांक base, अचिन्हित wid,
                                s_vpi_मूल्य*vp)
{
      अचिन्हित dwid = (wid + 2) / 3;
      अक्षर *rbuf = static_cast<अक्षर *>(need_result_buf(dwid+1, RBUF_VAL));
      दीर्घ end = base + (चिन्हित)wid;
      दीर्घ ssize = (चिन्हित)sig->मूल्य_size();
      अचिन्हित val = 0;

      rbuf[dwid] = 0;
      साठी (दीर्घ idx = base ;  idx < end ;  idx += 1) {
	    अचिन्हित bit = 0;
	    जर (idx < 0 || idx >= ssize) {
                  bit = 2; // BIT4_X
	    } अन्यथा {
                  निवड (sig->value(idx)) {
		      स्थिती BIT4_0:
			bit = 0;
			थांबा;
		      स्थिती BIT4_1:
			bit = 1;
			थांबा;
		      स्थिती BIT4_X:
			bit = 2;
			थांबा;
		      स्थिती BIT4_Z:
			bit = 3;
			थांबा;
                  }
	    }
	    val |= bit << 2*((idx-base) % 3);

	    जर ((idx-base) % 3 == 2) {
		dwid -= 1;
		rbuf[dwid] = oct_digits[val];
		val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      निवड (wid % 3) {
	  स्थिती 1:
	    जर (val == 2) val = 42;
	    अन्यथा जर (val == 3) val = 63;
	    थांबा;
	  स्थिती 2:
	    जर (val == 10) val = 42;
	    अन्यथा जर (val == 15) val = 63;
	    थांबा;
      }

      जर (dwid > 0) rbuf[0] = oct_digits[val];

      vp->value.str = rbuf;
}

स्थिर शून्य format_vpiHexStrVal(स्थिरांक vvp_signal_मूल्य*sig, पूर्णांक base, अचिन्हित wid,
                                s_vpi_मूल्य*vp)
{
      अचिन्हित dwid = (wid + 3) / 4;
      अक्षर *rbuf = static_cast<अक्षर *>(need_result_buf(dwid+1, RBUF_VAL));
      दीर्घ end = base + (चिन्हित)wid;
      दीर्घ ssize = (चिन्हित)sig->मूल्य_size();
      अचिन्हित val = 0;

      rbuf[dwid] = 0;
      साठी (दीर्घ idx = base ;  idx < end ;  idx += 1) {
	    अचिन्हित bit = 0;
	    जर (idx < 0 || idx >= ssize) {
                  bit = 2; // BIT4_X
	    } अन्यथा {
                  निवड (sig->value(idx)) {
		      स्थिती BIT4_0:
			bit = 0;
			थांबा;
		      स्थिती BIT4_1:
			bit = 1;
			थांबा;
		      स्थिती BIT4_X:
			bit = 2;
			थांबा;
		      स्थिती BIT4_Z:
			bit = 3;
			थांबा;
                  }
	    }
	    val |= bit << 2*((idx-base) % 4);

	    जर ((idx-base) % 4 == 3) {
		dwid -= 1;
		rbuf[dwid] = hex_digits[val];
		val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      निवड (wid % 4) {
	  स्थिती 1:
	    जर (val == 2) val = 170;
	    अन्यथा जर (val == 3) val = 255;
	    थांबा;
	  स्थिती 2:
	    जर (val == 10) val = 170;
	    अन्यथा जर (val == 15) val = 255;
	    थांबा;
	  स्थिती 3:
	    जर (val == 42) val = 170;
	    अन्यथा जर (val == 63) val = 255;
	    थांबा;
      }

      जर (dwid > 0) rbuf[0] = hex_digits[val];

      vp->value.str = rbuf;
}

स्थिर शून्य format_vpiDecStrVal(स्थिरांक vvp_signal_मूल्य*sig, पूर्णांक base, अचिन्हित wid,
                                पूर्णांक signed_flag, s_vpi_मूल्य*vp)
{
      अचिन्हित hwid = (sig->मूल्य_size()+2) / 3 + 1;
      अक्षर *rbuf = static_cast<अक्षर *>(need_result_buf(hwid, RBUF_VAL));
      दीर्घ ssize = (चिन्हित)sig->मूल्य_size();
      दीर्घ end = base + (चिन्हित)wid;

	/* Do we have an end outside of the real signal vector. */
      जर (base < 0 || end > ssize) {
	    तार्किक all_x = सत्य;
	    जर (end > ssize) end = ssize;
	    जर (base < 0) base = 0;
	    साठी (दीर्घ idx = base ;  idx < end ;  idx += 1) {
		  जर (sig->value(idx) != BIT4_X) {
			all_x = असत्य;
			थांबा;
		  }
	    }

	    जर (all_x) {
		  rbuf[0] = 'x';
	    } अन्यथा {
		  rbuf[0] = 'X';
	    }
	    rbuf[1] = 0;

	    vp->value.str = rbuf;
	    परत;
      }

      vvp_vector4_t vec4;
      जर (base == 0 && end == ssize) {
	    sig->vec4_मूल्य(vec4);
      } अन्यथा {
	    vvp_vector4_t tmp;
	    sig->vec4_मूल्य(tmp);
	    vec4 = tmp.subvalue(base, wid);
      }

      vpip_vec4_to_dec_str(vec4, rbuf, hwid, signed_flag);

      vp->value.str = rbuf;
}

स्थिर शून्य format_vpiIntVal(स्थिरांक vvp_signal_मूल्य*sig, पूर्णांक base, अचिन्हित wid,
                             पूर्णांक signed_flag, s_vpi_मूल्य*vp)
{
      vvp_vector4_t tmp;
      sig->vec4_मूल्य(tmp);
      vvp_vector4_t sub = tmp.subvalue(base, wid);

	// Normally, we'd be OK with just using long in the call to
	// vector4_to_value, but some compilers seem to take long as
	// distinct from int32_t AND int64_t. Since the condition is
	// constant, the compiler should eliminate the dead code.
      जर (आकार(vp->value.integer) == आकार(int32_t)) {
	    int32_t val = 0;
	    vector4_to_मूल्य(sub, val, signed_flag, असत्य);
	    vp->value.integer = val;
      } अन्यथा {
	    प्रतिज्ञा(आकार(vp->value.integer) == आकार(int64_t));
	    int64_t val = 0;
	    vector4_to_मूल्य(sub, val, signed_flag, असत्य);
	    vp->value.integer = val;
      }
}

स्थिर शून्य format_vpiRealVal(स्थिरांक vvp_signal_मूल्य*sig, पूर्णांक base, अचिन्हित wid,
                              पूर्णांक signed_flag, s_vpi_मूल्य*vp)
{
      vvp_vector4_t vec4(wid);
      दीर्घ ssize = (चिन्हित)sig->मूल्य_size();
      दीर्घ end = base + (चिन्हित)wid;
      जर (end > ssize) end = ssize;

      साठी (दीर्घ idx = (base < 0) ? 0 : base ;  idx < end ;  idx += 1) {
	    vec4.set_bit(idx-base, sig->value(idx));
      }

      vp->value.real = 0.0;
      vector4_to_मूल्य(vec4, vp->value.real, signed_flag);
}

स्थिर शून्य format_vpiStringVal(स्थिरांक vvp_signal_मूल्य*sig, पूर्णांक base, अचिन्हित wid,
                                s_vpi_मूल्य*vp)
{
      /* The result will use a character for each 8 bits of the
	 vector. Add one extra character for the highest bits that
	 don't form an 8 bit group. */
      अक्षर *rbuf = static_cast<अक्षर *>(need_result_buf(wid/8 + ((wid&7)!=0) + 1, RBUF_VAL));
      अक्षर *cp = rbuf;

      अक्षर tmp = 0;
      साठी (दीर्घ idx = base+(चिन्हित)wid-1; idx >= base; idx -= 1) {
	    tmp <<= 1;

	    जर (idx >=0 && idx < (चिन्हित)sig->मूल्य_size() &&
	        sig->value(idx) == BIT4_1) {
		   tmp |= 1;
	    }

	    जर (((idx-base)&7)==0){
		  /* Skip leading nulls. */
		  जर (tmp == 0 && cp == rbuf)
			सुरूठेवा;

		  /* Nulls in the middle get turned into spaces. */
		  *cp++ = tmp ? tmp : ' ';
		  tmp = 0;
	    }
      }
      *cp++ = 0;

      vp->value.str = rbuf;
}

स्थिर शून्य format_vpiScalarVal(स्थिरांक vvp_signal_मूल्य*sig, पूर्णांक base,
                                s_vpi_मूल्य*vp)
{
      जर (base >= 0 && base < (चिन्हित)sig->मूल्य_size()) {
	    निवड (sig->value(base)) {
		स्थिती BIT4_0:
		  vp->value.scalar = vpi0;
		  थांबा;
		स्थिती BIT4_1:
		  vp->value.scalar = vpi1;
		  थांबा;
		स्थिती BIT4_X: {
		  vvp_scalar_t strn = sig->scalar_मूल्य(base);
		  जर (strn.strength0() == 1) vp->value.scalar = vpiH;
		  अन्यथा जर (strn.strength1() == 1) vp->value.scalar = vpiL;
		  अन्यथा vp->value.scalar = vpiX;
		  थांबा;
		}
		स्थिती BIT4_Z:
		  vp->value.scalar = vpiZ;
		  थांबा;
	    }
      } अन्यथा {
	    vp->value.scalar = vpiX;
      }
}

स्थिर शून्य format_vpiStrengthVal(स्थिरांक vvp_signal_मूल्य*sig, पूर्णांक base,
                                  अचिन्हित wid, s_vpi_मूल्य*vp)
{
      दीर्घ end = base + (चिन्हित)wid;
      s_vpi_strengthval*op;

      op = static_cast<s_vpi_strengthval*>
           (need_result_buf(wid * आकार(s_vpi_strengthval), RBUF_VAL));

      साठी (दीर्घ idx = base ;  idx < end ;  idx += 1) {
	    जर (idx >=0 && idx < (चिन्हित)sig->मूल्य_size()) {
		  vvp_scalar_t val = sig->scalar_मूल्य(idx);

		  /* vvp_scalar_t strengths are 0-7, but the vpi strength
		     is bit0-bit7. This gets the vpi form of the strengths
		     from the vvp_scalar_t strengths. */
		  अचिन्हित s0 = 1 << val.strength0();
		  अचिन्हित s1 = 1 << val.strength1();

		  निवड (val.value()) {
		      स्थिती BIT4_0:
			op[idx-base].logic = vpi0;
			op[idx-base].s0 = s0|s1;
			op[idx-base].s1 = 0;
			थांबा;

		      स्थिती BIT4_1:
			op[idx-base].logic = vpi1;
			op[idx-base].s0 = 0;
			op[idx-base].s1 = s0|s1;
			थांबा;

		      स्थिती BIT4_X:
			op[idx-base].logic = vpiX;
			op[idx-base].s0 = s0;
			op[idx-base].s1 = s1;
			थांबा;

		      स्थिती BIT4_Z:
			op[idx-base].logic = vpiZ;
			op[idx-base].s0 = vpiHiZ;
			op[idx-base].s1 = vpiHiZ;
			थांबा;
		  }
	    } अन्यथा {
		  op[idx-base].logic = vpiX;
		  op[idx-base].s0 = vpiStrongDrive;
		  op[idx-base].s1 = vpiStrongDrive;
	    }
      }

      vp->value.strength = op;
}

स्थिर शून्य format_vpiVectorVal(स्थिरांक vvp_signal_मूल्य*sig, पूर्णांक base, अचिन्हित wid,
                                s_vpi_मूल्य*vp)
{
      दीर्घ end = base + (चिन्हित)wid;
      अचिन्हित पूर्णांक obit = 0;
      अचिन्हित hwid = (wid + 31)/32;

      s_vpi_vecval *op = static_cast<p_vpi_vecval>
                         (need_result_buf(hwid * आकार(s_vpi_vecval), RBUF_VAL));
      vp->value.vector = op;

      op->aval = op->bval = 0;
      साठी (दीर्घ idx = base ;  idx < end ;  idx += 1) {
	    जर (base >= 0 && base < (चिन्हित)sig->मूल्य_size()) {
		निवड (sig->value(idx)) {
		स्थिती BIT4_0:
		  op->aval &= ~(1 << obit);
		  op->bval &= ~(1 << obit);
		  थांबा;
		स्थिती BIT4_1:
		  op->aval |= (1 << obit);
		  op->bval &= ~(1 << obit);
		  थांबा;
		स्थिती BIT4_X:
		  op->aval |= (1 << obit);
		  op->bval |= (1 << obit);
		  थांबा;
		स्थिती BIT4_Z:
		  op->aval &= ~(1 << obit);
		  op->bval |= (1 << obit);
		  थांबा;
		}
	    } अन्यथा {  /* BIT4_X */
		  op->aval |= (1 << obit);
		  op->bval |= (1 << obit);
	    }

	    obit++;
	    जर (!(obit % 32)) {
		  op += 1;
		  जर ((op - vp->value.vector) < (सूचक_भेद_प्रकार)hwid)
			op->aval = op->bval = 0;
		  obit = 0;
	    }
      }
}

/*
 * implement vpi_get for vpiReg objects.
 */
स्थिर पूर्णांक signal_get(पूर्णांक code, vpiHandle ref)
{
      संरचना __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      प्रतिज्ञा(rfp);

      निवड (code) {
	  स्थिती vpi_ओळ_No:
	    परत 0;  // Not implemented for now!

	  स्थिती vpiSigned:
	    परत rfp->signed_flag != 0;

	  स्थिती vpiArray:
	    परत rfp->is_netarray != 0;

	  स्थिती vpiIndex: // This only works while we have a single index.
	    जर (rfp->is_netarray) {
		  s_vpi_मूल्य vp;
		  vp.format = vpiIntVal;
		  vpi_get_मूल्य(rfp->id.index, &vp);
		  परत vp.value.integer;
	    } अन्यथा {
		  परत vpiUndefined;
	    }

	  स्थिती vpiSize:
	    परत rfp->width();

	  स्थिती vpiNetType:
	    जर (ref->get_type_code()==vpiNet)
		  परत vpiWire;
	    अन्यथा
		  परत vpiUndefined;

	  स्थिती vpi_डावा_Range:
            परत rfp->msb.get_मूल्य();

	  स्थिती vpi_उजवा_Range:
            परत rfp->lsb.get_मूल्य();

	  स्थिती vpiScalar:
	    परत (rfp->msb.get_मूल्य() == 0 && rfp->lsb.get_मूल्य() == 0);
	  स्थिती vpiVector:
	    परत (rfp->msb.get_मूल्य() != rfp->lsb.get_मूल्य());

          स्थिती vpiAutomatic:
            परत vpip_scope(rfp)->is_automatic() ? 1  : 0;

#ifdef BR916_STOPGAP_FIX
          स्थिती _vpiFromThr:
            परत _vpiNoThr;
#endif

	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "VPI error: unknown signal_get property %d.\n",
	            code);
	    परत vpiUndefined;
      }
}

/*
 * implement vpi_get64 for vpiReg objects.
 */
स्थिर int64_t signal_get64(पूर्णांक code, vpiHandle ref)
{
      संरचना __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      प्रतिज्ञा(rfp);

      निवड (code) {
	    // This private property must return zero when undefined.
	  स्थिती _vpiNexusId:
	    जर (rfp->msb.get_मूल्य() == rfp->lsb.get_मूल्य())
		  परत reinterpret_cast<int64_t>(rfp->गाठ);
	    परत 0;

	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "VPI error: unknown signal_get64 property %d.\n",
	            code);
	    परत vpiUndefined;
      }
}

स्थिर अक्षर* signal_get_str(पूर्णांक code, vpiHandle ref)
{
      संरचना __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      प्रतिज्ञा(rfp);

      जर (code == vpiFile) {  // Not implemented for now!
	    परत simple_set_rbuf_str(file_names[0]);
      }

      जर ((code != vpiName) && (code != vpiFullName)) परत रिक्त;

      अक्षर *nm;
      स्थिरांक अक्षर *ixs;
      जर (rfp->is_netarray) {
	    nm = strdup(vpi_get_str(vpiName, rfp->within.parent));
	    s_vpi_मूल्य vp;
	    vp.format = vpiDecStrVal;
	    vpi_get_मूल्य(rfp->id.index, &vp);
	    ixs = vp.value.str;  /* do I need to strdup() this? */
      } अन्यथा {
	    nm = strdup(rfp->id.name);
	    ixs = रिक्त;
      }

	/* The scope information is added here for vpiFullName. */
      अक्षर *rbuf = generic_get_str(code, vpip_scope(rfp), nm, ixs);
      मुक्त(nm);
      परत rbuf;
}

स्थिर vpiHandle signal_get_handle(पूर्णांक code, vpiHandle ref)
{
      संरचना __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      प्रतिज्ञा(rfp);

      निवड (code) {

	  स्थिती vpiParent:
	    परत rfp->is_netarray? rfp->within.parent : रिक्त;

	  स्थिती vpiIndex:
	    परत rfp->is_netarray? rfp->id.index : रिक्त;

	  स्थिती vpi_डावा_Range:
	    परत &rfp->msb;
	  स्थिती vpi_उजवा_Range:
	    परत &rfp->lsb;

	  स्थिती vpiScope:
	    परत vpip_scope(rfp);

	  स्थिती vpiModule:
	    परत vpip_module(vpip_scope(rfp));
      }

      परत 0;
}

स्थिर vpiHandle signal_iterate(पूर्णांक code, vpiHandle ref)
{
      संरचना __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      प्रतिज्ञा(rfp);

      जर (code == vpiIndex) {
	    परत rfp->is_netarray ? rfp->id.index->vpi_iterate(code) : रिक्त;
      }

      परत 0;
}

शून्य __vpiSignal::make_bits()
{
      प्रतिज्ञा(bits == रिक्त);
      bits = नवीन संरचना __vpiBit[width() + 1];

	// Make word[-1] point to the parent.
      bits->parent = हे;
	// Now point to word[0].
      bits += 1;

	// Update each bit to point to the base
      संरचना __vpiBit*base = bits;
      साठी (अचिन्हित idx = 0; idx < width(); idx += 1) {
	    base[idx].bit0 = base;
	    पूर्णांक real_idx;
	    जर (msb.get_मूल्य() >= lsb.get_मूल्य()) {
		  real_idx = idx + lsb.get_मूल्य();
	    } अन्यथा {
		  real_idx = lsb.get_मूल्य() - idx;
	    }
	    base[idx].index = नवीन __vpiDecConst(real_idx);
      }
}

vpiHandle __vpiSignal::get_index(पूर्णांक idx)
{
	/* Check to see if the index is in range. */
      जर (msb.get_मूल्य() >= lsb.get_मूल्य()) {
	    जर ((idx > msb.get_मूल्य()) || (idx < lsb.get_मूल्य())) परत 0;
      } अन्यथा {
	    जर ((idx < msb.get_मूल्य()) || (idx > lsb.get_मूल्य())) परत 0;
      }

	/* Normalize the index */
      अचिन्हित norm_idx;
      जर (msb.get_मूल्य() >= lsb.get_मूल्य()) {
	    norm_idx = idx - lsb.get_मूल्य();
      } अन्यथा {
	    norm_idx = lsb.get_मूल्य() - idx;
      }

      जर (bits == रिक्त) make_bits();

      परत &(bits[norm_idx].as_bit);
}

शून्य __vpiSignal::get_bit_मूल्य(स्थिरांक संरचना __vpiBit*bit, p_vpi_मूल्य vp)
{
      अचिन्हित index = bit->get_norm_index();

      vvp_signal_मूल्य*vsig = dynamic_cast<vvp_signal_मूल्य*>(गाठ->fil);
      प्रतिज्ञा(vsig);

      जर (vp->format == vpiObjTypeVal) {
	    vp->format = vpiIntVal;
      }

      निवड (vp->format) {
	स्थिती vpiBinStrVal:
            format_vpiBinStrVal(vsig, index, 1, vp);
	    थांबा;

	स्थिती vpiOctStrVal:
            format_vpiOctStrVal(vsig, index, 1, vp);
	    थांबा;

	स्थिती vpiDecStrVal:
            format_vpiDecStrVal(vsig, index, 1, असत्य, vp);
	    थांबा;

	स्थिती vpiHexStrVal:
            format_vpiHexStrVal(vsig, index, 1, vp);
	    थांबा;

	स्थिती vpiStringVal:
            format_vpiStringVal(vsig, index, 1, vp);
	    थांबा;

	स्थिती vpiIntVal:
            format_vpiIntVal(vsig, index, 1, असत्य, vp);
	    थांबा;

	स्थिती vpiRealVal:
            format_vpiRealVal(vsig, index, 1, असत्य, vp);
	    थांबा;

	स्थिती vpiScalarVal:
            format_vpiScalarVal(vsig, index, vp);
	    थांबा;

        स्थिती vpiStrengthVal:
            format_vpiStrengthVal(vsig, index, 1, vp);
            थांबा;

	स्थिती vpiVectorVal:
            format_vpiVectorVal(vsig, index, 1, vp);
	    थांबा;

	पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "internal error: get_value() "
		    "type %d is not implemented for bit "
		    "select %s[%d] in scope %s.\n",
		    (पूर्णांक)vp->format, vpi_get_str(vpiName),
		    bit->get_index(),
	            vpip_scope(हे)->scope_name());
	    प्रतिज्ञा(0);
      };
}

vpiHandle __vpiSignal::put_bit_मूल्य(संरचना __vpiBit*bit, p_vpi_मूल्य vp, पूर्णांक flags)
{
      अचिन्हित index = bit->get_norm_index();
      vvp_net_ptr_t dest(गाठ, 0);
      vvp_vector4_t val = vec4_from_vpi_मूल्य(vp, 1);

      जर ((flags == vpiForceFlag) || (flags == vpiReleaseFlag)) {
	    ख_लिखो(मानक_त्रुटि, "Sorry: vpi_put_value() for %s does not "
	                    "currently support force/release.\n",
	                    bit->as_bit.vpi_get_str(vpiFullName));
	    परत रिक्त;
      }

      जर ((get_type_code() == vpiNet) &&
          !dynamic_cast<vvp_island_port*>(गाठ->fun)) {
	    गाठ->send_vec4_pv(val, index, width(),
	                            vthread_get_wt_context());
      } अन्यथा {
	    vvp_send_vec4_pv(dest, val, index, width(),
	                     vthread_get_wt_context());
      }

	// This is not a scheduled event so there is no event to return
      परत रिक्त;
}

स्थिर vpiHandle signal_index(पूर्णांक idx, vpiHandle ref)
{
      संरचना __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      प्रतिज्ञा(rfp);

	/* We can only get the bit for a net or reg. */
      PLI_INT32 type = vpi_get(vpiType, ref);
      जर ((type != vpiNet) && (type != vpiReg)) परत 0;

      परत rfp->get_index(idx);
}

अचिन्हित __vpiSignal::width(शून्य) स्थिरांक
{
      अचिन्हित wid = (msb.get_मूल्य() >= lsb.get_मूल्य())
	    ? (msb.get_मूल्य() - lsb.get_मूल्य() + 1)
	    : (lsb.get_मूल्य() - msb.get_मूल्य() + 1);

      परत wid;
}

/*
 * The get_value method reads the values of the functors and returns
 * the vector to the caller. This causes no side-effect, and reads the
 * variables like a %load would.
 */
स्थिर शून्य signal_get_मूल्य(vpiHandle ref, s_vpi_मूल्य*vp)
{
      संरचना __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      प्रतिज्ञा(rfp);

      अचिन्हित wid = rfp->width();

      vvp_signal_मूल्य*vsig = dynamic_cast<vvp_signal_मूल्य*>(rfp->गाठ->fil);
      प्रतिज्ञा(vsig);

      निवड (vp->format) {

	  स्थिती vpiIntVal:
	    format_vpiIntVal(vsig, 0, wid, rfp->signed_flag, vp);
	    थांबा;

	  स्थिती vpiScalarVal:
	    format_vpiScalarVal(vsig, 0, vp);
	    थांबा;

	  स्थिती vpiStrengthVal:
	    format_vpiStrengthVal(vsig, 0, wid, vp);
	    थांबा;

	  स्थिती vpiBinStrVal:
	    format_vpiBinStrVal(vsig, 0, wid, vp);
	    थांबा;

	  स्थिती vpiHexStrVal:
	    format_vpiHexStrVal(vsig, 0, wid, vp);
	    थांबा;

	  स्थिती vpiOctStrVal:
	    format_vpiOctStrVal(vsig, 0, wid, vp);
	    थांबा;

	  स्थिती vpiDecStrVal:
	    format_vpiDecStrVal(vsig, 0, wid, rfp->signed_flag, vp);
	    थांबा;

	  स्थिती vpiStringVal:
	    format_vpiStringVal(vsig, 0, wid, vp);
	    थांबा;

	  स्थिती vpiVectorVal:
	    format_vpiVectorVal(vsig, 0, wid, vp);
	    थांबा;

	  स्थिती vpiRealVal:
	    format_vpiRealVal(vsig, 0, wid, rfp->signed_flag, vp);
	    थांबा;

	  स्थिती vpiObjTypeVal:
	    जर (wid == 1) {
		  vp->format = vpiScalarVal;
		  format_vpiScalarVal(vsig, 0, vp);
	    } अन्यथा {
		  vp->format = vpiVectorVal;
		  format_vpiVectorVal(vsig, 0, wid, vp);
	    }
	    थांबा;

	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "vvp internal error: get_value: "
		    "value type %d not implemented."
		    " Signal is %s in scope %s\n",
		    (पूर्णांक)vp->format, vpi_get_str(vpiName, ref),
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

स्थिर vvp_vector4_t from_stringval(स्थिरांक अक्षर*str, अचिन्हित wid)
{
      अचिन्हित idx;
      स्थिरांक अक्षर*cp;

      cp = str + म_माप(str);
      idx = 0;

      vvp_vector4_t val(wid, BIT4_0);

      पर्यंत ((idx < wid) && (cp > str)) {
	    अचिन्हित byte = *--cp;
	    पूर्णांक bit;

	    साठी (bit = 0 ;  (bit < 8) && (idx < wid) ;  bit += 1) {
		  जर (byte & 1)
			val.set_bit(idx, BIT4_1);

		  byte >>= 1;
		  idx += 1;
	    }
      }

      परत val;
}

स्थिर vpiHandle signal_put_मूल्य(vpiHandle ref, s_vpi_मूल्य*vp, पूर्णांक flags)
{
      अचिन्हित wid;
      संरचना __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      प्रतिज्ञा(rfp);
      vvp_net_ptr_t dest(rfp->गाठ, 0);

      तार्किक net_flag = ref->get_type_code()==vpiNet;

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value. Note that
	   if this net is attached to an island, we need to rerun
	   the calculations immediately so we can return the
	   released value. */
      जर (flags == vpiReleaseFlag) {
	    प्रतिज्ञा(rfp->गाठ->fil);
	    rfp->गाठ->fil->force_unlink();
	    rfp->गाठ->fil->release(dest, net_flag);
	    rfp->गाठ->fun->force_flag(सत्य);
	    signal_get_मूल्य(ref, vp);
	    परत ref;
      }

	/* Make a vvp_vector4_t vector to receive the translated value
	   that we are going to poke. This will get populated
	   differently depending on the format. */
      wid = (rfp->msb.get_मूल्य() >= rfp->lsb.get_मूल्य())
	    ? (rfp->msb.get_मूल्य() - rfp->lsb.get_मूल्य() + 1)
	    : (rfp->lsb.get_मूल्य() - rfp->msb.get_मूल्य() + 1);

      vvp_vector4_t val = vec4_from_vpi_मूल्य(vp, wid);

      जर (flags == vpiForceFlag) {
	    vvp_vector2_t mask (vvp_vector2_t::FILL1, wid);
	    rfp->गाठ->force_vec4(val, mask);
      } अन्यथा जर (net_flag && !dynamic_cast<vvp_island_port*>(rfp->गाठ->fun)) {
	    rfp->गाठ->send_vec4(val, vthread_get_wt_context());
      } अन्यथा {
	    vvp_send_vec4(dest, val, vthread_get_wt_context());
      }
      परत ref;
}

vvp_vector4_t vec4_from_vpi_मूल्य(s_vpi_मूल्य*vp, अचिन्हित wid)
{
      vvp_vector4_t val (wid, BIT4_0);

      निवड (vp->format) {

	  स्थिती vpiIntVal: {
		दीर्घ vpi_val = vp->value.integer;
		साठी (अचिन्हित idx = 0 ;  idx < wid ;  idx += 1) {
		      vvp_bit4_t bit = (vpi_val & 1) ? BIT4_1 : BIT4_0;
		      val.set_bit(idx, bit);
		      vpi_val >>= 1;
		}
		थांबा;
	  }

	  स्थिती vpiVectorVal:
	    साठी (अचिन्हित idx = 0 ;  idx < wid ;  idx += 1) {
		  अचिन्हित दीर्घ aval = vp->value.vector[idx/32].aval;
		  अचिन्हित दीर्घ bval = vp->value.vector[idx/32].bval;
		  aval >>= idx%32;
		  bval >>= idx%32;
		  पूर्णांक bitmask = (aval&1) | ((bval<<1)&2);
		  vvp_bit4_t bit = scalar_to_bit4(bitmask);
		  val.set_bit(idx, bit);
	    }
	    थांबा;
	  स्थिती vpiBinStrVal:
	    vpip_bin_str_to_vec4(val, vp->value.str);
	    थांबा;
	  स्थिती vpiOctStrVal:
	    vpip_oct_str_to_vec4(val, vp->value.str);
	    थांबा;
	  स्थिती vpiDecStrVal:
	    vpip_dec_str_to_vec4(val, vp->value.str);
	    थांबा;
	  स्थिती vpiHexStrVal:
	    vpip_hex_str_to_vec4(val, vp->value.str);
	    थांबा;
	  स्थिती vpiScalarVal:
	    val.set_bit(0, scalar_to_bit4(vp->value.scalar));
	    थांबा;
	  स्थिती vpiStringVal:
	    val = from_stringval(vp->value.str, wid);
	    थांबा;
	  स्थिती vpiRealVal:
	    val = vvp_vector4_t(wid, vp->value.real);
	    थांबा;

	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "vvp internal error: put_value: "
		    "value type %d not implemented here.\n",
		    (पूर्णांक)vp->format);
	    प्रतिज्ञा(0);

      }

      परत val;
}

पूर्णांक __vpiSignal::vpi_get(पूर्णांक code)
{ परत signal_get(code, हे); }

int64_t __vpiSignal::vpi_get64(पूर्णांक code)
{ परत signal_get64(code, हे); }

अक्षर* __vpiSignal::vpi_get_str(पूर्णांक code)
{ परत signal_get_str(code, हे); }

शून्य __vpiSignal::vpi_get_मूल्य(p_vpi_मूल्य val)
{ signal_get_मूल्य(हे, val); }

vpiHandle __vpiSignal::vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णांक flags)
{ परत signal_put_मूल्य(हे, val, flags); }

vpiHandle __vpiSignal::vpi_handle(पूर्णांक code)
{ परत signal_get_handle(code, हे); }

vpiHandle __vpiSignal::vpi_iterate(पूर्णांक code)
{ परत signal_iterate(code, हे); }

vpiHandle __vpiSignal::vpi_index(पूर्णांक idx)
{ परत signal_index(idx, हे); }

संरचना signal_reg : सार्वजनिक __vpiSignal {
      ओळीत signal_reg() { }
      पूर्णांक get_type_code(शून्य) स्थिरांक override { परत vpiReg; }
};

संरचना signal_integer : सार्वजनिक __vpiSignal {
      ओळीत signal_integer() { }
      पूर्णांक get_type_code(शून्य) स्थिरांक override { परत vpiIntegerVar; }
};

संरचना signal_net : सार्वजनिक __vpiSignal {
      ओळीत signal_net() { }
      पूर्णांक get_type_code(शून्य) स्थिरांक override { परत vpiNet; }
};

संरचना signal_byte : सार्वजनिक __vpiSignal {
      ओळीत signal_byte() { }
      पूर्णांक get_type_code(शून्य) स्थिरांक override { परत vpiByteVar; }
};

संरचना signal_bitvar : सार्वजनिक __vpiSignal {
      ओळीत signal_bitvar() { }
      पूर्णांक get_type_code(शून्य) स्थिरांक override { परत vpiBitVar; }
};

संरचना signal_shortint : सार्वजनिक __vpiSignal {
      ओळीत signal_shortint() { }
      पूर्णांक get_type_code(शून्य) स्थिरांक override { परत vpiShortIntVar; }
};

संरचना signal_int : सार्वजनिक __vpiSignal {
      ओळीत signal_int() { }
      पूर्णांक get_type_code(शून्य) स्थिरांक override { परत vpiIntVar; }
};

संरचना signal_longint : सार्वजनिक __vpiSignal {
      ओळीत signal_longint() { }
      पूर्णांक get_type_code(शून्य) स्थिरांक override { परत vpiLongIntVar; }
};


/*
 * Construct a vpiIntegerVar object. Indicate the type using a flag
 * to minimize the code modifications. Icarus implements integers
 * as 'reg signed [31:0]'.
 */
vpiHandle vpip_make_int4(स्थिरांक अक्षर*name, पूर्णांक msb, पूर्णांक lsb, vvp_net_t*vec)
{
      __vpiSignal*obj = नवीन signal_integer;
      परत fill_in_var4(obj, name, msb, lsb, सत्य, vec);
}

/*
 * Construct the two-state SystemVerilog variables.
 */
vpiHandle vpip_make_int2(स्थिरांक अक्षर*name, पूर्णांक msb, पूर्णांक lsb, तार्किक signed_flag,
                         vvp_net_t*vec)
{
      __vpiSignal*obj;

	// All unsigned 2-state variables are a vpiBitVar. All 2-state
	// variables with a non-zero lsb are also a vpiBitVar.
      जर ((! signed_flag) || (lsb != 0) ) {
	    obj = नवीन signal_bitvar;
      } अन्यथा {
	      // These could also be bit declarations with matching
	      // information, but for now they get the apparent type.
	    निवड (msb) {
		स्थिती 7:
		  obj = नवीन signal_byte;
		  थांबा;
		स्थिती 15:
		  obj = नवीन signal_shortint;
		  थांबा;
		स्थिती 31:
		  obj = नवीन signal_int;
		  थांबा;
		स्थिती 63:
		  obj = नवीन signal_longint;
		  थांबा;
		पूर्वनिर्धारित:
		    // Every other type of bit vector is a vpiBitVar with
		    // array dimensions.
		  obj = नवीन signal_bitvar;
		  थांबा;
	    }
      }

      परत fill_in_var4(obj, name, msb, lsb, signed_flag, vec);
}

/*
 * Construct a vpiReg/vpiLogicVar object. It's like a net, except for the type.
 */
vpiHandle vpip_make_var4(स्थिरांक अक्षर*name, पूर्णांक msb, पूर्णांक lsb,
			तार्किक signed_flag, vvp_net_t*vec)
{
      __vpiSignal*obj = नवीन signal_reg;
      परत fill_in_var4(obj, name, msb, lsb, signed_flag, vec);
}

#ifdef CHECK_WITH_VALGRIND
स्थिर संरचना vpiSignal_plug **signal_pool = 0;
स्थिर अचिन्हित signal_pool_मोजणी = 0;
स्थिर अचिन्हित दीर्घ signal_मोजणी = 0;
स्थिर अचिन्हित दीर्घ signal_dels = 0;
#endif

संरचना vpiSignal_plug {
      अचिन्हित अक्षर space[आकार (संरचना __vpiSignal)];
#ifdef CHECK_WITH_VALGRIND
      संरचना vpiSignal_plug *pool;
#endif
};

शून्य* __vpiSignal::चालक नवीन(माप_प्रकार siz)
{
#ifdef CHECK_WITH_VALGRIND
      प्रतिज्ञा(siz == आकार(संरचना vpiSignal_plug) - आकार(संरचना vpiSignal_plug *));
#अन्यथा
      प्रतिज्ञा(siz == आकार(संरचना vpiSignal_plug));
#endif
      स्थिर संरचना vpiSignal_plug*alloc_array = 0;
      स्थिर अचिन्हित alloc_index = 0;
      स्थिरांक अचिन्हित alloc_मोजणी = 512;

      जर ((alloc_array == 0) || (alloc_index == alloc_मोजणी)) {
	    alloc_array = static_cast<संरचना vpiSignal_plug*>
	                  (सुस्मृति(alloc_मोजणी, आकार(संरचना vpiSignal_plug)));
	    alloc_index = 0;
#ifdef CHECK_WITH_VALGRIND
	    VALGRIND_MAKE_MEM_NOACCESS(alloc_array, alloc_मोजणी *
	                                            आकार(संरचना vpiSignal_plug));
	    VALGRIND_CREATE_MEMPOOL(alloc_array, 0, 1);
	    signal_pool_मोजणी += 1;
	    signal_pool = static_cast<vpiSignal_plug **>
	                  (पुनः_स्मृति(signal_pool,
	                   signal_pool_मोजणी*आकार(vpiSignal_plug **)));
	    signal_pool[signal_pool_मोजणी-1] = alloc_array;
#endif
      }

      संरचना vpiSignal_plug*cur = alloc_array + alloc_index;
#ifdef CHECK_WITH_VALGRIND
      VALGRIND_MEMPOOL_ALLOC(alloc_array, cur, आकार(संरचना vpiSignal_plug));
      cur->pool = alloc_array;
      signal_मोजणी += 1;
#endif
      alloc_index += 1;
      परत cur;
}

शून्य __vpiSignal::चालक delete(शून्य*)
{
      प्रतिज्ञा(0);
}

#ifdef CHECK_WITH_VALGRIND
शून्य signal_delete(vpiHandle item)
{
      संरचना __vpiSignal *obj = static_cast<__vpiSignal *> (item);
      प्रतिज्ञा(obj->गाठ->fil);
      obj->गाठ->fil->clear_all_callbacks();
      vvp_net_delete(obj->गाठ);
      जर (obj->bits) {
	    साठी (अचिन्हित idx=0; idx<obj->width(); idx+=1) {
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
      जर (RUNNING_ON_VALGRIND && (signal_मोजणी != signal_dels)) {
	    ख_साफ(रिक्त);
	    VALGRIND_PRINTF("Error: vvp missed deleting %ld of %lu signal(s).",
	                    (दीर्घ) signal_मोजणी - signal_dels, signal_मोजणी);
      }

      साठी (अचिन्हित idx = 0; idx < signal_pool_मोजणी; idx += 1) {
	    VALGRIND_DESTROY_MEMPOOL(signal_pool[idx]);
	    मुक्त(signal_pool[idx]);
      }

      मुक्त(signal_pool);
      signal_pool = 0;
      signal_pool_मोजणी = 0;
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
			      स्थिरांक अक्षर*name, पूर्णांक msb, पूर्णांक lsb,
			      तार्किक signed_flag, vvp_net_t*गाठ)
{
      obj->id.name = name? vpip_name_string(name) : रिक्त;
      obj->msb = __vpiDecConst(msb);
      obj->lsb = __vpiDecConst(lsb);
      obj->signed_flag = signed_flag? 1 : 0;
      obj->is_netarray = 0;
      obj->गाठ = गाठ;

	// Place this object within a scope. If this object is
	// attached to an array, then this value will be replaced with
	// the handle to the parent.
      obj->within.scope = scope;

      मोजणी_vpi_nets += 1;

      परत obj;
}

vpiHandle vpip_make_net4(__vpiScope*scope,
			 स्थिरांक अक्षर*name, पूर्णांक msb, पूर्णांक lsb,
			 तार्किक signed_flag, vvp_net_t*गाठ)
{
      संरचना __vpiSignal*obj = नवीन signal_net;
      परत fill_in_net4(obj, scope, name, msb, lsb, signed_flag, गाठ);
}

स्थिर पूर्णांक PV_get_base(संरचना __vpiPV*rfp)
{
	/* We return from the symbol base if it is defined. */
      जर (rfp->sbase != 0) {
	    s_vpi_मूल्य val;
	      /* Check to see if the value is defined. */
	    val.format = vpiVectorVal;
	    vpi_get_मूल्य(rfp->sbase, &val);
	    पूर्णांक शब्द = (vpi_get(vpiSize, rfp->sbase)-1)/32 + 1;
	    साठी(पूर्णांक idx = 0; idx < शब्द; idx += 1) {
		    /* Return INT_MIN to indicate an X base. */
		  जर (val.value.vector[idx].bval != 0) परत पूर्णांक_न्यून;
	    }
	      /* The value is defined so get and return it. */
	    val.format = vpiIntVal;
	    vpi_get_मूल्य(rfp->sbase, &val);
	    परत val.value.integer;
      }

	/* If the width is zero then tbase is the constant. */
      परत rfp->tbase;
}

स्थिर पूर्णांक PV_get(पूर्णांक code, vpiHandle ref)
{
      संरचना __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      प्रतिज्ञा(rfp);

      पूर्णांक rval = 0;
      निवड (code) {
	स्थिती vpi_ओळ_No:
	    परत 0;  // Not implemented for now!

	स्थिती vpiSigned:
	    परत 0;  // A part/bit select is always unsigned!

	स्थिती vpiSize:
	    परत rfp->width;

	  /* This is like the &A<> in array.cc. */
	स्थिती vpiConstantSelect:
	    परत rfp->sbase == 0;

	स्थिती vpi_डावा_Range:
            rval += rfp->width - 1;
	    // fallthrough
	स्थिती vpi_उजवा_Range: {
	    पूर्णांक डावा = vpi_get(vpi_डावा_Range, rfp->parent);
	    पूर्णांक उजवा = vpi_get(vpi_उजवा_Range, rfp->parent);
	    rval += PV_get_base(rfp);
	    परत डावा >= उजवा ? उजवा + rval : उजवा - rval;
	}

        स्थिती vpiAutomatic:
            परत vpi_get(vpiAutomatic, rfp->parent);

#जर defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
        स्थिती _vpiFromThr:
            परत _vpi_at_PV;
#endif

	पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "PV_get: property %d is unknown\n", code);
      }

      परत 0;
}

स्थिर अक्षर* PV_get_str(पूर्णांक code, vpiHandle ref)
{
      संरचना __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      प्रतिज्ञा(rfp);

      निवड (code) {
	स्थिती vpiFile:  // Not implemented for now!
	    परत simple_set_rbuf_str(file_names[0]);

	स्थिती vpiName:
	स्थिती vpiFullName: {
	    स्थिरांक अक्षर*nm = vpi_get_str(code, rfp->parent);
	    माप_प्रकार len = 256+म_माप(nm);
	    अक्षर *full = static_cast<अक्षर *>(दो_स्मृति(len));
	    snprintf(full, len, "%s[%d:%d]", nm,
	                                     (पूर्णांक)vpi_get(vpi_डावा_Range, ref),
	                                     (पूर्णांक)vpi_get(vpi_उजवा_Range, ref));
	    full[len-1] = 0;
	    अक्षर *res = simple_set_rbuf_str(full);
	    मुक्त(full);
	    परत res;
	}

	पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "PV_get_str: property %d is unknown.\n", code);
      }

      परत 0;
}

स्थिर शून्य PV_get_मूल्य(vpiHandle ref, p_vpi_मूल्य vp)
{
      संरचना __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      प्रतिज्ञा(rfp);

      vvp_signal_मूल्य*sig = dynamic_cast<vvp_signal_मूल्य*>(rfp->net->fil);
      प्रतिज्ञा(sig);

      निवड (vp->format) {

	  स्थिती vpiIntVal:
	    format_vpiIntVal(sig, PV_get_base(rfp), rfp->width, 0, vp);
	    थांबा;

	  स्थिती vpiBinStrVal:
	    format_vpiBinStrVal(sig, PV_get_base(rfp), rfp->width, vp);
	    थांबा;

	  स्थिती vpiOctStrVal:
	    format_vpiOctStrVal(sig, PV_get_base(rfp), rfp->width, vp);
	    थांबा;

	  स्थिती vpiHexStrVal:
	    format_vpiHexStrVal(sig, PV_get_base(rfp), rfp->width, vp);
	    थांबा;

	  स्थिती vpiDecStrVal:
	    format_vpiDecStrVal(sig, PV_get_base(rfp), rfp->width, 0, vp);
	    थांबा;

	  स्थिती vpiStringVal:
	    format_vpiStringVal(sig, PV_get_base(rfp), rfp->width, vp);
	    थांबा;

	  स्थिती vpiScalarVal:
	    format_vpiScalarVal(sig, PV_get_base(rfp), vp);
	    थांबा;

	  स्थिती vpiStrengthVal:
	    format_vpiStrengthVal(sig, PV_get_base(rfp), rfp->width, vp);
	    थांबा;

	  स्थिती vpiVectorVal:
	    format_vpiVectorVal(sig, PV_get_base(rfp), rfp->width, vp);
	    थांबा;

	  स्थिती vpiRealVal:
	    format_vpiRealVal(sig, PV_get_base(rfp), rfp->width, 0, vp);
	    थांबा;

	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "vvp internal error: PV_get_value: "
		    "value type %d not implemented. Signal is %s.\n",
		    (पूर्णांक)vp->format, vpi_get_str(vpiFullName, rfp->parent));
	    प्रतिज्ञा(0);
      }
}

स्थिर vpiHandle PV_put_मूल्य(vpiHandle ref, p_vpi_मूल्य vp, पूर्णांक flags)
{
      संरचना __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      प्रतिज्ञा(rfp);
      vvp_signal_मूल्य*sig = dynamic_cast<vvp_signal_मूल्य*>(rfp->net->fil);
      प्रतिज्ञा(sig);

      अचिन्हित sig_size = sig->मूल्य_size();
      अचिन्हित width = rfp->width;
      पूर्णांक base = PV_get_base(rfp);
      जर (base >= (चिन्हित) sig_size) परत 0;
      जर (base + (चिन्हित) width < 0) परत 0;

      vvp_vector4_t val;
      जर (flags != vpiReleaseFlag) {
	    val = vec4_from_vpi_मूल्य(vp, width);
      }

	/*
	 * If the base is less than zero then trim off any unneeded
	 * lower bits.
	 */
      जर (base < 0) {
	    width += base;
	    जर (flags != vpiReleaseFlag) {
		  val = val.subvalue(-base, width);
	    }
	    base = 0;
      }

	/*
	 * If the value is wider than the signal then trim off any
	 * unneeded upper bits.
	 */
      जर (base+width > sig_size) {
	    width = sig_size - base;
	    जर (flags != vpiReleaseFlag) {
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
      जर (flags == vpiReleaseFlag) {
	    प्रतिज्ञा(rfp->net->fil);
	      // XXXX Can't really do this if this is a partial release?
	    rfp->net->fil->force_unlink();
	    जर (full_sig) {
		  rfp->net->fil->release(dest, net_flag);
	    } अन्यथा {
		  rfp->net->fil->release_pv(dest, base, width, net_flag);
	    }
	    rfp->net->fun->force_flag(सत्य);
	    PV_get_मूल्य(ref, vp);
	    परत ref;
      }

      जर (flags == vpiForceFlag) {
	    जर (full_sig) {
		  vvp_vector2_t mask (vvp_vector2_t::FILL1, sig_size);
		  rfp->net->force_vec4(val, mask);
	    } अन्यथा {
		  vvp_vector2_t mask (vvp_vector2_t::FILL0, sig_size);
		  साठी (अचिन्हित idx = 0 ; idx < width ; idx += 1)
		        mask.set_bit(base+idx, 1);

		  vvp_vector4_t tmp (sig_size, BIT4_Z);

		    // vvp_net_t::force_vec4 propagates all the bits of the
		    // forced vector value, regardless of the mask. This
		    // ensures the unforced bits retain their current value.
		  sig->vec4_मूल्य(tmp);

		  tmp.set_vec(base, val);
		  rfp->net->force_vec4(tmp, mask);
	    }
      } अन्यथा जर (net_flag && !dynamic_cast<vvp_island_port*>(rfp->net->fun)) {
	    जर (full_sig) {
		  rfp->net->send_vec4(val, vthread_get_wt_context());
	    } अन्यथा {
		  rfp->net->send_vec4_pv(val, base, sig_size,
		                         vthread_get_wt_context());
	    }
      } अन्यथा {
	    जर (full_sig) {
		  vvp_send_vec4(dest, val, vthread_get_wt_context());
	    } अन्यथा {
		  vvp_send_vec4_pv(dest, val, base, sig_size,
	                           vthread_get_wt_context());
	    }
      }
      परत 0;
}

स्थिर vpiHandle PV_get_handle(पूर्णांक code, vpiHandle ref)
{
      संरचना __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      प्रतिज्ञा(rfp);

      निवड (code) {
	  स्थिती vpiParent:
	    परत rfp->parent;

	  स्थिती vpiScope:
	    परत vpi_handle(vpiScope, rfp->parent);

	  स्थिती vpiModule:
	    परत vpi_handle(vpiModule, rfp->parent);
      }

      परत 0;
}

ओळीत __vpiPV::__vpiPV()
{ }

पूर्णांक __vpiPV::get_type_code(शून्य) स्थिरांक
{ परत vpiPartSelect; }

पूर्णांक __vpiPV::vpi_get(पूर्णांक code)
{ परत PV_get(code, हे); }

अक्षर* __vpiPV::vpi_get_str(पूर्णांक code)
{ परत PV_get_str(code, हे); }

शून्य __vpiPV::vpi_get_मूल्य(p_vpi_मूल्य val)
{ PV_get_मूल्य(हे, val); }

vpiHandle __vpiPV::vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णांक flags)
{ परत PV_put_मूल्य(हे, val, flags); }

vpiHandle __vpiPV::vpi_handle(पूर्णांक code)
{ परत PV_get_handle(code, हे); }

vpiHandle vpip_make_PV(अक्षर*var, पूर्णांक base, पूर्णांक width)
{
      संरचना __vpiPV*obj = नवीन __vpiPV;
      compile_vpi_lookup(&obj->parent, strdup(var));
      obj->sbase = 0;
      obj->tbase = base;
      obj->width = (अचिन्हित) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      परत obj;
}

vpiHandle vpip_make_PV(अक्षर*var, अक्षर*चिन्ह, पूर्णांक width)
{
      संरचना __vpiPV*obj = नवीन __vpiPV;
      compile_vpi_lookup(&obj->parent, strdup(var));
      compile_vpi_lookup(&obj->sbase, चिन्ह);
      obj->tbase = 0;
      obj->width = (अचिन्हित) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      परत obj;
}

vpiHandle vpip_make_PV(अक्षर*var, vpiHandle handle, पूर्णांक width)
{
      संरचना __vpiPV*obj = नवीन __vpiPV;
      compile_vpi_lookup(&obj->parent, strdup(var));
      obj->sbase = handle;
      obj->tbase = 0;
      obj->width = (अचिन्हित) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      परत obj;
}

#ifdef CHECK_WITH_VALGRIND
शून्य PV_delete(vpiHandle item)
{
      संरचना __vpiPV *obj = dynamic_cast<__vpiPV*>(item);
      जर (obj->sbase) {
	    निवड (obj->sbase->get_type_code()) {
		स्थिती vpiMemoryWord:
		  जर (vpi_get(_vpiFromThr, obj->sbase) == _vpi_at_A) {
			A_delete(obj->sbase);
		  }
		  थांबा;
		स्थिती vpiPartSelect:
		  प्रतिज्ञा(vpi_get(_vpiFromThr, obj->sbase) == _vpi_at_PV);
		  PV_delete(obj->sbase);
		  थांबा;
	    }
      }
      प्रतिज्ञा(obj->net->fil);
      obj->net->fil->clear_all_callbacks();
      delete obj;
}
#endif
