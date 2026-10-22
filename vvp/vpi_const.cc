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

ଶ୍ରେଣୀ __vpiStringConst : ସର୍ବସାଧାରଣ __vpiHandle {
    ସର୍ବସାଧାରଣ:
      explicit __vpiStringConst(ଅକ୍ଷର*val);
      ~__vpiStringConst() override;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଶୂନ୍ୟ vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val) override;

    ବ୍ୟକ୍ତିଗତ:
      ଶୂନ୍ୟ process_string_();
    ବ୍ୟକ୍ତିଗତ:
      ଅକ୍ଷର*ମୂଲ୍ୟ_2;
      size_t ମୂଲ୍ୟ_len;
};

ଧାଡ଼ିଭିତରେ __vpiStringConst::__vpiStringConst(ଅକ୍ଷର*v)
: ମୂଲ୍ୟ_2(v)
{
      process_string_();
}

/*
 * Strings are described at the level of the vvp source as a string
 * with literal characters or octal escapes. No other escapes are
 * included, they are processed already by the compiler that generated
 * the vvp source.
 */
ଶୂନ୍ୟ __vpiStringConst::process_string_(ଶୂନ୍ୟ)
{
      ଅକ୍ଷର*chr = ମୂଲ୍ୟ_2;
      ଅକ୍ଷର*dp = ମୂଲ୍ୟ_2;

      ଯେତେବେଳେ (*chr) {
	    ଅକ୍ଷର next_char = *chr;

	      /* Process octal escapes that I might find. */
	    ଯଦି (*chr == '\\') {
		  ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = 1 ;  idx <= 3 ;  idx += 1) {
			ପ୍ରତିଜ୍ଞା(chr[idx] != 0);
			ପ୍ରତିଜ୍ଞା(chr[idx] < '8');
			ପ୍ରତିଜ୍ଞା(chr[idx] >= '0');
			next_char = next_char*8 + chr[idx] - '0';
		  }
		  chr += 3;
	    }
	    *dp++ = next_char;
	    chr += 1;
      }
      *dp = 0;
      ମୂଲ୍ୟ_len = dp - ମୂଲ୍ୟ_2;
}

__vpiStringConst::~__vpiStringConst()
{
      delete[] ମୂଲ୍ୟ_2;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiStringConst::get_type_code(ଶୂନ୍ୟ) ଅଚଳ
{ ଫେରନ୍ତୁ vpiConstant; }

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiStringConst::vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ବାଛ (code) {
          ସ୍ଥିତି vpiSize:
	    ଫେରନ୍ତୁ ମୂଲ୍ୟ_len * 8;

          ସ୍ଥିତି vpiSigned:
	      ଫେରନ୍ତୁ 0;

	  ସ୍ଥିତି vpiConstType:
	      ଫେରନ୍ତୁ vpiStringConst;

          ସ୍ଥିତି vpiAutomatic:
	      ଫେରନ୍ତୁ 0;

#ଯଦି defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          ସ୍ଥିତି _vpiFromThr:
	      ଫେରନ୍ତୁ _vpiNoThr;
#endif

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	      fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiStringConst\n", code);
	      ପ୍ରତିଜ୍ଞା(0);
	      ଫେରନ୍ତୁ 0;
      }
}

