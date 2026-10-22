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
#अगर_घोषित CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#पूर्ण_अगर
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <climits>
# include  <cassert>
# include  "ivl_alloc.h"

श्रेणी __vpiStringConst : खुला __vpiHandle {
    खुला:
      explicit __vpiStringConst(अक्षर*val);
      ~__vpiStringConst() override;
      पूर्णांक get_type_code(व्योम) स्थिर override;
      पूर्णांक vpi_get(पूर्णांक code) override;
      व्योम vpi_get_मूल्य(p_vpi_मूल्य val) override;

    निजी:
      व्योम process_string_();
    निजी:
      अक्षर*मूल्य_2;
      माप_प्रकार मूल्य_len;
};

अंतरभूत __vpiStringConst::__vpiStringConst(अक्षर*v)
: मूल्य_2(v)
{
      process_string_();
}

/*
 * Strings are described at the level of the vvp source as a string
 * with literal characters or octal escapes. No other escapes are
 * included, they are processed already by the compiler that generated
 * the vvp source.
 */
व्योम __vpiStringConst::process_string_(व्योम)
{
      अक्षर*chr = मूल्य_2;
      अक्षर*dp = मूल्य_2;

      जबतक (*chr) {
	    अक्षर अग्र_char = *chr;

	      /* Process octal escapes that I might find. */
	    अगर (*chr == '\\') {
		  क्रम (पूर्णांक idx = 1 ;  idx <= 3 ;  idx += 1) {
			निश्चित(chr[idx] != 0);
			निश्चित(chr[idx] < '8');
			निश्चित(chr[idx] >= '0');
			अग्र_char = अग्र_char*8 + chr[idx] - '0';
		  }
		  chr += 3;
	    }
	    *dp++ = अग्र_char;
	    chr += 1;
      }
      *dp = 0;
      मूल्य_len = dp - मूल्य_2;
}

__vpiStringConst::~__vpiStringConst()
{
      मिटाओ[] मूल्य_2;
}

पूर्णांक __vpiStringConst::get_type_code(व्योम) स्थिर
{ वापस vpiConstant; }

पूर्णांक __vpiStringConst::vpi_get(पूर्णांक code)
{
      चयन (code) {
          हाल vpi_आकार:
	    वापस मूल्य_len * 8;

          हाल vpiSigned:
	      वापस 0;

	  हाल vpiConstType:
	      वापस vpiStringConst;

          हाल vpiAutomatic:
	      वापस 0;

#अगर defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          हाल _vpiFromThr:
	      वापस _vpiNoThr;
#पूर्ण_अगर

	  शेष:
	      ख_लिखो(मानक_त्रुटि, "vvp error: get %d not supported "
		      "by vpiStringConst\n", code);
	      निश्चित(0);
	      वापस 0;
      }
}

