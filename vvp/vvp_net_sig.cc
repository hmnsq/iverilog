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
#ifdef CHECK_WITH_VALGRIND
# include  <valgrind/memcheck.h>
# include  <map>
#endif

# include  <iostream>

using ನಾಮಾಕಾಶ std;

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
template <ವರ್ಗ T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_mask_(ಧ್ರುವ T&val, ಧ್ರುವ T&force, T&filter, unsigned base)
{
      ಯದಿ (!test_force_mask_is_zero()) {
	      // Some bits are being forced. Go through the
	      // force_mask_ and force value to see which bits are
	      // propagated and which are kept from the forced
	      // value. Update the filter with the filtered result and
	      // return REPL to indicate that some bits have changed,
	      // or STOP if no bits change.
	    ತಾರ್ಕಿಕ propagate_flag = force_propagate_;
	    force_propagate_ = ಅಸತ್ಯ;
	    ಪ್ರತಿಜ್ಞಾ(force_mask_.size() == force.size());
	    ಪ್ರತಿಜ್ಞಾ((base+val.size()) <= force_mask_.size());

	    filter = val;
	    ಪ್ರತಿ (unsigned idx = 0 ; idx < val.size() ; idx += 1) {
		  ಯದಿ (force_mask_.value(base+idx))
			filter.set_bit(idx, force.value(base+idx));
		  ಅನ್ಯಥಾ
			propagate_flag = ಸತ್ಯ;
	    }

	    ಯದಿ (propagate_flag) {
		  run_vpi_callbacks();
		  ಪ್ರತ್ಯಾಗಮ REPL;
	    } ಅನ್ಯಥಾ {
		  ಪ್ರತ್ಯಾಗಮ STOP;
	    }

      } ಅನ್ಯಥಾ {
	    run_vpi_callbacks();
	    ಪ್ರತ್ಯಾಗಮ PROP;
      }
}

template <ವರ್ಗ T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_mask_(T&val, T force)
{

      ಯದಿ (test_force_mask(0)) {
	    val = force;
	    run_vpi_callbacks();
	    ಪ್ರತ್ಯಾಗಮ REPL;
      }
      run_vpi_callbacks();
      ಪ್ರತ್ಯಾಗಮ PROP;
}

template <ವರ್ಗ T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_ಒಳಹರಿವು_mask(ಧ್ರುವ T&val, ಧ್ರುವ T&force, T&rep) ಧ್ರುವ
{
      ಯದಿ (test_force_mask_is_zero())
	    ಪ್ರತ್ಯಾಗಮ PROP;

      ಪ್ರತಿಜ್ಞಾ(force_mask_.size() == force.size());

      rep = val;
      ಪ್ರತಿ (unsigned idx = 0 ; idx < val.size() ; idx += 1) {
	    ಯದಿ (force_mask_.value(idx))
		  rep.set_bit(idx, force.value(idx));
      }

      ಪ್ರತ್ಯಾಗಮ REPL;
}

vvp_signal_ಮೌಲ್ಯ::~vvp_signal_ಮೌಲ್ಯ()
{
}

ದ್ವಿಗುಣ vvp_signal_ಮೌಲ್ಯ::real_ಮೌಲ್ಯ() ಧ್ರುವ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ vvp_net_t::force_vec4(ಧ್ರುವ vvp_vector4_t&val, ಧ್ರುವ vvp_vector2_t&mask)
{
      ಪ್ರತಿಜ್ಞಾ(fil);
      fil->force_fil_vec4(val, mask);
      fun->force_flag(ಅಸತ್ಯ);
      vvp_send_vec4(out_, val, 0);
}

ಶೂನ್ಯ vvp_net_t::force_vec8(ಧ್ರುವ vvp_vector8_t&val, ಧ್ರುವ vvp_vector2_t&mask)
{
      ಪ್ರತಿಜ್ಞಾ(fil);
      fil->force_fil_vec8(val, mask);
      fun->force_flag(ಅಸತ್ಯ);
      vvp_send_vec8(out_, val);
}

ಶೂನ್ಯ vvp_net_t::force_real(ದ್ವಿಗುಣ val, ಧ್ರುವ vvp_vector2_t&mask)
{
      ಪ್ರತಿಜ್ಞಾ(fil);
      fil->force_fil_real(val, mask);
      fun->force_flag(ಅಸತ್ಯ);
      vvp_send_real(out_, val, 0);
}

/* **** vvp_fun_signal methods **** */

vvp_fun_signal_base::vvp_fun_signal_base()
{
      continuous_assign_active_ = ಅಸತ್ಯ;
      needs_init_ = ಸತ್ಯ;
      cassign_link = 0;
      ಎಣಿಕೆ_functors_sig += 1;
}

vvp_fun_signal4_sa::vvp_fun_signal4_sa(unsigned wid, vvp_bit4_t init)
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
ಶೂನ್ಯ vvp_fun_signal4_sa::recv_vec4(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit,
                                   vvp_context_t)
{
      ವಿಕಲ್ಪ (ptr.port()) {
	  ಪಕ್ಷ 0: // Normal input (feed from net, or set from process)
	      /* If we don't have a continuous assign mask then just
		 copy the bits, otherwise we need to see if there are
		 any holes in the mask so we can set those bits. */
	    ಯದಿ (assign_mask_.size() == 0) {
                  ಯದಿ (needs_init_ || !bits4_.eeq(bit)) {
			ಪ್ರತಿಜ್ಞಾ(bit.size() == bits4_.size());
			bits4_ = bit;
			needs_init_ = ಅಸತ್ಯ;
			ptr.ptr()->send_vec4(bits4_, 0);
		  }
	    } ಅನ್ಯಥಾ {
		  ತಾರ್ಕಿಕ changed = ಅಸತ್ಯ;
		  ಪ್ರತಿಜ್ಞಾ(bits4_.size() == assign_mask_.size());
		  ಪ್ರತಿ (unsigned idx = 0 ;  idx < bit.size() ;  idx += 1) {
			ಯದಿ (idx >= bits4_.size()) ಭಙ್ಗ;
			ಯದಿ (assign_mask_.value(idx)) ಅನುವರ್ತ;
			bits4_.set_bit(idx, bit.value(idx));
			changed = ಸತ್ಯ;
		  }
		  ಯದಿ (changed) {
			needs_init_ = ಅಸತ್ಯ;
			ptr.ptr()->send_vec4(bits4_, 0);
		  }
	    }
	    ಭಙ್ಗ;

	  ಪಕ್ಷ 1: // Continuous assign value
	      // Handle the simple case of the linked source being wider
	      // than this signal. Note we don't yet support the case of
	      // the linked source being narrower than this signal, or
	      // the case of an expression being assigned.
	    bits4_ = coerce_to_width(bit, bits4_.size());
	    assign_mask_ = vvp_vector2_t(vvp_vector2_t::FILL1, bits4_.size());
	    ptr.ptr()->send_vec4(bits4_, 0);
	    ಭಙ್ಗ;

	  ಸ್ವಭಾವ:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.port());
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಭಙ್ಗ;
      }
}

