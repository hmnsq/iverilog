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

using ନାମାକାଶ std;

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
ବାହ୍ୟ ଅଚଳ ଅକ୍ଷର hex_digits[256];
ବାହ୍ୟ ଅଚଳ ଅକ୍ଷର oct_digits[64];


ଶ୍ରେଣୀ __vpiVThrWord : ସର୍ବସାଧାରଣ __vpiHandle {
    ସର୍ବସାଧାରଣ:
      explicit __vpiVThrWord(unsigned base);
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଶୂନ୍ୟ vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val) override;

      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_subtype() ଅଚଳ { ଫେରନ୍ତୁ subtype; };
      unsigned get_index() ଅଚଳ { ଫେରନ୍ତୁ index; };

    ବ୍ୟକ୍ତିଗତ:
      ଅଚଳ ଅକ୍ଷର* name;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା subtype;
      unsigned index;
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା vthr_word_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code, vpiHandle ref)
{
      ଅଚଳ __vpiVThrWord*rfp = dynamic_cast<__vpiVThrWord*>(ref);

      ବାଛ (code) {

	  ସ୍ଥିତି vpiConstType:
	    ଫେରନ୍ତୁ rfp->get_subtype();

#ଯଦି defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  ସ୍ଥିତି _vpiFromThr:
	    ଫେରନ୍ତୁ _vpiWord;
#endif

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଫେରନ୍ତୁ 0;
      }
}

ସ୍ଥିର ଶୂନ୍ୟ vthr_real_get_ମୂଲ୍ୟ(vpiHandle ref, s_vpi_ମୂଲ୍ୟ*vp)
{
      ଅଚଳ __vpiVThrWord*obj = dynamic_cast<__vpiVThrWord*>(ref);
      ସ୍ଥିର ଅଚଳ size_t RBUF_USE_SIZE = 66;
      ଅକ୍ଷର *rbuf = static_cast<ଅକ୍ଷର *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));

      ଦ୍ୱିଗୁଣ val = 0.0;

	/* Get the actual value from the index. It is possible, by the
	   way, that the vpi_get_value is called from compiletf. If
	   that's the case, there will be no current thread, and this
	   will not have access to the proper value. Punt and return a
	   0.0 value instead. */
      ଯଦି (vpip_current_vthread)
	    val = vthread_get_real_ଗଦା(vpip_current_vthread,
	                                 obj->get_index());

      ବାଛ (vp->format) {

	  ସ୍ଥିତି vpiObjTypeVal:
	    vp->format = vpiRealVal;
	    // fallthrough
	  ସ୍ଥିତି vpiRealVal:
	    vp->value.real = val;
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiIntVal:
	      /* NaN or +/- infinity are translated as 0. */
	    ଯଦି (val != val || (val && (val == 0.5*val))) {
		  val = 0.0;
	    } ଅନ୍ୟଥା {
		  val = std::round(val);
	    }
	    vp->value.integer = (PLI_INT32)val;
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiDecStrVal:
	    ଯଦି (std::isnan(val))
		  snprintf(rbuf, RBUF_USE_SIZE, "%s", "nan");
	    ଅନ୍ୟଥା
		  snprintf(rbuf, RBUF_USE_SIZE, "%0.0f", std::round(val));
	    vp->value.str = rbuf;
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiOctStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" PRIo64, vlg_round_to_u64(val));
	    vp->value.str = rbuf;
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiHexStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" PRIx64, vlg_round_to_u64(val));
	    vp->value.str = rbuf;
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiBinStrVal: {
		uint64_t vali = vlg_round_to_u64(val);
		unsigned len = 0;

		ଯେତେବେଳେ (vali > 0) {
		      len += 1;
		      vali /= 2;
		}

		vali = vlg_round_to_u64(val);
		ପାଇଁ (unsigned idx = 0 ;  idx < len ;  idx += 1) {
		      rbuf[len-idx-1] = (vali & 1)? '1' : '0';
		      vali /= 2;
		}

		rbuf[len] = 0;
		ଯଦି (len == 0) {
		      rbuf[0] = '0';
		      rbuf[1] = 0;
		}
		vp->value.str = rbuf;
		ଭାଙ୍ଗ;
	  }

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiConstant (Real)\n", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vp->format);

	    vp->format = vpiSuppressVal;
	    ଭାଙ୍ଗ;
      }
}

