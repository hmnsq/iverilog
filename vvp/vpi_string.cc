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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#ਜੇ_ਅੰਤ
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <cassert>
# include  "ivl_alloc.h"

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ std;

__vpiStringVar::__vpiStringVar(__vpiScope*sc, ਅਚਲ ਅੱਖਰ*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

ਪੂਰਨ __vpiStringVar::get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ
{ ਵਾਪਸ vpiStringVar; }

ਪੂਰਨ __vpiStringVar::vpi_get(ਪੂਰਨ code)
{
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (get_net()->fun);
      ਪ੍ਰਤਿਜ੍ਞਾ(fun);
      string str = fun->get_string();

      ਚੋਣ (code) {
	  ਹਾਲਤ vpi_ਮਾਪ:
	      // The vpiSize of a string variable is the number of
	      // bytes (characters) in that string.
	    ਵਾਪਸ str.size();
	  ਹਾਲਤ vpi_ਖੱਬਾ_Range:
	    ਵਾਪਸ 0;
	  ਹਾਲਤ vpi_ਸੱਜਾ_Range:
            ਵਾਪਸ str.size() - 1;
	  ਮੂਲ:
	    fprintf(stderr, "vpi sorry: property is not implemented");
	    ਪ੍ਰਤਿਜ੍ਞਾ(ਝੂਠ);
	    ਵਾਪਸ 0;
      }
}

ਖਾਲੀ __vpiStringVar::vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val)
{
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (get_net()->fun);
      ਪ੍ਰਤਿਜ੍ਞਾ(fun);
      string str = fun->get_string();

      ਜੇ (val->format == vpiStringVal || val->format == vpiObjTypeVal) {
	    ਅੱਖਰ*rbuf = static_cast<ਅੱਖਰ *>(need_ਨਤੀਜਾ_buf(str.size()+1, RBUF_VAL));
	    strcpy(rbuf, str.c_str());
	    val->format = vpiStringVal;
	    val->value.str = rbuf;
	    ਵਾਪਸ;
      }

      val->format = vpiSuppressVal;
}

vpiHandle __vpiStringVar::vpi_put_ਮੁੱਲ(p_vpi_ਮੁੱਲ val, ਪੂਰਨ)
{
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (get_net()->fun);
      ਪ੍ਰਤਿਜ੍ਞਾ(fun);
      ਪ੍ਰਤਿਜ੍ਞਾ(val->format == vpiStringVal);
      vvp_net_ptr_t dest (get_net(), 0);
      vvp_send_string(dest, val->value.str, vthread_get_wt_context());
      ਵਾਪਸ 0;
}

vpiHandle vpip_make_string_var(ਅਚਲ ਅੱਖਰ*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ਅਚਲ ਅੱਖਰ*use_name = name ? vpip_name_string(name) : ਨਿਰਾ;

      __vpiStringVar*obj = ਨਵਾਂ __vpiStringVar(scope, use_name, net);

      ਵਾਪਸ obj;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਖਾਲੀ string_delete(vpiHandle ਵਸਤੂ)
{
      __vpiStringVar*obj = dynamic_cast<__vpiStringVar*>(ਵਸਤੂ);
      ਮਿਟਾਓ obj;
}
#ਜੇ_ਅੰਤ
