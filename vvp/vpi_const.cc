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
#ifdef CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#endif
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <climits>
# include  <cassert>
# include  "ivl_alloc.h"

વર્ગ __vpiStringConst : જાહેર __vpiHandle {
    જાહેર:
      explicit __vpiStringConst(અક્ષર*val);
      ~__vpiStringConst() override;
      પૂર્ણાંક get_type_code(શૂન્ય) અચલ override;
      પૂર્ણાંક vpi_get(પૂર્ણાંક code) override;
      શૂન્ય vpi_get_કિંમત(p_vpi_કિંમત val) override;

    ખાનગી:
      શૂન્ય process_string_();
    ખાનગી:
      અક્ષર*કિંમત_2;
      size_t કિંમત_len;
};

અંતઃરેખ __vpiStringConst::__vpiStringConst(અક્ષર*v)
: કિંમત_2(v)
{
      process_string_();
}

/*
 * Strings are described at the level of the vvp source as a string
 * with literal characters or octal escapes. No other escapes are
 * included, they are processed already by the compiler that generated
 * the vvp source.
 */
શૂન્ય __vpiStringConst::process_string_(શૂન્ય)
{
      અક્ષર*chr = કિંમત_2;
      અક્ષર*dp = કિંમત_2;

      જ્યારે (*chr) {
	    અક્ષર next_char = *chr;

	      /* Process octal escapes that I might find. */
	    જો (*chr == '\\') {
		  માટે (પૂર્ણાંક idx = 1 ;  idx <= 3 ;  idx += 1) {
			પ્રતિજ્ઞા(chr[idx] != 0);
			પ્રતિજ્ઞા(chr[idx] < '8');
			પ્રતિજ્ઞા(chr[idx] >= '0');
			next_char = next_char*8 + chr[idx] - '0';
		  }
		  chr += 3;
	    }
	    *dp++ = next_char;
	    chr += 1;
      }
      *dp = 0;
      કિંમત_len = dp - કિંમત_2;
}

__vpiStringConst::~__vpiStringConst()
{
      delete[] કિંમત_2;
}

પૂર્ણાંક __vpiStringConst::get_type_code(શૂન્ય) અચલ
{ પાછા vpiConstant; }

પૂર્ણાંક __vpiStringConst::vpi_get(પૂર્ણાંક code)
{
      પસંદગી (code) {
          સ્થિતિ vpiSize:
	    પાછા કિંમત_len * 8;

          સ્થિતિ vpiSigned:
	      પાછા 0;

	  સ્થિતિ vpiConstType:
	      પાછા vpiStringConst;

          સ્થિતિ vpiAutomatic:
	      પાછા 0;

#જો defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          સ્થિતિ _vpiFromThr:
	      પાછા _vpiNoThr;
#endif

	  મૂળભૂત:
	      fprintf(stderr, "vvp error: get %d not supported "
		      "by vpiStringConst\n", code);
	      પ્રતિજ્ઞા(0);
	      પાછા 0;
      }
}

