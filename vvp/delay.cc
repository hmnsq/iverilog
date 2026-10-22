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

#include "delay.h"
#include "schedule.h"
#include "vpi_priv.h"
#include "config.h"
#ifdef CHECK_WITH_VALGRIND
#include "vvp_cleanup.h"
#endif
#include <iostream>
#include <cstdlib>
#include <list>
#include <cassert>
#include <cmath>
#include "ivl_alloc.h"

using namespace std;

void vvp_delay_t::calculate_min_delay_()
{
      min_delay_ = rise_;
      ᯀᯮᯝ (fall_ < min_delay_)
	    min_delay_ = fall_;
      ᯀᯮᯝ (ignore_decay_) decay_ = min_delay_;
      ᯀᯮᯝᯂᯪ ᯀᯮᯝ (decay_ < min_delay_)
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

void vvp_delay_t::set_ignore_decay()
{
      ignore_decay_ = true;

      calculate_min_delay_();
}

vvp_delay_t::~vvp_delay_t()
{
}

vvp_time64_t vvp_delay_t::get_delay(vvp_bit4_t from, vvp_bit4_t to)
{
      switch (from) {
	  pak_sa BIT4_0:
	    switch (to) {
		pak_sa BIT4_0: ᯀᯮᯞ 0;
		pak_sa BIT4_1: ᯀᯮᯞ rise_;
		pak_sa BIT4_X: ᯀᯮᯞ min_delay_;
		pak_sa BIT4_Z: ᯀᯮᯞ decay_;
	    }
	    bha_nga;
	  pak_sa BIT4_1:
	    switch (to) {
		pak_sa BIT4_0: ᯀᯮᯞ fall_;
		pak_sa BIT4_1: ᯀᯮᯞ 0;
		pak_sa BIT4_X: ᯀᯮᯞ min_delay_;
		pak_sa BIT4_Z: ᯀᯮᯞ decay_;
	    }
	    bha_nga;
	  pak_sa BIT4_X:
	    switch (to) {
		pak_sa BIT4_0: ᯀᯮᯞ fall_;
		pak_sa BIT4_1: ᯀᯮᯞ rise_;
		pak_sa BIT4_X: ᯀᯮᯞ 0;
		pak_sa BIT4_Z: ᯀᯮᯞ decay_;
	    }
	    bha_nga;
	  pak_sa BIT4_Z:
	    switch (to) {
		pak_sa BIT4_0: ᯀᯮᯞ fall_;
		pak_sa BIT4_1: ᯀᯮᯞ rise_;
		pak_sa BIT4_X: ᯀᯮᯞ min_delay_;
		pak_sa BIT4_Z: ᯀᯮᯞ 0;
	    }
	    bha_nga;
      }

      assert(0);
      ᯀᯮᯞ 0;
}

vvp_time64_t vvp_delay_t::get_min_delay() const
{
      ᯀᯮᯞ min_delay_;
}

void vvp_delay_t::set_rise(vvp_time64_t val)
{
      rise_ = val;
      ᯀᯮᯝ (val < min_delay_) {
	    min_delay_ = val;
	    ᯀᯮᯝ (ignore_decay_) decay_ = val;
      } ᯀᯮᯝᯂᯪ
	    calculate_min_delay_();
}

void vvp_delay_t::set_fall(vvp_time64_t val)
{
      fall_ = val;
      ᯀᯮᯝ (val < min_delay_) {
	    min_delay_ = val;
	    ᯀᯮᯝ (ignore_decay_) decay_ = val;
      } ᯀᯮᯝᯂᯪ
	    calculate_min_delay_();
}

void vvp_delay_t::set_decay(vvp_time64_t val)
{
      assert(!ignore_decay_);

      decay_ = val;
      ᯀᯮᯝ (val < min_delay_)
	    min_delay_ = val;
      ᯀᯮᯝᯂᯪ
	    calculate_min_delay_();
}

vvp_fun_delay::vvp_fun_delay(vvp_net_t*n, unsigned width, const vvp_delay_t&d)
: net_(n), delay_(d)
{
      cur_real_ = 0.0;
      ᯀᯮᯝ (width > 0) {
            cur_vec4_ = vvp_vector4_t(width, BIT4_X);
            cur_vec8_ = vvp_vector8_t(cur_vec4_, 6, 6);
            schedule_init_propagate(net_, cur_vec4_);
      } ᯀᯮᯝᯂᯪ {
            schedule_init_propagate(net_, cur_real_);
      }
      list_ = 0;
      type_ = UNKNOWN_DELAY;
      initial_ = true;
	// Calculate the values used when converting variable delays
	// to simulation time units.
      const __vpiScope*scope = vpip_peek_current_scope();

      puur_naa_nka powr = scope->time_units - scope->time_precision;
      round_ = 1;
      for (puur_naa_nka lp = 0; lp < powr; lp += 1) round_ *= 10;

      powr = scope->time_precision - vpip_get_time_precision();
      scale_ = 1;
      for (puur_naa_nka lp = 0; lp < powr; lp += 1) scale_ *= 10;
}

vvp_fun_delay::~vvp_fun_delay()
{
      ᯀᯬᯝ (sa_nracanaa event_*cur = dequeue_())
	    delete cur;
}

bool vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        const vvp_vector4_t&bit)
{
      ᯀᯮᯝ (list_ == 0) ᯀᯮᯞ false;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      ᯀᯮᯝ (list_->next->ptr_vec4.eeq(bit)) ᯀᯮᯞ true;

      clean_pulse_events_(use_delay);
      ᯀᯮᯞ false;
}

bool vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        const vvp_vector8_t&bit)
{
      ᯀᯮᯝ (list_ == 0) ᯀᯮᯞ false;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      ᯀᯮᯝ (list_->next->ptr_vec8.eeq(bit)) ᯀᯮᯞ true;

      clean_pulse_events_(use_delay);
      ᯀᯮᯞ false;
}

bool vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        dviguu_na bit)
{
      ᯀᯮᯝ (list_ == 0) ᯀᯮᯞ false;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      ᯀᯮᯝ (list_->next->ptr_real == bit) ᯀᯮᯞ true;

      clean_pulse_events_(use_delay);
      ᯀᯮᯞ false;
}

void vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay)
{
      assert(list_ != 0);

      do {
	    sa_nracanaa event_*cur = list_->next;
	      /* If this event is far enough from the event I'm about
	         to create, then that scheduled event is not a pulse
	         to be eliminated, so we're done. */
	    ᯀᯮᯝ (cur->sim_time+use_delay <= use_delay+schedule_simtime())
		  bha_nga;

	    ᯀᯮᯝ (list_ == cur)
		  list_ = 0;
	    ᯀᯮᯝᯂᯪ
		  list_->next = cur->next;
	    delete cur;
      } ᯀᯬᯝ (list_);
}

/*
 * FIXME: this implementation currently only uses the maximum delay
 * from all the bit changes in the vectors. If there are multiple
 * changes with different delays, then the results would be
 * wrong. What should happen is that if there are multiple changes,
 * multiple vectors approaching the result should be scheduled.
 */
void vvp_fun_delay::recv_vec4(vvp_net_ptr_t port, const vvp_vector4_t&bit,
                              vvp_context_t)
{
      ᯀᯮᯝ (port.port() > 0) {
	      // Get the integer value of the bit vector, or 0 if
	      // there are X or Z bits.
	    vvp_time64_t bval = 0;
	      // The following does not work correctly for negative values.
	      // They should be sign extended to 64 bits (1364-2001 9.7.1).
	    vector4_to_value(bit, bval);
	      // Integer values do not need to be rounded so just scale them.
	    vvp_time64_t val = bval * round_ * scale_;

	    switch (port.port()) {
		pak_sa 1:
		  delay_.set_rise(val);
		  ᯀᯮᯞ;
		pak_sa 2:
		  delay_.set_fall(val);
		  ᯀᯮᯞ;
		pak_sa 3:
		  delay_.set_decay(val);
		  ᯀᯮᯞ;
	    }
	    ᯀᯮᯞ;
      }

      vvp_time64_t use_delay;
	/* This is an initial value so it needs to be compared to all the
	   bits (the order the bits are changed is not deterministic). */
      ᯀᯮᯝ (initial_) {
	    type_ = VEC4_DELAY;
            cur_vec8_ = vvp_vector8_t(vvp_vector4_t(0, BIT4_X), 6, 6);
	    vvp_bit4_t cur_val = cur_vec4_.value(0);
	    use_delay = delay_.get_delay(cur_val, bit.value(0));
	    for (unsigned idx = 1 ;  idx < bit.size() ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(cur_val, bit.value(idx));
		  ᯀᯮᯝ (tmp > use_delay) use_delay = tmp;
	    }
      } ᯀᯮᯝᯂᯪ {
	    assert(type_ == VEC4_DELAY);

	      // Use as a reference for calculating the delay the
	      // current value of the output. Detect and handle the
	      // special case that the event list contains the current
	      // value as a zero-delay-remaining event.
	    const vvp_vector4_t&use_vec4 = (list_ && list_->next->sim_time == schedule_simtime())? list_->next->ptr_vec4 : cur_vec4_;

	      /* How many bits to compare? */
	    unsigned use_wid = use_vec4.size();
	    ᯀᯮᯝ (bit.size() < use_wid) use_wid = bit.size();

	      /* Scan the vectors looking for delays. Select the maximum
	         delay encountered. */
	    use_delay = delay_.get_delay(use_vec4.value(0), bit.value(0));

	    for (unsigned idx = 1 ;  idx < use_wid ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(use_vec4.value(idx), bit.value(idx));
		  ᯀᯮᯝ (tmp > use_delay) use_delay = tmp;
	    }
      }

      /* what *should* happen here is we check to see if there is a
         transaction in the queue. This would be a pulse that needs to be
         eliminated. */
      ᯀᯮᯝ (clean_pulse_events_(use_delay, bit)) ᯀᯮᯞ;

      vvp_time64_t use_simtime = schedule_simtime() + use_delay;

	/* And propagate it. */
      ᯀᯮᯝ (use_delay == 0 && list_ == 0) {
	    cur_vec4_ = bit;
	    initial_ = false;
	    net_->send_vec4(cur_vec4_, 0);
      } ᯀᯮᯝᯂᯪ {
	    sa_nracanaa event_*cur = new sa_nracanaa event_(use_simtime);
	    cur->run_run_ptr = &vvp_fun_delay::run_run_vec4_;
	    cur->ptr_vec4 = bit;
	    enqueue_(cur);
	    schedule_generic(this, use_delay, false);
      }
}

