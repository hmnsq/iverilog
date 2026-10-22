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

using नामाकाश std;

unsigned long मोजणी_assign_events = 0;
unsigned long मोजणी_gen_events = 0;
unsigned long मोजणी_thread_events = 0;
  // Count the time events (A time cell created)
unsigned long मोजणी_time_events = 0;



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
संरचना event_s {
      संरचना event_s*next;
      आभास ~event_s() { }
      आभास शून्य run_run(शून्य) =0;

	// Write something about the event to stderr
      आभास शून्य single_step_display(शून्य);

	// Fallback new/delete
      स्थिर शून्य*operator नूतन (size_t size) { प्रत्यागम ::नूतन वर्ण[size]; }
      स्थिर शून्य operator delete(शून्य*ptr)  { ::delete[]( static_cast<वर्ण*>(ptr) ); }
};

शून्य event_s::single_step_display(शून्य)
{
      std::cerr << "event_s: Step into event " << typeid(*एतत्).name() << std::endl;
}

संरचना event_time_s {
      event_time_s() {
	    मोजणी_time_events += 1;
	    delay = 0;
	    start = 0;
	    active = 0;
	    inactive = 0;
	    nbassign = 0;
	    rwsync = 0;
	    rosync = 0;
	    del_thr = 0;
	    next = रिक्त;
      }
      vvp_time64_t delay;

      संरचना event_s*start;
      संरचना event_s*active;
      संरचना event_s*inactive;
      संरचना event_s*nbassign;
      संरचना event_s*rwsync;
      संरचना event_s*rosync;
      संरचना event_s*del_thr;

      संरचना event_time_s*next;

      स्थिर शून्य* operator नूतन (size_t);
      स्थिर शून्य operator delete(शून्य*obj, size_t s);
};

vvp_gen_event_s::~vvp_gen_event_s()
{
}

शून्य vvp_gen_event_s::single_step_display(शून्य)
{
      cerr << "vvp_gen_event_s: Step into event " << typeid(*एतत्).name() << endl;
}

/*
 * Derived event types
 */
संरचना vthread_event_s : सार्वजनिक event_s {
      vthread_t thr;
      शून्य run_run(शून्य) override;
      शून्य single_step_display(शून्य) override;

      स्थिर शून्य* operator नूतन(size_t);
      स्थिर शून्य operator delete(शून्य*);
};

शून्य vthread_event_s::run_run(शून्य)
{
      मोजणी_thread_events += 1;
      vthread_run(thr);
}

शून्य vthread_event_s::single_step_display(शून्य)
{
      __vpiScope*scope = vthread_scope(thr);
      cerr << "vthread_event: Resume thread"
	   << " scope=" << scope->vpi_get_str(vpiFullName)
	   << endl;
}

स्थिर ध्रुव size_t VTHR_CHUNK_मोजणी = 8192 / परिमाण(संरचना vthread_event_s);
स्थिर slab_t<परिमाण(vthread_event_s),VTHR_CHUNK_मोजणी> vthread_event_heap;

अन्तःस्थ शून्य* vthread_event_s::operator नूतन(size_t size)
{
      प्रतिज्ञा(size == परिमाण(vthread_event_s));
      प्रत्यागम vthread_event_heap.alloc_slab();
}

शून्य vthread_event_s::operator delete(शून्य*dptr)
{
      vthread_event_heap.free_slab(dptr);
}

संरचना del_thr_event_s : सार्वजनिक event_s {
      vthread_t thr;
      शून्य run_run(शून्य) override;
      शून्य single_step_display(शून्य) override;
};

शून्य del_thr_event_s::run_run(शून्य)
{
      vthread_delete(thr);
}

शून्य del_thr_event_s::single_step_display(शून्य)
{
      __vpiScope*scope = vthread_scope(thr);
      cerr << "del_thr_event: Reap completed thread"
	   << " scope=" << scope->vpi_get_str(vpiFullName) << endl;
}

संरचना assign_vector4_event_s  : सार्वजनिक event_s {
	/* The default constructor. */
      explicit assign_vector4_event_s(ध्रुव vvp_vector4_t&that) : val(that) {
	    base = 0;
	    vwid = 0;
	    next = रिक्त;
      }

	/* Where to do the assign. */
      vvp_net_ptr_t ptr;
	/* Value to assign. */
      vvp_vector4_t val;
	/* Offset of the part into the destination. */
      unsigned base;
	/* Width of the destination vector. */
      unsigned vwid;
      शून्य run_run(शून्य) override;
      शून्य single_step_display(शून्य) override;

      स्थिर शून्य* operator नूतन(size_t);
      स्थिर शून्य operator delete(शून्य*);
};

शून्य assign_vector4_event_s::run_run(शून्य)
{
      मोजणी_assign_events += 1;
      यदि (vwid > 0)
	    vvp_send_vec4_pv(ptr, val, base, vwid, 0);
      अन्यथा
	    vvp_send_vec4(ptr, val, 0);
}

