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

#شامل "delay.h"
#شامل "schedule.h"
#شامل "vpi_priv.h"
#شامل "config.h"
#ifdef CHECK_WITH_VALGRIND
#شامل "vvp_cleanup.h"
#endif
#شامل <iostream>
#شامل <cstdlib>
#شامل <list>
#شامل <cassert>
#شامل <cmath>
#شامل "ivl_alloc.h"

using فضاینام std;

تهی vvp_delay_t::calculate_min_delay_()
{
      min_delay_ = rise_;
      اگر (fall_ < min_delay_)
	    min_delay_ = fall_;
      اگر (ignore_decay_) decay_ = min_delay_;
      وگرنه اگر (decay_ < min_delay_)
	    min_delay_ = decay_;
}

vvp_delay_t::vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall)
{
      rise_ = rise;
      fall_ = fall;
      decay_= fall < rise? fall : rise;
      min_delay_ = decay_;
      ignore_decay_ = نادرست;
}

vvp_delay_t::vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall, vvp_time64_t decay)
{
      rise_ = rise;
      fall_ = fall;
      decay_= decay;
      ignore_decay_ = نادرست;

      calculate_min_delay_();
}

تهی vvp_delay_t::set_ignore_decay()
{
      ignore_decay_ = درست;

      calculate_min_delay_();
}

vvp_delay_t::~vvp_delay_t()
{
}

vvp_time64_t vvp_delay_t::get_delay(vvp_bit4_t from, vvp_bit4_t to)
{
      انتخاب (from) {
	  حالت BIT4_0:
	    انتخاب (to) {
		حالت BIT4_0: بازگشت 0;
		حالت BIT4_1: بازگشت rise_;
		حالت BIT4_X: بازگشت min_delay_;
		حالت BIT4_Z: بازگشت decay_;
	    }
	    بشکن;
	  حالت BIT4_1:
	    انتخاب (to) {
		حالت BIT4_0: بازگشت fall_;
		حالت BIT4_1: بازگشت 0;
		حالت BIT4_X: بازگشت min_delay_;
		حالت BIT4_Z: بازگشت decay_;
	    }
	    بشکن;
	  حالت BIT4_X:
	    انتخاب (to) {
		حالت BIT4_0: بازگشت fall_;
		حالت BIT4_1: بازگشت rise_;
		حالت BIT4_X: بازگشت 0;
		حالت BIT4_Z: بازگشت decay_;
	    }
	    بشکن;
	  حالت BIT4_Z:
	    انتخاب (to) {
		حالت BIT4_0: بازگشت fall_;
		حالت BIT4_1: بازگشت rise_;
		حالت BIT4_X: بازگشت min_delay_;
		حالت BIT4_Z: بازگشت 0;
	    }
	    بشکن;
      }

      أكد(0);
      بازگشت 0;
}

vvp_time64_t vvp_delay_t::get_min_delay() ثابت
{
      بازگشت min_delay_;
}

تهی vvp_delay_t::set_rise(vvp_time64_t val)
{
      rise_ = val;
      اگر (val < min_delay_) {
	    min_delay_ = val;
	    اگر (ignore_decay_) decay_ = val;
      } وگرنه
	    calculate_min_delay_();
}

تهی vvp_delay_t::set_fall(vvp_time64_t val)
{
      fall_ = val;
      اگر (val < min_delay_) {
	    min_delay_ = val;
	    اگر (ignore_decay_) decay_ = val;
      } وگرنه
	    calculate_min_delay_();
}

تهی vvp_delay_t::set_decay(vvp_time64_t val)
{
      أكد(!ignore_decay_);

      decay_ = val;
      اگر (val < min_delay_)
	    min_delay_ = val;
      وگرنه
	    calculate_min_delay_();
}

vvp_fun_delay::vvp_fun_delay(vvp_net_t*n, unsigned width, ثابت vvp_delay_t&d)
: net_(n), delay_(d)
{
      cur_real_ = 0.0;
      اگر (width > 0) {
            cur_vec4_ = vvp_vector4_t(width, BIT4_X);
            cur_vec8_ = vvp_vector8_t(cur_vec4_, 6, 6);
            schedule_init_propagate(net_, cur_vec4_);
      } وگرنه {
            schedule_init_propagate(net_, cur_real_);
      }
      list_ = 0;
      type_ = UNKNOWN_DELAY;
      initial_ = درست;
	// Calculate the values used when converting variable delays
	// to simulation time units.
      ثابت __vpiScope*scope = vpip_peek_current_scope();

      صحیح powr = scope->time_units - scope->time_precision;
      round_ = 1;
      برای (صحیح lp = 0; lp < powr; lp += 1) round_ *= 10;

      powr = scope->time_precision - vpip_get_time_precision();
      scale_ = 1;
      برای (صحیح lp = 0; lp < powr; lp += 1) scale_ *= 10;
}

vvp_fun_delay::~vvp_fun_delay()
{
      وقتی (ساختار event_*cur = dequeue_())
	    delete cur;
}

منطقی vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        ثابت vvp_vector4_t&bit)
{
      اگر (list_ == 0) بازگشت نادرست;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      اگر (list_->next->ptr_vec4.eeq(bit)) بازگشت درست;

      clean_pulse_events_(use_delay);
      بازگشت نادرست;
}

منطقی vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        ثابت vvp_vector8_t&bit)
{
      اگر (list_ == 0) بازگشت نادرست;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      اگر (list_->next->ptr_vec8.eeq(bit)) بازگشت درست;

      clean_pulse_events_(use_delay);
      بازگشت نادرست;
}

منطقی vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        دوگانه bit)
{
      اگر (list_ == 0) بازگشت نادرست;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      اگر (list_->next->ptr_real == bit) بازگشت درست;

      clean_pulse_events_(use_delay);
      بازگشت نادرست;
}