ଶୂନ୍ୟ __vpiStringConst::vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ vp)
{
      unsigned uint_ମୂଲ୍ୟ;
      p_vpi_vecval vecp;
      unsigned size = ମୂଲ୍ୟ_len;
      ଅକ୍ଷର*rbuf = 0;
      ଅକ୍ଷର*cp;

      ବାଛ (vp->format) {
	  ସ୍ଥିତି vpiObjTypeVal:
	      /* String parameters by default have vpiStringVal values. */
	    vp->format = vpiStringVal;
	    // fallthrough
	  ସ୍ଥିତି vpiStringVal:
	    cp = ମୂଲ୍ୟ_2;
	    rbuf = static_cast<ଅକ୍ଷର *>(need_result_buf(size + 1, RBUF_VAL));
	    vp->value.str = rbuf;

	    ପାଇଁ (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < size; i++) {
		  // Ignore leading null-bytes and replace other null-bytes with space.
		  // The LRM is not entirely clear on how null bytes should be handled.
		  // This is the implementation chosen for iverilog.
		  ଯଦି (*cp)
			*rbuf++ = *cp;
		  ଅନ୍ୟଥା ଯଦି (rbuf != vp->value.str)
			*rbuf++ = ' ';

		  cp++;
	    }
	    *rbuf = '\0';
	    ଭାଙ୍ଗ;

          ସ୍ଥିତି vpiDecStrVal:
	    // Take the (up to 4) characters of the text, convert the characters
	    // to a numerical value, and convert that value to a decimal string.
	    // For example, the string "A" is ASCII 65, so the resulting string
	    // will be "65". and the string "AB" is 65*256 + 66 == 16706, so
	    // the resulting string is "16706". The "size" is the number of
	    // characters of input text to put to work.
	    ଯଦି (size > 4){
		  // We only support standard integers. Ignore other bytes...
		  size = 4;
		  fprintf(stderr, "Warning (vpi_const.cc): %%d on constant strings only looks "
			  "at first 4 bytes!\n");
	    }
	    ସ୍ଥିର ଅଚଳ size_t RBUF_USE_SIZE = 12;
	    rbuf = static_cast<ଅକ୍ଷର *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));
	    uint_ମୂଲ୍ୟ = 0;
	    ପାଇଁ(unsigned i=0; i<size; i += 1){
		  uint_ମୂଲ୍ୟ <<=8;
		  uint_ମୂଲ୍ୟ += (unsigned ଅକ୍ଷର)(ମୂଲ୍ୟ_2[i]);
	    }
	    snprintf(rbuf, RBUF_USE_SIZE, "%u", uint_ମୂଲ୍ୟ);
	    vp->value.str = rbuf;
	    ଭାଙ୍ଗ;

          ସ୍ଥିତି vpiBinStrVal:
	      rbuf = static_cast<ଅକ୍ଷର *>(need_result_buf(8 * size + 1, RBUF_VAL));
	      cp = rbuf;
	      ପାଇଁ(unsigned i=0; i<size; i += 1){
		  ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା bit=7; bit>=0; bit -= 1){
		      *cp++ = "01"[ (ମୂଲ୍ୟ_2[i]>>bit)&1 ];
		  }
	      }
	      *cp = 0;
	      vp->value.str = rbuf;
	      ଭାଙ୍ଗ;

          ସ୍ଥିତି vpiHexStrVal:
	      rbuf = static_cast<ଅକ୍ଷର *>(need_result_buf(2 * size + 1, RBUF_VAL));
	      cp = rbuf;
	      ପାଇଁ(unsigned i=0; i<size; i += 1){
		  ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା nibble=1; nibble>=0; nibble -= 1){
		      *cp++ = "0123456789abcdef"[ (ମୂଲ୍ୟ_2[i]>>(nibble*4))&15 ];
		  }
	      }
	      *cp = 0;
	      vp->value.str = rbuf;
	      ଭାଙ୍ଗ;

          ସ୍ଥିତି vpiOctStrVal:
	      fprintf(stderr, "ERROR (vpi_const.cc): %%o display of constant strings not yet implemented\n");
	      ପ୍ରତିଜ୍ଞା(0);
	      ଭାଙ୍ଗ;

          ସ୍ଥିତି vpiIntVal:
	      vp->value.integer = 0;
	      ପାଇଁ(unsigned i=0; i<size; i += 1){
		  ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା bit=7; bit>=0; bit -= 1){
		      vp->value.integer <<= 1;
		      vp->value.integer += (ମୂଲ୍ୟ_2[i]>>bit)&1;
		  }
	      }
	      ଭାଙ୍ଗ;

          ସ୍ଥିତି vpiVectorVal:
              vp->value.vector = static_cast<p_vpi_vecval>
                                 (need_result_buf((size+3)/4*
                                                   ଆକାର(s_vpi_vecval),
                                                  RBUF_VAL));
              uint_ମୂଲ୍ୟ = 0;
              vecp = vp->value.vector;
              vecp->aval = vecp->bval = 0;
	      ପାଇଁ(unsigned i=0; i<size; i += 1){
		  vecp->aval |= ମୂଲ୍ୟ_2[i] << uint_ମୂଲ୍ୟ*8;
		  uint_ମୂଲ୍ୟ += 1;
		  ଯଦି (uint_ମୂଲ୍ୟ > 3) {
		      uint_ମୂଲ୍ୟ = 0;
		      vecp += 1;
		      vecp->aval = vecp->bval = 0;
		  }
	      }
	      ଭାଙ୍ଗ;


	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "ERROR (vpi_const.cc): vp->format: %d\n",
	            (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vp->format);
	    ପ୍ରତିଜ୍ଞା(0);

	    vp->format = vpiSuppressVal;
	    ଭାଙ୍ଗ;
      }
}


