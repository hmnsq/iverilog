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

#अन्तर्भाव "delay.h"
#अन्तर्भाव "schedule.h"
#अन्तर्भाव "vpi_priv.h"
#अन्तर्भाव "config.h"
#ifdef CHECK_WITH_VALGRIND
#अन्तर्भाव "vvp_cleanup.h"
#endif
#अन्तर्भाव <iostream>
#अन्तर्भाव <cstdlib>
#अन्तर्भाव <list>
#अन्तर्भाव <cassert>
#अन्तर्भाव <cmath>
#अन्तर्भाव "ivl_alloc.h"

using नामाकाश std;

शून्य vvp_delay_t::calculate_min_delay_()
{
      min_delay_ = rise_;
      यदि (fall_ < min_delay_)
	    min_delay_ = fall_;
      यदि (ignore_decay_) decay_ = min_delay_;
      अन्यथा यदि (decay_ < min_delay_)
	    min_delay_ = decay_;
}

vvp_delay_t::vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall)
{
      rise_ = rise;
      fall_ = fall;
      decay_= fall < rise? fall : rise;
      min_delay_ = decay_;
      ignore_decay_ = असत्य;
}

vvp_delay_t::vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall, vvp_time64_t decay)
{
      rise_ = rise;
      fall_ = fall;
      decay_= decay;
      ignore_decay_ = असत्य;

      calculate_min_delay_();
}

शून्य vvp_delay_t::set_ignore_decay()
{
      ignore_decay_ = सत्य;

      calculate_min_delay_();
}

vvp_delay_t::~vvp_delay_t()
{
}

vvp_time64_t vvp_delay_t::get_delay(vvp_bit4_t from, vvp_bit4_t to)
{
      विकल्प (from) {
	  पक्ष BIT4_0:
	    विकल्प (to) {
		पक्ष BIT4_0: प्रत्यागम 0;
		पक्ष BIT4_1: प्रत्यागम rise_;
		पक्ष BIT4_X: प्रत्यागम min_delay_;
		पक्ष BIT4_Z: प्रत्यागम decay_;
	    }
	    भङ्ग;
	  पक्ष BIT4_1:
	    विकल्प (to) {
		पक्ष BIT4_0: प्रत्यागम fall_;
		पक्ष BIT4_1: प्रत्यागम 0;
		पक्ष BIT4_X: प्रत्यागम min_delay_;
		पक्ष BIT4_Z: प्रत्यागम decay_;
	    }
	    भङ्ग;
	  पक्ष BIT4_X:
	    विकल्प (to) {
		पक्ष BIT4_0: प्रत्यागम fall_;
		पक्ष BIT4_1: प्रत्यागम rise_;
		पक्ष BIT4_X: प्रत्यागम 0;
		पक्ष BIT4_Z: प्रत्यागम decay_;
	    }
	    भङ्ग;
	  पक्ष BIT4_Z:
	    विकल्प (to) {
		पक्ष BIT4_0: प्रत्यागम fall_;
		पक्ष BIT4_1: प्रत्यागम rise_;
		पक्ष BIT4_X: प्रत्यागम min_delay_;
		पक्ष BIT4_Z: प्रत्यागम 0;
	    }
	    भङ्ग;
      }

      प्रतिज्ञा(0);
      प्रत्यागम 0;
}

vvp_time64_t vvp_delay_t::get_min_delay() ध्रुव
{
      प्रत्यागम min_delay_;
}

शून्य vvp_delay_t::set_rise(vvp_time64_t val)
{
      rise_ = val;
      यदि (val < min_delay_) {
	    min_delay_ = val;
	    यदि (ignore_decay_) decay_ = val;
      } अन्यथा
	    calculate_min_delay_();
}

शून्य vvp_delay_t::set_fall(vvp_time64_t val)
{
      fall_ = val;
      यदि (val < min_delay_) {
	    min_delay_ = val;
	    यदि (ignore_decay_) decay_ = val;
      } अन्यथा
	    calculate_min_delay_();
}

शून्य vvp_delay_t::set_decay(vvp_time64_t val)
{
      प्रतिज्ञा(!ignore_decay_);

      decay_ = val;
      यदि (val < min_delay_)
	    min_delay_ = val;
      अन्यथा
	    calculate_min_delay_();
}

vvp_fun_delay::vvp_fun_delay(vvp_net_t*n, unsigned width, ध्रुव vvp_delay_t&d)
: net_(n), delay_(d)
{
      cur_real_ = 0.0;
      यदि (width > 0) {
            cur_vec4_ = vvp_vector4_t(width, BIT4_X);
            cur_vec8_ = vvp_vector8_t(cur_vec4_, 6, 6);
            schedule_init_propagate(net_, cur_vec4_);
      } अन्यथा {
            schedule_init_propagate(net_, cur_real_);
      }
      list_ = 0;
      type_ = UNKNOWN_DELAY;
      initial_ = सत्य;
	// Calculate the values used when converting variable delays
	// to simulation time units.
      ध्रुव __vpiScope*scope = vpip_peek_current_scope();

      पूर्णाङ्क powr = scope->time_units - scope->time_precision;
      round_ = 1;
      प्रति (पूर्णाङ्क lp = 0; lp < powr; lp += 1) round_ *= 10;

      powr = scope->time_precision - vpip_get_time_precision();
      scale_ = 1;
      प्रति (पूर्णाङ्क lp = 0; lp < powr; lp += 1) scale_ *= 10;
}

vvp_fun_delay::~vvp_fun_delay()
{
      यावत् (संरचना event_*cur = dequeue_())
	    delete cur;
}

तार्किक vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        ध्रुव vvp_vector4_t&bit)
{
      यदि (list_ == 0) प्रत्यागम असत्य;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      यदि (list_->next->ptr_vec4.eeq(bit)) प्रत्यागम सत्य;

      clean_pulse_events_(use_delay);
      प्रत्यागम असत्य;
}

तार्किक vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        ध्रुव vvp_vector8_t&bit)
{
      यदि (list_ == 0) प्रत्यागम असत्य;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      यदि (list_->next->ptr_vec8.eeq(bit)) प्रत्यागम सत्य;

      clean_pulse_events_(use_delay);
      प्रत्यागम असत्य;
}

