/*
 * Copyright (c) 2001-2026 Stephen Williams (steve@icarus.com)
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

# include  "config.h"
# include  "schedule.h"
# include  "vthread.h"
# include  "vpi_priv.h"
# include  "vvp_net_sig.h"
# include  "slab.h"
# include  "compile.h"
# include  <new>
# include  <typeinfo>
# include  <csignal>
# include  <cstdlib>
# include  <cassert>
# include  <iostream>
#నిర్వచితమైతే CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
# include  "ivl_alloc.h"
#అయితే_ముగింపు

ఉపయోగించి నామాకాశ std;

గుర్తులేని దీర్ఘ లెక్క_కేటాయించు_events = 0;
గుర్తులేని దీర్ఘ లెక్క_gen_events = 0;
గుర్తులేని దీర్ఘ లెక్క_thread_events = 0;
  // Count the time events (A time cell created)
గుర్తులేని దీర్ఘ లెక్క_సమయం_events = 0;



/*
 * The event_s and event_time_s structures implement the Verilog
 * stratified event queue.
 *
 * The event_time_s objects are one per time step. Each time step in
 * turn contains a list of event_s objects that are the actual events.
 *
 * The event_s objects are base classes for the more specific sort of
 * event.
 */
నిర్మాణం event_s {
      నిర్మాణం event_s*తదుపరి_2;
      మాయా ~event_s() { }
      మాయా శూన్య నిరంతరం_నిరంతరం(శూన్య) =0;

	// Write something about the event to stderr
      మాయా శూన్య single_అడుగు_display(శూన్య);

	// Fallback new/delete
      స్థిర శూన్య*క్రియాకారకం కొత్త (size_t size) { తిరిగి ::కొత్త అక్షరం[size]; }
      స్థిర శూన్య క్రియాకారకం తొలగించు(శూన్య*ptr)  { ::తొలగించు[]( static_cast<అక్షరం*>(ptr) ); }
};

శూన్య event_s::single_అడుగు_display(శూన్య)
{
      std::cerr << "event_s: Step into event " << typeid(*ఇది).name() << std::పంక్తి_ముగింపు;
}

నిర్మాణం event_సమయం_s {
      event_సమయం_s() {
	    లెక్క_సమయం_events += 1;
	    delay = 0;
	    ప్రారంభం = 0;
	    చురుకైన = 0;
	    inactive = 0;
	    nbassign = 0;
	    rwsync = 0;
	    rosync = 0;
	    del_thr = 0;
	    తదుపరి_2 = ఖాళీ;
      }
      vvp_time64_t delay;

      నిర్మాణం event_s*ప్రారంభం;
      నిర్మాణం event_s*చురుకైన;
      నిర్మాణం event_s*inactive;
      నిర్మాణం event_s*nbassign;
      నిర్మాణం event_s*rwsync;
      నిర్మాణం event_s*rosync;
      నిర్మాణం event_s*del_thr;

      నిర్మాణం event_సమయం_s*తదుపరి_2;

      స్థిర శూన్య* క్రియాకారకం కొత్త (size_t);
      స్థిర శూన్య క్రియాకారకం తొలగించు(శూన్య*obj, size_t s);
};

vvp_gen_event_s::~vvp_gen_event_s()
{
}

శూన్య vvp_gen_event_s::single_అడుగు_display(శూన్య)
{
      cerr << "vvp_gen_event_s: Step into event " << typeid(*ఇది).name() << పంక్తి_ముగింపు;
}

/*
 * Derived event types
 */
నిర్మాణం vthread_event_s : బహిరంగ event_s {
      vthread_t thr;
      శూన్య నిరంతరం_నిరంతరం(శూన్య) override;
      శూన్య single_అడుగు_display(శూన్య) override;

      స్థిర శూన్య* క్రియాకారకం కొత్త(size_t);
      స్థిర శూన్య క్రియాకారకం తొలగించు(శూన్య*);
};

శూన్య vthread_event_s::నిరంతరం_నిరంతరం(శూన్య)
{
      లెక్క_thread_events += 1;
      vthread_నిరంతరం(thr);
}

శూన్య vthread_event_s::single_అడుగు_display(శూన్య)
{
      __vpiScope*scope = vthread_scope(thr);
      cerr << "vthread_event: Resume thread"
	   << " scope=" << scope->vpi_get_str(vpi_నిండిన_Name)
	   << పంక్తి_ముగింపు;
}

స్థిర స్థిరరాశి size_t VTHR_CHUNK_లెక్క = 8192 / పరిమాణం(నిర్మాణం vthread_event_s);
స్థిర slab_t<పరిమాణం(vthread_event_s),VTHR_CHUNK_లెక్క> vthread_event_కుప్ప;

అంతఃవరుస శూన్య* vthread_event_s::క్రియాకారకం కొత్త(size_t size)
{
      ప్రతిజ్ఞా(size == పరిమాణం(vthread_event_s));
      తిరిగి vthread_event_కుప్ప.alloc_slab();
}

శూన్య vthread_event_s::క్రియాకారకం తొలగించు(శూన్య*dptr)
{
      vthread_event_కుప్ప.free_slab(dptr);
}

నిర్మాణం del_thr_event_s : బహిరంగ event_s {
      vthread_t thr;
      శూన్య నిరంతరం_నిరంతరం(శూన్య) override;
      శూన్య single_అడుగు_display(శూన్య) override;
};

శూన్య del_thr_event_s::నిరంతరం_నిరంతరం(శూన్య)
{
      vthread_delete(thr);
}

శూన్య del_thr_event_s::single_అడుగు_display(శూన్య)
{
      __vpiScope*scope = vthread_scope(thr);
      cerr << "del_thr_event: Reap completed thread"
	   << " scope=" << scope->vpi_get_str(vpi_నిండిన_Name) << పంక్తి_ముగింపు;
}

నిర్మాణం కేటాయించు_vector4_event_s  : బహిరంగ event_s {
	/* The default constructor. */
      స్పష్టమైన కేటాయించు_vector4_event_s(స్థిరరాశి vvp_vector4_t&that) : val(that) {
	    పునాది = 0;
	    vwid = 0;
	    తదుపరి_2 = ఖాళీ;
      }

	/* Where to do the assign. */
      vvp_net_ptr_t ptr;
	/* Value to assign. */
      vvp_vector4_t val;
	/* Offset of the part into the destination. */
      గుర్తులేని పునాది;
	/* Width of the destination vector. */
      గుర్తులేని vwid;
      శూన్య నిరంతరం_నిరంతరం(శూన్య) override;
      శూన్య single_అడుగు_display(శూన్య) override;

      స్థిర శూన్య* క్రియాకారకం కొత్త(size_t);
      స్థిర శూన్య క్రియాకారకం తొలగించు(శూన్య*);
};

