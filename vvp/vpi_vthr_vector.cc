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

using নামাকাশ std;

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
বাহ্যিক ধ্রুবক অক্ষর hex_digits[256];
বাহ্যিক ধ্রুবক অক্ষর oct_digits[64];


শ্রেণী __vpiVThrWord : সর্বজনীন __vpiHandle {
    সর্বজনীন:
      explicit __vpiVThrWord(unsigned base);
      পূর্ণসংখ্যা get_type_code(শূন্য) ধ্রুবক override;
      পূর্ণসংখ্যা vpi_get(পূর্ণসংখ্যা code) override;
      শূন্য vpi_get_মান(p_vpi_মান val) override;

      পূর্ণসংখ্যা get_subtype() ধ্রুবক { ফেরত subtype; };
      unsigned get_সূচক() ধ্রুবক { ফেরত সূচক; };

    ব্যক্তিগত:
      ধ্রুবক অক্ষর* name;
      পূর্ণসংখ্যা subtype;
      unsigned সূচক;
};

স্থির পূর্ণসংখ্যা vthr_word_get(পূর্ণসংখ্যা code, vpiHandle ref)
{
      ধ্রুবক __vpiVThrWord*rfp = dynamic_cast<__vpiVThrWord*>(ref);

      নির্বাচন (code) {

	  ক্ষেত্র vpiConstType:
	    ফেরত rfp->get_subtype();

#যদি defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  ক্ষেত্র _vpiFromThr:
	    ফেরত _vpiWord;
#endif

	  পূর্বনির্ধারিত:
	    ফেরত 0;
      }
}

স্থির শূন্য vthr_real_get_মান(vpiHandle ref, s_vpi_মান*vp)
{
      ধ্রুবক __vpiVThrWord*obj = dynamic_cast<__vpiVThrWord*>(ref);
      স্থির ধ্রুবক size_t RBUF_USE_আকার = 66;
      অক্ষর *rbuf = static_cast<অক্ষর *>(need_ফলাফল_buf(RBUF_USE_আকার, RBUF_VAL));

      দ্বিগুণ val = 0.0;

	/* Get the actual value from the index. It is possible, by the
	   way, that the vpi_get_value is called from compiletf. If
	   that's the case, there will be no current thread, and this
	   will not have access to the proper value. Punt and return a
	   0.0 value instead. */
      যদি (vpip_current_vthread)
	    val = vthread_get_real_স্তূপ(vpip_current_vthread,
	                                 obj->get_সূচক());

      নির্বাচন (vp->format) {

	  ক্ষেত্র vpiObjTypeVal:
	    vp->format = vpiRealVal;
	    // fallthrough
	  ক্ষেত্র vpiRealVal:
	    vp->value.real = val;
	    বিরতি;

	  ক্ষেত্র vpiIntVal:
	      /* NaN or +/- infinity are translated as 0. */
	    যদি (val != val || (val && (val == 0.5*val))) {
		  val = 0.0;
	    } অন্যথা {
		  val = std::round(val);
	    }
	    vp->value.integer = (PLI_INT32)val;
	    বিরতি;

	  ক্ষেত্র vpiDecStrVal:
	    যদি (std::isnan(val))
		  snprintf(rbuf, RBUF_USE_আকার, "%s", "nan");
	    অন্যথা
		  snprintf(rbuf, RBUF_USE_আকার, "%0.0f", std::round(val));
	    vp->value.str = rbuf;
	    বিরতি;

	  ক্ষেত্র vpiOctStrVal:
	    snprintf(rbuf, RBUF_USE_আকার, "%" PRIo64, vlg_round_to_u64(val));
	    vp->value.str = rbuf;
	    বিরতি;

	  ক্ষেত্র vpiHexStrVal:
	    snprintf(rbuf, RBUF_USE_আকার, "%" PRIx64, vlg_round_to_u64(val));
	    vp->value.str = rbuf;
	    বিরতি;

	  ক্ষেত্র vpiBinStrVal: {
		uint64_t vali = vlg_round_to_u64(val);
		unsigned len = 0;

		যখন (vali > 0) {
		      len += 1;
		      vali /= 2;
		}

		vali = vlg_round_to_u64(val);
		জন্য (unsigned idx = 0 ;  idx < len ;  idx += 1) {
		      rbuf[len-idx-1] = (vali & 1)? '1' : '0';
		      vali /= 2;
		}

		rbuf[len] = 0;
		যদি (len == 0) {
		      rbuf[0] = '0';
		      rbuf[1] = 0;
		}
		vp->value.str = rbuf;
		বিরতি;
	  }

	  পূর্বনির্ধারিত:
	    fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiConstant (Real)\n", (পূর্ণসংখ্যা)vp->format);

	    vp->format = vpiSuppressVal;
	    বিরতি;
      }
}

