#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ IVL_delay_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IVL_delay_H
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

ਗਿਣਤੀ delay_ਕਿਨਾਰਾ_t {
      DELAY_ਕਿਨਾਰਾ_01 = 0, DELAY_ਕਿਨਾਰਾ_10, DELAY_ਕਿਨਾਰਾ_0z,
      DELAY_ਕਿਨਾਰਾ_z1,     DELAY_ਕਿਨਾਰਾ_1z, DELAY_ਕਿਨਾਰਾ_z0,
      DELAY_ਕਿਨਾਰਾ_0x,     DELAY_ਕਿਨਾਰਾ_x1, DELAY_ਕਿਨਾਰਾ_1x,
      DELAY_ਕਿਨਾਰਾ_x0,     DELAY_ਕਿਨਾਰਾ_xz, DELAY_ਕਿਨਾਰਾ_zx,
      DELAY_ਕਿਨਾਰਾ_ਗਿਣਤੀ
};

/*
 * Instances of this object are functions that calculate the delay for
 * the transition from the source vvp_bit4_t value to the destination
 * value.
 */
class vvp_delay_t {

    ਜਨਤਕ:
      vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall);
      vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall, vvp_time64_t decay);
      ~vvp_delay_t();

      vvp_time64_t get_delay(vvp_bit4_t from, vvp_bit4_t ਮੰਜ਼ਿਲ);
      vvp_time64_t get_min_delay() ਅਚਲ;

      ਖਾਲੀ ਸੈੱਟ_rise(vvp_time64_t val);
      ਖਾਲੀ ਸੈੱਟ_fall(vvp_time64_t val);
      ਖਾਲੀ ਸੈੱਟ_decay(vvp_time64_t val);
      ਖਾਲੀ ਸੈੱਟ_ignore_decay();

    ਨਿਜੀ:
      vvp_time64_t rise_, fall_, decay_;
      vvp_time64_t min_delay_;
      bool ignore_decay_;

      ਖਾਲੀ calculate_min_delay_();
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
class vvp_fun_delay  : ਜਨਤਕ vvp_net_fun_t, ਨਿਜੀ vvp_gen_event_s {

      ਗਿਣਤੀ delay_type_t {UNKNOWN_DELAY, VEC4_DELAY, VEC8_DELAY, REAL_DELAY};
      ਬਣਤਰ event_ {
	    explicit event_(vvp_time64_t s) : sim_ਸਮਾਂ(s) {
		  ptr_real = 0.0;
		  ਅਗਲਾ_2 = ਨਿਰਾ;
	    }
	    ਖਾਲੀ (vvp_fun_delay::*ਲੜੀਵਾਰ_ਲੜੀਵਾਰ_ptr)(ਅਚਲ ਬਣਤਰ vvp_fun_delay::event_*cur);
	    ਅਚਲ vvp_time64_t sim_ਸਮਾਂ;
	    vvp_vector4_t ptr_vec4;
	    vvp_vector8_t ptr_vec8;
	    ਦੁੱਗਣਾ ptr_real;
	    ਬਣਤਰ event_*ਅਗਲਾ_2;
      };

