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

ವರ್ಗ __vpiStringConst : ಸಾರ್ವಜನಿಕ __vpiHandle {
    ಸಾರ್ವಜನಿಕ:
      explicit __vpiStringConst(ವರ್ಣ*val);
      ~__vpiStringConst() override;
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override;
      ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;

    ಗುಪ್ತ:
      ಶೂನ್ಯ process_string_();
    ಗುಪ್ತ:
      ವರ್ಣ*ಮೌಲ್ಯ_2;
      size_t ಮೌಲ್ಯ_len;
};

ಅನ್ತಃಸ್ಥ __vpiStringConst::__vpiStringConst(ವರ್ಣ*v)
: ಮೌಲ್ಯ_2(v)
{
      process_string_();
}

/*
 * Strings are described at the level of the vvp source as a string
 * with literal characters or octal escapes. No other escapes are
 * included, they are processed already by the compiler that generated
 * the vvp source.
 */
ಶೂನ್ಯ __vpiStringConst::process_string_(ಶೂನ್ಯ)
{
      ವರ್ಣ*chr = ಮೌಲ್ಯ_2;
      ವರ್ಣ*dp = ಮೌಲ್ಯ_2;

      ಯಾವತ್ (*chr) {
	    ವರ್ಣ next_char = *chr;

	      /* Process octal escapes that I might find. */
	    ಯದಿ (*chr == '\\') {
		  ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ idx = 1 ;  idx <= 3 ;  idx += 1) {
			ಪ್ರತಿಜ್ಞಾ(chr[idx] != 0);
			ಪ್ರತಿಜ್ಞಾ(chr[idx] < '8');
			ಪ್ರತಿಜ್ಞಾ(chr[idx] >= '0');
			next_char = next_char*8 + chr[idx] - '0';
		  }
		  chr += 3;
	    }
	    *dp++ = next_char;
	    chr += 1;
      }
      *dp = 0;
      ಮೌಲ್ಯ_len = dp - ಮೌಲ್ಯ_2;
}

__vpiStringConst::~__vpiStringConst()
{
      delete[] ಮೌಲ್ಯ_2;
}

ಪೂರ್ಣಾಙ್ಕ __vpiStringConst::get_type_code(ಶೂನ್ಯ) ಧ್ರುವ
{ ಪ್ರತ್ಯಾಗಮ vpiConstant; }

ಪೂರ್ಣಾಙ್ಕ __vpiStringConst::vpi_get(ಪೂರ್ಣಾಙ್ಕ code)
{
      ವಿಕಲ್ಪ (code) {
          ಪಕ್ಷ vpiSize:
	    ಪ್ರತ್ಯಾಗಮ ಮೌಲ್ಯ_len * 8;

          ಪಕ್ಷ vpiSigned:
	      ಪ್ರತ್ಯಾಗಮ 0;

	  ಪಕ್ಷ vpiConstType:
	      ಪ್ರತ್ಯಾಗಮ vpiStringConst;

          ಪಕ್ಷ vpiAutomatic:
	      ಪ್ರತ್ಯಾಗಮ 0;

#ಯದಿ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          ಪಕ್ಷ _vpiFromThr:
	      ಪ್ರತ್ಯಾಗಮ _vpiNoThr;
#endif

	  ಸ್ವಭಾವ:
	      fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiStringConst\n", code);
	      ಪ್ರತಿಜ್ಞಾ(0);
	      ಪ್ರತ್ಯಾಗಮ 0;
      }
}

