/*
 * Copyright (c) 2001-2026 Stephen Williams (steve@icarus.com)
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

# include  "vpi_priv.h"
# include  "compile.h"
# include  "config.h"
#నిర్వచితమైతే CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#అయితే_ముగింపు
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <climits>
# include  <cassert>
# include  "ivl_alloc.h"

తరగతి __vpiStringConst : బహిరంగ __vpiHandle {
    బహిరంగ:
      స్పష్టమైన __vpiStringConst(అక్షరం*val);
      ~__vpiStringConst() override;
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override;
      పూర్ణాంకం vpi_get(పూర్ణాంకం code) override;
      శూన్య vpi_get_విలువ(p_vpi_విలువ val) override;

    అంతరంగ:
      శూన్య process_string_();
    అంతరంగ:
      అక్షరం*విలువ_2;
      size_t విలువ_len;
};

అంతఃవరుస __vpiStringConst::__vpiStringConst(అక్షరం*v)
: విలువ_2(v)
{
      process_string_();
}

/*
 * Strings are described at the level of the vvp source as a string
 * with literal characters or octal escapes. No other escapes are
 * included, they are processed already by the compiler that generated
 * the vvp source.
 */
శూన్య __vpiStringConst::process_string_(శూన్య)
{
      అక్షరం*chr = విలువ_2;
      అక్షరం*dp = విలువ_2;

      అయితే (*chr) {
	    అక్షరం తదుపరి_char = *chr;

	      /* Process octal escapes that I might find. */
	    ఒకవేళ (*chr == '\\') {
		  కొరకు (పూర్ణాంకం idx = 1 ;  idx <= 3 ;  idx += 1) {
			ప్రతిజ్ఞా(chr[idx] != 0);
			ప్రతిజ్ఞా(chr[idx] < '8');
			ప్రతిజ్ఞా(chr[idx] >= '0');
			తదుపరి_char = తదుపరి_char*8 + chr[idx] - '0';
		  }
		  chr += 3;
	    }
	    *dp++ = తదుపరి_char;
	    chr += 1;
      }
      *dp = 0;
      విలువ_len = dp - విలువ_2;
}

__vpiStringConst::~__vpiStringConst()
{
      తొలగించు[] విలువ_2;
}

పూర్ణాంకం __vpiStringConst::get_type_సంకేతం(శూన్య) స్థిరరాశి
{ తిరిగి vpiConstant; }

పూర్ణాంకం __vpiStringConst::vpi_get(పూర్ణాంకం code)
{
      ఎంపిక (code) {
          స్థితి vpi_కొలత:
	    తిరిగి విలువ_len * 8;

          స్థితి vpiSigned:
	      తిరిగి 0;

	  స్థితి vpiConstType:
	      తిరిగి vpiStringConst;

          స్థితి vpiAutomatic:
	      తిరిగి 0;

#ఒకవేళ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          స్థితి _vpiFromThr:
	      తిరిగి _vpiNoThr;
#అయితే_ముగింపు

	  అప్రమేయ:
	      fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiStringConst\n", code);
	      ప్రతిజ్ఞా(0);
	      తిరిగి 0;
      }
}