অন্তঃরেখ __vpiVThrWord::__vpiVThrWord(unsigned base)
{
      name = vpip_name_string("W<>");
      subtype = vpiRealConst;
      সূচক = base;
}

পূর্ণসংখ্যা __vpiVThrWord::get_type_code(শূন্য) ধ্রুবক
{ ফেরত vpiConstant; }

পূর্ণসংখ্যা __vpiVThrWord::vpi_get(পূর্ণসংখ্যা code)
{ ফেরত vthr_word_get(code, এই); }

শূন্য __vpiVThrWord::vpi_get_মান(p_vpi_মান val)
{ vthr_real_get_মান(এই, val); }

vpiHandle vpip_make_vthr_word(unsigned base, ধ্রুবক অক্ষর*type)
{
      প্রতিজ্ঞা(type[0] == 'r');
      প্রতিজ্ঞা(base < 65536);
      __vpiVThrWord*obj = নতুন __vpiVThrWord(base);

      ফেরত obj;
}

#ifdef CHECK_WITH_VALGRIND
স্থির map<vpiHandle, যৌক্তিক> handle_map;

শূন্য thread_word_delete(vpiHandle item)
{
      handle_map[item] = মিথ্যা;
}

স্থির শূন্য thread_word_delete_real(vpiHandle item)
{
      শ্রেণী __vpiVThrWord*obj = dynamic_cast<__vpiVThrWord*>(item);
      delete obj;
}
#endif

শ্রেণী __vpiVThrStrStack : সর্বজনীন __vpiHandle {
    সর্বজনীন:
      explicit __vpiVThrStrStack(unsigned depth);
      পূর্ণসংখ্যা get_type_code(শূন্য) ধ্রুবক override;
      পূর্ণসংখ্যা vpi_get(পূর্ণসংখ্যা code) override;
      শূন্য vpi_get_মান(p_vpi_মান val) override;
    ব্যক্তিগত:
      unsigned depth_;
};

__vpiVThrStrStack::__vpiVThrStrStack(unsigned d)
: depth_(d)
{
}

পূর্ণসংখ্যা __vpiVThrStrStack::get_type_code(শূন্য) ধ্রুবক
{ ফেরত vpiConstant; }

পূর্ণসংখ্যা __vpiVThrStrStack::vpi_get(পূর্ণসংখ্যা code)
{
      নির্বাচন (code) {
	  ক্ষেত্র vpiConstType:
	    ফেরত vpiStringConst;
#যদি defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  ক্ষেত্র _vpiFromThr:
	    ফেরত _vpiString;
#endif
	  ক্ষেত্র vpi_আকার:
	    ফেরত vthread_get_str_স্তূপ(vpip_current_vthread, depth_).size();
	  পূর্বনির্ধারিত:
	    ফেরত 0;
      }
}

শূন্য __vpiVThrStrStack::vpi_get_মান(p_vpi_মান vp)
{
      string val;
      অক্ষর*rbuf = 0;

      যদি (vpip_current_vthread)
	    val = vthread_get_str_স্তূপ(vpip_current_vthread, depth_);

      নির্বাচন (vp->format) {

	  ক্ষেত্র vpiObjTypeVal:
	    vp->format = vpiStringVal;
	    // fallthrough
	  ক্ষেত্র vpiStringVal:
	    rbuf = static_cast<অক্ষর *>(need_ফলাফল_buf(val.size()+1, RBUF_VAL));
	    strcpy(rbuf, val.c_str());
	    vp->value.str = rbuf;
	    বিরতি;

	  পূর্বনির্ধারিত:
	    fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiConstant (String)\n", (পূর্ণসংখ্যা)vp->format);

	    vp->format = vpiSuppressVal;
	    বিরতি;
      }
}