ಶೂನ್ಯ __vpiStringConst::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ vp)
{
      unsigned uint_ಮೌಲ್ಯ;
      p_vpi_vecval vecp;
      unsigned size = ಮೌಲ್ಯ_len;
      ವರ್ಣ*rbuf = 0;
      ವರ್ಣ*cp;

      ವಿಕಲ್ಪ (vp->format) {
	  ಪಕ್ಷ vpiObjTypeVal:
	      /* String parameters by default have vpiStringVal values. */
	    vp->format = vpiStringVal;
	    // fallthrough
	  ಪಕ್ಷ vpiStringVal:
	    cp = ಮೌಲ್ಯ_2;
	    rbuf = static_cast<ವರ್ಣ *>(need_result_buf(size + 1, RBUF_VAL));
	    vp->value.str = rbuf;

	    ಪ್ರತಿ (unsigned ಪೂರ್ಣಾಙ್ಕ i = 0; i < size; i++) {
		  // Ignore leading null-bytes and replace other null-bytes with space.
		  // The LRM is not entirely clear on how null bytes should be handled.
		  // This is the implementation chosen for iverilog.
		  ಯದಿ (*cp)
			*rbuf++ = *cp;
		  ಅನ್ಯಥಾ ಯದಿ (rbuf != vp->value.str)
			*rbuf++ = ' ';

		  cp++;
	    }
	    *rbuf = '\0';
	    ಭಙ್ಗ;

          ಪಕ್ಷ vpiDecStrVal:
	    // Take the (up to 4) characters of the text, convert the characters
	    // to a numerical value, and convert that value to a decimal string.
	    // For example, the string "A" is ASCII 65, so the resulting string
	    // will be "65". and the string "AB" is 65*256 + 66 == 16706, so
	    // the resulting string is "16706". The "size" is the number of
	    // characters of input text to put to work.
	    ಯದಿ (size > 4){
		  // We only support standard integers. Ignore other bytes...
		  size = 4;
		  fprintf(stderr, "Warning (vpi_const.cc): %%d on constant strings only looks "
			  "at first 4 bytes!\n");
	    }
	    ಸ್ಥಿರ ಧ್ರುವ size_t RBUF_USE_SIZE = 12;
	    rbuf = static_cast<ವರ್ಣ *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));
	    uint_ಮೌಲ್ಯ = 0;
	    ಪ್ರತಿ(unsigned i=0; i<size; i += 1){
		  uint_ಮೌಲ್ಯ <<=8;
		  uint_ಮೌಲ್ಯ += (unsigned ವರ್ಣ)(ಮೌಲ್ಯ_2[i]);
	    }
	    snprintf(rbuf, RBUF_USE_SIZE, "%u", uint_ಮೌಲ್ಯ);
	    vp->value.str = rbuf;
	    ಭಙ್ಗ;

          ಪಕ್ಷ vpiBinStrVal:
	      rbuf = static_cast<ವರ್ಣ *>(need_result_buf(8 * size + 1, RBUF_VAL));
	      cp = rbuf;
	      ಪ್ರತಿ(unsigned i=0; i<size; i += 1){
		  ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ bit=7; bit>=0; bit -= 1){
		      *cp++ = "01"[ (ಮೌಲ್ಯ_2[i]>>bit)&1 ];
		  }
	      }
	      *cp = 0;
	      vp->value.str = rbuf;
	      ಭಙ್ಗ;

          ಪಕ್ಷ vpiHexStrVal:
	      rbuf = static_cast<ವರ್ಣ *>(need_result_buf(2 * size + 1, RBUF_VAL));
	      cp = rbuf;
	      ಪ್ರತಿ(unsigned i=0; i<size; i += 1){
		  ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ nibble=1; nibble>=0; nibble -= 1){
		      *cp++ = "0123456789abcdef"[ (ಮೌಲ್ಯ_2[i]>>(nibble*4))&15 ];
		  }
	      }
	      *cp = 0;
	      vp->value.str = rbuf;
	      ಭಙ್ಗ;

          ಪಕ್ಷ vpiOctStrVal:
	      fprintf(stderr, "ERROR (vpi_const.cc): %%o display of constant strings not yet implemented\n");
	      ಪ್ರತಿಜ್ಞಾ(0);
	      ಭಙ್ಗ;

          ಪಕ್ಷ vpiIntVal:
	      vp->value.integer = 0;
	      ಪ್ರತಿ(unsigned i=0; i<size; i += 1){
		  ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ bit=7; bit>=0; bit -= 1){
		      vp->value.integer <<= 1;
		      vp->value.integer += (ಮೌಲ್ಯ_2[i]>>bit)&1;
		  }
	      }
	      ಭಙ್ಗ;

          ಪಕ್ಷ vpiVectorVal:
              vp->value.vector = static_cast<p_vpi_vecval>
                                 (need_result_buf((size+3)/4*
                                                   ಪರಿಮಾಣ(s_vpi_vecval),
                                                  RBUF_VAL));
              uint_ಮೌಲ್ಯ = 0;
              vecp = vp->value.vector;
              vecp->aval = vecp->bval = 0;
	      ಪ್ರತಿ(unsigned i=0; i<size; i += 1){
		  vecp->aval |= ಮೌಲ್ಯ_2[i] << uint_ಮೌಲ್ಯ*8;
		  uint_ಮೌಲ್ಯ += 1;
		  ಯದಿ (uint_ಮೌಲ್ಯ > 3) {
		      uint_ಮೌಲ್ಯ = 0;
		      vecp += 1;
		      vecp->aval = vecp->bval = 0;
		  }
	      }
	      ಭಙ್ಗ;


	  ಸ್ವಭಾವ:
	    fprintf(stderr, "ERROR (vpi_const.cc): vp->format: %d\n",
	            (ಪೂರ್ಣಾಙ್ಕ)vp->format);
	    ಪ್ರತಿಜ್ಞಾ(0);

	    vp->format = vpiSuppressVal;
	    ಭಙ್ಗ;
      }
}