व्योम __vpiStringConst::vpi_get_मूल्य(p_vpi_मूल्य vp)
{
      अचिन्हित uint_मूल्य;
      p_vpi_vecval vecp;
      अचिन्हित size = मूल्य_len;
      अक्षर*rbuf = 0;
      अक्षर*cp;

      चयन (vp->format) {
	  हाल vpiObjTypeVal:
	      /* String parameters by default have vpiStringVal values. */
	    vp->format = vpiStringVal;
	    // fallthrough
	  हाल vpiStringVal:
	    cp = मूल्य_2;
	    rbuf = static_cast<अक्षर *>(need_परिणाम_buf(size + 1, RBUF_VAL));
	    vp->value.str = rbuf;

	    क्रम (अचिन्हित पूर्णांक i = 0; i < size; i++) {
		  // Ignore leading null-bytes and replace other null-bytes with space.
		  // The LRM is not entirely clear on how null bytes should be handled.
		  // This is the implementation chosen for iverilog.
		  अगर (*cp)
			*rbuf++ = *cp;
		  अन्यथा अगर (rbuf != vp->value.str)
			*rbuf++ = ' ';

		  cp++;
	    }
	    *rbuf = '\0';
	    अवरोध;

          हाल vpiDecStrVal:
	    // Take the (up to 4) characters of the text, convert the characters
	    // to a numerical value, and convert that value to a decimal string.
	    // For example, the string "A" is ASCII 65, so the resulting string
	    // will be "65". and the string "AB" is 65*256 + 66 == 16706, so
	    // the resulting string is "16706". The "size" is the number of
	    // characters of input text to put to work.
	    अगर (size > 4){
		  // We only support standard integers. Ignore other bytes...
		  size = 4;
		  ख_लिखो(मानक_त्रुटि, "Warning (vpi_const.cc): %%d on constant strings only looks "
			  "at first 4 bytes!\n");
	    }
	    अटल स्थिर माप_प्रकार RBUF_USE_आकार = 12;
	    rbuf = static_cast<अक्षर *>(need_परिणाम_buf(RBUF_USE_आकार, RBUF_VAL));
	    uint_मूल्य = 0;
	    क्रम(अचिन्हित i=0; i<size; i += 1){
		  uint_मूल्य <<=8;
		  uint_मूल्य += (अचिन्हित अक्षर)(मूल्य_2[i]);
	    }
	    snprintf(rbuf, RBUF_USE_आकार, "%u", uint_मूल्य);
	    vp->value.str = rbuf;
	    अवरोध;

          हाल vpiBinStrVal:
	      rbuf = static_cast<अक्षर *>(need_परिणाम_buf(8 * size + 1, RBUF_VAL));
	      cp = rbuf;
	      क्रम(अचिन्हित i=0; i<size; i += 1){
		  क्रम(पूर्णांक bit=7; bit>=0; bit -= 1){
		      *cp++ = "01"[ (मूल्य_2[i]>>bit)&1 ];
		  }
	      }
	      *cp = 0;
	      vp->value.str = rbuf;
	      अवरोध;

          हाल vpiHexStrVal:
	      rbuf = static_cast<अक्षर *>(need_परिणाम_buf(2 * size + 1, RBUF_VAL));
	      cp = rbuf;
	      क्रम(अचिन्हित i=0; i<size; i += 1){
		  क्रम(पूर्णांक nibble=1; nibble>=0; nibble -= 1){
		      *cp++ = "0123456789abcdef"[ (मूल्य_2[i]>>(nibble*4))&15 ];
		  }
	      }
	      *cp = 0;
	      vp->value.str = rbuf;
	      अवरोध;

          हाल vpiOctStrVal:
	      ख_लिखो(मानक_त्रुटि, "ERROR (vpi_const.cc): %%o display of constant strings not yet implemented\n");
	      निश्चित(0);
	      अवरोध;

          हाल vpiIntVal:
	      vp->value.integer = 0;
	      क्रम(अचिन्हित i=0; i<size; i += 1){
		  क्रम(पूर्णांक bit=7; bit>=0; bit -= 1){
		      vp->value.integer <<= 1;
		      vp->value.integer += (मूल्य_2[i]>>bit)&1;
		  }
	      }
	      अवरोध;

          हाल vpiVectorVal:
              vp->value.vector = static_cast<p_vpi_vecval>
                                 (need_परिणाम_buf((size+3)/4*
                                                   माप(s_vpi_vecval),
                                                  RBUF_VAL));
              uint_मूल्य = 0;
              vecp = vp->value.vector;
              vecp->aval = vecp->bval = 0;
	      क्रम(अचिन्हित i=0; i<size; i += 1){
		  vecp->aval |= मूल्य_2[i] << uint_मूल्य*8;
		  uint_मूल्य += 1;
		  अगर (uint_मूल्य > 3) {
		      uint_मूल्य = 0;
		      vecp += 1;
		      vecp->aval = vecp->bval = 0;
		  }
	      }
	      अवरोध;


	  शेष:
	    ख_लिखो(मानक_त्रुटि, "ERROR (vpi_const.cc): vp->format: %d\n",
	            (पूर्णांक)vp->format);
	    निश्चित(0);

	    vp->format = vpiSuppressVal;
	    अवरोध;
      }
}


काष्ठा __vpiStringConstTEMP : खुला __vpiStringConst {
      explicit अंतरभूत __vpiStringConstTEMP(अक्षर*v) : __vpiStringConst(v) { }
      free_object_fun_t free_object_fun(व्योम) override;
};

अटल पूर्णांक free_अस्थायी_string(vpiHandle obj)
{
      काष्ठा __vpiStringConstTEMP*rfp = dynamic_cast<__vpiStringConstTEMP*>(obj);
      मिटाओ rfp;
      वापस 1;
}