શૂન્ય __vpiStringConst::vpi_get_કિંમત(p_vpi_કિંમત vp)
{
      unsigned uint_કિંમત;
      p_vpi_vecval vecp;
      unsigned size = કિંમત_len;
      અક્ષર*rbuf = 0;
      અક્ષર*cp;

      પસંદગી (vp->format) {
	  સ્થિતિ vpiObjTypeVal:
	      /* String parameters by default have vpiStringVal values. */
	    vp->format = vpiStringVal;
	    // fallthrough
	  સ્થિતિ vpiStringVal:
	    cp = કિંમત_2;
	    rbuf = static_cast<અક્ષર *>(need_result_buf(size + 1, RBUF_VAL));
	    vp->value.str = rbuf;

	    માટે (unsigned પૂર્ણાંક i = 0; i < size; i++) {
		  // Ignore leading null-bytes and replace other null-bytes with space.
		  // The LRM is not entirely clear on how null bytes should be handled.
		  // This is the implementation chosen for iverilog.
		  જો (*cp)
			*rbuf++ = *cp;
		  નહીંતર જો (rbuf != vp->value.str)
			*rbuf++ = ' ';

		  cp++;
	    }
	    *rbuf = '\0';
	    તોડો;

          સ્થિતિ vpiDecStrVal:
	    // Take the (up to 4) characters of the text, convert the characters
	    // to a numerical value, and convert that value to a decimal string.
	    // For example, the string "A" is ASCII 65, so the resulting string
	    // will be "65". and the string "AB" is 65*256 + 66 == 16706, so
	    // the resulting string is "16706". The "size" is the number of
	    // characters of input text to put to work.
	    જો (size > 4){
		  // We only support standard integers. Ignore other bytes...
		  size = 4;
		  fprintf(stderr, "Warning (vpi_const.cc): %%d on constant strings only looks "
			  "at first 4 bytes!\n");
	    }
	    સ્થિર અચલ size_t RBUF_USE_SIZE = 12;
	    rbuf = static_cast<અક્ષર *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));
	    uint_કિંમત = 0;
	    માટે(unsigned i=0; i<size; i += 1){
		  uint_કિંમત <<=8;
		  uint_કિંમત += (unsigned અક્ષર)(કિંમત_2[i]);
	    }
	    snprintf(rbuf, RBUF_USE_SIZE, "%u", uint_કિંમત);
	    vp->value.str = rbuf;
	    તોડો;

          સ્થિતિ vpiBinStrVal:
	      rbuf = static_cast<અક્ષર *>(need_result_buf(8 * size + 1, RBUF_VAL));
	      cp = rbuf;
	      માટે(unsigned i=0; i<size; i += 1){
		  માટે(પૂર્ણાંક bit=7; bit>=0; bit -= 1){
		      *cp++ = "01"[ (કિંમત_2[i]>>bit)&1 ];
		  }
	      }
	      *cp = 0;
	      vp->value.str = rbuf;
	      તોડો;

          સ્થિતિ vpiHexStrVal:
	      rbuf = static_cast<અક્ષર *>(need_result_buf(2 * size + 1, RBUF_VAL));
	      cp = rbuf;
	      માટે(unsigned i=0; i<size; i += 1){
		  માટે(પૂર્ણાંક nibble=1; nibble>=0; nibble -= 1){
		      *cp++ = "0123456789abcdef"[ (કિંમત_2[i]>>(nibble*4))&15 ];
		  }
	      }
	      *cp = 0;
	      vp->value.str = rbuf;
	      તોડો;

          સ્થિતિ vpiOctStrVal:
	      fprintf(stderr, "ERROR (vpi_const.cc): %%o display of constant strings not yet implemented\n");
	      પ્રતિજ્ઞા(0);
	      તોડો;

          સ્થિતિ vpiIntVal:
	      vp->value.integer = 0;
	      માટે(unsigned i=0; i<size; i += 1){
		  માટે(પૂર્ણાંક bit=7; bit>=0; bit -= 1){
		      vp->value.integer <<= 1;
		      vp->value.integer += (કિંમત_2[i]>>bit)&1;
		  }
	      }
	      તોડો;

          સ્થિતિ vpiVectorVal:
              vp->value.vector = static_cast<p_vpi_vecval>
                                 (need_result_buf((size+3)/4*
                                                   કદ(s_vpi_vecval),
                                                  RBUF_VAL));
              uint_કિંમત = 0;
              vecp = vp->value.vector;
              vecp->aval = vecp->bval = 0;
	      માટે(unsigned i=0; i<size; i += 1){
		  vecp->aval |= કિંમત_2[i] << uint_કિંમત*8;
		  uint_કિંમત += 1;
		  જો (uint_કિંમત > 3) {
		      uint_કિંમત = 0;
		      vecp += 1;
		      vecp->aval = vecp->bval = 0;
		  }
	      }
	      તોડો;


	  મૂળભૂત:
	    fprintf(stderr, "ERROR (vpi_const.cc): vp->format: %d\n",
	            (પૂર્ણાંક)vp->format);
	    પ્રતિજ્ઞા(0);

	    vp->format = vpiSuppressVal;
	    તોડો;
      }
}


