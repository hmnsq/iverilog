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
#अगर_घोषित CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
# include <map>
#पूर्ण_अगर
# include  <cstdio>
# include  <cstdlib>
# include  <cassert>
# include  "ivl_alloc.h"

using namespace std;

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
बाह्य स्थिर अक्षर hex_digits[256];
बाह्य स्थिर अक्षर oct_digits[64];


श्रेणी __vpiVThrWord : खुला __vpiHandle {
    खुला:
      explicit __vpiVThrWord(अचिन्हित base);
      पूर्णांक get_type_code(व्योम) स्थिर override;
      पूर्णांक vpi_get(पूर्णांक code) override;
      व्योम vpi_get_मूल्य(p_vpi_मूल्य val) override;

      पूर्णांक get_subtype() स्थिर { वापस subtype; };
      अचिन्हित get_सूचकांक() स्थिर { वापस सूचकांक; };

    निजी:
      स्थिर अक्षर* name;
      पूर्णांक subtype;
      अचिन्हित सूचकांक;
};

अटल पूर्णांक vthr_word_get(पूर्णांक code, vpiHandle ref)
{
      स्थिर __vpiVThrWord*rfp = dynamic_cast<__vpiVThrWord*>(ref);

      चयन (code) {

	  हाल vpiConstType:
	    वापस rfp->get_subtype();

#अगर defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  हाल _vpiFromThr:
	    वापस _vpiWord;
#पूर्ण_अगर

	  शेष:
	    वापस 0;
      }
}

अटल व्योम vthr_real_get_मूल्य(vpiHandle ref, s_vpi_मूल्य*vp)
{
      स्थिर __vpiVThrWord*obj = dynamic_cast<__vpiVThrWord*>(ref);
      अटल स्थिर माप_प्रकार RBUF_USE_आकार = 66;
      अक्षर *rbuf = static_cast<अक्षर *>(need_परिणाम_buf(RBUF_USE_आकार, RBUF_VAL));

      द्विगुन val = 0.0;

	/* Get the actual value from the index. It is possible, by the
	   way, that the vpi_get_value is called from compiletf. If
	   that's the case, there will be no current thread, and this
	   will not have access to the proper value. Punt and return a
	   0.0 value instead. */
      अगर (vpip_current_vthread)
	    val = vthread_get_real_ढेर(vpip_current_vthread,
	                                 obj->get_सूचकांक());

      चयन (vp->format) {

	  हाल vpiObjTypeVal:
	    vp->format = vpiRealVal;
	    // fallthrough
	  हाल vpiRealVal:
	    vp->value.real = val;
	    अवरोध;

	  हाल vpiIntVal:
	      /* NaN or +/- infinity are translated as 0. */
	    अगर (val != val || (val && (val == 0.5*val))) {
		  val = 0.0;
	    } अन्यथा {
		  val = std::round(val);
	    }
	    vp->value.integer = (PLI_INT32)val;
	    अवरोध;

	  हाल vpiDecStrVal:
	    अगर (std::isnan(val))
		  snprintf(rbuf, RBUF_USE_आकार, "%s", "nan");
	    अन्यथा
		  snprintf(rbuf, RBUF_USE_आकार, "%0.0f", std::round(val));
	    vp->value.str = rbuf;
	    अवरोध;

	  हाल vpiOctStrVal:
	    snprintf(rbuf, RBUF_USE_आकार, "%" PRIo64, vlg_round_to_u64(val));
	    vp->value.str = rbuf;
	    अवरोध;

	  हाल vpiHexStrVal:
	    snprintf(rbuf, RBUF_USE_आकार, "%" PRIx64, vlg_round_to_u64(val));
	    vp->value.str = rbuf;
	    अवरोध;

	  हाल vpiBinStrVal: {
		uint64_t vali = vlg_round_to_u64(val);
		अचिन्हित len = 0;

		जबतक (vali > 0) {
		      len += 1;
		      vali /= 2;
		}

		vali = vlg_round_to_u64(val);
		क्रम (अचिन्हित idx = 0 ;  idx < len ;  idx += 1) {
		      rbuf[len-idx-1] = (vali & 1)? '1' : '0';
		      vali /= 2;
		}

		rbuf[len] = 0;
		अगर (len == 0) {
		      rbuf[0] = '0';
		      rbuf[1] = 0;
		}
		vp->value.str = rbuf;
		अवरोध;
	  }

	  शेष:
	    ख_लिखो(मानक_त्रुटि, "vvp error: get %d not supported "
		      "by vpiConstant (Real)\n", (पूर्णांक)vp->format);

	    vp->format = vpiSuppressVal;
	    अवरोध;
      }
}

