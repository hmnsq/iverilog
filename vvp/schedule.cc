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

using નામાકાશ std;

unsigned long ગણતરી_assign_events = 0;
unsigned long ગણતરી_gen_events = 0;
unsigned long ગણતરી_thread_events = 0;
  // Count the time events (A time cell created)
unsigned long ગણતરી_time_events = 0;



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
સંરચના event_s {
      સંરચના event_s*next;
      આભાસી ~event_s() { }
      આભાસી શૂન્ય run_run(શૂન્ય) =0;

	// Write something about the event to stderr
      આભાસી શૂન્ય single_step_display(શૂન્ય);

	// Fallback new/delete
      સ્થિર શૂન્ય*operator નવું (size_t size) { પાછા ::નવું અક્ષર[size]; }
      સ્થિર શૂન્ય operator delete(શૂન્ય*ptr)  { ::delete[]( static_cast<અક્ષર*>(ptr) ); }
};

શૂન્ય event_s::single_step_display(શૂન્ય)
{
      std::cerr << "event_s: Step into event " << typeid(*આ).name() << std::endl;
}

સંરચના event_time_s {
      event_time_s() {
	    ગણતરી_time_events += 1;
	    delay = 0;
	    start = 0;
	    active = 0;
	    inactive = 0;
	    nbassign = 0;
	    rwsync = 0;
	    rosync = 0;
	    del_thr = 0;
	    next = ખાલી;
      }
      vvp_time64_t delay;

      સંરચના event_s*start;
      સંરચના event_s*active;
      સંરચના event_s*inactive;
      સંરચના event_s*nbassign;
      સંરચના event_s*rwsync;
      સંરચના event_s*rosync;
      સંરચના event_s*del_thr;

      સંરચના event_time_s*next;

      સ્થિર શૂન્ય* operator નવું (size_t);
      સ્થિર શૂન્ય operator delete(શૂન્ય*obj, size_t s);
};

vvp_gen_event_s::~vvp_gen_event_s()
{
}

શૂન્ય vvp_gen_event_s::single_step_display(શૂન્ય)
{
      cerr << "vvp_gen_event_s: Step into event " << typeid(*આ).name() << endl;
}

/*
 * Derived event types
 */
સંરચના vthread_event_s : જાહેર event_s {
      vthread_t thr;
      શૂન્ય run_run(શૂન્ય) override;
      શૂન્ય single_step_display(શૂન્ય) override;

      સ્થિર શૂન્ય* operator નવું(size_t);
      સ્થિર શૂન્ય operator delete(શૂન્ય*);
};

શૂન્ય vthread_event_s::run_run(શૂન્ય)
{
      ગણતરી_thread_events += 1;
      vthread_run(thr);
}

શૂન્ય vthread_event_s::single_step_display(શૂન્ય)
{
      __vpiScope*scope = vthread_scope(thr);
      cerr << "vthread_event: Resume thread"
	   << " scope=" << scope->vpi_get_str(vpiFullName)
	   << endl;
}

સ્થિર અચલ size_t VTHR_CHUNK_ગણતરી = 8192 / કદ(સંરચના vthread_event_s);
સ્થિર slab_t<કદ(vthread_event_s),VTHR_CHUNK_ગણતરી> vthread_event_heap;

અંતઃરેખ શૂન્ય* vthread_event_s::operator નવું(size_t size)
{
      પ્રતિજ્ઞા(size == કદ(vthread_event_s));
      પાછા vthread_event_heap.alloc_slab();
}

શૂન્ય vthread_event_s::operator delete(શૂન્ય*dptr)
{
      vthread_event_heap.free_slab(dptr);
}

સંરચના del_thr_event_s : જાહેર event_s {
      vthread_t thr;
      શૂન્ય run_run(શૂન્ય) override;
      શૂન્ય single_step_display(શૂન્ય) override;
};

શૂન્ય del_thr_event_s::run_run(શૂન્ય)
{
      vthread_delete(thr);
}

શૂન્ય del_thr_event_s::single_step_display(શૂન્ય)
{
      __vpiScope*scope = vthread_scope(thr);
      cerr << "del_thr_event: Reap completed thread"
	   << " scope=" << scope->vpi_get_str(vpiFullName) << endl;
}

સંરચના assign_vector4_event_s  : જાહેર event_s {
	/* The default constructor. */
      explicit assign_vector4_event_s(અચલ vvp_vector4_t&that) : val(that) {
	    base = 0;
	    vwid = 0;
	    next = ખાલી;
      }

	/* Where to do the assign. */
      vvp_net_ptr_t ptr;
	/* Value to assign. */
      vvp_vector4_t val;
	/* Offset of the part into the destination. */
      unsigned base;
	/* Width of the destination vector. */
      unsigned vwid;
      શૂન્ય run_run(શૂન્ય) override;
      શૂન્ય single_step_display(શૂન્ય) override;

      સ્થિર શૂન્ય* operator નવું(size_t);
      સ્થિર શૂન્ય operator delete(શૂન્ય*);
};

