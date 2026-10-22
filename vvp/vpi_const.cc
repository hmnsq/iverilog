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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#ਜੇ_ਅੰਤ
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <climits>
# include  <cassert>
# include  "ivl_alloc.h"

ਵਰਗ __vpiStringConst : ਜਨਤਕ __vpiHandle {
    ਜਨਤਕ:
      ਸਪਸ਼ਟ __vpiStringConst(ਅੱਖਰ*val);
      ~__vpiStringConst() override;
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override;
      ਪੂਰਨ vpi_get(ਪੂਰਨ code) override;
      ਖਾਲੀ vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val) override;

    ਨਿਜੀ:
      ਖਾਲੀ process_string_();
    ਨਿਜੀ:
      ਅੱਖਰ*ਮੁੱਲ_2;
      size_t ਮੁੱਲ_len;
};

ਸਤਰਅੰਦਰ __vpiStringConst::__vpiStringConst(ਅੱਖਰ*v)
: ਮੁੱਲ_2(v)
{
      process_string_();
}

/*
 * Strings are described at the level of the vvp source as a string
 * with literal characters or octal escapes. No other escapes are
 * included, they are processed already by the compiler that generated
 * the vvp source.
 */
ਖਾਲੀ __vpiStringConst::process_string_(ਖਾਲੀ)
{
      ਅੱਖਰ*chr = ਮੁੱਲ_2;
      ਅੱਖਰ*dp = ਮੁੱਲ_2;

      ਜਦੋਂ (*chr) {
	    ਅੱਖਰ ਅਗਲਾ_char = *chr;

	      /* Process octal escapes that I might find. */
	    ਜੇ (*chr == '\\') {
		  ਲਈ (ਪੂਰਨ idx = 1 ;  idx <= 3 ;  idx += 1) {
			ਪ੍ਰਤਿਜ੍ਞਾ(chr[idx] != 0);
			ਪ੍ਰਤਿਜ੍ਞਾ(chr[idx] < '8');
			ਪ੍ਰਤਿਜ੍ਞਾ(chr[idx] >= '0');
			ਅਗਲਾ_char = ਅਗਲਾ_char*8 + chr[idx] - '0';
		  }
		  chr += 3;
	    }
	    *dp++ = ਅਗਲਾ_char;
	    chr += 1;
      }
      *dp = 0;
      ਮੁੱਲ_len = dp - ਮੁੱਲ_2;
}

__vpiStringConst::~__vpiStringConst()
{
      ਮਿਟਾਓ[] ਮੁੱਲ_2;
}

ਪੂਰਨ __vpiStringConst::get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ
{ ਵਾਪਸ vpiConstant; }

ਪੂਰਨ __vpiStringConst::vpi_get(ਪੂਰਨ code)
{
      ਚੋਣ (code) {
          ਹਾਲਤ vpi_ਮਾਪ:
	    ਵਾਪਸ ਮੁੱਲ_len * 8;

          ਹਾਲਤ vpiSigned:
	      ਵਾਪਸ 0;

	  ਹਾਲਤ vpiConstType:
	      ਵਾਪਸ vpiStringConst;

          ਹਾਲਤ vpiAutomatic:
	      ਵਾਪਸ 0;

#ਜੇ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          ਹਾਲਤ _vpiFromThr:
	      ਵਾਪਸ _vpiNoThr;
#ਜੇ_ਅੰਤ

	  ਮੂਲ:
	      fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiStringConst\n", code);
	      ਪ੍ਰਤਿਜ੍ਞਾ(0);
	      ਵਾਪਸ 0;
      }
}

