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

using নামাকাশ std;

সংরচনা vvp_fun_part_state_s {
      vvp_fun_part_state_s() {}

      vvp_vector4_t bits;
};

vvp_fun_part::vvp_fun_part(unsigned base, unsigned wid)
: base_(base), wid_(wid)
{
}

vvp_fun_part::~vvp_fun_part()
{
}

vvp_fun_part_sa::vvp_fun_part_sa(unsigned base, unsigned wid)
: vvp_fun_part(base, wid)
{
      net_ = 0;
}

vvp_fun_part_sa::~vvp_fun_part_sa()
{
}

শূন্য vvp_fun_part_sa::recv_vec4(vvp_net_ptr_t port, ধ্রুৱ vvp_vector4_t&bit,
                                vvp_context_t)
{
      প্রতিজ্ঞা(port.port() == 0);

      vvp_vector4_t tmp (bit, base_, wid_);
      যদি (val_ .eeq( tmp ))
	    ঘূৰাই;

      val_ = tmp;

      যদি (net_ == 0) {
	    net_ = port.ptr();
	    schedule_functor(এতত্);
      }
}

/*
 * Handle the case that the part select node is actually fed by a part
 * select assignment. It's not exactly clear what might make this
 * happen, but is does seem to happen and this should have well
 * defined behavior.
 */
শূন্য vvp_fun_part_sa::recv_vec4_pv(vvp_net_ptr_t port, ধ্রুৱ vvp_vector4_t&bit,
				   unsigned base, unsigned vwid, vvp_context_t)
{
      vvp_vector4_t tmp (vwid, BIT4_Z);
      tmp.set_vec(base_, val_);
      tmp.set_vec(base, bit);
      recv_vec4(port, tmp, 0);
}

শূন্য vvp_fun_part_sa::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;
      ptr->send_vec4(val_, 0);
}

vvp_fun_part_aa::vvp_fun_part_aa(unsigned base, unsigned wid)
: vvp_fun_part(base, wid)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_add_item_to_context(এতত্, context_scope_);
}

vvp_fun_part_aa::~vvp_fun_part_aa()
{
}

শূন্য vvp_fun_part_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, নূতন vvp_vector4_t);
}

শূন্য vvp_fun_part_aa::reset_instance(vvp_context_t context)
{
      vvp_vector4_t*val = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));

      val->set_to_x();
}

#ifdef CHECK_WITH_VALGRIND
শূন্য vvp_fun_part_aa::free_instance(vvp_context_t context)
{
      vvp_vector4_t*val = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));
      delete val;
}
#endif

