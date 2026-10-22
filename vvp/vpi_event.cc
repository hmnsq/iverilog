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

अन्तःस्थ __vpiNamedEvent::__vpiNamedEvent(__vpiScope*sc, अचर वर्ण*nam)
{
      scope_ = sc;
      name_ = vpip_name_string(nam);
      callbacks_ = 0;
}

__vpiNamedEvent::~__vpiNamedEvent()
{
      जबसम्म (callbacks_) {
	    संरचना __vpiCallback *tmp = callbacks_->next;
	    delete callbacks_;
	    callbacks_ = tmp;
      }
}

पूर्णाङ्क __vpiNamedEvent::get_type_code(शून्य) अचर
{ फर्काऊ vpiNamedEvent; }

पूर्णाङ्क __vpiNamedEvent::vpi_get(पूर्णाङ्क code)
{
      विकल्प (code) {

	  पक्ष vpiAutomatic:
	    फर्काऊ scope_->is_automatic()? 1 : 0;
      }

      फर्काऊ 0;
}

वर्ण* __vpiNamedEvent::vpi_get_str(पूर्णाङ्क code)
{
      यदि (code == vpiFile) {  // Not implemented for now!
	    फर्काऊ simple_set_rbuf_str(file_names[0]);
      }
      फर्काऊ generic_get_str(code, scope_, name_, रिक्त);
}

vpiHandle __vpiNamedEvent::vpi_put_मान(p_vpi_मान, पूर्णाङ्क)
{
	// p_vpi_value may be NULL, and an event doesn't care
	// what the value is
      vvp_vector4_t val;
      vvp_net_ptr_t dest(funct, 0);
      vvp_send_vec4(dest, val, vthread_get_wt_context());

      फर्काऊ एतत्;
}

vpiHandle __vpiNamedEvent::vpi_handle(पूर्णाङ्क code)
{
      विकल्प (code) {
	  पक्ष vpiScope:
	    फर्काऊ scope_;

	  पक्ष vpiModule:
	    फर्काऊ vpip_module(scope_);
      }

      फर्काऊ 0;
}


vpiHandle vpip_make_named_event(अचर वर्ण*name, vvp_net_t*funct)
{
      __vpiNamedEvent*obj = नूतन __vpiNamedEvent(vpip_peek_current_scope(), name);

      obj->funct = funct;

      फर्काऊ obj;
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
शून्य __vpiNamedEvent::run_vpi_callbacks()
{
      संरचना __vpiCallback*next = callbacks_;
      संरचना __vpiCallback*prev = 0;
      जबसम्म (next) {
	    संरचना __vpiCallback*cur = next;
	    next = cur->next;

	    यदि (cur->cb_data.cb_rtn != 0) {
		  callback_execute(cur);
		  prev = cur;

	    } अन्यथा यदि (prev == 0) {
		  callbacks_ = next;
		  cur->next = 0;
		  delete cur;

	    } अन्यथा {
		  प्रतिज्ञा(prev->next == cur);
		  prev->next = next;
		  cur->next = 0;
		  delete cur;
	    }
      }
}
