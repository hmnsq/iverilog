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
#اگر_تعریف_شده CHECK_WITH_VALGRIND
#ضمن "vvp_cleanup.h"
#پایان_اگر
#ضمن <iostream>
#ضمن <cstdlib>
#ضمن <list>
#ضمن <cassert>
#ضمن <cmath>
#ضمن "ivl_alloc.h"

استفاده نطاق std;

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

فراغ vvp_delay_t::مجموعه_ignore_decay()
{
      ignore_decay_ = درست;

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

فراغ vvp_delay_t::مجموعه_rise(vvp_time64_t val)
{
      rise_ = val;
      إذا (val < min_delay_) {
	    min_delay_ = val;
	    إذا (ignore_decay_) decay_ = val;
      } وإلا
	    calculate_min_delay_();
}

فراغ vvp_delay_t::مجموعه_fall(vvp_time64_t val)
{
      fall_ = val;
      إذا (val < min_delay_) {
	    min_delay_ = val;
	    إذا (ignore_decay_) decay_ = val;
      } وإلا
	    calculate_min_delay_();
}

فراغ vvp_delay_t::مجموعه_decay(vvp_time64_t val)
{
      أكد(!ignore_decay_);

      decay_ = val;
      إذا (val < min_delay_)
	    min_delay_ = val;
      وإلا
	    calculate_min_delay_();
}

vvp_fun_delay::vvp_fun_delay(vvp_net_t*n, بیعلامت width, ثابت vvp_delay_t&d)
: net_(n), delay_(d)
{
      cur_real_ = 0.0;
      إذا (width > 0) {
            cur_vec4_ = vvp_vector4_t(width, BIT4_X);
            cur_vec8_ = vvp_vector8_t(cur_vec4_, 6, 6);
            زمانبندی_init_propagate(net_, cur_vec4_);
      } وإلا {
            زمانبندی_init_propagate(net_, cur_real_);
      }
      list_ = 0;
      type_ = UNKNOWN_DELAY;
      initial_ = درست;
	// Calculate the values used when converting variable delays
	// to simulation time units.
      ثابت __vpiScope*scope = vpip_peek_current_scope();

      صحيح powr = scope->زمان_units - scope->زمان_precision;
      دور = 1;
      لكل (صحيح lp = 0; lp < powr; lp += 1) دور *= 10;

      powr = scope->زمان_precision - vpip_get_زمان_precision();
      scale_ = 1;
      لكل (صحيح lp = 0; lp < powr; lp += 1) scale_ *= 10;
}

vvp_fun_delay::~vvp_fun_delay()
{
      طالما (بنية event_*cur = خارج_از_صف())
	    حذف cur;
}

منطقي vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        ثابت vvp_vector4_t&بیت)
{
      إذا (list_ == 0) أرجع خطأ;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      إذا (list_->بعدی_2->ptr_vec4.eeq(بیت)) أرجع درست;

      clean_pulse_events_(use_delay);
      أرجع خطأ;
}

منطقي vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        ثابت vvp_vector8_t&بیت)
{
      إذا (list_ == 0) أرجع خطأ;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      إذا (list_->بعدی_2->ptr_vec8.eeq(بیت)) أرجع درست;

      clean_pulse_events_(use_delay);
      أرجع خطأ;
}

منطقي vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        مزدوج بیت)
{
      إذا (list_ == 0) أرجع خطأ;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      إذا (list_->بعدی_2->ptr_real == بیت) أرجع درست;

      clean_pulse_events_(use_delay);
      أرجع خطأ;
}

فراغ vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay)
{
      أكد(list_ != 0);

      افعل {
	    بنية event_*cur = list_->بعدی_2;
	      /* If this event is far enough from the event I'm about
	         to create, then that scheduled event is not a pulse
	         to be eliminated, so we're done. */
	    إذا (cur->sim_زمان+use_delay <= use_delay+زمانبندی_simtime())
		  اكسر;

	    إذا (list_ == cur)
		  list_ = 0;
	    وإلا
		  list_->بعدی_2 = cur->بعدی_2;
	    حذف cur;
      } طالما (list_);
}

/*
 * FIXME: this implementation currently only uses the maximum delay
 * from all the bit changes in the vectors. If there are multiple
 * changes with different delays, then the results would be
 * wrong. What should happen is that if there are multiple changes,
 * multiple vectors approaching the result should be scheduled.
 */
