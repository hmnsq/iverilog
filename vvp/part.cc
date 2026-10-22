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

استفاده نطاق std;

بنية vvp_fun_part_state_s {
      vvp_fun_part_state_s() {}

      vvp_vector4_t بیتها;
};

vvp_fun_part::vvp_fun_part(بیعلامت پایه, بیعلامت wid)
: پایه_2(پایه), wid_(wid)
{
}

vvp_fun_part::~vvp_fun_part()
{
}

vvp_fun_part_sa::vvp_fun_part_sa(بیعلامت پایه, بیعلامت wid)
: vvp_fun_part(پایه, wid)
{
      net_ = 0;
}

vvp_fun_part_sa::~vvp_fun_part_sa()
{
}

فراغ vvp_fun_part_sa::recv_vec4(vvp_net_ptr_t پورت, ثابت vvp_vector4_t&بیت,
                                vvp_context_t)
{
      أكد(پورت.پورت() == 0);

      vvp_vector4_t tmp (بیت, پایه_2, wid_);
      إذا (val_ .eeq( tmp ))
	    أرجع;

      val_ = tmp;

      إذا (net_ == 0) {
	    net_ = پورت.ptr();
	    زمانبندی_functor(هذا);
      }
}

/*
 * Handle the case that the part select node is actually fed by a part
 * select assignment. It's not exactly clear what might make this
 * happen, but is does seem to happen and this should have well
 * defined behavior.
 */
فراغ vvp_fun_part_sa::recv_vec4_pv(vvp_net_ptr_t پورت, ثابت vvp_vector4_t&بیت,
				   بیعلامت پایه, بیعلامت vwid, vvp_context_t)
{
      vvp_vector4_t tmp (vwid, BIT4_Z);
      tmp.مجموعه_vec(پایه_2, val_);
      tmp.مجموعه_vec(پایه, بیت);
      recv_vec4(پورت, tmp, 0);
}

فراغ vvp_fun_part_sa::پیاپی_پیاپی()
{
      vvp_net_t*ptr = net_;
      net_ = 0;
      ptr->send_vec4(val_, 0);
}

vvp_fun_part_aa::vvp_fun_part_aa(بیعلامت پایه, بیعلامت wid)
: vvp_fun_part(پایه, wid)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_جمع_کن_قلم_مقصد_context(هذا, context_scope_);
}

vvp_fun_part_aa::~vvp_fun_part_aa()
{
}

فراغ vvp_fun_part_aa::alloc_instance(vvp_context_t context)
{
      vvp_مجموعه_context_قلم(context, context_idx_, جديد vvp_vector4_t);
}

فراغ vvp_fun_part_aa::reset_instance(vvp_context_t context)
{
      vvp_vector4_t*val = static_cast<vvp_vector4_t*>
            (vvp_get_context_قلم(context, context_idx_));

      val->مجموعه_مقصد_x();
}

#اگر_تعریف_شده CHECK_WITH_VALGRIND
فراغ vvp_fun_part_aa::free_instance(vvp_context_t context)
{
      vvp_vector4_t*val = static_cast<vvp_vector4_t*>
            (vvp_get_context_قلم(context, context_idx_));
      حذف val;
}
#پایان_اگر

فراغ vvp_fun_part_aa::recv_vec4(vvp_net_ptr_t پورت, ثابت vvp_vector4_t&بیت,
                                vvp_context_t context)
{
      إذا (context) {
            أكد(پورت.پورت() == 0);

            vvp_vector4_t*val = static_cast<vvp_vector4_t*>
                  (vvp_get_context_قلم(context, context_idx_));

            vvp_vector4_t tmp (wid_, BIT4_X);
            لكل (بیعلامت idx = 0 ;  idx < wid_ ;  idx += 1) {
                  إذا (idx + پایه_2 < بیت.size())
                        tmp.مجموعه_بیت(idx, بیت.value(پایه_2+idx));
            }
            إذا (!val->eeq( tmp )) {
                  *val = tmp;
                  پورت.ptr()->send_vec4(tmp, context);
            }
      } وإلا {
            context = context_scope_->زنده_contexts;
            طالما (context) {
                  recv_vec4(پورت, بیت, context);
                  context = vvp_get_بعدی_context(context);
            }
      }
}

/*
 * Handle the case that the part select node is actually fed by a part
 * select assignment. It's not exactly clear what might make this
 * happen, but is does seem to happen and this should have well
 * defined behavior.
 */
فراغ vvp_fun_part_aa::recv_vec4_pv(vvp_net_ptr_t پورت, ثابت vvp_vector4_t&بیت,
				   بیعلامت پایه, بیعلامت vwid, vvp_context_t context)
{
      إذا (context) {
	    ثابت vvp_vector4_t*val = static_cast<vvp_vector4_t*>
	                                         (vvp_get_context_قلم(context,
	                                                               context_idx_));

	    vvp_vector4_t tmp (vwid, BIT4_Z);
	    tmp.مجموعه_vec(پایه_2, *val);
	    tmp.مجموعه_vec(پایه, بیت);
	    recv_vec4(پورت, tmp, context);
      } وإلا {
	    context = context_scope_->زنده_contexts;
	    طالما (context) {
		  recv_vec4_pv(پورت, بیت, پایه, vwid, context);
		  context = vvp_get_بعدی_context(context);
	    }
      }
}

