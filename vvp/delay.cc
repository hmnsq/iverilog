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

#અન્તર્ભાવ "delay.h"
#અન્તર્ભાવ "schedule.h"
#અન્તર્ભાવ "vpi_priv.h"
#અન્તર્ભાવ "config.h"
#ifdef CHECK_WITH_VALGRIND
#અન્તર્ભાવ "vvp_cleanup.h"
#endif
#અન્તર્ભાવ <iostream>
#અન્તર્ભાવ <cstdlib>
#અન્તર્ભાવ <list>
#અન્તર્ભાવ <cassert>
#અન્તર્ભાવ <cmath>
#અન્તર્ભાવ "ivl_alloc.h"

using નામાકાશ std;

શૂન્ય vvp_delay_t::calculate_min_delay_()
{
      min_delay_ = rise_;
      જો (fall_ < min_delay_)
	    min_delay_ = fall_;
      જો (ignore_decay_) decay_ = min_delay_;
      નહીંતર જો (decay_ < min_delay_)
	    min_delay_ = decay_;
}

vvp_delay_t::vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall)
{
      rise_ = rise;
      fall_ = fall;
      decay_= fall < rise? fall : rise;
      min_delay_ = decay_;
      ignore_decay_ = ખોટું;
}

vvp_delay_t::vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall, vvp_time64_t decay)
{
      rise_ = rise;
      fall_ = fall;
      decay_= decay;
      ignore_decay_ = ખોટું;

      calculate_min_delay_();
}

શૂન્ય vvp_delay_t::set_ignore_decay()
{
      ignore_decay_ = સાચું;

      calculate_min_delay_();
}

vvp_delay_t::~vvp_delay_t()
{
}

vvp_time64_t vvp_delay_t::get_delay(vvp_bit4_t from, vvp_bit4_t to)
{
      પસંદગી (from) {
	  સ્થિતિ BIT4_0:
	    પસંદગી (to) {
		સ્થિતિ BIT4_0: પાછા 0;
		સ્થિતિ BIT4_1: પાછા rise_;
		સ્થિતિ BIT4_X: પાછા min_delay_;
		સ્થિતિ BIT4_Z: પાછા decay_;
	    }
	    તોડો;
	  સ્થિતિ BIT4_1:
	    પસંદગી (to) {
		સ્થિતિ BIT4_0: પાછા fall_;
		સ્થિતિ BIT4_1: પાછા 0;
		સ્થિતિ BIT4_X: પાછા min_delay_;
		સ્થિતિ BIT4_Z: પાછા decay_;
	    }
	    તોડો;
	  સ્થિતિ BIT4_X:
	    પસંદગી (to) {
		સ્થિતિ BIT4_0: પાછા fall_;
		સ્થિતિ BIT4_1: પાછા rise_;
		સ્થિતિ BIT4_X: પાછા 0;
		સ્થિતિ BIT4_Z: પાછા decay_;
	    }
	    તોડો;
	  સ્થિતિ BIT4_Z:
	    પસંદગી (to) {
		સ્થિતિ BIT4_0: પાછા fall_;
		સ્થિતિ BIT4_1: પાછા rise_;
		સ્થિતિ BIT4_X: પાછા min_delay_;
		સ્થિતિ BIT4_Z: પાછા 0;
	    }
	    તોડો;
      }

      પ્રતિજ્ઞા(0);
      પાછા 0;
}

vvp_time64_t vvp_delay_t::get_min_delay() અચલ
{
      પાછા min_delay_;
}

શૂન્ય vvp_delay_t::set_rise(vvp_time64_t val)
{
      rise_ = val;
      જો (val < min_delay_) {
	    min_delay_ = val;
	    જો (ignore_decay_) decay_ = val;
      } નહીંતર
	    calculate_min_delay_();
}

શૂન્ય vvp_delay_t::set_fall(vvp_time64_t val)
{
      fall_ = val;
      જો (val < min_delay_) {
	    min_delay_ = val;
	    જો (ignore_decay_) decay_ = val;
      } નહીંતર
	    calculate_min_delay_();
}

શૂન્ય vvp_delay_t::set_decay(vvp_time64_t val)
{
      પ્રતિજ્ઞા(!ignore_decay_);

      decay_ = val;
      જો (val < min_delay_)
	    min_delay_ = val;
      નહીંતર
	    calculate_min_delay_();
}

vvp_fun_delay::vvp_fun_delay(vvp_net_t*n, unsigned width, અચલ vvp_delay_t&d)
: net_(n), delay_(d)
{
      cur_real_ = 0.0;
      જો (width > 0) {
            cur_vec4_ = vvp_vector4_t(width, BIT4_X);
            cur_vec8_ = vvp_vector8_t(cur_vec4_, 6, 6);
            schedule_init_propagate(net_, cur_vec4_);
      } નહીંતર {
            schedule_init_propagate(net_, cur_real_);
      }
      list_ = 0;
      type_ = UNKNOWN_DELAY;
      initial_ = સાચું;
	// Calculate the values used when converting variable delays
	// to simulation time units.
      અચલ __vpiScope*scope = vpip_peek_current_scope();

      પૂર્ણાંક powr = scope->time_units - scope->time_precision;
      round_ = 1;
      માટે (પૂર્ણાંક lp = 0; lp < powr; lp += 1) round_ *= 10;

      powr = scope->time_precision - vpip_get_time_precision();
      scale_ = 1;
      માટે (પૂર્ણાંક lp = 0; lp < powr; lp += 1) scale_ *= 10;
}

vvp_fun_delay::~vvp_fun_delay()
{
      જ્યારે (સંરચના event_*cur = dequeue_())
	    delete cur;
}

તાર્કિક vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        અચલ vvp_vector4_t&bit)
{
      જો (list_ == 0) પાછા ખોટું;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      જો (list_->next->ptr_vec4.eeq(bit)) પાછા સાચું;

      clean_pulse_events_(use_delay);
      પાછા ખોટું;
}

તાર્કિક vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        અચલ vvp_vector8_t&bit)
{
      જો (list_ == 0) પાછા ખોટું;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      જો (list_->next->ptr_vec8.eeq(bit)) પાછા સાચું;

      clean_pulse_events_(use_delay);
      પાછા ખોટું;
}

