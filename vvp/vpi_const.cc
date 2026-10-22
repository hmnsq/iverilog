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
#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#جيڪڏهن_پورو
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
      صحيح get_type_ڪوڊ(فراغ) ثابت override;
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
	    حرف ايندڙ_char = *chr;

	      /* Process octal escapes that I might find. */
	    إذا (*chr == '\\') {
		  لكل (صحيح idx = 1 ;  idx <= 3 ;  idx += 1) {
			أكد(chr[idx] != 0);
			أكد(chr[idx] < '8');
			أكد(chr[idx] >= '0');
			ايندڙ_char = ايندڙ_char*8 + chr[idx] - '0';
		  }
		  chr += 3;
	    }
	    *dp++ = ايندڙ_char;
	    chr += 1;
      }
      *dp = 0;
      قدر_len = dp - قدر_2;
}

__vpiStringConst::~__vpiStringConst()
{
      ميٽيو[] قدر_2;
}

صحيح __vpiStringConst::get_type_ڪوڊ(فراغ) ثابت
{ أرجع vpiConstant; }

صحيح __vpiStringConst::vpi_get(صحيح code)
{
      بدل (code) {
          حالة vpi_سائيز:
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
#جيڪڏهن_پورو

	  افتراضي:
	      fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiStringConst\n", code);
	      أكد(0);
	      أرجع 0;
      }
}

