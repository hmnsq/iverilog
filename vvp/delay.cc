/*
 * Copyright (c) 2005-2026 Stephen Williams <steve@icarus.com>
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

#ಅನ್ತರ್ಭಾವ "delay.h"
#ಅನ್ತರ್ಭಾವ "schedule.h"
#ಅನ್ತರ್ಭಾವ "vpi_priv.h"
#ಅನ್ತರ್ಭಾವ "config.h"
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
#ಅನ್ತರ್ಭಾವ "vvp_cleanup.h"
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ <iostream>
#ಅನ್ತರ್ಭಾವ <cstdlib>
#ಅನ್ತರ್ಭಾವ <list>
#ಅನ್ತರ್ಭಾವ <cassert>
#ಅನ್ತರ್ಭಾವ <cmath>
#ಅನ್ತರ್ಭಾವ "ivl_alloc.h"

ಬಳಸಿ ನಾಮಾಕಾಶ std;

ಶೂನ್ಯ vvp_delay_t::calculate_min_delay_()
{
      min_delay_ = rise_;
      ಆದರೆ (fall_ < min_delay_)
	    min_delay_ = fall_;
      ಆದರೆ (ignore_decay_) decay_ = min_delay_;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (decay_ < min_delay_)
	    min_delay_ = decay_;
}

vvp_delay_t::vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall)
{
      rise_ = rise;
      fall_ = fall;
      decay_= fall < rise? fall : rise;
      min_delay_ = decay_;
      ignore_decay_ = ಸುಳ್ಳು;
}

vvp_delay_t::vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall, vvp_time64_t decay)
{
      rise_ = rise;
      fall_ = fall;
      decay_= decay;
      ignore_decay_ = ಸುಳ್ಳು;

      calculate_min_delay_();
}

ಶೂನ್ಯ vvp_delay_t::ಗಣ_ignore_decay()
{
      ignore_decay_ = ನಿಜ;

      calculate_min_delay_();
}

vvp_delay_t::~vvp_delay_t()
{
}

vvp_time64_t vvp_delay_t::get_delay(vvp_bit4_t from, vvp_bit4_t to)
{
      ಆಯ್ಕೆ (from) {
	  ಸ್ಥಿತಿ BIT4_0:
	    ಆಯ್ಕೆ (to) {
		ಸ್ಥಿತಿ BIT4_0: ಹಿಂತಿರುಗಿ 0;
		ಸ್ಥಿತಿ BIT4_1: ಹಿಂತಿರುಗಿ rise_;
		ಸ್ಥಿತಿ BIT4_X: ಹಿಂತಿರುಗಿ min_delay_;
		ಸ್ಥಿತಿ BIT4_Z: ಹಿಂತಿರುಗಿ decay_;
	    }
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ BIT4_1:
	    ಆಯ್ಕೆ (to) {
		ಸ್ಥಿತಿ BIT4_0: ಹಿಂತಿರುಗಿ fall_;
		ಸ್ಥಿತಿ BIT4_1: ಹಿಂತಿರುಗಿ 0;
		ಸ್ಥಿತಿ BIT4_X: ಹಿಂತಿರುಗಿ min_delay_;
		ಸ್ಥಿತಿ BIT4_Z: ಹಿಂತಿರುಗಿ decay_;
	    }
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ BIT4_X:
	    ಆಯ್ಕೆ (to) {
		ಸ್ಥಿತಿ BIT4_0: ಹಿಂತಿರುಗಿ fall_;
		ಸ್ಥಿತಿ BIT4_1: ಹಿಂತಿರುಗಿ rise_;
		ಸ್ಥಿತಿ BIT4_X: ಹಿಂತಿರುಗಿ 0;
		ಸ್ಥಿತಿ BIT4_Z: ಹಿಂತಿರುಗಿ decay_;
	    }
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ BIT4_Z:
	    ಆಯ್ಕೆ (to) {
		ಸ್ಥಿತಿ BIT4_0: ಹಿಂತಿರುಗಿ fall_;
		ಸ್ಥಿತಿ BIT4_1: ಹಿಂತಿರುಗಿ rise_;
		ಸ್ಥಿತಿ BIT4_X: ಹಿಂತಿರುಗಿ min_delay_;
		ಸ್ಥಿತಿ BIT4_Z: ಹಿಂತಿರುಗಿ 0;
	    }
	    ನಿಲ್ಲಿಸು;
      }

      ಪ್ರತಿಜ್ಞಾ(0);
      ಹಿಂತಿರುಗಿ 0;
}

vvp_time64_t vvp_delay_t::get_min_delay() ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ min_delay_;
}

ಶೂನ್ಯ vvp_delay_t::ಗಣ_rise(vvp_time64_t val)
{
      rise_ = val;
      ಆದರೆ (val < min_delay_) {
	    min_delay_ = val;
	    ಆದರೆ (ignore_decay_) decay_ = val;
      } ಇಲ್ಲದಿದ್ದರೆ
	    calculate_min_delay_();
}

ಶೂನ್ಯ vvp_delay_t::ಗಣ_fall(vvp_time64_t val)
{
      fall_ = val;
      ಆದರೆ (val < min_delay_) {
	    min_delay_ = val;
	    ಆದರೆ (ignore_decay_) decay_ = val;
      } ಇಲ್ಲದಿದ್ದರೆ
	    calculate_min_delay_();
}

ಶೂನ್ಯ vvp_delay_t::ಗಣ_decay(vvp_time64_t val)
{
      ಪ್ರತಿಜ್ಞಾ(!ignore_decay_);

      decay_ = val;
      ಆದರೆ (val < min_delay_)
	    min_delay_ = val;
      ಇಲ್ಲದಿದ್ದರೆ
	    calculate_min_delay_();
}

vvp_fun_delay::vvp_fun_delay(vvp_net_t*n, ಚಿಹ್ನೆರಹಿತ width, ಸ್ಥಿರಾಂಕ vvp_delay_t&d)
: net_(n), delay_(d)
{
      cur_real_ = 0.0;
      ಆದರೆ (width > 0) {
            cur_vec4_ = vvp_vector4_t(width, BIT4_X);
            cur_vec8_ = vvp_vector8_t(cur_vec4_, 6, 6);
            ವೇಳಾಪಟ್ಟಿ_init_propagate(net_, cur_vec4_);
      } ಇಲ್ಲದಿದ್ದರೆ {
            ವೇಳಾಪಟ್ಟಿ_init_propagate(net_, cur_real_);
      }
      list_ = 0;
      type_ = UNKNOWN_DELAY;
      initial_ = ನಿಜ;
	// Calculate the values used when converting variable delays
	// to simulation time units.
      ಸ್ಥಿರಾಂಕ __vpiScope*scope = vpip_peek_current_scope();

      ಪೂರ್ಣಾಂಕ powr = scope->ಸಮಯ_units - scope->ಸಮಯ_precision;
      ಸುತ್ತು = 1;
      ಗಾಗಿ (ಪೂರ್ಣಾಂಕ lp = 0; lp < powr; lp += 1) ಸುತ್ತು *= 10;

      powr = scope->ಸಮಯ_precision - vpip_get_ಸಮಯ_precision();
      scale_ = 1;
      ಗಾಗಿ (ಪೂರ್ಣಾಂಕ lp = 0; lp < powr; lp += 1) scale_ *= 10;
}

vvp_fun_delay::~vvp_fun_delay()
{
      ಆಗ (ರಚನೆ event_*cur = ಸರತಿಯಿಂದ_ತೆಗೆ())
	    ಅಳಿಸು cur;
}

ತಾರ್ಕಿಕ vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್)
{
      ಆದರೆ (list_ == 0) ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      ಆದರೆ (list_->ಮುಂದಿನ_2->ptr_vec4.eeq(ಬಿಟ್)) ಹಿಂತಿರುಗಿ ನಿಜ;

      clean_pulse_events_(use_delay);
      ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ತಾರ್ಕಿಕ vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        ಸ್ಥಿರಾಂಕ vvp_vector8_t&ಬಿಟ್)
{
      ಆದರೆ (list_ == 0) ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      ಆದರೆ (list_->ಮುಂದಿನ_2->ptr_vec8.eeq(ಬಿಟ್)) ಹಿಂತಿರುಗಿ ನಿಜ;

      clean_pulse_events_(use_delay);
      ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ತಾರ್ಕಿಕ vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        ದ್ವಿಗುಣ ಬಿಟ್)
{
      ಆದರೆ (list_ == 0) ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      ಆದರೆ (list_->ಮುಂದಿನ_2->ptr_real == ಬಿಟ್) ಹಿಂತಿರುಗಿ ನಿಜ;

      clean_pulse_events_(use_delay);
      ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ಶೂನ್ಯ vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay)
{
      ಪ್ರತಿಜ್ಞಾ(list_ != 0);

      ಮಾಡು {
	    ರಚನೆ event_*cur = list_->ಮುಂದಿನ_2;
	      /* If this event is far enough from the event I'm about
	         to create, then that scheduled event is not a pulse
	         to be eliminated, so we're done. */
	    ಆದರೆ (cur->sim_ಸಮಯ+use_delay <= use_delay+ವೇಳಾಪಟ್ಟಿ_simtime())
		  ನಿಲ್ಲಿಸು;

	    ಆದರೆ (list_ == cur)
		  list_ = 0;
	    ಇಲ್ಲದಿದ್ದರೆ
		  list_->ಮುಂದಿನ_2 = cur->ಮುಂದಿನ_2;
	    ಅಳಿಸು cur;
      } ಆಗ (list_);
}

