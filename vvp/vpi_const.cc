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
      explicit __vpiStringConst(वर्ण*val);
      ~__vpiStringConst() override;
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override;
      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override;
      शून्य vpi_get_मानम्(p_vpi_मानम् val) override;

    गुप्त:
      शून्य process_string_();
    गुप्त:
      वर्ण*मानम्_2;
      size_t मानम्_len;
};

अन्तःस्थ __vpiStringConst::__vpiStringConst(वर्ण*v)
: मानम्_2(v)
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
      वर्ण*chr = मानम्_2;
      वर्ण*dp = मानम्_2;

      जाव (*chr) {
	    वर्ण next_char = *chr;

	      /* Process octal escapes that I might find. */
	    जइ (*chr == '\\') {
		  प्रति (पूर्णाङ्क idx = 1 ;  idx <= 3 ;  idx += 1) {
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
      मानम्_len = dp - मानम्_2;
}

__vpiStringConst::~__vpiStringConst()
{
      delete[] मानम्_2;
}

पूर्णाङ्क __vpiStringConst::get_type_code(शून्य) ध्रुव
{ प्रत्यागम vpiConstant; }

पूर्णाङ्क __vpiStringConst::vpi_get(पूर्णाङ्क code)
{
      विकल्प (code) {
          पक्ष vpiSize:
	    प्रत्यागम मानम्_len * 8;

          पक्ष vpiSigned:
	      प्रत्यागम 0;

	  पक्ष vpiConstType:
	      प्रत्यागम vpiStringConst;

          पक्ष vpiAutomatic:
	      प्रत्यागम 0;

#जइ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          पक्ष _vpiFromThr:
	      प्रत्यागम _vpiNoThr;
#endif

	  स्वभाव:
	      fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiStringConst\n", code);
	      प्रतिज्ञा(0);
	      प्रत्यागम 0;
      }
}

शून्य __vpiStringConst::vpi_get_मानम्(p_vpi_मानम् vp)
{
      unsigned uint_मानम्;
      p_vpi_vecval vecp;
      unsigned size = मानम्_len;
      वर्ण*rbuf = 0;
      वर्ण*cp;

      विकल्प (vp->format) {
	  पक्ष vpiObjTypeVal:
	      /* String parameters by default have vpiStringVal values. */
	    vp->format = vpiStringVal;
	    // fallthrough
	  पक्ष vpiStringVal:
	    cp = मानम्_2;
	    rbuf = static_cast<वर्ण *>(need_result_buf(size + 1, RBUF_VAL));
	    vp->value.str = rbuf;

	    प्रति (unsigned पूर्णाङ्क i = 0; i < size; i++) {
		  // Ignore leading null-bytes and replace other null-bytes with space.
		  // The LRM is not entirely clear on how null bytes should be handled.
		  // This is the implementation chosen for iverilog.
		  जइ (*cp)
			*rbuf++ = *cp;
		  अण्णहा जइ (rbuf != vp->value.str)
			*rbuf++ = ' ';

		  cp++;
	    }
	    *rbuf = '\0';
	    भङ्ग;

          पक्ष vpiDecStrVal:
	    // Take the (up to 4) characters of the text, convert the characters
	    // to a numerical value, and convert that value to a decimal string.
	    // For example, the string "A" is ASCII 65, so the resulting string
	    // will be "65". and the string "AB" is 65*256 + 66 == 16706, so
	    // the resulting string is "16706". The "size" is the number of
	    // characters of input text to put to work.
	    जइ (size > 4){
		  // We only support standard integers. Ignore other bytes...
		  size = 4;
		  fprintf(stderr, "Warning (vpi_const.cc): %%d on constant strings only looks "
			  "at first 4 bytes!\n");
	    }
	    स्थिर ध्रुव size_t RBUF_USE_SIZE = 12;
	    rbuf = static_cast<वर्ण *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));
	    uint_मानम् = 0;
	    प्रति(unsigned i=0; i<size; i += 1){
		  uint_मानम् <<=8;
		  uint_मानम् += (unsigned वर्ण)(मानम्_2[i]);
	    }
	    snprintf(rbuf, RBUF_USE_SIZE, "%u", uint_मानम्);
	    vp->value.str = rbuf;
	    भङ्ग;

          पक्ष vpiBinStrVal:
	      rbuf = static_cast<वर्ण *>(need_result_buf(8 * size + 1, RBUF_VAL));
	      cp = rbuf;
	      प्रति(unsigned i=0; i<size; i += 1){
		  प्रति(पूर्णाङ्क bit=7; bit>=0; bit -= 1){
		      *cp++ = "01"[ (मानम्_2[i]>>bit)&1 ];
		  }
	      }
	      *cp = 0;
	      vp->value.str = rbuf;
	      भङ्ग;

          पक्ष vpiHexStrVal:
	      rbuf = static_cast<वर्ण *>(need_result_buf(2 * size + 1, RBUF_VAL));
	      cp = rbuf;
	      प्रति(unsigned i=0; i<size; i += 1){
		  प्रति(पूर्णाङ्क nibble=1; nibble>=0; nibble -= 1){
		      *cp++ = "0123456789abcdef"[ (मानम्_2[i]>>(nibble*4))&15 ];
		  }
	      }
	      *cp = 0;
	      vp->value.str = rbuf;
	      भङ्ग;

          पक्ष vpiOctStrVal:
	      fprintf(stderr, "ERROR (vpi_const.cc): %%o display of constant strings not yet implemented\n");
	      प्रतिज्ञा(0);
	      भङ्ग;

          पक्ष vpiIntVal:
	      vp->value.integer = 0;
	      प्रति(unsigned i=0; i<size; i += 1){
		  प्रति(पूर्णाङ्क bit=7; bit>=0; bit -= 1){
		      vp->value.integer <<= 1;
		      vp->value.integer += (मानम्_2[i]>>bit)&1;
		  }
	      }
	      भङ्ग;

          पक्ष vpiVectorVal:
              vp->value.vector = static_cast<p_vpi_vecval>
                                 (need_result_buf((size+3)/4*
                                                   परिमाण(s_vpi_vecval),
                                                  RBUF_VAL));
              uint_मानम् = 0;
              vecp = vp->value.vector;
              vecp->aval = vecp->bval = 0;
	      प्रति(unsigned i=0; i<size; i += 1){
		  vecp->aval |= मानम्_2[i] << uint_मानम्*8;
		  uint_मानम् += 1;
		  जइ (uint_मानम् > 3) {
		      uint_मानम् = 0;
		      vecp += 1;
		      vecp->aval = vecp->bval = 0;
		  }
	      }
	      भङ्ग;


	  स्वभाव:
	    fprintf(stderr, "ERROR (vpi_const.cc): vp->format: %d\n",
	            (पूर्णाङ्क)vp->format);
	    प्रतिज्ञा(0);

	    vp->format = vpiSuppressVal;
	    भङ्ग;
      }
}


