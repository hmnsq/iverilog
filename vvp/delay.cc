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
#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
#ضمن "vvp_cleanup.h"
#جيڪڏهن_پورو
#ضمن <iostream>
#ضمن <cstdlib>
#ضمن <list>
#ضمن <cassert>
#ضمن <cmath>
#ضمن "ivl_alloc.h"

استعمال نطاق std;

فراغ vvp_delay_t::calculate_min_delay_()
{
      min_delay_ = rise_;
      إذا (fall_ < min_delay_)
	    min_delay_ = fall_;
      إذا (ignore_decay_) decay_ = min_delay_;
      وإلا إذا (decay_ < min_delay_)
	    min_delay_ = decay_;
}

vvp_delay_t::vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall)
{
      rise_ = rise;
      fall_ = fall;
      decay_= fall < rise? fall : rise;
      min_delay_ = decay_;
      ignore_decay_ = خطأ;
}

vvp_delay_t::vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall, vvp_time64_t decay)
{
      rise_ = rise;
      fall_ = fall;
      decay_= decay;
      ignore_decay_ = خطأ;

      calculate_min_delay_();
}

فراغ vvp_delay_t::سيٽ_ignore_decay()
{
      ignore_decay_ = سچ;

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
		حالة BIT4_0: أرجع 0;
		حالة BIT4_1: أرجع rise_;
		حالة BIT4_X: أرجع min_delay_;
		حالة BIT4_Z: أرجع decay_;
	    }
	    اكسر;
	  حالة BIT4_1:
	    بدل (to) {
		حالة BIT4_0: أرجع fall_;
		حالة BIT4_1: أرجع 0;
		حالة BIT4_X: أرجع min_delay_;
		حالة BIT4_Z: أرجع decay_;
	    }
	    اكسر;
	  حالة BIT4_X:
	    بدل (to) {
		حالة BIT4_0: أرجع fall_;
		حالة BIT4_1: أرجع rise_;
		حالة BIT4_X: أرجع 0;
		حالة BIT4_Z: أرجع decay_;
	    }
	    اكسر;
	  حالة BIT4_Z:
	    بدل (to) {
		حالة BIT4_0: أرجع fall_;
		حالة BIT4_1: أرجع rise_;
		حالة BIT4_X: أرجع min_delay_;
		حالة BIT4_Z: أرجع 0;
	    }
	    اكسر;
      }

      أكد(0);
      أرجع 0;
}

vvp_time64_t vvp_delay_t::get_min_delay() ثابت
{
      أرجع min_delay_;
}

فراغ vvp_delay_t::سيٽ_rise(vvp_time64_t val)
{
      rise_ = val;
      إذا (val < min_delay_) {
	    min_delay_ = val;
	    إذا (ignore_decay_) decay_ = val;
      } وإلا
	    calculate_min_delay_();
}

فراغ vvp_delay_t::سيٽ_fall(vvp_time64_t val)
{
      fall_ = val;
      إذا (val < min_delay_) {
	    min_delay_ = val;
	    إذا (ignore_decay_) decay_ = val;
      } وإلا
	    calculate_min_delay_();
}

فراغ vvp_delay_t::سيٽ_decay(vvp_time64_t val)
{
      أكد(!ignore_decay_);

      decay_ = val;
      إذا (val < min_delay_)
	    min_delay_ = val;
      وإلا
	    calculate_min_delay_();
}

vvp_fun_delay::vvp_fun_delay(vvp_net_t*n, بنانشان width, ثابت vvp_delay_t&d)
: net_(n), delay_(d)
{
      cur_real_ = 0.0;
      إذا (width > 0) {
            cur_vec4_ = vvp_vector4_t(width, BIT4_X);
            cur_vec8_ = vvp_vector8_t(cur_vec4_, 6, 6);
            وقت_جدول_init_propagate(net_, cur_vec4_);
      } وإلا {
            وقت_جدول_init_propagate(net_, cur_real_);
      }
      list_ = 0;
      type_ = UNKNOWN_DELAY;
      initial_ = سچ;
	// Calculate the values used when converting variable delays
	// to simulation time units.
      ثابت __vpiScope*scope = vpip_peek_current_scope();

      صحيح powr = scope->وقت_units - scope->وقت_precision;
      ڦيرو = 1;
      لكل (صحيح lp = 0; lp < powr; lp += 1) ڦيرو *= 10;

      powr = scope->وقت_precision - vpip_get_وقت_precision();
      scale_ = 1;
      لكل (صحيح lp = 0; lp < powr; lp += 1) scale_ *= 10;
}

vvp_fun_delay::~vvp_fun_delay()
{
      طالما (بنية event_*cur = قطار_مان_ڪڍو())
	    ميٽيو cur;
}

منطقي vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        ثابت vvp_vector4_t&ٻٽو_انگ)
{
      إذا (list_ == 0) أرجع خطأ;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      إذا (list_->ايندڙ_2->ptr_vec4.eeq(ٻٽو_انگ)) أرجع سچ;

      clean_pulse_events_(use_delay);
      أرجع خطأ;
}

منطقي vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        ثابت vvp_vector8_t&ٻٽو_انگ)
{
      إذا (list_ == 0) أرجع خطأ;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      إذا (list_->ايندڙ_2->ptr_vec8.eeq(ٻٽو_انگ)) أرجع سچ;

      clean_pulse_events_(use_delay);
      أرجع خطأ;
}

منطقي vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        مزدوج ٻٽو_انگ)
{
      إذا (list_ == 0) أرجع خطأ;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      إذا (list_->ايندڙ_2->ptr_real == ٻٽو_انگ) أرجع سچ;

      clean_pulse_events_(use_delay);
      أرجع خطأ;
}

فراغ vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay)
{
      أكد(list_ != 0);

      افعل {
	    بنية event_*cur = list_->ايندڙ_2;
	      /* If this event is far enough from the event I'm about
	         to create, then that scheduled event is not a pulse
	         to be eliminated, so we're done. */
	    إذا (cur->sim_وقت+use_delay <= use_delay+وقت_جدول_simtime())
		  اكسر;

	    إذا (list_ == cur)
		  list_ = 0;
	    وإلا
		  list_->ايندڙ_2 = cur->ايندڙ_2;
	    ميٽيو cur;
      } طالما (list_);
}