ಸಂರಚನಾ __vpiStringConstTEMP : ಸಾರ್ವಜನಿಕ __vpiStringConst {
      explicit ಅನ್ತಃಸ್ಥ __vpiStringConstTEMP(ವರ್ಣ*v) : __vpiStringConst(v) { }
      free_object_fun_t free_object_fun(ಶೂನ್ಯ) override;
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ free_temp_string(vpiHandle obj)
{
      ಸಂರಚನಾ __vpiStringConstTEMP*rfp = dynamic_cast<__vpiStringConstTEMP*>(obj);
      delete rfp;
      ಪ್ರತ್ಯಾಗಮ 1;
}

__vpiHandle::free_object_fun_t __vpiStringConstTEMP::free_object_fun(ಶೂನ್ಯ)
{ ಪ್ರತ್ಯಾಗಮ &free_temp_string; }

vpiHandle vpip_make_string_const(ವರ್ಣ*text, ತಾರ್ಕಿಕ persistent_flag)
{
      __vpiStringConst*obj;

      obj = persistent_flag? ನೂತನ __vpiStringConst(text) : ನೂತನ __vpiStringConstTEMP(text);

      ಪ್ರತ್ಯಾಗಮ obj;
}


ವರ್ಗ __vpiStringParam  : ಸಾರ್ವಜನಿಕ __vpiStringConst {
    ಸಾರ್ವಜನಿಕ:
      __vpiStringParam(__vpiScope* scope, ವರ್ಣ*txt, ಧ್ರುವ ವರ್ಣ*name,
                       ತಾರ್ಕಿಕ local_flag, long file_idx, long lineno);
      ~__vpiStringParam() override;
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override;
      ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code) override;
      ವರ್ಣ*vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code) override;
      vpiHandle vpi_handle(ಪೂರ್ಣಾಙ್ಕ code) override;

    ಗುಪ್ತ:
      __vpiScope* scope_;
      ಧ್ರುವ ವರ್ಣ*basename_;
      ತಾರ್ಕಿಕ       local_flag_;
      unsigned   file_idx_;
      unsigned   lineno_;
};

ಅನ್ತಃಸ್ಥ __vpiStringParam::__vpiStringParam(__vpiScope* scope, ವರ್ಣ*txt, ಧ್ರುವ ವರ್ಣ*nam,
                                          ತಾರ್ಕಿಕ local_flag, long file_idx, long lineno)
: __vpiStringConst(txt)
{
      scope_ = scope;
      basename_ = nam;
      local_flag_ = local_flag;
      ಪ್ರತಿಜ್ಞಾ(file_idx >= 0 && file_idx <= INT_MAX);
      file_idx_ = (unsigned) file_idx;
      ಪ್ರತಿಜ್ಞಾ(lineno >= 0 && lineno <= INT_MAX);
      lineno_ = (unsigned) lineno;
}

__vpiStringParam::~__vpiStringParam()
{
      delete[]basename_;
}

ಪೂರ್ಣಾಙ್ಕ __vpiStringParam::get_type_code(ಶೂನ್ಯ) ಧ್ರುವ
{ ಪ್ರತ್ಯಾಗಮ vpiParameter; }

ಪೂರ್ಣಾಙ್ಕ __vpiStringParam::vpi_get(ಪೂರ್ಣಾಙ್ಕ code)
{
    ವಿಕಲ್ಪ (code) {
       ಪಕ್ಷ vpi_ಸಾಲು_No :
         ಪ್ರತ್ಯಾಗಮ lineno_;

       ಪಕ್ಷ vpiLocalParam :
         ಪ್ರತ್ಯಾಗಮ local_flag_;

       ಸ್ವಭಾವ :
         ಪ್ರತ್ಯಾಗಮ __vpiStringConst::vpi_get(code);
    }
}


ವರ್ಣ*__vpiStringParam::vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code)
{
      ಯದಿ (code == vpiFile) {
	    ಪ್ರತ್ಯಾಗಮ simple_set_rbuf_str(file_names[file_idx_]);
      }

      ಪ್ರತ್ಯಾಗಮ generic_get_str(code, scope_, basename_, ರಿಕ್ತ);
}


vpiHandle __vpiStringParam::vpi_handle(ಪೂರ್ಣಾಙ್ಕ code)
{
      ವಿಕಲ್ಪ (code) {
	  ಪಕ್ಷ vpiScope:
	    ಪ್ರತ್ಯಾಗಮ scope_;

	  ಪಕ್ಷ vpiModule:
	    ಪ್ರತ್ಯಾಗಮ vpip_module(scope_);

	  ಸ್ವಭಾವ:
	    ಪ್ರತ್ಯಾಗಮ 0;
      }
}

vpiHandle vpip_make_string_param(ಧ್ರುವ ವರ್ಣ*name, ವರ್ಣ*text,
                                 ತಾರ್ಕಿಕ local_flag, long file_idx, long lineno)
{
      __vpiStringParam*obj = ನೂತನ __vpiStringParam(vpip_peek_current_scope(), text, name,
                                                  local_flag, file_idx, lineno);

      ಪ್ರತ್ಯಾಗಮ obj;
}



