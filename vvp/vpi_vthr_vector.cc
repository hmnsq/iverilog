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

using نطاق std;

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
خارجي ثابت حرف hex_digits[256];
خارجي ثابت حرف oct_digits[64];


صنف __vpiVThrWord : عام __vpiHandle {
    عام:
      explicit __vpiVThrWord(unsigned base);
      صحيح get_type_code(فراغ) ثابت override;
      صحيح vpi_get(صحيح code) override;
      فراغ vpi_get_بەها(p_vpi_بەها val) override;

      صحيح get_subtype() ثابت { أرجع subtype; };
      unsigned get_index() ثابت { أرجع index; };

    خاص:
      ثابت حرف* name;
      صحيح subtype;
      unsigned index;
};

ساكن صحيح vthr_word_get(صحيح code, vpiHandle ref)
{
      ثابت __vpiVThrWord*rfp = dynamic_cast<__vpiVThrWord*>(ref);

      بدل (code) {

	  حالة vpiConstType:
	    أرجع rfp->get_subtype();

#إذا defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  حالة _vpiFromThr:
	    أرجع _vpiWord;
#endif

	  افتراضي:
	    أرجع 0;
      }
}

ساكن فراغ vthr_real_get_بەها(vpiHandle ref, s_vpi_بەها*vp)
{
      ثابت __vpiVThrWord*obj = dynamic_cast<__vpiVThrWord*>(ref);
      ساكن ثابت size_t RBUF_USE_SIZE = 66;
      حرف *rbuf = static_cast<حرف *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));

      مزدوج val = 0.0;

	/* Get the actual value from the index. It is possible, by the
	   way, that the vpi_get_value is called from compiletf. If
	   that's the case, there will be no current thread, and this
	   will not have access to the proper value. Punt and return a
	   0.0 value instead. */
      إذا (vpip_current_vthread)
	    val = vthread_get_real_کەڵەکە(vpip_current_vthread,
	                                 obj->get_index());

      بدل (vp->format) {

	  حالة vpiObjTypeVal:
	    vp->format = vpiRealVal;
	    // fallthrough
	  حالة vpiRealVal:
	    vp->value.real = val;
	    اكسر;

	  حالة vpiIntVal:
	      /* NaN or +/- infinity are translated as 0. */
	    إذا (val != val || (val && (val == 0.5*val))) {
		  val = 0.0;
	    } وإلا {
		  val = std::round(val);
	    }
	    vp->value.integer = (PLI_INT32)val;
	    اكسر;

	  حالة vpiDecStrVal:
	    إذا (std::isnan(val))
		  snprintf(rbuf, RBUF_USE_SIZE, "%s", "nan");
	    وإلا
		  snprintf(rbuf, RBUF_USE_SIZE, "%0.0f", std::round(val));
	    vp->value.str = rbuf;
	    اكسر;

	  حالة vpiOctStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" PRIo64, vlg_round_to_u64(val));
	    vp->value.str = rbuf;
	    اكسر;

	  حالة vpiHexStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" PRIx64, vlg_round_to_u64(val));
	    vp->value.str = rbuf;
	    اكسر;

	  حالة vpiBinStrVal: {
		uint64_t vali = vlg_round_to_u64(val);
		unsigned len = 0;

		طالما (vali > 0) {
		      len += 1;
		      vali /= 2;
		}

		vali = vlg_round_to_u64(val);
		لكل (unsigned idx = 0 ;  idx < len ;  idx += 1) {
		      rbuf[len-idx-1] = (vali & 1)? '1' : '0';
		      vali /= 2;
		}

		rbuf[len] = 0;
		إذا (len == 0) {
		      rbuf[0] = '0';
		      rbuf[1] = 0;
		}
		vp->value.str = rbuf;
		اكسر;
	  }

	  افتراضي:
	    fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiConstant (Real)\n", (صحيح)vp->format);

	    vp->format = vpiSuppressVal;
	    اكسر;
      }
}

مضمن __vpiVThrWord::__vpiVThrWord(unsigned base)
{
      name = vpip_name_string("W<>");
      subtype = vpiRealConst;
      index = base;
}

صحيح __vpiVThrWord::get_type_code(فراغ) ثابت
{ أرجع vpiConstant; }

صحيح __vpiVThrWord::vpi_get(صحيح code)
{ أرجع vthr_word_get(code, هذا); }

