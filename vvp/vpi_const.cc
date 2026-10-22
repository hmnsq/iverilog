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

শ্রেণী __vpiStringConst : সর্বজনীন __vpiHandle {
    সর্বজনীন:
      explicit __vpiStringConst(অক্ষর*val);
      ~__vpiStringConst() override;
      পূর্ণসংখ্যা get_type_code(শূন্য) ধ্রুবক override;
      পূর্ণসংখ্যা vpi_get(পূর্ণসংখ্যা code) override;
      শূন্য vpi_get_মান(p_vpi_মান val) override;

    ব্যক্তিগত:
      শূন্য process_string_();
    ব্যক্তিগত:
      অক্ষর*মান_2;
      size_t মান_len;
};

অন্তঃরেখ __vpiStringConst::__vpiStringConst(অক্ষর*v)
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
      অক্ষর*chr = মান_2;
      অক্ষর*dp = মান_2;

      যখন (*chr) {
	    অক্ষর পরবর্তী_char = *chr;

	      /* Process octal escapes that I might find. */
	    যদি (*chr == '\\') {
		  জন্য (পূর্ণসংখ্যা idx = 1 ;  idx <= 3 ;  idx += 1) {
			প্রতিজ্ঞা(chr[idx] != 0);
			প্রতিজ্ঞা(chr[idx] < '8');
			প্রতিজ্ঞা(chr[idx] >= '0');
			পরবর্তী_char = পরবর্তী_char*8 + chr[idx] - '0';
		  }
		  chr += 3;
	    }
	    *dp++ = পরবর্তী_char;
	    chr += 1;
      }
      *dp = 0;
      মান_len = dp - মান_2;
}

__vpiStringConst::~__vpiStringConst()
{
      delete[] মান_2;
}

পূর্ণসংখ্যা __vpiStringConst::get_type_code(শূন্য) ধ্রুবক
{ ফেরত vpiConstant; }

পূর্ণসংখ্যা __vpiStringConst::vpi_get(পূর্ণসংখ্যা code)
{
      নির্বাচন (code) {
          ক্ষেত্র vpi_আকার:
	    ফেরত মান_len * 8;

          ক্ষেত্র vpiSigned:
	      ফেরত 0;

	  ক্ষেত্র vpiConstType:
	      ফেরত vpiStringConst;

          ক্ষেত্র vpiAutomatic:
	      ফেরত 0;

#যদি defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          ক্ষেত্র _vpiFromThr:
	      ফেরত _vpiNoThr;
#endif

	  পূর্বনির্ধারিত:
	      fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiStringConst\n", code);
	      প্রতিজ্ঞা(0);
	      ফেরত 0;
      }
}