శూన్య __vpiStringConst::vpi_get_విలువ(p_vpi_విలువ vp)
{
      గుర్తులేని uint_విలువ;
      p_vpi_vecval vecp;
      గుర్తులేని size = విలువ_len;
      అక్షరం*rbuf = 0;
      అక్షరం*cp;

      ఎంపిక (vp->format) {
	  స్థితి vpiObjTypeVal:
	      /* String parameters by default have vpiStringVal values. */
	    vp->format = vpiStringVal;
	    // fallthrough
	  స్థితి vpiStringVal:
	    cp = విలువ_2;
	    rbuf = static_cast<అక్షరం *>(need_ఫలితం_buf(size + 1, RBUF_VAL));
	    vp->value.str = rbuf;

	    కొరకు (గుర్తులేని పూర్ణాంకం i = 0; i < size; i++) {
		  // Ignore leading null-bytes and replace other null-bytes with space.
		  // The LRM is not entirely clear on how null bytes should be handled.
		  // This is the implementation chosen for iverilog.
		  ఒకవేళ (*cp)
			*rbuf++ = *cp;
		  లేకపోతే ఒకవేళ (rbuf != vp->value.str)
			*rbuf++ = ' ';

		  cp++;
	    }
	    *rbuf = '\0';
	    ఆపు;

          స్థితి vpiDecStrVal:
	    // Take the (up to 4) characters of the text, convert the characters
	    // to a numerical value, and convert that value to a decimal string.
	    // For example, the string "A" is ASCII 65, so the resulting string
	    // will be "65". and the string "AB" is 65*256 + 66 == 16706, so
	    // the resulting string is "16706". The "size" is the number of
	    // characters of input text to put to work.
	    ఒకవేళ (size > 4){
		  // We only support standard integers. Ignore other bytes...
		  size = 4;
		  fprintf(stderr, "Warning (vpi_const.cc): %%d on constant strings only looks "
			  "at first 4 bytes!\n");
	    }
	    స్థిర స్థిరరాశి size_t RBUF_USE_కొలత = 12;
	    rbuf = static_cast<అక్షరం *>(need_ఫలితం_buf(RBUF_USE_కొలత, RBUF_VAL));
	    uint_విలువ = 0;
	    కొరకు(గుర్తులేని i=0; i<size; i += 1){
		  uint_విలువ <<=8;
		  uint_విలువ += (గుర్తులేని అక్షరం)(విలువ_2[i]);
	    }
	    snprintf(rbuf, RBUF_USE_కొలత, "%u", uint_విలువ);
	    vp->value.str = rbuf;
	    ఆపు;

          స్థితి vpiBinStrVal:
	      rbuf = static_cast<అక్షరం *>(need_ఫలితం_buf(8 * size + 1, RBUF_VAL));
	      cp = rbuf;
	      కొరకు(గుర్తులేని i=0; i<size; i += 1){
		  కొరకు(పూర్ణాంకం బిట్టు=7; బిట్టు>=0; బిట్టు -= 1){
		      *cp++ = "01"[ (విలువ_2[i]>>బిట్టు)&1 ];
		  }
	      }
	      *cp = 0;
	      vp->value.str = rbuf;
	      ఆపు;

          స్థితి vpiHexStrVal:
	      rbuf = static_cast<అక్షరం *>(need_ఫలితం_buf(2 * size + 1, RBUF_VAL));
	      cp = rbuf;
	      కొరకు(గుర్తులేని i=0; i<size; i += 1){
		  కొరకు(పూర్ణాంకం nibble=1; nibble>=0; nibble -= 1){
		      *cp++ = "0123456789abcdef"[ (విలువ_2[i]>>(nibble*4))&15 ];
		  }
	      }
	      *cp = 0;
	      vp->value.str = rbuf;
	      ఆపు;

          స్థితి vpiOctStrVal:
	      fprintf(stderr, "ERROR (vpi_const.cc): %%o display of constant strings not yet implemented\n");
	      ప్రతిజ్ఞా(0);
	      ఆపు;

          స్థితి vpiIntVal:
	      vp->value.integer = 0;
	      కొరకు(గుర్తులేని i=0; i<size; i += 1){
		  కొరకు(పూర్ణాంకం బిట్టు=7; బిట్టు>=0; బిట్టు -= 1){
		      vp->value.integer <<= 1;
		      vp->value.integer += (విలువ_2[i]>>బిట్టు)&1;
		  }
	      }
	      ఆపు;

          స్థితి vpi_సదిశ_Val:
              vp->value.vector = static_cast<p_vpi_vecval>
                                 (need_ఫలితం_buf((size+3)/4*
                                                   పరిమాణం(s_vpi_vecval),
                                                  RBUF_VAL));
              uint_విలువ = 0;
              vecp = vp->value.vector;
              vecp->aval = vecp->bval = 0;
	      కొరకు(గుర్తులేని i=0; i<size; i += 1){
		  vecp->aval |= విలువ_2[i] << uint_విలువ*8;
		  uint_విలువ += 1;
		  ఒకవేళ (uint_విలువ > 3) {
		      uint_విలువ = 0;
		      vecp += 1;
		      vecp->aval = vecp->bval = 0;
		  }
	      }
	      ఆపు;


	  అప్రమేయ:
	    fprintf(stderr, "ERROR (vpi_const.cc): vp->format: %d\n",
	            (పూర్ణాంకం)vp->format);
	    ప్రతిజ్ఞా(0);

	    vp->format = vpiSuppressVal;
	    ఆపు;
      }
}