/*
 * FIXME: this implementation currently only uses the maximum delay
 * from all the bit changes in the vectors. If there are multiple
 * changes with different delays, then the results would be
 * wrong. What should happen is that if there are multiple changes,
 * multiple vectors approaching the result should be scheduled.
 */
ಶೂನ್ಯ vvp_fun_delay::recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                              vvp_context_t)
{
      ಆದರೆ (ಪೋರ್ಟ್.ಪೋರ್ಟ್() > 0) {
	      // Get the integer value of the bit vector, or 0 if
	      // there are X or Z bits.
	    vvp_time64_t bval = 0;
	      // The following does not work correctly for negative values.
	      // They should be sign extended to 64 bits (1364-2001 9.7.1).
	    vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(ಬಿಟ್, bval);
	      // Integer values do not need to be rounded so just scale them.
	    vvp_time64_t val = bval * ಸುತ್ತು * scale_;

	    ಆಯ್ಕೆ (ಪೋರ್ಟ್.ಪೋರ್ಟ್()) {
		ಸ್ಥಿತಿ 1:
		  delay_.ಗಣ_rise(val);
		  ಹಿಂತಿರುಗಿ;
		ಸ್ಥಿತಿ 2:
		  delay_.ಗಣ_fall(val);
		  ಹಿಂತಿರುಗಿ;
		ಸ್ಥಿತಿ 3:
		  delay_.ಗಣ_decay(val);
		  ಹಿಂತಿರುಗಿ;
	    }
	    ಹಿಂತಿರುಗಿ;
      }

      vvp_time64_t use_delay;
	/* This is an initial value so it needs to be compared to all the
	   bits (the order the bits are changed is not deterministic). */
      ಆದರೆ (initial_) {
	    type_ = VEC4_DELAY;
            cur_vec8_ = vvp_vector8_t(vvp_vector4_t(0, BIT4_X), 6, 6);
	    vvp_bit4_t cur_val = cur_vec4_.value(0);
	    use_delay = delay_.get_delay(cur_val, ಬಿಟ್.value(0));
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 1 ;  idx < ಬಿಟ್.size() ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(cur_val, ಬಿಟ್.value(idx));
		  ಆದರೆ (tmp > use_delay) use_delay = tmp;
	    }
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಪ್ರತಿಜ್ಞಾ(type_ == VEC4_DELAY);

	      // Use as a reference for calculating the delay the
	      // current value of the output. Detect and handle the
	      // special case that the event list contains the current
	      // value as a zero-delay-remaining event.
	    ಸ್ಥಿರಾಂಕ vvp_vector4_t&use_vec4 = (list_ && list_->ಮುಂದಿನ_2->sim_ಸಮಯ == ವೇಳಾಪಟ್ಟಿ_simtime())? list_->ಮುಂದಿನ_2->ptr_vec4 : cur_vec4_;

	      /* How many bits to compare? */
	    ಚಿಹ್ನೆರಹಿತ use_wid = use_vec4.size();
	    ಆದರೆ (ಬಿಟ್.size() < use_wid) use_wid = ಬಿಟ್.size();

	      /* Scan the vectors looking for delays. Select the maximum
	         delay encountered. */
	    use_delay = delay_.get_delay(use_vec4.value(0), ಬಿಟ್.value(0));

	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 1 ;  idx < use_wid ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(use_vec4.value(idx), ಬಿಟ್.value(idx));
		  ಆದರೆ (tmp > use_delay) use_delay = tmp;
	    }
      }

      /* what *should* happen here is we check to see if there is a
         transaction in the queue. This would be a pulse that needs to be
         eliminated. */
      ಆದರೆ (clean_pulse_events_(use_delay, ಬಿಟ್)) ಹಿಂತಿರುಗಿ;

      vvp_time64_t use_simtime = ವೇಳಾಪಟ್ಟಿ_simtime() + use_delay;

	/* And propagate it. */
      ಆದರೆ (use_delay == 0 && list_ == 0) {
	    cur_vec4_ = ಬಿಟ್;
	    initial_ = ಸುಳ್ಳು;
	    net_->send_vec4(cur_vec4_, 0);
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ರಚನೆ event_*cur = ಹೊಸ ರಚನೆ event_(use_simtime);
	    cur->ಸತತ_ಸತತ_ptr = &vvp_fun_delay::ಸತತ_ಸತತ_vec4;
	    cur->ptr_vec4 = ಬಿಟ್;
	    ಸರತಿಗೆ_ಸೇರಿಸು(cur);
	    ವೇಳಾಪಟ್ಟಿ_generic(ಇದು, use_delay, ಸುಳ್ಳು);
      }
}

ಶೂನ್ಯ vvp_fun_delay::recv_vec4_pv(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
			         ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ಬಿಟ್, ತಳ, vwid, ctx);
}

/* See the recv_vec4 comment above. */
ಶೂನ್ಯ vvp_fun_delay::recv_vec8(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector8_t&ಬಿಟ್)
{
      ಪ್ರತಿಜ್ಞಾ(ಪೋರ್ಟ್.ಪೋರ್ಟ್() == 0);

      vvp_time64_t use_delay;
	/* This is an initial value so it needs to be compared to all the
	   bits (the order the bits are changed is not deterministic). */
      ಆದರೆ (initial_) {
	    type_ = VEC8_DELAY;
            cur_vec4_ = vvp_vector4_t(0, BIT4_X);
	    vvp_bit4_t cur_val = cur_vec8_.value(0).value();
	    use_delay = delay_.get_delay(cur_val, ಬಿಟ್.value(0).value());
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 1 ;  idx < ಬಿಟ್.size() ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(cur_val, ಬಿಟ್.value(idx).value());
		  ಆದರೆ (tmp > use_delay) use_delay = tmp;
	    }
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಪ್ರತಿಜ್ಞಾ(type_ == VEC8_DELAY);

	      // Use as a reference for calculating the delay the
	      // current value of the output. Detect and handle the
	      // special case that the event list contains the current
	      // value as a zero-delay-remaining event.
	    ಸ್ಥಿರಾಂಕ vvp_vector8_t&use_vec8 = (list_ && list_->ಮುಂದಿನ_2->sim_ಸಮಯ == ವೇಳಾಪಟ್ಟಿ_simtime())? list_->ಮುಂದಿನ_2->ptr_vec8 : cur_vec8_;

	      /* How many bits to compare? */
	    ಚಿಹ್ನೆರಹಿತ use_wid = use_vec8.size();
	    ಆದರೆ (ಬಿಟ್.size() < use_wid) use_wid = ಬಿಟ್.size();

	      /* Scan the vectors looking for delays. Select the maximum
	         delay encountered. */
	    use_delay = delay_.get_delay(use_vec8.value(0).value(),
	                                 ಬಿಟ್.value(0).value());

	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 1 ;  idx < use_wid ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(use_vec8.value(idx).value(),
		                         ಬಿಟ್.value(idx).value());
		  ಆದರೆ (tmp > use_delay) use_delay = tmp;
	    }
      }

      /* what *should* happen here is we check to see if there is a
         transaction in the queue. This would be a pulse that needs to be
         eliminated. */
      ಆದರೆ (clean_pulse_events_(use_delay, ಬಿಟ್)) ಹಿಂತಿರುಗಿ;

      vvp_time64_t use_simtime = ವೇಳಾಪಟ್ಟಿ_simtime() + use_delay;

	/* And propagate it. */
      ಆದರೆ (use_delay == 0 && list_ == 0) {
	    cur_vec8_ = ಬಿಟ್;
	    initial_ = ಸುಳ್ಳು;
	    net_->send_vec8(cur_vec8_);
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ರಚನೆ event_*cur = ಹೊಸ ರಚನೆ event_(use_simtime);
	    cur->ptr_vec8 = ಬಿಟ್;
	    cur->ಸತತ_ಸತತ_ptr = &vvp_fun_delay::ಸತತ_ಸತತ_vec8;
	    ಸರತಿಗೆ_ಸೇರಿಸು(cur);
	    ವೇಳಾಪಟ್ಟಿ_generic(ಇದು, use_delay, ಸುಳ್ಳು);
      }
}

