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

#ضمن "delay.h"
#ضمن "schedule.h"
#ضمن "vpi_priv.h"
#ضمن "config.h"
#ifdef CHECK_WITH_VALGRIND
#ضمن "vvp_cleanup.h"
#endif
#ضمن <iostream>
#ضمن <cstdlib>
#ضمن <list>
#ضمن <cassert>
#ضمن <cmath>
#ضمن "ivl_alloc.h"

using نطاق std;

فراغ vvp_delay_t::calculate_min_delay_()
{
      min_delay_ = rise_;
      אם (fall_ < min_delay_)
	    min_delay_ = fall_;
      אם (ignore_decay_) decay_ = min_delay_;
      אחרת אם (decay_ < min_delay_)
	    min_delay_ = decay_;
}

vvp_delay_t::vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall)
{
      rise_ = rise;
      fall_ = fall;
      decay_= fall < rise? fall : rise;
      min_delay_ = decay_;
      ignore_decay_ = שקר;
}

vvp_delay_t::vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall, vvp_time64_t decay)
{
      rise_ = rise;
      fall_ = fall;
      decay_= decay;
      ignore_decay_ = שקר;

      calculate_min_delay_();
}

فراغ vvp_delay_t::set_ignore_decay()
{
      ignore_decay_ = אמת;

      calculate_min_delay_();
}

vvp_delay_t::~vvp_delay_t()
{
}

vvp_time64_t vvp_delay_t::get_delay(vvp_bit4_t from, vvp_bit4_t to)
{
      بدل (from) {
	  حالة BIT4_0:
	    بدل (to) {
		حالة BIT4_0: החזר 0;
		حالة BIT4_1: החזר rise_;
		حالة BIT4_X: החזר min_delay_;
		حالة BIT4_Z: החזר decay_;
	    }
	    اكسر;
	  حالة BIT4_1:
	    بدل (to) {
		حالة BIT4_0: החזר fall_;
		حالة BIT4_1: החזר 0;
		حالة BIT4_X: החזר min_delay_;
		حالة BIT4_Z: החזר decay_;
	    }
	    اكسر;
	  حالة BIT4_X:
	    بدل (to) {
		حالة BIT4_0: החזר fall_;
		حالة BIT4_1: החזר rise_;
		حالة BIT4_X: החזר 0;
		حالة BIT4_Z: החזר decay_;
	    }
	    اكسر;
	  حالة BIT4_Z:
	    بدل (to) {
		حالة BIT4_0: החזר fall_;
		حالة BIT4_1: החזר rise_;
		حالة BIT4_X: החזר min_delay_;
		حالة BIT4_Z: החזר 0;
	    }
	    اكسر;
      }

      أكد(0);
      החזר 0;
}

vvp_time64_t vvp_delay_t::get_min_delay() ثابت
{
      החזר min_delay_;
}

فراغ vvp_delay_t::set_rise(vvp_time64_t val)
{
      rise_ = val;
      אם (val < min_delay_) {
	    min_delay_ = val;
	    אם (ignore_decay_) decay_ = val;
      } אחרת
	    calculate_min_delay_();
}

فراغ vvp_delay_t::set_fall(vvp_time64_t val)
{
      fall_ = val;
      אם (val < min_delay_) {
	    min_delay_ = val;
	    אם (ignore_decay_) decay_ = val;
      } אחרת
	    calculate_min_delay_();
}

فراغ vvp_delay_t::set_decay(vvp_time64_t val)
{
      أكد(!ignore_decay_);

      decay_ = val;
      אם (val < min_delay_)
	    min_delay_ = val;
      אחרת
	    calculate_min_delay_();
}

vvp_fun_delay::vvp_fun_delay(vvp_net_t*n, unsigned width, ثابت vvp_delay_t&d)
: net_(n), delay_(d)
{
      cur_real_ = 0.0;
      אם (width > 0) {
            cur_vec4_ = vvp_vector4_t(width, BIT4_X);
            cur_vec8_ = vvp_vector8_t(cur_vec4_, 6, 6);
            schedule_init_propagate(net_, cur_vec4_);
      } אחרת {
            schedule_init_propagate(net_, cur_real_);
      }
      list_ = 0;
      type_ = UNKNOWN_DELAY;
      initial_ = אמת;
	// Calculate the values used when converting variable delays
	// to simulation time units.
      ثابت __vpiScope*scope = vpip_peek_current_scope();

      صحيح powr = scope->time_units - scope->time_precision;
      round_ = 1;
      עבור (صحيح lp = 0; lp < powr; lp += 1) round_ *= 10;

      powr = scope->time_precision - vpip_get_time_precision();
      scale_ = 1;
      עבור (صحيح lp = 0; lp < powr; lp += 1) scale_ *= 10;
}

vvp_fun_delay::~vvp_fun_delay()
{
      כלעוד (بنية event_*cur = dequeue_())
	    delete cur;
}

منطقي vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        ثابت vvp_vector4_t&bit)
{
      אם (list_ == 0) החזר שקר;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      אם (list_->next->ptr_vec4.eeq(bit)) החזר אמת;

      clean_pulse_events_(use_delay);
      החזר שקר;
}

منطقي vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        ثابت vvp_vector8_t&bit)
{
      אם (list_ == 0) החזר שקר;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      אם (list_->next->ptr_vec8.eeq(bit)) החזר אמת;

      clean_pulse_events_(use_delay);
      החזר שקר;
}

منطقي vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        مزدوج bit)
{
      אם (list_ == 0) החזר שקר;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      אם (list_->next->ptr_real == bit) החזר אמת;

      clean_pulse_events_(use_delay);
      החזר שקר;
}

