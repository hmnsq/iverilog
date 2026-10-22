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

using ನಾಮಾಕಾಶ std;

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
ಬಾಹ್ಯ ಧ್ರುವ ವರ್ಣ hex_digits[256];
ಬಾಹ್ಯ ಧ್ರುವ ವರ್ಣ oct_digits[64];


ವರ್ಗ __vpiVThrWord : ಸಾರ್ವಜನಿಕ __vpiHandle {
    ಸಾರ್ವಜನಿಕ:
      explicit __vpiVThrWord(unsigned base);
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override;
      ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;

      ಪೂರ್ಣಾಙ್ಕ get_subtype() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ subtype; };
      unsigned get_index() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ index; };

    ಗುಪ್ತ:
      ಧ್ರುವ ವರ್ಣ* name;
      ಪೂರ್ಣಾಙ್ಕ subtype;
      unsigned index;
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ vthr_word_get(ಪೂರ್ಣಾಙ್ಕ code, vpiHandle ref)
{
      ಧ್ರುವ __vpiVThrWord*rfp = dynamic_cast<__vpiVThrWord*>(ref);

      ವಿಕಲ್ಪ (code) {

	  ಪಕ್ಷ vpiConstType:
	    ಪ್ರತ್ಯಾಗಮ rfp->get_subtype();

#ಯದಿ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  ಪಕ್ಷ _vpiFromThr:
	    ಪ್ರತ್ಯಾಗಮ _vpiWord;
#endif

	  ಸ್ವಭಾವ:
	    ಪ್ರತ್ಯಾಗಮ 0;
      }
}

ಸ್ಥಿರ ಶೂನ್ಯ vthr_real_get_ಮೌಲ್ಯ(vpiHandle ref, s_vpi_ಮೌಲ್ಯ*vp)
{
      ಧ್ರುವ __vpiVThrWord*obj = dynamic_cast<__vpiVThrWord*>(ref);
      ಸ್ಥಿರ ಧ್ರುವ size_t RBUF_USE_SIZE = 66;
      ವರ್ಣ *rbuf = static_cast<ವರ್ಣ *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));

      ದ್ವಿಗುಣ val = 0.0;

	/* Get the actual value from the index. It is possible, by the
	   way, that the vpi_get_value is called from compiletf. If
	   that's the case, there will be no current thread, and this
	   will not have access to the proper value. Punt and return a
	   0.0 value instead. */
      ಯದಿ (vpip_current_vthread)
	    val = vthread_get_real_ರಾಶಿ(vpip_current_vthread,
	                                 obj->get_index());

      ವಿಕಲ್ಪ (vp->format) {

	  ಪಕ್ಷ vpiObjTypeVal:
	    vp->format = vpiRealVal;
	    // fallthrough
	  ಪಕ್ಷ vpiRealVal:
	    vp->value.real = val;
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiIntVal:
	      /* NaN or +/- infinity are translated as 0. */
	    ಯದಿ (val != val || (val && (val == 0.5*val))) {
		  val = 0.0;
	    } ಅನ್ಯಥಾ {
		  val = std::round(val);
	    }
	    vp->value.integer = (PLI_INT32)val;
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiDecStrVal:
	    ಯದಿ (std::isnan(val))
		  snprintf(rbuf, RBUF_USE_SIZE, "%s", "nan");
	    ಅನ್ಯಥಾ
		  snprintf(rbuf, RBUF_USE_SIZE, "%0.0f", std::round(val));
	    vp->value.str = rbuf;
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiOctStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" PRIo64, vlg_round_to_u64(val));
	    vp->value.str = rbuf;
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiHexStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" PRIx64, vlg_round_to_u64(val));
	    vp->value.str = rbuf;
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiBinStrVal: {
		uint64_t vali = vlg_round_to_u64(val);
		unsigned len = 0;

		ಯಾವತ್ (vali > 0) {
		      len += 1;
		      vali /= 2;
		}

		vali = vlg_round_to_u64(val);
		ಪ್ರತಿ (unsigned idx = 0 ;  idx < len ;  idx += 1) {
		      rbuf[len-idx-1] = (vali & 1)? '1' : '0';
		      vali /= 2;
		}

		rbuf[len] = 0;
		ಯದಿ (len == 0) {
		      rbuf[0] = '0';
		      rbuf[1] = 0;
		}
		vp->value.str = rbuf;
		ಭಙ್ಗ;
	  }

	  ಸ್ವಭಾವ:
	    fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiConstant (Real)\n", (ಪೂರ್ಣಾಙ್ಕ)vp->format);

	    vp->format = vpiSuppressVal;
	    ಭಙ್ಗ;
      }
}

