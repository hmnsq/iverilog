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

வகுப்பு __vpiStringConst : பொது __vpiHandle {
    பொது:
      explicit __vpiStringConst(எழுத்து*val);
      ~__vpiStringConst() override;
      முழு get_type_code(வெறுமை) மாறிலி override;
      முழு vpi_get(முழு code) override;
      வெறுமை vpi_get_மதிப்பு(p_vpi_மதிப்பு val) override;

    தனிப்பட்ட:
      வெறுமை process_string_();
    தனிப்பட்ட:
      எழுத்து*மதிப்பு_2;
      size_t மதிப்பு_len;
};

உள்வரி __vpiStringConst::__vpiStringConst(எழுத்து*v)
: மதிப்பு_2(v)
{
      process_string_();
}

/*
 * Strings are described at the level of the vvp source as a string
 * with literal characters or octal escapes. No other escapes are
 * included, they are processed already by the compiler that generated
 * the vvp source.
 */
வெறுமை __vpiStringConst::process_string_(வெறுமை)
{
      எழுத்து*chr = மதிப்பு_2;
      எழுத்து*dp = மதிப்பு_2;

      போது (*chr) {
	    எழுத்து next_char = *chr;

	      /* Process octal escapes that I might find. */
	    எனில் (*chr == '\\') {
		  க்கு (முழு idx = 1 ;  idx <= 3 ;  idx += 1) {
			ப்ரதிஜ்ஞா(chr[idx] != 0);
			ப்ரதிஜ்ஞா(chr[idx] < '8');
			ப்ரதிஜ்ஞா(chr[idx] >= '0');
			next_char = next_char*8 + chr[idx] - '0';
		  }
		  chr += 3;
	    }
	    *dp++ = next_char;
	    chr += 1;
      }
      *dp = 0;
      மதிப்பு_len = dp - மதிப்பு_2;
}

__vpiStringConst::~__vpiStringConst()
{
      delete[] மதிப்பு_2;
}

முழு __vpiStringConst::get_type_code(வெறுமை) மாறிலி
{ திரும்பு vpiConstant; }

முழு __vpiStringConst::vpi_get(முழு code)
{
      தேர்வு (code) {
          நிலை vpiSize:
	    திரும்பு மதிப்பு_len * 8;

          நிலை vpiSigned:
	      திரும்பு 0;

	  நிலை vpiConstType:
	      திரும்பு vpiStringConst;

          நிலை vpiAutomatic:
	      திரும்பு 0;

#எனில் defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          நிலை _vpiFromThr:
	      திரும்பு _vpiNoThr;
#endif

	  இயல்பு:
	      fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiStringConst\n", code);
	      ப்ரதிஜ்ஞா(0);
	      திரும்பு 0;
      }
}

