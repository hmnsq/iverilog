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

using ನಾಮಾಕಾಶ std;

ಸಂರಚನಾ vvp_fun_part_state_s {
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

ಶೂನ್ಯ vvp_fun_part_sa::recv_vec4(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
                                vvp_context_t)
{
      ಪ್ರತಿಜ್ಞಾ(port.port() == 0);

      vvp_vector4_t tmp (bit, base_, wid_);
      ಯದಿ (val_ .eeq( tmp ))
	    ಪ್ರತ್ಯಾಗಮ;

      val_ = tmp;

      ಯದಿ (net_ == 0) {
	    net_ = port.ptr();
	    schedule_functor(ಏತತ್);
      }
}

/*
 * Handle the case that the part select node is actually fed by a part
 * select assignment. It's not exactly clear what might make this
 * happen, but is does seem to happen and this should have well
 * defined behavior.
 */
ಶೂನ್ಯ vvp_fun_part_sa::recv_vec4_pv(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
				   unsigned base, unsigned vwid, vvp_context_t)
{
      vvp_vector4_t tmp (vwid, BIT4_Z);
      tmp.set_vec(base_, val_);
      tmp.set_vec(base, bit);
      recv_vec4(port, tmp, 0);
}

ಶೂನ್ಯ vvp_fun_part_sa::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;
      ptr->send_vec4(val_, 0);
}

vvp_fun_part_aa::vvp_fun_part_aa(unsigned base, unsigned wid)
: vvp_fun_part(base, wid)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_add_item_to_context(ಏತತ್, context_scope_);
}

vvp_fun_part_aa::~vvp_fun_part_aa()
{
}

ಶೂನ್ಯ vvp_fun_part_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, ನೂತನ vvp_vector4_t);
}

ಶೂನ್ಯ vvp_fun_part_aa::reset_instance(vvp_context_t context)
{
      vvp_vector4_t*val = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));

      val->set_to_x();
}

#ifdef CHECK_WITH_VALGRIND
ಶೂನ್ಯ vvp_fun_part_aa::free_instance(vvp_context_t context)
{
      vvp_vector4_t*val = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));
      delete val;
}
#endif