अंतरभूत __vpiVThrWord::__vpiVThrWord(अचिन्हित base)
{
      name = vpip_name_string("W<>");
      subtype = vpiRealConst;
      सूचकांक = base;
}

पूर्णांक __vpiVThrWord::get_type_code(व्योम) स्थिर
{ वापस vpiConstant; }

पूर्णांक __vpiVThrWord::vpi_get(पूर्णांक code)
{ वापस vthr_word_get(code, यह); }

व्योम __vpiVThrWord::vpi_get_मूल्य(p_vpi_मूल्य val)
{ vthr_real_get_मूल्य(यह, val); }

vpiHandle vpip_make_vthr_word(अचिन्हित base, स्थिर अक्षर*type)
{
      निश्चित(type[0] == 'r');
      निश्चित(base < 65536);
      __vpiVThrWord*obj = नया __vpiVThrWord(base);

      वापस obj;
}

#अगर_घोषित CHECK_WITH_VALGRIND
अटल map<vpiHandle, bool> handle_map;

व्योम thread_word_delete(vpiHandle item)
{
      handle_map[item] = false;
}

अटल व्योम thread_word_delete_real(vpiHandle item)
{
      श्रेणी __vpiVThrWord*obj = dynamic_cast<__vpiVThrWord*>(item);
      मिटाओ obj;
}
#पूर्ण_अगर

श्रेणी __vpiVThrStrStack : खुला __vpiHandle {
    खुला:
      explicit __vpiVThrStrStack(अचिन्हित depth);
      पूर्णांक get_type_code(व्योम) स्थिर override;
      पूर्णांक vpi_get(पूर्णांक code) override;
      व्योम vpi_get_मूल्य(p_vpi_मूल्य val) override;
    निजी:
      अचिन्हित depth_;
};

__vpiVThrStrStack::__vpiVThrStrStack(अचिन्हित d)
: depth_(d)
{
}

पूर्णांक __vpiVThrStrStack::get_type_code(व्योम) स्थिर
{ वापस vpiConstant; }

पूर्णांक __vpiVThrStrStack::vpi_get(पूर्णांक code)
{
      चयन (code) {
	  हाल vpiConstType:
	    वापस vpiStringConst;
#अगर defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  हाल _vpiFromThr:
	    वापस _vpiString;
#पूर्ण_अगर
	  हाल vpi_आकार:
	    वापस vthread_get_str_ढेर(vpip_current_vthread, depth_).size();
	  शेष:
	    वापस 0;
      }
}

व्योम __vpiVThrStrStack::vpi_get_मूल्य(p_vpi_मूल्य vp)
{
      string val;
      अक्षर*rbuf = 0;

      अगर (vpip_current_vthread)
	    val = vthread_get_str_ढेर(vpip_current_vthread, depth_);

      चयन (vp->format) {

	  हाल vpiObjTypeVal:
	    vp->format = vpiStringVal;
	    // fallthrough
	  हाल vpiStringVal:
	    rbuf = static_cast<अक्षर *>(need_परिणाम_buf(val.size()+1, RBUF_VAL));
	    म_नकल(rbuf, val.c_str());
	    vp->value.str = rbuf;
	    अवरोध;

	  शेष:
	    ख_लिखो(मानक_त्रुटि, "vvp error: get %d not supported "
		      "by vpiConstant (String)\n", (पूर्णांक)vp->format);

	    vp->format = vpiSuppressVal;
	    अवरोध;
      }
}