ಶೂನ್ಯ vvp_fun_delay::recv_vec8_pv(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector8_t&ಬಿಟ್,
			         ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid)
{
      recv_vec8_pv_(ptr, ಬಿಟ್, ತಳ, vwid);
}

ಶೂನ್ಯ vvp_fun_delay::recv_real(vvp_net_ptr_t ಪೋರ್ಟ್, ದ್ವಿಗುಣ ಬಿಟ್,
                              vvp_context_t)
{
      ಆದರೆ (ಪೋರ್ಟ್.ಪೋರ್ಟ್() > 0) {
	    /* If the port is not 0, then this is a delay value that
	    should be rounded and converted to an integer delay. */
	    vvp_time64_t val = 0;
	    ಆದರೆ (ಬಿಟ್ > -0.5) {
		  val = (vvp_time64_t) (ಬಿಟ್ * ಸುತ್ತು + 0.5) * scale_;
	    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಬಿಟ್ != ಬಿಟ್) {
		    // For a NaN we use the default (0).
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  vvp_vector4_t vec4(8*ಗಾತ್ರ(vvp_time64_t),
		                     floor(-ಬಿಟ್ * ಸುತ್ತು + 0.5) * -1 * scale_);
		  vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(vec4, val);
	    }

	    ಆಯ್ಕೆ (ಪೋರ್ಟ್.ಪೋರ್ಟ್()) {
		ಸ್ಥಿತಿ 1:
		  delay_.ಗಣ_rise(val);
		  ಹಿಂತಿರುಗಿ;
		ಸ್ಥಿತಿ 2:
		  delay_.ಗಣ_fall(val);
		  ಹಿಂತಿರುಗಿ;
		ಸ್ಥಿತಿ 3:
		  delay_.ಗಣ_decay(val);
		  ಹಿಂತಿರುಗಿ;
	    }
	    ಹಿಂತಿರುಗಿ;
      }

      ಆದರೆ (initial_) {
	    type_ = REAL_DELAY;
            cur_vec4_ = vvp_vector4_t(0, BIT4_X);
            cur_vec8_ = vvp_vector8_t(cur_vec4_, 6, 6);
      } ಇಲ್ಲದಿದ್ದರೆ ಪ್ರತಿಜ್ಞಾ(type_ == REAL_DELAY);

      vvp_time64_t use_delay;
      use_delay = delay_.get_min_delay();

      /* Eliminate glitches. */
      ಆದರೆ (clean_pulse_events_(use_delay, ಬಿಟ್)) ಹಿಂತಿರುಗಿ;

      /* This must be done after cleaning pulses to avoid propagating
       * an incorrect value. */
      ಆದರೆ (cur_real_ == ಬಿಟ್) ಹಿಂತಿರುಗಿ;

      vvp_time64_t use_simtime = ವೇಳಾಪಟ್ಟಿ_simtime() + use_delay;

      ಆದರೆ (use_delay == 0 && list_ == 0) {
	    cur_real_ = ಬಿಟ್;
	    initial_ = ಸುಳ್ಳು;
	    net_->send_real(cur_real_, 0);
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ರಚನೆ event_*cur = ಹೊಸ ರಚನೆ event_(use_simtime);
	    cur->ಸತತ_ಸತತ_ptr = &vvp_fun_delay::ಸತತ_ಸತತ_real;
	    cur->ptr_real = ಬಿಟ್;
	    ಸರತಿಗೆ_ಸೇರಿಸು(cur);

	    ವೇಳಾಪಟ್ಟಿ_generic(ಇದು, use_delay, ಸುಳ್ಳು);
      }
}

ಶೂನ್ಯ vvp_fun_delay::ಸತತ_ಸತತ()
{
      vvp_time64_t sim_ಸಮಯ = ವೇಳಾಪಟ್ಟಿ_simtime();
      ಆದರೆ (list_ == 0 || list_->ಮುಂದಿನ_2->sim_ಸಮಯ > sim_ಸಮಯ)
	    ಹಿಂತಿರುಗಿ;

      ರಚನೆ event_*cur = ಸರತಿಯಿಂದ_ತೆಗೆ();
      ಆದರೆ (cur == 0)
	    ಹಿಂತಿರುಗಿ;

      (ಇದು->*(cur->ಸತತ_ಸತತ_ptr))(cur);
      initial_ = ಸುಳ್ಳು;
      ಅಳಿಸು cur;
}

ಶೂನ್ಯ vvp_fun_delay::ಸತತ_ಸತತ_vec4(ಸ್ಥಿರಾಂಕ ರಚನೆ event_*cur)
{
      cur_vec4_ = cur->ptr_vec4;
      net_->send_vec4(cur_vec4_, 0);
}

ಶೂನ್ಯ vvp_fun_delay::ಸತತ_ಸತತ_vec8(ಸ್ಥಿರಾಂಕ ರಚನೆ vvp_fun_delay::event_*cur)
{
      cur_vec8_ = cur->ptr_vec8;
      net_->send_vec8(cur_vec8_);
}

ಶೂನ್ಯ vvp_fun_delay::ಸತತ_ಸತತ_real(ಸ್ಥಿರಾಂಕ ರಚನೆ vvp_fun_delay::event_*cur)
{
      cur_real_ = cur->ptr_real;
      net_->send_real(cur_real_, 0);
}

vvp_fun_modpath::vvp_fun_modpath(vvp_net_t*net, ಚಿಹ್ನೆರಹಿತ width)
: net_(net), src_list_(0), ifnone_list_(0)
{
      cur_vec4_ = vvp_vector4_t(width, BIT4_X);
      ವೇಳಾಪಟ್ಟಿ_init_propagate(net_, cur_vec4_);
}

vvp_fun_modpath::~vvp_fun_modpath()
{
	// Delete the source probes.
      ಆಗ (src_list_) {
	    vvp_fun_modpath_src*tmp = src_list_;
	    src_list_ = tmp->ಮುಂದಿನ;
	    ಅಳಿಸು tmp;
      }
      ಆಗ (ifnone_list_) {
	    vvp_fun_modpath_src*tmp = ifnone_list_;
	    ifnone_list_ = tmp->ಮುಂದಿನ;
	    ಅಳಿಸು tmp;
      }
}

ಶೂನ್ಯ vvp_fun_modpath::ಕೂಡಿಸು_modpath_src(vvp_fun_modpath_src*that, ತಾರ್ಕಿಕ ifnone)
{
      ಪ್ರತಿಜ್ಞಾ(that->ಮುಂದಿನ == 0);
      ಆದರೆ (ifnone) {
	    that->ಮುಂದಿನ = ifnone_list_;
	    ifnone_list_ = that;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    that->ಮುಂದಿನ = src_list_;
	    src_list_ = that;
      }
}

ಸ್ಥಿರ vvp_time64_t delay_from_ಅಂಚು(vvp_bit4_t a, vvp_bit4_t b,
                                           vvp_time64_t ಸ್ಥಿರಾಂಕ array[12])
{
      ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ delay_ಅಂಚು_t bit4_table4[4];
      ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ bit4_table4 ಅಂಚು_ಕೋಷ್ಟಕ[4] = {
	    { DELAY_ಅಂಚು_01, DELAY_ಅಂಚು_01, DELAY_ಅಂಚು_0z, DELAY_ಅಂಚು_0x },
	    { DELAY_ಅಂಚು_10, DELAY_ಅಂಚು_10, DELAY_ಅಂಚು_1z, DELAY_ಅಂಚು_1x },
	    { DELAY_ಅಂಚು_z0, DELAY_ಅಂಚು_z1, DELAY_ಅಂಚು_z0, DELAY_ಅಂಚು_zx },
	    { DELAY_ಅಂಚು_x0, DELAY_ಅಂಚು_x1, DELAY_ಅಂಚು_xz, DELAY_ಅಂಚು_x0 }
      };

      ಹಿಂತಿರುಗಿ array[ ಅಂಚು_ಕೋಷ್ಟಕ[a][b] ];
}

