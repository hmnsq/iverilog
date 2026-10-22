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

using নামাকাশ std;

unsigned long গুনতি_assign_events = 0;
unsigned long গুনতি_gen_events = 0;
unsigned long গুনতি_thread_events = 0;
  // Count the time events (A time cell created)
unsigned long গুনতি_time_events = 0;



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
সংরচনা event_s {
      সংরচনা event_s*next;
      আভাস ~event_s() { }
      আভাস শূন্য run_run(শূন্য) =0;

	// Write something about the event to stderr
      আভাস শূন্য single_step_display(শূন্য);

	// Fallback new/delete
      স্থির শূন্য*operator নূতন (size_t size) { প্রত্যাগম ::নূতন বর্ণ[size]; }
      স্থির শূন্য operator delete(শূন্য*ptr)  { ::delete[]( static_cast<বর্ণ*>(ptr) ); }
};

শূন্য event_s::single_step_display(শূন্য)
{
      std::cerr << "event_s: Step into event " << typeid(*এতত্).name() << std::endl;
}

সংরচনা event_time_s {
      event_time_s() {
	    গুনতি_time_events += 1;
	    delay = 0;
	    start = 0;
	    active = 0;
	    inactive = 0;
	    nbassign = 0;
	    rwsync = 0;
	    rosync = 0;
	    del_thr = 0;
	    next = রিক্ত;
      }
      vvp_time64_t delay;

      সংরচনা event_s*start;
      সংরচনা event_s*active;
      সংরচনা event_s*inactive;
      সংরচনা event_s*nbassign;
      সংরচনা event_s*rwsync;
      সংরচনা event_s*rosync;
      সংরচনা event_s*del_thr;

      সংরচনা event_time_s*next;

      স্থির শূন্য* operator নূতন (size_t);
      স্থির শূন্য operator delete(শূন্য*obj, size_t s);
};

vvp_gen_event_s::~vvp_gen_event_s()
{
}

শূন্য vvp_gen_event_s::single_step_display(শূন্য)
{
      cerr << "vvp_gen_event_s: Step into event " << typeid(*এতত্).name() << endl;
}

/*
 * Derived event types
 */
সংরচনা vthread_event_s : সার্বজনিক event_s {
      vthread_t thr;
      শূন্য run_run(শূন্য) override;
      শূন্য single_step_display(শূন্য) override;

      স্থির শূন্য* operator নূতন(size_t);
      স্থির শূন্য operator delete(শূন্য*);
};

শূন্য vthread_event_s::run_run(শূন্য)
{
      গুনতি_thread_events += 1;
      vthread_run(thr);
}

শূন্য vthread_event_s::single_step_display(শূন্য)
{
      __vpiScope*scope = vthread_scope(thr);
      cerr << "vthread_event: Resume thread"
	   << " scope=" << scope->vpi_get_str(vpiFullName)
	   << endl;
}

স্থির ধ্রুব size_t VTHR_CHUNK_গুনতি = 8192 / পরিমাণ(সংরচনা vthread_event_s);
স্থির slab_t<পরিমাণ(vthread_event_s),VTHR_CHUNK_গুনতি> vthread_event_heap;

অন্তঃস্থ শূন্য* vthread_event_s::operator নূতন(size_t size)
{
      প্রতিজ্ঞা(size == পরিমাণ(vthread_event_s));
      প্রত্যাগম vthread_event_heap.alloc_slab();
}

শূন্য vthread_event_s::operator delete(শূন্য*dptr)
{
      vthread_event_heap.free_slab(dptr);
}

সংরচনা del_thr_event_s : সার্বজনিক event_s {
      vthread_t thr;
      শূন্য run_run(শূন্য) override;
      শূন্য single_step_display(শূন্য) override;
};

শূন্য del_thr_event_s::run_run(শূন্য)
{
      vthread_delete(thr);
}

শূন্য del_thr_event_s::single_step_display(শূন্য)
{
      __vpiScope*scope = vthread_scope(thr);
      cerr << "del_thr_event: Reap completed thread"
	   << " scope=" << scope->vpi_get_str(vpiFullName) << endl;
}

সংরচনা assign_vector4_event_s  : সার্বজনিক event_s {
	/* The default constructor. */
      explicit assign_vector4_event_s(ধ্রুব vvp_vector4_t&that) : val(that) {
	    base = 0;
	    vwid = 0;
	    next = রিক্ত;
      }

	/* Where to do the assign. */
      vvp_net_ptr_t ptr;
	/* Value to assign. */
      vvp_vector4_t val;
	/* Offset of the part into the destination. */
      unsigned base;
	/* Width of the destination vector. */
      unsigned vwid;
      শূন্য run_run(শূন্য) override;
      শূন্য single_step_display(শূন্য) override;

      স্থির শূন্য* operator নূতন(size_t);
      স্থির শূন্য operator delete(শূন্য*);
};

শূন্য assign_vector4_event_s::run_run(শূন্য)
{
      গুনতি_assign_events += 1;
      যদি (vwid > 0)
	    vvp_send_vec4_pv(ptr, val, base, vwid, 0);
      অন্যথা
	    vvp_send_vec4(ptr, val, 0);
}

