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

বর্গ __vpiStringConst : সার্বজনিক __vpiHandle {
    সার্বজনিক:
      explicit __vpiStringConst(বর্ণ*val);
      ~__vpiStringConst() override;
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব override;
      পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code) override;
      শূন্য vpi_get_মান(p_vpi_মান val) override;

    গুপ্ত:
      শূন্য process_string_();
    গুপ্ত:
      বর্ণ*মান_2;
      size_t মান_len;
};

অন্তঃস্থ __vpiStringConst::__vpiStringConst(বর্ণ*v)
: মান_2(v)
{
      process_string_();
}

/*
 * Strings are described at the level of the vvp source as a string
 * with literal characters or octal escapes. No other escapes are
 * included, they are processed already by the compiler that generated
 * the vvp source.
 */
শূন্য __vpiStringConst::process_string_(শূন্য)
{
      বর্ণ*chr = মান_2;
      বর্ণ*dp = মান_2;

      যাবত্ (*chr) {
	    বর্ণ next_char = *chr;

	      /* Process octal escapes that I might find. */
	    যদি (*chr == '\\') {
		  প্রতি (পূর্ণাঙ্ক idx = 1 ;  idx <= 3 ;  idx += 1) {
			প্রতিজ্ঞা(chr[idx] != 0);
			প্রতিজ্ঞা(chr[idx] < '8');
			প্রতিজ্ঞা(chr[idx] >= '0');
			next_char = next_char*8 + chr[idx] - '0';
		  }
		  chr += 3;
	    }
	    *dp++ = next_char;
	    chr += 1;
      }
      *dp = 0;
      মান_len = dp - মান_2;
}

__vpiStringConst::~__vpiStringConst()
{
      delete[] মান_2;
}

পূর্ণাঙ্ক __vpiStringConst::get_type_code(শূন্য) ধ্রুব
{ প্রত্যাগম vpiConstant; }

পূর্ণাঙ্ক __vpiStringConst::vpi_get(পূর্ণাঙ্ক code)
{
      বিকল্প (code) {
          পক্ষ vpiSize:
	    প্রত্যাগম মান_len * 8;

          পক্ষ vpiSigned:
	      প্রত্যাগম 0;

	  পক্ষ vpiConstType:
	      প্রত্যাগম vpiStringConst;

          পক্ষ vpiAutomatic:
	      প্রত্যাগম 0;

#যদি defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          পক্ষ _vpiFromThr:
	      প্রত্যাগম _vpiNoThr;
#endif

	  স্বভাব:
	      fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiStringConst\n", code);
	      প্রতিজ্ঞা(0);
	      প্রত্যাগম 0;
      }
}

