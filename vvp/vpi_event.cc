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

अंतरभूत __vpiNamedEvent::__vpiNamedEvent(__vpiScope*sc, स्थिर अक्षर*nam)
{
      scope_ = sc;
      name_ = vpip_name_string(nam);
      callbacks_ = 0;
}

__vpiNamedEvent::~__vpiNamedEvent()
{
      जबतक (callbacks_) {
	    काष्ठा __vpiCallback *tmp = callbacks_->अग्र_2;
	    मिटाओ callbacks_;
	    callbacks_ = tmp;
      }
}

पूर्णांक __vpiNamedEvent::get_type_code(व्योम) स्थिर
{ वापस vpiNamedEvent; }

पूर्णांक __vpiNamedEvent::vpi_get(पूर्णांक code)
{
      चयन (code) {

	  हाल vpiAutomatic:
	    वापस scope_->is_automatic()? 1 : 0;
      }

      वापस 0;
}

अक्षर* __vpiNamedEvent::vpi_get_str(पूर्णांक code)
{
      अगर (code == vpiFile) {  // Not implemented for now!
	    वापस simple_set_rbuf_str(file_names[0]);
      }
      वापस generic_get_str(code, scope_, name_, शून्य);
}

vpiHandle __vpiNamedEvent::vpi_put_मूल्य(p_vpi_मूल्य, पूर्णांक)
{
	// p_vpi_value may be NULL, and an event doesn't care
	// what the value is
      vvp_vector4_t val;
      vvp_net_ptr_t dest(funct, 0);
      vvp_send_vec4(dest, val, vthread_get_wt_context());

      वापस यह;
}

vpiHandle __vpiNamedEvent::vpi_handle(पूर्णांक code)
{
      चयन (code) {
	  हाल vpiScope:
	    वापस scope_;

	  हाल vpiModule:
	    वापस vpip_module(scope_);
      }

      वापस 0;
}


vpiHandle vpip_make_named_event(स्थिर अक्षर*name, vvp_net_t*funct)
{
      __vpiNamedEvent*obj = नया __vpiNamedEvent(vpip_peek_current_scope(), name);

      obj->funct = funct;

      वापस obj;
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
व्योम __vpiNamedEvent::run_vpi_callbacks()
{
      काष्ठा __vpiCallback*अग्र_2 = callbacks_;
      काष्ठा __vpiCallback*prev = 0;
      जबतक (अग्र_2) {
	    काष्ठा __vpiCallback*cur = अग्र_2;
	    अग्र_2 = cur->अग्र_2;

	    अगर (cur->cb_तथ्य.cb_rtn != 0) {
		  callback_execute(cur);
		  prev = cur;

	    } अन्यथा अगर (prev == 0) {
		  callbacks_ = अग्र_2;
		  cur->अग्र_2 = 0;
		  मिटाओ cur;

	    } अन्यथा {
		  निश्चित(prev->अग्र_2 == cur);
		  prev->अग्र_2 = अग्र_2;
		  cur->अग्र_2 = 0;
		  मिटाओ cur;
	    }
      }
}