শূন্য vvp_fun_part_aa::recv_vec4(vvp_net_ptr_t port, ধ্রুৱ vvp_vector4_t&bit,
                                vvp_context_t context)
{
      যদি (context) {
            প্রতিজ্ঞা(port.port() == 0);

            vvp_vector4_t*val = static_cast<vvp_vector4_t*>
                  (vvp_get_context_item(context, context_idx_));

            vvp_vector4_t tmp (wid_, BIT4_X);
            বাবে (unsigned idx = 0 ;  idx < wid_ ;  idx += 1) {
                  যদি (idx + base_ < bit.size())
                        tmp.set_bit(idx, bit.value(base_+idx));
            }
            যদি (!val->eeq( tmp )) {
                  *val = tmp;
                  port.ptr()->send_vec4(tmp, context);
            }
      } অন্যথা {
            context = context_scope_->live_contexts;
            যেতিয়ালৈ (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
      }
}

/*
 * Handle the case that the part select node is actually fed by a part
 * select assignment. It's not exactly clear what might make this
 * happen, but is does seem to happen and this should have well
 * defined behavior.
 */
শূন্য vvp_fun_part_aa::recv_vec4_pv(vvp_net_ptr_t port, ধ্রুৱ vvp_vector4_t&bit,
				   unsigned base, unsigned vwid, vvp_context_t context)
{
      যদি (context) {
	    ধ্রুৱ vvp_vector4_t*val = static_cast<vvp_vector4_t*>
	                                         (vvp_get_context_item(context,
	                                                               context_idx_));

	    vvp_vector4_t tmp (vwid, BIT4_Z);
	    tmp.set_vec(base_, *val);
	    tmp.set_vec(base, bit);
	    recv_vec4(port, tmp, context);
      } অন্যথা {
	    context = context_scope_->live_contexts;
	    যেতিয়ালৈ (context) {
		  recv_vec4_pv(port, bit, base, vwid, context);
		  context = vvp_get_next_context(context);
	    }
      }
}

vvp_fun_part_pv::vvp_fun_part_pv(unsigned b, unsigned w, unsigned v)
: base_(b), wid_(w), vwid_(v)
{
}

vvp_fun_part_pv::~vvp_fun_part_pv()
{
}

শূন্য vvp_fun_part_pv::recv_vec4(vvp_net_ptr_t port, ধ্রুৱ vvp_vector4_t&bit,
                                vvp_context_t context)
{
      প্রতিজ্ঞা(port.port() == 0);

      যদি (bit.size() != wid_) {
	    cerr << "internal error: part_pv data mismatch. "
		 << "base_=" << base_ << ", wid_=" << wid_
		 << ", vwid_=" << vwid_ << ", bit=" << bit
		 << endl;
      }
      প্রতিজ্ঞা(bit.size() == wid_);

      port.ptr()->send_vec4_pv(bit, base_, vwid_, context);
}

শূন্য vvp_fun_part_pv::recv_vec4_pv(vvp_net_ptr_t port, ধ্রুৱ vvp_vector4_t&bit,
                                 unsigned base, unsigned vwid, vvp_context_t ctx)
{
      প্রতিজ্ঞা(port.port() == 0);
      প্রতিজ্ঞা(base + bit.size() <= vwid);
      প্রতিজ্ঞা(vwid == wid_);

      vvp_vector4_t tmp(wid_, BIT4_Z);
      tmp.set_vec(base, bit);

      port.ptr()->send_vec4_pv(tmp, base_, vwid_, ctx);
}

শূন্য vvp_fun_part_pv::recv_vec8(vvp_net_ptr_t port, ধ্রুৱ vvp_vector8_t&bit)
{
      প্রতিজ্ঞা(port.port() == 0);

      যদি (bit.size() != wid_) {
	    cerr << "internal error: part_pv (strength-aware) data mismatch. "
		 << "base_=" << base_ << ", wid_=" << wid_
		 << ", vwid_=" << vwid_ << ", bit=" << bit
		 << endl;
      }
      প্রতিজ্ঞা(bit.size() == wid_);

      port.ptr()->send_vec8_pv(bit, base_, vwid_);
}

vvp_fun_part_var::vvp_fun_part_var(unsigned w, তার্কিক is_signed)
: wid_(w), is_signed_(is_signed)
{
}

vvp_fun_part_var::~vvp_fun_part_var()
{
}

তার্কিক vvp_fun_part_var::recv_vec4_(vvp_net_ptr_t port, ধ্রুৱ vvp_vector4_t&bit,
                                  পূর্ণাঙ্ক&base, vvp_vector4_t&source,
                                  vvp_vector4_t&ref)
{
      int32_t tmp;
      ৱিকল্প (port.port()) {
	  পক্ষ 0:
	    source = bit;
	    ভঙ্গ;
	  পক্ষ 1:
	      // INT_MIN is before the vector and is used to
	      // represent a 'bx value on the select input.
	      // It is also used to guard against 32 bit
	      // unsigned -> signed address overflow / wrap around.
	    যদি (!vector4_to_মান(bit, tmp, is_signed_) || (!is_signed_ && tmp < 0))
		  tmp = INT32_MIN;
	    যদি (static_cast<পূর্ণাঙ্ক>(tmp) == base) ঘূৰাই মিছা;
	    base = tmp;
	    ভঙ্গ;
	  স্ৱভাৱ:
	    fprintf(stderr, "Unsupported port type %u.\n", port.port());
	    প্রতিজ্ঞা(0);
	    ভঙ্গ;
      }

      vvp_vector4_t res (wid_);

      বাবে (unsigned idx = 0 ;  idx < wid_ ;  idx += 1) {
	    পূর্ণাঙ্ক adr = base+idx;
	    যদি (adr < 0) অনুৱর্ত;
	    যদি ((unsigned)adr >= source.size()) ভঙ্গ;

	    res.set_bit(idx, source.value((unsigned)adr));
      }

      যদি (! ref.eeq(res)) {
	    ref = res;
            ঘূৰাই সঁচা;
      }
      ঘূৰাই মিছা;
}

vvp_fun_part_var_sa::vvp_fun_part_var_sa(unsigned w, তার্কিক is_signed)
: vvp_fun_part_var(w, is_signed), base_(0)
{
}

vvp_fun_part_var_sa::~vvp_fun_part_var_sa()
{
}

শূন্য vvp_fun_part_var_sa::recv_vec4(vvp_net_ptr_t port, ধ্রুৱ vvp_vector4_t&bit,
                                    vvp_context_t)
{
      যদি (recv_vec4_(port, bit, base_, source_, ref_)) {
	    port.ptr()->send_vec4(ref_, 0);
      }
}

শূন্য vvp_fun_part_var_sa::recv_vec4_pv(vvp_net_ptr_t port, ধ্রুৱ vvp_vector4_t&bit,
				       unsigned base, unsigned vwid, vvp_context_t)
{
      vvp_vector4_t tmp = source_;
      যদি (tmp.size() == 0)
	    tmp = vvp_vector4_t(vwid);

      প্রতিজ্ঞা(tmp.size() == vwid);
      tmp.set_vec(base, bit);
      recv_vec4(port, tmp, 0);
}

সংরচনা vvp_fun_part_var_state_s {
      vvp_fun_part_var_state_s() : base(0) { }

      পূর্ণাঙ্ক base;
      vvp_vector4_t source;
      vvp_vector4_t ref;
};

vvp_fun_part_var_aa::vvp_fun_part_var_aa(unsigned w, তার্কিক is_signed)
: vvp_fun_part_var(w, is_signed)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_add_item_to_context(এতত্, context_scope_);
}