শূন্য __vpiStringConst::vpi_get_মান(p_vpi_মান vp)
{
      unsigned uint_মান;
      p_vpi_vecval vecp;
      unsigned size = মান_len;
      বর্ণ*rbuf = 0;
      বর্ণ*cp;

      বিকল্প (vp->format) {
	  পক্ষ vpiObjTypeVal:
	      /* String parameters by default have vpiStringVal values. */
	    vp->format = vpiStringVal;
	    // fallthrough
	  পক্ষ vpiStringVal:
	    cp = মান_2;
	    rbuf = static_cast<বর্ণ *>(need_result_buf(size + 1, RBUF_VAL));
	    vp->value.str = rbuf;

	    প্রতি (unsigned পূর্ণাঙ্ক i = 0; i < size; i++) {
		  // Ignore leading null-bytes and replace other null-bytes with space.
		  // The LRM is not entirely clear on how null bytes should be handled.
		  // This is the implementation chosen for iverilog.
		  যদি (*cp)
			*rbuf++ = *cp;
		  অন্যথা যদি (rbuf != vp->value.str)
			*rbuf++ = ' ';

		  cp++;
	    }
	    *rbuf = '\0';
	    ভঙ্গ;

          পক্ষ vpiDecStrVal:
	    // Take the (up to 4) characters of the text, convert the characters
	    // to a numerical value, and convert that value to a decimal string.
	    // For example, the string "A" is ASCII 65, so the resulting string
	    // will be "65". and the string "AB" is 65*256 + 66 == 16706, so
	    // the resulting string is "16706". The "size" is the number of
	    // characters of input text to put to work.
	    যদি (size > 4){
		  // We only support standard integers. Ignore other bytes...
		  size = 4;
		  fprintf(stderr, "Warning (vpi_const.cc): %%d on constant strings only looks "
			  "at first 4 bytes!\n");
	    }
	    স্থির ধ্রুব size_t RBUF_USE_SIZE = 12;
	    rbuf = static_cast<বর্ণ *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));
	    uint_মান = 0;
	    প্রতি(unsigned i=0; i<size; i += 1){
		  uint_মান <<=8;
		  uint_মান += (unsigned বর্ণ)(মান_2[i]);
	    }
	    snprintf(rbuf, RBUF_USE_SIZE, "%u", uint_মান);
	    vp->value.str = rbuf;
	    ভঙ্গ;

          পক্ষ vpiBinStrVal:
	      rbuf = static_cast<বর্ণ *>(need_result_buf(8 * size + 1, RBUF_VAL));
	      cp = rbuf;
	      প্রতি(unsigned i=0; i<size; i += 1){
		  প্রতি(পূর্ণাঙ্ক bit=7; bit>=0; bit -= 1){
		      *cp++ = "01"[ (মান_2[i]>>bit)&1 ];
		  }
	      }
	      *cp = 0;
	      vp->value.str = rbuf;
	      ভঙ্গ;

          পক্ষ vpiHexStrVal:
	      rbuf = static_cast<বর্ণ *>(need_result_buf(2 * size + 1, RBUF_VAL));
	      cp = rbuf;
	      প্রতি(unsigned i=0; i<size; i += 1){
		  প্রতি(পূর্ণাঙ্ক nibble=1; nibble>=0; nibble -= 1){
		      *cp++ = "0123456789abcdef"[ (মান_2[i]>>(nibble*4))&15 ];
		  }
	      }
	      *cp = 0;
	      vp->value.str = rbuf;
	      ভঙ্গ;

          পক্ষ vpiOctStrVal:
	      fprintf(stderr, "ERROR (vpi_const.cc): %%o display of constant strings not yet implemented\n");
	      প্রতিজ্ঞা(0);
	      ভঙ্গ;

          পক্ষ vpiIntVal:
	      vp->value.integer = 0;
	      প্রতি(unsigned i=0; i<size; i += 1){
		  প্রতি(পূর্ণাঙ্ক bit=7; bit>=0; bit -= 1){
		      vp->value.integer <<= 1;
		      vp->value.integer += (মান_2[i]>>bit)&1;
		  }
	      }
	      ভঙ্গ;

          পক্ষ vpiVectorVal:
              vp->value.vector = static_cast<p_vpi_vecval>
                                 (need_result_buf((size+3)/4*
                                                   পরিমাণ(s_vpi_vecval),
                                                  RBUF_VAL));
              uint_মান = 0;
              vecp = vp->value.vector;
              vecp->aval = vecp->bval = 0;
	      প্রতি(unsigned i=0; i<size; i += 1){
		  vecp->aval |= মান_2[i] << uint_মান*8;
		  uint_মান += 1;
		  যদি (uint_মান > 3) {
		      uint_মান = 0;
		      vecp += 1;
		      vecp->aval = vecp->bval = 0;
		  }
	      }
	      ভঙ্গ;


	  স্বভাব:
	    fprintf(stderr, "ERROR (vpi_const.cc): vp->format: %d\n",
	            (পূর্ণাঙ্ক)vp->format);
	    প্রতিজ্ঞা(0);

	    vp->format = vpiSuppressVal;
	    ভঙ্গ;
      }
}


