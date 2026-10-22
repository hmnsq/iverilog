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
#ifdef CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
# include  "ivl_alloc.h"
#endif

using നാമാകാശ std;

unsigned long എണ്ണം_assign_events = 0;
unsigned long എണ്ണം_gen_events = 0;
unsigned long എണ്ണം_thread_events = 0;
  // Count the time events (A time cell created)
unsigned long എണ്ണം_time_events = 0;



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
ഘടന event_s {
      ഘടന event_s*next;
      മായാ ~event_s() { }
      മായാ ശൂന്യം run_run(ശൂന്യം) =0;

	// Write something about the event to stderr
      മായാ ശൂന്യം single_step_display(ശൂന്യം);

	// Fallback new/delete
      സ്ഥിതം ശൂന്യം*operator പുതിയ (size_t size) { തിരികെ ::പുതിയ അക്ഷരം[size]; }
      സ്ഥിതം ശൂന്യം operator delete(ശൂന്യം*ptr)  { ::delete[]( static_cast<അക്ഷരം*>(ptr) ); }
};

ശൂന്യം event_s::single_step_display(ശൂന്യം)
{
      std::cerr << "event_s: Step into event " << typeid(*ഇത്).name() << std::endl;
}

ഘടന event_time_s {
      event_time_s() {
	    എണ്ണം_time_events += 1;
	    delay = 0;
	    start = 0;
	    active = 0;
	    inactive = 0;
	    nbassign = 0;
	    rwsync = 0;
	    rosync = 0;
	    del_thr = 0;
	    next = ശൂന്യ;
      }
      vvp_time64_t delay;

      ഘടന event_s*start;
      ഘടന event_s*active;
      ഘടന event_s*inactive;
      ഘടന event_s*nbassign;
      ഘടന event_s*rwsync;
      ഘടന event_s*rosync;
      ഘടന event_s*del_thr;

      ഘടന event_time_s*next;

      സ്ഥിതം ശൂന്യം* operator പുതിയ (size_t);
      സ്ഥിതം ശൂന്യം operator delete(ശൂന്യം*obj, size_t s);
};

vvp_gen_event_s::~vvp_gen_event_s()
{
}

ശൂന്യം vvp_gen_event_s::single_step_display(ശൂന്യം)
{
      cerr << "vvp_gen_event_s: Step into event " << typeid(*ഇത്).name() << endl;
}

/*
 * Derived event types
 */
ഘടന vthread_event_s : പൊതു event_s {
      vthread_t thr;
      ശൂന്യം run_run(ശൂന്യം) override;
      ശൂന്യം single_step_display(ശൂന്യം) override;

      സ്ഥിതം ശൂന്യം* operator പുതിയ(size_t);
      സ്ഥിതം ശൂന്യം operator delete(ശൂന്യം*);
};

ശൂന്യം vthread_event_s::run_run(ശൂന്യം)
{
      എണ്ണം_thread_events += 1;
      vthread_run(thr);
}

ശൂന്യം vthread_event_s::single_step_display(ശൂന്യം)
{
      __vpiScope*scope = vthread_scope(thr);
      cerr << "vthread_event: Resume thread"
	   << " scope=" << scope->vpi_get_str(vpiFullName)
	   << endl;
}

സ്ഥിതം സ്ഥിരം size_t VTHR_CHUNK_എണ്ണം = 8192 / വലുപ്പം(ഘടന vthread_event_s);
സ്ഥിതം slab_t<വലുപ്പം(vthread_event_s),VTHR_CHUNK_എണ്ണം> vthread_event_heap;

വരിയിൽ ശൂന്യം* vthread_event_s::operator പുതിയ(size_t size)
{
      പ്രതിജ്ഞാ(size == വലുപ്പം(vthread_event_s));
      തിരികെ vthread_event_heap.alloc_slab();
}

ശൂന്യം vthread_event_s::operator delete(ശൂന്യം*dptr)
{
      vthread_event_heap.free_slab(dptr);
}

ഘടന del_thr_event_s : പൊതു event_s {
      vthread_t thr;
      ശൂന്യം run_run(ശൂന്യം) override;
      ശൂന്യം single_step_display(ശൂന്യം) override;
};

ശൂന്യം del_thr_event_s::run_run(ശൂന്യം)
{
      vthread_delete(thr);
}

ശൂന്യം del_thr_event_s::single_step_display(ശൂന്യം)
{
      __vpiScope*scope = vthread_scope(thr);
      cerr << "del_thr_event: Reap completed thread"
	   << " scope=" << scope->vpi_get_str(vpiFullName) << endl;
}

ഘടന assign_vector4_event_s  : പൊതു event_s {
	/* The default constructor. */
      explicit assign_vector4_event_s(സ്ഥിരം vvp_vector4_t&that) : val(that) {
	    base = 0;
	    vwid = 0;
	    next = ശൂന്യ;
      }

	/* Where to do the assign. */
      vvp_net_ptr_t ptr;
	/* Value to assign. */
      vvp_vector4_t val;
	/* Offset of the part into the destination. */
      unsigned base;
	/* Width of the destination vector. */
      unsigned vwid;
      ശൂന്യം run_run(ശൂന്യം) override;
      ശൂന്യം single_step_display(ശൂന്യം) override;

      സ്ഥിതം ശൂന്യം* operator പുതിയ(size_t);
      സ്ഥിതം ശൂന്യം operator delete(ശൂന്യം*);
};

ശൂന്യം assign_vector4_event_s::run_run(ശൂന്യം)
{
      എണ്ണം_assign_events += 1;
      എങ്കിൽ (vwid > 0)
	    vvp_send_vec4_pv(ptr, val, base, vwid, 0);
      അല്ലെങ്കിൽ
	    vvp_send_vec4(ptr, val, 0);
}

