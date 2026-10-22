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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
# include <map>
#ਜੇ_ਅੰਤ
# include  <cstdio>
# include  <cstdlib>
# include  <cassert>
# include  "ivl_alloc.h"

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ std;

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
ਬਾਹਰੀ ਅਚਲ ਅੱਖਰ hex_digits[256];
ਬਾਹਰੀ ਅਚਲ ਅੱਖਰ oct_digits[64];


ਵਰਗ __vpiVThrWord : ਜਨਤਕ __vpiHandle {
    ਜਨਤਕ:
      ਸਪਸ਼ਟ __vpiVThrWord(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ);
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override;
      ਪੂਰਨ vpi_get(ਪੂਰਨ code) override;
      ਖਾਲੀ vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val) override;

      ਪੂਰਨ get_subtype() ਅਚਲ { ਵਾਪਸ subtype; };
      ਬਿਨਾਨਿਸ਼ਾਨੀ get_ਸੂਚਕਾਂਕ() ਅਚਲ { ਵਾਪਸ ਸੂਚਕਾਂਕ; };

    ਨਿਜੀ:
      ਅਚਲ ਅੱਖਰ* name;
      ਪੂਰਨ subtype;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੂਚਕਾਂਕ;
};

ਸਥਿਰ ਪੂਰਨ vthr_ਸ਼ਬਦ_get(ਪੂਰਨ code, vpiHandle ref)
{
      ਅਚਲ __vpiVThrWord*rfp = dynamic_cast<__vpiVThrWord*>(ref);

      ਚੋਣ (code) {

	  ਹਾਲਤ vpiConstType:
	    ਵਾਪਸ rfp->get_subtype();

#ਜੇ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  ਹਾਲਤ _vpiFromThr:
	    ਵਾਪਸ vpi_ਸ਼ਬਦ;
#ਜੇ_ਅੰਤ

	  ਮੂਲ:
	    ਵਾਪਸ 0;
      }
}

ਸਥਿਰ ਖਾਲੀ vthr_real_get_ਮੁੱਲ(vpiHandle ref, s_vpi_ਮੁੱਲ*vp)
{
      ਅਚਲ __vpiVThrWord*obj = dynamic_cast<__vpiVThrWord*>(ref);
      ਸਥਿਰ ਅਚਲ size_t RBUF_USE_ਮਾਪ = 66;
      ਅੱਖਰ *rbuf = static_cast<ਅੱਖਰ *>(need_ਨਤੀਜਾ_buf(RBUF_USE_ਮਾਪ, RBUF_VAL));

      ਦੁੱਗਣਾ val = 0.0;

	/* Get the actual value from the index. It is possible, by the
	   way, that the vpi_get_value is called from compiletf. If
	   that's the case, there will be no current thread, and this
	   will not have access to the proper value. Punt and return a
	   0.0 value instead. */
      ਜੇ (vpip_current_vthread)
	    val = vthread_get_real_ਢੇਰ(vpip_current_vthread,
	                                 obj->get_ਸੂਚਕਾਂਕ());

      ਚੋਣ (vp->format) {

	  ਹਾਲਤ vpiObjTypeVal:
	    vp->format = vpiRealVal;
	    // fallthrough
	  ਹਾਲਤ vpiRealVal:
	    vp->value.real = val;
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiIntVal:
	      /* NaN or +/- infinity are translated as 0. */
	    ਜੇ (val != val || (val && (val == 0.5*val))) {
		  val = 0.0;
	    } ਨਹੀਂਤਾਂ {
		  val = std::ਗੇੜਾ_2(val);
	    }
	    vp->value.integer = (PLI_INT32)val;
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiDecStrVal:
	    ਜੇ (std::isnan(val))
		  snprintf(rbuf, RBUF_USE_ਮਾਪ, "%s", "nan");
	    ਨਹੀਂਤਾਂ
		  snprintf(rbuf, RBUF_USE_ਮਾਪ, "%0.0f", std::ਗੇੜਾ_2(val));
	    vp->value.str = rbuf;
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiOctStrVal:
	    snprintf(rbuf, RBUF_USE_ਮਾਪ, "%" PRIo64, vlg_ਗੇੜਾ_ਮੰਜ਼ਿਲ_u64(val));
	    vp->value.str = rbuf;
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiHexStrVal:
	    snprintf(rbuf, RBUF_USE_ਮਾਪ, "%" PRIx64, vlg_ਗੇੜਾ_ਮੰਜ਼ਿਲ_u64(val));
	    vp->value.str = rbuf;
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiBinStrVal: {
		uint64_t vali = vlg_ਗੇੜਾ_ਮੰਜ਼ਿਲ_u64(val);
		ਬਿਨਾਨਿਸ਼ਾਨੀ len = 0;

		ਜਦੋਂ (vali > 0) {
		      len += 1;
		      vali /= 2;
		}

		vali = vlg_ਗੇੜਾ_ਮੰਜ਼ਿਲ_u64(val);
		ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < len ;  idx += 1) {
		      rbuf[len-idx-1] = (vali & 1)? '1' : '0';
		      vali /= 2;
		}

		rbuf[len] = 0;
		ਜੇ (len == 0) {
		      rbuf[0] = '0';
		      rbuf[1] = 0;
		}
		vp->value.str = rbuf;
		ਰੋਕੋ;
	  }

	  ਮੂਲ:
	    fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiConstant (Real)\n", (ਪੂਰਨ)vp->format);

	    vp->format = vpiSuppressVal;
	    ਰੋਕੋ;
      }
}

