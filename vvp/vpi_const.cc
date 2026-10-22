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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#ಆದರೆ_ಅಂತ್ಯ
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <climits>
# include  <cassert>
# include  "ivl_alloc.h"

ವರ್ಗ __vpiStringConst : ಸಾರ್ವಜನಿಕ __vpiHandle {
    ಸಾರ್ವಜನಿಕ:
      ಸ್ಪಷ್ಟ __vpiStringConst(ಅಕ್ಷರ*val);
      ~__vpiStringConst() override;
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override;
      ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ code) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;

    ಖಾಸಗಿ:
      ಶೂನ್ಯ process_string_();
    ಖಾಸಗಿ:
      ಅಕ್ಷರ*ಮೌಲ್ಯ_2;
      size_t ಮೌಲ್ಯ_len;
};

ಸಾಲೊಳಗೆ __vpiStringConst::__vpiStringConst(ಅಕ್ಷರ*v)
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
      ಅಕ್ಷರ*chr = ಮೌಲ್ಯ_2;
      ಅಕ್ಷರ*dp = ಮೌಲ್ಯ_2;

      ಆಗ (*chr) {
	    ಅಕ್ಷರ ಮುಂದಿನ_char = *chr;

	      /* Process octal escapes that I might find. */
	    ಆದರೆ (*chr == '\\') {
		  ಗಾಗಿ (ಪೂರ್ಣಾಂಕ idx = 1 ;  idx <= 3 ;  idx += 1) {
			ಪ್ರತಿಜ್ಞಾ(chr[idx] != 0);
			ಪ್ರತಿಜ್ಞಾ(chr[idx] < '8');
			ಪ್ರತಿಜ್ಞಾ(chr[idx] >= '0');
			ಮುಂದಿನ_char = ಮುಂದಿನ_char*8 + chr[idx] - '0';
		  }
		  chr += 3;
	    }
	    *dp++ = ಮುಂದಿನ_char;
	    chr += 1;
      }
      *dp = 0;
      ಮೌಲ್ಯ_len = dp - ಮೌಲ್ಯ_2;
}

__vpiStringConst::~__vpiStringConst()
{
      ಅಳಿಸು[] ಮೌಲ್ಯ_2;
}

ಪೂರ್ಣಾಂಕ __vpiStringConst::get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ
{ ಹಿಂತಿರುಗಿ vpiConstant; }

ಪೂರ್ಣಾಂಕ __vpiStringConst::vpi_get(ಪೂರ್ಣಾಂಕ code)
{
      ಆಯ್ಕೆ (code) {
          ಸ್ಥಿತಿ vpi_ಗಾತ್ರ:
	    ಹಿಂತಿರುಗಿ ಮೌಲ್ಯ_len * 8;

          ಸ್ಥಿತಿ vpiSigned:
	      ಹಿಂತಿರುಗಿ 0;

	  ಸ್ಥಿತಿ vpiConstType:
	      ಹಿಂತಿರುಗಿ vpiStringConst;

          ಸ್ಥಿತಿ vpiAutomatic:
	      ಹಿಂತಿರುಗಿ 0;

#ಆದರೆ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          ಸ್ಥಿತಿ _vpiFromThr:
	      ಹಿಂತಿರುಗಿ _vpiNoThr;
#ಆದರೆ_ಅಂತ್ಯ

	  ಪೂರ್ವನಿಯೋಜಿತ:
	      fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiStringConst\n", code);
	      ಪ್ರತಿಜ್ಞಾ(0);
	      ಹಿಂತಿರುಗಿ 0;
      }
}

