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

വർഗ്ഗം __vpiStringConst : പൊതു __vpiHandle {
    പൊതു:
      explicit __vpiStringConst(അക്ഷരം*val);
      ~__vpiStringConst() override;
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override;
      പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code) override;
      ശൂന്യം vpi_get_മൂല്യം(p_vpi_മൂല്യം val) override;

    സ്വകാര്യ:
      ശൂന്യം process_string_();
    സ്വകാര്യ:
      അക്ഷരം*മൂല്യം_2;
      size_t മൂല്യം_len;
};

വരിയിൽ __vpiStringConst::__vpiStringConst(അക്ഷരം*v)
: മൂല്യം_2(v)
{
      process_string_();
}

/*
 * Strings are described at the level of the vvp source as a string
 * with literal characters or octal escapes. No other escapes are
 * included, they are processed already by the compiler that generated
 * the vvp source.
 */
ശൂന്യം __vpiStringConst::process_string_(ശൂന്യം)
{
      അക്ഷരം*chr = മൂല്യം_2;
      അക്ഷരം*dp = മൂല്യം_2;

      ആകുമ്പോൾ (*chr) {
	    അക്ഷരം next_char = *chr;

	      /* Process octal escapes that I might find. */
	    എങ്കിൽ (*chr == '\\') {
		  വേണ്ടി (പൂർണ്ണസംഖ്യ idx = 1 ;  idx <= 3 ;  idx += 1) {
			പ്രതിജ്ഞാ(chr[idx] != 0);
			പ്രതിജ്ഞാ(chr[idx] < '8');
			പ്രതിജ്ഞാ(chr[idx] >= '0');
			next_char = next_char*8 + chr[idx] - '0';
		  }
		  chr += 3;
	    }
	    *dp++ = next_char;
	    chr += 1;
      }
      *dp = 0;
      മൂല്യം_len = dp - മൂല്യം_2;
}

__vpiStringConst::~__vpiStringConst()
{
      delete[] മൂല്യം_2;
}

പൂർണ്ണസംഖ്യ __vpiStringConst::get_type_code(ശൂന്യം) സ്ഥിരം
{ തിരികെ vpiConstant; }

പൂർണ്ണസംഖ്യ __vpiStringConst::vpi_get(പൂർണ്ണസംഖ്യ code)
{
      തിരഞ്ഞെടുപ്പ് (code) {
          അവസ്ഥ vpiSize:
	    തിരികെ മൂല്യം_len * 8;

          അവസ്ഥ vpiSigned:
	      തിരികെ 0;

	  അവസ്ഥ vpiConstType:
	      തിരികെ vpiStringConst;

          അവസ്ഥ vpiAutomatic:
	      തിരികെ 0;

#എങ്കിൽ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          അവസ്ഥ _vpiFromThr:
	      തിരികെ _vpiNoThr;
#endif

	  സ്ഥിരസ്ഥിതി:
	      fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiStringConst\n", code);
	      പ്രതിജ്ഞാ(0);
	      തിരികെ 0;
      }
}