ସଂରଚନା __vpiStringConstTEMP : ସର୍ବସାଧାରଣ __vpiStringConst {
      explicit ଧାଡ଼ିଭିତରେ __vpiStringConstTEMP(ଅକ୍ଷର*v) : __vpiStringConst(v) { }
      free_object_fun_t free_object_fun(ଶୂନ୍ୟ) override;
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା free_temp_string(vpiHandle obj)
{
      ସଂରଚନା __vpiStringConstTEMP*rfp = dynamic_cast<__vpiStringConstTEMP*>(obj);
      delete rfp;
      ଫେରନ୍ତୁ 1;
}

__vpiHandle::free_object_fun_t __vpiStringConstTEMP::free_object_fun(ଶୂନ୍ୟ)
{ ଫେରନ୍ତୁ &free_temp_string; }

vpiHandle vpip_make_string_const(ଅକ୍ଷର*text, ତାର୍କିକ persistent_flag)
{
      __vpiStringConst*obj;

      obj = persistent_flag? ନୂତନ __vpiStringConst(text) : ନୂତନ __vpiStringConstTEMP(text);

      ଫେରନ୍ତୁ obj;
}


ଶ୍ରେଣୀ __vpiStringParam  : ସର୍ବସାଧାରଣ __vpiStringConst {
    ସର୍ବସାଧାରଣ:
      __vpiStringParam(__vpiScope* scope, ଅକ୍ଷର*txt, ଅଚଳ ଅକ୍ଷର*name,
                       ତାର୍କିକ local_flag, long file_idx, long lineno);
      ~__vpiStringParam() override;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଅକ୍ଷର*vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      vpiHandle vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;

    ବ୍ୟକ୍ତିଗତ:
      __vpiScope* scope_;
      ଅଚଳ ଅକ୍ଷର*basename_;
      ତାର୍କିକ       local_flag_;
      unsigned   file_idx_;
      unsigned   lineno_;
};

ଧାଡ଼ିଭିତରେ __vpiStringParam::__vpiStringParam(__vpiScope* scope, ଅକ୍ଷର*txt, ଅଚଳ ଅକ୍ଷର*nam,
                                          ତାର୍କିକ local_flag, long file_idx, long lineno)
: __vpiStringConst(txt)
{
      scope_ = scope;
      basename_ = nam;
      local_flag_ = local_flag;
      ପ୍ରତିଜ୍ଞା(file_idx >= 0 && file_idx <= INT_MAX);
      file_idx_ = (unsigned) file_idx;
      ପ୍ରତିଜ୍ଞା(lineno >= 0 && lineno <= INT_MAX);
      lineno_ = (unsigned) lineno;
}

__vpiStringParam::~__vpiStringParam()
{
      delete[]basename_;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiStringParam::get_type_code(ଶୂନ୍ୟ) ଅଚଳ
{ ଫେରନ୍ତୁ vpiParameter; }

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiStringParam::vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
    ବାଛ (code) {
       ସ୍ଥିତି vpi_ଧାଡ଼ି_No :
         ଫେରନ୍ତୁ lineno_;

       ସ୍ଥିତି vpiLocalParam :
         ଫେରନ୍ତୁ local_flag_;

       ପୂର୍ବନିର୍ଦ୍ଧାରିତ :
         ଫେରନ୍ତୁ __vpiStringConst::vpi_get(code);
    }
}


ଅକ୍ଷର*__vpiStringParam::vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ଯଦି (code == vpiFile) {
	    ଫେରନ୍ତୁ simple_set_rbuf_str(file_names[file_idx_]);
      }

      ଫେରନ୍ତୁ generic_get_str(code, scope_, basename_, ଖାଲି);
}


