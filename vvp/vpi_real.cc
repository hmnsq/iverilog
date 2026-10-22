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

ساكن صحيح real_var_get(صحيح code, vpiHandle ref)
{
      بنية __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      أكد(rfp);

      بدل (code) {
	حالة vpiArray:
	    أرجع rfp->is_netarray != 0;

	حالة vpiSize:
	    أرجع 1;

	حالة vpi_قۇر_No:
	    أرجع 0; // Not implemented for now!

	حالة vpiAutomatic:
	  أرجع vpip_scope(rfp)->is_automatic()? 1 : 0;
      }

      أرجع 0;
}

ساكن حرف* real_var_get_str(صحيح code, vpiHandle ref)
{
      بنية __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      أكد(rfp);

      إذا (code == vpiFile) {  // Not implemented for now!
	    أرجع simple_set_rbuf_str(file_names[0]);
      }

      حرف *nm;
      ثابت حرف *ixs;
      إذا (rfp->is_netarray) {
	    nm = strdup(vpi_get_str(vpiName, rfp->within.parent));
	    s_vpi_قىممەت vp;
	    vp.format = vpiDecStrVal;
	    vpi_get_قىممەت(rfp->id.index, &vp);
	    ixs = vp.value.str;  /* do I need to strdup() this? */
      } وإلا {
	    nm = strdup(rfp->id.name);
	    ixs = عدم;
      }

      حرف *rbuf = generic_get_str(code, vpip_scope(rfp), nm, ixs);
      حرر(nm);
      أرجع rbuf;
}

ساكن vpiHandle real_var_get_handle(صحيح code, vpiHandle ref)
{
      بنية __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      أكد(rfp);

      بدل (code) {

	  حالة vpiParent:
	    أرجع rfp->is_netarray ? rfp->within.parent : عدم;

	  حالة vpiIndex:
	    أرجع rfp->is_netarray ? rfp->id.index : عدم;

	  حالة vpiScope:
	    أرجع vpip_scope(rfp);

	  حالة vpiModule:
	    أرجع vpip_module(vpip_scope(rfp));
      }

      أرجع 0;
}

ساكن vpiHandle real_var_iterate(صحيح code, vpiHandle ref)
{
      بنية __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      أكد(rfp);

      إذا (code == vpiIndex) {
	    أرجع rfp->is_netarray ? rfp->id.index->vpi_iterate(code) : عدم;
      }

      أرجع 0;
}

ساكن فراغ real_var_get_قىممەت(vpiHandle ref, s_vpi_قىممەت*vp)
{
      بنية __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      أكد(rfp);

      vvp_signal_قىممەت*fil
	    = dynamic_cast<vvp_signal_قىممەت*>(rfp->net->fil);

      fil->get_signal_قىممەت(vp);
}

ساكن vpiHandle real_var_put_قىممەت(vpiHandle ref, p_vpi_قىممەت vp, صحيح flags)
{
      بنية __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      أكد(rfp);
      vvp_net_ptr_t destination (rfp->net, 0);

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value. */
      إذا (flags == vpiReleaseFlag) {
	    أكد(rfp->net->fil);
	    rfp->net->fil->force_unlink();
	    rfp->net->fil->release(destination, rfp->is_wire);
	    real_var_get_قىممەت(ref, vp);
	    أرجع ref;
      }

      مزدوج result = real_from_vpi_قىممەت(vp);

      إذا (flags == vpiForceFlag) {
	    vvp_vector2_t mask (vvp_vector2_t::FILL1, 1);
	    rfp->net->force_real(result, mask);
      } وإلا إذا (rfp->is_wire) {
	    rfp->net->send_real(result, vthread_get_wt_context());
      } وإلا {
	    vvp_send_real(destination, result, vthread_get_wt_context());
      }
      أرجع 0;
}

مضمن __vpiRealVar::__vpiRealVar()
{ }

صحيح __vpiRealVar::get_type_code(فراغ) ثابت
{ أرجع vpiRealVar; }

صحيح __vpiRealVar::vpi_get(صحيح code)
{ أرجع real_var_get(code, هذا); }

حرف* __vpiRealVar::vpi_get_str(صحيح code)
{ أرجع real_var_get_str(code, هذا); }

فراغ __vpiRealVar::vpi_get_قىممەت(p_vpi_قىممەت val)
{ real_var_get_قىممەت(هذا, val); }

vpiHandle __vpiRealVar::vpi_put_قىممەت(p_vpi_قىممەت val, صحيح flags)
{ أرجع real_var_put_قىممەت(هذا, val, flags); }

vpiHandle __vpiRealVar::vpi_handle(صحيح code)
{ أرجع real_var_get_handle(code, هذا); }

vpiHandle __vpiRealVar::vpi_iterate(صحيح code)
{ أرجع real_var_iterate(code, هذا); }

ساكن vpiHandle vpip_make_real_(__vpiScope*scope, ثابت حرف*name,
				 vvp_net_t*net, منطقي is_wire)
{
      بنية __vpiRealVar*obj = جديد __vpiRealVar;

      obj->id.name = name ? vpip_name_string(name) : عدم;
      obj->is_netarray = 0;
      obj->is_wire = is_wire;
      obj->net = net;

      obj->within.scope = scope;

      أرجع obj;
}

vpiHandle vpip_make_real_var(ثابت حرف*name, vvp_net_t*net)
{
      أرجع vpip_make_real_(vpip_peek_current_scope(), name, net, خطأ);
}

vpiHandle vpip_make_real_net(__vpiScope*scope,
			     ثابت حرف*name, vvp_net_t*net)
{
      أرجع vpip_make_real_(scope, name, net, true);
}

#ifdef CHECK_WITH_VALGRIND
فراغ real_delete(vpiHandle item)
{
      بنية __vpiRealVar*obj = dynamic_cast<__vpiRealVar*>(item);
      أكد(obj->net->fil);
      obj->net->fil->clear_all_callbacks();
      vvp_net_delete(obj->net);

      delete obj;
}
#endif