ಶೂನ್ಯ __vpiStringConst::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ vp)
{
      ಚಿಹ್ನೆರಹಿತ uint_ಮೌಲ್ಯ;
      p_vpi_vecval vecp;
      ಚಿಹ್ನೆರಹಿತ size = ಮೌಲ್ಯ_len;
      ಅಕ್ಷರ*rbuf = 0;
      ಅಕ್ಷರ*cp;

      ಆಯ್ಕೆ (vp->format) {
	  ಸ್ಥಿತಿ vpiObjTypeVal:
	      /* String parameters by default have vpiStringVal values. */
	    vp->format = vpiStringVal;
	    // fallthrough
	  ಸ್ಥಿತಿ vpiStringVal:
	    cp = ಮೌಲ್ಯ_2;
	    rbuf = static_cast<ಅಕ್ಷರ *>(need_ಫಲಿತಾಂಶ_buf(size + 1, RBUF_VAL));
	    vp->value.str = rbuf;

	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ i = 0; i < size; i++) {
		  // Ignore leading null-bytes and replace other null-bytes with space.
		  // The LRM is not entirely clear on how null bytes should be handled.
		  // This is the implementation chosen for iverilog.
		  ಆದರೆ (*cp)
			*rbuf++ = *cp;
		  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (rbuf != vp->value.str)
			*rbuf++ = ' ';

		  cp++;
	    }
	    *rbuf = '\0';
	    ನಿಲ್ಲಿಸು;

          ಸ್ಥಿತಿ vpiDecStrVal:
	    // Take the (up to 4) characters of the text, convert the characters
	    // to a numerical value, and convert that value to a decimal string.
	    // For example, the string "A" is ASCII 65, so the resulting string
	    // will be "65". and the string "AB" is 65*256 + 66 == 16706, so
	    // the resulting string is "16706". The "size" is the number of
	    // characters of input text to put to work.
	    ಆದರೆ (size > 4){
		  // We only support standard integers. Ignore other bytes...
		  size = 4;
		  fprintf(stderr, "Warning (vpi_const.cc): %%d on constant strings only looks "
			  "at first 4 bytes!\n");
	    }
	    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ size_t RBUF_USE_ಗಾತ್ರ = 12;
	    rbuf = static_cast<ಅಕ್ಷರ *>(need_ಫಲಿತಾಂಶ_buf(RBUF_USE_ಗಾತ್ರ, RBUF_VAL));
	    uint_ಮೌಲ್ಯ = 0;
	    ಗಾಗಿ(ಚಿಹ್ನೆರಹಿತ i=0; i<size; i += 1){
		  uint_ಮೌಲ್ಯ <<=8;
		  uint_ಮೌಲ್ಯ += (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(ಮೌಲ್ಯ_2[i]);
	    }
	    snprintf(rbuf, RBUF_USE_ಗಾತ್ರ, "%u", uint_ಮೌಲ್ಯ);
	    vp->value.str = rbuf;
	    ನಿಲ್ಲಿಸು;

          ಸ್ಥಿತಿ vpiBinStrVal:
	      rbuf = static_cast<ಅಕ್ಷರ *>(need_ಫಲಿತಾಂಶ_buf(8 * size + 1, RBUF_VAL));
	      cp = rbuf;
	      ಗಾಗಿ(ಚಿಹ್ನೆರಹಿತ i=0; i<size; i += 1){
		  ಗಾಗಿ(ಪೂರ್ಣಾಂಕ ಬಿಟ್=7; ಬಿಟ್>=0; ಬಿಟ್ -= 1){
		      *cp++ = "01"[ (ಮೌಲ್ಯ_2[i]>>ಬಿಟ್)&1 ];
		  }
	      }
	      *cp = 0;
	      vp->value.str = rbuf;
	      ನಿಲ್ಲಿಸು;

          ಸ್ಥಿತಿ vpiHexStrVal:
	      rbuf = static_cast<ಅಕ್ಷರ *>(need_ಫಲಿತಾಂಶ_buf(2 * size + 1, RBUF_VAL));
	      cp = rbuf;
	      ಗಾಗಿ(ಚಿಹ್ನೆರಹಿತ i=0; i<size; i += 1){
		  ಗಾಗಿ(ಪೂರ್ಣಾಂಕ nibble=1; nibble>=0; nibble -= 1){
		      *cp++ = "0123456789abcdef"[ (ಮೌಲ್ಯ_2[i]>>(nibble*4))&15 ];
		  }
	      }
	      *cp = 0;
	      vp->value.str = rbuf;
	      ನಿಲ್ಲಿಸು;

          ಸ್ಥಿತಿ vpiOctStrVal:
	      fprintf(stderr, "ERROR (vpi_const.cc): %%o display of constant strings not yet implemented\n");
	      ಪ್ರತಿಜ್ಞಾ(0);
	      ನಿಲ್ಲಿಸು;

          ಸ್ಥಿತಿ vpiIntVal:
	      vp->value.integer = 0;
	      ಗಾಗಿ(ಚಿಹ್ನೆರಹಿತ i=0; i<size; i += 1){
		  ಗಾಗಿ(ಪೂರ್ಣಾಂಕ ಬಿಟ್=7; ಬಿಟ್>=0; ಬಿಟ್ -= 1){
		      vp->value.integer <<= 1;
		      vp->value.integer += (ಮೌಲ್ಯ_2[i]>>ಬಿಟ್)&1;
		  }
	      }
	      ನಿಲ್ಲಿಸು;

          ಸ್ಥಿತಿ vpi_ಸದಿಶ_Val:
              vp->value.vector = static_cast<p_vpi_vecval>
                                 (need_ಫಲಿತಾಂಶ_buf((size+3)/4*
                                                   ಗಾತ್ರ(s_vpi_vecval),
                                                  RBUF_VAL));
              uint_ಮೌಲ್ಯ = 0;
              vecp = vp->value.vector;
              vecp->aval = vecp->bval = 0;
	      ಗಾಗಿ(ಚಿಹ್ನೆರಹಿತ i=0; i<size; i += 1){
		  vecp->aval |= ಮೌಲ್ಯ_2[i] << uint_ಮೌಲ್ಯ*8;
		  uint_ಮೌಲ್ಯ += 1;
		  ಆದರೆ (uint_ಮೌಲ್ಯ > 3) {
		      uint_ಮೌಲ್ಯ = 0;
		      vecp += 1;
		      vecp->aval = vecp->bval = 0;
		  }
	      }
	      ನಿಲ್ಲಿಸು;


	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "ERROR (vpi_const.cc): vp->format: %d\n",
	            (ಪೂರ್ಣಾಂಕ)vp->format);
	    ಪ್ರತಿಜ್ಞಾ(0);

	    vp->format = vpiSuppressVal;
	    ನಿಲ್ಲಿಸು;
      }
}


