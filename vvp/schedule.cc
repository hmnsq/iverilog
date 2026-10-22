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

using فضاینام std;

unsigned long شمار_assign_events = 0;
unsigned long شمار_gen_events = 0;
unsigned long شمار_thread_events = 0;
  // Count the time events (A time cell created)
unsigned long شمار_time_events = 0;



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
ساختار event_s {
      ساختار event_s*next;
      مجازی ~event_s() { }
      مجازی تهی run_run(تهی) =0;

	// Write something about the event to stderr
      مجازی تهی single_step_display(تهی);

	// Fallback new/delete
      ایستا تهی*operator جدید (size_t size) { بازگشت ::جدید نویسه[size]; }
      ایستا تهی operator delete(تهی*ptr)  { ::delete[]( static_cast<نویسه*>(ptr) ); }
};

تهی event_s::single_step_display(تهی)
{
      std::cerr << "event_s: Step into event " << typeid(*این).name() << std::endl;
}

ساختار event_time_s {
      event_time_s() {
	    شمار_time_events += 1;
	    delay = 0;
	    start = 0;
	    active = 0;
	    inactive = 0;
	    nbassign = 0;
	    rwsync = 0;
	    rosync = 0;
	    del_thr = 0;
	    next = پوچ;
      }
      vvp_time64_t delay;

      ساختار event_s*start;
      ساختار event_s*active;
      ساختار event_s*inactive;
      ساختار event_s*nbassign;
      ساختار event_s*rwsync;
      ساختار event_s*rosync;
      ساختار event_s*del_thr;

      ساختار event_time_s*next;

      ایستا تهی* operator جدید (size_t);
      ایستا تهی operator delete(تهی*obj, size_t s);
};

vvp_gen_event_s::~vvp_gen_event_s()
{
}

تهی vvp_gen_event_s::single_step_display(تهی)
{
      cerr << "vvp_gen_event_s: Step into event " << typeid(*این).name() << endl;
}

/*
 * Derived event types
 */
ساختار vthread_event_s : عمومی event_s {
      vthread_t thr;
      تهی run_run(تهی) override;
      تهی single_step_display(تهی) override;

      ایستا تهی* operator جدید(size_t);
      ایستا تهی operator delete(تهی*);
};

تهی vthread_event_s::run_run(تهی)
{
      شمار_thread_events += 1;
      vthread_run(thr);
}

تهی vthread_event_s::single_step_display(تهی)
{
      __vpiScope*scope = vthread_scope(thr);
      cerr << "vthread_event: Resume thread"
	   << " scope=" << scope->vpi_get_str(vpiFullName)
	   << endl;
}

ایستا ثابت size_t VTHR_CHUNK_شمار = 8192 / اندازه(ساختار vthread_event_s);
ایستا slab_t<اندازه(vthread_event_s),VTHR_CHUNK_شمار> vthread_event_heap;

درونخطی تهی* vthread_event_s::operator جدید(size_t size)
{
      أكد(size == اندازه(vthread_event_s));
      بازگشت vthread_event_heap.alloc_slab();
}

تهی vthread_event_s::operator delete(تهی*dptr)
{
      vthread_event_heap.free_slab(dptr);
}

ساختار del_thr_event_s : عمومی event_s {
      vthread_t thr;
      تهی run_run(تهی) override;
      تهی single_step_display(تهی) override;
};

تهی del_thr_event_s::run_run(تهی)
{
      vthread_delete(thr);
}

تهی del_thr_event_s::single_step_display(تهی)
{
      __vpiScope*scope = vthread_scope(thr);
      cerr << "del_thr_event: Reap completed thread"
	   << " scope=" << scope->vpi_get_str(vpiFullName) << endl;
}

ساختار assign_vector4_event_s  : عمومی event_s {
	/* The default constructor. */
      explicit assign_vector4_event_s(ثابت vvp_vector4_t&that) : val(that) {
	    base = 0;
	    vwid = 0;
	    next = پوچ;
      }

	/* Where to do the assign. */
      vvp_net_ptr_t ptr;
	/* Value to assign. */
      vvp_vector4_t val;
	/* Offset of the part into the destination. */
      unsigned base;
	/* Width of the destination vector. */
      unsigned vwid;
      تهی run_run(تهی) override;
      تهی single_step_display(تهی) override;

      ایستا تهی* operator جدید(size_t);
      ایستا تهی operator delete(تهی*);
};

تهی assign_vector4_event_s::run_run(تهی)
{
      شمار_assign_events += 1;
      اگر (vwid > 0)
	    vvp_send_vec4_pv(ptr, val, base, vwid, 0);
      وگرنه
	    vvp_send_vec4(ptr, val, 0);
}