வெறுமை __vpiStringConst::vpi_get_மதிப்பு(p_vpi_மதிப்பு vp)
{
      unsigned uint_மதிப்பு;
      p_vpi_vecval vecp;
      unsigned size = மதிப்பு_len;
      எழுத்து*rbuf = 0;
      எழுத்து*cp;

      தேர்வு (vp->format) {
	  நிலை vpiObjTypeVal:
	      /* String parameters by default have vpiStringVal values. */
	    vp->format = vpiStringVal;
	    // fallthrough
	  நிலை vpiStringVal:
	    cp = மதிப்பு_2;
	    rbuf = static_cast<எழுத்து *>(need_result_buf(size + 1, RBUF_VAL));
	    vp->value.str = rbuf;

	    க்கு (unsigned முழு i = 0; i < size; i++) {
		  // Ignore leading null-bytes and replace other null-bytes with space.
		  // The LRM is not entirely clear on how null bytes should be handled.
		  // This is the implementation chosen for iverilog.
		  எனில் (*cp)
			*rbuf++ = *cp;
		  இல்லையெனில் எனில் (rbuf != vp->value.str)
			*rbuf++ = ' ';

		  cp++;
	    }
	    *rbuf = '\0';
	    நிறுத்து;

          நிலை vpiDecStrVal:
	    // Take the (up to 4) characters of the text, convert the characters
	    // to a numerical value, and convert that value to a decimal string.
	    // For example, the string "A" is ASCII 65, so the resulting string
	    // will be "65". and the string "AB" is 65*256 + 66 == 16706, so
	    // the resulting string is "16706". The "size" is the number of
	    // characters of input text to put to work.
	    எனில் (size > 4){
		  // We only support standard integers. Ignore other bytes...
		  size = 4;
		  fprintf(stderr, "Warning (vpi_const.cc): %%d on constant strings only looks "
			  "at first 4 bytes!\n");
	    }
	    நிலையான மாறிலி size_t RBUF_USE_SIZE = 12;
	    rbuf = static_cast<எழுத்து *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));
	    uint_மதிப்பு = 0;
	    க்கு(unsigned i=0; i<size; i += 1){
		  uint_மதிப்பு <<=8;
		  uint_மதிப்பு += (unsigned எழுத்து)(மதிப்பு_2[i]);
	    }
	    snprintf(rbuf, RBUF_USE_SIZE, "%u", uint_மதிப்பு);
	    vp->value.str = rbuf;
	    நிறுத்து;

          நிலை vpiBinStrVal:
	      rbuf = static_cast<எழுத்து *>(need_result_buf(8 * size + 1, RBUF_VAL));
	      cp = rbuf;
	      க்கு(unsigned i=0; i<size; i += 1){
		  க்கு(முழு bit=7; bit>=0; bit -= 1){
		      *cp++ = "01"[ (மதிப்பு_2[i]>>bit)&1 ];
		  }
	      }
	      *cp = 0;
	      vp->value.str = rbuf;
	      நிறுத்து;

          நிலை vpiHexStrVal:
	      rbuf = static_cast<எழுத்து *>(need_result_buf(2 * size + 1, RBUF_VAL));
	      cp = rbuf;
	      க்கு(unsigned i=0; i<size; i += 1){
		  க்கு(முழு nibble=1; nibble>=0; nibble -= 1){
		      *cp++ = "0123456789abcdef"[ (மதிப்பு_2[i]>>(nibble*4))&15 ];
		  }
	      }
	      *cp = 0;
	      vp->value.str = rbuf;
	      நிறுத்து;

          நிலை vpiOctStrVal:
	      fprintf(stderr, "ERROR (vpi_const.cc): %%o display of constant strings not yet implemented\n");
	      ப்ரதிஜ்ஞா(0);
	      நிறுத்து;

          நிலை vpiIntVal:
	      vp->value.integer = 0;
	      க்கு(unsigned i=0; i<size; i += 1){
		  க்கு(முழு bit=7; bit>=0; bit -= 1){
		      vp->value.integer <<= 1;
		      vp->value.integer += (மதிப்பு_2[i]>>bit)&1;
		  }
	      }
	      நிறுத்து;

          நிலை vpiVectorVal:
              vp->value.vector = static_cast<p_vpi_vecval>
                                 (need_result_buf((size+3)/4*
                                                   அளவு(s_vpi_vecval),
                                                  RBUF_VAL));
              uint_மதிப்பு = 0;
              vecp = vp->value.vector;
              vecp->aval = vecp->bval = 0;
	      க்கு(unsigned i=0; i<size; i += 1){
		  vecp->aval |= மதிப்பு_2[i] << uint_மதிப்பு*8;
		  uint_மதிப்பு += 1;
		  எனில் (uint_மதிப்பு > 3) {
		      uint_மதிப்பு = 0;
		      vecp += 1;
		      vecp->aval = vecp->bval = 0;
		  }
	      }
	      நிறுத்து;


	  இயல்பு:
	    fprintf(stderr, "ERROR (vpi_const.cc): vp->format: %d\n",
	            (முழு)vp->format);
	    ப்ரதிஜ்ஞா(0);

	    vp->format = vpiSuppressVal;
	    நிறுத்து;
      }
}


கட்டமைப்பு __vpiStringConstTEMP : பொது __vpiStringConst {
      explicit உள்வரி __vpiStringConstTEMP(எழுத்து*v) : __vpiStringConst(v) { }
      free_object_fun_t free_object_fun(வெறுமை) override;
};

நிலையான முழு free_temp_string(vpiHandle obj)
{
      கட்டமைப்பு __vpiStringConstTEMP*rfp = dynamic_cast<__vpiStringConstTEMP*>(obj);
      delete rfp;
      திரும்பு 1;
}

