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

#அந்தர்பாவ "delay.h"
#அந்தர்பாவ "schedule.h"
#அந்தர்பாவ "vpi_priv.h"
#அந்தர்பாவ "config.h"
#ifdef CHECK_WITH_VALGRIND
#அந்தர்பாவ "vvp_cleanup.h"
#endif
#அந்தர்பாவ <iostream>
#அந்தர்பாவ <cstdlib>
#அந்தர்பாவ <list>
#அந்தர்பாவ <cassert>
#அந்தர்பாவ <cmath>
#அந்தர்பாவ "ivl_alloc.h"

using நாமாகாஶ std;

வெறுமை vvp_delay_t::calculate_min_delay_()
{
      min_delay_ = rise_;
      எனில் (fall_ < min_delay_)
	    min_delay_ = fall_;
      எனில் (ignore_decay_) decay_ = min_delay_;
      இல்லையெனில் எனில் (decay_ < min_delay_)
	    min_delay_ = decay_;
}

vvp_delay_t::vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall)
{
      rise_ = rise;
      fall_ = fall;
      decay_= fall < rise? fall : rise;
      min_delay_ = decay_;
      ignore_decay_ = பொய்;
}

vvp_delay_t::vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall, vvp_time64_t decay)
{
      rise_ = rise;
      fall_ = fall;
      decay_= decay;
      ignore_decay_ = பொய்;

      calculate_min_delay_();
}

வெறுமை vvp_delay_t::set_ignore_decay()
{
      ignore_decay_ = உண்மை;

      calculate_min_delay_();
}

vvp_delay_t::~vvp_delay_t()
{
}

vvp_time64_t vvp_delay_t::get_delay(vvp_bit4_t from, vvp_bit4_t to)
{
      தேர்வு (from) {
	  நிலை BIT4_0:
	    தேர்வு (to) {
		நிலை BIT4_0: திரும்பு 0;
		நிலை BIT4_1: திரும்பு rise_;
		நிலை BIT4_X: திரும்பு min_delay_;
		நிலை BIT4_Z: திரும்பு decay_;
	    }
	    நிறுத்து;
	  நிலை BIT4_1:
	    தேர்வு (to) {
		நிலை BIT4_0: திரும்பு fall_;
		நிலை BIT4_1: திரும்பு 0;
		நிலை BIT4_X: திரும்பு min_delay_;
		நிலை BIT4_Z: திரும்பு decay_;
	    }
	    நிறுத்து;
	  நிலை BIT4_X:
	    தேர்வு (to) {
		நிலை BIT4_0: திரும்பு fall_;
		நிலை BIT4_1: திரும்பு rise_;
		நிலை BIT4_X: திரும்பு 0;
		நிலை BIT4_Z: திரும்பு decay_;
	    }
	    நிறுத்து;
	  நிலை BIT4_Z:
	    தேர்வு (to) {
		நிலை BIT4_0: திரும்பு fall_;
		நிலை BIT4_1: திரும்பு rise_;
		நிலை BIT4_X: திரும்பு min_delay_;
		நிலை BIT4_Z: திரும்பு 0;
	    }
	    நிறுத்து;
      }

      ப்ரதிஜ்ஞா(0);
      திரும்பு 0;
}

vvp_time64_t vvp_delay_t::get_min_delay() மாறிலி
{
      திரும்பு min_delay_;
}

வெறுமை vvp_delay_t::set_rise(vvp_time64_t val)
{
      rise_ = val;
      எனில் (val < min_delay_) {
	    min_delay_ = val;
	    எனில் (ignore_decay_) decay_ = val;
      } இல்லையெனில்
	    calculate_min_delay_();
}

வெறுமை vvp_delay_t::set_fall(vvp_time64_t val)
{
      fall_ = val;
      எனில் (val < min_delay_) {
	    min_delay_ = val;
	    எனில் (ignore_decay_) decay_ = val;
      } இல்லையெனில்
	    calculate_min_delay_();
}

வெறுமை vvp_delay_t::set_decay(vvp_time64_t val)
{
      ப்ரதிஜ்ஞா(!ignore_decay_);

      decay_ = val;
      எனில் (val < min_delay_)
	    min_delay_ = val;
      இல்லையெனில்
	    calculate_min_delay_();
}

vvp_fun_delay::vvp_fun_delay(vvp_net_t*n, unsigned width, மாறிலி vvp_delay_t&d)
: net_(n), delay_(d)
{
      cur_real_ = 0.0;
      எனில் (width > 0) {
            cur_vec4_ = vvp_vector4_t(width, BIT4_X);
            cur_vec8_ = vvp_vector8_t(cur_vec4_, 6, 6);
            schedule_init_propagate(net_, cur_vec4_);
      } இல்லையெனில் {
            schedule_init_propagate(net_, cur_real_);
      }
      list_ = 0;
      type_ = UNKNOWN_DELAY;
      initial_ = உண்மை;
	// Calculate the values used when converting variable delays
	// to simulation time units.
      மாறிலி __vpiScope*scope = vpip_peek_current_scope();

      முழு powr = scope->time_units - scope->time_precision;
      round_ = 1;
      க்கு (முழு lp = 0; lp < powr; lp += 1) round_ *= 10;

      powr = scope->time_precision - vpip_get_time_precision();
      scale_ = 1;
      க்கு (முழு lp = 0; lp < powr; lp += 1) scale_ *= 10;
}

vvp_fun_delay::~vvp_fun_delay()
{
      போது (கட்டமைப்பு event_*cur = dequeue_())
	    delete cur;
}

தருக்க vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        மாறிலி vvp_vector4_t&bit)
{
      எனில் (list_ == 0) திரும்பு பொய்;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      எனில் (list_->next->ptr_vec4.eeq(bit)) திரும்பு உண்மை;

      clean_pulse_events_(use_delay);
      திரும்பு பொய்;
}

தருக்க vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        மாறிலி vvp_vector8_t&bit)
{
      எனில் (list_ == 0) திரும்பு பொய்;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      எனில் (list_->next->ptr_vec8.eeq(bit)) திரும்பு உண்மை;

      clean_pulse_events_(use_delay);
      திரும்பு பொய்;
}