فراغ __vpiVThrWord::vpi_get_بەها(p_vpi_بەها val)
{ vthr_real_get_بەها(هذا, val); }

vpiHandle vpip_make_vthr_word(unsigned base, ثابت حرف*type)
{
      أكد(type[0] == 'r');
      أكد(base < 65536);
      __vpiVThrWord*obj = جديد __vpiVThrWord(base);

      أرجع obj;
}

#ifdef CHECK_WITH_VALGRIND
ساكن map<vpiHandle, منطقي> handle_map;

فراغ thread_word_delete(vpiHandle item)
{
      handle_map[item] = خطأ;
}

ساكن فراغ thread_word_delete_real(vpiHandle item)
{
      صنف __vpiVThrWord*obj = dynamic_cast<__vpiVThrWord*>(item);
      delete obj;
}
#endif

صنف __vpiVThrStrStack : عام __vpiHandle {
    عام:
      explicit __vpiVThrStrStack(unsigned depth);
      صحيح get_type_code(فراغ) ثابت override;
      صحيح vpi_get(صحيح code) override;
      فراغ vpi_get_بەها(p_vpi_بەها val) override;
    خاص:
      unsigned depth_;
};

__vpiVThrStrStack::__vpiVThrStrStack(unsigned d)
: depth_(d)
{
}

صحيح __vpiVThrStrStack::get_type_code(فراغ) ثابت
{ أرجع vpiConstant; }

صحيح __vpiVThrStrStack::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpiConstType:
	    أرجع vpiStringConst;
#إذا defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  حالة _vpiFromThr:
	    أرجع _vpiString;
#endif
	  حالة vpiSize:
	    أرجع vthread_get_str_کەڵەکە(vpip_current_vthread, depth_).size();
	  افتراضي:
	    أرجع 0;
      }
}

فراغ __vpiVThrStrStack::vpi_get_بەها(p_vpi_بەها vp)
{
      string val;
      حرف*rbuf = 0;

      إذا (vpip_current_vthread)
	    val = vthread_get_str_کەڵەکە(vpip_current_vthread, depth_);

      بدل (vp->format) {

	  حالة vpiObjTypeVal:
	    vp->format = vpiStringVal;
	    // fallthrough
	  حالة vpiStringVal:
	    rbuf = static_cast<حرف *>(need_result_buf(val.size()+1, RBUF_VAL));
	    strcpy(rbuf, val.c_str());
	    vp->value.str = rbuf;
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiConstant (String)\n", (صحيح)vp->format);

	    vp->format = vpiSuppressVal;
	    اكسر;
      }
}

صنف __vpiVThrVec4Stack : عام __vpiHandle {
    عام:
      __vpiVThrVec4Stack(unsigned depth, منطقي signed_flag, unsigned wid);
      صحيح get_type_code(فراغ) ثابت override;
      صحيح vpi_get(صحيح code) override;
      حرف*vpi_get_str(صحيح code) override;
      فراغ vpi_get_بەها(p_vpi_بەها val) override;
      vpiHandle vpi_put_بەها(p_vpi_بەها val, صحيح flags) override;
    خاص:
      ساكن فراغ vpi_get_بەها_string(p_vpi_بەها vp, ثابت vvp_vector4_t&val);
      ساكن فراغ vpi_get_بەها_binstr(p_vpi_بەها vp, ثابت vvp_vector4_t&val);
      فراغ vpi_get_بەها_decstr(p_vpi_بەها vp, ثابت vvp_vector4_t&val);
      فراغ vpi_get_بەها_int   (p_vpi_بەها vp, ثابت vvp_vector4_t&val);
      ساكن فراغ vpi_get_بەها_real  (p_vpi_بەها vp, ثابت vvp_vector4_t&val);
      ساكن فراغ vpi_get_بەها_strength(p_vpi_بەها vp, ثابت vvp_vector4_t&val);
      ساكن فراغ vpi_get_بەها_octstr(p_vpi_بەها vp, ثابت vvp_vector4_t&val);
      ساكن فراغ vpi_get_بەها_hexstr(p_vpi_بەها vp, ثابت vvp_vector4_t&val);
      ساكن فراغ vpi_get_بەها_vector(p_vpi_بەها vp, ثابت vvp_vector4_t&val);
    خاص:
      unsigned depth_;
      منطقي signed_flag_;
      unsigned expect_width_;
      ثابت حرف*name;
};

