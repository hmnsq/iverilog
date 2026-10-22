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

using नामाकाश std;

__vpiStringVar::__vpiStringVar(__vpiScope*sc, ध्रुव वर्ण*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

पूर्णाङ्क __vpiStringVar::get_type_code(शून्य) ध्रुव
{ प्रत्यागम vpiStringVar; }

पूर्णाङ्क __vpiStringVar::vpi_get(पूर्णाङ्क code)
{
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (get_net()->fun);
      प्रतिज्ञा(fun);
      string str = fun->get_string();

      विकल्प (code) {
	  पक्ष vpiSize:
	      // The vpiSize of a string variable is the number of
	      // bytes (characters) in that string.
	    प्रत्यागम str.size();
	  पक्ष vpi_बायाँ_Range:
	    प्रत्यागम 0;
	  पक्ष vpi_दायाँ_Range:
            प्रत्यागम str.size() - 1;
	  स्वभाव:
	    ख_लिखो(मानक_त्रुटि, "vpi sorry: property is not implemented");
	    प्रतिज्ञा(असत्य);
	    प्रत्यागम 0;
      }
}

शून्य __vpiStringVar::vpi_get_मूल्य(p_vpi_मूल्य val)
{
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (get_net()->fun);
      प्रतिज्ञा(fun);
      string str = fun->get_string();

      यदि (val->format == vpiStringVal || val->format == vpiObjTypeVal) {
	    वर्ण*rbuf = static_cast<वर्ण *>(need_result_buf(str.size()+1, RBUF_VAL));
	    म_नकल(rbuf, str.c_str());
	    val->format = vpiStringVal;
	    val->value.str = rbuf;
	    प्रत्यागम;
      }

      val->format = vpiSuppressVal;
}

vpiHandle __vpiStringVar::vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णाङ्क)
{
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (get_net()->fun);
      प्रतिज्ञा(fun);
      प्रतिज्ञा(val->format == vpiStringVal);
      vvp_net_ptr_t dest (get_net(), 0);
      vvp_send_string(dest, val->value.str, vthread_get_wt_context());
      प्रत्यागम 0;
}

vpiHandle vpip_make_string_var(ध्रुव वर्ण*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ध्रुव वर्ण*use_name = name ? vpip_name_string(name) : रिक्त;

      __vpiStringVar*obj = नूतन __vpiStringVar(scope, use_name, net);

      प्रत्यागम obj;
}

#ifdef CHECK_WITH_VALGRIND
शून्य string_delete(vpiHandle item)
{
      __vpiStringVar*obj = dynamic_cast<__vpiStringVar*>(item);
      delete obj;
}
#endif