__vpiHandle::free_object_fun_t __vpiStringConstTEMP::free_object_fun(வெறுமை)
{ திரும்பு &free_temp_string; }

vpiHandle vpip_make_string_const(எழுத்து*text, தருக்க persistent_flag)
{
      __vpiStringConst*obj;

      obj = persistent_flag? புதிய __vpiStringConst(text) : புதிய __vpiStringConstTEMP(text);

      திரும்பு obj;
}


வகுப்பு __vpiStringParam  : பொது __vpiStringConst {
    பொது:
      __vpiStringParam(__vpiScope* scope, எழுத்து*txt, மாறிலி எழுத்து*name,
                       தருக்க local_flag, long file_idx, long lineno);
      ~__vpiStringParam() override;
      முழு get_type_code(வெறுமை) மாறிலி override;
      முழு vpi_get(முழு code) override;
      எழுத்து*vpi_get_str(முழு code) override;
      vpiHandle vpi_handle(முழு code) override;

    தனிப்பட்ட:
      __vpiScope* scope_;
      மாறிலி எழுத்து*basename_;
      தருக்க       local_flag_;
      unsigned   file_idx_;
      unsigned   lineno_;
};

உள்வரி __vpiStringParam::__vpiStringParam(__vpiScope* scope, எழுத்து*txt, மாறிலி எழுத்து*nam,
                                          தருக்க local_flag, long file_idx, long lineno)
: __vpiStringConst(txt)
{
      scope_ = scope;
      basename_ = nam;
      local_flag_ = local_flag;
      ப்ரதிஜ்ஞா(file_idx >= 0 && file_idx <= INT_MAX);
      file_idx_ = (unsigned) file_idx;
      ப்ரதிஜ்ஞா(lineno >= 0 && lineno <= INT_MAX);
      lineno_ = (unsigned) lineno;
}

__vpiStringParam::~__vpiStringParam()
{
      delete[]basename_;
}

முழு __vpiStringParam::get_type_code(வெறுமை) மாறிலி
{ திரும்பு vpiParameter; }

முழு __vpiStringParam::vpi_get(முழு code)
{
    தேர்வு (code) {
       நிலை vpi_வரி_No :
         திரும்பு lineno_;

       நிலை vpiLocalParam :
         திரும்பு local_flag_;

       இயல்பு :
         திரும்பு __vpiStringConst::vpi_get(code);
    }
}


எழுத்து*__vpiStringParam::vpi_get_str(முழு code)
{
      எனில் (code == vpiFile) {
	    திரும்பு simple_set_rbuf_str(file_names[file_idx_]);
      }

      திரும்பு generic_get_str(code, scope_, basename_, வெற்று);
}


vpiHandle __vpiStringParam::vpi_handle(முழு code)
{
      தேர்வு (code) {
	  நிலை vpiScope:
	    திரும்பு scope_;

	  நிலை vpiModule:
	    திரும்பு vpip_module(scope_);

	  இயல்பு:
	    திரும்பு 0;
      }
}

vpiHandle vpip_make_string_param(மாறிலி எழுத்து*name, எழுத்து*text,
                                 தருக்க local_flag, long file_idx, long lineno)
{
      __vpiStringParam*obj = புதிய __vpiStringParam(vpip_peek_current_scope(), text, name,
                                                  local_flag, file_idx, lineno);

      திரும்பு obj;
}



உள்வரி __vpiBinaryConst::__vpiBinaryConst()
: signed_flag(0), sized_flag(0)
{ }

முழு __vpiBinaryConst::get_type_code(வெறுமை) மாறிலி
{ திரும்பு vpiConstant; }

முழு __vpiBinaryConst::vpi_get(முழு code)
{
      தேர்வு (code) {
	  நிலை vpiConstType:
	    திரும்பு vpi_இருமம்_Const;

	  நிலை vpi_வரி_No:
	    திரும்பு 0;  // Not implemented for now!

	  நிலை vpiSigned:
	    திரும்பு signed_flag? 1 : 0;

	  நிலை vpiSize:
	    திரும்பு bits.size();

          நிலை vpiAutomatic:
	    திரும்பு 0;

#எனில் defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          நிலை _vpiFromThr:
	      திரும்பு _vpiNoThr;
#endif

	  இயல்பு:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiBinaryConst\n", code);
	    ப்ரதிஜ்ஞா(0);
	    திரும்பு 0;
      }
}


