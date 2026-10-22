#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ IVL_delay_H
#ವ್ಯಾಖ್ಯಾನಿಸು IVL_delay_H
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

ಎಣಿಕೆ delay_ಅಂಚು_t {
      DELAY_ಅಂಚು_01 = 0, DELAY_ಅಂಚು_10, DELAY_ಅಂಚು_0z,
      DELAY_ಅಂಚು_z1,     DELAY_ಅಂಚು_1z, DELAY_ಅಂಚು_z0,
      DELAY_ಅಂಚು_0x,     DELAY_ಅಂಚು_x1, DELAY_ಅಂಚು_1x,
      DELAY_ಅಂಚು_x0,     DELAY_ಅಂಚು_xz, DELAY_ಅಂಚು_zx,
      DELAY_ಅಂಚು_ಎಣಿಕೆ
};

/*
 * Instances of this object are functions that calculate the delay for
 * the transition from the source vvp_bit4_t value to the destination
 * value.
 */
class vvp_delay_t {

    ಸಾರ್ವಜನಿಕ:
      vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall);
      vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall, vvp_time64_t decay);
      ~vvp_delay_t();

      vvp_time64_t get_delay(vvp_bit4_t from, vvp_bit4_t ಗಮ್ಯ);
      vvp_time64_t get_min_delay() ಸ್ಥಿರಾಂಕ;

      ಶೂನ್ಯ ಗಣ_rise(vvp_time64_t val);
      ಶೂನ್ಯ ಗಣ_fall(vvp_time64_t val);
      ಶೂನ್ಯ ಗಣ_decay(vvp_time64_t val);
      ಶೂನ್ಯ ಗಣ_ignore_decay();

    ಖಾಸಗಿ:
      vvp_time64_t rise_, fall_, decay_;
      vvp_time64_t min_delay_;
      bool ignore_decay_;

      ಶೂನ್ಯ calculate_min_delay_();
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
class vvp_fun_delay  : ಸಾರ್ವಜನಿಕ vvp_net_fun_t, ಖಾಸಗಿ vvp_gen_event_s {

      ಎಣಿಕೆ delay_type_t {UNKNOWN_DELAY, VEC4_DELAY, VEC8_DELAY, REAL_DELAY};
      ರಚನೆ event_ {
	    explicit event_(vvp_time64_t s) : sim_ಸಮಯ(s) {
		  ptr_real = 0.0;
		  ಮುಂದಿನ_2 = ಖಾಲಿ;
	    }
	    ಶೂನ್ಯ (vvp_fun_delay::*ಸತತ_ಸತತ_ptr)(ಸ್ಥಿರಾಂಕ ರಚನೆ vvp_fun_delay::event_*cur);
	    ಸ್ಥಿರಾಂಕ vvp_time64_t sim_ಸಮಯ;
	    vvp_vector4_t ptr_vec4;
	    vvp_vector8_t ptr_vec8;
	    ದ್ವಿಗುಣ ptr_real;
	    ರಚನೆ event_*ಮುಂದಿನ_2;
      };

    ಸಾರ್ವಜನಿಕ:
      vvp_fun_delay(vvp_net_t*net, ಚಿಹ್ನೆರಹಿತ ಅಗಲ, ಸ್ಥಿರಾಂಕ vvp_delay_t&d);
      ~vvp_fun_delay() override;

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                     vvp_context_t) override;
      ಶೂನ್ಯ recv_vec8(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector8_t&ಬಿಟ್) override;
      ಶೂನ್ಯ recv_real(vvp_net_ptr_t ಪೋರ್ಟ್, ದ್ವಿಗುಣ ಬಿಟ್,
                     vvp_context_t) override;

      ಶೂನ್ಯ recv_vec4_pv(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
			ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t ctx) override;
      ಶೂನ್ಯ recv_vec8_pv(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector8_t&ಬಿಟ್,
			ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid) override;

    ಖಾಸಗಿ:
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಸತತ_ಸತತ() override;


      ಶೂನ್ಯ ಸತತ_ಸತತ_vec4(ಸ್ಥಿರಾಂಕ ರಚನೆ vvp_fun_delay::event_*cur);
      ಶೂನ್ಯ ಸತತ_ಸತತ_vec8(ಸ್ಥಿರಾಂಕ ರಚನೆ vvp_fun_delay::event_*cur);
      ಶೂನ್ಯ ಸತತ_ಸತತ_real(ಸ್ಥಿರಾಂಕ ರಚನೆ vvp_fun_delay::event_*cur);

    ಖಾಸಗಿ:
      vvp_net_t*net_;
      vvp_delay_t delay_;
      delay_type_t type_;
      bool initial_; // Indicates if the value is still the initial value.

      vvp_vector4_t cur_vec4_;
      vvp_vector8_t cur_vec8_;
      ದ್ವಿಗುಣ cur_real_;
      vvp_time64_t ಸುತ್ತು, scale_; // Needed to scale variable time values.

      ರಚನೆ event_ *list_;
      ಶೂನ್ಯ ಸರತಿಗೆ_ಸೇರಿಸು(ರಚನೆ event_*cur)
      {
	    ಆದರೆ (list_) {
		  cur->ಮುಂದಿನ_2 = list_->ಮುಂದಿನ_2;
		  list_->ಮುಂದಿನ_2 = cur;
		  list_ = cur;
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  cur->ಮುಂದಿನ_2 = cur;
		  list_ = cur;
	    }
      }
      ರಚನೆ event_* ಸರತಿಯಿಂದ_ತೆಗೆ(ಶೂನ್ಯ)
      {
	    ಆದರೆ (list_ == 0)
		  ಹಿಂತಿರುಗಿ 0;
	    ರಚನೆ event_*cur = list_->ಮುಂದಿನ_2;
	    ಆದರೆ (list_ == cur)
		  list_ = 0;
	    ಇಲ್ಲದಿದ್ದರೆ
		  list_->ಮುಂದಿನ_2 = cur->ಮುಂದಿನ_2;
	    ಹಿಂತಿರುಗಿ cur;
      }
      bool clean_pulse_events_(vvp_time64_t use_delay, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್);
      bool clean_pulse_events_(vvp_time64_t use_delay, ಸ್ಥಿರಾಂಕ vvp_vector8_t&ಬಿಟ್);
      bool clean_pulse_events_(vvp_time64_t use_delay, ದ್ವಿಗುಣ ಬಿಟ್);
