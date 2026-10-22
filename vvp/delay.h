#यदि_अपरिभाषितम् IVL_delay_H
#परिभाषय IVL_delay_H
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

गणना delay_edge_t {
      DELAY_EDGE_01 = 0, DELAY_EDGE_10, DELAY_EDGE_0z,
      DELAY_EDGE_z1,     DELAY_EDGE_1z, DELAY_EDGE_z0,
      DELAY_EDGE_0x,     DELAY_EDGE_x1, DELAY_EDGE_1x,
      DELAY_EDGE_x0,     DELAY_EDGE_xz, DELAY_EDGE_zx,
      DELAY_EDGE_गणनम्
};

/*
 * Instances of this object are functions that calculate the delay for
 * the transition from the source vvp_bit4_t value to the destination
 * value.
 */
class vvp_delay_t {

    सार्वजनिक:
      vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall);
      vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall, vvp_time64_t decay);
      ~vvp_delay_t();

      vvp_time64_t get_delay(vvp_bit4_t from, vvp_bit4_t गन्तव्यम्);
      vvp_time64_t get_min_delay() ध्रुव;

      शून्य समुच्चयः_rise(vvp_time64_t val);
      शून्य समुच्चयः_fall(vvp_time64_t val);
      शून्य समुच्चयः_decay(vvp_time64_t val);
      शून्य समुच्चयः_ignore_decay();

    गुप्त:
      vvp_time64_t rise_, fall_, decay_;
      vvp_time64_t min_delay_;
      bool ignore_decay_;

      शून्य calculate_min_delay_();
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
class vvp_fun_delay  : सार्वजनिक vvp_net_fun_t, गुप्त vvp_gen_event_s {

      गणना delay_type_t {UNKNOWN_DELAY, VEC4_DELAY, VEC8_DELAY, REAL_DELAY};
      संरचना event_ {
	    explicit event_(vvp_time64_t s) : sim_time(s) {
		  ptr_real = 0.0;
		  अग्र_2 = रिक्त;
	    }
	    शून्य (vvp_fun_delay::*सततम्_सततम्_ptr)(ध्रुव संरचना vvp_fun_delay::event_*cur);
	    ध्रुव vvp_time64_t sim_time;
	    vvp_vector4_t ptr_vec4;
	    vvp_vector8_t ptr_vec8;
	    द्विगुण ptr_real;
	    संरचना event_*अग्र_2;
      };

    सार्वजनिक:
      vvp_fun_delay(vvp_net_t*net, अचिह्नित विस्तारः, ध्रुव vvp_delay_t&d);
      ~vvp_fun_delay() override;

      शून्य recv_vec4(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t) override;
      शून्य recv_vec8(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector8_t&bit) override;
      शून्य recv_real(vvp_net_ptr_t पत्तनम्, द्विगुण bit,
                     vvp_context_t) override;

      शून्य recv_vec4_pv(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
			अचिह्नित आधारः, अचिह्नित vwid, vvp_context_t ctx) override;
      शून्य recv_vec8_pv(vvp_net_ptr_t ptr, ध्रुव vvp_vector8_t&bit,
			अचिह्नित आधारः, अचिह्नित vwid) override;

    गुप्त:
      आभास शून्य सततम्_सततम्() override;


      शून्य सततम्_सततम्_vec4(ध्रुव संरचना vvp_fun_delay::event_*cur);
      शून्य सततम्_सततम्_vec8(ध्रुव संरचना vvp_fun_delay::event_*cur);
      शून्य सततम्_सततम्_real(ध्रुव संरचना vvp_fun_delay::event_*cur);

    गुप्त:
      vvp_net_t*net_;
      vvp_delay_t delay_;
      delay_type_t type_;
      bool initial_; // Indicates if the value is still the initial value.

      vvp_vector4_t cur_vec4_;
      vvp_vector8_t cur_vec8_;
      द्विगुण cur_real_;
      vvp_time64_t आवर्तनम्, scale_; // Needed to scale variable time values.

