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
#اگر_تعریف_شده CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#پایان_اگر
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <climits>
# include  <cassert>
# include  "ivl_alloc.h"

صنف __vpiStringConst : عام __vpiHandle {
    عام:
      صریح __vpiStringConst(حرف*val);
      ~__vpiStringConst() override;
      صحيح get_type_کد(فراغ) ثابت override;
      صحيح vpi_get(صحيح code) override;
      فراغ vpi_get_مقدار(p_vpi_مقدار val) override;

    خاص:
      فراغ process_string_();
    خاص:
      حرف*مقدار_2;
      size_t مقدار_len;
};

مضمن __vpiStringConst::__vpiStringConst(حرف*v)
: مقدار_2(v)
{
      process_string_();
}

/*
 * Strings are described at the level of the vvp source as a string
 * with literal characters or octal escapes. No other escapes are
 * included, they are processed already by the compiler that generated
 * the vvp source.
 */
فراغ __vpiStringConst::process_string_(فراغ)
{
      حرف*chr = مقدار_2;
      حرف*dp = مقدار_2;

      طالما (*chr) {
	    حرف بعدی_char = *chr;

	      /* Process octal escapes that I might find. */
	    إذا (*chr == '\\') {
		  لكل (صحيح idx = 1 ;  idx <= 3 ;  idx += 1) {
			أكد(chr[idx] != 0);
			أكد(chr[idx] < '8');
			أكد(chr[idx] >= '0');
			بعدی_char = بعدی_char*8 + chr[idx] - '0';
		  }
		  chr += 3;
	    }
	    *dp++ = بعدی_char;
	    chr += 1;
      }
      *dp = 0;
      مقدار_len = dp - مقدار_2;
}

__vpiStringConst::~__vpiStringConst()
{
      حذف[] مقدار_2;
}

صحيح __vpiStringConst::get_type_کد(فراغ) ثابت
{ أرجع vpiConstant; }

صحيح __vpiStringConst::vpi_get(صحيح code)
{
      بدل (code) {
          حالة vpi_اندازه:
	    أرجع مقدار_len * 8;

          حالة vpiSigned:
	      أرجع 0;

	  حالة vpiConstType:
	      أرجع vpiStringConst;

          حالة vpiAutomatic:
	      أرجع 0;

#إذا defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          حالة _vpiFromThr:
	      أرجع _vpiNoThr;
#پایان_اگر

	  افتراضي:
	      fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiStringConst\n", code);
	      أكد(0);
	      أرجع 0;
      }
}

