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
বাহ্য ধ্রুব বর্ণ hex_digits[256];
বাহ্য ধ্রুব বর্ণ oct_digits[64];


বর্গ __vpiVThrWord : সার্বজনিক __vpiHandle {
    সার্বজনিক:
      explicit __vpiVThrWord(unsigned base);
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব override;
      পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code) override;
      শূন্য vpi_get_মান(p_vpi_মান val) override;

      পূর্ণাঙ্ক get_subtype() ধ্রুব { প্রত্যাগম subtype; };
      unsigned get_index() ধ্রুব { প্রত্যাগম index; };

    গুপ্ত:
      ধ্রুব বর্ণ* name;
      পূর্ণাঙ্ক subtype;
      unsigned index;
};

স্থির পূর্ণাঙ্ক vthr_word_get(পূর্ণাঙ্ক code, vpiHandle ref)
{
      ধ্রুব __vpiVThrWord*rfp = dynamic_cast<__vpiVThrWord*>(ref);

      বিকল্প (code) {

	  পক্ষ vpiConstType:
	    প্রত্যাগম rfp->get_subtype();

#যদি defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  পক্ষ _vpiFromThr:
	    প্রত্যাগম _vpiWord;
#endif

	  স্বভাব:
	    প্রত্যাগম 0;
      }
}

স্থির শূন্য vthr_real_get_মান(vpiHandle ref, s_vpi_মান*vp)
{
      ধ্রুব __vpiVThrWord*obj = dynamic_cast<__vpiVThrWord*>(ref);
      স্থির ধ্রুব size_t RBUF_USE_SIZE = 66;
      বর্ণ *rbuf = static_cast<বর্ণ *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));

      দ্বিগুণ val = 0.0;

	/* Get the actual value from the index. It is possible, by the
	   way, that the vpi_get_value is called from compiletf. If
	   that's the case, there will be no current thread, and this
	   will not have access to the proper value. Punt and return a
	   0.0 value instead. */
      যদি (vpip_current_vthread)
	    val = vthread_get_real_স্তূপ(vpip_current_vthread,
	                                 obj->get_index());

      বিকল্প (vp->format) {

	  পক্ষ vpiObjTypeVal:
	    vp->format = vpiRealVal;
	    // fallthrough
	  পক্ষ vpiRealVal:
	    vp->value.real = val;
	    ভঙ্গ;

	  পক্ষ vpiIntVal:
	      /* NaN or +/- infinity are translated as 0. */
	    যদি (val != val || (val && (val == 0.5*val))) {
		  val = 0.0;
	    } অন্যথা {
		  val = std::round(val);
	    }
	    vp->value.integer = (PLI_INT32)val;
	    ভঙ্গ;

	  পক্ষ vpiDecStrVal:
	    যদি (std::isnan(val))
		  snprintf(rbuf, RBUF_USE_SIZE, "%s", "nan");
	    অন্যথা
		  snprintf(rbuf, RBUF_USE_SIZE, "%0.0f", std::round(val));
	    vp->value.str = rbuf;
	    ভঙ্গ;

	  পক্ষ vpiOctStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" PRIo64, vlg_round_to_u64(val));
	    vp->value.str = rbuf;
	    ভঙ্গ;

	  পক্ষ vpiHexStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" PRIx64, vlg_round_to_u64(val));
	    vp->value.str = rbuf;
	    ভঙ্গ;

	  পক্ষ vpiBinStrVal: {
		uint64_t vali = vlg_round_to_u64(val);
		unsigned len = 0;

		যাবত্ (vali > 0) {
		      len += 1;
		      vali /= 2;
		}

		vali = vlg_round_to_u64(val);
		প্রতি (unsigned idx = 0 ;  idx < len ;  idx += 1) {
		      rbuf[len-idx-1] = (vali & 1)? '1' : '0';
		      vali /= 2;
		}

		rbuf[len] = 0;
		যদি (len == 0) {
		      rbuf[0] = '0';
		      rbuf[1] = 0;
		}
		vp->value.str = rbuf;
		ভঙ্গ;
	  }

	  স্বভাব:
	    fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiConstant (Real)\n", (পূর্ণাঙ্ক)vp->format);

	    vp->format = vpiSuppressVal;
	    ভঙ্গ;
      }
}

অন্তঃস্থ __vpiVThrWord::__vpiVThrWord(unsigned base)
{
      name = vpip_name_string("W<>");
      subtype = vpiRealConst;
      index = base;
}

পূর্ণাঙ্ক __vpiVThrWord::get_type_code(শূন্য) ধ্রুব
{ প্রত্যাগম vpiConstant; }