ಶೂನ್ಯ vvp_fun_part_aa::recv_vec4(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
                                vvp_context_t context)
{
      ಯದಿ (context) {
            ಪ್ರತಿಜ್ಞಾ(port.port() == 0);

            vvp_vector4_t*val = static_cast<vvp_vector4_t*>
                  (vvp_get_context_item(context, context_idx_));

            vvp_vector4_t tmp (wid_, BIT4_X);
            ಪ್ರತಿ (unsigned idx = 0 ;  idx < wid_ ;  idx += 1) {
                  ಯದಿ (idx + base_ < bit.size())
                        tmp.set_bit(idx, bit.value(base_+idx));
            }
            ಯದಿ (!val->eeq( tmp )) {
                  *val = tmp;
                  port.ptr()->send_vec4(tmp, context);
            }
      } ಅನ್ಯಥಾ {
            context = context_scope_->live_contexts;
            ಯಾವತ್ (context) {
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
ಶೂನ್ಯ vvp_fun_part_aa::recv_vec4_pv(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
				   unsigned base, unsigned vwid, vvp_context_t context)
{
      ಯದಿ (context) {
	    ಧ್ರುವ vvp_vector4_t*val = static_cast<vvp_vector4_t*>
	                                         (vvp_get_context_item(context,
	                                                               context_idx_));

	    vvp_vector4_t tmp (vwid, BIT4_Z);
	    tmp.set_vec(base_, *val);
	    tmp.set_vec(base, bit);
	    recv_vec4(port, tmp, context);
      } ಅನ್ಯಥಾ {
	    context = context_scope_->live_contexts;
	    ಯಾವತ್ (context) {
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

ಶೂನ್ಯ vvp_fun_part_pv::recv_vec4(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
                                vvp_context_t context)
{
      ಪ್ರತಿಜ್ಞಾ(port.port() == 0);

      ಯದಿ (bit.size() != wid_) {
	    cerr << "internal error: part_pv data mismatch. "
		 << "base_=" << base_ << ", wid_=" << wid_
		 << ", vwid_=" << vwid_ << ", bit=" << bit
		 << endl;
      }
      ಪ್ರತಿಜ್ಞಾ(bit.size() == wid_);

      port.ptr()->send_vec4_pv(bit, base_, vwid_, context);
}

ಶೂನ್ಯ vvp_fun_part_pv::recv_vec4_pv(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
                                 unsigned base, unsigned vwid, vvp_context_t ctx)
{
      ಪ್ರತಿಜ್ಞಾ(port.port() == 0);
      ಪ್ರತಿಜ್ಞಾ(base + bit.size() <= vwid);
      ಪ್ರತಿಜ್ಞಾ(vwid == wid_);

      vvp_vector4_t tmp(wid_, BIT4_Z);
      tmp.set_vec(base, bit);

      port.ptr()->send_vec4_pv(tmp, base_, vwid_, ctx);
}

ಶೂನ್ಯ vvp_fun_part_pv::recv_vec8(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector8_t&bit)
{
      ಪ್ರತಿಜ್ಞಾ(port.port() == 0);

      ಯದಿ (bit.size() != wid_) {
	    cerr << "internal error: part_pv (strength-aware) data mismatch. "
		 << "base_=" << base_ << ", wid_=" << wid_
		 << ", vwid_=" << vwid_ << ", bit=" << bit
		 << endl;
      }
      ಪ್ರತಿಜ್ಞಾ(bit.size() == wid_);

      port.ptr()->send_vec8_pv(bit, base_, vwid_);
}

vvp_fun_part_var::vvp_fun_part_var(unsigned w, ತಾರ್ಕಿಕ is_signed)
: wid_(w), is_signed_(is_signed)
{
}

vvp_fun_part_var::~vvp_fun_part_var()
{
}

ತಾರ್ಕಿಕ vvp_fun_part_var::recv_vec4_(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
                                  ಪೂರ್ಣಾಙ್ಕ&base, vvp_vector4_t&source,
                                  vvp_vector4_t&ref)
{
      int32_t tmp;
      ವಿಕಲ್ಪ (port.port()) {
	  ಪಕ್ಷ 0:
	    source = bit;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ 1:
	      // INT_MIN is before the vector and is used to
	      // represent a 'bx value on the select input.
	      // It is also used to guard against 32 bit
	      // unsigned -> signed address overflow / wrap around.
	    ಯದಿ (!vector4_to_ಮೌಲ್ಯ(bit, tmp, is_signed_) || (!is_signed_ && tmp < 0))
		  tmp = INT32_MIN;
	    ಯದಿ (static_cast<ಪೂರ್ಣಾಙ್ಕ>(tmp) == base) ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	    base = tmp;
	    ಭಙ್ಗ;
	  ಸ್ವಭಾವ:
	    fprintf(stderr, "Unsupported port type %u.\n", port.port());
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಭಙ್ಗ;
      }

      vvp_vector4_t res (wid_);

      ಪ್ರತಿ (unsigned idx = 0 ;  idx < wid_ ;  idx += 1) {
	    ಪೂರ್ಣಾಙ್ಕ adr = base+idx;
	    ಯದಿ (adr < 0) ಅನುವರ್ತ;
	    ಯದಿ ((unsigned)adr >= source.size()) ಭಙ್ಗ;

	    res.set_bit(idx, source.value((unsigned)adr));
      }

      ಯದಿ (! ref.eeq(res)) {
	    ref = res;
            ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
      }
      ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

vvp_fun_part_var_sa::vvp_fun_part_var_sa(unsigned w, ತಾರ್ಕಿಕ is_signed)
: vvp_fun_part_var(w, is_signed), base_(0)
{
}

vvp_fun_part_var_sa::~vvp_fun_part_var_sa()
{
}

ಶೂನ್ಯ vvp_fun_part_var_sa::recv_vec4(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
                                    vvp_context_t)
{
      ಯದಿ (recv_vec4_(port, bit, base_, source_, ref_)) {
	    port.ptr()->send_vec4(ref_, 0);
      }
}

ಶೂನ್ಯ vvp_fun_part_var_sa::recv_vec4_pv(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
				       unsigned base, unsigned vwid, vvp_context_t)
{
      vvp_vector4_t tmp = source_;
      ಯದಿ (tmp.size() == 0)
	    tmp = vvp_vector4_t(vwid);

      ಪ್ರತಿಜ್ಞಾ(tmp.size() == vwid);
      tmp.set_vec(base, bit);
      recv_vec4(port, tmp, 0);
}

ಸಂರಚನಾ vvp_fun_part_var_state_s {
      vvp_fun_part_var_state_s() : base(0) { }

      ಪೂರ್ಣಾಙ್ಕ base;
      vvp_vector4_t source;
      vvp_vector4_t ref;
};

vvp_fun_part_var_aa::vvp_fun_part_var_aa(unsigned w, ತಾರ್ಕಿಕ is_signed)
: vvp_fun_part_var(w, is_signed)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_add_item_to_context(ಏತತ್, context_scope_);
}

vvp_fun_part_var_aa::~vvp_fun_part_var_aa()
{
}

ಶೂನ್ಯ vvp_fun_part_var_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, ನೂತನ vvp_fun_part_var_state_s);
}

ಶೂನ್ಯ vvp_fun_part_var_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->base = 0;
      state->source.set_to_x();
      state->ref.set_to_x();
}

#ifdef CHECK_WITH_VALGRIND
ಶೂನ್ಯ vvp_fun_part_var_aa::free_instance(vvp_context_t context)
{
      vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
            (vvp_get_context_item(context, context_idx_));
      delete state;
}
#endif

ಶೂನ್ಯ vvp_fun_part_var_aa::recv_vec4(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
                                    vvp_context_t context)
{
      ಯದಿ (context) {
            vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            ಯದಿ (recv_vec4_(port, bit, state->base, state->source, state->ref)) {
                  port.ptr()->send_vec4(state->ref, context);
            }
      } ಅನ್ಯಥಾ {
            context = context_scope_->live_contexts;
            ಯಾವತ್ (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
      }
}

ಶೂನ್ಯ vvp_fun_part_var_aa::recv_vec4_pv(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
				       unsigned base, unsigned vwid, vvp_context_t context)
{
      ಯದಿ (context) {
	    ಧ್ರುವ vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
	                                                      (vvp_get_context_item(context,
	                                                       context_idx_));

            vvp_vector4_t tmp = state->source;
            ಯದಿ (tmp.size() == 0)
                  tmp = vvp_vector4_t(vwid);

            ಪ್ರತಿಜ್ಞಾ(tmp.size() == vwid);
            tmp.set_vec(base, bit);
            recv_vec4(port, tmp, context);
      } ಅನ್ಯಥಾ {
            context = context_scope_->live_contexts;
            ಯಾವತ್ (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
      }
}

/*
 * Given a node functor, create a network node and link it into the
 * netlist. This form assumes nodes with a single input.
 */
ಶೂನ್ಯ link_ಗಂಟು_1(ವರ್ಣ*label, ವರ್ಣ*source, vvp_net_fun_t*fun)
{
      vvp_net_t*net = ನೂತನ vvp_net_t;
      net->fun = fun;

      define_functor_ಚಿಹ್ನೆ(label, net);
      ಮುಕ್ತ(label);

      ಒಳಹರಿವು_connect(net, 0, source);
}

ಶೂನ್ಯ compile_part_select(ವರ್ಣ*label, ವರ್ಣ*source,
			 unsigned base, unsigned wid)
{
      vvp_fun_part*fun = 0;
      ಯದಿ (vpip_peek_current_scope()->is_automatic()) {
            fun = ನೂತನ vvp_fun_part_aa(base, wid);
      } ಅನ್ಯಥಾ {
            fun = ನೂತನ vvp_fun_part_sa(base, wid);
      }
      link_ಗಂಟು_1(label, source, fun);
}

ಶೂನ್ಯ compile_part_select_pv(ವರ್ಣ*label, ವರ್ಣ*source,
			    unsigned base, unsigned wid,
			    unsigned vector_wid)
{
      vvp_fun_part_pv*fun = ನೂತನ vvp_fun_part_pv(base, wid, vector_wid);
      link_ಗಂಟು_1(label, source, fun);
}

ಶೂನ್ಯ compile_part_select_var(ವರ್ಣ*label, ವರ್ಣ*source, ವರ್ಣ*var,
			     unsigned wid, ತಾರ್ಕಿಕ is_signed)
{
      vvp_fun_part_var*fun = 0;
      ಯದಿ (vpip_peek_current_scope()->is_automatic()) {
            fun = ನೂತನ vvp_fun_part_var_aa(wid, is_signed);
      } ಅನ್ಯಥಾ {
            fun = ನೂತನ vvp_fun_part_var_sa(wid, is_signed);
      }
      vvp_net_t*net = ನೂತನ vvp_net_t;
      net->fun = fun;

      define_functor_ಚಿಹ್ನೆ(label, net);
      ಮುಕ್ತ(label);

      ಒಳಹರಿವು_connect(net, 0, source);
      ಒಳಹರಿವು_connect(net, 1, var);
}