ಶೂನ್ಯ vvp_fun_signal4_sa::recv_vec8(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector8_t&bit)
{
      recv_vec4(ptr, reduce4(bit), 0);
}

ಶೂನ್ಯ vvp_fun_signal4_sa::recv_vec4_pv(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit,
				      unsigned base, unsigned vwid, vvp_context_t)
{
      ಪ್ರತಿಜ್ಞಾ(bits4_.size() == vwid);
      unsigned wid = bit.size();

      ವಿಕಲ್ಪ (ptr.port()) {
	  ಪಕ್ಷ 0: // Normal input
	    ಯದಿ (assign_mask_.size() == 0) {
                  ಪ್ರತಿ (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
			ಯದಿ (base+idx >= bits4_.size()) ಭಙ್ಗ;
			bits4_.set_bit(base+idx, bit.value(idx));
		  }
		  needs_init_ = ಅಸತ್ಯ;
		  ptr.ptr()->send_vec4(bits4_,0);
	    } ಅನ್ಯಥಾ {
		  ತಾರ್ಕಿಕ changed = ಅಸತ್ಯ;
		  ಪ್ರತಿಜ್ಞಾ(bits4_.size() == assign_mask_.size());
		  ಪ್ರತಿ (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
			ಯದಿ (base+idx >= bits4_.size()) ಭಙ್ಗ;
			ಯದಿ (assign_mask_.value(base+idx)) ಅನುವರ್ತ;
			bits4_.set_bit(base+idx, bit.value(idx));
			changed = ಸತ್ಯ;
		  }
		  ಯದಿ (changed) {
			needs_init_ = ಅಸತ್ಯ;
			ptr.ptr()->send_vec4(bits4_,0);
		  }
	    }
	    ಭಙ್ಗ;

	  ಪಕ್ಷ 1: // Continuous assign value
	    ಯದಿ (assign_mask_.size() == 0)
		  assign_mask_ = vvp_vector2_t(vvp_vector2_t::FILL0, bits4_.size());
	    ಪ್ರತಿ (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		  ಯದಿ (base+idx >= bits4_.size())
			ಭಙ್ಗ;
		  bits4_.set_bit(base+idx, bit.value(idx));
		  assign_mask_.set_bit(base+idx, 1);
	    }
	    ptr.ptr()->send_vec4(bits4_,0);
	    ಭಙ್ಗ;

	  ಸ್ವಭಾವ:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.port());
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಭಙ್ಗ;
      }
}

ಶೂನ್ಯ vvp_fun_signal4_sa::recv_vec8_pv(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector8_t&bit,
				      unsigned base, unsigned vwid)
{
      recv_vec4_pv(ptr, reduce4(bit), base, vwid, 0);
}

ಶೂನ್ಯ vvp_fun_signal_base::deassign()
{
      continuous_assign_active_ = ಅಸತ್ಯ;
      assign_mask_ = vvp_vector2_t();
}

ಶೂನ್ಯ vvp_fun_signal_base::deassign_pv(unsigned base, unsigned wid)
{
      ಪ್ರತಿ (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
	    assign_mask_.set_bit(base+idx, 0);
      }

      ಯದಿ (assign_mask_.is_zero()) {
	    assign_mask_ = vvp_vector2_t();
      }
}

ಶೂನ್ಯ automatic_signal_base::release(vvp_net_ptr_t,ತಾರ್ಕಿಕ)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ automatic_signal_base::release_pv(vvp_net_ptr_t,unsigned,unsigned,ತಾರ್ಕಿಕ)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

unsigned automatic_signal_base::filter_size() ಧ್ರುವ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಪ್ರತ್ಯಾಗಮ(0);
}
ಶೂನ್ಯ automatic_signal_base::force_fil_vec4(ಧ್ರುವ vvp_vector4_t&, ಧ್ರುವ vvp_vector2_t&)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}
ಶೂನ್ಯ automatic_signal_base::force_fil_vec8(ಧ್ರುವ vvp_vector8_t&, ಧ್ರುವ vvp_vector2_t&)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}
ಶೂನ್ಯ automatic_signal_base::force_fil_real(ದ್ವಿಗುಣ, ಧ್ರುವ vvp_vector2_t&)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}
ಶೂನ್ಯ automatic_signal_base::get_ಮೌಲ್ಯ(ಸಂರಚನಾ t_vpi_ಮೌಲ್ಯ*)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಧ್ರುವ vvp_vector4_t& vvp_fun_signal4_sa::vec4_unfiltered_ಮೌಲ್ಯ() ಧ್ರುವ
{
      ಪ್ರತ್ಯಾಗಮ bits4_;
}

vvp_fun_signal4_aa::vvp_fun_signal4_aa(unsigned wid, vvp_bit4_t init)
{
      context_idx_ = vpip_add_item_to_context(ಏತತ್, vpip_peek_context_scope());
      size_ = wid;
      init_ = init;
}

vvp_fun_signal4_aa::~vvp_fun_signal4_aa()
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ vvp_fun_signal4_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, ನೂತನ vvp_vector4_t(size_, init_));
}

ಶೂನ್ಯ vvp_fun_signal4_aa::reset_instance(vvp_context_t context)
{
      vvp_vector4_t*bits = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));

      bits->fill_bits(init_);
}

#ifdef CHECK_WITH_VALGRIND
ಶೂನ್ಯ vvp_fun_signal4_aa::free_instance(vvp_context_t context)
{
      vvp_vector4_t*bits = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));
      delete bits;
}
#endif

/*
 * Continuous and forced assignments are not permitted on automatic
 * variables. So we only expect to receive on port 0.
 */
ಶೂನ್ಯ vvp_fun_signal4_aa::recv_vec4(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit,
                                   vvp_context_t context)
{
      ಪ್ರತಿಜ್ಞಾ(ptr.port() == 0);
      ಪ್ರತಿಜ್ಞಾ(context);

      vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));

      ಯದಿ (!bits4->eeq(bit)) {
            *bits4 = bit;
            ptr.ptr()->send_vec4(*bits4, context);
      }
}

