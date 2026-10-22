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

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ real_var_get(ಪೂರ್ಣಾಙ್ಕ code, vpiHandle ref)
{
      ಸಂರಚನಾ __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ವಿಕಲ್ಪ (code) {
	ಪಕ್ಷ vpiArray:
	    ಪ್ರತ್ಯಾಗಮ rfp->is_netarray != 0;

	ಪಕ್ಷ vpiSize:
	    ಪ್ರತ್ಯಾಗಮ 1;

	ಪಕ್ಷ vpi_ಸಾಲು_No:
	    ಪ್ರತ್ಯಾಗಮ 0; // Not implemented for now!

	ಪಕ್ಷ vpiAutomatic:
	  ಪ್ರತ್ಯಾಗಮ vpip_scope(rfp)->is_automatic()? 1 : 0;
      }

      ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ವರ್ಣ* real_var_get_str(ಪೂರ್ಣಾಙ್ಕ code, vpiHandle ref)
{
      ಸಂರಚನಾ __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ಯದಿ (code == vpiFile) {  // Not implemented for now!
	    ಪ್ರತ್ಯಾಗಮ simple_set_rbuf_str(file_names[0]);
      }

      ವರ್ಣ *nm;
      ಧ್ರುವ ವರ್ಣ *ixs;
      ಯದಿ (rfp->is_netarray) {
	    nm = strdup(vpi_get_str(vpiName, rfp->within.parent));
	    s_vpi_ಮೌಲ್ಯ vp;
	    vp.format = vpiDecStrVal;
	    vpi_get_ಮೌಲ್ಯ(rfp->id.index, &vp);
	    ixs = vp.value.str;  /* do I need to strdup() this? */
      } ಅನ್ಯಥಾ {
	    nm = strdup(rfp->id.name);
	    ixs = ರಿಕ್ತ;
      }

      ವರ್ಣ *rbuf = generic_get_str(code, vpip_scope(rfp), nm, ixs);
      ಮುಕ್ತ(nm);
      ಪ್ರತ್ಯಾಗಮ rbuf;
}

ಸ್ಥಿರ vpiHandle real_var_get_handle(ಪೂರ್ಣಾಙ್ಕ code, vpiHandle ref)
{
      ಸಂರಚನಾ __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ವಿಕಲ್ಪ (code) {

	  ಪಕ್ಷ vpiParent:
	    ಪ್ರತ್ಯಾಗಮ rfp->is_netarray ? rfp->within.parent : ರಿಕ್ತ;

	  ಪಕ್ಷ vpiIndex:
	    ಪ್ರತ್ಯಾಗಮ rfp->is_netarray ? rfp->id.index : ರಿಕ್ತ;

	  ಪಕ್ಷ vpiScope:
	    ಪ್ರತ್ಯಾಗಮ vpip_scope(rfp);

	  ಪಕ್ಷ vpiModule:
	    ಪ್ರತ್ಯಾಗಮ vpip_module(vpip_scope(rfp));
      }

      ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ vpiHandle real_var_iterate(ಪೂರ್ಣಾಙ್ಕ code, vpiHandle ref)
{
      ಸಂರಚನಾ __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ಯದಿ (code == vpiIndex) {
	    ಪ್ರತ್ಯಾಗಮ rfp->is_netarray ? rfp->id.index->vpi_iterate(code) : ರಿಕ್ತ;
      }

      ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ real_var_get_ಮೌಲ್ಯ(vpiHandle ref, s_vpi_ಮೌಲ್ಯ*vp)
{
      ಸಂರಚನಾ __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      vvp_signal_ಮೌಲ್ಯ*fil
	    = dynamic_cast<vvp_signal_ಮೌಲ್ಯ*>(rfp->net->fil);

      fil->get_signal_ಮೌಲ್ಯ(vp);
}

ಸ್ಥಿರ vpiHandle real_var_put_ಮೌಲ್ಯ(vpiHandle ref, p_vpi_ಮೌಲ್ಯ vp, ಪೂರ್ಣಾಙ್ಕ flags)
{
      ಸಂರಚನಾ __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);
      vvp_net_ptr_t destination (rfp->net, 0);

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value. */
      ಯದಿ (flags == vpiReleaseFlag) {
	    ಪ್ರತಿಜ್ಞಾ(rfp->net->fil);
	    rfp->net->fil->force_unlink();
	    rfp->net->fil->release(destination, rfp->is_wire);
	    real_var_get_ಮೌಲ್ಯ(ref, vp);
	    ಪ್ರತ್ಯಾಗಮ ref;
      }

      ದ್ವಿಗುಣ result = real_from_vpi_ಮೌಲ್ಯ(vp);

      ಯದಿ (flags == vpiForceFlag) {
	    vvp_vector2_t mask (vvp_vector2_t::FILL1, 1);
	    rfp->net->force_real(result, mask);
      } ಅನ್ಯಥಾ ಯದಿ (rfp->is_wire) {
	    rfp->net->send_real(result, vthread_get_wt_context());
      } ಅನ್ಯಥಾ {
	    vvp_send_real(destination, result, vthread_get_wt_context());
      }
      ಪ್ರತ್ಯಾಗಮ 0;
}

ಅನ್ತಃಸ್ಥ __vpiRealVar::__vpiRealVar()
{ }

ಪೂರ್ಣಾಙ್ಕ __vpiRealVar::get_type_code(ಶೂನ್ಯ) ಧ್ರುವ
{ ಪ್ರತ್ಯಾಗಮ vpiRealVar; }

ಪೂರ್ಣಾಙ್ಕ __vpiRealVar::vpi_get(ಪೂರ್ಣಾಙ್ಕ code)
{ ಪ್ರತ್ಯಾಗಮ real_var_get(code, ಏತತ್); }

ವರ್ಣ* __vpiRealVar::vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code)
{ ಪ್ರತ್ಯಾಗಮ real_var_get_str(code, ಏತತ್); }

ಶೂನ್ಯ __vpiRealVar::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val)
{ real_var_get_ಮೌಲ್ಯ(ಏತತ್, val); }

