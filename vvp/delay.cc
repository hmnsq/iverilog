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
#جے_تعریف_شدہ CHECK_WITH_VALGRIND
#ضمن "vvp_cleanup.h"
#جے_انت
#ضمن <iostream>
#ضمن <cstdlib>
#ضمن <list>
#ضمن <cassert>
#ضمن <cmath>
#ضمن "ivl_alloc.h"

ورتوں نطاق std;

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

فراغ vvp_delay_t::سیٹ_ignore_decay()
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

فراغ vvp_delay_t::سیٹ_rise(vvp_time64_t val)
{
      rise_ = val;
      إذا (val < min_delay_) {
	    min_delay_ = val;
	    إذا (ignore_decay_) decay_ = val;
      } وإلا
	    calculate_min_delay_();
}

فراغ vvp_delay_t::سیٹ_fall(vvp_time64_t val)
{
      fall_ = val;
      إذا (val < min_delay_) {
	    min_delay_ = val;
	    إذا (ignore_decay_) decay_ = val;
      } وإلا
	    calculate_min_delay_();
}

فراغ vvp_delay_t::سیٹ_decay(vvp_time64_t val)
{
      أكد(!ignore_decay_);

      decay_ = val;
      إذا (val < min_delay_)
	    min_delay_ = val;
      وإلا
	    calculate_min_delay_();
}

vvp_fun_delay::vvp_fun_delay(vvp_net_t*n, بنانشانی width, ثابت vvp_delay_t&d)
: net_(n), delay_(d)
{
      cur_real_ = 0.0;
      إذا (width > 0) {
            cur_vec4_ = vvp_vector4_t(width, BIT4_X);
            cur_vec8_ = vvp_vector8_t(cur_vec4_, 6, 6);
            ویلا_سارنی_init_propagate(net_, cur_vec4_);
      } وإلا {
            ویلا_سارنی_init_propagate(net_, cur_real_);
      }
      list_ = 0;
      type_ = UNKNOWN_DELAY;
      initial_ = سچ;
	// Calculate the values used when converting variable delays
	// to simulation time units.
      ثابت __vpiScope*scope = vpip_peek_current_scope();

      صحيح powr = scope->ویلا_units - scope->ویلا_precision;
      گیڑا = 1;
      لكل (صحيح lp = 0; lp < powr; lp += 1) گیڑا *= 10;

      powr = scope->ویلا_precision - vpip_get_ویلا_precision();
      scale_ = 1;
      لكل (صحيح lp = 0; lp < powr; lp += 1) scale_ *= 10;
}

vvp_fun_delay::~vvp_fun_delay()
{
      طالما (بنية event_*cur = قطار_توں_کڈھو())
	    مٹاؤ cur;
}

منطقي vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        ثابت vvp_vector4_t&ثنائی_ہندسہ)
{
      إذا (list_ == 0) أرجع خطأ;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      إذا (list_->اگلا_2->ptr_vec4.eeq(ثنائی_ہندسہ)) أرجع سچ;

      clean_pulse_events_(use_delay);
      أرجع خطأ;
}

منطقي vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        ثابت vvp_vector8_t&ثنائی_ہندسہ)
{
      إذا (list_ == 0) أرجع خطأ;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      إذا (list_->اگلا_2->ptr_vec8.eeq(ثنائی_ہندسہ)) أرجع سچ;

      clean_pulse_events_(use_delay);
      أرجع خطأ;
}

منطقي vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        مزدوج ثنائی_ہندسہ)
{
      إذا (list_ == 0) أرجع خطأ;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      إذا (list_->اگلا_2->ptr_real == ثنائی_ہندسہ) أرجع سچ;

      clean_pulse_events_(use_delay);
      أرجع خطأ;
}

فراغ vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay)
{
      أكد(list_ != 0);

      افعل {
	    بنية event_*cur = list_->اگلا_2;
	      /* If this event is far enough from the event I'm about
	         to create, then that scheduled event is not a pulse
	         to be eliminated, so we're done. */
	    إذا (cur->sim_ویلا+use_delay <= use_delay+ویلا_سارنی_simtime())
		  اكسر;

	    إذا (list_ == cur)
		  list_ = 0;
	    وإلا
		  list_->اگلا_2 = cur->اگلا_2;
	    مٹاؤ cur;
      } طالما (list_);
}

/*
 * FIXME: this implementation currently only uses the maximum delay
 * from all the bit changes in the vectors. If there are multiple
 * changes with different delays, then the results would be
 * wrong. What should happen is that if there are multiple changes,
 * multiple vectors approaching the result should be scheduled.
 */
