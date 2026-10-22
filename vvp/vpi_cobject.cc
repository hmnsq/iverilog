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

__vpiCobjectVar::__vpiCobjectVar(__vpiScope*sc, अचर वर्ण*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

पूर्णाङ्क __vpiCobjectVar::get_type_code(शून्य) अचर
{ फर्काऊ vpiClassVar; }

पूर्णाङ्क __vpiCobjectVar::vpi_get(पूर्णाङ्क code)
{
      विकल्प (code) {
	पक्ष vpi_पङ्क्ति_No:
	    फर्काऊ 0;  // Not implemented for now!

	पक्ष vpiSize:
	    फर्काऊ 64;

	पक्ष vpiConstType:
	    फर्काऊ vpiNullConst;

	पक्ष vpiSigned:
	    फर्काऊ 0;

	पक्ष vpiAutomatic:
	    फर्काऊ 0;

#यदि defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	पक्ष _vpiFromThr:
	    फर्काऊ _vpiNoThr;
#endif

	स्वभाव:
	    ख_लिखो(मानक_त्रुटि, "vvp error: get %d not supported "
	                    "by vpiClassVar\n", code);
	    प्रतिज्ञा(0);
	    फर्काऊ 0;
      }
}

शून्य __vpiCobjectVar::vpi_get_मान(p_vpi_मान val)
{
// FIXME: We need to get the assigned object address if one is assigned.
//fprintf(stderr, "HERE: %p\n", get_net());
      स्थिर अचर माप_प्रकार RBUF_USE_SIZE = 64 + 1;
      वर्ण*rbuf = static_cast<वर्ण *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));

      विकल्प (val->format) {
	पक्ष vpiObjTypeVal:
	    val->format = vpiStringVal;
	    // fall through
	पक्ष vpiBinStrVal:
	पक्ष vpiDecStrVal:
	पक्ष vpiOctStrVal:
	पक्ष vpiHexStrVal:
	पक्ष vpiStringVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "    null");
	    val->value.str = rbuf;
	    रोक;

	पक्ष vpiScalarVal:
	    val->value.scalar = vpi0;
	    रोक;

	पक्ष vpiIntVal:
	    val->value.integer = 0;
	    रोक;

	पक्ष vpiVectorVal:
	    val->value.vector = static_cast<p_vpi_vecval>
	                        (need_result_buf(2*परिमाण(s_vpi_vecval),
	                                         RBUF_VAL));
	    लागि (अचिन्हित idx = 0; idx < 2; idx += 1) {
		  val->value.vector[idx].aval = 0;
		  val->value.vector[idx].bval = 0;
	    }
	    रोक;

	पक्ष vpiRealVal:
	    val->value.real = 0.0;
	    रोक;

	स्वभाव:
	    ख_लिखो(मानक_त्रुटि, "vvp error: format %d not supported "
	                    "by vpiClassVar\n", (पूर्णाङ्क)val->format);
	    val->format = vpiSuppressVal;
	    रोक;
      }
}

vpiHandle vpip_make_cobject_var(अचर वर्ण*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      अचर वर्ण*use_name = name ? vpip_name_string(name) : रिक्त;

      __vpiCobjectVar*obj = नूतन __vpiCobjectVar(scope, use_name, net);

      फर्काऊ obj;
}

#ifdef CHECK_WITH_VALGRIND
शून्य class_delete(vpiHandle item)
{
      __vpiCobjectVar*obj = dynamic_cast<__vpiCobjectVar*>(item);
      delete obj;
}
#endif