فراغ vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay)
{
      أكد(list_ != 0);

      עשה {
	    بنية event_*cur = list_->next;
	      /* If this event is far enough from the event I'm about
	         to create, then that scheduled event is not a pulse
	         to be eliminated, so we're done. */
	    אם (cur->sim_time+use_delay <= use_delay+schedule_simtime())
		  اكسر;

	    אם (list_ == cur)
		  list_ = 0;
	    אחרת
		  list_->next = cur->next;
	    delete cur;
      } כלעוד (list_);
}

/*
 * FIXME: this implementation currently only uses the maximum delay
 * from all the bit changes in the vectors. If there are multiple
 * changes with different delays, then the results would be
 * wrong. What should happen is that if there are multiple changes,
 * multiple vectors approaching the result should be scheduled.
 */
فراغ vvp_fun_delay::recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                              vvp_context_t)
{
      אם (port.port() > 0) {
	      // Get the integer value of the bit vector, or 0 if
	      // there are X or Z bits.
	    vvp_time64_t bval = 0;
	      // The following does not work correctly for negative values.
	      // They should be sign extended to 64 bits (1364-2001 9.7.1).
	    vector4_to_value(bit, bval);
	      // Integer values do not need to be rounded so just scale them.
	    vvp_time64_t val = bval * round_ * scale_;

	    بدل (port.port()) {
		حالة 1:
		  delay_.set_rise(val);
		  החזר;
		حالة 2:
		  delay_.set_fall(val);
		  החזר;
		حالة 3:
		  delay_.set_decay(val);
		  החזר;
	    }
	    החזר;
      }

      vvp_time64_t use_delay;
	/* This is an initial value so it needs to be compared to all the
	   bits (the order the bits are changed is not deterministic). */
      אם (initial_) {
	    type_ = VEC4_DELAY;
            cur_vec8_ = vvp_vector8_t(vvp_vector4_t(0, BIT4_X), 6, 6);
	    vvp_bit4_t cur_val = cur_vec4_.value(0);
	    use_delay = delay_.get_delay(cur_val, bit.value(0));
	    עבור (unsigned idx = 1 ;  idx < bit.size() ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(cur_val, bit.value(idx));
		  אם (tmp > use_delay) use_delay = tmp;
	    }
      } אחרת {
	    أكد(type_ == VEC4_DELAY);

	      // Use as a reference for calculating the delay the
	      // current value of the output. Detect and handle the
	      // special case that the event list contains the current
	      // value as a zero-delay-remaining event.
	    ثابت vvp_vector4_t&use_vec4 = (list_ && list_->next->sim_time == schedule_simtime())? list_->next->ptr_vec4 : cur_vec4_;

	      /* How many bits to compare? */
	    unsigned use_wid = use_vec4.size();
	    אם (bit.size() < use_wid) use_wid = bit.size();

	      /* Scan the vectors looking for delays. Select the maximum
	         delay encountered. */
	    use_delay = delay_.get_delay(use_vec4.value(0), bit.value(0));

	    עבור (unsigned idx = 1 ;  idx < use_wid ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(use_vec4.value(idx), bit.value(idx));
		  אם (tmp > use_delay) use_delay = tmp;
	    }
      }

      /* what *should* happen here is we check to see if there is a
         transaction in the queue. This would be a pulse that needs to be
         eliminated. */
      אם (clean_pulse_events_(use_delay, bit)) החזר;

      vvp_time64_t use_simtime = schedule_simtime() + use_delay;

	/* And propagate it. */
      אם (use_delay == 0 && list_ == 0) {
	    cur_vec4_ = bit;
	    initial_ = שקר;
	    net_->send_vec4(cur_vec4_, 0);
      } אחרת {
	    بنية event_*cur = جديد بنية event_(use_simtime);
	    cur->run_run_ptr = &vvp_fun_delay::run_run_vec4_;
	    cur->ptr_vec4 = bit;
	    enqueue_(cur);
	    schedule_generic(هذا, use_delay, שקר);
      }
}

فراغ vvp_fun_delay::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
			         unsigned base, unsigned vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

/* See the recv_vec4 comment above. */
فراغ vvp_fun_delay::recv_vec8(vvp_net_ptr_t port, ثابت vvp_vector8_t&bit)
{
      أكد(port.port() == 0);

      vvp_time64_t use_delay;
	/* This is an initial value so it needs to be compared to all the
	   bits (the order the bits are changed is not deterministic). */
      אם (initial_) {
	    type_ = VEC8_DELAY;
            cur_vec4_ = vvp_vector4_t(0, BIT4_X);
	    vvp_bit4_t cur_val = cur_vec8_.value(0).value();
	    use_delay = delay_.get_delay(cur_val, bit.value(0).value());
	    עבור (unsigned idx = 1 ;  idx < bit.size() ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(cur_val, bit.value(idx).value());
		  אם (tmp > use_delay) use_delay = tmp;
	    }
      } אחרת {
	    أكد(type_ == VEC8_DELAY);

	      // Use as a reference for calculating the delay the
	      // current value of the output. Detect and handle the
	      // special case that the event list contains the current
	      // value as a zero-delay-remaining event.
	    ثابت vvp_vector8_t&use_vec8 = (list_ && list_->next->sim_time == schedule_simtime())? list_->next->ptr_vec8 : cur_vec8_;

	      /* How many bits to compare? */
	    unsigned use_wid = use_vec8.size();
	    אם (bit.size() < use_wid) use_wid = bit.size();

	      /* Scan the vectors looking for delays. Select the maximum
	         delay encountered. */
	    use_delay = delay_.get_delay(use_vec8.value(0).value(),
	                                 bit.value(0).value());

	    עבור (unsigned idx = 1 ;  idx < use_wid ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(use_vec8.value(idx).value(),
		                         bit.value(idx).value());
		  אם (tmp > use_delay) use_delay = tmp;
	    }
      }

      /* what *should* happen here is we check to see if there is a
         transaction in the queue. This would be a pulse that needs to be
         eliminated. */
      אם (clean_pulse_events_(use_delay, bit)) החזר;

      vvp_time64_t use_simtime = schedule_simtime() + use_delay;

	/* And propagate it. */
      אם (use_delay == 0 && list_ == 0) {
	    cur_vec8_ = bit;
	    initial_ = שקר;
	    net_->send_vec8(cur_vec8_);
      } אחרת {
	    بنية event_*cur = جديد بنية event_(use_simtime);
	    cur->ptr_vec8 = bit;
	    cur->run_run_ptr = &vvp_fun_delay::run_run_vec8_;
	    enqueue_(cur);
	    schedule_generic(هذا, use_delay, שקר);
      }
}

