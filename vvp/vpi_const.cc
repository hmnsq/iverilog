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

# include  "vpi_priv.h"
# include  "compile.h"
# include  "config.h"
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#اگر_ختم
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <climits>
# include  <cassert>
# include  "ivl_alloc.h"

جماعت __vpiStringConst : عوامی __vpiHandle {
    عوامی:
      صریح __vpiStringConst(حرف*val);
      ~__vpiStringConst() override;
      صحیح get_type_رمز(خالی) ثابت override;
      صحیح vpi_get(صحیح code) override;
      خالی vpi_get_قدر(p_vpi_قدر val) override;

    نجی:
      خالی process_string_();
    نجی:
      حرف*قدر_2;
      size_t قدر_len;
};

اندرونی __vpiStringConst::__vpiStringConst(حرف*v)
: قدر_2(v)
{
      process_string_();
}

/*
 * Strings are described at the level of the vvp source as a string
 * with literal characters or octal escapes. No other escapes are
 * included, they are processed already by the compiler that generated
 * the vvp source.
 */
خالی __vpiStringConst::process_string_(خالی)
{
      حرف*chr = قدر_2;
      حرف*dp = قدر_2;

      جب (*chr) {
	    حرف اگلا_char = *chr;

	      /* Process octal escapes that I might find. */
	    اگر (*chr == '\\') {
		  برائے (صحیح idx = 1 ;  idx <= 3 ;  idx += 1) {
			أكد(chr[idx] != 0);
			أكد(chr[idx] < '8');
			أكد(chr[idx] >= '0');
			اگلا_char = اگلا_char*8 + chr[idx] - '0';
		  }
		  chr += 3;
	    }
	    *dp++ = اگلا_char;
	    chr += 1;
      }
      *dp = 0;
      قدر_len = dp - قدر_2;
}

__vpiStringConst::~__vpiStringConst()
{
      حذف[] قدر_2;
}

صحیح __vpiStringConst::get_type_رمز(خالی) ثابت
{ واپس vpiConstant; }

صحیح __vpiStringConst::vpi_get(صحیح code)
{
      انتخاب (code) {
          حالت vpi_سائز:
	    واپس قدر_len * 8;

          حالت vpiSigned:
	      واپس 0;

	  حالت vpiConstType:
	      واپس vpiStringConst;

          حالت vpiAutomatic:
	      واپس 0;

#اگر defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          حالت _vpiFromThr:
	      واپس _vpiNoThr;
#اگر_ختم

	  طےشدہ:
	      fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiStringConst\n", code);
	      أكد(0);
	      واپس 0;
      }
}