ರಚನೆ __vpiStringConstTEMP : ಸಾರ್ವಜನಿಕ __vpiStringConst {
      ಸ್ಪಷ್ಟ ಸಾಲೊಳಗೆ __vpiStringConstTEMP(ಅಕ್ಷರ*v) : __vpiStringConst(v) { }
      free_object_fun_t free_object_fun(ಶೂನ್ಯ) override;
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ free_ತಾತ್ಕಾಲಿಕ_string(vpiHandle obj)
{
      ರಚನೆ __vpiStringConstTEMP*rfp = dynamic_cast<__vpiStringConstTEMP*>(obj);
      ಅಳಿಸು rfp;
      ಹಿಂತಿರುಗಿ 1;
}

__vpiHandle::free_object_fun_t __vpiStringConstTEMP::free_object_fun(ಶೂನ್ಯ)
{ ಹಿಂತಿರುಗಿ &free_ತಾತ್ಕಾಲಿಕ_string; }

vpiHandle vpip_make_string_const(ಅಕ್ಷರ*ಪಠ್ಯ, ತಾರ್ಕಿಕ persistent_flag)
{
      __vpiStringConst*obj;

      obj = persistent_flag? ಹೊಸ __vpiStringConst(ಪಠ್ಯ) : ಹೊಸ __vpiStringConstTEMP(ಪಠ್ಯ);

      ಹಿಂತಿರುಗಿ obj;
}


ವರ್ಗ __vpiStringParam  : ಸಾರ್ವಜನಿಕ __vpiStringConst {
    ಸಾರ್ವಜನಿಕ:
      __vpiStringParam(__vpiScope* scope, ಅಕ್ಷರ*txt, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name,
                       ತಾರ್ಕಿಕ local_flag, ದೀರ್ಘ file_idx, ದೀರ್ಘ lineno);
      ~__vpiStringParam() override;
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override;
      ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ code) override;
      ಅಕ್ಷರ*vpi_get_str(ಪೂರ್ಣಾಂಕ code) override;
      vpiHandle vpi_handle(ಪೂರ್ಣಾಂಕ code) override;

    ಖಾಸಗಿ:
      __vpiScope* scope_;
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*basename_;
      ತಾರ್ಕಿಕ       local_flag_;
      ಚಿಹ್ನೆರಹಿತ   file_idx_;
      ಚಿಹ್ನೆರಹಿತ   lineno_;
};

ಸಾಲೊಳಗೆ __vpiStringParam::__vpiStringParam(__vpiScope* scope, ಅಕ್ಷರ*txt, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*nam,
                                          ತಾರ್ಕಿಕ local_flag, ದೀರ್ಘ file_idx, ದೀರ್ಘ lineno)
: __vpiStringConst(txt)
{
      scope_ = scope;
      basename_ = nam;
      local_flag_ = local_flag;
      ಪ್ರತಿಜ್ಞಾ(file_idx >= 0 && file_idx <= ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ);
      file_idx_ = (ಚಿಹ್ನೆರಹಿತ) file_idx;
      ಪ್ರತಿಜ್ಞಾ(lineno >= 0 && lineno <= ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ);
      lineno_ = (ಚಿಹ್ನೆರಹಿತ) lineno;
}

__vpiStringParam::~__vpiStringParam()
{
      ಅಳಿಸು[]basename_;
}

ಪೂರ್ಣಾಂಕ __vpiStringParam::get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ
{ ಹಿಂತಿರುಗಿ vpiParameter; }

ಪೂರ್ಣಾಂಕ __vpiStringParam::vpi_get(ಪೂರ್ಣಾಂಕ code)
{
    ಆಯ್ಕೆ (code) {
       ಸ್ಥಿತಿ vpi_ಸಾಲು_No :
         ಹಿಂತಿರುಗಿ lineno_;

       ಸ್ಥಿತಿ vpiLocalParam :
         ಹಿಂತಿರುಗಿ local_flag_;

       ಪೂರ್ವನಿಯೋಜಿತ :
         ಹಿಂತಿರುಗಿ __vpiStringConst::vpi_get(code);
    }
}


ಅಕ್ಷರ*__vpiStringParam::vpi_get_str(ಪೂರ್ಣಾಂಕ code)
{
      ಆದರೆ (code == vpiFile) {
	    ಹಿಂತಿರುಗಿ simple_ಗಣ_rbuf_str(file_ಹೆಸರುಗಳು[file_idx_]);
      }

      ಹಿಂತಿರುಗಿ generic_get_str(code, scope_, basename_, ಖಾಲಿ);
}


vpiHandle __vpiStringParam::vpi_handle(ಪೂರ್ಣಾಂಕ code)
{
      ಆಯ್ಕೆ (code) {
	  ಸ್ಥಿತಿ vpiScope:
	    ಹಿಂತಿರುಗಿ scope_;

	  ಸ್ಥಿತಿ vpiModule:
	    ಹಿಂತಿರುಗಿ vpip_module(scope_);

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ 0;
      }
}

