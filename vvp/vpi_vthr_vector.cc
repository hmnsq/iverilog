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

using નામાકાશ std;

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
બાહ્ય અચલ અક્ષર hex_digits[256];
બાહ્ય અચલ અક્ષર oct_digits[64];


વર્ગ __vpiVThrWord : જાહેર __vpiHandle {
    જાહેર:
      explicit __vpiVThrWord(unsigned base);
      પૂર્ણાંક get_type_code(શૂન્ય) અચલ override;
      પૂર્ણાંક vpi_get(પૂર્ણાંક code) override;
      શૂન્ય vpi_get_કિંમત(p_vpi_કિંમત val) override;

      પૂર્ણાંક get_subtype() અચલ { પાછા subtype; };
      unsigned get_index() અચલ { પાછા index; };

    ખાનગી:
      અચલ અક્ષર* name;
      પૂર્ણાંક subtype;
      unsigned index;
};

સ્થિર પૂર્ણાંક vthr_word_get(પૂર્ણાંક code, vpiHandle ref)
{
      અચલ __vpiVThrWord*rfp = dynamic_cast<__vpiVThrWord*>(ref);

      પસંદગી (code) {

	  સ્થિતિ vpiConstType:
	    પાછા rfp->get_subtype();

#જો defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  સ્થિતિ _vpiFromThr:
	    પાછા _vpiWord;
#endif

	  મૂળભૂત:
	    પાછા 0;
      }
}

સ્થિર શૂન્ય vthr_real_get_કિંમત(vpiHandle ref, s_vpi_કિંમત*vp)
{
      અચલ __vpiVThrWord*obj = dynamic_cast<__vpiVThrWord*>(ref);
      સ્થિર અચલ size_t RBUF_USE_SIZE = 66;
      અક્ષર *rbuf = static_cast<અક્ષર *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));

      દ્વિગુણ val = 0.0;

	/* Get the actual value from the index. It is possible, by the
	   way, that the vpi_get_value is called from compiletf. If
	   that's the case, there will be no current thread, and this
	   will not have access to the proper value. Punt and return a
	   0.0 value instead. */
      જો (vpip_current_vthread)
	    val = vthread_get_real_ઢગલો(vpip_current_vthread,
	                                 obj->get_index());

      પસંદગી (vp->format) {

	  સ્થિતિ vpiObjTypeVal:
	    vp->format = vpiRealVal;
	    // fallthrough
	  સ્થિતિ vpiRealVal:
	    vp->value.real = val;
	    તોડો;

	  સ્થિતિ vpiIntVal:
	      /* NaN or +/- infinity are translated as 0. */
	    જો (val != val || (val && (val == 0.5*val))) {
		  val = 0.0;
	    } નહીંતર {
		  val = std::round(val);
	    }
	    vp->value.integer = (PLI_INT32)val;
	    તોડો;

	  સ્થિતિ vpiDecStrVal:
	    જો (std::isnan(val))
		  snprintf(rbuf, RBUF_USE_SIZE, "%s", "nan");
	    નહીંતર
		  snprintf(rbuf, RBUF_USE_SIZE, "%0.0f", std::round(val));
	    vp->value.str = rbuf;
	    તોડો;

	  સ્થિતિ vpiOctStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" PRIo64, vlg_round_to_u64(val));
	    vp->value.str = rbuf;
	    તોડો;

	  સ્થિતિ vpiHexStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" PRIx64, vlg_round_to_u64(val));
	    vp->value.str = rbuf;
	    તોડો;

	  સ્થિતિ vpiBinStrVal: {
		uint64_t vali = vlg_round_to_u64(val);
		unsigned len = 0;

		જ્યારે (vali > 0) {
		      len += 1;
		      vali /= 2;
		}

		vali = vlg_round_to_u64(val);
		માટે (unsigned idx = 0 ;  idx < len ;  idx += 1) {
		      rbuf[len-idx-1] = (vali & 1)? '1' : '0';
		      vali /= 2;
		}

		rbuf[len] = 0;
		જો (len == 0) {
		      rbuf[0] = '0';
		      rbuf[1] = 0;
		}
		vp->value.str = rbuf;
		તોડો;
	  }

	  મૂળભૂત:
	    fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiConstant (Real)\n", (પૂર્ણાંક)vp->format);

	    vp->format = vpiSuppressVal;
	    તોડો;
      }
}

