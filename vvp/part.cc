/*
 * Copyright (c) 2004-2025 Stephen Williams (steve@icarus.com)
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

# define __STDC_LIMIT_MACROS
# include  "compile.h"
# include  "part.h"
# include  <cstdlib>
# include  <climits>
# include  <stdint.h>
# include  <iostream>
# include  <cassert>

استعمال نطاق std;

بنية vvp_fun_part_state_s {
      vvp_fun_part_state_s() {}

      vvp_vector4_t بٽ;
};

vvp_fun_part::vvp_fun_part(بنانشان بنياد, بنانشان wid)
: بنياد_2(بنياد), wid_(wid)
{
}

vvp_fun_part::~vvp_fun_part()
{
}

vvp_fun_part_sa::vvp_fun_part_sa(بنانشان بنياد, بنانشان wid)
: vvp_fun_part(بنياد, wid)
{
      net_ = 0;
}

vvp_fun_part_sa::~vvp_fun_part_sa()
{
}

فراغ vvp_fun_part_sa::recv_vec4(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
                                vvp_context_t)
{
      أكد(پورٽ.پورٽ() == 0);

      vvp_vector4_t tmp (ٻٽو_انگ, بنياد_2, wid_);
      إذا (val_ .eeq( tmp ))
	    أرجع;

      val_ = tmp;

      إذا (net_ == 0) {
	    net_ = پورٽ.ptr();
	    وقت_جدول_functor(هذا);
      }
}

/*
 * Handle the case that the part select node is actually fed by a part
 * select assignment. It's not exactly clear what might make this
 * happen, but is does seem to happen and this should have well
 * defined behavior.
 */
فراغ vvp_fun_part_sa::recv_vec4_pv(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
				   بنانشان بنياد, بنانشان vwid, vvp_context_t)
{
      vvp_vector4_t tmp (vwid, BIT4_Z);
      tmp.سيٽ_vec(بنياد_2, val_);
      tmp.سيٽ_vec(بنياد, ٻٽو_انگ);
      recv_vec4(پورٽ, tmp, 0);
}

فراغ vvp_fun_part_sa::لاڳيتو_لاڳيتو()
{
      vvp_net_t*ptr = net_;
      net_ = 0;
      ptr->send_vec4(val_, 0);
}

vvp_fun_part_aa::vvp_fun_part_aa(بنانشان بنياد, بنانشان wid)
: vvp_fun_part(بنياد, wid)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_جوڙيو_شي_منزل_context(هذا, context_scope_);
}

vvp_fun_part_aa::~vvp_fun_part_aa()
{
}

فراغ vvp_fun_part_aa::alloc_instance(vvp_context_t context)
{
      vvp_سيٽ_context_شي(context, context_idx_, جديد vvp_vector4_t);
}

فراغ vvp_fun_part_aa::reset_instance(vvp_context_t context)
{
      vvp_vector4_t*val = static_cast<vvp_vector4_t*>
            (vvp_get_context_شي(context, context_idx_));

      val->سيٽ_منزل_x();
}

#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
فراغ vvp_fun_part_aa::free_instance(vvp_context_t context)
{
      vvp_vector4_t*val = static_cast<vvp_vector4_t*>
            (vvp_get_context_شي(context, context_idx_));
      ميٽيو val;
}
#جيڪڏهن_پورو

فراغ vvp_fun_part_aa::recv_vec4(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
                                vvp_context_t context)
{
      إذا (context) {
            أكد(پورٽ.پورٽ() == 0);

            vvp_vector4_t*val = static_cast<vvp_vector4_t*>
                  (vvp_get_context_شي(context, context_idx_));

            vvp_vector4_t tmp (wid_, BIT4_X);
            لكل (بنانشان idx = 0 ;  idx < wid_ ;  idx += 1) {
                  إذا (idx + بنياد_2 < ٻٽو_انگ.size())
                        tmp.سيٽ_ٻٽو_انگ(idx, ٻٽو_انگ.value(بنياد_2+idx));
            }
            إذا (!val->eeq( tmp )) {
                  *val = tmp;
                  پورٽ.ptr()->send_vec4(tmp, context);
            }
      } وإلا {
            context = context_scope_->جيئرو_contexts;
            طالما (context) {
                  recv_vec4(پورٽ, ٻٽو_انگ, context);
                  context = vvp_get_ايندڙ_context(context);
            }
      }
}

/*
 * Handle the case that the part select node is actually fed by a part
 * select assignment. It's not exactly clear what might make this
 * happen, but is does seem to happen and this should have well
 * defined behavior.
 */
فراغ vvp_fun_part_aa::recv_vec4_pv(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
				   بنانشان بنياد, بنانشان vwid, vvp_context_t context)
{
      إذا (context) {
	    ثابت vvp_vector4_t*val = static_cast<vvp_vector4_t*>
	                                         (vvp_get_context_شي(context,
	                                                               context_idx_));

	    vvp_vector4_t tmp (vwid, BIT4_Z);
	    tmp.سيٽ_vec(بنياد_2, *val);
	    tmp.سيٽ_vec(بنياد, ٻٽو_انگ);
	    recv_vec4(پورٽ, tmp, context);
      } وإلا {
	    context = context_scope_->جيئرو_contexts;
	    طالما (context) {
		  recv_vec4_pv(پورٽ, ٻٽو_انگ, بنياد, vwid, context);
		  context = vvp_get_ايندڙ_context(context);
	    }
      }
}