संरचना __vpiStringConstTEMP : सार्वजनिक __vpiStringConst {
      explicit अन्तःस्थ __vpiStringConstTEMP(वर्ण*v) : __vpiStringConst(v) { }
      free_object_fun_t free_object_fun(शून्य) override;
};

स्थिर पूर्णाङ्क free_temp_string(vpiHandle obj)
{
      संरचना __vpiStringConstTEMP*rfp = dynamic_cast<__vpiStringConstTEMP*>(obj);
      delete rfp;
      प्रत्यागम 1;
}

__vpiHandle::free_object_fun_t __vpiStringConstTEMP::free_object_fun(शून्य)
{ प्रत्यागम &free_temp_string; }

vpiHandle vpip_make_string_const(वर्ण*text, तार्किक persistent_flag)
{
      __vpiStringConst*obj;

      obj = persistent_flag? नूतन __vpiStringConst(text) : नूतन __vpiStringConstTEMP(text);

      प्रत्यागम obj;
}


वर्ग __vpiStringParam  : सार्वजनिक __vpiStringConst {
    सार्वजनिक:
      __vpiStringParam(__vpiScope* scope, वर्ण*txt, ध्रुव वर्ण*name,
                       तार्किक local_flag, long file_idx, long lineno);
      ~__vpiStringParam() override;
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override;
      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override;
      वर्ण*vpi_get_str(पूर्णाङ्क code) override;
      vpiHandle vpi_handle(पूर्णाङ्क code) override;

    गुप्त:
      __vpiScope* scope_;
      ध्रुव वर्ण*basename_;
      तार्किक       local_flag_;
      unsigned   file_idx_;
      unsigned   lineno_;
};

