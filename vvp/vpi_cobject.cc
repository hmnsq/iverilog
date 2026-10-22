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
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#اگر_ختم

__vpiCobjectVar::__vpiCobjectVar(__vpiScope*sc, ثابت حرف*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

صحیح __vpiCobjectVar::get_type_رمز(خالی) ثابت
{ واپس vpiClassVar; }

صحیح __vpiCobjectVar::vpi_get(صحیح code)
{
      انتخاب (code) {
	حالت vpi_سطر_No:
	    واپس 0;  // Not implemented for now!

	حالت vpi_سائز:
	    واپس 64;

	حالت vpiConstType:
	    واپس vpiNullConst;

	حالت vpiSigned:
	    واپس 0;

	حالت vpiAutomatic:
	    واپس 0;

#اگر defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	حالت _vpiFromThr:
	    واپس _vpiNoThr;
#اگر_ختم

	طےشدہ:
	    fprintf(stderr, "vvp error: get %d not supported "
	                    "by vpiClassVar\n", code);
	    أكد(0);
	    واپس 0;
      }
}

خالی __vpiCobjectVar::vpi_get_قدر(p_vpi_قدر val)
{
// FIXME: We need to get the assigned object address if one is assigned.
//fprintf(stderr, "HERE: %p\n", get_net());
      ساکن ثابت size_t RBUF_USE_سائز = 64 + 1;
      حرف*rbuf = static_cast<حرف *>(need_نتیجہ_buf(RBUF_USE_سائز, RBUF_VAL));

      انتخاب (val->format) {
	حالت vpiObjTypeVal:
	    val->format = vpiStringVal;
	    // fall through
	حالت vpiBinStrVal:
	حالت vpiDecStrVal:
	حالت vpiOctStrVal:
	حالت vpiHexStrVal:
	حالت vpiStringVal:
	    snprintf(rbuf, RBUF_USE_سائز, "    null");
	    val->value.str = rbuf;
	    توڑو;

	حالت vpiScalarVal:
	    val->value.scalar = vpi0;
	    توڑو;

	حالت vpiIntVal:
	    val->value.integer = 0;
	    توڑو;

	حالت vpi_سمتیہ_Val:
	    val->value.vector = static_cast<p_vpi_vecval>
	                        (need_نتیجہ_buf(2*حجم(s_vpi_vecval),
	                                         RBUF_VAL));
	    برائے (غیرعلامتی idx = 0; idx < 2; idx += 1) {
		  val->value.vector[idx].aval = 0;
		  val->value.vector[idx].bval = 0;
	    }
	    توڑو;

	حالت vpiRealVal:
	    val->value.real = 0.0;
	    توڑو;

	طےشدہ:
	    fprintf(stderr, "vvp error: format %d not supported "
	                    "by vpiClassVar\n", (صحیح)val->format);
	    val->format = vpiSuppressVal;
	    توڑو;
      }
}

vpiHandle vpip_make_cobject_var(ثابت حرف*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ثابت حرف*use_name = name ? vpip_name_string(name) : تہی;

      __vpiCobjectVar*obj = نیا __vpiCobjectVar(scope, use_name, net);

      واپس obj;
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
خالی class_delete(vpiHandle شے)
{
      __vpiCobjectVar*obj = dynamic_cast<__vpiCobjectVar*>(شے);
      حذف obj;
}
#اگر_ختم
