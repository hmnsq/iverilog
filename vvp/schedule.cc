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

using نطاق std;

unsigned long گنتی_assign_events = 0;
unsigned long گنتی_gen_events = 0;
unsigned long گنتی_thread_events = 0;
  // Count the time events (A time cell created)
unsigned long گنتی_time_events = 0;



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
بنية event_s {
      بنية event_s*next;
      virtual ~event_s() { }
      virtual فراغ run_run(فراغ) =0;

	// Write something about the event to stderr
      virtual فراغ single_step_display(فراغ);

	// Fallback new/delete
      ساكن فراغ*operator جديد (size_t size) { أرجع ::جديد حرف[size]; }
      ساكن فراغ operator delete(فراغ*ptr)  { ::delete[]( static_cast<حرف*>(ptr) ); }
};

فراغ event_s::single_step_display(فراغ)
{
      std::cerr << "event_s: Step into event " << typeid(*هذا).name() << std::endl;
}

بنية event_time_s {
      event_time_s() {
	    گنتی_time_events += 1;
	    delay = 0;
	    start = 0;
	    active = 0;
	    inactive = 0;
	    nbassign = 0;
	    rwsync = 0;
	    rosync = 0;
	    del_thr = 0;
	    next = عدم;
      }
      vvp_time64_t delay;

      بنية event_s*start;
      بنية event_s*active;
      بنية event_s*inactive;
      بنية event_s*nbassign;
      بنية event_s*rwsync;
      بنية event_s*rosync;
      بنية event_s*del_thr;

      بنية event_time_s*next;

      ساكن فراغ* operator جديد (size_t);
      ساكن فراغ operator delete(فراغ*obj, size_t s);
};

vvp_gen_event_s::~vvp_gen_event_s()
{
}

فراغ vvp_gen_event_s::single_step_display(فراغ)
{
      cerr << "vvp_gen_event_s: Step into event " << typeid(*هذا).name() << endl;
}

/*
 * Derived event types
 */
بنية vthread_event_s : عام event_s {
      vthread_t thr;
      فراغ run_run(فراغ) override;
      فراغ single_step_display(فراغ) override;

      ساكن فراغ* operator جديد(size_t);
      ساكن فراغ operator delete(فراغ*);
};

فراغ vthread_event_s::run_run(فراغ)
{
      گنتی_thread_events += 1;
      vthread_run(thr);
}

فراغ vthread_event_s::single_step_display(فراغ)
{
      __vpiScope*scope = vthread_scope(thr);
      cerr << "vthread_event: Resume thread"
	   << " scope=" << scope->vpi_get_str(vpiFullName)
	   << endl;
}

ساكن ثابت size_t VTHR_CHUNK_گنتی = 8192 / حجم(بنية vthread_event_s);
ساكن slab_t<حجم(vthread_event_s),VTHR_CHUNK_گنتی> vthread_event_heap;

مضمن فراغ* vthread_event_s::operator جديد(size_t size)
{
      أكد(size == حجم(vthread_event_s));
      أرجع vthread_event_heap.alloc_slab();
}

فراغ vthread_event_s::operator delete(فراغ*dptr)
{
      vthread_event_heap.free_slab(dptr);
}

بنية del_thr_event_s : عام event_s {
      vthread_t thr;
      فراغ run_run(فراغ) override;
      فراغ single_step_display(فراغ) override;
};

فراغ del_thr_event_s::run_run(فراغ)
{
      vthread_delete(thr);
}

فراغ del_thr_event_s::single_step_display(فراغ)
{
      __vpiScope*scope = vthread_scope(thr);
      cerr << "del_thr_event: Reap completed thread"
	   << " scope=" << scope->vpi_get_str(vpiFullName) << endl;
}

بنية assign_vector4_event_s  : عام event_s {
	/* The default constructor. */
      explicit assign_vector4_event_s(ثابت vvp_vector4_t&that) : val(that) {
	    base = 0;
	    vwid = 0;
	    next = عدم;
      }

	/* Where to do the assign. */
      vvp_net_ptr_t ptr;
	/* Value to assign. */
      vvp_vector4_t val;
	/* Offset of the part into the destination. */
      unsigned base;
	/* Width of the destination vector. */
      unsigned vwid;
      فراغ run_run(فراغ) override;
      فراغ single_step_display(فراغ) override;

      ساكن فراغ* operator جديد(size_t);
      ساكن فراغ operator delete(فراغ*);
};

فراغ assign_vector4_event_s::run_run(فراغ)
{
      گنتی_assign_events += 1;
      إذا (vwid > 0)
	    vvp_send_vec4_pv(ptr, val, base, vwid, 0);
      وإلا
	    vvp_send_vec4(ptr, val, 0);
}

