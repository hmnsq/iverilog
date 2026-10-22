/*
 * Copyright (c) 2012-2025 Picture Elements, Inc.
 *    Stephen Williams (steve@icarus.com)
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

# include  "compile.h"
# include  "vpi_priv.h"
# include  "config.h"
#अगर_घोषित CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#पूर्ण_अगर

__vpiCobjectVar::__vpiCobjectVar(__vpiScope*sc, स्थिर अक्षर*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

पूर्णांक __vpiCobjectVar::get_type_code(व्योम) स्थिर
{ वापस vpiClassVar; }

पूर्णांक __vpiCobjectVar::vpi_get(पूर्णांक code)
{
      चयन (code) {
	हाल vpi_पंक्ति_No:
	    वापस 0;  // Not implemented for now!

	हाल vpi_आकार:
	    वापस 64;

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
	                    "by vpiClassVar\n", code);
	    निश्चित(0);
	    वापस 0;
      }
}

व्योम __vpiCobjectVar::vpi_get_मूल्य(p_vpi_मूल्य val)
{
// FIXME: We need to get the assigned object address if one is assigned.
//fprintf(stderr, "HERE: %p\n", get_net());
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
	    snprintf(rbuf, RBUF_USE_आकार, "    null");
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
	                    "by vpiClassVar\n", (पूर्णांक)val->format);
	    val->format = vpiSuppressVal;
	    अवरोध;
      }
}

vpiHandle vpip_make_cobject_var(स्थिर अक्षर*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      स्थिर अक्षर*use_name = name ? vpip_name_string(name) : शून्य;

      __vpiCobjectVar*obj = नया __vpiCobjectVar(scope, use_name, net);

      वापस obj;
}

#अगर_घोषित CHECK_WITH_VALGRIND
व्योम class_delete(vpiHandle item)
{
      __vpiCobjectVar*obj = dynamic_cast<__vpiCobjectVar*>(item);
      मिटाओ obj;
}
#पूर्ण_अगर