ശൂന്യം __vpiStringConst::vpi_get_മൂല്യം(p_vpi_മൂല്യം vp)
{
      unsigned uint_മൂല്യം;
      p_vpi_vecval vecp;
      unsigned size = മൂല്യം_len;
      അക്ഷരം*rbuf = 0;
      അക്ഷരം*cp;

      തിരഞ്ഞെടുപ്പ് (vp->format) {
	  അവസ്ഥ vpiObjTypeVal:
	      /* String parameters by default have vpiStringVal values. */
	    vp->format = vpiStringVal;
	    // fallthrough
	  അവസ്ഥ vpiStringVal:
	    cp = മൂല്യം_2;
	    rbuf = static_cast<അക്ഷരം *>(need_result_buf(size + 1, RBUF_VAL));
	    vp->value.str = rbuf;

	    വേണ്ടി (unsigned പൂർണ്ണസംഖ്യ i = 0; i < size; i++) {
		  // Ignore leading null-bytes and replace other null-bytes with space.
		  // The LRM is not entirely clear on how null bytes should be handled.
		  // This is the implementation chosen for iverilog.
		  എങ്കിൽ (*cp)
			*rbuf++ = *cp;
		  അല്ലെങ്കിൽ എങ്കിൽ (rbuf != vp->value.str)
			*rbuf++ = ' ';

		  cp++;
	    }
	    *rbuf = '\0';
	    നിർത്തുക;

          അവസ്ഥ vpiDecStrVal:
	    // Take the (up to 4) characters of the text, convert the characters
	    // to a numerical value, and convert that value to a decimal string.
	    // For example, the string "A" is ASCII 65, so the resulting string
	    // will be "65". and the string "AB" is 65*256 + 66 == 16706, so
	    // the resulting string is "16706". The "size" is the number of
	    // characters of input text to put to work.
	    എങ്കിൽ (size > 4){
		  // We only support standard integers. Ignore other bytes...
		  size = 4;
		  fprintf(stderr, "Warning (vpi_const.cc): %%d on constant strings only looks "
			  "at first 4 bytes!\n");
	    }
	    സ്ഥിതം സ്ഥിരം size_t RBUF_USE_SIZE = 12;
	    rbuf = static_cast<അക്ഷരം *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));
	    uint_മൂല്യം = 0;
	    വേണ്ടി(unsigned i=0; i<size; i += 1){
		  uint_മൂല്യം <<=8;
		  uint_മൂല്യം += (unsigned അക്ഷരം)(മൂല്യം_2[i]);
	    }
	    snprintf(rbuf, RBUF_USE_SIZE, "%u", uint_മൂല്യം);
	    vp->value.str = rbuf;
	    നിർത്തുക;

          അവസ്ഥ vpiBinStrVal:
	      rbuf = static_cast<അക്ഷരം *>(need_result_buf(8 * size + 1, RBUF_VAL));
	      cp = rbuf;
	      വേണ്ടി(unsigned i=0; i<size; i += 1){
		  വേണ്ടി(പൂർണ്ണസംഖ്യ bit=7; bit>=0; bit -= 1){
		      *cp++ = "01"[ (മൂല്യം_2[i]>>bit)&1 ];
		  }
	      }
	      *cp = 0;
	      vp->value.str = rbuf;
	      നിർത്തുക;

          അവസ്ഥ vpiHexStrVal:
	      rbuf = static_cast<അക്ഷരം *>(need_result_buf(2 * size + 1, RBUF_VAL));
	      cp = rbuf;
	      വേണ്ടി(unsigned i=0; i<size; i += 1){
		  വേണ്ടി(പൂർണ്ണസംഖ്യ nibble=1; nibble>=0; nibble -= 1){
		      *cp++ = "0123456789abcdef"[ (മൂല്യം_2[i]>>(nibble*4))&15 ];
		  }
	      }
	      *cp = 0;
	      vp->value.str = rbuf;
	      നിർത്തുക;

          അവസ്ഥ vpiOctStrVal:
	      fprintf(stderr, "ERROR (vpi_const.cc): %%o display of constant strings not yet implemented\n");
	      പ്രതിജ്ഞാ(0);
	      നിർത്തുക;

          അവസ്ഥ vpiIntVal:
	      vp->value.integer = 0;
	      വേണ്ടി(unsigned i=0; i<size; i += 1){
		  വേണ്ടി(പൂർണ്ണസംഖ്യ bit=7; bit>=0; bit -= 1){
		      vp->value.integer <<= 1;
		      vp->value.integer += (മൂല്യം_2[i]>>bit)&1;
		  }
	      }
	      നിർത്തുക;

          അവസ്ഥ vpiVectorVal:
              vp->value.vector = static_cast<p_vpi_vecval>
                                 (need_result_buf((size+3)/4*
                                                   വലുപ്പം(s_vpi_vecval),
                                                  RBUF_VAL));
              uint_മൂല്യം = 0;
              vecp = vp->value.vector;
              vecp->aval = vecp->bval = 0;
	      വേണ്ടി(unsigned i=0; i<size; i += 1){
		  vecp->aval |= മൂല്യം_2[i] << uint_മൂല്യം*8;
		  uint_മൂല്യം += 1;
		  എങ്കിൽ (uint_മൂല്യം > 3) {
		      uint_മൂല്യം = 0;
		      vecp += 1;
		      vecp->aval = vecp->bval = 0;
		  }
	      }
	      നിർത്തുക;


	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "ERROR (vpi_const.cc): vp->format: %d\n",
	            (പൂർണ്ണസംഖ്യ)vp->format);
	    പ്രതിജ്ഞാ(0);

	    vp->format = vpiSuppressVal;
	    നിർത്തുക;
      }
}