সংরচনা __vpiStringConstTEMP : সার্বজনিক __vpiStringConst {
      explicit অন্তঃস্থ __vpiStringConstTEMP(বর্ণ*v) : __vpiStringConst(v) { }
      free_object_fun_t free_object_fun(শূন্য) override;
};

স্থির পূর্ণাঙ্ক free_temp_string(vpiHandle obj)
{
      সংরচনা __vpiStringConstTEMP*rfp = dynamic_cast<__vpiStringConstTEMP*>(obj);
      delete rfp;
      প্রত্যাগম 1;
}

__vpiHandle::free_object_fun_t __vpiStringConstTEMP::free_object_fun(শূন্য)
{ প্রত্যাগম &free_temp_string; }

vpiHandle vpip_make_string_const(বর্ণ*text, তার্কিক persistent_flag)
{
      __vpiStringConst*obj;

      obj = persistent_flag? নূতন __vpiStringConst(text) : নূতন __vpiStringConstTEMP(text);

      প্রত্যাগম obj;
}


বর্গ __vpiStringParam  : সার্বজনিক __vpiStringConst {
    সার্বজনিক:
      __vpiStringParam(__vpiScope* scope, বর্ণ*txt, ধ্রুব বর্ণ*name,
                       তার্কিক local_flag, long file_idx, long lineno);
      ~__vpiStringParam() override;
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব override;
      পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code) override;
      বর্ণ*vpi_get_str(পূর্ণাঙ্ক code) override;
      vpiHandle vpi_handle(পূর্ণাঙ্ক code) override;

    গুপ্ত:
      __vpiScope* scope_;
      ধ্রুব বর্ণ*basename_;
      তার্কিক       local_flag_;
      unsigned   file_idx_;
      unsigned   lineno_;
};

অন্তঃস্থ __vpiStringParam::__vpiStringParam(__vpiScope* scope, বর্ণ*txt, ধ্রুব বর্ণ*nam,
                                          তার্কিক local_flag, long file_idx, long lineno)
: __vpiStringConst(txt)
{
      scope_ = scope;
      basename_ = nam;
      local_flag_ = local_flag;
      প্রতিজ্ঞা(file_idx >= 0 && file_idx <= INT_MAX);
      file_idx_ = (unsigned) file_idx;
      প্রতিজ্ঞা(lineno >= 0 && lineno <= INT_MAX);
      lineno_ = (unsigned) lineno;
}

__vpiStringParam::~__vpiStringParam()
{
      delete[]basename_;
}

পূর্ণাঙ্ক __vpiStringParam::get_type_code(শূন্য) ধ্রুব
{ প্রত্যাগম vpiParameter; }

পূর্ণাঙ্ক __vpiStringParam::vpi_get(পূর্ণাঙ্ক code)
{
    বিকল্প (code) {
       পক্ষ vpi_সারি_No :
         প্রত্যাগম lineno_;

       পক্ষ vpiLocalParam :
         প্রত্যাগম local_flag_;

       স্বভাব :
         প্রত্যাগম __vpiStringConst::vpi_get(code);
    }
}


বর্ণ*__vpiStringParam::vpi_get_str(পূর্ণাঙ্ক code)
{
      যদি (code == vpiFile) {
	    প্রত্যাগম simple_set_rbuf_str(file_names[file_idx_]);
      }

      প্রত্যাগম generic_get_str(code, scope_, basename_, রিক্ত);
}


vpiHandle __vpiStringParam::vpi_handle(পূর্ণাঙ্ক code)
{
      বিকল্প (code) {
	  পক্ষ vpiScope:
	    প্রত্যাগম scope_;

	  পক্ষ vpiModule:
	    প্রত্যাগম vpip_module(scope_);

	  স্বভাব:
	    প্রত্যাগম 0;
      }
}