فراغ __vpiStringConst::vpi_get_مقدار(p_vpi_مقدار vp)
{
      بیعلامت uint_مقدار;
      p_vpi_vecval vecp;
      بیعلامت size = مقدار_len;
      حرف*rbuf = 0;
      حرف*cp;

      بدل (vp->format) {
	  حالة vpiObjTypeVal:
	      /* String parameters by default have vpiStringVal values. */
	    vp->format = vpiStringVal;
	    // fallthrough
	  حالة vpiStringVal:
	    cp = مقدار_2;
	    rbuf = static_cast<حرف *>(need_نتیجه_buf(size + 1, RBUF_VAL));
	    vp->value.str = rbuf;

	    لكل (بیعلامت صحيح i = 0; i < size; i++) {
		  // Ignore leading null-bytes and replace other null-bytes with space.
		  // The LRM is not entirely clear on how null bytes should be handled.
		  // This is the implementation chosen for iverilog.
		  إذا (*cp)
			*rbuf++ = *cp;
		  وإلا إذا (rbuf != vp->value.str)
			*rbuf++ = ' ';

		  cp++;
	    }
	    *rbuf = '\0';
	    اكسر;

          حالة vpiDecStrVal:
	    // Take the (up to 4) characters of the text, convert the characters
	    // to a numerical value, and convert that value to a decimal string.
	    // For example, the string "A" is ASCII 65, so the resulting string
	    // will be "65". and the string "AB" is 65*256 + 66 == 16706, so
	    // the resulting string is "16706". The "size" is the number of
	    // characters of input text to put to work.
	    إذا (size > 4){
		  // We only support standard integers. Ignore other bytes...
		  size = 4;
		  fprintf(stderr, "Warning (vpi_const.cc): %%d on constant strings only looks "
			  "at first 4 bytes!\n");
	    }
	    ساكن ثابت size_t RBUF_USE_اندازه = 12;
	    rbuf = static_cast<حرف *>(need_نتیجه_buf(RBUF_USE_اندازه, RBUF_VAL));
	    uint_مقدار = 0;
	    لكل(بیعلامت i=0; i<size; i += 1){
		  uint_مقدار <<=8;
		  uint_مقدار += (بیعلامت حرف)(مقدار_2[i]);
	    }
	    snprintf(rbuf, RBUF_USE_اندازه, "%u", uint_مقدار);
	    vp->value.str = rbuf;
	    اكسر;

          حالة vpiBinStrVal:
	      rbuf = static_cast<حرف *>(need_نتیجه_buf(8 * size + 1, RBUF_VAL));
	      cp = rbuf;
	      لكل(بیعلامت i=0; i<size; i += 1){
		  لكل(صحيح بیت=7; بیت>=0; بیت -= 1){
		      *cp++ = "01"[ (مقدار_2[i]>>بیت)&1 ];
		  }
	      }
	      *cp = 0;
	      vp->value.str = rbuf;
	      اكسر;

          حالة vpiHexStrVal:
	      rbuf = static_cast<حرف *>(need_نتیجه_buf(2 * size + 1, RBUF_VAL));
	      cp = rbuf;
	      لكل(بیعلامت i=0; i<size; i += 1){
		  لكل(صحيح nibble=1; nibble>=0; nibble -= 1){
		      *cp++ = "0123456789abcdef"[ (مقدار_2[i]>>(nibble*4))&15 ];
		  }
	      }
	      *cp = 0;
	      vp->value.str = rbuf;
	      اكسر;

          حالة vpiOctStrVal:
	      fprintf(stderr, "ERROR (vpi_const.cc): %%o display of constant strings not yet implemented\n");
	      أكد(0);
	      اكسر;

          حالة vpiIntVal:
	      vp->value.integer = 0;
	      لكل(بیعلامت i=0; i<size; i += 1){
		  لكل(صحيح بیت=7; بیت>=0; بیت -= 1){
		      vp->value.integer <<= 1;
		      vp->value.integer += (مقدار_2[i]>>بیت)&1;
		  }
	      }
	      اكسر;

          حالة vpi_وکتور_Val:
              vp->value.vector = static_cast<p_vpi_vecval>
                                 (need_نتیجه_buf((size+3)/4*
                                                   حجم(s_vpi_vecval),
                                                  RBUF_VAL));
              uint_مقدار = 0;
              vecp = vp->value.vector;
              vecp->aval = vecp->bval = 0;
	      لكل(بیعلامت i=0; i<size; i += 1){
		  vecp->aval |= مقدار_2[i] << uint_مقدار*8;
		  uint_مقدار += 1;
		  إذا (uint_مقدار > 3) {
		      uint_مقدار = 0;
		      vecp += 1;
		      vecp->aval = vecp->bval = 0;
		  }
	      }
	      اكسر;


	  افتراضي:
	    fprintf(stderr, "ERROR (vpi_const.cc): vp->format: %d\n",
	            (صحيح)vp->format);
	    أكد(0);

	    vp->format = vpiSuppressVal;
	    اكسر;
      }
}


بنية __vpiStringConstTEMP : عام __vpiStringConst {
      صریح مضمن __vpiStringConstTEMP(حرف*v) : __vpiStringConst(v) { }
      free_object_fun_t free_object_fun(فراغ) override;
};

ساكن صحيح free_موقت_string(vpiHandle obj)
{
      بنية __vpiStringConstTEMP*rfp = dynamic_cast<__vpiStringConstTEMP*>(obj);
      حذف rfp;
      أرجع 1;
}