ശൂന്യം assign_vector4_event_s::single_step_display(ശൂന്യം)
{
      cerr << "assign_vector4_event: Propagate val=" << val
	   << ", vwid=" << vwid << ", base=" << base << endl;
}

സ്ഥിതം സ്ഥിരം size_t ASSIGN4_CHUNK_എണ്ണം = 524288 / വലുപ്പം(ഘടന assign_vector4_event_s);
സ്ഥിതം slab_t<വലുപ്പം(assign_vector4_event_s),ASSIGN4_CHUNK_എണ്ണം> assign4_heap;

വരിയിൽ ശൂന്യം* assign_vector4_event_s::operator പുതിയ(size_t size)
{
      പ്രതിജ്ഞാ(size == വലുപ്പം(assign_vector4_event_s));
      തിരികെ assign4_heap.alloc_slab();
}

ശൂന്യം assign_vector4_event_s::operator delete(ശൂന്യം*dptr)
{
      assign4_heap.free_slab(dptr);
}

unsigned long എണ്ണം_assign4_pool(ശൂന്യം) { തിരികെ assign4_heap.pool; }

ഘടന assign_vector8_event_s  : പൊതു event_s {
      vvp_net_ptr_t ptr;
      vvp_vector8_t val;
      ശൂന്യം run_run(ശൂന്യം) override;
      ശൂന്യം single_step_display(ശൂന്യം) override;

      സ്ഥിതം ശൂന്യം* operator പുതിയ(size_t);
      സ്ഥിതം ശൂന്യം operator delete(ശൂന്യം*);
};

ശൂന്യം assign_vector8_event_s::run_run(ശൂന്യം)
{
      എണ്ണം_assign_events += 1;
      vvp_send_vec8(ptr, val);
}

ശൂന്യം assign_vector8_event_s::single_step_display(ശൂന്യം)
{
      cerr << "assign_vector8_event: Propagate val=" << val << endl;
}

സ്ഥിതം സ്ഥിരം size_t ASSIGN8_CHUNK_എണ്ണം = 8192 / വലുപ്പം(ഘടന assign_vector8_event_s);
സ്ഥിതം slab_t<വലുപ്പം(assign_vector8_event_s),ASSIGN8_CHUNK_എണ്ണം> assign8_heap;

വരിയിൽ ശൂന്യം* assign_vector8_event_s::operator പുതിയ(size_t size)
{
      പ്രതിജ്ഞാ(size == വലുപ്പം(assign_vector8_event_s));
      തിരികെ assign8_heap.alloc_slab();
}

ശൂന്യം assign_vector8_event_s::operator delete(ശൂന്യം*dptr)
{
      assign8_heap.free_slab(dptr);
}

unsigned long എണ്ണം_assign8_pool() { തിരികെ assign8_heap.pool; }

ഘടന assign_real_event_s  : പൊതു event_s {
      vvp_net_ptr_t ptr;
      ഇരട്ട val;
      ശൂന്യം run_run(ശൂന്യം) override;
      ശൂന്യം single_step_display(ശൂന്യം) override;

      സ്ഥിതം ശൂന്യം* operator പുതിയ(size_t);
      സ്ഥിതം ശൂന്യം operator delete(ശൂന്യം*);
};

ശൂന്യം assign_real_event_s::run_run(ശൂന്യം)
{
      എണ്ണം_assign_events += 1;
      vvp_send_real(ptr, val, 0);
}

ശൂന്യം assign_real_event_s::single_step_display(ശൂന്യം)
{
      cerr << "assign_real_event: Propagate val=" << val << endl;
}

സ്ഥിതം സ്ഥിരം size_t ASSIGNR_CHUNK_എണ്ണം = 8192 / വലുപ്പം(ഘടന assign_real_event_s);
സ്ഥിതം slab_t<വലുപ്പം(assign_real_event_s),ASSIGNR_CHUNK_എണ്ണം> assignr_heap;

വരിയിൽ ശൂന്യം* assign_real_event_s::operator പുതിയ (size_t size)
{
      പ്രതിജ്ഞാ(size == വലുപ്പം(assign_real_event_s));
      തിരികെ assignr_heap.alloc_slab();
}

ശൂന്യം assign_real_event_s::operator delete(ശൂന്യം*dptr)
{
      assignr_heap.free_slab(dptr);
}

unsigned long എണ്ണം_assign_real_pool(ശൂന്യം) { തിരികെ assignr_heap.pool; }

ഘടന assign_array_word_s  : പൊതു event_s {
      vvp_array_t mem;
      unsigned adr;
      vvp_vector4_t val;
      unsigned off;
      ശൂന്യം run_run(ശൂന്യം) override;

      സ്ഥിതം ശൂന്യം* operator പുതിയ(size_t);
      സ്ഥിതം ശൂന്യം operator delete(ശൂന്യം*);
};

ശൂന്യം assign_array_word_s::run_run(ശൂന്യം)
{
      എണ്ണം_assign_events += 1;
      mem->set_word(adr, off, val);
}

സ്ഥിതം സ്ഥിരം size_t ARRAY_W_CHUNK_എണ്ണം = 8192 / വലുപ്പം(ഘടന assign_array_word_s);
സ്ഥിതം slab_t<വലുപ്പം(assign_array_word_s),ARRAY_W_CHUNK_എണ്ണം> array_w_heap;