ഘടന __vpiStringConstTEMP : പൊതു __vpiStringConst {
      explicit വരിയിൽ __vpiStringConstTEMP(അക്ഷരം*v) : __vpiStringConst(v) { }
      free_object_fun_t free_object_fun(ശൂന്യം) override;
};

സ്ഥിതം പൂർണ്ണസംഖ്യ free_temp_string(vpiHandle obj)
{
      ഘടന __vpiStringConstTEMP*rfp = dynamic_cast<__vpiStringConstTEMP*>(obj);
      delete rfp;
      തിരികെ 1;
}

__vpiHandle::free_object_fun_t __vpiStringConstTEMP::free_object_fun(ശൂന്യം)
{ തിരികെ &free_temp_string; }

vpiHandle vpip_make_string_const(അക്ഷരം*text, തർക്കം persistent_flag)
{
      __vpiStringConst*obj;

      obj = persistent_flag? പുതിയ __vpiStringConst(text) : പുതിയ __vpiStringConstTEMP(text);

      തിരികെ obj;
}


വർഗ്ഗം __vpiStringParam  : പൊതു __vpiStringConst {
    പൊതു:
      __vpiStringParam(__vpiScope* scope, അക്ഷരം*txt, സ്ഥിരം അക്ഷരം*name,
                       തർക്കം local_flag, long file_idx, long lineno);
      ~__vpiStringParam() override;
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override;
      പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code) override;
      അക്ഷരം*vpi_get_str(പൂർണ്ണസംഖ്യ code) override;
      vpiHandle vpi_handle(പൂർണ്ണസംഖ്യ code) override;

    സ്വകാര്യ:
      __vpiScope* scope_;
      സ്ഥിരം അക്ഷരം*basename_;
      തർക്കം       local_flag_;
      unsigned   file_idx_;
      unsigned   lineno_;
};

വരിയിൽ __vpiStringParam::__vpiStringParam(__vpiScope* scope, അക്ഷരം*txt, സ്ഥിരം അക്ഷരം*nam,
                                          തർക്കം local_flag, long file_idx, long lineno)
: __vpiStringConst(txt)
{
      scope_ = scope;
      basename_ = nam;
      local_flag_ = local_flag;
      പ്രതിജ്ഞാ(file_idx >= 0 && file_idx <= INT_MAX);
      file_idx_ = (unsigned) file_idx;
      പ്രതിജ്ഞാ(lineno >= 0 && lineno <= INT_MAX);
      lineno_ = (unsigned) lineno;
}

__vpiStringParam::~__vpiStringParam()
{
      delete[]basename_;
}

പൂർണ്ണസംഖ്യ __vpiStringParam::get_type_code(ശൂന്യം) സ്ഥിരം
{ തിരികെ vpiParameter; }

പൂർണ്ണസംഖ്യ __vpiStringParam::vpi_get(പൂർണ്ണസംഖ്യ code)
{
    തിരഞ്ഞെടുപ്പ് (code) {
       അവസ്ഥ vpi_വരി_No :
         തിരികെ lineno_;

       അവസ്ഥ vpiLocalParam :
         തിരികെ local_flag_;

       സ്ഥിരസ്ഥിതി :
         തിരികെ __vpiStringConst::vpi_get(code);
    }
}


അക്ഷരം*__vpiStringParam::vpi_get_str(പൂർണ്ണസംഖ്യ code)
{
      എങ്കിൽ (code == vpiFile) {
	    തിരികെ simple_set_rbuf_str(file_names[file_idx_]);
      }

      തിരികെ generic_get_str(code, scope_, basename_, ശൂന്യ);
}


vpiHandle __vpiStringParam::vpi_handle(പൂർണ്ണസംഖ്യ code)
{
      തിരഞ്ഞെടുപ്പ് (code) {
	  അവസ്ഥ vpiScope:
	    തിരികെ scope_;

	  അവസ്ഥ vpiModule:
	    തിരികെ vpip_module(scope_);

	  സ്ഥിരസ്ഥിതി:
	    തിരികെ 0;
      }
}

