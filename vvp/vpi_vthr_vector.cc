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
#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
# include <map>
#جيڪڏهن_پورو
# include  <cstdio>
# include  <cstdlib>
# include  <cassert>
# include  "ivl_alloc.h"

استعمال نطاق std;

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
      واضح __vpiVThrWord(بنانشان بنياد);
      صحيح get_type_ڪوڊ(فراغ) ثابت override;
      صحيح vpi_get(صحيح code) override;
      فراغ vpi_get_قدر(p_vpi_قدر val) override;

      صحيح get_subtype() ثابت { أرجع subtype; };
      بنانشان get_اشاريو() ثابت { أرجع اشاريو; };

    خاص:
      ثابت حرف* name;
      صحيح subtype;
      بنانشان اشاريو;
};

ساكن صحيح vthr_لفظ_get(صحيح code, vpiHandle ref)
{
      ثابت __vpiVThrWord*rfp = dynamic_cast<__vpiVThrWord*>(ref);

      بدل (code) {

	  حالة vpiConstType:
	    أرجع rfp->get_subtype();

#إذا defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  حالة _vpiFromThr:
	    أرجع vpi_لفظ;
#جيڪڏهن_پورو

	  افتراضي:
	    أرجع 0;
      }
}

ساكن فراغ vthr_real_get_قدر(vpiHandle ref, s_vpi_قدر*vp)
{
      ثابت __vpiVThrWord*obj = dynamic_cast<__vpiVThrWord*>(ref);
      ساكن ثابت size_t RBUF_USE_سائيز = 66;
      حرف *rbuf = static_cast<حرف *>(need_نتيجو_buf(RBUF_USE_سائيز, RBUF_VAL));

      مزدوج val = 0.0;

	/* Get the actual value from the index. It is possible, by the
	   way, that the vpi_get_value is called from compiletf. If
	   that's the case, there will be no current thread, and this
	   will not have access to the proper value. Punt and return a
	   0.0 value instead. */
      إذا (vpip_current_vthread)
	    val = vthread_get_real_ڍير(vpip_current_vthread,
	                                 obj->get_اشاريو());

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
		  val = std::ڦيرو_2(val);
	    }
	    vp->value.integer = (PLI_INT32)val;
	    اكسر;

	  حالة vpiDecStrVal:
	    إذا (std::isnan(val))
		  snprintf(rbuf, RBUF_USE_سائيز, "%s", "nan");
	    وإلا
		  snprintf(rbuf, RBUF_USE_سائيز, "%0.0f", std::ڦيرو_2(val));
	    vp->value.str = rbuf;
	    اكسر;

	  حالة vpiOctStrVal:
	    snprintf(rbuf, RBUF_USE_سائيز, "%" PRIo64, vlg_ڦيرو_منزل_u64(val));
	    vp->value.str = rbuf;
	    اكسر;

	  حالة vpiHexStrVal:
	    snprintf(rbuf, RBUF_USE_سائيز, "%" PRIx64, vlg_ڦيرو_منزل_u64(val));
	    vp->value.str = rbuf;
	    اكسر;

	  حالة vpiBinStrVal: {
		uint64_t vali = vlg_ڦيرو_منزل_u64(val);
		بنانشان len = 0;

		طالما (vali > 0) {
		      len += 1;
		      vali /= 2;
		}

		vali = vlg_ڦيرو_منزل_u64(val);
		لكل (بنانشان idx = 0 ;  idx < len ;  idx += 1) {
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

مضمن __vpiVThrWord::__vpiVThrWord(بنانشان بنياد)
{
      name = vpip_name_string("W<>");
      subtype = vpiRealConst;
      اشاريو = بنياد;
}

صحيح __vpiVThrWord::get_type_ڪوڊ(فراغ) ثابت
{ أرجع vpiConstant; }

صحيح __vpiVThrWord::vpi_get(صحيح code)
{ أرجع vthr_لفظ_get(code, هذا); }

فراغ __vpiVThrWord::vpi_get_قدر(p_vpi_قدر val)
{ vthr_real_get_قدر(هذا, val); }

vpiHandle vpip_make_vthr_لفظ(بنانشان بنياد, ثابت حرف*type)
{
      أكد(type[0] == 'r');
      أكد(بنياد < 65536);
      __vpiVThrWord*obj = جديد __vpiVThrWord(بنياد);

      أرجع obj;
}

#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
ساكن map<vpiHandle, منطقي> handle_map;

فراغ thread_لفظ_delete(vpiHandle شي)
{
      handle_map[شي] = خطأ;
}

ساكن فراغ thread_لفظ_delete_real(vpiHandle شي)
{
      صنف __vpiVThrWord*obj = dynamic_cast<__vpiVThrWord*>(شي);
      ميٽيو obj;
}
#جيڪڏهن_پورو

صنف __vpiVThrStrStack : عام __vpiHandle {
    عام:
      واضح __vpiVThrStrStack(بنانشان اونهائي);
      صحيح get_type_ڪوڊ(فراغ) ثابت override;
      صحيح vpi_get(صحيح code) override;
      فراغ vpi_get_قدر(p_vpi_قدر val) override;
    خاص:
      بنانشان اونهائي_2;
};

__vpiVThrStrStack::__vpiVThrStrStack(بنانشان d)
: اونهائي_2(d)
{
}

صحيح __vpiVThrStrStack::get_type_ڪوڊ(فراغ) ثابت
{ أرجع vpiConstant; }

صحيح __vpiVThrStrStack::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpiConstType:
	    أرجع vpiStringConst;
#إذا defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  حالة _vpiFromThr:
	    أرجع _vpiString;
#جيڪڏهن_پورو
	  حالة vpi_سائيز:
	    أرجع vthread_get_str_ڍير(vpip_current_vthread, اونهائي_2).size();
	  افتراضي:
	    أرجع 0;
      }
}