تهی vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay)
{
      أكد(list_ != 0);

      انجامبده {
	    ساختار event_*cur = list_->next;
	      /* If this event is far enough from the event I'm about
	         to create, then that scheduled event is not a pulse
	         to be eliminated, so we're done. */
	    اگر (cur->sim_time+use_delay <= use_delay+schedule_simtime())
		  بشکن;

	    اگر (list_ == cur)
		  list_ = 0;
	    وگرنه
		  list_->next = cur->next;
	    delete cur;
      } وقتی (list_);
}

/*
 * FIXME: this implementation currently only uses the maximum delay
 * from all the bit changes in the vectors. If there are multiple
 * changes with different delays, then the results would be
 * wrong. What should happen is that if there are multiple changes,
 * multiple vectors approaching the result should be scheduled.
 */
تهی vvp_fun_delay::recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                              vvp_context_t)
{
      اگر (port.port() > 0) {
	      // Get the integer value of the bit vector, or 0 if
	      // there are X or Z bits.
	    vvp_time64_t bval = 0;
	      // The following does not work correctly for negative values.
	      // They should be sign extended to 64 bits (1364-2001 9.7.1).
	    vector4_to_مقدار(bit, bval);
	      // Integer values do not need to be rounded so just scale them.
	    vvp_time64_t val = bval * round_ * scale_;

	    انتخاب (port.port()) {
		حالت 1:
		  delay_.set_rise(val);
		  بازگشت;
		حالت 2:
		  delay_.set_fall(val);
		  بازگشت;
		حالت 3:
		  delay_.set_decay(val);
		  بازگشت;
	    }
	    بازگشت;
      }

      vvp_time64_t use_delay;
	/* This is an initial value so it needs to be compared to all the
	   bits (the order the bits are changed is not deterministic). */
      اگر (initial_) {
	    type_ = VEC4_DELAY;
            cur_vec8_ = vvp_vector8_t(vvp_vector4_t(0, BIT4_X), 6, 6);
	    vvp_bit4_t cur_val = cur_vec4_.value(0);
	    use_delay = delay_.get_delay(cur_val, bit.value(0));
	    برای (unsigned idx = 1 ;  idx < bit.size() ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(cur_val, bit.value(idx));
		  اگر (tmp > use_delay) use_delay = tmp;
	    }
      } وگرنه {
	    أكد(type_ == VEC4_DELAY);

	      // Use as a reference for calculating the delay the
	      // current value of the output. Detect and handle the
	      // special case that the event list contains the current
	      // value as a zero-delay-remaining event.
	    ثابت vvp_vector4_t&use_vec4 = (list_ && list_->next->sim_time == schedule_simtime())? list_->next->ptr_vec4 : cur_vec4_;

	      /* How many bits to compare? */
	    unsigned use_wid = use_vec4.size();
	    اگر (bit.size() < use_wid) use_wid = bit.size();

	      /* Scan the vectors looking for delays. Select the maximum
	         delay encountered. */
	    use_delay = delay_.get_delay(use_vec4.value(0), bit.value(0));

	    برای (unsigned idx = 1 ;  idx < use_wid ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(use_vec4.value(idx), bit.value(idx));
		  اگر (tmp > use_delay) use_delay = tmp;
	    }
      }

      /* what *should* happen here is we check to see if there is a
         transaction in the queue. This would be a pulse that needs to be
         eliminated. */
      اگر (clean_pulse_events_(use_delay, bit)) بازگشت;

      vvp_time64_t use_simtime = schedule_simtime() + use_delay;

	/* And propagate it. */
      اگر (use_delay == 0 && list_ == 0) {
	    cur_vec4_ = bit;
	    initial_ = نادرست;
	    net_->send_vec4(cur_vec4_, 0);
      } وگرنه {
	    ساختار event_*cur = جدید ساختار event_(use_simtime);
	    cur->run_run_ptr = &vvp_fun_delay::run_run_vec4_;
	    cur->ptr_vec4 = bit;
	    enqueue_(cur);
	    schedule_generic(این, use_delay, نادرست);
      }
}

تهی vvp_fun_delay::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
			         unsigned base, unsigned vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

/* See the recv_vec4 comment above. */
تهی vvp_fun_delay::recv_vec8(vvp_net_ptr_t port, ثابت vvp_vector8_t&bit)
{
      أكد(port.port() == 0);

      vvp_time64_t use_delay;
	/* This is an initial value so it needs to be compared to all the
	   bits (the order the bits are changed is not deterministic). */
      اگر (initial_) {
	    type_ = VEC8_DELAY;
            cur_vec4_ = vvp_vector4_t(0, BIT4_X);
	    vvp_bit4_t cur_val = cur_vec8_.value(0).value();
	    use_delay = delay_.get_delay(cur_val, bit.value(0).value());
	    برای (unsigned idx = 1 ;  idx < bit.size() ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(cur_val, bit.value(idx).value());
		  اگر (tmp > use_delay) use_delay = tmp;
	    }
      } وگرنه {
	    أكد(type_ == VEC8_DELAY);

	      // Use as a reference for calculating the delay the
	      // current value of the output. Detect and handle the
	      // special case that the event list contains the current
	      // value as a zero-delay-remaining event.
	    ثابت vvp_vector8_t&use_vec8 = (list_ && list_->next->sim_time == schedule_simtime())? list_->next->ptr_vec8 : cur_vec8_;

	      /* How many bits to compare? */
	    unsigned use_wid = use_vec8.size();
	    اگر (bit.size() < use_wid) use_wid = bit.size();

	      /* Scan the vectors looking for delays. Select the maximum
	         delay encountered. */
	    use_delay = delay_.get_delay(use_vec8.value(0).value(),
	                                 bit.value(0).value());

	    برای (unsigned idx = 1 ;  idx < use_wid ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(use_vec8.value(idx).value(),
		                         bit.value(idx).value());
		  اگر (tmp > use_delay) use_delay = tmp;
	    }
      }

      /* what *should* happen here is we check to see if there is a
         transaction in the queue. This would be a pulse that needs to be
         eliminated. */
      اگر (clean_pulse_events_(use_delay, bit)) بازگشت;

      vvp_time64_t use_simtime = schedule_simtime() + use_delay;

	/* And propagate it. */
      اگر (use_delay == 0 && list_ == 0) {
	    cur_vec8_ = bit;
	    initial_ = نادرست;
	    net_->send_vec8(cur_vec8_);
      } وگرنه {
	    ساختار event_*cur = جدید ساختار event_(use_simtime);
	    cur->ptr_vec8 = bit;
	    cur->run_run_ptr = &vvp_fun_delay::run_run_vec8_;
	    enqueue_(cur);
	    schedule_generic(این, use_delay, نادرست);
      }
}