ਸਤਰਅੰਦਰ __vpiVThrWord::__vpiVThrWord(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ)
{
      name = vpip_name_string("W<>");
      subtype = vpiRealConst;
      ਸੂਚਕਾਂਕ = ਅਧਾਰ;
}

ਪੂਰਨ __vpiVThrWord::get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ
{ ਵਾਪਸ vpiConstant; }

ਪੂਰਨ __vpiVThrWord::vpi_get(ਪੂਰਨ code)
{ ਵਾਪਸ vthr_ਸ਼ਬਦ_get(code, ਇਹ); }

ਖਾਲੀ __vpiVThrWord::vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val)
{ vthr_real_get_ਮੁੱਲ(ਇਹ, val); }

vpiHandle vpip_make_vthr_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਅਚਲ ਅੱਖਰ*type)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(type[0] == 'r');
      ਪ੍ਰਤਿਜ੍ਞਾ(ਅਧਾਰ < 65536);
      __vpiVThrWord*obj = ਨਵਾਂ __vpiVThrWord(ਅਧਾਰ);

      ਵਾਪਸ obj;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਸਥਿਰ map<vpiHandle, ਤਾਰਕਿਕ> handle_map;

ਖਾਲੀ thread_ਸ਼ਬਦ_delete(vpiHandle ਵਸਤੂ)
{
      handle_map[ਵਸਤੂ] = ਝੂਠ;
}

ਸਥਿਰ ਖਾਲੀ thread_ਸ਼ਬਦ_delete_real(vpiHandle ਵਸਤੂ)
{
      ਵਰਗ __vpiVThrWord*obj = dynamic_cast<__vpiVThrWord*>(ਵਸਤੂ);
      ਮਿਟਾਓ obj;
}
#ਜੇ_ਅੰਤ

ਵਰਗ __vpiVThrStrStack : ਜਨਤਕ __vpiHandle {
    ਜਨਤਕ:
      ਸਪਸ਼ਟ __vpiVThrStrStack(ਬਿਨਾਨਿਸ਼ਾਨੀ ਡੂੰਘਾਈ);
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override;
      ਪੂਰਨ vpi_get(ਪੂਰਨ code) override;
      ਖਾਲੀ vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val) override;
    ਨਿਜੀ:
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਡੂੰਘਾਈ_2;
};

__vpiVThrStrStack::__vpiVThrStrStack(ਬਿਨਾਨਿਸ਼ਾਨੀ d)
: ਡੂੰਘਾਈ_2(d)
{
}

ਪੂਰਨ __vpiVThrStrStack::get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ
{ ਵਾਪਸ vpiConstant; }

ਪੂਰਨ __vpiVThrStrStack::vpi_get(ਪੂਰਨ code)
{
      ਚੋਣ (code) {
	  ਹਾਲਤ vpiConstType:
	    ਵਾਪਸ vpiStringConst;
#ਜੇ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  ਹਾਲਤ _vpiFromThr:
	    ਵਾਪਸ _vpiString;
#ਜੇ_ਅੰਤ
	  ਹਾਲਤ vpi_ਮਾਪ:
	    ਵਾਪਸ vthread_get_str_ਢੇਰ(vpip_current_vthread, ਡੂੰਘਾਈ_2).size();
	  ਮੂਲ:
	    ਵਾਪਸ 0;
      }
}

