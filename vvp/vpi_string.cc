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

using নামাকাশ std;

__vpiStringVar::__vpiStringVar(__vpiScope*sc, ধ্রুব বর্ণ*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

পূর্ণাঙ্ক __vpiStringVar::get_type_code(শূন্য) ধ্রুব
{ প্রত্যাগম vpiStringVar; }

পূর্ণাঙ্ক __vpiStringVar::vpi_get(পূর্ণাঙ্ক code)
{
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (get_net()->fun);
      প্রতিজ্ঞা(fun);
      string str = fun->get_string();

      বিকল্প (code) {
	  পক্ষ vpiSize:
	      // The vpiSize of a string variable is the number of
	      // bytes (characters) in that string.
	    প্রত্যাগম str.size();
	  পক্ষ vpi_বাম_Range:
	    প্রত্যাগম 0;
	  পক্ষ vpi_ডান_Range:
            প্রত্যাগম str.size() - 1;
	  স্বভাব:
	    fprintf(stderr, "vpi sorry: property is not implemented");
	    প্রতিজ্ঞা(অসত্য);
	    প্রত্যাগম 0;
      }
}

শূন্য __vpiStringVar::vpi_get_মান(p_vpi_মান val)
{
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (get_net()->fun);
      প্রতিজ্ঞা(fun);
      string str = fun->get_string();

      যদি (val->format == vpiStringVal || val->format == vpiObjTypeVal) {
	    বর্ণ*rbuf = static_cast<বর্ণ *>(need_result_buf(str.size()+1, RBUF_VAL));
	    strcpy(rbuf, str.c_str());
	    val->format = vpiStringVal;
	    val->value.str = rbuf;
	    প্রত্যাগম;
      }

      val->format = vpiSuppressVal;
}

vpiHandle __vpiStringVar::vpi_put_মান(p_vpi_মান val, পূর্ণাঙ্ক)
{
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (get_net()->fun);
      প্রতিজ্ঞা(fun);
      প্রতিজ্ঞা(val->format == vpiStringVal);
      vvp_net_ptr_t dest (get_net(), 0);
      vvp_send_string(dest, val->value.str, vthread_get_wt_context());
      প্রত্যাগম 0;
}

vpiHandle vpip_make_string_var(ধ্রুব বর্ণ*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ধ্রুব বর্ণ*use_name = name ? vpip_name_string(name) : রিক্ত;

      __vpiStringVar*obj = নূতন __vpiStringVar(scope, use_name, net);

      প্রত্যাগম obj;
}

#ifdef CHECK_WITH_VALGRIND
শূন্য string_delete(vpiHandle item)
{
      __vpiStringVar*obj = dynamic_cast<__vpiStringVar*>(item);
      delete obj;
}
#endif
