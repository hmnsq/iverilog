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
#ifdef CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#endif
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <climits>
# include  <cassert>
# include  "ivl_alloc.h"

מחלקה __vpiStringConst : عام __vpiHandle {
    عام:
      explicit __vpiStringConst(حرف*val);
      ~__vpiStringConst() override;
      صحيح get_type_code(فراغ) ثابت override;
      صحيح vpi_get(صحيح code) override;
      فراغ vpi_get_value(p_vpi_value val) override;

    خاص:
      فراغ process_string_();
    خاص:
      حرف*value_;
      size_t value_len_;
};

مضمن __vpiStringConst::__vpiStringConst(حرف*v)
: value_(v)
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
      حرف*chr = value_;
      حرف*dp = value_;

      כלעוד (*chr) {
	    حرف next_char = *chr;

	      /* Process octal escapes that I might find. */
	    אם (*chr == '\\') {
		  עבור (صحيح idx = 1 ;  idx <= 3 ;  idx += 1) {
			أكد(chr[idx] != 0);
			أكد(chr[idx] < '8');
			أكد(chr[idx] >= '0');
			next_char = next_char*8 + chr[idx] - '0';
		  }
		  chr += 3;
	    }
	    *dp++ = next_char;
	    chr += 1;
      }
      *dp = 0;
      value_len_ = dp - value_;
}

__vpiStringConst::~__vpiStringConst()
{
      delete[] value_;
}

صحيح __vpiStringConst::get_type_code(فراغ) ثابت
{ החזר vpiConstant; }

صحيح __vpiStringConst::vpi_get(صحيح code)
{
      بدل (code) {
          حالة vpiSize:
	    החזר value_len_ * 8;

          حالة vpiSigned:
	      החזר 0;

	  حالة vpiConstType:
	      החזר vpiStringConst;

          حالة vpiAutomatic:
	      החזר 0;

#אם defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          حالة _vpiFromThr:
	      החזר _vpiNoThr;
#endif

	  افتراضي:
	      fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiStringConst\n", code);
	      أكد(0);
	      החזר 0;
      }
}