ਖਾਲੀ __vpiStringConst::vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ vp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ uint_ਮੁੱਲ;
      p_vpi_vecval vecp;
      ਬਿਨਾਨਿਸ਼ਾਨੀ size = ਮੁੱਲ_len;
      ਅੱਖਰ*rbuf = 0;
      ਅੱਖਰ*cp;

      ਚੋਣ (vp->format) {
	  ਹਾਲਤ vpiObjTypeVal:
	      /* String parameters by default have vpiStringVal values. */
	    vp->format = vpiStringVal;
	    // fallthrough
	  ਹਾਲਤ vpiStringVal:
	    cp = ਮੁੱਲ_2;
	    rbuf = static_cast<ਅੱਖਰ *>(need_ਨਤੀਜਾ_buf(size + 1, RBUF_VAL));
	    vp->value.str = rbuf;

	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ i = 0; i < size; i++) {
		  // Ignore leading null-bytes and replace other null-bytes with space.
		  // The LRM is not entirely clear on how null bytes should be handled.
		  // This is the implementation chosen for iverilog.
		  ਜੇ (*cp)
			*rbuf++ = *cp;
		  ਨਹੀਂਤਾਂ ਜੇ (rbuf != vp->value.str)
			*rbuf++ = ' ';

		  cp++;
	    }
	    *rbuf = '\0';
	    ਰੋਕੋ;

          ਹਾਲਤ vpiDecStrVal:
	    // Take the (up to 4) characters of the text, convert the characters
	    // to a numerical value, and convert that value to a decimal string.
	    // For example, the string "A" is ASCII 65, so the resulting string
	    // will be "65". and the string "AB" is 65*256 + 66 == 16706, so
	    // the resulting string is "16706". The "size" is the number of
	    // characters of input text to put to work.
	    ਜੇ (size > 4){
		  // We only support standard integers. Ignore other bytes...
		  size = 4;
		  fprintf(stderr, "Warning (vpi_const.cc): %%d on constant strings only looks "
			  "at first 4 bytes!\n");
	    }
	    ਸਥਿਰ ਅਚਲ size_t RBUF_USE_ਮਾਪ = 12;
	    rbuf = static_cast<ਅੱਖਰ *>(need_ਨਤੀਜਾ_buf(RBUF_USE_ਮਾਪ, RBUF_VAL));
	    uint_ਮੁੱਲ = 0;
	    ਲਈ(ਬਿਨਾਨਿਸ਼ਾਨੀ i=0; i<size; i += 1){
		  uint_ਮੁੱਲ <<=8;
		  uint_ਮੁੱਲ += (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(ਮੁੱਲ_2[i]);
	    }
	    snprintf(rbuf, RBUF_USE_ਮਾਪ, "%u", uint_ਮੁੱਲ);
	    vp->value.str = rbuf;
	    ਰੋਕੋ;

          ਹਾਲਤ vpiBinStrVal:
	      rbuf = static_cast<ਅੱਖਰ *>(need_ਨਤੀਜਾ_buf(8 * size + 1, RBUF_VAL));
	      cp = rbuf;
	      ਲਈ(ਬਿਨਾਨਿਸ਼ਾਨੀ i=0; i<size; i += 1){
		  ਲਈ(ਪੂਰਨ ਦੋਆਧਾਰੀ_ਅੰਕ=7; ਦੋਆਧਾਰੀ_ਅੰਕ>=0; ਦੋਆਧਾਰੀ_ਅੰਕ -= 1){
		      *cp++ = "01"[ (ਮੁੱਲ_2[i]>>ਦੋਆਧਾਰੀ_ਅੰਕ)&1 ];
		  }
	      }
	      *cp = 0;
	      vp->value.str = rbuf;
	      ਰੋਕੋ;

          ਹਾਲਤ vpiHexStrVal:
	      rbuf = static_cast<ਅੱਖਰ *>(need_ਨਤੀਜਾ_buf(2 * size + 1, RBUF_VAL));
	      cp = rbuf;
	      ਲਈ(ਬਿਨਾਨਿਸ਼ਾਨੀ i=0; i<size; i += 1){
		  ਲਈ(ਪੂਰਨ nibble=1; nibble>=0; nibble -= 1){
		      *cp++ = "0123456789abcdef"[ (ਮੁੱਲ_2[i]>>(nibble*4))&15 ];
		  }
	      }
	      *cp = 0;
	      vp->value.str = rbuf;
	      ਰੋਕੋ;

          ਹਾਲਤ vpiOctStrVal:
	      fprintf(stderr, "ERROR (vpi_const.cc): %%o display of constant strings not yet implemented\n");
	      ਪ੍ਰਤਿਜ੍ਞਾ(0);
	      ਰੋਕੋ;

          ਹਾਲਤ vpiIntVal:
	      vp->value.integer = 0;
	      ਲਈ(ਬਿਨਾਨਿਸ਼ਾਨੀ i=0; i<size; i += 1){
		  ਲਈ(ਪੂਰਨ ਦੋਆਧਾਰੀ_ਅੰਕ=7; ਦੋਆਧਾਰੀ_ਅੰਕ>=0; ਦੋਆਧਾਰੀ_ਅੰਕ -= 1){
		      vp->value.integer <<= 1;
		      vp->value.integer += (ਮੁੱਲ_2[i]>>ਦੋਆਧਾਰੀ_ਅੰਕ)&1;
		  }
	      }
	      ਰੋਕੋ;

          ਹਾਲਤ vpi_ਸਦਿਸ਼_Val:
              vp->value.vector = static_cast<p_vpi_vecval>
                                 (need_ਨਤੀਜਾ_buf((size+3)/4*
                                                   ਆਕਾਰ(s_vpi_vecval),
                                                  RBUF_VAL));
              uint_ਮੁੱਲ = 0;
              vecp = vp->value.vector;
              vecp->aval = vecp->bval = 0;
	      ਲਈ(ਬਿਨਾਨਿਸ਼ਾਨੀ i=0; i<size; i += 1){
		  vecp->aval |= ਮੁੱਲ_2[i] << uint_ਮੁੱਲ*8;
		  uint_ਮੁੱਲ += 1;
		  ਜੇ (uint_ਮੁੱਲ > 3) {
		      uint_ਮੁੱਲ = 0;
		      vecp += 1;
		      vecp->aval = vecp->bval = 0;
		  }
	      }
	      ਰੋਕੋ;


	  ਮੂਲ:
	    fprintf(stderr, "ERROR (vpi_const.cc): vp->format: %d\n",
	            (ਪੂਰਨ)vp->format);
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);

	    vp->format = vpiSuppressVal;
	    ਰੋਕੋ;
      }
}


