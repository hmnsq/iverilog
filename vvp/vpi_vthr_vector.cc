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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
# include <map>
#ಆದರೆ_ಅಂತ್ಯ
# include  <cstdio>
# include  <cstdlib>
# include  <cassert>
# include  "ivl_alloc.h"

ಬಳಸಿ ನಾಮಾಕಾಶ std;

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
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ hex_digits[256];
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ oct_digits[64];


ವರ್ಗ __vpiVThrWord : ಸಾರ್ವಜನಿಕ __vpiHandle {
    ಸಾರ್ವಜನಿಕ:
      ಸ್ಪಷ್ಟ __vpiVThrWord(ಚಿಹ್ನೆರಹಿತ ತಳ);
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override;
      ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ code) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;

      ಪೂರ್ಣಾಂಕ get_subtype() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ subtype; };
      ಚಿಹ್ನೆರಹಿತ get_ಸೂಚ್ಯಂಕ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ; };

    ಖಾಸಗಿ:
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* name;
      ಪೂರ್ಣಾಂಕ subtype;
      ಚಿಹ್ನೆರಹಿತ ಸೂಚ್ಯಂಕ;
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ vthr_ಶಬ್ದ_get(ಪೂರ್ಣಾಂಕ code, vpiHandle ref)
{
      ಸ್ಥಿರಾಂಕ __vpiVThrWord*rfp = dynamic_cast<__vpiVThrWord*>(ref);

      ಆಯ್ಕೆ (code) {

	  ಸ್ಥಿತಿ vpiConstType:
	    ಹಿಂತಿರುಗಿ rfp->get_subtype();

#ಆದರೆ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  ಸ್ಥಿತಿ _vpiFromThr:
	    ಹಿಂತಿರುಗಿ vpi_ಶಬ್ದ;
#ಆದರೆ_ಅಂತ್ಯ

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ 0;
      }
}

ಸ್ಥಿರ ಶೂನ್ಯ vthr_real_get_ಮೌಲ್ಯ(vpiHandle ref, s_vpi_ಮೌಲ್ಯ*vp)
{
      ಸ್ಥಿರಾಂಕ __vpiVThrWord*obj = dynamic_cast<__vpiVThrWord*>(ref);
      ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ size_t RBUF_USE_ಗಾತ್ರ = 66;
      ಅಕ್ಷರ *rbuf = static_cast<ಅಕ್ಷರ *>(need_ಫಲಿತಾಂಶ_buf(RBUF_USE_ಗಾತ್ರ, RBUF_VAL));

      ದ್ವಿಗುಣ val = 0.0;

	/* Get the actual value from the index. It is possible, by the
	   way, that the vpi_get_value is called from compiletf. If
	   that's the case, there will be no current thread, and this
	   will not have access to the proper value. Punt and return a
	   0.0 value instead. */
      ಆದರೆ (vpip_current_vthread)
	    val = vthread_get_real_ರಾಶಿ(vpip_current_vthread,
	                                 obj->get_ಸೂಚ್ಯಂಕ());

      ಆಯ್ಕೆ (vp->format) {

	  ಸ್ಥಿತಿ vpiObjTypeVal:
	    vp->format = vpiRealVal;
	    // fallthrough
	  ಸ್ಥಿತಿ vpiRealVal:
	    vp->value.real = val;
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiIntVal:
	      /* NaN or +/- infinity are translated as 0. */
	    ಆದರೆ (val != val || (val && (val == 0.5*val))) {
		  val = 0.0;
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  val = std::ಸುತ್ತು_2(val);
	    }
	    vp->value.integer = (PLI_INT32)val;
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiDecStrVal:
	    ಆದರೆ (std::isnan(val))
		  snprintf(rbuf, RBUF_USE_ಗಾತ್ರ, "%s", "nan");
	    ಇಲ್ಲದಿದ್ದರೆ
		  snprintf(rbuf, RBUF_USE_ಗಾತ್ರ, "%0.0f", std::ಸುತ್ತು_2(val));
	    vp->value.str = rbuf;
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiOctStrVal:
	    snprintf(rbuf, RBUF_USE_ಗಾತ್ರ, "%" PRIo64, vlg_ಸುತ್ತು_ಗಮ್ಯ_u64(val));
	    vp->value.str = rbuf;
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiHexStrVal:
	    snprintf(rbuf, RBUF_USE_ಗಾತ್ರ, "%" PRIx64, vlg_ಸುತ್ತು_ಗಮ್ಯ_u64(val));
	    vp->value.str = rbuf;
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiBinStrVal: {
		uint64_t vali = vlg_ಸುತ್ತು_ಗಮ್ಯ_u64(val);
		ಚಿಹ್ನೆರಹಿತ len = 0;

		ಆಗ (vali > 0) {
		      len += 1;
		      vali /= 2;
		}

		vali = vlg_ಸುತ್ತು_ಗಮ್ಯ_u64(val);
		ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < len ;  idx += 1) {
		      rbuf[len-idx-1] = (vali & 1)? '1' : '0';
		      vali /= 2;
		}

		rbuf[len] = 0;
		ಆದರೆ (len == 0) {
		      rbuf[0] = '0';
		      rbuf[1] = 0;
		}
		vp->value.str = rbuf;
		ನಿಲ್ಲಿಸು;
	  }

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiConstant (Real)\n", (ಪೂರ್ಣಾಂಕ)vp->format);

	    vp->format = vpiSuppressVal;
	    ನಿಲ್ಲಿಸು;
      }
}

