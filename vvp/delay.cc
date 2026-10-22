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

#అన్తర్భావ "delay.h"
#అన్తర్భావ "schedule.h"
#అన్తర్భావ "vpi_priv.h"
#అన్తర్భావ "config.h"
#నిర్వచితమైతే CHECK_WITH_VALGRIND
#అన్తర్భావ "vvp_cleanup.h"
#అయితే_ముగింపు
#అన్తర్భావ <iostream>
#అన్తర్భావ <cstdlib>
#అన్తర్భావ <list>
#అన్తర్భావ <cassert>
#అన్తర్భావ <cmath>
#అన్తర్భావ "ivl_alloc.h"

ఉపయోగించి నామాకాశ std;

శూన్య vvp_delay_t::calculate_min_delay_()
{
      min_delay_ = rise_;
      ఒకవేళ (fall_ < min_delay_)
	    min_delay_ = fall_;
      ఒకవేళ (ignore_decay_) decay_ = min_delay_;
      లేకపోతే ఒకవేళ (decay_ < min_delay_)
	    min_delay_ = decay_;
}

vvp_delay_t::vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall)
{
      rise_ = rise;
      fall_ = fall;
      decay_= fall < rise? fall : rise;
      min_delay_ = decay_;
      ignore_decay_ = అబద్ధం;
}

vvp_delay_t::vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall, vvp_time64_t decay)
{
      rise_ = rise;
      fall_ = fall;
      decay_= decay;
      ignore_decay_ = అబద్ధం;

      calculate_min_delay_();
}

శూన్య vvp_delay_t::సమితి_ignore_decay()
{
      ignore_decay_ = నిజం;

      calculate_min_delay_();
}

vvp_delay_t::~vvp_delay_t()
{
}

vvp_time64_t vvp_delay_t::get_delay(vvp_bit4_t from, vvp_bit4_t to)
{
      ఎంపిక (from) {
	  స్థితి BIT4_0:
	    ఎంపిక (to) {
		స్థితి BIT4_0: తిరిగి 0;
		స్థితి BIT4_1: తిరిగి rise_;
		స్థితి BIT4_X: తిరిగి min_delay_;
		స్థితి BIT4_Z: తిరిగి decay_;
	    }
	    ఆపు;
	  స్థితి BIT4_1:
	    ఎంపిక (to) {
		స్థితి BIT4_0: తిరిగి fall_;
		స్థితి BIT4_1: తిరిగి 0;
		స్థితి BIT4_X: తిరిగి min_delay_;
		స్థితి BIT4_Z: తిరిగి decay_;
	    }
	    ఆపు;
	  స్థితి BIT4_X:
	    ఎంపిక (to) {
		స్థితి BIT4_0: తిరిగి fall_;
		స్థితి BIT4_1: తిరిగి rise_;
		స్థితి BIT4_X: తిరిగి 0;
		స్థితి BIT4_Z: తిరిగి decay_;
	    }
	    ఆపు;
	  స్థితి BIT4_Z:
	    ఎంపిక (to) {
		స్థితి BIT4_0: తిరిగి fall_;
		స్థితి BIT4_1: తిరిగి rise_;
		స్థితి BIT4_X: తిరిగి min_delay_;
		స్థితి BIT4_Z: తిరిగి 0;
	    }
	    ఆపు;
      }

      ప్రతిజ్ఞా(0);
      తిరిగి 0;
}

vvp_time64_t vvp_delay_t::get_min_delay() స్థిరరాశి
{
      తిరిగి min_delay_;
}

శూన్య vvp_delay_t::సమితి_rise(vvp_time64_t val)
{
      rise_ = val;
      ఒకవేళ (val < min_delay_) {
	    min_delay_ = val;
	    ఒకవేళ (ignore_decay_) decay_ = val;
      } లేకపోతే
	    calculate_min_delay_();
}

శూన్య vvp_delay_t::సమితి_fall(vvp_time64_t val)
{
      fall_ = val;
      ఒకవేళ (val < min_delay_) {
	    min_delay_ = val;
	    ఒకవేళ (ignore_decay_) decay_ = val;
      } లేకపోతే
	    calculate_min_delay_();
}

శూన్య vvp_delay_t::సమితి_decay(vvp_time64_t val)
{
      ప్రతిజ్ఞా(!ignore_decay_);

      decay_ = val;
      ఒకవేళ (val < min_delay_)
	    min_delay_ = val;
      లేకపోతే
	    calculate_min_delay_();
}

vvp_fun_delay::vvp_fun_delay(vvp_net_t*n, గుర్తులేని width, స్థిరరాశి vvp_delay_t&d)
: net_(n), delay_(d)
{
      cur_real_ = 0.0;
      ఒకవేళ (width > 0) {
            cur_vec4_ = vvp_vector4_t(width, BIT4_X);
            cur_vec8_ = vvp_vector8_t(cur_vec4_, 6, 6);
            కాలపట్టిక_init_propagate(net_, cur_vec4_);
      } లేకపోతే {
            కాలపట్టిక_init_propagate(net_, cur_real_);
      }
      list_ = 0;
      type_ = UNKNOWN_DELAY;
      initial_ = నిజం;
	// Calculate the values used when converting variable delays
	// to simulation time units.
      స్థిరరాశి __vpiScope*scope = vpip_peek_current_scope();

      పూర్ణాంకం powr = scope->సమయం_units - scope->సమయం_precision;
      విడత = 1;
      కొరకు (పూర్ణాంకం lp = 0; lp < powr; lp += 1) విడత *= 10;

      powr = scope->సమయం_precision - vpip_get_సమయం_precision();
      scale_ = 1;
      కొరకు (పూర్ణాంకం lp = 0; lp < powr; lp += 1) scale_ *= 10;
}

vvp_fun_delay::~vvp_fun_delay()
{
      అయితే (నిర్మాణం event_*cur = వరుస_నుండి_తీయి())
	    తొలగించు cur;
}

తార్కిక vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        స్థిరరాశి vvp_vector4_t&బిట్టు)
{
      ఒకవేళ (list_ == 0) తిరిగి అబద్ధం;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      ఒకవేళ (list_->తదుపరి_2->ptr_vec4.eeq(బిట్టు)) తిరిగి నిజం;

      clean_pulse_events_(use_delay);
      తిరిగి అబద్ధం;
}

తార్కిక vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        స్థిరరాశి vvp_vector8_t&బిట్టు)
{
      ఒకవేళ (list_ == 0) తిరిగి అబద్ధం;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      ఒకవేళ (list_->తదుపరి_2->ptr_vec8.eeq(బిట్టు)) తిరిగి నిజం;

      clean_pulse_events_(use_delay);
      తిరిగి అబద్ధం;
}

తార్కిక vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        ద్విగుణ బిట్టు)
{
      ఒకవేళ (list_ == 0) తిరిగి అబద్ధం;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      ఒకవేళ (list_->తదుపరి_2->ptr_real == బిట్టు) తిరిగి నిజం;

      clean_pulse_events_(use_delay);
      తిరిగి అబద్ధం;
}

శూన్య vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay)
{
      ప్రతిజ్ఞా(list_ != 0);

      చేయి {
	    నిర్మాణం event_*cur = list_->తదుపరి_2;
	      /* If this event is far enough from the event I'm about
	         to create, then that scheduled event is not a pulse
	         to be eliminated, so we're done. */
	    ఒకవేళ (cur->sim_సమయం+use_delay <= use_delay+కాలపట్టిక_simtime())
		  ఆపు;

	    ఒకవేళ (list_ == cur)
		  list_ = 0;
	    లేకపోతే
		  list_->తదుపరి_2 = cur->తదుపరి_2;
	    తొలగించు cur;
      } అయితే (list_);
}