// Delete this when done!
      ಶೂನ್ಯ clean_pulse_events_(vvp_time64_t use_delay);
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

class vvp_fun_modpath  : ಸಾರ್ವಜನಿಕ vvp_net_fun_t, ಖಾಸಗಿ vvp_gen_event_s {

    ಸಾರ್ವಜನಿಕ:
      vvp_fun_modpath(vvp_net_t*net, ಚಿಹ್ನೆರಹಿತ ಅಗಲ);
      ~vvp_fun_modpath() override;

      ಶೂನ್ಯ ಕೂಡಿಸು_modpath_src(vvp_fun_modpath_src*that, bool ifnone);

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                     vvp_context_t) override;

    ಖಾಸಗಿ:
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಸತತ_ಸತತ() override;

    ಖಾಸಗಿ:
      vvp_net_t*net_;

      vvp_vector4_t cur_vec4_;

      vvp_fun_modpath_src*src_list_;
      vvp_fun_modpath_src*ifnone_list_;

    ಖಾಸಗಿ: // not implemented
      vvp_fun_modpath(ಸ್ಥಿರಾಂಕ vvp_fun_modpath&);
      vvp_fun_modpath& ಕ್ರಿಯಾಕಾರಕ= (ಸ್ಥಿರಾಂಕ vvp_fun_modpath&);
};

class vvp_fun_modpath_src  : ಸಾರ್ವಜನಿಕ vvp_net_fun_t {

      ಮಿತ್ರ class vvp_fun_modpath;

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_modpath_src(vvp_time64_t ಸ್ಥಿರಾಂಕ del[12]);
    ಸಂರಕ್ಷಿತ:
      ~vvp_fun_modpath_src() override;

    ಸಾರ್ವಜನಿಕ:
      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                     vvp_context_t) override;
      ವಾಸ್ತವಿಕ bool test_vec4(ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್);

      ಶೂನ್ಯ get_delay12(vvp_time64_t ಹೊರಹೋಗು[12]) ಸ್ಥಿರಾಂಕ;
      ಶೂನ್ಯ put_delay12(ಸ್ಥಿರಾಂಕ vvp_time64_t ಒಳ[12]);

    ಖಾಸಗಿ:
	// FIXME: Needs to be a 12-value array
      vvp_time64_t delay_[12];
	// Used by vvp_fun_modpath to keep a list of modpath_src objects.
      vvp_fun_modpath_src*ಮುಂದಿನ;

      vvp_time64_t wake_ಸಮಯ;
      bool condition_flag_;

    ಖಾಸಗಿ:
      vvp_fun_modpath_src(ಸ್ಥಿರಾಂಕ vvp_fun_modpath_src&);
      vvp_fun_modpath_src& ಕ್ರಿಯಾಕಾರಕ = (ಸ್ಥಿರಾಂಕ vvp_fun_modpath_src&);
};

class vvp_fun_modpath_ಅಂಚು  : ಸಾರ್ವಜನಿಕ vvp_fun_modpath_src {

    ಸಾರ್ವಜನಿಕ:
      vvp_fun_modpath_ಅಂಚು(vvp_time64_t del[12], bool pos, bool neg);

      bool test_vec4(ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್) override;

    ಖಾಸಗಿ:
      vvp_bit4_t old_ಮೌಲ್ಯ;
      bool posedge_;
      bool negedge_;
};

/*
* The intermodpath is used to implement the SDF INTERCONNECT feature
* Upon a (INTERCONNECT ...) statement an intermodpath will be inserted
* between port1 and port2 and its delay can be annotated
*/
class vvp_fun_intermodpath  : ಸಾರ್ವಜನಿಕ vvp_net_fun_t, ಖಾಸಗಿ vvp_gen_event_s {

    ಸಾರ್ವಜನಿಕ:
      vvp_fun_intermodpath(vvp_net_t*net, ಚಿಹ್ನೆರಹಿತ ಅಗಲ);
      ~vvp_fun_intermodpath() override;

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                     vvp_context_t) override;

      ಶೂನ್ಯ get_delay12(vvp_time64_t ಹೊರಹೋಗು[12]) ಸ್ಥಿರಾಂಕ;
      ಶೂನ್ಯ put_delay12(ಸ್ಥಿರಾಂಕ vvp_time64_t ಒಳ[12]);

    ಖಾಸಗಿ:
      ಶೂನ್ಯ ಸತತ_ಸತತ() override;

    ಖಾಸಗಿ:
      vvp_net_t*net_;

      vvp_vector4_t cur_vec4_;

      vvp_time64_t delay_[12];

    ಖಾಸಗಿ: // not implemented
      vvp_fun_intermodpath(ಸ್ಥಿರಾಂಕ vvp_fun_intermodpath&);
      vvp_fun_intermodpath& ಕ್ರಿಯಾಕಾರಕ= (ಸ್ಥಿರಾಂಕ vvp_fun_intermodpath&);
};

#ಆದರೆ_ಅಂತ್ಯ /* IVL_delay_H */