വരിയിൽ ശൂന്യം* assign_array_word_s::operator പുതിയ (size_t size)
{
      പ്രതിജ്ഞാ(size == വലുപ്പം(assign_array_word_s));
      തിരികെ array_w_heap.alloc_slab();
}

ശൂന്യം assign_array_word_s::operator delete(ശൂന്യം*ptr)
{
      array_w_heap.free_slab(ptr);
}

unsigned long എണ്ണം_assign_aword_pool(ശൂന്യം) { തിരികെ array_w_heap.pool; }

ഘടന force_vector4_event_s  : പൊതു event_s {
	/* The default constructor. */
      explicit force_vector4_event_s(സ്ഥിരം vvp_vector4_t&that): val(that) {
	    net = ശൂന്യ;
	    base = 0;
	    vwid = 0;
	    next = ശൂന്യ;
      }
	/* Where to do the force. */
      vvp_net_t*net;
	/* Value to force. */
      vvp_vector4_t val;
	/* Offset of the part into the destination. */
      unsigned base;
	/* Width of the destination vector. */
      unsigned vwid;

      ശൂന്യം run_run(ശൂന്യം) override;
      ശൂന്യം single_step_display(ശൂന്യം) override;

      സ്ഥിതം ശൂന്യം* operator പുതിയ(size_t);
      സ്ഥിതം ശൂന്യം operator delete(ശൂന്യം*);
};

ശൂന്യം force_vector4_event_s::run_run(ശൂന്യം)
{
      എണ്ണം_assign_events += 1;

      unsigned wid = val.size();
      എങ്കിൽ ((base + wid) > vwid)
	    wid = vwid - base;

	// Make a mask of which bits are to be forced, 0 for unforced
	// bits and 1 for forced bits.
      vvp_vector2_t mask (vvp_vector2_t::FILL0, vwid);
      വേണ്ടി (unsigned idx = 0 ; idx < wid ; idx += 1)
	    mask.set_bit(base+idx, 1);

      vvp_vector4_t tmp (vwid, BIT4_Z);

	// vvp_net_t::force_vec4 propagates all the bits of the
	// forced vector value, regardless of the mask. This
	// ensures the unforced bits retain their current value.
      vvp_signal_മൂല്യം*sig = dynamic_cast<vvp_signal_മൂല്യം*>(net->fil);
      പ്രതിജ്ഞാ(sig);
      sig->vec4_മൂല്യം(tmp);

      tmp.set_vec(base, val);
      net->force_vec4(tmp, mask);
}

ശൂന്യം force_vector4_event_s::single_step_display(ശൂന്യം)
{
      cerr << "force_vector4_event: Force val=" << val
	   << ", vwid=" << vwid << ", base=" << base << endl;
}

സ്ഥിതം സ്ഥിരം size_t FORCE4_CHUNK_എണ്ണം = 8192 / വലുപ്പം(ഘടന force_vector4_event_s);
സ്ഥിതം slab_t<വലുപ്പം(force_vector4_event_s),FORCE4_CHUNK_എണ്ണം> force4_heap;

വരിയിൽ ശൂന്യം* force_vector4_event_s::operator പുതിയ(size_t size)
{
      പ്രതിജ്ഞാ(size == വലുപ്പം(force_vector4_event_s));
      തിരികെ force4_heap.alloc_slab();
}

ശൂന്യം force_vector4_event_s::operator delete(ശൂന്യം*dptr)
{
      force4_heap.free_slab(dptr);
}

unsigned long എണ്ണം_force4_pool(ശൂന്യം) { തിരികെ force4_heap.pool; }

/*
 * This class supports the propagation of vec4 outputs from a
 * vvp_net_t object.
 */
ഘടന propagate_vector4_event_s : പൊതു event_s {
	/* The default constructor. */
      explicit propagate_vector4_event_s(സ്ഥിരം vvp_vector4_t&that) : val(that) {
	    net = ശൂന്യ;
	    next = ശൂന്യ;
      }
	/* A constructor that makes the val directly. */
      propagate_vector4_event_s(സ്ഥിരം vvp_vector4_t&that, unsigned adr, unsigned wid)
      : val(that,adr,wid) {
	    net = ശൂന്യ;
	    next = ശൂന്യ;
      }

	/* Propagate the output of this net. */
      vvp_net_t*net;
	/* value to propagate */
      vvp_vector4_t val;
	/* Action */
      ശൂന്യം run_run(ശൂന്യം) override;
      ശൂന്യം single_step_display(ശൂന്യം) override;
};

ശൂന്യം propagate_vector4_event_s::run_run(ശൂന്യം)
{
      net->send_vec4(val, 0);
}

ശൂന്യം propagate_vector4_event_s::single_step_display(ശൂന്യം)
{
      cerr << "propagate_vector4_event: Propagate val=" << val << endl;
}

/*
 * This class supports the propagation of real outputs from a
 * vvp_net_t object.
 */
ഘടന propagate_real_event_s : പൊതു event_s {
	/* Propagate the output of this net. */
      vvp_net_t*net;
	/* value to propagate */
      ഇരട്ട val;
	/* Action */
      ശൂന്യം run_run(ശൂന്യം) override;
      ശൂന്യം single_step_display(ശൂന്യം) override;
};

ശൂന്യം propagate_real_event_s::run_run(ശൂന്യം)
{
      net->send_real(val, 0);
}

ശൂന്യം propagate_real_event_s::single_step_display(ശൂന്യം)
{
      cerr << "propagate_real_event: Propagate val=" << val << endl;
}