నిర్మాణం __vpiStringConstTEMP : బహిరంగ __vpiStringConst {
      స్పష్టమైన అంతఃవరుస __vpiStringConstTEMP(అక్షరం*v) : __vpiStringConst(v) { }
      free_object_fun_t free_object_fun(శూన్య) override;
};

స్థిర పూర్ణాంకం free_తాత్కాలిక_string(vpiHandle obj)
{
      నిర్మాణం __vpiStringConstTEMP*rfp = dynamic_cast<__vpiStringConstTEMP*>(obj);
      తొలగించు rfp;
      తిరిగి 1;
}

__vpiHandle::free_object_fun_t __vpiStringConstTEMP::free_object_fun(శూన్య)
{ తిరిగి &free_తాత్కాలిక_string; }

vpiHandle vpip_make_string_const(అక్షరం*పాఠ్యం, తార్కిక persistent_flag)
{
      __vpiStringConst*obj;

      obj = persistent_flag? కొత్త __vpiStringConst(పాఠ్యం) : కొత్త __vpiStringConstTEMP(పాఠ్యం);

      తిరిగి obj;
}


తరగతి __vpiStringParam  : బహిరంగ __vpiStringConst {
    బహిరంగ:
      __vpiStringParam(__vpiScope* scope, అక్షరం*txt, స్థిరరాశి అక్షరం*name,
                       తార్కిక local_flag, దీర్ఘ file_idx, దీర్ఘ lineno);
      ~__vpiStringParam() override;
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override;
      పూర్ణాంకం vpi_get(పూర్ణాంకం code) override;
      అక్షరం*vpi_get_str(పూర్ణాంకం code) override;
      vpiHandle vpi_handle(పూర్ణాంకం code) override;

    అంతరంగ:
      __vpiScope* scope_;
      స్థిరరాశి అక్షరం*basename_;
      తార్కిక       local_flag_;
      గుర్తులేని   file_idx_;
      గుర్తులేని   lineno_;
};

అంతఃవరుస __vpiStringParam::__vpiStringParam(__vpiScope* scope, అక్షరం*txt, స్థిరరాశి అక్షరం*nam,
                                          తార్కిక local_flag, దీర్ఘ file_idx, దీర్ఘ lineno)
: __vpiStringConst(txt)
{
      scope_ = scope;
      basename_ = nam;
      local_flag_ = local_flag;
      ప్రతిజ్ఞా(file_idx >= 0 && file_idx <= గరిష్ఠ_పూర్ణాంకం);
      file_idx_ = (గుర్తులేని) file_idx;
      ప్రతిజ్ఞా(lineno >= 0 && lineno <= గరిష్ఠ_పూర్ణాంకం);
      lineno_ = (గుర్తులేని) lineno;
}

__vpiStringParam::~__vpiStringParam()
{
      తొలగించు[]basename_;
}

పూర్ణాంకం __vpiStringParam::get_type_సంకేతం(శూన్య) స్థిరరాశి
{ తిరిగి vpiParameter; }

పూర్ణాంకం __vpiStringParam::vpi_get(పూర్ణాంకం code)
{
    ఎంపిక (code) {
       స్థితి vpi_పంక్తి_No :
         తిరిగి lineno_;

       స్థితి vpiLocalParam :
         తిరిగి local_flag_;

       అప్రమేయ :
         తిరిగి __vpiStringConst::vpi_get(code);
    }
}


అక్షరం*__vpiStringParam::vpi_get_str(పూర్ణాంకం code)
{
      ఒకవేళ (code == vpiFile) {
	    తిరిగి simple_సమితి_rbuf_str(file_పేర్లు[file_idx_]);
      }

      తిరిగి generic_get_str(code, scope_, basename_, ఖాళీ);
}


