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

वर्ग __vpiStringConst : सार्वजनिक __vpiHandle {
    सार्वजनिक:
      explicit __vpiStringConst(अक्षर*val);
      ~__vpiStringConst() override;
      पूर्णांक get_type_code(शून्य) स्थिरांक override;
      पूर्णांक vpi_get(पूर्णांक code) override;
      शून्य vpi_get_मूल्य(p_vpi_मूल्य val) override;

    खाजगी:
      शून्य process_string_();
    खाजगी:
      अक्षर*मूल्य_2;
      माप_प्रकार मूल्य_len;
};

ओळीत __vpiStringConst::__vpiStringConst(अक्षर*v)
: मूल्य_2(v)
{
      process_string_();
}

/*
 * Strings are described at the level of the vvp source as a string
 * with literal characters or octal escapes. No other escapes are
 * included, they are processed already by the compiler that generated
 * the vvp source.
 */
शून्य __vpiStringConst::process_string_(शून्य)
{
      अक्षर*chr = मूल्य_2;
      अक्षर*dp = मूल्य_2;

      पर्यंत (*chr) {
	    अक्षर next_char = *chr;

	      /* Process octal escapes that I might find. */
	    जर (*chr == '\\') {
		  साठी (पूर्णांक idx = 1 ;  idx <= 3 ;  idx += 1) {
			प्रतिज्ञा(chr[idx] != 0);
			प्रतिज्ञा(chr[idx] < '8');
			प्रतिज्ञा(chr[idx] >= '0');
			next_char = next_char*8 + chr[idx] - '0';
		  }
		  chr += 3;
	    }
	    *dp++ = next_char;
	    chr += 1;
      }
      *dp = 0;
      मूल्य_len = dp - मूल्य_2;
}

__vpiStringConst::~__vpiStringConst()
{
      delete[] मूल्य_2;
}

पूर्णांक __vpiStringConst::get_type_code(शून्य) स्थिरांक
{ परत vpiConstant; }

पूर्णांक __vpiStringConst::vpi_get(पूर्णांक code)
{
      निवड (code) {
          स्थिती vpiSize:
	    परत मूल्य_len * 8;

          स्थिती vpiSigned:
	      परत 0;

	  स्थिती vpiConstType:
	      परत vpiStringConst;

          स्थिती vpiAutomatic:
	      परत 0;

#जर defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          स्थिती _vpiFromThr:
	      परत _vpiNoThr;
#endif

	  पूर्वनिर्धारित:
	      ख_लिखो(मानक_त्रुटि, "vvp error: get %d not supported "
		      "by vpiStringConst\n", code);
	      प्रतिज्ञा(0);
	      परत 0;
      }
}