ಸಾಲೊಳಗೆ __vpiVThrWord::__vpiVThrWord(ಚಿಹ್ನೆರಹಿತ ತಳ)
{
      name = vpip_name_string("W<>");
      subtype = vpiRealConst;
      ಸೂಚ್ಯಂಕ = ತಳ;
}

ಪೂರ್ಣಾಂಕ __vpiVThrWord::get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ
{ ಹಿಂತಿರುಗಿ vpiConstant; }

ಪೂರ್ಣಾಂಕ __vpiVThrWord::vpi_get(ಪೂರ್ಣಾಂಕ code)
{ ಹಿಂತಿರುಗಿ vthr_ಶಬ್ದ_get(code, ಇದು); }

ಶೂನ್ಯ __vpiVThrWord::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val)
{ vthr_real_get_ಮೌಲ್ಯ(ಇದು, val); }

vpiHandle vpip_make_vthr_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ ತಳ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*type)
{
      ಪ್ರತಿಜ್ಞಾ(type[0] == 'r');
      ಪ್ರತಿಜ್ಞಾ(ತಳ < 65536);
      __vpiVThrWord*obj = ಹೊಸ __vpiVThrWord(ತಳ);

      ಹಿಂತಿರುಗಿ obj;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಸ್ಥಿರ map<vpiHandle, ತಾರ್ಕಿಕ> handle_map;

ಶೂನ್ಯ thread_ಶಬ್ದ_delete(vpiHandle ಅಂಶ)
{
      handle_map[ಅಂಶ] = ಸುಳ್ಳು;
}

ಸ್ಥಿರ ಶೂನ್ಯ thread_ಶಬ್ದ_delete_real(vpiHandle ಅಂಶ)
{
      ವರ್ಗ __vpiVThrWord*obj = dynamic_cast<__vpiVThrWord*>(ಅಂಶ);
      ಅಳಿಸು obj;
}
#ಆದರೆ_ಅಂತ್ಯ

ವರ್ಗ __vpiVThrStrStack : ಸಾರ್ವಜನಿಕ __vpiHandle {
    ಸಾರ್ವಜನಿಕ:
      ಸ್ಪಷ್ಟ __vpiVThrStrStack(ಚಿಹ್ನೆರಹಿತ ಆಳ);
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override;
      ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ code) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;
    ಖಾಸಗಿ:
      ಚಿಹ್ನೆರಹಿತ ಆಳ_2;
};

__vpiVThrStrStack::__vpiVThrStrStack(ಚಿಹ್ನೆರಹಿತ d)
: ಆಳ_2(d)
{
}

ಪೂರ್ಣಾಂಕ __vpiVThrStrStack::get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ
{ ಹಿಂತಿರುಗಿ vpiConstant; }

ಪೂರ್ಣಾಂಕ __vpiVThrStrStack::vpi_get(ಪೂರ್ಣಾಂಕ code)
{
      ಆಯ್ಕೆ (code) {
	  ಸ್ಥಿತಿ vpiConstType:
	    ಹಿಂತಿರುಗಿ vpiStringConst;
#ಆದರೆ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  ಸ್ಥಿತಿ _vpiFromThr:
	    ಹಿಂತಿರುಗಿ _vpiString;
#ಆದರೆ_ಅಂತ್ಯ
	  ಸ್ಥಿತಿ vpi_ಗಾತ್ರ:
	    ಹಿಂತಿರುಗಿ vthread_get_str_ರಾಶಿ(vpip_current_vthread, ಆಳ_2).size();
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ 0;
      }
}

