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

using நாமாகாஶ std;

unsigned long மொத்தம்_assign_events = 0;
unsigned long மொத்தம்_gen_events = 0;
unsigned long மொத்தம்_thread_events = 0;
  // Count the time events (A time cell created)
unsigned long மொத்தம்_time_events = 0;



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
கட்டமைப்பு event_s {
      கட்டமைப்பு event_s*next;
      மெய்நிகர் ~event_s() { }
      மெய்நிகர் வெறுமை run_run(வெறுமை) =0;

	// Write something about the event to stderr
      மெய்நிகர் வெறுமை single_step_display(வெறுமை);

	// Fallback new/delete
      நிலையான வெறுமை*operator புதிய (size_t size) { திரும்பு ::புதிய எழுத்து[size]; }
      நிலையான வெறுமை operator delete(வெறுமை*ptr)  { ::delete[]( static_cast<எழுத்து*>(ptr) ); }
};

வெறுமை event_s::single_step_display(வெறுமை)
{
      std::cerr << "event_s: Step into event " << typeid(*இது).name() << std::endl;
}

கட்டமைப்பு event_time_s {
      event_time_s() {
	    மொத்தம்_time_events += 1;
	    delay = 0;
	    start = 0;
	    active = 0;
	    inactive = 0;
	    nbassign = 0;
	    rwsync = 0;
	    rosync = 0;
	    del_thr = 0;
	    next = வெற்று;
      }
      vvp_time64_t delay;

      கட்டமைப்பு event_s*start;
      கட்டமைப்பு event_s*active;
      கட்டமைப்பு event_s*inactive;
      கட்டமைப்பு event_s*nbassign;
      கட்டமைப்பு event_s*rwsync;
      கட்டமைப்பு event_s*rosync;
      கட்டமைப்பு event_s*del_thr;

      கட்டமைப்பு event_time_s*next;

      நிலையான வெறுமை* operator புதிய (size_t);
      நிலையான வெறுமை operator delete(வெறுமை*obj, size_t s);
};

vvp_gen_event_s::~vvp_gen_event_s()
{
}

வெறுமை vvp_gen_event_s::single_step_display(வெறுமை)
{
      cerr << "vvp_gen_event_s: Step into event " << typeid(*இது).name() << endl;
}

/*
 * Derived event types
 */
கட்டமைப்பு vthread_event_s : பொது event_s {
      vthread_t thr;
      வெறுமை run_run(வெறுமை) override;
      வெறுமை single_step_display(வெறுமை) override;

      நிலையான வெறுமை* operator புதிய(size_t);
      நிலையான வெறுமை operator delete(வெறுமை*);
};

வெறுமை vthread_event_s::run_run(வெறுமை)
{
      மொத்தம்_thread_events += 1;
      vthread_run(thr);
}

வெறுமை vthread_event_s::single_step_display(வெறுமை)
{
      __vpiScope*scope = vthread_scope(thr);
      cerr << "vthread_event: Resume thread"
	   << " scope=" << scope->vpi_get_str(vpiFullName)
	   << endl;
}

நிலையான மாறிலி size_t VTHR_CHUNK_மொத்தம் = 8192 / அளவு(கட்டமைப்பு vthread_event_s);
நிலையான slab_t<அளவு(vthread_event_s),VTHR_CHUNK_மொத்தம்> vthread_event_heap;

உள்வரி வெறுமை* vthread_event_s::operator புதிய(size_t size)
{
      ப்ரதிஜ்ஞா(size == அளவு(vthread_event_s));
      திரும்பு vthread_event_heap.alloc_slab();
}

வெறுமை vthread_event_s::operator delete(வெறுமை*dptr)
{
      vthread_event_heap.free_slab(dptr);
}

கட்டமைப்பு del_thr_event_s : பொது event_s {
      vthread_t thr;
      வெறுமை run_run(வெறுமை) override;
      வெறுமை single_step_display(வெறுமை) override;
};

வெறுமை del_thr_event_s::run_run(வெறுமை)
{
      vthread_delete(thr);
}

வெறுமை del_thr_event_s::single_step_display(வெறுமை)
{
      __vpiScope*scope = vthread_scope(thr);
      cerr << "del_thr_event: Reap completed thread"
	   << " scope=" << scope->vpi_get_str(vpiFullName) << endl;
}

கட்டமைப்பு assign_vector4_event_s  : பொது event_s {
	/* The default constructor. */
      explicit assign_vector4_event_s(மாறிலி vvp_vector4_t&that) : val(that) {
	    base = 0;
	    vwid = 0;
	    next = வெற்று;
      }

	/* Where to do the assign. */
      vvp_net_ptr_t ptr;
	/* Value to assign. */
      vvp_vector4_t val;
	/* Offset of the part into the destination. */
      unsigned base;
	/* Width of the destination vector. */
      unsigned vwid;
      வெறுமை run_run(வெறுமை) override;
      வெறுமை single_step_display(வெறுமை) override;

      நிலையான வெறுமை* operator புதிய(size_t);
      நிலையான வெறுமை operator delete(வெறுமை*);
};

