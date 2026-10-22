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

کارول نطاق std;

بنية vvp_fun_part_state_s {
      vvp_fun_part_state_s() {}

      vvp_vector4_t بټونه;
};

vvp_fun_part::vvp_fun_part(بینښه بنسټ, بینښه wid)
: بنسټ_2(بنسټ), wid_(wid)
{
}

vvp_fun_part::~vvp_fun_part()
{
}

vvp_fun_part_sa::vvp_fun_part_sa(بینښه بنسټ, بینښه wid)
: vvp_fun_part(بنسټ, wid)
{
      net_ = 0;
}

vvp_fun_part_sa::~vvp_fun_part_sa()
{
}

فراغ vvp_fun_part_sa::recv_vec4(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
                                vvp_context_t)
{
      أكد(پورټ.پورټ() == 0);

      vvp_vector4_t tmp (بټ, بنسټ_2, wid_);
      إذا (val_ .eeq( tmp ))
	    أرجع;

      val_ = tmp;

      إذا (net_ == 0) {
	    net_ = پورټ.ptr();
	    مهالوېش_functor(هذا);
      }
}

/*
 * Handle the case that the part select node is actually fed by a part
 * select assignment. It's not exactly clear what might make this
 * happen, but is does seem to happen and this should have well
 * defined behavior.
 */
فراغ vvp_fun_part_sa::recv_vec4_pv(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
				   بینښه بنسټ, بینښه vwid, vvp_context_t)
{
      vvp_vector4_t tmp (vwid, BIT4_Z);
      tmp.سېټ_vec(بنسټ_2, val_);
      tmp.سېټ_vec(بنسټ, بټ);
      recv_vec4(پورټ, tmp, 0);
}

فراغ vvp_fun_part_sa::پرلپسې_پرلپسې()
{
      vvp_net_t*ptr = net_;
      net_ = 0;
      ptr->send_vec4(val_, 0);
}

vvp_fun_part_aa::vvp_fun_part_aa(بینښه بنسټ, بینښه wid)
: vvp_fun_part(بنسټ, wid)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_جمع_کړه_توکی_منزل_context(هذا, context_scope_);
}

vvp_fun_part_aa::~vvp_fun_part_aa()
{
}

فراغ vvp_fun_part_aa::alloc_instance(vvp_context_t context)
{
      vvp_سېټ_context_توکی(context, context_idx_, جديد vvp_vector4_t);
}

فراغ vvp_fun_part_aa::reset_instance(vvp_context_t context)
{
      vvp_vector4_t*val = static_cast<vvp_vector4_t*>
            (vvp_get_context_توکی(context, context_idx_));

      val->سېټ_منزل_x();
}

#که_تعریف_شوی CHECK_WITH_VALGRIND
فراغ vvp_fun_part_aa::free_instance(vvp_context_t context)
{
      vvp_vector4_t*val = static_cast<vvp_vector4_t*>
            (vvp_get_context_توکی(context, context_idx_));
      ړنګول val;
}
#د_که_پای

فراغ vvp_fun_part_aa::recv_vec4(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
                                vvp_context_t context)
{
      إذا (context) {
            أكد(پورټ.پورټ() == 0);

            vvp_vector4_t*val = static_cast<vvp_vector4_t*>
                  (vvp_get_context_توکی(context, context_idx_));

            vvp_vector4_t tmp (wid_, BIT4_X);
            لكل (بینښه idx = 0 ;  idx < wid_ ;  idx += 1) {
                  إذا (idx + بنسټ_2 < بټ.size())
                        tmp.سېټ_بټ(idx, بټ.value(بنسټ_2+idx));
            }
            إذا (!val->eeq( tmp )) {
                  *val = tmp;
                  پورټ.ptr()->send_vec4(tmp, context);
            }
      } وإلا {
            context = context_scope_->ژوندی_contexts;
            طالما (context) {
                  recv_vec4(پورټ, بټ, context);
                  context = vvp_get_بل_context(context);
            }
      }
}

/*
 * Handle the case that the part select node is actually fed by a part
 * select assignment. It's not exactly clear what might make this
 * happen, but is does seem to happen and this should have well
 * defined behavior.
 */
فراغ vvp_fun_part_aa::recv_vec4_pv(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
				   بینښه بنسټ, بینښه vwid, vvp_context_t context)
{
      إذا (context) {
	    ثابت vvp_vector4_t*val = static_cast<vvp_vector4_t*>
	                                         (vvp_get_context_توکی(context,
	                                                               context_idx_));

	    vvp_vector4_t tmp (vwid, BIT4_Z);
	    tmp.سېټ_vec(بنسټ_2, *val);
	    tmp.سېټ_vec(بنسټ, بټ);
	    recv_vec4(پورټ, tmp, context);
      } وإلا {
	    context = context_scope_->ژوندی_contexts;
	    طالما (context) {
		  recv_vec4_pv(پورټ, بټ, بنسټ, vwid, context);
		  context = vvp_get_بل_context(context);
	    }
      }
}