__vpiHandle::free_object_fun_t __vpiStringConstTEMP::free_object_fun(व्योम)
{ वापस &free_अस्थायी_string; }

vpiHandle vpip_make_string_const(अक्षर*text, bool persistent_flag)
{
      __vpiStringConst*obj;

      obj = persistent_flag? नया __vpiStringConst(text) : नया __vpiStringConstTEMP(text);

      वापस obj;
}


श्रेणी __vpiStringParam  : खुला __vpiStringConst {
    खुला:
      __vpiStringParam(__vpiScope* scope, अक्षर*txt, स्थिर अक्षर*name,
                       bool local_flag, दीर्घ file_idx, दीर्घ lineno);
      ~__vpiStringParam() override;
      पूर्णांक get_type_code(व्योम) स्थिर override;
      पूर्णांक vpi_get(पूर्णांक code) override;
      अक्षर*vpi_get_str(पूर्णांक code) override;
      vpiHandle vpi_handle(पूर्णांक code) override;

    निजी:
      __vpiScope* scope_;
      स्थिर अक्षर*basename_;
      bool       local_flag_;
      अचिन्हित   file_idx_;
      अचिन्हित   lineno_;
};

अंतरभूत __vpiStringParam::__vpiStringParam(__vpiScope* scope, अक्षर*txt, स्थिर अक्षर*nam,
                                          bool local_flag, दीर्घ file_idx, दीर्घ lineno)
: __vpiStringConst(txt)
{
      scope_ = scope;
      basename_ = nam;
      local_flag_ = local_flag;
      निश्चित(file_idx >= 0 && file_idx <= पूर्णांक_उच्च);
      file_idx_ = (अचिन्हित) file_idx;
      निश्चित(lineno >= 0 && lineno <= पूर्णांक_उच्च);
      lineno_ = (अचिन्हित) lineno;
}

__vpiStringParam::~__vpiStringParam()
{
      मिटाओ[]basename_;
}

पूर्णांक __vpiStringParam::get_type_code(व्योम) स्थिर
{ वापस vpiParameter; }

पूर्णांक __vpiStringParam::vpi_get(पूर्णांक code)
{
    चयन (code) {
       हाल vpi_पंक्ति_No :
         वापस lineno_;

       हाल vpiLocalParam :
         वापस local_flag_;

       शेष :
         वापस __vpiStringConst::vpi_get(code);
    }
}


अक्षर*__vpiStringParam::vpi_get_str(पूर्णांक code)
{
      अगर (code == vpiFile) {
	    वापस simple_set_rbuf_str(file_names[file_idx_]);
      }

      वापस generic_get_str(code, scope_, basename_, शून्य);
}


vpiHandle __vpiStringParam::vpi_handle(पूर्णांक code)
{
      चयन (code) {
	  हाल vpiScope:
	    वापस scope_;

	  हाल vpiModule:
	    वापस vpip_module(scope_);

	  शेष:
	    वापस 0;
      }
}

vpiHandle vpip_make_string_param(स्थिर अक्षर*name, अक्षर*text,
                                 bool local_flag, दीर्घ file_idx, दीर्घ lineno)
{
      __vpiStringParam*obj = नया __vpiStringParam(vpip_peek_current_scope(), text, name,
                                                  local_flag, file_idx, lineno);

      वापस obj;
}



अंतरभूत __vpiBinaryConst::__vpiBinaryConst()
: signed_flag(0), sized_flag(0)
{ }

पूर्णांक __vpiBinaryConst::get_type_code(व्योम) स्थिर
{ वापस vpiConstant; }

पूर्णांक __vpiBinaryConst::vpi_get(पूर्णांक code)
{
      चयन (code) {
	  हाल vpiConstType:
	    वापस vpi_द्विआधारी_Const;

	  हाल vpi_पंक्ति_No:
	    वापस 0;  // Not implemented for now!

	  हाल vpiSigned:
	    वापस signed_flag? 1 : 0;

	  हाल vpi_आकार:
	    वापस bits.size();

          हाल vpiAutomatic:
	    वापस 0;

#अगर defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          हाल _vpiFromThr:
	      वापस _vpiNoThr;
#पूर्ण_अगर

	  शेष:
	    ख_लिखो(मानक_त्रुटि, "vvp error: get %d not supported "
		    "by vpiBinaryConst\n", code);
	    निश्चित(0);
	    वापस 0;
      }
}