ଧାଡ଼ିଭିତରେ __vpiVThrWord::__vpiVThrWord(unsigned base)
{
      name = vpip_name_string("W<>");
      subtype = vpiRealConst;
      index = base;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiVThrWord::get_type_code(ଶୂନ୍ୟ) ଅଚଳ
{ ଫେରନ୍ତୁ vpiConstant; }

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiVThrWord::vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{ ଫେରନ୍ତୁ vthr_word_get(code, ଏହି); }

ଶୂନ୍ୟ __vpiVThrWord::vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val)
{ vthr_real_get_ମୂଲ୍ୟ(ଏହି, val); }

vpiHandle vpip_make_vthr_word(unsigned base, ଅଚଳ ଅକ୍ଷର*type)
{
      ପ୍ରତିଜ୍ଞା(type[0] == 'r');
      ପ୍ରତିଜ୍ଞା(base < 65536);
      __vpiVThrWord*obj = ନୂତନ __vpiVThrWord(base);

      ଫେରନ୍ତୁ obj;
}

#ifdef CHECK_WITH_VALGRIND
ସ୍ଥିର map<vpiHandle, ତାର୍କିକ> handle_map;

ଶୂନ୍ୟ thread_word_delete(vpiHandle item)
{
      handle_map[item] = ମିଥ୍ୟା;
}

ସ୍ଥିର ଶୂନ୍ୟ thread_word_delete_real(vpiHandle item)
{
      ଶ୍ରେଣୀ __vpiVThrWord*obj = dynamic_cast<__vpiVThrWord*>(item);
      delete obj;
}
#endif

ଶ୍ରେଣୀ __vpiVThrStrStack : ସର୍ବସାଧାରଣ __vpiHandle {
    ସର୍ବସାଧାରଣ:
      explicit __vpiVThrStrStack(unsigned depth);
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଶୂନ୍ୟ vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val) override;
    ବ୍ୟକ୍ତିଗତ:
      unsigned depth_;
};

__vpiVThrStrStack::__vpiVThrStrStack(unsigned d)
: depth_(d)
{
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiVThrStrStack::get_type_code(ଶୂନ୍ୟ) ଅଚଳ
{ ଫେରନ୍ତୁ vpiConstant; }

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiVThrStrStack::vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ବାଛ (code) {
	  ସ୍ଥିତି vpiConstType:
	    ଫେରନ୍ତୁ vpiStringConst;
#ଯଦି defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  ସ୍ଥିତି _vpiFromThr:
	    ଫେରନ୍ତୁ _vpiString;
#endif
	  ସ୍ଥିତି vpiSize:
	    ଫେରନ୍ତୁ vthread_get_str_ଗଦା(vpip_current_vthread, depth_).size();
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଫେରନ୍ତୁ 0;
      }
}

ଶୂନ୍ୟ __vpiVThrStrStack::vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ vp)
{
      string val;
      ଅକ୍ଷର*rbuf = 0;

      ଯଦି (vpip_current_vthread)
	    val = vthread_get_str_ଗଦା(vpip_current_vthread, depth_);

      ବାଛ (vp->format) {

	  ସ୍ଥିତି vpiObjTypeVal:
	    vp->format = vpiStringVal;
	    // fallthrough
	  ସ୍ଥିତି vpiStringVal:
	    rbuf = static_cast<ଅକ୍ଷର *>(need_result_buf(val.size()+1, RBUF_VAL));
	    strcpy(rbuf, val.c_str());
	    vp->value.str = rbuf;
	    ଭାଙ୍ଗ;

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiConstant (String)\n", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vp->format);

	    vp->format = vpiSuppressVal;
	    ଭାଙ୍ଗ;
      }
}

