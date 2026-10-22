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

എണ്ണൽ delay_edge_t {
      DELAY_EDGE_01 = 0, DELAY_EDGE_10, DELAY_EDGE_0z,
      DELAY_EDGE_z1,     DELAY_EDGE_1z, DELAY_EDGE_z0,
      DELAY_EDGE_0x,     DELAY_EDGE_x1, DELAY_EDGE_1x,
      DELAY_EDGE_x0,     DELAY_EDGE_xz, DELAY_EDGE_zx,
      DELAY_EDGE_എണ്ണം
};

/*
 * Instances of this object are functions that calculate the delay for
 * the transition from the source vvp_bit4_t value to the destination
 * value.
 */
class vvp_delay_t {

    പൊതു:
      vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall);
      vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall, vvp_time64_t decay);
      ~vvp_delay_t();

      vvp_time64_t get_delay(vvp_bit4_t from, vvp_bit4_t to);
      vvp_time64_t get_min_delay() സ്ഥിരം;

      ശൂന്യം set_rise(vvp_time64_t val);
      ശൂന്യം set_fall(vvp_time64_t val);
      ശൂന്യം set_decay(vvp_time64_t val);
      ശൂന്യം set_ignore_decay();

    സ്വകാര്യ:
      vvp_time64_t rise_, fall_, decay_;
      vvp_time64_t min_delay_;
      bool ignore_decay_;

      ശൂന്യം calculate_min_delay_();
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
class vvp_fun_delay  : പൊതു vvp_net_fun_t, സ്വകാര്യ vvp_gen_event_s {

      എണ്ണൽ delay_type_t {UNKNOWN_DELAY, VEC4_DELAY, VEC8_DELAY, REAL_DELAY};
      ഘടന event_ {
	    explicit event_(vvp_time64_t s) : sim_time(s) {
		  ptr_real = 0.0;
		  next = ശൂന്യ;
	    }
	    ശൂന്യം (vvp_fun_delay::*run_run_ptr)(സ്ഥിരം ഘടന vvp_fun_delay::event_*cur);
	    സ്ഥിരം vvp_time64_t sim_time;
	    vvp_vector4_t ptr_vec4;
	    vvp_vector8_t ptr_vec8;
	    ഇരട്ട ptr_real;
	    ഘടന event_*next;
      };

    പൊതു:
      vvp_fun_delay(vvp_net_t*net, unsigned width, സ്ഥിരം vvp_delay_t&d);
      ~vvp_fun_delay() override;

      ശൂന്യം recv_vec4(vvp_net_ptr_t port, സ്ഥിരം vvp_vector4_t&bit,
                     vvp_context_t) override;
      ശൂന്യം recv_vec8(vvp_net_ptr_t port, സ്ഥിരം vvp_vector8_t&bit) override;
      ശൂന്യം recv_real(vvp_net_ptr_t port, ഇരട്ട bit,
                     vvp_context_t) override;

      ശൂന്യം recv_vec4_pv(vvp_net_ptr_t ptr, സ്ഥിരം vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t ctx) override;
      ശൂന്യം recv_vec8_pv(vvp_net_ptr_t ptr, സ്ഥിരം vvp_vector8_t&bit,
			unsigned base, unsigned vwid) override;

    സ്വകാര്യ:
      മായാ ശൂന്യം run_run() override;


      ശൂന്യം run_run_vec4_(സ്ഥിരം ഘടന vvp_fun_delay::event_*cur);
      ശൂന്യം run_run_vec8_(സ്ഥിരം ഘടന vvp_fun_delay::event_*cur);
      ശൂന്യം run_run_real_(സ്ഥിരം ഘടന vvp_fun_delay::event_*cur);

    സ്വകാര്യ:
      vvp_net_t*net_;
      vvp_delay_t delay_;
      delay_type_t type_;
      bool initial_; // Indicates if the value is still the initial value.

      vvp_vector4_t cur_vec4_;
      vvp_vector8_t cur_vec8_;
      ഇരട്ട cur_real_;
      vvp_time64_t round_, scale_; // Needed to scale variable time values.

