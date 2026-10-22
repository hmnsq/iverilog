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

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ std;

ਬਣਤਰ vvp_fun_part_state_s {
      vvp_fun_part_state_s() {}

      vvp_vector4_t ਬਿੱਟ;
};

vvp_fun_part::vvp_fun_part(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
: ਅਧਾਰ_2(ਅਧਾਰ), wid_(wid)
{
}

vvp_fun_part::~vvp_fun_part()
{
}

vvp_fun_part_sa::vvp_fun_part_sa(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
: vvp_fun_part(ਅਧਾਰ, wid)
{
      net_ = 0;
}

vvp_fun_part_sa::~vvp_fun_part_sa()
{
}

ਖਾਲੀ vvp_fun_part_sa::recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                                vvp_context_t)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ਪੋਰਟ.ਪੋਰਟ() == 0);

      vvp_vector4_t tmp (ਦੋਆਧਾਰੀ_ਅੰਕ, ਅਧਾਰ_2, wid_);
      ਜੇ (val_ .eeq( tmp ))
	    ਵਾਪਸ;

      val_ = tmp;

      ਜੇ (net_ == 0) {
	    net_ = ਪੋਰਟ.ptr();
	    ਸਮਾਂ_ਸਾਰਣੀ_functor(ਇਹ);
      }
}

/*
 * Handle the case that the part select node is actually fed by a part
 * select assignment. It's not exactly clear what might make this
 * happen, but is does seem to happen and this should have well
 * defined behavior.
 */
ਖਾਲੀ vvp_fun_part_sa::recv_vec4_pv(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
				   ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t)
{
      vvp_vector4_t tmp (vwid, BIT4_Z);
      tmp.ਸੈੱਟ_vec(ਅਧਾਰ_2, val_);
      tmp.ਸੈੱਟ_vec(ਅਧਾਰ, ਦੋਆਧਾਰੀ_ਅੰਕ);
      recv_vec4(ਪੋਰਟ, tmp, 0);
}

ਖਾਲੀ vvp_fun_part_sa::ਲੜੀਵਾਰ_ਲੜੀਵਾਰ()
{
      vvp_net_t*ptr = net_;
      net_ = 0;
      ptr->send_vec4(val_, 0);
}

vvp_fun_part_aa::vvp_fun_part_aa(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
: vvp_fun_part(ਅਧਾਰ, wid)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_ਜੋੜੋ_ਵਸਤੂ_ਮੰਜ਼ਿਲ_context(ਇਹ, context_scope_);
}

vvp_fun_part_aa::~vvp_fun_part_aa()
{
}

ਖਾਲੀ vvp_fun_part_aa::alloc_instance(vvp_context_t context)
{
      vvp_ਸੈੱਟ_context_ਵਸਤੂ(context, context_idx_, ਨਵਾਂ vvp_vector4_t);
}

ਖਾਲੀ vvp_fun_part_aa::reset_instance(vvp_context_t context)
{
      vvp_vector4_t*val = static_cast<vvp_vector4_t*>
            (vvp_get_context_ਵਸਤੂ(context, context_idx_));

      val->ਸੈੱਟ_ਮੰਜ਼ਿਲ_x();
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਖਾਲੀ vvp_fun_part_aa::free_instance(vvp_context_t context)
{
      vvp_vector4_t*val = static_cast<vvp_vector4_t*>
            (vvp_get_context_ਵਸਤੂ(context, context_idx_));
      ਮਿਟਾਓ val;
}
#ਜੇ_ਅੰਤ

ਖਾਲੀ vvp_fun_part_aa::recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                                vvp_context_t context)
{
      ਜੇ (context) {
            ਪ੍ਰਤਿਜ੍ਞਾ(ਪੋਰਟ.ਪੋਰਟ() == 0);

            vvp_vector4_t*val = static_cast<vvp_vector4_t*>
                  (vvp_get_context_ਵਸਤੂ(context, context_idx_));

            vvp_vector4_t tmp (wid_, BIT4_X);
            ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < wid_ ;  idx += 1) {
                  ਜੇ (idx + ਅਧਾਰ_2 < ਦੋਆਧਾਰੀ_ਅੰਕ.size())
                        tmp.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, ਦੋਆਧਾਰੀ_ਅੰਕ.value(ਅਧਾਰ_2+idx));
            }
            ਜੇ (!val->eeq( tmp )) {
                  *val = tmp;
                  ਪੋਰਟ.ptr()->send_vec4(tmp, context);
            }
      } ਨਹੀਂਤਾਂ {
            context = context_scope_->ਜੀਵਤ_contexts;
            ਜਦੋਂ (context) {
                  recv_vec4(ਪੋਰਟ, ਦੋਆਧਾਰੀ_ਅੰਕ, context);
                  context = vvp_get_ਅਗਲਾ_context(context);
            }
      }
}