तार्किक vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        द्विगुण bit)
{
      यदि (list_ == 0) प्रत्यागम असत्य;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      यदि (list_->next->ptr_real == bit) प्रत्यागम सत्य;

      clean_pulse_events_(use_delay);
      प्रत्यागम असत्य;
}

शून्य vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay)
{
      प्रतिज्ञा(list_ != 0);

      कुरु {
	    संरचना event_*cur = list_->next;
	      /* If this event is far enough from the event I'm about
	         to create, then that scheduled event is not a pulse
	         to be eliminated, so we're done. */
	    यदि (cur->sim_time+use_delay <= use_delay+schedule_simtime())
		  भङ्ग;

	    यदि (list_ == cur)
		  list_ = 0;
	    अन्यथा
		  list_->next = cur->next;
	    delete cur;
      } यावत् (list_);
}

/*
 * FIXME: this implementation currently only uses the maximum delay
 * from all the bit changes in the vectors. If there are multiple
 * changes with different delays, then the results would be
 * wrong. What should happen is that if there are multiple changes,
 * multiple vectors approaching the result should be scheduled.
 */
शून्य vvp_fun_delay::recv_vec4(vvp_net_ptr_t port, ध्रुव vvp_vector4_t&bit,
                              vvp_context_t)
{
      यदि (port.port() > 0) {
	      // Get the integer value of the bit vector, or 0 if
	      // there are X or Z bits.
	    vvp_time64_t bval = 0;
	      // The following does not work correctly for negative values.
	      // They should be sign extended to 64 bits (1364-2001 9.7.1).
	    vector4_to_मूल्य(bit, bval);
	      // Integer values do not need to be rounded so just scale them.
	    vvp_time64_t val = bval * round_ * scale_;

	    विकल्प (port.port()) {
		पक्ष 1:
		  delay_.set_rise(val);
		  प्रत्यागम;
		पक्ष 2:
		  delay_.set_fall(val);
		  प्रत्यागम;
		पक्ष 3:
		  delay_.set_decay(val);
		  प्रत्यागम;
	    }
	    प्रत्यागम;
      }

      vvp_time64_t use_delay;
	/* This is an initial value so it needs to be compared to all the
	   bits (the order the bits are changed is not deterministic). */
      यदि (initial_) {
	    type_ = VEC4_DELAY;
            cur_vec8_ = vvp_vector8_t(vvp_vector4_t(0, BIT4_X), 6, 6);
	    vvp_bit4_t cur_val = cur_vec4_.value(0);
	    use_delay = delay_.get_delay(cur_val, bit.value(0));
	    प्रति (unsigned idx = 1 ;  idx < bit.size() ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(cur_val, bit.value(idx));
		  यदि (tmp > use_delay) use_delay = tmp;
	    }
      } अन्यथा {
	    प्रतिज्ञा(type_ == VEC4_DELAY);

	      // Use as a reference for calculating the delay the
	      // current value of the output. Detect and handle the
	      // special case that the event list contains the current
	      // value as a zero-delay-remaining event.
	    ध्रुव vvp_vector4_t&use_vec4 = (list_ && list_->next->sim_time == schedule_simtime())? list_->next->ptr_vec4 : cur_vec4_;

	      /* How many bits to compare? */
	    unsigned use_wid = use_vec4.size();
	    यदि (bit.size() < use_wid) use_wid = bit.size();

	      /* Scan the vectors looking for delays. Select the maximum
	         delay encountered. */
	    use_delay = delay_.get_delay(use_vec4.value(0), bit.value(0));

	    प्रति (unsigned idx = 1 ;  idx < use_wid ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(use_vec4.value(idx), bit.value(idx));
		  यदि (tmp > use_delay) use_delay = tmp;
	    }
      }

      /* what *should* happen here is we check to see if there is a
         transaction in the queue. This would be a pulse that needs to be
         eliminated. */
      यदि (clean_pulse_events_(use_delay, bit)) प्रत्यागम;

      vvp_time64_t use_simtime = schedule_simtime() + use_delay;

	/* And propagate it. */
      यदि (use_delay == 0 && list_ == 0) {
	    cur_vec4_ = bit;
	    initial_ = असत्य;
	    net_->send_vec4(cur_vec4_, 0);
      } अन्यथा {
	    संरचना event_*cur = नूतन संरचना event_(use_simtime);
	    cur->run_run_ptr = &vvp_fun_delay::run_run_vec4_;
	    cur->ptr_vec4 = bit;
	    enqueue_(cur);
	    schedule_generic(एतत्, use_delay, असत्य);
      }
}