ಶೂನ್ಯ vvp_fun_modpath::recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                                vvp_context_t)
{
	/* Only the first port is used. */
      ಆದರೆ (ಪೋರ್ಟ್.ಪೋರ್ಟ್() > 0)
	    ಹಿಂತಿರುಗಿ;

      ಆದರೆ (cur_vec4_.eeq(ಬಿಟ್))
	    ಹಿಂತಿರುಗಿ;

	/* Select a time delay source that applies. Notice that there
	   may be multiple delay sources that apply, so collect all
	   the candidates into a list first. */
      list<vvp_fun_modpath_src*>candidate_list;
      vvp_time64_t candidate_wake_ಸಮಯ = 0;
      ಗಾಗಿ (vvp_fun_modpath_src*cur = src_list_ ;  cur ;  cur=cur->ಮುಂದಿನ) {
	      /* Skip paths that are disabled by conditions. */
	    ಆದರೆ (cur->condition_flag_ == ಸುಳ್ಳು)
		  ಮುಂದುವರಿ;

	    ಆದರೆ (candidate_list.empty()) {
		  candidate_list.push_back(cur);
		  candidate_wake_ಸಮಯ = cur->wake_ಸಮಯ;
	    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cur->wake_ಸಮಯ == candidate_wake_ಸಮಯ) {
		  candidate_list.push_back(cur);
	    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cur->wake_ಸಮಯ > candidate_wake_ಸಮಯ) {
		  candidate_list.assign(1, cur);
		  candidate_wake_ಸಮಯ = cur->wake_ಸಮಯ;
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  ಮುಂದುವರಿ; /* Skip this entry. */
	    }
      }

	/* Only add the ifnone delay if it has a later wake_time_ or
	 * if there are no normal delays. */
      vvp_time64_t ifnone_wake_ಸಮಯ = candidate_wake_ಸಮಯ;
      ಗಾಗಿ (vvp_fun_modpath_src*cur = ifnone_list_ ;  cur ;  cur=cur->ಮುಂದಿನ) {
	    ಆದರೆ (candidate_list.empty()) {
		  candidate_list.push_back(cur);
		  ifnone_wake_ಸಮಯ = cur->wake_ಸಮಯ;
	    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cur->wake_ಸಮಯ == ifnone_wake_ಸಮಯ &&
	               ifnone_wake_ಸಮಯ > candidate_wake_ಸಮಯ) {
		  candidate_list.push_back(cur);
	    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cur->wake_ಸಮಯ > ifnone_wake_ಸಮಯ) {
		  candidate_list.assign(1, cur);
		  ifnone_wake_ಸಮಯ = cur->wake_ಸಮಯ;
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  ಮುಂದುವರಿ; /* Skip this entry. */
	    }
      }

	/* Handle the special case that there are no delays that
	   match. This may happen, for example, if the set of
	   conditional delays is incomplete, leaving some cases
	   uncovered. In that case, just pass the data without delay */
      ಆದರೆ (candidate_list.empty()) {
	    cur_vec4_ = ಬಿಟ್;
	    ವೇಳಾಪಟ್ಟಿ_generic(ಇದು, 0, ಸುಳ್ಳು);
	    ಹಿಂತಿರುಗಿ;
      }

	/* Now given that we have a list of candidate delays, find for
	   each if the 12 numbers the minimum from all the
	   candidates. This minimum set becomes the chosen delay to
	   use. */
      vvp_time64_t ಹೊರಹೋಗು_at[12];
      vvp_time64_t now = ವೇಳಾಪಟ್ಟಿ_simtime();

      ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ list<vvp_fun_modpath_src*>::const_iterator iter_t;

      iter_t cur = candidate_list.begin();
      ಸ್ಥಿರಾಂಕ vvp_fun_modpath_src*src = *cur;

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < 12 ;  idx += 1) {
	    ಹೊರಹೋಗು_at[idx] = src->wake_ಸಮಯ + src->delay_[idx];
	    ಆದರೆ (ಹೊರಹೋಗು_at[idx] <= now)
		  ಹೊರಹೋಗು_at[idx] = 0;
	    ಇಲ್ಲದಿದ್ದರೆ
		  ಹೊರಹೋಗು_at[idx] -= now;
      }

      ಗಾಗಿ (++ cur ; cur != candidate_list.ಕೊನೆ() ; ++ cur ) {
	    src = *cur;
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < 12 ;  idx += 1) {
		  vvp_time64_t tmp = src->wake_ಸಮಯ + src->delay_[idx];
		  ಆದರೆ (tmp <= now)
			tmp = 0;
		  ಇಲ್ಲದಿದ್ದರೆ
			tmp -= now;
		  ಆದರೆ (tmp < ಹೊರಹೋಗು_at[idx])
			ಹೊರಹೋಗು_at[idx] = tmp;
	    }
      }

	/* Given the scheduled output time, create an output event. */
      vvp_time64_t use_delay = delay_from_ಅಂಚು(cur_vec4_.value(0),
					       ಬಿಟ್.value(0),
					       ಹೊರಹೋಗು_at);

	/* FIXME: This bases the edge delay on only the least
	   bit. This is WRONG! I need to find all the possible delays,
	   and schedule an event for each partial change. Hard! */
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 1 ;  idx < ಬಿಟ್.size() ;  idx += 1) {
	    vvp_time64_t tmp = delay_from_ಅಂಚು(cur_vec4_.value(idx),
					       ಬಿಟ್.value(idx),
					       ಹೊರಹೋಗು_at);
	      /* If the current and new bit values match then no delay
	       * is needed for this bit. */
	    ಆದರೆ (cur_vec4_.value(idx) == ಬಿಟ್.value(idx)) ಮುಂದುವರಿ;
	    ಆದರೆ (tmp != use_delay) {
		  fprintf(stderr, "sorry: multi-bit module path delays are "
		                  "currently not fully supported.\n");
		  fprintf(stderr, "     : using the LSB delay for all bits.\n");
		  ಮುಂದುವರಿ;
            }
      }

      cur_vec4_ = ಬಿಟ್;
      ವೇಳಾಪಟ್ಟಿ_generic(ಇದು, use_delay, ಸುಳ್ಳು);
}

ಶೂನ್ಯ vvp_fun_modpath::ಸತತ_ಸತತ()
{
      net_->send_vec4(cur_vec4_, 0);
}

vvp_fun_modpath_src::vvp_fun_modpath_src(vvp_time64_t ಸ್ಥಿರಾಂಕ del[12])
{
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = del[idx];

      ಮುಂದಿನ = 0;
      wake_ಸಮಯ = 0;
      condition_flag_ = ನಿಜ;
}

vvp_fun_modpath_src::~vvp_fun_modpath_src()
{
}

ಶೂನ್ಯ vvp_fun_modpath_src::get_delay12(vvp_time64_t val[12]) ಸ್ಥಿರಾಂಕ
{
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < 12 ;  idx += 1)
	    val[idx] = delay_[idx];
}

ಶೂನ್ಯ vvp_fun_modpath_src::put_delay12(ಸ್ಥಿರಾಂಕ vvp_time64_t val[12])
{
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = val[idx];
}

ಶೂನ್ಯ vvp_fun_modpath_src::recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                                    vvp_context_t)
{
      ಆದರೆ (ಪೋರ್ಟ್.ಪೋರ್ಟ್() == 0) {
	      // The modpath input...
	    ಆದರೆ (test_vec4(ಬಿಟ್))
		  wake_ಸಮಯ = ವೇಳಾಪಟ್ಟಿ_simtime();

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಪೋರ್ಟ್.ಪೋರ್ಟ್() == 1) {
	      // The modpath condition input...
	    ಆದರೆ (ಬಿಟ್.value(0) != BIT4_0)
		  condition_flag_ = ನಿಜ;
	    ಇಲ್ಲದಿದ್ದರೆ
		  condition_flag_ = ಸುಳ್ಳು;
      }
}