vpiHandle __vpiStringParam::vpi_handle(పూర్ణాంకం code)
{
      ఎంపిక (code) {
	  స్థితి vpiScope:
	    తిరిగి scope_;

	  స్థితి vpiModule:
	    తిరిగి vpip_module(scope_);

	  అప్రమేయ:
	    తిరిగి 0;
      }
}

vpiHandle vpip_make_string_param(స్థిరరాశి అక్షరం*name, అక్షరం*పాఠ్యం,
                                 తార్కిక local_flag, దీర్ఘ file_idx, దీర్ఘ lineno)
{
      __vpiStringParam*obj = కొత్త __vpiStringParam(vpip_peek_current_scope(), పాఠ్యం, name,
                                                  local_flag, file_idx, lineno);

      తిరిగి obj;
}



అంతఃవరుస __vpiBinaryConst::__vpiBinaryConst()
: signed_flag(0), sized_flag(0)
{ }

పూర్ణాంకం __vpiBinaryConst::get_type_సంకేతం(శూన్య) స్థిరరాశి
{ తిరిగి vpiConstant; }

పూర్ణాంకం __vpiBinaryConst::vpi_get(పూర్ణాంకం code)
{
      ఎంపిక (code) {
	  స్థితి vpiConstType:
	    తిరిగి vpi_ద్విమాన_Const;

	  స్థితి vpi_పంక్తి_No:
	    తిరిగి 0;  // Not implemented for now!

	  స్థితి vpiSigned:
	    తిరిగి signed_flag? 1 : 0;

	  స్థితి vpi_కొలత:
	    తిరిగి బిట్లు.size();

          స్థితి vpiAutomatic:
	    తిరిగి 0;

#ఒకవేళ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          స్థితి _vpiFromThr:
	      తిరిగి _vpiNoThr;
#అయితే_ముగింపు

	  అప్రమేయ:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiBinaryConst\n", code);
	    ప్రతిజ్ఞా(0);
	    తిరిగి 0;
      }
}


శూన్య __vpiBinaryConst::vpi_get_విలువ(p_vpi_విలువ val)
{
      ఎంపిక (val->format) {

	  స్థితి vpiObjTypeVal:
	  స్థితి vpiBinStrVal:
	  స్థితి vpiDecStrVal:
	  స్థితి vpiOctStrVal:
	  స్థితి vpiHexStrVal:
          స్థితి vpiScalarVal:
	  స్థితి vpiIntVal:
	  స్థితి vpi_సదిశ_Val:
	  స్థితి vpiStringVal:
	  స్థితి vpiRealVal:
	    vpip_vec4_get_విలువ(బిట్లు, బిట్లు.size(), signed_flag, val);
	    ఆపు;

	  అప్రమేయ:
	    fprintf(stderr, "vvp error: format %d not supported "
		    "by vpiBinaryConst\n", (పూర్ణాంకం)val->format);
	    val->format = vpiSuppressVal;
	    ఆపు;
      }
}

/*
 * Make a VPI constant from a vector string. The string is normally a
 * ASCII string, with each letter a 4-value bit. The first character
 * may be an 's' if the vector is signed.
 */
vpiHandle vpip_make_ద్విమాన_const(గుర్తులేని wid, స్థిరరాశి అక్షరం*బిట్లు)
{
      నిర్మాణం __vpiBinaryConst*obj = కొత్త __vpiBinaryConst;

      స్థిరరాశి అక్షరం*bp = బిట్లు;
      ఒకవేళ (*bp == 's') {
	    bp += 1;
	    obj->signed_flag = 1;
      }

      obj->బిట్లు = vector4_from_పాఠ్యం(bp, wid);

      తిరిగి obj;
}

vvp_vector4_t vector4_from_పాఠ్యం(స్థిరరాశి అక్షరం*బిట్లు, గుర్తులేని wid)
{
      vvp_vector4_t res (wid);

      కొరకు (గుర్తులేని idx = 0 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t val = BIT4_0;
	    ఎంపిక (బిట్లు[wid-idx-1]) {
		స్థితి '0':
		  val = BIT4_0;
		  ఆపు;
		స్థితి '1':
		  val = BIT4_1;
		  ఆపు;
		స్థితి 'x':
		  val = BIT4_X;
		  ఆపు;
		స్థితి 'z':
		  val = BIT4_Z;
		  ఆపు;
	    }

	    res.సమితి_బిట్టు(idx, val);
      }

      తిరిగి res;
}