ਖਾਲੀ __vpiVThrStrStack::vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ vp)
{
      string val;
      ਅੱਖਰ*rbuf = 0;

      ਜੇ (vpip_current_vthread)
	    val = vthread_get_str_ਢੇਰ(vpip_current_vthread, ਡੂੰਘਾਈ_2);

      ਚੋਣ (vp->format) {

	  ਹਾਲਤ vpiObjTypeVal:
	    vp->format = vpiStringVal;
	    // fallthrough
	  ਹਾਲਤ vpiStringVal:
	    rbuf = static_cast<ਅੱਖਰ *>(need_ਨਤੀਜਾ_buf(val.size()+1, RBUF_VAL));
	    strcpy(rbuf, val.c_str());
	    vp->value.str = rbuf;
	    ਰੋਕੋ;

	  ਮੂਲ:
	    fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiConstant (String)\n", (ਪੂਰਨ)vp->format);

	    vp->format = vpiSuppressVal;
	    ਰੋਕੋ;
      }
}

ਵਰਗ __vpiVThrVec4Stack : ਜਨਤਕ __vpiHandle {
    ਜਨਤਕ:
      __vpiVThrVec4Stack(ਬਿਨਾਨਿਸ਼ਾਨੀ ਡੂੰਘਾਈ, ਤਾਰਕਿਕ signed_flag, ਬਿਨਾਨਿਸ਼ਾਨੀ wid);
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override;
      ਪੂਰਨ vpi_get(ਪੂਰਨ code) override;
      ਅੱਖਰ*vpi_get_str(ਪੂਰਨ code) override;
      ਖਾਲੀ vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val) override;
      vpiHandle vpi_put_ਮੁੱਲ(p_vpi_ਮੁੱਲ val, ਪੂਰਨ flags) override;
    ਨਿਜੀ:
      ਸਥਿਰ ਖਾਲੀ vpi_get_ਮੁੱਲ_string(p_vpi_ਮੁੱਲ vp, ਅਚਲ vvp_vector4_t&val);
      ਸਥਿਰ ਖਾਲੀ vpi_get_ਮੁੱਲ_binstr(p_vpi_ਮੁੱਲ vp, ਅਚਲ vvp_vector4_t&val);
      ਖਾਲੀ vpi_get_ਮੁੱਲ_decstr(p_vpi_ਮੁੱਲ vp, ਅਚਲ vvp_vector4_t&val);
      ਖਾਲੀ vpi_get_ਮੁੱਲ_int   (p_vpi_ਮੁੱਲ vp, ਅਚਲ vvp_vector4_t&val);
      ਸਥਿਰ ਖਾਲੀ vpi_get_ਮੁੱਲ_real  (p_vpi_ਮੁੱਲ vp, ਅਚਲ vvp_vector4_t&val);
      ਸਥਿਰ ਖਾਲੀ vpi_get_ਮੁੱਲ_strength(p_vpi_ਮੁੱਲ vp, ਅਚਲ vvp_vector4_t&val);
      ਸਥਿਰ ਖਾਲੀ vpi_get_ਮੁੱਲ_octstr(p_vpi_ਮੁੱਲ vp, ਅਚਲ vvp_vector4_t&val);
      ਸਥਿਰ ਖਾਲੀ vpi_get_ਮੁੱਲ_hexstr(p_vpi_ਮੁੱਲ vp, ਅਚਲ vvp_vector4_t&val);
      ਸਥਿਰ ਖਾਲੀ vpi_get_ਮੁੱਲ_ਸਦਿਸ਼(p_vpi_ਮੁੱਲ vp, ਅਚਲ vvp_vector4_t&val);
    ਨਿਜੀ:
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਡੂੰਘਾਈ_2;
      ਤਾਰਕਿਕ signed_flag_;
      ਬਿਨਾਨਿਸ਼ਾਨੀ expect_ਚੌੜਾਈ;
      ਅਚਲ ਅੱਖਰ*name;
};