शून्य assign_vector4_event_s::single_step_display(शून्य)
{
      cerr << "assign_vector4_event: Propagate val=" << val
	   << ", vwid=" << vwid << ", base=" << base << endl;
}

स्थिर ध्रुव size_t ASSIGN4_CHUNK_मोजणी = 524288 / परिमाण(संरचना assign_vector4_event_s);
स्थिर slab_t<परिमाण(assign_vector4_event_s),ASSIGN4_CHUNK_मोजणी> assign4_heap;

अन्तःस्थ शून्य* assign_vector4_event_s::operator नूतन(size_t size)
{
      प्रतिज्ञा(size == परिमाण(assign_vector4_event_s));
      प्रत्यागम assign4_heap.alloc_slab();
}

शून्य assign_vector4_event_s::operator delete(शून्य*dptr)
{
      assign4_heap.free_slab(dptr);
}

unsigned long मोजणी_assign4_pool(शून्य) { प्रत्यागम assign4_heap.pool; }

संरचना assign_vector8_event_s  : सार्वजनिक event_s {
      vvp_net_ptr_t ptr;
      vvp_vector8_t val;
      शून्य run_run(शून्य) override;
      शून्य single_step_display(शून्य) override;

      स्थिर शून्य* operator नूतन(size_t);
      स्थिर शून्य operator delete(शून्य*);
};

शून्य assign_vector8_event_s::run_run(शून्य)
{
      मोजणी_assign_events += 1;
      vvp_send_vec8(ptr, val);
}

शून्य assign_vector8_event_s::single_step_display(शून्य)
{
      cerr << "assign_vector8_event: Propagate val=" << val << endl;
}

स्थिर ध्रुव size_t ASSIGN8_CHUNK_मोजणी = 8192 / परिमाण(संरचना assign_vector8_event_s);
स्थिर slab_t<परिमाण(assign_vector8_event_s),ASSIGN8_CHUNK_मोजणी> assign8_heap;

अन्तःस्थ शून्य* assign_vector8_event_s::operator नूतन(size_t size)
{
      प्रतिज्ञा(size == परिमाण(assign_vector8_event_s));
      प्रत्यागम assign8_heap.alloc_slab();
}

शून्य assign_vector8_event_s::operator delete(शून्य*dptr)
{
      assign8_heap.free_slab(dptr);
}

unsigned long मोजणी_assign8_pool() { प्रत्यागम assign8_heap.pool; }

संरचना assign_real_event_s  : सार्वजनिक event_s {
      vvp_net_ptr_t ptr;
      द्विगुण val;
      शून्य run_run(शून्य) override;
      शून्य single_step_display(शून्य) override;

      स्थिर शून्य* operator नूतन(size_t);
      स्थिर शून्य operator delete(शून्य*);
};

शून्य assign_real_event_s::run_run(शून्य)
{
      मोजणी_assign_events += 1;
      vvp_send_real(ptr, val, 0);
}

शून्य assign_real_event_s::single_step_display(शून्य)
{
      cerr << "assign_real_event: Propagate val=" << val << endl;
}

स्थिर ध्रुव size_t ASSIGNR_CHUNK_मोजणी = 8192 / परिमाण(संरचना assign_real_event_s);
स्थिर slab_t<परिमाण(assign_real_event_s),ASSIGNR_CHUNK_मोजणी> assignr_heap;

अन्तःस्थ शून्य* assign_real_event_s::operator नूतन (size_t size)
{
      प्रतिज्ञा(size == परिमाण(assign_real_event_s));
      प्रत्यागम assignr_heap.alloc_slab();
}

शून्य assign_real_event_s::operator delete(शून्य*dptr)
{
      assignr_heap.free_slab(dptr);
}

unsigned long मोजणी_assign_real_pool(शून्य) { प्रत्यागम assignr_heap.pool; }

संरचना assign_array_word_s  : सार्वजनिक event_s {
      vvp_array_t mem;
      unsigned adr;
      vvp_vector4_t val;
      unsigned off;
      शून्य run_run(शून्य) override;

      स्थिर शून्य* operator नूतन(size_t);
      स्थिर शून्य operator delete(शून्य*);
};

शून्य assign_array_word_s::run_run(शून्य)
{
      मोजणी_assign_events += 1;
      mem->set_word(adr, off, val);
}

स्थिर ध्रुव size_t ARRAY_W_CHUNK_मोजणी = 8192 / परिमाण(संरचना assign_array_word_s);
स्थिर slab_t<परिमाण(assign_array_word_s),ARRAY_W_CHUNK_मोजणी> array_w_heap;

अन्तःस्थ शून्य* assign_array_word_s::operator नूतन (size_t size)
{
      प्रतिज्ञा(size == परिमाण(assign_array_word_s));
      प्रत्यागम array_w_heap.alloc_slab();
}

