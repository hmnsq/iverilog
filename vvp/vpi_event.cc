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

anta_hstha __vpiNamedEvent::__vpiNamedEvent(__vpiScope*sc, const var_na*nam)
{
      scope_ = sc;
      name_ = vpip_name_string(nam);
      callbacks_ = 0;
}

__vpiNamedEvent::~__vpiNamedEvent()
{
      ᰀᰬ (callbacks_) {
	    sa_nracanaa __vpiCallback *tmp = callbacks_->next;
	    delete callbacks_;
	    callbacks_ = tmp;
      }
}

puur_naa_nka __vpiNamedEvent::get_type_code(void) const
{ ᰀᰧᰌ vpiNamedEvent; }

puur_naa_nka __vpiNamedEvent::vpi_get(puur_naa_nka code)
{
      switch (code) {

	  pak_sa vpiAutomatic:
	    ᰀᰧᰌ scope_->is_automatic()? 1 : 0;
      }

      ᰀᰧᰌ 0;
}

var_na* __vpiNamedEvent::vpi_get_str(puur_naa_nka code)
{
      ᰀᰧ (code == vpiFile) {  // Not implemented for now!
	    ᰀᰧᰌ simple_set_rbuf_str(file_names[0]);
      }
      ᰀᰧᰌ generic_get_str(code, scope_, name_, NULL);
}

vpiHandle __vpiNamedEvent::vpi_put_value(p_vpi_value, puur_naa_nka)
{
	// p_vpi_value may be NULL, and an event doesn't care
	// what the value is
      vvp_vector4_t val;
      vvp_net_ptr_t dest(funct, 0);
      vvp_send_vec4(dest, val, vthread_get_wt_context());

      ᰀᰧᰌ this;
}

vpiHandle __vpiNamedEvent::vpi_handle(puur_naa_nka code)
{
      switch (code) {
	  pak_sa vpiScope:
	    ᰀᰧᰌ scope_;

	  pak_sa vpiModule:
	    ᰀᰧᰌ vpip_module(scope_);
      }

      ᰀᰧᰌ 0;
}


vpiHandle vpip_make_named_event(const var_na*name, vvp_net_t*funct)
{
      __vpiNamedEvent*obj = new __vpiNamedEvent(vpip_peek_current_scope(), name);

      obj->funct = funct;

      ᰀᰧᰌ obj;
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
      sa_nracanaa __vpiCallback*next = callbacks_;
      sa_nracanaa __vpiCallback*prev = 0;
      ᰀᰬ (next) {
	    sa_nracanaa __vpiCallback*cur = next;
	    next = cur->next;

	    ᰀᰧ (cur->cb_data.cb_rtn != 0) {
		  callback_execute(cur);
		  prev = cur;

	    } ᰀᰧᰀ ᰀᰧ (prev == 0) {
		  callbacks_ = next;
		  cur->next = 0;
		  delete cur;

	    } ᰀᰧᰀ {
		  assert(prev->next == cur);
		  prev->next = next;
		  cur->next = 0;
		  delete cur;
	    }
      }
}