अन्तःस्थ __vpiStringParam::__vpiStringParam(__vpiScope* scope, वर्ण*txt, ध्रुव वर्ण*nam,
                                          तार्किक local_flag, long file_idx, long lineno)
: __vpiStringConst(txt)
{
      scope_ = scope;
      basename_ = nam;
      local_flag_ = local_flag;
      प्रतिज्ञा(file_idx >= 0 && file_idx <= INT_MAX);
      file_idx_ = (unsigned) file_idx;
      प्रतिज्ञा(lineno >= 0 && lineno <= INT_MAX);
      lineno_ = (unsigned) lineno;
}

__vpiStringParam::~__vpiStringParam()
{
      delete[]basename_;
}

पूर्णाङ्क __vpiStringParam::get_type_code(शून्य) ध्रुव
{ प्रत्यागम vpiParameter; }

पूर्णाङ्क __vpiStringParam::vpi_get(पूर्णाङ्क code)
{
    विकल्प (code) {
       पक्ष vpi_पङ्क्तिः_No :
         प्रत्यागम lineno_;

       पक्ष vpiLocalParam :
         प्रत्यागम local_flag_;

       स्वभाव :
         प्रत्यागम __vpiStringConst::vpi_get(code);
    }
}


वर्ण*__vpiStringParam::vpi_get_str(पूर्णाङ्क code)
{
      जइ (code == vpiFile) {
	    प्रत्यागम simple_set_rbuf_str(file_names[file_idx_]);
      }

      प्रत्यागम generic_get_str(code, scope_, basename_, रिक्त);
}


vpiHandle __vpiStringParam::vpi_handle(पूर्णाङ्क code)
{
      विकल्प (code) {
	  पक्ष vpiScope:
	    प्रत्यागम scope_;

	  पक्ष vpiModule:
	    प्रत्यागम vpip_module(scope_);

	  स्वभाव:
	    प्रत्यागम 0;
      }
}

vpiHandle vpip_make_string_param(ध्रुव वर्ण*name, वर्ण*text,
                                 तार्किक local_flag, long file_idx, long lineno)
{
      __vpiStringParam*obj = नूतन __vpiStringParam(vpip_peek_current_scope(), text, name,
                                                  local_flag, file_idx, lineno);

      प्रत्यागम obj;
}



अन्तःस्थ __vpiBinaryConst::__vpiBinaryConst()
: signed_flag(0), sized_flag(0)
{ }

पूर्णाङ्क __vpiBinaryConst::get_type_code(शून्य) ध्रुव
{ प्रत्यागम vpiConstant; }

पूर्णाङ्क __vpiBinaryConst::vpi_get(पूर्णाङ्क code)
{
      विकल्प (code) {
	  पक्ष vpiConstType:
	    प्रत्यागम vpi_द्विमान_Const;

	  पक्ष vpi_पङ्क्तिः_No:
	    प्रत्यागम 0;  // Not implemented for now!

	  पक्ष vpiSigned:
	    प्रत्यागम signed_flag? 1 : 0;

	  पक्ष vpiSize:
	    प्रत्यागम bits.size();

          पक्ष vpiAutomatic:
	    प्रत्यागम 0;

#जइ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          पक्ष _vpiFromThr:
	      प्रत्यागम _vpiNoThr;
#endif

	  स्वभाव:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiBinaryConst\n", code);
	    प्रतिज्ञा(0);
	    प्रत्यागम 0;
      }
}


शून्य __vpiBinaryConst::vpi_get_मानम्(p_vpi_मानम् val)
{
      विकल्प (val->format) {

	  पक्ष vpiObjTypeVal:
	  पक्ष vpiBinStrVal:
	  पक्ष vpiDecStrVal:
	  पक्ष vpiOctStrVal:
	  पक्ष vpiHexStrVal:
          पक्ष vpiScalarVal:
	  पक्ष vpiIntVal:
	  पक्ष vpiVectorVal:
	  पक्ष vpiStringVal:
	  पक्ष vpiRealVal:
	    vpip_vec4_get_मानम्(bits, bits.size(), signed_flag, val);
	    भङ्ग;

	  स्वभाव:
	    fprintf(stderr, "vvp error: format %d not supported "
		    "by vpiBinaryConst\n", (पूर्णाङ्क)val->format);
	    val->format = vpiSuppressVal;
	    भङ्ग;
      }
}