ഘടന assign_array_r_word_s  : പൊതു event_s {
      vvp_array_t mem;
      unsigned adr;
      ഇരട്ട val;
      ശൂന്യം run_run(ശൂന്യം) override;

      സ്ഥിതം ശൂന്യം* operator പുതിയ(size_t);
      സ്ഥിതം ശൂന്യം operator delete(ശൂന്യം*);
};

ശൂന്യം assign_array_r_word_s::run_run(ശൂന്യം)
{
      എണ്ണം_assign_events += 1;
      mem->set_word(adr, val);
}
സ്ഥിതം സ്ഥിരം size_t ARRAY_R_W_CHUNK_എണ്ണം = 8192 / വലുപ്പം(ഘടന assign_array_r_word_s);
സ്ഥിതം slab_t<വലുപ്പം(assign_array_r_word_s),ARRAY_R_W_CHUNK_എണ്ണം> array_r_w_heap;

വരിയിൽ ശൂന്യം* assign_array_r_word_s::operator പുതിയ(size_t size)
{
      പ്രതിജ്ഞാ(size == വലുപ്പം(assign_array_r_word_s));
      തിരികെ array_r_w_heap.alloc_slab();
}

ശൂന്യം assign_array_r_word_s::operator delete(ശൂന്യം*ptr)
{
      array_r_w_heap.free_slab(ptr);
}

unsigned long എണ്ണം_assign_arword_pool(ശൂന്യം) { തിരികെ array_r_w_heap.pool; }

ഘടന generic_event_s : പൊതു event_s {
      vvp_gen_event_t obj;
      തർക്കം delete_obj_when_done;
      ശൂന്യം run_run(ശൂന്യം) override;
      ശൂന്യം single_step_display(ശൂന്യം) override;

      സ്ഥിതം ശൂന്യം* operator പുതിയ(size_t);
      സ്ഥിതം ശൂന്യം operator delete(ശൂന്യം*);
};

ശൂന്യം generic_event_s::run_run(ശൂന്യം)
{
      എണ്ണം_gen_events += 1;
      എങ്കിൽ (obj) {
	    obj->run_run();
	    എങ്കിൽ (delete_obj_when_done)
		  delete obj;
      }
}

ശൂന്യം generic_event_s::single_step_display(ശൂന്യം)
{
      obj->single_step_display();
}

സ്ഥിതം സ്ഥിരം size_t GENERIC_CHUNK_എണ്ണം = 131072 / വലുപ്പം(ഘടന generic_event_s);
സ്ഥിതം slab_t<വലുപ്പം(generic_event_s),GENERIC_CHUNK_എണ്ണം> generic_event_heap;

വരിയിൽ ശൂന്യം* generic_event_s::operator പുതിയ(size_t size)
{
      പ്രതിജ്ഞാ(size == വലുപ്പം(generic_event_s));
      തിരികെ generic_event_heap.alloc_slab();
}

ശൂന്യം generic_event_s::operator delete(ശൂന്യം*ptr)
{
      generic_event_heap.free_slab(ptr);
}

unsigned long എണ്ണം_gen_pool(ശൂന്യം) { തിരികെ generic_event_heap.pool; }

/*
** These event_time_s will be required a lot, at high frequency.
** Once allocated, we never free them, but stash them away for next time.
*/


സ്ഥിതം സ്ഥിരം size_t TIME_CHUNK_എണ്ണം = 8192 / വലുപ്പം(ഘടന event_time_s);
സ്ഥിതം slab_t<വലുപ്പം(event_time_s),TIME_CHUNK_എണ്ണം> event_time_heap;

വരിയിൽ ശൂന്യം* event_time_s::operator പുതിയ (size_t size)
{
      പ്രതിജ്ഞാ(size == വലുപ്പം(ഘടന event_time_s));
      ശൂന്യം*ptr = event_time_heap.alloc_slab();
      തിരികെ ptr;
}

വരിയിൽ ശൂന്യം event_time_s::operator delete(ശൂന്യം*ptr, size_t)
{
      event_time_heap.free_slab(ptr);
}

unsigned long എണ്ണം_time_pool(ശൂന്യം) { തിരികെ event_time_heap.pool; }

/*
 * This is the head of the list of pending events. This includes all
 * the events that have not been executed yet, and reaches into the
 * future.
 */
സ്ഥിതം ഘടന event_time_s* sched_list = 0;

/*
 * This is a list of initialization events. The setup puts
 * initializations in this list so that they happen before the
 * simulation as a whole starts. This prevents time-0 triggers of
 * certain events.
 */
സ്ഥിതം ഘടന event_s* schedule_init_list = 0;

/*
 * This is the head of the list of final events.
 */
സ്ഥിതം ഘടന event_s* schedule_final_list = 0;

/*
 * This flag is true until a VPI task or function finishes the
 * simulation.
 */
സ്ഥിതം തർക്കം schedule_runnable = സത്യം;
സ്ഥിതം തർക്കം schedule_stopped_flag  = അസത്യം;
സ്ഥിതം തർക്കം schedule_single_step_flag = അസത്യം;
സ്ഥിതം തർക്കം no_signals_flag = അസത്യം;

ശൂന്യം schedule_finish(പൂർണ്ണസംഖ്യ)
{
      schedule_runnable = അസത്യം;
}

ശൂന്യം schedule_stop(പൂർണ്ണസംഖ്യ)
{
      schedule_stopped_flag = സത്യം;
}

ശൂന്യം schedule_single_step(പൂർണ്ണസംഖ്യ)
{
      schedule_single_step_flag = സത്യം;
}

തർക്കം schedule_finished(ശൂന്യം)
{
      തിരികെ !schedule_runnable;
}

തർക്കം schedule_stopped(ശൂന്യം)
{
      തിരികെ schedule_stopped_flag;
}

