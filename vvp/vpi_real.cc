/*
 * Copyright (c) 2003-2025 Stephen Williams (steve@icarus.com)
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
# include  "vvp_net_sig.h"
# include  "schedule.h"
#ifdef CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#endif
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <cassert>
# include  "ivl_alloc.h"

स्थिर पूर्णांक real_var_get(पूर्णांक code, vpiHandle ref)
{
      संरचना __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      प्रतिज्ञा(rfp);

      निवड (code) {
	स्थिती vpiArray:
	    परत rfp->is_netarray != 0;

	स्थिती vpiSize:
	    परत 1;

	स्थिती vpi_ओळ_No:
	    परत 0; // Not implemented for now!

	स्थिती vpiAutomatic:
	  परत vpip_scope(rfp)->is_automatic()? 1 : 0;
      }

      परत 0;
}

स्थिर अक्षर* real_var_get_str(पूर्णांक code, vpiHandle ref)
{
      संरचना __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      प्रतिज्ञा(rfp);

      जर (code == vpiFile) {  // Not implemented for now!
	    परत simple_set_rbuf_str(file_names[0]);
      }

      अक्षर *nm;
      स्थिरांक अक्षर *ixs;
      जर (rfp->is_netarray) {
	    nm = strdup(vpi_get_str(vpiName, rfp->within.parent));
	    s_vpi_मूल्य vp;
	    vp.format = vpiDecStrVal;
	    vpi_get_मूल्य(rfp->id.index, &vp);
	    ixs = vp.value.str;  /* do I need to strdup() this? */
      } अन्यथा {
	    nm = strdup(rfp->id.name);
	    ixs = रिक्त;
      }

      अक्षर *rbuf = generic_get_str(code, vpip_scope(rfp), nm, ixs);
      मुक्त(nm);
      परत rbuf;
}

स्थिर vpiHandle real_var_get_handle(पूर्णांक code, vpiHandle ref)
{
      संरचना __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      प्रतिज्ञा(rfp);

      निवड (code) {

	  स्थिती vpiParent:
	    परत rfp->is_netarray ? rfp->within.parent : रिक्त;

	  स्थिती vpiIndex:
	    परत rfp->is_netarray ? rfp->id.index : रिक्त;

	  स्थिती vpiScope:
	    परत vpip_scope(rfp);

	  स्थिती vpiModule:
	    परत vpip_module(vpip_scope(rfp));
      }

      परत 0;
}

स्थिर vpiHandle real_var_iterate(पूर्णांक code, vpiHandle ref)
{
      संरचना __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      प्रतिज्ञा(rfp);

      जर (code == vpiIndex) {
	    परत rfp->is_netarray ? rfp->id.index->vpi_iterate(code) : रिक्त;
      }

      परत 0;
}

स्थिर शून्य real_var_get_मूल्य(vpiHandle ref, s_vpi_मूल्य*vp)
{
      संरचना __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      प्रतिज्ञा(rfp);

      vvp_signal_मूल्य*fil
	    = dynamic_cast<vvp_signal_मूल्य*>(rfp->net->fil);

      fil->get_signal_मूल्य(vp);
}

स्थिर vpiHandle real_var_put_मूल्य(vpiHandle ref, p_vpi_मूल्य vp, पूर्णांक flags)
{
      संरचना __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      प्रतिज्ञा(rfp);
      vvp_net_ptr_t destination (rfp->net, 0);

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value. */
      जर (flags == vpiReleaseFlag) {
	    प्रतिज्ञा(rfp->net->fil);
	    rfp->net->fil->force_unlink();
	    rfp->net->fil->release(destination, rfp->is_wire);
	    real_var_get_मूल्य(ref, vp);
	    परत ref;
      }

      द्विगुण result = real_from_vpi_मूल्य(vp);

      जर (flags == vpiForceFlag) {
	    vvp_vector2_t mask (vvp_vector2_t::FILL1, 1);
	    rfp->net->force_real(result, mask);
      } अन्यथा जर (rfp->is_wire) {
	    rfp->net->send_real(result, vthread_get_wt_context());
      } अन्यथा {
	    vvp_send_real(destination, result, vthread_get_wt_context());
      }
      परत 0;
}

ओळीत __vpiRealVar::__vpiRealVar()
{ }

पूर्णांक __vpiRealVar::get_type_code(शून्य) स्थिरांक
{ परत vpiRealVar; }

पूर्णांक __vpiRealVar::vpi_get(पूर्णांक code)
{ परत real_var_get(code, हे); }

अक्षर* __vpiRealVar::vpi_get_str(पूर्णांक code)
{ परत real_var_get_str(code, हे); }

शून्य __vpiRealVar::vpi_get_मूल्य(p_vpi_मूल्य val)
{ real_var_get_मूल्य(हे, val); }

vpiHandle __vpiRealVar::vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णांक flags)
{ परत real_var_put_मूल्य(हे, val, flags); }

vpiHandle __vpiRealVar::vpi_handle(पूर्णांक code)
{ परत real_var_get_handle(code, हे); }

vpiHandle __vpiRealVar::vpi_iterate(पूर्णांक code)
{ परत real_var_iterate(code, हे); }

स्थिर vpiHandle vpip_make_real_(__vpiScope*scope, स्थिरांक अक्षर*name,
				 vvp_net_t*net, तार्किक is_wire)
{
      संरचना __vpiRealVar*obj = नवीन __vpiRealVar;

      obj->id.name = name ? vpip_name_string(name) : रिक्त;
      obj->is_netarray = 0;
      obj->is_wire = is_wire;
      obj->net = net;

      obj->within.scope = scope;

      परत obj;
}

vpiHandle vpip_make_real_var(स्थिरांक अक्षर*name, vvp_net_t*net)
{
      परत vpip_make_real_(vpip_peek_current_scope(), name, net, असत्य);
}

vpiHandle vpip_make_real_net(__vpiScope*scope,
			     स्थिरांक अक्षर*name, vvp_net_t*net)
{
      परत vpip_make_real_(scope, name, net, सत्य);
}

#ifdef CHECK_WITH_VALGRIND
शून्य real_delete(vpiHandle item)
{
      संरचना __vpiRealVar*obj = dynamic_cast<__vpiRealVar*>(item);
      प्रतिज्ञा(obj->net->fil);
      obj->net->fil->clear_all_callbacks();
      vvp_net_delete(obj->net);

      delete obj;
}
#endif