સંરચના __vpiStringConstTEMP : જાહેર __vpiStringConst {
      explicit અંતઃરેખ __vpiStringConstTEMP(અક્ષર*v) : __vpiStringConst(v) { }
      free_object_fun_t free_object_fun(શૂન્ય) override;
};

સ્થિર પૂર્ણાંક free_temp_string(vpiHandle obj)
{
      સંરચના __vpiStringConstTEMP*rfp = dynamic_cast<__vpiStringConstTEMP*>(obj);
      delete rfp;
      પાછા 1;
}

__vpiHandle::free_object_fun_t __vpiStringConstTEMP::free_object_fun(શૂન્ય)
{ પાછા &free_temp_string; }

vpiHandle vpip_make_string_const(અક્ષર*text, તાર્કિક persistent_flag)
{
      __vpiStringConst*obj;

      obj = persistent_flag? નવું __vpiStringConst(text) : નવું __vpiStringConstTEMP(text);

      પાછા obj;
}


વર્ગ __vpiStringParam  : જાહેર __vpiStringConst {
    જાહેર:
      __vpiStringParam(__vpiScope* scope, અક્ષર*txt, અચલ અક્ષર*name,
                       તાર્કિક local_flag, long file_idx, long lineno);
      ~__vpiStringParam() override;
      પૂર્ણાંક get_type_code(શૂન્ય) અચલ override;
      પૂર્ણાંક vpi_get(પૂર્ણાંક code) override;
      અક્ષર*vpi_get_str(પૂર્ણાંક code) override;
      vpiHandle vpi_handle(પૂર્ણાંક code) override;

    ખાનગી:
      __vpiScope* scope_;
      અચલ અક્ષર*basename_;
      તાર્કિક       local_flag_;
      unsigned   file_idx_;
      unsigned   lineno_;
};

અંતઃરેખ __vpiStringParam::__vpiStringParam(__vpiScope* scope, અક્ષર*txt, અચલ અક્ષર*nam,
                                          તાર્કિક local_flag, long file_idx, long lineno)
: __vpiStringConst(txt)
{
      scope_ = scope;
      basename_ = nam;
      local_flag_ = local_flag;
      પ્રતિજ્ઞા(file_idx >= 0 && file_idx <= INT_MAX);
      file_idx_ = (unsigned) file_idx;
      પ્રતિજ્ઞા(lineno >= 0 && lineno <= INT_MAX);
      lineno_ = (unsigned) lineno;
}

__vpiStringParam::~__vpiStringParam()
{
      delete[]basename_;
}

પૂર્ણાંક __vpiStringParam::get_type_code(શૂન્ય) અચલ
{ પાછા vpiParameter; }

પૂર્ણાંક __vpiStringParam::vpi_get(પૂર્ણાંક code)
{
    પસંદગી (code) {
       સ્થિતિ vpi_લીટી_No :
         પાછા lineno_;

       સ્થિતિ vpiLocalParam :
         પાછા local_flag_;

       મૂળભૂત :
         પાછા __vpiStringConst::vpi_get(code);
    }
}


અક્ષર*__vpiStringParam::vpi_get_str(પૂર્ણાંક code)
{
      જો (code == vpiFile) {
	    પાછા simple_set_rbuf_str(file_names[file_idx_]);
      }

      પાછા generic_get_str(code, scope_, basename_, ખાલી);
}


