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

using فضاینام std;

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
بیرونی ثابت نویسه hex_digits[256];
بیرونی ثابت نویسه oct_digits[64];


رده __vpiVThrWord : عمومی __vpiHandle {
    عمومی:
      explicit __vpiVThrWord(unsigned base);
      صحیح get_type_code(تهی) ثابت override;
      صحیح vpi_get(صحیح code) override;
      تهی vpi_get_مقدار(p_vpi_مقدار val) override;

      صحیح get_subtype() ثابت { بازگشت subtype; };
      unsigned get_index() ثابت { بازگشت index; };

    خصوصی:
      ثابت نویسه* name;
      صحیح subtype;
      unsigned index;
};

ایستا صحیح vthr_word_get(صحیح code, vpiHandle ref)
{
      ثابت __vpiVThrWord*rfp = dynamic_cast<__vpiVThrWord*>(ref);

      انتخاب (code) {

	  حالت vpiConstType:
	    بازگشت rfp->get_subtype();

#اگر defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  حالت _vpiFromThr:
	    بازگشت _vpiWord;
#endif

	  پیشفرض:
	    بازگشت 0;
      }
}

ایستا تهی vthr_real_get_مقدار(vpiHandle ref, s_vpi_مقدار*vp)
{
      ثابت __vpiVThrWord*obj = dynamic_cast<__vpiVThrWord*>(ref);
      ایستا ثابت size_t RBUF_USE_SIZE = 66;
      نویسه *rbuf = static_cast<نویسه *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));

      دوگانه val = 0.0;

	/* Get the actual value from the index. It is possible, by the
	   way, that the vpi_get_value is called from compiletf. If
	   that's the case, there will be no current thread, and this
	   will not have access to the proper value. Punt and return a
	   0.0 value instead. */
      اگر (vpip_current_vthread)
	    val = vthread_get_real_پشته(vpip_current_vthread,
	                                 obj->get_index());

      انتخاب (vp->format) {

	  حالت vpiObjTypeVal:
	    vp->format = vpiRealVal;
	    // fallthrough
	  حالت vpiRealVal:
	    vp->value.real = val;
	    بشکن;

	  حالت vpiIntVal:
	      /* NaN or +/- infinity are translated as 0. */
	    اگر (val != val || (val && (val == 0.5*val))) {
		  val = 0.0;
	    } وگرنه {
		  val = std::round(val);
	    }
	    vp->value.integer = (PLI_INT32)val;
	    بشکن;

	  حالت vpiDecStrVal:
	    اگر (std::isnan(val))
		  snprintf(rbuf, RBUF_USE_SIZE, "%s", "nan");
	    وگرنه
		  snprintf(rbuf, RBUF_USE_SIZE, "%0.0f", std::round(val));
	    vp->value.str = rbuf;
	    بشکن;

	  حالت vpiOctStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" PRIo64, vlg_round_to_u64(val));
	    vp->value.str = rbuf;
	    بشکن;

	  حالت vpiHexStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" PRIx64, vlg_round_to_u64(val));
	    vp->value.str = rbuf;
	    بشکن;

	  حالت vpiBinStrVal: {
		uint64_t vali = vlg_round_to_u64(val);
		unsigned len = 0;

		وقتی (vali > 0) {
		      len += 1;
		      vali /= 2;
		}

		vali = vlg_round_to_u64(val);
		برای (unsigned idx = 0 ;  idx < len ;  idx += 1) {
		      rbuf[len-idx-1] = (vali & 1)? '1' : '0';
		      vali /= 2;
		}

		rbuf[len] = 0;
		اگر (len == 0) {
		      rbuf[0] = '0';
		      rbuf[1] = 0;
		}
		vp->value.str = rbuf;
		بشکن;
	  }

	  پیشفرض:
	    fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiConstant (Real)\n", (صحیح)vp->format);

	    vp->format = vpiSuppressVal;
	    بشکن;
      }
}

درونخطی __vpiVThrWord::__vpiVThrWord(unsigned base)
{
      name = vpip_name_string("W<>");
      subtype = vpiRealConst;
      index = base;
}

صحیح __vpiVThrWord::get_type_code(تهی) ثابت
{ بازگشت vpiConstant; }

صحیح __vpiVThrWord::vpi_get(صحیح code)
{ بازگشت vthr_word_get(code, این); }

تهی __vpiVThrWord::vpi_get_مقدار(p_vpi_مقدار val)
{ vthr_real_get_مقدار(این, val); }

