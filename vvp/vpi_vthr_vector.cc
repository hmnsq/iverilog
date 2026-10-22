/*
 * Copyright (c) 2001-2026 Stephen Williams (steve@icarus.com)
 * Copyright (c) 2001 Stephan Boettcher <stephan@nevis.columbia.edu>
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

/*
 * vpiReg handles are handled here. These objects represent vectors of
 * .var objects that can be manipulated by the VPI module.
 */

# include  "vpi_priv.h"
# include  "vthread.h"
# include  "config.h"
#ifdef CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
# include <map>
#endif
# include  <cstdio>
# include  <cstdlib>
# include  <cassert>
# include  "ivl_alloc.h"

using നാമാകാശ std;

/*
 * Hex digits that represent 4-value bits of Verilog are not as
 * trivially obvious to display as if the bits were the usual 2-value
 * bits. So, although it is possible to write a function that
 * generates a correct character for 4*4-value bits, it is easier to
 * just perform the lookup in a table. This only takes 256 bytes,
 * which is not many executable instructions:-)
 *
 * The table is calculated as compile time, therefore, by the
 * draw_tt.c program.
 */
ബാഹ്യ സ്ഥിരം അക്ഷരം hex_digits[256];
ബാഹ്യ സ്ഥിരം അക്ഷരം oct_digits[64];


വർഗ്ഗം __vpiVThrWord : പൊതു __vpiHandle {
    പൊതു:
      explicit __vpiVThrWord(unsigned base);
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override;
      പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code) override;
      ശൂന്യം vpi_get_മൂല്യം(p_vpi_മൂല്യം val) override;

      പൂർണ്ണസംഖ്യ get_subtype() സ്ഥിരം { തിരികെ subtype; };
      unsigned get_index() സ്ഥിരം { തിരികെ index; };

    സ്വകാര്യ:
      സ്ഥിരം അക്ഷരം* name;
      പൂർണ്ണസംഖ്യ subtype;
      unsigned index;
};

സ്ഥിതം പൂർണ്ണസംഖ്യ vthr_word_get(പൂർണ്ണസംഖ്യ code, vpiHandle ref)
{
      സ്ഥിരം __vpiVThrWord*rfp = dynamic_cast<__vpiVThrWord*>(ref);

      തിരഞ്ഞെടുപ്പ് (code) {

	  അവസ്ഥ vpiConstType:
	    തിരികെ rfp->get_subtype();

#എങ്കിൽ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  അവസ്ഥ _vpiFromThr:
	    തിരികെ _vpiWord;
#endif

	  സ്ഥിരസ്ഥിതി:
	    തിരികെ 0;
      }
}