vpiHandle __vpiStringParam::vpi_handle(પૂર્ણાંક code)
{
      પસંદગી (code) {
	  સ્થિતિ vpiScope:
	    પાછા scope_;

	  સ્થિતિ vpiModule:
	    પાછા vpip_module(scope_);

	  મૂળભૂત:
	    પાછા 0;
      }
}

vpiHandle vpip_make_string_param(અચલ અક્ષર*name, અક્ષર*text,
                                 તાર્કિક local_flag, long file_idx, long lineno)
{
      __vpiStringParam*obj = નવું __vpiStringParam(vpip_peek_current_scope(), text, name,
                                                  local_flag, file_idx, lineno);

      પાછા obj;
}



અંતઃરેખ __vpiBinaryConst::__vpiBinaryConst()
: signed_flag(0), sized_flag(0)
{ }

પૂર્ણાંક __vpiBinaryConst::get_type_code(શૂન્ય) અચલ
{ પાછા vpiConstant; }

પૂર્ણાંક __vpiBinaryConst::vpi_get(પૂર્ણાંક code)
{
      પસંદગી (code) {
	  સ્થિતિ vpiConstType:
	    પાછા vpi_દ્વિઅંકી_Const;

	  સ્થિતિ vpi_લીટી_No:
	    પાછા 0;  // Not implemented for now!

	  સ્થિતિ vpiSigned:
	    પાછા signed_flag? 1 : 0;

	  સ્થિતિ vpiSize:
	    પાછા bits.size();

          સ્થિતિ vpiAutomatic:
	    પાછા 0;

#જો defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          સ્થિતિ _vpiFromThr:
	      પાછા _vpiNoThr;
#endif

	  મૂળભૂત:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiBinaryConst\n", code);
	    પ્રતિજ્ઞા(0);
	    પાછા 0;
      }
}


શૂન્ય __vpiBinaryConst::vpi_get_કિંમત(p_vpi_કિંમત val)
{
      પસંદગી (val->format) {

	  સ્થિતિ vpiObjTypeVal:
	  સ્થિતિ vpiBinStrVal:
	  સ્થિતિ vpiDecStrVal:
	  સ્થિતિ vpiOctStrVal:
	  સ્થિતિ vpiHexStrVal:
          સ્થિતિ vpiScalarVal:
	  સ્થિતિ vpiIntVal:
	  સ્થિતિ vpiVectorVal:
	  સ્થિતિ vpiStringVal:
	  સ્થિતિ vpiRealVal:
	    vpip_vec4_get_કિંમત(bits, bits.size(), signed_flag, val);
	    તોડો;

	  મૂળભૂત:
	    fprintf(stderr, "vvp error: format %d not supported "
		    "by vpiBinaryConst\n", (પૂર્ણાંક)val->format);
	    val->format = vpiSuppressVal;
	    તોડો;
      }
}

/*
 * Make a VPI constant from a vector string. The string is normally a
 * ASCII string, with each letter a 4-value bit. The first character
 * may be an 's' if the vector is signed.
 */
vpiHandle vpip_make_દ્વિઅંકી_const(unsigned wid, અચલ અક્ષર*bits)
{
      સંરચના __vpiBinaryConst*obj = નવું __vpiBinaryConst;

      અચલ અક્ષર*bp = bits;
      જો (*bp == 's') {
	    bp += 1;
	    obj->signed_flag = 1;
      }

      obj->bits = vector4_from_text(bp, wid);

      પાછા obj;
}

vvp_vector4_t vector4_from_text(અચલ અક્ષર*bits, unsigned wid)
{
      vvp_vector4_t res (wid);

      માટે (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t val = BIT4_0;
	    પસંદગી (bits[wid-idx-1]) {
		સ્થિતિ '0':
		  val = BIT4_0;
		  તોડો;
		સ્થિતિ '1':
		  val = BIT4_1;
		  તોડો;
		સ્થિતિ 'x':
		  val = BIT4_X;
		  તોડો;
		સ્થિતિ 'z':
		  val = BIT4_Z;
		  તોડો;
	    }

	    res.set_bit(idx, val);
      }

      પાછા res;
}