فراغ __vpiVThrStrStack::vpi_get_قدر(p_vpi_قدر vp)
{
      string val;
      حرف*rbuf = 0;

      إذا (vpip_current_vthread)
	    val = vthread_get_str_ڍير(vpip_current_vthread, اونهائي_2);

      بدل (vp->format) {

	  حالة vpiObjTypeVal:
	    vp->format = vpiStringVal;
	    // fallthrough
	  حالة vpiStringVal:
	    rbuf = static_cast<حرف *>(need_نتيجو_buf(val.size()+1, RBUF_VAL));
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
      __vpiVThrVec4Stack(بنانشان اونهائي, منطقي signed_flag, بنانشان wid);
      صحيح get_type_ڪوڊ(فراغ) ثابت override;
      صحيح vpi_get(صحيح code) override;
      حرف*vpi_get_str(صحيح code) override;
      فراغ vpi_get_قدر(p_vpi_قدر val) override;
      vpiHandle vpi_put_قدر(p_vpi_قدر val, صحيح flags) override;
    خاص:
      ساكن فراغ vpi_get_قدر_string(p_vpi_قدر vp, ثابت vvp_vector4_t&val);
      ساكن فراغ vpi_get_قدر_binstr(p_vpi_قدر vp, ثابت vvp_vector4_t&val);
      فراغ vpi_get_قدر_decstr(p_vpi_قدر vp, ثابت vvp_vector4_t&val);
      فراغ vpi_get_قدر_int   (p_vpi_قدر vp, ثابت vvp_vector4_t&val);
      ساكن فراغ vpi_get_قدر_real  (p_vpi_قدر vp, ثابت vvp_vector4_t&val);
      ساكن فراغ vpi_get_قدر_strength(p_vpi_قدر vp, ثابت vvp_vector4_t&val);
      ساكن فراغ vpi_get_قدر_octstr(p_vpi_قدر vp, ثابت vvp_vector4_t&val);
      ساكن فراغ vpi_get_قدر_hexstr(p_vpi_قدر vp, ثابت vvp_vector4_t&val);
      ساكن فراغ vpi_get_قدر_ويڪٽر(p_vpi_قدر vp, ثابت vvp_vector4_t&val);
    خاص:
      بنانشان اونهائي_2;
      منطقي signed_flag_;
      بنانشان expect_ويڪر;
      ثابت حرف*name;
};

__vpiVThrVec4Stack::__vpiVThrVec4Stack(بنانشان d, منطقي sf, بنانشان wid)
: اونهائي_2(d), signed_flag_(sf), expect_ويڪر(wid)
{
      name = vpip_name_string("S<,vec4,>");
}

صحيح __vpiVThrVec4Stack::get_type_ڪوڊ(فراغ) ثابت
{ أرجع vpiConstant; }


صحيح __vpiVThrVec4Stack::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpi_سائيز:
	    أرجع expect_ويڪر;

	  حالة vpiSigned:
	    أرجع signed_flag_? 1 : 0;

	  حالة vpiConstType:
	    أرجع vpi_ٻٽو_Const;

#إذا defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  حالة _vpiFromThr:
	    أرجع _vpiVThr;
#جيڪڏهن_پورو

	  افتراضي:
	    أرجع 0;
      }
}

حرف*__vpiVThrVec4Stack::vpi_get_str(صحيح code)
{
      بدل (code) {
	  حالة vpi_ڀريل_Name:
	    أرجع simple_سيٽ_rbuf_str(name);

	  افتراضي:
	    أرجع 0;
      }
}