શૂન્ય assign_vector4_event_s::run_run(શૂન્ય)
{
      ગણતરી_assign_events += 1;
      જો (vwid > 0)
	    vvp_send_vec4_pv(ptr, val, base, vwid, 0);
      નહીંતર
	    vvp_send_vec4(ptr, val, 0);
}

શૂન્ય assign_vector4_event_s::single_step_display(શૂન્ય)
{
      cerr << "assign_vector4_event: Propagate val=" << val
	   << ", vwid=" << vwid << ", base=" << base << endl;
}

સ્થિર અચલ size_t ASSIGN4_CHUNK_ગણતરી = 524288 / કદ(સંરચના assign_vector4_event_s);
સ્થિર slab_t<કદ(assign_vector4_event_s),ASSIGN4_CHUNK_ગણતરી> assign4_heap;

અંતઃરેખ શૂન્ય* assign_vector4_event_s::operator નવું(size_t size)
{
      પ્રતિજ્ઞા(size == કદ(assign_vector4_event_s));
      પાછા assign4_heap.alloc_slab();
}

શૂન્ય assign_vector4_event_s::operator delete(શૂન્ય*dptr)
{
      assign4_heap.free_slab(dptr);
}

unsigned long ગણતરી_assign4_pool(શૂન્ય) { પાછા assign4_heap.pool; }

સંરચના assign_vector8_event_s  : જાહેર event_s {
      vvp_net_ptr_t ptr;
      vvp_vector8_t val;
      શૂન્ય run_run(શૂન્ય) override;
      શૂન્ય single_step_display(શૂન્ય) override;

      સ્થિર શૂન્ય* operator નવું(size_t);
      સ્થિર શૂન્ય operator delete(શૂન્ય*);
};

શૂન્ય assign_vector8_event_s::run_run(શૂન્ય)
{
      ગણતરી_assign_events += 1;
      vvp_send_vec8(ptr, val);
}

શૂન્ય assign_vector8_event_s::single_step_display(શૂન્ય)
{
      cerr << "assign_vector8_event: Propagate val=" << val << endl;
}

સ્થિર અચલ size_t ASSIGN8_CHUNK_ગણતરી = 8192 / કદ(સંરચના assign_vector8_event_s);
સ્થિર slab_t<કદ(assign_vector8_event_s),ASSIGN8_CHUNK_ગણતરી> assign8_heap;

અંતઃરેખ શૂન્ય* assign_vector8_event_s::operator નવું(size_t size)
{
      પ્રતિજ્ઞા(size == કદ(assign_vector8_event_s));
      પાછા assign8_heap.alloc_slab();
}

શૂન્ય assign_vector8_event_s::operator delete(શૂન્ય*dptr)
{
      assign8_heap.free_slab(dptr);
}

unsigned long ગણતરી_assign8_pool() { પાછા assign8_heap.pool; }

સંરચના assign_real_event_s  : જાહેર event_s {
      vvp_net_ptr_t ptr;
      દ્વિગુણ val;
      શૂન્ય run_run(શૂન્ય) override;
      શૂન્ય single_step_display(શૂન્ય) override;

      સ્થિર શૂન્ય* operator નવું(size_t);
      સ્થિર શૂન્ય operator delete(શૂન્ય*);
};

શૂન્ય assign_real_event_s::run_run(શૂન્ય)
{
      ગણતરી_assign_events += 1;
      vvp_send_real(ptr, val, 0);
}

શૂન્ય assign_real_event_s::single_step_display(શૂન્ય)
{
      cerr << "assign_real_event: Propagate val=" << val << endl;
}

સ્થિર અચલ size_t ASSIGNR_CHUNK_ગણતરી = 8192 / કદ(સંરચના assign_real_event_s);
સ્થિર slab_t<કદ(assign_real_event_s),ASSIGNR_CHUNK_ગણતરી> assignr_heap;

અંતઃરેખ શૂન્ય* assign_real_event_s::operator નવું (size_t size)
{
      પ્રતિજ્ઞા(size == કદ(assign_real_event_s));
      પાછા assignr_heap.alloc_slab();
}

શૂન્ય assign_real_event_s::operator delete(શૂન્ય*dptr)
{
      assignr_heap.free_slab(dptr);
}

unsigned long ગણતરી_assign_real_pool(શૂન્ય) { પાછા assignr_heap.pool; }

સંરચના assign_array_word_s  : જાહેર event_s {
      vvp_array_t mem;
      unsigned adr;
      vvp_vector4_t val;
      unsigned off;
      શૂન્ય run_run(શૂન્ય) override;

      સ્થિર શૂન્ય* operator નવું(size_t);
      સ્થિર શૂન્ય operator delete(શૂન્ય*);
};

શૂન્ય assign_array_word_s::run_run(શૂન્ય)
{
      ગણતરી_assign_events += 1;
      mem->set_word(adr, off, val);
}

સ્થિર અચલ size_t ARRAY_W_CHUNK_ગણતરી = 8192 / કદ(સંરચના assign_array_word_s);
સ્થિર slab_t<કદ(assign_array_word_s),ARRAY_W_CHUNK_ગણતરી> array_w_heap;