vvp_fun_part_pv::vvp_fun_part_pv(بینښه b, بینښه w, بینښه v)
: بنسټ_2(b), wid_(w), vwid_(v)
{
}

vvp_fun_part_pv::~vvp_fun_part_pv()
{
}

فراغ vvp_fun_part_pv::recv_vec4(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
                                vvp_context_t context)
{
      أكد(پورټ.پورټ() == 0);

      إذا (بټ.size() != wid_) {
	    cerr << "internal error: part_pv data mismatch. "
		 << "base_=" << بنسټ_2 << ", wid_=" << wid_
		 << ", vwid_=" << vwid_ << ", bit=" << بټ
		 << کرښه_پای;
      }
      أكد(بټ.size() == wid_);

      پورټ.ptr()->send_vec4_pv(بټ, بنسټ_2, vwid_, context);
}

فراغ vvp_fun_part_pv::recv_vec4_pv(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
                                 بینښه بنسټ, بینښه vwid, vvp_context_t ctx)
{
      أكد(پورټ.پورټ() == 0);
      أكد(بنسټ + بټ.size() <= vwid);
      أكد(vwid == wid_);

      vvp_vector4_t tmp(wid_, BIT4_Z);
      tmp.سېټ_vec(بنسټ, بټ);

      پورټ.ptr()->send_vec4_pv(tmp, بنسټ_2, vwid_, ctx);
}

فراغ vvp_fun_part_pv::recv_vec8(vvp_net_ptr_t پورټ, ثابت vvp_vector8_t&بټ)
{
      أكد(پورټ.پورټ() == 0);

      إذا (بټ.size() != wid_) {
	    cerr << "internal error: part_pv (strength-aware) data mismatch. "
		 << "base_=" << بنسټ_2 << ", wid_=" << wid_
		 << ", vwid_=" << vwid_ << ", bit=" << بټ
		 << کرښه_پای;
      }
      أكد(بټ.size() == wid_);

      پورټ.ptr()->send_vec8_pv(بټ, بنسټ_2, vwid_);
}

vvp_fun_part_var::vvp_fun_part_var(بینښه w, منطقي is_signed)
: wid_(w), is_signed_(is_signed)
{
}

vvp_fun_part_var::~vvp_fun_part_var()
{
}

منطقي vvp_fun_part_var::recv_vec4_(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
                                  صحيح&بنسټ, vvp_vector4_t&سرچینه,
                                  vvp_vector4_t&ref)
{
      int32_t tmp;
      بدل (پورټ.پورټ()) {
	  حالة 0:
	    سرچینه = بټ;
	    اكسر;
	  حالة 1:
	      // INT_MIN is before the vector and is used to
	      // represent a 'bx value on the select input.
	      // It is also used to guard against 32 bit
	      // unsigned -> signed address overflow / wrap around.
	    إذا (!vector4_منزل_ارزښت(بټ, tmp, is_signed_) || (!is_signed_ && tmp < 0))
		  tmp = INT32_MIN;
	    إذا (static_cast<صحيح>(tmp) == بنسټ) أرجع خطأ;
	    بنسټ = tmp;
	    اكسر;
	  افتراضي:
	    fprintf(stderr, "Unsupported port type %u.\n", پورټ.پورټ());
	    أكد(0);
	    اكسر;
      }

      vvp_vector4_t res (wid_);

      لكل (بینښه idx = 0 ;  idx < wid_ ;  idx += 1) {
	    صحيح adr = بنسټ+idx;
	    إذا (adr < 0) استمر;
	    إذا ((بینښه)adr >= سرچینه.size()) اكسر;

	    res.سېټ_بټ(idx, سرچینه.value((بینښه)adr));
      }

      إذا (! ref.eeq(res)) {
	    ref = res;
            أرجع ریښتیا;
      }
      أرجع خطأ;
}

vvp_fun_part_var_sa::vvp_fun_part_var_sa(بینښه w, منطقي is_signed)
: vvp_fun_part_var(w, is_signed), بنسټ_2(0)
{
}

vvp_fun_part_var_sa::~vvp_fun_part_var_sa()
{
}

فراغ vvp_fun_part_var_sa::recv_vec4(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
                                    vvp_context_t)
{
      إذا (recv_vec4_(پورټ, بټ, بنسټ_2, سرچینه_2, ref_)) {
	    پورټ.ptr()->send_vec4(ref_, 0);
      }
}

فراغ vvp_fun_part_var_sa::recv_vec4_pv(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
				       بینښه بنسټ, بینښه vwid, vvp_context_t)
{
      vvp_vector4_t tmp = سرچینه_2;
      إذا (tmp.size() == 0)
	    tmp = vvp_vector4_t(vwid);

      أكد(tmp.size() == vwid);
      tmp.سېټ_vec(بنسټ, بټ);
      recv_vec4(پورټ, tmp, 0);
}