வெறுமை assign_vector4_event_s::run_run(வெறுமை)
{
      மொத்தம்_assign_events += 1;
      எனில் (vwid > 0)
	    vvp_send_vec4_pv(ptr, val, base, vwid, 0);
      இல்லையெனில்
	    vvp_send_vec4(ptr, val, 0);
}

வெறுமை assign_vector4_event_s::single_step_display(வெறுமை)
{
      cerr << "assign_vector4_event: Propagate val=" << val
	   << ", vwid=" << vwid << ", base=" << base << endl;
}

நிலையான மாறிலி size_t ASSIGN4_CHUNK_மொத்தம் = 524288 / அளவு(கட்டமைப்பு assign_vector4_event_s);
நிலையான slab_t<அளவு(assign_vector4_event_s),ASSIGN4_CHUNK_மொத்தம்> assign4_heap;

உள்வரி வெறுமை* assign_vector4_event_s::operator புதிய(size_t size)
{
      ப்ரதிஜ்ஞா(size == அளவு(assign_vector4_event_s));
      திரும்பு assign4_heap.alloc_slab();
}

வெறுமை assign_vector4_event_s::operator delete(வெறுமை*dptr)
{
      assign4_heap.free_slab(dptr);
}

unsigned long மொத்தம்_assign4_pool(வெறுமை) { திரும்பு assign4_heap.pool; }

கட்டமைப்பு assign_vector8_event_s  : பொது event_s {
      vvp_net_ptr_t ptr;
      vvp_vector8_t val;
      வெறுமை run_run(வெறுமை) override;
      வெறுமை single_step_display(வெறுமை) override;

      நிலையான வெறுமை* operator புதிய(size_t);
      நிலையான வெறுமை operator delete(வெறுமை*);
};

வெறுமை assign_vector8_event_s::run_run(வெறுமை)
{
      மொத்தம்_assign_events += 1;
      vvp_send_vec8(ptr, val);
}

வெறுமை assign_vector8_event_s::single_step_display(வெறுமை)
{
      cerr << "assign_vector8_event: Propagate val=" << val << endl;
}

நிலையான மாறிலி size_t ASSIGN8_CHUNK_மொத்தம் = 8192 / அளவு(கட்டமைப்பு assign_vector8_event_s);
நிலையான slab_t<அளவு(assign_vector8_event_s),ASSIGN8_CHUNK_மொத்தம்> assign8_heap;

உள்வரி வெறுமை* assign_vector8_event_s::operator புதிய(size_t size)
{
      ப்ரதிஜ்ஞா(size == அளவு(assign_vector8_event_s));
      திரும்பு assign8_heap.alloc_slab();
}

வெறுமை assign_vector8_event_s::operator delete(வெறுமை*dptr)
{
      assign8_heap.free_slab(dptr);
}

unsigned long மொத்தம்_assign8_pool() { திரும்பு assign8_heap.pool; }

கட்டமைப்பு assign_real_event_s  : பொது event_s {
      vvp_net_ptr_t ptr;
      இரட்டை val;
      வெறுமை run_run(வெறுமை) override;
      வெறுமை single_step_display(வெறுமை) override;

      நிலையான வெறுமை* operator புதிய(size_t);
      நிலையான வெறுமை operator delete(வெறுமை*);
};

வெறுமை assign_real_event_s::run_run(வெறுமை)
{
      மொத்தம்_assign_events += 1;
      vvp_send_real(ptr, val, 0);
}

வெறுமை assign_real_event_s::single_step_display(வெறுமை)
{
      cerr << "assign_real_event: Propagate val=" << val << endl;
}

நிலையான மாறிலி size_t ASSIGNR_CHUNK_மொத்தம் = 8192 / அளவு(கட்டமைப்பு assign_real_event_s);
நிலையான slab_t<அளவு(assign_real_event_s),ASSIGNR_CHUNK_மொத்தம்> assignr_heap;

உள்வரி வெறுமை* assign_real_event_s::operator புதிய (size_t size)
{
      ப்ரதிஜ்ஞா(size == அளவு(assign_real_event_s));
      திரும்பு assignr_heap.alloc_slab();
}

வெறுமை assign_real_event_s::operator delete(வெறுமை*dptr)
{
      assignr_heap.free_slab(dptr);
}

unsigned long மொத்தம்_assign_real_pool(வெறுமை) { திரும்பு assignr_heap.pool; }

கட்டமைப்பு assign_array_word_s  : பொது event_s {
      vvp_array_t mem;
      unsigned adr;
      vvp_vector4_t val;
      unsigned off;
      வெறுமை run_run(வெறுமை) override;

      நிலையான வெறுமை* operator புதிய(size_t);
      நிலையான வெறுமை operator delete(வெறுமை*);
};

வெறுமை assign_array_word_s::run_run(வெறுமை)
{
      மொத்தம்_assign_events += 1;
      mem->set_word(adr, off, val);
}

நிலையான மாறிலி size_t ARRAY_W_CHUNK_மொத்தம் = 8192 / அளவு(கட்டமைப்பு assign_array_word_s);
நிலையான slab_t<அளவு(assign_array_word_s),ARRAY_W_CHUNK_மொத்தம்> array_w_heap;

உள்வரி வெறுமை* assign_array_word_s::operator புதிய (size_t size)
{
      ப்ரதிஜ்ஞா(size == அளவு(assign_array_word_s));
      திரும்பு array_w_heap.alloc_slab();
}

