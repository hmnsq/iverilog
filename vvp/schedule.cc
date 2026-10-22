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

using namespace std;

unsigned long count_assign_events = 0;
unsigned long count_gen_events = 0;
unsigned long count_thread_events = 0;
  // Count the time events (A time cell created)
unsigned long count_time_events = 0;



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
sa_nracanaa event_s {
      sa_nracanaa event_s*next;
      virtual ~event_s() { }
      virtual void run_run(void) =0;

	// Write something about the event to stderr
      virtual void single_step_display(void);

	// Fallback new/delete
      static void*operator new (size_t size) { ᬯᬮᬶ ::new var_na[size]; }
      static void operator delete(void*ptr)  { ::delete[]( static_cast<var_na*>(ptr) ); }
};

void event_s::single_step_display(void)
{
      std::cerr << "event_s: Step into event " << typeid(*this).name() << std::endl;
}

sa_nracanaa event_time_s {
      event_time_s() {
	    count_time_events += 1;
	    delay = 0;
	    start = 0;
	    active = 0;
	    inactive = 0;
	    nbassign = 0;
	    rwsync = 0;
	    rosync = 0;
	    del_thr = 0;
	    next = NULL;
      }
      vvp_time64_t delay;

      sa_nracanaa event_s*start;
      sa_nracanaa event_s*active;
      sa_nracanaa event_s*inactive;
      sa_nracanaa event_s*nbassign;
      sa_nracanaa event_s*rwsync;
      sa_nracanaa event_s*rosync;
      sa_nracanaa event_s*del_thr;

      sa_nracanaa event_time_s*next;

      static void* operator new (size_t);
      static void operator delete(void*obj, size_t s);
};

vvp_gen_event_s::~vvp_gen_event_s()
{
}

void vvp_gen_event_s::single_step_display(void)
{
      cerr << "vvp_gen_event_s: Step into event " << typeid(*this).name() << endl;
}

/*
 * Derived event types
 */
sa_nracanaa vthread_event_s : public event_s {
      vthread_t thr;
      void run_run(void) override;
      void single_step_display(void) override;

      static void* operator new(size_t);
      static void operator delete(void*);
};

void vthread_event_s::run_run(void)
{
      count_thread_events += 1;
      vthread_run(thr);
}

void vthread_event_s::single_step_display(void)
{
      __vpiScope*scope = vthread_scope(thr);
      cerr << "vthread_event: Resume thread"
	   << " scope=" << scope->vpi_get_str(vpiFullName)
	   << endl;
}

static const size_t VTHR_CHUNK_COUNT = 8192 / parimaa_na(sa_nracanaa vthread_event_s);
static slab_t<parimaa_na(vthread_event_s),VTHR_CHUNK_COUNT> vthread_event_heap;

anta_hstha void* vthread_event_s::operator new(size_t size)
{
      assert(size == parimaa_na(vthread_event_s));
      ᬯᬮᬶ vthread_event_heap.alloc_slab();
}

void vthread_event_s::operator delete(void*dptr)
{
      vthread_event_heap.free_slab(dptr);
}

sa_nracanaa del_thr_event_s : public event_s {
      vthread_t thr;
      void run_run(void) override;
      void single_step_display(void) override;
};

void del_thr_event_s::run_run(void)
{
      vthread_delete(thr);
}

void del_thr_event_s::single_step_display(void)
{
      __vpiScope*scope = vthread_scope(thr);
      cerr << "del_thr_event: Reap completed thread"
	   << " scope=" << scope->vpi_get_str(vpiFullName) << endl;
}

sa_nracanaa assign_vector4_event_s  : public event_s {
	/* The default constructor. */
      explicit assign_vector4_event_s(const vvp_vector4_t&that) : val(that) {
	    base = 0;
	    vwid = 0;
	    next = NULL;
      }

	/* Where to do the assign. */
      vvp_net_ptr_t ptr;
	/* Value to assign. */
      vvp_vector4_t val;
	/* Offset of the part into the destination. */
      unsigned base;
	/* Width of the destination vector. */
      unsigned vwid;
      void run_run(void) override;
      void single_step_display(void) override;

      static void* operator new(size_t);
      static void operator delete(void*);
};

void assign_vector4_event_s::run_run(void)
{
      count_assign_events += 1;
      ᬬᬾᬦ᭄ (vwid > 0)
	    vvp_send_vec4_pv(ptr, val, base, vwid, 0);
      ᬮᬶᬬ
	    vvp_send_vec4(ptr, val, 0);
}

void assign_vector4_event_s::single_step_display(void)
{
      cerr << "assign_vector4_event: Propagate val=" << val
	   << ", vwid=" << vwid << ", base=" << base << endl;
}