শ্রেণী __vpiVThrVec4Stack : সর্বজনীন __vpiHandle {
    সর্বজনীন:
      __vpiVThrVec4Stack(unsigned depth, যৌক্তিক signed_flag, unsigned wid);
      পূর্ণসংখ্যা get_type_code(শূন্য) ধ্রুবক override;
      পূর্ণসংখ্যা vpi_get(পূর্ণসংখ্যা code) override;
      অক্ষর*vpi_get_str(পূর্ণসংখ্যা code) override;
      শূন্য vpi_get_মান(p_vpi_মান val) override;
      vpiHandle vpi_put_মান(p_vpi_মান val, পূর্ণসংখ্যা flags) override;
    ব্যক্তিগত:
      স্থির শূন্য vpi_get_মান_string(p_vpi_মান vp, ধ্রুবক vvp_vector4_t&val);
      স্থির শূন্য vpi_get_মান_binstr(p_vpi_মান vp, ধ্রুবক vvp_vector4_t&val);
      শূন্য vpi_get_মান_decstr(p_vpi_মান vp, ধ্রুবক vvp_vector4_t&val);
      শূন্য vpi_get_মান_int   (p_vpi_মান vp, ধ্রুবক vvp_vector4_t&val);
      স্থির শূন্য vpi_get_মান_real  (p_vpi_মান vp, ধ্রুবক vvp_vector4_t&val);
      স্থির শূন্য vpi_get_মান_strength(p_vpi_মান vp, ধ্রুবক vvp_vector4_t&val);
      স্থির শূন্য vpi_get_মান_octstr(p_vpi_মান vp, ধ্রুবক vvp_vector4_t&val);
      স্থির শূন্য vpi_get_মান_hexstr(p_vpi_মান vp, ধ্রুবক vvp_vector4_t&val);
      স্থির শূন্য vpi_get_মান_vector(p_vpi_মান vp, ধ্রুবক vvp_vector4_t&val);
    ব্যক্তিগত:
      unsigned depth_;
      যৌক্তিক signed_flag_;
      unsigned expect_width_;
      ধ্রুবক অক্ষর*name;
};

__vpiVThrVec4Stack::__vpiVThrVec4Stack(unsigned d, যৌক্তিক sf, unsigned wid)
: depth_(d), signed_flag_(sf), expect_width_(wid)
{
      name = vpip_name_string("S<,vec4,>");
}

পূর্ণসংখ্যা __vpiVThrVec4Stack::get_type_code(শূন্য) ধ্রুবক
{ ফেরত vpiConstant; }


পূর্ণসংখ্যা __vpiVThrVec4Stack::vpi_get(পূর্ণসংখ্যা code)
{
      নির্বাচন (code) {
	  ক্ষেত্র vpi_আকার:
	    ফেরত expect_width_;

	  ক্ষেত্র vpiSigned:
	    ফেরত signed_flag_? 1 : 0;

	  ক্ষেত্র vpiConstType:
	    ফেরত vpi_দ্বিমিক_Const;

#যদি defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  ক্ষেত্র _vpiFromThr:
	    ফেরত _vpiVThr;
#endif

	  পূর্বনির্ধারিত:
	    ফেরত 0;
      }
}

অক্ষর*__vpiVThrVec4Stack::vpi_get_str(পূর্ণসংখ্যা code)
{
      নির্বাচন (code) {
	  ক্ষেত্র vpiFullName:
	    ফেরত simple_set_rbuf_str(name);

	  পূর্বনির্ধারিত:
	    ফেরত 0;
      }
}