வெறுமை assign_array_word_s::operator delete(வெறுமை*ptr)
{
      array_w_heap.free_slab(ptr);
}

unsigned long மொத்தம்_assign_aword_pool(வெறுமை) { திரும்பு array_w_heap.pool; }

கட்டமைப்பு force_vector4_event_s  : பொது event_s {
	/* The default constructor. */
      explicit force_vector4_event_s(மாறிலி vvp_vector4_t&that): val(that) {
	    net = வெற்று;
	    base = 0;
	    vwid = 0;
	    next = வெற்று;
      }
	/* Where to do the force. */
      vvp_net_t*net;
	/* Value to force. */
      vvp_vector4_t val;
	/* Offset of the part into the destination. */
      unsigned base;
	/* Width of the destination vector. */
      unsigned vwid;

      வெறுமை run_run(வெறுமை) override;
      வெறுமை single_step_display(வெறுமை) override;

      நிலையான வெறுமை* operator புதிய(size_t);
      நிலையான வெறுமை operator delete(வெறுமை*);
};

வெறுமை force_vector4_event_s::run_run(வெறுமை)
{
      மொத்தம்_assign_events += 1;

      unsigned wid = val.size();
      எனில் ((base + wid) > vwid)
	    wid = vwid - base;

	// Make a mask of which bits are to be forced, 0 for unforced
	// bits and 1 for forced bits.
      vvp_vector2_t mask (vvp_vector2_t::FILL0, vwid);
      க்கு (unsigned idx = 0 ; idx < wid ; idx += 1)
	    mask.set_bit(base+idx, 1);

      vvp_vector4_t tmp (vwid, BIT4_Z);

	// vvp_net_t::force_vec4 propagates all the bits of the
	// forced vector value, regardless of the mask. This
	// ensures the unforced bits retain their current value.
      vvp_signal_மதிப்பு*sig = dynamic_cast<vvp_signal_மதிப்பு*>(net->fil);
      ப்ரதிஜ்ஞா(sig);
      sig->vec4_மதிப்பு(tmp);

      tmp.set_vec(base, val);
      net->force_vec4(tmp, mask);
}

வெறுமை force_vector4_event_s::single_step_display(வெறுமை)
{
      cerr << "force_vector4_event: Force val=" << val
	   << ", vwid=" << vwid << ", base=" << base << endl;
}

நிலையான மாறிலி size_t FORCE4_CHUNK_மொத்தம் = 8192 / அளவு(கட்டமைப்பு force_vector4_event_s);
நிலையான slab_t<அளவு(force_vector4_event_s),FORCE4_CHUNK_மொத்தம்> force4_heap;

உள்வரி வெறுமை* force_vector4_event_s::operator புதிய(size_t size)
{
      ப்ரதிஜ்ஞா(size == அளவு(force_vector4_event_s));
      திரும்பு force4_heap.alloc_slab();
}

வெறுமை force_vector4_event_s::operator delete(வெறுமை*dptr)
{
      force4_heap.free_slab(dptr);
}

unsigned long மொத்தம்_force4_pool(வெறுமை) { திரும்பு force4_heap.pool; }

/*
 * This class supports the propagation of vec4 outputs from a
 * vvp_net_t object.
 */
கட்டமைப்பு propagate_vector4_event_s : பொது event_s {
	/* The default constructor. */
      explicit propagate_vector4_event_s(மாறிலி vvp_vector4_t&that) : val(that) {
	    net = வெற்று;
	    next = வெற்று;
      }
	/* A constructor that makes the val directly. */
      propagate_vector4_event_s(மாறிலி vvp_vector4_t&that, unsigned adr, unsigned wid)
      : val(that,adr,wid) {
	    net = வெற்று;
	    next = வெற்று;
      }

	/* Propagate the output of this net. */
      vvp_net_t*net;
	/* value to propagate */
      vvp_vector4_t val;
	/* Action */
      வெறுமை run_run(வெறுமை) override;
      வெறுமை single_step_display(வெறுமை) override;
};

வெறுமை propagate_vector4_event_s::run_run(வெறுமை)
{
      net->send_vec4(val, 0);
}

வெறுமை propagate_vector4_event_s::single_step_display(வெறுமை)
{
      cerr << "propagate_vector4_event: Propagate val=" << val << endl;
}

/*
 * This class supports the propagation of real outputs from a
 * vvp_net_t object.
 */
கட்டமைப்பு propagate_real_event_s : பொது event_s {
	/* Propagate the output of this net. */
      vvp_net_t*net;
	/* value to propagate */
      இரட்டை val;
	/* Action */
      வெறுமை run_run(வெறுமை) override;
      வெறுமை single_step_display(வெறுமை) override;
};

வெறுமை propagate_real_event_s::run_run(வெறுமை)
{
      net->send_real(val, 0);
}

வெறுமை propagate_real_event_s::single_step_display(வெறுமை)
{
      cerr << "propagate_real_event: Propagate val=" << val << endl;
}

கட்டமைப்பு assign_array_r_word_s  : பொது event_s {
      vvp_array_t mem;
      unsigned adr;
      இரட்டை val;
      வெறுமை run_run(வெறுமை) override;

      நிலையான வெறுமை* operator புதிய(size_t);
      நிலையான வெறுமை operator delete(வெறுமை*);
};