शून्य vvp_fun_delay::recv_vec4_pv(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
			         unsigned base, unsigned vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

/* See the recv_vec4 comment above. */
शून्य vvp_fun_delay::recv_vec8(vvp_net_ptr_t port, ध्रुव vvp_vector8_t&bit)
{
      प्रतिज्ञा(port.port() == 0);

      vvp_time64_t use_delay;
	/* This is an initial value so it needs to be compared to all the
	   bits (the order the bits are changed is not deterministic). */
      यदि (initial_) {
	    type_ = VEC8_DELAY;
            cur_vec4_ = vvp_vector4_t(0, BIT4_X);
	    vvp_bit4_t cur_val = cur_vec8_.value(0).value();
	    use_delay = delay_.get_delay(cur_val, bit.value(0).value());
	    प्रति (unsigned idx = 1 ;  idx < bit.size() ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(cur_val, bit.value(idx).value());
		  यदि (tmp > use_delay) use_delay = tmp;
	    }
      } अन्यथा {
	    प्रतिज्ञा(type_ == VEC8_DELAY);

	      // Use as a reference for calculating the delay the
	      // current value of the output. Detect and handle the
	      // special case that the event list contains the current
	      // value as a zero-delay-remaining event.
	    ध्रुव vvp_vector8_t&use_vec8 = (list_ && list_->next->sim_time == schedule_simtime())? list_->next->ptr_vec8 : cur_vec8_;

	      /* How many bits to compare? */
	    unsigned use_wid = use_vec8.size();
	    यदि (bit.size() < use_wid) use_wid = bit.size();

	      /* Scan the vectors looking for delays. Select the maximum
	         delay encountered. */
	    use_delay = delay_.get_delay(use_vec8.value(0).value(),
	                                 bit.value(0).value());

	    प्रति (unsigned idx = 1 ;  idx < use_wid ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(use_vec8.value(idx).value(),
		                         bit.value(idx).value());
		  यदि (tmp > use_delay) use_delay = tmp;
	    }
      }

      /* what *should* happen here is we check to see if there is a
         transaction in the queue. This would be a pulse that needs to be
         eliminated. */
      यदि (clean_pulse_events_(use_delay, bit)) प्रत्यागम;

      vvp_time64_t use_simtime = schedule_simtime() + use_delay;

	/* And propagate it. */
      यदि (use_delay == 0 && list_ == 0) {
	    cur_vec8_ = bit;
	    initial_ = असत्य;
	    net_->send_vec8(cur_vec8_);
      } अन्यथा {
	    संरचना event_*cur = नूतन संरचना event_(use_simtime);
	    cur->ptr_vec8 = bit;
	    cur->run_run_ptr = &vvp_fun_delay::run_run_vec8_;
	    enqueue_(cur);
	    schedule_generic(एतत्, use_delay, असत्य);
      }
}

शून्य vvp_fun_delay::recv_vec8_pv(vvp_net_ptr_t ptr, ध्रुव vvp_vector8_t&bit,
			         unsigned base, unsigned vwid)
{
      recv_vec8_pv_(ptr, bit, base, vwid);
}

शून्य vvp_fun_delay::recv_real(vvp_net_ptr_t port, द्विगुण bit,
                              vvp_context_t)
{
      यदि (port.port() > 0) {
	    /* If the port is not 0, then this is a delay value that
	    should be rounded and converted to an integer delay. */
	    vvp_time64_t val = 0;
	    यदि (bit > -0.5) {
		  val = (vvp_time64_t) (bit * round_ + 0.5) * scale_;
	    } अन्यथा यदि (bit != bit) {
		    // For a NaN we use the default (0).
	    } अन्यथा {
		  vvp_vector4_t vec4(8*परिमाण(vvp_time64_t),
		                     floor(-bit * round_ + 0.5) * -1 * scale_);
		  vector4_to_मूल्य(vec4, val);
	    }

	    विकल्प (port.port()) {
		पक्ष 1:
		  delay_.set_rise(val);
		  प्रत्यागम;
		पक्ष 2:
		  delay_.set_fall(val);
		  प्रत्यागम;
		पक्ष 3:
		  delay_.set_decay(val);
		  प्रत्यागम;
	    }
	    प्रत्यागम;
      }

      यदि (initial_) {
	    type_ = REAL_DELAY;
            cur_vec4_ = vvp_vector4_t(0, BIT4_X);
            cur_vec8_ = vvp_vector8_t(cur_vec4_, 6, 6);
      } अन्यथा प्रतिज्ञा(type_ == REAL_DELAY);

      vvp_time64_t use_delay;
      use_delay = delay_.get_min_delay();

      /* Eliminate glitches. */
      यदि (clean_pulse_events_(use_delay, bit)) प्रत्यागम;

      /* This must be done after cleaning pulses to avoid propagating
       * an incorrect value. */
      यदि (cur_real_ == bit) प्रत्यागम;

      vvp_time64_t use_simtime = schedule_simtime() + use_delay;

      यदि (use_delay == 0 && list_ == 0) {
	    cur_real_ = bit;
	    initial_ = असत्य;
	    net_->send_real(cur_real_, 0);
      } अन्यथा {
	    संरचना event_*cur = नूतन संरचना event_(use_simtime);
	    cur->run_run_ptr = &vvp_fun_delay::run_run_real_;
	    cur->ptr_real = bit;
	    enqueue_(cur);

	    schedule_generic(एतत्, use_delay, असत्य);
      }
}

शून्य vvp_fun_delay::run_run()
{
      vvp_time64_t sim_time = schedule_simtime();
      यदि (list_ == 0 || list_->next->sim_time > sim_time)
	    प्रत्यागम;

      संरचना event_*cur = dequeue_();
      यदि (cur == 0)
	    प्रत्यागम;

      (एतत्->*(cur->run_run_ptr))(cur);
      initial_ = असत्य;
      delete cur;
}

शून्य vvp_fun_delay::run_run_vec4_(ध्रुव संरचना event_*cur)
{
      cur_vec4_ = cur->ptr_vec4;
      net_->send_vec4(cur_vec4_, 0);
}

शून्य vvp_fun_delay::run_run_vec8_(ध्रुव संरचना vvp_fun_delay::event_*cur)
{
      cur_vec8_ = cur->ptr_vec8;
      net_->send_vec8(cur_vec8_);
}

शून्य vvp_fun_delay::run_run_real_(ध्रुव संरचना vvp_fun_delay::event_*cur)
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
      यावत् (src_list_) {
	    vvp_fun_modpath_src*tmp = src_list_;
	    src_list_ = tmp->next_;
	    delete tmp;
      }
      यावत् (ifnone_list_) {
	    vvp_fun_modpath_src*tmp = ifnone_list_;
	    ifnone_list_ = tmp->next_;
	    delete tmp;
      }
}

शून्य vvp_fun_modpath::add_modpath_src(vvp_fun_modpath_src*that, तार्किक ifnone)
{
      प्रतिज्ञा(that->next_ == 0);
      यदि (ifnone) {
	    that->next_ = ifnone_list_;
	    ifnone_list_ = that;
      } अन्यथा {
	    that->next_ = src_list_;
	    src_list_ = that;
      }
}

स्थिर vvp_time64_t delay_from_edge(vvp_bit4_t a, vvp_bit4_t b,
                                           vvp_time64_t ध्रुव array[12])
{
      प्रकारनिर्देश delay_edge_t bit4_table4[4];
      स्थिर ध्रुव bit4_table4 edge_table[4] = {
	    { DELAY_EDGE_01, DELAY_EDGE_01, DELAY_EDGE_0z, DELAY_EDGE_0x },
	    { DELAY_EDGE_10, DELAY_EDGE_10, DELAY_EDGE_1z, DELAY_EDGE_1x },
	    { DELAY_EDGE_z0, DELAY_EDGE_z1, DELAY_EDGE_z0, DELAY_EDGE_zx },
	    { DELAY_EDGE_x0, DELAY_EDGE_x1, DELAY_EDGE_xz, DELAY_EDGE_x0 }
      };

      प्रत्यागम array[ edge_table[a][b] ];
}

