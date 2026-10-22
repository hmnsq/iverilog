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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
# include  <valgrind/memcheck.h>
# include  <map>
#ಆದರೆ_ಅಂತ್ಯ

# include  <iostream>

ಬಳಸಿ ನಾಮಾಕಾಶ std;

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
ಮಾದರಿ <ವರ್ಗ T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_ಮುಖವಾಡ(ಸ್ಥಿರಾಂಕ T&val, ಸ್ಥಿರಾಂಕ T&force, T&filter, ಚಿಹ್ನೆರಹಿತ ತಳ)
{
      ಆದರೆ (!test_force_ಮುಖವಾಡ_is_zero()) {
	      // Some bits are being forced. Go through the
	      // force_mask_ and force value to see which bits are
	      // propagated and which are kept from the forced
	      // value. Update the filter with the filtered result and
	      // return REPL to indicate that some bits have changed,
	      // or STOP if no bits change.
	    ತಾರ್ಕಿಕ propagate_flag = force_propagate_;
	    force_propagate_ = ಸುಳ್ಳು;
	    ಪ್ರತಿಜ್ಞಾ(force_ಮುಖವಾಡ_2.size() == force.size());
	    ಪ್ರತಿಜ್ಞಾ((ತಳ+val.size()) <= force_ಮುಖವಾಡ_2.size());

	    filter = val;
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < val.size() ; idx += 1) {
		  ಆದರೆ (force_ಮುಖವಾಡ_2.value(ತಳ+idx))
			filter.ಗಣ_ಬಿಟ್(idx, force.value(ತಳ+idx));
		  ಇಲ್ಲದಿದ್ದರೆ
			propagate_flag = ನಿಜ;
	    }

	    ಆದರೆ (propagate_flag) {
		  ಸತತ_vpi_callbacks();
		  ಹಿಂತಿರುಗಿ REPL;
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  ಹಿಂತಿರುಗಿ STOP;
	    }

      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಸತತ_vpi_callbacks();
	    ಹಿಂತಿರುಗಿ PROP;
      }
}

ಮಾದರಿ <ವರ್ಗ T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_ಮುಖವಾಡ(T&val, T force)
{

      ಆದರೆ (test_force_ಮುಖವಾಡ(0)) {
	    val = force;
	    ಸತತ_vpi_callbacks();
	    ಹಿಂತಿರುಗಿ REPL;
      }
      ಸತತ_vpi_callbacks();
      ಹಿಂತಿರುಗಿ PROP;
}

ಮಾದರಿ <ವರ್ಗ T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_ಒಳಹರಿವು_ಮುಖವಾಡ(ಸ್ಥಿರಾಂಕ T&val, ಸ್ಥಿರಾಂಕ T&force, T&rep) ಸ್ಥಿರಾಂಕ
{
      ಆದರೆ (test_force_ಮುಖವಾಡ_is_zero())
	    ಹಿಂತಿರುಗಿ PROP;

      ಪ್ರತಿಜ್ಞಾ(force_ಮುಖವಾಡ_2.size() == force.size());

      rep = val;
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < val.size() ; idx += 1) {
	    ಆದರೆ (force_ಮುಖವಾಡ_2.value(idx))
		  rep.ಗಣ_ಬಿಟ್(idx, force.value(idx));
      }

      ಹಿಂತಿರುಗಿ REPL;
}

vvp_signal_ಮೌಲ್ಯ::~vvp_signal_ಮೌಲ್ಯ()
{
}

ದ್ವಿಗುಣ vvp_signal_ಮೌಲ್ಯ::real_ಮೌಲ್ಯ() ಸ್ಥಿರಾಂಕ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ vvp_net_t::force_vec4(ಸ್ಥಿರಾಂಕ vvp_vector4_t&val, ಸ್ಥಿರಾಂಕ vvp_vector2_t&ಮುಖವಾಡ)
{
      ಪ್ರತಿಜ್ಞಾ(fil);
      fil->force_fil_vec4(val, ಮುಖವಾಡ);
      fun->force_flag(ಸುಳ್ಳು);
      vvp_send_vec4(ಹೊರಹೋಗು_2, val, 0);
}

ಶೂನ್ಯ vvp_net_t::force_vec8(ಸ್ಥಿರಾಂಕ vvp_vector8_t&val, ಸ್ಥಿರಾಂಕ vvp_vector2_t&ಮುಖವಾಡ)
{
      ಪ್ರತಿಜ್ಞಾ(fil);
      fil->force_fil_vec8(val, ಮುಖವಾಡ);
      fun->force_flag(ಸುಳ್ಳು);
      vvp_send_vec8(ಹೊರಹೋಗು_2, val);
}

ಶೂನ್ಯ vvp_net_t::force_real(ದ್ವಿಗುಣ val, ಸ್ಥಿರಾಂಕ vvp_vector2_t&ಮುಖವಾಡ)
{
      ಪ್ರತಿಜ್ಞಾ(fil);
      fil->force_fil_real(val, ಮುಖವಾಡ);
      fun->force_flag(ಸುಳ್ಳು);
      vvp_send_real(ಹೊರಹೋಗು_2, val, 0);
}

/* **** vvp_fun_signal methods **** */

vvp_fun_signal_ತಳ::vvp_fun_signal_ತಳ()
{
      continuous_ನಿಯೋಜಿಸು_ಸಕ್ರಿಯ = ಸುಳ್ಳು;
      needs_init_ = ನಿಜ;
      cassign_link = 0;
      ಎಣಿಕೆ_functors_sig += 1;
}

vvp_fun_signal4_sa::vvp_fun_signal4_sa(ಚಿಹ್ನೆರಹಿತ wid, vvp_bit4_t init)
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
ಶೂನ್ಯ vvp_fun_signal4_sa::recv_vec4(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                                   vvp_context_t)
{
      ಆಯ್ಕೆ (ptr.ಪೋರ್ಟ್()) {
	  ಸ್ಥಿತಿ 0: // Normal input (feed from net, or set from process)
	      /* If we don't have a continuous assign mask then just
		 copy the bits, otherwise we need to see if there are
		 any holes in the mask so we can set those bits. */
	    ಆದರೆ (ನಿಯೋಜಿಸು_ಮುಖವಾಡ.size() == 0) {
                  ಆದರೆ (needs_init_ || !bits4_.eeq(ಬಿಟ್)) {
			ಪ್ರತಿಜ್ಞಾ(ಬಿಟ್.size() == bits4_.size());
			bits4_ = ಬಿಟ್;
			needs_init_ = ಸುಳ್ಳು;
			ptr.ptr()->send_vec4(bits4_, 0);
		  }
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  ತಾರ್ಕಿಕ ಬದಲಾಯಿತು = ಸುಳ್ಳು;
		  ಪ್ರತಿಜ್ಞಾ(bits4_.size() == ನಿಯೋಜಿಸು_ಮುಖವಾಡ.size());
		  ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < ಬಿಟ್.size() ;  idx += 1) {
			ಆದರೆ (idx >= bits4_.size()) ನಿಲ್ಲಿಸು;
			ಆದರೆ (ನಿಯೋಜಿಸು_ಮುಖವಾಡ.value(idx)) ಮುಂದುವರಿ;
			bits4_.ಗಣ_ಬಿಟ್(idx, ಬಿಟ್.value(idx));
			ಬದಲಾಯಿತು = ನಿಜ;
		  }
		  ಆದರೆ (ಬದಲಾಯಿತು) {
			needs_init_ = ಸುಳ್ಳು;
			ptr.ptr()->send_vec4(bits4_, 0);
		  }
	    }
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ 1: // Continuous assign value
	      // Handle the simple case of the linked source being wider
	      // than this signal. Note we don't yet support the case of
	      // the linked source being narrower than this signal, or
	      // the case of an expression being assigned.
	    bits4_ = coerce_ಗಮ್ಯ_ಅಗಲ(ಬಿಟ್, bits4_.size());
	    ನಿಯೋಜಿಸು_ಮುಖವಾಡ = vvp_vector2_t(vvp_vector2_t::FILL1, bits4_.size());
	    ptr.ptr()->send_vec4(bits4_, 0);
	    ನಿಲ್ಲಿಸು;

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.ಪೋರ್ಟ್());
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ನಿಲ್ಲಿಸು;
      }
}