ಅನ್ತಃಸ್ಥ __vpiVThrWord::__vpiVThrWord(unsigned base)
{
      name = vpip_name_string("W<>");
      subtype = vpiRealConst;
      index = base;
}

ಪೂರ್ಣಾಙ್ಕ __vpiVThrWord::get_type_code(ಶೂನ್ಯ) ಧ್ರುವ
{ ಪ್ರತ್ಯಾಗಮ vpiConstant; }

ಪೂರ್ಣಾಙ್ಕ __vpiVThrWord::vpi_get(ಪೂರ್ಣಾಙ್ಕ code)
{ ಪ್ರತ್ಯಾಗಮ vthr_word_get(code, ಏತತ್); }

ಶೂನ್ಯ __vpiVThrWord::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val)
{ vthr_real_get_ಮೌಲ್ಯ(ಏತತ್, val); }

vpiHandle vpip_make_vthr_word(unsigned base, ಧ್ರುವ ವರ್ಣ*type)
{
      ಪ್ರತಿಜ್ಞಾ(type[0] == 'r');
      ಪ್ರತಿಜ್ಞಾ(base < 65536);
      __vpiVThrWord*obj = ನೂತನ __vpiVThrWord(base);

      ಪ್ರತ್ಯಾಗಮ obj;
}

#ifdef CHECK_WITH_VALGRIND
ಸ್ಥಿರ map<vpiHandle, ತಾರ್ಕಿಕ> handle_map;

ಶೂನ್ಯ thread_word_delete(vpiHandle item)
{
      handle_map[item] = ಅಸತ್ಯ;
}

ಸ್ಥಿರ ಶೂನ್ಯ thread_word_delete_real(vpiHandle item)
{
      ವರ್ಗ __vpiVThrWord*obj = dynamic_cast<__vpiVThrWord*>(item);
      delete obj;
}
#endif

ವರ್ಗ __vpiVThrStrStack : ಸಾರ್ವಜನಿಕ __vpiHandle {
    ಸಾರ್ವಜನಿಕ:
      explicit __vpiVThrStrStack(unsigned depth);
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override;
      ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;
    ಗುಪ್ತ:
      unsigned depth_;
};

__vpiVThrStrStack::__vpiVThrStrStack(unsigned d)
: depth_(d)
{
}

ಪೂರ್ಣಾಙ್ಕ __vpiVThrStrStack::get_type_code(ಶೂನ್ಯ) ಧ್ರುವ
{ ಪ್ರತ್ಯಾಗಮ vpiConstant; }

ಪೂರ್ಣಾಙ್ಕ __vpiVThrStrStack::vpi_get(ಪೂರ್ಣಾಙ್ಕ code)
{
      ವಿಕಲ್ಪ (code) {
	  ಪಕ್ಷ vpiConstType:
	    ಪ್ರತ್ಯಾಗಮ vpiStringConst;
#ಯದಿ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  ಪಕ್ಷ _vpiFromThr:
	    ಪ್ರತ್ಯಾಗಮ _vpiString;
#endif
	  ಪಕ್ಷ vpiSize:
	    ಪ್ರತ್ಯಾಗಮ vthread_get_str_ರಾಶಿ(vpip_current_vthread, depth_).size();
	  ಸ್ವಭಾವ:
	    ಪ್ರತ್ಯಾಗಮ 0;
      }
}