ತಾರ್ಕಿಕ vvp_fun_modpath_src::test_vec4(ಸ್ಥಿರಾಂಕ vvp_vector4_t&)
{
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

vvp_fun_modpath_ಅಂಚು::vvp_fun_modpath_ಅಂಚು(vvp_time64_t del[12],
					   ತಾರ್ಕಿಕ pos, ತಾರ್ಕಿಕ neg)
: vvp_fun_modpath_src(del)
{
      old_ಮೌಲ್ಯ = BIT4_X;
      posedge_ = pos;
      negedge_ = neg;
}

ತಾರ್ಕಿಕ vvp_fun_modpath_ಅಂಚು::test_vec4(ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್)
{
      vvp_bit4_t tmp = old_ಮೌಲ್ಯ;
      old_ಮೌಲ್ಯ = ಬಿಟ್.value(0);

      ಪೂರ್ಣಾಂಕ ಅಂಚು_flag = ಅಂಚು(tmp, old_ಮೌಲ್ಯ);
      ಆದರೆ (ಅಂಚು_flag > 0) ಹಿಂತಿರುಗಿ posedge_;
      ಆದರೆ (ಅಂಚು_flag < 0) ಹಿಂತಿರುಗಿ negedge_;
      ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}


/*
 * All the below routines that begin with
 * modpath_src_* belong the internal function
 * of an vpiModPathIn object. This is used to
 * make some specific delays path operations
 *
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ modpath_src_get(ಪೂರ್ಣಾಂಕ, vpiHandle ref)
{
      ರಚನೆ __vpiModPathSrc*obj =dynamic_cast<__vpiModPathSrc*>(ref);
      ಪ್ರತಿಜ್ಞಾ(obj);
      ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ modpath_src_get_ಮೌಲ್ಯ(vpiHandle ref, p_vpi_ಮೌಲ್ಯ)
{
      ರಚನೆ __vpiModPathSrc* modpathsrc = dynamic_cast<__vpiModPathSrc*>(ref);
      ಪ್ರತಿಜ್ಞಾ(modpathsrc);
      ಹಿಂತಿರುಗಿ;
}

ಸ್ಥಿರ vpiHandle modpath_src_put_ಮೌಲ್ಯ(vpiHandle ref, s_vpi_ಮೌಲ್ಯ *, ಪೂರ್ಣಾಂಕ )
{
      ರಚನೆ __vpiModPathSrc* modpathsrc = dynamic_cast<__vpiModPathSrc*>(ref);
      ಪ್ರತಿಜ್ಞಾ(modpathsrc);
      ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ vpiHandle modpath_src_get_handle(ಪೂರ್ಣಾಂಕ code, vpiHandle ref)
{
      ರಚನೆ __vpiModPathSrc*rfp = dynamic_cast<__vpiModPathSrc*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ಆಯ್ಕೆ (code) {

	ಸ್ಥಿತಿ vpiScope:
	  ಹಿಂತಿರುಗಿ rfp->dest->scope;

	  ಸ್ಥಿತಿ vpiModule:
	      { __vpiScope*scope = rfp->dest->scope;
		ಆಗ (scope && scope->get_type_ಸಂಕೇತ() != vpiModule)
		      scope = scope->scope;
		ಪ್ರತಿಜ್ಞಾ(scope);
		ಹಿಂತಿರುಗಿ scope;
	      }

	    // Handles to path term objects should really be obtained via
	    // the vpi_iterate and vpi_scan functions. Continue to allow
	    // them to be obtained here for backwards compatibility with
	    // older versions of Icarus Verilog.

	  ಸ್ಥಿತಿ vpi_Mod_ಮಾರ್ಗ_ಒಳ:
	    ಹಿಂತಿರುಗಿ &rfp->ಮಾರ್ಗ_ಪದ_ಒಳ;

	  ಸ್ಥಿತಿ vpi_Mod_ಮಾರ್ಗ_ಹೊರಹೋಗು:
	    ಹಿಂತಿರುಗಿ &rfp->dest->ಮಾರ್ಗ_ಪದ_ಹೊರಹೋಗು;
      }
      ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ vpiHandle modpath_src_iterate(ಪೂರ್ಣಾಂಕ code, vpiHandle ref)
{
      ರಚನೆ __vpiModPathSrc*rfp = dynamic_cast<__vpiModPathSrc*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

	// Module paths with multiple sources or destinations are
	// currently represented by a separate modpath object for
	// each source/destination combination, so there is only
	// ever one input path term and one output path term.
      ಆಯ್ಕೆ (code) {
	  ಸ್ಥಿತಿ vpi_Mod_ಮಾರ್ಗ_ಒಳ: {
	    vpiHandle*ವಾದಗಳು = static_cast<vpiHandle*>(ಸೊನ್ನೆ_ಸ್ಮರಣೆ_ಕೊಡು(1, ಗಾತ್ರ(vpiHandle*)));
	    ವಾದಗಳು[0] = &rfp->ಮಾರ್ಗ_ಪದ_ಒಳ;
	    ಹಿಂತಿರುಗಿ vpip_make_iterator(1, ವಾದಗಳು, ನಿಜ);
	  }
	  ಸ್ಥಿತಿ vpi_Mod_ಮಾರ್ಗ_ಹೊರಹೋಗು: {
	    vpiHandle*ವಾದಗಳು = static_cast<vpiHandle*>(ಸೊನ್ನೆ_ಸ್ಮರಣೆ_ಕೊಡು(1, ಗಾತ್ರ(vpiHandle*)));
	    ವಾದಗಳು[0] = &rfp->dest->ಮಾರ್ಗ_ಪದ_ಹೊರಹೋಗು;
	    ಹಿಂತಿರುಗಿ vpip_make_iterator(1, ವಾದಗಳು, ನಿಜ);
	  }
      }
      ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ vpiHandle modpath_src_ಸೂಚ್ಯಂಕ ( vpiHandle ref, ಪೂರ್ಣಾಂಕ)
{
      ಪ್ರತಿಜ್ಞಾ(ref->get_type_ಸಂಕೇತ() == vpi_Mod_ಮಾರ್ಗ_ಒಳ);
      ಹಿಂತಿರುಗಿ 0;
}


/*
 * This routine will put specific dimension of delay[] values
 * into a vpiHandle. In this case, we will put
 * specific delays values in a vpiModPathIn object
 *
 */
ಸ್ಥಿರ ಶೂನ್ಯ modpath_src_put_delays (vpiHandle ref, p_vpi_delay delays)
{
      vvp_time64_t tmp[12];
      ಪೂರ್ಣಾಂಕ idx;
      ರಚನೆ __vpiModPathSrc * src = dynamic_cast<__vpiModPathSrc*>(ref) ;
      ಪ್ರತಿಜ್ಞಾ(src) ;

      vvp_fun_modpath_src *fun = dynamic_cast<vvp_fun_modpath_src*>(src->net->fun);
      ಪ್ರತಿಜ್ಞಾ( fun );

      ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ map_array_t[12];
	// Only the first six entries are used for the less than twelve maps.
      ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ map_array_t map_1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ map_array_t map_2 = {0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0};
      ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ map_array_t map_3 = {0, 1, 2, 0, 2, 1, 0, 0, 0, 0, 0, 0};
      ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ map_array_t map_6 = {0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0};
      ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ map_array_t map12 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

      ಸ್ಥಿರಾಂಕ map_array_t*use_map = 0;
      ಆಯ್ಕೆ (delays->no_ದ_delays) {
	  ಸ್ಥಿತಿ 1:
	    use_map = &map_1;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 2:
	    use_map = &map_2;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 3:
	    use_map = &map_3;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 6:
	    use_map = &map_6;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 12:
	    use_map = &map12;
	    ನಿಲ್ಲಿಸು;
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ನಿಲ್ಲಿಸು;
      }

      ಆದರೆ (delays->ಸಮಯ_type == vpi_Sim_ಸಮಯ) {
	    ಗಾಗಿ (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_timestruct_ಗಮ್ಯ_ಸಮಯ(delays->da+use_map[0][idx]);
	    }
      } ಇಲ್ಲದಿದ್ದರೆ {
	      // You cannot create a modpath with a negative delay so set it
	      // to zero per 1364-2005 section 14.3.1.
	    ಗಾಗಿ (idx = 0 ; idx < delays->no_ದ_delays ; idx += 1) {
		  ಆದರೆ (delays->da[idx].real < 0.0) delays->da[idx].real = 0.0;
	    }
	    ಗಾಗಿ (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_scaled_real_ಗಮ್ಯ_time64(delays->da[use_map[0][idx]].real,
							src->dest->scope);
	    }
      }

      /* Now define the to-from-x delays if needed. */
      ಆದರೆ (delays->no_ದ_delays <= 6) {
	      /* 0->x is the minimum of 0->z and 0->1. */
	    tmp[DELAY_ಅಂಚು_0x] = tmp[DELAY_ಅಂಚು_0z] < tmp[DELAY_ಅಂಚು_01] ?
	                           tmp[DELAY_ಅಂಚು_0z] : tmp[DELAY_ಅಂಚು_01];
	      /* x->1 is the maximum of z->1 and 0->1. */
	    tmp[DELAY_ಅಂಚು_x1] = tmp[DELAY_ಅಂಚು_z1] > tmp[DELAY_ಅಂಚು_01] ?
	                           tmp[DELAY_ಅಂಚು_z1] : tmp[DELAY_ಅಂಚು_01];
	      /* 1->x is the minimum of 1->z and 1->0. */
	    tmp[DELAY_ಅಂಚು_1x] = tmp[DELAY_ಅಂಚು_1z] < tmp[DELAY_ಅಂಚು_10] ?
	                           tmp[DELAY_ಅಂಚು_1z] : tmp[DELAY_ಅಂಚು_10];
	      /* x->0 is the maximum of z->0 and 1->0. */
	    tmp[DELAY_ಅಂಚು_x0] = tmp[DELAY_ಅಂಚು_z0] > tmp[DELAY_ಅಂಚು_10] ?
	                           tmp[DELAY_ಅಂಚು_z0] : tmp[DELAY_ಅಂಚು_10];
	      /* x->z is the maximum of 1->z and 0->z. */
	    tmp[DELAY_ಅಂಚು_xz] = tmp[DELAY_ಅಂಚು_1z] > tmp[DELAY_ಅಂಚು_0z] ?
	                           tmp[DELAY_ಅಂಚು_1z] : tmp[DELAY_ಅಂಚು_0z];
	      /* z->x is the minimum of z->1 and z->0. */
	    tmp[DELAY_ಅಂಚು_zx] = tmp[DELAY_ಅಂಚು_z1] < tmp[DELAY_ಅಂಚು_z0] ?
	                           tmp[DELAY_ಅಂಚು_z1] : tmp[DELAY_ಅಂಚು_z0];
      }

      fun->put_delay12(tmp);
}

/*
 * This routine will retrieve the delay[12] values
 * of a vpiHandle. In this case, he will get an
 * specific delays values from a vpiModPathIn
 * object
 *
 */

ಸ್ಥಿರ ಶೂನ್ಯ modpath_src_get_delays ( vpiHandle ref, p_vpi_delay delays )
{
      ರಚನೆ __vpiModPathSrc*src = dynamic_cast<__vpiModPathSrc*>(ref) ;
      ಪ್ರತಿಜ್ಞಾ(src);

      vvp_fun_modpath_src *fun = dynamic_cast<vvp_fun_modpath_src*>(src->net->fun);
      ಪ್ರತಿಜ್ಞಾ(fun);

      ಪೂರ್ಣಾಂಕ idx;
      vvp_time64_t tmp[12];
      fun->get_delay12(tmp);

      ಆಯ್ಕೆ (delays->no_ದ_delays) {
	  ಸ್ಥಿತಿ 1:
	  ಸ್ಥಿತಿ 2:
	  ಸ್ಥಿತಿ 3:
	  ಸ್ಥಿತಿ 6:
	  ಸ್ಥಿತಿ 12:
	    ನಿಲ್ಲಿಸು;

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ನಿಲ್ಲಿಸು;
      }

      ಆದರೆ (delays->ಸಮಯ_type == vpi_Sim_ಸಮಯ) {
	    ಗಾಗಿ (idx = 0; idx < delays->no_ದ_delays; idx += 1) {
		  vpip_ಸಮಯ_ಗಮ್ಯ_timestruct(delays->da+idx, tmp[idx]);
	    }
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಗಾಗಿ (idx = 0; idx < delays->no_ದ_delays; idx += 1) {
		  delays->da[idx].real = vpip_ಸಮಯ_ಗಮ್ಯ_scaled_real(tmp[idx], src->dest->scope);
	    }
      }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pathterm_get(ಪೂರ್ಣಾಂಕ code, vpiHandle ref)
{
      ರಚನೆ __vpiModPathTerm*obj = dynamic_cast<__vpiModPathTerm*>(ref);
      ಪ್ರತಿಜ್ಞಾ(obj);

      ಆಯ್ಕೆ (code) {
	  ಸ್ಥಿತಿ vpi_ಅಂಚು_2:
	    ಹಿಂತಿರುಗಿ obj->ಅಂಚು;
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ 0;
      }
}

ಸ್ಥಿರ vpiHandle pathterm_get_handle(ಪೂರ್ಣಾಂಕ code, vpiHandle ref)
{
      ರಚನೆ __vpiModPathTerm*obj = dynamic_cast<__vpiModPathTerm*>(ref);
      ಪ್ರತಿಜ್ಞಾ(obj);

      ಆಯ್ಕೆ (code) {
	  ಸ್ಥಿತಿ vpi_ಅಭಿವ್ಯಕ್ತಿ:
	    ಹಿಂತಿರುಗಿ obj->ಅಭಿವ್ಯಕ್ತಿ;
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ 0;
      }
}

/*
* The __vpiModPathSrc class is what the VPI client sees as a
* vpiModPath object. The __vpiModPath structure contains items that
* are common to a bunch of modpaths, including the destination term.
*/
ಸಾಲೊಳಗೆ __vpiModPathSrc::__vpiModPathSrc()
{ }

ಪೂರ್ಣಾಂಕ __vpiModPathSrc::get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ
{ ಹಿಂತಿರುಗಿ vpi_Mod_ಮಾರ್ಗ; }

ಪೂರ್ಣಾಂಕ __vpiModPathSrc::vpi_get(ಪೂರ್ಣಾಂಕ code)
{ ಹಿಂತಿರುಗಿ modpath_src_get(code, ಇದು); }

ಶೂನ್ಯ __vpiModPathSrc::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val)
{ modpath_src_get_ಮೌಲ್ಯ(ಇದು, val); }

vpiHandle __vpiModPathSrc::vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಂಕ flags)
{ ಹಿಂತಿರುಗಿ modpath_src_put_ಮೌಲ್ಯ(ಇದು, val, flags); }