शून्य __vpiStringConst::vpi_get_मूल्य(p_vpi_मूल्य vp)
{
      अचिन्हित uint_मूल्य;
      p_vpi_vecval vecp;
      अचिन्हित size = मूल्य_len;
      अक्षर*rbuf = 0;
      अक्षर*cp;

      निवड (vp->format) {
	  स्थिती vpiObjTypeVal:
	      /* String parameters by default have vpiStringVal values. */
	    vp->format = vpiStringVal;
	    // fallthrough
	  स्थिती vpiStringVal:
	    cp = मूल्य_2;
	    rbuf = static_cast<अक्षर *>(need_result_buf(size + 1, RBUF_VAL));
	    vp->value.str = rbuf;

	    साठी (अचिन्हित पूर्णांक i = 0; i < size; i++) {
		  // Ignore leading null-bytes and replace other null-bytes with space.
		  // The LRM is not entirely clear on how null bytes should be handled.
		  // This is the implementation chosen for iverilog.
		  जर (*cp)
			*rbuf++ = *cp;
		  अन्यथा जर (rbuf != vp->value.str)
			*rbuf++ = ' ';

		  cp++;
	    }
	    *rbuf = '\0';
	    थांबा;

          स्थिती vpiDecStrVal:
	    // Take the (up to 4) characters of the text, convert the characters
	    // to a numerical value, and convert that value to a decimal string.
	    // For example, the string "A" is ASCII 65, so the resulting string
	    // will be "65". and the string "AB" is 65*256 + 66 == 16706, so
	    // the resulting string is "16706". The "size" is the number of
	    // characters of input text to put to work.
	    जर (size > 4){
		  // We only support standard integers. Ignore other bytes...
		  size = 4;
		  ख_लिखो(मानक_त्रुटि, "Warning (vpi_const.cc): %%d on constant strings only looks "
			  "at first 4 bytes!\n");
	    }
	    स्थिर स्थिरांक माप_प्रकार RBUF_USE_SIZE = 12;
	    rbuf = static_cast<अक्षर *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));
	    uint_मूल्य = 0;
	    साठी(अचिन्हित i=0; i<size; i += 1){
		  uint_मूल्य <<=8;
		  uint_मूल्य += (अचिन्हित अक्षर)(मूल्य_2[i]);
	    }
	    snprintf(rbuf, RBUF_USE_SIZE, "%u", uint_मूल्य);
	    vp->value.str = rbuf;
	    थांबा;

          स्थिती vpiBinStrVal:
	      rbuf = static_cast<अक्षर *>(need_result_buf(8 * size + 1, RBUF_VAL));
	      cp = rbuf;
	      साठी(अचिन्हित i=0; i<size; i += 1){
		  साठी(पूर्णांक bit=7; bit>=0; bit -= 1){
		      *cp++ = "01"[ (मूल्य_2[i]>>bit)&1 ];
		  }
	      }
	      *cp = 0;
	      vp->value.str = rbuf;
	      थांबा;

          स्थिती vpiHexStrVal:
	      rbuf = static_cast<अक्षर *>(need_result_buf(2 * size + 1, RBUF_VAL));
	      cp = rbuf;
	      साठी(अचिन्हित i=0; i<size; i += 1){
		  साठी(पूर्णांक nibble=1; nibble>=0; nibble -= 1){
		      *cp++ = "0123456789abcdef"[ (मूल्य_2[i]>>(nibble*4))&15 ];
		  }
	      }
	      *cp = 0;
	      vp->value.str = rbuf;
	      थांबा;

          स्थिती vpiOctStrVal:
	      ख_लिखो(मानक_त्रुटि, "ERROR (vpi_const.cc): %%o display of constant strings not yet implemented\n");
	      प्रतिज्ञा(0);
	      थांबा;

          स्थिती vpiIntVal:
	      vp->value.integer = 0;
	      साठी(अचिन्हित i=0; i<size; i += 1){
		  साठी(पूर्णांक bit=7; bit>=0; bit -= 1){
		      vp->value.integer <<= 1;
		      vp->value.integer += (मूल्य_2[i]>>bit)&1;
		  }
	      }
	      थांबा;

          स्थिती vpiVectorVal:
              vp->value.vector = static_cast<p_vpi_vecval>
                                 (need_result_buf((size+3)/4*
                                                   आकार(s_vpi_vecval),
                                                  RBUF_VAL));
              uint_मूल्य = 0;
              vecp = vp->value.vector;
              vecp->aval = vecp->bval = 0;
	      साठी(अचिन्हित i=0; i<size; i += 1){
		  vecp->aval |= मूल्य_2[i] << uint_मूल्य*8;
		  uint_मूल्य += 1;
		  जर (uint_मूल्य > 3) {
		      uint_मूल्य = 0;
		      vecp += 1;
		      vecp->aval = vecp->bval = 0;
		  }
	      }
	      थांबा;


	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "ERROR (vpi_const.cc): vp->format: %d\n",
	            (पूर्णांक)vp->format);
	    प्रतिज्ञा(0);

	    vp->format = vpiSuppressVal;
	    थांबा;
      }
}


संरचना __vpiStringConstTEMP : सार्वजनिक __vpiStringConst {
      explicit ओळीत __vpiStringConstTEMP(अक्षर*v) : __vpiStringConst(v) { }
      free_object_fun_t free_object_fun(शून्य) override;
};

स्थिर पूर्णांक free_temp_string(vpiHandle obj)
{
      संरचना __vpiStringConstTEMP*rfp = dynamic_cast<__vpiStringConstTEMP*>(obj);
      delete rfp;
      परत 1;
}

__vpiHandle::free_object_fun_t __vpiStringConstTEMP::free_object_fun(शून्य)
{ परत &free_temp_string; }

