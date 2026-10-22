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
#నిర్వచితమైతే CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
# include <map>
#అయితే_ముగింపు
# include  <cstdio>
# include  <cstdlib>
# include  <cassert>
# include  "ivl_alloc.h"

ఉపయోగించి నామాకాశ std;

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
బాహ్య స్థిరరాశి అక్షరం hex_digits[256];
బాహ్య స్థిరరాశి అక్షరం oct_digits[64];


తరగతి __vpiVThrWord : బహిరంగ __vpiHandle {
    బహిరంగ:
      స్పష్టమైన __vpiVThrWord(గుర్తులేని పునాది);
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override;
      పూర్ణాంకం vpi_get(పూర్ణాంకం code) override;
      శూన్య vpi_get_విలువ(p_vpi_విలువ val) override;

      పూర్ణాంకం get_subtype() స్థిరరాశి { తిరిగి subtype; };
      గుర్తులేని get_సూచీ() స్థిరరాశి { తిరిగి సూచీ; };

    అంతరంగ:
      స్థిరరాశి అక్షరం* name;
      పూర్ణాంకం subtype;
      గుర్తులేని సూచీ;
};

స్థిర పూర్ణాంకం vthr_మాట_get(పూర్ణాంకం code, vpiHandle ref)
{
      స్థిరరాశి __vpiVThrWord*rfp = dynamic_cast<__vpiVThrWord*>(ref);

      ఎంపిక (code) {

	  స్థితి vpiConstType:
	    తిరిగి rfp->get_subtype();

#ఒకవేళ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  స్థితి _vpiFromThr:
	    తిరిగి vpi_మాట;
#అయితే_ముగింపు

	  అప్రమేయ:
	    తిరిగి 0;
      }
}

స్థిర శూన్య vthr_real_get_విలువ(vpiHandle ref, s_vpi_విలువ*vp)
{
      స్థిరరాశి __vpiVThrWord*obj = dynamic_cast<__vpiVThrWord*>(ref);
      స్థిర స్థిరరాశి size_t RBUF_USE_కొలత = 66;
      అక్షరం *rbuf = static_cast<అక్షరం *>(need_ఫలితం_buf(RBUF_USE_కొలత, RBUF_VAL));

      ద్విగుణ val = 0.0;

	/* Get the actual value from the index. It is possible, by the
	   way, that the vpi_get_value is called from compiletf. If
	   that's the case, there will be no current thread, and this
	   will not have access to the proper value. Punt and return a
	   0.0 value instead. */
      ఒకవేళ (vpip_current_vthread)
	    val = vthread_get_real_దొంతర(vpip_current_vthread,
	                                 obj->get_సూచీ());

      ఎంపిక (vp->format) {

	  స్థితి vpiObjTypeVal:
	    vp->format = vpiRealVal;
	    // fallthrough
	  స్థితి vpiRealVal:
	    vp->value.real = val;
	    ఆపు;

	  స్థితి vpiIntVal:
	      /* NaN or +/- infinity are translated as 0. */
	    ఒకవేళ (val != val || (val && (val == 0.5*val))) {
		  val = 0.0;
	    } లేకపోతే {
		  val = std::విడత_2(val);
	    }
	    vp->value.integer = (PLI_INT32)val;
	    ఆపు;

	  స్థితి vpiDecStrVal:
	    ఒకవేళ (std::isnan(val))
		  snprintf(rbuf, RBUF_USE_కొలత, "%s", "nan");
	    లేకపోతే
		  snprintf(rbuf, RBUF_USE_కొలత, "%0.0f", std::విడత_2(val));
	    vp->value.str = rbuf;
	    ఆపు;

	  స్థితి vpiOctStrVal:
	    snprintf(rbuf, RBUF_USE_కొలత, "%" PRIo64, vlg_విడత_గమ్యం_u64(val));
	    vp->value.str = rbuf;
	    ఆపు;

	  స్థితి vpiHexStrVal:
	    snprintf(rbuf, RBUF_USE_కొలత, "%" PRIx64, vlg_విడత_గమ్యం_u64(val));
	    vp->value.str = rbuf;
	    ఆపు;

	  స్థితి vpiBinStrVal: {
		uint64_t vali = vlg_విడత_గమ్యం_u64(val);
		గుర్తులేని len = 0;

		అయితే (vali > 0) {
		      len += 1;
		      vali /= 2;
		}

		vali = vlg_విడత_గమ్యం_u64(val);
		కొరకు (గుర్తులేని idx = 0 ;  idx < len ;  idx += 1) {
		      rbuf[len-idx-1] = (vali & 1)? '1' : '0';
		      vali /= 2;
		}

		rbuf[len] = 0;
		ఒకవేళ (len == 0) {
		      rbuf[0] = '0';
		      rbuf[1] = 0;
		}
		vp->value.str = rbuf;
		ఆపు;
	  }

	  అప్రమేయ:
	    fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiConstant (Real)\n", (పూర్ణాంకం)vp->format);

	    vp->format = vpiSuppressVal;
	    ఆపు;
      }
}