/*
 * Handle the case that the part select node is actually fed by a part
 * select assignment. It's not exactly clear what might make this
 * happen, but is does seem to happen and this should have well
 * defined behavior.
 */
ਖਾਲੀ vvp_fun_part_aa::recv_vec4_pv(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
				   ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t context)
{
      ਜੇ (context) {
	    ਅਚਲ vvp_vector4_t*val = static_cast<vvp_vector4_t*>
	                                         (vvp_get_context_ਵਸਤੂ(context,
	                                                               context_idx_));

	    vvp_vector4_t tmp (vwid, BIT4_Z);
	    tmp.ਸੈੱਟ_vec(ਅਧਾਰ_2, *val);
	    tmp.ਸੈੱਟ_vec(ਅਧਾਰ, ਦੋਆਧਾਰੀ_ਅੰਕ);
	    recv_vec4(ਪੋਰਟ, tmp, context);
      } ਨਹੀਂਤਾਂ {
	    context = context_scope_->ਜੀਵਤ_contexts;
	    ਜਦੋਂ (context) {
		  recv_vec4_pv(ਪੋਰਟ, ਦੋਆਧਾਰੀ_ਅੰਕ, ਅਧਾਰ, vwid, context);
		  context = vvp_get_ਅਗਲਾ_context(context);
	    }
      }
}

vvp_fun_part_pv::vvp_fun_part_pv(ਬਿਨਾਨਿਸ਼ਾਨੀ b, ਬਿਨਾਨਿਸ਼ਾਨੀ w, ਬਿਨਾਨਿਸ਼ਾਨੀ v)
: ਅਧਾਰ_2(b), wid_(w), vwid_(v)
{
}

vvp_fun_part_pv::~vvp_fun_part_pv()
{
}

ਖਾਲੀ vvp_fun_part_pv::recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                                vvp_context_t context)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ਪੋਰਟ.ਪੋਰਟ() == 0);

      ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.size() != wid_) {
	    cerr << "internal error: part_pv data mismatch. "
		 << "base_=" << ਅਧਾਰ_2 << ", wid_=" << wid_
		 << ", vwid_=" << vwid_ << ", bit=" << ਦੋਆਧਾਰੀ_ਅੰਕ
		 << ਸਤਰ_ਅੰਤ;
      }
      ਪ੍ਰਤਿਜ੍ਞਾ(ਦੋਆਧਾਰੀ_ਅੰਕ.size() == wid_);

      ਪੋਰਟ.ptr()->send_vec4_pv(ਦੋਆਧਾਰੀ_ਅੰਕ, ਅਧਾਰ_2, vwid_, context);
}

