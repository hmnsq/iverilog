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

using நாமாகாஶ std;

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
வெளி மாறிலி எழுத்து hex_digits[256];
வெளி மாறிலி எழுத்து oct_digits[64];


வகுப்பு __vpiVThrWord : பொது __vpiHandle {
    பொது:
      explicit __vpiVThrWord(unsigned base);
      முழு get_type_code(வெறுமை) மாறிலி override;
      முழு vpi_get(முழு code) override;
      வெறுமை vpi_get_மதிப்பு(p_vpi_மதிப்பு val) override;

      முழு get_subtype() மாறிலி { திரும்பு subtype; };
      unsigned get_index() மாறிலி { திரும்பு index; };

    தனிப்பட்ட:
      மாறிலி எழுத்து* name;
      முழு subtype;
      unsigned index;
};

நிலையான முழு vthr_word_get(முழு code, vpiHandle ref)
{
      மாறிலி __vpiVThrWord*rfp = dynamic_cast<__vpiVThrWord*>(ref);

      தேர்வு (code) {

	  நிலை vpiConstType:
	    திரும்பு rfp->get_subtype();

#எனில் defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  நிலை _vpiFromThr:
	    திரும்பு _vpiWord;
#endif

	  இயல்பு:
	    திரும்பு 0;
      }
}

நிலையான வெறுமை vthr_real_get_மதிப்பு(vpiHandle ref, s_vpi_மதிப்பு*vp)
{
      மாறிலி __vpiVThrWord*obj = dynamic_cast<__vpiVThrWord*>(ref);
      நிலையான மாறிலி size_t RBUF_USE_SIZE = 66;
      எழுத்து *rbuf = static_cast<எழுத்து *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));

      இரட்டை val = 0.0;

	/* Get the actual value from the index. It is possible, by the
	   way, that the vpi_get_value is called from compiletf. If
	   that's the case, there will be no current thread, and this
	   will not have access to the proper value. Punt and return a
	   0.0 value instead. */
      எனில் (vpip_current_vthread)
	    val = vthread_get_real_அடுக்கு(vpip_current_vthread,
	                                 obj->get_index());

      தேர்வு (vp->format) {

	  நிலை vpiObjTypeVal:
	    vp->format = vpiRealVal;
	    // fallthrough
	  நிலை vpiRealVal:
	    vp->value.real = val;
	    நிறுத்து;

	  நிலை vpiIntVal:
	      /* NaN or +/- infinity are translated as 0. */
	    எனில் (val != val || (val && (val == 0.5*val))) {
		  val = 0.0;
	    } இல்லையெனில் {
		  val = std::round(val);
	    }
	    vp->value.integer = (PLI_INT32)val;
	    நிறுத்து;

	  நிலை vpiDecStrVal:
	    எனில் (std::isnan(val))
		  snprintf(rbuf, RBUF_USE_SIZE, "%s", "nan");
	    இல்லையெனில்
		  snprintf(rbuf, RBUF_USE_SIZE, "%0.0f", std::round(val));
	    vp->value.str = rbuf;
	    நிறுத்து;

	  நிலை vpiOctStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" PRIo64, vlg_round_to_u64(val));
	    vp->value.str = rbuf;
	    நிறுத்து;

	  நிலை vpiHexStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" PRIx64, vlg_round_to_u64(val));
	    vp->value.str = rbuf;
	    நிறுத்து;

	  நிலை vpiBinStrVal: {
		uint64_t vali = vlg_round_to_u64(val);
		unsigned len = 0;

		போது (vali > 0) {
		      len += 1;
		      vali /= 2;
		}

		vali = vlg_round_to_u64(val);
		க்கு (unsigned idx = 0 ;  idx < len ;  idx += 1) {
		      rbuf[len-idx-1] = (vali & 1)? '1' : '0';
		      vali /= 2;
		}

		rbuf[len] = 0;
		எனில் (len == 0) {
		      rbuf[0] = '0';
		      rbuf[1] = 0;
		}
		vp->value.str = rbuf;
		நிறுத்து;
	  }

	  இயல்பு:
	    fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiConstant (Real)\n", (முழு)vp->format);

	    vp->format = vpiSuppressVal;
	    நிறுத்து;
      }
}