فراغ assign_vector4_event_s::single_step_display(فراغ)
{
      cerr << "assign_vector4_event: Propagate val=" << val
	   << ", vwid=" << vwid << ", base=" << base << endl;
}

ساكن ثابت size_t ASSIGN4_CHUNK_گنتی = 524288 / حجم(بنية assign_vector4_event_s);
ساكن slab_t<حجم(assign_vector4_event_s),ASSIGN4_CHUNK_گنتی> assign4_heap;

مضمن فراغ* assign_vector4_event_s::operator جديد(size_t size)
{
      أكد(size == حجم(assign_vector4_event_s));
      أرجع assign4_heap.alloc_slab();
}

فراغ assign_vector4_event_s::operator delete(فراغ*dptr)
{
      assign4_heap.free_slab(dptr);
}

unsigned long گنتی_assign4_pool(فراغ) { أرجع assign4_heap.pool; }

بنية assign_vector8_event_s  : عام event_s {
      vvp_net_ptr_t ptr;
      vvp_vector8_t val;
      فراغ run_run(فراغ) override;
      فراغ single_step_display(فراغ) override;

      ساكن فراغ* operator جديد(size_t);
      ساكن فراغ operator delete(فراغ*);
};

فراغ assign_vector8_event_s::run_run(فراغ)
{
      گنتی_assign_events += 1;
      vvp_send_vec8(ptr, val);
}

فراغ assign_vector8_event_s::single_step_display(فراغ)
{
      cerr << "assign_vector8_event: Propagate val=" << val << endl;
}

ساكن ثابت size_t ASSIGN8_CHUNK_گنتی = 8192 / حجم(بنية assign_vector8_event_s);
ساكن slab_t<حجم(assign_vector8_event_s),ASSIGN8_CHUNK_گنتی> assign8_heap;

مضمن فراغ* assign_vector8_event_s::operator جديد(size_t size)
{
      أكد(size == حجم(assign_vector8_event_s));
      أرجع assign8_heap.alloc_slab();
}

فراغ assign_vector8_event_s::operator delete(فراغ*dptr)
{
      assign8_heap.free_slab(dptr);
}

unsigned long گنتی_assign8_pool() { أرجع assign8_heap.pool; }

بنية assign_real_event_s  : عام event_s {
      vvp_net_ptr_t ptr;
      مزدوج val;
      فراغ run_run(فراغ) override;
      فراغ single_step_display(فراغ) override;

      ساكن فراغ* operator جديد(size_t);
      ساكن فراغ operator delete(فراغ*);
};

فراغ assign_real_event_s::run_run(فراغ)
{
      گنتی_assign_events += 1;
      vvp_send_real(ptr, val, 0);
}

فراغ assign_real_event_s::single_step_display(فراغ)
{
      cerr << "assign_real_event: Propagate val=" << val << endl;
}

ساكن ثابت size_t ASSIGNR_CHUNK_گنتی = 8192 / حجم(بنية assign_real_event_s);
ساكن slab_t<حجم(assign_real_event_s),ASSIGNR_CHUNK_گنتی> assignr_heap;

مضمن فراغ* assign_real_event_s::operator جديد (size_t size)
{
      أكد(size == حجم(assign_real_event_s));
      أرجع assignr_heap.alloc_slab();
}

فراغ assign_real_event_s::operator delete(فراغ*dptr)
{
      assignr_heap.free_slab(dptr);
}

unsigned long گنتی_assign_real_pool(فراغ) { أرجع assignr_heap.pool; }

بنية assign_array_word_s  : عام event_s {
      vvp_array_t mem;
      unsigned adr;
      vvp_vector4_t val;
      unsigned off;
      فراغ run_run(فراغ) override;

      ساكن فراغ* operator جديد(size_t);
      ساكن فراغ operator delete(فراغ*);
};

فراغ assign_array_word_s::run_run(فراغ)
{
      گنتی_assign_events += 1;
      mem->set_word(adr, off, val);
}

ساكن ثابت size_t ARRAY_W_CHUNK_گنتی = 8192 / حجم(بنية assign_array_word_s);
ساكن slab_t<حجم(assign_array_word_s),ARRAY_W_CHUNK_گنتی> array_w_heap;

مضمن فراغ* assign_array_word_s::operator جديد (size_t size)
{
      أكد(size == حجم(assign_array_word_s));
      أرجع array_w_heap.alloc_slab();
}

فراغ assign_array_word_s::operator delete(فراغ*ptr)
{
      array_w_heap.free_slab(ptr);
}