/*
 * FIXME: this implementation currently only uses the maximum delay
 * from all the bit changes in the vectors. If there are multiple
 * changes with different delays, then the results would be
 * wrong. What should happen is that if there are multiple changes,
 * multiple vectors approaching the result should be scheduled.
 */
శూన్య vvp_fun_delay::recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                              vvp_context_t)
{
      ఒకవేళ (పోర్టు.పోర్టు() > 0) {
	      // Get the integer value of the bit vector, or 0 if
	      // there are X or Z bits.
	    vvp_time64_t bval = 0;
	      // The following does not work correctly for negative values.
	      // They should be sign extended to 64 bits (1364-2001 9.7.1).
	    vector4_గమ్యం_విలువ(బిట్టు, bval);
	      // Integer values do not need to be rounded so just scale them.
	    vvp_time64_t val = bval * విడత * scale_;

	    ఎంపిక (పోర్టు.పోర్టు()) {
		స్థితి 1:
		  delay_.సమితి_rise(val);
		  తిరిగి;
		స్థితి 2:
		  delay_.సమితి_fall(val);
		  తిరిగి;
		స్థితి 3:
		  delay_.సమితి_decay(val);
		  తిరిగి;
	    }
	    తిరిగి;
      }

      vvp_time64_t use_delay;
	/* This is an initial value so it needs to be compared to all the
	   bits (the order the bits are changed is not deterministic). */
      ఒకవేళ (initial_) {
	    type_ = VEC4_DELAY;
            cur_vec8_ = vvp_vector8_t(vvp_vector4_t(0, BIT4_X), 6, 6);
	    vvp_bit4_t cur_val = cur_vec4_.value(0);
	    use_delay = delay_.get_delay(cur_val, బిట్టు.value(0));
	    కొరకు (గుర్తులేని idx = 1 ;  idx < బిట్టు.size() ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(cur_val, బిట్టు.value(idx));
		  ఒకవేళ (tmp > use_delay) use_delay = tmp;
	    }
      } లేకపోతే {
	    ప్రతిజ్ఞా(type_ == VEC4_DELAY);

	      // Use as a reference for calculating the delay the
	      // current value of the output. Detect and handle the
	      // special case that the event list contains the current
	      // value as a zero-delay-remaining event.
	    స్థిరరాశి vvp_vector4_t&use_vec4 = (list_ && list_->తదుపరి_2->sim_సమయం == కాలపట్టిక_simtime())? list_->తదుపరి_2->ptr_vec4 : cur_vec4_;

	      /* How many bits to compare? */
	    గుర్తులేని use_wid = use_vec4.size();
	    ఒకవేళ (బిట్టు.size() < use_wid) use_wid = బిట్టు.size();

	      /* Scan the vectors looking for delays. Select the maximum
	         delay encountered. */
	    use_delay = delay_.get_delay(use_vec4.value(0), బిట్టు.value(0));

	    కొరకు (గుర్తులేని idx = 1 ;  idx < use_wid ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(use_vec4.value(idx), బిట్టు.value(idx));
		  ఒకవేళ (tmp > use_delay) use_delay = tmp;
	    }
      }

      /* what *should* happen here is we check to see if there is a
         transaction in the queue. This would be a pulse that needs to be
         eliminated. */
      ఒకవేళ (clean_pulse_events_(use_delay, బిట్టు)) తిరిగి;

      vvp_time64_t use_simtime = కాలపట్టిక_simtime() + use_delay;

	/* And propagate it. */
      ఒకవేళ (use_delay == 0 && list_ == 0) {
	    cur_vec4_ = బిట్టు;
	    initial_ = అబద్ధం;
	    net_->send_vec4(cur_vec4_, 0);
      } లేకపోతే {
	    నిర్మాణం event_*cur = కొత్త నిర్మాణం event_(use_simtime);
	    cur->నిరంతరం_నిరంతరం_ptr = &vvp_fun_delay::నిరంతరం_నిరంతరం_vec4;
	    cur->ptr_vec4 = బిట్టు;
	    వరుసలో_చేర్చు(cur);
	    కాలపట్టిక_generic(ఇది, use_delay, అబద్ధం);
      }
}

శూన్య vvp_fun_delay::recv_vec4_pv(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
			         గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, బిట్టు, పునాది, vwid, ctx);
}

/* See the recv_vec4 comment above. */
శూన్య vvp_fun_delay::recv_vec8(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector8_t&బిట్టు)
{
      ప్రతిజ్ఞా(పోర్టు.పోర్టు() == 0);

      vvp_time64_t use_delay;
	/* This is an initial value so it needs to be compared to all the
	   bits (the order the bits are changed is not deterministic). */
      ఒకవేళ (initial_) {
	    type_ = VEC8_DELAY;
            cur_vec4_ = vvp_vector4_t(0, BIT4_X);
	    vvp_bit4_t cur_val = cur_vec8_.value(0).value();
	    use_delay = delay_.get_delay(cur_val, బిట్టు.value(0).value());
	    కొరకు (గుర్తులేని idx = 1 ;  idx < బిట్టు.size() ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(cur_val, బిట్టు.value(idx).value());
		  ఒకవేళ (tmp > use_delay) use_delay = tmp;
	    }
      } లేకపోతే {
	    ప్రతిజ్ఞా(type_ == VEC8_DELAY);

	      // Use as a reference for calculating the delay the
	      // current value of the output. Detect and handle the
	      // special case that the event list contains the current
	      // value as a zero-delay-remaining event.
	    స్థిరరాశి vvp_vector8_t&use_vec8 = (list_ && list_->తదుపరి_2->sim_సమయం == కాలపట్టిక_simtime())? list_->తదుపరి_2->ptr_vec8 : cur_vec8_;

	      /* How many bits to compare? */
	    గుర్తులేని use_wid = use_vec8.size();
	    ఒకవేళ (బిట్టు.size() < use_wid) use_wid = బిట్టు.size();

	      /* Scan the vectors looking for delays. Select the maximum
	         delay encountered. */
	    use_delay = delay_.get_delay(use_vec8.value(0).value(),
	                                 బిట్టు.value(0).value());

	    కొరకు (గుర్తులేని idx = 1 ;  idx < use_wid ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(use_vec8.value(idx).value(),
		                         బిట్టు.value(idx).value());
		  ఒకవేళ (tmp > use_delay) use_delay = tmp;
	    }
      }

      /* what *should* happen here is we check to see if there is a
         transaction in the queue. This would be a pulse that needs to be
         eliminated. */
      ఒకవేళ (clean_pulse_events_(use_delay, బిట్టు)) తిరిగి;

      vvp_time64_t use_simtime = కాలపట్టిక_simtime() + use_delay;

	/* And propagate it. */
      ఒకవేళ (use_delay == 0 && list_ == 0) {
	    cur_vec8_ = బిట్టు;
	    initial_ = అబద్ధం;
	    net_->send_vec8(cur_vec8_);
      } లేకపోతే {
	    నిర్మాణం event_*cur = కొత్త నిర్మాణం event_(use_simtime);
	    cur->ptr_vec8 = బిట్టు;
	    cur->నిరంతరం_నిరంతరం_ptr = &vvp_fun_delay::నిరంతరం_నిరంతరం_vec8;
	    వరుసలో_చేర్చు(cur);
	    కాలపట్టిక_generic(ఇది, use_delay, అబద్ధం);
      }
}