అంతఃవరుస __vpiVThrWord::__vpiVThrWord(గుర్తులేని పునాది)
{
      name = vpip_name_string("W<>");
      subtype = vpiRealConst;
      సూచీ = పునాది;
}

పూర్ణాంకం __vpiVThrWord::get_type_సంకేతం(శూన్య) స్థిరరాశి
{ తిరిగి vpiConstant; }

పూర్ణాంకం __vpiVThrWord::vpi_get(పూర్ణాంకం code)
{ తిరిగి vthr_మాట_get(code, ఇది); }

శూన్య __vpiVThrWord::vpi_get_విలువ(p_vpi_విలువ val)
{ vthr_real_get_విలువ(ఇది, val); }

vpiHandle vpip_make_vthr_మాట(గుర్తులేని పునాది, స్థిరరాశి అక్షరం*type)
{
      ప్రతిజ్ఞా(type[0] == 'r');
      ప్రతిజ్ఞా(పునాది < 65536);
      __vpiVThrWord*obj = కొత్త __vpiVThrWord(పునాది);

      తిరిగి obj;
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
స్థిర map<vpiHandle, తార్కిక> handle_map;

శూన్య thread_మాట_delete(vpiHandle అంశం)
{
      handle_map[అంశం] = అబద్ధం;
}

స్థిర శూన్య thread_మాట_delete_real(vpiHandle అంశం)
{
      తరగతి __vpiVThrWord*obj = dynamic_cast<__vpiVThrWord*>(అంశం);
      తొలగించు obj;
}
#అయితే_ముగింపు

తరగతి __vpiVThrStrStack : బహిరంగ __vpiHandle {
    బహిరంగ:
      స్పష్టమైన __vpiVThrStrStack(గుర్తులేని లోతు);
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override;
      పూర్ణాంకం vpi_get(పూర్ణాంకం code) override;
      శూన్య vpi_get_విలువ(p_vpi_విలువ val) override;
    అంతరంగ:
      గుర్తులేని లోతు_2;
};

__vpiVThrStrStack::__vpiVThrStrStack(గుర్తులేని d)
: లోతు_2(d)
{
}

పూర్ణాంకం __vpiVThrStrStack::get_type_సంకేతం(శూన్య) స్థిరరాశి
{ తిరిగి vpiConstant; }

పూర్ణాంకం __vpiVThrStrStack::vpi_get(పూర్ణాంకం code)
{
      ఎంపిక (code) {
	  స్థితి vpiConstType:
	    తిరిగి vpiStringConst;
#ఒకవేళ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  స్థితి _vpiFromThr:
	    తిరిగి _vpiString;
#అయితే_ముగింపు
	  స్థితి vpi_కొలత:
	    తిరిగి vthread_get_str_దొంతర(vpip_current_vthread, లోతు_2).size();
	  అప్రమేయ:
	    తిరిగి 0;
      }
}

శూన్య __vpiVThrStrStack::vpi_get_విలువ(p_vpi_విలువ vp)
{
      string val;
      అక్షరం*rbuf = 0;

      ఒకవేళ (vpip_current_vthread)
	    val = vthread_get_str_దొంతర(vpip_current_vthread, లోతు_2);

      ఎంపిక (vp->format) {

	  స్థితి vpiObjTypeVal:
	    vp->format = vpiStringVal;
	    // fallthrough
	  స్థితి vpiStringVal:
	    rbuf = static_cast<అక్షరం *>(need_ఫలితం_buf(val.size()+1, RBUF_VAL));
	    strcpy(rbuf, val.c_str());
	    vp->value.str = rbuf;
	    ఆపు;

	  అప్రమేయ:
	    fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiConstant (String)\n", (పూర్ణాంకం)vp->format);

	    vp->format = vpiSuppressVal;
	    ఆపు;
      }
}

తరగతి __vpiVThrVec4Stack : బహిరంగ __vpiHandle {
    బహిరంగ:
      __vpiVThrVec4Stack(గుర్తులేని లోతు, తార్కిక signed_flag, గుర్తులేని wid);
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override;
      పూర్ణాంకం vpi_get(పూర్ణాంకం code) override;
      అక్షరం*vpi_get_str(పూర్ణాంకం code) override;
      శూన్య vpi_get_విలువ(p_vpi_విలువ val) override;
      vpiHandle vpi_put_విలువ(p_vpi_విలువ val, పూర్ణాంకం flags) override;
    అంతరంగ:
      స్థిర శూన్య vpi_get_విలువ_string(p_vpi_విలువ vp, స్థిరరాశి vvp_vector4_t&val);
      స్థిర శూన్య vpi_get_విలువ_binstr(p_vpi_విలువ vp, స్థిరరాశి vvp_vector4_t&val);
      శూన్య vpi_get_విలువ_decstr(p_vpi_విలువ vp, స్థిరరాశి vvp_vector4_t&val);
      శూన్య vpi_get_విలువ_int   (p_vpi_విలువ vp, స్థిరరాశి vvp_vector4_t&val);
      స్థిర శూన్య vpi_get_విలువ_real  (p_vpi_విలువ vp, స్థిరరాశి vvp_vector4_t&val);
      స్థిర శూన్య vpi_get_విలువ_strength(p_vpi_విలువ vp, స్థిరరాశి vvp_vector4_t&val);
      స్థిర శూన్య vpi_get_విలువ_octstr(p_vpi_విలువ vp, స్థిరరాశి vvp_vector4_t&val);
      స్థిర శూన్య vpi_get_విలువ_hexstr(p_vpi_విలువ vp, స్థిరరాశి vvp_vector4_t&val);
      స్థిర శూన్య vpi_get_విలువ_సదిశ(p_vpi_విలువ vp, స్థిరరాశి vvp_vector4_t&val);
    అంతరంగ:
      గుర్తులేని లోతు_2;
      తార్కిక signed_flag_;
      గుర్తులేని expect_వెడల్పు;
      స్థిరరాశి అక్షరం*name;
};

__vpiVThrVec4Stack::__vpiVThrVec4Stack(గుర్తులేని d, తార్కిక sf, గుర్తులేని wid)
: లోతు_2(d), signed_flag_(sf), expect_వెడల్పు(wid)
{
      name = vpip_name_string("S<,vec4,>");
}

పూర్ణాంకం __vpiVThrVec4Stack::get_type_సంకేతం(శూన్య) స్థిరరాశి
{ తిరిగి vpiConstant; }


పూర్ణాంకం __vpiVThrVec4Stack::vpi_get(పూర్ణాంకం code)
{
      ఎంపిక (code) {
	  స్థితి vpi_కొలత:
	    తిరిగి expect_వెడల్పు;

	  స్థితి vpiSigned:
	    తిరిగి signed_flag_? 1 : 0;

	  స్థితి vpiConstType:
	    తిరిగి vpi_ద్విమాన_Const;

#ఒకవేళ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  స్థితి _vpiFromThr:
	    తిరిగి _vpiVThr;
#అయితే_ముగింపు

	  అప్రమేయ:
	    తిరిగి 0;
      }
}

అక్షరం*__vpiVThrVec4Stack::vpi_get_str(పూర్ణాంకం code)
{
      ఎంపిక (code) {
	  స్థితి vpi_నిండిన_Name:
	    తిరిగి simple_సమితి_rbuf_str(name);

	  అప్రమేయ:
	    తిరిగి 0;
      }
}

శూన్య __vpiVThrVec4Stack::vpi_get_విలువ(p_vpi_విలువ vp)
{
      vvp_vector4_t val;

      ఒకవేళ (vpip_current_vthread)
	    val = vthread_get_vec4_దొంతర(vpip_current_vthread, లోతు_2);

      ఎంపిక (vp->format) {

	  స్థితి vpiBinStrVal:
	    vpi_get_విలువ_binstr(vp, val);
	    ఆపు;
	  స్థితి vpiDecStrVal:
	    vpi_get_విలువ_decstr(vp, val);
	    ఆపు;
	  స్థితి vpiOctStrVal:
	    vpi_get_విలువ_octstr(vp, val);
	    ఆపు;
	  స్థితి vpiHexStrVal:
	    vpi_get_విలువ_hexstr(vp, val);
	    ఆపు;
	  స్థితి vpiIntVal:
	    vpi_get_విలువ_int(vp, val);
	    ఆపు;
	  స్థితి vpiRealVal:
	    vpi_get_విలువ_real(vp, val);
	    ఆపు;
	  స్థితి vpiStringVal:
	    vpi_get_విలువ_string(vp, val);
	    ఆపు;
	  స్థితి vpiStrengthVal:
	    vpi_get_విలువ_strength(vp, val);
	    ఆపు;
	  స్థితి vpiObjTypeVal:
	    vp->format = vpi_సదిశ_Val;
	    // fallthrough
	  స్థితి vpi_సదిశ_Val:
	    vpi_get_విలువ_సదిశ(vp, val);
	    ఆపు;

	  అప్రమేయ:
	    fprintf(stderr, "internal error: vpi_get_value(<format=%d>)"
		    " not implemented for __vpiVThrVec4Stack.\n", vp->format);
	    ప్రతిజ్ఞా(0);
      }

}

శూన్య __vpiVThrVec4Stack::vpi_get_విలువ_binstr(p_vpi_విలువ vp, స్థిరరాశి vvp_vector4_t&val)
{
      గుర్తులేని wid = val.size();
      అక్షరం*rbuf = static_cast<అక్షరం*>(need_ఫలితం_buf(wid+1, RBUF_VAL));
      కొరకు (గుర్తులేని idx = 0 ; idx < wid ; idx += 1) {
	    rbuf[wid-idx-1] = vvp_bit4_గమ్యం_ascii(val.value(idx));
      }
      rbuf[wid] = 0;
      vp->value.str = rbuf;
}

శూన్య __vpiVThrVec4Stack::vpi_get_విలువ_decstr(p_vpi_విలువ vp, స్థిరరాశి vvp_vector4_t&val)
{
      గుర్తులేని wid = val.size();
      పూర్ణాంకం nbuf = (wid+2)/3 + 1;
      అక్షరం *rbuf = static_cast<అక్షరం*>(need_ఫలితం_buf(nbuf, RBUF_VAL));

      vpip_vec4_గమ్యం_dec_str(val, rbuf, nbuf, signed_flag_);
      vp->value.str = rbuf;
}

శూన్య __vpiVThrVec4Stack::vpi_get_విలువ_octstr(p_vpi_విలువ vp, స్థిరరాశి vvp_vector4_t&val)
{
      గుర్తులేని wid = val.size();
      గుర్తులేని owid = (wid + 2) / 3;
      అక్షరం*rbuf = static_cast<అక్షరం*>(need_ఫలితం_buf(owid+1, RBUF_VAL));
      rbuf[owid] = 0;

      గుర్తులేని oval = 0;
      కొరకు (గుర్తులేని idx = 0; idx < wid ; idx += 1) {
	    గుర్తులేని tmp = 0;
	    ఎంపిక (val.value(idx)) {
		స్థితి BIT4_0:
		  tmp = 0;
		  ఆపు;
		స్థితి BIT4_1:
		  tmp = 1;
		  ఆపు;
		స్థితి BIT4_X:
		  tmp = 2;
		  ఆపు;
		స్థితి BIT4_Z:
		  tmp = 3;
		  ఆపు;
	    }

	    oval = oval | (tmp << 2*(idx%3));

	    ఒకవేళ (idx%3 == 2) {
		  owid -= 1;
		  rbuf[owid] = oct_digits[oval];
		  oval = 0;
	    }
      }

      ఒకవేళ (owid > 0) {
	    owid -= 1;
	    rbuf[owid] = oct_digits[oval];
      }
      vp->value.str = rbuf;
}

శూన్య __vpiVThrVec4Stack::vpi_get_విలువ_hexstr(p_vpi_విలువ vp, స్థిరరాశి vvp_vector4_t&val)
{
      గుర్తులేని wid = val.size();
      గుర్తులేని hwid = (wid + 3) /4;
      అక్షరం*rbuf = static_cast<అక్షరం*>(need_ఫలితం_buf(hwid+1, RBUF_VAL));
      rbuf[hwid] = 0;

      గుర్తులేని hval = 0;
      కొరకు (గుర్తులేని idx = 0; idx < wid ; idx += 1) {
	    గుర్తులేని tmp = 0;
	    ఎంపిక (val.value(idx)) {
		స్థితి BIT4_0:
		  tmp = 0;
		  ఆపు;
		స్థితి BIT4_1:
		  tmp = 1;
		  ఆపు;
		స్థితి BIT4_X:
		  tmp = 2;
		  ఆపు;
		స్థితి BIT4_Z:
		  tmp = 3;
		  ఆపు;
	    }
	    hval = hval | (tmp << 2*(idx%4));

	    ఒకవేళ (idx%4 == 3) {
		  hwid -= 1;
		  rbuf[hwid] = hex_digits[hval];
		  hval = 0;
	    }
      }

      ఒకవేళ (hwid > 0) {
	    hwid -= 1;
	    rbuf[hwid] = hex_digits[hval];
      }
      vp->value.str = rbuf;
}

శూన్య __vpiVThrVec4Stack::vpi_get_విలువ_int(p_vpi_విలువ vp, స్థిరరాశి vvp_vector4_t&val)
{
      int32_t vali = 0;
      vector4_గమ్యం_విలువ(val, vali, signed_flag_, అబద్ధం);
      vp->value.integer = vali;
}

శూన్య __vpiVThrVec4Stack::vpi_get_విలువ_real(p_vpi_విలువ vp, స్థిరరాశి vvp_vector4_t&val)
{
      గుర్తులేని wid = val.size();
      vp->value.real = 0.0;

      కొరకు (గుర్తులేని idx = wid ; idx > 0 ; idx -= 1) {
	    vp->value.real *= 2.0;
	    ఒకవేళ (val.value(idx-1) == BIT4_1)
		  vp->value.real += 1.0;
      }
}

శూన్య __vpiVThrVec4Stack::vpi_get_విలువ_string(p_vpi_విలువ vp, స్థిరరాశి vvp_vector4_t&val)
{
      అక్షరం*rbuf = static_cast<అక్షరం*>(need_ఫలితం_buf((val.size() / 8) + 1, RBUF_VAL));
      అక్షరం*cp = rbuf;

      అక్షరం tmp = 0;
      కొరకు (పూర్ణాంకం bitnr = val.size()-1 ; bitnr >= 0 ; bitnr -= 1) {
	    tmp <<= 1;
	    ఎంపిక (val.value(bitnr)) {
		స్థితి BIT4_1:
		  tmp |= 1;
		  ఆపు;
		స్థితి BIT4_0:
		అప్రమేయ:
		  ఆపు;
	    }

	    ఒకవేళ ((bitnr&7)==0) {
		    // Don't include leading nuls
		  ఒకవేళ (tmp == 0 && cp == rbuf)
			కొనసాగు;

		  *cp++ = tmp? tmp : ' ';
		  tmp = 0;
	    }
      }
      *cp++ = 0;
      vp->format = vpiStringVal;
      vp->value.str = rbuf;
}

శూన్య __vpiVThrVec4Stack::vpi_get_విలువ_సదిశ(p_vpi_విలువ vp, స్థిరరాశి vvp_vector4_t&val)
{
      గుర్తులేని wid = val.size();

      గుర్తులేని hwid = (wid+31)/32;
      vp->value.vector = static_cast<s_vpi_vecval*>
                         (need_ఫలితం_buf(hwid*పరిమాణం(s_vpi_vecval), RBUF_VAL));
      ప్రతిజ్ఞా(vp->value.vector);

	/* Zero the top of the result buffer first since the fill loop below only writes
	   bits 0..wid-1, which can leave stale data in the unused high bits. */
      ఒకవేళ (hwid == 0) hwid = 1;
      vp->value.vector[hwid-1].aval = 0;
      vp->value.vector[hwid-1].bval = 0;

      కొరకు (గుర్తులేని idx = 0 ;  idx < wid ;  idx += 1) {
	    పూర్ణాంకం మాట = idx/32;
	    PLI_INT32 ముసుగు = 1 << (idx%32);

	    ఎంపిక (val.value(idx)) {
		స్థితి BIT4_0:
		  vp->value.vector[మాట].aval &= ~ముసుగు;
		  vp->value.vector[మాట].bval &= ~ముసుగు;
		  ఆపు;
		స్థితి BIT4_1:
		  vp->value.vector[మాట].aval |=  ముసుగు;
		  vp->value.vector[మాట].bval &= ~ముసుగు;
		  ఆపు;
		స్థితి BIT4_X:
		  vp->value.vector[మాట].aval |=  ముసుగు;
		  vp->value.vector[మాట].bval |=  ముసుగు;
		  ఆపు;
		స్థితి BIT4_Z:
		  vp->value.vector[మాట].aval &= ~ముసుగు;
		  vp->value.vector[మాట].bval |=  ముసుగు;
		  ఆపు;
	    }
      }
}

శూన్య __vpiVThrVec4Stack::vpi_get_విలువ_strength(p_vpi_విలువ vp, స్థిరరాశి vvp_vector4_t&val)
{
      s_vpi_strengthval*op = static_cast<s_vpi_strengthval*>
                             (need_ఫలితం_buf(val.size() * పరిమాణం(s_vpi_strengthval),
                                              RBUF_VAL));

      కొరకు (గుర్తులేని idx = 0 ; idx < val.size() ; idx += 1) {
	    ఎంపిక (val.value(idx)) {
		స్థితి BIT4_0:
		  op[idx].logic = vpi0;
		  op[idx].s0 = vpiStrongDrive;
		  op[idx].s1 = 0;
		  ఆపు;
		స్థితి BIT4_1:
		  op[idx].logic = vpi1;
		  op[idx].s0 = 0;
		  op[idx].s1 = vpiStrongDrive;
		  ఆపు;
		స్థితి BIT4_X:
		  op[idx].logic = vpiX;
		  op[idx].s0 = vpiStrongDrive;
		  op[idx].s1 = vpiStrongDrive;
		  ఆపు;
		స్థితి BIT4_Z:
		  op[idx].logic = vpiZ;
		  op[idx].s0 = vpiHiZ;
		  op[idx].s1 = vpiHiZ;
		  ఆపు;
	    }
      }

      vp->format = vpiStrengthVal;
      vp->value.strength = op;
}

vpiHandle __vpiVThrVec4Stack::vpi_put_విలువ(p_vpi_విలువ vp, పూర్ణాంకం /*flags*/)
{
      ప్రతిజ్ఞా(vpip_current_vthread);

      ఎంపిక (vp->format) {

	  అప్రమేయ:
	    fprintf(stderr, "internal error: vpi_put_value(<format=%d>)"
		    " not implemented for __vpiVThrVec4Stack.\n", vp->format);
	    ప్రతిజ్ఞా(0);
	    తిరిగి 0;
      }
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
శూన్య thread_vthr_delete(vpiHandle అంశం)
{
      handle_map[అంశం] = నిజం;
}

స్థిర శూన్య thread_vthr_delete_real(vpiHandle అంశం)
{
      __vpiVThrVec4Stack*obj = dynamic_cast<__vpiVThrVec4Stack*>(అంశం);
      తొలగించు obj;
}

శూన్య vpi_handle_delete()
{
      map<vpiHandle, తార్కిక>::iterator iter;
      కొరకు (iter = handle_map.begin(); iter != handle_map.చివర(); ++ iter ) {
	    ఒకవేళ (iter->second) thread_vthr_delete_real(iter->మొదటి);
	    లేకపోతే thread_మాట_delete_real(iter->మొదటి);
      }
}
#అయితే_ముగింపు


vpiHandle vpip_make_vthr_str_దొంతర(గుర్తులేని లోతు)
{
      __vpiVThrStrStack*obj = కొత్త __vpiVThrStrStack(లోతు);
      తిరిగి obj;
}

vpiHandle vpip_make_vthr_vec4_దొంతర(గుర్తులేని లోతు, తార్కిక signed_flag, గుర్తులేని wid)
{
      __vpiVThrVec4Stack*obj = కొత్త __vpiVThrVec4Stack(లోతు, signed_flag, wid);
      తిరిగి obj;
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
స్థిర map<vpiHandle, తార్కిక> దొంతర_map;

శూన్య thread_string_delete(vpiHandle అంశం)
{
      దొంతర_map[అంశం] = అబద్ధం;
}

స్థిర శూన్య thread_string_delete_real(vpiHandle అంశం)
{
      __vpiVThrStrStack*obj = dynamic_cast<__vpiVThrStrStack*>(అంశం);
      తొలగించు obj;
}

శూన్య vpi_దొంతర_delete()
{
      map<vpiHandle, తార్కిక>::iterator iter;
      కొరకు (iter = దొంతర_map.begin(); iter != దొంతర_map.చివర(); ++ iter ) {
	    thread_string_delete_real(iter->మొదటి);
      }
}
#అయితే_ముగింపు