शून्य vvp_fun_modpath::recv_vec4(vvp_net_ptr_t port, ध्रुव vvp_vector4_t&bit,
                                vvp_context_t)
{
	/* Only the first port is used. */
      यदि (port.port() > 0)
	    प्रत्यागम;

      यदि (cur_vec4_.eeq(bit))
	    प्रत्यागम;

	/* Select a time delay source that applies. Notice that there
	   may be multiple delay sources that apply, so collect all
	   the candidates into a list first. */
      list<vvp_fun_modpath_src*>candidate_list;
      vvp_time64_t candidate_wake_time = 0;
      प्रति (vvp_fun_modpath_src*cur = src_list_ ;  cur ;  cur=cur->next_) {
	      /* Skip paths that are disabled by conditions. */
	    यदि (cur->condition_flag_ == असत्य)
		  अनुवर्त;

	    यदि (candidate_list.empty()) {
		  candidate_list.push_back(cur);
		  candidate_wake_time = cur->wake_time_;
	    } अन्यथा यदि (cur->wake_time_ == candidate_wake_time) {
		  candidate_list.push_back(cur);
	    } अन्यथा यदि (cur->wake_time_ > candidate_wake_time) {
		  candidate_list.assign(1, cur);
		  candidate_wake_time = cur->wake_time_;
	    } अन्यथा {
		  अनुवर्त; /* Skip this entry. */
	    }
      }

	/* Only add the ifnone delay if it has a later wake_time_ or
	 * if there are no normal delays. */
      vvp_time64_t ifnone_wake_time = candidate_wake_time;
      प्रति (vvp_fun_modpath_src*cur = ifnone_list_ ;  cur ;  cur=cur->next_) {
	    यदि (candidate_list.empty()) {
		  candidate_list.push_back(cur);
		  ifnone_wake_time = cur->wake_time_;
	    } अन्यथा यदि (cur->wake_time_ == ifnone_wake_time &&
	               ifnone_wake_time > candidate_wake_time) {
		  candidate_list.push_back(cur);
	    } अन्यथा यदि (cur->wake_time_ > ifnone_wake_time) {
		  candidate_list.assign(1, cur);
		  ifnone_wake_time = cur->wake_time_;
	    } अन्यथा {
		  अनुवर्त; /* Skip this entry. */
	    }
      }

	/* Handle the special case that there are no delays that
	   match. This may happen, for example, if the set of
	   conditional delays is incomplete, leaving some cases
	   uncovered. In that case, just pass the data without delay */
      यदि (candidate_list.empty()) {
	    cur_vec4_ = bit;
	    schedule_generic(एतत्, 0, असत्य);
	    प्रत्यागम;
      }

	/* Now given that we have a list of candidate delays, find for
	   each if the 12 numbers the minimum from all the
	   candidates. This minimum set becomes the chosen delay to
	   use. */
      vvp_time64_t out_at[12];
      vvp_time64_t now = schedule_simtime();

      प्रकारनिर्देश list<vvp_fun_modpath_src*>::const_iterator iter_t;

      iter_t cur = candidate_list.begin();
      ध्रुव vvp_fun_modpath_src*src = *cur;

      प्रति (unsigned idx = 0 ;  idx < 12 ;  idx += 1) {
	    out_at[idx] = src->wake_time_ + src->delay_[idx];
	    यदि (out_at[idx] <= now)
		  out_at[idx] = 0;
	    अन्यथा
		  out_at[idx] -= now;
      }

      प्रति (++ cur ; cur != candidate_list.end() ; ++ cur ) {
	    src = *cur;
	    प्रति (unsigned idx = 0 ;  idx < 12 ;  idx += 1) {
		  vvp_time64_t tmp = src->wake_time_ + src->delay_[idx];
		  यदि (tmp <= now)
			tmp = 0;
		  अन्यथा
			tmp -= now;
		  यदि (tmp < out_at[idx])
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
      प्रति (unsigned idx = 1 ;  idx < bit.size() ;  idx += 1) {
	    vvp_time64_t tmp = delay_from_edge(cur_vec4_.value(idx),
					       bit.value(idx),
					       out_at);
	      /* If the current and new bit values match then no delay
	       * is needed for this bit. */
	    यदि (cur_vec4_.value(idx) == bit.value(idx)) अनुवर्त;
	    यदि (tmp != use_delay) {
		  fprintf(stderr, "sorry: multi-bit module path delays are "
		                  "currently not fully supported.\n");
		  fprintf(stderr, "     : using the LSB delay for all bits.\n");
		  अनुवर्त;
            }
      }

      cur_vec4_ = bit;
      schedule_generic(एतत्, use_delay, असत्य);
}

शून्य vvp_fun_modpath::run_run()
{
      net_->send_vec4(cur_vec4_, 0);
}

vvp_fun_modpath_src::vvp_fun_modpath_src(vvp_time64_t ध्रुव del[12])
{
      प्रति (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = del[idx];

      next_ = 0;
      wake_time_ = 0;
      condition_flag_ = सत्य;
}

vvp_fun_modpath_src::~vvp_fun_modpath_src()
{
}

शून्य vvp_fun_modpath_src::get_delay12(vvp_time64_t val[12]) ध्रुव
{
      प्रति (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    val[idx] = delay_[idx];
}

शून्य vvp_fun_modpath_src::put_delay12(ध्रुव vvp_time64_t val[12])
{
      प्रति (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = val[idx];
}

शून्य vvp_fun_modpath_src::recv_vec4(vvp_net_ptr_t port, ध्रुव vvp_vector4_t&bit,
                                    vvp_context_t)
{
      यदि (port.port() == 0) {
	      // The modpath input...
	    यदि (test_vec4(bit))
		  wake_time_ = schedule_simtime();

      } अन्यथा यदि (port.port() == 1) {
	      // The modpath condition input...
	    यदि (bit.value(0) != BIT4_0)
		  condition_flag_ = सत्य;
	    अन्यथा
		  condition_flag_ = असत्य;
      }
}

तार्किक vvp_fun_modpath_src::test_vec4(ध्रुव vvp_vector4_t&)
{
      प्रत्यागम सत्य;
}

vvp_fun_modpath_edge::vvp_fun_modpath_edge(vvp_time64_t del[12],
					   तार्किक pos, तार्किक neg)
: vvp_fun_modpath_src(del)
{
      old_मूल्य = BIT4_X;
      posedge_ = pos;
      negedge_ = neg;
}

तार्किक vvp_fun_modpath_edge::test_vec4(ध्रुव vvp_vector4_t&bit)
{
      vvp_bit4_t tmp = old_मूल्य;
      old_मूल्य = bit.value(0);

      पूर्णाङ्क edge_flag = edge(tmp, old_मूल्य);
      यदि (edge_flag > 0) प्रत्यागम posedge_;
      यदि (edge_flag < 0) प्रत्यागम negedge_;
      प्रत्यागम असत्य;
}


/*
 * All the below routines that begin with
 * modpath_src_* belong the internal function
 * of an vpiModPathIn object. This is used to
 * make some specific delays path operations
 *
 */
स्थिर पूर्णाङ्क modpath_src_get(पूर्णाङ्क, vpiHandle ref)
{
      संरचना __vpiModPathSrc*obj =dynamic_cast<__vpiModPathSrc*>(ref);
      प्रतिज्ञा(obj);
      प्रत्यागम 0;
}

स्थिर शून्य modpath_src_get_मूल्य(vpiHandle ref, p_vpi_मूल्य)
{
      संरचना __vpiModPathSrc* modpathsrc = dynamic_cast<__vpiModPathSrc*>(ref);
      प्रतिज्ञा(modpathsrc);
      प्रत्यागम;
}

स्थिर vpiHandle modpath_src_put_मूल्य(vpiHandle ref, s_vpi_मूल्य *, पूर्णाङ्क )
{
      संरचना __vpiModPathSrc* modpathsrc = dynamic_cast<__vpiModPathSrc*>(ref);
      प्रतिज्ञा(modpathsrc);
      प्रत्यागम 0;
}

स्थिर vpiHandle modpath_src_get_handle(पूर्णाङ्क code, vpiHandle ref)
{
      संरचना __vpiModPathSrc*rfp = dynamic_cast<__vpiModPathSrc*>(ref);
      प्रतिज्ञा(rfp);

      विकल्प (code) {

	पक्ष vpiScope:
	  प्रत्यागम rfp->dest->scope;

	  पक्ष vpiModule:
	      { __vpiScope*scope = rfp->dest->scope;
		यावत् (scope && scope->get_type_code() != vpiModule)
		      scope = scope->scope;
		प्रतिज्ञा(scope);
		प्रत्यागम scope;
	      }

	    // Handles to path term objects should really be obtained via
	    // the vpi_iterate and vpi_scan functions. Continue to allow
	    // them to be obtained here for backwards compatibility with
	    // older versions of Icarus Verilog.

	  पक्ष vpiModPathIn:
	    प्रत्यागम &rfp->path_term_in;

	  पक्ष vpiModPathOut:
	    प्रत्यागम &rfp->dest->path_term_out;
      }
      प्रत्यागम 0;
}

स्थिर vpiHandle modpath_src_iterate(पूर्णाङ्क code, vpiHandle ref)
{
      संरचना __vpiModPathSrc*rfp = dynamic_cast<__vpiModPathSrc*>(ref);
      प्रतिज्ञा(rfp);

	// Module paths with multiple sources or destinations are
	// currently represented by a separate modpath object for
	// each source/destination combination, so there is only
	// ever one input path term and one output path term.
      विकल्प (code) {
	  पक्ष vpiModPathIn: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(1, परिमाण(vpiHandle*)));
	    args[0] = &rfp->path_term_in;
	    प्रत्यागम vpip_make_iterator(1, args, सत्य);
	  }
	  पक्ष vpiModPathOut: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(1, परिमाण(vpiHandle*)));
	    args[0] = &rfp->dest->path_term_out;
	    प्रत्यागम vpip_make_iterator(1, args, सत्य);
	  }
      }
      प्रत्यागम 0;
}

स्थिर vpiHandle modpath_src_index ( vpiHandle ref, पूर्णाङ्क)
{
      प्रतिज्ञा(ref->get_type_code() == vpiModPathIn);
      प्रत्यागम 0;
}


/*
 * This routine will put specific dimension of delay[] values
 * into a vpiHandle. In this case, we will put
 * specific delays values in a vpiModPathIn object
 *
 */
स्थिर शून्य modpath_src_put_delays (vpiHandle ref, p_vpi_delay delays)
{
      vvp_time64_t tmp[12];
      पूर्णाङ्क idx;
      संरचना __vpiModPathSrc * src = dynamic_cast<__vpiModPathSrc*>(ref) ;
      प्रतिज्ञा(src) ;

      vvp_fun_modpath_src *fun = dynamic_cast<vvp_fun_modpath_src*>(src->net->fun);
      प्रतिज्ञा( fun );

      प्रकारनिर्देश unsigned वर्ण map_array_t[12];
	// Only the first six entries are used for the less than twelve maps.
      स्थिर ध्रुव map_array_t map_1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      स्थिर ध्रुव map_array_t map_2 = {0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0};
      स्थिर ध्रुव map_array_t map_3 = {0, 1, 2, 0, 2, 1, 0, 0, 0, 0, 0, 0};
      स्थिर ध्रुव map_array_t map_6 = {0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0};
      स्थिर ध्रुव map_array_t map12 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

      ध्रुव map_array_t*use_map = 0;
      विकल्प (delays->no_of_delays) {
	  पक्ष 1:
	    use_map = &map_1;
	    भङ्ग;
	  पक्ष 2:
	    use_map = &map_2;
	    भङ्ग;
	  पक्ष 3:
	    use_map = &map_3;
	    भङ्ग;
	  पक्ष 6:
	    use_map = &map_6;
	    भङ्ग;
	  पक्ष 12:
	    use_map = &map12;
	    भङ्ग;
	  स्वभाव:
	    प्रतिज्ञा(0);
	    भङ्ग;
      }

      यदि (delays->time_type == vpiSimTime) {
	    प्रति (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_timestruct_to_time(delays->da+use_map[0][idx]);
	    }
      } अन्यथा {
	      // You cannot create a modpath with a negative delay so set it
	      // to zero per 1364-2005 section 14.3.1.
	    प्रति (idx = 0 ; idx < delays->no_of_delays ; idx += 1) {
		  यदि (delays->da[idx].real < 0.0) delays->da[idx].real = 0.0;
	    }
	    प्रति (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_scaled_real_to_time64(delays->da[use_map[0][idx]].real,
							src->dest->scope);
	    }
      }

      /* Now define the to-from-x delays if needed. */
      यदि (delays->no_of_delays <= 6) {
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

स्थिर शून्य modpath_src_get_delays ( vpiHandle ref, p_vpi_delay delays )
{
      संरचना __vpiModPathSrc*src = dynamic_cast<__vpiModPathSrc*>(ref) ;
      प्रतिज्ञा(src);

      vvp_fun_modpath_src *fun = dynamic_cast<vvp_fun_modpath_src*>(src->net->fun);
      प्रतिज्ञा(fun);

      पूर्णाङ्क idx;
      vvp_time64_t tmp[12];
      fun->get_delay12(tmp);

      विकल्प (delays->no_of_delays) {
	  पक्ष 1:
	  पक्ष 2:
	  पक्ष 3:
	  पक्ष 6:
	  पक्ष 12:
	    भङ्ग;

	  स्वभाव:
	    प्रतिज्ञा(0);
	    भङ्ग;
      }

      यदि (delays->time_type == vpiSimTime) {
	    प्रति (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  vpip_time_to_timestruct(delays->da+idx, tmp[idx]);
	    }
      } अन्यथा {
	    प्रति (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  delays->da[idx].real = vpip_time_to_scaled_real(tmp[idx], src->dest->scope);
	    }
      }
}

स्थिर पूर्णाङ्क pathterm_get(पूर्णाङ्क code, vpiHandle ref)
{
      संरचना __vpiModPathTerm*obj = dynamic_cast<__vpiModPathTerm*>(ref);
      प्रतिज्ञा(obj);

      विकल्प (code) {
	  पक्ष vpiEdge:
	    प्रत्यागम obj->edge;
	  स्वभाव:
	    प्रत्यागम 0;
      }
}

स्थिर vpiHandle pathterm_get_handle(पूर्णाङ्क code, vpiHandle ref)
{
      संरचना __vpiModPathTerm*obj = dynamic_cast<__vpiModPathTerm*>(ref);
      प्रतिज्ञा(obj);

      विकल्प (code) {
	  पक्ष vpi_अभिव्यक्ती:
	    प्रत्यागम obj->अभिव्यक्ती;
	  स्वभाव:
	    प्रत्यागम 0;
      }
}

/*
* The __vpiModPathSrc class is what the VPI client sees as a
* vpiModPath object. The __vpiModPath structure contains items that
* are common to a bunch of modpaths, including the destination term.
*/
अन्तःस्थ __vpiModPathSrc::__vpiModPathSrc()
{ }

पूर्णाङ्क __vpiModPathSrc::get_type_code(शून्य) ध्रुव
{ प्रत्यागम vpiModPath; }

पूर्णाङ्क __vpiModPathSrc::vpi_get(पूर्णाङ्क code)
{ प्रत्यागम modpath_src_get(code, एतत्); }

शून्य __vpiModPathSrc::vpi_get_मूल्य(p_vpi_मूल्य val)
{ modpath_src_get_मूल्य(एतत्, val); }

vpiHandle __vpiModPathSrc::vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णाङ्क flags)
{ प्रत्यागम modpath_src_put_मूल्य(एतत्, val, flags); }

vpiHandle __vpiModPathSrc::vpi_handle(पूर्णाङ्क code)
{ प्रत्यागम modpath_src_get_handle(code, एतत्); }

vpiHandle __vpiModPathSrc::vpi_iterate(पूर्णाङ्क code)
{ प्रत्यागम modpath_src_iterate(code, एतत्); }

vpiHandle __vpiModPathSrc:: vpi_index(पूर्णाङ्क idx)
{ प्रत्यागम modpath_src_index(एतत्, idx); }

शून्य __vpiModPathSrc::vpi_get_delays(p_vpi_delay del)
{ modpath_src_get_delays(एतत्, del); }

शून्य __vpiModPathSrc::vpi_put_delays(p_vpi_delay del)
{ modpath_src_put_delays(एतत्, del); }

स्थिर पूर्णाङ्क modpath_src_free_object( vpiHandle ref )
{
      delete ref;
      प्रत्यागम 1 ;
}

__vpiHandle::free_object_fun_t __vpiModPathSrc::free_object_fun(शून्य)
{ प्रत्यागम &modpath_src_free_object; }


अन्तःस्थ __vpiModPathTerm::__vpiModPathTerm()
{ }

पूर्णाङ्क __vpiModPathTerm::get_type_code(शून्य) ध्रुव
{ प्रत्यागम vpiPathTerm; }

पूर्णाङ्क __vpiModPathTerm::vpi_get(पूर्णाङ्क code)
{ प्रत्यागम pathterm_get(code, एतत्); }

vpiHandle __vpiModPathTerm::vpi_handle(पूर्णाङ्क code)
{ प्रत्यागम pathterm_get_handle(code, एतत्); }

स्थिर शून्य initialize_path_term(संरचना __vpiModPathTerm&obj)
{
      obj.अभिव्यक्ती = 0;
      obj.edge = vpiNoEdge;
}

/*
 * This function will construct a vpiModPath Object.
 * give a respective "net", and will point to his
 * respective functor
 */

#ifdef CHECK_WITH_VALGRIND
स्थिर संरचना __vpiModPath**mp_list = 0;
स्थिर unsigned mp_मोजणी = 0;
#endif

संरचना __vpiModPath* vpip_make_modpath(vvp_net_t *net)
{
      संरचना __vpiModPath*obj = नूतन __vpiModPath;
      obj->scope = vpip_peek_current_scope ( );

      initialize_path_term(obj->path_term_out);
      obj->आदान_net = net ;

#ifdef CHECK_WITH_VALGRIND
      mp_मोजणी += 1;
      mp_list = static_cast<संरचना __vpiModPath **>
                (realloc(mp_list,
                 mp_मोजणी*परिमाण(संरचना __vpiModPath **)));
      mp_list[mp_मोजणी-1] = obj;
#endif
      प्रत्यागम obj;
}

#ifdef CHECK_WITH_VALGRIND
शून्य modpath_delete()
{
      प्रति (unsigned idx = 0; idx < mp_मोजणी; idx += 1) {
	    delete mp_list[idx];
      }
      मुक्त(mp_list);
      mp_list = 0;
      mp_मोजणी = 0;
}
#endif

/*
 * This function will construct a vpiModPathIn
 * ( struct __vpiModPathSrc ) Object. will give
 * a delays[12] values, and point to the specified functor
 *
 */

संरचना __vpiModPathSrc* vpip_make_modpath_src(संरचना __vpiModPath*path,
                                              vvp_net_t *net)
{
      संरचना __vpiModPathSrc *obj = नूतन __vpiModPathSrc;

      obj->dest = path;
      obj->type = 0;
      obj->net = net;
      initialize_path_term(obj->path_term_in);

      प्रत्यागम obj;
}

vvp_fun_intermodpath::vvp_fun_intermodpath(vvp_net_t*net, unsigned width)
: net_(net)
{
      प्रति (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = 0;

      cur_vec4_ = vvp_vector4_t(width, BIT4_X);
      schedule_init_propagate(net_, cur_vec4_);
}

vvp_fun_intermodpath::~vvp_fun_intermodpath()
{
}

शून्य vvp_fun_intermodpath::get_delay12(vvp_time64_t val[12]) ध्रुव
{
      प्रति (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    val[idx] = delay_[idx];
}

शून्य vvp_fun_intermodpath::put_delay12(ध्रुव vvp_time64_t val[12])
{
      प्रति (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = val[idx];
}

/*
 * FIXME: this implementation currently only uses the maximum delay
 * from all the bit changes in the vectors. If there are multiple
 * changes with different delays, then the results would be
 * wrong. What should happen is that if there are multiple changes,
 * multiple vectors approaching the result should be scheduled.
 */
शून्य vvp_fun_intermodpath::recv_vec4(vvp_net_ptr_t port, ध्रुव vvp_vector4_t&bit,
                              vvp_context_t)
{
	/* Only the first port is used. */
      यदि (port.port() > 0)
	    प्रत्यागम;

      यदि (cur_vec4_.eeq(bit))
	    प्रत्यागम;

	/* Given the scheduled output time, create an output event. */
      vvp_time64_t use_delay = delay_from_edge(cur_vec4_.value(0),
					       bit.value(0),
					       delay_);

	/* FIXME: This bases the edge delay on only the least
	   bit. This is WRONG! I need to find all the possible delays,
	   and schedule an event for each partial change. Hard! */
      प्रति (unsigned idx = 1 ;  idx < bit.size() ;  idx += 1) {
	    vvp_time64_t tmp = delay_from_edge(cur_vec4_.value(idx),
					       bit.value(idx),
					       delay_);
	      /* If the current and new bit values match then no delay
	       * is needed for this bit. */
	    यदि (cur_vec4_.value(idx) == bit.value(idx)) अनुवर्त;
	    प्रतिज्ञा(tmp == use_delay);
      }

      cur_vec4_ = bit;
      schedule_generic(एतत्, use_delay, असत्य);
}

शून्य vvp_fun_intermodpath::run_run()
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
स्थिर पूर्णाङ्क intermodpath_get(पूर्णाङ्क, vpiHandle ref)
{
      संरचना __vpiInterModPath*obj =dynamic_cast<__vpiInterModPath*>(ref);
      प्रतिज्ञा(obj);
      प्रत्यागम 0;
}

स्थिर शून्य intermodpath_get_मूल्य(vpiHandle ref, p_vpi_मूल्य)
{
      संरचना __vpiInterModPath* intermodpath = dynamic_cast<__vpiInterModPath*>(ref);
      प्रतिज्ञा(intermodpath);
      प्रत्यागम;
}

स्थिर vpiHandle intermodpath_put_मूल्य(vpiHandle ref, s_vpi_मूल्य *, पूर्णाङ्क )
{
      संरचना __vpiInterModPath* intermodpath = dynamic_cast<__vpiInterModPath*>(ref);
      प्रतिज्ञा(intermodpath);
      प्रत्यागम 0;
}

स्थिर vpiHandle intermodpath_get_handle(पूर्णाङ्क code, vpiHandle ref)
{
      संरचना __vpiInterModPath*rfp = dynamic_cast<__vpiInterModPath*>(ref);
      प्रतिज्ञा(rfp);

      विकल्प (code) {

	पक्ष vpiScope:
	  प्रत्यागम rfp->scope;

	  पक्ष vpiModule:
	      { __vpiScope*scope = rfp->scope;
		यावत् (scope && scope->get_type_code() != vpiModule)
		      scope = scope->scope;
		प्रतिज्ञा(scope);
		प्रत्यागम scope;
	      }
      }
      प्रत्यागम 0;
}

स्थिर vpiHandle intermodpath_iterate(पूर्णाङ्क code, vpiHandle ref)
{
      संरचना __vpiInterModPath*rfp = dynamic_cast<__vpiInterModPath*>(ref);
      प्रतिज्ञा(rfp);

	// For now intermodpaths only support exactly two ports
      विकल्प (code) {
	  पक्ष vpiPort: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(2, परिमाण(vpiHandle*)));
	    args[0] = rfp->port1;
	    args[1] = rfp->port2;
	    प्रत्यागम vpip_make_iterator(2, args, सत्य);
	  }
      }
      प्रत्यागम 0;
}

/*
 * This routine will put specific dimension of delay[] values
 * into a vpiHandle. In this case, we will put
 * specific delays values in a vpiInterModPath object
 *
 */
स्थिर शून्य intermodpath_put_delays (vpiHandle ref, p_vpi_delay delays)
{
      vvp_time64_t tmp[12];
      पूर्णाङ्क idx;
      संरचना __vpiInterModPath * src = dynamic_cast<__vpiInterModPath*>(ref) ;
      प्रतिज्ञा(src) ;

      vvp_fun_intermodpath *fun = dynamic_cast<vvp_fun_intermodpath*>(src->net->fun);
      प्रतिज्ञा( fun );

      प्रकारनिर्देश unsigned वर्ण map_array_t[12];
	// Only the first six entries are used for the less than twelve maps.
      स्थिर ध्रुव map_array_t map_1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      स्थिर ध्रुव map_array_t map_2 = {0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0};
      स्थिर ध्रुव map_array_t map_3 = {0, 1, 2, 0, 2, 1, 0, 0, 0, 0, 0, 0};
      स्थिर ध्रुव map_array_t map_6 = {0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0};
      स्थिर ध्रुव map_array_t map12 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

      ध्रुव map_array_t*use_map = 0;
      विकल्प (delays->no_of_delays) {
	  पक्ष 1:
	    use_map = &map_1;
	    भङ्ग;
	  पक्ष 2:
	    use_map = &map_2;
	    भङ्ग;
	  पक्ष 3:
	    use_map = &map_3;
	    भङ्ग;
	  पक्ष 6:
	    use_map = &map_6;
	    भङ्ग;
	  पक्ष 12:
	    use_map = &map12;
	    भङ्ग;
	  स्वभाव:
	    प्रतिज्ञा(0);
	    भङ्ग;
      }

      यदि (delays->time_type == vpiSimTime) {
	    प्रति (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_timestruct_to_time(delays->da+use_map[0][idx]);
	    }
      } अन्यथा {
	      // You cannot create a modpath with a negative delay so set it
	      // to zero per 1364-2005 section 14.3.1.
	    प्रति (idx = 0 ; idx < delays->no_of_delays ; idx += 1) {
		  यदि (delays->da[idx].real < 0.0) delays->da[idx].real = 0.0;
	    }
	    प्रति (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_scaled_real_to_time64(delays->da[use_map[0][idx]].real,
							src->scope);
	    }
      }

      /* Now define the to-from-x delays if needed. */
      यदि (delays->no_of_delays <= 6) {
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

स्थिर शून्य intermodpath_get_delays ( vpiHandle ref, p_vpi_delay delays )
{
      संरचना __vpiInterModPath*src = dynamic_cast<__vpiInterModPath*>(ref) ;
      प्रतिज्ञा(src);

      vvp_fun_intermodpath *fun = dynamic_cast<vvp_fun_intermodpath*>(src->net->fun);
      प्रतिज्ञा(fun);

      पूर्णाङ्क idx;
      vvp_time64_t tmp[12];
      fun->get_delay12(tmp);

      विकल्प (delays->no_of_delays) {
	  पक्ष 1:
	  पक्ष 2:
	  पक्ष 3:
	  पक्ष 6:
	  पक्ष 12:
	    भङ्ग;

	  स्वभाव:
	    प्रतिज्ञा(0);
	    भङ्ग;
      }

      यदि (delays->time_type == vpiSimTime) {
	    प्रति (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  vpip_time_to_timestruct(delays->da+idx, tmp[idx]);
	    }
      } अन्यथा {
	    प्रति (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  delays->da[idx].real = vpip_time_to_scaled_real(tmp[idx], src->scope);
	    }
      }
}

/*
* The __vpiInterModPath class is what the VPI client sees as a
* vpiInterModPath object.
*/
अन्तःस्थ __vpiInterModPath::__vpiInterModPath()
{ }

पूर्णाङ्क __vpiInterModPath::get_type_code(शून्य) ध्रुव
{ प्रत्यागम vpiInterModPath; }

पूर्णाङ्क __vpiInterModPath::vpi_get(पूर्णाङ्क code)
{ प्रत्यागम intermodpath_get(code, एतत्); }

शून्य __vpiInterModPath::vpi_get_मूल्य(p_vpi_मूल्य val)
{ intermodpath_get_मूल्य(एतत्, val); }

vpiHandle __vpiInterModPath::vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णाङ्क flags)
{ प्रत्यागम intermodpath_put_मूल्य(एतत्, val, flags); }

vpiHandle __vpiInterModPath::vpi_handle(पूर्णाङ्क code)
{ प्रत्यागम intermodpath_get_handle(code, एतत्); }

vpiHandle __vpiInterModPath::vpi_iterate(पूर्णाङ्क code)
{ प्रत्यागम intermodpath_iterate(code, एतत्); }

शून्य __vpiInterModPath::vpi_get_delays(p_vpi_delay del)
{ intermodpath_get_delays(एतत्, del); }

शून्य __vpiInterModPath::vpi_put_delays(p_vpi_delay del)
{ intermodpath_put_delays(एतत्, del); }

स्थिर पूर्णाङ्क intermodpath_free_object( vpiHandle ref )
{
      delete ref;
      प्रत्यागम 1 ;
}

__vpiHandle::free_object_fun_t __vpiInterModPath::free_object_fun(शून्य)
{ प्रत्यागम &intermodpath_free_object; }

/*
 * This function will construct a vpiInterModPath Object.
 * give a respective "net", and will point to his
 * respective functor
 */

#ifdef CHECK_WITH_VALGRIND
स्थिर संरचना __vpiInterModPath**imp_list = 0;
स्थिर unsigned imp_मोजणी = 0;
#endif

संरचना __vpiInterModPath* vpip_make_intermodpath(vvp_net_t *net, vpiPortInfo* port1, vpiPortInfo* port2)
{
      संरचना __vpiInterModPath*obj = नूतन __vpiInterModPath;
      obj->scope = vpip_peek_current_scope ( );

      obj->net = net;
      obj->port1 = port1;
      obj->port2 = port2;

#ifdef CHECK_WITH_VALGRIND
      imp_मोजणी += 1;
      imp_list = static_cast<संरचना __vpiInterModPath **>
                 (realloc(imp_list,
                  imp_मोजणी*परिमाण(संरचना __vpiInterModPath **)));
      imp_list[imp_मोजणी-1] = obj;
#endif
      प्रत्यागम obj;
}

#ifdef CHECK_WITH_VALGRIND
शून्य intermodpath_delete()
{
      प्रति (unsigned idx = 0; idx < mp_मोजणी; idx += 1) {
	    delete imp_list[idx];
      }
      मुक्त(imp_list);
      imp_list = 0;
      imp_मोजणी = 0;
}
#endif
