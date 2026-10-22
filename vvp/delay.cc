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

#অন্তর্ভাব "delay.h"
#অন্তর্ভাব "schedule.h"
#অন্তর্ভাব "vpi_priv.h"
#অন্তর্ভাব "config.h"
#ifdef CHECK_WITH_VALGRIND
#অন্তর্ভাব "vvp_cleanup.h"
#endif
#অন্তর্ভাব <iostream>
#অন্তর্ভাব <cstdlib>
#অন্তর্ভাব <list>
#অন্তর্ভাব <cassert>
#অন্তর্ভাব <cmath>
#অন্তর্ভাব "ivl_alloc.h"

using নামাকাশ std;

শূন্য vvp_delay_t::calculate_min_delay_()
{
      min_delay_ = rise_;
      যদি (fall_ < min_delay_)
	    min_delay_ = fall_;
      যদি (ignore_decay_) decay_ = min_delay_;
      অন্যথা যদি (decay_ < min_delay_)
	    min_delay_ = decay_;
}

vvp_delay_t::vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall)
{
      rise_ = rise;
      fall_ = fall;
      decay_= fall < rise? fall : rise;
      min_delay_ = decay_;
      ignore_decay_ = মিথ্যা;
}

vvp_delay_t::vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall, vvp_time64_t decay)
{
      rise_ = rise;
      fall_ = fall;
      decay_= decay;
      ignore_decay_ = মিথ্যা;

      calculate_min_delay_();
}

শূন্য vvp_delay_t::set_ignore_decay()
{
      ignore_decay_ = সত্য;

      calculate_min_delay_();
}

vvp_delay_t::~vvp_delay_t()
{
}

vvp_time64_t vvp_delay_t::get_delay(vvp_bit4_t from, vvp_bit4_t to)
{
      নির্বাচন (from) {
	  ক্ষেত্র BIT4_0:
	    নির্বাচন (to) {
		ক্ষেত্র BIT4_0: ফেরত 0;
		ক্ষেত্র BIT4_1: ফেরত rise_;
		ক্ষেত্র BIT4_X: ফেরত min_delay_;
		ক্ষেত্র BIT4_Z: ফেরত decay_;
	    }
	    বিরতি;
	  ক্ষেত্র BIT4_1:
	    নির্বাচন (to) {
		ক্ষেত্র BIT4_0: ফেরত fall_;
		ক্ষেত্র BIT4_1: ফেরত 0;
		ক্ষেত্র BIT4_X: ফেরত min_delay_;
		ক্ষেত্র BIT4_Z: ফেরত decay_;
	    }
	    বিরতি;
	  ক্ষেত্র BIT4_X:
	    নির্বাচন (to) {
		ক্ষেত্র BIT4_0: ফেরত fall_;
		ক্ষেত্র BIT4_1: ফেরত rise_;
		ক্ষেত্র BIT4_X: ফেরত 0;
		ক্ষেত্র BIT4_Z: ফেরত decay_;
	    }
	    বিরতি;
	  ক্ষেত্র BIT4_Z:
	    নির্বাচন (to) {
		ক্ষেত্র BIT4_0: ফেরত fall_;
		ক্ষেত্র BIT4_1: ফেরত rise_;
		ক্ষেত্র BIT4_X: ফেরত min_delay_;
		ক্ষেত্র BIT4_Z: ফেরত 0;
	    }
	    বিরতি;
      }

      প্রতিজ্ঞা(0);
      ফেরত 0;
}

vvp_time64_t vvp_delay_t::get_min_delay() ধ্রুবক
{
      ফেরত min_delay_;
}

শূন্য vvp_delay_t::set_rise(vvp_time64_t val)
{
      rise_ = val;
      যদি (val < min_delay_) {
	    min_delay_ = val;
	    যদি (ignore_decay_) decay_ = val;
      } অন্যথা
	    calculate_min_delay_();
}

শূন্য vvp_delay_t::set_fall(vvp_time64_t val)
{
      fall_ = val;
      যদি (val < min_delay_) {
	    min_delay_ = val;
	    যদি (ignore_decay_) decay_ = val;
      } অন্যথা
	    calculate_min_delay_();
}

শূন্য vvp_delay_t::set_decay(vvp_time64_t val)
{
      প্রতিজ্ঞা(!ignore_decay_);

      decay_ = val;
      যদি (val < min_delay_)
	    min_delay_ = val;
      অন্যথা
	    calculate_min_delay_();
}

vvp_fun_delay::vvp_fun_delay(vvp_net_t*n, unsigned width, ধ্রুবক vvp_delay_t&d)
: net_(n), delay_(d)
{
      cur_real_ = 0.0;
      যদি (width > 0) {
            cur_vec4_ = vvp_vector4_t(width, BIT4_X);
            cur_vec8_ = vvp_vector8_t(cur_vec4_, 6, 6);
            schedule_init_propagate(net_, cur_vec4_);
      } অন্যথা {
            schedule_init_propagate(net_, cur_real_);
      }
      list_ = 0;
      type_ = UNKNOWN_DELAY;
      initial_ = সত্য;
	// Calculate the values used when converting variable delays
	// to simulation time units.
      ধ্রুবক __vpiScope*scope = vpip_peek_current_scope();

      পূর্ণসংখ্যা powr = scope->time_units - scope->time_precision;
      round_ = 1;
      জন্য (পূর্ণসংখ্যা lp = 0; lp < powr; lp += 1) round_ *= 10;

      powr = scope->time_precision - vpip_get_time_precision();
      scale_ = 1;
      জন্য (পূর্ণসংখ্যা lp = 0; lp < powr; lp += 1) scale_ *= 10;
}

vvp_fun_delay::~vvp_fun_delay()
{
      যখন (কাঠামো event_*cur = dequeue_())
	    delete cur;
}

যৌক্তিক vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        ধ্রুবক vvp_vector4_t&bit)
{
      যদি (list_ == 0) ফেরত মিথ্যা;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      যদি (list_->পরবর্তী_2->ptr_vec4.eeq(bit)) ফেরত সত্য;

      clean_pulse_events_(use_delay);
      ফেরত মিথ্যা;
}

যৌক্তিক vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        ধ্রুবক vvp_vector8_t&bit)
{
      যদি (list_ == 0) ফেরত মিথ্যা;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      যদি (list_->পরবর্তী_2->ptr_vec8.eeq(bit)) ফেরত সত্য;

      clean_pulse_events_(use_delay);
      ফেরত মিথ্যা;
}

