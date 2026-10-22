/*
 * Copyright (c) 2004-2020 Stephen Williams (steve@icarus.com)
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

# include  "config.h"
# include  "vvp_net.h"
# include  "vvp_net_sig.h"
# include  "statistics.h"
# include  "vpi_priv.h"
# include  <vector>
# include  <cassert>
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
# include  <valgrind/memcheck.h>
# include  <map>
#ਜੇ_ਅੰਤ

# include  <iostream>

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ std;

/*
 * The filter_mask_ method takes as an input the value to propagate,
 * the mask of what is being forced, and returns a propagation
 * mode. In the process, it may update the filtered output value.
 *
 * The input value is the subvector "val" that is placed as "base" in
 * the output. The val may be shorter then the target vector.
 *
 * The "force" vector in the value being force, with the force_mask_
 * member a bit mask of which parts of the force vector really apply.
 */
ਸਾਂਚਾ <ਵਰਗ T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_ਨਕਾਬ(ਅਚਲ T&val, ਅਚਲ T&force, T&filter, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ)
{
      ਜੇ (!test_force_ਨਕਾਬ_is_zero()) {
	      // Some bits are being forced. Go through the
	      // force_mask_ and force value to see which bits are
	      // propagated and which are kept from the forced
	      // value. Update the filter with the filtered result and
	      // return REPL to indicate that some bits have changed,
	      // or STOP if no bits change.
	    ਤਾਰਕਿਕ propagate_flag = force_propagate_;
	    force_propagate_ = ਝੂਠ;
	    ਪ੍ਰਤਿਜ੍ਞਾ(force_ਨਕਾਬ_2.size() == force.size());
	    ਪ੍ਰਤਿਜ੍ਞਾ((ਅਧਾਰ+val.size()) <= force_ਨਕਾਬ_2.size());

	    filter = val;
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < val.size() ; idx += 1) {
		  ਜੇ (force_ਨਕਾਬ_2.value(ਅਧਾਰ+idx))
			filter.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, force.value(ਅਧਾਰ+idx));
		  ਨਹੀਂਤਾਂ
			propagate_flag = ਸੱਚ;
	    }

	    ਜੇ (propagate_flag) {
		  ਲੜੀਵਾਰ_vpi_callbacks();
		  ਵਾਪਸ REPL;
	    } ਨਹੀਂਤਾਂ {
		  ਵਾਪਸ STOP;
	    }

      } ਨਹੀਂਤਾਂ {
	    ਲੜੀਵਾਰ_vpi_callbacks();
	    ਵਾਪਸ PROP;
      }
}

ਸਾਂਚਾ <ਵਰਗ T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_ਨਕਾਬ(T&val, T force)
{

      ਜੇ (test_force_ਨਕਾਬ(0)) {
	    val = force;
	    ਲੜੀਵਾਰ_vpi_callbacks();
	    ਵਾਪਸ REPL;
      }
      ਲੜੀਵਾਰ_vpi_callbacks();
      ਵਾਪਸ PROP;
}

ਸਾਂਚਾ <ਵਰਗ T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_ਨਿਵੇਸ਼_ਨਕਾਬ(ਅਚਲ T&val, ਅਚਲ T&force, T&rep) ਅਚਲ
{
      ਜੇ (test_force_ਨਕਾਬ_is_zero())
	    ਵਾਪਸ PROP;

      ਪ੍ਰਤਿਜ੍ਞਾ(force_ਨਕਾਬ_2.size() == force.size());

      rep = val;
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < val.size() ; idx += 1) {
	    ਜੇ (force_ਨਕਾਬ_2.value(idx))
		  rep.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, force.value(idx));
      }

      ਵਾਪਸ REPL;
}

vvp_signal_ਮੁੱਲ::~vvp_signal_ਮੁੱਲ()
{
}

ਦੁੱਗਣਾ vvp_signal_ਮੁੱਲ::real_ਮੁੱਲ() ਅਚਲ
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
      ਵਾਪਸ 0;
}

ਖਾਲੀ vvp_net_t::force_vec4(ਅਚਲ vvp_vector4_t&val, ਅਚਲ vvp_vector2_t&ਨਕਾਬ)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(fil);
      fil->force_fil_vec4(val, ਨਕਾਬ);
      fun->force_flag(ਝੂਠ);
      vvp_send_vec4(ਬਾਹਰਲਾ_2, val, 0);
}

ਖਾਲੀ vvp_net_t::force_vec8(ਅਚਲ vvp_vector8_t&val, ਅਚਲ vvp_vector2_t&ਨਕਾਬ)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(fil);
      fil->force_fil_vec8(val, ਨਕਾਬ);
      fun->force_flag(ਝੂਠ);
      vvp_send_vec8(ਬਾਹਰਲਾ_2, val);
}

ਖਾਲੀ vvp_net_t::force_real(ਦੁੱਗਣਾ val, ਅਚਲ vvp_vector2_t&ਨਕਾਬ)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(fil);
      fil->force_fil_real(val, ਨਕਾਬ);
      fun->force_flag(ਝੂਠ);
      vvp_send_real(ਬਾਹਰਲਾ_2, val, 0);
}

/* **** vvp_fun_signal methods **** */

vvp_fun_signal_ਅਧਾਰ::vvp_fun_signal_ਅਧਾਰ()
{
      continuous_ਸੌਂਪੋ_ਸਰਗਰਮ = ਝੂਠ;
      needs_init_ = ਸੱਚ;
      cassign_link = 0;
      ਗਿਣਤੀ_functors_sig += 1;
}

vvp_fun_signal4_sa::vvp_fun_signal4_sa(ਬਿਨਾਨਿਸ਼ਾਨੀ wid, vvp_bit4_t init)
: bits4_(wid, init)
{
}

/*
 * Nets simply reflect their input to their output.
 *
 * NOTE: It is a quirk of vvp_fun_signal that it has an initial value
 * that needs to be propagated, but after that it only needs to
 * propagate if the value changes. Eliminating duplicate propagations
 * should improve performance, but has the quirk that an input that
 * matches the initial value might not be propagated. The hack used
 * herein is to keep a "needs_init_" flag that is turned false after
 * the first propagation, and forces the first propagation to happen
 * even if it matches the initial value.
 */
ਖਾਲੀ vvp_fun_signal4_sa::recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                                   vvp_context_t)
{
      ਚੋਣ (ptr.ਪੋਰਟ()) {
	  ਹਾਲਤ 0: // Normal input (feed from net, or set from process)
	      /* If we don't have a continuous assign mask then just
		 copy the bits, otherwise we need to see if there are
		 any holes in the mask so we can set those bits. */
	    ਜੇ (ਸੌਂਪੋ_ਨਕਾਬ.size() == 0) {
                  ਜੇ (needs_init_ || !bits4_.eeq(ਦੋਆਧਾਰੀ_ਅੰਕ)) {
			ਪ੍ਰਤਿਜ੍ਞਾ(ਦੋਆਧਾਰੀ_ਅੰਕ.size() == bits4_.size());
			bits4_ = ਦੋਆਧਾਰੀ_ਅੰਕ;
			needs_init_ = ਝੂਠ;
			ptr.ptr()->send_vec4(bits4_, 0);
		  }
	    } ਨਹੀਂਤਾਂ {
		  ਤਾਰਕਿਕ ਬਦਲਿਆ = ਝੂਠ;
		  ਪ੍ਰਤਿਜ੍ਞਾ(bits4_.size() == ਸੌਂਪੋ_ਨਕਾਬ.size());
		  ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < ਦੋਆਧਾਰੀ_ਅੰਕ.size() ;  idx += 1) {
			ਜੇ (idx >= bits4_.size()) ਰੋਕੋ;
			ਜੇ (ਸੌਂਪੋ_ਨਕਾਬ.value(idx)) ਜਾਰੀ;
			bits4_.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, ਦੋਆਧਾਰੀ_ਅੰਕ.value(idx));
			ਬਦਲਿਆ = ਸੱਚ;
		  }
		  ਜੇ (ਬਦਲਿਆ) {
			needs_init_ = ਝੂਠ;
			ptr.ptr()->send_vec4(bits4_, 0);
		  }
	    }
	    ਰੋਕੋ;

	  ਹਾਲਤ 1: // Continuous assign value
	      // Handle the simple case of the linked source being wider
	      // than this signal. Note we don't yet support the case of
	      // the linked source being narrower than this signal, or
	      // the case of an expression being assigned.
	    bits4_ = coerce_ਮੰਜ਼ਿਲ_ਚੌੜਾਈ(ਦੋਆਧਾਰੀ_ਅੰਕ, bits4_.size());
	    ਸੌਂਪੋ_ਨਕਾਬ = vvp_vector2_t(vvp_vector2_t::FILL1, bits4_.size());
	    ptr.ptr()->send_vec4(bits4_, 0);
	    ਰੋਕੋ;

	  ਮੂਲ:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.ਪੋਰਟ());
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
	    ਰੋਕੋ;
      }
}