فراغ vvp_fun_delay::recv_vec4(vvp_net_ptr_t پورت, ثابت vvp_vector4_t&بیت,
                              vvp_context_t)
{
      إذا (پورت.پورت() > 0) {
	      // Get the integer value of the bit vector, or 0 if
	      // there are X or Z bits.
	    vvp_time64_t bval = 0;
	      // The following does not work correctly for negative values.
	      // They should be sign extended to 64 bits (1364-2001 9.7.1).
	    vector4_مقصد_مقدار(بیت, bval);
	      // Integer values do not need to be rounded so just scale them.
	    vvp_time64_t val = bval * دور * scale_;

	    بدل (پورت.پورت()) {
		حالة 1:
		  delay_.مجموعه_rise(val);
		  أرجع;
		حالة 2:
		  delay_.مجموعه_fall(val);
		  أرجع;
		حالة 3:
		  delay_.مجموعه_decay(val);
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
	    use_delay = delay_.get_delay(cur_val, بیت.value(0));
	    لكل (بیعلامت idx = 1 ;  idx < بیت.size() ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(cur_val, بیت.value(idx));
		  إذا (tmp > use_delay) use_delay = tmp;
	    }
      } وإلا {
	    أكد(type_ == VEC4_DELAY);

	      // Use as a reference for calculating the delay the
	      // current value of the output. Detect and handle the
	      // special case that the event list contains the current
	      // value as a zero-delay-remaining event.
	    ثابت vvp_vector4_t&use_vec4 = (list_ && list_->بعدی_2->sim_زمان == زمانبندی_simtime())? list_->بعدی_2->ptr_vec4 : cur_vec4_;

	      /* How many bits to compare? */
	    بیعلامت use_wid = use_vec4.size();
	    إذا (بیت.size() < use_wid) use_wid = بیت.size();

	      /* Scan the vectors looking for delays. Select the maximum
	         delay encountered. */
	    use_delay = delay_.get_delay(use_vec4.value(0), بیت.value(0));

	    لكل (بیعلامت idx = 1 ;  idx < use_wid ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(use_vec4.value(idx), بیت.value(idx));
		  إذا (tmp > use_delay) use_delay = tmp;
	    }
      }

      /* what *should* happen here is we check to see if there is a
         transaction in the queue. This would be a pulse that needs to be
         eliminated. */
      إذا (clean_pulse_events_(use_delay, بیت)) أرجع;

      vvp_time64_t use_simtime = زمانبندی_simtime() + use_delay;

	/* And propagate it. */
      إذا (use_delay == 0 && list_ == 0) {
	    cur_vec4_ = بیت;
	    initial_ = خطأ;
	    net_->send_vec4(cur_vec4_, 0);
      } وإلا {
	    بنية event_*cur = جديد بنية event_(use_simtime);
	    cur->پیاپی_پیاپی_ptr = &vvp_fun_delay::پیاپی_پیاپی_vec4;
	    cur->ptr_vec4 = بیت;
	    افزودن_به_صف(cur);
	    زمانبندی_generic(هذا, use_delay, خطأ);
      }
}

فراغ vvp_fun_delay::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
			         بیعلامت پایه, بیعلامت vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, بیت, پایه, vwid, ctx);
}

/* See the recv_vec4 comment above. */
فراغ vvp_fun_delay::recv_vec8(vvp_net_ptr_t پورت, ثابت vvp_vector8_t&بیت)
{
      أكد(پورت.پورت() == 0);

      vvp_time64_t use_delay;
	/* This is an initial value so it needs to be compared to all the
	   bits (the order the bits are changed is not deterministic). */
      إذا (initial_) {
	    type_ = VEC8_DELAY;
            cur_vec4_ = vvp_vector4_t(0, BIT4_X);
	    vvp_bit4_t cur_val = cur_vec8_.value(0).value();
	    use_delay = delay_.get_delay(cur_val, بیت.value(0).value());
	    لكل (بیعلامت idx = 1 ;  idx < بیت.size() ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(cur_val, بیت.value(idx).value());
		  إذا (tmp > use_delay) use_delay = tmp;
	    }
      } وإلا {
	    أكد(type_ == VEC8_DELAY);

	      // Use as a reference for calculating the delay the
	      // current value of the output. Detect and handle the
	      // special case that the event list contains the current
	      // value as a zero-delay-remaining event.
	    ثابت vvp_vector8_t&use_vec8 = (list_ && list_->بعدی_2->sim_زمان == زمانبندی_simtime())? list_->بعدی_2->ptr_vec8 : cur_vec8_;

	      /* How many bits to compare? */
	    بیعلامت use_wid = use_vec8.size();
	    إذا (بیت.size() < use_wid) use_wid = بیت.size();

	      /* Scan the vectors looking for delays. Select the maximum
	         delay encountered. */
	    use_delay = delay_.get_delay(use_vec8.value(0).value(),
	                                 بیت.value(0).value());

	    لكل (بیعلامت idx = 1 ;  idx < use_wid ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(use_vec8.value(idx).value(),
		                         بیت.value(idx).value());
		  إذا (tmp > use_delay) use_delay = tmp;
	    }
      }

      /* what *should* happen here is we check to see if there is a
         transaction in the queue. This would be a pulse that needs to be
         eliminated. */
      إذا (clean_pulse_events_(use_delay, بیت)) أرجع;

      vvp_time64_t use_simtime = زمانبندی_simtime() + use_delay;

	/* And propagate it. */
      إذا (use_delay == 0 && list_ == 0) {
	    cur_vec8_ = بیت;
	    initial_ = خطأ;
	    net_->send_vec8(cur_vec8_);
      } وإلا {
	    بنية event_*cur = جديد بنية event_(use_simtime);
	    cur->ptr_vec8 = بیت;
	    cur->پیاپی_پیاپی_ptr = &vvp_fun_delay::پیاپی_پیاپی_vec8;
	    افزودن_به_صف(cur);
	    زمانبندی_generic(هذا, use_delay, خطأ);
      }
}

فراغ vvp_fun_delay::recv_vec8_pv(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&بیت,
			         بیعلامت پایه, بیعلامت vwid)
{
      recv_vec8_pv_(ptr, بیت, پایه, vwid);
}