vpiHandle vpip_make_string_param(സ്ഥിരം അക്ഷരം*name, അക്ഷരം*text,
                                 തർക്കം local_flag, long file_idx, long lineno)
{
      __vpiStringParam*obj = പുതിയ __vpiStringParam(vpip_peek_current_scope(), text, name,
                                                  local_flag, file_idx, lineno);

      തിരികെ obj;
}



വരിയിൽ __vpiBinaryConst::__vpiBinaryConst()
: signed_flag(0), sized_flag(0)
{ }

പൂർണ്ണസംഖ്യ __vpiBinaryConst::get_type_code(ശൂന്യം) സ്ഥിരം
{ തിരികെ vpiConstant; }

പൂർണ്ണസംഖ്യ __vpiBinaryConst::vpi_get(പൂർണ്ണസംഖ്യ code)
{
      തിരഞ്ഞെടുപ്പ് (code) {
	  അവസ്ഥ vpiConstType:
	    തിരികെ vpi_ദ്വിമാന_Const;

	  അവസ്ഥ vpi_വരി_No:
	    തിരികെ 0;  // Not implemented for now!

	  അവസ്ഥ vpiSigned:
	    തിരികെ signed_flag? 1 : 0;

	  അവസ്ഥ vpiSize:
	    തിരികെ bits.size();

          അവസ്ഥ vpiAutomatic:
	    തിരികെ 0;

#എങ്കിൽ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          അവസ്ഥ _vpiFromThr:
	      തിരികെ _vpiNoThr;
#endif

	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiBinaryConst\n", code);
	    പ്രതിജ്ഞാ(0);
	    തിരികെ 0;
      }
}


ശൂന്യം __vpiBinaryConst::vpi_get_മൂല്യം(p_vpi_മൂല്യം val)
{
      തിരഞ്ഞെടുപ്പ് (val->format) {

	  അവസ്ഥ vpiObjTypeVal:
	  അവസ്ഥ vpiBinStrVal:
	  അവസ്ഥ vpiDecStrVal:
	  അവസ്ഥ vpiOctStrVal:
	  അവസ്ഥ vpiHexStrVal:
          അവസ്ഥ vpiScalarVal:
	  അവസ്ഥ vpiIntVal:
	  അവസ്ഥ vpiVectorVal:
	  അവസ്ഥ vpiStringVal:
	  അവസ്ഥ vpiRealVal:
	    vpip_vec4_get_മൂല്യം(bits, bits.size(), signed_flag, val);
	    നിർത്തുക;

	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "vvp error: format %d not supported "
		    "by vpiBinaryConst\n", (പൂർണ്ണസംഖ്യ)val->format);
	    val->format = vpiSuppressVal;
	    നിർത്തുക;
      }
}

/*
 * Make a VPI constant from a vector string. The string is normally a
 * ASCII string, with each letter a 4-value bit. The first character
 * may be an 's' if the vector is signed.
 */
vpiHandle vpip_make_ദ്വിമാന_const(unsigned wid, സ്ഥിരം അക്ഷരം*bits)
{
      ഘടന __vpiBinaryConst*obj = പുതിയ __vpiBinaryConst;

      സ്ഥിരം അക്ഷരം*bp = bits;
      എങ്കിൽ (*bp == 's') {
	    bp += 1;
	    obj->signed_flag = 1;
      }

      obj->bits = vector4_from_text(bp, wid);

      തിരികെ obj;
}

vvp_vector4_t vector4_from_text(സ്ഥിരം അക്ഷരം*bits, unsigned wid)
{
      vvp_vector4_t res (wid);

      വേണ്ടി (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t val = BIT4_0;
	    തിരഞ്ഞെടുപ്പ് (bits[wid-idx-1]) {
		അവസ്ഥ '0':
		  val = BIT4_0;
		  നിർത്തുക;
		അവസ്ഥ '1':
		  val = BIT4_1;
		  നിർത്തുക;
		അവസ്ഥ 'x':
		  val = BIT4_X;
		  നിർത്തുക;
		അവസ്ഥ 'z':
		  val = BIT4_Z;
		  നിർത്തുക;
	    }

	    res.set_bit(idx, val);
      }

      തിരികെ res;
}