സ്ഥിതം ശൂന്യം vthr_real_get_മൂല്യം(vpiHandle ref, s_vpi_മൂല്യം*vp)
{
      സ്ഥിരം __vpiVThrWord*obj = dynamic_cast<__vpiVThrWord*>(ref);
      സ്ഥിതം സ്ഥിരം size_t RBUF_USE_SIZE = 66;
      അക്ഷരം *rbuf = static_cast<അക്ഷരം *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));

      ഇരട്ട val = 0.0;

	/* Get the actual value from the index. It is possible, by the
	   way, that the vpi_get_value is called from compiletf. If
	   that's the case, there will be no current thread, and this
	   will not have access to the proper value. Punt and return a
	   0.0 value instead. */
      എങ്കിൽ (vpip_current_vthread)
	    val = vthread_get_real_അട്ടി(vpip_current_vthread,
	                                 obj->get_index());

      തിരഞ്ഞെടുപ്പ് (vp->format) {

	  അവസ്ഥ vpiObjTypeVal:
	    vp->format = vpiRealVal;
	    // fallthrough
	  അവസ്ഥ vpiRealVal:
	    vp->value.real = val;
	    നിർത്തുക;

	  അവസ്ഥ vpiIntVal:
	      /* NaN or +/- infinity are translated as 0. */
	    എങ്കിൽ (val != val || (val && (val == 0.5*val))) {
		  val = 0.0;
	    } അല്ലെങ്കിൽ {
		  val = std::round(val);
	    }
	    vp->value.integer = (PLI_INT32)val;
	    നിർത്തുക;

	  അവസ്ഥ vpiDecStrVal:
	    എങ്കിൽ (std::isnan(val))
		  snprintf(rbuf, RBUF_USE_SIZE, "%s", "nan");
	    അല്ലെങ്കിൽ
		  snprintf(rbuf, RBUF_USE_SIZE, "%0.0f", std::round(val));
	    vp->value.str = rbuf;
	    നിർത്തുക;

	  അവസ്ഥ vpiOctStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" PRIo64, vlg_round_to_u64(val));
	    vp->value.str = rbuf;
	    നിർത്തുക;

	  അവസ്ഥ vpiHexStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" PRIx64, vlg_round_to_u64(val));
	    vp->value.str = rbuf;
	    നിർത്തുക;

	  അവസ്ഥ vpiBinStrVal: {
		uint64_t vali = vlg_round_to_u64(val);
		unsigned len = 0;

		ആകുമ്പോൾ (vali > 0) {
		      len += 1;
		      vali /= 2;
		}

		vali = vlg_round_to_u64(val);
		വേണ്ടി (unsigned idx = 0 ;  idx < len ;  idx += 1) {
		      rbuf[len-idx-1] = (vali & 1)? '1' : '0';
		      vali /= 2;
		}

		rbuf[len] = 0;
		എങ്കിൽ (len == 0) {
		      rbuf[0] = '0';
		      rbuf[1] = 0;
		}
		vp->value.str = rbuf;
		നിർത്തുക;
	  }

	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiConstant (Real)\n", (പൂർണ്ണസംഖ്യ)vp->format);

	    vp->format = vpiSuppressVal;
	    നിർത്തുക;
      }
}

വരിയിൽ __vpiVThrWord::__vpiVThrWord(unsigned base)
{
      name = vpip_name_string("W<>");
      subtype = vpiRealConst;
      index = base;
}

പൂർണ്ണസംഖ്യ __vpiVThrWord::get_type_code(ശൂന്യം) സ്ഥിരം
{ തിരികെ vpiConstant; }

പൂർണ്ണസംഖ്യ __vpiVThrWord::vpi_get(പൂർണ്ണസംഖ്യ code)
{ തിരികെ vthr_word_get(code, ഇത്); }

ശൂന്യം __vpiVThrWord::vpi_get_മൂല്യം(p_vpi_മൂല്യം val)
{ vthr_real_get_മൂല്യം(ഇത്, val); }

vpiHandle vpip_make_vthr_word(unsigned base, സ്ഥിരം അക്ഷരം*type)
{
      പ്രതിജ്ഞാ(type[0] == 'r');
      പ്രതിജ്ഞാ(base < 65536);
      __vpiVThrWord*obj = പുതിയ __vpiVThrWord(base);

      തിരികെ obj;
}

#ifdef CHECK_WITH_VALGRIND
സ്ഥിതം map<vpiHandle, തർക്കം> handle_map;

ശൂന്യം thread_word_delete(vpiHandle item)
{
      handle_map[item] = അസത്യം;
}

സ്ഥിതം ശൂന്യം thread_word_delete_real(vpiHandle item)
{
      വർഗ്ഗം __vpiVThrWord*obj = dynamic_cast<__vpiVThrWord*>(item);
      delete obj;
}
#endif

വർഗ്ഗം __vpiVThrStrStack : പൊതു __vpiHandle {
    പൊതു:
      explicit __vpiVThrStrStack(unsigned depth);
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override;
      പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code) override;
      ശൂന്യം vpi_get_മൂല്യം(p_vpi_മൂല്യം val) override;
    സ്വകാര്യ:
      unsigned depth_;
};

__vpiVThrStrStack::__vpiVThrStrStack(unsigned d)
: depth_(d)
{
}

പൂർണ്ണസംഖ്യ __vpiVThrStrStack::get_type_code(ശൂന്യം) സ്ഥിരം
{ തിരികെ vpiConstant; }