व्योम __vpiBinaryConst::vpi_get_मूल्य(p_vpi_मूल्य val)
{
      चयन (val->format) {

	  हाल vpiObjTypeVal:
	  हाल vpiBinStrVal:
	  हाल vpiDecStrVal:
	  हाल vpiOctStrVal:
	  हाल vpiHexStrVal:
          हाल vpiScalarVal:
	  हाल vpiIntVal:
	  हाल vpiVectorVal:
	  हाल vpiStringVal:
	  हाल vpiRealVal:
	    vpip_vec4_get_मूल्य(bits, bits.size(), signed_flag, val);
	    अवरोध;

	  शेष:
	    ख_लिखो(मानक_त्रुटि, "vvp error: format %d not supported "
		    "by vpiBinaryConst\n", (पूर्णांक)val->format);
	    val->format = vpiSuppressVal;
	    अवरोध;
      }
}

/*
 * Make a VPI constant from a vector string. The string is normally a
 * ASCII string, with each letter a 4-value bit. The first character
 * may be an 's' if the vector is signed.
 */
vpiHandle vpip_make_द्विआधारी_const(अचिन्हित wid, स्थिर अक्षर*bits)
{
      काष्ठा __vpiBinaryConst*obj = नया __vpiBinaryConst;

      स्थिर अक्षर*bp = bits;
      अगर (*bp == 's') {
	    bp += 1;
	    obj->signed_flag = 1;
      }

      obj->bits = vector4_from_text(bp, wid);

      वापस obj;
}

vvp_vector4_t vector4_from_text(स्थिर अक्षर*bits, अचिन्हित wid)
{
      vvp_vector4_t res (wid);

      क्रम (अचिन्हित idx = 0 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t val = BIT4_0;
	    चयन (bits[wid-idx-1]) {
		हाल '0':
		  val = BIT4_0;
		  अवरोध;
		हाल '1':
		  val = BIT4_1;
		  अवरोध;
		हाल 'x':
		  val = BIT4_X;
		  अवरोध;
		हाल 'z':
		  val = BIT4_Z;
		  अवरोध;
	    }

	    res.set_bit(idx, val);
      }

      वापस res;
}

काष्ठा __vpiBinaryParam  : खुला __vpiBinaryConst {
      __vpiBinaryParam(__vpiScope*scope, स्थिर vvp_vector4_t&b, bool is_signed,
                       स्थिर अक्षर*name, bool local_flag, दीर्घ file_idx, दीर्घ lineno);
      ~__vpiBinaryParam() override;
      पूर्णांक get_type_code(व्योम) स्थिर override;
      पूर्णांक vpi_get(पूर्णांक code) override;
      अक्षर*vpi_get_str(पूर्णांक code) override;
      vpiHandle vpi_handle(पूर्णांक code) override;

    निजी:
      __vpiScope*scope_;
      स्थिर अक्षर*basename_;
      bool     local_flag_;
      अचिन्हित file_idx_;
      अचिन्हित lineno_;
};

अंतरभूत __vpiBinaryParam::__vpiBinaryParam(__vpiScope*scope, स्थिर vvp_vector4_t&b,
                                          bool is_signed, स्थिर अक्षर*nam,
                                          bool local_flag, दीर्घ file_idx, दीर्घ lineno)
{
      scope_ = scope;
      bits = b;
      signed_flag = is_signed ? 1 : 0;
      sized_flag = 0;
      basename_ = nam;
      local_flag_ = local_flag;
      निश्चित(file_idx >= 0 && file_idx <= पूर्णांक_उच्च);
      file_idx_ = (अचिन्हित) file_idx;
      निश्चित(lineno >= 0 && lineno <= पूर्णांक_उच्च);
      lineno_ = (अचिन्हित) lineno;
}

__vpiBinaryParam::~__vpiBinaryParam()
{
      मिटाओ[]basename_;
}

पूर्णांक __vpiBinaryParam::get_type_code(व्योम) स्थिर
{ वापस vpiParameter; }

