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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#ಆದರೆ_ಅಂತ್ಯ
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <cassert>
# include  "ivl_alloc.h"

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ real_var_get(ಪೂರ್ಣಾಂಕ code, vpiHandle ref)
{
      ರಚನೆ __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ಆಯ್ಕೆ (code) {
	ಸ್ಥಿತಿ vpiArray:
	    ಹಿಂತಿರುಗಿ rfp->is_netarray != 0;

	ಸ್ಥಿತಿ vpi_ಗಾತ್ರ:
	    ಹಿಂತಿರುಗಿ 1;

	ಸ್ಥಿತಿ vpi_ಸಾಲು_No:
	    ಹಿಂತಿರುಗಿ 0; // Not implemented for now!

	ಸ್ಥಿತಿ vpiAutomatic:
	  ಹಿಂತಿರುಗಿ vpip_scope(rfp)->is_automatic()? 1 : 0;
      }

      ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಅಕ್ಷರ* real_var_get_str(ಪೂರ್ಣಾಂಕ code, vpiHandle ref)
{
      ರಚನೆ __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ಆದರೆ (code == vpiFile) {  // Not implemented for now!
	    ಹಿಂತಿರುಗಿ simple_ಗಣ_rbuf_str(file_ಹೆಸರುಗಳು[0]);
      }

      ಅಕ್ಷರ *nm;
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ixs;
      ಆದರೆ (rfp->is_netarray) {
	    nm = strdup(vpi_get_str(vpiName, rfp->within.ಪೋಷಕ));
	    s_vpi_ಮೌಲ್ಯ vp;
	    vp.format = vpiDecStrVal;
	    vpi_get_ಮೌಲ್ಯ(rfp->id.ಸೂಚ್ಯಂಕ, &vp);
	    ixs = vp.value.str;  /* do I need to strdup() this? */
      } ಇಲ್ಲದಿದ್ದರೆ {
	    nm = strdup(rfp->id.name);
	    ixs = ಖಾಲಿ;
      }

      ಅಕ್ಷರ *rbuf = generic_get_str(code, vpip_scope(rfp), nm, ixs);
      ಬಿಡುಗಡೆ(nm);
      ಹಿಂತಿರುಗಿ rbuf;
}

ಸ್ಥಿರ vpiHandle real_var_get_handle(ಪೂರ್ಣಾಂಕ code, vpiHandle ref)
{
      ರಚನೆ __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ಆಯ್ಕೆ (code) {

	  ಸ್ಥಿತಿ vpi_ಪೋಷಕ:
	    ಹಿಂತಿರುಗಿ rfp->is_netarray ? rfp->within.ಪೋಷಕ : ಖಾಲಿ;

	  ಸ್ಥಿತಿ vpi_ಸೂಚ್ಯಂಕ_2:
	    ಹಿಂತಿರುಗಿ rfp->is_netarray ? rfp->id.ಸೂಚ್ಯಂಕ : ಖಾಲಿ;

	  ಸ್ಥಿತಿ vpiScope:
	    ಹಿಂತಿರುಗಿ vpip_scope(rfp);

	  ಸ್ಥಿತಿ vpiModule:
	    ಹಿಂತಿರುಗಿ vpip_module(vpip_scope(rfp));
      }

      ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ vpiHandle real_var_iterate(ಪೂರ್ಣಾಂಕ code, vpiHandle ref)
{
      ರಚನೆ __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ಆದರೆ (code == vpi_ಸೂಚ್ಯಂಕ_2) {
	    ಹಿಂತಿರುಗಿ rfp->is_netarray ? rfp->id.ಸೂಚ್ಯಂಕ->vpi_iterate(code) : ಖಾಲಿ;
      }

      ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ real_var_get_ಮೌಲ್ಯ(vpiHandle ref, s_vpi_ಮೌಲ್ಯ*vp)
{
      ರಚನೆ __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      vvp_signal_ಮೌಲ್ಯ*fil
	    = dynamic_cast<vvp_signal_ಮೌಲ್ಯ*>(rfp->net->fil);

      fil->get_signal_ಮೌಲ್ಯ(vp);
}

ಸ್ಥಿರ vpiHandle real_var_put_ಮೌಲ್ಯ(vpiHandle ref, p_vpi_ಮೌಲ್ಯ vp, ಪೂರ್ಣಾಂಕ flags)
{
      ರಚನೆ __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);
      vvp_net_ptr_t destination (rfp->net, 0);

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value. */
      ಆದರೆ (flags == vpiReleaseFlag) {
	    ಪ್ರತಿಜ್ಞಾ(rfp->net->fil);
	    rfp->net->fil->force_unlink();
	    rfp->net->fil->release(destination, rfp->is_wire);
	    real_var_get_ಮೌಲ್ಯ(ref, vp);
	    ಹಿಂತಿರುಗಿ ref;
      }

      ದ್ವಿಗುಣ ಫಲಿತಾಂಶ = real_from_vpi_ಮೌಲ್ಯ(vp);

      ಆದರೆ (flags == vpiForceFlag) {
	    vvp_vector2_t ಮುಖವಾಡ (vvp_vector2_t::FILL1, 1);
	    rfp->net->force_real(ಫಲಿತಾಂಶ, ಮುಖವಾಡ);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (rfp->is_wire) {
	    rfp->net->send_real(ಫಲಿತಾಂಶ, vthread_get_wt_context());
      } ಇಲ್ಲದಿದ್ದರೆ {
	    vvp_send_real(destination, ಫಲಿತಾಂಶ, vthread_get_wt_context());
      }
      ಹಿಂತಿರುಗಿ 0;
}

ಸಾಲೊಳಗೆ __vpiRealVar::__vpiRealVar()
{ }

ಪೂರ್ಣಾಂಕ __vpiRealVar::get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ
{ ಹಿಂತಿರುಗಿ vpiRealVar; }

ಪೂರ್ಣಾಂಕ __vpiRealVar::vpi_get(ಪೂರ್ಣಾಂಕ code)
{ ಹಿಂತಿರುಗಿ real_var_get(code, ಇದು); }

ಅಕ್ಷರ* __vpiRealVar::vpi_get_str(ಪೂರ್ಣಾಂಕ code)
{ ಹಿಂತಿರುಗಿ real_var_get_str(code, ಇದು); }

ಶೂನ್ಯ __vpiRealVar::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val)
{ real_var_get_ಮೌಲ್ಯ(ಇದು, val); }

vpiHandle __vpiRealVar::vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಂಕ flags)
{ ಹಿಂತಿರುಗಿ real_var_put_ಮೌಲ್ಯ(ಇದು, val, flags); }

vpiHandle __vpiRealVar::vpi_handle(ಪೂರ್ಣಾಂಕ code)
{ ಹಿಂತಿರುಗಿ real_var_get_handle(code, ಇದು); }

vpiHandle __vpiRealVar::vpi_iterate(ಪೂರ್ಣಾಂಕ code)
{ ಹಿಂತಿರುಗಿ real_var_iterate(code, ಇದು); }

ಸ್ಥಿರ vpiHandle vpip_make_real_(__vpiScope*scope, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name,
				 vvp_net_t*net, ತಾರ್ಕಿಕ is_wire)
{
      ರಚನೆ __vpiRealVar*obj = ಹೊಸ __vpiRealVar;

      obj->id.name = name ? vpip_name_string(name) : ಖಾಲಿ;
      obj->is_netarray = 0;
      obj->is_wire = is_wire;
      obj->net = net;

      obj->within.scope = scope;

      ಹಿಂತಿರುಗಿ obj;
}

vpiHandle vpip_make_real_var(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, vvp_net_t*net)
{
      ಹಿಂತಿರುಗಿ vpip_make_real_(vpip_peek_current_scope(), name, net, ಸುಳ್ಳು);
}

vpiHandle vpip_make_real_net(__vpiScope*scope,
			     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, vvp_net_t*net)
{
      ಹಿಂತಿರುಗಿ vpip_make_real_(scope, name, net, ನಿಜ);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಶೂನ್ಯ real_delete(vpiHandle ಅಂಶ)
{
      ರಚನೆ __vpiRealVar*obj = dynamic_cast<__vpiRealVar*>(ಅಂಶ);
      ಪ್ರತಿಜ್ಞಾ(obj->net->fil);
      obj->net->fil->clear_all_callbacks();
      vvp_net_delete(obj->net);

      ಅಳಿಸು obj;
}
#ಆದರೆ_ಅಂತ್ಯ
