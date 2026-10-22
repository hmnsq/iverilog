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

#ଅନ୍ତର୍ଭାଵ "delay.h"
#ଅନ୍ତର୍ଭାଵ "schedule.h"
#ଅନ୍ତର୍ଭାଵ "vpi_priv.h"
#ଅନ୍ତର୍ଭାଵ "config.h"
#ifdef CHECK_WITH_VALGRIND
#ଅନ୍ତର୍ଭାଵ "vvp_cleanup.h"
#endif
#ଅନ୍ତର୍ଭାଵ <iostream>
#ଅନ୍ତର୍ଭାଵ <cstdlib>
#ଅନ୍ତର୍ଭାଵ <list>
#ଅନ୍ତର୍ଭାଵ <cassert>
#ଅନ୍ତର୍ଭାଵ <cmath>
#ଅନ୍ତର୍ଭାଵ "ivl_alloc.h"

using ନାମାକାଶ std;

ଶୂନ୍ୟ vvp_delay_t::calculate_min_delay_()
{
      min_delay_ = rise_;
      ଯଦି (fall_ < min_delay_)
	    min_delay_ = fall_;
      ଯଦି (ignore_decay_) decay_ = min_delay_;
      ଅନ୍ୟଥା ଯଦି (decay_ < min_delay_)
	    min_delay_ = decay_;
}

vvp_delay_t::vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall)
{
      rise_ = rise;
      fall_ = fall;
      decay_= fall < rise? fall : rise;
      min_delay_ = decay_;
      ignore_decay_ = ମିଥ୍ୟା;
}

vvp_delay_t::vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall, vvp_time64_t decay)
{
      rise_ = rise;
      fall_ = fall;
      decay_= decay;
      ignore_decay_ = ମିଥ୍ୟା;

      calculate_min_delay_();
}

ଶୂନ୍ୟ vvp_delay_t::set_ignore_decay()
{
      ignore_decay_ = ସତ୍ୟ;

      calculate_min_delay_();
}

vvp_delay_t::~vvp_delay_t()
{
}

vvp_time64_t vvp_delay_t::get_delay(vvp_bit4_t from, vvp_bit4_t to)
{
      ବାଛ (from) {
	  ସ୍ଥିତି BIT4_0:
	    ବାଛ (to) {
		ସ୍ଥିତି BIT4_0: ଫେରନ୍ତୁ 0;
		ସ୍ଥିତି BIT4_1: ଫେରନ୍ତୁ rise_;
		ସ୍ଥିତି BIT4_X: ଫେରନ୍ତୁ min_delay_;
		ସ୍ଥିତି BIT4_Z: ଫେରନ୍ତୁ decay_;
	    }
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି BIT4_1:
	    ବାଛ (to) {
		ସ୍ଥିତି BIT4_0: ଫେରନ୍ତୁ fall_;
		ସ୍ଥିତି BIT4_1: ଫେରନ୍ତୁ 0;
		ସ୍ଥିତି BIT4_X: ଫେରନ୍ତୁ min_delay_;
		ସ୍ଥିତି BIT4_Z: ଫେରନ୍ତୁ decay_;
	    }
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି BIT4_X:
	    ବାଛ (to) {
		ସ୍ଥିତି BIT4_0: ଫେରନ୍ତୁ fall_;
		ସ୍ଥିତି BIT4_1: ଫେରନ୍ତୁ rise_;
		ସ୍ଥିତି BIT4_X: ଫେରନ୍ତୁ 0;
		ସ୍ଥିତି BIT4_Z: ଫେରନ୍ତୁ decay_;
	    }
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି BIT4_Z:
	    ବାଛ (to) {
		ସ୍ଥିତି BIT4_0: ଫେରନ୍ତୁ fall_;
		ସ୍ଥିତି BIT4_1: ଫେରନ୍ତୁ rise_;
		ସ୍ଥିତି BIT4_X: ଫେରନ୍ତୁ min_delay_;
		ସ୍ଥିତି BIT4_Z: ଫେରନ୍ତୁ 0;
	    }
	    ଭାଙ୍ଗ;
      }

      ପ୍ରତିଜ୍ଞା(0);
      ଫେରନ୍ତୁ 0;
}

vvp_time64_t vvp_delay_t::get_min_delay() ଅଚଳ
{
      ଫେରନ୍ତୁ min_delay_;
}

ଶୂନ୍ୟ vvp_delay_t::set_rise(vvp_time64_t val)
{
      rise_ = val;
      ଯଦି (val < min_delay_) {
	    min_delay_ = val;
	    ଯଦି (ignore_decay_) decay_ = val;
      } ଅନ୍ୟଥା
	    calculate_min_delay_();
}

ଶୂନ୍ୟ vvp_delay_t::set_fall(vvp_time64_t val)
{
      fall_ = val;
      ଯଦି (val < min_delay_) {
	    min_delay_ = val;
	    ଯଦି (ignore_decay_) decay_ = val;
      } ଅନ୍ୟଥା
	    calculate_min_delay_();
}

ଶୂନ୍ୟ vvp_delay_t::set_decay(vvp_time64_t val)
{
      ପ୍ରତିଜ୍ଞା(!ignore_decay_);

      decay_ = val;
      ଯଦି (val < min_delay_)
	    min_delay_ = val;
      ଅନ୍ୟଥା
	    calculate_min_delay_();
}

vvp_fun_delay::vvp_fun_delay(vvp_net_t*n, unsigned width, ଅଚଳ vvp_delay_t&d)
: net_(n), delay_(d)
{
      cur_real_ = 0.0;
      ଯଦି (width > 0) {
            cur_vec4_ = vvp_vector4_t(width, BIT4_X);
            cur_vec8_ = vvp_vector8_t(cur_vec4_, 6, 6);
            schedule_init_propagate(net_, cur_vec4_);
      } ଅନ୍ୟଥା {
            schedule_init_propagate(net_, cur_real_);
      }
      list_ = 0;
      type_ = UNKNOWN_DELAY;
      initial_ = ସତ୍ୟ;
	// Calculate the values used when converting variable delays
	// to simulation time units.
      ଅଚଳ __vpiScope*scope = vpip_peek_current_scope();

      ପୂର୍ଣ୍ଣସଂଖ୍ୟା powr = scope->time_units - scope->time_precision;
      round_ = 1;
      ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା lp = 0; lp < powr; lp += 1) round_ *= 10;

      powr = scope->time_precision - vpip_get_time_precision();
      scale_ = 1;
      ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା lp = 0; lp < powr; lp += 1) scale_ *= 10;
}

vvp_fun_delay::~vvp_fun_delay()
{
      ଯେତେବେଳେ (ସଂରଚନା event_*cur = dequeue_())
	    delete cur;
}