പൂർണ്ണസംഖ്യ __vpiVThrStrStack::vpi_get(പൂർണ്ണസംഖ്യ code)
{
      തിരഞ്ഞെടുപ്പ് (code) {
	  അവസ്ഥ vpiConstType:
	    തിരികെ vpiStringConst;
#എങ്കിൽ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  അവസ്ഥ _vpiFromThr:
	    തിരികെ _vpiString;
#endif
	  അവസ്ഥ vpiSize:
	    തിരികെ vthread_get_str_അട്ടി(vpip_current_vthread, depth_).size();
	  സ്ഥിരസ്ഥിതി:
	    തിരികെ 0;
      }
}

ശൂന്യം __vpiVThrStrStack::vpi_get_മൂല്യം(p_vpi_മൂല്യം vp)
{
      string val;
      അക്ഷരം*rbuf = 0;

      എങ്കിൽ (vpip_current_vthread)
	    val = vthread_get_str_അട്ടി(vpip_current_vthread, depth_);

      തിരഞ്ഞെടുപ്പ് (vp->format) {

	  അവസ്ഥ vpiObjTypeVal:
	    vp->format = vpiStringVal;
	    // fallthrough
	  അവസ്ഥ vpiStringVal:
	    rbuf = static_cast<അക്ഷരം *>(need_result_buf(val.size()+1, RBUF_VAL));
	    strcpy(rbuf, val.c_str());
	    vp->value.str = rbuf;
	    നിർത്തുക;

	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiConstant (String)\n", (പൂർണ്ണസംഖ്യ)vp->format);

	    vp->format = vpiSuppressVal;
	    നിർത്തുക;
      }
}

വർഗ്ഗം __vpiVThrVec4Stack : പൊതു __vpiHandle {
    പൊതു:
      __vpiVThrVec4Stack(unsigned depth, തർക്കം signed_flag, unsigned wid);
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override;
      പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code) override;
      അക്ഷരം*vpi_get_str(പൂർണ്ണസംഖ്യ code) override;
      ശൂന്യം vpi_get_മൂല്യം(p_vpi_മൂല്യം val) override;
      vpiHandle vpi_put_മൂല്യം(p_vpi_മൂല്യം val, പൂർണ്ണസംഖ്യ flags) override;
    സ്വകാര്യ:
      സ്ഥിതം ശൂന്യം vpi_get_മൂല്യം_string(p_vpi_മൂല്യം vp, സ്ഥിരം vvp_vector4_t&val);
      സ്ഥിതം ശൂന്യം vpi_get_മൂല്യം_binstr(p_vpi_മൂല്യം vp, സ്ഥിരം vvp_vector4_t&val);
      ശൂന്യം vpi_get_മൂല്യം_decstr(p_vpi_മൂല്യം vp, സ്ഥിരം vvp_vector4_t&val);
      ശൂന്യം vpi_get_മൂല്യം_int   (p_vpi_മൂല്യം vp, സ്ഥിരം vvp_vector4_t&val);
      സ്ഥിതം ശൂന്യം vpi_get_മൂല്യം_real  (p_vpi_മൂല്യം vp, സ്ഥിരം vvp_vector4_t&val);
      സ്ഥിതം ശൂന്യം vpi_get_മൂല്യം_strength(p_vpi_മൂല്യം vp, സ്ഥിരം vvp_vector4_t&val);
      സ്ഥിതം ശൂന്യം vpi_get_മൂല്യം_octstr(p_vpi_മൂല്യം vp, സ്ഥിരം vvp_vector4_t&val);
      സ്ഥിതം ശൂന്യം vpi_get_മൂല്യം_hexstr(p_vpi_മൂല്യം vp, സ്ഥിരം vvp_vector4_t&val);
      സ്ഥിതം ശൂന്യം vpi_get_മൂല്യം_vector(p_vpi_മൂല്യം vp, സ്ഥിരം vvp_vector4_t&val);
    സ്വകാര്യ:
      unsigned depth_;
      തർക്കം signed_flag_;
      unsigned expect_width_;
      സ്ഥിരം അക്ഷരം*name;
};