உள்வரி __vpiVThrWord::__vpiVThrWord(unsigned base)
{
      name = vpip_name_string("W<>");
      subtype = vpiRealConst;
      index = base;
}

முழு __vpiVThrWord::get_type_code(வெறுமை) மாறிலி
{ திரும்பு vpiConstant; }

முழு __vpiVThrWord::vpi_get(முழு code)
{ திரும்பு vthr_word_get(code, இது); }

வெறுமை __vpiVThrWord::vpi_get_மதிப்பு(p_vpi_மதிப்பு val)
{ vthr_real_get_மதிப்பு(இது, val); }

vpiHandle vpip_make_vthr_word(unsigned base, மாறிலி எழுத்து*type)
{
      ப்ரதிஜ்ஞா(type[0] == 'r');
      ப்ரதிஜ்ஞா(base < 65536);
      __vpiVThrWord*obj = புதிய __vpiVThrWord(base);

      திரும்பு obj;
}

#ifdef CHECK_WITH_VALGRIND
நிலையான map<vpiHandle, தருக்க> handle_map;

வெறுமை thread_word_delete(vpiHandle item)
{
      handle_map[item] = பொய்;
}

நிலையான வெறுமை thread_word_delete_real(vpiHandle item)
{
      வகுப்பு __vpiVThrWord*obj = dynamic_cast<__vpiVThrWord*>(item);
      delete obj;
}
#endif

வகுப்பு __vpiVThrStrStack : பொது __vpiHandle {
    பொது:
      explicit __vpiVThrStrStack(unsigned depth);
      முழு get_type_code(வெறுமை) மாறிலி override;
      முழு vpi_get(முழு code) override;
      வெறுமை vpi_get_மதிப்பு(p_vpi_மதிப்பு val) override;
    தனிப்பட்ட:
      unsigned depth_;
};

__vpiVThrStrStack::__vpiVThrStrStack(unsigned d)
: depth_(d)
{
}

முழு __vpiVThrStrStack::get_type_code(வெறுமை) மாறிலி
{ திரும்பு vpiConstant; }

முழு __vpiVThrStrStack::vpi_get(முழு code)
{
      தேர்வு (code) {
	  நிலை vpiConstType:
	    திரும்பு vpiStringConst;
#எனில் defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  நிலை _vpiFromThr:
	    திரும்பு _vpiString;
#endif
	  நிலை vpiSize:
	    திரும்பு vthread_get_str_அடுக்கு(vpip_current_vthread, depth_).size();
	  இயல்பு:
	    திரும்பு 0;
      }
}

வெறுமை __vpiVThrStrStack::vpi_get_மதிப்பு(p_vpi_மதிப்பு vp)
{
      string val;
      எழுத்து*rbuf = 0;

      எனில் (vpip_current_vthread)
	    val = vthread_get_str_அடுக்கு(vpip_current_vthread, depth_);

      தேர்வு (vp->format) {

	  நிலை vpiObjTypeVal:
	    vp->format = vpiStringVal;
	    // fallthrough
	  நிலை vpiStringVal:
	    rbuf = static_cast<எழுத்து *>(need_result_buf(val.size()+1, RBUF_VAL));
	    strcpy(rbuf, val.c_str());
	    vp->value.str = rbuf;
	    நிறுத்து;

	  இயல்பு:
	    fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiConstant (String)\n", (முழு)vp->format);

	    vp->format = vpiSuppressVal;
	    நிறுத்து;
      }
}

