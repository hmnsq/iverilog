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

అంతఃవరుస __vpiNamedEvent::__vpiNamedEvent(__vpiScope*sc, స్థిరరాశి అక్షరం*nam)
{
      scope_ = sc;
      name_ = vpip_name_string(nam);
      callbacks_ = 0;
}

__vpiNamedEvent::~__vpiNamedEvent()
{
      అయితే (callbacks_) {
	    నిర్మాణం __vpiCallback *tmp = callbacks_->తదుపరి_2;
	    తొలగించు callbacks_;
	    callbacks_ = tmp;
      }
}

పూర్ణాంకం __vpiNamedEvent::get_type_సంకేతం(శూన్య) స్థిరరాశి
{ తిరిగి vpiNamedEvent; }

పూర్ణాంకం __vpiNamedEvent::vpi_get(పూర్ణాంకం code)
{
      ఎంపిక (code) {

	  స్థితి vpiAutomatic:
	    తిరిగి scope_->is_automatic()? 1 : 0;
      }

      తిరిగి 0;
}

అక్షరం* __vpiNamedEvent::vpi_get_str(పూర్ణాంకం code)
{
      ఒకవేళ (code == vpiFile) {  // Not implemented for now!
	    తిరిగి simple_సమితి_rbuf_str(file_పేర్లు[0]);
      }
      తిరిగి generic_get_str(code, scope_, name_, ఖాళీ);
}

vpiHandle __vpiNamedEvent::vpi_put_విలువ(p_vpi_విలువ, పూర్ణాంకం)
{
	// p_vpi_value may be NULL, and an event doesn't care
	// what the value is
      vvp_vector4_t val;
      vvp_net_ptr_t dest(funct, 0);
      vvp_send_vec4(dest, val, vthread_get_wt_context());

      తిరిగి ఇది;
}

vpiHandle __vpiNamedEvent::vpi_handle(పూర్ణాంకం code)
{
      ఎంపిక (code) {
	  స్థితి vpiScope:
	    తిరిగి scope_;

	  స్థితి vpiModule:
	    తిరిగి vpip_module(scope_);
      }

      తిరిగి 0;
}


vpiHandle vpip_make_named_event(స్థిరరాశి అక్షరం*name, vvp_net_t*funct)
{
      __vpiNamedEvent*obj = కొత్త __vpiNamedEvent(vpip_peek_current_scope(), name);

      obj->funct = funct;

      తిరిగి obj;
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
శూన్య __vpiNamedEvent::నిరంతరం_vpi_callbacks()
{
      నిర్మాణం __vpiCallback*తదుపరి_2 = callbacks_;
      నిర్మాణం __vpiCallback*prev = 0;
      అయితే (తదుపరి_2) {
	    నిర్మాణం __vpiCallback*cur = తదుపరి_2;
	    తదుపరి_2 = cur->తదుపరి_2;

	    ఒకవేళ (cur->cb_దత్తాంశం.cb_rtn != 0) {
		  callback_execute(cur);
		  prev = cur;

	    } లేకపోతే ఒకవేళ (prev == 0) {
		  callbacks_ = తదుపరి_2;
		  cur->తదుపరి_2 = 0;
		  తొలగించు cur;

	    } లేకపోతే {
		  ప్రతిజ్ఞా(prev->తదుపరి_2 == cur);
		  prev->తదుపరి_2 = తదుపరి_2;
		  cur->తదుపరి_2 = 0;
		  తొలగించు cur;
	    }
      }
}