__vpiVThrVec4Stack::__vpiVThrVec4Stack(unsigned d, തർക്കം sf, unsigned wid)
: depth_(d), signed_flag_(sf), expect_width_(wid)
{
      name = vpip_name_string("S<,vec4,>");
}

പൂർണ്ണസംഖ്യ __vpiVThrVec4Stack::get_type_code(ശൂന്യം) സ്ഥിരം
{ തിരികെ vpiConstant; }


പൂർണ്ണസംഖ്യ __vpiVThrVec4Stack::vpi_get(പൂർണ്ണസംഖ്യ code)
{
      തിരഞ്ഞെടുപ്പ് (code) {
	  അവസ്ഥ vpiSize:
	    തിരികെ expect_width_;

	  അവസ്ഥ vpiSigned:
	    തിരികെ signed_flag_? 1 : 0;

	  അവസ്ഥ vpiConstType:
	    തിരികെ vpi_ദ്വിമാന_Const;

#എങ്കിൽ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  അവസ്ഥ _vpiFromThr:
	    തിരികെ _vpiVThr;
#endif

	  സ്ഥിരസ്ഥിതി:
	    തിരികെ 0;
      }
}

അക്ഷരം*__vpiVThrVec4Stack::vpi_get_str(പൂർണ്ണസംഖ്യ code)
{
      തിരഞ്ഞെടുപ്പ് (code) {
	  അവസ്ഥ vpiFullName:
	    തിരികെ simple_set_rbuf_str(name);

	  സ്ഥിരസ്ഥിതി:
	    തിരികെ 0;
      }
}

ശൂന്യം __vpiVThrVec4Stack::vpi_get_മൂല്യം(p_vpi_മൂല്യം vp)
{
      vvp_vector4_t val;

      എങ്കിൽ (vpip_current_vthread)
	    val = vthread_get_vec4_അട്ടി(vpip_current_vthread, depth_);

      തിരഞ്ഞെടുപ്പ് (vp->format) {

	  അവസ്ഥ vpiBinStrVal:
	    vpi_get_മൂല്യം_binstr(vp, val);
	    നിർത്തുക;
	  അവസ്ഥ vpiDecStrVal:
	    vpi_get_മൂല്യം_decstr(vp, val);
	    നിർത്തുക;
	  അവസ്ഥ vpiOctStrVal:
	    vpi_get_മൂല്യം_octstr(vp, val);
	    നിർത്തുക;
	  അവസ്ഥ vpiHexStrVal:
	    vpi_get_മൂല്യം_hexstr(vp, val);
	    നിർത്തുക;
	  അവസ്ഥ vpiIntVal:
	    vpi_get_മൂല്യം_int(vp, val);
	    നിർത്തുക;
	  അവസ്ഥ vpiRealVal:
	    vpi_get_മൂല്യം_real(vp, val);
	    നിർത്തുക;
	  അവസ്ഥ vpiStringVal:
	    vpi_get_മൂല്യം_string(vp, val);
	    നിർത്തുക;
	  അവസ്ഥ vpiStrengthVal:
	    vpi_get_മൂല്യം_strength(vp, val);
	    നിർത്തുക;
	  അവസ്ഥ vpiObjTypeVal:
	    vp->format = vpiVectorVal;
	    // fallthrough
	  അവസ്ഥ vpiVectorVal:
	    vpi_get_മൂല്യം_vector(vp, val);
	    നിർത്തുക;

	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "internal error: vpi_get_value(<format=%d>)"
		    " not implemented for __vpiVThrVec4Stack.\n", vp->format);
	    പ്രതിജ്ഞാ(0);
      }

}

ശൂന്യം __vpiVThrVec4Stack::vpi_get_മൂല്യം_binstr(p_vpi_മൂല്യം vp, സ്ഥിരം vvp_vector4_t&val)
{
      unsigned wid = val.size();
      അക്ഷരം*rbuf = static_cast<അക്ഷരം*>(need_result_buf(wid+1, RBUF_VAL));
      വേണ്ടി (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    rbuf[wid-idx-1] = vvp_bit4_to_ascii(val.value(idx));
      }
      rbuf[wid] = 0;
      vp->value.str = rbuf;
}