வகுப்பு __vpiVThrVec4Stack : பொது __vpiHandle {
    பொது:
      __vpiVThrVec4Stack(unsigned depth, தருக்க signed_flag, unsigned wid);
      முழு get_type_code(வெறுமை) மாறிலி override;
      முழு vpi_get(முழு code) override;
      எழுத்து*vpi_get_str(முழு code) override;
      வெறுமை vpi_get_மதிப்பு(p_vpi_மதிப்பு val) override;
      vpiHandle vpi_put_மதிப்பு(p_vpi_மதிப்பு val, முழு flags) override;
    தனிப்பட்ட:
      நிலையான வெறுமை vpi_get_மதிப்பு_string(p_vpi_மதிப்பு vp, மாறிலி vvp_vector4_t&val);
      நிலையான வெறுமை vpi_get_மதிப்பு_binstr(p_vpi_மதிப்பு vp, மாறிலி vvp_vector4_t&val);
      வெறுமை vpi_get_மதிப்பு_decstr(p_vpi_மதிப்பு vp, மாறிலி vvp_vector4_t&val);
      வெறுமை vpi_get_மதிப்பு_int   (p_vpi_மதிப்பு vp, மாறிலி vvp_vector4_t&val);
      நிலையான வெறுமை vpi_get_மதிப்பு_real  (p_vpi_மதிப்பு vp, மாறிலி vvp_vector4_t&val);
      நிலையான வெறுமை vpi_get_மதிப்பு_strength(p_vpi_மதிப்பு vp, மாறிலி vvp_vector4_t&val);
      நிலையான வெறுமை vpi_get_மதிப்பு_octstr(p_vpi_மதிப்பு vp, மாறிலி vvp_vector4_t&val);
      நிலையான வெறுமை vpi_get_மதிப்பு_hexstr(p_vpi_மதிப்பு vp, மாறிலி vvp_vector4_t&val);
      நிலையான வெறுமை vpi_get_மதிப்பு_vector(p_vpi_மதிப்பு vp, மாறிலி vvp_vector4_t&val);
    தனிப்பட்ட:
      unsigned depth_;
      தருக்க signed_flag_;
      unsigned expect_width_;
      மாறிலி எழுத்து*name;
};

__vpiVThrVec4Stack::__vpiVThrVec4Stack(unsigned d, தருக்க sf, unsigned wid)
: depth_(d), signed_flag_(sf), expect_width_(wid)
{
      name = vpip_name_string("S<,vec4,>");
}

முழு __vpiVThrVec4Stack::get_type_code(வெறுமை) மாறிலி
{ திரும்பு vpiConstant; }


முழு __vpiVThrVec4Stack::vpi_get(முழு code)
{
      தேர்வு (code) {
	  நிலை vpiSize:
	    திரும்பு expect_width_;

	  நிலை vpiSigned:
	    திரும்பு signed_flag_? 1 : 0;

	  நிலை vpiConstType:
	    திரும்பு vpi_இருமம்_Const;

#எனில் defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  நிலை _vpiFromThr:
	    திரும்பு _vpiVThr;
#endif

	  இயல்பு:
	    திரும்பு 0;
      }
}

எழுத்து*__vpiVThrVec4Stack::vpi_get_str(முழு code)
{
      தேர்வு (code) {
	  நிலை vpiFullName:
	    திரும்பு simple_set_rbuf_str(name);

	  இயல்பு:
	    திரும்பு 0;
      }
}

வெறுமை __vpiVThrVec4Stack::vpi_get_மதிப்பு(p_vpi_மதிப்பு vp)
{
      vvp_vector4_t val;

      எனில் (vpip_current_vthread)
	    val = vthread_get_vec4_அடுக்கு(vpip_current_vthread, depth_);

      தேர்வு (vp->format) {

	  நிலை vpiBinStrVal:
	    vpi_get_மதிப்பு_binstr(vp, val);
	    நிறுத்து;
	  நிலை vpiDecStrVal:
	    vpi_get_மதிப்பு_decstr(vp, val);
	    நிறுத்து;
	  நிலை vpiOctStrVal:
	    vpi_get_மதிப்பு_octstr(vp, val);
	    நிறுத்து;
	  நிலை vpiHexStrVal:
	    vpi_get_மதிப்பு_hexstr(vp, val);
	    நிறுத்து;
	  நிலை vpiIntVal:
	    vpi_get_மதிப்பு_int(vp, val);
	    நிறுத்து;
	  நிலை vpiRealVal:
	    vpi_get_மதிப்பு_real(vp, val);
	    நிறுத்து;
	  நிலை vpiStringVal:
	    vpi_get_மதிப்பு_string(vp, val);
	    நிறுத்து;
	  நிலை vpiStrengthVal:
	    vpi_get_மதிப்பு_strength(vp, val);
	    நிறுத்து;
	  நிலை vpiObjTypeVal:
	    vp->format = vpiVectorVal;
	    // fallthrough
	  நிலை vpiVectorVal:
	    vpi_get_மதிப்பு_vector(vp, val);
	    நிறுத்து;

	  இயல்பு:
	    fprintf(stderr, "internal error: vpi_get_value(<format=%d>)"
		    " not implemented for __vpiVThrVec4Stack.\n", vp->format);
	    ப்ரதிஜ்ஞா(0);
      }

}

