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

using नामाकाश std;

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
बाह्य ध्रुव वर्ण hex_digits[256];
बाह्य ध्रुव वर्ण oct_digits[64];


वर्ग __vpiVThrWord : सार्वजनिक __vpiHandle {
    सार्वजनिक:
      explicit __vpiVThrWord(unsigned base);
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override;
      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override;
      शून्य vpi_get_मानम्(p_vpi_मानम् val) override;

      पूर्णाङ्क get_subtype() ध्रुव { प्रत्यागम subtype; };
      unsigned get_index() ध्रुव { प्रत्यागम index; };

    गुप्त:
      ध्रुव वर्ण* name;
      पूर्णाङ्क subtype;
      unsigned index;
};

स्थिर पूर्णाङ्क vthr_word_get(पूर्णाङ्क code, vpiHandle ref)
{
      ध्रुव __vpiVThrWord*rfp = dynamic_cast<__vpiVThrWord*>(ref);

      विकल्प (code) {

	  पक्ष vpiConstType:
	    प्रत्यागम rfp->get_subtype();

#जइ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  पक्ष _vpiFromThr:
	    प्रत्यागम _vpiWord;
#endif

	  स्वभाव:
	    प्रत्यागम 0;
      }
}

स्थिर शून्य vthr_real_get_मानम्(vpiHandle ref, s_vpi_मानम्*vp)
{
      ध्रुव __vpiVThrWord*obj = dynamic_cast<__vpiVThrWord*>(ref);
      स्थिर ध्रुव size_t RBUF_USE_SIZE = 66;
      वर्ण *rbuf = static_cast<वर्ण *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));

      द्विगुण val = 0.0;

	/* Get the actual value from the index. It is possible, by the
	   way, that the vpi_get_value is called from compiletf. If
	   that's the case, there will be no current thread, and this
	   will not have access to the proper value. Punt and return a
	   0.0 value instead. */
      जइ (vpip_current_vthread)
	    val = vthread_get_real_राशिः(vpip_current_vthread,
	                                 obj->get_index());

      विकल्प (vp->format) {

	  पक्ष vpiObjTypeVal:
	    vp->format = vpiRealVal;
	    // fallthrough
	  पक्ष vpiRealVal:
	    vp->value.real = val;
	    भङ्ग;

	  पक्ष vpiIntVal:
	      /* NaN or +/- infinity are translated as 0. */
	    जइ (val != val || (val && (val == 0.5*val))) {
		  val = 0.0;
	    } अण्णहा {
		  val = std::round(val);
	    }
	    vp->value.integer = (PLI_INT32)val;
	    भङ्ग;

	  पक्ष vpiDecStrVal:
	    जइ (std::isnan(val))
		  snprintf(rbuf, RBUF_USE_SIZE, "%s", "nan");
	    अण्णहा
		  snprintf(rbuf, RBUF_USE_SIZE, "%0.0f", std::round(val));
	    vp->value.str = rbuf;
	    भङ्ग;

	  पक्ष vpiOctStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" PRIo64, vlg_round_to_u64(val));
	    vp->value.str = rbuf;
	    भङ्ग;

	  पक्ष vpiHexStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" PRIx64, vlg_round_to_u64(val));
	    vp->value.str = rbuf;
	    भङ्ग;

	  पक्ष vpiBinStrVal: {
		uint64_t vali = vlg_round_to_u64(val);
		unsigned len = 0;

		जाव (vali > 0) {
		      len += 1;
		      vali /= 2;
		}

		vali = vlg_round_to_u64(val);
		प्रति (unsigned idx = 0 ;  idx < len ;  idx += 1) {
		      rbuf[len-idx-1] = (vali & 1)? '1' : '0';
		      vali /= 2;
		}

		rbuf[len] = 0;
		जइ (len == 0) {
		      rbuf[0] = '0';
		      rbuf[1] = 0;
		}
		vp->value.str = rbuf;
		भङ्ग;
	  }

	  स्वभाव:
	    fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiConstant (Real)\n", (पूर्णाङ्क)vp->format);

	    vp->format = vpiSuppressVal;
	    भङ्ग;
      }
}

