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

__vpiCobjectVar::__vpiCobjectVar(__vpiScope*sc, மாறிலி எழுத்து*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

முழு __vpiCobjectVar::get_type_code(வெறுமை) மாறிலி
{ திரும்பு vpiClassVar; }

முழு __vpiCobjectVar::vpi_get(முழு code)
{
      தேர்வு (code) {
	நிலை vpi_வரி_No:
	    திரும்பு 0;  // Not implemented for now!

	நிலை vpiSize:
	    திரும்பு 64;

	நிலை vpiConstType:
	    திரும்பு vpiNullConst;

	நிலை vpiSigned:
	    திரும்பு 0;

	நிலை vpiAutomatic:
	    திரும்பு 0;

#எனில் defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	நிலை _vpiFromThr:
	    திரும்பு _vpiNoThr;
#endif

	இயல்பு:
	    fprintf(stderr, "vvp error: get %d not supported "
	                    "by vpiClassVar\n", code);
	    ப்ரதிஜ்ஞா(0);
	    திரும்பு 0;
      }
}

வெறுமை __vpiCobjectVar::vpi_get_மதிப்பு(p_vpi_மதிப்பு val)
{
// FIXME: We need to get the assigned object address if one is assigned.
//fprintf(stderr, "HERE: %p\n", get_net());
      நிலையான மாறிலி size_t RBUF_USE_SIZE = 64 + 1;
      எழுத்து*rbuf = static_cast<எழுத்து *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));

      தேர்வு (val->format) {
	நிலை vpiObjTypeVal:
	    val->format = vpiStringVal;
	    // fall through
	நிலை vpiBinStrVal:
	நிலை vpiDecStrVal:
	நிலை vpiOctStrVal:
	நிலை vpiHexStrVal:
	நிலை vpiStringVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "    null");
	    val->value.str = rbuf;
	    நிறுத்து;

	நிலை vpiScalarVal:
	    val->value.scalar = vpi0;
	    நிறுத்து;

	நிலை vpiIntVal:
	    val->value.integer = 0;
	    நிறுத்து;

	நிலை vpiVectorVal:
	    val->value.vector = static_cast<p_vpi_vecval>
	                        (need_result_buf(2*அளவு(s_vpi_vecval),
	                                         RBUF_VAL));
	    க்கு (unsigned idx = 0; idx < 2; idx += 1) {
		  val->value.vector[idx].aval = 0;
		  val->value.vector[idx].bval = 0;
	    }
	    நிறுத்து;

	நிலை vpiRealVal:
	    val->value.real = 0.0;
	    நிறுத்து;

	இயல்பு:
	    fprintf(stderr, "vvp error: format %d not supported "
	                    "by vpiClassVar\n", (முழு)val->format);
	    val->format = vpiSuppressVal;
	    நிறுத்து;
      }
}

vpiHandle vpip_make_cobject_var(மாறிலி எழுத்து*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      மாறிலி எழுத்து*use_name = name ? vpip_name_string(name) : வெற்று;

      __vpiCobjectVar*obj = புதிய __vpiCobjectVar(scope, use_name, net);

      திரும்பு obj;
}

#ifdef CHECK_WITH_VALGRIND
வெறுமை class_delete(vpiHandle item)
{
      __vpiCobjectVar*obj = dynamic_cast<__vpiCobjectVar*>(item);
      delete obj;
}
#endif