__vpiVThrVec4Stack::__vpiVThrVec4Stack(unsigned d, منطقي sf, unsigned wid)
: depth_(d), signed_flag_(sf), expect_width_(wid)
{
      name = vpip_name_string("S<,vec4,>");
}

صحيح __vpiVThrVec4Stack::get_type_code(فراغ) ثابت
{ أرجع vpiConstant; }


صحيح __vpiVThrVec4Stack::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpiSize:
	    أرجع expect_width_;

	  حالة vpiSigned:
	    أرجع signed_flag_? 1 : 0;

	  حالة vpiConstType:
	    أرجع vpi_دووانەیی_Const;

#إذا defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  حالة _vpiFromThr:
	    أرجع _vpiVThr;
#endif

	  افتراضي:
	    أرجع 0;
      }
}

حرف*__vpiVThrVec4Stack::vpi_get_str(صحيح code)
{
      بدل (code) {
	  حالة vpiFullName:
	    أرجع simple_set_rbuf_str(name);

	  افتراضي:
	    أرجع 0;
      }
}

فراغ __vpiVThrVec4Stack::vpi_get_بەها(p_vpi_بەها vp)
{
      vvp_vector4_t val;

      إذا (vpip_current_vthread)
	    val = vthread_get_vec4_کەڵەکە(vpip_current_vthread, depth_);

      بدل (vp->format) {

	  حالة vpiBinStrVal:
	    vpi_get_بەها_binstr(vp, val);
	    اكسر;
	  حالة vpiDecStrVal:
	    vpi_get_بەها_decstr(vp, val);
	    اكسر;
	  حالة vpiOctStrVal:
	    vpi_get_بەها_octstr(vp, val);
	    اكسر;
	  حالة vpiHexStrVal:
	    vpi_get_بەها_hexstr(vp, val);
	    اكسر;
	  حالة vpiIntVal:
	    vpi_get_بەها_int(vp, val);
	    اكسر;
	  حالة vpiRealVal:
	    vpi_get_بەها_real(vp, val);
	    اكسر;
	  حالة vpiStringVal:
	    vpi_get_بەها_string(vp, val);
	    اكسر;
	  حالة vpiStrengthVal:
	    vpi_get_بەها_strength(vp, val);
	    اكسر;
	  حالة vpiObjTypeVal:
	    vp->format = vpiVectorVal;
	    // fallthrough
	  حالة vpiVectorVal:
	    vpi_get_بەها_vector(vp, val);
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "internal error: vpi_get_value(<format=%d>)"
		    " not implemented for __vpiVThrVec4Stack.\n", vp->format);
	    أكد(0);
      }

}

فراغ __vpiVThrVec4Stack::vpi_get_بەها_binstr(p_vpi_بەها vp, ثابت vvp_vector4_t&val)
{
      unsigned wid = val.size();
      حرف*rbuf = static_cast<حرف*>(need_result_buf(wid+1, RBUF_VAL));
      لكل (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    rbuf[wid-idx-1] = vvp_bit4_to_ascii(val.value(idx));
      }
      rbuf[wid] = 0;
      vp->value.str = rbuf;
}

فراغ __vpiVThrVec4Stack::vpi_get_بەها_decstr(p_vpi_بەها vp, ثابت vvp_vector4_t&val)
{
      unsigned wid = val.size();
      صحيح nbuf = (wid+2)/3 + 1;
      حرف *rbuf = static_cast<حرف*>(need_result_buf(nbuf, RBUF_VAL));

      vpip_vec4_to_dec_str(val, rbuf, nbuf, signed_flag_);
      vp->value.str = rbuf;
}

فراغ __vpiVThrVec4Stack::vpi_get_بەها_octstr(p_vpi_بەها vp, ثابت vvp_vector4_t&val)
{
      unsigned wid = val.size();
      unsigned owid = (wid + 2) / 3;
      حرف*rbuf = static_cast<حرف*>(need_result_buf(owid+1, RBUF_VAL));
      rbuf[owid] = 0;

      unsigned oval = 0;
      لكل (unsigned idx = 0; idx < wid ; idx += 1) {
	    unsigned tmp = 0;
	    بدل (val.value(idx)) {
		حالة BIT4_0:
		  tmp = 0;
		  اكسر;
		حالة BIT4_1:
		  tmp = 1;
		  اكسر;
		حالة BIT4_X:
		  tmp = 2;
		  اكسر;
		حالة BIT4_Z:
		  tmp = 3;
		  اكسر;
	    }

	    oval = oval | (tmp << 2*(idx%3));

	    إذا (idx%3 == 2) {
		  owid -= 1;
		  rbuf[owid] = oct_digits[oval];
		  oval = 0;
	    }
      }

      إذا (owid > 0) {
	    owid -= 1;
	    rbuf[owid] = oct_digits[oval];
      }
      vp->value.str = rbuf;
}

