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

#അന്തര്ഭാവ "delay.h"
#അന്തര്ഭാവ "schedule.h"
#അന്തര്ഭാവ "vpi_priv.h"
#അന്തര്ഭാവ "config.h"
#ifdef CHECK_WITH_VALGRIND
#അന്തര്ഭാവ "vvp_cleanup.h"
#endif
#അന്തര്ഭാവ <iostream>
#അന്തര്ഭാവ <cstdlib>
#അന്തര്ഭാവ <list>
#അന്തര്ഭാവ <cassert>
#അന്തര്ഭാവ <cmath>
#അന്തര്ഭാവ "ivl_alloc.h"

using നാമാകാശ std;

ശൂന്യം vvp_delay_t::calculate_min_delay_()
{
      min_delay_ = rise_;
      എങ്കിൽ (fall_ < min_delay_)
	    min_delay_ = fall_;
      എങ്കിൽ (ignore_decay_) decay_ = min_delay_;
      അല്ലെങ്കിൽ എങ്കിൽ (decay_ < min_delay_)
	    min_delay_ = decay_;
}

vvp_delay_t::vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall)
{
      rise_ = rise;
      fall_ = fall;
      decay_= fall < rise? fall : rise;
      min_delay_ = decay_;
      ignore_decay_ = അസത്യം;
}

vvp_delay_t::vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall, vvp_time64_t decay)
{
      rise_ = rise;
      fall_ = fall;
      decay_= decay;
      ignore_decay_ = അസത്യം;

      calculate_min_delay_();
}

ശൂന്യം vvp_delay_t::set_ignore_decay()
{
      ignore_decay_ = സത്യം;

      calculate_min_delay_();
}

vvp_delay_t::~vvp_delay_t()
{
}

vvp_time64_t vvp_delay_t::get_delay(vvp_bit4_t from, vvp_bit4_t to)
{
      തിരഞ്ഞെടുപ്പ് (from) {
	  അവസ്ഥ BIT4_0:
	    തിരഞ്ഞെടുപ്പ് (to) {
		അവസ്ഥ BIT4_0: തിരികെ 0;
		അവസ്ഥ BIT4_1: തിരികെ rise_;
		അവസ്ഥ BIT4_X: തിരികെ min_delay_;
		അവസ്ഥ BIT4_Z: തിരികെ decay_;
	    }
	    നിർത്തുക;
	  അവസ്ഥ BIT4_1:
	    തിരഞ്ഞെടുപ്പ് (to) {
		അവസ്ഥ BIT4_0: തിരികെ fall_;
		അവസ്ഥ BIT4_1: തിരികെ 0;
		അവസ്ഥ BIT4_X: തിരികെ min_delay_;
		അവസ്ഥ BIT4_Z: തിരികെ decay_;
	    }
	    നിർത്തുക;
	  അവസ്ഥ BIT4_X:
	    തിരഞ്ഞെടുപ്പ് (to) {
		അവസ്ഥ BIT4_0: തിരികെ fall_;
		അവസ്ഥ BIT4_1: തിരികെ rise_;
		അവസ്ഥ BIT4_X: തിരികെ 0;
		അവസ്ഥ BIT4_Z: തിരികെ decay_;
	    }
	    നിർത്തുക;
	  അവസ്ഥ BIT4_Z:
	    തിരഞ്ഞെടുപ്പ് (to) {
		അവസ്ഥ BIT4_0: തിരികെ fall_;
		അവസ്ഥ BIT4_1: തിരികെ rise_;
		അവസ്ഥ BIT4_X: തിരികെ min_delay_;
		അവസ്ഥ BIT4_Z: തിരികെ 0;
	    }
	    നിർത്തുക;
      }

      പ്രതിജ്ഞാ(0);
      തിരികെ 0;
}

vvp_time64_t vvp_delay_t::get_min_delay() സ്ഥിരം
{
      തിരികെ min_delay_;
}

ശൂന്യം vvp_delay_t::set_rise(vvp_time64_t val)
{
      rise_ = val;
      എങ്കിൽ (val < min_delay_) {
	    min_delay_ = val;
	    എങ്കിൽ (ignore_decay_) decay_ = val;
      } അല്ലെങ്കിൽ
	    calculate_min_delay_();
}

ശൂന്യം vvp_delay_t::set_fall(vvp_time64_t val)
{
      fall_ = val;
      എങ്കിൽ (val < min_delay_) {
	    min_delay_ = val;
	    എങ്കിൽ (ignore_decay_) decay_ = val;
      } അല്ലെങ്കിൽ
	    calculate_min_delay_();
}

ശൂന്യം vvp_delay_t::set_decay(vvp_time64_t val)
{
      പ്രതിജ്ഞാ(!ignore_decay_);

      decay_ = val;
      എങ്കിൽ (val < min_delay_)
	    min_delay_ = val;
      അല്ലെങ്കിൽ
	    calculate_min_delay_();
}

vvp_fun_delay::vvp_fun_delay(vvp_net_t*n, unsigned width, സ്ഥിരം vvp_delay_t&d)
: net_(n), delay_(d)
{
      cur_real_ = 0.0;
      എങ്കിൽ (width > 0) {
            cur_vec4_ = vvp_vector4_t(width, BIT4_X);
            cur_vec8_ = vvp_vector8_t(cur_vec4_, 6, 6);
            schedule_init_propagate(net_, cur_vec4_);
      } അല്ലെങ്കിൽ {
            schedule_init_propagate(net_, cur_real_);
      }
      list_ = 0;
      type_ = UNKNOWN_DELAY;
      initial_ = സത്യം;
	// Calculate the values used when converting variable delays
	// to simulation time units.
      സ്ഥിരം __vpiScope*scope = vpip_peek_current_scope();

      പൂർണ്ണസംഖ്യ powr = scope->time_units - scope->time_precision;
      round_ = 1;
      വേണ്ടി (പൂർണ്ണസംഖ്യ lp = 0; lp < powr; lp += 1) round_ *= 10;

      powr = scope->time_precision - vpip_get_time_precision();
      scale_ = 1;
      വേണ്ടി (പൂർണ്ണസംഖ്യ lp = 0; lp < powr; lp += 1) scale_ *= 10;
}

vvp_fun_delay::~vvp_fun_delay()
{
      ആകുമ്പോൾ (ഘടന event_*cur = dequeue_())
	    delete cur;
}

തർക്കം vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        സ്ഥിരം vvp_vector4_t&bit)
{
      എങ്കിൽ (list_ == 0) തിരികെ അസത്യം;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      എങ്കിൽ (list_->next->ptr_vec4.eeq(bit)) തിരികെ സത്യം;

      clean_pulse_events_(use_delay);
      തിരികെ അസത്യം;
}

തർക്കം vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        സ്ഥിരം vvp_vector8_t&bit)
{
      എങ്കിൽ (list_ == 0) തിരികെ അസത്യം;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      എങ്കിൽ (list_->next->ptr_vec8.eeq(bit)) തിരികെ സത്യം;

      clean_pulse_events_(use_delay);
      തിരികെ അസത്യം;
}