ਖਾਲੀ vvp_fun_signal4_sa::recv_vec8(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ)
{
      recv_vec4(ptr, reduce4(ਦੋਆਧਾਰੀ_ਅੰਕ), 0);
}

ਖਾਲੀ vvp_fun_signal4_sa::recv_vec4_pv(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
				      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(bits4_.size() == vwid);
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = ਦੋਆਧਾਰੀ_ਅੰਕ.size();

      ਚੋਣ (ptr.ਪੋਰਟ()) {
	  ਹਾਲਤ 0: // Normal input
	    ਜੇ (ਸੌਂਪੋ_ਨਕਾਬ.size() == 0) {
                  ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < wid ;  idx += 1) {
			ਜੇ (ਅਧਾਰ+idx >= bits4_.size()) ਰੋਕੋ;
			bits4_.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(ਅਧਾਰ+idx, ਦੋਆਧਾਰੀ_ਅੰਕ.value(idx));
		  }
		  needs_init_ = ਝੂਠ;
		  ptr.ptr()->send_vec4(bits4_,0);
	    } ਨਹੀਂਤਾਂ {
		  ਤਾਰਕਿਕ ਬਦਲਿਆ = ਝੂਠ;
		  ਪ੍ਰਤਿਜ੍ਞਾ(bits4_.size() == ਸੌਂਪੋ_ਨਕਾਬ.size());
		  ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < wid ;  idx += 1) {
			ਜੇ (ਅਧਾਰ+idx >= bits4_.size()) ਰੋਕੋ;
			ਜੇ (ਸੌਂਪੋ_ਨਕਾਬ.value(ਅਧਾਰ+idx)) ਜਾਰੀ;
			bits4_.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(ਅਧਾਰ+idx, ਦੋਆਧਾਰੀ_ਅੰਕ.value(idx));
			ਬਦਲਿਆ = ਸੱਚ;
		  }
		  ਜੇ (ਬਦਲਿਆ) {
			needs_init_ = ਝੂਠ;
			ptr.ptr()->send_vec4(bits4_,0);
		  }
	    }
	    ਰੋਕੋ;

	  ਹਾਲਤ 1: // Continuous assign value
	    ਜੇ (ਸੌਂਪੋ_ਨਕਾਬ.size() == 0)
		  ਸੌਂਪੋ_ਨਕਾਬ = vvp_vector2_t(vvp_vector2_t::FILL0, bits4_.size());
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < wid ;  idx += 1) {
		  ਜੇ (ਅਧਾਰ+idx >= bits4_.size())
			ਰੋਕੋ;
		  bits4_.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(ਅਧਾਰ+idx, ਦੋਆਧਾਰੀ_ਅੰਕ.value(idx));
		  ਸੌਂਪੋ_ਨਕਾਬ.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(ਅਧਾਰ+idx, 1);
	    }
	    ptr.ptr()->send_vec4(bits4_,0);
	    ਰੋਕੋ;

	  ਮੂਲ:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.ਪੋਰਟ());
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
	    ਰੋਕੋ;
      }
}

ਖਾਲੀ vvp_fun_signal4_sa::recv_vec8_pv(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
				      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid)
{
      recv_vec4_pv(ptr, reduce4(ਦੋਆਧਾਰੀ_ਅੰਕ), ਅਧਾਰ, vwid, 0);
}

ਖਾਲੀ vvp_fun_signal_ਅਧਾਰ::deassign()
{
      continuous_ਸੌਂਪੋ_ਸਰਗਰਮ = ਝੂਠ;
      ਸੌਂਪੋ_ਨਕਾਬ = vvp_vector2_t();
}

ਖਾਲੀ vvp_fun_signal_ਅਧਾਰ::deassign_pv(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
{
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < wid ;  idx += 1) {
	    ਸੌਂਪੋ_ਨਕਾਬ.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(ਅਧਾਰ+idx, 0);
      }

      ਜੇ (ਸੌਂਪੋ_ਨਕਾਬ.is_zero()) {
	    ਸੌਂਪੋ_ਨਕਾਬ = vvp_vector2_t();
      }
}

ਖਾਲੀ automatic_signal_ਅਧਾਰ::release(vvp_net_ptr_t,ਤਾਰਕਿਕ)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}

ਖਾਲੀ automatic_signal_ਅਧਾਰ::release_pv(vvp_net_ptr_t,ਬਿਨਾਨਿਸ਼ਾਨੀ,ਬਿਨਾਨਿਸ਼ਾਨੀ,ਤਾਰਕਿਕ)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}

ਬਿਨਾਨਿਸ਼ਾਨੀ automatic_signal_ਅਧਾਰ::filter_ਮਾਪ() ਅਚਲ
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
      ਵਾਪਸ(0);
}
ਖਾਲੀ automatic_signal_ਅਧਾਰ::force_fil_vec4(ਅਚਲ vvp_vector4_t&, ਅਚਲ vvp_vector2_t&)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}
ਖਾਲੀ automatic_signal_ਅਧਾਰ::force_fil_vec8(ਅਚਲ vvp_vector8_t&, ਅਚਲ vvp_vector2_t&)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}
ਖਾਲੀ automatic_signal_ਅਧਾਰ::force_fil_real(ਦੁੱਗਣਾ, ਅਚਲ vvp_vector2_t&)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}
ਖਾਲੀ automatic_signal_ਅਧਾਰ::get_ਮੁੱਲ(ਬਣਤਰ t_vpi_ਮੁੱਲ*)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}

ਅਚਲ vvp_vector4_t& vvp_fun_signal4_sa::vec4_unfiltered_ਮੁੱਲ() ਅਚਲ
{
      ਵਾਪਸ bits4_;
}

vvp_fun_signal4_aa::vvp_fun_signal4_aa(ਬਿਨਾਨਿਸ਼ਾਨੀ wid, vvp_bit4_t init)
{
      context_idx_ = vpip_ਜੋੜੋ_ਵਸਤੂ_ਮੰਜ਼ਿਲ_context(ਇਹ, vpip_peek_context_scope());
      ਮਾਪ_2 = wid;
      init_ = init;
}

vvp_fun_signal4_aa::~vvp_fun_signal4_aa()
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}