vpiHandle vpip_make_vthr_word(unsigned base, ثابت نویسه*type)
{
      أكد(type[0] == 'r');
      أكد(base < 65536);
      __vpiVThrWord*obj = جدید __vpiVThrWord(base);

      بازگشت obj;
}

#ifdef CHECK_WITH_VALGRIND
ایستا map<vpiHandle, منطقی> handle_map;

تهی thread_word_delete(vpiHandle item)
{
      handle_map[item] = نادرست;
}

ایستا تهی thread_word_delete_real(vpiHandle item)
{
      رده __vpiVThrWord*obj = dynamic_cast<__vpiVThrWord*>(item);
      delete obj;
}
#endif

رده __vpiVThrStrStack : عمومی __vpiHandle {
    عمومی:
      explicit __vpiVThrStrStack(unsigned depth);
      صحیح get_type_code(تهی) ثابت override;
      صحیح vpi_get(صحیح code) override;
      تهی vpi_get_مقدار(p_vpi_مقدار val) override;
    خصوصی:
      unsigned depth_;
};

__vpiVThrStrStack::__vpiVThrStrStack(unsigned d)
: depth_(d)
{
}

صحیح __vpiVThrStrStack::get_type_code(تهی) ثابت
{ بازگشت vpiConstant; }

صحیح __vpiVThrStrStack::vpi_get(صحیح code)
{
      انتخاب (code) {
	  حالت vpiConstType:
	    بازگشت vpiStringConst;
#اگر defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  حالت _vpiFromThr:
	    بازگشت _vpiString;
#endif
	  حالت vpiSize:
	    بازگشت vthread_get_str_پشته(vpip_current_vthread, depth_).size();
	  پیشفرض:
	    بازگشت 0;
      }
}

تهی __vpiVThrStrStack::vpi_get_مقدار(p_vpi_مقدار vp)
{
      string val;
      نویسه*rbuf = 0;

      اگر (vpip_current_vthread)
	    val = vthread_get_str_پشته(vpip_current_vthread, depth_);

      انتخاب (vp->format) {

	  حالت vpiObjTypeVal:
	    vp->format = vpiStringVal;
	    // fallthrough
	  حالت vpiStringVal:
	    rbuf = static_cast<نویسه *>(need_result_buf(val.size()+1, RBUF_VAL));
	    strcpy(rbuf, val.c_str());
	    vp->value.str = rbuf;
	    بشکن;

	  پیشفرض:
	    fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiConstant (String)\n", (صحیح)vp->format);

	    vp->format = vpiSuppressVal;
	    بشکن;
      }
}

رده __vpiVThrVec4Stack : عمومی __vpiHandle {
    عمومی:
      __vpiVThrVec4Stack(unsigned depth, منطقی signed_flag, unsigned wid);
      صحیح get_type_code(تهی) ثابت override;
      صحیح vpi_get(صحیح code) override;
      نویسه*vpi_get_str(صحیح code) override;
      تهی vpi_get_مقدار(p_vpi_مقدار val) override;
      vpiHandle vpi_put_مقدار(p_vpi_مقدار val, صحیح flags) override;
    خصوصی:
      ایستا تهی vpi_get_مقدار_string(p_vpi_مقدار vp, ثابت vvp_vector4_t&val);
      ایستا تهی vpi_get_مقدار_binstr(p_vpi_مقدار vp, ثابت vvp_vector4_t&val);
      تهی vpi_get_مقدار_decstr(p_vpi_مقدار vp, ثابت vvp_vector4_t&val);
      تهی vpi_get_مقدار_int   (p_vpi_مقدار vp, ثابت vvp_vector4_t&val);
      ایستا تهی vpi_get_مقدار_real  (p_vpi_مقدار vp, ثابت vvp_vector4_t&val);
      ایستا تهی vpi_get_مقدار_strength(p_vpi_مقدار vp, ثابت vvp_vector4_t&val);
      ایستا تهی vpi_get_مقدار_octstr(p_vpi_مقدار vp, ثابت vvp_vector4_t&val);
      ایستا تهی vpi_get_مقدار_hexstr(p_vpi_مقدار vp, ثابت vvp_vector4_t&val);
      ایستا تهی vpi_get_مقدار_vector(p_vpi_مقدار vp, ثابت vvp_vector4_t&val);
    خصوصی:
      unsigned depth_;
      منطقی signed_flag_;
      unsigned expect_width_;
      ثابت نویسه*name;
};