vvp_fun_part_pv::vvp_fun_part_pv(بنانشان b, بنانشان w, بنانشان v)
: بنياد_2(b), wid_(w), vwid_(v)
{
}

vvp_fun_part_pv::~vvp_fun_part_pv()
{
}

فراغ vvp_fun_part_pv::recv_vec4(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
                                vvp_context_t context)
{
      أكد(پورٽ.پورٽ() == 0);

      إذا (ٻٽو_انگ.size() != wid_) {
	    cerr << "internal error: part_pv data mismatch. "
		 << "base_=" << بنياد_2 << ", wid_=" << wid_
		 << ", vwid_=" << vwid_ << ", bit=" << ٻٽو_انگ
		 << سٽ_پڄاڻي;
      }
      أكد(ٻٽو_انگ.size() == wid_);

      پورٽ.ptr()->send_vec4_pv(ٻٽو_انگ, بنياد_2, vwid_, context);
}

فراغ vvp_fun_part_pv::recv_vec4_pv(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
                                 بنانشان بنياد, بنانشان vwid, vvp_context_t ctx)
{
      أكد(پورٽ.پورٽ() == 0);
      أكد(بنياد + ٻٽو_انگ.size() <= vwid);
      أكد(vwid == wid_);

      vvp_vector4_t tmp(wid_, BIT4_Z);
      tmp.سيٽ_vec(بنياد, ٻٽو_انگ);

      پورٽ.ptr()->send_vec4_pv(tmp, بنياد_2, vwid_, ctx);
}

فراغ vvp_fun_part_pv::recv_vec8(vvp_net_ptr_t پورٽ, ثابت vvp_vector8_t&ٻٽو_انگ)
{
      أكد(پورٽ.پورٽ() == 0);

      إذا (ٻٽو_انگ.size() != wid_) {
	    cerr << "internal error: part_pv (strength-aware) data mismatch. "
		 << "base_=" << بنياد_2 << ", wid_=" << wid_
		 << ", vwid_=" << vwid_ << ", bit=" << ٻٽو_انگ
		 << سٽ_پڄاڻي;
      }
      أكد(ٻٽو_انگ.size() == wid_);

      پورٽ.ptr()->send_vec8_pv(ٻٽو_انگ, بنياد_2, vwid_);
}

vvp_fun_part_var::vvp_fun_part_var(بنانشان w, منطقي is_signed)
: wid_(w), is_signed_(is_signed)
{
}

vvp_fun_part_var::~vvp_fun_part_var()
{
}

منطقي vvp_fun_part_var::recv_vec4_(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
                                  صحيح&بنياد, vvp_vector4_t&ذريعو,
                                  vvp_vector4_t&ref)
{
      int32_t tmp;
      بدل (پورٽ.پورٽ()) {
	  حالة 0:
	    ذريعو = ٻٽو_انگ;
	    اكسر;
	  حالة 1:
	      // INT_MIN is before the vector and is used to
	      // represent a 'bx value on the select input.
	      // It is also used to guard against 32 bit
	      // unsigned -> signed address overflow / wrap around.
	    إذا (!vector4_منزل_قدر(ٻٽو_انگ, tmp, is_signed_) || (!is_signed_ && tmp < 0))
		  tmp = INT32_MIN;
	    إذا (static_cast<صحيح>(tmp) == بنياد) أرجع خطأ;
	    بنياد = tmp;
	    اكسر;
	  افتراضي:
	    fprintf(stderr, "Unsupported port type %u.\n", پورٽ.پورٽ());
	    أكد(0);
	    اكسر;
      }

      vvp_vector4_t res (wid_);

      لكل (بنانشان idx = 0 ;  idx < wid_ ;  idx += 1) {
	    صحيح adr = بنياد+idx;
	    إذا (adr < 0) استمر;
	    إذا ((بنانشان)adr >= ذريعو.size()) اكسر;

	    res.سيٽ_ٻٽو_انگ(idx, ذريعو.value((بنانشان)adr));
      }

      إذا (! ref.eeq(res)) {
	    ref = res;
            أرجع سچ;
      }
      أرجع خطأ;
}

vvp_fun_part_var_sa::vvp_fun_part_var_sa(بنانشان w, منطقي is_signed)
: vvp_fun_part_var(w, is_signed), بنياد_2(0)
{
}

vvp_fun_part_var_sa::~vvp_fun_part_var_sa()
{
}

فراغ vvp_fun_part_var_sa::recv_vec4(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
                                    vvp_context_t)
{
      إذا (recv_vec4_(پورٽ, ٻٽو_انگ, بنياد_2, ذريعو_2, ref_)) {
	    پورٽ.ptr()->send_vec4(ref_, 0);
      }
}