ਖਾਲੀ vvp_fun_signal4_aa::alloc_instance(vvp_context_t context)
{
      vvp_ਸੈੱਟ_context_ਵਸਤੂ(context, context_idx_, ਨਵਾਂ vvp_vector4_t(ਮਾਪ_2, init_));
}

ਖਾਲੀ vvp_fun_signal4_aa::reset_instance(vvp_context_t context)
{
      vvp_vector4_t*ਬਿੱਟ = static_cast<vvp_vector4_t*>
            (vvp_get_context_ਵਸਤੂ(context, context_idx_));

      ਬਿੱਟ->fill_ਬਿੱਟ(init_);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਖਾਲੀ vvp_fun_signal4_aa::free_instance(vvp_context_t context)
{
      vvp_vector4_t*ਬਿੱਟ = static_cast<vvp_vector4_t*>
            (vvp_get_context_ਵਸਤੂ(context, context_idx_));
      ਮਿਟਾਓ ਬਿੱਟ;
}
#ਜੇ_ਅੰਤ

/*
 * Continuous and forced assignments are not permitted on automatic
 * variables. So we only expect to receive on port 0.
 */
ਖਾਲੀ vvp_fun_signal4_aa::recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                                   vvp_context_t context)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ptr.ਪੋਰਟ() == 0);
      ਪ੍ਰਤਿਜ੍ਞਾ(context);

      vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vvp_get_context_ਵਸਤੂ(context, context_idx_));

      ਜੇ (!bits4->eeq(ਦੋਆਧਾਰੀ_ਅੰਕ)) {
            *bits4 = ਦੋਆਧਾਰੀ_ਅੰਕ;
            ptr.ptr()->send_vec4(*bits4, context);
      }
}

ਖਾਲੀ vvp_fun_signal4_aa::recv_vec4_pv(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
				      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t context)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ptr.ਪੋਰਟ() == 0);
      ਪ੍ਰਤਿਜ੍ਞਾ(ਮਾਪ_2 == vwid);
      ਪ੍ਰਤਿਜ੍ਞਾ(context);

      vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vvp_get_context_ਵਸਤੂ(context, context_idx_));

      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = ਦੋਆਧਾਰੀ_ਅੰਕ.size();
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < wid ;  idx += 1) {
            ਜੇ (ਅਧਾਰ+idx >= bits4->size()) ਰੋਕੋ;
            bits4->ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(ਅਧਾਰ+idx, ਦੋਆਧਾਰੀ_ਅੰਕ.value(idx));
      }
      ptr.ptr()->send_vec4(*bits4, context);
}

ਬਿਨਾਨਿਸ਼ਾਨੀ vvp_fun_signal4_aa::ਮੁੱਲ_ਮਾਪ() ਅਚਲ
{
      ਵਾਪਸ ਮਾਪ_2;
}

vvp_bit4_t vvp_fun_signal4_aa::value(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ
{
      ਅਚਲ vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_ਵਸਤੂ(context_idx_));

      ਵਾਪਸ bits4->value(idx);
}

vvp_scalar_t vvp_fun_signal4_aa::scalar_ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ
{
      ਅਚਲ vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_ਵਸਤੂ(context_idx_));

      ਵਾਪਸ vvp_scalar_t(bits4->value(idx), 6, 6);
}

ਖਾਲੀ vvp_fun_signal4_aa::vec4_ਮੁੱਲ(vvp_vector4_t&val) ਅਚਲ
{
      ਅਚਲ vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_ਵਸਤੂ(context_idx_));

      val = *bits4;
}

ਅਚਲ vvp_vector4_t&vvp_fun_signal4_aa::vec4_unfiltered_ਮੁੱਲ() ਅਚਲ
{
      ਅਚਲ vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_ਵਸਤੂ(context_idx_));

      ਵਾਪਸ *bits4;
}

ਖਾਲੀ vvp_fun_signal4_aa::ਸੰਚਾਲਕ ਮਿਟਾਓ(ਖਾਲੀ*)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}

/*
 * Testing for equality, we want a bitwise test instead of an
 * arithmetic test because we want to treat for example -0 different
 * from +0.
 */
ਤਾਰਕਿਕ ਬਿੱਟ_equal(ਦੁੱਗਣਾ a, ਦੁੱਗਣਾ b)
{
      ਵਾਪਸ memcmp(&a, &b, ਆਕਾਰ a) == 0;
}

vvp_fun_signal_real_sa::vvp_fun_signal_real_sa()
{
      ਬਿੱਟ_2 = 0.0;
}

ਦੁੱਗਣਾ vvp_fun_signal_real_sa::real_unfiltered_ਮੁੱਲ() ਅਚਲ
{
      ਵਾਪਸ ਬਿੱਟ_2;
}

ਖਾਲੀ vvp_fun_signal_real_sa::recv_real(vvp_net_ptr_t ptr, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                                       vvp_context_t)
{
      ਚੋਣ (ptr.ਪੋਰਟ()) {
	  ਹਾਲਤ 0:
	    ਜੇ (!continuous_ਸੌਂਪੋ_ਸਰਗਰਮ) {
                  ਜੇ (needs_init_ || !ਬਿੱਟ_equal(ਬਿੱਟ_2, ਦੋਆਧਾਰੀ_ਅੰਕ)) {
			ਬਿੱਟ_2 = ਦੋਆਧਾਰੀ_ਅੰਕ;
			needs_init_ = ਝੂਠ;
			ptr.ptr()->send_real(ਦੋਆਧਾਰੀ_ਅੰਕ, 0);
		  }
	    }
	    ਰੋਕੋ;

	  ਹਾਲਤ 1: // Continuous assign value
	    continuous_ਸੌਂਪੋ_ਸਰਗਰਮ = ਸੱਚ;
	    ਬਿੱਟ_2 = ਦੋਆਧਾਰੀ_ਅੰਕ;
	    ptr.ptr()->send_real(ਦੋਆਧਾਰੀ_ਅੰਕ, 0);
	    ਰੋਕੋ;

	  ਮੂਲ:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.ਪੋਰਟ());
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
	    ਰੋਕੋ;
      }
}

vvp_fun_signal_real_aa::vvp_fun_signal_real_aa()
{
      context_idx_ = vpip_ਜੋੜੋ_ਵਸਤੂ_ਮੰਜ਼ਿਲ_context(ਇਹ, vpip_peek_context_scope());
}

vvp_fun_signal_real_aa::~vvp_fun_signal_real_aa()
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}

ਖਾਲੀ vvp_fun_signal_real_aa::alloc_instance(vvp_context_t context)
{
      ਦੁੱਗਣਾ*ਬਿੱਟ = ਨਵਾਂ ਦੁੱਗਣਾ;
      vvp_ਸੈੱਟ_context_ਵਸਤੂ(context, context_idx_, ਬਿੱਟ);

      *ਬਿੱਟ = 0.0;
}