تهی assign_vector4_event_s::single_step_display(تهی)
{
      cerr << "assign_vector4_event: Propagate val=" << val
	   << ", vwid=" << vwid << ", base=" << base << endl;
}

ایستا ثابت size_t ASSIGN4_CHUNK_شمار = 524288 / اندازه(ساختار assign_vector4_event_s);
ایستا slab_t<اندازه(assign_vector4_event_s),ASSIGN4_CHUNK_شمار> assign4_heap;

درونخطی تهی* assign_vector4_event_s::operator جدید(size_t size)
{
      أكد(size == اندازه(assign_vector4_event_s));
      بازگشت assign4_heap.alloc_slab();
}

تهی assign_vector4_event_s::operator delete(تهی*dptr)
{
      assign4_heap.free_slab(dptr);
}

unsigned long شمار_assign4_pool(تهی) { بازگشت assign4_heap.pool; }

ساختار assign_vector8_event_s  : عمومی event_s {
      vvp_net_ptr_t ptr;
      vvp_vector8_t val;
      تهی run_run(تهی) override;
      تهی single_step_display(تهی) override;

      ایستا تهی* operator جدید(size_t);
      ایستا تهی operator delete(تهی*);
};

تهی assign_vector8_event_s::run_run(تهی)
{
      شمار_assign_events += 1;
      vvp_send_vec8(ptr, val);
}

تهی assign_vector8_event_s::single_step_display(تهی)
{
      cerr << "assign_vector8_event: Propagate val=" << val << endl;
}

ایستا ثابت size_t ASSIGN8_CHUNK_شمار = 8192 / اندازه(ساختار assign_vector8_event_s);
ایستا slab_t<اندازه(assign_vector8_event_s),ASSIGN8_CHUNK_شمار> assign8_heap;

درونخطی تهی* assign_vector8_event_s::operator جدید(size_t size)
{
      أكد(size == اندازه(assign_vector8_event_s));
      بازگشت assign8_heap.alloc_slab();
}

تهی assign_vector8_event_s::operator delete(تهی*dptr)
{
      assign8_heap.free_slab(dptr);
}

unsigned long شمار_assign8_pool() { بازگشت assign8_heap.pool; }

ساختار assign_real_event_s  : عمومی event_s {
      vvp_net_ptr_t ptr;
      دوگانه val;
      تهی run_run(تهی) override;
      تهی single_step_display(تهی) override;

      ایستا تهی* operator جدید(size_t);
      ایستا تهی operator delete(تهی*);
};

تهی assign_real_event_s::run_run(تهی)
{
      شمار_assign_events += 1;
      vvp_send_real(ptr, val, 0);
}

تهی assign_real_event_s::single_step_display(تهی)
{
      cerr << "assign_real_event: Propagate val=" << val << endl;
}

ایستا ثابت size_t ASSIGNR_CHUNK_شمار = 8192 / اندازه(ساختار assign_real_event_s);
ایستا slab_t<اندازه(assign_real_event_s),ASSIGNR_CHUNK_شمار> assignr_heap;

درونخطی تهی* assign_real_event_s::operator جدید (size_t size)
{
      أكد(size == اندازه(assign_real_event_s));
      بازگشت assignr_heap.alloc_slab();
}

تهی assign_real_event_s::operator delete(تهی*dptr)
{
      assignr_heap.free_slab(dptr);
}

unsigned long شمار_assign_real_pool(تهی) { بازگشت assignr_heap.pool; }

ساختار assign_array_word_s  : عمومی event_s {
      vvp_array_t mem;
      unsigned adr;
      vvp_vector4_t val;
      unsigned off;
      تهی run_run(تهی) override;

      ایستا تهی* operator جدید(size_t);
      ایستا تهی operator delete(تهی*);
};

تهی assign_array_word_s::run_run(تهی)
{
      شمار_assign_events += 1;
      mem->set_word(adr, off, val);
}

ایستا ثابت size_t ARRAY_W_CHUNK_شمار = 8192 / اندازه(ساختار assign_array_word_s);
ایستا slab_t<اندازه(assign_array_word_s),ARRAY_W_CHUNK_شمار> array_w_heap;

درونخطی تهی* assign_array_word_s::operator جدید (size_t size)
{
      أكد(size == اندازه(assign_array_word_s));
      بازگشت array_w_heap.alloc_slab();
}

تهی assign_array_word_s::operator delete(تهی*ptr)
{
      array_w_heap.free_slab(ptr);
}

unsigned long شمار_assign_aword_pool(تهی) { بازگشت array_w_heap.pool; }