અંતઃરેખ __vpiVThrWord::__vpiVThrWord(unsigned base)
{
      name = vpip_name_string("W<>");
      subtype = vpiRealConst;
      index = base;
}

પૂર્ણાંક __vpiVThrWord::get_type_code(શૂન્ય) અચલ
{ પાછા vpiConstant; }

પૂર્ણાંક __vpiVThrWord::vpi_get(પૂર્ણાંક code)
{ પાછા vthr_word_get(code, આ); }

શૂન્ય __vpiVThrWord::vpi_get_કિંમત(p_vpi_કિંમત val)
{ vthr_real_get_કિંમત(આ, val); }

vpiHandle vpip_make_vthr_word(unsigned base, અચલ અક્ષર*type)
{
      પ્રતિજ્ઞા(type[0] == 'r');
      પ્રતિજ્ઞા(base < 65536);
      __vpiVThrWord*obj = નવું __vpiVThrWord(base);

      પાછા obj;
}

#ifdef CHECK_WITH_VALGRIND
સ્થિર map<vpiHandle, તાર્કિક> handle_map;

શૂન્ય thread_word_delete(vpiHandle item)
{
      handle_map[item] = ખોટું;
}

સ્થિર શૂન્ય thread_word_delete_real(vpiHandle item)
{
      વર્ગ __vpiVThrWord*obj = dynamic_cast<__vpiVThrWord*>(item);
      delete obj;
}
#endif

વર્ગ __vpiVThrStrStack : જાહેર __vpiHandle {
    જાહેર:
      explicit __vpiVThrStrStack(unsigned depth);
      પૂર્ણાંક get_type_code(શૂન્ય) અચલ override;
      પૂર્ણાંક vpi_get(પૂર્ણાંક code) override;
      શૂન્ય vpi_get_કિંમત(p_vpi_કિંમત val) override;
    ખાનગી:
      unsigned depth_;
};

__vpiVThrStrStack::__vpiVThrStrStack(unsigned d)
: depth_(d)
{
}

પૂર્ણાંક __vpiVThrStrStack::get_type_code(શૂન્ય) અચલ
{ પાછા vpiConstant; }

પૂર્ણાંક __vpiVThrStrStack::vpi_get(પૂર્ણાંક code)
{
      પસંદગી (code) {
	  સ્થિતિ vpiConstType:
	    પાછા vpiStringConst;
#જો defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  સ્થિતિ _vpiFromThr:
	    પાછા _vpiString;
#endif
	  સ્થિતિ vpiSize:
	    પાછા vthread_get_str_ઢગલો(vpip_current_vthread, depth_).size();
	  મૂળભૂત:
	    પાછા 0;
      }
}

શૂન્ય __vpiVThrStrStack::vpi_get_કિંમત(p_vpi_કિંમત vp)
{
      string val;
      અક્ષર*rbuf = 0;

      જો (vpip_current_vthread)
	    val = vthread_get_str_ઢગલો(vpip_current_vthread, depth_);

      પસંદગી (vp->format) {

	  સ્થિતિ vpiObjTypeVal:
	    vp->format = vpiStringVal;
	    // fallthrough
	  સ્થિતિ vpiStringVal:
	    rbuf = static_cast<અક્ષર *>(need_result_buf(val.size()+1, RBUF_VAL));
	    strcpy(rbuf, val.c_str());
	    vp->value.str = rbuf;
	    તોડો;

	  મૂળભૂત:
	    fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiConstant (String)\n", (પૂર્ણાંક)vp->format);

	    vp->format = vpiSuppressVal;
	    તોડો;
      }
}