শূন্য assign_vector4_event_s::single_step_display(শূন্য)
{
      cerr << "assign_vector4_event: Propagate val=" << val
	   << ", vwid=" << vwid << ", base=" << base << endl;
}

স্থির ধ্রুব size_t ASSIGN4_CHUNK_গুনতি = 524288 / পরিমাণ(সংরচনা assign_vector4_event_s);
স্থির slab_t<পরিমাণ(assign_vector4_event_s),ASSIGN4_CHUNK_গুনতি> assign4_heap;

অন্তঃস্থ শূন্য* assign_vector4_event_s::operator নূতন(size_t size)
{
      প্রতিজ্ঞা(size == পরিমাণ(assign_vector4_event_s));
      প্রত্যাগম assign4_heap.alloc_slab();
}

শূন্য assign_vector4_event_s::operator delete(শূন্য*dptr)
{
      assign4_heap.free_slab(dptr);
}

unsigned long গুনতি_assign4_pool(শূন্য) { প্রত্যাগম assign4_heap.pool; }

সংরচনা assign_vector8_event_s  : সার্বজনিক event_s {
      vvp_net_ptr_t ptr;
      vvp_vector8_t val;
      শূন্য run_run(শূন্য) override;
      শূন্য single_step_display(শূন্য) override;

      স্থির শূন্য* operator নূতন(size_t);
      স্থির শূন্য operator delete(শূন্য*);
};

শূন্য assign_vector8_event_s::run_run(শূন্য)
{
      গুনতি_assign_events += 1;
      vvp_send_vec8(ptr, val);
}

শূন্য assign_vector8_event_s::single_step_display(শূন্য)
{
      cerr << "assign_vector8_event: Propagate val=" << val << endl;
}

স্থির ধ্রুব size_t ASSIGN8_CHUNK_গুনতি = 8192 / পরিমাণ(সংরচনা assign_vector8_event_s);
স্থির slab_t<পরিমাণ(assign_vector8_event_s),ASSIGN8_CHUNK_গুনতি> assign8_heap;

অন্তঃস্থ শূন্য* assign_vector8_event_s::operator নূতন(size_t size)
{
      প্রতিজ্ঞা(size == পরিমাণ(assign_vector8_event_s));
      প্রত্যাগম assign8_heap.alloc_slab();
}

শূন্য assign_vector8_event_s::operator delete(শূন্য*dptr)
{
      assign8_heap.free_slab(dptr);
}

unsigned long গুনতি_assign8_pool() { প্রত্যাগম assign8_heap.pool; }

সংরচনা assign_real_event_s  : সার্বজনিক event_s {
      vvp_net_ptr_t ptr;
      দ্বিগুণ val;
      শূন্য run_run(শূন্য) override;
      শূন্য single_step_display(শূন্য) override;

      স্থির শূন্য* operator নূতন(size_t);
      স্থির শূন্য operator delete(শূন্য*);
};

শূন্য assign_real_event_s::run_run(শূন্য)
{
      গুনতি_assign_events += 1;
      vvp_send_real(ptr, val, 0);
}

শূন্য assign_real_event_s::single_step_display(শূন্য)
{
      cerr << "assign_real_event: Propagate val=" << val << endl;
}

স্থির ধ্রুব size_t ASSIGNR_CHUNK_গুনতি = 8192 / পরিমাণ(সংরচনা assign_real_event_s);
স্থির slab_t<পরিমাণ(assign_real_event_s),ASSIGNR_CHUNK_গুনতি> assignr_heap;

অন্তঃস্থ শূন্য* assign_real_event_s::operator নূতন (size_t size)
{
      প্রতিজ্ঞা(size == পরিমাণ(assign_real_event_s));
      প্রত্যাগম assignr_heap.alloc_slab();
}

শূন্য assign_real_event_s::operator delete(শূন্য*dptr)
{
      assignr_heap.free_slab(dptr);
}

unsigned long গুনতি_assign_real_pool(শূন্য) { প্রত্যাগম assignr_heap.pool; }

সংরচনা assign_array_word_s  : সার্বজনিক event_s {
      vvp_array_t mem;
      unsigned adr;
      vvp_vector4_t val;
      unsigned off;
      শূন্য run_run(শূন্য) override;

      স্থির শূন্য* operator নূতন(size_t);
      স্থির শূন্য operator delete(শূন্য*);
};

শূন্য assign_array_word_s::run_run(শূন্য)
{
      গুনতি_assign_events += 1;
      mem->set_word(adr, off, val);
}

স্থির ধ্রুব size_t ARRAY_W_CHUNK_গুনতি = 8192 / পরিমাণ(সংরচনা assign_array_word_s);
স্থির slab_t<পরিমাণ(assign_array_word_s),ARRAY_W_CHUNK_গুনতি> array_w_heap;

অন্তঃস্থ শূন্য* assign_array_word_s::operator নূতন (size_t size)
{
      প্রতিজ্ঞা(size == পরিমাণ(assign_array_word_s));
      প্রত্যাগম array_w_heap.alloc_slab();
}