/*
 * Make a VPI constant from a vector string. The string is normally a
 * ASCII string, with each letter a 4-value bit. The first character
 * may be an 's' if the vector is signed.
 */
vpiHandle vpip_make_द्विमान_const(unsigned wid, ध्रुव वर्ण*bits)
{
      संरचना __vpiBinaryConst*obj = नूतन __vpiBinaryConst;

      ध्रुव वर्ण*bp = bits;
      जइ (*bp == 's') {
	    bp += 1;
	    obj->signed_flag = 1;
      }

      obj->bits = vector4_from_text(bp, wid);

      प्रत्यागम obj;
}

vvp_vector4_t vector4_from_text(ध्रुव वर्ण*bits, unsigned wid)
{
      vvp_vector4_t res (wid);

      प्रति (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t val = BIT4_0;
	    विकल्प (bits[wid-idx-1]) {
		पक्ष '0':
		  val = BIT4_0;
		  भङ्ग;
		पक्ष '1':
		  val = BIT4_1;
		  भङ्ग;
		पक्ष 'x':
		  val = BIT4_X;
		  भङ्ग;
		पक्ष 'z':
		  val = BIT4_Z;
		  भङ्ग;
	    }

	    res.set_bit(idx, val);
      }

      प्रत्यागम res;
}

संरचना __vpiBinaryParam  : सार्वजनिक __vpiBinaryConst {
      __vpiBinaryParam(__vpiScope*scope, ध्रुव vvp_vector4_t&b, तार्किक is_signed,
                       ध्रुव वर्ण*name, तार्किक local_flag, long file_idx, long lineno);
      ~__vpiBinaryParam() override;
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override;
      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override;
      वर्ण*vpi_get_str(पूर्णाङ्क code) override;
      vpiHandle vpi_handle(पूर्णाङ्क code) override;

    गुप्त:
      __vpiScope*scope_;
      ध्रुव वर्ण*basename_;
      तार्किक     local_flag_;
      unsigned file_idx_;
      unsigned lineno_;
};

अन्तःस्थ __vpiBinaryParam::__vpiBinaryParam(__vpiScope*scope, ध्रुव vvp_vector4_t&b,
                                          तार्किक is_signed, ध्रुव वर्ण*nam,
                                          तार्किक local_flag, long file_idx, long lineno)
{
      scope_ = scope;
      bits = b;
      signed_flag = is_signed ? 1 : 0;
      sized_flag = 0;
      basename_ = nam;
      local_flag_ = local_flag;
      प्रतिज्ञा(file_idx >= 0 && file_idx <= INT_MAX);
      file_idx_ = (unsigned) file_idx;
      प्रतिज्ञा(lineno >= 0 && lineno <= INT_MAX);
      lineno_ = (unsigned) lineno;
}

__vpiBinaryParam::~__vpiBinaryParam()
{
      delete[]basename_;
}

पूर्णाङ्क __vpiBinaryParam::get_type_code(शून्य) ध्रुव
{ प्रत्यागम vpiParameter; }

पूर्णाङ्क __vpiBinaryParam::vpi_get(पूर्णाङ्क code)
{
    विकल्प (code) {
      पक्ष vpi_पङ्क्तिः_No :
        प्रत्यागम lineno_;

      पक्ष vpiLocalParam :
        प्रत्यागम local_flag_;

      स्वभाव :
        प्रत्यागम __vpiBinaryConst::vpi_get(code);
    }
}

वर्ण*__vpiBinaryParam::vpi_get_str(पूर्णाङ्क code)
{
      जइ (code == vpiFile)
	    प्रत्यागम simple_set_rbuf_str(file_names[file_idx_]);

      प्रत्यागम generic_get_str(code, scope_, basename_, रिक्त);
}