ਖਾਲੀ vvp_fun_signal_real_aa::reset_instance(vvp_context_t context)
{
      ਦੁੱਗਣਾ*ਬਿੱਟ = static_cast<ਦੁੱਗਣਾ*>
            (vvp_get_context_ਵਸਤੂ(context, context_idx_));

      *ਬਿੱਟ = 0.0;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਖਾਲੀ vvp_fun_signal_real_aa::free_instance(vvp_context_t context)
{
      ਦੁੱਗਣਾ*ਬਿੱਟ = static_cast<ਦੁੱਗਣਾ*>
            (vvp_get_context_ਵਸਤੂ(context, context_idx_));
      ਮਿਟਾਓ ਬਿੱਟ;
}
#ਜੇ_ਅੰਤ

ਦੁੱਗਣਾ vvp_fun_signal_real_aa::real_unfiltered_ਮੁੱਲ() ਅਚਲ
{
      ਅਚਲ ਦੁੱਗਣਾ*ਬਿੱਟ = static_cast<ਦੁੱਗਣਾ*>
            (vthread_get_rd_context_ਵਸਤੂ(context_idx_));

      ਵਾਪਸ *ਬਿੱਟ;
}

ਦੁੱਗਣਾ vvp_fun_signal_real_aa::real_ਮੁੱਲ() ਅਚਲ
{
      ਵਾਪਸ real_unfiltered_ਮੁੱਲ();
}

ਖਾਲੀ vvp_fun_signal_real_aa::recv_real(vvp_net_ptr_t ptr, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                                       vvp_context_t context)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ptr.ਪੋਰਟ() == 0);
      ਪ੍ਰਤਿਜ੍ਞਾ(context);

      ਦੁੱਗਣਾ*ਬਿੱਟ = static_cast<ਦੁੱਗਣਾ*>
            (vvp_get_context_ਵਸਤੂ(context, context_idx_));

      ਜੇ (!ਬਿੱਟ_equal(*ਬਿੱਟ,ਦੋਆਧਾਰੀ_ਅੰਕ)) {
            *ਬਿੱਟ = ਦੋਆਧਾਰੀ_ਅੰਕ;
            ptr.ptr()->send_real(ਦੋਆਧਾਰੀ_ਅੰਕ, context);
      }
}

ਬਿਨਾਨਿਸ਼ਾਨੀ vvp_fun_signal_real_aa::ਮੁੱਲ_ਮਾਪ() ਅਚਲ
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
      ਵਾਪਸ 1;
}

vvp_bit4_t vvp_fun_signal_real_aa::value(ਬਿਨਾਨਿਸ਼ਾਨੀ) ਅਚਲ
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
      ਵਾਪਸ BIT4_X;
}

vvp_scalar_t vvp_fun_signal_real_aa::scalar_ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ) ਅਚਲ
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
      ਵਾਪਸ vvp_scalar_t();
}

ਖਾਲੀ vvp_fun_signal_real_aa::vec4_ਮੁੱਲ(vvp_vector4_t&) ਅਚਲ
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}

ਖਾਲੀ* vvp_fun_signal_real_aa::ਸੰਚਾਲਕ ਨਵਾਂ(std::size_t size)
{
      ਵਾਪਸ vvp_net_fun_t::ਢੇਰ.alloc(size);
}

ਖਾਲੀ vvp_fun_signal_real_aa::ਸੰਚਾਲਕ ਮਿਟਾਓ(ਖਾਲੀ*)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}


vvp_fun_signal_string_sa::vvp_fun_signal_string_sa()
{
}

ਖਾਲੀ vvp_fun_signal_string_sa::recv_string(vvp_net_ptr_t ptr, ਅਚਲ std::string&ਦੋਆਧਾਰੀ_ਅੰਕ,
					   vvp_context_t)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ptr.ਪੋਰਟ() == 0);

      ਜੇ (needs_init_ || ਮੁੱਲ_2 != ਦੋਆਧਾਰੀ_ਅੰਕ) {
	    ਮੁੱਲ_2 = ਦੋਆਧਾਰੀ_ਅੰਕ;
	    needs_init_ = ਝੂਠ;

	    ptr.ptr()->send_string(ਦੋਆਧਾਰੀ_ਅੰਕ, 0);
      }
}

ਅਚਲ string& vvp_fun_signal_string_sa::get_string() ਅਚਲ
{
      ਵਾਪਸ ਮੁੱਲ_2;
}

vvp_fun_signal_string_aa::vvp_fun_signal_string_aa()
{
      context_idx_ = vpip_ਜੋੜੋ_ਵਸਤੂ_ਮੰਜ਼ਿਲ_context(ਇਹ, vpip_peek_context_scope());
}

vvp_fun_signal_string_aa::~vvp_fun_signal_string_aa()
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}

ਖਾਲੀ vvp_fun_signal_string_aa::alloc_instance(vvp_context_t context)
{
      string*ਬਿੱਟ = ਨਵਾਂ std::string;
      vvp_ਸੈੱਟ_context_ਵਸਤੂ(context, context_idx_, ਬਿੱਟ);
      *ਬਿੱਟ = "";
}

ਖਾਲੀ vvp_fun_signal_string_aa::reset_instance(vvp_context_t context)
{
      string*ਬਿੱਟ = static_cast<std::string*>
	    (vvp_get_context_ਵਸਤੂ(context, context_idx_));
      *ਬਿੱਟ = "";
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਖਾਲੀ vvp_fun_signal_string_aa::free_instance(vvp_context_t context)
{
      string*ਬਿੱਟ = static_cast<std::string*>
            (vvp_get_context_ਵਸਤੂ(context, context_idx_));
      ਮਿਟਾਓ ਬਿੱਟ;
}
#ਜੇ_ਅੰਤ

ਖਾਲੀ vvp_fun_signal_string_aa::recv_string(vvp_net_ptr_t ptr, ਅਚਲ std::string&ਦੋਆਧਾਰੀ_ਅੰਕ, vvp_context_t context)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ptr.ਪੋਰਟ() == 0);
      ਪ੍ਰਤਿਜ੍ਞਾ(context);

      string*ਬਿੱਟ = static_cast<std::string*>
	    (vvp_get_context_ਵਸਤੂ(context, context_idx_));

      ਜੇ (*ਬਿੱਟ != ਦੋਆਧਾਰੀ_ਅੰਕ) {
	    *ਬਿੱਟ = ਦੋਆਧਾਰੀ_ਅੰਕ;
	    ptr.ptr()->send_string(ਦੋਆਧਾਰੀ_ਅੰਕ, context);
      }
}

ਬਿਨਾਨਿਸ਼ਾਨੀ vvp_fun_signal_string_aa::ਮੁੱਲ_ਮਾਪ() ਅਚਲ
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
      ਵਾਪਸ 1;
}

vvp_bit4_t vvp_fun_signal_string_aa::value(ਬਿਨਾਨਿਸ਼ਾਨੀ) ਅਚਲ
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
      ਵਾਪਸ BIT4_X;
}

vvp_scalar_t vvp_fun_signal_string_aa::scalar_ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ) ਅਚਲ
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
      ਵਾਪਸ vvp_scalar_t();
}

ਖਾਲੀ vvp_fun_signal_string_aa::vec4_ਮੁੱਲ(vvp_vector4_t&) ਅਚਲ
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}

ਦੁੱਗਣਾ vvp_fun_signal_string_aa::real_ਮੁੱਲ() ਅਚਲ
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
      ਵਾਪਸ 0.0;
}

ਅਚਲ std::string& vvp_fun_signal_string_aa::get_string() ਅਚਲ
{
      ਅਚਲ string*ਬਿੱਟ = static_cast<std::string*>
            (vthread_get_rd_context_ਵਸਤੂ(context_idx_));

      ਵਾਪਸ *ਬਿੱਟ;
}

ਖਾਲੀ* vvp_fun_signal_string_aa::ਸੰਚਾਲਕ ਨਵਾਂ(std::size_t size)
{
      ਵਾਪਸ vvp_net_fun_t::ਢੇਰ.alloc(size);
}

ਖਾਲੀ vvp_fun_signal_string_aa::ਸੰਚਾਲਕ ਮਿਟਾਓ(ਖਾਲੀ*)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}

  /* OBJECT signals */