ശൂന്യം __vpiVThrVec4Stack::vpi_get_മൂല്യം_decstr(p_vpi_മൂല്യം vp, സ്ഥിരം vvp_vector4_t&val)
{
      unsigned wid = val.size();
      പൂർണ്ണസംഖ്യ nbuf = (wid+2)/3 + 1;
      അക്ഷരം *rbuf = static_cast<അക്ഷരം*>(need_result_buf(nbuf, RBUF_VAL));

      vpip_vec4_to_dec_str(val, rbuf, nbuf, signed_flag_);
      vp->value.str = rbuf;
}

ശൂന്യം __vpiVThrVec4Stack::vpi_get_മൂല്യം_octstr(p_vpi_മൂല്യം vp, സ്ഥിരം vvp_vector4_t&val)
{
      unsigned wid = val.size();
      unsigned owid = (wid + 2) / 3;
      അക്ഷരം*rbuf = static_cast<അക്ഷരം*>(need_result_buf(owid+1, RBUF_VAL));
      rbuf[owid] = 0;

      unsigned oval = 0;
      വേണ്ടി (unsigned idx = 0; idx < wid ; idx += 1) {
	    unsigned tmp = 0;
	    തിരഞ്ഞെടുപ്പ് (val.value(idx)) {
		അവസ്ഥ BIT4_0:
		  tmp = 0;
		  നിർത്തുക;
		അവസ്ഥ BIT4_1:
		  tmp = 1;
		  നിർത്തുക;
		അവസ്ഥ BIT4_X:
		  tmp = 2;
		  നിർത്തുക;
		അവസ്ഥ BIT4_Z:
		  tmp = 3;
		  നിർത്തുക;
	    }

	    oval = oval | (tmp << 2*(idx%3));

	    എങ്കിൽ (idx%3 == 2) {
		  owid -= 1;
		  rbuf[owid] = oct_digits[oval];
		  oval = 0;
	    }
      }

      എങ്കിൽ (owid > 0) {
	    owid -= 1;
	    rbuf[owid] = oct_digits[oval];
      }
      vp->value.str = rbuf;
}

ശൂന്യം __vpiVThrVec4Stack::vpi_get_മൂല്യം_hexstr(p_vpi_മൂല്യം vp, സ്ഥിരം vvp_vector4_t&val)
{
      unsigned wid = val.size();
      unsigned hwid = (wid + 3) /4;
      അക്ഷരം*rbuf = static_cast<അക്ഷരം*>(need_result_buf(hwid+1, RBUF_VAL));
      rbuf[hwid] = 0;

      unsigned hval = 0;
      വേണ്ടി (unsigned idx = 0; idx < wid ; idx += 1) {
	    unsigned tmp = 0;
	    തിരഞ്ഞെടുപ്പ് (val.value(idx)) {
		അവസ്ഥ BIT4_0:
		  tmp = 0;
		  നിർത്തുക;
		അവസ്ഥ BIT4_1:
		  tmp = 1;
		  നിർത്തുക;
		അവസ്ഥ BIT4_X:
		  tmp = 2;
		  നിർത്തുക;
		അവസ്ഥ BIT4_Z:
		  tmp = 3;
		  നിർത്തുക;
	    }
	    hval = hval | (tmp << 2*(idx%4));

	    എങ്കിൽ (idx%4 == 3) {
		  hwid -= 1;
		  rbuf[hwid] = hex_digits[hval];
		  hval = 0;
	    }
      }

      എങ്കിൽ (hwid > 0) {
	    hwid -= 1;
	    rbuf[hwid] = hex_digits[hval];
      }
      vp->value.str = rbuf;
}

ശൂന്യം __vpiVThrVec4Stack::vpi_get_മൂല്യം_int(p_vpi_മൂല്യം vp, സ്ഥിരം vvp_vector4_t&val)
{
      int32_t vali = 0;
      vector4_to_മൂല്യം(val, vali, signed_flag_, അസത്യം);
      vp->value.integer = vali;
}