ಅನ್ತಃಸ್ಥ __vpiBinaryConst::__vpiBinaryConst()
: signed_flag(0), sized_flag(0)
{ }

ಪೂರ್ಣಾಙ್ಕ __vpiBinaryConst::get_type_code(ಶೂನ್ಯ) ಧ್ರುವ
{ ಪ್ರತ್ಯಾಗಮ vpiConstant; }

ಪೂರ್ಣಾಙ್ಕ __vpiBinaryConst::vpi_get(ಪೂರ್ಣಾಙ್ಕ code)
{
      ವಿಕಲ್ಪ (code) {
	  ಪಕ್ಷ vpiConstType:
	    ಪ್ರತ್ಯಾಗಮ vpi_ದ್ವಿಮಾನ_Const;

	  ಪಕ್ಷ vpi_ಸಾಲು_No:
	    ಪ್ರತ್ಯಾಗಮ 0;  // Not implemented for now!

	  ಪಕ್ಷ vpiSigned:
	    ಪ್ರತ್ಯಾಗಮ signed_flag? 1 : 0;

	  ಪಕ್ಷ vpiSize:
	    ಪ್ರತ್ಯಾಗಮ bits.size();

          ಪಕ್ಷ vpiAutomatic:
	    ಪ್ರತ್ಯಾಗಮ 0;

#ಯದಿ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          ಪಕ್ಷ _vpiFromThr:
	      ಪ್ರತ್ಯಾಗಮ _vpiNoThr;
#endif

	  ಸ್ವಭಾವ:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiBinaryConst\n", code);
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಪ್ರತ್ಯಾಗಮ 0;
      }
}


ಶೂನ್ಯ __vpiBinaryConst::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val)
{
      ವಿಕಲ್ಪ (val->format) {

	  ಪಕ್ಷ vpiObjTypeVal:
	  ಪಕ್ಷ vpiBinStrVal:
	  ಪಕ್ಷ vpiDecStrVal:
	  ಪಕ್ಷ vpiOctStrVal:
	  ಪಕ್ಷ vpiHexStrVal:
          ಪಕ್ಷ vpiScalarVal:
	  ಪಕ್ಷ vpiIntVal:
	  ಪಕ್ಷ vpiVectorVal:
	  ಪಕ್ಷ vpiStringVal:
	  ಪಕ್ಷ vpiRealVal:
	    vpip_vec4_get_ಮೌಲ್ಯ(bits, bits.size(), signed_flag, val);
	    ಭಙ್ಗ;

	  ಸ್ವಭಾವ:
	    fprintf(stderr, "vvp error: format %d not supported "
		    "by vpiBinaryConst\n", (ಪೂರ್ಣಾಙ್ಕ)val->format);
	    val->format = vpiSuppressVal;
	    ಭಙ್ಗ;
      }
}

/*
 * Make a VPI constant from a vector string. The string is normally a
 * ASCII string, with each letter a 4-value bit. The first character
 * may be an 's' if the vector is signed.
 */
vpiHandle vpip_make_ದ್ವಿಮಾನ_const(unsigned wid, ಧ್ರುವ ವರ್ಣ*bits)
{
      ಸಂರಚನಾ __vpiBinaryConst*obj = ನೂತನ __vpiBinaryConst;

      ಧ್ರುವ ವರ್ಣ*bp = bits;
      ಯದಿ (*bp == 's') {
	    bp += 1;
	    obj->signed_flag = 1;
      }

      obj->bits = vector4_from_text(bp, wid);

      ಪ್ರತ್ಯಾಗಮ obj;
}

vvp_vector4_t vector4_from_text(ಧ್ರುವ ವರ್ಣ*bits, unsigned wid)
{
      vvp_vector4_t res (wid);

      ಪ್ರತಿ (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t val = BIT4_0;
	    ವಿಕಲ್ಪ (bits[wid-idx-1]) {
		ಪಕ್ಷ '0':
		  val = BIT4_0;
		  ಭಙ್ಗ;
		ಪಕ್ಷ '1':
		  val = BIT4_1;
		  ಭಙ್ಗ;
		ಪಕ್ಷ 'x':
		  val = BIT4_X;
		  ಭಙ್ಗ;
		ಪಕ್ಷ 'z':
		  val = BIT4_Z;
		  ಭಙ್ಗ;
	    }

	    res.set_bit(idx, val);
      }

      ಪ್ರತ್ಯಾಗಮ res;
}