vpiHandle vpip_make_string_const(अक्षर*text, तार्किक persistent_flag)
{
      __vpiStringConst*obj;

      obj = persistent_flag? नवीन __vpiStringConst(text) : नवीन __vpiStringConstTEMP(text);

      परत obj;
}


वर्ग __vpiStringParam  : सार्वजनिक __vpiStringConst {
    सार्वजनिक:
      __vpiStringParam(__vpiScope* scope, अक्षर*txt, स्थिरांक अक्षर*name,
                       तार्किक local_flag, दीर्घ file_idx, दीर्घ lineno);
      ~__vpiStringParam() override;
      पूर्णांक get_type_code(शून्य) स्थिरांक override;
      पूर्णांक vpi_get(पूर्णांक code) override;
      अक्षर*vpi_get_str(पूर्णांक code) override;
      vpiHandle vpi_handle(पूर्णांक code) override;

    खाजगी:
      __vpiScope* scope_;
      स्थिरांक अक्षर*basename_;
      तार्किक       local_flag_;
      अचिन्हित   file_idx_;
      अचिन्हित   lineno_;
};

ओळीत __vpiStringParam::__vpiStringParam(__vpiScope* scope, अक्षर*txt, स्थिरांक अक्षर*nam,
                                          तार्किक local_flag, दीर्घ file_idx, दीर्घ lineno)
: __vpiStringConst(txt)
{
      scope_ = scope;
      basename_ = nam;
      local_flag_ = local_flag;
      प्रतिज्ञा(file_idx >= 0 && file_idx <= पूर्णांक_उच्च);
      file_idx_ = (अचिन्हित) file_idx;
      प्रतिज्ञा(lineno >= 0 && lineno <= पूर्णांक_उच्च);
      lineno_ = (अचिन्हित) lineno;
}

__vpiStringParam::~__vpiStringParam()
{
      delete[]basename_;
}

पूर्णांक __vpiStringParam::get_type_code(शून्य) स्थिरांक
{ परत vpiParameter; }

पूर्णांक __vpiStringParam::vpi_get(पूर्णांक code)
{
    निवड (code) {
       स्थिती vpi_ओळ_No :
         परत lineno_;

       स्थिती vpiLocalParam :
         परत local_flag_;

       पूर्वनिर्धारित :
         परत __vpiStringConst::vpi_get(code);
    }
}


अक्षर*__vpiStringParam::vpi_get_str(पूर्णांक code)
{
      जर (code == vpiFile) {
	    परत simple_set_rbuf_str(file_names[file_idx_]);
      }

      परत generic_get_str(code, scope_, basename_, रिक्त);
}


vpiHandle __vpiStringParam::vpi_handle(पूर्णांक code)
{
      निवड (code) {
	  स्थिती vpiScope:
	    परत scope_;

	  स्थिती vpiModule:
	    परत vpip_module(scope_);

	  पूर्वनिर्धारित:
	    परत 0;
      }
}

vpiHandle vpip_make_string_param(स्थिरांक अक्षर*name, अक्षर*text,
                                 तार्किक local_flag, दीर्घ file_idx, दीर्घ lineno)
{
      __vpiStringParam*obj = नवीन __vpiStringParam(vpip_peek_current_scope(), text, name,
                                                  local_flag, file_idx, lineno);

      परत obj;
}



ओळीत __vpiBinaryConst::__vpiBinaryConst()
: signed_flag(0), sized_flag(0)
{ }

पूर्णांक __vpiBinaryConst::get_type_code(शून्य) स्थिरांक
{ परत vpiConstant; }

पूर्णांक __vpiBinaryConst::vpi_get(पूर्णांक code)
{
      निवड (code) {
	  स्थिती vpiConstType:
	    परत vpi_द्विमान_Const;

	  स्थिती vpi_ओळ_No:
	    परत 0;  // Not implemented for now!

	  स्थिती vpiSigned:
	    परत signed_flag? 1 : 0;

	  स्थिती vpiSize:
	    परत bits.size();

          स्थिती vpiAutomatic:
	    परत 0;

#जर defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          स्थिती _vpiFromThr:
	      परत _vpiNoThr;
#endif

	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "vvp error: get %d not supported "
		    "by vpiBinaryConst\n", code);
	    प्रतिज्ञा(0);
	    परत 0;
      }
}


