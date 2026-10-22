#నిర్వచితం_కాకపోతే IVL_delay_H
#నిర్వచించు IVL_delay_H
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

గణన delay_అంచు_t {
      DELAY_అంచు_01 = 0, DELAY_అంచు_10, DELAY_అంచు_0z,
      DELAY_అంచు_z1,     DELAY_అంచు_1z, DELAY_అంచు_z0,
      DELAY_అంచు_0x,     DELAY_అంచు_x1, DELAY_అంచు_1x,
      DELAY_అంచు_x0,     DELAY_అంచు_xz, DELAY_అంచు_zx,
      DELAY_అంచు_లెక్క
};

/*
 * Instances of this object are functions that calculate the delay for
 * the transition from the source vvp_bit4_t value to the destination
 * value.
 */
class vvp_delay_t {

    బహిరంగ:
      vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall);
      vvp_delay_t(vvp_time64_t rise, vvp_time64_t fall, vvp_time64_t decay);
      ~vvp_delay_t();

      vvp_time64_t get_delay(vvp_bit4_t from, vvp_bit4_t గమ్యం);
      vvp_time64_t get_min_delay() స్థిరరాశి;

      శూన్య సమితి_rise(vvp_time64_t val);
      శూన్య సమితి_fall(vvp_time64_t val);
      శూన్య సమితి_decay(vvp_time64_t val);
      శూన్య సమితి_ignore_decay();

    అంతరంగ:
      vvp_time64_t rise_, fall_, decay_;
      vvp_time64_t min_delay_;
      bool ignore_decay_;

      శూన్య calculate_min_delay_();
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
class vvp_fun_delay  : బహిరంగ vvp_net_fun_t, అంతరంగ vvp_gen_event_s {

      గణన delay_type_t {UNKNOWN_DELAY, VEC4_DELAY, VEC8_DELAY, REAL_DELAY};
      నిర్మాణం event_ {
	    explicit event_(vvp_time64_t s) : sim_సమయం(s) {
		  ptr_real = 0.0;
		  తదుపరి_2 = ఖాళీ;
	    }
	    శూన్య (vvp_fun_delay::*నిరంతరం_నిరంతరం_ptr)(స్థిరరాశి నిర్మాణం vvp_fun_delay::event_*cur);
	    స్థిరరాశి vvp_time64_t sim_సమయం;
	    vvp_vector4_t ptr_vec4;
	    vvp_vector8_t ptr_vec8;
	    ద్విగుణ ptr_real;
	    నిర్మాణం event_*తదుపరి_2;
      };

    బహిరంగ:
      vvp_fun_delay(vvp_net_t*net, గుర్తులేని వెడల్పు, స్థిరరాశి vvp_delay_t&d);
      ~vvp_fun_delay() override;

      శూన్య recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t) override;
      శూన్య recv_vec8(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector8_t&బిట్టు) override;
      శూన్య recv_real(vvp_net_ptr_t పోర్టు, ద్విగుణ బిట్టు,
                     vvp_context_t) override;

      శూన్య recv_vec4_pv(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
			గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t ctx) override;
      శూన్య recv_vec8_pv(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector8_t&బిట్టు,
			గుర్తులేని పునాది, గుర్తులేని vwid) override;

    అంతరంగ:
      మాయా శూన్య నిరంతరం_నిరంతరం() override;


      శూన్య నిరంతరం_నిరంతరం_vec4(స్థిరరాశి నిర్మాణం vvp_fun_delay::event_*cur);
      శూన్య నిరంతరం_నిరంతరం_vec8(స్థిరరాశి నిర్మాణం vvp_fun_delay::event_*cur);
      శూన్య నిరంతరం_నిరంతరం_real(స్థిరరాశి నిర్మాణం vvp_fun_delay::event_*cur);

    అంతరంగ:
      vvp_net_t*net_;
      vvp_delay_t delay_;
      delay_type_t type_;
      bool initial_; // Indicates if the value is still the initial value.

      vvp_vector4_t cur_vec4_;
      vvp_vector8_t cur_vec8_;
      ద్విగుణ cur_real_;
      vvp_time64_t విడత, scale_; // Needed to scale variable time values.

      నిర్మాణం event_ *list_;
      శూన్య వరుసలో_చేర్చు(నిర్మాణం event_*cur)
      {
	    ఒకవేళ (list_) {
		  cur->తదుపరి_2 = list_->తదుపరి_2;
		  list_->తదుపరి_2 = cur;
		  list_ = cur;
	    } లేకపోతే {
		  cur->తదుపరి_2 = cur;
		  list_ = cur;
	    }
      }
      నిర్మాణం event_* వరుస_నుండి_తీయి(శూన్య)
      {
	    ఒకవేళ (list_ == 0)
		  తిరిగి 0;
	    నిర్మాణం event_*cur = list_->తదుపరి_2;
	    ఒకవేళ (list_ == cur)
		  list_ = 0;
	    లేకపోతే
		  list_->తదుపరి_2 = cur->తదుపరి_2;
	    తిరిగి cur;
      }
      bool clean_pulse_events_(vvp_time64_t use_delay, స్థిరరాశి vvp_vector4_t&బిట్టు);
      bool clean_pulse_events_(vvp_time64_t use_delay, స్థిరరాశి vvp_vector8_t&బిట్టు);
      bool clean_pulse_events_(vvp_time64_t use_delay, ద్విగుణ బిట్టు);