శూన్య కేటాయించు_vector4_event_s::నిరంతరం_నిరంతరం(శూన్య)
{
      లెక్క_కేటాయించు_events += 1;
      ఒకవేళ (vwid > 0)
	    vvp_send_vec4_pv(ptr, val, పునాది, vwid, 0);
      లేకపోతే
	    vvp_send_vec4(ptr, val, 0);
}

శూన్య కేటాయించు_vector4_event_s::single_అడుగు_display(శూన్య)
{
      cerr << "assign_vector4_event: Propagate val=" << val
	   << ", vwid=" << vwid << ", base=" << పునాది << పంక్తి_ముగింపు;
}

స్థిర స్థిరరాశి size_t ASSIGN4_CHUNK_లెక్క = 524288 / పరిమాణం(నిర్మాణం కేటాయించు_vector4_event_s);
స్థిర slab_t<పరిమాణం(కేటాయించు_vector4_event_s),ASSIGN4_CHUNK_లెక్క> assign4_కుప్ప;

అంతఃవరుస శూన్య* కేటాయించు_vector4_event_s::క్రియాకారకం కొత్త(size_t size)
{
      ప్రతిజ్ఞా(size == పరిమాణం(కేటాయించు_vector4_event_s));
      తిరిగి assign4_కుప్ప.alloc_slab();
}

శూన్య కేటాయించు_vector4_event_s::క్రియాకారకం తొలగించు(శూన్య*dptr)
{
      assign4_కుప్ప.free_slab(dptr);
}

గుర్తులేని దీర్ఘ లెక్క_assign4_pool(శూన్య) { తిరిగి assign4_కుప్ప.pool; }

నిర్మాణం కేటాయించు_vector8_event_s  : బహిరంగ event_s {
      vvp_net_ptr_t ptr;
      vvp_vector8_t val;
      శూన్య నిరంతరం_నిరంతరం(శూన్య) override;
      శూన్య single_అడుగు_display(శూన్య) override;

      స్థిర శూన్య* క్రియాకారకం కొత్త(size_t);
      స్థిర శూన్య క్రియాకారకం తొలగించు(శూన్య*);
};

శూన్య కేటాయించు_vector8_event_s::నిరంతరం_నిరంతరం(శూన్య)
{
      లెక్క_కేటాయించు_events += 1;
      vvp_send_vec8(ptr, val);
}

శూన్య కేటాయించు_vector8_event_s::single_అడుగు_display(శూన్య)
{
      cerr << "assign_vector8_event: Propagate val=" << val << పంక్తి_ముగింపు;
}

స్థిర స్థిరరాశి size_t ASSIGN8_CHUNK_లెక్క = 8192 / పరిమాణం(నిర్మాణం కేటాయించు_vector8_event_s);
స్థిర slab_t<పరిమాణం(కేటాయించు_vector8_event_s),ASSIGN8_CHUNK_లెక్క> assign8_కుప్ప;

అంతఃవరుస శూన్య* కేటాయించు_vector8_event_s::క్రియాకారకం కొత్త(size_t size)
{
      ప్రతిజ్ఞా(size == పరిమాణం(కేటాయించు_vector8_event_s));
      తిరిగి assign8_కుప్ప.alloc_slab();
}

శూన్య కేటాయించు_vector8_event_s::క్రియాకారకం తొలగించు(శూన్య*dptr)
{
      assign8_కుప్ప.free_slab(dptr);
}

గుర్తులేని దీర్ఘ లెక్క_assign8_pool() { తిరిగి assign8_కుప్ప.pool; }

నిర్మాణం కేటాయించు_real_event_s  : బహిరంగ event_s {
      vvp_net_ptr_t ptr;
      ద్విగుణ val;
      శూన్య నిరంతరం_నిరంతరం(శూన్య) override;
      శూన్య single_అడుగు_display(శూన్య) override;

      స్థిర శూన్య* క్రియాకారకం కొత్త(size_t);
      స్థిర శూన్య క్రియాకారకం తొలగించు(శూన్య*);
};

శూన్య కేటాయించు_real_event_s::నిరంతరం_నిరంతరం(శూన్య)
{
      లెక్క_కేటాయించు_events += 1;
      vvp_send_real(ptr, val, 0);
}

శూన్య కేటాయించు_real_event_s::single_అడుగు_display(శూన్య)
{
      cerr << "assign_real_event: Propagate val=" << val << పంక్తి_ముగింపు;
}

స్థిర స్థిరరాశి size_t ASSIGNR_CHUNK_లెక్క = 8192 / పరిమాణం(నిర్మాణం కేటాయించు_real_event_s);
స్థిర slab_t<పరిమాణం(కేటాయించు_real_event_s),ASSIGNR_CHUNK_లెక్క> assignr_కుప్ప;

అంతఃవరుస శూన్య* కేటాయించు_real_event_s::క్రియాకారకం కొత్త (size_t size)
{
      ప్రతిజ్ఞా(size == పరిమాణం(కేటాయించు_real_event_s));
      తిరిగి assignr_కుప్ప.alloc_slab();
}

శూన్య కేటాయించు_real_event_s::క్రియాకారకం తొలగించు(శూన్య*dptr)
{
      assignr_కుప్ప.free_slab(dptr);
}

గుర్తులేని దీర్ఘ లెక్క_కేటాయించు_real_pool(శూన్య) { తిరిగి assignr_కుప్ప.pool; }

నిర్మాణం కేటాయించు_array_మాట_s  : బహిరంగ event_s {
      vvp_array_t mem;
      గుర్తులేని adr;
      vvp_vector4_t val;
      గుర్తులేని off;
      శూన్య నిరంతరం_నిరంతరం(శూన్య) override;

      స్థిర శూన్య* క్రియాకారకం కొత్త(size_t);
      స్థిర శూన్య క్రియాకారకం తొలగించు(శూన్య*);
};

శూన్య కేటాయించు_array_మాట_s::నిరంతరం_నిరంతరం(శూన్య)
{
      లెక్క_కేటాయించు_events += 1;
      mem->సమితి_మాట(adr, off, val);
}

స్థిర స్థిరరాశి size_t ARRAY_W_CHUNK_లెక్క = 8192 / పరిమాణం(నిర్మాణం కేటాయించు_array_మాట_s);
స్థిర slab_t<పరిమాణం(కేటాయించు_array_మాట_s),ARRAY_W_CHUNK_లెక్క> array_w_కుప్ప;