فراغ __vpiStringConst::vpi_get_value(p_vpi_value vp)
{
      unsigned uint_value;
      p_vpi_vecval vecp;
      unsigned size = value_len_;
      حرف*rbuf = 0;
      حرف*cp;

      بدل (vp->format) {
	  حالة vpiObjTypeVal:
	      /* String parameters by default have vpiStringVal values. */
	    vp->format = vpiStringVal;
	    // fallthrough
	  حالة vpiStringVal:
	    cp = value_;
	    rbuf = static_cast<حرف *>(need_result_buf(size + 1, RBUF_VAL));
	    vp->value.str = rbuf;

	    עבור (unsigned صحيح i = 0; i < size; i++) {
		  // Ignore leading null-bytes and replace other null-bytes with space.
		  // The LRM is not entirely clear on how null bytes should be handled.
		  // This is the implementation chosen for iverilog.
		  אם (*cp)
			*rbuf++ = *cp;
		  אחרת אם (rbuf != vp->value.str)
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
	    אם (size > 4){
		  // We only support standard integers. Ignore other bytes...
		  size = 4;
		  fprintf(stderr, "Warning (vpi_const.cc): %%d on constant strings only looks "
			  "at first 4 bytes!\n");
	    }
	    ساكن ثابت size_t RBUF_USE_SIZE = 12;
	    rbuf = static_cast<حرف *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));
	    uint_value = 0;
	    עבור(unsigned i=0; i<size; i += 1){
		  uint_value <<=8;
		  uint_value += (unsigned حرف)(value_[i]);
	    }
	    snprintf(rbuf, RBUF_USE_SIZE, "%u", uint_value);
	    vp->value.str = rbuf;
	    اكسر;

          حالة vpiBinStrVal:
	      rbuf = static_cast<حرف *>(need_result_buf(8 * size + 1, RBUF_VAL));
	      cp = rbuf;
	      עבור(unsigned i=0; i<size; i += 1){
		  עבור(صحيح bit=7; bit>=0; bit -= 1){
		      *cp++ = "01"[ (value_[i]>>bit)&1 ];
		  }
	      }
	      *cp = 0;
	      vp->value.str = rbuf;
	      اكسر;

          حالة vpiHexStrVal:
	      rbuf = static_cast<حرف *>(need_result_buf(2 * size + 1, RBUF_VAL));
	      cp = rbuf;
	      עבור(unsigned i=0; i<size; i += 1){
		  עבור(صحيح nibble=1; nibble>=0; nibble -= 1){
		      *cp++ = "0123456789abcdef"[ (value_[i]>>(nibble*4))&15 ];
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
	      עבור(unsigned i=0; i<size; i += 1){
		  עבור(صحيح bit=7; bit>=0; bit -= 1){
		      vp->value.integer <<= 1;
		      vp->value.integer += (value_[i]>>bit)&1;
		  }
	      }
	      اكسر;

          حالة vpiVectorVal:
              vp->value.vector = static_cast<p_vpi_vecval>
                                 (need_result_buf((size+3)/4*
                                                   حجم(s_vpi_vecval),
                                                  RBUF_VAL));
              uint_value = 0;
              vecp = vp->value.vector;
              vecp->aval = vecp->bval = 0;
	      עבור(unsigned i=0; i<size; i += 1){
		  vecp->aval |= value_[i] << uint_value*8;
		  uint_value += 1;
		  אם (uint_value > 3) {
		      uint_value = 0;
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
      explicit مضمن __vpiStringConstTEMP(حرف*v) : __vpiStringConst(v) { }
      free_object_fun_t free_object_fun(فراغ) override;
};

ساكن صحيح free_temp_string(vpiHandle obj)
{
      بنية __vpiStringConstTEMP*rfp = dynamic_cast<__vpiStringConstTEMP*>(obj);
      delete rfp;
      החזר 1;
}

__vpiHandle::free_object_fun_t __vpiStringConstTEMP::free_object_fun(فراغ)
{ החזר &free_temp_string; }

vpiHandle vpip_make_string_const(حرف*text, منطقي persistent_flag)
{
      __vpiStringConst*obj;

      obj = persistent_flag? جديد __vpiStringConst(text) : جديد __vpiStringConstTEMP(text);

      החזר obj;
}


מחלקה __vpiStringParam  : عام __vpiStringConst {
    عام:
      __vpiStringParam(__vpiScope* scope, حرف*txt, ثابت حرف*name,
                       منطقي local_flag, long file_idx, long lineno);
      ~__vpiStringParam() override;
      صحيح get_type_code(فراغ) ثابت override;
      صحيح vpi_get(صحيح code) override;
      حرف*vpi_get_str(صحيح code) override;
      vpiHandle vpi_handle(صحيح code) override;

    خاص:
      __vpiScope* scope_;
      ثابت حرف*basename_;
      منطقي       local_flag_;
      unsigned   file_idx_;
      unsigned   lineno_;
};

مضمن __vpiStringParam::__vpiStringParam(__vpiScope* scope, حرف*txt, ثابت حرف*nam,
                                          منطقي local_flag, long file_idx, long lineno)
: __vpiStringConst(txt)
{
      scope_ = scope;
      basename_ = nam;
      local_flag_ = local_flag;
      أكد(file_idx >= 0 && file_idx <= INT_MAX);
      file_idx_ = (unsigned) file_idx;
      أكد(lineno >= 0 && lineno <= INT_MAX);
      lineno_ = (unsigned) lineno;
}

__vpiStringParam::~__vpiStringParam()
{
      delete[]basename_;
}

صحيح __vpiStringParam::get_type_code(فراغ) ثابت
{ החזר vpiParameter; }

صحيح __vpiStringParam::vpi_get(صحيح code)
{
    بدل (code) {
       حالة vpiLineNo :
         החזר lineno_;

       حالة vpiLocalParam :
         החזר local_flag_;

       افتراضي :
         החזר __vpiStringConst::vpi_get(code);
    }
}


حرف*__vpiStringParam::vpi_get_str(صحيح code)
{
      אם (code == vpiFile) {
	    החזר simple_set_rbuf_str(file_names[file_idx_]);
      }

      החזר generic_get_str(code, scope_, basename_, عدم);
}


vpiHandle __vpiStringParam::vpi_handle(صحيح code)
{
      بدل (code) {
	  حالة vpiScope:
	    החזר scope_;

	  حالة vpiModule:
	    החזר vpip_module(scope_);

	  افتراضي:
	    החזר 0;
      }
}

vpiHandle vpip_make_string_param(ثابت حرف*name, حرف*text,
                                 منطقي local_flag, long file_idx, long lineno)
{
      __vpiStringParam*obj = جديد __vpiStringParam(vpip_peek_current_scope(), text, name,
                                                  local_flag, file_idx, lineno);

      החזר obj;
}



مضمن __vpiBinaryConst::__vpiBinaryConst()
: signed_flag(0), sized_flag(0)
{ }

صحيح __vpiBinaryConst::get_type_code(فراغ) ثابت
{ החזר vpiConstant; }

صحيح __vpiBinaryConst::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpiConstType:
	    החזר vpiBinaryConst;

	  حالة vpiLineNo:
	    החזר 0;  // Not implemented for now!

	  حالة vpiSigned:
	    החזר signed_flag? 1 : 0;

	  حالة vpiSize:
	    החזר bits.size();

          حالة vpiAutomatic:
	    החזר 0;

#אם defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          حالة _vpiFromThr:
	      החזר _vpiNoThr;
#endif

	  افتراضي:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiBinaryConst\n", code);
	    أكد(0);
	    החזר 0;
      }
}


فراغ __vpiBinaryConst::vpi_get_value(p_vpi_value val)
{
      بدل (val->format) {

	  حالة vpiObjTypeVal:
	  حالة vpiBinStrVal:
	  حالة vpiDecStrVal:
	  حالة vpiOctStrVal:
	  حالة vpiHexStrVal:
          حالة vpiScalarVal:
	  حالة vpiIntVal:
	  حالة vpiVectorVal:
	  حالة vpiStringVal:
	  حالة vpiRealVal:
	    vpip_vec4_get_value(bits, bits.size(), signed_flag, val);
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
vpiHandle vpip_make_binary_const(unsigned wid, ثابت حرف*bits)
{
      بنية __vpiBinaryConst*obj = جديد __vpiBinaryConst;

      ثابت حرف*bp = bits;
      אם (*bp == 's') {
	    bp += 1;
	    obj->signed_flag = 1;
      }

      obj->bits = vector4_from_text(bp, wid);

      החזר obj;
}

vvp_vector4_t vector4_from_text(ثابت حرف*bits, unsigned wid)
{
      vvp_vector4_t res (wid);

      עבור (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t val = BIT4_0;
	    بدل (bits[wid-idx-1]) {
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

	    res.set_bit(idx, val);
      }

      החזר res;
}

بنية __vpiBinaryParam  : عام __vpiBinaryConst {
      __vpiBinaryParam(__vpiScope*scope, ثابت vvp_vector4_t&b, منطقي is_signed,
                       ثابت حرف*name, منطقي local_flag, long file_idx, long lineno);
      ~__vpiBinaryParam() override;
      صحيح get_type_code(فراغ) ثابت override;
      صحيح vpi_get(صحيح code) override;
      حرف*vpi_get_str(صحيح code) override;
      vpiHandle vpi_handle(صحيح code) override;

    خاص:
      __vpiScope*scope_;
      ثابت حرف*basename_;
      منطقي     local_flag_;
      unsigned file_idx_;
      unsigned lineno_;
};

مضمن __vpiBinaryParam::__vpiBinaryParam(__vpiScope*scope, ثابت vvp_vector4_t&b,
                                          منطقي is_signed, ثابت حرف*nam,
                                          منطقي local_flag, long file_idx, long lineno)
{
      scope_ = scope;
      bits = b;
      signed_flag = is_signed ? 1 : 0;
      sized_flag = 0;
      basename_ = nam;
      local_flag_ = local_flag;
      أكد(file_idx >= 0 && file_idx <= INT_MAX);
      file_idx_ = (unsigned) file_idx;
      أكد(lineno >= 0 && lineno <= INT_MAX);
      lineno_ = (unsigned) lineno;
}

__vpiBinaryParam::~__vpiBinaryParam()
{
      delete[]basename_;
}

صحيح __vpiBinaryParam::get_type_code(فراغ) ثابت
{ החזר vpiParameter; }

صحيح __vpiBinaryParam::vpi_get(صحيح code)
{
    بدل (code) {
      حالة vpiLineNo :
        החזר lineno_;

      حالة vpiLocalParam :
        החזר local_flag_;

      افتراضي :
        החזר __vpiBinaryConst::vpi_get(code);
    }
}

حرف*__vpiBinaryParam::vpi_get_str(صحيح code)
{
      אם (code == vpiFile)
	    החזר simple_set_rbuf_str(file_names[file_idx_]);

      החזר generic_get_str(code, scope_, basename_, عدم);
}


vpiHandle __vpiBinaryParam::vpi_handle(صحيح code)
{
      بدل (code) {
	  حالة vpiScope:
	    החזר scope_;

	  حالة vpiModule:
	    החזר vpip_module(scope_);

	  افتراضي:
	    החזר 0;
      }
}


vpiHandle vpip_make_binary_param(ثابت حرف*name, ثابت vvp_vector4_t&bits,
				 منطقي signed_flag, منطقي local_flag,
				 long file_idx, long lineno)
{
      بنية __vpiBinaryParam*obj = جديد __vpiBinaryParam(vpip_peek_current_scope(),
                                                         bits, signed_flag, name,
                                                         local_flag, file_idx, lineno);

      החזר obj;
}



__vpiDecConst::__vpiDecConst(صحيح val)
{
      value = val;
}

__vpiDecConst::__vpiDecConst(ثابت __vpiDecConst&that)
: __vpiHandle(), value(that.value)
{
}


صحيح __vpiDecConst::get_type_code(فراغ) ثابت
{ החזר vpiConstant; }

صحيح __vpiDecConst::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpiConstType:
	    החזר vpiDecConst;

	  حالة vpiSigned:
	    החזר 1;

	  حالة vpiSize:
	    החזר 32;

          حالة vpiAutomatic:
	    החזר 0;

#אם defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          حالة _vpiFromThr:
	      החזר _vpiNoThr;
#endif

	  افتراضي:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiDecConst\n", code);
	    أكد(0);
	    החזר 0;
      }
}


فراغ __vpiDecConst::vpi_get_value(p_vpi_value vp)
{
      ساكن ثابت size_t RBUF_USE_SIZE = 64 + 1;
      حرف*rbuf = static_cast<حرف *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));
      حرف*cp = rbuf;

      بدل (vp->format) {

	  حالة vpiObjTypeVal:
	  حالة vpiIntVal: {
		vp->value.integer = value;
		اكسر;
	  }

          حالة vpiDecStrVal:
	      snprintf(rbuf, RBUF_USE_SIZE, "%d", value);
	      vp->value.str = rbuf;
	      اكسر;

          حالة vpiBinStrVal:
	      עבור(صحيح bit=31; bit>=0;bit--){
		  *cp++ = "01"[ (static_cast<unsigned>(value)>>bit)&1 ];
	      }
	      *cp = 0;

	      vp->value.str = rbuf;
	      اكسر;

          حالة vpiHexStrVal:
	      snprintf(rbuf, RBUF_USE_SIZE, "%08x", value);
	      vp->value.str = rbuf;
	      اكسر;

          حالة vpiOctStrVal:
	      snprintf(rbuf, RBUF_USE_SIZE, "%011x", value);
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

صحيح __vpiRealConst::get_type_code(فراغ) ثابت
{ החזר vpiConstant; }

صحيح __vpiRealConst::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpiLineNo:
	    החזר 0;  // Not implemented for now!

	  حالة vpiSize:
	    החזר 1;

	  حالة vpiConstType:
	    החזר vpiRealConst;

	  حالة vpiSigned:
	    החזר 1;

          حالة vpiAutomatic:
	    החזר 0;

#אם defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          حالة _vpiFromThr:
	      החזר _vpiNoThr;
#endif

	  افتراضي:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiRealConst\n", code);
	    أكد(0);
	    החזר 0;
      }
}


فراغ __vpiRealConst::vpi_get_value(p_vpi_value val)
{
      vpip_real_get_value(value, val);
}


vpiHandle vpip_make_real_const(مزدوج value)
{
      __vpiRealConst*obj = جديد __vpiRealConst(value);
      החזר obj;
}

بنية __vpiRealParam  : عام __vpiRealConst {
      __vpiRealParam(__vpiScope* scope, مزدوج val, ثابت حرف*name,
                     منطقي local_flag, long file_idx, long lineno);
      ~__vpiRealParam() override;
      صحيح get_type_code(فراغ) ثابت override;
      صحيح vpi_get(صحيح code) override;
      حرف*vpi_get_str(صحيح code) override;
      vpiHandle vpi_handle(صحيح code) override;

    خاص:
      __vpiScope* scope_;
      ثابت حرف*basename_;
      منطقي local_flag_;
      unsigned file_idx_;
      unsigned lineno_;
};


مضمن __vpiRealParam::__vpiRealParam(__vpiScope* scope, مزدوج val, ثابت حرف*name,
                                      منطقي local_flag, long file_idx, long lineno)
: __vpiRealConst(val)
{
      scope_ = scope;
      basename_ = name;
      local_flag_ = local_flag;
      أكد(file_idx >= 0 && file_idx <= INT_MAX);
      file_idx_ = (unsigned) file_idx;
      أكد(lineno >= 0 && lineno <= INT_MAX);
      lineno_ = (unsigned) lineno;
}

__vpiRealParam::~__vpiRealParam()
{
      delete[]basename_;
}


صحيح __vpiRealParam::get_type_code(فراغ) ثابت
{ החזר vpiParameter; }

صحيح __vpiRealParam::vpi_get(صحيح code)
{
    بدل (code) {
      حالة vpiLineNo :
        החזר lineno_;

      حالة vpiLocalParam :
        החזר local_flag_;

      افتراضي :
           החזר __vpiRealConst::vpi_get(code);
    }
}

حرف* __vpiRealParam::vpi_get_str(صحيح code)
{
      אם (code == vpiFile)
            החזר simple_set_rbuf_str(file_names[file_idx_]);

      החזר generic_get_str(code, scope_, basename_, عدم);
}

vpiHandle __vpiRealParam::vpi_handle(صحيح code)
{
      بدل (code) {
          حالة vpiScope:
            החזר scope_;

	  حالة vpiModule:
	    החזר vpip_module(scope_);

          افتراضي:
            החזר 0;
      }
}


vpiHandle vpip_make_real_param(ثابت حرف*name, مزدوج value,
                               منطقي local_flag, long file_idx, long lineno)
{
      بنية __vpiRealParam*obj = جديد __vpiRealParam(vpip_peek_current_scope(),
                                                     value, name, local_flag,
                                                     file_idx, lineno);
      החזר obj;
}

/*
 * Make a VPI null constant
 */
مضمن __vpiNullConst::__vpiNullConst()
{ }

صحيح __vpiNullConst::get_type_code(فراغ) ثابت
{ החזר vpiConstant; }

صحيح __vpiNullConst::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpiLineNo:
	    החזר 0;  // Not implemented for now!

	  حالة vpiSize:
	    החזר 32;

	  حالة vpiConstType:
	    החזר vpiNullConst;

	  حالة vpiSigned:
	    החזר 0;

          حالة vpiAutomatic:
	    החזר 0;

#אם defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          حالة _vpiFromThr:
	      החזר _vpiNoThr;
#endif

	  افتراضي:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiNullConst\n", code);
	    أكد(0);
	    החזר 0;
      }
}

