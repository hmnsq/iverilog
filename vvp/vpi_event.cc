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

اندرونی __vpiNamedEvent::__vpiNamedEvent(__vpiScope*sc, ثابت حرف*nam)
{
      scope_ = sc;
      name_ = vpip_name_string(nam);
      callbacks_ = 0;
}

__vpiNamedEvent::~__vpiNamedEvent()
{
      جب (callbacks_) {
	    ساخت __vpiCallback *tmp = callbacks_->اگلا_2;
	    حذف callbacks_;
	    callbacks_ = tmp;
      }
}

صحیح __vpiNamedEvent::get_type_رمز(خالی) ثابت
{ واپس vpiNamedEvent; }

صحیح __vpiNamedEvent::vpi_get(صحیح code)
{
      انتخاب (code) {

	  حالت vpiAutomatic:
	    واپس scope_->is_automatic()? 1 : 0;
      }

      واپس 0;
}

حرف* __vpiNamedEvent::vpi_get_str(صحیح code)
{
      اگر (code == vpiFile) {  // Not implemented for now!
	    واپس simple_سیٹ_rbuf_str(file_نام[0]);
      }
      واپس generic_get_str(code, scope_, name_, تہی);
}

vpiHandle __vpiNamedEvent::vpi_put_قدر(p_vpi_قدر, صحیح)
{
	// p_vpi_value may be NULL, and an event doesn't care
	// what the value is
      vvp_vector4_t val;
      vvp_net_ptr_t dest(funct, 0);
      vvp_send_vec4(dest, val, vthread_get_wt_context());

      واپس یہ;
}

vpiHandle __vpiNamedEvent::vpi_handle(صحیح code)
{
      انتخاب (code) {
	  حالت vpiScope:
	    واپس scope_;

	  حالت vpiModule:
	    واپس vpip_module(scope_);
      }

      واپس 0;
}


vpiHandle vpip_make_named_event(ثابت حرف*name, vvp_net_t*funct)
{
      __vpiNamedEvent*obj = نیا __vpiNamedEvent(vpip_peek_current_scope(), name);

      obj->funct = funct;

      واپس obj;
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
خالی __vpiNamedEvent::تسلسل_vpi_callbacks()
{
      ساخت __vpiCallback*اگلا_2 = callbacks_;
      ساخت __vpiCallback*prev = 0;
      جب (اگلا_2) {
	    ساخت __vpiCallback*cur = اگلا_2;
	    اگلا_2 = cur->اگلا_2;

	    اگر (cur->cb_ڈیٹا.cb_rtn != 0) {
		  callback_execute(cur);
		  prev = cur;

	    } ورنہ اگر (prev == 0) {
		  callbacks_ = اگلا_2;
		  cur->اگلا_2 = 0;
		  حذف cur;

	    } ورنہ {
		  أكد(prev->اگلا_2 == cur);
		  prev->اگلا_2 = اگلا_2;
		  cur->اگلا_2 = 0;
		  حذف cur;
	    }
      }
}