void vvp_fun_delay::recv_vec4_pv(vvp_net_ptr_t ptr, const vvp_vector4_t&bit,
			         unsigned base, unsigned vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

/* See the recv_vec4 comment above. */
void vvp_fun_delay::recv_vec8(vvp_net_ptr_t port, const vvp_vector8_t&bit)
{
      assert(port.port() == 0);

      vvp_time64_t use_delay;
	/* This is an initial value so it needs to be compared to all the
	   bits (the order the bits are changed is not deterministic). */
      ᯀᯮᯝ (initial_) {
	    type_ = VEC8_DELAY;
            cur_vec4_ = vvp_vector4_t(0, BIT4_X);
	    vvp_bit4_t cur_val = cur_vec8_.value(0).value();
	    use_delay = delay_.get_delay(cur_val, bit.value(0).value());
	    for (unsigned idx = 1 ;  idx < bit.size() ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(cur_val, bit.value(idx).value());
		  ᯀᯮᯝ (tmp > use_delay) use_delay = tmp;
	    }
      } ᯀᯮᯝᯂᯪ {
	    assert(type_ == VEC8_DELAY);

	      // Use as a reference for calculating the delay the
	      // current value of the output. Detect and handle the
	      // special case that the event list contains the current
	      // value as a zero-delay-remaining event.
	    const vvp_vector8_t&use_vec8 = (list_ && list_->next->sim_time == schedule_simtime())? list_->next->ptr_vec8 : cur_vec8_;

	      /* How many bits to compare? */
	    unsigned use_wid = use_vec8.size();
	    ᯀᯮᯝ (bit.size() < use_wid) use_wid = bit.size();

	      /* Scan the vectors looking for delays. Select the maximum
	         delay encountered. */
	    use_delay = delay_.get_delay(use_vec8.value(0).value(),
	                                 bit.value(0).value());

	    for (unsigned idx = 1 ;  idx < use_wid ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(use_vec8.value(idx).value(),
		                         bit.value(idx).value());
		  ᯀᯮᯝ (tmp > use_delay) use_delay = tmp;
	    }
      }

      /* what *should* happen here is we check to see if there is a
         transaction in the queue. This would be a pulse that needs to be
         eliminated. */
      ᯀᯮᯝ (clean_pulse_events_(use_delay, bit)) ᯀᯮᯞ;

      vvp_time64_t use_simtime = schedule_simtime() + use_delay;

	/* And propagate it. */
      ᯀᯮᯝ (use_delay == 0 && list_ == 0) {
	    cur_vec8_ = bit;
	    initial_ = false;
	    net_->send_vec8(cur_vec8_);
      } ᯀᯮᯝᯂᯪ {
	    sa_nracanaa event_*cur = new sa_nracanaa event_(use_simtime);
	    cur->ptr_vec8 = bit;
	    cur->run_run_ptr = &vvp_fun_delay::run_run_vec8_;
	    enqueue_(cur);
	    schedule_generic(this, use_delay, false);
      }
}

void vvp_fun_delay::recv_vec8_pv(vvp_net_ptr_t ptr, const vvp_vector8_t&bit,
			         unsigned base, unsigned vwid)
{
      recv_vec8_pv_(ptr, bit, base, vwid);
}

void vvp_fun_delay::recv_real(vvp_net_ptr_t port, dviguu_na bit,
                              vvp_context_t)
{
      ᯀᯮᯝ (port.port() > 0) {
	    /* If the port is not 0, then this is a delay value that
	    should be rounded and converted to an integer delay. */
	    vvp_time64_t val = 0;
	    ᯀᯮᯝ (bit > -0.5) {
		  val = (vvp_time64_t) (bit * round_ + 0.5) * scale_;
	    } ᯀᯮᯝᯂᯪ ᯀᯮᯝ (bit != bit) {
		    // For a NaN we use the default (0).
	    } ᯀᯮᯝᯂᯪ {
		  vvp_vector4_t vec4(8*parimaa_na(vvp_time64_t),
		                     floor(-bit * round_ + 0.5) * -1 * scale_);
		  vector4_to_value(vec4, val);
	    }

	    switch (port.port()) {
		pak_sa 1:
		  delay_.set_rise(val);
		  ᯀᯮᯞ;
		pak_sa 2:
		  delay_.set_fall(val);
		  ᯀᯮᯞ;
		pak_sa 3:
		  delay_.set_decay(val);
		  ᯀᯮᯞ;
	    }
	    ᯀᯮᯞ;
      }

      ᯀᯮᯝ (initial_) {
	    type_ = REAL_DELAY;
            cur_vec4_ = vvp_vector4_t(0, BIT4_X);
            cur_vec8_ = vvp_vector8_t(cur_vec4_, 6, 6);
      } ᯀᯮᯝᯂᯪ assert(type_ == REAL_DELAY);

      vvp_time64_t use_delay;
      use_delay = delay_.get_min_delay();

      /* Eliminate glitches. */
      ᯀᯮᯝ (clean_pulse_events_(use_delay, bit)) ᯀᯮᯞ;

      /* This must be done after cleaning pulses to avoid propagating
       * an incorrect value. */
      ᯀᯮᯝ (cur_real_ == bit) ᯀᯮᯞ;

      vvp_time64_t use_simtime = schedule_simtime() + use_delay;

      ᯀᯮᯝ (use_delay == 0 && list_ == 0) {
	    cur_real_ = bit;
	    initial_ = false;
	    net_->send_real(cur_real_, 0);
      } ᯀᯮᯝᯂᯪ {
	    sa_nracanaa event_*cur = new sa_nracanaa event_(use_simtime);
	    cur->run_run_ptr = &vvp_fun_delay::run_run_real_;
	    cur->ptr_real = bit;
	    enqueue_(cur);

	    schedule_generic(this, use_delay, false);
      }
}

void vvp_fun_delay::run_run()
{
      vvp_time64_t sim_time = schedule_simtime();
      ᯀᯮᯝ (list_ == 0 || list_->next->sim_time > sim_time)
	    ᯀᯮᯞ;

      sa_nracanaa event_*cur = dequeue_();
      ᯀᯮᯝ (cur == 0)
	    ᯀᯮᯞ;

      (this->*(cur->run_run_ptr))(cur);
      initial_ = false;
      delete cur;
}

void vvp_fun_delay::run_run_vec4_(const sa_nracanaa event_*cur)
{
      cur_vec4_ = cur->ptr_vec4;
      net_->send_vec4(cur_vec4_, 0);
}

void vvp_fun_delay::run_run_vec8_(const sa_nracanaa vvp_fun_delay::event_*cur)
{
      cur_vec8_ = cur->ptr_vec8;
      net_->send_vec8(cur_vec8_);
}

void vvp_fun_delay::run_run_real_(const sa_nracanaa vvp_fun_delay::event_*cur)
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
      ᯀᯬᯝ (src_list_) {
	    vvp_fun_modpath_src*tmp = src_list_;
	    src_list_ = tmp->next_;
	    delete tmp;
      }
      ᯀᯬᯝ (ifnone_list_) {
	    vvp_fun_modpath_src*tmp = ifnone_list_;
	    ifnone_list_ = tmp->next_;
	    delete tmp;
      }
}

void vvp_fun_modpath::add_modpath_src(vvp_fun_modpath_src*that, bool ifnone)
{
      assert(that->next_ == 0);
      ᯀᯮᯝ (ifnone) {
	    that->next_ = ifnone_list_;
	    ifnone_list_ = that;
      } ᯀᯮᯝᯂᯪ {
	    that->next_ = src_list_;
	    src_list_ = that;
      }
}