vpiHandle __vpiModPathSrc::vpi_handle(ಪೂರ್ಣಾಂಕ code)
{ ಹಿಂತಿರುಗಿ modpath_src_get_handle(code, ಇದು); }

vpiHandle __vpiModPathSrc::vpi_iterate(ಪೂರ್ಣಾಂಕ code)
{ ಹಿಂತಿರುಗಿ modpath_src_iterate(code, ಇದು); }

vpiHandle __vpiModPathSrc:: vpi_ಸೂಚ್ಯಂಕ(ಪೂರ್ಣಾಂಕ idx)
{ ಹಿಂತಿರುಗಿ modpath_src_ಸೂಚ್ಯಂಕ(ಇದು, idx); }

ಶೂನ್ಯ __vpiModPathSrc::vpi_get_delays(p_vpi_delay del)
{ modpath_src_get_delays(ಇದು, del); }

ಶೂನ್ಯ __vpiModPathSrc::vpi_put_delays(p_vpi_delay del)
{ modpath_src_put_delays(ಇದು, del); }

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ modpath_src_free_object( vpiHandle ref )
{
      ಅಳಿಸು ref;
      ಹಿಂತಿರುಗಿ 1 ;
}

__vpiHandle::free_object_fun_t __vpiModPathSrc::free_object_fun(ಶೂನ್ಯ)
{ ಹಿಂತಿರುಗಿ &modpath_src_free_object; }


ಸಾಲೊಳಗೆ __vpiModPathTerm::__vpiModPathTerm()
{ }

ಪೂರ್ಣಾಂಕ __vpiModPathTerm::get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ
{ ಹಿಂತಿರುಗಿ vpi_ಮಾರ್ಗ_ಪದ; }

ಪೂರ್ಣಾಂಕ __vpiModPathTerm::vpi_get(ಪೂರ್ಣಾಂಕ code)
{ ಹಿಂತಿರುಗಿ pathterm_get(code, ಇದು); }

vpiHandle __vpiModPathTerm::vpi_handle(ಪೂರ್ಣಾಂಕ code)
{ ಹಿಂತಿರುಗಿ pathterm_get_handle(code, ಇದು); }

ಸ್ಥಿರ ಶೂನ್ಯ initialize_ಮಾರ್ಗ_ಪದ(ರಚನೆ __vpiModPathTerm&obj)
{
      obj.ಅಭಿವ್ಯಕ್ತಿ = 0;
      obj.ಅಂಚು = vpi_No_ಅಂಚು;
}

/*
 * This function will construct a vpiModPath Object.
 * give a respective "net", and will point to his
 * respective functor
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಸ್ಥಿರ ರಚನೆ __vpiModPath**mp_list = 0;
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ mp_ಎಣಿಕೆ = 0;
#ಆದರೆ_ಅಂತ್ಯ

ರಚನೆ __vpiModPath* vpip_make_modpath(vvp_net_t *net)
{
      ರಚನೆ __vpiModPath*obj = ಹೊಸ __vpiModPath;
      obj->scope = vpip_peek_current_scope ( );

      initialize_ಮಾರ್ಗ_ಪದ(obj->ಮಾರ್ಗ_ಪದ_ಹೊರಹೋಗು);
      obj->ಒಳಹರಿವು_net = net ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
      mp_ಎಣಿಕೆ += 1;
      mp_list = static_cast<ರಚನೆ __vpiModPath **>
                (realloc(mp_list,
                 mp_ಎಣಿಕೆ*ಗಾತ್ರ(ರಚನೆ __vpiModPath **)));
      mp_list[mp_ಎಣಿಕೆ-1] = obj;
#ಆದರೆ_ಅಂತ್ಯ
      ಹಿಂತಿರುಗಿ obj;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಶೂನ್ಯ modpath_delete()
{
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0; idx < mp_ಎಣಿಕೆ; idx += 1) {
	    ಅಳಿಸು mp_list[idx];
      }
      ಬಿಡುಗಡೆ(mp_list);
      mp_list = 0;
      mp_ಎಣಿಕೆ = 0;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * This function will construct a vpiModPathIn
 * ( struct __vpiModPathSrc ) Object. will give
 * a delays[12] values, and point to the specified functor
 *
 */