পূর্ণাঙ্ক __vpiVThrWord::vpi_get(পূর্ণাঙ্ক code)
{ প্রত্যাগম vthr_word_get(code, এতত্); }

শূন্য __vpiVThrWord::vpi_get_মান(p_vpi_মান val)
{ vthr_real_get_মান(এতত্, val); }

vpiHandle vpip_make_vthr_word(unsigned base, ধ্রুব বর্ণ*type)
{
      প্রতিজ্ঞা(type[0] == 'r');
      প্রতিজ্ঞা(base < 65536);
      __vpiVThrWord*obj = নূতন __vpiVThrWord(base);

      প্রত্যাগম obj;
}

#ifdef CHECK_WITH_VALGRIND
স্থির map<vpiHandle, তার্কিক> handle_map;

শূন্য thread_word_delete(vpiHandle item)
{
      handle_map[item] = অসত্য;
}

স্থির শূন্য thread_word_delete_real(vpiHandle item)
{
      বর্গ __vpiVThrWord*obj = dynamic_cast<__vpiVThrWord*>(item);
      delete obj;
}
#endif

বর্গ __vpiVThrStrStack : সার্বজনিক __vpiHandle {
    সার্বজনিক:
      explicit __vpiVThrStrStack(unsigned depth);
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব override;
      পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code) override;
      শূন্য vpi_get_মান(p_vpi_মান val) override;
    গুপ্ত:
      unsigned depth_;
};

__vpiVThrStrStack::__vpiVThrStrStack(unsigned d)
: depth_(d)
{
}

পূর্ণাঙ্ক __vpiVThrStrStack::get_type_code(শূন্য) ধ্রুব
{ প্রত্যাগম vpiConstant; }

পূর্ণাঙ্ক __vpiVThrStrStack::vpi_get(পূর্ণাঙ্ক code)
{
      বিকল্প (code) {
	  পক্ষ vpiConstType:
	    প্রত্যাগম vpiStringConst;
#যদি defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  পক্ষ _vpiFromThr:
	    প্রত্যাগম _vpiString;
#endif
	  পক্ষ vpiSize:
	    প্রত্যাগম vthread_get_str_স্তূপ(vpip_current_vthread, depth_).size();
	  স্বভাব:
	    প্রত্যাগম 0;
      }
}

শূন্য __vpiVThrStrStack::vpi_get_মান(p_vpi_মান vp)
{
      string val;
      বর্ণ*rbuf = 0;

      যদি (vpip_current_vthread)
	    val = vthread_get_str_স্তূপ(vpip_current_vthread, depth_);

      বিকল্প (vp->format) {

	  পক্ষ vpiObjTypeVal:
	    vp->format = vpiStringVal;
	    // fallthrough
	  পক্ষ vpiStringVal:
	    rbuf = static_cast<বর্ণ *>(need_result_buf(val.size()+1, RBUF_VAL));
	    strcpy(rbuf, val.c_str());
	    vp->value.str = rbuf;
	    ভঙ্গ;

	  স্বভাব:
	    fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiConstant (String)\n", (পূর্ণাঙ্ক)vp->format);

	    vp->format = vpiSuppressVal;
	    ভঙ্গ;
      }
}

বর্গ __vpiVThrVec4Stack : সার্বজনিক __vpiHandle {
    সার্বজনিক:
      __vpiVThrVec4Stack(unsigned depth, তার্কিক signed_flag, unsigned wid);
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব override;
      পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code) override;
      বর্ণ*vpi_get_str(পূর্ণাঙ্ক code) override;
      শূন্য vpi_get_মান(p_vpi_মান val) override;
      vpiHandle vpi_put_মান(p_vpi_মান val, পূর্ণাঙ্ক flags) override;
    গুপ্ত:
      স্থির শূন্য vpi_get_মান_string(p_vpi_মান vp, ধ্রুব vvp_vector4_t&val);
      স্থির শূন্য vpi_get_মান_binstr(p_vpi_মান vp, ধ্রুব vvp_vector4_t&val);
      শূন্য vpi_get_মান_decstr(p_vpi_মান vp, ধ্রুব vvp_vector4_t&val);
      শূন্য vpi_get_মান_int   (p_vpi_মান vp, ধ্রুব vvp_vector4_t&val);
      স্থির শূন্য vpi_get_মান_real  (p_vpi_মান vp, ধ্রুব vvp_vector4_t&val);
      স্থির শূন্য vpi_get_মান_strength(p_vpi_মান vp, ধ্রুব vvp_vector4_t&val);
      স্থির শূন্য vpi_get_মান_octstr(p_vpi_মান vp, ধ্রুব vvp_vector4_t&val);
      স্থির শূন্য vpi_get_মান_hexstr(p_vpi_মান vp, ধ্রুব vvp_vector4_t&val);
      স্থির শূন্য vpi_get_মান_vector(p_vpi_মান vp, ধ্রুব vvp_vector4_t&val);
    গুপ্ত:
      unsigned depth_;
      তার্কিক signed_flag_;
      unsigned expect_width_;
      ধ্রুব বর্ণ*name;
};

