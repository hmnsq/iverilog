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

__vpiCobjectVar::__vpiCobjectVar(__vpiScope*sc, ثابت حرف*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

صحيح __vpiCobjectVar::get_type_code(فراغ) ثابت
{ أرجع vpiClassVar; }

صحيح __vpiCobjectVar::vpi_get(صحيح code)
{
      بدل (code) {
	حالة vpi_هێڵ_No:
	    أرجع 0;  // Not implemented for now!

	حالة vpiSize:
	    أرجع 64;

	حالة vpiConstType:
	    أرجع vpiNullConst;

	حالة vpiSigned:
	    أرجع 0;

	حالة vpiAutomatic:
	    أرجع 0;

#إذا defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	حالة _vpiFromThr:
	    أرجع _vpiNoThr;
#endif

	افتراضي:
	    fprintf(stderr, "vvp error: get %d not supported "
	                    "by vpiClassVar\n", code);
	    أكد(0);
	    أرجع 0;
      }
}

فراغ __vpiCobjectVar::vpi_get_بەها(p_vpi_بەها val)
{
// FIXME: We need to get the assigned object address if one is assigned.
//fprintf(stderr, "HERE: %p\n", get_net());
      ساكن ثابت size_t RBUF_USE_SIZE = 64 + 1;
      حرف*rbuf = static_cast<حرف *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));

      بدل (val->format) {
	حالة vpiObjTypeVal:
	    val->format = vpiStringVal;
	    // fall through
	حالة vpiBinStrVal:
	حالة vpiDecStrVal:
	حالة vpiOctStrVal:
	حالة vpiHexStrVal:
	حالة vpiStringVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "    null");
	    val->value.str = rbuf;
	    اكسر;

	حالة vpiScalarVal:
	    val->value.scalar = vpi0;
	    اكسر;

	حالة vpiIntVal:
	    val->value.integer = 0;
	    اكسر;

	حالة vpiVectorVal:
	    val->value.vector = static_cast<p_vpi_vecval>
	                        (need_result_buf(2*حجم(s_vpi_vecval),
	                                         RBUF_VAL));
	    لكل (unsigned idx = 0; idx < 2; idx += 1) {
		  val->value.vector[idx].aval = 0;
		  val->value.vector[idx].bval = 0;
	    }
	    اكسر;

	حالة vpiRealVal:
	    val->value.real = 0.0;
	    اكسر;

	افتراضي:
	    fprintf(stderr, "vvp error: format %d not supported "
	                    "by vpiClassVar\n", (صحيح)val->format);
	    val->format = vpiSuppressVal;
	    اكسر;
      }
}

vpiHandle vpip_make_cobject_var(ثابت حرف*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ثابت حرف*use_name = name ? vpip_name_string(name) : عدم;

      __vpiCobjectVar*obj = جديد __vpiCobjectVar(scope, use_name, net);

      أرجع obj;
}

#ifdef CHECK_WITH_VALGRIND
فراغ class_delete(vpiHandle item)
{
      __vpiCobjectVar*obj = dynamic_cast<__vpiCobjectVar*>(item);
      delete obj;
}
#endif
