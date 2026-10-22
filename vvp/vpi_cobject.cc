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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#ਜੇ_ਅੰਤ

__vpiCobjectVar::__vpiCobjectVar(__vpiScope*sc, ਅਚਲ ਅੱਖਰ*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

ਪੂਰਨ __vpiCobjectVar::get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ
{ ਵਾਪਸ vpiClassVar; }

ਪੂਰਨ __vpiCobjectVar::vpi_get(ਪੂਰਨ code)
{
      ਚੋਣ (code) {
	ਹਾਲਤ vpi_ਸਤਰ_No:
	    ਵਾਪਸ 0;  // Not implemented for now!

	ਹਾਲਤ vpi_ਮਾਪ:
	    ਵਾਪਸ 64;

	ਹਾਲਤ vpiConstType:
	    ਵਾਪਸ vpiNullConst;

	ਹਾਲਤ vpiSigned:
	    ਵਾਪਸ 0;

	ਹਾਲਤ vpiAutomatic:
	    ਵਾਪਸ 0;

#ਜੇ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	ਹਾਲਤ _vpiFromThr:
	    ਵਾਪਸ _vpiNoThr;
#ਜੇ_ਅੰਤ

	ਮੂਲ:
	    fprintf(stderr, "vvp error: get %d not supported "
	                    "by vpiClassVar\n", code);
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
	    ਵਾਪਸ 0;
      }
}

ਖਾਲੀ __vpiCobjectVar::vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val)
{
// FIXME: We need to get the assigned object address if one is assigned.
//fprintf(stderr, "HERE: %p\n", get_net());
      ਸਥਿਰ ਅਚਲ size_t RBUF_USE_ਮਾਪ = 64 + 1;
      ਅੱਖਰ*rbuf = static_cast<ਅੱਖਰ *>(need_ਨਤੀਜਾ_buf(RBUF_USE_ਮਾਪ, RBUF_VAL));

      ਚੋਣ (val->format) {
	ਹਾਲਤ vpiObjTypeVal:
	    val->format = vpiStringVal;
	    // fall through
	ਹਾਲਤ vpiBinStrVal:
	ਹਾਲਤ vpiDecStrVal:
	ਹਾਲਤ vpiOctStrVal:
	ਹਾਲਤ vpiHexStrVal:
	ਹਾਲਤ vpiStringVal:
	    snprintf(rbuf, RBUF_USE_ਮਾਪ, "    null");
	    val->value.str = rbuf;
	    ਰੋਕੋ;

	ਹਾਲਤ vpiScalarVal:
	    val->value.scalar = vpi0;
	    ਰੋਕੋ;

	ਹਾਲਤ vpiIntVal:
	    val->value.integer = 0;
	    ਰੋਕੋ;

	ਹਾਲਤ vpi_ਸਦਿਸ਼_Val:
	    val->value.vector = static_cast<p_vpi_vecval>
	                        (need_ਨਤੀਜਾ_buf(2*ਆਕਾਰ(s_vpi_vecval),
	                                         RBUF_VAL));
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0; idx < 2; idx += 1) {
		  val->value.vector[idx].aval = 0;
		  val->value.vector[idx].bval = 0;
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ vpiRealVal:
	    val->value.real = 0.0;
	    ਰੋਕੋ;

	ਮੂਲ:
	    fprintf(stderr, "vvp error: format %d not supported "
	                    "by vpiClassVar\n", (ਪੂਰਨ)val->format);
	    val->format = vpiSuppressVal;
	    ਰੋਕੋ;
      }
}

vpiHandle vpip_make_cobject_var(ਅਚਲ ਅੱਖਰ*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ਅਚਲ ਅੱਖਰ*use_name = name ? vpip_name_string(name) : ਨਿਰਾ;

      __vpiCobjectVar*obj = ਨਵਾਂ __vpiCobjectVar(scope, use_name, net);

      ਵਾਪਸ obj;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਖਾਲੀ class_delete(vpiHandle ਵਸਤੂ)
{
      __vpiCobjectVar*obj = dynamic_cast<__vpiCobjectVar*>(ਵਸਤੂ);
      ਮਿਟਾਓ obj;
}
#ਜੇ_ਅੰਤ