ତାର୍କିକ vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        ଅଚଳ vvp_vector4_t&bit)
{
      ଯଦି (list_ == 0) ଫେରନ୍ତୁ ମିଥ୍ୟା;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      ଯଦି (list_->next->ptr_vec4.eeq(bit)) ଫେରନ୍ତୁ ସତ୍ୟ;

      clean_pulse_events_(use_delay);
      ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ତାର୍କିକ vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        ଅଚଳ vvp_vector8_t&bit)
{
      ଯଦି (list_ == 0) ଫେରନ୍ତୁ ମିଥ୍ୟା;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      ଯଦି (list_->next->ptr_vec8.eeq(bit)) ଫେରନ୍ତୁ ସତ୍ୟ;

      clean_pulse_events_(use_delay);
      ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ତାର୍କିକ vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        ଦ୍ୱିଗୁଣ bit)
{
      ଯଦି (list_ == 0) ଫେରନ୍ତୁ ମିଥ୍ୟା;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      ଯଦି (list_->next->ptr_real == bit) ଫେରନ୍ତୁ ସତ୍ୟ;

      clean_pulse_events_(use_delay);
      ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ଶୂନ୍ୟ vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay)
{
      ପ୍ରତିଜ୍ଞା(list_ != 0);

      କର {
	    ସଂରଚନା event_*cur = list_->next;
	      /* If this event is far enough from the event I'm about
	         to create, then that scheduled event is not a pulse
	         to be eliminated, so we're done. */
	    ଯଦି (cur->sim_time+use_delay <= use_delay+schedule_simtime())
		  ଭାଙ୍ଗ;

	    ଯଦି (list_ == cur)
		  list_ = 0;
	    ଅନ୍ୟଥା
		  list_->next = cur->next;
	    delete cur;
      } ଯେତେବେଳେ (list_);
}

/*
 * FIXME: this implementation currently only uses the maximum delay
 * from all the bit changes in the vectors. If there are multiple
 * changes with different delays, then the results would be
 * wrong. What should happen is that if there are multiple changes,
 * multiple vectors approaching the result should be scheduled.
 */
ଶୂନ୍ୟ vvp_fun_delay::recv_vec4(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
                              vvp_context_t)
{
      ଯଦି (port.port() > 0) {
	      // Get the integer value of the bit vector, or 0 if
	      // there are X or Z bits.
	    vvp_time64_t bval = 0;
	      // The following does not work correctly for negative values.
	      // They should be sign extended to 64 bits (1364-2001 9.7.1).
	    vector4_to_ମୂଲ୍ୟ(bit, bval);
	      // Integer values do not need to be rounded so just scale them.
	    vvp_time64_t val = bval * round_ * scale_;

	    ବାଛ (port.port()) {
		ସ୍ଥିତି 1:
		  delay_.set_rise(val);
		  ଫେରନ୍ତୁ;
		ସ୍ଥିତି 2:
		  delay_.set_fall(val);
		  ଫେରନ୍ତୁ;
		ସ୍ଥିତି 3:
		  delay_.set_decay(val);
		  ଫେରନ୍ତୁ;
	    }
	    ଫେରନ୍ତୁ;
      }

      vvp_time64_t use_delay;
	/* This is an initial value so it needs to be compared to all the
	   bits (the order the bits are changed is not deterministic). */
      ଯଦି (initial_) {
	    type_ = VEC4_DELAY;
            cur_vec8_ = vvp_vector8_t(vvp_vector4_t(0, BIT4_X), 6, 6);
	    vvp_bit4_t cur_val = cur_vec4_.value(0);
	    use_delay = delay_.get_delay(cur_val, bit.value(0));
	    ପାଇଁ (unsigned idx = 1 ;  idx < bit.size() ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(cur_val, bit.value(idx));
		  ଯଦି (tmp > use_delay) use_delay = tmp;
	    }
      } ଅନ୍ୟଥା {
	    ପ୍ରତିଜ୍ଞା(type_ == VEC4_DELAY);

	      // Use as a reference for calculating the delay the
	      // current value of the output. Detect and handle the
	      // special case that the event list contains the current
	      // value as a zero-delay-remaining event.
	    ଅଚଳ vvp_vector4_t&use_vec4 = (list_ && list_->next->sim_time == schedule_simtime())? list_->next->ptr_vec4 : cur_vec4_;

	      /* How many bits to compare? */
	    unsigned use_wid = use_vec4.size();
	    ଯଦି (bit.size() < use_wid) use_wid = bit.size();

	      /* Scan the vectors looking for delays. Select the maximum
	         delay encountered. */
	    use_delay = delay_.get_delay(use_vec4.value(0), bit.value(0));

	    ପାଇଁ (unsigned idx = 1 ;  idx < use_wid ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(use_vec4.value(idx), bit.value(idx));
		  ଯଦି (tmp > use_delay) use_delay = tmp;
	    }
      }

      /* what *should* happen here is we check to see if there is a
         transaction in the queue. This would be a pulse that needs to be
         eliminated. */
      ଯଦି (clean_pulse_events_(use_delay, bit)) ଫେରନ୍ତୁ;

      vvp_time64_t use_simtime = schedule_simtime() + use_delay;

	/* And propagate it. */
      ଯଦି (use_delay == 0 && list_ == 0) {
	    cur_vec4_ = bit;
	    initial_ = ମିଥ୍ୟା;
	    net_->send_vec4(cur_vec4_, 0);
      } ଅନ୍ୟଥା {
	    ସଂରଚନା event_*cur = ନୂତନ ସଂରଚନା event_(use_simtime);
	    cur->run_run_ptr = &vvp_fun_delay::run_run_vec4_;
	    cur->ptr_vec4 = bit;
	    enqueue_(cur);
	    schedule_generic(ଏହି, use_delay, ମିଥ୍ୟା);
      }
}