ಸಂರಚನಾ __vpiBinaryParam  : ಸಾರ್ವಜನಿಕ __vpiBinaryConst {
      __vpiBinaryParam(__vpiScope*scope, ಧ್ರುವ vvp_vector4_t&b, ತಾರ್ಕಿಕ is_signed,
                       ಧ್ರುವ ವರ್ಣ*name, ತಾರ್ಕಿಕ local_flag, long file_idx, long lineno);
      ~__vpiBinaryParam() override;
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override;
      ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code) override;
      ವರ್ಣ*vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code) override;
      vpiHandle vpi_handle(ಪೂರ್ಣಾಙ್ಕ code) override;

    ಗುಪ್ತ:
      __vpiScope*scope_;
      ಧ್ರುವ ವರ್ಣ*basename_;
      ತಾರ್ಕಿಕ     local_flag_;
      unsigned file_idx_;
      unsigned lineno_;
};

ಅನ್ತಃಸ್ಥ __vpiBinaryParam::__vpiBinaryParam(__vpiScope*scope, ಧ್ರುವ vvp_vector4_t&b,
                                          ತಾರ್ಕಿಕ is_signed, ಧ್ರುವ ವರ್ಣ*nam,
                                          ತಾರ್ಕಿಕ local_flag, long file_idx, long lineno)
{
      scope_ = scope;
      bits = b;
      signed_flag = is_signed ? 1 : 0;
      sized_flag = 0;
      basename_ = nam;
      local_flag_ = local_flag;
      ಪ್ರತಿಜ್ಞಾ(file_idx >= 0 && file_idx <= INT_MAX);
      file_idx_ = (unsigned) file_idx;
      ಪ್ರತಿಜ್ಞಾ(lineno >= 0 && lineno <= INT_MAX);
      lineno_ = (unsigned) lineno;
}

__vpiBinaryParam::~__vpiBinaryParam()
{
      delete[]basename_;
}

ಪೂರ್ಣಾಙ್ಕ __vpiBinaryParam::get_type_code(ಶೂನ್ಯ) ಧ್ರುವ
{ ಪ್ರತ್ಯಾಗಮ vpiParameter; }

ಪೂರ್ಣಾಙ್ಕ __vpiBinaryParam::vpi_get(ಪೂರ್ಣಾಙ್ಕ code)
{
    ವಿಕಲ್ಪ (code) {
      ಪಕ್ಷ vpi_ಸಾಲು_No :
        ಪ್ರತ್ಯಾಗಮ lineno_;

      ಪಕ್ಷ vpiLocalParam :
        ಪ್ರತ್ಯಾಗಮ local_flag_;

      ಸ್ವಭಾವ :
        ಪ್ರತ್ಯಾಗಮ __vpiBinaryConst::vpi_get(code);
    }
}

ವರ್ಣ*__vpiBinaryParam::vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code)
{
      ಯದಿ (code == vpiFile)
	    ಪ್ರತ್ಯಾಗಮ simple_set_rbuf_str(file_names[file_idx_]);

      ಪ್ರತ್ಯಾಗಮ generic_get_str(code, scope_, basename_, ರಿಕ್ತ);
}


vpiHandle __vpiBinaryParam::vpi_handle(ಪೂರ್ಣಾಙ್ಕ code)
{
      ವಿಕಲ್ಪ (code) {
	  ಪಕ್ಷ vpiScope:
	    ಪ್ರತ್ಯಾಗಮ scope_;

	  ಪಕ್ಷ vpiModule:
	    ಪ್ರತ್ಯಾಗಮ vpip_module(scope_);

	  ಸ್ವಭಾವ:
	    ಪ್ರತ್ಯಾಗಮ 0;
      }
}


vpiHandle vpip_make_ದ್ವಿಮಾನ_param(ಧ್ರುವ ವರ್ಣ*name, ಧ್ರುವ vvp_vector4_t&bits,
				 ತಾರ್ಕಿಕ signed_flag, ತಾರ್ಕಿಕ local_flag,
				 long file_idx, long lineno)
{
      ಸಂರಚನಾ __vpiBinaryParam*obj = ನೂತನ __vpiBinaryParam(vpip_peek_current_scope(),
                                                         bits, signed_flag, name,
                                                         local_flag, file_idx, lineno);

      ಪ್ರತ್ಯಾಗಮ obj;
}



__vpiDecConst::__vpiDecConst(ಪೂರ್ಣಾಙ್ಕ val)
{
      value = val;
}

__vpiDecConst::__vpiDecConst(ಧ್ರುವ __vpiDecConst&that)
: __vpiHandle(), value(that.value)
{
}


ಪೂರ್ಣಾಙ್ಕ __vpiDecConst::get_type_code(ಶೂನ್ಯ) ಧ್ರುವ
{ ಪ್ರತ್ಯಾಗಮ vpiConstant; }

