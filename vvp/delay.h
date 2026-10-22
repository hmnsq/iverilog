#ifndef IVL_delay_H
#define IVL_delay_H
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

تعداد delay_edge_t {
      DELAY_EDGE_01 = 0, DELAY_EDGE_10, DELAY_EDGE_0z,
      DELAY_EDGE_z1,     DELAY_EDGE_1z, DELAY_EDGE_z0,
      DELAY_EDGE_0x,     DELAY_EDGE_x1, DELAY_EDGE_1x,
      DELAY_EDGE_x0,     DELAY_EDGE_xz, DELAY_EDGE_zx,
      DELAY_EDGE_سانى
};

/*
 * Instances of this object are functions that calculate the delay for
 * the transition from the source vvp_bit4_t value to the destination
 * value.
 */
class vvp_delay_t {

    عام:
      vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall);
      vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall, vvp_time64_t decay);
      ~vvp_delay_t();

      vvp_time64_t get_delay(vvp_bit4_t from, vvp_bit4_t to);
      vvp_time64_t get_min_delay() ثابت;

      فراغ set_rise(vvp_time64_t val);
      فراغ set_fall(vvp_time64_t val);
      فراغ set_decay(vvp_time64_t val);
      فراغ set_ignore_decay();

    خاص:
      vvp_time64_t rise_, fall_, decay_;
      vvp_time64_t min_delay_;
      bool ignore_decay_;

      فراغ calculate_min_delay_();
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
class vvp_fun_delay  : عام vvp_net_fun_t, خاص vvp_gen_event_s {

      تعداد delay_type_t {UNKNOWN_DELAY, VEC4_DELAY, VEC8_DELAY, REAL_DELAY};
      بنية event_ {
	    explicit event_(vvp_time64_t s) : sim_time(s) {
		  ptr_real = 0.0;
		  next = عدم;
	    }
	    فراغ (vvp_fun_delay::*run_run_ptr)(ثابت بنية vvp_fun_delay::event_*cur);
	    ثابت vvp_time64_t sim_time;
	    vvp_vector4_t ptr_vec4;
	    vvp_vector8_t ptr_vec8;
	    مزدوج ptr_real;
	    بنية event_*next;
      };

    عام:
      vvp_fun_delay(vvp_net_t*net, unsigned width, ثابت vvp_delay_t&d);
      ~vvp_fun_delay() override;

      فراغ recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                     vvp_context_t) override;
      فراغ recv_vec8(vvp_net_ptr_t port, ثابت vvp_vector8_t&bit) override;
      فراغ recv_real(vvp_net_ptr_t port, مزدوج bit,
                     vvp_context_t) override;

      فراغ recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t ctx) override;
      فراغ recv_vec8_pv(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&bit,
			unsigned base, unsigned vwid) override;

    خاص:
      virtual فراغ run_run() override;


      فراغ run_run_vec4_(ثابت بنية vvp_fun_delay::event_*cur);
      فراغ run_run_vec8_(ثابت بنية vvp_fun_delay::event_*cur);
      فراغ run_run_real_(ثابت بنية vvp_fun_delay::event_*cur);

    خاص:
      vvp_net_t*net_;
      vvp_delay_t delay_;
      delay_type_t type_;
      bool initial_; // Indicates if the value is still the initial value.

      vvp_vector4_t cur_vec4_;
      vvp_vector8_t cur_vec8_;
      مزدوج cur_real_;
      vvp_time64_t round_, scale_; // Needed to scale variable time values.

      بنية event_ *list_;
      فراغ enqueue_(بنية event_*cur)
      {
	    إذا (list_) {
		  cur->next = list_->next;
		  list_->next = cur;
		  list_ = cur;
	    } وإلا {
		  cur->next = cur;
		  list_ = cur;
	    }
      }
      بنية event_* dequeue_(فراغ)
      {
	    إذا (list_ == 0)
		  أرجع 0;
	    بنية event_*cur = list_->next;
	    إذا (list_ == cur)
		  list_ = 0;
	    وإلا
		  list_->next = cur->next;
	    أرجع cur;
      }
      bool clean_pulse_events_(vvp_time64_t use_delay, ثابت vvp_vector4_t&bit);
      bool clean_pulse_events_(vvp_time64_t use_delay, ثابت vvp_vector8_t&bit);
      bool clean_pulse_events_(vvp_time64_t use_delay, مزدوج bit);