ଶୂନ୍ୟ vvp_fun_delay::recv_vec4_pv(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
			         unsigned base, unsigned vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

/* See the recv_vec4 comment above. */
ଶୂନ୍ୟ vvp_fun_delay::recv_vec8(vvp_net_ptr_t port, ଅଚଳ vvp_vector8_t&bit)
{
      ପ୍ରତିଜ୍ଞା(port.port() == 0);

      vvp_time64_t use_delay;
	/* This is an initial value so it needs to be compared to all the
	   bits (the order the bits are changed is not deterministic). */
      ଯଦି (initial_) {
	    type_ = VEC8_DELAY;
            cur_vec4_ = vvp_vector4_t(0, BIT4_X);
	    vvp_bit4_t cur_val = cur_vec8_.value(0).value();
	    use_delay = delay_.get_delay(cur_val, bit.value(0).value());
	    ପାଇଁ (unsigned idx = 1 ;  idx < bit.size() ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(cur_val, bit.value(idx).value());
		  ଯଦି (tmp > use_delay) use_delay = tmp;
	    }
      } ଅନ୍ୟଥା {
	    ପ୍ରତିଜ୍ଞା(type_ == VEC8_DELAY);

	      // Use as a reference for calculating the delay the
	      // current value of the output. Detect and handle the
	      // special case that the event list contains the current
	      // value as a zero-delay-remaining event.
	    ଅଚଳ vvp_vector8_t&use_vec8 = (list_ && list_->next->sim_time == schedule_simtime())? list_->next->ptr_vec8 : cur_vec8_;

	      /* How many bits to compare? */
	    unsigned use_wid = use_vec8.size();
	    ଯଦି (bit.size() < use_wid) use_wid = bit.size();

	      /* Scan the vectors looking for delays. Select the maximum
	         delay encountered. */
	    use_delay = delay_.get_delay(use_vec8.value(0).value(),
	                                 bit.value(0).value());

	    ପାଇଁ (unsigned idx = 1 ;  idx < use_wid ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(use_vec8.value(idx).value(),
		                         bit.value(idx).value());
		  ଯଦି (tmp > use_delay) use_delay = tmp;
	    }
      }

      /* what *should* happen here is we check to see if there is a
         transaction in the queue. This would be a pulse that needs to be
         eliminated. */
      ଯଦି (clean_pulse_events_(use_delay, bit)) ଫେରନ୍ତୁ;

      vvp_time64_t use_simtime = schedule_simtime() + use_delay;

	/* And propagate it. */
      ଯଦି (use_delay == 0 && list_ == 0) {
	    cur_vec8_ = bit;
	    initial_ = ମିଥ୍ୟା;
	    net_->send_vec8(cur_vec8_);
      } ଅନ୍ୟଥା {
	    ସଂରଚନା event_*cur = ନୂତନ ସଂରଚନା event_(use_simtime);
	    cur->ptr_vec8 = bit;
	    cur->run_run_ptr = &vvp_fun_delay::run_run_vec8_;
	    enqueue_(cur);
	    schedule_generic(ଏହି, use_delay, ମିଥ୍ୟା);
      }
}

ଶୂନ୍ୟ vvp_fun_delay::recv_vec8_pv(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector8_t&bit,
			         unsigned base, unsigned vwid)
{
      recv_vec8_pv_(ptr, bit, base, vwid);
}

ଶୂନ୍ୟ vvp_fun_delay::recv_real(vvp_net_ptr_t port, ଦ୍ୱିଗୁଣ bit,
                              vvp_context_t)
{
      ଯଦି (port.port() > 0) {
	    /* If the port is not 0, then this is a delay value that
	    should be rounded and converted to an integer delay. */
	    vvp_time64_t val = 0;
	    ଯଦି (bit > -0.5) {
		  val = (vvp_time64_t) (bit * round_ + 0.5) * scale_;
	    } ଅନ୍ୟଥା ଯଦି (bit != bit) {
		    // For a NaN we use the default (0).
	    } ଅନ୍ୟଥା {
		  vvp_vector4_t vec4(8*ଆକାର(vvp_time64_t),
		                     floor(-bit * round_ + 0.5) * -1 * scale_);
		  vector4_to_ମୂଲ୍ୟ(vec4, val);
	    }

	    ବାଛ (port.port()) {
		ସ୍ଥିତି 1:
		  delay_.set_rise(val);
		  ଫେରନ୍ତୁ;
		ସ୍ଥିତି 2:
		  delay_.set_fall(val);
		  ଫେରନ୍ତୁ;
		ସ୍ଥିତି 3:
		  delay_.set_decay(val);
		  ଫେରନ୍ତୁ;
	    }
	    ଫେରନ୍ତୁ;
      }

      ଯଦି (initial_) {
	    type_ = REAL_DELAY;
            cur_vec4_ = vvp_vector4_t(0, BIT4_X);
            cur_vec8_ = vvp_vector8_t(cur_vec4_, 6, 6);
      } ଅନ୍ୟଥା ପ୍ରତିଜ୍ଞା(type_ == REAL_DELAY);

      vvp_time64_t use_delay;
      use_delay = delay_.get_min_delay();

      /* Eliminate glitches. */
      ଯଦି (clean_pulse_events_(use_delay, bit)) ଫେରନ୍ତୁ;

      /* This must be done after cleaning pulses to avoid propagating
       * an incorrect value. */
      ଯଦି (cur_real_ == bit) ଫେରନ୍ତୁ;

      vvp_time64_t use_simtime = schedule_simtime() + use_delay;

      ଯଦି (use_delay == 0 && list_ == 0) {
	    cur_real_ = bit;
	    initial_ = ମିଥ୍ୟା;
	    net_->send_real(cur_real_, 0);
      } ଅନ୍ୟଥା {
	    ସଂରଚନା event_*cur = ନୂତନ ସଂରଚନା event_(use_simtime);
	    cur->run_run_ptr = &vvp_fun_delay::run_run_real_;
	    cur->ptr_real = bit;
	    enqueue_(cur);

	    schedule_generic(ଏହି, use_delay, ମିଥ୍ୟା);
      }
}

ଶୂନ୍ୟ vvp_fun_delay::run_run()
{
      vvp_time64_t sim_time = schedule_simtime();
      ଯଦି (list_ == 0 || list_->next->sim_time > sim_time)
	    ଫେରନ୍ତୁ;

      ସଂରଚନା event_*cur = dequeue_();
      ଯଦି (cur == 0)
	    ଫେରନ୍ତୁ;

      (ଏହି->*(cur->run_run_ptr))(cur);
      initial_ = ମିଥ୍ୟା;
      delete cur;
}

ଶୂନ୍ୟ vvp_fun_delay::run_run_vec4_(ଅଚଳ ସଂରଚନା event_*cur)
{
      cur_vec4_ = cur->ptr_vec4;
      net_->send_vec4(cur_vec4_, 0);
}

ଶୂନ୍ୟ vvp_fun_delay::run_run_vec8_(ଅଚଳ ସଂରଚନା vvp_fun_delay::event_*cur)
{
      cur_vec8_ = cur->ptr_vec8;
      net_->send_vec8(cur_vec8_);
}

ଶୂନ୍ୟ vvp_fun_delay::run_run_real_(ଅଚଳ ସଂରଚନା vvp_fun_delay::event_*cur)
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
      ଯେତେବେଳେ (src_list_) {
	    vvp_fun_modpath_src*tmp = src_list_;
	    src_list_ = tmp->next_;
	    delete tmp;
      }
      ଯେତେବେଳେ (ifnone_list_) {
	    vvp_fun_modpath_src*tmp = ifnone_list_;
	    ifnone_list_ = tmp->next_;
	    delete tmp;
      }
}

ଶୂନ୍ୟ vvp_fun_modpath::add_modpath_src(vvp_fun_modpath_src*that, ତାର୍କିକ ifnone)
{
      ପ୍ରତିଜ୍ଞା(that->next_ == 0);
      ଯଦି (ifnone) {
	    that->next_ = ifnone_list_;
	    ifnone_list_ = that;
      } ଅନ୍ୟଥା {
	    that->next_ = src_list_;
	    src_list_ = that;
      }
}