ساختار force_vector4_event_s  : عمومی event_s {
	/* The default constructor. */
      explicit force_vector4_event_s(ثابت vvp_vector4_t&that): val(that) {
	    net = پوچ;
	    base = 0;
	    vwid = 0;
	    next = پوچ;
      }
	/* Where to do the force. */
      vvp_net_t*net;
	/* Value to force. */
      vvp_vector4_t val;
	/* Offset of the part into the destination. */
      unsigned base;
	/* Width of the destination vector. */
      unsigned vwid;

      تهی run_run(تهی) override;
      تهی single_step_display(تهی) override;

      ایستا تهی* operator جدید(size_t);
      ایستا تهی operator delete(تهی*);
};

تهی force_vector4_event_s::run_run(تهی)
{
      شمار_assign_events += 1;

      unsigned wid = val.size();
      اگر ((base + wid) > vwid)
	    wid = vwid - base;

	// Make a mask of which bits are to be forced, 0 for unforced
	// bits and 1 for forced bits.
      vvp_vector2_t mask (vvp_vector2_t::FILL0, vwid);
      برای (unsigned idx = 0 ; idx < wid ; idx += 1)
	    mask.set_bit(base+idx, 1);

      vvp_vector4_t tmp (vwid, BIT4_Z);

	// vvp_net_t::force_vec4 propagates all the bits of the
	// forced vector value, regardless of the mask. This
	// ensures the unforced bits retain their current value.
      vvp_signal_مقدار*sig = dynamic_cast<vvp_signal_مقدار*>(net->fil);
      أكد(sig);
      sig->vec4_مقدار(tmp);

      tmp.set_vec(base, val);
      net->force_vec4(tmp, mask);
}

تهی force_vector4_event_s::single_step_display(تهی)
{
      cerr << "force_vector4_event: Force val=" << val
	   << ", vwid=" << vwid << ", base=" << base << endl;
}

ایستا ثابت size_t FORCE4_CHUNK_شمار = 8192 / اندازه(ساختار force_vector4_event_s);
ایستا slab_t<اندازه(force_vector4_event_s),FORCE4_CHUNK_شمار> force4_heap;

درونخطی تهی* force_vector4_event_s::operator جدید(size_t size)
{
      أكد(size == اندازه(force_vector4_event_s));
      بازگشت force4_heap.alloc_slab();
}

تهی force_vector4_event_s::operator delete(تهی*dptr)
{
      force4_heap.free_slab(dptr);
}

unsigned long شمار_force4_pool(تهی) { بازگشت force4_heap.pool; }

/*
 * This class supports the propagation of vec4 outputs from a
 * vvp_net_t object.
 */
ساختار propagate_vector4_event_s : عمومی event_s {
	/* The default constructor. */
      explicit propagate_vector4_event_s(ثابت vvp_vector4_t&that) : val(that) {
	    net = پوچ;
	    next = پوچ;
      }
	/* A constructor that makes the val directly. */
      propagate_vector4_event_s(ثابت vvp_vector4_t&that, unsigned adr, unsigned wid)
      : val(that,adr,wid) {
	    net = پوچ;
	    next = پوچ;
      }

	/* Propagate the output of this net. */
      vvp_net_t*net;
	/* value to propagate */
      vvp_vector4_t val;
	/* Action */
      تهی run_run(تهی) override;
      تهی single_step_display(تهی) override;
};

تهی propagate_vector4_event_s::run_run(تهی)
{
      net->send_vec4(val, 0);
}

تهی propagate_vector4_event_s::single_step_display(تهی)
{
      cerr << "propagate_vector4_event: Propagate val=" << val << endl;
}

/*
 * This class supports the propagation of real outputs from a
 * vvp_net_t object.
 */
ساختار propagate_real_event_s : عمومی event_s {
	/* Propagate the output of this net. */
      vvp_net_t*net;
	/* value to propagate */
      دوگانه val;
	/* Action */
      تهی run_run(تهی) override;
      تهی single_step_display(تهی) override;
};

تهی propagate_real_event_s::run_run(تهی)
{
      net->send_real(val, 0);
}

تهی propagate_real_event_s::single_step_display(تهی)
{
      cerr << "propagate_real_event: Propagate val=" << val << endl;
}

ساختار assign_array_r_word_s  : عمومی event_s {
      vvp_array_t mem;
      unsigned adr;
      دوگانه val;
      تهی run_run(تهی) override;

      ایستا تهی* operator جدید(size_t);
      ایستا تهی operator delete(تهی*);
};