శూన్య vvp_fun_delay::recv_vec8_pv(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector8_t&బిట్టు,
			         గుర్తులేని పునాది, గుర్తులేని vwid)
{
      recv_vec8_pv_(ptr, బిట్టు, పునాది, vwid);
}

శూన్య vvp_fun_delay::recv_real(vvp_net_ptr_t పోర్టు, ద్విగుణ బిట్టు,
                              vvp_context_t)
{
      ఒకవేళ (పోర్టు.పోర్టు() > 0) {
	    /* If the port is not 0, then this is a delay value that
	    should be rounded and converted to an integer delay. */
	    vvp_time64_t val = 0;
	    ఒకవేళ (బిట్టు > -0.5) {
		  val = (vvp_time64_t) (బిట్టు * విడత + 0.5) * scale_;
	    } లేకపోతే ఒకవేళ (బిట్టు != బిట్టు) {
		    // For a NaN we use the default (0).
	    } లేకపోతే {
		  vvp_vector4_t vec4(8*పరిమాణం(vvp_time64_t),
		                     floor(-బిట్టు * విడత + 0.5) * -1 * scale_);
		  vector4_గమ్యం_విలువ(vec4, val);
	    }

	    ఎంపిక (పోర్టు.పోర్టు()) {
		స్థితి 1:
		  delay_.సమితి_rise(val);
		  తిరిగి;
		స్థితి 2:
		  delay_.సమితి_fall(val);
		  తిరిగి;
		స్థితి 3:
		  delay_.సమితి_decay(val);
		  తిరిగి;
	    }
	    తిరిగి;
      }

      ఒకవేళ (initial_) {
	    type_ = REAL_DELAY;
            cur_vec4_ = vvp_vector4_t(0, BIT4_X);
            cur_vec8_ = vvp_vector8_t(cur_vec4_, 6, 6);
      } లేకపోతే ప్రతిజ్ఞా(type_ == REAL_DELAY);

      vvp_time64_t use_delay;
      use_delay = delay_.get_min_delay();

      /* Eliminate glitches. */
      ఒకవేళ (clean_pulse_events_(use_delay, బిట్టు)) తిరిగి;

      /* This must be done after cleaning pulses to avoid propagating
       * an incorrect value. */
      ఒకవేళ (cur_real_ == బిట్టు) తిరిగి;

      vvp_time64_t use_simtime = కాలపట్టిక_simtime() + use_delay;

      ఒకవేళ (use_delay == 0 && list_ == 0) {
	    cur_real_ = బిట్టు;
	    initial_ = అబద్ధం;
	    net_->send_real(cur_real_, 0);
      } లేకపోతే {
	    నిర్మాణం event_*cur = కొత్త నిర్మాణం event_(use_simtime);
	    cur->నిరంతరం_నిరంతరం_ptr = &vvp_fun_delay::నిరంతరం_నిరంతరం_real;
	    cur->ptr_real = బిట్టు;
	    వరుసలో_చేర్చు(cur);

	    కాలపట్టిక_generic(ఇది, use_delay, అబద్ధం);
      }
}

శూన్య vvp_fun_delay::నిరంతరం_నిరంతరం()
{
      vvp_time64_t sim_సమయం = కాలపట్టిక_simtime();
      ఒకవేళ (list_ == 0 || list_->తదుపరి_2->sim_సమయం > sim_సమయం)
	    తిరిగి;

      నిర్మాణం event_*cur = వరుస_నుండి_తీయి();
      ఒకవేళ (cur == 0)
	    తిరిగి;

      (ఇది->*(cur->నిరంతరం_నిరంతరం_ptr))(cur);
      initial_ = అబద్ధం;
      తొలగించు cur;
}

శూన్య vvp_fun_delay::నిరంతరం_నిరంతరం_vec4(స్థిరరాశి నిర్మాణం event_*cur)
{
      cur_vec4_ = cur->ptr_vec4;
      net_->send_vec4(cur_vec4_, 0);
}

శూన్య vvp_fun_delay::నిరంతరం_నిరంతరం_vec8(స్థిరరాశి నిర్మాణం vvp_fun_delay::event_*cur)
{
      cur_vec8_ = cur->ptr_vec8;
      net_->send_vec8(cur_vec8_);
}

శూన్య vvp_fun_delay::నిరంతరం_నిరంతరం_real(స్థిరరాశి నిర్మాణం vvp_fun_delay::event_*cur)
{
      cur_real_ = cur->ptr_real;
      net_->send_real(cur_real_, 0);
}

vvp_fun_modpath::vvp_fun_modpath(vvp_net_t*net, గుర్తులేని width)
: net_(net), src_list_(0), ifnone_list_(0)
{
      cur_vec4_ = vvp_vector4_t(width, BIT4_X);
      కాలపట్టిక_init_propagate(net_, cur_vec4_);
}

vvp_fun_modpath::~vvp_fun_modpath()
{
	// Delete the source probes.
      అయితే (src_list_) {
	    vvp_fun_modpath_src*tmp = src_list_;
	    src_list_ = tmp->తదుపరి;
	    తొలగించు tmp;
      }
      అయితే (ifnone_list_) {
	    vvp_fun_modpath_src*tmp = ifnone_list_;
	    ifnone_list_ = tmp->తదుపరి;
	    తొలగించు tmp;
      }
}

శూన్య vvp_fun_modpath::కూడు_modpath_src(vvp_fun_modpath_src*that, తార్కిక ifnone)
{
      ప్రతిజ్ఞా(that->తదుపరి == 0);
      ఒకవేళ (ifnone) {
	    that->తదుపరి = ifnone_list_;
	    ifnone_list_ = that;
      } లేకపోతే {
	    that->తదుపరి = src_list_;
	    src_list_ = that;
      }
}

స్థిర vvp_time64_t delay_from_అంచు(vvp_bit4_t a, vvp_bit4_t b,
                                           vvp_time64_t స్థిరరాశి array[12])
{
      రకనిర్వచనం delay_అంచు_t bit4_table4[4];
      స్థిర స్థిరరాశి bit4_table4 అంచు_పట్టిక[4] = {
	    { DELAY_అంచు_01, DELAY_అంచు_01, DELAY_అంచు_0z, DELAY_అంచు_0x },
	    { DELAY_అంచు_10, DELAY_అంచు_10, DELAY_అంచు_1z, DELAY_అంచు_1x },
	    { DELAY_అంచు_z0, DELAY_అంచు_z1, DELAY_అంచు_z0, DELAY_అంచు_zx },
	    { DELAY_అంచు_x0, DELAY_అంచు_x1, DELAY_అంచు_xz, DELAY_అంచు_x0 }
      };

      తిరిగి array[ అంచు_పట్టిక[a][b] ];
}

