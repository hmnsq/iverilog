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

ایستا صحیح real_var_get(صحیح code, vpiHandle ref)
{
      ساختار __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      أكد(rfp);

      انتخاب (code) {
	حالت vpiArray:
	    بازگشت rfp->is_netarray != 0;

	حالت vpiSize:
	    بازگشت 1;

	حالت vpi_سطر_No:
	    بازگشت 0; // Not implemented for now!

	حالت vpiAutomatic:
	  بازگشت vpip_scope(rfp)->is_automatic()? 1 : 0;
      }

      بازگشت 0;
}

ایستا نویسه* real_var_get_str(صحیح code, vpiHandle ref)
{
      ساختار __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      أكد(rfp);

      اگر (code == vpiFile) {  // Not implemented for now!
	    بازگشت simple_set_rbuf_str(file_names[0]);
      }

      نویسه *nm;
      ثابت نویسه *ixs;
      اگر (rfp->is_netarray) {
	    nm = strdup(vpi_get_str(vpiName, rfp->within.parent));
	    s_vpi_مقدار vp;
	    vp.format = vpiDecStrVal;
	    vpi_get_مقدار(rfp->id.index, &vp);
	    ixs = vp.value.str;  /* do I need to strdup() this? */
      } وگرنه {
	    nm = strdup(rfp->id.name);
	    ixs = پوچ;
      }

      نویسه *rbuf = generic_get_str(code, vpip_scope(rfp), nm, ixs);
      آزاد(nm);
      بازگشت rbuf;
}

ایستا vpiHandle real_var_get_handle(صحیح code, vpiHandle ref)
{
      ساختار __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      أكد(rfp);

      انتخاب (code) {

	  حالت vpiParent:
	    بازگشت rfp->is_netarray ? rfp->within.parent : پوچ;

	  حالت vpiIndex:
	    بازگشت rfp->is_netarray ? rfp->id.index : پوچ;

	  حالت vpiScope:
	    بازگشت vpip_scope(rfp);

	  حالت vpiModule:
	    بازگشت vpip_module(vpip_scope(rfp));
      }

      بازگشت 0;
}

ایستا vpiHandle real_var_iterate(صحیح code, vpiHandle ref)
{
      ساختار __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      أكد(rfp);

      اگر (code == vpiIndex) {
	    بازگشت rfp->is_netarray ? rfp->id.index->vpi_iterate(code) : پوچ;
      }

      بازگشت 0;
}

ایستا تهی real_var_get_مقدار(vpiHandle ref, s_vpi_مقدار*vp)
{
      ساختار __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      أكد(rfp);

      vvp_signal_مقدار*fil
	    = dynamic_cast<vvp_signal_مقدار*>(rfp->net->fil);

      fil->get_signal_مقدار(vp);
}

ایستا vpiHandle real_var_put_مقدار(vpiHandle ref, p_vpi_مقدار vp, صحیح flags)
{
      ساختار __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      أكد(rfp);
      vvp_net_ptr_t destination (rfp->net, 0);

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value. */
      اگر (flags == vpiReleaseFlag) {
	    أكد(rfp->net->fil);
	    rfp->net->fil->force_unlink();
	    rfp->net->fil->release(destination, rfp->is_wire);
	    real_var_get_مقدار(ref, vp);
	    بازگشت ref;
      }

      دوگانه result = real_from_vpi_مقدار(vp);

      اگر (flags == vpiForceFlag) {
	    vvp_vector2_t mask (vvp_vector2_t::FILL1, 1);
	    rfp->net->force_real(result, mask);
      } وگرنه اگر (rfp->is_wire) {
	    rfp->net->send_real(result, vthread_get_wt_context());
      } وگرنه {
	    vvp_send_real(destination, result, vthread_get_wt_context());
      }
      بازگشت 0;
}

درونخطی __vpiRealVar::__vpiRealVar()
{ }

صحیح __vpiRealVar::get_type_code(تهی) ثابت
{ بازگشت vpiRealVar; }

صحیح __vpiRealVar::vpi_get(صحیح code)
{ بازگشت real_var_get(code, این); }

نویسه* __vpiRealVar::vpi_get_str(صحیح code)
{ بازگشت real_var_get_str(code, این); }

تهی __vpiRealVar::vpi_get_مقدار(p_vpi_مقدار val)
{ real_var_get_مقدار(این, val); }

vpiHandle __vpiRealVar::vpi_put_مقدار(p_vpi_مقدار val, صحیح flags)
{ بازگشت real_var_put_مقدار(این, val, flags); }

vpiHandle __vpiRealVar::vpi_handle(صحیح code)
{ بازگشت real_var_get_handle(code, این); }

vpiHandle __vpiRealVar::vpi_iterate(صحیح code)
{ بازگشت real_var_iterate(code, این); }

ایستا vpiHandle vpip_make_real_(__vpiScope*scope, ثابت نویسه*name,
				 vvp_net_t*net, منطقی is_wire)
{
      ساختار __vpiRealVar*obj = جدید __vpiRealVar;

      obj->id.name = name ? vpip_name_string(name) : پوچ;
      obj->is_netarray = 0;
      obj->is_wire = is_wire;
      obj->net = net;

      obj->within.scope = scope;

      بازگشت obj;
}

vpiHandle vpip_make_real_var(ثابت نویسه*name, vvp_net_t*net)
{
      بازگشت vpip_make_real_(vpip_peek_current_scope(), name, net, نادرست);
}

vpiHandle vpip_make_real_net(__vpiScope*scope,
			     ثابت نویسه*name, vvp_net_t*net)
{
      بازگشت vpip_make_real_(scope, name, net, درست);
}

#ifdef CHECK_WITH_VALGRIND
تهی real_delete(vpiHandle item)
{
      ساختار __vpiRealVar*obj = dynamic_cast<__vpiRealVar*>(item);
      أكد(obj->net->fil);
      obj->net->fil->clear_all_callbacks();
      vvp_net_delete(obj->net);

      delete obj;
}
#endif