تهی vvp_fun_delay::recv_vec8_pv(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&bit,
			         unsigned base, unsigned vwid)
{
      recv_vec8_pv_(ptr, bit, base, vwid);
}

تهی vvp_fun_delay::recv_real(vvp_net_ptr_t port, دوگانه bit,
                              vvp_context_t)
{
      اگر (port.port() > 0) {
	    /* If the port is not 0, then this is a delay value that
	    should be rounded and converted to an integer delay. */
	    vvp_time64_t val = 0;
	    اگر (bit > -0.5) {
		  val = (vvp_time64_t) (bit * round_ + 0.5) * scale_;
	    } وگرنه اگر (bit != bit) {
		    // For a NaN we use the default (0).
	    } وگرنه {
		  vvp_vector4_t vec4(8*اندازه(vvp_time64_t),
		                     floor(-bit * round_ + 0.5) * -1 * scale_);
		  vector4_to_مقدار(vec4, val);
	    }

	    انتخاب (port.port()) {
		حالت 1:
		  delay_.set_rise(val);
		  بازگشت;
		حالت 2:
		  delay_.set_fall(val);
		  بازگشت;
		حالت 3:
		  delay_.set_decay(val);
		  بازگشت;
	    }
	    بازگشت;
      }

      اگر (initial_) {
	    type_ = REAL_DELAY;
            cur_vec4_ = vvp_vector4_t(0, BIT4_X);
            cur_vec8_ = vvp_vector8_t(cur_vec4_, 6, 6);
      } وگرنه أكد(type_ == REAL_DELAY);

      vvp_time64_t use_delay;
      use_delay = delay_.get_min_delay();

      /* Eliminate glitches. */
      اگر (clean_pulse_events_(use_delay, bit)) بازگشت;

      /* This must be done after cleaning pulses to avoid propagating
       * an incorrect value. */
      اگر (cur_real_ == bit) بازگشت;

      vvp_time64_t use_simtime = schedule_simtime() + use_delay;

      اگر (use_delay == 0 && list_ == 0) {
	    cur_real_ = bit;
	    initial_ = نادرست;
	    net_->send_real(cur_real_, 0);
      } وگرنه {
	    ساختار event_*cur = جدید ساختار event_(use_simtime);
	    cur->run_run_ptr = &vvp_fun_delay::run_run_real_;
	    cur->ptr_real = bit;
	    enqueue_(cur);

	    schedule_generic(این, use_delay, نادرست);
      }
}

تهی vvp_fun_delay::run_run()
{
      vvp_time64_t sim_time = schedule_simtime();
      اگر (list_ == 0 || list_->next->sim_time > sim_time)
	    بازگشت;

      ساختار event_*cur = dequeue_();
      اگر (cur == 0)
	    بازگشت;

      (این->*(cur->run_run_ptr))(cur);
      initial_ = نادرست;
      delete cur;
}

تهی vvp_fun_delay::run_run_vec4_(ثابت ساختار event_*cur)
{
      cur_vec4_ = cur->ptr_vec4;
      net_->send_vec4(cur_vec4_, 0);
}

تهی vvp_fun_delay::run_run_vec8_(ثابت ساختار vvp_fun_delay::event_*cur)
{
      cur_vec8_ = cur->ptr_vec8;
      net_->send_vec8(cur_vec8_);
}

تهی vvp_fun_delay::run_run_real_(ثابت ساختار vvp_fun_delay::event_*cur)
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
      وقتی (src_list_) {
	    vvp_fun_modpath_src*tmp = src_list_;
	    src_list_ = tmp->next_;
	    delete tmp;
      }
      وقتی (ifnone_list_) {
	    vvp_fun_modpath_src*tmp = ifnone_list_;
	    ifnone_list_ = tmp->next_;
	    delete tmp;
      }
}

تهی vvp_fun_modpath::add_modpath_src(vvp_fun_modpath_src*that, منطقی ifnone)
{
      أكد(that->next_ == 0);
      اگر (ifnone) {
	    that->next_ = ifnone_list_;
	    ifnone_list_ = that;
      } وگرنه {
	    that->next_ = src_list_;
	    src_list_ = that;
      }
}

ایستا vvp_time64_t delay_from_edge(vvp_bit4_t a, vvp_bit4_t b,
                                           vvp_time64_t ثابت array[12])
{
      تعریفنوع delay_edge_t bit4_table4[4];
      ایستا ثابت bit4_table4 edge_table[4] = {
	    { DELAY_EDGE_01, DELAY_EDGE_01, DELAY_EDGE_0z, DELAY_EDGE_0x },
	    { DELAY_EDGE_10, DELAY_EDGE_10, DELAY_EDGE_1z, DELAY_EDGE_1x },
	    { DELAY_EDGE_z0, DELAY_EDGE_z1, DELAY_EDGE_z0, DELAY_EDGE_zx },
	    { DELAY_EDGE_x0, DELAY_EDGE_x1, DELAY_EDGE_xz, DELAY_EDGE_x0 }
      };

      بازگشت array[ edge_table[a][b] ];
}