வெறுமை __vpiBinaryConst::vpi_get_மதிப்பு(p_vpi_மதிப்பு val)
{
      தேர்வு (val->format) {

	  நிலை vpiObjTypeVal:
	  நிலை vpiBinStrVal:
	  நிலை vpiDecStrVal:
	  நிலை vpiOctStrVal:
	  நிலை vpiHexStrVal:
          நிலை vpiScalarVal:
	  நிலை vpiIntVal:
	  நிலை vpiVectorVal:
	  நிலை vpiStringVal:
	  நிலை vpiRealVal:
	    vpip_vec4_get_மதிப்பு(bits, bits.size(), signed_flag, val);
	    நிறுத்து;

	  இயல்பு:
	    fprintf(stderr, "vvp error: format %d not supported "
		    "by vpiBinaryConst\n", (முழு)val->format);
	    val->format = vpiSuppressVal;
	    நிறுத்து;
      }
}

/*
 * Make a VPI constant from a vector string. The string is normally a
 * ASCII string, with each letter a 4-value bit. The first character
 * may be an 's' if the vector is signed.
 */
vpiHandle vpip_make_இருமம்_const(unsigned wid, மாறிலி எழுத்து*bits)
{
      கட்டமைப்பு __vpiBinaryConst*obj = புதிய __vpiBinaryConst;

      மாறிலி எழுத்து*bp = bits;
      எனில் (*bp == 's') {
	    bp += 1;
	    obj->signed_flag = 1;
      }

      obj->bits = vector4_from_text(bp, wid);

      திரும்பு obj;
}

vvp_vector4_t vector4_from_text(மாறிலி எழுத்து*bits, unsigned wid)
{
      vvp_vector4_t res (wid);

      க்கு (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t val = BIT4_0;
	    தேர்வு (bits[wid-idx-1]) {
		நிலை '0':
		  val = BIT4_0;
		  நிறுத்து;
		நிலை '1':
		  val = BIT4_1;
		  நிறுத்து;
		நிலை 'x':
		  val = BIT4_X;
		  நிறுத்து;
		நிலை 'z':
		  val = BIT4_Z;
		  நிறுத்து;
	    }

	    res.set_bit(idx, val);
      }

      திரும்பு res;
}

கட்டமைப்பு __vpiBinaryParam  : பொது __vpiBinaryConst {
      __vpiBinaryParam(__vpiScope*scope, மாறிலி vvp_vector4_t&b, தருக்க is_signed,
                       மாறிலி எழுத்து*name, தருக்க local_flag, long file_idx, long lineno);
      ~__vpiBinaryParam() override;
      முழு get_type_code(வெறுமை) மாறிலி override;
      முழு vpi_get(முழு code) override;
      எழுத்து*vpi_get_str(முழு code) override;
      vpiHandle vpi_handle(முழு code) override;

    தனிப்பட்ட:
      __vpiScope*scope_;
      மாறிலி எழுத்து*basename_;
      தருக்க     local_flag_;
      unsigned file_idx_;
      unsigned lineno_;
};

உள்வரி __vpiBinaryParam::__vpiBinaryParam(__vpiScope*scope, மாறிலி vvp_vector4_t&b,
                                          தருக்க is_signed, மாறிலி எழுத்து*nam,
                                          தருக்க local_flag, long file_idx, long lineno)
{
      scope_ = scope;
      bits = b;
      signed_flag = is_signed ? 1 : 0;
      sized_flag = 0;
      basename_ = nam;
      local_flag_ = local_flag;
      ப்ரதிஜ்ஞா(file_idx >= 0 && file_idx <= INT_MAX);
      file_idx_ = (unsigned) file_idx;
      ப்ரதிஜ்ஞா(lineno >= 0 && lineno <= INT_MAX);
      lineno_ = (unsigned) lineno;
}

__vpiBinaryParam::~__vpiBinaryParam()
{
      delete[]basename_;
}

முழு __vpiBinaryParam::get_type_code(வெறுமை) மாறிலி
{ திரும்பு vpiParameter; }