فراغ __vpiVThrVec4Stack::vpi_get_بەها_hexstr(p_vpi_بەها vp, ثابت vvp_vector4_t&val)
{
      unsigned wid = val.size();
      unsigned hwid = (wid + 3) /4;
      حرف*rbuf = static_cast<حرف*>(need_result_buf(hwid+1, RBUF_VAL));
      rbuf[hwid] = 0;

      unsigned hval = 0;
      لكل (unsigned idx = 0; idx < wid ; idx += 1) {
	    unsigned tmp = 0;
	    بدل (val.value(idx)) {
		حالة BIT4_0:
		  tmp = 0;
		  اكسر;
		حالة BIT4_1:
		  tmp = 1;
		  اكسر;
		حالة BIT4_X:
		  tmp = 2;
		  اكسر;
		حالة BIT4_Z:
		  tmp = 3;
		  اكسر;
	    }
	    hval = hval | (tmp << 2*(idx%4));

	    إذا (idx%4 == 3) {
		  hwid -= 1;
		  rbuf[hwid] = hex_digits[hval];
		  hval = 0;
	    }
      }

      إذا (hwid > 0) {
	    hwid -= 1;
	    rbuf[hwid] = hex_digits[hval];
      }
      vp->value.str = rbuf;
}

فراغ __vpiVThrVec4Stack::vpi_get_بەها_int(p_vpi_بەها vp, ثابت vvp_vector4_t&val)
{
      int32_t vali = 0;
      vector4_to_بەها(val, vali, signed_flag_, خطأ);
      vp->value.integer = vali;
}

فراغ __vpiVThrVec4Stack::vpi_get_بەها_real(p_vpi_بەها vp, ثابت vvp_vector4_t&val)
{
      unsigned wid = val.size();
      vp->value.real = 0.0;

      لكل (unsigned idx = wid ; idx > 0 ; idx -= 1) {
	    vp->value.real *= 2.0;
	    إذا (val.value(idx-1) == BIT4_1)
		  vp->value.real += 1.0;
      }
}

فراغ __vpiVThrVec4Stack::vpi_get_بەها_string(p_vpi_بەها vp, ثابت vvp_vector4_t&val)
{
      حرف*rbuf = static_cast<حرف*>(need_result_buf((val.size() / 8) + 1, RBUF_VAL));
      حرف*cp = rbuf;

      حرف tmp = 0;
      لكل (صحيح bitnr = val.size()-1 ; bitnr >= 0 ; bitnr -= 1) {
	    tmp <<= 1;
	    بدل (val.value(bitnr)) {
		حالة BIT4_1:
		  tmp |= 1;
		  اكسر;
		حالة BIT4_0:
		افتراضي:
		  اكسر;
	    }

	    إذا ((bitnr&7)==0) {
		    // Don't include leading nuls
		  إذا (tmp == 0 && cp == rbuf)
			استمر;

		  *cp++ = tmp? tmp : ' ';
		  tmp = 0;
	    }
      }
      *cp++ = 0;
      vp->format = vpiStringVal;
      vp->value.str = rbuf;
}

فراغ __vpiVThrVec4Stack::vpi_get_بەها_vector(p_vpi_بەها vp, ثابت vvp_vector4_t&val)
{
      unsigned wid = val.size();

      unsigned hwid = (wid+31)/32;
      vp->value.vector = static_cast<s_vpi_vecval*>
                         (need_result_buf(hwid*حجم(s_vpi_vecval), RBUF_VAL));
      أكد(vp->value.vector);

	/* Zero the top of the result buffer first since the fill loop below only writes
	   bits 0..wid-1, which can leave stale data in the unused high bits. */
      إذا (hwid == 0) hwid = 1;
      vp->value.vector[hwid-1].aval = 0;
      vp->value.vector[hwid-1].bval = 0;

      لكل (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
	    صحيح word = idx/32;
	    PLI_INT32 mask = 1 << (idx%32);

	    بدل (val.value(idx)) {
		حالة BIT4_0:
		  vp->value.vector[word].aval &= ~mask;
		  vp->value.vector[word].bval &= ~mask;
		  اكسر;
		حالة BIT4_1:
		  vp->value.vector[word].aval |=  mask;
		  vp->value.vector[word].bval &= ~mask;
		  اكسر;
		حالة BIT4_X:
		  vp->value.vector[word].aval |=  mask;
		  vp->value.vector[word].bval |=  mask;
		  اكسر;
		حالة BIT4_Z:
		  vp->value.vector[word].aval &= ~mask;
		  vp->value.vector[word].bval |=  mask;
		  اكسر;
	    }
      }
}