vpiHandle __vpiBinaryParam::vpi_handle(पूर्णाङ्क code)
{
      विकल्प (code) {
	  पक्ष vpiScope:
	    प्रत्यागम scope_;

	  पक्ष vpiModule:
	    प्रत्यागम vpip_module(scope_);

	  स्वभाव:
	    प्रत्यागम 0;
      }
}


vpiHandle vpip_make_द्विमान_param(ध्रुव वर्ण*name, ध्रुव vvp_vector4_t&bits,
				 तार्किक signed_flag, तार्किक local_flag,
				 long file_idx, long lineno)
{
      संरचना __vpiBinaryParam*obj = नूतन __vpiBinaryParam(vpip_peek_current_scope(),
                                                         bits, signed_flag, name,
                                                         local_flag, file_idx, lineno);

      प्रत्यागम obj;
}



__vpiDecConst::__vpiDecConst(पूर्णाङ्क val)
{
      value = val;
}

__vpiDecConst::__vpiDecConst(ध्रुव __vpiDecConst&that)
: __vpiHandle(), value(that.value)
{
}


पूर्णाङ्क __vpiDecConst::get_type_code(शून्य) ध्रुव
{ प्रत्यागम vpiConstant; }

पूर्णाङ्क __vpiDecConst::vpi_get(पूर्णाङ्क code)
{
      विकल्प (code) {
	  पक्ष vpiConstType:
	    प्रत्यागम vpiDecConst;

	  पक्ष vpiSigned:
	    प्रत्यागम 1;

	  पक्ष vpiSize:
	    प्रत्यागम 32;

          पक्ष vpiAutomatic:
	    प्रत्यागम 0;

#जइ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          पक्ष _vpiFromThr:
	      प्रत्यागम _vpiNoThr;
#endif

	  स्वभाव:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiDecConst\n", code);
	    प्रतिज्ञा(0);
	    प्रत्यागम 0;
      }
}


शून्य __vpiDecConst::vpi_get_मानम्(p_vpi_मानम् vp)
{
      स्थिर ध्रुव size_t RBUF_USE_SIZE = 64 + 1;
      वर्ण*rbuf = static_cast<वर्ण *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));
      वर्ण*cp = rbuf;

      विकल्प (vp->format) {

	  पक्ष vpiObjTypeVal:
	  पक्ष vpiIntVal: {
		vp->value.integer = value;
		भङ्ग;
	  }

          पक्ष vpiDecStrVal:
	      snprintf(rbuf, RBUF_USE_SIZE, "%d", value);
	      vp->value.str = rbuf;
	      भङ्ग;

          पक्ष vpiBinStrVal:
	      प्रति(पूर्णाङ्क bit=31; bit>=0;bit--){
		  *cp++ = "01"[ (static_cast<unsigned>(value)>>bit)&1 ];
	      }
	      *cp = 0;

	      vp->value.str = rbuf;
	      भङ्ग;

          पक्ष vpiHexStrVal:
	      snprintf(rbuf, RBUF_USE_SIZE, "%08x", value);
	      vp->value.str = rbuf;
	      भङ्ग;

          पक्ष vpiOctStrVal:
	      snprintf(rbuf, RBUF_USE_SIZE, "%011x", value);
	      vp->value.str = rbuf;
	      भङ्ग;

	  स्वभाव:
	    fprintf(stderr, "vvp error (vpi_const.cc): format %d not supported "
		    "by vpiDecConst\n", (पूर्णाङ्क)vp->format);
	    vp->format = vpiSuppressVal;
	    भङ्ग;
      }
}


अन्तःस्थ __vpiRealConst::__vpiRealConst(द्विगुण val)
: value(val)
{ }

पूर्णाङ्क __vpiRealConst::get_type_code(शून्य) ध्रुव
{ प्रत्यागम vpiConstant; }