ಶೂನ್ಯ __vpiVThrStrStack::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ vp)
{
      string val;
      ವರ್ಣ*rbuf = 0;

      ಯದಿ (vpip_current_vthread)
	    val = vthread_get_str_ರಾಶಿ(vpip_current_vthread, depth_);

      ವಿಕಲ್ಪ (vp->format) {

	  ಪಕ್ಷ vpiObjTypeVal:
	    vp->format = vpiStringVal;
	    // fallthrough
	  ಪಕ್ಷ vpiStringVal:
	    rbuf = static_cast<ವರ್ಣ *>(need_result_buf(val.size()+1, RBUF_VAL));
	    strcpy(rbuf, val.c_str());
	    vp->value.str = rbuf;
	    ಭಙ್ಗ;

	  ಸ್ವಭಾವ:
	    fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiConstant (String)\n", (ಪೂರ್ಣಾಙ್ಕ)vp->format);

	    vp->format = vpiSuppressVal;
	    ಭಙ್ಗ;
      }
}

ವರ್ಗ __vpiVThrVec4Stack : ಸಾರ್ವಜನಿಕ __vpiHandle {
    ಸಾರ್ವಜನಿಕ:
      __vpiVThrVec4Stack(unsigned depth, ತಾರ್ಕಿಕ signed_flag, unsigned wid);
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override;
      ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code) override;
      ವರ್ಣ*vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;
      vpiHandle vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಙ್ಕ flags) override;
    ಗುಪ್ತ:
      ಸ್ಥಿರ ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ_string(p_vpi_ಮೌಲ್ಯ vp, ಧ್ರುವ vvp_vector4_t&val);
      ಸ್ಥಿರ ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ_binstr(p_vpi_ಮೌಲ್ಯ vp, ಧ್ರುವ vvp_vector4_t&val);
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ_decstr(p_vpi_ಮೌಲ್ಯ vp, ಧ್ರುವ vvp_vector4_t&val);
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ_int   (p_vpi_ಮೌಲ್ಯ vp, ಧ್ರುವ vvp_vector4_t&val);
      ಸ್ಥಿರ ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ_real  (p_vpi_ಮೌಲ್ಯ vp, ಧ್ರುವ vvp_vector4_t&val);
      ಸ್ಥಿರ ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ_strength(p_vpi_ಮೌಲ್ಯ vp, ಧ್ರುವ vvp_vector4_t&val);
      ಸ್ಥಿರ ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ_octstr(p_vpi_ಮೌಲ್ಯ vp, ಧ್ರುವ vvp_vector4_t&val);
      ಸ್ಥಿರ ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ_hexstr(p_vpi_ಮೌಲ್ಯ vp, ಧ್ರುವ vvp_vector4_t&val);
      ಸ್ಥಿರ ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ_vector(p_vpi_ಮೌಲ್ಯ vp, ಧ್ರುವ vvp_vector4_t&val);
    ಗುಪ್ತ:
      unsigned depth_;
      ತಾರ್ಕಿಕ signed_flag_;
      unsigned expect_width_;
      ಧ್ರುವ ವರ್ಣ*name;
};

__vpiVThrVec4Stack::__vpiVThrVec4Stack(unsigned d, ತಾರ್ಕಿಕ sf, unsigned wid)
: depth_(d), signed_flag_(sf), expect_width_(wid)
{
      name = vpip_name_string("S<,vec4,>");
}

ಪೂರ್ಣಾಙ್ಕ __vpiVThrVec4Stack::get_type_code(ಶೂನ್ಯ) ಧ್ರುವ
{ ಪ್ರತ್ಯಾಗಮ vpiConstant; }


ಪೂರ್ಣಾಙ್ಕ __vpiVThrVec4Stack::vpi_get(ಪೂರ್ಣಾಙ್ಕ code)
{
      ವಿಕಲ್ಪ (code) {
	  ಪಕ್ಷ vpiSize:
	    ಪ್ರತ್ಯಾಗಮ expect_width_;

	  ಪಕ್ಷ vpiSigned:
	    ಪ್ರತ್ಯಾಗಮ signed_flag_? 1 : 0;

	  ಪಕ್ಷ vpiConstType:
	    ಪ್ರತ್ಯಾಗಮ vpi_ದ್ವಿಮಾನ_Const;

#ಯದಿ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  ಪಕ್ಷ _vpiFromThr:
	    ಪ್ರತ್ಯಾಗಮ _vpiVThr;
#endif

	  ಸ್ವಭಾವ:
	    ಪ್ರತ್ಯಾಗಮ 0;
      }
}

