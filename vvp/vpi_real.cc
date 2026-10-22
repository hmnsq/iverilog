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

സ്ഥിതം പൂർണ്ണസംഖ്യ real_var_get(പൂർണ്ണസംഖ്യ code, vpiHandle ref)
{
      ഘടന __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      പ്രതിജ്ഞാ(rfp);

      തിരഞ്ഞെടുപ്പ് (code) {
	അവസ്ഥ vpiArray:
	    തിരികെ rfp->is_netarray != 0;

	അവസ്ഥ vpiSize:
	    തിരികെ 1;

	അവസ്ഥ vpi_വരി_No:
	    തിരികെ 0; // Not implemented for now!

	അവസ്ഥ vpiAutomatic:
	  തിരികെ vpip_scope(rfp)->is_automatic()? 1 : 0;
      }

      തിരികെ 0;
}

സ്ഥിതം അക്ഷരം* real_var_get_str(പൂർണ്ണസംഖ്യ code, vpiHandle ref)
{
      ഘടന __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      പ്രതിജ്ഞാ(rfp);

      എങ്കിൽ (code == vpiFile) {  // Not implemented for now!
	    തിരികെ simple_set_rbuf_str(file_names[0]);
      }

      അക്ഷരം *nm;
      സ്ഥിരം അക്ഷരം *ixs;
      എങ്കിൽ (rfp->is_netarray) {
	    nm = strdup(vpi_get_str(vpiName, rfp->within.parent));
	    s_vpi_മൂല്യം vp;
	    vp.format = vpiDecStrVal;
	    vpi_get_മൂല്യം(rfp->id.index, &vp);
	    ixs = vp.value.str;  /* do I need to strdup() this? */
      } അല്ലെങ്കിൽ {
	    nm = strdup(rfp->id.name);
	    ixs = ശൂന്യ;
      }

      അക്ഷരം *rbuf = generic_get_str(code, vpip_scope(rfp), nm, ixs);
      സ്വതന്ത്രം(nm);
      തിരികെ rbuf;
}

സ്ഥിതം vpiHandle real_var_get_handle(പൂർണ്ണസംഖ്യ code, vpiHandle ref)
{
      ഘടന __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      പ്രതിജ്ഞാ(rfp);

      തിരഞ്ഞെടുപ്പ് (code) {

	  അവസ്ഥ vpiParent:
	    തിരികെ rfp->is_netarray ? rfp->within.parent : ശൂന്യ;

	  അവസ്ഥ vpiIndex:
	    തിരികെ rfp->is_netarray ? rfp->id.index : ശൂന്യ;

	  അവസ്ഥ vpiScope:
	    തിരികെ vpip_scope(rfp);

	  അവസ്ഥ vpiModule:
	    തിരികെ vpip_module(vpip_scope(rfp));
      }

      തിരികെ 0;
}

സ്ഥിതം vpiHandle real_var_iterate(പൂർണ്ണസംഖ്യ code, vpiHandle ref)
{
      ഘടന __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      പ്രതിജ്ഞാ(rfp);

      എങ്കിൽ (code == vpiIndex) {
	    തിരികെ rfp->is_netarray ? rfp->id.index->vpi_iterate(code) : ശൂന്യ;
      }

      തിരികെ 0;
}

സ്ഥിതം ശൂന്യം real_var_get_മൂല്യം(vpiHandle ref, s_vpi_മൂല്യം*vp)
{
      ഘടന __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      പ്രതിജ്ഞാ(rfp);

      vvp_signal_മൂല്യം*fil
	    = dynamic_cast<vvp_signal_മൂല്യം*>(rfp->net->fil);

      fil->get_signal_മൂല്യം(vp);
}

