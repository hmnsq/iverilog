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

رده __vpiStringConst : عمومی __vpiHandle {
    عمومی:
      explicit __vpiStringConst(نویسه*val);
      ~__vpiStringConst() override;
      صحیح get_type_code(تهی) ثابت override;
      صحیح vpi_get(صحیح code) override;
      تهی vpi_get_مقدار(p_vpi_مقدار val) override;

    خصوصی:
      تهی process_string_();
    خصوصی:
      نویسه*مقدار_2;
      size_t مقدار_len;
};

درونخطی __vpiStringConst::__vpiStringConst(نویسه*v)
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
تهی __vpiStringConst::process_string_(تهی)
{
      نویسه*chr = مقدار_2;
      نویسه*dp = مقدار_2;

      وقتی (*chr) {
	    نویسه next_char = *chr;

	      /* Process octal escapes that I might find. */
	    اگر (*chr == '\\') {
		  برای (صحیح idx = 1 ;  idx <= 3 ;  idx += 1) {
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
      مقدار_len = dp - مقدار_2;
}

__vpiStringConst::~__vpiStringConst()
{
      delete[] مقدار_2;
}

صحیح __vpiStringConst::get_type_code(تهی) ثابت
{ بازگشت vpiConstant; }

صحیح __vpiStringConst::vpi_get(صحیح code)
{
      انتخاب (code) {
          حالت vpiSize:
	    بازگشت مقدار_len * 8;

          حالت vpiSigned:
	      بازگشت 0;

	  حالت vpiConstType:
	      بازگشت vpiStringConst;

          حالت vpiAutomatic:
	      بازگشت 0;

#اگر defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          حالت _vpiFromThr:
	      بازگشت _vpiNoThr;
#endif

	  پیشفرض:
	      fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiStringConst\n", code);
	      أكد(0);
	      بازگشت 0;
      }
}

تهی __vpiStringConst::vpi_get_مقدار(p_vpi_مقدار vp)
{
      unsigned uint_مقدار;
      p_vpi_vecval vecp;
      unsigned size = مقدار_len;
      نویسه*rbuf = 0;
      نویسه*cp;

      انتخاب (vp->format) {
	  حالت vpiObjTypeVal:
	      /* String parameters by default have vpiStringVal values. */
	    vp->format = vpiStringVal;
	    // fallthrough
	  حالت vpiStringVal:
	    cp = مقدار_2;
	    rbuf = static_cast<نویسه *>(need_result_buf(size + 1, RBUF_VAL));
	    vp->value.str = rbuf;

	    برای (unsigned صحیح i = 0; i < size; i++) {
		  // Ignore leading null-bytes and replace other null-bytes with space.
		  // The LRM is not entirely clear on how null bytes should be handled.
		  // This is the implementation chosen for iverilog.
		  اگر (*cp)
			*rbuf++ = *cp;
		  وگرنه اگر (rbuf != vp->value.str)
			*rbuf++ = ' ';

		  cp++;
	    }
	    *rbuf = '\0';
	    بشکن;

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
	    ایستا ثابت size_t RBUF_USE_SIZE = 12;
	    rbuf = static_cast<نویسه *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));
	    uint_مقدار = 0;
	    برای(unsigned i=0; i<size; i += 1){
		  uint_مقدار <<=8;
		  uint_مقدار += (unsigned نویسه)(مقدار_2[i]);
	    }
	    snprintf(rbuf, RBUF_USE_SIZE, "%u", uint_مقدار);
	    vp->value.str = rbuf;
	    بشکن;

          حالت vpiBinStrVal:
	      rbuf = static_cast<نویسه *>(need_result_buf(8 * size + 1, RBUF_VAL));
	      cp = rbuf;
	      برای(unsigned i=0; i<size; i += 1){
		  برای(صحیح bit=7; bit>=0; bit -= 1){
		      *cp++ = "01"[ (مقدار_2[i]>>bit)&1 ];
		  }
	      }
	      *cp = 0;
	      vp->value.str = rbuf;
	      بشکن;

          حالت vpiHexStrVal:
	      rbuf = static_cast<نویسه *>(need_result_buf(2 * size + 1, RBUF_VAL));
	      cp = rbuf;
	      برای(unsigned i=0; i<size; i += 1){
		  برای(صحیح nibble=1; nibble>=0; nibble -= 1){
		      *cp++ = "0123456789abcdef"[ (مقدار_2[i]>>(nibble*4))&15 ];
		  }
	      }
	      *cp = 0;
	      vp->value.str = rbuf;
	      بشکن;

          حالت vpiOctStrVal:
	      fprintf(stderr, "ERROR (vpi_const.cc): %%o display of constant strings not yet implemented\n");
	      أكد(0);
	      بشکن;

          حالت vpiIntVal:
	      vp->value.integer = 0;
	      برای(unsigned i=0; i<size; i += 1){
		  برای(صحیح bit=7; bit>=0; bit -= 1){
		      vp->value.integer <<= 1;
		      vp->value.integer += (مقدار_2[i]>>bit)&1;
		  }
	      }
	      بشکن;

          حالت vpiVectorVal:
              vp->value.vector = static_cast<p_vpi_vecval>
                                 (need_result_buf((size+3)/4*
                                                   اندازه(s_vpi_vecval),
                                                  RBUF_VAL));
              uint_مقدار = 0;
              vecp = vp->value.vector;
              vecp->aval = vecp->bval = 0;
	      برای(unsigned i=0; i<size; i += 1){
		  vecp->aval |= مقدار_2[i] << uint_مقدار*8;
		  uint_مقدار += 1;
		  اگر (uint_مقدار > 3) {
		      uint_مقدار = 0;
		      vecp += 1;
		      vecp->aval = vecp->bval = 0;
		  }
	      }
	      بشکن;


	  پیشفرض:
	    fprintf(stderr, "ERROR (vpi_const.cc): vp->format: %d\n",
	            (صحیح)vp->format);
	    أكد(0);

	    vp->format = vpiSuppressVal;
	    بشکن;
      }
}


ساختار __vpiStringConstTEMP : عمومی __vpiStringConst {
      explicit درونخطی __vpiStringConstTEMP(نویسه*v) : __vpiStringConst(v) { }
      free_object_fun_t free_object_fun(تهی) override;
};

ایستا صحیح free_temp_string(vpiHandle obj)
{
      ساختار __vpiStringConstTEMP*rfp = dynamic_cast<__vpiStringConstTEMP*>(obj);
      delete rfp;
      بازگشت 1;
}

__vpiHandle::free_object_fun_t __vpiStringConstTEMP::free_object_fun(تهی)
{ بازگشت &free_temp_string; }

vpiHandle vpip_make_string_const(نویسه*text, منطقی persistent_flag)
{
      __vpiStringConst*obj;

      obj = persistent_flag? جدید __vpiStringConst(text) : جدید __vpiStringConstTEMP(text);

      بازگشت obj;
}


رده __vpiStringParam  : عمومی __vpiStringConst {
    عمومی:
      __vpiStringParam(__vpiScope* scope, نویسه*txt, ثابت نویسه*name,
                       منطقی local_flag, long file_idx, long lineno);
      ~__vpiStringParam() override;
      صحیح get_type_code(تهی) ثابت override;
      صحیح vpi_get(صحیح code) override;
      نویسه*vpi_get_str(صحیح code) override;
      vpiHandle vpi_handle(صحیح code) override;

    خصوصی:
      __vpiScope* scope_;
      ثابت نویسه*basename_;
      منطقی       local_flag_;
      unsigned   file_idx_;
      unsigned   lineno_;
};

درونخطی __vpiStringParam::__vpiStringParam(__vpiScope* scope, نویسه*txt, ثابت نویسه*nam,
                                          منطقی local_flag, long file_idx, long lineno)
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

صحیح __vpiStringParam::get_type_code(تهی) ثابت
{ بازگشت vpiParameter; }

صحیح __vpiStringParam::vpi_get(صحیح code)
{
    انتخاب (code) {
       حالت vpi_سطر_No :
         بازگشت lineno_;

       حالت vpiLocalParam :
         بازگشت local_flag_;

       پیشفرض :
         بازگشت __vpiStringConst::vpi_get(code);
    }
}


نویسه*__vpiStringParam::vpi_get_str(صحیح code)
{
      اگر (code == vpiFile) {
	    بازگشت simple_set_rbuf_str(file_names[file_idx_]);
      }

      بازگشت generic_get_str(code, scope_, basename_, پوچ);
}


vpiHandle __vpiStringParam::vpi_handle(صحیح code)
{
      انتخاب (code) {
	  حالت vpiScope:
	    بازگشت scope_;

	  حالت vpiModule:
	    بازگشت vpip_module(scope_);

	  پیشفرض:
	    بازگشت 0;
      }
}

vpiHandle vpip_make_string_param(ثابت نویسه*name, نویسه*text,
                                 منطقی local_flag, long file_idx, long lineno)
{
      __vpiStringParam*obj = جدید __vpiStringParam(vpip_peek_current_scope(), text, name,
                                                  local_flag, file_idx, lineno);

      بازگشت obj;
}



درونخطی __vpiBinaryConst::__vpiBinaryConst()
: signed_flag(0), sized_flag(0)
{ }

صحیح __vpiBinaryConst::get_type_code(تهی) ثابت
{ بازگشت vpiConstant; }

صحیح __vpiBinaryConst::vpi_get(صحیح code)
{
      انتخاب (code) {
	  حالت vpiConstType:
	    بازگشت vpi_دودویی_Const;

	  حالت vpi_سطر_No:
	    بازگشت 0;  // Not implemented for now!

	  حالت vpiSigned:
	    بازگشت signed_flag? 1 : 0;

	  حالت vpiSize:
	    بازگشت bits.size();

          حالت vpiAutomatic:
	    بازگشت 0;

#اگر defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          حالت _vpiFromThr:
	      بازگشت _vpiNoThr;
#endif

	  پیشفرض:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiBinaryConst\n", code);
	    أكد(0);
	    بازگشت 0;
      }
}


تهی __vpiBinaryConst::vpi_get_مقدار(p_vpi_مقدار val)
{
      انتخاب (val->format) {

	  حالت vpiObjTypeVal:
	  حالت vpiBinStrVal:
	  حالت vpiDecStrVal:
	  حالت vpiOctStrVal:
	  حالت vpiHexStrVal:
          حالت vpiScalarVal:
	  حالت vpiIntVal:
	  حالت vpiVectorVal:
	  حالت vpiStringVal:
	  حالت vpiRealVal:
	    vpip_vec4_get_مقدار(bits, bits.size(), signed_flag, val);
	    بشکن;

	  پیشفرض:
	    fprintf(stderr, "vvp error: format %d not supported "
		    "by vpiBinaryConst\n", (صحیح)val->format);
	    val->format = vpiSuppressVal;
	    بشکن;
      }
}

/*
 * Make a VPI constant from a vector string. The string is normally a
 * ASCII string, with each letter a 4-value bit. The first character
 * may be an 's' if the vector is signed.
 */
vpiHandle vpip_make_دودویی_const(unsigned wid, ثابت نویسه*bits)
{
      ساختار __vpiBinaryConst*obj = جدید __vpiBinaryConst;

      ثابت نویسه*bp = bits;
      اگر (*bp == 's') {
	    bp += 1;
	    obj->signed_flag = 1;
      }

      obj->bits = vector4_from_text(bp, wid);

      بازگشت obj;
}

vvp_vector4_t vector4_from_text(ثابت نویسه*bits, unsigned wid)
{
      vvp_vector4_t res (wid);

      برای (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t val = BIT4_0;
	    انتخاب (bits[wid-idx-1]) {
		حالت '0':
		  val = BIT4_0;
		  بشکن;
		حالت '1':
		  val = BIT4_1;
		  بشکن;
		حالت 'x':
		  val = BIT4_X;
		  بشکن;
		حالت 'z':
		  val = BIT4_Z;
		  بشکن;
	    }

	    res.set_bit(idx, val);
      }

      بازگشت res;
}

ساختار __vpiBinaryParam  : عمومی __vpiBinaryConst {
      __vpiBinaryParam(__vpiScope*scope, ثابت vvp_vector4_t&b, منطقی is_signed,
                       ثابت نویسه*name, منطقی local_flag, long file_idx, long lineno);
      ~__vpiBinaryParam() override;
      صحیح get_type_code(تهی) ثابت override;
      صحیح vpi_get(صحیح code) override;
      نویسه*vpi_get_str(صحیح code) override;
      vpiHandle vpi_handle(صحیح code) override;

    خصوصی:
      __vpiScope*scope_;
      ثابت نویسه*basename_;
      منطقی     local_flag_;
      unsigned file_idx_;
      unsigned lineno_;
};

درونخطی __vpiBinaryParam::__vpiBinaryParam(__vpiScope*scope, ثابت vvp_vector4_t&b,
                                          منطقی is_signed, ثابت نویسه*nam,
                                          منطقی local_flag, long file_idx, long lineno)
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

صحیح __vpiBinaryParam::get_type_code(تهی) ثابت
{ بازگشت vpiParameter; }

صحیح __vpiBinaryParam::vpi_get(صحیح code)
{
    انتخاب (code) {
      حالت vpi_سطر_No :
        بازگشت lineno_;

      حالت vpiLocalParam :
        بازگشت local_flag_;

      پیشفرض :
        بازگشت __vpiBinaryConst::vpi_get(code);
    }
}

نویسه*__vpiBinaryParam::vpi_get_str(صحیح code)
{
      اگر (code == vpiFile)
	    بازگشت simple_set_rbuf_str(file_names[file_idx_]);

      بازگشت generic_get_str(code, scope_, basename_, پوچ);
}


vpiHandle __vpiBinaryParam::vpi_handle(صحیح code)
{
      انتخاب (code) {
	  حالت vpiScope:
	    بازگشت scope_;

	  حالت vpiModule:
	    بازگشت vpip_module(scope_);

	  پیشفرض:
	    بازگشت 0;
      }
}


vpiHandle vpip_make_دودویی_param(ثابت نویسه*name, ثابت vvp_vector4_t&bits,
				 منطقی signed_flag, منطقی local_flag,
				 long file_idx, long lineno)
{
      ساختار __vpiBinaryParam*obj = جدید __vpiBinaryParam(vpip_peek_current_scope(),
                                                         bits, signed_flag, name,
                                                         local_flag, file_idx, lineno);

      بازگشت obj;
}



__vpiDecConst::__vpiDecConst(صحیح val)
{
      value = val;
}

__vpiDecConst::__vpiDecConst(ثابت __vpiDecConst&that)
: __vpiHandle(), value(that.value)
{
}


صحیح __vpiDecConst::get_type_code(تهی) ثابت
{ بازگشت vpiConstant; }

صحیح __vpiDecConst::vpi_get(صحیح code)
{
      انتخاب (code) {
	  حالت vpiConstType:
	    بازگشت vpiDecConst;

	  حالت vpiSigned:
	    بازگشت 1;

	  حالت vpiSize:
	    بازگشت 32;

          حالت vpiAutomatic:
	    بازگشت 0;

#اگر defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          حالت _vpiFromThr:
	      بازگشت _vpiNoThr;
#endif

	  پیشفرض:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiDecConst\n", code);
	    أكد(0);
	    بازگشت 0;
      }
}


تهی __vpiDecConst::vpi_get_مقدار(p_vpi_مقدار vp)
{
      ایستا ثابت size_t RBUF_USE_SIZE = 64 + 1;
      نویسه*rbuf = static_cast<نویسه *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));
      نویسه*cp = rbuf;

      انتخاب (vp->format) {

	  حالت vpiObjTypeVal:
	  حالت vpiIntVal: {
		vp->value.integer = value;
		بشکن;
	  }

          حالت vpiDecStrVal:
	      snprintf(rbuf, RBUF_USE_SIZE, "%d", value);
	      vp->value.str = rbuf;
	      بشکن;

          حالت vpiBinStrVal:
	      برای(صحیح bit=31; bit>=0;bit--){
		  *cp++ = "01"[ (static_cast<unsigned>(value)>>bit)&1 ];
	      }
	      *cp = 0;

	      vp->value.str = rbuf;
	      بشکن;

          حالت vpiHexStrVal:
	      snprintf(rbuf, RBUF_USE_SIZE, "%08x", value);
	      vp->value.str = rbuf;
	      بشکن;

          حالت vpiOctStrVal:
	      snprintf(rbuf, RBUF_USE_SIZE, "%011x", value);
	      vp->value.str = rbuf;
	      بشکن;

	  پیشفرض:
	    fprintf(stderr, "vvp error (vpi_const.cc): format %d not supported "
		    "by vpiDecConst\n", (صحیح)vp->format);
	    vp->format = vpiSuppressVal;
	    بشکن;
      }
}