unsigned long گنتی_assign_aword_pool(فراغ) { أرجع array_w_heap.pool; }

بنية force_vector4_event_s  : عام event_s {
	/* The default constructor. */
      explicit force_vector4_event_s(ثابت vvp_vector4_t&that): val(that) {
	    net = عدم;
	    base = 0;
	    vwid = 0;
	    next = عدم;
      }
	/* Where to do the force. */
      vvp_net_t*net;
	/* Value to force. */
      vvp_vector4_t val;
	/* Offset of the part into the destination. */
      unsigned base;
	/* Width of the destination vector. */
      unsigned vwid;

      فراغ run_run(فراغ) override;
      فراغ single_step_display(فراغ) override;

      ساكن فراغ* operator جديد(size_t);
      ساكن فراغ operator delete(فراغ*);
};

فراغ force_vector4_event_s::run_run(فراغ)
{
      گنتی_assign_events += 1;

      unsigned wid = val.size();
      إذا ((base + wid) > vwid)
	    wid = vwid - base;

	// Make a mask of which bits are to be forced, 0 for unforced
	// bits and 1 for forced bits.
      vvp_vector2_t mask (vvp_vector2_t::FILL0, vwid);
      لكل (unsigned idx = 0 ; idx < wid ; idx += 1)
	    mask.set_bit(base+idx, 1);

      vvp_vector4_t tmp (vwid, BIT4_Z);

	// vvp_net_t::force_vec4 propagates all the bits of the
	// forced vector value, regardless of the mask. This
	// ensures the unforced bits retain their current value.
      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*>(net->fil);
      أكد(sig);
      sig->vec4_قدر(tmp);

      tmp.set_vec(base, val);
      net->force_vec4(tmp, mask);
}

فراغ force_vector4_event_s::single_step_display(فراغ)
{
      cerr << "force_vector4_event: Force val=" << val
	   << ", vwid=" << vwid << ", base=" << base << endl;
}

ساكن ثابت size_t FORCE4_CHUNK_گنتی = 8192 / حجم(بنية force_vector4_event_s);
ساكن slab_t<حجم(force_vector4_event_s),FORCE4_CHUNK_گنتی> force4_heap;

مضمن فراغ* force_vector4_event_s::operator جديد(size_t size)
{
      أكد(size == حجم(force_vector4_event_s));
      أرجع force4_heap.alloc_slab();
}

فراغ force_vector4_event_s::operator delete(فراغ*dptr)
{
      force4_heap.free_slab(dptr);
}

unsigned long گنتی_force4_pool(فراغ) { أرجع force4_heap.pool; }

/*
 * This class supports the propagation of vec4 outputs from a
 * vvp_net_t object.
 */
بنية propagate_vector4_event_s : عام event_s {
	/* The default constructor. */
      explicit propagate_vector4_event_s(ثابت vvp_vector4_t&that) : val(that) {
	    net = عدم;
	    next = عدم;
      }
	/* A constructor that makes the val directly. */
      propagate_vector4_event_s(ثابت vvp_vector4_t&that, unsigned adr, unsigned wid)
      : val(that,adr,wid) {
	    net = عدم;
	    next = عدم;
      }

	/* Propagate the output of this net. */
      vvp_net_t*net;
	/* value to propagate */
      vvp_vector4_t val;
	/* Action */
      فراغ run_run(فراغ) override;
      فراغ single_step_display(فراغ) override;
};

فراغ propagate_vector4_event_s::run_run(فراغ)
{
      net->send_vec4(val, 0);
}

فراغ propagate_vector4_event_s::single_step_display(فراغ)
{
      cerr << "propagate_vector4_event: Propagate val=" << val << endl;
}

/*
 * This class supports the propagation of real outputs from a
 * vvp_net_t object.
 */
بنية propagate_real_event_s : عام event_s {
	/* Propagate the output of this net. */
      vvp_net_t*net;
	/* value to propagate */
      مزدوج val;
	/* Action */
      فراغ run_run(فراغ) override;
      فراغ single_step_display(فراغ) override;
};

فراغ propagate_real_event_s::run_run(فراغ)
{
      net->send_real(val, 0);
}

فراغ propagate_real_event_s::single_step_display(فراغ)
{
      cerr << "propagate_real_event: Propagate val=" << val << endl;
}

بنية assign_array_r_word_s  : عام event_s {
      vvp_array_t mem;
      unsigned adr;
      مزدوج val;
      فراغ run_run(فراغ) override;

      ساكن فراغ* operator جديد(size_t);
      ساكن فراغ operator delete(فراغ*);
};

