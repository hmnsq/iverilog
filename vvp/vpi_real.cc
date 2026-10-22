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

સ્થિર પૂર્ણાંક real_var_get(પૂર્ણાંક code, vpiHandle ref)
{
      સંરચના __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      પ્રતિજ્ઞા(rfp);

      પસંદગી (code) {
	સ્થિતિ vpiArray:
	    પાછા rfp->is_netarray != 0;

	સ્થિતિ vpiSize:
	    પાછા 1;

	સ્થિતિ vpi_લીટી_No:
	    પાછા 0; // Not implemented for now!

	સ્થિતિ vpiAutomatic:
	  પાછા vpip_scope(rfp)->is_automatic()? 1 : 0;
      }

      પાછા 0;
}

સ્થિર અક્ષર* real_var_get_str(પૂર્ણાંક code, vpiHandle ref)
{
      સંરચના __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      પ્રતિજ્ઞા(rfp);

      જો (code == vpiFile) {  // Not implemented for now!
	    પાછા simple_set_rbuf_str(file_names[0]);
      }

      અક્ષર *nm;
      અચલ અક્ષર *ixs;
      જો (rfp->is_netarray) {
	    nm = strdup(vpi_get_str(vpiName, rfp->within.parent));
	    s_vpi_કિંમત vp;
	    vp.format = vpiDecStrVal;
	    vpi_get_કિંમત(rfp->id.index, &vp);
	    ixs = vp.value.str;  /* do I need to strdup() this? */
      } નહીંતર {
	    nm = strdup(rfp->id.name);
	    ixs = ખાલી;
      }

      અક્ષર *rbuf = generic_get_str(code, vpip_scope(rfp), nm, ixs);
      મુક્ત(nm);
      પાછા rbuf;
}

સ્થિર vpiHandle real_var_get_handle(પૂર્ણાંક code, vpiHandle ref)
{
      સંરચના __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      પ્રતિજ્ઞા(rfp);

      પસંદગી (code) {

	  સ્થિતિ vpiParent:
	    પાછા rfp->is_netarray ? rfp->within.parent : ખાલી;

	  સ્થિતિ vpiIndex:
	    પાછા rfp->is_netarray ? rfp->id.index : ખાલી;

	  સ્થિતિ vpiScope:
	    પાછા vpip_scope(rfp);

	  સ્થિતિ vpiModule:
	    પાછા vpip_module(vpip_scope(rfp));
      }

      પાછા 0;
}

સ્થિર vpiHandle real_var_iterate(પૂર્ણાંક code, vpiHandle ref)
{
      સંરચના __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      પ્રતિજ્ઞા(rfp);

      જો (code == vpiIndex) {
	    પાછા rfp->is_netarray ? rfp->id.index->vpi_iterate(code) : ખાલી;
      }

      પાછા 0;
}

સ્થિર શૂન્ય real_var_get_કિંમત(vpiHandle ref, s_vpi_કિંમત*vp)
{
      સંરચના __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      પ્રતિજ્ઞા(rfp);

      vvp_signal_કિંમત*fil
	    = dynamic_cast<vvp_signal_કિંમત*>(rfp->net->fil);

      fil->get_signal_કિંમત(vp);
}

સ્થિર vpiHandle real_var_put_કિંમત(vpiHandle ref, p_vpi_કિંમત vp, પૂર્ણાંક flags)
{
      સંરચના __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      પ્રતિજ્ઞા(rfp);
      vvp_net_ptr_t destination (rfp->net, 0);

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value. */
      જો (flags == vpiReleaseFlag) {
	    પ્રતિજ્ઞા(rfp->net->fil);
	    rfp->net->fil->force_unlink();
	    rfp->net->fil->release(destination, rfp->is_wire);
	    real_var_get_કિંમત(ref, vp);
	    પાછા ref;
      }

      દ્વિગુણ result = real_from_vpi_કિંમત(vp);

      જો (flags == vpiForceFlag) {
	    vvp_vector2_t mask (vvp_vector2_t::FILL1, 1);
	    rfp->net->force_real(result, mask);
      } નહીંતર જો (rfp->is_wire) {
	    rfp->net->send_real(result, vthread_get_wt_context());
      } નહીંતર {
	    vvp_send_real(destination, result, vthread_get_wt_context());
      }
      પાછા 0;
}

અંતઃરેખ __vpiRealVar::__vpiRealVar()
{ }

પૂર્ણાંક __vpiRealVar::get_type_code(શૂન્ય) અચલ
{ પાછા vpiRealVar; }

પૂર્ણાંક __vpiRealVar::vpi_get(પૂર્ણાંક code)
{ પાછા real_var_get(code, આ); }

અક્ષર* __vpiRealVar::vpi_get_str(પૂર્ણાંક code)
{ પાછા real_var_get_str(code, આ); }

શૂન્ય __vpiRealVar::vpi_get_કિંમત(p_vpi_કિંમત val)
{ real_var_get_કિંમત(આ, val); }

vpiHandle __vpiRealVar::vpi_put_કિંમત(p_vpi_કિંમત val, પૂર્ણાંક flags)
{ પાછા real_var_put_કિંમત(આ, val, flags); }

vpiHandle __vpiRealVar::vpi_handle(પૂર્ણાંક code)
{ પાછા real_var_get_handle(code, આ); }

vpiHandle __vpiRealVar::vpi_iterate(પૂર્ણાંક code)
{ પાછા real_var_iterate(code, આ); }

સ્થિર vpiHandle vpip_make_real_(__vpiScope*scope, અચલ અક્ષર*name,
				 vvp_net_t*net, તાર્કિક is_wire)
{
      સંરચના __vpiRealVar*obj = નવું __vpiRealVar;

      obj->id.name = name ? vpip_name_string(name) : ખાલી;
      obj->is_netarray = 0;
      obj->is_wire = is_wire;
      obj->net = net;

      obj->within.scope = scope;

      પાછા obj;
}

vpiHandle vpip_make_real_var(અચલ અક્ષર*name, vvp_net_t*net)
{
      પાછા vpip_make_real_(vpip_peek_current_scope(), name, net, ખોટું);
}

vpiHandle vpip_make_real_net(__vpiScope*scope,
			     અચલ અક્ષર*name, vvp_net_t*net)
{
      પાછા vpip_make_real_(scope, name, net, સાચું);
}

#ifdef CHECK_WITH_VALGRIND
શૂન્ય real_delete(vpiHandle item)
{
      સંરચના __vpiRealVar*obj = dynamic_cast<__vpiRealVar*>(item);
      પ્રતિજ્ઞા(obj->net->fil);
      obj->net->fil->clear_all_callbacks();
      vvp_net_delete(obj->net);

      delete obj;
}
#endif