ਬਣਤਰ __vpiStringConstTEMP : ਜਨਤਕ __vpiStringConst {
      ਸਪਸ਼ਟ ਸਤਰਅੰਦਰ __vpiStringConstTEMP(ਅੱਖਰ*v) : __vpiStringConst(v) { }
      free_object_fun_t free_object_fun(ਖਾਲੀ) override;
};

ਸਥਿਰ ਪੂਰਨ free_ਆਰਜ਼ੀ_string(vpiHandle obj)
{
      ਬਣਤਰ __vpiStringConstTEMP*rfp = dynamic_cast<__vpiStringConstTEMP*>(obj);
      ਮਿਟਾਓ rfp;
      ਵਾਪਸ 1;
}

__vpiHandle::free_object_fun_t __vpiStringConstTEMP::free_object_fun(ਖਾਲੀ)
{ ਵਾਪਸ &free_ਆਰਜ਼ੀ_string; }

vpiHandle vpip_make_string_const(ਅੱਖਰ*ਪਾਠ, ਤਾਰਕਿਕ persistent_flag)
{
      __vpiStringConst*obj;

      obj = persistent_flag? ਨਵਾਂ __vpiStringConst(ਪਾਠ) : ਨਵਾਂ __vpiStringConstTEMP(ਪਾਠ);

      ਵਾਪਸ obj;
}


ਵਰਗ __vpiStringParam  : ਜਨਤਕ __vpiStringConst {
    ਜਨਤਕ:
      __vpiStringParam(__vpiScope* scope, ਅੱਖਰ*txt, ਅਚਲ ਅੱਖਰ*name,
                       ਤਾਰਕਿਕ local_flag, ਲੰਮਾ file_idx, ਲੰਮਾ lineno);
      ~__vpiStringParam() override;
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override;
      ਪੂਰਨ vpi_get(ਪੂਰਨ code) override;
      ਅੱਖਰ*vpi_get_str(ਪੂਰਨ code) override;
      vpiHandle vpi_handle(ਪੂਰਨ code) override;

    ਨਿਜੀ:
      __vpiScope* scope_;
      ਅਚਲ ਅੱਖਰ*basename_;
      ਤਾਰਕਿਕ       local_flag_;
      ਬਿਨਾਨਿਸ਼ਾਨੀ   file_idx_;
      ਬਿਨਾਨਿਸ਼ਾਨੀ   lineno_;
};

ਸਤਰਅੰਦਰ __vpiStringParam::__vpiStringParam(__vpiScope* scope, ਅੱਖਰ*txt, ਅਚਲ ਅੱਖਰ*nam,
                                          ਤਾਰਕਿਕ local_flag, ਲੰਮਾ file_idx, ਲੰਮਾ lineno)
: __vpiStringConst(txt)
{
      scope_ = scope;
      basename_ = nam;
      local_flag_ = local_flag;
      ਪ੍ਰਤਿਜ੍ਞਾ(file_idx >= 0 && file_idx <= ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ);
      file_idx_ = (ਬਿਨਾਨਿਸ਼ਾਨੀ) file_idx;
      ਪ੍ਰਤਿਜ੍ਞਾ(lineno >= 0 && lineno <= ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ);
      lineno_ = (ਬਿਨਾਨਿਸ਼ਾਨੀ) lineno;
}

__vpiStringParam::~__vpiStringParam()
{
      ਮਿਟਾਓ[]basename_;
}

ਪੂਰਨ __vpiStringParam::get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ
{ ਵਾਪਸ vpiParameter; }

ਪੂਰਨ __vpiStringParam::vpi_get(ਪੂਰਨ code)
{
    ਚੋਣ (code) {
       ਹਾਲਤ vpi_ਸਤਰ_No :
         ਵਾਪਸ lineno_;

       ਹਾਲਤ vpiLocalParam :
         ਵਾਪਸ local_flag_;

       ਮੂਲ :
         ਵਾਪਸ __vpiStringConst::vpi_get(code);
    }
}


ਅੱਖਰ*__vpiStringParam::vpi_get_str(ਪੂਰਨ code)
{
      ਜੇ (code == vpiFile) {
	    ਵਾਪਸ simple_ਸੈੱਟ_rbuf_str(file_ਨਾਂ[file_idx_]);
      }

      ਵਾਪਸ generic_get_str(code, scope_, basename_, ਨਿਰਾ);
}