முழு __vpiBinaryParam::vpi_get(முழு code)
{
    தேர்வு (code) {
      நிலை vpi_வரி_No :
        திரும்பு lineno_;

      நிலை vpiLocalParam :
        திரும்பு local_flag_;

      இயல்பு :
        திரும்பு __vpiBinaryConst::vpi_get(code);
    }
}

எழுத்து*__vpiBinaryParam::vpi_get_str(முழு code)
{
      எனில் (code == vpiFile)
	    திரும்பு simple_set_rbuf_str(file_names[file_idx_]);

      திரும்பு generic_get_str(code, scope_, basename_, வெற்று);
}


vpiHandle __vpiBinaryParam::vpi_handle(முழு code)
{
      தேர்வு (code) {
	  நிலை vpiScope:
	    திரும்பு scope_;

	  நிலை vpiModule:
	    திரும்பு vpip_module(scope_);

	  இயல்பு:
	    திரும்பு 0;
      }
}


vpiHandle vpip_make_இருமம்_param(மாறிலி எழுத்து*name, மாறிலி vvp_vector4_t&bits,
				 தருக்க signed_flag, தருக்க local_flag,
				 long file_idx, long lineno)
{
      கட்டமைப்பு __vpiBinaryParam*obj = புதிய __vpiBinaryParam(vpip_peek_current_scope(),
                                                         bits, signed_flag, name,
                                                         local_flag, file_idx, lineno);

      திரும்பு obj;
}



__vpiDecConst::__vpiDecConst(முழு val)
{
      value = val;
}

__vpiDecConst::__vpiDecConst(மாறிலி __vpiDecConst&that)
: __vpiHandle(), value(that.value)
{
}


முழு __vpiDecConst::get_type_code(வெறுமை) மாறிலி
{ திரும்பு vpiConstant; }

முழு __vpiDecConst::vpi_get(முழு code)
{
      தேர்வு (code) {
	  நிலை vpiConstType:
	    திரும்பு vpiDecConst;

	  நிலை vpiSigned:
	    திரும்பு 1;

	  நிலை vpiSize:
	    திரும்பு 32;

          நிலை vpiAutomatic:
	    திரும்பு 0;

#எனில் defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          நிலை _vpiFromThr:
	      திரும்பு _vpiNoThr;
#endif

	  இயல்பு:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiDecConst\n", code);
	    ப்ரதிஜ்ஞா(0);
	    திரும்பு 0;
      }
}


வெறுமை __vpiDecConst::vpi_get_மதிப்பு(p_vpi_மதிப்பு vp)
{
      நிலையான மாறிலி size_t RBUF_USE_SIZE = 64 + 1;
      எழுத்து*rbuf = static_cast<எழுத்து *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));
      எழுத்து*cp = rbuf;

      தேர்வு (vp->format) {

	  நிலை vpiObjTypeVal:
	  நிலை vpiIntVal: {
		vp->value.integer = value;
		நிறுத்து;
	  }

          நிலை vpiDecStrVal:
	      snprintf(rbuf, RBUF_USE_SIZE, "%d", value);
	      vp->value.str = rbuf;
	      நிறுத்து;

          நிலை vpiBinStrVal:
	      க்கு(முழு bit=31; bit>=0;bit--){
		  *cp++ = "01"[ (static_cast<unsigned>(value)>>bit)&1 ];
	      }
	      *cp = 0;

	      vp->value.str = rbuf;
	      நிறுத்து;

          நிலை vpiHexStrVal:
	      snprintf(rbuf, RBUF_USE_SIZE, "%08x", value);
	      vp->value.str = rbuf;
	      நிறுத்து;

          நிலை vpiOctStrVal:
	      snprintf(rbuf, RBUF_USE_SIZE, "%011x", value);
	      vp->value.str = rbuf;
	      நிறுத்து;

	  இயல்பு:
	    fprintf(stderr, "vvp error (vpi_const.cc): format %d not supported "
		    "by vpiDecConst\n", (முழு)vp->format);
	    vp->format = vpiSuppressVal;
	    நிறுத்து;
      }
}


உள்வரி __vpiRealConst::__vpiRealConst(இரட்டை val)
: value(val)
{ }

முழு __vpiRealConst::get_type_code(வெறுமை) மாறிலி
{ திரும்பு vpiConstant; }