पूर्णांक __vpiBinaryParam::vpi_get(पूर्णांक code)
{
    चयन (code) {
      हाल vpi_पंक्ति_No :
        वापस lineno_;

      हाल vpiLocalParam :
        वापस local_flag_;

      शेष :
        वापस __vpiBinaryConst::vpi_get(code);
    }
}

अक्षर*__vpiBinaryParam::vpi_get_str(पूर्णांक code)
{
      अगर (code == vpiFile)
	    वापस simple_set_rbuf_str(file_names[file_idx_]);

      वापस generic_get_str(code, scope_, basename_, शून्य);
}


vpiHandle __vpiBinaryParam::vpi_handle(पूर्णांक code)
{
      चयन (code) {
	  हाल vpiScope:
	    वापस scope_;

	  हाल vpiModule:
	    वापस vpip_module(scope_);

	  शेष:
	    वापस 0;
      }
}


vpiHandle vpip_make_द्विआधारी_param(स्थिर अक्षर*name, स्थिर vvp_vector4_t&bits,
				 bool signed_flag, bool local_flag,
				 दीर्घ file_idx, दीर्घ lineno)
{
      काष्ठा __vpiBinaryParam*obj = नया __vpiBinaryParam(vpip_peek_current_scope(),
                                                         bits, signed_flag, name,
                                                         local_flag, file_idx, lineno);

      वापस obj;
}



__vpiDecConst::__vpiDecConst(पूर्णांक val)
{
      value = val;
}

__vpiDecConst::__vpiDecConst(स्थिर __vpiDecConst&that)
: __vpiHandle(), value(that.value)
{
}


पूर्णांक __vpiDecConst::get_type_code(व्योम) स्थिर
{ वापस vpiConstant; }

पूर्णांक __vpiDecConst::vpi_get(पूर्णांक code)
{
      चयन (code) {
	  हाल vpiConstType:
	    वापस vpiDecConst;

	  हाल vpiSigned:
	    वापस 1;

	  हाल vpi_आकार:
	    वापस 32;

          हाल vpiAutomatic:
	    वापस 0;

#अगर defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          हाल _vpiFromThr:
	      वापस _vpiNoThr;
#पूर्ण_अगर

	  शेष:
	    ख_लिखो(मानक_त्रुटि, "vvp error: get %d not supported "
		    "by vpiDecConst\n", code);
	    निश्चित(0);
	    वापस 0;
      }
}


व्योम __vpiDecConst::vpi_get_मूल्य(p_vpi_मूल्य vp)
{
      अटल स्थिर माप_प्रकार RBUF_USE_आकार = 64 + 1;
      अक्षर*rbuf = static_cast<अक्षर *>(need_परिणाम_buf(RBUF_USE_आकार, RBUF_VAL));
      अक्षर*cp = rbuf;

      चयन (vp->format) {

	  हाल vpiObjTypeVal:
	  हाल vpiIntVal: {
		vp->value.integer = value;
		अवरोध;
	  }

          हाल vpiDecStrVal:
	      snprintf(rbuf, RBUF_USE_आकार, "%d", value);
	      vp->value.str = rbuf;
	      अवरोध;

          हाल vpiBinStrVal:
	      क्रम(पूर्णांक bit=31; bit>=0;bit--){
		  *cp++ = "01"[ (static_cast<अचिन्हित>(value)>>bit)&1 ];
	      }
	      *cp = 0;

	      vp->value.str = rbuf;
	      अवरोध;

          हाल vpiHexStrVal:
	      snprintf(rbuf, RBUF_USE_आकार, "%08x", value);
	      vp->value.str = rbuf;
	      अवरोध;

          हाल vpiOctStrVal:
	      snprintf(rbuf, RBUF_USE_आकार, "%011x", value);
	      vp->value.str = rbuf;
	      अवरोध;

	  शेष:
	    ख_लिखो(मानक_त्रुटि, "vvp error (vpi_const.cc): format %d not supported "
		    "by vpiDecConst\n", (पूर्णांक)vp->format);
	    vp->format = vpiSuppressVal;
	    अवरोध;
      }
}


अंतरभूत __vpiRealConst::__vpiRealConst(द्विगुन val)
: value(val)
{ }

पूर्णांक __vpiRealConst::get_type_code(व्योम) स्थिर
{ वापस vpiConstant; }