पूर्णाङ्क __vpiRealConst::vpi_get(पूर्णाङ्क code)
{
      विकल्प (code) {
	  पक्ष vpi_पङ्क्तिः_No:
	    प्रत्यागम 0;  // Not implemented for now!

	  पक्ष vpiSize:
	    प्रत्यागम 1;

	  पक्ष vpiConstType:
	    प्रत्यागम vpiRealConst;

	  पक्ष vpiSigned:
	    प्रत्यागम 1;

          पक्ष vpiAutomatic:
	    प्रत्यागम 0;

#जइ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          पक्ष _vpiFromThr:
	      प्रत्यागम _vpiNoThr;
#endif

	  स्वभाव:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiRealConst\n", code);
	    प्रतिज्ञा(0);
	    प्रत्यागम 0;
      }
}


शून्य __vpiRealConst::vpi_get_मानम्(p_vpi_मानम् val)
{
      vpip_real_get_मानम्(value, val);
}


vpiHandle vpip_make_real_const(द्विगुण value)
{
      __vpiRealConst*obj = नूतन __vpiRealConst(value);
      प्रत्यागम obj;
}

संरचना __vpiRealParam  : सार्वजनिक __vpiRealConst {
      __vpiRealParam(__vpiScope* scope, द्विगुण val, ध्रुव वर्ण*name,
                     तार्किक local_flag, long file_idx, long lineno);
      ~__vpiRealParam() override;
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override;
      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override;
      वर्ण*vpi_get_str(पूर्णाङ्क code) override;
      vpiHandle vpi_handle(पूर्णाङ्क code) override;

    गुप्त:
      __vpiScope* scope_;
      ध्रुव वर्ण*basename_;
      तार्किक local_flag_;
      unsigned file_idx_;
      unsigned lineno_;
};


अन्तःस्थ __vpiRealParam::__vpiRealParam(__vpiScope* scope, द्विगुण val, ध्रुव वर्ण*name,
                                      तार्किक local_flag, long file_idx, long lineno)
: __vpiRealConst(val)
{
      scope_ = scope;
      basename_ = name;
      local_flag_ = local_flag;
      प्रतिज्ञा(file_idx >= 0 && file_idx <= INT_MAX);
      file_idx_ = (unsigned) file_idx;
      प्रतिज्ञा(lineno >= 0 && lineno <= INT_MAX);
      lineno_ = (unsigned) lineno;
}

__vpiRealParam::~__vpiRealParam()
{
      delete[]basename_;
}


पूर्णाङ्क __vpiRealParam::get_type_code(शून्य) ध्रुव
{ प्रत्यागम vpiParameter; }

पूर्णाङ्क __vpiRealParam::vpi_get(पूर्णाङ्क code)
{
    विकल्प (code) {
      पक्ष vpi_पङ्क्तिः_No :
        प्रत्यागम lineno_;

      पक्ष vpiLocalParam :
        प्रत्यागम local_flag_;

      स्वभाव :
           प्रत्यागम __vpiRealConst::vpi_get(code);
    }
}

वर्ण* __vpiRealParam::vpi_get_str(पूर्णाङ्क code)
{
      जइ (code == vpiFile)
            प्रत्यागम simple_set_rbuf_str(file_names[file_idx_]);

      प्रत्यागम generic_get_str(code, scope_, basename_, रिक्त);
}

vpiHandle __vpiRealParam::vpi_handle(पूर्णाङ्क code)
{
      विकल्प (code) {
          पक्ष vpiScope:
            प्रत्यागम scope_;

	  पक्ष vpiModule:
	    प्रत्यागम vpip_module(scope_);

          स्वभाव:
            प्रत्यागम 0;
      }
}


vpiHandle vpip_make_real_param(ध्रुव वर्ण*name, द्विगुण value,
                               तार्किक local_flag, long file_idx, long lineno)
{
      संरचना __vpiRealParam*obj = नूतन __vpiRealParam(vpip_peek_current_scope(),
                                                     value, name, local_flag,
                                                     file_idx, lineno);
      प्रत्यागम obj;
}

/*
 * Make a VPI null constant
 */
अन्तःस्थ __vpiNullConst::__vpiNullConst()
{ }

पूर्णाङ्क __vpiNullConst::get_type_code(शून्य) ध्रुव
{ प्रत्यागम vpiConstant; }