__vpiHandle::free_object_fun_t __vpiStringConstTEMP::free_object_fun(فراغ)
{ أرجع &free_موقت_string; }

vpiHandle vpip_make_string_const(حرف*متن, منطقي persistent_flag)
{
      __vpiStringConst*obj;

      obj = persistent_flag? جديد __vpiStringConst(متن) : جديد __vpiStringConstTEMP(متن);

      أرجع obj;
}


صنف __vpiStringParam  : عام __vpiStringConst {
    عام:
      __vpiStringParam(__vpiScope* scope, حرف*txt, ثابت حرف*name,
                       منطقي local_flag, دراز file_idx, دراز lineno);
      ~__vpiStringParam() override;
      صحيح get_type_کد(فراغ) ثابت override;
      صحيح vpi_get(صحيح code) override;
      حرف*vpi_get_str(صحيح code) override;
      vpiHandle vpi_handle(صحيح code) override;

    خاص:
      __vpiScope* scope_;
      ثابت حرف*basename_;
      منطقي       local_flag_;
      بیعلامت   file_idx_;
      بیعلامت   lineno_;
};

مضمن __vpiStringParam::__vpiStringParam(__vpiScope* scope, حرف*txt, ثابت حرف*nam,
                                          منطقي local_flag, دراز file_idx, دراز lineno)
: __vpiStringConst(txt)
{
      scope_ = scope;
      basename_ = nam;
      local_flag_ = local_flag;
      أكد(file_idx >= 0 && file_idx <= بیشترین_صحیح);
      file_idx_ = (بیعلامت) file_idx;
      أكد(lineno >= 0 && lineno <= بیشترین_صحیح);
      lineno_ = (بیعلامت) lineno;
}

__vpiStringParam::~__vpiStringParam()
{
      حذف[]basename_;
}

صحيح __vpiStringParam::get_type_کد(فراغ) ثابت
{ أرجع vpiParameter; }

صحيح __vpiStringParam::vpi_get(صحيح code)
{
    بدل (code) {
       حالة vpi_سطر_No :
         أرجع lineno_;

       حالة vpiLocalParam :
         أرجع local_flag_;

       افتراضي :
         أرجع __vpiStringConst::vpi_get(code);
    }
}


حرف*__vpiStringParam::vpi_get_str(صحيح code)
{
      إذا (code == vpiFile) {
	    أرجع simple_مجموعه_rbuf_str(file_نامها[file_idx_]);
      }

      أرجع generic_get_str(code, scope_, basename_, عدم);
}


vpiHandle __vpiStringParam::vpi_handle(صحيح code)
{
      بدل (code) {
	  حالة vpiScope:
	    أرجع scope_;

	  حالة vpiModule:
	    أرجع vpip_module(scope_);

	  افتراضي:
	    أرجع 0;
      }
}

vpiHandle vpip_make_string_param(ثابت حرف*name, حرف*متن,
                                 منطقي local_flag, دراز file_idx, دراز lineno)
{
      __vpiStringParam*obj = جديد __vpiStringParam(vpip_peek_current_scope(), متن, name,
                                                  local_flag, file_idx, lineno);

      أرجع obj;
}



مضمن __vpiBinaryConst::__vpiBinaryConst()
: signed_flag(0), sized_flag(0)
{ }

صحيح __vpiBinaryConst::get_type_کد(فراغ) ثابت
{ أرجع vpiConstant; }

صحيح __vpiBinaryConst::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpiConstType:
	    أرجع vpi_دودویی_Const;

	  حالة vpi_سطر_No:
	    أرجع 0;  // Not implemented for now!

	  حالة vpiSigned:
	    أرجع signed_flag? 1 : 0;

	  حالة vpi_اندازه:
	    أرجع بیتها.size();

          حالة vpiAutomatic:
	    أرجع 0;

#إذا defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          حالة _vpiFromThr:
	      أرجع _vpiNoThr;
