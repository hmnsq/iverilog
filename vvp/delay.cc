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
#ifdef CHECK_WITH_VALGRIND
#ಅನ್ತರ್ಭಾವ "vvp_cleanup.h"
#endif
#ಅನ್ತರ್ಭಾವ <iostream>
#ಅನ್ತರ್ಭಾವ <cstdlib>
#ಅನ್ತರ್ಭಾವ <list>
#ಅನ್ತರ್ಭಾವ <cassert>
#ಅನ್ತರ್ಭಾವ <cmath>
#ಅನ್ತರ್ಭಾವ "ivl_alloc.h"

using ನಾಮಾಕಾಶ std;

ಶೂನ್ಯ vvp_delay_t::calculate_min_delay_()
{
      min_delay_ = rise_;
      ಯದಿ (fall_ < min_delay_)
	    min_delay_ = fall_;
      ಯದಿ (ignore_decay_) decay_ = min_delay_;
      ಅನ್ಯಥಾ ಯದಿ (decay_ < min_delay_)
	    min_delay_ = decay_;
}

vvp_delay_t::vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall)
{
      rise_ = rise;
      fall_ = fall;
      decay_= fall < rise? fall : rise;
      min_delay_ = decay_;
      ignore_decay_ = ಅಸತ್ಯ;
}

vvp_delay_t::vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall, vvp_time64_t decay)
{
      rise_ = rise;
      fall_ = fall;
      decay_= decay;
      ignore_decay_ = ಅಸತ್ಯ;

      calculate_min_delay_();
}

ಶೂನ್ಯ vvp_delay_t::set_ignore_decay()
{
      ignore_decay_ = ಸತ್ಯ;

      calculate_min_delay_();
}

vvp_delay_t::~vvp_delay_t()
{
}

vvp_time64_t vvp_delay_t::get_delay(vvp_bit4_t from, vvp_bit4_t to)
{
      ವಿಕಲ್ಪ (from) {
	  ಪಕ್ಷ BIT4_0:
	    ವಿಕಲ್ಪ (to) {
		ಪಕ್ಷ BIT4_0: ಪ್ರತ್ಯಾಗಮ 0;
		ಪಕ್ಷ BIT4_1: ಪ್ರತ್ಯಾಗಮ rise_;
		ಪಕ್ಷ BIT4_X: ಪ್ರತ್ಯಾಗಮ min_delay_;
		ಪಕ್ಷ BIT4_Z: ಪ್ರತ್ಯಾಗಮ decay_;
	    }
	    ಭಙ್ಗ;
	  ಪಕ್ಷ BIT4_1:
	    ವಿಕಲ್ಪ (to) {
		ಪಕ್ಷ BIT4_0: ಪ್ರತ್ಯಾಗಮ fall_;
		ಪಕ್ಷ BIT4_1: ಪ್ರತ್ಯಾಗಮ 0;
		ಪಕ್ಷ BIT4_X: ಪ್ರತ್ಯಾಗಮ min_delay_;
		ಪಕ್ಷ BIT4_Z: ಪ್ರತ್ಯಾಗಮ decay_;
	    }
	    ಭಙ್ಗ;
	  ಪಕ್ಷ BIT4_X:
	    ವಿಕಲ್ಪ (to) {
		ಪಕ್ಷ BIT4_0: ಪ್ರತ್ಯಾಗಮ fall_;
		ಪಕ್ಷ BIT4_1: ಪ್ರತ್ಯಾಗಮ rise_;
		ಪಕ್ಷ BIT4_X: ಪ್ರತ್ಯಾಗಮ 0;
		ಪಕ್ಷ BIT4_Z: ಪ್ರತ್ಯಾಗಮ decay_;
	    }
	    ಭಙ್ಗ;
	  ಪಕ್ಷ BIT4_Z:
	    ವಿಕಲ್ಪ (to) {
		ಪಕ್ಷ BIT4_0: ಪ್ರತ್ಯಾಗಮ fall_;
		ಪಕ್ಷ BIT4_1: ಪ್ರತ್ಯಾಗಮ rise_;
		ಪಕ್ಷ BIT4_X: ಪ್ರತ್ಯಾಗಮ min_delay_;
		ಪಕ್ಷ BIT4_Z: ಪ್ರತ್ಯಾಗಮ 0;
	    }
	    ಭಙ್ಗ;
      }

      ಪ್ರತಿಜ್ಞಾ(0);
      ಪ್ರತ್ಯಾಗಮ 0;
}

vvp_time64_t vvp_delay_t::get_min_delay() ಧ್ರುವ
{
      ಪ್ರತ್ಯಾಗಮ min_delay_;
}

ಶೂನ್ಯ vvp_delay_t::set_rise(vvp_time64_t val)
{
      rise_ = val;
      ಯದಿ (val < min_delay_) {
	    min_delay_ = val;
	    ಯದಿ (ignore_decay_) decay_ = val;
      } ಅನ್ಯಥಾ
	    calculate_min_delay_();
}

ಶೂನ್ಯ vvp_delay_t::set_fall(vvp_time64_t val)
{
      fall_ = val;
      ಯದಿ (val < min_delay_) {
	    min_delay_ = val;
	    ಯದಿ (ignore_decay_) decay_ = val;
      } ಅನ್ಯಥಾ
	    calculate_min_delay_();
}

ಶೂನ್ಯ vvp_delay_t::set_decay(vvp_time64_t val)
{
      ಪ್ರತಿಜ್ಞಾ(!ignore_decay_);

      decay_ = val;
      ಯದಿ (val < min_delay_)
	    min_delay_ = val;
      ಅನ್ಯಥಾ
	    calculate_min_delay_();
}

vvp_fun_delay::vvp_fun_delay(vvp_net_t*n, unsigned width, ಧ್ರುವ vvp_delay_t&d)
: net_(n), delay_(d)
{
      cur_real_ = 0.0;
      ಯದಿ (width > 0) {
            cur_vec4_ = vvp_vector4_t(width, BIT4_X);
            cur_vec8_ = vvp_vector8_t(cur_vec4_, 6, 6);
            schedule_init_propagate(net_, cur_vec4_);
      } ಅನ್ಯಥಾ {
            schedule_init_propagate(net_, cur_real_);
      }
      list_ = 0;
      type_ = UNKNOWN_DELAY;
      initial_ = ಸತ್ಯ;
	// Calculate the values used when converting variable delays
	// to simulation time units.
      ಧ್ರುವ __vpiScope*scope = vpip_peek_current_scope();

      ಪೂರ್ಣಾಙ್ಕ powr = scope->time_units - scope->time_precision;
      round_ = 1;
      ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ lp = 0; lp < powr; lp += 1) round_ *= 10;

      powr = scope->time_precision - vpip_get_time_precision();
      scale_ = 1;
      ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ lp = 0; lp < powr; lp += 1) scale_ *= 10;
}

vvp_fun_delay::~vvp_fun_delay()
{
      ಯಾವತ್ (ಸಂರಚನಾ event_*cur = dequeue_())
	    delete cur;
}

ತಾರ್ಕಿಕ vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        ಧ್ರುವ vvp_vector4_t&bit)
{
      ಯದಿ (list_ == 0) ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      ಯದಿ (list_->next->ptr_vec4.eeq(bit)) ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;

      clean_pulse_events_(use_delay);
      ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

ತಾರ್ಕಿಕ vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        ಧ್ರುವ vvp_vector8_t&bit)
{
      ಯದಿ (list_ == 0) ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      ಯದಿ (list_->next->ptr_vec8.eeq(bit)) ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;

      clean_pulse_events_(use_delay);
      ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

ತಾರ್ಕಿಕ vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        ದ್ವಿಗುಣ bit)
{
      ಯದಿ (list_ == 0) ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      ಯದಿ (list_->next->ptr_real == bit) ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;

      clean_pulse_events_(use_delay);
      ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

ಶೂನ್ಯ vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay)
{
      ಪ್ರತಿಜ್ಞಾ(list_ != 0);

      ಕುರು {
	    ಸಂರಚನಾ event_*cur = list_->next;
	      /* If this event is far enough from the event I'm about
	         to create, then that scheduled event is not a pulse
	         to be eliminated, so we're done. */
	    ಯದಿ (cur->sim_time+use_delay <= use_delay+schedule_simtime())
		  ಭಙ್ಗ;

	    ಯದಿ (list_ == cur)
		  list_ = 0;
	    ಅನ್ಯಥಾ
		  list_->next = cur->next;
	    delete cur;
      } ಯಾವತ್ (list_);
}

