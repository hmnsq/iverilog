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

using ನಾಮಾಕಾಶ std;

__vpiStringVar::__vpiStringVar(__vpiScope*sc, ಧ್ರುವ ವರ್ಣ*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

ಪೂರ್ಣಾಙ್ಕ __vpiStringVar::get_type_code(ಶೂನ್ಯ) ಧ್ರುವ
{ ಪ್ರತ್ಯಾಗಮ vpiStringVar; }

ಪೂರ್ಣಾಙ್ಕ __vpiStringVar::vpi_get(ಪೂರ್ಣಾಙ್ಕ code)
{
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (get_net()->fun);
      ಪ್ರತಿಜ್ಞಾ(fun);
      string str = fun->get_string();

      ವಿಕಲ್ಪ (code) {
	  ಪಕ್ಷ vpiSize:
	      // The vpiSize of a string variable is the number of
	      // bytes (characters) in that string.
	    ಪ್ರತ್ಯಾಗಮ str.size();
	  ಪಕ್ಷ vpi_ಎಡ_Range:
	    ಪ್ರತ್ಯಾಗಮ 0;
	  ಪಕ್ಷ vpi_ಬಲ_Range:
            ಪ್ರತ್ಯಾಗಮ str.size() - 1;
	  ಸ್ವಭಾವ:
	    fprintf(stderr, "vpi sorry: property is not implemented");
	    ಪ್ರತಿಜ್ಞಾ(ಅಸತ್ಯ);
	    ಪ್ರತ್ಯಾಗಮ 0;
      }
}

ಶೂನ್ಯ __vpiStringVar::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val)
{
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (get_net()->fun);
      ಪ್ರತಿಜ್ಞಾ(fun);
      string str = fun->get_string();

      ಯದಿ (val->format == vpiStringVal || val->format == vpiObjTypeVal) {
	    ವರ್ಣ*rbuf = static_cast<ವರ್ಣ *>(need_result_buf(str.size()+1, RBUF_VAL));
	    strcpy(rbuf, str.c_str());
	    val->format = vpiStringVal;
	    val->value.str = rbuf;
	    ಪ್ರತ್ಯಾಗಮ;
      }

      val->format = vpiSuppressVal;
}

vpiHandle __vpiStringVar::vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಙ್ಕ)
{
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (get_net()->fun);
      ಪ್ರತಿಜ್ಞಾ(fun);
      ಪ್ರತಿಜ್ಞಾ(val->format == vpiStringVal);
      vvp_net_ptr_t dest (get_net(), 0);
      vvp_send_string(dest, val->value.str, vthread_get_wt_context());
      ಪ್ರತ್ಯಾಗಮ 0;
}

vpiHandle vpip_make_string_var(ಧ್ರುವ ವರ್ಣ*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ಧ್ರುವ ವರ್ಣ*use_name = name ? vpip_name_string(name) : ರಿಕ್ತ;

      __vpiStringVar*obj = ನೂತನ __vpiStringVar(scope, use_name, net);

      ಪ್ರತ್ಯಾಗಮ obj;
}

#ifdef CHECK_WITH_VALGRIND
ಶೂನ್ಯ string_delete(vpiHandle item)
{
      __vpiStringVar*obj = dynamic_cast<__vpiStringVar*>(item);
      delete obj;
}
#endif
