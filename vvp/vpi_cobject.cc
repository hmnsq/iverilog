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

__vpiCobjectVar::__vpiCobjectVar(__vpiScope*sc, ثابت نویسه*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

صحیح __vpiCobjectVar::get_type_code(تهی) ثابت
{ بازگشت vpiClassVar; }

صحیح __vpiCobjectVar::vpi_get(صحیح code)
{
      انتخاب (code) {
	حالت vpi_سطر_No:
	    بازگشت 0;  // Not implemented for now!

	حالت vpiSize:
	    بازگشت 64;

	حالت vpiConstType:
	    بازگشت vpiNullConst;

	حالت vpiSigned:
	    بازگشت 0;

	حالت vpiAutomatic:
	    بازگشت 0;

#اگر defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	حالت _vpiFromThr:
	    بازگشت _vpiNoThr;
#endif

	پیشفرض:
	    fprintf(stderr, "vvp error: get %d not supported "
	                    "by vpiClassVar\n", code);
	    أكد(0);
	    بازگشت 0;
      }
}

تهی __vpiCobjectVar::vpi_get_مقدار(p_vpi_مقدار val)
{
// FIXME: We need to get the assigned object address if one is assigned.
//fprintf(stderr, "HERE: %p\n", get_net());
      ایستا ثابت size_t RBUF_USE_SIZE = 64 + 1;
      نویسه*rbuf = static_cast<نویسه *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));

      انتخاب (val->format) {
	حالت vpiObjTypeVal:
	    val->format = vpiStringVal;
	    // fall through
	حالت vpiBinStrVal:
	حالت vpiDecStrVal:
	حالت vpiOctStrVal:
	حالت vpiHexStrVal:
	حالت vpiStringVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "    null");
	    val->value.str = rbuf;
	    بشکن;

	حالت vpiScalarVal:
	    val->value.scalar = vpi0;
	    بشکن;

	حالت vpiIntVal:
	    val->value.integer = 0;
	    بشکن;

	حالت vpiVectorVal:
	    val->value.vector = static_cast<p_vpi_vecval>
	                        (need_result_buf(2*اندازه(s_vpi_vecval),
	                                         RBUF_VAL));
	    برای (unsigned idx = 0; idx < 2; idx += 1) {
		  val->value.vector[idx].aval = 0;
		  val->value.vector[idx].bval = 0;
	    }
	    بشکن;

	حالت vpiRealVal:
	    val->value.real = 0.0;
	    بشکن;

	پیشفرض:
	    fprintf(stderr, "vvp error: format %d not supported "
	                    "by vpiClassVar\n", (صحیح)val->format);
	    val->format = vpiSuppressVal;
	    بشکن;
      }
}

vpiHandle vpip_make_cobject_var(ثابت نویسه*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ثابت نویسه*use_name = name ? vpip_name_string(name) : پوچ;

      __vpiCobjectVar*obj = جدید __vpiCobjectVar(scope, use_name, net);

      بازگشت obj;
}

#ifdef CHECK_WITH_VALGRIND
تهی class_delete(vpiHandle item)
{
      __vpiCobjectVar*obj = dynamic_cast<__vpiCobjectVar*>(item);
      delete obj;
}
#endif