शून्य __vpiBinaryConst::vpi_get_मूल्य(p_vpi_मूल्य val)
{
      निवड (val->format) {

	  स्थिती vpiObjTypeVal:
	  स्थिती vpiBinStrVal:
	  स्थिती vpiDecStrVal:
	  स्थिती vpiOctStrVal:
	  स्थिती vpiHexStrVal:
          स्थिती vpiScalarVal:
	  स्थिती vpiIntVal:
	  स्थिती vpiVectorVal:
	  स्थिती vpiStringVal:
	  स्थिती vpiRealVal:
	    vpip_vec4_get_मूल्य(bits, bits.size(), signed_flag, val);
	    थांबा;

	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "vvp error: format %d not supported "
		    "by vpiBinaryConst\n", (पूर्णांक)val->format);
	    val->format = vpiSuppressVal;
	    थांबा;
      }
}

/*
 * Make a VPI constant from a vector string. The string is normally a
 * ASCII string, with each letter a 4-value bit. The first character
 * may be an 's' if the vector is signed.
 */
vpiHandle vpip_make_द्विमान_const(अचिन्हित wid, स्थिरांक अक्षर*bits)
{
      संरचना __vpiBinaryConst*obj = नवीन __vpiBinaryConst;

      स्थिरांक अक्षर*bp = bits;
      जर (*bp == 's') {
	    bp += 1;
	    obj->signed_flag = 1;
      }

      obj->bits = vector4_from_text(bp, wid);

      परत obj;
}

vvp_vector4_t vector4_from_text(स्थिरांक अक्षर*bits, अचिन्हित wid)
{
      vvp_vector4_t res (wid);

      साठी (अचिन्हित idx = 0 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t val = BIT4_0;
	    निवड (bits[wid-idx-1]) {
		स्थिती '0':
		  val = BIT4_0;
		  थांबा;
		स्थिती '1':
		  val = BIT4_1;
		  थांबा;
		स्थिती 'x':
		  val = BIT4_X;
		  थांबा;
		स्थिती 'z':
		  val = BIT4_Z;
		  थांबा;
	    }

	    res.set_bit(idx, val);
      }

      परत res;
}

संरचना __vpiBinaryParam  : सार्वजनिक __vpiBinaryConst {
      __vpiBinaryParam(__vpiScope*scope, स्थिरांक vvp_vector4_t&b, तार्किक is_signed,
                       स्थिरांक अक्षर*name, तार्किक local_flag, दीर्घ file_idx, दीर्घ lineno);
      ~__vpiBinaryParam() override;
      पूर्णांक get_type_code(शून्य) स्थिरांक override;
      पूर्णांक vpi_get(पूर्णांक code) override;
      अक्षर*vpi_get_str(पूर्णांक code) override;
      vpiHandle vpi_handle(पूर्णांक code) override;

    खाजगी:
      __vpiScope*scope_;
      स्थिरांक अक्षर*basename_;
      तार्किक     local_flag_;
      अचिन्हित file_idx_;
      अचिन्हित lineno_;
};

ओळीत __vpiBinaryParam::__vpiBinaryParam(__vpiScope*scope, स्थिरांक vvp_vector4_t&b,
                                          तार्किक is_signed, स्थिरांक अक्षर*nam,
                                          तार्किक local_flag, दीर्घ file_idx, दीर्घ lineno)
{
      scope_ = scope;
      bits = b;
      signed_flag = is_signed ? 1 : 0;
      sized_flag = 0;
      basename_ = nam;
      local_flag_ = local_flag;
      प्रतिज्ञा(file_idx >= 0 && file_idx <= पूर्णांक_उच्च);
      file_idx_ = (अचिन्हित) file_idx;
      प्रतिज्ञा(lineno >= 0 && lineno <= पूर्णांक_उच्च);
      lineno_ = (अचिन्हित) lineno;
}

__vpiBinaryParam::~__vpiBinaryParam()
{
      delete[]basename_;
}

पूर्णांक __vpiBinaryParam::get_type_code(शून्य) स्थिरांक
{ परत vpiParameter; }