வெறுமை __vpiVThrVec4Stack::vpi_get_மதிப்பு_binstr(p_vpi_மதிப்பு vp, மாறிலி vvp_vector4_t&val)
{
      unsigned wid = val.size();
      எழுத்து*rbuf = static_cast<எழுத்து*>(need_result_buf(wid+1, RBUF_VAL));
      க்கு (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    rbuf[wid-idx-1] = vvp_bit4_to_ascii(val.value(idx));
      }
      rbuf[wid] = 0;
      vp->value.str = rbuf;
}

வெறுமை __vpiVThrVec4Stack::vpi_get_மதிப்பு_decstr(p_vpi_மதிப்பு vp, மாறிலி vvp_vector4_t&val)
{
      unsigned wid = val.size();
      முழு nbuf = (wid+2)/3 + 1;
      எழுத்து *rbuf = static_cast<எழுத்து*>(need_result_buf(nbuf, RBUF_VAL));

      vpip_vec4_to_dec_str(val, rbuf, nbuf, signed_flag_);
      vp->value.str = rbuf;
}

வெறுமை __vpiVThrVec4Stack::vpi_get_மதிப்பு_octstr(p_vpi_மதிப்பு vp, மாறிலி vvp_vector4_t&val)
{
      unsigned wid = val.size();
      unsigned owid = (wid + 2) / 3;
      எழுத்து*rbuf = static_cast<எழுத்து*>(need_result_buf(owid+1, RBUF_VAL));
      rbuf[owid] = 0;

      unsigned oval = 0;
      க்கு (unsigned idx = 0; idx < wid ; idx += 1) {
	    unsigned tmp = 0;
	    தேர்வு (val.value(idx)) {
		நிலை BIT4_0:
		  tmp = 0;
		  நிறுத்து;
		நிலை BIT4_1:
		  tmp = 1;
		  நிறுத்து;
		நிலை BIT4_X:
		  tmp = 2;
		  நிறுத்து;
		நிலை BIT4_Z:
		  tmp = 3;
		  நிறுத்து;
	    }

	    oval = oval | (tmp << 2*(idx%3));

	    எனில் (idx%3 == 2) {
		  owid -= 1;
		  rbuf[owid] = oct_digits[oval];
		  oval = 0;
	    }
      }

      எனில் (owid > 0) {
	    owid -= 1;
	    rbuf[owid] = oct_digits[oval];
      }
      vp->value.str = rbuf;
}

வெறுமை __vpiVThrVec4Stack::vpi_get_மதிப்பு_hexstr(p_vpi_மதிப்பு vp, மாறிலி vvp_vector4_t&val)
{
      unsigned wid = val.size();
      unsigned hwid = (wid + 3) /4;
      எழுத்து*rbuf = static_cast<எழுத்து*>(need_result_buf(hwid+1, RBUF_VAL));
      rbuf[hwid] = 0;

      unsigned hval = 0;
      க்கு (unsigned idx = 0; idx < wid ; idx += 1) {
	    unsigned tmp = 0;
	    தேர்வு (val.value(idx)) {
		நிலை BIT4_0:
		  tmp = 0;
		  நிறுத்து;
		நிலை BIT4_1:
		  tmp = 1;
		  நிறுத்து;
		நிலை BIT4_X:
		  tmp = 2;
		  நிறுத்து;
		நிலை BIT4_Z:
		  tmp = 3;
		  நிறுத்து;
	    }
	    hval = hval | (tmp << 2*(idx%4));

	    எனில் (idx%4 == 3) {
		  hwid -= 1;
		  rbuf[hwid] = hex_digits[hval];
		  hval = 0;
	    }
      }

      எனில் (hwid > 0) {
	    hwid -= 1;
	    rbuf[hwid] = hex_digits[hval];
      }
      vp->value.str = rbuf;
}