শূন্য __vpiStringConst::vpi_get_মান(p_vpi_মান vp)
{
      unsigned uint_মান;
      p_vpi_vecval vecp;
      unsigned size = মান_len;
      অক্ষর*rbuf = 0;
      অক্ষর*cp;

      নির্বাচন (vp->format) {
	  ক্ষেত্র vpiObjTypeVal:
	      /* String parameters by default have vpiStringVal values. */
	    vp->format = vpiStringVal;
	    // fallthrough
	  ক্ষেত্র vpiStringVal:
	    cp = মান_2;
	    rbuf = static_cast<অক্ষর *>(need_ফলাফল_buf(size + 1, RBUF_VAL));
	    vp->value.str = rbuf;

	    জন্য (unsigned পূর্ণসংখ্যা i = 0; i < size; i++) {
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
	    বিরতি;

          ক্ষেত্র vpiDecStrVal:
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
	    স্থির ধ্রুবক size_t RBUF_USE_আকার = 12;
	    rbuf = static_cast<অক্ষর *>(need_ফলাফল_buf(RBUF_USE_আকার, RBUF_VAL));
	    uint_মান = 0;
	    জন্য(unsigned i=0; i<size; i += 1){
		  uint_মান <<=8;
		  uint_মান += (unsigned অক্ষর)(মান_2[i]);
	    }
	    snprintf(rbuf, RBUF_USE_আকার, "%u", uint_মান);
	    vp->value.str = rbuf;
	    বিরতি;

          ক্ষেত্র vpiBinStrVal:
	      rbuf = static_cast<অক্ষর *>(need_ফলাফল_buf(8 * size + 1, RBUF_VAL));
	      cp = rbuf;
	      জন্য(unsigned i=0; i<size; i += 1){
		  জন্য(পূর্ণসংখ্যা bit=7; bit>=0; bit -= 1){
		      *cp++ = "01"[ (মান_2[i]>>bit)&1 ];
		  }
	      }
	      *cp = 0;
	      vp->value.str = rbuf;
	      বিরতি;

          ক্ষেত্র vpiHexStrVal:
	      rbuf = static_cast<অক্ষর *>(need_ফলাফল_buf(2 * size + 1, RBUF_VAL));
	      cp = rbuf;
	      জন্য(unsigned i=0; i<size; i += 1){
		  জন্য(পূর্ণসংখ্যা nibble=1; nibble>=0; nibble -= 1){
		      *cp++ = "0123456789abcdef"[ (মান_2[i]>>(nibble*4))&15 ];
		  }
	      }
	      *cp = 0;
	      vp->value.str = rbuf;
	      বিরতি;

          ক্ষেত্র vpiOctStrVal:
	      fprintf(stderr, "ERROR (vpi_const.cc): %%o display of constant strings not yet implemented\n");
	      প্রতিজ্ঞা(0);
	      বিরতি;

          ক্ষেত্র vpiIntVal:
	      vp->value.integer = 0;
	      জন্য(unsigned i=0; i<size; i += 1){
		  জন্য(পূর্ণসংখ্যা bit=7; bit>=0; bit -= 1){
		      vp->value.integer <<= 1;
		      vp->value.integer += (মান_2[i]>>bit)&1;
		  }
	      }
	      বিরতি;

          ক্ষেত্র vpiVectorVal:
              vp->value.vector = static_cast<p_vpi_vecval>
                                 (need_ফলাফল_buf((size+3)/4*
                                                   আকার(s_vpi_vecval),
                                                  RBUF_VAL));
              uint_মান = 0;
              vecp = vp->value.vector;
              vecp->aval = vecp->bval = 0;
	      জন্য(unsigned i=0; i<size; i += 1){
		  vecp->aval |= মান_2[i] << uint_মান*8;
		  uint_মান += 1;
		  যদি (uint_মান > 3) {
		      uint_মান = 0;
		      vecp += 1;
		      vecp->aval = vecp->bval = 0;
		  }
	      }
	      বিরতি;


	  পূর্বনির্ধারিত:
	    fprintf(stderr, "ERROR (vpi_const.cc): vp->format: %d\n",
	            (পূর্ণসংখ্যা)vp->format);
	    প্রতিজ্ঞা(0);

	    vp->format = vpiSuppressVal;
	    বিরতি;
      }
}


কাঠামো __vpiStringConstTEMP : সর্বজনীন __vpiStringConst {
      explicit অন্তঃরেখ __vpiStringConstTEMP(অক্ষর*v) : __vpiStringConst(v) { }
      free_object_fun_t free_object_fun(শূন্য) override;
};

স্থির পূর্ণসংখ্যা free_অস্থায়ী_string(vpiHandle obj)
{
      কাঠামো __vpiStringConstTEMP*rfp = dynamic_cast<__vpiStringConstTEMP*>(obj);
      delete rfp;
      ফেরত 1;
}

__vpiHandle::free_object_fun_t __vpiStringConstTEMP::free_object_fun(শূন্য)
{ ফেরত &free_অস্থায়ী_string; }

vpiHandle vpip_make_string_const(অক্ষর*text, যৌক্তিক persistent_flag)
{
      __vpiStringConst*obj;

      obj = persistent_flag? নতুন __vpiStringConst(text) : নতুন __vpiStringConstTEMP(text);

      ফেরত obj;
}


শ্রেণী __vpiStringParam  : সর্বজনীন __vpiStringConst {
    সর্বজনীন:
      __vpiStringParam(__vpiScope* scope, অক্ষর*txt, ধ্রুবক অক্ষর*name,
                       যৌক্তিক local_flag, long file_idx, long lineno);
      ~__vpiStringParam() override;
      পূর্ণসংখ্যা get_type_code(শূন্য) ধ্রুবক override;
      পূর্ণসংখ্যা vpi_get(পূর্ণসংখ্যা code) override;
      অক্ষর*vpi_get_str(পূর্ণসংখ্যা code) override;
      vpiHandle vpi_handle(পূর্ণসংখ্যা code) override;

    ব্যক্তিগত:
      __vpiScope* scope_;
      ধ্রুবক অক্ষর*basename_;
      যৌক্তিক       local_flag_;
      unsigned   file_idx_;
      unsigned   lineno_;
};

অন্তঃরেখ __vpiStringParam::__vpiStringParam(__vpiScope* scope, অক্ষর*txt, ধ্রুবক অক্ষর*nam,
                                          যৌক্তিক local_flag, long file_idx, long lineno)
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

পূর্ণসংখ্যা __vpiStringParam::get_type_code(শূন্য) ধ্রুবক
{ ফেরত vpiParameter; }

পূর্ণসংখ্যা __vpiStringParam::vpi_get(পূর্ণসংখ্যা code)
{
    নির্বাচন (code) {
       ক্ষেত্র vpi_সারি_No :
         ফেরত lineno_;

       ক্ষেত্র vpiLocalParam :
         ফেরত local_flag_;

       পূর্বনির্ধারিত :
         ফেরত __vpiStringConst::vpi_get(code);
    }
}


অক্ষর*__vpiStringParam::vpi_get_str(পূর্ণসংখ্যা code)
{
      যদি (code == vpiFile) {
	    ফেরত simple_set_rbuf_str(file_names[file_idx_]);
      }

      ফেরত generic_get_str(code, scope_, basename_, ফাঁকা);
}


vpiHandle __vpiStringParam::vpi_handle(পূর্ণসংখ্যা code)
{
      নির্বাচন (code) {
	  ক্ষেত্র vpiScope:
	    ফেরত scope_;

	  ক্ষেত্র vpiModule:
	    ফেরত vpip_module(scope_);

	  পূর্বনির্ধারিত:
	    ফেরত 0;
      }
}

vpiHandle vpip_make_string_param(ধ্রুবক অক্ষর*name, অক্ষর*text,
                                 যৌক্তিক local_flag, long file_idx, long lineno)
{
      __vpiStringParam*obj = নতুন __vpiStringParam(vpip_peek_current_scope(), text, name,
                                                  local_flag, file_idx, lineno);

      ফেরত obj;
}



অন্তঃরেখ __vpiBinaryConst::__vpiBinaryConst()
: signed_flag(0), sized_flag(0)
{ }

পূর্ণসংখ্যা __vpiBinaryConst::get_type_code(শূন্য) ধ্রুবক
{ ফেরত vpiConstant; }

পূর্ণসংখ্যা __vpiBinaryConst::vpi_get(পূর্ণসংখ্যা code)
{
      নির্বাচন (code) {
	  ক্ষেত্র vpiConstType:
	    ফেরত vpi_দ্বিমিক_Const;

	  ক্ষেত্র vpi_সারি_No:
	    ফেরত 0;  // Not implemented for now!

	  ক্ষেত্র vpiSigned:
	    ফেরত signed_flag? 1 : 0;

	  ক্ষেত্র vpi_আকার:
	    ফেরত bits.size();

          ক্ষেত্র vpiAutomatic:
	    ফেরত 0;

#যদি defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          ক্ষেত্র _vpiFromThr:
	      ফেরত _vpiNoThr;
#endif

	  পূর্বনির্ধারিত:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiBinaryConst\n", code);
	    প্রতিজ্ঞা(0);
	    ফেরত 0;
      }
}


শূন্য __vpiBinaryConst::vpi_get_মান(p_vpi_মান val)
{
      নির্বাচন (val->format) {

	  ক্ষেত্র vpiObjTypeVal:
	  ক্ষেত্র vpiBinStrVal:
	  ক্ষেত্র vpiDecStrVal:
	  ক্ষেত্র vpiOctStrVal:
	  ক্ষেত্র vpiHexStrVal:
          ক্ষেত্র vpiScalarVal:
	  ক্ষেত্র vpiIntVal:
	  ক্ষেত্র vpiVectorVal:
	  ক্ষেত্র vpiStringVal:
	  ক্ষেত্র vpiRealVal:
	    vpip_vec4_get_মান(bits, bits.size(), signed_flag, val);
	    বিরতি;

	  পূর্বনির্ধারিত:
	    fprintf(stderr, "vvp error: format %d not supported "
		    "by vpiBinaryConst\n", (পূর্ণসংখ্যা)val->format);
	    val->format = vpiSuppressVal;
	    বিরতি;
      }
}

/*
 * Make a VPI constant from a vector string. The string is normally a
 * ASCII string, with each letter a 4-value bit. The first character
 * may be an 's' if the vector is signed.
 */
vpiHandle vpip_make_দ্বিমিক_const(unsigned wid, ধ্রুবক অক্ষর*bits)
{
      কাঠামো __vpiBinaryConst*obj = নতুন __vpiBinaryConst;

      ধ্রুবক অক্ষর*bp = bits;
      যদি (*bp == 's') {
	    bp += 1;
	    obj->signed_flag = 1;
      }

      obj->bits = vector4_from_text(bp, wid);

      ফেরত obj;
}

vvp_vector4_t vector4_from_text(ধ্রুবক অক্ষর*bits, unsigned wid)
{
      vvp_vector4_t res (wid);

      জন্য (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t val = BIT4_0;
	    নির্বাচন (bits[wid-idx-1]) {
		ক্ষেত্র '0':
		  val = BIT4_0;
		  বিরতি;
		ক্ষেত্র '1':
		  val = BIT4_1;
		  বিরতি;
		ক্ষেত্র 'x':
		  val = BIT4_X;
		  বিরতি;
		ক্ষেত্র 'z':
		  val = BIT4_Z;
		  বিরতি;
	    }

	    res.set_bit(idx, val);
      }

      ফেরত res;
}

কাঠামো __vpiBinaryParam  : সর্বজনীন __vpiBinaryConst {
      __vpiBinaryParam(__vpiScope*scope, ধ্রুবক vvp_vector4_t&b, যৌক্তিক is_signed,
                       ধ্রুবক অক্ষর*name, যৌক্তিক local_flag, long file_idx, long lineno);
      ~__vpiBinaryParam() override;
      পূর্ণসংখ্যা get_type_code(শূন্য) ধ্রুবক override;
      পূর্ণসংখ্যা vpi_get(পূর্ণসংখ্যা code) override;
      অক্ষর*vpi_get_str(পূর্ণসংখ্যা code) override;
      vpiHandle vpi_handle(পূর্ণসংখ্যা code) override;

    ব্যক্তিগত:
      __vpiScope*scope_;
      ধ্রুবক অক্ষর*basename_;
      যৌক্তিক     local_flag_;
      unsigned file_idx_;
      unsigned lineno_;
};

অন্তঃরেখ __vpiBinaryParam::__vpiBinaryParam(__vpiScope*scope, ধ্রুবক vvp_vector4_t&b,
                                          যৌক্তিক is_signed, ধ্রুবক অক্ষর*nam,
                                          যৌক্তিক local_flag, long file_idx, long lineno)
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

পূর্ণসংখ্যা __vpiBinaryParam::get_type_code(শূন্য) ধ্রুবক
{ ফেরত vpiParameter; }

পূর্ণসংখ্যা __vpiBinaryParam::vpi_get(পূর্ণসংখ্যা code)
{
    নির্বাচন (code) {
      ক্ষেত্র vpi_সারি_No :
        ফেরত lineno_;

      ক্ষেত্র vpiLocalParam :
        ফেরত local_flag_;

      পূর্বনির্ধারিত :
        ফেরত __vpiBinaryConst::vpi_get(code);
    }
}

অক্ষর*__vpiBinaryParam::vpi_get_str(পূর্ণসংখ্যা code)
{
      যদি (code == vpiFile)
	    ফেরত simple_set_rbuf_str(file_names[file_idx_]);

      ফেরত generic_get_str(code, scope_, basename_, ফাঁকা);
}


vpiHandle __vpiBinaryParam::vpi_handle(পূর্ণসংখ্যা code)
{
      নির্বাচন (code) {
	  ক্ষেত্র vpiScope:
	    ফেরত scope_;

	  ক্ষেত্র vpiModule:
	    ফেরত vpip_module(scope_);

	  পূর্বনির্ধারিত:
	    ফেরত 0;
      }
}


vpiHandle vpip_make_দ্বিমিক_param(ধ্রুবক অক্ষর*name, ধ্রুবক vvp_vector4_t&bits,
				 যৌক্তিক signed_flag, যৌক্তিক local_flag,
				 long file_idx, long lineno)
{
      কাঠামো __vpiBinaryParam*obj = নতুন __vpiBinaryParam(vpip_peek_current_scope(),
                                                         bits, signed_flag, name,
                                                         local_flag, file_idx, lineno);

      ফেরত obj;
}



__vpiDecConst::__vpiDecConst(পূর্ণসংখ্যা val)
{
      value = val;
}

__vpiDecConst::__vpiDecConst(ধ্রুবক __vpiDecConst&that)
: __vpiHandle(), value(that.value)
{
}


পূর্ণসংখ্যা __vpiDecConst::get_type_code(শূন্য) ধ্রুবক
{ ফেরত vpiConstant; }

পূর্ণসংখ্যা __vpiDecConst::vpi_get(পূর্ণসংখ্যা code)
{
      নির্বাচন (code) {
	  ক্ষেত্র vpiConstType:
	    ফেরত vpiDecConst;

	  ক্ষেত্র vpiSigned:
	    ফেরত 1;

	  ক্ষেত্র vpi_আকার:
	    ফেরত 32;

          ক্ষেত্র vpiAutomatic:
	    ফেরত 0;

#যদি defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          ক্ষেত্র _vpiFromThr:
	      ফেরত _vpiNoThr;
#endif

	  পূর্বনির্ধারিত:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiDecConst\n", code);
	    প্রতিজ্ঞা(0);
	    ফেরত 0;
      }
}


শূন্য __vpiDecConst::vpi_get_মান(p_vpi_মান vp)
{
      স্থির ধ্রুবক size_t RBUF_USE_আকার = 64 + 1;
      অক্ষর*rbuf = static_cast<অক্ষর *>(need_ফলাফল_buf(RBUF_USE_আকার, RBUF_VAL));
      অক্ষর*cp = rbuf;

      নির্বাচন (vp->format) {

	  ক্ষেত্র vpiObjTypeVal:
	  ক্ষেত্র vpiIntVal: {
		vp->value.integer = value;
		বিরতি;
	  }

          ক্ষেত্র vpiDecStrVal:
	      snprintf(rbuf, RBUF_USE_আকার, "%d", value);
	      vp->value.str = rbuf;
	      বিরতি;

          ক্ষেত্র vpiBinStrVal:
	      জন্য(পূর্ণসংখ্যা bit=31; bit>=0;bit--){
		  *cp++ = "01"[ (static_cast<unsigned>(value)>>bit)&1 ];
	      }
	      *cp = 0;

	      vp->value.str = rbuf;
	      বিরতি;

          ক্ষেত্র vpiHexStrVal:
	      snprintf(rbuf, RBUF_USE_আকার, "%08x", value);
	      vp->value.str = rbuf;
	      বিরতি;

          ক্ষেত্র vpiOctStrVal:
	      snprintf(rbuf, RBUF_USE_আকার, "%011x", value);
	      vp->value.str = rbuf;
	      বিরতি;

	  পূর্বনির্ধারিত:
	    fprintf(stderr, "vvp error (vpi_const.cc): format %d not supported "
		    "by vpiDecConst\n", (পূর্ণসংখ্যা)vp->format);
	    vp->format = vpiSuppressVal;
	    বিরতি;
      }
}


অন্তঃরেখ __vpiRealConst::__vpiRealConst(দ্বিগুণ val)
: value(val)
{ }

পূর্ণসংখ্যা __vpiRealConst::get_type_code(শূন্য) ধ্রুবক
{ ফেরত vpiConstant; }

পূর্ণসংখ্যা __vpiRealConst::vpi_get(পূর্ণসংখ্যা code)
{
      নির্বাচন (code) {
	  ক্ষেত্র vpi_সারি_No:
	    ফেরত 0;  // Not implemented for now!

	  ক্ষেত্র vpi_আকার:
	    ফেরত 1;

	  ক্ষেত্র vpiConstType:
	    ফেরত vpiRealConst;

	  ক্ষেত্র vpiSigned:
	    ফেরত 1;

          ক্ষেত্র vpiAutomatic:
	    ফেরত 0;

#যদি defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          ক্ষেত্র _vpiFromThr:
	      ফেরত _vpiNoThr;
#endif

	  পূর্বনির্ধারিত:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiRealConst\n", code);
	    প্রতিজ্ঞা(0);
	    ফেরত 0;
      }
}


শূন্য __vpiRealConst::vpi_get_মান(p_vpi_মান val)
{
      vpip_real_get_মান(value, val);
}


vpiHandle vpip_make_real_const(দ্বিগুণ value)
{
      __vpiRealConst*obj = নতুন __vpiRealConst(value);
      ফেরত obj;
}

কাঠামো __vpiRealParam  : সর্বজনীন __vpiRealConst {
      __vpiRealParam(__vpiScope* scope, দ্বিগুণ val, ধ্রুবক অক্ষর*name,
                     যৌক্তিক local_flag, long file_idx, long lineno);
      ~__vpiRealParam() override;
      পূর্ণসংখ্যা get_type_code(শূন্য) ধ্রুবক override;
      পূর্ণসংখ্যা vpi_get(পূর্ণসংখ্যা code) override;
      অক্ষর*vpi_get_str(পূর্ণসংখ্যা code) override;
      vpiHandle vpi_handle(পূর্ণসংখ্যা code) override;

    ব্যক্তিগত:
      __vpiScope* scope_;
      ধ্রুবক অক্ষর*basename_;
      যৌক্তিক local_flag_;
      unsigned file_idx_;
      unsigned lineno_;
};


অন্তঃরেখ __vpiRealParam::__vpiRealParam(__vpiScope* scope, দ্বিগুণ val, ধ্রুবক অক্ষর*name,
                                      যৌক্তিক local_flag, long file_idx, long lineno)
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


পূর্ণসংখ্যা __vpiRealParam::get_type_code(শূন্য) ধ্রুবক
{ ফেরত vpiParameter; }

পূর্ণসংখ্যা __vpiRealParam::vpi_get(পূর্ণসংখ্যা code)
{
    নির্বাচন (code) {
      ক্ষেত্র vpi_সারি_No :
        ফেরত lineno_;

      ক্ষেত্র vpiLocalParam :
        ফেরত local_flag_;

      পূর্বনির্ধারিত :
           ফেরত __vpiRealConst::vpi_get(code);
    }
}

অক্ষর* __vpiRealParam::vpi_get_str(পূর্ণসংখ্যা code)
{
      যদি (code == vpiFile)
            ফেরত simple_set_rbuf_str(file_names[file_idx_]);

      ফেরত generic_get_str(code, scope_, basename_, ফাঁকা);
}

vpiHandle __vpiRealParam::vpi_handle(পূর্ণসংখ্যা code)
{
      নির্বাচন (code) {
          ক্ষেত্র vpiScope:
            ফেরত scope_;

	  ক্ষেত্র vpiModule:
	    ফেরত vpip_module(scope_);

          পূর্বনির্ধারিত:
            ফেরত 0;
      }
}


vpiHandle vpip_make_real_param(ধ্রুবক অক্ষর*name, দ্বিগুণ value,
                               যৌক্তিক local_flag, long file_idx, long lineno)
{
      কাঠামো __vpiRealParam*obj = নতুন __vpiRealParam(vpip_peek_current_scope(),
                                                     value, name, local_flag,
                                                     file_idx, lineno);
      ফেরত obj;
}

/*
 * Make a VPI null constant
 */
অন্তঃরেখ __vpiNullConst::__vpiNullConst()
{ }

পূর্ণসংখ্যা __vpiNullConst::get_type_code(শূন্য) ধ্রুবক
{ ফেরত vpiConstant; }

পূর্ণসংখ্যা __vpiNullConst::vpi_get(পূর্ণসংখ্যা code)
{
      নির্বাচন (code) {
	  ক্ষেত্র vpi_সারি_No:
	    ফেরত 0;  // Not implemented for now!

	  ক্ষেত্র vpi_আকার:
	    ফেরত 32;

	  ক্ষেত্র vpiConstType:
	    ফেরত vpiNullConst;

	  ক্ষেত্র vpiSigned:
	    ফেরত 0;

          ক্ষেত্র vpiAutomatic:
	    ফেরত 0;

#যদি defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          ক্ষেত্র _vpiFromThr:
	      ফেরত _vpiNoThr;
#endif

	  পূর্বনির্ধারিত:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiNullConst\n", code);
	    প্রতিজ্ঞা(0);
	    ফেরত 0;
      }
}

শূন্য __vpiNullConst::vpi_get_মান(p_vpi_মান val)
{
      স্থির ধ্রুবক size_t RBUF_USE_আকার = 64 + 1;
      অক্ষর*rbuf = static_cast<অক্ষর *>(need_ফলাফল_buf(RBUF_USE_আকার, RBUF_VAL));

      নির্বাচন (val->format) {

	  ক্ষেত্র vpiObjTypeVal:
	    val->format = vpiStringVal;
	    // fall through
	  ক্ষেত্র vpiBinStrVal:
	  ক্ষেত্র vpiDecStrVal:
	  ক্ষেত্র vpiOctStrVal:
	  ক্ষেত্র vpiHexStrVal:
	  ক্ষেত্র vpiStringVal:
	    snprintf(rbuf, RBUF_USE_আকার, "null");
	    val->value.str = rbuf;
	    বিরতি;

          ক্ষেত্র vpiScalarVal:
	    val->value.scalar = vpi0;
	    বিরতি;

	  ক্ষেত্র vpiIntVal:
	    val->value.integer = 0;
	    বিরতি;

	  ক্ষেত্র vpiVectorVal:
	    val->value.vector = static_cast<p_vpi_vecval>
	                        (need_ফলাফল_buf(2*আকার(s_vpi_vecval),
	                                         RBUF_VAL));
	    জন্য (unsigned idx = 0; idx < 2; idx += 1) {
		  val->value.vector[idx].aval = 0;
		  val->value.vector[idx].bval = 0;
	    }
	    বিরতি;

	  ক্ষেত্র vpiRealVal:
	    val->value.real = 0.0;
	    বিরতি;

	  পূর্বনির্ধারিত:
	    fprintf(stderr, "vvp error: format %d not supported "
		    "by vpiNullConst\n", (পূর্ণসংখ্যা)val->format);
	    val->format = vpiSuppressVal;
	    বিরতি;
      }
}

vpiHandle vpip_make_null_const()
{
      __vpiNullConst*obj = নতুন __vpiNullConst;
      ফেরত obj;
}

#ifdef CHECK_WITH_VALGRIND
শূন্য constant_delete(vpiHandle item)
{
      প্রতিজ্ঞা(item->get_type_code() == vpiConstant);
      নির্বাচন(vpi_get(vpiConstType, item)) {
	  ক্ষেত্র vpiStringConst:
	    delete dynamic_cast<__vpiStringConst*>(item);
	    বিরতি;
	  ক্ষেত্র vpiDecConst:
	    delete dynamic_cast<__vpiDecConst*>(item);
	    বিরতি;
	  ক্ষেত্র vpi_দ্বিমিক_Const:
	    delete dynamic_cast<__vpiBinaryConst*>(item);
	    বিরতি;
	  ক্ষেত্র vpiRealConst:
	    delete dynamic_cast<__vpiRealConst*>(item);
	    বিরতি;
	  ক্ষেত্র vpiNullConst:
	    delete dynamic_cast<__vpiNullConst*>(item);
	    বিরতি;
	  পূর্বনির্ধারিত:
	    প্রতিজ্ঞা(0);
      }
}

শূন্য parameter_delete(vpiHandle item)
{
      নির্বাচন(vpi_get(vpiConstType, item)) {
	  ক্ষেত্র vpiStringConst: {
	    শ্রেণী __vpiStringParam*rfp = dynamic_cast<__vpiStringParam*>(item);
	    delete rfp;
	    বিরতি; }
	  ক্ষেত্র vpi_দ্বিমিক_Const: {
	    কাঠামো __vpiBinaryParam*rfp = dynamic_cast<__vpiBinaryParam*>(item);
	    delete rfp;
	    বিরতি; }
	  ক্ষেত্র vpiRealConst: {
	    কাঠামো __vpiRealParam*rfp = dynamic_cast<__vpiRealParam*>(item);
	    delete rfp;
	    বিরতি; }
	  পূর্বনির্ধারিত:
	    প্রতিজ্ঞা(0);
      }
}
#endif