ഘടന __vpiBinaryParam  : പൊതു __vpiBinaryConst {
      __vpiBinaryParam(__vpiScope*scope, സ്ഥിരം vvp_vector4_t&b, തർക്കം is_signed,
                       സ്ഥിരം അക്ഷരം*name, തർക്കം local_flag, long file_idx, long lineno);
      ~__vpiBinaryParam() override;
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override;
      പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code) override;
      അക്ഷരം*vpi_get_str(പൂർണ്ണസംഖ്യ code) override;
      vpiHandle vpi_handle(പൂർണ്ണസംഖ്യ code) override;

    സ്വകാര്യ:
      __vpiScope*scope_;
      സ്ഥിരം അക്ഷരം*basename_;
      തർക്കം     local_flag_;
      unsigned file_idx_;
      unsigned lineno_;
};

വരിയിൽ __vpiBinaryParam::__vpiBinaryParam(__vpiScope*scope, സ്ഥിരം vvp_vector4_t&b,
                                          തർക്കം is_signed, സ്ഥിരം അക്ഷരം*nam,
                                          തർക്കം local_flag, long file_idx, long lineno)
{
      scope_ = scope;
      bits = b;
      signed_flag = is_signed ? 1 : 0;
      sized_flag = 0;
      basename_ = nam;
      local_flag_ = local_flag;
      പ്രതിജ്ഞാ(file_idx >= 0 && file_idx <= INT_MAX);
      file_idx_ = (unsigned) file_idx;
      പ്രതിജ്ഞാ(lineno >= 0 && lineno <= INT_MAX);
      lineno_ = (unsigned) lineno;
}

__vpiBinaryParam::~__vpiBinaryParam()
{
      delete[]basename_;
}

പൂർണ്ണസംഖ്യ __vpiBinaryParam::get_type_code(ശൂന്യം) സ്ഥിരം
{ തിരികെ vpiParameter; }

പൂർണ്ണസംഖ്യ __vpiBinaryParam::vpi_get(പൂർണ്ണസംഖ്യ code)
{
    തിരഞ്ഞെടുപ്പ് (code) {
      അവസ്ഥ vpi_വരി_No :
        തിരികെ lineno_;

      അവസ്ഥ vpiLocalParam :
        തിരികെ local_flag_;

      സ്ഥിരസ്ഥിതി :
        തിരികെ __vpiBinaryConst::vpi_get(code);
    }
}

അക്ഷരം*__vpiBinaryParam::vpi_get_str(പൂർണ്ണസംഖ്യ code)
{
      എങ്കിൽ (code == vpiFile)
	    തിരികെ simple_set_rbuf_str(file_names[file_idx_]);

      തിരികെ generic_get_str(code, scope_, basename_, ശൂന്യ);
}


vpiHandle __vpiBinaryParam::vpi_handle(പൂർണ്ണസംഖ്യ code)
{
      തിരഞ്ഞെടുപ്പ് (code) {
	  അവസ്ഥ vpiScope:
	    തിരികെ scope_;

	  അവസ്ഥ vpiModule:
	    തിരികെ vpip_module(scope_);

	  സ്ഥിരസ്ഥിതി:
	    തിരികെ 0;
      }
}


vpiHandle vpip_make_ദ്വിമാന_param(സ്ഥിരം അക്ഷരം*name, സ്ഥിരം vvp_vector4_t&bits,
				 തർക്കം signed_flag, തർക്കം local_flag,
				 long file_idx, long lineno)
{
      ഘടന __vpiBinaryParam*obj = പുതിയ __vpiBinaryParam(vpip_peek_current_scope(),
                                                         bits, signed_flag, name,
                                                         local_flag, file_idx, lineno);

      തിരികെ obj;
}



__vpiDecConst::__vpiDecConst(പൂർണ്ണസംഖ്യ val)
{
      value = val;
}

__vpiDecConst::__vpiDecConst(സ്ഥിരം __vpiDecConst&that)
: __vpiHandle(), value(that.value)
{
}


പൂർണ്ണസംഖ്യ __vpiDecConst::get_type_code(ശൂന്യം) സ്ഥിരം
{ തിരികെ vpiConstant; }

