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

#ਅਨ੍ਤਰ੍ਭਾਵ "delay.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "schedule.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vpi_priv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
#ਅਨ੍ਤਰ੍ਭਾਵ "vvp_cleanup.h"
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <iostream>
#ਅਨ੍ਤਰ੍ਭਾਵ <cstdlib>
#ਅਨ੍ਤਰ੍ਭਾਵ <list>
#ਅਨ੍ਤਰ੍ਭਾਵ <cassert>
#ਅਨ੍ਤਰ੍ਭਾਵ <cmath>
#ਅਨ੍ਤਰ੍ਭਾਵ "ivl_alloc.h"

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ std;

ਖਾਲੀ vvp_delay_t::calculate_min_delay_()
{
      min_delay_ = rise_;
      ਜੇ (fall_ < min_delay_)
	    min_delay_ = fall_;
      ਜੇ (ignore_decay_) decay_ = min_delay_;
      ਨਹੀਂਤਾਂ ਜੇ (decay_ < min_delay_)
	    min_delay_ = decay_;
}

vvp_delay_t::vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall)
{
      rise_ = rise;
      fall_ = fall;
      decay_= fall < rise? fall : rise;
      min_delay_ = decay_;
      ignore_decay_ = ਝੂਠ;
}

vvp_delay_t::vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall, vvp_time64_t decay)
{
      rise_ = rise;
      fall_ = fall;
      decay_= decay;
      ignore_decay_ = ਝੂਠ;

      calculate_min_delay_();
}

ਖਾਲੀ vvp_delay_t::ਸੈੱਟ_ignore_decay()
{
      ignore_decay_ = ਸੱਚ;

      calculate_min_delay_();
}

vvp_delay_t::~vvp_delay_t()
{
}

vvp_time64_t vvp_delay_t::get_delay(vvp_bit4_t from, vvp_bit4_t to)
{
      ਚੋਣ (from) {
	  ਹਾਲਤ BIT4_0:
	    ਚੋਣ (to) {
		ਹਾਲਤ BIT4_0: ਵਾਪਸ 0;
		ਹਾਲਤ BIT4_1: ਵਾਪਸ rise_;
		ਹਾਲਤ BIT4_X: ਵਾਪਸ min_delay_;
		ਹਾਲਤ BIT4_Z: ਵਾਪਸ decay_;
	    }
	    ਰੋਕੋ;
	  ਹਾਲਤ BIT4_1:
	    ਚੋਣ (to) {
		ਹਾਲਤ BIT4_0: ਵਾਪਸ fall_;
		ਹਾਲਤ BIT4_1: ਵਾਪਸ 0;
		ਹਾਲਤ BIT4_X: ਵਾਪਸ min_delay_;
		ਹਾਲਤ BIT4_Z: ਵਾਪਸ decay_;
	    }
	    ਰੋਕੋ;
	  ਹਾਲਤ BIT4_X:
	    ਚੋਣ (to) {
		ਹਾਲਤ BIT4_0: ਵਾਪਸ fall_;
		ਹਾਲਤ BIT4_1: ਵਾਪਸ rise_;
		ਹਾਲਤ BIT4_X: ਵਾਪਸ 0;
		ਹਾਲਤ BIT4_Z: ਵਾਪਸ decay_;
	    }
	    ਰੋਕੋ;
	  ਹਾਲਤ BIT4_Z:
	    ਚੋਣ (to) {
		ਹਾਲਤ BIT4_0: ਵਾਪਸ fall_;
		ਹਾਲਤ BIT4_1: ਵਾਪਸ rise_;
		ਹਾਲਤ BIT4_X: ਵਾਪਸ min_delay_;
		ਹਾਲਤ BIT4_Z: ਵਾਪਸ 0;
	    }
	    ਰੋਕੋ;
      }

      ਪ੍ਰਤਿਜ੍ਞਾ(0);
      ਵਾਪਸ 0;
}

vvp_time64_t vvp_delay_t::get_min_delay() ਅਚਲ
{
      ਵਾਪਸ min_delay_;
}

ਖਾਲੀ vvp_delay_t::ਸੈੱਟ_rise(vvp_time64_t val)
{
      rise_ = val;
      ਜੇ (val < min_delay_) {
	    min_delay_ = val;
	    ਜੇ (ignore_decay_) decay_ = val;
      } ਨਹੀਂਤਾਂ
	    calculate_min_delay_();
}

ਖਾਲੀ vvp_delay_t::ਸੈੱਟ_fall(vvp_time64_t val)
{
      fall_ = val;
      ਜੇ (val < min_delay_) {
	    min_delay_ = val;
	    ਜੇ (ignore_decay_) decay_ = val;
      } ਨਹੀਂਤਾਂ
	    calculate_min_delay_();
}

ਖਾਲੀ vvp_delay_t::ਸੈੱਟ_decay(vvp_time64_t val)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(!ignore_decay_);

      decay_ = val;
      ਜੇ (val < min_delay_)
	    min_delay_ = val;
      ਨਹੀਂਤਾਂ
	    calculate_min_delay_();
}

vvp_fun_delay::vvp_fun_delay(vvp_net_t*n, ਬਿਨਾਨਿਸ਼ਾਨੀ width, ਅਚਲ vvp_delay_t&d)
: net_(n), delay_(d)
{
      cur_real_ = 0.0;
      ਜੇ (width > 0) {
            cur_vec4_ = vvp_vector4_t(width, BIT4_X);
            cur_vec8_ = vvp_vector8_t(cur_vec4_, 6, 6);
            ਸਮਾਂ_ਸਾਰਣੀ_init_propagate(net_, cur_vec4_);
      } ਨਹੀਂਤਾਂ {
            ਸਮਾਂ_ਸਾਰਣੀ_init_propagate(net_, cur_real_);
      }
      list_ = 0;
      type_ = UNKNOWN_DELAY;
      initial_ = ਸੱਚ;
	// Calculate the values used when converting variable delays
	// to simulation time units.
      ਅਚਲ __vpiScope*scope = vpip_peek_current_scope();

      ਪੂਰਨ powr = scope->ਸਮਾਂ_units - scope->ਸਮਾਂ_precision;
      ਗੇੜਾ = 1;
      ਲਈ (ਪੂਰਨ lp = 0; lp < powr; lp += 1) ਗੇੜਾ *= 10;

      powr = scope->ਸਮਾਂ_precision - vpip_get_ਸਮਾਂ_precision();
      scale_ = 1;
      ਲਈ (ਪੂਰਨ lp = 0; lp < powr; lp += 1) scale_ *= 10;
}

vvp_fun_delay::~vvp_fun_delay()
{
      ਜਦੋਂ (ਬਣਤਰ event_*cur = ਕਤਾਰ_ਤੋਂ_ਕੱਢੋ())
	    ਮਿਟਾਓ cur;
}

ਤਾਰਕਿਕ vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ)
{
      ਜੇ (list_ == 0) ਵਾਪਸ ਝੂਠ;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      ਜੇ (list_->ਅਗਲਾ_2->ptr_vec4.eeq(ਦੋਆਧਾਰੀ_ਅੰਕ)) ਵਾਪਸ ਸੱਚ;

      clean_pulse_events_(use_delay);
      ਵਾਪਸ ਝੂਠ;
}

ਤਾਰਕਿਕ vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ)
{
      ਜੇ (list_ == 0) ਵਾਪਸ ਝੂਠ;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      ਜੇ (list_->ਅਗਲਾ_2->ptr_vec8.eeq(ਦੋਆਧਾਰੀ_ਅੰਕ)) ਵਾਪਸ ਸੱਚ;

      clean_pulse_events_(use_delay);
      ਵਾਪਸ ਝੂਠ;
}

ਤਾਰਕਿਕ vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ)
{
      ਜੇ (list_ == 0) ਵਾਪਸ ਝੂਠ;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      ਜੇ (list_->ਅਗਲਾ_2->ptr_real == ਦੋਆਧਾਰੀ_ਅੰਕ) ਵਾਪਸ ਸੱਚ;

      clean_pulse_events_(use_delay);
      ਵਾਪਸ ਝੂਠ;
}

ਖਾਲੀ vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(list_ != 0);

      ਕਰੋ {
	    ਬਣਤਰ event_*cur = list_->ਅਗਲਾ_2;
	      /* If this event is far enough from the event I'm about
	         to create, then that scheduled event is not a pulse
	         to be eliminated, so we're done. */
	    ਜੇ (cur->sim_ਸਮਾਂ+use_delay <= use_delay+ਸਮਾਂ_ਸਾਰਣੀ_simtime())
		  ਰੋਕੋ;

	    ਜੇ (list_ == cur)
		  list_ = 0;
	    ਨਹੀਂਤਾਂ
		  list_->ਅਗਲਾ_2 = cur->ਅਗਲਾ_2;
	    ਮਿਟਾਓ cur;
      } ਜਦੋਂ (list_);
}