ಶೂನ್ಯ __vpiVThrStrStack::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ vp)
{
      string val;
      ಅಕ್ಷರ*rbuf = 0;

      ಆದರೆ (vpip_current_vthread)
	    val = vthread_get_str_ರಾಶಿ(vpip_current_vthread, ಆಳ_2);

      ಆಯ್ಕೆ (vp->format) {

	  ಸ್ಥಿತಿ vpiObjTypeVal:
	    vp->format = vpiStringVal;
	    // fallthrough
	  ಸ್ಥಿತಿ vpiStringVal:
	    rbuf = static_cast<ಅಕ್ಷರ *>(need_ಫಲಿತಾಂಶ_buf(val.size()+1, RBUF_VAL));
	    strcpy(rbuf, val.c_str());
	    vp->value.str = rbuf;
	    ನಿಲ್ಲಿಸು;

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiConstant (String)\n", (ಪೂರ್ಣಾಂಕ)vp->format);

	    vp->format = vpiSuppressVal;
	    ನಿಲ್ಲಿಸು;
      }
}

ವರ್ಗ __vpiVThrVec4Stack : ಸಾರ್ವಜನಿಕ __vpiHandle {
    ಸಾರ್ವಜನಿಕ:
      __vpiVThrVec4Stack(ಚಿಹ್ನೆರಹಿತ ಆಳ, ತಾರ್ಕಿಕ signed_flag, ಚಿಹ್ನೆರಹಿತ wid);
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override;
      ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ code) override;
      ಅಕ್ಷರ*vpi_get_str(ಪೂರ್ಣಾಂಕ code) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;
      vpiHandle vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಂಕ flags) override;
    ಖಾಸಗಿ:
      ಸ್ಥಿರ ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ_string(p_vpi_ಮೌಲ್ಯ vp, ಸ್ಥಿರಾಂಕ vvp_vector4_t&val);
      ಸ್ಥಿರ ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ_binstr(p_vpi_ಮೌಲ್ಯ vp, ಸ್ಥಿರಾಂಕ vvp_vector4_t&val);
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ_decstr(p_vpi_ಮೌಲ್ಯ vp, ಸ್ಥಿರಾಂಕ vvp_vector4_t&val);
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ_int   (p_vpi_ಮೌಲ್ಯ vp, ಸ್ಥಿರಾಂಕ vvp_vector4_t&val);
      ಸ್ಥಿರ ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ_real  (p_vpi_ಮೌಲ್ಯ vp, ಸ್ಥಿರಾಂಕ vvp_vector4_t&val);
      ಸ್ಥಿರ ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ_strength(p_vpi_ಮೌಲ್ಯ vp, ಸ್ಥಿರಾಂಕ vvp_vector4_t&val);
      ಸ್ಥಿರ ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ_octstr(p_vpi_ಮೌಲ್ಯ vp, ಸ್ಥಿರಾಂಕ vvp_vector4_t&val);
      ಸ್ಥಿರ ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ_hexstr(p_vpi_ಮೌಲ್ಯ vp, ಸ್ಥಿರಾಂಕ vvp_vector4_t&val);
      ಸ್ಥಿರ ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ_ಸದಿಶ(p_vpi_ಮೌಲ್ಯ vp, ಸ್ಥಿರಾಂಕ vvp_vector4_t&val);
    ಖಾಸಗಿ:
      ಚಿಹ್ನೆರಹಿತ ಆಳ_2;
      ತಾರ್ಕಿಕ signed_flag_;
      ಚಿಹ್ನೆರಹಿತ expect_ಅಗಲ;
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name;
};

__vpiVThrVec4Stack::__vpiVThrVec4Stack(ಚಿಹ್ನೆರಹಿತ d, ತಾರ್ಕಿಕ sf, ಚಿಹ್ನೆರಹಿತ wid)
: ಆಳ_2(d), signed_flag_(sf), expect_ಅಗಲ(wid)
{
      name = vpip_name_string("S<,vec4,>");
}

ಪೂರ್ಣಾಂಕ __vpiVThrVec4Stack::get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ
{ ಹಿಂತಿರುಗಿ vpiConstant; }