ಶೂನ್ಯ vvp_fun_signal4_aa::recv_vec4_pv(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit,
				      unsigned base, unsigned vwid, vvp_context_t context)
{
      ಪ್ರತಿಜ್ಞಾ(ptr.port() == 0);
      ಪ್ರತಿಜ್ಞಾ(size_ == vwid);
      ಪ್ರತಿಜ್ಞಾ(context);

      vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));

      unsigned wid = bit.size();
      ಪ್ರತಿ (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
            ಯದಿ (base+idx >= bits4->size()) ಭಙ್ಗ;
            bits4->set_bit(base+idx, bit.value(idx));
      }
      ptr.ptr()->send_vec4(*bits4, context);
}

unsigned vvp_fun_signal4_aa::ಮೌಲ್ಯ_size() ಧ್ರುವ
{
      ಪ್ರತ್ಯಾಗಮ size_;
}

vvp_bit4_t vvp_fun_signal4_aa::value(unsigned idx) ಧ್ರುವ
{
      ಧ್ರುವ vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_item(context_idx_));

      ಪ್ರತ್ಯಾಗಮ bits4->value(idx);
}

vvp_scalar_t vvp_fun_signal4_aa::scalar_ಮೌಲ್ಯ(unsigned idx) ಧ್ರುವ
{
      ಧ್ರುವ vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_item(context_idx_));

      ಪ್ರತ್ಯಾಗಮ vvp_scalar_t(bits4->value(idx), 6, 6);
}

ಶೂನ್ಯ vvp_fun_signal4_aa::vec4_ಮೌಲ್ಯ(vvp_vector4_t&val) ಧ್ರುವ
{
      ಧ್ರುವ vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_item(context_idx_));

      val = *bits4;
}

ಧ್ರುವ vvp_vector4_t&vvp_fun_signal4_aa::vec4_unfiltered_ಮೌಲ್ಯ() ಧ್ರುವ
{
      ಧ್ರುವ vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_item(context_idx_));

      ಪ್ರತ್ಯಾಗಮ *bits4;
}

ಶೂನ್ಯ vvp_fun_signal4_aa::operator delete(ಶೂನ್ಯ*)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

/*
 * Testing for equality, we want a bitwise test instead of an
 * arithmetic test because we want to treat for example -0 different
 * from +0.
 */
ತಾರ್ಕಿಕ bits_equal(ದ್ವಿಗುಣ a, ದ್ವಿಗುಣ b)
{
      ಪ್ರತ್ಯಾಗಮ memcmp(&a, &b, ಪರಿಮಾಣ a) == 0;
}

vvp_fun_signal_real_sa::vvp_fun_signal_real_sa()
{
      bits_ = 0.0;
}

ದ್ವಿಗುಣ vvp_fun_signal_real_sa::real_unfiltered_ಮೌಲ್ಯ() ಧ್ರುವ
{
      ಪ್ರತ್ಯಾಗಮ bits_;
}

ಶೂನ್ಯ vvp_fun_signal_real_sa::recv_real(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ bit,
                                       vvp_context_t)
{
      ವಿಕಲ್ಪ (ptr.port()) {
	  ಪಕ್ಷ 0:
	    ಯದಿ (!continuous_assign_active_) {
                  ಯದಿ (needs_init_ || !bits_equal(bits_, bit)) {
			bits_ = bit;
			needs_init_ = ಅಸತ್ಯ;
			ptr.ptr()->send_real(bit, 0);
		  }
	    }
	    ಭಙ್ಗ;

	  ಪಕ್ಷ 1: // Continuous assign value
	    continuous_assign_active_ = ಸತ್ಯ;
	    bits_ = bit;
	    ptr.ptr()->send_real(bit, 0);
	    ಭಙ್ಗ;

	  ಸ್ವಭಾವ:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.port());
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಭಙ್ಗ;
      }
}

vvp_fun_signal_real_aa::vvp_fun_signal_real_aa()
{
      context_idx_ = vpip_add_item_to_context(ಏತತ್, vpip_peek_context_scope());
}

vvp_fun_signal_real_aa::~vvp_fun_signal_real_aa()
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ vvp_fun_signal_real_aa::alloc_instance(vvp_context_t context)
{
      ದ್ವಿಗುಣ*bits = ನೂತನ ದ್ವಿಗುಣ;
      vvp_set_context_item(context, context_idx_, bits);

      *bits = 0.0;
}

ಶೂನ್ಯ vvp_fun_signal_real_aa::reset_instance(vvp_context_t context)
{
      ದ್ವಿಗುಣ*bits = static_cast<ದ್ವಿಗುಣ*>
            (vvp_get_context_item(context, context_idx_));

      *bits = 0.0;
}

#ifdef CHECK_WITH_VALGRIND
ಶೂನ್ಯ vvp_fun_signal_real_aa::free_instance(vvp_context_t context)
{
      ದ್ವಿಗುಣ*bits = static_cast<ದ್ವಿಗುಣ*>
            (vvp_get_context_item(context, context_idx_));
      delete bits;
}
#endif

ದ್ವಿಗುಣ vvp_fun_signal_real_aa::real_unfiltered_ಮೌಲ್ಯ() ಧ್ರುವ
{
      ಧ್ರುವ ದ್ವಿಗುಣ*bits = static_cast<ದ್ವಿಗುಣ*>
            (vthread_get_rd_context_item(context_idx_));

      ಪ್ರತ್ಯಾಗಮ *bits;
}

ದ್ವಿಗುಣ vvp_fun_signal_real_aa::real_ಮೌಲ್ಯ() ಧ್ರುವ
{
      ಪ್ರತ್ಯಾಗಮ real_unfiltered_ಮೌಲ್ಯ();
}

ಶೂನ್ಯ vvp_fun_signal_real_aa::recv_real(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ bit,
                                       vvp_context_t context)
{
      ಪ್ರತಿಜ್ಞಾ(ptr.port() == 0);
      ಪ್ರತಿಜ್ಞಾ(context);

      ದ್ವಿಗುಣ*bits = static_cast<ದ್ವಿಗುಣ*>
            (vvp_get_context_item(context, context_idx_));

      ಯದಿ (!bits_equal(*bits,bit)) {
            *bits = bit;
            ptr.ptr()->send_real(bit, context);
      }
}