વર્ગ __vpiVThrVec4Stack : જાહેર __vpiHandle {
    જાહેર:
      __vpiVThrVec4Stack(unsigned depth, તાર્કિક signed_flag, unsigned wid);
      પૂર્ણાંક get_type_code(શૂન્ય) અચલ override;
      પૂર્ણાંક vpi_get(પૂર્ણાંક code) override;
      અક્ષર*vpi_get_str(પૂર્ણાંક code) override;
      શૂન્ય vpi_get_કિંમત(p_vpi_કિંમત val) override;
      vpiHandle vpi_put_કિંમત(p_vpi_કિંમત val, પૂર્ણાંક flags) override;
    ખાનગી:
      સ્થિર શૂન્ય vpi_get_કિંમત_string(p_vpi_કિંમત vp, અચલ vvp_vector4_t&val);
      સ્થિર શૂન્ય vpi_get_કિંમત_binstr(p_vpi_કિંમત vp, અચલ vvp_vector4_t&val);
      શૂન્ય vpi_get_કિંમત_decstr(p_vpi_કિંમત vp, અચલ vvp_vector4_t&val);
      શૂન્ય vpi_get_કિંમત_int   (p_vpi_કિંમત vp, અચલ vvp_vector4_t&val);
      સ્થિર શૂન્ય vpi_get_કિંમત_real  (p_vpi_કિંમત vp, અચલ vvp_vector4_t&val);
      સ્થિર શૂન્ય vpi_get_કિંમત_strength(p_vpi_કિંમત vp, અચલ vvp_vector4_t&val);
      સ્થિર શૂન્ય vpi_get_કિંમત_octstr(p_vpi_કિંમત vp, અચલ vvp_vector4_t&val);
      સ્થિર શૂન્ય vpi_get_કિંમત_hexstr(p_vpi_કિંમત vp, અચલ vvp_vector4_t&val);
      સ્થિર શૂન્ય vpi_get_કિંમત_vector(p_vpi_કિંમત vp, અચલ vvp_vector4_t&val);
    ખાનગી:
      unsigned depth_;
      તાર્કિક signed_flag_;
      unsigned expect_width_;
      અચલ અક્ષર*name;
};

__vpiVThrVec4Stack::__vpiVThrVec4Stack(unsigned d, તાર્કિક sf, unsigned wid)
: depth_(d), signed_flag_(sf), expect_width_(wid)
{
      name = vpip_name_string("S<,vec4,>");
}

પૂર્ણાંક __vpiVThrVec4Stack::get_type_code(શૂન્ય) અચલ
{ પાછા vpiConstant; }


પૂર્ણાંક __vpiVThrVec4Stack::vpi_get(પૂર્ણાંક code)
{
      પસંદગી (code) {
	  સ્થિતિ vpiSize:
	    પાછા expect_width_;

	  સ્થિતિ vpiSigned:
	    પાછા signed_flag_? 1 : 0;

	  સ્થિતિ vpiConstType:
	    પાછા vpi_દ્વિઅંકી_Const;

#જો defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  સ્થિતિ _vpiFromThr:
	    પાછા _vpiVThr;
#endif

	  મૂળભૂત:
	    પાછા 0;
      }
}

અક્ષર*__vpiVThrVec4Stack::vpi_get_str(પૂર્ણાંક code)
{
      પસંદગી (code) {
	  સ્થિતિ vpiFullName:
	    પાછા simple_set_rbuf_str(name);

	  મૂળભૂત:
	    પાછા 0;
      }
}