शून्य assign_array_word_s::operator delete(शून्य*ptr)
{
      array_w_heap.free_slab(ptr);
}

unsigned long मोजणी_assign_aword_pool(शून्य) { प्रत्यागम array_w_heap.pool; }

संरचना force_vector4_event_s  : सार्वजनिक event_s {
	/* The default constructor. */
      explicit force_vector4_event_s(ध्रुव vvp_vector4_t&that): val(that) {
	    net = रिक्त;
	    base = 0;
	    vwid = 0;
	    next = रिक्त;
      }
	/* Where to do the force. */
      vvp_net_t*net;
	/* Value to force. */
      vvp_vector4_t val;
	/* Offset of the part into the destination. */
      unsigned base;
	/* Width of the destination vector. */
      unsigned vwid;

      शून्य run_run(शून्य) override;
      शून्य single_step_display(शून्य) override;

      स्थिर शून्य* operator नूतन(size_t);
      स्थिर शून्य operator delete(शून्य*);
};

शून्य force_vector4_event_s::run_run(शून्य)
{
      मोजणी_assign_events += 1;

      unsigned wid = val.size();
      यदि ((base + wid) > vwid)
	    wid = vwid - base;

	// Make a mask of which bits are to be forced, 0 for unforced
	// bits and 1 for forced bits.
      vvp_vector2_t mask (vvp_vector2_t::FILL0, vwid);
      प्रति (unsigned idx = 0 ; idx < wid ; idx += 1)
	    mask.set_bit(base+idx, 1);

      vvp_vector4_t tmp (vwid, BIT4_Z);

	// vvp_net_t::force_vec4 propagates all the bits of the
	// forced vector value, regardless of the mask. This
	// ensures the unforced bits retain their current value.
      vvp_signal_मूल्य*sig = dynamic_cast<vvp_signal_मूल्य*>(net->fil);
      प्रतिज्ञा(sig);
      sig->vec4_मूल्य(tmp);

      tmp.set_vec(base, val);
      net->force_vec4(tmp, mask);
}

शून्य force_vector4_event_s::single_step_display(शून्य)
{
      cerr << "force_vector4_event: Force val=" << val
	   << ", vwid=" << vwid << ", base=" << base << endl;
}

स्थिर ध्रुव size_t FORCE4_CHUNK_मोजणी = 8192 / परिमाण(संरचना force_vector4_event_s);
स्थिर slab_t<परिमाण(force_vector4_event_s),FORCE4_CHUNK_मोजणी> force4_heap;

अन्तःस्थ शून्य* force_vector4_event_s::operator नूतन(size_t size)
{
      प्रतिज्ञा(size == परिमाण(force_vector4_event_s));
      प्रत्यागम force4_heap.alloc_slab();
}

शून्य force_vector4_event_s::operator delete(शून्य*dptr)
{
      force4_heap.free_slab(dptr);
}

unsigned long मोजणी_force4_pool(शून्य) { प्रत्यागम force4_heap.pool; }

/*
 * This class supports the propagation of vec4 outputs from a
 * vvp_net_t object.
 */
संरचना propagate_vector4_event_s : सार्वजनिक event_s {
	/* The default constructor. */
      explicit propagate_vector4_event_s(ध्रुव vvp_vector4_t&that) : val(that) {
	    net = रिक्त;
	    next = रिक्त;
      }
	/* A constructor that makes the val directly. */
      propagate_vector4_event_s(ध्रुव vvp_vector4_t&that, unsigned adr, unsigned wid)
      : val(that,adr,wid) {
	    net = रिक्त;
	    next = रिक्त;
      }

	/* Propagate the output of this net. */
      vvp_net_t*net;
	/* value to propagate */
      vvp_vector4_t val;
	/* Action */
      शून्य run_run(शून्य) override;
      शून्य single_step_display(शून्य) override;
};

शून्य propagate_vector4_event_s::run_run(शून्य)
{
      net->send_vec4(val, 0);
}

शून्य propagate_vector4_event_s::single_step_display(शून्य)
{
      cerr << "propagate_vector4_event: Propagate val=" << val << endl;
}

/*
 * This class supports the propagation of real outputs from a
 * vvp_net_t object.
 */
संरचना propagate_real_event_s : सार्वजनिक event_s {
	/* Propagate the output of this net. */
      vvp_net_t*net;
	/* value to propagate */
      द्विगुण val;
	/* Action */
      शून्य run_run(शून्य) override;
      शून्य single_step_display(शून्य) override;
};

शून्य propagate_real_event_s::run_run(शून्य)
{
      net->send_real(val, 0);
}

शून्य propagate_real_event_s::single_step_display(शून्य)
{
      cerr << "propagate_real_event: Propagate val=" << val << endl;
}