فراغ vvp_fun_delay::recv_real(vvp_net_ptr_t پورت, مزدوج بیت,
                              vvp_context_t)
{
      إذا (پورت.پورت() > 0) {
	    /* If the port is not 0, then this is a delay value that
	    should be rounded and converted to an integer delay. */
	    vvp_time64_t val = 0;
	    إذا (بیت > -0.5) {
		  val = (vvp_time64_t) (بیت * دور + 0.5) * scale_;
	    } وإلا إذا (بیت != بیت) {
		    // For a NaN we use the default (0).
	    } وإلا {
		  vvp_vector4_t vec4(8*حجم(vvp_time64_t),
		                     floor(-بیت * دور + 0.5) * -1 * scale_);
		  vector4_مقصد_مقدار(vec4, val);
	    }

	    بدل (پورت.پورت()) {
		حالة 1:
		  delay_.مجموعه_rise(val);
		  أرجع;
		حالة 2:
		  delay_.مجموعه_fall(val);
		  أرجع;
		حالة 3:
		  delay_.مجموعه_decay(val);
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
      إذا (clean_pulse_events_(use_delay, بیت)) أرجع;

      /* This must be done after cleaning pulses to avoid propagating
       * an incorrect value. */
      إذا (cur_real_ == بیت) أرجع;

      vvp_time64_t use_simtime = زمانبندی_simtime() + use_delay;

      إذا (use_delay == 0 && list_ == 0) {
	    cur_real_ = بیت;
	    initial_ = خطأ;
	    net_->send_real(cur_real_, 0);
      } وإلا {
	    بنية event_*cur = جديد بنية event_(use_simtime);
	    cur->پیاپی_پیاپی_ptr = &vvp_fun_delay::پیاپی_پیاپی_real;
	    cur->ptr_real = بیت;
	    افزودن_به_صف(cur);

	    زمانبندی_generic(هذا, use_delay, خطأ);
      }
}

فراغ vvp_fun_delay::پیاپی_پیاپی()
{
      vvp_time64_t sim_زمان = زمانبندی_simtime();
      إذا (list_ == 0 || list_->بعدی_2->sim_زمان > sim_زمان)
	    أرجع;

      بنية event_*cur = خارج_از_صف();
      إذا (cur == 0)
	    أرجع;

      (هذا->*(cur->پیاپی_پیاپی_ptr))(cur);
      initial_ = خطأ;
      حذف cur;
}

فراغ vvp_fun_delay::پیاپی_پیاپی_vec4(ثابت بنية event_*cur)
{
      cur_vec4_ = cur->ptr_vec4;
      net_->send_vec4(cur_vec4_, 0);
}

فراغ vvp_fun_delay::پیاپی_پیاپی_vec8(ثابت بنية vvp_fun_delay::event_*cur)
{
      cur_vec8_ = cur->ptr_vec8;
      net_->send_vec8(cur_vec8_);
}

فراغ vvp_fun_delay::پیاپی_پیاپی_real(ثابت بنية vvp_fun_delay::event_*cur)
{
      cur_real_ = cur->ptr_real;
      net_->send_real(cur_real_, 0);
}

vvp_fun_modpath::vvp_fun_modpath(vvp_net_t*net, بیعلامت width)
: net_(net), src_list_(0), ifnone_list_(0)
{
      cur_vec4_ = vvp_vector4_t(width, BIT4_X);
      زمانبندی_init_propagate(net_, cur_vec4_);
}

vvp_fun_modpath::~vvp_fun_modpath()
{
	// Delete the source probes.
      طالما (src_list_) {
	    vvp_fun_modpath_src*tmp = src_list_;
	    src_list_ = tmp->بعدی;
	    حذف tmp;
      }
      طالما (ifnone_list_) {
	    vvp_fun_modpath_src*tmp = ifnone_list_;
	    ifnone_list_ = tmp->بعدی;
	    حذف tmp;
      }
}

فراغ vvp_fun_modpath::جمع_کن_modpath_src(vvp_fun_modpath_src*that, منطقي ifnone)
{
      أكد(that->بعدی == 0);
      إذا (ifnone) {
	    that->بعدی = ifnone_list_;
	    ifnone_list_ = that;
      } وإلا {
	    that->بعدی = src_list_;
	    src_list_ = that;
      }
}

ساكن vvp_time64_t delay_from_یال(vvp_bit4_t a, vvp_bit4_t b,
                                           vvp_time64_t ثابت array[12])
{
      تعريفنوع delay_یال_t bit4_table4[4];
      ساكن ثابت bit4_table4 یال_جدول[4] = {
	    { DELAY_یال_01, DELAY_یال_01, DELAY_یال_0z, DELAY_یال_0x },
	    { DELAY_یال_10, DELAY_یال_10, DELAY_یال_1z, DELAY_یال_1x },
	    { DELAY_یال_z0, DELAY_یال_z1, DELAY_یال_z0, DELAY_یال_zx },
	    { DELAY_یال_x0, DELAY_یال_x1, DELAY_یال_xz, DELAY_یال_x0 }
      };

      أرجع array[ یال_جدول[a][b] ];
}

فراغ vvp_fun_modpath::recv_vec4(vvp_net_ptr_t پورت, ثابت vvp_vector4_t&بیت,
                                vvp_context_t)
{
	/* Only the first port is used. */
      إذا (پورت.پورت() > 0)
	    أرجع;

      إذا (cur_vec4_.eeq(بیت))
	    أرجع;

	/* Select a time delay source that applies. Notice that there
	   may be multiple delay sources that apply, so collect all
	   the candidates into a list first. */
      list<vvp_fun_modpath_src*>candidate_list;
      vvp_time64_t candidate_wake_زمان = 0;
      لكل (vvp_fun_modpath_src*cur = src_list_ ;  cur ;  cur=cur->بعدی) {
	      /* Skip paths that are disabled by conditions. */
	    إذا (cur->condition_flag_ == خطأ)
		  استمر;

	    إذا (candidate_list.empty()) {
		  candidate_list.push_back(cur);
		  candidate_wake_زمان = cur->wake_زمان;
	    } وإلا إذا (cur->wake_زمان == candidate_wake_زمان) {
		  candidate_list.push_back(cur);
	    } وإلا إذا (cur->wake_زمان > candidate_wake_زمان) {
		  candidate_list.assign(1, cur);
		  candidate_wake_زمان = cur->wake_زمان;
	    } وإلا {
		  استمر; /* Skip this entry. */
	    }
      }

	/* Only add the ifnone delay if it has a later wake_time_ or
	 * if there are no normal delays. */
      vvp_time64_t ifnone_wake_زمان = candidate_wake_زمان;
      لكل (vvp_fun_modpath_src*cur = ifnone_list_ ;  cur ;  cur=cur->بعدی) {
	    إذا (candidate_list.empty()) {
		  candidate_list.push_back(cur);
		  ifnone_wake_زمان = cur->wake_زمان;
	    } وإلا إذا (cur->wake_زمان == ifnone_wake_زمان &&
	               ifnone_wake_زمان > candidate_wake_زمان) {
		  candidate_list.push_back(cur);
	    } وإلا إذا (cur->wake_زمان > ifnone_wake_زمان) {
		  candidate_list.assign(1, cur);
		  ifnone_wake_زمان = cur->wake_زمان;
	    } وإلا {
		  استمر; /* Skip this entry. */
	    }
      }

	/* Handle the special case that there are no delays that
	   match. This may happen, for example, if the set of
	   conditional delays is incomplete, leaving some cases
	   uncovered. In that case, just pass the data without delay */
      إذا (candidate_list.empty()) {
	    cur_vec4_ = بیت;
	    زمانبندی_generic(هذا, 0, خطأ);
	    أرجع;
      }

	/* Now given that we have a list of candidate delays, find for
	   each if the 12 numbers the minimum from all the
	   candidates. This minimum set becomes the chosen delay to
	   use. */
      vvp_time64_t بیرون_at[12];
      vvp_time64_t now = زمانبندی_simtime();

      تعريفنوع list<vvp_fun_modpath_src*>::const_iterator iter_t;

      iter_t cur = candidate_list.begin();
      ثابت vvp_fun_modpath_src*src = *cur;

      لكل (بیعلامت idx = 0 ;  idx < 12 ;  idx += 1) {
	    بیرون_at[idx] = src->wake_زمان + src->delay_[idx];
	    إذا (بیرون_at[idx] <= now)
		  بیرون_at[idx] = 0;
	    وإلا
		  بیرون_at[idx] -= now;
      }

      لكل (++ cur ; cur != candidate_list.انتها() ; ++ cur ) {
	    src = *cur;
	    لكل (بیعلامت idx = 0 ;  idx < 12 ;  idx += 1) {
		  vvp_time64_t tmp = src->wake_زمان + src->delay_[idx];
		  إذا (tmp <= now)
			tmp = 0;
		  وإلا
			tmp -= now;
		  إذا (tmp < بیرون_at[idx])
			بیرون_at[idx] = tmp;
	    }
      }

	/* Given the scheduled output time, create an output event. */
      vvp_time64_t use_delay = delay_from_یال(cur_vec4_.value(0),
					       بیت.value(0),
					       بیرون_at);

	/* FIXME: This bases the edge delay on only the least
	   bit. This is WRONG! I need to find all the possible delays,
	   and schedule an event for each partial change. Hard! */
      لكل (بیعلامت idx = 1 ;  idx < بیت.size() ;  idx += 1) {
	    vvp_time64_t tmp = delay_from_یال(cur_vec4_.value(idx),
					       بیت.value(idx),
					       بیرون_at);
	      /* If the current and new bit values match then no delay
	       * is needed for this bit. */
	    إذا (cur_vec4_.value(idx) == بیت.value(idx)) استمر;
	    إذا (tmp != use_delay) {
		  fprintf(stderr, "sorry: multi-bit module path delays are "
		                  "currently not fully supported.\n");
		  fprintf(stderr, "     : using the LSB delay for all bits.\n");
		  استمر;
            }
      }

      cur_vec4_ = بیت;
      زمانبندی_generic(هذا, use_delay, خطأ);
}

فراغ vvp_fun_modpath::پیاپی_پیاپی()
{
      net_->send_vec4(cur_vec4_, 0);
}

vvp_fun_modpath_src::vvp_fun_modpath_src(vvp_time64_t ثابت del[12])
{
      لكل (بیعلامت idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = del[idx];

      بعدی = 0;
      wake_زمان = 0;
      condition_flag_ = درست;
}

vvp_fun_modpath_src::~vvp_fun_modpath_src()
{
}

فراغ vvp_fun_modpath_src::get_delay12(vvp_time64_t val[12]) ثابت
{
      لكل (بیعلامت idx = 0 ;  idx < 12 ;  idx += 1)
	    val[idx] = delay_[idx];
}

فراغ vvp_fun_modpath_src::put_delay12(ثابت vvp_time64_t val[12])
{
      لكل (بیعلامت idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = val[idx];
}

فراغ vvp_fun_modpath_src::recv_vec4(vvp_net_ptr_t پورت, ثابت vvp_vector4_t&بیت,
                                    vvp_context_t)
{
      إذا (پورت.پورت() == 0) {
	      // The modpath input...
	    إذا (test_vec4(بیت))
		  wake_زمان = زمانبندی_simtime();

      } وإلا إذا (پورت.پورت() == 1) {
	      // The modpath condition input...
	    إذا (بیت.value(0) != BIT4_0)
		  condition_flag_ = درست;
	    وإلا
		  condition_flag_ = خطأ;
      }
}

منطقي vvp_fun_modpath_src::test_vec4(ثابت vvp_vector4_t&)
{
      أرجع درست;
}

vvp_fun_modpath_یال::vvp_fun_modpath_یال(vvp_time64_t del[12],
					   منطقي pos, منطقي neg)
: vvp_fun_modpath_src(del)
{
      old_مقدار = BIT4_X;
      posedge_ = pos;
      negedge_ = neg;
}

منطقي vvp_fun_modpath_یال::test_vec4(ثابت vvp_vector4_t&بیت)
{
      vvp_bit4_t tmp = old_مقدار;
      old_مقدار = بیت.value(0);

      صحيح یال_flag = یال(tmp, old_مقدار);
      إذا (یال_flag > 0) أرجع posedge_;
      إذا (یال_flag < 0) أرجع negedge_;
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

ساكن فراغ modpath_src_get_مقدار(vpiHandle ref, p_vpi_مقدار)
{
      بنية __vpiModPathSrc* modpathsrc = dynamic_cast<__vpiModPathSrc*>(ref);
      أكد(modpathsrc);
      أرجع;
}

ساكن vpiHandle modpath_src_put_مقدار(vpiHandle ref, s_vpi_مقدار *, صحيح )
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
		طالما (scope && scope->get_type_کد() != vpiModule)
		      scope = scope->scope;
		أكد(scope);
		أرجع scope;
	      }

	    // Handles to path term objects should really be obtained via
	    // the vpi_iterate and vpi_scan functions. Continue to allow
	    // them to be obtained here for backwards compatibility with
	    // older versions of Icarus Verilog.

	  حالة vpi_Mod_مسیر_درون:
	    أرجع &rfp->مسیر_جمله_درون;

	  حالة vpi_Mod_مسیر_بیرون:
	    أرجع &rfp->dest->مسیر_جمله_بیرون;
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
	  حالة vpi_Mod_مسیر_درون: {
	    vpiHandle*آرگومانها = static_cast<vpiHandle*>(حافظه_صفر_بده(1, حجم(vpiHandle*)));
	    آرگومانها[0] = &rfp->مسیر_جمله_درون;
	    أرجع vpip_make_iterator(1, آرگومانها, درست);
	  }
	  حالة vpi_Mod_مسیر_بیرون: {
	    vpiHandle*آرگومانها = static_cast<vpiHandle*>(حافظه_صفر_بده(1, حجم(vpiHandle*)));
	    آرگومانها[0] = &rfp->dest->مسیر_جمله_بیرون;
	    أرجع vpip_make_iterator(1, آرگومانها, درست);
	  }
      }
      أرجع 0;
}

ساكن vpiHandle modpath_src_شاخص ( vpiHandle ref, صحيح)
{
      أكد(ref->get_type_کد() == vpi_Mod_مسیر_درون);
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

      تعريفنوع بیعلامت حرف map_array_t[12];
	// Only the first six entries are used for the less than twelve maps.
      ساكن ثابت map_array_t map_1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      ساكن ثابت map_array_t map_2 = {0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0};
      ساكن ثابت map_array_t map_3 = {0, 1, 2, 0, 2, 1, 0, 0, 0, 0, 0, 0};
      ساكن ثابت map_array_t map_6 = {0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0};
      ساكن ثابت map_array_t map12 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

      ثابت map_array_t*use_map = 0;
      بدل (delays->no_از_delays) {
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

      إذا (delays->زمان_type == vpi_Sim_زمان) {
	    لكل (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_timestruct_مقصد_زمان(delays->da+use_map[0][idx]);
	    }
      } وإلا {
	      // You cannot create a modpath with a negative delay so set it
	      // to zero per 1364-2005 section 14.3.1.
	    لكل (idx = 0 ; idx < delays->no_از_delays ; idx += 1) {
		  إذا (delays->da[idx].real < 0.0) delays->da[idx].real = 0.0;
	    }
	    لكل (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_scaled_real_مقصد_time64(delays->da[use_map[0][idx]].real,
							src->dest->scope);
	    }
      }

      /* Now define the to-from-x delays if needed. */
      إذا (delays->no_از_delays <= 6) {
	      /* 0->x is the minimum of 0->z and 0->1. */
	    tmp[DELAY_یال_0x] = tmp[DELAY_یال_0z] < tmp[DELAY_یال_01] ?
	                           tmp[DELAY_یال_0z] : tmp[DELAY_یال_01];
	      /* x->1 is the maximum of z->1 and 0->1. */
	    tmp[DELAY_یال_x1] = tmp[DELAY_یال_z1] > tmp[DELAY_یال_01] ?
	                           tmp[DELAY_یال_z1] : tmp[DELAY_یال_01];
	      /* 1->x is the minimum of 1->z and 1->0. */
	    tmp[DELAY_یال_1x] = tmp[DELAY_یال_1z] < tmp[DELAY_یال_10] ?
	                           tmp[DELAY_یال_1z] : tmp[DELAY_یال_10];
	      /* x->0 is the maximum of z->0 and 1->0. */
	    tmp[DELAY_یال_x0] = tmp[DELAY_یال_z0] > tmp[DELAY_یال_10] ?
	                           tmp[DELAY_یال_z0] : tmp[DELAY_یال_10];
	      /* x->z is the maximum of 1->z and 0->z. */
	    tmp[DELAY_یال_xz] = tmp[DELAY_یال_1z] > tmp[DELAY_یال_0z] ?
	                           tmp[DELAY_یال_1z] : tmp[DELAY_یال_0z];
	      /* z->x is the minimum of z->1 and z->0. */
	    tmp[DELAY_یال_zx] = tmp[DELAY_یال_z1] < tmp[DELAY_یال_z0] ?
	                           tmp[DELAY_یال_z1] : tmp[DELAY_یال_z0];
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

      بدل (delays->no_از_delays) {
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

      إذا (delays->زمان_type == vpi_Sim_زمان) {
	    لكل (idx = 0; idx < delays->no_از_delays; idx += 1) {
		  vpip_زمان_مقصد_timestruct(delays->da+idx, tmp[idx]);
	    }
      } وإلا {
	    لكل (idx = 0; idx < delays->no_از_delays; idx += 1) {
		  delays->da[idx].real = vpip_زمان_مقصد_scaled_real(tmp[idx], src->dest->scope);
	    }
      }
}

ساكن صحيح pathterm_get(صحيح code, vpiHandle ref)
{
      بنية __vpiModPathTerm*obj = dynamic_cast<__vpiModPathTerm*>(ref);
      أكد(obj);

      بدل (code) {
	  حالة vpi_یال_2:
	    أرجع obj->یال;
	  افتراضي:
	    أرجع 0;
      }
}

ساكن vpiHandle pathterm_get_handle(صحيح code, vpiHandle ref)
{
      بنية __vpiModPathTerm*obj = dynamic_cast<__vpiModPathTerm*>(ref);
      أكد(obj);

      بدل (code) {
	  حالة vpi_عبارت:
	    أرجع obj->عبارت;
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

صحيح __vpiModPathSrc::get_type_کد(فراغ) ثابت
{ أرجع vpi_Mod_مسیر; }

صحيح __vpiModPathSrc::vpi_get(صحيح code)
{ أرجع modpath_src_get(code, هذا); }

فراغ __vpiModPathSrc::vpi_get_مقدار(p_vpi_مقدار val)
{ modpath_src_get_مقدار(هذا, val); }

vpiHandle __vpiModPathSrc::vpi_put_مقدار(p_vpi_مقدار val, صحيح flags)
{ أرجع modpath_src_put_مقدار(هذا, val, flags); }

vpiHandle __vpiModPathSrc::vpi_handle(صحيح code)
{ أرجع modpath_src_get_handle(code, هذا); }

vpiHandle __vpiModPathSrc::vpi_iterate(صحيح code)
{ أرجع modpath_src_iterate(code, هذا); }

vpiHandle __vpiModPathSrc:: vpi_شاخص(صحيح idx)
{ أرجع modpath_src_شاخص(هذا, idx); }

فراغ __vpiModPathSrc::vpi_get_delays(p_vpi_delay del)
{ modpath_src_get_delays(هذا, del); }

فراغ __vpiModPathSrc::vpi_put_delays(p_vpi_delay del)
{ modpath_src_put_delays(هذا, del); }

ساكن صحيح modpath_src_free_object( vpiHandle ref )
{
      حذف ref;
      أرجع 1 ;
}

__vpiHandle::free_object_fun_t __vpiModPathSrc::free_object_fun(فراغ)
{ أرجع &modpath_src_free_object; }


مضمن __vpiModPathTerm::__vpiModPathTerm()
{ }

صحيح __vpiModPathTerm::get_type_کد(فراغ) ثابت
{ أرجع vpi_مسیر_جمله; }

صحيح __vpiModPathTerm::vpi_get(صحيح code)
{ أرجع pathterm_get(code, هذا); }

vpiHandle __vpiModPathTerm::vpi_handle(صحيح code)
{ أرجع pathterm_get_handle(code, هذا); }

ساكن فراغ initialize_مسیر_جمله(بنية __vpiModPathTerm&obj)
{
      obj.عبارت = 0;
      obj.یال = vpi_No_یال;
}

/*
 * This function will construct a vpiModPath Object.
 * give a respective "net", and will point to his
 * respective functor
 */

#اگر_تعریف_شده CHECK_WITH_VALGRIND
ساكن بنية __vpiModPath**mp_list = 0;
ساكن بیعلامت mp_شمار = 0;
#پایان_اگر

بنية __vpiModPath* vpip_make_modpath(vvp_net_t *net)
{
      بنية __vpiModPath*obj = جديد __vpiModPath;
      obj->scope = vpip_peek_current_scope ( );

      initialize_مسیر_جمله(obj->مسیر_جمله_بیرون);
      obj->درون_داد_net = net ;

#اگر_تعریف_شده CHECK_WITH_VALGRIND
      mp_شمار += 1;
      mp_list = static_cast<بنية __vpiModPath **>
                (realloc(mp_list,
                 mp_شمار*حجم(بنية __vpiModPath **)));
      mp_list[mp_شمار-1] = obj;
#پایان_اگر
      أرجع obj;
}

#اگر_تعریف_شده CHECK_WITH_VALGRIND
فراغ modpath_delete()
{
      لكل (بیعلامت idx = 0; idx < mp_شمار; idx += 1) {
	    حذف mp_list[idx];
      }
      حرر(mp_list);
      mp_list = 0;
      mp_شمار = 0;
}
#پایان_اگر

/*
 * This function will construct a vpiModPathIn
 * ( struct __vpiModPathSrc ) Object. will give
 * a delays[12] values, and point to the specified functor
 *
 */

بنية __vpiModPathSrc* vpip_make_modpath_src(بنية __vpiModPath*مسیر,
                                              vvp_net_t *net)
{
      بنية __vpiModPathSrc *obj = جديد __vpiModPathSrc;

      obj->dest = مسیر;
      obj->type = 0;
      obj->net = net;
      initialize_مسیر_جمله(obj->مسیر_جمله_درون);

      أرجع obj;
}

vvp_fun_intermodpath::vvp_fun_intermodpath(vvp_net_t*net, بیعلامت width)
: net_(net)
{
      لكل (بیعلامت idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = 0;

      cur_vec4_ = vvp_vector4_t(width, BIT4_X);
      زمانبندی_init_propagate(net_, cur_vec4_);
}

vvp_fun_intermodpath::~vvp_fun_intermodpath()
{
}

فراغ vvp_fun_intermodpath::get_delay12(vvp_time64_t val[12]) ثابت
{
      لكل (بیعلامت idx = 0 ;  idx < 12 ;  idx += 1)
	    val[idx] = delay_[idx];
}

فراغ vvp_fun_intermodpath::put_delay12(ثابت vvp_time64_t val[12])
{
      لكل (بیعلامت idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = val[idx];
}

/*
 * FIXME: this implementation currently only uses the maximum delay
 * from all the bit changes in the vectors. If there are multiple
 * changes with different delays, then the results would be
 * wrong. What should happen is that if there are multiple changes,
 * multiple vectors approaching the result should be scheduled.
 */
فراغ vvp_fun_intermodpath::recv_vec4(vvp_net_ptr_t پورت, ثابت vvp_vector4_t&بیت,
                              vvp_context_t)
{
	/* Only the first port is used. */
      إذا (پورت.پورت() > 0)
	    أرجع;

      إذا (cur_vec4_.eeq(بیت))
	    أرجع;

	/* Given the scheduled output time, create an output event. */
      vvp_time64_t use_delay = delay_from_یال(cur_vec4_.value(0),
					       بیت.value(0),
					       delay_);

	/* FIXME: This bases the edge delay on only the least
	   bit. This is WRONG! I need to find all the possible delays,
	   and schedule an event for each partial change. Hard! */
      لكل (بیعلامت idx = 1 ;  idx < بیت.size() ;  idx += 1) {
	    vvp_time64_t tmp = delay_from_یال(cur_vec4_.value(idx),
					       بیت.value(idx),
					       delay_);
	      /* If the current and new bit values match then no delay
	       * is needed for this bit. */
	    إذا (cur_vec4_.value(idx) == بیت.value(idx)) استمر;
	    أكد(tmp == use_delay);
      }

      cur_vec4_ = بیت;
      زمانبندی_generic(هذا, use_delay, خطأ);
}

فراغ vvp_fun_intermodpath::پیاپی_پیاپی()
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

ساكن فراغ intermodpath_get_مقدار(vpiHandle ref, p_vpi_مقدار)
{
      بنية __vpiInterModPath* intermodpath = dynamic_cast<__vpiInterModPath*>(ref);
      أكد(intermodpath);
      أرجع;
}

ساكن vpiHandle intermodpath_put_مقدار(vpiHandle ref, s_vpi_مقدار *, صحيح )
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
		طالما (scope && scope->get_type_کد() != vpiModule)
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
	  حالة vpi_پورت: {
	    vpiHandle*آرگومانها = static_cast<vpiHandle*>(حافظه_صفر_بده(2, حجم(vpiHandle*)));
	    آرگومانها[0] = rfp->port1;
	    آرگومانها[1] = rfp->port2;
	    أرجع vpip_make_iterator(2, آرگومانها, درست);
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

      تعريفنوع بیعلامت حرف map_array_t[12];
	// Only the first six entries are used for the less than twelve maps.
      ساكن ثابت map_array_t map_1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      ساكن ثابت map_array_t map_2 = {0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0};
      ساكن ثابت map_array_t map_3 = {0, 1, 2, 0, 2, 1, 0, 0, 0, 0, 0, 0};
      ساكن ثابت map_array_t map_6 = {0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0};
      ساكن ثابت map_array_t map12 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

      ثابت map_array_t*use_map = 0;
      بدل (delays->no_از_delays) {
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

      إذا (delays->زمان_type == vpi_Sim_زمان) {
	    لكل (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_timestruct_مقصد_زمان(delays->da+use_map[0][idx]);
	    }
      } وإلا {
	      // You cannot create a modpath with a negative delay so set it
	      // to zero per 1364-2005 section 14.3.1.
	    لكل (idx = 0 ; idx < delays->no_از_delays ; idx += 1) {
		  إذا (delays->da[idx].real < 0.0) delays->da[idx].real = 0.0;
	    }
	    لكل (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_scaled_real_مقصد_time64(delays->da[use_map[0][idx]].real,
							src->scope);
	    }
      }

      /* Now define the to-from-x delays if needed. */
      إذا (delays->no_از_delays <= 6) {
	      /* 0->x is the minimum of 0->z and 0->1. */
	    tmp[DELAY_یال_0x] = tmp[DELAY_یال_0z] < tmp[DELAY_یال_01] ?
	                           tmp[DELAY_یال_0z] : tmp[DELAY_یال_01];
	      /* x->1 is the maximum of z->1 and 0->1. */
	    tmp[DELAY_یال_x1] = tmp[DELAY_یال_z1] > tmp[DELAY_یال_01] ?
	                           tmp[DELAY_یال_z1] : tmp[DELAY_یال_01];
	      /* 1->x is the minimum of 1->z and 1->0. */
	    tmp[DELAY_یال_1x] = tmp[DELAY_یال_1z] < tmp[DELAY_یال_10] ?
	                           tmp[DELAY_یال_1z] : tmp[DELAY_یال_10];
	      /* x->0 is the maximum of z->0 and 1->0. */
	    tmp[DELAY_یال_x0] = tmp[DELAY_یال_z0] > tmp[DELAY_یال_10] ?
	                           tmp[DELAY_یال_z0] : tmp[DELAY_یال_10];
	      /* x->z is the maximum of 1->z and 0->z. */
	    tmp[DELAY_یال_xz] = tmp[DELAY_یال_1z] > tmp[DELAY_یال_0z] ?
	                           tmp[DELAY_یال_1z] : tmp[DELAY_یال_0z];
	      /* z->x is the minimum of z->1 and z->0. */
	    tmp[DELAY_یال_zx] = tmp[DELAY_یال_z1] < tmp[DELAY_یال_z0] ?
	                           tmp[DELAY_یال_z1] : tmp[DELAY_یال_z0];
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

      بدل (delays->no_از_delays) {
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

      إذا (delays->زمان_type == vpi_Sim_زمان) {
	    لكل (idx = 0; idx < delays->no_از_delays; idx += 1) {
		  vpip_زمان_مقصد_timestruct(delays->da+idx, tmp[idx]);
	    }
      } وإلا {
	    لكل (idx = 0; idx < delays->no_از_delays; idx += 1) {
		  delays->da[idx].real = vpip_زمان_مقصد_scaled_real(tmp[idx], src->scope);
	    }
      }
}

/*
* The __vpiInterModPath class is what the VPI client sees as a
* vpiInterModPath object.
*/
مضمن __vpiInterModPath::__vpiInterModPath()
{ }

صحيح __vpiInterModPath::get_type_کد(فراغ) ثابت
{ أرجع vpi_Inter_Mod_مسیر; }

صحيح __vpiInterModPath::vpi_get(صحيح code)
{ أرجع intermodpath_get(code, هذا); }

فراغ __vpiInterModPath::vpi_get_مقدار(p_vpi_مقدار val)
{ intermodpath_get_مقدار(هذا, val); }

vpiHandle __vpiInterModPath::vpi_put_مقدار(p_vpi_مقدار val, صحيح flags)
{ أرجع intermodpath_put_مقدار(هذا, val, flags); }

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
      حذف ref;
      أرجع 1 ;
}

__vpiHandle::free_object_fun_t __vpiInterModPath::free_object_fun(فراغ)
{ أرجع &intermodpath_free_object; }

/*
 * This function will construct a vpiInterModPath Object.
 * give a respective "net", and will point to his
 * respective functor
 */

#اگر_تعریف_شده CHECK_WITH_VALGRIND
ساكن بنية __vpiInterModPath**imp_list = 0;
ساكن بیعلامت imp_شمار = 0;
#پایان_اگر

بنية __vpiInterModPath* vpip_make_intermodpath(vvp_net_t *net, vpi_پورت_Info* port1, vpi_پورت_Info* port2)
{
      بنية __vpiInterModPath*obj = جديد __vpiInterModPath;
      obj->scope = vpip_peek_current_scope ( );

      obj->net = net;
      obj->port1 = port1;
      obj->port2 = port2;

#اگر_تعریف_شده CHECK_WITH_VALGRIND
      imp_شمار += 1;
      imp_list = static_cast<بنية __vpiInterModPath **>
                 (realloc(imp_list,
                  imp_شمار*حجم(بنية __vpiInterModPath **)));
      imp_list[imp_شمار-1] = obj;
#پایان_اگر
      أرجع obj;
}

#اگر_تعریف_شده CHECK_WITH_VALGRIND
فراغ intermodpath_delete()
{
      لكل (بیعلامت idx = 0; idx < mp_شمار; idx += 1) {
	    حذف imp_list[idx];
      }
      حرر(imp_list);
      imp_list = 0;
      imp_شمار = 0;
}
#پایان_اگر