خالی __vpiStringConst::vpi_get_قدر(p_vpi_قدر vp)
{
      غیرعلامتی uint_قدر;
      p_vpi_vecval vecp;
      غیرعلامتی size = قدر_len;
      حرف*rbuf = 0;
      حرف*cp;

      انتخاب (vp->format) {
	  حالت vpiObjTypeVal:
	      /* String parameters by default have vpiStringVal values. */
	    vp->format = vpiStringVal;
	    // fallthrough
	  حالت vpiStringVal:
	    cp = قدر_2;
	    rbuf = static_cast<حرف *>(need_نتیجہ_buf(size + 1, RBUF_VAL));
	    vp->value.str = rbuf;

	    برائے (غیرعلامتی صحیح i = 0; i < size; i++) {
		  // Ignore leading null-bytes and replace other null-bytes with space.
		  // The LRM is not entirely clear on how null bytes should be handled.
		  // This is the implementation chosen for iverilog.
		  اگر (*cp)
			*rbuf++ = *cp;
		  ورنہ اگر (rbuf != vp->value.str)
			*rbuf++ = ' ';

		  cp++;
	    }
	    *rbuf = '\0';
	    توڑو;

          حالت vpiDecStrVal:
	    // Take the (up to 4) characters of the text, convert the characters
	    // to a numerical value, and convert that value to a decimal string.
	    // For example, the string "A" is ASCII 65, so the resulting string
	    // will be "65". and the string "AB" is 65*256 + 66 == 16706, so
	    // the resulting string is "16706". The "size" is the number of
	    // characters of input text to put to work.
	    اگر (size > 4){
		  // We only support standard integers. Ignore other bytes...
		  size = 4;
		  fprintf(stderr, "Warning (vpi_const.cc): %%d on constant strings only looks "
			  "at first 4 bytes!\n");
	    }
	    ساکن ثابت size_t RBUF_USE_سائز = 12;
	    rbuf = static_cast<حرف *>(need_نتیجہ_buf(RBUF_USE_سائز, RBUF_VAL));
	    uint_قدر = 0;
	    برائے(غیرعلامتی i=0; i<size; i += 1){
		  uint_قدر <<=8;
		  uint_قدر += (غیرعلامتی حرف)(قدر_2[i]);
	    }
	    snprintf(rbuf, RBUF_USE_سائز, "%u", uint_قدر);
	    vp->value.str = rbuf;
	    توڑو;

          حالت vpiBinStrVal:
	      rbuf = static_cast<حرف *>(need_نتیجہ_buf(8 * size + 1, RBUF_VAL));
	      cp = rbuf;
	      برائے(غیرعلامتی i=0; i<size; i += 1){
		  برائے(صحیح ثنائی_ہندسہ=7; ثنائی_ہندسہ>=0; ثنائی_ہندسہ -= 1){
		      *cp++ = "01"[ (قدر_2[i]>>ثنائی_ہندسہ)&1 ];
		  }
	      }
	      *cp = 0;
	      vp->value.str = rbuf;
	      توڑو;

          حالت vpiHexStrVal:
	      rbuf = static_cast<حرف *>(need_نتیجہ_buf(2 * size + 1, RBUF_VAL));
	      cp = rbuf;
	      برائے(غیرعلامتی i=0; i<size; i += 1){
		  برائے(صحیح nibble=1; nibble>=0; nibble -= 1){
		      *cp++ = "0123456789abcdef"[ (قدر_2[i]>>(nibble*4))&15 ];
		  }
	      }
	      *cp = 0;
	      vp->value.str = rbuf;
	      توڑو;

          حالت vpiOctStrVal:
	      fprintf(stderr, "ERROR (vpi_const.cc): %%o display of constant strings not yet implemented\n");
	      أكد(0);
	      توڑو;

          حالت vpiIntVal:
	      vp->value.integer = 0;
	      برائے(غیرعلامتی i=0; i<size; i += 1){
		  برائے(صحیح ثنائی_ہندسہ=7; ثنائی_ہندسہ>=0; ثنائی_ہندسہ -= 1){
		      vp->value.integer <<= 1;
		      vp->value.integer += (قدر_2[i]>>ثنائی_ہندسہ)&1;
		  }
	      }
	      توڑو;

          حالت vpi_سمتیہ_Val:
              vp->value.vector = static_cast<p_vpi_vecval>
                                 (need_نتیجہ_buf((size+3)/4*
                                                   حجم(s_vpi_vecval),
                                                  RBUF_VAL));
              uint_قدر = 0;
              vecp = vp->value.vector;
              vecp->aval = vecp->bval = 0;
	      برائے(غیرعلامتی i=0; i<size; i += 1){
		  vecp->aval |= قدر_2[i] << uint_قدر*8;
		  uint_قدر += 1;
		  اگر (uint_قدر > 3) {
		      uint_قدر = 0;
		      vecp += 1;
		      vecp->aval = vecp->bval = 0;
		  }
	      }
	      توڑو;


	  طےشدہ:
	    fprintf(stderr, "ERROR (vpi_const.cc): vp->format: %d\n",
	            (صحیح)vp->format);
	    أكد(0);

	    vp->format = vpiSuppressVal;
	    توڑو;
      }
}


ساخت __vpiStringConstTEMP : عوامی __vpiStringConst {
      صریح اندرونی __vpiStringConstTEMP(حرف*v) : __vpiStringConst(v) { }
      free_object_fun_t free_object_fun(خالی) override;
};

ساکن صحیح free_عارضی_string(vpiHandle obj)
{
      ساخت __vpiStringConstTEMP*rfp = dynamic_cast<__vpiStringConstTEMP*>(obj);
      حذف rfp;
      واپس 1;
}