vvp_fun_part_pv::vvp_fun_part_pv(بیعلامت b, بیعلامت w, بیعلامت v)
: پایه_2(b), wid_(w), vwid_(v)
{
}

vvp_fun_part_pv::~vvp_fun_part_pv()
{
}

فراغ vvp_fun_part_pv::recv_vec4(vvp_net_ptr_t پورت, ثابت vvp_vector4_t&بیت,
                                vvp_context_t context)
{
      أكد(پورت.پورت() == 0);

      إذا (بیت.size() != wid_) {
	    cerr << "internal error: part_pv data mismatch. "
		 << "base_=" << پایه_2 << ", wid_=" << wid_
		 << ", vwid_=" << vwid_ << ", bit=" << بیت
		 << پایان_سطر;
      }
      أكد(بیت.size() == wid_);

      پورت.ptr()->send_vec4_pv(بیت, پایه_2, vwid_, context);
}

فراغ vvp_fun_part_pv::recv_vec4_pv(vvp_net_ptr_t پورت, ثابت vvp_vector4_t&بیت,
                                 بیعلامت پایه, بیعلامت vwid, vvp_context_t ctx)
{
      أكد(پورت.پورت() == 0);
      أكد(پایه + بیت.size() <= vwid);
      أكد(vwid == wid_);

      vvp_vector4_t tmp(wid_, BIT4_Z);
      tmp.مجموعه_vec(پایه, بیت);

      پورت.ptr()->send_vec4_pv(tmp, پایه_2, vwid_, ctx);
}

فراغ vvp_fun_part_pv::recv_vec8(vvp_net_ptr_t پورت, ثابت vvp_vector8_t&بیت)
{
      أكد(پورت.پورت() == 0);

      إذا (بیت.size() != wid_) {
	    cerr << "internal error: part_pv (strength-aware) data mismatch. "
		 << "base_=" << پایه_2 << ", wid_=" << wid_
		 << ", vwid_=" << vwid_ << ", bit=" << بیت
		 << پایان_سطر;
      }
      أكد(بیت.size() == wid_);

      پورت.ptr()->send_vec8_pv(بیت, پایه_2, vwid_);
}

vvp_fun_part_var::vvp_fun_part_var(بیعلامت w, منطقي is_signed)
: wid_(w), is_signed_(is_signed)
{
}

vvp_fun_part_var::~vvp_fun_part_var()
{
}

منطقي vvp_fun_part_var::recv_vec4_(vvp_net_ptr_t پورت, ثابت vvp_vector4_t&بیت,
                                  صحيح&پایه, vvp_vector4_t&منبع,
                                  vvp_vector4_t&ref)
{
      int32_t tmp;
      بدل (پورت.پورت()) {
	  حالة 0:
	    منبع = بیت;
	    اكسر;
	  حالة 1:
	      // INT_MIN is before the vector and is used to
	      // represent a 'bx value on the select input.
	      // It is also used to guard against 32 bit
	      // unsigned -> signed address overflow / wrap around.
	    إذا (!vector4_مقصد_مقدار(بیت, tmp, is_signed_) || (!is_signed_ && tmp < 0))
		  tmp = INT32_MIN;
	    إذا (static_cast<صحيح>(tmp) == پایه) أرجع خطأ;
	    پایه = tmp;
	    اكسر;
	  افتراضي:
	    fprintf(stderr, "Unsupported port type %u.\n", پورت.پورت());
	    أكد(0);
	    اكسر;
      }

      vvp_vector4_t res (wid_);

      لكل (بیعلامت idx = 0 ;  idx < wid_ ;  idx += 1) {
	    صحيح adr = پایه+idx;
	    إذا (adr < 0) استمر;
	    إذا ((بیعلامت)adr >= منبع.size()) اكسر;

	    res.مجموعه_بیت(idx, منبع.value((بیعلامت)adr));
      }

      إذا (! ref.eeq(res)) {
	    ref = res;
            أرجع درست;
      }
      أرجع خطأ;
}

vvp_fun_part_var_sa::vvp_fun_part_var_sa(بیعلامت w, منطقي is_signed)
: vvp_fun_part_var(w, is_signed), پایه_2(0)
{
}

vvp_fun_part_var_sa::~vvp_fun_part_var_sa()
{
}

فراغ vvp_fun_part_var_sa::recv_vec4(vvp_net_ptr_t پورت, ثابت vvp_vector4_t&بیت,
                                    vvp_context_t)
{
      إذا (recv_vec4_(پورت, بیت, پایه_2, منبع_2, ref_)) {
	    پورت.ptr()->send_vec4(ref_, 0);
      }
}