تهی vvp_fun_modpath::recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                                vvp_context_t)
{
	/* Only the first port is used. */
      اگر (port.port() > 0)
	    بازگشت;

      اگر (cur_vec4_.eeq(bit))
	    بازگشت;

	/* Select a time delay source that applies. Notice that there
	   may be multiple delay sources that apply, so collect all
	   the candidates into a list first. */
      list<vvp_fun_modpath_src*>candidate_list;
      vvp_time64_t candidate_wake_time = 0;
      برای (vvp_fun_modpath_src*cur = src_list_ ;  cur ;  cur=cur->next_) {
	      /* Skip paths that are disabled by conditions. */
	    اگر (cur->condition_flag_ == نادرست)
		  ادامه;

	    اگر (candidate_list.empty()) {
		  candidate_list.push_back(cur);
		  candidate_wake_time = cur->wake_time_;
	    } وگرنه اگر (cur->wake_time_ == candidate_wake_time) {
		  candidate_list.push_back(cur);
	    } وگرنه اگر (cur->wake_time_ > candidate_wake_time) {
		  candidate_list.assign(1, cur);
		  candidate_wake_time = cur->wake_time_;
	    } وگرنه {
		  ادامه; /* Skip this entry. */
	    }
      }

	/* Only add the ifnone delay if it has a later wake_time_ or
	 * if there are no normal delays. */
      vvp_time64_t ifnone_wake_time = candidate_wake_time;
      برای (vvp_fun_modpath_src*cur = ifnone_list_ ;  cur ;  cur=cur->next_) {
	    اگر (candidate_list.empty()) {
		  candidate_list.push_back(cur);
		  ifnone_wake_time = cur->wake_time_;
	    } وگرنه اگر (cur->wake_time_ == ifnone_wake_time &&
	               ifnone_wake_time > candidate_wake_time) {
		  candidate_list.push_back(cur);
	    } وگرنه اگر (cur->wake_time_ > ifnone_wake_time) {
		  candidate_list.assign(1, cur);
		  ifnone_wake_time = cur->wake_time_;
	    } وگرنه {
		  ادامه; /* Skip this entry. */
	    }
      }

	/* Handle the special case that there are no delays that
	   match. This may happen, for example, if the set of
	   conditional delays is incomplete, leaving some cases
	   uncovered. In that case, just pass the data without delay */
      اگر (candidate_list.empty()) {
	    cur_vec4_ = bit;
	    schedule_generic(این, 0, نادرست);
	    بازگشت;
      }

	/* Now given that we have a list of candidate delays, find for
	   each if the 12 numbers the minimum from all the
	   candidates. This minimum set becomes the chosen delay to
	   use. */
      vvp_time64_t out_at[12];
      vvp_time64_t now = schedule_simtime();

      تعریفنوع list<vvp_fun_modpath_src*>::const_iterator iter_t;

      iter_t cur = candidate_list.begin();
      ثابت vvp_fun_modpath_src*src = *cur;

      برای (unsigned idx = 0 ;  idx < 12 ;  idx += 1) {
	    out_at[idx] = src->wake_time_ + src->delay_[idx];
	    اگر (out_at[idx] <= now)
		  out_at[idx] = 0;
	    وگرنه
		  out_at[idx] -= now;
      }

      برای (++ cur ; cur != candidate_list.end() ; ++ cur ) {
	    src = *cur;
	    برای (unsigned idx = 0 ;  idx < 12 ;  idx += 1) {
		  vvp_time64_t tmp = src->wake_time_ + src->delay_[idx];
		  اگر (tmp <= now)
			tmp = 0;
		  وگرنه
			tmp -= now;
		  اگر (tmp < out_at[idx])
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
      برای (unsigned idx = 1 ;  idx < bit.size() ;  idx += 1) {
	    vvp_time64_t tmp = delay_from_edge(cur_vec4_.value(idx),
					       bit.value(idx),
					       out_at);
	      /* If the current and new bit values match then no delay
	       * is needed for this bit. */
	    اگر (cur_vec4_.value(idx) == bit.value(idx)) ادامه;
	    اگر (tmp != use_delay) {
		  fprintf(stderr, "sorry: multi-bit module path delays are "
		                  "currently not fully supported.\n");
		  fprintf(stderr, "     : using the LSB delay for all bits.\n");
		  ادامه;
            }
      }

      cur_vec4_ = bit;
      schedule_generic(این, use_delay, نادرست);
}

تهی vvp_fun_modpath::run_run()
{
      net_->send_vec4(cur_vec4_, 0);
}

vvp_fun_modpath_src::vvp_fun_modpath_src(vvp_time64_t ثابت del[12])
{
      برای (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = del[idx];

      next_ = 0;
      wake_time_ = 0;
      condition_flag_ = درست;
}

vvp_fun_modpath_src::~vvp_fun_modpath_src()
{
}

تهی vvp_fun_modpath_src::get_delay12(vvp_time64_t val[12]) ثابت
{
      برای (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    val[idx] = delay_[idx];
}

تهی vvp_fun_modpath_src::put_delay12(ثابت vvp_time64_t val[12])
{
      برای (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = val[idx];
}

تهی vvp_fun_modpath_src::recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                                    vvp_context_t)
{
      اگر (port.port() == 0) {
	      // The modpath input...
	    اگر (test_vec4(bit))
		  wake_time_ = schedule_simtime();

      } وگرنه اگر (port.port() == 1) {
	      // The modpath condition input...
	    اگر (bit.value(0) != BIT4_0)
		  condition_flag_ = درست;
	    وگرنه
		  condition_flag_ = نادرست;
      }
}

منطقی vvp_fun_modpath_src::test_vec4(ثابت vvp_vector4_t&)
{
      بازگشت درست;
}

vvp_fun_modpath_edge::vvp_fun_modpath_edge(vvp_time64_t del[12],
					   منطقی pos, منطقی neg)
: vvp_fun_modpath_src(del)
{
      old_مقدار = BIT4_X;
      posedge_ = pos;
      negedge_ = neg;
}

منطقی vvp_fun_modpath_edge::test_vec4(ثابت vvp_vector4_t&bit)
{
      vvp_bit4_t tmp = old_مقدار;
      old_مقدار = bit.value(0);

      صحیح edge_flag = edge(tmp, old_مقدار);
      اگر (edge_flag > 0) بازگشت posedge_;
      اگر (edge_flag < 0) بازگشت negedge_;
      بازگشت نادرست;
}


/*
 * All the below routines that begin with
 * modpath_src_* belong the internal function
 * of an vpiModPathIn object. This is used to
 * make some specific delays path operations
 *
 */
ایستا صحیح modpath_src_get(صحیح, vpiHandle ref)
{
      ساختار __vpiModPathSrc*obj =dynamic_cast<__vpiModPathSrc*>(ref);
      أكد(obj);
      بازگشت 0;
}

ایستا تهی modpath_src_get_مقدار(vpiHandle ref, p_vpi_مقدار)
{
      ساختار __vpiModPathSrc* modpathsrc = dynamic_cast<__vpiModPathSrc*>(ref);
      أكد(modpathsrc);
      بازگشت;
}

ایستا vpiHandle modpath_src_put_مقدار(vpiHandle ref, s_vpi_مقدار *, صحیح )
{
      ساختار __vpiModPathSrc* modpathsrc = dynamic_cast<__vpiModPathSrc*>(ref);
      أكد(modpathsrc);
      بازگشت 0;
}

ایستا vpiHandle modpath_src_get_handle(صحیح code, vpiHandle ref)
{
      ساختار __vpiModPathSrc*rfp = dynamic_cast<__vpiModPathSrc*>(ref);
      أكد(rfp);

      انتخاب (code) {

	حالت vpiScope:
	  بازگشت rfp->dest->scope;

	  حالت vpiModule:
	      { __vpiScope*scope = rfp->dest->scope;
		وقتی (scope && scope->get_type_code() != vpiModule)
		      scope = scope->scope;
		أكد(scope);
		بازگشت scope;
	      }

	    // Handles to path term objects should really be obtained via
	    // the vpi_iterate and vpi_scan functions. Continue to allow
	    // them to be obtained here for backwards compatibility with
	    // older versions of Icarus Verilog.

	  حالت vpiModPathIn:
	    بازگشت &rfp->path_term_in;

	  حالت vpiModPathOut:
	    بازگشت &rfp->dest->path_term_out;
      }
      بازگشت 0;
}

ایستا vpiHandle modpath_src_iterate(صحیح code, vpiHandle ref)
{
      ساختار __vpiModPathSrc*rfp = dynamic_cast<__vpiModPathSrc*>(ref);
      أكد(rfp);

	// Module paths with multiple sources or destinations are
	// currently represented by a separate modpath object for
	// each source/destination combination, so there is only
	// ever one input path term and one output path term.
      انتخاب (code) {
	  حالت vpiModPathIn: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(1, اندازه(vpiHandle*)));
	    args[0] = &rfp->path_term_in;
	    بازگشت vpip_make_iterator(1, args, درست);
	  }
	  حالت vpiModPathOut: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(1, اندازه(vpiHandle*)));
	    args[0] = &rfp->dest->path_term_out;
	    بازگشت vpip_make_iterator(1, args, درست);
	  }
      }
      بازگشت 0;
}