/*
 * FIXME: this implementation currently only uses the maximum delay
 * from all the bit changes in the vectors. If there are multiple
 * changes with different delays, then the results would be
 * wrong. What should happen is that if there are multiple changes,
 * multiple vectors approaching the result should be scheduled.
 */
فراغ vvp_fun_delay::recv_vec4(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
                              vvp_context_t)
{
      إذا (پورٽ.پورٽ() > 0) {
	      // Get the integer value of the bit vector, or 0 if
	      // there are X or Z bits.
	    vvp_time64_t bval = 0;
	      // The following does not work correctly for negative values.
	      // They should be sign extended to 64 bits (1364-2001 9.7.1).
	    vector4_منزل_قدر(ٻٽو_انگ, bval);
	      // Integer values do not need to be rounded so just scale them.
	    vvp_time64_t val = bval * ڦيرو * scale_;

	    بدل (پورٽ.پورٽ()) {
		حالة 1:
		  delay_.سيٽ_rise(val);
		  أرجع;
		حالة 2:
		  delay_.سيٽ_fall(val);
		  أرجع;
		حالة 3:
		  delay_.سيٽ_decay(val);
		  أرجع;
	    }
	    أرجع;
      }

      vvp_time64_t use_delay;
	/* This is an initial value so it needs to be compared to all the
	   bits (the order the bits are changed is not deterministic). */
      إذا (initial_) {
	    type_ = VEC4_DELAY;
            cur_vec8_ = vvp_vector8_t(vvp_vector4_t(0, BIT4_X), 6, 6);
	    vvp_bit4_t cur_val = cur_vec4_.value(0);
	    use_delay = delay_.get_delay(cur_val, ٻٽو_انگ.value(0));
	    لكل (بنانشان idx = 1 ;  idx < ٻٽو_انگ.size() ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(cur_val, ٻٽو_انگ.value(idx));
		  إذا (tmp > use_delay) use_delay = tmp;
	    }
      } وإلا {
	    أكد(type_ == VEC4_DELAY);

	      // Use as a reference for calculating the delay the
	      // current value of the output. Detect and handle the
	      // special case that the event list contains the current
	      // value as a zero-delay-remaining event.
	    ثابت vvp_vector4_t&use_vec4 = (list_ && list_->ايندڙ_2->sim_وقت == وقت_جدول_simtime())? list_->ايندڙ_2->ptr_vec4 : cur_vec4_;

	      /* How many bits to compare? */
	    بنانشان use_wid = use_vec4.size();
	    إذا (ٻٽو_انگ.size() < use_wid) use_wid = ٻٽو_انگ.size();

	      /* Scan the vectors looking for delays. Select the maximum
	         delay encountered. */
	    use_delay = delay_.get_delay(use_vec4.value(0), ٻٽو_انگ.value(0));

	    لكل (بنانشان idx = 1 ;  idx < use_wid ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(use_vec4.value(idx), ٻٽو_انگ.value(idx));
		  إذا (tmp > use_delay) use_delay = tmp;
	    }
      }

      /* what *should* happen here is we check to see if there is a
         transaction in the queue. This would be a pulse that needs to be
         eliminated. */
      إذا (clean_pulse_events_(use_delay, ٻٽو_انگ)) أرجع;

      vvp_time64_t use_simtime = وقت_جدول_simtime() + use_delay;

	/* And propagate it. */
      إذا (use_delay == 0 && list_ == 0) {
	    cur_vec4_ = ٻٽو_انگ;
	    initial_ = خطأ;
	    net_->send_vec4(cur_vec4_, 0);
      } وإلا {
	    بنية event_*cur = جديد بنية event_(use_simtime);
	    cur->لاڳيتو_لاڳيتو_ptr = &vvp_fun_delay::لاڳيتو_لاڳيتو_vec4;
	    cur->ptr_vec4 = ٻٽو_انگ;
	    قطار_اندر_وجهو(cur);
	    وقت_جدول_generic(هذا, use_delay, خطأ);
      }
}

فراغ vvp_fun_delay::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ٻٽو_انگ,
			         بنانشان بنياد, بنانشان vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ٻٽو_انگ, بنياد, vwid, ctx);
}

/* See the recv_vec4 comment above. */
فراغ vvp_fun_delay::recv_vec8(vvp_net_ptr_t پورٽ, ثابت vvp_vector8_t&ٻٽو_انگ)
{
      أكد(پورٽ.پورٽ() == 0);

      vvp_time64_t use_delay;
	/* This is an initial value so it needs to be compared to all the
	   bits (the order the bits are changed is not deterministic). */
      إذا (initial_) {
	    type_ = VEC8_DELAY;
            cur_vec4_ = vvp_vector4_t(0, BIT4_X);
	    vvp_bit4_t cur_val = cur_vec8_.value(0).value();
	    use_delay = delay_.get_delay(cur_val, ٻٽو_انگ.value(0).value());
	    لكل (بنانشان idx = 1 ;  idx < ٻٽو_انگ.size() ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(cur_val, ٻٽو_انگ.value(idx).value());
		  إذا (tmp > use_delay) use_delay = tmp;
	    }
      } وإلا {
	    أكد(type_ == VEC8_DELAY);

	      // Use as a reference for calculating the delay the
	      // current value of the output. Detect and handle the
	      // special case that the event list contains the current
	      // value as a zero-delay-remaining event.
	    ثابت vvp_vector8_t&use_vec8 = (list_ && list_->ايندڙ_2->sim_وقت == وقت_جدول_simtime())? list_->ايندڙ_2->ptr_vec8 : cur_vec8_;

	      /* How many bits to compare? */
	    بنانشان use_wid = use_vec8.size();
	    إذا (ٻٽو_انگ.size() < use_wid) use_wid = ٻٽو_انگ.size();

	      /* Scan the vectors looking for delays. Select the maximum
	         delay encountered. */
	    use_delay = delay_.get_delay(use_vec8.value(0).value(),
	                                 ٻٽو_انگ.value(0).value());

	    لكل (بنانشان idx = 1 ;  idx < use_wid ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(use_vec8.value(idx).value(),
		                         ٻٽو_انگ.value(idx).value());
		  إذا (tmp > use_delay) use_delay = tmp;
	    }
      }

      /* what *should* happen here is we check to see if there is a
         transaction in the queue. This would be a pulse that needs to be
         eliminated. */
      إذا (clean_pulse_events_(use_delay, ٻٽو_انگ)) أرجع;

      vvp_time64_t use_simtime = وقت_جدول_simtime() + use_delay;

	/* And propagate it. */
      إذا (use_delay == 0 && list_ == 0) {
	    cur_vec8_ = ٻٽو_انگ;
	    initial_ = خطأ;
	    net_->send_vec8(cur_vec8_);
      } وإلا {
	    بنية event_*cur = جديد بنية event_(use_simtime);
	    cur->ptr_vec8 = ٻٽو_انگ;
	    cur->لاڳيتو_لاڳيتو_ptr = &vvp_fun_delay::لاڳيتو_لاڳيتو_vec8;
	    قطار_اندر_وجهو(cur);
	    وقت_جدول_generic(هذا, use_delay, خطأ);
      }
}