vpiHandle __vpiStringParam::vpi_handle(ਪੂਰਨ code)
{
      ਚੋਣ (code) {
	  ਹਾਲਤ vpiScope:
	    ਵਾਪਸ scope_;

	  ਹਾਲਤ vpiModule:
	    ਵਾਪਸ vpip_module(scope_);

	  ਮੂਲ:
	    ਵਾਪਸ 0;
      }
}

vpiHandle vpip_make_string_param(ਅਚਲ ਅੱਖਰ*name, ਅੱਖਰ*ਪਾਠ,
                                 ਤਾਰਕਿਕ local_flag, ਲੰਮਾ file_idx, ਲੰਮਾ lineno)
{
      __vpiStringParam*obj = ਨਵਾਂ __vpiStringParam(vpip_peek_current_scope(), ਪਾਠ, name,
                                                  local_flag, file_idx, lineno);

      ਵਾਪਸ obj;
}



ਸਤਰਅੰਦਰ __vpiBinaryConst::__vpiBinaryConst()
: signed_flag(0), sized_flag(0)
{ }

ਪੂਰਨ __vpiBinaryConst::get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ
{ ਵਾਪਸ vpiConstant; }

ਪੂਰਨ __vpiBinaryConst::vpi_get(ਪੂਰਨ code)
{
      ਚੋਣ (code) {
	  ਹਾਲਤ vpiConstType:
	    ਵਾਪਸ vpi_ਦੁਆਧਾਰੀ_Const;

	  ਹਾਲਤ vpi_ਸਤਰ_No:
	    ਵਾਪਸ 0;  // Not implemented for now!

	  ਹਾਲਤ vpiSigned:
	    ਵਾਪਸ signed_flag? 1 : 0;

	  ਹਾਲਤ vpi_ਮਾਪ:
	    ਵਾਪਸ ਬਿੱਟ.size();

          ਹਾਲਤ vpiAutomatic:
	    ਵਾਪਸ 0;

#ਜੇ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          ਹਾਲਤ _vpiFromThr:
	      ਵਾਪਸ _vpiNoThr;
#ਜੇ_ਅੰਤ

	  ਮੂਲ:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiBinaryConst\n", code);
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
	    ਵਾਪਸ 0;
      }
}


ਖਾਲੀ __vpiBinaryConst::vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val)
{
      ਚੋਣ (val->format) {

	  ਹਾਲਤ vpiObjTypeVal:
	  ਹਾਲਤ vpiBinStrVal:
	  ਹਾਲਤ vpiDecStrVal:
	  ਹਾਲਤ vpiOctStrVal:
	  ਹਾਲਤ vpiHexStrVal:
          ਹਾਲਤ vpiScalarVal:
	  ਹਾਲਤ vpiIntVal:
	  ਹਾਲਤ vpi_ਸਦਿਸ਼_Val:
	  ਹਾਲਤ vpiStringVal:
	  ਹਾਲਤ vpiRealVal:
	    vpip_vec4_get_ਮੁੱਲ(ਬਿੱਟ, ਬਿੱਟ.size(), signed_flag, val);
	    ਰੋਕੋ;

	  ਮੂਲ:
	    fprintf(stderr, "vvp error: format %d not supported "
		    "by vpiBinaryConst\n", (ਪੂਰਨ)val->format);
	    val->format = vpiSuppressVal;
	    ਰੋਕੋ;
      }
}

/*
 * Make a VPI constant from a vector string. The string is normally a
 * ASCII string, with each letter a 4-value bit. The first character
 * may be an 's' if the vector is signed.
 */
vpiHandle vpip_make_ਦੁਆਧਾਰੀ_const(ਬਿਨਾਨਿਸ਼ਾਨੀ wid, ਅਚਲ ਅੱਖਰ*ਬਿੱਟ)
{
      ਬਣਤਰ __vpiBinaryConst*obj = ਨਵਾਂ __vpiBinaryConst;

      ਅਚਲ ਅੱਖਰ*bp = ਬਿੱਟ;
      ਜੇ (*bp == 's') {
	    bp += 1;
	    obj->signed_flag = 1;
      }

      obj->ਬਿੱਟ = vector4_from_ਪਾਠ(bp, wid);

      ਵਾਪਸ obj;
}

vvp_vector4_t vector4_from_ਪਾਠ(ਅਚਲ ਅੱਖਰ*ਬਿੱਟ, ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
{
      vvp_vector4_t res (wid);

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t val = BIT4_0;
	    ਚੋਣ (ਬਿੱਟ[wid-idx-1]) {
		ਹਾਲਤ '0':
		  val = BIT4_0;
		  ਰੋਕੋ;
		ਹਾਲਤ '1':
		  val = BIT4_1;
		  ਰੋਕੋ;
		ਹਾਲਤ 'x':
		  val = BIT4_X;
		  ਰੋਕੋ;
		ਹਾਲਤ 'z':
		  val = BIT4_Z;
		  ਰੋਕੋ;
	    }

	    res.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, val);
      }

      ਵਾਪਸ res;
}