સંરચના __vpiBinaryParam  : જાહેર __vpiBinaryConst {
      __vpiBinaryParam(__vpiScope*scope, અચલ vvp_vector4_t&b, તાર્કિક is_signed,
                       અચલ અક્ષર*name, તાર્કિક local_flag, long file_idx, long lineno);
      ~__vpiBinaryParam() override;
      પૂર્ણાંક get_type_code(શૂન્ય) અચલ override;
      પૂર્ણાંક vpi_get(પૂર્ણાંક code) override;
      અક્ષર*vpi_get_str(પૂર્ણાંક code) override;
      vpiHandle vpi_handle(પૂર્ણાંક code) override;

    ખાનગી:
      __vpiScope*scope_;
      અચલ અક્ષર*basename_;
      તાર્કિક     local_flag_;
      unsigned file_idx_;
      unsigned lineno_;
};

અંતઃરેખ __vpiBinaryParam::__vpiBinaryParam(__vpiScope*scope, અચલ vvp_vector4_t&b,
                                          તાર્કિક is_signed, અચલ અક્ષર*nam,
                                          તાર્કિક local_flag, long file_idx, long lineno)
{
      scope_ = scope;
      bits = b;
      signed_flag = is_signed ? 1 : 0;
      sized_flag = 0;
      basename_ = nam;
      local_flag_ = local_flag;
      પ્રતિજ્ઞા(file_idx >= 0 && file_idx <= INT_MAX);
      file_idx_ = (unsigned) file_idx;
      પ્રતિજ્ઞા(lineno >= 0 && lineno <= INT_MAX);
      lineno_ = (unsigned) lineno;
}

__vpiBinaryParam::~__vpiBinaryParam()
{
      delete[]basename_;
}

પૂર્ણાંક __vpiBinaryParam::get_type_code(શૂન્ય) અચલ
{ પાછા vpiParameter; }

પૂર્ણાંક __vpiBinaryParam::vpi_get(પૂર્ણાંક code)
{
    પસંદગી (code) {
      સ્થિતિ vpi_લીટી_No :
        પાછા lineno_;

      સ્થિતિ vpiLocalParam :
        પાછા local_flag_;

      મૂળભૂત :
        પાછા __vpiBinaryConst::vpi_get(code);
    }
}

અક્ષર*__vpiBinaryParam::vpi_get_str(પૂર્ણાંક code)
{
      જો (code == vpiFile)
	    પાછા simple_set_rbuf_str(file_names[file_idx_]);

      પાછા generic_get_str(code, scope_, basename_, ખાલી);
}


vpiHandle __vpiBinaryParam::vpi_handle(પૂર્ણાંક code)
{
      પસંદગી (code) {
	  સ્થિતિ vpiScope:
	    પાછા scope_;

	  સ્થિતિ vpiModule:
	    પાછા vpip_module(scope_);

	  મૂળભૂત:
	    પાછા 0;
      }
}


vpiHandle vpip_make_દ્વિઅંકી_param(અચલ અક્ષર*name, અચલ vvp_vector4_t&bits,
				 તાર્કિક signed_flag, તાર્કિક local_flag,
				 long file_idx, long lineno)
{
      સંરચના __vpiBinaryParam*obj = નવું __vpiBinaryParam(vpip_peek_current_scope(),
                                                         bits, signed_flag, name,
                                                         local_flag, file_idx, lineno);

      પાછા obj;
}



__vpiDecConst::__vpiDecConst(પૂર્ણાંક val)
{
      value = val;
}

__vpiDecConst::__vpiDecConst(અચલ __vpiDecConst&that)
: __vpiHandle(), value(that.value)
{
}


પૂર્ણાંક __vpiDecConst::get_type_code(શૂન્ય) અચલ
{ પાછા vpiConstant; }