സ്ഥിതം vpiHandle real_var_put_മൂല്യം(vpiHandle ref, p_vpi_മൂല്യം vp, പൂർണ്ണസംഖ്യ flags)
{
      ഘടന __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      പ്രതിജ്ഞാ(rfp);
      vvp_net_ptr_t destination (rfp->net, 0);

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value. */
      എങ്കിൽ (flags == vpiReleaseFlag) {
	    പ്രതിജ്ഞാ(rfp->net->fil);
	    rfp->net->fil->force_unlink();
	    rfp->net->fil->release(destination, rfp->is_wire);
	    real_var_get_മൂല്യം(ref, vp);
	    തിരികെ ref;
      }

      ഇരട്ട result = real_from_vpi_മൂല്യം(vp);

      എങ്കിൽ (flags == vpiForceFlag) {
	    vvp_vector2_t mask (vvp_vector2_t::FILL1, 1);
	    rfp->net->force_real(result, mask);
      } അല്ലെങ്കിൽ എങ്കിൽ (rfp->is_wire) {
	    rfp->net->send_real(result, vthread_get_wt_context());
      } അല്ലെങ്കിൽ {
	    vvp_send_real(destination, result, vthread_get_wt_context());
      }
      തിരികെ 0;
}

വരിയിൽ __vpiRealVar::__vpiRealVar()
{ }

പൂർണ്ണസംഖ്യ __vpiRealVar::get_type_code(ശൂന്യം) സ്ഥിരം
{ തിരികെ vpiRealVar; }

പൂർണ്ണസംഖ്യ __vpiRealVar::vpi_get(പൂർണ്ണസംഖ്യ code)
{ തിരികെ real_var_get(code, ഇത്); }

അക്ഷരം* __vpiRealVar::vpi_get_str(പൂർണ്ണസംഖ്യ code)
{ തിരികെ real_var_get_str(code, ഇത്); }

ശൂന്യം __vpiRealVar::vpi_get_മൂല്യം(p_vpi_മൂല്യം val)
{ real_var_get_മൂല്യം(ഇത്, val); }

vpiHandle __vpiRealVar::vpi_put_മൂല്യം(p_vpi_മൂല്യം val, പൂർണ്ണസംഖ്യ flags)
{ തിരികെ real_var_put_മൂല്യം(ഇത്, val, flags); }

vpiHandle __vpiRealVar::vpi_handle(പൂർണ്ണസംഖ്യ code)
{ തിരികെ real_var_get_handle(code, ഇത്); }

vpiHandle __vpiRealVar::vpi_iterate(പൂർണ്ണസംഖ്യ code)
{ തിരികെ real_var_iterate(code, ഇത്); }

സ്ഥിതം vpiHandle vpip_make_real_(__vpiScope*scope, സ്ഥിരം അക്ഷരം*name,
				 vvp_net_t*net, തർക്കം is_wire)
{
      ഘടന __vpiRealVar*obj = പുതിയ __vpiRealVar;

      obj->id.name = name ? vpip_name_string(name) : ശൂന്യ;
      obj->is_netarray = 0;
      obj->is_wire = is_wire;
      obj->net = net;

      obj->within.scope = scope;

      തിരികെ obj;
}

vpiHandle vpip_make_real_var(സ്ഥിരം അക്ഷരം*name, vvp_net_t*net)
{
      തിരികെ vpip_make_real_(vpip_peek_current_scope(), name, net, അസത്യം);
}

vpiHandle vpip_make_real_net(__vpiScope*scope,
			     സ്ഥിരം അക്ഷരം*name, vvp_net_t*net)
{
      തിരികെ vpip_make_real_(scope, name, net, സത്യം);
}

#ifdef CHECK_WITH_VALGRIND
ശൂന്യം real_delete(vpiHandle item)
{
      ഘടന __vpiRealVar*obj = dynamic_cast<__vpiRealVar*>(item);
      പ്രതിജ്ഞാ(obj->net->fil);
      obj->net->fil->clear_all_callbacks();
      vvp_net_delete(obj->net);

      delete obj;
}
#endif
