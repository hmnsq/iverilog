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

অন্তঃরেখ __vpiNamedEvent::__vpiNamedEvent(__vpiScope*sc, ধ্রুবক অক্ষর*nam)
{
      scope_ = sc;
      name_ = vpip_name_string(nam);
      callbacks_ = 0;
}

__vpiNamedEvent::~__vpiNamedEvent()
{
      যখন (callbacks_) {
	    কাঠামো __vpiCallback *tmp = callbacks_->পরবর্তী_2;
	    delete callbacks_;
	    callbacks_ = tmp;
      }
}

পূর্ণসংখ্যা __vpiNamedEvent::get_type_code(শূন্য) ধ্রুবক
{ ফেরত vpiNamedEvent; }

পূর্ণসংখ্যা __vpiNamedEvent::vpi_get(পূর্ণসংখ্যা code)
{
      নির্বাচন (code) {

	  ক্ষেত্র vpiAutomatic:
	    ফেরত scope_->is_automatic()? 1 : 0;
      }

      ফেরত 0;
}

অক্ষর* __vpiNamedEvent::vpi_get_str(পূর্ণসংখ্যা code)
{
      যদি (code == vpiFile) {  // Not implemented for now!
	    ফেরত simple_set_rbuf_str(file_names[0]);
      }
      ফেরত generic_get_str(code, scope_, name_, ফাঁকা);
}

vpiHandle __vpiNamedEvent::vpi_put_মান(p_vpi_মান, পূর্ণসংখ্যা)
{
	// p_vpi_value may be NULL, and an event doesn't care
	// what the value is
      vvp_vector4_t val;
      vvp_net_ptr_t dest(funct, 0);
      vvp_send_vec4(dest, val, vthread_get_wt_context());

      ফেরত এই;
}

vpiHandle __vpiNamedEvent::vpi_handle(পূর্ণসংখ্যা code)
{
      নির্বাচন (code) {
	  ক্ষেত্র vpiScope:
	    ফেরত scope_;

	  ক্ষেত্র vpiModule:
	    ফেরত vpip_module(scope_);
      }

      ফেরত 0;
}


vpiHandle vpip_make_named_event(ধ্রুবক অক্ষর*name, vvp_net_t*funct)
{
      __vpiNamedEvent*obj = নতুন __vpiNamedEvent(vpip_peek_current_scope(), name);

      obj->funct = funct;

      ফেরত obj;
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
শূন্য __vpiNamedEvent::run_vpi_callbacks()
{
      কাঠামো __vpiCallback*পরবর্তী_2 = callbacks_;
      কাঠামো __vpiCallback*prev = 0;
      যখন (পরবর্তী_2) {
	    কাঠামো __vpiCallback*cur = পরবর্তী_2;
	    পরবর্তী_2 = cur->পরবর্তী_2;

	    যদি (cur->cb_তথ্য.cb_rtn != 0) {
		  callback_execute(cur);
		  prev = cur;

	    } অন্যথা যদি (prev == 0) {
		  callbacks_ = পরবর্তী_2;
		  cur->পরবর্তী_2 = 0;
		  delete cur;

	    } অন্যথা {
		  প্রতিজ্ঞা(prev->পরবর্তী_2 == cur);
		  prev->পরবর্তী_2 = পরবর্তী_2;
		  cur->পরবর্তী_2 = 0;
		  delete cur;
	    }
      }
}