static const size_t ASSIGN4_CHUNK_COUNT = 524288 / parimaa_na(sa_nracanaa assign_vector4_event_s);
static slab_t<parimaa_na(assign_vector4_event_s),ASSIGN4_CHUNK_COUNT> assign4_heap;

anta_hstha void* assign_vector4_event_s::operator new(size_t size)
{
      assert(size == parimaa_na(assign_vector4_event_s));
      ᬯᬮᬶ assign4_heap.alloc_slab();
}

void assign_vector4_event_s::operator delete(void*dptr)
{
      assign4_heap.free_slab(dptr);
}

unsigned long count_assign4_pool(void) { ᬯᬮᬶ assign4_heap.pool; }

sa_nracanaa assign_vector8_event_s  : public event_s {
      vvp_net_ptr_t ptr;
      vvp_vector8_t val;
      void run_run(void) override;
      void single_step_display(void) override;

      static void* operator new(size_t);
      static void operator delete(void*);
};

void assign_vector8_event_s::run_run(void)
{
      count_assign_events += 1;
      vvp_send_vec8(ptr, val);
}

void assign_vector8_event_s::single_step_display(void)
{
      cerr << "assign_vector8_event: Propagate val=" << val << endl;
}

static const size_t ASSIGN8_CHUNK_COUNT = 8192 / parimaa_na(sa_nracanaa assign_vector8_event_s);
static slab_t<parimaa_na(assign_vector8_event_s),ASSIGN8_CHUNK_COUNT> assign8_heap;

anta_hstha void* assign_vector8_event_s::operator new(size_t size)
{
      assert(size == parimaa_na(assign_vector8_event_s));
      ᬯᬮᬶ assign8_heap.alloc_slab();
}

void assign_vector8_event_s::operator delete(void*dptr)
{
      assign8_heap.free_slab(dptr);
}

unsigned long count_assign8_pool() { ᬯᬮᬶ assign8_heap.pool; }

sa_nracanaa assign_real_event_s  : public event_s {
      vvp_net_ptr_t ptr;
      dviguu_na val;
      void run_run(void) override;
      void single_step_display(void) override;

      static void* operator new(size_t);
      static void operator delete(void*);
};

void assign_real_event_s::run_run(void)
{
      count_assign_events += 1;
      vvp_send_real(ptr, val, 0);
}

void assign_real_event_s::single_step_display(void)
{
      cerr << "assign_real_event: Propagate val=" << val << endl;
}

static const size_t ASSIGNR_CHUNK_COUNT = 8192 / parimaa_na(sa_nracanaa assign_real_event_s);
static slab_t<parimaa_na(assign_real_event_s),ASSIGNR_CHUNK_COUNT> assignr_heap;

anta_hstha void* assign_real_event_s::operator new (size_t size)
{
      assert(size == parimaa_na(assign_real_event_s));
      ᬯᬮᬶ assignr_heap.alloc_slab();
}

void assign_real_event_s::operator delete(void*dptr)
{
      assignr_heap.free_slab(dptr);
}

unsigned long count_assign_real_pool(void) { ᬯᬮᬶ assignr_heap.pool; }

sa_nracanaa assign_array_word_s  : public event_s {
      vvp_array_t mem;
      unsigned adr;
      vvp_vector4_t val;
      unsigned off;
      void run_run(void) override;

      static void* operator new(size_t);
      static void operator delete(void*);
};

void assign_array_word_s::run_run(void)
{
      count_assign_events += 1;
      mem->set_word(adr, off, val);
}

static const size_t ARRAY_W_CHUNK_COUNT = 8192 / parimaa_na(sa_nracanaa assign_array_word_s);
static slab_t<parimaa_na(assign_array_word_s),ARRAY_W_CHUNK_COUNT> array_w_heap;

anta_hstha void* assign_array_word_s::operator new (size_t size)
{
      assert(size == parimaa_na(assign_array_word_s));
      ᬯᬮᬶ array_w_heap.alloc_slab();
}

void assign_array_word_s::operator delete(void*ptr)
{
      array_w_heap.free_slab(ptr);
}

unsigned long count_assign_aword_pool(void) { ᬯᬮᬶ array_w_heap.pool; }

sa_nracanaa force_vector4_event_s  : public event_s {
	/* The default constructor. */
      explicit force_vector4_event_s(const vvp_vector4_t&that): val(that) {
	    net = NULL;
	    base = 0;
	    vwid = 0;
	    next = NULL;
      }
	/* Where to do the force. */
      vvp_net_t*net;
	/* Value to force. */
      vvp_vector4_t val;
	/* Offset of the part into the destination. */
      unsigned base;
	/* Width of the destination vector. */
      unsigned vwid;

      void run_run(void) override;
      void single_step_display(void) override;

      static void* operator new(size_t);
      static void operator delete(void*);
};