__vpiVThrVec4Stack::__vpiVThrVec4Stack(ਬਿਨਾਨਿਸ਼ਾਨੀ d, ਤਾਰਕਿਕ sf, ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
: ਡੂੰਘਾਈ_2(d), signed_flag_(sf), expect_ਚੌੜਾਈ(wid)
{
      name = vpip_name_string("S<,vec4,>");
}

ਪੂਰਨ __vpiVThrVec4Stack::get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ
{ ਵਾਪਸ vpiConstant; }


ਪੂਰਨ __vpiVThrVec4Stack::vpi_get(ਪੂਰਨ code)
{
      ਚੋਣ (code) {
	  ਹਾਲਤ vpi_ਮਾਪ:
	    ਵਾਪਸ expect_ਚੌੜਾਈ;

	  ਹਾਲਤ vpiSigned:
	    ਵਾਪਸ signed_flag_? 1 : 0;

	  ਹਾਲਤ vpiConstType:
	    ਵਾਪਸ vpi_ਦੁਆਧਾਰੀ_Const;

#ਜੇ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  ਹਾਲਤ _vpiFromThr:
	    ਵਾਪਸ _vpiVThr;
#ਜੇ_ਅੰਤ

	  ਮੂਲ:
	    ਵਾਪਸ 0;
      }
}

ਅੱਖਰ*__vpiVThrVec4Stack::vpi_get_str(ਪੂਰਨ code)
{
      ਚੋਣ (code) {
	  ਹਾਲਤ vpi_ਭਰਿਆ_Name:
	    ਵਾਪਸ simple_ਸੈੱਟ_rbuf_str(name);

	  ਮੂਲ:
	    ਵਾਪਸ 0;
      }
}

ਖਾਲੀ __vpiVThrVec4Stack::vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ vp)
{
      vvp_vector4_t val;

      ਜੇ (vpip_current_vthread)
	    val = vthread_get_vec4_ਢੇਰ(vpip_current_vthread, ਡੂੰਘਾਈ_2);

      ਚੋਣ (vp->format) {

	  ਹਾਲਤ vpiBinStrVal:
	    vpi_get_ਮੁੱਲ_binstr(vp, val);
	    ਰੋਕੋ;
	  ਹਾਲਤ vpiDecStrVal:
	    vpi_get_ਮੁੱਲ_decstr(vp, val);
	    ਰੋਕੋ;
	  ਹਾਲਤ vpiOctStrVal:
	    vpi_get_ਮੁੱਲ_octstr(vp, val);
	    ਰੋਕੋ;
	  ਹਾਲਤ vpiHexStrVal:
	    vpi_get_ਮੁੱਲ_hexstr(vp, val);
	    ਰੋਕੋ;
	  ਹਾਲਤ vpiIntVal:
	    vpi_get_ਮੁੱਲ_int(vp, val);
	    ਰੋਕੋ;
	  ਹਾਲਤ vpiRealVal:
	    vpi_get_ਮੁੱਲ_real(vp, val);
	    ਰੋਕੋ;
	  ਹਾਲਤ vpiStringVal:
	    vpi_get_ਮੁੱਲ_string(vp, val);
	    ਰੋਕੋ;
	  ਹਾਲਤ vpiStrengthVal:
	    vpi_get_ਮੁੱਲ_strength(vp, val);
	    ਰੋਕੋ;
	  ਹਾਲਤ vpiObjTypeVal:
	    vp->format = vpi_ਸਦਿਸ਼_Val;
	    // fallthrough
	  ਹਾਲਤ vpi_ਸਦਿਸ਼_Val:
	    vpi_get_ਮੁੱਲ_ਸਦਿਸ਼(vp, val);
	    ਰੋਕੋ;

	  ਮੂਲ:
	    fprintf(stderr, "internal error: vpi_get_value(<format=%d>)"
		    " not implemented for __vpiVThrVec4Stack.\n", vp->format);
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
      }

}

ਖਾਲੀ __vpiVThrVec4Stack::vpi_get_ਮੁੱਲ_binstr(p_vpi_ਮੁੱਲ vp, ਅਚਲ vvp_vector4_t&val)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = val.size();
      ਅੱਖਰ*rbuf = static_cast<ਅੱਖਰ*>(need_ਨਤੀਜਾ_buf(wid+1, RBUF_VAL));
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < wid ; idx += 1) {
	    rbuf[wid-idx-1] = vvp_bit4_ਮੰਜ਼ਿਲ_ascii(val.value(idx));
      }
      rbuf[wid] = 0;
      vp->value.str = rbuf;
}