అంతఃవరుస శూన్య* కేటాయించు_array_మాట_s::క్రియాకారకం కొత్త (size_t size)
{
      ప్రతిజ్ఞా(size == పరిమాణం(కేటాయించు_array_మాట_s));
      తిరిగి array_w_కుప్ప.alloc_slab();
}

శూన్య కేటాయించు_array_మాట_s::క్రియాకారకం తొలగించు(శూన్య*ptr)
{
      array_w_కుప్ప.free_slab(ptr);
}

గుర్తులేని దీర్ఘ లెక్క_కేటాయించు_aword_pool(శూన్య) { తిరిగి array_w_కుప్ప.pool; }

నిర్మాణం force_vector4_event_s  : బహిరంగ event_s {
	/* The default constructor. */
      స్పష్టమైన force_vector4_event_s(స్థిరరాశి vvp_vector4_t&that): val(that) {
	    net = ఖాళీ;
	    పునాది = 0;
	    vwid = 0;
	    తదుపరి_2 = ఖాళీ;
      }
	/* Where to do the force. */
      vvp_net_t*net;
	/* Value to force. */
      vvp_vector4_t val;
	/* Offset of the part into the destination. */
      గుర్తులేని పునాది;
	/* Width of the destination vector. */
      గుర్తులేని vwid;

      శూన్య నిరంతరం_నిరంతరం(శూన్య) override;
      శూన్య single_అడుగు_display(శూన్య) override;

      స్థిర శూన్య* క్రియాకారకం కొత్త(size_t);
      స్థిర శూన్య క్రియాకారకం తొలగించు(శూన్య*);
};

శూన్య force_vector4_event_s::నిరంతరం_నిరంతరం(శూన్య)
{
      లెక్క_కేటాయించు_events += 1;

      గుర్తులేని wid = val.size();
      ఒకవేళ ((పునాది + wid) > vwid)
	    wid = vwid - పునాది;

	// Make a mask of which bits are to be forced, 0 for unforced
	// bits and 1 for forced bits.
      vvp_vector2_t ముసుగు (vvp_vector2_t::FILL0, vwid);
      కొరకు (గుర్తులేని idx = 0 ; idx < wid ; idx += 1)
	    ముసుగు.సమితి_బిట్టు(పునాది+idx, 1);

      vvp_vector4_t tmp (vwid, BIT4_Z);

	// vvp_net_t::force_vec4 propagates all the bits of the
	// forced vector value, regardless of the mask. This
	// ensures the unforced bits retain their current value.
      vvp_signal_విలువ*sig = dynamic_cast<vvp_signal_విలువ*>(net->fil);
      ప్రతిజ్ఞా(sig);
      sig->vec4_విలువ(tmp);

      tmp.సమితి_vec(పునాది, val);
      net->force_vec4(tmp, ముసుగు);
}

శూన్య force_vector4_event_s::single_అడుగు_display(శూన్య)
{
      cerr << "force_vector4_event: Force val=" << val
	   << ", vwid=" << vwid << ", base=" << పునాది << పంక్తి_ముగింపు;
}

స్థిర స్థిరరాశి size_t FORCE4_CHUNK_లెక్క = 8192 / పరిమాణం(నిర్మాణం force_vector4_event_s);
స్థిర slab_t<పరిమాణం(force_vector4_event_s),FORCE4_CHUNK_లెక్క> force4_కుప్ప;

అంతఃవరుస శూన్య* force_vector4_event_s::క్రియాకారకం కొత్త(size_t size)
{
      ప్రతిజ్ఞా(size == పరిమాణం(force_vector4_event_s));
      తిరిగి force4_కుప్ప.alloc_slab();
}

శూన్య force_vector4_event_s::క్రియాకారకం తొలగించు(శూన్య*dptr)
{
      force4_కుప్ప.free_slab(dptr);
}

గుర్తులేని దీర్ఘ లెక్క_force4_pool(శూన్య) { తిరిగి force4_కుప్ప.pool; }

/*
 * This class supports the propagation of vec4 outputs from a
 * vvp_net_t object.
 */
నిర్మాణం propagate_vector4_event_s : బహిరంగ event_s {
	/* The default constructor. */
      స్పష్టమైన propagate_vector4_event_s(స్థిరరాశి vvp_vector4_t&that) : val(that) {
	    net = ఖాళీ;
	    తదుపరి_2 = ఖాళీ;
      }
	/* A constructor that makes the val directly. */
      propagate_vector4_event_s(స్థిరరాశి vvp_vector4_t&that, గుర్తులేని adr, గుర్తులేని wid)
      : val(that,adr,wid) {
	    net = ఖాళీ;
	    తదుపరి_2 = ఖాళీ;
      }

	/* Propagate the output of this net. */
      vvp_net_t*net;
	/* value to propagate */
      vvp_vector4_t val;
	/* Action */
      శూన్య నిరంతరం_నిరంతరం(శూన్య) override;
      శూన్య single_అడుగు_display(శూన్య) override;
};

శూన్య propagate_vector4_event_s::నిరంతరం_నిరంతరం(శూన్య)
{
      net->send_vec4(val, 0);
}

శూన్య propagate_vector4_event_s::single_అడుగు_display(శూన్య)
{
      cerr << "propagate_vector4_event: Propagate val=" << val << పంక్తి_ముగింపు;
}

/*
 * This class supports the propagation of real outputs from a
 * vvp_net_t object.
 */
నిర్మాణం propagate_real_event_s : బహిరంగ event_s {
	/* Propagate the output of this net. */
      vvp_net_t*net;
	/* value to propagate */
      ద్విగుణ val;
	/* Action */
      శూన్య నిరంతరం_నిరంతరం(శూన్య) override;
      శూన్య single_అడుగు_display(శూన్య) override;
};

శూన్య propagate_real_event_s::నిరంతరం_నిరంతరం(శూన్య)
{
      net->send_real(val, 0);
}

శూన్య propagate_real_event_s::single_అడుగు_display(శూన్య)
{
      cerr << "propagate_real_event: Propagate val=" << val << పంక్తి_ముగింపు;
}

నిర్మాణం కేటాయించు_array_r_మాట_s  : బహిరంగ event_s {
      vvp_array_t mem;
      గుర్తులేని adr;
      ద్విగుణ val;
      శూన్య నిరంతరం_నిరంతరం(శూన్య) override;

      స్థిర శూన్య* క్రియాకారకం కొత్త(size_t);
      స్థిర శూన్య క్రియాకారకం తొలగించు(శూన్య*);
};