ಪೂರ್ಣಾಂಕ __vpiVThrVec4Stack::vpi_get(ಪೂರ್ಣಾಂಕ code)
{
      ಆಯ್ಕೆ (code) {
	  ಸ್ಥಿತಿ vpi_ಗಾತ್ರ:
	    ಹಿಂತಿರುಗಿ expect_ಅಗಲ;

	  ಸ್ಥಿತಿ vpiSigned:
	    ಹಿಂತಿರುಗಿ signed_flag_? 1 : 0;

	  ಸ್ಥಿತಿ vpiConstType:
	    ಹಿಂತಿರುಗಿ vpi_ದ್ವಿಮಾನ_Const;

#ಆದರೆ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  ಸ್ಥಿತಿ _vpiFromThr:
	    ಹಿಂತಿರುಗಿ _vpiVThr;
#ಆದರೆ_ಅಂತ್ಯ

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ 0;
      }
}

ಅಕ್ಷರ*__vpiVThrVec4Stack::vpi_get_str(ಪೂರ್ಣಾಂಕ code)
{
      ಆಯ್ಕೆ (code) {
	  ಸ್ಥಿತಿ vpi_ತುಂಬಿದ_Name:
	    ಹಿಂತಿರುಗಿ simple_ಗಣ_rbuf_str(name);

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ 0;
      }
}

ಶೂನ್ಯ __vpiVThrVec4Stack::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ vp)
{
      vvp_vector4_t val;

      ಆದರೆ (vpip_current_vthread)
	    val = vthread_get_vec4_ರಾಶಿ(vpip_current_vthread, ಆಳ_2);

      ಆಯ್ಕೆ (vp->format) {

	  ಸ್ಥಿತಿ vpiBinStrVal:
	    vpi_get_ಮೌಲ್ಯ_binstr(vp, val);
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ vpiDecStrVal:
	    vpi_get_ಮೌಲ್ಯ_decstr(vp, val);
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ vpiOctStrVal:
	    vpi_get_ಮೌಲ್ಯ_octstr(vp, val);
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ vpiHexStrVal:
	    vpi_get_ಮೌಲ್ಯ_hexstr(vp, val);
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ vpiIntVal:
	    vpi_get_ಮೌಲ್ಯ_int(vp, val);
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ vpiRealVal:
	    vpi_get_ಮೌಲ್ಯ_real(vp, val);
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ vpiStringVal:
	    vpi_get_ಮೌಲ್ಯ_string(vp, val);
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ vpiStrengthVal:
	    vpi_get_ಮೌಲ್ಯ_strength(vp, val);
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ vpiObjTypeVal:
	    vp->format = vpi_ಸದಿಶ_Val;
	    // fallthrough
	  ಸ್ಥಿತಿ vpi_ಸದಿಶ_Val:
	    vpi_get_ಮೌಲ್ಯ_ಸದಿಶ(vp, val);
	    ನಿಲ್ಲಿಸು;

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "internal error: vpi_get_value(<format=%d>)"
		    " not implemented for __vpiVThrVec4Stack.\n", vp->format);
	    ಪ್ರತಿಜ್ಞಾ(0);
      }

}

ಶೂನ್ಯ __vpiVThrVec4Stack::vpi_get_ಮೌಲ್ಯ_binstr(p_vpi_ಮೌಲ್ಯ vp, ಸ್ಥಿರಾಂಕ vvp_vector4_t&val)
{
      ಚಿಹ್ನೆರಹಿತ wid = val.size();
      ಅಕ್ಷರ*rbuf = static_cast<ಅಕ್ಷರ*>(need_ಫಲಿತಾಂಶ_buf(wid+1, RBUF_VAL));
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < wid ; idx += 1) {
	    rbuf[wid-idx-1] = vvp_bit4_ಗಮ್ಯ_ascii(val.value(idx));
      }
      rbuf[wid] = 0;
      vp->value.str = rbuf;
}

ಶೂನ್ಯ __vpiVThrVec4Stack::vpi_get_ಮೌಲ್ಯ_decstr(p_vpi_ಮೌಲ್ಯ vp, ಸ್ಥಿರಾಂಕ vvp_vector4_t&val)
{
      ಚಿಹ್ನೆರಹಿತ wid = val.size();
      ಪೂರ್ಣಾಂಕ nbuf = (wid+2)/3 + 1;
      ಅಕ್ಷರ *rbuf = static_cast<ಅಕ್ಷರ*>(need_ಫಲಿತಾಂಶ_buf(nbuf, RBUF_VAL));

      vpip_vec4_ಗಮ್ಯ_dec_str(val, rbuf, nbuf, signed_flag_);
      vp->value.str = rbuf;
}