ଶ୍ରେଣୀ __vpiVThrVec4Stack : ସର୍ବସାଧାରଣ __vpiHandle {
    ସର୍ବସାଧାରଣ:
      __vpiVThrVec4Stack(unsigned depth, ତାର୍କିକ signed_flag, unsigned wid);
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଅକ୍ଷର*vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଶୂନ୍ୟ vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val) override;
      vpiHandle vpi_put_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags) override;
    ବ୍ୟକ୍ତିଗତ:
      ସ୍ଥିର ଶୂନ୍ୟ vpi_get_ମୂଲ୍ୟ_string(p_vpi_ମୂଲ୍ୟ vp, ଅଚଳ vvp_vector4_t&val);
      ସ୍ଥିର ଶୂନ୍ୟ vpi_get_ମୂଲ୍ୟ_binstr(p_vpi_ମୂଲ୍ୟ vp, ଅଚଳ vvp_vector4_t&val);
      ଶୂନ୍ୟ vpi_get_ମୂଲ୍ୟ_decstr(p_vpi_ମୂଲ୍ୟ vp, ଅଚଳ vvp_vector4_t&val);
      ଶୂନ୍ୟ vpi_get_ମୂଲ୍ୟ_int   (p_vpi_ମୂଲ୍ୟ vp, ଅଚଳ vvp_vector4_t&val);
      ସ୍ଥିର ଶୂନ୍ୟ vpi_get_ମୂଲ୍ୟ_real  (p_vpi_ମୂଲ୍ୟ vp, ଅଚଳ vvp_vector4_t&val);
      ସ୍ଥିର ଶୂନ୍ୟ vpi_get_ମୂଲ୍ୟ_strength(p_vpi_ମୂଲ୍ୟ vp, ଅଚଳ vvp_vector4_t&val);
      ସ୍ଥିର ଶୂନ୍ୟ vpi_get_ମୂଲ୍ୟ_octstr(p_vpi_ମୂଲ୍ୟ vp, ଅଚଳ vvp_vector4_t&val);
      ସ୍ଥିର ଶୂନ୍ୟ vpi_get_ମୂଲ୍ୟ_hexstr(p_vpi_ମୂଲ୍ୟ vp, ଅଚଳ vvp_vector4_t&val);
      ସ୍ଥିର ଶୂନ୍ୟ vpi_get_ମୂଲ୍ୟ_vector(p_vpi_ମୂଲ୍ୟ vp, ଅଚଳ vvp_vector4_t&val);
    ବ୍ୟକ୍ତିଗତ:
      unsigned depth_;
      ତାର୍କିକ signed_flag_;
      unsigned expect_width_;
      ଅଚଳ ଅକ୍ଷର*name;
};

__vpiVThrVec4Stack::__vpiVThrVec4Stack(unsigned d, ତାର୍କିକ sf, unsigned wid)
: depth_(d), signed_flag_(sf), expect_width_(wid)
{
      name = vpip_name_string("S<,vec4,>");
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiVThrVec4Stack::get_type_code(ଶୂନ୍ୟ) ଅଚଳ
{ ଫେରନ୍ତୁ vpiConstant; }


ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiVThrVec4Stack::vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ବାଛ (code) {
	  ସ୍ଥିତି vpiSize:
	    ଫେରନ୍ତୁ expect_width_;

	  ସ୍ଥିତି vpiSigned:
	    ଫେରନ୍ତୁ signed_flag_? 1 : 0;

	  ସ୍ଥିତି vpiConstType:
	    ଫେରନ୍ତୁ vpi_ଦ୍ୱିମିକ_Const;

#ଯଦି defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  ସ୍ଥିତି _vpiFromThr:
	    ଫେରନ୍ତୁ _vpiVThr;
#endif

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଫେରନ୍ତୁ 0;
      }
}

ଅକ୍ଷର*__vpiVThrVec4Stack::vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ବାଛ (code) {
	  ସ୍ଥିତି vpiFullName:
	    ଫେରନ୍ତୁ simple_set_rbuf_str(name);

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଫେରନ୍ତୁ 0;
      }
}