#پایان_اگر

	  افتراضي:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiBinaryConst\n", code);
	    أكد(0);
	    أرجع 0;
      }
}


فراغ __vpiBinaryConst::vpi_get_مقدار(p_vpi_مقدار val)
{
      بدل (val->format) {

	  حالة vpiObjTypeVal:
	  حالة vpiBinStrVal:
	  حالة vpiDecStrVal:
	  حالة vpiOctStrVal:
	  حالة vpiHexStrVal:
          حالة vpiScalarVal:
	  حالة vpiIntVal:
	  حالة vpi_وکتور_Val:
	  حالة vpiStringVal:
	  حالة vpiRealVal:
	    vpip_vec4_get_مقدار(بیتها, بیتها.size(), signed_flag, val);
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "vvp error: format %d not supported "
		    "by vpiBinaryConst\n", (صحيح)val->format);
	    val->format = vpiSuppressVal;
	    اكسر;
      }
}

/*
 * Make a VPI constant from a vector string. The string is normally a
 * ASCII string, with each letter a 4-value bit. The first character
 * may be an 's' if the vector is signed.
 */
vpiHandle vpip_make_دودویی_const(بیعلامت wid, ثابت حرف*بیتها)
{
      بنية __vpiBinaryConst*obj = جديد __vpiBinaryConst;

      ثابت حرف*bp = بیتها;
      إذا (*bp == 's') {
	    bp += 1;
	    obj->signed_flag = 1;
      }

      obj->بیتها = vector4_from_متن(bp, wid);

      أرجع obj;
}

vvp_vector4_t vector4_from_متن(ثابت حرف*بیتها, بیعلامت wid)
{
      vvp_vector4_t res (wid);

      لكل (بیعلامت idx = 0 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t val = BIT4_0;
	    بدل (بیتها[wid-idx-1]) {
		حالة '0':
		  val = BIT4_0;
		  اكسر;
		حالة '1':
		  val = BIT4_1;
		  اكسر;
		حالة 'x':
		  val = BIT4_X;
		  اكسر;
		حالة 'z':
		  val = BIT4_Z;
		  اكسر;
	    }

	    res.مجموعه_بیت(idx, val);
      }

      أرجع res;
}

بنية __vpiBinaryParam  : عام __vpiBinaryConst {
      __vpiBinaryParam(__vpiScope*scope, ثابت vvp_vector4_t&b, منطقي is_signed,
                       ثابت حرف*name, منطقي local_flag, دراز file_idx, دراز lineno);
      ~__vpiBinaryParam() override;
      صحيح get_type_کد(فراغ) ثابت override;
      صحيح vpi_get(صحيح code) override;
      حرف*vpi_get_str(صحيح code) override;
      vpiHandle vpi_handle(صحيح code) override;

    خاص:
      __vpiScope*scope_;
      ثابت حرف*basename_;
      منطقي     local_flag_;
      بیعلامت file_idx_;
      بیعلامت lineno_;
};

مضمن __vpiBinaryParam::__vpiBinaryParam(__vpiScope*scope, ثابت vvp_vector4_t&b,
                                          منطقي is_signed, ثابت حرف*nam,
                                          منطقي local_flag, دراز file_idx, دراز lineno)
{
      scope_ = scope;
      بیتها = b;
      signed_flag = is_signed ? 1 : 0;
      sized_flag = 0;
      basename_ = nam;
      local_flag_ = local_flag;
      أكد(file_idx >= 0 && file_idx <= بیشترین_صحیح);
      file_idx_ = (بیعلامت) file_idx;
      أكد(lineno >= 0 && lineno <= بیشترین_صحیح);
      lineno_ = (بیعلامت) lineno;
}

__vpiBinaryParam::~__vpiBinaryParam()
{
      حذف[]basename_;
}

صحيح __vpiBinaryParam::get_type_کد(فراغ) ثابت
{ أرجع vpiParameter; }