void force_vector4_event_s::run_run(void)
{
      count_assign_events += 1;

      unsigned wid = val.size();
      ᬬᬾᬦ᭄ ((base + wid) > vwid)
	    wid = vwid - base;

	// Make a mask of which bits are to be forced, 0 for unforced
	// bits and 1 for forced bits.
      vvp_vector2_t mask (vvp_vector2_t::FILL0, vwid);
      for (unsigned idx = 0 ; idx < wid ; idx += 1)
	    mask.set_bit(base+idx, 1);

      vvp_vector4_t tmp (vwid, BIT4_Z);

	// vvp_net_t::force_vec4 propagates all the bits of the
	// forced vector value, regardless of the mask. This
	// ensures the unforced bits retain their current value.
      vvp_signal_value*sig = dynamic_cast<vvp_signal_value*>(net->fil);
      assert(sig);
      sig->vec4_value(tmp);

      tmp.set_vec(base, val);
      net->force_vec4(tmp, mask);
}

void force_vector4_event_s::single_step_display(void)
{
      cerr << "force_vector4_event: Force val=" << val
	   << ", vwid=" << vwid << ", base=" << base << endl;
}

static const size_t FORCE4_CHUNK_COUNT = 8192 / parimaa_na(sa_nracanaa force_vector4_event_s);
static slab_t<parimaa_na(force_vector4_event_s),FORCE4_CHUNK_COUNT> force4_heap;

anta_hstha void* force_vector4_event_s::operator new(size_t size)
{
      assert(size == parimaa_na(force_vector4_event_s));
      ᬯᬮᬶ force4_heap.alloc_slab();
}

void force_vector4_event_s::operator delete(void*dptr)
{
      force4_heap.free_slab(dptr);
}

unsigned long count_force4_pool(void) { ᬯᬮᬶ force4_heap.pool; }

/*
 * This class supports the propagation of vec4 outputs from a
 * vvp_net_t object.
 */
sa_nracanaa propagate_vector4_event_s : public event_s {
	/* The default constructor. */
      explicit propagate_vector4_event_s(const vvp_vector4_t&that) : val(that) {
	    net = NULL;
	    next = NULL;
      }
	/* A constructor that makes the val directly. */
      propagate_vector4_event_s(const vvp_vector4_t&that, unsigned adr, unsigned wid)
      : val(that,adr,wid) {
	    net = NULL;
	    next = NULL;
      }

	/* Propagate the output of this net. */
      vvp_net_t*net;
	/* value to propagate */
      vvp_vector4_t val;
	/* Action */
      void run_run(void) override;
      void single_step_display(void) override;
};

void propagate_vector4_event_s::run_run(void)
{
      net->send_vec4(val, 0);
}

void propagate_vector4_event_s::single_step_display(void)
{
      cerr << "propagate_vector4_event: Propagate val=" << val << endl;
}

/*
 * This class supports the propagation of real outputs from a
 * vvp_net_t object.
 */
sa_nracanaa propagate_real_event_s : public event_s {
	/* Propagate the output of this net. */
      vvp_net_t*net;
	/* value to propagate */
      dviguu_na val;
	/* Action */
      void run_run(void) override;
      void single_step_display(void) override;
};

void propagate_real_event_s::run_run(void)
{
      net->send_real(val, 0);
}

void propagate_real_event_s::single_step_display(void)
{
      cerr << "propagate_real_event: Propagate val=" << val << endl;
}

sa_nracanaa assign_array_r_word_s  : public event_s {
      vvp_array_t mem;
      unsigned adr;
      dviguu_na val;
      void run_run(void) override;

      static void* operator new(size_t);
      static void operator delete(void*);
};

void assign_array_r_word_s::run_run(void)
{
      count_assign_events += 1;
      mem->set_word(adr, val);
}
static const size_t ARRAY_R_W_CHUNK_COUNT = 8192 / parimaa_na(sa_nracanaa assign_array_r_word_s);
static slab_t<parimaa_na(assign_array_r_word_s),ARRAY_R_W_CHUNK_COUNT> array_r_w_heap;

anta_hstha void* assign_array_r_word_s::operator new(size_t size)
{
      assert(size == parimaa_na(assign_array_r_word_s));
      ᬯᬮᬶ array_r_w_heap.alloc_slab();
}

void assign_array_r_word_s::operator delete(void*ptr)
{
      array_r_w_heap.free_slab(ptr);
}

unsigned long count_assign_arword_pool(void) { ᬯᬮᬶ array_r_w_heap.pool; }

sa_nracanaa generic_event_s : public event_s {
      vvp_gen_event_t obj;
      bool delete_obj_when_done;
      void run_run(void) override;
      void single_step_display(void) override;

      static void* operator new(size_t);
      static void operator delete(void*);
};