ଶୂନ୍ୟ __vpiVThrVec4Stack::vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ vp)
{
      vvp_vector4_t val;

      ଯଦି (vpip_current_vthread)
	    val = vthread_get_vec4_ଗଦା(vpip_current_vthread, depth_);

      ବାଛ (vp->format) {

	  ସ୍ଥିତି vpiBinStrVal:
	    vpi_get_ମୂଲ୍ୟ_binstr(vp, val);
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି vpiDecStrVal:
	    vpi_get_ମୂଲ୍ୟ_decstr(vp, val);
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି vpiOctStrVal:
	    vpi_get_ମୂଲ୍ୟ_octstr(vp, val);
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି vpiHexStrVal:
	    vpi_get_ମୂଲ୍ୟ_hexstr(vp, val);
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି vpiIntVal:
	    vpi_get_ମୂଲ୍ୟ_int(vp, val);
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି vpiRealVal:
	    vpi_get_ମୂଲ୍ୟ_real(vp, val);
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି vpiStringVal:
	    vpi_get_ମୂଲ୍ୟ_string(vp, val);
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି vpiStrengthVal:
	    vpi_get_ମୂଲ୍ୟ_strength(vp, val);
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି vpiObjTypeVal:
	    vp->format = vpiVectorVal;
	    // fallthrough
	  ସ୍ଥିତି vpiVectorVal:
	    vpi_get_ମୂଲ୍ୟ_vector(vp, val);
	    ଭାଙ୍ଗ;

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "internal error: vpi_get_value(<format=%d>)"
		    " not implemented for __vpiVThrVec4Stack.\n", vp->format);
	    ପ୍ରତିଜ୍ଞା(0);
      }

}

ଶୂନ୍ୟ __vpiVThrVec4Stack::vpi_get_ମୂଲ୍ୟ_binstr(p_vpi_ମୂଲ୍ୟ vp, ଅଚଳ vvp_vector4_t&val)
{
      unsigned wid = val.size();
      ଅକ୍ଷର*rbuf = static_cast<ଅକ୍ଷର*>(need_result_buf(wid+1, RBUF_VAL));
      ପାଇଁ (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    rbuf[wid-idx-1] = vvp_bit4_to_ascii(val.value(idx));
      }
      rbuf[wid] = 0;
      vp->value.str = rbuf;
}

ଶୂନ୍ୟ __vpiVThrVec4Stack::vpi_get_ମୂଲ୍ୟ_decstr(p_vpi_ମୂଲ୍ୟ vp, ଅଚଳ vvp_vector4_t&val)
{
      unsigned wid = val.size();
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା nbuf = (wid+2)/3 + 1;
      ଅକ୍ଷର *rbuf = static_cast<ଅକ୍ଷର*>(need_result_buf(nbuf, RBUF_VAL));

      vpip_vec4_to_dec_str(val, rbuf, nbuf, signed_flag_);
      vp->value.str = rbuf;
}

ଶୂନ୍ୟ __vpiVThrVec4Stack::vpi_get_ମୂଲ୍ୟ_octstr(p_vpi_ମୂଲ୍ୟ vp, ଅଚଳ vvp_vector4_t&val)
{
      unsigned wid = val.size();
      unsigned owid = (wid + 2) / 3;
      ଅକ୍ଷର*rbuf = static_cast<ଅକ୍ଷର*>(need_result_buf(owid+1, RBUF_VAL));
      rbuf[owid] = 0;

      unsigned oval = 0;
      ପାଇଁ (unsigned idx = 0; idx < wid ; idx += 1) {
	    unsigned tmp = 0;
	    ବାଛ (val.value(idx)) {
		ସ୍ଥିତି BIT4_0:
		  tmp = 0;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି BIT4_1:
		  tmp = 1;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି BIT4_X:
		  tmp = 2;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି BIT4_Z:
		  tmp = 3;
		  ଭାଙ୍ଗ;
	    }

	    oval = oval | (tmp << 2*(idx%3));

	    ଯଦି (idx%3 == 2) {
		  owid -= 1;
		  rbuf[owid] = oct_digits[oval];
		  oval = 0;
	    }
      }

      ଯଦି (owid > 0) {
	    owid -= 1;
	    rbuf[owid] = oct_digits[oval];
      }
      vp->value.str = rbuf;
}

ଶୂନ୍ୟ __vpiVThrVec4Stack::vpi_get_ମୂଲ୍ୟ_hexstr(p_vpi_ମୂଲ୍ୟ vp, ଅଚଳ vvp_vector4_t&val)
{
      unsigned wid = val.size();
      unsigned hwid = (wid + 3) /4;
      ଅକ୍ଷର*rbuf = static_cast<ଅକ୍ଷର*>(need_result_buf(hwid+1, RBUF_VAL));
      rbuf[hwid] = 0;

      unsigned hval = 0;
      ପାଇଁ (unsigned idx = 0; idx < wid ; idx += 1) {
	    unsigned tmp = 0;
	    ବାଛ (val.value(idx)) {
		ସ୍ଥିତି BIT4_0:
		  tmp = 0;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି BIT4_1:
		  tmp = 1;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି BIT4_X:
		  tmp = 2;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି BIT4_Z:
		  tmp = 3;
		  ଭାଙ୍ଗ;
	    }
	    hval = hval | (tmp << 2*(idx%4));

	    ଯଦି (idx%4 == 3) {
		  hwid -= 1;
		  rbuf[hwid] = hex_digits[hval];
		  hval = 0;
	    }
      }

      ଯଦି (hwid > 0) {
	    hwid -= 1;
	    rbuf[hwid] = hex_digits[hval];
      }
      vp->value.str = rbuf;
}