فراغ vvp_fun_delay::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                              vvp_context_t)
{
      إذا (پورٹ.پورٹ() > 0) {
	      // Get the integer value of the bit vector, or 0 if
	      // there are X or Z bits.
	    vvp_time64_t bval = 0;
	      // The following does not work correctly for negative values.
	      // They should be sign extended to 64 bits (1364-2001 9.7.1).
	    vector4_منزل_قدر(ثنائی_ہندسہ, bval);
	      // Integer values do not need to be rounded so just scale them.
	    vvp_time64_t val = bval * گیڑا * scale_;

	    بدل (پورٹ.پورٹ()) {
		حالة 1:
		  delay_.سیٹ_rise(val);
		  أرجع;
		حالة 2:
		  delay_.سیٹ_fall(val);
		  أرجع;
		حالة 3:
		  delay_.سیٹ_decay(val);
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
	    use_delay = delay_.get_delay(cur_val, ثنائی_ہندسہ.value(0));
	    لكل (بنانشانی idx = 1 ;  idx < ثنائی_ہندسہ.size() ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(cur_val, ثنائی_ہندسہ.value(idx));
		  إذا (tmp > use_delay) use_delay = tmp;
	    }
      } وإلا {
	    أكد(type_ == VEC4_DELAY);

	      // Use as a reference for calculating the delay the
	      // current value of the output. Detect and handle the
	      // special case that the event list contains the current
	      // value as a zero-delay-remaining event.
	    ثابت vvp_vector4_t&use_vec4 = (list_ && list_->اگلا_2->sim_ویلا == ویلا_سارنی_simtime())? list_->اگلا_2->ptr_vec4 : cur_vec4_;

	      /* How many bits to compare? */
	    بنانشانی use_wid = use_vec4.size();
	    إذا (ثنائی_ہندسہ.size() < use_wid) use_wid = ثنائی_ہندسہ.size();

	      /* Scan the vectors looking for delays. Select the maximum
	         delay encountered. */
	    use_delay = delay_.get_delay(use_vec4.value(0), ثنائی_ہندسہ.value(0));

	    لكل (بنانشانی idx = 1 ;  idx < use_wid ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(use_vec4.value(idx), ثنائی_ہندسہ.value(idx));
		  إذا (tmp > use_delay) use_delay = tmp;
	    }
      }

      /* what *should* happen here is we check to see if there is a
         transaction in the queue. This would be a pulse that needs to be
         eliminated. */
      إذا (clean_pulse_events_(use_delay, ثنائی_ہندسہ)) أرجع;

      vvp_time64_t use_simtime = ویلا_سارنی_simtime() + use_delay;

	/* And propagate it. */
      إذا (use_delay == 0 && list_ == 0) {
	    cur_vec4_ = ثنائی_ہندسہ;
	    initial_ = خطأ;
	    net_->send_vec4(cur_vec4_, 0);
      } وإلا {
	    بنية event_*cur = جديد بنية event_(use_simtime);
	    cur->لگاتار_لگاتار_ptr = &vvp_fun_delay::لگاتار_لگاتار_vec4;
	    cur->ptr_vec4 = ثنائی_ہندسہ;
	    قطار_وچ_پاؤ(cur);
	    ویلا_سارنی_generic(هذا, use_delay, خطأ);
      }
}

فراغ vvp_fun_delay::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			         بنانشانی ادھار, بنانشانی vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ثنائی_ہندسہ, ادھار, vwid, ctx);
}

/* See the recv_vec4 comment above. */
فراغ vvp_fun_delay::recv_vec8(vvp_net_ptr_t پورٹ, ثابت vvp_vector8_t&ثنائی_ہندسہ)
{
      أكد(پورٹ.پورٹ() == 0);

      vvp_time64_t use_delay;
	/* This is an initial value so it needs to be compared to all the
	   bits (the order the bits are changed is not deterministic). */
      إذا (initial_) {
	    type_ = VEC8_DELAY;
            cur_vec4_ = vvp_vector4_t(0, BIT4_X);
	    vvp_bit4_t cur_val = cur_vec8_.value(0).value();
	    use_delay = delay_.get_delay(cur_val, ثنائی_ہندسہ.value(0).value());
	    لكل (بنانشانی idx = 1 ;  idx < ثنائی_ہندسہ.size() ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(cur_val, ثنائی_ہندسہ.value(idx).value());
		  إذا (tmp > use_delay) use_delay = tmp;
	    }
      } وإلا {
	    أكد(type_ == VEC8_DELAY);

	      // Use as a reference for calculating the delay the
	      // current value of the output. Detect and handle the
	      // special case that the event list contains the current
	      // value as a zero-delay-remaining event.
	    ثابت vvp_vector8_t&use_vec8 = (list_ && list_->اگلا_2->sim_ویلا == ویلا_سارنی_simtime())? list_->اگلا_2->ptr_vec8 : cur_vec8_;

	      /* How many bits to compare? */
	    بنانشانی use_wid = use_vec8.size();
	    إذا (ثنائی_ہندسہ.size() < use_wid) use_wid = ثنائی_ہندسہ.size();

	      /* Scan the vectors looking for delays. Select the maximum
	         delay encountered. */
	    use_delay = delay_.get_delay(use_vec8.value(0).value(),
	                                 ثنائی_ہندسہ.value(0).value());

	    لكل (بنانشانی idx = 1 ;  idx < use_wid ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(use_vec8.value(idx).value(),
		                         ثنائی_ہندسہ.value(idx).value());
		  إذا (tmp > use_delay) use_delay = tmp;
	    }
      }

      /* what *should* happen here is we check to see if there is a
         transaction in the queue. This would be a pulse that needs to be
         eliminated. */
      إذا (clean_pulse_events_(use_delay, ثنائی_ہندسہ)) أرجع;

      vvp_time64_t use_simtime = ویلا_سارنی_simtime() + use_delay;

	/* And propagate it. */
      إذا (use_delay == 0 && list_ == 0) {
	    cur_vec8_ = ثنائی_ہندسہ;
	    initial_ = خطأ;
	    net_->send_vec8(cur_vec8_);
      } وإلا {
	    بنية event_*cur = جديد بنية event_(use_simtime);
	    cur->ptr_vec8 = ثنائی_ہندسہ;
	    cur->لگاتار_لگاتار_ptr = &vvp_fun_delay::لگاتار_لگاتار_vec8;
	    قطار_وچ_پاؤ(cur);
	    ویلا_سارنی_generic(هذا, use_delay, خطأ);
      }
}