श्रेणी __vpiVThrVec4Stack : खुला __vpiHandle {
    खुला:
      __vpiVThrVec4Stack(अचिन्हित depth, bool signed_flag, अचिन्हित wid);
      पूर्णांक get_type_code(व्योम) स्थिर override;
      पूर्णांक vpi_get(पूर्णांक code) override;
      अक्षर*vpi_get_str(पूर्णांक code) override;
      व्योम vpi_get_मूल्य(p_vpi_मूल्य val) override;
      vpiHandle vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णांक flags) override;
    निजी:
      अटल व्योम vpi_get_मूल्य_string(p_vpi_मूल्य vp, स्थिर vvp_vector4_t&val);
      अटल व्योम vpi_get_मूल्य_binstr(p_vpi_मूल्य vp, स्थिर vvp_vector4_t&val);
      व्योम vpi_get_मूल्य_decstr(p_vpi_मूल्य vp, स्थिर vvp_vector4_t&val);
      व्योम vpi_get_मूल्य_int   (p_vpi_मूल्य vp, स्थिर vvp_vector4_t&val);
      अटल व्योम vpi_get_मूल्य_real  (p_vpi_मूल्य vp, स्थिर vvp_vector4_t&val);
      अटल व्योम vpi_get_मूल्य_strength(p_vpi_मूल्य vp, स्थिर vvp_vector4_t&val);
      अटल व्योम vpi_get_मूल्य_octstr(p_vpi_मूल्य vp, स्थिर vvp_vector4_t&val);
      अटल व्योम vpi_get_मूल्य_hexstr(p_vpi_मूल्य vp, स्थिर vvp_vector4_t&val);
      अटल व्योम vpi_get_मूल्य_vector(p_vpi_मूल्य vp, स्थिर vvp_vector4_t&val);
    निजी:
      अचिन्हित depth_;
      bool signed_flag_;
      अचिन्हित expect_width_;
      स्थिर अक्षर*name;
};

__vpiVThrVec4Stack::__vpiVThrVec4Stack(अचिन्हित d, bool sf, अचिन्हित wid)
: depth_(d), signed_flag_(sf), expect_width_(wid)
{
      name = vpip_name_string("S<,vec4,>");
}

पूर्णांक __vpiVThrVec4Stack::get_type_code(व्योम) स्थिर
{ वापस vpiConstant; }


पूर्णांक __vpiVThrVec4Stack::vpi_get(पूर्णांक code)
{
      चयन (code) {
	  हाल vpi_आकार:
	    वापस expect_width_;

	  हाल vpiSigned:
	    वापस signed_flag_? 1 : 0;

	  हाल vpiConstType:
	    वापस vpi_द्विआधारी_Const;

#अगर defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  हाल _vpiFromThr:
	    वापस _vpiVThr;
#पूर्ण_अगर

	  शेष:
	    वापस 0;
      }
}

अक्षर*__vpiVThrVec4Stack::vpi_get_str(पूर्णांक code)
{
      चयन (code) {
	  हाल vpiFullName:
	    वापस simple_set_rbuf_str(name);

	  शेष:
	    वापस 0;
      }
}

व्योम __vpiVThrVec4Stack::vpi_get_मूल्य(p_vpi_मूल्य vp)
{
      vvp_vector4_t val;

      अगर (vpip_current_vthread)
	    val = vthread_get_vec4_ढेर(vpip_current_vthread, depth_);

      चयन (vp->format) {

	  हाल vpiBinStrVal:
	    vpi_get_मूल्य_binstr(vp, val);
	    अवरोध;
	  हाल vpiDecStrVal:
	    vpi_get_मूल्य_decstr(vp, val);
	    अवरोध;
	  हाल vpiOctStrVal:
	    vpi_get_मूल्य_octstr(vp, val);
	    अवरोध;
	  हाल vpiHexStrVal:
	    vpi_get_मूल्य_hexstr(vp, val);
	    अवरोध;
	  हाल vpiIntVal:
	    vpi_get_मूल्य_int(vp, val);
	    अवरोध;
	  हाल vpiRealVal:
	    vpi_get_मूल्य_real(vp, val);
	    अवरोध;
	  हाल vpiStringVal:
	    vpi_get_मूल्य_string(vp, val);
	    अवरोध;
	  हाल vpiStrengthVal:
	    vpi_get_मूल्य_strength(vp, val);
	    अवरोध;
	  हाल vpiObjTypeVal:
	    vp->format = vpiVectorVal;
	    // fallthrough
	  हाल vpiVectorVal:
	    vpi_get_मूल्य_vector(vp, val);
	    अवरोध;

	  शेष:
	    ख_लिखो(मानक_त्रुटि, "internal error: vpi_get_value(<format=%d>)"
		    " not implemented for __vpiVThrVec4Stack.\n", vp->format);
	    निश्चित(0);
      }

}