تهی assign_array_r_word_s::run_run(تهی)
{
      شمار_assign_events += 1;
      mem->set_word(adr, val);
}
ایستا ثابت size_t ARRAY_R_W_CHUNK_شمار = 8192 / اندازه(ساختار assign_array_r_word_s);
ایستا slab_t<اندازه(assign_array_r_word_s),ARRAY_R_W_CHUNK_شمار> array_r_w_heap;

درونخطی تهی* assign_array_r_word_s::operator جدید(size_t size)
{
      أكد(size == اندازه(assign_array_r_word_s));
      بازگشت array_r_w_heap.alloc_slab();
}

تهی assign_array_r_word_s::operator delete(تهی*ptr)
{
      array_r_w_heap.free_slab(ptr);
}

unsigned long شمار_assign_arword_pool(تهی) { بازگشت array_r_w_heap.pool; }

ساختار generic_event_s : عمومی event_s {
      vvp_gen_event_t obj;
      منطقی delete_obj_when_done;
      تهی run_run(تهی) override;
      تهی single_step_display(تهی) override;

      ایستا تهی* operator جدید(size_t);
      ایستا تهی operator delete(تهی*);
};

تهی generic_event_s::run_run(تهی)
{
      شمار_gen_events += 1;
      اگر (obj) {
	    obj->run_run();
	    اگر (delete_obj_when_done)
		  delete obj;
      }
}

تهی generic_event_s::single_step_display(تهی)
{
      obj->single_step_display();
}

ایستا ثابت size_t GENERIC_CHUNK_شمار = 131072 / اندازه(ساختار generic_event_s);
ایستا slab_t<اندازه(generic_event_s),GENERIC_CHUNK_شمار> generic_event_heap;

درونخطی تهی* generic_event_s::operator جدید(size_t size)
{
      أكد(size == اندازه(generic_event_s));
      بازگشت generic_event_heap.alloc_slab();
}

تهی generic_event_s::operator delete(تهی*ptr)
{
      generic_event_heap.free_slab(ptr);
}

unsigned long شمار_gen_pool(تهی) { بازگشت generic_event_heap.pool; }

/*
** These event_time_s will be required a lot, at high frequency.
** Once allocated, we never free them, but stash them away for next time.
*/


ایستا ثابت size_t TIME_CHUNK_شمار = 8192 / اندازه(ساختار event_time_s);
ایستا slab_t<اندازه(event_time_s),TIME_CHUNK_شمار> event_time_heap;

درونخطی تهی* event_time_s::operator جدید (size_t size)
{
      أكد(size == اندازه(ساختار event_time_s));
      تهی*ptr = event_time_heap.alloc_slab();
      بازگشت ptr;
}

درونخطی تهی event_time_s::operator delete(تهی*ptr, size_t)
{
      event_time_heap.free_slab(ptr);
}

unsigned long شمار_time_pool(تهی) { بازگشت event_time_heap.pool; }

/*
 * This is the head of the list of pending events. This includes all
 * the events that have not been executed yet, and reaches into the
 * future.
 */
ایستا ساختار event_time_s* sched_list = 0;

/*
 * This is a list of initialization events. The setup puts
 * initializations in this list so that they happen before the
 * simulation as a whole starts. This prevents time-0 triggers of
 * certain events.
 */
ایستا ساختار event_s* schedule_init_list = 0;

/*
 * This is the head of the list of final events.
 */
ایستا ساختار event_s* schedule_final_list = 0;

/*
 * This flag is true until a VPI task or function finishes the
 * simulation.
 */
ایستا منطقی schedule_runnable = درست;
ایستا منطقی schedule_stopped_flag  = نادرست;
ایستا منطقی schedule_single_step_flag = نادرست;
ایستا منطقی no_signals_flag = نادرست;

تهی schedule_finish(صحیح)
{
      schedule_runnable = نادرست;
}

تهی schedule_stop(صحیح)
{
      schedule_stopped_flag = درست;
}

تهی schedule_single_step(صحیح)
{
      schedule_single_step_flag = درست;
}

منطقی schedule_finished(تهی)
{
      بازگشت !schedule_runnable;
}

منطقی schedule_stopped(تهی)
{
      بازگشت schedule_stopped_flag;
}

بیرونی "C" تهی vvp_no_signals(تهی)
{
      no_signals_flag = درست;
}

/*
 * These are the signal handling infrastructure. The SIGINT signal
 * leads to an implicit $stop. The SIGHUP and SIGTERM signals lead
 * to an implicit $finish.
 */
بیرونی منطقی stop_is_finish;

