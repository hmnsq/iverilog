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
#अगर_घोषित CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#पूर्ण_अगर
# include  <cmath>
# include  <iostream>
# include  <cstdio>
# include  <cstdlib>
# include  <climits>
# include  <cstring>
# include  <cassert>
#अगर_घोषित CHECK_WITH_VALGRIND
# include  <valgrind/memcheck.h>
#पूर्ण_अगर
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
बाह्य स्थिर अक्षर hex_digits[256];
बाह्य स्थिर अक्षर oct_digits[64];

/*
 * The string values need a result buf to hold the results. This
 * buffer can be reused for that purpose. Whenever I have a need, the
 * need_result_buf function makes sure that need can be met.
 */
व्योम *need_परिणाम_buf(अचिन्हित cnt, vpi_rbuf_t type)
{
      अटल व्योम*परिणाम_buf[2] = {0, 0};
      अटल माप_प्रकार परिणाम_buf_आकार[2] = {0, 0};

      अगर (type == RBUF_DEL) {
	    मुक्त(परिणाम_buf[RBUF_VAL]);
	    परिणाम_buf[RBUF_VAL] = 0;
	    परिणाम_buf_आकार[RBUF_VAL] = 0;

	    मुक्त(परिणाम_buf[RBUF_STR]);
	    परिणाम_buf[RBUF_STR] = 0;
	    परिणाम_buf_आकार[RBUF_STR] = 0;

	    वापस 0;
      }

      cnt = (cnt + 0x0fff) & ~0x0fff;

      अगर (परिणाम_buf_आकार[type] == 0) {
	    परिणाम_buf[type] = दो_स्मृति(cnt);
	    परिणाम_buf_आकार[type] = cnt;
      } अन्यथा अगर (परिणाम_buf_आकार[type] < cnt) {
	    परिणाम_buf[type] = पुनः_स्मृति(परिणाम_buf[type], cnt);
	    परिणाम_buf_आकार[type] = cnt;
      }

      वापस परिणाम_buf[type];
}

अक्षर *simple_set_rbuf_str(स्थिर अक्षर *s1)
{
      अक्षर *res = static_cast<अक्षर *>(need_परिणाम_buf(म_माप(s1)+1, RBUF_STR));
      अगर (res) म_नकल(res,s1);
      वापस res;
}

अक्षर *generic_get_str(पूर्णांक code, vpiHandle ref, स्थिर अक्षर *name, स्थिर अक्षर *सूचकांक)
{
      माप_प्रकार len = म_माप(name) + 1;  /* include space for null termination */
      अक्षर *bn = शून्य;
      bool is_bn_a_pkg = false;
      अगर (code == vpiFullName) {
	    bn = strdup(vpi_get_str(code,ref));
	    माप_प्रकार bn_len = म_माप(bn);
	    is_bn_a_pkg = (bn_len >= 3) && (bn[bn_len-1] == ':') && (bn[bn_len-2] == ':');
	    len += bn_len;
	    अगर (! is_bn_a_pkg) len += 1; // include space for "." separator
      }
      अगर (सूचकांक != शून्य) len += म_माप(सूचकांक) + 2;  /* include space for brackets */

      अक्षर *res = static_cast<अक्षर *>(need_परिणाम_buf(len, RBUF_STR));
      अगर (!res) {
	    मुक्त(bn);
	    वापस शून्य;
      }
      *res=0;  /* start with nothing */

	/* if this works, I can make it more efficient later */
      अगर (bn != शून्य) {
	    म_जोड़ो(res, bn);
	      // A package already has the "::" separator in the name
	    अगर (! is_bn_a_pkg) म_जोड़ो(res, ".");
	    मुक्त(bn);
      }
      म_जोड़ो(res, name);
      अगर (सूचकांक != शून्य) {
	    म_जोड़ो(res, "[");
	    म_जोड़ो(res, सूचकांक);
	    म_जोड़ो(res, "]");
      }
      वापस res;
}

अटल vpiHandle fill_in_net4(काष्ठा __vpiSignal*obj, __vpiScope*scope,
                              स्थिर अक्षर*name, पूर्णांक msb, पूर्णांक lsb,
                              bool signed_flag, vvp_net_t*गाँठ);

अटल vpiHandle fill_in_var4(काष्ठा __vpiSignal*obj,
                              स्थिर अक्षर*name, पूर्णांक msb, पूर्णांक lsb,
                              bool signed_flag, vvp_net_t*गाँठ)
{
	// Variable declarations are always resolved immediately,
	// so we can assume they belong in the current scope.
      वापस fill_in_net4(obj, vpip_peek_current_scope(),
			  name, msb, lsb, signed_flag, गाँठ);
}

/*
 * The standard formatting/conversion routines.
 * They work with full or partial signals.
 */

अटल व्योम format_vpiBinStrVal(स्थिर vvp_signal_मूल्य*sig, पूर्णांक base, अचिन्हित wid,
                                s_vpi_मूल्य*vp)
{
      अक्षर *rbuf = static_cast<अक्षर *>(need_परिणाम_buf(wid+1, RBUF_VAL));
      दीर्घ end = base + (चिन्हित)wid;
      दीर्घ offset = end - 1;
      दीर्घ ssize = (चिन्हित)sig->मूल्य_आकार();

      क्रम (दीर्घ idx = base ;  idx < end ;  idx += 1) {
	    अगर (idx < 0 || idx >= ssize) {
                  rbuf[offset-idx] = 'x';
	    } अन्यथा {
                  rbuf[offset-idx] = vvp_bit4_to_ascii(sig->value(idx));
	    }
      }
      rbuf[wid] = 0;

      vp->value.str = rbuf;
}

