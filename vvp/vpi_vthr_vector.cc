/*
 * Copyright (c) 2001-2026 Stephen Williams (steve@icarus.com)
 * Copyright (c) 2001 Stephan Boettcher <stephan@nevis.columbia.edu>
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

# include  "vpi_priv.h"
# include  "vthread.h"
# include  "config.h"
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
# include <map>
#اگر_ختم
# include  <cstdio>
# include  <cstdlib>
# include  <cassert>
# include  "ivl_alloc.h"

استعمال نامخلا std;

/*
 * Hex digits that represent 4-value bits of Verilog are not as
 * trivially obvious to display as if the bits were the usual 2-value
 * bits. So, although it is possible to write a function that
 * generates a correct character for 4*4-value bits, it is easier to
 * just perform the lookup in a table. This only takes 256 bytes,
 * which is not many executable instructions:-)
 *
 * The table is calculated as compile time, therefore, by the
 * draw_tt.c program.
 */
خارجی ثابت حرف hex_digits[256];
خارجی ثابت حرف oct_digits[64];


جماعت __vpiVThrWord : عوامی __vpiHandle {
    عوامی:
      صریح __vpiVThrWord(غیرعلامتی بنیاد);
      صحیح get_type_رمز(خالی) ثابت override;
      صحیح vpi_get(صحیح code) override;
      خالی vpi_get_قدر(p_vpi_قدر val) override;

      صحیح get_subtype() ثابت { واپس subtype; };
      غیرعلامتی get_اشاریہ() ثابت { واپس اشاریہ; };

    نجی:
      ثابت حرف* name;
      صحیح subtype;
      غیرعلامتی اشاریہ;
};

ساکن صحیح vthr_لفظ_get(صحیح code, vpiHandle ref)
{
      ثابت __vpiVThrWord*rfp = dynamic_cast<__vpiVThrWord*>(ref);

      انتخاب (code) {

	  حالت vpiConstType:
	    واپس rfp->get_subtype();

#اگر defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  حالت _vpiFromThr:
	    واپس vpi_لفظ;
#اگر_ختم

	  طےشدہ:
	    واپس 0;
      }
}

ساکن خالی vthr_real_get_قدر(vpiHandle ref, s_vpi_قدر*vp)
{
      ثابت __vpiVThrWord*obj = dynamic_cast<__vpiVThrWord*>(ref);
      ساکن ثابت size_t RBUF_USE_سائز = 66;
      حرف *rbuf = static_cast<حرف *>(need_نتیجہ_buf(RBUF_USE_سائز, RBUF_VAL));

      دوہرا val = 0.0;

	/* Get the actual value from the index. It is possible, by the
	   way, that the vpi_get_value is called from compiletf. If
	   that's the case, there will be no current thread, and this
	   will not have access to the proper value. Punt and return a
	   0.0 value instead. */
      اگر (vpip_current_vthread)
	    val = vthread_get_real_ڈھیر(vpip_current_vthread,
	                                 obj->get_اشاریہ());

      انتخاب (vp->format) {

	  حالت vpiObjTypeVal:
	    vp->format = vpiRealVal;
	    // fallthrough
	  حالت vpiRealVal:
	    vp->value.real = val;
	    توڑو;

	  حالت vpiIntVal:
	      /* NaN or +/- infinity are translated as 0. */
	    اگر (val != val || (val && (val == 0.5*val))) {
		  val = 0.0;
	    } ورنہ {
		  val = std::باری_2(val);
	    }
	    vp->value.integer = (PLI_INT32)val;
	    توڑو;

	  حالت vpiDecStrVal:
	    اگر (std::isnan(val))
		  snprintf(rbuf, RBUF_USE_سائز, "%s", "nan");
	    ورنہ
		  snprintf(rbuf, RBUF_USE_سائز, "%0.0f", std::باری_2(val));
	    vp->value.str = rbuf;
	    توڑو;

	  حالت vpiOctStrVal:
	    snprintf(rbuf, RBUF_USE_سائز, "%" PRIo64, vlg_باری_مقصد_u64(val));
	    vp->value.str = rbuf;
	    توڑو;

	  حالت vpiHexStrVal:
	    snprintf(rbuf, RBUF_USE_سائز, "%" PRIx64, vlg_باری_مقصد_u64(val));
	    vp->value.str = rbuf;
	    توڑو;

	  حالت vpiBinStrVal: {
		uint64_t vali = vlg_باری_مقصد_u64(val);
		غیرعلامتی len = 0;

		جب (vali > 0) {
		      len += 1;
		      vali /= 2;
		}

		vali = vlg_باری_مقصد_u64(val);
		برائے (غیرعلامتی idx = 0 ;  idx < len ;  idx += 1) {
		      rbuf[len-idx-1] = (vali & 1)? '1' : '0';
		      vali /= 2;
		}

		rbuf[len] = 0;
		اگر (len == 0) {
		      rbuf[0] = '0';
		      rbuf[1] = 0;
		}
		vp->value.str = rbuf;
		توڑو;
	  }

	  طےشدہ:
	    fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiConstant (Real)\n", (صحیح)vp->format);

	    vp->format = vpiSuppressVal;
	    توڑو;
      }
}

