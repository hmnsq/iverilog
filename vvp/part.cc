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

ಬಳಸಿ ನಾಮಾಕಾಶ std;

ರಚನೆ vvp_fun_part_state_s {
      vvp_fun_part_state_s() {}

      vvp_vector4_t ಬಿಟ್ಟುಗಳು;
};

vvp_fun_part::vvp_fun_part(ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ wid)
: ತಳ_2(ತಳ), wid_(wid)
{
}

vvp_fun_part::~vvp_fun_part()
{
}

vvp_fun_part_sa::vvp_fun_part_sa(ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ wid)
: vvp_fun_part(ತಳ, wid)
{
      net_ = 0;
}

vvp_fun_part_sa::~vvp_fun_part_sa()
{
}

ಶೂನ್ಯ vvp_fun_part_sa::recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                                vvp_context_t)
{
      ಪ್ರತಿಜ್ಞಾ(ಪೋರ್ಟ್.ಪೋರ್ಟ್() == 0);

      vvp_vector4_t tmp (ಬಿಟ್, ತಳ_2, wid_);
      ಆದರೆ (val_ .eeq( tmp ))
	    ಹಿಂತಿರುಗಿ;

      val_ = tmp;

      ಆದರೆ (net_ == 0) {
	    net_ = ಪೋರ್ಟ್.ptr();
	    ವೇಳಾಪಟ್ಟಿ_functor(ಇದು);
      }
}

/*
 * Handle the case that the part select node is actually fed by a part
 * select assignment. It's not exactly clear what might make this
 * happen, but is does seem to happen and this should have well
 * defined behavior.
 */
ಶೂನ್ಯ vvp_fun_part_sa::recv_vec4_pv(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
				   ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t)
{
      vvp_vector4_t tmp (vwid, BIT4_Z);
      tmp.ಗಣ_vec(ತಳ_2, val_);
      tmp.ಗಣ_vec(ತಳ, ಬಿಟ್);
      recv_vec4(ಪೋರ್ಟ್, tmp, 0);
}

ಶೂನ್ಯ vvp_fun_part_sa::ಸತತ_ಸತತ()
{
      vvp_net_t*ptr = net_;
      net_ = 0;
      ptr->send_vec4(val_, 0);
}

vvp_fun_part_aa::vvp_fun_part_aa(ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ wid)
: vvp_fun_part(ತಳ, wid)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_ಕೂಡಿಸು_ಅಂಶ_ಗಮ್ಯ_context(ಇದು, context_scope_);
}

vvp_fun_part_aa::~vvp_fun_part_aa()
{
}

ಶೂನ್ಯ vvp_fun_part_aa::alloc_instance(vvp_context_t context)
{
      vvp_ಗಣ_context_ಅಂಶ(context, context_idx_, ಹೊಸ vvp_vector4_t);
}

ಶೂನ್ಯ vvp_fun_part_aa::reset_instance(vvp_context_t context)
{
      vvp_vector4_t*val = static_cast<vvp_vector4_t*>
            (vvp_get_context_ಅಂಶ(context, context_idx_));

      val->ಗಣ_ಗಮ್ಯ_x();
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಶೂನ್ಯ vvp_fun_part_aa::free_instance(vvp_context_t context)
{
      vvp_vector4_t*val = static_cast<vvp_vector4_t*>
            (vvp_get_context_ಅಂಶ(context, context_idx_));
      ಅಳಿಸು val;
}
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ vvp_fun_part_aa::recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                                vvp_context_t context)
{
      ಆದರೆ (context) {
            ಪ್ರತಿಜ್ಞಾ(ಪೋರ್ಟ್.ಪೋರ್ಟ್() == 0);

            vvp_vector4_t*val = static_cast<vvp_vector4_t*>
                  (vvp_get_context_ಅಂಶ(context, context_idx_));

            vvp_vector4_t tmp (wid_, BIT4_X);
            ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < wid_ ;  idx += 1) {
                  ಆದರೆ (idx + ತಳ_2 < ಬಿಟ್.size())
                        tmp.ಗಣ_ಬಿಟ್(idx, ಬಿಟ್.value(ತಳ_2+idx));
            }
            ಆದರೆ (!val->eeq( tmp )) {
                  *val = tmp;
                  ಪೋರ್ಟ್.ptr()->send_vec4(tmp, context);
            }
      } ಇಲ್ಲದಿದ್ದರೆ {
            context = context_scope_->ಜೀವಂತ_contexts;
            ಆಗ (context) {
                  recv_vec4(ಪೋರ್ಟ್, ಬಿಟ್, context);
                  context = vvp_get_ಮುಂದಿನ_context(context);
            }
      }
}