فراغ vvp_fun_part_var_sa::recv_vec4_pv(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
				       بنانشان بنياد, بنانشان vwid, vvp_context_t)
{
      vvp_vector4_t tmp = ذريعو_2;
      إذا (tmp.size() == 0)
	    tmp = vvp_vector4_t(vwid);

      أكد(tmp.size() == vwid);
      tmp.سيٽ_vec(بنياد, ٻٽو_انگ);
      recv_vec4(پورٽ, tmp, 0);
}

بنية vvp_fun_part_var_state_s {
      vvp_fun_part_var_state_s() : بنياد(0) { }

      صحيح بنياد;
      vvp_vector4_t ذريعو;
      vvp_vector4_t ref;
};

vvp_fun_part_var_aa::vvp_fun_part_var_aa(بنانشان w, منطقي is_signed)
: vvp_fun_part_var(w, is_signed)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_جوڙيو_شي_منزل_context(هذا, context_scope_);
}

vvp_fun_part_var_aa::~vvp_fun_part_var_aa()
{
}

فراغ vvp_fun_part_var_aa::alloc_instance(vvp_context_t context)
{
      vvp_سيٽ_context_شي(context, context_idx_, جديد vvp_fun_part_var_state_s);
}

فراغ vvp_fun_part_var_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
            (vvp_get_context_شي(context, context_idx_));

      state->بنياد = 0;
      state->ذريعو.سيٽ_منزل_x();
      state->ref.سيٽ_منزل_x();
}

#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
فراغ vvp_fun_part_var_aa::free_instance(vvp_context_t context)
{
      vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
            (vvp_get_context_شي(context, context_idx_));
      ميٽيو state;
}
#جيڪڏهن_پورو

فراغ vvp_fun_part_var_aa::recv_vec4(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
                                    vvp_context_t context)
{
      إذا (context) {
            vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
                  (vvp_get_context_شي(context, context_idx_));

            إذا (recv_vec4_(پورٽ, ٻٽو_انگ, state->بنياد, state->ذريعو, state->ref)) {
                  پورٽ.ptr()->send_vec4(state->ref, context);
            }
      } وإلا {
            context = context_scope_->جيئرو_contexts;
            طالما (context) {
                  recv_vec4(پورٽ, ٻٽو_انگ, context);
                  context = vvp_get_ايندڙ_context(context);
            }
      }
}

فراغ vvp_fun_part_var_aa::recv_vec4_pv(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
				       بنانشان بنياد, بنانشان vwid, vvp_context_t context)
{
      إذا (context) {
	    ثابت vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
	                                                      (vvp_get_context_شي(context,
	                                                       context_idx_));

            vvp_vector4_t tmp = state->ذريعو;
            إذا (tmp.size() == 0)
                  tmp = vvp_vector4_t(vwid);

            أكد(tmp.size() == vwid);
            tmp.سيٽ_vec(بنياد, ٻٽو_انگ);
            recv_vec4(پورٽ, tmp, context);
      } وإلا {
            context = context_scope_->جيئرو_contexts;
            طالما (context) {
                  recv_vec4(پورٽ, ٻٽو_انگ, context);
                  context = vvp_get_ايندڙ_context(context);
            }
      }
}

/*
 * Given a node functor, create a network node and link it into the
 * netlist. This form assumes nodes with a single input.
 */
فراغ link_ڳنڍ_1(حرف*label, حرف*ذريعو, vvp_net_fun_t*fun)
{
      vvp_net_t*net = جديد vvp_net_t;
      net->fun = fun;

      define_functor_نشان(label, net);
      حرر(label);

      داخلا_connect(net, 0, ذريعو);
}

فراغ compile_part_select(حرف*label, حرف*ذريعو,
			 بنانشان بنياد, بنانشان wid)
{
      vvp_fun_part*fun = 0;
      إذا (vpip_peek_current_scope()->is_automatic()) {
            fun = جديد vvp_fun_part_aa(بنياد, wid);
      } وإلا {
            fun = جديد vvp_fun_part_sa(بنياد, wid);
      }
      link_ڳنڍ_1(label, ذريعو, fun);
}

فراغ compile_part_select_pv(حرف*label, حرف*ذريعو,
			    بنانشان بنياد, بنانشان wid,
			    بنانشان ويڪٽر_wid)
{
      vvp_fun_part_pv*fun = جديد vvp_fun_part_pv(بنياد, wid, ويڪٽر_wid);
      link_ڳنڍ_1(label, ذريعو, fun);
}

فراغ compile_part_select_var(حرف*label, حرف*ذريعو, حرف*var,
			     بنانشان wid, منطقي is_signed)
{
      vvp_fun_part_var*fun = 0;
      إذا (vpip_peek_current_scope()->is_automatic()) {
            fun = جديد vvp_fun_part_var_aa(wid, is_signed);
      } وإلا {
            fun = جديد vvp_fun_part_var_sa(wid, is_signed);
      }
      vvp_net_t*net = جديد vvp_net_t;
      net->fun = fun;

      define_functor_نشان(label, net);
      حرر(label);

      داخلا_connect(net, 0, ذريعو);
      داخلا_connect(net, 1, var);
}