ಶೂನ್ಯ __vpiVThrVec4Stack::vpi_get_ಮೌಲ್ಯ_octstr(p_vpi_ಮೌಲ್ಯ vp, ಸ್ಥಿರಾಂಕ vvp_vector4_t&val)
{
      ಚಿಹ್ನೆರಹಿತ wid = val.size();
      ಚಿಹ್ನೆರಹಿತ owid = (wid + 2) / 3;
      ಅಕ್ಷರ*rbuf = static_cast<ಅಕ್ಷರ*>(need_ಫಲಿತಾಂಶ_buf(owid+1, RBUF_VAL));
      rbuf[owid] = 0;

      ಚಿಹ್ನೆರಹಿತ oval = 0;
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0; idx < wid ; idx += 1) {
	    ಚಿಹ್ನೆರಹಿತ tmp = 0;
	    ಆಯ್ಕೆ (val.value(idx)) {
		ಸ್ಥಿತಿ BIT4_0:
		  tmp = 0;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BIT4_1:
		  tmp = 1;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BIT4_X:
		  tmp = 2;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BIT4_Z:
		  tmp = 3;
		  ನಿಲ್ಲಿಸು;
	    }

	    oval = oval | (tmp << 2*(idx%3));

	    ಆದರೆ (idx%3 == 2) {
		  owid -= 1;
		  rbuf[owid] = oct_digits[oval];
		  oval = 0;
	    }
      }

      ಆದರೆ (owid > 0) {
	    owid -= 1;
	    rbuf[owid] = oct_digits[oval];
      }
      vp->value.str = rbuf;
}

ಶೂನ್ಯ __vpiVThrVec4Stack::vpi_get_ಮೌಲ್ಯ_hexstr(p_vpi_ಮೌಲ್ಯ vp, ಸ್ಥಿರಾಂಕ vvp_vector4_t&val)
{
      ಚಿಹ್ನೆರಹಿತ wid = val.size();
      ಚಿಹ್ನೆರಹಿತ hwid = (wid + 3) /4;
      ಅಕ್ಷರ*rbuf = static_cast<ಅಕ್ಷರ*>(need_ಫಲಿತಾಂಶ_buf(hwid+1, RBUF_VAL));
      rbuf[hwid] = 0;

      ಚಿಹ್ನೆರಹಿತ hval = 0;
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0; idx < wid ; idx += 1) {
	    ಚಿಹ್ನೆರಹಿತ tmp = 0;
	    ಆಯ್ಕೆ (val.value(idx)) {
		ಸ್ಥಿತಿ BIT4_0:
		  tmp = 0;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BIT4_1:
		  tmp = 1;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BIT4_X:
		  tmp = 2;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BIT4_Z:
		  tmp = 3;
		  ನಿಲ್ಲಿಸು;
	    }
	    hval = hval | (tmp << 2*(idx%4));

	    ಆದರೆ (idx%4 == 3) {
		  hwid -= 1;
		  rbuf[hwid] = hex_digits[hval];
		  hval = 0;
	    }
      }

      ಆದರೆ (hwid > 0) {
	    hwid -= 1;
	    rbuf[hwid] = hex_digits[hval];
      }
      vp->value.str = rbuf;
}

ಶೂನ್ಯ __vpiVThrVec4Stack::vpi_get_ಮೌಲ್ಯ_int(p_vpi_ಮೌಲ್ಯ vp, ಸ್ಥಿರಾಂಕ vvp_vector4_t&val)
{
      int32_t vali = 0;
      vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(val, vali, signed_flag_, ಸುಳ್ಳು);
      vp->value.integer = vali;
}

ಶೂನ್ಯ __vpiVThrVec4Stack::vpi_get_ಮೌಲ್ಯ_real(p_vpi_ಮೌಲ್ಯ vp, ಸ್ಥಿರಾಂಕ vvp_vector4_t&val)
{
      ಚಿಹ್ನೆರಹಿತ wid = val.size();
      vp->value.real = 0.0;

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = wid ; idx > 0 ; idx -= 1) {
	    vp->value.real *= 2.0;
	    ಆದರೆ (val.value(idx-1) == BIT4_1)
		  vp->value.real += 1.0;
      }
}