अन्तःस्थ __vpiVThrWord::__vpiVThrWord(unsigned base)
{
      name = vpip_name_string("W<>");
      subtype = vpiRealConst;
      index = base;
}

पूर्णाङ्क __vpiVThrWord::get_type_code(शून्य) ध्रुव
{ प्रत्यागम vpiConstant; }

पूर्णाङ्क __vpiVThrWord::vpi_get(पूर्णाङ्क code)
{ प्रत्यागम vthr_word_get(code, एतत्); }

शून्य __vpiVThrWord::vpi_get_मानम्(p_vpi_मानम् val)
{ vthr_real_get_मानम्(एतत्, val); }

vpiHandle vpip_make_vthr_word(unsigned base, ध्रुव वर्ण*type)
{
      प्रतिज्ञा(type[0] == 'r');
      प्रतिज्ञा(base < 65536);
      __vpiVThrWord*obj = नूतन __vpiVThrWord(base);

      प्रत्यागम obj;
}

#ifdef CHECK_WITH_VALGRIND
स्थिर map<vpiHandle, तार्किक> handle_map;

शून्य thread_word_delete(vpiHandle item)
{
      handle_map[item] = मिच्छ;
}

स्थिर शून्य thread_word_delete_real(vpiHandle item)
{
      वर्ग __vpiVThrWord*obj = dynamic_cast<__vpiVThrWord*>(item);
      delete obj;
}
#endif

वर्ग __vpiVThrStrStack : सार्वजनिक __vpiHandle {
    सार्वजनिक:
      explicit __vpiVThrStrStack(unsigned depth);
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override;
      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override;
      शून्य vpi_get_मानम्(p_vpi_मानम् val) override;
    गुप्त:
      unsigned depth_;
};

__vpiVThrStrStack::__vpiVThrStrStack(unsigned d)
: depth_(d)
{
}

पूर्णाङ्क __vpiVThrStrStack::get_type_code(शून्य) ध्रुव
{ प्रत्यागम vpiConstant; }

पूर्णाङ्क __vpiVThrStrStack::vpi_get(पूर्णाङ्क code)
{
      विकल्प (code) {
	  पक्ष vpiConstType:
	    प्रत्यागम vpiStringConst;
#जइ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  पक्ष _vpiFromThr:
	    प्रत्यागम _vpiString;
#endif
	  पक्ष vpiSize:
	    प्रत्यागम vthread_get_str_राशिः(vpip_current_vthread, depth_).size();
	  स्वभाव:
	    प्रत्यागम 0;
      }
}

शून्य __vpiVThrStrStack::vpi_get_मानम्(p_vpi_मानम् vp)
{
      string val;
      वर्ण*rbuf = 0;

      जइ (vpip_current_vthread)
	    val = vthread_get_str_राशिः(vpip_current_vthread, depth_);

      विकल्प (vp->format) {

	  पक्ष vpiObjTypeVal:
	    vp->format = vpiStringVal;
	    // fallthrough
	  पक्ष vpiStringVal:
	    rbuf = static_cast<वर्ण *>(need_result_buf(val.size()+1, RBUF_VAL));
	    strcpy(rbuf, val.c_str());
	    vp->value.str = rbuf;
	    भङ्ग;

	  स्वभाव:
	    fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiConstant (String)\n", (पूर्णाङ्क)vp->format);

	    vp->format = vpiSuppressVal;
	    भङ्ग;
      }
}