      ഘടന event_ *list_;
      ശൂന്യം enqueue_(ഘടന event_*cur)
      {
	    എങ്കിൽ (list_) {
		  cur->next = list_->next;
		  list_->next = cur;
		  list_ = cur;
	    } അല്ലെങ്കിൽ {
		  cur->next = cur;
		  list_ = cur;
	    }
      }
      ഘടന event_* dequeue_(ശൂന്യം)
      {
	    എങ്കിൽ (list_ == 0)
		  തിരികെ 0;
	    ഘടന event_*cur = list_->next;
	    എങ്കിൽ (list_ == cur)
		  list_ = 0;
	    അല്ലെങ്കിൽ
		  list_->next = cur->next;
	    തിരികെ cur;
      }
      bool clean_pulse_events_(vvp_time64_t use_delay, സ്ഥിരം vvp_vector4_t&bit);
      bool clean_pulse_events_(vvp_time64_t use_delay, സ്ഥിരം vvp_vector8_t&bit);
      bool clean_pulse_events_(vvp_time64_t use_delay, ഇരട്ട bit);
// Delete this when done!
      ശൂന്യം clean_pulse_events_(vvp_time64_t use_delay);
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

class vvp_fun_modpath  : പൊതു vvp_net_fun_t, സ്വകാര്യ vvp_gen_event_s {

    പൊതു:
      vvp_fun_modpath(vvp_net_t*net, unsigned width);
      ~vvp_fun_modpath() override;

      ശൂന്യം add_modpath_src(vvp_fun_modpath_src*that, bool ifnone);

      ശൂന്യം recv_vec4(vvp_net_ptr_t port, സ്ഥിരം vvp_vector4_t&bit,
                     vvp_context_t) override;

    സ്വകാര്യ:
      മായാ ശൂന്യം run_run() override;

    സ്വകാര്യ:
      vvp_net_t*net_;

      vvp_vector4_t cur_vec4_;

      vvp_fun_modpath_src*src_list_;
      vvp_fun_modpath_src*ifnone_list_;

    സ്വകാര്യ: // not implemented
      vvp_fun_modpath(സ്ഥിരം vvp_fun_modpath&);
      vvp_fun_modpath& operator= (സ്ഥിരം vvp_fun_modpath&);
};

class vvp_fun_modpath_src  : പൊതു vvp_net_fun_t {

      friend class vvp_fun_modpath;

    പൊതു:
      explicit vvp_fun_modpath_src(vvp_time64_t സ്ഥിരം del[12]);
    സംരക്ഷിത:
      ~vvp_fun_modpath_src() override;

    പൊതു:
      ശൂന്യം recv_vec4(vvp_net_ptr_t port, സ്ഥിരം vvp_vector4_t&bit,
                     vvp_context_t) override;
      മായാ bool test_vec4(സ്ഥിരം vvp_vector4_t&bit);

      ശൂന്യം get_delay12(vvp_time64_t out[12]) സ്ഥിരം;
      ശൂന്യം put_delay12(സ്ഥിരം vvp_time64_t in[12]);

    സ്വകാര്യ:
	// FIXME: Needs to be a 12-value array
      vvp_time64_t delay_[12];
	// Used by vvp_fun_modpath to keep a list of modpath_src objects.
      vvp_fun_modpath_src*next_;

      vvp_time64_t wake_time_;
      bool condition_flag_;

    സ്വകാര്യ:
      vvp_fun_modpath_src(സ്ഥിരം vvp_fun_modpath_src&);
      vvp_fun_modpath_src& operator = (സ്ഥിരം vvp_fun_modpath_src&);
};

class vvp_fun_modpath_edge  : പൊതു vvp_fun_modpath_src {

    പൊതു:
      vvp_fun_modpath_edge(vvp_time64_t del[12], bool pos, bool neg);

      bool test_vec4(സ്ഥിരം vvp_vector4_t&bit) override;

    സ്വകാര്യ:
      vvp_bit4_t old_മൂല്യം;
      bool posedge_;
      bool negedge_;
};

/*
* The intermodpath is used to implement the SDF INTERCONNECT feature
* Upon a (INTERCONNECT ...) statement an intermodpath will be inserted
* between port1 and port2 and its delay can be annotated
*/
class vvp_fun_intermodpath  : പൊതു vvp_net_fun_t, സ്വകാര്യ vvp_gen_event_s {

    പൊതു:
      vvp_fun_intermodpath(vvp_net_t*net, unsigned width);
      ~vvp_fun_intermodpath() override;

      ശൂന്യം recv_vec4(vvp_net_ptr_t port, സ്ഥിരം vvp_vector4_t&bit,
                     vvp_context_t) override;

      ശൂന്യം get_delay12(vvp_time64_t out[12]) സ്ഥിരം;
      ശൂന്യം put_delay12(സ്ഥിരം vvp_time64_t in[12]);

    സ്വകാര്യ:
      ശൂന്യം run_run() override;

    സ്വകാര്യ:
      vvp_net_t*net_;

      vvp_vector4_t cur_vec4_;

      vvp_time64_t delay_[12];

    സ്വകാര്യ: // not implemented
      vvp_fun_intermodpath(സ്ഥിരം vvp_fun_intermodpath&);
      vvp_fun_intermodpath& operator= (സ്ഥിരം vvp_fun_intermodpath&);
};

#endif /* IVL_delay_H */