ਖਾਲੀ vvp_fun_part_pv::recv_vec4_pv(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                                 ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t ctx)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ਪੋਰਟ.ਪੋਰਟ() == 0);
      ਪ੍ਰਤਿਜ੍ਞਾ(ਅਧਾਰ + ਦੋਆਧਾਰੀ_ਅੰਕ.size() <= vwid);
      ਪ੍ਰਤਿਜ੍ਞਾ(vwid == wid_);

      vvp_vector4_t tmp(wid_, BIT4_Z);
      tmp.ਸੈੱਟ_vec(ਅਧਾਰ, ਦੋਆਧਾਰੀ_ਅੰਕ);

      ਪੋਰਟ.ptr()->send_vec4_pv(tmp, ਅਧਾਰ_2, vwid_, ctx);
}

ਖਾਲੀ vvp_fun_part_pv::recv_vec8(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ਪੋਰਟ.ਪੋਰਟ() == 0);

      ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.size() != wid_) {
	    cerr << "internal error: part_pv (strength-aware) data mismatch. "
		 << "base_=" << ਅਧਾਰ_2 << ", wid_=" << wid_
		 << ", vwid_=" << vwid_ << ", bit=" << ਦੋਆਧਾਰੀ_ਅੰਕ
		 << ਸਤਰ_ਅੰਤ;
      }
      ਪ੍ਰਤਿਜ੍ਞਾ(ਦੋਆਧਾਰੀ_ਅੰਕ.size() == wid_);

      ਪੋਰਟ.ptr()->send_vec8_pv(ਦੋਆਧਾਰੀ_ਅੰਕ, ਅਧਾਰ_2, vwid_);
}

vvp_fun_part_var::vvp_fun_part_var(ਬਿਨਾਨਿਸ਼ਾਨੀ w, ਤਾਰਕਿਕ is_signed)
: wid_(w), is_signed_(is_signed)
{
}

vvp_fun_part_var::~vvp_fun_part_var()
{
}

ਤਾਰਕਿਕ vvp_fun_part_var::recv_vec4_(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                                  ਪੂਰਨ&ਅਧਾਰ, vvp_vector4_t&ਸਰੋਤ,
                                  vvp_vector4_t&ref)
{
      int32_t tmp;
      ਚੋਣ (ਪੋਰਟ.ਪੋਰਟ()) {
	  ਹਾਲਤ 0:
	    ਸਰੋਤ = ਦੋਆਧਾਰੀ_ਅੰਕ;
	    ਰੋਕੋ;
	  ਹਾਲਤ 1:
	      // INT_MIN is before the vector and is used to
	      // represent a 'bx value on the select input.
	      // It is also used to guard against 32 bit
	      // unsigned -> signed address overflow / wrap around.
	    ਜੇ (!vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(ਦੋਆਧਾਰੀ_ਅੰਕ, tmp, is_signed_) || (!is_signed_ && tmp < 0))
		  tmp = INT32_MIN;
	    ਜੇ (static_cast<ਪੂਰਨ>(tmp) == ਅਧਾਰ) ਵਾਪਸ ਝੂਠ;
	    ਅਧਾਰ = tmp;
	    ਰੋਕੋ;
	  ਮੂਲ:
	    fprintf(stderr, "Unsupported port type %u.\n", ਪੋਰਟ.ਪੋਰਟ());
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
	    ਰੋਕੋ;
      }

      vvp_vector4_t res (wid_);

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < wid_ ;  idx += 1) {
	    ਪੂਰਨ adr = ਅਧਾਰ+idx;
	    ਜੇ (adr < 0) ਜਾਰੀ;
	    ਜੇ ((ਬਿਨਾਨਿਸ਼ਾਨੀ)adr >= ਸਰੋਤ.size()) ਰੋਕੋ;

	    res.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, ਸਰੋਤ.value((ਬਿਨਾਨਿਸ਼ਾਨੀ)adr));
      }

      ਜੇ (! ref.eeq(res)) {
	    ref = res;
            ਵਾਪਸ ਸੱਚ;
      }
      ਵਾਪਸ ਝੂਠ;
}