/*
 * FIXME: this implementation currently only uses the maximum delay
 * from all the bit changes in the vectors. If there are multiple
 * changes with different delays, then the results would be
 * wrong. What should happen is that if there are multiple changes,
 * multiple vectors approaching the result should be scheduled.
 */
ਖਾਲੀ vvp_fun_delay::recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                              vvp_context_t)
{
      ਜੇ (ਪੋਰਟ.ਪੋਰਟ() > 0) {
	      // Get the integer value of the bit vector, or 0 if
	      // there are X or Z bits.
	    vvp_time64_t bval = 0;
	      // The following does not work correctly for negative values.
	      // They should be sign extended to 64 bits (1364-2001 9.7.1).
	    vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(ਦੋਆਧਾਰੀ_ਅੰਕ, bval);
	      // Integer values do not need to be rounded so just scale them.
	    vvp_time64_t val = bval * ਗੇੜਾ * scale_;

	    ਚੋਣ (ਪੋਰਟ.ਪੋਰਟ()) {
		ਹਾਲਤ 1:
		  delay_.ਸੈੱਟ_rise(val);
		  ਵਾਪਸ;
		ਹਾਲਤ 2:
		  delay_.ਸੈੱਟ_fall(val);
		  ਵਾਪਸ;
		ਹਾਲਤ 3:
		  delay_.ਸੈੱਟ_decay(val);
		  ਵਾਪਸ;
	    }
	    ਵਾਪਸ;
      }

      vvp_time64_t use_delay;
	/* This is an initial value so it needs to be compared to all the
	   bits (the order the bits are changed is not deterministic). */
      ਜੇ (initial_) {
	    type_ = VEC4_DELAY;
            cur_vec8_ = vvp_vector8_t(vvp_vector4_t(0, BIT4_X), 6, 6);
	    vvp_bit4_t cur_val = cur_vec4_.value(0);
	    use_delay = delay_.get_delay(cur_val, ਦੋਆਧਾਰੀ_ਅੰਕ.value(0));
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 1 ;  idx < ਦੋਆਧਾਰੀ_ਅੰਕ.size() ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(cur_val, ਦੋਆਧਾਰੀ_ਅੰਕ.value(idx));
		  ਜੇ (tmp > use_delay) use_delay = tmp;
	    }
      } ਨਹੀਂਤਾਂ {
	    ਪ੍ਰਤਿਜ੍ਞਾ(type_ == VEC4_DELAY);

	      // Use as a reference for calculating the delay the
	      // current value of the output. Detect and handle the
	      // special case that the event list contains the current
	      // value as a zero-delay-remaining event.
	    ਅਚਲ vvp_vector4_t&use_vec4 = (list_ && list_->ਅਗਲਾ_2->sim_ਸਮਾਂ == ਸਮਾਂ_ਸਾਰਣੀ_simtime())? list_->ਅਗਲਾ_2->ptr_vec4 : cur_vec4_;

	      /* How many bits to compare? */
	    ਬਿਨਾਨਿਸ਼ਾਨੀ use_wid = use_vec4.size();
	    ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.size() < use_wid) use_wid = ਦੋਆਧਾਰੀ_ਅੰਕ.size();

	      /* Scan the vectors looking for delays. Select the maximum
	         delay encountered. */
	    use_delay = delay_.get_delay(use_vec4.value(0), ਦੋਆਧਾਰੀ_ਅੰਕ.value(0));

	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 1 ;  idx < use_wid ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(use_vec4.value(idx), ਦੋਆਧਾਰੀ_ਅੰਕ.value(idx));
		  ਜੇ (tmp > use_delay) use_delay = tmp;
	    }
      }

      /* what *should* happen here is we check to see if there is a
         transaction in the queue. This would be a pulse that needs to be
         eliminated. */
      ਜੇ (clean_pulse_events_(use_delay, ਦੋਆਧਾਰੀ_ਅੰਕ)) ਵਾਪਸ;

      vvp_time64_t use_simtime = ਸਮਾਂ_ਸਾਰਣੀ_simtime() + use_delay;

	/* And propagate it. */
      ਜੇ (use_delay == 0 && list_ == 0) {
	    cur_vec4_ = ਦੋਆਧਾਰੀ_ਅੰਕ;
	    initial_ = ਝੂਠ;
	    net_->send_vec4(cur_vec4_, 0);
      } ਨਹੀਂਤਾਂ {
	    ਬਣਤਰ event_*cur = ਨਵਾਂ ਬਣਤਰ event_(use_simtime);
	    cur->ਲੜੀਵਾਰ_ਲੜੀਵਾਰ_ptr = &vvp_fun_delay::ਲੜੀਵਾਰ_ਲੜੀਵਾਰ_vec4;
	    cur->ptr_vec4 = ਦੋਆਧਾਰੀ_ਅੰਕ;
	    ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(cur);
	    ਸਮਾਂ_ਸਾਰਣੀ_generic(ਇਹ, use_delay, ਝੂਠ);
      }
}

ਖਾਲੀ vvp_fun_delay::recv_vec4_pv(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			         ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ, ਅਧਾਰ, vwid, ctx);
}

/* See the recv_vec4 comment above. */
ਖਾਲੀ vvp_fun_delay::recv_vec8(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ਪੋਰਟ.ਪੋਰਟ() == 0);

      vvp_time64_t use_delay;
	/* This is an initial value so it needs to be compared to all the
	   bits (the order the bits are changed is not deterministic). */
      ਜੇ (initial_) {
	    type_ = VEC8_DELAY;
            cur_vec4_ = vvp_vector4_t(0, BIT4_X);
	    vvp_bit4_t cur_val = cur_vec8_.value(0).value();
	    use_delay = delay_.get_delay(cur_val, ਦੋਆਧਾਰੀ_ਅੰਕ.value(0).value());
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 1 ;  idx < ਦੋਆਧਾਰੀ_ਅੰਕ.size() ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(cur_val, ਦੋਆਧਾਰੀ_ਅੰਕ.value(idx).value());
		  ਜੇ (tmp > use_delay) use_delay = tmp;
	    }
      } ਨਹੀਂਤਾਂ {
	    ਪ੍ਰਤਿਜ੍ਞਾ(type_ == VEC8_DELAY);

	      // Use as a reference for calculating the delay the
	      // current value of the output. Detect and handle the
	      // special case that the event list contains the current
	      // value as a zero-delay-remaining event.
	    ਅਚਲ vvp_vector8_t&use_vec8 = (list_ && list_->ਅਗਲਾ_2->sim_ਸਮਾਂ == ਸਮਾਂ_ਸਾਰਣੀ_simtime())? list_->ਅਗਲਾ_2->ptr_vec8 : cur_vec8_;

	      /* How many bits to compare? */
	    ਬਿਨਾਨਿਸ਼ਾਨੀ use_wid = use_vec8.size();
	    ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.size() < use_wid) use_wid = ਦੋਆਧਾਰੀ_ਅੰਕ.size();

	      /* Scan the vectors looking for delays. Select the maximum
	         delay encountered. */
	    use_delay = delay_.get_delay(use_vec8.value(0).value(),
	                                 ਦੋਆਧਾਰੀ_ਅੰਕ.value(0).value());

	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 1 ;  idx < use_wid ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(use_vec8.value(idx).value(),
		                         ਦੋਆਧਾਰੀ_ਅੰਕ.value(idx).value());
		  ਜੇ (tmp > use_delay) use_delay = tmp;
	    }
      }

      /* what *should* happen here is we check to see if there is a
         transaction in the queue. This would be a pulse that needs to be
         eliminated. */
      ਜੇ (clean_pulse_events_(use_delay, ਦੋਆਧਾਰੀ_ਅੰਕ)) ਵਾਪਸ;

      vvp_time64_t use_simtime = ਸਮਾਂ_ਸਾਰਣੀ_simtime() + use_delay;

	/* And propagate it. */
      ਜੇ (use_delay == 0 && list_ == 0) {
	    cur_vec8_ = ਦੋਆਧਾਰੀ_ਅੰਕ;
	    initial_ = ਝੂਠ;
	    net_->send_vec8(cur_vec8_);
      } ਨਹੀਂਤਾਂ {
	    ਬਣਤਰ event_*cur = ਨਵਾਂ ਬਣਤਰ event_(use_simtime);
	    cur->ptr_vec8 = ਦੋਆਧਾਰੀ_ਅੰਕ;
	    cur->ਲੜੀਵਾਰ_ਲੜੀਵਾਰ_ptr = &vvp_fun_delay::ਲੜੀਵਾਰ_ਲੜੀਵਾਰ_vec8;
	    ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(cur);
	    ਸਮਾਂ_ਸਾਰਣੀ_generic(ਇਹ, use_delay, ਝੂਠ);
      }
}

ਖਾਲੀ vvp_fun_delay::recv_vec8_pv(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			         ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid)
{
      recv_vec8_pv_(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ, ਅਧਾਰ, vwid);
}