ਖਾਲੀ __vpiVThrVec4Stack::vpi_get_ਮੁੱਲ_decstr(p_vpi_ਮੁੱਲ vp, ਅਚਲ vvp_vector4_t&val)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = val.size();
      ਪੂਰਨ nbuf = (wid+2)/3 + 1;
      ਅੱਖਰ *rbuf = static_cast<ਅੱਖਰ*>(need_ਨਤੀਜਾ_buf(nbuf, RBUF_VAL));

      vpip_vec4_ਮੰਜ਼ਿਲ_dec_str(val, rbuf, nbuf, signed_flag_);
      vp->value.str = rbuf;
}

ਖਾਲੀ __vpiVThrVec4Stack::vpi_get_ਮੁੱਲ_octstr(p_vpi_ਮੁੱਲ vp, ਅਚਲ vvp_vector4_t&val)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = val.size();
      ਬਿਨਾਨਿਸ਼ਾਨੀ owid = (wid + 2) / 3;
      ਅੱਖਰ*rbuf = static_cast<ਅੱਖਰ*>(need_ਨਤੀਜਾ_buf(owid+1, RBUF_VAL));
      rbuf[owid] = 0;

      ਬਿਨਾਨਿਸ਼ਾਨੀ oval = 0;
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0; idx < wid ; idx += 1) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ tmp = 0;
	    ਚੋਣ (val.value(idx)) {
		ਹਾਲਤ BIT4_0:
		  tmp = 0;
		  ਰੋਕੋ;
		ਹਾਲਤ BIT4_1:
		  tmp = 1;
		  ਰੋਕੋ;
		ਹਾਲਤ BIT4_X:
		  tmp = 2;
		  ਰੋਕੋ;
		ਹਾਲਤ BIT4_Z:
		  tmp = 3;
		  ਰੋਕੋ;
	    }

	    oval = oval | (tmp << 2*(idx%3));

	    ਜੇ (idx%3 == 2) {
		  owid -= 1;
		  rbuf[owid] = oct_digits[oval];
		  oval = 0;
	    }
      }

      ਜੇ (owid > 0) {
	    owid -= 1;
	    rbuf[owid] = oct_digits[oval];
      }
      vp->value.str = rbuf;
}

ਖਾਲੀ __vpiVThrVec4Stack::vpi_get_ਮੁੱਲ_hexstr(p_vpi_ਮੁੱਲ vp, ਅਚਲ vvp_vector4_t&val)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = val.size();
      ਬਿਨਾਨਿਸ਼ਾਨੀ hwid = (wid + 3) /4;
      ਅੱਖਰ*rbuf = static_cast<ਅੱਖਰ*>(need_ਨਤੀਜਾ_buf(hwid+1, RBUF_VAL));
      rbuf[hwid] = 0;

      ਬਿਨਾਨਿਸ਼ਾਨੀ hval = 0;
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0; idx < wid ; idx += 1) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ tmp = 0;
	    ਚੋਣ (val.value(idx)) {
		ਹਾਲਤ BIT4_0:
		  tmp = 0;
		  ਰੋਕੋ;
		ਹਾਲਤ BIT4_1:
		  tmp = 1;
		  ਰੋਕੋ;
		ਹਾਲਤ BIT4_X:
		  tmp = 2;
		  ਰੋਕੋ;
		ਹਾਲਤ BIT4_Z:
		  tmp = 3;
		  ਰੋਕੋ;
	    }
	    hval = hval | (tmp << 2*(idx%4));

	    ਜੇ (idx%4 == 3) {
		  hwid -= 1;
		  rbuf[hwid] = hex_digits[hval];
		  hval = 0;
	    }
      }

      ਜੇ (hwid > 0) {
	    hwid -= 1;
	    rbuf[hwid] = hex_digits[hval];
      }
      vp->value.str = rbuf;
}

ਖਾਲੀ __vpiVThrVec4Stack::vpi_get_ਮੁੱਲ_int(p_vpi_ਮੁੱਲ vp, ਅਚਲ vvp_vector4_t&val)
{
      int32_t vali = 0;
      vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(val, vali, signed_flag_, ਝੂਠ);
      vp->value.integer = vali;
}

ਖਾਲੀ __vpiVThrVec4Stack::vpi_get_ਮੁੱਲ_real(p_vpi_ਮੁੱਲ vp, ਅਚਲ vvp_vector4_t&val)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = val.size();
      vp->value.real = 0.0;

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = wid ; idx > 0 ; idx -= 1) {
	    vp->value.real *= 2.0;
	    ਜੇ (val.value(idx-1) == BIT4_1)
		  vp->value.real += 1.0;
      }
}