ایستا vpiHandle modpath_src_index ( vpiHandle ref, صحیح)
{
      أكد(ref->get_type_code() == vpiModPathIn);
      بازگشت 0;
}


/*
 * This routine will put specific dimension of delay[] values
 * into a vpiHandle. In this case, we will put
 * specific delays values in a vpiModPathIn object
 *
 */
ایستا تهی modpath_src_put_delays (vpiHandle ref, p_vpi_delay delays)
{
      vvp_time64_t tmp[12];
      صحیح idx;
      ساختار __vpiModPathSrc * src = dynamic_cast<__vpiModPathSrc*>(ref) ;
      أكد(src) ;

      vvp_fun_modpath_src *fun = dynamic_cast<vvp_fun_modpath_src*>(src->net->fun);
      أكد( fun );

      تعریفنوع unsigned نویسه map_array_t[12];
	// Only the first six entries are used for the less than twelve maps.
      ایستا ثابت map_array_t map_1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      ایستا ثابت map_array_t map_2 = {0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0};
      ایستا ثابت map_array_t map_3 = {0, 1, 2, 0, 2, 1, 0, 0, 0, 0, 0, 0};
      ایستا ثابت map_array_t map_6 = {0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0};
      ایستا ثابت map_array_t map12 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

      ثابت map_array_t*use_map = 0;
      انتخاب (delays->no_of_delays) {
	  حالت 1:
	    use_map = &map_1;
	    بشکن;
	  حالت 2:
	    use_map = &map_2;
	    بشکن;
	  حالت 3:
	    use_map = &map_3;
	    بشکن;
	  حالت 6:
	    use_map = &map_6;
	    بشکن;
	  حالت 12:
	    use_map = &map12;
	    بشکن;
	  پیشفرض:
	    أكد(0);
	    بشکن;
      }

      اگر (delays->time_type == vpiSimTime) {
	    برای (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_timestruct_to_time(delays->da+use_map[0][idx]);
	    }
      } وگرنه {
	      // You cannot create a modpath with a negative delay so set it
	      // to zero per 1364-2005 section 14.3.1.
	    برای (idx = 0 ; idx < delays->no_of_delays ; idx += 1) {
		  اگر (delays->da[idx].real < 0.0) delays->da[idx].real = 0.0;
	    }
	    برای (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_scaled_real_to_time64(delays->da[use_map[0][idx]].real,
							src->dest->scope);
	    }
      }

      /* Now define the to-from-x delays if needed. */
      اگر (delays->no_of_delays <= 6) {
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

ایستا تهی modpath_src_get_delays ( vpiHandle ref, p_vpi_delay delays )
{
      ساختار __vpiModPathSrc*src = dynamic_cast<__vpiModPathSrc*>(ref) ;
      أكد(src);

      vvp_fun_modpath_src *fun = dynamic_cast<vvp_fun_modpath_src*>(src->net->fun);
      أكد(fun);

      صحیح idx;
      vvp_time64_t tmp[12];
      fun->get_delay12(tmp);

      انتخاب (delays->no_of_delays) {
	  حالت 1:
	  حالت 2:
	  حالت 3:
	  حالت 6:
	  حالت 12:
	    بشکن;

	  پیشفرض:
	    أكد(0);
	    بشکن;
      }

      اگر (delays->time_type == vpiSimTime) {
	    برای (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  vpip_time_to_timestruct(delays->da+idx, tmp[idx]);
	    }
      } وگرنه {
	    برای (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  delays->da[idx].real = vpip_time_to_scaled_real(tmp[idx], src->dest->scope);
	    }
      }
}

ایستا صحیح pathterm_get(صحیح code, vpiHandle ref)
{
      ساختار __vpiModPathTerm*obj = dynamic_cast<__vpiModPathTerm*>(ref);
      أكد(obj);

      انتخاب (code) {
	  حالت vpiEdge:
	    بازگشت obj->edge;
	  پیشفرض:
	    بازگشت 0;
      }
}

ایستا vpiHandle pathterm_get_handle(صحیح code, vpiHandle ref)
{
      ساختار __vpiModPathTerm*obj = dynamic_cast<__vpiModPathTerm*>(ref);
      أكد(obj);

      انتخاب (code) {
	  حالت vpi_عبارت:
	    بازگشت obj->عبارت;
	  پیشفرض:
	    بازگشت 0;
      }
}

/*
* The __vpiModPathSrc class is what the VPI client sees as a
* vpiModPath object. The __vpiModPath structure contains items that
* are common to a bunch of modpaths, including the destination term.
*/
درونخطی __vpiModPathSrc::__vpiModPathSrc()
{ }

صحیح __vpiModPathSrc::get_type_code(تهی) ثابت
{ بازگشت vpiModPath; }

صحیح __vpiModPathSrc::vpi_get(صحیح code)
{ بازگشت modpath_src_get(code, این); }

تهی __vpiModPathSrc::vpi_get_مقدار(p_vpi_مقدار val)
{ modpath_src_get_مقدار(این, val); }

vpiHandle __vpiModPathSrc::vpi_put_مقدار(p_vpi_مقدار val, صحیح flags)
{ بازگشت modpath_src_put_مقدار(این, val, flags); }

vpiHandle __vpiModPathSrc::vpi_handle(صحیح code)
{ بازگشت modpath_src_get_handle(code, این); }

vpiHandle __vpiModPathSrc::vpi_iterate(صحیح code)
{ بازگشت modpath_src_iterate(code, این); }

vpiHandle __vpiModPathSrc:: vpi_index(صحیح idx)
{ بازگشت modpath_src_index(این, idx); }

تهی __vpiModPathSrc::vpi_get_delays(p_vpi_delay del)
{ modpath_src_get_delays(این, del); }

تهی __vpiModPathSrc::vpi_put_delays(p_vpi_delay del)
{ modpath_src_put_delays(این, del); }

ایستا صحیح modpath_src_free_object( vpiHandle ref )
{
      delete ref;
      بازگشت 1 ;
}

__vpiHandle::free_object_fun_t __vpiModPathSrc::free_object_fun(تهی)
{ بازگشت &modpath_src_free_object; }


درونخطی __vpiModPathTerm::__vpiModPathTerm()
{ }

صحیح __vpiModPathTerm::get_type_code(تهی) ثابت
{ بازگشت vpiPathTerm; }

صحیح __vpiModPathTerm::vpi_get(صحیح code)
{ بازگشت pathterm_get(code, این); }

vpiHandle __vpiModPathTerm::vpi_handle(صحیح code)
{ بازگشت pathterm_get_handle(code, این); }

ایستا تهی initialize_path_term(ساختار __vpiModPathTerm&obj)
{
      obj.عبارت = 0;
      obj.edge = vpiNoEdge;
}

/*
 * This function will construct a vpiModPath Object.
 * give a respective "net", and will point to his
 * respective functor
 */

#ifdef CHECK_WITH_VALGRIND
ایستا ساختار __vpiModPath**mp_list = 0;
ایستا unsigned mp_شمار = 0;
#endif

ساختار __vpiModPath* vpip_make_modpath(vvp_net_t *net)
{
      ساختار __vpiModPath*obj = جدید __vpiModPath;
      obj->scope = vpip_peek_current_scope ( );

      initialize_path_term(obj->path_term_out);
      obj->درون_داد_net = net ;

#ifdef CHECK_WITH_VALGRIND
      mp_شمار += 1;
      mp_list = static_cast<ساختار __vpiModPath **>
                (realloc(mp_list,
                 mp_شمار*اندازه(ساختار __vpiModPath **)));
      mp_list[mp_شمار-1] = obj;
#endif
      بازگشت obj;
}

#ifdef CHECK_WITH_VALGRIND
تهی modpath_delete()
{
      برای (unsigned idx = 0; idx < mp_شمار; idx += 1) {
	    delete mp_list[idx];
      }
      آزاد(mp_list);
      mp_list = 0;
      mp_شمار = 0;
}
#endif

/*
 * This function will construct a vpiModPathIn
 * ( struct __vpiModPathSrc ) Object. will give
 * a delays[12] values, and point to the specified functor
 *
 */

ساختار __vpiModPathSrc* vpip_make_modpath_src(ساختار __vpiModPath*path,
                                              vvp_net_t *net)
{
      ساختار __vpiModPathSrc *obj = جدید __vpiModPathSrc;

      obj->dest = path;
      obj->type = 0;
      obj->net = net;
      initialize_path_term(obj->path_term_in);

      بازگشت obj;
}

vvp_fun_intermodpath::vvp_fun_intermodpath(vvp_net_t*net, unsigned width)
: net_(net)
{
      برای (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = 0;

      cur_vec4_ = vvp_vector4_t(width, BIT4_X);
      schedule_init_propagate(net_, cur_vec4_);
}

vvp_fun_intermodpath::~vvp_fun_intermodpath()
{
}

تهی vvp_fun_intermodpath::get_delay12(vvp_time64_t val[12]) ثابت
{
      برای (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    val[idx] = delay_[idx];
}

تهی vvp_fun_intermodpath::put_delay12(ثابت vvp_time64_t val[12])
{
      برای (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = val[idx];
}

/*
 * FIXME: this implementation currently only uses the maximum delay
 * from all the bit changes in the vectors. If there are multiple
 * changes with different delays, then the results would be
 * wrong. What should happen is that if there are multiple changes,
 * multiple vectors approaching the result should be scheduled.
 */
تهی vvp_fun_intermodpath::recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                              vvp_context_t)
{
	/* Only the first port is used. */
      اگر (port.port() > 0)
	    بازگشت;

      اگر (cur_vec4_.eeq(bit))
	    بازگشت;

	/* Given the scheduled output time, create an output event. */
      vvp_time64_t use_delay = delay_from_edge(cur_vec4_.value(0),
					       bit.value(0),
					       delay_);

	/* FIXME: This bases the edge delay on only the least
	   bit. This is WRONG! I need to find all the possible delays,
	   and schedule an event for each partial change. Hard! */
      برای (unsigned idx = 1 ;  idx < bit.size() ;  idx += 1) {
	    vvp_time64_t tmp = delay_from_edge(cur_vec4_.value(idx),
					       bit.value(idx),
					       delay_);
	      /* If the current and new bit values match then no delay
	       * is needed for this bit. */
	    اگر (cur_vec4_.value(idx) == bit.value(idx)) ادامه;
	    أكد(tmp == use_delay);
      }

      cur_vec4_ = bit;
      schedule_generic(این, use_delay, نادرست);
}

تهی vvp_fun_intermodpath::run_run()
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
ایستا صحیح intermodpath_get(صحیح, vpiHandle ref)
{
      ساختار __vpiInterModPath*obj =dynamic_cast<__vpiInterModPath*>(ref);
      أكد(obj);
      بازگشت 0;
}

ایستا تهی intermodpath_get_مقدار(vpiHandle ref, p_vpi_مقدار)
{
      ساختار __vpiInterModPath* intermodpath = dynamic_cast<__vpiInterModPath*>(ref);
      أكد(intermodpath);
      بازگشت;
}

ایستا vpiHandle intermodpath_put_مقدار(vpiHandle ref, s_vpi_مقدار *, صحیح )
{
      ساختار __vpiInterModPath* intermodpath = dynamic_cast<__vpiInterModPath*>(ref);
      أكد(intermodpath);
      بازگشت 0;
}

ایستا vpiHandle intermodpath_get_handle(صحیح code, vpiHandle ref)
{
      ساختار __vpiInterModPath*rfp = dynamic_cast<__vpiInterModPath*>(ref);
      أكد(rfp);

      انتخاب (code) {

	حالت vpiScope:
	  بازگشت rfp->scope;

	  حالت vpiModule:
	      { __vpiScope*scope = rfp->scope;
		وقتی (scope && scope->get_type_code() != vpiModule)
		      scope = scope->scope;
		أكد(scope);
		بازگشت scope;
	      }
      }
      بازگشت 0;
}

ایستا vpiHandle intermodpath_iterate(صحیح code, vpiHandle ref)
{
      ساختار __vpiInterModPath*rfp = dynamic_cast<__vpiInterModPath*>(ref);
      أكد(rfp);

	// For now intermodpaths only support exactly two ports
      انتخاب (code) {
	  حالت vpiPort: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(2, اندازه(vpiHandle*)));
	    args[0] = rfp->port1;
	    args[1] = rfp->port2;
	    بازگشت vpip_make_iterator(2, args, درست);
	  }
      }
      بازگشت 0;
}

/*
 * This routine will put specific dimension of delay[] values
 * into a vpiHandle. In this case, we will put
 * specific delays values in a vpiInterModPath object
 *
 */
ایستا تهی intermodpath_put_delays (vpiHandle ref, p_vpi_delay delays)
{
      vvp_time64_t tmp[12];
      صحیح idx;
      ساختار __vpiInterModPath * src = dynamic_cast<__vpiInterModPath*>(ref) ;
      أكد(src) ;

      vvp_fun_intermodpath *fun = dynamic_cast<vvp_fun_intermodpath*>(src->net->fun);
      أكد( fun );

      تعریفنوع unsigned نویسه map_array_t[12];
	// Only the first six entries are used for the less than twelve maps.
      ایستا ثابت map_array_t map_1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      ایستا ثابت map_array_t map_2 = {0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0};
      ایستا ثابت map_array_t map_3 = {0, 1, 2, 0, 2, 1, 0, 0, 0, 0, 0, 0};
      ایستا ثابت map_array_t map_6 = {0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0};
      ایستا ثابت map_array_t map12 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

      ثابت map_array_t*use_map = 0;
      انتخاب (delays->no_of_delays) {
	  حالت 1:
	    use_map = &map_1;
	    بشکن;
	  حالت 2:
	    use_map = &map_2;
	    بشکن;
	  حالت 3:
	    use_map = &map_3;
	    بشکن;
	  حالت 6:
	    use_map = &map_6;
	    بشکن;
	  حالت 12:
	    use_map = &map12;
	    بشکن;
	  پیشفرض:
	    أكد(0);
	    بشکن;
      }

      اگر (delays->time_type == vpiSimTime) {
	    برای (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_timestruct_to_time(delays->da+use_map[0][idx]);
	    }
      } وگرنه {
	      // You cannot create a modpath with a negative delay so set it
	      // to zero per 1364-2005 section 14.3.1.
	    برای (idx = 0 ; idx < delays->no_of_delays ; idx += 1) {
		  اگر (delays->da[idx].real < 0.0) delays->da[idx].real = 0.0;
	    }
	    برای (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_scaled_real_to_time64(delays->da[use_map[0][idx]].real,
							src->scope);
	    }
      }

      /* Now define the to-from-x delays if needed. */
      اگر (delays->no_of_delays <= 6) {
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

ایستا تهی intermodpath_get_delays ( vpiHandle ref, p_vpi_delay delays )
{
      ساختار __vpiInterModPath*src = dynamic_cast<__vpiInterModPath*>(ref) ;
      أكد(src);

      vvp_fun_intermodpath *fun = dynamic_cast<vvp_fun_intermodpath*>(src->net->fun);
      أكد(fun);

      صحیح idx;
      vvp_time64_t tmp[12];
      fun->get_delay12(tmp);

      انتخاب (delays->no_of_delays) {
	  حالت 1:
	  حالت 2:
	  حالت 3:
	  حالت 6:
	  حالت 12:
	    بشکن;

	  پیشفرض:
	    أكد(0);
	    بشکن;
      }

      اگر (delays->time_type == vpiSimTime) {
	    برای (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  vpip_time_to_timestruct(delays->da+idx, tmp[idx]);
	    }
      } وگرنه {
	    برای (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  delays->da[idx].real = vpip_time_to_scaled_real(tmp[idx], src->scope);
	    }
      }
}

/*
* The __vpiInterModPath class is what the VPI client sees as a
* vpiInterModPath object.
*/
درونخطی __vpiInterModPath::__vpiInterModPath()
{ }

صحیح __vpiInterModPath::get_type_code(تهی) ثابت
{ بازگشت vpiInterModPath; }

صحیح __vpiInterModPath::vpi_get(صحیح code)
{ بازگشت intermodpath_get(code, این); }

تهی __vpiInterModPath::vpi_get_مقدار(p_vpi_مقدار val)
{ intermodpath_get_مقدار(این, val); }

vpiHandle __vpiInterModPath::vpi_put_مقدار(p_vpi_مقدار val, صحیح flags)
{ بازگشت intermodpath_put_مقدار(این, val, flags); }

vpiHandle __vpiInterModPath::vpi_handle(صحیح code)
{ بازگشت intermodpath_get_handle(code, این); }

vpiHandle __vpiInterModPath::vpi_iterate(صحیح code)
{ بازگشت intermodpath_iterate(code, این); }

تهی __vpiInterModPath::vpi_get_delays(p_vpi_delay del)
{ intermodpath_get_delays(این, del); }

تهی __vpiInterModPath::vpi_put_delays(p_vpi_delay del)
{ intermodpath_put_delays(این, del); }

ایستا صحیح intermodpath_free_object( vpiHandle ref )
{
      delete ref;
      بازگشت 1 ;
}

__vpiHandle::free_object_fun_t __vpiInterModPath::free_object_fun(تهی)
{ بازگشت &intermodpath_free_object; }

/*
 * This function will construct a vpiInterModPath Object.
 * give a respective "net", and will point to his
 * respective functor
 */

#ifdef CHECK_WITH_VALGRIND
ایستا ساختار __vpiInterModPath**imp_list = 0;
ایستا unsigned imp_شمار = 0;
#endif

ساختار __vpiInterModPath* vpip_make_intermodpath(vvp_net_t *net, vpiPortInfo* port1, vpiPortInfo* port2)
{
      ساختار __vpiInterModPath*obj = جدید __vpiInterModPath;
      obj->scope = vpip_peek_current_scope ( );

      obj->net = net;
      obj->port1 = port1;
      obj->port2 = port2;

#ifdef CHECK_WITH_VALGRIND
      imp_شمار += 1;
      imp_list = static_cast<ساختار __vpiInterModPath **>
                 (realloc(imp_list,
                  imp_شمار*اندازه(ساختار __vpiInterModPath **)));
      imp_list[imp_شمار-1] = obj;
#endif
      بازگشت obj;
}

#ifdef CHECK_WITH_VALGRIND
تهی intermodpath_delete()
{
      برای (unsigned idx = 0; idx < mp_شمار; idx += 1) {
	    delete imp_list[idx];
      }
      آزاد(imp_list);
      imp_list = 0;
      imp_شمار = 0;
}
#endif