தருக்க vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        இரட்டை bit)
{
      எனில் (list_ == 0) திரும்பு பொய்;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      எனில் (list_->next->ptr_real == bit) திரும்பு உண்மை;

      clean_pulse_events_(use_delay);
      திரும்பு பொய்;
}

வெறுமை vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay)
{
      ப்ரதிஜ்ஞா(list_ != 0);

      செய் {
	    கட்டமைப்பு event_*cur = list_->next;
	      /* If this event is far enough from the event I'm about
	         to create, then that scheduled event is not a pulse
	         to be eliminated, so we're done. */
	    எனில் (cur->sim_time+use_delay <= use_delay+schedule_simtime())
		  நிறுத்து;

	    எனில் (list_ == cur)
		  list_ = 0;
	    இல்லையெனில்
		  list_->next = cur->next;
	    delete cur;
      } போது (list_);
}

/*
 * FIXME: this implementation currently only uses the maximum delay
 * from all the bit changes in the vectors. If there are multiple
 * changes with different delays, then the results would be
 * wrong. What should happen is that if there are multiple changes,
 * multiple vectors approaching the result should be scheduled.
 */
வெறுமை vvp_fun_delay::recv_vec4(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
                              vvp_context_t)
{
      எனில் (port.port() > 0) {
	      // Get the integer value of the bit vector, or 0 if
	      // there are X or Z bits.
	    vvp_time64_t bval = 0;
	      // The following does not work correctly for negative values.
	      // They should be sign extended to 64 bits (1364-2001 9.7.1).
	    vector4_to_மதிப்பு(bit, bval);
	      // Integer values do not need to be rounded so just scale them.
	    vvp_time64_t val = bval * round_ * scale_;

	    தேர்வு (port.port()) {
		நிலை 1:
		  delay_.set_rise(val);
		  திரும்பு;
		நிலை 2:
		  delay_.set_fall(val);
		  திரும்பு;
		நிலை 3:
		  delay_.set_decay(val);
		  திரும்பு;
	    }
	    திரும்பு;
      }

      vvp_time64_t use_delay;
	/* This is an initial value so it needs to be compared to all the
	   bits (the order the bits are changed is not deterministic). */
      எனில் (initial_) {
	    type_ = VEC4_DELAY;
            cur_vec8_ = vvp_vector8_t(vvp_vector4_t(0, BIT4_X), 6, 6);
	    vvp_bit4_t cur_val = cur_vec4_.value(0);
	    use_delay = delay_.get_delay(cur_val, bit.value(0));
	    க்கு (unsigned idx = 1 ;  idx < bit.size() ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(cur_val, bit.value(idx));
		  எனில் (tmp > use_delay) use_delay = tmp;
	    }
      } இல்லையெனில் {
	    ப்ரதிஜ்ஞா(type_ == VEC4_DELAY);

	      // Use as a reference for calculating the delay the
	      // current value of the output. Detect and handle the
	      // special case that the event list contains the current
	      // value as a zero-delay-remaining event.
	    மாறிலி vvp_vector4_t&use_vec4 = (list_ && list_->next->sim_time == schedule_simtime())? list_->next->ptr_vec4 : cur_vec4_;

	      /* How many bits to compare? */
	    unsigned use_wid = use_vec4.size();
	    எனில் (bit.size() < use_wid) use_wid = bit.size();

	      /* Scan the vectors looking for delays. Select the maximum
	         delay encountered. */
	    use_delay = delay_.get_delay(use_vec4.value(0), bit.value(0));

	    க்கு (unsigned idx = 1 ;  idx < use_wid ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(use_vec4.value(idx), bit.value(idx));
		  எனில் (tmp > use_delay) use_delay = tmp;
	    }
      }

      /* what *should* happen here is we check to see if there is a
         transaction in the queue. This would be a pulse that needs to be
         eliminated. */
      எனில் (clean_pulse_events_(use_delay, bit)) திரும்பு;

      vvp_time64_t use_simtime = schedule_simtime() + use_delay;

	/* And propagate it. */
      எனில் (use_delay == 0 && list_ == 0) {
	    cur_vec4_ = bit;
	    initial_ = பொய்;
	    net_->send_vec4(cur_vec4_, 0);
      } இல்லையெனில் {
	    கட்டமைப்பு event_*cur = புதிய கட்டமைப்பு event_(use_simtime);
	    cur->run_run_ptr = &vvp_fun_delay::run_run_vec4_;
	    cur->ptr_vec4 = bit;
	    enqueue_(cur);
	    schedule_generic(இது, use_delay, பொய்);
      }
}