પૂર્ણાંક __vpiDecConst::vpi_get(પૂર્ણાંક code)
{
      પસંદગી (code) {
	  સ્થિતિ vpiConstType:
	    પાછા vpiDecConst;

	  સ્થિતિ vpiSigned:
	    પાછા 1;

	  સ્થિતિ vpiSize:
	    પાછા 32;

          સ્થિતિ vpiAutomatic:
	    પાછા 0;

#જો defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          સ્થિતિ _vpiFromThr:
	      પાછા _vpiNoThr;
#endif

	  મૂળભૂત:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiDecConst\n", code);
	    પ્રતિજ્ઞા(0);
	    પાછા 0;
      }
}


શૂન્ય __vpiDecConst::vpi_get_કિંમત(p_vpi_કિંમત vp)
{
      સ્થિર અચલ size_t RBUF_USE_SIZE = 64 + 1;
      અક્ષર*rbuf = static_cast<અક્ષર *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));
      અક્ષર*cp = rbuf;

      પસંદગી (vp->format) {

	  સ્થિતિ vpiObjTypeVal:
	  સ્થિતિ vpiIntVal: {
		vp->value.integer = value;
		તોડો;
	  }

          સ્થિતિ vpiDecStrVal:
	      snprintf(rbuf, RBUF_USE_SIZE, "%d", value);
	      vp->value.str = rbuf;
	      તોડો;

          સ્થિતિ vpiBinStrVal:
	      માટે(પૂર્ણાંક bit=31; bit>=0;bit--){
		  *cp++ = "01"[ (static_cast<unsigned>(value)>>bit)&1 ];
	      }
	      *cp = 0;

	      vp->value.str = rbuf;
	      તોડો;

          સ્થિતિ vpiHexStrVal:
	      snprintf(rbuf, RBUF_USE_SIZE, "%08x", value);
	      vp->value.str = rbuf;
	      તોડો;

          સ્થિતિ vpiOctStrVal:
	      snprintf(rbuf, RBUF_USE_SIZE, "%011x", value);
	      vp->value.str = rbuf;
	      તોડો;

	  મૂળભૂત:
	    fprintf(stderr, "vvp error (vpi_const.cc): format %d not supported "
		    "by vpiDecConst\n", (પૂર્ણાંક)vp->format);
	    vp->format = vpiSuppressVal;
	    તોડો;
      }
}


અંતઃરેખ __vpiRealConst::__vpiRealConst(દ્વિગુણ val)
: value(val)
{ }

પૂર્ણાંક __vpiRealConst::get_type_code(શૂન્ય) અચલ
{ પાછા vpiConstant; }

પૂર્ણાંક __vpiRealConst::vpi_get(પૂર્ણાંક code)
{
      પસંદગી (code) {
	  સ્થિતિ vpi_લીટી_No:
	    પાછા 0;  // Not implemented for now!

	  સ્થિતિ vpiSize:
	    પાછા 1;

	  સ્થિતિ vpiConstType:
	    પાછા vpiRealConst;

	  સ્થિતિ vpiSigned:
	    પાછા 1;

          સ્થિતિ vpiAutomatic:
	    પાછા 0;

#જો defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          સ્થિતિ _vpiFromThr:
	      પાછા _vpiNoThr;
#endif

	  મૂળભૂત:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiRealConst\n", code);
	    પ્રતિજ્ઞા(0);
	    પાછા 0;
      }
}


શૂન્ય __vpiRealConst::vpi_get_કિંમત(p_vpi_કિંમત val)
{
      vpip_real_get_કિંમત(value, val);
}


vpiHandle vpip_make_real_const(દ્વિગુણ value)
{
      __vpiRealConst*obj = નવું __vpiRealConst(value);
      પાછા obj;
}