வெறுமை assign_array_r_word_s::run_run(வெறுமை)
{
      மொத்தம்_assign_events += 1;
      mem->set_word(adr, val);
}
நிலையான மாறிலி size_t ARRAY_R_W_CHUNK_மொத்தம் = 8192 / அளவு(கட்டமைப்பு assign_array_r_word_s);
நிலையான slab_t<அளவு(assign_array_r_word_s),ARRAY_R_W_CHUNK_மொத்தம்> array_r_w_heap;

உள்வரி வெறுமை* assign_array_r_word_s::operator புதிய(size_t size)
{
      ப்ரதிஜ்ஞா(size == அளவு(assign_array_r_word_s));
      திரும்பு array_r_w_heap.alloc_slab();
}

வெறுமை assign_array_r_word_s::operator delete(வெறுமை*ptr)
{
      array_r_w_heap.free_slab(ptr);
}

unsigned long மொத்தம்_assign_arword_pool(வெறுமை) { திரும்பு array_r_w_heap.pool; }

கட்டமைப்பு generic_event_s : பொது event_s {
      vvp_gen_event_t obj;
      தருக்க delete_obj_when_done;
      வெறுமை run_run(வெறுமை) override;
      வெறுமை single_step_display(வெறுமை) override;

      நிலையான வெறுமை* operator புதிய(size_t);
      நிலையான வெறுமை operator delete(வெறுமை*);
};

வெறுமை generic_event_s::run_run(வெறுமை)
{
      மொத்தம்_gen_events += 1;
      எனில் (obj) {
	    obj->run_run();
	    எனில் (delete_obj_when_done)
		  delete obj;
      }
}

வெறுமை generic_event_s::single_step_display(வெறுமை)
{
      obj->single_step_display();
}

நிலையான மாறிலி size_t GENERIC_CHUNK_மொத்தம் = 131072 / அளவு(கட்டமைப்பு generic_event_s);
நிலையான slab_t<அளவு(generic_event_s),GENERIC_CHUNK_மொத்தம்> generic_event_heap;

உள்வரி வெறுமை* generic_event_s::operator புதிய(size_t size)
{
      ப்ரதிஜ்ஞா(size == அளவு(generic_event_s));
      திரும்பு generic_event_heap.alloc_slab();
}

வெறுமை generic_event_s::operator delete(வெறுமை*ptr)
{
      generic_event_heap.free_slab(ptr);
}

unsigned long மொத்தம்_gen_pool(வெறுமை) { திரும்பு generic_event_heap.pool; }

/*
** These event_time_s will be required a lot, at high frequency.
** Once allocated, we never free them, but stash them away for next time.
*/


நிலையான மாறிலி size_t TIME_CHUNK_மொத்தம் = 8192 / அளவு(கட்டமைப்பு event_time_s);
நிலையான slab_t<அளவு(event_time_s),TIME_CHUNK_மொத்தம்> event_time_heap;

உள்வரி வெறுமை* event_time_s::operator புதிய (size_t size)
{
      ப்ரதிஜ்ஞா(size == அளவு(கட்டமைப்பு event_time_s));
      வெறுமை*ptr = event_time_heap.alloc_slab();
      திரும்பு ptr;
}

உள்வரி வெறுமை event_time_s::operator delete(வெறுமை*ptr, size_t)
{
      event_time_heap.free_slab(ptr);
}

unsigned long மொத்தம்_time_pool(வெறுமை) { திரும்பு event_time_heap.pool; }

/*
 * This is the head of the list of pending events. This includes all
 * the events that have not been executed yet, and reaches into the
 * future.
 */
நிலையான கட்டமைப்பு event_time_s* sched_list = 0;

/*
 * This is a list of initialization events. The setup puts
 * initializations in this list so that they happen before the
 * simulation as a whole starts. This prevents time-0 triggers of
 * certain events.
 */
நிலையான கட்டமைப்பு event_s* schedule_init_list = 0;

/*
 * This is the head of the list of final events.
 */
நிலையான கட்டமைப்பு event_s* schedule_final_list = 0;

/*
 * This flag is true until a VPI task or function finishes the
 * simulation.
 */
நிலையான தருக்க schedule_runnable = உண்மை;
நிலையான தருக்க schedule_stopped_flag  = பொய்;
நிலையான தருக்க schedule_single_step_flag = பொய்;
நிலையான தருக்க no_signals_flag = பொய்;

வெறுமை schedule_finish(முழு)
{
      schedule_runnable = பொய்;
}

வெறுமை schedule_stop(முழு)
{
      schedule_stopped_flag = உண்மை;
}

வெறுமை schedule_single_step(முழு)
{
      schedule_single_step_flag = உண்மை;
}

தருக்க schedule_finished(வெறுமை)
{
      திரும்பு !schedule_runnable;
}

தருக்க schedule_stopped(வெறுமை)
{
      திரும்பு schedule_stopped_flag;
}

வெளி "C" வெறுமை vvp_no_signals(வெறுமை)
{
      no_signals_flag = உண்மை;
}