vpiHandle vpip_make_string_param(ধ্রুব বর্ণ*name, বর্ণ*text,
                                 তার্কিক local_flag, long file_idx, long lineno)
{
      __vpiStringParam*obj = নূতন __vpiStringParam(vpip_peek_current_scope(), text, name,
                                                  local_flag, file_idx, lineno);

      প্রত্যাগম obj;
}



অন্তঃস্থ __vpiBinaryConst::__vpiBinaryConst()
: signed_flag(0), sized_flag(0)
{ }

পূর্ণাঙ্ক __vpiBinaryConst::get_type_code(শূন্য) ধ্রুব
{ প্রত্যাগম vpiConstant; }

পূর্ণাঙ্ক __vpiBinaryConst::vpi_get(পূর্ণাঙ্ক code)
{
      বিকল্প (code) {
	  পক্ষ vpiConstType:
	    প্রত্যাগম vpi_দ্বিমিক_Const;

	  পক্ষ vpi_সারি_No:
	    প্রত্যাগম 0;  // Not implemented for now!

	  পক্ষ vpiSigned:
	    প্রত্যাগম signed_flag? 1 : 0;

	  পক্ষ vpiSize:
	    প্রত্যাগম bits.size();

          পক্ষ vpiAutomatic:
	    প্রত্যাগম 0;

#যদি defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          পক্ষ _vpiFromThr:
	      প্রত্যাগম _vpiNoThr;
#endif

	  স্বভাব:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiBinaryConst\n", code);
	    প্রতিজ্ঞা(0);
	    প্রত্যাগম 0;
      }
}


শূন্য __vpiBinaryConst::vpi_get_মান(p_vpi_মান val)
{
      বিকল্প (val->format) {

	  পক্ষ vpiObjTypeVal:
	  পক্ষ vpiBinStrVal:
	  পক্ষ vpiDecStrVal:
	  পক্ষ vpiOctStrVal:
	  পক্ষ vpiHexStrVal:
          পক্ষ vpiScalarVal:
	  পক্ষ vpiIntVal:
	  পক্ষ vpiVectorVal:
	  পক্ষ vpiStringVal:
	  পক্ষ vpiRealVal:
	    vpip_vec4_get_মান(bits, bits.size(), signed_flag, val);
	    ভঙ্গ;

	  স্বভাব:
	    fprintf(stderr, "vvp error: format %d not supported "
		    "by vpiBinaryConst\n", (পূর্ণাঙ্ক)val->format);
	    val->format = vpiSuppressVal;
	    ভঙ্গ;
      }
}

/*
 * Make a VPI constant from a vector string. The string is normally a
 * ASCII string, with each letter a 4-value bit. The first character
 * may be an 's' if the vector is signed.
 */
vpiHandle vpip_make_দ্বিমিক_const(unsigned wid, ধ্রুব বর্ণ*bits)
{
      সংরচনা __vpiBinaryConst*obj = নূতন __vpiBinaryConst;

      ধ্রুব বর্ণ*bp = bits;
      যদি (*bp == 's') {
	    bp += 1;
	    obj->signed_flag = 1;
      }

      obj->bits = vector4_from_text(bp, wid);

      প্রত্যাগম obj;
}

vvp_vector4_t vector4_from_text(ধ্রুব বর্ণ*bits, unsigned wid)
{
      vvp_vector4_t res (wid);

      প্রতি (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t val = BIT4_0;
	    বিকল্প (bits[wid-idx-1]) {
		পক্ষ '0':
		  val = BIT4_0;
		  ভঙ্গ;
		পক্ষ '1':
		  val = BIT4_1;
		  ভঙ্গ;
		পক্ষ 'x':
		  val = BIT4_X;
		  ভঙ্গ;
		পক্ষ 'z':
		  val = BIT4_Z;
		  ভঙ্গ;
	    }

	    res.set_bit(idx, val);
      }

      প্রত্যাগম res;
}