যৌক্তিক vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        দ্বিগুণ bit)
{
      যদি (list_ == 0) ফেরত মিথ্যা;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      যদি (list_->পরবর্তী_2->ptr_real == bit) ফেরত সত্য;

      clean_pulse_events_(use_delay);
      ফেরত মিথ্যা;
}

শূন্য vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay)
{
      প্রতিজ্ঞা(list_ != 0);

      করো {
	    কাঠামো event_*cur = list_->পরবর্তী_2;
	      /* If this event is far enough from the event I'm about
	         to create, then that scheduled event is not a pulse
	         to be eliminated, so we're done. */
	    যদি (cur->sim_time+use_delay <= use_delay+schedule_simtime())
		  বিরতি;

	    যদি (list_ == cur)
		  list_ = 0;
	    অন্যথা
		  list_->পরবর্তী_2 = cur->পরবর্তী_2;
	    delete cur;
      } যখন (list_);
}

/*
 * FIXME: this implementation currently only uses the maximum delay
 * from all the bit changes in the vectors. If there are multiple
 * changes with different delays, then the results would be
 * wrong. What should happen is that if there are multiple changes,
 * multiple vectors approaching the result should be scheduled.
 */
শূন্য vvp_fun_delay::recv_vec4(vvp_net_ptr_t port, ধ্রুবক vvp_vector4_t&bit,
                              vvp_context_t)
{
      যদি (port.port() > 0) {
	      // Get the integer value of the bit vector, or 0 if
	      // there are X or Z bits.
	    vvp_time64_t bval = 0;
	      // The following does not work correctly for negative values.
	      // They should be sign extended to 64 bits (1364-2001 9.7.1).
	    vector4_to_মান(bit, bval);
	      // Integer values do not need to be rounded so just scale them.
	    vvp_time64_t val = bval * round_ * scale_;

	    নির্বাচন (port.port()) {
		ক্ষেত্র 1:
		  delay_.set_rise(val);
		  ফেরত;
		ক্ষেত্র 2:
		  delay_.set_fall(val);
		  ফেরত;
		ক্ষেত্র 3:
		  delay_.set_decay(val);
		  ফেরত;
	    }
	    ফেরত;
      }

      vvp_time64_t use_delay;
	/* This is an initial value so it needs to be compared to all the
	   bits (the order the bits are changed is not deterministic). */
      যদি (initial_) {
	    type_ = VEC4_DELAY;
            cur_vec8_ = vvp_vector8_t(vvp_vector4_t(0, BIT4_X), 6, 6);
	    vvp_bit4_t cur_val = cur_vec4_.value(0);
	    use_delay = delay_.get_delay(cur_val, bit.value(0));
	    জন্য (unsigned idx = 1 ;  idx < bit.size() ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(cur_val, bit.value(idx));
		  যদি (tmp > use_delay) use_delay = tmp;
	    }
      } অন্যথা {
	    প্রতিজ্ঞা(type_ == VEC4_DELAY);

	      // Use as a reference for calculating the delay the
	      // current value of the output. Detect and handle the
	      // special case that the event list contains the current
	      // value as a zero-delay-remaining event.
	    ধ্রুবক vvp_vector4_t&use_vec4 = (list_ && list_->পরবর্তী_2->sim_time == schedule_simtime())? list_->পরবর্তী_2->ptr_vec4 : cur_vec4_;

	      /* How many bits to compare? */
	    unsigned use_wid = use_vec4.size();
	    যদি (bit.size() < use_wid) use_wid = bit.size();

	      /* Scan the vectors looking for delays. Select the maximum
	         delay encountered. */
	    use_delay = delay_.get_delay(use_vec4.value(0), bit.value(0));

	    জন্য (unsigned idx = 1 ;  idx < use_wid ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(use_vec4.value(idx), bit.value(idx));
		  যদি (tmp > use_delay) use_delay = tmp;
	    }
      }

      /* what *should* happen here is we check to see if there is a
         transaction in the queue. This would be a pulse that needs to be
         eliminated. */
      যদি (clean_pulse_events_(use_delay, bit)) ফেরত;

      vvp_time64_t use_simtime = schedule_simtime() + use_delay;

	/* And propagate it. */
      যদি (use_delay == 0 && list_ == 0) {
	    cur_vec4_ = bit;
	    initial_ = মিথ্যা;
	    net_->send_vec4(cur_vec4_, 0);
      } অন্যথা {
	    কাঠামো event_*cur = নতুন কাঠামো event_(use_simtime);
	    cur->run_run_ptr = &vvp_fun_delay::run_run_vec4_;
	    cur->ptr_vec4 = bit;
	    enqueue_(cur);
	    schedule_generic(এই, use_delay, মিথ্যা);
      }
}