पूर्णांक __vpiRealConst::vpi_get(पूर्णांक code)
{
      चयन (code) {
	  हाल vpi_पंक्ति_No:
	    वापस 0;  // Not implemented for now!

	  हाल vpi_आकार:
	    वापस 1;

	  हाल vpiConstType:
	    वापस vpiRealConst;

	  हाल vpiSigned:
	    वापस 1;

          हाल vpiAutomatic:
	    वापस 0;

#अगर defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          हाल _vpiFromThr:
	      वापस _vpiNoThr;
#पूर्ण_अगर

	  शेष:
	    ख_लिखो(मानक_त्रुटि, "vvp error: get %d not supported "
		    "by vpiRealConst\n", code);
	    निश्चित(0);
	    वापस 0;
      }
}


व्योम __vpiRealConst::vpi_get_मूल्य(p_vpi_मूल्य val)
{
      vpip_real_get_मूल्य(value, val);
}


vpiHandle vpip_make_real_const(द्विगुन value)
{
      __vpiRealConst*obj = नया __vpiRealConst(value);
      वापस obj;
}

काष्ठा __vpiRealParam  : खुला __vpiRealConst {
      __vpiRealParam(__vpiScope* scope, द्विगुन val, स्थिर अक्षर*name,
                     bool local_flag, दीर्घ file_idx, दीर्घ lineno);
      ~__vpiRealParam() override;
      पूर्णांक get_type_code(व्योम) स्थिर override;
      पूर्णांक vpi_get(पूर्णांक code) override;
      अक्षर*vpi_get_str(पूर्णांक code) override;
      vpiHandle vpi_handle(पूर्णांक code) override;

    निजी:
      __vpiScope* scope_;
      स्थिर अक्षर*basename_;
      bool local_flag_;
      अचिन्हित file_idx_;
      अचिन्हित lineno_;
};


अंतरभूत __vpiRealParam::__vpiRealParam(__vpiScope* scope, द्विगुन val, स्थिर अक्षर*name,
                                      bool local_flag, दीर्घ file_idx, दीर्घ lineno)
: __vpiRealConst(val)
{
      scope_ = scope;
      basename_ = name;
      local_flag_ = local_flag;
      निश्चित(file_idx >= 0 && file_idx <= पूर्णांक_उच्च);
      file_idx_ = (अचिन्हित) file_idx;
      निश्चित(lineno >= 0 && lineno <= पूर्णांक_उच्च);
      lineno_ = (अचिन्हित) lineno;
}

__vpiRealParam::~__vpiRealParam()
{
      मिटाओ[]basename_;
}


पूर्णांक __vpiRealParam::get_type_code(व्योम) स्थिर
{ वापस vpiParameter; }

पूर्णांक __vpiRealParam::vpi_get(पूर्णांक code)
{
    चयन (code) {
      हाल vpi_पंक्ति_No :
        वापस lineno_;

      हाल vpiLocalParam :
        वापस local_flag_;

      शेष :
           वापस __vpiRealConst::vpi_get(code);
    }
}

अक्षर* __vpiRealParam::vpi_get_str(पूर्णांक code)
{
      अगर (code == vpiFile)
            वापस simple_set_rbuf_str(file_names[file_idx_]);

      वापस generic_get_str(code, scope_, basename_, शून्य);
}

vpiHandle __vpiRealParam::vpi_handle(पूर्णांक code)
{
      चयन (code) {
          हाल vpiScope:
            वापस scope_;

	  हाल vpiModule:
	    वापस vpip_module(scope_);

          शेष:
            वापस 0;
      }
}


vpiHandle vpip_make_real_param(स्थिर अक्षर*name, द्विगुन value,
                               bool local_flag, दीर्घ file_idx, दीर्घ lineno)
{
      काष्ठा __vpiRealParam*obj = नया __vpiRealParam(vpip_peek_current_scope(),
                                                     value, name, local_flag,
                                                     file_idx, lineno);
      वापस obj;
}

/*
 * Make a VPI null constant
 */
अंतरभूत __vpiNullConst::__vpiNullConst()
{ }

पूर्णांक __vpiNullConst::get_type_code(व्योम) स्थिर
{ वापस vpiConstant; }

