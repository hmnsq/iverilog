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

#شامل "delay.h"
#شامل "schedule.h"
#شامل "vpi_priv.h"
#شامل "config.h"
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
#شامل "vvp_cleanup.h"
#اگر_ختم
#شامل <iostream>
#شامل <cstdlib>
#شامل <list>
#شامل <cassert>
#شامل <cmath>
#شامل "ivl_alloc.h"

استعمال نامخلا std;

خالی vvp_delay_t::calculate_min_delay_()
{
      min_delay_ = rise_;
      اگر (fall_ < min_delay_)
	    min_delay_ = fall_;
      اگر (ignore_decay_) decay_ = min_delay_;
      ورنہ اگر (decay_ < min_delay_)
	    min_delay_ = decay_;
}

vvp_delay_t::vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall)
{
      rise_ = rise;
      fall_ = fall;
      decay_= fall < rise? fall : rise;
      min_delay_ = decay_;
      ignore_decay_ = جھوٹ;
}

vvp_delay_t::vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall, vvp_time64_t decay)
{
      rise_ = rise;
      fall_ = fall;
      decay_= decay;
      ignore_decay_ = جھوٹ;

      calculate_min_delay_();
}

خالی vvp_delay_t::سیٹ_ignore_decay()
{
      ignore_decay_ = سچ;

      calculate_min_delay_();
}

vvp_delay_t::~vvp_delay_t()
{
}

vvp_time64_t vvp_delay_t::get_delay(vvp_bit4_t from, vvp_bit4_t to)
{
      انتخاب (from) {
	  حالت BIT4_0:
	    انتخاب (to) {
		حالت BIT4_0: واپس 0;
		حالت BIT4_1: واپس rise_;
		حالت BIT4_X: واپس min_delay_;
		حالت BIT4_Z: واپس decay_;
	    }
	    توڑو;
	  حالت BIT4_1:
	    انتخاب (to) {
		حالت BIT4_0: واپس fall_;
		حالت BIT4_1: واپس 0;
		حالت BIT4_X: واپس min_delay_;
		حالت BIT4_Z: واپس decay_;
	    }
	    توڑو;
	  حالت BIT4_X:
	    انتخاب (to) {
		حالت BIT4_0: واپس fall_;
		حالت BIT4_1: واپس rise_;
		حالت BIT4_X: واپس 0;
		حالت BIT4_Z: واپس decay_;
	    }
	    توڑو;
	  حالت BIT4_Z:
	    انتخاب (to) {
		حالت BIT4_0: واپس fall_;
		حالت BIT4_1: واپس rise_;
		حالت BIT4_X: واپس min_delay_;
		حالت BIT4_Z: واپس 0;
	    }
	    توڑو;
      }

      أكد(0);
      واپس 0;
}

vvp_time64_t vvp_delay_t::get_min_delay() ثابت
{
      واپس min_delay_;
}

خالی vvp_delay_t::سیٹ_rise(vvp_time64_t val)
{
      rise_ = val;
      اگر (val < min_delay_) {
	    min_delay_ = val;
	    اگر (ignore_decay_) decay_ = val;
      } ورنہ
	    calculate_min_delay_();
}

خالی vvp_delay_t::سیٹ_fall(vvp_time64_t val)
{
      fall_ = val;
      اگر (val < min_delay_) {
	    min_delay_ = val;
	    اگر (ignore_decay_) decay_ = val;
      } ورنہ
	    calculate_min_delay_();
}

خالی vvp_delay_t::سیٹ_decay(vvp_time64_t val)
{
      أكد(!ignore_decay_);

      decay_ = val;
      اگر (val < min_delay_)
	    min_delay_ = val;
      ورنہ
	    calculate_min_delay_();
}

vvp_fun_delay::vvp_fun_delay(vvp_net_t*n, غیرعلامتی width, ثابت vvp_delay_t&d)
: net_(n), delay_(d)
{
      cur_real_ = 0.0;
      اگر (width > 0) {
            cur_vec4_ = vvp_vector4_t(width, BIT4_X);
            cur_vec8_ = vvp_vector8_t(cur_vec4_, 6, 6);
            جدول_وقت_init_propagate(net_, cur_vec4_);
      } ورنہ {
            جدول_وقت_init_propagate(net_, cur_real_);
      }
      list_ = 0;
      type_ = UNKNOWN_DELAY;
      initial_ = سچ;
	// Calculate the values used when converting variable delays
	// to simulation time units.
      ثابت __vpiScope*scope = vpip_peek_current_scope();

      صحیح powr = scope->وقت_units - scope->وقت_precision;
      باری = 1;
      برائے (صحیح lp = 0; lp < powr; lp += 1) باری *= 10;

      powr = scope->وقت_precision - vpip_get_وقت_precision();
      scale_ = 1;
      برائے (صحیح lp = 0; lp < powr; lp += 1) scale_ *= 10;
}

vvp_fun_delay::~vvp_fun_delay()
{
      جب (ساخت event_*cur = قطار_سے_نکالو())
	    حذف cur;
}

منطقی vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        ثابت vvp_vector4_t&ثنائی_ہندسہ)
{
      اگر (list_ == 0) واپس جھوٹ;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      اگر (list_->اگلا_2->ptr_vec4.eeq(ثنائی_ہندسہ)) واپس سچ;

      clean_pulse_events_(use_delay);
      واپس جھوٹ;
}

منطقی vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        ثابت vvp_vector8_t&ثنائی_ہندسہ)
{
      اگر (list_ == 0) واپس جھوٹ;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      اگر (list_->اگلا_2->ptr_vec8.eeq(ثنائی_ہندسہ)) واپس سچ;

      clean_pulse_events_(use_delay);
      واپس جھوٹ;
}

منطقی vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay,
                                        دوہرا ثنائی_ہندسہ)
{
      اگر (list_ == 0) واپس جھوٹ;

	/* If the most recent event and the new event have the same
	 * value then we need to skip the new event. */
      اگر (list_->اگلا_2->ptr_real == ثنائی_ہندسہ) واپس سچ;

      clean_pulse_events_(use_delay);
      واپس جھوٹ;
}

خالی vvp_fun_delay::clean_pulse_events_(vvp_time64_t use_delay)
{
      أكد(list_ != 0);

      کرو {
	    ساخت event_*cur = list_->اگلا_2;
	      /* If this event is far enough from the event I'm about
	         to create, then that scheduled event is not a pulse
	         to be eliminated, so we're done. */
	    اگر (cur->sim_وقت+use_delay <= use_delay+جدول_وقت_simtime())
		  توڑو;

	    اگر (list_ == cur)
		  list_ = 0;
	    ورنہ
		  list_->اگلا_2 = cur->اگلا_2;
	    حذف cur;
      } جب (list_);
}