فراغ __vpiVThrVec4Stack::vpi_get_بەها_strength(p_vpi_بەها vp, ثابت vvp_vector4_t&val)
{
      s_vpi_strengthval*op = static_cast<s_vpi_strengthval*>
                             (need_result_buf(val.size() * حجم(s_vpi_strengthval),
                                              RBUF_VAL));

      لكل (unsigned idx = 0 ; idx < val.size() ; idx += 1) {
	    بدل (val.value(idx)) {
		حالة BIT4_0:
		  op[idx].logic = vpi0;
		  op[idx].s0 = vpiStrongDrive;
		  op[idx].s1 = 0;
		  اكسر;
		حالة BIT4_1:
		  op[idx].logic = vpi1;
		  op[idx].s0 = 0;
		  op[idx].s1 = vpiStrongDrive;
		  اكسر;
		حالة BIT4_X:
		  op[idx].logic = vpiX;
		  op[idx].s0 = vpiStrongDrive;
		  op[idx].s1 = vpiStrongDrive;
		  اكسر;
		حالة BIT4_Z:
		  op[idx].logic = vpiZ;
		  op[idx].s0 = vpiHiZ;
		  op[idx].s1 = vpiHiZ;
		  اكسر;
	    }
      }

      vp->format = vpiStrengthVal;
      vp->value.strength = op;
}

vpiHandle __vpiVThrVec4Stack::vpi_put_بەها(p_vpi_بەها vp, صحيح /*flags*/)
{
      أكد(vpip_current_vthread);

      بدل (vp->format) {

	  افتراضي:
	    fprintf(stderr, "internal error: vpi_put_value(<format=%d>)"
		    " not implemented for __vpiVThrVec4Stack.\n", vp->format);
	    أكد(0);
	    أرجع 0;
      }
}

#ifdef CHECK_WITH_VALGRIND
فراغ thread_vthr_delete(vpiHandle item)
{
      handle_map[item] = true;
}

ساكن فراغ thread_vthr_delete_real(vpiHandle item)
{
      __vpiVThrVec4Stack*obj = dynamic_cast<__vpiVThrVec4Stack*>(item);
      delete obj;
}

فراغ vpi_handle_delete()
{
      map<vpiHandle, منطقي>::iterator iter;
      لكل (iter = handle_map.begin(); iter != handle_map.end(); ++ iter ) {
	    إذا (iter->second) thread_vthr_delete_real(iter->first);
	    وإلا thread_word_delete_real(iter->first);
      }
}
#endif


vpiHandle vpip_make_vthr_str_کەڵەکە(unsigned depth)
{
      __vpiVThrStrStack*obj = جديد __vpiVThrStrStack(depth);
      أرجع obj;
}

vpiHandle vpip_make_vthr_vec4_کەڵەکە(unsigned depth, منطقي signed_flag, unsigned wid)
{
      __vpiVThrVec4Stack*obj = جديد __vpiVThrVec4Stack(depth, signed_flag, wid);
      أرجع obj;
}

#ifdef CHECK_WITH_VALGRIND
ساكن map<vpiHandle, منطقي> کەڵەکە_map;

فراغ thread_string_delete(vpiHandle item)
{
      کەڵەکە_map[item] = خطأ;
}

ساكن فراغ thread_string_delete_real(vpiHandle item)
{
      __vpiVThrStrStack*obj = dynamic_cast<__vpiVThrStrStack*>(item);
      delete obj;
}

فراغ vpi_کەڵەکە_delete()
{
      map<vpiHandle, منطقي>::iterator iter;
      لكل (iter = کەڵەکە_map.begin(); iter != کەڵەکە_map.end(); ++ iter ) {
	    thread_string_delete_real(iter->first);
      }
}
#endif