সংরচনা __vpiBinaryParam  : সার্বজনিক __vpiBinaryConst {
      __vpiBinaryParam(__vpiScope*scope, ধ্রুব vvp_vector4_t&b, তার্কিক is_signed,
                       ধ্রুব বর্ণ*name, তার্কিক local_flag, long file_idx, long lineno);
      ~__vpiBinaryParam() override;
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব override;
      পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code) override;
      বর্ণ*vpi_get_str(পূর্ণাঙ্ক code) override;
      vpiHandle vpi_handle(পূর্ণাঙ্ক code) override;

    গুপ্ত:
      __vpiScope*scope_;
      ধ্রুব বর্ণ*basename_;
      তার্কিক     local_flag_;
      unsigned file_idx_;
      unsigned lineno_;
};

অন্তঃস্থ __vpiBinaryParam::__vpiBinaryParam(__vpiScope*scope, ধ্রুব vvp_vector4_t&b,
                                          তার্কিক is_signed, ধ্রুব বর্ণ*nam,
                                          তার্কিক local_flag, long file_idx, long lineno)
{
      scope_ = scope;
      bits = b;
      signed_flag = is_signed ? 1 : 0;
      sized_flag = 0;
      basename_ = nam;
      local_flag_ = local_flag;
      প্রতিজ্ঞা(file_idx >= 0 && file_idx <= INT_MAX);
      file_idx_ = (unsigned) file_idx;
      প্রতিজ্ঞা(lineno >= 0 && lineno <= INT_MAX);
      lineno_ = (unsigned) lineno;
}

__vpiBinaryParam::~__vpiBinaryParam()
{
      delete[]basename_;
}

পূর্ণাঙ্ক __vpiBinaryParam::get_type_code(শূন্য) ধ্রুব
{ প্রত্যাগম vpiParameter; }

পূর্ণাঙ্ক __vpiBinaryParam::vpi_get(পূর্ণাঙ্ক code)
{
    বিকল্প (code) {
      পক্ষ vpi_সারি_No :
        প্রত্যাগম lineno_;

      পক্ষ vpiLocalParam :
        প্রত্যাগম local_flag_;

      স্বভাব :
        প্রত্যাগম __vpiBinaryConst::vpi_get(code);
    }
}

বর্ণ*__vpiBinaryParam::vpi_get_str(পূর্ণাঙ্ক code)
{
      যদি (code == vpiFile)
	    প্রত্যাগম simple_set_rbuf_str(file_names[file_idx_]);

      প্রত্যাগম generic_get_str(code, scope_, basename_, রিক্ত);
}


vpiHandle __vpiBinaryParam::vpi_handle(পূর্ণাঙ্ক code)
{
      বিকল্প (code) {
	  পক্ষ vpiScope:
	    প্রত্যাগম scope_;

	  পক্ষ vpiModule:
	    প্রত্যাগম vpip_module(scope_);

	  স্বভাব:
	    প্রত্যাগম 0;
      }
}


vpiHandle vpip_make_দ্বিমিক_param(ধ্রুব বর্ণ*name, ধ্রুব vvp_vector4_t&bits,
				 তার্কিক signed_flag, তার্কিক local_flag,
				 long file_idx, long lineno)
{
      সংরচনা __vpiBinaryParam*obj = নূতন __vpiBinaryParam(vpip_peek_current_scope(),
                                                         bits, signed_flag, name,
                                                         local_flag, file_idx, lineno);

      প্রত্যাগম obj;
}



__vpiDecConst::__vpiDecConst(পূর্ণাঙ্ক val)
{
      value = val;
}

__vpiDecConst::__vpiDecConst(ধ্রুব __vpiDecConst&that)
: __vpiHandle(), value(that.value)
{
}


পূর্ণাঙ্ক __vpiDecConst::get_type_code(শূন্য) ধ্রুব
{ প্রত্যাগম vpiConstant; }