તાર્કિક vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        દ્વિગુણ bit)
{
      જો (list_ == 0) પાછા ખોટું;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      જો (list_->next->ptr_real == bit) પાછા સાચું;

      clean_pulse_events_(use_delay);
      પાછા ખોટું;
}

શૂન્ય vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay)
{
      પ્રતિજ્ઞા(list_ != 0);

      કરો {
	    સંરચના event_*cur = list_->next;
	      /* If this event is far enough from the event I'm about
	         to create, then that scheduled event is not a pulse
	         to be eliminated, so we're done. */
	    જો (cur->sim_time+use_delay <= use_delay+schedule_simtime())
		  તોડો;

	    જો (list_ == cur)
		  list_ = 0;
	    નહીંતર
		  list_->next = cur->next;
	    delete cur;
      } જ્યારે (list_);
}

/*
 * FIXME: this implementation currently only uses the maximum delay
 * from all the bit changes in the vectors. If there are multiple
 * changes with different delays, then the results would be
 * wrong. What should happen is that if there are multiple changes,
 * multiple vectors approaching the result should be scheduled.
 */
શૂન્ય vvp_fun_delay::recv_vec4(vvp_net_ptr_t port, અચલ vvp_vector4_t&bit,
                              vvp_context_t)
{
      જો (port.port() > 0) {
	      // Get the integer value of the bit vector, or 0 if
	      // there are X or Z bits.
	    vvp_time64_t bval = 0;
	      // The following does not work correctly for negative values.
	      // They should be sign extended to 64 bits (1364-2001 9.7.1).
	    vector4_to_કિંમત(bit, bval);
	      // Integer values do not need to be rounded so just scale them.
	    vvp_time64_t val = bval * round_ * scale_;

	    પસંદગી (port.port()) {
		સ્થિતિ 1:
		  delay_.set_rise(val);
		  પાછા;
		સ્થિતિ 2:
		  delay_.set_fall(val);
		  પાછા;
		સ્થિતિ 3:
		  delay_.set_decay(val);
		  પાછા;
	    }
	    પાછા;
      }

      vvp_time64_t use_delay;
	/* This is an initial value so it needs to be compared to all the
	   bits (the order the bits are changed is not deterministic). */
      જો (initial_) {
	    type_ = VEC4_DELAY;
            cur_vec8_ = vvp_vector8_t(vvp_vector4_t(0, BIT4_X), 6, 6);
	    vvp_bit4_t cur_val = cur_vec4_.value(0);
	    use_delay = delay_.get_delay(cur_val, bit.value(0));
	    માટે (unsigned idx = 1 ;  idx < bit.size() ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(cur_val, bit.value(idx));
		  જો (tmp > use_delay) use_delay = tmp;
	    }
      } નહીંતર {
	    પ્રતિજ્ઞા(type_ == VEC4_DELAY);

	      // Use as a reference for calculating the delay the
	      // current value of the output. Detect and handle the
	      // special case that the event list contains the current
	      // value as a zero-delay-remaining event.
	    અચલ vvp_vector4_t&use_vec4 = (list_ && list_->next->sim_time == schedule_simtime())? list_->next->ptr_vec4 : cur_vec4_;

	      /* How many bits to compare? */
	    unsigned use_wid = use_vec4.size();
	    જો (bit.size() < use_wid) use_wid = bit.size();

	      /* Scan the vectors looking for delays. Select the maximum
	         delay encountered. */
	    use_delay = delay_.get_delay(use_vec4.value(0), bit.value(0));

	    માટે (unsigned idx = 1 ;  idx < use_wid ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(use_vec4.value(idx), bit.value(idx));
		  જો (tmp > use_delay) use_delay = tmp;
	    }
      }

      /* what *should* happen here is we check to see if there is a
         transaction in the queue. This would be a pulse that needs to be
         eliminated. */
      જો (clean_pulse_events_(use_delay, bit)) પાછા;

      vvp_time64_t use_simtime = schedule_simtime() + use_delay;

	/* And propagate it. */
      જો (use_delay == 0 && list_ == 0) {
	    cur_vec4_ = bit;
	    initial_ = ખોટું;
	    net_->send_vec4(cur_vec4_, 0);
      } નહીંતર {
	    સંરચના event_*cur = નવું સંરચના event_(use_simtime);
	    cur->run_run_ptr = &vvp_fun_delay::run_run_vec4_;
	    cur->ptr_vec4 = bit;
	    enqueue_(cur);
	    schedule_generic(આ, use_delay, ખોટું);
      }
}