నిర్మాణం __vpiBinaryParam  : బహిరంగ __vpiBinaryConst {
      __vpiBinaryParam(__vpiScope*scope, స్థిరరాశి vvp_vector4_t&b, తార్కిక is_signed,
                       స్థిరరాశి అక్షరం*name, తార్కిక local_flag, దీర్ఘ file_idx, దీర్ఘ lineno);
      ~__vpiBinaryParam() override;
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override;
      పూర్ణాంకం vpi_get(పూర్ణాంకం code) override;
      అక్షరం*vpi_get_str(పూర్ణాంకం code) override;
      vpiHandle vpi_handle(పూర్ణాంకం code) override;

    అంతరంగ:
      __vpiScope*scope_;
      స్థిరరాశి అక్షరం*basename_;
      తార్కిక     local_flag_;
      గుర్తులేని file_idx_;
      గుర్తులేని lineno_;
};

అంతఃవరుస __vpiBinaryParam::__vpiBinaryParam(__vpiScope*scope, స్థిరరాశి vvp_vector4_t&b,
                                          తార్కిక is_signed, స్థిరరాశి అక్షరం*nam,
                                          తార్కిక local_flag, దీర్ఘ file_idx, దీర్ఘ lineno)
{
      scope_ = scope;
      బిట్లు = b;
      signed_flag = is_signed ? 1 : 0;
      sized_flag = 0;
      basename_ = nam;
      local_flag_ = local_flag;
      ప్రతిజ్ఞా(file_idx >= 0 && file_idx <= గరిష్ఠ_పూర్ణాంకం);
      file_idx_ = (గుర్తులేని) file_idx;
      ప్రతిజ్ఞా(lineno >= 0 && lineno <= గరిష్ఠ_పూర్ణాంకం);
      lineno_ = (గుర్తులేని) lineno;
}

__vpiBinaryParam::~__vpiBinaryParam()
{
      తొలగించు[]basename_;
}

పూర్ణాంకం __vpiBinaryParam::get_type_సంకేతం(శూన్య) స్థిరరాశి
{ తిరిగి vpiParameter; }

పూర్ణాంకం __vpiBinaryParam::vpi_get(పూర్ణాంకం code)
{
    ఎంపిక (code) {
      స్థితి vpi_పంక్తి_No :
        తిరిగి lineno_;

      స్థితి vpiLocalParam :
        తిరిగి local_flag_;

      అప్రమేయ :
        తిరిగి __vpiBinaryConst::vpi_get(code);
    }
}

అక్షరం*__vpiBinaryParam::vpi_get_str(పూర్ణాంకం code)
{
      ఒకవేళ (code == vpiFile)
	    తిరిగి simple_సమితి_rbuf_str(file_పేర్లు[file_idx_]);

      తిరిగి generic_get_str(code, scope_, basename_, ఖాళీ);
}


vpiHandle __vpiBinaryParam::vpi_handle(పూర్ణాంకం code)
{
      ఎంపిక (code) {
	  స్థితి vpiScope:
	    తిరిగి scope_;

	  స్థితి vpiModule:
	    తిరిగి vpip_module(scope_);

	  అప్రమేయ:
	    తిరిగి 0;
      }
}


vpiHandle vpip_make_ద్విమాన_param(స్థిరరాశి అక్షరం*name, స్థిరరాశి vvp_vector4_t&బిట్లు,
				 తార్కిక signed_flag, తార్కిక local_flag,
				 దీర్ఘ file_idx, దీర్ఘ lineno)
{
      నిర్మాణం __vpiBinaryParam*obj = కొత్త __vpiBinaryParam(vpip_peek_current_scope(),
                                                         బిట్లు, signed_flag, name,
                                                         local_flag, file_idx, lineno);

      తిరిగి obj;
}



__vpiDecConst::__vpiDecConst(పూర్ణాంకం val)
{
      value = val;
}

__vpiDecConst::__vpiDecConst(స్థిరరాశి __vpiDecConst&that)
: __vpiHandle(), value(that.value)
{
}