ವರ್ಣ*__vpiVThrVec4Stack::vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code)
{
      ವಿಕಲ್ಪ (code) {
	  ಪಕ್ಷ vpiFullName:
	    ಪ್ರತ್ಯಾಗಮ simple_set_rbuf_str(name);

	  ಸ್ವಭಾವ:
	    ಪ್ರತ್ಯಾಗಮ 0;
      }
}

ಶೂನ್ಯ __vpiVThrVec4Stack::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ vp)
{
      vvp_vector4_t val;

      ಯದಿ (vpip_current_vthread)
	    val = vthread_get_vec4_ರಾಶಿ(vpip_current_vthread, depth_);

      ವಿಕಲ್ಪ (vp->format) {

	  ಪಕ್ಷ vpiBinStrVal:
	    vpi_get_ಮೌಲ್ಯ_binstr(vp, val);
	    ಭಙ್ಗ;
	  ಪಕ್ಷ vpiDecStrVal:
	    vpi_get_ಮೌಲ್ಯ_decstr(vp, val);
	    ಭಙ್ಗ;
	  ಪಕ್ಷ vpiOctStrVal:
	    vpi_get_ಮೌಲ್ಯ_octstr(vp, val);
	    ಭಙ್ಗ;
	  ಪಕ್ಷ vpiHexStrVal:
	    vpi_get_ಮೌಲ್ಯ_hexstr(vp, val);
	    ಭಙ್ಗ;
	  ಪಕ್ಷ vpiIntVal:
	    vpi_get_ಮೌಲ್ಯ_int(vp, val);
	    ಭಙ್ಗ;
	  ಪಕ್ಷ vpiRealVal:
	    vpi_get_ಮೌಲ್ಯ_real(vp, val);
	    ಭಙ್ಗ;
	  ಪಕ್ಷ vpiStringVal:
	    vpi_get_ಮೌಲ್ಯ_string(vp, val);
	    ಭಙ್ಗ;
	  ಪಕ್ಷ vpiStrengthVal:
	    vpi_get_ಮೌಲ್ಯ_strength(vp, val);
	    ಭಙ್ಗ;
	  ಪಕ್ಷ vpiObjTypeVal:
	    vp->format = vpiVectorVal;
	    // fallthrough
	  ಪಕ್ಷ vpiVectorVal:
	    vpi_get_ಮೌಲ್ಯ_vector(vp, val);
	    ಭಙ್ಗ;

	  ಸ್ವಭಾವ:
	    fprintf(stderr, "internal error: vpi_get_value(<format=%d>)"
		    " not implemented for __vpiVThrVec4Stack.\n", vp->format);
	    ಪ್ರತಿಜ್ಞಾ(0);
      }

}

ಶೂನ್ಯ __vpiVThrVec4Stack::vpi_get_ಮೌಲ್ಯ_binstr(p_vpi_ಮೌಲ್ಯ vp, ಧ್ರುವ vvp_vector4_t&val)
{
      unsigned wid = val.size();
      ವರ್ಣ*rbuf = static_cast<ವರ್ಣ*>(need_result_buf(wid+1, RBUF_VAL));
      ಪ್ರತಿ (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    rbuf[wid-idx-1] = vvp_bit4_to_ascii(val.value(idx));
      }
      rbuf[wid] = 0;
      vp->value.str = rbuf;
}

ಶೂನ್ಯ __vpiVThrVec4Stack::vpi_get_ಮೌಲ್ಯ_decstr(p_vpi_ಮೌಲ್ಯ vp, ಧ್ರುವ vvp_vector4_t&val)
{
      unsigned wid = val.size();
      ಪೂರ್ಣಾಙ್ಕ nbuf = (wid+2)/3 + 1;
      ವರ್ಣ *rbuf = static_cast<ವರ್ಣ*>(need_result_buf(nbuf, RBUF_VAL));

      vpip_vec4_to_dec_str(val, rbuf, nbuf, signed_flag_);
      vp->value.str = rbuf;
}

