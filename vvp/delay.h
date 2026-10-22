#اگر_غیر_تعریف_شدہ IVL_delay_H
#تعریف_کرو IVL_delay_H
/*
 * Copyright 2005-2026 Stephen Williams
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

# include  <stddef.h>
# include  "vvp_net.h"
# include  "schedule.h"

شمار delay_کنارہ_t {
      DELAY_کنارہ_01 = 0, DELAY_کنارہ_10, DELAY_کنارہ_0z,
      DELAY_کنارہ_z1,     DELAY_کنارہ_1z, DELAY_کنارہ_z0,
      DELAY_کنارہ_0x,     DELAY_کنارہ_x1, DELAY_کنارہ_1x,
      DELAY_کنارہ_x0,     DELAY_کنارہ_xz, DELAY_کنارہ_zx,
      DELAY_کنارہ_گنتی
};

/*
 * Instances of this object are functions that calculate the delay for
 * the transition from the source vvp_bit4_t value to the destination
 * value.
 */
class vvp_delay_t {

    عوامی:
      vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall);
      vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall, vvp_time64_t decay);
      ~vvp_delay_t();

      vvp_time64_t get_delay(vvp_bit4_t from, vvp_bit4_t مقصد);
      vvp_time64_t get_min_delay() ثابت;

      خالی سیٹ_rise(vvp_time64_t val);
      خالی سیٹ_fall(vvp_time64_t val);
      خالی سیٹ_decay(vvp_time64_t val);
      خالی سیٹ_ignore_decay();

    نجی:
      vvp_time64_t rise_, fall_, decay_;
      vvp_time64_t min_delay_;
      bool ignore_decay_;

      خالی calculate_min_delay_();
};

/* vvp_fun_delay
 * This is a lighter weight version of vvp_fun_drive, that only
 * carries delays. The output that it propagates is vvp_vector4_t so
 * drive strengths are lost, but then again it doesn't go through the
 * effort of calculating strength values either.
 *
 * The node needs a pointer to the vvp_net_t input so that it knows
 * how to find its output when propagating delayed output.
 *
 * NOTE: This node supports vec4 and real by repeating whatever was
 * input. This is a bit of a hack, as it may be more efficient to
 * create the right type of vvp_fun_delay_real.
 */
class vvp_fun_delay  : عوامی vvp_net_fun_t, نجی vvp_gen_event_s {

      شمار delay_type_t {UNKNOWN_DELAY, VEC4_DELAY, VEC8_DELAY, REAL_DELAY};
      ساخت event_ {
	    explicit event_(vvp_time64_t s) : sim_وقت(s) {
		  ptr_real = 0.0;
		  اگلا_2 = تہی;
	    }
	    خالی (vvp_fun_delay::*تسلسل_تسلسل_ptr)(ثابت ساخت vvp_fun_delay::event_*cur);
	    ثابت vvp_time64_t sim_وقت;
	    vvp_vector4_t ptr_vec4;
	    vvp_vector8_t ptr_vec8;
	    دوہرا ptr_real;
	    ساخت event_*اگلا_2;
      };

    عوامی:
      vvp_fun_delay(vvp_net_t*net, غیرعلامتی چوڑائی, ثابت vvp_delay_t&d);
      ~vvp_fun_delay() override;

      خالی recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override;
      خالی recv_vec8(vvp_net_ptr_t پورٹ, ثابت vvp_vector8_t&ثنائی_ہندسہ) override;
      خالی recv_real(vvp_net_ptr_t پورٹ, دوہرا ثنائی_ہندسہ,
                     vvp_context_t) override;

      خالی recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t ctx) override;
      خالی recv_vec8_pv(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&ثنائی_ہندسہ,
			غیرعلامتی بنیاد, غیرعلامتی vwid) override;

    نجی:
      مجازی خالی تسلسل_تسلسل() override;


      خالی تسلسل_تسلسل_vec4(ثابت ساخت vvp_fun_delay::event_*cur);
      خالی تسلسل_تسلسل_vec8(ثابت ساخت vvp_fun_delay::event_*cur);
      خالی تسلسل_تسلسل_real(ثابت ساخت vvp_fun_delay::event_*cur);

    نجی:
      vvp_net_t*net_;
      vvp_delay_t delay_;
      delay_type_t type_;
      bool initial_; // Indicates if the value is still the initial value.

      vvp_vector4_t cur_vec4_;
      vvp_vector8_t cur_vec8_;
      دوہرا cur_real_;
      vvp_time64_t باری, scale_; // Needed to scale variable time values.

      ساخت event_ *list_;
      خالی قطار_میں_ڈالو(ساخت event_*cur)
      {
	    اگر (list_) {
		  cur->اگلا_2 = list_->اگلا_2;
		  list_->اگلا_2 = cur;
		  list_ = cur;
	    } ورنہ {
		  cur->اگلا_2 = cur;
		  list_ = cur;
	    }
      }
      ساخت event_* قطار_سے_نکالو(خالی)
      {
	    اگر (list_ == 0)
		  واپس 0;
	    ساخت event_*cur = list_->اگلا_2;
	    اگر (list_ == cur)
		  list_ = 0;
	    ورنہ
		  list_->اگلا_2 = cur->اگلا_2;
	    واپس cur;
      }
      bool clean_pulse_events_(vvp_time64_t use_delay, ثابت vvp_vector4_t&ثنائی_ہندسہ);
      bool clean_pulse_events_(vvp_time64_t use_delay, ثابت vvp_vector8_t&ثنائی_ہندسہ);
      bool clean_pulse_events_(vvp_time64_t use_delay, دوہرا ثنائی_ہندسہ);