صحيح __vpiBinaryParam::vpi_get(صحيح code)
{
    بدل (code) {
      حالة vpi_سطر_No :
        أرجع lineno_;

      حالة vpiLocalParam :
        أرجع local_flag_;

      افتراضي :
        أرجع __vpiBinaryConst::vpi_get(code);
    }
}

حرف*__vpiBinaryParam::vpi_get_str(صحيح code)
{
      إذا (code == vpiFile)
	    أرجع simple_مجموعه_rbuf_str(file_نامها[file_idx_]);

      أرجع generic_get_str(code, scope_, basename_, عدم);
}


vpiHandle __vpiBinaryParam::vpi_handle(صحيح code)
{
      بدل (code) {
	  حالة vpiScope:
	    أرجع scope_;

	  حالة vpiModule:
	    أرجع vpip_module(scope_);

	  افتراضي:
	    أرجع 0;
      }
}


vpiHandle vpip_make_دودویی_param(ثابت حرف*name, ثابت vvp_vector4_t&بیتها,
				 منطقي signed_flag, منطقي local_flag,
				 دراز file_idx, دراز lineno)
{
      بنية __vpiBinaryParam*obj = جديد __vpiBinaryParam(vpip_peek_current_scope(),
                                                         بیتها, signed_flag, name,
                                                         local_flag, file_idx, lineno);

      أرجع obj;
}



__vpiDecConst::__vpiDecConst(صحيح val)
{
      value = val;
}

__vpiDecConst::__vpiDecConst(ثابت __vpiDecConst&that)
: __vpiHandle(), value(that.value)
{
}


صحيح __vpiDecConst::get_type_کد(فراغ) ثابت
{ أرجع vpiConstant; }

صحيح __vpiDecConst::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpiConstType:
	    أرجع vpiDecConst;

	  حالة vpiSigned:
	    أرجع 1;

	  حالة vpi_اندازه:
	    أرجع 32;

          حالة vpiAutomatic:
	    أرجع 0;

#إذا defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          حالة _vpiFromThr:
	      أرجع _vpiNoThr;
#پایان_اگر

	  افتراضي:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiDecConst\n", code);
	    أكد(0);
	    أرجع 0;
      }
}


فراغ __vpiDecConst::vpi_get_مقدار(p_vpi_مقدار vp)
{
      ساكن ثابت size_t RBUF_USE_اندازه = 64 + 1;
      حرف*rbuf = static_cast<حرف *>(need_نتیجه_buf(RBUF_USE_اندازه, RBUF_VAL));
      حرف*cp = rbuf;

      بدل (vp->format) {

	  حالة vpiObjTypeVal:
	  حالة vpiIntVal: {
		vp->value.integer = value;
		اكسر;
	  }

          حالة vpiDecStrVal:
	      snprintf(rbuf, RBUF_USE_اندازه, "%d", value);
	      vp->value.str = rbuf;
	      اكسر;

          حالة vpiBinStrVal:
	      لكل(صحيح بیت=31; بیت>=0;بیت--){
		  *cp++ = "01"[ (static_cast<بیعلامت>(value)>>بیت)&1 ];
	      }
	      *cp = 0;

	      vp->value.str = rbuf;
	      اكسر;

          حالة vpiHexStrVal:
	      snprintf(rbuf, RBUF_USE_اندازه, "%08x", value);
	      vp->value.str = rbuf;
	      اكسر;

          حالة vpiOctStrVal:
	      snprintf(rbuf, RBUF_USE_اندازه, "%011x", value);
	      vp->value.str = rbuf;
	      اكسر;

	  افتراضي:
	    fprintf(stderr, "vvp error (vpi_const.cc): format %d not supported "
		    "by vpiDecConst\n", (صحيح)vp->format);
	    vp->format = vpiSuppressVal;
	    اكسر;
      }
}


مضمن __vpiRealConst::__vpiRealConst(مزدوج val)
: value(val)
{ }

صحيح __vpiRealConst::get_type_کد(فراغ) ثابت
{ أرجع vpiConstant; }

