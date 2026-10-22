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

স্থির পূর্ণসংখ্যা real_var_get(পূর্ণসংখ্যা code, vpiHandle ref)
{
      কাঠামো __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      প্রতিজ্ঞা(rfp);

      নির্বাচন (code) {
	ক্ষেত্র vpiArray:
	    ফেরত rfp->is_netarray != 0;

	ক্ষেত্র vpi_আকার:
	    ফেরত 1;

	ক্ষেত্র vpi_সারি_No:
	    ফেরত 0; // Not implemented for now!

	ক্ষেত্র vpiAutomatic:
	  ফেরত vpip_scope(rfp)->is_automatic()? 1 : 0;
      }

      ফেরত 0;
}

স্থির অক্ষর* real_var_get_str(পূর্ণসংখ্যা code, vpiHandle ref)
{
      কাঠামো __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      প্রতিজ্ঞা(rfp);

      যদি (code == vpiFile) {  // Not implemented for now!
	    ফেরত simple_set_rbuf_str(file_names[0]);
      }

      অক্ষর *nm;
      ধ্রুবক অক্ষর *ixs;
      যদি (rfp->is_netarray) {
	    nm = strdup(vpi_get_str(vpiName, rfp->within.parent));
	    s_vpi_মান vp;
	    vp.format = vpiDecStrVal;
	    vpi_get_মান(rfp->id.সূচক, &vp);
	    ixs = vp.value.str;  /* do I need to strdup() this? */
      } অন্যথা {
	    nm = strdup(rfp->id.name);
	    ixs = ফাঁকা;
      }

      অক্ষর *rbuf = generic_get_str(code, vpip_scope(rfp), nm, ixs);
      মুক্ত(nm);
      ফেরত rbuf;
}

স্থির vpiHandle real_var_get_handle(পূর্ণসংখ্যা code, vpiHandle ref)
{
      কাঠামো __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      প্রতিজ্ঞা(rfp);

      নির্বাচন (code) {

	  ক্ষেত্র vpiParent:
	    ফেরত rfp->is_netarray ? rfp->within.parent : ফাঁকা;

	  ক্ষেত্র vpi_সূচক_2:
	    ফেরত rfp->is_netarray ? rfp->id.সূচক : ফাঁকা;

	  ক্ষেত্র vpiScope:
	    ফেরত vpip_scope(rfp);

	  ক্ষেত্র vpiModule:
	    ফেরত vpip_module(vpip_scope(rfp));
      }

      ফেরত 0;
}

স্থির vpiHandle real_var_iterate(পূর্ণসংখ্যা code, vpiHandle ref)
{
      কাঠামো __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      প্রতিজ্ঞা(rfp);

      যদি (code == vpi_সূচক_2) {
	    ফেরত rfp->is_netarray ? rfp->id.সূচক->vpi_iterate(code) : ফাঁকা;
      }

      ফেরত 0;
}

স্থির শূন্য real_var_get_মান(vpiHandle ref, s_vpi_মান*vp)
{
      কাঠামো __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      প্রতিজ্ঞা(rfp);

      vvp_signal_মান*fil
	    = dynamic_cast<vvp_signal_মান*>(rfp->net->fil);

      fil->get_signal_মান(vp);
}

স্থির vpiHandle real_var_put_মান(vpiHandle ref, p_vpi_মান vp, পূর্ণসংখ্যা flags)
{
      কাঠামো __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
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
	    ফেরত ref;
      }

      দ্বিগুণ ফলাফল = real_from_vpi_মান(vp);

      যদি (flags == vpiForceFlag) {
	    vvp_vector2_t mask (vvp_vector2_t::FILL1, 1);
	    rfp->net->force_real(ফলাফল, mask);
      } অন্যথা যদি (rfp->is_wire) {
	    rfp->net->send_real(ফলাফল, vthread_get_wt_context());
      } অন্যথা {
	    vvp_send_real(destination, ফলাফল, vthread_get_wt_context());
      }
      ফেরত 0;
}

অন্তঃরেখ __vpiRealVar::__vpiRealVar()
{ }

পূর্ণসংখ্যা __vpiRealVar::get_type_code(শূন্য) ধ্রুবক
{ ফেরত vpiRealVar; }

পূর্ণসংখ্যা __vpiRealVar::vpi_get(পূর্ণসংখ্যা code)
{ ফেরত real_var_get(code, এই); }

অক্ষর* __vpiRealVar::vpi_get_str(পূর্ণসংখ্যা code)
{ ফেরত real_var_get_str(code, এই); }

শূন্য __vpiRealVar::vpi_get_মান(p_vpi_মান val)
{ real_var_get_মান(এই, val); }

vpiHandle __vpiRealVar::vpi_put_মান(p_vpi_মান val, পূর্ণসংখ্যা flags)
{ ফেরত real_var_put_মান(এই, val, flags); }

vpiHandle __vpiRealVar::vpi_handle(পূর্ণসংখ্যা code)
{ ফেরত real_var_get_handle(code, এই); }

vpiHandle __vpiRealVar::vpi_iterate(পূর্ণসংখ্যা code)
{ ফেরত real_var_iterate(code, এই); }

স্থির vpiHandle vpip_make_real_(__vpiScope*scope, ধ্রুবক অক্ষর*name,
				 vvp_net_t*net, যৌক্তিক is_wire)
{
      কাঠামো __vpiRealVar*obj = নতুন __vpiRealVar;

      obj->id.name = name ? vpip_name_string(name) : ফাঁকা;
      obj->is_netarray = 0;
      obj->is_wire = is_wire;
      obj->net = net;

      obj->within.scope = scope;

      ফেরত obj;
}

vpiHandle vpip_make_real_var(ধ্রুবক অক্ষর*name, vvp_net_t*net)
{
      ফেরত vpip_make_real_(vpip_peek_current_scope(), name, net, মিথ্যা);
}

vpiHandle vpip_make_real_net(__vpiScope*scope,
			     ধ্রুবক অক্ষর*name, vvp_net_t*net)
{
      ফেরত vpip_make_real_(scope, name, net, সত্য);
}

#ifdef CHECK_WITH_VALGRIND
শূন্য real_delete(vpiHandle item)
{
      কাঠামো __vpiRealVar*obj = dynamic_cast<__vpiRealVar*>(item);
      প্রতিজ্ঞা(obj->net->fil);
      obj->net->fil->clear_all_callbacks();
      vvp_net_delete(obj->net);

      delete obj;
}
#endif
