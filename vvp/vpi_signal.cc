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
#यदि_परिभाषितम् CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#यदि_अन्तः
# include  <cmath>
# include  <iostream>
# include  <cstdio>
# include  <cstdlib>
# include  <climits>
# include  <cstring>
# include  <cassert>
#यदि_परिभाषितम् CHECK_WITH_VALGRIND
# include  <valgrind/memcheck.h>
#यदि_अन्तः
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
शून्य *need_परिणाम_buf(अचिह्नित cnt, vpi_rbuf_t type)
{
      स्थिर शून्य*परिणाम_buf[2] = {0, 0};
      स्थिर size_t परिणाम_buf_आकार[2] = {0, 0};

      यदि (type == RBUF_DEL) {
	    मुक्त(परिणाम_buf[RBUF_VAL]);
	    परिणाम_buf[RBUF_VAL] = 0;
	    परिणाम_buf_आकार[RBUF_VAL] = 0;

	    मुक्त(परिणाम_buf[RBUF_STR]);
	    परिणाम_buf[RBUF_STR] = 0;
	    परिणाम_buf_आकार[RBUF_STR] = 0;

	    प्रत्यागम 0;
      }

      cnt = (cnt + 0x0fff) & ~0x0fff;

      यदि (परिणाम_buf_आकार[type] == 0) {
	    परिणाम_buf[type] = स्मृतिं_देहि(cnt);
	    परिणाम_buf_आकार[type] = cnt;
      } अन्यथा यदि (परिणाम_buf_आकार[type] < cnt) {
	    परिणाम_buf[type] = realloc(परिणाम_buf[type], cnt);
	    परिणाम_buf_आकार[type] = cnt;
      }

      प्रत्यागम परिणाम_buf[type];
}

वर्ण *simple_समुच्चयः_rbuf_str(ध्रुव वर्ण *s1)
{
      वर्ण *res = static_cast<वर्ण *>(need_परिणाम_buf(वाक्यदैर्घ्यम्(s1)+1, RBUF_STR));
      यदि (res) वाक्यं_प्रतिलिख(res,s1);
      प्रत्यागम res;
}

वर्ण *generic_get_str(पूर्णाङ्क code, vpiHandle ref, ध्रुव वर्ण *name, ध्रुव वर्ण *सूचकांक)
{
      size_t len = वाक्यदैर्घ्यम्(name) + 1;  /* include space for null termination */
      वर्ण *bn = रिक्त;
      तार्किक is_bn_a_pkg = असत्य;
      यदि (code == vpi_पूर्णम्_Name) {
	    bn = strdup(vpi_get_str(code,ref));
	    size_t bn_len = वाक्यदैर्घ्यम्(bn);
	    is_bn_a_pkg = (bn_len >= 3) && (bn[bn_len-1] == ':') && (bn[bn_len-2] == ':');
	    len += bn_len;
	    यदि (! is_bn_a_pkg) len += 1; // include space for "." separator
      }
      यदि (सूचकांक != रिक्त) len += वाक्यदैर्घ्यम्(सूचकांक) + 2;  /* include space for brackets */

      वर्ण *res = static_cast<वर्ण *>(need_परिणाम_buf(len, RBUF_STR));
      यदि (!res) {
	    मुक्त(bn);
	    प्रत्यागम रिक्त;
      }
      *res=0;  /* start with nothing */

	/* if this works, I can make it more efficient later */
      यदि (bn != रिक्त) {
	    वाक्यं_योजय(res, bn);
	      // A package already has the "::" separator in the name
	    यदि (! is_bn_a_pkg) वाक्यं_योजय(res, ".");
	    मुक्त(bn);
      }
      वाक्यं_योजय(res, name);
      यदि (सूचकांक != रिक्त) {
	    वाक्यं_योजय(res, "[");
	    वाक्यं_योजय(res, सूचकांक);
	    वाक्यं_योजय(res, "]");
      }
      प्रत्यागम res;
}

स्थिर vpiHandle fill_अन्तर्_net4(संरचना __vpiSignal*obj, __vpiScope*scope,
                              ध्रुव वर्ण*name, पूर्णाङ्क msb, पूर्णाङ्क lsb,
                              तार्किक signed_flag, vvp_net_t*ग्रन्थिः);

स्थिर vpiHandle fill_अन्तर्_var4(संरचना __vpiSignal*obj,
                              ध्रुव वर्ण*name, पूर्णाङ्क msb, पूर्णाङ्क lsb,
                              तार्किक signed_flag, vvp_net_t*ग्रन्थिः)
{
	// Variable declarations are always resolved immediately,
	// so we can assume they belong in the current scope.
      प्रत्यागम fill_अन्तर्_net4(obj, vpip_peek_current_scope(),
			  name, msb, lsb, signed_flag, ग्रन्थिः);
}

/*
 * The standard formatting/conversion routines.
 * They work with full or partial signals.
 */

स्थिर शून्य format_vpiBinStrVal(ध्रुव vvp_signal_मानम्*sig, पूर्णाङ्क आधारः, अचिह्नित wid,
                                s_vpi_मानम्*vp)
{
      वर्ण *rbuf = static_cast<वर्ण *>(need_परिणाम_buf(wid+1, RBUF_VAL));
      दीर्घ अन्तः = आधारः + (चिह्नित)wid;
      दीर्घ विस्थापनम्_2 = अन्तः - 1;
      दीर्घ ssize = (चिह्नित)sig->मानम्_आकार();

      प्रति (दीर्घ idx = आधारः ;  idx < अन्तः ;  idx += 1) {
	    यदि (idx < 0 || idx >= ssize) {
                  rbuf[विस्थापनम्_2-idx] = 'x';
	    } अन्यथा {
                  rbuf[विस्थापनम्_2-idx] = vvp_bit4_गन्तव्यम्_ascii(sig->value(idx));
	    }
      }
      rbuf[wid] = 0;

      vp->value.str = rbuf;
}

