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
#جے_تعریف_شدہ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#جے_انت
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <climits>
# include  <cassert>
# include  "ivl_alloc.h"

صنف __vpiStringConst : عام __vpiHandle {
    عام:
      واضح __vpiStringConst(حرف*val);
      ~__vpiStringConst() override;
      صحيح get_type_کوڈ(فراغ) ثابت override;
      صحيح vpi_get(صحيح code) override;
      فراغ vpi_get_قدر(p_vpi_قدر val) override;

    خاص:
      فراغ process_string_();
    خاص:
      حرف*قدر_2;
      size_t قدر_len;
};

مضمن __vpiStringConst::__vpiStringConst(حرف*v)
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
فراغ __vpiStringConst::process_string_(فراغ)
{
      حرف*chr = قدر_2;
      حرف*dp = قدر_2;

      طالما (*chr) {
	    حرف اگلا_char = *chr;

	      /* Process octal escapes that I might find. */
	    إذا (*chr == '\\') {
		  لكل (صحيح idx = 1 ;  idx <= 3 ;  idx += 1) {
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
      مٹاؤ[] قدر_2;
}

صحيح __vpiStringConst::get_type_کوڈ(فراغ) ثابت
{ أرجع vpiConstant; }

صحيح __vpiStringConst::vpi_get(صحيح code)
{
      بدل (code) {
          حالة vpi_ناپ:
	    أرجع قدر_len * 8;

          حالة vpiSigned:
	      أرجع 0;

	  حالة vpiConstType:
	      أرجع vpiStringConst;

          حالة vpiAutomatic:
	      أرجع 0;

#إذا defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          حالة _vpiFromThr:
	      أرجع _vpiNoThr;
#جے_انت

	  افتراضي:
	      fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiStringConst\n", code);
	      أكد(0);
	      أرجع 0;
      }
}

فراغ __vpiStringConst::vpi_get_قدر(p_vpi_قدر vp)
{
      بنانشانی uint_قدر;
      p_vpi_vecval vecp;
      بنانشانی size = قدر_len;
      حرف*rbuf = 0;
      حرف*cp;

      بدل (vp->format) {
	  حالة vpiObjTypeVal:
	      /* String parameters by default have vpiStringVal values. */
	    vp->format = vpiStringVal;
	    // fallthrough
	  حالة vpiStringVal:
	    cp = قدر_2;
	    rbuf = static_cast<حرف *>(need_نتیجہ_buf(size + 1, RBUF_VAL));
	    vp->value.str = rbuf;

	    لكل (بنانشانی صحيح i = 0; i < size; i++) {
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
	    ساكن ثابت size_t RBUF_USE_ناپ = 12;
	    rbuf = static_cast<حرف *>(need_نتیجہ_buf(RBUF_USE_ناپ, RBUF_VAL));
	    uint_قدر = 0;
	    لكل(بنانشانی i=0; i<size; i += 1){
		  uint_قدر <<=8;
		  uint_قدر += (بنانشانی حرف)(قدر_2[i]);
	    }
	    snprintf(rbuf, RBUF_USE_ناپ, "%u", uint_قدر);
	    vp->value.str = rbuf;
	    اكسر;

          حالة vpiBinStrVal:
	      rbuf = static_cast<حرف *>(need_نتیجہ_buf(8 * size + 1, RBUF_VAL));
	      cp = rbuf;
	      لكل(بنانشانی i=0; i<size; i += 1){
		  لكل(صحيح ثنائی_ہندسہ=7; ثنائی_ہندسہ>=0; ثنائی_ہندسہ -= 1){
		      *cp++ = "01"[ (قدر_2[i]>>ثنائی_ہندسہ)&1 ];
		  }
	      }
	      *cp = 0;
	      vp->value.str = rbuf;
	      اكسر;

          حالة vpiHexStrVal:
	      rbuf = static_cast<حرف *>(need_نتیجہ_buf(2 * size + 1, RBUF_VAL));
	      cp = rbuf;
	      لكل(بنانشانی i=0; i<size; i += 1){
		  لكل(صحيح nibble=1; nibble>=0; nibble -= 1){
		      *cp++ = "0123456789abcdef"[ (قدر_2[i]>>(nibble*4))&15 ];
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
	      لكل(بنانشانی i=0; i<size; i += 1){
		  لكل(صحيح ثنائی_ہندسہ=7; ثنائی_ہندسہ>=0; ثنائی_ہندسہ -= 1){
		      vp->value.integer <<= 1;
		      vp->value.integer += (قدر_2[i]>>ثنائی_ہندسہ)&1;
		  }
	      }
	      اكسر;

          حالة vpi_سمتیہ_Val:
              vp->value.vector = static_cast<p_vpi_vecval>
                                 (need_نتیجہ_buf((size+3)/4*
                                                   حجم(s_vpi_vecval),
                                                  RBUF_VAL));
              uint_قدر = 0;
              vecp = vp->value.vector;
              vecp->aval = vecp->bval = 0;
	      لكل(بنانشانی i=0; i<size; i += 1){
		  vecp->aval |= قدر_2[i] << uint_قدر*8;
		  uint_قدر += 1;
		  إذا (uint_قدر > 3) {
		      uint_قدر = 0;
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
      واضح مضمن __vpiStringConstTEMP(حرف*v) : __vpiStringConst(v) { }
      free_object_fun_t free_object_fun(فراغ) override;
};

ساكن صحيح free_عارضی_string(vpiHandle obj)
{
      بنية __vpiStringConstTEMP*rfp = dynamic_cast<__vpiStringConstTEMP*>(obj);
      مٹاؤ rfp;
      أرجع 1;
}

__vpiHandle::free_object_fun_t __vpiStringConstTEMP::free_object_fun(فراغ)
{ أرجع &free_عارضی_string; }

vpiHandle vpip_make_string_const(حرف*متن, منطقي persistent_flag)
{
      __vpiStringConst*obj;

      obj = persistent_flag? جديد __vpiStringConst(متن) : جديد __vpiStringConstTEMP(متن);

      أرجع obj;
}


صنف __vpiStringParam  : عام __vpiStringConst {
    عام:
      __vpiStringParam(__vpiScope* scope, حرف*txt, ثابت حرف*name,
                       منطقي local_flag, لما file_idx, لما lineno);
      ~__vpiStringParam() override;
      صحيح get_type_کوڈ(فراغ) ثابت override;
      صحيح vpi_get(صحيح code) override;
      حرف*vpi_get_str(صحيح code) override;
      vpiHandle vpi_handle(صحيح code) override;

    خاص:
      __vpiScope* scope_;
      ثابت حرف*basename_;
      منطقي       local_flag_;
      بنانشانی   file_idx_;
      بنانشانی   lineno_;
};

مضمن __vpiStringParam::__vpiStringParam(__vpiScope* scope, حرف*txt, ثابت حرف*nam,
                                          منطقي local_flag, لما file_idx, لما lineno)
: __vpiStringConst(txt)
{
      scope_ = scope;
      basename_ = nam;
      local_flag_ = local_flag;
      أكد(file_idx >= 0 && file_idx <= ودھ_توں_ودھ_عدد);
      file_idx_ = (بنانشانی) file_idx;
      أكد(lineno >= 0 && lineno <= ودھ_توں_ودھ_عدد);
      lineno_ = (بنانشانی) lineno;
}

__vpiStringParam::~__vpiStringParam()
{
      مٹاؤ[]basename_;
}

صحيح __vpiStringParam::get_type_کوڈ(فراغ) ثابت
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
	    أرجع simple_سیٹ_rbuf_str(file_ناں[file_idx_]);
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
                                 منطقي local_flag, لما file_idx, لما lineno)
{
      __vpiStringParam*obj = جديد __vpiStringParam(vpip_peek_current_scope(), متن, name,
                                                  local_flag, file_idx, lineno);

      أرجع obj;
}



مضمن __vpiBinaryConst::__vpiBinaryConst()
: signed_flag(0), sized_flag(0)
{ }

صحيح __vpiBinaryConst::get_type_کوڈ(فراغ) ثابت
{ أرجع vpiConstant; }

صحيح __vpiBinaryConst::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpiConstType:
	    أرجع vpi_ثنائی_Const;

	  حالة vpi_سطر_No:
	    أرجع 0;  // Not implemented for now!

	  حالة vpiSigned:
	    أرجع signed_flag? 1 : 0;

	  حالة vpi_ناپ:
	    أرجع بٹ.size();

          حالة vpiAutomatic:
	    أرجع 0;

#إذا defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          حالة _vpiFromThr:
	      أرجع _vpiNoThr;
#جے_انت

	  افتراضي:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiBinaryConst\n", code);
	    أكد(0);
	    أرجع 0;
      }
}


فراغ __vpiBinaryConst::vpi_get_قدر(p_vpi_قدر val)
{
      بدل (val->format) {

	  حالة vpiObjTypeVal:
	  حالة vpiBinStrVal:
	  حالة vpiDecStrVal:
	  حالة vpiOctStrVal:
	  حالة vpiHexStrVal:
          حالة vpiScalarVal:
	  حالة vpiIntVal:
	  حالة vpi_سمتیہ_Val:
	  حالة vpiStringVal:
	  حالة vpiRealVal:
	    vpip_vec4_get_قدر(بٹ, بٹ.size(), signed_flag, val);
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
vpiHandle vpip_make_ثنائی_const(بنانشانی wid, ثابت حرف*بٹ)
{
      بنية __vpiBinaryConst*obj = جديد __vpiBinaryConst;

      ثابت حرف*bp = بٹ;
      إذا (*bp == 's') {
	    bp += 1;
	    obj->signed_flag = 1;
      }

      obj->بٹ = vector4_from_متن(bp, wid);

      أرجع obj;
}

vvp_vector4_t vector4_from_متن(ثابت حرف*بٹ, بنانشانی wid)
{
      vvp_vector4_t res (wid);

      لكل (بنانشانی idx = 0 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t val = BIT4_0;
	    بدل (بٹ[wid-idx-1]) {
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

	    res.سیٹ_ثنائی_ہندسہ(idx, val);
      }

      أرجع res;
}

بنية __vpiBinaryParam  : عام __vpiBinaryConst {
      __vpiBinaryParam(__vpiScope*scope, ثابت vvp_vector4_t&b, منطقي is_signed,
                       ثابت حرف*name, منطقي local_flag, لما file_idx, لما lineno);
      ~__vpiBinaryParam() override;
      صحيح get_type_کوڈ(فراغ) ثابت override;
      صحيح vpi_get(صحيح code) override;
      حرف*vpi_get_str(صحيح code) override;
      vpiHandle vpi_handle(صحيح code) override;

    خاص:
      __vpiScope*scope_;
      ثابت حرف*basename_;
      منطقي     local_flag_;
      بنانشانی file_idx_;
      بنانشانی lineno_;
};

مضمن __vpiBinaryParam::__vpiBinaryParam(__vpiScope*scope, ثابت vvp_vector4_t&b,
                                          منطقي is_signed, ثابت حرف*nam,
                                          منطقي local_flag, لما file_idx, لما lineno)
{
      scope_ = scope;
      بٹ = b;
      signed_flag = is_signed ? 1 : 0;
      sized_flag = 0;
      basename_ = nam;
      local_flag_ = local_flag;
      أكد(file_idx >= 0 && file_idx <= ودھ_توں_ودھ_عدد);
      file_idx_ = (بنانشانی) file_idx;
      أكد(lineno >= 0 && lineno <= ودھ_توں_ودھ_عدد);
      lineno_ = (بنانشانی) lineno;
}

__vpiBinaryParam::~__vpiBinaryParam()
{
      مٹاؤ[]basename_;
}

صحيح __vpiBinaryParam::get_type_کوڈ(فراغ) ثابت
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
	    أرجع simple_سیٹ_rbuf_str(file_ناں[file_idx_]);

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


vpiHandle vpip_make_ثنائی_param(ثابت حرف*name, ثابت vvp_vector4_t&بٹ,
				 منطقي signed_flag, منطقي local_flag,
				 لما file_idx, لما lineno)
{
      بنية __vpiBinaryParam*obj = جديد __vpiBinaryParam(vpip_peek_current_scope(),
                                                         بٹ, signed_flag, name,
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


صحيح __vpiDecConst::get_type_کوڈ(فراغ) ثابت
{ أرجع vpiConstant; }

صحيح __vpiDecConst::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpiConstType:
	    أرجع vpiDecConst;

	  حالة vpiSigned:
	    أرجع 1;

	  حالة vpi_ناپ:
	    أرجع 32;

          حالة vpiAutomatic:
	    أرجع 0;

#إذا defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          حالة _vpiFromThr:
	      أرجع _vpiNoThr;
#جے_انت

	  افتراضي:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiDecConst\n", code);
	    أكد(0);
	    أرجع 0;
      }
}


فراغ __vpiDecConst::vpi_get_قدر(p_vpi_قدر vp)
{
      ساكن ثابت size_t RBUF_USE_ناپ = 64 + 1;
      حرف*rbuf = static_cast<حرف *>(need_نتیجہ_buf(RBUF_USE_ناپ, RBUF_VAL));
      حرف*cp = rbuf;

      بدل (vp->format) {

	  حالة vpiObjTypeVal:
	  حالة vpiIntVal: {
		vp->value.integer = value;
		اكسر;
	  }

          حالة vpiDecStrVal:
	      snprintf(rbuf, RBUF_USE_ناپ, "%d", value);
	      vp->value.str = rbuf;
	      اكسر;

          حالة vpiBinStrVal:
	      لكل(صحيح ثنائی_ہندسہ=31; ثنائی_ہندسہ>=0;ثنائی_ہندسہ--){
		  *cp++ = "01"[ (static_cast<بنانشانی>(value)>>ثنائی_ہندسہ)&1 ];
	      }
	      *cp = 0;

	      vp->value.str = rbuf;
	      اكسر;

          حالة vpiHexStrVal:
	      snprintf(rbuf, RBUF_USE_ناپ, "%08x", value);
	      vp->value.str = rbuf;
	      اكسر;

          حالة vpiOctStrVal:
	      snprintf(rbuf, RBUF_USE_ناپ, "%011x", value);
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

صحيح __vpiRealConst::get_type_کوڈ(فراغ) ثابت
{ أرجع vpiConstant; }

صحيح __vpiRealConst::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpi_سطر_No:
	    أرجع 0;  // Not implemented for now!

	  حالة vpi_ناپ:
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
#جے_انت

	  افتراضي:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiRealConst\n", code);
	    أكد(0);
	    أرجع 0;
      }
}


فراغ __vpiRealConst::vpi_get_قدر(p_vpi_قدر val)
{
      vpip_real_get_قدر(value, val);
}


vpiHandle vpip_make_real_const(مزدوج value)
{
      __vpiRealConst*obj = جديد __vpiRealConst(value);
      أرجع obj;
}

بنية __vpiRealParam  : عام __vpiRealConst {
      __vpiRealParam(__vpiScope* scope, مزدوج val, ثابت حرف*name,
                     منطقي local_flag, لما file_idx, لما lineno);
      ~__vpiRealParam() override;
      صحيح get_type_کوڈ(فراغ) ثابت override;
      صحيح vpi_get(صحيح code) override;
      حرف*vpi_get_str(صحيح code) override;
      vpiHandle vpi_handle(صحيح code) override;

    خاص:
      __vpiScope* scope_;
      ثابت حرف*basename_;
      منطقي local_flag_;
      بنانشانی file_idx_;
      بنانشانی lineno_;
};


مضمن __vpiRealParam::__vpiRealParam(__vpiScope* scope, مزدوج val, ثابت حرف*name,
                                      منطقي local_flag, لما file_idx, لما lineno)
: __vpiRealConst(val)
{
      scope_ = scope;
      basename_ = name;
      local_flag_ = local_flag;
      أكد(file_idx >= 0 && file_idx <= ودھ_توں_ودھ_عدد);
      file_idx_ = (بنانشانی) file_idx;
      أكد(lineno >= 0 && lineno <= ودھ_توں_ودھ_عدد);
      lineno_ = (بنانشانی) lineno;
}

__vpiRealParam::~__vpiRealParam()
{
      مٹاؤ[]basename_;
}


صحيح __vpiRealParam::get_type_کوڈ(فراغ) ثابت
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
            أرجع simple_سیٹ_rbuf_str(file_ناں[file_idx_]);

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
                               منطقي local_flag, لما file_idx, لما lineno)
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

صحيح __vpiNullConst::get_type_کوڈ(فراغ) ثابت
{ أرجع vpiConstant; }

صحيح __vpiNullConst::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpi_سطر_No:
	    أرجع 0;  // Not implemented for now!

	  حالة vpi_ناپ:
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
#جے_انت

	  افتراضي:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiNullConst\n", code);
	    أكد(0);
	    أرجع 0;
      }
}

فراغ __vpiNullConst::vpi_get_قدر(p_vpi_قدر val)
{
      ساكن ثابت size_t RBUF_USE_ناپ = 64 + 1;
      حرف*rbuf = static_cast<حرف *>(need_نتیجہ_buf(RBUF_USE_ناپ, RBUF_VAL));

      بدل (val->format) {

	  حالة vpiObjTypeVal:
	    val->format = vpiStringVal;
	    // fall through
	  حالة vpiBinStrVal:
	  حالة vpiDecStrVal:
	  حالة vpiOctStrVal:
	  حالة vpiHexStrVal:
	  حالة vpiStringVal:
	    snprintf(rbuf, RBUF_USE_ناپ, "null");
	    val->value.str = rbuf;
	    اكسر;

          حالة vpiScalarVal:
	    val->value.scalar = vpi0;
	    اكسر;

	  حالة vpiIntVal:
	    val->value.integer = 0;
	    اكسر;

	  حالة vpi_سمتیہ_Val:
	    val->value.vector = static_cast<p_vpi_vecval>
	                        (need_نتیجہ_buf(2*حجم(s_vpi_vecval),
	                                         RBUF_VAL));
	    لكل (بنانشانی idx = 0; idx < 2; idx += 1) {
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

#جے_تعریف_شدہ CHECK_WITH_VALGRIND
فراغ constant_delete(vpiHandle شے)
{
      أكد(شے->get_type_کوڈ() == vpiConstant);
      بدل(vpi_get(vpiConstType, شے)) {
	  حالة vpiStringConst:
	    مٹاؤ dynamic_cast<__vpiStringConst*>(شے);
	    اكسر;
	  حالة vpiDecConst:
	    مٹاؤ dynamic_cast<__vpiDecConst*>(شے);
	    اكسر;
	  حالة vpi_ثنائی_Const:
	    مٹاؤ dynamic_cast<__vpiBinaryConst*>(شے);
	    اكسر;
	  حالة vpiRealConst:
	    مٹاؤ dynamic_cast<__vpiRealConst*>(شے);
	    اكسر;
	  حالة vpiNullConst:
	    مٹاؤ dynamic_cast<__vpiNullConst*>(شے);
	    اكسر;
	  افتراضي:
	    أكد(0);
      }
}

فراغ parameter_delete(vpiHandle شے)
{
      بدل(vpi_get(vpiConstType, شے)) {
	  حالة vpiStringConst: {
	    صنف __vpiStringParam*rfp = dynamic_cast<__vpiStringParam*>(شے);
	    مٹاؤ rfp;
	    اكسر; }
	  حالة vpi_ثنائی_Const: {
	    بنية __vpiBinaryParam*rfp = dynamic_cast<__vpiBinaryParam*>(شے);
	    مٹاؤ rfp;
	    اكسر; }
	  حالة vpiRealConst: {
	    بنية __vpiRealParam*rfp = dynamic_cast<__vpiRealParam*>(شے);
	    مٹاؤ rfp;
	    اكسر; }
	  افتراضي:
	    أكد(0);
      }
}
#جے_انت