শূন্য __vpiVThrVec4Stack::vpi_get_মান(p_vpi_মান vp)
{
      vvp_vector4_t val;

      যদি (vpip_current_vthread)
	    val = vthread_get_vec4_স্তূপ(vpip_current_vthread, depth_);

      নির্বাচন (vp->format) {

	  ক্ষেত্র vpiBinStrVal:
	    vpi_get_মান_binstr(vp, val);
	    বিরতি;
	  ক্ষেত্র vpiDecStrVal:
	    vpi_get_মান_decstr(vp, val);
	    বিরতি;
	  ক্ষেত্র vpiOctStrVal:
	    vpi_get_মান_octstr(vp, val);
	    বিরতি;
	  ক্ষেত্র vpiHexStrVal:
	    vpi_get_মান_hexstr(vp, val);
	    বিরতি;
	  ক্ষেত্র vpiIntVal:
	    vpi_get_মান_int(vp, val);
	    বিরতি;
	  ক্ষেত্র vpiRealVal:
	    vpi_get_মান_real(vp, val);
	    বিরতি;
	  ক্ষেত্র vpiStringVal:
	    vpi_get_মান_string(vp, val);
	    বিরতি;
	  ক্ষেত্র vpiStrengthVal:
	    vpi_get_মান_strength(vp, val);
	    বিরতি;
	  ক্ষেত্র vpiObjTypeVal:
	    vp->format = vpiVectorVal;
	    // fallthrough
	  ক্ষেত্র vpiVectorVal:
	    vpi_get_মান_vector(vp, val);
	    বিরতি;

	  পূর্বনির্ধারিত:
	    fprintf(stderr, "internal error: vpi_get_value(<format=%d>)"
		    " not implemented for __vpiVThrVec4Stack.\n", vp->format);
	    প্রতিজ্ঞা(0);
      }

}

শূন্য __vpiVThrVec4Stack::vpi_get_মান_binstr(p_vpi_মান vp, ধ্রুবক vvp_vector4_t&val)
{
      unsigned wid = val.size();
      অক্ষর*rbuf = static_cast<অক্ষর*>(need_ফলাফল_buf(wid+1, RBUF_VAL));
      জন্য (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    rbuf[wid-idx-1] = vvp_bit4_to_ascii(val.value(idx));
      }
      rbuf[wid] = 0;
      vp->value.str = rbuf;
}

শূন্য __vpiVThrVec4Stack::vpi_get_মান_decstr(p_vpi_মান vp, ধ্রুবক vvp_vector4_t&val)
{
      unsigned wid = val.size();
      পূর্ণসংখ্যা nbuf = (wid+2)/3 + 1;
      অক্ষর *rbuf = static_cast<অক্ষর*>(need_ফলাফল_buf(nbuf, RBUF_VAL));

      vpip_vec4_to_dec_str(val, rbuf, nbuf, signed_flag_);
      vp->value.str = rbuf;
}

শূন্য __vpiVThrVec4Stack::vpi_get_মান_octstr(p_vpi_মান vp, ধ্রুবক vvp_vector4_t&val)
{
      unsigned wid = val.size();
      unsigned owid = (wid + 2) / 3;
      অক্ষর*rbuf = static_cast<অক্ষর*>(need_ফলাফল_buf(owid+1, RBUF_VAL));
      rbuf[owid] = 0;

      unsigned oval = 0;
      জন্য (unsigned idx = 0; idx < wid ; idx += 1) {
	    unsigned tmp = 0;
	    নির্বাচন (val.value(idx)) {
		ক্ষেত্র BIT4_0:
		  tmp = 0;
		  বিরতি;
		ক্ষেত্র BIT4_1:
		  tmp = 1;
		  বিরতি;
		ক্ষেত্র BIT4_X:
		  tmp = 2;
		  বিরতি;
		ক্ষেত্র BIT4_Z:
		  tmp = 3;
		  বিরতি;
	    }

	    oval = oval | (tmp << 2*(idx%3));

	    যদি (idx%3 == 2) {
		  owid -= 1;
		  rbuf[owid] = oct_digits[oval];
		  oval = 0;
	    }
      }

      যদি (owid > 0) {
	    owid -= 1;
	    rbuf[owid] = oct_digits[oval];
      }
      vp->value.str = rbuf;
}