اندرونی __vpiVThrWord::__vpiVThrWord(غیرعلامتی بنیاد)
{
      name = vpip_name_string("W<>");
      subtype = vpiRealConst;
      اشاریہ = بنیاد;
}

صحیح __vpiVThrWord::get_type_رمز(خالی) ثابت
{ واپس vpiConstant; }

صحیح __vpiVThrWord::vpi_get(صحیح code)
{ واپس vthr_لفظ_get(code, یہ); }

خالی __vpiVThrWord::vpi_get_قدر(p_vpi_قدر val)
{ vthr_real_get_قدر(یہ, val); }

vpiHandle vpip_make_vthr_لفظ(غیرعلامتی بنیاد, ثابت حرف*type)
{
      أكد(type[0] == 'r');
      أكد(بنیاد < 65536);
      __vpiVThrWord*obj = نیا __vpiVThrWord(بنیاد);

      واپس obj;
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
ساکن map<vpiHandle, منطقی> handle_map;

خالی thread_لفظ_delete(vpiHandle شے)
{
      handle_map[شے] = جھوٹ;
}

ساکن خالی thread_لفظ_delete_real(vpiHandle شے)
{
      جماعت __vpiVThrWord*obj = dynamic_cast<__vpiVThrWord*>(شے);
      حذف obj;
}
#اگر_ختم

جماعت __vpiVThrStrStack : عوامی __vpiHandle {
    عوامی:
      صریح __vpiVThrStrStack(غیرعلامتی گہرائی);
      صحیح get_type_رمز(خالی) ثابت override;
      صحیح vpi_get(صحیح code) override;
      خالی vpi_get_قدر(p_vpi_قدر val) override;
    نجی:
      غیرعلامتی گہرائی_2;
};

__vpiVThrStrStack::__vpiVThrStrStack(غیرعلامتی d)
: گہرائی_2(d)
{
}

صحیح __vpiVThrStrStack::get_type_رمز(خالی) ثابت
{ واپس vpiConstant; }

صحیح __vpiVThrStrStack::vpi_get(صحیح code)
{
      انتخاب (code) {
	  حالت vpiConstType:
	    واپس vpiStringConst;
#اگر defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  حالت _vpiFromThr:
	    واپس _vpiString;
#اگر_ختم
	  حالت vpi_سائز:
	    واپس vthread_get_str_ڈھیر(vpip_current_vthread, گہرائی_2).size();
	  طےشدہ:
	    واپس 0;
      }
}

خالی __vpiVThrStrStack::vpi_get_قدر(p_vpi_قدر vp)
{
      string val;
      حرف*rbuf = 0;

      اگر (vpip_current_vthread)
	    val = vthread_get_str_ڈھیر(vpip_current_vthread, گہرائی_2);

      انتخاب (vp->format) {

	  حالت vpiObjTypeVal:
	    vp->format = vpiStringVal;
	    // fallthrough
	  حالت vpiStringVal:
	    rbuf = static_cast<حرف *>(need_نتیجہ_buf(val.size()+1, RBUF_VAL));
	    strcpy(rbuf, val.c_str());
	    vp->value.str = rbuf;
	    توڑو;

	  طےشدہ:
	    fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiConstant (String)\n", (صحیح)vp->format);

	    vp->format = vpiSuppressVal;
	    توڑو;
      }
}

جماعت __vpiVThrVec4Stack : عوامی __vpiHandle {
    عوامی:
      __vpiVThrVec4Stack(غیرعلامتی گہرائی, منطقی signed_flag, غیرعلامتی wid);
      صحیح get_type_رمز(خالی) ثابت override;
      صحیح vpi_get(صحیح code) override;
      حرف*vpi_get_str(صحیح code) override;
      خالی vpi_get_قدر(p_vpi_قدر val) override;
      vpiHandle vpi_put_قدر(p_vpi_قدر val, صحیح flags) override;
    نجی:
      ساکن خالی vpi_get_قدر_string(p_vpi_قدر vp, ثابت vvp_vector4_t&val);
      ساکن خالی vpi_get_قدر_binstr(p_vpi_قدر vp, ثابت vvp_vector4_t&val);
      خالی vpi_get_قدر_decstr(p_vpi_قدر vp, ثابت vvp_vector4_t&val);
      خالی vpi_get_قدر_int   (p_vpi_قدر vp, ثابت vvp_vector4_t&val);
      ساکن خالی vpi_get_قدر_real  (p_vpi_قدر vp, ثابت vvp_vector4_t&val);
      ساکن خالی vpi_get_قدر_strength(p_vpi_قدر vp, ثابت vvp_vector4_t&val);
      ساکن خالی vpi_get_قدر_octstr(p_vpi_قدر vp, ثابت vvp_vector4_t&val);
      ساکن خالی vpi_get_قدر_hexstr(p_vpi_قدر vp, ثابت vvp_vector4_t&val);
      ساکن خالی vpi_get_قدر_سمتیہ(p_vpi_قدر vp, ثابت vvp_vector4_t&val);
    نجی:
      غیرعلامتی گہرائی_2;
      منطقی signed_flag_;
      غیرعلامتی expect_چوڑائی;
      ثابت حرف*name;
};

__vpiVThrVec4Stack::__vpiVThrVec4Stack(غیرعلامتی d, منطقی sf, غیرعلامتی wid)
: گہرائی_2(d), signed_flag_(sf), expect_چوڑائی(wid)
{
      name = vpip_name_string("S<,vec4,>");
}

صحیح __vpiVThrVec4Stack::get_type_رمز(خالی) ثابت
{ واپس vpiConstant; }


صحیح __vpiVThrVec4Stack::vpi_get(صحیح code)
{
      انتخاب (code) {
	  حالت vpi_سائز:
	    واپس expect_چوڑائی;

	  حالت vpiSigned:
	    واپس signed_flag_? 1 : 0;

	  حالت vpiConstType:
	    واپس vpi_ثنائی_Const;

#اگر defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  حالت _vpiFromThr:
	    واپس _vpiVThr;
#اگر_ختم

	  طےشدہ:
	    واپس 0;
      }
}

حرف*__vpiVThrVec4Stack::vpi_get_str(صحیح code)
{
      انتخاب (code) {
	  حالت vpi_بھرا_Name:
	    واپس simple_سیٹ_rbuf_str(name);

	  طےشدہ:
	    واپس 0;
      }
}

خالی __vpiVThrVec4Stack::vpi_get_قدر(p_vpi_قدر vp)
{
      vvp_vector4_t val;

      اگر (vpip_current_vthread)
	    val = vthread_get_vec4_ڈھیر(vpip_current_vthread, گہرائی_2);

      انتخاب (vp->format) {

	  حالت vpiBinStrVal:
	    vpi_get_قدر_binstr(vp, val);
	    توڑو;
	  حالت vpiDecStrVal:
	    vpi_get_قدر_decstr(vp, val);
	    توڑو;
	  حالت vpiOctStrVal:
	    vpi_get_قدر_octstr(vp, val);
	    توڑو;
	  حالت vpiHexStrVal:
	    vpi_get_قدر_hexstr(vp, val);
	    توڑو;
	  حالت vpiIntVal:
	    vpi_get_قدر_int(vp, val);
	    توڑو;
	  حالت vpiRealVal:
	    vpi_get_قدر_real(vp, val);
	    توڑو;
	  حالت vpiStringVal:
	    vpi_get_قدر_string(vp, val);
	    توڑو;
	  حالت vpiStrengthVal:
	    vpi_get_قدر_strength(vp, val);
	    توڑو;
	  حالت vpiObjTypeVal:
	    vp->format = vpi_سمتیہ_Val;
	    // fallthrough
	  حالت vpi_سمتیہ_Val:
	    vpi_get_قدر_سمتیہ(vp, val);
	    توڑو;

	  طےشدہ:
	    fprintf(stderr, "internal error: vpi_get_value(<format=%d>)"
		    " not implemented for __vpiVThrVec4Stack.\n", vp->format);
	    أكد(0);
      }

}

خالی __vpiVThrVec4Stack::vpi_get_قدر_binstr(p_vpi_قدر vp, ثابت vvp_vector4_t&val)
{
      غیرعلامتی wid = val.size();
      حرف*rbuf = static_cast<حرف*>(need_نتیجہ_buf(wid+1, RBUF_VAL));
      برائے (غیرعلامتی idx = 0 ; idx < wid ; idx += 1) {
	    rbuf[wid-idx-1] = vvp_bit4_مقصد_ascii(val.value(idx));
      }
      rbuf[wid] = 0;
      vp->value.str = rbuf;
}

خالی __vpiVThrVec4Stack::vpi_get_قدر_decstr(p_vpi_قدر vp, ثابت vvp_vector4_t&val)
{
      غیرعلامتی wid = val.size();
      صحیح nbuf = (wid+2)/3 + 1;
      حرف *rbuf = static_cast<حرف*>(need_نتیجہ_buf(nbuf, RBUF_VAL));

      vpip_vec4_مقصد_dec_str(val, rbuf, nbuf, signed_flag_);
      vp->value.str = rbuf;
}

خالی __vpiVThrVec4Stack::vpi_get_قدر_octstr(p_vpi_قدر vp, ثابت vvp_vector4_t&val)
{
      غیرعلامتی wid = val.size();
      غیرعلامتی owid = (wid + 2) / 3;
      حرف*rbuf = static_cast<حرف*>(need_نتیجہ_buf(owid+1, RBUF_VAL));
      rbuf[owid] = 0;

      غیرعلامتی oval = 0;
      برائے (غیرعلامتی idx = 0; idx < wid ; idx += 1) {
	    غیرعلامتی tmp = 0;
	    انتخاب (val.value(idx)) {
		حالت BIT4_0:
		  tmp = 0;
		  توڑو;
		حالت BIT4_1:
		  tmp = 1;
		  توڑو;
		حالت BIT4_X:
		  tmp = 2;
		  توڑو;
		حالت BIT4_Z:
		  tmp = 3;
		  توڑو;
	    }

	    oval = oval | (tmp << 2*(idx%3));

	    اگر (idx%3 == 2) {
		  owid -= 1;
		  rbuf[owid] = oct_digits[oval];
		  oval = 0;
	    }
      }

      اگر (owid > 0) {
	    owid -= 1;
	    rbuf[owid] = oct_digits[oval];
      }
      vp->value.str = rbuf;
}

خالی __vpiVThrVec4Stack::vpi_get_قدر_hexstr(p_vpi_قدر vp, ثابت vvp_vector4_t&val)
{
      غیرعلامتی wid = val.size();
      غیرعلامتی hwid = (wid + 3) /4;
      حرف*rbuf = static_cast<حرف*>(need_نتیجہ_buf(hwid+1, RBUF_VAL));
      rbuf[hwid] = 0;

      غیرعلامتی hval = 0;
      برائے (غیرعلامتی idx = 0; idx < wid ; idx += 1) {
	    غیرعلامتی tmp = 0;
	    انتخاب (val.value(idx)) {
		حالت BIT4_0:
		  tmp = 0;
		  توڑو;
		حالت BIT4_1:
		  tmp = 1;
		  توڑو;
		حالت BIT4_X:
		  tmp = 2;
		  توڑو;
		حالت BIT4_Z:
		  tmp = 3;
		  توڑو;
	    }
	    hval = hval | (tmp << 2*(idx%4));

	    اگر (idx%4 == 3) {
		  hwid -= 1;
		  rbuf[hwid] = hex_digits[hval];
		  hval = 0;
	    }
      }

      اگر (hwid > 0) {
	    hwid -= 1;
	    rbuf[hwid] = hex_digits[hval];
      }
      vp->value.str = rbuf;
}

خالی __vpiVThrVec4Stack::vpi_get_قدر_int(p_vpi_قدر vp, ثابت vvp_vector4_t&val)
{
      int32_t vali = 0;
      vector4_مقصد_قدر(val, vali, signed_flag_, جھوٹ);
      vp->value.integer = vali;
}

خالی __vpiVThrVec4Stack::vpi_get_قدر_real(p_vpi_قدر vp, ثابت vvp_vector4_t&val)
{
      غیرعلامتی wid = val.size();
      vp->value.real = 0.0;

      برائے (غیرعلامتی idx = wid ; idx > 0 ; idx -= 1) {
	    vp->value.real *= 2.0;
	    اگر (val.value(idx-1) == BIT4_1)
		  vp->value.real += 1.0;
      }
}

خالی __vpiVThrVec4Stack::vpi_get_قدر_string(p_vpi_قدر vp, ثابت vvp_vector4_t&val)
{
      حرف*rbuf = static_cast<حرف*>(need_نتیجہ_buf((val.size() / 8) + 1, RBUF_VAL));
      حرف*cp = rbuf;

      حرف tmp = 0;
      برائے (صحیح bitnr = val.size()-1 ; bitnr >= 0 ; bitnr -= 1) {
	    tmp <<= 1;
	    انتخاب (val.value(bitnr)) {
		حالت BIT4_1:
		  tmp |= 1;
		  توڑو;
		حالت BIT4_0:
		طےشدہ:
		  توڑو;
	    }

	    اگر ((bitnr&7)==0) {
		    // Don't include leading nuls
		  اگر (tmp == 0 && cp == rbuf)
			جاری;

		  *cp++ = tmp? tmp : ' ';
		  tmp = 0;
	    }
      }
      *cp++ = 0;
      vp->format = vpiStringVal;
      vp->value.str = rbuf;
}

خالی __vpiVThrVec4Stack::vpi_get_قدر_سمتیہ(p_vpi_قدر vp, ثابت vvp_vector4_t&val)
{
      غیرعلامتی wid = val.size();

      غیرعلامتی hwid = (wid+31)/32;
      vp->value.vector = static_cast<s_vpi_vecval*>
                         (need_نتیجہ_buf(hwid*حجم(s_vpi_vecval), RBUF_VAL));
      أكد(vp->value.vector);

	/* Zero the top of the result buffer first since the fill loop below only writes
	   bits 0..wid-1, which can leave stale data in the unused high bits. */
      اگر (hwid == 0) hwid = 1;
      vp->value.vector[hwid-1].aval = 0;
      vp->value.vector[hwid-1].bval = 0;

      برائے (غیرعلامتی idx = 0 ;  idx < wid ;  idx += 1) {
	    صحیح لفظ = idx/32;
	    PLI_INT32 نقاب = 1 << (idx%32);

	    انتخاب (val.value(idx)) {
		حالت BIT4_0:
		  vp->value.vector[لفظ].aval &= ~نقاب;
		  vp->value.vector[لفظ].bval &= ~نقاب;
		  توڑو;
		حالت BIT4_1:
		  vp->value.vector[لفظ].aval |=  نقاب;
		  vp->value.vector[لفظ].bval &= ~نقاب;
		  توڑو;
		حالت BIT4_X:
		  vp->value.vector[لفظ].aval |=  نقاب;
		  vp->value.vector[لفظ].bval |=  نقاب;
		  توڑو;
		حالت BIT4_Z:
		  vp->value.vector[لفظ].aval &= ~نقاب;
		  vp->value.vector[لفظ].bval |=  نقاب;
		  توڑو;
	    }
      }
}

خالی __vpiVThrVec4Stack::vpi_get_قدر_strength(p_vpi_قدر vp, ثابت vvp_vector4_t&val)
{
      s_vpi_strengthval*op = static_cast<s_vpi_strengthval*>
                             (need_نتیجہ_buf(val.size() * حجم(s_vpi_strengthval),
                                              RBUF_VAL));

      برائے (غیرعلامتی idx = 0 ; idx < val.size() ; idx += 1) {
	    انتخاب (val.value(idx)) {
		حالت BIT4_0:
		  op[idx].logic = vpi0;
		  op[idx].s0 = vpiStrongDrive;
		  op[idx].s1 = 0;
		  توڑو;
		حالت BIT4_1:
		  op[idx].logic = vpi1;
		  op[idx].s0 = 0;
		  op[idx].s1 = vpiStrongDrive;
		  توڑو;
		حالت BIT4_X:
		  op[idx].logic = vpiX;
		  op[idx].s0 = vpiStrongDrive;
		  op[idx].s1 = vpiStrongDrive;
		  توڑو;
		حالت BIT4_Z:
		  op[idx].logic = vpiZ;
		  op[idx].s0 = vpiHiZ;
		  op[idx].s1 = vpiHiZ;
		  توڑو;
	    }
      }

      vp->format = vpiStrengthVal;
      vp->value.strength = op;
}

vpiHandle __vpiVThrVec4Stack::vpi_put_قدر(p_vpi_قدر vp, صحیح /*flags*/)
{
      أكد(vpip_current_vthread);

      انتخاب (vp->format) {

	  طےشدہ:
	    fprintf(stderr, "internal error: vpi_put_value(<format=%d>)"
		    " not implemented for __vpiVThrVec4Stack.\n", vp->format);
	    أكد(0);
	    واپس 0;
      }
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
خالی thread_vthr_delete(vpiHandle شے)
{
      handle_map[شے] = سچ;
}

ساکن خالی thread_vthr_delete_real(vpiHandle شے)
{
      __vpiVThrVec4Stack*obj = dynamic_cast<__vpiVThrVec4Stack*>(شے);
      حذف obj;
}

خالی vpi_handle_delete()
{
      map<vpiHandle, منطقی>::iterator iter;
      برائے (iter = handle_map.begin(); iter != handle_map.انتہا(); ++ iter ) {
	    اگر (iter->second) thread_vthr_delete_real(iter->پہلا);
	    ورنہ thread_لفظ_delete_real(iter->پہلا);
      }
}
#اگر_ختم


vpiHandle vpip_make_vthr_str_ڈھیر(غیرعلامتی گہرائی)
{
      __vpiVThrStrStack*obj = نیا __vpiVThrStrStack(گہرائی);
      واپس obj;
}

vpiHandle vpip_make_vthr_vec4_ڈھیر(غیرعلامتی گہرائی, منطقی signed_flag, غیرعلامتی wid)
{
      __vpiVThrVec4Stack*obj = نیا __vpiVThrVec4Stack(گہرائی, signed_flag, wid);
      واپس obj;
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
ساکن map<vpiHandle, منطقی> ڈھیر_map;

خالی thread_string_delete(vpiHandle شے)
{
      ڈھیر_map[شے] = جھوٹ;
}

ساکن خالی thread_string_delete_real(vpiHandle شے)
{
      __vpiVThrStrStack*obj = dynamic_cast<__vpiVThrStrStack*>(شے);
      حذف obj;
}

خالی vpi_ڈھیر_delete()
{
      map<vpiHandle, منطقی>::iterator iter;
      برائے (iter = ڈھیر_map.begin(); iter != ڈھیر_map.انتہا(); ++ iter ) {
	    thread_string_delete_real(iter->پہلا);
      }
}
#اگر_ختم