/*
 * FIXME: this implementation currently only uses the maximum delay
 * from all the bit changes in the vectors. If there are multiple
 * changes with different delays, then the results would be
 * wrong. What should happen is that if there are multiple changes,
 * multiple vectors approaching the result should be scheduled.
 */
خالی vvp_fun_delay::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                              vvp_context_t)
{
      اگر (پورٹ.پورٹ() > 0) {
	      // Get the integer value of the bit vector, or 0 if
	      // there are X or Z bits.
	    vvp_time64_t bval = 0;
	      // The following does not work correctly for negative values.
	      // They should be sign extended to 64 bits (1364-2001 9.7.1).
	    vector4_مقصد_قدر(ثنائی_ہندسہ, bval);
	      // Integer values do not need to be rounded so just scale them.
	    vvp_time64_t val = bval * باری * scale_;

	    انتخاب (پورٹ.پورٹ()) {
		حالت 1:
		  delay_.سیٹ_rise(val);
		  واپس;
		حالت 2:
		  delay_.سیٹ_fall(val);
		  واپس;
		حالت 3:
		  delay_.سیٹ_decay(val);
		  واپس;
	    }
	    واپس;
      }

      vvp_time64_t use_delay;
	/* This is an initial value so it needs to be compared to all the
	   bits (the order the bits are changed is not deterministic). */
      اگر (initial_) {
	    type_ = VEC4_DELAY;
            cur_vec8_ = vvp_vector8_t(vvp_vector4_t(0, BIT4_X), 6, 6);
	    vvp_bit4_t cur_val = cur_vec4_.value(0);
	    use_delay = delay_.get_delay(cur_val, ثنائی_ہندسہ.value(0));
	    برائے (غیرعلامتی idx = 1 ;  idx < ثنائی_ہندسہ.size() ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(cur_val, ثنائی_ہندسہ.value(idx));
		  اگر (tmp > use_delay) use_delay = tmp;
	    }
      } ورنہ {
	    أكد(type_ == VEC4_DELAY);

	      // Use as a reference for calculating the delay the
	      // current value of the output. Detect and handle the
	      // special case that the event list contains the current
	      // value as a zero-delay-remaining event.
	    ثابت vvp_vector4_t&use_vec4 = (list_ && list_->اگلا_2->sim_وقت == جدول_وقت_simtime())? list_->اگلا_2->ptr_vec4 : cur_vec4_;

	      /* How many bits to compare? */
	    غیرعلامتی use_wid = use_vec4.size();
	    اگر (ثنائی_ہندسہ.size() < use_wid) use_wid = ثنائی_ہندسہ.size();

	      /* Scan the vectors looking for delays. Select the maximum
	         delay encountered. */
	    use_delay = delay_.get_delay(use_vec4.value(0), ثنائی_ہندسہ.value(0));

	    برائے (غیرعلامتی idx = 1 ;  idx < use_wid ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(use_vec4.value(idx), ثنائی_ہندسہ.value(idx));
		  اگر (tmp > use_delay) use_delay = tmp;
	    }
      }

      /* what *should* happen here is we check to see if there is a
         transaction in the queue. This would be a pulse that needs to be
         eliminated. */
      اگر (clean_pulse_events_(use_delay, ثنائی_ہندسہ)) واپس;

      vvp_time64_t use_simtime = جدول_وقت_simtime() + use_delay;

	/* And propagate it. */
      اگر (use_delay == 0 && list_ == 0) {
	    cur_vec4_ = ثنائی_ہندسہ;
	    initial_ = جھوٹ;
	    net_->send_vec4(cur_vec4_, 0);
      } ورنہ {
	    ساخت event_*cur = نیا ساخت event_(use_simtime);
	    cur->تسلسل_تسلسل_ptr = &vvp_fun_delay::تسلسل_تسلسل_vec4;
	    cur->ptr_vec4 = ثنائی_ہندسہ;
	    قطار_میں_ڈالو(cur);
	    جدول_وقت_generic(یہ, use_delay, جھوٹ);
      }
}

خالی vvp_fun_delay::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			         غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ثنائی_ہندسہ, بنیاد, vwid, ctx);
}

/* See the recv_vec4 comment above. */
خالی vvp_fun_delay::recv_vec8(vvp_net_ptr_t پورٹ, ثابت vvp_vector8_t&ثنائی_ہندسہ)
{
      أكد(پورٹ.پورٹ() == 0);

      vvp_time64_t use_delay;
	/* This is an initial value so it needs to be compared to all the
	   bits (the order the bits are changed is not deterministic). */
      اگر (initial_) {
	    type_ = VEC8_DELAY;
            cur_vec4_ = vvp_vector4_t(0, BIT4_X);
	    vvp_bit4_t cur_val = cur_vec8_.value(0).value();
	    use_delay = delay_.get_delay(cur_val, ثنائی_ہندسہ.value(0).value());
	    برائے (غیرعلامتی idx = 1 ;  idx < ثنائی_ہندسہ.size() ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(cur_val, ثنائی_ہندسہ.value(idx).value());
		  اگر (tmp > use_delay) use_delay = tmp;
	    }
      } ورنہ {
	    أكد(type_ == VEC8_DELAY);

	      // Use as a reference for calculating the delay the
	      // current value of the output. Detect and handle the
	      // special case that the event list contains the current
	      // value as a zero-delay-remaining event.
	    ثابت vvp_vector8_t&use_vec8 = (list_ && list_->اگلا_2->sim_وقت == جدول_وقت_simtime())? list_->اگلا_2->ptr_vec8 : cur_vec8_;

	      /* How many bits to compare? */
	    غیرعلامتی use_wid = use_vec8.size();
	    اگر (ثنائی_ہندسہ.size() < use_wid) use_wid = ثنائی_ہندسہ.size();

	      /* Scan the vectors looking for delays. Select the maximum
	         delay encountered. */
	    use_delay = delay_.get_delay(use_vec8.value(0).value(),
	                                 ثنائی_ہندسہ.value(0).value());

	    برائے (غیرعلامتی idx = 1 ;  idx < use_wid ;  idx += 1) {
		  vvp_time64_t tmp;
		  tmp = delay_.get_delay(use_vec8.value(idx).value(),
		                         ثنائی_ہندسہ.value(idx).value());
		  اگر (tmp > use_delay) use_delay = tmp;
	    }
      }

      /* what *should* happen here is we check to see if there is a
         transaction in the queue. This would be a pulse that needs to be
         eliminated. */
      اگر (clean_pulse_events_(use_delay, ثنائی_ہندسہ)) واپس;

      vvp_time64_t use_simtime = جدول_وقت_simtime() + use_delay;

	/* And propagate it. */
      اگر (use_delay == 0 && list_ == 0) {
	    cur_vec8_ = ثنائی_ہندسہ;
	    initial_ = جھوٹ;
	    net_->send_vec8(cur_vec8_);
      } ورنہ {
	    ساخت event_*cur = نیا ساخت event_(use_simtime);
	    cur->ptr_vec8 = ثنائی_ہندسہ;
	    cur->تسلسل_تسلسل_ptr = &vvp_fun_delay::تسلسل_تسلسل_vec8;
	    قطار_میں_ڈالو(cur);
	    جدول_وقت_generic(یہ, use_delay, جھوٹ);
      }
}