فراغ vvp_fun_delay::recv_vec8_pv(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&bit,
			         unsigned base, unsigned vwid)
{
      recv_vec8_pv_(ptr, bit, base, vwid);
}

فراغ vvp_fun_delay::recv_real(vvp_net_ptr_t port, مزدوج bit,
                              vvp_context_t)
{
      אם (port.port() > 0) {
	    /* If the port is not 0, then this is a delay value that
	    should be rounded and converted to an integer delay. */
	    vvp_time64_t val = 0;
	    אם (bit > -0.5) {
		  val = (vvp_time64_t) (bit * round_ + 0.5) * scale_;
	    } אחרת אם (bit != bit) {
		    // For a NaN we use the default (0).
	    } אחרת {
		  vvp_vector4_t vec4(8*حجم(vvp_time64_t),
		                     floor(-bit * round_ + 0.5) * -1 * scale_);
		  vector4_to_value(vec4, val);
	    }

	    بدل (port.port()) {
		حالة 1:
		  delay_.set_rise(val);
		  החזר;
		حالة 2:
		  delay_.set_fall(val);
		  החזר;
		حالة 3:
		  delay_.set_decay(val);
		  החזר;
	    }
	    החזר;
      }

      אם (initial_) {
	    type_ = REAL_DELAY;
            cur_vec4_ = vvp_vector4_t(0, BIT4_X);
            cur_vec8_ = vvp_vector8_t(cur_vec4_, 6, 6);
      } אחרת أكد(type_ == REAL_DELAY);

      vvp_time64_t use_delay;
      use_delay = delay_.get_min_delay();

      /* Eliminate glitches. */
      אם (clean_pulse_events_(use_delay, bit)) החזר;

      /* This must be done after cleaning pulses to avoid propagating
       * an incorrect value. */
      אם (cur_real_ == bit) החזר;

      vvp_time64_t use_simtime = schedule_simtime() + use_delay;

      אם (use_delay == 0 && list_ == 0) {
	    cur_real_ = bit;
	    initial_ = שקר;
	    net_->send_real(cur_real_, 0);
      } אחרת {
	    بنية event_*cur = جديد بنية event_(use_simtime);
	    cur->run_run_ptr = &vvp_fun_delay::run_run_real_;
	    cur->ptr_real = bit;
	    enqueue_(cur);

	    schedule_generic(هذا, use_delay, שקר);
      }
}

فراغ vvp_fun_delay::run_run()
{
      vvp_time64_t sim_time = schedule_simtime();
      אם (list_ == 0 || list_->next->sim_time > sim_time)
	    החזר;

      بنية event_*cur = dequeue_();
      אם (cur == 0)
	    החזר;

      (هذا->*(cur->run_run_ptr))(cur);
      initial_ = שקר;
      delete cur;
}

فراغ vvp_fun_delay::run_run_vec4_(ثابت بنية event_*cur)
{
      cur_vec4_ = cur->ptr_vec4;
      net_->send_vec4(cur_vec4_, 0);
}

فراغ vvp_fun_delay::run_run_vec8_(ثابت بنية vvp_fun_delay::event_*cur)
{
      cur_vec8_ = cur->ptr_vec8;
      net_->send_vec8(cur_vec8_);
}

فراغ vvp_fun_delay::run_run_real_(ثابت بنية vvp_fun_delay::event_*cur)
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
      כלעוד (src_list_) {
	    vvp_fun_modpath_src*tmp = src_list_;
	    src_list_ = tmp->next_;
	    delete tmp;
      }
      כלעוד (ifnone_list_) {
	    vvp_fun_modpath_src*tmp = ifnone_list_;
	    ifnone_list_ = tmp->next_;
	    delete tmp;
      }
}

فراغ vvp_fun_modpath::add_modpath_src(vvp_fun_modpath_src*that, منطقي ifnone)
{
      أكد(that->next_ == 0);
      אם (ifnone) {
	    that->next_ = ifnone_list_;
	    ifnone_list_ = that;
      } אחרת {
	    that->next_ = src_list_;
	    src_list_ = that;
      }
}

ساكن vvp_time64_t delay_from_edge(vvp_bit4_t a, vvp_bit4_t b,
                                           vvp_time64_t ثابت array[12])
{
      تعريفنوع delay_edge_t bit4_table4[4];
      ساكن ثابت bit4_table4 edge_table[4] = {
	    { DELAY_EDGE_01, DELAY_EDGE_01, DELAY_EDGE_0z, DELAY_EDGE_0x },
	    { DELAY_EDGE_10, DELAY_EDGE_10, DELAY_EDGE_1z, DELAY_EDGE_1x },
	    { DELAY_EDGE_z0, DELAY_EDGE_z1, DELAY_EDGE_z0, DELAY_EDGE_zx },
	    { DELAY_EDGE_x0, DELAY_EDGE_x1, DELAY_EDGE_xz, DELAY_EDGE_x0 }
      };

      החזר array[ edge_table[a][b] ];
}

