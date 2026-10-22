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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#ಆದರೆ_ಅಂತ್ಯ
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <cassert>
# include  "ivl_alloc.h"

ಬಳಸಿ ನಾಮಾಕಾಶ std;

__vpiStringVar::__vpiStringVar(__vpiScope*sc, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

ಪೂರ್ಣಾಂಕ __vpiStringVar::get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ
{ ಹಿಂತಿರುಗಿ vpiStringVar; }

ಪೂರ್ಣಾಂಕ __vpiStringVar::vpi_get(ಪೂರ್ಣಾಂಕ code)
{
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (get_net()->fun);
      ಪ್ರತಿಜ್ಞಾ(fun);
      string str = fun->get_string();

      ಆಯ್ಕೆ (code) {
	  ಸ್ಥಿತಿ vpi_ಗಾತ್ರ:
	      // The vpiSize of a string variable is the number of
	      // bytes (characters) in that string.
	    ಹಿಂತಿರುಗಿ str.size();
	  ಸ್ಥಿತಿ vpi_ಎಡ_Range:
	    ಹಿಂತಿರುಗಿ 0;
	  ಸ್ಥಿತಿ vpi_ಬಲ_Range:
            ಹಿಂತಿರುಗಿ str.size() - 1;
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "vpi sorry: property is not implemented");
	    ಪ್ರತಿಜ್ಞಾ(ಸುಳ್ಳು);
	    ಹಿಂತಿರುಗಿ 0;
      }
}

ಶೂನ್ಯ __vpiStringVar::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val)
{
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (get_net()->fun);
      ಪ್ರತಿಜ್ಞಾ(fun);
      string str = fun->get_string();

      ಆದರೆ (val->format == vpiStringVal || val->format == vpiObjTypeVal) {
	    ಅಕ್ಷರ*rbuf = static_cast<ಅಕ್ಷರ *>(need_ಫಲಿತಾಂಶ_buf(str.size()+1, RBUF_VAL));
	    strcpy(rbuf, str.c_str());
	    val->format = vpiStringVal;
	    val->value.str = rbuf;
	    ಹಿಂತಿರುಗಿ;
      }

      val->format = vpiSuppressVal;
}

vpiHandle __vpiStringVar::vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಂಕ)
{
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (get_net()->fun);
      ಪ್ರತಿಜ್ಞಾ(fun);
      ಪ್ರತಿಜ್ಞಾ(val->format == vpiStringVal);
      vvp_net_ptr_t dest (get_net(), 0);
      vvp_send_string(dest, val->value.str, vthread_get_wt_context());
      ಹಿಂತಿರುಗಿ 0;
}

vpiHandle vpip_make_string_var(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*use_name = name ? vpip_name_string(name) : ಖಾಲಿ;

      __vpiStringVar*obj = ಹೊಸ __vpiStringVar(scope, use_name, net);

      ಹಿಂತಿರುಗಿ obj;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಶೂನ್ಯ string_delete(vpiHandle ಅಂಶ)
{
      __vpiStringVar*obj = dynamic_cast<__vpiStringVar*>(ಅಂಶ);
      ಅಳಿಸು obj;
}
#ಆದರೆ_ಅಂತ್ಯ
