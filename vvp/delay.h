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

গণনা delay_edge_t {
      DELAY_EDGE_01 = 0, DELAY_EDGE_10, DELAY_EDGE_0z,
      DELAY_EDGE_z1,     DELAY_EDGE_1z, DELAY_EDGE_z0,
      DELAY_EDGE_0x,     DELAY_EDGE_x1, DELAY_EDGE_1x,
      DELAY_EDGE_x0,     DELAY_EDGE_xz, DELAY_EDGE_zx,
      DELAY_EDGE_গুনতি
};

/*
 * Instances of this object are functions that calculate the delay for
 * the transition from the source vvp_bit4_t value to the destination
 * value.
 */
class vvp_delay_t {

    সার্ৱজনিক:
      vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall);
      vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall, vvp_time64_t decay);
      ~vvp_delay_t();

      vvp_time64_t get_delay(vvp_bit4_t from, vvp_bit4_t to);
      vvp_time64_t get_min_delay() ধ্রুৱ;

      শূন্য set_rise(vvp_time64_t val);
      শূন্য set_fall(vvp_time64_t val);
      শূন্য set_decay(vvp_time64_t val);
      শূন্য set_ignore_decay();

    গুপ্ত:
      vvp_time64_t rise_, fall_, decay_;
      vvp_time64_t min_delay_;
      bool ignore_decay_;

      শূন্য calculate_min_delay_();
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
class vvp_fun_delay  : সার্ৱজনিক vvp_net_fun_t, গুপ্ত vvp_gen_event_s {

      গণনা delay_type_t {UNKNOWN_DELAY, VEC4_DELAY, VEC8_DELAY, REAL_DELAY};
      সংরচনা event_ {
	    explicit event_(vvp_time64_t s) : sim_time(s) {
		  ptr_real = 0.0;
		  next = রিক্ত;
	    }
	    শূন্য (vvp_fun_delay::*run_run_ptr)(ধ্রুৱ সংরচনা vvp_fun_delay::event_*cur);
	    ধ্রুৱ vvp_time64_t sim_time;
	    vvp_vector4_t ptr_vec4;
	    vvp_vector8_t ptr_vec8;
	    দ্ৱিগুণ ptr_real;
	    সংরচনা event_*next;
      };

    সার্ৱজনিক:
      vvp_fun_delay(vvp_net_t*net, unsigned width, ধ্রুৱ vvp_delay_t&d);
      ~vvp_fun_delay() override;

      শূন্য recv_vec4(vvp_net_ptr_t port, ধ্রুৱ vvp_vector4_t&bit,
                     vvp_context_t) override;
      শূন্য recv_vec8(vvp_net_ptr_t port, ধ্রুৱ vvp_vector8_t&bit) override;
      শূন্য recv_real(vvp_net_ptr_t port, দ্ৱিগুণ bit,
                     vvp_context_t) override;

      শূন্য recv_vec4_pv(vvp_net_ptr_t ptr, ধ্রুৱ vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t ctx) override;
      শূন্য recv_vec8_pv(vvp_net_ptr_t ptr, ধ্রুৱ vvp_vector8_t&bit,
			unsigned base, unsigned vwid) override;

    গুপ্ত:
      আভাস শূন্য run_run() override;


      শূন্য run_run_vec4_(ধ্রুৱ সংরচনা vvp_fun_delay::event_*cur);
      শূন্য run_run_vec8_(ধ্রুৱ সংরচনা vvp_fun_delay::event_*cur);
      শূন্য run_run_real_(ধ্রুৱ সংরচনা vvp_fun_delay::event_*cur);

    গুপ্ত:
      vvp_net_t*net_;
      vvp_delay_t delay_;
      delay_type_t type_;
      bool initial_; // Indicates if the value is still the initial value.

      vvp_vector4_t cur_vec4_;
      vvp_vector8_t cur_vec8_;
      দ্ৱিগুণ cur_real_;
      vvp_time64_t round_, scale_; // Needed to scale variable time values.

      সংরচনা event_ *list_;
      শূন্য enqueue_(সংরচনা event_*cur)
      {
	    যদি (list_) {
		  cur->next = list_->next;
		  list_->next = cur;
		  list_ = cur;
	    } অন্যথা {
		  cur->next = cur;
		  list_ = cur;
	    }
      }
      সংরচনা event_* dequeue_(শূন্য)
      {
	    যদি (list_ == 0)
		  ঘূৰাই 0;
	    সংরচনা event_*cur = list_->next;
	    যদি (list_ == cur)
		  list_ = 0;
	    অন্যথা
		  list_->next = cur->next;
	    ঘূৰাই cur;
      }
      bool clean_pulse_events_(vvp_time64_t use_delay, ধ্রুৱ vvp_vector4_t&bit);
      bool clean_pulse_events_(vvp_time64_t use_delay, ধ্রুৱ vvp_vector8_t&bit);
      bool clean_pulse_events_(vvp_time64_t use_delay, দ্ৱিগুণ bit);
// Delete this when done!
      শূন্য clean_pulse_events_(vvp_time64_t use_delay);
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

class vvp_fun_modpath  : সার্ৱজনিক vvp_net_fun_t, গুপ্ত vvp_gen_event_s {