__vpiVThrVec4Stack::__vpiVThrVec4Stack(unsigned d, তার্কিক sf, unsigned wid)
: depth_(d), signed_flag_(sf), expect_width_(wid)
{
      name = vpip_name_string("S<,vec4,>");
}

পূর্ণাঙ্ক __vpiVThrVec4Stack::get_type_code(শূন্য) ধ্রুব
{ প্রত্যাগম vpiConstant; }


পূর্ণাঙ্ক __vpiVThrVec4Stack::vpi_get(পূর্ণাঙ্ক code)
{
      বিকল্প (code) {
	  পক্ষ vpiSize:
	    প্রত্যাগম expect_width_;

	  পক্ষ vpiSigned:
	    প্রত্যাগম signed_flag_? 1 : 0;

	  পক্ষ vpiConstType:
	    প্রত্যাগম vpi_দ্বিমিক_Const;

#যদি defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  পক্ষ _vpiFromThr:
	    প্রত্যাগম _vpiVThr;
#endif

	  স্বভাব:
	    প্রত্যাগম 0;
      }
}

বর্ণ*__vpiVThrVec4Stack::vpi_get_str(পূর্ণাঙ্ক code)
{
      বিকল্প (code) {
	  পক্ষ vpiFullName:
	    প্রত্যাগম simple_set_rbuf_str(name);

	  স্বভাব:
	    প্রত্যাগম 0;
      }
}

শূন্য __vpiVThrVec4Stack::vpi_get_মান(p_vpi_মান vp)
{
      vvp_vector4_t val;

      যদি (vpip_current_vthread)
	    val = vthread_get_vec4_স্তূপ(vpip_current_vthread, depth_);

      বিকল্প (vp->format) {

	  পক্ষ vpiBinStrVal:
	    vpi_get_মান_binstr(vp, val);
	    ভঙ্গ;
	  পক্ষ vpiDecStrVal:
	    vpi_get_মান_decstr(vp, val);
	    ভঙ্গ;
	  পক্ষ vpiOctStrVal:
	    vpi_get_মান_octstr(vp, val);
	    ভঙ্গ;
	  পক্ষ vpiHexStrVal:
	    vpi_get_মান_hexstr(vp, val);
	    ভঙ্গ;
	  পক্ষ vpiIntVal:
	    vpi_get_মান_int(vp, val);
	    ভঙ্গ;
	  পক্ষ vpiRealVal:
	    vpi_get_মান_real(vp, val);
	    ভঙ্গ;
	  পক্ষ vpiStringVal:
	    vpi_get_মান_string(vp, val);
	    ভঙ্গ;
	  পক্ষ vpiStrengthVal:
	    vpi_get_মান_strength(vp, val);
	    ভঙ্গ;
	  পক্ষ vpiObjTypeVal:
	    vp->format = vpiVectorVal;
	    // fallthrough
	  পক্ষ vpiVectorVal:
	    vpi_get_মান_vector(vp, val);
	    ভঙ্গ;

	  স্বভাব:
	    fprintf(stderr, "internal error: vpi_get_value(<format=%d>)"
		    " not implemented for __vpiVThrVec4Stack.\n", vp->format);
	    প্রতিজ্ঞা(0);
      }

}

শূন্য __vpiVThrVec4Stack::vpi_get_মান_binstr(p_vpi_মান vp, ধ্রুব vvp_vector4_t&val)
{
      unsigned wid = val.size();
      বর্ণ*rbuf = static_cast<বর্ণ*>(need_result_buf(wid+1, RBUF_VAL));
      প্রতি (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    rbuf[wid-idx-1] = vvp_bit4_to_ascii(val.value(idx));
      }
      rbuf[wid] = 0;
      vp->value.str = rbuf;
}

শূন্য __vpiVThrVec4Stack::vpi_get_মান_decstr(p_vpi_মান vp, ধ্রুব vvp_vector4_t&val)
{
      unsigned wid = val.size();
      পূর্ণাঙ্ক nbuf = (wid+2)/3 + 1;
      বর্ণ *rbuf = static_cast<বর্ণ*>(need_result_buf(nbuf, RBUF_VAL));

      vpip_vec4_to_dec_str(val, rbuf, nbuf, signed_flag_);
      vp->value.str = rbuf;
}