संरचना assign_array_r_word_s  : सार्वजनिक event_s {
      vvp_array_t mem;
      unsigned adr;
      द्विगुण val;
      शून्य run_run(शून्य) override;

      स्थिर शून्य* operator नूतन(size_t);
      स्थिर शून्य operator delete(शून्य*);
};

शून्य assign_array_r_word_s::run_run(शून्य)
{
      मोजणी_assign_events += 1;
      mem->set_word(adr, val);
}
स्थिर ध्रुव size_t ARRAY_R_W_CHUNK_मोजणी = 8192 / परिमाण(संरचना assign_array_r_word_s);
स्थिर slab_t<परिमाण(assign_array_r_word_s),ARRAY_R_W_CHUNK_मोजणी> array_r_w_heap;

अन्तःस्थ शून्य* assign_array_r_word_s::operator नूतन(size_t size)
{
      प्रतिज्ञा(size == परिमाण(assign_array_r_word_s));
      प्रत्यागम array_r_w_heap.alloc_slab();
}

शून्य assign_array_r_word_s::operator delete(शून्य*ptr)
{
      array_r_w_heap.free_slab(ptr);
}

unsigned long मोजणी_assign_arword_pool(शून्य) { प्रत्यागम array_r_w_heap.pool; }

संरचना generic_event_s : सार्वजनिक event_s {
      vvp_gen_event_t obj;
      तार्किक delete_obj_when_done;
      शून्य run_run(शून्य) override;
      शून्य single_step_display(शून्य) override;

      स्थिर शून्य* operator नूतन(size_t);
      स्थिर शून्य operator delete(शून्य*);
};

शून्य generic_event_s::run_run(शून्य)
{
      मोजणी_gen_events += 1;
      यदि (obj) {
	    obj->run_run();
	    यदि (delete_obj_when_done)
		  delete obj;
      }
}

शून्य generic_event_s::single_step_display(शून्य)
{
      obj->single_step_display();
}

स्थिर ध्रुव size_t GENERIC_CHUNK_मोजणी = 131072 / परिमाण(संरचना generic_event_s);
स्थिर slab_t<परिमाण(generic_event_s),GENERIC_CHUNK_मोजणी> generic_event_heap;

अन्तःस्थ शून्य* generic_event_s::operator नूतन(size_t size)
{
      प्रतिज्ञा(size == परिमाण(generic_event_s));
      प्रत्यागम generic_event_heap.alloc_slab();
}

शून्य generic_event_s::operator delete(शून्य*ptr)
{
      generic_event_heap.free_slab(ptr);
}

unsigned long मोजणी_gen_pool(शून्य) { प्रत्यागम generic_event_heap.pool; }

/*
** These event_time_s will be required a lot, at high frequency.
** Once allocated, we never free them, but stash them away for next time.
*/


स्थिर ध्रुव size_t TIME_CHUNK_मोजणी = 8192 / परिमाण(संरचना event_time_s);
स्थिर slab_t<परिमाण(event_time_s),TIME_CHUNK_मोजणी> event_time_heap;

अन्तःस्थ शून्य* event_time_s::operator नूतन (size_t size)
{
      प्रतिज्ञा(size == परिमाण(संरचना event_time_s));
      शून्य*ptr = event_time_heap.alloc_slab();
      प्रत्यागम ptr;
}

अन्तःस्थ शून्य event_time_s::operator delete(शून्य*ptr, size_t)
{
      event_time_heap.free_slab(ptr);
}

unsigned long मोजणी_time_pool(शून्य) { प्रत्यागम event_time_heap.pool; }

/*
 * This is the head of the list of pending events. This includes all
 * the events that have not been executed yet, and reaches into the
 * future.
 */
स्थिर संरचना event_time_s* sched_list = 0;

/*
 * This is a list of initialization events. The setup puts
 * initializations in this list so that they happen before the
 * simulation as a whole starts. This prevents time-0 triggers of
 * certain events.
 */
स्थिर संरचना event_s* schedule_init_list = 0;

/*
 * This is the head of the list of final events.
 */
स्थिर संरचना event_s* schedule_final_list = 0;

/*
 * This flag is true until a VPI task or function finishes the
 * simulation.
 */
स्थिर तार्किक schedule_runnable = सत्य;
स्थिर तार्किक schedule_stopped_flag  = असत्य;
स्थिर तार्किक schedule_single_step_flag = असत्य;
स्थिर तार्किक no_signals_flag = असत्य;

शून्य schedule_finish(पूर्णाङ्क)
{
      schedule_runnable = असत्य;
}

शून्य schedule_stop(पूर्णाङ्क)
{
      schedule_stopped_flag = सत्य;
}

शून्य schedule_single_step(पूर्णाङ्क)
{
      schedule_single_step_flag = सत्य;
}

तार्किक schedule_finished(शून्य)
{
      प्रत्यागम !schedule_runnable;
}

तार्किक schedule_stopped(शून्य)
{
      प्रत्यागम schedule_stopped_flag;
}