અંતઃરેખ શૂન્ય* assign_array_word_s::operator નવું (size_t size)
{
      પ્રતિજ્ઞા(size == કદ(assign_array_word_s));
      પાછા array_w_heap.alloc_slab();
}

શૂન્ય assign_array_word_s::operator delete(શૂન્ય*ptr)
{
      array_w_heap.free_slab(ptr);
}

unsigned long ગણતરી_assign_aword_pool(શૂન્ય) { પાછા array_w_heap.pool; }

સંરચના force_vector4_event_s  : જાહેર event_s {
	/* The default constructor. */
      explicit force_vector4_event_s(અચલ vvp_vector4_t&that): val(that) {
	    net = ખાલી;
	    base = 0;
	    vwid = 0;
	    next = ખાલી;
      }
	/* Where to do the force. */
      vvp_net_t*net;
	/* Value to force. */
      vvp_vector4_t val;
	/* Offset of the part into the destination. */
      unsigned base;
	/* Width of the destination vector. */
      unsigned vwid;

      શૂન્ય run_run(શૂન્ય) override;
      શૂન્ય single_step_display(શૂન્ય) override;

      સ્થિર શૂન્ય* operator નવું(size_t);
      સ્થિર શૂન્ય operator delete(શૂન્ય*);
};

શૂન્ય force_vector4_event_s::run_run(શૂન્ય)
{
      ગણતરી_assign_events += 1;

      unsigned wid = val.size();
      જો ((base + wid) > vwid)
	    wid = vwid - base;

	// Make a mask of which bits are to be forced, 0 for unforced
	// bits and 1 for forced bits.
      vvp_vector2_t mask (vvp_vector2_t::FILL0, vwid);
      માટે (unsigned idx = 0 ; idx < wid ; idx += 1)
	    mask.set_bit(base+idx, 1);

      vvp_vector4_t tmp (vwid, BIT4_Z);

	// vvp_net_t::force_vec4 propagates all the bits of the
	// forced vector value, regardless of the mask. This
	// ensures the unforced bits retain their current value.
      vvp_signal_કિંમત*sig = dynamic_cast<vvp_signal_કિંમત*>(net->fil);
      પ્રતિજ્ઞા(sig);
      sig->vec4_કિંમત(tmp);

      tmp.set_vec(base, val);
      net->force_vec4(tmp, mask);
}

શૂન્ય force_vector4_event_s::single_step_display(શૂન્ય)
{
      cerr << "force_vector4_event: Force val=" << val
	   << ", vwid=" << vwid << ", base=" << base << endl;
}

સ્થિર અચલ size_t FORCE4_CHUNK_ગણતરી = 8192 / કદ(સંરચના force_vector4_event_s);
સ્થિર slab_t<કદ(force_vector4_event_s),FORCE4_CHUNK_ગણતરી> force4_heap;

અંતઃરેખ શૂન્ય* force_vector4_event_s::operator નવું(size_t size)
{
      પ્રતિજ્ઞા(size == કદ(force_vector4_event_s));
      પાછા force4_heap.alloc_slab();
}

શૂન્ય force_vector4_event_s::operator delete(શૂન્ય*dptr)
{
      force4_heap.free_slab(dptr);
}

unsigned long ગણતરી_force4_pool(શૂન્ય) { પાછા force4_heap.pool; }

/*
 * This class supports the propagation of vec4 outputs from a
 * vvp_net_t object.
 */
સંરચના propagate_vector4_event_s : જાહેર event_s {
	/* The default constructor. */
      explicit propagate_vector4_event_s(અચલ vvp_vector4_t&that) : val(that) {
	    net = ખાલી;
	    next = ખાલી;
      }
	/* A constructor that makes the val directly. */
      propagate_vector4_event_s(અચલ vvp_vector4_t&that, unsigned adr, unsigned wid)
      : val(that,adr,wid) {
	    net = ખાલી;
	    next = ખાલી;
      }

	/* Propagate the output of this net. */
      vvp_net_t*net;
	/* value to propagate */
      vvp_vector4_t val;
	/* Action */
      શૂન્ય run_run(શૂન્ય) override;
      શૂન્ય single_step_display(શૂન્ય) override;
};

શૂન્ય propagate_vector4_event_s::run_run(શૂન્ય)
{
      net->send_vec4(val, 0);
}

શૂન્ય propagate_vector4_event_s::single_step_display(શૂન્ય)
{
      cerr << "propagate_vector4_event: Propagate val=" << val << endl;
}

/*
 * This class supports the propagation of real outputs from a
 * vvp_net_t object.
 */
સંરચના propagate_real_event_s : જાહેર event_s {
	/* Propagate the output of this net. */
      vvp_net_t*net;
	/* value to propagate */
      દ્વિગુણ val;
	/* Action */
      શૂન્ય run_run(શૂન્ય) override;
      શૂન્ય single_step_display(શૂન્ય) override;
};

શૂન્ય propagate_real_event_s::run_run(શૂન્ય)
{
      net->send_real(val, 0);
}

