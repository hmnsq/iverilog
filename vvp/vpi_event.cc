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

અંતઃરેખ __vpiNamedEvent::__vpiNamedEvent(__vpiScope*sc, અચલ અક્ષર*nam)
{
      scope_ = sc;
      name_ = vpip_name_string(nam);
      callbacks_ = 0;
}

__vpiNamedEvent::~__vpiNamedEvent()
{
      જ્યારે (callbacks_) {
	    સંરચના __vpiCallback *tmp = callbacks_->next;
	    delete callbacks_;
	    callbacks_ = tmp;
      }
}

પૂર્ણાંક __vpiNamedEvent::get_type_code(શૂન્ય) અચલ
{ પાછા vpiNamedEvent; }

પૂર્ણાંક __vpiNamedEvent::vpi_get(પૂર્ણાંક code)
{
      પસંદગી (code) {

	  સ્થિતિ vpiAutomatic:
	    પાછા scope_->is_automatic()? 1 : 0;
      }

      પાછા 0;
}

અક્ષર* __vpiNamedEvent::vpi_get_str(પૂર્ણાંક code)
{
      જો (code == vpiFile) {  // Not implemented for now!
	    પાછા simple_set_rbuf_str(file_names[0]);
      }
      પાછા generic_get_str(code, scope_, name_, ખાલી);
}

vpiHandle __vpiNamedEvent::vpi_put_કિંમત(p_vpi_કિંમત, પૂર્ણાંક)
{
	// p_vpi_value may be NULL, and an event doesn't care
	// what the value is
      vvp_vector4_t val;
      vvp_net_ptr_t dest(funct, 0);
      vvp_send_vec4(dest, val, vthread_get_wt_context());

      પાછા આ;
}

vpiHandle __vpiNamedEvent::vpi_handle(પૂર્ણાંક code)
{
      પસંદગી (code) {
	  સ્થિતિ vpiScope:
	    પાછા scope_;

	  સ્થિતિ vpiModule:
	    પાછા vpip_module(scope_);
      }

      પાછા 0;
}


vpiHandle vpip_make_named_event(અચલ અક્ષર*name, vvp_net_t*funct)
{
      __vpiNamedEvent*obj = નવું __vpiNamedEvent(vpip_peek_current_scope(), name);

      obj->funct = funct;

      પાછા obj;
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
શૂન્ય __vpiNamedEvent::run_vpi_callbacks()
{
      સંરચના __vpiCallback*next = callbacks_;
      સંરચના __vpiCallback*prev = 0;
      જ્યારે (next) {
	    સંરચના __vpiCallback*cur = next;
	    next = cur->next;

	    જો (cur->cb_data.cb_rtn != 0) {
		  callback_execute(cur);
		  prev = cur;

	    } નહીંતર જો (prev == 0) {
		  callbacks_ = next;
		  cur->next = 0;
		  delete cur;

	    } નહીંતર {
		  પ્રતિજ્ઞા(prev->next == cur);
		  prev->next = next;
		  cur->next = 0;
		  delete cur;
	    }
      }
}