ബാഹ്യ "C" ശൂന്യം vvp_no_signals(ശൂന്യം)
{
      no_signals_flag = സത്യം;
}

/*
 * These are the signal handling infrastructure. The SIGINT signal
 * leads to an implicit $stop. The SIGHUP and SIGTERM signals lead
 * to an implicit $finish.
 */
ബാഹ്യ തർക്കം stop_is_finish;

ബാഹ്യ "C" ശൂന്യം signals_handler(പൂർണ്ണസംഖ്യ signum)
{
#ifdef __MINGW32__
	// Windows implements the original UNIX semantics for signal,
	// so we have to re-establish the signal handler each time a
	// signal is caught.
      സൂചന(signum, &signals_handler);
#endif
      എങ്കിൽ (signum != SIGINT)
	    stop_is_finish = സത്യം;
      schedule_stopped_flag = സത്യം;
}

സ്ഥിതം ശൂന്യം signals_capture(ശൂന്യം)
{
      എങ്കിൽ (no_signals_flag)
	   തിരികെ;
#ifndef __MINGW32__
      സൂചന(SIGHUP,  &signals_handler);
#endif
      സൂചന(SIGINT,  &signals_handler);
      സൂചന(SIGTERM, &signals_handler);
}

സ്ഥിതം ശൂന്യം signals_revert(ശൂന്യം)
{
      എങ്കിൽ (no_signals_flag)
	   തിരികെ;
#ifndef __MINGW32__
      സൂചന(SIGHUP,  SIG_DFL);
#endif
      സൂചന(SIGINT,  SIG_DFL);
      സൂചന(SIGTERM, SIG_DFL);
}

/*
 * This function puts an event on the end of the pre-simulation event queue.
 */
സ്ഥിതം ശൂന്യം schedule_init_event(ഘടന event_s*cur)
{
      എങ്കിൽ (schedule_init_list == 0) {
            cur->next = cur;
      } അല്ലെങ്കിൽ {
            cur->next = schedule_init_list->next;
            schedule_init_list->next = cur;
      }
      schedule_init_list = cur;
}

/*
 * This function puts an event on the end of the post-simulation event queue.
 */
സ്ഥിതം ശൂന്യം schedule_final_event(ഘടന event_s*cur)
{
      എങ്കിൽ (schedule_final_list == 0) {
            cur->next = cur;
      } അല്ലെങ്കിൽ {
            cur->next = schedule_final_list->next;
            schedule_final_list->next = cur;
      }
      schedule_final_list = cur;
}

/*
 * This function does all the hard work of putting an event into the
 * event queue. The event delay is taken from the event structure
 * itself, and the structure is placed in the right place in the
 * queue.
 */
തരനിർവചനം എണ്ണൽ event_queue_e { SEQ_START, SEQ_ACTIVE, SEQ_INACTIVE, SEQ_NBASSIGN,
			     SEQ_RWSYNC, SEQ_ROSYNC, DEL_THREAD } event_queue_t;

സ്ഥിതം ശൂന്യം schedule_event_(ഘടന event_s*cur, vvp_time64_t delay,
			    event_queue_t select_queue)
{
      cur->next = cur;
      ഘടന event_time_s*ctim = sched_list;

      എങ്കിൽ (sched_list == 0) {
	      /* Is the event_time list completely empty? Create the
		 first event_time object. */
	    ctim = പുതിയ ഘടന event_time_s;
	    ctim->delay = delay;
	    ctim->next  = 0;
	    sched_list = ctim;

      } അല്ലെങ്കിൽ എങ്കിൽ (sched_list->delay > delay) {

	      /* Am I looking for an event before the first event_time?
		 If so, create a new event_time to go in front. */
	    ഘടന event_time_s*tmp = പുതിയ ഘടന event_time_s;
	    tmp->delay = delay;
	    tmp->next = ctim;
	    ctim->delay -= delay;
	    ctim = tmp;
	    sched_list = ctim;

      } അല്ലെങ്കിൽ {
	    ഘടന event_time_s*prev = 0;

	    ആകുമ്പോൾ (ctim->next && (ctim->delay < delay)) {
		  delay -= ctim->delay;
		  prev = ctim;
		  ctim = ctim->next;
	    }

	    എങ്കിൽ (ctim->delay > delay) {
		  ഘടന event_time_s*tmp = പുതിയ ഘടന event_time_s;
		  tmp->delay = delay;
		  tmp->next  = prev->next;
		  prev->next = tmp;

		  tmp->next->delay -= delay;
		  ctim = tmp;

	    } അല്ലെങ്കിൽ എങ്കിൽ (ctim->delay == delay) {

	    } അല്ലെങ്കിൽ {
		  പ്രതിജ്ഞാ(ctim->next == 0);
		  ഘടന event_time_s*tmp = പുതിയ ഘടന event_time_s;
		  tmp->delay = delay - ctim->delay;
		  tmp->next = 0;
		  ctim->next = tmp;

		  ctim = tmp;
	    }
      }

	/* By this point, ctim is the event_time structure that is to
	   receive the event at hand. Put the event in to the
	   appropriate list for the kind of assign we have at hand. */

      ഘടന event_s** q = 0;

      തിരഞ്ഞെടുപ്പ് (select_queue) {
	  അവസ്ഥ SEQ_START:
	    q = &ctim->start;
	    നിർത്തുക;

	  അവസ്ഥ SEQ_ACTIVE:
	    q = &ctim->active;
	    നിർത്തുക;

	  അവസ്ഥ SEQ_INACTIVE:
	    പ്രതിജ്ഞാ(delay == 0);
	    q = &ctim->inactive;
	    നിർത്തുക;

	  അവസ്ഥ SEQ_NBASSIGN:
	    q = &ctim->nbassign;
	    നിർത്തുക;

	  അവസ്ഥ SEQ_RWSYNC:
	    q = &ctim->rwsync;
	    നിർത്തുക;

	  അവസ്ഥ SEQ_ROSYNC:
	    q = &ctim->rosync;
	    നിർത്തുക;

	  അവസ്ഥ DEL_THREAD:
	    q = &ctim->del_thr;
	    നിർത്തുക;
      }

      എങ്കിൽ (q) {
	    എങ്കിൽ (*q) {
		  /* Put the cur event on the end of the queue. */
		  cur->next = (*q)->next;
		  (*q)->next = cur;
	    }
	    *q = cur;
      }
}