/*
 * Handle the case that the part select node is actually fed by a part
 * select assignment. It's not exactly clear what might make this
 * happen, but is does seem to happen and this should have well
 * defined behavior.
 */
ಶೂನ್ಯ vvp_fun_part_aa::recv_vec4_pv(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
				   ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t context)
{
      ಆದರೆ (context) {
	    ಸ್ಥಿರಾಂಕ vvp_vector4_t*val = static_cast<vvp_vector4_t*>
	                                         (vvp_get_context_ಅಂಶ(context,
	                                                               context_idx_));

	    vvp_vector4_t tmp (vwid, BIT4_Z);
	    tmp.ಗಣ_vec(ತಳ_2, *val);
	    tmp.ಗಣ_vec(ತಳ, ಬಿಟ್);
	    recv_vec4(ಪೋರ್ಟ್, tmp, context);
      } ಇಲ್ಲದಿದ್ದರೆ {
	    context = context_scope_->ಜೀವಂತ_contexts;
	    ಆಗ (context) {
		  recv_vec4_pv(ಪೋರ್ಟ್, ಬಿಟ್, ತಳ, vwid, context);
		  context = vvp_get_ಮುಂದಿನ_context(context);
	    }
      }
}

vvp_fun_part_pv::vvp_fun_part_pv(ಚಿಹ್ನೆರಹಿತ b, ಚಿಹ್ನೆರಹಿತ w, ಚಿಹ್ನೆರಹಿತ v)
: ತಳ_2(b), wid_(w), vwid_(v)
{
}

vvp_fun_part_pv::~vvp_fun_part_pv()
{
}

ಶೂನ್ಯ vvp_fun_part_pv::recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                                vvp_context_t context)
{
      ಪ್ರತಿಜ್ಞಾ(ಪೋರ್ಟ್.ಪೋರ್ಟ್() == 0);

      ಆದರೆ (ಬಿಟ್.size() != wid_) {
	    cerr << "internal error: part_pv data mismatch. "
		 << "base_=" << ತಳ_2 << ", wid_=" << wid_
		 << ", vwid_=" << vwid_ << ", bit=" << ಬಿಟ್
		 << ಸಾಲು_ಅಂತ್ಯ;
      }
      ಪ್ರತಿಜ್ಞಾ(ಬಿಟ್.size() == wid_);

      ಪೋರ್ಟ್.ptr()->send_vec4_pv(ಬಿಟ್, ತಳ_2, vwid_, context);
}

ಶೂನ್ಯ vvp_fun_part_pv::recv_vec4_pv(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                                 ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t ctx)
{
      ಪ್ರತಿಜ್ಞಾ(ಪೋರ್ಟ್.ಪೋರ್ಟ್() == 0);
      ಪ್ರತಿಜ್ಞಾ(ತಳ + ಬಿಟ್.size() <= vwid);
      ಪ್ರತಿಜ್ಞಾ(vwid == wid_);

      vvp_vector4_t tmp(wid_, BIT4_Z);
      tmp.ಗಣ_vec(ತಳ, ಬಿಟ್);

      ಪೋರ್ಟ್.ptr()->send_vec4_pv(tmp, ತಳ_2, vwid_, ctx);
}

ಶೂನ್ಯ vvp_fun_part_pv::recv_vec8(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector8_t&ಬಿಟ್)
{
      ಪ್ರತಿಜ್ಞಾ(ಪೋರ್ಟ್.ಪೋರ್ಟ್() == 0);

      ಆದರೆ (ಬಿಟ್.size() != wid_) {
	    cerr << "internal error: part_pv (strength-aware) data mismatch. "
		 << "base_=" << ತಳ_2 << ", wid_=" << wid_
		 << ", vwid_=" << vwid_ << ", bit=" << ಬಿಟ್
		 << ಸಾಲು_ಅಂತ್ಯ;
      }
      ಪ್ರತಿಜ್ಞಾ(ಬಿಟ್.size() == wid_);

      ಪೋರ್ಟ್.ptr()->send_vec8_pv(ಬಿಟ್, ತಳ_2, vwid_);
}

vvp_fun_part_var::vvp_fun_part_var(ಚಿಹ್ನೆರಹಿತ w, ತಾರ್ಕಿಕ is_signed)
: wid_(w), is_signed_(is_signed)
{
}

vvp_fun_part_var::~vvp_fun_part_var()
{
}