فراغ assign_array_r_word_s::run_run(فراغ)
{
      گنتی_assign_events += 1;
      mem->set_word(adr, val);
}
ساكن ثابت size_t ARRAY_R_W_CHUNK_گنتی = 8192 / حجم(بنية assign_array_r_word_s);
ساكن slab_t<حجم(assign_array_r_word_s),ARRAY_R_W_CHUNK_گنتی> array_r_w_heap;

مضمن فراغ* assign_array_r_word_s::operator جديد(size_t size)
{
      أكد(size == حجم(assign_array_r_word_s));
      أرجع array_r_w_heap.alloc_slab();
}

فراغ assign_array_r_word_s::operator delete(فراغ*ptr)
{
      array_r_w_heap.free_slab(ptr);
}

unsigned long گنتی_assign_arword_pool(فراغ) { أرجع array_r_w_heap.pool; }

بنية generic_event_s : عام event_s {
      vvp_gen_event_t obj;
      منطقي delete_obj_when_done;
      فراغ run_run(فراغ) override;
      فراغ single_step_display(فراغ) override;

      ساكن فراغ* operator جديد(size_t);
      ساكن فراغ operator delete(فراغ*);
};

فراغ generic_event_s::run_run(فراغ)
{
      گنتی_gen_events += 1;
      إذا (obj) {
	    obj->run_run();
	    إذا (delete_obj_when_done)
		  delete obj;
      }
}

فراغ generic_event_s::single_step_display(فراغ)
{
      obj->single_step_display();
}

ساكن ثابت size_t GENERIC_CHUNK_گنتی = 131072 / حجم(بنية generic_event_s);
ساكن slab_t<حجم(generic_event_s),GENERIC_CHUNK_گنتی> generic_event_heap;

مضمن فراغ* generic_event_s::operator جديد(size_t size)
{
      أكد(size == حجم(generic_event_s));
      أرجع generic_event_heap.alloc_slab();
}

فراغ generic_event_s::operator delete(فراغ*ptr)
{
      generic_event_heap.free_slab(ptr);
}

unsigned long گنتی_gen_pool(فراغ) { أرجع generic_event_heap.pool; }

/*
** These event_time_s will be required a lot, at high frequency.
** Once allocated, we never free them, but stash them away for next time.
*/


ساكن ثابت size_t TIME_CHUNK_گنتی = 8192 / حجم(بنية event_time_s);
ساكن slab_t<حجم(event_time_s),TIME_CHUNK_گنتی> event_time_heap;

مضمن فراغ* event_time_s::operator جديد (size_t size)
{
      أكد(size == حجم(بنية event_time_s));
      فراغ*ptr = event_time_heap.alloc_slab();
      أرجع ptr;
}

مضمن فراغ event_time_s::operator delete(فراغ*ptr, size_t)
{
      event_time_heap.free_slab(ptr);
}

unsigned long گنتی_time_pool(فراغ) { أرجع event_time_heap.pool; }

/*
 * This is the head of the list of pending events. This includes all
 * the events that have not been executed yet, and reaches into the
 * future.
 */
ساكن بنية event_time_s* sched_list = 0;

/*
 * This is a list of initialization events. The setup puts
 * initializations in this list so that they happen before the
 * simulation as a whole starts. This prevents time-0 triggers of
 * certain events.
 */
ساكن بنية event_s* schedule_init_list = 0;

/*
 * This is the head of the list of final events.
 */
ساكن بنية event_s* schedule_final_list = 0;

/*
 * This flag is true until a VPI task or function finishes the
 * simulation.
 */
ساكن منطقي schedule_runnable = true;
ساكن منطقي schedule_stopped_flag  = خطأ;
ساكن منطقي schedule_single_step_flag = خطأ;
ساكن منطقي no_signals_flag = خطأ;

فراغ schedule_finish(صحيح)
{
      schedule_runnable = خطأ;
}

فراغ schedule_stop(صحيح)
{
      schedule_stopped_flag = true;
}

فراغ schedule_single_step(صحيح)
{
      schedule_single_step_flag = true;
}

منطقي schedule_finished(فراغ)
{
      أرجع !schedule_runnable;
}

منطقي schedule_stopped(فراغ)
{
      أرجع schedule_stopped_flag;
}

خارجي "C" فراغ vvp_no_signals(فراغ)
{
      no_signals_flag = true;
}

/*
 * These are the signal handling infrastructure. The SIGINT signal
 * leads to an implicit $stop. The SIGHUP and SIGTERM signals lead
 * to an implicit $finish.
 */
