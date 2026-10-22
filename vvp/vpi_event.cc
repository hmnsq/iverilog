/*
 * Copyright (c) 2002-2017 Stephen Williams (steve@icarus.com)
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
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <cassert>
# include  "ivl_alloc.h"

ಅನ್ತಃಸ್ಥ __vpiNamedEvent::__vpiNamedEvent(__vpiScope*sc, ಧ್ರುವ ವರ್ಣ*nam)
{
      scope_ = sc;
      name_ = vpip_name_string(nam);
      callbacks_ = 0;
}

__vpiNamedEvent::~__vpiNamedEvent()
{
      ಯಾವತ್ (callbacks_) {
	    ಸಂರಚನಾ __vpiCallback *tmp = callbacks_->next;
	    delete callbacks_;
	    callbacks_ = tmp;
      }
}

ಪೂರ್ಣಾಙ್ಕ __vpiNamedEvent::get_type_code(ಶೂನ್ಯ) ಧ್ರುವ
{ ಪ್ರತ್ಯಾಗಮ vpiNamedEvent; }

ಪೂರ್ಣಾಙ್ಕ __vpiNamedEvent::vpi_get(ಪೂರ್ಣಾಙ್ಕ code)
{
      ವಿಕಲ್ಪ (code) {

	  ಪಕ್ಷ vpiAutomatic:
	    ಪ್ರತ್ಯಾಗಮ scope_->is_automatic()? 1 : 0;
      }

      ಪ್ರತ್ಯಾಗಮ 0;
}

ವರ್ಣ* __vpiNamedEvent::vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code)
{
      ಯದಿ (code == vpiFile) {  // Not implemented for now!
	    ಪ್ರತ್ಯಾಗಮ simple_set_rbuf_str(file_names[0]);
      }
      ಪ್ರತ್ಯಾಗಮ generic_get_str(code, scope_, name_, ರಿಕ್ತ);
}

vpiHandle __vpiNamedEvent::vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ, ಪೂರ್ಣಾಙ್ಕ)
{
	// p_vpi_value may be NULL, and an event doesn't care
	// what the value is
      vvp_vector4_t val;
      vvp_net_ptr_t dest(funct, 0);
      vvp_send_vec4(dest, val, vthread_get_wt_context());

      ಪ್ರತ್ಯಾಗಮ ಏತತ್;
}

vpiHandle __vpiNamedEvent::vpi_handle(ಪೂರ್ಣಾಙ್ಕ code)
{
      ವಿಕಲ್ಪ (code) {
	  ಪಕ್ಷ vpiScope:
	    ಪ್ರತ್ಯಾಗಮ scope_;

	  ಪಕ್ಷ vpiModule:
	    ಪ್ರತ್ಯಾಗಮ vpip_module(scope_);
      }

      ಪ್ರತ್ಯಾಗಮ 0;
}


vpiHandle vpip_make_named_event(ಧ್ರುವ ವರ್ಣ*name, vvp_net_t*funct)
{
      __vpiNamedEvent*obj = ನೂತನ __vpiNamedEvent(vpip_peek_current_scope(), name);

      obj->funct = funct;

      ಪ್ರತ್ಯಾಗಮ obj;
}

/*
 * This function runs the callbacks for a named event. All the
 * callbacks are listed in the callback member of the event handle,
 * this function scans that list.
 *
 * This also handles the case where the callback has been removed. The
 * vpi_remove_cb doesn't actually remove any callbacks, it marks them
 * as canceled by clearing the cb_rtn function. This function reaps
 * those marked handles when it scans the list.
 *
 * We can not use vpi_free_object() here since it does not really
 * delete the callback.
 */
ಶೂನ್ಯ __vpiNamedEvent::run_vpi_callbacks()
{
      ಸಂರಚನಾ __vpiCallback*next = callbacks_;
      ಸಂರಚನಾ __vpiCallback*prev = 0;
      ಯಾವತ್ (next) {
	    ಸಂರಚನಾ __vpiCallback*cur = next;
	    next = cur->next;

	    ಯದಿ (cur->cb_data.cb_rtn != 0) {
		  callback_execute(cur);
		  prev = cur;

	    } ಅನ್ಯಥಾ ಯದಿ (prev == 0) {
		  callbacks_ = next;
		  cur->next = 0;
		  delete cur;

	    } ಅನ್ಯಥಾ {
		  ಪ್ರತಿಜ್ಞಾ(prev->next == cur);
		  prev->next = next;
		  cur->next = 0;
		  delete cur;
	    }
      }
}