vpiHandle vpip_make_string_param(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, ಅಕ್ಷರ*ಪಠ್ಯ,
                                 ತಾರ್ಕಿಕ local_flag, ದೀರ್ಘ file_idx, ದೀರ್ಘ lineno)
{
      __vpiStringParam*obj = ಹೊಸ __vpiStringParam(vpip_peek_current_scope(), ಪಠ್ಯ, name,
                                                  local_flag, file_idx, lineno);

      ಹಿಂತಿರುಗಿ obj;
}



ಸಾಲೊಳಗೆ __vpiBinaryConst::__vpiBinaryConst()
: signed_flag(0), sized_flag(0)
{ }

ಪೂರ್ಣಾಂಕ __vpiBinaryConst::get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ
{ ಹಿಂತಿರುಗಿ vpiConstant; }

ಪೂರ್ಣಾಂಕ __vpiBinaryConst::vpi_get(ಪೂರ್ಣಾಂಕ code)
{
      ಆಯ್ಕೆ (code) {
	  ಸ್ಥಿತಿ vpiConstType:
	    ಹಿಂತಿರುಗಿ vpi_ದ್ವಿಮಾನ_Const;

	  ಸ್ಥಿತಿ vpi_ಸಾಲು_No:
	    ಹಿಂತಿರುಗಿ 0;  // Not implemented for now!

	  ಸ್ಥಿತಿ vpiSigned:
	    ಹಿಂತಿರುಗಿ signed_flag? 1 : 0;

	  ಸ್ಥಿತಿ vpi_ಗಾತ್ರ:
	    ಹಿಂತಿರುಗಿ ಬಿಟ್ಟುಗಳು.size();

          ಸ್ಥಿತಿ vpiAutomatic:
	    ಹಿಂತಿರುಗಿ 0;

#ಆದರೆ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          ಸ್ಥಿತಿ _vpiFromThr:
	      ಹಿಂತಿರುಗಿ _vpiNoThr;
#ಆದರೆ_ಅಂತ್ಯ

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiBinaryConst\n", code);
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಹಿಂತಿರುಗಿ 0;
      }
}


ಶೂನ್ಯ __vpiBinaryConst::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val)
{
      ಆಯ್ಕೆ (val->format) {

	  ಸ್ಥಿತಿ vpiObjTypeVal:
	  ಸ್ಥಿತಿ vpiBinStrVal:
	  ಸ್ಥಿತಿ vpiDecStrVal:
	  ಸ್ಥಿತಿ vpiOctStrVal:
	  ಸ್ಥಿತಿ vpiHexStrVal:
          ಸ್ಥಿತಿ vpiScalarVal:
	  ಸ್ಥಿತಿ vpiIntVal:
	  ಸ್ಥಿತಿ vpi_ಸದಿಶ_Val:
	  ಸ್ಥಿತಿ vpiStringVal:
	  ಸ್ಥಿತಿ vpiRealVal:
	    vpip_vec4_get_ಮೌಲ್ಯ(ಬಿಟ್ಟುಗಳು, ಬಿಟ್ಟುಗಳು.size(), signed_flag, val);
	    ನಿಲ್ಲಿಸು;

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "vvp error: format %d not supported "
		    "by vpiBinaryConst\n", (ಪೂರ್ಣಾಂಕ)val->format);
	    val->format = vpiSuppressVal;
	    ನಿಲ್ಲಿಸು;
      }
}

/*
 * Make a VPI constant from a vector string. The string is normally a
 * ASCII string, with each letter a 4-value bit. The first character
 * may be an 's' if the vector is signed.
 */
vpiHandle vpip_make_ದ್ವಿಮಾನ_const(ಚಿಹ್ನೆರಹಿತ wid, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*ಬಿಟ್ಟುಗಳು)
{
      ರಚನೆ __vpiBinaryConst*obj = ಹೊಸ __vpiBinaryConst;

      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*bp = ಬಿಟ್ಟುಗಳು;
      ಆದರೆ (*bp == 's') {
	    bp += 1;
	    obj->signed_flag = 1;
      }

      obj->ಬಿಟ್ಟುಗಳು = vector4_from_ಪಠ್ಯ(bp, wid);

      ಹಿಂತಿರುಗಿ obj;
}

vvp_vector4_t vector4_from_ಪಠ್ಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*ಬಿಟ್ಟುಗಳು, ಚಿಹ್ನೆರಹಿತ wid)
{
      vvp_vector4_t res (wid);

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t val = BIT4_0;
	    ಆಯ್ಕೆ (ಬಿಟ್ಟುಗಳು[wid-idx-1]) {
		ಸ್ಥಿತಿ '0':
		  val = BIT4_0;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ '1':
		  val = BIT4_1;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'x':
		  val = BIT4_X;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'z':
		  val = BIT4_Z;
		  ನಿಲ್ಲಿಸು;
	    }

	    res.ಗಣ_ಬಿಟ್(idx, val);
      }

      ಹಿಂತಿರುಗಿ res;
}