पूर्णांक __vpiBinaryParam::vpi_get(पूर्णांक code)
{
    निवड (code) {
      स्थिती vpi_ओळ_No :
        परत lineno_;

      स्थिती vpiLocalParam :
        परत local_flag_;

      पूर्वनिर्धारित :
        परत __vpiBinaryConst::vpi_get(code);
    }
}

अक्षर*__vpiBinaryParam::vpi_get_str(पूर्णांक code)
{
      जर (code == vpiFile)
	    परत simple_set_rbuf_str(file_names[file_idx_]);

      परत generic_get_str(code, scope_, basename_, रिक्त);
}


vpiHandle __vpiBinaryParam::vpi_handle(पूर्णांक code)
{
      निवड (code) {
	  स्थिती vpiScope:
	    परत scope_;

	  स्थिती vpiModule:
	    परत vpip_module(scope_);

	  पूर्वनिर्धारित:
	    परत 0;
      }
}


vpiHandle vpip_make_द्विमान_param(स्थिरांक अक्षर*name, स्थिरांक vvp_vector4_t&bits,
				 तार्किक signed_flag, तार्किक local_flag,
				 दीर्घ file_idx, दीर्घ lineno)
{
      संरचना __vpiBinaryParam*obj = नवीन __vpiBinaryParam(vpip_peek_current_scope(),
                                                         bits, signed_flag, name,
                                                         local_flag, file_idx, lineno);

      परत obj;
}



__vpiDecConst::__vpiDecConst(पूर्णांक val)
{
      value = val;
}

__vpiDecConst::__vpiDecConst(स्थिरांक __vpiDecConst&that)
: __vpiHandle(), value(that.value)
{
}


पूर्णांक __vpiDecConst::get_type_code(शून्य) स्थिरांक
{ परत vpiConstant; }

पूर्णांक __vpiDecConst::vpi_get(पूर्णांक code)
{
      निवड (code) {
	  स्थिती vpiConstType:
	    परत vpiDecConst;

	  स्थिती vpiSigned:
	    परत 1;

	  स्थिती vpiSize:
	    परत 32;

          स्थिती vpiAutomatic:
	    परत 0;

#जर defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          स्थिती _vpiFromThr:
	      परत _vpiNoThr;
#endif

	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "vvp error: get %d not supported "
		    "by vpiDecConst\n", code);
	    प्रतिज्ञा(0);
	    परत 0;
      }
}


शून्य __vpiDecConst::vpi_get_मूल्य(p_vpi_मूल्य vp)
{
      स्थिर स्थिरांक माप_प्रकार RBUF_USE_SIZE = 64 + 1;
      अक्षर*rbuf = static_cast<अक्षर *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));
      अक्षर*cp = rbuf;

      निवड (vp->format) {

	  स्थिती vpiObjTypeVal:
	  स्थिती vpiIntVal: {
		vp->value.integer = value;
		थांबा;
	  }

          स्थिती vpiDecStrVal:
	      snprintf(rbuf, RBUF_USE_SIZE, "%d", value);
	      vp->value.str = rbuf;
	      थांबा;

          स्थिती vpiBinStrVal:
	      साठी(पूर्णांक bit=31; bit>=0;bit--){
		  *cp++ = "01"[ (static_cast<अचिन्हित>(value)>>bit)&1 ];
	      }
	      *cp = 0;

	      vp->value.str = rbuf;
	      थांबा;

          स्थिती vpiHexStrVal:
	      snprintf(rbuf, RBUF_USE_SIZE, "%08x", value);
	      vp->value.str = rbuf;
	      थांबा;

          स्थिती vpiOctStrVal:
	      snprintf(rbuf, RBUF_USE_SIZE, "%011x", value);
	      vp->value.str = rbuf;
	      थांबा;

	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "vvp error (vpi_const.cc): format %d not supported "
		    "by vpiDecConst\n", (पूर्णांक)vp->format);
	    vp->format = vpiSuppressVal;
	    थांबा;
      }
}


ओळीत __vpiRealConst::__vpiRealConst(द्विगुण val)
: value(val)
{ }

पूर्णांक __vpiRealConst::get_type_code(शून्य) स्थिरांक
{ परत vpiConstant; }