vvp_fun_part_var_sa::vvp_fun_part_var_sa(ਬਿਨਾਨਿਸ਼ਾਨੀ w, ਤਾਰਕਿਕ is_signed)
: vvp_fun_part_var(w, is_signed), ਅਧਾਰ_2(0)
{
}

vvp_fun_part_var_sa::~vvp_fun_part_var_sa()
{
}

ਖਾਲੀ vvp_fun_part_var_sa::recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                                    vvp_context_t)
{
      ਜੇ (recv_vec4_(ਪੋਰਟ, ਦੋਆਧਾਰੀ_ਅੰਕ, ਅਧਾਰ_2, ਸਰੋਤ_2, ref_)) {
	    ਪੋਰਟ.ptr()->send_vec4(ref_, 0);
      }
}

ਖਾਲੀ vvp_fun_part_var_sa::recv_vec4_pv(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
				       ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t)
{
      vvp_vector4_t tmp = ਸਰੋਤ_2;
      ਜੇ (tmp.size() == 0)
	    tmp = vvp_vector4_t(vwid);

      ਪ੍ਰਤਿਜ੍ਞਾ(tmp.size() == vwid);
      tmp.ਸੈੱਟ_vec(ਅਧਾਰ, ਦੋਆਧਾਰੀ_ਅੰਕ);
      recv_vec4(ਪੋਰਟ, tmp, 0);
}

ਬਣਤਰ vvp_fun_part_var_state_s {
      vvp_fun_part_var_state_s() : ਅਧਾਰ(0) { }

      ਪੂਰਨ ਅਧਾਰ;
      vvp_vector4_t ਸਰੋਤ;
      vvp_vector4_t ref;
};

vvp_fun_part_var_aa::vvp_fun_part_var_aa(ਬਿਨਾਨਿਸ਼ਾਨੀ w, ਤਾਰਕਿਕ is_signed)
: vvp_fun_part_var(w, is_signed)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_ਜੋੜੋ_ਵਸਤੂ_ਮੰਜ਼ਿਲ_context(ਇਹ, context_scope_);
}

vvp_fun_part_var_aa::~vvp_fun_part_var_aa()
{
}

ਖਾਲੀ vvp_fun_part_var_aa::alloc_instance(vvp_context_t context)
{
      vvp_ਸੈੱਟ_context_ਵਸਤੂ(context, context_idx_, ਨਵਾਂ vvp_fun_part_var_state_s);
}

ਖਾਲੀ vvp_fun_part_var_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
            (vvp_get_context_ਵਸਤੂ(context, context_idx_));

      state->ਅਧਾਰ = 0;
      state->ਸਰੋਤ.ਸੈੱਟ_ਮੰਜ਼ਿਲ_x();
      state->ref.ਸੈੱਟ_ਮੰਜ਼ਿਲ_x();
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਖਾਲੀ vvp_fun_part_var_aa::free_instance(vvp_context_t context)
{
      vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
            (vvp_get_context_ਵਸਤੂ(context, context_idx_));
      ਮਿਟਾਓ state;
}
#ਜੇ_ਅੰਤ

ਖਾਲੀ vvp_fun_part_var_aa::recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                                    vvp_context_t context)
{
      ਜੇ (context) {
            vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
                  (vvp_get_context_ਵਸਤੂ(context, context_idx_));

            ਜੇ (recv_vec4_(ਪੋਰਟ, ਦੋਆਧਾਰੀ_ਅੰਕ, state->ਅਧਾਰ, state->ਸਰੋਤ, state->ref)) {
                  ਪੋਰਟ.ptr()->send_vec4(state->ref, context);
            }
      } ਨਹੀਂਤਾਂ {
            context = context_scope_->ਜੀਵਤ_contexts;
            ਜਦੋਂ (context) {
                  recv_vec4(ਪੋਰਟ, ਦੋਆਧਾਰੀ_ਅੰਕ, context);
                  context = vvp_get_ਅਗਲਾ_context(context);
            }
      }
}