بیرونی "C" تهی signals_handler(صحیح signum)
{
#ifdef __MINGW32__
	// Windows implements the original UNIX semantics for signal,
	// so we have to re-establish the signal handler each time a
	// signal is caught.
      سیگنال(signum, &signals_handler);
#endif
      اگر (signum != SIGINT)
	    stop_is_finish = درست;
      schedule_stopped_flag = درست;
}

ایستا تهی signals_capture(تهی)
{
      اگر (no_signals_flag)
	   بازگشت;
#ifndef __MINGW32__
      سیگنال(SIGHUP,  &signals_handler);
#endif
      سیگنال(SIGINT,  &signals_handler);
      سیگنال(SIGTERM, &signals_handler);
}

ایستا تهی signals_revert(تهی)
{
      اگر (no_signals_flag)
	   بازگشت;
#ifndef __MINGW32__
      سیگنال(SIGHUP,  SIG_DFL);
#endif
      سیگنال(SIGINT,  SIG_DFL);
      سیگنال(SIGTERM, SIG_DFL);
}

/*
 * This function puts an event on the end of the pre-simulation event queue.
 */
ایستا تهی schedule_init_event(ساختار event_s*cur)
{
      اگر (schedule_init_list == 0) {
            cur->next = cur;
      } وگرنه {
            cur->next = schedule_init_list->next;
            schedule_init_list->next = cur;
      }
      schedule_init_list = cur;
}

/*
 * This function puts an event on the end of the post-simulation event queue.
 */
ایستا تهی schedule_final_event(ساختار event_s*cur)
{
      اگر (schedule_final_list == 0) {
            cur->next = cur;
      } وگرنه {
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
تعریفنوع شمارش event_queue_e { SEQ_START, SEQ_ACTIVE, SEQ_INACTIVE, SEQ_NBASSIGN,
			     SEQ_RWSYNC, SEQ_ROSYNC, DEL_THREAD } event_queue_t;

ایستا تهی schedule_event_(ساختار event_s*cur, vvp_time64_t delay,
			    event_queue_t select_queue)
{
      cur->next = cur;
      ساختار event_time_s*ctim = sched_list;

      اگر (sched_list == 0) {
	      /* Is the event_time list completely empty? Create the
		 first event_time object. */
	    ctim = جدید ساختار event_time_s;
	    ctim->delay = delay;
	    ctim->next  = 0;
	    sched_list = ctim;

      } وگرنه اگر (sched_list->delay > delay) {

	      /* Am I looking for an event before the first event_time?
		 If so, create a new event_time to go in front. */
	    ساختار event_time_s*tmp = جدید ساختار event_time_s;
	    tmp->delay = delay;
	    tmp->next = ctim;
	    ctim->delay -= delay;
	    ctim = tmp;
	    sched_list = ctim;

      } وگرنه {
	    ساختار event_time_s*prev = 0;

	    وقتی (ctim->next && (ctim->delay < delay)) {
		  delay -= ctim->delay;
		  prev = ctim;
		  ctim = ctim->next;
	    }

	    اگر (ctim->delay > delay) {
		  ساختار event_time_s*tmp = جدید ساختار event_time_s;
		  tmp->delay = delay;
		  tmp->next  = prev->next;
		  prev->next = tmp;

		  tmp->next->delay -= delay;
		  ctim = tmp;

	    } وگرنه اگر (ctim->delay == delay) {

	    } وگرنه {
		  أكد(ctim->next == 0);
		  ساختار event_time_s*tmp = جدید ساختار event_time_s;
		  tmp->delay = delay - ctim->delay;
		  tmp->next = 0;
		  ctim->next = tmp;

		  ctim = tmp;
	    }
      }

	/* By this point, ctim is the event_time structure that is to
	   receive the event at hand. Put the event in to the
	   appropriate list for the kind of assign we have at hand. */

      ساختار event_s** q = 0;

      انتخاب (select_queue) {
	  حالت SEQ_START:
	    q = &ctim->start;
	    بشکن;

	  حالت SEQ_ACTIVE:
	    q = &ctim->active;
	    بشکن;

	  حالت SEQ_INACTIVE:
	    أكد(delay == 0);
	    q = &ctim->inactive;
	    بشکن;

	  حالت SEQ_NBASSIGN:
	    q = &ctim->nbassign;
	    بشکن;

	  حالت SEQ_RWSYNC:
	    q = &ctim->rwsync;
	    بشکن;

	  حالت SEQ_ROSYNC:
	    q = &ctim->rosync;
	    بشکن;

	  حالت DEL_THREAD:
	    q = &ctim->del_thr;
	    بشکن;
      }

      اگر (q) {
	    اگر (*q) {
		  /* Put the cur event on the end of the queue. */
		  cur->next = (*q)->next;
		  (*q)->next = cur;
	    }
	    *q = cur;
      }
}

ایستا تهی schedule_event_افزودن(ساختار event_s*cur)
{
      اگر ((sched_list == 0) || (sched_list->delay > 0)) {
	    schedule_event_(cur, 0, SEQ_ACTIVE);
	    بازگشت;
      }

      ساختار event_time_s*ctim = sched_list;

      اگر (ctim->active == 0) {
	    cur->next = cur;
	    ctim->active = cur;
	    بازگشت;
      }

      cur->next = ctim->active->next;
      ctim->active->next = cur;
}

تهی schedule_vthread(vthread_t thr, vvp_time64_t delay, منطقی افزودن_flag)
{
      ساختار vthread_event_s*cur = جدید vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);

      اگر (افزودن_flag && (delay == 0)) {
	      /* Special case: If the delay is 0, the push_flag means
		 I can push this event in front of everything. This is
		 used by the %fork statement, for example, to perform
		 task calls. */
	    schedule_event_افزودن(cur);

      } وگرنه {
	    schedule_event_(cur, delay, SEQ_ACTIVE);
      }
}

تهی schedule_t0_trigger(vvp_net_ptr_t ptr)
{
      vvp_vector4_t bit (1, BIT4_X);
      ساختار assign_vector4_event_s*cur = جدید ساختار assign_vector4_event_s(bit);
      cur->ptr = ptr;
      schedule_event_(cur, 0, SEQ_INACTIVE);
}

تهی schedule_inactive(vthread_t thr)
{
      ساختار vthread_event_s*cur = جدید vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);
      schedule_event_(cur, 0, SEQ_INACTIVE);
}