শূন্য __vpiVThrVec4Stack::vpi_get_মান_octstr(p_vpi_মান vp, ধ্রুব vvp_vector4_t&val)
{
      unsigned wid = val.size();
      unsigned owid = (wid + 2) / 3;
      বর্ণ*rbuf = static_cast<বর্ণ*>(need_result_buf(owid+1, RBUF_VAL));
      rbuf[owid] = 0;

      unsigned oval = 0;
      প্রতি (unsigned idx = 0; idx < wid ; idx += 1) {
	    unsigned tmp = 0;
	    বিকল্প (val.value(idx)) {
		পক্ষ BIT4_0:
		  tmp = 0;
		  ভঙ্গ;
		পক্ষ BIT4_1:
		  tmp = 1;
		  ভঙ্গ;
		পক্ষ BIT4_X:
		  tmp = 2;
		  ভঙ্গ;
		পক্ষ BIT4_Z:
		  tmp = 3;
		  ভঙ্গ;
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

শূন্য __vpiVThrVec4Stack::vpi_get_মান_hexstr(p_vpi_মান vp, ধ্রুব vvp_vector4_t&val)
{
      unsigned wid = val.size();
      unsigned hwid = (wid + 3) /4;
      বর্ণ*rbuf = static_cast<বর্ণ*>(need_result_buf(hwid+1, RBUF_VAL));
      rbuf[hwid] = 0;

      unsigned hval = 0;
      প্রতি (unsigned idx = 0; idx < wid ; idx += 1) {
	    unsigned tmp = 0;
	    বিকল্প (val.value(idx)) {
		পক্ষ BIT4_0:
		  tmp = 0;
		  ভঙ্গ;
		পক্ষ BIT4_1:
		  tmp = 1;
		  ভঙ্গ;
		পক্ষ BIT4_X:
		  tmp = 2;
		  ভঙ্গ;
		পক্ষ BIT4_Z:
		  tmp = 3;
		  ভঙ্গ;
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

শূন্য __vpiVThrVec4Stack::vpi_get_মান_int(p_vpi_মান vp, ধ্রুব vvp_vector4_t&val)
{
      int32_t vali = 0;
      vector4_to_মান(val, vali, signed_flag_, অসত্য);
      vp->value.integer = vali;
}

শূন্য __vpiVThrVec4Stack::vpi_get_মান_real(p_vpi_মান vp, ধ্রুব vvp_vector4_t&val)
{
      unsigned wid = val.size();
      vp->value.real = 0.0;

      প্রতি (unsigned idx = wid ; idx > 0 ; idx -= 1) {
	    vp->value.real *= 2.0;
	    যদি (val.value(idx-1) == BIT4_1)
		  vp->value.real += 1.0;
      }
}

শূন্য __vpiVThrVec4Stack::vpi_get_মান_string(p_vpi_মান vp, ধ্রুব vvp_vector4_t&val)
{
      বর্ণ*rbuf = static_cast<বর্ণ*>(need_result_buf((val.size() / 8) + 1, RBUF_VAL));
      বর্ণ*cp = rbuf;

      বর্ণ tmp = 0;
      প্রতি (পূর্ণাঙ্ক bitnr = val.size()-1 ; bitnr >= 0 ; bitnr -= 1) {
	    tmp <<= 1;
	    বিকল্প (val.value(bitnr)) {
		পক্ষ BIT4_1:
		  tmp |= 1;
		  ভঙ্গ;
		পক্ষ BIT4_0:
		স্বভাব:
		  ভঙ্গ;
	    }

	    যদি ((bitnr&7)==0) {
		    // Don't include leading nuls
		  যদি (tmp == 0 && cp == rbuf)
			অনুবর্ত;

		  *cp++ = tmp? tmp : ' ';
		  tmp = 0;
	    }
      }
      *cp++ = 0;
      vp->format = vpiStringVal;
      vp->value.str = rbuf;
}

শূন্য __vpiVThrVec4Stack::vpi_get_মান_vector(p_vpi_মান vp, ধ্রুব vvp_vector4_t&val)
{
      unsigned wid = val.size();

      unsigned hwid = (wid+31)/32;
      vp->value.vector = static_cast<s_vpi_vecval*>
                         (need_result_buf(hwid*পরিমাণ(s_vpi_vecval), RBUF_VAL));
      প্রতিজ্ঞা(vp->value.vector);

	/* Zero the top of the result buffer first since the fill loop below only writes
	   bits 0..wid-1, which can leave stale data in the unused high bits. */
      যদি (hwid == 0) hwid = 1;
      vp->value.vector[hwid-1].aval = 0;
      vp->value.vector[hwid-1].bval = 0;

      প্রতি (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
	    পূর্ণাঙ্ক word = idx/32;
	    PLI_INT32 mask = 1 << (idx%32);

	    বিকল্প (val.value(idx)) {
		পক্ষ BIT4_0:
		  vp->value.vector[word].aval &= ~mask;
		  vp->value.vector[word].bval &= ~mask;
		  ভঙ্গ;
		পক্ষ BIT4_1:
		  vp->value.vector[word].aval |=  mask;
		  vp->value.vector[word].bval &= ~mask;
		  ভঙ্গ;
		পক্ষ BIT4_X:
		  vp->value.vector[word].aval |=  mask;
		  vp->value.vector[word].bval |=  mask;
		  ভঙ্গ;
		পক্ষ BIT4_Z:
		  vp->value.vector[word].aval &= ~mask;
		  vp->value.vector[word].bval |=  mask;
		  ভঙ্গ;
	    }
      }
}

শূন্য __vpiVThrVec4Stack::vpi_get_মান_strength(p_vpi_মান vp, ধ্রুব vvp_vector4_t&val)
{
      s_vpi_strengthval*op = static_cast<s_vpi_strengthval*>
                             (need_result_buf(val.size() * পরিমাণ(s_vpi_strengthval),
                                              RBUF_VAL));

      প্রতি (unsigned idx = 0 ; idx < val.size() ; idx += 1) {
	    বিকল্প (val.value(idx)) {
		পক্ষ BIT4_0:
		  op[idx].logic = vpi0;
		  op[idx].s0 = vpiStrongDrive;
		  op[idx].s1 = 0;
		  ভঙ্গ;
		পক্ষ BIT4_1:
		  op[idx].logic = vpi1;
		  op[idx].s0 = 0;
		  op[idx].s1 = vpiStrongDrive;
		  ভঙ্গ;
		পক্ষ BIT4_X:
		  op[idx].logic = vpiX;
		  op[idx].s0 = vpiStrongDrive;
		  op[idx].s1 = vpiStrongDrive;
		  ভঙ্গ;
		পক্ষ BIT4_Z:
		  op[idx].logic = vpiZ;
		  op[idx].s0 = vpiHiZ;
		  op[idx].s1 = vpiHiZ;
		  ভঙ্গ;
	    }
      }

      vp->format = vpiStrengthVal;
      vp->value.strength = op;
}

vpiHandle __vpiVThrVec4Stack::vpi_put_মান(p_vpi_মান vp, পূর্ণাঙ্ক /*flags*/)
{
      প্রতিজ্ঞা(vpip_current_vthread);

      বিকল্প (vp->format) {

	  স্বভাব:
	    fprintf(stderr, "internal error: vpi_put_value(<format=%d>)"
		    " not implemented for __vpiVThrVec4Stack.\n", vp->format);
	    প্রতিজ্ঞা(0);
	    প্রত্যাগম 0;
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
      map<vpiHandle, তার্কিক>::iterator iter;
      প্রতি (iter = handle_map.begin(); iter != handle_map.end(); ++ iter ) {
	    যদি (iter->second) thread_vthr_delete_real(iter->first);
	    অন্যথা thread_word_delete_real(iter->first);
      }
}
#endif


vpiHandle vpip_make_vthr_str_স্তূপ(unsigned depth)
{
      __vpiVThrStrStack*obj = নূতন __vpiVThrStrStack(depth);
      প্রত্যাগম obj;
}

vpiHandle vpip_make_vthr_vec4_স্তূপ(unsigned depth, তার্কিক signed_flag, unsigned wid)
{
      __vpiVThrVec4Stack*obj = নূতন __vpiVThrVec4Stack(depth, signed_flag, wid);
      প্রত্যাগম obj;
}

#ifdef CHECK_WITH_VALGRIND
স্থির map<vpiHandle, তার্কিক> স্তূপ_map;

শূন্য thread_string_delete(vpiHandle item)
{
      স্তূপ_map[item] = অসত্য;
}

স্থির শূন্য thread_string_delete_real(vpiHandle item)
{
      __vpiVThrStrStack*obj = dynamic_cast<__vpiVThrStrStack*>(item);
      delete obj;
}

শূন্য vpi_স্তূপ_delete()
{
      map<vpiHandle, তার্কিক>::iterator iter;
      প্রতি (iter = স্তূপ_map.begin(); iter != স্তূপ_map.end(); ++ iter ) {
	    thread_string_delete_real(iter->first);
      }
}
#endif