ತಾರ್ಕಿಕ vvp_fun_part_var::recv_vec4_(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                                  ಪೂರ್ಣಾಂಕ&ತಳ, vvp_vector4_t&ಮೂಲ,
                                  vvp_vector4_t&ref)
{
      int32_t tmp;
      ಆಯ್ಕೆ (ಪೋರ್ಟ್.ಪೋರ್ಟ್()) {
	  ಸ್ಥಿತಿ 0:
	    ಮೂಲ = ಬಿಟ್;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 1:
	      // INT_MIN is before the vector and is used to
	      // represent a 'bx value on the select input.
	      // It is also used to guard against 32 bit
	      // unsigned -> signed address overflow / wrap around.
	    ಆದರೆ (!vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(ಬಿಟ್, tmp, is_signed_) || (!is_signed_ && tmp < 0))
		  tmp = INT32_MIN;
	    ಆದರೆ (static_cast<ಪೂರ್ಣಾಂಕ>(tmp) == ತಳ) ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	    ತಳ = tmp;
	    ನಿಲ್ಲಿಸು;
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "Unsupported port type %u.\n", ಪೋರ್ಟ್.ಪೋರ್ಟ್());
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ನಿಲ್ಲಿಸು;
      }

      vvp_vector4_t res (wid_);

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < wid_ ;  idx += 1) {
	    ಪೂರ್ಣಾಂಕ adr = ತಳ+idx;
	    ಆದರೆ (adr < 0) ಮುಂದುವರಿ;
	    ಆದರೆ ((ಚಿಹ್ನೆರಹಿತ)adr >= ಮೂಲ.size()) ನಿಲ್ಲಿಸು;

	    res.ಗಣ_ಬಿಟ್(idx, ಮೂಲ.value((ಚಿಹ್ನೆರಹಿತ)adr));
      }

      ಆದರೆ (! ref.eeq(res)) {
	    ref = res;
            ಹಿಂತಿರುಗಿ ನಿಜ;
      }
      ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

vvp_fun_part_var_sa::vvp_fun_part_var_sa(ಚಿಹ್ನೆರಹಿತ w, ತಾರ್ಕಿಕ is_signed)
: vvp_fun_part_var(w, is_signed), ತಳ_2(0)
{
}

vvp_fun_part_var_sa::~vvp_fun_part_var_sa()
{
}

ಶೂನ್ಯ vvp_fun_part_var_sa::recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                                    vvp_context_t)
{
      ಆದರೆ (recv_vec4_(ಪೋರ್ಟ್, ಬಿಟ್, ತಳ_2, ಮೂಲ_2, ref_)) {
	    ಪೋರ್ಟ್.ptr()->send_vec4(ref_, 0);
      }
}

ಶೂನ್ಯ vvp_fun_part_var_sa::recv_vec4_pv(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
				       ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t)
{
      vvp_vector4_t tmp = ಮೂಲ_2;
      ಆದರೆ (tmp.size() == 0)
	    tmp = vvp_vector4_t(vwid);

      ಪ್ರತಿಜ್ಞಾ(tmp.size() == vwid);
      tmp.ಗಣ_vec(ತಳ, ಬಿಟ್);
      recv_vec4(ಪೋರ್ಟ್, tmp, 0);
}

ರಚನೆ vvp_fun_part_var_state_s {
      vvp_fun_part_var_state_s() : ತಳ(0) { }

      ಪೂರ್ಣಾಂಕ ತಳ;
      vvp_vector4_t ಮೂಲ;
      vvp_vector4_t ref;
};

vvp_fun_part_var_aa::vvp_fun_part_var_aa(ಚಿಹ್ನೆರಹಿತ w, ತಾರ್ಕಿಕ is_signed)
: vvp_fun_part_var(w, is_signed)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_ಕೂಡಿಸು_ಅಂಶ_ಗಮ್ಯ_context(ಇದು, context_scope_);
}

vvp_fun_part_var_aa::~vvp_fun_part_var_aa()
{
}

ಶೂನ್ಯ vvp_fun_part_var_aa::alloc_instance(vvp_context_t context)
{
      vvp_ಗಣ_context_ಅಂಶ(context, context_idx_, ಹೊಸ vvp_fun_part_var_state_s);
}