فراغ vvp_fun_delay::recv_vec8_pv(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&ٻٽو_انگ,
			         بنانشان بنياد, بنانشان vwid)
{
      recv_vec8_pv_(ptr, ٻٽو_انگ, بنياد, vwid);
}

فراغ vvp_fun_delay::recv_real(vvp_net_ptr_t پورٽ, مزدوج ٻٽو_انگ,
                              vvp_context_t)
{
      إذا (پورٽ.پورٽ() > 0) {
	    /* If the port is not 0, then this is a delay value that
	    should be rounded and converted to an integer delay. */
	    vvp_time64_t val = 0;
	    إذا (ٻٽو_انگ > -0.5) {
		  val = (vvp_time64_t) (ٻٽو_انگ * ڦيرو + 0.5) * scale_;
	    } وإلا إذا (ٻٽو_انگ != ٻٽو_انگ) {
		    // For a NaN we use the default (0).
	    } وإلا {
		  vvp_vector4_t vec4(8*حجم(vvp_time64_t),
		                     floor(-ٻٽو_انگ * ڦيرو + 0.5) * -1 * scale_);
		  vector4_منزل_قدر(vec4, val);
	    }

	    بدل (پورٽ.پورٽ()) {
		حالة 1:
		  delay_.سيٽ_rise(val);
		  أرجع;
		حالة 2:
		  delay_.سيٽ_fall(val);
		  أرجع;
		حالة 3:
		  delay_.سيٽ_decay(val);
		  أرجع;
	    }
	    أرجع;
      }

      إذا (initial_) {
	    type_ = REAL_DELAY;
            cur_vec4_ = vvp_vector4_t(0, BIT4_X);
            cur_vec8_ = vvp_vector8_t(cur_vec4_, 6, 6);
      } وإلا أكد(type_ == REAL_DELAY);

      vvp_time64_t use_delay;
      use_delay = delay_.get_min_delay();

      /* Eliminate glitches. */
      إذا (clean_pulse_events_(use_delay, ٻٽو_انگ)) أرجع;

      /* This must be done after cleaning pulses to avoid propagating
       * an incorrect value. */
      إذا (cur_real_ == ٻٽو_انگ) أرجع;

      vvp_time64_t use_simtime = وقت_جدول_simtime() + use_delay;

      إذا (use_delay == 0 && list_ == 0) {
	    cur_real_ = ٻٽو_انگ;
	    initial_ = خطأ;
	    net_->send_real(cur_real_, 0);
      } وإلا {
	    بنية event_*cur = جديد بنية event_(use_simtime);
	    cur->لاڳيتو_لاڳيتو_ptr = &vvp_fun_delay::لاڳيتو_لاڳيتو_real;
	    cur->ptr_real = ٻٽو_انگ;
	    قطار_اندر_وجهو(cur);

	    وقت_جدول_generic(هذا, use_delay, خطأ);
      }
}

فراغ vvp_fun_delay::لاڳيتو_لاڳيتو()
{
      vvp_time64_t sim_وقت = وقت_جدول_simtime();
      إذا (list_ == 0 || list_->ايندڙ_2->sim_وقت > sim_وقت)
	    أرجع;

      بنية event_*cur = قطار_مان_ڪڍو();
      إذا (cur == 0)
	    أرجع;

      (هذا->*(cur->لاڳيتو_لاڳيتو_ptr))(cur);
      initial_ = خطأ;
      ميٽيو cur;
}

فراغ vvp_fun_delay::لاڳيتو_لاڳيتو_vec4(ثابت بنية event_*cur)
{
      cur_vec4_ = cur->ptr_vec4;
      net_->send_vec4(cur_vec4_, 0);
}

فراغ vvp_fun_delay::لاڳيتو_لاڳيتو_vec8(ثابت بنية vvp_fun_delay::event_*cur)
{
      cur_vec8_ = cur->ptr_vec8;
      net_->send_vec8(cur_vec8_);
}

فراغ vvp_fun_delay::لاڳيتو_لاڳيتو_real(ثابت بنية vvp_fun_delay::event_*cur)
{
      cur_real_ = cur->ptr_real;
      net_->send_real(cur_real_, 0);
}

vvp_fun_modpath::vvp_fun_modpath(vvp_net_t*net, بنانشان width)
: net_(net), src_list_(0), ifnone_list_(0)
{
      cur_vec4_ = vvp_vector4_t(width, BIT4_X);
      وقت_جدول_init_propagate(net_, cur_vec4_);
}

vvp_fun_modpath::~vvp_fun_modpath()
{
	// Delete the source probes.
      طالما (src_list_) {
	    vvp_fun_modpath_src*tmp = src_list_;
	    src_list_ = tmp->ايندڙ;
	    ميٽيو tmp;
      }
      طالما (ifnone_list_) {
	    vvp_fun_modpath_src*tmp = ifnone_list_;
	    ifnone_list_ = tmp->ايندڙ;
	    ميٽيو tmp;
      }
}

فراغ vvp_fun_modpath::جوڙيو_modpath_src(vvp_fun_modpath_src*that, منطقي ifnone)
{
      أكد(that->ايندڙ == 0);
      إذا (ifnone) {
	    that->ايندڙ = ifnone_list_;
	    ifnone_list_ = that;
      } وإلا {
	    that->ايندڙ = src_list_;
	    src_list_ = that;
      }
}