فراغ vvp_fun_modpath::recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                                vvp_context_t)
{
	/* Only the first port is used. */
      אם (port.port() > 0)
	    החזר;

      אם (cur_vec4_.eeq(bit))
	    החזר;

	/* Select a time delay source that applies. Notice that there
	   may be multiple delay sources that apply, so collect all
	   the candidates into a list first. */
      list<vvp_fun_modpath_src*>candidate_list;
      vvp_time64_t candidate_wake_time = 0;
      עבור (vvp_fun_modpath_src*cur = src_list_ ;  cur ;  cur=cur->next_) {
	      /* Skip paths that are disabled by conditions. */
	    אם (cur->condition_flag_ == שקר)
		  استمر;

	    אם (candidate_list.empty()) {
		  candidate_list.push_back(cur);
		  candidate_wake_time = cur->wake_time_;
	    } אחרת אם (cur->wake_time_ == candidate_wake_time) {
		  candidate_list.push_back(cur);
	    } אחרת אם (cur->wake_time_ > candidate_wake_time) {
		  candidate_list.assign(1, cur);
		  candidate_wake_time = cur->wake_time_;
	    } אחרת {
		  استمر; /* Skip this entry. */
	    }
      }

	/* Only add the ifnone delay if it has a later wake_time_ or
	 * if there are no normal delays. */
      vvp_time64_t ifnone_wake_time = candidate_wake_time;
      עבור (vvp_fun_modpath_src*cur = ifnone_list_ ;  cur ;  cur=cur->next_) {
	    אם (candidate_list.empty()) {
		  candidate_list.push_back(cur);
		  ifnone_wake_time = cur->wake_time_;
	    } אחרת אם (cur->wake_time_ == ifnone_wake_time &&
	               ifnone_wake_time > candidate_wake_time) {
		  candidate_list.push_back(cur);
	    } אחרת אם (cur->wake_time_ > ifnone_wake_time) {
		  candidate_list.assign(1, cur);
		  ifnone_wake_time = cur->wake_time_;
	    } אחרת {
		  استمر; /* Skip this entry. */
	    }
      }

	/* Handle the special case that there are no delays that
	   match. This may happen, for example, if the set of
	   conditional delays is incomplete, leaving some cases
	   uncovered. In that case, just pass the data without delay */
      אם (candidate_list.empty()) {
	    cur_vec4_ = bit;
	    schedule_generic(هذا, 0, שקר);
	    החזר;
      }

	/* Now given that we have a list of candidate delays, find for
	   each if the 12 numbers the minimum from all the
	   candidates. This minimum set becomes the chosen delay to
	   use. */
      vvp_time64_t out_at[12];
      vvp_time64_t now = schedule_simtime();

      تعريفنوع list<vvp_fun_modpath_src*>::const_iterator iter_t;

      iter_t cur = candidate_list.begin();
      ثابت vvp_fun_modpath_src*src = *cur;

      עבור (unsigned idx = 0 ;  idx < 12 ;  idx += 1) {
	    out_at[idx] = src->wake_time_ + src->delay_[idx];
	    אם (out_at[idx] <= now)
		  out_at[idx] = 0;
	    אחרת
		  out_at[idx] -= now;
      }

      עבור (++ cur ; cur != candidate_list.end() ; ++ cur ) {
	    src = *cur;
	    עבור (unsigned idx = 0 ;  idx < 12 ;  idx += 1) {
		  vvp_time64_t tmp = src->wake_time_ + src->delay_[idx];
		  אם (tmp <= now)
			tmp = 0;
		  אחרת
			tmp -= now;
		  אם (tmp < out_at[idx])
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
      עבור (unsigned idx = 1 ;  idx < bit.size() ;  idx += 1) {
	    vvp_time64_t tmp = delay_from_edge(cur_vec4_.value(idx),
					       bit.value(idx),
					       out_at);
	      /* If the current and new bit values match then no delay
	       * is needed for this bit. */
	    אם (cur_vec4_.value(idx) == bit.value(idx)) استمر;
	    אם (tmp != use_delay) {
		  fprintf(stderr, "sorry: multi-bit module path delays are "
		                  "currently not fully supported.\n");
		  fprintf(stderr, "     : using the LSB delay for all bits.\n");
		  استمر;
            }
      }

      cur_vec4_ = bit;
      schedule_generic(هذا, use_delay, שקר);
}

فراغ vvp_fun_modpath::run_run()
{
      net_->send_vec4(cur_vec4_, 0);
}

vvp_fun_modpath_src::vvp_fun_modpath_src(vvp_time64_t ثابت del[12])
{
      עבור (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = del[idx];

      next_ = 0;
      wake_time_ = 0;
      condition_flag_ = אמת;
}

vvp_fun_modpath_src::~vvp_fun_modpath_src()
{
}

فراغ vvp_fun_modpath_src::get_delay12(vvp_time64_t val[12]) ثابت
{
      עבור (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    val[idx] = delay_[idx];
}

فراغ vvp_fun_modpath_src::put_delay12(ثابت vvp_time64_t val[12])
{
      עבור (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = val[idx];
}

فراغ vvp_fun_modpath_src::recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                                    vvp_context_t)
{
      אם (port.port() == 0) {
	      // The modpath input...
	    אם (test_vec4(bit))
		  wake_time_ = schedule_simtime();

      } אחרת אם (port.port() == 1) {
	      // The modpath condition input...
	    אם (bit.value(0) != BIT4_0)
		  condition_flag_ = אמת;
	    אחרת
		  condition_flag_ = שקר;
      }
}

منطقي vvp_fun_modpath_src::test_vec4(ثابت vvp_vector4_t&)
{
      החזר אמת;
}

vvp_fun_modpath_edge::vvp_fun_modpath_edge(vvp_time64_t del[12],
					   منطقي pos, منطقي neg)
: vvp_fun_modpath_src(del)
{
      old_value_ = BIT4_X;
      posedge_ = pos;
      negedge_ = neg;
}

منطقي vvp_fun_modpath_edge::test_vec4(ثابت vvp_vector4_t&bit)
{
      vvp_bit4_t tmp = old_value_;
      old_value_ = bit.value(0);

      صحيح edge_flag = edge(tmp, old_value_);
      אם (edge_flag > 0) החזר posedge_;
      אם (edge_flag < 0) החזר negedge_;
      החזר שקר;
}


/*
 * All the below routines that begin with
 * modpath_src_* belong the internal function
 * of an vpiModPathIn object. This is used to
 * make some specific delays path operations
 *
 */
ساكن صحيح modpath_src_get(صحيح, vpiHandle ref)
{
      بنية __vpiModPathSrc*obj =dynamic_cast<__vpiModPathSrc*>(ref);
      أكد(obj);
      החזר 0;
}

ساكن فراغ modpath_src_get_value(vpiHandle ref, p_vpi_value)
{
      بنية __vpiModPathSrc* modpathsrc = dynamic_cast<__vpiModPathSrc*>(ref);
      أكد(modpathsrc);
      החזר;
}

ساكن vpiHandle modpath_src_put_value(vpiHandle ref, s_vpi_value *, صحيح )
{
      بنية __vpiModPathSrc* modpathsrc = dynamic_cast<__vpiModPathSrc*>(ref);
      أكد(modpathsrc);
      החזר 0;
}

ساكن vpiHandle modpath_src_get_handle(صحيح code, vpiHandle ref)
{
      بنية __vpiModPathSrc*rfp = dynamic_cast<__vpiModPathSrc*>(ref);
      أكد(rfp);

      بدل (code) {

	حالة vpiScope:
	  החזר rfp->dest->scope;

	  حالة vpiModule:
	      { __vpiScope*scope = rfp->dest->scope;
		כלעוד (scope && scope->get_type_code() != vpiModule)
		      scope = scope->scope;
		أكد(scope);
		החזר scope;
	      }

	    // Handles to path term objects should really be obtained via
	    // the vpi_iterate and vpi_scan functions. Continue to allow
	    // them to be obtained here for backwards compatibility with
	    // older versions of Icarus Verilog.

	  حالة vpiModPathIn:
	    החזר &rfp->path_term_in;

	  حالة vpiModPathOut:
	    החזר &rfp->dest->path_term_out;
      }
      החזר 0;
}

ساكن vpiHandle modpath_src_iterate(صحيح code, vpiHandle ref)
{
      بنية __vpiModPathSrc*rfp = dynamic_cast<__vpiModPathSrc*>(ref);
      أكد(rfp);

	// Module paths with multiple sources or destinations are
	// currently represented by a separate modpath object for
	// each source/destination combination, so there is only
	// ever one input path term and one output path term.
      بدل (code) {
	  حالة vpiModPathIn: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(1, حجم(vpiHandle*)));
	    args[0] = &rfp->path_term_in;
	    החזר vpip_make_iterator(1, args, אמת);
	  }
	  حالة vpiModPathOut: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(1, حجم(vpiHandle*)));
	    args[0] = &rfp->dest->path_term_out;
	    החזר vpip_make_iterator(1, args, אמת);
	  }
      }
      החזר 0;
}