ಶೂನ್ಯ __vpiVThrVec4Stack::vpi_get_ಮೌಲ್ಯ_string(p_vpi_ಮೌಲ್ಯ vp, ಸ್ಥಿರಾಂಕ vvp_vector4_t&val)
{
      ಅಕ್ಷರ*rbuf = static_cast<ಅಕ್ಷರ*>(need_ಫಲಿತಾಂಶ_buf((val.size() / 8) + 1, RBUF_VAL));
      ಅಕ್ಷರ*cp = rbuf;

      ಅಕ್ಷರ tmp = 0;
      ಗಾಗಿ (ಪೂರ್ಣಾಂಕ bitnr = val.size()-1 ; bitnr >= 0 ; bitnr -= 1) {
	    tmp <<= 1;
	    ಆಯ್ಕೆ (val.value(bitnr)) {
		ಸ್ಥಿತಿ BIT4_1:
		  tmp |= 1;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BIT4_0:
		ಪೂರ್ವನಿಯೋಜಿತ:
		  ನಿಲ್ಲಿಸು;
	    }

	    ಆದರೆ ((bitnr&7)==0) {
		    // Don't include leading nuls
		  ಆದರೆ (tmp == 0 && cp == rbuf)
			ಮುಂದುವರಿ;

		  *cp++ = tmp? tmp : ' ';
		  tmp = 0;
	    }
      }
      *cp++ = 0;
      vp->format = vpiStringVal;
      vp->value.str = rbuf;
}

ಶೂನ್ಯ __vpiVThrVec4Stack::vpi_get_ಮೌಲ್ಯ_ಸದಿಶ(p_vpi_ಮೌಲ್ಯ vp, ಸ್ಥಿರಾಂಕ vvp_vector4_t&val)
{
      ಚಿಹ್ನೆರಹಿತ wid = val.size();

      ಚಿಹ್ನೆರಹಿತ hwid = (wid+31)/32;
      vp->value.vector = static_cast<s_vpi_vecval*>
                         (need_ಫಲಿತಾಂಶ_buf(hwid*ಗಾತ್ರ(s_vpi_vecval), RBUF_VAL));
      ಪ್ರತಿಜ್ಞಾ(vp->value.vector);

	/* Zero the top of the result buffer first since the fill loop below only writes
	   bits 0..wid-1, which can leave stale data in the unused high bits. */
      ಆದರೆ (hwid == 0) hwid = 1;
      vp->value.vector[hwid-1].aval = 0;
      vp->value.vector[hwid-1].bval = 0;

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < wid ;  idx += 1) {
	    ಪೂರ್ಣಾಂಕ ಶಬ್ದ = idx/32;
	    PLI_INT32 ಮುಖವಾಡ = 1 << (idx%32);

	    ಆಯ್ಕೆ (val.value(idx)) {
		ಸ್ಥಿತಿ BIT4_0:
		  vp->value.vector[ಶಬ್ದ].aval &= ~ಮುಖವಾಡ;
		  vp->value.vector[ಶಬ್ದ].bval &= ~ಮುಖವಾಡ;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BIT4_1:
		  vp->value.vector[ಶಬ್ದ].aval |=  ಮುಖವಾಡ;
		  vp->value.vector[ಶಬ್ದ].bval &= ~ಮುಖವಾಡ;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BIT4_X:
		  vp->value.vector[ಶಬ್ದ].aval |=  ಮುಖವಾಡ;
		  vp->value.vector[ಶಬ್ದ].bval |=  ಮುಖವಾಡ;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BIT4_Z:
		  vp->value.vector[ಶಬ್ದ].aval &= ~ಮುಖವಾಡ;
		  vp->value.vector[ಶಬ್ದ].bval |=  ಮುಖವಾಡ;
		  ನಿಲ್ಲಿಸು;
	    }
      }
}