പൂർണ്ണസംഖ്യ __vpiDecConst::vpi_get(പൂർണ്ണസംഖ്യ code)
{
      തിരഞ്ഞെടുപ്പ് (code) {
	  അവസ്ഥ vpiConstType:
	    തിരികെ vpiDecConst;

	  അവസ്ഥ vpiSigned:
	    തിരികെ 1;

	  അവസ്ഥ vpiSize:
	    തിരികെ 32;

          അവസ്ഥ vpiAutomatic:
	    തിരികെ 0;

#എങ്കിൽ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          അവസ്ഥ _vpiFromThr:
	      തിരികെ _vpiNoThr;
#endif

	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiDecConst\n", code);
	    പ്രതിജ്ഞാ(0);
	    തിരികെ 0;
      }
}


ശൂന്യം __vpiDecConst::vpi_get_മൂല്യം(p_vpi_മൂല്യം vp)
{
      സ്ഥിതം സ്ഥിരം size_t RBUF_USE_SIZE = 64 + 1;
      അക്ഷരം*rbuf = static_cast<അക്ഷരം *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));
      അക്ഷരം*cp = rbuf;

      തിരഞ്ഞെടുപ്പ് (vp->format) {

	  അവസ്ഥ vpiObjTypeVal:
	  അവസ്ഥ vpiIntVal: {
		vp->value.integer = value;
		നിർത്തുക;
	  }

          അവസ്ഥ vpiDecStrVal:
	      snprintf(rbuf, RBUF_USE_SIZE, "%d", value);
	      vp->value.str = rbuf;
	      നിർത്തുക;

          അവസ്ഥ vpiBinStrVal:
	      വേണ്ടി(പൂർണ്ണസംഖ്യ bit=31; bit>=0;bit--){
		  *cp++ = "01"[ (static_cast<unsigned>(value)>>bit)&1 ];
	      }
	      *cp = 0;

	      vp->value.str = rbuf;
	      നിർത്തുക;

          അവസ്ഥ vpiHexStrVal:
	      snprintf(rbuf, RBUF_USE_SIZE, "%08x", value);
	      vp->value.str = rbuf;
	      നിർത്തുക;

          അവസ്ഥ vpiOctStrVal:
	      snprintf(rbuf, RBUF_USE_SIZE, "%011x", value);
	      vp->value.str = rbuf;
	      നിർത്തുക;

	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "vvp error (vpi_const.cc): format %d not supported "
		    "by vpiDecConst\n", (പൂർണ്ണസംഖ്യ)vp->format);
	    vp->format = vpiSuppressVal;
	    നിർത്തുക;
      }
}


വരിയിൽ __vpiRealConst::__vpiRealConst(ഇരട്ട val)
: value(val)
{ }

പൂർണ്ണസംഖ്യ __vpiRealConst::get_type_code(ശൂന്യം) സ്ഥിരം
{ തിരികെ vpiConstant; }

പൂർണ്ണസംഖ്യ __vpiRealConst::vpi_get(പൂർണ്ണസംഖ്യ code)
{
      തിരഞ്ഞെടുപ്പ് (code) {
	  അവസ്ഥ vpi_വരി_No:
	    തിരികെ 0;  // Not implemented for now!

	  അവസ്ഥ vpiSize:
	    തിരികെ 1;

	  അവസ്ഥ vpiConstType:
	    തിരികെ vpiRealConst;

	  അവസ്ഥ vpiSigned:
	    തിരികെ 1;

          അവസ്ഥ vpiAutomatic:
	    തിരികെ 0;

#എങ്കിൽ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          അവസ്ഥ _vpiFromThr:
	      തിരികെ _vpiNoThr;
#endif

	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiRealConst\n", code);
	    പ്രതിജ്ഞാ(0);
	    തിരികെ 0;
      }
}


ശൂന്യം __vpiRealConst::vpi_get_മൂല്യം(p_vpi_മൂല്യം val)
{
      vpip_real_get_മൂല്യം(value, val);
}


vpiHandle vpip_make_real_const(ഇരട്ട value)
{
      __vpiRealConst*obj = പുതിയ __vpiRealConst(value);
      തിരികെ obj;
}