శూన్య vvp_fun_modpath::recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                                vvp_context_t)
{
	/* Only the first port is used. */
      ఒకవేళ (పోర్టు.పోర్టు() > 0)
	    తిరిగి;

      ఒకవేళ (cur_vec4_.eeq(బిట్టు))
	    తిరిగి;

	/* Select a time delay source that applies. Notice that there
	   may be multiple delay sources that apply, so collect all
	   the candidates into a list first. */
      list<vvp_fun_modpath_src*>candidate_list;
      vvp_time64_t candidate_wake_సమయం = 0;
      కొరకు (vvp_fun_modpath_src*cur = src_list_ ;  cur ;  cur=cur->తదుపరి) {
	      /* Skip paths that are disabled by conditions. */
	    ఒకవేళ (cur->condition_flag_ == అబద్ధం)
		  కొనసాగు;

	    ఒకవేళ (candidate_list.empty()) {
		  candidate_list.push_back(cur);
		  candidate_wake_సమయం = cur->wake_సమయం;
	    } లేకపోతే ఒకవేళ (cur->wake_సమయం == candidate_wake_సమయం) {
		  candidate_list.push_back(cur);
	    } లేకపోతే ఒకవేళ (cur->wake_సమయం > candidate_wake_సమయం) {
		  candidate_list.assign(1, cur);
		  candidate_wake_సమయం = cur->wake_సమయం;
	    } లేకపోతే {
		  కొనసాగు; /* Skip this entry. */
	    }
      }

	/* Only add the ifnone delay if it has a later wake_time_ or
	 * if there are no normal delays. */
      vvp_time64_t ifnone_wake_సమయం = candidate_wake_సమయం;
      కొరకు (vvp_fun_modpath_src*cur = ifnone_list_ ;  cur ;  cur=cur->తదుపరి) {
	    ఒకవేళ (candidate_list.empty()) {
		  candidate_list.push_back(cur);
		  ifnone_wake_సమయం = cur->wake_సమయం;
	    } లేకపోతే ఒకవేళ (cur->wake_సమయం == ifnone_wake_సమయం &&
	               ifnone_wake_సమయం > candidate_wake_సమయం) {
		  candidate_list.push_back(cur);
	    } లేకపోతే ఒకవేళ (cur->wake_సమయం > ifnone_wake_సమయం) {
		  candidate_list.assign(1, cur);
		  ifnone_wake_సమయం = cur->wake_సమయం;
	    } లేకపోతే {
		  కొనసాగు; /* Skip this entry. */
	    }
      }

	/* Handle the special case that there are no delays that
	   match. This may happen, for example, if the set of
	   conditional delays is incomplete, leaving some cases
	   uncovered. In that case, just pass the data without delay */
      ఒకవేళ (candidate_list.empty()) {
	    cur_vec4_ = బిట్టు;
	    కాలపట్టిక_generic(ఇది, 0, అబద్ధం);
	    తిరిగి;
      }

	/* Now given that we have a list of candidate delays, find for
	   each if the 12 numbers the minimum from all the
	   candidates. This minimum set becomes the chosen delay to
	   use. */
      vvp_time64_t బహిర్గతం_at[12];
      vvp_time64_t now = కాలపట్టిక_simtime();

      రకనిర్వచనం list<vvp_fun_modpath_src*>::const_iterator iter_t;

      iter_t cur = candidate_list.begin();
      స్థిరరాశి vvp_fun_modpath_src*src = *cur;

      కొరకు (గుర్తులేని idx = 0 ;  idx < 12 ;  idx += 1) {
	    బహిర్గతం_at[idx] = src->wake_సమయం + src->delay_[idx];
	    ఒకవేళ (బహిర్గతం_at[idx] <= now)
		  బహిర్గతం_at[idx] = 0;
	    లేకపోతే
		  బహిర్గతం_at[idx] -= now;
      }

      కొరకు (++ cur ; cur != candidate_list.చివర() ; ++ cur ) {
	    src = *cur;
	    కొరకు (గుర్తులేని idx = 0 ;  idx < 12 ;  idx += 1) {
		  vvp_time64_t tmp = src->wake_సమయం + src->delay_[idx];
		  ఒకవేళ (tmp <= now)
			tmp = 0;
		  లేకపోతే
			tmp -= now;
		  ఒకవేళ (tmp < బహిర్గతం_at[idx])
			బహిర్గతం_at[idx] = tmp;
	    }
      }

	/* Given the scheduled output time, create an output event. */
      vvp_time64_t use_delay = delay_from_అంచు(cur_vec4_.value(0),
					       బిట్టు.value(0),
					       బహిర్గతం_at);

	/* FIXME: This bases the edge delay on only the least
	   bit. This is WRONG! I need to find all the possible delays,
	   and schedule an event for each partial change. Hard! */
      కొరకు (గుర్తులేని idx = 1 ;  idx < బిట్టు.size() ;  idx += 1) {
	    vvp_time64_t tmp = delay_from_అంచు(cur_vec4_.value(idx),
					       బిట్టు.value(idx),
					       బహిర్గతం_at);
	      /* If the current and new bit values match then no delay
	       * is needed for this bit. */
	    ఒకవేళ (cur_vec4_.value(idx) == బిట్టు.value(idx)) కొనసాగు;
	    ఒకవేళ (tmp != use_delay) {
		  fprintf(stderr, "sorry: multi-bit module path delays are "
		                  "currently not fully supported.\n");
		  fprintf(stderr, "     : using the LSB delay for all bits.\n");
		  కొనసాగు;
            }
      }

      cur_vec4_ = బిట్టు;
      కాలపట్టిక_generic(ఇది, use_delay, అబద్ధం);
}

శూన్య vvp_fun_modpath::నిరంతరం_నిరంతరం()
{
      net_->send_vec4(cur_vec4_, 0);
}