स्थिर शून्य format_vpiOctStrVal(ध्रुव vvp_signal_मानम्*sig, पूर्णाङ्क आधारः, अचिह्नित wid,
                                s_vpi_मानम्*vp)
{
      अचिह्नित dwid = (wid + 2) / 3;
      वर्ण *rbuf = static_cast<वर्ण *>(need_परिणाम_buf(dwid+1, RBUF_VAL));
      दीर्घ अन्तः = आधारः + (चिह्नित)wid;
      दीर्घ ssize = (चिह्नित)sig->मानम्_आकार();
      अचिह्नित val = 0;

      rbuf[dwid] = 0;
      प्रति (दीर्घ idx = आधारः ;  idx < अन्तः ;  idx += 1) {
	    अचिह्नित bit = 0;
	    यदि (idx < 0 || idx >= ssize) {
                  bit = 2; // BIT4_X
	    } अन्यथा {
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
	    val |= bit << 2*((idx-आधारः) % 3);

	    यदि ((idx-आधारः) % 3 == 2) {
		dwid -= 1;
		rbuf[dwid] = oct_digits[val];
		val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      विकल्प (wid % 3) {
	  पक्ष 1:
	    यदि (val == 2) val = 42;
	    अन्यथा यदि (val == 3) val = 63;
	    भङ्ग;
	  पक्ष 2:
	    यदि (val == 10) val = 42;
	    अन्यथा यदि (val == 15) val = 63;
	    भङ्ग;
      }

      यदि (dwid > 0) rbuf[0] = oct_digits[val];

      vp->value.str = rbuf;
}

स्थिर शून्य format_vpiHexStrVal(ध्रुव vvp_signal_मानम्*sig, पूर्णाङ्क आधारः, अचिह्नित wid,
                                s_vpi_मानम्*vp)
{
      अचिह्नित dwid = (wid + 3) / 4;
      वर्ण *rbuf = static_cast<वर्ण *>(need_परिणाम_buf(dwid+1, RBUF_VAL));
      दीर्घ अन्तः = आधारः + (चिह्नित)wid;
      दीर्घ ssize = (चिह्नित)sig->मानम्_आकार();
      अचिह्नित val = 0;

      rbuf[dwid] = 0;
      प्रति (दीर्घ idx = आधारः ;  idx < अन्तः ;  idx += 1) {
	    अचिह्नित bit = 0;
	    यदि (idx < 0 || idx >= ssize) {
                  bit = 2; // BIT4_X
	    } अन्यथा {
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
	    val |= bit << 2*((idx-आधारः) % 4);

	    यदि ((idx-आधारः) % 4 == 3) {
		dwid -= 1;
		rbuf[dwid] = hex_digits[val];
		val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      विकल्प (wid % 4) {
	  पक्ष 1:
	    यदि (val == 2) val = 170;
	    अन्यथा यदि (val == 3) val = 255;
	    भङ्ग;
	  पक्ष 2:
	    यदि (val == 10) val = 170;
	    अन्यथा यदि (val == 15) val = 255;
	    भङ्ग;
	  पक्ष 3:
	    यदि (val == 42) val = 170;
	    अन्यथा यदि (val == 63) val = 255;
	    भङ्ग;
      }

      यदि (dwid > 0) rbuf[0] = hex_digits[val];

      vp->value.str = rbuf;
}

स्थिर शून्य format_vpiDecStrVal(ध्रुव vvp_signal_मानम्*sig, पूर्णाङ्क आधारः, अचिह्नित wid,
                                पूर्णाङ्क signed_flag, s_vpi_मानम्*vp)
{
      अचिह्नित hwid = (sig->मानम्_आकार()+2) / 3 + 1;
      वर्ण *rbuf = static_cast<वर्ण *>(need_परिणाम_buf(hwid, RBUF_VAL));
      दीर्घ ssize = (चिह्नित)sig->मानम्_आकार();
      दीर्घ अन्तः = आधारः + (चिह्नित)wid;

	/* Do we have an end outside of the real signal vector. */
      यदि (आधारः < 0 || अन्तः > ssize) {
	    तार्किक all_x = सत्य;
	    यदि (अन्तः > ssize) अन्तः = ssize;
	    यदि (आधारः < 0) आधारः = 0;
	    प्रति (दीर्घ idx = आधारः ;  idx < अन्तः ;  idx += 1) {
		  यदि (sig->value(idx) != BIT4_X) {
			all_x = असत्य;
			भङ्ग;
		  }
	    }

	    यदि (all_x) {
		  rbuf[0] = 'x';
	    } अन्यथा {
		  rbuf[0] = 'X';
	    }
	    rbuf[1] = 0;

	    vp->value.str = rbuf;
	    प्रत्यागम;
      }

      vvp_vector4_t vec4;
      यदि (आधारः == 0 && अन्तः == ssize) {
	    sig->vec4_मानम्(vec4);
      } अन्यथा {
	    vvp_vector4_t tmp;
	    sig->vec4_मानम्(tmp);
	    vec4 = tmp.subvalue(आधारः, wid);
      }

      vpip_vec4_गन्तव्यम्_dec_str(vec4, rbuf, hwid, signed_flag);

      vp->value.str = rbuf;
}

स्थिर शून्य format_vpiIntVal(ध्रुव vvp_signal_मानम्*sig, पूर्णाङ्क आधारः, अचिह्नित wid,
                             पूर्णाङ्क signed_flag, s_vpi_मानम्*vp)
{
      vvp_vector4_t tmp;
      sig->vec4_मानम्(tmp);
      vvp_vector4_t sub = tmp.subvalue(आधारः, wid);

	// Normally, we'd be OK with just using long in the call to
	// vector4_to_value, but some compilers seem to take long as
	// distinct from int32_t AND int64_t. Since the condition is
	// constant, the compiler should eliminate the dead code.
      यदि (परिमाण(vp->value.integer) == परिमाण(int32_t)) {
	    int32_t val = 0;
	    vector4_गन्तव्यम्_मानम्(sub, val, signed_flag, असत्य);
	    vp->value.integer = val;
      } अन्यथा {
	    प्रतिज्ञा(परिमाण(vp->value.integer) == परिमाण(int64_t));
	    int64_t val = 0;
	    vector4_गन्तव्यम्_मानम्(sub, val, signed_flag, असत्य);
	    vp->value.integer = val;
      }
}

स्थिर शून्य format_vpiRealVal(ध्रुव vvp_signal_मानम्*sig, पूर्णाङ्क आधारः, अचिह्नित wid,
                              पूर्णाङ्क signed_flag, s_vpi_मानम्*vp)
{
      vvp_vector4_t vec4(wid);
      दीर्घ ssize = (चिह्नित)sig->मानम्_आकार();
      दीर्घ अन्तः = आधारः + (चिह्नित)wid;
      यदि (अन्तः > ssize) अन्तः = ssize;

      प्रति (दीर्घ idx = (आधारः < 0) ? 0 : आधारः ;  idx < अन्तः ;  idx += 1) {
	    vec4.समुच्चयः_bit(idx-आधारः, sig->value(idx));
      }

      vp->value.real = 0.0;
      vector4_गन्तव्यम्_मानम्(vec4, vp->value.real, signed_flag);
}

स्थिर शून्य format_vpiStringVal(ध्रुव vvp_signal_मानम्*sig, पूर्णाङ्क आधारः, अचिह्नित wid,
                                s_vpi_मानम्*vp)
{
      /* The result will use a character for each 8 bits of the
	 vector. Add one extra character for the highest bits that
	 don't form an 8 bit group. */
      वर्ण *rbuf = static_cast<वर्ण *>(need_परिणाम_buf(wid/8 + ((wid&7)!=0) + 1, RBUF_VAL));
      वर्ण *cp = rbuf;

      वर्ण tmp = 0;
      प्रति (दीर्घ idx = आधारः+(चिह्नित)wid-1; idx >= आधारः; idx -= 1) {
	    tmp <<= 1;

	    यदि (idx >=0 && idx < (चिह्नित)sig->मानम्_आकार() &&
	        sig->value(idx) == BIT4_1) {
		   tmp |= 1;
	    }

	    यदि (((idx-आधारः)&7)==0){
		  /* Skip leading nulls. */
		  यदि (tmp == 0 && cp == rbuf)
			अनुवर्त;

		  /* Nulls in the middle get turned into spaces. */
		  *cp++ = tmp ? tmp : ' ';
		  tmp = 0;
	    }
      }
      *cp++ = 0;

      vp->value.str = rbuf;
}

स्थिर शून्य format_vpiScalarVal(ध्रुव vvp_signal_मानम्*sig, पूर्णाङ्क आधारः,
                                s_vpi_मानम्*vp)
{
      यदि (आधारः >= 0 && आधारः < (चिह्नित)sig->मानम्_आकार()) {
	    विकल्प (sig->value(आधारः)) {
		पक्ष BIT4_0:
		  vp->value.scalar = vpi0;
		  भङ्ग;
		पक्ष BIT4_1:
		  vp->value.scalar = vpi1;
		  भङ्ग;
		पक्ष BIT4_X: {
		  vvp_scalar_t strn = sig->scalar_मानम्(आधारः);
		  यदि (strn.strength0() == 1) vp->value.scalar = vpiH;
		  अन्यथा यदि (strn.strength1() == 1) vp->value.scalar = vpiL;
		  अन्यथा vp->value.scalar = vpiX;
		  भङ्ग;
		}
		पक्ष BIT4_Z:
		  vp->value.scalar = vpiZ;
		  भङ्ग;
	    }
      } अन्यथा {
	    vp->value.scalar = vpiX;
      }
}

स्थिर शून्य format_vpiStrengthVal(ध्रुव vvp_signal_मानम्*sig, पूर्णाङ्क आधारः,
                                  अचिह्नित wid, s_vpi_मानम्*vp)
{
      दीर्घ अन्तः = आधारः + (चिह्नित)wid;
      s_vpi_strengthval*op;

      op = static_cast<s_vpi_strengthval*>
           (need_परिणाम_buf(wid * परिमाण(s_vpi_strengthval), RBUF_VAL));

      प्रति (दीर्घ idx = आधारः ;  idx < अन्तः ;  idx += 1) {
	    यदि (idx >=0 && idx < (चिह्नित)sig->मानम्_आकार()) {
		  vvp_scalar_t val = sig->scalar_मानम्(idx);

		  /* vvp_scalar_t strengths are 0-7, but the vpi strength
		     is bit0-bit7. This gets the vpi form of the strengths
		     from the vvp_scalar_t strengths. */
		  अचिह्नित s0 = 1 << val.strength0();
		  अचिह्नित s1 = 1 << val.strength1();

		  विकल्प (val.value()) {
		      पक्ष BIT4_0:
			op[idx-आधारः].logic = vpi0;
			op[idx-आधारः].s0 = s0|s1;
			op[idx-आधारः].s1 = 0;
			भङ्ग;

		      पक्ष BIT4_1:
			op[idx-आधारः].logic = vpi1;
			op[idx-आधारः].s0 = 0;
			op[idx-आधारः].s1 = s0|s1;
			भङ्ग;

		      पक्ष BIT4_X:
			op[idx-आधारः].logic = vpiX;
			op[idx-आधारः].s0 = s0;
			op[idx-आधारः].s1 = s1;
			भङ्ग;

		      पक्ष BIT4_Z:
			op[idx-आधारः].logic = vpiZ;
			op[idx-आधारः].s0 = vpiHiZ;
			op[idx-आधारः].s1 = vpiHiZ;
			भङ्ग;
		  }
	    } अन्यथा {
		  op[idx-आधारः].logic = vpiX;
		  op[idx-आधारः].s0 = vpiStrongDrive;
		  op[idx-आधारः].s1 = vpiStrongDrive;
	    }
      }

      vp->value.strength = op;
}

स्थिर शून्य format_vpi_सदिशम्_Val(ध्रुव vvp_signal_मानम्*sig, पूर्णाङ्क आधारः, अचिह्नित wid,
                                s_vpi_मानम्*vp)
{
      दीर्घ अन्तः = आधारः + (चिह्नित)wid;
      अचिह्नित पूर्णाङ्क obit = 0;
      अचिह्नित hwid = (wid + 31)/32;

      s_vpi_vecval *op = static_cast<p_vpi_vecval>
                         (need_परिणाम_buf(hwid * परिमाण(s_vpi_vecval), RBUF_VAL));
      vp->value.vector = op;

      op->aval = op->bval = 0;
      प्रति (दीर्घ idx = आधारः ;  idx < अन्तः ;  idx += 1) {
	    यदि (आधारः >= 0 && आधारः < (चिह्नित)sig->मानम्_आकार()) {
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
	    } अन्यथा {  /* BIT4_X */
		  op->aval |= (1 << obit);
		  op->bval |= (1 << obit);
	    }

	    obit++;
	    यदि (!(obit % 32)) {
		  op += 1;
		  यदि ((op - vp->value.vector) < (ptrdiff_t)hwid)
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
	    प्रत्यागम 0;  // Not implemented for now!

	  पक्ष vpiSigned:
	    प्रत्यागम rfp->signed_flag != 0;

	  पक्ष vpiArray:
	    प्रत्यागम rfp->is_netarray != 0;

	  पक्ष vpi_सूचकांक_2: // This only works while we have a single index.
	    यदि (rfp->is_netarray) {
		  s_vpi_मानम् vp;
		  vp.format = vpiIntVal;
		  vpi_get_मानम्(rfp->id.सूचकांक, &vp);
		  प्रत्यागम vp.value.integer;
	    } अन्यथा {
		  प्रत्यागम vpiUndefined;
	    }

	  पक्ष vpi_आकार:
	    प्रत्यागम rfp->width();

	  पक्ष vpiNetType:
	    यदि (ref->get_type_संहिता()==vpiNet)
		  प्रत्यागम vpiWire;
	    अन्यथा
		  प्रत्यागम vpiUndefined;

	  पक्ष vpi_वामः_Range:
            प्रत्यागम rfp->msb.get_मानम्();

	  पक्ष vpi_दक्षिणः_Range:
            प्रत्यागम rfp->lsb.get_मानम्();

	  पक्ष vpiScalar:
	    प्रत्यागम (rfp->msb.get_मानम्() == 0 && rfp->lsb.get_मानम्() == 0);
	  पक्ष vpi_सदिशम्:
	    प्रत्यागम (rfp->msb.get_मानम्() != rfp->lsb.get_मानम्());

          पक्ष vpiAutomatic:
            प्रत्यागम vpip_scope(rfp)->is_automatic() ? 1  : 0;

#यदि_परिभाषितम् BR916_STOPGAP_FIX
          पक्ष _vpiFromThr:
            प्रत्यागम _vpiNoThr;
#यदि_अन्तः

	  स्वभाव:
	    fprintf(stderr, "VPI error: unknown signal_get property %d.\n",
	            code);
	    प्रत्यागम vpiUndefined;
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
	    यदि (rfp->msb.get_मानम्() == rfp->lsb.get_मानम्())
		  प्रत्यागम reinterpret_cast<int64_t>(rfp->ग्रन्थिः);
	    प्रत्यागम 0;

	  स्वभाव:
	    fprintf(stderr, "VPI error: unknown signal_get64 property %d.\n",
	            code);
	    प्रत्यागम vpiUndefined;
      }
}

स्थिर वर्ण* signal_get_str(पूर्णाङ्क code, vpiHandle ref)
{
      संरचना __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      प्रतिज्ञा(rfp);

      यदि (code == vpiFile) {  // Not implemented for now!
	    प्रत्यागम simple_समुच्चयः_rbuf_str(file_नामानि[0]);
      }

      यदि ((code != vpiName) && (code != vpi_पूर्णम्_Name)) प्रत्यागम रिक्त;

      वर्ण *nm;
      ध्रुव वर्ण *ixs;
      यदि (rfp->is_netarray) {
	    nm = strdup(vpi_get_str(vpiName, rfp->within.parent));
	    s_vpi_मानम् vp;
	    vp.format = vpiDecStrVal;
	    vpi_get_मानम्(rfp->id.सूचकांक, &vp);
	    ixs = vp.value.str;  /* do I need to strdup() this? */
      } अन्यथा {
	    nm = strdup(rfp->id.name);
	    ixs = रिक्त;
      }

	/* The scope information is added here for vpiFullName. */
      वर्ण *rbuf = generic_get_str(code, vpip_scope(rfp), nm, ixs);
      मुक्त(nm);
      प्रत्यागम rbuf;
}

स्थिर vpiHandle signal_get_handle(पूर्णाङ्क code, vpiHandle ref)
{
      संरचना __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      प्रतिज्ञा(rfp);

      विकल्प (code) {

	  पक्ष vpiParent:
	    प्रत्यागम rfp->is_netarray? rfp->within.parent : रिक्त;

	  पक्ष vpi_सूचकांक_2:
	    प्रत्यागम rfp->is_netarray? rfp->id.सूचकांक : रिक्त;

	  पक्ष vpi_वामः_Range:
	    प्रत्यागम &rfp->msb;
	  पक्ष vpi_दक्षिणः_Range:
	    प्रत्यागम &rfp->lsb;

	  पक्ष vpiScope:
	    प्रत्यागम vpip_scope(rfp);

	  पक्ष vpiModule:
	    प्रत्यागम vpip_module(vpip_scope(rfp));
      }

      प्रत्यागम 0;
}

स्थिर vpiHandle signal_iterate(पूर्णाङ्क code, vpiHandle ref)
{
      संरचना __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      प्रतिज्ञा(rfp);

      यदि (code == vpi_सूचकांक_2) {
	    प्रत्यागम rfp->is_netarray ? rfp->id.सूचकांक->vpi_iterate(code) : रिक्त;
      }

      प्रत्यागम 0;
}

शून्य __vpiSignal::make_द्वयङ्काः()
{
      प्रतिज्ञा(द्वयङ्काः == रिक्त);
      द्वयङ्काः = नूतन संरचना __vpiBit[width() + 1];

	// Make word[-1] point to the parent.
      द्वयङ्काः->parent = एतत्;
	// Now point to word[0].
      द्वयङ्काः += 1;

	// Update each bit to point to the base
      संरचना __vpiBit*आधारः = द्वयङ्काः;
      प्रति (अचिह्नित idx = 0; idx < width(); idx += 1) {
	    आधारः[idx].bit0 = आधारः;
	    पूर्णाङ्क real_idx;
	    यदि (msb.get_मानम्() >= lsb.get_मानम्()) {
		  real_idx = idx + lsb.get_मानम्();
	    } अन्यथा {
		  real_idx = lsb.get_मानम्() - idx;
	    }
	    आधारः[idx].सूचकांक = नूतन __vpiDecConst(real_idx);
      }
}

vpiHandle __vpiSignal::get_सूचकांक(पूर्णाङ्क idx)
{
	/* Check to see if the index is in range. */
      यदि (msb.get_मानम्() >= lsb.get_मानम्()) {
	    यदि ((idx > msb.get_मानम्()) || (idx < lsb.get_मानम्())) प्रत्यागम 0;
      } अन्यथा {
	    यदि ((idx < msb.get_मानम्()) || (idx > lsb.get_मानम्())) प्रत्यागम 0;
      }

	/* Normalize the index */
      अचिह्नित norm_idx;
      यदि (msb.get_मानम्() >= lsb.get_मानम्()) {
	    norm_idx = idx - lsb.get_मानम्();
      } अन्यथा {
	    norm_idx = lsb.get_मानम्() - idx;
      }

      यदि (द्वयङ्काः == रिक्त) make_द्वयङ्काः();

      प्रत्यागम &(द्वयङ्काः[norm_idx].as_bit);
}

शून्य __vpiSignal::get_bit_मानम्(ध्रुव संरचना __vpiBit*bit, p_vpi_मानम् vp)
{
      अचिह्नित सूचकांक = bit->get_norm_सूचकांक();

      vvp_signal_मानम्*vsig = dynamic_cast<vvp_signal_मानम्*>(ग्रन्थिः->fil);
      प्रतिज्ञा(vsig);

      यदि (vp->format == vpiObjTypeVal) {
	    vp->format = vpiIntVal;
      }

      विकल्प (vp->format) {
	पक्ष vpiBinStrVal:
            format_vpiBinStrVal(vsig, सूचकांक, 1, vp);
	    भङ्ग;

	पक्ष vpiOctStrVal:
            format_vpiOctStrVal(vsig, सूचकांक, 1, vp);
	    भङ्ग;

	पक्ष vpiDecStrVal:
            format_vpiDecStrVal(vsig, सूचकांक, 1, असत्य, vp);
	    भङ्ग;

	पक्ष vpiHexStrVal:
            format_vpiHexStrVal(vsig, सूचकांक, 1, vp);
	    भङ्ग;

	पक्ष vpiStringVal:
            format_vpiStringVal(vsig, सूचकांक, 1, vp);
	    भङ्ग;

	पक्ष vpiIntVal:
            format_vpiIntVal(vsig, सूचकांक, 1, असत्य, vp);
	    भङ्ग;

	पक्ष vpiRealVal:
            format_vpiRealVal(vsig, सूचकांक, 1, असत्य, vp);
	    भङ्ग;

	पक्ष vpiScalarVal:
            format_vpiScalarVal(vsig, सूचकांक, vp);
	    भङ्ग;

        पक्ष vpiStrengthVal:
            format_vpiStrengthVal(vsig, सूचकांक, 1, vp);
            भङ्ग;

	पक्ष vpi_सदिशम्_Val:
            format_vpi_सदिशम्_Val(vsig, सूचकांक, 1, vp);
	    भङ्ग;

	स्वभाव:
	    fprintf(stderr, "internal error: get_value() "
		    "type %d is not implemented for bit "
		    "select %s[%d] in scope %s.\n",
		    (पूर्णाङ्क)vp->format, vpi_get_str(vpiName),
		    bit->get_सूचकांक(),
	            vpip_scope(एतत्)->scope_name());
	    प्रतिज्ञा(0);
      };
}

vpiHandle __vpiSignal::put_bit_मानम्(संरचना __vpiBit*bit, p_vpi_मानम् vp, पूर्णाङ्क flags)
{
      अचिह्नित सूचकांक = bit->get_norm_सूचकांक();
      vvp_net_ptr_t dest(ग्रन्थिः, 0);
      vvp_vector4_t val = vec4_from_vpi_मानम्(vp, 1);

      यदि ((flags == vpiForceFlag) || (flags == vpiReleaseFlag)) {
	    fprintf(stderr, "Sorry: vpi_put_value() for %s does not "
	                    "currently support force/release.\n",
	                    bit->as_bit.vpi_get_str(vpi_पूर्णम्_Name));
	    प्रत्यागम रिक्त;
      }

      यदि ((get_type_संहिता() == vpiNet) &&
          !dynamic_cast<vvp_island_पत्तनम्*>(ग्रन्थिः->fun)) {
	    ग्रन्थिः->send_vec4_pv(val, सूचकांक, width(),
	                            vthread_get_wt_context());
      } अन्यथा {
	    vvp_send_vec4_pv(dest, val, सूचकांक, width(),
	                     vthread_get_wt_context());
      }

	// This is not a scheduled event so there is no event to return
      प्रत्यागम रिक्त;
}

स्थिर vpiHandle signal_सूचकांक(पूर्णाङ्क idx, vpiHandle ref)
{
      संरचना __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      प्रतिज्ञा(rfp);

	/* We can only get the bit for a net or reg. */
      PLI_INT32 type = vpi_get(vpiType, ref);
      यदि ((type != vpiNet) && (type != vpiReg)) प्रत्यागम 0;

      प्रत्यागम rfp->get_सूचकांक(idx);
}

अचिह्नित __vpiSignal::width(शून्य) ध्रुव
{
      अचिह्नित wid = (msb.get_मानम्() >= lsb.get_मानम्())
	    ? (msb.get_मानम्() - lsb.get_मानम्() + 1)
	    : (lsb.get_मानम्() - msb.get_मानम्() + 1);

      प्रत्यागम wid;
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

      अचिह्नित wid = rfp->width();

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

	  पक्ष vpi_सदिशम्_Val:
	    format_vpi_सदिशम्_Val(vsig, 0, wid, vp);
	    भङ्ग;

	  पक्ष vpiRealVal:
	    format_vpiRealVal(vsig, 0, wid, rfp->signed_flag, vp);
	    भङ्ग;

	  पक्ष vpiObjTypeVal:
	    यदि (wid == 1) {
		  vp->format = vpiScalarVal;
		  format_vpiScalarVal(vsig, 0, vp);
	    } अन्यथा {
		  vp->format = vpi_सदिशम्_Val;
		  format_vpi_सदिशम्_Val(vsig, 0, wid, vp);
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

स्थिर vvp_vector4_t from_stringval(ध्रुव वर्ण*str, अचिह्नित wid)
{
      अचिह्नित idx;
      ध्रुव वर्ण*cp;

      cp = str + वाक्यदैर्घ्यम्(str);
      idx = 0;

      vvp_vector4_t val(wid, BIT4_0);

      यावत् ((idx < wid) && (cp > str)) {
	    अचिह्नित byte = *--cp;
	    पूर्णाङ्क bit;

	    प्रति (bit = 0 ;  (bit < 8) && (idx < wid) ;  bit += 1) {
		  यदि (byte & 1)
			val.समुच्चयः_bit(idx, BIT4_1);

		  byte >>= 1;
		  idx += 1;
	    }
      }

      प्रत्यागम val;
}

स्थिर vpiHandle signal_put_मानम्(vpiHandle ref, s_vpi_मानम्*vp, पूर्णाङ्क flags)
{
      अचिह्नित wid;
      संरचना __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      प्रतिज्ञा(rfp);
      vvp_net_ptr_t dest(rfp->ग्रन्थिः, 0);

      तार्किक net_flag = ref->get_type_संहिता()==vpiNet;

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value. Note that
	   if this net is attached to an island, we need to rerun
	   the calculations immediately so we can return the
	   released value. */
      यदि (flags == vpiReleaseFlag) {
	    प्रतिज्ञा(rfp->ग्रन्थिः->fil);
	    rfp->ग्रन्थिः->fil->force_unlink();
	    rfp->ग्रन्थिः->fil->release(dest, net_flag);
	    rfp->ग्रन्थिः->fun->force_flag(सत्य);
	    signal_get_मानम्(ref, vp);
	    प्रत्यागम ref;
      }

	/* Make a vvp_vector4_t vector to receive the translated value
	   that we are going to poke. This will get populated
	   differently depending on the format. */
      wid = (rfp->msb.get_मानम्() >= rfp->lsb.get_मानम्())
	    ? (rfp->msb.get_मानम्() - rfp->lsb.get_मानम्() + 1)
	    : (rfp->lsb.get_मानम्() - rfp->msb.get_मानम्() + 1);

      vvp_vector4_t val = vec4_from_vpi_मानम्(vp, wid);

      यदि (flags == vpiForceFlag) {
	    vvp_vector2_t आवरणम् (vvp_vector2_t::FILL1, wid);
	    rfp->ग्रन्थिः->force_vec4(val, आवरणम्);
      } अन्यथा यदि (net_flag && !dynamic_cast<vvp_island_पत्तनम्*>(rfp->ग्रन्थिः->fun)) {
	    rfp->ग्रन्थिः->send_vec4(val, vthread_get_wt_context());
      } अन्यथा {
	    vvp_send_vec4(dest, val, vthread_get_wt_context());
      }
      प्रत्यागम ref;
}

vvp_vector4_t vec4_from_vpi_मानम्(s_vpi_मानम्*vp, अचिह्नित wid)
{
      vvp_vector4_t val (wid, BIT4_0);

      विकल्प (vp->format) {

	  पक्ष vpiIntVal: {
		दीर्घ vpi_val = vp->value.integer;
		प्रति (अचिह्नित idx = 0 ;  idx < wid ;  idx += 1) {
		      vvp_bit4_t bit = (vpi_val & 1) ? BIT4_1 : BIT4_0;
		      val.समुच्चयः_bit(idx, bit);
		      vpi_val >>= 1;
		}
		भङ्ग;
	  }

	  पक्ष vpi_सदिशम्_Val:
	    प्रति (अचिह्नित idx = 0 ;  idx < wid ;  idx += 1) {
		  अचिह्नित दीर्घ aval = vp->value.vector[idx/32].aval;
		  अचिह्नित दीर्घ bval = vp->value.vector[idx/32].bval;
		  aval >>= idx%32;
		  bval >>= idx%32;
		  पूर्णाङ्क bitmask = (aval&1) | ((bval<<1)&2);
		  vvp_bit4_t bit = scalar_गन्तव्यम्_bit4(bitmask);
		  val.समुच्चयः_bit(idx, bit);
	    }
	    भङ्ग;
	  पक्ष vpiBinStrVal:
	    vpip_bin_str_गन्तव्यम्_vec4(val, vp->value.str);
	    भङ्ग;
	  पक्ष vpiOctStrVal:
	    vpip_oct_str_गन्तव्यम्_vec4(val, vp->value.str);
	    भङ्ग;
	  पक्ष vpiDecStrVal:
	    vpip_dec_str_गन्तव्यम्_vec4(val, vp->value.str);
	    भङ्ग;
	  पक्ष vpiHexStrVal:
	    vpip_hex_str_गन्तव्यम्_vec4(val, vp->value.str);
	    भङ्ग;
	  पक्ष vpiScalarVal:
	    val.समुच्चयः_bit(0, scalar_गन्तव्यम्_bit4(vp->value.scalar));
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

      प्रत्यागम val;
}

पूर्णाङ्क __vpiSignal::vpi_get(पूर्णाङ्क code)
{ प्रत्यागम signal_get(code, एतत्); }

int64_t __vpiSignal::vpi_get64(पूर्णाङ्क code)
{ प्रत्यागम signal_get64(code, एतत्); }

वर्ण* __vpiSignal::vpi_get_str(पूर्णाङ्क code)
{ प्रत्यागम signal_get_str(code, एतत्); }

शून्य __vpiSignal::vpi_get_मानम्(p_vpi_मानम् val)
{ signal_get_मानम्(एतत्, val); }

vpiHandle __vpiSignal::vpi_put_मानम्(p_vpi_मानम् val, पूर्णाङ्क flags)
{ प्रत्यागम signal_put_मानम्(एतत्, val, flags); }

vpiHandle __vpiSignal::vpi_handle(पूर्णाङ्क code)
{ प्रत्यागम signal_get_handle(code, एतत्); }

vpiHandle __vpiSignal::vpi_iterate(पूर्णाङ्क code)
{ प्रत्यागम signal_iterate(code, एतत्); }

vpiHandle __vpiSignal::vpi_सूचकांक(पूर्णाङ्क idx)
{ प्रत्यागम signal_सूचकांक(idx, एतत्); }

संरचना signal_reg : सार्वजनिक __vpiSignal {
      अन्तःस्थ signal_reg() { }
      पूर्णाङ्क get_type_संहिता(शून्य) ध्रुव override { प्रत्यागम vpiReg; }
};

संरचना signal_integer : सार्वजनिक __vpiSignal {
      अन्तःस्थ signal_integer() { }
      पूर्णाङ्क get_type_संहिता(शून्य) ध्रुव override { प्रत्यागम vpiIntegerVar; }
};

संरचना signal_net : सार्वजनिक __vpiSignal {
      अन्तःस्थ signal_net() { }
      पूर्णाङ्क get_type_संहिता(शून्य) ध्रुव override { प्रत्यागम vpiNet; }
};

संरचना signal_byte : सार्वजनिक __vpiSignal {
      अन्तःस्थ signal_byte() { }
      पूर्णाङ्क get_type_संहिता(शून्य) ध्रुव override { प्रत्यागम vpiByteVar; }
};

संरचना signal_bitvar : सार्वजनिक __vpiSignal {
      अन्तःस्थ signal_bitvar() { }
      पूर्णाङ्क get_type_संहिता(शून्य) ध्रुव override { प्रत्यागम vpiBitVar; }
};

संरचना signal_shortint : सार्वजनिक __vpiSignal {
      अन्तःस्थ signal_shortint() { }
      पूर्णाङ्क get_type_संहिता(शून्य) ध्रुव override { प्रत्यागम vpiShortIntVar; }
};

संरचना signal_int : सार्वजनिक __vpiSignal {
      अन्तःस्थ signal_int() { }
      पूर्णाङ्क get_type_संहिता(शून्य) ध्रुव override { प्रत्यागम vpiIntVar; }
};

संरचना signal_longint : सार्वजनिक __vpiSignal {
      अन्तःस्थ signal_longint() { }
      पूर्णाङ्क get_type_संहिता(शून्य) ध्रुव override { प्रत्यागम vpiLongIntVar; }
};


/*
 * Construct a vpiIntegerVar object. Indicate the type using a flag
 * to minimize the code modifications. Icarus implements integers
 * as 'reg signed [31:0]'.
 */
vpiHandle vpip_make_int4(ध्रुव वर्ण*name, पूर्णाङ्क msb, पूर्णाङ्क lsb, vvp_net_t*vec)
{
      __vpiSignal*obj = नूतन signal_integer;
      प्रत्यागम fill_अन्तर्_var4(obj, name, msb, lsb, सत्य, vec);
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
      यदि ((! signed_flag) || (lsb != 0) ) {
	    obj = नूतन signal_bitvar;
      } अन्यथा {
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

      प्रत्यागम fill_अन्तर्_var4(obj, name, msb, lsb, signed_flag, vec);
}

/*
 * Construct a vpiReg/vpiLogicVar object. It's like a net, except for the type.
 */
vpiHandle vpip_make_var4(ध्रुव वर्ण*name, पूर्णाङ्क msb, पूर्णाङ्क lsb,
			तार्किक signed_flag, vvp_net_t*vec)
{
      __vpiSignal*obj = नूतन signal_reg;
      प्रत्यागम fill_अन्तर्_var4(obj, name, msb, lsb, signed_flag, vec);
}

#यदि_परिभाषितम् CHECK_WITH_VALGRIND
स्थिर संरचना vpiSignal_plug **signal_pool = 0;
स्थिर अचिह्नित signal_pool_गणनम् = 0;
स्थिर अचिह्नित दीर्घ signal_गणनम् = 0;
स्थिर अचिह्नित दीर्घ signal_dels = 0;
#यदि_अन्तः

संरचना vpiSignal_plug {
      अचिह्नित वर्ण space[परिमाण (संरचना __vpiSignal)];
#यदि_परिभाषितम् CHECK_WITH_VALGRIND
      संरचना vpiSignal_plug *pool;
#यदि_अन्तः
};

शून्य* __vpiSignal::संक्रियकः नूतन(size_t siz)
{
#यदि_परिभाषितम् CHECK_WITH_VALGRIND
      प्रतिज्ञा(siz == परिमाण(संरचना vpiSignal_plug) - परिमाण(संरचना vpiSignal_plug *));
#अन्यथा
      प्रतिज्ञा(siz == परिमाण(संरचना vpiSignal_plug));
#यदि_अन्तः
      स्थिर संरचना vpiSignal_plug*alloc_array = 0;
      स्थिर अचिह्नित alloc_सूचकांक = 0;
      ध्रुव अचिह्नित alloc_गणनम् = 512;

      यदि ((alloc_array == 0) || (alloc_सूचकांक == alloc_गणनम्)) {
	    alloc_array = static_cast<संरचना vpiSignal_plug*>
	                  (शून्यस्मृतिं_देहि(alloc_गणनम्, परिमाण(संरचना vpiSignal_plug)));
	    alloc_सूचकांक = 0;
#यदि_परिभाषितम् CHECK_WITH_VALGRIND
	    VALGRIND_MAKE_MEM_NOACCESS(alloc_array, alloc_गणनम् *
	                                            परिमाण(संरचना vpiSignal_plug));
	    VALGRIND_CREATE_MEMPOOL(alloc_array, 0, 1);
	    signal_pool_गणनम् += 1;
	    signal_pool = static_cast<vpiSignal_plug **>
	                  (realloc(signal_pool,
	                   signal_pool_गणनम्*परिमाण(vpiSignal_plug **)));
	    signal_pool[signal_pool_गणनम्-1] = alloc_array;
#यदि_अन्तः
      }

      संरचना vpiSignal_plug*cur = alloc_array + alloc_सूचकांक;
#यदि_परिभाषितम् CHECK_WITH_VALGRIND
      VALGRIND_MEMPOOL_ALLOC(alloc_array, cur, परिमाण(संरचना vpiSignal_plug));
      cur->pool = alloc_array;
      signal_गणनम् += 1;
#यदि_अन्तः
      alloc_सूचकांक += 1;
      प्रत्यागम cur;
}

शून्य __vpiSignal::संक्रियकः विलोपय(शून्य*)
{
      प्रतिज्ञा(0);
}

#यदि_परिभाषितम् CHECK_WITH_VALGRIND
शून्य signal_delete(vpiHandle item)
{
      संरचना __vpiSignal *obj = static_cast<__vpiSignal *> (item);
      प्रतिज्ञा(obj->ग्रन्थिः->fil);
      obj->ग्रन्थिः->fil->clear_all_callbacks();
      vvp_net_delete(obj->ग्रन्थिः);
      यदि (obj->द्वयङ्काः) {
	    प्रति (अचिह्नित idx=0; idx<obj->width(); idx+=1) {
		  विलोपय obj->द्वयङ्काः[idx].सूचकांक;
	    }
	    obj->द्वयङ्काः -= 1;
	    विलोपय [] obj->द्वयङ्काः;
      }
      signal_dels += 1;
      VALGRIND_MEMPOOL_FREE(reinterpret_cast<vpiSignal_plug *>(obj)->pool, obj);
}

शून्य signal_pool_delete()
{
      यदि (RUNNING_ON_VALGRIND && (signal_गणनम् != signal_dels)) {
	    fflush(रिक्त);
	    VALGRIND_PRINTF("Error: vvp missed deleting %ld of %lu signal(s).",
	                    (दीर्घ) signal_गणनम् - signal_dels, signal_गणनम्);
      }

      प्रति (अचिह्नित idx = 0; idx < signal_pool_गणनम्; idx += 1) {
	    VALGRIND_DESTROY_MEMPOOL(signal_pool[idx]);
	    मुक्त(signal_pool[idx]);
      }

      मुक्त(signal_pool);
      signal_pool = 0;
      signal_pool_गणनम् = 0;
}
#यदि_अन्तः

/*
 * Construct a vpiNet object. Give the object specified dimensions,
 * and point to the specified functor for the lsb.
 *
 * The name is the PLI name for the object. If it is an array it is
 * <name>[<index>].
 */
स्थिर vpiHandle fill_अन्तर्_net4(संरचना __vpiSignal*obj, __vpiScope*scope,
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

      प्रत्यागम obj;
}

vpiHandle vpip_make_net4(__vpiScope*scope,
			 ध्रुव वर्ण*name, पूर्णाङ्क msb, पूर्णाङ्क lsb,
			 तार्किक signed_flag, vvp_net_t*ग्रन्थिः)
{
      संरचना __vpiSignal*obj = नूतन signal_net;
      प्रत्यागम fill_अन्तर्_net4(obj, scope, name, msb, lsb, signed_flag, ग्रन्थिः);
}

स्थिर पूर्णाङ्क PV_get_आधारः(संरचना __vpiPV*rfp)
{
	/* We return from the symbol base if it is defined. */
      यदि (rfp->sbase != 0) {
	    s_vpi_मानम् val;
	      /* Check to see if the value is defined. */
	    val.format = vpi_सदिशम्_Val;
	    vpi_get_मानम्(rfp->sbase, &val);
	    पूर्णाङ्क शब्दाः = (vpi_get(vpi_आकार, rfp->sbase)-1)/32 + 1;
	    प्रति(पूर्णाङ्क idx = 0; idx < शब्दाः; idx += 1) {
		    /* Return INT_MIN to indicate an X base. */
		  यदि (val.value.vector[idx].bval != 0) प्रत्यागम INT_MIN;
	    }
	      /* The value is defined so get and return it. */
	    val.format = vpiIntVal;
	    vpi_get_मानम्(rfp->sbase, &val);
	    प्रत्यागम val.value.integer;
      }

	/* If the width is zero then tbase is the constant. */
      प्रत्यागम rfp->tbase;
}

स्थिर पूर्णाङ्क PV_get(पूर्णाङ्क code, vpiHandle ref)
{
      संरचना __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      प्रतिज्ञा(rfp);

      पूर्णाङ्क rval = 0;
      विकल्प (code) {
	पक्ष vpi_पङ्क्तिः_No:
	    प्रत्यागम 0;  // Not implemented for now!

	पक्ष vpiSigned:
	    प्रत्यागम 0;  // A part/bit select is always unsigned!

	पक्ष vpi_आकार:
	    प्रत्यागम rfp->width;

	  /* This is like the &A<> in array.cc. */
	पक्ष vpiConstantSelect:
	    प्रत्यागम rfp->sbase == 0;

	पक्ष vpi_वामः_Range:
            rval += rfp->width - 1;
	    // fallthrough
	पक्ष vpi_दक्षिणः_Range: {
	    पूर्णाङ्क वामतः = vpi_get(vpi_वामः_Range, rfp->parent);
	    पूर्णाङ्क दक्षिणतः = vpi_get(vpi_दक्षिणः_Range, rfp->parent);
	    rval += PV_get_आधारः(rfp);
	    प्रत्यागम वामतः >= दक्षिणतः ? दक्षिणतः + rval : दक्षिणतः - rval;
	}

        पक्ष vpiAutomatic:
            प्रत्यागम vpi_get(vpiAutomatic, rfp->parent);

#यदि defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
        पक्ष _vpiFromThr:
            प्रत्यागम _vpi_at_PV;
#यदि_अन्तः

	स्वभाव:
	    fprintf(stderr, "PV_get: property %d is unknown\n", code);
      }

      प्रत्यागम 0;
}

स्थिर वर्ण* PV_get_str(पूर्णाङ्क code, vpiHandle ref)
{
      संरचना __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      प्रतिज्ञा(rfp);

      विकल्प (code) {
	पक्ष vpiFile:  // Not implemented for now!
	    प्रत्यागम simple_समुच्चयः_rbuf_str(file_नामानि[0]);

	पक्ष vpiName:
	पक्ष vpi_पूर्णम्_Name: {
	    ध्रुव वर्ण*nm = vpi_get_str(code, rfp->parent);
	    size_t len = 256+वाक्यदैर्घ्यम्(nm);
	    वर्ण *पूर्णम् = static_cast<वर्ण *>(स्मृतिं_देहि(len));
	    snprintf(पूर्णम्, len, "%s[%d:%d]", nm,
	                                     (पूर्णाङ्क)vpi_get(vpi_वामः_Range, ref),
	                                     (पूर्णाङ्क)vpi_get(vpi_दक्षिणः_Range, ref));
	    पूर्णम्[len-1] = 0;
	    वर्ण *res = simple_समुच्चयः_rbuf_str(पूर्णम्);
	    मुक्त(पूर्णम्);
	    प्रत्यागम res;
	}

	स्वभाव:
	    fprintf(stderr, "PV_get_str: property %d is unknown.\n", code);
      }

      प्रत्यागम 0;
}

स्थिर शून्य PV_get_मानम्(vpiHandle ref, p_vpi_मानम् vp)
{
      संरचना __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      प्रतिज्ञा(rfp);

      vvp_signal_मानम्*sig = dynamic_cast<vvp_signal_मानम्*>(rfp->net->fil);
      प्रतिज्ञा(sig);

      विकल्प (vp->format) {

	  पक्ष vpiIntVal:
	    format_vpiIntVal(sig, PV_get_आधारः(rfp), rfp->width, 0, vp);
	    भङ्ग;

	  पक्ष vpiBinStrVal:
	    format_vpiBinStrVal(sig, PV_get_आधारः(rfp), rfp->width, vp);
	    भङ्ग;

	  पक्ष vpiOctStrVal:
	    format_vpiOctStrVal(sig, PV_get_आधारः(rfp), rfp->width, vp);
	    भङ्ग;

	  पक्ष vpiHexStrVal:
	    format_vpiHexStrVal(sig, PV_get_आधारः(rfp), rfp->width, vp);
	    भङ्ग;

	  पक्ष vpiDecStrVal:
	    format_vpiDecStrVal(sig, PV_get_आधारः(rfp), rfp->width, 0, vp);
	    भङ्ग;

	  पक्ष vpiStringVal:
	    format_vpiStringVal(sig, PV_get_आधारः(rfp), rfp->width, vp);
	    भङ्ग;

	  पक्ष vpiScalarVal:
	    format_vpiScalarVal(sig, PV_get_आधारः(rfp), vp);
	    भङ्ग;

	  पक्ष vpiStrengthVal:
	    format_vpiStrengthVal(sig, PV_get_आधारः(rfp), rfp->width, vp);
	    भङ्ग;

	  पक्ष vpi_सदिशम्_Val:
	    format_vpi_सदिशम्_Val(sig, PV_get_आधारः(rfp), rfp->width, vp);
	    भङ्ग;

	  पक्ष vpiRealVal:
	    format_vpiRealVal(sig, PV_get_आधारः(rfp), rfp->width, 0, vp);
	    भङ्ग;

	  स्वभाव:
	    fprintf(stderr, "vvp internal error: PV_get_value: "
		    "value type %d not implemented. Signal is %s.\n",
		    (पूर्णाङ्क)vp->format, vpi_get_str(vpi_पूर्णम्_Name, rfp->parent));
	    प्रतिज्ञा(0);
      }
}

स्थिर vpiHandle PV_put_मानम्(vpiHandle ref, p_vpi_मानम् vp, पूर्णाङ्क flags)
{
      संरचना __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      प्रतिज्ञा(rfp);
      vvp_signal_मानम्*sig = dynamic_cast<vvp_signal_मानम्*>(rfp->net->fil);
      प्रतिज्ञा(sig);

      अचिह्नित sig_आकार = sig->मानम्_आकार();
      अचिह्नित width = rfp->width;
      पूर्णाङ्क आधारः = PV_get_आधारः(rfp);
      यदि (आधारः >= (चिह्नित) sig_आकार) प्रत्यागम 0;
      यदि (आधारः + (चिह्नित) width < 0) प्रत्यागम 0;

      vvp_vector4_t val;
      यदि (flags != vpiReleaseFlag) {
	    val = vec4_from_vpi_मानम्(vp, width);
      }

	/*
	 * If the base is less than zero then trim off any unneeded
	 * lower bits.
	 */
      यदि (आधारः < 0) {
	    width += आधारः;
	    यदि (flags != vpiReleaseFlag) {
		  val = val.subvalue(-आधारः, width);
	    }
	    आधारः = 0;
      }

	/*
	 * If the value is wider than the signal then trim off any
	 * unneeded upper bits.
	 */
      यदि (आधारः+width > sig_आकार) {
	    width = sig_आकार - आधारः;
	    यदि (flags != vpiReleaseFlag) {
		  val = val.subvalue(0, width);
	    }
      }

      प्रतिज्ञा(rfp->parent);
      तार्किक net_flag = rfp->parent->get_type_संहिता()==vpiNet;
      तार्किक पूर्णम्_sig = आधारः == 0 && width == sig_आकार;

      vvp_net_ptr_t dest(rfp->net, 0);

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value.  Note that
	   if this net is attached to an island, we need to rerun
	   the calculations immediately so we can return the
	   released value.*/
      यदि (flags == vpiReleaseFlag) {
	    प्रतिज्ञा(rfp->net->fil);
	      // XXXX Can't really do this if this is a partial release?
	    rfp->net->fil->force_unlink();
	    यदि (पूर्णम्_sig) {
		  rfp->net->fil->release(dest, net_flag);
	    } अन्यथा {
		  rfp->net->fil->release_pv(dest, आधारः, width, net_flag);
	    }
	    rfp->net->fun->force_flag(सत्य);
	    PV_get_मानम्(ref, vp);
	    प्रत्यागम ref;
      }

      यदि (flags == vpiForceFlag) {
	    यदि (पूर्णम्_sig) {
		  vvp_vector2_t आवरणम् (vvp_vector2_t::FILL1, sig_आकार);
		  rfp->net->force_vec4(val, आवरणम्);
	    } अन्यथा {
		  vvp_vector2_t आवरणम् (vvp_vector2_t::FILL0, sig_आकार);
		  प्रति (अचिह्नित idx = 0 ; idx < width ; idx += 1)
		        आवरणम्.समुच्चयः_bit(आधारः+idx, 1);

		  vvp_vector4_t tmp (sig_आकार, BIT4_Z);

		    // vvp_net_t::force_vec4 propagates all the bits of the
		    // forced vector value, regardless of the mask. This
		    // ensures the unforced bits retain their current value.
		  sig->vec4_मानम्(tmp);

		  tmp.समुच्चयः_vec(आधारः, val);
		  rfp->net->force_vec4(tmp, आवरणम्);
	    }
      } अन्यथा यदि (net_flag && !dynamic_cast<vvp_island_पत्तनम्*>(rfp->net->fun)) {
	    यदि (पूर्णम्_sig) {
		  rfp->net->send_vec4(val, vthread_get_wt_context());
	    } अन्यथा {
		  rfp->net->send_vec4_pv(val, आधारः, sig_आकार,
		                         vthread_get_wt_context());
	    }
      } अन्यथा {
	    यदि (पूर्णम्_sig) {
		  vvp_send_vec4(dest, val, vthread_get_wt_context());
	    } अन्यथा {
		  vvp_send_vec4_pv(dest, val, आधारः, sig_आकार,
	                           vthread_get_wt_context());
	    }
      }
      प्रत्यागम 0;
}

स्थिर vpiHandle PV_get_handle(पूर्णाङ्क code, vpiHandle ref)
{
      संरचना __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      प्रतिज्ञा(rfp);

      विकल्प (code) {
	  पक्ष vpiParent:
	    प्रत्यागम rfp->parent;

	  पक्ष vpiScope:
	    प्रत्यागम vpi_handle(vpiScope, rfp->parent);

	  पक्ष vpiModule:
	    प्रत्यागम vpi_handle(vpiModule, rfp->parent);
      }

      प्रत्यागम 0;
}

अन्तःस्थ __vpiPV::__vpiPV()
{ }

पूर्णाङ्क __vpiPV::get_type_संहिता(शून्य) ध्रुव
{ प्रत्यागम vpiPartSelect; }

पूर्णाङ्क __vpiPV::vpi_get(पूर्णाङ्क code)
{ प्रत्यागम PV_get(code, एतत्); }

वर्ण* __vpiPV::vpi_get_str(पूर्णाङ्क code)
{ प्रत्यागम PV_get_str(code, एतत्); }

शून्य __vpiPV::vpi_get_मानम्(p_vpi_मानम् val)
{ PV_get_मानम्(एतत्, val); }

vpiHandle __vpiPV::vpi_put_मानम्(p_vpi_मानम् val, पूर्णाङ्क flags)
{ प्रत्यागम PV_put_मानम्(एतत्, val, flags); }

vpiHandle __vpiPV::vpi_handle(पूर्णाङ्क code)
{ प्रत्यागम PV_get_handle(code, एतत्); }

vpiHandle vpip_make_PV(वर्ण*var, पूर्णाङ्क आधारः, पूर्णाङ्क width)
{
      संरचना __vpiPV*obj = नूतन __vpiPV;
      compile_vpi_lookup(&obj->parent, strdup(var));
      obj->sbase = 0;
      obj->tbase = आधारः;
      obj->width = (अचिह्नित) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      प्रत्यागम obj;
}

vpiHandle vpip_make_PV(वर्ण*var, वर्ण*चिह्नम्, पूर्णाङ्क width)
{
      संरचना __vpiPV*obj = नूतन __vpiPV;
      compile_vpi_lookup(&obj->parent, strdup(var));
      compile_vpi_lookup(&obj->sbase, चिह्नम्);
      obj->tbase = 0;
      obj->width = (अचिह्नित) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      प्रत्यागम obj;
}

vpiHandle vpip_make_PV(वर्ण*var, vpiHandle handle, पूर्णाङ्क width)
{
      संरचना __vpiPV*obj = नूतन __vpiPV;
      compile_vpi_lookup(&obj->parent, strdup(var));
      obj->sbase = handle;
      obj->tbase = 0;
      obj->width = (अचिह्नित) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      प्रत्यागम obj;
}

#यदि_परिभाषितम् CHECK_WITH_VALGRIND
शून्य PV_delete(vpiHandle item)
{
      संरचना __vpiPV *obj = dynamic_cast<__vpiPV*>(item);
      यदि (obj->sbase) {
	    विकल्प (obj->sbase->get_type_संहिता()) {
		पक्ष vpi_स्मृतिः_Word:
		  यदि (vpi_get(_vpiFromThr, obj->sbase) == _vpi_at_A) {
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
      विलोपय obj;
}
#यदि_अन्तः