পূর্ণাঙ্ক __vpiDecConst::vpi_get(পূর্ণাঙ্ক code)
{
      বিকল্প (code) {
	  পক্ষ vpiConstType:
	    প্রত্যাগম vpiDecConst;

	  পক্ষ vpiSigned:
	    প্রত্যাগম 1;

	  পক্ষ vpiSize:
	    প্রত্যাগম 32;

          পক্ষ vpiAutomatic:
	    প্রত্যাগম 0;

#যদি defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          পক্ষ _vpiFromThr:
	      প্রত্যাগম _vpiNoThr;
#endif

	  স্বভাব:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiDecConst\n", code);
	    প্রতিজ্ঞা(0);
	    প্রত্যাগম 0;
      }
}


শূন্য __vpiDecConst::vpi_get_মান(p_vpi_মান vp)
{
      স্থির ধ্রুব size_t RBUF_USE_SIZE = 64 + 1;
      বর্ণ*rbuf = static_cast<বর্ণ *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));
      বর্ণ*cp = rbuf;

      বিকল্প (vp->format) {

	  পক্ষ vpiObjTypeVal:
	  পক্ষ vpiIntVal: {
		vp->value.integer = value;
		ভঙ্গ;
	  }

          পক্ষ vpiDecStrVal:
	      snprintf(rbuf, RBUF_USE_SIZE, "%d", value);
	      vp->value.str = rbuf;
	      ভঙ্গ;

          পক্ষ vpiBinStrVal:
	      প্রতি(পূর্ণাঙ্ক bit=31; bit>=0;bit--){
		  *cp++ = "01"[ (static_cast<unsigned>(value)>>bit)&1 ];
	      }
	      *cp = 0;

	      vp->value.str = rbuf;
	      ভঙ্গ;

          পক্ষ vpiHexStrVal:
	      snprintf(rbuf, RBUF_USE_SIZE, "%08x", value);
	      vp->value.str = rbuf;
	      ভঙ্গ;

          পক্ষ vpiOctStrVal:
	      snprintf(rbuf, RBUF_USE_SIZE, "%011x", value);
	      vp->value.str = rbuf;
	      ভঙ্গ;

	  স্বভাব:
	    fprintf(stderr, "vvp error (vpi_const.cc): format %d not supported "
		    "by vpiDecConst\n", (পূর্ণাঙ্ক)vp->format);
	    vp->format = vpiSuppressVal;
	    ভঙ্গ;
      }
}


অন্তঃস্থ __vpiRealConst::__vpiRealConst(দ্বিগুণ val)
: value(val)
{ }

পূর্ণাঙ্ক __vpiRealConst::get_type_code(শূন্য) ধ্রুব
{ প্রত্যাগম vpiConstant; }

পূর্ণাঙ্ক __vpiRealConst::vpi_get(পূর্ণাঙ্ক code)
{
      বিকল্প (code) {
	  পক্ষ vpi_সারি_No:
	    প্রত্যাগম 0;  // Not implemented for now!

	  পক্ষ vpiSize:
	    প্রত্যাগম 1;

	  পক্ষ vpiConstType:
	    প্রত্যাগম vpiRealConst;

	  পক্ষ vpiSigned:
	    প্রত্যাগম 1;

          পক্ষ vpiAutomatic:
	    প্রত্যাগম 0;

#যদি defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          পক্ষ _vpiFromThr:
	      প্রত্যাগম _vpiNoThr;
#endif

	  স্বভাব:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiRealConst\n", code);
	    প্রতিজ্ঞা(0);
	    প্রত্যাগম 0;
      }
}


শূন্য __vpiRealConst::vpi_get_মান(p_vpi_মান val)
{
      vpip_real_get_মান(value, val);
}


vpiHandle vpip_make_real_const(দ্বিগুণ value)
{
      __vpiRealConst*obj = নূতন __vpiRealConst(value);
      প্রত্যাগম obj;
}