ਖਾਲੀ __vpiVThrVec4Stack::vpi_get_ਮੁੱਲ_string(p_vpi_ਮੁੱਲ vp, ਅਚਲ vvp_vector4_t&val)
{
      ਅੱਖਰ*rbuf = static_cast<ਅੱਖਰ*>(need_ਨਤੀਜਾ_buf((val.size() / 8) + 1, RBUF_VAL));
      ਅੱਖਰ*cp = rbuf;

      ਅੱਖਰ tmp = 0;
      ਲਈ (ਪੂਰਨ bitnr = val.size()-1 ; bitnr >= 0 ; bitnr -= 1) {
	    tmp <<= 1;
	    ਚੋਣ (val.value(bitnr)) {
		ਹਾਲਤ BIT4_1:
		  tmp |= 1;
		  ਰੋਕੋ;
		ਹਾਲਤ BIT4_0:
		ਮੂਲ:
		  ਰੋਕੋ;
	    }

	    ਜੇ ((bitnr&7)==0) {
		    // Don't include leading nuls
		  ਜੇ (tmp == 0 && cp == rbuf)
			ਜਾਰੀ;

		  *cp++ = tmp? tmp : ' ';
		  tmp = 0;
	    }
      }
      *cp++ = 0;
      vp->format = vpiStringVal;
      vp->value.str = rbuf;
}

ਖਾਲੀ __vpiVThrVec4Stack::vpi_get_ਮੁੱਲ_ਸਦਿਸ਼(p_vpi_ਮੁੱਲ vp, ਅਚਲ vvp_vector4_t&val)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = val.size();

      ਬਿਨਾਨਿਸ਼ਾਨੀ hwid = (wid+31)/32;
      vp->value.vector = static_cast<s_vpi_vecval*>
                         (need_ਨਤੀਜਾ_buf(hwid*ਆਕਾਰ(s_vpi_vecval), RBUF_VAL));
      ਪ੍ਰਤਿਜ੍ਞਾ(vp->value.vector);

	/* Zero the top of the result buffer first since the fill loop below only writes
	   bits 0..wid-1, which can leave stale data in the unused high bits. */
      ਜੇ (hwid == 0) hwid = 1;
      vp->value.vector[hwid-1].aval = 0;
      vp->value.vector[hwid-1].bval = 0;

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < wid ;  idx += 1) {
	    ਪੂਰਨ ਸ਼ਬਦ_2 = idx/32;
	    PLI_INT32 ਨਕਾਬ = 1 << (idx%32);

	    ਚੋਣ (val.value(idx)) {
		ਹਾਲਤ BIT4_0:
		  vp->value.vector[ਸ਼ਬਦ_2].aval &= ~ਨਕਾਬ;
		  vp->value.vector[ਸ਼ਬਦ_2].bval &= ~ਨਕਾਬ;
		  ਰੋਕੋ;
		ਹਾਲਤ BIT4_1:
		  vp->value.vector[ਸ਼ਬਦ_2].aval |=  ਨਕਾਬ;
		  vp->value.vector[ਸ਼ਬਦ_2].bval &= ~ਨਕਾਬ;
		  ਰੋਕੋ;
		ਹਾਲਤ BIT4_X:
		  vp->value.vector[ਸ਼ਬਦ_2].aval |=  ਨਕਾਬ;
		  vp->value.vector[ਸ਼ਬਦ_2].bval |=  ਨਕਾਬ;
		  ਰੋਕੋ;
		ਹਾਲਤ BIT4_Z:
		  vp->value.vector[ਸ਼ਬਦ_2].aval &= ~ਨਕਾਬ;
		  vp->value.vector[ਸ਼ਬਦ_2].bval |=  ਨਕਾਬ;
		  ਰੋਕੋ;
	    }
      }
}