ଶୂନ୍ୟ __vpiVThrVec4Stack::vpi_get_ମୂଲ୍ୟ_int(p_vpi_ମୂଲ୍ୟ vp, ଅଚଳ vvp_vector4_t&val)
{
      int32_t vali = 0;
      vector4_to_ମୂଲ୍ୟ(val, vali, signed_flag_, ମିଥ୍ୟା);
      vp->value.integer = vali;
}

ଶୂନ୍ୟ __vpiVThrVec4Stack::vpi_get_ମୂଲ୍ୟ_real(p_vpi_ମୂଲ୍ୟ vp, ଅଚଳ vvp_vector4_t&val)
{
      unsigned wid = val.size();
      vp->value.real = 0.0;

      ପାଇଁ (unsigned idx = wid ; idx > 0 ; idx -= 1) {
	    vp->value.real *= 2.0;
	    ଯଦି (val.value(idx-1) == BIT4_1)
		  vp->value.real += 1.0;
      }
}

ଶୂନ୍ୟ __vpiVThrVec4Stack::vpi_get_ମୂଲ୍ୟ_string(p_vpi_ମୂଲ୍ୟ vp, ଅଚଳ vvp_vector4_t&val)
{
      ଅକ୍ଷର*rbuf = static_cast<ଅକ୍ଷର*>(need_result_buf((val.size() / 8) + 1, RBUF_VAL));
      ଅକ୍ଷର*cp = rbuf;

      ଅକ୍ଷର tmp = 0;
      ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା bitnr = val.size()-1 ; bitnr >= 0 ; bitnr -= 1) {
	    tmp <<= 1;
	    ବାଛ (val.value(bitnr)) {
		ସ୍ଥିତି BIT4_1:
		  tmp |= 1;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି BIT4_0:
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		  ଭାଙ୍ଗ;
	    }

	    ଯଦି ((bitnr&7)==0) {
		    // Don't include leading nuls
		  ଯଦି (tmp == 0 && cp == rbuf)
			ଜାରି;

		  *cp++ = tmp? tmp : ' ';
		  tmp = 0;
	    }
      }
      *cp++ = 0;
      vp->format = vpiStringVal;
      vp->value.str = rbuf;
}

ଶୂନ୍ୟ __vpiVThrVec4Stack::vpi_get_ମୂଲ୍ୟ_vector(p_vpi_ମୂଲ୍ୟ vp, ଅଚଳ vvp_vector4_t&val)
{
      unsigned wid = val.size();

      unsigned hwid = (wid+31)/32;
      vp->value.vector = static_cast<s_vpi_vecval*>
                         (need_result_buf(hwid*ଆକାର(s_vpi_vecval), RBUF_VAL));
      ପ୍ରତିଜ୍ଞା(vp->value.vector);

	/* Zero the top of the result buffer first since the fill loop below only writes
	   bits 0..wid-1, which can leave stale data in the unused high bits. */
      ଯଦି (hwid == 0) hwid = 1;
      vp->value.vector[hwid-1].aval = 0;
      vp->value.vector[hwid-1].bval = 0;

      ପାଇଁ (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା word = idx/32;
	    PLI_INT32 mask = 1 << (idx%32);

	    ବାଛ (val.value(idx)) {
		ସ୍ଥିତି BIT4_0:
		  vp->value.vector[word].aval &= ~mask;
		  vp->value.vector[word].bval &= ~mask;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି BIT4_1:
		  vp->value.vector[word].aval |=  mask;
		  vp->value.vector[word].bval &= ~mask;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି BIT4_X:
		  vp->value.vector[word].aval |=  mask;
		  vp->value.vector[word].bval |=  mask;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି BIT4_Z:
		  vp->value.vector[word].aval &= ~mask;
		  vp->value.vector[word].bval |=  mask;
		  ଭାଙ୍ଗ;
	    }
      }
}