সংরচনা __vpiRealParam  : সার্বজনিক __vpiRealConst {
      __vpiRealParam(__vpiScope* scope, দ্বিগুণ val, ধ্রুব বর্ণ*name,
                     তার্কিক local_flag, long file_idx, long lineno);
      ~__vpiRealParam() override;
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব override;
      পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code) override;
      বর্ণ*vpi_get_str(পূর্ণাঙ্ক code) override;
      vpiHandle vpi_handle(পূর্ণাঙ্ক code) override;

    গুপ্ত:
      __vpiScope* scope_;
      ধ্রুব বর্ণ*basename_;
      তার্কিক local_flag_;
      unsigned file_idx_;
      unsigned lineno_;
};


অন্তঃস্থ __vpiRealParam::__vpiRealParam(__vpiScope* scope, দ্বিগুণ val, ধ্রুব বর্ণ*name,
                                      তার্কিক local_flag, long file_idx, long lineno)
: __vpiRealConst(val)
{
      scope_ = scope;
      basename_ = name;
      local_flag_ = local_flag;
      প্রতিজ্ঞা(file_idx >= 0 && file_idx <= INT_MAX);
      file_idx_ = (unsigned) file_idx;
      প্রতিজ্ঞা(lineno >= 0 && lineno <= INT_MAX);
      lineno_ = (unsigned) lineno;
}

__vpiRealParam::~__vpiRealParam()
{
      delete[]basename_;
}


পূর্ণাঙ্ক __vpiRealParam::get_type_code(শূন্য) ধ্রুব
{ প্রত্যাগম vpiParameter; }

পূর্ণাঙ্ক __vpiRealParam::vpi_get(পূর্ণাঙ্ক code)
{
    বিকল্প (code) {
      পক্ষ vpi_সারি_No :
        প্রত্যাগম lineno_;

      পক্ষ vpiLocalParam :
        প্রত্যাগম local_flag_;

      স্বভাব :
           প্রত্যাগম __vpiRealConst::vpi_get(code);
    }
}

বর্ণ* __vpiRealParam::vpi_get_str(পূর্ণাঙ্ক code)
{
      যদি (code == vpiFile)
            প্রত্যাগম simple_set_rbuf_str(file_names[file_idx_]);

      প্রত্যাগম generic_get_str(code, scope_, basename_, রিক্ত);
}

vpiHandle __vpiRealParam::vpi_handle(পূর্ণাঙ্ক code)
{
      বিকল্প (code) {
          পক্ষ vpiScope:
            প্রত্যাগম scope_;

	  পক্ষ vpiModule:
	    প্রত্যাগম vpip_module(scope_);

          স্বভাব:
            প্রত্যাগম 0;
      }
}


vpiHandle vpip_make_real_param(ধ্রুব বর্ণ*name, দ্বিগুণ value,
                               তার্কিক local_flag, long file_idx, long lineno)
{
      সংরচনা __vpiRealParam*obj = নূতন __vpiRealParam(vpip_peek_current_scope(),
                                                     value, name, local_flag,
                                                     file_idx, lineno);
      প্রত্যাগম obj;
}

/*
 * Make a VPI null constant
 */
অন্তঃস্থ __vpiNullConst::__vpiNullConst()
{ }

পূর্ণাঙ্ক __vpiNullConst::get_type_code(শূন্য) ধ্রুব
{ প্রত্যাগম vpiConstant; }

পূর্ণাঙ্ক __vpiNullConst::vpi_get(পূর্ণাঙ্ক code)
{
      বিকল্প (code) {
	  পক্ষ vpi_সারি_No:
	    প্রত্যাগম 0;  // Not implemented for now!

	  পক্ষ vpiSize:
	    প্রত্যাগম 32;

	  পক্ষ vpiConstType:
	    প্রত্যাগম vpiNullConst;

	  পক্ষ vpiSigned:
	    প্রত্যাগম 0;

          পক্ষ vpiAutomatic:
	    প্রত্যাগম 0;

#যদি defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          পক্ষ _vpiFromThr:
	      প্রত্যাগম _vpiNoThr;
#endif

	  স্বভাব:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiNullConst\n", code);
	    প্রতিজ্ঞা(0);
	    প্রত্যাগম 0;
      }
}