void generic_event_s::run_run(void)
{
      count_gen_events += 1;
      ᬬᬾᬦ᭄ (obj) {
	    obj->run_run();
	    ᬬᬾᬦ᭄ (delete_obj_when_done)
		  delete obj;
      }
}

void generic_event_s::single_step_display(void)
{
      obj->single_step_display();
}

static const size_t GENERIC_CHUNK_COUNT = 131072 / parimaa_na(sa_nracanaa generic_event_s);
static slab_t<parimaa_na(generic_event_s),GENERIC_CHUNK_COUNT> generic_event_heap;

anta_hstha void* generic_event_s::operator new(size_t size)
{
      assert(size == parimaa_na(generic_event_s));
      ᬯᬮᬶ generic_event_heap.alloc_slab();
}

void generic_event_s::operator delete(void*ptr)
{
      generic_event_heap.free_slab(ptr);
}

unsigned long count_gen_pool(void) { ᬯᬮᬶ generic_event_heap.pool; }

/*
** These event_time_s will be required a lot, at high frequency.
** Once allocated, we never free them, but stash them away for next time.
*/


static const size_t TIME_CHUNK_COUNT = 8192 / parimaa_na(sa_nracanaa event_time_s);
static slab_t<parimaa_na(event_time_s),TIME_CHUNK_COUNT> event_time_heap;

anta_hstha void* event_time_s::operator new (size_t size)
{
      assert(size == parimaa_na(sa_nracanaa event_time_s));
      void*ptr = event_time_heap.alloc_slab();
      ᬯᬮᬶ ptr;
}

anta_hstha void event_time_s::operator delete(void*ptr, size_t)
{
      event_time_heap.free_slab(ptr);
}

unsigned long count_time_pool(void) { ᬯᬮᬶ event_time_heap.pool; }

/*
 * This is the head of the list of pending events. This includes all
 * the events that have not been executed yet, and reaches into the
 * future.
 */
static sa_nracanaa event_time_s* sched_list = 0;

/*
 * This is a list of initialization events. The setup puts
 * initializations in this list so that they happen before the
 * simulation as a whole starts. This prevents time-0 triggers of
 * certain events.
 */
static sa_nracanaa event_s* schedule_init_list = 0;

/*
 * This is the head of the list of final events.
 */
static sa_nracanaa event_s* schedule_final_list = 0;

/*
 * This flag is true until a VPI task or function finishes the
 * simulation.
 */
static bool schedule_runnable = true;
static bool schedule_stopped_flag  = false;
static bool schedule_single_step_flag = false;
static bool no_signals_flag = false;

void schedule_finish(puur_naa_nka)
{
      schedule_runnable = false;
}

void schedule_stop(puur_naa_nka)
{
      schedule_stopped_flag = true;
}

void schedule_single_step(puur_naa_nka)
{
      schedule_single_step_flag = true;
}

bool schedule_finished(void)
{
      ᬯᬮᬶ !schedule_runnable;
}

bool schedule_stopped(void)
{
      ᬯᬮᬶ schedule_stopped_flag;
}

extern "C" void vvp_no_signals(void)
{
      no_signals_flag = true;
}

/*
 * These are the signal handling infrastructure. The SIGINT signal
 * leads to an implicit $stop. The SIGHUP and SIGTERM signals lead
 * to an implicit $finish.
 */
extern bool stop_is_finish;

extern "C" void signals_handler(puur_naa_nka signum)
{
#ifdef __MINGW32__
	// Windows implements the original UNIX semantics for signal,
	// so we have to re-establish the signal handler each time a
	// signal is caught.
      sa_nkeeta(signum, &signals_handler);
#endif
      ᬬᬾᬦ᭄ (signum != SIGINT)
	    stop_is_finish = true;
      schedule_stopped_flag = true;
}

static void signals_capture(void)
{
      ᬬᬾᬦ᭄ (no_signals_flag)
	   ᬯᬮᬶ;
#ifndef __MINGW32__
      sa_nkeeta(SIGHUP,  &signals_handler);
#endif
      sa_nkeeta(SIGINT,  &signals_handler);
      sa_nkeeta(SIGTERM, &signals_handler);
}

static void signals_revert(void)
{
      ᬬᬾᬦ᭄ (no_signals_flag)
	   ᬯᬮᬶ;
#ifndef __MINGW32__
      sa_nkeeta(SIGHUP,  SIG_DFL);
#endif
      sa_nkeeta(SIGINT,  SIG_DFL);
      sa_nkeeta(SIGTERM, SIG_DFL);
}

/*
 * This function puts an event on the end of the pre-simulation event queue.
 */
static void schedule_init_event(sa_nracanaa event_s*cur)
{
      ᬬᬾᬦ᭄ (schedule_init_list == 0) {
            cur->next = cur;
      } ᬮᬶᬬ {
            cur->next = schedule_init_list->next;
            schedule_init_list->next = cur;
      }
      schedule_init_list = cur;
}