ਖਾਲੀ __vpiVThrVec4Stack::vpi_get_ਮੁੱਲ_strength(p_vpi_ਮੁੱਲ vp, ਅਚਲ vvp_vector4_t&val)
{
      s_vpi_strengthval*op = static_cast<s_vpi_strengthval*>
                             (need_ਨਤੀਜਾ_buf(val.size() * ਆਕਾਰ(s_vpi_strengthval),
                                              RBUF_VAL));

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < val.size() ; idx += 1) {
	    ਚੋਣ (val.value(idx)) {
		ਹਾਲਤ BIT4_0:
		  op[idx].logic = vpi0;
		  op[idx].s0 = vpiStrongDrive;
		  op[idx].s1 = 0;
		  ਰੋਕੋ;
		ਹਾਲਤ BIT4_1:
		  op[idx].logic = vpi1;
		  op[idx].s0 = 0;
		  op[idx].s1 = vpiStrongDrive;
		  ਰੋਕੋ;
		ਹਾਲਤ BIT4_X:
		  op[idx].logic = vpiX;
		  op[idx].s0 = vpiStrongDrive;
		  op[idx].s1 = vpiStrongDrive;
		  ਰੋਕੋ;
		ਹਾਲਤ BIT4_Z:
		  op[idx].logic = vpiZ;
		  op[idx].s0 = vpiHiZ;
		  op[idx].s1 = vpiHiZ;
		  ਰੋਕੋ;
	    }
      }

      vp->format = vpiStrengthVal;
      vp->value.strength = op;
}

vpiHandle __vpiVThrVec4Stack::vpi_put_ਮੁੱਲ(p_vpi_ਮੁੱਲ vp, ਪੂਰਨ /*flags*/)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(vpip_current_vthread);

      ਚੋਣ (vp->format) {

	  ਮੂਲ:
	    fprintf(stderr, "internal error: vpi_put_value(<format=%d>)"
		    " not implemented for __vpiVThrVec4Stack.\n", vp->format);
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
	    ਵਾਪਸ 0;
      }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਖਾਲੀ thread_vthr_delete(vpiHandle ਵਸਤੂ)
{
      handle_map[ਵਸਤੂ] = ਸੱਚ;
}

ਸਥਿਰ ਖਾਲੀ thread_vthr_delete_real(vpiHandle ਵਸਤੂ)
{
      __vpiVThrVec4Stack*obj = dynamic_cast<__vpiVThrVec4Stack*>(ਵਸਤੂ);
      ਮਿਟਾਓ obj;
}

ਖਾਲੀ vpi_handle_delete()
{
      map<vpiHandle, ਤਾਰਕਿਕ>::iterator iter;
      ਲਈ (iter = handle_map.begin(); iter != handle_map.ਸਿਰਾ(); ++ iter ) {
	    ਜੇ (iter->second) thread_vthr_delete_real(iter->ਪਹਿਲਾ);
	    ਨਹੀਂਤਾਂ thread_ਸ਼ਬਦ_delete_real(iter->ਪਹਿਲਾ);
      }
}
#ਜੇ_ਅੰਤ


vpiHandle vpip_make_vthr_str_ਢੇਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਡੂੰਘਾਈ)
{
      __vpiVThrStrStack*obj = ਨਵਾਂ __vpiVThrStrStack(ਡੂੰਘਾਈ);
      ਵਾਪਸ obj;
}

vpiHandle vpip_make_vthr_vec4_ਢੇਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਡੂੰਘਾਈ, ਤਾਰਕਿਕ signed_flag, ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
{
      __vpiVThrVec4Stack*obj = ਨਵਾਂ __vpiVThrVec4Stack(ਡੂੰਘਾਈ, signed_flag, wid);
      ਵਾਪਸ obj;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਸਥਿਰ map<vpiHandle, ਤਾਰਕਿਕ> ਢੇਰ_map;

ਖਾਲੀ thread_string_delete(vpiHandle ਵਸਤੂ)
{
      ਢੇਰ_map[ਵਸਤੂ] = ਝੂਠ;
}

ਸਥਿਰ ਖਾਲੀ thread_string_delete_real(vpiHandle ਵਸਤੂ)
{
      __vpiVThrStrStack*obj = dynamic_cast<__vpiVThrStrStack*>(ਵਸਤੂ);
      ਮਿਟਾਓ obj;
}

ਖਾਲੀ vpi_ਢੇਰ_delete()
{
      map<vpiHandle, ਤਾਰਕਿਕ>::iterator iter;
      ਲਈ (iter = ਢੇਰ_map.begin(); iter != ਢੇਰ_map.ਸਿਰਾ(); ++ iter ) {
	    thread_string_delete_real(iter->ਪਹਿਲਾ);
      }
}
#ਜੇ_ਅੰਤ