vpiHandle __vpiStringParam::vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ବାଛ (code) {
	  ସ୍ଥିତି vpiScope:
	    ଫେରନ୍ତୁ scope_;

	  ସ୍ଥିତି vpiModule:
	    ଫେରନ୍ତୁ vpip_module(scope_);

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଫେରନ୍ତୁ 0;
      }
}

vpiHandle vpip_make_string_param(ଅଚଳ ଅକ୍ଷର*name, ଅକ୍ଷର*text,
                                 ତାର୍କିକ local_flag, long file_idx, long lineno)
{
      __vpiStringParam*obj = ନୂତନ __vpiStringParam(vpip_peek_current_scope(), text, name,
                                                  local_flag, file_idx, lineno);

      ଫେରନ୍ତୁ obj;
}



ଧାଡ଼ିଭିତରେ __vpiBinaryConst::__vpiBinaryConst()
: signed_flag(0), sized_flag(0)
{ }

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiBinaryConst::get_type_code(ଶୂନ୍ୟ) ଅଚଳ
{ ଫେରନ୍ତୁ vpiConstant; }

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiBinaryConst::vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ବାଛ (code) {
	  ସ୍ଥିତି vpiConstType:
	    ଫେରନ୍ତୁ vpi_ଦ୍ୱିମିକ_Const;

	  ସ୍ଥିତି vpi_ଧାଡ଼ି_No:
	    ଫେରନ୍ତୁ 0;  // Not implemented for now!

	  ସ୍ଥିତି vpiSigned:
	    ଫେରନ୍ତୁ signed_flag? 1 : 0;

	  ସ୍ଥିତି vpiSize:
	    ଫେରନ୍ତୁ bits.size();

          ସ୍ଥିତି vpiAutomatic:
	    ଫେରନ୍ତୁ 0;

#ଯଦି defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          ସ୍ଥିତି _vpiFromThr:
	      ଫେରନ୍ତୁ _vpiNoThr;
#endif

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiBinaryConst\n", code);
	    ପ୍ରତିଜ୍ଞା(0);
	    ଫେରନ୍ତୁ 0;
      }
}


ଶୂନ୍ୟ __vpiBinaryConst::vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val)
{
      ବାଛ (val->format) {

	  ସ୍ଥିତି vpiObjTypeVal:
	  ସ୍ଥିତି vpiBinStrVal:
	  ସ୍ଥିତି vpiDecStrVal:
	  ସ୍ଥିତି vpiOctStrVal:
	  ସ୍ଥିତି vpiHexStrVal:
          ସ୍ଥିତି vpiScalarVal:
	  ସ୍ଥିତି vpiIntVal:
	  ସ୍ଥିତି vpiVectorVal:
	  ସ୍ଥିତି vpiStringVal:
	  ସ୍ଥିତି vpiRealVal:
	    vpip_vec4_get_ମୂଲ୍ୟ(bits, bits.size(), signed_flag, val);
	    ଭାଙ୍ଗ;

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "vvp error: format %d not supported "
		    "by vpiBinaryConst\n", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)val->format);
	    val->format = vpiSuppressVal;
	    ଭାଙ୍ଗ;
      }
}

/*
 * Make a VPI constant from a vector string. The string is normally a
 * ASCII string, with each letter a 4-value bit. The first character
 * may be an 's' if the vector is signed.
 */
vpiHandle vpip_make_ଦ୍ୱିମିକ_const(unsigned wid, ଅଚଳ ଅକ୍ଷର*bits)
{
      ସଂରଚନା __vpiBinaryConst*obj = ନୂତନ __vpiBinaryConst;

      ଅଚଳ ଅକ୍ଷର*bp = bits;
      ଯଦି (*bp == 's') {
	    bp += 1;
	    obj->signed_flag = 1;
      }

      obj->bits = vector4_from_text(bp, wid);

      ଫେରନ୍ତୁ obj;
}