पूर्णांक __vpiRealConst::vpi_get(पूर्णांक code)
{
      निवड (code) {
	  स्थिती vpi_ओळ_No:
	    परत 0;  // Not implemented for now!

	  स्थिती vpiSize:
	    परत 1;

	  स्थिती vpiConstType:
	    परत vpiRealConst;

	  स्थिती vpiSigned:
	    परत 1;

          स्थिती vpiAutomatic:
	    परत 0;

#जर defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          स्थिती _vpiFromThr:
	      परत _vpiNoThr;
#endif

	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "vvp error: get %d not supported "
		    "by vpiRealConst\n", code);
	    प्रतिज्ञा(0);
	    परत 0;
      }
}


शून्य __vpiRealConst::vpi_get_मूल्य(p_vpi_मूल्य val)
{
      vpip_real_get_मूल्य(value, val);
}


vpiHandle vpip_make_real_const(द्विगुण value)
{
      __vpiRealConst*obj = नवीन __vpiRealConst(value);
      परत obj;
}

संरचना __vpiRealParam  : सार्वजनिक __vpiRealConst {
      __vpiRealParam(__vpiScope* scope, द्विगुण val, स्थिरांक अक्षर*name,
                     तार्किक local_flag, दीर्घ file_idx, दीर्घ lineno);
      ~__vpiRealParam() override;
      पूर्णांक get_type_code(शून्य) स्थिरांक override;
      पूर्णांक vpi_get(पूर्णांक code) override;
      अक्षर*vpi_get_str(पूर्णांक code) override;
      vpiHandle vpi_handle(पूर्णांक code) override;

    खाजगी:
      __vpiScope* scope_;
      स्थिरांक अक्षर*basename_;
      तार्किक local_flag_;
      अचिन्हित file_idx_;
      अचिन्हित lineno_;
};


ओळीत __vpiRealParam::__vpiRealParam(__vpiScope* scope, द्विगुण val, स्थिरांक अक्षर*name,
                                      तार्किक local_flag, दीर्घ file_idx, दीर्घ lineno)
: __vpiRealConst(val)
{
      scope_ = scope;
      basename_ = name;
      local_flag_ = local_flag;
      प्रतिज्ञा(file_idx >= 0 && file_idx <= पूर्णांक_उच्च);
      file_idx_ = (अचिन्हित) file_idx;
      प्रतिज्ञा(lineno >= 0 && lineno <= पूर्णांक_उच्च);
      lineno_ = (अचिन्हित) lineno;
}

__vpiRealParam::~__vpiRealParam()
{
      delete[]basename_;
}


पूर्णांक __vpiRealParam::get_type_code(शून्य) स्थिरांक
{ परत vpiParameter; }

पूर्णांक __vpiRealParam::vpi_get(पूर्णांक code)
{
    निवड (code) {
      स्थिती vpi_ओळ_No :
        परत lineno_;

      स्थिती vpiLocalParam :
        परत local_flag_;

      पूर्वनिर्धारित :
           परत __vpiRealConst::vpi_get(code);
    }
}

अक्षर* __vpiRealParam::vpi_get_str(पूर्णांक code)
{
      जर (code == vpiFile)
            परत simple_set_rbuf_str(file_names[file_idx_]);

      परत generic_get_str(code, scope_, basename_, रिक्त);
}

vpiHandle __vpiRealParam::vpi_handle(पूर्णांक code)
{
      निवड (code) {
          स्थिती vpiScope:
            परत scope_;

	  स्थिती vpiModule:
	    परत vpip_module(scope_);

          पूर्वनिर्धारित:
            परत 0;
      }
}


vpiHandle vpip_make_real_param(स्थिरांक अक्षर*name, द्विगुण value,
                               तार्किक local_flag, दीर्घ file_idx, दीर्घ lineno)
{
      संरचना __vpiRealParam*obj = नवीन __vpiRealParam(vpip_peek_current_scope(),
                                                     value, name, local_flag,
                                                     file_idx, lineno);
      परत obj;
}

/*
 * Make a VPI null constant
 */
ओळीत __vpiNullConst::__vpiNullConst()
{ }

पूर्णांक __vpiNullConst::get_type_code(शून्य) स्थिरांक
{ परत vpiConstant; }