वर्ग __vpiVThrVec4Stack : सार्वजनिक __vpiHandle {
    सार्वजनिक:
      __vpiVThrVec4Stack(unsigned depth, तार्किक signed_flag, unsigned wid);
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override;
      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override;
      वर्ण*vpi_get_str(पूर्णाङ्क code) override;
      शून्य vpi_get_मानम्(p_vpi_मानम् val) override;
      vpiHandle vpi_put_मानम्(p_vpi_मानम् val, पूर्णाङ्क flags) override;
    गुप्त:
      स्थिर शून्य vpi_get_मानम्_string(p_vpi_मानम् vp, ध्रुव vvp_vector4_t&val);
      स्थिर शून्य vpi_get_मानम्_binstr(p_vpi_मानम् vp, ध्रुव vvp_vector4_t&val);
      शून्य vpi_get_मानम्_decstr(p_vpi_मानम् vp, ध्रुव vvp_vector4_t&val);
      शून्य vpi_get_मानम्_int   (p_vpi_मानम् vp, ध्रुव vvp_vector4_t&val);
      स्थिर शून्य vpi_get_मानम्_real  (p_vpi_मानम् vp, ध्रुव vvp_vector4_t&val);
      स्थिर शून्य vpi_get_मानम्_strength(p_vpi_मानम् vp, ध्रुव vvp_vector4_t&val);
      स्थिर शून्य vpi_get_मानम्_octstr(p_vpi_मानम् vp, ध्रुव vvp_vector4_t&val);
      स्थिर शून्य vpi_get_मानम्_hexstr(p_vpi_मानम् vp, ध्रुव vvp_vector4_t&val);
      स्थिर शून्य vpi_get_मानम्_vector(p_vpi_मानम् vp, ध्रुव vvp_vector4_t&val);
    गुप्त:
      unsigned depth_;
      तार्किक signed_flag_;
      unsigned expect_width_;
      ध्रुव वर्ण*name;
};

__vpiVThrVec4Stack::__vpiVThrVec4Stack(unsigned d, तार्किक sf, unsigned wid)
: depth_(d), signed_flag_(sf), expect_width_(wid)
{
      name = vpip_name_string("S<,vec4,>");
}

पूर्णाङ्क __vpiVThrVec4Stack::get_type_code(शून्य) ध्रुव
{ प्रत्यागम vpiConstant; }


पूर्णाङ्क __vpiVThrVec4Stack::vpi_get(पूर्णाङ्क code)
{
      विकल्प (code) {
	  पक्ष vpiSize:
	    प्रत्यागम expect_width_;

	  पक्ष vpiSigned:
	    प्रत्यागम signed_flag_? 1 : 0;

	  पक्ष vpiConstType:
	    प्रत्यागम vpi_द्विमान_Const;

#जइ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  पक्ष _vpiFromThr:
	    प्रत्यागम _vpiVThr;
#endif

	  स्वभाव:
	    प्रत्यागम 0;
      }
}

वर्ण*__vpiVThrVec4Stack::vpi_get_str(पूर्णाङ्क code)
{
      विकल्प (code) {
	  पक्ष vpiFullName:
	    प्रत्यागम simple_set_rbuf_str(name);

	  स्वभाव:
	    प्रत्यागम 0;
      }
}

शून्य __vpiVThrVec4Stack::vpi_get_मानम्(p_vpi_मानम् vp)
{
      vvp_vector4_t val;

      जइ (vpip_current_vthread)
	    val = vthread_get_vec4_राशिः(vpip_current_vthread, depth_);

      विकल्प (vp->format) {

	  पक्ष vpiBinStrVal:
	    vpi_get_मानम्_binstr(vp, val);
	    भङ्ग;
	  पक्ष vpiDecStrVal:
	    vpi_get_मानम्_decstr(vp, val);
	    भङ्ग;
	  पक्ष vpiOctStrVal:
	    vpi_get_मानम्_octstr(vp, val);
	    भङ्ग;
	  पक्ष vpiHexStrVal:
	    vpi_get_मानम्_hexstr(vp, val);
	    भङ्ग;
	  पक्ष vpiIntVal:
	    vpi_get_मानम्_int(vp, val);
	    भङ्ग;
	  पक्ष vpiRealVal:
	    vpi_get_मानम्_real(vp, val);
	    भङ्ग;
	  पक्ष vpiStringVal:
	    vpi_get_मानम्_string(vp, val);
	    भङ्ग;
	  पक्ष vpiStrengthVal:
	    vpi_get_मानम्_strength(vp, val);
	    भङ्ग;
	  पक्ष vpiObjTypeVal:
	    vp->format = vpiVectorVal;
	    // fallthrough
	  पक्ष vpiVectorVal:
	    vpi_get_मानम्_vector(vp, val);
	    भङ्ग;

	  स्वभाव:
	    fprintf(stderr, "internal error: vpi_get_value(<format=%d>)"
		    " not implemented for __vpiVThrVec4Stack.\n", vp->format);
	    प्रतिज्ञा(0);
      }

}