بنية vvp_fun_part_var_state_s {
      vvp_fun_part_var_state_s() : بنسټ(0) { }

      صحيح بنسټ;
      vvp_vector4_t سرچینه;
      vvp_vector4_t ref;
};

vvp_fun_part_var_aa::vvp_fun_part_var_aa(بینښه w, منطقي is_signed)
: vvp_fun_part_var(w, is_signed)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_جمع_کړه_توکی_منزل_context(هذا, context_scope_);
}

vvp_fun_part_var_aa::~vvp_fun_part_var_aa()
{
}

فراغ vvp_fun_part_var_aa::alloc_instance(vvp_context_t context)
{
      vvp_سېټ_context_توکی(context, context_idx_, جديد vvp_fun_part_var_state_s);
}

فراغ vvp_fun_part_var_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
            (vvp_get_context_توکی(context, context_idx_));

      state->بنسټ = 0;
      state->سرچینه.سېټ_منزل_x();
      state->ref.سېټ_منزل_x();
}

#که_تعریف_شوی CHECK_WITH_VALGRIND
فراغ vvp_fun_part_var_aa::free_instance(vvp_context_t context)
{
      vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
            (vvp_get_context_توکی(context, context_idx_));
      ړنګول state;
}
#د_که_پای

فراغ vvp_fun_part_var_aa::recv_vec4(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
                                    vvp_context_t context)
{
      إذا (context) {
            vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
                  (vvp_get_context_توکی(context, context_idx_));

            إذا (recv_vec4_(پورټ, بټ, state->بنسټ, state->سرچینه, state->ref)) {
                  پورټ.ptr()->send_vec4(state->ref, context);
            }
      } وإلا {
            context = context_scope_->ژوندی_contexts;
            طالما (context) {
                  recv_vec4(پورټ, بټ, context);
                  context = vvp_get_بل_context(context);
            }
      }
}

فراغ vvp_fun_part_var_aa::recv_vec4_pv(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
				       بینښه بنسټ, بینښه vwid, vvp_context_t context)
{
      إذا (context) {
	    ثابت vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
	                                                      (vvp_get_context_توکی(context,
	                                                       context_idx_));

            vvp_vector4_t tmp = state->سرچینه;
            إذا (tmp.size() == 0)
                  tmp = vvp_vector4_t(vwid);

            أكد(tmp.size() == vwid);
            tmp.سېټ_vec(بنسټ, بټ);
            recv_vec4(پورټ, tmp, context);
      } وإلا {
            context = context_scope_->ژوندی_contexts;
            طالما (context) {
                  recv_vec4(پورټ, بټ, context);
                  context = vvp_get_بل_context(context);
            }
      }
}

/*
 * Given a node functor, create a network node and link it into the
 * netlist. This form assumes nodes with a single input.
 */
فراغ link_غوټه_1(حرف*label, حرف*سرچینه, vvp_net_fun_t*fun)
{
      vvp_net_t*net = جديد vvp_net_t;
      net->fun = fun;

      define_functor_نښه(label, net);
      حرر(label);

      ننوت_connect(net, 0, سرچینه);
}

فراغ compile_part_select(حرف*label, حرف*سرچینه,
			 بینښه بنسټ, بینښه wid)
{
      vvp_fun_part*fun = 0;
      إذا (vpip_peek_current_scope()->is_automatic()) {
            fun = جديد vvp_fun_part_aa(بنسټ, wid);
      } وإلا {
            fun = جديد vvp_fun_part_sa(بنسټ, wid);
      }
      link_غوټه_1(label, سرچینه, fun);
}

فراغ compile_part_select_pv(حرف*label, حرف*سرچینه,
			    بینښه بنسټ, بینښه wid,
			    بینښه ویکټور_wid)
{
      vvp_fun_part_pv*fun = جديد vvp_fun_part_pv(بنسټ, wid, ویکټور_wid);
      link_غوټه_1(label, سرچینه, fun);
}

فراغ compile_part_select_var(حرف*label, حرف*سرچینه, حرف*var,
			     بینښه wid, منطقي is_signed)
{
      vvp_fun_part_var*fun = 0;
      إذا (vpip_peek_current_scope()->is_automatic()) {
            fun = جديد vvp_fun_part_var_aa(wid, is_signed);
      } وإلا {
            fun = جديد vvp_fun_part_var_sa(wid, is_signed);
      }
      vvp_net_t*net = جديد vvp_net_t;
      net->fun = fun;

      define_functor_نښه(label, net);
      حرر(label);

      ننوت_connect(net, 0, سرچینه);
      ننوت_connect(net, 1, var);
}
