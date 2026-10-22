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

स्थिर पूर्णाङ्क real_var_get(पूर्णाङ्क code, vpiHandle ref)
{
      संरचना __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      प्रतिज्ञा(rfp);

      विकल्प (code) {
	पक्ष vpiArray:
	    प्रत्यागम rfp->is_netarray != 0;

	पक्ष vpiSize:
	    प्रत्यागम 1;

	पक्ष vpi_पङ्क्तिः_No:
	    प्रत्यागम 0; // Not implemented for now!

	पक्ष vpiAutomatic:
	  प्रत्यागम vpip_scope(rfp)->is_automatic()? 1 : 0;
      }

      प्रत्यागम 0;
}

स्थिर वर्ण* real_var_get_str(पूर्णाङ्क code, vpiHandle ref)
{
      संरचना __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      प्रतिज्ञा(rfp);

      जइ (code == vpiFile) {  // Not implemented for now!
	    प्रत्यागम simple_set_rbuf_str(file_names[0]);
      }

      वर्ण *nm;
      ध्रुव वर्ण *ixs;
      जइ (rfp->is_netarray) {
	    nm = strdup(vpi_get_str(vpiName, rfp->within.parent));
	    s_vpi_मानम् vp;
	    vp.format = vpiDecStrVal;
	    vpi_get_मानम्(rfp->id.index, &vp);
	    ixs = vp.value.str;  /* do I need to strdup() this? */
      } अण्णहा {
	    nm = strdup(rfp->id.name);
	    ixs = रिक्त;
      }

      वर्ण *rbuf = generic_get_str(code, vpip_scope(rfp), nm, ixs);
      मुक्त(nm);
      प्रत्यागम rbuf;
}

स्थिर vpiHandle real_var_get_handle(पूर्णाङ्क code, vpiHandle ref)
{
      संरचना __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      प्रतिज्ञा(rfp);

      विकल्प (code) {

	  पक्ष vpiParent:
	    प्रत्यागम rfp->is_netarray ? rfp->within.parent : रिक्त;

	  पक्ष vpiIndex:
	    प्रत्यागम rfp->is_netarray ? rfp->id.index : रिक्त;

	  पक्ष vpiScope:
	    प्रत्यागम vpip_scope(rfp);

	  पक्ष vpiModule:
	    प्रत्यागम vpip_module(vpip_scope(rfp));
      }

      प्रत्यागम 0;
}

स्थिर vpiHandle real_var_iterate(पूर्णाङ्क code, vpiHandle ref)
{
      संरचना __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      प्रतिज्ञा(rfp);

      जइ (code == vpiIndex) {
	    प्रत्यागम rfp->is_netarray ? rfp->id.index->vpi_iterate(code) : रिक्त;
      }

      प्रत्यागम 0;
}

स्थिर शून्य real_var_get_मानम्(vpiHandle ref, s_vpi_मानम्*vp)
{
      संरचना __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      प्रतिज्ञा(rfp);

      vvp_signal_मानम्*fil
	    = dynamic_cast<vvp_signal_मानम्*>(rfp->net->fil);

      fil->get_signal_मानम्(vp);
}

स्थिर vpiHandle real_var_put_मानम्(vpiHandle ref, p_vpi_मानम् vp, पूर्णाङ्क flags)
{
      संरचना __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      प्रतिज्ञा(rfp);
      vvp_net_ptr_t destination (rfp->net, 0);

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value. */
      जइ (flags == vpiReleaseFlag) {
	    प्रतिज्ञा(rfp->net->fil);
	    rfp->net->fil->force_unlink();
	    rfp->net->fil->release(destination, rfp->is_wire);
	    real_var_get_मानम्(ref, vp);
	    प्रत्यागम ref;
      }

      द्विगुण result = real_from_vpi_मानम्(vp);

      जइ (flags == vpiForceFlag) {
	    vvp_vector2_t mask (vvp_vector2_t::FILL1, 1);
	    rfp->net->force_real(result, mask);
      } अण्णहा जइ (rfp->is_wire) {
	    rfp->net->send_real(result, vthread_get_wt_context());
      } अण्णहा {
	    vvp_send_real(destination, result, vthread_get_wt_context());
      }
      प्रत्यागम 0;
}

अन्तःस्थ __vpiRealVar::__vpiRealVar()
{ }

पूर्णाङ्क __vpiRealVar::get_type_code(शून्य) ध्रुव
{ प्रत्यागम vpiRealVar; }

पूर्णाङ्क __vpiRealVar::vpi_get(पूर्णाङ्क code)
{ प्रत्यागम real_var_get(code, एतत्); }

वर्ण* __vpiRealVar::vpi_get_str(पूर्णाङ्क code)
{ प्रत्यागम real_var_get_str(code, एतत्); }

शून्य __vpiRealVar::vpi_get_मानम्(p_vpi_मानम् val)
{ real_var_get_मानम्(एतत्, val); }

vpiHandle __vpiRealVar::vpi_put_मानम्(p_vpi_मानम् val, पूर्णाङ्क flags)
{ प्रत्यागम real_var_put_मानम्(एतत्, val, flags); }

vpiHandle __vpiRealVar::vpi_handle(पूर्णाङ्क code)
{ प्रत्यागम real_var_get_handle(code, एतत्); }

vpiHandle __vpiRealVar::vpi_iterate(पूर्णाङ्क code)
{ प्रत्यागम real_var_iterate(code, एतत्); }

स्थिर vpiHandle vpip_make_real_(__vpiScope*scope, ध्रुव वर्ण*name,
				 vvp_net_t*net, तार्किक is_wire)
{
      संरचना __vpiRealVar*obj = नूतन __vpiRealVar;

      obj->id.name = name ? vpip_name_string(name) : रिक्त;
      obj->is_netarray = 0;
      obj->is_wire = is_wire;
      obj->net = net;

      obj->within.scope = scope;

      प्रत्यागम obj;
}

vpiHandle vpip_make_real_var(ध्रुव वर्ण*name, vvp_net_t*net)
{
      प्रत्यागम vpip_make_real_(vpip_peek_current_scope(), name, net, मिच्छ);
}

vpiHandle vpip_make_real_net(__vpiScope*scope,
			     ध्रुव वर्ण*name, vvp_net_t*net)
{
      प्रत्यागम vpip_make_real_(scope, name, net, सच्च);
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
