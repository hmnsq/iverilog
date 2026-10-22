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

using நாமாகாஶ std;

__vpiStringVar::__vpiStringVar(__vpiScope*sc, மாறிலி எழுத்து*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

முழு __vpiStringVar::get_type_code(வெறுமை) மாறிலி
{ திரும்பு vpiStringVar; }

முழு __vpiStringVar::vpi_get(முழு code)
{
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (get_net()->fun);
      ப்ரதிஜ்ஞா(fun);
      string str = fun->get_string();

      தேர்வு (code) {
	  நிலை vpiSize:
	      // The vpiSize of a string variable is the number of
	      // bytes (characters) in that string.
	    திரும்பு str.size();
	  நிலை vpi_இடது_Range:
	    திரும்பு 0;
	  நிலை vpi_வலது_Range:
            திரும்பு str.size() - 1;
	  இயல்பு:
	    fprintf(stderr, "vpi sorry: property is not implemented");
	    ப்ரதிஜ்ஞா(பொய்);
	    திரும்பு 0;
      }
}

வெறுமை __vpiStringVar::vpi_get_மதிப்பு(p_vpi_மதிப்பு val)
{
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (get_net()->fun);
      ப்ரதிஜ்ஞா(fun);
      string str = fun->get_string();

      எனில் (val->format == vpiStringVal || val->format == vpiObjTypeVal) {
	    எழுத்து*rbuf = static_cast<எழுத்து *>(need_result_buf(str.size()+1, RBUF_VAL));
	    strcpy(rbuf, str.c_str());
	    val->format = vpiStringVal;
	    val->value.str = rbuf;
	    திரும்பு;
      }

      val->format = vpiSuppressVal;
}

vpiHandle __vpiStringVar::vpi_put_மதிப்பு(p_vpi_மதிப்பு val, முழு)
{
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (get_net()->fun);
      ப்ரதிஜ்ஞா(fun);
      ப்ரதிஜ்ஞா(val->format == vpiStringVal);
      vvp_net_ptr_t dest (get_net(), 0);
      vvp_send_string(dest, val->value.str, vthread_get_wt_context());
      திரும்பு 0;
}

vpiHandle vpip_make_string_var(மாறிலி எழுத்து*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      மாறிலி எழுத்து*use_name = name ? vpip_name_string(name) : வெற்று;

      __vpiStringVar*obj = புதிய __vpiStringVar(scope, use_name, net);

      திரும்பு obj;
}

#ifdef CHECK_WITH_VALGRIND
வெறுமை string_delete(vpiHandle item)
{
      __vpiStringVar*obj = dynamic_cast<__vpiStringVar*>(item);
      delete obj;
}
#endif