درونخطی __vpiRealConst::__vpiRealConst(دوگانه val)
: value(val)
{ }

صحیح __vpiRealConst::get_type_code(تهی) ثابت
{ بازگشت vpiConstant; }

صحیح __vpiRealConst::vpi_get(صحیح code)
{
      انتخاب (code) {
	  حالت vpi_سطر_No:
	    بازگشت 0;  // Not implemented for now!

	  حالت vpiSize:
	    بازگشت 1;

	  حالت vpiConstType:
	    بازگشت vpiRealConst;

	  حالت vpiSigned:
	    بازگشت 1;

          حالت vpiAutomatic:
	    بازگشت 0;

#اگر defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          حالت _vpiFromThr:
	      بازگشت _vpiNoThr;
#endif

	  پیشفرض:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiRealConst\n", code);
	    أكد(0);
	    بازگشت 0;
      }
}


تهی __vpiRealConst::vpi_get_مقدار(p_vpi_مقدار val)
{
      vpip_real_get_مقدار(value, val);
}


vpiHandle vpip_make_real_const(دوگانه value)
{
      __vpiRealConst*obj = جدید __vpiRealConst(value);
      بازگشت obj;
}

ساختار __vpiRealParam  : عمومی __vpiRealConst {
      __vpiRealParam(__vpiScope* scope, دوگانه val, ثابت نویسه*name,
                     منطقی local_flag, long file_idx, long lineno);
      ~__vpiRealParam() override;
      صحیح get_type_code(تهی) ثابت override;
      صحیح vpi_get(صحیح code) override;
      نویسه*vpi_get_str(صحیح code) override;
      vpiHandle vpi_handle(صحیح code) override;

    خصوصی:
      __vpiScope* scope_;
      ثابت نویسه*basename_;
      منطقی local_flag_;
      unsigned file_idx_;
      unsigned lineno_;
};


