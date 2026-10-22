#अगर_अघोषित IVL_delay_H
#घोषणा IVL_delay_H
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

क्रमागत delay_edge_t {
      DELAY_EDGE_01 = 0, DELAY_EDGE_10, DELAY_EDGE_0z,
      DELAY_EDGE_z1,     DELAY_EDGE_1z, DELAY_EDGE_z0,
      DELAY_EDGE_0x,     DELAY_EDGE_x1, DELAY_EDGE_1x,
      DELAY_EDGE_x0,     DELAY_EDGE_xz, DELAY_EDGE_zx,
      DELAY_EDGE_गिनती
};

/*
 * Instances of this object are functions that calculate the delay for
 * the transition from the source vvp_bit4_t value to the destination
 * value.
 */
class vvp_delay_t {

    खुला:
      vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall);
      vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall, vvp_time64_t decay);
      ~vvp_delay_t();

      vvp_time64_t get_delay(vvp_bit4_t from, vvp_bit4_t to);
      vvp_time64_t get_min_delay() स्थिर;

      व्योम set_rise(vvp_time64_t val);
      व्योम set_fall(vvp_time64_t val);
      व्योम set_decay(vvp_time64_t val);
      व्योम set_ignore_decay();

    निजी:
      vvp_time64_t rise_, fall_, decay_;
      vvp_time64_t min_delay_;
      bool ignore_decay_;

      व्योम calculate_min_delay_();
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
class vvp_fun_delay  : खुला vvp_net_fun_t, निजी vvp_gen_event_s {

      क्रमागत delay_type_t {UNKNOWN_DELAY, VEC4_DELAY, VEC8_DELAY, REAL_DELAY};
      काष्ठा event_ {
	    explicit event_(vvp_time64_t s) : sim_time(s) {
		  ptr_real = 0.0;
		  अग्र_2 = शून्य;
	    }
	    व्योम (vvp_fun_delay::*run_run_ptr)(स्थिर काष्ठा vvp_fun_delay::event_*cur);
	    स्थिर vvp_time64_t sim_time;
	    vvp_vector4_t ptr_vec4;
	    vvp_vector8_t ptr_vec8;
	    द्विगुन ptr_real;
	    काष्ठा event_*अग्र_2;
      };

    खुला:
      vvp_fun_delay(vvp_net_t*net, अचिन्हित width, स्थिर vvp_delay_t&d);
      ~vvp_fun_delay() override;

      व्योम recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                     vvp_context_t) override;
      व्योम recv_vec8(vvp_net_ptr_t port, स्थिर vvp_vector8_t&bit) override;
      व्योम recv_real(vvp_net_ptr_t port, द्विगुन bit,
                     vvp_context_t) override;

      व्योम recv_vec4_pv(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
			अचिन्हित base, अचिन्हित vwid, vvp_context_t ctx) override;
      व्योम recv_vec8_pv(vvp_net_ptr_t ptr, स्थिर vvp_vector8_t&bit,
			अचिन्हित base, अचिन्हित vwid) override;

    निजी:
      भव व्योम run_run() override;


      व्योम run_run_vec4_(स्थिर काष्ठा vvp_fun_delay::event_*cur);
      व्योम run_run_vec8_(स्थिर काष्ठा vvp_fun_delay::event_*cur);
      व्योम run_run_real_(स्थिर काष्ठा vvp_fun_delay::event_*cur);

    निजी:
      vvp_net_t*net_;
      vvp_delay_t delay_;
      delay_type_t type_;
      bool initial_; // Indicates if the value is still the initial value.

      vvp_vector4_t cur_vec4_;
      vvp_vector8_t cur_vec8_;
      द्विगुन cur_real_;
      vvp_time64_t round_, scale_; // Needed to scale variable time values.

      काष्ठा event_ *list_;
      व्योम enqueue_(काष्ठा event_*cur)
      {
	    अगर (list_) {
		  cur->अग्र_2 = list_->अग्र_2;
		  list_->अग्र_2 = cur;
		  list_ = cur;
	    } अन्यथा {
		  cur->अग्र_2 = cur;
		  list_ = cur;
	    }
      }
      काष्ठा event_* dequeue_(व्योम)
      {
	    अगर (list_ == 0)
		  वापस 0;
	    काष्ठा event_*cur = list_->अग्र_2;
	    अगर (list_ == cur)
		  list_ = 0;
	    अन्यथा
		  list_->अग्र_2 = cur->अग्र_2;
	    वापस cur;
      }
      bool clean_pulse_events_(vvp_time64_t use_delay, स्थिर vvp_vector4_t&bit);
      bool clean_pulse_events_(vvp_time64_t use_delay, स्थिर vvp_vector8_t&bit);
      bool clean_pulse_events_(vvp_time64_t use_delay, द्विगुन bit);
