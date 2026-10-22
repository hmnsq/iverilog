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

static số_nguyên real_var_get(số_nguyên code, vpiHandle ref)
{
      struct __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      assert(rfp);

      switch (code) {
	case vpiArray:
	    trả_về rfp->is_netarray != 0;

	case vpiSize:
	    trả_về 1;

	case vpiLineNo:
	    trả_về 0; // Not implemented for now!

	case vpiAutomatic:
	  trả_về vpip_scope(rfp)->is_automatic()? 1 : 0;
      }

      trả_về 0;
}

static char* real_var_get_str(số_nguyên code, vpiHandle ref)
{
      struct __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      assert(rfp);

      nếu (code == vpiFile) {  // Not implemented for now!
	    trả_về simple_set_rbuf_str(file_names[0]);
      }

      char *nm;
      const char *ixs;
      nếu (rfp->is_netarray) {
	    nm = strdup(vpi_get_str(vpiName, rfp->within.parent));
	    s_vpi_value vp;
	    vp.format = vpiDecStrVal;
	    vpi_get_value(rfp->id.index, &vp);
	    ixs = vp.value.str;  /* do I need to strdup() this? */
      } không_thì {
	    nm = strdup(rfp->id.name);
	    ixs = NULL;
      }

      char *rbuf = generic_get_str(code, vpip_scope(rfp), nm, ixs);
      free(nm);
      trả_về rbuf;
}

static vpiHandle real_var_get_handle(số_nguyên code, vpiHandle ref)
{
      struct __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      assert(rfp);

      switch (code) {

	  case vpiParent:
	    trả_về rfp->is_netarray ? rfp->within.parent : NULL;

	  case vpiIndex:
	    trả_về rfp->is_netarray ? rfp->id.index : NULL;

	  case vpiScope:
	    trả_về vpip_scope(rfp);

	  case vpiModule:
	    trả_về vpip_module(vpip_scope(rfp));
      }

      trả_về 0;
}

static vpiHandle real_var_iterate(số_nguyên code, vpiHandle ref)
{
      struct __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      assert(rfp);

      nếu (code == vpiIndex) {
	    trả_về rfp->is_netarray ? rfp->id.index->vpi_iterate(code) : NULL;
      }

      trả_về 0;
}

static void real_var_get_value(vpiHandle ref, s_vpi_value*vp)
{
      struct __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      assert(rfp);

      vvp_signal_value*fil
	    = dynamic_cast<vvp_signal_value*>(rfp->net->fil);

      fil->get_signal_value(vp);
}

static vpiHandle real_var_put_value(vpiHandle ref, p_vpi_value vp, số_nguyên flags)
{
      struct __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      assert(rfp);
      vvp_net_ptr_t destination (rfp->net, 0);

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value. */
      nếu (flags == vpiReleaseFlag) {
	    assert(rfp->net->fil);
	    rfp->net->fil->force_unlink();
	    rfp->net->fil->release(destination, rfp->is_wire);
	    real_var_get_value(ref, vp);
	    trả_về ref;
      }

      double result = real_from_vpi_value(vp);

      nếu (flags == vpiForceFlag) {
	    vvp_vector2_t mask (vvp_vector2_t::FILL1, 1);
	    rfp->net->force_real(result, mask);
      } không_thì nếu (rfp->is_wire) {
	    rfp->net->send_real(result, vthread_get_wt_context());
      } không_thì {
	    vvp_send_real(destination, result, vthread_get_wt_context());
      }
      trả_về 0;
}

inline __vpiRealVar::__vpiRealVar()
{ }

số_nguyên __vpiRealVar::get_type_code(void) const
{ trả_về vpiRealVar; }

số_nguyên __vpiRealVar::vpi_get(số_nguyên code)
{ trả_về real_var_get(code, this); }

char* __vpiRealVar::vpi_get_str(số_nguyên code)
{ trả_về real_var_get_str(code, this); }

void __vpiRealVar::vpi_get_value(p_vpi_value val)
{ real_var_get_value(this, val); }

vpiHandle __vpiRealVar::vpi_put_value(p_vpi_value val, số_nguyên flags)
{ trả_về real_var_put_value(this, val, flags); }

vpiHandle __vpiRealVar::vpi_handle(số_nguyên code)
{ trả_về real_var_get_handle(code, this); }

vpiHandle __vpiRealVar::vpi_iterate(số_nguyên code)
{ trả_về real_var_iterate(code, this); }

static vpiHandle vpip_make_real_(__vpiScope*scope, const char*name,
				 vvp_net_t*net, bool is_wire)
{
      struct __vpiRealVar*obj = new __vpiRealVar;

      obj->id.name = name ? vpip_name_string(name) : NULL;
      obj->is_netarray = 0;
      obj->is_wire = is_wire;
      obj->net = net;

      obj->within.scope = scope;

      trả_về obj;
}

vpiHandle vpip_make_real_var(const char*name, vvp_net_t*net)
{
      trả_về vpip_make_real_(vpip_peek_current_scope(), name, net, false);
}

vpiHandle vpip_make_real_net(__vpiScope*scope,
			     const char*name, vvp_net_t*net)
{
      trả_về vpip_make_real_(scope, name, net, true);
}

#ifdef CHECK_WITH_VALGRIND
void real_delete(vpiHandle item)
{
      struct __vpiRealVar*obj = dynamic_cast<__vpiRealVar*>(item);
      assert(obj->net->fil);
      obj->net->fil->clear_all_callbacks();
      vvp_net_delete(obj->net);

      delete obj;
}
#endif