ശൂന്യം __vpiVThrVec4Stack::vpi_get_മൂല്യം_real(p_vpi_മൂല്യം vp, സ്ഥിരം vvp_vector4_t&val)
{
      unsigned wid = val.size();
      vp->value.real = 0.0;

      വേണ്ടി (unsigned idx = wid ; idx > 0 ; idx -= 1) {
	    vp->value.real *= 2.0;
	    എങ്കിൽ (val.value(idx-1) == BIT4_1)
		  vp->value.real += 1.0;
      }
}

ശൂന്യം __vpiVThrVec4Stack::vpi_get_മൂല്യം_string(p_vpi_മൂല്യം vp, സ്ഥിരം vvp_vector4_t&val)
{
      അക്ഷരം*rbuf = static_cast<അക്ഷരം*>(need_result_buf((val.size() / 8) + 1, RBUF_VAL));
      അക്ഷരം*cp = rbuf;

      അക്ഷരം tmp = 0;
      വേണ്ടി (പൂർണ്ണസംഖ്യ bitnr = val.size()-1 ; bitnr >= 0 ; bitnr -= 1) {
	    tmp <<= 1;
	    തിരഞ്ഞെടുപ്പ് (val.value(bitnr)) {
		അവസ്ഥ BIT4_1:
		  tmp |= 1;
		  നിർത്തുക;
		അവസ്ഥ BIT4_0:
		സ്ഥിരസ്ഥിതി:
		  നിർത്തുക;
	    }

	    എങ്കിൽ ((bitnr&7)==0) {
		    // Don't include leading nuls
		  എങ്കിൽ (tmp == 0 && cp == rbuf)
			തുടരുക;

		  *cp++ = tmp? tmp : ' ';
		  tmp = 0;
	    }
      }
      *cp++ = 0;
      vp->format = vpiStringVal;
      vp->value.str = rbuf;
}

ശൂന്യം __vpiVThrVec4Stack::vpi_get_മൂല്യം_vector(p_vpi_മൂല്യം vp, സ്ഥിരം vvp_vector4_t&val)
{
      unsigned wid = val.size();

      unsigned hwid = (wid+31)/32;
      vp->value.vector = static_cast<s_vpi_vecval*>
                         (need_result_buf(hwid*വലുപ്പം(s_vpi_vecval), RBUF_VAL));
      പ്രതിജ്ഞാ(vp->value.vector);

	/* Zero the top of the result buffer first since the fill loop below only writes
	   bits 0..wid-1, which can leave stale data in the unused high bits. */
      എങ്കിൽ (hwid == 0) hwid = 1;
      vp->value.vector[hwid-1].aval = 0;
      vp->value.vector[hwid-1].bval = 0;

      വേണ്ടി (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
	    പൂർണ്ണസംഖ്യ word = idx/32;
	    PLI_INT32 mask = 1 << (idx%32);

	    തിരഞ്ഞെടുപ്പ് (val.value(idx)) {
		അവസ്ഥ BIT4_0:
		  vp->value.vector[word].aval &= ~mask;
		  vp->value.vector[word].bval &= ~mask;
		  നിർത്തുക;
		അവസ്ഥ BIT4_1:
		  vp->value.vector[word].aval |=  mask;
		  vp->value.vector[word].bval &= ~mask;
		  നിർത്തുക;
		അവസ്ഥ BIT4_X:
		  vp->value.vector[word].aval |=  mask;
		  vp->value.vector[word].bval |=  mask;
		  നിർത്തുക;
		അവസ്ഥ BIT4_Z:
		  vp->value.vector[word].aval &= ~mask;
		  vp->value.vector[word].bval |=  mask;
		  നിർത്തുക;
	    }
      }
}