শূন্য vvp_fun_delay::recv_vec4_pv(vvp_net_ptr_t ptr, ধ্রুবক vvp_vector4_t&bit,
			         unsigned base, unsigned vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

/* See the recv_vec4 comment above. */
শূন্য vvp_fun_delay::recv_vec8(vvp_net_ptr_t port, ধ্রুবক vvp_vector8_t&bit)
{
      প্রতিজ্ঞা(port.port() == 0);

      vvp_time64_t use_delay;
	/* This is an initial value so it needs to be compared to all the
	   bits (the order the bits are changed is not deterministic). */
      যদি (initial_) {
	    type_ = VEC8_DELAY;
            cur_vec4_ = vvp_vector4_t(0, BIT4_X);
	    vvp_bit4_t cur_val = cur_vec8_.value(0).value();
	    use_delay = delay_.get_delay(cur_val, bit.value(0).value());
	    জন্য (unsigned idx = 1 ;  idx < bit.size() ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(cur_val, bit.value(idx).value());
		  যদি (tmp > use_delay) use_delay = tmp;
	    }
      } অন্যথা {
	    প্রতিজ্ঞা(type_ == VEC8_DELAY);

	      // Use as a reference for calculating the delay the
	      // current value of the output. Detect and handle the
	      // special case that the event list contains the current
	      // value as a zero-delay-remaining event.
	    ধ্রুবক vvp_vector8_t&use_vec8 = (list_ && list_->পরবর্তী_2->sim_time == schedule_simtime())? list_->পরবর্তী_2->ptr_vec8 : cur_vec8_;

	      /* How many bits to compare? */
	    unsigned use_wid = use_vec8.size();
	    যদি (bit.size() < use_wid) use_wid = bit.size();

	      /* Scan the vectors looking for delays. Select the maximum
	         delay encountered. */
	    use_delay = delay_.get_delay(use_vec8.value(0).value(),
	                                 bit.value(0).value());

	    জন্য (unsigned idx = 1 ;  idx < use_wid ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(use_vec8.value(idx).value(),
		                         bit.value(idx).value());
		  যদি (tmp > use_delay) use_delay = tmp;
	    }
      }

      /* what *should* happen here is we check to see if there is a
         transaction in the queue. This would be a pulse that needs to be
         eliminated. */
      যদি (clean_pulse_events_(use_delay, bit)) ফেরত;

      vvp_time64_t use_simtime = schedule_simtime() + use_delay;

	/* And propagate it. */
      যদি (use_delay == 0 && list_ == 0) {
	    cur_vec8_ = bit;
	    initial_ = মিথ্যা;
	    net_->send_vec8(cur_vec8_);
      } অন্যথা {
	    কাঠামো event_*cur = নতুন কাঠামো event_(use_simtime);
	    cur->ptr_vec8 = bit;
	    cur->run_run_ptr = &vvp_fun_delay::run_run_vec8_;
	    enqueue_(cur);
	    schedule_generic(এই, use_delay, মিথ্যা);
      }
}

শূন্য vvp_fun_delay::recv_vec8_pv(vvp_net_ptr_t ptr, ধ্রুবক vvp_vector8_t&bit,
			         unsigned base, unsigned vwid)
{
      recv_vec8_pv_(ptr, bit, base, vwid);
}

শূন্য vvp_fun_delay::recv_real(vvp_net_ptr_t port, দ্বিগুণ bit,
                              vvp_context_t)
{
      যদি (port.port() > 0) {
	    /* If the port is not 0, then this is a delay value that
	    should be rounded and converted to an integer delay. */
	    vvp_time64_t val = 0;
	    যদি (bit > -0.5) {
		  val = (vvp_time64_t) (bit * round_ + 0.5) * scale_;
	    } অন্যথা যদি (bit != bit) {
		    // For a NaN we use the default (0).
	    } অন্যথা {
		  vvp_vector4_t vec4(8*আকার(vvp_time64_t),
		                     floor(-bit * round_ + 0.5) * -1 * scale_);
		  vector4_to_মান(vec4, val);
	    }

	    নির্বাচন (port.port()) {
		ক্ষেত্র 1:
		  delay_.set_rise(val);
		  ফেরত;
		ক্ষেত্র 2:
		  delay_.set_fall(val);
		  ফেরত;
		ক্ষেত্র 3:
		  delay_.set_decay(val);
		  ফেরত;
	    }
	    ফেরত;
      }

      যদি (initial_) {
	    type_ = REAL_DELAY;
            cur_vec4_ = vvp_vector4_t(0, BIT4_X);
            cur_vec8_ = vvp_vector8_t(cur_vec4_, 6, 6);
      } অন্যথা প্রতিজ্ঞা(type_ == REAL_DELAY);

      vvp_time64_t use_delay;
      use_delay = delay_.get_min_delay();

      /* Eliminate glitches. */
      যদি (clean_pulse_events_(use_delay, bit)) ফেরত;

      /* This must be done after cleaning pulses to avoid propagating
       * an incorrect value. */
      যদি (cur_real_ == bit) ফেরত;

      vvp_time64_t use_simtime = schedule_simtime() + use_delay;

      যদি (use_delay == 0 && list_ == 0) {
	    cur_real_ = bit;
	    initial_ = মিথ্যা;
	    net_->send_real(cur_real_, 0);
      } অন্যথা {
	    কাঠামো event_*cur = নতুন কাঠামো event_(use_simtime);
	    cur->run_run_ptr = &vvp_fun_delay::run_run_real_;
	    cur->ptr_real = bit;
	    enqueue_(cur);

	    schedule_generic(এই, use_delay, মিথ্যা);
      }
}

শূন্য vvp_fun_delay::run_run()
{
      vvp_time64_t sim_time = schedule_simtime();
      যদি (list_ == 0 || list_->পরবর্তী_2->sim_time > sim_time)
	    ফেরত;

      কাঠামো event_*cur = dequeue_();
      যদি (cur == 0)
	    ফেরত;

      (এই->*(cur->run_run_ptr))(cur);
      initial_ = মিথ্যা;
      delete cur;
}

শূন্য vvp_fun_delay::run_run_vec4_(ধ্রুবক কাঠামো event_*cur)
{
      cur_vec4_ = cur->ptr_vec4;
      net_->send_vec4(cur_vec4_, 0);
}

শূন্য vvp_fun_delay::run_run_vec8_(ধ্রুবক কাঠামো vvp_fun_delay::event_*cur)
{
      cur_vec8_ = cur->ptr_vec8;
      net_->send_vec8(cur_vec8_);
}

শূন্য vvp_fun_delay::run_run_real_(ধ্রুবক কাঠামো vvp_fun_delay::event_*cur)
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
      যখন (src_list_) {
	    vvp_fun_modpath_src*tmp = src_list_;
	    src_list_ = tmp->পরবর্তী;
	    delete tmp;
      }
      যখন (ifnone_list_) {
	    vvp_fun_modpath_src*tmp = ifnone_list_;
	    ifnone_list_ = tmp->পরবর্তী;
	    delete tmp;
      }
}

শূন্য vvp_fun_modpath::add_modpath_src(vvp_fun_modpath_src*that, যৌক্তিক ifnone)
{
      প্রতিজ্ঞা(that->পরবর্তী == 0);
      যদি (ifnone) {
	    that->পরবর্তী = ifnone_list_;
	    ifnone_list_ = that;
      } অন্যথা {
	    that->পরবর্তী = src_list_;
	    src_list_ = that;
      }
}

স্থির vvp_time64_t delay_from_edge(vvp_bit4_t a, vvp_bit4_t b,
                                           vvp_time64_t ধ্রুবক array[12])
{
      প্রকারলেখ delay_edge_t bit4_table4[4];
      স্থির ধ্রুবক bit4_table4 edge_table[4] = {
	    { DELAY_EDGE_01, DELAY_EDGE_01, DELAY_EDGE_0z, DELAY_EDGE_0x },
	    { DELAY_EDGE_10, DELAY_EDGE_10, DELAY_EDGE_1z, DELAY_EDGE_1x },
	    { DELAY_EDGE_z0, DELAY_EDGE_z1, DELAY_EDGE_z0, DELAY_EDGE_zx },
	    { DELAY_EDGE_x0, DELAY_EDGE_x1, DELAY_EDGE_xz, DELAY_EDGE_x0 }
      };

      ফেরত array[ edge_table[a][b] ];
}