__vpiVThrVec4Stack::__vpiVThrVec4Stack(unsigned d, منطقی sf, unsigned wid)
: depth_(d), signed_flag_(sf), expect_width_(wid)
{
      name = vpip_name_string("S<,vec4,>");
}

صحیح __vpiVThrVec4Stack::get_type_code(تهی) ثابت
{ بازگشت vpiConstant; }


صحیح __vpiVThrVec4Stack::vpi_get(صحیح code)
{
      انتخاب (code) {
	  حالت vpiSize:
	    بازگشت expect_width_;

	  حالت vpiSigned:
	    بازگشت signed_flag_? 1 : 0;

	  حالت vpiConstType:
	    بازگشت vpi_دودویی_Const;

#اگر defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  حالت _vpiFromThr:
	    بازگشت _vpiVThr;
#endif

	  پیشفرض:
	    بازگشت 0;
      }
}

نویسه*__vpiVThrVec4Stack::vpi_get_str(صحیح code)
{
      انتخاب (code) {
	  حالت vpiFullName:
	    بازگشت simple_set_rbuf_str(name);

	  پیشفرض:
	    بازگشت 0;
      }
}

تهی __vpiVThrVec4Stack::vpi_get_مقدار(p_vpi_مقدار vp)
{
      vvp_vector4_t val;

      اگر (vpip_current_vthread)
	    val = vthread_get_vec4_پشته(vpip_current_vthread, depth_);

      انتخاب (vp->format) {

	  حالت vpiBinStrVal:
	    vpi_get_مقدار_binstr(vp, val);
	    بشکن;
	  حالت vpiDecStrVal:
	    vpi_get_مقدار_decstr(vp, val);
	    بشکن;
	  حالت vpiOctStrVal:
	    vpi_get_مقدار_octstr(vp, val);
	    بشکن;
	  حالت vpiHexStrVal:
	    vpi_get_مقدار_hexstr(vp, val);
	    بشکن;
	  حالت vpiIntVal:
	    vpi_get_مقدار_int(vp, val);
	    بشکن;
	  حالت vpiRealVal:
	    vpi_get_مقدار_real(vp, val);
	    بشکن;
	  حالت vpiStringVal:
	    vpi_get_مقدار_string(vp, val);
	    بشکن;
	  حالت vpiStrengthVal:
	    vpi_get_مقدار_strength(vp, val);
	    بشکن;
	  حالت vpiObjTypeVal:
	    vp->format = vpiVectorVal;
	    // fallthrough
	  حالت vpiVectorVal:
	    vpi_get_مقدار_vector(vp, val);
	    بشکن;

	  پیشفرض:
	    fprintf(stderr, "internal error: vpi_get_value(<format=%d>)"
		    " not implemented for __vpiVThrVec4Stack.\n", vp->format);
	    أكد(0);
      }

}

تهی __vpiVThrVec4Stack::vpi_get_مقدار_binstr(p_vpi_مقدار vp, ثابت vvp_vector4_t&val)
{
      unsigned wid = val.size();
      نویسه*rbuf = static_cast<نویسه*>(need_result_buf(wid+1, RBUF_VAL));
      برای (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    rbuf[wid-idx-1] = vvp_bit4_to_ascii(val.value(idx));
      }
      rbuf[wid] = 0;
      vp->value.str = rbuf;
}

تهی __vpiVThrVec4Stack::vpi_get_مقدار_decstr(p_vpi_مقدار vp, ثابت vvp_vector4_t&val)
{
      unsigned wid = val.size();
      صحیح nbuf = (wid+2)/3 + 1;
      نویسه *rbuf = static_cast<نویسه*>(need_result_buf(nbuf, RBUF_VAL));

      vpip_vec4_to_dec_str(val, rbuf, nbuf, signed_flag_);
      vp->value.str = rbuf;
}

تهی __vpiVThrVec4Stack::vpi_get_مقدار_octstr(p_vpi_مقدار vp, ثابت vvp_vector4_t&val)
{
      unsigned wid = val.size();
      unsigned owid = (wid + 2) / 3;
      نویسه*rbuf = static_cast<نویسه*>(need_result_buf(owid+1, RBUF_VAL));
      rbuf[owid] = 0;

      unsigned oval = 0;
      برای (unsigned idx = 0; idx < wid ; idx += 1) {
	    unsigned tmp = 0;
	    انتخاب (val.value(idx)) {
		حالت BIT4_0:
		  tmp = 0;
		  بشکن;
		حالت BIT4_1:
		  tmp = 1;
		  بشکن;
		حالت BIT4_X:
		  tmp = 2;
		  بشکن;
		حالت BIT4_Z:
		  tmp = 3;
		  بشکن;
	    }

	    oval = oval | (tmp << 2*(idx%3));

	    اگر (idx%3 == 2) {
		  owid -= 1;
		  rbuf[owid] = oct_digits[oval];
		  oval = 0;
	    }
      }

      اگر (owid > 0) {
	    owid -= 1;
	    rbuf[owid] = oct_digits[oval];
      }
      vp->value.str = rbuf;
}

