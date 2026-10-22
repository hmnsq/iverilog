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
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#اگر_ختم
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <cassert>
# include  "ivl_alloc.h"

ساکن صحیح real_var_get(صحیح code, vpiHandle ref)
{
      ساخت __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      أكد(rfp);

      انتخاب (code) {
	حالت vpiArray:
	    واپس rfp->is_netarray != 0;

	حالت vpi_سائز:
	    واپس 1;

	حالت vpi_سطر_No:
	    واپس 0; // Not implemented for now!

	حالت vpiAutomatic:
	  واپس vpip_scope(rfp)->is_automatic()? 1 : 0;
      }

      واپس 0;
}

ساکن حرف* real_var_get_str(صحیح code, vpiHandle ref)
{
      ساخت __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      أكد(rfp);

      اگر (code == vpiFile) {  // Not implemented for now!
	    واپس simple_سیٹ_rbuf_str(file_نام[0]);
      }

      حرف *nm;
      ثابت حرف *ixs;
      اگر (rfp->is_netarray) {
	    nm = strdup(vpi_get_str(vpiName, rfp->within.والد));
	    s_vpi_قدر vp;
	    vp.format = vpiDecStrVal;
	    vpi_get_قدر(rfp->id.اشاریہ, &vp);
	    ixs = vp.value.str;  /* do I need to strdup() this? */
      } ورنہ {
	    nm = strdup(rfp->id.name);
	    ixs = تہی;
      }

      حرف *rbuf = generic_get_str(code, vpip_scope(rfp), nm, ixs);
      آزاد(nm);
      واپس rbuf;
}

ساکن vpiHandle real_var_get_handle(صحیح code, vpiHandle ref)
{
      ساخت __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      أكد(rfp);

      انتخاب (code) {

	  حالت vpi_والد:
	    واپس rfp->is_netarray ? rfp->within.والد : تہی;

	  حالت vpi_اشاریہ_2:
	    واپس rfp->is_netarray ? rfp->id.اشاریہ : تہی;

	  حالت vpiScope:
	    واپس vpip_scope(rfp);

	  حالت vpiModule:
	    واپس vpip_module(vpip_scope(rfp));
      }

      واپس 0;
}

ساکن vpiHandle real_var_iterate(صحیح code, vpiHandle ref)
{
      ساخت __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      أكد(rfp);

      اگر (code == vpi_اشاریہ_2) {
	    واپس rfp->is_netarray ? rfp->id.اشاریہ->vpi_iterate(code) : تہی;
      }

      واپس 0;
}

ساکن خالی real_var_get_قدر(vpiHandle ref, s_vpi_قدر*vp)
{
      ساخت __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      أكد(rfp);

      vvp_signal_قدر*fil
	    = dynamic_cast<vvp_signal_قدر*>(rfp->net->fil);

      fil->get_signal_قدر(vp);
}

ساکن vpiHandle real_var_put_قدر(vpiHandle ref, p_vpi_قدر vp, صحیح flags)
{
      ساخت __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      أكد(rfp);
      vvp_net_ptr_t destination (rfp->net, 0);

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value. */
      اگر (flags == vpiReleaseFlag) {
	    أكد(rfp->net->fil);
	    rfp->net->fil->force_unlink();
	    rfp->net->fil->release(destination, rfp->is_wire);
	    real_var_get_قدر(ref, vp);
	    واپس ref;
      }

      دوہرا نتیجہ = real_from_vpi_قدر(vp);

      اگر (flags == vpiForceFlag) {
	    vvp_vector2_t نقاب (vvp_vector2_t::FILL1, 1);
	    rfp->net->force_real(نتیجہ, نقاب);
      } ورنہ اگر (rfp->is_wire) {
	    rfp->net->send_real(نتیجہ, vthread_get_wt_context());
      } ورنہ {
	    vvp_send_real(destination, نتیجہ, vthread_get_wt_context());
      }
      واپس 0;
}

اندرونی __vpiRealVar::__vpiRealVar()
{ }

صحیح __vpiRealVar::get_type_رمز(خالی) ثابت
{ واپس vpiRealVar; }

صحیح __vpiRealVar::vpi_get(صحیح code)
{ واپس real_var_get(code, یہ); }

حرف* __vpiRealVar::vpi_get_str(صحیح code)
{ واپس real_var_get_str(code, یہ); }

خالی __vpiRealVar::vpi_get_قدر(p_vpi_قدر val)
{ real_var_get_قدر(یہ, val); }

vpiHandle __vpiRealVar::vpi_put_قدر(p_vpi_قدر val, صحیح flags)
{ واپس real_var_put_قدر(یہ, val, flags); }

vpiHandle __vpiRealVar::vpi_handle(صحیح code)
{ واپس real_var_get_handle(code, یہ); }

vpiHandle __vpiRealVar::vpi_iterate(صحیح code)
{ واپس real_var_iterate(code, یہ); }

ساکن vpiHandle vpip_make_real_(__vpiScope*scope, ثابت حرف*name,
				 vvp_net_t*net, منطقی is_wire)
{
      ساخت __vpiRealVar*obj = نیا __vpiRealVar;

      obj->id.name = name ? vpip_name_string(name) : تہی;
      obj->is_netarray = 0;
      obj->is_wire = is_wire;
      obj->net = net;

      obj->within.scope = scope;

      واپس obj;
}

vpiHandle vpip_make_real_var(ثابت حرف*name, vvp_net_t*net)
{
      واپس vpip_make_real_(vpip_peek_current_scope(), name, net, جھوٹ);
}

vpiHandle vpip_make_real_net(__vpiScope*scope,
			     ثابت حرف*name, vvp_net_t*net)
{
      واپس vpip_make_real_(scope, name, net, سچ);
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
خالی real_delete(vpiHandle شے)
{
      ساخت __vpiRealVar*obj = dynamic_cast<__vpiRealVar*>(شے);
      أكد(obj->net->fil);
      obj->net->fil->clear_all_callbacks();
      vvp_net_delete(obj->net);

      حذف obj;
}
#اگر_ختم