શૂન્ય propagate_real_event_s::single_step_display(શૂન્ય)
{
      cerr << "propagate_real_event: Propagate val=" << val << endl;
}

સંરચના assign_array_r_word_s  : જાહેર event_s {
      vvp_array_t mem;
      unsigned adr;
      દ્વિગુણ val;
      શૂન્ય run_run(શૂન્ય) override;

      સ્થિર શૂન્ય* operator નવું(size_t);
      સ્થિર શૂન્ય operator delete(શૂન્ય*);
};

શૂન્ય assign_array_r_word_s::run_run(શૂન્ય)
{
      ગણતરી_assign_events += 1;
      mem->set_word(adr, val);
}
સ્થિર અચલ size_t ARRAY_R_W_CHUNK_ગણતરી = 8192 / કદ(સંરચના assign_array_r_word_s);
સ્થિર slab_t<કદ(assign_array_r_word_s),ARRAY_R_W_CHUNK_ગણતરી> array_r_w_heap;

અંતઃરેખ શૂન્ય* assign_array_r_word_s::operator નવું(size_t size)
{
      પ્રતિજ્ઞા(size == કદ(assign_array_r_word_s));
      પાછા array_r_w_heap.alloc_slab();
}

શૂન્ય assign_array_r_word_s::operator delete(શૂન્ય*ptr)
{
      array_r_w_heap.free_slab(ptr);
}

unsigned long ગણતરી_assign_arword_pool(શૂન્ય) { પાછા array_r_w_heap.pool; }

સંરચના generic_event_s : જાહેર event_s {
      vvp_gen_event_t obj;
      તાર્કિક delete_obj_when_done;
      શૂન્ય run_run(શૂન્ય) override;
      શૂન્ય single_step_display(શૂન્ય) override;

      સ્થિર શૂન્ય* operator નવું(size_t);
      સ્થિર શૂન્ય operator delete(શૂન્ય*);
};

શૂન્ય generic_event_s::run_run(શૂન્ય)
{
      ગણતરી_gen_events += 1;
      જો (obj) {
	    obj->run_run();
	    જો (delete_obj_when_done)
		  delete obj;
      }
}

શૂન્ય generic_event_s::single_step_display(શૂન્ય)
{
      obj->single_step_display();
}

સ્થિર અચલ size_t GENERIC_CHUNK_ગણતરી = 131072 / કદ(સંરચના generic_event_s);
સ્થિર slab_t<કદ(generic_event_s),GENERIC_CHUNK_ગણતરી> generic_event_heap;

અંતઃરેખ શૂન્ય* generic_event_s::operator નવું(size_t size)
{
      પ્રતિજ્ઞા(size == કદ(generic_event_s));
      પાછા generic_event_heap.alloc_slab();
}

શૂન્ય generic_event_s::operator delete(શૂન્ય*ptr)
{
      generic_event_heap.free_slab(ptr);
}

unsigned long ગણતરી_gen_pool(શૂન્ય) { પાછા generic_event_heap.pool; }

/*
** These event_time_s will be required a lot, at high frequency.
** Once allocated, we never free them, but stash them away for next time.
*/


સ્થિર અચલ size_t TIME_CHUNK_ગણતરી = 8192 / કદ(સંરચના event_time_s);
સ્થિર slab_t<કદ(event_time_s),TIME_CHUNK_ગણતરી> event_time_heap;

અંતઃરેખ શૂન્ય* event_time_s::operator નવું (size_t size)
{
      પ્રતિજ્ઞા(size == કદ(સંરચના event_time_s));
      શૂન્ય*ptr = event_time_heap.alloc_slab();
      પાછા ptr;
}

અંતઃરેખ શૂન્ય event_time_s::operator delete(શૂન્ય*ptr, size_t)
{
      event_time_heap.free_slab(ptr);
}

unsigned long ગણતરી_time_pool(શૂન્ય) { પાછા event_time_heap.pool; }

/*
 * This is the head of the list of pending events. This includes all
 * the events that have not been executed yet, and reaches into the
 * future.
 */
સ્થિર સંરચના event_time_s* sched_list = 0;

/*
 * This is a list of initialization events. The setup puts
 * initializations in this list so that they happen before the
 * simulation as a whole starts. This prevents time-0 triggers of
 * certain events.
 */
સ્થિર સંરચના event_s* schedule_init_list = 0;

/*
 * This is the head of the list of final events.
 */
સ્થિર સંરચના event_s* schedule_final_list = 0;

/*
 * This flag is true until a VPI task or function finishes the
 * simulation.
 */
સ્થિર તાર્કિક schedule_runnable = સાચું;
સ્થિર તાર્કિક schedule_stopped_flag  = ખોટું;
સ્થિર તાર્કિક schedule_single_step_flag = ખોટું;
સ્થિર તાર્કિક no_signals_flag = ખોટું;

શૂન્ય schedule_finish(પૂર્ણાંક)
{
      schedule_runnable = ખોટું;
}

શૂન્ય schedule_stop(પૂર્ણાંક)
{
      schedule_stopped_flag = સાચું;
}