unsigned vvp_fun_signal_real_aa::ಮೌಲ್ಯ_size() ಧ್ರುವ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಪ್ರತ್ಯಾಗಮ 1;
}

vvp_bit4_t vvp_fun_signal_real_aa::value(unsigned) ಧ್ರುವ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಪ್ರತ್ಯಾಗಮ BIT4_X;
}

vvp_scalar_t vvp_fun_signal_real_aa::scalar_ಮೌಲ್ಯ(unsigned) ಧ್ರುವ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಪ್ರತ್ಯಾಗಮ vvp_scalar_t();
}

ಶೂನ್ಯ vvp_fun_signal_real_aa::vec4_ಮೌಲ್ಯ(vvp_vector4_t&) ಧ್ರುವ
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ* vvp_fun_signal_real_aa::operator ನೂತನ(std::size_t size)
{
      ಪ್ರತ್ಯಾಗಮ vvp_net_fun_t::heap_.alloc(size);
}

ಶೂನ್ಯ vvp_fun_signal_real_aa::operator delete(ಶೂನ್ಯ*)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}


vvp_fun_signal_string_sa::vvp_fun_signal_string_sa()
{
}

ಶೂನ್ಯ vvp_fun_signal_string_sa::recv_string(vvp_net_ptr_t ptr, ಧ್ರುವ std::string&bit,
					   vvp_context_t)
{
      ಪ್ರತಿಜ್ಞಾ(ptr.port() == 0);

      ಯದಿ (needs_init_ || ಮೌಲ್ಯ_2 != bit) {
	    ಮೌಲ್ಯ_2 = bit;
	    needs_init_ = ಅಸತ್ಯ;

	    ptr.ptr()->send_string(bit, 0);
      }
}

ಧ್ರುವ string& vvp_fun_signal_string_sa::get_string() ಧ್ರುವ
{
      ಪ್ರತ್ಯಾಗಮ ಮೌಲ್ಯ_2;
}

vvp_fun_signal_string_aa::vvp_fun_signal_string_aa()
{
      context_idx_ = vpip_add_item_to_context(ಏತತ್, vpip_peek_context_scope());
}

vvp_fun_signal_string_aa::~vvp_fun_signal_string_aa()
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ vvp_fun_signal_string_aa::alloc_instance(vvp_context_t context)
{
      string*bits = ನೂತನ std::string;
      vvp_set_context_item(context, context_idx_, bits);
      *bits = "";
}

ಶೂನ್ಯ vvp_fun_signal_string_aa::reset_instance(vvp_context_t context)
{
      string*bits = static_cast<std::string*>
	    (vvp_get_context_item(context, context_idx_));
      *bits = "";
}

#ifdef CHECK_WITH_VALGRIND
ಶೂನ್ಯ vvp_fun_signal_string_aa::free_instance(vvp_context_t context)
{
      string*bits = static_cast<std::string*>
            (vvp_get_context_item(context, context_idx_));
      delete bits;
}
#endif

ಶೂನ್ಯ vvp_fun_signal_string_aa::recv_string(vvp_net_ptr_t ptr, ಧ್ರುವ std::string&bit, vvp_context_t context)
{
      ಪ್ರತಿಜ್ಞಾ(ptr.port() == 0);
      ಪ್ರತಿಜ್ಞಾ(context);

      string*bits = static_cast<std::string*>
	    (vvp_get_context_item(context, context_idx_));

      ಯದಿ (*bits != bit) {
	    *bits = bit;
	    ptr.ptr()->send_string(bit, context);
      }
}

unsigned vvp_fun_signal_string_aa::ಮೌಲ್ಯ_size() ಧ್ರುವ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಪ್ರತ್ಯಾಗಮ 1;
}

vvp_bit4_t vvp_fun_signal_string_aa::value(unsigned) ಧ್ರುವ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಪ್ರತ್ಯಾಗಮ BIT4_X;
}

vvp_scalar_t vvp_fun_signal_string_aa::scalar_ಮೌಲ್ಯ(unsigned) ಧ್ರುವ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಪ್ರತ್ಯಾಗಮ vvp_scalar_t();
}

ಶೂನ್ಯ vvp_fun_signal_string_aa::vec4_ಮೌಲ್ಯ(vvp_vector4_t&) ಧ್ರುವ
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ದ್ವಿಗುಣ vvp_fun_signal_string_aa::real_ಮೌಲ್ಯ() ಧ್ರುವ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಪ್ರತ್ಯಾಗಮ 0.0;
}

ಧ್ರುವ std::string& vvp_fun_signal_string_aa::get_string() ಧ್ರುವ
{
      ಧ್ರುವ string*bits = static_cast<std::string*>
            (vthread_get_rd_context_item(context_idx_));

      ಪ್ರತ್ಯಾಗಮ *bits;
}

ಶೂನ್ಯ* vvp_fun_signal_string_aa::operator ನೂತನ(std::size_t size)
{
      ಪ್ರತ್ಯಾಗಮ vvp_net_fun_t::heap_.alloc(size);
}

ಶೂನ್ಯ vvp_fun_signal_string_aa::operator delete(ಶೂನ್ಯ*)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

  /* OBJECT signals */

vvp_fun_signal_object_sa::vvp_fun_signal_object_sa(unsigned size)
: vvp_fun_signal_object(size)
{
}

#ifdef CHECK_WITH_VALGRIND
ಶೂನ್ಯ vvp_fun_signal_object_aa::free_instance(vvp_context_t context)
{
      vvp_object_t*bits = static_cast<vvp_object_t*>
            (vvp_get_context_item(context, context_idx_));
      delete bits;
}
#endif

ಶೂನ್ಯ vvp_fun_signal_object_sa::recv_object(vvp_net_ptr_t ptr, vvp_object_t bit,
					   vvp_context_t)
{
      ಪ್ರತಿಜ್ಞಾ(ptr.port() == 0);

      ಯದಿ (needs_init_ || ಮೌಲ್ಯ_2 != bit) {
	    ಮೌಲ್ಯ_2 = bit;
	    needs_init_ = ಅಸತ್ಯ;

	    ptr.ptr()->send_object(bit, 0);
      }
}

vvp_object_t vvp_fun_signal_object_sa::get_object() ಧ್ರುವ
{
      ಪ್ರತ್ಯಾಗಮ ಮೌಲ್ಯ_2;
}

vvp_fun_signal_object_aa::vvp_fun_signal_object_aa(unsigned size)
: vvp_fun_signal_object(size)
{
      context_idx_ = vpip_add_item_to_context(ಏತತ್, vpip_peek_context_scope());
}