बाह्य "C" शून्य vvp_no_signals(शून्य)
{
      no_signals_flag = सत्य;
}

/*
 * These are the signal handling infrastructure. The SIGINT signal
 * leads to an implicit $stop. The SIGHUP and SIGTERM signals lead
 * to an implicit $finish.
 */
बाह्य तार्किक stop_is_finish;

बाह्य "C" शून्य signals_handler(पूर्णाङ्क signum)
{
#ifdef __MINGW32__
	// Windows implements the original UNIX semantics for signal,
	// so we have to re-establish the signal handler each time a
	// signal is caught.
      सङ्केत(signum, &signals_handler);
#endif
      यदि (signum != SIGINT)
	    stop_is_finish = सत्य;
      schedule_stopped_flag = सत्य;
}

स्थिर शून्य signals_capture(शून्य)
{
      यदि (no_signals_flag)
	   प्रत्यागम;
#ifndef __MINGW32__
      सङ्केत(SIGHUP,  &signals_handler);
#endif
      सङ्केत(SIGINT,  &signals_handler);
      सङ्केत(SIGTERM, &signals_handler);
}

स्थिर शून्य signals_revert(शून्य)
{
      यदि (no_signals_flag)
	   प्रत्यागम;
#ifndef __MINGW32__
      सङ्केत(SIGHUP,  SIG_DFL);
#endif
      सङ्केत(SIGINT,  SIG_DFL);
      सङ्केत(SIGTERM, SIG_DFL);
}

/*
 * This function puts an event on the end of the pre-simulation event queue.
 */
स्थिर शून्य schedule_init_event(संरचना event_s*cur)
{
      यदि (schedule_init_list == 0) {
            cur->next = cur;
      } अन्यथा {
            cur->next = schedule_init_list->next;
            schedule_init_list->next = cur;
      }
      schedule_init_list = cur;
}

/*
 * This function puts an event on the end of the post-simulation event queue.
 */
स्थिर शून्य schedule_final_event(संरचना event_s*cur)
{
      यदि (schedule_final_list == 0) {
            cur->next = cur;
      } अन्यथा {
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
प्रकारनिर्देश गणना event_queue_e { SEQ_START, SEQ_ACTIVE, SEQ_INACTIVE, SEQ_NBASSIGN,
			     SEQ_RWSYNC, SEQ_ROSYNC, DEL_THREAD } event_queue_t;

स्थिर शून्य schedule_event_(संरचना event_s*cur, vvp_time64_t delay,
			    event_queue_t select_queue)
{
      cur->next = cur;
      संरचना event_time_s*ctim = sched_list;

      यदि (sched_list == 0) {
	      /* Is the event_time list completely empty? Create the
		 first event_time object. */
	    ctim = नूतन संरचना event_time_s;
	    ctim->delay = delay;
	    ctim->next  = 0;
	    sched_list = ctim;

      } अन्यथा यदि (sched_list->delay > delay) {

	      /* Am I looking for an event before the first event_time?
		 If so, create a new event_time to go in front. */
	    संरचना event_time_s*tmp = नूतन संरचना event_time_s;
	    tmp->delay = delay;
	    tmp->next = ctim;
	    ctim->delay -= delay;
	    ctim = tmp;
	    sched_list = ctim;

      } अन्यथा {
	    संरचना event_time_s*prev = 0;

	    यावत् (ctim->next && (ctim->delay < delay)) {
		  delay -= ctim->delay;
		  prev = ctim;
		  ctim = ctim->next;
	    }

	    यदि (ctim->delay > delay) {
		  संरचना event_time_s*tmp = नूतन संरचना event_time_s;
		  tmp->delay = delay;
		  tmp->next  = prev->next;
		  prev->next = tmp;

		  tmp->next->delay -= delay;
		  ctim = tmp;

	    } अन्यथा यदि (ctim->delay == delay) {

	    } अन्यथा {
		  प्रतिज्ञा(ctim->next == 0);
		  संरचना event_time_s*tmp = नूतन संरचना event_time_s;
		  tmp->delay = delay - ctim->delay;
		  tmp->next = 0;
		  ctim->next = tmp;

		  ctim = tmp;
	    }
      }

	/* By this point, ctim is the event_time structure that is to
	   receive the event at hand. Put the event in to the
	   appropriate list for the kind of assign we have at hand. */

      संरचना event_s** q = 0;

      विकल्प (select_queue) {
	  पक्ष SEQ_START:
	    q = &ctim->start;
	    भङ्ग;

	  पक्ष SEQ_ACTIVE:
	    q = &ctim->active;
	    भङ्ग;

	  पक्ष SEQ_INACTIVE:
	    प्रतिज्ञा(delay == 0);
	    q = &ctim->inactive;
	    भङ्ग;

	  पक्ष SEQ_NBASSIGN:
	    q = &ctim->nbassign;
	    भङ्ग;

	  पक्ष SEQ_RWSYNC:
	    q = &ctim->rwsync;
	    भङ्ग;

	  पक्ष SEQ_ROSYNC:
	    q = &ctim->rosync;
	    भङ्ग;

	  पक्ष DEL_THREAD:
	    q = &ctim->del_thr;
	    भङ्ग;
      }

      यदि (q) {
	    यदि (*q) {
		  /* Put the cur event on the end of the queue. */
		  cur->next = (*q)->next;
		  (*q)->next = cur;
	    }
	    *q = cur;
      }
}

स्थिर शून्य schedule_event_ढकला(संरचना event_s*cur)
{
      यदि ((sched_list == 0) || (sched_list->delay > 0)) {
	    schedule_event_(cur, 0, SEQ_ACTIVE);
	    प्रत्यागम;
      }

      संरचना event_time_s*ctim = sched_list;

      यदि (ctim->active == 0) {
	    cur->next = cur;
	    ctim->active = cur;
	    प्रत्यागम;
      }

      cur->next = ctim->active->next;
      ctim->active->next = cur;
}

शून्य schedule_vthread(vthread_t thr, vvp_time64_t delay, तार्किक ढकला_flag)
{
      संरचना vthread_event_s*cur = नूतन vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);

      यदि (ढकला_flag && (delay == 0)) {
	      /* Special case: If the delay is 0, the push_flag means
		 I can push this event in front of everything. This is
		 used by the %fork statement, for example, to perform
		 task calls. */
	    schedule_event_ढकला(cur);

      } अन्यथा {
	    schedule_event_(cur, delay, SEQ_ACTIVE);
      }
}

शून्य schedule_t0_trigger(vvp_net_ptr_t ptr)
{
      vvp_vector4_t bit (1, BIT4_X);
      संरचना assign_vector4_event_s*cur = नूतन संरचना assign_vector4_event_s(bit);
      cur->ptr = ptr;
      schedule_event_(cur, 0, SEQ_INACTIVE);
}

शून्य schedule_inactive(vthread_t thr)
{
      संरचना vthread_event_s*cur = नूतन vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);
      schedule_event_(cur, 0, SEQ_INACTIVE);
}