पूर्णांक __vpiNullConst::vpi_get(पूर्णांक code)
{
      चयन (code) {
	  हाल vpi_पंक्ति_No:
	    वापस 0;  // Not implemented for now!

	  हाल vpi_आकार:
	    वापस 32;

	  हाल vpiConstType:
	    वापस vpiNullConst;

	  हाल vpiSigned:
	    वापस 0;

          हाल vpiAutomatic:
	    वापस 0;

#अगर defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
          हाल _vpiFromThr:
	      वापस _vpiNoThr;
#पूर्ण_अगर

	  शेष:
	    ख_लिखो(मानक_त्रुटि, "vvp error: get %d not supported "
		    "by vpiNullConst\n", code);
	    निश्चित(0);
	    वापस 0;
      }
}

व्योम __vpiNullConst::vpi_get_मूल्य(p_vpi_मूल्य val)
{
      अटल स्थिर माप_प्रकार RBUF_USE_आकार = 64 + 1;
      अक्षर*rbuf = static_cast<अक्षर *>(need_परिणाम_buf(RBUF_USE_आकार, RBUF_VAL));

      चयन (val->format) {

	  हाल vpiObjTypeVal:
	    val->format = vpiStringVal;
	    // fall through
	  हाल vpiBinStrVal:
	  हाल vpiDecStrVal:
	  हाल vpiOctStrVal:
	  हाल vpiHexStrVal:
	  हाल vpiStringVal:
	    snprintf(rbuf, RBUF_USE_आकार, "null");
	    val->value.str = rbuf;
	    अवरोध;

          हाल vpiScalarVal:
	    val->value.scalar = vpi0;
	    अवरोध;

	  हाल vpiIntVal:
	    val->value.integer = 0;
	    अवरोध;

	  हाल vpiVectorVal:
	    val->value.vector = static_cast<p_vpi_vecval>
	                        (need_परिणाम_buf(2*माप(s_vpi_vecval),
	                                         RBUF_VAL));
	    क्रम (अचिन्हित idx = 0; idx < 2; idx += 1) {
		  val->value.vector[idx].aval = 0;
		  val->value.vector[idx].bval = 0;
	    }
	    अवरोध;

	  हाल vpiRealVal:
	    val->value.real = 0.0;
	    अवरोध;

	  शेष:
	    ख_लिखो(मानक_त्रुटि, "vvp error: format %d not supported "
		    "by vpiNullConst\n", (पूर्णांक)val->format);
	    val->format = vpiSuppressVal;
	    अवरोध;
      }
}

vpiHandle vpip_make_null_const()
{
      __vpiNullConst*obj = नया __vpiNullConst;
      वापस obj;
}

#अगर_घोषित CHECK_WITH_VALGRIND
व्योम constant_delete(vpiHandle item)
{
      निश्चित(item->get_type_code() == vpiConstant);
      चयन(vpi_get(vpiConstType, item)) {
	  हाल vpiStringConst:
	    मिटाओ dynamic_cast<__vpiStringConst*>(item);
	    अवरोध;
	  हाल vpiDecConst:
	    मिटाओ dynamic_cast<__vpiDecConst*>(item);
	    अवरोध;
	  हाल vpi_द्विआधारी_Const:
	    मिटाओ dynamic_cast<__vpiBinaryConst*>(item);
	    अवरोध;
	  हाल vpiRealConst:
	    मिटाओ dynamic_cast<__vpiRealConst*>(item);
	    अवरोध;
	  हाल vpiNullConst:
	    मिटाओ dynamic_cast<__vpiNullConst*>(item);
	    अवरोध;
	  शेष:
	    निश्चित(0);
      }
}

व्योम parameter_delete(vpiHandle item)
{
      चयन(vpi_get(vpiConstType, item)) {
	  हाल vpiStringConst: {
	    श्रेणी __vpiStringParam*rfp = dynamic_cast<__vpiStringParam*>(item);
	    मिटाओ rfp;
	    अवरोध; }
	  हाल vpi_द्विआधारी_Const: {
	    काष्ठा __vpiBinaryParam*rfp = dynamic_cast<__vpiBinaryParam*>(item);
	    मिटाओ rfp;
	    अवरोध; }
	  हाल vpiRealConst: {
	    काष्ठा __vpiRealParam*rfp = dynamic_cast<__vpiRealParam*>(item);
	    मिटाओ rfp;
	    अवरोध; }
	  शेष:
	    निश्चित(0);
      }
}
#पूर्ण_अगर