શૂન્ય schedule_single_step(પૂર્ણાંક)
{
      schedule_single_step_flag = સાચું;
}

તાર્કિક schedule_finished(શૂન્ય)
{
      પાછા !schedule_runnable;
}

તાર્કિક schedule_stopped(શૂન્ય)
{
      પાછા schedule_stopped_flag;
}

બાહ્ય "C" શૂન્ય vvp_no_signals(શૂન્ય)
{
      no_signals_flag = સાચું;
}

/*
 * These are the signal handling infrastructure. The SIGINT signal
 * leads to an implicit $stop. The SIGHUP and SIGTERM signals lead
 * to an implicit $finish.
 */
બાહ્ય તાર્કિક stop_is_finish;

બાહ્ય "C" શૂન્ય signals_handler(પૂર્ણાંક signum)
{
#ifdef __MINGW32__
	// Windows implements the original UNIX semantics for signal,
	// so we have to re-establish the signal handler each time a
	// signal is caught.
      સંકેત(signum, &signals_handler);
#endif
      જો (signum != SIGINT)
	    stop_is_finish = સાચું;
      schedule_stopped_flag = સાચું;
}

સ્થિર શૂન્ય signals_capture(શૂન્ય)
{
      જો (no_signals_flag)
	   પાછા;
#ifndef __MINGW32__
      સંકેત(SIGHUP,  &signals_handler);
#endif
      સંકેત(SIGINT,  &signals_handler);
      સંકેત(SIGTERM, &signals_handler);
}

સ્થિર શૂન્ય signals_revert(શૂન્ય)
{
      જો (no_signals_flag)
	   પાછા;
#ifndef __MINGW32__
      સંકેત(SIGHUP,  SIG_DFL);
#endif
      સંકેત(SIGINT,  SIG_DFL);
      સંકેત(SIGTERM, SIG_DFL);
}

/*
 * This function puts an event on the end of the pre-simulation event queue.
 */
સ્થિર શૂન્ય schedule_init_event(સંરચના event_s*cur)
{
      જો (schedule_init_list == 0) {
            cur->next = cur;
      } નહીંતર {
            cur->next = schedule_init_list->next;
            schedule_init_list->next = cur;
      }
      schedule_init_list = cur;
}

/*
 * This function puts an event on the end of the post-simulation event queue.
 */
સ્થિર શૂન્ય schedule_final_event(સંરચના event_s*cur)
{
      જો (schedule_final_list == 0) {
            cur->next = cur;
      } નહીંતર {
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
પ્રકારવ્યાખ્યા ગણના event_queue_e { SEQ_START, SEQ_ACTIVE, SEQ_INACTIVE, SEQ_NBASSIGN,
			     SEQ_RWSYNC, SEQ_ROSYNC, DEL_THREAD } event_queue_t;

સ્થિર શૂન્ય schedule_event_(સંરચના event_s*cur, vvp_time64_t delay,
			    event_queue_t select_queue)
{
      cur->next = cur;
      સંરચના event_time_s*ctim = sched_list;

      જો (sched_list == 0) {
	      /* Is the event_time list completely empty? Create the
		 first event_time object. */
	    ctim = નવું સંરચના event_time_s;
	    ctim->delay = delay;
	    ctim->next  = 0;
	    sched_list = ctim;

      } નહીંતર જો (sched_list->delay > delay) {

	      /* Am I looking for an event before the first event_time?
		 If so, create a new event_time to go in front. */
	    સંરચના event_time_s*tmp = નવું સંરચના event_time_s;
	    tmp->delay = delay;
	    tmp->next = ctim;
	    ctim->delay -= delay;
	    ctim = tmp;
	    sched_list = ctim;

      } નહીંતર {
	    સંરચના event_time_s*prev = 0;

	    જ્યારે (ctim->next && (ctim->delay < delay)) {
		  delay -= ctim->delay;
		  prev = ctim;
		  ctim = ctim->next;
	    }

	    જો (ctim->delay > delay) {
		  સંરચના event_time_s*tmp = નવું સંરચના event_time_s;
		  tmp->delay = delay;
		  tmp->next  = prev->next;
		  prev->next = tmp;

		  tmp->next->delay -= delay;
		  ctim = tmp;

	    } નહીંતર જો (ctim->delay == delay) {

	    } નહીંતર {
		  પ્રતિજ્ઞા(ctim->next == 0);
		  સંરચના event_time_s*tmp = નવું સંરચના event_time_s;
		  tmp->delay = delay - ctim->delay;
		  tmp->next = 0;
		  ctim->next = tmp;

		  ctim = tmp;
	    }
      }

	/* By this point, ctim is the event_time structure that is to
	   receive the event at hand. Put the event in to the
	   appropriate list for the kind of assign we have at hand. */

      સંરચના event_s** q = 0;

      પસંદગી (select_queue) {
	  સ્થિતિ SEQ_START:
	    q = &ctim->start;
	    તોડો;

	  સ્થિતિ SEQ_ACTIVE:
	    q = &ctim->active;
	    તોડો;

	  સ્થિતિ SEQ_INACTIVE:
	    પ્રતિજ્ઞા(delay == 0);
	    q = &ctim->inactive;
	    તોડો;

	  સ્થિતિ SEQ_NBASSIGN:
	    q = &ctim->nbassign;
	    તોડો;

	  સ્થિતિ SEQ_RWSYNC:
	    q = &ctim->rwsync;
	    તોડો;

	  સ્થિતિ SEQ_ROSYNC:
	    q = &ctim->rosync;
	    તોડો;

	  સ્થિતિ DEL_THREAD:
	    q = &ctim->del_thr;
	    તોડો;
      }

      જો (q) {
	    જો (*q) {
		  /* Put the cur event on the end of the queue. */
		  cur->next = (*q)->next;
		  (*q)->next = cur;
	    }
	    *q = cur;
      }
}

સ્થિર શૂન્ય schedule_event_ધકેલો(સંરચના event_s*cur)
{
      જો ((sched_list == 0) || (sched_list->delay > 0)) {
	    schedule_event_(cur, 0, SEQ_ACTIVE);
	    પાછા;
      }

      સંરચના event_time_s*ctim = sched_list;

      જો (ctim->active == 0) {
	    cur->next = cur;
	    ctim->active = cur;
	    પાછા;
      }

      cur->next = ctim->active->next;
      ctim->active->next = cur;
}

શૂન્ય schedule_vthread(vthread_t thr, vvp_time64_t delay, તાર્કિક ધકેલો_flag)
{
      સંરચના vthread_event_s*cur = નવું vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);

      જો (ધકેલો_flag && (delay == 0)) {
	      /* Special case: If the delay is 0, the push_flag means
		 I can push this event in front of everything. This is
		 used by the %fork statement, for example, to perform
		 task calls. */
	    schedule_event_ધકેલો(cur);

      } નહીંતર {
	    schedule_event_(cur, delay, SEQ_ACTIVE);
      }
}

શૂન્ય schedule_t0_trigger(vvp_net_ptr_t ptr)
{
      vvp_vector4_t bit (1, BIT4_X);
      સંરચના assign_vector4_event_s*cur = નવું સંરચના assign_vector4_event_s(bit);
      cur->ptr = ptr;
      schedule_event_(cur, 0, SEQ_INACTIVE);
}

શૂન્ય schedule_inactive(vthread_t thr)
{
      સંરચના vthread_event_s*cur = નવું vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);
      schedule_event_(cur, 0, SEQ_INACTIVE);
}