শূন্য assign_array_word_s::operator delete(শূন্য*ptr)
{
      array_w_heap.free_slab(ptr);
}

unsigned long গুনতি_assign_aword_pool(শূন্য) { প্রত্যাগম array_w_heap.pool; }

সংরচনা force_vector4_event_s  : সার্বজনিক event_s {
	/* The default constructor. */
      explicit force_vector4_event_s(ধ্রুব vvp_vector4_t&that): val(that) {
	    net = রিক্ত;
	    base = 0;
	    vwid = 0;
	    next = রিক্ত;
      }
	/* Where to do the force. */
      vvp_net_t*net;
	/* Value to force. */
      vvp_vector4_t val;
	/* Offset of the part into the destination. */
      unsigned base;
	/* Width of the destination vector. */
      unsigned vwid;

      শূন্য run_run(শূন্য) override;
      শূন্য single_step_display(শূন্য) override;

      স্থির শূন্য* operator নূতন(size_t);
      স্থির শূন্য operator delete(শূন্য*);
};

শূন্য force_vector4_event_s::run_run(শূন্য)
{
      গুনতি_assign_events += 1;

      unsigned wid = val.size();
      যদি ((base + wid) > vwid)
	    wid = vwid - base;

	// Make a mask of which bits are to be forced, 0 for unforced
	// bits and 1 for forced bits.
      vvp_vector2_t mask (vvp_vector2_t::FILL0, vwid);
      প্রতি (unsigned idx = 0 ; idx < wid ; idx += 1)
	    mask.set_bit(base+idx, 1);

      vvp_vector4_t tmp (vwid, BIT4_Z);

	// vvp_net_t::force_vec4 propagates all the bits of the
	// forced vector value, regardless of the mask. This
	// ensures the unforced bits retain their current value.
      vvp_signal_মান*sig = dynamic_cast<vvp_signal_মান*>(net->fil);
      প্রতিজ্ঞা(sig);
      sig->vec4_মান(tmp);

      tmp.set_vec(base, val);
      net->force_vec4(tmp, mask);
}

শূন্য force_vector4_event_s::single_step_display(শূন্য)
{
      cerr << "force_vector4_event: Force val=" << val
	   << ", vwid=" << vwid << ", base=" << base << endl;
}

স্থির ধ্রুব size_t FORCE4_CHUNK_গুনতি = 8192 / পরিমাণ(সংরচনা force_vector4_event_s);
স্থির slab_t<পরিমাণ(force_vector4_event_s),FORCE4_CHUNK_গুনতি> force4_heap;

অন্তঃস্থ শূন্য* force_vector4_event_s::operator নূতন(size_t size)
{
      প্রতিজ্ঞা(size == পরিমাণ(force_vector4_event_s));
      প্রত্যাগম force4_heap.alloc_slab();
}

শূন্য force_vector4_event_s::operator delete(শূন্য*dptr)
{
      force4_heap.free_slab(dptr);
}

unsigned long গুনতি_force4_pool(শূন্য) { প্রত্যাগম force4_heap.pool; }

/*
 * This class supports the propagation of vec4 outputs from a
 * vvp_net_t object.
 */
সংরচনা propagate_vector4_event_s : সার্বজনিক event_s {
	/* The default constructor. */
      explicit propagate_vector4_event_s(ধ্রুব vvp_vector4_t&that) : val(that) {
	    net = রিক্ত;
	    next = রিক্ত;
      }
	/* A constructor that makes the val directly. */
      propagate_vector4_event_s(ধ্রুব vvp_vector4_t&that, unsigned adr, unsigned wid)
      : val(that,adr,wid) {
	    net = রিক্ত;
	    next = রিক্ত;
      }

	/* Propagate the output of this net. */
      vvp_net_t*net;
	/* value to propagate */
      vvp_vector4_t val;
	/* Action */
      শূন্য run_run(শূন্য) override;
      শূন্য single_step_display(শূন্য) override;
};

শূন্য propagate_vector4_event_s::run_run(শূন্য)
{
      net->send_vec4(val, 0);
}

শূন্য propagate_vector4_event_s::single_step_display(শূন্য)
{
      cerr << "propagate_vector4_event: Propagate val=" << val << endl;
}

/*
 * This class supports the propagation of real outputs from a
 * vvp_net_t object.
 */
সংরচনা propagate_real_event_s : সার্বজনিক event_s {
	/* Propagate the output of this net. */
      vvp_net_t*net;
	/* value to propagate */
      দ্বিগুণ val;
	/* Action */
      শূন্য run_run(শূন্য) override;
      শূন্য single_step_display(শূন্য) override;
};

শূন্য propagate_real_event_s::run_run(শূন্য)
{
      net->send_real(val, 0);
}

শূন্য propagate_real_event_s::single_step_display(শূন্য)
{
      cerr << "propagate_real_event: Propagate val=" << val << endl;
}