vvp_fun_modpath_src::vvp_fun_modpath_src(vvp_time64_t స్థిరరాశి del[12])
{
      కొరకు (గుర్తులేని idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = del[idx];

      తదుపరి = 0;
      wake_సమయం = 0;
      condition_flag_ = నిజం;
}

vvp_fun_modpath_src::~vvp_fun_modpath_src()
{
}

శూన్య vvp_fun_modpath_src::get_delay12(vvp_time64_t val[12]) స్థిరరాశి
{
      కొరకు (గుర్తులేని idx = 0 ;  idx < 12 ;  idx += 1)
	    val[idx] = delay_[idx];
}

శూన్య vvp_fun_modpath_src::put_delay12(స్థిరరాశి vvp_time64_t val[12])
{
      కొరకు (గుర్తులేని idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = val[idx];
}

శూన్య vvp_fun_modpath_src::recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                                    vvp_context_t)
{
      ఒకవేళ (పోర్టు.పోర్టు() == 0) {
	      // The modpath input...
	    ఒకవేళ (test_vec4(బిట్టు))
		  wake_సమయం = కాలపట్టిక_simtime();

      } లేకపోతే ఒకవేళ (పోర్టు.పోర్టు() == 1) {
	      // The modpath condition input...
	    ఒకవేళ (బిట్టు.value(0) != BIT4_0)
		  condition_flag_ = నిజం;
	    లేకపోతే
		  condition_flag_ = అబద్ధం;
      }
}

తార్కిక vvp_fun_modpath_src::test_vec4(స్థిరరాశి vvp_vector4_t&)
{
      తిరిగి నిజం;
}

vvp_fun_modpath_అంచు::vvp_fun_modpath_అంచు(vvp_time64_t del[12],
					   తార్కిక pos, తార్కిక neg)
: vvp_fun_modpath_src(del)
{
      old_విలువ = BIT4_X;
      posedge_ = pos;
      negedge_ = neg;
}

తార్కిక vvp_fun_modpath_అంచు::test_vec4(స్థిరరాశి vvp_vector4_t&బిట్టు)
{
      vvp_bit4_t tmp = old_విలువ;
      old_విలువ = బిట్టు.value(0);

      పూర్ణాంకం అంచు_flag = అంచు(tmp, old_విలువ);
      ఒకవేళ (అంచు_flag > 0) తిరిగి posedge_;
      ఒకవేళ (అంచు_flag < 0) తిరిగి negedge_;
      తిరిగి అబద్ధం;
}


/*
 * All the below routines that begin with
 * modpath_src_* belong the internal function
 * of an vpiModPathIn object. This is used to
 * make some specific delays path operations
 *
 */
స్థిర పూర్ణాంకం modpath_src_get(పూర్ణాంకం, vpiHandle ref)
{
      నిర్మాణం __vpiModPathSrc*obj =dynamic_cast<__vpiModPathSrc*>(ref);
      ప్రతిజ్ఞా(obj);
      తిరిగి 0;
}

స్థిర శూన్య modpath_src_get_విలువ(vpiHandle ref, p_vpi_విలువ)
{
      నిర్మాణం __vpiModPathSrc* modpathsrc = dynamic_cast<__vpiModPathSrc*>(ref);
      ప్రతిజ్ఞా(modpathsrc);
      తిరిగి;
}

స్థిర vpiHandle modpath_src_put_విలువ(vpiHandle ref, s_vpi_విలువ *, పూర్ణాంకం )
{
      నిర్మాణం __vpiModPathSrc* modpathsrc = dynamic_cast<__vpiModPathSrc*>(ref);
      ప్రతిజ్ఞా(modpathsrc);
      తిరిగి 0;
}

స్థిర vpiHandle modpath_src_get_handle(పూర్ణాంకం code, vpiHandle ref)
{
      నిర్మాణం __vpiModPathSrc*rfp = dynamic_cast<__vpiModPathSrc*>(ref);
      ప్రతిజ్ఞా(rfp);

      ఎంపిక (code) {

	స్థితి vpiScope:
	  తిరిగి rfp->dest->scope;

	  స్థితి vpiModule:
	      { __vpiScope*scope = rfp->dest->scope;
		అయితే (scope && scope->get_type_సంకేతం() != vpiModule)
		      scope = scope->scope;
		ప్రతిజ్ఞా(scope);
		తిరిగి scope;
	      }

	    // Handles to path term objects should really be obtained via
	    // the vpi_iterate and vpi_scan functions. Continue to allow
	    // them to be obtained here for backwards compatibility with
	    // older versions of Icarus Verilog.

	  స్థితి vpi_Mod_మార్గం_లోపల:
	    తిరిగి &rfp->మార్గం_పదం_లోపల;

	  స్థితి vpi_Mod_మార్గం_బహిర్గతం:
	    తిరిగి &rfp->dest->మార్గం_పదం_బహిర్గతం;
      }
      తిరిగి 0;
}

స్థిర vpiHandle modpath_src_iterate(పూర్ణాంకం code, vpiHandle ref)
{
      నిర్మాణం __vpiModPathSrc*rfp = dynamic_cast<__vpiModPathSrc*>(ref);
      ప్రతిజ్ఞా(rfp);

	// Module paths with multiple sources or destinations are
	// currently represented by a separate modpath object for
	// each source/destination combination, so there is only
	// ever one input path term and one output path term.
      ఎంపిక (code) {
	  స్థితి vpi_Mod_మార్గం_లోపల: {
	    vpiHandle*వాదనలు = static_cast<vpiHandle*>(సున్నా_జ్ఞాపకం_ఇవ్వు(1, పరిమాణం(vpiHandle*)));
	    వాదనలు[0] = &rfp->మార్గం_పదం_లోపల;
	    తిరిగి vpip_make_iterator(1, వాదనలు, నిజం);
	  }
	  స్థితి vpi_Mod_మార్గం_బహిర్గతం: {
	    vpiHandle*వాదనలు = static_cast<vpiHandle*>(సున్నా_జ్ఞాపకం_ఇవ్వు(1, పరిమాణం(vpiHandle*)));
	    వాదనలు[0] = &rfp->dest->మార్గం_పదం_బహిర్గతం;
	    తిరిగి vpip_make_iterator(1, వాదనలు, నిజం);
	  }
      }
      తిరిగి 0;
}

స్థిర vpiHandle modpath_src_సూచీ ( vpiHandle ref, పూర్ణాంకం)
{
      ప్రతిజ్ఞా(ref->get_type_సంకేతం() == vpi_Mod_మార్గం_లోపల);
      తిరిగి 0;
}


/*
 * This routine will put specific dimension of delay[] values
 * into a vpiHandle. In this case, we will put
 * specific delays values in a vpiModPathIn object
 *
 */
స్థిర శూన్య modpath_src_put_delays (vpiHandle ref, p_vpi_delay delays)
{
      vvp_time64_t tmp[12];
      పూర్ణాంకం idx;
      నిర్మాణం __vpiModPathSrc * src = dynamic_cast<__vpiModPathSrc*>(ref) ;
      ప్రతిజ్ఞా(src) ;

      vvp_fun_modpath_src *fun = dynamic_cast<vvp_fun_modpath_src*>(src->net->fun);
      ప్రతిజ్ఞా( fun );

      రకనిర్వచనం గుర్తులేని అక్షరం map_array_t[12];
	// Only the first six entries are used for the less than twelve maps.
      స్థిర స్థిరరాశి map_array_t map_1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      స్థిర స్థిరరాశి map_array_t map_2 = {0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0};
      స్థిర స్థిరరాశి map_array_t map_3 = {0, 1, 2, 0, 2, 1, 0, 0, 0, 0, 0, 0};
      స్థిర స్థిరరాశి map_array_t map_6 = {0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0};
      స్థిర స్థిరరాశి map_array_t map12 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

      స్థిరరాశి map_array_t*use_map = 0;
      ఎంపిక (delays->no_యొక్క_delays) {
	  స్థితి 1:
	    use_map = &map_1;
	    ఆపు;
	  స్థితి 2:
	    use_map = &map_2;
	    ఆపు;
	  స్థితి 3:
	    use_map = &map_3;
	    ఆపు;
	  స్థితి 6:
	    use_map = &map_6;
	    ఆపు;
	  స్థితి 12:
	    use_map = &map12;
	    ఆపు;
	  అప్రమేయ:
	    ప్రతిజ్ఞా(0);
	    ఆపు;
      }

      ఒకవేళ (delays->సమయం_type == vpi_Sim_సమయం) {
	    కొరకు (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_timestruct_గమ్యం_సమయం(delays->da+use_map[0][idx]);
	    }
      } లేకపోతే {
	      // You cannot create a modpath with a negative delay so set it
	      // to zero per 1364-2005 section 14.3.1.
	    కొరకు (idx = 0 ; idx < delays->no_యొక్క_delays ; idx += 1) {
		  ఒకవేళ (delays->da[idx].real < 0.0) delays->da[idx].real = 0.0;
	    }
	    కొరకు (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_scaled_real_గమ్యం_time64(delays->da[use_map[0][idx]].real,
							src->dest->scope);
	    }
      }

      /* Now define the to-from-x delays if needed. */
      ఒకవేళ (delays->no_యొక్క_delays <= 6) {
	      /* 0->x is the minimum of 0->z and 0->1. */
	    tmp[DELAY_అంచు_0x] = tmp[DELAY_అంచు_0z] < tmp[DELAY_అంచు_01] ?
	                           tmp[DELAY_అంచు_0z] : tmp[DELAY_అంచు_01];
	      /* x->1 is the maximum of z->1 and 0->1. */
	    tmp[DELAY_అంచు_x1] = tmp[DELAY_అంచు_z1] > tmp[DELAY_అంచు_01] ?
	                           tmp[DELAY_అంచు_z1] : tmp[DELAY_అంచు_01];
	      /* 1->x is the minimum of 1->z and 1->0. */
	    tmp[DELAY_అంచు_1x] = tmp[DELAY_అంచు_1z] < tmp[DELAY_అంచు_10] ?
	                           tmp[DELAY_అంచు_1z] : tmp[DELAY_అంచు_10];
	      /* x->0 is the maximum of z->0 and 1->0. */
	    tmp[DELAY_అంచు_x0] = tmp[DELAY_అంచు_z0] > tmp[DELAY_అంచు_10] ?
	                           tmp[DELAY_అంచు_z0] : tmp[DELAY_అంచు_10];
	      /* x->z is the maximum of 1->z and 0->z. */
	    tmp[DELAY_అంచు_xz] = tmp[DELAY_అంచు_1z] > tmp[DELAY_అంచు_0z] ?
	                           tmp[DELAY_అంచు_1z] : tmp[DELAY_అంచు_0z];
	      /* z->x is the minimum of z->1 and z->0. */
	    tmp[DELAY_అంచు_zx] = tmp[DELAY_అంచు_z1] < tmp[DELAY_అంచు_z0] ?
	                           tmp[DELAY_అంచు_z1] : tmp[DELAY_అంచు_z0];
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

స్థిర శూన్య modpath_src_get_delays ( vpiHandle ref, p_vpi_delay delays )
{
      నిర్మాణం __vpiModPathSrc*src = dynamic_cast<__vpiModPathSrc*>(ref) ;
      ప్రతిజ్ఞా(src);

      vvp_fun_modpath_src *fun = dynamic_cast<vvp_fun_modpath_src*>(src->net->fun);
      ప్రతిజ్ఞా(fun);

      పూర్ణాంకం idx;
      vvp_time64_t tmp[12];
      fun->get_delay12(tmp);

      ఎంపిక (delays->no_యొక్క_delays) {
	  స్థితి 1:
	  స్థితి 2:
	  స్థితి 3:
	  స్థితి 6:
	  స్థితి 12:
	    ఆపు;

	  అప్రమేయ:
	    ప్రతిజ్ఞా(0);
	    ఆపు;
      }

      ఒకవేళ (delays->సమయం_type == vpi_Sim_సమయం) {
	    కొరకు (idx = 0; idx < delays->no_యొక్క_delays; idx += 1) {
		  vpip_సమయం_గమ్యం_timestruct(delays->da+idx, tmp[idx]);
	    }
      } లేకపోతే {
	    కొరకు (idx = 0; idx < delays->no_యొక్క_delays; idx += 1) {
		  delays->da[idx].real = vpip_సమయం_గమ్యం_scaled_real(tmp[idx], src->dest->scope);
	    }
      }
}

స్థిర పూర్ణాంకం pathterm_get(పూర్ణాంకం code, vpiHandle ref)
{
      నిర్మాణం __vpiModPathTerm*obj = dynamic_cast<__vpiModPathTerm*>(ref);
      ప్రతిజ్ఞా(obj);

      ఎంపిక (code) {
	  స్థితి vpi_అంచు_2:
	    తిరిగి obj->అంచు;
	  అప్రమేయ:
	    తిరిగి 0;
      }
}

స్థిర vpiHandle pathterm_get_handle(పూర్ణాంకం code, vpiHandle ref)
{
      నిర్మాణం __vpiModPathTerm*obj = dynamic_cast<__vpiModPathTerm*>(ref);
      ప్రతిజ్ఞా(obj);

      ఎంపిక (code) {
	  స్థితి vpi_సమాసం:
	    తిరిగి obj->సమాసం;
	  అప్రమేయ:
	    తిరిగి 0;
      }
}

/*
* The __vpiModPathSrc class is what the VPI client sees as a
* vpiModPath object. The __vpiModPath structure contains items that
* are common to a bunch of modpaths, including the destination term.
*/
అంతఃవరుస __vpiModPathSrc::__vpiModPathSrc()
{ }

పూర్ణాంకం __vpiModPathSrc::get_type_సంకేతం(శూన్య) స్థిరరాశి
{ తిరిగి vpi_Mod_మార్గం; }

పూర్ణాంకం __vpiModPathSrc::vpi_get(పూర్ణాంకం code)
{ తిరిగి modpath_src_get(code, ఇది); }

శూన్య __vpiModPathSrc::vpi_get_విలువ(p_vpi_విలువ val)
{ modpath_src_get_విలువ(ఇది, val); }

vpiHandle __vpiModPathSrc::vpi_put_విలువ(p_vpi_విలువ val, పూర్ణాంకం flags)
{ తిరిగి modpath_src_put_విలువ(ఇది, val, flags); }

vpiHandle __vpiModPathSrc::vpi_handle(పూర్ణాంకం code)
{ తిరిగి modpath_src_get_handle(code, ఇది); }

vpiHandle __vpiModPathSrc::vpi_iterate(పూర్ణాంకం code)
{ తిరిగి modpath_src_iterate(code, ఇది); }

vpiHandle __vpiModPathSrc:: vpi_సూచీ(పూర్ణాంకం idx)
{ తిరిగి modpath_src_సూచీ(ఇది, idx); }

శూన్య __vpiModPathSrc::vpi_get_delays(p_vpi_delay del)
{ modpath_src_get_delays(ఇది, del); }

శూన్య __vpiModPathSrc::vpi_put_delays(p_vpi_delay del)
{ modpath_src_put_delays(ఇది, del); }

స్థిర పూర్ణాంకం modpath_src_free_object( vpiHandle ref )
{
      తొలగించు ref;
      తిరిగి 1 ;
}

__vpiHandle::free_object_fun_t __vpiModPathSrc::free_object_fun(శూన్య)
{ తిరిగి &modpath_src_free_object; }


అంతఃవరుస __vpiModPathTerm::__vpiModPathTerm()
{ }

పూర్ణాంకం __vpiModPathTerm::get_type_సంకేతం(శూన్య) స్థిరరాశి
{ తిరిగి vpi_మార్గం_పదం; }

పూర్ణాంకం __vpiModPathTerm::vpi_get(పూర్ణాంకం code)
{ తిరిగి pathterm_get(code, ఇది); }

vpiHandle __vpiModPathTerm::vpi_handle(పూర్ణాంకం code)
{ తిరిగి pathterm_get_handle(code, ఇది); }

స్థిర శూన్య initialize_మార్గం_పదం(నిర్మాణం __vpiModPathTerm&obj)
{
      obj.సమాసం = 0;
      obj.అంచు = vpi_No_అంచు;
}

/*
 * This function will construct a vpiModPath Object.
 * give a respective "net", and will point to his
 * respective functor
 */

#నిర్వచితమైతే CHECK_WITH_VALGRIND
స్థిర నిర్మాణం __vpiModPath**mp_list = 0;
స్థిర గుర్తులేని mp_లెక్క = 0;
#అయితే_ముగింపు

నిర్మాణం __vpiModPath* vpip_make_modpath(vvp_net_t *net)
{
      నిర్మాణం __vpiModPath*obj = కొత్త __vpiModPath;
      obj->scope = vpip_peek_current_scope ( );

      initialize_మార్గం_పదం(obj->మార్గం_పదం_బహిర్గతం);
      obj->ఆదానం_net = net ;

#నిర్వచితమైతే CHECK_WITH_VALGRIND
      mp_లెక్క += 1;
      mp_list = static_cast<నిర్మాణం __vpiModPath **>
                (realloc(mp_list,
                 mp_లెక్క*పరిమాణం(నిర్మాణం __vpiModPath **)));
      mp_list[mp_లెక్క-1] = obj;
#అయితే_ముగింపు
      తిరిగి obj;
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
శూన్య modpath_delete()
{
      కొరకు (గుర్తులేని idx = 0; idx < mp_లెక్క; idx += 1) {
	    తొలగించు mp_list[idx];
      }
      విడుదల(mp_list);
      mp_list = 0;
      mp_లెక్క = 0;
}
#అయితే_ముగింపు

/*
 * This function will construct a vpiModPathIn
 * ( struct __vpiModPathSrc ) Object. will give
 * a delays[12] values, and point to the specified functor
 *
 */

నిర్మాణం __vpiModPathSrc* vpip_make_modpath_src(నిర్మాణం __vpiModPath*మార్గం,
                                              vvp_net_t *net)
{
      నిర్మాణం __vpiModPathSrc *obj = కొత్త __vpiModPathSrc;

      obj->dest = మార్గం;
      obj->type = 0;
      obj->net = net;
      initialize_మార్గం_పదం(obj->మార్గం_పదం_లోపల);

      తిరిగి obj;
}

vvp_fun_intermodpath::vvp_fun_intermodpath(vvp_net_t*net, గుర్తులేని width)
: net_(net)
{
      కొరకు (గుర్తులేని idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = 0;

      cur_vec4_ = vvp_vector4_t(width, BIT4_X);
      కాలపట్టిక_init_propagate(net_, cur_vec4_);
}

vvp_fun_intermodpath::~vvp_fun_intermodpath()
{
}

శూన్య vvp_fun_intermodpath::get_delay12(vvp_time64_t val[12]) స్థిరరాశి
{
      కొరకు (గుర్తులేని idx = 0 ;  idx < 12 ;  idx += 1)
	    val[idx] = delay_[idx];
}

శూన్య vvp_fun_intermodpath::put_delay12(స్థిరరాశి vvp_time64_t val[12])
{
      కొరకు (గుర్తులేని idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = val[idx];
}

/*
 * FIXME: this implementation currently only uses the maximum delay
 * from all the bit changes in the vectors. If there are multiple
 * changes with different delays, then the results would be
 * wrong. What should happen is that if there are multiple changes,
 * multiple vectors approaching the result should be scheduled.
 */
శూన్య vvp_fun_intermodpath::recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                              vvp_context_t)
{
	/* Only the first port is used. */
      ఒకవేళ (పోర్టు.పోర్టు() > 0)
	    తిరిగి;

      ఒకవేళ (cur_vec4_.eeq(బిట్టు))
	    తిరిగి;

	/* Given the scheduled output time, create an output event. */
      vvp_time64_t use_delay = delay_from_అంచు(cur_vec4_.value(0),
					       బిట్టు.value(0),
					       delay_);

	/* FIXME: This bases the edge delay on only the least
	   bit. This is WRONG! I need to find all the possible delays,
	   and schedule an event for each partial change. Hard! */
      కొరకు (గుర్తులేని idx = 1 ;  idx < బిట్టు.size() ;  idx += 1) {
	    vvp_time64_t tmp = delay_from_అంచు(cur_vec4_.value(idx),
					       బిట్టు.value(idx),
					       delay_);
	      /* If the current and new bit values match then no delay
	       * is needed for this bit. */
	    ఒకవేళ (cur_vec4_.value(idx) == బిట్టు.value(idx)) కొనసాగు;
	    ప్రతిజ్ఞా(tmp == use_delay);
      }

      cur_vec4_ = బిట్టు;
      కాలపట్టిక_generic(ఇది, use_delay, అబద్ధం);
}

శూన్య vvp_fun_intermodpath::నిరంతరం_నిరంతరం()
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
స్థిర పూర్ణాంకం intermodpath_get(పూర్ణాంకం, vpiHandle ref)
{
      నిర్మాణం __vpiInterModPath*obj =dynamic_cast<__vpiInterModPath*>(ref);
      ప్రతిజ్ఞా(obj);
      తిరిగి 0;
}

స్థిర శూన్య intermodpath_get_విలువ(vpiHandle ref, p_vpi_విలువ)
{
      నిర్మాణం __vpiInterModPath* intermodpath = dynamic_cast<__vpiInterModPath*>(ref);
      ప్రతిజ్ఞా(intermodpath);
      తిరిగి;
}

స్థిర vpiHandle intermodpath_put_విలువ(vpiHandle ref, s_vpi_విలువ *, పూర్ణాంకం )
{
      నిర్మాణం __vpiInterModPath* intermodpath = dynamic_cast<__vpiInterModPath*>(ref);
      ప్రతిజ్ఞా(intermodpath);
      తిరిగి 0;
}

స్థిర vpiHandle intermodpath_get_handle(పూర్ణాంకం code, vpiHandle ref)
{
      నిర్మాణం __vpiInterModPath*rfp = dynamic_cast<__vpiInterModPath*>(ref);
      ప్రతిజ్ఞా(rfp);

      ఎంపిక (code) {

	స్థితి vpiScope:
	  తిరిగి rfp->scope;

	  స్థితి vpiModule:
	      { __vpiScope*scope = rfp->scope;
		అయితే (scope && scope->get_type_సంకేతం() != vpiModule)
		      scope = scope->scope;
		ప్రతిజ్ఞా(scope);
		తిరిగి scope;
	      }
      }
      తిరిగి 0;
}

స్థిర vpiHandle intermodpath_iterate(పూర్ణాంకం code, vpiHandle ref)
{
      నిర్మాణం __vpiInterModPath*rfp = dynamic_cast<__vpiInterModPath*>(ref);
      ప్రతిజ్ఞా(rfp);

	// For now intermodpaths only support exactly two ports
      ఎంపిక (code) {
	  స్థితి vpi_పోర్టు: {
	    vpiHandle*వాదనలు = static_cast<vpiHandle*>(సున్నా_జ్ఞాపకం_ఇవ్వు(2, పరిమాణం(vpiHandle*)));
	    వాదనలు[0] = rfp->port1;
	    వాదనలు[1] = rfp->port2;
	    తిరిగి vpip_make_iterator(2, వాదనలు, నిజం);
	  }
      }
      తిరిగి 0;
}

/*
 * This routine will put specific dimension of delay[] values
 * into a vpiHandle. In this case, we will put
 * specific delays values in a vpiInterModPath object
 *
 */
స్థిర శూన్య intermodpath_put_delays (vpiHandle ref, p_vpi_delay delays)
{
      vvp_time64_t tmp[12];
      పూర్ణాంకం idx;
      నిర్మాణం __vpiInterModPath * src = dynamic_cast<__vpiInterModPath*>(ref) ;
      ప్రతిజ్ఞా(src) ;

      vvp_fun_intermodpath *fun = dynamic_cast<vvp_fun_intermodpath*>(src->net->fun);
      ప్రతిజ్ఞా( fun );

      రకనిర్వచనం గుర్తులేని అక్షరం map_array_t[12];
	// Only the first six entries are used for the less than twelve maps.
      స్థిర స్థిరరాశి map_array_t map_1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      స్థిర స్థిరరాశి map_array_t map_2 = {0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0};
      స్థిర స్థిరరాశి map_array_t map_3 = {0, 1, 2, 0, 2, 1, 0, 0, 0, 0, 0, 0};
      స్థిర స్థిరరాశి map_array_t map_6 = {0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0};
      స్థిర స్థిరరాశి map_array_t map12 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

      స్థిరరాశి map_array_t*use_map = 0;
      ఎంపిక (delays->no_యొక్క_delays) {
	  స్థితి 1:
	    use_map = &map_1;
	    ఆపు;
	  స్థితి 2:
	    use_map = &map_2;
	    ఆపు;
	  స్థితి 3:
	    use_map = &map_3;
	    ఆపు;
	  స్థితి 6:
	    use_map = &map_6;
	    ఆపు;
	  స్థితి 12:
	    use_map = &map12;
	    ఆపు;
	  అప్రమేయ:
	    ప్రతిజ్ఞా(0);
	    ఆపు;
      }

      ఒకవేళ (delays->సమయం_type == vpi_Sim_సమయం) {
	    కొరకు (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_timestruct_గమ్యం_సమయం(delays->da+use_map[0][idx]);
	    }
      } లేకపోతే {
	      // You cannot create a modpath with a negative delay so set it
	      // to zero per 1364-2005 section 14.3.1.
	    కొరకు (idx = 0 ; idx < delays->no_యొక్క_delays ; idx += 1) {
		  ఒకవేళ (delays->da[idx].real < 0.0) delays->da[idx].real = 0.0;
	    }
	    కొరకు (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_scaled_real_గమ్యం_time64(delays->da[use_map[0][idx]].real,
							src->scope);
	    }
      }

      /* Now define the to-from-x delays if needed. */
      ఒకవేళ (delays->no_యొక్క_delays <= 6) {
	      /* 0->x is the minimum of 0->z and 0->1. */
	    tmp[DELAY_అంచు_0x] = tmp[DELAY_అంచు_0z] < tmp[DELAY_అంచు_01] ?
	                           tmp[DELAY_అంచు_0z] : tmp[DELAY_అంచు_01];
	      /* x->1 is the maximum of z->1 and 0->1. */
	    tmp[DELAY_అంచు_x1] = tmp[DELAY_అంచు_z1] > tmp[DELAY_అంచు_01] ?
	                           tmp[DELAY_అంచు_z1] : tmp[DELAY_అంచు_01];
	      /* 1->x is the minimum of 1->z and 1->0. */
	    tmp[DELAY_అంచు_1x] = tmp[DELAY_అంచు_1z] < tmp[DELAY_అంచు_10] ?
	                           tmp[DELAY_అంచు_1z] : tmp[DELAY_అంచు_10];
	      /* x->0 is the maximum of z->0 and 1->0. */
	    tmp[DELAY_అంచు_x0] = tmp[DELAY_అంచు_z0] > tmp[DELAY_అంచు_10] ?
	                           tmp[DELAY_అంచు_z0] : tmp[DELAY_అంచు_10];
	      /* x->z is the maximum of 1->z and 0->z. */
	    tmp[DELAY_అంచు_xz] = tmp[DELAY_అంచు_1z] > tmp[DELAY_అంచు_0z] ?
	                           tmp[DELAY_అంచు_1z] : tmp[DELAY_అంచు_0z];
	      /* z->x is the minimum of z->1 and z->0. */
	    tmp[DELAY_అంచు_zx] = tmp[DELAY_అంచు_z1] < tmp[DELAY_అంచు_z0] ?
	                           tmp[DELAY_అంచు_z1] : tmp[DELAY_అంచు_z0];
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

స్థిర శూన్య intermodpath_get_delays ( vpiHandle ref, p_vpi_delay delays )
{
      నిర్మాణం __vpiInterModPath*src = dynamic_cast<__vpiInterModPath*>(ref) ;
      ప్రతిజ్ఞా(src);

      vvp_fun_intermodpath *fun = dynamic_cast<vvp_fun_intermodpath*>(src->net->fun);
      ప్రతిజ్ఞా(fun);

      పూర్ణాంకం idx;
      vvp_time64_t tmp[12];
      fun->get_delay12(tmp);

      ఎంపిక (delays->no_యొక్క_delays) {
	  స్థితి 1:
	  స్థితి 2:
	  స్థితి 3:
	  స్థితి 6:
	  స్థితి 12:
	    ఆపు;

	  అప్రమేయ:
	    ప్రతిజ్ఞా(0);
	    ఆపు;
      }

      ఒకవేళ (delays->సమయం_type == vpi_Sim_సమయం) {
	    కొరకు (idx = 0; idx < delays->no_యొక్క_delays; idx += 1) {
		  vpip_సమయం_గమ్యం_timestruct(delays->da+idx, tmp[idx]);
	    }
      } లేకపోతే {
	    కొరకు (idx = 0; idx < delays->no_యొక్క_delays; idx += 1) {
		  delays->da[idx].real = vpip_సమయం_గమ్యం_scaled_real(tmp[idx], src->scope);
	    }
      }
}

/*
* The __vpiInterModPath class is what the VPI client sees as a
* vpiInterModPath object.
*/
అంతఃవరుస __vpiInterModPath::__vpiInterModPath()
{ }

పూర్ణాంకం __vpiInterModPath::get_type_సంకేతం(శూన్య) స్థిరరాశి
{ తిరిగి vpi_Inter_Mod_మార్గం; }

పూర్ణాంకం __vpiInterModPath::vpi_get(పూర్ణాంకం code)
{ తిరిగి intermodpath_get(code, ఇది); }

శూన్య __vpiInterModPath::vpi_get_విలువ(p_vpi_విలువ val)
{ intermodpath_get_విలువ(ఇది, val); }

vpiHandle __vpiInterModPath::vpi_put_విలువ(p_vpi_విలువ val, పూర్ణాంకం flags)
{ తిరిగి intermodpath_put_విలువ(ఇది, val, flags); }

vpiHandle __vpiInterModPath::vpi_handle(పూర్ణాంకం code)
{ తిరిగి intermodpath_get_handle(code, ఇది); }

vpiHandle __vpiInterModPath::vpi_iterate(పూర్ణాంకం code)
{ తిరిగి intermodpath_iterate(code, ఇది); }

శూన్య __vpiInterModPath::vpi_get_delays(p_vpi_delay del)
{ intermodpath_get_delays(ఇది, del); }

శూన్య __vpiInterModPath::vpi_put_delays(p_vpi_delay del)
{ intermodpath_put_delays(ఇది, del); }

స్థిర పూర్ణాంకం intermodpath_free_object( vpiHandle ref )
{
      తొలగించు ref;
      తిరిగి 1 ;
}

__vpiHandle::free_object_fun_t __vpiInterModPath::free_object_fun(శూన్య)
{ తిరిగి &intermodpath_free_object; }

/*
 * This function will construct a vpiInterModPath Object.
 * give a respective "net", and will point to his
 * respective functor
 */

#నిర్వచితమైతే CHECK_WITH_VALGRIND
స్థిర నిర్మాణం __vpiInterModPath**imp_list = 0;
స్థిర గుర్తులేని imp_లెక్క = 0;
#అయితే_ముగింపు

నిర్మాణం __vpiInterModPath* vpip_make_intermodpath(vvp_net_t *net, vpi_పోర్టు_Info* port1, vpi_పోర్టు_Info* port2)
{
      నిర్మాణం __vpiInterModPath*obj = కొత్త __vpiInterModPath;
      obj->scope = vpip_peek_current_scope ( );

      obj->net = net;
      obj->port1 = port1;
      obj->port2 = port2;

#నిర్వచితమైతే CHECK_WITH_VALGRIND
      imp_లెక్క += 1;
      imp_list = static_cast<నిర్మాణం __vpiInterModPath **>
                 (realloc(imp_list,
                  imp_లెక్క*పరిమాణం(నిర్మాణం __vpiInterModPath **)));
      imp_list[imp_లెక్క-1] = obj;
#అయితే_ముగింపు
      తిరిగి obj;
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
శూన్య intermodpath_delete()
{
      కొరకు (గుర్తులేని idx = 0; idx < mp_లెక్క; idx += 1) {
	    తొలగించు imp_list[idx];
      }
      విడుదల(imp_list);
      imp_list = 0;
      imp_లెక్క = 0;
}
#అయితే_ముగింపు