فراغ vvp_fun_delay::recv_vec8_pv(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&ثنائی_ہندسہ,
			         بنانشانی ادھار, بنانشانی vwid)
{
      recv_vec8_pv_(ptr, ثنائی_ہندسہ, ادھار, vwid);
}

فراغ vvp_fun_delay::recv_real(vvp_net_ptr_t پورٹ, مزدوج ثنائی_ہندسہ,
                              vvp_context_t)
{
      إذا (پورٹ.پورٹ() > 0) {
	    /* If the port is not 0, then this is a delay value that
	    should be rounded and converted to an integer delay. */
	    vvp_time64_t val = 0;
	    إذا (ثنائی_ہندسہ > -0.5) {
		  val = (vvp_time64_t) (ثنائی_ہندسہ * گیڑا + 0.5) * scale_;
	    } وإلا إذا (ثنائی_ہندسہ != ثنائی_ہندسہ) {
		    // For a NaN we use the default (0).
	    } وإلا {
		  vvp_vector4_t vec4(8*حجم(vvp_time64_t),
		                     floor(-ثنائی_ہندسہ * گیڑا + 0.5) * -1 * scale_);
		  vector4_منزل_قدر(vec4, val);
	    }

	    بدل (پورٹ.پورٹ()) {
		حالة 1:
		  delay_.سیٹ_rise(val);
		  أرجع;
		حالة 2:
		  delay_.سیٹ_fall(val);
		  أرجع;
		حالة 3:
		  delay_.سیٹ_decay(val);
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
      إذا (clean_pulse_events_(use_delay, ثنائی_ہندسہ)) أرجع;

      /* This must be done after cleaning pulses to avoid propagating
       * an incorrect value. */
      إذا (cur_real_ == ثنائی_ہندسہ) أرجع;

      vvp_time64_t use_simtime = ویلا_سارنی_simtime() + use_delay;

      إذا (use_delay == 0 && list_ == 0) {
	    cur_real_ = ثنائی_ہندسہ;
	    initial_ = خطأ;
	    net_->send_real(cur_real_, 0);
      } وإلا {
	    بنية event_*cur = جديد بنية event_(use_simtime);
	    cur->لگاتار_لگاتار_ptr = &vvp_fun_delay::لگاتار_لگاتار_real;
	    cur->ptr_real = ثنائی_ہندسہ;
	    قطار_وچ_پاؤ(cur);

	    ویلا_سارنی_generic(هذا, use_delay, خطأ);
      }
}

فراغ vvp_fun_delay::لگاتار_لگاتار()
{
      vvp_time64_t sim_ویلا = ویلا_سارنی_simtime();
      إذا (list_ == 0 || list_->اگلا_2->sim_ویلا > sim_ویلا)
	    أرجع;

      بنية event_*cur = قطار_توں_کڈھو();
      إذا (cur == 0)
	    أرجع;

      (هذا->*(cur->لگاتار_لگاتار_ptr))(cur);
      initial_ = خطأ;
      مٹاؤ cur;
}

فراغ vvp_fun_delay::لگاتار_لگاتار_vec4(ثابت بنية event_*cur)
{
      cur_vec4_ = cur->ptr_vec4;
      net_->send_vec4(cur_vec4_, 0);
}

فراغ vvp_fun_delay::لگاتار_لگاتار_vec8(ثابت بنية vvp_fun_delay::event_*cur)
{
      cur_vec8_ = cur->ptr_vec8;
      net_->send_vec8(cur_vec8_);
}

فراغ vvp_fun_delay::لگاتار_لگاتار_real(ثابت بنية vvp_fun_delay::event_*cur)
{
      cur_real_ = cur->ptr_real;
      net_->send_real(cur_real_, 0);
}

vvp_fun_modpath::vvp_fun_modpath(vvp_net_t*net, بنانشانی width)
: net_(net), src_list_(0), ifnone_list_(0)
{
      cur_vec4_ = vvp_vector4_t(width, BIT4_X);
      ویلا_سارنی_init_propagate(net_, cur_vec4_);
}

vvp_fun_modpath::~vvp_fun_modpath()
{
	// Delete the source probes.
      طالما (src_list_) {
	    vvp_fun_modpath_src*tmp = src_list_;
	    src_list_ = tmp->اگلا;
	    مٹاؤ tmp;
      }
      طالما (ifnone_list_) {
	    vvp_fun_modpath_src*tmp = ifnone_list_;
	    ifnone_list_ = tmp->اگلا;
	    مٹاؤ tmp;
      }
}

فراغ vvp_fun_modpath::جوڑو_modpath_src(vvp_fun_modpath_src*that, منطقي ifnone)
{
      أكد(that->اگلا == 0);
      إذا (ifnone) {
	    that->اگلا = ifnone_list_;
	    ifnone_list_ = that;
      } وإلا {
	    that->اگلا = src_list_;
	    src_list_ = that;
      }
}

ساكن vvp_time64_t delay_from_کنارا(vvp_bit4_t a, vvp_bit4_t b,
                                           vvp_time64_t ثابت array[12])
{
      تعريفنوع delay_کنارا_t bit4_table4[4];
      ساكن ثابت bit4_table4 کنارا_جدول[4] = {
	    { DELAY_کنارا_01, DELAY_کنارا_01, DELAY_کنارا_0z, DELAY_کنارا_0x },
	    { DELAY_کنارا_10, DELAY_کنارا_10, DELAY_کنارا_1z, DELAY_کنارا_1x },
	    { DELAY_کنارا_z0, DELAY_کنارا_z1, DELAY_کنارا_z0, DELAY_کنارا_zx },
	    { DELAY_کنارا_x0, DELAY_کنارا_x1, DELAY_کنارا_xz, DELAY_کنارا_x0 }
      };

      أرجع array[ کنارا_جدول[a][b] ];
}

فراغ vvp_fun_modpath::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                vvp_context_t)
{
	/* Only the first port is used. */
      إذا (پورٹ.پورٹ() > 0)
	    أرجع;

      إذا (cur_vec4_.eeq(ثنائی_ہندسہ))
	    أرجع;

	/* Select a time delay source that applies. Notice that there
	   may be multiple delay sources that apply, so collect all
	   the candidates into a list first. */
      list<vvp_fun_modpath_src*>candidate_list;
      vvp_time64_t candidate_wake_ویلا = 0;
      لكل (vvp_fun_modpath_src*cur = src_list_ ;  cur ;  cur=cur->اگلا) {
	      /* Skip paths that are disabled by conditions. */
	    إذا (cur->condition_flag_ == خطأ)
		  استمر;

	    إذا (candidate_list.empty()) {
		  candidate_list.push_back(cur);
		  candidate_wake_ویلا = cur->wake_ویلا;
	    } وإلا إذا (cur->wake_ویلا == candidate_wake_ویلا) {
		  candidate_list.push_back(cur);
	    } وإلا إذا (cur->wake_ویلا > candidate_wake_ویلا) {
		  candidate_list.assign(1, cur);
		  candidate_wake_ویلا = cur->wake_ویلا;
	    } وإلا {
		  استمر; /* Skip this entry. */
	    }
      }

	/* Only add the ifnone delay if it has a later wake_time_ or
	 * if there are no normal delays. */
      vvp_time64_t ifnone_wake_ویلا = candidate_wake_ویلا;
      لكل (vvp_fun_modpath_src*cur = ifnone_list_ ;  cur ;  cur=cur->اگلا) {
	    إذا (candidate_list.empty()) {
		  candidate_list.push_back(cur);
		  ifnone_wake_ویلا = cur->wake_ویلا;
	    } وإلا إذا (cur->wake_ویلا == ifnone_wake_ویلا &&
	               ifnone_wake_ویلا > candidate_wake_ویلا) {
		  candidate_list.push_back(cur);
	    } وإلا إذا (cur->wake_ویلا > ifnone_wake_ویلا) {
		  candidate_list.assign(1, cur);
		  ifnone_wake_ویلا = cur->wake_ویلا;
	    } وإلا {
		  استمر; /* Skip this entry. */
	    }
      }

	/* Handle the special case that there are no delays that
	   match. This may happen, for example, if the set of
	   conditional delays is incomplete, leaving some cases
	   uncovered. In that case, just pass the data without delay */
      إذا (candidate_list.empty()) {
	    cur_vec4_ = ثنائی_ہندسہ;
	    ویلا_سارنی_generic(هذا, 0, خطأ);
	    أرجع;
      }

	/* Now given that we have a list of candidate delays, find for
	   each if the 12 numbers the minimum from all the
	   candidates. This minimum set becomes the chosen delay to
	   use. */
      vvp_time64_t باہرلا_at[12];
      vvp_time64_t now = ویلا_سارنی_simtime();

      تعريفنوع list<vvp_fun_modpath_src*>::const_iterator iter_t;

      iter_t cur = candidate_list.begin();
      ثابت vvp_fun_modpath_src*src = *cur;

      لكل (بنانشانی idx = 0 ;  idx < 12 ;  idx += 1) {
	    باہرلا_at[idx] = src->wake_ویلا + src->delay_[idx];
	    إذا (باہرلا_at[idx] <= now)
		  باہرلا_at[idx] = 0;
	    وإلا
		  باہرلا_at[idx] -= now;
      }

      لكل (++ cur ; cur != candidate_list.سرا() ; ++ cur ) {
	    src = *cur;
	    لكل (بنانشانی idx = 0 ;  idx < 12 ;  idx += 1) {
		  vvp_time64_t tmp = src->wake_ویلا + src->delay_[idx];
		  إذا (tmp <= now)
			tmp = 0;
		  وإلا
			tmp -= now;
		  إذا (tmp < باہرلا_at[idx])
			باہرلا_at[idx] = tmp;
	    }
      }

	/* Given the scheduled output time, create an output event. */
      vvp_time64_t use_delay = delay_from_کنارا(cur_vec4_.value(0),
					       ثنائی_ہندسہ.value(0),
					       باہرلا_at);

	/* FIXME: This bases the edge delay on only the least
	   bit. This is WRONG! I need to find all the possible delays,
	   and schedule an event for each partial change. Hard! */
      لكل (بنانشانی idx = 1 ;  idx < ثنائی_ہندسہ.size() ;  idx += 1) {
	    vvp_time64_t tmp = delay_from_کنارا(cur_vec4_.value(idx),
					       ثنائی_ہندسہ.value(idx),
					       باہرلا_at);
	      /* If the current and new bit values match then no delay
	       * is needed for this bit. */
	    إذا (cur_vec4_.value(idx) == ثنائی_ہندسہ.value(idx)) استمر;
	    إذا (tmp != use_delay) {
		  fprintf(stderr, "sorry: multi-bit module path delays are "
		                  "currently not fully supported.\n");
		  fprintf(stderr, "     : using the LSB delay for all bits.\n");
		  استمر;
            }
      }

      cur_vec4_ = ثنائی_ہندسہ;
      ویلا_سارنی_generic(هذا, use_delay, خطأ);
}

فراغ vvp_fun_modpath::لگاتار_لگاتار()
{
      net_->send_vec4(cur_vec4_, 0);
}

vvp_fun_modpath_src::vvp_fun_modpath_src(vvp_time64_t ثابت del[12])
{
      لكل (بنانشانی idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = del[idx];

      اگلا = 0;
      wake_ویلا = 0;
      condition_flag_ = سچ;
}

vvp_fun_modpath_src::~vvp_fun_modpath_src()
{
}

فراغ vvp_fun_modpath_src::get_delay12(vvp_time64_t val[12]) ثابت
{
      لكل (بنانشانی idx = 0 ;  idx < 12 ;  idx += 1)
	    val[idx] = delay_[idx];
}

فراغ vvp_fun_modpath_src::put_delay12(ثابت vvp_time64_t val[12])
{
      لكل (بنانشانی idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = val[idx];
}

فراغ vvp_fun_modpath_src::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                    vvp_context_t)
{
      إذا (پورٹ.پورٹ() == 0) {
	      // The modpath input...
	    إذا (test_vec4(ثنائی_ہندسہ))
		  wake_ویلا = ویلا_سارنی_simtime();

      } وإلا إذا (پورٹ.پورٹ() == 1) {
	      // The modpath condition input...
	    إذا (ثنائی_ہندسہ.value(0) != BIT4_0)
		  condition_flag_ = سچ;
	    وإلا
		  condition_flag_ = خطأ;
      }
}

منطقي vvp_fun_modpath_src::test_vec4(ثابت vvp_vector4_t&)
{
      أرجع سچ;
}

vvp_fun_modpath_کنارا::vvp_fun_modpath_کنارا(vvp_time64_t del[12],
					   منطقي pos, منطقي neg)
: vvp_fun_modpath_src(del)
{
      old_قدر = BIT4_X;
      posedge_ = pos;
      negedge_ = neg;
}

منطقي vvp_fun_modpath_کنارا::test_vec4(ثابت vvp_vector4_t&ثنائی_ہندسہ)
{
      vvp_bit4_t tmp = old_قدر;
      old_قدر = ثنائی_ہندسہ.value(0);

      صحيح کنارا_flag = کنارا(tmp, old_قدر);
      إذا (کنارا_flag > 0) أرجع posedge_;
      إذا (کنارا_flag < 0) أرجع negedge_;
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
		طالما (scope && scope->get_type_کوڈ() != vpiModule)
		      scope = scope->scope;
		أكد(scope);
		أرجع scope;
	      }

	    // Handles to path term objects should really be obtained via
	    // the vpi_iterate and vpi_scan functions. Continue to allow
	    // them to be obtained here for backwards compatibility with
	    // older versions of Icarus Verilog.

	  حالة vpi_Mod_رستہ_وچ:
	    أرجع &rfp->رستہ_رکن_وچ;

	  حالة vpi_Mod_رستہ_باہرلا:
	    أرجع &rfp->dest->رستہ_رکن_باہرلا;
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
	  حالة vpi_Mod_رستہ_وچ: {
	    vpiHandle*دلیلاں = static_cast<vpiHandle*>(صفر_حافظہ_دیو(1, حجم(vpiHandle*)));
	    دلیلاں[0] = &rfp->رستہ_رکن_وچ;
	    أرجع vpip_make_iterator(1, دلیلاں, سچ);
	  }
	  حالة vpi_Mod_رستہ_باہرلا: {
	    vpiHandle*دلیلاں = static_cast<vpiHandle*>(صفر_حافظہ_دیو(1, حجم(vpiHandle*)));
	    دلیلاں[0] = &rfp->dest->رستہ_رکن_باہرلا;
	    أرجع vpip_make_iterator(1, دلیلاں, سچ);
	  }
      }
      أرجع 0;
}