തർക്കം vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        ഇരട്ട bit)
{
      എങ്കിൽ (list_ == 0) തിരികെ അസത്യം;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      എങ്കിൽ (list_->next->ptr_real == bit) തിരികെ സത്യം;

      clean_pulse_events_(use_delay);
      തിരികെ അസത്യം;
}

ശൂന്യം vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay)
{
      പ്രതിജ്ഞാ(list_ != 0);

      ചെയ്യുക {
	    ഘടന event_*cur = list_->next;
	      /* If this event is far enough from the event I'm about
	         to create, then that scheduled event is not a pulse
	         to be eliminated, so we're done. */
	    എങ്കിൽ (cur->sim_time+use_delay <= use_delay+schedule_simtime())
		  നിർത്തുക;

	    എങ്കിൽ (list_ == cur)
		  list_ = 0;
	    അല്ലെങ്കിൽ
		  list_->next = cur->next;
	    delete cur;
      } ആകുമ്പോൾ (list_);
}

/*
 * FIXME: this implementation currently only uses the maximum delay
 * from all the bit changes in the vectors. If there are multiple
 * changes with different delays, then the results would be
 * wrong. What should happen is that if there are multiple changes,
 * multiple vectors approaching the result should be scheduled.
 */
ശൂന്യം vvp_fun_delay::recv_vec4(vvp_net_ptr_t port, സ്ഥിരം vvp_vector4_t&bit,
                              vvp_context_t)
{
      എങ്കിൽ (port.port() > 0) {
	      // Get the integer value of the bit vector, or 0 if
	      // there are X or Z bits.
	    vvp_time64_t bval = 0;
	      // The following does not work correctly for negative values.
	      // They should be sign extended to 64 bits (1364-2001 9.7.1).
	    vector4_to_മൂല്യം(bit, bval);
	      // Integer values do not need to be rounded so just scale them.
	    vvp_time64_t val = bval * round_ * scale_;

	    തിരഞ്ഞെടുപ്പ് (port.port()) {
		അവസ്ഥ 1:
		  delay_.set_rise(val);
		  തിരികെ;
		അവസ്ഥ 2:
		  delay_.set_fall(val);
		  തിരികെ;
		അവസ്ഥ 3:
		  delay_.set_decay(val);
		  തിരികെ;
	    }
	    തിരികെ;
      }

      vvp_time64_t use_delay;
	/* This is an initial value so it needs to be compared to all the
	   bits (the order the bits are changed is not deterministic). */
      എങ്കിൽ (initial_) {
	    type_ = VEC4_DELAY;
            cur_vec8_ = vvp_vector8_t(vvp_vector4_t(0, BIT4_X), 6, 6);
	    vvp_bit4_t cur_val = cur_vec4_.value(0);
	    use_delay = delay_.get_delay(cur_val, bit.value(0));
	    വേണ്ടി (unsigned idx = 1 ;  idx < bit.size() ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(cur_val, bit.value(idx));
		  എങ്കിൽ (tmp > use_delay) use_delay = tmp;
	    }
      } അല്ലെങ്കിൽ {
	    പ്രതിജ്ഞാ(type_ == VEC4_DELAY);

	      // Use as a reference for calculating the delay the
	      // current value of the output. Detect and handle the
	      // special case that the event list contains the current
	      // value as a zero-delay-remaining event.
	    സ്ഥിരം vvp_vector4_t&use_vec4 = (list_ && list_->next->sim_time == schedule_simtime())? list_->next->ptr_vec4 : cur_vec4_;

	      /* How many bits to compare? */
	    unsigned use_wid = use_vec4.size();
	    എങ്കിൽ (bit.size() < use_wid) use_wid = bit.size();

	      /* Scan the vectors looking for delays. Select the maximum
	         delay encountered. */
	    use_delay = delay_.get_delay(use_vec4.value(0), bit.value(0));

	    വേണ്ടി (unsigned idx = 1 ;  idx < use_wid ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(use_vec4.value(idx), bit.value(idx));
		  എങ്കിൽ (tmp > use_delay) use_delay = tmp;
	    }
      }

      /* what *should* happen here is we check to see if there is a
         transaction in the queue. This would be a pulse that needs to be
         eliminated. */
      എങ്കിൽ (clean_pulse_events_(use_delay, bit)) തിരികെ;

      vvp_time64_t use_simtime = schedule_simtime() + use_delay;

	/* And propagate it. */
      എങ്കിൽ (use_delay == 0 && list_ == 0) {
	    cur_vec4_ = bit;
	    initial_ = അസത്യം;
	    net_->send_vec4(cur_vec4_, 0);
      } അല്ലെങ്കിൽ {
	    ഘടന event_*cur = പുതിയ ഘടന event_(use_simtime);
	    cur->run_run_ptr = &vvp_fun_delay::run_run_vec4_;
	    cur->ptr_vec4 = bit;
	    enqueue_(cur);
	    schedule_generic(ഇത്, use_delay, അസത്യം);
      }
}