पूर्णाङ्क __vpiNullConst::vpi_get(पूर्णाङ्क code)
{
      विकल्प (code) {
	  पक्ष vpi_पङ्क्तिः_No:
	    प्रत्यागम 0;  // Not implemented for now!

	  पक्ष vpiSize:
	    प्रत्यागम 32;

	  पक्ष vpiConstType:
	    प्रत्यागम vpiNullConst;

	  पक्ष vpiSigned:
	    प्रत्यागम 0;

          पक्ष vpiAutomatic:
	    प्रत्यागम 0;

#जइ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          पक्ष _vpiFromThr:
	      प्रत्यागम _vpiNoThr;
#endif

	  स्वभाव:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiNullConst\n", code);
	    प्रतिज्ञा(0);
	    प्रत्यागम 0;
      }
}

शून्य __vpiNullConst::vpi_get_मानम्(p_vpi_मानम् val)
{
      स्थिर ध्रुव size_t RBUF_USE_SIZE = 64 + 1;
      वर्ण*rbuf = static_cast<वर्ण *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));

      विकल्प (val->format) {

	  पक्ष vpiObjTypeVal:
	    val->format = vpiStringVal;
	    // fall through
	  पक्ष vpiBinStrVal:
	  पक्ष vpiDecStrVal:
	  पक्ष vpiOctStrVal:
	  पक्ष vpiHexStrVal:
	  पक्ष vpiStringVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "null");
	    val->value.str = rbuf;
	    भङ्ग;

          पक्ष vpiScalarVal:
	    val->value.scalar = vpi0;
	    भङ्ग;

	  पक्ष vpiIntVal:
	    val->value.integer = 0;
	    भङ्ग;

	  पक्ष vpiVectorVal:
	    val->value.vector = static_cast<p_vpi_vecval>
	                        (need_result_buf(2*परिमाण(s_vpi_vecval),
	                                         RBUF_VAL));
	    प्रति (unsigned idx = 0; idx < 2; idx += 1) {
		  val->value.vector[idx].aval = 0;
		  val->value.vector[idx].bval = 0;
	    }
	    भङ्ग;

	  पक्ष vpiRealVal:
	    val->value.real = 0.0;
	    भङ्ग;

	  स्वभाव:
	    fprintf(stderr, "vvp error: format %d not supported "
		    "by vpiNullConst\n", (पूर्णाङ्क)val->format);
	    val->format = vpiSuppressVal;
	    भङ्ग;
      }
}

vpiHandle vpip_make_null_const()
{
      __vpiNullConst*obj = नूतन __vpiNullConst;
      प्रत्यागम obj;
}

#ifdef CHECK_WITH_VALGRIND
शून्य constant_delete(vpiHandle item)
{
      प्रतिज्ञा(item->get_type_code() == vpiConstant);
      विकल्प(vpi_get(vpiConstType, item)) {
	  पक्ष vpiStringConst:
	    delete dynamic_cast<__vpiStringConst*>(item);
	    भङ्ग;
	  पक्ष vpiDecConst:
	    delete dynamic_cast<__vpiDecConst*>(item);
	    भङ्ग;
	  पक्ष vpi_द्विमान_Const:
	    delete dynamic_cast<__vpiBinaryConst*>(item);
	    भङ्ग;
	  पक्ष vpiRealConst:
	    delete dynamic_cast<__vpiRealConst*>(item);
	    भङ्ग;
	  पक्ष vpiNullConst:
	    delete dynamic_cast<__vpiNullConst*>(item);
	    भङ्ग;
	  स्वभाव:
	    प्रतिज्ञा(0);
      }
}

शून्य parameter_delete(vpiHandle item)
{
      विकल्प(vpi_get(vpiConstType, item)) {
	  पक्ष vpiStringConst: {
	    वर्ग __vpiStringParam*rfp = dynamic_cast<__vpiStringParam*>(item);
	    delete rfp;
	    भङ्ग; }
	  पक्ष vpi_द्विमान_Const: {
	    संरचना __vpiBinaryParam*rfp = dynamic_cast<__vpiBinaryParam*>(item);
	    delete rfp;
	    भङ्ग; }
	  पक्ष vpiRealConst: {
	    संरचना __vpiRealParam*rfp = dynamic_cast<__vpiRealParam*>(item);
	    delete rfp;
	    भङ्ग; }
	  स्वभाव:
	    प्रतिज्ञा(0);
      }
}
#endif