ಶೂನ್ಯ __vpiVThrVec4Stack::vpi_get_ಮೌಲ್ಯ_octstr(p_vpi_ಮೌಲ್ಯ vp, ಧ್ರುವ vvp_vector4_t&val)
{
      unsigned wid = val.size();
      unsigned owid = (wid + 2) / 3;
      ವರ್ಣ*rbuf = static_cast<ವರ್ಣ*>(need_result_buf(owid+1, RBUF_VAL));
      rbuf[owid] = 0;

      unsigned oval = 0;
      ಪ್ರತಿ (unsigned idx = 0; idx < wid ; idx += 1) {
	    unsigned tmp = 0;
	    ವಿಕಲ್ಪ (val.value(idx)) {
		ಪಕ್ಷ BIT4_0:
		  tmp = 0;
		  ಭಙ್ಗ;
		ಪಕ್ಷ BIT4_1:
		  tmp = 1;
		  ಭಙ್ಗ;
		ಪಕ್ಷ BIT4_X:
		  tmp = 2;
		  ಭಙ್ಗ;
		ಪಕ್ಷ BIT4_Z:
		  tmp = 3;
		  ಭಙ್ಗ;
	    }

	    oval = oval | (tmp << 2*(idx%3));

	    ಯದಿ (idx%3 == 2) {
		  owid -= 1;
		  rbuf[owid] = oct_digits[oval];
		  oval = 0;
	    }
      }

      ಯದಿ (owid > 0) {
	    owid -= 1;
	    rbuf[owid] = oct_digits[oval];
      }
      vp->value.str = rbuf;
}

ಶೂನ್ಯ __vpiVThrVec4Stack::vpi_get_ಮೌಲ್ಯ_hexstr(p_vpi_ಮೌಲ್ಯ vp, ಧ್ರುವ vvp_vector4_t&val)
{
      unsigned wid = val.size();
      unsigned hwid = (wid + 3) /4;
      ವರ್ಣ*rbuf = static_cast<ವರ್ಣ*>(need_result_buf(hwid+1, RBUF_VAL));
      rbuf[hwid] = 0;

      unsigned hval = 0;
      ಪ್ರತಿ (unsigned idx = 0; idx < wid ; idx += 1) {
	    unsigned tmp = 0;
	    ವಿಕಲ್ಪ (val.value(idx)) {
		ಪಕ್ಷ BIT4_0:
		  tmp = 0;
		  ಭಙ್ಗ;
		ಪಕ್ಷ BIT4_1:
		  tmp = 1;
		  ಭಙ್ಗ;
		ಪಕ್ಷ BIT4_X:
		  tmp = 2;
		  ಭಙ್ಗ;
		ಪಕ್ಷ BIT4_Z:
		  tmp = 3;
		  ಭಙ್ಗ;
	    }
	    hval = hval | (tmp << 2*(idx%4));

	    ಯದಿ (idx%4 == 3) {
		  hwid -= 1;
		  rbuf[hwid] = hex_digits[hval];
		  hval = 0;
	    }
      }

      ಯದಿ (hwid > 0) {
	    hwid -= 1;
	    rbuf[hwid] = hex_digits[hval];
      }
      vp->value.str = rbuf;
}

ಶೂನ್ಯ __vpiVThrVec4Stack::vpi_get_ಮೌಲ್ಯ_int(p_vpi_ಮೌಲ್ಯ vp, ಧ್ರುವ vvp_vector4_t&val)
{
      int32_t vali = 0;
      vector4_to_ಮೌಲ್ಯ(val, vali, signed_flag_, ಅಸತ್ಯ);
      vp->value.integer = vali;
}

ಶೂನ್ಯ __vpiVThrVec4Stack::vpi_get_ಮೌಲ್ಯ_real(p_vpi_ಮೌಲ್ಯ vp, ಧ್ರುವ vvp_vector4_t&val)
{
      unsigned wid = val.size();
      vp->value.real = 0.0;

      ಪ್ರತಿ (unsigned idx = wid ; idx > 0 ; idx -= 1) {
	    vp->value.real *= 2.0;
	    ಯದಿ (val.value(idx-1) == BIT4_1)
		  vp->value.real += 1.0;
      }
}