فراغ vvp_fun_part_var_sa::recv_vec4_pv(vvp_net_ptr_t پورت, ثابت vvp_vector4_t&بیت,
				       بیعلامت پایه, بیعلامت vwid, vvp_context_t)
{
      vvp_vector4_t tmp = منبع_2;
      إذا (tmp.size() == 0)
	    tmp = vvp_vector4_t(vwid);

      أكد(tmp.size() == vwid);
      tmp.مجموعه_vec(پایه, بیت);
      recv_vec4(پورت, tmp, 0);
}

بنية vvp_fun_part_var_state_s {
      vvp_fun_part_var_state_s() : پایه(0) { }

      صحيح پایه;
      vvp_vector4_t منبع;
      vvp_vector4_t ref;
};

vvp_fun_part_var_aa::vvp_fun_part_var_aa(بیعلامت w, منطقي is_signed)
: vvp_fun_part_var(w, is_signed)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_جمع_کن_قلم_مقصد_context(هذا, context_scope_);
}

vvp_fun_part_var_aa::~vvp_fun_part_var_aa()
{
}

فراغ vvp_fun_part_var_aa::alloc_instance(vvp_context_t context)
{
      vvp_مجموعه_context_قلم(context, context_idx_, جديد vvp_fun_part_var_state_s);
}

فراغ vvp_fun_part_var_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
            (vvp_get_context_قلم(context, context_idx_));

      state->پایه = 0;
      state->منبع.مجموعه_مقصد_x();
      state->ref.مجموعه_مقصد_x();
}

#اگر_تعریف_شده CHECK_WITH_VALGRIND
فراغ vvp_fun_part_var_aa::free_instance(vvp_context_t context)
{
      vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
            (vvp_get_context_قلم(context, context_idx_));
      حذف state;
}
#پایان_اگر

فراغ vvp_fun_part_var_aa::recv_vec4(vvp_net_ptr_t پورت, ثابت vvp_vector4_t&بیت,
                                    vvp_context_t context)
{
      إذا (context) {
            vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
                  (vvp_get_context_قلم(context, context_idx_));

            إذا (recv_vec4_(پورت, بیت, state->پایه, state->منبع, state->ref)) {
                  پورت.ptr()->send_vec4(state->ref, context);
            }
      } وإلا {
            context = context_scope_->زنده_contexts;
            طالما (context) {
                  recv_vec4(پورت, بیت, context);
                  context = vvp_get_بعدی_context(context);
            }
      }
}

فراغ vvp_fun_part_var_aa::recv_vec4_pv(vvp_net_ptr_t پورت, ثابت vvp_vector4_t&بیت,
				       بیعلامت پایه, بیعلامت vwid, vvp_context_t context)
{
      إذا (context) {
	    ثابت vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
	                                                      (vvp_get_context_قلم(context,
	                                                       context_idx_));

            vvp_vector4_t tmp = state->منبع;
            إذا (tmp.size() == 0)
                  tmp = vvp_vector4_t(vwid);

            أكد(tmp.size() == vwid);
            tmp.مجموعه_vec(پایه, بیت);
            recv_vec4(پورت, tmp, context);
      } وإلا {
            context = context_scope_->زنده_contexts;
            طالما (context) {
                  recv_vec4(پورت, بیت, context);
                  context = vvp_get_بعدی_context(context);
            }
      }
}

/*
 * Given a node functor, create a network node and link it into the
 * netlist. This form assumes nodes with a single input.
 */
فراغ link_گره_1(حرف*label, حرف*منبع, vvp_net_fun_t*fun)
{
      vvp_net_t*net = جديد vvp_net_t;
      net->fun = fun;

      define_functor_نماد(label, net);
      حرر(label);

      درون_داد_connect(net, 0, منبع);
}

فراغ compile_part_select(حرف*label, حرف*منبع,
			 بیعلامت پایه, بیعلامت wid)
{
      vvp_fun_part*fun = 0;
      إذا (vpip_peek_current_scope()->is_automatic()) {
            fun = جديد vvp_fun_part_aa(پایه, wid);
      } وإلا {
            fun = جديد vvp_fun_part_sa(پایه, wid);
      }
      link_گره_1(label, منبع, fun);
}

فراغ compile_part_select_pv(حرف*label, حرف*منبع,
			    بیعلامت پایه, بیعلامت wid,
			    بیعلامت وکتور_wid)
{
      vvp_fun_part_pv*fun = جديد vvp_fun_part_pv(پایه, wid, وکتور_wid);
      link_گره_1(label, منبع, fun);
}

فراغ compile_part_select_var(حرف*label, حرف*منبع, حرف*var,
			     بیعلامت wid, منطقي is_signed)
{
      vvp_fun_part_var*fun = 0;
      إذا (vpip_peek_current_scope()->is_automatic()) {
            fun = جديد vvp_fun_part_var_aa(wid, is_signed);
      } وإلا {
            fun = جديد vvp_fun_part_var_sa(wid, is_signed);
      }
      vvp_net_t*net = جديد vvp_net_t;
      net->fun = fun;

      define_functor_نماد(label, net);
      حرر(label);

      درون_داد_connect(net, 0, منبع);
      درون_داد_connect(net, 1, var);
}