સંરચના __vpiRealParam  : જાહેર __vpiRealConst {
      __vpiRealParam(__vpiScope* scope, દ્વિગુણ val, અચલ અક્ષર*name,
                     તાર્કિક local_flag, long file_idx, long lineno);
      ~__vpiRealParam() override;
      પૂર્ણાંક get_type_code(શૂન્ય) અચલ override;
      પૂર્ણાંક vpi_get(પૂર્ણાંક code) override;
      અક્ષર*vpi_get_str(પૂર્ણાંક code) override;
      vpiHandle vpi_handle(પૂર્ણાંક code) override;

    ખાનગી:
      __vpiScope* scope_;
      અચલ અક્ષર*basename_;
      તાર્કિક local_flag_;
      unsigned file_idx_;
      unsigned lineno_;
};


અંતઃરેખ __vpiRealParam::__vpiRealParam(__vpiScope* scope, દ્વિગુણ val, અચલ અક્ષર*name,
                                      તાર્કિક local_flag, long file_idx, long lineno)
: __vpiRealConst(val)
{
      scope_ = scope;
      basename_ = name;
      local_flag_ = local_flag;
      પ્રતિજ્ઞા(file_idx >= 0 && file_idx <= INT_MAX);
      file_idx_ = (unsigned) file_idx;
      પ્રતિજ્ઞા(lineno >= 0 && lineno <= INT_MAX);
      lineno_ = (unsigned) lineno;
}

__vpiRealParam::~__vpiRealParam()
{
      delete[]basename_;
}


પૂર્ણાંક __vpiRealParam::get_type_code(શૂન્ય) અચલ
{ પાછા vpiParameter; }

પૂર્ણાંક __vpiRealParam::vpi_get(પૂર્ણાંક code)
{
    પસંદગી (code) {
      સ્થિતિ vpi_લીટી_No :
        પાછા lineno_;

      સ્થિતિ vpiLocalParam :
        પાછા local_flag_;

      મૂળભૂત :
           પાછા __vpiRealConst::vpi_get(code);
    }
}

અક્ષર* __vpiRealParam::vpi_get_str(પૂર્ણાંક code)
{
      જો (code == vpiFile)
            પાછા simple_set_rbuf_str(file_names[file_idx_]);

      પાછા generic_get_str(code, scope_, basename_, ખાલી);
}

vpiHandle __vpiRealParam::vpi_handle(પૂર્ણાંક code)
{
      પસંદગી (code) {
          સ્થિતિ vpiScope:
            પાછા scope_;

	  સ્થિતિ vpiModule:
	    પાછા vpip_module(scope_);

          મૂળભૂત:
            પાછા 0;
      }
}


vpiHandle vpip_make_real_param(અચલ અક્ષર*name, દ્વિગુણ value,
                               તાર્કિક local_flag, long file_idx, long lineno)
{
      સંરચના __vpiRealParam*obj = નવું __vpiRealParam(vpip_peek_current_scope(),
                                                     value, name, local_flag,
                                                     file_idx, lineno);
      પાછા obj;
}

/*
 * Make a VPI null constant
 */
અંતઃરેખ __vpiNullConst::__vpiNullConst()
{ }

પૂર્ણાંક __vpiNullConst::get_type_code(શૂન્ય) અચલ
{ પાછા vpiConstant; }

પૂર્ણાંક __vpiNullConst::vpi_get(પૂર્ણાંક code)
{
      પસંદગી (code) {
	  સ્થિતિ vpi_લીટી_No:
	    પાછા 0;  // Not implemented for now!

	  સ્થિતિ vpiSize:
	    પાછા 32;

	  સ્થિતિ vpiConstType:
	    પાછા vpiNullConst;

	  સ્થિતિ vpiSigned:
	    પાછા 0;

          સ્થિતિ vpiAutomatic:
	    પાછા 0;

#જો defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          સ્થિતિ _vpiFromThr:
	      પાછા _vpiNoThr;
#endif

	  મૂળભૂત:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by vpiNullConst\n", code);
	    પ્રતિજ્ઞા(0);
	    પાછા 0;
      }
}