    সার্ৱজনিক:
      vvp_fun_modpath(vvp_net_t*net, unsigned width);
      ~vvp_fun_modpath() override;

      শূন্য add_modpath_src(vvp_fun_modpath_src*that, bool ifnone);

      শূন্য recv_vec4(vvp_net_ptr_t port, ধ্রুৱ vvp_vector4_t&bit,
                     vvp_context_t) override;

    গুপ্ত:
      আভাস শূন্য run_run() override;

    গুপ্ত:
      vvp_net_t*net_;

      vvp_vector4_t cur_vec4_;

      vvp_fun_modpath_src*src_list_;
      vvp_fun_modpath_src*ifnone_list_;

    গুপ্ত: // not implemented
      vvp_fun_modpath(ধ্রুৱ vvp_fun_modpath&);
      vvp_fun_modpath& operator= (ধ্রুৱ vvp_fun_modpath&);
};

class vvp_fun_modpath_src  : সার্ৱজনিক vvp_net_fun_t {

      friend class vvp_fun_modpath;

    সার্ৱজনিক:
      explicit vvp_fun_modpath_src(vvp_time64_t ধ্রুৱ del[12]);
    রক্ষিত:
      ~vvp_fun_modpath_src() override;

    সার্ৱজনিক:
      শূন্য recv_vec4(vvp_net_ptr_t port, ধ্রুৱ vvp_vector4_t&bit,
                     vvp_context_t) override;
      আভাস bool test_vec4(ধ্রুৱ vvp_vector4_t&bit);

      শূন্য get_delay12(vvp_time64_t out[12]) ধ্রুৱ;
      শূন্য put_delay12(ধ্রুৱ vvp_time64_t in[12]);

    গুপ্ত:
	// FIXME: Needs to be a 12-value array
      vvp_time64_t delay_[12];
	// Used by vvp_fun_modpath to keep a list of modpath_src objects.
      vvp_fun_modpath_src*next_;

      vvp_time64_t wake_time_;
      bool condition_flag_;

    গুপ্ত:
      vvp_fun_modpath_src(ধ্রুৱ vvp_fun_modpath_src&);
      vvp_fun_modpath_src& operator = (ধ্রুৱ vvp_fun_modpath_src&);
};

class vvp_fun_modpath_edge  : সার্ৱজনিক vvp_fun_modpath_src {

    সার্ৱজনিক:
      vvp_fun_modpath_edge(vvp_time64_t del[12], bool pos, bool neg);

      bool test_vec4(ধ্রুৱ vvp_vector4_t&bit) override;

    গুপ্ত:
      vvp_bit4_t old_মান;
      bool posedge_;
      bool negedge_;
};

/*
* The intermodpath is used to implement the SDF INTERCONNECT feature
* Upon a (INTERCONNECT ...) statement an intermodpath will be inserted
* between port1 and port2 and its delay can be annotated
*/
class vvp_fun_intermodpath  : সার্ৱজনিক vvp_net_fun_t, গুপ্ত vvp_gen_event_s {

    সার্ৱজনিক:
      vvp_fun_intermodpath(vvp_net_t*net, unsigned width);
      ~vvp_fun_intermodpath() override;

      শূন্য recv_vec4(vvp_net_ptr_t port, ধ্রুৱ vvp_vector4_t&bit,
                     vvp_context_t) override;

      শূন্য get_delay12(vvp_time64_t out[12]) ধ্রুৱ;
      শূন্য put_delay12(ধ্রুৱ vvp_time64_t in[12]);

    গুপ্ত:
      শূন্য run_run() override;

    গুপ্ত:
      vvp_net_t*net_;

      vvp_vector4_t cur_vec4_;

      vvp_time64_t delay_[12];

    গুপ্ত: // not implemented
      vvp_fun_intermodpath(ধ্রুৱ vvp_fun_intermodpath&);
      vvp_fun_intermodpath& operator= (ধ্রুৱ vvp_fun_intermodpath&);
};

#endif /* IVL_delay_H */