अटल व्योम format_vpiOctStrVal(स्थिर vvp_signal_मूल्य*sig, पूर्णांक base, अचिन्हित wid,
                                s_vpi_मूल्य*vp)
{
      अचिन्हित dwid = (wid + 2) / 3;
      अक्षर *rbuf = static_cast<अक्षर *>(need_परिणाम_buf(dwid+1, RBUF_VAL));
      दीर्घ end = base + (चिन्हित)wid;
      दीर्घ ssize = (चिन्हित)sig->मूल्य_आकार();
      अचिन्हित val = 0;

      rbuf[dwid] = 0;
      क्रम (दीर्घ idx = base ;  idx < end ;  idx += 1) {
	    अचिन्हित bit = 0;
	    अगर (idx < 0 || idx >= ssize) {
                  bit = 2; // BIT4_X
	    } अन्यथा {
                  चयन (sig->value(idx)) {
		      हाल BIT4_0:
			bit = 0;
			अवरोध;
		      हाल BIT4_1:
			bit = 1;
			अवरोध;
		      हाल BIT4_X:
			bit = 2;
			अवरोध;
		      हाल BIT4_Z:
			bit = 3;
			अवरोध;
                  }
	    }
	    val |= bit << 2*((idx-base) % 3);

	    अगर ((idx-base) % 3 == 2) {
		dwid -= 1;
		rbuf[dwid] = oct_digits[val];
		val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      चयन (wid % 3) {
	  हाल 1:
	    अगर (val == 2) val = 42;
	    अन्यथा अगर (val == 3) val = 63;
	    अवरोध;
	  हाल 2:
	    अगर (val == 10) val = 42;
	    अन्यथा अगर (val == 15) val = 63;
	    अवरोध;
      }

      अगर (dwid > 0) rbuf[0] = oct_digits[val];

      vp->value.str = rbuf;
}

अटल व्योम format_vpiHexStrVal(स्थिर vvp_signal_मूल्य*sig, पूर्णांक base, अचिन्हित wid,
                                s_vpi_मूल्य*vp)
{
      अचिन्हित dwid = (wid + 3) / 4;
      अक्षर *rbuf = static_cast<अक्षर *>(need_परिणाम_buf(dwid+1, RBUF_VAL));
      दीर्घ end = base + (चिन्हित)wid;
      दीर्घ ssize = (चिन्हित)sig->मूल्य_आकार();
      अचिन्हित val = 0;

      rbuf[dwid] = 0;
      क्रम (दीर्घ idx = base ;  idx < end ;  idx += 1) {
	    अचिन्हित bit = 0;
	    अगर (idx < 0 || idx >= ssize) {
                  bit = 2; // BIT4_X
	    } अन्यथा {
                  चयन (sig->value(idx)) {
		      हाल BIT4_0:
			bit = 0;
			अवरोध;
		      हाल BIT4_1:
			bit = 1;
			अवरोध;
		      हाल BIT4_X:
			bit = 2;
			अवरोध;
		      हाल BIT4_Z:
			bit = 3;
			अवरोध;
                  }
	    }
	    val |= bit << 2*((idx-base) % 4);

	    अगर ((idx-base) % 4 == 3) {
		dwid -= 1;
		rbuf[dwid] = hex_digits[val];
		val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      चयन (wid % 4) {
	  हाल 1:
	    अगर (val == 2) val = 170;
	    अन्यथा अगर (val == 3) val = 255;
	    अवरोध;
	  हाल 2:
	    अगर (val == 10) val = 170;
	    अन्यथा अगर (val == 15) val = 255;
	    अवरोध;
	  हाल 3:
	    अगर (val == 42) val = 170;
	    अन्यथा अगर (val == 63) val = 255;
	    अवरोध;
      }

      अगर (dwid > 0) rbuf[0] = hex_digits[val];

      vp->value.str = rbuf;
}

अटल व्योम format_vpiDecStrVal(स्थिर vvp_signal_मूल्य*sig, पूर्णांक base, अचिन्हित wid,
                                पूर्णांक signed_flag, s_vpi_मूल्य*vp)
{
      अचिन्हित hwid = (sig->मूल्य_आकार()+2) / 3 + 1;
      अक्षर *rbuf = static_cast<अक्षर *>(need_परिणाम_buf(hwid, RBUF_VAL));
      दीर्घ ssize = (चिन्हित)sig->मूल्य_आकार();
      दीर्घ end = base + (चिन्हित)wid;

	/* Do we have an end outside of the real signal vector. */
      अगर (base < 0 || end > ssize) {
	    bool all_x = true;
	    अगर (end > ssize) end = ssize;
	    अगर (base < 0) base = 0;
	    क्रम (दीर्घ idx = base ;  idx < end ;  idx += 1) {
		  अगर (sig->value(idx) != BIT4_X) {
			all_x = false;
			अवरोध;
		  }
	    }

	    अगर (all_x) {
		  rbuf[0] = 'x';
	    } अन्यथा {
		  rbuf[0] = 'X';
	    }
	    rbuf[1] = 0;

	    vp->value.str = rbuf;
	    वापस;
      }

      vvp_vector4_t vec4;
      अगर (base == 0 && end == ssize) {
	    sig->vec4_मूल्य(vec4);
      } अन्यथा {
	    vvp_vector4_t tmp;
	    sig->vec4_मूल्य(tmp);
	    vec4 = tmp.subvalue(base, wid);
      }

      vpip_vec4_to_dec_str(vec4, rbuf, hwid, signed_flag);

      vp->value.str = rbuf;
}

अटल व्योम format_vpiIntVal(स्थिर vvp_signal_मूल्य*sig, पूर्णांक base, अचिन्हित wid,
                             पूर्णांक signed_flag, s_vpi_मूल्य*vp)
{
      vvp_vector4_t tmp;
      sig->vec4_मूल्य(tmp);
      vvp_vector4_t sub = tmp.subvalue(base, wid);

	// Normally, we'd be OK with just using long in the call to
	// vector4_to_value, but some compilers seem to take long as
	// distinct from int32_t AND int64_t. Since the condition is
	// constant, the compiler should eliminate the dead code.
      अगर (माप(vp->value.integer) == माप(int32_t)) {
	    int32_t val = 0;
	    vector4_to_मूल्य(sub, val, signed_flag, false);
	    vp->value.integer = val;
      } अन्यथा {
	    निश्चित(माप(vp->value.integer) == माप(int64_t));
	    int64_t val = 0;
	    vector4_to_मूल्य(sub, val, signed_flag, false);
	    vp->value.integer = val;
      }
}

अटल व्योम format_vpiRealVal(स्थिर vvp_signal_मूल्य*sig, पूर्णांक base, अचिन्हित wid,
                              पूर्णांक signed_flag, s_vpi_मूल्य*vp)
{
      vvp_vector4_t vec4(wid);
      दीर्घ ssize = (चिन्हित)sig->मूल्य_आकार();
      दीर्घ end = base + (चिन्हित)wid;
      अगर (end > ssize) end = ssize;

      क्रम (दीर्घ idx = (base < 0) ? 0 : base ;  idx < end ;  idx += 1) {
	    vec4.set_bit(idx-base, sig->value(idx));
      }

      vp->value.real = 0.0;
      vector4_to_मूल्य(vec4, vp->value.real, signed_flag);
}

अटल व्योम format_vpiStringVal(स्थिर vvp_signal_मूल्य*sig, पूर्णांक base, अचिन्हित wid,
                                s_vpi_मूल्य*vp)
{
      /* The result will use a character for each 8 bits of the
	 vector. Add one extra character for the highest bits that
	 don't form an 8 bit group. */
      अक्षर *rbuf = static_cast<अक्षर *>(need_परिणाम_buf(wid/8 + ((wid&7)!=0) + 1, RBUF_VAL));
      अक्षर *cp = rbuf;

      अक्षर tmp = 0;
      क्रम (दीर्घ idx = base+(चिन्हित)wid-1; idx >= base; idx -= 1) {
	    tmp <<= 1;

	    अगर (idx >=0 && idx < (चिन्हित)sig->मूल्य_आकार() &&
	        sig->value(idx) == BIT4_1) {
		   tmp |= 1;
	    }

	    अगर (((idx-base)&7)==0){
		  /* Skip leading nulls. */
		  अगर (tmp == 0 && cp == rbuf)
			जारी;

		  /* Nulls in the middle get turned into spaces. */
		  *cp++ = tmp ? tmp : ' ';
		  tmp = 0;
	    }
      }
      *cp++ = 0;

      vp->value.str = rbuf;
}

अटल व्योम format_vpiScalarVal(स्थिर vvp_signal_मूल्य*sig, पूर्णांक base,
                                s_vpi_मूल्य*vp)
{
      अगर (base >= 0 && base < (चिन्हित)sig->मूल्य_आकार()) {
	    चयन (sig->value(base)) {
		हाल BIT4_0:
		  vp->value.scalar = vpi0;
		  अवरोध;
		हाल BIT4_1:
		  vp->value.scalar = vpi1;
		  अवरोध;
		हाल BIT4_X: {
		  vvp_scalar_t strn = sig->scalar_मूल्य(base);
		  अगर (strn.strength0() == 1) vp->value.scalar = vpiH;
		  अन्यथा अगर (strn.strength1() == 1) vp->value.scalar = vpiL;
		  अन्यथा vp->value.scalar = vpiX;
		  अवरोध;
		}
		हाल BIT4_Z:
		  vp->value.scalar = vpiZ;
		  अवरोध;
	    }
      } अन्यथा {
	    vp->value.scalar = vpiX;
      }
}

अटल व्योम format_vpiStrengthVal(स्थिर vvp_signal_मूल्य*sig, पूर्णांक base,
                                  अचिन्हित wid, s_vpi_मूल्य*vp)
{
      दीर्घ end = base + (चिन्हित)wid;
      s_vpi_strengthval*op;

      op = static_cast<s_vpi_strengthval*>
           (need_परिणाम_buf(wid * माप(s_vpi_strengthval), RBUF_VAL));

      क्रम (दीर्घ idx = base ;  idx < end ;  idx += 1) {
	    अगर (idx >=0 && idx < (चिन्हित)sig->मूल्य_आकार()) {
		  vvp_scalar_t val = sig->scalar_मूल्य(idx);

		  /* vvp_scalar_t strengths are 0-7, but the vpi strength
		     is bit0-bit7. This gets the vpi form of the strengths
		     from the vvp_scalar_t strengths. */
		  अचिन्हित s0 = 1 << val.strength0();
		  अचिन्हित s1 = 1 << val.strength1();

		  चयन (val.value()) {
		      हाल BIT4_0:
			op[idx-base].logic = vpi0;
			op[idx-base].s0 = s0|s1;
			op[idx-base].s1 = 0;
			अवरोध;

		      हाल BIT4_1:
			op[idx-base].logic = vpi1;
			op[idx-base].s0 = 0;
			op[idx-base].s1 = s0|s1;
			अवरोध;

		      हाल BIT4_X:
			op[idx-base].logic = vpiX;
			op[idx-base].s0 = s0;
			op[idx-base].s1 = s1;
			अवरोध;

		      हाल BIT4_Z:
			op[idx-base].logic = vpiZ;
			op[idx-base].s0 = vpiHiZ;
			op[idx-base].s1 = vpiHiZ;
			अवरोध;
		  }
	    } अन्यथा {
		  op[idx-base].logic = vpiX;
		  op[idx-base].s0 = vpiStrongDrive;
		  op[idx-base].s1 = vpiStrongDrive;
	    }
      }

      vp->value.strength = op;
}

अटल व्योम format_vpiVectorVal(स्थिर vvp_signal_मूल्य*sig, पूर्णांक base, अचिन्हित wid,
                                s_vpi_मूल्य*vp)
{
      दीर्घ end = base + (चिन्हित)wid;
      अचिन्हित पूर्णांक obit = 0;
      अचिन्हित hwid = (wid + 31)/32;

      s_vpi_vecval *op = static_cast<p_vpi_vecval>
                         (need_परिणाम_buf(hwid * माप(s_vpi_vecval), RBUF_VAL));
      vp->value.vector = op;

      op->aval = op->bval = 0;
      क्रम (दीर्घ idx = base ;  idx < end ;  idx += 1) {
	    अगर (base >= 0 && base < (चिन्हित)sig->मूल्य_आकार()) {
		चयन (sig->value(idx)) {
		हाल BIT4_0:
		  op->aval &= ~(1 << obit);
		  op->bval &= ~(1 << obit);
		  अवरोध;
		हाल BIT4_1:
		  op->aval |= (1 << obit);
		  op->bval &= ~(1 << obit);
		  अवरोध;
		हाल BIT4_X:
		  op->aval |= (1 << obit);
		  op->bval |= (1 << obit);
		  अवरोध;
		हाल BIT4_Z:
		  op->aval &= ~(1 << obit);
		  op->bval |= (1 << obit);
		  अवरोध;
		}
	    } अन्यथा {  /* BIT4_X */
		  op->aval |= (1 << obit);
		  op->bval |= (1 << obit);
	    }

	    obit++;
	    अगर (!(obit % 32)) {
		  op += 1;
		  अगर ((op - vp->value.vector) < (सूचक_भेद_प्रकार)hwid)
			op->aval = op->bval = 0;
		  obit = 0;
	    }
      }
}

/*
 * implement vpi_get for vpiReg objects.
 */
अटल पूर्णांक signal_get(पूर्णांक code, vpiHandle ref)
{
      काष्ठा __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      निश्चित(rfp);

      चयन (code) {
	  हाल vpi_पंक्ति_No:
	    वापस 0;  // Not implemented for now!

	  हाल vpiSigned:
	    वापस rfp->signed_flag != 0;

	  हाल vpiArray:
	    वापस rfp->is_netarray != 0;

	  हाल vpi_सूचकांक_2: // This only works while we have a single index.
	    अगर (rfp->is_netarray) {
		  s_vpi_मूल्य vp;
		  vp.format = vpiIntVal;
		  vpi_get_मूल्य(rfp->id.सूचकांक, &vp);
		  वापस vp.value.integer;
	    } अन्यथा {
		  वापस vpiUndefined;
	    }

	  हाल vpi_आकार:
	    वापस rfp->width();

	  हाल vpiNetType:
	    अगर (ref->get_type_code()==vpiNet)
		  वापस vpiWire;
	    अन्यथा
		  वापस vpiUndefined;

	  हाल vpi_बायाँ_Range:
            वापस rfp->msb.get_मूल्य();

	  हाल vpi_दायाँ_Range:
            वापस rfp->lsb.get_मूल्य();

	  हाल vpiScalar:
	    वापस (rfp->msb.get_मूल्य() == 0 && rfp->lsb.get_मूल्य() == 0);
	  हाल vpiVector:
	    वापस (rfp->msb.get_मूल्य() != rfp->lsb.get_मूल्य());

          हाल vpiAutomatic:
            वापस vpip_scope(rfp)->is_automatic() ? 1  : 0;

#अगर_घोषित BR916_STOPGAP_FIX
          हाल _vpiFromThr:
            वापस _vpiNoThr;
#पूर्ण_अगर

	  शेष:
	    ख_लिखो(मानक_त्रुटि, "VPI error: unknown signal_get property %d.\n",
	            code);
	    वापस vpiUndefined;
      }
}

/*
 * implement vpi_get64 for vpiReg objects.
 */
अटल int64_t signal_get64(पूर्णांक code, vpiHandle ref)
{
      काष्ठा __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      निश्चित(rfp);

      चयन (code) {
	    // This private property must return zero when undefined.
	  हाल _vpiNexusId:
	    अगर (rfp->msb.get_मूल्य() == rfp->lsb.get_मूल्य())
		  वापस reinterpret_cast<int64_t>(rfp->गाँठ);
	    वापस 0;

	  शेष:
	    ख_लिखो(मानक_त्रुटि, "VPI error: unknown signal_get64 property %d.\n",
	            code);
	    वापस vpiUndefined;
      }
}

अटल अक्षर* signal_get_str(पूर्णांक code, vpiHandle ref)
{
      काष्ठा __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      निश्चित(rfp);

      अगर (code == vpiFile) {  // Not implemented for now!
	    वापस simple_set_rbuf_str(file_names[0]);
      }

      अगर ((code != vpiName) && (code != vpiFullName)) वापस शून्य;

      अक्षर *nm;
      स्थिर अक्षर *ixs;
      अगर (rfp->is_netarray) {
	    nm = strdup(vpi_get_str(vpiName, rfp->within.parent));
	    s_vpi_मूल्य vp;
	    vp.format = vpiDecStrVal;
	    vpi_get_मूल्य(rfp->id.सूचकांक, &vp);
	    ixs = vp.value.str;  /* do I need to strdup() this? */
      } अन्यथा {
	    nm = strdup(rfp->id.name);
	    ixs = शून्य;
      }

	/* The scope information is added here for vpiFullName. */
      अक्षर *rbuf = generic_get_str(code, vpip_scope(rfp), nm, ixs);
      मुक्त(nm);
      वापस rbuf;
}

अटल vpiHandle signal_get_handle(पूर्णांक code, vpiHandle ref)
{
      काष्ठा __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      निश्चित(rfp);

      चयन (code) {

	  हाल vpiParent:
	    वापस rfp->is_netarray? rfp->within.parent : शून्य;

	  हाल vpi_सूचकांक_2:
	    वापस rfp->is_netarray? rfp->id.सूचकांक : शून्य;

	  हाल vpi_बायाँ_Range:
	    वापस &rfp->msb;
	  हाल vpi_दायाँ_Range:
	    वापस &rfp->lsb;

	  हाल vpiScope:
	    वापस vpip_scope(rfp);

	  हाल vpiModule:
	    वापस vpip_module(vpip_scope(rfp));
      }

      वापस 0;
}

अटल vpiHandle signal_iterate(पूर्णांक code, vpiHandle ref)
{
      काष्ठा __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      निश्चित(rfp);

      अगर (code == vpi_सूचकांक_2) {
	    वापस rfp->is_netarray ? rfp->id.सूचकांक->vpi_iterate(code) : शून्य;
      }

      वापस 0;
}

व्योम __vpiSignal::make_bits()
{
      निश्चित(bits == शून्य);
      bits = नया काष्ठा __vpiBit[width() + 1];

	// Make word[-1] point to the parent.
      bits->parent = यह;
	// Now point to word[0].
      bits += 1;

	// Update each bit to point to the base
      काष्ठा __vpiBit*base = bits;
      क्रम (अचिन्हित idx = 0; idx < width(); idx += 1) {
	    base[idx].bit0 = base;
	    पूर्णांक real_idx;
	    अगर (msb.get_मूल्य() >= lsb.get_मूल्य()) {
		  real_idx = idx + lsb.get_मूल्य();
	    } अन्यथा {
		  real_idx = lsb.get_मूल्य() - idx;
	    }
	    base[idx].सूचकांक = नया __vpiDecConst(real_idx);
      }
}

vpiHandle __vpiSignal::get_सूचकांक(पूर्णांक idx)
{
	/* Check to see if the index is in range. */
      अगर (msb.get_मूल्य() >= lsb.get_मूल्य()) {
	    अगर ((idx > msb.get_मूल्य()) || (idx < lsb.get_मूल्य())) वापस 0;
      } अन्यथा {
	    अगर ((idx < msb.get_मूल्य()) || (idx > lsb.get_मूल्य())) वापस 0;
      }

	/* Normalize the index */
      अचिन्हित norm_idx;
      अगर (msb.get_मूल्य() >= lsb.get_मूल्य()) {
	    norm_idx = idx - lsb.get_मूल्य();
      } अन्यथा {
	    norm_idx = lsb.get_मूल्य() - idx;
      }

      अगर (bits == शून्य) make_bits();

      वापस &(bits[norm_idx].as_bit);
}

व्योम __vpiSignal::get_bit_मूल्य(स्थिर काष्ठा __vpiBit*bit, p_vpi_मूल्य vp)
{
      अचिन्हित सूचकांक = bit->get_norm_सूचकांक();

      vvp_signal_मूल्य*vsig = dynamic_cast<vvp_signal_मूल्य*>(गाँठ->fil);
      निश्चित(vsig);

      अगर (vp->format == vpiObjTypeVal) {
	    vp->format = vpiIntVal;
      }

      चयन (vp->format) {
	हाल vpiBinStrVal:
            format_vpiBinStrVal(vsig, सूचकांक, 1, vp);
	    अवरोध;

	हाल vpiOctStrVal:
            format_vpiOctStrVal(vsig, सूचकांक, 1, vp);
	    अवरोध;

	हाल vpiDecStrVal:
            format_vpiDecStrVal(vsig, सूचकांक, 1, false, vp);
	    अवरोध;

	हाल vpiHexStrVal:
            format_vpiHexStrVal(vsig, सूचकांक, 1, vp);
	    अवरोध;

	हाल vpiStringVal:
            format_vpiStringVal(vsig, सूचकांक, 1, vp);
	    अवरोध;

	हाल vpiIntVal:
            format_vpiIntVal(vsig, सूचकांक, 1, false, vp);
	    अवरोध;

	हाल vpiRealVal:
            format_vpiRealVal(vsig, सूचकांक, 1, false, vp);
	    अवरोध;

	हाल vpiScalarVal:
            format_vpiScalarVal(vsig, सूचकांक, vp);
	    अवरोध;

        हाल vpiStrengthVal:
            format_vpiStrengthVal(vsig, सूचकांक, 1, vp);
            अवरोध;

	हाल vpiVectorVal:
            format_vpiVectorVal(vsig, सूचकांक, 1, vp);
	    अवरोध;

	शेष:
	    ख_लिखो(मानक_त्रुटि, "internal error: get_value() "
		    "type %d is not implemented for bit "
		    "select %s[%d] in scope %s.\n",
		    (पूर्णांक)vp->format, vpi_get_str(vpiName),
		    bit->get_सूचकांक(),
	            vpip_scope(यह)->scope_name());
	    निश्चित(0);
      };
}

vpiHandle __vpiSignal::put_bit_मूल्य(काष्ठा __vpiBit*bit, p_vpi_मूल्य vp, पूर्णांक flags)
{
      अचिन्हित सूचकांक = bit->get_norm_सूचकांक();
      vvp_net_ptr_t dest(गाँठ, 0);
      vvp_vector4_t val = vec4_from_vpi_मूल्य(vp, 1);

      अगर ((flags == vpiForceFlag) || (flags == vpiReleaseFlag)) {
	    ख_लिखो(मानक_त्रुटि, "Sorry: vpi_put_value() for %s does not "
	                    "currently support force/release.\n",
	                    bit->as_bit.vpi_get_str(vpiFullName));
	    वापस शून्य;
      }

      अगर ((get_type_code() == vpiNet) &&
          !dynamic_cast<vvp_island_port*>(गाँठ->fun)) {
	    गाँठ->send_vec4_pv(val, सूचकांक, width(),
	                            vthread_get_wt_context());
      } अन्यथा {
	    vvp_send_vec4_pv(dest, val, सूचकांक, width(),
	                     vthread_get_wt_context());
      }

	// This is not a scheduled event so there is no event to return
      वापस शून्य;
}

अटल vpiHandle signal_सूचकांक(पूर्णांक idx, vpiHandle ref)
{
      काष्ठा __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      निश्चित(rfp);

	/* We can only get the bit for a net or reg. */
      PLI_INT32 type = vpi_get(vpiType, ref);
      अगर ((type != vpiNet) && (type != vpiReg)) वापस 0;

      वापस rfp->get_सूचकांक(idx);
}

अचिन्हित __vpiSignal::width(व्योम) स्थिर
{
      अचिन्हित wid = (msb.get_मूल्य() >= lsb.get_मूल्य())
	    ? (msb.get_मूल्य() - lsb.get_मूल्य() + 1)
	    : (lsb.get_मूल्य() - msb.get_मूल्य() + 1);

      वापस wid;
}

/*
 * The get_value method reads the values of the functors and returns
 * the vector to the caller. This causes no side-effect, and reads the
 * variables like a %load would.
 */
अटल व्योम signal_get_मूल्य(vpiHandle ref, s_vpi_मूल्य*vp)
{
      काष्ठा __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      निश्चित(rfp);

      अचिन्हित wid = rfp->width();

      vvp_signal_मूल्य*vsig = dynamic_cast<vvp_signal_मूल्य*>(rfp->गाँठ->fil);
      निश्चित(vsig);

      चयन (vp->format) {

	  हाल vpiIntVal:
	    format_vpiIntVal(vsig, 0, wid, rfp->signed_flag, vp);
	    अवरोध;

	  हाल vpiScalarVal:
	    format_vpiScalarVal(vsig, 0, vp);
	    अवरोध;

	  हाल vpiStrengthVal:
	    format_vpiStrengthVal(vsig, 0, wid, vp);
	    अवरोध;

	  हाल vpiBinStrVal:
	    format_vpiBinStrVal(vsig, 0, wid, vp);
	    अवरोध;

	  हाल vpiHexStrVal:
	    format_vpiHexStrVal(vsig, 0, wid, vp);
	    अवरोध;

	  हाल vpiOctStrVal:
	    format_vpiOctStrVal(vsig, 0, wid, vp);
	    अवरोध;

	  हाल vpiDecStrVal:
	    format_vpiDecStrVal(vsig, 0, wid, rfp->signed_flag, vp);
	    अवरोध;

	  हाल vpiStringVal:
	    format_vpiStringVal(vsig, 0, wid, vp);
	    अवरोध;

	  हाल vpiVectorVal:
	    format_vpiVectorVal(vsig, 0, wid, vp);
	    अवरोध;

	  हाल vpiRealVal:
	    format_vpiRealVal(vsig, 0, wid, rfp->signed_flag, vp);
	    अवरोध;

	  हाल vpiObjTypeVal:
	    अगर (wid == 1) {
		  vp->format = vpiScalarVal;
		  format_vpiScalarVal(vsig, 0, vp);
	    } अन्यथा {
		  vp->format = vpiVectorVal;
		  format_vpiVectorVal(vsig, 0, wid, vp);
	    }
	    अवरोध;

	  शेष:
	    ख_लिखो(मानक_त्रुटि, "vvp internal error: get_value: "
		    "value type %d not implemented."
		    " Signal is %s in scope %s\n",
		    (पूर्णांक)vp->format, vpi_get_str(vpiName, ref),
		    vpip_scope(rfp)->scope_name());
	    निश्चित(0);
      }
}

/*
 * The put_value method writes the value into the vector, and returns
 * the affected ref. This operation works much like the %set or
 * %assign instructions and causes all the side-effects that the
 * equivalent instruction would cause.
 */

अटल vvp_vector4_t from_stringval(स्थिर अक्षर*str, अचिन्हित wid)
{
      अचिन्हित idx;
      स्थिर अक्षर*cp;

      cp = str + म_माप(str);
      idx = 0;

      vvp_vector4_t val(wid, BIT4_0);

      जबतक ((idx < wid) && (cp > str)) {
	    अचिन्हित byte = *--cp;
	    पूर्णांक bit;

	    क्रम (bit = 0 ;  (bit < 8) && (idx < wid) ;  bit += 1) {
		  अगर (byte & 1)
			val.set_bit(idx, BIT4_1);

		  byte >>= 1;
		  idx += 1;
	    }
      }

      वापस val;
}

अटल vpiHandle signal_put_मूल्य(vpiHandle ref, s_vpi_मूल्य*vp, पूर्णांक flags)
{
      अचिन्हित wid;
      काष्ठा __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      निश्चित(rfp);
      vvp_net_ptr_t dest(rfp->गाँठ, 0);

      bool net_flag = ref->get_type_code()==vpiNet;

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value. Note that
	   if this net is attached to an island, we need to rerun
	   the calculations immediately so we can return the
	   released value. */
      अगर (flags == vpiReleaseFlag) {
	    निश्चित(rfp->गाँठ->fil);
	    rfp->गाँठ->fil->force_unlink();
	    rfp->गाँठ->fil->release(dest, net_flag);
	    rfp->गाँठ->fun->force_flag(true);
	    signal_get_मूल्य(ref, vp);
	    वापस ref;
      }

	/* Make a vvp_vector4_t vector to receive the translated value
	   that we are going to poke. This will get populated
	   differently depending on the format. */
      wid = (rfp->msb.get_मूल्य() >= rfp->lsb.get_मूल्य())
	    ? (rfp->msb.get_मूल्य() - rfp->lsb.get_मूल्य() + 1)
	    : (rfp->lsb.get_मूल्य() - rfp->msb.get_मूल्य() + 1);

      vvp_vector4_t val = vec4_from_vpi_मूल्य(vp, wid);

      अगर (flags == vpiForceFlag) {
	    vvp_vector2_t mask (vvp_vector2_t::FILL1, wid);
	    rfp->गाँठ->force_vec4(val, mask);
      } अन्यथा अगर (net_flag && !dynamic_cast<vvp_island_port*>(rfp->गाँठ->fun)) {
	    rfp->गाँठ->send_vec4(val, vthread_get_wt_context());
      } अन्यथा {
	    vvp_send_vec4(dest, val, vthread_get_wt_context());
      }
      वापस ref;
}

vvp_vector4_t vec4_from_vpi_मूल्य(s_vpi_मूल्य*vp, अचिन्हित wid)
{
      vvp_vector4_t val (wid, BIT4_0);

      चयन (vp->format) {

	  हाल vpiIntVal: {
		दीर्घ vpi_val = vp->value.integer;
		क्रम (अचिन्हित idx = 0 ;  idx < wid ;  idx += 1) {
		      vvp_bit4_t bit = (vpi_val & 1) ? BIT4_1 : BIT4_0;
		      val.set_bit(idx, bit);
		      vpi_val >>= 1;
		}
		अवरोध;
	  }

	  हाल vpiVectorVal:
	    क्रम (अचिन्हित idx = 0 ;  idx < wid ;  idx += 1) {
		  अचिन्हित दीर्घ aval = vp->value.vector[idx/32].aval;
		  अचिन्हित दीर्घ bval = vp->value.vector[idx/32].bval;
		  aval >>= idx%32;
		  bval >>= idx%32;
		  पूर्णांक bitmask = (aval&1) | ((bval<<1)&2);
		  vvp_bit4_t bit = scalar_to_bit4(bitmask);
		  val.set_bit(idx, bit);
	    }
	    अवरोध;
	  हाल vpiBinStrVal:
	    vpip_bin_str_to_vec4(val, vp->value.str);
	    अवरोध;
	  हाल vpiOctStrVal:
	    vpip_oct_str_to_vec4(val, vp->value.str);
	    अवरोध;
	  हाल vpiDecStrVal:
	    vpip_dec_str_to_vec4(val, vp->value.str);
	    अवरोध;
	  हाल vpiHexStrVal:
	    vpip_hex_str_to_vec4(val, vp->value.str);
	    अवरोध;
	  हाल vpiScalarVal:
	    val.set_bit(0, scalar_to_bit4(vp->value.scalar));
	    अवरोध;
	  हाल vpiStringVal:
	    val = from_stringval(vp->value.str, wid);
	    अवरोध;
	  हाल vpiRealVal:
	    val = vvp_vector4_t(wid, vp->value.real);
	    अवरोध;

	  शेष:
	    ख_लिखो(मानक_त्रुटि, "vvp internal error: put_value: "
		    "value type %d not implemented here.\n",
		    (पूर्णांक)vp->format);
	    निश्चित(0);

      }

      वापस val;
}

पूर्णांक __vpiSignal::vpi_get(पूर्णांक code)
{ वापस signal_get(code, यह); }

int64_t __vpiSignal::vpi_get64(पूर्णांक code)
{ वापस signal_get64(code, यह); }

अक्षर* __vpiSignal::vpi_get_str(पूर्णांक code)
{ वापस signal_get_str(code, यह); }

व्योम __vpiSignal::vpi_get_मूल्य(p_vpi_मूल्य val)
{ signal_get_मूल्य(यह, val); }

vpiHandle __vpiSignal::vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णांक flags)
{ वापस signal_put_मूल्य(यह, val, flags); }

vpiHandle __vpiSignal::vpi_handle(पूर्णांक code)
{ वापस signal_get_handle(code, यह); }

vpiHandle __vpiSignal::vpi_iterate(पूर्णांक code)
{ वापस signal_iterate(code, यह); }

vpiHandle __vpiSignal::vpi_सूचकांक(पूर्णांक idx)
{ वापस signal_सूचकांक(idx, यह); }

काष्ठा signal_reg : खुला __vpiSignal {
      अंतरभूत signal_reg() { }
      पूर्णांक get_type_code(व्योम) स्थिर override { वापस vpiReg; }
};

काष्ठा signal_integer : खुला __vpiSignal {
      अंतरभूत signal_integer() { }
      पूर्णांक get_type_code(व्योम) स्थिर override { वापस vpiIntegerVar; }
};

काष्ठा signal_net : खुला __vpiSignal {
      अंतरभूत signal_net() { }
      पूर्णांक get_type_code(व्योम) स्थिर override { वापस vpiNet; }
};

काष्ठा signal_byte : खुला __vpiSignal {
      अंतरभूत signal_byte() { }
      पूर्णांक get_type_code(व्योम) स्थिर override { वापस vpiByteVar; }
};

काष्ठा signal_bitvar : खुला __vpiSignal {
      अंतरभूत signal_bitvar() { }
      पूर्णांक get_type_code(व्योम) स्थिर override { वापस vpiBitVar; }
};

काष्ठा signal_shortint : खुला __vpiSignal {
      अंतरभूत signal_shortint() { }
      पूर्णांक get_type_code(व्योम) स्थिर override { वापस vpiShortIntVar; }
};

काष्ठा signal_int : खुला __vpiSignal {
      अंतरभूत signal_int() { }
      पूर्णांक get_type_code(व्योम) स्थिर override { वापस vpiIntVar; }
};

काष्ठा signal_longint : खुला __vpiSignal {
      अंतरभूत signal_longint() { }
      पूर्णांक get_type_code(व्योम) स्थिर override { वापस vpiLongIntVar; }
};


/*
 * Construct a vpiIntegerVar object. Indicate the type using a flag
 * to minimize the code modifications. Icarus implements integers
 * as 'reg signed [31:0]'.
 */
vpiHandle vpip_make_int4(स्थिर अक्षर*name, पूर्णांक msb, पूर्णांक lsb, vvp_net_t*vec)
{
      __vpiSignal*obj = नया signal_integer;
      वापस fill_in_var4(obj, name, msb, lsb, true, vec);
}

/*
 * Construct the two-state SystemVerilog variables.
 */
vpiHandle vpip_make_int2(स्थिर अक्षर*name, पूर्णांक msb, पूर्णांक lsb, bool signed_flag,
                         vvp_net_t*vec)
{
      __vpiSignal*obj;

	// All unsigned 2-state variables are a vpiBitVar. All 2-state
	// variables with a non-zero lsb are also a vpiBitVar.
      अगर ((! signed_flag) || (lsb != 0) ) {
	    obj = नया signal_bitvar;
      } अन्यथा {
	      // These could also be bit declarations with matching
	      // information, but for now they get the apparent type.
	    चयन (msb) {
		हाल 7:
		  obj = नया signal_byte;
		  अवरोध;
		हाल 15:
		  obj = नया signal_shortint;
		  अवरोध;
		हाल 31:
		  obj = नया signal_int;
		  अवरोध;
		हाल 63:
		  obj = नया signal_longint;
		  अवरोध;
		शेष:
		    // Every other type of bit vector is a vpiBitVar with
		    // array dimensions.
		  obj = नया signal_bitvar;
		  अवरोध;
	    }
      }

      वापस fill_in_var4(obj, name, msb, lsb, signed_flag, vec);
}

/*
 * Construct a vpiReg/vpiLogicVar object. It's like a net, except for the type.
 */
vpiHandle vpip_make_var4(स्थिर अक्षर*name, पूर्णांक msb, पूर्णांक lsb,
			bool signed_flag, vvp_net_t*vec)
{
      __vpiSignal*obj = नया signal_reg;
      वापस fill_in_var4(obj, name, msb, lsb, signed_flag, vec);
}

#अगर_घोषित CHECK_WITH_VALGRIND
अटल काष्ठा vpiSignal_plug **signal_pool = 0;
अटल अचिन्हित signal_pool_गिनती = 0;
अटल अचिन्हित दीर्घ signal_गिनती = 0;
अटल अचिन्हित दीर्घ signal_dels = 0;
#पूर्ण_अगर

काष्ठा vpiSignal_plug {
      अचिन्हित अक्षर space[माप (काष्ठा __vpiSignal)];
#अगर_घोषित CHECK_WITH_VALGRIND
      काष्ठा vpiSignal_plug *pool;
#पूर्ण_अगर
};

व्योम* __vpiSignal::चालक नया(माप_प्रकार siz)
{
#अगर_घोषित CHECK_WITH_VALGRIND
      निश्चित(siz == माप(काष्ठा vpiSignal_plug) - माप(काष्ठा vpiSignal_plug *));
#अन्यथा
      निश्चित(siz == माप(काष्ठा vpiSignal_plug));
#पूर्ण_अगर
      अटल काष्ठा vpiSignal_plug*alloc_array = 0;
      अटल अचिन्हित alloc_सूचकांक = 0;
      स्थिर अचिन्हित alloc_गिनती = 512;

      अगर ((alloc_array == 0) || (alloc_सूचकांक == alloc_गिनती)) {
	    alloc_array = static_cast<काष्ठा vpiSignal_plug*>
	                  (सुस्मृति(alloc_गिनती, माप(काष्ठा vpiSignal_plug)));
	    alloc_सूचकांक = 0;
#अगर_घोषित CHECK_WITH_VALGRIND
	    VALGRIND_MAKE_MEM_NOACCESS(alloc_array, alloc_गिनती *
	                                            माप(काष्ठा vpiSignal_plug));
	    VALGRIND_CREATE_MEMPOOL(alloc_array, 0, 1);
	    signal_pool_गिनती += 1;
	    signal_pool = static_cast<vpiSignal_plug **>
	                  (पुनः_स्मृति(signal_pool,
	                   signal_pool_गिनती*माप(vpiSignal_plug **)));
	    signal_pool[signal_pool_गिनती-1] = alloc_array;
#पूर्ण_अगर
      }

      काष्ठा vpiSignal_plug*cur = alloc_array + alloc_सूचकांक;
#अगर_घोषित CHECK_WITH_VALGRIND
      VALGRIND_MEMPOOL_ALLOC(alloc_array, cur, माप(काष्ठा vpiSignal_plug));
      cur->pool = alloc_array;
      signal_गिनती += 1;
#पूर्ण_अगर
      alloc_सूचकांक += 1;
      वापस cur;
}

व्योम __vpiSignal::चालक मिटाओ(व्योम*)
{
      निश्चित(0);
}

#अगर_घोषित CHECK_WITH_VALGRIND
व्योम signal_delete(vpiHandle item)
{
      काष्ठा __vpiSignal *obj = static_cast<__vpiSignal *> (item);
      निश्चित(obj->गाँठ->fil);
      obj->गाँठ->fil->clear_all_callbacks();
      vvp_net_delete(obj->गाँठ);
      अगर (obj->bits) {
	    क्रम (अचिन्हित idx=0; idx<obj->width(); idx+=1) {
		  मिटाओ obj->bits[idx].सूचकांक;
	    }
	    obj->bits -= 1;
	    मिटाओ [] obj->bits;
      }
      signal_dels += 1;
      VALGRIND_MEMPOOL_FREE(reinterpret_cast<vpiSignal_plug *>(obj)->pool, obj);
}

व्योम signal_pool_delete()
{
      अगर (RUNNING_ON_VALGRIND && (signal_गिनती != signal_dels)) {
	    ख_साफ(शून्य);
	    VALGRIND_PRINTF("Error: vvp missed deleting %ld of %lu signal(s).",
	                    (दीर्घ) signal_गिनती - signal_dels, signal_गिनती);
      }

      क्रम (अचिन्हित idx = 0; idx < signal_pool_गिनती; idx += 1) {
	    VALGRIND_DESTROY_MEMPOOL(signal_pool[idx]);
	    मुक्त(signal_pool[idx]);
      }

      मुक्त(signal_pool);
      signal_pool = 0;
      signal_pool_गिनती = 0;
}
#पूर्ण_अगर

/*
 * Construct a vpiNet object. Give the object specified dimensions,
 * and point to the specified functor for the lsb.
 *
 * The name is the PLI name for the object. If it is an array it is
 * <name>[<index>].
 */
अटल vpiHandle fill_in_net4(काष्ठा __vpiSignal*obj, __vpiScope*scope,
			      स्थिर अक्षर*name, पूर्णांक msb, पूर्णांक lsb,
			      bool signed_flag, vvp_net_t*गाँठ)
{
      obj->id.name = name? vpip_name_string(name) : शून्य;
      obj->msb = __vpiDecConst(msb);
      obj->lsb = __vpiDecConst(lsb);
      obj->signed_flag = signed_flag? 1 : 0;
      obj->is_netarray = 0;
      obj->गाँठ = गाँठ;

	// Place this object within a scope. If this object is
	// attached to an array, then this value will be replaced with
	// the handle to the parent.
      obj->within.scope = scope;

      गिनती_vpi_nets += 1;

      वापस obj;
}

vpiHandle vpip_make_net4(__vpiScope*scope,
			 स्थिर अक्षर*name, पूर्णांक msb, पूर्णांक lsb,
			 bool signed_flag, vvp_net_t*गाँठ)
{
      काष्ठा __vpiSignal*obj = नया signal_net;
      वापस fill_in_net4(obj, scope, name, msb, lsb, signed_flag, गाँठ);
}

अटल पूर्णांक PV_get_base(काष्ठा __vpiPV*rfp)
{
	/* We return from the symbol base if it is defined. */
      अगर (rfp->sbase != 0) {
	    s_vpi_मूल्य val;
	      /* Check to see if the value is defined. */
	    val.format = vpiVectorVal;
	    vpi_get_मूल्य(rfp->sbase, &val);
	    पूर्णांक शब्दगण = (vpi_get(vpi_आकार, rfp->sbase)-1)/32 + 1;
	    क्रम(पूर्णांक idx = 0; idx < शब्दगण; idx += 1) {
		    /* Return INT_MIN to indicate an X base. */
		  अगर (val.value.vector[idx].bval != 0) वापस पूर्णांक_न्यून;
	    }
	      /* The value is defined so get and return it. */
	    val.format = vpiIntVal;
	    vpi_get_मूल्य(rfp->sbase, &val);
	    वापस val.value.integer;
      }

	/* If the width is zero then tbase is the constant. */
      वापस rfp->tbase;
}

अटल पूर्णांक PV_get(पूर्णांक code, vpiHandle ref)
{
      काष्ठा __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      निश्चित(rfp);

      पूर्णांक rval = 0;
      चयन (code) {
	हाल vpi_पंक्ति_No:
	    वापस 0;  // Not implemented for now!

	हाल vpiSigned:
	    वापस 0;  // A part/bit select is always unsigned!

	हाल vpi_आकार:
	    वापस rfp->width;

	  /* This is like the &A<> in array.cc. */
	हाल vpiConstantSelect:
	    वापस rfp->sbase == 0;

	हाल vpi_बायाँ_Range:
            rval += rfp->width - 1;
	    // fallthrough
	हाल vpi_दायाँ_Range: {
	    पूर्णांक बाँए = vpi_get(vpi_बायाँ_Range, rfp->parent);
	    पूर्णांक दाँए = vpi_get(vpi_दायाँ_Range, rfp->parent);
	    rval += PV_get_base(rfp);
	    वापस बाँए >= दाँए ? दाँए + rval : दाँए - rval;
	}

        हाल vpiAutomatic:
            वापस vpi_get(vpiAutomatic, rfp->parent);

#अगर defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
        हाल _vpiFromThr:
            वापस _vpi_at_PV;
#पूर्ण_अगर

	शेष:
	    ख_लिखो(मानक_त्रुटि, "PV_get: property %d is unknown\n", code);
      }

      वापस 0;
}

अटल अक्षर* PV_get_str(पूर्णांक code, vpiHandle ref)
{
      काष्ठा __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      निश्चित(rfp);

      चयन (code) {
	हाल vpiFile:  // Not implemented for now!
	    वापस simple_set_rbuf_str(file_names[0]);

	हाल vpiName:
	हाल vpiFullName: {
	    स्थिर अक्षर*nm = vpi_get_str(code, rfp->parent);
	    माप_प्रकार len = 256+म_माप(nm);
	    अक्षर *full = static_cast<अक्षर *>(दो_स्मृति(len));
	    snprintf(full, len, "%s[%d:%d]", nm,
	                                     (पूर्णांक)vpi_get(vpi_बायाँ_Range, ref),
	                                     (पूर्णांक)vpi_get(vpi_दायाँ_Range, ref));
	    full[len-1] = 0;
	    अक्षर *res = simple_set_rbuf_str(full);
	    मुक्त(full);
	    वापस res;
	}

	शेष:
	    ख_लिखो(मानक_त्रुटि, "PV_get_str: property %d is unknown.\n", code);
      }

      वापस 0;
}

अटल व्योम PV_get_मूल्य(vpiHandle ref, p_vpi_मूल्य vp)
{
      काष्ठा __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      निश्चित(rfp);

      vvp_signal_मूल्य*sig = dynamic_cast<vvp_signal_मूल्य*>(rfp->net->fil);
      निश्चित(sig);

      चयन (vp->format) {

	  हाल vpiIntVal:
	    format_vpiIntVal(sig, PV_get_base(rfp), rfp->width, 0, vp);
	    अवरोध;

	  हाल vpiBinStrVal:
	    format_vpiBinStrVal(sig, PV_get_base(rfp), rfp->width, vp);
	    अवरोध;

	  हाल vpiOctStrVal:
	    format_vpiOctStrVal(sig, PV_get_base(rfp), rfp->width, vp);
	    अवरोध;

	  हाल vpiHexStrVal:
	    format_vpiHexStrVal(sig, PV_get_base(rfp), rfp->width, vp);
	    अवरोध;

	  हाल vpiDecStrVal:
	    format_vpiDecStrVal(sig, PV_get_base(rfp), rfp->width, 0, vp);
	    अवरोध;

	  हाल vpiStringVal:
	    format_vpiStringVal(sig, PV_get_base(rfp), rfp->width, vp);
	    अवरोध;

	  हाल vpiScalarVal:
	    format_vpiScalarVal(sig, PV_get_base(rfp), vp);
	    अवरोध;

	  हाल vpiStrengthVal:
	    format_vpiStrengthVal(sig, PV_get_base(rfp), rfp->width, vp);
	    अवरोध;

	  हाल vpiVectorVal:
	    format_vpiVectorVal(sig, PV_get_base(rfp), rfp->width, vp);
	    अवरोध;

	  हाल vpiRealVal:
	    format_vpiRealVal(sig, PV_get_base(rfp), rfp->width, 0, vp);
	    अवरोध;

	  शेष:
	    ख_लिखो(मानक_त्रुटि, "vvp internal error: PV_get_value: "
		    "value type %d not implemented. Signal is %s.\n",
		    (पूर्णांक)vp->format, vpi_get_str(vpiFullName, rfp->parent));
	    निश्चित(0);
      }
}

अटल vpiHandle PV_put_मूल्य(vpiHandle ref, p_vpi_मूल्य vp, पूर्णांक flags)
{
      काष्ठा __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      निश्चित(rfp);
      vvp_signal_मूल्य*sig = dynamic_cast<vvp_signal_मूल्य*>(rfp->net->fil);
      निश्चित(sig);

      अचिन्हित sig_आकार = sig->मूल्य_आकार();
      अचिन्हित width = rfp->width;
      पूर्णांक base = PV_get_base(rfp);
      अगर (base >= (चिन्हित) sig_आकार) वापस 0;
      अगर (base + (चिन्हित) width < 0) वापस 0;

      vvp_vector4_t val;
      अगर (flags != vpiReleaseFlag) {
	    val = vec4_from_vpi_मूल्य(vp, width);
      }

	/*
	 * If the base is less than zero then trim off any unneeded
	 * lower bits.
	 */
      अगर (base < 0) {
	    width += base;
	    अगर (flags != vpiReleaseFlag) {
		  val = val.subvalue(-base, width);
	    }
	    base = 0;
      }

	/*
	 * If the value is wider than the signal then trim off any
	 * unneeded upper bits.
	 */
      अगर (base+width > sig_आकार) {
	    width = sig_आकार - base;
	    अगर (flags != vpiReleaseFlag) {
		  val = val.subvalue(0, width);
	    }
      }

      निश्चित(rfp->parent);
      bool net_flag = rfp->parent->get_type_code()==vpiNet;
      bool full_sig = base == 0 && width == sig_आकार;

      vvp_net_ptr_t dest(rfp->net, 0);

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value.  Note that
	   if this net is attached to an island, we need to rerun
	   the calculations immediately so we can return the
	   released value.*/
      अगर (flags == vpiReleaseFlag) {
	    निश्चित(rfp->net->fil);
	      // XXXX Can't really do this if this is a partial release?
	    rfp->net->fil->force_unlink();
	    अगर (full_sig) {
		  rfp->net->fil->release(dest, net_flag);
	    } अन्यथा {
		  rfp->net->fil->release_pv(dest, base, width, net_flag);
	    }
	    rfp->net->fun->force_flag(true);
	    PV_get_मूल्य(ref, vp);
	    वापस ref;
      }

      अगर (flags == vpiForceFlag) {
	    अगर (full_sig) {
		  vvp_vector2_t mask (vvp_vector2_t::FILL1, sig_आकार);
		  rfp->net->force_vec4(val, mask);
	    } अन्यथा {
		  vvp_vector2_t mask (vvp_vector2_t::FILL0, sig_आकार);
		  क्रम (अचिन्हित idx = 0 ; idx < width ; idx += 1)
		        mask.set_bit(base+idx, 1);

		  vvp_vector4_t tmp (sig_आकार, BIT4_Z);

		    // vvp_net_t::force_vec4 propagates all the bits of the
		    // forced vector value, regardless of the mask. This
		    // ensures the unforced bits retain their current value.
		  sig->vec4_मूल्य(tmp);

		  tmp.set_vec(base, val);
		  rfp->net->force_vec4(tmp, mask);
	    }
      } अन्यथा अगर (net_flag && !dynamic_cast<vvp_island_port*>(rfp->net->fun)) {
	    अगर (full_sig) {
		  rfp->net->send_vec4(val, vthread_get_wt_context());
	    } अन्यथा {
		  rfp->net->send_vec4_pv(val, base, sig_आकार,
		                         vthread_get_wt_context());
	    }
      } अन्यथा {
	    अगर (full_sig) {
		  vvp_send_vec4(dest, val, vthread_get_wt_context());
	    } अन्यथा {
		  vvp_send_vec4_pv(dest, val, base, sig_आकार,
	                           vthread_get_wt_context());
	    }
      }
      वापस 0;
}

अटल vpiHandle PV_get_handle(पूर्णांक code, vpiHandle ref)
{
      काष्ठा __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      निश्चित(rfp);

      चयन (code) {
	  हाल vpiParent:
	    वापस rfp->parent;

	  हाल vpiScope:
	    वापस vpi_handle(vpiScope, rfp->parent);

	  हाल vpiModule:
	    वापस vpi_handle(vpiModule, rfp->parent);
      }

      वापस 0;
}

अंतरभूत __vpiPV::__vpiPV()
{ }

पूर्णांक __vpiPV::get_type_code(व्योम) स्थिर
{ वापस vpiPartSelect; }

पूर्णांक __vpiPV::vpi_get(पूर्णांक code)
{ वापस PV_get(code, यह); }

अक्षर* __vpiPV::vpi_get_str(पूर्णांक code)
{ वापस PV_get_str(code, यह); }

व्योम __vpiPV::vpi_get_मूल्य(p_vpi_मूल्य val)
{ PV_get_मूल्य(यह, val); }

vpiHandle __vpiPV::vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णांक flags)
{ वापस PV_put_मूल्य(यह, val, flags); }

vpiHandle __vpiPV::vpi_handle(पूर्णांक code)
{ वापस PV_get_handle(code, यह); }

vpiHandle vpip_make_PV(अक्षर*var, पूर्णांक base, पूर्णांक width)
{
      काष्ठा __vpiPV*obj = नया __vpiPV;
      compile_vpi_lookup(&obj->parent, strdup(var));
      obj->sbase = 0;
      obj->tbase = base;
      obj->width = (अचिन्हित) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      वापस obj;
}

vpiHandle vpip_make_PV(अक्षर*var, अक्षर*प्रतीक, पूर्णांक width)
{
      काष्ठा __vpiPV*obj = नया __vpiPV;
      compile_vpi_lookup(&obj->parent, strdup(var));
      compile_vpi_lookup(&obj->sbase, प्रतीक);
      obj->tbase = 0;
      obj->width = (अचिन्हित) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      वापस obj;
}

vpiHandle vpip_make_PV(अक्षर*var, vpiHandle handle, पूर्णांक width)
{
      काष्ठा __vpiPV*obj = नया __vpiPV;
      compile_vpi_lookup(&obj->parent, strdup(var));
      obj->sbase = handle;
      obj->tbase = 0;
      obj->width = (अचिन्हित) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      वापस obj;
}

#अगर_घोषित CHECK_WITH_VALGRIND
व्योम PV_delete(vpiHandle item)
{
      काष्ठा __vpiPV *obj = dynamic_cast<__vpiPV*>(item);
      अगर (obj->sbase) {
	    चयन (obj->sbase->get_type_code()) {
		हाल vpiMemoryWord:
		  अगर (vpi_get(_vpiFromThr, obj->sbase) == _vpi_at_A) {
			A_delete(obj->sbase);
		  }
		  अवरोध;
		हाल vpiPartSelect:
		  निश्चित(vpi_get(_vpiFromThr, obj->sbase) == _vpi_at_PV);
		  PV_delete(obj->sbase);
		  अवरोध;
	    }
      }
      निश्चित(obj->net->fil);
      obj->net->fil->clear_all_callbacks();
      मिटाओ obj;
}
#पूर्ण_अगर