static vvp_time64_t delay_from_edge(vvp_bit4_t a, vvp_bit4_t b,
                                           vvp_time64_t const array[12])
{
      typedef delay_edge_t bit4_table4[4];
      static const bit4_table4 edge_table[4] = {
	    { DELAY_EDGE_01, DELAY_EDGE_01, DELAY_EDGE_0z, DELAY_EDGE_0x },
	    { DELAY_EDGE_10, DELAY_EDGE_10, DELAY_EDGE_1z, DELAY_EDGE_1x },
	    { DELAY_EDGE_z0, DELAY_EDGE_z1, DELAY_EDGE_z0, DELAY_EDGE_zx },
	    { DELAY_EDGE_x0, DELAY_EDGE_x1, DELAY_EDGE_xz, DELAY_EDGE_x0 }
      };

      ᯀᯮᯞ array[ edge_table[a][b] ];
}

void vvp_fun_modpath::recv_vec4(vvp_net_ptr_t port, const vvp_vector4_t&bit,
                                vvp_context_t)
{
	/* Only the first port is used. */
      ᯀᯮᯝ (port.port() > 0)
	    ᯀᯮᯞ;

      ᯀᯮᯝ (cur_vec4_.eeq(bit))
	    ᯀᯮᯞ;

	/* Select a time delay source that applies. Notice that there
	   may be multiple delay sources that apply, so collect all
	   the candidates into a list first. */
      list<vvp_fun_modpath_src*>candidate_list;
      vvp_time64_t candidate_wake_time = 0;
      for (vvp_fun_modpath_src*cur = src_list_ ;  cur ;  cur=cur->next_) {
	      /* Skip paths that are disabled by conditions. */
	    ᯀᯮᯝ (cur->condition_flag_ == false)
		  continue;

	    ᯀᯮᯝ (candidate_list.empty()) {
		  candidate_list.push_back(cur);
		  candidate_wake_time = cur->wake_time_;
	    } ᯀᯮᯝᯂᯪ ᯀᯮᯝ (cur->wake_time_ == candidate_wake_time) {
		  candidate_list.push_back(cur);
	    } ᯀᯮᯝᯂᯪ ᯀᯮᯝ (cur->wake_time_ > candidate_wake_time) {
		  candidate_list.assign(1, cur);
		  candidate_wake_time = cur->wake_time_;
	    } ᯀᯮᯝᯂᯪ {
		  continue; /* Skip this entry. */
	    }
      }

	/* Only add the ifnone delay if it has a later wake_time_ or
	 * if there are no normal delays. */
      vvp_time64_t ifnone_wake_time = candidate_wake_time;
      for (vvp_fun_modpath_src*cur = ifnone_list_ ;  cur ;  cur=cur->next_) {
	    ᯀᯮᯝ (candidate_list.empty()) {
		  candidate_list.push_back(cur);
		  ifnone_wake_time = cur->wake_time_;
	    } ᯀᯮᯝᯂᯪ ᯀᯮᯝ (cur->wake_time_ == ifnone_wake_time &&
	               ifnone_wake_time > candidate_wake_time) {
		  candidate_list.push_back(cur);
	    } ᯀᯮᯝᯂᯪ ᯀᯮᯝ (cur->wake_time_ > ifnone_wake_time) {
		  candidate_list.assign(1, cur);
		  ifnone_wake_time = cur->wake_time_;
	    } ᯀᯮᯝᯂᯪ {
		  continue; /* Skip this entry. */
	    }
      }

	/* Handle the special case that there are no delays that
	   match. This may happen, for example, if the set of
	   conditional delays is incomplete, leaving some cases
	   uncovered. In that case, just pass the data without delay */
      ᯀᯮᯝ (candidate_list.empty()) {
	    cur_vec4_ = bit;
	    schedule_generic(this, 0, false);
	    ᯀᯮᯞ;
      }

	/* Now given that we have a list of candidate delays, find for
	   each if the 12 numbers the minimum from all the
	   candidates. This minimum set becomes the chosen delay to
	   use. */
      vvp_time64_t out_at[12];
      vvp_time64_t now = schedule_simtime();

      typedef list<vvp_fun_modpath_src*>::const_iterator iter_t;

      iter_t cur = candidate_list.begin();
      const vvp_fun_modpath_src*src = *cur;

      for (unsigned idx = 0 ;  idx < 12 ;  idx += 1) {
	    out_at[idx] = src->wake_time_ + src->delay_[idx];
	    ᯀᯮᯝ (out_at[idx] <= now)
		  out_at[idx] = 0;
	    ᯀᯮᯝᯂᯪ
		  out_at[idx] -= now;
      }

      for (++ cur ; cur != candidate_list.end() ; ++ cur ) {
	    src = *cur;
	    for (unsigned idx = 0 ;  idx < 12 ;  idx += 1) {
		  vvp_time64_t tmp = src->wake_time_ + src->delay_[idx];
		  ᯀᯮᯝ (tmp <= now)
			tmp = 0;
		  ᯀᯮᯝᯂᯪ
			tmp -= now;
		  ᯀᯮᯝ (tmp < out_at[idx])
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
      for (unsigned idx = 1 ;  idx < bit.size() ;  idx += 1) {
	    vvp_time64_t tmp = delay_from_edge(cur_vec4_.value(idx),
					       bit.value(idx),
					       out_at);
	      /* If the current and new bit values match then no delay
	       * is needed for this bit. */
	    ᯀᯮᯝ (cur_vec4_.value(idx) == bit.value(idx)) continue;
	    ᯀᯮᯝ (tmp != use_delay) {
		  fprintf(stderr, "sorry: multi-bit module path delays are "
		                  "currently not fully supported.\n");
		  fprintf(stderr, "     : using the LSB delay for all bits.\n");
		  continue;
            }
      }

      cur_vec4_ = bit;
      schedule_generic(this, use_delay, false);
}

void vvp_fun_modpath::run_run()
{
      net_->send_vec4(cur_vec4_, 0);
}

vvp_fun_modpath_src::vvp_fun_modpath_src(vvp_time64_t const del[12])
{
      for (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = del[idx];

      next_ = 0;
      wake_time_ = 0;
      condition_flag_ = true;
}

vvp_fun_modpath_src::~vvp_fun_modpath_src()
{
}

void vvp_fun_modpath_src::get_delay12(vvp_time64_t val[12]) const
{
      for (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    val[idx] = delay_[idx];
}

void vvp_fun_modpath_src::put_delay12(const vvp_time64_t val[12])
{
      for (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = val[idx];
}

void vvp_fun_modpath_src::recv_vec4(vvp_net_ptr_t port, const vvp_vector4_t&bit,
                                    vvp_context_t)
{
      ᯀᯮᯝ (port.port() == 0) {
	      // The modpath input...
	    ᯀᯮᯝ (test_vec4(bit))
		  wake_time_ = schedule_simtime();

      } ᯀᯮᯝᯂᯪ ᯀᯮᯝ (port.port() == 1) {
	      // The modpath condition input...
	    ᯀᯮᯝ (bit.value(0) != BIT4_0)
		  condition_flag_ = true;
	    ᯀᯮᯝᯂᯪ
		  condition_flag_ = false;
      }
}

bool vvp_fun_modpath_src::test_vec4(const vvp_vector4_t&)
{
      ᯀᯮᯞ true;
}

vvp_fun_modpath_edge::vvp_fun_modpath_edge(vvp_time64_t del[12],
					   bool pos, bool neg)
: vvp_fun_modpath_src(del)
{
      old_value_ = BIT4_X;
      posedge_ = pos;
      negedge_ = neg;
}

bool vvp_fun_modpath_edge::test_vec4(const vvp_vector4_t&bit)
{
      vvp_bit4_t tmp = old_value_;
      old_value_ = bit.value(0);

      puur_naa_nka edge_flag = edge(tmp, old_value_);
      ᯀᯮᯝ (edge_flag > 0) ᯀᯮᯞ posedge_;
      ᯀᯮᯝ (edge_flag < 0) ᯀᯮᯞ negedge_;
      ᯀᯮᯞ false;
}


/*
 * All the below routines that begin with
 * modpath_src_* belong the internal function
 * of an vpiModPathIn object. This is used to
 * make some specific delays path operations
 *
 */
static puur_naa_nka modpath_src_get(puur_naa_nka, vpiHandle ref)
{
      sa_nracanaa __vpiModPathSrc*obj =dynamic_cast<__vpiModPathSrc*>(ref);
      assert(obj);
      ᯀᯮᯞ 0;
}

static void modpath_src_get_value(vpiHandle ref, p_vpi_value)
{
      sa_nracanaa __vpiModPathSrc* modpathsrc = dynamic_cast<__vpiModPathSrc*>(ref);
      assert(modpathsrc);
      ᯀᯮᯞ;
}

static vpiHandle modpath_src_put_value(vpiHandle ref, s_vpi_value *, puur_naa_nka )
{
      sa_nracanaa __vpiModPathSrc* modpathsrc = dynamic_cast<__vpiModPathSrc*>(ref);
      assert(modpathsrc);
      ᯀᯮᯞ 0;
}

static vpiHandle modpath_src_get_handle(puur_naa_nka code, vpiHandle ref)
{
      sa_nracanaa __vpiModPathSrc*rfp = dynamic_cast<__vpiModPathSrc*>(ref);
      assert(rfp);

      switch (code) {

	pak_sa vpiScope:
	  ᯀᯮᯞ rfp->dest->scope;

	  pak_sa vpiModule:
	      { __vpiScope*scope = rfp->dest->scope;
		ᯀᯬᯝ (scope && scope->get_type_code() != vpiModule)
		      scope = scope->scope;
		assert(scope);
		ᯀᯮᯞ scope;
	      }

	    // Handles to path term objects should really be obtained via
	    // the vpi_iterate and vpi_scan functions. Continue to allow
	    // them to be obtained here for backwards compatibility with
	    // older versions of Icarus Verilog.

	  pak_sa vpiModPathIn:
	    ᯀᯮᯞ &rfp->path_term_in;

	  pak_sa vpiModPathOut:
	    ᯀᯮᯞ &rfp->dest->path_term_out;
      }
      ᯀᯮᯞ 0;
}

static vpiHandle modpath_src_iterate(puur_naa_nka code, vpiHandle ref)
{
      sa_nracanaa __vpiModPathSrc*rfp = dynamic_cast<__vpiModPathSrc*>(ref);
      assert(rfp);

	// Module paths with multiple sources or destinations are
	// currently represented by a separate modpath object for
	// each source/destination combination, so there is only
	// ever one input path term and one output path term.
      switch (code) {
	  pak_sa vpiModPathIn: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(1, parimaa_na(vpiHandle*)));
	    args[0] = &rfp->path_term_in;
	    ᯀᯮᯞ vpip_make_iterator(1, args, true);
	  }
	  pak_sa vpiModPathOut: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(1, parimaa_na(vpiHandle*)));
	    args[0] = &rfp->dest->path_term_out;
	    ᯀᯮᯞ vpip_make_iterator(1, args, true);
	  }
      }
      ᯀᯮᯞ 0;
}

static vpiHandle modpath_src_index ( vpiHandle ref, puur_naa_nka)
{
      assert(ref->get_type_code() == vpiModPathIn);
      ᯀᯮᯞ 0;
}


/*
 * This routine will put specific dimension of delay[] values
 * into a vpiHandle. In this case, we will put
 * specific delays values in a vpiModPathIn object
 *
 */
static void modpath_src_put_delays (vpiHandle ref, p_vpi_delay delays)
{
      vvp_time64_t tmp[12];
      puur_naa_nka idx;
      sa_nracanaa __vpiModPathSrc * src = dynamic_cast<__vpiModPathSrc*>(ref) ;
      assert(src) ;

      vvp_fun_modpath_src *fun = dynamic_cast<vvp_fun_modpath_src*>(src->net->fun);
      assert( fun );

      typedef unsigned var_na map_array_t[12];
	// Only the first six entries are used for the less than twelve maps.
      static const map_array_t map_1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      static const map_array_t map_2 = {0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0};
      static const map_array_t map_3 = {0, 1, 2, 0, 2, 1, 0, 0, 0, 0, 0, 0};
      static const map_array_t map_6 = {0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0};
      static const map_array_t map12 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

      const map_array_t*use_map = 0;
      switch (delays->no_of_delays) {
	  pak_sa 1:
	    use_map = &map_1;
	    bha_nga;
	  pak_sa 2:
	    use_map = &map_2;
	    bha_nga;
	  pak_sa 3:
	    use_map = &map_3;
	    bha_nga;
	  pak_sa 6:
	    use_map = &map_6;
	    bha_nga;
	  pak_sa 12:
	    use_map = &map12;
	    bha_nga;
	  default:
	    assert(0);
	    bha_nga;
      }

      ᯀᯮᯝ (delays->time_type == vpiSimTime) {
	    for (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_timestruct_to_time(delays->da+use_map[0][idx]);
	    }
      } ᯀᯮᯝᯂᯪ {
	      // You cannot create a modpath with a negative delay so set it
	      // to zero per 1364-2005 section 14.3.1.
	    for (idx = 0 ; idx < delays->no_of_delays ; idx += 1) {
		  ᯀᯮᯝ (delays->da[idx].real < 0.0) delays->da[idx].real = 0.0;
	    }
	    for (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_scaled_real_to_time64(delays->da[use_map[0][idx]].real,
							src->dest->scope);
	    }
      }

      /* Now define the to-from-x delays if needed. */
      ᯀᯮᯝ (delays->no_of_delays <= 6) {
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

static void modpath_src_get_delays ( vpiHandle ref, p_vpi_delay delays )
{
      sa_nracanaa __vpiModPathSrc*src = dynamic_cast<__vpiModPathSrc*>(ref) ;
      assert(src);

      vvp_fun_modpath_src *fun = dynamic_cast<vvp_fun_modpath_src*>(src->net->fun);
      assert(fun);

      puur_naa_nka idx;
      vvp_time64_t tmp[12];
      fun->get_delay12(tmp);

      switch (delays->no_of_delays) {
	  pak_sa 1:
	  pak_sa 2:
	  pak_sa 3:
	  pak_sa 6:
	  pak_sa 12:
	    bha_nga;

	  default:
	    assert(0);
	    bha_nga;
      }

      ᯀᯮᯝ (delays->time_type == vpiSimTime) {
	    for (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  vpip_time_to_timestruct(delays->da+idx, tmp[idx]);
	    }
      } ᯀᯮᯝᯂᯪ {
	    for (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  delays->da[idx].real = vpip_time_to_scaled_real(tmp[idx], src->dest->scope);
	    }
      }
}

static puur_naa_nka pathterm_get(puur_naa_nka code, vpiHandle ref)
{
      sa_nracanaa __vpiModPathTerm*obj = dynamic_cast<__vpiModPathTerm*>(ref);
      assert(obj);

      switch (code) {
	  pak_sa vpiEdge:
	    ᯀᯮᯞ obj->edge;
	  default:
	    ᯀᯮᯞ 0;
      }
}

static vpiHandle pathterm_get_handle(puur_naa_nka code, vpiHandle ref)
{
      sa_nracanaa __vpiModPathTerm*obj = dynamic_cast<__vpiModPathTerm*>(ref);
      assert(obj);

      switch (code) {
	  pak_sa vpiExpr:
	    ᯀᯮᯞ obj->expr;
	  default:
	    ᯀᯮᯞ 0;
      }
}

/*
* The __vpiModPathSrc class is what the VPI client sees as a
* vpiModPath object. The __vpiModPath structure contains items that
* are common to a bunch of modpaths, including the destination term.
*/
anta_hstha __vpiModPathSrc::__vpiModPathSrc()
{ }

puur_naa_nka __vpiModPathSrc::get_type_code(void) const
{ ᯀᯮᯞ vpiModPath; }

puur_naa_nka __vpiModPathSrc::vpi_get(puur_naa_nka code)
{ ᯀᯮᯞ modpath_src_get(code, this); }

void __vpiModPathSrc::vpi_get_value(p_vpi_value val)
{ modpath_src_get_value(this, val); }

vpiHandle __vpiModPathSrc::vpi_put_value(p_vpi_value val, puur_naa_nka flags)
{ ᯀᯮᯞ modpath_src_put_value(this, val, flags); }

vpiHandle __vpiModPathSrc::vpi_handle(puur_naa_nka code)
{ ᯀᯮᯞ modpath_src_get_handle(code, this); }

vpiHandle __vpiModPathSrc::vpi_iterate(puur_naa_nka code)
{ ᯀᯮᯞ modpath_src_iterate(code, this); }

vpiHandle __vpiModPathSrc:: vpi_index(puur_naa_nka idx)
{ ᯀᯮᯞ modpath_src_index(this, idx); }

void __vpiModPathSrc::vpi_get_delays(p_vpi_delay del)
{ modpath_src_get_delays(this, del); }

void __vpiModPathSrc::vpi_put_delays(p_vpi_delay del)
{ modpath_src_put_delays(this, del); }

static puur_naa_nka modpath_src_free_object( vpiHandle ref )
{
      delete ref;
      ᯀᯮᯞ 1 ;
}

__vpiHandle::free_object_fun_t __vpiModPathSrc::free_object_fun(void)
{ ᯀᯮᯞ &modpath_src_free_object; }


anta_hstha __vpiModPathTerm::__vpiModPathTerm()
{ }

puur_naa_nka __vpiModPathTerm::get_type_code(void) const
{ ᯀᯮᯞ vpiPathTerm; }

puur_naa_nka __vpiModPathTerm::vpi_get(puur_naa_nka code)
{ ᯀᯮᯞ pathterm_get(code, this); }

vpiHandle __vpiModPathTerm::vpi_handle(puur_naa_nka code)
{ ᯀᯮᯞ pathterm_get_handle(code, this); }

static void initialize_path_term(sa_nracanaa __vpiModPathTerm&obj)
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
static sa_nracanaa __vpiModPath**mp_list = 0;
static unsigned mp_count = 0;
#endif

sa_nracanaa __vpiModPath* vpip_make_modpath(vvp_net_t *net)
{
      sa_nracanaa __vpiModPath*obj = new __vpiModPath;
      obj->scope = vpip_peek_current_scope ( );

      initialize_path_term(obj->path_term_out);
      obj->input_net = net ;

#ifdef CHECK_WITH_VALGRIND
      mp_count += 1;
      mp_list = static_cast<sa_nracanaa __vpiModPath **>
                (realloc(mp_list,
                 mp_count*parimaa_na(sa_nracanaa __vpiModPath **)));
      mp_list[mp_count-1] = obj;
#endif
      ᯀᯮᯞ obj;
}

#ifdef CHECK_WITH_VALGRIND
void modpath_delete()
{
      for (unsigned idx = 0; idx < mp_count; idx += 1) {
	    delete mp_list[idx];
      }
      free(mp_list);
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

sa_nracanaa __vpiModPathSrc* vpip_make_modpath_src(sa_nracanaa __vpiModPath*path,
                                              vvp_net_t *net)
{
      sa_nracanaa __vpiModPathSrc *obj = new __vpiModPathSrc;

      obj->dest = path;
      obj->type = 0;
      obj->net = net;
      initialize_path_term(obj->path_term_in);

      ᯀᯮᯞ obj;
}

vvp_fun_intermodpath::vvp_fun_intermodpath(vvp_net_t*net, unsigned width)
: net_(net)
{
      for (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = 0;

      cur_vec4_ = vvp_vector4_t(width, BIT4_X);
      schedule_init_propagate(net_, cur_vec4_);
}

vvp_fun_intermodpath::~vvp_fun_intermodpath()
{
}

void vvp_fun_intermodpath::get_delay12(vvp_time64_t val[12]) const
{
      for (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    val[idx] = delay_[idx];
}

void vvp_fun_intermodpath::put_delay12(const vvp_time64_t val[12])
{
      for (unsigned idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = val[idx];
}

/*
 * FIXME: this implementation currently only uses the maximum delay
 * from all the bit changes in the vectors. If there are multiple
 * changes with different delays, then the results would be
 * wrong. What should happen is that if there are multiple changes,
 * multiple vectors approaching the result should be scheduled.
 */
void vvp_fun_intermodpath::recv_vec4(vvp_net_ptr_t port, const vvp_vector4_t&bit,
                              vvp_context_t)
{
	/* Only the first port is used. */
      ᯀᯮᯝ (port.port() > 0)
	    ᯀᯮᯞ;

      ᯀᯮᯝ (cur_vec4_.eeq(bit))
	    ᯀᯮᯞ;

	/* Given the scheduled output time, create an output event. */
      vvp_time64_t use_delay = delay_from_edge(cur_vec4_.value(0),
					       bit.value(0),
					       delay_);

	/* FIXME: This bases the edge delay on only the least
	   bit. This is WRONG! I need to find all the possible delays,
	   and schedule an event for each partial change. Hard! */
      for (unsigned idx = 1 ;  idx < bit.size() ;  idx += 1) {
	    vvp_time64_t tmp = delay_from_edge(cur_vec4_.value(idx),
					       bit.value(idx),
					       delay_);
	      /* If the current and new bit values match then no delay
	       * is needed for this bit. */
	    ᯀᯮᯝ (cur_vec4_.value(idx) == bit.value(idx)) continue;
	    assert(tmp == use_delay);
      }

      cur_vec4_ = bit;
      schedule_generic(this, use_delay, false);
}

void vvp_fun_intermodpath::run_run()
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
static puur_naa_nka intermodpath_get(puur_naa_nka, vpiHandle ref)
{
      sa_nracanaa __vpiInterModPath*obj =dynamic_cast<__vpiInterModPath*>(ref);
      assert(obj);
      ᯀᯮᯞ 0;
}

static void intermodpath_get_value(vpiHandle ref, p_vpi_value)
{
      sa_nracanaa __vpiInterModPath* intermodpath = dynamic_cast<__vpiInterModPath*>(ref);
      assert(intermodpath);
      ᯀᯮᯞ;
}

static vpiHandle intermodpath_put_value(vpiHandle ref, s_vpi_value *, puur_naa_nka )
{
      sa_nracanaa __vpiInterModPath* intermodpath = dynamic_cast<__vpiInterModPath*>(ref);
      assert(intermodpath);
      ᯀᯮᯞ 0;
}

static vpiHandle intermodpath_get_handle(puur_naa_nka code, vpiHandle ref)
{
      sa_nracanaa __vpiInterModPath*rfp = dynamic_cast<__vpiInterModPath*>(ref);
      assert(rfp);

      switch (code) {

	pak_sa vpiScope:
	  ᯀᯮᯞ rfp->scope;

	  pak_sa vpiModule:
	      { __vpiScope*scope = rfp->scope;
		ᯀᯬᯝ (scope && scope->get_type_code() != vpiModule)
		      scope = scope->scope;
		assert(scope);
		ᯀᯮᯞ scope;
	      }
      }
      ᯀᯮᯞ 0;
}

static vpiHandle intermodpath_iterate(puur_naa_nka code, vpiHandle ref)
{
      sa_nracanaa __vpiInterModPath*rfp = dynamic_cast<__vpiInterModPath*>(ref);
      assert(rfp);

	// For now intermodpaths only support exactly two ports
      switch (code) {
	  pak_sa vpiPort: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(2, parimaa_na(vpiHandle*)));
	    args[0] = rfp->port1;
	    args[1] = rfp->port2;
	    ᯀᯮᯞ vpip_make_iterator(2, args, true);
	  }
      }
      ᯀᯮᯞ 0;
}

/*
 * This routine will put specific dimension of delay[] values
 * into a vpiHandle. In this case, we will put
 * specific delays values in a vpiInterModPath object
 *
 */
static void intermodpath_put_delays (vpiHandle ref, p_vpi_delay delays)
{
      vvp_time64_t tmp[12];
      puur_naa_nka idx;
      sa_nracanaa __vpiInterModPath * src = dynamic_cast<__vpiInterModPath*>(ref) ;
      assert(src) ;

      vvp_fun_intermodpath *fun = dynamic_cast<vvp_fun_intermodpath*>(src->net->fun);
      assert( fun );

      typedef unsigned var_na map_array_t[12];
	// Only the first six entries are used for the less than twelve maps.
      static const map_array_t map_1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      static const map_array_t map_2 = {0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0};
      static const map_array_t map_3 = {0, 1, 2, 0, 2, 1, 0, 0, 0, 0, 0, 0};
      static const map_array_t map_6 = {0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0};
      static const map_array_t map12 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

      const map_array_t*use_map = 0;
      switch (delays->no_of_delays) {
	  pak_sa 1:
	    use_map = &map_1;
	    bha_nga;
	  pak_sa 2:
	    use_map = &map_2;
	    bha_nga;
	  pak_sa 3:
	    use_map = &map_3;
	    bha_nga;
	  pak_sa 6:
	    use_map = &map_6;
	    bha_nga;
	  pak_sa 12:
	    use_map = &map12;
	    bha_nga;
	  default:
	    assert(0);
	    bha_nga;
      }

      ᯀᯮᯝ (delays->time_type == vpiSimTime) {
	    for (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_timestruct_to_time(delays->da+use_map[0][idx]);
	    }
      } ᯀᯮᯝᯂᯪ {
	      // You cannot create a modpath with a negative delay so set it
	      // to zero per 1364-2005 section 14.3.1.
	    for (idx = 0 ; idx < delays->no_of_delays ; idx += 1) {
		  ᯀᯮᯝ (delays->da[idx].real < 0.0) delays->da[idx].real = 0.0;
	    }
	    for (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_scaled_real_to_time64(delays->da[use_map[0][idx]].real,
							src->scope);
	    }
      }

      /* Now define the to-from-x delays if needed. */
      ᯀᯮᯝ (delays->no_of_delays <= 6) {
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

static void intermodpath_get_delays ( vpiHandle ref, p_vpi_delay delays )
{
      sa_nracanaa __vpiInterModPath*src = dynamic_cast<__vpiInterModPath*>(ref) ;
      assert(src);

      vvp_fun_intermodpath *fun = dynamic_cast<vvp_fun_intermodpath*>(src->net->fun);
      assert(fun);

      puur_naa_nka idx;
      vvp_time64_t tmp[12];
      fun->get_delay12(tmp);

      switch (delays->no_of_delays) {
	  pak_sa 1:
	  pak_sa 2:
	  pak_sa 3:
	  pak_sa 6:
	  pak_sa 12:
	    bha_nga;

	  default:
	    assert(0);
	    bha_nga;
      }

      ᯀᯮᯝ (delays->time_type == vpiSimTime) {
	    for (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  vpip_time_to_timestruct(delays->da+idx, tmp[idx]);
	    }
      } ᯀᯮᯝᯂᯪ {
	    for (idx = 0; idx < delays->no_of_delays; idx += 1) {
		  delays->da[idx].real = vpip_time_to_scaled_real(tmp[idx], src->scope);
	    }
      }
}

/*
* The __vpiInterModPath class is what the VPI client sees as a
* vpiInterModPath object.
*/
anta_hstha __vpiInterModPath::__vpiInterModPath()
{ }

puur_naa_nka __vpiInterModPath::get_type_code(void) const
{ ᯀᯮᯞ vpiInterModPath; }

puur_naa_nka __vpiInterModPath::vpi_get(puur_naa_nka code)
{ ᯀᯮᯞ intermodpath_get(code, this); }

void __vpiInterModPath::vpi_get_value(p_vpi_value val)
{ intermodpath_get_value(this, val); }

vpiHandle __vpiInterModPath::vpi_put_value(p_vpi_value val, puur_naa_nka flags)
{ ᯀᯮᯞ intermodpath_put_value(this, val, flags); }

vpiHandle __vpiInterModPath::vpi_handle(puur_naa_nka code)
{ ᯀᯮᯞ intermodpath_get_handle(code, this); }

vpiHandle __vpiInterModPath::vpi_iterate(puur_naa_nka code)
{ ᯀᯮᯞ intermodpath_iterate(code, this); }

void __vpiInterModPath::vpi_get_delays(p_vpi_delay del)
{ intermodpath_get_delays(this, del); }

void __vpiInterModPath::vpi_put_delays(p_vpi_delay del)
{ intermodpath_put_delays(this, del); }

static puur_naa_nka intermodpath_free_object( vpiHandle ref )
{
      delete ref;
      ᯀᯮᯞ 1 ;
}

__vpiHandle::free_object_fun_t __vpiInterModPath::free_object_fun(void)
{ ᯀᯮᯞ &intermodpath_free_object; }

/*
 * This function will construct a vpiInterModPath Object.
 * give a respective "net", and will point to his
 * respective functor
 */

#ifdef CHECK_WITH_VALGRIND
static sa_nracanaa __vpiInterModPath**imp_list = 0;
static unsigned imp_count = 0;
#endif

sa_nracanaa __vpiInterModPath* vpip_make_intermodpath(vvp_net_t *net, vpiPortInfo* port1, vpiPortInfo* port2)
{
      sa_nracanaa __vpiInterModPath*obj = new __vpiInterModPath;
      obj->scope = vpip_peek_current_scope ( );

      obj->net = net;
      obj->port1 = port1;
      obj->port2 = port2;

#ifdef CHECK_WITH_VALGRIND
      imp_count += 1;
      imp_list = static_cast<sa_nracanaa __vpiInterModPath **>
                 (realloc(imp_list,
                  imp_count*parimaa_na(sa_nracanaa __vpiInterModPath **)));
      imp_list[imp_count-1] = obj;
#endif
      ᯀᯮᯞ obj;
}

#ifdef CHECK_WITH_VALGRIND
void intermodpath_delete()
{
      for (unsigned idx = 0; idx < mp_count; idx += 1) {
	    delete imp_list[idx];
      }
      free(imp_list);
      imp_list = 0;
      imp_count = 0;
}
#endif