ਬਣਤਰ __vpiBinaryParam  : ਜਨਤਕ __vpiBinaryConst {
      __vpiBinaryParam(__vpiScope*scope, ਅਚਲ vvp_vector4_t&b, ਤਾਰਕਿਕ is_signed,
                       ਅਚਲ ਅੱਖਰ*name, ਤਾਰਕਿਕ local_flag, ਲੰਮਾ file_idx, ਲੰਮਾ lineno);
      ~__vpiBinaryParam() override;
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override;
      ਪੂਰਨ vpi_get(ਪੂਰਨ code) override;
      ਅੱਖਰ*vpi_get_str(ਪੂਰਨ code) override;
      vpiHandle vpi_handle(ਪੂਰਨ code) override;

    ਨਿਜੀ:
      __vpiScope*scope_;
      ਅਚਲ ਅੱਖਰ*basename_;
      ਤਾਰਕਿਕ     local_flag_;
      ਬਿਨਾਨਿਸ਼ਾਨੀ file_idx_;
      ਬਿਨਾਨਿਸ਼ਾਨੀ lineno_;
};

ਸਤਰਅੰਦਰ __vpiBinaryParam::__vpiBinaryParam(__vpiScope*scope, ਅਚਲ vvp_vector4_t&b,
                                          ਤਾਰਕਿਕ is_signed, ਅਚਲ ਅੱਖਰ*nam,
                                          ਤਾਰਕਿਕ local_flag, ਲੰਮਾ file_idx, ਲੰਮਾ lineno)
{
      scope_ = scope;
      ਬਿੱਟ = b;
      signed_flag = is_signed ? 1 : 0;
      sized_flag = 0;
      basename_ = nam;
      local_flag_ = local_flag;
      ਪ੍ਰਤਿਜ੍ਞਾ(file_idx >= 0 && file_idx <= ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ);
      file_idx_ = (ਬਿਨਾਨਿਸ਼ਾਨੀ) file_idx;
      ਪ੍ਰਤਿਜ੍ਞਾ(lineno >= 0 && lineno <= ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ);
      lineno_ = (ਬਿਨਾਨਿਸ਼ਾਨੀ) lineno;
}

__vpiBinaryParam::~__vpiBinaryParam()
{
      ਮਿਟਾਓ[]basename_;
}

ਪੂਰਨ __vpiBinaryParam::get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ
{ ਵਾਪਸ vpiParameter; }

ਪੂਰਨ __vpiBinaryParam::vpi_get(ਪੂਰਨ code)
{
    ਚੋਣ (code) {
      ਹਾਲਤ vpi_ਸਤਰ_No :
        ਵਾਪਸ lineno_;

      ਹਾਲਤ vpiLocalParam :
        ਵਾਪਸ local_flag_;

      ਮੂਲ :
        ਵਾਪਸ __vpiBinaryConst::vpi_get(code);
    }
}

ਅੱਖਰ*__vpiBinaryParam::vpi_get_str(ਪੂਰਨ code)
{
      ਜੇ (code == vpiFile)
	    ਵਾਪਸ simple_ਸੈੱਟ_rbuf_str(file_ਨਾਂ[file_idx_]);

      ਵਾਪਸ generic_get_str(code, scope_, basename_, ਨਿਰਾ);
}


vpiHandle __vpiBinaryParam::vpi_handle(ਪੂਰਨ code)
{
      ਚੋਣ (code) {
	  ਹਾਲਤ vpiScope:
	    ਵਾਪਸ scope_;

	  ਹਾਲਤ vpiModule:
	    ਵਾਪਸ vpip_module(scope_);

	  ਮੂਲ:
	    ਵਾਪਸ 0;
      }
}


vpiHandle vpip_make_ਦੁਆਧਾਰੀ_param(ਅਚਲ ਅੱਖਰ*name, ਅਚਲ vvp_vector4_t&ਬਿੱਟ,
				 ਤਾਰਕਿਕ signed_flag, ਤਾਰਕਿਕ local_flag,
				 ਲੰਮਾ file_idx, ਲੰਮਾ lineno)
{
      ਬਣਤਰ __vpiBinaryParam*obj = ਨਵਾਂ __vpiBinaryParam(vpip_peek_current_scope(),
                                                         ਬਿੱਟ, signed_flag, name,
                                                         local_flag, file_idx, lineno);

      ਵਾਪਸ obj;
}



__vpiDecConst::__vpiDecConst(ਪੂਰਨ val)
{
      value = val;
}