فراغ __vpiNullConst::vpi_get_value(p_vpi_value val)
{
      ساكن ثابت size_t RBUF_USE_SIZE = 64 + 1;
      حرف*rbuf = static_cast<حرف *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));

      بدل (val->format) {

	  حالة vpiObjTypeVal:
	    val->format = vpiStringVal;
	    // fall through
	  حالة vpiBinStrVal:
	  حالة vpiDecStrVal:
	  حالة vpiOctStrVal:
	  حالة vpiHexStrVal:
	  حالة vpiStringVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "null");
	    val->value.str = rbuf;
	    اكسر;

          حالة vpiScalarVal:
	    val->value.scalar = vpi0;
	    اكسر;

	  حالة vpiIntVal:
	    val->value.integer = 0;
	    اكسر;

	  حالة vpiVectorVal:
	    val->value.vector = static_cast<p_vpi_vecval>
	                        (need_result_buf(2*حجم(s_vpi_vecval),
	                                         RBUF_VAL));
	    עבור (unsigned idx = 0; idx < 2; idx += 1) {
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
      החזר obj;
}

#ifdef CHECK_WITH_VALGRIND
فراغ constant_delete(vpiHandle item)
{
      أكد(item->get_type_code() == vpiConstant);
      بدل(vpi_get(vpiConstType, item)) {
	  حالة vpiStringConst:
	    delete dynamic_cast<__vpiStringConst*>(item);
	    اكسر;
	  حالة vpiDecConst:
	    delete dynamic_cast<__vpiDecConst*>(item);
	    اكسر;
	  حالة vpiBinaryConst:
	    delete dynamic_cast<__vpiBinaryConst*>(item);
	    اكسر;
	  حالة vpiRealConst:
	    delete dynamic_cast<__vpiRealConst*>(item);
	    اكسر;
	  حالة vpiNullConst:
	    delete dynamic_cast<__vpiNullConst*>(item);
	    اكسر;
	  افتراضي:
	    أكد(0);
      }
}

فراغ parameter_delete(vpiHandle item)
{
      بدل(vpi_get(vpiConstType, item)) {
	  حالة vpiStringConst: {
	    מחלקה __vpiStringParam*rfp = dynamic_cast<__vpiStringParam*>(item);
	    delete rfp;
	    اكسر; }
	  حالة vpiBinaryConst: {
	    بنية __vpiBinaryParam*rfp = dynamic_cast<__vpiBinaryParam*>(item);
	    delete rfp;
	    اكسر; }
	  حالة vpiRealConst: {
	    بنية __vpiRealParam*rfp = dynamic_cast<__vpiRealParam*>(item);
	    delete rfp;
	    اكسر; }
	  افتراضي:
	    أكد(0);
      }
}
#endif