__vpiHandle::free_object_fun_t __vpiStringConstTEMP::free_object_fun(خالی)
{ واپس &free_عارضی_string; }

vpiHandle vpip_make_string_const(حرف*متن, منطقی persistent_flag)
{
      __vpiStringConst*obj;

      obj = persistent_flag? نیا __vpiStringConst(متن) : نیا __vpiStringConstTEMP(متن);

      واپس obj;
}


جماعت __vpiStringParam  : عوامی __vpiStringConst {
    عوامی:
      __vpiStringParam(__vpiScope* scope, حرف*txt, ثابت حرف*name,
                       منطقی local_flag, طویل file_idx, طویل lineno);
      ~__vpiStringParam() override;
      صحیح get_type_رمز(خالی) ثابت override;
      صحیح vpi_get(صحیح code) override;
      حرف*vpi_get_str(صحیح code) override;
      vpiHandle vpi_handle(صحیح code) override;

    نجی:
      __vpiScope* scope_;
      ثابت حرف*basename_;
      منطقی       local_flag_;
      غیرعلامتی   file_idx_;
      غیرعلامتی   lineno_;
};

اندرونی __vpiStringParam::__vpiStringParam(__vpiScope* scope, حرف*txt, ثابت حرف*nam,
                                          منطقی local_flag, طویل file_idx, طویل lineno)
: __vpiStringConst(txt)
{
      scope_ = scope;
      basename_ = nam;
      local_flag_ = local_flag;
      أكد(file_idx >= 0 && file_idx <= زیادہ_سے_زیادہ_عدد);
      file_idx_ = (غیرعلامتی) file_idx;
      أكد(lineno >= 0 && lineno <= زیادہ_سے_زیادہ_عدد);
      lineno_ = (غیرعلامتی) lineno;
}

__vpiStringParam::~__vpiStringParam()
{
      حذف[]basename_;
}

صحیح __vpiStringParam::get_type_رمز(خالی) ثابت
{ واپس vpiParameter; }

صحیح __vpiStringParam::vpi_get(صحیح code)
{
    انتخاب (code) {
       حالت vpi_سطر_No :
         واپس lineno_;

       حالت vpiLocalParam :
         واپس local_flag_;

       طےشدہ :
         واپس __vpiStringConst::vpi_get(code);
    }
}


حرف*__vpiStringParam::vpi_get_str(صحیح code)
{
      اگر (code == vpiFile) {
	    واپس simple_سیٹ_rbuf_str(file_نام[file_idx_]);
      }

      واپس generic_get_str(code, scope_, basename_, تہی);
}


vpiHandle __vpiStringParam::vpi_handle(صحیح code)
{
      انتخاب (code) {
	  حالت vpiScope:
	    واپس scope_;

	  حالت vpiModule:
	    واپس vpip_module(scope_);

	  طےشدہ:
	    واپس 0;
      }
}

vpiHandle vpip_make_string_param(ثابت حرف*name, حرف*متن,
                                 منطقی local_flag, طویل file_idx, طویل lineno)
{
      __vpiStringParam*obj = نیا __vpiStringParam(vpip_peek_current_scope(), متن, name,
                                                  local_flag, file_idx, lineno);

      واپس obj;
}



اندرونی __vpiBinaryConst::__vpiBinaryConst()
: signed_flag(0), sized_flag(0)
{ }

صحیح __vpiBinaryConst::get_type_رمز(خالی) ثابت
{ واپس vpiConstant; }

صحیح __vpiBinaryConst::vpi_get(صحیح code)
{
      انتخاب (code) {
	  حالت vpiConstType:
	    واپس vpi_ثنائی_Const;

	  حالت vpi_سطر_No:
	    واپس 0;  // Not implemented for now!

	  حالت vpiSigned:
	    واپس signed_flag? 1 : 0;

	  حالت vpi_سائز:
	    واپس بٹ.size();

          حالت vpiAutomatic:
	    واپس 0;

#اگر defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          حالت _vpiFromThr:
	      واپس _vpiNoThr;
#اگر_ختم

	  طےشدہ:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiBinaryConst\n", code);
	    أكد(0);
	    واپس 0;
      }
}