فراغ __vpiVThrVec4Stack::vpi_get_قدر(p_vpi_قدر vp)
{
      vvp_vector4_t val;

      إذا (vpip_current_vthread)
	    val = vthread_get_vec4_ڍير(vpip_current_vthread, اونهائي_2);

      بدل (vp->format) {

	  حالة vpiBinStrVal:
	    vpi_get_قدر_binstr(vp, val);
	    اكسر;
	  حالة vpiDecStrVal:
	    vpi_get_قدر_decstr(vp, val);
	    اكسر;
	  حالة vpiOctStrVal:
	    vpi_get_قدر_octstr(vp, val);
	    اكسر;
	  حالة vpiHexStrVal:
	    vpi_get_قدر_hexstr(vp, val);
	    اكسر;
	  حالة vpiIntVal:
	    vpi_get_قدر_int(vp, val);
	    اكسر;
	  حالة vpiRealVal:
	    vpi_get_قدر_real(vp, val);
	    اكسر;
	  حالة vpiStringVal:
	    vpi_get_قدر_string(vp, val);
	    اكسر;
	  حالة vpiStrengthVal:
	    vpi_get_قدر_strength(vp, val);
	    اكسر;
	  حالة vpiObjTypeVal:
	    vp->format = vpi_ويڪٽر_Val;
	    // fallthrough
	  حالة vpi_ويڪٽر_Val:
	    vpi_get_قدر_ويڪٽر(vp, val);
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "internal error: vpi_get_value(<format=%d>)"
		    " not implemented for __vpiVThrVec4Stack.\n", vp->format);
	    أكد(0);
      }

}

فراغ __vpiVThrVec4Stack::vpi_get_قدر_binstr(p_vpi_قدر vp, ثابت vvp_vector4_t&val)
{
      بنانشان wid = val.size();
      حرف*rbuf = static_cast<حرف*>(need_نتيجو_buf(wid+1, RBUF_VAL));
      لكل (بنانشان idx = 0 ; idx < wid ; idx += 1) {
	    rbuf[wid-idx-1] = vvp_bit4_منزل_ascii(val.value(idx));
      }
      rbuf[wid] = 0;
      vp->value.str = rbuf;
}

فراغ __vpiVThrVec4Stack::vpi_get_قدر_decstr(p_vpi_قدر vp, ثابت vvp_vector4_t&val)
{
      بنانشان wid = val.size();
      صحيح nbuf = (wid+2)/3 + 1;
      حرف *rbuf = static_cast<حرف*>(need_نتيجو_buf(nbuf, RBUF_VAL));

      vpip_vec4_منزل_dec_str(val, rbuf, nbuf, signed_flag_);
      vp->value.str = rbuf;
}