ಶೂನ್ಯ __vpiVThrVec4Stack::vpi_get_ಮೌಲ್ಯ_string(p_vpi_ಮೌಲ್ಯ vp, ಧ್ರುವ vvp_vector4_t&val)
{
      ವರ್ಣ*rbuf = static_cast<ವರ್ಣ*>(need_result_buf((val.size() / 8) + 1, RBUF_VAL));
      ವರ್ಣ*cp = rbuf;

      ವರ್ಣ tmp = 0;
      ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ bitnr = val.size()-1 ; bitnr >= 0 ; bitnr -= 1) {
	    tmp <<= 1;
	    ವಿಕಲ್ಪ (val.value(bitnr)) {
		ಪಕ್ಷ BIT4_1:
		  tmp |= 1;
		  ಭಙ್ಗ;
		ಪಕ್ಷ BIT4_0:
		ಸ್ವಭಾವ:
		  ಭಙ್ಗ;
	    }

	    ಯದಿ ((bitnr&7)==0) {
		    // Don't include leading nuls
		  ಯದಿ (tmp == 0 && cp == rbuf)
			ಅನುವರ್ತ;

		  *cp++ = tmp? tmp : ' ';
		  tmp = 0;
	    }
      }
      *cp++ = 0;
      vp->format = vpiStringVal;
      vp->value.str = rbuf;
}

ಶೂನ್ಯ __vpiVThrVec4Stack::vpi_get_ಮೌಲ್ಯ_vector(p_vpi_ಮೌಲ್ಯ vp, ಧ್ರುವ vvp_vector4_t&val)
{
      unsigned wid = val.size();

      unsigned hwid = (wid+31)/32;
      vp->value.vector = static_cast<s_vpi_vecval*>
                         (need_result_buf(hwid*ಪರಿಮಾಣ(s_vpi_vecval), RBUF_VAL));
      ಪ್ರತಿಜ್ಞಾ(vp->value.vector);

	/* Zero the top of the result buffer first since the fill loop below only writes
	   bits 0..wid-1, which can leave stale data in the unused high bits. */
      ಯದಿ (hwid == 0) hwid = 1;
      vp->value.vector[hwid-1].aval = 0;
      vp->value.vector[hwid-1].bval = 0;

      ಪ್ರತಿ (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
	    ಪೂರ್ಣಾಙ್ಕ word = idx/32;
	    PLI_INT32 mask = 1 << (idx%32);

	    ವಿಕಲ್ಪ (val.value(idx)) {
		ಪಕ್ಷ BIT4_0:
		  vp->value.vector[word].aval &= ~mask;
		  vp->value.vector[word].bval &= ~mask;
		  ಭಙ್ಗ;
		ಪಕ್ಷ BIT4_1:
		  vp->value.vector[word].aval |=  mask;
		  vp->value.vector[word].bval &= ~mask;
		  ಭಙ್ಗ;
		ಪಕ್ಷ BIT4_X:
		  vp->value.vector[word].aval |=  mask;
		  vp->value.vector[word].bval |=  mask;
		  ಭಙ್ಗ;
		ಪಕ್ಷ BIT4_Z:
		  vp->value.vector[word].aval &= ~mask;
		  vp->value.vector[word].bval |=  mask;
		  ಭಙ್ಗ;
	    }
      }
}