ಶೂನ್ಯ vvp_fun_part_var_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
            (vvp_get_context_ಅಂಶ(context, context_idx_));

      state->ತಳ = 0;
      state->ಮೂಲ.ಗಣ_ಗಮ್ಯ_x();
      state->ref.ಗಣ_ಗಮ್ಯ_x();
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಶೂನ್ಯ vvp_fun_part_var_aa::free_instance(vvp_context_t context)
{
      vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
            (vvp_get_context_ಅಂಶ(context, context_idx_));
      ಅಳಿಸು state;
}
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ vvp_fun_part_var_aa::recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                                    vvp_context_t context)
{
      ಆದರೆ (context) {
            vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
                  (vvp_get_context_ಅಂಶ(context, context_idx_));

            ಆದರೆ (recv_vec4_(ಪೋರ್ಟ್, ಬಿಟ್, state->ತಳ, state->ಮೂಲ, state->ref)) {
                  ಪೋರ್ಟ್.ptr()->send_vec4(state->ref, context);
            }
      } ಇಲ್ಲದಿದ್ದರೆ {
            context = context_scope_->ಜೀವಂತ_contexts;
            ಆಗ (context) {
                  recv_vec4(ಪೋರ್ಟ್, ಬಿಟ್, context);
                  context = vvp_get_ಮುಂದಿನ_context(context);
            }
      }
}

ಶೂನ್ಯ vvp_fun_part_var_aa::recv_vec4_pv(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
				       ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t context)
{
      ಆದರೆ (context) {
	    ಸ್ಥಿರಾಂಕ vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
	                                                      (vvp_get_context_ಅಂಶ(context,
	                                                       context_idx_));

            vvp_vector4_t tmp = state->ಮೂಲ;
            ಆದರೆ (tmp.size() == 0)
                  tmp = vvp_vector4_t(vwid);

            ಪ್ರತಿಜ್ಞಾ(tmp.size() == vwid);
            tmp.ಗಣ_vec(ತಳ, ಬಿಟ್);
            recv_vec4(ಪೋರ್ಟ್, tmp, context);
      } ಇಲ್ಲದಿದ್ದರೆ {
            context = context_scope_->ಜೀವಂತ_contexts;
            ಆಗ (context) {
                  recv_vec4(ಪೋರ್ಟ್, ಬಿಟ್, context);
                  context = vvp_get_ಮುಂದಿನ_context(context);
            }
      }
}

/*
 * Given a node functor, create a network node and link it into the
 * netlist. This form assumes nodes with a single input.
 */
ಶೂನ್ಯ link_ಗಂಟು_1(ಅಕ್ಷರ*label, ಅಕ್ಷರ*ಮೂಲ, vvp_net_fun_t*fun)
{
      vvp_net_t*net = ಹೊಸ vvp_net_t;
      net->fun = fun;

      define_functor_ಚಿಹ್ನೆ(label, net);
      ಬಿಡುಗಡೆ(label);

      ಒಳಹರಿವು_connect(net, 0, ಮೂಲ);
}

ಶೂನ್ಯ compile_part_select(ಅಕ್ಷರ*label, ಅಕ್ಷರ*ಮೂಲ,
			 ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ wid)
{
      vvp_fun_part*fun = 0;
      ಆದರೆ (vpip_peek_current_scope()->is_automatic()) {
            fun = ಹೊಸ vvp_fun_part_aa(ತಳ, wid);
      } ಇಲ್ಲದಿದ್ದರೆ {
            fun = ಹೊಸ vvp_fun_part_sa(ತಳ, wid);
      }
      link_ಗಂಟು_1(label, ಮೂಲ, fun);
}

ಶೂನ್ಯ compile_part_select_pv(ಅಕ್ಷರ*label, ಅಕ್ಷರ*ಮೂಲ,
			    ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ wid,
			    ಚಿಹ್ನೆರಹಿತ ಸದಿಶ_wid)
{
      vvp_fun_part_pv*fun = ಹೊಸ vvp_fun_part_pv(ತಳ, wid, ಸದಿಶ_wid);
      link_ಗಂಟು_1(label, ಮೂಲ, fun);
}

ಶೂನ್ಯ compile_part_select_var(ಅಕ್ಷರ*label, ಅಕ್ಷರ*ಮೂಲ, ಅಕ್ಷರ*var,
			     ಚಿಹ್ನೆರಹಿತ wid, ತಾರ್ಕಿಕ is_signed)
{
      vvp_fun_part_var*fun = 0;
      ಆದರೆ (vpip_peek_current_scope()->is_automatic()) {
            fun = ಹೊಸ vvp_fun_part_var_aa(wid, is_signed);
      } ಇಲ್ಲದಿದ್ದರೆ {
            fun = ಹೊಸ vvp_fun_part_var_sa(wid, is_signed);
      }
      vvp_net_t*net = ಹೊಸ vvp_net_t;
      net->fun = fun;

      define_functor_ಚಿಹ್ನೆ(label, net);
      ಬಿಡುಗಡೆ(label);

      ಒಳಹರಿವು_connect(net, 0, ಮೂಲ);
      ಒಳಹರಿವು_connect(net, 1, var);
}