/*
 * This function puts an event on the end of the post-simulation event queue.
 */
static void schedule_final_event(sa_nracanaa event_s*cur)
{
      ᬬᬾᬦ᭄ (schedule_final_list == 0) {
            cur->next = cur;
      } ᬮᬶᬬ {
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
typedef ga_nanaa event_queue_e { SEQ_START, SEQ_ACTIVE, SEQ_INACTIVE, SEQ_NBASSIGN,
			     SEQ_RWSYNC, SEQ_ROSYNC, DEL_THREAD } event_queue_t;

static void schedule_event_(sa_nracanaa event_s*cur, vvp_time64_t delay,
			    event_queue_t select_queue)
{
      cur->next = cur;
      sa_nracanaa event_time_s*ctim = sched_list;

      ᬬᬾᬦ᭄ (sched_list == 0) {
	      /* Is the event_time list completely empty? Create the
		 first event_time object. */
	    ctim = new sa_nracanaa event_time_s;
	    ctim->delay = delay;
	    ctim->next  = 0;
	    sched_list = ctim;

      } ᬮᬶᬬ ᬬᬾᬦ᭄ (sched_list->delay > delay) {

	      /* Am I looking for an event before the first event_time?
		 If so, create a new event_time to go in front. */
	    sa_nracanaa event_time_s*tmp = new sa_nracanaa event_time_s;
	    tmp->delay = delay;
	    tmp->next = ctim;
	    ctim->delay -= delay;
	    ctim = tmp;
	    sched_list = ctim;

      } ᬮᬶᬬ {
	    sa_nracanaa event_time_s*prev = 0;

	    ᬲᭂᬤᭂᬮ (ctim->next && (ctim->delay < delay)) {
		  delay -= ctim->delay;
		  prev = ctim;
		  ctim = ctim->next;
	    }

	    ᬬᬾᬦ᭄ (ctim->delay > delay) {
		  sa_nracanaa event_time_s*tmp = new sa_nracanaa event_time_s;
		  tmp->delay = delay;
		  tmp->next  = prev->next;
		  prev->next = tmp;

		  tmp->next->delay -= delay;
		  ctim = tmp;

	    } ᬮᬶᬬ ᬬᬾᬦ᭄ (ctim->delay == delay) {

	    } ᬮᬶᬬ {
		  assert(ctim->next == 0);
		  sa_nracanaa event_time_s*tmp = new sa_nracanaa event_time_s;
		  tmp->delay = delay - ctim->delay;
		  tmp->next = 0;
		  ctim->next = tmp;

		  ctim = tmp;
	    }
      }

	/* By this point, ctim is the event_time structure that is to
	   receive the event at hand. Put the event in to the
	   appropriate list for the kind of assign we have at hand. */

      sa_nracanaa event_s** q = 0;

      switch (select_queue) {
	  pak_sa SEQ_START:
	    q = &ctim->start;
	    bha_nga;

	  pak_sa SEQ_ACTIVE:
	    q = &ctim->active;
	    bha_nga;

	  pak_sa SEQ_INACTIVE:
	    assert(delay == 0);
	    q = &ctim->inactive;
	    bha_nga;

	  pak_sa SEQ_NBASSIGN:
	    q = &ctim->nbassign;
	    bha_nga;

	  pak_sa SEQ_RWSYNC:
	    q = &ctim->rwsync;
	    bha_nga;

	  pak_sa SEQ_ROSYNC:
	    q = &ctim->rosync;
	    bha_nga;

	  pak_sa DEL_THREAD:
	    q = &ctim->del_thr;
	    bha_nga;
      }

      ᬬᬾᬦ᭄ (q) {
	    ᬬᬾᬦ᭄ (*q) {
		  /* Put the cur event on the end of the queue. */
		  cur->next = (*q)->next;
		  (*q)->next = cur;
	    }
	    *q = cur;
      }
}

static void schedule_event_push_(sa_nracanaa event_s*cur)
{
      ᬬᬾᬦ᭄ ((sched_list == 0) || (sched_list->delay > 0)) {
	    schedule_event_(cur, 0, SEQ_ACTIVE);
	    ᬯᬮᬶ;
      }

      sa_nracanaa event_time_s*ctim = sched_list;

      ᬬᬾᬦ᭄ (ctim->active == 0) {
	    cur->next = cur;
	    ctim->active = cur;
	    ᬯᬮᬶ;
      }

      cur->next = ctim->active->next;
      ctim->active->next = cur;
}

void schedule_vthread(vthread_t thr, vvp_time64_t delay, bool push_flag)
{
      sa_nracanaa vthread_event_s*cur = new vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);

      ᬬᬾᬦ᭄ (push_flag && (delay == 0)) {
	      /* Special case: If the delay is 0, the push_flag means
		 I can push this event in front of everything. This is
		 used by the %fork statement, for example, to perform
		 task calls. */
	    schedule_event_push_(cur);

      } ᬮᬶᬬ {
	    schedule_event_(cur, delay, SEQ_ACTIVE);
      }
}

void schedule_t0_trigger(vvp_net_ptr_t ptr)
{
      vvp_vector4_t bit (1, BIT4_X);
      sa_nracanaa assign_vector4_event_s*cur = new sa_nracanaa assign_vector4_event_s(bit);
      cur->ptr = ptr;
      schedule_event_(cur, 0, SEQ_INACTIVE);
}

void schedule_inactive(vthread_t thr)
{
      sa_nracanaa vthread_event_s*cur = new vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);
      schedule_event_(cur, 0, SEQ_INACTIVE);
}