व्योम __vpiVThrVec4Stack::vpi_get_मूल्य_binstr(p_vpi_मूल्य vp, स्थिर vvp_vector4_t&val)
{
      अचिन्हित wid = val.size();
      अक्षर*rbuf = static_cast<अक्षर*>(need_परिणाम_buf(wid+1, RBUF_VAL));
      क्रम (अचिन्हित idx = 0 ; idx < wid ; idx += 1) {
	    rbuf[wid-idx-1] = vvp_bit4_to_ascii(val.value(idx));
      }
      rbuf[wid] = 0;
      vp->value.str = rbuf;
}

व्योम __vpiVThrVec4Stack::vpi_get_मूल्य_decstr(p_vpi_मूल्य vp, स्थिर vvp_vector4_t&val)
{
      अचिन्हित wid = val.size();
      पूर्णांक nbuf = (wid+2)/3 + 1;
      अक्षर *rbuf = static_cast<अक्षर*>(need_परिणाम_buf(nbuf, RBUF_VAL));

      vpip_vec4_to_dec_str(val, rbuf, nbuf, signed_flag_);
      vp->value.str = rbuf;
}

व्योम __vpiVThrVec4Stack::vpi_get_मूल्य_octstr(p_vpi_मूल्य vp, स्थिर vvp_vector4_t&val)
{
      अचिन्हित wid = val.size();
      अचिन्हित owid = (wid + 2) / 3;
      अक्षर*rbuf = static_cast<अक्षर*>(need_परिणाम_buf(owid+1, RBUF_VAL));
      rbuf[owid] = 0;

      अचिन्हित oval = 0;
      क्रम (अचिन्हित idx = 0; idx < wid ; idx += 1) {
	    अचिन्हित tmp = 0;
	    चयन (val.value(idx)) {
		हाल BIT4_0:
		  tmp = 0;
		  अवरोध;
		हाल BIT4_1:
		  tmp = 1;
		  अवरोध;
		हाल BIT4_X:
		  tmp = 2;
		  अवरोध;
		हाल BIT4_Z:
		  tmp = 3;
		  अवरोध;
	    }

	    oval = oval | (tmp << 2*(idx%3));

	    अगर (idx%3 == 2) {
		  owid -= 1;
		  rbuf[owid] = oct_digits[oval];
		  oval = 0;
	    }
      }

      अगर (owid > 0) {
	    owid -= 1;
	    rbuf[owid] = oct_digits[oval];
      }
      vp->value.str = rbuf;
}

व्योम __vpiVThrVec4Stack::vpi_get_मूल्य_hexstr(p_vpi_मूल्य vp, स्थिर vvp_vector4_t&val)
{
      अचिन्हित wid = val.size();
      अचिन्हित hwid = (wid + 3) /4;
      अक्षर*rbuf = static_cast<अक्षर*>(need_परिणाम_buf(hwid+1, RBUF_VAL));
      rbuf[hwid] = 0;

      अचिन्हित hval = 0;
      क्रम (अचिन्हित idx = 0; idx < wid ; idx += 1) {
	    अचिन्हित tmp = 0;
	    चयन (val.value(idx)) {
		हाल BIT4_0:
		  tmp = 0;
		  अवरोध;
		हाल BIT4_1:
		  tmp = 1;
		  अवरोध;
		हाल BIT4_X:
		  tmp = 2;
		  अवरोध;
		हाल BIT4_Z:
		  tmp = 3;
		  अवरोध;
	    }
	    hval = hval | (tmp << 2*(idx%4));

	    अगर (idx%4 == 3) {
		  hwid -= 1;
		  rbuf[hwid] = hex_digits[hval];
		  hval = 0;
	    }
      }

      अगर (hwid > 0) {
	    hwid -= 1;
	    rbuf[hwid] = hex_digits[hval];
      }
      vp->value.str = rbuf;
}