صحيح __vpiRealConst::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpi_سطر_No:
	    أرجع 0;  // Not implemented for now!

	  حالة vpi_اندازه:
	    أرجع 1;

	  حالة vpiConstType:
	    أرجع vpiRealConst;

	  حالة vpiSigned:
	    أرجع 1;

          حالة vpiAutomatic:
	    أرجع 0;

#إذا defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          حالة _vpiFromThr:
	      أرجع _vpiNoThr;
#پایان_اگر

	  افتراضي:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiRealConst\n", code);
	    أكد(0);
	    أرجع 0;
      }
}


فراغ __vpiRealConst::vpi_get_مقدار(p_vpi_مقدار val)
{
      vpip_real_get_مقدار(value, val);
}


vpiHandle vpip_make_real_const(مزدوج value)
{
      __vpiRealConst*obj = جديد __vpiRealConst(value);
      أرجع obj;
}

بنية __vpiRealParam  : عام __vpiRealConst {
      __vpiRealParam(__vpiScope* scope, مزدوج val, ثابت حرف*name,
                     منطقي local_flag, دراز file_idx, دراز lineno);
      ~__vpiRealParam() override;
      صحيح get_type_کد(فراغ) ثابت override;
      صحيح vpi_get(صحيح code) override;
      حرف*vpi_get_str(صحيح code) override;
      vpiHandle vpi_handle(صحيح code) override;

    خاص:
      __vpiScope* scope_;
      ثابت حرف*basename_;
      منطقي local_flag_;
      بیعلامت file_idx_;
      بیعلامت lineno_;
};


مضمن __vpiRealParam::__vpiRealParam(__vpiScope* scope, مزدوج val, ثابت حرف*name,
                                      منطقي local_flag, دراز file_idx, دراز lineno)
: __vpiRealConst(val)
{
      scope_ = scope;
      basename_ = name;
      local_flag_ = local_flag;
      أكد(file_idx >= 0 && file_idx <= بیشترین_صحیح);
      file_idx_ = (بیعلامت) file_idx;
      أكد(lineno >= 0 && lineno <= بیشترین_صحیح);
      lineno_ = (بیعلامت) lineno;
}

__vpiRealParam::~__vpiRealParam()
{
      حذف[]basename_;
}


صحيح __vpiRealParam::get_type_کد(فراغ) ثابت
{ أرجع vpiParameter; }

صحيح __vpiRealParam::vpi_get(صحيح code)
{
    بدل (code) {
      حالة vpi_سطر_No :
        أرجع lineno_;

      حالة vpiLocalParam :
        أرجع local_flag_;

      افتراضي :
           أرجع __vpiRealConst::vpi_get(code);
    }
}

حرف* __vpiRealParam::vpi_get_str(صحيح code)
{
      إذا (code == vpiFile)
            أرجع simple_مجموعه_rbuf_str(file_نامها[file_idx_]);

      أرجع generic_get_str(code, scope_, basename_, عدم);
}

vpiHandle __vpiRealParam::vpi_handle(صحيح code)
{
      بدل (code) {
          حالة vpiScope:
            أرجع scope_;

	  حالة vpiModule:
	    أرجع vpip_module(scope_);

          افتراضي:
            أرجع 0;
      }
}


vpiHandle vpip_make_real_param(ثابت حرف*name, مزدوج value,
                               منطقي local_flag, دراز file_idx, دراز lineno)
{
      بنية __vpiRealParam*obj = جديد __vpiRealParam(vpip_peek_current_scope(),
                                                     value, name, local_flag,
                                                     file_idx, lineno);
      أرجع obj;
}

/*
 * Make a VPI null constant
 */
مضمن __vpiNullConst::__vpiNullConst()
{ }

صحيح __vpiNullConst::get_type_کد(فراغ) ثابت
{ أرجع vpiConstant; }