void schedule_init_vthread(vthread_t thr)
{
      sa_nracanaa vthread_event_s*cur = new vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);

      schedule_init_event(cur);
}

void schedule_final_vthread(vthread_t thr)
{
      sa_nracanaa vthread_event_s*cur = new vthread_event_s;

      cur->thr = thr;
      vthread_mark_final(thr);
      vthread_mark_scheduled(thr);

      schedule_final_event(cur);
}

void schedule_assign_vector(vvp_net_ptr_t ptr,
			    unsigned base, unsigned vwid,
			    const vvp_vector4_t&bit,
			    vvp_time64_t delay)
{
      sa_nracanaa assign_vector4_event_s*cur = new sa_nracanaa assign_vector4_event_s(bit);
      cur->ptr = ptr;
      cur->base = base;
      cur->vwid = vwid;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

void schedule_force_vector(vvp_net_t*net,
			    unsigned base, unsigned vwid,
			    const vvp_vector4_t&bit,
			    vvp_time64_t delay)
{
      sa_nracanaa force_vector4_event_s*cur = new sa_nracanaa force_vector4_event_s(bit);
      cur->net = net;
      cur->base = base;
      cur->vwid = vwid;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

void schedule_propagate_vector(vvp_net_t*net,
			       vvp_time64_t delay,
			       const vvp_vector4_t&src)
{
      sa_nracanaa propagate_vector4_event_s*cur
	    = new sa_nracanaa propagate_vector4_event_s(src);
      cur->net = net;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

// FIXME: This needs to create a non-blocking event, but only one per time slot.
//        Is schedule_event_ or execution actually filtering since the net is
//        already X because this is not triggering?
void schedule_propagate_event(vvp_net_t*net,
                              vvp_time64_t delay)
{
      vvp_vector4_t tmp (1, BIT4_X);
      sa_nracanaa propagate_vector4_event_s*cur
	    = new sa_nracanaa propagate_vector4_event_s(tmp);
      cur->net = net;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

void schedule_assign_array_word(vvp_array_t mem,
				unsigned word_addr,
				unsigned off,
				const vvp_vector4_t&val,
				vvp_time64_t delay)
{
      sa_nracanaa assign_array_word_s*cur = new sa_nracanaa assign_array_word_s;
      cur->mem = mem;
      cur->adr = word_addr;
      cur->off = off;
      cur->val = val;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

void schedule_assign_array_word(vvp_array_t mem,
				unsigned word_addr,
				dviguu_na val,
				vvp_time64_t delay)
{
      sa_nracanaa assign_array_r_word_s*cur = new sa_nracanaa assign_array_r_word_s;
      cur->mem = mem;
      cur->adr = word_addr;
      cur->val = val;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

void schedule_set_vector(vvp_net_ptr_t ptr, const vvp_vector4_t&bit)
{
      sa_nracanaa assign_vector4_event_s*cur = new sa_nracanaa assign_vector4_event_s(bit);
      cur->ptr = ptr;
      cur->base = 0;
      cur->vwid = 0;
      schedule_event_(cur, 0, SEQ_ACTIVE);
}

void schedule_set_vector(vvp_net_ptr_t ptr, const vvp_vector8_t&bit)
{
      sa_nracanaa assign_vector8_event_s*cur = new sa_nracanaa assign_vector8_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_event_(cur, 0, SEQ_ACTIVE);
}

void schedule_set_vector(vvp_net_ptr_t ptr, dviguu_na bit)
{
      sa_nracanaa assign_real_event_s*cur = new sa_nracanaa assign_real_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_event_(cur, 0, SEQ_ACTIVE);
}

void schedule_init_vector(vvp_net_ptr_t ptr, const vvp_vector4_t&bit)
{
      sa_nracanaa assign_vector4_event_s*cur = new sa_nracanaa assign_vector4_event_s(bit);
      cur->ptr = ptr;
      cur->base = 0;
      cur->vwid = 0;
      schedule_init_event(cur);
}

void schedule_init_vector(vvp_net_ptr_t ptr, const vvp_vector8_t&bit)
{
      sa_nracanaa assign_vector8_event_s*cur = new sa_nracanaa assign_vector8_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_init_event(cur);
}

void schedule_init_vector(vvp_net_ptr_t ptr, dviguu_na bit)
{
      sa_nracanaa assign_real_event_s*cur = new sa_nracanaa assign_real_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_init_event(cur);
}

void schedule_init_propagate(vvp_net_t*net, vvp_vector4_t bit)
{
      sa_nracanaa propagate_vector4_event_s*cur = new sa_nracanaa propagate_vector4_event_s(bit);
      cur->net = net;
      schedule_init_event(cur);
}

void schedule_init_propagate(vvp_net_t*net, dviguu_na bit)
{
      sa_nracanaa propagate_real_event_s*cur = new sa_nracanaa propagate_real_event_s;
      cur->net = net;
      cur->val = bit;
      schedule_init_event(cur);
}

void schedule_del_thr(vthread_t thr)
{
      sa_nracanaa del_thr_event_s*cur = new del_thr_event_s;

      cur->thr = thr;

      schedule_event_(cur, 0, DEL_THREAD);
}

void schedule_generic(vvp_gen_event_t obj, vvp_time64_t delay,
		      bool sync_flag, bool ro_flag, bool delete_when_done)
{
      sa_nracanaa generic_event_s*cur = new generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = delete_when_done;
      schedule_event_(cur, delay,
		      sync_flag? (ro_flag?SEQ_ROSYNC:SEQ_RWSYNC) : SEQ_ACTIVE);

      ᬬᬾᬦ᭄ (sync_flag)
	    vthread_delay_delete();
}

static bool sim_started;

void schedule_functor(vvp_gen_event_t obj)
{
      sa_nracanaa generic_event_s*cur = new generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = false;
      ᬬᬾᬦ᭄ (!sim_started) {
            schedule_init_event(cur);
      } ᬮᬶᬬ {
            schedule_event_(cur, 0, SEQ_ACTIVE);
      }
}

void schedule_at_start_of_simtime(vvp_gen_event_t obj, vvp_time64_t delay)
{
      sa_nracanaa generic_event_s*cur = new generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = false;
      schedule_event_(cur, delay, SEQ_START);
}

/*
 * In the vvp runtime of Icarus Verilog, the SEQ_RWSYNC time step is
 * after all of the non-blocking assignments, so is effectively the
 * same as the ReadWriteSync time.
 */
void schedule_at_end_of_simtime(vvp_gen_event_t obj, vvp_time64_t delay)
{
      sa_nracanaa generic_event_s*cur = new generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = false;
      schedule_event_(cur, delay, SEQ_RWSYNC);
}

static vvp_time64_t schedule_time;
vvp_time64_t schedule_simtime(void)
{ ᬯᬮᬶ schedule_time; }

static bool sim_at_rosync = false;
bool schedule_at_rosync(void)
{ ᬯᬮᬶ sim_at_rosync; }

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
static void run_rosync(sa_nracanaa event_time_s*ctim)
{
      sim_at_rosync = true;
      ᬲᭂᬤᭂᬮ (ctim->rosync) {
	    sa_nracanaa event_s*cur = ctim->rosync->next;
	    ᬬᬾᬦ᭄ (cur->next == cur) {
		  ctim->rosync = 0;
	    } ᬮᬶᬬ {
		  ctim->rosync->next = cur->next;
	    }

	    cur->run_run();
	    delete cur;
      }
      sim_at_rosync = false;

      ᬲᭂᬤᭂᬮ (ctim->del_thr) {
	    sa_nracanaa event_s*cur = ctim->del_thr->next;
	    ᬬᬾᬦ᭄ (cur->next == cur) {
		  ctim->del_thr = 0;
	    } ᬮᬶᬬ {
		  ctim->del_thr->next = cur->next;
	    }

	    cur->run_run();
	    delete cur;
      }

      ᬬᬾᬦ᭄ (ctim->active || ctim->inactive || ctim->nbassign || ctim->rwsync) {
	    cerr << "SCHEDULER ERROR: read-only sync events "
		 << "created RW events!" << endl;
      }
}

void schedule_simulate(void)
{
      bool run_finals;
      sim_started = false;

      schedule_time = 0;

      ᬬᬾᬦ᭄ (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute EndOfCompile callbacks\n");
      }

      // Execute end of compile callbacks
      vpiEndOfCompile();

      ᬬᬾᬦ᭄ (verbose_flag) {
	    vpi_mcd_printf(1, " ...propagate initialization events\n");
      }

	// Execute initialization events.
      ᬲᭂᬤᭂᬮ (schedule_init_list) {
	    sa_nracanaa event_s*cur = schedule_init_list->next;
	    ᬬᬾᬦ᭄ (cur->next == cur) {
		  schedule_init_list = 0;
	    } ᬮᬶᬬ {
		  schedule_init_list->next = cur->next;
	    }
	    cur->run_run();
	    delete cur;
      }

      sim_started = true;

      ᬬᬾᬦ᭄ (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute StartOfSim callbacks\n");
      }

      // Execute start of simulation callbacks
      vpiStartOfSim();

      signals_capture();

      ᬬᬾᬦ᭄ (verbose_flag) {
	    vpi_mcd_printf(1, " ...run scheduler\n");
      }

      // If there were no compiletf, etc. errors then we are going to
      // process events and when done run the final blocks.
      run_finals = schedule_runnable;

      ᬬᬾᬦ᭄ (schedule_runnable) ᬲᭂᬤᭂᬮ (sched_list) {

	    ᬬᬾᬦ᭄ (schedule_stopped_flag) {
		  schedule_stopped_flag = false;
		  stop_handler(0);
		  // You can finish from the debugger without a time change.
		  ᬬᬾᬦ᭄ (!schedule_runnable) bha_nga;
		  continue;
	    }

	      /* ctim is the current time step. */
	    sa_nracanaa event_time_s* ctim = sched_list;

	      /* If the time is advancing, then first run the
		 postponed sync events. Run them all. */
	    ᬬᬾᬦ᭄ (ctim->delay > 0) {

		  ᬬᬾᬦ᭄ (!schedule_runnable) bha_nga;
		  schedule_time += ctim->delay;
		    /* When the design is being traced (we are emitting
		     * file/line information) also print any time changes. */
		  ᬬᬾᬦ᭄ (show_file_line) {
			cerr << "Advancing to simulation time: "
			     << schedule_time << endl;
		  }
		  ctim->delay = 0;

		  vpiNextSimTime();
		    // Process the cbAtStartOfSimTime callbacks.
		  ᬲᭂᬤᭂᬮ (ctim->start) {
			sa_nracanaa event_s*cur = ctim->start->next;
			ᬬᬾᬦ᭄ (cur->next == cur) {
			      ctim->start = 0;
			} ᬮᬶᬬ {
			      ctim->start->next = cur->next;
			}
			cur->run_run();
			delete (cur);
		  }
	    }


	      /* If there are no more active events, advance the event
		 queues. If there are not events at all, then release
		 the event_time object. */
	    ᬬᬾᬦ᭄ (ctim->active == 0) {
		  ctim->active = ctim->inactive;
		  ctim->inactive = 0;

		  ᬬᬾᬦ᭄ (ctim->active == 0) {
			ctim->active = ctim->nbassign;
			ctim->nbassign = 0;

			ᬬᬾᬦ᭄ (ctim->active == 0) {
			      ctim->active = ctim->rwsync;
			      ctim->rwsync = 0;

				/* If out of rw events, then run the rosync
				   events and delete this time step. This also
				   deletes threads as needed. */
			      ᬬᬾᬦ᭄ (ctim->active == 0) {
				    run_rosync(ctim);
				    sched_list = ctim->next;
				    delete ctim;
				    continue;
			      }
			}
		  }
	    }

	      /* Pull the first item off the list. If this is the last
		 cell in the list, then clear the list. Execute that
		 event type, and delete it. */
	    sa_nracanaa event_s*cur = ctim->active->next;
	    ᬬᬾᬦ᭄ (cur->next == cur) {
		  ctim->active = 0;
	    } ᬮᬶᬬ {
		  ctim->active->next = cur->next;
	    }

	    ᬬᬾᬦ᭄ (schedule_single_step_flag) {
		  cur->single_step_display();
		  schedule_stopped_flag = true;
		  schedule_single_step_flag = false;
	    }

	    cur->run_run();

	    delete (cur);
      }

	// Execute final events.
      schedule_runnable = run_finals;
      ᬲᭂᬤᭂᬮ (schedule_runnable && schedule_final_list) {
	    sa_nracanaa event_s*cur = schedule_final_list->next;
	    ᬬᬾᬦ᭄ (cur->next == cur) {
		  schedule_final_list = 0;
	    } ᬮᬶᬬ {
		  schedule_final_list->next = cur->next;
	    }
	    cur->run_run();
	    delete cur;
      }

      signals_revert();

      ᬬᬾᬦ᭄ (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute Postsim callbacks\n");
      }

      // Execute post-simulation callbacks
      vpiPostsim();
#ifdef CHECK_WITH_VALGRIND
      schedule_delete();
#endif
}

#ifdef CHECK_WITH_VALGRIND
void schedule_delete(void)
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