vvp_fun_signal_object_sa::vvp_fun_signal_object_sa(ਬਿਨਾਨਿਸ਼ਾਨੀ size)
: vvp_fun_signal_object(size)
{
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਖਾਲੀ vvp_fun_signal_object_aa::free_instance(vvp_context_t context)
{
      vvp_object_t*ਬਿੱਟ = static_cast<vvp_object_t*>
            (vvp_get_context_ਵਸਤੂ(context, context_idx_));
      ਮਿਟਾਓ ਬਿੱਟ;
}
#ਜੇ_ਅੰਤ

ਖਾਲੀ vvp_fun_signal_object_sa::recv_object(vvp_net_ptr_t ptr, vvp_object_t ਦੋਆਧਾਰੀ_ਅੰਕ,
					   vvp_context_t)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ptr.ਪੋਰਟ() == 0);

      ਜੇ (needs_init_ || ਮੁੱਲ_2 != ਦੋਆਧਾਰੀ_ਅੰਕ) {
	    ਮੁੱਲ_2 = ਦੋਆਧਾਰੀ_ਅੰਕ;
	    needs_init_ = ਝੂਠ;

	    ptr.ptr()->send_object(ਦੋਆਧਾਰੀ_ਅੰਕ, 0);
      }
}

vvp_object_t vvp_fun_signal_object_sa::get_object() ਅਚਲ
{
      ਵਾਪਸ ਮੁੱਲ_2;
}

vvp_fun_signal_object_aa::vvp_fun_signal_object_aa(ਬਿਨਾਨਿਸ਼ਾਨੀ size)
: vvp_fun_signal_object(size)
{
      context_idx_ = vpip_ਜੋੜੋ_ਵਸਤੂ_ਮੰਜ਼ਿਲ_context(ਇਹ, vpip_peek_context_scope());
}

vvp_fun_signal_object_aa::~vvp_fun_signal_object_aa()
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}

ਖਾਲੀ vvp_fun_signal_object_aa::alloc_instance(vvp_context_t context)
{
      vvp_object_t*ਬਿੱਟ = ਨਵਾਂ vvp_object_t;
      vvp_ਸੈੱਟ_context_ਵਸਤੂ(context, context_idx_, ਬਿੱਟ);
      ਬਿੱਟ->reset();
}

ਖਾਲੀ vvp_fun_signal_object_aa::reset_instance(vvp_context_t context)
{
      vvp_object_t*ਬਿੱਟ = static_cast<vvp_object_t*>
	    (vvp_get_context_ਵਸਤੂ(context, context_idx_));
      ਬਿੱਟ->reset();
}

vvp_object_t vvp_fun_signal_object_aa::get_object() ਅਚਲ
{
      ਅਚਲ vvp_object_t*ਬਿੱਟ = static_cast<vvp_object_t*>
	    (vthread_get_rd_context_ਵਸਤੂ(context_idx_));
      ਵਾਪਸ *ਬਿੱਟ;
}

ਖਾਲੀ vvp_fun_signal_object_aa::recv_object(vvp_net_ptr_t ptr, vvp_object_t ਦੋਆਧਾਰੀ_ਅੰਕ,
					   vvp_context_t context)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ptr.ਪੋਰਟ() == 0);
      ਪ੍ਰਤਿਜ੍ਞਾ(context);

      vvp_object_t*ਬਿੱਟ = static_cast<vvp_object_t*>
	    (vvp_get_context_ਵਸਤੂ(context, context_idx_));

      ਜੇ (*ਬਿੱਟ != ਦੋਆਧਾਰੀ_ਅੰਕ) {
	    *ਬਿੱਟ = ਦੋਆਧਾਰੀ_ਅੰਕ;
	    ptr.ptr()->send_object(ਦੋਆਧਾਰੀ_ਅੰਕ, context);
      }
}

ਬਿਨਾਨਿਸ਼ਾਨੀ vvp_fun_signal_object_aa::ਮੁੱਲ_ਮਾਪ() ਅਚਲ
{
      ਵਾਪਸ 1;
}

vvp_bit4_t vvp_fun_signal_object_aa::value(ਬਿਨਾਨਿਸ਼ਾਨੀ) ਅਚਲ
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
      ਵਾਪਸ BIT4_X;
}

vvp_scalar_t vvp_fun_signal_object_aa::scalar_ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ) ਅਚਲ
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
      ਵਾਪਸ vvp_scalar_t();
}

ਖਾਲੀ vvp_fun_signal_object_aa::vec4_ਮੁੱਲ(vvp_vector4_t&) ਅਚਲ
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}

ਖਾਲੀ* vvp_fun_signal_object_aa::ਸੰਚਾਲਕ ਨਵਾਂ(std::size_t size)
{
      ਵਾਪਸ vvp_net_fun_t::ਢੇਰ.alloc(size);
}

ਖਾਲੀ vvp_fun_signal_object_aa::ਸੰਚਾਲਕ ਮਿਟਾਓ(ਖਾਲੀ*)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}

  /* **** */

vvp_fun_force::vvp_fun_force()
{
}

vvp_fun_force::~vvp_fun_force()
{
}

ਖਾਲੀ vvp_fun_force::recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			      vvp_context_t)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ptr.ਪੋਰਟ() == 0);
      vvp_net_t*net = ptr.ptr();

      vvp_net_t*dst = net->ਪੋਰਟ[3].ptr();
      ਪ੍ਰਤਿਜ੍ਞਾ(dst->fil);

      dst->force_vec4(coerce_ਮੰਜ਼ਿਲ_ਚੌੜਾਈ(ਦੋਆਧਾਰੀ_ਅੰਕ, dst->fil->filter_ਮਾਪ()), vvp_vector2_t(vvp_vector2_t::FILL1, dst->fil->filter_ਮਾਪ()));
}

ਖਾਲੀ vvp_fun_force::recv_real(vvp_net_ptr_t ptr, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ, vvp_context_t)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ptr.ਪੋਰਟ() == 0);
      vvp_net_t*net = ptr.ptr();
      vvp_net_t*dst = net->ਪੋਰਟ[3].ptr();
      dst->force_real(ਦੋਆਧਾਰੀ_ਅੰਕ, vvp_vector2_t(vvp_vector2_t::FILL1, 1));
}

vvp_wire_ਅਧਾਰ::vvp_wire_ਅਧਾਰ()
{
}

vvp_wire_ਅਧਾਰ::~vvp_wire_ਅਧਾਰ()
{
}

vvp_bit4_t vvp_wire_ਅਧਾਰ::driven_ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ) ਅਚਲ
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
      ਵਾਪਸ BIT4_X;
}

ਤਾਰਕਿਕ vvp_wire_ਅਧਾਰ::is_forced(ਬਿਨਾਨਿਸ਼ਾਨੀ) ਅਚਲ
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
      ਵਾਪਸ ਝੂਠ;
}

vvp_wire_vec4::vvp_wire_vec4(ਬਿਨਾਨਿਸ਼ਾਨੀ wid, vvp_bit4_t init)
: bits4_(wid, init)
{
      needs_init_ = ਸੱਚ;
}