ಶೂನ್ಯ vvp_fun_signal4_sa::recv_vec8(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector8_t&ಬಿಟ್)
{
      recv_vec4(ptr, reduce4(ಬಿಟ್), 0);
}

ಶೂನ್ಯ vvp_fun_signal4_sa::recv_vec4_pv(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
				      ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t)
{
      ಪ್ರತಿಜ್ಞಾ(bits4_.size() == vwid);
      ಚಿಹ್ನೆರಹಿತ wid = ಬಿಟ್.size();

      ಆಯ್ಕೆ (ptr.ಪೋರ್ಟ್()) {
	  ಸ್ಥಿತಿ 0: // Normal input
	    ಆದರೆ (ನಿಯೋಜಿಸು_ಮುಖವಾಡ.size() == 0) {
                  ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < wid ;  idx += 1) {
			ಆದರೆ (ತಳ+idx >= bits4_.size()) ನಿಲ್ಲಿಸು;
			bits4_.ಗಣ_ಬಿಟ್(ತಳ+idx, ಬಿಟ್.value(idx));
		  }
		  needs_init_ = ಸುಳ್ಳು;
		  ptr.ptr()->send_vec4(bits4_,0);
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  ತಾರ್ಕಿಕ ಬದಲಾಯಿತು = ಸುಳ್ಳು;
		  ಪ್ರತಿಜ್ಞಾ(bits4_.size() == ನಿಯೋಜಿಸು_ಮುಖವಾಡ.size());
		  ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < wid ;  idx += 1) {
			ಆದರೆ (ತಳ+idx >= bits4_.size()) ನಿಲ್ಲಿಸು;
			ಆದರೆ (ನಿಯೋಜಿಸು_ಮುಖವಾಡ.value(ತಳ+idx)) ಮುಂದುವರಿ;
			bits4_.ಗಣ_ಬಿಟ್(ತಳ+idx, ಬಿಟ್.value(idx));
			ಬದಲಾಯಿತು = ನಿಜ;
		  }
		  ಆದರೆ (ಬದಲಾಯಿತು) {
			needs_init_ = ಸುಳ್ಳು;
			ptr.ptr()->send_vec4(bits4_,0);
		  }
	    }
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ 1: // Continuous assign value
	    ಆದರೆ (ನಿಯೋಜಿಸು_ಮುಖವಾಡ.size() == 0)
		  ನಿಯೋಜಿಸು_ಮುಖವಾಡ = vvp_vector2_t(vvp_vector2_t::FILL0, bits4_.size());
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < wid ;  idx += 1) {
		  ಆದರೆ (ತಳ+idx >= bits4_.size())
			ನಿಲ್ಲಿಸು;
		  bits4_.ಗಣ_ಬಿಟ್(ತಳ+idx, ಬಿಟ್.value(idx));
		  ನಿಯೋಜಿಸು_ಮುಖವಾಡ.ಗಣ_ಬಿಟ್(ತಳ+idx, 1);
	    }
	    ptr.ptr()->send_vec4(bits4_,0);
	    ನಿಲ್ಲಿಸು;

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.ಪೋರ್ಟ್());
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ನಿಲ್ಲಿಸು;
      }
}

ಶೂನ್ಯ vvp_fun_signal4_sa::recv_vec8_pv(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector8_t&ಬಿಟ್,
				      ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid)
{
      recv_vec4_pv(ptr, reduce4(ಬಿಟ್), ತಳ, vwid, 0);
}

ಶೂನ್ಯ vvp_fun_signal_ತಳ::deassign()
{
      continuous_ನಿಯೋಜಿಸು_ಸಕ್ರಿಯ = ಸುಳ್ಳು;
      ನಿಯೋಜಿಸು_ಮುಖವಾಡ = vvp_vector2_t();
}

ಶೂನ್ಯ vvp_fun_signal_ತಳ::deassign_pv(ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ wid)
{
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < wid ;  idx += 1) {
	    ನಿಯೋಜಿಸು_ಮುಖವಾಡ.ಗಣ_ಬಿಟ್(ತಳ+idx, 0);
      }

      ಆದರೆ (ನಿಯೋಜಿಸು_ಮುಖವಾಡ.is_zero()) {
	    ನಿಯೋಜಿಸು_ಮುಖವಾಡ = vvp_vector2_t();
      }
}

ಶೂನ್ಯ automatic_signal_ತಳ::release(vvp_net_ptr_t,ತಾರ್ಕಿಕ)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ automatic_signal_ತಳ::release_pv(vvp_net_ptr_t,ಚಿಹ್ನೆರಹಿತ,ಚಿಹ್ನೆರಹಿತ,ತಾರ್ಕಿಕ)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಚಿಹ್ನೆರಹಿತ automatic_signal_ತಳ::filter_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಹಿಂತಿರುಗಿ(0);
}
ಶೂನ್ಯ automatic_signal_ತಳ::force_fil_vec4(ಸ್ಥಿರಾಂಕ vvp_vector4_t&, ಸ್ಥಿರಾಂಕ vvp_vector2_t&)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}
ಶೂನ್ಯ automatic_signal_ತಳ::force_fil_vec8(ಸ್ಥಿರಾಂಕ vvp_vector8_t&, ಸ್ಥಿರಾಂಕ vvp_vector2_t&)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}
ಶೂನ್ಯ automatic_signal_ತಳ::force_fil_real(ದ್ವಿಗುಣ, ಸ್ಥಿರಾಂಕ vvp_vector2_t&)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}
ಶೂನ್ಯ automatic_signal_ತಳ::get_ಮೌಲ್ಯ(ರಚನೆ t_vpi_ಮೌಲ್ಯ*)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಸ್ಥಿರಾಂಕ vvp_vector4_t& vvp_fun_signal4_sa::vec4_unfiltered_ಮೌಲ್ಯ() ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ bits4_;
}

vvp_fun_signal4_aa::vvp_fun_signal4_aa(ಚಿಹ್ನೆರಹಿತ wid, vvp_bit4_t init)
{
      context_idx_ = vpip_ಕೂಡಿಸು_ಅಂಶ_ಗಮ್ಯ_context(ಇದು, vpip_peek_context_scope());
      ಗಾತ್ರ_3 = wid;
      init_ = init;
}

vvp_fun_signal4_aa::~vvp_fun_signal4_aa()
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ vvp_fun_signal4_aa::alloc_instance(vvp_context_t context)
{
      vvp_ಗಣ_context_ಅಂಶ(context, context_idx_, ಹೊಸ vvp_vector4_t(ಗಾತ್ರ_3, init_));
}