vvp_fun_signal_object_aa::~vvp_fun_signal_object_aa()
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ vvp_fun_signal_object_aa::alloc_instance(vvp_context_t context)
{
      vvp_object_t*bits = ನೂತನ vvp_object_t;
      vvp_set_context_item(context, context_idx_, bits);
      bits->reset();
}

ಶೂನ್ಯ vvp_fun_signal_object_aa::reset_instance(vvp_context_t context)
{
      vvp_object_t*bits = static_cast<vvp_object_t*>
	    (vvp_get_context_item(context, context_idx_));
      bits->reset();
}

vvp_object_t vvp_fun_signal_object_aa::get_object() ಧ್ರುವ
{
      ಧ್ರುವ vvp_object_t*bits = static_cast<vvp_object_t*>
	    (vthread_get_rd_context_item(context_idx_));
      ಪ್ರತ್ಯಾಗಮ *bits;
}

ಶೂನ್ಯ vvp_fun_signal_object_aa::recv_object(vvp_net_ptr_t ptr, vvp_object_t bit,
					   vvp_context_t context)
{
      ಪ್ರತಿಜ್ಞಾ(ptr.port() == 0);
      ಪ್ರತಿಜ್ಞಾ(context);

      vvp_object_t*bits = static_cast<vvp_object_t*>
	    (vvp_get_context_item(context, context_idx_));

      ಯದಿ (*bits != bit) {
	    *bits = bit;
	    ptr.ptr()->send_object(bit, context);
      }
}

unsigned vvp_fun_signal_object_aa::ಮೌಲ್ಯ_size() ಧ್ರುವ
{
      ಪ್ರತ್ಯಾಗಮ 1;
}

vvp_bit4_t vvp_fun_signal_object_aa::value(unsigned) ಧ್ರುವ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಪ್ರತ್ಯಾಗಮ BIT4_X;
}

vvp_scalar_t vvp_fun_signal_object_aa::scalar_ಮೌಲ್ಯ(unsigned) ಧ್ರುವ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಪ್ರತ್ಯಾಗಮ vvp_scalar_t();
}

ಶೂನ್ಯ vvp_fun_signal_object_aa::vec4_ಮೌಲ್ಯ(vvp_vector4_t&) ಧ್ರುವ
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ* vvp_fun_signal_object_aa::operator ನೂತನ(std::size_t size)
{
      ಪ್ರತ್ಯಾಗಮ vvp_net_fun_t::heap_.alloc(size);
}

ಶೂನ್ಯ vvp_fun_signal_object_aa::operator delete(ಶೂನ್ಯ*)
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

ಶೂನ್ಯ vvp_fun_force::recv_vec4(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit,
			      vvp_context_t)
{
      ಪ್ರತಿಜ್ಞಾ(ptr.port() == 0);
      vvp_net_t*net = ptr.ptr();

      vvp_net_t*dst = net->port[3].ptr();
      ಪ್ರತಿಜ್ಞಾ(dst->fil);

      dst->force_vec4(coerce_to_width(bit, dst->fil->filter_size()), vvp_vector2_t(vvp_vector2_t::FILL1, dst->fil->filter_size()));
}

ಶೂನ್ಯ vvp_fun_force::recv_real(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ bit, vvp_context_t)
{
      ಪ್ರತಿಜ್ಞಾ(ptr.port() == 0);
      vvp_net_t*net = ptr.ptr();
      vvp_net_t*dst = net->port[3].ptr();
      dst->force_real(bit, vvp_vector2_t(vvp_vector2_t::FILL1, 1));
}

vvp_wire_base::vvp_wire_base()
{
}

vvp_wire_base::~vvp_wire_base()
{
}

vvp_bit4_t vvp_wire_base::driven_ಮೌಲ್ಯ(unsigned) ಧ್ರುವ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಪ್ರತ್ಯಾಗಮ BIT4_X;
}

ತಾರ್ಕಿಕ vvp_wire_base::is_forced(unsigned) ಧ್ರುವ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

vvp_wire_vec4::vvp_wire_vec4(unsigned wid, vvp_bit4_t init)
: bits4_(wid, init)
{
      needs_init_ = ಸತ್ಯ;
}

vvp_net_fil_t::prop_t vvp_wire_vec4::filter_vec4(ಧ್ರುವ vvp_vector4_t&bit, vvp_vector4_t&rep,
						 unsigned base, unsigned vwid)
{
	// Special case! the input bit is 0 wid. Interpret this as a
	// vector of BIT4_X to match the width of the bits4_ vector.
	// FIXME! This is a hack to work around some buggy gate
	// implementations! This should be removed!
      ಯದಿ (base==0 && vwid==0) {
	    vvp_vector4_t tmp (bits4_.size(), BIT4_X);
	    ಯದಿ (bits4_ .eeq(tmp) && !needs_init_) ಪ್ರತ್ಯಾಗಮ STOP;
	    bits4_ = tmp;
	    needs_init_ = ಅಸತ್ಯ;
	    ಪ್ರತ್ಯಾಗಮ filter_mask_(tmp, force4_, rep, 0);
      }

      ಯದಿ (vwid != bits4_.size()) {
	    cerr << "Internal error: Input vector expected width="
		 << bits4_.size() << ", got "
		 << "bit=" << bit << ", base=" << base << ", vwid=" << vwid
		 << endl;
      }
      ಪ್ರತಿಜ್ಞಾ(bits4_.size() == vwid);

	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      ಯದಿ (base==0 && bit.size()==vwid) {
	    ಯದಿ (bits4_ .eeq( bit ) && !needs_init_) ಪ್ರತ್ಯಾಗಮ STOP;
	    bits4_ = bit;
      } ಅನ್ಯಥಾ {
	    ತಾರ್ಕಿಕ rc = bits4_.set_vec(base, bit);
	    ಯದಿ (rc == ಅಸತ್ಯ && !needs_init_) ಪ್ರತ್ಯಾಗಮ STOP;
      }

      needs_init_ = ಅಸತ್ಯ;
      ಪ್ರತ್ಯಾಗಮ filter_mask_(bit, force4_, rep, base);
}