শূন্য __vpiNullConst::vpi_get_মান(p_vpi_মান val)
{
      স্থির ধ্রুব size_t RBUF_USE_SIZE = 64 + 1;
      বর্ণ*rbuf = static_cast<বর্ণ *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));

      বিকল্প (val->format) {

	  পক্ষ vpiObjTypeVal:
	    val->format = vpiStringVal;
	    // fall through
	  পক্ষ vpiBinStrVal:
	  পক্ষ vpiDecStrVal:
	  পক্ষ vpiOctStrVal:
	  পক্ষ vpiHexStrVal:
	  পক্ষ vpiStringVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "null");
	    val->value.str = rbuf;
	    ভঙ্গ;

          পক্ষ vpiScalarVal:
	    val->value.scalar = vpi0;
	    ভঙ্গ;

	  পক্ষ vpiIntVal:
	    val->value.integer = 0;
	    ভঙ্গ;

	  পক্ষ vpiVectorVal:
	    val->value.vector = static_cast<p_vpi_vecval>
	                        (need_result_buf(2*পরিমাণ(s_vpi_vecval),
	                                         RBUF_VAL));
	    প্রতি (unsigned idx = 0; idx < 2; idx += 1) {
		  val->value.vector[idx].aval = 0;
		  val->value.vector[idx].bval = 0;
	    }
	    ভঙ্গ;

	  পক্ষ vpiRealVal:
	    val->value.real = 0.0;
	    ভঙ্গ;

	  স্বভাব:
	    fprintf(stderr, "vvp error: format %d not supported "
		    "by vpiNullConst\n", (পূর্ণাঙ্ক)val->format);
	    val->format = vpiSuppressVal;
	    ভঙ্গ;
      }
}

vpiHandle vpip_make_null_const()
{
      __vpiNullConst*obj = নূতন __vpiNullConst;
      প্রত্যাগম obj;
}

#ifdef CHECK_WITH_VALGRIND
শূন্য constant_delete(vpiHandle item)
{
      প্রতিজ্ঞা(item->get_type_code() == vpiConstant);
      বিকল্প(vpi_get(vpiConstType, item)) {
	  পক্ষ vpiStringConst:
	    delete dynamic_cast<__vpiStringConst*>(item);
	    ভঙ্গ;
	  পক্ষ vpiDecConst:
	    delete dynamic_cast<__vpiDecConst*>(item);
	    ভঙ্গ;
	  পক্ষ vpi_দ্বিমিক_Const:
	    delete dynamic_cast<__vpiBinaryConst*>(item);
	    ভঙ্গ;
	  পক্ষ vpiRealConst:
	    delete dynamic_cast<__vpiRealConst*>(item);
	    ভঙ্গ;
	  পক্ষ vpiNullConst:
	    delete dynamic_cast<__vpiNullConst*>(item);
	    ভঙ্গ;
	  স্বভাব:
	    প্রতিজ্ঞা(0);
      }
}

শূন্য parameter_delete(vpiHandle item)
{
      বিকল্প(vpi_get(vpiConstType, item)) {
	  পক্ষ vpiStringConst: {
	    বর্গ __vpiStringParam*rfp = dynamic_cast<__vpiStringParam*>(item);
	    delete rfp;
	    ভঙ্গ; }
	  পক্ষ vpi_দ্বিমিক_Const: {
	    সংরচনা __vpiBinaryParam*rfp = dynamic_cast<__vpiBinaryParam*>(item);
	    delete rfp;
	    ভঙ্গ; }
	  পক্ষ vpiRealConst: {
	    সংরচনা __vpiRealParam*rfp = dynamic_cast<__vpiRealParam*>(item);
	    delete rfp;
	    ভঙ্গ; }
	  স্বভাব:
	    প্রতিজ্ঞা(0);
      }
}
#endif