ಶೂನ್ಯ vvp_fun_signal4_aa::reset_instance(vvp_context_t context)
{
      vvp_vector4_t*ಬಿಟ್ಟುಗಳು = static_cast<vvp_vector4_t*>
            (vvp_get_context_ಅಂಶ(context, context_idx_));

      ಬಿಟ್ಟುಗಳು->fill_ಬಿಟ್ಟುಗಳು(init_);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಶೂನ್ಯ vvp_fun_signal4_aa::free_instance(vvp_context_t context)
{
      vvp_vector4_t*ಬಿಟ್ಟುಗಳು = static_cast<vvp_vector4_t*>
            (vvp_get_context_ಅಂಶ(context, context_idx_));
      ಅಳಿಸು ಬಿಟ್ಟುಗಳು;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Continuous and forced assignments are not permitted on automatic
 * variables. So we only expect to receive on port 0.
 */
ಶೂನ್ಯ vvp_fun_signal4_aa::recv_vec4(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                                   vvp_context_t context)
{
      ಪ್ರತಿಜ್ಞಾ(ptr.ಪೋರ್ಟ್() == 0);
      ಪ್ರತಿಜ್ಞಾ(context);

      vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vvp_get_context_ಅಂಶ(context, context_idx_));

      ಆದರೆ (!bits4->eeq(ಬಿಟ್)) {
            *bits4 = ಬಿಟ್;
            ptr.ptr()->send_vec4(*bits4, context);
      }
}

ಶೂನ್ಯ vvp_fun_signal4_aa::recv_vec4_pv(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
				      ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t context)
{
      ಪ್ರತಿಜ್ಞಾ(ptr.ಪೋರ್ಟ್() == 0);
      ಪ್ರತಿಜ್ಞಾ(ಗಾತ್ರ_3 == vwid);
      ಪ್ರತಿಜ್ಞಾ(context);

      vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vvp_get_context_ಅಂಶ(context, context_idx_));

      ಚಿಹ್ನೆರಹಿತ wid = ಬಿಟ್.size();
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < wid ;  idx += 1) {
            ಆದರೆ (ತಳ+idx >= bits4->size()) ನಿಲ್ಲಿಸು;
            bits4->ಗಣ_ಬಿಟ್(ತಳ+idx, ಬಿಟ್.value(idx));
      }
      ptr.ptr()->send_vec4(*bits4, context);
}

ಚಿಹ್ನೆರಹಿತ vvp_fun_signal4_aa::ಮೌಲ್ಯ_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ ಗಾತ್ರ_3;
}

vvp_bit4_t vvp_fun_signal4_aa::value(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ
{
      ಸ್ಥಿರಾಂಕ vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_ಅಂಶ(context_idx_));

      ಹಿಂತಿರುಗಿ bits4->value(idx);
}

vvp_scalar_t vvp_fun_signal4_aa::scalar_ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ
{
      ಸ್ಥಿರಾಂಕ vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_ಅಂಶ(context_idx_));

      ಹಿಂತಿರುಗಿ vvp_scalar_t(bits4->value(idx), 6, 6);
}

ಶೂನ್ಯ vvp_fun_signal4_aa::vec4_ಮೌಲ್ಯ(vvp_vector4_t&val) ಸ್ಥಿರಾಂಕ
{
      ಸ್ಥಿರಾಂಕ vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_ಅಂಶ(context_idx_));

      val = *bits4;
}

ಸ್ಥಿರಾಂಕ vvp_vector4_t&vvp_fun_signal4_aa::vec4_unfiltered_ಮೌಲ್ಯ() ಸ್ಥಿರಾಂಕ
{
      ಸ್ಥಿರಾಂಕ vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_ಅಂಶ(context_idx_));

      ಹಿಂತಿರುಗಿ *bits4;
}

ಶೂನ್ಯ vvp_fun_signal4_aa::ಕ್ರಿಯಾಕಾರಕ ಅಳಿಸು(ಶೂನ್ಯ*)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

/*
 * Testing for equality, we want a bitwise test instead of an
 * arithmetic test because we want to treat for example -0 different
 * from +0.
 */
ತಾರ್ಕಿಕ ಬಿಟ್ಟುಗಳು_equal(ದ್ವಿಗುಣ a, ದ್ವಿಗುಣ b)
{
      ಹಿಂತಿರುಗಿ memcmp(&a, &b, ಗಾತ್ರ a) == 0;
}

vvp_fun_signal_real_sa::vvp_fun_signal_real_sa()
{
      ಬಿಟ್ಟುಗಳು_2 = 0.0;
}

ದ್ವಿಗುಣ vvp_fun_signal_real_sa::real_unfiltered_ಮೌಲ್ಯ() ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ ಬಿಟ್ಟುಗಳು_2;
}

ಶೂನ್ಯ vvp_fun_signal_real_sa::recv_real(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ ಬಿಟ್,
                                       vvp_context_t)
{
      ಆಯ್ಕೆ (ptr.ಪೋರ್ಟ್()) {
	  ಸ್ಥಿತಿ 0:
	    ಆದರೆ (!continuous_ನಿಯೋಜಿಸು_ಸಕ್ರಿಯ) {
                  ಆದರೆ (needs_init_ || !ಬಿಟ್ಟುಗಳು_equal(ಬಿಟ್ಟುಗಳು_2, ಬಿಟ್)) {
			ಬಿಟ್ಟುಗಳು_2 = ಬಿಟ್;
			needs_init_ = ಸುಳ್ಳು;
			ptr.ptr()->send_real(ಬಿಟ್, 0);
		  }
	    }
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ 1: // Continuous assign value
	    continuous_ನಿಯೋಜಿಸು_ಸಕ್ರಿಯ = ನಿಜ;
	    ಬಿಟ್ಟುಗಳು_2 = ಬಿಟ್;
	    ptr.ptr()->send_real(ಬಿಟ್, 0);
	    ನಿಲ್ಲಿಸು;

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.ಪೋರ್ಟ್());
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ನಿಲ್ಲಿಸು;
      }
}

vvp_fun_signal_real_aa::vvp_fun_signal_real_aa()
{
      context_idx_ = vpip_ಕೂಡಿಸು_ಅಂಶ_ಗಮ್ಯ_context(ಇದು, vpip_peek_context_scope());
}

vvp_fun_signal_real_aa::~vvp_fun_signal_real_aa()
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ vvp_fun_signal_real_aa::alloc_instance(vvp_context_t context)
{
      ದ್ವಿಗುಣ*ಬಿಟ್ಟುಗಳು = ಹೊಸ ದ್ವಿಗುಣ;
      vvp_ಗಣ_context_ಅಂಶ(context, context_idx_, ಬಿಟ್ಟುಗಳು);

      *ಬಿಟ್ಟುಗಳು = 0.0;
}

ಶೂನ್ಯ vvp_fun_signal_real_aa::reset_instance(vvp_context_t context)
{
      ದ್ವಿಗುಣ*ಬಿಟ್ಟುಗಳು = static_cast<ದ್ವಿಗುಣ*>
            (vvp_get_context_ಅಂಶ(context, context_idx_));

      *ಬಿಟ್ಟುಗಳು = 0.0;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಶೂನ್ಯ vvp_fun_signal_real_aa::free_instance(vvp_context_t context)
{
      ದ್ವಿಗುಣ*ಬಿಟ್ಟುಗಳು = static_cast<ದ್ವಿಗುಣ*>
            (vvp_get_context_ಅಂಶ(context, context_idx_));
      ಅಳಿಸು ಬಿಟ್ಟುಗಳು;
}
#ಆದರೆ_ಅಂತ್ಯ

ದ್ವಿಗುಣ vvp_fun_signal_real_aa::real_unfiltered_ಮೌಲ್ಯ() ಸ್ಥಿರಾಂಕ
{
      ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ*ಬಿಟ್ಟುಗಳು = static_cast<ದ್ವಿಗುಣ*>
            (vthread_get_rd_context_ಅಂಶ(context_idx_));

      ಹಿಂತಿರುಗಿ *ಬಿಟ್ಟುಗಳು;
}

ದ್ವಿಗುಣ vvp_fun_signal_real_aa::real_ಮೌಲ್ಯ() ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ real_unfiltered_ಮೌಲ್ಯ();
}