ಪೂರ್ಣಾಙ್ಕ __vpiDecConst::vpi_get(ಪೂರ್ಣಾಙ್ಕ code)
{
      ವಿಕಲ್ಪ (code) {
	  ಪಕ್ಷ vpiConstType:
	    ಪ್ರತ್ಯಾಗಮ vpiDecConst;

	  ಪಕ್ಷ vpiSigned:
	    ಪ್ರತ್ಯಾಗಮ 1;

	  ಪಕ್ಷ vpiSize:
	    ಪ್ರತ್ಯಾಗಮ 32;

          ಪಕ್ಷ vpiAutomatic:
	    ಪ್ರತ್ಯಾಗಮ 0;

#ಯದಿ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          ಪಕ್ಷ _vpiFromThr:
	      ಪ್ರತ್ಯಾಗಮ _vpiNoThr;
#endif

	  ಸ್ವಭಾವ:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiDecConst\n", code);
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಪ್ರತ್ಯಾಗಮ 0;
      }
}


ಶೂನ್ಯ __vpiDecConst::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ vp)
{
      ಸ್ಥಿರ ಧ್ರುವ size_t RBUF_USE_SIZE = 64 + 1;
      ವರ್ಣ*rbuf = static_cast<ವರ್ಣ *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));
      ವರ್ಣ*cp = rbuf;

      ವಿಕಲ್ಪ (vp->format) {

	  ಪಕ್ಷ vpiObjTypeVal:
	  ಪಕ್ಷ vpiIntVal: {
		vp->value.integer = value;
		ಭಙ್ಗ;
	  }

          ಪಕ್ಷ vpiDecStrVal:
	      snprintf(rbuf, RBUF_USE_SIZE, "%d", value);
	      vp->value.str = rbuf;
	      ಭಙ್ಗ;

          ಪಕ್ಷ vpiBinStrVal:
	      ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ bit=31; bit>=0;bit--){
		  *cp++ = "01"[ (static_cast<unsigned>(value)>>bit)&1 ];
	      }
	      *cp = 0;

	      vp->value.str = rbuf;
	      ಭಙ್ಗ;

          ಪಕ್ಷ vpiHexStrVal:
	      snprintf(rbuf, RBUF_USE_SIZE, "%08x", value);
	      vp->value.str = rbuf;
	      ಭಙ್ಗ;

          ಪಕ್ಷ vpiOctStrVal:
	      snprintf(rbuf, RBUF_USE_SIZE, "%011x", value);
	      vp->value.str = rbuf;
	      ಭಙ್ಗ;

	  ಸ್ವಭಾವ:
	    fprintf(stderr, "vvp error (vpi_const.cc): format %d not supported "
		    "by vpiDecConst\n", (ಪೂರ್ಣಾಙ್ಕ)vp->format);
	    vp->format = vpiSuppressVal;
	    ಭಙ್ಗ;
      }
}


ಅನ್ತಃಸ್ಥ __vpiRealConst::__vpiRealConst(ದ್ವಿಗುಣ val)
: value(val)
{ }

ಪೂರ್ಣಾಙ್ಕ __vpiRealConst::get_type_code(ಶೂನ್ಯ) ಧ್ರುವ
{ ಪ್ರತ್ಯಾಗಮ vpiConstant; }

ಪೂರ್ಣಾಙ್ಕ __vpiRealConst::vpi_get(ಪೂರ್ಣಾಙ್ಕ code)
{
      ವಿಕಲ್ಪ (code) {
	  ಪಕ್ಷ vpi_ಸಾಲು_No:
	    ಪ್ರತ್ಯಾಗಮ 0;  // Not implemented for now!

	  ಪಕ್ಷ vpiSize:
	    ಪ್ರತ್ಯಾಗಮ 1;

	  ಪಕ್ಷ vpiConstType:
	    ಪ್ರತ್ಯಾಗಮ vpiRealConst;

	  ಪಕ್ಷ vpiSigned:
	    ಪ್ರತ್ಯಾಗಮ 1;

          ಪಕ್ಷ vpiAutomatic:
	    ಪ್ರತ್ಯಾಗಮ 0;

#ಯದಿ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          ಪಕ್ಷ _vpiFromThr:
	      ಪ್ರತ್ಯಾಗಮ _vpiNoThr;
#endif

	  ಸ್ವಭಾವ:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiRealConst\n", code);
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಪ್ರತ್ಯಾಗಮ 0;
      }
}


ಶೂನ್ಯ __vpiRealConst::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val)
{
      vpip_real_get_ಮೌಲ್ಯ(value, val);
}


vpiHandle vpip_make_real_const(ದ್ವಿಗುಣ value)
{
      __vpiRealConst*obj = ನೂತನ __vpiRealConst(value);
      ಪ್ರತ್ಯಾಗಮ obj;
}

