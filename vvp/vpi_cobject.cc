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
#నిర్వచితమైతే CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#అయితే_ముగింపు

__vpiCobjectVar::__vpiCobjectVar(__vpiScope*sc, స్థిరరాశి అక్షరం*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

పూర్ణాంకం __vpiCobjectVar::get_type_సంకేతం(శూన్య) స్థిరరాశి
{ తిరిగి vpiClassVar; }

పూర్ణాంకం __vpiCobjectVar::vpi_get(పూర్ణాంకం code)
{
      ఎంపిక (code) {
	స్థితి vpi_పంక్తి_No:
	    తిరిగి 0;  // Not implemented for now!

	స్థితి vpi_కొలత:
	    తిరిగి 64;

	స్థితి vpiConstType:
	    తిరిగి vpiNullConst;

	స్థితి vpiSigned:
	    తిరిగి 0;

	స్థితి vpiAutomatic:
	    తిరిగి 0;

#ఒకవేళ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	స్థితి _vpiFromThr:
	    తిరిగి _vpiNoThr;
#అయితే_ముగింపు

	అప్రమేయ:
	    fprintf(stderr, "vvp error: get %d not supported "
	                    "by vpiClassVar\n", code);
	    ప్రతిజ్ఞా(0);
	    తిరిగి 0;
      }
}

శూన్య __vpiCobjectVar::vpi_get_విలువ(p_vpi_విలువ val)
{
// FIXME: We need to get the assigned object address if one is assigned.
//fprintf(stderr, "HERE: %p\n", get_net());
      స్థిర స్థిరరాశి size_t RBUF_USE_కొలత = 64 + 1;
      అక్షరం*rbuf = static_cast<అక్షరం *>(need_ఫలితం_buf(RBUF_USE_కొలత, RBUF_VAL));

      ఎంపిక (val->format) {
	స్థితి vpiObjTypeVal:
	    val->format = vpiStringVal;
	    // fall through
	స్థితి vpiBinStrVal:
	స్థితి vpiDecStrVal:
	స్థితి vpiOctStrVal:
	స్థితి vpiHexStrVal:
	స్థితి vpiStringVal:
	    snprintf(rbuf, RBUF_USE_కొలత, "    null");
	    val->value.str = rbuf;
	    ఆపు;

	స్థితి vpiScalarVal:
	    val->value.scalar = vpi0;
	    ఆపు;

	స్థితి vpiIntVal:
	    val->value.integer = 0;
	    ఆపు;

	స్థితి vpi_సదిశ_Val:
	    val->value.vector = static_cast<p_vpi_vecval>
	                        (need_ఫలితం_buf(2*పరిమాణం(s_vpi_vecval),
	                                         RBUF_VAL));
	    కొరకు (గుర్తులేని idx = 0; idx < 2; idx += 1) {
		  val->value.vector[idx].aval = 0;
		  val->value.vector[idx].bval = 0;
	    }
	    ఆపు;

	స్థితి vpiRealVal:
	    val->value.real = 0.0;
	    ఆపు;

	అప్రమేయ:
	    fprintf(stderr, "vvp error: format %d not supported "
	                    "by vpiClassVar\n", (పూర్ణాంకం)val->format);
	    val->format = vpiSuppressVal;
	    ఆపు;
      }
}

vpiHandle vpip_make_cobject_var(స్థిరరాశి అక్షరం*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      స్థిరరాశి అక్షరం*use_name = name ? vpip_name_string(name) : ఖాళీ;

      __vpiCobjectVar*obj = కొత్త __vpiCobjectVar(scope, use_name, net);

      తిరిగి obj;
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
శూన్య class_delete(vpiHandle అంశం)
{
      __vpiCobjectVar*obj = dynamic_cast<__vpiCobjectVar*>(అంశం);
      తొలగించు obj;
}
#అయితే_ముగింపు