ಶೂನ್ಯ vvp_fun_signal_real_aa::recv_real(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ ಬಿಟ್,
                                       vvp_context_t context)
{
      ಪ್ರತಿಜ್ಞಾ(ptr.ಪೋರ್ಟ್() == 0);
      ಪ್ರತಿಜ್ಞಾ(context);

      ದ್ವಿಗುಣ*ಬಿಟ್ಟುಗಳು = static_cast<ದ್ವಿಗುಣ*>
            (vvp_get_context_ಅಂಶ(context, context_idx_));

      ಆದರೆ (!ಬಿಟ್ಟುಗಳು_equal(*ಬಿಟ್ಟುಗಳು,ಬಿಟ್)) {
            *ಬಿಟ್ಟುಗಳು = ಬಿಟ್;
            ptr.ptr()->send_real(ಬಿಟ್, context);
      }
}

ಚಿಹ್ನೆರಹಿತ vvp_fun_signal_real_aa::ಮೌಲ್ಯ_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಹಿಂತಿರುಗಿ 1;
}

vvp_bit4_t vvp_fun_signal_real_aa::value(ಚಿಹ್ನೆರಹಿತ) ಸ್ಥಿರಾಂಕ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಹಿಂತಿರುಗಿ BIT4_X;
}

vvp_scalar_t vvp_fun_signal_real_aa::scalar_ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ) ಸ್ಥಿರಾಂಕ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಹಿಂತಿರುಗಿ vvp_scalar_t();
}

ಶೂನ್ಯ vvp_fun_signal_real_aa::vec4_ಮೌಲ್ಯ(vvp_vector4_t&) ಸ್ಥಿರಾಂಕ
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ* vvp_fun_signal_real_aa::ಕ್ರಿಯಾಕಾರಕ ಹೊಸ(std::size_t size)
{
      ಹಿಂತಿರುಗಿ vvp_net_fun_t::ರಾಶಿ.alloc(size);
}

ಶೂನ್ಯ vvp_fun_signal_real_aa::ಕ್ರಿಯಾಕಾರಕ ಅಳಿಸು(ಶೂನ್ಯ*)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}


vvp_fun_signal_string_sa::vvp_fun_signal_string_sa()
{
}

ಶೂನ್ಯ vvp_fun_signal_string_sa::recv_string(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ std::string&ಬಿಟ್,
					   vvp_context_t)
{
      ಪ್ರತಿಜ್ಞಾ(ptr.ಪೋರ್ಟ್() == 0);

      ಆದರೆ (needs_init_ || ಮೌಲ್ಯ_2 != ಬಿಟ್) {
	    ಮೌಲ್ಯ_2 = ಬಿಟ್;
	    needs_init_ = ಸುಳ್ಳು;

	    ptr.ptr()->send_string(ಬಿಟ್, 0);
      }
}

ಸ್ಥಿರಾಂಕ string& vvp_fun_signal_string_sa::get_string() ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ ಮೌಲ್ಯ_2;
}

vvp_fun_signal_string_aa::vvp_fun_signal_string_aa()
{
      context_idx_ = vpip_ಕೂಡಿಸು_ಅಂಶ_ಗಮ್ಯ_context(ಇದು, vpip_peek_context_scope());
}

vvp_fun_signal_string_aa::~vvp_fun_signal_string_aa()
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ vvp_fun_signal_string_aa::alloc_instance(vvp_context_t context)
{
      string*ಬಿಟ್ಟುಗಳು = ಹೊಸ std::string;
      vvp_ಗಣ_context_ಅಂಶ(context, context_idx_, ಬಿಟ್ಟುಗಳು);
      *ಬಿಟ್ಟುಗಳು = "";
}

ಶೂನ್ಯ vvp_fun_signal_string_aa::reset_instance(vvp_context_t context)
{
      string*ಬಿಟ್ಟುಗಳು = static_cast<std::string*>
	    (vvp_get_context_ಅಂಶ(context, context_idx_));
      *ಬಿಟ್ಟುಗಳು = "";
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಶೂನ್ಯ vvp_fun_signal_string_aa::free_instance(vvp_context_t context)
{
      string*ಬಿಟ್ಟುಗಳು = static_cast<std::string*>
            (vvp_get_context_ಅಂಶ(context, context_idx_));
      ಅಳಿಸು ಬಿಟ್ಟುಗಳು;
}
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ vvp_fun_signal_string_aa::recv_string(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ std::string&ಬಿಟ್, vvp_context_t context)
{
      ಪ್ರತಿಜ್ಞಾ(ptr.ಪೋರ್ಟ್() == 0);
      ಪ್ರತಿಜ್ಞಾ(context);

      string*ಬಿಟ್ಟುಗಳು = static_cast<std::string*>
	    (vvp_get_context_ಅಂಶ(context, context_idx_));

      ಆದರೆ (*ಬಿಟ್ಟುಗಳು != ಬಿಟ್) {
	    *ಬಿಟ್ಟುಗಳು = ಬಿಟ್;
	    ptr.ptr()->send_string(ಬಿಟ್, context);
      }
}

ಚಿಹ್ನೆರಹಿತ vvp_fun_signal_string_aa::ಮೌಲ್ಯ_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಹಿಂತಿರುಗಿ 1;
}

vvp_bit4_t vvp_fun_signal_string_aa::value(ಚಿಹ್ನೆರಹಿತ) ಸ್ಥಿರಾಂಕ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಹಿಂತಿರುಗಿ BIT4_X;
}

vvp_scalar_t vvp_fun_signal_string_aa::scalar_ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ) ಸ್ಥಿರಾಂಕ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಹಿಂತಿರುಗಿ vvp_scalar_t();
}

ಶೂನ್ಯ vvp_fun_signal_string_aa::vec4_ಮೌಲ್ಯ(vvp_vector4_t&) ಸ್ಥಿರಾಂಕ
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ದ್ವಿಗುಣ vvp_fun_signal_string_aa::real_ಮೌಲ್ಯ() ಸ್ಥಿರಾಂಕ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಹಿಂತಿರುಗಿ 0.0;
}

ಸ್ಥಿರಾಂಕ std::string& vvp_fun_signal_string_aa::get_string() ಸ್ಥಿರಾಂಕ
{
      ಸ್ಥಿರಾಂಕ string*ಬಿಟ್ಟುಗಳು = static_cast<std::string*>
            (vthread_get_rd_context_ಅಂಶ(context_idx_));

      ಹಿಂತಿರುಗಿ *ಬಿಟ್ಟುಗಳು;
}

ಶೂನ್ಯ* vvp_fun_signal_string_aa::ಕ್ರಿಯಾಕಾರಕ ಹೊಸ(std::size_t size)
{
      ಹಿಂತಿರುಗಿ vvp_net_fun_t::ರಾಶಿ.alloc(size);
}

ಶೂನ್ಯ vvp_fun_signal_string_aa::ಕ್ರಿಯಾಕಾರಕ ಅಳಿಸು(ಶೂನ್ಯ*)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

  /* OBJECT signals */

