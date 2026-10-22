/*
 * Copyright (c) 2012-2025 Stephen Williams (steve@icarus.com)
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
# include  "vvp_net_sig.h"
# include  "schedule.h"
#నిర్వచితమైతే CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#అయితే_ముగింపు
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <cassert>
# include  "ivl_alloc.h"

ఉపయోగించి నామాకాశ std;

__vpiStringVar::__vpiStringVar(__vpiScope*sc, స్థిరరాశి అక్షరం*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

పూర్ణాంకం __vpiStringVar::get_type_సంకేతం(శూన్య) స్థిరరాశి
{ తిరిగి vpiStringVar; }

పూర్ణాంకం __vpiStringVar::vpi_get(పూర్ణాంకం code)
{
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (get_net()->fun);
      ప్రతిజ్ఞా(fun);
      string str = fun->get_string();

      ఎంపిక (code) {
	  స్థితి vpi_కొలత:
	      // The vpiSize of a string variable is the number of
	      // bytes (characters) in that string.
	    తిరిగి str.size();
	  స్థితి vpi_ఎడమ_Range:
	    తిరిగి 0;
	  స్థితి vpi_కుడి_Range:
            తిరిగి str.size() - 1;
	  అప్రమేయ:
	    fprintf(stderr, "vpi sorry: property is not implemented");
	    ప్రతిజ్ఞా(అబద్ధం);
	    తిరిగి 0;
      }
}

శూన్య __vpiStringVar::vpi_get_విలువ(p_vpi_విలువ val)
{
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (get_net()->fun);
      ప్రతిజ్ఞా(fun);
      string str = fun->get_string();

      ఒకవేళ (val->format == vpiStringVal || val->format == vpiObjTypeVal) {
	    అక్షరం*rbuf = static_cast<అక్షరం *>(need_ఫలితం_buf(str.size()+1, RBUF_VAL));
	    strcpy(rbuf, str.c_str());
	    val->format = vpiStringVal;
	    val->value.str = rbuf;
	    తిరిగి;
      }

      val->format = vpiSuppressVal;
}

vpiHandle __vpiStringVar::vpi_put_విలువ(p_vpi_విలువ val, పూర్ణాంకం)
{
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (get_net()->fun);
      ప్రతిజ్ఞా(fun);
      ప్రతిజ్ఞా(val->format == vpiStringVal);
      vvp_net_ptr_t dest (get_net(), 0);
      vvp_send_string(dest, val->value.str, vthread_get_wt_context());
      తిరిగి 0;
}

vpiHandle vpip_make_string_var(స్థిరరాశి అక్షరం*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      స్థిరరాశి అక్షరం*use_name = name ? vpip_name_string(name) : ఖాళీ;

      __vpiStringVar*obj = కొత్త __vpiStringVar(scope, use_name, net);

      తిరిగి obj;
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
శూన్య string_delete(vpiHandle అంశం)
{
      __vpiStringVar*obj = dynamic_cast<__vpiStringVar*>(అంశం);
      తొలగించు obj;
}
#అయితే_ముగింపు