__vpiDecConst::__vpiDecConst(ਅਚਲ __vpiDecConst&that)
: __vpiHandle(), value(that.value)
{
}


ਪੂਰਨ __vpiDecConst::get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ
{ ਵਾਪਸ vpiConstant; }

ਪੂਰਨ __vpiDecConst::vpi_get(ਪੂਰਨ code)
{
      ਚੋਣ (code) {
	  ਹਾਲਤ vpiConstType:
	    ਵਾਪਸ vpiDecConst;

	  ਹਾਲਤ vpiSigned:
	    ਵਾਪਸ 1;

	  ਹਾਲਤ vpi_ਮਾਪ:
	    ਵਾਪਸ 32;

          ਹਾਲਤ vpiAutomatic:
	    ਵਾਪਸ 0;

#ਜੇ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          ਹਾਲਤ _vpiFromThr:
	      ਵਾਪਸ _vpiNoThr;
#ਜੇ_ਅੰਤ

	  ਮੂਲ:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiDecConst\n", code);
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
	    ਵਾਪਸ 0;
      }
}


ਖਾਲੀ __vpiDecConst::vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ vp)
{
      ਸਥਿਰ ਅਚਲ size_t RBUF_USE_ਮਾਪ = 64 + 1;
      ਅੱਖਰ*rbuf = static_cast<ਅੱਖਰ *>(need_ਨਤੀਜਾ_buf(RBUF_USE_ਮਾਪ, RBUF_VAL));
      ਅੱਖਰ*cp = rbuf;

      ਚੋਣ (vp->format) {

	  ਹਾਲਤ vpiObjTypeVal:
	  ਹਾਲਤ vpiIntVal: {
		vp->value.integer = value;
		ਰੋਕੋ;
	  }

          ਹਾਲਤ vpiDecStrVal:
	      snprintf(rbuf, RBUF_USE_ਮਾਪ, "%d", value);
	      vp->value.str = rbuf;
	      ਰੋਕੋ;

          ਹਾਲਤ vpiBinStrVal:
	      ਲਈ(ਪੂਰਨ ਦੋਆਧਾਰੀ_ਅੰਕ=31; ਦੋਆਧਾਰੀ_ਅੰਕ>=0;ਦੋਆਧਾਰੀ_ਅੰਕ--){
		  *cp++ = "01"[ (static_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ>(value)>>ਦੋਆਧਾਰੀ_ਅੰਕ)&1 ];
	      }
	      *cp = 0;

	      vp->value.str = rbuf;
	      ਰੋਕੋ;

          ਹਾਲਤ vpiHexStrVal:
	      snprintf(rbuf, RBUF_USE_ਮਾਪ, "%08x", value);
	      vp->value.str = rbuf;
	      ਰੋਕੋ;

          ਹਾਲਤ vpiOctStrVal:
	      snprintf(rbuf, RBUF_USE_ਮਾਪ, "%011x", value);
	      vp->value.str = rbuf;
	      ਰੋਕੋ;

	  ਮੂਲ:
	    fprintf(stderr, "vvp error (vpi_const.cc): format %d not supported "
		    "by vpiDecConst\n", (ਪੂਰਨ)vp->format);
	    vp->format = vpiSuppressVal;
	    ਰੋਕੋ;
      }
}


ਸਤਰਅੰਦਰ __vpiRealConst::__vpiRealConst(ਦੁੱਗਣਾ val)
: value(val)
{ }

ਪੂਰਨ __vpiRealConst::get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ
{ ਵਾਪਸ vpiConstant; }

ਪੂਰਨ __vpiRealConst::vpi_get(ਪੂਰਨ code)
{
      ਚੋਣ (code) {
	  ਹਾਲਤ vpi_ਸਤਰ_No:
	    ਵਾਪਸ 0;  // Not implemented for now!

	  ਹਾਲਤ vpi_ਮਾਪ:
	    ਵਾਪਸ 1;

	  ਹਾਲਤ vpiConstType:
	    ਵਾਪਸ vpiRealConst;

	  ਹਾਲਤ vpiSigned:
	    ਵਾਪਸ 1;

          ਹਾਲਤ vpiAutomatic:
	    ਵਾਪਸ 0;

#ਜੇ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          ਹਾਲਤ _vpiFromThr:
	      ਵਾਪਸ _vpiNoThr;
#ਜੇ_ਅੰਤ

	  ਮੂਲ:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiRealConst\n", code);
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
	    ਵਾਪਸ 0;
      }
}


ਖਾਲੀ __vpiRealConst::vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val)
{
      vpip_real_get_ਮੁੱਲ(value, val);
}


vpiHandle vpip_make_real_const(ਦੁੱਗਣਾ value)
{
      __vpiRealConst*obj = ਨਵਾਂ __vpiRealConst(value);
      ਵਾਪਸ obj;
}