vvp_fun_part_var_aa::~vvp_fun_part_var_aa()
{
}

শূন্য vvp_fun_part_var_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, নূতন vvp_fun_part_var_state_s);
}

শূন্য vvp_fun_part_var_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->base = 0;
      state->source.set_to_x();
      state->ref.set_to_x();
}

#ifdef CHECK_WITH_VALGRIND
শূন্য vvp_fun_part_var_aa::free_instance(vvp_context_t context)
{
      vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
            (vvp_get_context_item(context, context_idx_));
      delete state;
}
#endif

শূন্য vvp_fun_part_var_aa::recv_vec4(vvp_net_ptr_t port, ধ্রুৱ vvp_vector4_t&bit,
                                    vvp_context_t context)
{
      যদি (context) {
            vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            যদি (recv_vec4_(port, bit, state->base, state->source, state->ref)) {
                  port.ptr()->send_vec4(state->ref, context);
            }
      } অন্যথা {
            context = context_scope_->live_contexts;
            যেতিয়ালৈ (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
      }
}

শূন্য vvp_fun_part_var_aa::recv_vec4_pv(vvp_net_ptr_t port, ধ্রুৱ vvp_vector4_t&bit,
				       unsigned base, unsigned vwid, vvp_context_t context)
{
      যদি (context) {
	    ধ্রুৱ vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
	                                                      (vvp_get_context_item(context,
	                                                       context_idx_));

            vvp_vector4_t tmp = state->source;
            যদি (tmp.size() == 0)
                  tmp = vvp_vector4_t(vwid);

            প্রতিজ্ঞা(tmp.size() == vwid);
            tmp.set_vec(base, bit);
            recv_vec4(port, tmp, context);
      } অন্যথা {
            context = context_scope_->live_contexts;
            যেতিয়ালৈ (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
      }
}

/*
 * Given a node functor, create a network node and link it into the
 * netlist. This form assumes nodes with a single input.
 */
শূন্য link_গ্ৰন্থি_1(ৱর্ণ*label, ৱর্ণ*source, vvp_net_fun_t*fun)
{
      vvp_net_t*net = নূতন vvp_net_t;
      net->fun = fun;

      define_functor_প্ৰতীক(label, net);
      মুক্ত(label);

      প্ৰবেশ_connect(net, 0, source);
}

শূন্য compile_part_select(ৱর্ণ*label, ৱর্ণ*source,
			 unsigned base, unsigned wid)
{
      vvp_fun_part*fun = 0;
      যদি (vpip_peek_current_scope()->is_automatic()) {
            fun = নূতন vvp_fun_part_aa(base, wid);
      } অন্যথা {
            fun = নূতন vvp_fun_part_sa(base, wid);
      }
      link_গ্ৰন্থি_1(label, source, fun);
}

শূন্য compile_part_select_pv(ৱর্ণ*label, ৱর্ণ*source,
			    unsigned base, unsigned wid,
			    unsigned vector_wid)
{
      vvp_fun_part_pv*fun = নূতন vvp_fun_part_pv(base, wid, vector_wid);
      link_গ্ৰন্থি_1(label, source, fun);
}

শূন্য compile_part_select_var(ৱর্ণ*label, ৱর্ণ*source, ৱর্ণ*var,
			     unsigned wid, তার্কিক is_signed)
{
      vvp_fun_part_var*fun = 0;
      যদি (vpip_peek_current_scope()->is_automatic()) {
            fun = নূতন vvp_fun_part_var_aa(wid, is_signed);
      } অন্যথা {
            fun = নূতন vvp_fun_part_var_sa(wid, is_signed);
      }
      vvp_net_t*net = নূতন vvp_net_t;
      net->fun = fun;

      define_functor_প্ৰতীক(label, net);
      মুক্ত(label);

      প্ৰবেশ_connect(net, 0, source);
      প্ৰবেশ_connect(net, 1, var);
}