تهی __vpiVThrVec4Stack::vpi_get_مقدار_hexstr(p_vpi_مقدار vp, ثابت vvp_vector4_t&val)
{
      unsigned wid = val.size();
      unsigned hwid = (wid + 3) /4;
      نویسه*rbuf = static_cast<نویسه*>(need_result_buf(hwid+1, RBUF_VAL));
      rbuf[hwid] = 0;

      unsigned hval = 0;
      برای (unsigned idx = 0; idx < wid ; idx += 1) {
	    unsigned tmp = 0;
	    انتخاب (val.value(idx)) {
		حالت BIT4_0:
		  tmp = 0;
		  بشکن;
		حالت BIT4_1:
		  tmp = 1;
		  بشکن;
		حالت BIT4_X:
		  tmp = 2;
		  بشکن;
		حالت BIT4_Z:
		  tmp = 3;
		  بشکن;
	    }
	    hval = hval | (tmp << 2*(idx%4));

	    اگر (idx%4 == 3) {
		  hwid -= 1;
		  rbuf[hwid] = hex_digits[hval];
		  hval = 0;
	    }
      }

      اگر (hwid > 0) {
	    hwid -= 1;
	    rbuf[hwid] = hex_digits[hval];
      }
      vp->value.str = rbuf;
}

تهی __vpiVThrVec4Stack::vpi_get_مقدار_int(p_vpi_مقدار vp, ثابت vvp_vector4_t&val)
{
      int32_t vali = 0;
      vector4_to_مقدار(val, vali, signed_flag_, نادرست);
      vp->value.integer = vali;
}

تهی __vpiVThrVec4Stack::vpi_get_مقدار_real(p_vpi_مقدار vp, ثابت vvp_vector4_t&val)
{
      unsigned wid = val.size();
      vp->value.real = 0.0;

      برای (unsigned idx = wid ; idx > 0 ; idx -= 1) {
	    vp->value.real *= 2.0;
	    اگر (val.value(idx-1) == BIT4_1)
		  vp->value.real += 1.0;
      }
}

تهی __vpiVThrVec4Stack::vpi_get_مقدار_string(p_vpi_مقدار vp, ثابت vvp_vector4_t&val)
{
      نویسه*rbuf = static_cast<نویسه*>(need_result_buf((val.size() / 8) + 1, RBUF_VAL));
      نویسه*cp = rbuf;

      نویسه tmp = 0;
      برای (صحیح bitnr = val.size()-1 ; bitnr >= 0 ; bitnr -= 1) {
	    tmp <<= 1;
	    انتخاب (val.value(bitnr)) {
		حالت BIT4_1:
		  tmp |= 1;
		  بشکن;
		حالت BIT4_0:
		پیشفرض:
		  بشکن;
	    }

	    اگر ((bitnr&7)==0) {
		    // Don't include leading nuls
		  اگر (tmp == 0 && cp == rbuf)
			ادامه;

		  *cp++ = tmp? tmp : ' ';
		  tmp = 0;
	    }
      }
      *cp++ = 0;
      vp->format = vpiStringVal;
      vp->value.str = rbuf;
}