vpiHandle __vpiRealVar::vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಙ್ಕ flags)
{ ಪ್ರತ್ಯಾಗಮ real_var_put_ಮೌಲ್ಯ(ಏತತ್, val, flags); }

vpiHandle __vpiRealVar::vpi_handle(ಪೂರ್ಣಾಙ್ಕ code)
{ ಪ್ರತ್ಯಾಗಮ real_var_get_handle(code, ಏತತ್); }

vpiHandle __vpiRealVar::vpi_iterate(ಪೂರ್ಣಾಙ್ಕ code)
{ ಪ್ರತ್ಯಾಗಮ real_var_iterate(code, ಏತತ್); }

ಸ್ಥಿರ vpiHandle vpip_make_real_(__vpiScope*scope, ಧ್ರುವ ವರ್ಣ*name,
				 vvp_net_t*net, ತಾರ್ಕಿಕ is_wire)
{
      ಸಂರಚನಾ __vpiRealVar*obj = ನೂತನ __vpiRealVar;

      obj->id.name = name ? vpip_name_string(name) : ರಿಕ್ತ;
      obj->is_netarray = 0;
      obj->is_wire = is_wire;
      obj->net = net;

      obj->within.scope = scope;

      ಪ್ರತ್ಯಾಗಮ obj;
}

vpiHandle vpip_make_real_var(ಧ್ರುವ ವರ್ಣ*name, vvp_net_t*net)
{
      ಪ್ರತ್ಯಾಗಮ vpip_make_real_(vpip_peek_current_scope(), name, net, ಅಸತ್ಯ);
}

vpiHandle vpip_make_real_net(__vpiScope*scope,
			     ಧ್ರುವ ವರ್ಣ*name, vvp_net_t*net)
{
      ಪ್ರತ್ಯಾಗಮ vpip_make_real_(scope, name, net, ಸತ್ಯ);
}

#ifdef CHECK_WITH_VALGRIND
ಶೂನ್ಯ real_delete(vpiHandle item)
{
      ಸಂರಚನಾ __vpiRealVar*obj = dynamic_cast<__vpiRealVar*>(item);
      ಪ್ರತಿಜ್ಞಾ(obj->net->fil);
      obj->net->fil->clear_all_callbacks();
      vvp_net_delete(obj->net);

      delete obj;
}
#endif