ساكن vpiHandle modpath_src_index ( vpiHandle ref, صحيح)
{
      أكد(ref->get_type_code() == vpiModPathIn);
      החזר 0;
}


/*
 * This routine will put specific dimension of delay[] values
 * into a vpiHandle. In this case, we will put
 * specific delays values in a vpiModPathIn object
 *
 */
ساكن فراغ modpath_src_put_delays (vpiHandle ref, p_vpi_delay delays)
{
      vvp_time64_t tmp[12];
      صحيح idx;
      بنية __vpiModPathSrc * src = dynamic_cast<__vpiModPathSrc*>(ref) ;
      أكد(src) ;

      vvp_fun_modpath_src *fun = dynamic_cast<vvp_fun_modpath_src*>(src->net->fun);
      أكد( fun );

      تعريفنوع unsigned حرف map_array_t[12];
	// Only the first six entries are used for the less than twelve maps.
      ساكن ثابت map_array_t map_1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      ساكن ثابت map_array_t map_2 = {0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0};
      ساكن ثابت map_array_t map_3 = {0, 1, 2, 0, 2, 1, 0, 0, 0, 0, 0, 0};
      ساكن ثابت map_array_t map_6 = {0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0};
      ساكن ثابت map_array_t map12 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

      ثابت map_array_t*use_map = 0;
      بدل (delays->no_of_delays) {
	  حالة 1:
	    use_map = &map_1;
	    اكسر;
	  حالة 2:
	    use_map = &map_2;
	    اكسر;
	  حالة 3:
	    use_map = &map_3;
	    اكسر;
	  حالة 6:
	    use_map = &map_6;
	    اكسر;
	  حالة 12:
	    use_map = &map12;
	    اكسر;
	  افتراضي:
	    أكد(0);
	    اكسر;
      }

      אם (delays->time_type == vpiSimTime) {
	    עבור (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_timestruct_to_time(delays->da+use_map[0][idx]);
	    }
      } אחרת {
	      // You cannot create a modpath with a negative delay so set it
	      // to zero per 1364-2005 section 14.3.1.
	    עבור (idx = 0 ; idx < delays->no_of_delays ; idx += 1) {
		  אם (delays->da[idx].real < 0.0) delays->da[idx].real = 0.0;
	    }
	    עבור (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_scaled_real_to_time64(delays->da[use_map[0][idx]].real,
							src->dest->scope);
	    }
      }

      /* Now define the to-from-x delays if needed. */
      אם (delays->no_of_delays <= 6) {
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

ساكن فراغ modpath_src_get_delays ( vpiHandle ref, p_vpi_delay delays )
{
      بنية __vpiModPathSrc*src = dynamic_cast<__vpiModPathSrc*>(ref) ;
      أكد(src);

      vvp_fun_modpath_src *fun = dynamic_cast<vvp_fun_modpath_src*>(src->net->fun);
      أكد(fun);

      صحيح idx;
      vvp_time64_t tmp[12];
      fun->get_delay12(tmp);

      بدل (delays->no_of_delays) {
	  حالة 1:
	  حالة 2:
	  حالة 3:
	  حالة 6:
	  حالة 12:
	    اكسر;

	  افتراضي:
	    أكد(0);
	    اكسر;
      }

      אם (delays->time_type == vpiSimTime) {
	    עבור (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  vpip_time_to_timestruct(delays->da+idx, tmp[idx]);
	    }
      } אחרת {
	    עבור (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  delays->da[idx].real = vpip_time_to_scaled_real(tmp[idx], src->dest->scope);
	    }
      }
}

ساكن صحيح pathterm_get(صحيح code, vpiHandle ref)
{
      بنية __vpiModPathTerm*obj = dynamic_cast<__vpiModPathTerm*>(ref);
      أكد(obj);

      بدل (code) {
	  حالة vpiEdge:
	    החזר obj->edge;
	  افتراضي:
	    החזר 0;
      }
}

ساكن vpiHandle pathterm_get_handle(صحيح code, vpiHandle ref)
{
      بنية __vpiModPathTerm*obj = dynamic_cast<__vpiModPathTerm*>(ref);
      أكد(obj);

      بدل (code) {
	  حالة vpiExpr:
	    החזר obj->expr;
	  افتراضي:
	    החזר 0;
      }
}

/*
* The __vpiModPathSrc class is what the VPI client sees as a
* vpiModPath object. The __vpiModPath structure contains items that
* are common to a bunch of modpaths, including the destination term.
*/
مضمن __vpiModPathSrc::__vpiModPathSrc()
{ }

صحيح __vpiModPathSrc::get_type_code(فراغ) ثابت
{ החזר vpiModPath; }

صحيح __vpiModPathSrc::vpi_get(صحيح code)
{ החזר modpath_src_get(code, هذا); }

فراغ __vpiModPathSrc::vpi_get_value(p_vpi_value val)
{ modpath_src_get_value(هذا, val); }

vpiHandle __vpiModPathSrc::vpi_put_value(p_vpi_value val, صحيح flags)
{ החזר modpath_src_put_value(هذا, val, flags); }

vpiHandle __vpiModPathSrc::vpi_handle(صحيح code)
{ החזר modpath_src_get_handle(code, هذا); }

vpiHandle __vpiModPathSrc::vpi_iterate(صحيح code)
{ החזר modpath_src_iterate(code, هذا); }

vpiHandle __vpiModPathSrc:: vpi_index(صحيح idx)
{ החזר modpath_src_index(هذا, idx); }

فراغ __vpiModPathSrc::vpi_get_delays(p_vpi_delay del)
{ modpath_src_get_delays(هذا, del); }

فراغ __vpiModPathSrc::vpi_put_delays(p_vpi_delay del)
{ modpath_src_put_delays(هذا, del); }

ساكن صحيح modpath_src_free_object( vpiHandle ref )
{
      delete ref;
      החזר 1 ;
}

__vpiHandle::free_object_fun_t __vpiModPathSrc::free_object_fun(فراغ)
{ החזר &modpath_src_free_object; }


مضمن __vpiModPathTerm::__vpiModPathTerm()
{ }

صحيح __vpiModPathTerm::get_type_code(فراغ) ثابت
{ החזר vpiPathTerm; }

صحيح __vpiModPathTerm::vpi_get(صحيح code)
{ החזר pathterm_get(code, هذا); }

vpiHandle __vpiModPathTerm::vpi_handle(صحيح code)
{ החזר pathterm_get_handle(code, هذا); }

ساكن فراغ initialize_path_term(بنية __vpiModPathTerm&obj)
{
      obj.expr = 0;
      obj.edge = vpiNoEdge;
}

/*
 * This function will construct a vpiModPath Object.
 * give a respective "net", and will point to his
 * respective functor
 */

#ifdef CHECK_WITH_VALGRIND
ساكن بنية __vpiModPath**mp_list = 0;
ساكن unsigned mp_count = 0;
#endif

بنية __vpiModPath* vpip_make_modpath(vvp_net_t *net)
{
      بنية __vpiModPath*obj = جديد __vpiModPath;
      obj->scope = vpip_peek_current_scope ( );

      initialize_path_term(obj->path_term_out);
      obj->input_net = net ;

#ifdef CHECK_WITH_VALGRIND
      mp_count += 1;
      mp_list = static_cast<بنية __vpiModPath **>
                (realloc(mp_list,
                 mp_count*حجم(بنية __vpiModPath **)));
      mp_list[mp_count-1] = obj;
#endif
      החזר obj;
}

#ifdef CHECK_WITH_VALGRIND
فراغ modpath_delete()
{
      עבור (unsigned idx = 0; idx < mp_count; idx += 1) {
	    delete mp_list[idx];
      }
      حرر(mp_list);
      mp_list = 0;
      mp_count = 0;
}
#endif

/*
 * This function will construct a vpiModPathIn
 * ( struct __vpiModPathSrc ) Object. will give
 * a delays[12] values, and point to the specified functor
 *
 */

بنية __vpiModPathSrc* vpip_make_modpath_src(بنية __vpiModPath*path,
                                              vvp_net_t *net)
{
      بنية __vpiModPathSrc *obj = جديد __vpiModPathSrc;

      obj->dest = path;
      obj->type = 0;
      obj->net = net;
      initialize_path_term(obj->path_term_in);

      החזר obj;
}

vvp_fun_intermodpath::vvp_fun_intermodpath(vvp_net_t*net, unsigned width)
: net_(net)
{
      עבור (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = 0;

      cur_vec4_ = vvp_vector4_t(width, BIT4_X);
      schedule_init_propagate(net_, cur_vec4_);
}

vvp_fun_intermodpath::~vvp_fun_intermodpath()
{
}

فراغ vvp_fun_intermodpath::get_delay12(vvp_time64_t val[12]) ثابت
{
      עבור (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    val[idx] = delay_[idx];
}

فراغ vvp_fun_intermodpath::put_delay12(ثابت vvp_time64_t val[12])
{
      עבור (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = val[idx];
}

/*
 * FIXME: this implementation currently only uses the maximum delay
 * from all the bit changes in the vectors. If there are multiple
 * changes with different delays, then the results would be
 * wrong. What should happen is that if there are multiple changes,
 * multiple vectors approaching the result should be scheduled.
 */
فراغ vvp_fun_intermodpath::recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                              vvp_context_t)
{
	/* Only the first port is used. */
      אם (port.port() > 0)
	    החזר;

      אם (cur_vec4_.eeq(bit))
	    החזר;

	/* Given the scheduled output time, create an output event. */
      vvp_time64_t use_delay = delay_from_edge(cur_vec4_.value(0),
					       bit.value(0),
					       delay_);

	/* FIXME: This bases the edge delay on only the least
	   bit. This is WRONG! I need to find all the possible delays,
	   and schedule an event for each partial change. Hard! */
      עבור (unsigned idx = 1 ;  idx < bit.size() ;  idx += 1) {
	    vvp_time64_t tmp = delay_from_edge(cur_vec4_.value(idx),
					       bit.value(idx),
					       delay_);
	      /* If the current and new bit values match then no delay
	       * is needed for this bit. */
	    אם (cur_vec4_.value(idx) == bit.value(idx)) استمر;
	    أكد(tmp == use_delay);
      }

      cur_vec4_ = bit;
      schedule_generic(هذا, use_delay, שקר);
}

فراغ vvp_fun_intermodpath::run_run()
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
ساكن صحيح intermodpath_get(صحيح, vpiHandle ref)
{
      بنية __vpiInterModPath*obj =dynamic_cast<__vpiInterModPath*>(ref);
      أكد(obj);
      החזר 0;
}

ساكن فراغ intermodpath_get_value(vpiHandle ref, p_vpi_value)
{
      بنية __vpiInterModPath* intermodpath = dynamic_cast<__vpiInterModPath*>(ref);
      أكد(intermodpath);
      החזר;
}

ساكن vpiHandle intermodpath_put_value(vpiHandle ref, s_vpi_value *, صحيح )
{
      بنية __vpiInterModPath* intermodpath = dynamic_cast<__vpiInterModPath*>(ref);
      أكد(intermodpath);
      החזר 0;
}

ساكن vpiHandle intermodpath_get_handle(صحيح code, vpiHandle ref)
{
      بنية __vpiInterModPath*rfp = dynamic_cast<__vpiInterModPath*>(ref);
      أكد(rfp);

      بدل (code) {

	حالة vpiScope:
	  החזר rfp->scope;

	  حالة vpiModule:
	      { __vpiScope*scope = rfp->scope;
		כלעוד (scope && scope->get_type_code() != vpiModule)
		      scope = scope->scope;
		أكد(scope);
		החזר scope;
	      }
      }
      החזר 0;
}

ساكن vpiHandle intermodpath_iterate(صحيح code, vpiHandle ref)
{
      بنية __vpiInterModPath*rfp = dynamic_cast<__vpiInterModPath*>(ref);
      أكد(rfp);

	// For now intermodpaths only support exactly two ports
      بدل (code) {
	  حالة vpiPort: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(2, حجم(vpiHandle*)));
	    args[0] = rfp->port1;
	    args[1] = rfp->port2;
	    החזר vpip_make_iterator(2, args, אמת);
	  }
      }
      החזר 0;
}

/*
 * This routine will put specific dimension of delay[] values
 * into a vpiHandle. In this case, we will put
 * specific delays values in a vpiInterModPath object
 *
 */
ساكن فراغ intermodpath_put_delays (vpiHandle ref, p_vpi_delay delays)
{
      vvp_time64_t tmp[12];
      صحيح idx;
      بنية __vpiInterModPath * src = dynamic_cast<__vpiInterModPath*>(ref) ;
      أكد(src) ;

      vvp_fun_intermodpath *fun = dynamic_cast<vvp_fun_intermodpath*>(src->net->fun);
      أكد( fun );

      تعريفنوع unsigned حرف map_array_t[12];
	// Only the first six entries are used for the less than twelve maps.
      ساكن ثابت map_array_t map_1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      ساكن ثابت map_array_t map_2 = {0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0};
      ساكن ثابت map_array_t map_3 = {0, 1, 2, 0, 2, 1, 0, 0, 0, 0, 0, 0};
      ساكن ثابت map_array_t map_6 = {0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0};
      ساكن ثابت map_array_t map12 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

      ثابت map_array_t*use_map = 0;
      بدل (delays->no_of_delays) {
	  حالة 1:
	    use_map = &map_1;
	    اكسر;
	  حالة 2:
	    use_map = &map_2;
	    اكسر;
	  حالة 3:
	    use_map = &map_3;
	    اكسر;
	  حالة 6:
	    use_map = &map_6;
	    اكسر;
	  حالة 12:
	    use_map = &map12;
	    اكسر;
	  افتراضي:
	    أكد(0);
	    اكسر;
      }

      אם (delays->time_type == vpiSimTime) {
	    עבור (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_timestruct_to_time(delays->da+use_map[0][idx]);
	    }
      } אחרת {
	      // You cannot create a modpath with a negative delay so set it
	      // to zero per 1364-2005 section 14.3.1.
	    עבור (idx = 0 ; idx < delays->no_of_delays ; idx += 1) {
		  אם (delays->da[idx].real < 0.0) delays->da[idx].real = 0.0;
	    }
	    עבור (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_scaled_real_to_time64(delays->da[use_map[0][idx]].real,
							src->scope);
	    }
      }

      /* Now define the to-from-x delays if needed. */
      אם (delays->no_of_delays <= 6) {
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

ساكن فراغ intermodpath_get_delays ( vpiHandle ref, p_vpi_delay delays )
{
      بنية __vpiInterModPath*src = dynamic_cast<__vpiInterModPath*>(ref) ;
      أكد(src);

      vvp_fun_intermodpath *fun = dynamic_cast<vvp_fun_intermodpath*>(src->net->fun);
      أكد(fun);

      صحيح idx;
      vvp_time64_t tmp[12];
      fun->get_delay12(tmp);

      بدل (delays->no_of_delays) {
	  حالة 1:
	  حالة 2:
	  حالة 3:
	  حالة 6:
	  حالة 12:
	    اكسر;

	  افتراضي:
	    أكد(0);
	    اكسر;
      }

      אם (delays->time_type == vpiSimTime) {
	    עבור (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  vpip_time_to_timestruct(delays->da+idx, tmp[idx]);
	    }
      } אחרת {
	    עבור (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  delays->da[idx].real = vpip_time_to_scaled_real(tmp[idx], src->scope);
	    }
      }
}

/*
* The __vpiInterModPath class is what the VPI client sees as a
* vpiInterModPath object.
*/
مضمن __vpiInterModPath::__vpiInterModPath()
{ }

صحيح __vpiInterModPath::get_type_code(فراغ) ثابت
{ החזר vpiInterModPath; }

صحيح __vpiInterModPath::vpi_get(صحيح code)
{ החזר intermodpath_get(code, هذا); }

فراغ __vpiInterModPath::vpi_get_value(p_vpi_value val)
{ intermodpath_get_value(هذا, val); }

vpiHandle __vpiInterModPath::vpi_put_value(p_vpi_value val, صحيح flags)
{ החזר intermodpath_put_value(هذا, val, flags); }

vpiHandle __vpiInterModPath::vpi_handle(صحيح code)
{ החזר intermodpath_get_handle(code, هذا); }

vpiHandle __vpiInterModPath::vpi_iterate(صحيح code)
{ החזר intermodpath_iterate(code, هذا); }

فراغ __vpiInterModPath::vpi_get_delays(p_vpi_delay del)
{ intermodpath_get_delays(هذا, del); }

فراغ __vpiInterModPath::vpi_put_delays(p_vpi_delay del)
{ intermodpath_put_delays(هذا, del); }

ساكن صحيح intermodpath_free_object( vpiHandle ref )
{
      delete ref;
      החזר 1 ;
}

__vpiHandle::free_object_fun_t __vpiInterModPath::free_object_fun(فراغ)
{ החזר &intermodpath_free_object; }

/*
 * This function will construct a vpiInterModPath Object.
 * give a respective "net", and will point to his
 * respective functor
 */

#ifdef CHECK_WITH_VALGRIND
ساكن بنية __vpiInterModPath**imp_list = 0;
ساكن unsigned imp_count = 0;
#endif

بنية __vpiInterModPath* vpip_make_intermodpath(vvp_net_t *net, vpiPortInfo* port1, vpiPortInfo* port2)
{
      بنية __vpiInterModPath*obj = جديد __vpiInterModPath;
      obj->scope = vpip_peek_current_scope ( );

      obj->net = net;
      obj->port1 = port1;
      obj->port2 = port2;

#ifdef CHECK_WITH_VALGRIND
      imp_count += 1;
      imp_list = static_cast<بنية __vpiInterModPath **>
                 (realloc(imp_list,
                  imp_count*حجم(بنية __vpiInterModPath **)));
      imp_list[imp_count-1] = obj;
#endif
      החזר obj;
}

#ifdef CHECK_WITH_VALGRIND
فراغ intermodpath_delete()
{
      עבור (unsigned idx = 0; idx < mp_count; idx += 1) {
	    delete imp_list[idx];
      }
      حرر(imp_list);
      imp_list = 0;
      imp_count = 0;
}
#endif