تهی __vpiVThrVec4Stack::vpi_get_مقدار_vector(p_vpi_مقدار vp, ثابت vvp_vector4_t&val)
{
      unsigned wid = val.size();

      unsigned hwid = (wid+31)/32;
      vp->value.vector = static_cast<s_vpi_vecval*>
                         (need_result_buf(hwid*اندازه(s_vpi_vecval), RBUF_VAL));
      أكد(vp->value.vector);

	/* Zero the top of the result buffer first since the fill loop below only writes
	   bits 0..wid-1, which can leave stale data in the unused high bits. */
      اگر (hwid == 0) hwid = 1;
      vp->value.vector[hwid-1].aval = 0;
      vp->value.vector[hwid-1].bval = 0;

      برای (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
	    صحیح word = idx/32;
	    PLI_INT32 mask = 1 << (idx%32);

	    انتخاب (val.value(idx)) {
		حالت BIT4_0:
		  vp->value.vector[word].aval &= ~mask;
		  vp->value.vector[word].bval &= ~mask;
		  بشکن;
		حالت BIT4_1:
		  vp->value.vector[word].aval |=  mask;
		  vp->value.vector[word].bval &= ~mask;
		  بشکن;
		حالت BIT4_X:
		  vp->value.vector[word].aval |=  mask;
		  vp->value.vector[word].bval |=  mask;
		  بشکن;
		حالت BIT4_Z:
		  vp->value.vector[word].aval &= ~mask;
		  vp->value.vector[word].bval |=  mask;
		  بشکن;
	    }
      }
}

تهی __vpiVThrVec4Stack::vpi_get_مقدار_strength(p_vpi_مقدار vp, ثابت vvp_vector4_t&val)
{
      s_vpi_strengthval*op = static_cast<s_vpi_strengthval*>
                             (need_result_buf(val.size() * اندازه(s_vpi_strengthval),
                                              RBUF_VAL));

      برای (unsigned idx = 0 ; idx < val.size() ; idx += 1) {
	    انتخاب (val.value(idx)) {
		حالت BIT4_0:
		  op[idx].logic = vpi0;
		  op[idx].s0 = vpiStrongDrive;
		  op[idx].s1 = 0;
		  بشکن;
		حالت BIT4_1:
		  op[idx].logic = vpi1;
		  op[idx].s0 = 0;
		  op[idx].s1 = vpiStrongDrive;
		  بشکن;
		حالت BIT4_X:
		  op[idx].logic = vpiX;
		  op[idx].s0 = vpiStrongDrive;
		  op[idx].s1 = vpiStrongDrive;
		  بشکن;
		حالت BIT4_Z:
		  op[idx].logic = vpiZ;
		  op[idx].s0 = vpiHiZ;
		  op[idx].s1 = vpiHiZ;
		  بشکن;
	    }
      }

      vp->format = vpiStrengthVal;
      vp->value.strength = op;
}

vpiHandle __vpiVThrVec4Stack::vpi_put_مقدار(p_vpi_مقدار vp, صحیح /*flags*/)
{
      أكد(vpip_current_vthread);

      انتخاب (vp->format) {

	  پیشفرض:
	    fprintf(stderr, "internal error: vpi_put_value(<format=%d>)"
		    " not implemented for __vpiVThrVec4Stack.\n", vp->format);
	    أكد(0);
	    بازگشت 0;
      }
}

#ifdef CHECK_WITH_VALGRIND
تهی thread_vthr_delete(vpiHandle item)
{
      handle_map[item] = درست;
}

ایستا تهی thread_vthr_delete_real(vpiHandle item)
{
      __vpiVThrVec4Stack*obj = dynamic_cast<__vpiVThrVec4Stack*>(item);
      delete obj;
}

تهی vpi_handle_delete()
{
      map<vpiHandle, منطقی>::iterator iter;
      برای (iter = handle_map.begin(); iter != handle_map.end(); ++ iter ) {
	    اگر (iter->second) thread_vthr_delete_real(iter->first);
	    وگرنه thread_word_delete_real(iter->first);
      }
}
#endif


vpiHandle vpip_make_vthr_str_پشته(unsigned depth)
{
      __vpiVThrStrStack*obj = جدید __vpiVThrStrStack(depth);
      بازگشت obj;
}

vpiHandle vpip_make_vthr_vec4_پشته(unsigned depth, منطقی signed_flag, unsigned wid)
{
      __vpiVThrVec4Stack*obj = جدید __vpiVThrVec4Stack(depth, signed_flag, wid);
      بازگشت obj;
}

#ifdef CHECK_WITH_VALGRIND
ایستا map<vpiHandle, منطقی> پشته_map;

تهی thread_string_delete(vpiHandle item)
{
      پشته_map[item] = نادرست;
}

ایستا تهی thread_string_delete_real(vpiHandle item)
{
      __vpiVThrStrStack*obj = dynamic_cast<__vpiVThrStrStack*>(item);
      delete obj;
}

تهی vpi_پشته_delete()
{
      map<vpiHandle, منطقی>::iterator iter;
      برای (iter = پشته_map.begin(); iter != پشته_map.end(); ++ iter ) {
	    thread_string_delete_real(iter->first);
      }
}
#endif
