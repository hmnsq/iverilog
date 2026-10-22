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

#समावेश "delay.h"
#समावेश "schedule.h"
#समावेश "vpi_priv.h"
#समावेश "config.h"
#अगर_घोषित CHECK_WITH_VALGRIND
#समावेश "vvp_cleanup.h"
#पूर्ण_अगर
#समावेश <iostream>
#समावेश <cstdlib>
#समावेश <list>
#समावेश <cassert>
#समावेश <cmath>
#समावेश "ivl_alloc.h"

using namespace std;

व्योम vvp_delay_t::calculate_min_delay_()
{
      min_delay_ = rise_;
      अगर (fall_ < min_delay_)
	    min_delay_ = fall_;
      अगर (ignore_decay_) decay_ = min_delay_;
      अन्यथा अगर (decay_ < min_delay_)
	    min_delay_ = decay_;
}

vvp_delay_t::vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall)
{
      rise_ = rise;
      fall_ = fall;
      decay_= fall < rise? fall : rise;
      min_delay_ = decay_;
      ignore_decay_ = false;
}

vvp_delay_t::vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall, vvp_time64_t decay)
{
      rise_ = rise;
      fall_ = fall;
      decay_= decay;
      ignore_decay_ = false;

      calculate_min_delay_();
}

व्योम vvp_delay_t::set_ignore_decay()
{
      ignore_decay_ = true;

      calculate_min_delay_();
}

vvp_delay_t::~vvp_delay_t()
{
}

vvp_time64_t vvp_delay_t::get_delay(vvp_bit4_t from, vvp_bit4_t to)
{
      चयन (from) {
	  हाल BIT4_0:
	    चयन (to) {
		हाल BIT4_0: वापस 0;
		हाल BIT4_1: वापस rise_;
		हाल BIT4_X: वापस min_delay_;
		हाल BIT4_Z: वापस decay_;
	    }
	    अवरोध;
	  हाल BIT4_1:
	    चयन (to) {
		हाल BIT4_0: वापस fall_;
		हाल BIT4_1: वापस 0;
		हाल BIT4_X: वापस min_delay_;
		हाल BIT4_Z: वापस decay_;
	    }
	    अवरोध;
	  हाल BIT4_X:
	    चयन (to) {
		हाल BIT4_0: वापस fall_;
		हाल BIT4_1: वापस rise_;
		हाल BIT4_X: वापस 0;
		हाल BIT4_Z: वापस decay_;
	    }
	    अवरोध;
	  हाल BIT4_Z:
	    चयन (to) {
		हाल BIT4_0: वापस fall_;
		हाल BIT4_1: वापस rise_;
		हाल BIT4_X: वापस min_delay_;
		हाल BIT4_Z: वापस 0;
	    }
	    अवरोध;
      }

      निश्चित(0);
      वापस 0;
}

vvp_time64_t vvp_delay_t::get_min_delay() स्थिर
{
      वापस min_delay_;
}

व्योम vvp_delay_t::set_rise(vvp_time64_t val)
{
      rise_ = val;
      अगर (val < min_delay_) {
	    min_delay_ = val;
	    अगर (ignore_decay_) decay_ = val;
      } अन्यथा
	    calculate_min_delay_();
}

व्योम vvp_delay_t::set_fall(vvp_time64_t val)
{
      fall_ = val;
      अगर (val < min_delay_) {
	    min_delay_ = val;
	    अगर (ignore_decay_) decay_ = val;
      } अन्यथा
	    calculate_min_delay_();
}

व्योम vvp_delay_t::set_decay(vvp_time64_t val)
{
      निश्चित(!ignore_decay_);

      decay_ = val;
      अगर (val < min_delay_)
	    min_delay_ = val;
      अन्यथा
	    calculate_min_delay_();
}

vvp_fun_delay::vvp_fun_delay(vvp_net_t*n, अचिन्हित width, स्थिर vvp_delay_t&d)
: net_(n), delay_(d)
{
      cur_real_ = 0.0;
      अगर (width > 0) {
            cur_vec4_ = vvp_vector4_t(width, BIT4_X);
            cur_vec8_ = vvp_vector8_t(cur_vec4_, 6, 6);
            schedule_init_propagate(net_, cur_vec4_);
      } अन्यथा {
            schedule_init_propagate(net_, cur_real_);
      }
      list_ = 0;
      type_ = UNKNOWN_DELAY;
      initial_ = true;
	// Calculate the values used when converting variable delays
	// to simulation time units.
      स्थिर __vpiScope*scope = vpip_peek_current_scope();

      पूर्णांक powr = scope->time_units - scope->time_precision;
      round_ = 1;
      क्रम (पूर्णांक lp = 0; lp < powr; lp += 1) round_ *= 10;

      powr = scope->time_precision - vpip_get_time_precision();
      scale_ = 1;
      क्रम (पूर्णांक lp = 0; lp < powr; lp += 1) scale_ *= 10;
}

vvp_fun_delay::~vvp_fun_delay()
{
      जबतक (काष्ठा event_*cur = dequeue_())
	    मिटाओ cur;
}

bool vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        स्थिर vvp_vector4_t&bit)
{
      अगर (list_ == 0) वापस false;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      अगर (list_->अग्र_2->ptr_vec4.eeq(bit)) वापस true;

      clean_pulse_events_(use_delay);
      वापस false;
}

bool vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        स्थिर vvp_vector8_t&bit)
{
      अगर (list_ == 0) वापस false;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      अगर (list_->अग्र_2->ptr_vec8.eeq(bit)) वापस true;

      clean_pulse_events_(use_delay);
      वापस false;
}

bool vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        द्विगुन bit)
{
      अगर (list_ == 0) वापस false;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      अगर (list_->अग्र_2->ptr_real == bit) वापस true;

      clean_pulse_events_(use_delay);
      वापस false;
}

