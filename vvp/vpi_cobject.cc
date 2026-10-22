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
#ifdef CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#endif

__vpiCobjectVar::__vpiCobjectVar(__vpiScope*sc, स्थिरांक अक्षर*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

पूर्णांक __vpiCobjectVar::get_type_code(शून्य) स्थिरांक
{ परत vpiClassVar; }

पूर्णांक __vpiCobjectVar::vpi_get(पूर्णांक code)
{
      निवड (code) {
	स्थिती vpi_ओळ_No:
	    परत 0;  // Not implemented for now!

	स्थिती vpiSize:
	    परत 64;

	स्थिती vpiConstType:
	    परत vpiNullConst;

	स्थिती vpiSigned:
	    परत 0;

	स्थिती vpiAutomatic:
	    परत 0;

#जर defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	स्थिती _vpiFromThr:
	    परत _vpiNoThr;
#endif

	पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "vvp error: get %d not supported "
	                    "by vpiClassVar\n", code);
	    प्रतिज्ञा(0);
	    परत 0;
      }
}

शून्य __vpiCobjectVar::vpi_get_मूल्य(p_vpi_मूल्य val)
{
// FIXME: We need to get the assigned object address if one is assigned.
//fprintf(stderr, "HERE: %p\n", get_net());
      स्थिर स्थिरांक माप_प्रकार RBUF_USE_SIZE = 64 + 1;
      अक्षर*rbuf = static_cast<अक्षर *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));

      निवड (val->format) {
	स्थिती vpiObjTypeVal:
	    val->format = vpiStringVal;
	    // fall through
	स्थिती vpiBinStrVal:
	स्थिती vpiDecStrVal:
	स्थिती vpiOctStrVal:
	स्थिती vpiHexStrVal:
	स्थिती vpiStringVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "    null");
	    val->value.str = rbuf;
	    थांबा;

	स्थिती vpiScalarVal:
	    val->value.scalar = vpi0;
	    थांबा;

	स्थिती vpiIntVal:
	    val->value.integer = 0;
	    थांबा;

	स्थिती vpiVectorVal:
	    val->value.vector = static_cast<p_vpi_vecval>
	                        (need_result_buf(2*आकार(s_vpi_vecval),
	                                         RBUF_VAL));
	    साठी (अचिन्हित idx = 0; idx < 2; idx += 1) {
		  val->value.vector[idx].aval = 0;
		  val->value.vector[idx].bval = 0;
	    }
	    थांबा;

	स्थिती vpiRealVal:
	    val->value.real = 0.0;
	    थांबा;

	पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "vvp error: format %d not supported "
	                    "by vpiClassVar\n", (पूर्णांक)val->format);
	    val->format = vpiSuppressVal;
	    थांबा;
      }
}

vpiHandle vpip_make_cobject_var(स्थिरांक अक्षर*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      स्थिरांक अक्षर*use_name = name ? vpip_name_string(name) : रिक्त;

      __vpiCobjectVar*obj = नवीन __vpiCobjectVar(scope, use_name, net);

      परत obj;
}

#ifdef CHECK_WITH_VALGRIND
शून्य class_delete(vpiHandle item)
{
      __vpiCobjectVar*obj = dynamic_cast<__vpiCobjectVar*>(item);
      delete obj;
}
#endif