வெறுமை __vpiVThrVec4Stack::vpi_get_மதிப்பு_int(p_vpi_மதிப்பு vp, மாறிலி vvp_vector4_t&val)
{
      int32_t vali = 0;
      vector4_to_மதிப்பு(val, vali, signed_flag_, பொய்);
      vp->value.integer = vali;
}

வெறுமை __vpiVThrVec4Stack::vpi_get_மதிப்பு_real(p_vpi_மதிப்பு vp, மாறிலி vvp_vector4_t&val)
{
      unsigned wid = val.size();
      vp->value.real = 0.0;

      க்கு (unsigned idx = wid ; idx > 0 ; idx -= 1) {
	    vp->value.real *= 2.0;
	    எனில் (val.value(idx-1) == BIT4_1)
		  vp->value.real += 1.0;
      }
}

வெறுமை __vpiVThrVec4Stack::vpi_get_மதிப்பு_string(p_vpi_மதிப்பு vp, மாறிலி vvp_vector4_t&val)
{
      எழுத்து*rbuf = static_cast<எழுத்து*>(need_result_buf((val.size() / 8) + 1, RBUF_VAL));
      எழுத்து*cp = rbuf;

      எழுத்து tmp = 0;
      க்கு (முழு bitnr = val.size()-1 ; bitnr >= 0 ; bitnr -= 1) {
	    tmp <<= 1;
	    தேர்வு (val.value(bitnr)) {
		நிலை BIT4_1:
		  tmp |= 1;
		  நிறுத்து;
		நிலை BIT4_0:
		இயல்பு:
		  நிறுத்து;
	    }

	    எனில் ((bitnr&7)==0) {
		    // Don't include leading nuls
		  எனில் (tmp == 0 && cp == rbuf)
			தொடர்;

		  *cp++ = tmp? tmp : ' ';
		  tmp = 0;
	    }
      }
      *cp++ = 0;
      vp->format = vpiStringVal;
      vp->value.str = rbuf;
}

வெறுமை __vpiVThrVec4Stack::vpi_get_மதிப்பு_vector(p_vpi_மதிப்பு vp, மாறிலி vvp_vector4_t&val)
{
      unsigned wid = val.size();

      unsigned hwid = (wid+31)/32;
      vp->value.vector = static_cast<s_vpi_vecval*>
                         (need_result_buf(hwid*அளவு(s_vpi_vecval), RBUF_VAL));
      ப்ரதிஜ்ஞா(vp->value.vector);

	/* Zero the top of the result buffer first since the fill loop below only writes
	   bits 0..wid-1, which can leave stale data in the unused high bits. */
      எனில் (hwid == 0) hwid = 1;
      vp->value.vector[hwid-1].aval = 0;
      vp->value.vector[hwid-1].bval = 0;

      க்கு (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
	    முழு word = idx/32;
	    PLI_INT32 mask = 1 << (idx%32);

	    தேர்வு (val.value(idx)) {
		நிலை BIT4_0:
		  vp->value.vector[word].aval &= ~mask;
		  vp->value.vector[word].bval &= ~mask;
		  நிறுத்து;
		நிலை BIT4_1:
		  vp->value.vector[word].aval |=  mask;
		  vp->value.vector[word].bval &= ~mask;
		  நிறுத்து;
		நிலை BIT4_X:
		  vp->value.vector[word].aval |=  mask;
		  vp->value.vector[word].bval |=  mask;
		  நிறுத்து;
		நிலை BIT4_Z:
		  vp->value.vector[word].aval &= ~mask;
		  vp->value.vector[word].bval |=  mask;
		  நிறுத்து;
	    }
      }
}