ಶೂನ್ಯ __vpiVThrVec4Stack::vpi_get_ಮೌಲ್ಯ_strength(p_vpi_ಮೌಲ್ಯ vp, ಸ್ಥಿರಾಂಕ vvp_vector4_t&val)
{
      s_vpi_strengthval*op = static_cast<s_vpi_strengthval*>
                             (need_ಫಲಿತಾಂಶ_buf(val.size() * ಗಾತ್ರ(s_vpi_strengthval),
                                              RBUF_VAL));

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < val.size() ; idx += 1) {
	    ಆಯ್ಕೆ (val.value(idx)) {
		ಸ್ಥಿತಿ BIT4_0:
		  op[idx].logic = vpi0;
		  op[idx].s0 = vpiStrongDrive;
		  op[idx].s1 = 0;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BIT4_1:
		  op[idx].logic = vpi1;
		  op[idx].s0 = 0;
		  op[idx].s1 = vpiStrongDrive;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BIT4_X:
		  op[idx].logic = vpiX;
		  op[idx].s0 = vpiStrongDrive;
		  op[idx].s1 = vpiStrongDrive;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BIT4_Z:
		  op[idx].logic = vpiZ;
		  op[idx].s0 = vpiHiZ;
		  op[idx].s1 = vpiHiZ;
		  ನಿಲ್ಲಿಸು;
	    }
      }

      vp->format = vpiStrengthVal;
      vp->value.strength = op;
}

vpiHandle __vpiVThrVec4Stack::vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ vp, ಪೂರ್ಣಾಂಕ /*flags*/)
{
      ಪ್ರತಿಜ್ಞಾ(vpip_current_vthread);

      ಆಯ್ಕೆ (vp->format) {

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "internal error: vpi_put_value(<format=%d>)"
		    " not implemented for __vpiVThrVec4Stack.\n", vp->format);
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಹಿಂತಿರುಗಿ 0;
      }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಶೂನ್ಯ thread_vthr_delete(vpiHandle ಅಂಶ)
{
      handle_map[ಅಂಶ] = ನಿಜ;
}

ಸ್ಥಿರ ಶೂನ್ಯ thread_vthr_delete_real(vpiHandle ಅಂಶ)
{
      __vpiVThrVec4Stack*obj = dynamic_cast<__vpiVThrVec4Stack*>(ಅಂಶ);
      ಅಳಿಸು obj;
}

ಶೂನ್ಯ vpi_handle_delete()
{
      map<vpiHandle, ತಾರ್ಕಿಕ>::iterator iter;
      ಗಾಗಿ (iter = handle_map.begin(); iter != handle_map.ಕೊನೆ(); ++ iter ) {
	    ಆದರೆ (iter->second) thread_vthr_delete_real(iter->ಮೊದಲ);
	    ಇಲ್ಲದಿದ್ದರೆ thread_ಶಬ್ದ_delete_real(iter->ಮೊದಲ);
      }
}
#ಆದರೆ_ಅಂತ್ಯ


vpiHandle vpip_make_vthr_str_ರಾಶಿ(ಚಿಹ್ನೆರಹಿತ ಆಳ)
{
      __vpiVThrStrStack*obj = ಹೊಸ __vpiVThrStrStack(ಆಳ);
      ಹಿಂತಿರುಗಿ obj;
}

vpiHandle vpip_make_vthr_vec4_ರಾಶಿ(ಚಿಹ್ನೆರಹಿತ ಆಳ, ತಾರ್ಕಿಕ signed_flag, ಚಿಹ್ನೆರಹಿತ wid)
{
      __vpiVThrVec4Stack*obj = ಹೊಸ __vpiVThrVec4Stack(ಆಳ, signed_flag, wid);
      ಹಿಂತಿರುಗಿ obj;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಸ್ಥಿರ map<vpiHandle, ತಾರ್ಕಿಕ> ರಾಶಿ_map;

ಶೂನ್ಯ thread_string_delete(vpiHandle ಅಂಶ)
{
      ರಾಶಿ_map[ಅಂಶ] = ಸುಳ್ಳು;
}

ಸ್ಥಿರ ಶೂನ್ಯ thread_string_delete_real(vpiHandle ಅಂಶ)
{
      __vpiVThrStrStack*obj = dynamic_cast<__vpiVThrStrStack*>(ಅಂಶ);
      ಅಳಿಸು obj;
}

ಶೂನ್ಯ vpi_ರಾಶಿ_delete()
{
      map<vpiHandle, ತಾರ್ಕಿಕ>::iterator iter;
      ಗಾಗಿ (iter = ರಾಶಿ_map.begin(); iter != ರಾಶಿ_map.ಕೊನೆ(); ++ iter ) {
	    thread_string_delete_real(iter->ಮೊದಲ);
      }
}
#ಆದರೆ_ಅಂತ್ಯ