সংরচনা assign_array_r_word_s  : সার্বজনিক event_s {
      vvp_array_t mem;
      unsigned adr;
      দ্বিগুণ val;
      শূন্য run_run(শূন্য) override;

      স্থির শূন্য* operator নূতন(size_t);
      স্থির শূন্য operator delete(শূন্য*);
};

শূন্য assign_array_r_word_s::run_run(শূন্য)
{
      গুনতি_assign_events += 1;
      mem->set_word(adr, val);
}
স্থির ধ্রুব size_t ARRAY_R_W_CHUNK_গুনতি = 8192 / পরিমাণ(সংরচনা assign_array_r_word_s);
স্থির slab_t<পরিমাণ(assign_array_r_word_s),ARRAY_R_W_CHUNK_গুনতি> array_r_w_heap;

অন্তঃস্থ শূন্য* assign_array_r_word_s::operator নূতন(size_t size)
{
      প্রতিজ্ঞা(size == পরিমাণ(assign_array_r_word_s));
      প্রত্যাগম array_r_w_heap.alloc_slab();
}

শূন্য assign_array_r_word_s::operator delete(শূন্য*ptr)
{
      array_r_w_heap.free_slab(ptr);
}

unsigned long গুনতি_assign_arword_pool(শূন্য) { প্রত্যাগম array_r_w_heap.pool; }

সংরচনা generic_event_s : সার্বজনিক event_s {
      vvp_gen_event_t obj;
      তার্কিক delete_obj_when_done;
      শূন্য run_run(শূন্য) override;
      শূন্য single_step_display(শূন্য) override;

      স্থির শূন্য* operator নূতন(size_t);
      স্থির শূন্য operator delete(শূন্য*);
};

শূন্য generic_event_s::run_run(শূন্য)
{
      গুনতি_gen_events += 1;
      যদি (obj) {
	    obj->run_run();
	    যদি (delete_obj_when_done)
		  delete obj;
      }
}

শূন্য generic_event_s::single_step_display(শূন্য)
{
      obj->single_step_display();
}

স্থির ধ্রুব size_t GENERIC_CHUNK_গুনতি = 131072 / পরিমাণ(সংরচনা generic_event_s);
স্থির slab_t<পরিমাণ(generic_event_s),GENERIC_CHUNK_গুনতি> generic_event_heap;

অন্তঃস্থ শূন্য* generic_event_s::operator নূতন(size_t size)
{
      প্রতিজ্ঞা(size == পরিমাণ(generic_event_s));
      প্রত্যাগম generic_event_heap.alloc_slab();
}

শূন্য generic_event_s::operator delete(শূন্য*ptr)
{
      generic_event_heap.free_slab(ptr);
}

unsigned long গুনতি_gen_pool(শূন্য) { প্রত্যাগম generic_event_heap.pool; }

/*
** These event_time_s will be required a lot, at high frequency.
** Once allocated, we never free them, but stash them away for next time.
*/


স্থির ধ্রুব size_t TIME_CHUNK_গুনতি = 8192 / পরিমাণ(সংরচনা event_time_s);
স্থির slab_t<পরিমাণ(event_time_s),TIME_CHUNK_গুনতি> event_time_heap;

অন্তঃস্থ শূন্য* event_time_s::operator নূতন (size_t size)
{
      প্রতিজ্ঞা(size == পরিমাণ(সংরচনা event_time_s));
      শূন্য*ptr = event_time_heap.alloc_slab();
      প্রত্যাগম ptr;
}

অন্তঃস্থ শূন্য event_time_s::operator delete(শূন্য*ptr, size_t)
{
      event_time_heap.free_slab(ptr);
}

unsigned long গুনতি_time_pool(শূন্য) { প্রত্যাগম event_time_heap.pool; }

/*
 * This is the head of the list of pending events. This includes all
 * the events that have not been executed yet, and reaches into the
 * future.
 */
স্থির সংরচনা event_time_s* sched_list = 0;

/*
 * This is a list of initialization events. The setup puts
 * initializations in this list so that they happen before the
 * simulation as a whole starts. This prevents time-0 triggers of
 * certain events.
 */
স্থির সংরচনা event_s* schedule_init_list = 0;

/*
 * This is the head of the list of final events.
 */
স্থির সংরচনা event_s* schedule_final_list = 0;

/*
 * This flag is true until a VPI task or function finishes the
 * simulation.
 */
স্থির তার্কিক schedule_runnable = সত্য;
স্থির তার্কিক schedule_stopped_flag  = অসত্য;
স্থির তার্কিক schedule_single_step_flag = অসত্য;
স্থির তার্কিক no_signals_flag = অসত্য;

শূন্য schedule_finish(পূর্ণাঙ্ক)
{
      schedule_runnable = অসত্য;
}

শূন্য schedule_stop(পূর্ণাঙ্ক)
{
      schedule_stopped_flag = সত্য;
}

শূন্য schedule_single_step(পূর্ণাঙ্ক)
{
      schedule_single_step_flag = সত্য;
}

তার্কিক schedule_finished(শূন্য)
{
      প্রত্যাগম !schedule_runnable;
}

তার্কিক schedule_stopped(শূন্য)
{
      প্রত্যাগম schedule_stopped_flag;
}

