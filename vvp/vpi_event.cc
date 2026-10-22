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

ਸਤਰਅੰਦਰ __vpiNamedEvent::__vpiNamedEvent(__vpiScope*sc, ਅਚਲ ਅੱਖਰ*nam)
{
      scope_ = sc;
      name_ = vpip_name_string(nam);
      callbacks_ = 0;
}

__vpiNamedEvent::~__vpiNamedEvent()
{
      ਜਦੋਂ (callbacks_) {
	    ਬਣਤਰ __vpiCallback *tmp = callbacks_->ਅਗਲਾ_2;
	    ਮਿਟਾਓ callbacks_;
	    callbacks_ = tmp;
      }
}

ਪੂਰਨ __vpiNamedEvent::get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ
{ ਵਾਪਸ vpiNamedEvent; }

ਪੂਰਨ __vpiNamedEvent::vpi_get(ਪੂਰਨ code)
{
      ਚੋਣ (code) {

	  ਹਾਲਤ vpiAutomatic:
	    ਵਾਪਸ scope_->is_automatic()? 1 : 0;
      }

      ਵਾਪਸ 0;
}

ਅੱਖਰ* __vpiNamedEvent::vpi_get_str(ਪੂਰਨ code)
{
      ਜੇ (code == vpiFile) {  // Not implemented for now!
	    ਵਾਪਸ simple_ਸੈੱਟ_rbuf_str(file_ਨਾਂ[0]);
      }
      ਵਾਪਸ generic_get_str(code, scope_, name_, ਨਿਰਾ);
}

vpiHandle __vpiNamedEvent::vpi_put_ਮੁੱਲ(p_vpi_ਮੁੱਲ, ਪੂਰਨ)
{
	// p_vpi_value may be NULL, and an event doesn't care
	// what the value is
      vvp_vector4_t val;
      vvp_net_ptr_t dest(funct, 0);
      vvp_send_vec4(dest, val, vthread_get_wt_context());

      ਵਾਪਸ ਇਹ;
}

vpiHandle __vpiNamedEvent::vpi_handle(ਪੂਰਨ code)
{
      ਚੋਣ (code) {
	  ਹਾਲਤ vpiScope:
	    ਵਾਪਸ scope_;

	  ਹਾਲਤ vpiModule:
	    ਵਾਪਸ vpip_module(scope_);
      }

      ਵਾਪਸ 0;
}


vpiHandle vpip_make_named_event(ਅਚਲ ਅੱਖਰ*name, vvp_net_t*funct)
{
      __vpiNamedEvent*obj = ਨਵਾਂ __vpiNamedEvent(vpip_peek_current_scope(), name);

      obj->funct = funct;

      ਵਾਪਸ obj;
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
ਖਾਲੀ __vpiNamedEvent::ਲੜੀਵਾਰ_vpi_callbacks()
{
      ਬਣਤਰ __vpiCallback*ਅਗਲਾ_2 = callbacks_;
      ਬਣਤਰ __vpiCallback*prev = 0;
      ਜਦੋਂ (ਅਗਲਾ_2) {
	    ਬਣਤਰ __vpiCallback*cur = ਅਗਲਾ_2;
	    ਅਗਲਾ_2 = cur->ਅਗਲਾ_2;

	    ਜੇ (cur->cb_ਅੰਕੜੇ.cb_rtn != 0) {
		  callback_execute(cur);
		  prev = cur;

	    } ਨਹੀਂਤਾਂ ਜੇ (prev == 0) {
		  callbacks_ = ਅਗਲਾ_2;
		  cur->ਅਗਲਾ_2 = 0;
		  ਮਿਟਾਓ cur;

	    } ਨਹੀਂਤਾਂ {
		  ਪ੍ਰਤਿਜ੍ਞਾ(prev->ਅਗਲਾ_2 == cur);
		  prev->ਅਗਲਾ_2 = ਅਗਲਾ_2;
		  cur->ਅਗਲਾ_2 = 0;
		  ਮਿਟਾਓ cur;
	    }
      }
}