/*
 * FIXME: this implementation currently only uses the maximum delay
 * from all the bit changes in the vectors. If there are multiple
 * changes with different delays, then the results would be
 * wrong. What should happen is that if there are multiple changes,
 * multiple vectors approaching the result should be scheduled.
 */
ಶೂನ್ಯ vvp_fun_delay::recv_vec4(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
                              vvp_context_t)
{
      ಯದಿ (port.port() > 0) {
	      // Get the integer value of the bit vector, or 0 if
	      // there are X or Z bits.
	    vvp_time64_t bval = 0;
	      // The following does not work correctly for negative values.
	      // They should be sign extended to 64 bits (1364-2001 9.7.1).
	    vector4_to_ಮೌಲ್ಯ(bit, bval);
	      // Integer values do not need to be rounded so just scale them.
	    vvp_time64_t val = bval * round_ * scale_;

	    ವಿಕಲ್ಪ (port.port()) {
		ಪಕ್ಷ 1:
		  delay_.set_rise(val);
		  ಪ್ರತ್ಯಾಗಮ;
		ಪಕ್ಷ 2:
		  delay_.set_fall(val);
		  ಪ್ರತ್ಯಾಗಮ;
		ಪಕ್ಷ 3:
		  delay_.set_decay(val);
		  ಪ್ರತ್ಯಾಗಮ;
	    }
	    ಪ್ರತ್ಯಾಗಮ;
      }

      vvp_time64_t use_delay;
	/* This is an initial value so it needs to be compared to all the
	   bits (the order the bits are changed is not deterministic). */
      ಯದಿ (initial_) {
	    type_ = VEC4_DELAY;
            cur_vec8_ = vvp_vector8_t(vvp_vector4_t(0, BIT4_X), 6, 6);
	    vvp_bit4_t cur_val = cur_vec4_.value(0);
	    use_delay = delay_.get_delay(cur_val, bit.value(0));
	    ಪ್ರತಿ (unsigned idx = 1 ;  idx < bit.size() ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(cur_val, bit.value(idx));
		  ಯದಿ (tmp > use_delay) use_delay = tmp;
	    }
      } ಅನ್ಯಥಾ {
	    ಪ್ರತಿಜ್ಞಾ(type_ == VEC4_DELAY);

	      // Use as a reference for calculating the delay the
	      // current value of the output. Detect and handle the
	      // special case that the event list contains the current
	      // value as a zero-delay-remaining event.
	    ಧ್ರುವ vvp_vector4_t&use_vec4 = (list_ && list_->next->sim_time == schedule_simtime())? list_->next->ptr_vec4 : cur_vec4_;

	      /* How many bits to compare? */
	    unsigned use_wid = use_vec4.size();
	    ಯದಿ (bit.size() < use_wid) use_wid = bit.size();

	      /* Scan the vectors looking for delays. Select the maximum
	         delay encountered. */
	    use_delay = delay_.get_delay(use_vec4.value(0), bit.value(0));

	    ಪ್ರತಿ (unsigned idx = 1 ;  idx < use_wid ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(use_vec4.value(idx), bit.value(idx));
		  ಯದಿ (tmp > use_delay) use_delay = tmp;
	    }
      }

      /* what *should* happen here is we check to see if there is a
         transaction in the queue. This would be a pulse that needs to be
         eliminated. */
      ಯದಿ (clean_pulse_events_(use_delay, bit)) ಪ್ರತ್ಯಾಗಮ;

      vvp_time64_t use_simtime = schedule_simtime() + use_delay;

	/* And propagate it. */
      ಯದಿ (use_delay == 0 && list_ == 0) {
	    cur_vec4_ = bit;
	    initial_ = ಅಸತ್ಯ;
	    net_->send_vec4(cur_vec4_, 0);
      } ಅನ್ಯಥಾ {
	    ಸಂರಚನಾ event_*cur = ನೂತನ ಸಂರಚನಾ event_(use_simtime);
	    cur->run_run_ptr = &vvp_fun_delay::run_run_vec4_;
	    cur->ptr_vec4 = bit;
	    enqueue_(cur);
	    schedule_generic(ಏತತ್, use_delay, ಅಸತ್ಯ);
      }
}