ರಚನೆ __vpiModPathSrc* vpip_make_modpath_src(ರಚನೆ __vpiModPath*ಮಾರ್ಗ,
                                              vvp_net_t *net)
{
      ರಚನೆ __vpiModPathSrc *obj = ಹೊಸ __vpiModPathSrc;

      obj->dest = ಮಾರ್ಗ;
      obj->type = 0;
      obj->net = net;
      initialize_ಮಾರ್ಗ_ಪದ(obj->ಮಾರ್ಗ_ಪದ_ಒಳ);

      ಹಿಂತಿರುಗಿ obj;
}

vvp_fun_intermodpath::vvp_fun_intermodpath(vvp_net_t*net, ಚಿಹ್ನೆರಹಿತ width)
: net_(net)
{
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = 0;

      cur_vec4_ = vvp_vector4_t(width, BIT4_X);
      ವೇಳಾಪಟ್ಟಿ_init_propagate(net_, cur_vec4_);
}

vvp_fun_intermodpath::~vvp_fun_intermodpath()
{
}

ಶೂನ್ಯ vvp_fun_intermodpath::get_delay12(vvp_time64_t val[12]) ಸ್ಥಿರಾಂಕ
{
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < 12 ;  idx += 1)
	    val[idx] = delay_[idx];
}

ಶೂನ್ಯ vvp_fun_intermodpath::put_delay12(ಸ್ಥಿರಾಂಕ vvp_time64_t val[12])
{
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = val[idx];
}

/*
 * FIXME: this implementation currently only uses the maximum delay
 * from all the bit changes in the vectors. If there are multiple
 * changes with different delays, then the results would be
 * wrong. What should happen is that if there are multiple changes,
 * multiple vectors approaching the result should be scheduled.
 */
ಶೂನ್ಯ vvp_fun_intermodpath::recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                              vvp_context_t)
{
	/* Only the first port is used. */
      ಆದರೆ (ಪೋರ್ಟ್.ಪೋರ್ಟ್() > 0)
	    ಹಿಂತಿರುಗಿ;

      ಆದರೆ (cur_vec4_.eeq(ಬಿಟ್))
	    ಹಿಂತಿರುಗಿ;

	/* Given the scheduled output time, create an output event. */
      vvp_time64_t use_delay = delay_from_ಅಂಚು(cur_vec4_.value(0),
					       ಬಿಟ್.value(0),
					       delay_);

	/* FIXME: This bases the edge delay on only the least
	   bit. This is WRONG! I need to find all the possible delays,
	   and schedule an event for each partial change. Hard! */
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 1 ;  idx < ಬಿಟ್.size() ;  idx += 1) {
	    vvp_time64_t tmp = delay_from_ಅಂಚು(cur_vec4_.value(idx),
					       ಬಿಟ್.value(idx),
					       delay_);
	      /* If the current and new bit values match then no delay
	       * is needed for this bit. */
	    ಆದರೆ (cur_vec4_.value(idx) == ಬಿಟ್.value(idx)) ಮುಂದುವರಿ;
	    ಪ್ರತಿಜ್ಞಾ(tmp == use_delay);
      }

      cur_vec4_ = ಬಿಟ್;
      ವೇಳಾಪಟ್ಟಿ_generic(ಇದು, use_delay, ಸುಳ್ಳು);
}

ಶೂನ್ಯ vvp_fun_intermodpath::ಸತತ_ಸತತ()
{
      net_->send_vec4(cur_vec4_, 0);
}

/*
 * All the below routines that begin with
 * intermodpath_* belong the internal function
 * of an vpiInterModPath object. This is used to
 * make some specific delays path operations
 *
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ intermodpath_get(ಪೂರ್ಣಾಂಕ, vpiHandle ref)
{
      ರಚನೆ __vpiInterModPath*obj =dynamic_cast<__vpiInterModPath*>(ref);
      ಪ್ರತಿಜ್ಞಾ(obj);
      ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ intermodpath_get_ಮೌಲ್ಯ(vpiHandle ref, p_vpi_ಮೌಲ್ಯ)
{
      ರಚನೆ __vpiInterModPath* intermodpath = dynamic_cast<__vpiInterModPath*>(ref);
      ಪ್ರತಿಜ್ಞಾ(intermodpath);
      ಹಿಂತಿರುಗಿ;
}

ಸ್ಥಿರ vpiHandle intermodpath_put_ಮೌಲ್ಯ(vpiHandle ref, s_vpi_ಮೌಲ್ಯ *, ಪೂರ್ಣಾಂಕ )
{
      ರಚನೆ __vpiInterModPath* intermodpath = dynamic_cast<__vpiInterModPath*>(ref);
      ಪ್ರತಿಜ್ಞಾ(intermodpath);
      ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ vpiHandle intermodpath_get_handle(ಪೂರ್ಣಾಂಕ code, vpiHandle ref)
{
      ರಚನೆ __vpiInterModPath*rfp = dynamic_cast<__vpiInterModPath*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ಆಯ್ಕೆ (code) {

	ಸ್ಥಿತಿ vpiScope:
	  ಹಿಂತಿರುಗಿ rfp->scope;

	  ಸ್ಥಿತಿ vpiModule:
	      { __vpiScope*scope = rfp->scope;
		ಆಗ (scope && scope->get_type_ಸಂಕೇತ() != vpiModule)
		      scope = scope->scope;
		ಪ್ರತಿಜ್ಞಾ(scope);
		ಹಿಂತಿರುಗಿ scope;
	      }
      }
      ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ vpiHandle intermodpath_iterate(ಪೂರ್ಣಾಂಕ code, vpiHandle ref)
{
      ರಚನೆ __vpiInterModPath*rfp = dynamic_cast<__vpiInterModPath*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

	// For now intermodpaths only support exactly two ports
      ಆಯ್ಕೆ (code) {
	  ಸ್ಥಿತಿ vpi_ಪೋರ್ಟ್: {
	    vpiHandle*ವಾದಗಳು = static_cast<vpiHandle*>(ಸೊನ್ನೆ_ಸ್ಮರಣೆ_ಕೊಡು(2, ಗಾತ್ರ(vpiHandle*)));
	    ವಾದಗಳು[0] = rfp->port1;
	    ವಾದಗಳು[1] = rfp->port2;
	    ಹಿಂತಿರುಗಿ vpip_make_iterator(2, ವಾದಗಳು, ನಿಜ);
	  }
      }
      ಹಿಂತಿರುಗಿ 0;
}

/*
 * This routine will put specific dimension of delay[] values
 * into a vpiHandle. In this case, we will put
 * specific delays values in a vpiInterModPath object
 *
 */
