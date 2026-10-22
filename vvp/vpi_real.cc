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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#ਜੇ_ਅੰਤ
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <cassert>
# include  "ivl_alloc.h"

ਸਥਿਰ ਪੂਰਨ real_var_get(ਪੂਰਨ code, vpiHandle ref)
{
      ਬਣਤਰ __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(rfp);

      ਚੋਣ (code) {
	ਹਾਲਤ vpiArray:
	    ਵਾਪਸ rfp->is_netarray != 0;

	ਹਾਲਤ vpi_ਮਾਪ:
	    ਵਾਪਸ 1;

	ਹਾਲਤ vpi_ਸਤਰ_No:
	    ਵਾਪਸ 0; // Not implemented for now!

	ਹਾਲਤ vpiAutomatic:
	  ਵਾਪਸ vpip_scope(rfp)->is_automatic()? 1 : 0;
      }

      ਵਾਪਸ 0;
}

ਸਥਿਰ ਅੱਖਰ* real_var_get_str(ਪੂਰਨ code, vpiHandle ref)
{
      ਬਣਤਰ __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(rfp);

      ਜੇ (code == vpiFile) {  // Not implemented for now!
	    ਵਾਪਸ simple_ਸੈੱਟ_rbuf_str(file_ਨਾਂ[0]);
      }

      ਅੱਖਰ *nm;
      ਅਚਲ ਅੱਖਰ *ixs;
      ਜੇ (rfp->is_netarray) {
	    nm = strdup(vpi_get_str(vpiName, rfp->within.ਮਾਪਾ));
	    s_vpi_ਮੁੱਲ vp;
	    vp.format = vpiDecStrVal;
	    vpi_get_ਮੁੱਲ(rfp->id.ਸੂਚਕਾਂਕ, &vp);
	    ixs = vp.value.str;  /* do I need to strdup() this? */
      } ਨਹੀਂਤਾਂ {
	    nm = strdup(rfp->id.name);
	    ixs = ਨਿਰਾ;
      }

      ਅੱਖਰ *rbuf = generic_get_str(code, vpip_scope(rfp), nm, ixs);
      ਮੁਕਤ(nm);
      ਵਾਪਸ rbuf;
}

ਸਥਿਰ vpiHandle real_var_get_handle(ਪੂਰਨ code, vpiHandle ref)
{
      ਬਣਤਰ __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(rfp);

      ਚੋਣ (code) {

	  ਹਾਲਤ vpi_ਮਾਪਾ:
	    ਵਾਪਸ rfp->is_netarray ? rfp->within.ਮਾਪਾ : ਨਿਰਾ;

	  ਹਾਲਤ vpi_ਸੂਚਕਾਂਕ_2:
	    ਵਾਪਸ rfp->is_netarray ? rfp->id.ਸੂਚਕਾਂਕ : ਨਿਰਾ;

	  ਹਾਲਤ vpiScope:
	    ਵਾਪਸ vpip_scope(rfp);

	  ਹਾਲਤ vpiModule:
	    ਵਾਪਸ vpip_module(vpip_scope(rfp));
      }

      ਵਾਪਸ 0;
}