فراغ __vpiStringConst::vpi_get_قدر(p_vpi_قدر vp)
{
      بنانشان uint_قدر;
      p_vpi_vecval vecp;
      بنانشان size = قدر_len;
      حرف*rbuf = 0;
      حرف*cp;

      بدل (vp->format) {
	  حالة vpiObjTypeVal:
	      /* String parameters by default have vpiStringVal values. */
	    vp->format = vpiStringVal;
	    // fallthrough
	  حالة vpiStringVal:
	    cp = قدر_2;
	    rbuf = static_cast<حرف *>(need_نتيجو_buf(size + 1, RBUF_VAL));
	    vp->value.str = rbuf;

	    لكل (بنانشان صحيح i = 0; i < size; i++) {
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
	    ساكن ثابت size_t RBUF_USE_سائيز = 12;
	    rbuf = static_cast<حرف *>(need_نتيجو_buf(RBUF_USE_سائيز, RBUF_VAL));
	    uint_قدر = 0;
	    لكل(بنانشان i=0; i<size; i += 1){
		  uint_قدر <<=8;
		  uint_قدر += (بنانشان حرف)(قدر_2[i]);
	    }
	    snprintf(rbuf, RBUF_USE_سائيز, "%u", uint_قدر);
	    vp->value.str = rbuf;
	    اكسر;

          حالة vpiBinStrVal:
	      rbuf = static_cast<حرف *>(need_نتيجو_buf(8 * size + 1, RBUF_VAL));
	      cp = rbuf;
	      لكل(بنانشان i=0; i<size; i += 1){
		  لكل(صحيح ٻٽو_انگ=7; ٻٽو_انگ>=0; ٻٽو_انگ -= 1){
		      *cp++ = "01"[ (قدر_2[i]>>ٻٽو_انگ)&1 ];
		  }
	      }
	      *cp = 0;
	      vp->value.str = rbuf;
	      اكسر;

          حالة vpiHexStrVal:
	      rbuf = static_cast<حرف *>(need_نتيجو_buf(2 * size + 1, RBUF_VAL));
	      cp = rbuf;
	      لكل(بنانشان i=0; i<size; i += 1){
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
	      لكل(بنانشان i=0; i<size; i += 1){
		  لكل(صحيح ٻٽو_انگ=7; ٻٽو_انگ>=0; ٻٽو_انگ -= 1){
		      vp->value.integer <<= 1;
		      vp->value.integer += (قدر_2[i]>>ٻٽو_انگ)&1;
		  }
	      }
	      اكسر;

          حالة vpi_ويڪٽر_Val:
              vp->value.vector = static_cast<p_vpi_vecval>
                                 (need_نتيجو_buf((size+3)/4*
                                                   حجم(s_vpi_vecval),
                                                  RBUF_VAL));
              uint_قدر = 0;
              vecp = vp->value.vector;
              vecp->aval = vecp->bval = 0;
	      لكل(بنانشان i=0; i<size; i += 1){
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

ساكن صحيح free_عارضي_string(vpiHandle obj)
{
      بنية __vpiStringConstTEMP*rfp = dynamic_cast<__vpiStringConstTEMP*>(obj);
      ميٽيو rfp;
      أرجع 1;
}

__vpiHandle::free_object_fun_t __vpiStringConstTEMP::free_object_fun(فراغ)
{ أرجع &free_عارضي_string; }

vpiHandle vpip_make_string_const(حرف*متن, منطقي persistent_flag)
{
      __vpiStringConst*obj;

      obj = persistent_flag? جديد __vpiStringConst(متن) : جديد __vpiStringConstTEMP(متن);

      أرجع obj;
}


صنف __vpiStringParam  : عام __vpiStringConst {
    عام:
      __vpiStringParam(__vpiScope* scope, حرف*txt, ثابت حرف*name,
                       منطقي local_flag, ڊگهو file_idx, ڊگهو lineno);
      ~__vpiStringParam() override;
      صحيح get_type_ڪوڊ(فراغ) ثابت override;
      صحيح vpi_get(صحيح code) override;
      حرف*vpi_get_str(صحيح code) override;
      vpiHandle vpi_handle(صحيح code) override;

    خاص:
      __vpiScope* scope_;
      ثابت حرف*basename_;
      منطقي       local_flag_;
      بنانشان   file_idx_;
      بنانشان   lineno_;
};

مضمن __vpiStringParam::__vpiStringParam(__vpiScope* scope, حرف*txt, ثابت حرف*nam,
                                          منطقي local_flag, ڊگهو file_idx, ڊگهو lineno)
: __vpiStringConst(txt)
{
      scope_ = scope;
      basename_ = nam;
      local_flag_ = local_flag;
      أكد(file_idx >= 0 && file_idx <= وڌ_کان_وڌ_عدد);
      file_idx_ = (بنانشان) file_idx;
      أكد(lineno >= 0 && lineno <= وڌ_کان_وڌ_عدد);
      lineno_ = (بنانشان) lineno;
}

__vpiStringParam::~__vpiStringParam()
{
      ميٽيو[]basename_;
}

صحيح __vpiStringParam::get_type_ڪوڊ(فراغ) ثابت
{ أرجع vpiParameter; }

صحيح __vpiStringParam::vpi_get(صحيح code)
{
    بدل (code) {
       حالة vpi_سٽ_No :
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
	    أرجع simple_سيٽ_rbuf_str(file_نالا[file_idx_]);
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
                                 منطقي local_flag, ڊگهو file_idx, ڊگهو lineno)
{
      __vpiStringParam*obj = جديد __vpiStringParam(vpip_peek_current_scope(), متن, name,
                                                  local_flag, file_idx, lineno);

      أرجع obj;
}



مضمن __vpiBinaryConst::__vpiBinaryConst()
: signed_flag(0), sized_flag(0)
{ }

صحيح __vpiBinaryConst::get_type_ڪوڊ(فراغ) ثابت
{ أرجع vpiConstant; }

صحيح __vpiBinaryConst::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpiConstType:
	    أرجع vpi_ٻٽو_Const;

	  حالة vpi_سٽ_No:
	    أرجع 0;  // Not implemented for now!

	  حالة vpiSigned:
	    أرجع signed_flag? 1 : 0;

	  حالة vpi_سائيز:
	    أرجع بٽ.size();

          حالة vpiAutomatic:
	    أرجع 0;

#إذا defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          حالة _vpiFromThr:
	      أرجع _vpiNoThr;
#جيڪڏهن_پورو

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
	  حالة vpi_ويڪٽر_Val:
	  حالة vpiStringVal:
	  حالة vpiRealVal:
	    vpip_vec4_get_قدر(بٽ, بٽ.size(), signed_flag, val);
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
vpiHandle vpip_make_ٻٽو_const(بنانشان wid, ثابت حرف*بٽ)
{
      بنية __vpiBinaryConst*obj = جديد __vpiBinaryConst;

      ثابت حرف*bp = بٽ;
      إذا (*bp == 's') {
	    bp += 1;
	    obj->signed_flag = 1;
      }

      obj->بٽ = vector4_from_متن(bp, wid);

      أرجع obj;
}

vvp_vector4_t vector4_from_متن(ثابت حرف*بٽ, بنانشان wid)
{
      vvp_vector4_t res (wid);

      لكل (بنانشان idx = 0 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t val = BIT4_0;
	    بدل (بٽ[wid-idx-1]) {
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

	    res.سيٽ_ٻٽو_انگ(idx, val);
      }

      أرجع res;
}

بنية __vpiBinaryParam  : عام __vpiBinaryConst {
      __vpiBinaryParam(__vpiScope*scope, ثابت vvp_vector4_t&b, منطقي is_signed,
                       ثابت حرف*name, منطقي local_flag, ڊگهو file_idx, ڊگهو lineno);
      ~__vpiBinaryParam() override;
      صحيح get_type_ڪوڊ(فراغ) ثابت override;
      صحيح vpi_get(صحيح code) override;
      حرف*vpi_get_str(صحيح code) override;
      vpiHandle vpi_handle(صحيح code) override;

    خاص:
      __vpiScope*scope_;
      ثابت حرف*basename_;
      منطقي     local_flag_;
      بنانشان file_idx_;
      بنانشان lineno_;
};

مضمن __vpiBinaryParam::__vpiBinaryParam(__vpiScope*scope, ثابت vvp_vector4_t&b,
                                          منطقي is_signed, ثابت حرف*nam,
                                          منطقي local_flag, ڊگهو file_idx, ڊگهو lineno)
{
      scope_ = scope;
      بٽ = b;
      signed_flag = is_signed ? 1 : 0;
      sized_flag = 0;
      basename_ = nam;
      local_flag_ = local_flag;
      أكد(file_idx >= 0 && file_idx <= وڌ_کان_وڌ_عدد);
      file_idx_ = (بنانشان) file_idx;
      أكد(lineno >= 0 && lineno <= وڌ_کان_وڌ_عدد);
      lineno_ = (بنانشان) lineno;
}

__vpiBinaryParam::~__vpiBinaryParam()
{
      ميٽيو[]basename_;
}

صحيح __vpiBinaryParam::get_type_ڪوڊ(فراغ) ثابت
{ أرجع vpiParameter; }

صحيح __vpiBinaryParam::vpi_get(صحيح code)
{
    بدل (code) {
      حالة vpi_سٽ_No :
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
	    أرجع simple_سيٽ_rbuf_str(file_نالا[file_idx_]);

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


vpiHandle vpip_make_ٻٽو_param(ثابت حرف*name, ثابت vvp_vector4_t&بٽ,
				 منطقي signed_flag, منطقي local_flag,
				 ڊگهو file_idx, ڊگهو lineno)
{
      بنية __vpiBinaryParam*obj = جديد __vpiBinaryParam(vpip_peek_current_scope(),
                                                         بٽ, signed_flag, name,
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


صحيح __vpiDecConst::get_type_ڪوڊ(فراغ) ثابت
{ أرجع vpiConstant; }

صحيح __vpiDecConst::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpiConstType:
	    أرجع vpiDecConst;

	  حالة vpiSigned:
	    أرجع 1;

	  حالة vpi_سائيز:
	    أرجع 32;

          حالة vpiAutomatic:
	    أرجع 0;

#إذا defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          حالة _vpiFromThr:
	      أرجع _vpiNoThr;
#جيڪڏهن_پورو

	  افتراضي:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiDecConst\n", code);
	    أكد(0);
	    أرجع 0;
      }
}


فراغ __vpiDecConst::vpi_get_قدر(p_vpi_قدر vp)
{
      ساكن ثابت size_t RBUF_USE_سائيز = 64 + 1;
      حرف*rbuf = static_cast<حرف *>(need_نتيجو_buf(RBUF_USE_سائيز, RBUF_VAL));
      حرف*cp = rbuf;

      بدل (vp->format) {

	  حالة vpiObjTypeVal:
	  حالة vpiIntVal: {
		vp->value.integer = value;
		اكسر;
	  }

          حالة vpiDecStrVal:
	      snprintf(rbuf, RBUF_USE_سائيز, "%d", value);
	      vp->value.str = rbuf;
	      اكسر;

          حالة vpiBinStrVal:
	      لكل(صحيح ٻٽو_انگ=31; ٻٽو_انگ>=0;ٻٽو_انگ--){
		  *cp++ = "01"[ (static_cast<بنانشان>(value)>>ٻٽو_انگ)&1 ];
	      }
	      *cp = 0;

	      vp->value.str = rbuf;
	      اكسر;

          حالة vpiHexStrVal:
	      snprintf(rbuf, RBUF_USE_سائيز, "%08x", value);
	      vp->value.str = rbuf;
	      اكسر;

          حالة vpiOctStrVal:
	      snprintf(rbuf, RBUF_USE_سائيز, "%011x", value);
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

صحيح __vpiRealConst::get_type_ڪوڊ(فراغ) ثابت
{ أرجع vpiConstant; }

صحيح __vpiRealConst::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpi_سٽ_No:
	    أرجع 0;  // Not implemented for now!

	  حالة vpi_سائيز:
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
#جيڪڏهن_پورو

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
                     منطقي local_flag, ڊگهو file_idx, ڊگهو lineno);
      ~__vpiRealParam() override;
      صحيح get_type_ڪوڊ(فراغ) ثابت override;
      صحيح vpi_get(صحيح code) override;
      حرف*vpi_get_str(صحيح code) override;
      vpiHandle vpi_handle(صحيح code) override;

    خاص:
      __vpiScope* scope_;
      ثابت حرف*basename_;
      منطقي local_flag_;
      بنانشان file_idx_;
      بنانشان lineno_;
};


مضمن __vpiRealParam::__vpiRealParam(__vpiScope* scope, مزدوج val, ثابت حرف*name,
                                      منطقي local_flag, ڊگهو file_idx, ڊگهو lineno)
: __vpiRealConst(val)
{
      scope_ = scope;
      basename_ = name;
      local_flag_ = local_flag;
      أكد(file_idx >= 0 && file_idx <= وڌ_کان_وڌ_عدد);
      file_idx_ = (بنانشان) file_idx;
      أكد(lineno >= 0 && lineno <= وڌ_کان_وڌ_عدد);
      lineno_ = (بنانشان) lineno;
}

__vpiRealParam::~__vpiRealParam()
{
      ميٽيو[]basename_;
}


صحيح __vpiRealParam::get_type_ڪوڊ(فراغ) ثابت
{ أرجع vpiParameter; }

صحيح __vpiRealParam::vpi_get(صحيح code)
{
    بدل (code) {
      حالة vpi_سٽ_No :
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
            أرجع simple_سيٽ_rbuf_str(file_نالا[file_idx_]);

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
                               منطقي local_flag, ڊگهو file_idx, ڊگهو lineno)
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

صحيح __vpiNullConst::get_type_ڪوڊ(فراغ) ثابت
{ أرجع vpiConstant; }

صحيح __vpiNullConst::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpi_سٽ_No:
	    أرجع 0;  // Not implemented for now!

	  حالة vpi_سائيز:
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
#جيڪڏهن_پورو

	  افتراضي:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiNullConst\n", code);
	    أكد(0);
	    أرجع 0;
      }
}

فراغ __vpiNullConst::vpi_get_قدر(p_vpi_قدر val)
{
      ساكن ثابت size_t RBUF_USE_سائيز = 64 + 1;
      حرف*rbuf = static_cast<حرف *>(need_نتيجو_buf(RBUF_USE_سائيز, RBUF_VAL));

      بدل (val->format) {

	  حالة vpiObjTypeVal:
	    val->format = vpiStringVal;
	    // fall through
	  حالة vpiBinStrVal:
	  حالة vpiDecStrVal:
	  حالة vpiOctStrVal:
	  حالة vpiHexStrVal:
	  حالة vpiStringVal:
	    snprintf(rbuf, RBUF_USE_سائيز, "null");
	    val->value.str = rbuf;
	    اكسر;

          حالة vpiScalarVal:
	    val->value.scalar = vpi0;
	    اكسر;

	  حالة vpiIntVal:
	    val->value.integer = 0;
	    اكسر;

	  حالة vpi_ويڪٽر_Val:
	    val->value.vector = static_cast<p_vpi_vecval>
	                        (need_نتيجو_buf(2*حجم(s_vpi_vecval),
	                                         RBUF_VAL));
	    لكل (بنانشان idx = 0; idx < 2; idx += 1) {
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

#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
فراغ constant_delete(vpiHandle شي)
{
      أكد(شي->get_type_ڪوڊ() == vpiConstant);
      بدل(vpi_get(vpiConstType, شي)) {
	  حالة vpiStringConst:
	    ميٽيو dynamic_cast<__vpiStringConst*>(شي);
	    اكسر;
	  حالة vpiDecConst:
	    ميٽيو dynamic_cast<__vpiDecConst*>(شي);
	    اكسر;
	  حالة vpi_ٻٽو_Const:
	    ميٽيو dynamic_cast<__vpiBinaryConst*>(شي);
	    اكسر;
	  حالة vpiRealConst:
	    ميٽيو dynamic_cast<__vpiRealConst*>(شي);
	    اكسر;
	  حالة vpiNullConst:
	    ميٽيو dynamic_cast<__vpiNullConst*>(شي);
	    اكسر;
	  افتراضي:
	    أكد(0);
      }
}

فراغ parameter_delete(vpiHandle شي)
{
      بدل(vpi_get(vpiConstType, شي)) {
	  حالة vpiStringConst: {
	    صنف __vpiStringParam*rfp = dynamic_cast<__vpiStringParam*>(شي);
	    ميٽيو rfp;
	    اكسر; }
	  حالة vpi_ٻٽو_Const: {
	    بنية __vpiBinaryParam*rfp = dynamic_cast<__vpiBinaryParam*>(شي);
	    ميٽيو rfp;
	    اكسر; }
	  حالة vpiRealConst: {
	    بنية __vpiRealParam*rfp = dynamic_cast<__vpiRealParam*>(شي);
	    ميٽيو rfp;
	    اكسر; }
	  افتراضي:
	    أكد(0);
      }
}
#جيڪڏهن_پورو