vvp_net_fil_t::prop_t vvp_wire_vec4::filter_vec4(ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ, vvp_vector4_t&rep,
						 ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid)
{
	// Special case! the input bit is 0 wid. Interpret this as a
	// vector of BIT4_X to match the width of the bits4_ vector.
	// FIXME! This is a hack to work around some buggy gate
	// implementations! This should be removed!
      ਜੇ (ਅਧਾਰ==0 && vwid==0) {
	    vvp_vector4_t tmp (bits4_.size(), BIT4_X);
	    ਜੇ (bits4_ .eeq(tmp) && !needs_init_) ਵਾਪਸ STOP;
	    bits4_ = tmp;
	    needs_init_ = ਝੂਠ;
	    ਵਾਪਸ filter_ਨਕਾਬ(tmp, force4_, rep, 0);
      }

      ਜੇ (vwid != bits4_.size()) {
	    cerr << "Internal error: Input vector expected width="
		 << bits4_.size() << ", got "
		 << "bit=" << ਦੋਆਧਾਰੀ_ਅੰਕ << ", base=" << ਅਧਾਰ << ", vwid=" << vwid
		 << ਸਤਰ_ਅੰਤ;
      }
      ਪ੍ਰਤਿਜ੍ਞਾ(bits4_.size() == vwid);

	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      ਜੇ (ਅਧਾਰ==0 && ਦੋਆਧਾਰੀ_ਅੰਕ.size()==vwid) {
	    ਜੇ (bits4_ .eeq( ਦੋਆਧਾਰੀ_ਅੰਕ ) && !needs_init_) ਵਾਪਸ STOP;
	    bits4_ = ਦੋਆਧਾਰੀ_ਅੰਕ;
      } ਨਹੀਂਤਾਂ {
	    ਤਾਰਕਿਕ rc = bits4_.ਸੈੱਟ_vec(ਅਧਾਰ, ਦੋਆਧਾਰੀ_ਅੰਕ);
	    ਜੇ (rc == ਝੂਠ && !needs_init_) ਵਾਪਸ STOP;
      }

      needs_init_ = ਝੂਠ;
      ਵਾਪਸ filter_ਨਕਾਬ(ਦੋਆਧਾਰੀ_ਅੰਕ, force4_, rep, ਅਧਾਰ);
}

vvp_net_fil_t::prop_t vvp_wire_vec4::filter_vec8(ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                                                 vvp_vector8_t&rep,
                                                 ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ,
                                                 ਬਿਨਾਨਿਸ਼ਾਨੀ vwid)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(bits4_.size() == vwid);

	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      vvp_vector4_t bit4 (reduce4(ਦੋਆਧਾਰੀ_ਅੰਕ));
      ਜੇ (ਅਧਾਰ==0 && bit4.size()==vwid) {
	    ਜੇ (bits4_ .eeq( bit4 ) && !needs_init_) ਵਾਪਸ STOP;
	    bits4_ = bit4;
      } ਨਹੀਂਤਾਂ {
	    ਤਾਰਕਿਕ rc = bits4_.ਸੈੱਟ_vec(ਅਧਾਰ, bit4);
	    ਜੇ (rc == ਝੂਠ && !needs_init_) ਵਾਪਸ STOP;
      }

      needs_init_ = ਝੂਠ;
      ਵਾਪਸ filter_ਨਕਾਬ(ਦੋਆਧਾਰੀ_ਅੰਕ, vvp_vector8_t(force4_,6,6), rep, ਅਧਾਰ);
}

ਬਿਨਾਨਿਸ਼ਾਨੀ vvp_wire_vec4::filter_ਮਾਪ() ਅਚਲ
{
      ਵਾਪਸ bits4_.size();
}

ਖਾਲੀ vvp_wire_vec4::force_fil_vec4(ਅਚਲ vvp_vector4_t&val, ਅਚਲ vvp_vector2_t&ਨਕਾਬ)
{
      force_ਨਕਾਬ(ਨਕਾਬ);

      ਜੇ (force4_.size() == 0) {
	    force4_ = val;
      } ਨਹੀਂਤਾਂ {
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0; idx < ਨਕਾਬ.size() ; idx += 1) {
		  ਜੇ (ਨਕਾਬ.value(idx) == 0)
			ਜਾਰੀ;

		  force4_.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, val.value(idx));
	    }
      }
      ਲੜੀਵਾਰ_vpi_callbacks();
}

ਖਾਲੀ vvp_wire_vec4::force_fil_vec8(ਅਚਲ vvp_vector8_t&, ਅਚਲ vvp_vector2_t&)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}

ਖਾਲੀ vvp_wire_vec4::force_fil_real(ਦੁੱਗਣਾ, ਅਚਲ vvp_vector2_t&)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}

ਖਾਲੀ vvp_wire_vec4::release(vvp_net_ptr_t ptr, ਤਾਰਕਿਕ net_flag)
{
      vvp_vector2_t ਨਕਾਬ (vvp_vector2_t::FILL1, bits4_.size());
      ਜੇ (net_flag) {
	      // Wires revert to their unforced value after release.
            release_ਨਕਾਬ(ਨਕਾਬ);
	    needs_init_ = ! force4_ .eeq(bits4_);
	    ptr.ptr()->send_vec4(bits4_, 0);
	    ਲੜੀਵਾਰ_vpi_callbacks();
      } ਨਹੀਂਤਾਂ {
	      // Variables keep the current value.
	    vvp_vector4_t res (bits4_.size());
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx=0; idx<bits4_.size(); idx += 1)
		  res.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx,value(idx));
            release_ਨਕਾਬ(ਨਕਾਬ);
	    ptr.ptr()->fun->recv_vec4(ptr, res, 0);
      }
}

ਖਾਲੀ vvp_wire_vec4::release_pv(vvp_net_ptr_t ptr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ wid, ਤਾਰਕਿਕ net_flag)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(bits4_.size() >= ਅਧਾਰ + wid);

      vvp_vector2_t ਨਕਾਬ (vvp_vector2_t::FILL0, bits4_.size());
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < wid ; idx += 1)
	    ਨਕਾਬ.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(ਅਧਾਰ+idx, 1);

      ਜੇ (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_ਨਕਾਬ(ਨਕਾਬ);
	    needs_init_ = ! force4_.subvalue(ਅਧਾਰ,wid) .eeq(bits4_.subvalue(ਅਧਾਰ,wid));
	    ptr.ptr()->send_vec4_pv(bits4_.subvalue(ਅਧਾਰ,wid),
				    ਅਧਾਰ, bits4_.size(), 0);
	    ਲੜੀਵਾਰ_vpi_callbacks();
      } ਨਹੀਂਤਾਂ {
	      // Variables keep the current value.
	    vvp_vector4_t res (wid);
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx=0; idx<wid; idx += 1)
		  res.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx,value(ਅਧਾਰ+idx));
	    release_ਨਕਾਬ(ਨਕਾਬ);
	    ptr.ptr()->fun->recv_vec4_pv(ptr, res, ਅਧਾਰ, bits4_.size(), 0);
      }
}

ਬਿਨਾਨਿਸ਼ਾਨੀ vvp_wire_vec4::ਮੁੱਲ_ਮਾਪ() ਅਚਲ
{
      ਵਾਪਸ bits4_.size();
}

vvp_bit4_t vvp_wire_vec4::filtered_ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ
{
      ਜੇ (test_force_ਨਕਾਬ(idx))
	    ਵਾਪਸ force4_.value(idx);
      ਨਹੀਂਤਾਂ
	    ਵਾਪਸ bits4_.value(idx);
}

vvp_bit4_t vvp_wire_vec4::value(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ
{
      ਵਾਪਸ filtered_ਮੁੱਲ(idx);
}

vvp_scalar_t vvp_wire_vec4::scalar_ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ
{
      ਵਾਪਸ vvp_scalar_t(value(idx),6,6);
}

ਖਾਲੀ vvp_wire_vec4::vec4_ਮੁੱਲ(vvp_vector4_t&val) ਅਚਲ
{
      val = bits4_;
      ਜੇ (test_force_ਨਕਾਬ_is_zero())
	    ਵਾਪਸ;

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < bits4_.size() ; idx += 1)
	    val.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, filtered_ਮੁੱਲ(idx));
}

vvp_bit4_t vvp_wire_vec4::driven_ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ
{
      ਵਾਪਸ bits4_.value(idx);
}