व्योम __vpiVThrVec4Stack::vpi_get_मूल्य_int(p_vpi_मूल्य vp, स्थिर vvp_vector4_t&val)
{
      int32_t vali = 0;
      vector4_to_मूल्य(val, vali, signed_flag_, false);
      vp->value.integer = vali;
}

व्योम __vpiVThrVec4Stack::vpi_get_मूल्य_real(p_vpi_मूल्य vp, स्थिर vvp_vector4_t&val)
{
      अचिन्हित wid = val.size();
      vp->value.real = 0.0;

      क्रम (अचिन्हित idx = wid ; idx > 0 ; idx -= 1) {
	    vp->value.real *= 2.0;
	    अगर (val.value(idx-1) == BIT4_1)
		  vp->value.real += 1.0;
      }
}

व्योम __vpiVThrVec4Stack::vpi_get_मूल्य_string(p_vpi_मूल्य vp, स्थिर vvp_vector4_t&val)
{
      अक्षर*rbuf = static_cast<अक्षर*>(need_परिणाम_buf((val.size() / 8) + 1, RBUF_VAL));
      अक्षर*cp = rbuf;

      अक्षर tmp = 0;
      क्रम (पूर्णांक bitnr = val.size()-1 ; bitnr >= 0 ; bitnr -= 1) {
	    tmp <<= 1;
	    चयन (val.value(bitnr)) {
		हाल BIT4_1:
		  tmp |= 1;
		  अवरोध;
		हाल BIT4_0:
		शेष:
		  अवरोध;
	    }

	    अगर ((bitnr&7)==0) {
		    // Don't include leading nuls
		  अगर (tmp == 0 && cp == rbuf)
			जारी;

		  *cp++ = tmp? tmp : ' ';
		  tmp = 0;
	    }
      }
      *cp++ = 0;
      vp->format = vpiStringVal;
      vp->value.str = rbuf;
}

व्योम __vpiVThrVec4Stack::vpi_get_मूल्य_vector(p_vpi_मूल्य vp, स्थिर vvp_vector4_t&val)
{
      अचिन्हित wid = val.size();

      अचिन्हित hwid = (wid+31)/32;
      vp->value.vector = static_cast<s_vpi_vecval*>
                         (need_परिणाम_buf(hwid*माप(s_vpi_vecval), RBUF_VAL));
      निश्चित(vp->value.vector);

	/* Zero the top of the result buffer first since the fill loop below only writes
	   bits 0..wid-1, which can leave stale data in the unused high bits. */
      अगर (hwid == 0) hwid = 1;
      vp->value.vector[hwid-1].aval = 0;
      vp->value.vector[hwid-1].bval = 0;

      क्रम (अचिन्हित idx = 0 ;  idx < wid ;  idx += 1) {
	    पूर्णांक word = idx/32;
	    PLI_INT32 mask = 1 << (idx%32);

	    चयन (val.value(idx)) {
		हाल BIT4_0:
		  vp->value.vector[word].aval &= ~mask;
		  vp->value.vector[word].bval &= ~mask;
		  अवरोध;
		हाल BIT4_1:
		  vp->value.vector[word].aval |=  mask;
		  vp->value.vector[word].bval &= ~mask;
		  अवरोध;
		हाल BIT4_X:
		  vp->value.vector[word].aval |=  mask;
		  vp->value.vector[word].bval |=  mask;
		  अवरोध;
		हाल BIT4_Z:
		  vp->value.vector[word].aval &= ~mask;
		  vp->value.vector[word].bval |=  mask;
		  अवरोध;
	    }
      }
}