ساكن vvp_time64_t delay_from_ڪنارو(vvp_bit4_t a, vvp_bit4_t b,
                                           vvp_time64_t ثابت array[12])
{
      تعريفنوع delay_ڪنارو_t bit4_table4[4];
      ساكن ثابت bit4_table4 ڪنارو_جدول[4] = {
	    { DELAY_ڪنارو_01, DELAY_ڪنارو_01, DELAY_ڪنارو_0z, DELAY_ڪنارو_0x },
	    { DELAY_ڪنارو_10, DELAY_ڪنارو_10, DELAY_ڪنارو_1z, DELAY_ڪنارو_1x },
	    { DELAY_ڪنارو_z0, DELAY_ڪنارو_z1, DELAY_ڪنارو_z0, DELAY_ڪنارو_zx },
	    { DELAY_ڪنارو_x0, DELAY_ڪنارو_x1, DELAY_ڪنارو_xz, DELAY_ڪنارو_x0 }
      };

      أرجع array[ ڪنارو_جدول[a][b] ];
}

فراغ vvp_fun_modpath::recv_vec4(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
                                vvp_context_t)
{
	/* Only the first port is used. */
      إذا (پورٽ.پورٽ() > 0)
	    أرجع;

      إذا (cur_vec4_.eeq(ٻٽو_انگ))
	    أرجع;

	/* Select a time delay source that applies. Notice that there
	   may be multiple delay sources that apply, so collect all
	   the candidates into a list first. */
      list<vvp_fun_modpath_src*>candidate_list;
      vvp_time64_t candidate_wake_وقت = 0;
      لكل (vvp_fun_modpath_src*cur = src_list_ ;  cur ;  cur=cur->ايندڙ) {
	      /* Skip paths that are disabled by conditions. */
	    إذا (cur->condition_flag_ == خطأ)
		  استمر;

	    إذا (candidate_list.empty()) {
		  candidate_list.push_back(cur);
		  candidate_wake_وقت = cur->wake_وقت;
	    } وإلا إذا (cur->wake_وقت == candidate_wake_وقت) {
		  candidate_list.push_back(cur);
	    } وإلا إذا (cur->wake_وقت > candidate_wake_وقت) {
		  candidate_list.assign(1, cur);
		  candidate_wake_وقت = cur->wake_وقت;
	    } وإلا {
		  استمر; /* Skip this entry. */
	    }
      }

	/* Only add the ifnone delay if it has a later wake_time_ or
	 * if there are no normal delays. */
      vvp_time64_t ifnone_wake_وقت = candidate_wake_وقت;
      لكل (vvp_fun_modpath_src*cur = ifnone_list_ ;  cur ;  cur=cur->ايندڙ) {
	    إذا (candidate_list.empty()) {
		  candidate_list.push_back(cur);
		  ifnone_wake_وقت = cur->wake_وقت;
	    } وإلا إذا (cur->wake_وقت == ifnone_wake_وقت &&
	               ifnone_wake_وقت > candidate_wake_وقت) {
		  candidate_list.push_back(cur);
	    } وإلا إذا (cur->wake_وقت > ifnone_wake_وقت) {
		  candidate_list.assign(1, cur);
		  ifnone_wake_وقت = cur->wake_وقت;
	    } وإلا {
		  استمر; /* Skip this entry. */
	    }
      }

	/* Handle the special case that there are no delays that
	   match. This may happen, for example, if the set of
	   conditional delays is incomplete, leaving some cases
	   uncovered. In that case, just pass the data without delay */
      إذا (candidate_list.empty()) {
	    cur_vec4_ = ٻٽو_انگ;
	    وقت_جدول_generic(هذا, 0, خطأ);
	    أرجع;
      }

	/* Now given that we have a list of candidate delays, find for
	   each if the 12 numbers the minimum from all the
	   candidates. This minimum set becomes the chosen delay to
	   use. */
      vvp_time64_t ٻاهريون_at[12];
      vvp_time64_t now = وقت_جدول_simtime();

      تعريفنوع list<vvp_fun_modpath_src*>::const_iterator iter_t;

      iter_t cur = candidate_list.begin();
      ثابت vvp_fun_modpath_src*src = *cur;

      لكل (بنانشان idx = 0 ;  idx < 12 ;  idx += 1) {
	    ٻاهريون_at[idx] = src->wake_وقت + src->delay_[idx];
	    إذا (ٻاهريون_at[idx] <= now)
		  ٻاهريون_at[idx] = 0;
	    وإلا
		  ٻاهريون_at[idx] -= now;
      }

      لكل (++ cur ; cur != candidate_list.پڇاڙي() ; ++ cur ) {
	    src = *cur;
	    لكل (بنانشان idx = 0 ;  idx < 12 ;  idx += 1) {
		  vvp_time64_t tmp = src->wake_وقت + src->delay_[idx];
		  إذا (tmp <= now)
			tmp = 0;
		  وإلا
			tmp -= now;
		  إذا (tmp < ٻاهريون_at[idx])
			ٻاهريون_at[idx] = tmp;
	    }
      }

	/* Given the scheduled output time, create an output event. */
      vvp_time64_t use_delay = delay_from_ڪنارو(cur_vec4_.value(0),
					       ٻٽو_انگ.value(0),
					       ٻاهريون_at);

	/* FIXME: This bases the edge delay on only the least
	   bit. This is WRONG! I need to find all the possible delays,
	   and schedule an event for each partial change. Hard! */
      لكل (بنانشان idx = 1 ;  idx < ٻٽو_انگ.size() ;  idx += 1) {
	    vvp_time64_t tmp = delay_from_ڪنارو(cur_vec4_.value(idx),
					       ٻٽو_انگ.value(idx),
					       ٻاهريون_at);
	      /* If the current and new bit values match then no delay
	       * is needed for this bit. */
	    إذا (cur_vec4_.value(idx) == ٻٽو_انگ.value(idx)) استمر;
	    إذا (tmp != use_delay) {
		  fprintf(stderr, "sorry: multi-bit module path delays are "
		                  "currently not fully supported.\n");
		  fprintf(stderr, "     : using the LSB delay for all bits.\n");
		  استمر;
            }
      }

      cur_vec4_ = ٻٽو_انگ;
      وقت_جدول_generic(هذا, use_delay, خطأ);
}

