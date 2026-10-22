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

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା real_var_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code, vpiHandle ref)
{
      ସଂରଚନା __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      ପ୍ରତିଜ୍ଞା(rfp);

      ବାଛ (code) {
	ସ୍ଥିତି vpiArray:
	    ଫେରନ୍ତୁ rfp->is_netarray != 0;

	ସ୍ଥିତି vpiSize:
	    ଫେରନ୍ତୁ 1;

	ସ୍ଥିତି vpi_ଧାଡ଼ି_No:
	    ଫେରନ୍ତୁ 0; // Not implemented for now!

	ସ୍ଥିତି vpiAutomatic:
	  ଫେରନ୍ତୁ vpip_scope(rfp)->is_automatic()? 1 : 0;
      }

      ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଅକ୍ଷର* real_var_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code, vpiHandle ref)
{
      ସଂରଚନା __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      ପ୍ରତିଜ୍ଞା(rfp);

      ଯଦି (code == vpiFile) {  // Not implemented for now!
	    ଫେରନ୍ତୁ simple_set_rbuf_str(file_names[0]);
      }

      ଅକ୍ଷର *nm;
      ଅଚଳ ଅକ୍ଷର *ixs;
      ଯଦି (rfp->is_netarray) {
	    nm = strdup(vpi_get_str(vpiName, rfp->within.parent));
	    s_vpi_ମୂଲ୍ୟ vp;
	    vp.format = vpiDecStrVal;
	    vpi_get_ମୂଲ୍ୟ(rfp->id.index, &vp);
	    ixs = vp.value.str;  /* do I need to strdup() this? */
      } ଅନ୍ୟଥା {
	    nm = strdup(rfp->id.name);
	    ixs = ଖାଲି;
      }

      ଅକ୍ଷର *rbuf = generic_get_str(code, vpip_scope(rfp), nm, ixs);
      ମୁକ୍ତ(nm);
      ଫେରନ୍ତୁ rbuf;
}

ସ୍ଥିର vpiHandle real_var_get_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code, vpiHandle ref)
{
      ସଂରଚନା __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      ପ୍ରତିଜ୍ଞା(rfp);

      ବାଛ (code) {

	  ସ୍ଥିତି vpiParent:
	    ଫେରନ୍ତୁ rfp->is_netarray ? rfp->within.parent : ଖାଲି;

	  ସ୍ଥିତି vpiIndex:
	    ଫେରନ୍ତୁ rfp->is_netarray ? rfp->id.index : ଖାଲି;

	  ସ୍ଥିତି vpiScope:
	    ଫେରନ୍ତୁ vpip_scope(rfp);

	  ସ୍ଥିତି vpiModule:
	    ଫେରନ୍ତୁ vpip_module(vpip_scope(rfp));
      }

      ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର vpiHandle real_var_iterate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code, vpiHandle ref)
{
      ସଂରଚନା __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      ପ୍ରତିଜ୍ଞା(rfp);

      ଯଦି (code == vpiIndex) {
	    ଫେରନ୍ତୁ rfp->is_netarray ? rfp->id.index->vpi_iterate(code) : ଖାଲି;
      }

      ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଶୂନ୍ୟ real_var_get_ମୂଲ୍ୟ(vpiHandle ref, s_vpi_ମୂଲ୍ୟ*vp)
{
      ସଂରଚନା __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      ପ୍ରତିଜ୍ଞା(rfp);

      vvp_signal_ମୂଲ୍ୟ*fil
	    = dynamic_cast<vvp_signal_ମୂଲ୍ୟ*>(rfp->net->fil);

      fil->get_signal_ମୂଲ୍ୟ(vp);
}

ସ୍ଥିର vpiHandle real_var_put_ମୂଲ୍ୟ(vpiHandle ref, p_vpi_ମୂଲ୍ୟ vp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
      ସଂରଚନା __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      ପ୍ରତିଜ୍ଞା(rfp);
      vvp_net_ptr_t destination (rfp->net, 0);

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value. */
      ଯଦି (flags == vpiReleaseFlag) {
	    ପ୍ରତିଜ୍ଞା(rfp->net->fil);
	    rfp->net->fil->force_unlink();
	    rfp->net->fil->release(destination, rfp->is_wire);
	    real_var_get_ମୂଲ୍ୟ(ref, vp);
	    ଫେରନ୍ତୁ ref;
      }

      ଦ୍ୱିଗୁଣ result = real_from_vpi_ମୂଲ୍ୟ(vp);

      ଯଦି (flags == vpiForceFlag) {
	    vvp_vector2_t mask (vvp_vector2_t::FILL1, 1);
	    rfp->net->force_real(result, mask);
      } ଅନ୍ୟଥା ଯଦି (rfp->is_wire) {
	    rfp->net->send_real(result, vthread_get_wt_context());
      } ଅନ୍ୟଥା {
	    vvp_send_real(destination, result, vthread_get_wt_context());
      }
      ଫେରନ୍ତୁ 0;
}

ଧାଡ଼ିଭିତରେ __vpiRealVar::__vpiRealVar()
{ }

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiRealVar::get_type_code(ଶୂନ୍ୟ) ଅଚଳ
{ ଫେରନ୍ତୁ vpiRealVar; }

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiRealVar::vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{ ଫେରନ୍ତୁ real_var_get(code, ଏହି); }

ଅକ୍ଷର* __vpiRealVar::vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{ ଫେରନ୍ତୁ real_var_get_str(code, ଏହି); }

ଶୂନ୍ୟ __vpiRealVar::vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val)
{ real_var_get_ମୂଲ୍ୟ(ଏହି, val); }

vpiHandle __vpiRealVar::vpi_put_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{ ଫେରନ୍ତୁ real_var_put_ମୂଲ୍ୟ(ଏହି, val, flags); }

vpiHandle __vpiRealVar::vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{ ଫେରନ୍ତୁ real_var_get_handle(code, ଏହି); }

vpiHandle __vpiRealVar::vpi_iterate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{ ଫେରନ୍ତୁ real_var_iterate(code, ଏହି); }

ସ୍ଥିର vpiHandle vpip_make_real_(__vpiScope*scope, ଅଚଳ ଅକ୍ଷର*name,
				 vvp_net_t*net, ତାର୍କିକ is_wire)
{
      ସଂରଚନା __vpiRealVar*obj = ନୂତନ __vpiRealVar;

      obj->id.name = name ? vpip_name_string(name) : ଖାଲି;
      obj->is_netarray = 0;
      obj->is_wire = is_wire;
      obj->net = net;

      obj->within.scope = scope;

      ଫେରନ୍ତୁ obj;
}

vpiHandle vpip_make_real_var(ଅଚଳ ଅକ୍ଷର*name, vvp_net_t*net)
{
      ଫେରନ୍ତୁ vpip_make_real_(vpip_peek_current_scope(), name, net, ମିଥ୍ୟା);
}

vpiHandle vpip_make_real_net(__vpiScope*scope,
			     ଅଚଳ ଅକ୍ଷର*name, vvp_net_t*net)
{
      ଫେରନ୍ତୁ vpip_make_real_(scope, name, net, ସତ୍ୟ);
}

#ifdef CHECK_WITH_VALGRIND
ଶୂନ୍ୟ real_delete(vpiHandle item)
{
      ସଂରଚନା __vpiRealVar*obj = dynamic_cast<__vpiRealVar*>(item);
      ପ୍ରତିଜ୍ଞା(obj->net->fil);
      obj->net->fil->clear_all_callbacks();
      vvp_net_delete(obj->net);

      delete obj;
}
#endif