શૂન્ય __vpiVThrVec4Stack::vpi_get_કિંમત(p_vpi_કિંમત vp)
{
      vvp_vector4_t val;

      જો (vpip_current_vthread)
	    val = vthread_get_vec4_ઢગલો(vpip_current_vthread, depth_);

      પસંદગી (vp->format) {

	  સ્થિતિ vpiBinStrVal:
	    vpi_get_કિંમત_binstr(vp, val);
	    તોડો;
	  સ્થિતિ vpiDecStrVal:
	    vpi_get_કિંમત_decstr(vp, val);
	    તોડો;
	  સ્થિતિ vpiOctStrVal:
	    vpi_get_કિંમત_octstr(vp, val);
	    તોડો;
	  સ્થિતિ vpiHexStrVal:
	    vpi_get_કિંમત_hexstr(vp, val);
	    તોડો;
	  સ્થિતિ vpiIntVal:
	    vpi_get_કિંમત_int(vp, val);
	    તોડો;
	  સ્થિતિ vpiRealVal:
	    vpi_get_કિંમત_real(vp, val);
	    તોડો;
	  સ્થિતિ vpiStringVal:
	    vpi_get_કિંમત_string(vp, val);
	    તોડો;
	  સ્થિતિ vpiStrengthVal:
	    vpi_get_કિંમત_strength(vp, val);
	    તોડો;
	  સ્થિતિ vpiObjTypeVal:
	    vp->format = vpiVectorVal;
	    // fallthrough
	  સ્થિતિ vpiVectorVal:
	    vpi_get_કિંમત_vector(vp, val);
	    તોડો;

	  મૂળભૂત:
	    fprintf(stderr, "internal error: vpi_get_value(<format=%d>)"
		    " not implemented for __vpiVThrVec4Stack.\n", vp->format);
	    પ્રતિજ્ઞા(0);
      }

}

શૂન્ય __vpiVThrVec4Stack::vpi_get_કિંમત_binstr(p_vpi_કિંમત vp, અચલ vvp_vector4_t&val)
{
      unsigned wid = val.size();
      અક્ષર*rbuf = static_cast<અક્ષર*>(need_result_buf(wid+1, RBUF_VAL));
      માટે (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    rbuf[wid-idx-1] = vvp_bit4_to_ascii(val.value(idx));
      }
      rbuf[wid] = 0;
      vp->value.str = rbuf;
}

શૂન્ય __vpiVThrVec4Stack::vpi_get_કિંમત_decstr(p_vpi_કિંમત vp, અચલ vvp_vector4_t&val)
{
      unsigned wid = val.size();
      પૂર્ણાંક nbuf = (wid+2)/3 + 1;
      અક્ષર *rbuf = static_cast<અક્ષર*>(need_result_buf(nbuf, RBUF_VAL));

      vpip_vec4_to_dec_str(val, rbuf, nbuf, signed_flag_);
      vp->value.str = rbuf;
}

શૂન્ય __vpiVThrVec4Stack::vpi_get_કિંમત_octstr(p_vpi_કિંમત vp, અચલ vvp_vector4_t&val)
{
      unsigned wid = val.size();
      unsigned owid = (wid + 2) / 3;
      અક્ષર*rbuf = static_cast<અક્ષર*>(need_result_buf(owid+1, RBUF_VAL));
      rbuf[owid] = 0;

      unsigned oval = 0;
      માટે (unsigned idx = 0; idx < wid ; idx += 1) {
	    unsigned tmp = 0;
	    પસંદગી (val.value(idx)) {
		સ્થિતિ BIT4_0:
		  tmp = 0;
		  તોડો;
		સ્થિતિ BIT4_1:
		  tmp = 1;
		  તોડો;
		સ્થિતિ BIT4_X:
		  tmp = 2;
		  તોડો;
		સ્થિતિ BIT4_Z:
		  tmp = 3;
		  તોડો;
	    }

	    oval = oval | (tmp << 2*(idx%3));

	    જો (idx%3 == 2) {
		  owid -= 1;
		  rbuf[owid] = oct_digits[oval];
		  oval = 0;
	    }
      }

      જો (owid > 0) {
	    owid -= 1;
	    rbuf[owid] = oct_digits[oval];
      }
      vp->value.str = rbuf;
}

શૂન્ય __vpiVThrVec4Stack::vpi_get_કિંમત_hexstr(p_vpi_કિંમત vp, અચલ vvp_vector4_t&val)
{
      unsigned wid = val.size();
      unsigned hwid = (wid + 3) /4;
      અક્ષર*rbuf = static_cast<અક્ષર*>(need_result_buf(hwid+1, RBUF_VAL));
      rbuf[hwid] = 0;

      unsigned hval = 0;
      માટે (unsigned idx = 0; idx < wid ; idx += 1) {
	    unsigned tmp = 0;
	    પસંદગી (val.value(idx)) {
		સ્થિતિ BIT4_0:
		  tmp = 0;
		  તોડો;
		સ્થિતિ BIT4_1:
		  tmp = 1;
		  તોડો;
		સ્થિતિ BIT4_X:
		  tmp = 2;
		  તોડો;
		સ્થિતિ BIT4_Z:
		  tmp = 3;
		  તોડો;
	    }
	    hval = hval | (tmp << 2*(idx%4));

	    જો (idx%4 == 3) {
		  hwid -= 1;
		  rbuf[hwid] = hex_digits[hval];
		  hval = 0;
	    }
      }

      જો (hwid > 0) {
	    hwid -= 1;
	    rbuf[hwid] = hex_digits[hval];
      }
      vp->value.str = rbuf;
}