ساكن vpiHandle modpath_src_اشاریہ ( vpiHandle ref, صحيح)
{
      أكد(ref->get_type_کوڈ() == vpi_Mod_رستہ_وچ);
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

      تعريفنوع بنانشانی حرف map_array_t[12];
	// Only the first six entries are used for the less than twelve maps.
      ساكن ثابت map_array_t map_1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      ساكن ثابت map_array_t map_2 = {0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0};
      ساكن ثابت map_array_t map_3 = {0, 1, 2, 0, 2, 1, 0, 0, 0, 0, 0, 0};
      ساكن ثابت map_array_t map_6 = {0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0};
      ساكن ثابت map_array_t map12 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

      ثابت map_array_t*use_map = 0;
      بدل (delays->no_دا_delays) {
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

      إذا (delays->ویلا_type == vpi_Sim_ویلا) {
	    لكل (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_timestruct_منزل_ویلا(delays->da+use_map[0][idx]);
	    }
      } وإلا {
	      // You cannot create a modpath with a negative delay so set it
	      // to zero per 1364-2005 section 14.3.1.
	    لكل (idx = 0 ; idx < delays->no_دا_delays ; idx += 1) {
		  إذا (delays->da[idx].real < 0.0) delays->da[idx].real = 0.0;
	    }
	    لكل (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_scaled_real_منزل_time64(delays->da[use_map[0][idx]].real,
							src->dest->scope);
	    }
      }

      /* Now define the to-from-x delays if needed. */
      إذا (delays->no_دا_delays <= 6) {
	      /* 0->x is the minimum of 0->z and 0->1. */
	    tmp[DELAY_کنارا_0x] = tmp[DELAY_کنارا_0z] < tmp[DELAY_کنارا_01] ?
	                           tmp[DELAY_کنارا_0z] : tmp[DELAY_کنارا_01];
	      /* x->1 is the maximum of z->1 and 0->1. */
	    tmp[DELAY_کنارا_x1] = tmp[DELAY_کنارا_z1] > tmp[DELAY_کنارا_01] ?
	                           tmp[DELAY_کنارا_z1] : tmp[DELAY_کنارا_01];
	      /* 1->x is the minimum of 1->z and 1->0. */
	    tmp[DELAY_کنارا_1x] = tmp[DELAY_کنارا_1z] < tmp[DELAY_کنارا_10] ?
	                           tmp[DELAY_کنارا_1z] : tmp[DELAY_کنارا_10];
	      /* x->0 is the maximum of z->0 and 1->0. */
	    tmp[DELAY_کنارا_x0] = tmp[DELAY_کنارا_z0] > tmp[DELAY_کنارا_10] ?
	                           tmp[DELAY_کنارا_z0] : tmp[DELAY_کنارا_10];
	      /* x->z is the maximum of 1->z and 0->z. */
	    tmp[DELAY_کنارا_xz] = tmp[DELAY_کنارا_1z] > tmp[DELAY_کنارا_0z] ?
	                           tmp[DELAY_کنارا_1z] : tmp[DELAY_کنارا_0z];
	      /* z->x is the minimum of z->1 and z->0. */
	    tmp[DELAY_کنارا_zx] = tmp[DELAY_کنارا_z1] < tmp[DELAY_کنارا_z0] ?
	                           tmp[DELAY_کنارا_z1] : tmp[DELAY_کنارا_z0];
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

      بدل (delays->no_دا_delays) {
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

      إذا (delays->ویلا_type == vpi_Sim_ویلا) {
	    لكل (idx = 0; idx < delays->no_دا_delays; idx += 1) {
		  vpip_ویلا_منزل_timestruct(delays->da+idx, tmp[idx]);
	    }
      } وإلا {
	    لكل (idx = 0; idx < delays->no_دا_delays; idx += 1) {
		  delays->da[idx].real = vpip_ویلا_منزل_scaled_real(tmp[idx], src->dest->scope);
	    }
      }
}

ساكن صحيح pathterm_get(صحيح code, vpiHandle ref)
{
      بنية __vpiModPathTerm*obj = dynamic_cast<__vpiModPathTerm*>(ref);
      أكد(obj);

      بدل (code) {
	  حالة vpi_کنارا_2:
	    أرجع obj->کنارا;
	  افتراضي:
	    أرجع 0;
      }
}

ساكن vpiHandle pathterm_get_handle(صحيح code, vpiHandle ref)
{
      بنية __vpiModPathTerm*obj = dynamic_cast<__vpiModPathTerm*>(ref);
      أكد(obj);

      بدل (code) {
	  حالة vpi_اظہاریہ:
	    أرجع obj->اظہاریہ;
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

صحيح __vpiModPathSrc::get_type_کوڈ(فراغ) ثابت
{ أرجع vpi_Mod_رستہ; }

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

vpiHandle __vpiModPathSrc:: vpi_اشاریہ(صحيح idx)
{ أرجع modpath_src_اشاریہ(هذا, idx); }

فراغ __vpiModPathSrc::vpi_get_delays(p_vpi_delay del)
{ modpath_src_get_delays(هذا, del); }

فراغ __vpiModPathSrc::vpi_put_delays(p_vpi_delay del)
{ modpath_src_put_delays(هذا, del); }

ساكن صحيح modpath_src_free_object( vpiHandle ref )
{
      مٹاؤ ref;
      أرجع 1 ;
}

__vpiHandle::free_object_fun_t __vpiModPathSrc::free_object_fun(فراغ)
{ أرجع &modpath_src_free_object; }


مضمن __vpiModPathTerm::__vpiModPathTerm()
{ }

صحيح __vpiModPathTerm::get_type_کوڈ(فراغ) ثابت
{ أرجع vpi_رستہ_رکن; }

صحيح __vpiModPathTerm::vpi_get(صحيح code)
{ أرجع pathterm_get(code, هذا); }

vpiHandle __vpiModPathTerm::vpi_handle(صحيح code)
{ أرجع pathterm_get_handle(code, هذا); }

ساكن فراغ initialize_رستہ_رکن(بنية __vpiModPathTerm&obj)
{
      obj.اظہاریہ = 0;
      obj.کنارا = vpi_No_کنارا;
}

/*
 * This function will construct a vpiModPath Object.
 * give a respective "net", and will point to his
 * respective functor
 */

#جے_تعریف_شدہ CHECK_WITH_VALGRIND
ساكن بنية __vpiModPath**mp_list = 0;
ساكن بنانشانی mp_گنتی = 0;
#جے_انت

بنية __vpiModPath* vpip_make_modpath(vvp_net_t *net)
{
      بنية __vpiModPath*obj = جديد __vpiModPath;
      obj->scope = vpip_peek_current_scope ( );

      initialize_رستہ_رکن(obj->رستہ_رکن_باہرلا);
      obj->داخلہ_net = net ;

#جے_تعریف_شدہ CHECK_WITH_VALGRIND
      mp_گنتی += 1;
      mp_list = static_cast<بنية __vpiModPath **>
                (realloc(mp_list,
                 mp_گنتی*حجم(بنية __vpiModPath **)));
      mp_list[mp_گنتی-1] = obj;
#جے_انت
      أرجع obj;
}

#جے_تعریف_شدہ CHECK_WITH_VALGRIND
فراغ modpath_delete()
{
      لكل (بنانشانی idx = 0; idx < mp_گنتی; idx += 1) {
	    مٹاؤ mp_list[idx];
      }
      حرر(mp_list);
      mp_list = 0;
      mp_گنتی = 0;
}
#جے_انت

/*
 * This function will construct a vpiModPathIn
 * ( struct __vpiModPathSrc ) Object. will give
 * a delays[12] values, and point to the specified functor
 *
 */

بنية __vpiModPathSrc* vpip_make_modpath_src(بنية __vpiModPath*رستہ,
                                              vvp_net_t *net)
{
      بنية __vpiModPathSrc *obj = جديد __vpiModPathSrc;

      obj->dest = رستہ;
      obj->type = 0;
      obj->net = net;
      initialize_رستہ_رکن(obj->رستہ_رکن_وچ);

      أرجع obj;
}

vvp_fun_intermodpath::vvp_fun_intermodpath(vvp_net_t*net, بنانشانی width)
: net_(net)
{
      لكل (بنانشانی idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = 0;

      cur_vec4_ = vvp_vector4_t(width, BIT4_X);
      ویلا_سارنی_init_propagate(net_, cur_vec4_);
}

vvp_fun_intermodpath::~vvp_fun_intermodpath()
{
}

فراغ vvp_fun_intermodpath::get_delay12(vvp_time64_t val[12]) ثابت
{
      لكل (بنانشانی idx = 0 ;  idx < 12 ;  idx += 1)
	    val[idx] = delay_[idx];
}

فراغ vvp_fun_intermodpath::put_delay12(ثابت vvp_time64_t val[12])
{
      لكل (بنانشانی idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = val[idx];
}

/*
 * FIXME: this implementation currently only uses the maximum delay
 * from all the bit changes in the vectors. If there are multiple
 * changes with different delays, then the results would be
 * wrong. What should happen is that if there are multiple changes,
 * multiple vectors approaching the result should be scheduled.
 */
فراغ vvp_fun_intermodpath::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                              vvp_context_t)
{
	/* Only the first port is used. */
      إذا (پورٹ.پورٹ() > 0)
	    أرجع;

      إذا (cur_vec4_.eeq(ثنائی_ہندسہ))
	    أرجع;

	/* Given the scheduled output time, create an output event. */
      vvp_time64_t use_delay = delay_from_کنارا(cur_vec4_.value(0),
					       ثنائی_ہندسہ.value(0),
					       delay_);

	/* FIXME: This bases the edge delay on only the least
	   bit. This is WRONG! I need to find all the possible delays,
	   and schedule an event for each partial change. Hard! */
      لكل (بنانشانی idx = 1 ;  idx < ثنائی_ہندسہ.size() ;  idx += 1) {
	    vvp_time64_t tmp = delay_from_کنارا(cur_vec4_.value(idx),
					       ثنائی_ہندسہ.value(idx),
					       delay_);
	      /* If the current and new bit values match then no delay
	       * is needed for this bit. */
	    إذا (cur_vec4_.value(idx) == ثنائی_ہندسہ.value(idx)) استمر;
	    أكد(tmp == use_delay);
      }

      cur_vec4_ = ثنائی_ہندسہ;
      ویلا_سارنی_generic(هذا, use_delay, خطأ);
}

فراغ vvp_fun_intermodpath::لگاتار_لگاتار()
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
		طالما (scope && scope->get_type_کوڈ() != vpiModule)
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
	  حالة vpi_پورٹ: {
	    vpiHandle*دلیلاں = static_cast<vpiHandle*>(صفر_حافظہ_دیو(2, حجم(vpiHandle*)));
	    دلیلاں[0] = rfp->port1;
	    دلیلاں[1] = rfp->port2;
	    أرجع vpip_make_iterator(2, دلیلاں, سچ);
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

      تعريفنوع بنانشانی حرف map_array_t[12];
	// Only the first six entries are used for the less than twelve maps.
      ساكن ثابت map_array_t map_1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      ساكن ثابت map_array_t map_2 = {0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0};
      ساكن ثابت map_array_t map_3 = {0, 1, 2, 0, 2, 1, 0, 0, 0, 0, 0, 0};
      ساكن ثابت map_array_t map_6 = {0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0};
      ساكن ثابت map_array_t map12 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

      ثابت map_array_t*use_map = 0;
      بدل (delays->no_دا_delays) {
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

      إذا (delays->ویلا_type == vpi_Sim_ویلا) {
	    لكل (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_timestruct_منزل_ویلا(delays->da+use_map[0][idx]);
	    }
      } وإلا {
	      // You cannot create a modpath with a negative delay so set it
	      // to zero per 1364-2005 section 14.3.1.
	    لكل (idx = 0 ; idx < delays->no_دا_delays ; idx += 1) {
		  إذا (delays->da[idx].real < 0.0) delays->da[idx].real = 0.0;
	    }
	    لكل (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_scaled_real_منزل_time64(delays->da[use_map[0][idx]].real,
							src->scope);
	    }
      }

      /* Now define the to-from-x delays if needed. */
      إذا (delays->no_دا_delays <= 6) {
	      /* 0->x is the minimum of 0->z and 0->1. */
	    tmp[DELAY_کنارا_0x] = tmp[DELAY_کنارا_0z] < tmp[DELAY_کنارا_01] ?
	                           tmp[DELAY_کنارا_0z] : tmp[DELAY_کنارا_01];
	      /* x->1 is the maximum of z->1 and 0->1. */
	    tmp[DELAY_کنارا_x1] = tmp[DELAY_کنارا_z1] > tmp[DELAY_کنارا_01] ?
	                           tmp[DELAY_کنارا_z1] : tmp[DELAY_کنارا_01];
	      /* 1->x is the minimum of 1->z and 1->0. */
	    tmp[DELAY_کنارا_1x] = tmp[DELAY_کنارا_1z] < tmp[DELAY_کنارا_10] ?
	                           tmp[DELAY_کنارا_1z] : tmp[DELAY_کنارا_10];
	      /* x->0 is the maximum of z->0 and 1->0. */
	    tmp[DELAY_کنارا_x0] = tmp[DELAY_کنارا_z0] > tmp[DELAY_کنارا_10] ?
	                           tmp[DELAY_کنارا_z0] : tmp[DELAY_کنارا_10];
	      /* x->z is the maximum of 1->z and 0->z. */
	    tmp[DELAY_کنارا_xz] = tmp[DELAY_کنارا_1z] > tmp[DELAY_کنارا_0z] ?
	                           tmp[DELAY_کنارا_1z] : tmp[DELAY_کنارا_0z];
	      /* z->x is the minimum of z->1 and z->0. */
	    tmp[DELAY_کنارا_zx] = tmp[DELAY_کنارا_z1] < tmp[DELAY_کنارا_z0] ?
	                           tmp[DELAY_کنارا_z1] : tmp[DELAY_کنارا_z0];
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

      بدل (delays->no_دا_delays) {
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

      إذا (delays->ویلا_type == vpi_Sim_ویلا) {
	    لكل (idx = 0; idx < delays->no_دا_delays; idx += 1) {
		  vpip_ویلا_منزل_timestruct(delays->da+idx, tmp[idx]);
	    }
      } وإلا {
	    لكل (idx = 0; idx < delays->no_دا_delays; idx += 1) {
		  delays->da[idx].real = vpip_ویلا_منزل_scaled_real(tmp[idx], src->scope);
	    }
      }
}

/*
* The __vpiInterModPath class is what the VPI client sees as a
* vpiInterModPath object.
*/
مضمن __vpiInterModPath::__vpiInterModPath()
{ }

صحيح __vpiInterModPath::get_type_کوڈ(فراغ) ثابت
{ أرجع vpi_Inter_Mod_رستہ; }

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
      مٹاؤ ref;
      أرجع 1 ;
}

__vpiHandle::free_object_fun_t __vpiInterModPath::free_object_fun(فراغ)
{ أرجع &intermodpath_free_object; }

/*
 * This function will construct a vpiInterModPath Object.
 * give a respective "net", and will point to his
 * respective functor
 */

#جے_تعریف_شدہ CHECK_WITH_VALGRIND
ساكن بنية __vpiInterModPath**imp_list = 0;
ساكن بنانشانی imp_گنتی = 0;
#جے_انت

بنية __vpiInterModPath* vpip_make_intermodpath(vvp_net_t *net, vpi_پورٹ_Info* port1, vpi_پورٹ_Info* port2)
{
      بنية __vpiInterModPath*obj = جديد __vpiInterModPath;
      obj->scope = vpip_peek_current_scope ( );

      obj->net = net;
      obj->port1 = port1;
      obj->port2 = port2;

#جے_تعریف_شدہ CHECK_WITH_VALGRIND
      imp_گنتی += 1;
      imp_list = static_cast<بنية __vpiInterModPath **>
                 (realloc(imp_list,
                  imp_گنتی*حجم(بنية __vpiInterModPath **)));
      imp_list[imp_گنتی-1] = obj;
#جے_انت
      أرجع obj;
}

#جے_تعریف_شدہ CHECK_WITH_VALGRIND
فراغ intermodpath_delete()
{
      لكل (بنانشانی idx = 0; idx < mp_گنتی; idx += 1) {
	    مٹاؤ imp_list[idx];
      }
      حرر(imp_list);
      imp_list = 0;
      imp_گنتی = 0;
}
#جے_انت