      संरचना event_ *list_;
      शून्य पङ्क्तौ_निक्षिप(संरचना event_*cur)
      {
	    यदि (list_) {
		  cur->अग्र_2 = list_->अग्र_2;
		  list_->अग्र_2 = cur;
		  list_ = cur;
	    } अन्यथा {
		  cur->अग्र_2 = cur;
		  list_ = cur;
	    }
      }
      संरचना event_* पङ्क्तेः_निष्कासय(शून्य)
      {
	    यदि (list_ == 0)
		  प्रत्यागम 0;
	    संरचना event_*cur = list_->अग्र_2;
	    यदि (list_ == cur)
		  list_ = 0;
	    अन्यथा
		  list_->अग्र_2 = cur->अग्र_2;
	    प्रत्यागम cur;
      }
      bool clean_pulse_events_(vvp_time64_t use_delay, ध्रुव vvp_vector4_t&bit);
      bool clean_pulse_events_(vvp_time64_t use_delay, ध्रुव vvp_vector8_t&bit);
      bool clean_pulse_events_(vvp_time64_t use_delay, द्विगुण bit);
// Delete this when done!
      शून्य clean_pulse_events_(vvp_time64_t use_delay);
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

class vvp_fun_modpath  : सार्वजनिक vvp_net_fun_t, गुप्त vvp_gen_event_s {

    सार्वजनिक:
      vvp_fun_modpath(vvp_net_t*net, अचिह्नित विस्तारः);
      ~vvp_fun_modpath() override;

      शून्य सङ्कलय_modpath_src(vvp_fun_modpath_src*that, bool ifnone);

      शून्य recv_vec4(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t) override;

    गुप्त:
      आभास शून्य सततम्_सततम्() override;

    गुप्त:
      vvp_net_t*net_;

      vvp_vector4_t cur_vec4_;

      vvp_fun_modpath_src*src_list_;
      vvp_fun_modpath_src*ifnone_list_;

    गुप्त: // not implemented
      vvp_fun_modpath(ध्रुव vvp_fun_modpath&);
      vvp_fun_modpath& संक्रियकः= (ध्रुव vvp_fun_modpath&);
};

class vvp_fun_modpath_src  : सार्वजनिक vvp_net_fun_t {

      मित्रम् class vvp_fun_modpath;

    सार्वजनिक:
      explicit vvp_fun_modpath_src(vvp_time64_t ध्रुव del[12]);
    रक्षित:
      ~vvp_fun_modpath_src() override;

    सार्वजनिक:
      शून्य recv_vec4(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t) override;
      आभास bool test_vec4(ध्रुव vvp_vector4_t&bit);

      शून्य get_delay12(vvp_time64_t बहिर्गमनम्[12]) ध्रुव;
      शून्य put_delay12(ध्रुव vvp_time64_t अन्तर्[12]);

    गुप्त:
	// FIXME: Needs to be a 12-value array
      vvp_time64_t delay_[12];
	// Used by vvp_fun_modpath to keep a list of modpath_src objects.
      vvp_fun_modpath_src*अग्र;

      vvp_time64_t wake_time_;
      bool condition_flag_;

    गुप्त:
      vvp_fun_modpath_src(ध्रुव vvp_fun_modpath_src&);
      vvp_fun_modpath_src& संक्रियकः = (ध्रुव vvp_fun_modpath_src&);
};

class vvp_fun_modpath_edge  : सार्वजनिक vvp_fun_modpath_src {

    सार्वजनिक:
      vvp_fun_modpath_edge(vvp_time64_t del[12], bool pos, bool neg);

      bool test_vec4(ध्रुव vvp_vector4_t&bit) override;

    गुप्त:
      vvp_bit4_t old_मानम्;
      bool posedge_;
      bool negedge_;
};

/*
* The intermodpath is used to implement the SDF INTERCONNECT feature
* Upon a (INTERCONNECT ...) statement an intermodpath will be inserted
* between port1 and port2 and its delay can be annotated
*/
class vvp_fun_intermodpath  : सार्वजनिक vvp_net_fun_t, गुप्त vvp_gen_event_s {

    सार्वजनिक:
      vvp_fun_intermodpath(vvp_net_t*net, अचिह्नित विस्तारः);
      ~vvp_fun_intermodpath() override;

      शून्य recv_vec4(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t) override;

      शून्य get_delay12(vvp_time64_t बहिर्गमनम्[12]) ध्रुव;
      शून्य put_delay12(ध्रुव vvp_time64_t अन्तर्[12]);

    गुप्त:
      शून्य सततम्_सततम्() override;

    गुप्त:
      vvp_net_t*net_;

      vvp_vector4_t cur_vec4_;

      vvp_time64_t delay_[12];

    गुप्त: // not implemented
      vvp_fun_intermodpath(ध्रुव vvp_fun_intermodpath&);
      vvp_fun_intermodpath& संक्रियकः= (ध्रुव vvp_fun_intermodpath&);
};

#यदि_अन्तः /* IVL_delay_H */