صحيح __vpiNullConst::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpi_سطر_No:
	    أرجع 0;  // Not implemented for now!

	  حالة vpi_اندازه:
	    أرجع 32;

	  حالة vpiConstType:
	    أرجع vpiNullConst;

	  حالة vpiSigned:
	    أرجع 0;

          حالة vpiAutomatic:
	    أرجع 0;

#إذا defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          حالة _vpiFromThr:
	      أرجع _vpiNoThr;
#پایان_اگر

	  افتراضي:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiNullConst\n", code);
	    أكد(0);
	    أرجع 0;
      }
}

فراغ __vpiNullConst::vpi_get_مقدار(p_vpi_مقدار val)
{
      ساكن ثابت size_t RBUF_USE_اندازه = 64 + 1;
      حرف*rbuf = static_cast<حرف *>(need_نتیجه_buf(RBUF_USE_اندازه, RBUF_VAL));

      بدل (val->format) {

	  حالة vpiObjTypeVal:
	    val->format = vpiStringVal;
	    // fall through
	  حالة vpiBinStrVal:
	  حالة vpiDecStrVal:
	  حالة vpiOctStrVal:
	  حالة vpiHexStrVal:
	  حالة vpiStringVal:
	    snprintf(rbuf, RBUF_USE_اندازه, "null");
	    val->value.str = rbuf;
	    اكسر;

          حالة vpiScalarVal:
	    val->value.scalar = vpi0;
	    اكسر;

	  حالة vpiIntVal:
	    val->value.integer = 0;
	    اكسر;

	  حالة vpi_وکتور_Val:
	    val->value.vector = static_cast<p_vpi_vecval>
	                        (need_نتیجه_buf(2*حجم(s_vpi_vecval),
	                                         RBUF_VAL));
	    لكل (بیعلامت idx = 0; idx < 2; idx += 1) {
		  val->value.vector[idx].aval = 0;
		  val->value.vector[idx].bval = 0;
	    }
	    اكسر;

	  حالة vpiRealVal:
	    val->value.real = 0.0;
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "vvp error: format %d not supported "
		    "by vpiNullConst\n", (صحيح)val->format);
	    val->format = vpiSuppressVal;
	    اكسر;
      }
}

vpiHandle vpip_make_null_const()
{
      __vpiNullConst*obj = جديد __vpiNullConst;
      أرجع obj;
}

#اگر_تعریف_شده CHECK_WITH_VALGRIND
فراغ constant_delete(vpiHandle قلم)
{
      أكد(قلم->get_type_کد() == vpiConstant);
      بدل(vpi_get(vpiConstType, قلم)) {
	  حالة vpiStringConst:
	    حذف dynamic_cast<__vpiStringConst*>(قلم);
	    اكسر;
	  حالة vpiDecConst:
	    حذف dynamic_cast<__vpiDecConst*>(قلم);
	    اكسر;
	  حالة vpi_دودویی_Const:
	    حذف dynamic_cast<__vpiBinaryConst*>(قلم);
	    اكسر;
	  حالة vpiRealConst:
	    حذف dynamic_cast<__vpiRealConst*>(قلم);
	    اكسر;
	  حالة vpiNullConst:
	    حذف dynamic_cast<__vpiNullConst*>(قلم);
	    اكسر;
	  افتراضي:
	    أكد(0);
      }
}

فراغ parameter_delete(vpiHandle قلم)
{
      بدل(vpi_get(vpiConstType, قلم)) {
	  حالة vpiStringConst: {
	    صنف __vpiStringParam*rfp = dynamic_cast<__vpiStringParam*>(قلم);
	    حذف rfp;
	    اكسر; }
	  حالة vpi_دودویی_Const: {
	    بنية __vpiBinaryParam*rfp = dynamic_cast<__vpiBinaryParam*>(قلم);
	    حذف rfp;
	    اكسر; }
	  حالة vpiRealConst: {
	    بنية __vpiRealParam*rfp = dynamic_cast<__vpiRealParam*>(قلم);
	    حذف rfp;
	    اكسر; }
	  افتراضي:
	    أكد(0);
      }
}
#پایان_اگر