ସ୍ଥିର vvp_time64_t delay_from_edge(vvp_bit4_t a, vvp_bit4_t b,
                                           vvp_time64_t ଅଚଳ array[12])
{
      ପ୍ରକାରବ୍ୟାଖ୍ୟା delay_edge_t bit4_table4[4];
      ସ୍ଥିର ଅଚଳ bit4_table4 edge_table[4] = {
	    { DELAY_EDGE_01, DELAY_EDGE_01, DELAY_EDGE_0z, DELAY_EDGE_0x },
	    { DELAY_EDGE_10, DELAY_EDGE_10, DELAY_EDGE_1z, DELAY_EDGE_1x },
	    { DELAY_EDGE_z0, DELAY_EDGE_z1, DELAY_EDGE_z0, DELAY_EDGE_zx },
	    { DELAY_EDGE_x0, DELAY_EDGE_x1, DELAY_EDGE_xz, DELAY_EDGE_x0 }
      };

      ଫେରନ୍ତୁ array[ edge_table[a][b] ];
}

ଶୂନ୍ୟ vvp_fun_modpath::recv_vec4(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
                                vvp_context_t)
{
	/* Only the first port is used. */
      ଯଦି (port.port() > 0)
	    ଫେରନ୍ତୁ;

      ଯଦି (cur_vec4_.eeq(bit))
	    ଫେରନ୍ତୁ;

	/* Select a time delay source that applies. Notice that there
	   may be multiple delay sources that apply, so collect all
	   the candidates into a list first. */
      list<vvp_fun_modpath_src*>candidate_list;
      vvp_time64_t candidate_wake_time = 0;
      ପାଇଁ (vvp_fun_modpath_src*cur = src_list_ ;  cur ;  cur=cur->next_) {
	      /* Skip paths that are disabled by conditions. */
	    ଯଦି (cur->condition_flag_ == ମିଥ୍ୟା)
		  ଜାରି;

	    ଯଦି (candidate_list.empty()) {
		  candidate_list.push_back(cur);
		  candidate_wake_time = cur->wake_time_;
	    } ଅନ୍ୟଥା ଯଦି (cur->wake_time_ == candidate_wake_time) {
		  candidate_list.push_back(cur);
	    } ଅନ୍ୟଥା ଯଦି (cur->wake_time_ > candidate_wake_time) {
		  candidate_list.assign(1, cur);
		  candidate_wake_time = cur->wake_time_;
	    } ଅନ୍ୟଥା {
		  ଜାରି; /* Skip this entry. */
	    }
      }

	/* Only add the ifnone delay if it has a later wake_time_ or
	 * if there are no normal delays. */
      vvp_time64_t ifnone_wake_time = candidate_wake_time;
      ପାଇଁ (vvp_fun_modpath_src*cur = ifnone_list_ ;  cur ;  cur=cur->next_) {
	    ଯଦି (candidate_list.empty()) {
		  candidate_list.push_back(cur);
		  ifnone_wake_time = cur->wake_time_;
	    } ଅନ୍ୟଥା ଯଦି (cur->wake_time_ == ifnone_wake_time &&
	               ifnone_wake_time > candidate_wake_time) {
		  candidate_list.push_back(cur);
	    } ଅନ୍ୟଥା ଯଦି (cur->wake_time_ > ifnone_wake_time) {
		  candidate_list.assign(1, cur);
		  ifnone_wake_time = cur->wake_time_;
	    } ଅନ୍ୟଥା {
		  ଜାରି; /* Skip this entry. */
	    }
      }

	/* Handle the special case that there are no delays that
	   match. This may happen, for example, if the set of
	   conditional delays is incomplete, leaving some cases
	   uncovered. In that case, just pass the data without delay */
      ଯଦି (candidate_list.empty()) {
	    cur_vec4_ = bit;
	    schedule_generic(ଏହି, 0, ମିଥ୍ୟା);
	    ଫେରନ୍ତୁ;
      }

	/* Now given that we have a list of candidate delays, find for
	   each if the 12 numbers the minimum from all the
	   candidates. This minimum set becomes the chosen delay to
	   use. */
      vvp_time64_t out_at[12];
      vvp_time64_t now = schedule_simtime();

      ପ୍ରକାରବ୍ୟାଖ୍ୟା list<vvp_fun_modpath_src*>::const_iterator iter_t;

      iter_t cur = candidate_list.begin();
      ଅଚଳ vvp_fun_modpath_src*src = *cur;

      ପାଇଁ (unsigned idx = 0 ;  idx < 12 ;  idx += 1) {
	    out_at[idx] = src->wake_time_ + src->delay_[idx];
	    ଯଦି (out_at[idx] <= now)
		  out_at[idx] = 0;
	    ଅନ୍ୟଥା
		  out_at[idx] -= now;
      }

      ପାଇଁ (++ cur ; cur != candidate_list.end() ; ++ cur ) {
	    src = *cur;
	    ପାଇଁ (unsigned idx = 0 ;  idx < 12 ;  idx += 1) {
		  vvp_time64_t tmp = src->wake_time_ + src->delay_[idx];
		  ଯଦି (tmp <= now)
			tmp = 0;
		  ଅନ୍ୟଥା
			tmp -= now;
		  ଯଦି (tmp < out_at[idx])
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
      ପାଇଁ (unsigned idx = 1 ;  idx < bit.size() ;  idx += 1) {
	    vvp_time64_t tmp = delay_from_edge(cur_vec4_.value(idx),
					       bit.value(idx),
					       out_at);
	      /* If the current and new bit values match then no delay
	       * is needed for this bit. */
	    ଯଦି (cur_vec4_.value(idx) == bit.value(idx)) ଜାରି;
	    ଯଦି (tmp != use_delay) {
		  fprintf(stderr, "sorry: multi-bit module path delays are "
		                  "currently not fully supported.\n");
		  fprintf(stderr, "     : using the LSB delay for all bits.\n");
		  ଜାରି;
            }
      }

      cur_vec4_ = bit;
      schedule_generic(ଏହି, use_delay, ମିଥ୍ୟା);
}

ଶୂନ୍ୟ vvp_fun_modpath::run_run()
{
      net_->send_vec4(cur_vec4_, 0);
}

vvp_fun_modpath_src::vvp_fun_modpath_src(vvp_time64_t ଅଚଳ del[12])
{
      ପାଇଁ (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = del[idx];

      next_ = 0;
      wake_time_ = 0;
      condition_flag_ = ସତ୍ୟ;
}

vvp_fun_modpath_src::~vvp_fun_modpath_src()
{
}

ଶୂନ୍ୟ vvp_fun_modpath_src::get_delay12(vvp_time64_t val[12]) ଅଚଳ
{
      ପାଇଁ (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    val[idx] = delay_[idx];
}

ଶୂନ୍ୟ vvp_fun_modpath_src::put_delay12(ଅଚଳ vvp_time64_t val[12])
{
      ପାଇଁ (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = val[idx];
}

ଶୂନ୍ୟ vvp_fun_modpath_src::recv_vec4(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
                                    vvp_context_t)
{
      ଯଦି (port.port() == 0) {
	      // The modpath input...
	    ଯଦି (test_vec4(bit))
		  wake_time_ = schedule_simtime();

      } ଅନ୍ୟଥା ଯଦି (port.port() == 1) {
	      // The modpath condition input...
	    ଯଦି (bit.value(0) != BIT4_0)
		  condition_flag_ = ସତ୍ୟ;
	    ଅନ୍ୟଥା
		  condition_flag_ = ମିଥ୍ୟା;
      }
}

ତାର୍କିକ vvp_fun_modpath_src::test_vec4(ଅଚଳ vvp_vector4_t&)
{
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

vvp_fun_modpath_edge::vvp_fun_modpath_edge(vvp_time64_t del[12],
					   ତାର୍କିକ pos, ତାର୍କିକ neg)
: vvp_fun_modpath_src(del)
{
      old_ମୂଲ୍ୟ = BIT4_X;
      posedge_ = pos;
      negedge_ = neg;
}

ତାର୍କିକ vvp_fun_modpath_edge::test_vec4(ଅଚଳ vvp_vector4_t&bit)
{
      vvp_bit4_t tmp = old_ମୂଲ୍ୟ;
      old_ମୂଲ୍ୟ = bit.value(0);

      ପୂର୍ଣ୍ଣସଂଖ୍ୟା edge_flag = edge(tmp, old_ମୂଲ୍ୟ);
      ଯଦି (edge_flag > 0) ଫେରନ୍ତୁ posedge_;
      ଯଦି (edge_flag < 0) ଫେରନ୍ତୁ negedge_;
      ଫେରନ୍ତୁ ମିଥ୍ୟା;
}


/*
 * All the below routines that begin with
 * modpath_src_* belong the internal function
 * of an vpiModPathIn object. This is used to
 * make some specific delays path operations
 *
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା modpath_src_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, vpiHandle ref)
{
      ସଂରଚନା __vpiModPathSrc*obj =dynamic_cast<__vpiModPathSrc*>(ref);
      ପ୍ରତିଜ୍ଞା(obj);
      ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଶୂନ୍ୟ modpath_src_get_ମୂଲ୍ୟ(vpiHandle ref, p_vpi_ମୂଲ୍ୟ)
{
      ସଂରଚନା __vpiModPathSrc* modpathsrc = dynamic_cast<__vpiModPathSrc*>(ref);
      ପ୍ରତିଜ୍ଞା(modpathsrc);
      ଫେରନ୍ତୁ;
}

ସ୍ଥିର vpiHandle modpath_src_put_ମୂଲ୍ୟ(vpiHandle ref, s_vpi_ମୂଲ୍ୟ *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା )
{
      ସଂରଚନା __vpiModPathSrc* modpathsrc = dynamic_cast<__vpiModPathSrc*>(ref);
      ପ୍ରତିଜ୍ଞା(modpathsrc);
      ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର vpiHandle modpath_src_get_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code, vpiHandle ref)
{
      ସଂରଚନା __vpiModPathSrc*rfp = dynamic_cast<__vpiModPathSrc*>(ref);
      ପ୍ରତିଜ୍ଞା(rfp);

      ବାଛ (code) {

	ସ୍ଥିତି vpiScope:
	  ଫେରନ୍ତୁ rfp->dest->scope;

	  ସ୍ଥିତି vpiModule:
	      { __vpiScope*scope = rfp->dest->scope;
		ଯେତେବେଳେ (scope && scope->get_type_code() != vpiModule)
		      scope = scope->scope;
		ପ୍ରତିଜ୍ଞା(scope);
		ଫେରନ୍ତୁ scope;
	      }

	    // Handles to path term objects should really be obtained via
	    // the vpi_iterate and vpi_scan functions. Continue to allow
	    // them to be obtained here for backwards compatibility with
	    // older versions of Icarus Verilog.

	  ସ୍ଥିତି vpiModPathIn:
	    ଫେରନ୍ତୁ &rfp->path_term_in;

	  ସ୍ଥିତି vpiModPathOut:
	    ଫେରନ୍ତୁ &rfp->dest->path_term_out;
      }
      ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର vpiHandle modpath_src_iterate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code, vpiHandle ref)
{
      ସଂରଚନା __vpiModPathSrc*rfp = dynamic_cast<__vpiModPathSrc*>(ref);
      ପ୍ରତିଜ୍ଞା(rfp);

	// Module paths with multiple sources or destinations are
	// currently represented by a separate modpath object for
	// each source/destination combination, so there is only
	// ever one input path term and one output path term.
      ବାଛ (code) {
	  ସ୍ଥିତି vpiModPathIn: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(1, ଆକାର(vpiHandle*)));
	    args[0] = &rfp->path_term_in;
	    ଫେରନ୍ତୁ vpip_make_iterator(1, args, ସତ୍ୟ);
	  }
	  ସ୍ଥିତି vpiModPathOut: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(1, ଆକାର(vpiHandle*)));
	    args[0] = &rfp->dest->path_term_out;
	    ଫେରନ୍ତୁ vpip_make_iterator(1, args, ସତ୍ୟ);
	  }
      }
      ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର vpiHandle modpath_src_index ( vpiHandle ref, ପୂର୍ଣ୍ଣସଂଖ୍ୟା)
{
      ପ୍ରତିଜ୍ଞା(ref->get_type_code() == vpiModPathIn);
      ଫେରନ୍ତୁ 0;
}


/*
 * This routine will put specific dimension of delay[] values
 * into a vpiHandle. In this case, we will put
 * specific delays values in a vpiModPathIn object
 *
 */
ସ୍ଥିର ଶୂନ୍ୟ modpath_src_put_delays (vpiHandle ref, p_vpi_delay delays)
{
      vvp_time64_t tmp[12];
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx;
      ସଂରଚନା __vpiModPathSrc * src = dynamic_cast<__vpiModPathSrc*>(ref) ;
      ପ୍ରତିଜ୍ଞା(src) ;

      vvp_fun_modpath_src *fun = dynamic_cast<vvp_fun_modpath_src*>(src->net->fun);
      ପ୍ରତିଜ୍ଞା( fun );

      ପ୍ରକାରବ୍ୟାଖ୍ୟା unsigned ଅକ୍ଷର map_array_t[12];
	// Only the first six entries are used for the less than twelve maps.
      ସ୍ଥିର ଅଚଳ map_array_t map_1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      ସ୍ଥିର ଅଚଳ map_array_t map_2 = {0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0};
      ସ୍ଥିର ଅଚଳ map_array_t map_3 = {0, 1, 2, 0, 2, 1, 0, 0, 0, 0, 0, 0};
      ସ୍ଥିର ଅଚଳ map_array_t map_6 = {0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0};
      ସ୍ଥିର ଅଚଳ map_array_t map12 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

      ଅଚଳ map_array_t*use_map = 0;
      ବାଛ (delays->no_of_delays) {
	  ସ୍ଥିତି 1:
	    use_map = &map_1;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 2:
	    use_map = &map_2;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 3:
	    use_map = &map_3;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 6:
	    use_map = &map_6;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 12:
	    use_map = &map12;
	    ଭାଙ୍ଗ;
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ପ୍ରତିଜ୍ଞା(0);
	    ଭାଙ୍ଗ;
      }

      ଯଦି (delays->time_type == vpiSimTime) {
	    ପାଇଁ (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_timestruct_to_time(delays->da+use_map[0][idx]);
	    }
      } ଅନ୍ୟଥା {
	      // You cannot create a modpath with a negative delay so set it
	      // to zero per 1364-2005 section 14.3.1.
	    ପାଇଁ (idx = 0 ; idx < delays->no_of_delays ; idx += 1) {
		  ଯଦି (delays->da[idx].real < 0.0) delays->da[idx].real = 0.0;
	    }
	    ପାଇଁ (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_scaled_real_to_time64(delays->da[use_map[0][idx]].real,
							src->dest->scope);
	    }
      }

      /* Now define the to-from-x delays if needed. */
      ଯଦି (delays->no_of_delays <= 6) {
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

ସ୍ଥିର ଶୂନ୍ୟ modpath_src_get_delays ( vpiHandle ref, p_vpi_delay delays )
{
      ସଂରଚନା __vpiModPathSrc*src = dynamic_cast<__vpiModPathSrc*>(ref) ;
      ପ୍ରତିଜ୍ଞା(src);

      vvp_fun_modpath_src *fun = dynamic_cast<vvp_fun_modpath_src*>(src->net->fun);
      ପ୍ରତିଜ୍ଞା(fun);

      ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx;
      vvp_time64_t tmp[12];
      fun->get_delay12(tmp);

      ବାଛ (delays->no_of_delays) {
	  ସ୍ଥିତି 1:
	  ସ୍ଥିତି 2:
	  ସ୍ଥିତି 3:
	  ସ୍ଥିତି 6:
	  ସ୍ଥିତି 12:
	    ଭାଙ୍ଗ;

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ପ୍ରତିଜ୍ଞା(0);
	    ଭାଙ୍ଗ;
      }

      ଯଦି (delays->time_type == vpiSimTime) {
	    ପାଇଁ (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  vpip_time_to_timestruct(delays->da+idx, tmp[idx]);
	    }
      } ଅନ୍ୟଥା {
	    ପାଇଁ (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  delays->da[idx].real = vpip_time_to_scaled_real(tmp[idx], src->dest->scope);
	    }
      }
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pathterm_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code, vpiHandle ref)
{
      ସଂରଚନା __vpiModPathTerm*obj = dynamic_cast<__vpiModPathTerm*>(ref);
      ପ୍ରତିଜ୍ଞା(obj);

      ବାଛ (code) {
	  ସ୍ଥିତି vpiEdge:
	    ଫେରନ୍ତୁ obj->edge;
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଫେରନ୍ତୁ 0;
      }
}

ସ୍ଥିର vpiHandle pathterm_get_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code, vpiHandle ref)
{
      ସଂରଚନା __vpiModPathTerm*obj = dynamic_cast<__vpiModPathTerm*>(ref);
      ପ୍ରତିଜ୍ଞା(obj);

      ବାଛ (code) {
	  ସ୍ଥିତି vpi_ପଦସମଷ୍ଟି:
	    ଫେରନ୍ତୁ obj->ପଦସମଷ୍ଟି;
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଫେରନ୍ତୁ 0;
      }
}

/*
* The __vpiModPathSrc class is what the VPI client sees as a
* vpiModPath object. The __vpiModPath structure contains items that
* are common to a bunch of modpaths, including the destination term.
*/
ଧାଡ଼ିଭିତରେ __vpiModPathSrc::__vpiModPathSrc()
{ }

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiModPathSrc::get_type_code(ଶୂନ୍ୟ) ଅଚଳ
{ ଫେରନ୍ତୁ vpiModPath; }

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiModPathSrc::vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{ ଫେରନ୍ତୁ modpath_src_get(code, ଏହି); }

ଶୂନ୍ୟ __vpiModPathSrc::vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val)
{ modpath_src_get_ମୂଲ୍ୟ(ଏହି, val); }

vpiHandle __vpiModPathSrc::vpi_put_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{ ଫେରନ୍ତୁ modpath_src_put_ମୂଲ୍ୟ(ଏହି, val, flags); }

vpiHandle __vpiModPathSrc::vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{ ଫେରନ୍ତୁ modpath_src_get_handle(code, ଏହି); }

vpiHandle __vpiModPathSrc::vpi_iterate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{ ଫେରନ୍ତୁ modpath_src_iterate(code, ଏହି); }

vpiHandle __vpiModPathSrc:: vpi_index(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{ ଫେରନ୍ତୁ modpath_src_index(ଏହି, idx); }

ଶୂନ୍ୟ __vpiModPathSrc::vpi_get_delays(p_vpi_delay del)
{ modpath_src_get_delays(ଏହି, del); }

ଶୂନ୍ୟ __vpiModPathSrc::vpi_put_delays(p_vpi_delay del)
{ modpath_src_put_delays(ଏହି, del); }

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା modpath_src_free_object( vpiHandle ref )
{
      delete ref;
      ଫେରନ୍ତୁ 1 ;
}

__vpiHandle::free_object_fun_t __vpiModPathSrc::free_object_fun(ଶୂନ୍ୟ)
{ ଫେରନ୍ତୁ &modpath_src_free_object; }


ଧାଡ଼ିଭିତରେ __vpiModPathTerm::__vpiModPathTerm()
{ }

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiModPathTerm::get_type_code(ଶୂନ୍ୟ) ଅଚଳ
{ ଫେରନ୍ତୁ vpiPathTerm; }

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiModPathTerm::vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{ ଫେରନ୍ତୁ pathterm_get(code, ଏହି); }

vpiHandle __vpiModPathTerm::vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{ ଫେରନ୍ତୁ pathterm_get_handle(code, ଏହି); }

ସ୍ଥିର ଶୂନ୍ୟ initialize_path_term(ସଂରଚନା __vpiModPathTerm&obj)
{
      obj.ପଦସମଷ୍ଟି = 0;
      obj.edge = vpiNoEdge;
}

/*
 * This function will construct a vpiModPath Object.
 * give a respective "net", and will point to his
 * respective functor
 */

#ifdef CHECK_WITH_VALGRIND
ସ୍ଥିର ସଂରଚନା __vpiModPath**mp_list = 0;
ସ୍ଥିର unsigned mp_ଗଣନା = 0;
#endif

ସଂରଚନା __vpiModPath* vpip_make_modpath(vvp_net_t *net)
{
      ସଂରଚନା __vpiModPath*obj = ନୂତନ __vpiModPath;
      obj->scope = vpip_peek_current_scope ( );

      initialize_path_term(obj->path_term_out);
      obj->ନିବେଶ_net = net ;

#ifdef CHECK_WITH_VALGRIND
      mp_ଗଣନା += 1;
      mp_list = static_cast<ସଂରଚନା __vpiModPath **>
                (realloc(mp_list,
                 mp_ଗଣନା*ଆକାର(ସଂରଚନା __vpiModPath **)));
      mp_list[mp_ଗଣନା-1] = obj;
#endif
      ଫେରନ୍ତୁ obj;
}

#ifdef CHECK_WITH_VALGRIND
ଶୂନ୍ୟ modpath_delete()
{
      ପାଇଁ (unsigned idx = 0; idx < mp_ଗଣନା; idx += 1) {
	    delete mp_list[idx];
      }
      ମୁକ୍ତ(mp_list);
      mp_list = 0;
      mp_ଗଣନା = 0;
}
#endif

/*
 * This function will construct a vpiModPathIn
 * ( struct __vpiModPathSrc ) Object. will give
 * a delays[12] values, and point to the specified functor
 *
 */

ସଂରଚନା __vpiModPathSrc* vpip_make_modpath_src(ସଂରଚନା __vpiModPath*path,
                                              vvp_net_t *net)
{
      ସଂରଚନା __vpiModPathSrc *obj = ନୂତନ __vpiModPathSrc;

      obj->dest = path;
      obj->type = 0;
      obj->net = net;
      initialize_path_term(obj->path_term_in);

      ଫେରନ୍ତୁ obj;
}

vvp_fun_intermodpath::vvp_fun_intermodpath(vvp_net_t*net, unsigned width)
: net_(net)
{
      ପାଇଁ (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = 0;

      cur_vec4_ = vvp_vector4_t(width, BIT4_X);
      schedule_init_propagate(net_, cur_vec4_);
}

vvp_fun_intermodpath::~vvp_fun_intermodpath()
{
}

ଶୂନ୍ୟ vvp_fun_intermodpath::get_delay12(vvp_time64_t val[12]) ଅଚଳ
{
      ପାଇଁ (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    val[idx] = delay_[idx];
}

ଶୂନ୍ୟ vvp_fun_intermodpath::put_delay12(ଅଚଳ vvp_time64_t val[12])
{
      ପାଇଁ (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = val[idx];
}

/*
 * FIXME: this implementation currently only uses the maximum delay
 * from all the bit changes in the vectors. If there are multiple
 * changes with different delays, then the results would be
 * wrong. What should happen is that if there are multiple changes,
 * multiple vectors approaching the result should be scheduled.
 */
ଶୂନ୍ୟ vvp_fun_intermodpath::recv_vec4(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
                              vvp_context_t)
{
	/* Only the first port is used. */
      ଯଦି (port.port() > 0)
	    ଫେରନ୍ତୁ;

      ଯଦି (cur_vec4_.eeq(bit))
	    ଫେରନ୍ତୁ;

	/* Given the scheduled output time, create an output event. */
      vvp_time64_t use_delay = delay_from_edge(cur_vec4_.value(0),
					       bit.value(0),
					       delay_);

	/* FIXME: This bases the edge delay on only the least
	   bit. This is WRONG! I need to find all the possible delays,
	   and schedule an event for each partial change. Hard! */
      ପାଇଁ (unsigned idx = 1 ;  idx < bit.size() ;  idx += 1) {
	    vvp_time64_t tmp = delay_from_edge(cur_vec4_.value(idx),
					       bit.value(idx),
					       delay_);
	      /* If the current and new bit values match then no delay
	       * is needed for this bit. */
	    ଯଦି (cur_vec4_.value(idx) == bit.value(idx)) ଜାରି;
	    ପ୍ରତିଜ୍ଞା(tmp == use_delay);
      }

      cur_vec4_ = bit;
      schedule_generic(ଏହି, use_delay, ମିଥ୍ୟା);
}

ଶୂନ୍ୟ vvp_fun_intermodpath::run_run()
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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା intermodpath_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, vpiHandle ref)
{
      ସଂରଚନା __vpiInterModPath*obj =dynamic_cast<__vpiInterModPath*>(ref);
      ପ୍ରତିଜ୍ଞା(obj);
      ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଶୂନ୍ୟ intermodpath_get_ମୂଲ୍ୟ(vpiHandle ref, p_vpi_ମୂଲ୍ୟ)
{
      ସଂରଚନା __vpiInterModPath* intermodpath = dynamic_cast<__vpiInterModPath*>(ref);
      ପ୍ରତିଜ୍ଞା(intermodpath);
      ଫେରନ୍ତୁ;
}

ସ୍ଥିର vpiHandle intermodpath_put_ମୂଲ୍ୟ(vpiHandle ref, s_vpi_ମୂଲ୍ୟ *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା )
{
      ସଂରଚନା __vpiInterModPath* intermodpath = dynamic_cast<__vpiInterModPath*>(ref);
      ପ୍ରତିଜ୍ଞା(intermodpath);
      ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର vpiHandle intermodpath_get_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code, vpiHandle ref)
{
      ସଂରଚନା __vpiInterModPath*rfp = dynamic_cast<__vpiInterModPath*>(ref);
      ପ୍ରତିଜ୍ଞା(rfp);

      ବାଛ (code) {

	ସ୍ଥିତି vpiScope:
	  ଫେରନ୍ତୁ rfp->scope;

	  ସ୍ଥିତି vpiModule:
	      { __vpiScope*scope = rfp->scope;
		ଯେତେବେଳେ (scope && scope->get_type_code() != vpiModule)
		      scope = scope->scope;
		ପ୍ରତିଜ୍ଞା(scope);
		ଫେରନ୍ତୁ scope;
	      }
      }
      ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର vpiHandle intermodpath_iterate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code, vpiHandle ref)
{
      ସଂରଚନା __vpiInterModPath*rfp = dynamic_cast<__vpiInterModPath*>(ref);
      ପ୍ରତିଜ୍ଞା(rfp);

	// For now intermodpaths only support exactly two ports
      ବାଛ (code) {
	  ସ୍ଥିତି vpiPort: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(2, ଆକାର(vpiHandle*)));
	    args[0] = rfp->port1;
	    args[1] = rfp->port2;
	    ଫେରନ୍ତୁ vpip_make_iterator(2, args, ସତ୍ୟ);
	  }
      }
      ଫେରନ୍ତୁ 0;
}

/*
 * This routine will put specific dimension of delay[] values
 * into a vpiHandle. In this case, we will put
 * specific delays values in a vpiInterModPath object
 *
 */
ସ୍ଥିର ଶୂନ୍ୟ intermodpath_put_delays (vpiHandle ref, p_vpi_delay delays)
{
      vvp_time64_t tmp[12];
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx;
      ସଂରଚନା __vpiInterModPath * src = dynamic_cast<__vpiInterModPath*>(ref) ;
      ପ୍ରତିଜ୍ଞା(src) ;

      vvp_fun_intermodpath *fun = dynamic_cast<vvp_fun_intermodpath*>(src->net->fun);
      ପ୍ରତିଜ୍ଞା( fun );

      ପ୍ରକାରବ୍ୟାଖ୍ୟା unsigned ଅକ୍ଷର map_array_t[12];
	// Only the first six entries are used for the less than twelve maps.
      ସ୍ଥିର ଅଚଳ map_array_t map_1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      ସ୍ଥିର ଅଚଳ map_array_t map_2 = {0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0};
      ସ୍ଥିର ଅଚଳ map_array_t map_3 = {0, 1, 2, 0, 2, 1, 0, 0, 0, 0, 0, 0};
      ସ୍ଥିର ଅଚଳ map_array_t map_6 = {0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0};
      ସ୍ଥିର ଅଚଳ map_array_t map12 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

      ଅଚଳ map_array_t*use_map = 0;
      ବାଛ (delays->no_of_delays) {
	  ସ୍ଥିତି 1:
	    use_map = &map_1;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 2:
	    use_map = &map_2;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 3:
	    use_map = &map_3;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 6:
	    use_map = &map_6;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 12:
	    use_map = &map12;
	    ଭାଙ୍ଗ;
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ପ୍ରତିଜ୍ଞା(0);
	    ଭାଙ୍ଗ;
      }

      ଯଦି (delays->time_type == vpiSimTime) {
	    ପାଇଁ (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_timestruct_to_time(delays->da+use_map[0][idx]);
	    }
      } ଅନ୍ୟଥା {
	      // You cannot create a modpath with a negative delay so set it
	      // to zero per 1364-2005 section 14.3.1.
	    ପାଇଁ (idx = 0 ; idx < delays->no_of_delays ; idx += 1) {
		  ଯଦି (delays->da[idx].real < 0.0) delays->da[idx].real = 0.0;
	    }
	    ପାଇଁ (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_scaled_real_to_time64(delays->da[use_map[0][idx]].real,
							src->scope);
	    }
      }

      /* Now define the to-from-x delays if needed. */
      ଯଦି (delays->no_of_delays <= 6) {
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

ସ୍ଥିର ଶୂନ୍ୟ intermodpath_get_delays ( vpiHandle ref, p_vpi_delay delays )
{
      ସଂରଚନା __vpiInterModPath*src = dynamic_cast<__vpiInterModPath*>(ref) ;
      ପ୍ରତିଜ୍ଞା(src);

      vvp_fun_intermodpath *fun = dynamic_cast<vvp_fun_intermodpath*>(src->net->fun);
      ପ୍ରତିଜ୍ଞା(fun);

      ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx;
      vvp_time64_t tmp[12];
      fun->get_delay12(tmp);

      ବାଛ (delays->no_of_delays) {
	  ସ୍ଥିତି 1:
	  ସ୍ଥିତି 2:
	  ସ୍ଥିତି 3:
	  ସ୍ଥିତି 6:
	  ସ୍ଥିତି 12:
	    ଭାଙ୍ଗ;

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ପ୍ରତିଜ୍ଞା(0);
	    ଭାଙ୍ଗ;
      }

      ଯଦି (delays->time_type == vpiSimTime) {
	    ପାଇଁ (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  vpip_time_to_timestruct(delays->da+idx, tmp[idx]);
	    }
      } ଅନ୍ୟଥା {
	    ପାଇଁ (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  delays->da[idx].real = vpip_time_to_scaled_real(tmp[idx], src->scope);
	    }
      }
}

/*
* The __vpiInterModPath class is what the VPI client sees as a
* vpiInterModPath object.
*/
ଧାଡ଼ିଭିତରେ __vpiInterModPath::__vpiInterModPath()
{ }

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiInterModPath::get_type_code(ଶୂନ୍ୟ) ଅଚଳ
{ ଫେରନ୍ତୁ vpiInterModPath; }

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiInterModPath::vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{ ଫେରନ୍ତୁ intermodpath_get(code, ଏହି); }

ଶୂନ୍ୟ __vpiInterModPath::vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val)
{ intermodpath_get_ମୂଲ୍ୟ(ଏହି, val); }

vpiHandle __vpiInterModPath::vpi_put_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{ ଫେରନ୍ତୁ intermodpath_put_ମୂଲ୍ୟ(ଏହି, val, flags); }

vpiHandle __vpiInterModPath::vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{ ଫେରନ୍ତୁ intermodpath_get_handle(code, ଏହି); }

vpiHandle __vpiInterModPath::vpi_iterate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{ ଫେରନ୍ତୁ intermodpath_iterate(code, ଏହି); }

ଶୂନ୍ୟ __vpiInterModPath::vpi_get_delays(p_vpi_delay del)
{ intermodpath_get_delays(ଏହି, del); }

ଶୂନ୍ୟ __vpiInterModPath::vpi_put_delays(p_vpi_delay del)
{ intermodpath_put_delays(ଏହି, del); }

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା intermodpath_free_object( vpiHandle ref )
{
      delete ref;
      ଫେରନ୍ତୁ 1 ;
}

__vpiHandle::free_object_fun_t __vpiInterModPath::free_object_fun(ଶୂନ୍ୟ)
{ ଫେରନ୍ତୁ &intermodpath_free_object; }

/*
 * This function will construct a vpiInterModPath Object.
 * give a respective "net", and will point to his
 * respective functor
 */

#ifdef CHECK_WITH_VALGRIND
ସ୍ଥିର ସଂରଚନା __vpiInterModPath**imp_list = 0;
ସ୍ଥିର unsigned imp_ଗଣନା = 0;
#endif

ସଂରଚନା __vpiInterModPath* vpip_make_intermodpath(vvp_net_t *net, vpiPortInfo* port1, vpiPortInfo* port2)
{
      ସଂରଚନା __vpiInterModPath*obj = ନୂତନ __vpiInterModPath;
      obj->scope = vpip_peek_current_scope ( );

      obj->net = net;
      obj->port1 = port1;
      obj->port2 = port2;

#ifdef CHECK_WITH_VALGRIND
      imp_ଗଣନା += 1;
      imp_list = static_cast<ସଂରଚନା __vpiInterModPath **>
                 (realloc(imp_list,
                  imp_ଗଣନା*ଆକାର(ସଂରଚନା __vpiInterModPath **)));
      imp_list[imp_ଗଣନା-1] = obj;
#endif
      ଫେରନ୍ତୁ obj;
}

#ifdef CHECK_WITH_VALGRIND
ଶୂନ୍ୟ intermodpath_delete()
{
      ପାଇଁ (unsigned idx = 0; idx < mp_ଗଣନା; idx += 1) {
	    delete imp_list[idx];
      }
      ମୁକ୍ତ(imp_list);
      imp_list = 0;
      imp_ଗଣନା = 0;
}
#endif