ಸಂರಚನಾ __vpiRealParam  : ಸಾರ್ವಜನಿಕ __vpiRealConst {
      __vpiRealParam(__vpiScope* scope, ದ್ವಿಗುಣ val, ಧ್ರುವ ವರ್ಣ*name,
                     ತಾರ್ಕಿಕ local_flag, long file_idx, long lineno);
      ~__vpiRealParam() override;
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override;
      ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code) override;
      ವರ್ಣ*vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code) override;
      vpiHandle vpi_handle(ಪೂರ್ಣಾಙ್ಕ code) override;

    ಗುಪ್ತ:
      __vpiScope* scope_;
      ಧ್ರುವ ವರ್ಣ*basename_;
      ತಾರ್ಕಿಕ local_flag_;
      unsigned file_idx_;
      unsigned lineno_;
};


ಅನ್ತಃಸ್ಥ __vpiRealParam::__vpiRealParam(__vpiScope* scope, ದ್ವಿಗುಣ val, ಧ್ರುವ ವರ್ಣ*name,
                                      ತಾರ್ಕಿಕ local_flag, long file_idx, long lineno)
: __vpiRealConst(val)
{
      scope_ = scope;
      basename_ = name;
      local_flag_ = local_flag;
      ಪ್ರತಿಜ್ಞಾ(file_idx >= 0 && file_idx <= INT_MAX);
      file_idx_ = (unsigned) file_idx;
      ಪ್ರತಿಜ್ಞಾ(lineno >= 0 && lineno <= INT_MAX);
      lineno_ = (unsigned) lineno;
}

__vpiRealParam::~__vpiRealParam()
{
      delete[]basename_;
}


ಪೂರ್ಣಾಙ್ಕ __vpiRealParam::get_type_code(ಶೂನ್ಯ) ಧ್ರುವ
{ ಪ್ರತ್ಯಾಗಮ vpiParameter; }

ಪೂರ್ಣಾಙ್ಕ __vpiRealParam::vpi_get(ಪೂರ್ಣಾಙ್ಕ code)
{
    ವಿಕಲ್ಪ (code) {
      ಪಕ್ಷ vpi_ಸಾಲು_No :
        ಪ್ರತ್ಯಾಗಮ lineno_;

      ಪಕ್ಷ vpiLocalParam :
        ಪ್ರತ್ಯಾಗಮ local_flag_;

      ಸ್ವಭಾವ :
           ಪ್ರತ್ಯಾಗಮ __vpiRealConst::vpi_get(code);
    }
}

ವರ್ಣ* __vpiRealParam::vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code)
{
      ಯದಿ (code == vpiFile)
            ಪ್ರತ್ಯಾಗಮ simple_set_rbuf_str(file_names[file_idx_]);

      ಪ್ರತ್ಯಾಗಮ generic_get_str(code, scope_, basename_, ರಿಕ್ತ);
}

vpiHandle __vpiRealParam::vpi_handle(ಪೂರ್ಣಾಙ್ಕ code)
{
      ವಿಕಲ್ಪ (code) {
          ಪಕ್ಷ vpiScope:
            ಪ್ರತ್ಯಾಗಮ scope_;

	  ಪಕ್ಷ vpiModule:
	    ಪ್ರತ್ಯಾಗಮ vpip_module(scope_);

          ಸ್ವಭಾವ:
            ಪ್ರತ್ಯಾಗಮ 0;
      }
}


vpiHandle vpip_make_real_param(ಧ್ರುವ ವರ್ಣ*name, ದ್ವಿಗುಣ value,
                               ತಾರ್ಕಿಕ local_flag, long file_idx, long lineno)
{
      ಸಂರಚನಾ __vpiRealParam*obj = ನೂತನ __vpiRealParam(vpip_peek_current_scope(),
                                                     value, name, local_flag,
                                                     file_idx, lineno);
      ಪ್ರತ್ಯಾಗಮ obj;
}

/*
 * Make a VPI null constant
 */
ಅನ್ತಃಸ್ಥ __vpiNullConst::__vpiNullConst()
{ }

ಪೂರ್ಣಾಙ್ಕ __vpiNullConst::get_type_code(ಶೂನ್ಯ) ಧ್ರುವ
{ ಪ್ರತ್ಯಾಗಮ vpiConstant; }

ಪೂರ್ಣಾಙ್ಕ __vpiNullConst::vpi_get(ಪೂರ್ಣಾಙ್ಕ code)
{
      ವಿಕಲ್ಪ (code) {
	  ಪಕ್ಷ vpi_ಸಾಲು_No:
	    ಪ್ರತ್ಯಾಗಮ 0;  // Not implemented for now!

	  ಪಕ್ಷ vpiSize:
	    ಪ್ರತ್ಯಾಗಮ 32;

	  ಪಕ್ಷ vpiConstType:
	    ಪ್ರತ್ಯಾಗಮ vpiNullConst;

	  ಪಕ್ಷ vpiSigned:
	    ಪ್ರತ್ಯಾಗಮ 0;

          ಪಕ್ಷ vpiAutomatic:
	    ಪ್ರತ್ಯಾಗಮ 0;

#ಯದಿ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          ಪಕ್ಷ _vpiFromThr:
	      ಪ್ರತ್ಯಾಗಮ _vpiNoThr;
#endif

	  ಸ್ವಭಾವ:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiNullConst\n", code);
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಪ್ರತ್ಯಾಗಮ 0;
      }
}

