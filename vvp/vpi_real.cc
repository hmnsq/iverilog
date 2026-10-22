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
#अगर_घोषित CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#पूर्ण_अगर
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <cassert>
# include  "ivl_alloc.h"

अटल पूर्णांक real_var_get(पूर्णांक code, vpiHandle ref)
{
      काष्ठा __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      निश्चित(rfp);

      चयन (code) {
	हाल vpiArray:
	    वापस rfp->is_netarray != 0;

	हाल vpi_आकार:
	    वापस 1;

	हाल vpi_पंक्ति_No:
	    वापस 0; // Not implemented for now!

	हाल vpiAutomatic:
	  वापस vpip_scope(rfp)->is_automatic()? 1 : 0;
      }

      वापस 0;
}

अटल अक्षर* real_var_get_str(पूर्णांक code, vpiHandle ref)
{
      काष्ठा __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      निश्चित(rfp);

      अगर (code == vpiFile) {  // Not implemented for now!
	    वापस simple_set_rbuf_str(file_names[0]);
      }

      अक्षर *nm;
      स्थिर अक्षर *ixs;
      अगर (rfp->is_netarray) {
	    nm = strdup(vpi_get_str(vpiName, rfp->within.parent));
	    s_vpi_मूल्य vp;
	    vp.format = vpiDecStrVal;
	    vpi_get_मूल्य(rfp->id.सूचकांक, &vp);
	    ixs = vp.value.str;  /* do I need to strdup() this? */
      } अन्यथा {
	    nm = strdup(rfp->id.name);
	    ixs = शून्य;
      }

      अक्षर *rbuf = generic_get_str(code, vpip_scope(rfp), nm, ixs);
      मुक्त(nm);
      वापस rbuf;
}

अटल vpiHandle real_var_get_handle(पूर्णांक code, vpiHandle ref)
{
      काष्ठा __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      निश्चित(rfp);

      चयन (code) {

	  हाल vpiParent:
	    वापस rfp->is_netarray ? rfp->within.parent : शून्य;

	  हाल vpi_सूचकांक_2:
	    वापस rfp->is_netarray ? rfp->id.सूचकांक : शून्य;

	  हाल vpiScope:
	    वापस vpip_scope(rfp);

	  हाल vpiModule:
	    वापस vpip_module(vpip_scope(rfp));
      }

      वापस 0;
}

अटल vpiHandle real_var_iterate(पूर्णांक code, vpiHandle ref)
{
      काष्ठा __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      निश्चित(rfp);

      अगर (code == vpi_सूचकांक_2) {
	    वापस rfp->is_netarray ? rfp->id.सूचकांक->vpi_iterate(code) : शून्य;
      }

      वापस 0;
}

अटल व्योम real_var_get_मूल्य(vpiHandle ref, s_vpi_मूल्य*vp)
{
      काष्ठा __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      निश्चित(rfp);

      vvp_signal_मूल्य*fil
	    = dynamic_cast<vvp_signal_मूल्य*>(rfp->net->fil);

      fil->get_signal_मूल्य(vp);
}

अटल vpiHandle real_var_put_मूल्य(vpiHandle ref, p_vpi_मूल्य vp, पूर्णांक flags)
{
      काष्ठा __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      निश्चित(rfp);
      vvp_net_ptr_t destination (rfp->net, 0);

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value. */
      अगर (flags == vpiReleaseFlag) {
	    निश्चित(rfp->net->fil);
	    rfp->net->fil->force_unlink();
	    rfp->net->fil->release(destination, rfp->is_wire);
	    real_var_get_मूल्य(ref, vp);
	    वापस ref;
      }

      द्विगुन परिणाम = real_from_vpi_मूल्य(vp);

      अगर (flags == vpiForceFlag) {
	    vvp_vector2_t mask (vvp_vector2_t::FILL1, 1);
	    rfp->net->force_real(परिणाम, mask);
      } अन्यथा अगर (rfp->is_wire) {
	    rfp->net->send_real(परिणाम, vthread_get_wt_context());
      } अन्यथा {
	    vvp_send_real(destination, परिणाम, vthread_get_wt_context());
      }
      वापस 0;
}

अंतरभूत __vpiRealVar::__vpiRealVar()
{ }

पूर्णांक __vpiRealVar::get_type_code(व्योम) स्थिर
{ वापस vpiRealVar; }

पूर्णांक __vpiRealVar::vpi_get(पूर्णांक code)
{ वापस real_var_get(code, यह); }

अक्षर* __vpiRealVar::vpi_get_str(पूर्णांक code)
{ वापस real_var_get_str(code, यह); }

व्योम __vpiRealVar::vpi_get_मूल्य(p_vpi_मूल्य val)
{ real_var_get_मूल्य(यह, val); }

vpiHandle __vpiRealVar::vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णांक flags)
{ वापस real_var_put_मूल्य(यह, val, flags); }

vpiHandle __vpiRealVar::vpi_handle(पूर्णांक code)
{ वापस real_var_get_handle(code, यह); }

vpiHandle __vpiRealVar::vpi_iterate(पूर्णांक code)
{ वापस real_var_iterate(code, यह); }

अटल vpiHandle vpip_make_real_(__vpiScope*scope, स्थिर अक्षर*name,
				 vvp_net_t*net, bool is_wire)
{
      काष्ठा __vpiRealVar*obj = नया __vpiRealVar;

      obj->id.name = name ? vpip_name_string(name) : शून्य;
      obj->is_netarray = 0;
      obj->is_wire = is_wire;
      obj->net = net;

      obj->within.scope = scope;

      वापस obj;
}

vpiHandle vpip_make_real_var(स्थिर अक्षर*name, vvp_net_t*net)
{
      वापस vpip_make_real_(vpip_peek_current_scope(), name, net, false);
}

vpiHandle vpip_make_real_net(__vpiScope*scope,
			     स्थिर अक्षर*name, vvp_net_t*net)
{
      वापस vpip_make_real_(scope, name, net, true);
}

#अगर_घोषित CHECK_WITH_VALGRIND
व्योम real_delete(vpiHandle item)
{
      काष्ठा __vpiRealVar*obj = dynamic_cast<__vpiRealVar*>(item);
      निश्चित(obj->net->fil);
      obj->net->fil->clear_all_callbacks();
      vvp_net_delete(obj->net);

      मिटाओ obj;
}
#पूर्ण_अगर