வெறுமை vvp_fun_delay::recv_vec4_pv(vvp_net_ptr_t ptr, மாறிலி vvp_vector4_t&bit,
			         unsigned base, unsigned vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

/* See the recv_vec4 comment above. */
வெறுமை vvp_fun_delay::recv_vec8(vvp_net_ptr_t port, மாறிலி vvp_vector8_t&bit)
{
      ப்ரதிஜ்ஞா(port.port() == 0);

      vvp_time64_t use_delay;
	/* This is an initial value so it needs to be compared to all the
	   bits (the order the bits are changed is not deterministic). */
      எனில் (initial_) {
	    type_ = VEC8_DELAY;
            cur_vec4_ = vvp_vector4_t(0, BIT4_X);
	    vvp_bit4_t cur_val = cur_vec8_.value(0).value();
	    use_delay = delay_.get_delay(cur_val, bit.value(0).value());
	    க்கு (unsigned idx = 1 ;  idx < bit.size() ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(cur_val, bit.value(idx).value());
		  எனில் (tmp > use_delay) use_delay = tmp;
	    }
      } இல்லையெனில் {
	    ப்ரதிஜ்ஞா(type_ == VEC8_DELAY);

	      // Use as a reference for calculating the delay the
	      // current value of the output. Detect and handle the
	      // special case that the event list contains the current
	      // value as a zero-delay-remaining event.
	    மாறிலி vvp_vector8_t&use_vec8 = (list_ && list_->next->sim_time == schedule_simtime())? list_->next->ptr_vec8 : cur_vec8_;

	      /* How many bits to compare? */
	    unsigned use_wid = use_vec8.size();
	    எனில் (bit.size() < use_wid) use_wid = bit.size();

	      /* Scan the vectors looking for delays. Select the maximum
	         delay encountered. */
	    use_delay = delay_.get_delay(use_vec8.value(0).value(),
	                                 bit.value(0).value());

	    க்கு (unsigned idx = 1 ;  idx < use_wid ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(use_vec8.value(idx).value(),
		                         bit.value(idx).value());
		  எனில் (tmp > use_delay) use_delay = tmp;
	    }
      }

      /* what *should* happen here is we check to see if there is a
         transaction in the queue. This would be a pulse that needs to be
         eliminated. */
      எனில் (clean_pulse_events_(use_delay, bit)) திரும்பு;

      vvp_time64_t use_simtime = schedule_simtime() + use_delay;

	/* And propagate it. */
      எனில் (use_delay == 0 && list_ == 0) {
	    cur_vec8_ = bit;
	    initial_ = பொய்;
	    net_->send_vec8(cur_vec8_);
      } இல்லையெனில் {
	    கட்டமைப்பு event_*cur = புதிய கட்டமைப்பு event_(use_simtime);
	    cur->ptr_vec8 = bit;
	    cur->run_run_ptr = &vvp_fun_delay::run_run_vec8_;
	    enqueue_(cur);
	    schedule_generic(இது, use_delay, பொய்);
      }
}

வெறுமை vvp_fun_delay::recv_vec8_pv(vvp_net_ptr_t ptr, மாறிலி vvp_vector8_t&bit,
			         unsigned base, unsigned vwid)
{
      recv_vec8_pv_(ptr, bit, base, vwid);
}

வெறுமை vvp_fun_delay::recv_real(vvp_net_ptr_t port, இரட்டை bit,
                              vvp_context_t)
{
      எனில் (port.port() > 0) {
	    /* If the port is not 0, then this is a delay value that
	    should be rounded and converted to an integer delay. */
	    vvp_time64_t val = 0;
	    எனில் (bit > -0.5) {
		  val = (vvp_time64_t) (bit * round_ + 0.5) * scale_;
	    } இல்லையெனில் எனில் (bit != bit) {
		    // For a NaN we use the default (0).
	    } இல்லையெனில் {
		  vvp_vector4_t vec4(8*அளவு(vvp_time64_t),
		                     floor(-bit * round_ + 0.5) * -1 * scale_);
		  vector4_to_மதிப்பு(vec4, val);
	    }

	    தேர்வு (port.port()) {
		நிலை 1:
		  delay_.set_rise(val);
		  திரும்பு;
		நிலை 2:
		  delay_.set_fall(val);
		  திரும்பு;
		நிலை 3:
		  delay_.set_decay(val);
		  திரும்பு;
	    }
	    திரும்பு;
      }

      எனில் (initial_) {
	    type_ = REAL_DELAY;
            cur_vec4_ = vvp_vector4_t(0, BIT4_X);
            cur_vec8_ = vvp_vector8_t(cur_vec4_, 6, 6);
      } இல்லையெனில் ப்ரதிஜ்ஞா(type_ == REAL_DELAY);

      vvp_time64_t use_delay;
      use_delay = delay_.get_min_delay();

      /* Eliminate glitches. */
      எனில் (clean_pulse_events_(use_delay, bit)) திரும்பு;

      /* This must be done after cleaning pulses to avoid propagating
       * an incorrect value. */
      எனில் (cur_real_ == bit) திரும்பு;

      vvp_time64_t use_simtime = schedule_simtime() + use_delay;

      எனில் (use_delay == 0 && list_ == 0) {
	    cur_real_ = bit;
	    initial_ = பொய்;
	    net_->send_real(cur_real_, 0);
      } இல்லையெனில் {
	    கட்டமைப்பு event_*cur = புதிய கட்டமைப்பு event_(use_simtime);
	    cur->run_run_ptr = &vvp_fun_delay::run_run_real_;
	    cur->ptr_real = bit;
	    enqueue_(cur);

	    schedule_generic(இது, use_delay, பொய்);
      }
}

வெறுமை vvp_fun_delay::run_run()
{
      vvp_time64_t sim_time = schedule_simtime();
      எனில் (list_ == 0 || list_->next->sim_time > sim_time)
	    திரும்பு;

      கட்டமைப்பு event_*cur = dequeue_();
      எனில் (cur == 0)
	    திரும்பு;

      (இது->*(cur->run_run_ptr))(cur);
      initial_ = பொய்;
      delete cur;
}

வெறுமை vvp_fun_delay::run_run_vec4_(மாறிலி கட்டமைப்பு event_*cur)
{
      cur_vec4_ = cur->ptr_vec4;
      net_->send_vec4(cur_vec4_, 0);
}

வெறுமை vvp_fun_delay::run_run_vec8_(மாறிலி கட்டமைப்பு vvp_fun_delay::event_*cur)
{
      cur_vec8_ = cur->ptr_vec8;
      net_->send_vec8(cur_vec8_);
}

வெறுமை vvp_fun_delay::run_run_real_(மாறிலி கட்டமைப்பு vvp_fun_delay::event_*cur)
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
      போது (src_list_) {
	    vvp_fun_modpath_src*tmp = src_list_;
	    src_list_ = tmp->next_;
	    delete tmp;
      }
      போது (ifnone_list_) {
	    vvp_fun_modpath_src*tmp = ifnone_list_;
	    ifnone_list_ = tmp->next_;
	    delete tmp;
      }
}

வெறுமை vvp_fun_modpath::add_modpath_src(vvp_fun_modpath_src*that, தருக்க ifnone)
{
      ப்ரதிஜ்ஞா(that->next_ == 0);
      எனில் (ifnone) {
	    that->next_ = ifnone_list_;
	    ifnone_list_ = that;
      } இல்லையெனில் {
	    that->next_ = src_list_;
	    src_list_ = that;
      }
}

நிலையான vvp_time64_t delay_from_edge(vvp_bit4_t a, vvp_bit4_t b,
                                           vvp_time64_t மாறிலி array[12])
{
      வகைவரை delay_edge_t bit4_table4[4];
      நிலையான மாறிலி bit4_table4 edge_table[4] = {
	    { DELAY_EDGE_01, DELAY_EDGE_01, DELAY_EDGE_0z, DELAY_EDGE_0x },
	    { DELAY_EDGE_10, DELAY_EDGE_10, DELAY_EDGE_1z, DELAY_EDGE_1x },
	    { DELAY_EDGE_z0, DELAY_EDGE_z1, DELAY_EDGE_z0, DELAY_EDGE_zx },
	    { DELAY_EDGE_x0, DELAY_EDGE_x1, DELAY_EDGE_xz, DELAY_EDGE_x0 }
      };

      திரும்பு array[ edge_table[a][b] ];
}