ರಚನೆ __vpiBinaryParam  : ಸಾರ್ವಜನಿಕ __vpiBinaryConst {
      __vpiBinaryParam(__vpiScope*scope, ಸ್ಥಿರಾಂಕ vvp_vector4_t&b, ತಾರ್ಕಿಕ is_signed,
                       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, ತಾರ್ಕಿಕ local_flag, ದೀರ್ಘ file_idx, ದೀರ್ಘ lineno);
      ~__vpiBinaryParam() override;
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override;
      ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ code) override;
      ಅಕ್ಷರ*vpi_get_str(ಪೂರ್ಣಾಂಕ code) override;
      vpiHandle vpi_handle(ಪೂರ್ಣಾಂಕ code) override;

    ಖಾಸಗಿ:
      __vpiScope*scope_;
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*basename_;
      ತಾರ್ಕಿಕ     local_flag_;
      ಚಿಹ್ನೆರಹಿತ file_idx_;
      ಚಿಹ್ನೆರಹಿತ lineno_;
};

ಸಾಲೊಳಗೆ __vpiBinaryParam::__vpiBinaryParam(__vpiScope*scope, ಸ್ಥಿರಾಂಕ vvp_vector4_t&b,
                                          ತಾರ್ಕಿಕ is_signed, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*nam,
                                          ತಾರ್ಕಿಕ local_flag, ದೀರ್ಘ file_idx, ದೀರ್ಘ lineno)
{
      scope_ = scope;
      ಬಿಟ್ಟುಗಳು = b;
      signed_flag = is_signed ? 1 : 0;
      sized_flag = 0;
      basename_ = nam;
      local_flag_ = local_flag;
      ಪ್ರತಿಜ್ಞಾ(file_idx >= 0 && file_idx <= ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ);
      file_idx_ = (ಚಿಹ್ನೆರಹಿತ) file_idx;
      ಪ್ರತಿಜ್ಞಾ(lineno >= 0 && lineno <= ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ);
      lineno_ = (ಚಿಹ್ನೆರಹಿತ) lineno;
}

__vpiBinaryParam::~__vpiBinaryParam()
{
      ಅಳಿಸು[]basename_;
}

ಪೂರ್ಣಾಂಕ __vpiBinaryParam::get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ
{ ಹಿಂತಿರುಗಿ vpiParameter; }

ಪೂರ್ಣಾಂಕ __vpiBinaryParam::vpi_get(ಪೂರ್ಣಾಂಕ code)
{
    ಆಯ್ಕೆ (code) {
      ಸ್ಥಿತಿ vpi_ಸಾಲು_No :
        ಹಿಂತಿರುಗಿ lineno_;

      ಸ್ಥಿತಿ vpiLocalParam :
        ಹಿಂತಿರುಗಿ local_flag_;

      ಪೂರ್ವನಿಯೋಜಿತ :
        ಹಿಂತಿರುಗಿ __vpiBinaryConst::vpi_get(code);
    }
}

ಅಕ್ಷರ*__vpiBinaryParam::vpi_get_str(ಪೂರ್ಣಾಂಕ code)
{
      ಆದರೆ (code == vpiFile)
	    ಹಿಂತಿರುಗಿ simple_ಗಣ_rbuf_str(file_ಹೆಸರುಗಳು[file_idx_]);

      ಹಿಂತಿರುಗಿ generic_get_str(code, scope_, basename_, ಖಾಲಿ);
}


vpiHandle __vpiBinaryParam::vpi_handle(ಪೂರ್ಣಾಂಕ code)
{
      ಆಯ್ಕೆ (code) {
	  ಸ್ಥಿತಿ vpiScope:
	    ಹಿಂತಿರುಗಿ scope_;

	  ಸ್ಥಿತಿ vpiModule:
	    ಹಿಂತಿರುಗಿ vpip_module(scope_);

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ 0;
      }
}


vpiHandle vpip_make_ದ್ವಿಮಾನ_param(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್ಟುಗಳು,
				 ತಾರ್ಕಿಕ signed_flag, ತಾರ್ಕಿಕ local_flag,
				 ದೀರ್ಘ file_idx, ದೀರ್ಘ lineno)
{
      ರಚನೆ __vpiBinaryParam*obj = ಹೊಸ __vpiBinaryParam(vpip_peek_current_scope(),
                                                         ಬಿಟ್ಟುಗಳು, signed_flag, name,
                                                         local_flag, file_idx, lineno);

      ಹಿಂತಿರುಗಿ obj;
}



__vpiDecConst::__vpiDecConst(ಪೂರ್ಣಾಂಕ val)
{
      value = val;
}

__vpiDecConst::__vpiDecConst(ಸ್ಥಿರಾಂಕ __vpiDecConst&that)
: __vpiHandle(), value(that.value)
{
}


ಪೂರ್ಣಾಂಕ __vpiDecConst::get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ
{ ಹಿಂತಿರುಗಿ vpiConstant; }