శూన్య కేటాయించు_array_r_మాట_s::నిరంతరం_నిరంతరం(శూన్య)
{
      లెక్క_కేటాయించు_events += 1;
      mem->సమితి_మాట(adr, val);
}
స్థిర స్థిరరాశి size_t ARRAY_R_W_CHUNK_లెక్క = 8192 / పరిమాణం(నిర్మాణం కేటాయించు_array_r_మాట_s);
స్థిర slab_t<పరిమాణం(కేటాయించు_array_r_మాట_s),ARRAY_R_W_CHUNK_లెక్క> array_r_w_కుప్ప;

అంతఃవరుస శూన్య* కేటాయించు_array_r_మాట_s::క్రియాకారకం కొత్త(size_t size)
{
      ప్రతిజ్ఞా(size == పరిమాణం(కేటాయించు_array_r_మాట_s));
      తిరిగి array_r_w_కుప్ప.alloc_slab();
}

శూన్య కేటాయించు_array_r_మాట_s::క్రియాకారకం తొలగించు(శూన్య*ptr)
{
      array_r_w_కుప్ప.free_slab(ptr);
}

గుర్తులేని దీర్ఘ లెక్క_కేటాయించు_arword_pool(శూన్య) { తిరిగి array_r_w_కుప్ప.pool; }

నిర్మాణం generic_event_s : బహిరంగ event_s {
      vvp_gen_event_t obj;
      తార్కిక delete_obj_when_పూర్తి;
      శూన్య నిరంతరం_నిరంతరం(శూన్య) override;
      శూన్య single_అడుగు_display(శూన్య) override;

      స్థిర శూన్య* క్రియాకారకం కొత్త(size_t);
      స్థిర శూన్య క్రియాకారకం తొలగించు(శూన్య*);
};

శూన్య generic_event_s::నిరంతరం_నిరంతరం(శూన్య)
{
      లెక్క_gen_events += 1;
      ఒకవేళ (obj) {
	    obj->నిరంతరం_నిరంతరం();
	    ఒకవేళ (delete_obj_when_పూర్తి)
		  తొలగించు obj;
      }
}

శూన్య generic_event_s::single_అడుగు_display(శూన్య)
{
      obj->single_అడుగు_display();
}

స్థిర స్థిరరాశి size_t GENERIC_CHUNK_లెక్క = 131072 / పరిమాణం(నిర్మాణం generic_event_s);
స్థిర slab_t<పరిమాణం(generic_event_s),GENERIC_CHUNK_లెక్క> generic_event_కుప్ప;

అంతఃవరుస శూన్య* generic_event_s::క్రియాకారకం కొత్త(size_t size)
{
      ప్రతిజ్ఞా(size == పరిమాణం(generic_event_s));
      తిరిగి generic_event_కుప్ప.alloc_slab();
}

శూన్య generic_event_s::క్రియాకారకం తొలగించు(శూన్య*ptr)
{
      generic_event_కుప్ప.free_slab(ptr);
}

గుర్తులేని దీర్ఘ లెక్క_gen_pool(శూన్య) { తిరిగి generic_event_కుప్ప.pool; }

/*
** These event_time_s will be required a lot, at high frequency.
** Once allocated, we never free them, but stash them away for next time.
*/


స్థిర స్థిరరాశి size_t సమయం_CHUNK_లెక్క = 8192 / పరిమాణం(నిర్మాణం event_సమయం_s);
స్థిర slab_t<పరిమాణం(event_సమయం_s),సమయం_CHUNK_లెక్క> event_సమయం_కుప్ప;

అంతఃవరుస శూన్య* event_సమయం_s::క్రియాకారకం కొత్త (size_t size)
{
      ప్రతిజ్ఞా(size == పరిమాణం(నిర్మాణం event_సమయం_s));
      శూన్య*ptr = event_సమయం_కుప్ప.alloc_slab();
      తిరిగి ptr;
}

అంతఃవరుస శూన్య event_సమయం_s::క్రియాకారకం తొలగించు(శూన్య*ptr, size_t)
{
      event_సమయం_కుప్ప.free_slab(ptr);
}

గుర్తులేని దీర్ఘ లెక్క_సమయం_pool(శూన్య) { తిరిగి event_సమయం_కుప్ప.pool; }

/*
 * This is the head of the list of pending events. This includes all
 * the events that have not been executed yet, and reaches into the
 * future.
 */
స్థిర నిర్మాణం event_సమయం_s* sched_list = 0;

/*
 * This is a list of initialization events. The setup puts
 * initializations in this list so that they happen before the
 * simulation as a whole starts. This prevents time-0 triggers of
 * certain events.
 */
స్థిర నిర్మాణం event_s* కాలపట్టిక_init_list = 0;

/*
 * This is the head of the list of final events.
 */
స్థిర నిర్మాణం event_s* కాలపట్టిక_final_list = 0;

/*
 * This flag is true until a VPI task or function finishes the
 * simulation.
 */
స్థిర తార్కిక కాలపట్టిక_runnable = నిజం;
స్థిర తార్కిక కాలపట్టిక_stopped_flag  = అబద్ధం;
స్థిర తార్కిక కాలపట్టిక_single_అడుగు_flag = అబద్ధం;
స్థిర తార్కిక no_సంకేతాలు_flag = అబద్ధం;

శూన్య కాలపట్టిక_ముగింపు(పూర్ణాంకం)
{
      కాలపట్టిక_runnable = అబద్ధం;
}

శూన్య కాలపట్టిక_stop(పూర్ణాంకం)
{
      కాలపట్టిక_stopped_flag = నిజం;
}

శూన్య కాలపట్టిక_single_అడుగు(పూర్ణాంకం)
{
      కాలపట్టిక_single_అడుగు_flag = నిజం;
}

తార్కిక కాలపట్టిక_ముగిసిన(శూన్య)
{
      తిరిగి !కాలపట్టిక_runnable;
}

తార్కిక కాలపట్టిక_stopped(శూన్య)
{
      తిరిగి కాలపట్టిక_stopped_flag;
}

బాహ్య "C" శూన్య vvp_no_సంకేతాలు(శూన్య)
{
      no_సంకేతాలు_flag = నిజం;
}

/*
 * These are the signal handling infrastructure. The SIGINT signal
 * leads to an implicit $stop. The SIGHUP and SIGTERM signals lead
 * to an implicit $finish.
 */
బాహ్య తార్కిక stop_is_ముగింపు;

బాహ్య "C" శూన్య సంకేతాలు_handler(పూర్ణాంకం signum)
{
#నిర్వచితమైతే __MINGW32__
	// Windows implements the original UNIX semantics for signal,
	// so we have to re-establish the signal handler each time a
	// signal is caught.
      సంకేతం(signum, &సంకేతాలు_handler);
#అయితే_ముగింపు
      ఒకవేళ (signum != SIGINT)
	    stop_is_ముగింపు = నిజం;
      కాలపట్టిక_stopped_flag = నిజం;
}