शून्य __vpiVThrVec4Stack::vpi_get_मानम्_binstr(p_vpi_मानम् vp, ध्रुव vvp_vector4_t&val)
{
      unsigned wid = val.size();
      वर्ण*rbuf = static_cast<वर्ण*>(need_result_buf(wid+1, RBUF_VAL));
      प्रति (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    rbuf[wid-idx-1] = vvp_bit4_to_ascii(val.value(idx));
      }
      rbuf[wid] = 0;
      vp->value.str = rbuf;
}

शून्य __vpiVThrVec4Stack::vpi_get_मानम्_decstr(p_vpi_मानम् vp, ध्रुव vvp_vector4_t&val)
{
      unsigned wid = val.size();
      पूर्णाङ्क nbuf = (wid+2)/3 + 1;
      वर्ण *rbuf = static_cast<वर्ण*>(need_result_buf(nbuf, RBUF_VAL));

      vpip_vec4_to_dec_str(val, rbuf, nbuf, signed_flag_);
      vp->value.str = rbuf;
}

शून्य __vpiVThrVec4Stack::vpi_get_मानम्_octstr(p_vpi_मानम् vp, ध्रुव vvp_vector4_t&val)
{
      unsigned wid = val.size();
      unsigned owid = (wid + 2) / 3;
      वर्ण*rbuf = static_cast<वर्ण*>(need_result_buf(owid+1, RBUF_VAL));
      rbuf[owid] = 0;

      unsigned oval = 0;
      प्रति (unsigned idx = 0; idx < wid ; idx += 1) {
	    unsigned tmp = 0;
	    विकल्प (val.value(idx)) {
		पक्ष BIT4_0:
		  tmp = 0;
		  भङ्ग;
		पक्ष BIT4_1:
		  tmp = 1;
		  भङ्ग;
		पक्ष BIT4_X:
		  tmp = 2;
		  भङ्ग;
		पक्ष BIT4_Z:
		  tmp = 3;
		  भङ्ग;
	    }

	    oval = oval | (tmp << 2*(idx%3));

	    जइ (idx%3 == 2) {
		  owid -= 1;
		  rbuf[owid] = oct_digits[oval];
		  oval = 0;
	    }
      }

      जइ (owid > 0) {
	    owid -= 1;
	    rbuf[owid] = oct_digits[oval];
      }
      vp->value.str = rbuf;
}

शून्य __vpiVThrVec4Stack::vpi_get_मानम्_hexstr(p_vpi_मानम् vp, ध्रुव vvp_vector4_t&val)
{
      unsigned wid = val.size();
      unsigned hwid = (wid + 3) /4;
      वर्ण*rbuf = static_cast<वर्ण*>(need_result_buf(hwid+1, RBUF_VAL));
      rbuf[hwid] = 0;

      unsigned hval = 0;
      प्रति (unsigned idx = 0; idx < wid ; idx += 1) {
	    unsigned tmp = 0;
	    विकल्प (val.value(idx)) {
		पक्ष BIT4_0:
		  tmp = 0;
		  भङ्ग;
		पक्ष BIT4_1:
		  tmp = 1;
		  भङ्ग;
		पक्ष BIT4_X:
		  tmp = 2;
		  भङ्ग;
		पक्ष BIT4_Z:
		  tmp = 3;
		  भङ्ग;
	    }
	    hval = hval | (tmp << 2*(idx%4));

	    जइ (idx%4 == 3) {
		  hwid -= 1;
		  rbuf[hwid] = hex_digits[hval];
		  hval = 0;
	    }
      }

      जइ (hwid > 0) {
	    hwid -= 1;
	    rbuf[hwid] = hex_digits[hval];
      }
      vp->value.str = rbuf;
}