ಪೂರ್ಣಾಂಕ __vpiDecConst::vpi_get(ಪೂರ್ಣಾಂಕ code)
{
      ಆಯ್ಕೆ (code) {
	  ಸ್ಥಿತಿ vpiConstType:
	    ಹಿಂತಿರುಗಿ vpiDecConst;

	  ಸ್ಥಿತಿ vpiSigned:
	    ಹಿಂತಿರುಗಿ 1;

	  ಸ್ಥಿತಿ vpi_ಗಾತ್ರ:
	    ಹಿಂತಿರುಗಿ 32;

          ಸ್ಥಿತಿ vpiAutomatic:
	    ಹಿಂತಿರುಗಿ 0;

#ಆದರೆ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          ಸ್ಥಿತಿ _vpiFromThr:
	      ಹಿಂತಿರುಗಿ _vpiNoThr;
#ಆದರೆ_ಅಂತ್ಯ

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiDecConst\n", code);
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಹಿಂತಿರುಗಿ 0;
      }
}


ಶೂನ್ಯ __vpiDecConst::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ vp)
{
      ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ size_t RBUF_USE_ಗಾತ್ರ = 64 + 1;
      ಅಕ್ಷರ*rbuf = static_cast<ಅಕ್ಷರ *>(need_ಫಲಿತಾಂಶ_buf(RBUF_USE_ಗಾತ್ರ, RBUF_VAL));
      ಅಕ್ಷರ*cp = rbuf;

      ಆಯ್ಕೆ (vp->format) {

	  ಸ್ಥಿತಿ vpiObjTypeVal:
	  ಸ್ಥಿತಿ vpiIntVal: {
		vp->value.integer = value;
		ನಿಲ್ಲಿಸು;
	  }

          ಸ್ಥಿತಿ vpiDecStrVal:
	      snprintf(rbuf, RBUF_USE_ಗಾತ್ರ, "%d", value);
	      vp->value.str = rbuf;
	      ನಿಲ್ಲಿಸು;

          ಸ್ಥಿತಿ vpiBinStrVal:
	      ಗಾಗಿ(ಪೂರ್ಣಾಂಕ ಬಿಟ್=31; ಬಿಟ್>=0;ಬಿಟ್--){
		  *cp++ = "01"[ (static_cast<ಚಿಹ್ನೆರಹಿತ>(value)>>ಬಿಟ್)&1 ];
	      }
	      *cp = 0;

	      vp->value.str = rbuf;
	      ನಿಲ್ಲಿಸು;

          ಸ್ಥಿತಿ vpiHexStrVal:
	      snprintf(rbuf, RBUF_USE_ಗಾತ್ರ, "%08x", value);
	      vp->value.str = rbuf;
	      ನಿಲ್ಲಿಸು;

          ಸ್ಥಿತಿ vpiOctStrVal:
	      snprintf(rbuf, RBUF_USE_ಗಾತ್ರ, "%011x", value);
	      vp->value.str = rbuf;
	      ನಿಲ್ಲಿಸು;

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "vvp error (vpi_const.cc): format %d not supported "
		    "by vpiDecConst\n", (ಪೂರ್ಣಾಂಕ)vp->format);
	    vp->format = vpiSuppressVal;
	    ನಿಲ್ಲಿಸು;
      }
}


ಸಾಲೊಳಗೆ __vpiRealConst::__vpiRealConst(ದ್ವಿಗುಣ val)
: value(val)
{ }

ಪೂರ್ಣಾಂಕ __vpiRealConst::get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ
{ ಹಿಂತಿರುಗಿ vpiConstant; }

ಪೂರ್ಣಾಂಕ __vpiRealConst::vpi_get(ಪೂರ್ಣಾಂಕ code)
{
      ಆಯ್ಕೆ (code) {
	  ಸ್ಥಿತಿ vpi_ಸಾಲು_No:
	    ಹಿಂತಿರುಗಿ 0;  // Not implemented for now!

	  ಸ್ಥಿತಿ vpi_ಗಾತ್ರ:
	    ಹಿಂತಿರುಗಿ 1;

	  ಸ್ಥಿತಿ vpiConstType:
	    ಹಿಂತಿರುಗಿ vpiRealConst;

	  ಸ್ಥಿತಿ vpiSigned:
	    ಹಿಂತಿರುಗಿ 1;

          ಸ್ಥಿತಿ vpiAutomatic:
	    ಹಿಂತಿರುಗಿ 0;

#ಆದರೆ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          ಸ್ಥಿತಿ _vpiFromThr:
	      ಹಿಂತಿರುಗಿ _vpiNoThr;
#ಆದರೆ_ಅಂತ್ಯ

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiRealConst\n", code);
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಹಿಂತಿರುಗಿ 0;
      }
}


ಶೂನ್ಯ __vpiRealConst::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val)
{
      vpip_real_get_ಮೌಲ್ಯ(value, val);
}


vpiHandle vpip_make_real_const(ದ್ವಿಗುಣ value)
{
      __vpiRealConst*obj = ಹೊಸ __vpiRealConst(value);
      ಹಿಂತಿರುಗಿ obj;
}