ಶೂನ್ಯ __vpiVThrVec4Stack::vpi_get_ಮೌಲ್ಯ_strength(p_vpi_ಮೌಲ್ಯ vp, ಧ್ರುವ vvp_vector4_t&val)
{
      s_vpi_strengthval*op = static_cast<s_vpi_strengthval*>
                             (need_result_buf(val.size() * ಪರಿಮಾಣ(s_vpi_strengthval),
                                              RBUF_VAL));

      ಪ್ರತಿ (unsigned idx = 0 ; idx < val.size() ; idx += 1) {
	    ವಿಕಲ್ಪ (val.value(idx)) {
		ಪಕ್ಷ BIT4_0:
		  op[idx].logic = vpi0;
		  op[idx].s0 = vpiStrongDrive;
		  op[idx].s1 = 0;
		  ಭಙ್ಗ;
		ಪಕ್ಷ BIT4_1:
		  op[idx].logic = vpi1;
		  op[idx].s0 = 0;
		  op[idx].s1 = vpiStrongDrive;
		  ಭಙ್ಗ;
		ಪಕ್ಷ BIT4_X:
		  op[idx].logic = vpiX;
		  op[idx].s0 = vpiStrongDrive;
		  op[idx].s1 = vpiStrongDrive;
		  ಭಙ್ಗ;
		ಪಕ್ಷ BIT4_Z:
		  op[idx].logic = vpiZ;
		  op[idx].s0 = vpiHiZ;
		  op[idx].s1 = vpiHiZ;
		  ಭಙ್ಗ;
	    }
      }

      vp->format = vpiStrengthVal;
      vp->value.strength = op;
}

vpiHandle __vpiVThrVec4Stack::vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ vp, ಪೂರ್ಣಾಙ್ಕ /*flags*/)
{
      ಪ್ರತಿಜ್ಞಾ(vpip_current_vthread);

      ವಿಕಲ್ಪ (vp->format) {

	  ಸ್ವಭಾವ:
	    fprintf(stderr, "internal error: vpi_put_value(<format=%d>)"
		    " not implemented for __vpiVThrVec4Stack.\n", vp->format);
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಪ್ರತ್ಯಾಗಮ 0;
      }
}

#ifdef CHECK_WITH_VALGRIND
ಶೂನ್ಯ thread_vthr_delete(vpiHandle item)
{
      handle_map[item] = ಸತ್ಯ;
}

ಸ್ಥಿರ ಶೂನ್ಯ thread_vthr_delete_real(vpiHandle item)
{
      __vpiVThrVec4Stack*obj = dynamic_cast<__vpiVThrVec4Stack*>(item);
      delete obj;
}

ಶೂನ್ಯ vpi_handle_delete()
{
      map<vpiHandle, ತಾರ್ಕಿಕ>::iterator iter;
      ಪ್ರತಿ (iter = handle_map.begin(); iter != handle_map.end(); ++ iter ) {
	    ಯದಿ (iter->second) thread_vthr_delete_real(iter->first);
	    ಅನ್ಯಥಾ thread_word_delete_real(iter->first);
      }
}
#endif


vpiHandle vpip_make_vthr_str_ರಾಶಿ(unsigned depth)
{
      __vpiVThrStrStack*obj = ನೂತನ __vpiVThrStrStack(depth);
      ಪ್ರತ್ಯಾಗಮ obj;
}

vpiHandle vpip_make_vthr_vec4_ರಾಶಿ(unsigned depth, ತಾರ್ಕಿಕ signed_flag, unsigned wid)
{
      __vpiVThrVec4Stack*obj = ನೂತನ __vpiVThrVec4Stack(depth, signed_flag, wid);
      ಪ್ರತ್ಯಾಗಮ obj;
}

#ifdef CHECK_WITH_VALGRIND
ಸ್ಥಿರ map<vpiHandle, ತಾರ್ಕಿಕ> ರಾಶಿ_map;

ಶೂನ್ಯ thread_string_delete(vpiHandle item)
{
      ರಾಶಿ_map[item] = ಅಸತ್ಯ;
}

ಸ್ಥಿರ ಶೂನ್ಯ thread_string_delete_real(vpiHandle item)
{
      __vpiVThrStrStack*obj = dynamic_cast<__vpiVThrStrStack*>(item);
      delete obj;
}

ಶೂನ್ಯ vpi_ರಾಶಿ_delete()
{
      map<vpiHandle, ತಾರ್ಕಿಕ>::iterator iter;
      ಪ್ರತಿ (iter = ರಾಶಿ_map.begin(); iter != ರಾಶಿ_map.end(); ++ iter ) {
	    thread_string_delete_real(iter->first);
      }
}
#endif