vvp_net_fil_t::prop_t vvp_wire_vec4::filter_vec8(ಧ್ರುವ vvp_vector8_t&bit,
                                                 vvp_vector8_t&rep,
                                                 unsigned base,
                                                 unsigned vwid)
{
      ಪ್ರತಿಜ್ಞಾ(bits4_.size() == vwid);

	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      vvp_vector4_t bit4 (reduce4(bit));
      ಯದಿ (base==0 && bit4.size()==vwid) {
	    ಯದಿ (bits4_ .eeq( bit4 ) && !needs_init_) ಪ್ರತ್ಯಾಗಮ STOP;
	    bits4_ = bit4;
      } ಅನ್ಯಥಾ {
	    ತಾರ್ಕಿಕ rc = bits4_.set_vec(base, bit4);
	    ಯದಿ (rc == ಅಸತ್ಯ && !needs_init_) ಪ್ರತ್ಯಾಗಮ STOP;
      }

      needs_init_ = ಅಸತ್ಯ;
      ಪ್ರತ್ಯಾಗಮ filter_mask_(bit, vvp_vector8_t(force4_,6,6), rep, base);
}

unsigned vvp_wire_vec4::filter_size() ಧ್ರುವ
{
      ಪ್ರತ್ಯಾಗಮ bits4_.size();
}

ಶೂನ್ಯ vvp_wire_vec4::force_fil_vec4(ಧ್ರುವ vvp_vector4_t&val, ಧ್ರುವ vvp_vector2_t&mask)
{
      force_mask(mask);

      ಯದಿ (force4_.size() == 0) {
	    force4_ = val;
      } ಅನ್ಯಥಾ {
	    ಪ್ರತಿ (unsigned idx = 0; idx < mask.size() ; idx += 1) {
		  ಯದಿ (mask.value(idx) == 0)
			ಅನುವರ್ತ;

		  force4_.set_bit(idx, val.value(idx));
	    }
      }
      run_vpi_callbacks();
}

ಶೂನ್ಯ vvp_wire_vec4::force_fil_vec8(ಧ್ರುವ vvp_vector8_t&, ಧ್ರುವ vvp_vector2_t&)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ vvp_wire_vec4::force_fil_real(ದ್ವಿಗುಣ, ಧ್ರುವ vvp_vector2_t&)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ vvp_wire_vec4::release(vvp_net_ptr_t ptr, ತಾರ್ಕಿಕ net_flag)
{
      vvp_vector2_t mask (vvp_vector2_t::FILL1, bits4_.size());
      ಯದಿ (net_flag) {
	      // Wires revert to their unforced value after release.
            release_mask(mask);
	    needs_init_ = ! force4_ .eeq(bits4_);
	    ptr.ptr()->send_vec4(bits4_, 0);
	    run_vpi_callbacks();
      } ಅನ್ಯಥಾ {
	      // Variables keep the current value.
	    vvp_vector4_t res (bits4_.size());
	    ಪ್ರತಿ (unsigned idx=0; idx<bits4_.size(); idx += 1)
		  res.set_bit(idx,value(idx));
            release_mask(mask);
	    ptr.ptr()->fun->recv_vec4(ptr, res, 0);
      }
}

ಶೂನ್ಯ vvp_wire_vec4::release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, ತಾರ್ಕಿಕ net_flag)
{
      ಪ್ರತಿಜ್ಞಾ(bits4_.size() >= base + wid);

      vvp_vector2_t mask (vvp_vector2_t::FILL0, bits4_.size());
      ಪ್ರತಿ (unsigned idx = 0 ; idx < wid ; idx += 1)
	    mask.set_bit(base+idx, 1);

      ಯದಿ (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_mask(mask);
	    needs_init_ = ! force4_.subvalue(base,wid) .eeq(bits4_.subvalue(base,wid));
	    ptr.ptr()->send_vec4_pv(bits4_.subvalue(base,wid),
				    base, bits4_.size(), 0);
	    run_vpi_callbacks();
      } ಅನ್ಯಥಾ {
	      // Variables keep the current value.
	    vvp_vector4_t res (wid);
	    ಪ್ರತಿ (unsigned idx=0; idx<wid; idx += 1)
		  res.set_bit(idx,value(base+idx));
	    release_mask(mask);
	    ptr.ptr()->fun->recv_vec4_pv(ptr, res, base, bits4_.size(), 0);
      }
}

unsigned vvp_wire_vec4::ಮೌಲ್ಯ_size() ಧ್ರುವ
{
      ಪ್ರತ್ಯಾಗಮ bits4_.size();
}

vvp_bit4_t vvp_wire_vec4::filtered_ಮೌಲ್ಯ(unsigned idx) ಧ್ರುವ
{
      ಯದಿ (test_force_mask(idx))
	    ಪ್ರತ್ಯಾಗಮ force4_.value(idx);
      ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ bits4_.value(idx);
}

vvp_bit4_t vvp_wire_vec4::value(unsigned idx) ಧ್ರುವ
{
      ಪ್ರತ್ಯಾಗಮ filtered_ಮೌಲ್ಯ(idx);
}

vvp_scalar_t vvp_wire_vec4::scalar_ಮೌಲ್ಯ(unsigned idx) ಧ್ರುವ
{
      ಪ್ರತ್ಯಾಗಮ vvp_scalar_t(value(idx),6,6);
}

ಶೂನ್ಯ vvp_wire_vec4::vec4_ಮೌಲ್ಯ(vvp_vector4_t&val) ಧ್ರುವ
{
      val = bits4_;
      ಯದಿ (test_force_mask_is_zero())
	    ಪ್ರತ್ಯಾಗಮ;

      ಪ್ರತಿ (unsigned idx = 0 ; idx < bits4_.size() ; idx += 1)
	    val.set_bit(idx, filtered_ಮೌಲ್ಯ(idx));
}

vvp_bit4_t vvp_wire_vec4::driven_ಮೌಲ್ಯ(unsigned idx) ಧ್ರುವ
{
      ಪ್ರತ್ಯಾಗಮ bits4_.value(idx);
}

ತಾರ್ಕಿಕ vvp_wire_vec4::is_forced(unsigned idx) ಧ್ರುವ
{
      ಪ್ರತ್ಯಾಗಮ test_force_mask(idx);
}