خالی __vpiBinaryConst::vpi_get_قدر(p_vpi_قدر val)
{
      انتخاب (val->format) {

	  حالت vpiObjTypeVal:
	  حالت vpiBinStrVal:
	  حالت vpiDecStrVal:
	  حالت vpiOctStrVal:
	  حالت vpiHexStrVal:
          حالت vpiScalarVal:
	  حالت vpiIntVal:
	  حالت vpi_سمتیہ_Val:
	  حالت vpiStringVal:
	  حالت vpiRealVal:
	    vpip_vec4_get_قدر(بٹ, بٹ.size(), signed_flag, val);
	    توڑو;

	  طےشدہ:
	    fprintf(stderr, "vvp error: format %d not supported "
		    "by vpiBinaryConst\n", (صحیح)val->format);
	    val->format = vpiSuppressVal;
	    توڑو;
      }
}

/*
 * Make a VPI constant from a vector string. The string is normally a
 * ASCII string, with each letter a 4-value bit. The first character
 * may be an 's' if the vector is signed.
 */
vpiHandle vpip_make_ثنائی_const(غیرعلامتی wid, ثابت حرف*بٹ)
{
      ساخت __vpiBinaryConst*obj = نیا __vpiBinaryConst;

      ثابت حرف*bp = بٹ;
      اگر (*bp == 's') {
	    bp += 1;
	    obj->signed_flag = 1;
      }

      obj->بٹ = vector4_from_متن(bp, wid);

      واپس obj;
}

vvp_vector4_t vector4_from_متن(ثابت حرف*بٹ, غیرعلامتی wid)
{
      vvp_vector4_t res (wid);

      برائے (غیرعلامتی idx = 0 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t val = BIT4_0;
	    انتخاب (بٹ[wid-idx-1]) {
		حالت '0':
		  val = BIT4_0;
		  توڑو;
		حالت '1':
		  val = BIT4_1;
		  توڑو;
		حالت 'x':
		  val = BIT4_X;
		  توڑو;
		حالت 'z':
		  val = BIT4_Z;
		  توڑو;
	    }

	    res.سیٹ_ثنائی_ہندسہ(idx, val);
      }

      واپس res;
}

ساخت __vpiBinaryParam  : عوامی __vpiBinaryConst {
      __vpiBinaryParam(__vpiScope*scope, ثابت vvp_vector4_t&b, منطقی is_signed,
                       ثابت حرف*name, منطقی local_flag, طویل file_idx, طویل lineno);
      ~__vpiBinaryParam() override;
      صحیح get_type_رمز(خالی) ثابت override;
      صحیح vpi_get(صحیح code) override;
      حرف*vpi_get_str(صحیح code) override;
      vpiHandle vpi_handle(صحیح code) override;

    نجی:
      __vpiScope*scope_;
      ثابت حرف*basename_;
      منطقی     local_flag_;
      غیرعلامتی file_idx_;
      غیرعلامتی lineno_;
};

اندرونی __vpiBinaryParam::__vpiBinaryParam(__vpiScope*scope, ثابت vvp_vector4_t&b,
                                          منطقی is_signed, ثابت حرف*nam,
                                          منطقی local_flag, طویل file_idx, طویل lineno)
{
      scope_ = scope;
      بٹ = b;
      signed_flag = is_signed ? 1 : 0;
      sized_flag = 0;
      basename_ = nam;
      local_flag_ = local_flag;
      أكد(file_idx >= 0 && file_idx <= زیادہ_سے_زیادہ_عدد);
      file_idx_ = (غیرعلامتی) file_idx;
      أكد(lineno >= 0 && lineno <= زیادہ_سے_زیادہ_عدد);
      lineno_ = (غیرعلامتی) lineno;
}

__vpiBinaryParam::~__vpiBinaryParam()
{
      حذف[]basename_;
}

صحیح __vpiBinaryParam::get_type_رمز(خالی) ثابت
{ واپس vpiParameter; }