درونخطی __vpiRealParam::__vpiRealParam(__vpiScope* scope, دوگانه val, ثابت نویسه*name,
                                      منطقی local_flag, long file_idx, long lineno)
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


صحیح __vpiRealParam::get_type_code(تهی) ثابت
{ بازگشت vpiParameter; }

صحیح __vpiRealParam::vpi_get(صحیح code)
{
    انتخاب (code) {
      حالت vpi_سطر_No :
        بازگشت lineno_;

      حالت vpiLocalParam :
        بازگشت local_flag_;

      پیشفرض :
           بازگشت __vpiRealConst::vpi_get(code);
    }
}

نویسه* __vpiRealParam::vpi_get_str(صحیح code)
{
      اگر (code == vpiFile)
            بازگشت simple_set_rbuf_str(file_names[file_idx_]);

      بازگشت generic_get_str(code, scope_, basename_, پوچ);
}

vpiHandle __vpiRealParam::vpi_handle(صحیح code)
{
      انتخاب (code) {
          حالت vpiScope:
            بازگشت scope_;

	  حالت vpiModule:
	    بازگشت vpip_module(scope_);

          پیشفرض:
            بازگشت 0;
      }
}


vpiHandle vpip_make_real_param(ثابت نویسه*name, دوگانه value,
                               منطقی local_flag, long file_idx, long lineno)
{
      ساختار __vpiRealParam*obj = جدید __vpiRealParam(vpip_peek_current_scope(),
                                                     value, name, local_flag,
                                                     file_idx, lineno);
      بازگشت obj;
}

