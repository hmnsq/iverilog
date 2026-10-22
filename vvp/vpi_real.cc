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
#నిర్వచితమైతే CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#అయితే_ముగింపు
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <cassert>
# include  "ivl_alloc.h"

స్థిర పూర్ణాంకం real_var_get(పూర్ణాంకం code, vpiHandle ref)
{
      నిర్మాణం __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      ప్రతిజ్ఞా(rfp);

      ఎంపిక (code) {
	స్థితి vpiArray:
	    తిరిగి rfp->is_netarray != 0;

	స్థితి vpi_కొలత:
	    తిరిగి 1;

	స్థితి vpi_పంక్తి_No:
	    తిరిగి 0; // Not implemented for now!

	స్థితి vpiAutomatic:
	  తిరిగి vpip_scope(rfp)->is_automatic()? 1 : 0;
      }

      తిరిగి 0;
}

స్థిర అక్షరం* real_var_get_str(పూర్ణాంకం code, vpiHandle ref)
{
      నిర్మాణం __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      ప్రతిజ్ఞా(rfp);

      ఒకవేళ (code == vpiFile) {  // Not implemented for now!
	    తిరిగి simple_సమితి_rbuf_str(file_పేర్లు[0]);
      }

      అక్షరం *nm;
      స్థిరరాశి అక్షరం *ixs;
      ఒకవేళ (rfp->is_netarray) {
	    nm = strdup(vpi_get_str(vpiName, rfp->within.మాతృ));
	    s_vpi_విలువ vp;
	    vp.format = vpiDecStrVal;
	    vpi_get_విలువ(rfp->id.సూచీ, &vp);
	    ixs = vp.value.str;  /* do I need to strdup() this? */
      } లేకపోతే {
	    nm = strdup(rfp->id.name);
	    ixs = ఖాళీ;
      }

      అక్షరం *rbuf = generic_get_str(code, vpip_scope(rfp), nm, ixs);
      విడుదల(nm);
      తిరిగి rbuf;
}

స్థిర vpiHandle real_var_get_handle(పూర్ణాంకం code, vpiHandle ref)
{
      నిర్మాణం __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      ప్రతిజ్ఞా(rfp);

      ఎంపిక (code) {

	  స్థితి vpi_మాతృ:
	    తిరిగి rfp->is_netarray ? rfp->within.మాతృ : ఖాళీ;

	  స్థితి vpi_సూచీ_2:
	    తిరిగి rfp->is_netarray ? rfp->id.సూచీ : ఖాళీ;

	  స్థితి vpiScope:
	    తిరిగి vpip_scope(rfp);

	  స్థితి vpiModule:
	    తిరిగి vpip_module(vpip_scope(rfp));
      }

      తిరిగి 0;
}

స్థిర vpiHandle real_var_iterate(పూర్ణాంకం code, vpiHandle ref)
{
      నిర్మాణం __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      ప్రతిజ్ఞా(rfp);

      ఒకవేళ (code == vpi_సూచీ_2) {
	    తిరిగి rfp->is_netarray ? rfp->id.సూచీ->vpi_iterate(code) : ఖాళీ;
      }

      తిరిగి 0;
}

స్థిర శూన్య real_var_get_విలువ(vpiHandle ref, s_vpi_విలువ*vp)
{
      నిర్మాణం __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      ప్రతిజ్ఞా(rfp);

      vvp_signal_విలువ*fil
	    = dynamic_cast<vvp_signal_విలువ*>(rfp->net->fil);

      fil->get_signal_విలువ(vp);
}

స్థిర vpiHandle real_var_put_విలువ(vpiHandle ref, p_vpi_విలువ vp, పూర్ణాంకం flags)
{
      నిర్మాణం __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      ప్రతిజ్ఞా(rfp);
      vvp_net_ptr_t destination (rfp->net, 0);

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value. */
      ఒకవేళ (flags == vpiReleaseFlag) {
	    ప్రతిజ్ఞా(rfp->net->fil);
	    rfp->net->fil->force_unlink();
	    rfp->net->fil->release(destination, rfp->is_wire);
	    real_var_get_విలువ(ref, vp);
	    తిరిగి ref;
      }

      ద్విగుణ ఫలితం = real_from_vpi_విలువ(vp);

      ఒకవేళ (flags == vpiForceFlag) {
	    vvp_vector2_t ముసుగు (vvp_vector2_t::FILL1, 1);
	    rfp->net->force_real(ఫలితం, ముసుగు);
      } లేకపోతే ఒకవేళ (rfp->is_wire) {
	    rfp->net->send_real(ఫలితం, vthread_get_wt_context());
      } లేకపోతే {
	    vvp_send_real(destination, ఫలితం, vthread_get_wt_context());
      }
      తిరిగి 0;
}

అంతఃవరుస __vpiRealVar::__vpiRealVar()
{ }

పూర్ణాంకం __vpiRealVar::get_type_సంకేతం(శూన్య) స్థిరరాశి
{ తిరిగి vpiRealVar; }

పూర్ణాంకం __vpiRealVar::vpi_get(పూర్ణాంకం code)
{ తిరిగి real_var_get(code, ఇది); }

అక్షరం* __vpiRealVar::vpi_get_str(పూర్ణాంకం code)
{ తిరిగి real_var_get_str(code, ఇది); }

శూన్య __vpiRealVar::vpi_get_విలువ(p_vpi_విలువ val)
{ real_var_get_విలువ(ఇది, val); }

vpiHandle __vpiRealVar::vpi_put_విలువ(p_vpi_విలువ val, పూర్ణాంకం flags)
{ తిరిగి real_var_put_విలువ(ఇది, val, flags); }

vpiHandle __vpiRealVar::vpi_handle(పూర్ణాంకం code)
{ తిరిగి real_var_get_handle(code, ఇది); }

vpiHandle __vpiRealVar::vpi_iterate(పూర్ణాంకం code)
{ తిరిగి real_var_iterate(code, ఇది); }

స్థిర vpiHandle vpip_make_real_(__vpiScope*scope, స్థిరరాశి అక్షరం*name,
				 vvp_net_t*net, తార్కిక is_wire)
{
      నిర్మాణం __vpiRealVar*obj = కొత్త __vpiRealVar;

      obj->id.name = name ? vpip_name_string(name) : ఖాళీ;
      obj->is_netarray = 0;
      obj->is_wire = is_wire;
      obj->net = net;

      obj->within.scope = scope;

      తిరిగి obj;
}

vpiHandle vpip_make_real_var(స్థిరరాశి అక్షరం*name, vvp_net_t*net)
{
      తిరిగి vpip_make_real_(vpip_peek_current_scope(), name, net, అబద్ధం);
}

vpiHandle vpip_make_real_net(__vpiScope*scope,
			     స్థిరరాశి అక్షరం*name, vvp_net_t*net)
{
      తిరిగి vpip_make_real_(scope, name, net, నిజం);
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
శూన్య real_delete(vpiHandle అంశం)
{
      నిర్మాణం __vpiRealVar*obj = dynamic_cast<__vpiRealVar*>(అంశం);
      ప్రతిజ్ఞా(obj->net->fil);
      obj->net->fil->clear_all_callbacks();
      vvp_net_delete(obj->net);

      తొలగించు obj;
}
#అయితే_ముగింపు