పూర్ణాంకం __vpiDecConst::get_type_సంకేతం(శూన్య) స్థిరరాశి
{ తిరిగి vpiConstant; }

పూర్ణాంకం __vpiDecConst::vpi_get(పూర్ణాంకం code)
{
      ఎంపిక (code) {
	  స్థితి vpiConstType:
	    తిరిగి vpiDecConst;

	  స్థితి vpiSigned:
	    తిరిగి 1;

	  స్థితి vpi_కొలత:
	    తిరిగి 32;

          స్థితి vpiAutomatic:
	    తిరిగి 0;

#ఒకవేళ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          స్థితి _vpiFromThr:
	      తిరిగి _vpiNoThr;
#అయితే_ముగింపు

	  అప్రమేయ:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiDecConst\n", code);
	    ప్రతిజ్ఞా(0);
	    తిరిగి 0;
      }
}


శూన్య __vpiDecConst::vpi_get_విలువ(p_vpi_విలువ vp)
{
      స్థిర స్థిరరాశి size_t RBUF_USE_కొలత = 64 + 1;
      అక్షరం*rbuf = static_cast<అక్షరం *>(need_ఫలితం_buf(RBUF_USE_కొలత, RBUF_VAL));
      అక్షరం*cp = rbuf;

      ఎంపిక (vp->format) {

	  స్థితి vpiObjTypeVal:
	  స్థితి vpiIntVal: {
		vp->value.integer = value;
		ఆపు;
	  }

          స్థితి vpiDecStrVal:
	      snprintf(rbuf, RBUF_USE_కొలత, "%d", value);
	      vp->value.str = rbuf;
	      ఆపు;

          స్థితి vpiBinStrVal:
	      కొరకు(పూర్ణాంకం బిట్టు=31; బిట్టు>=0;బిట్టు--){
		  *cp++ = "01"[ (static_cast<గుర్తులేని>(value)>>బిట్టు)&1 ];
	      }
	      *cp = 0;

	      vp->value.str = rbuf;
	      ఆపు;

          స్థితి vpiHexStrVal:
	      snprintf(rbuf, RBUF_USE_కొలత, "%08x", value);
	      vp->value.str = rbuf;
	      ఆపు;

          స్థితి vpiOctStrVal:
	      snprintf(rbuf, RBUF_USE_కొలత, "%011x", value);
	      vp->value.str = rbuf;
	      ఆపు;

	  అప్రమేయ:
	    fprintf(stderr, "vvp error (vpi_const.cc): format %d not supported "
		    "by vpiDecConst\n", (పూర్ణాంకం)vp->format);
	    vp->format = vpiSuppressVal;
	    ఆపు;
      }
}


అంతఃవరుస __vpiRealConst::__vpiRealConst(ద్విగుణ val)
: value(val)
{ }

పూర్ణాంకం __vpiRealConst::get_type_సంకేతం(శూన్య) స్థిరరాశి
{ తిరిగి vpiConstant; }

పూర్ణాంకం __vpiRealConst::vpi_get(పూర్ణాంకం code)
{
      ఎంపిక (code) {
	  స్థితి vpi_పంక్తి_No:
	    తిరిగి 0;  // Not implemented for now!

	  స్థితి vpi_కొలత:
	    తిరిగి 1;

	  స్థితి vpiConstType:
	    తిరిగి vpiRealConst;

	  స్థితి vpiSigned:
	    తిరిగి 1;

          స్థితి vpiAutomatic:
	    తిరిగి 0;

#ఒకవేళ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          స్థితి _vpiFromThr:
	      తిరిగి _vpiNoThr;
#అయితే_ముగింపు

	  అప్రమేయ:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiRealConst\n", code);
	    ప్రతిజ్ఞా(0);
	    తిరిగి 0;
      }
}


శూన్య __vpiRealConst::vpi_get_విలువ(p_vpi_విలువ val)
{
      vpip_real_get_విలువ(value, val);
}


vpiHandle vpip_make_real_const(ద్విగుణ value)
{
      __vpiRealConst*obj = కొత్త __vpiRealConst(value);
      తిరిగి obj;
}