ಶೂನ್ಯ vvp_fun_delay::recv_vec4_pv(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit,
			         unsigned base, unsigned vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

/* See the recv_vec4 comment above. */
ಶೂನ್ಯ vvp_fun_delay::recv_vec8(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector8_t&bit)
{
      ಪ್ರತಿಜ್ಞಾ(port.port() == 0);

      vvp_time64_t use_delay;
	/* This is an initial value so it needs to be compared to all the
	   bits (the order the bits are changed is not deterministic). */
      ಯದಿ (initial_) {
	    type_ = VEC8_DELAY;
            cur_vec4_ = vvp_vector4_t(0, BIT4_X);
	    vvp_bit4_t cur_val = cur_vec8_.value(0).value();
	    use_delay = delay_.get_delay(cur_val, bit.value(0).value());
	    ಪ್ರತಿ (unsigned idx = 1 ;  idx < bit.size() ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(cur_val, bit.value(idx).value());
		  ಯದಿ (tmp > use_delay) use_delay = tmp;
	    }
      } ಅನ್ಯಥಾ {
	    ಪ್ರತಿಜ್ಞಾ(type_ == VEC8_DELAY);

	      // Use as a reference for calculating the delay the
	      // current value of the output. Detect and handle the
	      // special case that the event list contains the current
	      // value as a zero-delay-remaining event.
	    ಧ್ರುವ vvp_vector8_t&use_vec8 = (list_ && list_->next->sim_time == schedule_simtime())? list_->next->ptr_vec8 : cur_vec8_;

	      /* How many bits to compare? */
	    unsigned use_wid = use_vec8.size();
	    ಯದಿ (bit.size() < use_wid) use_wid = bit.size();

	      /* Scan the vectors looking for delays. Select the maximum
	         delay encountered. */
	    use_delay = delay_.get_delay(use_vec8.value(0).value(),
	                                 bit.value(0).value());

	    ಪ್ರತಿ (unsigned idx = 1 ;  idx < use_wid ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(use_vec8.value(idx).value(),
		                         bit.value(idx).value());
		  ಯದಿ (tmp > use_delay) use_delay = tmp;
	    }
      }

      /* what *should* happen here is we check to see if there is a
         transaction in the queue. This would be a pulse that needs to be
         eliminated. */
      ಯದಿ (clean_pulse_events_(use_delay, bit)) ಪ್ರತ್ಯಾಗಮ;

      vvp_time64_t use_simtime = schedule_simtime() + use_delay;

	/* And propagate it. */
      ಯದಿ (use_delay == 0 && list_ == 0) {
	    cur_vec8_ = bit;
	    initial_ = ಅಸತ್ಯ;
	    net_->send_vec8(cur_vec8_);
      } ಅನ್ಯಥಾ {
	    ಸಂರಚನಾ event_*cur = ನೂತನ ಸಂರಚನಾ event_(use_simtime);
	    cur->ptr_vec8 = bit;
	    cur->run_run_ptr = &vvp_fun_delay::run_run_vec8_;
	    enqueue_(cur);
	    schedule_generic(ಏತತ್, use_delay, ಅಸತ್ಯ);
      }
}

ಶೂನ್ಯ vvp_fun_delay::recv_vec8_pv(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector8_t&bit,
			         unsigned base, unsigned vwid)
{
      recv_vec8_pv_(ptr, bit, base, vwid);
}

ಶೂನ್ಯ vvp_fun_delay::recv_real(vvp_net_ptr_t port, ದ್ವಿಗುಣ bit,
                              vvp_context_t)
{
      ಯದಿ (port.port() > 0) {
	    /* If the port is not 0, then this is a delay value that
	    should be rounded and converted to an integer delay. */
	    vvp_time64_t val = 0;
	    ಯದಿ (bit > -0.5) {
		  val = (vvp_time64_t) (bit * round_ + 0.5) * scale_;
	    } ಅನ್ಯಥಾ ಯದಿ (bit != bit) {
		    // For a NaN we use the default (0).
	    } ಅನ್ಯಥಾ {
		  vvp_vector4_t vec4(8*ಪರಿಮಾಣ(vvp_time64_t),
		                     floor(-bit * round_ + 0.5) * -1 * scale_);
		  vector4_to_ಮೌಲ್ಯ(vec4, val);
	    }

	    ವಿಕಲ್ಪ (port.port()) {
		ಪಕ್ಷ 1:
		  delay_.set_rise(val);
		  ಪ್ರತ್ಯಾಗಮ;
		ಪಕ್ಷ 2:
		  delay_.set_fall(val);
		  ಪ್ರತ್ಯಾಗಮ;
		ಪಕ್ಷ 3:
		  delay_.set_decay(val);
		  ಪ್ರತ್ಯಾಗಮ;
	    }
	    ಪ್ರತ್ಯಾಗಮ;
      }

      ಯದಿ (initial_) {
	    type_ = REAL_DELAY;
            cur_vec4_ = vvp_vector4_t(0, BIT4_X);
            cur_vec8_ = vvp_vector8_t(cur_vec4_, 6, 6);
      } ಅನ್ಯಥಾ ಪ್ರತಿಜ್ಞಾ(type_ == REAL_DELAY);

      vvp_time64_t use_delay;
      use_delay = delay_.get_min_delay();

      /* Eliminate glitches. */
      ಯದಿ (clean_pulse_events_(use_delay, bit)) ಪ್ರತ್ಯಾಗಮ;

      /* This must be done after cleaning pulses to avoid propagating
       * an incorrect value. */
      ಯದಿ (cur_real_ == bit) ಪ್ರತ್ಯಾಗಮ;

      vvp_time64_t use_simtime = schedule_simtime() + use_delay;

      ಯದಿ (use_delay == 0 && list_ == 0) {
	    cur_real_ = bit;
	    initial_ = ಅಸತ್ಯ;
	    net_->send_real(cur_real_, 0);
      } ಅನ್ಯಥಾ {
	    ಸಂರಚನಾ event_*cur = ನೂತನ ಸಂರಚನಾ event_(use_simtime);
	    cur->run_run_ptr = &vvp_fun_delay::run_run_real_;
	    cur->ptr_real = bit;
	    enqueue_(cur);

	    schedule_generic(ಏತತ್, use_delay, ಅಸತ್ಯ);
      }
}

ಶೂನ್ಯ vvp_fun_delay::run_run()
{
      vvp_time64_t sim_time = schedule_simtime();
      ಯದಿ (list_ == 0 || list_->next->sim_time > sim_time)
	    ಪ್ರತ್ಯಾಗಮ;

      ಸಂರಚನಾ event_*cur = dequeue_();
      ಯದಿ (cur == 0)
	    ಪ್ರತ್ಯಾಗಮ;

      (ಏತತ್->*(cur->run_run_ptr))(cur);
      initial_ = ಅಸತ್ಯ;
      delete cur;
}

ಶೂನ್ಯ vvp_fun_delay::run_run_vec4_(ಧ್ರುವ ಸಂರಚನಾ event_*cur)
{
      cur_vec4_ = cur->ptr_vec4;
      net_->send_vec4(cur_vec4_, 0);
}

ಶೂನ್ಯ vvp_fun_delay::run_run_vec8_(ಧ್ರುವ ಸಂರಚನಾ vvp_fun_delay::event_*cur)
{
      cur_vec8_ = cur->ptr_vec8;
      net_->send_vec8(cur_vec8_);
}

ಶೂನ್ಯ vvp_fun_delay::run_run_real_(ಧ್ರುವ ಸಂರಚನಾ vvp_fun_delay::event_*cur)
{
      cur_real_ = cur->ptr_real;
      net_->send_real(cur_real_, 0);
}

vvp_fun_modpath::vvp_fun_modpath(vvp_net_t*net, unsigned width)
: net_(net), src_list_(0), ifnone_list_(0)
{
      cur_vec4_ = vvp_vector4_t(width, BIT4_X);
      schedule_init_propagate(net_, cur_vec4_);
}

vvp_fun_modpath::~vvp_fun_modpath()
{
	// Delete the source probes.
      ಯಾವತ್ (src_list_) {
	    vvp_fun_modpath_src*tmp = src_list_;
	    src_list_ = tmp->next_;
	    delete tmp;
      }
      ಯಾವತ್ (ifnone_list_) {
	    vvp_fun_modpath_src*tmp = ifnone_list_;
	    ifnone_list_ = tmp->next_;
	    delete tmp;
      }
}

ಶೂನ್ಯ vvp_fun_modpath::add_modpath_src(vvp_fun_modpath_src*that, ತಾರ್ಕಿಕ ifnone)
{
      ಪ್ರತಿಜ್ಞಾ(that->next_ == 0);
      ಯದಿ (ifnone) {
	    that->next_ = ifnone_list_;
	    ifnone_list_ = that;
      } ಅನ್ಯಥಾ {
	    that->next_ = src_list_;
	    src_list_ = that;
      }
}

ಸ್ಥಿರ vvp_time64_t delay_from_edge(vvp_bit4_t a, vvp_bit4_t b,
                                           vvp_time64_t ಧ್ರುವ array[12])
{
      ಪ್ರಕಾರನಿರ್ದೇಶ delay_edge_t bit4_table4[4];
      ಸ್ಥಿರ ಧ್ರುವ bit4_table4 edge_table[4] = {
	    { DELAY_EDGE_01, DELAY_EDGE_01, DELAY_EDGE_0z, DELAY_EDGE_0x },
	    { DELAY_EDGE_10, DELAY_EDGE_10, DELAY_EDGE_1z, DELAY_EDGE_1x },
	    { DELAY_EDGE_z0, DELAY_EDGE_z1, DELAY_EDGE_z0, DELAY_EDGE_zx },
	    { DELAY_EDGE_x0, DELAY_EDGE_x1, DELAY_EDGE_xz, DELAY_EDGE_x0 }
      };

      ಪ್ರತ್ಯಾಗಮ array[ edge_table[a][b] ];
}

ಶೂನ್ಯ vvp_fun_modpath::recv_vec4(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
                                vvp_context_t)
{
	/* Only the first port is used. */
      ಯದಿ (port.port() > 0)
	    ಪ್ರತ್ಯಾಗಮ;

      ಯದಿ (cur_vec4_.eeq(bit))
	    ಪ್ರತ್ಯಾಗಮ;

	/* Select a time delay source that applies. Notice that there
	   may be multiple delay sources that apply, so collect all
	   the candidates into a list first. */
      list<vvp_fun_modpath_src*>candidate_list;
      vvp_time64_t candidate_wake_time = 0;
      ಪ್ರತಿ (vvp_fun_modpath_src*cur = src_list_ ;  cur ;  cur=cur->next_) {
	      /* Skip paths that are disabled by conditions. */
	    ಯದಿ (cur->condition_flag_ == ಅಸತ್ಯ)
		  ಅನುವರ್ತ;

	    ಯದಿ (candidate_list.empty()) {
		  candidate_list.push_back(cur);
		  candidate_wake_time = cur->wake_time_;
	    } ಅನ್ಯಥಾ ಯದಿ (cur->wake_time_ == candidate_wake_time) {
		  candidate_list.push_back(cur);
	    } ಅನ್ಯಥಾ ಯದಿ (cur->wake_time_ > candidate_wake_time) {
		  candidate_list.assign(1, cur);
		  candidate_wake_time = cur->wake_time_;
	    } ಅನ್ಯಥಾ {
		  ಅನುವರ್ತ; /* Skip this entry. */
	    }
      }

	/* Only add the ifnone delay if it has a later wake_time_ or
	 * if there are no normal delays. */
      vvp_time64_t ifnone_wake_time = candidate_wake_time;
      ಪ್ರತಿ (vvp_fun_modpath_src*cur = ifnone_list_ ;  cur ;  cur=cur->next_) {
	    ಯದಿ (candidate_list.empty()) {
		  candidate_list.push_back(cur);
		  ifnone_wake_time = cur->wake_time_;
	    } ಅನ್ಯಥಾ ಯದಿ (cur->wake_time_ == ifnone_wake_time &&
	               ifnone_wake_time > candidate_wake_time) {
		  candidate_list.push_back(cur);
	    } ಅನ್ಯಥಾ ಯದಿ (cur->wake_time_ > ifnone_wake_time) {
		  candidate_list.assign(1, cur);
		  ifnone_wake_time = cur->wake_time_;
	    } ಅನ್ಯಥಾ {
		  ಅನುವರ್ತ; /* Skip this entry. */
	    }
      }

	/* Handle the special case that there are no delays that
	   match. This may happen, for example, if the set of
	   conditional delays is incomplete, leaving some cases
	   uncovered. In that case, just pass the data without delay */
      ಯದಿ (candidate_list.empty()) {
	    cur_vec4_ = bit;
	    schedule_generic(ಏತತ್, 0, ಅಸತ್ಯ);
	    ಪ್ರತ್ಯಾಗಮ;
      }

	/* Now given that we have a list of candidate delays, find for
	   each if the 12 numbers the minimum from all the
	   candidates. This minimum set becomes the chosen delay to
	   use. */
      vvp_time64_t out_at[12];
      vvp_time64_t now = schedule_simtime();

      ಪ್ರಕಾರನಿರ್ದೇಶ list<vvp_fun_modpath_src*>::const_iterator iter_t;

      iter_t cur = candidate_list.begin();
      ಧ್ರುವ vvp_fun_modpath_src*src = *cur;

      ಪ್ರತಿ (unsigned idx = 0 ;  idx < 12 ;  idx += 1) {
	    out_at[idx] = src->wake_time_ + src->delay_[idx];
	    ಯದಿ (out_at[idx] <= now)
		  out_at[idx] = 0;
	    ಅನ್ಯಥಾ
		  out_at[idx] -= now;
      }

      ಪ್ರತಿ (++ cur ; cur != candidate_list.end() ; ++ cur ) {
	    src = *cur;
	    ಪ್ರತಿ (unsigned idx = 0 ;  idx < 12 ;  idx += 1) {
		  vvp_time64_t tmp = src->wake_time_ + src->delay_[idx];
		  ಯದಿ (tmp <= now)
			tmp = 0;
		  ಅನ್ಯಥಾ
			tmp -= now;
		  ಯದಿ (tmp < out_at[idx])
			out_at[idx] = tmp;
	    }
      }

	/* Given the scheduled output time, create an output event. */
      vvp_time64_t use_delay = delay_from_edge(cur_vec4_.value(0),
					       bit.value(0),
					       out_at);

	/* FIXME: This bases the edge delay on only the least
	   bit. This is WRONG! I need to find all the possible delays,
	   and schedule an event for each partial change. Hard! */
      ಪ್ರತಿ (unsigned idx = 1 ;  idx < bit.size() ;  idx += 1) {
	    vvp_time64_t tmp = delay_from_edge(cur_vec4_.value(idx),
					       bit.value(idx),
					       out_at);
	      /* If the current and new bit values match then no delay
	       * is needed for this bit. */
	    ಯದಿ (cur_vec4_.value(idx) == bit.value(idx)) ಅನುವರ್ತ;
	    ಯದಿ (tmp != use_delay) {
		  fprintf(stderr, "sorry: multi-bit module path delays are "
		                  "currently not fully supported.\n");
		  fprintf(stderr, "     : using the LSB delay for all bits.\n");
		  ಅನುವರ್ತ;
            }
      }

      cur_vec4_ = bit;
      schedule_generic(ಏತತ್, use_delay, ಅಸತ್ಯ);
}

ಶೂನ್ಯ vvp_fun_modpath::run_run()
{
      net_->send_vec4(cur_vec4_, 0);
}

vvp_fun_modpath_src::vvp_fun_modpath_src(vvp_time64_t ಧ್ರುವ del[12])
{
      ಪ್ರತಿ (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = del[idx];

      next_ = 0;
      wake_time_ = 0;
      condition_flag_ = ಸತ್ಯ;
}

vvp_fun_modpath_src::~vvp_fun_modpath_src()
{
}

ಶೂನ್ಯ vvp_fun_modpath_src::get_delay12(vvp_time64_t val[12]) ಧ್ರುವ
{
      ಪ್ರತಿ (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    val[idx] = delay_[idx];
}

ಶೂನ್ಯ vvp_fun_modpath_src::put_delay12(ಧ್ರುವ vvp_time64_t val[12])
{
      ಪ್ರತಿ (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = val[idx];
}

ಶೂನ್ಯ vvp_fun_modpath_src::recv_vec4(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
                                    vvp_context_t)
{
      ಯದಿ (port.port() == 0) {
	      // The modpath input...
	    ಯದಿ (test_vec4(bit))
		  wake_time_ = schedule_simtime();

      } ಅನ್ಯಥಾ ಯದಿ (port.port() == 1) {
	      // The modpath condition input...
	    ಯದಿ (bit.value(0) != BIT4_0)
		  condition_flag_ = ಸತ್ಯ;
	    ಅನ್ಯಥಾ
		  condition_flag_ = ಅಸತ್ಯ;
      }
}

ತಾರ್ಕಿಕ vvp_fun_modpath_src::test_vec4(ಧ್ರುವ vvp_vector4_t&)
{
      ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

vvp_fun_modpath_edge::vvp_fun_modpath_edge(vvp_time64_t del[12],
					   ತಾರ್ಕಿಕ pos, ತಾರ್ಕಿಕ neg)
: vvp_fun_modpath_src(del)
{
      old_ಮೌಲ್ಯ = BIT4_X;
      posedge_ = pos;
      negedge_ = neg;
}

ತಾರ್ಕಿಕ vvp_fun_modpath_edge::test_vec4(ಧ್ರುವ vvp_vector4_t&bit)
{
      vvp_bit4_t tmp = old_ಮೌಲ್ಯ;
      old_ಮೌಲ್ಯ = bit.value(0);

      ಪೂರ್ಣಾಙ್ಕ edge_flag = edge(tmp, old_ಮೌಲ್ಯ);
      ಯದಿ (edge_flag > 0) ಪ್ರತ್ಯಾಗಮ posedge_;
      ಯದಿ (edge_flag < 0) ಪ್ರತ್ಯಾಗಮ negedge_;
      ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}


/*
 * All the below routines that begin with
 * modpath_src_* belong the internal function
 * of an vpiModPathIn object. This is used to
 * make some specific delays path operations
 *
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ modpath_src_get(ಪೂರ್ಣಾಙ್ಕ, vpiHandle ref)
{
      ಸಂರಚನಾ __vpiModPathSrc*obj =dynamic_cast<__vpiModPathSrc*>(ref);
      ಪ್ರತಿಜ್ಞಾ(obj);
      ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ modpath_src_get_ಮೌಲ್ಯ(vpiHandle ref, p_vpi_ಮೌಲ್ಯ)
{
      ಸಂರಚನಾ __vpiModPathSrc* modpathsrc = dynamic_cast<__vpiModPathSrc*>(ref);
      ಪ್ರತಿಜ್ಞಾ(modpathsrc);
      ಪ್ರತ್ಯಾಗಮ;
}

ಸ್ಥಿರ vpiHandle modpath_src_put_ಮೌಲ್ಯ(vpiHandle ref, s_vpi_ಮೌಲ್ಯ *, ಪೂರ್ಣಾಙ್ಕ )
{
      ಸಂರಚನಾ __vpiModPathSrc* modpathsrc = dynamic_cast<__vpiModPathSrc*>(ref);
      ಪ್ರತಿಜ್ಞಾ(modpathsrc);
      ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ vpiHandle modpath_src_get_handle(ಪೂರ್ಣಾಙ್ಕ code, vpiHandle ref)
{
      ಸಂರಚನಾ __vpiModPathSrc*rfp = dynamic_cast<__vpiModPathSrc*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ವಿಕಲ್ಪ (code) {

	ಪಕ್ಷ vpiScope:
	  ಪ್ರತ್ಯಾಗಮ rfp->dest->scope;

	  ಪಕ್ಷ vpiModule:
	      { __vpiScope*scope = rfp->dest->scope;
		ಯಾವತ್ (scope && scope->get_type_code() != vpiModule)
		      scope = scope->scope;
		ಪ್ರತಿಜ್ಞಾ(scope);
		ಪ್ರತ್ಯಾಗಮ scope;
	      }

	    // Handles to path term objects should really be obtained via
	    // the vpi_iterate and vpi_scan functions. Continue to allow
	    // them to be obtained here for backwards compatibility with
	    // older versions of Icarus Verilog.

	  ಪಕ್ಷ vpiModPathIn:
	    ಪ್ರತ್ಯಾಗಮ &rfp->path_term_in;

	  ಪಕ್ಷ vpiModPathOut:
	    ಪ್ರತ್ಯಾಗಮ &rfp->dest->path_term_out;
      }
      ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ vpiHandle modpath_src_iterate(ಪೂರ್ಣಾಙ್ಕ code, vpiHandle ref)
{
      ಸಂರಚನಾ __vpiModPathSrc*rfp = dynamic_cast<__vpiModPathSrc*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

	// Module paths with multiple sources or destinations are
	// currently represented by a separate modpath object for
	// each source/destination combination, so there is only
	// ever one input path term and one output path term.
      ವಿಕಲ್ಪ (code) {
	  ಪಕ್ಷ vpiModPathIn: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(1, ಪರಿಮಾಣ(vpiHandle*)));
	    args[0] = &rfp->path_term_in;
	    ಪ್ರತ್ಯಾಗಮ vpip_make_iterator(1, args, ಸತ್ಯ);
	  }
	  ಪಕ್ಷ vpiModPathOut: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(1, ಪರಿಮಾಣ(vpiHandle*)));
	    args[0] = &rfp->dest->path_term_out;
	    ಪ್ರತ್ಯಾಗಮ vpip_make_iterator(1, args, ಸತ್ಯ);
	  }
      }
      ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ vpiHandle modpath_src_index ( vpiHandle ref, ಪೂರ್ಣಾಙ್ಕ)
{
      ಪ್ರತಿಜ್ಞಾ(ref->get_type_code() == vpiModPathIn);
      ಪ್ರತ್ಯಾಗಮ 0;
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
      ಪೂರ್ಣಾಙ್ಕ idx;
      ಸಂರಚನಾ __vpiModPathSrc * src = dynamic_cast<__vpiModPathSrc*>(ref) ;
      ಪ್ರತಿಜ್ಞಾ(src) ;

      vvp_fun_modpath_src *fun = dynamic_cast<vvp_fun_modpath_src*>(src->net->fun);
      ಪ್ರತಿಜ್ಞಾ( fun );

      ಪ್ರಕಾರನಿರ್ದೇಶ unsigned ವರ್ಣ map_array_t[12];
	// Only the first six entries are used for the less than twelve maps.
      ಸ್ಥಿರ ಧ್ರುವ map_array_t map_1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      ಸ್ಥಿರ ಧ್ರುವ map_array_t map_2 = {0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0};
      ಸ್ಥಿರ ಧ್ರುವ map_array_t map_3 = {0, 1, 2, 0, 2, 1, 0, 0, 0, 0, 0, 0};
      ಸ್ಥಿರ ಧ್ರುವ map_array_t map_6 = {0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0};
      ಸ್ಥಿರ ಧ್ರುವ map_array_t map12 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

      ಧ್ರುವ map_array_t*use_map = 0;
      ವಿಕಲ್ಪ (delays->no_of_delays) {
	  ಪಕ್ಷ 1:
	    use_map = &map_1;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ 2:
	    use_map = &map_2;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ 3:
	    use_map = &map_3;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ 6:
	    use_map = &map_6;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ 12:
	    use_map = &map12;
	    ಭಙ್ಗ;
	  ಸ್ವಭಾವ:
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಭಙ್ಗ;
      }

      ಯದಿ (delays->time_type == vpiSimTime) {
	    ಪ್ರತಿ (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_timestruct_to_time(delays->da+use_map[0][idx]);
	    }
      } ಅನ್ಯಥಾ {
	      // You cannot create a modpath with a negative delay so set it
	      // to zero per 1364-2005 section 14.3.1.
	    ಪ್ರತಿ (idx = 0 ; idx < delays->no_of_delays ; idx += 1) {
		  ಯದಿ (delays->da[idx].real < 0.0) delays->da[idx].real = 0.0;
	    }
	    ಪ್ರತಿ (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_scaled_real_to_time64(delays->da[use_map[0][idx]].real,
							src->dest->scope);
	    }
      }

      /* Now define the to-from-x delays if needed. */
      ಯದಿ (delays->no_of_delays <= 6) {
	      /* 0->x is the minimum of 0->z and 0->1. */
	    tmp[DELAY_EDGE_0x] = tmp[DELAY_EDGE_0z] < tmp[DELAY_EDGE_01] ?
	                           tmp[DELAY_EDGE_0z] : tmp[DELAY_EDGE_01];
	      /* x->1 is the maximum of z->1 and 0->1. */
	    tmp[DELAY_EDGE_x1] = tmp[DELAY_EDGE_z1] > tmp[DELAY_EDGE_01] ?
	                           tmp[DELAY_EDGE_z1] : tmp[DELAY_EDGE_01];
	      /* 1->x is the minimum of 1->z and 1->0. */
	    tmp[DELAY_EDGE_1x] = tmp[DELAY_EDGE_1z] < tmp[DELAY_EDGE_10] ?
	                           tmp[DELAY_EDGE_1z] : tmp[DELAY_EDGE_10];
	      /* x->0 is the maximum of z->0 and 1->0. */
	    tmp[DELAY_EDGE_x0] = tmp[DELAY_EDGE_z0] > tmp[DELAY_EDGE_10] ?
	                           tmp[DELAY_EDGE_z0] : tmp[DELAY_EDGE_10];
	      /* x->z is the maximum of 1->z and 0->z. */
	    tmp[DELAY_EDGE_xz] = tmp[DELAY_EDGE_1z] > tmp[DELAY_EDGE_0z] ?
	                           tmp[DELAY_EDGE_1z] : tmp[DELAY_EDGE_0z];
	      /* z->x is the minimum of z->1 and z->0. */
	    tmp[DELAY_EDGE_zx] = tmp[DELAY_EDGE_z1] < tmp[DELAY_EDGE_z0] ?
	                           tmp[DELAY_EDGE_z1] : tmp[DELAY_EDGE_z0];
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
      ಸಂರಚನಾ __vpiModPathSrc*src = dynamic_cast<__vpiModPathSrc*>(ref) ;
      ಪ್ರತಿಜ್ಞಾ(src);

      vvp_fun_modpath_src *fun = dynamic_cast<vvp_fun_modpath_src*>(src->net->fun);
      ಪ್ರತಿಜ್ಞಾ(fun);

      ಪೂರ್ಣಾಙ್ಕ idx;
      vvp_time64_t tmp[12];
      fun->get_delay12(tmp);

      ವಿಕಲ್ಪ (delays->no_of_delays) {
	  ಪಕ್ಷ 1:
	  ಪಕ್ಷ 2:
	  ಪಕ್ಷ 3:
	  ಪಕ್ಷ 6:
	  ಪಕ್ಷ 12:
	    ಭಙ್ಗ;

	  ಸ್ವಭಾವ:
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಭಙ್ಗ;
      }

      ಯದಿ (delays->time_type == vpiSimTime) {
	    ಪ್ರತಿ (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  vpip_time_to_timestruct(delays->da+idx, tmp[idx]);
	    }
      } ಅನ್ಯಥಾ {
	    ಪ್ರತಿ (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  delays->da[idx].real = vpip_time_to_scaled_real(tmp[idx], src->dest->scope);
	    }
      }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ pathterm_get(ಪೂರ್ಣಾಙ್ಕ code, vpiHandle ref)
{
      ಸಂರಚನಾ __vpiModPathTerm*obj = dynamic_cast<__vpiModPathTerm*>(ref);
      ಪ್ರತಿಜ್ಞಾ(obj);

      ವಿಕಲ್ಪ (code) {
	  ಪಕ್ಷ vpiEdge:
	    ಪ್ರತ್ಯಾಗಮ obj->edge;
	  ಸ್ವಭಾವ:
	    ಪ್ರತ್ಯಾಗಮ 0;
      }
}

ಸ್ಥಿರ vpiHandle pathterm_get_handle(ಪೂರ್ಣಾಙ್ಕ code, vpiHandle ref)
{
      ಸಂರಚನಾ __vpiModPathTerm*obj = dynamic_cast<__vpiModPathTerm*>(ref);
      ಪ್ರತಿಜ್ಞಾ(obj);

      ವಿಕಲ್ಪ (code) {
	  ಪಕ್ಷ vpi_ಅಭಿವ್ಯಕ್ತಿ:
	    ಪ್ರತ್ಯಾಗಮ obj->ಅಭಿವ್ಯಕ್ತಿ;
	  ಸ್ವಭಾವ:
	    ಪ್ರತ್ಯಾಗಮ 0;
      }
}

/*
* The __vpiModPathSrc class is what the VPI client sees as a
* vpiModPath object. The __vpiModPath structure contains items that
* are common to a bunch of modpaths, including the destination term.
*/
ಅನ್ತಃಸ್ಥ __vpiModPathSrc::__vpiModPathSrc()
{ }

ಪೂರ್ಣಾಙ್ಕ __vpiModPathSrc::get_type_code(ಶೂನ್ಯ) ಧ್ರುವ
{ ಪ್ರತ್ಯಾಗಮ vpiModPath; }

ಪೂರ್ಣಾಙ್ಕ __vpiModPathSrc::vpi_get(ಪೂರ್ಣಾಙ್ಕ code)
{ ಪ್ರತ್ಯಾಗಮ modpath_src_get(code, ಏತತ್); }

ಶೂನ್ಯ __vpiModPathSrc::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val)
{ modpath_src_get_ಮೌಲ್ಯ(ಏತತ್, val); }

vpiHandle __vpiModPathSrc::vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಙ್ಕ flags)
{ ಪ್ರತ್ಯಾಗಮ modpath_src_put_ಮೌಲ್ಯ(ಏತತ್, val, flags); }

vpiHandle __vpiModPathSrc::vpi_handle(ಪೂರ್ಣಾಙ್ಕ code)
{ ಪ್ರತ್ಯಾಗಮ modpath_src_get_handle(code, ಏತತ್); }

vpiHandle __vpiModPathSrc::vpi_iterate(ಪೂರ್ಣಾಙ್ಕ code)
{ ಪ್ರತ್ಯಾಗಮ modpath_src_iterate(code, ಏತತ್); }

vpiHandle __vpiModPathSrc:: vpi_index(ಪೂರ್ಣಾಙ್ಕ idx)
{ ಪ್ರತ್ಯಾಗಮ modpath_src_index(ಏತತ್, idx); }

ಶೂನ್ಯ __vpiModPathSrc::vpi_get_delays(p_vpi_delay del)
{ modpath_src_get_delays(ಏತತ್, del); }

ಶೂನ್ಯ __vpiModPathSrc::vpi_put_delays(p_vpi_delay del)
{ modpath_src_put_delays(ಏತತ್, del); }

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ modpath_src_free_object( vpiHandle ref )
{
      delete ref;
      ಪ್ರತ್ಯಾಗಮ 1 ;
}

__vpiHandle::free_object_fun_t __vpiModPathSrc::free_object_fun(ಶೂನ್ಯ)
{ ಪ್ರತ್ಯಾಗಮ &modpath_src_free_object; }


ಅನ್ತಃಸ್ಥ __vpiModPathTerm::__vpiModPathTerm()
{ }

ಪೂರ್ಣಾಙ್ಕ __vpiModPathTerm::get_type_code(ಶೂನ್ಯ) ಧ್ರುವ
{ ಪ್ರತ್ಯಾಗಮ vpiPathTerm; }

ಪೂರ್ಣಾಙ್ಕ __vpiModPathTerm::vpi_get(ಪೂರ್ಣಾಙ್ಕ code)
{ ಪ್ರತ್ಯಾಗಮ pathterm_get(code, ಏತತ್); }

vpiHandle __vpiModPathTerm::vpi_handle(ಪೂರ್ಣಾಙ್ಕ code)
{ ಪ್ರತ್ಯಾಗಮ pathterm_get_handle(code, ಏತತ್); }

ಸ್ಥಿರ ಶೂನ್ಯ initialize_path_term(ಸಂರಚನಾ __vpiModPathTerm&obj)
{
      obj.ಅಭಿವ್ಯಕ್ತಿ = 0;
      obj.edge = vpiNoEdge;
}

/*
 * This function will construct a vpiModPath Object.
 * give a respective "net", and will point to his
 * respective functor
 */

#ifdef CHECK_WITH_VALGRIND
ಸ್ಥಿರ ಸಂರಚನಾ __vpiModPath**mp_list = 0;
ಸ್ಥಿರ unsigned mp_ಎಣಿಕೆ = 0;
#endif

ಸಂರಚನಾ __vpiModPath* vpip_make_modpath(vvp_net_t *net)
{
      ಸಂರಚನಾ __vpiModPath*obj = ನೂತನ __vpiModPath;
      obj->scope = vpip_peek_current_scope ( );

      initialize_path_term(obj->path_term_out);
      obj->ಒಳಹರಿವು_net = net ;

#ifdef CHECK_WITH_VALGRIND
      mp_ಎಣಿಕೆ += 1;
      mp_list = static_cast<ಸಂರಚನಾ __vpiModPath **>
                (realloc(mp_list,
                 mp_ಎಣಿಕೆ*ಪರಿಮಾಣ(ಸಂರಚನಾ __vpiModPath **)));
      mp_list[mp_ಎಣಿಕೆ-1] = obj;
#endif
      ಪ್ರತ್ಯಾಗಮ obj;
}

#ifdef CHECK_WITH_VALGRIND
ಶೂನ್ಯ modpath_delete()
{
      ಪ್ರತಿ (unsigned idx = 0; idx < mp_ಎಣಿಕೆ; idx += 1) {
	    delete mp_list[idx];
      }
      ಮುಕ್ತ(mp_list);
      mp_list = 0;
      mp_ಎಣಿಕೆ = 0;
}
#endif

/*
 * This function will construct a vpiModPathIn
 * ( struct __vpiModPathSrc ) Object. will give
 * a delays[12] values, and point to the specified functor
 *
 */

ಸಂರಚನಾ __vpiModPathSrc* vpip_make_modpath_src(ಸಂರಚನಾ __vpiModPath*path,
                                              vvp_net_t *net)
{
      ಸಂರಚನಾ __vpiModPathSrc *obj = ನೂತನ __vpiModPathSrc;

      obj->dest = path;
      obj->type = 0;
      obj->net = net;
      initialize_path_term(obj->path_term_in);

      ಪ್ರತ್ಯಾಗಮ obj;
}

vvp_fun_intermodpath::vvp_fun_intermodpath(vvp_net_t*net, unsigned width)
: net_(net)
{
      ಪ್ರತಿ (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = 0;

      cur_vec4_ = vvp_vector4_t(width, BIT4_X);
      schedule_init_propagate(net_, cur_vec4_);
}

vvp_fun_intermodpath::~vvp_fun_intermodpath()
{
}

ಶೂನ್ಯ vvp_fun_intermodpath::get_delay12(vvp_time64_t val[12]) ಧ್ರುವ
{
      ಪ್ರತಿ (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    val[idx] = delay_[idx];
}

ಶೂನ್ಯ vvp_fun_intermodpath::put_delay12(ಧ್ರುವ vvp_time64_t val[12])
{
      ಪ್ರತಿ (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = val[idx];
}

/*
 * FIXME: this implementation currently only uses the maximum delay
 * from all the bit changes in the vectors. If there are multiple
 * changes with different delays, then the results would be
 * wrong. What should happen is that if there are multiple changes,
 * multiple vectors approaching the result should be scheduled.
 */
ಶೂನ್ಯ vvp_fun_intermodpath::recv_vec4(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
                              vvp_context_t)
{
	/* Only the first port is used. */
      ಯದಿ (port.port() > 0)
	    ಪ್ರತ್ಯಾಗಮ;

      ಯದಿ (cur_vec4_.eeq(bit))
	    ಪ್ರತ್ಯಾಗಮ;

	/* Given the scheduled output time, create an output event. */
      vvp_time64_t use_delay = delay_from_edge(cur_vec4_.value(0),
					       bit.value(0),
					       delay_);

	/* FIXME: This bases the edge delay on only the least
	   bit. This is WRONG! I need to find all the possible delays,
	   and schedule an event for each partial change. Hard! */
      ಪ್ರತಿ (unsigned idx = 1 ;  idx < bit.size() ;  idx += 1) {
	    vvp_time64_t tmp = delay_from_edge(cur_vec4_.value(idx),
					       bit.value(idx),
					       delay_);
	      /* If the current and new bit values match then no delay
	       * is needed for this bit. */
	    ಯದಿ (cur_vec4_.value(idx) == bit.value(idx)) ಅನುವರ್ತ;
	    ಪ್ರತಿಜ್ಞಾ(tmp == use_delay);
      }

      cur_vec4_ = bit;
      schedule_generic(ಏತತ್, use_delay, ಅಸತ್ಯ);
}

ಶೂನ್ಯ vvp_fun_intermodpath::run_run()
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
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ intermodpath_get(ಪೂರ್ಣಾಙ್ಕ, vpiHandle ref)
{
      ಸಂರಚನಾ __vpiInterModPath*obj =dynamic_cast<__vpiInterModPath*>(ref);
      ಪ್ರತಿಜ್ಞಾ(obj);
      ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ intermodpath_get_ಮೌಲ್ಯ(vpiHandle ref, p_vpi_ಮೌಲ್ಯ)
{
      ಸಂರಚನಾ __vpiInterModPath* intermodpath = dynamic_cast<__vpiInterModPath*>(ref);
      ಪ್ರತಿಜ್ಞಾ(intermodpath);
      ಪ್ರತ್ಯಾಗಮ;
}

ಸ್ಥಿರ vpiHandle intermodpath_put_ಮೌಲ್ಯ(vpiHandle ref, s_vpi_ಮೌಲ್ಯ *, ಪೂರ್ಣಾಙ್ಕ )
{
      ಸಂರಚನಾ __vpiInterModPath* intermodpath = dynamic_cast<__vpiInterModPath*>(ref);
      ಪ್ರತಿಜ್ಞಾ(intermodpath);
      ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ vpiHandle intermodpath_get_handle(ಪೂರ್ಣಾಙ್ಕ code, vpiHandle ref)
{
      ಸಂರಚನಾ __vpiInterModPath*rfp = dynamic_cast<__vpiInterModPath*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ವಿಕಲ್ಪ (code) {

	ಪಕ್ಷ vpiScope:
	  ಪ್ರತ್ಯಾಗಮ rfp->scope;

	  ಪಕ್ಷ vpiModule:
	      { __vpiScope*scope = rfp->scope;
		ಯಾವತ್ (scope && scope->get_type_code() != vpiModule)
		      scope = scope->scope;
		ಪ್ರತಿಜ್ಞಾ(scope);
		ಪ್ರತ್ಯಾಗಮ scope;
	      }
      }
      ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ vpiHandle intermodpath_iterate(ಪೂರ್ಣಾಙ್ಕ code, vpiHandle ref)
{
      ಸಂರಚನಾ __vpiInterModPath*rfp = dynamic_cast<__vpiInterModPath*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

	// For now intermodpaths only support exactly two ports
      ವಿಕಲ್ಪ (code) {
	  ಪಕ್ಷ vpiPort: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(2, ಪರಿಮಾಣ(vpiHandle*)));
	    args[0] = rfp->port1;
	    args[1] = rfp->port2;
	    ಪ್ರತ್ಯಾಗಮ vpip_make_iterator(2, args, ಸತ್ಯ);
	  }
      }
      ಪ್ರತ್ಯಾಗಮ 0;
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
      ಪೂರ್ಣಾಙ್ಕ idx;
      ಸಂರಚನಾ __vpiInterModPath * src = dynamic_cast<__vpiInterModPath*>(ref) ;
      ಪ್ರತಿಜ್ಞಾ(src) ;

      vvp_fun_intermodpath *fun = dynamic_cast<vvp_fun_intermodpath*>(src->net->fun);
      ಪ್ರತಿಜ್ಞಾ( fun );

      ಪ್ರಕಾರನಿರ್ದೇಶ unsigned ವರ್ಣ map_array_t[12];
	// Only the first six entries are used for the less than twelve maps.
      ಸ್ಥಿರ ಧ್ರುವ map_array_t map_1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      ಸ್ಥಿರ ಧ್ರುವ map_array_t map_2 = {0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0};
      ಸ್ಥಿರ ಧ್ರುವ map_array_t map_3 = {0, 1, 2, 0, 2, 1, 0, 0, 0, 0, 0, 0};
      ಸ್ಥಿರ ಧ್ರುವ map_array_t map_6 = {0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0};
      ಸ್ಥಿರ ಧ್ರುವ map_array_t map12 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

      ಧ್ರುವ map_array_t*use_map = 0;
      ವಿಕಲ್ಪ (delays->no_of_delays) {
	  ಪಕ್ಷ 1:
	    use_map = &map_1;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ 2:
	    use_map = &map_2;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ 3:
	    use_map = &map_3;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ 6:
	    use_map = &map_6;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ 12:
	    use_map = &map12;
	    ಭಙ್ಗ;
	  ಸ್ವಭಾವ:
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಭಙ್ಗ;
      }

      ಯದಿ (delays->time_type == vpiSimTime) {
	    ಪ್ರತಿ (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_timestruct_to_time(delays->da+use_map[0][idx]);
	    }
      } ಅನ್ಯಥಾ {
	      // You cannot create a modpath with a negative delay so set it
	      // to zero per 1364-2005 section 14.3.1.
	    ಪ್ರತಿ (idx = 0 ; idx < delays->no_of_delays ; idx += 1) {
		  ಯದಿ (delays->da[idx].real < 0.0) delays->da[idx].real = 0.0;
	    }
	    ಪ್ರತಿ (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_scaled_real_to_time64(delays->da[use_map[0][idx]].real,
							src->scope);
	    }
      }

      /* Now define the to-from-x delays if needed. */
      ಯದಿ (delays->no_of_delays <= 6) {
	      /* 0->x is the minimum of 0->z and 0->1. */
	    tmp[DELAY_EDGE_0x] = tmp[DELAY_EDGE_0z] < tmp[DELAY_EDGE_01] ?
	                           tmp[DELAY_EDGE_0z] : tmp[DELAY_EDGE_01];
	      /* x->1 is the maximum of z->1 and 0->1. */
	    tmp[DELAY_EDGE_x1] = tmp[DELAY_EDGE_z1] > tmp[DELAY_EDGE_01] ?
	                           tmp[DELAY_EDGE_z1] : tmp[DELAY_EDGE_01];
	      /* 1->x is the minimum of 1->z and 1->0. */
	    tmp[DELAY_EDGE_1x] = tmp[DELAY_EDGE_1z] < tmp[DELAY_EDGE_10] ?
	                           tmp[DELAY_EDGE_1z] : tmp[DELAY_EDGE_10];
	      /* x->0 is the maximum of z->0 and 1->0. */
	    tmp[DELAY_EDGE_x0] = tmp[DELAY_EDGE_z0] > tmp[DELAY_EDGE_10] ?
	                           tmp[DELAY_EDGE_z0] : tmp[DELAY_EDGE_10];
	      /* x->z is the maximum of 1->z and 0->z. */
	    tmp[DELAY_EDGE_xz] = tmp[DELAY_EDGE_1z] > tmp[DELAY_EDGE_0z] ?
	                           tmp[DELAY_EDGE_1z] : tmp[DELAY_EDGE_0z];
	      /* z->x is the minimum of z->1 and z->0. */
	    tmp[DELAY_EDGE_zx] = tmp[DELAY_EDGE_z1] < tmp[DELAY_EDGE_z0] ?
	                           tmp[DELAY_EDGE_z1] : tmp[DELAY_EDGE_z0];
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
      ಸಂರಚನಾ __vpiInterModPath*src = dynamic_cast<__vpiInterModPath*>(ref) ;
      ಪ್ರತಿಜ್ಞಾ(src);

      vvp_fun_intermodpath *fun = dynamic_cast<vvp_fun_intermodpath*>(src->net->fun);
      ಪ್ರತಿಜ್ಞಾ(fun);

      ಪೂರ್ಣಾಙ್ಕ idx;
      vvp_time64_t tmp[12];
      fun->get_delay12(tmp);

      ವಿಕಲ್ಪ (delays->no_of_delays) {
	  ಪಕ್ಷ 1:
	  ಪಕ್ಷ 2:
	  ಪಕ್ಷ 3:
	  ಪಕ್ಷ 6:
	  ಪಕ್ಷ 12:
	    ಭಙ್ಗ;

	  ಸ್ವಭಾವ:
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಭಙ್ಗ;
      }

      ಯದಿ (delays->time_type == vpiSimTime) {
	    ಪ್ರತಿ (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  vpip_time_to_timestruct(delays->da+idx, tmp[idx]);
	    }
      } ಅನ್ಯಥಾ {
	    ಪ್ರತಿ (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  delays->da[idx].real = vpip_time_to_scaled_real(tmp[idx], src->scope);
	    }
      }
}

/*
* The __vpiInterModPath class is what the VPI client sees as a
* vpiInterModPath object.
*/
ಅನ್ತಃಸ್ಥ __vpiInterModPath::__vpiInterModPath()
{ }

ಪೂರ್ಣಾಙ್ಕ __vpiInterModPath::get_type_code(ಶೂನ್ಯ) ಧ್ರುವ
{ ಪ್ರತ್ಯಾಗಮ vpiInterModPath; }

ಪೂರ್ಣಾಙ್ಕ __vpiInterModPath::vpi_get(ಪೂರ್ಣಾಙ್ಕ code)
{ ಪ್ರತ್ಯಾಗಮ intermodpath_get(code, ಏತತ್); }

ಶೂನ್ಯ __vpiInterModPath::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val)
{ intermodpath_get_ಮೌಲ್ಯ(ಏತತ್, val); }

vpiHandle __vpiInterModPath::vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಙ್ಕ flags)
{ ಪ್ರತ್ಯಾಗಮ intermodpath_put_ಮೌಲ್ಯ(ಏತತ್, val, flags); }

vpiHandle __vpiInterModPath::vpi_handle(ಪೂರ್ಣಾಙ್ಕ code)
{ ಪ್ರತ್ಯಾಗಮ intermodpath_get_handle(code, ಏತತ್); }

vpiHandle __vpiInterModPath::vpi_iterate(ಪೂರ್ಣಾಙ್ಕ code)
{ ಪ್ರತ್ಯಾಗಮ intermodpath_iterate(code, ಏತತ್); }

ಶೂನ್ಯ __vpiInterModPath::vpi_get_delays(p_vpi_delay del)
{ intermodpath_get_delays(ಏತತ್, del); }

ಶೂನ್ಯ __vpiInterModPath::vpi_put_delays(p_vpi_delay del)
{ intermodpath_put_delays(ಏತತ್, del); }

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ intermodpath_free_object( vpiHandle ref )
{
      delete ref;
      ಪ್ರತ್ಯಾಗಮ 1 ;
}

__vpiHandle::free_object_fun_t __vpiInterModPath::free_object_fun(ಶೂನ್ಯ)
{ ಪ್ರತ್ಯಾಗಮ &intermodpath_free_object; }

/*
 * This function will construct a vpiInterModPath Object.
 * give a respective "net", and will point to his
 * respective functor
 */

#ifdef CHECK_WITH_VALGRIND
ಸ್ಥಿರ ಸಂರಚನಾ __vpiInterModPath**imp_list = 0;
ಸ್ಥಿರ unsigned imp_ಎಣಿಕೆ = 0;
#endif

ಸಂರಚನಾ __vpiInterModPath* vpip_make_intermodpath(vvp_net_t *net, vpiPortInfo* port1, vpiPortInfo* port2)
{
      ಸಂರಚನಾ __vpiInterModPath*obj = ನೂತನ __vpiInterModPath;
      obj->scope = vpip_peek_current_scope ( );

      obj->net = net;
      obj->port1 = port1;
      obj->port2 = port2;

#ifdef CHECK_WITH_VALGRIND
      imp_ಎಣಿಕೆ += 1;
      imp_list = static_cast<ಸಂರಚನಾ __vpiInterModPath **>
                 (realloc(imp_list,
                  imp_ಎಣಿಕೆ*ಪರಿಮಾಣ(ಸಂರಚನಾ __vpiInterModPath **)));
      imp_list[imp_ಎಣಿಕೆ-1] = obj;
#endif
      ಪ್ರತ್ಯಾಗಮ obj;
}

#ifdef CHECK_WITH_VALGRIND
ಶೂನ್ಯ intermodpath_delete()
{
      ಪ್ರತಿ (unsigned idx = 0; idx < mp_ಎಣಿಕೆ; idx += 1) {
	    delete imp_list[idx];
      }
      ಮುಕ್ತ(imp_list);
      imp_list = 0;
      imp_ಎಣಿಕೆ = 0;
}
#endif