ਖਾਲੀ vvp_fun_delay::recv_real(vvp_net_ptr_t ਪੋਰਟ, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                              vvp_context_t)
{
      ਜੇ (ਪੋਰਟ.ਪੋਰਟ() > 0) {
	    /* If the port is not 0, then this is a delay value that
	    should be rounded and converted to an integer delay. */
	    vvp_time64_t val = 0;
	    ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ > -0.5) {
		  val = (vvp_time64_t) (ਦੋਆਧਾਰੀ_ਅੰਕ * ਗੇੜਾ + 0.5) * scale_;
	    } ਨਹੀਂਤਾਂ ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ != ਦੋਆਧਾਰੀ_ਅੰਕ) {
		    // For a NaN we use the default (0).
	    } ਨਹੀਂਤਾਂ {
		  vvp_vector4_t vec4(8*ਆਕਾਰ(vvp_time64_t),
		                     floor(-ਦੋਆਧਾਰੀ_ਅੰਕ * ਗੇੜਾ + 0.5) * -1 * scale_);
		  vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(vec4, val);
	    }

	    ਚੋਣ (ਪੋਰਟ.ਪੋਰਟ()) {
		ਹਾਲਤ 1:
		  delay_.ਸੈੱਟ_rise(val);
		  ਵਾਪਸ;
		ਹਾਲਤ 2:
		  delay_.ਸੈੱਟ_fall(val);
		  ਵਾਪਸ;
		ਹਾਲਤ 3:
		  delay_.ਸੈੱਟ_decay(val);
		  ਵਾਪਸ;
	    }
	    ਵਾਪਸ;
      }

      ਜੇ (initial_) {
	    type_ = REAL_DELAY;
            cur_vec4_ = vvp_vector4_t(0, BIT4_X);
            cur_vec8_ = vvp_vector8_t(cur_vec4_, 6, 6);
      } ਨਹੀਂਤਾਂ ਪ੍ਰਤਿਜ੍ਞਾ(type_ == REAL_DELAY);

      vvp_time64_t use_delay;
      use_delay = delay_.get_min_delay();

      /* Eliminate glitches. */
      ਜੇ (clean_pulse_events_(use_delay, ਦੋਆਧਾਰੀ_ਅੰਕ)) ਵਾਪਸ;

      /* This must be done after cleaning pulses to avoid propagating
       * an incorrect value. */
      ਜੇ (cur_real_ == ਦੋਆਧਾਰੀ_ਅੰਕ) ਵਾਪਸ;

      vvp_time64_t use_simtime = ਸਮਾਂ_ਸਾਰਣੀ_simtime() + use_delay;

      ਜੇ (use_delay == 0 && list_ == 0) {
	    cur_real_ = ਦੋਆਧਾਰੀ_ਅੰਕ;
	    initial_ = ਝੂਠ;
	    net_->send_real(cur_real_, 0);
      } ਨਹੀਂਤਾਂ {
	    ਬਣਤਰ event_*cur = ਨਵਾਂ ਬਣਤਰ event_(use_simtime);
	    cur->ਲੜੀਵਾਰ_ਲੜੀਵਾਰ_ptr = &vvp_fun_delay::ਲੜੀਵਾਰ_ਲੜੀਵਾਰ_real;
	    cur->ptr_real = ਦੋਆਧਾਰੀ_ਅੰਕ;
	    ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(cur);

	    ਸਮਾਂ_ਸਾਰਣੀ_generic(ਇਹ, use_delay, ਝੂਠ);
      }
}

ਖਾਲੀ vvp_fun_delay::ਲੜੀਵਾਰ_ਲੜੀਵਾਰ()
{
      vvp_time64_t sim_ਸਮਾਂ = ਸਮਾਂ_ਸਾਰਣੀ_simtime();
      ਜੇ (list_ == 0 || list_->ਅਗਲਾ_2->sim_ਸਮਾਂ > sim_ਸਮਾਂ)
	    ਵਾਪਸ;

      ਬਣਤਰ event_*cur = ਕਤਾਰ_ਤੋਂ_ਕੱਢੋ();
      ਜੇ (cur == 0)
	    ਵਾਪਸ;

      (ਇਹ->*(cur->ਲੜੀਵਾਰ_ਲੜੀਵਾਰ_ptr))(cur);
      initial_ = ਝੂਠ;
      ਮਿਟਾਓ cur;
}

ਖਾਲੀ vvp_fun_delay::ਲੜੀਵਾਰ_ਲੜੀਵਾਰ_vec4(ਅਚਲ ਬਣਤਰ event_*cur)
{
      cur_vec4_ = cur->ptr_vec4;
      net_->send_vec4(cur_vec4_, 0);
}

ਖਾਲੀ vvp_fun_delay::ਲੜੀਵਾਰ_ਲੜੀਵਾਰ_vec8(ਅਚਲ ਬਣਤਰ vvp_fun_delay::event_*cur)
{
      cur_vec8_ = cur->ptr_vec8;
      net_->send_vec8(cur_vec8_);
}

ਖਾਲੀ vvp_fun_delay::ਲੜੀਵਾਰ_ਲੜੀਵਾਰ_real(ਅਚਲ ਬਣਤਰ vvp_fun_delay::event_*cur)
{
      cur_real_ = cur->ptr_real;
      net_->send_real(cur_real_, 0);
}

vvp_fun_modpath::vvp_fun_modpath(vvp_net_t*net, ਬਿਨਾਨਿਸ਼ਾਨੀ width)
: net_(net), src_list_(0), ifnone_list_(0)
{
      cur_vec4_ = vvp_vector4_t(width, BIT4_X);
      ਸਮਾਂ_ਸਾਰਣੀ_init_propagate(net_, cur_vec4_);
}

vvp_fun_modpath::~vvp_fun_modpath()
{
	// Delete the source probes.
      ਜਦੋਂ (src_list_) {
	    vvp_fun_modpath_src*tmp = src_list_;
	    src_list_ = tmp->ਅਗਲਾ;
	    ਮਿਟਾਓ tmp;
      }
      ਜਦੋਂ (ifnone_list_) {
	    vvp_fun_modpath_src*tmp = ifnone_list_;
	    ifnone_list_ = tmp->ਅਗਲਾ;
	    ਮਿਟਾਓ tmp;
      }
}

ਖਾਲੀ vvp_fun_modpath::ਜੋੜੋ_modpath_src(vvp_fun_modpath_src*that, ਤਾਰਕਿਕ ifnone)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(that->ਅਗਲਾ == 0);
      ਜੇ (ifnone) {
	    that->ਅਗਲਾ = ifnone_list_;
	    ifnone_list_ = that;
      } ਨਹੀਂਤਾਂ {
	    that->ਅਗਲਾ = src_list_;
	    src_list_ = that;
      }
}

ਸਥਿਰ vvp_time64_t delay_from_ਕਿਨਾਰਾ(vvp_bit4_t a, vvp_bit4_t b,
                                           vvp_time64_t ਅਚਲ array[12])
{
      ਕਿਸਮਪਰਿਭਾਸ਼ਾ delay_ਕਿਨਾਰਾ_t bit4_table4[4];
      ਸਥਿਰ ਅਚਲ bit4_table4 ਕਿਨਾਰਾ_ਸਾਰਣੀ[4] = {
	    { DELAY_ਕਿਨਾਰਾ_01, DELAY_ਕਿਨਾਰਾ_01, DELAY_ਕਿਨਾਰਾ_0z, DELAY_ਕਿਨਾਰਾ_0x },
	    { DELAY_ਕਿਨਾਰਾ_10, DELAY_ਕਿਨਾਰਾ_10, DELAY_ਕਿਨਾਰਾ_1z, DELAY_ਕਿਨਾਰਾ_1x },
	    { DELAY_ਕਿਨਾਰਾ_z0, DELAY_ਕਿਨਾਰਾ_z1, DELAY_ਕਿਨਾਰਾ_z0, DELAY_ਕਿਨਾਰਾ_zx },
	    { DELAY_ਕਿਨਾਰਾ_x0, DELAY_ਕਿਨਾਰਾ_x1, DELAY_ਕਿਨਾਰਾ_xz, DELAY_ਕਿਨਾਰਾ_x0 }
      };

      ਵਾਪਸ array[ ਕਿਨਾਰਾ_ਸਾਰਣੀ[a][b] ];
}