സ്ഥിതം ശൂന്യം schedule_event_തള്ളുക(ഘടന event_s*cur)
{
      എങ്കിൽ ((sched_list == 0) || (sched_list->delay > 0)) {
	    schedule_event_(cur, 0, SEQ_ACTIVE);
	    തിരികെ;
      }

      ഘടന event_time_s*ctim = sched_list;

      എങ്കിൽ (ctim->active == 0) {
	    cur->next = cur;
	    ctim->active = cur;
	    തിരികെ;
      }

      cur->next = ctim->active->next;
      ctim->active->next = cur;
}

ശൂന്യം schedule_vthread(vthread_t thr, vvp_time64_t delay, തർക്കം തള്ളുക_flag)
{
      ഘടന vthread_event_s*cur = പുതിയ vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);

      എങ്കിൽ (തള്ളുക_flag && (delay == 0)) {
	      /* Special case: If the delay is 0, the push_flag means
		 I can push this event in front of everything. This is
		 used by the %fork statement, for example, to perform
		 task calls. */
	    schedule_event_തള്ളുക(cur);

      } അല്ലെങ്കിൽ {
	    schedule_event_(cur, delay, SEQ_ACTIVE);
      }
}

ശൂന്യം schedule_t0_trigger(vvp_net_ptr_t ptr)
{
      vvp_vector4_t bit (1, BIT4_X);
      ഘടന assign_vector4_event_s*cur = പുതിയ ഘടന assign_vector4_event_s(bit);
      cur->ptr = ptr;
      schedule_event_(cur, 0, SEQ_INACTIVE);
}

ശൂന്യം schedule_inactive(vthread_t thr)
{
      ഘടന vthread_event_s*cur = പുതിയ vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);
      schedule_event_(cur, 0, SEQ_INACTIVE);
}

ശൂന്യം schedule_init_vthread(vthread_t thr)
{
      ഘടന vthread_event_s*cur = പുതിയ vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);

      schedule_init_event(cur);
}

ശൂന്യം schedule_final_vthread(vthread_t thr)
{
      ഘടന vthread_event_s*cur = പുതിയ vthread_event_s;

      cur->thr = thr;
      vthread_mark_final(thr);
      vthread_mark_scheduled(thr);

      schedule_final_event(cur);
}

