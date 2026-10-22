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
#ifdef CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#endif
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <cassert>
# include  "ivl_alloc.h"

using നാമാകാശ std;

__vpiStringVar::__vpiStringVar(__vpiScope*sc, സ്ഥിരം അക്ഷരം*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

പൂർണ്ണസംഖ്യ __vpiStringVar::get_type_code(ശൂന്യം) സ്ഥിരം
{ തിരികെ vpiStringVar; }

പൂർണ്ണസംഖ്യ __vpiStringVar::vpi_get(പൂർണ്ണസംഖ്യ code)
{
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (get_net()->fun);
      പ്രതിജ്ഞാ(fun);
      string str = fun->get_string();

      തിരഞ്ഞെടുപ്പ് (code) {
	  അവസ്ഥ vpiSize:
	      // The vpiSize of a string variable is the number of
	      // bytes (characters) in that string.
	    തിരികെ str.size();
	  അവസ്ഥ vpi_ഇടത്_Range:
	    തിരികെ 0;
	  അവസ്ഥ vpi_വലത്_Range:
            തിരികെ str.size() - 1;
	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "vpi sorry: property is not implemented");
	    പ്രതിജ്ഞാ(അസത്യം);
	    തിരികെ 0;
      }
}

ശൂന്യം __vpiStringVar::vpi_get_മൂല്യം(p_vpi_മൂല്യം val)
{
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (get_net()->fun);
      പ്രതിജ്ഞാ(fun);
      string str = fun->get_string();

      എങ്കിൽ (val->format == vpiStringVal || val->format == vpiObjTypeVal) {
	    അക്ഷരം*rbuf = static_cast<അക്ഷരം *>(need_result_buf(str.size()+1, RBUF_VAL));
	    strcpy(rbuf, str.c_str());
	    val->format = vpiStringVal;
	    val->value.str = rbuf;
	    തിരികെ;
      }

      val->format = vpiSuppressVal;
}

vpiHandle __vpiStringVar::vpi_put_മൂല്യം(p_vpi_മൂല്യം val, പൂർണ്ണസംഖ്യ)
{
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (get_net()->fun);
      പ്രതിജ്ഞാ(fun);
      പ്രതിജ്ഞാ(val->format == vpiStringVal);
      vvp_net_ptr_t dest (get_net(), 0);
      vvp_send_string(dest, val->value.str, vthread_get_wt_context());
      തിരികെ 0;
}

vpiHandle vpip_make_string_var(സ്ഥിരം അക്ഷരം*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      സ്ഥിരം അക്ഷരം*use_name = name ? vpip_name_string(name) : ശൂന്യ;

      __vpiStringVar*obj = പുതിയ __vpiStringVar(scope, use_name, net);

      തിരികെ obj;
}

#ifdef CHECK_WITH_VALGRIND
ശൂന്യം string_delete(vpiHandle item)
{
      __vpiStringVar*obj = dynamic_cast<__vpiStringVar*>(item);
      delete obj;
}
#endif