શૂન્ય __vpiVThrVec4Stack::vpi_get_કિંમત_int(p_vpi_કિંમત vp, અચલ vvp_vector4_t&val)
{
      int32_t vali = 0;
      vector4_to_કિંમત(val, vali, signed_flag_, ખોટું);
      vp->value.integer = vali;
}

શૂન્ય __vpiVThrVec4Stack::vpi_get_કિંમત_real(p_vpi_કિંમત vp, અચલ vvp_vector4_t&val)
{
      unsigned wid = val.size();
      vp->value.real = 0.0;

      માટે (unsigned idx = wid ; idx > 0 ; idx -= 1) {
	    vp->value.real *= 2.0;
	    જો (val.value(idx-1) == BIT4_1)
		  vp->value.real += 1.0;
      }
}

શૂન્ય __vpiVThrVec4Stack::vpi_get_કિંમત_string(p_vpi_કિંમત vp, અચલ vvp_vector4_t&val)
{
      અક્ષર*rbuf = static_cast<અક્ષર*>(need_result_buf((val.size() / 8) + 1, RBUF_VAL));
      અક્ષર*cp = rbuf;

      અક્ષર tmp = 0;
      માટે (પૂર્ણાંક bitnr = val.size()-1 ; bitnr >= 0 ; bitnr -= 1) {
	    tmp <<= 1;
	    પસંદગી (val.value(bitnr)) {
		સ્થિતિ BIT4_1:
		  tmp |= 1;
		  તોડો;
		સ્થિતિ BIT4_0:
		મૂળભૂત:
		  તોડો;
	    }

	    જો ((bitnr&7)==0) {
		    // Don't include leading nuls
		  જો (tmp == 0 && cp == rbuf)
			ચાલુ;

		  *cp++ = tmp? tmp : ' ';
		  tmp = 0;
	    }
      }
      *cp++ = 0;
      vp->format = vpiStringVal;
      vp->value.str = rbuf;
}

શૂન્ય __vpiVThrVec4Stack::vpi_get_કિંમત_vector(p_vpi_કિંમત vp, અચલ vvp_vector4_t&val)
{
      unsigned wid = val.size();

      unsigned hwid = (wid+31)/32;
      vp->value.vector = static_cast<s_vpi_vecval*>
                         (need_result_buf(hwid*કદ(s_vpi_vecval), RBUF_VAL));
      પ્રતિજ્ઞા(vp->value.vector);

	/* Zero the top of the result buffer first since the fill loop below only writes
	   bits 0..wid-1, which can leave stale data in the unused high bits. */
      જો (hwid == 0) hwid = 1;
      vp->value.vector[hwid-1].aval = 0;
      vp->value.vector[hwid-1].bval = 0;

      માટે (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
	    પૂર્ણાંક word = idx/32;
	    PLI_INT32 mask = 1 << (idx%32);

	    પસંદગી (val.value(idx)) {
		સ્થિતિ BIT4_0:
		  vp->value.vector[word].aval &= ~mask;
		  vp->value.vector[word].bval &= ~mask;
		  તોડો;
		સ્થિતિ BIT4_1:
		  vp->value.vector[word].aval |=  mask;
		  vp->value.vector[word].bval &= ~mask;
		  તોડો;
		સ્થિતિ BIT4_X:
		  vp->value.vector[word].aval |=  mask;
		  vp->value.vector[word].bval |=  mask;
		  તોડો;
		સ્થિતિ BIT4_Z:
		  vp->value.vector[word].aval &= ~mask;
		  vp->value.vector[word].bval |=  mask;
		  તોડો;
	    }
      }
}