বাহ্য "C" শূন্য vvp_no_signals(শূন্য)
{
      no_signals_flag = সত্য;
}

/*
 * These are the signal handling infrastructure. The SIGINT signal
 * leads to an implicit $stop. The SIGHUP and SIGTERM signals lead
 * to an implicit $finish.
 */
বাহ্য তার্কিক stop_is_finish;

বাহ্য "C" শূন্য signals_handler(পূর্ণাঙ্ক signum)
{
#ifdef __MINGW32__
	// Windows implements the original UNIX semantics for signal,
	// so we have to re-establish the signal handler each time a
	// signal is caught.
      সঙ্কেত(signum, &signals_handler);
#endif
      যদি (signum != SIGINT)
	    stop_is_finish = সত্য;
      schedule_stopped_flag = সত্য;
}

স্থির শূন্য signals_capture(শূন্য)
{
      যদি (no_signals_flag)
	   প্রত্যাগম;
#ifndef __MINGW32__
      সঙ্কেত(SIGHUP,  &signals_handler);
#endif
      সঙ্কেত(SIGINT,  &signals_handler);
      সঙ্কেত(SIGTERM, &signals_handler);
}

স্থির শূন্য signals_revert(শূন্য)
{
      যদি (no_signals_flag)
	   প্রত্যাগম;
#ifndef __MINGW32__
      সঙ্কেত(SIGHUP,  SIG_DFL);
#endif
      সঙ্কেত(SIGINT,  SIG_DFL);
      সঙ্কেত(SIGTERM, SIG_DFL);
}

/*
 * This function puts an event on the end of the pre-simulation event queue.
 */
স্থির শূন্য schedule_init_event(সংরচনা event_s*cur)
{
      যদি (schedule_init_list == 0) {
            cur->next = cur;
      } অন্যথা {
            cur->next = schedule_init_list->next;
            schedule_init_list->next = cur;
      }
      schedule_init_list = cur;
}

/*
 * This function puts an event on the end of the post-simulation event queue.
 */
স্থির শূন্য schedule_final_event(সংরচনা event_s*cur)
{
      যদি (schedule_final_list == 0) {
            cur->next = cur;
      } অন্যথা {
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
প্রকারনির্দেশ গণনা event_queue_e { SEQ_START, SEQ_ACTIVE, SEQ_INACTIVE, SEQ_NBASSIGN,
			     SEQ_RWSYNC, SEQ_ROSYNC, DEL_THREAD } event_queue_t;

স্থির শূন্য schedule_event_(সংরচনা event_s*cur, vvp_time64_t delay,
			    event_queue_t select_queue)
{
      cur->next = cur;
      সংরচনা event_time_s*ctim = sched_list;

      যদি (sched_list == 0) {
	      /* Is the event_time list completely empty? Create the
		 first event_time object. */
	    ctim = নূতন সংরচনা event_time_s;
	    ctim->delay = delay;
	    ctim->next  = 0;
	    sched_list = ctim;

      } অন্যথা যদি (sched_list->delay > delay) {

	      /* Am I looking for an event before the first event_time?
		 If so, create a new event_time to go in front. */
	    সংরচনা event_time_s*tmp = নূতন সংরচনা event_time_s;
	    tmp->delay = delay;
	    tmp->next = ctim;
	    ctim->delay -= delay;
	    ctim = tmp;
	    sched_list = ctim;

      } অন্যথা {
	    সংরচনা event_time_s*prev = 0;

	    যাবত্ (ctim->next && (ctim->delay < delay)) {
		  delay -= ctim->delay;
		  prev = ctim;
		  ctim = ctim->next;
	    }

	    যদি (ctim->delay > delay) {
		  সংরচনা event_time_s*tmp = নূতন সংরচনা event_time_s;
		  tmp->delay = delay;
		  tmp->next  = prev->next;
		  prev->next = tmp;

		  tmp->next->delay -= delay;
		  ctim = tmp;

	    } অন্যথা যদি (ctim->delay == delay) {

	    } অন্যথা {
		  প্রতিজ্ঞা(ctim->next == 0);
		  সংরচনা event_time_s*tmp = নূতন সংরচনা event_time_s;
		  tmp->delay = delay - ctim->delay;
		  tmp->next = 0;
		  ctim->next = tmp;

		  ctim = tmp;
	    }
      }

	/* By this point, ctim is the event_time structure that is to
	   receive the event at hand. Put the event in to the
	   appropriate list for the kind of assign we have at hand. */

      সংরচনা event_s** q = 0;

      বিকল্প (select_queue) {
	  পক্ষ SEQ_START:
	    q = &ctim->start;
	    ভঙ্গ;

	  পক্ষ SEQ_ACTIVE:
	    q = &ctim->active;
	    ভঙ্গ;

	  পক্ষ SEQ_INACTIVE:
	    প্রতিজ্ঞা(delay == 0);
	    q = &ctim->inactive;
	    ভঙ্গ;

	  পক্ষ SEQ_NBASSIGN:
	    q = &ctim->nbassign;
	    ভঙ্গ;

	  পক্ষ SEQ_RWSYNC:
	    q = &ctim->rwsync;
	    ভঙ্গ;

	  পক্ষ SEQ_ROSYNC:
	    q = &ctim->rosync;
	    ভঙ্গ;

	  পক্ষ DEL_THREAD:
	    q = &ctim->del_thr;
	    ভঙ্গ;
      }

      যদি (q) {
	    যদি (*q) {
		  /* Put the cur event on the end of the queue. */
		  cur->next = (*q)->next;
		  (*q)->next = cur;
	    }
	    *q = cur;
      }
}

স্থির শূন্য schedule_event_ঠেলো(সংরচনা event_s*cur)
{
      যদি ((sched_list == 0) || (sched_list->delay > 0)) {
	    schedule_event_(cur, 0, SEQ_ACTIVE);
	    প্রত্যাগম;
      }

      সংরচনা event_time_s*ctim = sched_list;

      যদি (ctim->active == 0) {
	    cur->next = cur;
	    ctim->active = cur;
	    প্রত্যাগম;
      }

      cur->next = ctim->active->next;
      ctim->active->next = cur;
}

শূন্য schedule_vthread(vthread_t thr, vvp_time64_t delay, তার্কিক ঠেলো_flag)
{
      সংরচনা vthread_event_s*cur = নূতন vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);

      যদি (ঠেলো_flag && (delay == 0)) {
	      /* Special case: If the delay is 0, the push_flag means
		 I can push this event in front of everything. This is
		 used by the %fork statement, for example, to perform
		 task calls. */
	    schedule_event_ঠেলো(cur);

      } অন্যথা {
	    schedule_event_(cur, delay, SEQ_ACTIVE);
      }
}

শূন্য schedule_t0_trigger(vvp_net_ptr_t ptr)
{
      vvp_vector4_t bit (1, BIT4_X);
      সংরচনা assign_vector4_event_s*cur = নূতন সংরচনা assign_vector4_event_s(bit);
      cur->ptr = ptr;
      schedule_event_(cur, 0, SEQ_INACTIVE);
}

শূন্য schedule_inactive(vthread_t thr)
{
      সংরচনা vthread_event_s*cur = নূতন vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);
      schedule_event_(cur, 0, SEQ_INACTIVE);
}