خارجي منطقي stop_is_finish;

خارجي "C" فراغ signals_handler(صحيح signum)
{
#ifdef __MINGW32__
	// Windows implements the original UNIX semantics for signal,
	// so we have to re-establish the signal handler each time a
	// signal is caught.
      إشارة(signum, &signals_handler);
#endif
      إذا (signum != SIGINT)
	    stop_is_finish = true;
      schedule_stopped_flag = true;
}

ساكن فراغ signals_capture(فراغ)
{
      إذا (no_signals_flag)
	   أرجع;
#ifndef __MINGW32__
      إشارة(SIGHUP,  &signals_handler);
#endif
      إشارة(SIGINT,  &signals_handler);
      إشارة(SIGTERM, &signals_handler);
}

ساكن فراغ signals_revert(فراغ)
{
      إذا (no_signals_flag)
	   أرجع;
#ifndef __MINGW32__
      إشارة(SIGHUP,  SIG_DFL);
#endif
      إشارة(SIGINT,  SIG_DFL);
      إشارة(SIGTERM, SIG_DFL);
}

/*
 * This function puts an event on the end of the pre-simulation event queue.
 */
ساكن فراغ schedule_init_event(بنية event_s*cur)
{
      إذا (schedule_init_list == 0) {
            cur->next = cur;
      } وإلا {
            cur->next = schedule_init_list->next;
            schedule_init_list->next = cur;
      }
      schedule_init_list = cur;
}

/*
 * This function puts an event on the end of the post-simulation event queue.
 */
ساكن فراغ schedule_final_event(بنية event_s*cur)
{
      إذا (schedule_final_list == 0) {
            cur->next = cur;
      } وإلا {
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
تعريفنوع تعداد event_queue_e { SEQ_START, SEQ_ACTIVE, SEQ_INACTIVE, SEQ_NBASSIGN,
			     SEQ_RWSYNC, SEQ_ROSYNC, DEL_THREAD } event_queue_t;

ساكن فراغ schedule_event_(بنية event_s*cur, vvp_time64_t delay,
			    event_queue_t select_queue)
{
      cur->next = cur;
      بنية event_time_s*ctim = sched_list;

      إذا (sched_list == 0) {
	      /* Is the event_time list completely empty? Create the
		 first event_time object. */
	    ctim = جديد بنية event_time_s;
	    ctim->delay = delay;
	    ctim->next  = 0;
	    sched_list = ctim;

      } وإلا إذا (sched_list->delay > delay) {

	      /* Am I looking for an event before the first event_time?
		 If so, create a new event_time to go in front. */
	    بنية event_time_s*tmp = جديد بنية event_time_s;
	    tmp->delay = delay;
	    tmp->next = ctim;
	    ctim->delay -= delay;
	    ctim = tmp;
	    sched_list = ctim;

      } وإلا {
	    بنية event_time_s*prev = 0;

	    طالما (ctim->next && (ctim->delay < delay)) {
		  delay -= ctim->delay;
		  prev = ctim;
		  ctim = ctim->next;
	    }

	    إذا (ctim->delay > delay) {
		  بنية event_time_s*tmp = جديد بنية event_time_s;
		  tmp->delay = delay;
		  tmp->next  = prev->next;
		  prev->next = tmp;

		  tmp->next->delay -= delay;
		  ctim = tmp;

	    } وإلا إذا (ctim->delay == delay) {

	    } وإلا {
		  أكد(ctim->next == 0);
		  بنية event_time_s*tmp = جديد بنية event_time_s;
		  tmp->delay = delay - ctim->delay;
		  tmp->next = 0;
		  ctim->next = tmp;

		  ctim = tmp;
	    }
      }

	/* By this point, ctim is the event_time structure that is to
	   receive the event at hand. Put the event in to the
	   appropriate list for the kind of assign we have at hand. */

      بنية event_s** q = 0;

      بدل (select_queue) {
	  حالة SEQ_START:
	    q = &ctim->start;
	    اكسر;

	  حالة SEQ_ACTIVE:
	    q = &ctim->active;
	    اكسر;

	  حالة SEQ_INACTIVE:
	    أكد(delay == 0);
	    q = &ctim->inactive;
	    اكسر;

	  حالة SEQ_NBASSIGN:
	    q = &ctim->nbassign;
	    اكسر;

	  حالة SEQ_RWSYNC:
	    q = &ctim->rwsync;
	    اكسر;

	  حالة SEQ_ROSYNC:
	    q = &ctim->rosync;
	    اكسر;

	  حالة DEL_THREAD:
	    q = &ctim->del_thr;
	    اكسر;
      }

      إذا (q) {
	    إذا (*q) {
		  /* Put the cur event on the end of the queue. */
		  cur->next = (*q)->next;
		  (*q)->next = cur;
	    }
	    *q = cur;
      }
}

ساكن فراغ schedule_event_ڈالو(بنية event_s*cur)
{
      إذا ((sched_list == 0) || (sched_list->delay > 0)) {
	    schedule_event_(cur, 0, SEQ_ACTIVE);
	    أرجع;
      }

      بنية event_time_s*ctim = sched_list;

      إذا (ctim->active == 0) {
	    cur->next = cur;
	    ctim->active = cur;
	    أرجع;
      }

      cur->next = ctim->active->next;
      ctim->active->next = cur;
}

فراغ schedule_vthread(vthread_t thr, vvp_time64_t delay, منطقي ڈالو_flag)
{
      بنية vthread_event_s*cur = جديد vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);

      إذا (ڈالو_flag && (delay == 0)) {
	      /* Special case: If the delay is 0, the push_flag means
		 I can push this event in front of everything. This is
		 used by the %fork statement, for example, to perform
		 task calls. */
	    schedule_event_ڈالو(cur);

      } وإلا {
	    schedule_event_(cur, delay, SEQ_ACTIVE);
      }
}

فراغ schedule_t0_trigger(vvp_net_ptr_t ptr)
{
      vvp_vector4_t bit (1, BIT4_X);
      بنية assign_vector4_event_s*cur = جديد بنية assign_vector4_event_s(bit);
      cur->ptr = ptr;
      schedule_event_(cur, 0, SEQ_INACTIVE);
}

فراغ schedule_inactive(vthread_t thr)
{
      بنية vthread_event_s*cur = جديد vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);
      schedule_event_(cur, 0, SEQ_INACTIVE);
}