صحیح __vpiBinaryParam::vpi_get(صحیح code)
{
    انتخاب (code) {
      حالت vpi_سطر_No :
        واپس lineno_;

      حالت vpiLocalParam :
        واپس local_flag_;

      طےشدہ :
        واپس __vpiBinaryConst::vpi_get(code);
    }
}

حرف*__vpiBinaryParam::vpi_get_str(صحیح code)
{
      اگر (code == vpiFile)
	    واپس simple_سیٹ_rbuf_str(file_نام[file_idx_]);

      واپس generic_get_str(code, scope_, basename_, تہی);
}


vpiHandle __vpiBinaryParam::vpi_handle(صحیح code)
{
      انتخاب (code) {
	  حالت vpiScope:
	    واپس scope_;

	  حالت vpiModule:
	    واپس vpip_module(scope_);

	  طےشدہ:
	    واپس 0;
      }
}


vpiHandle vpip_make_ثنائی_param(ثابت حرف*name, ثابت vvp_vector4_t&بٹ,
				 منطقی signed_flag, منطقی local_flag,
				 طویل file_idx, طویل lineno)
{
      ساخت __vpiBinaryParam*obj = نیا __vpiBinaryParam(vpip_peek_current_scope(),
                                                         بٹ, signed_flag, name,
                                                         local_flag, file_idx, lineno);

      واپس obj;
}



__vpiDecConst::__vpiDecConst(صحیح val)
{
      value = val;
}

__vpiDecConst::__vpiDecConst(ثابت __vpiDecConst&that)
: __vpiHandle(), value(that.value)
{
}


صحیح __vpiDecConst::get_type_رمز(خالی) ثابت
{ واپس vpiConstant; }

صحیح __vpiDecConst::vpi_get(صحیح code)
{
      انتخاب (code) {
	  حالت vpiConstType:
	    واپس vpiDecConst;

	  حالت vpiSigned:
	    واپس 1;

	  حالت vpi_سائز:
	    واپس 32;

          حالت vpiAutomatic:
	    واپس 0;

#اگر defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          حالت _vpiFromThr:
	      واپس _vpiNoThr;
#اگر_ختم

	  طےشدہ:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiDecConst\n", code);
	    أكد(0);
	    واپس 0;
      }
}


خالی __vpiDecConst::vpi_get_قدر(p_vpi_قدر vp)
{
      ساکن ثابت size_t RBUF_USE_سائز = 64 + 1;
      حرف*rbuf = static_cast<حرف *>(need_نتیجہ_buf(RBUF_USE_سائز, RBUF_VAL));
      حرف*cp = rbuf;

      انتخاب (vp->format) {

	  حالت vpiObjTypeVal:
	  حالت vpiIntVal: {
		vp->value.integer = value;
		توڑو;
	  }

          حالت vpiDecStrVal:
	      snprintf(rbuf, RBUF_USE_سائز, "%d", value);
	      vp->value.str = rbuf;
	      توڑو;

          حالت vpiBinStrVal:
	      برائے(صحیح ثنائی_ہندسہ=31; ثنائی_ہندسہ>=0;ثنائی_ہندسہ--){
		  *cp++ = "01"[ (static_cast<غیرعلامتی>(value)>>ثنائی_ہندسہ)&1 ];
	      }
	      *cp = 0;

	      vp->value.str = rbuf;
	      توڑو;

          حالت vpiHexStrVal:
	      snprintf(rbuf, RBUF_USE_سائز, "%08x", value);
	      vp->value.str = rbuf;
	      توڑو;

          حالت vpiOctStrVal:
	      snprintf(rbuf, RBUF_USE_سائز, "%011x", value);
	      vp->value.str = rbuf;
	      توڑو;

	  طےشدہ:
	    fprintf(stderr, "vvp error (vpi_const.cc): format %d not supported "
		    "by vpiDecConst\n", (صحیح)vp->format);
	    vp->format = vpiSuppressVal;
	    توڑو;
      }
}


اندرونی __vpiRealConst::__vpiRealConst(دوہرا val)
: value(val)
{ }