// Delete this when done!
      فراغ clean_pulse_events_(vvp_time64_t use_delay);
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

class vvp_fun_modpath  : عام vvp_net_fun_t, خاص vvp_gen_event_s {

    عام:
      vvp_fun_modpath(vvp_net_t*net, unsigned width);
      ~vvp_fun_modpath() override;

      فراغ add_modpath_src(vvp_fun_modpath_src*that, bool ifnone);

      فراغ recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                     vvp_context_t) override;

    خاص:
      virtual فراغ run_run() override;

    خاص:
      vvp_net_t*net_;

      vvp_vector4_t cur_vec4_;

      vvp_fun_modpath_src*src_list_;
      vvp_fun_modpath_src*ifnone_list_;

    خاص: // not implemented
      vvp_fun_modpath(ثابت vvp_fun_modpath&);
      vvp_fun_modpath& operator= (ثابت vvp_fun_modpath&);
};

class vvp_fun_modpath_src  : عام vvp_net_fun_t {

      friend class vvp_fun_modpath;

    عام:
      explicit vvp_fun_modpath_src(vvp_time64_t ثابت del[12]);
    محمي:
      ~vvp_fun_modpath_src() override;

    عام:
      فراغ recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                     vvp_context_t) override;
      virtual bool test_vec4(ثابت vvp_vector4_t&bit);

      فراغ get_delay12(vvp_time64_t out[12]) ثابت;
      فراغ put_delay12(ثابت vvp_time64_t in[12]);

    خاص:
	// FIXME: Needs to be a 12-value array
      vvp_time64_t delay_[12];
	// Used by vvp_fun_modpath to keep a list of modpath_src objects.
      vvp_fun_modpath_src*next_;

      vvp_time64_t wake_time_;
      bool condition_flag_;

    خاص:
      vvp_fun_modpath_src(ثابت vvp_fun_modpath_src&);
      vvp_fun_modpath_src& operator = (ثابت vvp_fun_modpath_src&);
};

class vvp_fun_modpath_edge  : عام vvp_fun_modpath_src {

    عام:
      vvp_fun_modpath_edge(vvp_time64_t del[12], bool pos, bool neg);

      bool test_vec4(ثابت vvp_vector4_t&bit) override;

    خاص:
      vvp_bit4_t old_قىممەت;
      bool posedge_;
      bool negedge_;
};

/*
* The intermodpath is used to implement the SDF INTERCONNECT feature
* Upon a (INTERCONNECT ...) statement an intermodpath will be inserted
* between port1 and port2 and its delay can be annotated
*/
class vvp_fun_intermodpath  : عام vvp_net_fun_t, خاص vvp_gen_event_s {

    عام:
      vvp_fun_intermodpath(vvp_net_t*net, unsigned width);
      ~vvp_fun_intermodpath() override;

      فراغ recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                     vvp_context_t) override;

      فراغ get_delay12(vvp_time64_t out[12]) ثابت;
      فراغ put_delay12(ثابت vvp_time64_t in[12]);

    خاص:
      فراغ run_run() override;

    خاص:
      vvp_net_t*net_;

      vvp_vector4_t cur_vec4_;

      vvp_time64_t delay_[12];

    خاص: // not implemented
      vvp_fun_intermodpath(ثابت vvp_fun_intermodpath&);
      vvp_fun_intermodpath& operator= (ثابت vvp_fun_intermodpath&);
};

#endif /* IVL_delay_H */