فراغ schedule_init_vthread(vthread_t thr)
{
      بنية vthread_event_s*cur = جديد vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);

      schedule_init_event(cur);
}

فراغ schedule_final_vthread(vthread_t thr)
{
      بنية vthread_event_s*cur = جديد vthread_event_s;

      cur->thr = thr;
      vthread_mark_final(thr);
      vthread_mark_scheduled(thr);

      schedule_final_event(cur);
}

فراغ schedule_assign_vector(vvp_net_ptr_t ptr,
			    unsigned base, unsigned vwid,
			    ثابت vvp_vector4_t&bit,
			    vvp_time64_t delay)
{
      بنية assign_vector4_event_s*cur = جديد بنية assign_vector4_event_s(bit);
      cur->ptr = ptr;
      cur->base = base;
      cur->vwid = vwid;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

فراغ schedule_force_vector(vvp_net_t*net,
			    unsigned base, unsigned vwid,
			    ثابت vvp_vector4_t&bit,
			    vvp_time64_t delay)
{
      بنية force_vector4_event_s*cur = جديد بنية force_vector4_event_s(bit);
      cur->net = net;
      cur->base = base;
      cur->vwid = vwid;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

فراغ schedule_propagate_vector(vvp_net_t*net,
			       vvp_time64_t delay,
			       ثابت vvp_vector4_t&src)
{
      بنية propagate_vector4_event_s*cur
	    = جديد بنية propagate_vector4_event_s(src);
      cur->net = net;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

// FIXME: This needs to create a non-blocking event, but only one per time slot.
//        Is schedule_event_ or execution actually filtering since the net is
//        already X because this is not triggering?
فراغ schedule_propagate_event(vvp_net_t*net,
                              vvp_time64_t delay)
{
      vvp_vector4_t tmp (1, BIT4_X);
      بنية propagate_vector4_event_s*cur
	    = جديد بنية propagate_vector4_event_s(tmp);
      cur->net = net;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

فراغ schedule_assign_array_word(vvp_array_t mem,
				unsigned word_addr,
				unsigned off,
				ثابت vvp_vector4_t&val,
				vvp_time64_t delay)
{
      بنية assign_array_word_s*cur = جديد بنية assign_array_word_s;
      cur->mem = mem;
      cur->adr = word_addr;
      cur->off = off;
      cur->val = val;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

فراغ schedule_assign_array_word(vvp_array_t mem,
				unsigned word_addr,
				مزدوج val,
				vvp_time64_t delay)
{
      بنية assign_array_r_word_s*cur = جديد بنية assign_array_r_word_s;
      cur->mem = mem;
      cur->adr = word_addr;
      cur->val = val;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

فراغ schedule_set_vector(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit)
{
      بنية assign_vector4_event_s*cur = جديد بنية assign_vector4_event_s(bit);
      cur->ptr = ptr;
      cur->base = 0;
      cur->vwid = 0;
      schedule_event_(cur, 0, SEQ_ACTIVE);
}

فراغ schedule_set_vector(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&bit)
{
      بنية assign_vector8_event_s*cur = جديد بنية assign_vector8_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_event_(cur, 0, SEQ_ACTIVE);
}

فراغ schedule_set_vector(vvp_net_ptr_t ptr, مزدوج bit)
{
      بنية assign_real_event_s*cur = جديد بنية assign_real_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_event_(cur, 0, SEQ_ACTIVE);
}

فراغ schedule_init_vector(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit)
{
      بنية assign_vector4_event_s*cur = جديد بنية assign_vector4_event_s(bit);
      cur->ptr = ptr;
      cur->base = 0;
      cur->vwid = 0;
      schedule_init_event(cur);
}

فراغ schedule_init_vector(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&bit)
{
      بنية assign_vector8_event_s*cur = جديد بنية assign_vector8_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_init_event(cur);
}

فراغ schedule_init_vector(vvp_net_ptr_t ptr, مزدوج bit)
{
      بنية assign_real_event_s*cur = جديد بنية assign_real_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_init_event(cur);
}

فراغ schedule_init_propagate(vvp_net_t*net, vvp_vector4_t bit)
{
      بنية propagate_vector4_event_s*cur = جديد بنية propagate_vector4_event_s(bit);
      cur->net = net;
      schedule_init_event(cur);
}

فراغ schedule_init_propagate(vvp_net_t*net, مزدوج bit)
{
      بنية propagate_real_event_s*cur = جديد بنية propagate_real_event_s;
      cur->net = net;
      cur->val = bit;
      schedule_init_event(cur);
}

فراغ schedule_del_thr(vthread_t thr)
{
      بنية del_thr_event_s*cur = جديد del_thr_event_s;

      cur->thr = thr;

      schedule_event_(cur, 0, DEL_THREAD);
}

فراغ schedule_generic(vvp_gen_event_t obj, vvp_time64_t delay,
		      منطقي sync_flag, منطقي ro_flag, منطقي delete_when_done)
{
      بنية generic_event_s*cur = جديد generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = delete_when_done;
      schedule_event_(cur, delay,
		      sync_flag? (ro_flag?SEQ_ROSYNC:SEQ_RWSYNC) : SEQ_ACTIVE);

      إذا (sync_flag)
	    vthread_delay_delete();
}

ساكن منطقي sim_started;

فراغ schedule_functor(vvp_gen_event_t obj)
{
      بنية generic_event_s*cur = جديد generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = خطأ;
      إذا (!sim_started) {
            schedule_init_event(cur);
      } وإلا {
            schedule_event_(cur, 0, SEQ_ACTIVE);
      }
}

فراغ schedule_at_start_of_simtime(vvp_gen_event_t obj, vvp_time64_t delay)
{
      بنية generic_event_s*cur = جديد generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = خطأ;
      schedule_event_(cur, delay, SEQ_START);
}

/*
 * In the vvp runtime of Icarus Verilog, the SEQ_RWSYNC time step is
 * after all of the non-blocking assignments, so is effectively the
 * same as the ReadWriteSync time.
 */
فراغ schedule_at_end_of_simtime(vvp_gen_event_t obj, vvp_time64_t delay)
{
      بنية generic_event_s*cur = جديد generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = خطأ;
      schedule_event_(cur, delay, SEQ_RWSYNC);
}

ساكن vvp_time64_t schedule_time;
vvp_time64_t schedule_simtime(فراغ)
{ أرجع schedule_time; }

ساكن منطقي sim_at_rosync = خطأ;
منطقي schedule_at_rosync(فراغ)
{ أرجع sim_at_rosync; }

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
ساكن فراغ run_rosync(بنية event_time_s*ctim)
{
      sim_at_rosync = true;
      طالما (ctim->rosync) {
	    بنية event_s*cur = ctim->rosync->next;
	    إذا (cur->next == cur) {
		  ctim->rosync = 0;
	    } وإلا {
		  ctim->rosync->next = cur->next;
	    }

	    cur->run_run();
	    delete cur;
      }
      sim_at_rosync = خطأ;

      طالما (ctim->del_thr) {
	    بنية event_s*cur = ctim->del_thr->next;
	    إذا (cur->next == cur) {
		  ctim->del_thr = 0;
	    } وإلا {
		  ctim->del_thr->next = cur->next;
	    }

	    cur->run_run();
	    delete cur;
      }

      إذا (ctim->active || ctim->inactive || ctim->nbassign || ctim->rwsync) {
	    cerr << "SCHEDULER ERROR: read-only sync events "
		 << "created RW events!" << endl;
      }
}

فراغ schedule_simulate(فراغ)
{
      منطقي run_finals;
      sim_started = خطأ;

      schedule_time = 0;

      إذا (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute EndOfCompile callbacks\n");
      }

      // Execute end of compile callbacks
      vpiEndOfCompile();

      إذا (verbose_flag) {
	    vpi_mcd_printf(1, " ...propagate initialization events\n");
      }

	// Execute initialization events.
      طالما (schedule_init_list) {
	    بنية event_s*cur = schedule_init_list->next;
	    إذا (cur->next == cur) {
		  schedule_init_list = 0;
	    } وإلا {
		  schedule_init_list->next = cur->next;
	    }
	    cur->run_run();
	    delete cur;
      }

      sim_started = true;

      إذا (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute StartOfSim callbacks\n");
      }

      // Execute start of simulation callbacks
      vpiStartOfSim();

      signals_capture();

      إذا (verbose_flag) {
	    vpi_mcd_printf(1, " ...run scheduler\n");
      }

      // If there were no compiletf, etc. errors then we are going to
      // process events and when done run the final blocks.
      run_finals = schedule_runnable;

      إذا (schedule_runnable) طالما (sched_list) {

	    إذا (schedule_stopped_flag) {
		  schedule_stopped_flag = خطأ;
		  stop_handler(0);
		  // You can finish from the debugger without a time change.
		  إذا (!schedule_runnable) اكسر;
		  استمر;
	    }

	      /* ctim is the current time step. */
	    بنية event_time_s* ctim = sched_list;

	      /* If the time is advancing, then first run the
		 postponed sync events. Run them all. */
	    إذا (ctim->delay > 0) {

		  إذا (!schedule_runnable) اكسر;
		  schedule_time += ctim->delay;
		    /* When the design is being traced (we are emitting
		     * file/line information) also print any time changes. */
		  إذا (show_file_سطر) {
			cerr << "Advancing to simulation time: "
			     << schedule_time << endl;
		  }
		  ctim->delay = 0;

		  vpiNextSimTime();
		    // Process the cbAtStartOfSimTime callbacks.
		  طالما (ctim->start) {
			بنية event_s*cur = ctim->start->next;
			إذا (cur->next == cur) {
			      ctim->start = 0;
			} وإلا {
			      ctim->start->next = cur->next;
			}
			cur->run_run();
			delete (cur);
		  }
	    }


	      /* If there are no more active events, advance the event
		 queues. If there are not events at all, then release
		 the event_time object. */
	    إذا (ctim->active == 0) {
		  ctim->active = ctim->inactive;
		  ctim->inactive = 0;

		  إذا (ctim->active == 0) {
			ctim->active = ctim->nbassign;
			ctim->nbassign = 0;

			إذا (ctim->active == 0) {
			      ctim->active = ctim->rwsync;
			      ctim->rwsync = 0;

				/* If out of rw events, then run the rosync
				   events and delete this time step. This also
				   deletes threads as needed. */
			      إذا (ctim->active == 0) {
				    run_rosync(ctim);
				    sched_list = ctim->next;
				    delete ctim;
				    استمر;
			      }
			}
		  }
	    }

	      /* Pull the first item off the list. If this is the last
		 cell in the list, then clear the list. Execute that
		 event type, and delete it. */
	    بنية event_s*cur = ctim->active->next;
	    إذا (cur->next == cur) {
		  ctim->active = 0;
	    } وإلا {
		  ctim->active->next = cur->next;
	    }

	    إذا (schedule_single_step_flag) {
		  cur->single_step_display();
		  schedule_stopped_flag = true;
		  schedule_single_step_flag = خطأ;
	    }

	    cur->run_run();

	    delete (cur);
      }

	// Execute final events.
      schedule_runnable = run_finals;
      طالما (schedule_runnable && schedule_final_list) {
	    بنية event_s*cur = schedule_final_list->next;
	    إذا (cur->next == cur) {
		  schedule_final_list = 0;
	    } وإلا {
		  schedule_final_list->next = cur->next;
	    }
	    cur->run_run();
	    delete cur;
      }

      signals_revert();

      إذا (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute Postsim callbacks\n");
      }

      // Execute post-simulation callbacks
      vpiPostsim();
#ifdef CHECK_WITH_VALGRIND
      schedule_delete();
#endif
}

#ifdef CHECK_WITH_VALGRIND
فراغ schedule_delete(فراغ)
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