صحیح __vpiRealConst::get_type_رمز(خالی) ثابت
{ واپس vpiConstant; }

صحیح __vpiRealConst::vpi_get(صحیح code)
{
      انتخاب (code) {
	  حالت vpi_سطر_No:
	    واپس 0;  // Not implemented for now!

	  حالت vpi_سائز:
	    واپس 1;

	  حالت vpiConstType:
	    واپس vpiRealConst;

	  حالت vpiSigned:
	    واپس 1;

          حالت vpiAutomatic:
	    واپس 0;

#اگر defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          حالت _vpiFromThr:
	      واپس _vpiNoThr;
#اگر_ختم

	  طےشدہ:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiRealConst\n", code);
	    أكد(0);
	    واپس 0;
      }
}


خالی __vpiRealConst::vpi_get_قدر(p_vpi_قدر val)
{
      vpip_real_get_قدر(value, val);
}


vpiHandle vpip_make_real_const(دوہرا value)
{
      __vpiRealConst*obj = نیا __vpiRealConst(value);
      واپس obj;
}

ساخت __vpiRealParam  : عوامی __vpiRealConst {
      __vpiRealParam(__vpiScope* scope, دوہرا val, ثابت حرف*name,
                     منطقی local_flag, طویل file_idx, طویل lineno);
      ~__vpiRealParam() override;
      صحیح get_type_رمز(خالی) ثابت override;
      صحیح vpi_get(صحیح code) override;
      حرف*vpi_get_str(صحیح code) override;
      vpiHandle vpi_handle(صحیح code) override;

    نجی:
      __vpiScope* scope_;
      ثابت حرف*basename_;
      منطقی local_flag_;
      غیرعلامتی file_idx_;
      غیرعلامتی lineno_;
};


اندرونی __vpiRealParam::__vpiRealParam(__vpiScope* scope, دوہرا val, ثابت حرف*name,
                                      منطقی local_flag, طویل file_idx, طویل lineno)
: __vpiRealConst(val)
{
      scope_ = scope;
      basename_ = name;
      local_flag_ = local_flag;
      أكد(file_idx >= 0 && file_idx <= زیادہ_سے_زیادہ_عدد);
      file_idx_ = (غیرعلامتی) file_idx;
      أكد(lineno >= 0 && lineno <= زیادہ_سے_زیادہ_عدد);
      lineno_ = (غیرعلامتی) lineno;
}

__vpiRealParam::~__vpiRealParam()
{
      حذف[]basename_;
}


صحیح __vpiRealParam::get_type_رمز(خالی) ثابت
{ واپس vpiParameter; }

صحیح __vpiRealParam::vpi_get(صحیح code)
{
    انتخاب (code) {
      حالت vpi_سطر_No :
        واپس lineno_;

      حالت vpiLocalParam :
        واپس local_flag_;

      طےشدہ :
           واپس __vpiRealConst::vpi_get(code);
    }
}

حرف* __vpiRealParam::vpi_get_str(صحیح code)
{
      اگر (code == vpiFile)
            واپس simple_سیٹ_rbuf_str(file_نام[file_idx_]);

      واپس generic_get_str(code, scope_, basename_, تہی);
}

vpiHandle __vpiRealParam::vpi_handle(صحیح code)
{
      انتخاب (code) {
          حالت vpiScope:
            واپس scope_;

	  حالت vpiModule:
	    واپس vpip_module(scope_);

          طےشدہ:
            واپس 0;
      }
}


vpiHandle vpip_make_real_param(ثابت حرف*name, دوہرا value,
                               منطقی local_flag, طویل file_idx, طویل lineno)
{
      ساخت __vpiRealParam*obj = نیا __vpiRealParam(vpip_peek_current_scope(),
                                                     value, name, local_flag,
                                                     file_idx, lineno);
      واپس obj;
}

/*
 * Make a VPI null constant
 */
اندرونی __vpiNullConst::__vpiNullConst()
{ }

صحیح __vpiNullConst::get_type_رمز(خالی) ثابت
{ واپس vpiConstant; }

