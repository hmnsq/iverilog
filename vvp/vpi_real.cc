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

நிலையான முழு real_var_get(முழு code, vpiHandle ref)
{
      கட்டமைப்பு __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      ப்ரதிஜ்ஞா(rfp);

      தேர்வு (code) {
	நிலை vpiArray:
	    திரும்பு rfp->is_netarray != 0;

	நிலை vpiSize:
	    திரும்பு 1;

	நிலை vpi_வரி_No:
	    திரும்பு 0; // Not implemented for now!

	நிலை vpiAutomatic:
	  திரும்பு vpip_scope(rfp)->is_automatic()? 1 : 0;
      }

      திரும்பு 0;
}

நிலையான எழுத்து* real_var_get_str(முழு code, vpiHandle ref)
{
      கட்டமைப்பு __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      ப்ரதிஜ்ஞா(rfp);

      எனில் (code == vpiFile) {  // Not implemented for now!
	    திரும்பு simple_set_rbuf_str(file_names[0]);
      }

      எழுத்து *nm;
      மாறிலி எழுத்து *ixs;
      எனில் (rfp->is_netarray) {
	    nm = strdup(vpi_get_str(vpiName, rfp->within.parent));
	    s_vpi_மதிப்பு vp;
	    vp.format = vpiDecStrVal;
	    vpi_get_மதிப்பு(rfp->id.index, &vp);
	    ixs = vp.value.str;  /* do I need to strdup() this? */
      } இல்லையெனில் {
	    nm = strdup(rfp->id.name);
	    ixs = வெற்று;
      }

      எழுத்து *rbuf = generic_get_str(code, vpip_scope(rfp), nm, ixs);
      விடுவி(nm);
      திரும்பு rbuf;
}

நிலையான vpiHandle real_var_get_handle(முழு code, vpiHandle ref)
{
      கட்டமைப்பு __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      ப்ரதிஜ்ஞா(rfp);

      தேர்வு (code) {

	  நிலை vpiParent:
	    திரும்பு rfp->is_netarray ? rfp->within.parent : வெற்று;

	  நிலை vpiIndex:
	    திரும்பு rfp->is_netarray ? rfp->id.index : வெற்று;

	  நிலை vpiScope:
	    திரும்பு vpip_scope(rfp);

	  நிலை vpiModule:
	    திரும்பு vpip_module(vpip_scope(rfp));
      }

      திரும்பு 0;
}

நிலையான vpiHandle real_var_iterate(முழு code, vpiHandle ref)
{
      கட்டமைப்பு __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      ப்ரதிஜ்ஞா(rfp);

      எனில் (code == vpiIndex) {
	    திரும்பு rfp->is_netarray ? rfp->id.index->vpi_iterate(code) : வெற்று;
      }

      திரும்பு 0;
}

நிலையான வெறுமை real_var_get_மதிப்பு(vpiHandle ref, s_vpi_மதிப்பு*vp)
{
      கட்டமைப்பு __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      ப்ரதிஜ்ஞா(rfp);

      vvp_signal_மதிப்பு*fil
	    = dynamic_cast<vvp_signal_மதிப்பு*>(rfp->net->fil);

      fil->get_signal_மதிப்பு(vp);
}

நிலையான vpiHandle real_var_put_மதிப்பு(vpiHandle ref, p_vpi_மதிப்பு vp, முழு flags)
{
      கட்டமைப்பு __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      ப்ரதிஜ்ஞா(rfp);
      vvp_net_ptr_t destination (rfp->net, 0);

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value. */
      எனில் (flags == vpiReleaseFlag) {
	    ப்ரதிஜ்ஞா(rfp->net->fil);
	    rfp->net->fil->force_unlink();
	    rfp->net->fil->release(destination, rfp->is_wire);
	    real_var_get_மதிப்பு(ref, vp);
	    திரும்பு ref;
      }

      இரட்டை result = real_from_vpi_மதிப்பு(vp);

      எனில் (flags == vpiForceFlag) {
	    vvp_vector2_t mask (vvp_vector2_t::FILL1, 1);
	    rfp->net->force_real(result, mask);
      } இல்லையெனில் எனில் (rfp->is_wire) {
	    rfp->net->send_real(result, vthread_get_wt_context());
      } இல்லையெனில் {
	    vvp_send_real(destination, result, vthread_get_wt_context());
      }
      திரும்பு 0;
}

உள்வரி __vpiRealVar::__vpiRealVar()
{ }

முழு __vpiRealVar::get_type_code(வெறுமை) மாறிலி
{ திரும்பு vpiRealVar; }

முழு __vpiRealVar::vpi_get(முழு code)
{ திரும்பு real_var_get(code, இது); }

எழுத்து* __vpiRealVar::vpi_get_str(முழு code)
{ திரும்பு real_var_get_str(code, இது); }

வெறுமை __vpiRealVar::vpi_get_மதிப்பு(p_vpi_மதிப்பு val)
{ real_var_get_மதிப்பு(இது, val); }

vpiHandle __vpiRealVar::vpi_put_மதிப்பு(p_vpi_மதிப்பு val, முழு flags)
{ திரும்பு real_var_put_மதிப்பு(இது, val, flags); }

vpiHandle __vpiRealVar::vpi_handle(முழு code)
{ திரும்பு real_var_get_handle(code, இது); }

vpiHandle __vpiRealVar::vpi_iterate(முழு code)
{ திரும்பு real_var_iterate(code, இது); }

நிலையான vpiHandle vpip_make_real_(__vpiScope*scope, மாறிலி எழுத்து*name,
				 vvp_net_t*net, தருக்க is_wire)
{
      கட்டமைப்பு __vpiRealVar*obj = புதிய __vpiRealVar;

      obj->id.name = name ? vpip_name_string(name) : வெற்று;
      obj->is_netarray = 0;
      obj->is_wire = is_wire;
      obj->net = net;

      obj->within.scope = scope;

      திரும்பு obj;
}

vpiHandle vpip_make_real_var(மாறிலி எழுத்து*name, vvp_net_t*net)
{
      திரும்பு vpip_make_real_(vpip_peek_current_scope(), name, net, பொய்);
}

vpiHandle vpip_make_real_net(__vpiScope*scope,
			     மாறிலி எழுத்து*name, vvp_net_t*net)
{
      திரும்பு vpip_make_real_(scope, name, net, உண்மை);
}

#ifdef CHECK_WITH_VALGRIND
வெறுமை real_delete(vpiHandle item)
{
      கட்டமைப்பு __vpiRealVar*obj = dynamic_cast<__vpiRealVar*>(item);
      ப்ரதிஜ்ஞா(obj->net->fil);
      obj->net->fil->clear_all_callbacks();
      vvp_net_delete(obj->net);

      delete obj;
}
#endif