నిర్మాణం __vpiRealParam  : బహిరంగ __vpiRealConst {
      __vpiRealParam(__vpiScope* scope, ద్విగుణ val, స్థిరరాశి అక్షరం*name,
                     తార్కిక local_flag, దీర్ఘ file_idx, దీర్ఘ lineno);
      ~__vpiRealParam() override;
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override;
      పూర్ణాంకం vpi_get(పూర్ణాంకం code) override;
      అక్షరం*vpi_get_str(పూర్ణాంకం code) override;
      vpiHandle vpi_handle(పూర్ణాంకం code) override;

    అంతరంగ:
      __vpiScope* scope_;
      స్థిరరాశి అక్షరం*basename_;
      తార్కిక local_flag_;
      గుర్తులేని file_idx_;
      గుర్తులేని lineno_;
};


అంతఃవరుస __vpiRealParam::__vpiRealParam(__vpiScope* scope, ద్విగుణ val, స్థిరరాశి అక్షరం*name,
                                      తార్కిక local_flag, దీర్ఘ file_idx, దీర్ఘ lineno)
: __vpiRealConst(val)
{
      scope_ = scope;
      basename_ = name;
      local_flag_ = local_flag;
      ప్రతిజ్ఞా(file_idx >= 0 && file_idx <= గరిష్ఠ_పూర్ణాంకం);
      file_idx_ = (గుర్తులేని) file_idx;
      ప్రతిజ్ఞా(lineno >= 0 && lineno <= గరిష్ఠ_పూర్ణాంకం);
      lineno_ = (గుర్తులేని) lineno;
}

__vpiRealParam::~__vpiRealParam()
{
      తొలగించు[]basename_;
}


పూర్ణాంకం __vpiRealParam::get_type_సంకేతం(శూన్య) స్థిరరాశి
{ తిరిగి vpiParameter; }

పూర్ణాంకం __vpiRealParam::vpi_get(పూర్ణాంకం code)
{
    ఎంపిక (code) {
      స్థితి vpi_పంక్తి_No :
        తిరిగి lineno_;

      స్థితి vpiLocalParam :
        తిరిగి local_flag_;

      అప్రమేయ :
           తిరిగి __vpiRealConst::vpi_get(code);
    }
}

అక్షరం* __vpiRealParam::vpi_get_str(పూర్ణాంకం code)
{
      ఒకవేళ (code == vpiFile)
            తిరిగి simple_సమితి_rbuf_str(file_పేర్లు[file_idx_]);

      తిరిగి generic_get_str(code, scope_, basename_, ఖాళీ);
}

vpiHandle __vpiRealParam::vpi_handle(పూర్ణాంకం code)
{
      ఎంపిక (code) {
          స్థితి vpiScope:
            తిరిగి scope_;

	  స్థితి vpiModule:
	    తిరిగి vpip_module(scope_);

          అప్రమేయ:
            తిరిగి 0;
      }
}


vpiHandle vpip_make_real_param(స్థిరరాశి అక్షరం*name, ద్విగుణ value,
                               తార్కిక local_flag, దీర్ఘ file_idx, దీర్ఘ lineno)
{
      నిర్మాణం __vpiRealParam*obj = కొత్త __vpiRealParam(vpip_peek_current_scope(),
                                                     value, name, local_flag,
                                                     file_idx, lineno);
      తిరిగి obj;
}

/*
 * Make a VPI null constant
 */
అంతఃవరుస __vpiNullConst::__vpiNullConst()
{ }

పూర్ణాంకం __vpiNullConst::get_type_సంకేతం(శూన్య) స్థిరరాశి
{ తిరిగి vpiConstant; }

పూర్ణాంకం __vpiNullConst::vpi_get(పూర్ణాంకం code)
{
      ఎంపిక (code) {
	  స్థితి vpi_పంక్తి_No:
	    తిరిగి 0;  // Not implemented for now!

	  స్థితి vpi_కొలత:
	    తిరిగి 32;

	  స్థితి vpiConstType:
	    తిరిగి vpiNullConst;

	  స్థితి vpiSigned:
	    తిరిగి 0;

          స్థితి vpiAutomatic:
	    తిరిగి 0;

#ఒకవేళ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          స్థితి _vpiFromThr:
	      తిరిగి _vpiNoThr;
#అయితే_ముగింపు

	  అప్రమేయ:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiNullConst\n", code);
	    ప్రతిజ్ఞా(0);
	    తిరిగి 0;
      }
}