/*
 * Make a VPI null constant
 */
درونخطی __vpiNullConst::__vpiNullConst()
{ }

صحیح __vpiNullConst::get_type_code(تهی) ثابت
{ بازگشت vpiConstant; }

صحیح __vpiNullConst::vpi_get(صحیح code)
{
      انتخاب (code) {
	  حالت vpi_سطر_No:
	    بازگشت 0;  // Not implemented for now!

	  حالت vpiSize:
	    بازگشت 32;

	  حالت vpiConstType:
	    بازگشت vpiNullConst;

	  حالت vpiSigned:
	    بازگشت 0;

          حالت vpiAutomatic:
	    بازگشت 0;

#اگر defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          حالت _vpiFromThr:
	      بازگشت _vpiNoThr;
#endif

	  پیشفرض:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiNullConst\n", code);
	    أكد(0);
	    بازگشت 0;
      }
}

تهی __vpiNullConst::vpi_get_مقدار(p_vpi_مقدار val)
{
      ایستا ثابت size_t RBUF_USE_SIZE = 64 + 1;
      نویسه*rbuf = static_cast<نویسه *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));

      انتخاب (val->format) {

	  حالت vpiObjTypeVal:
	    val->format = vpiStringVal;
	    // fall through
	  حالت vpiBinStrVal:
	  حالت vpiDecStrVal:
	  حالت vpiOctStrVal:
	  حالت vpiHexStrVal:
	  حالت vpiStringVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "null");
	    val->value.str = rbuf;
	    بشکن;

          حالت vpiScalarVal:
	    val->value.scalar = vpi0;
	    بشکن;

	  حالت vpiIntVal:
	    val->value.integer = 0;
	    بشکن;

	  حالت vpiVectorVal:
	    val->value.vector = static_cast<p_vpi_vecval>
	                        (need_result_buf(2*اندازه(s_vpi_vecval),
	                                         RBUF_VAL));
	    برای (unsigned idx = 0; idx < 2; idx += 1) {
		  val->value.vector[idx].aval = 0;
		  val->value.vector[idx].bval = 0;
	    }
	    بشکن;

	  حالت vpiRealVal:
	    val->value.real = 0.0;
	    بشکن;

	  پیشفرض:
	    fprintf(stderr, "vvp error: format %d not supported "
		    "by vpiNullConst\n", (صحیح)val->format);
	    val->format = vpiSuppressVal;
	    بشکن;
      }
}