ಸ್ಥಿರ ಶೂನ್ಯ intermodpath_put_delays (vpiHandle ref, p_vpi_delay delays)
{
      vvp_time64_t tmp[12];
      ಪೂರ್ಣಾಂಕ idx;
      ರಚನೆ __vpiInterModPath * src = dynamic_cast<__vpiInterModPath*>(ref) ;
      ಪ್ರತಿಜ್ಞಾ(src) ;

      vvp_fun_intermodpath *fun = dynamic_cast<vvp_fun_intermodpath*>(src->net->fun);
      ಪ್ರತಿಜ್ಞಾ( fun );

      ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ map_array_t[12];
	// Only the first six entries are used for the less than twelve maps.
      ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ map_array_t map_1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ map_array_t map_2 = {0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0};
      ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ map_array_t map_3 = {0, 1, 2, 0, 2, 1, 0, 0, 0, 0, 0, 0};
      ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ map_array_t map_6 = {0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0};
      ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ map_array_t map12 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

      ಸ್ಥಿರಾಂಕ map_array_t*use_map = 0;
      ಆಯ್ಕೆ (delays->no_ದ_delays) {
	  ಸ್ಥಿತಿ 1:
	    use_map = &map_1;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 2:
	    use_map = &map_2;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 3:
	    use_map = &map_3;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 6:
	    use_map = &map_6;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 12:
	    use_map = &map12;
	    ನಿಲ್ಲಿಸು;
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ನಿಲ್ಲಿಸು;
      }

      ಆದರೆ (delays->ಸಮಯ_type == vpi_Sim_ಸಮಯ) {
	    ಗಾಗಿ (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_timestruct_ಗಮ್ಯ_ಸಮಯ(delays->da+use_map[0][idx]);
	    }
      } ಇಲ್ಲದಿದ್ದರೆ {
	      // You cannot create a modpath with a negative delay so set it
	      // to zero per 1364-2005 section 14.3.1.
	    ಗಾಗಿ (idx = 0 ; idx < delays->no_ದ_delays ; idx += 1) {
		  ಆದರೆ (delays->da[idx].real < 0.0) delays->da[idx].real = 0.0;
	    }
	    ಗಾಗಿ (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_scaled_real_ಗಮ್ಯ_time64(delays->da[use_map[0][idx]].real,
							src->scope);
	    }
      }

      /* Now define the to-from-x delays if needed. */
      ಆದರೆ (delays->no_ದ_delays <= 6) {
	      /* 0->x is the minimum of 0->z and 0->1. */
	    tmp[DELAY_ಅಂಚು_0x] = tmp[DELAY_ಅಂಚು_0z] < tmp[DELAY_ಅಂಚು_01] ?
	                           tmp[DELAY_ಅಂಚು_0z] : tmp[DELAY_ಅಂಚು_01];
	      /* x->1 is the maximum of z->1 and 0->1. */
	    tmp[DELAY_ಅಂಚು_x1] = tmp[DELAY_ಅಂಚು_z1] > tmp[DELAY_ಅಂಚು_01] ?
	                           tmp[DELAY_ಅಂಚು_z1] : tmp[DELAY_ಅಂಚು_01];
	      /* 1->x is the minimum of 1->z and 1->0. */
	    tmp[DELAY_ಅಂಚು_1x] = tmp[DELAY_ಅಂಚು_1z] < tmp[DELAY_ಅಂಚು_10] ?
	                           tmp[DELAY_ಅಂಚು_1z] : tmp[DELAY_ಅಂಚು_10];
	      /* x->0 is the maximum of z->0 and 1->0. */
	    tmp[DELAY_ಅಂಚು_x0] = tmp[DELAY_ಅಂಚು_z0] > tmp[DELAY_ಅಂಚು_10] ?
	                           tmp[DELAY_ಅಂಚು_z0] : tmp[DELAY_ಅಂಚು_10];
	      /* x->z is the maximum of 1->z and 0->z. */
	    tmp[DELAY_ಅಂಚು_xz] = tmp[DELAY_ಅಂಚು_1z] > tmp[DELAY_ಅಂಚು_0z] ?
	                           tmp[DELAY_ಅಂಚು_1z] : tmp[DELAY_ಅಂಚು_0z];
	      /* z->x is the minimum of z->1 and z->0. */
	    tmp[DELAY_ಅಂಚು_zx] = tmp[DELAY_ಅಂಚು_z1] < tmp[DELAY_ಅಂಚು_z0] ?
	                           tmp[DELAY_ಅಂಚು_z1] : tmp[DELAY_ಅಂಚು_z0];
      }

      fun->put_delay12(tmp);
}

/*
 * This routine will retrieve the delay[12] values
 * of a vpiHandle. In this case, he will get an
 * specific delays values from a vpiInterModPath
 * object
 *
 */

ಸ್ಥಿರ ಶೂನ್ಯ intermodpath_get_delays ( vpiHandle ref, p_vpi_delay delays )
{
      ರಚನೆ __vpiInterModPath*src = dynamic_cast<__vpiInterModPath*>(ref) ;
      ಪ್ರತಿಜ್ಞಾ(src);

      vvp_fun_intermodpath *fun = dynamic_cast<vvp_fun_intermodpath*>(src->net->fun);
      ಪ್ರತಿಜ್ಞಾ(fun);

      ಪೂರ್ಣಾಂಕ idx;
      vvp_time64_t tmp[12];
      fun->get_delay12(tmp);

      ಆಯ್ಕೆ (delays->no_ದ_delays) {
	  ಸ್ಥಿತಿ 1:
	  ಸ್ಥಿತಿ 2:
	  ಸ್ಥಿತಿ 3:
	  ಸ್ಥಿತಿ 6:
	  ಸ್ಥಿತಿ 12:
	    ನಿಲ್ಲಿಸು;

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ನಿಲ್ಲಿಸು;
      }

      ಆದರೆ (delays->ಸಮಯ_type == vpi_Sim_ಸಮಯ) {
	    ಗಾಗಿ (idx = 0; idx < delays->no_ದ_delays; idx += 1) {
		  vpip_ಸಮಯ_ಗಮ್ಯ_timestruct(delays->da+idx, tmp[idx]);
	    }
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಗಾಗಿ (idx = 0; idx < delays->no_ದ_delays; idx += 1) {
		  delays->da[idx].real = vpip_ಸಮಯ_ಗಮ್ಯ_scaled_real(tmp[idx], src->scope);
	    }
      }
}

/*
* The __vpiInterModPath class is what the VPI client sees as a
* vpiInterModPath object.
*/
ಸಾಲೊಳಗೆ __vpiInterModPath::__vpiInterModPath()
{ }

ಪೂರ್ಣಾಂಕ __vpiInterModPath::get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ
{ ಹಿಂತಿರುಗಿ vpi_Inter_Mod_ಮಾರ್ಗ; }

ಪೂರ್ಣಾಂಕ __vpiInterModPath::vpi_get(ಪೂರ್ಣಾಂಕ code)
{ ಹಿಂತಿರುಗಿ intermodpath_get(code, ಇದು); }

ಶೂನ್ಯ __vpiInterModPath::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val)
{ intermodpath_get_ಮೌಲ್ಯ(ಇದು, val); }

vpiHandle __vpiInterModPath::vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಂಕ flags)
{ ಹಿಂತಿರುಗಿ intermodpath_put_ಮೌಲ್ಯ(ಇದು, val, flags); }

vpiHandle __vpiInterModPath::vpi_handle(ಪೂರ್ಣಾಂಕ code)
{ ಹಿಂತಿರುಗಿ intermodpath_get_handle(code, ಇದು); }

vpiHandle __vpiInterModPath::vpi_iterate(ಪೂರ್ಣಾಂಕ code)
{ ಹಿಂತಿರುಗಿ intermodpath_iterate(code, ಇದು); }

ಶೂನ್ಯ __vpiInterModPath::vpi_get_delays(p_vpi_delay del)
{ intermodpath_get_delays(ಇದು, del); }

ಶೂನ್ಯ __vpiInterModPath::vpi_put_delays(p_vpi_delay del)
{ intermodpath_put_delays(ಇದು, del); }

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ intermodpath_free_object( vpiHandle ref )
{
      ಅಳಿಸು ref;
      ಹಿಂತಿರುಗಿ 1 ;
}

__vpiHandle::free_object_fun_t __vpiInterModPath::free_object_fun(ಶೂನ್ಯ)
{ ಹಿಂತಿರುಗಿ &intermodpath_free_object; }

/*
 * This function will construct a vpiInterModPath Object.
 * give a respective "net", and will point to his
 * respective functor
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಸ್ಥಿರ ರಚನೆ __vpiInterModPath**imp_list = 0;
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ imp_ಎಣಿಕೆ = 0;
#ಆದರೆ_ಅಂತ್ಯ

ರಚನೆ __vpiInterModPath* vpip_make_intermodpath(vvp_net_t *net, vpi_ಪೋರ್ಟ್_Info* port1, vpi_ಪೋರ್ಟ್_Info* port2)
{
      ರಚನೆ __vpiInterModPath*obj = ಹೊಸ __vpiInterModPath;
      obj->scope = vpip_peek_current_scope ( );

      obj->net = net;
      obj->port1 = port1;
      obj->port2 = port2;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
      imp_ಎಣಿಕೆ += 1;
      imp_list = static_cast<ರಚನೆ __vpiInterModPath **>
                 (realloc(imp_list,
                  imp_ಎಣಿಕೆ*ಗಾತ್ರ(ರಚನೆ __vpiInterModPath **)));
      imp_list[imp_ಎಣಿಕೆ-1] = obj;
#ಆದರೆ_ಅಂತ್ಯ
      ಹಿಂತಿರುಗಿ obj;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಶೂನ್ಯ intermodpath_delete()
{
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0; idx < mp_ಎಣಿಕೆ; idx += 1) {
	    ಅಳಿಸು imp_list[idx];
      }
      ಬಿಡುಗಡೆ(imp_list);
      imp_list = 0;
      imp_ಎಣಿಕೆ = 0;
}
#ಆದರೆ_ಅಂತ್ಯ