ਬਣਤਰ __vpiRealParam  : ਜਨਤਕ __vpiRealConst {
      __vpiRealParam(__vpiScope* scope, ਦੁੱਗਣਾ val, ਅਚਲ ਅੱਖਰ*name,
                     ਤਾਰਕਿਕ local_flag, ਲੰਮਾ file_idx, ਲੰਮਾ lineno);
      ~__vpiRealParam() override;
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override;
      ਪੂਰਨ vpi_get(ਪੂਰਨ code) override;
      ਅੱਖਰ*vpi_get_str(ਪੂਰਨ code) override;
      vpiHandle vpi_handle(ਪੂਰਨ code) override;

    ਨਿਜੀ:
      __vpiScope* scope_;
      ਅਚਲ ਅੱਖਰ*basename_;
      ਤਾਰਕਿਕ local_flag_;
      ਬਿਨਾਨਿਸ਼ਾਨੀ file_idx_;
      ਬਿਨਾਨਿਸ਼ਾਨੀ lineno_;
};


ਸਤਰਅੰਦਰ __vpiRealParam::__vpiRealParam(__vpiScope* scope, ਦੁੱਗਣਾ val, ਅਚਲ ਅੱਖਰ*name,
                                      ਤਾਰਕਿਕ local_flag, ਲੰਮਾ file_idx, ਲੰਮਾ lineno)
: __vpiRealConst(val)
{
      scope_ = scope;
      basename_ = name;
      local_flag_ = local_flag;
      ਪ੍ਰਤਿਜ੍ਞਾ(file_idx >= 0 && file_idx <= ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ);
      file_idx_ = (ਬਿਨਾਨਿਸ਼ਾਨੀ) file_idx;
      ਪ੍ਰਤਿਜ੍ਞਾ(lineno >= 0 && lineno <= ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ);
      lineno_ = (ਬਿਨਾਨਿਸ਼ਾਨੀ) lineno;
}

__vpiRealParam::~__vpiRealParam()
{
      ਮਿਟਾਓ[]basename_;
}


ਪੂਰਨ __vpiRealParam::get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ
{ ਵਾਪਸ vpiParameter; }

ਪੂਰਨ __vpiRealParam::vpi_get(ਪੂਰਨ code)
{
    ਚੋਣ (code) {
      ਹਾਲਤ vpi_ਸਤਰ_No :
        ਵਾਪਸ lineno_;

      ਹਾਲਤ vpiLocalParam :
        ਵਾਪਸ local_flag_;

      ਮੂਲ :
           ਵਾਪਸ __vpiRealConst::vpi_get(code);
    }
}

ਅੱਖਰ* __vpiRealParam::vpi_get_str(ਪੂਰਨ code)
{
      ਜੇ (code == vpiFile)
            ਵਾਪਸ simple_ਸੈੱਟ_rbuf_str(file_ਨਾਂ[file_idx_]);

      ਵਾਪਸ generic_get_str(code, scope_, basename_, ਨਿਰਾ);
}

vpiHandle __vpiRealParam::vpi_handle(ਪੂਰਨ code)
{
      ਚੋਣ (code) {
          ਹਾਲਤ vpiScope:
            ਵਾਪਸ scope_;

	  ਹਾਲਤ vpiModule:
	    ਵਾਪਸ vpip_module(scope_);

          ਮੂਲ:
            ਵਾਪਸ 0;
      }
}


vpiHandle vpip_make_real_param(ਅਚਲ ਅੱਖਰ*name, ਦੁੱਗਣਾ value,
                               ਤਾਰਕਿਕ local_flag, ਲੰਮਾ file_idx, ਲੰਮਾ lineno)
{
      ਬਣਤਰ __vpiRealParam*obj = ਨਵਾਂ __vpiRealParam(vpip_peek_current_scope(),
                                                     value, name, local_flag,
                                                     file_idx, lineno);
      ਵਾਪਸ obj;
}

/*
 * Make a VPI null constant
 */
ਸਤਰਅੰਦਰ __vpiNullConst::__vpiNullConst()
{ }

ਪੂਰਨ __vpiNullConst::get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ
{ ਵਾਪਸ vpiConstant; }

ਪੂਰਨ __vpiNullConst::vpi_get(ਪੂਰਨ code)
{
      ਚੋਣ (code) {
	  ਹਾਲਤ vpi_ਸਤਰ_No:
	    ਵਾਪਸ 0;  // Not implemented for now!

	  ਹਾਲਤ vpi_ਮਾਪ:
	    ਵਾਪਸ 32;

	  ਹਾਲਤ vpiConstType:
	    ਵਾਪਸ vpiNullConst;

	  ਹਾਲਤ vpiSigned:
	    ਵਾਪਸ 0;

          ਹਾਲਤ vpiAutomatic:
	    ਵਾਪਸ 0;

#ਜੇ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          ਹਾਲਤ _vpiFromThr:
	      ਵਾਪਸ _vpiNoThr;
#ਜੇ_ਅੰਤ

	  ਮੂਲ:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiNullConst\n", code);
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
	    ਵਾਪਸ 0;
      }
}