வெறுமை vvp_fun_modpath::recv_vec4(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
                                vvp_context_t)
{
	/* Only the first port is used. */
      எனில் (port.port() > 0)
	    திரும்பு;

      எனில் (cur_vec4_.eeq(bit))
	    திரும்பு;

	/* Select a time delay source that applies. Notice that there
	   may be multiple delay sources that apply, so collect all
	   the candidates into a list first. */
      list<vvp_fun_modpath_src*>candidate_list;
      vvp_time64_t candidate_wake_time = 0;
      க்கு (vvp_fun_modpath_src*cur = src_list_ ;  cur ;  cur=cur->next_) {
	      /* Skip paths that are disabled by conditions. */
	    எனில் (cur->condition_flag_ == பொய்)
		  தொடர்;

	    எனில் (candidate_list.empty()) {
		  candidate_list.push_back(cur);
		  candidate_wake_time = cur->wake_time_;
	    } இல்லையெனில் எனில் (cur->wake_time_ == candidate_wake_time) {
		  candidate_list.push_back(cur);
	    } இல்லையெனில் எனில் (cur->wake_time_ > candidate_wake_time) {
		  candidate_list.assign(1, cur);
		  candidate_wake_time = cur->wake_time_;
	    } இல்லையெனில் {
		  தொடர்; /* Skip this entry. */
	    }
      }

	/* Only add the ifnone delay if it has a later wake_time_ or
	 * if there are no normal delays. */
      vvp_time64_t ifnone_wake_time = candidate_wake_time;
      க்கு (vvp_fun_modpath_src*cur = ifnone_list_ ;  cur ;  cur=cur->next_) {
	    எனில் (candidate_list.empty()) {
		  candidate_list.push_back(cur);
		  ifnone_wake_time = cur->wake_time_;
	    } இல்லையெனில் எனில் (cur->wake_time_ == ifnone_wake_time &&
	               ifnone_wake_time > candidate_wake_time) {
		  candidate_list.push_back(cur);
	    } இல்லையெனில் எனில் (cur->wake_time_ > ifnone_wake_time) {
		  candidate_list.assign(1, cur);
		  ifnone_wake_time = cur->wake_time_;
	    } இல்லையெனில் {
		  தொடர்; /* Skip this entry. */
	    }
      }

	/* Handle the special case that there are no delays that
	   match. This may happen, for example, if the set of
	   conditional delays is incomplete, leaving some cases
	   uncovered. In that case, just pass the data without delay */
      எனில் (candidate_list.empty()) {
	    cur_vec4_ = bit;
	    schedule_generic(இது, 0, பொய்);
	    திரும்பு;
      }

	/* Now given that we have a list of candidate delays, find for
	   each if the 12 numbers the minimum from all the
	   candidates. This minimum set becomes the chosen delay to
	   use. */
      vvp_time64_t out_at[12];
      vvp_time64_t now = schedule_simtime();

      வகைவரை list<vvp_fun_modpath_src*>::const_iterator iter_t;

      iter_t cur = candidate_list.begin();
      மாறிலி vvp_fun_modpath_src*src = *cur;

      க்கு (unsigned idx = 0 ;  idx < 12 ;  idx += 1) {
	    out_at[idx] = src->wake_time_ + src->delay_[idx];
	    எனில் (out_at[idx] <= now)
		  out_at[idx] = 0;
	    இல்லையெனில்
		  out_at[idx] -= now;
      }

      க்கு (++ cur ; cur != candidate_list.end() ; ++ cur ) {
	    src = *cur;
	    க்கு (unsigned idx = 0 ;  idx < 12 ;  idx += 1) {
		  vvp_time64_t tmp = src->wake_time_ + src->delay_[idx];
		  எனில் (tmp <= now)
			tmp = 0;
		  இல்லையெனில்
			tmp -= now;
		  எனில் (tmp < out_at[idx])
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
      க்கு (unsigned idx = 1 ;  idx < bit.size() ;  idx += 1) {
	    vvp_time64_t tmp = delay_from_edge(cur_vec4_.value(idx),
					       bit.value(idx),
					       out_at);
	      /* If the current and new bit values match then no delay
	       * is needed for this bit. */
	    எனில் (cur_vec4_.value(idx) == bit.value(idx)) தொடர்;
	    எனில் (tmp != use_delay) {
		  fprintf(stderr, "sorry: multi-bit module path delays are "
		                  "currently not fully supported.\n");
		  fprintf(stderr, "     : using the LSB delay for all bits.\n");
		  தொடர்;
            }
      }

      cur_vec4_ = bit;
      schedule_generic(இது, use_delay, பொய்);
}

வெறுமை vvp_fun_modpath::run_run()
{
      net_->send_vec4(cur_vec4_, 0);
}

vvp_fun_modpath_src::vvp_fun_modpath_src(vvp_time64_t மாறிலி del[12])
{
      க்கு (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = del[idx];

      next_ = 0;
      wake_time_ = 0;
      condition_flag_ = உண்மை;
}

vvp_fun_modpath_src::~vvp_fun_modpath_src()
{
}

வெறுமை vvp_fun_modpath_src::get_delay12(vvp_time64_t val[12]) மாறிலி
{
      க்கு (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    val[idx] = delay_[idx];
}

வெறுமை vvp_fun_modpath_src::put_delay12(மாறிலி vvp_time64_t val[12])
{
      க்கு (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = val[idx];
}

வெறுமை vvp_fun_modpath_src::recv_vec4(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
                                    vvp_context_t)
{
      எனில் (port.port() == 0) {
	      // The modpath input...
	    எனில் (test_vec4(bit))
		  wake_time_ = schedule_simtime();

      } இல்லையெனில் எனில் (port.port() == 1) {
	      // The modpath condition input...
	    எனில் (bit.value(0) != BIT4_0)
		  condition_flag_ = உண்மை;
	    இல்லையெனில்
		  condition_flag_ = பொய்;
      }
}

தருக்க vvp_fun_modpath_src::test_vec4(மாறிலி vvp_vector4_t&)
{
      திரும்பு உண்மை;
}

vvp_fun_modpath_edge::vvp_fun_modpath_edge(vvp_time64_t del[12],
					   தருக்க pos, தருக்க neg)
: vvp_fun_modpath_src(del)
{
      old_மதிப்பு = BIT4_X;
      posedge_ = pos;
      negedge_ = neg;
}

தருக்க vvp_fun_modpath_edge::test_vec4(மாறிலி vvp_vector4_t&bit)
{
      vvp_bit4_t tmp = old_மதிப்பு;
      old_மதிப்பு = bit.value(0);

      முழு edge_flag = edge(tmp, old_மதிப்பு);
      எனில் (edge_flag > 0) திரும்பு posedge_;
      எனில் (edge_flag < 0) திரும்பு negedge_;
      திரும்பு பொய்;
}


/*
 * All the below routines that begin with
 * modpath_src_* belong the internal function
 * of an vpiModPathIn object. This is used to
 * make some specific delays path operations
 *
 */
நிலையான முழு modpath_src_get(முழு, vpiHandle ref)
{
      கட்டமைப்பு __vpiModPathSrc*obj =dynamic_cast<__vpiModPathSrc*>(ref);
      ப்ரதிஜ்ஞா(obj);
      திரும்பு 0;
}

நிலையான வெறுமை modpath_src_get_மதிப்பு(vpiHandle ref, p_vpi_மதிப்பு)
{
      கட்டமைப்பு __vpiModPathSrc* modpathsrc = dynamic_cast<__vpiModPathSrc*>(ref);
      ப்ரதிஜ்ஞா(modpathsrc);
      திரும்பு;
}

நிலையான vpiHandle modpath_src_put_மதிப்பு(vpiHandle ref, s_vpi_மதிப்பு *, முழு )
{
      கட்டமைப்பு __vpiModPathSrc* modpathsrc = dynamic_cast<__vpiModPathSrc*>(ref);
      ப்ரதிஜ்ஞா(modpathsrc);
      திரும்பு 0;
}

நிலையான vpiHandle modpath_src_get_handle(முழு code, vpiHandle ref)
{
      கட்டமைப்பு __vpiModPathSrc*rfp = dynamic_cast<__vpiModPathSrc*>(ref);
      ப்ரதிஜ்ஞா(rfp);

      தேர்வு (code) {

	நிலை vpiScope:
	  திரும்பு rfp->dest->scope;

	  நிலை vpiModule:
	      { __vpiScope*scope = rfp->dest->scope;
		போது (scope && scope->get_type_code() != vpiModule)
		      scope = scope->scope;
		ப்ரதிஜ்ஞா(scope);
		திரும்பு scope;
	      }

	    // Handles to path term objects should really be obtained via
	    // the vpi_iterate and vpi_scan functions. Continue to allow
	    // them to be obtained here for backwards compatibility with
	    // older versions of Icarus Verilog.

	  நிலை vpiModPathIn:
	    திரும்பு &rfp->path_term_in;

	  நிலை vpiModPathOut:
	    திரும்பு &rfp->dest->path_term_out;
      }
      திரும்பு 0;
}

நிலையான vpiHandle modpath_src_iterate(முழு code, vpiHandle ref)
{
      கட்டமைப்பு __vpiModPathSrc*rfp = dynamic_cast<__vpiModPathSrc*>(ref);
      ப்ரதிஜ்ஞா(rfp);

	// Module paths with multiple sources or destinations are
	// currently represented by a separate modpath object for
	// each source/destination combination, so there is only
	// ever one input path term and one output path term.
      தேர்வு (code) {
	  நிலை vpiModPathIn: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(1, அளவு(vpiHandle*)));
	    args[0] = &rfp->path_term_in;
	    திரும்பு vpip_make_iterator(1, args, உண்மை);
	  }
	  நிலை vpiModPathOut: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(1, அளவு(vpiHandle*)));
	    args[0] = &rfp->dest->path_term_out;
	    திரும்பு vpip_make_iterator(1, args, உண்மை);
	  }
      }
      திரும்பு 0;
}

நிலையான vpiHandle modpath_src_index ( vpiHandle ref, முழு)
{
      ப்ரதிஜ்ஞா(ref->get_type_code() == vpiModPathIn);
      திரும்பு 0;
}


/*
 * This routine will put specific dimension of delay[] values
 * into a vpiHandle. In this case, we will put
 * specific delays values in a vpiModPathIn object
 *
 */
நிலையான வெறுமை modpath_src_put_delays (vpiHandle ref, p_vpi_delay delays)
{
      vvp_time64_t tmp[12];
      முழு idx;
      கட்டமைப்பு __vpiModPathSrc * src = dynamic_cast<__vpiModPathSrc*>(ref) ;
      ப்ரதிஜ்ஞா(src) ;

      vvp_fun_modpath_src *fun = dynamic_cast<vvp_fun_modpath_src*>(src->net->fun);
      ப்ரதிஜ்ஞா( fun );

      வகைவரை unsigned எழுத்து map_array_t[12];
	// Only the first six entries are used for the less than twelve maps.
      நிலையான மாறிலி map_array_t map_1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      நிலையான மாறிலி map_array_t map_2 = {0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0};
      நிலையான மாறிலி map_array_t map_3 = {0, 1, 2, 0, 2, 1, 0, 0, 0, 0, 0, 0};
      நிலையான மாறிலி map_array_t map_6 = {0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0};
      நிலையான மாறிலி map_array_t map12 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

      மாறிலி map_array_t*use_map = 0;
      தேர்வு (delays->no_of_delays) {
	  நிலை 1:
	    use_map = &map_1;
	    நிறுத்து;
	  நிலை 2:
	    use_map = &map_2;
	    நிறுத்து;
	  நிலை 3:
	    use_map = &map_3;
	    நிறுத்து;
	  நிலை 6:
	    use_map = &map_6;
	    நிறுத்து;
	  நிலை 12:
	    use_map = &map12;
	    நிறுத்து;
	  இயல்பு:
	    ப்ரதிஜ்ஞா(0);
	    நிறுத்து;
      }

      எனில் (delays->time_type == vpiSimTime) {
	    க்கு (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_timestruct_to_time(delays->da+use_map[0][idx]);
	    }
      } இல்லையெனில் {
	      // You cannot create a modpath with a negative delay so set it
	      // to zero per 1364-2005 section 14.3.1.
	    க்கு (idx = 0 ; idx < delays->no_of_delays ; idx += 1) {
		  எனில் (delays->da[idx].real < 0.0) delays->da[idx].real = 0.0;
	    }
	    க்கு (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_scaled_real_to_time64(delays->da[use_map[0][idx]].real,
							src->dest->scope);
	    }
      }

      /* Now define the to-from-x delays if needed. */
      எனில் (delays->no_of_delays <= 6) {
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

நிலையான வெறுமை modpath_src_get_delays ( vpiHandle ref, p_vpi_delay delays )
{
      கட்டமைப்பு __vpiModPathSrc*src = dynamic_cast<__vpiModPathSrc*>(ref) ;
      ப்ரதிஜ்ஞா(src);

      vvp_fun_modpath_src *fun = dynamic_cast<vvp_fun_modpath_src*>(src->net->fun);
      ப்ரதிஜ்ஞா(fun);

      முழு idx;
      vvp_time64_t tmp[12];
      fun->get_delay12(tmp);

      தேர்வு (delays->no_of_delays) {
	  நிலை 1:
	  நிலை 2:
	  நிலை 3:
	  நிலை 6:
	  நிலை 12:
	    நிறுத்து;

	  இயல்பு:
	    ப்ரதிஜ்ஞா(0);
	    நிறுத்து;
      }

      எனில் (delays->time_type == vpiSimTime) {
	    க்கு (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  vpip_time_to_timestruct(delays->da+idx, tmp[idx]);
	    }
      } இல்லையெனில் {
	    க்கு (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  delays->da[idx].real = vpip_time_to_scaled_real(tmp[idx], src->dest->scope);
	    }
      }
}

நிலையான முழு pathterm_get(முழு code, vpiHandle ref)
{
      கட்டமைப்பு __vpiModPathTerm*obj = dynamic_cast<__vpiModPathTerm*>(ref);
      ப்ரதிஜ்ஞா(obj);

      தேர்வு (code) {
	  நிலை vpiEdge:
	    திரும்பு obj->edge;
	  இயல்பு:
	    திரும்பு 0;
      }
}

நிலையான vpiHandle pathterm_get_handle(முழு code, vpiHandle ref)
{
      கட்டமைப்பு __vpiModPathTerm*obj = dynamic_cast<__vpiModPathTerm*>(ref);
      ப்ரதிஜ்ஞா(obj);

      தேர்வு (code) {
	  நிலை vpi_கோவை:
	    திரும்பு obj->கோவை;
	  இயல்பு:
	    திரும்பு 0;
      }
}

/*
* The __vpiModPathSrc class is what the VPI client sees as a
* vpiModPath object. The __vpiModPath structure contains items that
* are common to a bunch of modpaths, including the destination term.
*/
உள்வரி __vpiModPathSrc::__vpiModPathSrc()
{ }

முழு __vpiModPathSrc::get_type_code(வெறுமை) மாறிலி
{ திரும்பு vpiModPath; }

முழு __vpiModPathSrc::vpi_get(முழு code)
{ திரும்பு modpath_src_get(code, இது); }

வெறுமை __vpiModPathSrc::vpi_get_மதிப்பு(p_vpi_மதிப்பு val)
{ modpath_src_get_மதிப்பு(இது, val); }

vpiHandle __vpiModPathSrc::vpi_put_மதிப்பு(p_vpi_மதிப்பு val, முழு flags)
{ திரும்பு modpath_src_put_மதிப்பு(இது, val, flags); }

vpiHandle __vpiModPathSrc::vpi_handle(முழு code)
{ திரும்பு modpath_src_get_handle(code, இது); }

vpiHandle __vpiModPathSrc::vpi_iterate(முழு code)
{ திரும்பு modpath_src_iterate(code, இது); }

vpiHandle __vpiModPathSrc:: vpi_index(முழு idx)
{ திரும்பு modpath_src_index(இது, idx); }

வெறுமை __vpiModPathSrc::vpi_get_delays(p_vpi_delay del)
{ modpath_src_get_delays(இது, del); }

வெறுமை __vpiModPathSrc::vpi_put_delays(p_vpi_delay del)
{ modpath_src_put_delays(இது, del); }

நிலையான முழு modpath_src_free_object( vpiHandle ref )
{
      delete ref;
      திரும்பு 1 ;
}

__vpiHandle::free_object_fun_t __vpiModPathSrc::free_object_fun(வெறுமை)
{ திரும்பு &modpath_src_free_object; }


உள்வரி __vpiModPathTerm::__vpiModPathTerm()
{ }

முழு __vpiModPathTerm::get_type_code(வெறுமை) மாறிலி
{ திரும்பு vpiPathTerm; }

முழு __vpiModPathTerm::vpi_get(முழு code)
{ திரும்பு pathterm_get(code, இது); }

vpiHandle __vpiModPathTerm::vpi_handle(முழு code)
{ திரும்பு pathterm_get_handle(code, இது); }

நிலையான வெறுமை initialize_path_term(கட்டமைப்பு __vpiModPathTerm&obj)
{
      obj.கோவை = 0;
      obj.edge = vpiNoEdge;
}

/*
 * This function will construct a vpiModPath Object.
 * give a respective "net", and will point to his
 * respective functor
 */

#ifdef CHECK_WITH_VALGRIND
நிலையான கட்டமைப்பு __vpiModPath**mp_list = 0;
நிலையான unsigned mp_மொத்தம் = 0;
#endif

கட்டமைப்பு __vpiModPath* vpip_make_modpath(vvp_net_t *net)
{
      கட்டமைப்பு __vpiModPath*obj = புதிய __vpiModPath;
      obj->scope = vpip_peek_current_scope ( );

      initialize_path_term(obj->path_term_out);
      obj->உள்ளீடு_net = net ;

#ifdef CHECK_WITH_VALGRIND
      mp_மொத்தம் += 1;
      mp_list = static_cast<கட்டமைப்பு __vpiModPath **>
                (realloc(mp_list,
                 mp_மொத்தம்*அளவு(கட்டமைப்பு __vpiModPath **)));
      mp_list[mp_மொத்தம்-1] = obj;
#endif
      திரும்பு obj;
}

#ifdef CHECK_WITH_VALGRIND
வெறுமை modpath_delete()
{
      க்கு (unsigned idx = 0; idx < mp_மொத்தம்; idx += 1) {
	    delete mp_list[idx];
      }
      விடுவி(mp_list);
      mp_list = 0;
      mp_மொத்தம் = 0;
}
#endif

/*
 * This function will construct a vpiModPathIn
 * ( struct __vpiModPathSrc ) Object. will give
 * a delays[12] values, and point to the specified functor
 *
 */

கட்டமைப்பு __vpiModPathSrc* vpip_make_modpath_src(கட்டமைப்பு __vpiModPath*path,
                                              vvp_net_t *net)
{
      கட்டமைப்பு __vpiModPathSrc *obj = புதிய __vpiModPathSrc;

      obj->dest = path;
      obj->type = 0;
      obj->net = net;
      initialize_path_term(obj->path_term_in);

      திரும்பு obj;
}

vvp_fun_intermodpath::vvp_fun_intermodpath(vvp_net_t*net, unsigned width)
: net_(net)
{
      க்கு (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = 0;

      cur_vec4_ = vvp_vector4_t(width, BIT4_X);
      schedule_init_propagate(net_, cur_vec4_);
}

vvp_fun_intermodpath::~vvp_fun_intermodpath()
{
}

வெறுமை vvp_fun_intermodpath::get_delay12(vvp_time64_t val[12]) மாறிலி
{
      க்கு (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    val[idx] = delay_[idx];
}

வெறுமை vvp_fun_intermodpath::put_delay12(மாறிலி vvp_time64_t val[12])
{
      க்கு (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = val[idx];
}

/*
 * FIXME: this implementation currently only uses the maximum delay
 * from all the bit changes in the vectors. If there are multiple
 * changes with different delays, then the results would be
 * wrong. What should happen is that if there are multiple changes,
 * multiple vectors approaching the result should be scheduled.
 */
வெறுமை vvp_fun_intermodpath::recv_vec4(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
                              vvp_context_t)
{
	/* Only the first port is used. */
      எனில் (port.port() > 0)
	    திரும்பு;

      எனில் (cur_vec4_.eeq(bit))
	    திரும்பு;

	/* Given the scheduled output time, create an output event. */
      vvp_time64_t use_delay = delay_from_edge(cur_vec4_.value(0),
					       bit.value(0),
					       delay_);

	/* FIXME: This bases the edge delay on only the least
	   bit. This is WRONG! I need to find all the possible delays,
	   and schedule an event for each partial change. Hard! */
      க்கு (unsigned idx = 1 ;  idx < bit.size() ;  idx += 1) {
	    vvp_time64_t tmp = delay_from_edge(cur_vec4_.value(idx),
					       bit.value(idx),
					       delay_);
	      /* If the current and new bit values match then no delay
	       * is needed for this bit. */
	    எனில் (cur_vec4_.value(idx) == bit.value(idx)) தொடர்;
	    ப்ரதிஜ்ஞா(tmp == use_delay);
      }

      cur_vec4_ = bit;
      schedule_generic(இது, use_delay, பொய்);
}

வெறுமை vvp_fun_intermodpath::run_run()
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
நிலையான முழு intermodpath_get(முழு, vpiHandle ref)
{
      கட்டமைப்பு __vpiInterModPath*obj =dynamic_cast<__vpiInterModPath*>(ref);
      ப்ரதிஜ்ஞா(obj);
      திரும்பு 0;
}

நிலையான வெறுமை intermodpath_get_மதிப்பு(vpiHandle ref, p_vpi_மதிப்பு)
{
      கட்டமைப்பு __vpiInterModPath* intermodpath = dynamic_cast<__vpiInterModPath*>(ref);
      ப்ரதிஜ்ஞா(intermodpath);
      திரும்பு;
}

நிலையான vpiHandle intermodpath_put_மதிப்பு(vpiHandle ref, s_vpi_மதிப்பு *, முழு )
{
      கட்டமைப்பு __vpiInterModPath* intermodpath = dynamic_cast<__vpiInterModPath*>(ref);
      ப்ரதிஜ்ஞா(intermodpath);
      திரும்பு 0;
}

நிலையான vpiHandle intermodpath_get_handle(முழு code, vpiHandle ref)
{
      கட்டமைப்பு __vpiInterModPath*rfp = dynamic_cast<__vpiInterModPath*>(ref);
      ப்ரதிஜ்ஞா(rfp);

      தேர்வு (code) {

	நிலை vpiScope:
	  திரும்பு rfp->scope;

	  நிலை vpiModule:
	      { __vpiScope*scope = rfp->scope;
		போது (scope && scope->get_type_code() != vpiModule)
		      scope = scope->scope;
		ப்ரதிஜ்ஞா(scope);
		திரும்பு scope;
	      }
      }
      திரும்பு 0;
}

நிலையான vpiHandle intermodpath_iterate(முழு code, vpiHandle ref)
{
      கட்டமைப்பு __vpiInterModPath*rfp = dynamic_cast<__vpiInterModPath*>(ref);
      ப்ரதிஜ்ஞா(rfp);

	// For now intermodpaths only support exactly two ports
      தேர்வு (code) {
	  நிலை vpiPort: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(2, அளவு(vpiHandle*)));
	    args[0] = rfp->port1;
	    args[1] = rfp->port2;
	    திரும்பு vpip_make_iterator(2, args, உண்மை);
	  }
      }
      திரும்பு 0;
}

/*
 * This routine will put specific dimension of delay[] values
 * into a vpiHandle. In this case, we will put
 * specific delays values in a vpiInterModPath object
 *
 */
நிலையான வெறுமை intermodpath_put_delays (vpiHandle ref, p_vpi_delay delays)
{
      vvp_time64_t tmp[12];
      முழு idx;
      கட்டமைப்பு __vpiInterModPath * src = dynamic_cast<__vpiInterModPath*>(ref) ;
      ப்ரதிஜ்ஞா(src) ;

      vvp_fun_intermodpath *fun = dynamic_cast<vvp_fun_intermodpath*>(src->net->fun);
      ப்ரதிஜ்ஞா( fun );

      வகைவரை unsigned எழுத்து map_array_t[12];
	// Only the first six entries are used for the less than twelve maps.
      நிலையான மாறிலி map_array_t map_1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      நிலையான மாறிலி map_array_t map_2 = {0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0};
      நிலையான மாறிலி map_array_t map_3 = {0, 1, 2, 0, 2, 1, 0, 0, 0, 0, 0, 0};
      நிலையான மாறிலி map_array_t map_6 = {0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0};
      நிலையான மாறிலி map_array_t map12 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

      மாறிலி map_array_t*use_map = 0;
      தேர்வு (delays->no_of_delays) {
	  நிலை 1:
	    use_map = &map_1;
	    நிறுத்து;
	  நிலை 2:
	    use_map = &map_2;
	    நிறுத்து;
	  நிலை 3:
	    use_map = &map_3;
	    நிறுத்து;
	  நிலை 6:
	    use_map = &map_6;
	    நிறுத்து;
	  நிலை 12:
	    use_map = &map12;
	    நிறுத்து;
	  இயல்பு:
	    ப்ரதிஜ்ஞா(0);
	    நிறுத்து;
      }

      எனில் (delays->time_type == vpiSimTime) {
	    க்கு (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_timestruct_to_time(delays->da+use_map[0][idx]);
	    }
      } இல்லையெனில் {
	      // You cannot create a modpath with a negative delay so set it
	      // to zero per 1364-2005 section 14.3.1.
	    க்கு (idx = 0 ; idx < delays->no_of_delays ; idx += 1) {
		  எனில் (delays->da[idx].real < 0.0) delays->da[idx].real = 0.0;
	    }
	    க்கு (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_scaled_real_to_time64(delays->da[use_map[0][idx]].real,
							src->scope);
	    }
      }

      /* Now define the to-from-x delays if needed. */
      எனில் (delays->no_of_delays <= 6) {
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

நிலையான வெறுமை intermodpath_get_delays ( vpiHandle ref, p_vpi_delay delays )
{
      கட்டமைப்பு __vpiInterModPath*src = dynamic_cast<__vpiInterModPath*>(ref) ;
      ப்ரதிஜ்ஞா(src);

      vvp_fun_intermodpath *fun = dynamic_cast<vvp_fun_intermodpath*>(src->net->fun);
      ப்ரதிஜ்ஞா(fun);

      முழு idx;
      vvp_time64_t tmp[12];
      fun->get_delay12(tmp);

      தேர்வு (delays->no_of_delays) {
	  நிலை 1:
	  நிலை 2:
	  நிலை 3:
	  நிலை 6:
	  நிலை 12:
	    நிறுத்து;

	  இயல்பு:
	    ப்ரதிஜ்ஞா(0);
	    நிறுத்து;
      }

      எனில் (delays->time_type == vpiSimTime) {
	    க்கு (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  vpip_time_to_timestruct(delays->da+idx, tmp[idx]);
	    }
      } இல்லையெனில் {
	    க்கு (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  delays->da[idx].real = vpip_time_to_scaled_real(tmp[idx], src->scope);
	    }
      }
}

/*
* The __vpiInterModPath class is what the VPI client sees as a
* vpiInterModPath object.
*/
உள்வரி __vpiInterModPath::__vpiInterModPath()
{ }

முழு __vpiInterModPath::get_type_code(வெறுமை) மாறிலி
{ திரும்பு vpiInterModPath; }

முழு __vpiInterModPath::vpi_get(முழு code)
{ திரும்பு intermodpath_get(code, இது); }

வெறுமை __vpiInterModPath::vpi_get_மதிப்பு(p_vpi_மதிப்பு val)
{ intermodpath_get_மதிப்பு(இது, val); }

vpiHandle __vpiInterModPath::vpi_put_மதிப்பு(p_vpi_மதிப்பு val, முழு flags)
{ திரும்பு intermodpath_put_மதிப்பு(இது, val, flags); }

vpiHandle __vpiInterModPath::vpi_handle(முழு code)
{ திரும்பு intermodpath_get_handle(code, இது); }

vpiHandle __vpiInterModPath::vpi_iterate(முழு code)
{ திரும்பு intermodpath_iterate(code, இது); }

வெறுமை __vpiInterModPath::vpi_get_delays(p_vpi_delay del)
{ intermodpath_get_delays(இது, del); }

வெறுமை __vpiInterModPath::vpi_put_delays(p_vpi_delay del)
{ intermodpath_put_delays(இது, del); }

நிலையான முழு intermodpath_free_object( vpiHandle ref )
{
      delete ref;
      திரும்பு 1 ;
}

__vpiHandle::free_object_fun_t __vpiInterModPath::free_object_fun(வெறுமை)
{ திரும்பு &intermodpath_free_object; }

/*
 * This function will construct a vpiInterModPath Object.
 * give a respective "net", and will point to his
 * respective functor
 */

#ifdef CHECK_WITH_VALGRIND
நிலையான கட்டமைப்பு __vpiInterModPath**imp_list = 0;
நிலையான unsigned imp_மொத்தம் = 0;
#endif

கட்டமைப்பு __vpiInterModPath* vpip_make_intermodpath(vvp_net_t *net, vpiPortInfo* port1, vpiPortInfo* port2)
{
      கட்டமைப்பு __vpiInterModPath*obj = புதிய __vpiInterModPath;
      obj->scope = vpip_peek_current_scope ( );

      obj->net = net;
      obj->port1 = port1;
      obj->port2 = port2;

#ifdef CHECK_WITH_VALGRIND
      imp_மொத்தம் += 1;
      imp_list = static_cast<கட்டமைப்பு __vpiInterModPath **>
                 (realloc(imp_list,
                  imp_மொத்தம்*அளவு(கட்டமைப்பு __vpiInterModPath **)));
      imp_list[imp_மொத்தம்-1] = obj;
#endif
      திரும்பு obj;
}

#ifdef CHECK_WITH_VALGRIND
வெறுமை intermodpath_delete()
{
      க்கு (unsigned idx = 0; idx < mp_மொத்தம்; idx += 1) {
	    delete imp_list[idx];
      }
      விடுவி(imp_list);
      imp_list = 0;
      imp_மொத்தம் = 0;
}
#endif