શૂન્ય schedule_init_vthread(vthread_t thr)
{
      સંરચના vthread_event_s*cur = નવું vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);

      schedule_init_event(cur);
}

શૂન્ય schedule_final_vthread(vthread_t thr)
{
      સંરચના vthread_event_s*cur = નવું vthread_event_s;

      cur->thr = thr;
      vthread_mark_final(thr);
      vthread_mark_scheduled(thr);

      schedule_final_event(cur);
}

શૂન્ય schedule_assign_vector(vvp_net_ptr_t ptr,
			    unsigned base, unsigned vwid,
			    અચલ vvp_vector4_t&bit,
			    vvp_time64_t delay)
{
      સંરચના assign_vector4_event_s*cur = નવું સંરચના assign_vector4_event_s(bit);
      cur->ptr = ptr;
      cur->base = base;
      cur->vwid = vwid;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

શૂન્ય schedule_force_vector(vvp_net_t*net,
			    unsigned base, unsigned vwid,
			    અચલ vvp_vector4_t&bit,
			    vvp_time64_t delay)
{
      સંરચના force_vector4_event_s*cur = નવું સંરચના force_vector4_event_s(bit);
      cur->net = net;
      cur->base = base;
      cur->vwid = vwid;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

શૂન્ય schedule_propagate_vector(vvp_net_t*net,
			       vvp_time64_t delay,
			       અચલ vvp_vector4_t&src)
{
      સંરચના propagate_vector4_event_s*cur
	    = નવું સંરચના propagate_vector4_event_s(src);
      cur->net = net;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

// FIXME: This needs to create a non-blocking event, but only one per time slot.
//        Is schedule_event_ or execution actually filtering since the net is
//        already X because this is not triggering?
શૂન્ય schedule_propagate_event(vvp_net_t*net,
                              vvp_time64_t delay)
{
      vvp_vector4_t tmp (1, BIT4_X);
      સંરચના propagate_vector4_event_s*cur
	    = નવું સંરચના propagate_vector4_event_s(tmp);
      cur->net = net;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

શૂન્ય schedule_assign_array_word(vvp_array_t mem,
				unsigned word_addr,
				unsigned off,
				અચલ vvp_vector4_t&val,
				vvp_time64_t delay)
{
      સંરચના assign_array_word_s*cur = નવું સંરચના assign_array_word_s;
      cur->mem = mem;
      cur->adr = word_addr;
      cur->off = off;
      cur->val = val;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

શૂન્ય schedule_assign_array_word(vvp_array_t mem,
				unsigned word_addr,
				દ્વિગુણ val,
				vvp_time64_t delay)
{
      સંરચના assign_array_r_word_s*cur = નવું સંરચના assign_array_r_word_s;
      cur->mem = mem;
      cur->adr = word_addr;
      cur->val = val;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

શૂન્ય schedule_set_vector(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit)
{
      સંરચના assign_vector4_event_s*cur = નવું સંરચના assign_vector4_event_s(bit);
      cur->ptr = ptr;
      cur->base = 0;
      cur->vwid = 0;
      schedule_event_(cur, 0, SEQ_ACTIVE);
}

શૂન્ય schedule_set_vector(vvp_net_ptr_t ptr, અચલ vvp_vector8_t&bit)
{
      સંરચના assign_vector8_event_s*cur = નવું સંરચના assign_vector8_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_event_(cur, 0, SEQ_ACTIVE);
}

શૂન્ય schedule_set_vector(vvp_net_ptr_t ptr, દ્વિગુણ bit)
{
      સંરચના assign_real_event_s*cur = નવું સંરચના assign_real_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_event_(cur, 0, SEQ_ACTIVE);
}

શૂન્ય schedule_init_vector(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit)
{
      સંરચના assign_vector4_event_s*cur = નવું સંરચના assign_vector4_event_s(bit);
      cur->ptr = ptr;
      cur->base = 0;
      cur->vwid = 0;
      schedule_init_event(cur);
}

શૂન્ય schedule_init_vector(vvp_net_ptr_t ptr, અચલ vvp_vector8_t&bit)
{
      સંરચના assign_vector8_event_s*cur = નવું સંરચના assign_vector8_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_init_event(cur);
}

શૂન્ય schedule_init_vector(vvp_net_ptr_t ptr, દ્વિગુણ bit)
{
      સંરચના assign_real_event_s*cur = નવું સંરચના assign_real_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_init_event(cur);
}

શૂન્ય schedule_init_propagate(vvp_net_t*net, vvp_vector4_t bit)
{
      સંરચના propagate_vector4_event_s*cur = નવું સંરચના propagate_vector4_event_s(bit);
      cur->net = net;
      schedule_init_event(cur);
}

શૂન્ય schedule_init_propagate(vvp_net_t*net, દ્વિગુણ bit)
{
      સંરચના propagate_real_event_s*cur = નવું સંરચના propagate_real_event_s;
      cur->net = net;
      cur->val = bit;
      schedule_init_event(cur);
}

શૂન્ય schedule_del_thr(vthread_t thr)
{
      સંરચના del_thr_event_s*cur = નવું del_thr_event_s;

      cur->thr = thr;

      schedule_event_(cur, 0, DEL_THREAD);
}

શૂન્ય schedule_generic(vvp_gen_event_t obj, vvp_time64_t delay,
		      તાર્કિક sync_flag, તાર્કિક ro_flag, તાર્કિક delete_when_done)
{
      સંરચના generic_event_s*cur = નવું generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = delete_when_done;
      schedule_event_(cur, delay,
		      sync_flag? (ro_flag?SEQ_ROSYNC:SEQ_RWSYNC) : SEQ_ACTIVE);

      જો (sync_flag)
	    vthread_delay_delete();
}

સ્થિર તાર્કિક sim_started;

શૂન્ય schedule_functor(vvp_gen_event_t obj)
{
      સંરચના generic_event_s*cur = નવું generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = ખોટું;
      જો (!sim_started) {
            schedule_init_event(cur);
      } નહીંતર {
            schedule_event_(cur, 0, SEQ_ACTIVE);
      }
}

શૂન્ય schedule_at_start_of_simtime(vvp_gen_event_t obj, vvp_time64_t delay)
{
      સંરચના generic_event_s*cur = નવું generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = ખોટું;
      schedule_event_(cur, delay, SEQ_START);
}

/*
 * In the vvp runtime of Icarus Verilog, the SEQ_RWSYNC time step is
 * after all of the non-blocking assignments, so is effectively the
 * same as the ReadWriteSync time.
 */
શૂન્ય schedule_at_end_of_simtime(vvp_gen_event_t obj, vvp_time64_t delay)
{
      સંરચના generic_event_s*cur = નવું generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = ખોટું;
      schedule_event_(cur, delay, SEQ_RWSYNC);
}

સ્થિર vvp_time64_t schedule_time;
vvp_time64_t schedule_simtime(શૂન્ય)
{ પાછા schedule_time; }

સ્થિર તાર્કિક sim_at_rosync = ખોટું;
તાર્કિક schedule_at_rosync(શૂન્ય)
{ પાછા sim_at_rosync; }

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
સ્થિર શૂન્ય run_rosync(સંરચના event_time_s*ctim)
{
      sim_at_rosync = સાચું;
      જ્યારે (ctim->rosync) {
	    સંરચના event_s*cur = ctim->rosync->next;
	    જો (cur->next == cur) {
		  ctim->rosync = 0;
	    } નહીંતર {
		  ctim->rosync->next = cur->next;
	    }

	    cur->run_run();
	    delete cur;
      }
      sim_at_rosync = ખોટું;

      જ્યારે (ctim->del_thr) {
	    સંરચના event_s*cur = ctim->del_thr->next;
	    જો (cur->next == cur) {
		  ctim->del_thr = 0;
	    } નહીંતર {
		  ctim->del_thr->next = cur->next;
	    }

	    cur->run_run();
	    delete cur;
      }

      જો (ctim->active || ctim->inactive || ctim->nbassign || ctim->rwsync) {
	    cerr << "SCHEDULER ERROR: read-only sync events "
		 << "created RW events!" << endl;
      }
}

શૂન્ય schedule_simulate(શૂન્ય)
{
      તાર્કિક run_finals;
      sim_started = ખોટું;

      schedule_time = 0;

      જો (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute EndOfCompile callbacks\n");
      }

      // Execute end of compile callbacks
      vpiEndOfCompile();

      જો (verbose_flag) {
	    vpi_mcd_printf(1, " ...propagate initialization events\n");
      }

	// Execute initialization events.
      જ્યારે (schedule_init_list) {
	    સંરચના event_s*cur = schedule_init_list->next;
	    જો (cur->next == cur) {
		  schedule_init_list = 0;
	    } નહીંતર {
		  schedule_init_list->next = cur->next;
	    }
	    cur->run_run();
	    delete cur;
      }

      sim_started = સાચું;

      જો (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute StartOfSim callbacks\n");
      }

      // Execute start of simulation callbacks
      vpiStartOfSim();

      signals_capture();

      જો (verbose_flag) {
	    vpi_mcd_printf(1, " ...run scheduler\n");
      }

      // If there were no compiletf, etc. errors then we are going to
      // process events and when done run the final blocks.
      run_finals = schedule_runnable;

      જો (schedule_runnable) જ્યારે (sched_list) {

	    જો (schedule_stopped_flag) {
		  schedule_stopped_flag = ખોટું;
		  stop_handler(0);
		  // You can finish from the debugger without a time change.
		  જો (!schedule_runnable) તોડો;
		  ચાલુ;
	    }

	      /* ctim is the current time step. */
	    સંરચના event_time_s* ctim = sched_list;

	      /* If the time is advancing, then first run the
		 postponed sync events. Run them all. */
	    જો (ctim->delay > 0) {

		  જો (!schedule_runnable) તોડો;
		  schedule_time += ctim->delay;
		    /* When the design is being traced (we are emitting
		     * file/line information) also print any time changes. */
		  જો (show_file_લીટી) {
			cerr << "Advancing to simulation time: "
			     << schedule_time << endl;
		  }
		  ctim->delay = 0;

		  vpiNextSimTime();
		    // Process the cbAtStartOfSimTime callbacks.
		  જ્યારે (ctim->start) {
			સંરચના event_s*cur = ctim->start->next;
			જો (cur->next == cur) {
			      ctim->start = 0;
			} નહીંતર {
			      ctim->start->next = cur->next;
			}
			cur->run_run();
			delete (cur);
		  }
	    }


	      /* If there are no more active events, advance the event
		 queues. If there are not events at all, then release
		 the event_time object. */
	    જો (ctim->active == 0) {
		  ctim->active = ctim->inactive;
		  ctim->inactive = 0;

		  જો (ctim->active == 0) {
			ctim->active = ctim->nbassign;
			ctim->nbassign = 0;

			જો (ctim->active == 0) {
			      ctim->active = ctim->rwsync;
			      ctim->rwsync = 0;

				/* If out of rw events, then run the rosync
				   events and delete this time step. This also
				   deletes threads as needed. */
			      જો (ctim->active == 0) {
				    run_rosync(ctim);
				    sched_list = ctim->next;
				    delete ctim;
				    ચાલુ;
			      }
			}
		  }
	    }

	      /* Pull the first item off the list. If this is the last
		 cell in the list, then clear the list. Execute that
		 event type, and delete it. */
	    સંરચના event_s*cur = ctim->active->next;
	    જો (cur->next == cur) {
		  ctim->active = 0;
	    } નહીંતર {
		  ctim->active->next = cur->next;
	    }

	    જો (schedule_single_step_flag) {
		  cur->single_step_display();
		  schedule_stopped_flag = સાચું;
		  schedule_single_step_flag = ખોટું;
	    }

	    cur->run_run();

	    delete (cur);
      }

	// Execute final events.
      schedule_runnable = run_finals;
      જ્યારે (schedule_runnable && schedule_final_list) {
	    સંરચના event_s*cur = schedule_final_list->next;
	    જો (cur->next == cur) {
		  schedule_final_list = 0;
	    } નહીંતર {
		  schedule_final_list->next = cur->next;
	    }
	    cur->run_run();
	    delete cur;
      }

      signals_revert();

      જો (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute Postsim callbacks\n");
      }

      // Execute post-simulation callbacks
      vpiPostsim();
#ifdef CHECK_WITH_VALGRIND
      schedule_delete();
#endif
}

#ifdef CHECK_WITH_VALGRIND
શૂન્ય schedule_delete(શૂન્ય)
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