صحیح __vpiNullConst::vpi_get(صحیح code)
{
      انتخاب (code) {
	  حالت vpi_سطر_No:
	    واپس 0;  // Not implemented for now!

	  حالت vpi_سائز:
	    واپس 32;

	  حالت vpiConstType:
	    واپس vpiNullConst;

	  حالت vpiSigned:
	    واپس 0;

          حالت vpiAutomatic:
	    واپس 0;

#اگر defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          حالت _vpiFromThr:
	      واپس _vpiNoThr;
#اگر_ختم

	  طےشدہ:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiNullConst\n", code);
	    أكد(0);
	    واپس 0;
      }
}

خالی __vpiNullConst::vpi_get_قدر(p_vpi_قدر val)
{
      ساکن ثابت size_t RBUF_USE_سائز = 64 + 1;
      حرف*rbuf = static_cast<حرف *>(need_نتیجہ_buf(RBUF_USE_سائز, RBUF_VAL));

      انتخاب (val->format) {

	  حالت vpiObjTypeVal:
	    val->format = vpiStringVal;
	    // fall through
	  حالت vpiBinStrVal:
	  حالت vpiDecStrVal:
	  حالت vpiOctStrVal:
	  حالت vpiHexStrVal:
	  حالت vpiStringVal:
	    snprintf(rbuf, RBUF_USE_سائز, "null");
	    val->value.str = rbuf;
	    توڑو;

          حالت vpiScalarVal:
	    val->value.scalar = vpi0;
	    توڑو;

	  حالت vpiIntVal:
	    val->value.integer = 0;
	    توڑو;

	  حالت vpi_سمتیہ_Val:
	    val->value.vector = static_cast<p_vpi_vecval>
	                        (need_نتیجہ_buf(2*حجم(s_vpi_vecval),
	                                         RBUF_VAL));
	    برائے (غیرعلامتی idx = 0; idx < 2; idx += 1) {
		  val->value.vector[idx].aval = 0;
		  val->value.vector[idx].bval = 0;
	    }
	    توڑو;

	  حالت vpiRealVal:
	    val->value.real = 0.0;
	    توڑو;

	  طےشدہ:
	    fprintf(stderr, "vvp error: format %d not supported "
		    "by vpiNullConst\n", (صحیح)val->format);
	    val->format = vpiSuppressVal;
	    توڑو;
      }
}

vpiHandle vpip_make_null_const()
{
      __vpiNullConst*obj = نیا __vpiNullConst;
      واپس obj;
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
خالی constant_delete(vpiHandle شے)
{
      أكد(شے->get_type_رمز() == vpiConstant);
      انتخاب(vpi_get(vpiConstType, شے)) {
	  حالت vpiStringConst:
	    حذف dynamic_cast<__vpiStringConst*>(شے);
	    توڑو;
	  حالت vpiDecConst:
	    حذف dynamic_cast<__vpiDecConst*>(شے);
	    توڑو;
	  حالت vpi_ثنائی_Const:
	    حذف dynamic_cast<__vpiBinaryConst*>(شے);
	    توڑو;
	  حالت vpiRealConst:
	    حذف dynamic_cast<__vpiRealConst*>(شے);
	    توڑو;
	  حالت vpiNullConst:
	    حذف dynamic_cast<__vpiNullConst*>(شے);
	    توڑو;
	  طےشدہ:
	    أكد(0);
      }
}

خالی parameter_delete(vpiHandle شے)
{
      انتخاب(vpi_get(vpiConstType, شے)) {
	  حالت vpiStringConst: {
	    جماعت __vpiStringParam*rfp = dynamic_cast<__vpiStringParam*>(شے);
	    حذف rfp;
	    توڑو; }
	  حالت vpi_ثنائی_Const: {
	    ساخت __vpiBinaryParam*rfp = dynamic_cast<__vpiBinaryParam*>(شے);
	    حذف rfp;
	    توڑو; }
	  حالت vpiRealConst: {
	    ساخت __vpiRealParam*rfp = dynamic_cast<__vpiRealParam*>(شے);
	    حذف rfp;
	    توڑو; }
	  طےشدہ:
	    أكد(0);
      }
}
#اگر_ختم