முழு __vpiRealConst::vpi_get(முழு code)
{
      தேர்வு (code) {
	  நிலை vpi_வரி_No:
	    திரும்பு 0;  // Not implemented for now!

	  நிலை vpiSize:
	    திரும்பு 1;

	  நிலை vpiConstType:
	    திரும்பு vpiRealConst;

	  நிலை vpiSigned:
	    திரும்பு 1;

          நிலை vpiAutomatic:
	    திரும்பு 0;

#எனில் defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          நிலை _vpiFromThr:
	      திரும்பு _vpiNoThr;
#endif

	  இயல்பு:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiRealConst\n", code);
	    ப்ரதிஜ்ஞா(0);
	    திரும்பு 0;
      }
}


வெறுமை __vpiRealConst::vpi_get_மதிப்பு(p_vpi_மதிப்பு val)
{
      vpip_real_get_மதிப்பு(value, val);
}


vpiHandle vpip_make_real_const(இரட்டை value)
{
      __vpiRealConst*obj = புதிய __vpiRealConst(value);
      திரும்பு obj;
}

கட்டமைப்பு __vpiRealParam  : பொது __vpiRealConst {
      __vpiRealParam(__vpiScope* scope, இரட்டை val, மாறிலி எழுத்து*name,
                     தருக்க local_flag, long file_idx, long lineno);
      ~__vpiRealParam() override;
      முழு get_type_code(வெறுமை) மாறிலி override;
      முழு vpi_get(முழு code) override;
      எழுத்து*vpi_get_str(முழு code) override;
      vpiHandle vpi_handle(முழு code) override;

    தனிப்பட்ட:
      __vpiScope* scope_;
      மாறிலி எழுத்து*basename_;
      தருக்க local_flag_;
      unsigned file_idx_;
      unsigned lineno_;
};


உள்வரி __vpiRealParam::__vpiRealParam(__vpiScope* scope, இரட்டை val, மாறிலி எழுத்து*name,
                                      தருக்க local_flag, long file_idx, long lineno)
: __vpiRealConst(val)
{
      scope_ = scope;
      basename_ = name;
      local_flag_ = local_flag;
      ப்ரதிஜ்ஞா(file_idx >= 0 && file_idx <= INT_MAX);
      file_idx_ = (unsigned) file_idx;
      ப்ரதிஜ்ஞா(lineno >= 0 && lineno <= INT_MAX);
      lineno_ = (unsigned) lineno;
}

__vpiRealParam::~__vpiRealParam()
{
      delete[]basename_;
}


முழு __vpiRealParam::get_type_code(வெறுமை) மாறிலி
{ திரும்பு vpiParameter; }

முழு __vpiRealParam::vpi_get(முழு code)
{
    தேர்வு (code) {
      நிலை vpi_வரி_No :
        திரும்பு lineno_;

      நிலை vpiLocalParam :
        திரும்பு local_flag_;

      இயல்பு :
           திரும்பு __vpiRealConst::vpi_get(code);
    }
}

எழுத்து* __vpiRealParam::vpi_get_str(முழு code)
{
      எனில் (code == vpiFile)
            திரும்பு simple_set_rbuf_str(file_names[file_idx_]);

      திரும்பு generic_get_str(code, scope_, basename_, வெற்று);
}

vpiHandle __vpiRealParam::vpi_handle(முழு code)
{
      தேர்வு (code) {
          நிலை vpiScope:
            திரும்பு scope_;

	  நிலை vpiModule:
	    திரும்பு vpip_module(scope_);

          இயல்பு:
            திரும்பு 0;
      }
}


vpiHandle vpip_make_real_param(மாறிலி எழுத்து*name, இரட்டை value,
                               தருக்க local_flag, long file_idx, long lineno)
{
      கட்டமைப்பு __vpiRealParam*obj = புதிய __vpiRealParam(vpip_peek_current_scope(),
                                                     value, name, local_flag,
                                                     file_idx, lineno);
      திரும்பு obj;
}

/*
 * Make a VPI null constant
 */
உள்வரி __vpiNullConst::__vpiNullConst()
{ }

முழு __vpiNullConst::get_type_code(வெறுமை) மாறிலி
{ திரும்பு vpiConstant; }