ਖਾਲੀ vvp_fun_part_var_aa::recv_vec4_pv(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
				       ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t context)
{
      ਜੇ (context) {
	    ਅਚਲ vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
	                                                      (vvp_get_context_ਵਸਤੂ(context,
	                                                       context_idx_));

            vvp_vector4_t tmp = state->ਸਰੋਤ;
            ਜੇ (tmp.size() == 0)
                  tmp = vvp_vector4_t(vwid);

            ਪ੍ਰਤਿਜ੍ਞਾ(tmp.size() == vwid);
            tmp.ਸੈੱਟ_vec(ਅਧਾਰ, ਦੋਆਧਾਰੀ_ਅੰਕ);
            recv_vec4(ਪੋਰਟ, tmp, context);
      } ਨਹੀਂਤਾਂ {
            context = context_scope_->ਜੀਵਤ_contexts;
            ਜਦੋਂ (context) {
                  recv_vec4(ਪੋਰਟ, ਦੋਆਧਾਰੀ_ਅੰਕ, context);
                  context = vvp_get_ਅਗਲਾ_context(context);
            }
      }
}

/*
 * Given a node functor, create a network node and link it into the
 * netlist. This form assumes nodes with a single input.
 */
ਖਾਲੀ link_ਗੰਢ_1(ਅੱਖਰ*label, ਅੱਖਰ*ਸਰੋਤ, vvp_net_fun_t*fun)
{
      vvp_net_t*net = ਨਵਾਂ vvp_net_t;
      net->fun = fun;

      define_functor_ਚਿੰਨ੍ਹ(label, net);
      ਮੁਕਤ(label);

      ਨਿਵੇਸ਼_connect(net, 0, ਸਰੋਤ);
}

ਖਾਲੀ compile_part_select(ਅੱਖਰ*label, ਅੱਖਰ*ਸਰੋਤ,
			 ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
{
      vvp_fun_part*fun = 0;
      ਜੇ (vpip_peek_current_scope()->is_automatic()) {
            fun = ਨਵਾਂ vvp_fun_part_aa(ਅਧਾਰ, wid);
      } ਨਹੀਂਤਾਂ {
            fun = ਨਵਾਂ vvp_fun_part_sa(ਅਧਾਰ, wid);
      }
      link_ਗੰਢ_1(label, ਸਰੋਤ, fun);
}

ਖਾਲੀ compile_part_select_pv(ਅੱਖਰ*label, ਅੱਖਰ*ਸਰੋਤ,
			    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ wid,
			    ਬਿਨਾਨਿਸ਼ਾਨੀ ਸਦਿਸ਼_wid)
{
      vvp_fun_part_pv*fun = ਨਵਾਂ vvp_fun_part_pv(ਅਧਾਰ, wid, ਸਦਿਸ਼_wid);
      link_ਗੰਢ_1(label, ਸਰੋਤ, fun);
}

ਖਾਲੀ compile_part_select_var(ਅੱਖਰ*label, ਅੱਖਰ*ਸਰੋਤ, ਅੱਖਰ*var,
			     ਬਿਨਾਨਿਸ਼ਾਨੀ wid, ਤਾਰਕਿਕ is_signed)
{
      vvp_fun_part_var*fun = 0;
      ਜੇ (vpip_peek_current_scope()->is_automatic()) {
            fun = ਨਵਾਂ vvp_fun_part_var_aa(wid, is_signed);
      } ਨਹੀਂਤਾਂ {
            fun = ਨਵਾਂ vvp_fun_part_var_sa(wid, is_signed);
      }
      vvp_net_t*net = ਨਵਾਂ vvp_net_t;
      net->fun = fun;

      define_functor_ਚਿੰਨ੍ਹ(label, net);
      ਮੁਕਤ(label);

      ਨਿਵੇਸ਼_connect(net, 0, ਸਰੋਤ);
      ਨਿਵੇਸ਼_connect(net, 1, var);
}
