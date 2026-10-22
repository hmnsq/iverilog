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

static ཧྲིལ real_var_get(ཧྲིལ code, vpiHandle ref)
{
      sa_nracanaa __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      assert(rfp);

      switch (code) {
	pak_sa vpiArray:
	    ཕྱིར rfp->is_netarray != 0;

	pak_sa vpiSize:
	    ཕྱིར 1;

	pak_sa vpiLineNo:
	    ཕྱིར 0; // Not implemented for now!

	pak_sa vpiAutomatic:
	  ཕྱིར vpip_scope(rfp)->is_automatic()? 1 : 0;
      }

      ཕྱིར 0;
}

static var_na* real_var_get_str(ཧྲིལ code, vpiHandle ref)
{
      sa_nracanaa __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      assert(rfp);

      གལ་ཏེ (code == vpiFile) {  // Not implemented for now!
	    ཕྱིར simple_set_rbuf_str(file_names[0]);
      }

      var_na *nm;
      const var_na *ixs;
      གལ་ཏེ (rfp->is_netarray) {
	    nm = strdup(vpi_get_str(vpiName, rfp->within.parent));
	    s_vpi_value vp;
	    vp.format = vpiDecStrVal;
	    vpi_get_value(rfp->id.index, &vp);
	    ixs = vp.value.str;  /* do I need to strdup() this? */
      } དེ་མིན {
	    nm = strdup(rfp->id.name);
	    ixs = NULL;
      }

      var_na *rbuf = generic_get_str(code, vpip_scope(rfp), nm, ixs);
      free(nm);
      ཕྱིར rbuf;
}

static vpiHandle real_var_get_handle(ཧྲིལ code, vpiHandle ref)
{
      sa_nracanaa __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      assert(rfp);

      switch (code) {

	  pak_sa vpiParent:
	    ཕྱིར rfp->is_netarray ? rfp->within.parent : NULL;

	  pak_sa vpiIndex:
	    ཕྱིར rfp->is_netarray ? rfp->id.index : NULL;

	  pak_sa vpiScope:
	    ཕྱིར vpip_scope(rfp);

	  pak_sa vpiModule:
	    ཕྱིར vpip_module(vpip_scope(rfp));
      }

      ཕྱིར 0;
}

static vpiHandle real_var_iterate(ཧྲིལ code, vpiHandle ref)
{
      sa_nracanaa __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      assert(rfp);

      གལ་ཏེ (code == vpiIndex) {
	    ཕྱིར rfp->is_netarray ? rfp->id.index->vpi_iterate(code) : NULL;
      }

      ཕྱིར 0;
}

static void real_var_get_value(vpiHandle ref, s_vpi_value*vp)
{
      sa_nracanaa __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      assert(rfp);

      vvp_signal_value*fil
	    = dynamic_cast<vvp_signal_value*>(rfp->net->fil);

      fil->get_signal_value(vp);
}

static vpiHandle real_var_put_value(vpiHandle ref, p_vpi_value vp, ཧྲིལ flags)
{
      sa_nracanaa __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      assert(rfp);
      vvp_net_ptr_t destination (rfp->net, 0);

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value. */
      གལ་ཏེ (flags == vpiReleaseFlag) {
	    assert(rfp->net->fil);
	    rfp->net->fil->force_unlink();
	    rfp->net->fil->release(destination, rfp->is_wire);
	    real_var_get_value(ref, vp);
	    ཕྱིར ref;
      }

      dviguu_na result = real_from_vpi_value(vp);

      གལ་ཏེ (flags == vpiForceFlag) {
	    vvp_vector2_t mask (vvp_vector2_t::FILL1, 1);
	    rfp->net->force_real(result, mask);
      } དེ་མིན གལ་ཏེ (rfp->is_wire) {
	    rfp->net->send_real(result, vthread_get_wt_context());
      } དེ་མིན {
	    vvp_send_real(destination, result, vthread_get_wt_context());
      }
      ཕྱིར 0;
}

anta_hstha __vpiRealVar::__vpiRealVar()
{ }

ཧྲིལ __vpiRealVar::get_type_code(void) const
{ ཕྱིར vpiRealVar; }

ཧྲིལ __vpiRealVar::vpi_get(ཧྲིལ code)
{ ཕྱིར real_var_get(code, this); }

var_na* __vpiRealVar::vpi_get_str(ཧྲིལ code)
{ ཕྱིར real_var_get_str(code, this); }

void __vpiRealVar::vpi_get_value(p_vpi_value val)
{ real_var_get_value(this, val); }

vpiHandle __vpiRealVar::vpi_put_value(p_vpi_value val, ཧྲིལ flags)
{ ཕྱིར real_var_put_value(this, val, flags); }

vpiHandle __vpiRealVar::vpi_handle(ཧྲིལ code)
{ ཕྱིར real_var_get_handle(code, this); }

vpiHandle __vpiRealVar::vpi_iterate(ཧྲིལ code)
{ ཕྱིར real_var_iterate(code, this); }

static vpiHandle vpip_make_real_(__vpiScope*scope, const var_na*name,
				 vvp_net_t*net, bool is_wire)
{
      sa_nracanaa __vpiRealVar*obj = new __vpiRealVar;

      obj->id.name = name ? vpip_name_string(name) : NULL;
      obj->is_netarray = 0;
      obj->is_wire = is_wire;
      obj->net = net;

      obj->within.scope = scope;

      ཕྱིར obj;
}

vpiHandle vpip_make_real_var(const var_na*name, vvp_net_t*net)
{
      ཕྱིར vpip_make_real_(vpip_peek_current_scope(), name, net, false);
}

vpiHandle vpip_make_real_net(__vpiScope*scope,
			     const var_na*name, vvp_net_t*net)
{
      ཕྱིར vpip_make_real_(scope, name, net, true);
}

#ifdef CHECK_WITH_VALGRIND
void real_delete(vpiHandle item)
{
      sa_nracanaa __vpiRealVar*obj = dynamic_cast<__vpiRealVar*>(item);
      assert(obj->net->fil);
      obj->net->fil->clear_all_callbacks();
      vvp_net_delete(obj->net);

      delete obj;
}
#endif