ଶୂନ୍ୟ __vpiVThrVec4Stack::vpi_get_ମୂଲ୍ୟ_strength(p_vpi_ମୂଲ୍ୟ vp, ଅଚଳ vvp_vector4_t&val)
{
      s_vpi_strengthval*op = static_cast<s_vpi_strengthval*>
                             (need_result_buf(val.size() * ଆକାର(s_vpi_strengthval),
                                              RBUF_VAL));

      ପାଇଁ (unsigned idx = 0 ; idx < val.size() ; idx += 1) {
	    ବାଛ (val.value(idx)) {
		ସ୍ଥିତି BIT4_0:
		  op[idx].logic = vpi0;
		  op[idx].s0 = vpiStrongDrive;
		  op[idx].s1 = 0;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି BIT4_1:
		  op[idx].logic = vpi1;
		  op[idx].s0 = 0;
		  op[idx].s1 = vpiStrongDrive;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି BIT4_X:
		  op[idx].logic = vpiX;
		  op[idx].s0 = vpiStrongDrive;
		  op[idx].s1 = vpiStrongDrive;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି BIT4_Z:
		  op[idx].logic = vpiZ;
		  op[idx].s0 = vpiHiZ;
		  op[idx].s1 = vpiHiZ;
		  ଭାଙ୍ଗ;
	    }
      }

      vp->format = vpiStrengthVal;
      vp->value.strength = op;
}

vpiHandle __vpiVThrVec4Stack::vpi_put_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ vp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା /*flags*/)
{
      ପ୍ରତିଜ୍ଞା(vpip_current_vthread);

      ବାଛ (vp->format) {

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "internal error: vpi_put_value(<format=%d>)"
		    " not implemented for __vpiVThrVec4Stack.\n", vp->format);
	    ପ୍ରତିଜ୍ଞା(0);
	    ଫେରନ୍ତୁ 0;
      }
}

#ifdef CHECK_WITH_VALGRIND
ଶୂନ୍ୟ thread_vthr_delete(vpiHandle item)
{
      handle_map[item] = ସତ୍ୟ;
}

ସ୍ଥିର ଶୂନ୍ୟ thread_vthr_delete_real(vpiHandle item)
{
      __vpiVThrVec4Stack*obj = dynamic_cast<__vpiVThrVec4Stack*>(item);
      delete obj;
}

ଶୂନ୍ୟ vpi_handle_delete()
{
      map<vpiHandle, ତାର୍କିକ>::iterator iter;
      ପାଇଁ (iter = handle_map.begin(); iter != handle_map.end(); ++ iter ) {
	    ଯଦି (iter->second) thread_vthr_delete_real(iter->first);
	    ଅନ୍ୟଥା thread_word_delete_real(iter->first);
      }
}
#endif


vpiHandle vpip_make_vthr_str_ଗଦା(unsigned depth)
{
      __vpiVThrStrStack*obj = ନୂତନ __vpiVThrStrStack(depth);
      ଫେରନ୍ତୁ obj;
}

vpiHandle vpip_make_vthr_vec4_ଗଦା(unsigned depth, ତାର୍କିକ signed_flag, unsigned wid)
{
      __vpiVThrVec4Stack*obj = ନୂତନ __vpiVThrVec4Stack(depth, signed_flag, wid);
      ଫେରନ୍ତୁ obj;
}

#ifdef CHECK_WITH_VALGRIND
ସ୍ଥିର map<vpiHandle, ତାର୍କିକ> ଗଦା_map;

ଶୂନ୍ୟ thread_string_delete(vpiHandle item)
{
      ଗଦା_map[item] = ମିଥ୍ୟା;
}

ସ୍ଥିର ଶୂନ୍ୟ thread_string_delete_real(vpiHandle item)
{
      __vpiVThrStrStack*obj = dynamic_cast<__vpiVThrStrStack*>(item);
      delete obj;
}

ଶୂନ୍ୟ vpi_ଗଦା_delete()
{
      map<vpiHandle, ତାର୍କିକ>::iterator iter;
      ପାଇଁ (iter = ଗଦା_map.begin(); iter != ଗଦା_map.end(); ++ iter ) {
	    thread_string_delete_real(iter->first);
      }
}
#endif