vvp_fun_signal_object_sa::vvp_fun_signal_object_sa(ಚಿಹ್ನೆರಹಿತ size)
: vvp_fun_signal_object(size)
{
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಶೂನ್ಯ vvp_fun_signal_object_aa::free_instance(vvp_context_t context)
{
      vvp_object_t*ಬಿಟ್ಟುಗಳು = static_cast<vvp_object_t*>
            (vvp_get_context_ಅಂಶ(context, context_idx_));
      ಅಳಿಸು ಬಿಟ್ಟುಗಳು;
}
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ vvp_fun_signal_object_sa::recv_object(vvp_net_ptr_t ptr, vvp_object_t ಬಿಟ್,
					   vvp_context_t)
{
      ಪ್ರತಿಜ್ಞಾ(ptr.ಪೋರ್ಟ್() == 0);

      ಆದರೆ (needs_init_ || ಮೌಲ್ಯ_2 != ಬಿಟ್) {
	    ಮೌಲ್ಯ_2 = ಬಿಟ್;
	    needs_init_ = ಸುಳ್ಳು;

	    ptr.ptr()->send_object(ಬಿಟ್, 0);
      }
}

vvp_object_t vvp_fun_signal_object_sa::get_object() ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ ಮೌಲ್ಯ_2;
}

vvp_fun_signal_object_aa::vvp_fun_signal_object_aa(ಚಿಹ್ನೆರಹಿತ size)
: vvp_fun_signal_object(size)
{
      context_idx_ = vpip_ಕೂಡಿಸು_ಅಂಶ_ಗಮ್ಯ_context(ಇದು, vpip_peek_context_scope());
}

vvp_fun_signal_object_aa::~vvp_fun_signal_object_aa()
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ vvp_fun_signal_object_aa::alloc_instance(vvp_context_t context)
{
      vvp_object_t*ಬಿಟ್ಟುಗಳು = ಹೊಸ vvp_object_t;
      vvp_ಗಣ_context_ಅಂಶ(context, context_idx_, ಬಿಟ್ಟುಗಳು);
      ಬಿಟ್ಟುಗಳು->reset();
}

ಶೂನ್ಯ vvp_fun_signal_object_aa::reset_instance(vvp_context_t context)
{
      vvp_object_t*ಬಿಟ್ಟುಗಳು = static_cast<vvp_object_t*>
	    (vvp_get_context_ಅಂಶ(context, context_idx_));
      ಬಿಟ್ಟುಗಳು->reset();
}

vvp_object_t vvp_fun_signal_object_aa::get_object() ಸ್ಥಿರಾಂಕ
{
      ಸ್ಥಿರಾಂಕ vvp_object_t*ಬಿಟ್ಟುಗಳು = static_cast<vvp_object_t*>
	    (vthread_get_rd_context_ಅಂಶ(context_idx_));
      ಹಿಂತಿರುಗಿ *ಬಿಟ್ಟುಗಳು;
}

ಶೂನ್ಯ vvp_fun_signal_object_aa::recv_object(vvp_net_ptr_t ptr, vvp_object_t ಬಿಟ್,
					   vvp_context_t context)
{
      ಪ್ರತಿಜ್ಞಾ(ptr.ಪೋರ್ಟ್() == 0);
      ಪ್ರತಿಜ್ಞಾ(context);

      vvp_object_t*ಬಿಟ್ಟುಗಳು = static_cast<vvp_object_t*>
	    (vvp_get_context_ಅಂಶ(context, context_idx_));

      ಆದರೆ (*ಬಿಟ್ಟುಗಳು != ಬಿಟ್) {
	    *ಬಿಟ್ಟುಗಳು = ಬಿಟ್;
	    ptr.ptr()->send_object(ಬಿಟ್, context);
      }
}

ಚಿಹ್ನೆರಹಿತ vvp_fun_signal_object_aa::ಮೌಲ್ಯ_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ 1;
}

vvp_bit4_t vvp_fun_signal_object_aa::value(ಚಿಹ್ನೆರಹಿತ) ಸ್ಥಿರಾಂಕ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಹಿಂತಿರುಗಿ BIT4_X;
}

vvp_scalar_t vvp_fun_signal_object_aa::scalar_ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ) ಸ್ಥಿರಾಂಕ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಹಿಂತಿರುಗಿ vvp_scalar_t();
}

ಶೂನ್ಯ vvp_fun_signal_object_aa::vec4_ಮೌಲ್ಯ(vvp_vector4_t&) ಸ್ಥಿರಾಂಕ
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ* vvp_fun_signal_object_aa::ಕ್ರಿಯಾಕಾರಕ ಹೊಸ(std::size_t size)
{
      ಹಿಂತಿರುಗಿ vvp_net_fun_t::ರಾಶಿ.alloc(size);
}

ಶೂನ್ಯ vvp_fun_signal_object_aa::ಕ್ರಿಯಾಕಾರಕ ಅಳಿಸು(ಶೂನ್ಯ*)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

  /* **** */

vvp_fun_force::vvp_fun_force()
{
}

vvp_fun_force::~vvp_fun_force()
{
}

ಶೂನ್ಯ vvp_fun_force::recv_vec4(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
			      vvp_context_t)
{
      ಪ್ರತಿಜ್ಞಾ(ptr.ಪೋರ್ಟ್() == 0);
      vvp_net_t*net = ptr.ptr();

      vvp_net_t*dst = net->ಪೋರ್ಟ್[3].ptr();
      ಪ್ರತಿಜ್ಞಾ(dst->fil);

      dst->force_vec4(coerce_ಗಮ್ಯ_ಅಗಲ(ಬಿಟ್, dst->fil->filter_ಗಾತ್ರ()), vvp_vector2_t(vvp_vector2_t::FILL1, dst->fil->filter_ಗಾತ್ರ()));
}

ಶೂನ್ಯ vvp_fun_force::recv_real(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ ಬಿಟ್, vvp_context_t)
{
      ಪ್ರತಿಜ್ಞಾ(ptr.ಪೋರ್ಟ್() == 0);
      vvp_net_t*net = ptr.ptr();
      vvp_net_t*dst = net->ಪೋರ್ಟ್[3].ptr();
      dst->force_real(ಬಿಟ್, vvp_vector2_t(vvp_vector2_t::FILL1, 1));
}

vvp_wire_ತಳ::vvp_wire_ತಳ()
{
}

vvp_wire_ತಳ::~vvp_wire_ತಳ()
{
}

vvp_bit4_t vvp_wire_ತಳ::driven_ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ) ಸ್ಥಿರಾಂಕ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಹಿಂತಿರುಗಿ BIT4_X;
}

ತಾರ್ಕಿಕ vvp_wire_ತಳ::is_forced(ಚಿಹ್ನೆರಹಿತ) ಸ್ಥಿರಾಂಕ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

vvp_wire_vec4::vvp_wire_vec4(ಚಿಹ್ನೆರಹಿತ wid, vvp_bit4_t init)
: bits4_(wid, init)
{
      needs_init_ = ನಿಜ;
}