शून्य __vpiVThrVec4Stack::vpi_get_मानम्_int(p_vpi_मानम् vp, ध्रुव vvp_vector4_t&val)
{
      int32_t vali = 0;
      vector4_to_मानम्(val, vali, signed_flag_, मिच्छ);
      vp->value.integer = vali;
}

शून्य __vpiVThrVec4Stack::vpi_get_मानम्_real(p_vpi_मानम् vp, ध्रुव vvp_vector4_t&val)
{
      unsigned wid = val.size();
      vp->value.real = 0.0;

      प्रति (unsigned idx = wid ; idx > 0 ; idx -= 1) {
	    vp->value.real *= 2.0;
	    जइ (val.value(idx-1) == BIT4_1)
		  vp->value.real += 1.0;
      }
}

शून्य __vpiVThrVec4Stack::vpi_get_मानम्_string(p_vpi_मानम् vp, ध्रुव vvp_vector4_t&val)
{
      वर्ण*rbuf = static_cast<वर्ण*>(need_result_buf((val.size() / 8) + 1, RBUF_VAL));
      वर्ण*cp = rbuf;

      वर्ण tmp = 0;
      प्रति (पूर्णाङ्क bitnr = val.size()-1 ; bitnr >= 0 ; bitnr -= 1) {
	    tmp <<= 1;
	    विकल्प (val.value(bitnr)) {
		पक्ष BIT4_1:
		  tmp |= 1;
		  भङ्ग;
		पक्ष BIT4_0:
		स्वभाव:
		  भङ्ग;
	    }

	    जइ ((bitnr&7)==0) {
		    // Don't include leading nuls
		  जइ (tmp == 0 && cp == rbuf)
			अनुवर्त;

		  *cp++ = tmp? tmp : ' ';
		  tmp = 0;
	    }
      }
      *cp++ = 0;
      vp->format = vpiStringVal;
      vp->value.str = rbuf;
}

शून्य __vpiVThrVec4Stack::vpi_get_मानम्_vector(p_vpi_मानम् vp, ध्रुव vvp_vector4_t&val)
{
      unsigned wid = val.size();

      unsigned hwid = (wid+31)/32;
      vp->value.vector = static_cast<s_vpi_vecval*>
                         (need_result_buf(hwid*परिमाण(s_vpi_vecval), RBUF_VAL));
      प्रतिज्ञा(vp->value.vector);

	/* Zero the top of the result buffer first since the fill loop below only writes
	   bits 0..wid-1, which can leave stale data in the unused high bits. */
      जइ (hwid == 0) hwid = 1;
      vp->value.vector[hwid-1].aval = 0;
      vp->value.vector[hwid-1].bval = 0;

      प्रति (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
	    पूर्णाङ्क word = idx/32;
	    PLI_INT32 mask = 1 << (idx%32);

	    विकल्प (val.value(idx)) {
		पक्ष BIT4_0:
		  vp->value.vector[word].aval &= ~mask;
		  vp->value.vector[word].bval &= ~mask;
		  भङ्ग;
		पक्ष BIT4_1:
		  vp->value.vector[word].aval |=  mask;
		  vp->value.vector[word].bval &= ~mask;
		  भङ्ग;
		पक्ष BIT4_X:
		  vp->value.vector[word].aval |=  mask;
		  vp->value.vector[word].bval |=  mask;
		  भङ्ग;
		पक्ष BIT4_Z:
		  vp->value.vector[word].aval &= ~mask;
		  vp->value.vector[word].bval |=  mask;
		  भङ्ग;
	    }
      }
}