व्योम __vpiVThrVec4Stack::vpi_get_मूल्य_strength(p_vpi_मूल्य vp, स्थिर vvp_vector4_t&val)
{
      s_vpi_strengthval*op = static_cast<s_vpi_strengthval*>
                             (need_परिणाम_buf(val.size() * माप(s_vpi_strengthval),
                                              RBUF_VAL));

      क्रम (अचिन्हित idx = 0 ; idx < val.size() ; idx += 1) {
	    चयन (val.value(idx)) {
		हाल BIT4_0:
		  op[idx].logic = vpi0;
		  op[idx].s0 = vpiStrongDrive;
		  op[idx].s1 = 0;
		  अवरोध;
		हाल BIT4_1:
		  op[idx].logic = vpi1;
		  op[idx].s0 = 0;
		  op[idx].s1 = vpiStrongDrive;
		  अवरोध;
		हाल BIT4_X:
		  op[idx].logic = vpiX;
		  op[idx].s0 = vpiStrongDrive;
		  op[idx].s1 = vpiStrongDrive;
		  अवरोध;
		हाल BIT4_Z:
		  op[idx].logic = vpiZ;
		  op[idx].s0 = vpiHiZ;
		  op[idx].s1 = vpiHiZ;
		  अवरोध;
	    }
      }

      vp->format = vpiStrengthVal;
      vp->value.strength = op;
}

vpiHandle __vpiVThrVec4Stack::vpi_put_मूल्य(p_vpi_मूल्य vp, पूर्णांक /*flags*/)
{
      निश्चित(vpip_current_vthread);

      चयन (vp->format) {

	  शेष:
	    ख_लिखो(मानक_त्रुटि, "internal error: vpi_put_value(<format=%d>)"
		    " not implemented for __vpiVThrVec4Stack.\n", vp->format);
	    निश्चित(0);
	    वापस 0;
      }
}

#अगर_घोषित CHECK_WITH_VALGRIND
व्योम thread_vthr_delete(vpiHandle item)
{
      handle_map[item] = true;
}

अटल व्योम thread_vthr_delete_real(vpiHandle item)
{
      __vpiVThrVec4Stack*obj = dynamic_cast<__vpiVThrVec4Stack*>(item);
      मिटाओ obj;
}

व्योम vpi_handle_delete()
{
      map<vpiHandle, bool>::iterator iter;
      क्रम (iter = handle_map.begin(); iter != handle_map.end(); ++ iter ) {
	    अगर (iter->second) thread_vthr_delete_real(iter->first);
	    अन्यथा thread_word_delete_real(iter->first);
      }
}
#पूर्ण_अगर


vpiHandle vpip_make_vthr_str_ढेर(अचिन्हित depth)
{
      __vpiVThrStrStack*obj = नया __vpiVThrStrStack(depth);
      वापस obj;
}

vpiHandle vpip_make_vthr_vec4_ढेर(अचिन्हित depth, bool signed_flag, अचिन्हित wid)
{
      __vpiVThrVec4Stack*obj = नया __vpiVThrVec4Stack(depth, signed_flag, wid);
      वापस obj;
}

#अगर_घोषित CHECK_WITH_VALGRIND
अटल map<vpiHandle, bool> ढेर_map;

व्योम thread_string_delete(vpiHandle item)
{
      ढेर_map[item] = false;
}

अटल व्योम thread_string_delete_real(vpiHandle item)
{
      __vpiVThrStrStack*obj = dynamic_cast<__vpiVThrStrStack*>(item);
      मिटाओ obj;
}

व्योम vpi_ढेर_delete()
{
      map<vpiHandle, bool>::iterator iter;
      क्रम (iter = ढेर_map.begin(); iter != ढेर_map.end(); ++ iter ) {
	    thread_string_delete_real(iter->first);
      }
}
#पूर्ण_अगर