શૂન્ય __vpiVThrVec4Stack::vpi_get_કિંમત_strength(p_vpi_કિંમત vp, અચલ vvp_vector4_t&val)
{
      s_vpi_strengthval*op = static_cast<s_vpi_strengthval*>
                             (need_result_buf(val.size() * કદ(s_vpi_strengthval),
                                              RBUF_VAL));

      માટે (unsigned idx = 0 ; idx < val.size() ; idx += 1) {
	    પસંદગી (val.value(idx)) {
		સ્થિતિ BIT4_0:
		  op[idx].logic = vpi0;
		  op[idx].s0 = vpiStrongDrive;
		  op[idx].s1 = 0;
		  તોડો;
		સ્થિતિ BIT4_1:
		  op[idx].logic = vpi1;
		  op[idx].s0 = 0;
		  op[idx].s1 = vpiStrongDrive;
		  તોડો;
		સ્થિતિ BIT4_X:
		  op[idx].logic = vpiX;
		  op[idx].s0 = vpiStrongDrive;
		  op[idx].s1 = vpiStrongDrive;
		  તોડો;
		સ્થિતિ BIT4_Z:
		  op[idx].logic = vpiZ;
		  op[idx].s0 = vpiHiZ;
		  op[idx].s1 = vpiHiZ;
		  તોડો;
	    }
      }

      vp->format = vpiStrengthVal;
      vp->value.strength = op;
}

vpiHandle __vpiVThrVec4Stack::vpi_put_કિંમત(p_vpi_કિંમત vp, પૂર્ણાંક /*flags*/)
{
      પ્રતિજ્ઞા(vpip_current_vthread);

      પસંદગી (vp->format) {

	  મૂળભૂત:
	    fprintf(stderr, "internal error: vpi_put_value(<format=%d>)"
		    " not implemented for __vpiVThrVec4Stack.\n", vp->format);
	    પ્રતિજ્ઞા(0);
	    પાછા 0;
      }
}

#ifdef CHECK_WITH_VALGRIND
શૂન્ય thread_vthr_delete(vpiHandle item)
{
      handle_map[item] = સાચું;
}

સ્થિર શૂન્ય thread_vthr_delete_real(vpiHandle item)
{
      __vpiVThrVec4Stack*obj = dynamic_cast<__vpiVThrVec4Stack*>(item);
      delete obj;
}

શૂન્ય vpi_handle_delete()
{
      map<vpiHandle, તાર્કિક>::iterator iter;
      માટે (iter = handle_map.begin(); iter != handle_map.end(); ++ iter ) {
	    જો (iter->second) thread_vthr_delete_real(iter->first);
	    નહીંતર thread_word_delete_real(iter->first);
      }
}
#endif


vpiHandle vpip_make_vthr_str_ઢગલો(unsigned depth)
{
      __vpiVThrStrStack*obj = નવું __vpiVThrStrStack(depth);
      પાછા obj;
}

vpiHandle vpip_make_vthr_vec4_ઢગલો(unsigned depth, તાર્કિક signed_flag, unsigned wid)
{
      __vpiVThrVec4Stack*obj = નવું __vpiVThrVec4Stack(depth, signed_flag, wid);
      પાછા obj;
}

#ifdef CHECK_WITH_VALGRIND
સ્થિર map<vpiHandle, તાર્કિક> ઢગલો_map;

શૂન્ય thread_string_delete(vpiHandle item)
{
      ઢગલો_map[item] = ખોટું;
}

સ્થિર શૂન્ય thread_string_delete_real(vpiHandle item)
{
      __vpiVThrStrStack*obj = dynamic_cast<__vpiVThrStrStack*>(item);
      delete obj;
}

શૂન્ય vpi_ઢગલો_delete()
{
      map<vpiHandle, તાર્કિક>::iterator iter;
      માટે (iter = ઢગલો_map.begin(); iter != ઢગલો_map.end(); ++ iter ) {
	    thread_string_delete_real(iter->first);
      }
}
#endif
