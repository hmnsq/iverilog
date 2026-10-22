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

using فضاینام std;

__vpiStringVar::__vpiStringVar(__vpiScope*sc, ثابت نویسه*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

صحیح __vpiStringVar::get_type_code(تهی) ثابت
{ بازگشت vpiStringVar; }

صحیح __vpiStringVar::vpi_get(صحیح code)
{
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (get_net()->fun);
      أكد(fun);
      string str = fun->get_string();

      انتخاب (code) {
	  حالت vpiSize:
	      // The vpiSize of a string variable is the number of
	      // bytes (characters) in that string.
	    بازگشت str.size();
	  حالت vpi_چپ_Range:
	    بازگشت 0;
	  حالت vpi_راست_Range:
            بازگشت str.size() - 1;
	  پیشفرض:
	    fprintf(stderr, "vpi sorry: property is not implemented");
	    أكد(نادرست);
	    بازگشت 0;
      }
}

تهی __vpiStringVar::vpi_get_مقدار(p_vpi_مقدار val)
{
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (get_net()->fun);
      أكد(fun);
      string str = fun->get_string();

      اگر (val->format == vpiStringVal || val->format == vpiObjTypeVal) {
	    نویسه*rbuf = static_cast<نویسه *>(need_result_buf(str.size()+1, RBUF_VAL));
	    strcpy(rbuf, str.c_str());
	    val->format = vpiStringVal;
	    val->value.str = rbuf;
	    بازگشت;
      }

      val->format = vpiSuppressVal;
}

vpiHandle __vpiStringVar::vpi_put_مقدار(p_vpi_مقدار val, صحیح)
{
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (get_net()->fun);
      أكد(fun);
      أكد(val->format == vpiStringVal);
      vvp_net_ptr_t dest (get_net(), 0);
      vvp_send_string(dest, val->value.str, vthread_get_wt_context());
      بازگشت 0;
}

vpiHandle vpip_make_string_var(ثابت نویسه*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ثابت نویسه*use_name = name ? vpip_name_string(name) : پوچ;

      __vpiStringVar*obj = جدید __vpiStringVar(scope, use_name, net);

      بازگشت obj;
}

#ifdef CHECK_WITH_VALGRIND
تهی string_delete(vpiHandle item)
{
      __vpiStringVar*obj = dynamic_cast<__vpiStringVar*>(item);
      delete obj;
}
#endif
