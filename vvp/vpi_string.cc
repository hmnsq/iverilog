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
#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#جيڪڏهن_پورو
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <cassert>
# include  "ivl_alloc.h"

استعمال نطاق std;

__vpiStringVar::__vpiStringVar(__vpiScope*sc, ثابت حرف*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

صحيح __vpiStringVar::get_type_ڪوڊ(فراغ) ثابت
{ أرجع vpiStringVar; }

صحيح __vpiStringVar::vpi_get(صحيح code)
{
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (get_net()->fun);
      أكد(fun);
      string str = fun->get_string();

      بدل (code) {
	  حالة vpi_سائيز:
	      // The vpiSize of a string variable is the number of
	      // bytes (characters) in that string.
	    أرجع str.size();
	  حالة vpi_کاٻو_Range:
	    أرجع 0;
	  حالة vpi_ساڄو_Range:
            أرجع str.size() - 1;
	  افتراضي:
	    fprintf(stderr, "vpi sorry: property is not implemented");
	    أكد(خطأ);
	    أرجع 0;
      }
}

فراغ __vpiStringVar::vpi_get_قدر(p_vpi_قدر val)
{
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (get_net()->fun);
      أكد(fun);
      string str = fun->get_string();

      إذا (val->format == vpiStringVal || val->format == vpiObjTypeVal) {
	    حرف*rbuf = static_cast<حرف *>(need_نتيجو_buf(str.size()+1, RBUF_VAL));
	    strcpy(rbuf, str.c_str());
	    val->format = vpiStringVal;
	    val->value.str = rbuf;
	    أرجع;
      }

      val->format = vpiSuppressVal;
}

vpiHandle __vpiStringVar::vpi_put_قدر(p_vpi_قدر val, صحيح)
{
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (get_net()->fun);
      أكد(fun);
      أكد(val->format == vpiStringVal);
      vvp_net_ptr_t dest (get_net(), 0);
      vvp_send_string(dest, val->value.str, vthread_get_wt_context());
      أرجع 0;
}

vpiHandle vpip_make_string_var(ثابت حرف*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ثابت حرف*use_name = name ? vpip_name_string(name) : عدم;

      __vpiStringVar*obj = جديد __vpiStringVar(scope, use_name, net);

      أرجع obj;
}

#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
فراغ string_delete(vpiHandle شي)
{
      __vpiStringVar*obj = dynamic_cast<__vpiStringVar*>(شي);
      ميٽيو obj;
}
#جيڪڏهن_پورو