व्योम vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay)
{
      निश्चित(list_ != 0);

      करो {
	    काष्ठा event_*cur = list_->अग्र_2;
	      /* If this event is far enough from the event I'm about
	         to create, then that scheduled event is not a pulse
	         to be eliminated, so we're done. */
	    अगर (cur->sim_time+use_delay <= use_delay+schedule_simtime())
		  अवरोध;

	    अगर (list_ == cur)
		  list_ = 0;
	    अन्यथा
		  list_->अग्र_2 = cur->अग्र_2;
	    मिटाओ cur;
      } जबतक (list_);
}

/*
 * FIXME: this implementation currently only uses the maximum delay
 * from all the bit changes in the vectors. If there are multiple
 * changes with different delays, then the results would be
 * wrong. What should happen is that if there are multiple changes,
 * multiple vectors approaching the result should be scheduled.
 */
व्योम vvp_fun_delay::recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                              vvp_context_t)
{
      अगर (port.port() > 0) {
	      // Get the integer value of the bit vector, or 0 if
	      // there are X or Z bits.
	    vvp_time64_t bval = 0;
	      // The following does not work correctly for negative values.
	      // They should be sign extended to 64 bits (1364-2001 9.7.1).
	    vector4_to_मूल्य(bit, bval);
	      // Integer values do not need to be rounded so just scale them.
	    vvp_time64_t val = bval * round_ * scale_;

	    चयन (port.port()) {
		हाल 1:
		  delay_.set_rise(val);
		  वापस;
		हाल 2:
		  delay_.set_fall(val);
		  वापस;
		हाल 3:
		  delay_.set_decay(val);
		  वापस;
	    }
	    वापस;
      }

      vvp_time64_t use_delay;
	/* This is an initial value so it needs to be compared to all the
	   bits (the order the bits are changed is not deterministic). */
      अगर (initial_) {
	    type_ = VEC4_DELAY;
            cur_vec8_ = vvp_vector8_t(vvp_vector4_t(0, BIT4_X), 6, 6);
	    vvp_bit4_t cur_val = cur_vec4_.value(0);
	    use_delay = delay_.get_delay(cur_val, bit.value(0));
	    क्रम (अचिन्हित idx = 1 ;  idx < bit.size() ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(cur_val, bit.value(idx));
		  अगर (tmp > use_delay) use_delay = tmp;
	    }
      } अन्यथा {
	    निश्चित(type_ == VEC4_DELAY);

	      // Use as a reference for calculating the delay the
	      // current value of the output. Detect and handle the
	      // special case that the event list contains the current
	      // value as a zero-delay-remaining event.
	    स्थिर vvp_vector4_t&use_vec4 = (list_ && list_->अग्र_2->sim_time == schedule_simtime())? list_->अग्र_2->ptr_vec4 : cur_vec4_;

	      /* How many bits to compare? */
	    अचिन्हित use_wid = use_vec4.size();
	    अगर (bit.size() < use_wid) use_wid = bit.size();

	      /* Scan the vectors looking for delays. Select the maximum
	         delay encountered. */
	    use_delay = delay_.get_delay(use_vec4.value(0), bit.value(0));

	    क्रम (अचिन्हित idx = 1 ;  idx < use_wid ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(use_vec4.value(idx), bit.value(idx));
		  अगर (tmp > use_delay) use_delay = tmp;
	    }
      }

      /* what *should* happen here is we check to see if there is a
         transaction in the queue. This would be a pulse that needs to be
         eliminated. */
      अगर (clean_pulse_events_(use_delay, bit)) वापस;

      vvp_time64_t use_simtime = schedule_simtime() + use_delay;

	/* And propagate it. */
      अगर (use_delay == 0 && list_ == 0) {
	    cur_vec4_ = bit;
	    initial_ = false;
	    net_->send_vec4(cur_vec4_, 0);
      } अन्यथा {
	    काष्ठा event_*cur = नया काष्ठा event_(use_simtime);
	    cur->run_run_ptr = &vvp_fun_delay::run_run_vec4_;
	    cur->ptr_vec4 = bit;
	    enqueue_(cur);
	    schedule_generic(यह, use_delay, false);
      }
}

