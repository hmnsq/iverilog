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

__vpiCobjectVar::__vpiCobjectVar(__vpiScope*sc, സ്ഥിരം അക്ഷരം*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

പൂർണ്ണസംഖ്യ __vpiCobjectVar::get_type_code(ശൂന്യം) സ്ഥിരം
{ തിരികെ vpiClassVar; }

പൂർണ്ണസംഖ്യ __vpiCobjectVar::vpi_get(പൂർണ്ണസംഖ്യ code)
{
      തിരഞ്ഞെടുപ്പ് (code) {
	അവസ്ഥ vpi_വരി_No:
	    തിരികെ 0;  // Not implemented for now!

	അവസ്ഥ vpiSize:
	    തിരികെ 64;

	അവസ്ഥ vpiConstType:
	    തിരികെ vpiNullConst;

	അവസ്ഥ vpiSigned:
	    തിരികെ 0;

	അവസ്ഥ vpiAutomatic:
	    തിരികെ 0;

#എങ്കിൽ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	അവസ്ഥ _vpiFromThr:
	    തിരികെ _vpiNoThr;
#endif

	സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "vvp error: get %d not supported "
	                    "by vpiClassVar\n", code);
	    പ്രതിജ്ഞാ(0);
	    തിരികെ 0;
      }
}

ശൂന്യം __vpiCobjectVar::vpi_get_മൂല്യം(p_vpi_മൂല്യം val)
{
// FIXME: We need to get the assigned object address if one is assigned.
//fprintf(stderr, "HERE: %p\n", get_net());
      സ്ഥിതം സ്ഥിരം size_t RBUF_USE_SIZE = 64 + 1;
      അക്ഷരം*rbuf = static_cast<അക്ഷരം *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));

      തിരഞ്ഞെടുപ്പ് (val->format) {
	അവസ്ഥ vpiObjTypeVal:
	    val->format = vpiStringVal;
	    // fall through
	അവസ്ഥ vpiBinStrVal:
	അവസ്ഥ vpiDecStrVal:
	അവസ്ഥ vpiOctStrVal:
	അവസ്ഥ vpiHexStrVal:
	അവസ്ഥ vpiStringVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "    null");
	    val->value.str = rbuf;
	    നിർത്തുക;

	അവസ്ഥ vpiScalarVal:
	    val->value.scalar = vpi0;
	    നിർത്തുക;

	അവസ്ഥ vpiIntVal:
	    val->value.integer = 0;
	    നിർത്തുക;

	അവസ്ഥ vpiVectorVal:
	    val->value.vector = static_cast<p_vpi_vecval>
	                        (need_result_buf(2*വലുപ്പം(s_vpi_vecval),
	                                         RBUF_VAL));
	    വേണ്ടി (unsigned idx = 0; idx < 2; idx += 1) {
		  val->value.vector[idx].aval = 0;
		  val->value.vector[idx].bval = 0;
	    }
	    നിർത്തുക;

	അവസ്ഥ vpiRealVal:
	    val->value.real = 0.0;
	    നിർത്തുക;

	സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "vvp error: format %d not supported "
	                    "by vpiClassVar\n", (പൂർണ്ണസംഖ്യ)val->format);
	    val->format = vpiSuppressVal;
	    നിർത്തുക;
      }
}

vpiHandle vpip_make_cobject_var(സ്ഥിരം അക്ഷരം*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      സ്ഥിരം അക്ഷരം*use_name = name ? vpip_name_string(name) : ശൂന്യ;

      __vpiCobjectVar*obj = പുതിയ __vpiCobjectVar(scope, use_name, net);

      തിരികെ obj;
}

#ifdef CHECK_WITH_VALGRIND
ശൂന്യം class_delete(vpiHandle item)
{
      __vpiCobjectVar*obj = dynamic_cast<__vpiCobjectVar*>(item);
      delete obj;
}
#endif