// Delete this when done!
      خالی clean_pulse_events_(vvp_time64_t use_delay);
};

/*
* These objects implement module delay paths. The fun_modpath functor
* is the output of the modpath, and the vvp_fun_modpath_src is the
* source of the modpath. The modpath source tracks events on the
* inputs to enable delays, and the vvp_fun_modpath, when it's time to
* schedule, looks at the associated modpath_src objects for which
* paths are active.
*/
class vvp_fun_modpath;
class vvp_fun_modpath_src;

class vvp_fun_modpath  : عوامی vvp_net_fun_t, نجی vvp_gen_event_s {

    عوامی:
      vvp_fun_modpath(vvp_net_t*net, غیرعلامتی چوڑائی);
      ~vvp_fun_modpath() override;

      خالی جوڑو_modpath_src(vvp_fun_modpath_src*that, bool ifnone);

      خالی recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override;

    نجی:
      مجازی خالی تسلسل_تسلسل() override;

    نجی:
      vvp_net_t*net_;

      vvp_vector4_t cur_vec4_;

      vvp_fun_modpath_src*src_list_;
      vvp_fun_modpath_src*ifnone_list_;

    نجی: // not implemented
      vvp_fun_modpath(ثابت vvp_fun_modpath&);
      vvp_fun_modpath& عامل= (ثابت vvp_fun_modpath&);
};

class vvp_fun_modpath_src  : عوامی vvp_net_fun_t {

      دوست class vvp_fun_modpath;

    عوامی:
      explicit vvp_fun_modpath_src(vvp_time64_t ثابت del[12]);
    محفوظ:
      ~vvp_fun_modpath_src() override;

    عوامی:
      خالی recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override;
      مجازی bool test_vec4(ثابت vvp_vector4_t&ثنائی_ہندسہ);

      خالی get_delay12(vvp_time64_t برآمد[12]) ثابت;
      خالی put_delay12(ثابت vvp_time64_t اندر[12]);

    نجی:
	// FIXME: Needs to be a 12-value array
      vvp_time64_t delay_[12];
	// Used by vvp_fun_modpath to keep a list of modpath_src objects.
      vvp_fun_modpath_src*اگلا;

      vvp_time64_t wake_وقت;
      bool condition_flag_;

    نجی:
      vvp_fun_modpath_src(ثابت vvp_fun_modpath_src&);
      vvp_fun_modpath_src& عامل = (ثابت vvp_fun_modpath_src&);
};

class vvp_fun_modpath_کنارہ  : عوامی vvp_fun_modpath_src {

    عوامی:
      vvp_fun_modpath_کنارہ(vvp_time64_t del[12], bool pos, bool neg);

      bool test_vec4(ثابت vvp_vector4_t&ثنائی_ہندسہ) override;

    نجی:
      vvp_bit4_t old_قدر;
      bool posedge_;
      bool negedge_;
};

/*
* The intermodpath is used to implement the SDF INTERCONNECT feature
* Upon a (INTERCONNECT ...) statement an intermodpath will be inserted
* between port1 and port2 and its delay can be annotated
*/
class vvp_fun_intermodpath  : عوامی vvp_net_fun_t, نجی vvp_gen_event_s {

    عوامی:
      vvp_fun_intermodpath(vvp_net_t*net, غیرعلامتی چوڑائی);
      ~vvp_fun_intermodpath() override;

      خالی recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override;

      خالی get_delay12(vvp_time64_t برآمد[12]) ثابت;
      خالی put_delay12(ثابت vvp_time64_t اندر[12]);

    نجی:
      خالی تسلسل_تسلسل() override;

    نجی:
      vvp_net_t*net_;

      vvp_vector4_t cur_vec4_;

      vvp_time64_t delay_[12];

    نجی: // not implemented
      vvp_fun_intermodpath(ثابت vvp_fun_intermodpath&);
      vvp_fun_intermodpath& عامل= (ثابت vvp_fun_intermodpath&);
};

#اگر_ختم /* IVL_delay_H */