ಶೂನ್ಯ __vpiNullConst::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val)
{
      ಸ್ಥಿರ ಧ್ರುವ size_t RBUF_USE_SIZE = 64 + 1;
      ವರ್ಣ*rbuf = static_cast<ವರ್ಣ *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));

      ವಿಕಲ್ಪ (val->format) {

	  ಪಕ್ಷ vpiObjTypeVal:
	    val->format = vpiStringVal;
	    // fall through
	  ಪಕ್ಷ vpiBinStrVal:
	  ಪಕ್ಷ vpiDecStrVal:
	  ಪಕ್ಷ vpiOctStrVal:
	  ಪಕ್ಷ vpiHexStrVal:
	  ಪಕ್ಷ vpiStringVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "null");
	    val->value.str = rbuf;
	    ಭಙ್ಗ;

          ಪಕ್ಷ vpiScalarVal:
	    val->value.scalar = vpi0;
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiIntVal:
	    val->value.integer = 0;
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiVectorVal:
	    val->value.vector = static_cast<p_vpi_vecval>
	                        (need_result_buf(2*ಪರಿಮಾಣ(s_vpi_vecval),
	                                         RBUF_VAL));
	    ಪ್ರತಿ (unsigned idx = 0; idx < 2; idx += 1) {
		  val->value.vector[idx].aval = 0;
		  val->value.vector[idx].bval = 0;
	    }
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiRealVal:
	    val->value.real = 0.0;
	    ಭಙ್ಗ;

	  ಸ್ವಭಾವ:
	    fprintf(stderr, "vvp error: format %d not supported "
		    "by vpiNullConst\n", (ಪೂರ್ಣಾಙ್ಕ)val->format);
	    val->format = vpiSuppressVal;
	    ಭಙ್ಗ;
      }
}

vpiHandle vpip_make_null_const()
{
      __vpiNullConst*obj = ನೂತನ __vpiNullConst;
      ಪ್ರತ್ಯಾಗಮ obj;
}

#ifdef CHECK_WITH_VALGRIND
ಶೂನ್ಯ constant_delete(vpiHandle item)
{
      ಪ್ರತಿಜ್ಞಾ(item->get_type_code() == vpiConstant);
      ವಿಕಲ್ಪ(vpi_get(vpiConstType, item)) {
	  ಪಕ್ಷ vpiStringConst:
	    delete dynamic_cast<__vpiStringConst*>(item);
	    ಭಙ್ಗ;
	  ಪಕ್ಷ vpiDecConst:
	    delete dynamic_cast<__vpiDecConst*>(item);
	    ಭಙ್ಗ;
	  ಪಕ್ಷ vpi_ದ್ವಿಮಾನ_Const:
	    delete dynamic_cast<__vpiBinaryConst*>(item);
	    ಭಙ್ಗ;
	  ಪಕ್ಷ vpiRealConst:
	    delete dynamic_cast<__vpiRealConst*>(item);
	    ಭಙ್ಗ;
	  ಪಕ್ಷ vpiNullConst:
	    delete dynamic_cast<__vpiNullConst*>(item);
	    ಭಙ್ಗ;
	  ಸ್ವಭಾವ:
	    ಪ್ರತಿಜ್ಞಾ(0);
      }
}

ಶೂನ್ಯ parameter_delete(vpiHandle item)
{
      ವಿಕಲ್ಪ(vpi_get(vpiConstType, item)) {
	  ಪಕ್ಷ vpiStringConst: {
	    ವರ್ಗ __vpiStringParam*rfp = dynamic_cast<__vpiStringParam*>(item);
	    delete rfp;
	    ಭಙ್ಗ; }
	  ಪಕ್ಷ vpi_ದ್ವಿಮಾನ_Const: {
	    ಸಂರಚನಾ __vpiBinaryParam*rfp = dynamic_cast<__vpiBinaryParam*>(item);
	    delete rfp;
	    ಭಙ್ಗ; }
	  ಪಕ್ಷ vpiRealConst: {
	    ಸಂರಚನಾ __vpiRealParam*rfp = dynamic_cast<__vpiRealParam*>(item);
	    delete rfp;
	    ಭಙ್ಗ; }
	  ಸ್ವಭಾವ:
	    ಪ್ರತಿಜ್ಞಾ(0);
      }
}
#endif