ശൂന്യം vvp_fun_delay::recv_vec4_pv(vvp_net_ptr_t ptr, സ്ഥിരം vvp_vector4_t&bit,
			         unsigned base, unsigned vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

/* See the recv_vec4 comment above. */
ശൂന്യം vvp_fun_delay::recv_vec8(vvp_net_ptr_t port, സ്ഥിരം vvp_vector8_t&bit)
{
      പ്രതിജ്ഞാ(port.port() == 0);

      vvp_time64_t use_delay;
	/* This is an initial value so it needs to be compared to all the
	   bits (the order the bits are changed is not deterministic). */
      എങ്കിൽ (initial_) {
	    type_ = VEC8_DELAY;
            cur_vec4_ = vvp_vector4_t(0, BIT4_X);
	    vvp_bit4_t cur_val = cur_vec8_.value(0).value();
	    use_delay = delay_.get_delay(cur_val, bit.value(0).value());
	    വേണ്ടി (unsigned idx = 1 ;  idx < bit.size() ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(cur_val, bit.value(idx).value());
		  എങ്കിൽ (tmp > use_delay) use_delay = tmp;
	    }
      } അല്ലെങ്കിൽ {
	    പ്രതിജ്ഞാ(type_ == VEC8_DELAY);

	      // Use as a reference for calculating the delay the
	      // current value of the output. Detect and handle the
	      // special case that the event list contains the current
	      // value as a zero-delay-remaining event.
	    സ്ഥിരം vvp_vector8_t&use_vec8 = (list_ && list_->next->sim_time == schedule_simtime())? list_->next->ptr_vec8 : cur_vec8_;

	      /* How many bits to compare? */
	    unsigned use_wid = use_vec8.size();
	    എങ്കിൽ (bit.size() < use_wid) use_wid = bit.size();

	      /* Scan the vectors looking for delays. Select the maximum
	         delay encountered. */
	    use_delay = delay_.get_delay(use_vec8.value(0).value(),
	                                 bit.value(0).value());

	    വേണ്ടി (unsigned idx = 1 ;  idx < use_wid ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(use_vec8.value(idx).value(),
		                         bit.value(idx).value());
		  എങ്കിൽ (tmp > use_delay) use_delay = tmp;
	    }
      }

      /* what *should* happen here is we check to see if there is a
         transaction in the queue. This would be a pulse that needs to be
         eliminated. */
      എങ്കിൽ (clean_pulse_events_(use_delay, bit)) തിരികെ;

      vvp_time64_t use_simtime = schedule_simtime() + use_delay;

	/* And propagate it. */
      എങ്കിൽ (use_delay == 0 && list_ == 0) {
	    cur_vec8_ = bit;
	    initial_ = അസത്യം;
	    net_->send_vec8(cur_vec8_);
      } അല്ലെങ്കിൽ {
	    ഘടന event_*cur = പുതിയ ഘടന event_(use_simtime);
	    cur->ptr_vec8 = bit;
	    cur->run_run_ptr = &vvp_fun_delay::run_run_vec8_;
	    enqueue_(cur);
	    schedule_generic(ഇത്, use_delay, അസത്യം);
      }
}

ശൂന്യം vvp_fun_delay::recv_vec8_pv(vvp_net_ptr_t ptr, സ്ഥിരം vvp_vector8_t&bit,
			         unsigned base, unsigned vwid)
{
      recv_vec8_pv_(ptr, bit, base, vwid);
}

ശൂന്യം vvp_fun_delay::recv_real(vvp_net_ptr_t port, ഇരട്ട bit,
                              vvp_context_t)
{
      എങ്കിൽ (port.port() > 0) {
	    /* If the port is not 0, then this is a delay value that
	    should be rounded and converted to an integer delay. */
	    vvp_time64_t val = 0;
	    എങ്കിൽ (bit > -0.5) {
		  val = (vvp_time64_t) (bit * round_ + 0.5) * scale_;
	    } അല്ലെങ്കിൽ എങ്കിൽ (bit != bit) {
		    // For a NaN we use the default (0).
	    } അല്ലെങ്കിൽ {
		  vvp_vector4_t vec4(8*വലുപ്പം(vvp_time64_t),
		                     floor(-bit * round_ + 0.5) * -1 * scale_);
		  vector4_to_മൂല്യം(vec4, val);
	    }

	    തിരഞ്ഞെടുപ്പ് (port.port()) {
		അവസ്ഥ 1:
		  delay_.set_rise(val);
		  തിരികെ;
		അവസ്ഥ 2:
		  delay_.set_fall(val);
		  തിരികെ;
		അവസ്ഥ 3:
		  delay_.set_decay(val);
		  തിരികെ;
	    }
	    തിരികെ;
      }

      എങ്കിൽ (initial_) {
	    type_ = REAL_DELAY;
            cur_vec4_ = vvp_vector4_t(0, BIT4_X);
            cur_vec8_ = vvp_vector8_t(cur_vec4_, 6, 6);
      } അല്ലെങ്കിൽ പ്രതിജ്ഞാ(type_ == REAL_DELAY);

      vvp_time64_t use_delay;
      use_delay = delay_.get_min_delay();

      /* Eliminate glitches. */
      എങ്കിൽ (clean_pulse_events_(use_delay, bit)) തിരികെ;

      /* This must be done after cleaning pulses to avoid propagating
       * an incorrect value. */
      എങ്കിൽ (cur_real_ == bit) തിരികെ;

      vvp_time64_t use_simtime = schedule_simtime() + use_delay;

      എങ്കിൽ (use_delay == 0 && list_ == 0) {
	    cur_real_ = bit;
	    initial_ = അസത്യം;
	    net_->send_real(cur_real_, 0);
      } അല്ലെങ്കിൽ {
	    ഘടന event_*cur = പുതിയ ഘടന event_(use_simtime);
	    cur->run_run_ptr = &vvp_fun_delay::run_run_real_;
	    cur->ptr_real = bit;
	    enqueue_(cur);

	    schedule_generic(ഇത്, use_delay, അസത്യം);
      }
}

ശൂന്യം vvp_fun_delay::run_run()
{
      vvp_time64_t sim_time = schedule_simtime();
      എങ്കിൽ (list_ == 0 || list_->next->sim_time > sim_time)
	    തിരികെ;

      ഘടന event_*cur = dequeue_();
      എങ്കിൽ (cur == 0)
	    തിരികെ;

      (ഇത്->*(cur->run_run_ptr))(cur);
      initial_ = അസത്യം;
      delete cur;
}

ശൂന്യം vvp_fun_delay::run_run_vec4_(സ്ഥിരം ഘടന event_*cur)
{
      cur_vec4_ = cur->ptr_vec4;
      net_->send_vec4(cur_vec4_, 0);
}

ശൂന്യം vvp_fun_delay::run_run_vec8_(സ്ഥിരം ഘടന vvp_fun_delay::event_*cur)
{
      cur_vec8_ = cur->ptr_vec8;
      net_->send_vec8(cur_vec8_);
}

ശൂന്യം vvp_fun_delay::run_run_real_(സ്ഥിരം ഘടന vvp_fun_delay::event_*cur)
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
      ആകുമ്പോൾ (src_list_) {
	    vvp_fun_modpath_src*tmp = src_list_;
	    src_list_ = tmp->next_;
	    delete tmp;
      }
      ആകുമ്പോൾ (ifnone_list_) {
	    vvp_fun_modpath_src*tmp = ifnone_list_;
	    ifnone_list_ = tmp->next_;
	    delete tmp;
      }
}

ശൂന്യം vvp_fun_modpath::add_modpath_src(vvp_fun_modpath_src*that, തർക്കം ifnone)
{
      പ്രതിജ്ഞാ(that->next_ == 0);
      എങ്കിൽ (ifnone) {
	    that->next_ = ifnone_list_;
	    ifnone_list_ = that;
      } അല്ലെങ്കിൽ {
	    that->next_ = src_list_;
	    src_list_ = that;
      }
}

സ്ഥിതം vvp_time64_t delay_from_edge(vvp_bit4_t a, vvp_bit4_t b,
                                           vvp_time64_t സ്ഥിരം array[12])
{
      തരനിർവചനം delay_edge_t bit4_table4[4];
      സ്ഥിതം സ്ഥിരം bit4_table4 edge_table[4] = {
	    { DELAY_EDGE_01, DELAY_EDGE_01, DELAY_EDGE_0z, DELAY_EDGE_0x },
	    { DELAY_EDGE_10, DELAY_EDGE_10, DELAY_EDGE_1z, DELAY_EDGE_1x },
	    { DELAY_EDGE_z0, DELAY_EDGE_z1, DELAY_EDGE_z0, DELAY_EDGE_zx },
	    { DELAY_EDGE_x0, DELAY_EDGE_x1, DELAY_EDGE_xz, DELAY_EDGE_x0 }
      };

      തിരികെ array[ edge_table[a][b] ];
}

