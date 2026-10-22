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

স্থির পূর্ণাঙ্ক real_var_get(পূর্ণাঙ্ক code, vpiHandle ref)
{
      সংরচনা __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      প্রতিজ্ঞা(rfp);

      ৱিকল্প (code) {
	পক্ষ vpiArray:
	    ঘূৰাই rfp->is_netarray != 0;

	পক্ষ vpiSize:
	    ঘূৰাই 1;

	পক্ষ vpi_সাৰি_No:
	    ঘূৰাই 0; // Not implemented for now!

	পক্ষ vpiAutomatic:
	  ঘূৰাই vpip_scope(rfp)->is_automatic()? 1 : 0;
      }

      ঘূৰাই 0;
}

স্থির ৱর্ণ* real_var_get_str(পূর্ণাঙ্ক code, vpiHandle ref)
{
      সংরচনা __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      প্রতিজ্ঞা(rfp);

      যদি (code == vpiFile) {  // Not implemented for now!
	    ঘূৰাই simple_set_rbuf_str(file_names[0]);
      }

      ৱর্ণ *nm;
      ধ্রুৱ ৱর্ণ *ixs;
      যদি (rfp->is_netarray) {
	    nm = strdup(vpi_get_str(vpiName, rfp->within.parent));
	    s_vpi_মান vp;
	    vp.format = vpiDecStrVal;
	    vpi_get_মান(rfp->id.index, &vp);
	    ixs = vp.value.str;  /* do I need to strdup() this? */
      } অন্যথা {
	    nm = strdup(rfp->id.name);
	    ixs = রিক্ত;
      }

      ৱর্ণ *rbuf = generic_get_str(code, vpip_scope(rfp), nm, ixs);
      মুক্ত(nm);
      ঘূৰাই rbuf;
}

স্থির vpiHandle real_var_get_handle(পূর্ণাঙ্ক code, vpiHandle ref)
{
      সংরচনা __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      প্রতিজ্ঞা(rfp);

      ৱিকল্প (code) {

	  পক্ষ vpiParent:
	    ঘূৰাই rfp->is_netarray ? rfp->within.parent : রিক্ত;

	  পক্ষ vpiIndex:
	    ঘূৰাই rfp->is_netarray ? rfp->id.index : রিক্ত;

	  পক্ষ vpiScope:
	    ঘূৰাই vpip_scope(rfp);

	  পক্ষ vpiModule:
	    ঘূৰাই vpip_module(vpip_scope(rfp));
      }

      ঘূৰাই 0;
}

স্থির vpiHandle real_var_iterate(পূর্ণাঙ্ক code, vpiHandle ref)
{
      সংরচনা __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      প্রতিজ্ঞা(rfp);

      যদি (code == vpiIndex) {
	    ঘূৰাই rfp->is_netarray ? rfp->id.index->vpi_iterate(code) : রিক্ত;
      }

      ঘূৰাই 0;
}

স্থির শূন্য real_var_get_মান(vpiHandle ref, s_vpi_মান*vp)
{
      সংরচনা __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      প্রতিজ্ঞা(rfp);

      vvp_signal_মান*fil
	    = dynamic_cast<vvp_signal_মান*>(rfp->net->fil);

      fil->get_signal_মান(vp);
}

স্থির vpiHandle real_var_put_মান(vpiHandle ref, p_vpi_মান vp, পূর্ণাঙ্ক flags)
{
      সংরচনা __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      প্রতিজ্ঞা(rfp);
      vvp_net_ptr_t destination (rfp->net, 0);

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value. */
      যদি (flags == vpiReleaseFlag) {
	    প্রতিজ্ঞা(rfp->net->fil);
	    rfp->net->fil->force_unlink();
	    rfp->net->fil->release(destination, rfp->is_wire);
	    real_var_get_মান(ref, vp);
	    ঘূৰাই ref;
      }

      দ্ৱিগুণ result = real_from_vpi_মান(vp);

      যদি (flags == vpiForceFlag) {
	    vvp_vector2_t mask (vvp_vector2_t::FILL1, 1);
	    rfp->net->force_real(result, mask);
      } অন্যথা যদি (rfp->is_wire) {
	    rfp->net->send_real(result, vthread_get_wt_context());
      } অন্যথা {
	    vvp_send_real(destination, result, vthread_get_wt_context());
      }
      ঘূৰাই 0;
}

অন্তঃস্থ __vpiRealVar::__vpiRealVar()
{ }

পূর্ণাঙ্ক __vpiRealVar::get_type_code(শূন্য) ধ্রুৱ
{ ঘূৰাই vpiRealVar; }

পূর্ণাঙ্ক __vpiRealVar::vpi_get(পূর্ণাঙ্ক code)
{ ঘূৰাই real_var_get(code, এতত্); }

ৱর্ণ* __vpiRealVar::vpi_get_str(পূর্ণাঙ্ক code)
{ ঘূৰাই real_var_get_str(code, এতত্); }

শূন্য __vpiRealVar::vpi_get_মান(p_vpi_মান val)
{ real_var_get_মান(এতত্, val); }

vpiHandle __vpiRealVar::vpi_put_মান(p_vpi_মান val, পূর্ণাঙ্ক flags)
{ ঘূৰাই real_var_put_মান(এতত্, val, flags); }

vpiHandle __vpiRealVar::vpi_handle(পূর্ণাঙ্ক code)
{ ঘূৰাই real_var_get_handle(code, এতত্); }

vpiHandle __vpiRealVar::vpi_iterate(পূর্ণাঙ্ক code)
{ ঘূৰাই real_var_iterate(code, এতত্); }

স্থির vpiHandle vpip_make_real_(__vpiScope*scope, ধ্রুৱ ৱর্ণ*name,
				 vvp_net_t*net, তার্কিক is_wire)
{
      সংরচনা __vpiRealVar*obj = নূতন __vpiRealVar;

      obj->id.name = name ? vpip_name_string(name) : রিক্ত;
      obj->is_netarray = 0;
      obj->is_wire = is_wire;
      obj->net = net;

      obj->within.scope = scope;

      ঘূৰাই obj;
}

vpiHandle vpip_make_real_var(ধ্রুৱ ৱর্ণ*name, vvp_net_t*net)
{
      ঘূৰাই vpip_make_real_(vpip_peek_current_scope(), name, net, মিছা);
}

vpiHandle vpip_make_real_net(__vpiScope*scope,
			     ধ্রুৱ ৱর্ণ*name, vvp_net_t*net)
{
      ঘূৰাই vpip_make_real_(scope, name, net, সঁচা);
}

#ifdef CHECK_WITH_VALGRIND
শূন্য real_delete(vpiHandle item)
{
      সংরচনা __vpiRealVar*obj = dynamic_cast<__vpiRealVar*>(item);
      প্রতিজ্ঞা(obj->net->fil);
      obj->net->fil->clear_all_callbacks();
      vvp_net_delete(obj->net);

      delete obj;
}
#endif