تهی schedule_init_vthread(vthread_t thr)
{
      ساختار vthread_event_s*cur = جدید vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);

      schedule_init_event(cur);
}

تهی schedule_final_vthread(vthread_t thr)
{
      ساختار vthread_event_s*cur = جدید vthread_event_s;

      cur->thr = thr;
      vthread_mark_final(thr);
      vthread_mark_scheduled(thr);

      schedule_final_event(cur);
}

تهی schedule_assign_vector(vvp_net_ptr_t ptr,
			    unsigned base, unsigned vwid,
			    ثابت vvp_vector4_t&bit,
			    vvp_time64_t delay)
{
      ساختار assign_vector4_event_s*cur = جدید ساختار assign_vector4_event_s(bit);
      cur->ptr = ptr;
      cur->base = base;
      cur->vwid = vwid;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

تهی schedule_force_vector(vvp_net_t*net,
			    unsigned base, unsigned vwid,
			    ثابت vvp_vector4_t&bit,
			    vvp_time64_t delay)
{
      ساختار force_vector4_event_s*cur = جدید ساختار force_vector4_event_s(bit);
      cur->net = net;
      cur->base = base;
      cur->vwid = vwid;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

تهی schedule_propagate_vector(vvp_net_t*net,
			       vvp_time64_t delay,
			       ثابت vvp_vector4_t&src)
{
      ساختار propagate_vector4_event_s*cur
	    = جدید ساختار propagate_vector4_event_s(src);
      cur->net = net;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

// FIXME: This needs to create a non-blocking event, but only one per time slot.
//        Is schedule_event_ or execution actually filtering since the net is
//        already X because this is not triggering?
تهی schedule_propagate_event(vvp_net_t*net,
                              vvp_time64_t delay)
{
      vvp_vector4_t tmp (1, BIT4_X);
      ساختار propagate_vector4_event_s*cur
	    = جدید ساختار propagate_vector4_event_s(tmp);
      cur->net = net;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

تهی schedule_assign_array_word(vvp_array_t mem,
				unsigned word_addr,
				unsigned off,
				ثابت vvp_vector4_t&val,
				vvp_time64_t delay)
{
      ساختار assign_array_word_s*cur = جدید ساختار assign_array_word_s;
      cur->mem = mem;
      cur->adr = word_addr;
      cur->off = off;
      cur->val = val;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

تهی schedule_assign_array_word(vvp_array_t mem,
				unsigned word_addr,
				دوگانه val,
				vvp_time64_t delay)
{
      ساختار assign_array_r_word_s*cur = جدید ساختار assign_array_r_word_s;
      cur->mem = mem;
      cur->adr = word_addr;
      cur->val = val;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

تهی schedule_set_vector(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit)
{
      ساختار assign_vector4_event_s*cur = جدید ساختار assign_vector4_event_s(bit);
      cur->ptr = ptr;
      cur->base = 0;
      cur->vwid = 0;
      schedule_event_(cur, 0, SEQ_ACTIVE);
}

تهی schedule_set_vector(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&bit)
{
      ساختار assign_vector8_event_s*cur = جدید ساختار assign_vector8_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_event_(cur, 0, SEQ_ACTIVE);
}

تهی schedule_set_vector(vvp_net_ptr_t ptr, دوگانه bit)
{
      ساختار assign_real_event_s*cur = جدید ساختار assign_real_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_event_(cur, 0, SEQ_ACTIVE);
}

تهی schedule_init_vector(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit)
{
      ساختار assign_vector4_event_s*cur = جدید ساختار assign_vector4_event_s(bit);
      cur->ptr = ptr;
      cur->base = 0;
      cur->vwid = 0;
      schedule_init_event(cur);
}

تهی schedule_init_vector(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&bit)
{
      ساختار assign_vector8_event_s*cur = جدید ساختار assign_vector8_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_init_event(cur);
}

تهی schedule_init_vector(vvp_net_ptr_t ptr, دوگانه bit)
{
      ساختار assign_real_event_s*cur = جدید ساختار assign_real_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_init_event(cur);
}

تهی schedule_init_propagate(vvp_net_t*net, vvp_vector4_t bit)
{
      ساختار propagate_vector4_event_s*cur = جدید ساختار propagate_vector4_event_s(bit);
      cur->net = net;
      schedule_init_event(cur);
}

تهی schedule_init_propagate(vvp_net_t*net, دوگانه bit)
{
      ساختار propagate_real_event_s*cur = جدید ساختار propagate_real_event_s;
      cur->net = net;
      cur->val = bit;
      schedule_init_event(cur);
}

تهی schedule_del_thr(vthread_t thr)
{
      ساختار del_thr_event_s*cur = جدید del_thr_event_s;

      cur->thr = thr;

      schedule_event_(cur, 0, DEL_THREAD);
}

تهی schedule_generic(vvp_gen_event_t obj, vvp_time64_t delay,
		      منطقی sync_flag, منطقی ro_flag, منطقی delete_when_done)
{
      ساختار generic_event_s*cur = جدید generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = delete_when_done;
      schedule_event_(cur, delay,
		      sync_flag? (ro_flag?SEQ_ROSYNC:SEQ_RWSYNC) : SEQ_ACTIVE);

      اگر (sync_flag)
	    vthread_delay_delete();
}

ایستا منطقی sim_started;

تهی schedule_functor(vvp_gen_event_t obj)
{
      ساختار generic_event_s*cur = جدید generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = نادرست;
      اگر (!sim_started) {
            schedule_init_event(cur);
      } وگرنه {
            schedule_event_(cur, 0, SEQ_ACTIVE);
      }
}

تهی schedule_at_start_of_simtime(vvp_gen_event_t obj, vvp_time64_t delay)
{
      ساختار generic_event_s*cur = جدید generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = نادرست;
      schedule_event_(cur, delay, SEQ_START);
}

/*
 * In the vvp runtime of Icarus Verilog, the SEQ_RWSYNC time step is
 * after all of the non-blocking assignments, so is effectively the
 * same as the ReadWriteSync time.
 */
تهی schedule_at_end_of_simtime(vvp_gen_event_t obj, vvp_time64_t delay)
{
      ساختار generic_event_s*cur = جدید generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = نادرست;
      schedule_event_(cur, delay, SEQ_RWSYNC);
}

ایستا vvp_time64_t schedule_time;
vvp_time64_t schedule_simtime(تهی)
{ بازگشت schedule_time; }

ایستا منطقی sim_at_rosync = نادرست;
منطقی schedule_at_rosync(تهی)
{ بازگشت sim_at_rosync; }

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
ایستا تهی run_rosync(ساختار event_time_s*ctim)
{
      sim_at_rosync = درست;
      وقتی (ctim->rosync) {
	    ساختار event_s*cur = ctim->rosync->next;
	    اگر (cur->next == cur) {
		  ctim->rosync = 0;
	    } وگرنه {
		  ctim->rosync->next = cur->next;
	    }

	    cur->run_run();
	    delete cur;
      }
      sim_at_rosync = نادرست;

      وقتی (ctim->del_thr) {
	    ساختار event_s*cur = ctim->del_thr->next;
	    اگر (cur->next == cur) {
		  ctim->del_thr = 0;
	    } وگرنه {
		  ctim->del_thr->next = cur->next;
	    }

	    cur->run_run();
	    delete cur;
      }

      اگر (ctim->active || ctim->inactive || ctim->nbassign || ctim->rwsync) {
	    cerr << "SCHEDULER ERROR: read-only sync events "
		 << "created RW events!" << endl;
      }
}

تهی schedule_simulate(تهی)
{
      منطقی run_finals;
      sim_started = نادرست;

      schedule_time = 0;

      اگر (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute EndOfCompile callbacks\n");
      }

      // Execute end of compile callbacks
      vpiEndOfCompile();

      اگر (verbose_flag) {
	    vpi_mcd_printf(1, " ...propagate initialization events\n");
      }

	// Execute initialization events.
      وقتی (schedule_init_list) {
	    ساختار event_s*cur = schedule_init_list->next;
	    اگر (cur->next == cur) {
		  schedule_init_list = 0;
	    } وگرنه {
		  schedule_init_list->next = cur->next;
	    }
	    cur->run_run();
	    delete cur;
      }

      sim_started = درست;

      اگر (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute StartOfSim callbacks\n");
      }

      // Execute start of simulation callbacks
      vpiStartOfSim();

      signals_capture();

      اگر (verbose_flag) {
	    vpi_mcd_printf(1, " ...run scheduler\n");
      }

      // If there were no compiletf, etc. errors then we are going to
      // process events and when done run the final blocks.
      run_finals = schedule_runnable;

      اگر (schedule_runnable) وقتی (sched_list) {

	    اگر (schedule_stopped_flag) {
		  schedule_stopped_flag = نادرست;
		  stop_handler(0);
		  // You can finish from the debugger without a time change.
		  اگر (!schedule_runnable) بشکن;
		  ادامه;
	    }

	      /* ctim is the current time step. */
	    ساختار event_time_s* ctim = sched_list;

	      /* If the time is advancing, then first run the
		 postponed sync events. Run them all. */
	    اگر (ctim->delay > 0) {

		  اگر (!schedule_runnable) بشکن;
		  schedule_time += ctim->delay;
		    /* When the design is being traced (we are emitting
		     * file/line information) also print any time changes. */
		  اگر (show_file_سطر) {
			cerr << "Advancing to simulation time: "
			     << schedule_time << endl;
		  }
		  ctim->delay = 0;

		  vpiNextSimTime();
		    // Process the cbAtStartOfSimTime callbacks.
		  وقتی (ctim->start) {
			ساختار event_s*cur = ctim->start->next;
			اگر (cur->next == cur) {
			      ctim->start = 0;
			} وگرنه {
			      ctim->start->next = cur->next;
			}
			cur->run_run();
			delete (cur);
		  }
	    }


	      /* If there are no more active events, advance the event
		 queues. If there are not events at all, then release
		 the event_time object. */
	    اگر (ctim->active == 0) {
		  ctim->active = ctim->inactive;
		  ctim->inactive = 0;

		  اگر (ctim->active == 0) {
			ctim->active = ctim->nbassign;
			ctim->nbassign = 0;

			اگر (ctim->active == 0) {
			      ctim->active = ctim->rwsync;
			      ctim->rwsync = 0;

				/* If out of rw events, then run the rosync
				   events and delete this time step. This also
				   deletes threads as needed. */
			      اگر (ctim->active == 0) {
				    run_rosync(ctim);
				    sched_list = ctim->next;
				    delete ctim;
				    ادامه;
			      }
			}
		  }
	    }

	      /* Pull the first item off the list. If this is the last
		 cell in the list, then clear the list. Execute that
		 event type, and delete it. */
	    ساختار event_s*cur = ctim->active->next;
	    اگر (cur->next == cur) {
		  ctim->active = 0;
	    } وگرنه {
		  ctim->active->next = cur->next;
	    }

	    اگر (schedule_single_step_flag) {
		  cur->single_step_display();
		  schedule_stopped_flag = درست;
		  schedule_single_step_flag = نادرست;
	    }

	    cur->run_run();

	    delete (cur);
      }

	// Execute final events.
      schedule_runnable = run_finals;
      وقتی (schedule_runnable && schedule_final_list) {
	    ساختار event_s*cur = schedule_final_list->next;
	    اگر (cur->next == cur) {
		  schedule_final_list = 0;
	    } وگرنه {
		  schedule_final_list->next = cur->next;
	    }
	    cur->run_run();
	    delete cur;
      }

      signals_revert();

      اگر (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute Postsim callbacks\n");
      }

      // Execute post-simulation callbacks
      vpiPostsim();
#ifdef CHECK_WITH_VALGRIND
      schedule_delete();
#endif
}

#ifdef CHECK_WITH_VALGRIND
تهی schedule_delete(تهی)
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