শূন্য __vpiVThrVec4Stack::vpi_get_মান_hexstr(p_vpi_মান vp, ধ্রুবক vvp_vector4_t&val)
{
      unsigned wid = val.size();
      unsigned hwid = (wid + 3) /4;
      অক্ষর*rbuf = static_cast<অক্ষর*>(need_ফলাফল_buf(hwid+1, RBUF_VAL));
      rbuf[hwid] = 0;

      unsigned hval = 0;
      জন্য (unsigned idx = 0; idx < wid ; idx += 1) {
	    unsigned tmp = 0;
	    নির্বাচন (val.value(idx)) {
		ক্ষেত্র BIT4_0:
		  tmp = 0;
		  বিরতি;
		ক্ষেত্র BIT4_1:
		  tmp = 1;
		  বিরতি;
		ক্ষেত্র BIT4_X:
		  tmp = 2;
		  বিরতি;
		ক্ষেত্র BIT4_Z:
		  tmp = 3;
		  বিরতি;
	    }
	    hval = hval | (tmp << 2*(idx%4));

	    যদি (idx%4 == 3) {
		  hwid -= 1;
		  rbuf[hwid] = hex_digits[hval];
		  hval = 0;
	    }
      }

      যদি (hwid > 0) {
	    hwid -= 1;
	    rbuf[hwid] = hex_digits[hval];
      }
      vp->value.str = rbuf;
}

শূন্য __vpiVThrVec4Stack::vpi_get_মান_int(p_vpi_মান vp, ধ্রুবক vvp_vector4_t&val)
{
      int32_t vali = 0;
      vector4_to_মান(val, vali, signed_flag_, মিথ্যা);
      vp->value.integer = vali;
}

শূন্য __vpiVThrVec4Stack::vpi_get_মান_real(p_vpi_মান vp, ধ্রুবক vvp_vector4_t&val)
{
      unsigned wid = val.size();
      vp->value.real = 0.0;

      জন্য (unsigned idx = wid ; idx > 0 ; idx -= 1) {
	    vp->value.real *= 2.0;
	    যদি (val.value(idx-1) == BIT4_1)
		  vp->value.real += 1.0;
      }
}

শূন্য __vpiVThrVec4Stack::vpi_get_মান_string(p_vpi_মান vp, ধ্রুবক vvp_vector4_t&val)
{
      অক্ষর*rbuf = static_cast<অক্ষর*>(need_ফলাফল_buf((val.size() / 8) + 1, RBUF_VAL));
      অক্ষর*cp = rbuf;

      অক্ষর tmp = 0;
      জন্য (পূর্ণসংখ্যা bitnr = val.size()-1 ; bitnr >= 0 ; bitnr -= 1) {
	    tmp <<= 1;
	    নির্বাচন (val.value(bitnr)) {
		ক্ষেত্র BIT4_1:
		  tmp |= 1;
		  বিরতি;
		ক্ষেত্র BIT4_0:
		পূর্বনির্ধারিত:
		  বিরতি;
	    }

	    যদি ((bitnr&7)==0) {
		    // Don't include leading nuls
		  যদি (tmp == 0 && cp == rbuf)
			চালু;

		  *cp++ = tmp? tmp : ' ';
		  tmp = 0;
	    }
      }
      *cp++ = 0;
      vp->format = vpiStringVal;
      vp->value.str = rbuf;
}

শূন্য __vpiVThrVec4Stack::vpi_get_মান_vector(p_vpi_মান vp, ধ্রুবক vvp_vector4_t&val)
{
      unsigned wid = val.size();

      unsigned hwid = (wid+31)/32;
      vp->value.vector = static_cast<s_vpi_vecval*>
                         (need_ফলাফল_buf(hwid*আকার(s_vpi_vecval), RBUF_VAL));
      প্রতিজ্ঞা(vp->value.vector);

	/* Zero the top of the result buffer first since the fill loop below only writes
	   bits 0..wid-1, which can leave stale data in the unused high bits. */
      যদি (hwid == 0) hwid = 1;
      vp->value.vector[hwid-1].aval = 0;
      vp->value.vector[hwid-1].bval = 0;

      জন্য (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
	    পূর্ণসংখ্যা word = idx/32;
	    PLI_INT32 mask = 1 << (idx%32);

	    নির্বাচন (val.value(idx)) {
		ক্ষেত্র BIT4_0:
		  vp->value.vector[word].aval &= ~mask;
		  vp->value.vector[word].bval &= ~mask;
		  বিরতি;
		ক্ষেত্র BIT4_1:
		  vp->value.vector[word].aval |=  mask;
		  vp->value.vector[word].bval &= ~mask;
		  বিরতি;
		ক্ষেত্র BIT4_X:
		  vp->value.vector[word].aval |=  mask;
		  vp->value.vector[word].bval |=  mask;
		  বিরতি;
		ক্ষেত্র BIT4_Z:
		  vp->value.vector[word].aval &= ~mask;
		  vp->value.vector[word].bval |=  mask;
		  বিরতি;
	    }
      }
}