ਤਾਰਕਿਕ vvp_wire_vec4::is_forced(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ
{
      ਵਾਪਸ test_force_ਨਕਾਬ(idx);
}

vvp_wire_vec8::vvp_wire_vec8(ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
: bits8_(wid)
{
      needs_init_ = ਸੱਚ;
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_vec4(ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                                                 vvp_vector4_t&rep,
                                                 ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ,
                                                 ਬਿਨਾਨਿਸ਼ਾਨੀ vwid)
{
	// For now there is no support for a non-zero base.
      ਪ੍ਰਤਿਜ੍ਞਾ(0 == ਅਧਾਰ);
      ਪ੍ਰਤਿਜ੍ਞਾ(bits8_.size() == vwid);
      ਪ੍ਰਤਿਜ੍ਞਾ(bits8_.size() == ਦੋਆਧਾਰੀ_ਅੰਕ.size());
	// QUESTION: Is it really correct to propagate a vec4 if this
	// is a vec8 node? In fact, it is really possible for a vec4
	// value to get through to a vec8 filter?
      vvp_vector8_t rep8;
      prop_t rc = filter_vec8(vvp_vector8_t(ਦੋਆਧਾਰੀ_ਅੰਕ,6,6), rep8, 0, vwid);
      ਜੇ (rc == REPL)
	    rep = reduce4(rep8);

      needs_init_ = ਝੂਠ;
      ਵਾਪਸ rc;
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_vec8(ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ, vvp_vector8_t&rep, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(vwid == bits8_.size());
	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      ਜੇ (ਅਧਾਰ==0 && ਦੋਆਧਾਰੀ_ਅੰਕ.size()==vwid) {
	    ਜੇ (bits8_.eeq(ਦੋਆਧਾਰੀ_ਅੰਕ) && !needs_init_) ਵਾਪਸ STOP;
	    bits8_ = ਦੋਆਧਾਰੀ_ਅੰਕ;
      } ਨਹੀਂਤਾਂ {
	    ਜੇ (bits8_.subvalue(ਅਧਾਰ, ਦੋਆਧਾਰੀ_ਅੰਕ.size()).eeq(ਦੋਆਧਾਰੀ_ਅੰਕ) &&
		!needs_init_) {
		  ਵਾਪਸ STOP;
	    }
	    ਜੇ (bits8_.size() == 0)
		  bits8_ = vvp_vector8_t(vwid);
	    ਪ੍ਰਤਿਜ੍ਞਾ(bits8_.size() == vwid);
	    bits8_.ਸੈੱਟ_vec(ਅਧਾਰ, ਦੋਆਧਾਰੀ_ਅੰਕ);
      }
      needs_init_ = ਝੂਠ;
      ਵਾਪਸ filter_ਨਕਾਬ(ਦੋਆਧਾਰੀ_ਅੰਕ, force8_, rep, ਅਧਾਰ);
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_ਨਿਵੇਸ਼_vec8(ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ, vvp_vector8_t&rep) ਅਚਲ
{
      ਵਾਪਸ filter_ਨਿਵੇਸ਼_ਨਕਾਬ(ਦੋਆਧਾਰੀ_ਅੰਕ, force8_, rep);
}

ਬਿਨਾਨਿਸ਼ਾਨੀ vvp_wire_vec8::filter_ਮਾਪ() ਅਚਲ
{
      ਵਾਪਸ bits8_.size();
}

ਖਾਲੀ vvp_wire_vec8::force_fil_vec4(ਅਚਲ vvp_vector4_t&val, ਅਚਲ vvp_vector2_t&ਨਕਾਬ)
{
      force_fil_vec8(vvp_vector8_t(val,6,6), ਨਕਾਬ);
}

ਖਾਲੀ vvp_wire_vec8::force_fil_vec8(ਅਚਲ vvp_vector8_t&val, ਅਚਲ vvp_vector2_t&ਨਕਾਬ)
{
      force_ਨਕਾਬ(ਨਕਾਬ);

      ਜੇ (force8_.size() == 0) {
	    force8_ = val;
      } ਨਹੀਂਤਾਂ {
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0; idx < ਨਕਾਬ.size() ; idx += 1) {
		  ਜੇ (ਨਕਾਬ.value(idx) == 0)
			ਜਾਰੀ;

		  force8_.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, val.value(idx));
	    }
      }
      ਲੜੀਵਾਰ_vpi_callbacks();
}

ਖਾਲੀ vvp_wire_vec8::force_fil_real(ਦੁੱਗਣਾ, ਅਚਲ vvp_vector2_t&)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}

ਖਾਲੀ vvp_wire_vec8::release(vvp_net_ptr_t ptr, ਤਾਰਕਿਕ net_flag)
{
	// Wires revert to their unforced value after release.
      vvp_vector2_t ਨਕਾਬ (vvp_vector2_t::FILL1, bits8_.size());
      release_ਨਕਾਬ(ਨਕਾਬ);
      ਜੇ (net_flag) {
	    needs_init_ = !force8_ .eeq(bits8_);
	    ptr.ptr()->send_vec8(bits8_);
      } ਨਹੀਂਤਾਂ {
	// Variable do not know about strength so this should not be able
	// to happen. If for some reason it can then it should not be too
	// hard to fix this code like was done for vvp_wire_vec4 above.
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
//	    ptr.ptr()->fun->recv_vec8(ptr, force8_);
      }
}

ਖਾਲੀ vvp_wire_vec8::release_pv(vvp_net_ptr_t ptr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ wid, ਤਾਰਕਿਕ net_flag)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(bits8_.size() >= ਅਧਾਰ + wid);

      vvp_vector2_t ਨਕਾਬ (vvp_vector2_t::FILL0, bits8_.size());
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < wid ; idx += 1)
	    ਨਕਾਬ.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(ਅਧਾਰ+idx, 1);

      release_ਨਕਾਬ(ਨਕਾਬ);

      ਜੇ (net_flag) {
	    needs_init_ = !force8_.subvalue(ਅਧਾਰ,wid) .eeq((bits8_.subvalue(ਅਧਾਰ,wid)));
	    ptr.ptr()->send_vec8_pv(bits8_.subvalue(ਅਧਾਰ,wid),
				    ਅਧਾਰ, bits8_.size());
	    ਲੜੀਵਾਰ_vpi_callbacks();
      } ਨਹੀਂਤਾਂ {
	// Variable do not know about strength so this should not be able
	// to happen. If for some reason it can then it should not be too
	// hard to fix this code like was done for vvp_wire_vec4 above.
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
//	    ptr.ptr()->fun->recv_vec8_pv(ptr, force8_.subvalue(base,wid),
//					 base, force8_.size());
      }
}

ਬਿਨਾਨਿਸ਼ਾਨੀ vvp_wire_vec8::ਮੁੱਲ_ਮਾਪ() ਅਚਲ
{
      ਵਾਪਸ bits8_.size();
}

vvp_scalar_t vvp_wire_vec8::filtered_ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ
{
      ਜੇ (test_force_ਨਕਾਬ(idx))
	    ਵਾਪਸ force8_.value(idx);
      ਨਹੀਂਤਾਂ
	    ਵਾਪਸ bits8_.value(idx);
}

vvp_bit4_t vvp_wire_vec8::value(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ
{
      ਵਾਪਸ filtered_ਮੁੱਲ(idx).value();
}

vvp_scalar_t vvp_wire_vec8::scalar_ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ
{
      ਵਾਪਸ filtered_ਮੁੱਲ(idx);
}

vvp_vector8_t vvp_wire_vec8::vec8_ਮੁੱਲ() ਅਚਲ
{
      vvp_vector8_t tmp = bits8_;
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < bits8_.size() ; idx += 1)
	    tmp.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, filtered_ਮੁੱਲ(idx));
      ਵਾਪਸ tmp;
}

