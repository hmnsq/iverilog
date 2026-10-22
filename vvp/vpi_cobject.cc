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

__vpiCobjectVar::__vpiCobjectVar(__vpiScope*sc, અચલ અક્ષર*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

પૂર્ણાંક __vpiCobjectVar::get_type_code(શૂન્ય) અચલ
{ પાછા vpiClassVar; }

પૂર્ણાંક __vpiCobjectVar::vpi_get(પૂર્ણાંક code)
{
      પસંદગી (code) {
	સ્થિતિ vpi_લીટી_No:
	    પાછા 0;  // Not implemented for now!

	સ્થિતિ vpiSize:
	    પાછા 64;

	સ્થિતિ vpiConstType:
	    પાછા vpiNullConst;

	સ્થિતિ vpiSigned:
	    પાછા 0;

	સ્થિતિ vpiAutomatic:
	    પાછા 0;

#જો defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	સ્થિતિ _vpiFromThr:
	    પાછા _vpiNoThr;
#endif

	મૂળભૂત:
	    fprintf(stderr, "vvp error: get %d not supported "
	                    "by vpiClassVar\n", code);
	    પ્રતિજ્ઞા(0);
	    પાછા 0;
      }
}

શૂન્ય __vpiCobjectVar::vpi_get_કિંમત(p_vpi_કિંમત val)
{
// FIXME: We need to get the assigned object address if one is assigned.
//fprintf(stderr, "HERE: %p\n", get_net());
      સ્થિર અચલ size_t RBUF_USE_SIZE = 64 + 1;
      અક્ષર*rbuf = static_cast<અક્ષર *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));

      પસંદગી (val->format) {
	સ્થિતિ vpiObjTypeVal:
	    val->format = vpiStringVal;
	    // fall through
	સ્થિતિ vpiBinStrVal:
	સ્થિતિ vpiDecStrVal:
	સ્થિતિ vpiOctStrVal:
	સ્થિતિ vpiHexStrVal:
	સ્થિતિ vpiStringVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "    null");
	    val->value.str = rbuf;
	    તોડો;

	સ્થિતિ vpiScalarVal:
	    val->value.scalar = vpi0;
	    તોડો;

	સ્થિતિ vpiIntVal:
	    val->value.integer = 0;
	    તોડો;

	સ્થિતિ vpiVectorVal:
	    val->value.vector = static_cast<p_vpi_vecval>
	                        (need_result_buf(2*કદ(s_vpi_vecval),
	                                         RBUF_VAL));
	    માટે (unsigned idx = 0; idx < 2; idx += 1) {
		  val->value.vector[idx].aval = 0;
		  val->value.vector[idx].bval = 0;
	    }
	    તોડો;

	સ્થિતિ vpiRealVal:
	    val->value.real = 0.0;
	    તોડો;

	મૂળભૂત:
	    fprintf(stderr, "vvp error: format %d not supported "
	                    "by vpiClassVar\n", (પૂર્ણાંક)val->format);
	    val->format = vpiSuppressVal;
	    તોડો;
      }
}

vpiHandle vpip_make_cobject_var(અચલ અક્ષર*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      અચલ અક્ષર*use_name = name ? vpip_name_string(name) : ખાલી;

      __vpiCobjectVar*obj = નવું __vpiCobjectVar(scope, use_name, net);

      પાછા obj;
}

#ifdef CHECK_WITH_VALGRIND
શૂન્ય class_delete(vpiHandle item)
{
      __vpiCobjectVar*obj = dynamic_cast<__vpiCobjectVar*>(item);
      delete obj;
}
#endif