vvp_vector4_t vector4_from_text(ଅଚଳ ଅକ୍ଷର*bits, unsigned wid)
{
      vvp_vector4_t res (wid);

      ପାଇଁ (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t val = BIT4_0;
	    ବାଛ (bits[wid-idx-1]) {
		ସ୍ଥିତି '0':
		  val = BIT4_0;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି '1':
		  val = BIT4_1;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି 'x':
		  val = BIT4_X;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି 'z':
		  val = BIT4_Z;
		  ଭାଙ୍ଗ;
	    }

	    res.set_bit(idx, val);
      }

      ଫେରନ୍ତୁ res;
}

ସଂରଚନା __vpiBinaryParam  : ସର୍ବସାଧାରଣ __vpiBinaryConst {
      __vpiBinaryParam(__vpiScope*scope, ଅଚଳ vvp_vector4_t&b, ତାର୍କିକ is_signed,
                       ଅଚଳ ଅକ୍ଷର*name, ତାର୍କିକ local_flag, long file_idx, long lineno);
      ~__vpiBinaryParam() override;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଅକ୍ଷର*vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      vpiHandle vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;

    ବ୍ୟକ୍ତିଗତ:
      __vpiScope*scope_;
      ଅଚଳ ଅକ୍ଷର*basename_;
      ତାର୍କିକ     local_flag_;
      unsigned file_idx_;
      unsigned lineno_;
};

ଧାଡ଼ିଭିତରେ __vpiBinaryParam::__vpiBinaryParam(__vpiScope*scope, ଅଚଳ vvp_vector4_t&b,
                                          ତାର୍କିକ is_signed, ଅଚଳ ଅକ୍ଷର*nam,
                                          ତାର୍କିକ local_flag, long file_idx, long lineno)
{
      scope_ = scope;
      bits = b;
      signed_flag = is_signed ? 1 : 0;
      sized_flag = 0;
      basename_ = nam;
      local_flag_ = local_flag;
      ପ୍ରତିଜ୍ଞା(file_idx >= 0 && file_idx <= INT_MAX);
      file_idx_ = (unsigned) file_idx;
      ପ୍ରତିଜ୍ଞା(lineno >= 0 && lineno <= INT_MAX);
      lineno_ = (unsigned) lineno;
}

__vpiBinaryParam::~__vpiBinaryParam()
{
      delete[]basename_;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiBinaryParam::get_type_code(ଶୂନ୍ୟ) ଅଚଳ
{ ଫେରନ୍ତୁ vpiParameter; }

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiBinaryParam::vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
    ବାଛ (code) {
      ସ୍ଥିତି vpi_ଧାଡ଼ି_No :
        ଫେରନ୍ତୁ lineno_;

      ସ୍ଥିତି vpiLocalParam :
        ଫେରନ୍ତୁ local_flag_;

      ପୂର୍ବନିର୍ଦ୍ଧାରିତ :
        ଫେରନ୍ତୁ __vpiBinaryConst::vpi_get(code);
    }
}

ଅକ୍ଷର*__vpiBinaryParam::vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ଯଦି (code == vpiFile)
	    ଫେରନ୍ତୁ simple_set_rbuf_str(file_names[file_idx_]);

      ଫେରନ୍ତୁ generic_get_str(code, scope_, basename_, ଖାଲି);
}


vpiHandle __vpiBinaryParam::vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ବାଛ (code) {
	  ସ୍ଥିତି vpiScope:
	    ଫେରନ୍ତୁ scope_;

	  ସ୍ଥିତି vpiModule:
	    ଫେରନ୍ତୁ vpip_module(scope_);

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଫେରନ୍ତୁ 0;
      }
}


vpiHandle vpip_make_ଦ୍ୱିମିକ_param(ଅଚଳ ଅକ୍ଷର*name, ଅଚଳ vvp_vector4_t&bits,
				 ତାର୍କିକ signed_flag, ତାର୍କିକ local_flag,
				 long file_idx, long lineno)
{
      ସଂରଚନା __vpiBinaryParam*obj = ନୂତନ __vpiBinaryParam(vpip_peek_current_scope(),
                                                         bits, signed_flag, name,
                                                         local_flag, file_idx, lineno);

      ଫେରନ୍ତୁ obj;
}



__vpiDecConst::__vpiDecConst(ପୂର୍ଣ୍ଣସଂଖ୍ୟା val)
{
      value = val;
}