فراغ vvp_fun_modpath::لاڳيتو_لاڳيتو()
{
      net_->send_vec4(cur_vec4_, 0);
}

vvp_fun_modpath_src::vvp_fun_modpath_src(vvp_time64_t ثابت del[12])
{
      لكل (بنانشان idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = del[idx];

      ايندڙ = 0;
      wake_وقت = 0;
      condition_flag_ = سچ;
}

vvp_fun_modpath_src::~vvp_fun_modpath_src()
{
}

فراغ vvp_fun_modpath_src::get_delay12(vvp_time64_t val[12]) ثابت
{
      لكل (بنانشان idx = 0 ;  idx < 12 ;  idx += 1)
	    val[idx] = delay_[idx];
}

فراغ vvp_fun_modpath_src::put_delay12(ثابت vvp_time64_t val[12])
{
      لكل (بنانشان idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = val[idx];
}

فراغ vvp_fun_modpath_src::recv_vec4(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
                                    vvp_context_t)
{
      إذا (پورٽ.پورٽ() == 0) {
	      // The modpath input...
	    إذا (test_vec4(ٻٽو_انگ))
		  wake_وقت = وقت_جدول_simtime();

      } وإلا إذا (پورٽ.پورٽ() == 1) {
	      // The modpath condition input...
	    إذا (ٻٽو_انگ.value(0) != BIT4_0)
		  condition_flag_ = سچ;
	    وإلا
		  condition_flag_ = خطأ;
      }
}

منطقي vvp_fun_modpath_src::test_vec4(ثابت vvp_vector4_t&)
{
      أرجع سچ;
}

vvp_fun_modpath_ڪنارو::vvp_fun_modpath_ڪنارو(vvp_time64_t del[12],
					   منطقي pos, منطقي neg)
: vvp_fun_modpath_src(del)
{
      old_قدر = BIT4_X;
      posedge_ = pos;
      negedge_ = neg;
}

منطقي vvp_fun_modpath_ڪنارو::test_vec4(ثابت vvp_vector4_t&ٻٽو_انگ)
{
      vvp_bit4_t tmp = old_قدر;
      old_قدر = ٻٽو_انگ.value(0);

      صحيح ڪنارو_flag = ڪنارو(tmp, old_قدر);
      إذا (ڪنارو_flag > 0) أرجع posedge_;
      إذا (ڪنارو_flag < 0) أرجع negedge_;
      أرجع خطأ;
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
      أرجع 0;
}

ساكن فراغ modpath_src_get_قدر(vpiHandle ref, p_vpi_قدر)
{
      بنية __vpiModPathSrc* modpathsrc = dynamic_cast<__vpiModPathSrc*>(ref);
      أكد(modpathsrc);
      أرجع;
}

ساكن vpiHandle modpath_src_put_قدر(vpiHandle ref, s_vpi_قدر *, صحيح )
{
      بنية __vpiModPathSrc* modpathsrc = dynamic_cast<__vpiModPathSrc*>(ref);
      أكد(modpathsrc);
      أرجع 0;
}

ساكن vpiHandle modpath_src_get_handle(صحيح code, vpiHandle ref)
{
      بنية __vpiModPathSrc*rfp = dynamic_cast<__vpiModPathSrc*>(ref);
      أكد(rfp);

      بدل (code) {

	حالة vpiScope:
	  أرجع rfp->dest->scope;

	  حالة vpiModule:
	      { __vpiScope*scope = rfp->dest->scope;
		طالما (scope && scope->get_type_ڪوڊ() != vpiModule)
		      scope = scope->scope;
		أكد(scope);
		أرجع scope;
	      }

	    // Handles to path term objects should really be obtained via
	    // the vpi_iterate and vpi_scan functions. Continue to allow
	    // them to be obtained here for backwards compatibility with
	    // older versions of Icarus Verilog.

	  حالة vpi_Mod_رستو_اندريون:
	    أرجع &rfp->رستو_رڪن_اندريون;

	  حالة vpi_Mod_رستو_ٻاهريون:
	    أرجع &rfp->dest->رستو_رڪن_ٻاهريون;
      }
      أرجع 0;
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
	  حالة vpi_Mod_رستو_اندريون: {
	    vpiHandle*دليلون = static_cast<vpiHandle*>(صفر_يادگيري_ڏيو(1, حجم(vpiHandle*)));
	    دليلون[0] = &rfp->رستو_رڪن_اندريون;
	    أرجع vpip_make_iterator(1, دليلون, سچ);
	  }
	  حالة vpi_Mod_رستو_ٻاهريون: {
	    vpiHandle*دليلون = static_cast<vpiHandle*>(صفر_يادگيري_ڏيو(1, حجم(vpiHandle*)));
	    دليلون[0] = &rfp->dest->رستو_رڪن_ٻاهريون;
	    أرجع vpip_make_iterator(1, دليلون, سچ);
	  }
      }
      أرجع 0;
}

ساكن vpiHandle modpath_src_اشاريو ( vpiHandle ref, صحيح)
{
      أكد(ref->get_type_ڪوڊ() == vpi_Mod_رستو_اندريون);
      أرجع 0;
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

      تعريفنوع بنانشان حرف map_array_t[12];
	// Only the first six entries are used for the less than twelve maps.
      ساكن ثابت map_array_t map_1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      ساكن ثابت map_array_t map_2 = {0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0};
      ساكن ثابت map_array_t map_3 = {0, 1, 2, 0, 2, 1, 0, 0, 0, 0, 0, 0};
      ساكن ثابت map_array_t map_6 = {0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0};
      ساكن ثابت map_array_t map12 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

      ثابت map_array_t*use_map = 0;
      بدل (delays->no_جو_delays) {
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

      إذا (delays->وقت_type == vpi_Sim_وقت) {
	    لكل (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_timestruct_منزل_وقت(delays->da+use_map[0][idx]);
	    }
      } وإلا {
	      // You cannot create a modpath with a negative delay so set it
	      // to zero per 1364-2005 section 14.3.1.
	    لكل (idx = 0 ; idx < delays->no_جو_delays ; idx += 1) {
		  إذا (delays->da[idx].real < 0.0) delays->da[idx].real = 0.0;
	    }
	    لكل (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_scaled_real_منزل_time64(delays->da[use_map[0][idx]].real,
							src->dest->scope);
	    }
      }

      /* Now define the to-from-x delays if needed. */
      إذا (delays->no_جو_delays <= 6) {
	      /* 0->x is the minimum of 0->z and 0->1. */
	    tmp[DELAY_ڪنارو_0x] = tmp[DELAY_ڪنارو_0z] < tmp[DELAY_ڪنارو_01] ?
	                           tmp[DELAY_ڪنارو_0z] : tmp[DELAY_ڪنارو_01];
	      /* x->1 is the maximum of z->1 and 0->1. */
	    tmp[DELAY_ڪنارو_x1] = tmp[DELAY_ڪنارو_z1] > tmp[DELAY_ڪنارو_01] ?
	                           tmp[DELAY_ڪنارو_z1] : tmp[DELAY_ڪنارو_01];
	      /* 1->x is the minimum of 1->z and 1->0. */
	    tmp[DELAY_ڪنارو_1x] = tmp[DELAY_ڪنارو_1z] < tmp[DELAY_ڪنارو_10] ?
	                           tmp[DELAY_ڪنارو_1z] : tmp[DELAY_ڪنارو_10];
	      /* x->0 is the maximum of z->0 and 1->0. */
	    tmp[DELAY_ڪنارو_x0] = tmp[DELAY_ڪنارو_z0] > tmp[DELAY_ڪنارو_10] ?
	                           tmp[DELAY_ڪنارو_z0] : tmp[DELAY_ڪنارو_10];
	      /* x->z is the maximum of 1->z and 0->z. */
	    tmp[DELAY_ڪنارو_xz] = tmp[DELAY_ڪنارو_1z] > tmp[DELAY_ڪنارو_0z] ?
	                           tmp[DELAY_ڪنارو_1z] : tmp[DELAY_ڪنارو_0z];
	      /* z->x is the minimum of z->1 and z->0. */
	    tmp[DELAY_ڪنارو_zx] = tmp[DELAY_ڪنارو_z1] < tmp[DELAY_ڪنارو_z0] ?
	                           tmp[DELAY_ڪنارو_z1] : tmp[DELAY_ڪنارو_z0];
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

      بدل (delays->no_جو_delays) {
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

      إذا (delays->وقت_type == vpi_Sim_وقت) {
	    لكل (idx = 0; idx < delays->no_جو_delays; idx += 1) {
		  vpip_وقت_منزل_timestruct(delays->da+idx, tmp[idx]);
	    }
      } وإلا {
	    لكل (idx = 0; idx < delays->no_جو_delays; idx += 1) {
		  delays->da[idx].real = vpip_وقت_منزل_scaled_real(tmp[idx], src->dest->scope);
	    }
      }
}

ساكن صحيح pathterm_get(صحيح code, vpiHandle ref)
{
      بنية __vpiModPathTerm*obj = dynamic_cast<__vpiModPathTerm*>(ref);
      أكد(obj);

      بدل (code) {
	  حالة vpi_ڪنارو_2:
	    أرجع obj->ڪنارو;
	  افتراضي:
	    أرجع 0;
      }
}

ساكن vpiHandle pathterm_get_handle(صحيح code, vpiHandle ref)
{
      بنية __vpiModPathTerm*obj = dynamic_cast<__vpiModPathTerm*>(ref);
      أكد(obj);

      بدل (code) {
	  حالة vpi_اظهار:
	    أرجع obj->اظهار;
	  افتراضي:
	    أرجع 0;
      }
}

/*
* The __vpiModPathSrc class is what the VPI client sees as a
* vpiModPath object. The __vpiModPath structure contains items that
* are common to a bunch of modpaths, including the destination term.
*/
مضمن __vpiModPathSrc::__vpiModPathSrc()
{ }

صحيح __vpiModPathSrc::get_type_ڪوڊ(فراغ) ثابت
{ أرجع vpi_Mod_رستو; }

صحيح __vpiModPathSrc::vpi_get(صحيح code)
{ أرجع modpath_src_get(code, هذا); }

فراغ __vpiModPathSrc::vpi_get_قدر(p_vpi_قدر val)
{ modpath_src_get_قدر(هذا, val); }

vpiHandle __vpiModPathSrc::vpi_put_قدر(p_vpi_قدر val, صحيح flags)
{ أرجع modpath_src_put_قدر(هذا, val, flags); }

vpiHandle __vpiModPathSrc::vpi_handle(صحيح code)
{ أرجع modpath_src_get_handle(code, هذا); }

vpiHandle __vpiModPathSrc::vpi_iterate(صحيح code)
{ أرجع modpath_src_iterate(code, هذا); }

vpiHandle __vpiModPathSrc:: vpi_اشاريو(صحيح idx)
{ أرجع modpath_src_اشاريو(هذا, idx); }

فراغ __vpiModPathSrc::vpi_get_delays(p_vpi_delay del)
{ modpath_src_get_delays(هذا, del); }

فراغ __vpiModPathSrc::vpi_put_delays(p_vpi_delay del)
{ modpath_src_put_delays(هذا, del); }

ساكن صحيح modpath_src_free_object( vpiHandle ref )
{
      ميٽيو ref;
      أرجع 1 ;
}

__vpiHandle::free_object_fun_t __vpiModPathSrc::free_object_fun(فراغ)
{ أرجع &modpath_src_free_object; }


مضمن __vpiModPathTerm::__vpiModPathTerm()
{ }

صحيح __vpiModPathTerm::get_type_ڪوڊ(فراغ) ثابت
{ أرجع vpi_رستو_رڪن; }

صحيح __vpiModPathTerm::vpi_get(صحيح code)
{ أرجع pathterm_get(code, هذا); }

vpiHandle __vpiModPathTerm::vpi_handle(صحيح code)
{ أرجع pathterm_get_handle(code, هذا); }

ساكن فراغ initialize_رستو_رڪن(بنية __vpiModPathTerm&obj)
{
      obj.اظهار = 0;
      obj.ڪنارو = vpi_No_ڪنارو;
}

/*
 * This function will construct a vpiModPath Object.
 * give a respective "net", and will point to his
 * respective functor
 */

#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
ساكن بنية __vpiModPath**mp_list = 0;
ساكن بنانشان mp_ڳڻپ = 0;
#جيڪڏهن_پورو

بنية __vpiModPath* vpip_make_modpath(vvp_net_t *net)
{
      بنية __vpiModPath*obj = جديد __vpiModPath;
      obj->scope = vpip_peek_current_scope ( );

      initialize_رستو_رڪن(obj->رستو_رڪن_ٻاهريون);
      obj->داخلا_net = net ;

#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
      mp_ڳڻپ += 1;
      mp_list = static_cast<بنية __vpiModPath **>
                (realloc(mp_list,
                 mp_ڳڻپ*حجم(بنية __vpiModPath **)));
      mp_list[mp_ڳڻپ-1] = obj;
#جيڪڏهن_پورو
      أرجع obj;
}

#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
فراغ modpath_delete()
{
      لكل (بنانشان idx = 0; idx < mp_ڳڻپ; idx += 1) {
	    ميٽيو mp_list[idx];
      }
      حرر(mp_list);
      mp_list = 0;
      mp_ڳڻپ = 0;
}
#جيڪڏهن_پورو

/*
 * This function will construct a vpiModPathIn
 * ( struct __vpiModPathSrc ) Object. will give
 * a delays[12] values, and point to the specified functor
 *
 */

بنية __vpiModPathSrc* vpip_make_modpath_src(بنية __vpiModPath*رستو,
                                              vvp_net_t *net)
{
      بنية __vpiModPathSrc *obj = جديد __vpiModPathSrc;

      obj->dest = رستو;
      obj->type = 0;
      obj->net = net;
      initialize_رستو_رڪن(obj->رستو_رڪن_اندريون);

      أرجع obj;
}

vvp_fun_intermodpath::vvp_fun_intermodpath(vvp_net_t*net, بنانشان width)
: net_(net)
{
      لكل (بنانشان idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = 0;

      cur_vec4_ = vvp_vector4_t(width, BIT4_X);
      وقت_جدول_init_propagate(net_, cur_vec4_);
}

vvp_fun_intermodpath::~vvp_fun_intermodpath()
{
}

فراغ vvp_fun_intermodpath::get_delay12(vvp_time64_t val[12]) ثابت
{
      لكل (بنانشان idx = 0 ;  idx < 12 ;  idx += 1)
	    val[idx] = delay_[idx];
}

فراغ vvp_fun_intermodpath::put_delay12(ثابت vvp_time64_t val[12])
{
      لكل (بنانشان idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = val[idx];
}

/*
 * FIXME: this implementation currently only uses the maximum delay
 * from all the bit changes in the vectors. If there are multiple
 * changes with different delays, then the results would be
 * wrong. What should happen is that if there are multiple changes,
 * multiple vectors approaching the result should be scheduled.
 */
فراغ vvp_fun_intermodpath::recv_vec4(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
                              vvp_context_t)
{
	/* Only the first port is used. */
      إذا (پورٽ.پورٽ() > 0)
	    أرجع;

      إذا (cur_vec4_.eeq(ٻٽو_انگ))
	    أرجع;

	/* Given the scheduled output time, create an output event. */
      vvp_time64_t use_delay = delay_from_ڪنارو(cur_vec4_.value(0),
					       ٻٽو_انگ.value(0),
					       delay_);

	/* FIXME: This bases the edge delay on only the least
	   bit. This is WRONG! I need to find all the possible delays,
	   and schedule an event for each partial change. Hard! */
      لكل (بنانشان idx = 1 ;  idx < ٻٽو_انگ.size() ;  idx += 1) {
	    vvp_time64_t tmp = delay_from_ڪنارو(cur_vec4_.value(idx),
					       ٻٽو_انگ.value(idx),
					       delay_);
	      /* If the current and new bit values match then no delay
	       * is needed for this bit. */
	    إذا (cur_vec4_.value(idx) == ٻٽو_انگ.value(idx)) استمر;
	    أكد(tmp == use_delay);
      }

      cur_vec4_ = ٻٽو_انگ;
      وقت_جدول_generic(هذا, use_delay, خطأ);
}

فراغ vvp_fun_intermodpath::لاڳيتو_لاڳيتو()
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
      أرجع 0;
}

ساكن فراغ intermodpath_get_قدر(vpiHandle ref, p_vpi_قدر)
{
      بنية __vpiInterModPath* intermodpath = dynamic_cast<__vpiInterModPath*>(ref);
      أكد(intermodpath);
      أرجع;
}

ساكن vpiHandle intermodpath_put_قدر(vpiHandle ref, s_vpi_قدر *, صحيح )
{
      بنية __vpiInterModPath* intermodpath = dynamic_cast<__vpiInterModPath*>(ref);
      أكد(intermodpath);
      أرجع 0;
}

ساكن vpiHandle intermodpath_get_handle(صحيح code, vpiHandle ref)
{
      بنية __vpiInterModPath*rfp = dynamic_cast<__vpiInterModPath*>(ref);
      أكد(rfp);

      بدل (code) {

	حالة vpiScope:
	  أرجع rfp->scope;

	  حالة vpiModule:
	      { __vpiScope*scope = rfp->scope;
		طالما (scope && scope->get_type_ڪوڊ() != vpiModule)
		      scope = scope->scope;
		أكد(scope);
		أرجع scope;
	      }
      }
      أرجع 0;
}

ساكن vpiHandle intermodpath_iterate(صحيح code, vpiHandle ref)
{
      بنية __vpiInterModPath*rfp = dynamic_cast<__vpiInterModPath*>(ref);
      أكد(rfp);

	// For now intermodpaths only support exactly two ports
      بدل (code) {
	  حالة vpi_پورٽ: {
	    vpiHandle*دليلون = static_cast<vpiHandle*>(صفر_يادگيري_ڏيو(2, حجم(vpiHandle*)));
	    دليلون[0] = rfp->port1;
	    دليلون[1] = rfp->port2;
	    أرجع vpip_make_iterator(2, دليلون, سچ);
	  }
      }
      أرجع 0;
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

      تعريفنوع بنانشان حرف map_array_t[12];
	// Only the first six entries are used for the less than twelve maps.
      ساكن ثابت map_array_t map_1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      ساكن ثابت map_array_t map_2 = {0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0};
      ساكن ثابت map_array_t map_3 = {0, 1, 2, 0, 2, 1, 0, 0, 0, 0, 0, 0};
      ساكن ثابت map_array_t map_6 = {0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0};
      ساكن ثابت map_array_t map12 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

      ثابت map_array_t*use_map = 0;
      بدل (delays->no_جو_delays) {
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

      إذا (delays->وقت_type == vpi_Sim_وقت) {
	    لكل (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_timestruct_منزل_وقت(delays->da+use_map[0][idx]);
	    }
      } وإلا {
	      // You cannot create a modpath with a negative delay so set it
	      // to zero per 1364-2005 section 14.3.1.
	    لكل (idx = 0 ; idx < delays->no_جو_delays ; idx += 1) {
		  إذا (delays->da[idx].real < 0.0) delays->da[idx].real = 0.0;
	    }
	    لكل (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_scaled_real_منزل_time64(delays->da[use_map[0][idx]].real,
							src->scope);
	    }
      }

      /* Now define the to-from-x delays if needed. */
      إذا (delays->no_جو_delays <= 6) {
	      /* 0->x is the minimum of 0->z and 0->1. */
	    tmp[DELAY_ڪنارو_0x] = tmp[DELAY_ڪنارو_0z] < tmp[DELAY_ڪنارو_01] ?
	                           tmp[DELAY_ڪنارو_0z] : tmp[DELAY_ڪنارو_01];
	      /* x->1 is the maximum of z->1 and 0->1. */
	    tmp[DELAY_ڪنارو_x1] = tmp[DELAY_ڪنارو_z1] > tmp[DELAY_ڪنارو_01] ?
	                           tmp[DELAY_ڪنارو_z1] : tmp[DELAY_ڪنارو_01];
	      /* 1->x is the minimum of 1->z and 1->0. */
	    tmp[DELAY_ڪنارو_1x] = tmp[DELAY_ڪنارو_1z] < tmp[DELAY_ڪنارو_10] ?
	                           tmp[DELAY_ڪنارو_1z] : tmp[DELAY_ڪنارو_10];
	      /* x->0 is the maximum of z->0 and 1->0. */
	    tmp[DELAY_ڪنارو_x0] = tmp[DELAY_ڪنارو_z0] > tmp[DELAY_ڪنارو_10] ?
	                           tmp[DELAY_ڪنارو_z0] : tmp[DELAY_ڪنارو_10];
	      /* x->z is the maximum of 1->z and 0->z. */
	    tmp[DELAY_ڪنارو_xz] = tmp[DELAY_ڪنارو_1z] > tmp[DELAY_ڪنارو_0z] ?
	                           tmp[DELAY_ڪنارو_1z] : tmp[DELAY_ڪنارو_0z];
	      /* z->x is the minimum of z->1 and z->0. */
	    tmp[DELAY_ڪنارو_zx] = tmp[DELAY_ڪنارو_z1] < tmp[DELAY_ڪنارو_z0] ?
	                           tmp[DELAY_ڪنارو_z1] : tmp[DELAY_ڪنارو_z0];
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

      بدل (delays->no_جو_delays) {
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

      إذا (delays->وقت_type == vpi_Sim_وقت) {
	    لكل (idx = 0; idx < delays->no_جو_delays; idx += 1) {
		  vpip_وقت_منزل_timestruct(delays->da+idx, tmp[idx]);
	    }
      } وإلا {
	    لكل (idx = 0; idx < delays->no_جو_delays; idx += 1) {
		  delays->da[idx].real = vpip_وقت_منزل_scaled_real(tmp[idx], src->scope);
	    }
      }
}

/*
* The __vpiInterModPath class is what the VPI client sees as a
* vpiInterModPath object.
*/
مضمن __vpiInterModPath::__vpiInterModPath()
{ }

صحيح __vpiInterModPath::get_type_ڪوڊ(فراغ) ثابت
{ أرجع vpi_Inter_Mod_رستو; }

صحيح __vpiInterModPath::vpi_get(صحيح code)
{ أرجع intermodpath_get(code, هذا); }

فراغ __vpiInterModPath::vpi_get_قدر(p_vpi_قدر val)
{ intermodpath_get_قدر(هذا, val); }

vpiHandle __vpiInterModPath::vpi_put_قدر(p_vpi_قدر val, صحيح flags)
{ أرجع intermodpath_put_قدر(هذا, val, flags); }

vpiHandle __vpiInterModPath::vpi_handle(صحيح code)
{ أرجع intermodpath_get_handle(code, هذا); }

vpiHandle __vpiInterModPath::vpi_iterate(صحيح code)
{ أرجع intermodpath_iterate(code, هذا); }

فراغ __vpiInterModPath::vpi_get_delays(p_vpi_delay del)
{ intermodpath_get_delays(هذا, del); }

فراغ __vpiInterModPath::vpi_put_delays(p_vpi_delay del)
{ intermodpath_put_delays(هذا, del); }

ساكن صحيح intermodpath_free_object( vpiHandle ref )
{
      ميٽيو ref;
      أرجع 1 ;
}

__vpiHandle::free_object_fun_t __vpiInterModPath::free_object_fun(فراغ)
{ أرجع &intermodpath_free_object; }

/*
 * This function will construct a vpiInterModPath Object.
 * give a respective "net", and will point to his
 * respective functor
 */

#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
ساكن بنية __vpiInterModPath**imp_list = 0;
ساكن بنانشان imp_ڳڻپ = 0;
#جيڪڏهن_پورو

بنية __vpiInterModPath* vpip_make_intermodpath(vvp_net_t *net, vpi_پورٽ_Info* port1, vpi_پورٽ_Info* port2)
{
      بنية __vpiInterModPath*obj = جديد __vpiInterModPath;
      obj->scope = vpip_peek_current_scope ( );

      obj->net = net;
      obj->port1 = port1;
      obj->port2 = port2;

#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
      imp_ڳڻپ += 1;
      imp_list = static_cast<بنية __vpiInterModPath **>
                 (realloc(imp_list,
                  imp_ڳڻپ*حجم(بنية __vpiInterModPath **)));
      imp_list[imp_ڳڻپ-1] = obj;
#جيڪڏهن_پورو
      أرجع obj;
}

#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
فراغ intermodpath_delete()
{
      لكل (بنانشان idx = 0; idx < mp_ڳڻپ; idx += 1) {
	    ميٽيو imp_list[idx];
      }
      حرر(imp_list);
      imp_list = 0;
      imp_ڳڻپ = 0;
}
#جيڪڏهن_پورو