ರಚನೆ __vpiRealParam  : ಸಾರ್ವಜನಿಕ __vpiRealConst {
      __vpiRealParam(__vpiScope* scope, ದ್ವಿಗುಣ val, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name,
                     ತಾರ್ಕಿಕ local_flag, ದೀರ್ಘ file_idx, ದೀರ್ಘ lineno);
      ~__vpiRealParam() override;
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override;
      ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ code) override;
      ಅಕ್ಷರ*vpi_get_str(ಪೂರ್ಣಾಂಕ code) override;
      vpiHandle vpi_handle(ಪೂರ್ಣಾಂಕ code) override;

    ಖಾಸಗಿ:
      __vpiScope* scope_;
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*basename_;
      ತಾರ್ಕಿಕ local_flag_;
      ಚಿಹ್ನೆರಹಿತ file_idx_;
      ಚಿಹ್ನೆರಹಿತ lineno_;
};


ಸಾಲೊಳಗೆ __vpiRealParam::__vpiRealParam(__vpiScope* scope, ದ್ವಿಗುಣ val, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name,
                                      ತಾರ್ಕಿಕ local_flag, ದೀರ್ಘ file_idx, ದೀರ್ಘ lineno)
: __vpiRealConst(val)
{
      scope_ = scope;
      basename_ = name;
      local_flag_ = local_flag;
      ಪ್ರತಿಜ್ಞಾ(file_idx >= 0 && file_idx <= ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ);
      file_idx_ = (ಚಿಹ್ನೆರಹಿತ) file_idx;
      ಪ್ರತಿಜ್ಞಾ(lineno >= 0 && lineno <= ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ);
      lineno_ = (ಚಿಹ್ನೆರಹಿತ) lineno;
}

__vpiRealParam::~__vpiRealParam()
{
      ಅಳಿಸು[]basename_;
}


ಪೂರ್ಣಾಂಕ __vpiRealParam::get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ
{ ಹಿಂತಿರುಗಿ vpiParameter; }

ಪೂರ್ಣಾಂಕ __vpiRealParam::vpi_get(ಪೂರ್ಣಾಂಕ code)
{
    ಆಯ್ಕೆ (code) {
      ಸ್ಥಿತಿ vpi_ಸಾಲು_No :
        ಹಿಂತಿರುಗಿ lineno_;

      ಸ್ಥಿತಿ vpiLocalParam :
        ಹಿಂತಿರುಗಿ local_flag_;

      ಪೂರ್ವನಿಯೋಜಿತ :
           ಹಿಂತಿರುಗಿ __vpiRealConst::vpi_get(code);
    }
}

ಅಕ್ಷರ* __vpiRealParam::vpi_get_str(ಪೂರ್ಣಾಂಕ code)
{
      ಆದರೆ (code == vpiFile)
            ಹಿಂತಿರುಗಿ simple_ಗಣ_rbuf_str(file_ಹೆಸರುಗಳು[file_idx_]);

      ಹಿಂತಿರುಗಿ generic_get_str(code, scope_, basename_, ಖಾಲಿ);
}

vpiHandle __vpiRealParam::vpi_handle(ಪೂರ್ಣಾಂಕ code)
{
      ಆಯ್ಕೆ (code) {
          ಸ್ಥಿತಿ vpiScope:
            ಹಿಂತಿರುಗಿ scope_;

	  ಸ್ಥಿತಿ vpiModule:
	    ಹಿಂತಿರುಗಿ vpip_module(scope_);

          ಪೂರ್ವನಿಯೋಜಿತ:
            ಹಿಂತಿರುಗಿ 0;
      }
}


vpiHandle vpip_make_real_param(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, ದ್ವಿಗುಣ value,
                               ತಾರ್ಕಿಕ local_flag, ದೀರ್ಘ file_idx, ದೀರ್ಘ lineno)
{
      ರಚನೆ __vpiRealParam*obj = ಹೊಸ __vpiRealParam(vpip_peek_current_scope(),
                                                     value, name, local_flag,
                                                     file_idx, lineno);
      ಹಿಂತಿರುಗಿ obj;
}

/*
 * Make a VPI null constant
 */
ಸಾಲೊಳಗೆ __vpiNullConst::__vpiNullConst()
{ }

ಪೂರ್ಣಾಂಕ __vpiNullConst::get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ
{ ಹಿಂತಿರುಗಿ vpiConstant; }

ಪೂರ್ಣಾಂಕ __vpiNullConst::vpi_get(ಪೂರ್ಣಾಂಕ code)
{
      ಆಯ್ಕೆ (code) {
	  ಸ್ಥಿತಿ vpi_ಸಾಲು_No:
	    ಹಿಂತಿರುಗಿ 0;  // Not implemented for now!

	  ಸ್ಥಿತಿ vpi_ಗಾತ್ರ:
	    ಹಿಂತಿರುಗಿ 32;

	  ಸ್ಥಿತಿ vpiConstType:
	    ಹಿಂತಿರುಗಿ vpiNullConst;

	  ಸ್ಥಿತಿ vpiSigned:
	    ಹಿಂತಿರುಗಿ 0;

          ಸ್ಥಿತಿ vpiAutomatic:
	    ಹಿಂತಿರುಗಿ 0;

#ಆದರೆ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          ಸ್ಥಿತಿ _vpiFromThr:
	      ಹಿಂತಿರುಗಿ _vpiNoThr;
#ಆದರೆ_ಅಂತ್ಯ

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiNullConst\n", code);
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಹಿಂತಿರುಗಿ 0;
      }
}