/*
 * These are the signal handling infrastructure. The SIGINT signal
 * leads to an implicit $stop. The SIGHUP and SIGTERM signals lead
 * to an implicit $finish.
 */
வெளி தருக்க stop_is_finish;

வெளி "C" வெறுமை signals_handler(முழு signum)
{
#ifdef __MINGW32__
	// Windows implements the original UNIX semantics for signal,
	// so we have to re-establish the signal handler each time a
	// signal is caught.
      சமிக்ஞை(signum, &signals_handler);
#endif
      எனில் (signum != SIGINT)
	    stop_is_finish = உண்மை;
      schedule_stopped_flag = உண்மை;
}

நிலையான வெறுமை signals_capture(வெறுமை)
{
      எனில் (no_signals_flag)
	   திரும்பு;
#ifndef __MINGW32__
      சமிக்ஞை(SIGHUP,  &signals_handler);
#endif
      சமிக்ஞை(SIGINT,  &signals_handler);
      சமிக்ஞை(SIGTERM, &signals_handler);
}

நிலையான வெறுமை signals_revert(வெறுமை)
{
      எனில் (no_signals_flag)
	   திரும்பு;
#ifndef __MINGW32__
      சமிக்ஞை(SIGHUP,  SIG_DFL);
#endif
      சமிக்ஞை(SIGINT,  SIG_DFL);
      சமிக்ஞை(SIGTERM, SIG_DFL);
}

/*
 * This function puts an event on the end of the pre-simulation event queue.
 */
நிலையான வெறுமை schedule_init_event(கட்டமைப்பு event_s*cur)
{
      எனில் (schedule_init_list == 0) {
            cur->next = cur;
      } இல்லையெனில் {
            cur->next = schedule_init_list->next;
            schedule_init_list->next = cur;
      }
      schedule_init_list = cur;
}

/*
 * This function puts an event on the end of the post-simulation event queue.
 */
நிலையான வெறுமை schedule_final_event(கட்டமைப்பு event_s*cur)
{
      எனில் (schedule_final_list == 0) {
            cur->next = cur;
      } இல்லையெனில் {
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
வகைவரை எண்ணிக்கை event_queue_e { SEQ_START, SEQ_ACTIVE, SEQ_INACTIVE, SEQ_NBASSIGN,
			     SEQ_RWSYNC, SEQ_ROSYNC, DEL_THREAD } event_queue_t;

நிலையான வெறுமை schedule_event_(கட்டமைப்பு event_s*cur, vvp_time64_t delay,
			    event_queue_t select_queue)
{
      cur->next = cur;
      கட்டமைப்பு event_time_s*ctim = sched_list;

      எனில் (sched_list == 0) {
	      /* Is the event_time list completely empty? Create the
		 first event_time object. */
	    ctim = புதிய கட்டமைப்பு event_time_s;
	    ctim->delay = delay;
	    ctim->next  = 0;
	    sched_list = ctim;

      } இல்லையெனில் எனில் (sched_list->delay > delay) {

	      /* Am I looking for an event before the first event_time?
		 If so, create a new event_time to go in front. */
	    கட்டமைப்பு event_time_s*tmp = புதிய கட்டமைப்பு event_time_s;
	    tmp->delay = delay;
	    tmp->next = ctim;
	    ctim->delay -= delay;
	    ctim = tmp;
	    sched_list = ctim;

      } இல்லையெனில் {
	    கட்டமைப்பு event_time_s*prev = 0;

	    போது (ctim->next && (ctim->delay < delay)) {
		  delay -= ctim->delay;
		  prev = ctim;
		  ctim = ctim->next;
	    }

	    எனில் (ctim->delay > delay) {
		  கட்டமைப்பு event_time_s*tmp = புதிய கட்டமைப்பு event_time_s;
		  tmp->delay = delay;
		  tmp->next  = prev->next;
		  prev->next = tmp;

		  tmp->next->delay -= delay;
		  ctim = tmp;

	    } இல்லையெனில் எனில் (ctim->delay == delay) {

	    } இல்லையெனில் {
		  ப்ரதிஜ்ஞா(ctim->next == 0);
		  கட்டமைப்பு event_time_s*tmp = புதிய கட்டமைப்பு event_time_s;
		  tmp->delay = delay - ctim->delay;
		  tmp->next = 0;
		  ctim->next = tmp;

		  ctim = tmp;
	    }
      }

	/* By this point, ctim is the event_time structure that is to
	   receive the event at hand. Put the event in to the
	   appropriate list for the kind of assign we have at hand. */

      கட்டமைப்பு event_s** q = 0;

      தேர்வு (select_queue) {
	  நிலை SEQ_START:
	    q = &ctim->start;
	    நிறுத்து;

	  நிலை SEQ_ACTIVE:
	    q = &ctim->active;
	    நிறுத்து;

	  நிலை SEQ_INACTIVE:
	    ப்ரதிஜ்ஞா(delay == 0);
	    q = &ctim->inactive;
	    நிறுத்து;

	  நிலை SEQ_NBASSIGN:
	    q = &ctim->nbassign;
	    நிறுத்து;

	  நிலை SEQ_RWSYNC:
	    q = &ctim->rwsync;
	    நிறுத்து;

	  நிலை SEQ_ROSYNC:
	    q = &ctim->rosync;
	    நிறுத்து;

	  நிலை DEL_THREAD:
	    q = &ctim->del_thr;
	    நிறுத்து;
      }

      எனில் (q) {
	    எனில் (*q) {
		  /* Put the cur event on the end of the queue. */
		  cur->next = (*q)->next;
		  (*q)->next = cur;
	    }
	    *q = cur;
      }
}

நிலையான வெறுமை schedule_event_தள்ளு(கட்டமைப்பு event_s*cur)
{
      எனில் ((sched_list == 0) || (sched_list->delay > 0)) {
	    schedule_event_(cur, 0, SEQ_ACTIVE);
	    திரும்பு;
      }

      கட்டமைப்பு event_time_s*ctim = sched_list;

      எனில் (ctim->active == 0) {
	    cur->next = cur;
	    ctim->active = cur;
	    திரும்பு;
      }

      cur->next = ctim->active->next;
      ctim->active->next = cur;
}

வெறுமை schedule_vthread(vthread_t thr, vvp_time64_t delay, தருக்க தள்ளு_flag)
{
      கட்டமைப்பு vthread_event_s*cur = புதிய vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);

      எனில் (தள்ளு_flag && (delay == 0)) {
	      /* Special case: If the delay is 0, the push_flag means
		 I can push this event in front of everything. This is
		 used by the %fork statement, for example, to perform
		 task calls. */
	    schedule_event_தள்ளு(cur);

      } இல்லையெனில் {
	    schedule_event_(cur, delay, SEQ_ACTIVE);
      }
}

வெறுமை schedule_t0_trigger(vvp_net_ptr_t ptr)
{
      vvp_vector4_t bit (1, BIT4_X);
      கட்டமைப்பு assign_vector4_event_s*cur = புதிய கட்டமைப்பு assign_vector4_event_s(bit);
      cur->ptr = ptr;
      schedule_event_(cur, 0, SEQ_INACTIVE);
}

வெறுமை schedule_inactive(vthread_t thr)
{
      கட்டமைப்பு vthread_event_s*cur = புதிய vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);
      schedule_event_(cur, 0, SEQ_INACTIVE);
}