ശൂന്യം schedule_assign_vector(vvp_net_ptr_t ptr,
			    unsigned base, unsigned vwid,
			    സ്ഥിരം vvp_vector4_t&bit,
			    vvp_time64_t delay)
{
      ഘടന assign_vector4_event_s*cur = പുതിയ ഘടന assign_vector4_event_s(bit);
      cur->ptr = ptr;
      cur->base = base;
      cur->vwid = vwid;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

ശൂന്യം schedule_force_vector(vvp_net_t*net,
			    unsigned base, unsigned vwid,
			    സ്ഥിരം vvp_vector4_t&bit,
			    vvp_time64_t delay)
{
      ഘടന force_vector4_event_s*cur = പുതിയ ഘടന force_vector4_event_s(bit);
      cur->net = net;
      cur->base = base;
      cur->vwid = vwid;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

ശൂന്യം schedule_propagate_vector(vvp_net_t*net,
			       vvp_time64_t delay,
			       സ്ഥിരം vvp_vector4_t&src)
{
      ഘടന propagate_vector4_event_s*cur
	    = പുതിയ ഘടന propagate_vector4_event_s(src);
      cur->net = net;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

// FIXME: This needs to create a non-blocking event, but only one per time slot.
//        Is schedule_event_ or execution actually filtering since the net is
//        already X because this is not triggering?
ശൂന്യം schedule_propagate_event(vvp_net_t*net,
                              vvp_time64_t delay)
{
      vvp_vector4_t tmp (1, BIT4_X);
      ഘടന propagate_vector4_event_s*cur
	    = പുതിയ ഘടന propagate_vector4_event_s(tmp);
      cur->net = net;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

ശൂന്യം schedule_assign_array_word(vvp_array_t mem,
				unsigned word_addr,
				unsigned off,
				സ്ഥിരം vvp_vector4_t&val,
				vvp_time64_t delay)
{
      ഘടന assign_array_word_s*cur = പുതിയ ഘടന assign_array_word_s;
      cur->mem = mem;
      cur->adr = word_addr;
      cur->off = off;
      cur->val = val;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

ശൂന്യം schedule_assign_array_word(vvp_array_t mem,
				unsigned word_addr,
				ഇരട്ട val,
				vvp_time64_t delay)
{
      ഘടന assign_array_r_word_s*cur = പുതിയ ഘടന assign_array_r_word_s;
      cur->mem = mem;
      cur->adr = word_addr;
      cur->val = val;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

ശൂന്യം schedule_set_vector(vvp_net_ptr_t ptr, സ്ഥിരം vvp_vector4_t&bit)
{
      ഘടന assign_vector4_event_s*cur = പുതിയ ഘടന assign_vector4_event_s(bit);
      cur->ptr = ptr;
      cur->base = 0;
      cur->vwid = 0;
      schedule_event_(cur, 0, SEQ_ACTIVE);
}

ശൂന്യം schedule_set_vector(vvp_net_ptr_t ptr, സ്ഥിരം vvp_vector8_t&bit)
{
      ഘടന assign_vector8_event_s*cur = പുതിയ ഘടന assign_vector8_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_event_(cur, 0, SEQ_ACTIVE);
}

ശൂന്യം schedule_set_vector(vvp_net_ptr_t ptr, ഇരട്ട bit)
{
      ഘടന assign_real_event_s*cur = പുതിയ ഘടന assign_real_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_event_(cur, 0, SEQ_ACTIVE);
}

ശൂന്യം schedule_init_vector(vvp_net_ptr_t ptr, സ്ഥിരം vvp_vector4_t&bit)
{
      ഘടന assign_vector4_event_s*cur = പുതിയ ഘടന assign_vector4_event_s(bit);
      cur->ptr = ptr;
      cur->base = 0;
      cur->vwid = 0;
      schedule_init_event(cur);
}

ശൂന്യം schedule_init_vector(vvp_net_ptr_t ptr, സ്ഥിരം vvp_vector8_t&bit)
{
      ഘടന assign_vector8_event_s*cur = പുതിയ ഘടന assign_vector8_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_init_event(cur);
}

ശൂന്യം schedule_init_vector(vvp_net_ptr_t ptr, ഇരട്ട bit)
{
      ഘടന assign_real_event_s*cur = പുതിയ ഘടന assign_real_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_init_event(cur);
}

ശൂന്യം schedule_init_propagate(vvp_net_t*net, vvp_vector4_t bit)
{
      ഘടന propagate_vector4_event_s*cur = പുതിയ ഘടന propagate_vector4_event_s(bit);
      cur->net = net;
      schedule_init_event(cur);
}

ശൂന്യം schedule_init_propagate(vvp_net_t*net, ഇരട്ട bit)
{
      ഘടന propagate_real_event_s*cur = പുതിയ ഘടന propagate_real_event_s;
      cur->net = net;
      cur->val = bit;
      schedule_init_event(cur);
}

ശൂന്യം schedule_del_thr(vthread_t thr)
{
      ഘടന del_thr_event_s*cur = പുതിയ del_thr_event_s;

      cur->thr = thr;

      schedule_event_(cur, 0, DEL_THREAD);
}

ശൂന്യം schedule_generic(vvp_gen_event_t obj, vvp_time64_t delay,
		      തർക്കം sync_flag, തർക്കം ro_flag, തർക്കം delete_when_done)
{
      ഘടന generic_event_s*cur = പുതിയ generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = delete_when_done;
      schedule_event_(cur, delay,
		      sync_flag? (ro_flag?SEQ_ROSYNC:SEQ_RWSYNC) : SEQ_ACTIVE);

      എങ്കിൽ (sync_flag)
	    vthread_delay_delete();
}

സ്ഥിതം തർക്കം sim_started;

ശൂന്യം schedule_functor(vvp_gen_event_t obj)
{
      ഘടന generic_event_s*cur = പുതിയ generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = അസത്യം;
      എങ്കിൽ (!sim_started) {
            schedule_init_event(cur);
      } അല്ലെങ്കിൽ {
            schedule_event_(cur, 0, SEQ_ACTIVE);
      }
}

ശൂന്യം schedule_at_start_of_simtime(vvp_gen_event_t obj, vvp_time64_t delay)
{
      ഘടന generic_event_s*cur = പുതിയ generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = അസത്യം;
      schedule_event_(cur, delay, SEQ_START);
}

/*
 * In the vvp runtime of Icarus Verilog, the SEQ_RWSYNC time step is
 * after all of the non-blocking assignments, so is effectively the
 * same as the ReadWriteSync time.
 */
ശൂന്യം schedule_at_end_of_simtime(vvp_gen_event_t obj, vvp_time64_t delay)
{
      ഘടന generic_event_s*cur = പുതിയ generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = അസത്യം;
      schedule_event_(cur, delay, SEQ_RWSYNC);
}

സ്ഥിതം vvp_time64_t schedule_time;
vvp_time64_t schedule_simtime(ശൂന്യം)
{ തിരികെ schedule_time; }

സ്ഥിതം തർക്കം sim_at_rosync = അസത്യം;
തർക്കം schedule_at_rosync(ശൂന്യം)
{ തിരികെ sim_at_rosync; }

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
സ്ഥിതം ശൂന്യം run_rosync(ഘടന event_time_s*ctim)
{
      sim_at_rosync = സത്യം;
      ആകുമ്പോൾ (ctim->rosync) {
	    ഘടന event_s*cur = ctim->rosync->next;
	    എങ്കിൽ (cur->next == cur) {
		  ctim->rosync = 0;
	    } അല്ലെങ്കിൽ {
		  ctim->rosync->next = cur->next;
	    }

	    cur->run_run();
	    delete cur;
      }
      sim_at_rosync = അസത്യം;

      ആകുമ്പോൾ (ctim->del_thr) {
	    ഘടന event_s*cur = ctim->del_thr->next;
	    എങ്കിൽ (cur->next == cur) {
		  ctim->del_thr = 0;
	    } അല്ലെങ്കിൽ {
		  ctim->del_thr->next = cur->next;
	    }

	    cur->run_run();
	    delete cur;
      }

      എങ്കിൽ (ctim->active || ctim->inactive || ctim->nbassign || ctim->rwsync) {
	    cerr << "SCHEDULER ERROR: read-only sync events "
		 << "created RW events!" << endl;
      }
}

ശൂന്യം schedule_simulate(ശൂന്യം)
{
      തർക്കം run_finals;
      sim_started = അസത്യം;

      schedule_time = 0;

      എങ്കിൽ (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute EndOfCompile callbacks\n");
      }

      // Execute end of compile callbacks
      vpiEndOfCompile();

      എങ്കിൽ (verbose_flag) {
	    vpi_mcd_printf(1, " ...propagate initialization events\n");
      }

	// Execute initialization events.
      ആകുമ്പോൾ (schedule_init_list) {
	    ഘടന event_s*cur = schedule_init_list->next;
	    എങ്കിൽ (cur->next == cur) {
		  schedule_init_list = 0;
	    } അല്ലെങ്കിൽ {
		  schedule_init_list->next = cur->next;
	    }
	    cur->run_run();
	    delete cur;
      }

      sim_started = സത്യം;

      എങ്കിൽ (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute StartOfSim callbacks\n");
      }

      // Execute start of simulation callbacks
      vpiStartOfSim();

      signals_capture();

      എങ്കിൽ (verbose_flag) {
	    vpi_mcd_printf(1, " ...run scheduler\n");
      }

      // If there were no compiletf, etc. errors then we are going to
      // process events and when done run the final blocks.
      run_finals = schedule_runnable;

      എങ്കിൽ (schedule_runnable) ആകുമ്പോൾ (sched_list) {

	    എങ്കിൽ (schedule_stopped_flag) {
		  schedule_stopped_flag = അസത്യം;
		  stop_handler(0);
		  // You can finish from the debugger without a time change.
		  എങ്കിൽ (!schedule_runnable) നിർത്തുക;
		  തുടരുക;
	    }

	      /* ctim is the current time step. */
	    ഘടന event_time_s* ctim = sched_list;

	      /* If the time is advancing, then first run the
		 postponed sync events. Run them all. */
	    എങ്കിൽ (ctim->delay > 0) {

		  എങ്കിൽ (!schedule_runnable) നിർത്തുക;
		  schedule_time += ctim->delay;
		    /* When the design is being traced (we are emitting
		     * file/line information) also print any time changes. */
		  എങ്കിൽ (show_file_വരി) {
			cerr << "Advancing to simulation time: "
			     << schedule_time << endl;
		  }
		  ctim->delay = 0;

		  vpiNextSimTime();
		    // Process the cbAtStartOfSimTime callbacks.
		  ആകുമ്പോൾ (ctim->start) {
			ഘടന event_s*cur = ctim->start->next;
			എങ്കിൽ (cur->next == cur) {
			      ctim->start = 0;
			} അല്ലെങ്കിൽ {
			      ctim->start->next = cur->next;
			}
			cur->run_run();
			delete (cur);
		  }
	    }


	      /* If there are no more active events, advance the event
		 queues. If there are not events at all, then release
		 the event_time object. */
	    എങ്കിൽ (ctim->active == 0) {
		  ctim->active = ctim->inactive;
		  ctim->inactive = 0;

		  എങ്കിൽ (ctim->active == 0) {
			ctim->active = ctim->nbassign;
			ctim->nbassign = 0;

			എങ്കിൽ (ctim->active == 0) {
			      ctim->active = ctim->rwsync;
			      ctim->rwsync = 0;

				/* If out of rw events, then run the rosync
				   events and delete this time step. This also
				   deletes threads as needed. */
			      എങ്കിൽ (ctim->active == 0) {
				    run_rosync(ctim);
				    sched_list = ctim->next;
				    delete ctim;
				    തുടരുക;
			      }
			}
		  }
	    }

	      /* Pull the first item off the list. If this is the last
		 cell in the list, then clear the list. Execute that
		 event type, and delete it. */
	    ഘടന event_s*cur = ctim->active->next;
	    എങ്കിൽ (cur->next == cur) {
		  ctim->active = 0;
	    } അല്ലെങ്കിൽ {
		  ctim->active->next = cur->next;
	    }

	    എങ്കിൽ (schedule_single_step_flag) {
		  cur->single_step_display();
		  schedule_stopped_flag = സത്യം;
		  schedule_single_step_flag = അസത്യം;
	    }

	    cur->run_run();

	    delete (cur);
      }

	// Execute final events.
      schedule_runnable = run_finals;
      ആകുമ്പോൾ (schedule_runnable && schedule_final_list) {
	    ഘടന event_s*cur = schedule_final_list->next;
	    എങ്കിൽ (cur->next == cur) {
		  schedule_final_list = 0;
	    } അല്ലെങ്കിൽ {
		  schedule_final_list->next = cur->next;
	    }
	    cur->run_run();
	    delete cur;
      }

      signals_revert();

      എങ്കിൽ (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute Postsim callbacks\n");
      }

      // Execute post-simulation callbacks
      vpiPostsim();
#ifdef CHECK_WITH_VALGRIND
      schedule_delete();
#endif
}

#ifdef CHECK_WITH_VALGRIND
ശൂന്യം schedule_delete(ശൂന്യം)
{
      vthread_event_heap.delete_pool();
      assign4_heap.delete_pool();
      assign8_heap.delete_pool();
      assignr_heap.delete_pool();
      array_w_heap.delete_pool();
      array_r_w_heap.delete_pool();
      generic_event_heap.delete_pool();
      event_time_heap.delete_pool();
}
#endif