શૂન્ય __vpiNullConst::vpi_get_કિંમત(p_vpi_કિંમત val)
{
      સ્થિર અચલ size_t RBUF_USE_SIZE = 64 + 1;
      અક્ષર*rbuf = static_cast<અક્ષર *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));

      પસંદગી (val->format) {

	  સ્થિતિ vpiObjTypeVal:
	    val->format = vpiStringVal;
	    // fall through
	  સ્થિતિ vpiBinStrVal:
	  સ્થિતિ vpiDecStrVal:
	  સ્થિતિ vpiOctStrVal:
	  સ્થિતિ vpiHexStrVal:
	  સ્થિતિ vpiStringVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "null");
	    val->value.str = rbuf;
	    તોડો;

          સ્થિતિ vpiScalarVal:
	    val->value.scalar = vpi0;
	    તોડો;

	  સ્થિતિ vpiIntVal:
	    val->value.integer = 0;
	    તોડો;

	  સ્થિતિ vpiVectorVal:
	    val->value.vector = static_cast<p_vpi_vecval>
	                        (need_result_buf(2*કદ(s_vpi_vecval),
	                                         RBUF_VAL));
	    માટે (unsigned idx = 0; idx < 2; idx += 1) {
		  val->value.vector[idx].aval = 0;
		  val->value.vector[idx].bval = 0;
	    }
	    તોડો;

	  સ્થિતિ vpiRealVal:
	    val->value.real = 0.0;
	    તોડો;

	  મૂળભૂત:
	    fprintf(stderr, "vvp error: format %d not supported "
		    "by vpiNullConst\n", (પૂર્ણાંક)val->format);
	    val->format = vpiSuppressVal;
	    તોડો;
      }
}

vpiHandle vpip_make_null_const()
{
      __vpiNullConst*obj = નવું __vpiNullConst;
      પાછા obj;
}

#ifdef CHECK_WITH_VALGRIND
શૂન્ય constant_delete(vpiHandle item)
{
      પ્રતિજ્ઞા(item->get_type_code() == vpiConstant);
      પસંદગી(vpi_get(vpiConstType, item)) {
	  સ્થિતિ vpiStringConst:
	    delete dynamic_cast<__vpiStringConst*>(item);
	    તોડો;
	  સ્થિતિ vpiDecConst:
	    delete dynamic_cast<__vpiDecConst*>(item);
	    તોડો;
	  સ્થિતિ vpi_દ્વિઅંકી_Const:
	    delete dynamic_cast<__vpiBinaryConst*>(item);
	    તોડો;
	  સ્થિતિ vpiRealConst:
	    delete dynamic_cast<__vpiRealConst*>(item);
	    તોડો;
	  સ્થિતિ vpiNullConst:
	    delete dynamic_cast<__vpiNullConst*>(item);
	    તોડો;
	  મૂળભૂત:
	    પ્રતિજ્ઞા(0);
      }
}

શૂન્ય parameter_delete(vpiHandle item)
{
      પસંદગી(vpi_get(vpiConstType, item)) {
	  સ્થિતિ vpiStringConst: {
	    વર્ગ __vpiStringParam*rfp = dynamic_cast<__vpiStringParam*>(item);
	    delete rfp;
	    તોડો; }
	  સ્થિતિ vpi_દ્વિઅંકી_Const: {
	    સંરચના __vpiBinaryParam*rfp = dynamic_cast<__vpiBinaryParam*>(item);
	    delete rfp;
	    તોડો; }
	  સ્થિતિ vpiRealConst: {
	    સંરચના __vpiRealParam*rfp = dynamic_cast<__vpiRealParam*>(item);
	    delete rfp;
	    તોડો; }
	  મૂળભૂત:
	    પ્રતિજ્ઞા(0);
      }
}
#endif