வெறுமை schedule_init_vthread(vthread_t thr)
{
      கட்டமைப்பு vthread_event_s*cur = புதிய vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);

      schedule_init_event(cur);
}

வெறுமை schedule_final_vthread(vthread_t thr)
{
      கட்டமைப்பு vthread_event_s*cur = புதிய vthread_event_s;

      cur->thr = thr;
      vthread_mark_final(thr);
      vthread_mark_scheduled(thr);

      schedule_final_event(cur);
}

வெறுமை schedule_assign_vector(vvp_net_ptr_t ptr,
			    unsigned base, unsigned vwid,
			    மாறிலி vvp_vector4_t&bit,
			    vvp_time64_t delay)
{
      கட்டமைப்பு assign_vector4_event_s*cur = புதிய கட்டமைப்பு assign_vector4_event_s(bit);
      cur->ptr = ptr;
      cur->base = base;
      cur->vwid = vwid;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

வெறுமை schedule_force_vector(vvp_net_t*net,
			    unsigned base, unsigned vwid,
			    மாறிலி vvp_vector4_t&bit,
			    vvp_time64_t delay)
{
      கட்டமைப்பு force_vector4_event_s*cur = புதிய கட்டமைப்பு force_vector4_event_s(bit);
      cur->net = net;
      cur->base = base;
      cur->vwid = vwid;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

வெறுமை schedule_propagate_vector(vvp_net_t*net,
			       vvp_time64_t delay,
			       மாறிலி vvp_vector4_t&src)
{
      கட்டமைப்பு propagate_vector4_event_s*cur
	    = புதிய கட்டமைப்பு propagate_vector4_event_s(src);
      cur->net = net;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

// FIXME: This needs to create a non-blocking event, but only one per time slot.
//        Is schedule_event_ or execution actually filtering since the net is
//        already X because this is not triggering?
வெறுமை schedule_propagate_event(vvp_net_t*net,
                              vvp_time64_t delay)
{
      vvp_vector4_t tmp (1, BIT4_X);
      கட்டமைப்பு propagate_vector4_event_s*cur
	    = புதிய கட்டமைப்பு propagate_vector4_event_s(tmp);
      cur->net = net;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

வெறுமை schedule_assign_array_word(vvp_array_t mem,
				unsigned word_addr,
				unsigned off,
				மாறிலி vvp_vector4_t&val,
				vvp_time64_t delay)
{
      கட்டமைப்பு assign_array_word_s*cur = புதிய கட்டமைப்பு assign_array_word_s;
      cur->mem = mem;
      cur->adr = word_addr;
      cur->off = off;
      cur->val = val;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

வெறுமை schedule_assign_array_word(vvp_array_t mem,
				unsigned word_addr,
				இரட்டை val,
				vvp_time64_t delay)
{
      கட்டமைப்பு assign_array_r_word_s*cur = புதிய கட்டமைப்பு assign_array_r_word_s;
      cur->mem = mem;
      cur->adr = word_addr;
      cur->val = val;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

வெறுமை schedule_set_vector(vvp_net_ptr_t ptr, மாறிலி vvp_vector4_t&bit)
{
      கட்டமைப்பு assign_vector4_event_s*cur = புதிய கட்டமைப்பு assign_vector4_event_s(bit);
      cur->ptr = ptr;
      cur->base = 0;
      cur->vwid = 0;
      schedule_event_(cur, 0, SEQ_ACTIVE);
}

வெறுமை schedule_set_vector(vvp_net_ptr_t ptr, மாறிலி vvp_vector8_t&bit)
{
      கட்டமைப்பு assign_vector8_event_s*cur = புதிய கட்டமைப்பு assign_vector8_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_event_(cur, 0, SEQ_ACTIVE);
}

வெறுமை schedule_set_vector(vvp_net_ptr_t ptr, இரட்டை bit)
{
      கட்டமைப்பு assign_real_event_s*cur = புதிய கட்டமைப்பு assign_real_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_event_(cur, 0, SEQ_ACTIVE);
}

வெறுமை schedule_init_vector(vvp_net_ptr_t ptr, மாறிலி vvp_vector4_t&bit)
{
      கட்டமைப்பு assign_vector4_event_s*cur = புதிய கட்டமைப்பு assign_vector4_event_s(bit);
      cur->ptr = ptr;
      cur->base = 0;
      cur->vwid = 0;
      schedule_init_event(cur);
}

வெறுமை schedule_init_vector(vvp_net_ptr_t ptr, மாறிலி vvp_vector8_t&bit)
{
      கட்டமைப்பு assign_vector8_event_s*cur = புதிய கட்டமைப்பு assign_vector8_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_init_event(cur);
}

வெறுமை schedule_init_vector(vvp_net_ptr_t ptr, இரட்டை bit)
{
      கட்டமைப்பு assign_real_event_s*cur = புதிய கட்டமைப்பு assign_real_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_init_event(cur);
}

வெறுமை schedule_init_propagate(vvp_net_t*net, vvp_vector4_t bit)
{
      கட்டமைப்பு propagate_vector4_event_s*cur = புதிய கட்டமைப்பு propagate_vector4_event_s(bit);
      cur->net = net;
      schedule_init_event(cur);
}

வெறுமை schedule_init_propagate(vvp_net_t*net, இரட்டை bit)
{
      கட்டமைப்பு propagate_real_event_s*cur = புதிய கட்டமைப்பு propagate_real_event_s;
      cur->net = net;
      cur->val = bit;
      schedule_init_event(cur);
}

வெறுமை schedule_del_thr(vthread_t thr)
{
      கட்டமைப்பு del_thr_event_s*cur = புதிய del_thr_event_s;

      cur->thr = thr;

      schedule_event_(cur, 0, DEL_THREAD);
}

வெறுமை schedule_generic(vvp_gen_event_t obj, vvp_time64_t delay,
		      தருக்க sync_flag, தருக்க ro_flag, தருக்க delete_when_done)
{
      கட்டமைப்பு generic_event_s*cur = புதிய generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = delete_when_done;
      schedule_event_(cur, delay,
		      sync_flag? (ro_flag?SEQ_ROSYNC:SEQ_RWSYNC) : SEQ_ACTIVE);

      எனில் (sync_flag)
	    vthread_delay_delete();
}

நிலையான தருக்க sim_started;

வெறுமை schedule_functor(vvp_gen_event_t obj)
{
      கட்டமைப்பு generic_event_s*cur = புதிய generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = பொய்;
      எனில் (!sim_started) {
            schedule_init_event(cur);
      } இல்லையெனில் {
            schedule_event_(cur, 0, SEQ_ACTIVE);
      }
}

வெறுமை schedule_at_start_of_simtime(vvp_gen_event_t obj, vvp_time64_t delay)
{
      கட்டமைப்பு generic_event_s*cur = புதிய generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = பொய்;
      schedule_event_(cur, delay, SEQ_START);
}

/*
 * In the vvp runtime of Icarus Verilog, the SEQ_RWSYNC time step is
 * after all of the non-blocking assignments, so is effectively the
 * same as the ReadWriteSync time.
 */
வெறுமை schedule_at_end_of_simtime(vvp_gen_event_t obj, vvp_time64_t delay)
{
      கட்டமைப்பு generic_event_s*cur = புதிய generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = பொய்;
      schedule_event_(cur, delay, SEQ_RWSYNC);
}

நிலையான vvp_time64_t schedule_time;
vvp_time64_t schedule_simtime(வெறுமை)
{ திரும்பு schedule_time; }

நிலையான தருக்க sim_at_rosync = பொய்;
தருக்க schedule_at_rosync(வெறுமை)
{ திரும்பு sim_at_rosync; }

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
நிலையான வெறுமை run_rosync(கட்டமைப்பு event_time_s*ctim)
{
      sim_at_rosync = உண்மை;
      போது (ctim->rosync) {
	    கட்டமைப்பு event_s*cur = ctim->rosync->next;
	    எனில் (cur->next == cur) {
		  ctim->rosync = 0;
	    } இல்லையெனில் {
		  ctim->rosync->next = cur->next;
	    }

	    cur->run_run();
	    delete cur;
      }
      sim_at_rosync = பொய்;

      போது (ctim->del_thr) {
	    கட்டமைப்பு event_s*cur = ctim->del_thr->next;
	    எனில் (cur->next == cur) {
		  ctim->del_thr = 0;
	    } இல்லையெனில் {
		  ctim->del_thr->next = cur->next;
	    }

	    cur->run_run();
	    delete cur;
      }

      எனில் (ctim->active || ctim->inactive || ctim->nbassign || ctim->rwsync) {
	    cerr << "SCHEDULER ERROR: read-only sync events "
		 << "created RW events!" << endl;
      }
}

வெறுமை schedule_simulate(வெறுமை)
{
      தருக்க run_finals;
      sim_started = பொய்;

      schedule_time = 0;

      எனில் (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute EndOfCompile callbacks\n");
      }

      // Execute end of compile callbacks
      vpiEndOfCompile();

      எனில் (verbose_flag) {
	    vpi_mcd_printf(1, " ...propagate initialization events\n");
      }

	// Execute initialization events.
      போது (schedule_init_list) {
	    கட்டமைப்பு event_s*cur = schedule_init_list->next;
	    எனில் (cur->next == cur) {
		  schedule_init_list = 0;
	    } இல்லையெனில் {
		  schedule_init_list->next = cur->next;
	    }
	    cur->run_run();
	    delete cur;
      }

      sim_started = உண்மை;

      எனில் (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute StartOfSim callbacks\n");
      }

      // Execute start of simulation callbacks
      vpiStartOfSim();

      signals_capture();

      எனில் (verbose_flag) {
	    vpi_mcd_printf(1, " ...run scheduler\n");
      }

      // If there were no compiletf, etc. errors then we are going to
      // process events and when done run the final blocks.
      run_finals = schedule_runnable;

      எனில் (schedule_runnable) போது (sched_list) {

	    எனில் (schedule_stopped_flag) {
		  schedule_stopped_flag = பொய்;
		  stop_handler(0);
		  // You can finish from the debugger without a time change.
		  எனில் (!schedule_runnable) நிறுத்து;
		  தொடர்;
	    }

	      /* ctim is the current time step. */
	    கட்டமைப்பு event_time_s* ctim = sched_list;

	      /* If the time is advancing, then first run the
		 postponed sync events. Run them all. */
	    எனில் (ctim->delay > 0) {

		  எனில் (!schedule_runnable) நிறுத்து;
		  schedule_time += ctim->delay;
		    /* When the design is being traced (we are emitting
		     * file/line information) also print any time changes. */
		  எனில் (show_file_வரி) {
			cerr << "Advancing to simulation time: "
			     << schedule_time << endl;
		  }
		  ctim->delay = 0;

		  vpiNextSimTime();
		    // Process the cbAtStartOfSimTime callbacks.
		  போது (ctim->start) {
			கட்டமைப்பு event_s*cur = ctim->start->next;
			எனில் (cur->next == cur) {
			      ctim->start = 0;
			} இல்லையெனில் {
			      ctim->start->next = cur->next;
			}
			cur->run_run();
			delete (cur);
		  }
	    }


	      /* If there are no more active events, advance the event
		 queues. If there are not events at all, then release
		 the event_time object. */
	    எனில் (ctim->active == 0) {
		  ctim->active = ctim->inactive;
		  ctim->inactive = 0;

		  எனில் (ctim->active == 0) {
			ctim->active = ctim->nbassign;
			ctim->nbassign = 0;

			எனில் (ctim->active == 0) {
			      ctim->active = ctim->rwsync;
			      ctim->rwsync = 0;

				/* If out of rw events, then run the rosync
				   events and delete this time step. This also
				   deletes threads as needed. */
			      எனில் (ctim->active == 0) {
				    run_rosync(ctim);
				    sched_list = ctim->next;
				    delete ctim;
				    தொடர்;
			      }
			}
		  }
	    }

	      /* Pull the first item off the list. If this is the last
		 cell in the list, then clear the list. Execute that
		 event type, and delete it. */
	    கட்டமைப்பு event_s*cur = ctim->active->next;
	    எனில் (cur->next == cur) {
		  ctim->active = 0;
	    } இல்லையெனில் {
		  ctim->active->next = cur->next;
	    }

	    எனில் (schedule_single_step_flag) {
		  cur->single_step_display();
		  schedule_stopped_flag = உண்மை;
		  schedule_single_step_flag = பொய்;
	    }

	    cur->run_run();

	    delete (cur);
      }

	// Execute final events.
      schedule_runnable = run_finals;
      போது (schedule_runnable && schedule_final_list) {
	    கட்டமைப்பு event_s*cur = schedule_final_list->next;
	    எனில் (cur->next == cur) {
		  schedule_final_list = 0;
	    } இல்லையெனில் {
		  schedule_final_list->next = cur->next;
	    }
	    cur->run_run();
	    delete cur;
      }

      signals_revert();

      எனில் (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute Postsim callbacks\n");
      }

      // Execute post-simulation callbacks
      vpiPostsim();
#ifdef CHECK_WITH_VALGRIND
      schedule_delete();
#endif
}

#ifdef CHECK_WITH_VALGRIND
வெறுமை schedule_delete(வெறுமை)
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