ഘടന __vpiRealParam  : പൊതു __vpiRealConst {
      __vpiRealParam(__vpiScope* scope, ഇരട്ട val, സ്ഥിരം അക്ഷരം*name,
                     തർക്കം local_flag, long file_idx, long lineno);
      ~__vpiRealParam() override;
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override;
      പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code) override;
      അക്ഷരം*vpi_get_str(പൂർണ്ണസംഖ്യ code) override;
      vpiHandle vpi_handle(പൂർണ്ണസംഖ്യ code) override;

    സ്വകാര്യ:
      __vpiScope* scope_;
      സ്ഥിരം അക്ഷരം*basename_;
      തർക്കം local_flag_;
      unsigned file_idx_;
      unsigned lineno_;
};


വരിയിൽ __vpiRealParam::__vpiRealParam(__vpiScope* scope, ഇരട്ട val, സ്ഥിരം അക്ഷരം*name,
                                      തർക്കം local_flag, long file_idx, long lineno)
: __vpiRealConst(val)
{
      scope_ = scope;
      basename_ = name;
      local_flag_ = local_flag;
      പ്രതിജ്ഞാ(file_idx >= 0 && file_idx <= INT_MAX);
      file_idx_ = (unsigned) file_idx;
      പ്രതിജ്ഞാ(lineno >= 0 && lineno <= INT_MAX);
      lineno_ = (unsigned) lineno;
}

__vpiRealParam::~__vpiRealParam()
{
      delete[]basename_;
}


പൂർണ്ണസംഖ്യ __vpiRealParam::get_type_code(ശൂന്യം) സ്ഥിരം
{ തിരികെ vpiParameter; }

പൂർണ്ണസംഖ്യ __vpiRealParam::vpi_get(പൂർണ്ണസംഖ്യ code)
{
    തിരഞ്ഞെടുപ്പ് (code) {
      അവസ്ഥ vpi_വരി_No :
        തിരികെ lineno_;

      അവസ്ഥ vpiLocalParam :
        തിരികെ local_flag_;

      സ്ഥിരസ്ഥിതി :
           തിരികെ __vpiRealConst::vpi_get(code);
    }
}

അക്ഷരം* __vpiRealParam::vpi_get_str(പൂർണ്ണസംഖ്യ code)
{
      എങ്കിൽ (code == vpiFile)
            തിരികെ simple_set_rbuf_str(file_names[file_idx_]);

      തിരികെ generic_get_str(code, scope_, basename_, ശൂന്യ);
}

vpiHandle __vpiRealParam::vpi_handle(പൂർണ്ണസംഖ്യ code)
{
      തിരഞ്ഞെടുപ്പ് (code) {
          അവസ്ഥ vpiScope:
            തിരികെ scope_;

	  അവസ്ഥ vpiModule:
	    തിരികെ vpip_module(scope_);

          സ്ഥിരസ്ഥിതി:
            തിരികെ 0;
      }
}


vpiHandle vpip_make_real_param(സ്ഥിരം അക്ഷരം*name, ഇരട്ട value,
                               തർക്കം local_flag, long file_idx, long lineno)
{
      ഘടന __vpiRealParam*obj = പുതിയ __vpiRealParam(vpip_peek_current_scope(),
                                                     value, name, local_flag,
                                                     file_idx, lineno);
      തിരികെ obj;
}

/*
 * Make a VPI null constant
 */
വരിയിൽ __vpiNullConst::__vpiNullConst()
{ }

പൂർണ്ണസംഖ്യ __vpiNullConst::get_type_code(ശൂന്യം) സ്ഥിരം
{ തിരികെ vpiConstant; }

പൂർണ്ണസംഖ്യ __vpiNullConst::vpi_get(പൂർണ്ണസംഖ്യ code)
{
      തിരഞ്ഞെടുപ്പ് (code) {
	  അവസ്ഥ vpi_വരി_No:
	    തിരികെ 0;  // Not implemented for now!

	  അവസ്ഥ vpiSize:
	    തിരികെ 32;

	  അവസ്ഥ vpiConstType:
	    തിരികെ vpiNullConst;

	  അവസ്ഥ vpiSigned:
	    തിരികെ 0;

          അവസ്ഥ vpiAutomatic:
	    തിരികെ 0;

#എങ്കിൽ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          അവസ്ഥ _vpiFromThr:
	      തിരികെ _vpiNoThr;
#endif

	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiNullConst\n", code);
	    പ്രതിജ്ഞാ(0);
	    തിരികെ 0;
      }
}