vvp_net_fil_t::prop_t vvp_wire_vec4::filter_vec4(ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್, vvp_vector4_t&rep,
						 ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid)
{
	// Special case! the input bit is 0 wid. Interpret this as a
	// vector of BIT4_X to match the width of the bits4_ vector.
	// FIXME! This is a hack to work around some buggy gate
	// implementations! This should be removed!
      ಆದರೆ (ತಳ==0 && vwid==0) {
	    vvp_vector4_t tmp (bits4_.size(), BIT4_X);
	    ಆದರೆ (bits4_ .eeq(tmp) && !needs_init_) ಹಿಂತಿರುಗಿ STOP;
	    bits4_ = tmp;
	    needs_init_ = ಸುಳ್ಳು;
	    ಹಿಂತಿರುಗಿ filter_ಮುಖವಾಡ(tmp, force4_, rep, 0);
      }

      ಆದರೆ (vwid != bits4_.size()) {
	    cerr << "Internal error: Input vector expected width="
		 << bits4_.size() << ", got "
		 << "bit=" << ಬಿಟ್ << ", base=" << ತಳ << ", vwid=" << vwid
		 << ಸಾಲು_ಅಂತ್ಯ;
      }
      ಪ್ರತಿಜ್ಞಾ(bits4_.size() == vwid);

	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      ಆದರೆ (ತಳ==0 && ಬಿಟ್.size()==vwid) {
	    ಆದರೆ (bits4_ .eeq( ಬಿಟ್ ) && !needs_init_) ಹಿಂತಿರುಗಿ STOP;
	    bits4_ = ಬಿಟ್;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ತಾರ್ಕಿಕ rc = bits4_.ಗಣ_vec(ತಳ, ಬಿಟ್);
	    ಆದರೆ (rc == ಸುಳ್ಳು && !needs_init_) ಹಿಂತಿರುಗಿ STOP;
      }

      needs_init_ = ಸುಳ್ಳು;
      ಹಿಂತಿರುಗಿ filter_ಮುಖವಾಡ(ಬಿಟ್, force4_, rep, ತಳ);
}

vvp_net_fil_t::prop_t vvp_wire_vec4::filter_vec8(ಸ್ಥಿರಾಂಕ vvp_vector8_t&ಬಿಟ್,
                                                 vvp_vector8_t&rep,
                                                 ಚಿಹ್ನೆರಹಿತ ತಳ,
                                                 ಚಿಹ್ನೆರಹಿತ vwid)
{
      ಪ್ರತಿಜ್ಞಾ(bits4_.size() == vwid);

	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      vvp_vector4_t bit4 (reduce4(ಬಿಟ್));
      ಆದರೆ (ತಳ==0 && bit4.size()==vwid) {
	    ಆದರೆ (bits4_ .eeq( bit4 ) && !needs_init_) ಹಿಂತಿರುಗಿ STOP;
	    bits4_ = bit4;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ತಾರ್ಕಿಕ rc = bits4_.ಗಣ_vec(ತಳ, bit4);
	    ಆದರೆ (rc == ಸುಳ್ಳು && !needs_init_) ಹಿಂತಿರುಗಿ STOP;
      }

      needs_init_ = ಸುಳ್ಳು;
      ಹಿಂತಿರುಗಿ filter_ಮುಖವಾಡ(ಬಿಟ್, vvp_vector8_t(force4_,6,6), rep, ತಳ);
}

ಚಿಹ್ನೆರಹಿತ vvp_wire_vec4::filter_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ bits4_.size();
}

ಶೂನ್ಯ vvp_wire_vec4::force_fil_vec4(ಸ್ಥಿರಾಂಕ vvp_vector4_t&val, ಸ್ಥಿರಾಂಕ vvp_vector2_t&ಮುಖವಾಡ)
{
      force_ಮುಖವಾಡ(ಮುಖವಾಡ);

      ಆದರೆ (force4_.size() == 0) {
	    force4_ = val;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0; idx < ಮುಖವಾಡ.size() ; idx += 1) {
		  ಆದರೆ (ಮುಖವಾಡ.value(idx) == 0)
			ಮುಂದುವರಿ;

		  force4_.ಗಣ_ಬಿಟ್(idx, val.value(idx));
	    }
      }
      ಸತತ_vpi_callbacks();
}

ಶೂನ್ಯ vvp_wire_vec4::force_fil_vec8(ಸ್ಥಿರಾಂಕ vvp_vector8_t&, ಸ್ಥಿರಾಂಕ vvp_vector2_t&)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ vvp_wire_vec4::force_fil_real(ದ್ವಿಗುಣ, ಸ್ಥಿರಾಂಕ vvp_vector2_t&)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ vvp_wire_vec4::release(vvp_net_ptr_t ptr, ತಾರ್ಕಿಕ net_flag)
{
      vvp_vector2_t ಮುಖವಾಡ (vvp_vector2_t::FILL1, bits4_.size());
      ಆದರೆ (net_flag) {
	      // Wires revert to their unforced value after release.
            release_ಮುಖವಾಡ(ಮುಖವಾಡ);
	    needs_init_ = ! force4_ .eeq(bits4_);
	    ptr.ptr()->send_vec4(bits4_, 0);
	    ಸತತ_vpi_callbacks();
      } ಇಲ್ಲದಿದ್ದರೆ {
	      // Variables keep the current value.
	    vvp_vector4_t res (bits4_.size());
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx=0; idx<bits4_.size(); idx += 1)
		  res.ಗಣ_ಬಿಟ್(idx,value(idx));
            release_ಮುಖವಾಡ(ಮುಖವಾಡ);
	    ptr.ptr()->fun->recv_vec4(ptr, res, 0);
      }
}

ಶೂನ್ಯ vvp_wire_vec4::release_pv(vvp_net_ptr_t ptr, ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ wid, ತಾರ್ಕಿಕ net_flag)
{
      ಪ್ರತಿಜ್ಞಾ(bits4_.size() >= ತಳ + wid);

      vvp_vector2_t ಮುಖವಾಡ (vvp_vector2_t::FILL0, bits4_.size());
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < wid ; idx += 1)
	    ಮುಖವಾಡ.ಗಣ_ಬಿಟ್(ತಳ+idx, 1);

      ಆದರೆ (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_ಮುಖವಾಡ(ಮುಖವಾಡ);
	    needs_init_ = ! force4_.subvalue(ತಳ,wid) .eeq(bits4_.subvalue(ತಳ,wid));
	    ptr.ptr()->send_vec4_pv(bits4_.subvalue(ತಳ,wid),
				    ತಳ, bits4_.size(), 0);
	    ಸತತ_vpi_callbacks();
      } ಇಲ್ಲದಿದ್ದರೆ {
	      // Variables keep the current value.
	    vvp_vector4_t res (wid);
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx=0; idx<wid; idx += 1)
		  res.ಗಣ_ಬಿಟ್(idx,value(ತಳ+idx));
	    release_ಮುಖವಾಡ(ಮುಖವಾಡ);
	    ptr.ptr()->fun->recv_vec4_pv(ptr, res, ತಳ, bits4_.size(), 0);
      }
}

ಚಿಹ್ನೆರಹಿತ vvp_wire_vec4::ಮೌಲ್ಯ_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ bits4_.size();
}

vvp_bit4_t vvp_wire_vec4::filtered_ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ
{
      ಆದರೆ (test_force_ಮುಖವಾಡ(idx))
	    ಹಿಂತಿರುಗಿ force4_.value(idx);
      ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ bits4_.value(idx);
}

vvp_bit4_t vvp_wire_vec4::value(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ filtered_ಮೌಲ್ಯ(idx);
}

vvp_scalar_t vvp_wire_vec4::scalar_ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ vvp_scalar_t(value(idx),6,6);
}

ಶೂನ್ಯ vvp_wire_vec4::vec4_ಮೌಲ್ಯ(vvp_vector4_t&val) ಸ್ಥಿರಾಂಕ
{
      val = bits4_;
      ಆದರೆ (test_force_ಮುಖವಾಡ_is_zero())
	    ಹಿಂತಿರುಗಿ;

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < bits4_.size() ; idx += 1)
	    val.ಗಣ_ಬಿಟ್(idx, filtered_ಮೌಲ್ಯ(idx));
}

vvp_bit4_t vvp_wire_vec4::driven_ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ bits4_.value(idx);
}