शून्य __vpiVThrVec4Stack::vpi_get_मानम्_strength(p_vpi_मानम् vp, ध्रुव vvp_vector4_t&val)
{
      s_vpi_strengthval*op = static_cast<s_vpi_strengthval*>
                             (need_result_buf(val.size() * परिमाण(s_vpi_strengthval),
                                              RBUF_VAL));

      प्रति (unsigned idx = 0 ; idx < val.size() ; idx += 1) {
	    विकल्प (val.value(idx)) {
		पक्ष BIT4_0:
		  op[idx].logic = vpi0;
		  op[idx].s0 = vpiStrongDrive;
		  op[idx].s1 = 0;
		  भङ्ग;
		पक्ष BIT4_1:
		  op[idx].logic = vpi1;
		  op[idx].s0 = 0;
		  op[idx].s1 = vpiStrongDrive;
		  भङ्ग;
		पक्ष BIT4_X:
		  op[idx].logic = vpiX;
		  op[idx].s0 = vpiStrongDrive;
		  op[idx].s1 = vpiStrongDrive;
		  भङ्ग;
		पक्ष BIT4_Z:
		  op[idx].logic = vpiZ;
		  op[idx].s0 = vpiHiZ;
		  op[idx].s1 = vpiHiZ;
		  भङ्ग;
	    }
      }

      vp->format = vpiStrengthVal;
      vp->value.strength = op;
}

vpiHandle __vpiVThrVec4Stack::vpi_put_मानम्(p_vpi_मानम् vp, पूर्णाङ्क /*flags*/)
{
      प्रतिज्ञा(vpip_current_vthread);

      विकल्प (vp->format) {

	  स्वभाव:
	    fprintf(stderr, "internal error: vpi_put_value(<format=%d>)"
		    " not implemented for __vpiVThrVec4Stack.\n", vp->format);
	    प्रतिज्ञा(0);
	    प्रत्यागम 0;
      }
}

#ifdef CHECK_WITH_VALGRIND
शून्य thread_vthr_delete(vpiHandle item)
{
      handle_map[item] = सच्च;
}

स्थिर शून्य thread_vthr_delete_real(vpiHandle item)
{
      __vpiVThrVec4Stack*obj = dynamic_cast<__vpiVThrVec4Stack*>(item);
      delete obj;
}

शून्य vpi_handle_delete()
{
      map<vpiHandle, तार्किक>::iterator iter;
      प्रति (iter = handle_map.begin(); iter != handle_map.end(); ++ iter ) {
	    जइ (iter->second) thread_vthr_delete_real(iter->first);
	    अण्णहा thread_word_delete_real(iter->first);
      }
}
#endif


vpiHandle vpip_make_vthr_str_राशिः(unsigned depth)
{
      __vpiVThrStrStack*obj = नूतन __vpiVThrStrStack(depth);
      प्रत्यागम obj;
}

vpiHandle vpip_make_vthr_vec4_राशिः(unsigned depth, तार्किक signed_flag, unsigned wid)
{
      __vpiVThrVec4Stack*obj = नूतन __vpiVThrVec4Stack(depth, signed_flag, wid);
      प्रत्यागम obj;
}

#ifdef CHECK_WITH_VALGRIND
स्थिर map<vpiHandle, तार्किक> राशिः_map;

शून्य thread_string_delete(vpiHandle item)
{
      राशिः_map[item] = मिच्छ;
}

स्थिर शून्य thread_string_delete_real(vpiHandle item)
{
      __vpiVThrStrStack*obj = dynamic_cast<__vpiVThrStrStack*>(item);
      delete obj;
}

शून्य vpi_राशिः_delete()
{
      map<vpiHandle, तार्किक>::iterator iter;
      प्रति (iter = राशिः_map.begin(); iter != राशिः_map.end(); ++ iter ) {
	    thread_string_delete_real(iter->first);
      }
}
#endif