শূন্য __vpiVThrVec4Stack::vpi_get_মান_strength(p_vpi_মান vp, ধ্রুবক vvp_vector4_t&val)
{
      s_vpi_strengthval*op = static_cast<s_vpi_strengthval*>
                             (need_ফলাফল_buf(val.size() * আকার(s_vpi_strengthval),
                                              RBUF_VAL));

      জন্য (unsigned idx = 0 ; idx < val.size() ; idx += 1) {
	    নির্বাচন (val.value(idx)) {
		ক্ষেত্র BIT4_0:
		  op[idx].logic = vpi0;
		  op[idx].s0 = vpiStrongDrive;
		  op[idx].s1 = 0;
		  বিরতি;
		ক্ষেত্র BIT4_1:
		  op[idx].logic = vpi1;
		  op[idx].s0 = 0;
		  op[idx].s1 = vpiStrongDrive;
		  বিরতি;
		ক্ষেত্র BIT4_X:
		  op[idx].logic = vpiX;
		  op[idx].s0 = vpiStrongDrive;
		  op[idx].s1 = vpiStrongDrive;
		  বিরতি;
		ক্ষেত্র BIT4_Z:
		  op[idx].logic = vpiZ;
		  op[idx].s0 = vpiHiZ;
		  op[idx].s1 = vpiHiZ;
		  বিরতি;
	    }
      }

      vp->format = vpiStrengthVal;
      vp->value.strength = op;
}

vpiHandle __vpiVThrVec4Stack::vpi_put_মান(p_vpi_মান vp, পূর্ণসংখ্যা /*flags*/)
{
      প্রতিজ্ঞা(vpip_current_vthread);

      নির্বাচন (vp->format) {

	  পূর্বনির্ধারিত:
	    fprintf(stderr, "internal error: vpi_put_value(<format=%d>)"
		    " not implemented for __vpiVThrVec4Stack.\n", vp->format);
	    প্রতিজ্ঞা(0);
	    ফেরত 0;
      }
}

#ifdef CHECK_WITH_VALGRIND
শূন্য thread_vthr_delete(vpiHandle item)
{
      handle_map[item] = সত্য;
}

স্থির শূন্য thread_vthr_delete_real(vpiHandle item)
{
      __vpiVThrVec4Stack*obj = dynamic_cast<__vpiVThrVec4Stack*>(item);
      delete obj;
}

শূন্য vpi_handle_delete()
{
      map<vpiHandle, যৌক্তিক>::iterator iter;
      জন্য (iter = handle_map.begin(); iter != handle_map.end(); ++ iter ) {
	    যদি (iter->second) thread_vthr_delete_real(iter->first);
	    অন্যথা thread_word_delete_real(iter->first);
      }
}
#endif


vpiHandle vpip_make_vthr_str_স্তূপ(unsigned depth)
{
      __vpiVThrStrStack*obj = নতুন __vpiVThrStrStack(depth);
      ফেরত obj;
}

vpiHandle vpip_make_vthr_vec4_স্তূপ(unsigned depth, যৌক্তিক signed_flag, unsigned wid)
{
      __vpiVThrVec4Stack*obj = নতুন __vpiVThrVec4Stack(depth, signed_flag, wid);
      ফেরত obj;
}

#ifdef CHECK_WITH_VALGRIND
স্থির map<vpiHandle, যৌক্তিক> স্তূপ_map;

শূন্য thread_string_delete(vpiHandle item)
{
      স্তূপ_map[item] = মিথ্যা;
}

স্থির শূন্য thread_string_delete_real(vpiHandle item)
{
      __vpiVThrStrStack*obj = dynamic_cast<__vpiVThrStrStack*>(item);
      delete obj;
}

শূন্য vpi_স্তূপ_delete()
{
      map<vpiHandle, যৌক্তিক>::iterator iter;
      জন্য (iter = স্তূপ_map.begin(); iter != স্তূপ_map.end(); ++ iter ) {
	    thread_string_delete_real(iter->first);
      }
}
#endif