पूर्णांक __vpiNullConst::vpi_get(पूर्णांक code)
{
      निवड (code) {
	  स्थिती vpi_ओळ_No:
	    परत 0;  // Not implemented for now!

	  स्थिती vpiSize:
	    परत 32;

	  स्थिती vpiConstType:
	    परत vpiNullConst;

	  स्थिती vpiSigned:
	    परत 0;

          स्थिती vpiAutomatic:
	    परत 0;

#जर defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          स्थिती _vpiFromThr:
	      परत _vpiNoThr;
#endif

	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "vvp error: get %d not supported "
		    "by vpiNullConst\n", code);
	    प्रतिज्ञा(0);
	    परत 0;
      }
}

शून्य __vpiNullConst::vpi_get_मूल्य(p_vpi_मूल्य val)
{
      स्थिर स्थिरांक माप_प्रकार RBUF_USE_SIZE = 64 + 1;
      अक्षर*rbuf = static_cast<अक्षर *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));

      निवड (val->format) {

	  स्थिती vpiObjTypeVal:
	    val->format = vpiStringVal;
	    // fall through
	  स्थिती vpiBinStrVal:
	  स्थिती vpiDecStrVal:
	  स्थिती vpiOctStrVal:
	  स्थिती vpiHexStrVal:
	  स्थिती vpiStringVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "null");
	    val->value.str = rbuf;
	    थांबा;

          स्थिती vpiScalarVal:
	    val->value.scalar = vpi0;
	    थांबा;

	  स्थिती vpiIntVal:
	    val->value.integer = 0;
	    थांबा;

	  स्थिती vpiVectorVal:
	    val->value.vector = static_cast<p_vpi_vecval>
	                        (need_result_buf(2*आकार(s_vpi_vecval),
	                                         RBUF_VAL));
	    साठी (अचिन्हित idx = 0; idx < 2; idx += 1) {
		  val->value.vector[idx].aval = 0;
		  val->value.vector[idx].bval = 0;
	    }
	    थांबा;

	  स्थिती vpiRealVal:
	    val->value.real = 0.0;
	    थांबा;

	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "vvp error: format %d not supported "
		    "by vpiNullConst\n", (पूर्णांक)val->format);
	    val->format = vpiSuppressVal;
	    थांबा;
      }
}

vpiHandle vpip_make_null_const()
{
      __vpiNullConst*obj = नवीन __vpiNullConst;
      परत obj;
}

#ifdef CHECK_WITH_VALGRIND
शून्य constant_delete(vpiHandle item)
{
      प्रतिज्ञा(item->get_type_code() == vpiConstant);
      निवड(vpi_get(vpiConstType, item)) {
	  स्थिती vpiStringConst:
	    delete dynamic_cast<__vpiStringConst*>(item);
	    थांबा;
	  स्थिती vpiDecConst:
	    delete dynamic_cast<__vpiDecConst*>(item);
	    थांबा;
	  स्थिती vpi_द्विमान_Const:
	    delete dynamic_cast<__vpiBinaryConst*>(item);
	    थांबा;
	  स्थिती vpiRealConst:
	    delete dynamic_cast<__vpiRealConst*>(item);
	    थांबा;
	  स्थिती vpiNullConst:
	    delete dynamic_cast<__vpiNullConst*>(item);
	    थांबा;
	  पूर्वनिर्धारित:
	    प्रतिज्ञा(0);
      }
}

शून्य parameter_delete(vpiHandle item)
{
      निवड(vpi_get(vpiConstType, item)) {
	  स्थिती vpiStringConst: {
	    वर्ग __vpiStringParam*rfp = dynamic_cast<__vpiStringParam*>(item);
	    delete rfp;
	    थांबा; }
	  स्थिती vpi_द्विमान_Const: {
	    संरचना __vpiBinaryParam*rfp = dynamic_cast<__vpiBinaryParam*>(item);
	    delete rfp;
	    थांबा; }
	  स्थिती vpiRealConst: {
	    संरचना __vpiRealParam*rfp = dynamic_cast<__vpiRealParam*>(item);
	    delete rfp;
	    थांबा; }
	  पूर्वनिर्धारित:
	    प्रतिज्ञा(0);
      }
}
#endif