ശൂന്യം __vpiVThrVec4Stack::vpi_get_മൂല്യം_strength(p_vpi_മൂല്യം vp, സ്ഥിരം vvp_vector4_t&val)
{
      s_vpi_strengthval*op = static_cast<s_vpi_strengthval*>
                             (need_result_buf(val.size() * വലുപ്പം(s_vpi_strengthval),
                                              RBUF_VAL));

      വേണ്ടി (unsigned idx = 0 ; idx < val.size() ; idx += 1) {
	    തിരഞ്ഞെടുപ്പ് (val.value(idx)) {
		അവസ്ഥ BIT4_0:
		  op[idx].logic = vpi0;
		  op[idx].s0 = vpiStrongDrive;
		  op[idx].s1 = 0;
		  നിർത്തുക;
		അവസ്ഥ BIT4_1:
		  op[idx].logic = vpi1;
		  op[idx].s0 = 0;
		  op[idx].s1 = vpiStrongDrive;
		  നിർത്തുക;
		അവസ്ഥ BIT4_X:
		  op[idx].logic = vpiX;
		  op[idx].s0 = vpiStrongDrive;
		  op[idx].s1 = vpiStrongDrive;
		  നിർത്തുക;
		അവസ്ഥ BIT4_Z:
		  op[idx].logic = vpiZ;
		  op[idx].s0 = vpiHiZ;
		  op[idx].s1 = vpiHiZ;
		  നിർത്തുക;
	    }
      }

      vp->format = vpiStrengthVal;
      vp->value.strength = op;
}

vpiHandle __vpiVThrVec4Stack::vpi_put_മൂല്യം(p_vpi_മൂല്യം vp, പൂർണ്ണസംഖ്യ /*flags*/)
{
      പ്രതിജ്ഞാ(vpip_current_vthread);

      തിരഞ്ഞെടുപ്പ് (vp->format) {

	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "internal error: vpi_put_value(<format=%d>)"
		    " not implemented for __vpiVThrVec4Stack.\n", vp->format);
	    പ്രതിജ്ഞാ(0);
	    തിരികെ 0;
      }
}

#ifdef CHECK_WITH_VALGRIND
ശൂന്യം thread_vthr_delete(vpiHandle item)
{
      handle_map[item] = സത്യം;
}

സ്ഥിതം ശൂന്യം thread_vthr_delete_real(vpiHandle item)
{
      __vpiVThrVec4Stack*obj = dynamic_cast<__vpiVThrVec4Stack*>(item);
      delete obj;
}

ശൂന്യം vpi_handle_delete()
{
      map<vpiHandle, തർക്കം>::iterator iter;
      വേണ്ടി (iter = handle_map.begin(); iter != handle_map.end(); ++ iter ) {
	    എങ്കിൽ (iter->second) thread_vthr_delete_real(iter->first);
	    അല്ലെങ്കിൽ thread_word_delete_real(iter->first);
      }
}
#endif


vpiHandle vpip_make_vthr_str_അട്ടി(unsigned depth)
{
      __vpiVThrStrStack*obj = പുതിയ __vpiVThrStrStack(depth);
      തിരികെ obj;
}

vpiHandle vpip_make_vthr_vec4_അട്ടി(unsigned depth, തർക്കം signed_flag, unsigned wid)
{
      __vpiVThrVec4Stack*obj = പുതിയ __vpiVThrVec4Stack(depth, signed_flag, wid);
      തിരികെ obj;
}

#ifdef CHECK_WITH_VALGRIND
സ്ഥിതം map<vpiHandle, തർക്കം> അട്ടി_map;

ശൂന്യം thread_string_delete(vpiHandle item)
{
      അട്ടി_map[item] = അസത്യം;
}

സ്ഥിതം ശൂന്യം thread_string_delete_real(vpiHandle item)
{
      __vpiVThrStrStack*obj = dynamic_cast<__vpiVThrStrStack*>(item);
      delete obj;
}

ശൂന്യം vpi_അട്ടി_delete()
{
      map<vpiHandle, തർക്കം>::iterator iter;
      വേണ്ടി (iter = അട്ടി_map.begin(); iter != അട്ടി_map.end(); ++ iter ) {
	    thread_string_delete_real(iter->first);
      }
}
#endif