    ਜਨਤਕ:
      vvp_fun_delay(vvp_net_t*net, ਬਿਨਾਨਿਸ਼ਾਨੀ ਚੌੜਾਈ, ਅਚਲ vvp_delay_t&d);
      ~vvp_fun_delay() override;

      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;
      ਖਾਲੀ recv_vec8(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ) override;
      ਖਾਲੀ recv_real(vvp_net_ptr_t ਪੋਰਟ, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;

      ਖਾਲੀ recv_vec4_pv(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t ctx) override;
      ਖਾਲੀ recv_vec8_pv(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid) override;

    ਨਿਜੀ:
      ਆਭਾਸੀ ਖਾਲੀ ਲੜੀਵਾਰ_ਲੜੀਵਾਰ() override;


      ਖਾਲੀ ਲੜੀਵਾਰ_ਲੜੀਵਾਰ_vec4(ਅਚਲ ਬਣਤਰ vvp_fun_delay::event_*cur);
      ਖਾਲੀ ਲੜੀਵਾਰ_ਲੜੀਵਾਰ_vec8(ਅਚਲ ਬਣਤਰ vvp_fun_delay::event_*cur);
      ਖਾਲੀ ਲੜੀਵਾਰ_ਲੜੀਵਾਰ_real(ਅਚਲ ਬਣਤਰ vvp_fun_delay::event_*cur);

    ਨਿਜੀ:
      vvp_net_t*net_;
      vvp_delay_t delay_;
      delay_type_t type_;
      bool initial_; // Indicates if the value is still the initial value.

      vvp_vector4_t cur_vec4_;
      vvp_vector8_t cur_vec8_;
      ਦੁੱਗਣਾ cur_real_;
      vvp_time64_t ਗੇੜਾ, scale_; // Needed to scale variable time values.

      ਬਣਤਰ event_ *list_;
      ਖਾਲੀ ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(ਬਣਤਰ event_*cur)
      {
	    ਜੇ (list_) {
		  cur->ਅਗਲਾ_2 = list_->ਅਗਲਾ_2;
		  list_->ਅਗਲਾ_2 = cur;
		  list_ = cur;
	    } ਨਹੀਂਤਾਂ {
		  cur->ਅਗਲਾ_2 = cur;
		  list_ = cur;
	    }
      }
      ਬਣਤਰ event_* ਕਤਾਰ_ਤੋਂ_ਕੱਢੋ(ਖਾਲੀ)
      {
	    ਜੇ (list_ == 0)
		  ਵਾਪਸ 0;
	    ਬਣਤਰ event_*cur = list_->ਅਗਲਾ_2;
	    ਜੇ (list_ == cur)
		  list_ = 0;
	    ਨਹੀਂਤਾਂ
		  list_->ਅਗਲਾ_2 = cur->ਅਗਲਾ_2;
	    ਵਾਪਸ cur;
      }
      bool clean_pulse_events_(vvp_time64_t use_delay, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ);
      bool clean_pulse_events_(vvp_time64_t use_delay, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ);
      bool clean_pulse_events_(vvp_time64_t use_delay, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ);
// Delete this when done!
      ਖਾਲੀ clean_pulse_events_(vvp_time64_t use_delay);
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

class vvp_fun_modpath  : ਜਨਤਕ vvp_net_fun_t, ਨਿਜੀ vvp_gen_event_s {

    ਜਨਤਕ:
      vvp_fun_modpath(vvp_net_t*net, ਬਿਨਾਨਿਸ਼ਾਨੀ ਚੌੜਾਈ);
      ~vvp_fun_modpath() override;

      ਖਾਲੀ ਜੋੜੋ_modpath_src(vvp_fun_modpath_src*that, bool ifnone);

      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;

    ਨਿਜੀ:
      ਆਭਾਸੀ ਖਾਲੀ ਲੜੀਵਾਰ_ਲੜੀਵਾਰ() override;

    ਨਿਜੀ:
      vvp_net_t*net_;

      vvp_vector4_t cur_vec4_;

      vvp_fun_modpath_src*src_list_;
      vvp_fun_modpath_src*ifnone_list_;

    ਨਿਜੀ: // not implemented
      vvp_fun_modpath(ਅਚਲ vvp_fun_modpath&);
      vvp_fun_modpath& ਸੰਚਾਲਕ= (ਅਚਲ vvp_fun_modpath&);
};

class vvp_fun_modpath_src  : ਜਨਤਕ vvp_net_fun_t {

      ਮਿੱਤਰ class vvp_fun_modpath;

    ਜਨਤਕ:
      explicit vvp_fun_modpath_src(vvp_time64_t ਅਚਲ del[12]);
    ਸੁਰੱਖਿਅਤ:
      ~vvp_fun_modpath_src() override;

    ਜਨਤਕ:
      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;
      ਆਭਾਸੀ bool test_vec4(ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ);

      ਖਾਲੀ get_delay12(vvp_time64_t ਬਾਹਰਲਾ[12]) ਅਚਲ;
      ਖਾਲੀ put_delay12(ਅਚਲ vvp_time64_t ਵਿੱਚ[12]);

    ਨਿਜੀ:
	// FIXME: Needs to be a 12-value array
      vvp_time64_t delay_[12];
	// Used by vvp_fun_modpath to keep a list of modpath_src objects.
      vvp_fun_modpath_src*ਅਗਲਾ;

      vvp_time64_t wake_ਸਮਾਂ;
      bool condition_flag_;

    ਨਿਜੀ:
      vvp_fun_modpath_src(ਅਚਲ vvp_fun_modpath_src&);
      vvp_fun_modpath_src& ਸੰਚਾਲਕ = (ਅਚਲ vvp_fun_modpath_src&);
};

class vvp_fun_modpath_ਕਿਨਾਰਾ  : ਜਨਤਕ vvp_fun_modpath_src {

    ਜਨਤਕ:
      vvp_fun_modpath_ਕਿਨਾਰਾ(vvp_time64_t del[12], bool pos, bool neg);

      bool test_vec4(ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ) override;

    ਨਿਜੀ:
      vvp_bit4_t old_ਮੁੱਲ;
      bool posedge_;
      bool negedge_;
};

/*
* The intermodpath is used to implement the SDF INTERCONNECT feature
* Upon a (INTERCONNECT ...) statement an intermodpath will be inserted
* between port1 and port2 and its delay can be annotated
*/
class vvp_fun_intermodpath  : ਜਨਤਕ vvp_net_fun_t, ਨਿਜੀ vvp_gen_event_s {

    ਜਨਤਕ:
      vvp_fun_intermodpath(vvp_net_t*net, ਬਿਨਾਨਿਸ਼ਾਨੀ ਚੌੜਾਈ);
      ~vvp_fun_intermodpath() override;

      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;

      ਖਾਲੀ get_delay12(vvp_time64_t ਬਾਹਰਲਾ[12]) ਅਚਲ;
      ਖਾਲੀ put_delay12(ਅਚਲ vvp_time64_t ਵਿੱਚ[12]);

    ਨਿਜੀ:
      ਖਾਲੀ ਲੜੀਵਾਰ_ਲੜੀਵਾਰ() override;

    ਨਿਜੀ:
      vvp_net_t*net_;

      vvp_vector4_t cur_vec4_;

      vvp_time64_t delay_[12];

    ਨਿਜੀ: // not implemented
      vvp_fun_intermodpath(ਅਚਲ vvp_fun_intermodpath&);
      vvp_fun_intermodpath& ਸੰਚਾਲਕ= (ਅਚਲ vvp_fun_intermodpath&);
};

#ਜੇ_ਅੰਤ /* IVL_delay_H */