__vpiDecConst::__vpiDecConst(ଅଚଳ __vpiDecConst&that)
: __vpiHandle(), value(that.value)
{
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiDecConst::get_type_code(ଶୂନ୍ୟ) ଅଚଳ
{ ଫେରନ୍ତୁ vpiConstant; }

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiDecConst::vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ବାଛ (code) {
	  ସ୍ଥିତି vpiConstType:
	    ଫେରନ୍ତୁ vpiDecConst;

	  ସ୍ଥିତି vpiSigned:
	    ଫେରନ୍ତୁ 1;

	  ସ୍ଥିତି vpiSize:
	    ଫେରନ୍ତୁ 32;

          ସ୍ଥିତି vpiAutomatic:
	    ଫେରନ୍ତୁ 0;

#ଯଦି defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          ସ୍ଥିତି _vpiFromThr:
	      ଫେରନ୍ତୁ _vpiNoThr;
#endif

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiDecConst\n", code);
	    ପ୍ରତିଜ୍ଞା(0);
	    ଫେରନ୍ତୁ 0;
      }
}


ଶୂନ୍ୟ __vpiDecConst::vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ vp)
{
      ସ୍ଥିର ଅଚଳ size_t RBUF_USE_SIZE = 64 + 1;
      ଅକ୍ଷର*rbuf = static_cast<ଅକ୍ଷର *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));
      ଅକ୍ଷର*cp = rbuf;

      ବାଛ (vp->format) {

	  ସ୍ଥିତି vpiObjTypeVal:
	  ସ୍ଥିତି vpiIntVal: {
		vp->value.integer = value;
		ଭାଙ୍ଗ;
	  }

          ସ୍ଥିତି vpiDecStrVal:
	      snprintf(rbuf, RBUF_USE_SIZE, "%d", value);
	      vp->value.str = rbuf;
	      ଭାଙ୍ଗ;

          ସ୍ଥିତି vpiBinStrVal:
	      ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା bit=31; bit>=0;bit--){
		  *cp++ = "01"[ (static_cast<unsigned>(value)>>bit)&1 ];
	      }
	      *cp = 0;

	      vp->value.str = rbuf;
	      ଭାଙ୍ଗ;

          ସ୍ଥିତି vpiHexStrVal:
	      snprintf(rbuf, RBUF_USE_SIZE, "%08x", value);
	      vp->value.str = rbuf;
	      ଭାଙ୍ଗ;

          ସ୍ଥିତି vpiOctStrVal:
	      snprintf(rbuf, RBUF_USE_SIZE, "%011x", value);
	      vp->value.str = rbuf;
	      ଭାଙ୍ଗ;

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "vvp error (vpi_const.cc): format %d not supported "
		    "by vpiDecConst\n", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vp->format);
	    vp->format = vpiSuppressVal;
	    ଭାଙ୍ଗ;
      }
}


ଧାଡ଼ିଭିତରେ __vpiRealConst::__vpiRealConst(ଦ୍ୱିଗୁଣ val)
: value(val)
{ }

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiRealConst::get_type_code(ଶୂନ୍ୟ) ଅଚଳ
{ ଫେରନ୍ତୁ vpiConstant; }

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiRealConst::vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ବାଛ (code) {
	  ସ୍ଥିତି vpi_ଧାଡ଼ି_No:
	    ଫେରନ୍ତୁ 0;  // Not implemented for now!

	  ସ୍ଥିତି vpiSize:
	    ଫେରନ୍ତୁ 1;

	  ସ୍ଥିତି vpiConstType:
	    ଫେରନ୍ତୁ vpiRealConst;

	  ସ୍ଥିତି vpiSigned:
	    ଫେରନ୍ତୁ 1;

          ସ୍ଥିତି vpiAutomatic:
	    ଫେରନ୍ତୁ 0;

#ଯଦି defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          ସ୍ଥିତି _vpiFromThr:
	      ଫେରନ୍ତୁ _vpiNoThr;
#endif

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiRealConst\n", code);
	    ପ୍ରତିଜ୍ଞା(0);
	    ଫେରନ୍ତୁ 0;
      }
}


ଶୂନ୍ୟ __vpiRealConst::vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val)
{
      vpip_real_get_ମୂଲ୍ୟ(value, val);
}