ਖਾਲੀ vvp_wire_vec8::vec4_ਮੁੱਲ(vvp_vector4_t&val) ਅਚਲ
{
      val = reduce4(vec8_ਮੁੱਲ());
}

vvp_bit4_t vvp_wire_vec8::driven_ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ
{
      ਵਾਪਸ bits8_.value(idx).value();
}

ਤਾਰਕਿਕ vvp_wire_vec8::is_forced(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ
{
      ਵਾਪਸ test_force_ਨਕਾਬ(idx);
}

vvp_wire_real::vvp_wire_real()
: ਦੋਆਧਾਰੀ_ਅੰਕ_2(0.0), force_(0.0)
{
}

vvp_net_fil_t::prop_t vvp_wire_real::filter_real(ਦੁੱਗਣਾ&ਦੋਆਧਾਰੀ_ਅੰਕ)
{
      ਜੇ (ਬਿੱਟ_equal(ਦੋਆਧਾਰੀ_ਅੰਕ_2, ਦੋਆਧਾਰੀ_ਅੰਕ) && !needs_init_) ਵਾਪਸ STOP;
      ਦੋਆਧਾਰੀ_ਅੰਕ_2 = ਦੋਆਧਾਰੀ_ਅੰਕ;
      needs_init_ = ਝੂਠ;
      ਵਾਪਸ filter_ਨਕਾਬ(ਦੋਆਧਾਰੀ_ਅੰਕ, force_);
}

ਬਿਨਾਨਿਸ਼ਾਨੀ vvp_wire_real::filter_ਮਾਪ() ਅਚਲ
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
      ਵਾਪਸ 0;
}

ਖਾਲੀ vvp_wire_real::force_fil_vec4(ਅਚਲ vvp_vector4_t&, ਅਚਲ vvp_vector2_t&)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}

ਖਾਲੀ vvp_wire_real::force_fil_vec8(ਅਚਲ vvp_vector8_t&, ਅਚਲ vvp_vector2_t&)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}

ਖਾਲੀ vvp_wire_real::force_fil_real(ਦੁੱਗਣਾ val, ਅਚਲ vvp_vector2_t&ਨਕਾਬ)
{
      force_ਨਕਾਬ(ਨਕਾਬ);
      ਜੇ (ਨਕਾਬ.value(0))
	    force_ = val;

      ਲੜੀਵਾਰ_vpi_callbacks();
}

ਖਾਲੀ vvp_wire_real::release(vvp_net_ptr_t ptr, ਤਾਰਕਿਕ net_flag)
{
      vvp_vector2_t ਨਕਾਬ (vvp_vector2_t::FILL1, 1);
      ਜੇ (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_ਨਕਾਬ(ਨਕਾਬ);
	    needs_init_ = !ਬਿੱਟ_equal(force_, ਦੋਆਧਾਰੀ_ਅੰਕ_2);
	    ptr.ptr()->send_real(ਦੋਆਧਾਰੀ_ਅੰਕ_2, 0);
      } ਨਹੀਂਤਾਂ {
	      // Variables keep the current value.
	    ਦੁੱਗਣਾ res =  real_ਮੁੱਲ();
	    release_ਨਕਾਬ(ਨਕਾਬ);
	    ptr.ptr()->fun->recv_real(ptr, res, 0);
      }
}

ਖਾਲੀ vvp_wire_real::release_pv(vvp_net_ptr_t, ਬਿਨਾਨਿਸ਼ਾਨੀ, ਬਿਨਾਨਿਸ਼ਾਨੀ, ਤਾਰਕਿਕ)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
#ਜੇ 0
	// A real is a single value. If for some reason this part release
	// can happen the following code should work correctly (requires
	// a base of 0 and a width of 1).
      vvp_vector2_t ਨਕਾਬ (vvp_vector2_t::FILL1, 1);
      ਪ੍ਰਤਿਜ੍ਞਾ(ਅਧਾਰ == 0 && wid == 1);

      ਜੇ (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_ਨਕਾਬ(ਨਕਾਬ);
	    ptr.ptr()->send_real(ਦੋਆਧਾਰੀ_ਅੰਕ_2, 0);
      } ਨਹੀਂਤਾਂ {
	      // Variables keep the current value.
	    ਦੁੱਗਣਾ res =  real_ਮੁੱਲ();
	    release_ਨਕਾਬ(ਨਕਾਬ);
	    ptr.ptr()->fun->recv_real(ptr, res, 0);
      }
#ਜੇ_ਅੰਤ
}

ਬਿਨਾਨਿਸ਼ਾਨੀ vvp_wire_real::ਮੁੱਲ_ਮਾਪ() ਅਚਲ
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
      ਵਾਪਸ 1;
}

vvp_bit4_t vvp_wire_real::value(ਬਿਨਾਨਿਸ਼ਾਨੀ) ਅਚਲ
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
      ਵਾਪਸ BIT4_X;
}

vvp_scalar_t vvp_wire_real::scalar_ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ) ਅਚਲ
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
      ਵਾਪਸ vvp_scalar_t();
}

ਖਾਲੀ vvp_wire_real::vec4_ਮੁੱਲ(vvp_vector4_t&) ਅਚਲ
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}

ਦੁੱਗਣਾ vvp_wire_real::real_ਮੁੱਲ() ਅਚਲ
{
      ਜੇ (test_force_ਨਕਾਬ(0))
	    ਵਾਪਸ force_;
      ਨਹੀਂਤਾਂ
	    ਵਾਪਸ ਦੋਆਧਾਰੀ_ਅੰਕ_2;
}

#ਜੇ 0
vvp_wire_string::vvp_wire_string()
{
}

ਬਿਨਾਨਿਸ਼ਾਨੀ vvp_wire_string::filter_ਮਾਪ() ਅਚਲ
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
      ਵਾਪਸ 0;
}

ਖਾਲੀ vvp_wire_string::force_fil_vec4(ਅਚਲ vvp_vector4_t&, ਅਚਲ vvp_vector2_t&)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}
ਖਾਲੀ vvp_wire_string::force_fil_vec8(ਅਚਲ vvp_vector8_t&, ਅਚਲ vvp_vector2_t&)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}
ਖਾਲੀ vvp_wire_string::force_fil_real(ਦੁੱਗਣਾ, ਅਚਲ vvp_vector2_t&)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}

ਖਾਲੀ vvp_wire_string::release(vvp_net_ptr_t ptr, ਤਾਰਕਿਕ net_flag)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}

ਖਾਲੀ vvp_wire_string::release_pv(vvp_net_ptr_t, ਬਿਨਾਨਿਸ਼ਾਨੀ, ਬਿਨਾਨਿਸ਼ਾਨੀ, ਤਾਰਕਿਕ)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}

ਬਿਨਾਨਿਸ਼ਾਨੀ vvp_wire_string::ਮੁੱਲ_ਮਾਪ() ਅਚਲ
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
      ਵਾਪਸ 1;
}

vvp_bit4_t vvp_wire_string::value(ਬਿਨਾਨਿਸ਼ਾਨੀ) ਅਚਲ
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
      ਵਾਪਸ BIT4_X;
}

vvp_scalar_t vvp_wire_string::scalar_ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ) ਅਚਲ
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
      ਵਾਪਸ vvp_scalar_t();
}

ਖਾਲੀ vvp_wire_string::vec4_ਮੁੱਲ(vvp_vector4_t&) ਅਚਲ
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}

ਦੁੱਗਣਾ vvp_wire_string::real_ਮੁੱਲ() ਅਚਲ
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
      ਵਾਪਸ 0.0;
}
#ਜੇ_ਅੰਤ