ਖਾਲੀ __vpiNullConst::vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val)
{
      ਸਥਿਰ ਅਚਲ size_t RBUF_USE_ਮਾਪ = 64 + 1;
      ਅੱਖਰ*rbuf = static_cast<ਅੱਖਰ *>(need_ਨਤੀਜਾ_buf(RBUF_USE_ਮਾਪ, RBUF_VAL));

      ਚੋਣ (val->format) {

	  ਹਾਲਤ vpiObjTypeVal:
	    val->format = vpiStringVal;
	    // fall through
	  ਹਾਲਤ vpiBinStrVal:
	  ਹਾਲਤ vpiDecStrVal:
	  ਹਾਲਤ vpiOctStrVal:
	  ਹਾਲਤ vpiHexStrVal:
	  ਹਾਲਤ vpiStringVal:
	    snprintf(rbuf, RBUF_USE_ਮਾਪ, "null");
	    val->value.str = rbuf;
	    ਰੋਕੋ;

          ਹਾਲਤ vpiScalarVal:
	    val->value.scalar = vpi0;
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiIntVal:
	    val->value.integer = 0;
	    ਰੋਕੋ;

	  ਹਾਲਤ vpi_ਸਦਿਸ਼_Val:
	    val->value.vector = static_cast<p_vpi_vecval>
	                        (need_ਨਤੀਜਾ_buf(2*ਆਕਾਰ(s_vpi_vecval),
	                                         RBUF_VAL));
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0; idx < 2; idx += 1) {
		  val->value.vector[idx].aval = 0;
		  val->value.vector[idx].bval = 0;
	    }
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiRealVal:
	    val->value.real = 0.0;
	    ਰੋਕੋ;

	  ਮੂਲ:
	    fprintf(stderr, "vvp error: format %d not supported "
		    "by vpiNullConst\n", (ਪੂਰਨ)val->format);
	    val->format = vpiSuppressVal;
	    ਰੋਕੋ;
      }
}

vpiHandle vpip_make_null_const()
{
      __vpiNullConst*obj = ਨਵਾਂ __vpiNullConst;
      ਵਾਪਸ obj;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਖਾਲੀ constant_delete(vpiHandle ਵਸਤੂ)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ਵਸਤੂ->get_type_ਕੋਡ() == vpiConstant);
      ਚੋਣ(vpi_get(vpiConstType, ਵਸਤੂ)) {
	  ਹਾਲਤ vpiStringConst:
	    ਮਿਟਾਓ dynamic_cast<__vpiStringConst*>(ਵਸਤੂ);
	    ਰੋਕੋ;
	  ਹਾਲਤ vpiDecConst:
	    ਮਿਟਾਓ dynamic_cast<__vpiDecConst*>(ਵਸਤੂ);
	    ਰੋਕੋ;
	  ਹਾਲਤ vpi_ਦੁਆਧਾਰੀ_Const:
	    ਮਿਟਾਓ dynamic_cast<__vpiBinaryConst*>(ਵਸਤੂ);
	    ਰੋਕੋ;
	  ਹਾਲਤ vpiRealConst:
	    ਮਿਟਾਓ dynamic_cast<__vpiRealConst*>(ਵਸਤੂ);
	    ਰੋਕੋ;
	  ਹਾਲਤ vpiNullConst:
	    ਮਿਟਾਓ dynamic_cast<__vpiNullConst*>(ਵਸਤੂ);
	    ਰੋਕੋ;
	  ਮੂਲ:
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
      }
}

ਖਾਲੀ parameter_delete(vpiHandle ਵਸਤੂ)
{
      ਚੋਣ(vpi_get(vpiConstType, ਵਸਤੂ)) {
	  ਹਾਲਤ vpiStringConst: {
	    ਵਰਗ __vpiStringParam*rfp = dynamic_cast<__vpiStringParam*>(ਵਸਤੂ);
	    ਮਿਟਾਓ rfp;
	    ਰੋਕੋ; }
	  ਹਾਲਤ vpi_ਦੁਆਧਾਰੀ_Const: {
	    ਬਣਤਰ __vpiBinaryParam*rfp = dynamic_cast<__vpiBinaryParam*>(ਵਸਤੂ);
	    ਮਿਟਾਓ rfp;
	    ਰੋਕੋ; }
	  ਹਾਲਤ vpiRealConst: {
	    ਬਣਤਰ __vpiRealParam*rfp = dynamic_cast<__vpiRealParam*>(ਵਸਤੂ);
	    ਮਿਟਾਓ rfp;
	    ਰੋਕੋ; }
	  ਮੂਲ:
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
      }
}
#ਜੇ_ਅੰਤ