خالی vvp_fun_delay::recv_vec8_pv(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&ثنائی_ہندسہ,
			         غیرعلامتی بنیاد, غیرعلامتی vwid)
{
      recv_vec8_pv_(ptr, ثنائی_ہندسہ, بنیاد, vwid);
}

خالی vvp_fun_delay::recv_real(vvp_net_ptr_t پورٹ, دوہرا ثنائی_ہندسہ,
                              vvp_context_t)
{
      اگر (پورٹ.پورٹ() > 0) {
	    /* If the port is not 0, then this is a delay value that
	    should be rounded and converted to an integer delay. */
	    vvp_time64_t val = 0;
	    اگر (ثنائی_ہندسہ > -0.5) {
		  val = (vvp_time64_t) (ثنائی_ہندسہ * باری + 0.5) * scale_;
	    } ورنہ اگر (ثنائی_ہندسہ != ثنائی_ہندسہ) {
		    // For a NaN we use the default (0).
	    } ورنہ {
		  vvp_vector4_t vec4(8*حجم(vvp_time64_t),
		                     floor(-ثنائی_ہندسہ * باری + 0.5) * -1 * scale_);
		  vector4_مقصد_قدر(vec4, val);
	    }

	    انتخاب (پورٹ.پورٹ()) {
		حالت 1:
		  delay_.سیٹ_rise(val);
		  واپس;
		حالت 2:
		  delay_.سیٹ_fall(val);
		  واپس;
		حالت 3:
		  delay_.سیٹ_decay(val);
		  واپس;
	    }
	    واپس;
      }

      اگر (initial_) {
	    type_ = REAL_DELAY;
            cur_vec4_ = vvp_vector4_t(0, BIT4_X);
            cur_vec8_ = vvp_vector8_t(cur_vec4_, 6, 6);
      } ورنہ أكد(type_ == REAL_DELAY);

      vvp_time64_t use_delay;
      use_delay = delay_.get_min_delay();

      /* Eliminate glitches. */
      اگر (clean_pulse_events_(use_delay, ثنائی_ہندسہ)) واپس;

      /* This must be done after cleaning pulses to avoid propagating
       * an incorrect value. */
      اگر (cur_real_ == ثنائی_ہندسہ) واپس;

      vvp_time64_t use_simtime = جدول_وقت_simtime() + use_delay;

      اگر (use_delay == 0 && list_ == 0) {
	    cur_real_ = ثنائی_ہندسہ;
	    initial_ = جھوٹ;
	    net_->send_real(cur_real_, 0);
      } ورنہ {
	    ساخت event_*cur = نیا ساخت event_(use_simtime);
	    cur->تسلسل_تسلسل_ptr = &vvp_fun_delay::تسلسل_تسلسل_real;
	    cur->ptr_real = ثنائی_ہندسہ;
	    قطار_میں_ڈالو(cur);

	    جدول_وقت_generic(یہ, use_delay, جھوٹ);
      }
}

خالی vvp_fun_delay::تسلسل_تسلسل()
{
      vvp_time64_t sim_وقت = جدول_وقت_simtime();
      اگر (list_ == 0 || list_->اگلا_2->sim_وقت > sim_وقت)
	    واپس;

      ساخت event_*cur = قطار_سے_نکالو();
      اگر (cur == 0)
	    واپس;

      (یہ->*(cur->تسلسل_تسلسل_ptr))(cur);
      initial_ = جھوٹ;
      حذف cur;
}

خالی vvp_fun_delay::تسلسل_تسلسل_vec4(ثابت ساخت event_*cur)
{
      cur_vec4_ = cur->ptr_vec4;
      net_->send_vec4(cur_vec4_, 0);
}

خالی vvp_fun_delay::تسلسل_تسلسل_vec8(ثابت ساخت vvp_fun_delay::event_*cur)
{
      cur_vec8_ = cur->ptr_vec8;
      net_->send_vec8(cur_vec8_);
}

خالی vvp_fun_delay::تسلسل_تسلسل_real(ثابت ساخت vvp_fun_delay::event_*cur)
{
      cur_real_ = cur->ptr_real;
      net_->send_real(cur_real_, 0);
}

vvp_fun_modpath::vvp_fun_modpath(vvp_net_t*net, غیرعلامتی width)
: net_(net), src_list_(0), ifnone_list_(0)
{
      cur_vec4_ = vvp_vector4_t(width, BIT4_X);
      جدول_وقت_init_propagate(net_, cur_vec4_);
}

vvp_fun_modpath::~vvp_fun_modpath()
{
	// Delete the source probes.
      جب (src_list_) {
	    vvp_fun_modpath_src*tmp = src_list_;
	    src_list_ = tmp->اگلا;
	    حذف tmp;
      }
      جب (ifnone_list_) {
	    vvp_fun_modpath_src*tmp = ifnone_list_;
	    ifnone_list_ = tmp->اگلا;
	    حذف tmp;
      }
}

خالی vvp_fun_modpath::جوڑو_modpath_src(vvp_fun_modpath_src*that, منطقی ifnone)
{
      أكد(that->اگلا == 0);
      اگر (ifnone) {
	    that->اگلا = ifnone_list_;
	    ifnone_list_ = that;
      } ورنہ {
	    that->اگلا = src_list_;
	    src_list_ = that;
      }
}

ساکن vvp_time64_t delay_from_کنارہ(vvp_bit4_t a, vvp_bit4_t b,
                                           vvp_time64_t ثابت array[12])
{
      قسمتعریف delay_کنارہ_t bit4_table4[4];
      ساکن ثابت bit4_table4 کنارہ_جدول[4] = {
	    { DELAY_کنارہ_01, DELAY_کنارہ_01, DELAY_کنارہ_0z, DELAY_کنارہ_0x },
	    { DELAY_کنارہ_10, DELAY_کنارہ_10, DELAY_کنارہ_1z, DELAY_کنارہ_1x },
	    { DELAY_کنارہ_z0, DELAY_کنارہ_z1, DELAY_کنارہ_z0, DELAY_کنارہ_zx },
	    { DELAY_کنارہ_x0, DELAY_کنارہ_x1, DELAY_کنارہ_xz, DELAY_کنارہ_x0 }
      };

      واپس array[ کنارہ_جدول[a][b] ];
}