فراغ __vpiVThrVec4Stack::vpi_get_قدر_octstr(p_vpi_قدر vp, ثابت vvp_vector4_t&val)
{
      بنانشان wid = val.size();
      بنانشان owid = (wid + 2) / 3;
      حرف*rbuf = static_cast<حرف*>(need_نتيجو_buf(owid+1, RBUF_VAL));
      rbuf[owid] = 0;

      بنانشان oval = 0;
      لكل (بنانشان idx = 0; idx < wid ; idx += 1) {
	    بنانشان tmp = 0;
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

فراغ __vpiVThrVec4Stack::vpi_get_قدر_hexstr(p_vpi_قدر vp, ثابت vvp_vector4_t&val)
{
      بنانشان wid = val.size();
      بنانشان hwid = (wid + 3) /4;
      حرف*rbuf = static_cast<حرف*>(need_نتيجو_buf(hwid+1, RBUF_VAL));
      rbuf[hwid] = 0;

      بنانشان hval = 0;
      لكل (بنانشان idx = 0; idx < wid ; idx += 1) {
	    بنانشان tmp = 0;
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

فراغ __vpiVThrVec4Stack::vpi_get_قدر_int(p_vpi_قدر vp, ثابت vvp_vector4_t&val)
{
      int32_t vali = 0;
      vector4_منزل_قدر(val, vali, signed_flag_, خطأ);
      vp->value.integer = vali;
}

فراغ __vpiVThrVec4Stack::vpi_get_قدر_real(p_vpi_قدر vp, ثابت vvp_vector4_t&val)
{
      بنانشان wid = val.size();
      vp->value.real = 0.0;

      لكل (بنانشان idx = wid ; idx > 0 ; idx -= 1) {
	    vp->value.real *= 2.0;
	    إذا (val.value(idx-1) == BIT4_1)
		  vp->value.real += 1.0;
      }
}

فراغ __vpiVThrVec4Stack::vpi_get_قدر_string(p_vpi_قدر vp, ثابت vvp_vector4_t&val)
{
      حرف*rbuf = static_cast<حرف*>(need_نتيجو_buf((val.size() / 8) + 1, RBUF_VAL));
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

فراغ __vpiVThrVec4Stack::vpi_get_قدر_ويڪٽر(p_vpi_قدر vp, ثابت vvp_vector4_t&val)
{
      بنانشان wid = val.size();

      بنانشان hwid = (wid+31)/32;
      vp->value.vector = static_cast<s_vpi_vecval*>
                         (need_نتيجو_buf(hwid*حجم(s_vpi_vecval), RBUF_VAL));
      أكد(vp->value.vector);

	/* Zero the top of the result buffer first since the fill loop below only writes
	   bits 0..wid-1, which can leave stale data in the unused high bits. */
      إذا (hwid == 0) hwid = 1;
      vp->value.vector[hwid-1].aval = 0;
      vp->value.vector[hwid-1].bval = 0;

      لكل (بنانشان idx = 0 ;  idx < wid ;  idx += 1) {
	    صحيح لفظ_2 = idx/32;
	    PLI_INT32 نقاب = 1 << (idx%32);

	    بدل (val.value(idx)) {
		حالة BIT4_0:
		  vp->value.vector[لفظ_2].aval &= ~نقاب;
		  vp->value.vector[لفظ_2].bval &= ~نقاب;
		  اكسر;
		حالة BIT4_1:
		  vp->value.vector[لفظ_2].aval |=  نقاب;
		  vp->value.vector[لفظ_2].bval &= ~نقاب;
		  اكسر;
		حالة BIT4_X:
		  vp->value.vector[لفظ_2].aval |=  نقاب;
		  vp->value.vector[لفظ_2].bval |=  نقاب;
		  اكسر;
		حالة BIT4_Z:
		  vp->value.vector[لفظ_2].aval &= ~نقاب;
		  vp->value.vector[لفظ_2].bval |=  نقاب;
		  اكسر;
	    }
      }
}

فراغ __vpiVThrVec4Stack::vpi_get_قدر_strength(p_vpi_قدر vp, ثابت vvp_vector4_t&val)
{
      s_vpi_strengthval*op = static_cast<s_vpi_strengthval*>
                             (need_نتيجو_buf(val.size() * حجم(s_vpi_strengthval),
                                              RBUF_VAL));

      لكل (بنانشان idx = 0 ; idx < val.size() ; idx += 1) {
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

vpiHandle __vpiVThrVec4Stack::vpi_put_قدر(p_vpi_قدر vp, صحيح /*flags*/)
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

#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
فراغ thread_vthr_delete(vpiHandle شي)
{
      handle_map[شي] = سچ;
}

ساكن فراغ thread_vthr_delete_real(vpiHandle شي)
{
      __vpiVThrVec4Stack*obj = dynamic_cast<__vpiVThrVec4Stack*>(شي);
      ميٽيو obj;
}

فراغ vpi_handle_delete()
{
      map<vpiHandle, منطقي>::iterator iter;
      لكل (iter = handle_map.begin(); iter != handle_map.پڇاڙي(); ++ iter ) {
	    إذا (iter->second) thread_vthr_delete_real(iter->پهريون);
	    وإلا thread_لفظ_delete_real(iter->پهريون);
      }
}
#جيڪڏهن_پورو


vpiHandle vpip_make_vthr_str_ڍير(بنانشان اونهائي)
{
      __vpiVThrStrStack*obj = جديد __vpiVThrStrStack(اونهائي);
      أرجع obj;
}

vpiHandle vpip_make_vthr_vec4_ڍير(بنانشان اونهائي, منطقي signed_flag, بنانشان wid)
{
      __vpiVThrVec4Stack*obj = جديد __vpiVThrVec4Stack(اونهائي, signed_flag, wid);
      أرجع obj;
}

#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
ساكن map<vpiHandle, منطقي> ڍير_map;

فراغ thread_string_delete(vpiHandle شي)
{
      ڍير_map[شي] = خطأ;
}

ساكن فراغ thread_string_delete_real(vpiHandle شي)
{
      __vpiVThrStrStack*obj = dynamic_cast<__vpiVThrStrStack*>(شي);
      ميٽيو obj;
}

فراغ vpi_ڍير_delete()
{
      map<vpiHandle, منطقي>::iterator iter;
      لكل (iter = ڍير_map.begin(); iter != ڍير_map.پڇاڙي(); ++ iter ) {
	    thread_string_delete_real(iter->پهريون);
      }
}
#جيڪڏهن_پورو