ತಾರ್ಕಿಕ vvp_wire_vec4::is_forced(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ test_force_ಮುಖವಾಡ(idx);
}

vvp_wire_vec8::vvp_wire_vec8(ಚಿಹ್ನೆರಹಿತ wid)
: bits8_(wid)
{
      needs_init_ = ನಿಜ;
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_vec4(ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                                                 vvp_vector4_t&rep,
                                                 ಚಿಹ್ನೆರಹಿತ ತಳ,
                                                 ಚಿಹ್ನೆರಹಿತ vwid)
{
	// For now there is no support for a non-zero base.
      ಪ್ರತಿಜ್ಞಾ(0 == ತಳ);
      ಪ್ರತಿಜ್ಞಾ(bits8_.size() == vwid);
      ಪ್ರತಿಜ್ಞಾ(bits8_.size() == ಬಿಟ್.size());
	// QUESTION: Is it really correct to propagate a vec4 if this
	// is a vec8 node? In fact, it is really possible for a vec4
	// value to get through to a vec8 filter?
      vvp_vector8_t rep8;
      prop_t rc = filter_vec8(vvp_vector8_t(ಬಿಟ್,6,6), rep8, 0, vwid);
      ಆದರೆ (rc == REPL)
	    rep = reduce4(rep8);

      needs_init_ = ಸುಳ್ಳು;
      ಹಿಂತಿರುಗಿ rc;
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_vec8(ಸ್ಥಿರಾಂಕ vvp_vector8_t&ಬಿಟ್, vvp_vector8_t&rep, ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid)
{
      ಪ್ರತಿಜ್ಞಾ(vwid == bits8_.size());
	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      ಆದರೆ (ತಳ==0 && ಬಿಟ್.size()==vwid) {
	    ಆದರೆ (bits8_.eeq(ಬಿಟ್) && !needs_init_) ಹಿಂತಿರುಗಿ STOP;
	    bits8_ = ಬಿಟ್;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಆದರೆ (bits8_.subvalue(ತಳ, ಬಿಟ್.size()).eeq(ಬಿಟ್) &&
		!needs_init_) {
		  ಹಿಂತಿರುಗಿ STOP;
	    }
	    ಆದರೆ (bits8_.size() == 0)
		  bits8_ = vvp_vector8_t(vwid);
	    ಪ್ರತಿಜ್ಞಾ(bits8_.size() == vwid);
	    bits8_.ಗಣ_vec(ತಳ, ಬಿಟ್);
      }
      needs_init_ = ಸುಳ್ಳು;
      ಹಿಂತಿರುಗಿ filter_ಮುಖವಾಡ(ಬಿಟ್, force8_, rep, ತಳ);
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_ಒಳಹರಿವು_vec8(ಸ್ಥಿರಾಂಕ vvp_vector8_t&ಬಿಟ್, vvp_vector8_t&rep) ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ filter_ಒಳಹರಿವು_ಮುಖವಾಡ(ಬಿಟ್, force8_, rep);
}

ಚಿಹ್ನೆರಹಿತ vvp_wire_vec8::filter_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ bits8_.size();
}

ಶೂನ್ಯ vvp_wire_vec8::force_fil_vec4(ಸ್ಥಿರಾಂಕ vvp_vector4_t&val, ಸ್ಥಿರಾಂಕ vvp_vector2_t&ಮುಖವಾಡ)
{
      force_fil_vec8(vvp_vector8_t(val,6,6), ಮುಖವಾಡ);
}

ಶೂನ್ಯ vvp_wire_vec8::force_fil_vec8(ಸ್ಥಿರಾಂಕ vvp_vector8_t&val, ಸ್ಥಿರಾಂಕ vvp_vector2_t&ಮುಖವಾಡ)
{
      force_ಮುಖವಾಡ(ಮುಖವಾಡ);

      ಆದರೆ (force8_.size() == 0) {
	    force8_ = val;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0; idx < ಮುಖವಾಡ.size() ; idx += 1) {
		  ಆದರೆ (ಮುಖವಾಡ.value(idx) == 0)
			ಮುಂದುವರಿ;

		  force8_.ಗಣ_ಬಿಟ್(idx, val.value(idx));
	    }
      }
      ಸತತ_vpi_callbacks();
}

ಶೂನ್ಯ vvp_wire_vec8::force_fil_real(ದ್ವಿಗುಣ, ಸ್ಥಿರಾಂಕ vvp_vector2_t&)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ vvp_wire_vec8::release(vvp_net_ptr_t ptr, ತಾರ್ಕಿಕ net_flag)
{
	// Wires revert to their unforced value after release.
      vvp_vector2_t ಮುಖವಾಡ (vvp_vector2_t::FILL1, bits8_.size());
      release_ಮುಖವಾಡ(ಮುಖವಾಡ);
      ಆದರೆ (net_flag) {
	    needs_init_ = !force8_ .eeq(bits8_);
	    ptr.ptr()->send_vec8(bits8_);
      } ಇಲ್ಲದಿದ್ದರೆ {
	// Variable do not know about strength so this should not be able
	// to happen. If for some reason it can then it should not be too
	// hard to fix this code like was done for vvp_wire_vec4 above.
	    ಪ್ರತಿಜ್ಞಾ(0);
//	    ptr.ptr()->fun->recv_vec8(ptr, force8_);
      }
}

ಶೂನ್ಯ vvp_wire_vec8::release_pv(vvp_net_ptr_t ptr, ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ wid, ತಾರ್ಕಿಕ net_flag)
{
      ಪ್ರತಿಜ್ಞಾ(bits8_.size() >= ತಳ + wid);

      vvp_vector2_t ಮುಖವಾಡ (vvp_vector2_t::FILL0, bits8_.size());
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < wid ; idx += 1)
	    ಮುಖವಾಡ.ಗಣ_ಬಿಟ್(ತಳ+idx, 1);

      release_ಮುಖವಾಡ(ಮುಖವಾಡ);

      ಆದರೆ (net_flag) {
	    needs_init_ = !force8_.subvalue(ತಳ,wid) .eeq((bits8_.subvalue(ತಳ,wid)));
	    ptr.ptr()->send_vec8_pv(bits8_.subvalue(ತಳ,wid),
				    ತಳ, bits8_.size());
	    ಸತತ_vpi_callbacks();
      } ಇಲ್ಲದಿದ್ದರೆ {
	// Variable do not know about strength so this should not be able
	// to happen. If for some reason it can then it should not be too
	// hard to fix this code like was done for vvp_wire_vec4 above.
	    ಪ್ರತಿಜ್ಞಾ(0);
//	    ptr.ptr()->fun->recv_vec8_pv(ptr, force8_.subvalue(base,wid),
//					 base, force8_.size());
      }
}

ಚಿಹ್ನೆರಹಿತ vvp_wire_vec8::ಮೌಲ್ಯ_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ bits8_.size();
}

vvp_scalar_t vvp_wire_vec8::filtered_ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ
{
      ಆದರೆ (test_force_ಮುಖವಾಡ(idx))
	    ಹಿಂತಿರುಗಿ force8_.value(idx);
      ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ bits8_.value(idx);
}

vvp_bit4_t vvp_wire_vec8::value(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ filtered_ಮೌಲ್ಯ(idx).value();
}

vvp_scalar_t vvp_wire_vec8::scalar_ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ filtered_ಮೌಲ್ಯ(idx);
}

vvp_vector8_t vvp_wire_vec8::vec8_ಮೌಲ್ಯ() ಸ್ಥಿರಾಂಕ
{
      vvp_vector8_t tmp = bits8_;
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < bits8_.size() ; idx += 1)
	    tmp.ಗಣ_ಬಿಟ್(idx, filtered_ಮೌಲ್ಯ(idx));
      ಹಿಂತಿರುಗಿ tmp;
}