స్థిర శూన్య సంకేతాలు_capture(శూన్య)
{
      ఒకవేళ (no_సంకేతాలు_flag)
	   తిరిగి;
#నిర్వచితం_కాకపోతే __MINGW32__
      సంకేతం(SIGHUP,  &సంకేతాలు_handler);
#అయితే_ముగింపు
      సంకేతం(SIGINT,  &సంకేతాలు_handler);
      సంకేతం(SIGTERM, &సంకేతాలు_handler);
}

స్థిర శూన్య సంకేతాలు_revert(శూన్య)
{
      ఒకవేళ (no_సంకేతాలు_flag)
	   తిరిగి;
#నిర్వచితం_కాకపోతే __MINGW32__
      సంకేతం(SIGHUP,  SIG_DFL);
#అయితే_ముగింపు
      సంకేతం(SIGINT,  SIG_DFL);
      సంకేతం(SIGTERM, SIG_DFL);
}

/*
 * This function puts an event on the end of the pre-simulation event queue.
 */
స్థిర శూన్య కాలపట్టిక_init_event(నిర్మాణం event_s*cur)
{
      ఒకవేళ (కాలపట్టిక_init_list == 0) {
            cur->తదుపరి_2 = cur;
      } లేకపోతే {
            cur->తదుపరి_2 = కాలపట్టిక_init_list->తదుపరి_2;
            కాలపట్టిక_init_list->తదుపరి_2 = cur;
      }
      కాలపట్టిక_init_list = cur;
}

/*
 * This function puts an event on the end of the post-simulation event queue.
 */
స్థిర శూన్య కాలపట్టిక_final_event(నిర్మాణం event_s*cur)
{
      ఒకవేళ (కాలపట్టిక_final_list == 0) {
            cur->తదుపరి_2 = cur;
      } లేకపోతే {
            cur->తదుపరి_2 = కాలపట్టిక_final_list->తదుపరి_2;
            కాలపట్టిక_final_list->తదుపరి_2 = cur;
      }
      కాలపట్టిక_final_list = cur;
}

/*
 * This function does all the hard work of putting an event into the
 * event queue. The event delay is taken from the event structure
 * itself, and the structure is placed in the right place in the
 * queue.
 */
రకనిర్వచనం గణన event_వరుస_e { SEQ_ప్రారంభం, SEQ_చురుకైన, SEQ_INACTIVE, SEQ_NBASSIGN,
			     SEQ_RWSYNC, SEQ_ROSYNC, DEL_THREAD } event_వరుస_t;

స్థిర శూన్య కాలపట్టిక_event(నిర్మాణం event_s*cur, vvp_time64_t delay,
			    event_వరుస_t select_వరుస)
{
      cur->తదుపరి_2 = cur;
      నిర్మాణం event_సమయం_s*ctim = sched_list;

      ఒకవేళ (sched_list == 0) {
	      /* Is the event_time list completely empty? Create the
		 first event_time object. */
	    ctim = కొత్త నిర్మాణం event_సమయం_s;
	    ctim->delay = delay;
	    ctim->తదుపరి_2  = 0;
	    sched_list = ctim;

      } లేకపోతే ఒకవేళ (sched_list->delay > delay) {

	      /* Am I looking for an event before the first event_time?
		 If so, create a new event_time to go in front. */
	    నిర్మాణం event_సమయం_s*tmp = కొత్త నిర్మాణం event_సమయం_s;
	    tmp->delay = delay;
	    tmp->తదుపరి_2 = ctim;
	    ctim->delay -= delay;
	    ctim = tmp;
	    sched_list = ctim;

      } లేకపోతే {
	    నిర్మాణం event_సమయం_s*prev = 0;

	    అయితే (ctim->తదుపరి_2 && (ctim->delay < delay)) {
		  delay -= ctim->delay;
		  prev = ctim;
		  ctim = ctim->తదుపరి_2;
	    }

	    ఒకవేళ (ctim->delay > delay) {
		  నిర్మాణం event_సమయం_s*tmp = కొత్త నిర్మాణం event_సమయం_s;
		  tmp->delay = delay;
		  tmp->తదుపరి_2  = prev->తదుపరి_2;
		  prev->తదుపరి_2 = tmp;

		  tmp->తదుపరి_2->delay -= delay;
		  ctim = tmp;

	    } లేకపోతే ఒకవేళ (ctim->delay == delay) {

	    } లేకపోతే {
		  ప్రతిజ్ఞా(ctim->తదుపరి_2 == 0);
		  నిర్మాణం event_సమయం_s*tmp = కొత్త నిర్మాణం event_సమయం_s;
		  tmp->delay = delay - ctim->delay;
		  tmp->తదుపరి_2 = 0;
		  ctim->తదుపరి_2 = tmp;

		  ctim = tmp;
	    }
      }

	/* By this point, ctim is the event_time structure that is to
	   receive the event at hand. Put the event in to the
	   appropriate list for the kind of assign we have at hand. */

      నిర్మాణం event_s** q = 0;

      ఎంపిక (select_వరుస) {
	  స్థితి SEQ_ప్రారంభం:
	    q = &ctim->ప్రారంభం;
	    ఆపు;

	  స్థితి SEQ_చురుకైన:
	    q = &ctim->చురుకైన;
	    ఆపు;

	  స్థితి SEQ_INACTIVE:
	    ప్రతిజ్ఞా(delay == 0);
	    q = &ctim->inactive;
	    ఆపు;

	  స్థితి SEQ_NBASSIGN:
	    q = &ctim->nbassign;
	    ఆపు;

	  స్థితి SEQ_RWSYNC:
	    q = &ctim->rwsync;
	    ఆపు;

	  స్థితి SEQ_ROSYNC:
	    q = &ctim->rosync;
	    ఆపు;

	  స్థితి DEL_THREAD:
	    q = &ctim->del_thr;
	    ఆపు;
      }

      ఒకవేళ (q) {
	    ఒకవేళ (*q) {
		  /* Put the cur event on the end of the queue. */
		  cur->తదుపరి_2 = (*q)->తదుపరి_2;
		  (*q)->తదుపరి_2 = cur;
	    }
	    *q = cur;
      }
}