ਖਾਲੀ vvp_fun_modpath::recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                                vvp_context_t)
{
	/* Only the first port is used. */
      ਜੇ (ਪੋਰਟ.ਪੋਰਟ() > 0)
	    ਵਾਪਸ;

      ਜੇ (cur_vec4_.eeq(ਦੋਆਧਾਰੀ_ਅੰਕ))
	    ਵਾਪਸ;

	/* Select a time delay source that applies. Notice that there
	   may be multiple delay sources that apply, so collect all
	   the candidates into a list first. */
      list<vvp_fun_modpath_src*>candidate_list;
      vvp_time64_t candidate_wake_ਸਮਾਂ = 0;
      ਲਈ (vvp_fun_modpath_src*cur = src_list_ ;  cur ;  cur=cur->ਅਗਲਾ) {
	      /* Skip paths that are disabled by conditions. */
	    ਜੇ (cur->condition_flag_ == ਝੂਠ)
		  ਜਾਰੀ;

	    ਜੇ (candidate_list.empty()) {
		  candidate_list.push_back(cur);
		  candidate_wake_ਸਮਾਂ = cur->wake_ਸਮਾਂ;
	    } ਨਹੀਂਤਾਂ ਜੇ (cur->wake_ਸਮਾਂ == candidate_wake_ਸਮਾਂ) {
		  candidate_list.push_back(cur);
	    } ਨਹੀਂਤਾਂ ਜੇ (cur->wake_ਸਮਾਂ > candidate_wake_ਸਮਾਂ) {
		  candidate_list.assign(1, cur);
		  candidate_wake_ਸਮਾਂ = cur->wake_ਸਮਾਂ;
	    } ਨਹੀਂਤਾਂ {
		  ਜਾਰੀ; /* Skip this entry. */
	    }
      }

	/* Only add the ifnone delay if it has a later wake_time_ or
	 * if there are no normal delays. */
      vvp_time64_t ifnone_wake_ਸਮਾਂ = candidate_wake_ਸਮਾਂ;
      ਲਈ (vvp_fun_modpath_src*cur = ifnone_list_ ;  cur ;  cur=cur->ਅਗਲਾ) {
	    ਜੇ (candidate_list.empty()) {
		  candidate_list.push_back(cur);
		  ifnone_wake_ਸਮਾਂ = cur->wake_ਸਮਾਂ;
	    } ਨਹੀਂਤਾਂ ਜੇ (cur->wake_ਸਮਾਂ == ifnone_wake_ਸਮਾਂ &&
	               ifnone_wake_ਸਮਾਂ > candidate_wake_ਸਮਾਂ) {
		  candidate_list.push_back(cur);
	    } ਨਹੀਂਤਾਂ ਜੇ (cur->wake_ਸਮਾਂ > ifnone_wake_ਸਮਾਂ) {
		  candidate_list.assign(1, cur);
		  ifnone_wake_ਸਮਾਂ = cur->wake_ਸਮਾਂ;
	    } ਨਹੀਂਤਾਂ {
		  ਜਾਰੀ; /* Skip this entry. */
	    }
      }

	/* Handle the special case that there are no delays that
	   match. This may happen, for example, if the set of
	   conditional delays is incomplete, leaving some cases
	   uncovered. In that case, just pass the data without delay */
      ਜੇ (candidate_list.empty()) {
	    cur_vec4_ = ਦੋਆਧਾਰੀ_ਅੰਕ;
	    ਸਮਾਂ_ਸਾਰਣੀ_generic(ਇਹ, 0, ਝੂਠ);
	    ਵਾਪਸ;
      }

	/* Now given that we have a list of candidate delays, find for
	   each if the 12 numbers the minimum from all the
	   candidates. This minimum set becomes the chosen delay to
	   use. */
      vvp_time64_t ਬਾਹਰਲਾ_at[12];
      vvp_time64_t now = ਸਮਾਂ_ਸਾਰਣੀ_simtime();

      ਕਿਸਮਪਰਿਭਾਸ਼ਾ list<vvp_fun_modpath_src*>::const_iterator iter_t;

      iter_t cur = candidate_list.begin();
      ਅਚਲ vvp_fun_modpath_src*src = *cur;

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < 12 ;  idx += 1) {
	    ਬਾਹਰਲਾ_at[idx] = src->wake_ਸਮਾਂ + src->delay_[idx];
	    ਜੇ (ਬਾਹਰਲਾ_at[idx] <= now)
		  ਬਾਹਰਲਾ_at[idx] = 0;
	    ਨਹੀਂਤਾਂ
		  ਬਾਹਰਲਾ_at[idx] -= now;
      }

      ਲਈ (++ cur ; cur != candidate_list.ਸਿਰਾ() ; ++ cur ) {
	    src = *cur;
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < 12 ;  idx += 1) {
		  vvp_time64_t tmp = src->wake_ਸਮਾਂ + src->delay_[idx];
		  ਜੇ (tmp <= now)
			tmp = 0;
		  ਨਹੀਂਤਾਂ
			tmp -= now;
		  ਜੇ (tmp < ਬਾਹਰਲਾ_at[idx])
			ਬਾਹਰਲਾ_at[idx] = tmp;
	    }
      }

	/* Given the scheduled output time, create an output event. */
      vvp_time64_t use_delay = delay_from_ਕਿਨਾਰਾ(cur_vec4_.value(0),
					       ਦੋਆਧਾਰੀ_ਅੰਕ.value(0),
					       ਬਾਹਰਲਾ_at);

	/* FIXME: This bases the edge delay on only the least
	   bit. This is WRONG! I need to find all the possible delays,
	   and schedule an event for each partial change. Hard! */
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 1 ;  idx < ਦੋਆਧਾਰੀ_ਅੰਕ.size() ;  idx += 1) {
	    vvp_time64_t tmp = delay_from_ਕਿਨਾਰਾ(cur_vec4_.value(idx),
					       ਦੋਆਧਾਰੀ_ਅੰਕ.value(idx),
					       ਬਾਹਰਲਾ_at);
	      /* If the current and new bit values match then no delay
	       * is needed for this bit. */
	    ਜੇ (cur_vec4_.value(idx) == ਦੋਆਧਾਰੀ_ਅੰਕ.value(idx)) ਜਾਰੀ;
	    ਜੇ (tmp != use_delay) {
		  fprintf(stderr, "sorry: multi-bit module path delays are "
		                  "currently not fully supported.\n");
		  fprintf(stderr, "     : using the LSB delay for all bits.\n");
		  ਜਾਰੀ;
            }
      }

      cur_vec4_ = ਦੋਆਧਾਰੀ_ਅੰਕ;
      ਸਮਾਂ_ਸਾਰਣੀ_generic(ਇਹ, use_delay, ਝੂਠ);
}

ਖਾਲੀ vvp_fun_modpath::ਲੜੀਵਾਰ_ਲੜੀਵਾਰ()
{
      net_->send_vec4(cur_vec4_, 0);
}

vvp_fun_modpath_src::vvp_fun_modpath_src(vvp_time64_t ਅਚਲ del[12])
{
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = del[idx];

      ਅਗਲਾ = 0;
      wake_ਸਮਾਂ = 0;
      condition_flag_ = ਸੱਚ;
}

vvp_fun_modpath_src::~vvp_fun_modpath_src()
{
}

ਖਾਲੀ vvp_fun_modpath_src::get_delay12(vvp_time64_t val[12]) ਅਚਲ
{
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < 12 ;  idx += 1)
	    val[idx] = delay_[idx];
}

ਖਾਲੀ vvp_fun_modpath_src::put_delay12(ਅਚਲ vvp_time64_t val[12])
{
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = val[idx];
}

ਖਾਲੀ vvp_fun_modpath_src::recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                                    vvp_context_t)
{
      ਜੇ (ਪੋਰਟ.ਪੋਰਟ() == 0) {
	      // The modpath input...
	    ਜੇ (test_vec4(ਦੋਆਧਾਰੀ_ਅੰਕ))
		  wake_ਸਮਾਂ = ਸਮਾਂ_ਸਾਰਣੀ_simtime();

      } ਨਹੀਂਤਾਂ ਜੇ (ਪੋਰਟ.ਪੋਰਟ() == 1) {
	      // The modpath condition input...
	    ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.value(0) != BIT4_0)
		  condition_flag_ = ਸੱਚ;
	    ਨਹੀਂਤਾਂ
		  condition_flag_ = ਝੂਠ;
      }
}

ਤਾਰਕਿਕ vvp_fun_modpath_src::test_vec4(ਅਚਲ vvp_vector4_t&)
{
      ਵਾਪਸ ਸੱਚ;
}

vvp_fun_modpath_ਕਿਨਾਰਾ::vvp_fun_modpath_ਕਿਨਾਰਾ(vvp_time64_t del[12],
					   ਤਾਰਕਿਕ pos, ਤਾਰਕਿਕ neg)
: vvp_fun_modpath_src(del)
{
      old_ਮੁੱਲ = BIT4_X;
      posedge_ = pos;
      negedge_ = neg;
}