vvp_wire_vec8::vvp_wire_vec8(unsigned wid)
: bits8_(wid)
{
      needs_init_ = ಸತ್ಯ;
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_vec4(ಧ್ರುವ vvp_vector4_t&bit,
                                                 vvp_vector4_t&rep,
                                                 unsigned base,
                                                 unsigned vwid)
{
	// For now there is no support for a non-zero base.
      ಪ್ರತಿಜ್ಞಾ(0 == base);
      ಪ್ರತಿಜ್ಞಾ(bits8_.size() == vwid);
      ಪ್ರತಿಜ್ಞಾ(bits8_.size() == bit.size());
	// QUESTION: Is it really correct to propagate a vec4 if this
	// is a vec8 node? In fact, it is really possible for a vec4
	// value to get through to a vec8 filter?
      vvp_vector8_t rep8;
      prop_t rc = filter_vec8(vvp_vector8_t(bit,6,6), rep8, 0, vwid);
      ಯದಿ (rc == REPL)
	    rep = reduce4(rep8);

      needs_init_ = ಅಸತ್ಯ;
      ಪ್ರತ್ಯಾಗಮ rc;
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_vec8(ಧ್ರುವ vvp_vector8_t&bit, vvp_vector8_t&rep, unsigned base, unsigned vwid)
{
      ಪ್ರತಿಜ್ಞಾ(vwid == bits8_.size());
	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      ಯದಿ (base==0 && bit.size()==vwid) {
	    ಯದಿ (bits8_.eeq(bit) && !needs_init_) ಪ್ರತ್ಯಾಗಮ STOP;
	    bits8_ = bit;
      } ಅನ್ಯಥಾ {
	    ಯದಿ (bits8_.subvalue(base, bit.size()).eeq(bit) &&
		!needs_init_) {
		  ಪ್ರತ್ಯಾಗಮ STOP;
	    }
	    ಯದಿ (bits8_.size() == 0)
		  bits8_ = vvp_vector8_t(vwid);
	    ಪ್ರತಿಜ್ಞಾ(bits8_.size() == vwid);
	    bits8_.set_vec(base, bit);
      }
      needs_init_ = ಅಸತ್ಯ;
      ಪ್ರತ್ಯಾಗಮ filter_mask_(bit, force8_, rep, base);
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_ಒಳಹರಿವು_vec8(ಧ್ರುವ vvp_vector8_t&bit, vvp_vector8_t&rep) ಧ್ರುವ
{
      ಪ್ರತ್ಯಾಗಮ filter_ಒಳಹರಿವು_mask(bit, force8_, rep);
}

unsigned vvp_wire_vec8::filter_size() ಧ್ರುವ
{
      ಪ್ರತ್ಯಾಗಮ bits8_.size();
}

ಶೂನ್ಯ vvp_wire_vec8::force_fil_vec4(ಧ್ರುವ vvp_vector4_t&val, ಧ್ರುವ vvp_vector2_t&mask)
{
      force_fil_vec8(vvp_vector8_t(val,6,6), mask);
}

ಶೂನ್ಯ vvp_wire_vec8::force_fil_vec8(ಧ್ರುವ vvp_vector8_t&val, ಧ್ರುವ vvp_vector2_t&mask)
{
      force_mask(mask);

      ಯದಿ (force8_.size() == 0) {
	    force8_ = val;
      } ಅನ್ಯಥಾ {
	    ಪ್ರತಿ (unsigned idx = 0; idx < mask.size() ; idx += 1) {
		  ಯದಿ (mask.value(idx) == 0)
			ಅನುವರ್ತ;

		  force8_.set_bit(idx, val.value(idx));
	    }
      }
      run_vpi_callbacks();
}

ಶೂನ್ಯ vvp_wire_vec8::force_fil_real(ದ್ವಿಗುಣ, ಧ್ರುವ vvp_vector2_t&)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ vvp_wire_vec8::release(vvp_net_ptr_t ptr, ತಾರ್ಕಿಕ net_flag)
{
	// Wires revert to their unforced value after release.
      vvp_vector2_t mask (vvp_vector2_t::FILL1, bits8_.size());
      release_mask(mask);
      ಯದಿ (net_flag) {
	    needs_init_ = !force8_ .eeq(bits8_);
	    ptr.ptr()->send_vec8(bits8_);
      } ಅನ್ಯಥಾ {
	// Variable do not know about strength so this should not be able
	// to happen. If for some reason it can then it should not be too
	// hard to fix this code like was done for vvp_wire_vec4 above.
	    ಪ್ರತಿಜ್ಞಾ(0);
//	    ptr.ptr()->fun->recv_vec8(ptr, force8_);
      }
}

ಶೂನ್ಯ vvp_wire_vec8::release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, ತಾರ್ಕಿಕ net_flag)
{
      ಪ್ರತಿಜ್ಞಾ(bits8_.size() >= base + wid);

      vvp_vector2_t mask (vvp_vector2_t::FILL0, bits8_.size());
      ಪ್ರತಿ (unsigned idx = 0 ; idx < wid ; idx += 1)
	    mask.set_bit(base+idx, 1);

      release_mask(mask);

      ಯದಿ (net_flag) {
	    needs_init_ = !force8_.subvalue(base,wid) .eeq((bits8_.subvalue(base,wid)));
	    ptr.ptr()->send_vec8_pv(bits8_.subvalue(base,wid),
				    base, bits8_.size());
	    run_vpi_callbacks();
      } ಅನ್ಯಥಾ {
	// Variable do not know about strength so this should not be able
	// to happen. If for some reason it can then it should not be too
	// hard to fix this code like was done for vvp_wire_vec4 above.
	    ಪ್ರತಿಜ್ಞಾ(0);
//	    ptr.ptr()->fun->recv_vec8_pv(ptr, force8_.subvalue(base,wid),
//					 base, force8_.size());
      }
}

unsigned vvp_wire_vec8::ಮೌಲ್ಯ_size() ಧ್ರುವ
{
      ಪ್ರತ್ಯಾಗಮ bits8_.size();
}

vvp_scalar_t vvp_wire_vec8::filtered_ಮೌಲ್ಯ(unsigned idx) ಧ್ರುವ
{
      ಯದಿ (test_force_mask(idx))
	    ಪ್ರತ್ಯಾಗಮ force8_.value(idx);
      ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ bits8_.value(idx);
}

vvp_bit4_t vvp_wire_vec8::value(unsigned idx) ಧ್ರುವ
{
      ಪ್ರತ್ಯಾಗಮ filtered_ಮೌಲ್ಯ(idx).value();
}

vvp_scalar_t vvp_wire_vec8::scalar_ಮೌಲ್ಯ(unsigned idx) ಧ್ರುವ
{
      ಪ್ರತ್ಯಾಗಮ filtered_ಮೌಲ್ಯ(idx);
}

vvp_vector8_t vvp_wire_vec8::vec8_ಮೌಲ್ಯ() ಧ್ರುವ
{
      vvp_vector8_t tmp = bits8_;
      ಪ್ರತಿ (unsigned idx = 0 ; idx < bits8_.size() ; idx += 1)
	    tmp.set_bit(idx, filtered_ಮೌಲ್ಯ(idx));
      ಪ್ರತ್ಯಾಗಮ tmp;
}

ಶೂನ್ಯ vvp_wire_vec8::vec4_ಮೌಲ್ಯ(vvp_vector4_t&val) ಧ್ರುವ
{
      val = reduce4(vec8_ಮೌಲ್ಯ());
}

vvp_bit4_t vvp_wire_vec8::driven_ಮೌಲ್ಯ(unsigned idx) ಧ್ರುವ
{
      ಪ್ರತ್ಯಾಗಮ bits8_.value(idx).value();
}

ತಾರ್ಕಿಕ vvp_wire_vec8::is_forced(unsigned idx) ಧ್ರುವ
{
      ಪ್ರತ್ಯಾಗಮ test_force_mask(idx);
}

vvp_wire_real::vvp_wire_real()
: bit_(0.0), force_(0.0)
{
}

vvp_net_fil_t::prop_t vvp_wire_real::filter_real(ದ್ವಿಗುಣ&bit)
{
      ಯದಿ (bits_equal(bit_, bit) && !needs_init_) ಪ್ರತ್ಯಾಗಮ STOP;
      bit_ = bit;
      needs_init_ = ಅಸತ್ಯ;
      ಪ್ರತ್ಯಾಗಮ filter_mask_(bit, force_);
}

unsigned vvp_wire_real::filter_size() ಧ್ರುವ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ vvp_wire_real::force_fil_vec4(ಧ್ರುವ vvp_vector4_t&, ಧ್ರುವ vvp_vector2_t&)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ vvp_wire_real::force_fil_vec8(ಧ್ರುವ vvp_vector8_t&, ಧ್ರುವ vvp_vector2_t&)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ vvp_wire_real::force_fil_real(ದ್ವಿಗುಣ val, ಧ್ರುವ vvp_vector2_t&mask)
{
      force_mask(mask);
      ಯದಿ (mask.value(0))
	    force_ = val;

      run_vpi_callbacks();
}

ಶೂನ್ಯ vvp_wire_real::release(vvp_net_ptr_t ptr, ತಾರ್ಕಿಕ net_flag)
{
      vvp_vector2_t mask (vvp_vector2_t::FILL1, 1);
      ಯದಿ (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_mask(mask);
	    needs_init_ = !bits_equal(force_, bit_);
	    ptr.ptr()->send_real(bit_, 0);
      } ಅನ್ಯಥಾ {
	      // Variables keep the current value.
	    ದ್ವಿಗುಣ res =  real_ಮೌಲ್ಯ();
	    release_mask(mask);
	    ptr.ptr()->fun->recv_real(ptr, res, 0);
      }
}

ಶೂನ್ಯ vvp_wire_real::release_pv(vvp_net_ptr_t, unsigned, unsigned, ತಾರ್ಕಿಕ)
{
      ಪ್ರತಿಜ್ಞಾ(0);
#ಯದಿ 0
	// A real is a single value. If for some reason this part release
	// can happen the following code should work correctly (requires
	// a base of 0 and a width of 1).
      vvp_vector2_t mask (vvp_vector2_t::FILL1, 1);
      ಪ್ರತಿಜ್ಞಾ(base == 0 && wid == 1);

      ಯದಿ (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_mask(mask);
	    ptr.ptr()->send_real(bit_, 0);
      } ಅನ್ಯಥಾ {
	      // Variables keep the current value.
	    ದ್ವಿಗುಣ res =  real_ಮೌಲ್ಯ();
	    release_mask(mask);
	    ptr.ptr()->fun->recv_real(ptr, res, 0);
      }
#endif
}

unsigned vvp_wire_real::ಮೌಲ್ಯ_size() ಧ್ರುವ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಪ್ರತ್ಯಾಗಮ 1;
}

vvp_bit4_t vvp_wire_real::value(unsigned) ಧ್ರುವ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಪ್ರತ್ಯಾಗಮ BIT4_X;
}