शून्य schedule_init_vthread(vthread_t thr)
{
      संरचना vthread_event_s*cur = नूतन vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);

      schedule_init_event(cur);
}

शून्य schedule_final_vthread(vthread_t thr)
{
      संरचना vthread_event_s*cur = नूतन vthread_event_s;

      cur->thr = thr;
      vthread_mark_final(thr);
      vthread_mark_scheduled(thr);

      schedule_final_event(cur);
}

शून्य schedule_assign_vector(vvp_net_ptr_t ptr,
			    unsigned base, unsigned vwid,
			    ध्रुव vvp_vector4_t&bit,
			    vvp_time64_t delay)
{
      संरचना assign_vector4_event_s*cur = नूतन संरचना assign_vector4_event_s(bit);
      cur->ptr = ptr;
      cur->base = base;
      cur->vwid = vwid;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

शून्य schedule_force_vector(vvp_net_t*net,
			    unsigned base, unsigned vwid,
			    ध्रुव vvp_vector4_t&bit,
			    vvp_time64_t delay)
{
      संरचना force_vector4_event_s*cur = नूतन संरचना force_vector4_event_s(bit);
      cur->net = net;
      cur->base = base;
      cur->vwid = vwid;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

शून्य schedule_propagate_vector(vvp_net_t*net,
			       vvp_time64_t delay,
			       ध्रुव vvp_vector4_t&src)
{
      संरचना propagate_vector4_event_s*cur
	    = नूतन संरचना propagate_vector4_event_s(src);
      cur->net = net;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

// FIXME: This needs to create a non-blocking event, but only one per time slot.
//        Is schedule_event_ or execution actually filtering since the net is
//        already X because this is not triggering?
शून्य schedule_propagate_event(vvp_net_t*net,
                              vvp_time64_t delay)
{
      vvp_vector4_t tmp (1, BIT4_X);
      संरचना propagate_vector4_event_s*cur
	    = नूतन संरचना propagate_vector4_event_s(tmp);
      cur->net = net;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

शून्य schedule_assign_array_word(vvp_array_t mem,
				unsigned word_addr,
				unsigned off,
				ध्रुव vvp_vector4_t&val,
				vvp_time64_t delay)
{
      संरचना assign_array_word_s*cur = नूतन संरचना assign_array_word_s;
      cur->mem = mem;
      cur->adr = word_addr;
      cur->off = off;
      cur->val = val;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

शून्य schedule_assign_array_word(vvp_array_t mem,
				unsigned word_addr,
				द्विगुण val,
				vvp_time64_t delay)
{
      संरचना assign_array_r_word_s*cur = नूतन संरचना assign_array_r_word_s;
      cur->mem = mem;
      cur->adr = word_addr;
      cur->val = val;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

शून्य schedule_set_vector(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit)
{
      संरचना assign_vector4_event_s*cur = नूतन संरचना assign_vector4_event_s(bit);
      cur->ptr = ptr;
      cur->base = 0;
      cur->vwid = 0;
      schedule_event_(cur, 0, SEQ_ACTIVE);
}

शून्य schedule_set_vector(vvp_net_ptr_t ptr, ध्रुव vvp_vector8_t&bit)
{
      संरचना assign_vector8_event_s*cur = नूतन संरचना assign_vector8_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_event_(cur, 0, SEQ_ACTIVE);
}

शून्य schedule_set_vector(vvp_net_ptr_t ptr, द्विगुण bit)
{
      संरचना assign_real_event_s*cur = नूतन संरचना assign_real_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_event_(cur, 0, SEQ_ACTIVE);
}

शून्य schedule_init_vector(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit)
{
      संरचना assign_vector4_event_s*cur = नूतन संरचना assign_vector4_event_s(bit);
      cur->ptr = ptr;
      cur->base = 0;
      cur->vwid = 0;
      schedule_init_event(cur);
}

शून्य schedule_init_vector(vvp_net_ptr_t ptr, ध्रुव vvp_vector8_t&bit)
{
      संरचना assign_vector8_event_s*cur = नूतन संरचना assign_vector8_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_init_event(cur);
}

शून्य schedule_init_vector(vvp_net_ptr_t ptr, द्विगुण bit)
{
      संरचना assign_real_event_s*cur = नूतन संरचना assign_real_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_init_event(cur);
}

शून्य schedule_init_propagate(vvp_net_t*net, vvp_vector4_t bit)
{
      संरचना propagate_vector4_event_s*cur = नूतन संरचना propagate_vector4_event_s(bit);
      cur->net = net;
      schedule_init_event(cur);
}

शून्य schedule_init_propagate(vvp_net_t*net, द्विगुण bit)
{
      संरचना propagate_real_event_s*cur = नूतन संरचना propagate_real_event_s;
      cur->net = net;
      cur->val = bit;
      schedule_init_event(cur);
}

शून्य schedule_del_thr(vthread_t thr)
{
      संरचना del_thr_event_s*cur = नूतन del_thr_event_s;

      cur->thr = thr;

      schedule_event_(cur, 0, DEL_THREAD);
}

शून्य schedule_generic(vvp_gen_event_t obj, vvp_time64_t delay,
		      तार्किक sync_flag, तार्किक ro_flag, तार्किक delete_when_done)
{
      संरचना generic_event_s*cur = नूतन generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = delete_when_done;
      schedule_event_(cur, delay,
		      sync_flag? (ro_flag?SEQ_ROSYNC:SEQ_RWSYNC) : SEQ_ACTIVE);

      यदि (sync_flag)
	    vthread_delay_delete();
}

स्थिर तार्किक sim_started;

शून्य schedule_functor(vvp_gen_event_t obj)
{
      संरचना generic_event_s*cur = नूतन generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = असत्य;
      यदि (!sim_started) {
            schedule_init_event(cur);
      } अन्यथा {
            schedule_event_(cur, 0, SEQ_ACTIVE);
      }
}

शून्य schedule_at_start_of_simtime(vvp_gen_event_t obj, vvp_time64_t delay)
{
      संरचना generic_event_s*cur = नूतन generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = असत्य;
      schedule_event_(cur, delay, SEQ_START);
}

/*
 * In the vvp runtime of Icarus Verilog, the SEQ_RWSYNC time step is
 * after all of the non-blocking assignments, so is effectively the
 * same as the ReadWriteSync time.
 */
शून्य schedule_at_end_of_simtime(vvp_gen_event_t obj, vvp_time64_t delay)
{
      संरचना generic_event_s*cur = नूतन generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = असत्य;
      schedule_event_(cur, delay, SEQ_RWSYNC);
}

स्थिर vvp_time64_t schedule_time;
vvp_time64_t schedule_simtime(शून्य)
{ प्रत्यागम schedule_time; }

स्थिर तार्किक sim_at_rosync = असत्य;
तार्किक schedule_at_rosync(शून्य)
{ प्रत्यागम sim_at_rosync; }

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
स्थिर शून्य run_rosync(संरचना event_time_s*ctim)
{
      sim_at_rosync = सत्य;
      यावत् (ctim->rosync) {
	    संरचना event_s*cur = ctim->rosync->next;
	    यदि (cur->next == cur) {
		  ctim->rosync = 0;
	    } अन्यथा {
		  ctim->rosync->next = cur->next;
	    }

	    cur->run_run();
	    delete cur;
      }
      sim_at_rosync = असत्य;

      यावत् (ctim->del_thr) {
	    संरचना event_s*cur = ctim->del_thr->next;
	    यदि (cur->next == cur) {
		  ctim->del_thr = 0;
	    } अन्यथा {
		  ctim->del_thr->next = cur->next;
	    }

	    cur->run_run();
	    delete cur;
      }

      यदि (ctim->active || ctim->inactive || ctim->nbassign || ctim->rwsync) {
	    cerr << "SCHEDULER ERROR: read-only sync events "
		 << "created RW events!" << endl;
      }
}

शून्य schedule_simulate(शून्य)
{
      तार्किक run_finals;
      sim_started = असत्य;

      schedule_time = 0;

      यदि (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute EndOfCompile callbacks\n");
      }

      // Execute end of compile callbacks
      vpiEndOfCompile();

      यदि (verbose_flag) {
	    vpi_mcd_printf(1, " ...propagate initialization events\n");
      }

	// Execute initialization events.
      यावत् (schedule_init_list) {
	    संरचना event_s*cur = schedule_init_list->next;
	    यदि (cur->next == cur) {
		  schedule_init_list = 0;
	    } अन्यथा {
		  schedule_init_list->next = cur->next;
	    }
	    cur->run_run();
	    delete cur;
      }

      sim_started = सत्य;

      यदि (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute StartOfSim callbacks\n");
      }

      // Execute start of simulation callbacks
      vpiStartOfSim();

      signals_capture();

      यदि (verbose_flag) {
	    vpi_mcd_printf(1, " ...run scheduler\n");
      }

      // If there were no compiletf, etc. errors then we are going to
      // process events and when done run the final blocks.
      run_finals = schedule_runnable;

      यदि (schedule_runnable) यावत् (sched_list) {

	    यदि (schedule_stopped_flag) {
		  schedule_stopped_flag = असत्य;
		  stop_handler(0);
		  // You can finish from the debugger without a time change.
		  यदि (!schedule_runnable) भङ्ग;
		  अनुवर्त;
	    }

	      /* ctim is the current time step. */
	    संरचना event_time_s* ctim = sched_list;

	      /* If the time is advancing, then first run the
		 postponed sync events. Run them all. */
	    यदि (ctim->delay > 0) {

		  यदि (!schedule_runnable) भङ्ग;
		  schedule_time += ctim->delay;
		    /* When the design is being traced (we are emitting
		     * file/line information) also print any time changes. */
		  यदि (show_file_ओळ) {
			cerr << "Advancing to simulation time: "
			     << schedule_time << endl;
		  }
		  ctim->delay = 0;

		  vpiNextSimTime();
		    // Process the cbAtStartOfSimTime callbacks.
		  यावत् (ctim->start) {
			संरचना event_s*cur = ctim->start->next;
			यदि (cur->next == cur) {
			      ctim->start = 0;
			} अन्यथा {
			      ctim->start->next = cur->next;
			}
			cur->run_run();
			delete (cur);
		  }
	    }


	      /* If there are no more active events, advance the event
		 queues. If there are not events at all, then release
		 the event_time object. */
	    यदि (ctim->active == 0) {
		  ctim->active = ctim->inactive;
		  ctim->inactive = 0;

		  यदि (ctim->active == 0) {
			ctim->active = ctim->nbassign;
			ctim->nbassign = 0;

			यदि (ctim->active == 0) {
			      ctim->active = ctim->rwsync;
			      ctim->rwsync = 0;

				/* If out of rw events, then run the rosync
				   events and delete this time step. This also
				   deletes threads as needed. */
			      यदि (ctim->active == 0) {
				    run_rosync(ctim);
				    sched_list = ctim->next;
				    delete ctim;
				    अनुवर्त;
			      }
			}
		  }
	    }

	      /* Pull the first item off the list. If this is the last
		 cell in the list, then clear the list. Execute that
		 event type, and delete it. */
	    संरचना event_s*cur = ctim->active->next;
	    यदि (cur->next == cur) {
		  ctim->active = 0;
	    } अन्यथा {
		  ctim->active->next = cur->next;
	    }

	    यदि (schedule_single_step_flag) {
		  cur->single_step_display();
		  schedule_stopped_flag = सत्य;
		  schedule_single_step_flag = असत्य;
	    }

	    cur->run_run();

	    delete (cur);
      }

	// Execute final events.
      schedule_runnable = run_finals;
      यावत् (schedule_runnable && schedule_final_list) {
	    संरचना event_s*cur = schedule_final_list->next;
	    यदि (cur->next == cur) {
		  schedule_final_list = 0;
	    } अन्यथा {
		  schedule_final_list->next = cur->next;
	    }
	    cur->run_run();
	    delete cur;
      }

      signals_revert();

      यदि (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute Postsim callbacks\n");
      }

      // Execute post-simulation callbacks
      vpiPostsim();
#ifdef CHECK_WITH_VALGRIND
      schedule_delete();
#endif
}

#ifdef CHECK_WITH_VALGRIND
शून्य schedule_delete(शून्य)
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