vpiHandle vpip_make_real_const(ଦ୍ୱିଗୁଣ value)
{
      __vpiRealConst*obj = ନୂତନ __vpiRealConst(value);
      ଫେରନ୍ତୁ obj;
}

ସଂରଚନା __vpiRealParam  : ସର୍ବସାଧାରଣ __vpiRealConst {
      __vpiRealParam(__vpiScope* scope, ଦ୍ୱିଗୁଣ val, ଅଚଳ ଅକ୍ଷର*name,
                     ତାର୍କିକ local_flag, long file_idx, long lineno);
      ~__vpiRealParam() override;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଅକ୍ଷର*vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      vpiHandle vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;

    ବ୍ୟକ୍ତିଗତ:
      __vpiScope* scope_;
      ଅଚଳ ଅକ୍ଷର*basename_;
      ତାର୍କିକ local_flag_;
      unsigned file_idx_;
      unsigned lineno_;
};


ଧାଡ଼ିଭିତରେ __vpiRealParam::__vpiRealParam(__vpiScope* scope, ଦ୍ୱିଗୁଣ val, ଅଚଳ ଅକ୍ଷର*name,
                                      ତାର୍କିକ local_flag, long file_idx, long lineno)
: __vpiRealConst(val)
{
      scope_ = scope;
      basename_ = name;
      local_flag_ = local_flag;
      ପ୍ରତିଜ୍ଞା(file_idx >= 0 && file_idx <= INT_MAX);
      file_idx_ = (unsigned) file_idx;
      ପ୍ରତିଜ୍ଞା(lineno >= 0 && lineno <= INT_MAX);
      lineno_ = (unsigned) lineno;
}

__vpiRealParam::~__vpiRealParam()
{
      delete[]basename_;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiRealParam::get_type_code(ଶୂନ୍ୟ) ଅଚଳ
{ ଫେରନ୍ତୁ vpiParameter; }

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiRealParam::vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
    ବାଛ (code) {
      ସ୍ଥିତି vpi_ଧାଡ଼ି_No :
        ଫେରନ୍ତୁ lineno_;

      ସ୍ଥିତି vpiLocalParam :
        ଫେରନ୍ତୁ local_flag_;

      ପୂର୍ବନିର୍ଦ୍ଧାରିତ :
           ଫେରନ୍ତୁ __vpiRealConst::vpi_get(code);
    }
}

ଅକ୍ଷର* __vpiRealParam::vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ଯଦି (code == vpiFile)
            ଫେରନ୍ତୁ simple_set_rbuf_str(file_names[file_idx_]);

      ଫେରନ୍ତୁ generic_get_str(code, scope_, basename_, ଖାଲି);
}

vpiHandle __vpiRealParam::vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ବାଛ (code) {
          ସ୍ଥିତି vpiScope:
            ଫେରନ୍ତୁ scope_;

	  ସ୍ଥିତି vpiModule:
	    ଫେରନ୍ତୁ vpip_module(scope_);

          ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            ଫେରନ୍ତୁ 0;
      }
}


vpiHandle vpip_make_real_param(ଅଚଳ ଅକ୍ଷର*name, ଦ୍ୱିଗୁଣ value,
                               ତାର୍କିକ local_flag, long file_idx, long lineno)
{
      ସଂରଚନା __vpiRealParam*obj = ନୂତନ __vpiRealParam(vpip_peek_current_scope(),
                                                     value, name, local_flag,
                                                     file_idx, lineno);
      ଫେରନ୍ତୁ obj;
}

/*
 * Make a VPI null constant
 */
ଧାଡ଼ିଭିତରେ __vpiNullConst::__vpiNullConst()
{ }

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiNullConst::get_type_code(ଶୂନ୍ୟ) ଅଚଳ
{ ଫେରନ୍ତୁ vpiConstant; }

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiNullConst::vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ବାଛ (code) {
	  ସ୍ଥିତି vpi_ଧାଡ଼ି_No:
	    ଫେରନ୍ତୁ 0;  // Not implemented for now!

	  ସ୍ଥିତି vpiSize:
	    ଫେରନ୍ତୁ 32;

	  ସ୍ଥିତି vpiConstType:
	    ଫେରନ୍ତୁ vpiNullConst;

	  ସ୍ଥିତି vpiSigned:
	    ଫେରନ୍ତୁ 0;

          ସ୍ଥିତି vpiAutomatic:
	    ଫେରନ୍ତୁ 0;

#ଯଦି defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          ସ୍ଥିତି _vpiFromThr:
	      ଫେରନ୍ତୁ _vpiNoThr;
#endif

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiNullConst\n", code);
	    ପ୍ରତିଜ୍ଞା(0);
	    ଫେରନ୍ତୁ 0;
      }
}