స్థిర శూన్య కాలపట్టిక_event_నెట్టు(నిర్మాణం event_s*cur)
{
      ఒకవేళ ((sched_list == 0) || (sched_list->delay > 0)) {
	    కాలపట్టిక_event(cur, 0, SEQ_చురుకైన);
	    తిరిగి;
      }

      నిర్మాణం event_సమయం_s*ctim = sched_list;

      ఒకవేళ (ctim->చురుకైన == 0) {
	    cur->తదుపరి_2 = cur;
	    ctim->చురుకైన = cur;
	    తిరిగి;
      }

      cur->తదుపరి_2 = ctim->చురుకైన->తదుపరి_2;
      ctim->చురుకైన->తదుపరి_2 = cur;
}

శూన్య కాలపట్టిక_vthread(vthread_t thr, vvp_time64_t delay, తార్కిక నెట్టు_flag)
{
      నిర్మాణం vthread_event_s*cur = కొత్త vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);

      ఒకవేళ (నెట్టు_flag && (delay == 0)) {
	      /* Special case: If the delay is 0, the push_flag means
		 I can push this event in front of everything. This is
		 used by the %fork statement, for example, to perform
		 task calls. */
	    కాలపట్టిక_event_నెట్టు(cur);

      } లేకపోతే {
	    కాలపట్టిక_event(cur, delay, SEQ_చురుకైన);
      }
}

శూన్య కాలపట్టిక_t0_trigger(vvp_net_ptr_t ptr)
{
      vvp_vector4_t బిట్టు (1, BIT4_X);
      నిర్మాణం కేటాయించు_vector4_event_s*cur = కొత్త నిర్మాణం కేటాయించు_vector4_event_s(బిట్టు);
      cur->ptr = ptr;
      కాలపట్టిక_event(cur, 0, SEQ_INACTIVE);
}

శూన్య కాలపట్టిక_inactive(vthread_t thr)
{
      నిర్మాణం vthread_event_s*cur = కొత్త vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);
      కాలపట్టిక_event(cur, 0, SEQ_INACTIVE);
}

శూన్య కాలపట్టిక_init_vthread(vthread_t thr)
{
      నిర్మాణం vthread_event_s*cur = కొత్త vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);

      కాలపట్టిక_init_event(cur);
}

శూన్య కాలపట్టిక_final_vthread(vthread_t thr)
{
      నిర్మాణం vthread_event_s*cur = కొత్త vthread_event_s;

      cur->thr = thr;
      vthread_mark_final(thr);
      vthread_mark_scheduled(thr);

      కాలపట్టిక_final_event(cur);
}

శూన్య కాలపట్టిక_కేటాయించు_సదిశ(vvp_net_ptr_t ptr,
			    గుర్తులేని పునాది, గుర్తులేని vwid,
			    స్థిరరాశి vvp_vector4_t&బిట్టు,
			    vvp_time64_t delay)
{
      నిర్మాణం కేటాయించు_vector4_event_s*cur = కొత్త నిర్మాణం కేటాయించు_vector4_event_s(బిట్టు);
      cur->ptr = ptr;
      cur->పునాది = పునాది;
      cur->vwid = vwid;
      కాలపట్టిక_event(cur, delay, SEQ_NBASSIGN);
}

శూన్య కాలపట్టిక_force_సదిశ(vvp_net_t*net,
			    గుర్తులేని పునాది, గుర్తులేని vwid,
			    స్థిరరాశి vvp_vector4_t&బిట్టు,
			    vvp_time64_t delay)
{
      నిర్మాణం force_vector4_event_s*cur = కొత్త నిర్మాణం force_vector4_event_s(బిట్టు);
      cur->net = net;
      cur->పునాది = పునాది;
      cur->vwid = vwid;
      కాలపట్టిక_event(cur, delay, SEQ_NBASSIGN);
}

శూన్య కాలపట్టిక_propagate_సదిశ(vvp_net_t*net,
			       vvp_time64_t delay,
			       స్థిరరాశి vvp_vector4_t&src)
{
      నిర్మాణం propagate_vector4_event_s*cur
	    = కొత్త నిర్మాణం propagate_vector4_event_s(src);
      cur->net = net;
      కాలపట్టిక_event(cur, delay, SEQ_NBASSIGN);
}

// FIXME: This needs to create a non-blocking event, but only one per time slot.
//        Is schedule_event_ or execution actually filtering since the net is
//        already X because this is not triggering?
శూన్య కాలపట్టిక_propagate_event(vvp_net_t*net,
                              vvp_time64_t delay)
{
      vvp_vector4_t tmp (1, BIT4_X);
      నిర్మాణం propagate_vector4_event_s*cur
	    = కొత్త నిర్మాణం propagate_vector4_event_s(tmp);
      cur->net = net;
      కాలపట్టిక_event(cur, delay, SEQ_NBASSIGN);
}

శూన్య కాలపట్టిక_కేటాయించు_array_మాట(vvp_array_t mem,
				గుర్తులేని మాట_addr,
				గుర్తులేని off,
				స్థిరరాశి vvp_vector4_t&val,
				vvp_time64_t delay)
{
      నిర్మాణం కేటాయించు_array_మాట_s*cur = కొత్త నిర్మాణం కేటాయించు_array_మాట_s;
      cur->mem = mem;
      cur->adr = మాట_addr;
      cur->off = off;
      cur->val = val;
      కాలపట్టిక_event(cur, delay, SEQ_NBASSIGN);
}

శూన్య కాలపట్టిక_కేటాయించు_array_మాట(vvp_array_t mem,
				గుర్తులేని మాట_addr,
				ద్విగుణ val,
				vvp_time64_t delay)
{
      నిర్మాణం కేటాయించు_array_r_మాట_s*cur = కొత్త నిర్మాణం కేటాయించు_array_r_మాట_s;
      cur->mem = mem;
      cur->adr = మాట_addr;
      cur->val = val;
      కాలపట్టిక_event(cur, delay, SEQ_NBASSIGN);
}

శూన్య కాలపట్టిక_సమితి_సదిశ(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు)
{
      నిర్మాణం కేటాయించు_vector4_event_s*cur = కొత్త నిర్మాణం కేటాయించు_vector4_event_s(బిట్టు);
      cur->ptr = ptr;
      cur->పునాది = 0;
      cur->vwid = 0;
      కాలపట్టిక_event(cur, 0, SEQ_చురుకైన);
}

శూన్య కాలపట్టిక_సమితి_సదిశ(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector8_t&బిట్టు)
{
      నిర్మాణం కేటాయించు_vector8_event_s*cur = కొత్త నిర్మాణం కేటాయించు_vector8_event_s;
      cur->ptr = ptr;
      cur->val = బిట్టు;
      కాలపట్టిక_event(cur, 0, SEQ_చురుకైన);
}