ശൂന്യം vvp_fun_modpath::recv_vec4(vvp_net_ptr_t port, സ്ഥിരം vvp_vector4_t&bit,
                                vvp_context_t)
{
	/* Only the first port is used. */
      എങ്കിൽ (port.port() > 0)
	    തിരികെ;

      എങ്കിൽ (cur_vec4_.eeq(bit))
	    തിരികെ;

	/* Select a time delay source that applies. Notice that there
	   may be multiple delay sources that apply, so collect all
	   the candidates into a list first. */
      list<vvp_fun_modpath_src*>candidate_list;
      vvp_time64_t candidate_wake_time = 0;
      വേണ്ടി (vvp_fun_modpath_src*cur = src_list_ ;  cur ;  cur=cur->next_) {
	      /* Skip paths that are disabled by conditions. */
	    എങ്കിൽ (cur->condition_flag_ == അസത്യം)
		  തുടരുക;

	    എങ്കിൽ (candidate_list.empty()) {
		  candidate_list.push_back(cur);
		  candidate_wake_time = cur->wake_time_;
	    } അല്ലെങ്കിൽ എങ്കിൽ (cur->wake_time_ == candidate_wake_time) {
		  candidate_list.push_back(cur);
	    } അല്ലെങ്കിൽ എങ്കിൽ (cur->wake_time_ > candidate_wake_time) {
		  candidate_list.assign(1, cur);
		  candidate_wake_time = cur->wake_time_;
	    } അല്ലെങ്കിൽ {
		  തുടരുക; /* Skip this entry. */
	    }
      }

	/* Only add the ifnone delay if it has a later wake_time_ or
	 * if there are no normal delays. */
      vvp_time64_t ifnone_wake_time = candidate_wake_time;
      വേണ്ടി (vvp_fun_modpath_src*cur = ifnone_list_ ;  cur ;  cur=cur->next_) {
	    എങ്കിൽ (candidate_list.empty()) {
		  candidate_list.push_back(cur);
		  ifnone_wake_time = cur->wake_time_;
	    } അല്ലെങ്കിൽ എങ്കിൽ (cur->wake_time_ == ifnone_wake_time &&
	               ifnone_wake_time > candidate_wake_time) {
		  candidate_list.push_back(cur);
	    } അല്ലെങ്കിൽ എങ്കിൽ (cur->wake_time_ > ifnone_wake_time) {
		  candidate_list.assign(1, cur);
		  ifnone_wake_time = cur->wake_time_;
	    } അല്ലെങ്കിൽ {
		  തുടരുക; /* Skip this entry. */
	    }
      }

	/* Handle the special case that there are no delays that
	   match. This may happen, for example, if the set of
	   conditional delays is incomplete, leaving some cases
	   uncovered. In that case, just pass the data without delay */
      എങ്കിൽ (candidate_list.empty()) {
	    cur_vec4_ = bit;
	    schedule_generic(ഇത്, 0, അസത്യം);
	    തിരികെ;
      }

	/* Now given that we have a list of candidate delays, find for
	   each if the 12 numbers the minimum from all the
	   candidates. This minimum set becomes the chosen delay to
	   use. */
      vvp_time64_t out_at[12];
      vvp_time64_t now = schedule_simtime();

      തരനിർവചനം list<vvp_fun_modpath_src*>::const_iterator iter_t;

      iter_t cur = candidate_list.begin();
      സ്ഥിരം vvp_fun_modpath_src*src = *cur;

      വേണ്ടി (unsigned idx = 0 ;  idx < 12 ;  idx += 1) {
	    out_at[idx] = src->wake_time_ + src->delay_[idx];
	    എങ്കിൽ (out_at[idx] <= now)
		  out_at[idx] = 0;
	    അല്ലെങ്കിൽ
		  out_at[idx] -= now;
      }

      വേണ്ടി (++ cur ; cur != candidate_list.end() ; ++ cur ) {
	    src = *cur;
	    വേണ്ടി (unsigned idx = 0 ;  idx < 12 ;  idx += 1) {
		  vvp_time64_t tmp = src->wake_time_ + src->delay_[idx];
		  എങ്കിൽ (tmp <= now)
			tmp = 0;
		  അല്ലെങ്കിൽ
			tmp -= now;
		  എങ്കിൽ (tmp < out_at[idx])
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
      വേണ്ടി (unsigned idx = 1 ;  idx < bit.size() ;  idx += 1) {
	    vvp_time64_t tmp = delay_from_edge(cur_vec4_.value(idx),
					       bit.value(idx),
					       out_at);
	      /* If the current and new bit values match then no delay
	       * is needed for this bit. */
	    എങ്കിൽ (cur_vec4_.value(idx) == bit.value(idx)) തുടരുക;
	    എങ്കിൽ (tmp != use_delay) {
		  fprintf(stderr, "sorry: multi-bit module path delays are "
		                  "currently not fully supported.\n");
		  fprintf(stderr, "     : using the LSB delay for all bits.\n");
		  തുടരുക;
            }
      }

      cur_vec4_ = bit;
      schedule_generic(ഇത്, use_delay, അസത്യം);
}

ശൂന്യം vvp_fun_modpath::run_run()
{
      net_->send_vec4(cur_vec4_, 0);
}

vvp_fun_modpath_src::vvp_fun_modpath_src(vvp_time64_t സ്ഥിരം del[12])
{
      വേണ്ടി (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = del[idx];

      next_ = 0;
      wake_time_ = 0;
      condition_flag_ = സത്യം;
}

vvp_fun_modpath_src::~vvp_fun_modpath_src()
{
}

ശൂന്യം vvp_fun_modpath_src::get_delay12(vvp_time64_t val[12]) സ്ഥിരം
{
      വേണ്ടി (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    val[idx] = delay_[idx];
}

ശൂന്യം vvp_fun_modpath_src::put_delay12(സ്ഥിരം vvp_time64_t val[12])
{
      വേണ്ടി (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = val[idx];
}

ശൂന്യം vvp_fun_modpath_src::recv_vec4(vvp_net_ptr_t port, സ്ഥിരം vvp_vector4_t&bit,
                                    vvp_context_t)
{
      എങ്കിൽ (port.port() == 0) {
	      // The modpath input...
	    എങ്കിൽ (test_vec4(bit))
		  wake_time_ = schedule_simtime();

      } അല്ലെങ്കിൽ എങ്കിൽ (port.port() == 1) {
	      // The modpath condition input...
	    എങ്കിൽ (bit.value(0) != BIT4_0)
		  condition_flag_ = സത്യം;
	    അല്ലെങ്കിൽ
		  condition_flag_ = അസത്യം;
      }
}

തർക്കം vvp_fun_modpath_src::test_vec4(സ്ഥിരം vvp_vector4_t&)
{
      തിരികെ സത്യം;
}

vvp_fun_modpath_edge::vvp_fun_modpath_edge(vvp_time64_t del[12],
					   തർക്കം pos, തർക്കം neg)
: vvp_fun_modpath_src(del)
{
      old_മൂല്യം = BIT4_X;
      posedge_ = pos;
      negedge_ = neg;
}

തർക്കം vvp_fun_modpath_edge::test_vec4(സ്ഥിരം vvp_vector4_t&bit)
{
      vvp_bit4_t tmp = old_മൂല്യം;
      old_മൂല്യം = bit.value(0);

      പൂർണ്ണസംഖ്യ edge_flag = edge(tmp, old_മൂല്യം);
      എങ്കിൽ (edge_flag > 0) തിരികെ posedge_;
      എങ്കിൽ (edge_flag < 0) തിരികെ negedge_;
      തിരികെ അസത്യം;
}


/*
 * All the below routines that begin with
 * modpath_src_* belong the internal function
 * of an vpiModPathIn object. This is used to
 * make some specific delays path operations
 *
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ modpath_src_get(പൂർണ്ണസംഖ്യ, vpiHandle ref)
{
      ഘടന __vpiModPathSrc*obj =dynamic_cast<__vpiModPathSrc*>(ref);
      പ്രതിജ്ഞാ(obj);
      തിരികെ 0;
}

സ്ഥിതം ശൂന്യം modpath_src_get_മൂല്യം(vpiHandle ref, p_vpi_മൂല്യം)
{
      ഘടന __vpiModPathSrc* modpathsrc = dynamic_cast<__vpiModPathSrc*>(ref);
      പ്രതിജ്ഞാ(modpathsrc);
      തിരികെ;
}

സ്ഥിതം vpiHandle modpath_src_put_മൂല്യം(vpiHandle ref, s_vpi_മൂല്യം *, പൂർണ്ണസംഖ്യ )
{
      ഘടന __vpiModPathSrc* modpathsrc = dynamic_cast<__vpiModPathSrc*>(ref);
      പ്രതിജ്ഞാ(modpathsrc);
      തിരികെ 0;
}

സ്ഥിതം vpiHandle modpath_src_get_handle(പൂർണ്ണസംഖ്യ code, vpiHandle ref)
{
      ഘടന __vpiModPathSrc*rfp = dynamic_cast<__vpiModPathSrc*>(ref);
      പ്രതിജ്ഞാ(rfp);

      തിരഞ്ഞെടുപ്പ് (code) {

	അവസ്ഥ vpiScope:
	  തിരികെ rfp->dest->scope;

	  അവസ്ഥ vpiModule:
	      { __vpiScope*scope = rfp->dest->scope;
		ആകുമ്പോൾ (scope && scope->get_type_code() != vpiModule)
		      scope = scope->scope;
		പ്രതിജ്ഞാ(scope);
		തിരികെ scope;
	      }

	    // Handles to path term objects should really be obtained via
	    // the vpi_iterate and vpi_scan functions. Continue to allow
	    // them to be obtained here for backwards compatibility with
	    // older versions of Icarus Verilog.

	  അവസ്ഥ vpiModPathIn:
	    തിരികെ &rfp->path_term_in;

	  അവസ്ഥ vpiModPathOut:
	    തിരികെ &rfp->dest->path_term_out;
      }
      തിരികെ 0;
}

സ്ഥിതം vpiHandle modpath_src_iterate(പൂർണ്ണസംഖ്യ code, vpiHandle ref)
{
      ഘടന __vpiModPathSrc*rfp = dynamic_cast<__vpiModPathSrc*>(ref);
      പ്രതിജ്ഞാ(rfp);

	// Module paths with multiple sources or destinations are
	// currently represented by a separate modpath object for
	// each source/destination combination, so there is only
	// ever one input path term and one output path term.
      തിരഞ്ഞെടുപ്പ് (code) {
	  അവസ്ഥ vpiModPathIn: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(1, വലുപ്പം(vpiHandle*)));
	    args[0] = &rfp->path_term_in;
	    തിരികെ vpip_make_iterator(1, args, സത്യം);
	  }
	  അവസ്ഥ vpiModPathOut: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(1, വലുപ്പം(vpiHandle*)));
	    args[0] = &rfp->dest->path_term_out;
	    തിരികെ vpip_make_iterator(1, args, സത്യം);
	  }
      }
      തിരികെ 0;
}

സ്ഥിതം vpiHandle modpath_src_index ( vpiHandle ref, പൂർണ്ണസംഖ്യ)
{
      പ്രതിജ്ഞാ(ref->get_type_code() == vpiModPathIn);
      തിരികെ 0;
}


/*
 * This routine will put specific dimension of delay[] values
 * into a vpiHandle. In this case, we will put
 * specific delays values in a vpiModPathIn object
 *
 */
സ്ഥിതം ശൂന്യം modpath_src_put_delays (vpiHandle ref, p_vpi_delay delays)
{
      vvp_time64_t tmp[12];
      പൂർണ്ണസംഖ്യ idx;
      ഘടന __vpiModPathSrc * src = dynamic_cast<__vpiModPathSrc*>(ref) ;
      പ്രതിജ്ഞാ(src) ;

      vvp_fun_modpath_src *fun = dynamic_cast<vvp_fun_modpath_src*>(src->net->fun);
      പ്രതിജ്ഞാ( fun );

      തരനിർവചനം unsigned അക്ഷരം map_array_t[12];
	// Only the first six entries are used for the less than twelve maps.
      സ്ഥിതം സ്ഥിരം map_array_t map_1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      സ്ഥിതം സ്ഥിരം map_array_t map_2 = {0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0};
      സ്ഥിതം സ്ഥിരം map_array_t map_3 = {0, 1, 2, 0, 2, 1, 0, 0, 0, 0, 0, 0};
      സ്ഥിതം സ്ഥിരം map_array_t map_6 = {0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0};
      സ്ഥിതം സ്ഥിരം map_array_t map12 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

      സ്ഥിരം map_array_t*use_map = 0;
      തിരഞ്ഞെടുപ്പ് (delays->no_of_delays) {
	  അവസ്ഥ 1:
	    use_map = &map_1;
	    നിർത്തുക;
	  അവസ്ഥ 2:
	    use_map = &map_2;
	    നിർത്തുക;
	  അവസ്ഥ 3:
	    use_map = &map_3;
	    നിർത്തുക;
	  അവസ്ഥ 6:
	    use_map = &map_6;
	    നിർത്തുക;
	  അവസ്ഥ 12:
	    use_map = &map12;
	    നിർത്തുക;
	  സ്ഥിരസ്ഥിതി:
	    പ്രതിജ്ഞാ(0);
	    നിർത്തുക;
      }

      എങ്കിൽ (delays->time_type == vpiSimTime) {
	    വേണ്ടി (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_timestruct_to_time(delays->da+use_map[0][idx]);
	    }
      } അല്ലെങ്കിൽ {
	      // You cannot create a modpath with a negative delay so set it
	      // to zero per 1364-2005 section 14.3.1.
	    വേണ്ടി (idx = 0 ; idx < delays->no_of_delays ; idx += 1) {
		  എങ്കിൽ (delays->da[idx].real < 0.0) delays->da[idx].real = 0.0;
	    }
	    വേണ്ടി (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_scaled_real_to_time64(delays->da[use_map[0][idx]].real,
							src->dest->scope);
	    }
      }

      /* Now define the to-from-x delays if needed. */
      എങ്കിൽ (delays->no_of_delays <= 6) {
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

സ്ഥിതം ശൂന്യം modpath_src_get_delays ( vpiHandle ref, p_vpi_delay delays )
{
      ഘടന __vpiModPathSrc*src = dynamic_cast<__vpiModPathSrc*>(ref) ;
      പ്രതിജ്ഞാ(src);

      vvp_fun_modpath_src *fun = dynamic_cast<vvp_fun_modpath_src*>(src->net->fun);
      പ്രതിജ്ഞാ(fun);

      പൂർണ്ണസംഖ്യ idx;
      vvp_time64_t tmp[12];
      fun->get_delay12(tmp);

      തിരഞ്ഞെടുപ്പ് (delays->no_of_delays) {
	  അവസ്ഥ 1:
	  അവസ്ഥ 2:
	  അവസ്ഥ 3:
	  അവസ്ഥ 6:
	  അവസ്ഥ 12:
	    നിർത്തുക;

	  സ്ഥിരസ്ഥിതി:
	    പ്രതിജ്ഞാ(0);
	    നിർത്തുക;
      }

      എങ്കിൽ (delays->time_type == vpiSimTime) {
	    വേണ്ടി (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  vpip_time_to_timestruct(delays->da+idx, tmp[idx]);
	    }
      } അല്ലെങ്കിൽ {
	    വേണ്ടി (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  delays->da[idx].real = vpip_time_to_scaled_real(tmp[idx], src->dest->scope);
	    }
      }
}

സ്ഥിതം പൂർണ്ണസംഖ്യ pathterm_get(പൂർണ്ണസംഖ്യ code, vpiHandle ref)
{
      ഘടന __vpiModPathTerm*obj = dynamic_cast<__vpiModPathTerm*>(ref);
      പ്രതിജ്ഞാ(obj);

      തിരഞ്ഞെടുപ്പ് (code) {
	  അവസ്ഥ vpiEdge:
	    തിരികെ obj->edge;
	  സ്ഥിരസ്ഥിതി:
	    തിരികെ 0;
      }
}

സ്ഥിതം vpiHandle pathterm_get_handle(പൂർണ്ണസംഖ്യ code, vpiHandle ref)
{
      ഘടന __vpiModPathTerm*obj = dynamic_cast<__vpiModPathTerm*>(ref);
      പ്രതിജ്ഞാ(obj);

      തിരഞ്ഞെടുപ്പ് (code) {
	  അവസ്ഥ vpi_വ്യഞ്ജകം:
	    തിരികെ obj->വ്യഞ്ജകം;
	  സ്ഥിരസ്ഥിതി:
	    തിരികെ 0;
      }
}

/*
* The __vpiModPathSrc class is what the VPI client sees as a
* vpiModPath object. The __vpiModPath structure contains items that
* are common to a bunch of modpaths, including the destination term.
*/
വരിയിൽ __vpiModPathSrc::__vpiModPathSrc()
{ }

പൂർണ്ണസംഖ്യ __vpiModPathSrc::get_type_code(ശൂന്യം) സ്ഥിരം
{ തിരികെ vpiModPath; }

പൂർണ്ണസംഖ്യ __vpiModPathSrc::vpi_get(പൂർണ്ണസംഖ്യ code)
{ തിരികെ modpath_src_get(code, ഇത്); }

ശൂന്യം __vpiModPathSrc::vpi_get_മൂല്യം(p_vpi_മൂല്യം val)
{ modpath_src_get_മൂല്യം(ഇത്, val); }

vpiHandle __vpiModPathSrc::vpi_put_മൂല്യം(p_vpi_മൂല്യം val, പൂർണ്ണസംഖ്യ flags)
{ തിരികെ modpath_src_put_മൂല്യം(ഇത്, val, flags); }

vpiHandle __vpiModPathSrc::vpi_handle(പൂർണ്ണസംഖ്യ code)
{ തിരികെ modpath_src_get_handle(code, ഇത്); }

vpiHandle __vpiModPathSrc::vpi_iterate(പൂർണ്ണസംഖ്യ code)
{ തിരികെ modpath_src_iterate(code, ഇത്); }

vpiHandle __vpiModPathSrc:: vpi_index(പൂർണ്ണസംഖ്യ idx)
{ തിരികെ modpath_src_index(ഇത്, idx); }

ശൂന്യം __vpiModPathSrc::vpi_get_delays(p_vpi_delay del)
{ modpath_src_get_delays(ഇത്, del); }

ശൂന്യം __vpiModPathSrc::vpi_put_delays(p_vpi_delay del)
{ modpath_src_put_delays(ഇത്, del); }

സ്ഥിതം പൂർണ്ണസംഖ്യ modpath_src_free_object( vpiHandle ref )
{
      delete ref;
      തിരികെ 1 ;
}

__vpiHandle::free_object_fun_t __vpiModPathSrc::free_object_fun(ശൂന്യം)
{ തിരികെ &modpath_src_free_object; }


വരിയിൽ __vpiModPathTerm::__vpiModPathTerm()
{ }

പൂർണ്ണസംഖ്യ __vpiModPathTerm::get_type_code(ശൂന്യം) സ്ഥിരം
{ തിരികെ vpiPathTerm; }

പൂർണ്ണസംഖ്യ __vpiModPathTerm::vpi_get(പൂർണ്ണസംഖ്യ code)
{ തിരികെ pathterm_get(code, ഇത്); }

vpiHandle __vpiModPathTerm::vpi_handle(പൂർണ്ണസംഖ്യ code)
{ തിരികെ pathterm_get_handle(code, ഇത്); }

സ്ഥിതം ശൂന്യം initialize_path_term(ഘടന __vpiModPathTerm&obj)
{
      obj.വ്യഞ്ജകം = 0;
      obj.edge = vpiNoEdge;
}

/*
 * This function will construct a vpiModPath Object.
 * give a respective "net", and will point to his
 * respective functor
 */

#ifdef CHECK_WITH_VALGRIND
സ്ഥിതം ഘടന __vpiModPath**mp_list = 0;
സ്ഥിതം unsigned mp_എണ്ണം = 0;
#endif

ഘടന __vpiModPath* vpip_make_modpath(vvp_net_t *net)
{
      ഘടന __vpiModPath*obj = പുതിയ __vpiModPath;
      obj->scope = vpip_peek_current_scope ( );

      initialize_path_term(obj->path_term_out);
      obj->നിവേശം_net = net ;

#ifdef CHECK_WITH_VALGRIND
      mp_എണ്ണം += 1;
      mp_list = static_cast<ഘടന __vpiModPath **>
                (realloc(mp_list,
                 mp_എണ്ണം*വലുപ്പം(ഘടന __vpiModPath **)));
      mp_list[mp_എണ്ണം-1] = obj;
#endif
      തിരികെ obj;
}

#ifdef CHECK_WITH_VALGRIND
ശൂന്യം modpath_delete()
{
      വേണ്ടി (unsigned idx = 0; idx < mp_എണ്ണം; idx += 1) {
	    delete mp_list[idx];
      }
      സ്വതന്ത്രം(mp_list);
      mp_list = 0;
      mp_എണ്ണം = 0;
}
#endif

/*
 * This function will construct a vpiModPathIn
 * ( struct __vpiModPathSrc ) Object. will give
 * a delays[12] values, and point to the specified functor
 *
 */

ഘടന __vpiModPathSrc* vpip_make_modpath_src(ഘടന __vpiModPath*path,
                                              vvp_net_t *net)
{
      ഘടന __vpiModPathSrc *obj = പുതിയ __vpiModPathSrc;

      obj->dest = path;
      obj->type = 0;
      obj->net = net;
      initialize_path_term(obj->path_term_in);

      തിരികെ obj;
}

vvp_fun_intermodpath::vvp_fun_intermodpath(vvp_net_t*net, unsigned width)
: net_(net)
{
      വേണ്ടി (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = 0;

      cur_vec4_ = vvp_vector4_t(width, BIT4_X);
      schedule_init_propagate(net_, cur_vec4_);
}

vvp_fun_intermodpath::~vvp_fun_intermodpath()
{
}

ശൂന്യം vvp_fun_intermodpath::get_delay12(vvp_time64_t val[12]) സ്ഥിരം
{
      വേണ്ടി (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    val[idx] = delay_[idx];
}

ശൂന്യം vvp_fun_intermodpath::put_delay12(സ്ഥിരം vvp_time64_t val[12])
{
      വേണ്ടി (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = val[idx];
}

/*
 * FIXME: this implementation currently only uses the maximum delay
 * from all the bit changes in the vectors. If there are multiple
 * changes with different delays, then the results would be
 * wrong. What should happen is that if there are multiple changes,
 * multiple vectors approaching the result should be scheduled.
 */
ശൂന്യം vvp_fun_intermodpath::recv_vec4(vvp_net_ptr_t port, സ്ഥിരം vvp_vector4_t&bit,
                              vvp_context_t)
{
	/* Only the first port is used. */
      എങ്കിൽ (port.port() > 0)
	    തിരികെ;

      എങ്കിൽ (cur_vec4_.eeq(bit))
	    തിരികെ;

	/* Given the scheduled output time, create an output event. */
      vvp_time64_t use_delay = delay_from_edge(cur_vec4_.value(0),
					       bit.value(0),
					       delay_);

	/* FIXME: This bases the edge delay on only the least
	   bit. This is WRONG! I need to find all the possible delays,
	   and schedule an event for each partial change. Hard! */
      വേണ്ടി (unsigned idx = 1 ;  idx < bit.size() ;  idx += 1) {
	    vvp_time64_t tmp = delay_from_edge(cur_vec4_.value(idx),
					       bit.value(idx),
					       delay_);
	      /* If the current and new bit values match then no delay
	       * is needed for this bit. */
	    എങ്കിൽ (cur_vec4_.value(idx) == bit.value(idx)) തുടരുക;
	    പ്രതിജ്ഞാ(tmp == use_delay);
      }

      cur_vec4_ = bit;
      schedule_generic(ഇത്, use_delay, അസത്യം);
}

ശൂന്യം vvp_fun_intermodpath::run_run()
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
സ്ഥിതം പൂർണ്ണസംഖ്യ intermodpath_get(പൂർണ്ണസംഖ്യ, vpiHandle ref)
{
      ഘടന __vpiInterModPath*obj =dynamic_cast<__vpiInterModPath*>(ref);
      പ്രതിജ്ഞാ(obj);
      തിരികെ 0;
}

സ്ഥിതം ശൂന്യം intermodpath_get_മൂല്യം(vpiHandle ref, p_vpi_മൂല്യം)
{
      ഘടന __vpiInterModPath* intermodpath = dynamic_cast<__vpiInterModPath*>(ref);
      പ്രതിജ്ഞാ(intermodpath);
      തിരികെ;
}

സ്ഥിതം vpiHandle intermodpath_put_മൂല്യം(vpiHandle ref, s_vpi_മൂല്യം *, പൂർണ്ണസംഖ്യ )
{
      ഘടന __vpiInterModPath* intermodpath = dynamic_cast<__vpiInterModPath*>(ref);
      പ്രതിജ്ഞാ(intermodpath);
      തിരികെ 0;
}

സ്ഥിതം vpiHandle intermodpath_get_handle(പൂർണ്ണസംഖ്യ code, vpiHandle ref)
{
      ഘടന __vpiInterModPath*rfp = dynamic_cast<__vpiInterModPath*>(ref);
      പ്രതിജ്ഞാ(rfp);

      തിരഞ്ഞെടുപ്പ് (code) {

	അവസ്ഥ vpiScope:
	  തിരികെ rfp->scope;

	  അവസ്ഥ vpiModule:
	      { __vpiScope*scope = rfp->scope;
		ആകുമ്പോൾ (scope && scope->get_type_code() != vpiModule)
		      scope = scope->scope;
		പ്രതിജ്ഞാ(scope);
		തിരികെ scope;
	      }
      }
      തിരികെ 0;
}

സ്ഥിതം vpiHandle intermodpath_iterate(പൂർണ്ണസംഖ്യ code, vpiHandle ref)
{
      ഘടന __vpiInterModPath*rfp = dynamic_cast<__vpiInterModPath*>(ref);
      പ്രതിജ്ഞാ(rfp);

	// For now intermodpaths only support exactly two ports
      തിരഞ്ഞെടുപ്പ് (code) {
	  അവസ്ഥ vpiPort: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(2, വലുപ്പം(vpiHandle*)));
	    args[0] = rfp->port1;
	    args[1] = rfp->port2;
	    തിരികെ vpip_make_iterator(2, args, സത്യം);
	  }
      }
      തിരികെ 0;
}

/*
 * This routine will put specific dimension of delay[] values
 * into a vpiHandle. In this case, we will put
 * specific delays values in a vpiInterModPath object
 *
 */
സ്ഥിതം ശൂന്യം intermodpath_put_delays (vpiHandle ref, p_vpi_delay delays)
{
      vvp_time64_t tmp[12];
      പൂർണ്ണസംഖ്യ idx;
      ഘടന __vpiInterModPath * src = dynamic_cast<__vpiInterModPath*>(ref) ;
      പ്രതിജ്ഞാ(src) ;

      vvp_fun_intermodpath *fun = dynamic_cast<vvp_fun_intermodpath*>(src->net->fun);
      പ്രതിജ്ഞാ( fun );

      തരനിർവചനം unsigned അക്ഷരം map_array_t[12];
	// Only the first six entries are used for the less than twelve maps.
      സ്ഥിതം സ്ഥിരം map_array_t map_1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      സ്ഥിതം സ്ഥിരം map_array_t map_2 = {0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0};
      സ്ഥിതം സ്ഥിരം map_array_t map_3 = {0, 1, 2, 0, 2, 1, 0, 0, 0, 0, 0, 0};
      സ്ഥിതം സ്ഥിരം map_array_t map_6 = {0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0};
      സ്ഥിതം സ്ഥിരം map_array_t map12 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

      സ്ഥിരം map_array_t*use_map = 0;
      തിരഞ്ഞെടുപ്പ് (delays->no_of_delays) {
	  അവസ്ഥ 1:
	    use_map = &map_1;
	    നിർത്തുക;
	  അവസ്ഥ 2:
	    use_map = &map_2;
	    നിർത്തുക;
	  അവസ്ഥ 3:
	    use_map = &map_3;
	    നിർത്തുക;
	  അവസ്ഥ 6:
	    use_map = &map_6;
	    നിർത്തുക;
	  അവസ്ഥ 12:
	    use_map = &map12;
	    നിർത്തുക;
	  സ്ഥിരസ്ഥിതി:
	    പ്രതിജ്ഞാ(0);
	    നിർത്തുക;
      }

      എങ്കിൽ (delays->time_type == vpiSimTime) {
	    വേണ്ടി (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_timestruct_to_time(delays->da+use_map[0][idx]);
	    }
      } അല്ലെങ്കിൽ {
	      // You cannot create a modpath with a negative delay so set it
	      // to zero per 1364-2005 section 14.3.1.
	    വേണ്ടി (idx = 0 ; idx < delays->no_of_delays ; idx += 1) {
		  എങ്കിൽ (delays->da[idx].real < 0.0) delays->da[idx].real = 0.0;
	    }
	    വേണ്ടി (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_scaled_real_to_time64(delays->da[use_map[0][idx]].real,
							src->scope);
	    }
      }

      /* Now define the to-from-x delays if needed. */
      എങ്കിൽ (delays->no_of_delays <= 6) {
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

സ്ഥിതം ശൂന്യം intermodpath_get_delays ( vpiHandle ref, p_vpi_delay delays )
{
      ഘടന __vpiInterModPath*src = dynamic_cast<__vpiInterModPath*>(ref) ;
      പ്രതിജ്ഞാ(src);

      vvp_fun_intermodpath *fun = dynamic_cast<vvp_fun_intermodpath*>(src->net->fun);
      പ്രതിജ്ഞാ(fun);

      പൂർണ്ണസംഖ്യ idx;
      vvp_time64_t tmp[12];
      fun->get_delay12(tmp);

      തിരഞ്ഞെടുപ്പ് (delays->no_of_delays) {
	  അവസ്ഥ 1:
	  അവസ്ഥ 2:
	  അവസ്ഥ 3:
	  അവസ്ഥ 6:
	  അവസ്ഥ 12:
	    നിർത്തുക;

	  സ്ഥിരസ്ഥിതി:
	    പ്രതിജ്ഞാ(0);
	    നിർത്തുക;
      }

      എങ്കിൽ (delays->time_type == vpiSimTime) {
	    വേണ്ടി (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  vpip_time_to_timestruct(delays->da+idx, tmp[idx]);
	    }
      } അല്ലെങ്കിൽ {
	    വേണ്ടി (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  delays->da[idx].real = vpip_time_to_scaled_real(tmp[idx], src->scope);
	    }
      }
}

/*
* The __vpiInterModPath class is what the VPI client sees as a
* vpiInterModPath object.
*/
വരിയിൽ __vpiInterModPath::__vpiInterModPath()
{ }

പൂർണ്ണസംഖ്യ __vpiInterModPath::get_type_code(ശൂന്യം) സ്ഥിരം
{ തിരികെ vpiInterModPath; }

പൂർണ്ണസംഖ്യ __vpiInterModPath::vpi_get(പൂർണ്ണസംഖ്യ code)
{ തിരികെ intermodpath_get(code, ഇത്); }

ശൂന്യം __vpiInterModPath::vpi_get_മൂല്യം(p_vpi_മൂല്യം val)
{ intermodpath_get_മൂല്യം(ഇത്, val); }

vpiHandle __vpiInterModPath::vpi_put_മൂല്യം(p_vpi_മൂല്യം val, പൂർണ്ണസംഖ്യ flags)
{ തിരികെ intermodpath_put_മൂല്യം(ഇത്, val, flags); }

vpiHandle __vpiInterModPath::vpi_handle(പൂർണ്ണസംഖ്യ code)
{ തിരികെ intermodpath_get_handle(code, ഇത്); }

vpiHandle __vpiInterModPath::vpi_iterate(പൂർണ്ണസംഖ്യ code)
{ തിരികെ intermodpath_iterate(code, ഇത്); }

ശൂന്യം __vpiInterModPath::vpi_get_delays(p_vpi_delay del)
{ intermodpath_get_delays(ഇത്, del); }

ശൂന്യം __vpiInterModPath::vpi_put_delays(p_vpi_delay del)
{ intermodpath_put_delays(ഇത്, del); }

സ്ഥിതം പൂർണ്ണസംഖ്യ intermodpath_free_object( vpiHandle ref )
{
      delete ref;
      തിരികെ 1 ;
}

__vpiHandle::free_object_fun_t __vpiInterModPath::free_object_fun(ശൂന്യം)
{ തിരികെ &intermodpath_free_object; }

/*
 * This function will construct a vpiInterModPath Object.
 * give a respective "net", and will point to his
 * respective functor
 */

#ifdef CHECK_WITH_VALGRIND
സ്ഥിതം ഘടന __vpiInterModPath**imp_list = 0;
സ്ഥിതം unsigned imp_എണ്ണം = 0;
#endif

ഘടന __vpiInterModPath* vpip_make_intermodpath(vvp_net_t *net, vpiPortInfo* port1, vpiPortInfo* port2)
{
      ഘടന __vpiInterModPath*obj = പുതിയ __vpiInterModPath;
      obj->scope = vpip_peek_current_scope ( );

      obj->net = net;
      obj->port1 = port1;
      obj->port2 = port2;

#ifdef CHECK_WITH_VALGRIND
      imp_എണ്ണം += 1;
      imp_list = static_cast<ഘടന __vpiInterModPath **>
                 (realloc(imp_list,
                  imp_എണ്ണം*വലുപ്പം(ഘടന __vpiInterModPath **)));
      imp_list[imp_എണ്ണം-1] = obj;
#endif
      തിരികെ obj;
}

#ifdef CHECK_WITH_VALGRIND
ശൂന്യം intermodpath_delete()
{
      വേണ്ടി (unsigned idx = 0; idx < mp_എണ്ണം; idx += 1) {
	    delete imp_list[idx];
      }
      സ്വതന്ത്രം(imp_list);
      imp_list = 0;
      imp_എണ്ണം = 0;
}
#endif