શૂન્ય vvp_fun_delay::recv_vec4_pv(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit,
			         unsigned base, unsigned vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

/* See the recv_vec4 comment above. */
શૂન્ય vvp_fun_delay::recv_vec8(vvp_net_ptr_t port, અચલ vvp_vector8_t&bit)
{
      પ્રતિજ્ઞા(port.port() == 0);

      vvp_time64_t use_delay;
	/* This is an initial value so it needs to be compared to all the
	   bits (the order the bits are changed is not deterministic). */
      જો (initial_) {
	    type_ = VEC8_DELAY;
            cur_vec4_ = vvp_vector4_t(0, BIT4_X);
	    vvp_bit4_t cur_val = cur_vec8_.value(0).value();
	    use_delay = delay_.get_delay(cur_val, bit.value(0).value());
	    માટે (unsigned idx = 1 ;  idx < bit.size() ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(cur_val, bit.value(idx).value());
		  જો (tmp > use_delay) use_delay = tmp;
	    }
      } નહીંતર {
	    પ્રતિજ્ઞા(type_ == VEC8_DELAY);

	      // Use as a reference for calculating the delay the
	      // current value of the output. Detect and handle the
	      // special case that the event list contains the current
	      // value as a zero-delay-remaining event.
	    અચલ vvp_vector8_t&use_vec8 = (list_ && list_->next->sim_time == schedule_simtime())? list_->next->ptr_vec8 : cur_vec8_;

	      /* How many bits to compare? */
	    unsigned use_wid = use_vec8.size();
	    જો (bit.size() < use_wid) use_wid = bit.size();

	      /* Scan the vectors looking for delays. Select the maximum
	         delay encountered. */
	    use_delay = delay_.get_delay(use_vec8.value(0).value(),
	                                 bit.value(0).value());

	    માટે (unsigned idx = 1 ;  idx < use_wid ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(use_vec8.value(idx).value(),
		                         bit.value(idx).value());
		  જો (tmp > use_delay) use_delay = tmp;
	    }
      }

      /* what *should* happen here is we check to see if there is a
         transaction in the queue. This would be a pulse that needs to be
         eliminated. */
      જો (clean_pulse_events_(use_delay, bit)) પાછા;

      vvp_time64_t use_simtime = schedule_simtime() + use_delay;

	/* And propagate it. */
      જો (use_delay == 0 && list_ == 0) {
	    cur_vec8_ = bit;
	    initial_ = ખોટું;
	    net_->send_vec8(cur_vec8_);
      } નહીંતર {
	    સંરચના event_*cur = નવું સંરચના event_(use_simtime);
	    cur->ptr_vec8 = bit;
	    cur->run_run_ptr = &vvp_fun_delay::run_run_vec8_;
	    enqueue_(cur);
	    schedule_generic(આ, use_delay, ખોટું);
      }
}

શૂન્ય vvp_fun_delay::recv_vec8_pv(vvp_net_ptr_t ptr, અચલ vvp_vector8_t&bit,
			         unsigned base, unsigned vwid)
{
      recv_vec8_pv_(ptr, bit, base, vwid);
}

શૂન્ય vvp_fun_delay::recv_real(vvp_net_ptr_t port, દ્વિગુણ bit,
                              vvp_context_t)
{
      જો (port.port() > 0) {
	    /* If the port is not 0, then this is a delay value that
	    should be rounded and converted to an integer delay. */
	    vvp_time64_t val = 0;
	    જો (bit > -0.5) {
		  val = (vvp_time64_t) (bit * round_ + 0.5) * scale_;
	    } નહીંતર જો (bit != bit) {
		    // For a NaN we use the default (0).
	    } નહીંતર {
		  vvp_vector4_t vec4(8*કદ(vvp_time64_t),
		                     floor(-bit * round_ + 0.5) * -1 * scale_);
		  vector4_to_કિંમત(vec4, val);
	    }

	    પસંદગી (port.port()) {
		સ્થિતિ 1:
		  delay_.set_rise(val);
		  પાછા;
		સ્થિતિ 2:
		  delay_.set_fall(val);
		  પાછા;
		સ્થિતિ 3:
		  delay_.set_decay(val);
		  પાછા;
	    }
	    પાછા;
      }

      જો (initial_) {
	    type_ = REAL_DELAY;
            cur_vec4_ = vvp_vector4_t(0, BIT4_X);
            cur_vec8_ = vvp_vector8_t(cur_vec4_, 6, 6);
      } નહીંતર પ્રતિજ્ઞા(type_ == REAL_DELAY);

      vvp_time64_t use_delay;
      use_delay = delay_.get_min_delay();

      /* Eliminate glitches. */
      જો (clean_pulse_events_(use_delay, bit)) પાછા;

      /* This must be done after cleaning pulses to avoid propagating
       * an incorrect value. */
      જો (cur_real_ == bit) પાછા;

      vvp_time64_t use_simtime = schedule_simtime() + use_delay;

      જો (use_delay == 0 && list_ == 0) {
	    cur_real_ = bit;
	    initial_ = ખોટું;
	    net_->send_real(cur_real_, 0);
      } નહીંતર {
	    સંરચના event_*cur = નવું સંરચના event_(use_simtime);
	    cur->run_run_ptr = &vvp_fun_delay::run_run_real_;
	    cur->ptr_real = bit;
	    enqueue_(cur);

	    schedule_generic(આ, use_delay, ખોટું);
      }
}

શૂન્ય vvp_fun_delay::run_run()
{
      vvp_time64_t sim_time = schedule_simtime();
      જો (list_ == 0 || list_->next->sim_time > sim_time)
	    પાછા;

      સંરચના event_*cur = dequeue_();
      જો (cur == 0)
	    પાછા;

      (આ->*(cur->run_run_ptr))(cur);
      initial_ = ખોટું;
      delete cur;
}

શૂન્ય vvp_fun_delay::run_run_vec4_(અચલ સંરચના event_*cur)
{
      cur_vec4_ = cur->ptr_vec4;
      net_->send_vec4(cur_vec4_, 0);
}

શૂન્ય vvp_fun_delay::run_run_vec8_(અચલ સંરચના vvp_fun_delay::event_*cur)
{
      cur_vec8_ = cur->ptr_vec8;
      net_->send_vec8(cur_vec8_);
}

શૂન્ય vvp_fun_delay::run_run_real_(અચલ સંરચના vvp_fun_delay::event_*cur)
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
      જ્યારે (src_list_) {
	    vvp_fun_modpath_src*tmp = src_list_;
	    src_list_ = tmp->next_;
	    delete tmp;
      }
      જ્યારે (ifnone_list_) {
	    vvp_fun_modpath_src*tmp = ifnone_list_;
	    ifnone_list_ = tmp->next_;
	    delete tmp;
      }
}

શૂન્ય vvp_fun_modpath::add_modpath_src(vvp_fun_modpath_src*that, તાર્કિક ifnone)
{
      પ્રતિજ્ઞા(that->next_ == 0);
      જો (ifnone) {
	    that->next_ = ifnone_list_;
	    ifnone_list_ = that;
      } નહીંતર {
	    that->next_ = src_list_;
	    src_list_ = that;
      }
}

સ્થિર vvp_time64_t delay_from_edge(vvp_bit4_t a, vvp_bit4_t b,
                                           vvp_time64_t અચલ array[12])
{
      પ્રકારવ્યાખ્યા delay_edge_t bit4_table4[4];
      સ્થિર અચલ bit4_table4 edge_table[4] = {
	    { DELAY_EDGE_01, DELAY_EDGE_01, DELAY_EDGE_0z, DELAY_EDGE_0x },
	    { DELAY_EDGE_10, DELAY_EDGE_10, DELAY_EDGE_1z, DELAY_EDGE_1x },
	    { DELAY_EDGE_z0, DELAY_EDGE_z1, DELAY_EDGE_z0, DELAY_EDGE_zx },
	    { DELAY_EDGE_x0, DELAY_EDGE_x1, DELAY_EDGE_xz, DELAY_EDGE_x0 }
      };

      પાછા array[ edge_table[a][b] ];
}