// Delete this when done!
      व्योम clean_pulse_events_(vvp_time64_t use_delay);
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

class vvp_fun_modpath  : खुला vvp_net_fun_t, निजी vvp_gen_event_s {

    खुला:
      vvp_fun_modpath(vvp_net_t*net, अचिन्हित width);
      ~vvp_fun_modpath() override;

      व्योम add_modpath_src(vvp_fun_modpath_src*that, bool ifnone);

      व्योम recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                     vvp_context_t) override;

    निजी:
      भव व्योम run_run() override;

    निजी:
      vvp_net_t*net_;

      vvp_vector4_t cur_vec4_;

      vvp_fun_modpath_src*src_list_;
      vvp_fun_modpath_src*ifnone_list_;

    निजी: // not implemented
      vvp_fun_modpath(स्थिर vvp_fun_modpath&);
      vvp_fun_modpath& चालक= (स्थिर vvp_fun_modpath&);
};

class vvp_fun_modpath_src  : खुला vvp_net_fun_t {

      मित्र class vvp_fun_modpath;

    खुला:
      explicit vvp_fun_modpath_src(vvp_time64_t स्थिर del[12]);
    रक्षित:
      ~vvp_fun_modpath_src() override;

    खुला:
      व्योम recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                     vvp_context_t) override;
      भव bool test_vec4(स्थिर vvp_vector4_t&bit);

      व्योम get_delay12(vvp_time64_t out[12]) स्थिर;
      व्योम put_delay12(स्थिर vvp_time64_t in[12]);

    निजी:
	// FIXME: Needs to be a 12-value array
      vvp_time64_t delay_[12];
	// Used by vvp_fun_modpath to keep a list of modpath_src objects.
      vvp_fun_modpath_src*अग्र;

      vvp_time64_t wake_time_;
      bool condition_flag_;

    निजी:
      vvp_fun_modpath_src(स्थिर vvp_fun_modpath_src&);
      vvp_fun_modpath_src& चालक = (स्थिर vvp_fun_modpath_src&);
};

class vvp_fun_modpath_edge  : खुला vvp_fun_modpath_src {

    खुला:
      vvp_fun_modpath_edge(vvp_time64_t del[12], bool pos, bool neg);

      bool test_vec4(स्थिर vvp_vector4_t&bit) override;

    निजी:
      vvp_bit4_t old_मूल्य;
      bool posedge_;
      bool negedge_;
};

/*
* The intermodpath is used to implement the SDF INTERCONNECT feature
* Upon a (INTERCONNECT ...) statement an intermodpath will be inserted
* between port1 and port2 and its delay can be annotated
*/
class vvp_fun_intermodpath  : खुला vvp_net_fun_t, निजी vvp_gen_event_s {

    खुला:
      vvp_fun_intermodpath(vvp_net_t*net, अचिन्हित width);
      ~vvp_fun_intermodpath() override;

      व्योम recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                     vvp_context_t) override;

      व्योम get_delay12(vvp_time64_t out[12]) स्थिर;
      व्योम put_delay12(स्थिर vvp_time64_t in[12]);

    निजी:
      व्योम run_run() override;

    निजी:
      vvp_net_t*net_;

      vvp_vector4_t cur_vec4_;

      vvp_time64_t delay_[12];

    निजी: // not implemented
      vvp_fun_intermodpath(स्थिर vvp_fun_intermodpath&);
      vvp_fun_intermodpath& चालक= (स्थिर vvp_fun_intermodpath&);
};

#पूर्ण_अगर /* IVL_delay_H */
