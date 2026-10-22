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

ଧାଡ଼ିଭିତରେ __vpiNamedEvent::__vpiNamedEvent(__vpiScope*sc, ଅଚଳ ଅକ୍ଷର*nam)
{
      scope_ = sc;
      name_ = vpip_name_string(nam);
      callbacks_ = 0;
}

__vpiNamedEvent::~__vpiNamedEvent()
{
      ଯେତେବେଳେ (callbacks_) {
	    ସଂରଚନା __vpiCallback *tmp = callbacks_->next;
	    delete callbacks_;
	    callbacks_ = tmp;
      }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiNamedEvent::get_type_code(ଶୂନ୍ୟ) ଅଚଳ
{ ଫେରନ୍ତୁ vpiNamedEvent; }

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiNamedEvent::vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ବାଛ (code) {

	  ସ୍ଥିତି vpiAutomatic:
	    ଫେରନ୍ତୁ scope_->is_automatic()? 1 : 0;
      }

      ଫେରନ୍ତୁ 0;
}

ଅକ୍ଷର* __vpiNamedEvent::vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ଯଦି (code == vpiFile) {  // Not implemented for now!
	    ଫେରନ୍ତୁ simple_set_rbuf_str(file_names[0]);
      }
      ଫେରନ୍ତୁ generic_get_str(code, scope_, name_, ଖାଲି);
}

vpiHandle __vpiNamedEvent::vpi_put_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା)
{
	// p_vpi_value may be NULL, and an event doesn't care
	// what the value is
      vvp_vector4_t val;
      vvp_net_ptr_t dest(funct, 0);
      vvp_send_vec4(dest, val, vthread_get_wt_context());

      ଫେରନ୍ତୁ ଏହି;
}

vpiHandle __vpiNamedEvent::vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ବାଛ (code) {
	  ସ୍ଥିତି vpiScope:
	    ଫେରନ୍ତୁ scope_;

	  ସ୍ଥିତି vpiModule:
	    ଫେରନ୍ତୁ vpip_module(scope_);
      }

      ଫେରନ୍ତୁ 0;
}


vpiHandle vpip_make_named_event(ଅଚଳ ଅକ୍ଷର*name, vvp_net_t*funct)
{
      __vpiNamedEvent*obj = ନୂତନ __vpiNamedEvent(vpip_peek_current_scope(), name);

      obj->funct = funct;

      ଫେରନ୍ତୁ obj;
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
ଶୂନ୍ୟ __vpiNamedEvent::run_vpi_callbacks()
{
      ସଂରଚନା __vpiCallback*next = callbacks_;
      ସଂରଚନା __vpiCallback*prev = 0;
      ଯେତେବେଳେ (next) {
	    ସଂରଚନା __vpiCallback*cur = next;
	    next = cur->next;

	    ଯଦି (cur->cb_data.cb_rtn != 0) {
		  callback_execute(cur);
		  prev = cur;

	    } ଅନ୍ୟଥା ଯଦି (prev == 0) {
		  callbacks_ = next;
		  cur->next = 0;
		  delete cur;

	    } ଅନ୍ୟଥା {
		  ପ୍ରତିଜ୍ଞା(prev->next == cur);
		  prev->next = next;
		  cur->next = 0;
		  delete cur;
	    }
      }
}