வெறுமை __vpiVThrVec4Stack::vpi_get_மதிப்பு_strength(p_vpi_மதிப்பு vp, மாறிலி vvp_vector4_t&val)
{
      s_vpi_strengthval*op = static_cast<s_vpi_strengthval*>
                             (need_result_buf(val.size() * அளவு(s_vpi_strengthval),
                                              RBUF_VAL));

      க்கு (unsigned idx = 0 ; idx < val.size() ; idx += 1) {
	    தேர்வு (val.value(idx)) {
		நிலை BIT4_0:
		  op[idx].logic = vpi0;
		  op[idx].s0 = vpiStrongDrive;
		  op[idx].s1 = 0;
		  நிறுத்து;
		நிலை BIT4_1:
		  op[idx].logic = vpi1;
		  op[idx].s0 = 0;
		  op[idx].s1 = vpiStrongDrive;
		  நிறுத்து;
		நிலை BIT4_X:
		  op[idx].logic = vpiX;
		  op[idx].s0 = vpiStrongDrive;
		  op[idx].s1 = vpiStrongDrive;
		  நிறுத்து;
		நிலை BIT4_Z:
		  op[idx].logic = vpiZ;
		  op[idx].s0 = vpiHiZ;
		  op[idx].s1 = vpiHiZ;
		  நிறுத்து;
	    }
      }

      vp->format = vpiStrengthVal;
      vp->value.strength = op;
}

vpiHandle __vpiVThrVec4Stack::vpi_put_மதிப்பு(p_vpi_மதிப்பு vp, முழு /*flags*/)
{
      ப்ரதிஜ்ஞா(vpip_current_vthread);

      தேர்வு (vp->format) {

	  இயல்பு:
	    fprintf(stderr, "internal error: vpi_put_value(<format=%d>)"
		    " not implemented for __vpiVThrVec4Stack.\n", vp->format);
	    ப்ரதிஜ்ஞா(0);
	    திரும்பு 0;
      }
}

#ifdef CHECK_WITH_VALGRIND
வெறுமை thread_vthr_delete(vpiHandle item)
{
      handle_map[item] = உண்மை;
}

நிலையான வெறுமை thread_vthr_delete_real(vpiHandle item)
{
      __vpiVThrVec4Stack*obj = dynamic_cast<__vpiVThrVec4Stack*>(item);
      delete obj;
}

வெறுமை vpi_handle_delete()
{
      map<vpiHandle, தருக்க>::iterator iter;
      க்கு (iter = handle_map.begin(); iter != handle_map.end(); ++ iter ) {
	    எனில் (iter->second) thread_vthr_delete_real(iter->first);
	    இல்லையெனில் thread_word_delete_real(iter->first);
      }
}
#endif


vpiHandle vpip_make_vthr_str_அடுக்கு(unsigned depth)
{
      __vpiVThrStrStack*obj = புதிய __vpiVThrStrStack(depth);
      திரும்பு obj;
}

vpiHandle vpip_make_vthr_vec4_அடுக்கு(unsigned depth, தருக்க signed_flag, unsigned wid)
{
      __vpiVThrVec4Stack*obj = புதிய __vpiVThrVec4Stack(depth, signed_flag, wid);
      திரும்பு obj;
}

#ifdef CHECK_WITH_VALGRIND
நிலையான map<vpiHandle, தருக்க> அடுக்கு_map;

வெறுமை thread_string_delete(vpiHandle item)
{
      அடுக்கு_map[item] = பொய்;
}

நிலையான வெறுமை thread_string_delete_real(vpiHandle item)
{
      __vpiVThrStrStack*obj = dynamic_cast<__vpiVThrStrStack*>(item);
      delete obj;
}

வெறுமை vpi_அடுக்கு_delete()
{
      map<vpiHandle, தருக்க>::iterator iter;
      க்கு (iter = அடுக்கு_map.begin(); iter != அடுக்கு_map.end(); ++ iter ) {
	    thread_string_delete_real(iter->first);
      }
}
#endif