శూన్య కాలపట్టిక_సమితి_సదిశ(vvp_net_ptr_t ptr, ద్విగుణ బిట్టు)
{
      నిర్మాణం కేటాయించు_real_event_s*cur = కొత్త నిర్మాణం కేటాయించు_real_event_s;
      cur->ptr = ptr;
      cur->val = బిట్టు;
      కాలపట్టిక_event(cur, 0, SEQ_చురుకైన);
}

శూన్య కాలపట్టిక_init_సదిశ(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు)
{
      నిర్మాణం కేటాయించు_vector4_event_s*cur = కొత్త నిర్మాణం కేటాయించు_vector4_event_s(బిట్టు);
      cur->ptr = ptr;
      cur->పునాది = 0;
      cur->vwid = 0;
      కాలపట్టిక_init_event(cur);
}

శూన్య కాలపట్టిక_init_సదిశ(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector8_t&బిట్టు)
{
      నిర్మాణం కేటాయించు_vector8_event_s*cur = కొత్త నిర్మాణం కేటాయించు_vector8_event_s;
      cur->ptr = ptr;
      cur->val = బిట్టు;
      కాలపట్టిక_init_event(cur);
}

శూన్య కాలపట్టిక_init_సదిశ(vvp_net_ptr_t ptr, ద్విగుణ బిట్టు)
{
      నిర్మాణం కేటాయించు_real_event_s*cur = కొత్త నిర్మాణం కేటాయించు_real_event_s;
      cur->ptr = ptr;
      cur->val = బిట్టు;
      కాలపట్టిక_init_event(cur);
}

శూన్య కాలపట్టిక_init_propagate(vvp_net_t*net, vvp_vector4_t బిట్టు)
{
      నిర్మాణం propagate_vector4_event_s*cur = కొత్త నిర్మాణం propagate_vector4_event_s(బిట్టు);
      cur->net = net;
      కాలపట్టిక_init_event(cur);
}

శూన్య కాలపట్టిక_init_propagate(vvp_net_t*net, ద్విగుణ బిట్టు)
{
      నిర్మాణం propagate_real_event_s*cur = కొత్త నిర్మాణం propagate_real_event_s;
      cur->net = net;
      cur->val = బిట్టు;
      కాలపట్టిక_init_event(cur);
}

శూన్య కాలపట్టిక_del_thr(vthread_t thr)
{
      నిర్మాణం del_thr_event_s*cur = కొత్త del_thr_event_s;

      cur->thr = thr;

      కాలపట్టిక_event(cur, 0, DEL_THREAD);
}

శూన్య కాలపట్టిక_generic(vvp_gen_event_t obj, vvp_time64_t delay,
		      తార్కిక sync_flag, తార్కిక ro_flag, తార్కిక delete_when_పూర్తి)
{
      నిర్మాణం generic_event_s*cur = కొత్త generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_పూర్తి = delete_when_పూర్తి;
      కాలపట్టిక_event(cur, delay,
		      sync_flag? (ro_flag?SEQ_ROSYNC:SEQ_RWSYNC) : SEQ_చురుకైన);

      ఒకవేళ (sync_flag)
	    vthread_delay_delete();
}

స్థిర తార్కిక sim_started;

శూన్య కాలపట్టిక_functor(vvp_gen_event_t obj)
{
      నిర్మాణం generic_event_s*cur = కొత్త generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_పూర్తి = అబద్ధం;
      ఒకవేళ (!sim_started) {
            కాలపట్టిక_init_event(cur);
      } లేకపోతే {
            కాలపట్టిక_event(cur, 0, SEQ_చురుకైన);
      }
}

శూన్య కాలపట్టిక_at_ప్రారంభం_యొక్క_simtime(vvp_gen_event_t obj, vvp_time64_t delay)
{
      నిర్మాణం generic_event_s*cur = కొత్త generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_పూర్తి = అబద్ధం;
      కాలపట్టిక_event(cur, delay, SEQ_ప్రారంభం);
}

/*
 * In the vvp runtime of Icarus Verilog, the SEQ_RWSYNC time step is
 * after all of the non-blocking assignments, so is effectively the
 * same as the ReadWriteSync time.
 */
శూన్య కాలపట్టిక_at_చివర_యొక్క_simtime(vvp_gen_event_t obj, vvp_time64_t delay)
{
      నిర్మాణం generic_event_s*cur = కొత్త generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_పూర్తి = అబద్ధం;
      కాలపట్టిక_event(cur, delay, SEQ_RWSYNC);
}

స్థిర vvp_time64_t కాలపట్టిక_సమయం;
vvp_time64_t కాలపట్టిక_simtime(శూన్య)
{ తిరిగి కాలపట్టిక_సమయం; }

స్థిర తార్కిక sim_at_rosync = అబద్ధం;
తార్కిక కాలపట్టిక_at_rosync(శూన్య)
{ తిరిగి sim_at_rosync; }

/*
 * The scheduler uses this function to drain the rosync events of the
 * current time. The ctim object is still in the event queue, because
 * it is legal for a rosync callback to create other rosync
 * callbacks. It is *not* legal for them to create any other kinds of
 * events, and that is why the rosync is treated specially.
 *
 * Once all the rosync callbacks are done we can safely delete any
 * threads that finished during this time step.
 */
స్థిర శూన్య నిరంతరం_rosync(నిర్మాణం event_సమయం_s*ctim)
{
      sim_at_rosync = నిజం;
      అయితే (ctim->rosync) {
	    నిర్మాణం event_s*cur = ctim->rosync->తదుపరి_2;
	    ఒకవేళ (cur->తదుపరి_2 == cur) {
		  ctim->rosync = 0;
	    } లేకపోతే {
		  ctim->rosync->తదుపరి_2 = cur->తదుపరి_2;
	    }

	    cur->నిరంతరం_నిరంతరం();
	    తొలగించు cur;
      }
      sim_at_rosync = అబద్ధం;

      అయితే (ctim->del_thr) {
	    నిర్మాణం event_s*cur = ctim->del_thr->తదుపరి_2;
	    ఒకవేళ (cur->తదుపరి_2 == cur) {
		  ctim->del_thr = 0;
	    } లేకపోతే {
		  ctim->del_thr->తదుపరి_2 = cur->తదుపరి_2;
	    }

	    cur->నిరంతరం_నిరంతరం();
	    తొలగించు cur;
      }

      ఒకవేళ (ctim->చురుకైన || ctim->inactive || ctim->nbassign || ctim->rwsync) {
	    cerr << "SCHEDULER ERROR: read-only sync events "
		 << "created RW events!" << పంక్తి_ముగింపు;
      }
}