ଶୂନ୍ୟ __vpiNullConst::vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val)
{
      ସ୍ଥିର ଅଚଳ size_t RBUF_USE_SIZE = 64 + 1;
      ଅକ୍ଷର*rbuf = static_cast<ଅକ୍ଷର *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));

      ବାଛ (val->format) {

	  ସ୍ଥିତି vpiObjTypeVal:
	    val->format = vpiStringVal;
	    // fall through
	  ସ୍ଥିତି vpiBinStrVal:
	  ସ୍ଥିତି vpiDecStrVal:
	  ସ୍ଥିତି vpiOctStrVal:
	  ସ୍ଥିତି vpiHexStrVal:
	  ସ୍ଥିତି vpiStringVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "null");
	    val->value.str = rbuf;
	    ଭାଙ୍ଗ;

          ସ୍ଥିତି vpiScalarVal:
	    val->value.scalar = vpi0;
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiIntVal:
	    val->value.integer = 0;
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiVectorVal:
	    val->value.vector = static_cast<p_vpi_vecval>
	                        (need_result_buf(2*ଆକାର(s_vpi_vecval),
	                                         RBUF_VAL));
	    ପାଇଁ (unsigned idx = 0; idx < 2; idx += 1) {
		  val->value.vector[idx].aval = 0;
		  val->value.vector[idx].bval = 0;
	    }
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiRealVal:
	    val->value.real = 0.0;
	    ଭାଙ୍ଗ;

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "vvp error: format %d not supported "
		    "by vpiNullConst\n", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)val->format);
	    val->format = vpiSuppressVal;
	    ଭାଙ୍ଗ;
      }
}

vpiHandle vpip_make_null_const()
{
      __vpiNullConst*obj = ନୂତନ __vpiNullConst;
      ଫେରନ୍ତୁ obj;
}

#ifdef CHECK_WITH_VALGRIND
ଶୂନ୍ୟ constant_delete(vpiHandle item)
{
      ପ୍ରତିଜ୍ଞା(item->get_type_code() == vpiConstant);
      ବାଛ(vpi_get(vpiConstType, item)) {
	  ସ୍ଥିତି vpiStringConst:
	    delete dynamic_cast<__vpiStringConst*>(item);
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି vpiDecConst:
	    delete dynamic_cast<__vpiDecConst*>(item);
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି vpi_ଦ୍ୱିମିକ_Const:
	    delete dynamic_cast<__vpiBinaryConst*>(item);
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି vpiRealConst:
	    delete dynamic_cast<__vpiRealConst*>(item);
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି vpiNullConst:
	    delete dynamic_cast<__vpiNullConst*>(item);
	    ଭାଙ୍ଗ;
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ପ୍ରତିଜ୍ଞା(0);
      }
}

ଶୂନ୍ୟ parameter_delete(vpiHandle item)
{
      ବାଛ(vpi_get(vpiConstType, item)) {
	  ସ୍ଥିତି vpiStringConst: {
	    ଶ୍ରେଣୀ __vpiStringParam*rfp = dynamic_cast<__vpiStringParam*>(item);
	    delete rfp;
	    ଭାଙ୍ଗ; }
	  ସ୍ଥିତି vpi_ଦ୍ୱିମିକ_Const: {
	    ସଂରଚନା __vpiBinaryParam*rfp = dynamic_cast<__vpiBinaryParam*>(item);
	    delete rfp;
	    ଭାଙ୍ଗ; }
	  ସ୍ଥିତି vpiRealConst: {
	    ସଂରଚନା __vpiRealParam*rfp = dynamic_cast<__vpiRealParam*>(item);
	    delete rfp;
	    ଭାଙ୍ଗ; }
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ପ୍ରତିଜ୍ଞା(0);
      }
}
#endif