శూన్య __vpiNullConst::vpi_get_విలువ(p_vpi_విలువ val)
{
      స్థిర స్థిరరాశి size_t RBUF_USE_కొలత = 64 + 1;
      అక్షరం*rbuf = static_cast<అక్షరం *>(need_ఫలితం_buf(RBUF_USE_కొలత, RBUF_VAL));

      ఎంపిక (val->format) {

	  స్థితి vpiObjTypeVal:
	    val->format = vpiStringVal;
	    // fall through
	  స్థితి vpiBinStrVal:
	  స్థితి vpiDecStrVal:
	  స్థితి vpiOctStrVal:
	  స్థితి vpiHexStrVal:
	  స్థితి vpiStringVal:
	    snprintf(rbuf, RBUF_USE_కొలత, "null");
	    val->value.str = rbuf;
	    ఆపు;

          స్థితి vpiScalarVal:
	    val->value.scalar = vpi0;
	    ఆపు;

	  స్థితి vpiIntVal:
	    val->value.integer = 0;
	    ఆపు;

	  స్థితి vpi_సదిశ_Val:
	    val->value.vector = static_cast<p_vpi_vecval>
	                        (need_ఫలితం_buf(2*పరిమాణం(s_vpi_vecval),
	                                         RBUF_VAL));
	    కొరకు (గుర్తులేని idx = 0; idx < 2; idx += 1) {
		  val->value.vector[idx].aval = 0;
		  val->value.vector[idx].bval = 0;
	    }
	    ఆపు;

	  స్థితి vpiRealVal:
	    val->value.real = 0.0;
	    ఆపు;

	  అప్రమేయ:
	    fprintf(stderr, "vvp error: format %d not supported "
		    "by vpiNullConst\n", (పూర్ణాంకం)val->format);
	    val->format = vpiSuppressVal;
	    ఆపు;
      }
}

vpiHandle vpip_make_null_const()
{
      __vpiNullConst*obj = కొత్త __vpiNullConst;
      తిరిగి obj;
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
శూన్య constant_delete(vpiHandle అంశం)
{
      ప్రతిజ్ఞా(అంశం->get_type_సంకేతం() == vpiConstant);
      ఎంపిక(vpi_get(vpiConstType, అంశం)) {
	  స్థితి vpiStringConst:
	    తొలగించు dynamic_cast<__vpiStringConst*>(అంశం);
	    ఆపు;
	  స్థితి vpiDecConst:
	    తొలగించు dynamic_cast<__vpiDecConst*>(అంశం);
	    ఆపు;
	  స్థితి vpi_ద్విమాన_Const:
	    తొలగించు dynamic_cast<__vpiBinaryConst*>(అంశం);
	    ఆపు;
	  స్థితి vpiRealConst:
	    తొలగించు dynamic_cast<__vpiRealConst*>(అంశం);
	    ఆపు;
	  స్థితి vpiNullConst:
	    తొలగించు dynamic_cast<__vpiNullConst*>(అంశం);
	    ఆపు;
	  అప్రమేయ:
	    ప్రతిజ్ఞా(0);
      }
}

శూన్య parameter_delete(vpiHandle అంశం)
{
      ఎంపిక(vpi_get(vpiConstType, అంశం)) {
	  స్థితి vpiStringConst: {
	    తరగతి __vpiStringParam*rfp = dynamic_cast<__vpiStringParam*>(అంశం);
	    తొలగించు rfp;
	    ఆపు; }
	  స్థితి vpi_ద్విమాన_Const: {
	    నిర్మాణం __vpiBinaryParam*rfp = dynamic_cast<__vpiBinaryParam*>(అంశం);
	    తొలగించు rfp;
	    ఆపు; }
	  స్థితి vpiRealConst: {
	    నిర్మాణం __vpiRealParam*rfp = dynamic_cast<__vpiRealParam*>(అంశం);
	    తొలగించు rfp;
	    ఆపు; }
	  అప్రమేయ:
	    ప్రతిజ్ఞా(0);
      }
}
#అయితే_ముగింపు