முழு __vpiNullConst::vpi_get(முழு code)
{
      தேர்வு (code) {
	  நிலை vpi_வரி_No:
	    திரும்பு 0;  // Not implemented for now!

	  நிலை vpiSize:
	    திரும்பு 32;

	  நிலை vpiConstType:
	    திரும்பு vpiNullConst;

	  நிலை vpiSigned:
	    திரும்பு 0;

          நிலை vpiAutomatic:
	    திரும்பு 0;

#எனில் defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          நிலை _vpiFromThr:
	      திரும்பு _vpiNoThr;
#endif

	  இயல்பு:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiNullConst\n", code);
	    ப்ரதிஜ்ஞா(0);
	    திரும்பு 0;
      }
}

வெறுமை __vpiNullConst::vpi_get_மதிப்பு(p_vpi_மதிப்பு val)
{
      நிலையான மாறிலி size_t RBUF_USE_SIZE = 64 + 1;
      எழுத்து*rbuf = static_cast<எழுத்து *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));

      தேர்வு (val->format) {

	  நிலை vpiObjTypeVal:
	    val->format = vpiStringVal;
	    // fall through
	  நிலை vpiBinStrVal:
	  நிலை vpiDecStrVal:
	  நிலை vpiOctStrVal:
	  நிலை vpiHexStrVal:
	  நிலை vpiStringVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "null");
	    val->value.str = rbuf;
	    நிறுத்து;

          நிலை vpiScalarVal:
	    val->value.scalar = vpi0;
	    நிறுத்து;

	  நிலை vpiIntVal:
	    val->value.integer = 0;
	    நிறுத்து;

	  நிலை vpiVectorVal:
	    val->value.vector = static_cast<p_vpi_vecval>
	                        (need_result_buf(2*அளவு(s_vpi_vecval),
	                                         RBUF_VAL));
	    க்கு (unsigned idx = 0; idx < 2; idx += 1) {
		  val->value.vector[idx].aval = 0;
		  val->value.vector[idx].bval = 0;
	    }
	    நிறுத்து;

	  நிலை vpiRealVal:
	    val->value.real = 0.0;
	    நிறுத்து;

	  இயல்பு:
	    fprintf(stderr, "vvp error: format %d not supported "
		    "by vpiNullConst\n", (முழு)val->format);
	    val->format = vpiSuppressVal;
	    நிறுத்து;
      }
}

vpiHandle vpip_make_null_const()
{
      __vpiNullConst*obj = புதிய __vpiNullConst;
      திரும்பு obj;
}

#ifdef CHECK_WITH_VALGRIND
வெறுமை constant_delete(vpiHandle item)
{
      ப்ரதிஜ்ஞா(item->get_type_code() == vpiConstant);
      தேர்வு(vpi_get(vpiConstType, item)) {
	  நிலை vpiStringConst:
	    delete dynamic_cast<__vpiStringConst*>(item);
	    நிறுத்து;
	  நிலை vpiDecConst:
	    delete dynamic_cast<__vpiDecConst*>(item);
	    நிறுத்து;
	  நிலை vpi_இருமம்_Const:
	    delete dynamic_cast<__vpiBinaryConst*>(item);
	    நிறுத்து;
	  நிலை vpiRealConst:
	    delete dynamic_cast<__vpiRealConst*>(item);
	    நிறுத்து;
	  நிலை vpiNullConst:
	    delete dynamic_cast<__vpiNullConst*>(item);
	    நிறுத்து;
	  இயல்பு:
	    ப்ரதிஜ்ஞா(0);
      }
}

வெறுமை parameter_delete(vpiHandle item)
{
      தேர்வு(vpi_get(vpiConstType, item)) {
	  நிலை vpiStringConst: {
	    வகுப்பு __vpiStringParam*rfp = dynamic_cast<__vpiStringParam*>(item);
	    delete rfp;
	    நிறுத்து; }
	  நிலை vpi_இருமம்_Const: {
	    கட்டமைப்பு __vpiBinaryParam*rfp = dynamic_cast<__vpiBinaryParam*>(item);
	    delete rfp;
	    நிறுத்து; }
	  நிலை vpiRealConst: {
	    கட்டமைப்பு __vpiRealParam*rfp = dynamic_cast<__vpiRealParam*>(item);
	    delete rfp;
	    நிறுத்து; }
	  இயல்பு:
	    ப்ரதிஜ்ஞா(0);
      }
}
#endif