ਸਥਿਰ vpiHandle real_var_iterate(ਪੂਰਨ code, vpiHandle ref)
{
      ਬਣਤਰ __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(rfp);

      ਜੇ (code == vpi_ਸੂਚਕਾਂਕ_2) {
	    ਵਾਪਸ rfp->is_netarray ? rfp->id.ਸੂਚਕਾਂਕ->vpi_iterate(code) : ਨਿਰਾ;
      }

      ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ real_var_get_ਮੁੱਲ(vpiHandle ref, s_vpi_ਮੁੱਲ*vp)
{
      ਬਣਤਰ __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(rfp);

      vvp_signal_ਮੁੱਲ*fil
	    = dynamic_cast<vvp_signal_ਮੁੱਲ*>(rfp->net->fil);

      fil->get_signal_ਮੁੱਲ(vp);
}

ਸਥਿਰ vpiHandle real_var_put_ਮੁੱਲ(vpiHandle ref, p_vpi_ਮੁੱਲ vp, ਪੂਰਨ flags)
{
      ਬਣਤਰ __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(rfp);
      vvp_net_ptr_t destination (rfp->net, 0);

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value. */
      ਜੇ (flags == vpiReleaseFlag) {
	    ਪ੍ਰਤਿਜ੍ਞਾ(rfp->net->fil);
	    rfp->net->fil->force_unlink();
	    rfp->net->fil->release(destination, rfp->is_wire);
	    real_var_get_ਮੁੱਲ(ref, vp);
	    ਵਾਪਸ ref;
      }

      ਦੁੱਗਣਾ ਨਤੀਜਾ = real_from_vpi_ਮੁੱਲ(vp);

      ਜੇ (flags == vpiForceFlag) {
	    vvp_vector2_t ਨਕਾਬ (vvp_vector2_t::FILL1, 1);
	    rfp->net->force_real(ਨਤੀਜਾ, ਨਕਾਬ);
      } ਨਹੀਂਤਾਂ ਜੇ (rfp->is_wire) {
	    rfp->net->send_real(ਨਤੀਜਾ, vthread_get_wt_context());
      } ਨਹੀਂਤਾਂ {
	    vvp_send_real(destination, ਨਤੀਜਾ, vthread_get_wt_context());
      }
      ਵਾਪਸ 0;
}

ਸਤਰਅੰਦਰ __vpiRealVar::__vpiRealVar()
{ }

ਪੂਰਨ __vpiRealVar::get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ
{ ਵਾਪਸ vpiRealVar; }

ਪੂਰਨ __vpiRealVar::vpi_get(ਪੂਰਨ code)
{ ਵਾਪਸ real_var_get(code, ਇਹ); }

ਅੱਖਰ* __vpiRealVar::vpi_get_str(ਪੂਰਨ code)
{ ਵਾਪਸ real_var_get_str(code, ਇਹ); }

ਖਾਲੀ __vpiRealVar::vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val)
{ real_var_get_ਮੁੱਲ(ਇਹ, val); }

vpiHandle __vpiRealVar::vpi_put_ਮੁੱਲ(p_vpi_ਮੁੱਲ val, ਪੂਰਨ flags)
{ ਵਾਪਸ real_var_put_ਮੁੱਲ(ਇਹ, val, flags); }

vpiHandle __vpiRealVar::vpi_handle(ਪੂਰਨ code)
{ ਵਾਪਸ real_var_get_handle(code, ਇਹ); }

vpiHandle __vpiRealVar::vpi_iterate(ਪੂਰਨ code)
{ ਵਾਪਸ real_var_iterate(code, ਇਹ); }

ਸਥਿਰ vpiHandle vpip_make_real_(__vpiScope*scope, ਅਚਲ ਅੱਖਰ*name,
				 vvp_net_t*net, ਤਾਰਕਿਕ is_wire)
{
      ਬਣਤਰ __vpiRealVar*obj = ਨਵਾਂ __vpiRealVar;

      obj->id.name = name ? vpip_name_string(name) : ਨਿਰਾ;
      obj->is_netarray = 0;
      obj->is_wire = is_wire;
      obj->net = net;

      obj->within.scope = scope;

      ਵਾਪਸ obj;
}

vpiHandle vpip_make_real_var(ਅਚਲ ਅੱਖਰ*name, vvp_net_t*net)
{
      ਵਾਪਸ vpip_make_real_(vpip_peek_current_scope(), name, net, ਝੂਠ);
}

vpiHandle vpip_make_real_net(__vpiScope*scope,
			     ਅਚਲ ਅੱਖਰ*name, vvp_net_t*net)
{
      ਵਾਪਸ vpip_make_real_(scope, name, net, ਸੱਚ);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਖਾਲੀ real_delete(vpiHandle ਵਸਤੂ)
{
      ਬਣਤਰ __vpiRealVar*obj = dynamic_cast<__vpiRealVar*>(ਵਸਤੂ);
      ਪ੍ਰਤਿਜ੍ਞਾ(obj->net->fil);
      obj->net->fil->clear_all_callbacks();
      vvp_net_delete(obj->net);

      ਮਿਟਾਓ obj;
}
#ਜੇ_ਅੰਤ