vvp_scalar_t vvp_wire_real::scalar_ಮೌಲ್ಯ(unsigned) ಧ್ರುವ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಪ್ರತ್ಯಾಗಮ vvp_scalar_t();
}

ಶೂನ್ಯ vvp_wire_real::vec4_ಮೌಲ್ಯ(vvp_vector4_t&) ಧ್ರುವ
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ದ್ವಿಗುಣ vvp_wire_real::real_ಮೌಲ್ಯ() ಧ್ರುವ
{
      ಯದಿ (test_force_mask(0))
	    ಪ್ರತ್ಯಾಗಮ force_;
      ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ bit_;
}

#ಯದಿ 0
vvp_wire_string::vvp_wire_string()
{
}

unsigned vvp_wire_string::filter_size() ಧ್ರುವ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ vvp_wire_string::force_fil_vec4(ಧ್ರುವ vvp_vector4_t&, ಧ್ರುವ vvp_vector2_t&)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}
ಶೂನ್ಯ vvp_wire_string::force_fil_vec8(ಧ್ರುವ vvp_vector8_t&, ಧ್ರುವ vvp_vector2_t&)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}
ಶೂನ್ಯ vvp_wire_string::force_fil_real(ದ್ವಿಗುಣ, ಧ್ರುವ vvp_vector2_t&)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ vvp_wire_string::release(vvp_net_ptr_t ptr, ತಾರ್ಕಿಕ net_flag)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ vvp_wire_string::release_pv(vvp_net_ptr_t, unsigned, unsigned, ತಾರ್ಕಿಕ)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

unsigned vvp_wire_string::ಮೌಲ್ಯ_size() ಧ್ರುವ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಪ್ರತ್ಯಾಗಮ 1;
}

vvp_bit4_t vvp_wire_string::value(unsigned) ಧ್ರುವ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಪ್ರತ್ಯಾಗಮ BIT4_X;
}

vvp_scalar_t vvp_wire_string::scalar_ಮೌಲ್ಯ(unsigned) ಧ್ರುವ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಪ್ರತ್ಯಾಗಮ vvp_scalar_t();
}

ಶೂನ್ಯ vvp_wire_string::vec4_ಮೌಲ್ಯ(vvp_vector4_t&) ಧ್ರುವ
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ದ್ವಿಗುಣ vvp_wire_string::real_ಮೌಲ್ಯ() ಧ್ರುವ
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಪ್ರತ್ಯಾಗಮ 0.0;
}
#endif