শূন্য schedule_init_vthread(vthread_t thr)
{
      সংরচনা vthread_event_s*cur = নূতন vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);

      schedule_init_event(cur);
}

শূন্য schedule_final_vthread(vthread_t thr)
{
      সংরচনা vthread_event_s*cur = নূতন vthread_event_s;

      cur->thr = thr;
      vthread_mark_final(thr);
      vthread_mark_scheduled(thr);

      schedule_final_event(cur);
}

শূন্য schedule_assign_vector(vvp_net_ptr_t ptr,
			    unsigned base, unsigned vwid,
			    ধ্রুব vvp_vector4_t&bit,
			    vvp_time64_t delay)
{
      সংরচনা assign_vector4_event_s*cur = নূতন সংরচনা assign_vector4_event_s(bit);
      cur->ptr = ptr;
      cur->base = base;
      cur->vwid = vwid;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

শূন্য schedule_force_vector(vvp_net_t*net,
			    unsigned base, unsigned vwid,
			    ধ্রুব vvp_vector4_t&bit,
			    vvp_time64_t delay)
{
      সংরচনা force_vector4_event_s*cur = নূতন সংরচনা force_vector4_event_s(bit);
      cur->net = net;
      cur->base = base;
      cur->vwid = vwid;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

শূন্য schedule_propagate_vector(vvp_net_t*net,
			       vvp_time64_t delay,
			       ধ্রুব vvp_vector4_t&src)
{
      সংরচনা propagate_vector4_event_s*cur
	    = নূতন সংরচনা propagate_vector4_event_s(src);
      cur->net = net;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

// FIXME: This needs to create a non-blocking event, but only one per time slot.
//        Is schedule_event_ or execution actually filtering since the net is
//        already X because this is not triggering?
শূন্য schedule_propagate_event(vvp_net_t*net,
                              vvp_time64_t delay)
{
      vvp_vector4_t tmp (1, BIT4_X);
      সংরচনা propagate_vector4_event_s*cur
	    = নূতন সংরচনা propagate_vector4_event_s(tmp);
      cur->net = net;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

শূন্য schedule_assign_array_word(vvp_array_t mem,
				unsigned word_addr,
				unsigned off,
				ধ্রুব vvp_vector4_t&val,
				vvp_time64_t delay)
{
      সংরচনা assign_array_word_s*cur = নূতন সংরচনা assign_array_word_s;
      cur->mem = mem;
      cur->adr = word_addr;
      cur->off = off;
      cur->val = val;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

শূন্য schedule_assign_array_word(vvp_array_t mem,
				unsigned word_addr,
				দ্বিগুণ val,
				vvp_time64_t delay)
{
      সংরচনা assign_array_r_word_s*cur = নূতন সংরচনা assign_array_r_word_s;
      cur->mem = mem;
      cur->adr = word_addr;
      cur->val = val;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

শূন্য schedule_set_vector(vvp_net_ptr_t ptr, ধ্রুব vvp_vector4_t&bit)
{
      সংরচনা assign_vector4_event_s*cur = নূতন সংরচনা assign_vector4_event_s(bit);
      cur->ptr = ptr;
      cur->base = 0;
      cur->vwid = 0;
      schedule_event_(cur, 0, SEQ_ACTIVE);
}

শূন্য schedule_set_vector(vvp_net_ptr_t ptr, ধ্রুব vvp_vector8_t&bit)
{
      সংরচনা assign_vector8_event_s*cur = নূতন সংরচনা assign_vector8_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_event_(cur, 0, SEQ_ACTIVE);
}

শূন্য schedule_set_vector(vvp_net_ptr_t ptr, দ্বিগুণ bit)
{
      সংরচনা assign_real_event_s*cur = নূতন সংরচনা assign_real_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_event_(cur, 0, SEQ_ACTIVE);
}

শূন্য schedule_init_vector(vvp_net_ptr_t ptr, ধ্রুব vvp_vector4_t&bit)
{
      সংরচনা assign_vector4_event_s*cur = নূতন সংরচনা assign_vector4_event_s(bit);
      cur->ptr = ptr;
      cur->base = 0;
      cur->vwid = 0;
      schedule_init_event(cur);
}

শূন্য schedule_init_vector(vvp_net_ptr_t ptr, ধ্রুব vvp_vector8_t&bit)
{
      সংরচনা assign_vector8_event_s*cur = নূতন সংরচনা assign_vector8_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_init_event(cur);
}

শূন্য schedule_init_vector(vvp_net_ptr_t ptr, দ্বিগুণ bit)
{
      সংরচনা assign_real_event_s*cur = নূতন সংরচনা assign_real_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_init_event(cur);
}

শূন্য schedule_init_propagate(vvp_net_t*net, vvp_vector4_t bit)
{
      সংরচনা propagate_vector4_event_s*cur = নূতন সংরচনা propagate_vector4_event_s(bit);
      cur->net = net;
      schedule_init_event(cur);
}

শূন্য schedule_init_propagate(vvp_net_t*net, দ্বিগুণ bit)
{
      সংরচনা propagate_real_event_s*cur = নূতন সংরচনা propagate_real_event_s;
      cur->net = net;
      cur->val = bit;
      schedule_init_event(cur);
}

শূন্য schedule_del_thr(vthread_t thr)
{
      সংরচনা del_thr_event_s*cur = নূতন del_thr_event_s;

      cur->thr = thr;

      schedule_event_(cur, 0, DEL_THREAD);
}

শূন্য schedule_generic(vvp_gen_event_t obj, vvp_time64_t delay,
		      তার্কিক sync_flag, তার্কিক ro_flag, তার্কিক delete_when_done)
{
      সংরচনা generic_event_s*cur = নূতন generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = delete_when_done;
      schedule_event_(cur, delay,
		      sync_flag? (ro_flag?SEQ_ROSYNC:SEQ_RWSYNC) : SEQ_ACTIVE);

      যদি (sync_flag)
	    vthread_delay_delete();
}

স্থির তার্কিক sim_started;

শূন্য schedule_functor(vvp_gen_event_t obj)
{
      সংরচনা generic_event_s*cur = নূতন generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = অসত্য;
      যদি (!sim_started) {
            schedule_init_event(cur);
      } অন্যথা {
            schedule_event_(cur, 0, SEQ_ACTIVE);
      }
}

শূন্য schedule_at_start_of_simtime(vvp_gen_event_t obj, vvp_time64_t delay)
{
      সংরচনা generic_event_s*cur = নূতন generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = অসত্য;
      schedule_event_(cur, delay, SEQ_START);
}

/*
 * In the vvp runtime of Icarus Verilog, the SEQ_RWSYNC time step is
 * after all of the non-blocking assignments, so is effectively the
 * same as the ReadWriteSync time.
 */
শূন্য schedule_at_end_of_simtime(vvp_gen_event_t obj, vvp_time64_t delay)
{
      সংরচনা generic_event_s*cur = নূতন generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = অসত্য;
      schedule_event_(cur, delay, SEQ_RWSYNC);
}

স্থির vvp_time64_t schedule_time;
vvp_time64_t schedule_simtime(শূন্য)
{ প্রত্যাগম schedule_time; }

স্থির তার্কিক sim_at_rosync = অসত্য;
তার্কিক schedule_at_rosync(শূন্য)
{ প্রত্যাগম sim_at_rosync; }

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
স্থির শূন্য run_rosync(সংরচনা event_time_s*ctim)
{
      sim_at_rosync = সত্য;
      যাবত্ (ctim->rosync) {
	    সংরচনা event_s*cur = ctim->rosync->next;
	    যদি (cur->next == cur) {
		  ctim->rosync = 0;
	    } অন্যথা {
		  ctim->rosync->next = cur->next;
	    }

	    cur->run_run();
	    delete cur;
      }
      sim_at_rosync = অসত্য;

      যাবত্ (ctim->del_thr) {
	    সংরচনা event_s*cur = ctim->del_thr->next;
	    যদি (cur->next == cur) {
		  ctim->del_thr = 0;
	    } অন্যথা {
		  ctim->del_thr->next = cur->next;
	    }

	    cur->run_run();
	    delete cur;
      }

      যদি (ctim->active || ctim->inactive || ctim->nbassign || ctim->rwsync) {
	    cerr << "SCHEDULER ERROR: read-only sync events "
		 << "created RW events!" << endl;
      }
}

শূন্য schedule_simulate(শূন্য)
{
      তার্কিক run_finals;
      sim_started = অসত্য;

      schedule_time = 0;

      যদি (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute EndOfCompile callbacks\n");
      }

      // Execute end of compile callbacks
      vpiEndOfCompile();

      যদি (verbose_flag) {
	    vpi_mcd_printf(1, " ...propagate initialization events\n");
      }

	// Execute initialization events.
      যাবত্ (schedule_init_list) {
	    সংরচনা event_s*cur = schedule_init_list->next;
	    যদি (cur->next == cur) {
		  schedule_init_list = 0;
	    } অন্যথা {
		  schedule_init_list->next = cur->next;
	    }
	    cur->run_run();
	    delete cur;
      }

      sim_started = সত্য;

      যদি (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute StartOfSim callbacks\n");
      }

      // Execute start of simulation callbacks
      vpiStartOfSim();

      signals_capture();

      যদি (verbose_flag) {
	    vpi_mcd_printf(1, " ...run scheduler\n");
      }

      // If there were no compiletf, etc. errors then we are going to
      // process events and when done run the final blocks.
      run_finals = schedule_runnable;

      যদি (schedule_runnable) যাবত্ (sched_list) {

	    যদি (schedule_stopped_flag) {
		  schedule_stopped_flag = অসত্য;
		  stop_handler(0);
		  // You can finish from the debugger without a time change.
		  যদি (!schedule_runnable) ভঙ্গ;
		  অনুবর্ত;
	    }

	      /* ctim is the current time step. */
	    সংরচনা event_time_s* ctim = sched_list;

	      /* If the time is advancing, then first run the
		 postponed sync events. Run them all. */
	    যদি (ctim->delay > 0) {

		  যদি (!schedule_runnable) ভঙ্গ;
		  schedule_time += ctim->delay;
		    /* When the design is being traced (we are emitting
		     * file/line information) also print any time changes. */
		  যদি (show_file_সারি) {
			cerr << "Advancing to simulation time: "
			     << schedule_time << endl;
		  }
		  ctim->delay = 0;

		  vpiNextSimTime();
		    // Process the cbAtStartOfSimTime callbacks.
		  যাবত্ (ctim->start) {
			সংরচনা event_s*cur = ctim->start->next;
			যদি (cur->next == cur) {
			      ctim->start = 0;
			} অন্যথা {
			      ctim->start->next = cur->next;
			}
			cur->run_run();
			delete (cur);
		  }
	    }


	      /* If there are no more active events, advance the event
		 queues. If there are not events at all, then release
		 the event_time object. */
	    যদি (ctim->active == 0) {
		  ctim->active = ctim->inactive;
		  ctim->inactive = 0;

		  যদি (ctim->active == 0) {
			ctim->active = ctim->nbassign;
			ctim->nbassign = 0;

			যদি (ctim->active == 0) {
			      ctim->active = ctim->rwsync;
			      ctim->rwsync = 0;

				/* If out of rw events, then run the rosync
				   events and delete this time step. This also
				   deletes threads as needed. */
			      যদি (ctim->active == 0) {
				    run_rosync(ctim);
				    sched_list = ctim->next;
				    delete ctim;
				    অনুবর্ত;
			      }
			}
		  }
	    }

	      /* Pull the first item off the list. If this is the last
		 cell in the list, then clear the list. Execute that
		 event type, and delete it. */
	    সংরচনা event_s*cur = ctim->active->next;
	    যদি (cur->next == cur) {
		  ctim->active = 0;
	    } অন্যথা {
		  ctim->active->next = cur->next;
	    }

	    যদি (schedule_single_step_flag) {
		  cur->single_step_display();
		  schedule_stopped_flag = সত্য;
		  schedule_single_step_flag = অসত্য;
	    }

	    cur->run_run();

	    delete (cur);
      }

	// Execute final events.
      schedule_runnable = run_finals;
      যাবত্ (schedule_runnable && schedule_final_list) {
	    সংরচনা event_s*cur = schedule_final_list->next;
	    যদি (cur->next == cur) {
		  schedule_final_list = 0;
	    } অন্যথা {
		  schedule_final_list->next = cur->next;
	    }
	    cur->run_run();
	    delete cur;
      }

      signals_revert();

      যদি (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute Postsim callbacks\n");
      }

      // Execute post-simulation callbacks
      vpiPostsim();
#ifdef CHECK_WITH_VALGRIND
      schedule_delete();
#endif
}

#ifdef CHECK_WITH_VALGRIND
শূন্য schedule_delete(শূন্য)
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