ಶೂನ್ಯ vvp_wire_vec8::vec4_ಮೌಲ್ಯ(vvp_vector4_t&val) ಸ್ಥಿರಾಂಕ
{
      val = reduce4(vec8_ಮೌಲ್ಯ());
}

vvp_bit4_t vvp_wire_vec8::driven_ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ bits8_.value(idx).value();
}

ತಾರ್ಕಿಕ vvp_wire_vec8::is_forced(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ test_force_ಮುಖವಾಡ(idx);
}

vvp_wire_real::vvp_wire_real()
: ಬಿಟ್_2(0.0), force_(0.0)
{
}

vvp_net_fil_t::prop_t vvp_wire_real::filter_real(ದ್ವಿಗುಣ&ಬಿಟ್)
{
      ಆದರೆ (ಬಿಟ್ಟುಗಳು_equal(ಬಿಟ್_2, ಬಿಟ್) && !needs_init_) ಹಿಂತಿರುಗಿ STOP;
      ಬಿಟ್_2 = ಬಿಟ್;
      needs_init_ = ಸುಳ್ಳು;
      ಹಿಂತಿರುಗಿ filter_ಮುಖವಾಡ(ಬಿಟ್, force_);
}

ಚಿಹ್ನೆರಹಿತ vvp_wire_real::filter_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ vvp_wire_real::force_fil_vec4(ಸ್ಥಿರಾಂಕ vvp_vector4_t&, ಸ್ಥಿರಾಂಕ vvp_vector2_t&)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ vvp_wire_real::force_fil_vec8(ಸ್ಥಿರಾಂಕ vvp_vector8_t&, ಸ್ಥಿರಾಂಕ vvp_vector2_t&)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ vvp_wire_real::force_fil_real(ದ್ವಿಗುಣ val, ಸ್ಥಿರಾಂಕ vvp_vector2_t&ಮುಖವಾಡ)
{
      force_ಮುಖವಾಡ(ಮುಖವಾಡ);
      ಆದರೆ (ಮುಖವಾಡ.value(0))
	    force_ = val;

      ಸತತ_vpi_callbacks();
}

ಶೂನ್ಯ vvp_wire_real::release(vvp_net_ptr_t ptr, ತಾರ್ಕಿಕ net_flag)
{
      vvp_vector2_t ಮುಖವಾಡ (vvp_vector2_t::FILL1, 1);
      ಆದರೆ (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_ಮುಖವಾಡ(ಮುಖವಾಡ);
	    needs_init_ = !ಬಿಟ್ಟುಗಳು_equal(force_, ಬಿಟ್_2);
	    ptr.ptr()->send_real(ಬಿಟ್_2, 0);
      } ಇಲ್ಲದಿದ್ದರೆ {
	      // Variables keep the current value.
	    ದ್ವಿಗುಣ res =  real_ಮೌಲ್ಯ();
	    release_ಮುಖವಾಡ(ಮುಖವಾಡ);
	    ptr.ptr()->fun->recv_real(ptr, res, 0);
      }
}

ಶೂನ್ಯ vvp_wire_real::release_pv(vvp_net_ptr_t, ಚಿಹ್ನೆರಹಿತ, ಚಿಹ್ನೆರಹಿತ, ತಾರ್ಕಿಕ)
{
      ಪ್ರತಿಜ್ಞಾ(0);
#ಆದರೆ 0
	// A real is a single value. If for some reason this part release
	// can happen the following code should work correctly (requires
	// a base of 0 and a width of 1).
      vvp_vector2_t ಮುಖವಾಡ (vvp_vector2_t::FILL1, 1);
      ಪ್ರತಿಜ್ಞಾ(ತಳ == 0 && wid == 1);

      ಆದರೆ (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_ಮುಖವಾಡ(ಮುಖವಾಡ);
	    ptr.ptr()->send_real(ಬಿಟ್_2, 0);
      } ಇಲ್ಲದಿದ್ದರೆ {
	      // Variables keep the current value.
	    ದ್ವಿಗುಣ res =  real_ಮೌಲ್ಯ();
	    release_ಮುಖವಾಡ(ಮುಖವಾಡ);
	    ptr.ptr()->fun->recv_real(ptr, res, 0);
      }
#ಆದರೆ_ಅಂತ್ಯ
}

ಚಿಹ್ನೆರಹಿತ vvp_wire_real::ಮೌಲ್ಯ_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಹಿಂತಿರುಗಿ 1;
}

vvp_bit4_t vvp_wire_real::value(ಚಿಹ್ನೆರಹಿತ) ಸ್ಥಿರಾಂಕ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಹಿಂತಿರುಗಿ BIT4_X;
}

vvp_scalar_t vvp_wire_real::scalar_ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ) ಸ್ಥಿರಾಂಕ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಹಿಂತಿರುಗಿ vvp_scalar_t();
}

ಶೂನ್ಯ vvp_wire_real::vec4_ಮೌಲ್ಯ(vvp_vector4_t&) ಸ್ಥಿರಾಂಕ
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ದ್ವಿಗುಣ vvp_wire_real::real_ಮೌಲ್ಯ() ಸ್ಥಿರಾಂಕ
{
      ಆದರೆ (test_force_ಮುಖವಾಡ(0))
	    ಹಿಂತಿರುಗಿ force_;
      ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ ಬಿಟ್_2;
}

#ಆದರೆ 0
vvp_wire_string::vvp_wire_string()
{
}

ಚಿಹ್ನೆರಹಿತ vvp_wire_string::filter_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ vvp_wire_string::force_fil_vec4(ಸ್ಥಿರಾಂಕ vvp_vector4_t&, ಸ್ಥಿರಾಂಕ vvp_vector2_t&)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}
ಶೂನ್ಯ vvp_wire_string::force_fil_vec8(ಸ್ಥಿರಾಂಕ vvp_vector8_t&, ಸ್ಥಿರಾಂಕ vvp_vector2_t&)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}
ಶೂನ್ಯ vvp_wire_string::force_fil_real(ದ್ವಿಗುಣ, ಸ್ಥಿರಾಂಕ vvp_vector2_t&)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ vvp_wire_string::release(vvp_net_ptr_t ptr, ತಾರ್ಕಿಕ net_flag)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ vvp_wire_string::release_pv(vvp_net_ptr_t, ಚಿಹ್ನೆರಹಿತ, ಚಿಹ್ನೆರಹಿತ, ತಾರ್ಕಿಕ)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಚಿಹ್ನೆರಹಿತ vvp_wire_string::ಮೌಲ್ಯ_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಹಿಂತಿರುಗಿ 1;
}

vvp_bit4_t vvp_wire_string::value(ಚಿಹ್ನೆರಹಿತ) ಸ್ಥಿರಾಂಕ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಹಿಂತಿರುಗಿ BIT4_X;
}

vvp_scalar_t vvp_wire_string::scalar_ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ) ಸ್ಥಿರಾಂಕ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಹಿಂತಿರುಗಿ vvp_scalar_t();
}

ಶೂನ್ಯ vvp_wire_string::vec4_ಮೌಲ್ಯ(vvp_vector4_t&) ಸ್ಥಿರಾಂಕ
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ದ್ವಿಗುಣ vvp_wire_string::real_ಮೌಲ್ಯ() ಸ್ಥಿರಾಂಕ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಹಿಂತಿರುಗಿ 0.0;
}
#ಆದರೆ_ಅಂತ್ಯ