व्योम vvp_fun_delay::recv_vec4_pv(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
			         अचिन्हित base, अचिन्हित vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

/* See the recv_vec4 comment above. */
व्योम vvp_fun_delay::recv_vec8(vvp_net_ptr_t port, स्थिर vvp_vector8_t&bit)
{
      निश्चित(port.port() == 0);

      vvp_time64_t use_delay;
	/* This is an initial value so it needs to be compared to all the
	   bits (the order the bits are changed is not deterministic). */
      अगर (initial_) {
	    type_ = VEC8_DELAY;
            cur_vec4_ = vvp_vector4_t(0, BIT4_X);
	    vvp_bit4_t cur_val = cur_vec8_.value(0).value();
	    use_delay = delay_.get_delay(cur_val, bit.value(0).value());
	    क्रम (अचिन्हित idx = 1 ;  idx < bit.size() ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(cur_val, bit.value(idx).value());
		  अगर (tmp > use_delay) use_delay = tmp;
	    }
      } अन्यथा {
	    निश्चित(type_ == VEC8_DELAY);

	      // Use as a reference for calculating the delay the
	      // current value of the output. Detect and handle the
	      // special case that the event list contains the current
	      // value as a zero-delay-remaining event.
	    स्थिर vvp_vector8_t&use_vec8 = (list_ && list_->अग्र_2->sim_time == schedule_simtime())? list_->अग्र_2->ptr_vec8 : cur_vec8_;

	      /* How many bits to compare? */
	    अचिन्हित use_wid = use_vec8.size();
	    अगर (bit.size() < use_wid) use_wid = bit.size();

	      /* Scan the vectors looking for delays. Select the maximum
	         delay encountered. */
	    use_delay = delay_.get_delay(use_vec8.value(0).value(),
	                                 bit.value(0).value());

	    क्रम (अचिन्हित idx = 1 ;  idx < use_wid ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(use_vec8.value(idx).value(),
		                         bit.value(idx).value());
		  अगर (tmp > use_delay) use_delay = tmp;
	    }
      }

      /* what *should* happen here is we check to see if there is a
         transaction in the queue. This would be a pulse that needs to be
         eliminated. */
      अगर (clean_pulse_events_(use_delay, bit)) वापस;

      vvp_time64_t use_simtime = schedule_simtime() + use_delay;

	/* And propagate it. */
      अगर (use_delay == 0 && list_ == 0) {
	    cur_vec8_ = bit;
	    initial_ = false;
	    net_->send_vec8(cur_vec8_);
      } अन्यथा {
	    काष्ठा event_*cur = नया काष्ठा event_(use_simtime);
	    cur->ptr_vec8 = bit;
	    cur->run_run_ptr = &vvp_fun_delay::run_run_vec8_;
	    enqueue_(cur);
	    schedule_generic(यह, use_delay, false);
      }
}

व्योम vvp_fun_delay::recv_vec8_pv(vvp_net_ptr_t ptr, स्थिर vvp_vector8_t&bit,
			         अचिन्हित base, अचिन्हित vwid)
{
      recv_vec8_pv_(ptr, bit, base, vwid);
}

व्योम vvp_fun_delay::recv_real(vvp_net_ptr_t port, द्विगुन bit,
                              vvp_context_t)
{
      अगर (port.port() > 0) {
	    /* If the port is not 0, then this is a delay value that
	    should be rounded and converted to an integer delay. */
	    vvp_time64_t val = 0;
	    अगर (bit > -0.5) {
		  val = (vvp_time64_t) (bit * round_ + 0.5) * scale_;
	    } अन्यथा अगर (bit != bit) {
		    // For a NaN we use the default (0).
	    } अन्यथा {
		  vvp_vector4_t vec4(8*माप(vvp_time64_t),
		                     न्यूनमान(-bit * round_ + 0.5) * -1 * scale_);
		  vector4_to_मूल्य(vec4, val);
	    }

	    चयन (port.port()) {
		हाल 1:
		  delay_.set_rise(val);
		  वापस;
		हाल 2:
		  delay_.set_fall(val);
		  वापस;
		हाल 3:
		  delay_.set_decay(val);
		  वापस;
	    }
	    वापस;
      }

      अगर (initial_) {
	    type_ = REAL_DELAY;
            cur_vec4_ = vvp_vector4_t(0, BIT4_X);
            cur_vec8_ = vvp_vector8_t(cur_vec4_, 6, 6);
      } अन्यथा निश्चित(type_ == REAL_DELAY);

      vvp_time64_t use_delay;
      use_delay = delay_.get_min_delay();

      /* Eliminate glitches. */
      अगर (clean_pulse_events_(use_delay, bit)) वापस;

      /* This must be done after cleaning pulses to avoid propagating
       * an incorrect value. */
      अगर (cur_real_ == bit) वापस;

      vvp_time64_t use_simtime = schedule_simtime() + use_delay;

      अगर (use_delay == 0 && list_ == 0) {
	    cur_real_ = bit;
	    initial_ = false;
	    net_->send_real(cur_real_, 0);
      } अन्यथा {
	    काष्ठा event_*cur = नया काष्ठा event_(use_simtime);
	    cur->run_run_ptr = &vvp_fun_delay::run_run_real_;
	    cur->ptr_real = bit;
	    enqueue_(cur);

	    schedule_generic(यह, use_delay, false);
      }
}

व्योम vvp_fun_delay::run_run()
{
      vvp_time64_t sim_time = schedule_simtime();
      अगर (list_ == 0 || list_->अग्र_2->sim_time > sim_time)
	    वापस;

      काष्ठा event_*cur = dequeue_();
      अगर (cur == 0)
	    वापस;

      (यह->*(cur->run_run_ptr))(cur);
      initial_ = false;
      मिटाओ cur;
}

व्योम vvp_fun_delay::run_run_vec4_(स्थिर काष्ठा event_*cur)
{
      cur_vec4_ = cur->ptr_vec4;
      net_->send_vec4(cur_vec4_, 0);
}

व्योम vvp_fun_delay::run_run_vec8_(स्थिर काष्ठा vvp_fun_delay::event_*cur)
{
      cur_vec8_ = cur->ptr_vec8;
      net_->send_vec8(cur_vec8_);
}

व्योम vvp_fun_delay::run_run_real_(स्थिर काष्ठा vvp_fun_delay::event_*cur)
{
      cur_real_ = cur->ptr_real;
      net_->send_real(cur_real_, 0);
}

vvp_fun_modpath::vvp_fun_modpath(vvp_net_t*net, अचिन्हित width)
: net_(net), src_list_(0), ifnone_list_(0)
{
      cur_vec4_ = vvp_vector4_t(width, BIT4_X);
      schedule_init_propagate(net_, cur_vec4_);
}

vvp_fun_modpath::~vvp_fun_modpath()
{
	// Delete the source probes.
      जबतक (src_list_) {
	    vvp_fun_modpath_src*tmp = src_list_;
	    src_list_ = tmp->अग्र;
	    मिटाओ tmp;
      }
      जबतक (ifnone_list_) {
	    vvp_fun_modpath_src*tmp = ifnone_list_;
	    ifnone_list_ = tmp->अग्र;
	    मिटाओ tmp;
      }
}

व्योम vvp_fun_modpath::add_modpath_src(vvp_fun_modpath_src*that, bool ifnone)
{
      निश्चित(that->अग्र == 0);
      अगर (ifnone) {
	    that->अग्र = ifnone_list_;
	    ifnone_list_ = that;
      } अन्यथा {
	    that->अग्र = src_list_;
	    src_list_ = that;
      }
}

अटल vvp_time64_t delay_from_edge(vvp_bit4_t a, vvp_bit4_t b,
                                           vvp_time64_t स्थिर array[12])
{
      प्रकार delay_edge_t bit4_table4[4];
      अटल स्थिर bit4_table4 edge_table[4] = {
	    { DELAY_EDGE_01, DELAY_EDGE_01, DELAY_EDGE_0z, DELAY_EDGE_0x },
	    { DELAY_EDGE_10, DELAY_EDGE_10, DELAY_EDGE_1z, DELAY_EDGE_1x },
	    { DELAY_EDGE_z0, DELAY_EDGE_z1, DELAY_EDGE_z0, DELAY_EDGE_zx },
	    { DELAY_EDGE_x0, DELAY_EDGE_x1, DELAY_EDGE_xz, DELAY_EDGE_x0 }
      };

      वापस array[ edge_table[a][b] ];
}

व्योम vvp_fun_modpath::recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                                vvp_context_t)
{
	/* Only the first port is used. */
      अगर (port.port() > 0)
	    वापस;

      अगर (cur_vec4_.eeq(bit))
	    वापस;

	/* Select a time delay source that applies. Notice that there
	   may be multiple delay sources that apply, so collect all
	   the candidates into a list first. */
      list<vvp_fun_modpath_src*>candidate_list;
      vvp_time64_t candidate_wake_time = 0;
      क्रम (vvp_fun_modpath_src*cur = src_list_ ;  cur ;  cur=cur->अग्र) {
	      /* Skip paths that are disabled by conditions. */
	    अगर (cur->condition_flag_ == false)
		  जारी;

	    अगर (candidate_list.empty()) {
		  candidate_list.push_back(cur);
		  candidate_wake_time = cur->wake_time_;
	    } अन्यथा अगर (cur->wake_time_ == candidate_wake_time) {
		  candidate_list.push_back(cur);
	    } अन्यथा अगर (cur->wake_time_ > candidate_wake_time) {
		  candidate_list.assign(1, cur);
		  candidate_wake_time = cur->wake_time_;
	    } अन्यथा {
		  जारी; /* Skip this entry. */
	    }
      }

	/* Only add the ifnone delay if it has a later wake_time_ or
	 * if there are no normal delays. */
      vvp_time64_t ifnone_wake_time = candidate_wake_time;
      क्रम (vvp_fun_modpath_src*cur = ifnone_list_ ;  cur ;  cur=cur->अग्र) {
	    अगर (candidate_list.empty()) {
		  candidate_list.push_back(cur);
		  ifnone_wake_time = cur->wake_time_;
	    } अन्यथा अगर (cur->wake_time_ == ifnone_wake_time &&
	               ifnone_wake_time > candidate_wake_time) {
		  candidate_list.push_back(cur);
	    } अन्यथा अगर (cur->wake_time_ > ifnone_wake_time) {
		  candidate_list.assign(1, cur);
		  ifnone_wake_time = cur->wake_time_;
	    } अन्यथा {
		  जारी; /* Skip this entry. */
	    }
      }

	/* Handle the special case that there are no delays that
	   match. This may happen, for example, if the set of
	   conditional delays is incomplete, leaving some cases
	   uncovered. In that case, just pass the data without delay */
      अगर (candidate_list.empty()) {
	    cur_vec4_ = bit;
	    schedule_generic(यह, 0, false);
	    वापस;
      }

	/* Now given that we have a list of candidate delays, find for
	   each if the 12 numbers the minimum from all the
	   candidates. This minimum set becomes the chosen delay to
	   use. */
      vvp_time64_t out_at[12];
      vvp_time64_t now = schedule_simtime();

      प्रकार list<vvp_fun_modpath_src*>::const_iterator iter_t;

      iter_t cur = candidate_list.begin();
      स्थिर vvp_fun_modpath_src*src = *cur;

      क्रम (अचिन्हित idx = 0 ;  idx < 12 ;  idx += 1) {
	    out_at[idx] = src->wake_time_ + src->delay_[idx];
	    अगर (out_at[idx] <= now)
		  out_at[idx] = 0;
	    अन्यथा
		  out_at[idx] -= now;
      }

      क्रम (++ cur ; cur != candidate_list.end() ; ++ cur ) {
	    src = *cur;
	    क्रम (अचिन्हित idx = 0 ;  idx < 12 ;  idx += 1) {
		  vvp_time64_t tmp = src->wake_time_ + src->delay_[idx];
		  अगर (tmp <= now)
			tmp = 0;
		  अन्यथा
			tmp -= now;
		  अगर (tmp < out_at[idx])
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
      क्रम (अचिन्हित idx = 1 ;  idx < bit.size() ;  idx += 1) {
	    vvp_time64_t tmp = delay_from_edge(cur_vec4_.value(idx),
					       bit.value(idx),
					       out_at);
	      /* If the current and new bit values match then no delay
	       * is needed for this bit. */
	    अगर (cur_vec4_.value(idx) == bit.value(idx)) जारी;
	    अगर (tmp != use_delay) {
		  ख_लिखो(मानक_त्रुटि, "sorry: multi-bit module path delays are "
		                  "currently not fully supported.\n");
		  ख_लिखो(मानक_त्रुटि, "     : using the LSB delay for all bits.\n");
		  जारी;
            }
      }

      cur_vec4_ = bit;
      schedule_generic(यह, use_delay, false);
}

व्योम vvp_fun_modpath::run_run()
{
      net_->send_vec4(cur_vec4_, 0);
}

vvp_fun_modpath_src::vvp_fun_modpath_src(vvp_time64_t स्थिर del[12])
{
      क्रम (अचिन्हित idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = del[idx];

      अग्र = 0;
      wake_time_ = 0;
      condition_flag_ = true;
}

vvp_fun_modpath_src::~vvp_fun_modpath_src()
{
}

व्योम vvp_fun_modpath_src::get_delay12(vvp_time64_t val[12]) स्थिर
{
      क्रम (अचिन्हित idx = 0 ;  idx < 12 ;  idx += 1)
	    val[idx] = delay_[idx];
}

व्योम vvp_fun_modpath_src::put_delay12(स्थिर vvp_time64_t val[12])
{
      क्रम (अचिन्हित idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = val[idx];
}

व्योम vvp_fun_modpath_src::recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                                    vvp_context_t)
{
      अगर (port.port() == 0) {
	      // The modpath input...
	    अगर (test_vec4(bit))
		  wake_time_ = schedule_simtime();

      } अन्यथा अगर (port.port() == 1) {
	      // The modpath condition input...
	    अगर (bit.value(0) != BIT4_0)
		  condition_flag_ = true;
	    अन्यथा
		  condition_flag_ = false;
      }
}

bool vvp_fun_modpath_src::test_vec4(स्थिर vvp_vector4_t&)
{
      वापस true;
}

vvp_fun_modpath_edge::vvp_fun_modpath_edge(vvp_time64_t del[12],
					   bool pos, bool neg)
: vvp_fun_modpath_src(del)
{
      old_मूल्य = BIT4_X;
      posedge_ = pos;
      negedge_ = neg;
}

bool vvp_fun_modpath_edge::test_vec4(स्थिर vvp_vector4_t&bit)
{
      vvp_bit4_t tmp = old_मूल्य;
      old_मूल्य = bit.value(0);

      पूर्णांक edge_flag = edge(tmp, old_मूल्य);
      अगर (edge_flag > 0) वापस posedge_;
      अगर (edge_flag < 0) वापस negedge_;
      वापस false;
}


/*
 * All the below routines that begin with
 * modpath_src_* belong the internal function
 * of an vpiModPathIn object. This is used to
 * make some specific delays path operations
 *
 */
अटल पूर्णांक modpath_src_get(पूर्णांक, vpiHandle ref)
{
      काष्ठा __vpiModPathSrc*obj =dynamic_cast<__vpiModPathSrc*>(ref);
      निश्चित(obj);
      वापस 0;
}

अटल व्योम modpath_src_get_मूल्य(vpiHandle ref, p_vpi_मूल्य)
{
      काष्ठा __vpiModPathSrc* modpathsrc = dynamic_cast<__vpiModPathSrc*>(ref);
      निश्चित(modpathsrc);
      वापस;
}

अटल vpiHandle modpath_src_put_मूल्य(vpiHandle ref, s_vpi_मूल्य *, पूर्णांक )
{
      काष्ठा __vpiModPathSrc* modpathsrc = dynamic_cast<__vpiModPathSrc*>(ref);
      निश्चित(modpathsrc);
      वापस 0;
}

अटल vpiHandle modpath_src_get_handle(पूर्णांक code, vpiHandle ref)
{
      काष्ठा __vpiModPathSrc*rfp = dynamic_cast<__vpiModPathSrc*>(ref);
      निश्चित(rfp);

      चयन (code) {

	हाल vpiScope:
	  वापस rfp->dest->scope;

	  हाल vpiModule:
	      { __vpiScope*scope = rfp->dest->scope;
		जबतक (scope && scope->get_type_code() != vpiModule)
		      scope = scope->scope;
		निश्चित(scope);
		वापस scope;
	      }

	    // Handles to path term objects should really be obtained via
	    // the vpi_iterate and vpi_scan functions. Continue to allow
	    // them to be obtained here for backwards compatibility with
	    // older versions of Icarus Verilog.

	  हाल vpiModPathIn:
	    वापस &rfp->path_term_in;

	  हाल vpiModPathOut:
	    वापस &rfp->dest->path_term_out;
      }
      वापस 0;
}

अटल vpiHandle modpath_src_iterate(पूर्णांक code, vpiHandle ref)
{
      काष्ठा __vpiModPathSrc*rfp = dynamic_cast<__vpiModPathSrc*>(ref);
      निश्चित(rfp);

	// Module paths with multiple sources or destinations are
	// currently represented by a separate modpath object for
	// each source/destination combination, so there is only
	// ever one input path term and one output path term.
      चयन (code) {
	  हाल vpiModPathIn: {
	    vpiHandle*args = static_cast<vpiHandle*>(सुस्मृति(1, माप(vpiHandle*)));
	    args[0] = &rfp->path_term_in;
	    वापस vpip_make_iterator(1, args, true);
	  }
	  हाल vpiModPathOut: {
	    vpiHandle*args = static_cast<vpiHandle*>(सुस्मृति(1, माप(vpiHandle*)));
	    args[0] = &rfp->dest->path_term_out;
	    वापस vpip_make_iterator(1, args, true);
	  }
      }
      वापस 0;
}

अटल vpiHandle modpath_src_सूचकांक ( vpiHandle ref, पूर्णांक)
{
      निश्चित(ref->get_type_code() == vpiModPathIn);
      वापस 0;
}


/*
 * This routine will put specific dimension of delay[] values
 * into a vpiHandle. In this case, we will put
 * specific delays values in a vpiModPathIn object
 *
 */
अटल व्योम modpath_src_put_delays (vpiHandle ref, p_vpi_delay delays)
{
      vvp_time64_t tmp[12];
      पूर्णांक idx;
      काष्ठा __vpiModPathSrc * src = dynamic_cast<__vpiModPathSrc*>(ref) ;
      निश्चित(src) ;

      vvp_fun_modpath_src *fun = dynamic_cast<vvp_fun_modpath_src*>(src->net->fun);
      निश्चित( fun );

      प्रकार अचिन्हित अक्षर map_array_t[12];
	// Only the first six entries are used for the less than twelve maps.
      अटल स्थिर map_array_t map_1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      अटल स्थिर map_array_t map_2 = {0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0};
      अटल स्थिर map_array_t map_3 = {0, 1, 2, 0, 2, 1, 0, 0, 0, 0, 0, 0};
      अटल स्थिर map_array_t map_6 = {0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0};
      अटल स्थिर map_array_t map12 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

      स्थिर map_array_t*use_map = 0;
      चयन (delays->no_of_delays) {
	  हाल 1:
	    use_map = &map_1;
	    अवरोध;
	  हाल 2:
	    use_map = &map_2;
	    अवरोध;
	  हाल 3:
	    use_map = &map_3;
	    अवरोध;
	  हाल 6:
	    use_map = &map_6;
	    अवरोध;
	  हाल 12:
	    use_map = &map12;
	    अवरोध;
	  शेष:
	    निश्चित(0);
	    अवरोध;
      }

      अगर (delays->time_type == vpiSimTime) {
	    क्रम (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_timestruct_to_time(delays->da+use_map[0][idx]);
	    }
      } अन्यथा {
	      // You cannot create a modpath with a negative delay so set it
	      // to zero per 1364-2005 section 14.3.1.
	    क्रम (idx = 0 ; idx < delays->no_of_delays ; idx += 1) {
		  अगर (delays->da[idx].real < 0.0) delays->da[idx].real = 0.0;
	    }
	    क्रम (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_scaled_real_to_time64(delays->da[use_map[0][idx]].real,
							src->dest->scope);
	    }
      }

      /* Now define the to-from-x delays if needed. */
      अगर (delays->no_of_delays <= 6) {
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

अटल व्योम modpath_src_get_delays ( vpiHandle ref, p_vpi_delay delays )
{
      काष्ठा __vpiModPathSrc*src = dynamic_cast<__vpiModPathSrc*>(ref) ;
      निश्चित(src);

      vvp_fun_modpath_src *fun = dynamic_cast<vvp_fun_modpath_src*>(src->net->fun);
      निश्चित(fun);

      पूर्णांक idx;
      vvp_time64_t tmp[12];
      fun->get_delay12(tmp);

      चयन (delays->no_of_delays) {
	  हाल 1:
	  हाल 2:
	  हाल 3:
	  हाल 6:
	  हाल 12:
	    अवरोध;

	  शेष:
	    निश्चित(0);
	    अवरोध;
      }

      अगर (delays->time_type == vpiSimTime) {
	    क्रम (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  vpip_time_to_timestruct(delays->da+idx, tmp[idx]);
	    }
      } अन्यथा {
	    क्रम (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  delays->da[idx].real = vpip_time_to_scaled_real(tmp[idx], src->dest->scope);
	    }
      }
}

अटल पूर्णांक pathterm_get(पूर्णांक code, vpiHandle ref)
{
      काष्ठा __vpiModPathTerm*obj = dynamic_cast<__vpiModPathTerm*>(ref);
      निश्चित(obj);

      चयन (code) {
	  हाल vpiEdge:
	    वापस obj->edge;
	  शेष:
	    वापस 0;
      }
}

अटल vpiHandle pathterm_get_handle(पूर्णांक code, vpiHandle ref)
{
      काष्ठा __vpiModPathTerm*obj = dynamic_cast<__vpiModPathTerm*>(ref);
      निश्चित(obj);

      चयन (code) {
	  हाल vpi_व्यंजक:
	    वापस obj->व्यंजक;
	  शेष:
	    वापस 0;
      }
}

/*
* The __vpiModPathSrc class is what the VPI client sees as a
* vpiModPath object. The __vpiModPath structure contains items that
* are common to a bunch of modpaths, including the destination term.
*/
अंतरभूत __vpiModPathSrc::__vpiModPathSrc()
{ }

पूर्णांक __vpiModPathSrc::get_type_code(व्योम) स्थिर
{ वापस vpiModPath; }

पूर्णांक __vpiModPathSrc::vpi_get(पूर्णांक code)
{ वापस modpath_src_get(code, यह); }

व्योम __vpiModPathSrc::vpi_get_मूल्य(p_vpi_मूल्य val)
{ modpath_src_get_मूल्य(यह, val); }

vpiHandle __vpiModPathSrc::vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णांक flags)
{ वापस modpath_src_put_मूल्य(यह, val, flags); }

vpiHandle __vpiModPathSrc::vpi_handle(पूर्णांक code)
{ वापस modpath_src_get_handle(code, यह); }

vpiHandle __vpiModPathSrc::vpi_iterate(पूर्णांक code)
{ वापस modpath_src_iterate(code, यह); }

vpiHandle __vpiModPathSrc:: vpi_सूचकांक(पूर्णांक idx)
{ वापस modpath_src_सूचकांक(यह, idx); }

व्योम __vpiModPathSrc::vpi_get_delays(p_vpi_delay del)
{ modpath_src_get_delays(यह, del); }

व्योम __vpiModPathSrc::vpi_put_delays(p_vpi_delay del)
{ modpath_src_put_delays(यह, del); }

अटल पूर्णांक modpath_src_free_object( vpiHandle ref )
{
      मिटाओ ref;
      वापस 1 ;
}

__vpiHandle::free_object_fun_t __vpiModPathSrc::free_object_fun(व्योम)
{ वापस &modpath_src_free_object; }


अंतरभूत __vpiModPathTerm::__vpiModPathTerm()
{ }

पूर्णांक __vpiModPathTerm::get_type_code(व्योम) स्थिर
{ वापस vpiPathTerm; }

पूर्णांक __vpiModPathTerm::vpi_get(पूर्णांक code)
{ वापस pathterm_get(code, यह); }

vpiHandle __vpiModPathTerm::vpi_handle(पूर्णांक code)
{ वापस pathterm_get_handle(code, यह); }

अटल व्योम initialize_path_term(काष्ठा __vpiModPathTerm&obj)
{
      obj.व्यंजक = 0;
      obj.edge = vpiNoEdge;
}

/*
 * This function will construct a vpiModPath Object.
 * give a respective "net", and will point to his
 * respective functor
 */

#अगर_घोषित CHECK_WITH_VALGRIND
अटल काष्ठा __vpiModPath**mp_list = 0;
अटल अचिन्हित mp_गिनती = 0;
#पूर्ण_अगर

काष्ठा __vpiModPath* vpip_make_modpath(vvp_net_t *net)
{
      काष्ठा __vpiModPath*obj = नया __vpiModPath;
      obj->scope = vpip_peek_current_scope ( );

      initialize_path_term(obj->path_term_out);
      obj->निवेश_net = net ;

#अगर_घोषित CHECK_WITH_VALGRIND
      mp_गिनती += 1;
      mp_list = static_cast<काष्ठा __vpiModPath **>
                (पुनः_स्मृति(mp_list,
                 mp_गिनती*माप(काष्ठा __vpiModPath **)));
      mp_list[mp_गिनती-1] = obj;
#पूर्ण_अगर
      वापस obj;
}

#अगर_घोषित CHECK_WITH_VALGRIND
व्योम modpath_delete()
{
      क्रम (अचिन्हित idx = 0; idx < mp_गिनती; idx += 1) {
	    मिटाओ mp_list[idx];
      }
      मुक्त(mp_list);
      mp_list = 0;
      mp_गिनती = 0;
}
#पूर्ण_अगर

/*
 * This function will construct a vpiModPathIn
 * ( struct __vpiModPathSrc ) Object. will give
 * a delays[12] values, and point to the specified functor
 *
 */

काष्ठा __vpiModPathSrc* vpip_make_modpath_src(काष्ठा __vpiModPath*path,
                                              vvp_net_t *net)
{
      काष्ठा __vpiModPathSrc *obj = नया __vpiModPathSrc;

      obj->dest = path;
      obj->type = 0;
      obj->net = net;
      initialize_path_term(obj->path_term_in);

      वापस obj;
}

vvp_fun_intermodpath::vvp_fun_intermodpath(vvp_net_t*net, अचिन्हित width)
: net_(net)
{
      क्रम (अचिन्हित idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = 0;

      cur_vec4_ = vvp_vector4_t(width, BIT4_X);
      schedule_init_propagate(net_, cur_vec4_);
}

vvp_fun_intermodpath::~vvp_fun_intermodpath()
{
}

व्योम vvp_fun_intermodpath::get_delay12(vvp_time64_t val[12]) स्थिर
{
      क्रम (अचिन्हित idx = 0 ;  idx < 12 ;  idx += 1)
	    val[idx] = delay_[idx];
}

व्योम vvp_fun_intermodpath::put_delay12(स्थिर vvp_time64_t val[12])
{
      क्रम (अचिन्हित idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = val[idx];
}

/*
 * FIXME: this implementation currently only uses the maximum delay
 * from all the bit changes in the vectors. If there are multiple
 * changes with different delays, then the results would be
 * wrong. What should happen is that if there are multiple changes,
 * multiple vectors approaching the result should be scheduled.
 */
व्योम vvp_fun_intermodpath::recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                              vvp_context_t)
{
	/* Only the first port is used. */
      अगर (port.port() > 0)
	    वापस;

      अगर (cur_vec4_.eeq(bit))
	    वापस;

	/* Given the scheduled output time, create an output event. */
      vvp_time64_t use_delay = delay_from_edge(cur_vec4_.value(0),
					       bit.value(0),
					       delay_);

	/* FIXME: This bases the edge delay on only the least
	   bit. This is WRONG! I need to find all the possible delays,
	   and schedule an event for each partial change. Hard! */
      क्रम (अचिन्हित idx = 1 ;  idx < bit.size() ;  idx += 1) {
	    vvp_time64_t tmp = delay_from_edge(cur_vec4_.value(idx),
					       bit.value(idx),
					       delay_);
	      /* If the current and new bit values match then no delay
	       * is needed for this bit. */
	    अगर (cur_vec4_.value(idx) == bit.value(idx)) जारी;
	    निश्चित(tmp == use_delay);
      }

      cur_vec4_ = bit;
      schedule_generic(यह, use_delay, false);
}

व्योम vvp_fun_intermodpath::run_run()
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
अटल पूर्णांक intermodpath_get(पूर्णांक, vpiHandle ref)
{
      काष्ठा __vpiInterModPath*obj =dynamic_cast<__vpiInterModPath*>(ref);
      निश्चित(obj);
      वापस 0;
}

अटल व्योम intermodpath_get_मूल्य(vpiHandle ref, p_vpi_मूल्य)
{
      काष्ठा __vpiInterModPath* intermodpath = dynamic_cast<__vpiInterModPath*>(ref);
      निश्चित(intermodpath);
      वापस;
}

अटल vpiHandle intermodpath_put_मूल्य(vpiHandle ref, s_vpi_मूल्य *, पूर्णांक )
{
      काष्ठा __vpiInterModPath* intermodpath = dynamic_cast<__vpiInterModPath*>(ref);
      निश्चित(intermodpath);
      वापस 0;
}

अटल vpiHandle intermodpath_get_handle(पूर्णांक code, vpiHandle ref)
{
      काष्ठा __vpiInterModPath*rfp = dynamic_cast<__vpiInterModPath*>(ref);
      निश्चित(rfp);

      चयन (code) {

	हाल vpiScope:
	  वापस rfp->scope;

	  हाल vpiModule:
	      { __vpiScope*scope = rfp->scope;
		जबतक (scope && scope->get_type_code() != vpiModule)
		      scope = scope->scope;
		निश्चित(scope);
		वापस scope;
	      }
      }
      वापस 0;
}

अटल vpiHandle intermodpath_iterate(पूर्णांक code, vpiHandle ref)
{
      काष्ठा __vpiInterModPath*rfp = dynamic_cast<__vpiInterModPath*>(ref);
      निश्चित(rfp);

	// For now intermodpaths only support exactly two ports
      चयन (code) {
	  हाल vpiPort: {
	    vpiHandle*args = static_cast<vpiHandle*>(सुस्मृति(2, माप(vpiHandle*)));
	    args[0] = rfp->port1;
	    args[1] = rfp->port2;
	    वापस vpip_make_iterator(2, args, true);
	  }
      }
      वापस 0;
}

/*
 * This routine will put specific dimension of delay[] values
 * into a vpiHandle. In this case, we will put
 * specific delays values in a vpiInterModPath object
 *
 */
अटल व्योम intermodpath_put_delays (vpiHandle ref, p_vpi_delay delays)
{
      vvp_time64_t tmp[12];
      पूर्णांक idx;
      काष्ठा __vpiInterModPath * src = dynamic_cast<__vpiInterModPath*>(ref) ;
      निश्चित(src) ;

      vvp_fun_intermodpath *fun = dynamic_cast<vvp_fun_intermodpath*>(src->net->fun);
      निश्चित( fun );

      प्रकार अचिन्हित अक्षर map_array_t[12];
	// Only the first six entries are used for the less than twelve maps.
      अटल स्थिर map_array_t map_1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      अटल स्थिर map_array_t map_2 = {0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0};
      अटल स्थिर map_array_t map_3 = {0, 1, 2, 0, 2, 1, 0, 0, 0, 0, 0, 0};
      अटल स्थिर map_array_t map_6 = {0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0};
      अटल स्थिर map_array_t map12 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

      स्थिर map_array_t*use_map = 0;
      चयन (delays->no_of_delays) {
	  हाल 1:
	    use_map = &map_1;
	    अवरोध;
	  हाल 2:
	    use_map = &map_2;
	    अवरोध;
	  हाल 3:
	    use_map = &map_3;
	    अवरोध;
	  हाल 6:
	    use_map = &map_6;
	    अवरोध;
	  हाल 12:
	    use_map = &map12;
	    अवरोध;
	  शेष:
	    निश्चित(0);
	    अवरोध;
      }

      अगर (delays->time_type == vpiSimTime) {
	    क्रम (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_timestruct_to_time(delays->da+use_map[0][idx]);
	    }
      } अन्यथा {
	      // You cannot create a modpath with a negative delay so set it
	      // to zero per 1364-2005 section 14.3.1.
	    क्रम (idx = 0 ; idx < delays->no_of_delays ; idx += 1) {
		  अगर (delays->da[idx].real < 0.0) delays->da[idx].real = 0.0;
	    }
	    क्रम (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_scaled_real_to_time64(delays->da[use_map[0][idx]].real,
							src->scope);
	    }
      }

      /* Now define the to-from-x delays if needed. */
      अगर (delays->no_of_delays <= 6) {
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

अटल व्योम intermodpath_get_delays ( vpiHandle ref, p_vpi_delay delays )
{
      काष्ठा __vpiInterModPath*src = dynamic_cast<__vpiInterModPath*>(ref) ;
      निश्चित(src);

      vvp_fun_intermodpath *fun = dynamic_cast<vvp_fun_intermodpath*>(src->net->fun);
      निश्चित(fun);

      पूर्णांक idx;
      vvp_time64_t tmp[12];
      fun->get_delay12(tmp);

      चयन (delays->no_of_delays) {
	  हाल 1:
	  हाल 2:
	  हाल 3:
	  हाल 6:
	  हाल 12:
	    अवरोध;

	  शेष:
	    निश्चित(0);
	    अवरोध;
      }

      अगर (delays->time_type == vpiSimTime) {
	    क्रम (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  vpip_time_to_timestruct(delays->da+idx, tmp[idx]);
	    }
      } अन्यथा {
	    क्रम (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  delays->da[idx].real = vpip_time_to_scaled_real(tmp[idx], src->scope);
	    }
      }
}

/*
* The __vpiInterModPath class is what the VPI client sees as a
* vpiInterModPath object.
*/
अंतरभूत __vpiInterModPath::__vpiInterModPath()
{ }

पूर्णांक __vpiInterModPath::get_type_code(व्योम) स्थिर
{ वापस vpiInterModPath; }

पूर्णांक __vpiInterModPath::vpi_get(पूर्णांक code)
{ वापस intermodpath_get(code, यह); }

व्योम __vpiInterModPath::vpi_get_मूल्य(p_vpi_मूल्य val)
{ intermodpath_get_मूल्य(यह, val); }

vpiHandle __vpiInterModPath::vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णांक flags)
{ वापस intermodpath_put_मूल्य(यह, val, flags); }

vpiHandle __vpiInterModPath::vpi_handle(पूर्णांक code)
{ वापस intermodpath_get_handle(code, यह); }

vpiHandle __vpiInterModPath::vpi_iterate(पूर्णांक code)
{ वापस intermodpath_iterate(code, यह); }

व्योम __vpiInterModPath::vpi_get_delays(p_vpi_delay del)
{ intermodpath_get_delays(यह, del); }

व्योम __vpiInterModPath::vpi_put_delays(p_vpi_delay del)
{ intermodpath_put_delays(यह, del); }

अटल पूर्णांक intermodpath_free_object( vpiHandle ref )
{
      मिटाओ ref;
      वापस 1 ;
}

__vpiHandle::free_object_fun_t __vpiInterModPath::free_object_fun(व्योम)
{ वापस &intermodpath_free_object; }

/*
 * This function will construct a vpiInterModPath Object.
 * give a respective "net", and will point to his
 * respective functor
 */

#अगर_घोषित CHECK_WITH_VALGRIND
अटल काष्ठा __vpiInterModPath**imp_list = 0;
अटल अचिन्हित imp_गिनती = 0;
#पूर्ण_अगर

काष्ठा __vpiInterModPath* vpip_make_intermodpath(vvp_net_t *net, vpiPortInfo* port1, vpiPortInfo* port2)
{
      काष्ठा __vpiInterModPath*obj = नया __vpiInterModPath;
      obj->scope = vpip_peek_current_scope ( );

      obj->net = net;
      obj->port1 = port1;
      obj->port2 = port2;

#अगर_घोषित CHECK_WITH_VALGRIND
      imp_गिनती += 1;
      imp_list = static_cast<काष्ठा __vpiInterModPath **>
                 (पुनः_स्मृति(imp_list,
                  imp_गिनती*माप(काष्ठा __vpiInterModPath **)));
      imp_list[imp_गिनती-1] = obj;
#पूर्ण_अगर
      वापस obj;
}

#अगर_घोषित CHECK_WITH_VALGRIND
व्योम intermodpath_delete()
{
      क्रम (अचिन्हित idx = 0; idx < mp_गिनती; idx += 1) {
	    मिटाओ imp_list[idx];
      }
      मुक्त(imp_list);
      imp_list = 0;
      imp_गिनती = 0;
}
#पूर्ण_अगर