ശൂന്യം __vpiNullConst::vpi_get_മൂല്യം(p_vpi_മൂല്യം val)
{
      സ്ഥിതം സ്ഥിരം size_t RBUF_USE_SIZE = 64 + 1;
      അക്ഷരം*rbuf = static_cast<അക്ഷരം *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));

      തിരഞ്ഞെടുപ്പ് (val->format) {

	  അവസ്ഥ vpiObjTypeVal:
	    val->format = vpiStringVal;
	    // fall through
	  അവസ്ഥ vpiBinStrVal:
	  അവസ്ഥ vpiDecStrVal:
	  അവസ്ഥ vpiOctStrVal:
	  അവസ്ഥ vpiHexStrVal:
	  അവസ്ഥ vpiStringVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "null");
	    val->value.str = rbuf;
	    നിർത്തുക;

          അവസ്ഥ vpiScalarVal:
	    val->value.scalar = vpi0;
	    നിർത്തുക;

	  അവസ്ഥ vpiIntVal:
	    val->value.integer = 0;
	    നിർത്തുക;

	  അവസ്ഥ vpiVectorVal:
	    val->value.vector = static_cast<p_vpi_vecval>
	                        (need_result_buf(2*വലുപ്പം(s_vpi_vecval),
	                                         RBUF_VAL));
	    വേണ്ടി (unsigned idx = 0; idx < 2; idx += 1) {
		  val->value.vector[idx].aval = 0;
		  val->value.vector[idx].bval = 0;
	    }
	    നിർത്തുക;

	  അവസ്ഥ vpiRealVal:
	    val->value.real = 0.0;
	    നിർത്തുക;

	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "vvp error: format %d not supported "
		    "by vpiNullConst\n", (പൂർണ്ണസംഖ്യ)val->format);
	    val->format = vpiSuppressVal;
	    നിർത്തുക;
      }
}

vpiHandle vpip_make_null_const()
{
      __vpiNullConst*obj = പുതിയ __vpiNullConst;
      തിരികെ obj;
}

#ifdef CHECK_WITH_VALGRIND
ശൂന്യം constant_delete(vpiHandle item)
{
      പ്രതിജ്ഞാ(item->get_type_code() == vpiConstant);
      തിരഞ്ഞെടുപ്പ്(vpi_get(vpiConstType, item)) {
	  അവസ്ഥ vpiStringConst:
	    delete dynamic_cast<__vpiStringConst*>(item);
	    നിർത്തുക;
	  അവസ്ഥ vpiDecConst:
	    delete dynamic_cast<__vpiDecConst*>(item);
	    നിർത്തുക;
	  അവസ്ഥ vpi_ദ്വിമാന_Const:
	    delete dynamic_cast<__vpiBinaryConst*>(item);
	    നിർത്തുക;
	  അവസ്ഥ vpiRealConst:
	    delete dynamic_cast<__vpiRealConst*>(item);
	    നിർത്തുക;
	  അവസ്ഥ vpiNullConst:
	    delete dynamic_cast<__vpiNullConst*>(item);
	    നിർത്തുക;
	  സ്ഥിരസ്ഥിതി:
	    പ്രതിജ്ഞാ(0);
      }
}

ശൂന്യം parameter_delete(vpiHandle item)
{
      തിരഞ്ഞെടുപ്പ്(vpi_get(vpiConstType, item)) {
	  അവസ്ഥ vpiStringConst: {
	    വർഗ്ഗം __vpiStringParam*rfp = dynamic_cast<__vpiStringParam*>(item);
	    delete rfp;
	    നിർത്തുക; }
	  അവസ്ഥ vpi_ദ്വിമാന_Const: {
	    ഘടന __vpiBinaryParam*rfp = dynamic_cast<__vpiBinaryParam*>(item);
	    delete rfp;
	    നിർത്തുക; }
	  അവസ്ഥ vpiRealConst: {
	    ഘടന __vpiRealParam*rfp = dynamic_cast<__vpiRealParam*>(item);
	    delete rfp;
	    നിർത്തുക; }
	  സ്ഥിരസ്ഥിതി:
	    പ്രതിജ്ഞാ(0);
      }
}
#endif