ਤਾਰਕਿਕ vvp_fun_modpath_ਕਿਨਾਰਾ::test_vec4(ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ)
{
      vvp_bit4_t tmp = old_ਮੁੱਲ;
      old_ਮੁੱਲ = ਦੋਆਧਾਰੀ_ਅੰਕ.value(0);

      ਪੂਰਨ ਕਿਨਾਰਾ_flag = ਕਿਨਾਰਾ(tmp, old_ਮੁੱਲ);
      ਜੇ (ਕਿਨਾਰਾ_flag > 0) ਵਾਪਸ posedge_;
      ਜੇ (ਕਿਨਾਰਾ_flag < 0) ਵਾਪਸ negedge_;
      ਵਾਪਸ ਝੂਠ;
}


/*
 * All the below routines that begin with
 * modpath_src_* belong the internal function
 * of an vpiModPathIn object. This is used to
 * make some specific delays path operations
 *
 */
ਸਥਿਰ ਪੂਰਨ modpath_src_get(ਪੂਰਨ, vpiHandle ref)
{
      ਬਣਤਰ __vpiModPathSrc*obj =dynamic_cast<__vpiModPathSrc*>(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(obj);
      ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ modpath_src_get_ਮੁੱਲ(vpiHandle ref, p_vpi_ਮੁੱਲ)
{
      ਬਣਤਰ __vpiModPathSrc* modpathsrc = dynamic_cast<__vpiModPathSrc*>(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(modpathsrc);
      ਵਾਪਸ;
}

ਸਥਿਰ vpiHandle modpath_src_put_ਮੁੱਲ(vpiHandle ref, s_vpi_ਮੁੱਲ *, ਪੂਰਨ )
{
      ਬਣਤਰ __vpiModPathSrc* modpathsrc = dynamic_cast<__vpiModPathSrc*>(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(modpathsrc);
      ਵਾਪਸ 0;
}

ਸਥਿਰ vpiHandle modpath_src_get_handle(ਪੂਰਨ code, vpiHandle ref)
{
      ਬਣਤਰ __vpiModPathSrc*rfp = dynamic_cast<__vpiModPathSrc*>(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(rfp);

      ਚੋਣ (code) {

	ਹਾਲਤ vpiScope:
	  ਵਾਪਸ rfp->dest->scope;

	  ਹਾਲਤ vpiModule:
	      { __vpiScope*scope = rfp->dest->scope;
		ਜਦੋਂ (scope && scope->get_type_ਕੋਡ() != vpiModule)
		      scope = scope->scope;
		ਪ੍ਰਤਿਜ੍ਞਾ(scope);
		ਵਾਪਸ scope;
	      }

	    // Handles to path term objects should really be obtained via
	    // the vpi_iterate and vpi_scan functions. Continue to allow
	    // them to be obtained here for backwards compatibility with
	    // older versions of Icarus Verilog.

	  ਹਾਲਤ vpi_Mod_ਰਸਤਾ_ਵਿੱਚ:
	    ਵਾਪਸ &rfp->ਰਸਤਾ_ਪਦ_ਵਿੱਚ;

	  ਹਾਲਤ vpi_Mod_ਰਸਤਾ_ਬਾਹਰਲਾ:
	    ਵਾਪਸ &rfp->dest->ਰਸਤਾ_ਪਦ_ਬਾਹਰਲਾ;
      }
      ਵਾਪਸ 0;
}

ਸਥਿਰ vpiHandle modpath_src_iterate(ਪੂਰਨ code, vpiHandle ref)
{
      ਬਣਤਰ __vpiModPathSrc*rfp = dynamic_cast<__vpiModPathSrc*>(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(rfp);

	// Module paths with multiple sources or destinations are
	// currently represented by a separate modpath object for
	// each source/destination combination, so there is only
	// ever one input path term and one output path term.
      ਚੋਣ (code) {
	  ਹਾਲਤ vpi_Mod_ਰਸਤਾ_ਵਿੱਚ: {
	    vpiHandle*ਦਲੀਲਾਂ = static_cast<vpiHandle*>(ਸਿਫਰ_ਸਮ੍ਰਿਤੀ_ਦਿਓ(1, ਆਕਾਰ(vpiHandle*)));
	    ਦਲੀਲਾਂ[0] = &rfp->ਰਸਤਾ_ਪਦ_ਵਿੱਚ;
	    ਵਾਪਸ vpip_make_iterator(1, ਦਲੀਲਾਂ, ਸੱਚ);
	  }
	  ਹਾਲਤ vpi_Mod_ਰਸਤਾ_ਬਾਹਰਲਾ: {
	    vpiHandle*ਦਲੀਲਾਂ = static_cast<vpiHandle*>(ਸਿਫਰ_ਸਮ੍ਰਿਤੀ_ਦਿਓ(1, ਆਕਾਰ(vpiHandle*)));
	    ਦਲੀਲਾਂ[0] = &rfp->dest->ਰਸਤਾ_ਪਦ_ਬਾਹਰਲਾ;
	    ਵਾਪਸ vpip_make_iterator(1, ਦਲੀਲਾਂ, ਸੱਚ);
	  }
      }
      ਵਾਪਸ 0;
}

ਸਥਿਰ vpiHandle modpath_src_ਸੂਚਕਾਂਕ ( vpiHandle ref, ਪੂਰਨ)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ref->get_type_ਕੋਡ() == vpi_Mod_ਰਸਤਾ_ਵਿੱਚ);
      ਵਾਪਸ 0;
}


/*
 * This routine will put specific dimension of delay[] values
 * into a vpiHandle. In this case, we will put
 * specific delays values in a vpiModPathIn object
 *
 */
ਸਥਿਰ ਖਾਲੀ modpath_src_put_delays (vpiHandle ref, p_vpi_delay delays)
{
      vvp_time64_t tmp[12];
      ਪੂਰਨ idx;
      ਬਣਤਰ __vpiModPathSrc * src = dynamic_cast<__vpiModPathSrc*>(ref) ;
      ਪ੍ਰਤਿਜ੍ਞਾ(src) ;

      vvp_fun_modpath_src *fun = dynamic_cast<vvp_fun_modpath_src*>(src->net->fun);
      ਪ੍ਰਤਿਜ੍ਞਾ( fun );

      ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ map_array_t[12];
	// Only the first six entries are used for the less than twelve maps.
      ਸਥਿਰ ਅਚਲ map_array_t map_1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      ਸਥਿਰ ਅਚਲ map_array_t map_2 = {0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0};
      ਸਥਿਰ ਅਚਲ map_array_t map_3 = {0, 1, 2, 0, 2, 1, 0, 0, 0, 0, 0, 0};
      ਸਥਿਰ ਅਚਲ map_array_t map_6 = {0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0};
      ਸਥਿਰ ਅਚਲ map_array_t map12 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

      ਅਚਲ map_array_t*use_map = 0;
      ਚੋਣ (delays->no_ਦਾ_delays) {
	  ਹਾਲਤ 1:
	    use_map = &map_1;
	    ਰੋਕੋ;
	  ਹਾਲਤ 2:
	    use_map = &map_2;
	    ਰੋਕੋ;
	  ਹਾਲਤ 3:
	    use_map = &map_3;
	    ਰੋਕੋ;
	  ਹਾਲਤ 6:
	    use_map = &map_6;
	    ਰੋਕੋ;
	  ਹਾਲਤ 12:
	    use_map = &map12;
	    ਰੋਕੋ;
	  ਮੂਲ:
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
	    ਰੋਕੋ;
      }

      ਜੇ (delays->ਸਮਾਂ_type == vpi_Sim_ਸਮਾਂ) {
	    ਲਈ (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_timestruct_ਮੰਜ਼ਿਲ_ਸਮਾਂ(delays->da+use_map[0][idx]);
	    }
      } ਨਹੀਂਤਾਂ {
	      // You cannot create a modpath with a negative delay so set it
	      // to zero per 1364-2005 section 14.3.1.
	    ਲਈ (idx = 0 ; idx < delays->no_ਦਾ_delays ; idx += 1) {
		  ਜੇ (delays->da[idx].real < 0.0) delays->da[idx].real = 0.0;
	    }
	    ਲਈ (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_scaled_real_ਮੰਜ਼ਿਲ_time64(delays->da[use_map[0][idx]].real,
							src->dest->scope);
	    }
      }

      /* Now define the to-from-x delays if needed. */
      ਜੇ (delays->no_ਦਾ_delays <= 6) {
	      /* 0->x is the minimum of 0->z and 0->1. */
	    tmp[DELAY_ਕਿਨਾਰਾ_0x] = tmp[DELAY_ਕਿਨਾਰਾ_0z] < tmp[DELAY_ਕਿਨਾਰਾ_01] ?
	                           tmp[DELAY_ਕਿਨਾਰਾ_0z] : tmp[DELAY_ਕਿਨਾਰਾ_01];
	      /* x->1 is the maximum of z->1 and 0->1. */
	    tmp[DELAY_ਕਿਨਾਰਾ_x1] = tmp[DELAY_ਕਿਨਾਰਾ_z1] > tmp[DELAY_ਕਿਨਾਰਾ_01] ?
	                           tmp[DELAY_ਕਿਨਾਰਾ_z1] : tmp[DELAY_ਕਿਨਾਰਾ_01];
	      /* 1->x is the minimum of 1->z and 1->0. */
	    tmp[DELAY_ਕਿਨਾਰਾ_1x] = tmp[DELAY_ਕਿਨਾਰਾ_1z] < tmp[DELAY_ਕਿਨਾਰਾ_10] ?
	                           tmp[DELAY_ਕਿਨਾਰਾ_1z] : tmp[DELAY_ਕਿਨਾਰਾ_10];
	      /* x->0 is the maximum of z->0 and 1->0. */
	    tmp[DELAY_ਕਿਨਾਰਾ_x0] = tmp[DELAY_ਕਿਨਾਰਾ_z0] > tmp[DELAY_ਕਿਨਾਰਾ_10] ?
	                           tmp[DELAY_ਕਿਨਾਰਾ_z0] : tmp[DELAY_ਕਿਨਾਰਾ_10];
	      /* x->z is the maximum of 1->z and 0->z. */
	    tmp[DELAY_ਕਿਨਾਰਾ_xz] = tmp[DELAY_ਕਿਨਾਰਾ_1z] > tmp[DELAY_ਕਿਨਾਰਾ_0z] ?
	                           tmp[DELAY_ਕਿਨਾਰਾ_1z] : tmp[DELAY_ਕਿਨਾਰਾ_0z];
	      /* z->x is the minimum of z->1 and z->0. */
	    tmp[DELAY_ਕਿਨਾਰਾ_zx] = tmp[DELAY_ਕਿਨਾਰਾ_z1] < tmp[DELAY_ਕਿਨਾਰਾ_z0] ?
	                           tmp[DELAY_ਕਿਨਾਰਾ_z1] : tmp[DELAY_ਕਿਨਾਰਾ_z0];
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

ਸਥਿਰ ਖਾਲੀ modpath_src_get_delays ( vpiHandle ref, p_vpi_delay delays )
{
      ਬਣਤਰ __vpiModPathSrc*src = dynamic_cast<__vpiModPathSrc*>(ref) ;
      ਪ੍ਰਤਿਜ੍ਞਾ(src);

      vvp_fun_modpath_src *fun = dynamic_cast<vvp_fun_modpath_src*>(src->net->fun);
      ਪ੍ਰਤਿਜ੍ਞਾ(fun);

      ਪੂਰਨ idx;
      vvp_time64_t tmp[12];
      fun->get_delay12(tmp);

      ਚੋਣ (delays->no_ਦਾ_delays) {
	  ਹਾਲਤ 1:
	  ਹਾਲਤ 2:
	  ਹਾਲਤ 3:
	  ਹਾਲਤ 6:
	  ਹਾਲਤ 12:
	    ਰੋਕੋ;

	  ਮੂਲ:
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
	    ਰੋਕੋ;
      }

      ਜੇ (delays->ਸਮਾਂ_type == vpi_Sim_ਸਮਾਂ) {
	    ਲਈ (idx = 0; idx < delays->no_ਦਾ_delays; idx += 1) {
		  vpip_ਸਮਾਂ_ਮੰਜ਼ਿਲ_timestruct(delays->da+idx, tmp[idx]);
	    }
      } ਨਹੀਂਤਾਂ {
	    ਲਈ (idx = 0; idx < delays->no_ਦਾ_delays; idx += 1) {
		  delays->da[idx].real = vpip_ਸਮਾਂ_ਮੰਜ਼ਿਲ_scaled_real(tmp[idx], src->dest->scope);
	    }
      }
}

ਸਥਿਰ ਪੂਰਨ pathterm_get(ਪੂਰਨ code, vpiHandle ref)
{
      ਬਣਤਰ __vpiModPathTerm*obj = dynamic_cast<__vpiModPathTerm*>(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(obj);

      ਚੋਣ (code) {
	  ਹਾਲਤ vpi_ਕਿਨਾਰਾ_2:
	    ਵਾਪਸ obj->ਕਿਨਾਰਾ;
	  ਮੂਲ:
	    ਵਾਪਸ 0;
      }
}

ਸਥਿਰ vpiHandle pathterm_get_handle(ਪੂਰਨ code, vpiHandle ref)
{
      ਬਣਤਰ __vpiModPathTerm*obj = dynamic_cast<__vpiModPathTerm*>(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(obj);

      ਚੋਣ (code) {
	  ਹਾਲਤ vpi_ਵਿਅੰਜਕ:
	    ਵਾਪਸ obj->ਵਿਅੰਜਕ;
	  ਮੂਲ:
	    ਵਾਪਸ 0;
      }
}

/*
* The __vpiModPathSrc class is what the VPI client sees as a
* vpiModPath object. The __vpiModPath structure contains items that
* are common to a bunch of modpaths, including the destination term.
*/
ਸਤਰਅੰਦਰ __vpiModPathSrc::__vpiModPathSrc()
{ }

ਪੂਰਨ __vpiModPathSrc::get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ
{ ਵਾਪਸ vpi_Mod_ਰਸਤਾ; }

ਪੂਰਨ __vpiModPathSrc::vpi_get(ਪੂਰਨ code)
{ ਵਾਪਸ modpath_src_get(code, ਇਹ); }

ਖਾਲੀ __vpiModPathSrc::vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val)
{ modpath_src_get_ਮੁੱਲ(ਇਹ, val); }

vpiHandle __vpiModPathSrc::vpi_put_ਮੁੱਲ(p_vpi_ਮੁੱਲ val, ਪੂਰਨ flags)
{ ਵਾਪਸ modpath_src_put_ਮੁੱਲ(ਇਹ, val, flags); }

vpiHandle __vpiModPathSrc::vpi_handle(ਪੂਰਨ code)
{ ਵਾਪਸ modpath_src_get_handle(code, ਇਹ); }

vpiHandle __vpiModPathSrc::vpi_iterate(ਪੂਰਨ code)
{ ਵਾਪਸ modpath_src_iterate(code, ਇਹ); }

vpiHandle __vpiModPathSrc:: vpi_ਸੂਚਕਾਂਕ(ਪੂਰਨ idx)
{ ਵਾਪਸ modpath_src_ਸੂਚਕਾਂਕ(ਇਹ, idx); }

ਖਾਲੀ __vpiModPathSrc::vpi_get_delays(p_vpi_delay del)
{ modpath_src_get_delays(ਇਹ, del); }

ਖਾਲੀ __vpiModPathSrc::vpi_put_delays(p_vpi_delay del)
{ modpath_src_put_delays(ਇਹ, del); }

ਸਥਿਰ ਪੂਰਨ modpath_src_free_object( vpiHandle ref )
{
      ਮਿਟਾਓ ref;
      ਵਾਪਸ 1 ;
}

__vpiHandle::free_object_fun_t __vpiModPathSrc::free_object_fun(ਖਾਲੀ)
{ ਵਾਪਸ &modpath_src_free_object; }


ਸਤਰਅੰਦਰ __vpiModPathTerm::__vpiModPathTerm()
{ }

ਪੂਰਨ __vpiModPathTerm::get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ
{ ਵਾਪਸ vpi_ਰਸਤਾ_ਪਦ; }

ਪੂਰਨ __vpiModPathTerm::vpi_get(ਪੂਰਨ code)
{ ਵਾਪਸ pathterm_get(code, ਇਹ); }

vpiHandle __vpiModPathTerm::vpi_handle(ਪੂਰਨ code)
{ ਵਾਪਸ pathterm_get_handle(code, ਇਹ); }

ਸਥਿਰ ਖਾਲੀ initialize_ਰਸਤਾ_ਪਦ(ਬਣਤਰ __vpiModPathTerm&obj)
{
      obj.ਵਿਅੰਜਕ = 0;
      obj.ਕਿਨਾਰਾ = vpi_No_ਕਿਨਾਰਾ;
}

/*
 * This function will construct a vpiModPath Object.
 * give a respective "net", and will point to his
 * respective functor
 */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਸਥਿਰ ਬਣਤਰ __vpiModPath**mp_list = 0;
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ mp_ਗਿਣਤੀ = 0;
#ਜੇ_ਅੰਤ

ਬਣਤਰ __vpiModPath* vpip_make_modpath(vvp_net_t *net)
{
      ਬਣਤਰ __vpiModPath*obj = ਨਵਾਂ __vpiModPath;
      obj->scope = vpip_peek_current_scope ( );

      initialize_ਰਸਤਾ_ਪਦ(obj->ਰਸਤਾ_ਪਦ_ਬਾਹਰਲਾ);
      obj->ਨਿਵੇਸ਼_net = net ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
      mp_ਗਿਣਤੀ += 1;
      mp_list = static_cast<ਬਣਤਰ __vpiModPath **>
                (realloc(mp_list,
                 mp_ਗਿਣਤੀ*ਆਕਾਰ(ਬਣਤਰ __vpiModPath **)));
      mp_list[mp_ਗਿਣਤੀ-1] = obj;
#ਜੇ_ਅੰਤ
      ਵਾਪਸ obj;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਖਾਲੀ modpath_delete()
{
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0; idx < mp_ਗਿਣਤੀ; idx += 1) {
	    ਮਿਟਾਓ mp_list[idx];
      }
      ਮੁਕਤ(mp_list);
      mp_list = 0;
      mp_ਗਿਣਤੀ = 0;
}
#ਜੇ_ਅੰਤ

/*
 * This function will construct a vpiModPathIn
 * ( struct __vpiModPathSrc ) Object. will give
 * a delays[12] values, and point to the specified functor
 *
 */

ਬਣਤਰ __vpiModPathSrc* vpip_make_modpath_src(ਬਣਤਰ __vpiModPath*ਰਸਤਾ,
                                              vvp_net_t *net)
{
      ਬਣਤਰ __vpiModPathSrc *obj = ਨਵਾਂ __vpiModPathSrc;

      obj->dest = ਰਸਤਾ;
      obj->type = 0;
      obj->net = net;
      initialize_ਰਸਤਾ_ਪਦ(obj->ਰਸਤਾ_ਪਦ_ਵਿੱਚ);

      ਵਾਪਸ obj;
}

vvp_fun_intermodpath::vvp_fun_intermodpath(vvp_net_t*net, ਬਿਨਾਨਿਸ਼ਾਨੀ width)
: net_(net)
{
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = 0;

      cur_vec4_ = vvp_vector4_t(width, BIT4_X);
      ਸਮਾਂ_ਸਾਰਣੀ_init_propagate(net_, cur_vec4_);
}

vvp_fun_intermodpath::~vvp_fun_intermodpath()
{
}

ਖਾਲੀ vvp_fun_intermodpath::get_delay12(vvp_time64_t val[12]) ਅਚਲ
{
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < 12 ;  idx += 1)
	    val[idx] = delay_[idx];
}

ਖਾਲੀ vvp_fun_intermodpath::put_delay12(ਅਚਲ vvp_time64_t val[12])
{
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = val[idx];
}

/*
 * FIXME: this implementation currently only uses the maximum delay
 * from all the bit changes in the vectors. If there are multiple
 * changes with different delays, then the results would be
 * wrong. What should happen is that if there are multiple changes,
 * multiple vectors approaching the result should be scheduled.
 */
ਖਾਲੀ vvp_fun_intermodpath::recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                              vvp_context_t)
{
	/* Only the first port is used. */
      ਜੇ (ਪੋਰਟ.ਪੋਰਟ() > 0)
	    ਵਾਪਸ;

      ਜੇ (cur_vec4_.eeq(ਦੋਆਧਾਰੀ_ਅੰਕ))
	    ਵਾਪਸ;

	/* Given the scheduled output time, create an output event. */
      vvp_time64_t use_delay = delay_from_ਕਿਨਾਰਾ(cur_vec4_.value(0),
					       ਦੋਆਧਾਰੀ_ਅੰਕ.value(0),
					       delay_);

	/* FIXME: This bases the edge delay on only the least
	   bit. This is WRONG! I need to find all the possible delays,
	   and schedule an event for each partial change. Hard! */
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 1 ;  idx < ਦੋਆਧਾਰੀ_ਅੰਕ.size() ;  idx += 1) {
	    vvp_time64_t tmp = delay_from_ਕਿਨਾਰਾ(cur_vec4_.value(idx),
					       ਦੋਆਧਾਰੀ_ਅੰਕ.value(idx),
					       delay_);
	      /* If the current and new bit values match then no delay
	       * is needed for this bit. */
	    ਜੇ (cur_vec4_.value(idx) == ਦੋਆਧਾਰੀ_ਅੰਕ.value(idx)) ਜਾਰੀ;
	    ਪ੍ਰਤਿਜ੍ਞਾ(tmp == use_delay);
      }

      cur_vec4_ = ਦੋਆਧਾਰੀ_ਅੰਕ;
      ਸਮਾਂ_ਸਾਰਣੀ_generic(ਇਹ, use_delay, ਝੂਠ);
}

ਖਾਲੀ vvp_fun_intermodpath::ਲੜੀਵਾਰ_ਲੜੀਵਾਰ()
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
ਸਥਿਰ ਪੂਰਨ intermodpath_get(ਪੂਰਨ, vpiHandle ref)
{
      ਬਣਤਰ __vpiInterModPath*obj =dynamic_cast<__vpiInterModPath*>(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(obj);
      ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ intermodpath_get_ਮੁੱਲ(vpiHandle ref, p_vpi_ਮੁੱਲ)
{
      ਬਣਤਰ __vpiInterModPath* intermodpath = dynamic_cast<__vpiInterModPath*>(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(intermodpath);
      ਵਾਪਸ;
}

ਸਥਿਰ vpiHandle intermodpath_put_ਮੁੱਲ(vpiHandle ref, s_vpi_ਮੁੱਲ *, ਪੂਰਨ )
{
      ਬਣਤਰ __vpiInterModPath* intermodpath = dynamic_cast<__vpiInterModPath*>(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(intermodpath);
      ਵਾਪਸ 0;
}

ਸਥਿਰ vpiHandle intermodpath_get_handle(ਪੂਰਨ code, vpiHandle ref)
{
      ਬਣਤਰ __vpiInterModPath*rfp = dynamic_cast<__vpiInterModPath*>(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(rfp);

      ਚੋਣ (code) {

	ਹਾਲਤ vpiScope:
	  ਵਾਪਸ rfp->scope;

	  ਹਾਲਤ vpiModule:
	      { __vpiScope*scope = rfp->scope;
		ਜਦੋਂ (scope && scope->get_type_ਕੋਡ() != vpiModule)
		      scope = scope->scope;
		ਪ੍ਰਤਿਜ੍ਞਾ(scope);
		ਵਾਪਸ scope;
	      }
      }
      ਵਾਪਸ 0;
}

ਸਥਿਰ vpiHandle intermodpath_iterate(ਪੂਰਨ code, vpiHandle ref)
{
      ਬਣਤਰ __vpiInterModPath*rfp = dynamic_cast<__vpiInterModPath*>(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(rfp);

	// For now intermodpaths only support exactly two ports
      ਚੋਣ (code) {
	  ਹਾਲਤ vpi_ਪੋਰਟ: {
	    vpiHandle*ਦਲੀਲਾਂ = static_cast<vpiHandle*>(ਸਿਫਰ_ਸਮ੍ਰਿਤੀ_ਦਿਓ(2, ਆਕਾਰ(vpiHandle*)));
	    ਦਲੀਲਾਂ[0] = rfp->port1;
	    ਦਲੀਲਾਂ[1] = rfp->port2;
	    ਵਾਪਸ vpip_make_iterator(2, ਦਲੀਲਾਂ, ਸੱਚ);
	  }
      }
      ਵਾਪਸ 0;
}

/*
 * This routine will put specific dimension of delay[] values
 * into a vpiHandle. In this case, we will put
 * specific delays values in a vpiInterModPath object
 *
 */
ਸਥਿਰ ਖਾਲੀ intermodpath_put_delays (vpiHandle ref, p_vpi_delay delays)
{
      vvp_time64_t tmp[12];
      ਪੂਰਨ idx;
      ਬਣਤਰ __vpiInterModPath * src = dynamic_cast<__vpiInterModPath*>(ref) ;
      ਪ੍ਰਤਿਜ੍ਞਾ(src) ;

      vvp_fun_intermodpath *fun = dynamic_cast<vvp_fun_intermodpath*>(src->net->fun);
      ਪ੍ਰਤਿਜ੍ਞਾ( fun );

      ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ map_array_t[12];
	// Only the first six entries are used for the less than twelve maps.
      ਸਥਿਰ ਅਚਲ map_array_t map_1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      ਸਥਿਰ ਅਚਲ map_array_t map_2 = {0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0};
      ਸਥਿਰ ਅਚਲ map_array_t map_3 = {0, 1, 2, 0, 2, 1, 0, 0, 0, 0, 0, 0};
      ਸਥਿਰ ਅਚਲ map_array_t map_6 = {0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0};
      ਸਥਿਰ ਅਚਲ map_array_t map12 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

      ਅਚਲ map_array_t*use_map = 0;
      ਚੋਣ (delays->no_ਦਾ_delays) {
	  ਹਾਲਤ 1:
	    use_map = &map_1;
	    ਰੋਕੋ;
	  ਹਾਲਤ 2:
	    use_map = &map_2;
	    ਰੋਕੋ;
	  ਹਾਲਤ 3:
	    use_map = &map_3;
	    ਰੋਕੋ;
	  ਹਾਲਤ 6:
	    use_map = &map_6;
	    ਰੋਕੋ;
	  ਹਾਲਤ 12:
	    use_map = &map12;
	    ਰੋਕੋ;
	  ਮੂਲ:
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
	    ਰੋਕੋ;
      }

      ਜੇ (delays->ਸਮਾਂ_type == vpi_Sim_ਸਮਾਂ) {
	    ਲਈ (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_timestruct_ਮੰਜ਼ਿਲ_ਸਮਾਂ(delays->da+use_map[0][idx]);
	    }
      } ਨਹੀਂਤਾਂ {
	      // You cannot create a modpath with a negative delay so set it
	      // to zero per 1364-2005 section 14.3.1.
	    ਲਈ (idx = 0 ; idx < delays->no_ਦਾ_delays ; idx += 1) {
		  ਜੇ (delays->da[idx].real < 0.0) delays->da[idx].real = 0.0;
	    }
	    ਲਈ (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_scaled_real_ਮੰਜ਼ਿਲ_time64(delays->da[use_map[0][idx]].real,
							src->scope);
	    }
      }

      /* Now define the to-from-x delays if needed. */
      ਜੇ (delays->no_ਦਾ_delays <= 6) {
	      /* 0->x is the minimum of 0->z and 0->1. */
	    tmp[DELAY_ਕਿਨਾਰਾ_0x] = tmp[DELAY_ਕਿਨਾਰਾ_0z] < tmp[DELAY_ਕਿਨਾਰਾ_01] ?
	                           tmp[DELAY_ਕਿਨਾਰਾ_0z] : tmp[DELAY_ਕਿਨਾਰਾ_01];
	      /* x->1 is the maximum of z->1 and 0->1. */
	    tmp[DELAY_ਕਿਨਾਰਾ_x1] = tmp[DELAY_ਕਿਨਾਰਾ_z1] > tmp[DELAY_ਕਿਨਾਰਾ_01] ?
	                           tmp[DELAY_ਕਿਨਾਰਾ_z1] : tmp[DELAY_ਕਿਨਾਰਾ_01];
	      /* 1->x is the minimum of 1->z and 1->0. */
	    tmp[DELAY_ਕਿਨਾਰਾ_1x] = tmp[DELAY_ਕਿਨਾਰਾ_1z] < tmp[DELAY_ਕਿਨਾਰਾ_10] ?
	                           tmp[DELAY_ਕਿਨਾਰਾ_1z] : tmp[DELAY_ਕਿਨਾਰਾ_10];
	      /* x->0 is the maximum of z->0 and 1->0. */
	    tmp[DELAY_ਕਿਨਾਰਾ_x0] = tmp[DELAY_ਕਿਨਾਰਾ_z0] > tmp[DELAY_ਕਿਨਾਰਾ_10] ?
	                           tmp[DELAY_ਕਿਨਾਰਾ_z0] : tmp[DELAY_ਕਿਨਾਰਾ_10];
	      /* x->z is the maximum of 1->z and 0->z. */
	    tmp[DELAY_ਕਿਨਾਰਾ_xz] = tmp[DELAY_ਕਿਨਾਰਾ_1z] > tmp[DELAY_ਕਿਨਾਰਾ_0z] ?
	                           tmp[DELAY_ਕਿਨਾਰਾ_1z] : tmp[DELAY_ਕਿਨਾਰਾ_0z];
	      /* z->x is the minimum of z->1 and z->0. */
	    tmp[DELAY_ਕਿਨਾਰਾ_zx] = tmp[DELAY_ਕਿਨਾਰਾ_z1] < tmp[DELAY_ਕਿਨਾਰਾ_z0] ?
	                           tmp[DELAY_ਕਿਨਾਰਾ_z1] : tmp[DELAY_ਕਿਨਾਰਾ_z0];
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

ਸਥਿਰ ਖਾਲੀ intermodpath_get_delays ( vpiHandle ref, p_vpi_delay delays )
{
      ਬਣਤਰ __vpiInterModPath*src = dynamic_cast<__vpiInterModPath*>(ref) ;
      ਪ੍ਰਤਿਜ੍ਞਾ(src);

      vvp_fun_intermodpath *fun = dynamic_cast<vvp_fun_intermodpath*>(src->net->fun);
      ਪ੍ਰਤਿਜ੍ਞਾ(fun);

      ਪੂਰਨ idx;
      vvp_time64_t tmp[12];
      fun->get_delay12(tmp);

      ਚੋਣ (delays->no_ਦਾ_delays) {
	  ਹਾਲਤ 1:
	  ਹਾਲਤ 2:
	  ਹਾਲਤ 3:
	  ਹਾਲਤ 6:
	  ਹਾਲਤ 12:
	    ਰੋਕੋ;

	  ਮੂਲ:
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
	    ਰੋਕੋ;
      }

      ਜੇ (delays->ਸਮਾਂ_type == vpi_Sim_ਸਮਾਂ) {
	    ਲਈ (idx = 0; idx < delays->no_ਦਾ_delays; idx += 1) {
		  vpip_ਸਮਾਂ_ਮੰਜ਼ਿਲ_timestruct(delays->da+idx, tmp[idx]);
	    }
      } ਨਹੀਂਤਾਂ {
	    ਲਈ (idx = 0; idx < delays->no_ਦਾ_delays; idx += 1) {
		  delays->da[idx].real = vpip_ਸਮਾਂ_ਮੰਜ਼ਿਲ_scaled_real(tmp[idx], src->scope);
	    }
      }
}

/*
* The __vpiInterModPath class is what the VPI client sees as a
* vpiInterModPath object.
*/
ਸਤਰਅੰਦਰ __vpiInterModPath::__vpiInterModPath()
{ }

ਪੂਰਨ __vpiInterModPath::get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ
{ ਵਾਪਸ vpi_Inter_Mod_ਰਸਤਾ; }

ਪੂਰਨ __vpiInterModPath::vpi_get(ਪੂਰਨ code)
{ ਵਾਪਸ intermodpath_get(code, ਇਹ); }

ਖਾਲੀ __vpiInterModPath::vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val)
{ intermodpath_get_ਮੁੱਲ(ਇਹ, val); }

vpiHandle __vpiInterModPath::vpi_put_ਮੁੱਲ(p_vpi_ਮੁੱਲ val, ਪੂਰਨ flags)
{ ਵਾਪਸ intermodpath_put_ਮੁੱਲ(ਇਹ, val, flags); }

vpiHandle __vpiInterModPath::vpi_handle(ਪੂਰਨ code)
{ ਵਾਪਸ intermodpath_get_handle(code, ਇਹ); }

vpiHandle __vpiInterModPath::vpi_iterate(ਪੂਰਨ code)
{ ਵਾਪਸ intermodpath_iterate(code, ਇਹ); }

ਖਾਲੀ __vpiInterModPath::vpi_get_delays(p_vpi_delay del)
{ intermodpath_get_delays(ਇਹ, del); }

ਖਾਲੀ __vpiInterModPath::vpi_put_delays(p_vpi_delay del)
{ intermodpath_put_delays(ਇਹ, del); }

ਸਥਿਰ ਪੂਰਨ intermodpath_free_object( vpiHandle ref )
{
      ਮਿਟਾਓ ref;
      ਵਾਪਸ 1 ;
}

__vpiHandle::free_object_fun_t __vpiInterModPath::free_object_fun(ਖਾਲੀ)
{ ਵਾਪਸ &intermodpath_free_object; }

/*
 * This function will construct a vpiInterModPath Object.
 * give a respective "net", and will point to his
 * respective functor
 */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਸਥਿਰ ਬਣਤਰ __vpiInterModPath**imp_list = 0;
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ imp_ਗਿਣਤੀ = 0;
#ਜੇ_ਅੰਤ

ਬਣਤਰ __vpiInterModPath* vpip_make_intermodpath(vvp_net_t *net, vpi_ਪੋਰਟ_Info* port1, vpi_ਪੋਰਟ_Info* port2)
{
      ਬਣਤਰ __vpiInterModPath*obj = ਨਵਾਂ __vpiInterModPath;
      obj->scope = vpip_peek_current_scope ( );

      obj->net = net;
      obj->port1 = port1;
      obj->port2 = port2;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
      imp_ਗਿਣਤੀ += 1;
      imp_list = static_cast<ਬਣਤਰ __vpiInterModPath **>
                 (realloc(imp_list,
                  imp_ਗਿਣਤੀ*ਆਕਾਰ(ਬਣਤਰ __vpiInterModPath **)));
      imp_list[imp_ਗਿਣਤੀ-1] = obj;
#ਜੇ_ਅੰਤ
      ਵਾਪਸ obj;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਖਾਲੀ intermodpath_delete()
{
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0; idx < mp_ਗਿਣਤੀ; idx += 1) {
	    ਮਿਟਾਓ imp_list[idx];
      }
      ਮੁਕਤ(imp_list);
      imp_list = 0;
      imp_ਗਿਣਤੀ = 0;
}
#ਜੇ_ਅੰਤ