શૂન્ય vvp_fun_modpath::recv_vec4(vvp_net_ptr_t port, અચલ vvp_vector4_t&bit,
                                vvp_context_t)
{
	/* Only the first port is used. */
      જો (port.port() > 0)
	    પાછા;

      જો (cur_vec4_.eeq(bit))
	    પાછા;

	/* Select a time delay source that applies. Notice that there
	   may be multiple delay sources that apply, so collect all
	   the candidates into a list first. */
      list<vvp_fun_modpath_src*>candidate_list;
      vvp_time64_t candidate_wake_time = 0;
      માટે (vvp_fun_modpath_src*cur = src_list_ ;  cur ;  cur=cur->next_) {
	      /* Skip paths that are disabled by conditions. */
	    જો (cur->condition_flag_ == ખોટું)
		  ચાલુ;

	    જો (candidate_list.empty()) {
		  candidate_list.push_back(cur);
		  candidate_wake_time = cur->wake_time_;
	    } નહીંતર જો (cur->wake_time_ == candidate_wake_time) {
		  candidate_list.push_back(cur);
	    } નહીંતર જો (cur->wake_time_ > candidate_wake_time) {
		  candidate_list.assign(1, cur);
		  candidate_wake_time = cur->wake_time_;
	    } નહીંતર {
		  ચાલુ; /* Skip this entry. */
	    }
      }

	/* Only add the ifnone delay if it has a later wake_time_ or
	 * if there are no normal delays. */
      vvp_time64_t ifnone_wake_time = candidate_wake_time;
      માટે (vvp_fun_modpath_src*cur = ifnone_list_ ;  cur ;  cur=cur->next_) {
	    જો (candidate_list.empty()) {
		  candidate_list.push_back(cur);
		  ifnone_wake_time = cur->wake_time_;
	    } નહીંતર જો (cur->wake_time_ == ifnone_wake_time &&
	               ifnone_wake_time > candidate_wake_time) {
		  candidate_list.push_back(cur);
	    } નહીંતર જો (cur->wake_time_ > ifnone_wake_time) {
		  candidate_list.assign(1, cur);
		  ifnone_wake_time = cur->wake_time_;
	    } નહીંતર {
		  ચાલુ; /* Skip this entry. */
	    }
      }

	/* Handle the special case that there are no delays that
	   match. This may happen, for example, if the set of
	   conditional delays is incomplete, leaving some cases
	   uncovered. In that case, just pass the data without delay */
      જો (candidate_list.empty()) {
	    cur_vec4_ = bit;
	    schedule_generic(આ, 0, ખોટું);
	    પાછા;
      }

	/* Now given that we have a list of candidate delays, find for
	   each if the 12 numbers the minimum from all the
	   candidates. This minimum set becomes the chosen delay to
	   use. */
      vvp_time64_t out_at[12];
      vvp_time64_t now = schedule_simtime();

      પ્રકારવ્યાખ્યા list<vvp_fun_modpath_src*>::const_iterator iter_t;

      iter_t cur = candidate_list.begin();
      અચલ vvp_fun_modpath_src*src = *cur;

      માટે (unsigned idx = 0 ;  idx < 12 ;  idx += 1) {
	    out_at[idx] = src->wake_time_ + src->delay_[idx];
	    જો (out_at[idx] <= now)
		  out_at[idx] = 0;
	    નહીંતર
		  out_at[idx] -= now;
      }

      માટે (++ cur ; cur != candidate_list.end() ; ++ cur ) {
	    src = *cur;
	    માટે (unsigned idx = 0 ;  idx < 12 ;  idx += 1) {
		  vvp_time64_t tmp = src->wake_time_ + src->delay_[idx];
		  જો (tmp <= now)
			tmp = 0;
		  નહીંતર
			tmp -= now;
		  જો (tmp < out_at[idx])
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
      માટે (unsigned idx = 1 ;  idx < bit.size() ;  idx += 1) {
	    vvp_time64_t tmp = delay_from_edge(cur_vec4_.value(idx),
					       bit.value(idx),
					       out_at);
	      /* If the current and new bit values match then no delay
	       * is needed for this bit. */
	    જો (cur_vec4_.value(idx) == bit.value(idx)) ચાલુ;
	    જો (tmp != use_delay) {
		  fprintf(stderr, "sorry: multi-bit module path delays are "
		                  "currently not fully supported.\n");
		  fprintf(stderr, "     : using the LSB delay for all bits.\n");
		  ચાલુ;
            }
      }

      cur_vec4_ = bit;
      schedule_generic(આ, use_delay, ખોટું);
}

શૂન્ય vvp_fun_modpath::run_run()
{
      net_->send_vec4(cur_vec4_, 0);
}

vvp_fun_modpath_src::vvp_fun_modpath_src(vvp_time64_t અચલ del[12])
{
      માટે (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = del[idx];

      next_ = 0;
      wake_time_ = 0;
      condition_flag_ = સાચું;
}

vvp_fun_modpath_src::~vvp_fun_modpath_src()
{
}

શૂન્ય vvp_fun_modpath_src::get_delay12(vvp_time64_t val[12]) અચલ
{
      માટે (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    val[idx] = delay_[idx];
}

શૂન્ય vvp_fun_modpath_src::put_delay12(અચલ vvp_time64_t val[12])
{
      માટે (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = val[idx];
}

શૂન્ય vvp_fun_modpath_src::recv_vec4(vvp_net_ptr_t port, અચલ vvp_vector4_t&bit,
                                    vvp_context_t)
{
      જો (port.port() == 0) {
	      // The modpath input...
	    જો (test_vec4(bit))
		  wake_time_ = schedule_simtime();

      } નહીંતર જો (port.port() == 1) {
	      // The modpath condition input...
	    જો (bit.value(0) != BIT4_0)
		  condition_flag_ = સાચું;
	    નહીંતર
		  condition_flag_ = ખોટું;
      }
}

તાર્કિક vvp_fun_modpath_src::test_vec4(અચલ vvp_vector4_t&)
{
      પાછા સાચું;
}

vvp_fun_modpath_edge::vvp_fun_modpath_edge(vvp_time64_t del[12],
					   તાર્કિક pos, તાર્કિક neg)
: vvp_fun_modpath_src(del)
{
      old_કિંમત = BIT4_X;
      posedge_ = pos;
      negedge_ = neg;
}

તાર્કિક vvp_fun_modpath_edge::test_vec4(અચલ vvp_vector4_t&bit)
{
      vvp_bit4_t tmp = old_કિંમત;
      old_કિંમત = bit.value(0);

      પૂર્ણાંક edge_flag = edge(tmp, old_કિંમત);
      જો (edge_flag > 0) પાછા posedge_;
      જો (edge_flag < 0) પાછા negedge_;
      પાછા ખોટું;
}


/*
 * All the below routines that begin with
 * modpath_src_* belong the internal function
 * of an vpiModPathIn object. This is used to
 * make some specific delays path operations
 *
 */
સ્થિર પૂર્ણાંક modpath_src_get(પૂર્ણાંક, vpiHandle ref)
{
      સંરચના __vpiModPathSrc*obj =dynamic_cast<__vpiModPathSrc*>(ref);
      પ્રતિજ્ઞા(obj);
      પાછા 0;
}

સ્થિર શૂન્ય modpath_src_get_કિંમત(vpiHandle ref, p_vpi_કિંમત)
{
      સંરચના __vpiModPathSrc* modpathsrc = dynamic_cast<__vpiModPathSrc*>(ref);
      પ્રતિજ્ઞા(modpathsrc);
      પાછા;
}

સ્થિર vpiHandle modpath_src_put_કિંમત(vpiHandle ref, s_vpi_કિંમત *, પૂર્ણાંક )
{
      સંરચના __vpiModPathSrc* modpathsrc = dynamic_cast<__vpiModPathSrc*>(ref);
      પ્રતિજ્ઞા(modpathsrc);
      પાછા 0;
}

સ્થિર vpiHandle modpath_src_get_handle(પૂર્ણાંક code, vpiHandle ref)
{
      સંરચના __vpiModPathSrc*rfp = dynamic_cast<__vpiModPathSrc*>(ref);
      પ્રતિજ્ઞા(rfp);

      પસંદગી (code) {

	સ્થિતિ vpiScope:
	  પાછા rfp->dest->scope;

	  સ્થિતિ vpiModule:
	      { __vpiScope*scope = rfp->dest->scope;
		જ્યારે (scope && scope->get_type_code() != vpiModule)
		      scope = scope->scope;
		પ્રતિજ્ઞા(scope);
		પાછા scope;
	      }

	    // Handles to path term objects should really be obtained via
	    // the vpi_iterate and vpi_scan functions. Continue to allow
	    // them to be obtained here for backwards compatibility with
	    // older versions of Icarus Verilog.

	  સ્થિતિ vpiModPathIn:
	    પાછા &rfp->path_term_in;

	  સ્થિતિ vpiModPathOut:
	    પાછા &rfp->dest->path_term_out;
      }
      પાછા 0;
}

સ્થિર vpiHandle modpath_src_iterate(પૂર્ણાંક code, vpiHandle ref)
{
      સંરચના __vpiModPathSrc*rfp = dynamic_cast<__vpiModPathSrc*>(ref);
      પ્રતિજ્ઞા(rfp);

	// Module paths with multiple sources or destinations are
	// currently represented by a separate modpath object for
	// each source/destination combination, so there is only
	// ever one input path term and one output path term.
      પસંદગી (code) {
	  સ્થિતિ vpiModPathIn: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(1, કદ(vpiHandle*)));
	    args[0] = &rfp->path_term_in;
	    પાછા vpip_make_iterator(1, args, સાચું);
	  }
	  સ્થિતિ vpiModPathOut: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(1, કદ(vpiHandle*)));
	    args[0] = &rfp->dest->path_term_out;
	    પાછા vpip_make_iterator(1, args, સાચું);
	  }
      }
      પાછા 0;
}

સ્થિર vpiHandle modpath_src_index ( vpiHandle ref, પૂર્ણાંક)
{
      પ્રતિજ્ઞા(ref->get_type_code() == vpiModPathIn);
      પાછા 0;
}


/*
 * This routine will put specific dimension of delay[] values
 * into a vpiHandle. In this case, we will put
 * specific delays values in a vpiModPathIn object
 *
 */
સ્થિર શૂન્ય modpath_src_put_delays (vpiHandle ref, p_vpi_delay delays)
{
      vvp_time64_t tmp[12];
      પૂર્ણાંક idx;
      સંરચના __vpiModPathSrc * src = dynamic_cast<__vpiModPathSrc*>(ref) ;
      પ્રતિજ્ઞા(src) ;

      vvp_fun_modpath_src *fun = dynamic_cast<vvp_fun_modpath_src*>(src->net->fun);
      પ્રતિજ્ઞા( fun );

      પ્રકારવ્યાખ્યા unsigned અક્ષર map_array_t[12];
	// Only the first six entries are used for the less than twelve maps.
      સ્થિર અચલ map_array_t map_1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      સ્થિર અચલ map_array_t map_2 = {0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0};
      સ્થિર અચલ map_array_t map_3 = {0, 1, 2, 0, 2, 1, 0, 0, 0, 0, 0, 0};
      સ્થિર અચલ map_array_t map_6 = {0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0};
      સ્થિર અચલ map_array_t map12 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

      અચલ map_array_t*use_map = 0;
      પસંદગી (delays->no_of_delays) {
	  સ્થિતિ 1:
	    use_map = &map_1;
	    તોડો;
	  સ્થિતિ 2:
	    use_map = &map_2;
	    તોડો;
	  સ્થિતિ 3:
	    use_map = &map_3;
	    તોડો;
	  સ્થિતિ 6:
	    use_map = &map_6;
	    તોડો;
	  સ્થિતિ 12:
	    use_map = &map12;
	    તોડો;
	  મૂળભૂત:
	    પ્રતિજ્ઞા(0);
	    તોડો;
      }

      જો (delays->time_type == vpiSimTime) {
	    માટે (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_timestruct_to_time(delays->da+use_map[0][idx]);
	    }
      } નહીંતર {
	      // You cannot create a modpath with a negative delay so set it
	      // to zero per 1364-2005 section 14.3.1.
	    માટે (idx = 0 ; idx < delays->no_of_delays ; idx += 1) {
		  જો (delays->da[idx].real < 0.0) delays->da[idx].real = 0.0;
	    }
	    માટે (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_scaled_real_to_time64(delays->da[use_map[0][idx]].real,
							src->dest->scope);
	    }
      }

      /* Now define the to-from-x delays if needed. */
      જો (delays->no_of_delays <= 6) {
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

સ્થિર શૂન્ય modpath_src_get_delays ( vpiHandle ref, p_vpi_delay delays )
{
      સંરચના __vpiModPathSrc*src = dynamic_cast<__vpiModPathSrc*>(ref) ;
      પ્રતિજ્ઞા(src);

      vvp_fun_modpath_src *fun = dynamic_cast<vvp_fun_modpath_src*>(src->net->fun);
      પ્રતિજ્ઞા(fun);

      પૂર્ણાંક idx;
      vvp_time64_t tmp[12];
      fun->get_delay12(tmp);

      પસંદગી (delays->no_of_delays) {
	  સ્થિતિ 1:
	  સ્થિતિ 2:
	  સ્થિતિ 3:
	  સ્થિતિ 6:
	  સ્થિતિ 12:
	    તોડો;

	  મૂળભૂત:
	    પ્રતિજ્ઞા(0);
	    તોડો;
      }

      જો (delays->time_type == vpiSimTime) {
	    માટે (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  vpip_time_to_timestruct(delays->da+idx, tmp[idx]);
	    }
      } નહીંતર {
	    માટે (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  delays->da[idx].real = vpip_time_to_scaled_real(tmp[idx], src->dest->scope);
	    }
      }
}

સ્થિર પૂર્ણાંક pathterm_get(પૂર્ણાંક code, vpiHandle ref)
{
      સંરચના __vpiModPathTerm*obj = dynamic_cast<__vpiModPathTerm*>(ref);
      પ્રતિજ્ઞા(obj);

      પસંદગી (code) {
	  સ્થિતિ vpiEdge:
	    પાછા obj->edge;
	  મૂળભૂત:
	    પાછા 0;
      }
}

સ્થિર vpiHandle pathterm_get_handle(પૂર્ણાંક code, vpiHandle ref)
{
      સંરચના __vpiModPathTerm*obj = dynamic_cast<__vpiModPathTerm*>(ref);
      પ્રતિજ્ઞા(obj);

      પસંદગી (code) {
	  સ્થિતિ vpi_પદાવલિ:
	    પાછા obj->પદાવલિ;
	  મૂળભૂત:
	    પાછા 0;
      }
}

/*
* The __vpiModPathSrc class is what the VPI client sees as a
* vpiModPath object. The __vpiModPath structure contains items that
* are common to a bunch of modpaths, including the destination term.
*/
અંતઃરેખ __vpiModPathSrc::__vpiModPathSrc()
{ }

પૂર્ણાંક __vpiModPathSrc::get_type_code(શૂન્ય) અચલ
{ પાછા vpiModPath; }

પૂર્ણાંક __vpiModPathSrc::vpi_get(પૂર્ણાંક code)
{ પાછા modpath_src_get(code, આ); }

શૂન્ય __vpiModPathSrc::vpi_get_કિંમત(p_vpi_કિંમત val)
{ modpath_src_get_કિંમત(આ, val); }

vpiHandle __vpiModPathSrc::vpi_put_કિંમત(p_vpi_કિંમત val, પૂર્ણાંક flags)
{ પાછા modpath_src_put_કિંમત(આ, val, flags); }

vpiHandle __vpiModPathSrc::vpi_handle(પૂર્ણાંક code)
{ પાછા modpath_src_get_handle(code, આ); }

vpiHandle __vpiModPathSrc::vpi_iterate(પૂર્ણાંક code)
{ પાછા modpath_src_iterate(code, આ); }

vpiHandle __vpiModPathSrc:: vpi_index(પૂર્ણાંક idx)
{ પાછા modpath_src_index(આ, idx); }

શૂન્ય __vpiModPathSrc::vpi_get_delays(p_vpi_delay del)
{ modpath_src_get_delays(આ, del); }

શૂન્ય __vpiModPathSrc::vpi_put_delays(p_vpi_delay del)
{ modpath_src_put_delays(આ, del); }

સ્થિર પૂર્ણાંક modpath_src_free_object( vpiHandle ref )
{
      delete ref;
      પાછા 1 ;
}

__vpiHandle::free_object_fun_t __vpiModPathSrc::free_object_fun(શૂન્ય)
{ પાછા &modpath_src_free_object; }


અંતઃરેખ __vpiModPathTerm::__vpiModPathTerm()
{ }

પૂર્ણાંક __vpiModPathTerm::get_type_code(શૂન્ય) અચલ
{ પાછા vpiPathTerm; }

પૂર્ણાંક __vpiModPathTerm::vpi_get(પૂર્ણાંક code)
{ પાછા pathterm_get(code, આ); }

vpiHandle __vpiModPathTerm::vpi_handle(પૂર્ણાંક code)
{ પાછા pathterm_get_handle(code, આ); }

સ્થિર શૂન્ય initialize_path_term(સંરચના __vpiModPathTerm&obj)
{
      obj.પદાવલિ = 0;
      obj.edge = vpiNoEdge;
}

/*
 * This function will construct a vpiModPath Object.
 * give a respective "net", and will point to his
 * respective functor
 */

#ifdef CHECK_WITH_VALGRIND
સ્થિર સંરચના __vpiModPath**mp_list = 0;
સ્થિર unsigned mp_ગણતરી = 0;
#endif

સંરચના __vpiModPath* vpip_make_modpath(vvp_net_t *net)
{
      સંરચના __vpiModPath*obj = નવું __vpiModPath;
      obj->scope = vpip_peek_current_scope ( );

      initialize_path_term(obj->path_term_out);
      obj->આગત_net = net ;

#ifdef CHECK_WITH_VALGRIND
      mp_ગણતરી += 1;
      mp_list = static_cast<સંરચના __vpiModPath **>
                (realloc(mp_list,
                 mp_ગણતરી*કદ(સંરચના __vpiModPath **)));
      mp_list[mp_ગણતરી-1] = obj;
#endif
      પાછા obj;
}

#ifdef CHECK_WITH_VALGRIND
શૂન્ય modpath_delete()
{
      માટે (unsigned idx = 0; idx < mp_ગણતરી; idx += 1) {
	    delete mp_list[idx];
      }
      મુક્ત(mp_list);
      mp_list = 0;
      mp_ગણતરી = 0;
}
#endif

/*
 * This function will construct a vpiModPathIn
 * ( struct __vpiModPathSrc ) Object. will give
 * a delays[12] values, and point to the specified functor
 *
 */

સંરચના __vpiModPathSrc* vpip_make_modpath_src(સંરચના __vpiModPath*path,
                                              vvp_net_t *net)
{
      સંરચના __vpiModPathSrc *obj = નવું __vpiModPathSrc;

      obj->dest = path;
      obj->type = 0;
      obj->net = net;
      initialize_path_term(obj->path_term_in);

      પાછા obj;
}

vvp_fun_intermodpath::vvp_fun_intermodpath(vvp_net_t*net, unsigned width)
: net_(net)
{
      માટે (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = 0;

      cur_vec4_ = vvp_vector4_t(width, BIT4_X);
      schedule_init_propagate(net_, cur_vec4_);
}

vvp_fun_intermodpath::~vvp_fun_intermodpath()
{
}

શૂન્ય vvp_fun_intermodpath::get_delay12(vvp_time64_t val[12]) અચલ
{
      માટે (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    val[idx] = delay_[idx];
}

શૂન્ય vvp_fun_intermodpath::put_delay12(અચલ vvp_time64_t val[12])
{
      માટે (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = val[idx];
}

/*
 * FIXME: this implementation currently only uses the maximum delay
 * from all the bit changes in the vectors. If there are multiple
 * changes with different delays, then the results would be
 * wrong. What should happen is that if there are multiple changes,
 * multiple vectors approaching the result should be scheduled.
 */
શૂન્ય vvp_fun_intermodpath::recv_vec4(vvp_net_ptr_t port, અચલ vvp_vector4_t&bit,
                              vvp_context_t)
{
	/* Only the first port is used. */
      જો (port.port() > 0)
	    પાછા;

      જો (cur_vec4_.eeq(bit))
	    પાછા;

	/* Given the scheduled output time, create an output event. */
      vvp_time64_t use_delay = delay_from_edge(cur_vec4_.value(0),
					       bit.value(0),
					       delay_);

	/* FIXME: This bases the edge delay on only the least
	   bit. This is WRONG! I need to find all the possible delays,
	   and schedule an event for each partial change. Hard! */
      માટે (unsigned idx = 1 ;  idx < bit.size() ;  idx += 1) {
	    vvp_time64_t tmp = delay_from_edge(cur_vec4_.value(idx),
					       bit.value(idx),
					       delay_);
	      /* If the current and new bit values match then no delay
	       * is needed for this bit. */
	    જો (cur_vec4_.value(idx) == bit.value(idx)) ચાલુ;
	    પ્રતિજ્ઞા(tmp == use_delay);
      }

      cur_vec4_ = bit;
      schedule_generic(આ, use_delay, ખોટું);
}

શૂન્ય vvp_fun_intermodpath::run_run()
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
સ્થિર પૂર્ણાંક intermodpath_get(પૂર્ણાંક, vpiHandle ref)
{
      સંરચના __vpiInterModPath*obj =dynamic_cast<__vpiInterModPath*>(ref);
      પ્રતિજ્ઞા(obj);
      પાછા 0;
}

સ્થિર શૂન્ય intermodpath_get_કિંમત(vpiHandle ref, p_vpi_કિંમત)
{
      સંરચના __vpiInterModPath* intermodpath = dynamic_cast<__vpiInterModPath*>(ref);
      પ્રતિજ્ઞા(intermodpath);
      પાછા;
}

સ્થિર vpiHandle intermodpath_put_કિંમત(vpiHandle ref, s_vpi_કિંમત *, પૂર્ણાંક )
{
      સંરચના __vpiInterModPath* intermodpath = dynamic_cast<__vpiInterModPath*>(ref);
      પ્રતિજ્ઞા(intermodpath);
      પાછા 0;
}

સ્થિર vpiHandle intermodpath_get_handle(પૂર્ણાંક code, vpiHandle ref)
{
      સંરચના __vpiInterModPath*rfp = dynamic_cast<__vpiInterModPath*>(ref);
      પ્રતિજ્ઞા(rfp);

      પસંદગી (code) {

	સ્થિતિ vpiScope:
	  પાછા rfp->scope;

	  સ્થિતિ vpiModule:
	      { __vpiScope*scope = rfp->scope;
		જ્યારે (scope && scope->get_type_code() != vpiModule)
		      scope = scope->scope;
		પ્રતિજ્ઞા(scope);
		પાછા scope;
	      }
      }
      પાછા 0;
}

સ્થિર vpiHandle intermodpath_iterate(પૂર્ણાંક code, vpiHandle ref)
{
      સંરચના __vpiInterModPath*rfp = dynamic_cast<__vpiInterModPath*>(ref);
      પ્રતિજ્ઞા(rfp);

	// For now intermodpaths only support exactly two ports
      પસંદગી (code) {
	  સ્થિતિ vpiPort: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(2, કદ(vpiHandle*)));
	    args[0] = rfp->port1;
	    args[1] = rfp->port2;
	    પાછા vpip_make_iterator(2, args, સાચું);
	  }
      }
      પાછા 0;
}

/*
 * This routine will put specific dimension of delay[] values
 * into a vpiHandle. In this case, we will put
 * specific delays values in a vpiInterModPath object
 *
 */
સ્થિર શૂન્ય intermodpath_put_delays (vpiHandle ref, p_vpi_delay delays)
{
      vvp_time64_t tmp[12];
      પૂર્ણાંક idx;
      સંરચના __vpiInterModPath * src = dynamic_cast<__vpiInterModPath*>(ref) ;
      પ્રતિજ્ઞા(src) ;

      vvp_fun_intermodpath *fun = dynamic_cast<vvp_fun_intermodpath*>(src->net->fun);
      પ્રતિજ્ઞા( fun );

      પ્રકારવ્યાખ્યા unsigned અક્ષર map_array_t[12];
	// Only the first six entries are used for the less than twelve maps.
      સ્થિર અચલ map_array_t map_1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      સ્થિર અચલ map_array_t map_2 = {0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0};
      સ્થિર અચલ map_array_t map_3 = {0, 1, 2, 0, 2, 1, 0, 0, 0, 0, 0, 0};
      સ્થિર અચલ map_array_t map_6 = {0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0};
      સ્થિર અચલ map_array_t map12 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

      અચલ map_array_t*use_map = 0;
      પસંદગી (delays->no_of_delays) {
	  સ્થિતિ 1:
	    use_map = &map_1;
	    તોડો;
	  સ્થિતિ 2:
	    use_map = &map_2;
	    તોડો;
	  સ્થિતિ 3:
	    use_map = &map_3;
	    તોડો;
	  સ્થિતિ 6:
	    use_map = &map_6;
	    તોડો;
	  સ્થિતિ 12:
	    use_map = &map12;
	    તોડો;
	  મૂળભૂત:
	    પ્રતિજ્ઞા(0);
	    તોડો;
      }

      જો (delays->time_type == vpiSimTime) {
	    માટે (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_timestruct_to_time(delays->da+use_map[0][idx]);
	    }
      } નહીંતર {
	      // You cannot create a modpath with a negative delay so set it
	      // to zero per 1364-2005 section 14.3.1.
	    માટે (idx = 0 ; idx < delays->no_of_delays ; idx += 1) {
		  જો (delays->da[idx].real < 0.0) delays->da[idx].real = 0.0;
	    }
	    માટે (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_scaled_real_to_time64(delays->da[use_map[0][idx]].real,
							src->scope);
	    }
      }

      /* Now define the to-from-x delays if needed. */
      જો (delays->no_of_delays <= 6) {
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

સ્થિર શૂન્ય intermodpath_get_delays ( vpiHandle ref, p_vpi_delay delays )
{
      સંરચના __vpiInterModPath*src = dynamic_cast<__vpiInterModPath*>(ref) ;
      પ્રતિજ્ઞા(src);

      vvp_fun_intermodpath *fun = dynamic_cast<vvp_fun_intermodpath*>(src->net->fun);
      પ્રતિજ્ઞા(fun);

      પૂર્ણાંક idx;
      vvp_time64_t tmp[12];
      fun->get_delay12(tmp);

      પસંદગી (delays->no_of_delays) {
	  સ્થિતિ 1:
	  સ્થિતિ 2:
	  સ્થિતિ 3:
	  સ્થિતિ 6:
	  સ્થિતિ 12:
	    તોડો;

	  મૂળભૂત:
	    પ્રતિજ્ઞા(0);
	    તોડો;
      }

      જો (delays->time_type == vpiSimTime) {
	    માટે (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  vpip_time_to_timestruct(delays->da+idx, tmp[idx]);
	    }
      } નહીંતર {
	    માટે (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  delays->da[idx].real = vpip_time_to_scaled_real(tmp[idx], src->scope);
	    }
      }
}

/*
* The __vpiInterModPath class is what the VPI client sees as a
* vpiInterModPath object.
*/
અંતઃરેખ __vpiInterModPath::__vpiInterModPath()
{ }

પૂર્ણાંક __vpiInterModPath::get_type_code(શૂન્ય) અચલ
{ પાછા vpiInterModPath; }

પૂર્ણાંક __vpiInterModPath::vpi_get(પૂર્ણાંક code)
{ પાછા intermodpath_get(code, આ); }

શૂન્ય __vpiInterModPath::vpi_get_કિંમત(p_vpi_કિંમત val)
{ intermodpath_get_કિંમત(આ, val); }

vpiHandle __vpiInterModPath::vpi_put_કિંમત(p_vpi_કિંમત val, પૂર્ણાંક flags)
{ પાછા intermodpath_put_કિંમત(આ, val, flags); }

vpiHandle __vpiInterModPath::vpi_handle(પૂર્ણાંક code)
{ પાછા intermodpath_get_handle(code, આ); }

vpiHandle __vpiInterModPath::vpi_iterate(પૂર્ણાંક code)
{ પાછા intermodpath_iterate(code, આ); }

શૂન્ય __vpiInterModPath::vpi_get_delays(p_vpi_delay del)
{ intermodpath_get_delays(આ, del); }

શૂન્ય __vpiInterModPath::vpi_put_delays(p_vpi_delay del)
{ intermodpath_put_delays(આ, del); }

સ્થિર પૂર્ણાંક intermodpath_free_object( vpiHandle ref )
{
      delete ref;
      પાછા 1 ;
}

__vpiHandle::free_object_fun_t __vpiInterModPath::free_object_fun(શૂન્ય)
{ પાછા &intermodpath_free_object; }

/*
 * This function will construct a vpiInterModPath Object.
 * give a respective "net", and will point to his
 * respective functor
 */

#ifdef CHECK_WITH_VALGRIND
સ્થિર સંરચના __vpiInterModPath**imp_list = 0;
સ્થિર unsigned imp_ગણતરી = 0;
#endif

સંરચના __vpiInterModPath* vpip_make_intermodpath(vvp_net_t *net, vpiPortInfo* port1, vpiPortInfo* port2)
{
      સંરચના __vpiInterModPath*obj = નવું __vpiInterModPath;
      obj->scope = vpip_peek_current_scope ( );

      obj->net = net;
      obj->port1 = port1;
      obj->port2 = port2;

#ifdef CHECK_WITH_VALGRIND
      imp_ગણતરી += 1;
      imp_list = static_cast<સંરચના __vpiInterModPath **>
                 (realloc(imp_list,
                  imp_ગણતરી*કદ(સંરચના __vpiInterModPath **)));
      imp_list[imp_ગણતરી-1] = obj;
#endif
      પાછા obj;
}

#ifdef CHECK_WITH_VALGRIND
શૂન્ય intermodpath_delete()
{
      માટે (unsigned idx = 0; idx < mp_ગણતરી; idx += 1) {
	    delete imp_list[idx];
      }
      મુક્ત(imp_list);
      imp_list = 0;
      imp_ગણતરી = 0;
}
#endif