శూన్య కాలపట్టిక_simulate(శూన్య)
{
      తార్కిక నిరంతరం_finals;
      sim_started = అబద్ధం;

      కాలపట్టిక_సమయం = 0;

      ఒకవేళ (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute EndOfCompile callbacks\n");
      }

      // Execute end of compile callbacks
      vpi_చివర_యొక్క_Compile();

      ఒకవేళ (verbose_flag) {
	    vpi_mcd_printf(1, " ...propagate initialization events\n");
      }

	// Execute initialization events.
      అయితే (కాలపట్టిక_init_list) {
	    నిర్మాణం event_s*cur = కాలపట్టిక_init_list->తదుపరి_2;
	    ఒకవేళ (cur->తదుపరి_2 == cur) {
		  కాలపట్టిక_init_list = 0;
	    } లేకపోతే {
		  కాలపట్టిక_init_list->తదుపరి_2 = cur->తదుపరి_2;
	    }
	    cur->నిరంతరం_నిరంతరం();
	    తొలగించు cur;
      }

      sim_started = నిజం;

      ఒకవేళ (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute StartOfSim callbacks\n");
      }

      // Execute start of simulation callbacks
      vpi_ప్రారంభం_యొక్క_Sim();

      సంకేతాలు_capture();

      ఒకవేళ (verbose_flag) {
	    vpi_mcd_printf(1, " ...run scheduler\n");
      }

      // If there were no compiletf, etc. errors then we are going to
      // process events and when done run the final blocks.
      నిరంతరం_finals = కాలపట్టిక_runnable;

      ఒకవేళ (కాలపట్టిక_runnable) అయితే (sched_list) {

	    ఒకవేళ (కాలపట్టిక_stopped_flag) {
		  కాలపట్టిక_stopped_flag = అబద్ధం;
		  stop_handler(0);
		  // You can finish from the debugger without a time change.
		  ఒకవేళ (!కాలపట్టిక_runnable) ఆపు;
		  కొనసాగు;
	    }

	      /* ctim is the current time step. */
	    నిర్మాణం event_సమయం_s* ctim = sched_list;

	      /* If the time is advancing, then first run the
		 postponed sync events. Run them all. */
	    ఒకవేళ (ctim->delay > 0) {

		  ఒకవేళ (!కాలపట్టిక_runnable) ఆపు;
		  కాలపట్టిక_సమయం += ctim->delay;
		    /* When the design is being traced (we are emitting
		     * file/line information) also print any time changes. */
		  ఒకవేళ (show_file_పంక్తి) {
			cerr << "Advancing to simulation time: "
			     << కాలపట్టిక_సమయం << పంక్తి_ముగింపు;
		  }
		  ctim->delay = 0;

		  vpi_తదుపరి_Sim_సమయం();
		    // Process the cbAtStartOfSimTime callbacks.
		  అయితే (ctim->ప్రారంభం) {
			నిర్మాణం event_s*cur = ctim->ప్రారంభం->తదుపరి_2;
			ఒకవేళ (cur->తదుపరి_2 == cur) {
			      ctim->ప్రారంభం = 0;
			} లేకపోతే {
			      ctim->ప్రారంభం->తదుపరి_2 = cur->తదుపరి_2;
			}
			cur->నిరంతరం_నిరంతరం();
			తొలగించు (cur);
		  }
	    }


	      /* If there are no more active events, advance the event
		 queues. If there are not events at all, then release
		 the event_time object. */
	    ఒకవేళ (ctim->చురుకైన == 0) {
		  ctim->చురుకైన = ctim->inactive;
		  ctim->inactive = 0;

		  ఒకవేళ (ctim->చురుకైన == 0) {
			ctim->చురుకైన = ctim->nbassign;
			ctim->nbassign = 0;

			ఒకవేళ (ctim->చురుకైన == 0) {
			      ctim->చురుకైన = ctim->rwsync;
			      ctim->rwsync = 0;

				/* If out of rw events, then run the rosync
				   events and delete this time step. This also
				   deletes threads as needed. */
			      ఒకవేళ (ctim->చురుకైన == 0) {
				    నిరంతరం_rosync(ctim);
				    sched_list = ctim->తదుపరి_2;
				    తొలగించు ctim;
				    కొనసాగు;
			      }
			}
		  }
	    }

	      /* Pull the first item off the list. If this is the last
		 cell in the list, then clear the list. Execute that
		 event type, and delete it. */
	    నిర్మాణం event_s*cur = ctim->చురుకైన->తదుపరి_2;
	    ఒకవేళ (cur->తదుపరి_2 == cur) {
		  ctim->చురుకైన = 0;
	    } లేకపోతే {
		  ctim->చురుకైన->తదుపరి_2 = cur->తదుపరి_2;
	    }

	    ఒకవేళ (కాలపట్టిక_single_అడుగు_flag) {
		  cur->single_అడుగు_display();
		  కాలపట్టిక_stopped_flag = నిజం;
		  కాలపట్టిక_single_అడుగు_flag = అబద్ధం;
	    }

	    cur->నిరంతరం_నిరంతరం();

	    తొలగించు (cur);
      }

	// Execute final events.
      కాలపట్టిక_runnable = నిరంతరం_finals;
      అయితే (కాలపట్టిక_runnable && కాలపట్టిక_final_list) {
	    నిర్మాణం event_s*cur = కాలపట్టిక_final_list->తదుపరి_2;
	    ఒకవేళ (cur->తదుపరి_2 == cur) {
		  కాలపట్టిక_final_list = 0;
	    } లేకపోతే {
		  కాలపట్టిక_final_list->తదుపరి_2 = cur->తదుపరి_2;
	    }
	    cur->నిరంతరం_నిరంతరం();
	    తొలగించు cur;
      }

      సంకేతాలు_revert();

      ఒకవేళ (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute Postsim callbacks\n");
      }

      // Execute post-simulation callbacks
      vpiPostsim();
#నిర్వచితమైతే CHECK_WITH_VALGRIND
      కాలపట్టిక_delete();
#అయితే_ముగింపు
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
శూన్య కాలపట్టిక_delete(శూన్య)
{
      vthread_event_కుప్ప.delete_pool();
      assign4_కుప్ప.delete_pool();
      assign8_కుప్ప.delete_pool();
      assignr_కుప్ప.delete_pool();
      array_w_కుప్ప.delete_pool();
      array_r_w_కుప్ప.delete_pool();
      generic_event_కుప్ప.delete_pool();
      event_సమయం_కుప్ప.delete_pool();
}
#అయితే_ముగింపు