vpiHandle vpip_make_null_const()
{
      __vpiNullConst*obj = جدید __vpiNullConst;
      بازگشت obj;
}

#ifdef CHECK_WITH_VALGRIND
تهی constant_delete(vpiHandle item)
{
      أكد(item->get_type_code() == vpiConstant);
      انتخاب(vpi_get(vpiConstType, item)) {
	  حالت vpiStringConst:
	    delete dynamic_cast<__vpiStringConst*>(item);
	    بشکن;
	  حالت vpiDecConst:
	    delete dynamic_cast<__vpiDecConst*>(item);
	    بشکن;
	  حالت vpi_دودویی_Const:
	    delete dynamic_cast<__vpiBinaryConst*>(item);
	    بشکن;
	  حالت vpiRealConst:
	    delete dynamic_cast<__vpiRealConst*>(item);
	    بشکن;
	  حالت vpiNullConst:
	    delete dynamic_cast<__vpiNullConst*>(item);
	    بشکن;
	  پیشفرض:
	    أكد(0);
      }
}

تهی parameter_delete(vpiHandle item)
{
      انتخاب(vpi_get(vpiConstType, item)) {
	  حالت vpiStringConst: {
	    رده __vpiStringParam*rfp = dynamic_cast<__vpiStringParam*>(item);
	    delete rfp;
	    بشکن; }
	  حالت vpi_دودویی_Const: {
	    ساختار __vpiBinaryParam*rfp = dynamic_cast<__vpiBinaryParam*>(item);
	    delete rfp;
	    بشکن; }
	  حالت vpiRealConst: {
	    ساختار __vpiRealParam*rfp = dynamic_cast<__vpiRealParam*>(item);
	    delete rfp;
	    بشکن; }
	  پیشفرض:
	    أكد(0);
      }
}
#endif