خالی vvp_fun_modpath::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                vvp_context_t)
{
	/* Only the first port is used. */
      اگر (پورٹ.پورٹ() > 0)
	    واپس;

      اگر (cur_vec4_.eeq(ثنائی_ہندسہ))
	    واپس;

	/* Select a time delay source that applies. Notice that there
	   may be multiple delay sources that apply, so collect all
	   the candidates into a list first. */
      list<vvp_fun_modpath_src*>candidate_list;
      vvp_time64_t candidate_wake_وقت = 0;
      برائے (vvp_fun_modpath_src*cur = src_list_ ;  cur ;  cur=cur->اگلا) {
	      /* Skip paths that are disabled by conditions. */
	    اگر (cur->condition_flag_ == جھوٹ)
		  جاری;

	    اگر (candidate_list.empty()) {
		  candidate_list.push_back(cur);
		  candidate_wake_وقت = cur->wake_وقت;
	    } ورنہ اگر (cur->wake_وقت == candidate_wake_وقت) {
		  candidate_list.push_back(cur);
	    } ورنہ اگر (cur->wake_وقت > candidate_wake_وقت) {
		  candidate_list.assign(1, cur);
		  candidate_wake_وقت = cur->wake_وقت;
	    } ورنہ {
		  جاری; /* Skip this entry. */
	    }
      }

	/* Only add the ifnone delay if it has a later wake_time_ or
	 * if there are no normal delays. */
      vvp_time64_t ifnone_wake_وقت = candidate_wake_وقت;
      برائے (vvp_fun_modpath_src*cur = ifnone_list_ ;  cur ;  cur=cur->اگلا) {
	    اگر (candidate_list.empty()) {
		  candidate_list.push_back(cur);
		  ifnone_wake_وقت = cur->wake_وقت;
	    } ورنہ اگر (cur->wake_وقت == ifnone_wake_وقت &&
	               ifnone_wake_وقت > candidate_wake_وقت) {
		  candidate_list.push_back(cur);
	    } ورنہ اگر (cur->wake_وقت > ifnone_wake_وقت) {
		  candidate_list.assign(1, cur);
		  ifnone_wake_وقت = cur->wake_وقت;
	    } ورنہ {
		  جاری; /* Skip this entry. */
	    }
      }

	/* Handle the special case that there are no delays that
	   match. This may happen, for example, if the set of
	   conditional delays is incomplete, leaving some cases
	   uncovered. In that case, just pass the data without delay */
      اگر (candidate_list.empty()) {
	    cur_vec4_ = ثنائی_ہندسہ;
	    جدول_وقت_generic(یہ, 0, جھوٹ);
	    واپس;
      }

	/* Now given that we have a list of candidate delays, find for
	   each if the 12 numbers the minimum from all the
	   candidates. This minimum set becomes the chosen delay to
	   use. */
      vvp_time64_t برآمد_at[12];
      vvp_time64_t now = جدول_وقت_simtime();

      قسمتعریف list<vvp_fun_modpath_src*>::const_iterator iter_t;

      iter_t cur = candidate_list.begin();
      ثابت vvp_fun_modpath_src*src = *cur;

      برائے (غیرعلامتی idx = 0 ;  idx < 12 ;  idx += 1) {
	    برآمد_at[idx] = src->wake_وقت + src->delay_[idx];
	    اگر (برآمد_at[idx] <= now)
		  برآمد_at[idx] = 0;
	    ورنہ
		  برآمد_at[idx] -= now;
      }

      برائے (++ cur ; cur != candidate_list.انتہا() ; ++ cur ) {
	    src = *cur;
	    برائے (غیرعلامتی idx = 0 ;  idx < 12 ;  idx += 1) {
		  vvp_time64_t tmp = src->wake_وقت + src->delay_[idx];
		  اگر (tmp <= now)
			tmp = 0;
		  ورنہ
			tmp -= now;
		  اگر (tmp < برآمد_at[idx])
			برآمد_at[idx] = tmp;
	    }
      }

	/* Given the scheduled output time, create an output event. */
      vvp_time64_t use_delay = delay_from_کنارہ(cur_vec4_.value(0),
					       ثنائی_ہندسہ.value(0),
					       برآمد_at);

	/* FIXME: This bases the edge delay on only the least
	   bit. This is WRONG! I need to find all the possible delays,
	   and schedule an event for each partial change. Hard! */
      برائے (غیرعلامتی idx = 1 ;  idx < ثنائی_ہندسہ.size() ;  idx += 1) {
	    vvp_time64_t tmp = delay_from_کنارہ(cur_vec4_.value(idx),
					       ثنائی_ہندسہ.value(idx),
					       برآمد_at);
	      /* If the current and new bit values match then no delay
	       * is needed for this bit. */
	    اگر (cur_vec4_.value(idx) == ثنائی_ہندسہ.value(idx)) جاری;
	    اگر (tmp != use_delay) {
		  fprintf(stderr, "sorry: multi-bit module path delays are "
		                  "currently not fully supported.\n");
		  fprintf(stderr, "     : using the LSB delay for all bits.\n");
		  جاری;
            }
      }

      cur_vec4_ = ثنائی_ہندسہ;
      جدول_وقت_generic(یہ, use_delay, جھوٹ);
}

خالی vvp_fun_modpath::تسلسل_تسلسل()
{
      net_->send_vec4(cur_vec4_, 0);
}