শূন্য vvp_fun_modpath::recv_vec4(vvp_net_ptr_t port, ধ্রুবক vvp_vector4_t&bit,
                                vvp_context_t)
{
	/* Only the first port is used. */
      যদি (port.port() > 0)
	    ফেরত;

      যদি (cur_vec4_.eeq(bit))
	    ফেরত;

	/* Select a time delay source that applies. Notice that there
	   may be multiple delay sources that apply, so collect all
	   the candidates into a list first. */
      list<vvp_fun_modpath_src*>candidate_list;
      vvp_time64_t candidate_wake_time = 0;
      জন্য (vvp_fun_modpath_src*cur = src_list_ ;  cur ;  cur=cur->পরবর্তী) {
	      /* Skip paths that are disabled by conditions. */
	    যদি (cur->condition_flag_ == মিথ্যা)
		  চালু;

	    যদি (candidate_list.empty()) {
		  candidate_list.push_back(cur);
		  candidate_wake_time = cur->wake_time_;
	    } অন্যথা যদি (cur->wake_time_ == candidate_wake_time) {
		  candidate_list.push_back(cur);
	    } অন্যথা যদি (cur->wake_time_ > candidate_wake_time) {
		  candidate_list.assign(1, cur);
		  candidate_wake_time = cur->wake_time_;
	    } অন্যথা {
		  চালু; /* Skip this entry. */
	    }
      }

	/* Only add the ifnone delay if it has a later wake_time_ or
	 * if there are no normal delays. */
      vvp_time64_t ifnone_wake_time = candidate_wake_time;
      জন্য (vvp_fun_modpath_src*cur = ifnone_list_ ;  cur ;  cur=cur->পরবর্তী) {
	    যদি (candidate_list.empty()) {
		  candidate_list.push_back(cur);
		  ifnone_wake_time = cur->wake_time_;
	    } অন্যথা যদি (cur->wake_time_ == ifnone_wake_time &&
	               ifnone_wake_time > candidate_wake_time) {
		  candidate_list.push_back(cur);
	    } অন্যথা যদি (cur->wake_time_ > ifnone_wake_time) {
		  candidate_list.assign(1, cur);
		  ifnone_wake_time = cur->wake_time_;
	    } অন্যথা {
		  চালু; /* Skip this entry. */
	    }
      }

	/* Handle the special case that there are no delays that
	   match. This may happen, for example, if the set of
	   conditional delays is incomplete, leaving some cases
	   uncovered. In that case, just pass the data without delay */
      যদি (candidate_list.empty()) {
	    cur_vec4_ = bit;
	    schedule_generic(এই, 0, মিথ্যা);
	    ফেরত;
      }

	/* Now given that we have a list of candidate delays, find for
	   each if the 12 numbers the minimum from all the
	   candidates. This minimum set becomes the chosen delay to
	   use. */
      vvp_time64_t out_at[12];
      vvp_time64_t now = schedule_simtime();

      প্রকারলেখ list<vvp_fun_modpath_src*>::const_iterator iter_t;

      iter_t cur = candidate_list.begin();
      ধ্রুবক vvp_fun_modpath_src*src = *cur;

      জন্য (unsigned idx = 0 ;  idx < 12 ;  idx += 1) {
	    out_at[idx] = src->wake_time_ + src->delay_[idx];
	    যদি (out_at[idx] <= now)
		  out_at[idx] = 0;
	    অন্যথা
		  out_at[idx] -= now;
      }

      জন্য (++ cur ; cur != candidate_list.end() ; ++ cur ) {
	    src = *cur;
	    জন্য (unsigned idx = 0 ;  idx < 12 ;  idx += 1) {
		  vvp_time64_t tmp = src->wake_time_ + src->delay_[idx];
		  যদি (tmp <= now)
			tmp = 0;
		  অন্যথা
			tmp -= now;
		  যদি (tmp < out_at[idx])
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
      জন্য (unsigned idx = 1 ;  idx < bit.size() ;  idx += 1) {
	    vvp_time64_t tmp = delay_from_edge(cur_vec4_.value(idx),
					       bit.value(idx),
					       out_at);
	      /* If the current and new bit values match then no delay
	       * is needed for this bit. */
	    যদি (cur_vec4_.value(idx) == bit.value(idx)) চালু;
	    যদি (tmp != use_delay) {
		  fprintf(stderr, "sorry: multi-bit module path delays are "
		                  "currently not fully supported.\n");
		  fprintf(stderr, "     : using the LSB delay for all bits.\n");
		  চালু;
            }
      }

      cur_vec4_ = bit;
      schedule_generic(এই, use_delay, মিথ্যা);
}

শূন্য vvp_fun_modpath::run_run()
{
      net_->send_vec4(cur_vec4_, 0);
}

vvp_fun_modpath_src::vvp_fun_modpath_src(vvp_time64_t ধ্রুবক del[12])
{
      জন্য (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = del[idx];

      পরবর্তী = 0;
      wake_time_ = 0;
      condition_flag_ = সত্য;
}

vvp_fun_modpath_src::~vvp_fun_modpath_src()
{
}

শূন্য vvp_fun_modpath_src::get_delay12(vvp_time64_t val[12]) ধ্রুবক
{
      জন্য (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    val[idx] = delay_[idx];
}

শূন্য vvp_fun_modpath_src::put_delay12(ধ্রুবক vvp_time64_t val[12])
{
      জন্য (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = val[idx];
}

শূন্য vvp_fun_modpath_src::recv_vec4(vvp_net_ptr_t port, ধ্রুবক vvp_vector4_t&bit,
                                    vvp_context_t)
{
      যদি (port.port() == 0) {
	      // The modpath input...
	    যদি (test_vec4(bit))
		  wake_time_ = schedule_simtime();

      } অন্যথা যদি (port.port() == 1) {
	      // The modpath condition input...
	    যদি (bit.value(0) != BIT4_0)
		  condition_flag_ = সত্য;
	    অন্যথা
		  condition_flag_ = মিথ্যা;
      }
}

যৌক্তিক vvp_fun_modpath_src::test_vec4(ধ্রুবক vvp_vector4_t&)
{
      ফেরত সত্য;
}

vvp_fun_modpath_edge::vvp_fun_modpath_edge(vvp_time64_t del[12],
					   যৌক্তিক pos, যৌক্তিক neg)
: vvp_fun_modpath_src(del)
{
      old_মান = BIT4_X;
      posedge_ = pos;
      negedge_ = neg;
}

যৌক্তিক vvp_fun_modpath_edge::test_vec4(ধ্রুবক vvp_vector4_t&bit)
{
      vvp_bit4_t tmp = old_মান;
      old_মান = bit.value(0);

      পূর্ণসংখ্যা edge_flag = edge(tmp, old_মান);
      যদি (edge_flag > 0) ফেরত posedge_;
      যদি (edge_flag < 0) ফেরত negedge_;
      ফেরত মিথ্যা;
}


/*
 * All the below routines that begin with
 * modpath_src_* belong the internal function
 * of an vpiModPathIn object. This is used to
 * make some specific delays path operations
 *
 */
স্থির পূর্ণসংখ্যা modpath_src_get(পূর্ণসংখ্যা, vpiHandle ref)
{
      কাঠামো __vpiModPathSrc*obj =dynamic_cast<__vpiModPathSrc*>(ref);
      প্রতিজ্ঞা(obj);
      ফেরত 0;
}

স্থির শূন্য modpath_src_get_মান(vpiHandle ref, p_vpi_মান)
{
      কাঠামো __vpiModPathSrc* modpathsrc = dynamic_cast<__vpiModPathSrc*>(ref);
      প্রতিজ্ঞা(modpathsrc);
      ফেরত;
}

স্থির vpiHandle modpath_src_put_মান(vpiHandle ref, s_vpi_মান *, পূর্ণসংখ্যা )
{
      কাঠামো __vpiModPathSrc* modpathsrc = dynamic_cast<__vpiModPathSrc*>(ref);
      প্রতিজ্ঞা(modpathsrc);
      ফেরত 0;
}

স্থির vpiHandle modpath_src_get_handle(পূর্ণসংখ্যা code, vpiHandle ref)
{
      কাঠামো __vpiModPathSrc*rfp = dynamic_cast<__vpiModPathSrc*>(ref);
      প্রতিজ্ঞা(rfp);

      নির্বাচন (code) {

	ক্ষেত্র vpiScope:
	  ফেরত rfp->dest->scope;

	  ক্ষেত্র vpiModule:
	      { __vpiScope*scope = rfp->dest->scope;
		যখন (scope && scope->get_type_code() != vpiModule)
		      scope = scope->scope;
		প্রতিজ্ঞা(scope);
		ফেরত scope;
	      }

	    // Handles to path term objects should really be obtained via
	    // the vpi_iterate and vpi_scan functions. Continue to allow
	    // them to be obtained here for backwards compatibility with
	    // older versions of Icarus Verilog.

	  ক্ষেত্র vpiModPathIn:
	    ফেরত &rfp->path_term_in;

	  ক্ষেত্র vpiModPathOut:
	    ফেরত &rfp->dest->path_term_out;
      }
      ফেরত 0;
}

স্থির vpiHandle modpath_src_iterate(পূর্ণসংখ্যা code, vpiHandle ref)
{
      কাঠামো __vpiModPathSrc*rfp = dynamic_cast<__vpiModPathSrc*>(ref);
      প্রতিজ্ঞা(rfp);

	// Module paths with multiple sources or destinations are
	// currently represented by a separate modpath object for
	// each source/destination combination, so there is only
	// ever one input path term and one output path term.
      নির্বাচন (code) {
	  ক্ষেত্র vpiModPathIn: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(1, আকার(vpiHandle*)));
	    args[0] = &rfp->path_term_in;
	    ফেরত vpip_make_iterator(1, args, সত্য);
	  }
	  ক্ষেত্র vpiModPathOut: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(1, আকার(vpiHandle*)));
	    args[0] = &rfp->dest->path_term_out;
	    ফেরত vpip_make_iterator(1, args, সত্য);
	  }
      }
      ফেরত 0;
}

স্থির vpiHandle modpath_src_সূচক ( vpiHandle ref, পূর্ণসংখ্যা)
{
      প্রতিজ্ঞা(ref->get_type_code() == vpiModPathIn);
      ফেরত 0;
}


/*
 * This routine will put specific dimension of delay[] values
 * into a vpiHandle. In this case, we will put
 * specific delays values in a vpiModPathIn object
 *
 */
স্থির শূন্য modpath_src_put_delays (vpiHandle ref, p_vpi_delay delays)
{
      vvp_time64_t tmp[12];
      পূর্ণসংখ্যা idx;
      কাঠামো __vpiModPathSrc * src = dynamic_cast<__vpiModPathSrc*>(ref) ;
      প্রতিজ্ঞা(src) ;

      vvp_fun_modpath_src *fun = dynamic_cast<vvp_fun_modpath_src*>(src->net->fun);
      প্রতিজ্ঞা( fun );

      প্রকারলেখ unsigned অক্ষর map_array_t[12];
	// Only the first six entries are used for the less than twelve maps.
      স্থির ধ্রুবক map_array_t map_1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      স্থির ধ্রুবক map_array_t map_2 = {0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0};
      স্থির ধ্রুবক map_array_t map_3 = {0, 1, 2, 0, 2, 1, 0, 0, 0, 0, 0, 0};
      স্থির ধ্রুবক map_array_t map_6 = {0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0};
      স্থির ধ্রুবক map_array_t map12 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

      ধ্রুবক map_array_t*use_map = 0;
      নির্বাচন (delays->no_of_delays) {
	  ক্ষেত্র 1:
	    use_map = &map_1;
	    বিরতি;
	  ক্ষেত্র 2:
	    use_map = &map_2;
	    বিরতি;
	  ক্ষেত্র 3:
	    use_map = &map_3;
	    বিরতি;
	  ক্ষেত্র 6:
	    use_map = &map_6;
	    বিরতি;
	  ক্ষেত্র 12:
	    use_map = &map12;
	    বিরতি;
	  পূর্বনির্ধারিত:
	    প্রতিজ্ঞা(0);
	    বিরতি;
      }

      যদি (delays->time_type == vpiSimTime) {
	    জন্য (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_timestruct_to_time(delays->da+use_map[0][idx]);
	    }
      } অন্যথা {
	      // You cannot create a modpath with a negative delay so set it
	      // to zero per 1364-2005 section 14.3.1.
	    জন্য (idx = 0 ; idx < delays->no_of_delays ; idx += 1) {
		  যদি (delays->da[idx].real < 0.0) delays->da[idx].real = 0.0;
	    }
	    জন্য (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_scaled_real_to_time64(delays->da[use_map[0][idx]].real,
							src->dest->scope);
	    }
      }

      /* Now define the to-from-x delays if needed. */
      যদি (delays->no_of_delays <= 6) {
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

স্থির শূন্য modpath_src_get_delays ( vpiHandle ref, p_vpi_delay delays )
{
      কাঠামো __vpiModPathSrc*src = dynamic_cast<__vpiModPathSrc*>(ref) ;
      প্রতিজ্ঞা(src);

      vvp_fun_modpath_src *fun = dynamic_cast<vvp_fun_modpath_src*>(src->net->fun);
      প্রতিজ্ঞা(fun);

      পূর্ণসংখ্যা idx;
      vvp_time64_t tmp[12];
      fun->get_delay12(tmp);

      নির্বাচন (delays->no_of_delays) {
	  ক্ষেত্র 1:
	  ক্ষেত্র 2:
	  ক্ষেত্র 3:
	  ক্ষেত্র 6:
	  ক্ষেত্র 12:
	    বিরতি;

	  পূর্বনির্ধারিত:
	    প্রতিজ্ঞা(0);
	    বিরতি;
      }

      যদি (delays->time_type == vpiSimTime) {
	    জন্য (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  vpip_time_to_timestruct(delays->da+idx, tmp[idx]);
	    }
      } অন্যথা {
	    জন্য (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  delays->da[idx].real = vpip_time_to_scaled_real(tmp[idx], src->dest->scope);
	    }
      }
}

স্থির পূর্ণসংখ্যা pathterm_get(পূর্ণসংখ্যা code, vpiHandle ref)
{
      কাঠামো __vpiModPathTerm*obj = dynamic_cast<__vpiModPathTerm*>(ref);
      প্রতিজ্ঞা(obj);

      নির্বাচন (code) {
	  ক্ষেত্র vpiEdge:
	    ফেরত obj->edge;
	  পূর্বনির্ধারিত:
	    ফেরত 0;
      }
}

স্থির vpiHandle pathterm_get_handle(পূর্ণসংখ্যা code, vpiHandle ref)
{
      কাঠামো __vpiModPathTerm*obj = dynamic_cast<__vpiModPathTerm*>(ref);
      প্রতিজ্ঞা(obj);

      নির্বাচন (code) {
	  ক্ষেত্র vpi_রাশিমালা:
	    ফেরত obj->রাশিমালা;
	  পূর্বনির্ধারিত:
	    ফেরত 0;
      }
}

/*
* The __vpiModPathSrc class is what the VPI client sees as a
* vpiModPath object. The __vpiModPath structure contains items that
* are common to a bunch of modpaths, including the destination term.
*/
অন্তঃরেখ __vpiModPathSrc::__vpiModPathSrc()
{ }

পূর্ণসংখ্যা __vpiModPathSrc::get_type_code(শূন্য) ধ্রুবক
{ ফেরত vpiModPath; }

পূর্ণসংখ্যা __vpiModPathSrc::vpi_get(পূর্ণসংখ্যা code)
{ ফেরত modpath_src_get(code, এই); }

শূন্য __vpiModPathSrc::vpi_get_মান(p_vpi_মান val)
{ modpath_src_get_মান(এই, val); }

vpiHandle __vpiModPathSrc::vpi_put_মান(p_vpi_মান val, পূর্ণসংখ্যা flags)
{ ফেরত modpath_src_put_মান(এই, val, flags); }

vpiHandle __vpiModPathSrc::vpi_handle(পূর্ণসংখ্যা code)
{ ফেরত modpath_src_get_handle(code, এই); }

vpiHandle __vpiModPathSrc::vpi_iterate(পূর্ণসংখ্যা code)
{ ফেরত modpath_src_iterate(code, এই); }

vpiHandle __vpiModPathSrc:: vpi_সূচক(পূর্ণসংখ্যা idx)
{ ফেরত modpath_src_সূচক(এই, idx); }

শূন্য __vpiModPathSrc::vpi_get_delays(p_vpi_delay del)
{ modpath_src_get_delays(এই, del); }

শূন্য __vpiModPathSrc::vpi_put_delays(p_vpi_delay del)
{ modpath_src_put_delays(এই, del); }

স্থির পূর্ণসংখ্যা modpath_src_free_object( vpiHandle ref )
{
      delete ref;
      ফেরত 1 ;
}

__vpiHandle::free_object_fun_t __vpiModPathSrc::free_object_fun(শূন্য)
{ ফেরত &modpath_src_free_object; }


অন্তঃরেখ __vpiModPathTerm::__vpiModPathTerm()
{ }

পূর্ণসংখ্যা __vpiModPathTerm::get_type_code(শূন্য) ধ্রুবক
{ ফেরত vpiPathTerm; }

পূর্ণসংখ্যা __vpiModPathTerm::vpi_get(পূর্ণসংখ্যা code)
{ ফেরত pathterm_get(code, এই); }

vpiHandle __vpiModPathTerm::vpi_handle(পূর্ণসংখ্যা code)
{ ফেরত pathterm_get_handle(code, এই); }

স্থির শূন্য initialize_path_term(কাঠামো __vpiModPathTerm&obj)
{
      obj.রাশিমালা = 0;
      obj.edge = vpiNoEdge;
}

/*
 * This function will construct a vpiModPath Object.
 * give a respective "net", and will point to his
 * respective functor
 */

#ifdef CHECK_WITH_VALGRIND
স্থির কাঠামো __vpiModPath**mp_list = 0;
স্থির unsigned mp_গুনতি = 0;
#endif

কাঠামো __vpiModPath* vpip_make_modpath(vvp_net_t *net)
{
      কাঠামো __vpiModPath*obj = নতুন __vpiModPath;
      obj->scope = vpip_peek_current_scope ( );

      initialize_path_term(obj->path_term_out);
      obj->প্রবেশ_net = net ;

#ifdef CHECK_WITH_VALGRIND
      mp_গুনতি += 1;
      mp_list = static_cast<কাঠামো __vpiModPath **>
                (realloc(mp_list,
                 mp_গুনতি*আকার(কাঠামো __vpiModPath **)));
      mp_list[mp_গুনতি-1] = obj;
#endif
      ফেরত obj;
}

#ifdef CHECK_WITH_VALGRIND
শূন্য modpath_delete()
{
      জন্য (unsigned idx = 0; idx < mp_গুনতি; idx += 1) {
	    delete mp_list[idx];
      }
      মুক্ত(mp_list);
      mp_list = 0;
      mp_গুনতি = 0;
}
#endif

/*
 * This function will construct a vpiModPathIn
 * ( struct __vpiModPathSrc ) Object. will give
 * a delays[12] values, and point to the specified functor
 *
 */

কাঠামো __vpiModPathSrc* vpip_make_modpath_src(কাঠামো __vpiModPath*path,
                                              vvp_net_t *net)
{
      কাঠামো __vpiModPathSrc *obj = নতুন __vpiModPathSrc;

      obj->dest = path;
      obj->type = 0;
      obj->net = net;
      initialize_path_term(obj->path_term_in);

      ফেরত obj;
}

vvp_fun_intermodpath::vvp_fun_intermodpath(vvp_net_t*net, unsigned width)
: net_(net)
{
      জন্য (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = 0;

      cur_vec4_ = vvp_vector4_t(width, BIT4_X);
      schedule_init_propagate(net_, cur_vec4_);
}

vvp_fun_intermodpath::~vvp_fun_intermodpath()
{
}

শূন্য vvp_fun_intermodpath::get_delay12(vvp_time64_t val[12]) ধ্রুবক
{
      জন্য (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    val[idx] = delay_[idx];
}

শূন্য vvp_fun_intermodpath::put_delay12(ধ্রুবক vvp_time64_t val[12])
{
      জন্য (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = val[idx];
}

/*
 * FIXME: this implementation currently only uses the maximum delay
 * from all the bit changes in the vectors. If there are multiple
 * changes with different delays, then the results would be
 * wrong. What should happen is that if there are multiple changes,
 * multiple vectors approaching the result should be scheduled.
 */
শূন্য vvp_fun_intermodpath::recv_vec4(vvp_net_ptr_t port, ধ্রুবক vvp_vector4_t&bit,
                              vvp_context_t)
{
	/* Only the first port is used. */
      যদি (port.port() > 0)
	    ফেরত;

      যদি (cur_vec4_.eeq(bit))
	    ফেরত;

	/* Given the scheduled output time, create an output event. */
      vvp_time64_t use_delay = delay_from_edge(cur_vec4_.value(0),
					       bit.value(0),
					       delay_);

	/* FIXME: This bases the edge delay on only the least
	   bit. This is WRONG! I need to find all the possible delays,
	   and schedule an event for each partial change. Hard! */
      জন্য (unsigned idx = 1 ;  idx < bit.size() ;  idx += 1) {
	    vvp_time64_t tmp = delay_from_edge(cur_vec4_.value(idx),
					       bit.value(idx),
					       delay_);
	      /* If the current and new bit values match then no delay
	       * is needed for this bit. */
	    যদি (cur_vec4_.value(idx) == bit.value(idx)) চালু;
	    প্রতিজ্ঞা(tmp == use_delay);
      }

      cur_vec4_ = bit;
      schedule_generic(এই, use_delay, মিথ্যা);
}

শূন্য vvp_fun_intermodpath::run_run()
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
স্থির পূর্ণসংখ্যা intermodpath_get(পূর্ণসংখ্যা, vpiHandle ref)
{
      কাঠামো __vpiInterModPath*obj =dynamic_cast<__vpiInterModPath*>(ref);
      প্রতিজ্ঞা(obj);
      ফেরত 0;
}

স্থির শূন্য intermodpath_get_মান(vpiHandle ref, p_vpi_মান)
{
      কাঠামো __vpiInterModPath* intermodpath = dynamic_cast<__vpiInterModPath*>(ref);
      প্রতিজ্ঞা(intermodpath);
      ফেরত;
}

স্থির vpiHandle intermodpath_put_মান(vpiHandle ref, s_vpi_মান *, পূর্ণসংখ্যা )
{
      কাঠামো __vpiInterModPath* intermodpath = dynamic_cast<__vpiInterModPath*>(ref);
      প্রতিজ্ঞা(intermodpath);
      ফেরত 0;
}

স্থির vpiHandle intermodpath_get_handle(পূর্ণসংখ্যা code, vpiHandle ref)
{
      কাঠামো __vpiInterModPath*rfp = dynamic_cast<__vpiInterModPath*>(ref);
      প্রতিজ্ঞা(rfp);

      নির্বাচন (code) {

	ক্ষেত্র vpiScope:
	  ফেরত rfp->scope;

	  ক্ষেত্র vpiModule:
	      { __vpiScope*scope = rfp->scope;
		যখন (scope && scope->get_type_code() != vpiModule)
		      scope = scope->scope;
		প্রতিজ্ঞা(scope);
		ফেরত scope;
	      }
      }
      ফেরত 0;
}

স্থির vpiHandle intermodpath_iterate(পূর্ণসংখ্যা code, vpiHandle ref)
{
      কাঠামো __vpiInterModPath*rfp = dynamic_cast<__vpiInterModPath*>(ref);
      প্রতিজ্ঞা(rfp);

	// For now intermodpaths only support exactly two ports
      নির্বাচন (code) {
	  ক্ষেত্র vpiPort: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(2, আকার(vpiHandle*)));
	    args[0] = rfp->port1;
	    args[1] = rfp->port2;
	    ফেরত vpip_make_iterator(2, args, সত্য);
	  }
      }
      ফেরত 0;
}

/*
 * This routine will put specific dimension of delay[] values
 * into a vpiHandle. In this case, we will put
 * specific delays values in a vpiInterModPath object
 *
 */
স্থির শূন্য intermodpath_put_delays (vpiHandle ref, p_vpi_delay delays)
{
      vvp_time64_t tmp[12];
      পূর্ণসংখ্যা idx;
      কাঠামো __vpiInterModPath * src = dynamic_cast<__vpiInterModPath*>(ref) ;
      প্রতিজ্ঞা(src) ;

      vvp_fun_intermodpath *fun = dynamic_cast<vvp_fun_intermodpath*>(src->net->fun);
      প্রতিজ্ঞা( fun );

      প্রকারলেখ unsigned অক্ষর map_array_t[12];
	// Only the first six entries are used for the less than twelve maps.
      স্থির ধ্রুবক map_array_t map_1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      স্থির ধ্রুবক map_array_t map_2 = {0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0};
      স্থির ধ্রুবক map_array_t map_3 = {0, 1, 2, 0, 2, 1, 0, 0, 0, 0, 0, 0};
      স্থির ধ্রুবক map_array_t map_6 = {0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0};
      স্থির ধ্রুবক map_array_t map12 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

      ধ্রুবক map_array_t*use_map = 0;
      নির্বাচন (delays->no_of_delays) {
	  ক্ষেত্র 1:
	    use_map = &map_1;
	    বিরতি;
	  ক্ষেত্র 2:
	    use_map = &map_2;
	    বিরতি;
	  ক্ষেত্র 3:
	    use_map = &map_3;
	    বিরতি;
	  ক্ষেত্র 6:
	    use_map = &map_6;
	    বিরতি;
	  ক্ষেত্র 12:
	    use_map = &map12;
	    বিরতি;
	  পূর্বনির্ধারিত:
	    প্রতিজ্ঞা(0);
	    বিরতি;
      }

      যদি (delays->time_type == vpiSimTime) {
	    জন্য (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_timestruct_to_time(delays->da+use_map[0][idx]);
	    }
      } অন্যথা {
	      // You cannot create a modpath with a negative delay so set it
	      // to zero per 1364-2005 section 14.3.1.
	    জন্য (idx = 0 ; idx < delays->no_of_delays ; idx += 1) {
		  যদি (delays->da[idx].real < 0.0) delays->da[idx].real = 0.0;
	    }
	    জন্য (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_scaled_real_to_time64(delays->da[use_map[0][idx]].real,
							src->scope);
	    }
      }

      /* Now define the to-from-x delays if needed. */
      যদি (delays->no_of_delays <= 6) {
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

স্থির শূন্য intermodpath_get_delays ( vpiHandle ref, p_vpi_delay delays )
{
      কাঠামো __vpiInterModPath*src = dynamic_cast<__vpiInterModPath*>(ref) ;
      প্রতিজ্ঞা(src);

      vvp_fun_intermodpath *fun = dynamic_cast<vvp_fun_intermodpath*>(src->net->fun);
      প্রতিজ্ঞা(fun);

      পূর্ণসংখ্যা idx;
      vvp_time64_t tmp[12];
      fun->get_delay12(tmp);

      নির্বাচন (delays->no_of_delays) {
	  ক্ষেত্র 1:
	  ক্ষেত্র 2:
	  ক্ষেত্র 3:
	  ক্ষেত্র 6:
	  ক্ষেত্র 12:
	    বিরতি;

	  পূর্বনির্ধারিত:
	    প্রতিজ্ঞা(0);
	    বিরতি;
      }

      যদি (delays->time_type == vpiSimTime) {
	    জন্য (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  vpip_time_to_timestruct(delays->da+idx, tmp[idx]);
	    }
      } অন্যথা {
	    জন্য (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  delays->da[idx].real = vpip_time_to_scaled_real(tmp[idx], src->scope);
	    }
      }
}

/*
* The __vpiInterModPath class is what the VPI client sees as a
* vpiInterModPath object.
*/
অন্তঃরেখ __vpiInterModPath::__vpiInterModPath()
{ }

পূর্ণসংখ্যা __vpiInterModPath::get_type_code(শূন্য) ধ্রুবক
{ ফেরত vpiInterModPath; }

পূর্ণসংখ্যা __vpiInterModPath::vpi_get(পূর্ণসংখ্যা code)
{ ফেরত intermodpath_get(code, এই); }

শূন্য __vpiInterModPath::vpi_get_মান(p_vpi_মান val)
{ intermodpath_get_মান(এই, val); }

vpiHandle __vpiInterModPath::vpi_put_মান(p_vpi_মান val, পূর্ণসংখ্যা flags)
{ ফেরত intermodpath_put_মান(এই, val, flags); }

vpiHandle __vpiInterModPath::vpi_handle(পূর্ণসংখ্যা code)
{ ফেরত intermodpath_get_handle(code, এই); }

vpiHandle __vpiInterModPath::vpi_iterate(পূর্ণসংখ্যা code)
{ ফেরত intermodpath_iterate(code, এই); }

শূন্য __vpiInterModPath::vpi_get_delays(p_vpi_delay del)
{ intermodpath_get_delays(এই, del); }

শূন্য __vpiInterModPath::vpi_put_delays(p_vpi_delay del)
{ intermodpath_put_delays(এই, del); }

স্থির পূর্ণসংখ্যা intermodpath_free_object( vpiHandle ref )
{
      delete ref;
      ফেরত 1 ;
}

__vpiHandle::free_object_fun_t __vpiInterModPath::free_object_fun(শূন্য)
{ ফেরত &intermodpath_free_object; }

/*
 * This function will construct a vpiInterModPath Object.
 * give a respective "net", and will point to his
 * respective functor
 */

#ifdef CHECK_WITH_VALGRIND
স্থির কাঠামো __vpiInterModPath**imp_list = 0;
স্থির unsigned imp_গুনতি = 0;
#endif

কাঠামো __vpiInterModPath* vpip_make_intermodpath(vvp_net_t *net, vpiPortInfo* port1, vpiPortInfo* port2)
{
      কাঠামো __vpiInterModPath*obj = নতুন __vpiInterModPath;
      obj->scope = vpip_peek_current_scope ( );

      obj->net = net;
      obj->port1 = port1;
      obj->port2 = port2;

#ifdef CHECK_WITH_VALGRIND
      imp_গুনতি += 1;
      imp_list = static_cast<কাঠামো __vpiInterModPath **>
                 (realloc(imp_list,
                  imp_গুনতি*আকার(কাঠামো __vpiInterModPath **)));
      imp_list[imp_গুনতি-1] = obj;
#endif
      ফেরত obj;
}

#ifdef CHECK_WITH_VALGRIND
শূন্য intermodpath_delete()
{
      জন্য (unsigned idx = 0; idx < mp_গুনতি; idx += 1) {
	    delete imp_list[idx];
      }
      মুক্ত(imp_list);
      imp_list = 0;
      imp_গুনতি = 0;
}
#endif
