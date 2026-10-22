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

inline __vpiNamedEvent::__vpiNamedEvent(__vpiScope*sc, const char*nam)
{
      scope_ = sc;
      name_ = vpip_name_string(nam);
      callbacks_ = 0;
}

__vpiNamedEvent::~__vpiNamedEvent()
{
      trong_khi (callbacks_) {
	    struct __vpiCallback *tmp = callbacks_->next;
	    delete callbacks_;
	    callbacks_ = tmp;
      }
}

số_nguyên __vpiNamedEvent::get_type_code(void) const
{ trả_về vpiNamedEvent; }

số_nguyên __vpiNamedEvent::vpi_get(số_nguyên code)
{
      switch (code) {

	  case vpiAutomatic:
	    trả_về scope_->is_automatic()? 1 : 0;
      }

      trả_về 0;
}

char* __vpiNamedEvent::vpi_get_str(số_nguyên code)
{
      nếu (code == vpiFile) {  // Not implemented for now!
	    trả_về simple_set_rbuf_str(file_names[0]);
      }
      trả_về generic_get_str(code, scope_, name_, NULL);
}

vpiHandle __vpiNamedEvent::vpi_put_value(p_vpi_value, số_nguyên)
{
	// p_vpi_value may be NULL, and an event doesn't care
	// what the value is
      vvp_vector4_t val;
      vvp_net_ptr_t dest(funct, 0);
      vvp_send_vec4(dest, val, vthread_get_wt_context());

      trả_về this;
}

vpiHandle __vpiNamedEvent::vpi_handle(số_nguyên code)
{
      switch (code) {
	  case vpiScope:
	    trả_về scope_;

	  case vpiModule:
	    trả_về vpip_module(scope_);
      }

      trả_về 0;
}


vpiHandle vpip_make_named_event(const char*name, vvp_net_t*funct)
{
      __vpiNamedEvent*obj = new __vpiNamedEvent(vpip_peek_current_scope(), name);

      obj->funct = funct;

      trả_về obj;
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
void __vpiNamedEvent::run_vpi_callbacks()
{
      struct __vpiCallback*next = callbacks_;
      struct __vpiCallback*prev = 0;
      trong_khi (next) {
	    struct __vpiCallback*cur = next;
	    next = cur->next;

	    nếu (cur->cb_data.cb_rtn != 0) {
		  callback_execute(cur);
		  prev = cur;

	    } không_thì nếu (prev == 0) {
		  callbacks_ = next;
		  cur->next = 0;
		  delete cur;

	    } không_thì {
		  assert(prev->next == cur);
		  prev->next = next;
		  cur->next = 0;
		  delete cur;
	    }
      }
}