vvp_fun_modpath_src::vvp_fun_modpath_src(vvp_time64_t ثابت del[12])
{
      برائے (غیرعلامتی idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = del[idx];

      اگلا = 0;
      wake_وقت = 0;
      condition_flag_ = سچ;
}

vvp_fun_modpath_src::~vvp_fun_modpath_src()
{
}

خالی vvp_fun_modpath_src::get_delay12(vvp_time64_t val[12]) ثابت
{
      برائے (غیرعلامتی idx = 0 ;  idx < 12 ;  idx += 1)
	    val[idx] = delay_[idx];
}

خالی vvp_fun_modpath_src::put_delay12(ثابت vvp_time64_t val[12])
{
      برائے (غیرعلامتی idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = val[idx];
}

خالی vvp_fun_modpath_src::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                    vvp_context_t)
{
      اگر (پورٹ.پورٹ() == 0) {
	      // The modpath input...
	    اگر (test_vec4(ثنائی_ہندسہ))
		  wake_وقت = جدول_وقت_simtime();

      } ورنہ اگر (پورٹ.پورٹ() == 1) {
	      // The modpath condition input...
	    اگر (ثنائی_ہندسہ.value(0) != BIT4_0)
		  condition_flag_ = سچ;
	    ورنہ
		  condition_flag_ = جھوٹ;
      }
}

منطقی vvp_fun_modpath_src::test_vec4(ثابت vvp_vector4_t&)
{
      واپس سچ;
}

vvp_fun_modpath_کنارہ::vvp_fun_modpath_کنارہ(vvp_time64_t del[12],
					   منطقی pos, منطقی neg)
: vvp_fun_modpath_src(del)
{
      old_قدر = BIT4_X;
      posedge_ = pos;
      negedge_ = neg;
}

منطقی vvp_fun_modpath_کنارہ::test_vec4(ثابت vvp_vector4_t&ثنائی_ہندسہ)
{
      vvp_bit4_t tmp = old_قدر;
      old_قدر = ثنائی_ہندسہ.value(0);

      صحیح کنارہ_flag = کنارہ(tmp, old_قدر);
      اگر (کنارہ_flag > 0) واپس posedge_;
      اگر (کنارہ_flag < 0) واپس negedge_;
      واپس جھوٹ;
}


/*
 * All the below routines that begin with
 * modpath_src_* belong the internal function
 * of an vpiModPathIn object. This is used to
 * make some specific delays path operations
 *
 */
ساکن صحیح modpath_src_get(صحیح, vpiHandle ref)
{
      ساخت __vpiModPathSrc*obj =dynamic_cast<__vpiModPathSrc*>(ref);
      أكد(obj);
      واپس 0;
}

ساکن خالی modpath_src_get_قدر(vpiHandle ref, p_vpi_قدر)
{
      ساخت __vpiModPathSrc* modpathsrc = dynamic_cast<__vpiModPathSrc*>(ref);
      أكد(modpathsrc);
      واپس;
}

ساکن vpiHandle modpath_src_put_قدر(vpiHandle ref, s_vpi_قدر *, صحیح )
{
      ساخت __vpiModPathSrc* modpathsrc = dynamic_cast<__vpiModPathSrc*>(ref);
      أكد(modpathsrc);
      واپس 0;
}

ساکن vpiHandle modpath_src_get_handle(صحیح code, vpiHandle ref)
{
      ساخت __vpiModPathSrc*rfp = dynamic_cast<__vpiModPathSrc*>(ref);
      أكد(rfp);

      انتخاب (code) {

	حالت vpiScope:
	  واپس rfp->dest->scope;

	  حالت vpiModule:
	      { __vpiScope*scope = rfp->dest->scope;
		جب (scope && scope->get_type_رمز() != vpiModule)
		      scope = scope->scope;
		أكد(scope);
		واپس scope;
	      }

	    // Handles to path term objects should really be obtained via
	    // the vpi_iterate and vpi_scan functions. Continue to allow
	    // them to be obtained here for backwards compatibility with
	    // older versions of Icarus Verilog.

	  حالت vpi_Mod_راستہ_اندر:
	    واپس &rfp->راستہ_رکن_اندر;

	  حالت vpi_Mod_راستہ_برآمد:
	    واپس &rfp->dest->راستہ_رکن_برآمد;
      }
      واپس 0;
}

ساکن vpiHandle modpath_src_iterate(صحیح code, vpiHandle ref)
{
      ساخت __vpiModPathSrc*rfp = dynamic_cast<__vpiModPathSrc*>(ref);
      أكد(rfp);

	// Module paths with multiple sources or destinations are
	// currently represented by a separate modpath object for
	// each source/destination combination, so there is only
	// ever one input path term and one output path term.
      انتخاب (code) {
	  حالت vpi_Mod_راستہ_اندر: {
	    vpiHandle*دلائل = static_cast<vpiHandle*>(صفر_حافظہ_دو(1, حجم(vpiHandle*)));
	    دلائل[0] = &rfp->راستہ_رکن_اندر;
	    واپس vpip_make_iterator(1, دلائل, سچ);
	  }
	  حالت vpi_Mod_راستہ_برآمد: {
	    vpiHandle*دلائل = static_cast<vpiHandle*>(صفر_حافظہ_دو(1, حجم(vpiHandle*)));
	    دلائل[0] = &rfp->dest->راستہ_رکن_برآمد;
	    واپس vpip_make_iterator(1, دلائل, سچ);
	  }
      }
      واپس 0;
}

ساکن vpiHandle modpath_src_اشاریہ ( vpiHandle ref, صحیح)
{
      أكد(ref->get_type_رمز() == vpi_Mod_راستہ_اندر);
      واپس 0;
}


/*
 * This routine will put specific dimension of delay[] values
 * into a vpiHandle. In this case, we will put
 * specific delays values in a vpiModPathIn object
 *
 */
ساکن خالی modpath_src_put_delays (vpiHandle ref, p_vpi_delay delays)
{
      vvp_time64_t tmp[12];
      صحیح idx;
      ساخت __vpiModPathSrc * src = dynamic_cast<__vpiModPathSrc*>(ref) ;
      أكد(src) ;

      vvp_fun_modpath_src *fun = dynamic_cast<vvp_fun_modpath_src*>(src->net->fun);
      أكد( fun );

      قسمتعریف غیرعلامتی حرف map_array_t[12];
	// Only the first six entries are used for the less than twelve maps.
      ساکن ثابت map_array_t map_1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      ساکن ثابت map_array_t map_2 = {0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0};
      ساکن ثابت map_array_t map_3 = {0, 1, 2, 0, 2, 1, 0, 0, 0, 0, 0, 0};
      ساکن ثابت map_array_t map_6 = {0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0};
      ساکن ثابت map_array_t map12 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

      ثابت map_array_t*use_map = 0;
      انتخاب (delays->no_کا_delays) {
	  حالت 1:
	    use_map = &map_1;
	    توڑو;
	  حالت 2:
	    use_map = &map_2;
	    توڑو;
	  حالت 3:
	    use_map = &map_3;
	    توڑو;
	  حالت 6:
	    use_map = &map_6;
	    توڑو;
	  حالت 12:
	    use_map = &map12;
	    توڑو;
	  طےشدہ:
	    أكد(0);
	    توڑو;
      }

      اگر (delays->وقت_type == vpi_Sim_وقت) {
	    برائے (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_timestruct_مقصد_وقت(delays->da+use_map[0][idx]);
	    }
      } ورنہ {
	      // You cannot create a modpath with a negative delay so set it
	      // to zero per 1364-2005 section 14.3.1.
	    برائے (idx = 0 ; idx < delays->no_کا_delays ; idx += 1) {
		  اگر (delays->da[idx].real < 0.0) delays->da[idx].real = 0.0;
	    }
	    برائے (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_scaled_real_مقصد_time64(delays->da[use_map[0][idx]].real,
							src->dest->scope);
	    }
      }

      /* Now define the to-from-x delays if needed. */
      اگر (delays->no_کا_delays <= 6) {
	      /* 0->x is the minimum of 0->z and 0->1. */
	    tmp[DELAY_کنارہ_0x] = tmp[DELAY_کنارہ_0z] < tmp[DELAY_کنارہ_01] ?
	                           tmp[DELAY_کنارہ_0z] : tmp[DELAY_کنارہ_01];
	      /* x->1 is the maximum of z->1 and 0->1. */
	    tmp[DELAY_کنارہ_x1] = tmp[DELAY_کنارہ_z1] > tmp[DELAY_کنارہ_01] ?
	                           tmp[DELAY_کنارہ_z1] : tmp[DELAY_کنارہ_01];
	      /* 1->x is the minimum of 1->z and 1->0. */
	    tmp[DELAY_کنارہ_1x] = tmp[DELAY_کنارہ_1z] < tmp[DELAY_کنارہ_10] ?
	                           tmp[DELAY_کنارہ_1z] : tmp[DELAY_کنارہ_10];
	      /* x->0 is the maximum of z->0 and 1->0. */
	    tmp[DELAY_کنارہ_x0] = tmp[DELAY_کنارہ_z0] > tmp[DELAY_کنارہ_10] ?
	                           tmp[DELAY_کنارہ_z0] : tmp[DELAY_کنارہ_10];
	      /* x->z is the maximum of 1->z and 0->z. */
	    tmp[DELAY_کنارہ_xz] = tmp[DELAY_کنارہ_1z] > tmp[DELAY_کنارہ_0z] ?
	                           tmp[DELAY_کنارہ_1z] : tmp[DELAY_کنارہ_0z];
	      /* z->x is the minimum of z->1 and z->0. */
	    tmp[DELAY_کنارہ_zx] = tmp[DELAY_کنارہ_z1] < tmp[DELAY_کنارہ_z0] ?
	                           tmp[DELAY_کنارہ_z1] : tmp[DELAY_کنارہ_z0];
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

ساکن خالی modpath_src_get_delays ( vpiHandle ref, p_vpi_delay delays )
{
      ساخت __vpiModPathSrc*src = dynamic_cast<__vpiModPathSrc*>(ref) ;
      أكد(src);

      vvp_fun_modpath_src *fun = dynamic_cast<vvp_fun_modpath_src*>(src->net->fun);
      أكد(fun);

      صحیح idx;
      vvp_time64_t tmp[12];
      fun->get_delay12(tmp);

      انتخاب (delays->no_کا_delays) {
	  حالت 1:
	  حالت 2:
	  حالت 3:
	  حالت 6:
	  حالت 12:
	    توڑو;

	  طےشدہ:
	    أكد(0);
	    توڑو;
      }

      اگر (delays->وقت_type == vpi_Sim_وقت) {
	    برائے (idx = 0; idx < delays->no_کا_delays; idx += 1) {
		  vpip_وقت_مقصد_timestruct(delays->da+idx, tmp[idx]);
	    }
      } ورنہ {
	    برائے (idx = 0; idx < delays->no_کا_delays; idx += 1) {
		  delays->da[idx].real = vpip_وقت_مقصد_scaled_real(tmp[idx], src->dest->scope);
	    }
      }
}

ساکن صحیح pathterm_get(صحیح code, vpiHandle ref)
{
      ساخت __vpiModPathTerm*obj = dynamic_cast<__vpiModPathTerm*>(ref);
      أكد(obj);

      انتخاب (code) {
	  حالت vpi_کنارہ_2:
	    واپس obj->کنارہ;
	  طےشدہ:
	    واپس 0;
      }
}

ساکن vpiHandle pathterm_get_handle(صحیح code, vpiHandle ref)
{
      ساخت __vpiModPathTerm*obj = dynamic_cast<__vpiModPathTerm*>(ref);
      أكد(obj);

      انتخاب (code) {
	  حالت vpi_اظہاریہ:
	    واپس obj->اظہاریہ;
	  طےشدہ:
	    واپس 0;
      }
}

/*
* The __vpiModPathSrc class is what the VPI client sees as a
* vpiModPath object. The __vpiModPath structure contains items that
* are common to a bunch of modpaths, including the destination term.
*/
اندرونی __vpiModPathSrc::__vpiModPathSrc()
{ }

صحیح __vpiModPathSrc::get_type_رمز(خالی) ثابت
{ واپس vpi_Mod_راستہ; }

صحیح __vpiModPathSrc::vpi_get(صحیح code)
{ واپس modpath_src_get(code, یہ); }

خالی __vpiModPathSrc::vpi_get_قدر(p_vpi_قدر val)
{ modpath_src_get_قدر(یہ, val); }

vpiHandle __vpiModPathSrc::vpi_put_قدر(p_vpi_قدر val, صحیح flags)
{ واپس modpath_src_put_قدر(یہ, val, flags); }

vpiHandle __vpiModPathSrc::vpi_handle(صحیح code)
{ واپس modpath_src_get_handle(code, یہ); }

vpiHandle __vpiModPathSrc::vpi_iterate(صحیح code)
{ واپس modpath_src_iterate(code, یہ); }

vpiHandle __vpiModPathSrc:: vpi_اشاریہ(صحیح idx)
{ واپس modpath_src_اشاریہ(یہ, idx); }

خالی __vpiModPathSrc::vpi_get_delays(p_vpi_delay del)
{ modpath_src_get_delays(یہ, del); }

خالی __vpiModPathSrc::vpi_put_delays(p_vpi_delay del)
{ modpath_src_put_delays(یہ, del); }

ساکن صحیح modpath_src_free_object( vpiHandle ref )
{
      حذف ref;
      واپس 1 ;
}

__vpiHandle::free_object_fun_t __vpiModPathSrc::free_object_fun(خالی)
{ واپس &modpath_src_free_object; }


اندرونی __vpiModPathTerm::__vpiModPathTerm()
{ }

صحیح __vpiModPathTerm::get_type_رمز(خالی) ثابت
{ واپس vpi_راستہ_رکن; }

صحیح __vpiModPathTerm::vpi_get(صحیح code)
{ واپس pathterm_get(code, یہ); }

vpiHandle __vpiModPathTerm::vpi_handle(صحیح code)
{ واپس pathterm_get_handle(code, یہ); }

ساکن خالی initialize_راستہ_رکن(ساخت __vpiModPathTerm&obj)
{
      obj.اظہاریہ = 0;
      obj.کنارہ = vpi_No_کنارہ;
}

/*
 * This function will construct a vpiModPath Object.
 * give a respective "net", and will point to his
 * respective functor
 */

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
ساکن ساخت __vpiModPath**mp_list = 0;
ساکن غیرعلامتی mp_گنتی = 0;
#اگر_ختم

ساخت __vpiModPath* vpip_make_modpath(vvp_net_t *net)
{
      ساخت __vpiModPath*obj = نیا __vpiModPath;
      obj->scope = vpip_peek_current_scope ( );

      initialize_راستہ_رکن(obj->راستہ_رکن_برآمد);
      obj->داخلہ_net = net ;

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
      mp_گنتی += 1;
      mp_list = static_cast<ساخت __vpiModPath **>
                (realloc(mp_list,
                 mp_گنتی*حجم(ساخت __vpiModPath **)));
      mp_list[mp_گنتی-1] = obj;
#اگر_ختم
      واپس obj;
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
خالی modpath_delete()
{
      برائے (غیرعلامتی idx = 0; idx < mp_گنتی; idx += 1) {
	    حذف mp_list[idx];
      }
      آزاد(mp_list);
      mp_list = 0;
      mp_گنتی = 0;
}
#اگر_ختم

/*
 * This function will construct a vpiModPathIn
 * ( struct __vpiModPathSrc ) Object. will give
 * a delays[12] values, and point to the specified functor
 *
 */

ساخت __vpiModPathSrc* vpip_make_modpath_src(ساخت __vpiModPath*راستہ,
                                              vvp_net_t *net)
{
      ساخت __vpiModPathSrc *obj = نیا __vpiModPathSrc;

      obj->dest = راستہ;
      obj->type = 0;
      obj->net = net;
      initialize_راستہ_رکن(obj->راستہ_رکن_اندر);

      واپس obj;
}

vvp_fun_intermodpath::vvp_fun_intermodpath(vvp_net_t*net, غیرعلامتی width)
: net_(net)
{
      برائے (غیرعلامتی idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = 0;

      cur_vec4_ = vvp_vector4_t(width, BIT4_X);
      جدول_وقت_init_propagate(net_, cur_vec4_);
}

vvp_fun_intermodpath::~vvp_fun_intermodpath()
{
}

خالی vvp_fun_intermodpath::get_delay12(vvp_time64_t val[12]) ثابت
{
      برائے (غیرعلامتی idx = 0 ;  idx < 12 ;  idx += 1)
	    val[idx] = delay_[idx];
}

خالی vvp_fun_intermodpath::put_delay12(ثابت vvp_time64_t val[12])
{
      برائے (غیرعلامتی idx = 0 ;  idx < 12 ;  idx += 1)
	    delay_[idx] = val[idx];
}

/*
 * FIXME: this implementation currently only uses the maximum delay
 * from all the bit changes in the vectors. If there are multiple
 * changes with different delays, then the results would be
 * wrong. What should happen is that if there are multiple changes,
 * multiple vectors approaching the result should be scheduled.
 */
خالی vvp_fun_intermodpath::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                              vvp_context_t)
{
	/* Only the first port is used. */
      اگر (پورٹ.پورٹ() > 0)
	    واپس;

      اگر (cur_vec4_.eeq(ثنائی_ہندسہ))
	    واپس;

	/* Given the scheduled output time, create an output event. */
      vvp_time64_t use_delay = delay_from_کنارہ(cur_vec4_.value(0),
					       ثنائی_ہندسہ.value(0),
					       delay_);

	/* FIXME: This bases the edge delay on only the least
	   bit. This is WRONG! I need to find all the possible delays,
	   and schedule an event for each partial change. Hard! */
      برائے (غیرعلامتی idx = 1 ;  idx < ثنائی_ہندسہ.size() ;  idx += 1) {
	    vvp_time64_t tmp = delay_from_کنارہ(cur_vec4_.value(idx),
					       ثنائی_ہندسہ.value(idx),
					       delay_);
	      /* If the current and new bit values match then no delay
	       * is needed for this bit. */
	    اگر (cur_vec4_.value(idx) == ثنائی_ہندسہ.value(idx)) جاری;
	    أكد(tmp == use_delay);
      }

      cur_vec4_ = ثنائی_ہندسہ;
      جدول_وقت_generic(یہ, use_delay, جھوٹ);
}

خالی vvp_fun_intermodpath::تسلسل_تسلسل()
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
ساکن صحیح intermodpath_get(صحیح, vpiHandle ref)
{
      ساخت __vpiInterModPath*obj =dynamic_cast<__vpiInterModPath*>(ref);
      أكد(obj);
      واپس 0;
}

ساکن خالی intermodpath_get_قدر(vpiHandle ref, p_vpi_قدر)
{
      ساخت __vpiInterModPath* intermodpath = dynamic_cast<__vpiInterModPath*>(ref);
      أكد(intermodpath);
      واپس;
}

ساکن vpiHandle intermodpath_put_قدر(vpiHandle ref, s_vpi_قدر *, صحیح )
{
      ساخت __vpiInterModPath* intermodpath = dynamic_cast<__vpiInterModPath*>(ref);
      أكد(intermodpath);
      واپس 0;
}

ساکن vpiHandle intermodpath_get_handle(صحیح code, vpiHandle ref)
{
      ساخت __vpiInterModPath*rfp = dynamic_cast<__vpiInterModPath*>(ref);
      أكد(rfp);

      انتخاب (code) {

	حالت vpiScope:
	  واپس rfp->scope;

	  حالت vpiModule:
	      { __vpiScope*scope = rfp->scope;
		جب (scope && scope->get_type_رمز() != vpiModule)
		      scope = scope->scope;
		أكد(scope);
		واپس scope;
	      }
      }
      واپس 0;
}

ساکن vpiHandle intermodpath_iterate(صحیح code, vpiHandle ref)
{
      ساخت __vpiInterModPath*rfp = dynamic_cast<__vpiInterModPath*>(ref);
      أكد(rfp);

	// For now intermodpaths only support exactly two ports
      انتخاب (code) {
	  حالت vpi_پورٹ: {
	    vpiHandle*دلائل = static_cast<vpiHandle*>(صفر_حافظہ_دو(2, حجم(vpiHandle*)));
	    دلائل[0] = rfp->port1;
	    دلائل[1] = rfp->port2;
	    واپس vpip_make_iterator(2, دلائل, سچ);
	  }
      }
      واپس 0;
}

/*
 * This routine will put specific dimension of delay[] values
 * into a vpiHandle. In this case, we will put
 * specific delays values in a vpiInterModPath object
 *
 */
ساکن خالی intermodpath_put_delays (vpiHandle ref, p_vpi_delay delays)
{
      vvp_time64_t tmp[12];
      صحیح idx;
      ساخت __vpiInterModPath * src = dynamic_cast<__vpiInterModPath*>(ref) ;
      أكد(src) ;

      vvp_fun_intermodpath *fun = dynamic_cast<vvp_fun_intermodpath*>(src->net->fun);
      أكد( fun );

      قسمتعریف غیرعلامتی حرف map_array_t[12];
	// Only the first six entries are used for the less than twelve maps.
      ساکن ثابت map_array_t map_1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      ساکن ثابت map_array_t map_2 = {0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0};
      ساکن ثابت map_array_t map_3 = {0, 1, 2, 0, 2, 1, 0, 0, 0, 0, 0, 0};
      ساکن ثابت map_array_t map_6 = {0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0};
      ساکن ثابت map_array_t map12 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

      ثابت map_array_t*use_map = 0;
      انتخاب (delays->no_کا_delays) {
	  حالت 1:
	    use_map = &map_1;
	    توڑو;
	  حالت 2:
	    use_map = &map_2;
	    توڑو;
	  حالت 3:
	    use_map = &map_3;
	    توڑو;
	  حالت 6:
	    use_map = &map_6;
	    توڑو;
	  حالت 12:
	    use_map = &map12;
	    توڑو;
	  طےشدہ:
	    أكد(0);
	    توڑو;
      }

      اگر (delays->وقت_type == vpi_Sim_وقت) {
	    برائے (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_timestruct_مقصد_وقت(delays->da+use_map[0][idx]);
	    }
      } ورنہ {
	      // You cannot create a modpath with a negative delay so set it
	      // to zero per 1364-2005 section 14.3.1.
	    برائے (idx = 0 ; idx < delays->no_کا_delays ; idx += 1) {
		  اگر (delays->da[idx].real < 0.0) delays->da[idx].real = 0.0;
	    }
	    برائے (idx = 0 ; idx < 12 ; idx += 1) {
		  tmp[idx] = vpip_scaled_real_مقصد_time64(delays->da[use_map[0][idx]].real,
							src->scope);
	    }
      }

      /* Now define the to-from-x delays if needed. */
      اگر (delays->no_کا_delays <= 6) {
	      /* 0->x is the minimum of 0->z and 0->1. */
	    tmp[DELAY_کنارہ_0x] = tmp[DELAY_کنارہ_0z] < tmp[DELAY_کنارہ_01] ?
	                           tmp[DELAY_کنارہ_0z] : tmp[DELAY_کنارہ_01];
	      /* x->1 is the maximum of z->1 and 0->1. */
	    tmp[DELAY_کنارہ_x1] = tmp[DELAY_کنارہ_z1] > tmp[DELAY_کنارہ_01] ?
	                           tmp[DELAY_کنارہ_z1] : tmp[DELAY_کنارہ_01];
	      /* 1->x is the minimum of 1->z and 1->0. */
	    tmp[DELAY_کنارہ_1x] = tmp[DELAY_کنارہ_1z] < tmp[DELAY_کنارہ_10] ?
	                           tmp[DELAY_کنارہ_1z] : tmp[DELAY_کنارہ_10];
	      /* x->0 is the maximum of z->0 and 1->0. */
	    tmp[DELAY_کنارہ_x0] = tmp[DELAY_کنارہ_z0] > tmp[DELAY_کنارہ_10] ?
	                           tmp[DELAY_کنارہ_z0] : tmp[DELAY_کنارہ_10];
	      /* x->z is the maximum of 1->z and 0->z. */
	    tmp[DELAY_کنارہ_xz] = tmp[DELAY_کنارہ_1z] > tmp[DELAY_کنارہ_0z] ?
	                           tmp[DELAY_کنارہ_1z] : tmp[DELAY_کنارہ_0z];
	      /* z->x is the minimum of z->1 and z->0. */
	    tmp[DELAY_کنارہ_zx] = tmp[DELAY_کنارہ_z1] < tmp[DELAY_کنارہ_z0] ?
	                           tmp[DELAY_کنارہ_z1] : tmp[DELAY_کنارہ_z0];
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

ساکن خالی intermodpath_get_delays ( vpiHandle ref, p_vpi_delay delays )
{
      ساخت __vpiInterModPath*src = dynamic_cast<__vpiInterModPath*>(ref) ;
      أكد(src);

      vvp_fun_intermodpath *fun = dynamic_cast<vvp_fun_intermodpath*>(src->net->fun);
      أكد(fun);

      صحیح idx;
      vvp_time64_t tmp[12];
      fun->get_delay12(tmp);

      انتخاب (delays->no_کا_delays) {
	  حالت 1:
	  حالت 2:
	  حالت 3:
	  حالت 6:
	  حالت 12:
	    توڑو;

	  طےشدہ:
	    أكد(0);
	    توڑو;
      }

      اگر (delays->وقت_type == vpi_Sim_وقت) {
	    برائے (idx = 0; idx < delays->no_کا_delays; idx += 1) {
		  vpip_وقت_مقصد_timestruct(delays->da+idx, tmp[idx]);
	    }
      } ورنہ {
	    برائے (idx = 0; idx < delays->no_کا_delays; idx += 1) {
		  delays->da[idx].real = vpip_وقت_مقصد_scaled_real(tmp[idx], src->scope);
	    }
      }
}

/*
* The __vpiInterModPath class is what the VPI client sees as a
* vpiInterModPath object.
*/
اندرونی __vpiInterModPath::__vpiInterModPath()
{ }

صحیح __vpiInterModPath::get_type_رمز(خالی) ثابت
{ واپس vpi_Inter_Mod_راستہ; }

صحیح __vpiInterModPath::vpi_get(صحیح code)
{ واپس intermodpath_get(code, یہ); }

خالی __vpiInterModPath::vpi_get_قدر(p_vpi_قدر val)
{ intermodpath_get_قدر(یہ, val); }

vpiHandle __vpiInterModPath::vpi_put_قدر(p_vpi_قدر val, صحیح flags)
{ واپس intermodpath_put_قدر(یہ, val, flags); }

vpiHandle __vpiInterModPath::vpi_handle(صحیح code)
{ واپس intermodpath_get_handle(code, یہ); }

vpiHandle __vpiInterModPath::vpi_iterate(صحیح code)
{ واپس intermodpath_iterate(code, یہ); }

خالی __vpiInterModPath::vpi_get_delays(p_vpi_delay del)
{ intermodpath_get_delays(یہ, del); }

خالی __vpiInterModPath::vpi_put_delays(p_vpi_delay del)
{ intermodpath_put_delays(یہ, del); }

ساکن صحیح intermodpath_free_object( vpiHandle ref )
{
      حذف ref;
      واپس 1 ;
}

__vpiHandle::free_object_fun_t __vpiInterModPath::free_object_fun(خالی)
{ واپس &intermodpath_free_object; }

/*
 * This function will construct a vpiInterModPath Object.
 * give a respective "net", and will point to his
 * respective functor
 */

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
ساکن ساخت __vpiInterModPath**imp_list = 0;
ساکن غیرعلامتی imp_گنتی = 0;
#اگر_ختم

ساخت __vpiInterModPath* vpip_make_intermodpath(vvp_net_t *net, vpi_پورٹ_Info* port1, vpi_پورٹ_Info* port2)
{
      ساخت __vpiInterModPath*obj = نیا __vpiInterModPath;
      obj->scope = vpip_peek_current_scope ( );

      obj->net = net;
      obj->port1 = port1;
      obj->port2 = port2;

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
      imp_گنتی += 1;
      imp_list = static_cast<ساخت __vpiInterModPath **>
                 (realloc(imp_list,
                  imp_گنتی*حجم(ساخت __vpiInterModPath **)));
      imp_list[imp_گنتی-1] = obj;
#اگر_ختم
      واپس obj;
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
خالی intermodpath_delete()
{
      برائے (غیرعلامتی idx = 0; idx < mp_گنتی; idx += 1) {
	    حذف imp_list[idx];
      }
      آزاد(imp_list);
      imp_list = 0;
      imp_گنتی = 0;
}
#اگر_ختم