ಶೂನ್ಯ __vpiNullConst::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val)
{
      ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ size_t RBUF_USE_ಗಾತ್ರ = 64 + 1;
      ಅಕ್ಷರ*rbuf = static_cast<ಅಕ್ಷರ *>(need_ಫಲಿತಾಂಶ_buf(RBUF_USE_ಗಾತ್ರ, RBUF_VAL));

      ಆಯ್ಕೆ (val->format) {

	  ಸ್ಥಿತಿ vpiObjTypeVal:
	    val->format = vpiStringVal;
	    // fall through
	  ಸ್ಥಿತಿ vpiBinStrVal:
	  ಸ್ಥಿತಿ vpiDecStrVal:
	  ಸ್ಥಿತಿ vpiOctStrVal:
	  ಸ್ಥಿತಿ vpiHexStrVal:
	  ಸ್ಥಿತಿ vpiStringVal:
	    snprintf(rbuf, RBUF_USE_ಗಾತ್ರ, "null");
	    val->value.str = rbuf;
	    ನಿಲ್ಲಿಸು;

          ಸ್ಥಿತಿ vpiScalarVal:
	    val->value.scalar = vpi0;
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiIntVal:
	    val->value.integer = 0;
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpi_ಸದಿಶ_Val:
	    val->value.vector = static_cast<p_vpi_vecval>
	                        (need_ಫಲಿತಾಂಶ_buf(2*ಗಾತ್ರ(s_vpi_vecval),
	                                         RBUF_VAL));
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0; idx < 2; idx += 1) {
		  val->value.vector[idx].aval = 0;
		  val->value.vector[idx].bval = 0;
	    }
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiRealVal:
	    val->value.real = 0.0;
	    ನಿಲ್ಲಿಸು;

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "vvp error: format %d not supported "
		    "by vpiNullConst\n", (ಪೂರ್ಣಾಂಕ)val->format);
	    val->format = vpiSuppressVal;
	    ನಿಲ್ಲಿಸು;
      }
}

vpiHandle vpip_make_null_const()
{
      __vpiNullConst*obj = ಹೊಸ __vpiNullConst;
      ಹಿಂತಿರುಗಿ obj;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಶೂನ್ಯ constant_delete(vpiHandle ಅಂಶ)
{
      ಪ್ರತಿಜ್ಞಾ(ಅಂಶ->get_type_ಸಂಕೇತ() == vpiConstant);
      ಆಯ್ಕೆ(vpi_get(vpiConstType, ಅಂಶ)) {
	  ಸ್ಥಿತಿ vpiStringConst:
	    ಅಳಿಸು dynamic_cast<__vpiStringConst*>(ಅಂಶ);
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ vpiDecConst:
	    ಅಳಿಸು dynamic_cast<__vpiDecConst*>(ಅಂಶ);
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ vpi_ದ್ವಿಮಾನ_Const:
	    ಅಳಿಸು dynamic_cast<__vpiBinaryConst*>(ಅಂಶ);
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ vpiRealConst:
	    ಅಳಿಸು dynamic_cast<__vpiRealConst*>(ಅಂಶ);
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ vpiNullConst:
	    ಅಳಿಸು dynamic_cast<__vpiNullConst*>(ಅಂಶ);
	    ನಿಲ್ಲಿಸು;
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    ಪ್ರತಿಜ್ಞಾ(0);
      }
}

ಶೂನ್ಯ parameter_delete(vpiHandle ಅಂಶ)
{
      ಆಯ್ಕೆ(vpi_get(vpiConstType, ಅಂಶ)) {
	  ಸ್ಥಿತಿ vpiStringConst: {
	    ವರ್ಗ __vpiStringParam*rfp = dynamic_cast<__vpiStringParam*>(ಅಂಶ);
	    ಅಳಿಸು rfp;
	    ನಿಲ್ಲಿಸು; }
	  ಸ್ಥಿತಿ vpi_ದ್ವಿಮಾನ_Const: {
	    ರಚನೆ __vpiBinaryParam*rfp = dynamic_cast<__vpiBinaryParam*>(ಅಂಶ);
	    ಅಳಿಸು rfp;
	    ನಿಲ್ಲಿಸು; }
	  ಸ್ಥಿತಿ vpiRealConst: {
	    ರಚನೆ __vpiRealParam*rfp = dynamic_cast<__vpiRealParam*>(ಅಂಶ);
	    ಅಳಿಸು rfp;
	    ನಿಲ್ಲಿಸು; }
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    ಪ್ರತಿಜ್ಞಾ(0);
      }
}
#ಆದರೆ_ಅಂತ್ಯ