// Delete this when done!
      శూన్య clean_pulse_events_(vvp_time64_t use_delay);
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

class vvp_fun_modpath  : బహిరంగ vvp_net_fun_t, అంతరంగ vvp_gen_event_s {

    బహిరంగ:
      vvp_fun_modpath(vvp_net_t*net, గుర్తులేని వెడల్పు);
      ~vvp_fun_modpath() override;

      శూన్య కూడు_modpath_src(vvp_fun_modpath_src*that, bool ifnone);

      శూన్య recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t) override;

    అంతరంగ:
      మాయా శూన్య నిరంతరం_నిరంతరం() override;

    అంతరంగ:
      vvp_net_t*net_;

      vvp_vector4_t cur_vec4_;

      vvp_fun_modpath_src*src_list_;
      vvp_fun_modpath_src*ifnone_list_;

    అంతరంగ: // not implemented
      vvp_fun_modpath(స్థిరరాశి vvp_fun_modpath&);
      vvp_fun_modpath& క్రియాకారకం= (స్థిరరాశి vvp_fun_modpath&);
};

class vvp_fun_modpath_src  : బహిరంగ vvp_net_fun_t {

      మిత్ర class vvp_fun_modpath;

    బహిరంగ:
      explicit vvp_fun_modpath_src(vvp_time64_t స్థిరరాశి del[12]);
    రక్షిత:
      ~vvp_fun_modpath_src() override;

    బహిరంగ:
      శూన్య recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t) override;
      మాయా bool test_vec4(స్థిరరాశి vvp_vector4_t&బిట్టు);

      శూన్య get_delay12(vvp_time64_t బహిర్గతం[12]) స్థిరరాశి;
      శూన్య put_delay12(స్థిరరాశి vvp_time64_t లోపల[12]);

    అంతరంగ:
	// FIXME: Needs to be a 12-value array
      vvp_time64_t delay_[12];
	// Used by vvp_fun_modpath to keep a list of modpath_src objects.
      vvp_fun_modpath_src*తదుపరి;

      vvp_time64_t wake_సమయం;
      bool condition_flag_;

    అంతరంగ:
      vvp_fun_modpath_src(స్థిరరాశి vvp_fun_modpath_src&);
      vvp_fun_modpath_src& క్రియాకారకం = (స్థిరరాశి vvp_fun_modpath_src&);
};

class vvp_fun_modpath_అంచు  : బహిరంగ vvp_fun_modpath_src {

    బహిరంగ:
      vvp_fun_modpath_అంచు(vvp_time64_t del[12], bool pos, bool neg);

      bool test_vec4(స్థిరరాశి vvp_vector4_t&బిట్టు) override;

    అంతరంగ:
      vvp_bit4_t old_విలువ;
      bool posedge_;
      bool negedge_;
};

/*
* The intermodpath is used to implement the SDF INTERCONNECT feature
* Upon a (INTERCONNECT ...) statement an intermodpath will be inserted
* between port1 and port2 and its delay can be annotated
*/
class vvp_fun_intermodpath  : బహిరంగ vvp_net_fun_t, అంతరంగ vvp_gen_event_s {

    బహిరంగ:
      vvp_fun_intermodpath(vvp_net_t*net, గుర్తులేని వెడల్పు);
      ~vvp_fun_intermodpath() override;

      శూన్య recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t) override;

      శూన్య get_delay12(vvp_time64_t బహిర్గతం[12]) స్థిరరాశి;
      శూన్య put_delay12(స్థిరరాశి vvp_time64_t లోపల[12]);

    అంతరంగ:
      శూన్య నిరంతరం_నిరంతరం() override;

    అంతరంగ:
      vvp_net_t*net_;

      vvp_vector4_t cur_vec4_;

      vvp_time64_t delay_[12];

    అంతరంగ: // not implemented
      vvp_fun_intermodpath(స్థిరరాశి vvp_fun_intermodpath&);
      vvp_fun_intermodpath& క్రియాకారకం= (స్థిరరాశి vvp_fun_intermodpath&);
};

#అయితే_ముగింపు /* IVL_delay_H */
