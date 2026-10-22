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

using ନାମାକାଶ std;

unsigned long ଗଣନା_assign_events = 0;
unsigned long ଗଣନା_gen_events = 0;
unsigned long ଗଣନା_thread_events = 0;
  // Count the time events (A time cell created)
unsigned long ଗଣନା_time_events = 0;



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
ସଂରଚନା event_s {
      ସଂରଚନା event_s*next;
      ଆଭାସୀ ~event_s() { }
      ଆଭାସୀ ଶୂନ୍ୟ run_run(ଶୂନ୍ୟ) =0;

	// Write something about the event to stderr
      ଆଭାସୀ ଶୂନ୍ୟ single_step_display(ଶୂନ୍ୟ);

	// Fallback new/delete
      ସ୍ଥିର ଶୂନ୍ୟ*operator ନୂତନ (size_t size) { ଫେରନ୍ତୁ ::ନୂତନ ଅକ୍ଷର[size]; }
      ସ୍ଥିର ଶୂନ୍ୟ operator delete(ଶୂନ୍ୟ*ptr)  { ::delete[]( static_cast<ଅକ୍ଷର*>(ptr) ); }
};

ଶୂନ୍ୟ event_s::single_step_display(ଶୂନ୍ୟ)
{
      std::cerr << "event_s: Step into event " << typeid(*ଏହି).name() << std::endl;
}

ସଂରଚନା event_time_s {
      event_time_s() {
	    ଗଣନା_time_events += 1;
	    delay = 0;
	    start = 0;
	    active = 0;
	    inactive = 0;
	    nbassign = 0;
	    rwsync = 0;
	    rosync = 0;
	    del_thr = 0;
	    next = ଖାଲି;
      }
      vvp_time64_t delay;

      ସଂରଚନା event_s*start;
      ସଂରଚନା event_s*active;
      ସଂରଚନା event_s*inactive;
      ସଂରଚନା event_s*nbassign;
      ସଂରଚନା event_s*rwsync;
      ସଂରଚନା event_s*rosync;
      ସଂରଚନା event_s*del_thr;

      ସଂରଚନା event_time_s*next;

      ସ୍ଥିର ଶୂନ୍ୟ* operator ନୂତନ (size_t);
      ସ୍ଥିର ଶୂନ୍ୟ operator delete(ଶୂନ୍ୟ*obj, size_t s);
};

vvp_gen_event_s::~vvp_gen_event_s()
{
}

ଶୂନ୍ୟ vvp_gen_event_s::single_step_display(ଶୂନ୍ୟ)
{
      cerr << "vvp_gen_event_s: Step into event " << typeid(*ଏହି).name() << endl;
}

/*
 * Derived event types
 */
ସଂରଚନା vthread_event_s : ସର୍ବସାଧାରଣ event_s {
      vthread_t thr;
      ଶୂନ୍ୟ run_run(ଶୂନ୍ୟ) override;
      ଶୂନ୍ୟ single_step_display(ଶୂନ୍ୟ) override;

      ସ୍ଥିର ଶୂନ୍ୟ* operator ନୂତନ(size_t);
      ସ୍ଥିର ଶୂନ୍ୟ operator delete(ଶୂନ୍ୟ*);
};

ଶୂନ୍ୟ vthread_event_s::run_run(ଶୂନ୍ୟ)
{
      ଗଣନା_thread_events += 1;
      vthread_run(thr);
}

ଶୂନ୍ୟ vthread_event_s::single_step_display(ଶୂନ୍ୟ)
{
      __vpiScope*scope = vthread_scope(thr);
      cerr << "vthread_event: Resume thread"
	   << " scope=" << scope->vpi_get_str(vpiFullName)
	   << endl;
}

ସ୍ଥିର ଅଚଳ size_t VTHR_CHUNK_ଗଣନା = 8192 / ଆକାର(ସଂରଚନା vthread_event_s);
ସ୍ଥିର slab_t<ଆକାର(vthread_event_s),VTHR_CHUNK_ଗଣନା> vthread_event_heap;

ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ* vthread_event_s::operator ନୂତନ(size_t size)
{
      ପ୍ରତିଜ୍ଞା(size == ଆକାର(vthread_event_s));
      ଫେରନ୍ତୁ vthread_event_heap.alloc_slab();
}

ଶୂନ୍ୟ vthread_event_s::operator delete(ଶୂନ୍ୟ*dptr)
{
      vthread_event_heap.free_slab(dptr);
}

ସଂରଚନା del_thr_event_s : ସର୍ବସାଧାରଣ event_s {
      vthread_t thr;
      ଶୂନ୍ୟ run_run(ଶୂନ୍ୟ) override;
      ଶୂନ୍ୟ single_step_display(ଶୂନ୍ୟ) override;
};

ଶୂନ୍ୟ del_thr_event_s::run_run(ଶୂନ୍ୟ)
{
      vthread_delete(thr);
}

ଶୂନ୍ୟ del_thr_event_s::single_step_display(ଶୂନ୍ୟ)
{
      __vpiScope*scope = vthread_scope(thr);
      cerr << "del_thr_event: Reap completed thread"
	   << " scope=" << scope->vpi_get_str(vpiFullName) << endl;
}

ସଂରଚନା assign_vector4_event_s  : ସର୍ବସାଧାରଣ event_s {
	/* The default constructor. */
      explicit assign_vector4_event_s(ଅଚଳ vvp_vector4_t&that) : val(that) {
	    base = 0;
	    vwid = 0;
	    next = ଖାଲି;
      }

	/* Where to do the assign. */
      vvp_net_ptr_t ptr;
	/* Value to assign. */
      vvp_vector4_t val;
	/* Offset of the part into the destination. */
      unsigned base;
	/* Width of the destination vector. */
      unsigned vwid;
      ଶୂନ୍ୟ run_run(ଶୂନ୍ୟ) override;
      ଶୂନ୍ୟ single_step_display(ଶୂନ୍ୟ) override;

      ସ୍ଥିର ଶୂନ୍ୟ* operator ନୂତନ(size_t);
      ସ୍ଥିର ଶୂନ୍ୟ operator delete(ଶୂନ୍ୟ*);
};

ଶୂନ୍ୟ assign_vector4_event_s::run_run(ଶୂନ୍ୟ)
{
      ଗଣନା_assign_events += 1;
      ଯଦି (vwid > 0)
	    vvp_send_vec4_pv(ptr, val, base, vwid, 0);
      ଅନ୍ୟଥା
	    vvp_send_vec4(ptr, val, 0);
}

ଶୂନ୍ୟ assign_vector4_event_s::single_step_display(ଶୂନ୍ୟ)
{
      cerr << "assign_vector4_event: Propagate val=" << val
	   << ", vwid=" << vwid << ", base=" << base << endl;
}

ସ୍ଥିର ଅଚଳ size_t ASSIGN4_CHUNK_ଗଣନା = 524288 / ଆକାର(ସଂରଚନା assign_vector4_event_s);
ସ୍ଥିର slab_t<ଆକାର(assign_vector4_event_s),ASSIGN4_CHUNK_ଗଣନା> assign4_heap;

ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ* assign_vector4_event_s::operator ନୂତନ(size_t size)
{
      ପ୍ରତିଜ୍ଞା(size == ଆକାର(assign_vector4_event_s));
      ଫେରନ୍ତୁ assign4_heap.alloc_slab();
}

ଶୂନ୍ୟ assign_vector4_event_s::operator delete(ଶୂନ୍ୟ*dptr)
{
      assign4_heap.free_slab(dptr);
}

unsigned long ଗଣନା_assign4_pool(ଶୂନ୍ୟ) { ଫେରନ୍ତୁ assign4_heap.pool; }

ସଂରଚନା assign_vector8_event_s  : ସର୍ବସାଧାରଣ event_s {
      vvp_net_ptr_t ptr;
      vvp_vector8_t val;
      ଶୂନ୍ୟ run_run(ଶୂନ୍ୟ) override;
      ଶୂନ୍ୟ single_step_display(ଶୂନ୍ୟ) override;

      ସ୍ଥିର ଶୂନ୍ୟ* operator ନୂତନ(size_t);
      ସ୍ଥିର ଶୂନ୍ୟ operator delete(ଶୂନ୍ୟ*);
};

ଶୂନ୍ୟ assign_vector8_event_s::run_run(ଶୂନ୍ୟ)
{
      ଗଣନା_assign_events += 1;
      vvp_send_vec8(ptr, val);
}

ଶୂନ୍ୟ assign_vector8_event_s::single_step_display(ଶୂନ୍ୟ)
{
      cerr << "assign_vector8_event: Propagate val=" << val << endl;
}

ସ୍ଥିର ଅଚଳ size_t ASSIGN8_CHUNK_ଗଣନା = 8192 / ଆକାର(ସଂରଚନା assign_vector8_event_s);
ସ୍ଥିର slab_t<ଆକାର(assign_vector8_event_s),ASSIGN8_CHUNK_ଗଣନା> assign8_heap;

ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ* assign_vector8_event_s::operator ନୂତନ(size_t size)
{
      ପ୍ରତିଜ୍ଞା(size == ଆକାର(assign_vector8_event_s));
      ଫେରନ୍ତୁ assign8_heap.alloc_slab();
}

ଶୂନ୍ୟ assign_vector8_event_s::operator delete(ଶୂନ୍ୟ*dptr)
{
      assign8_heap.free_slab(dptr);
}

unsigned long ଗଣନା_assign8_pool() { ଫେରନ୍ତୁ assign8_heap.pool; }

ସଂରଚନା assign_real_event_s  : ସର୍ବସାଧାରଣ event_s {
      vvp_net_ptr_t ptr;
      ଦ୍ୱିଗୁଣ val;
      ଶୂନ୍ୟ run_run(ଶୂନ୍ୟ) override;
      ଶୂନ୍ୟ single_step_display(ଶୂନ୍ୟ) override;

      ସ୍ଥିର ଶୂନ୍ୟ* operator ନୂତନ(size_t);
      ସ୍ଥିର ଶୂନ୍ୟ operator delete(ଶୂନ୍ୟ*);
};

ଶୂନ୍ୟ assign_real_event_s::run_run(ଶୂନ୍ୟ)
{
      ଗଣନା_assign_events += 1;
      vvp_send_real(ptr, val, 0);
}

ଶୂନ୍ୟ assign_real_event_s::single_step_display(ଶୂନ୍ୟ)
{
      cerr << "assign_real_event: Propagate val=" << val << endl;
}

ସ୍ଥିର ଅଚଳ size_t ASSIGNR_CHUNK_ଗଣନା = 8192 / ଆକାର(ସଂରଚନା assign_real_event_s);
ସ୍ଥିର slab_t<ଆକାର(assign_real_event_s),ASSIGNR_CHUNK_ଗଣନା> assignr_heap;

ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ* assign_real_event_s::operator ନୂତନ (size_t size)
{
      ପ୍ରତିଜ୍ଞା(size == ଆକାର(assign_real_event_s));
      ଫେରନ୍ତୁ assignr_heap.alloc_slab();
}

ଶୂନ୍ୟ assign_real_event_s::operator delete(ଶୂନ୍ୟ*dptr)
{
      assignr_heap.free_slab(dptr);
}

unsigned long ଗଣନା_assign_real_pool(ଶୂନ୍ୟ) { ଫେରନ୍ତୁ assignr_heap.pool; }

ସଂରଚନା assign_array_word_s  : ସର୍ବସାଧାରଣ event_s {
      vvp_array_t mem;
      unsigned adr;
      vvp_vector4_t val;
      unsigned off;
      ଶୂନ୍ୟ run_run(ଶୂନ୍ୟ) override;

      ସ୍ଥିର ଶୂନ୍ୟ* operator ନୂତନ(size_t);
      ସ୍ଥିର ଶୂନ୍ୟ operator delete(ଶୂନ୍ୟ*);
};

ଶୂନ୍ୟ assign_array_word_s::run_run(ଶୂନ୍ୟ)
{
      ଗଣନା_assign_events += 1;
      mem->set_word(adr, off, val);
}

ସ୍ଥିର ଅଚଳ size_t ARRAY_W_CHUNK_ଗଣନା = 8192 / ଆକାର(ସଂରଚନା assign_array_word_s);
ସ୍ଥିର slab_t<ଆକାର(assign_array_word_s),ARRAY_W_CHUNK_ଗଣନା> array_w_heap;

ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ* assign_array_word_s::operator ନୂତନ (size_t size)
{
      ପ୍ରତିଜ୍ଞା(size == ଆକାର(assign_array_word_s));
      ଫେରନ୍ତୁ array_w_heap.alloc_slab();
}

ଶୂନ୍ୟ assign_array_word_s::operator delete(ଶୂନ୍ୟ*ptr)
{
      array_w_heap.free_slab(ptr);
}

unsigned long ଗଣନା_assign_aword_pool(ଶୂନ୍ୟ) { ଫେରନ୍ତୁ array_w_heap.pool; }

ସଂରଚନା force_vector4_event_s  : ସର୍ବସାଧାରଣ event_s {
	/* The default constructor. */
      explicit force_vector4_event_s(ଅଚଳ vvp_vector4_t&that): val(that) {
	    net = ଖାଲି;
	    base = 0;
	    vwid = 0;
	    next = ଖାଲି;
      }
	/* Where to do the force. */
      vvp_net_t*net;
	/* Value to force. */
      vvp_vector4_t val;
	/* Offset of the part into the destination. */
      unsigned base;
	/* Width of the destination vector. */
      unsigned vwid;

      ଶୂନ୍ୟ run_run(ଶୂନ୍ୟ) override;
      ଶୂନ୍ୟ single_step_display(ଶୂନ୍ୟ) override;

      ସ୍ଥିର ଶୂନ୍ୟ* operator ନୂତନ(size_t);
      ସ୍ଥିର ଶୂନ୍ୟ operator delete(ଶୂନ୍ୟ*);
};

ଶୂନ୍ୟ force_vector4_event_s::run_run(ଶୂନ୍ୟ)
{
      ଗଣନା_assign_events += 1;

      unsigned wid = val.size();
      ଯଦି ((base + wid) > vwid)
	    wid = vwid - base;

	// Make a mask of which bits are to be forced, 0 for unforced
	// bits and 1 for forced bits.
      vvp_vector2_t mask (vvp_vector2_t::FILL0, vwid);
      ପାଇଁ (unsigned idx = 0 ; idx < wid ; idx += 1)
	    mask.set_bit(base+idx, 1);

      vvp_vector4_t tmp (vwid, BIT4_Z);

	// vvp_net_t::force_vec4 propagates all the bits of the
	// forced vector value, regardless of the mask. This
	// ensures the unforced bits retain their current value.
      vvp_signal_ମୂଲ୍ୟ*sig = dynamic_cast<vvp_signal_ମୂଲ୍ୟ*>(net->fil);
      ପ୍ରତିଜ୍ଞା(sig);
      sig->vec4_ମୂଲ୍ୟ(tmp);

      tmp.set_vec(base, val);
      net->force_vec4(tmp, mask);
}

ଶୂନ୍ୟ force_vector4_event_s::single_step_display(ଶୂନ୍ୟ)
{
      cerr << "force_vector4_event: Force val=" << val
	   << ", vwid=" << vwid << ", base=" << base << endl;
}

ସ୍ଥିର ଅଚଳ size_t FORCE4_CHUNK_ଗଣନା = 8192 / ଆକାର(ସଂରଚନା force_vector4_event_s);
ସ୍ଥିର slab_t<ଆକାର(force_vector4_event_s),FORCE4_CHUNK_ଗଣନା> force4_heap;

ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ* force_vector4_event_s::operator ନୂତନ(size_t size)
{
      ପ୍ରତିଜ୍ଞା(size == ଆକାର(force_vector4_event_s));
      ଫେରନ୍ତୁ force4_heap.alloc_slab();
}

ଶୂନ୍ୟ force_vector4_event_s::operator delete(ଶୂନ୍ୟ*dptr)
{
      force4_heap.free_slab(dptr);
}

unsigned long ଗଣନା_force4_pool(ଶୂନ୍ୟ) { ଫେରନ୍ତୁ force4_heap.pool; }

/*
 * This class supports the propagation of vec4 outputs from a
 * vvp_net_t object.
 */
ସଂରଚନା propagate_vector4_event_s : ସର୍ବସାଧାରଣ event_s {
	/* The default constructor. */
      explicit propagate_vector4_event_s(ଅଚଳ vvp_vector4_t&that) : val(that) {
	    net = ଖାଲି;
	    next = ଖାଲି;
      }
	/* A constructor that makes the val directly. */
      propagate_vector4_event_s(ଅଚଳ vvp_vector4_t&that, unsigned adr, unsigned wid)
      : val(that,adr,wid) {
	    net = ଖାଲି;
	    next = ଖାଲି;
      }

	/* Propagate the output of this net. */
      vvp_net_t*net;
	/* value to propagate */
      vvp_vector4_t val;
	/* Action */
      ଶୂନ୍ୟ run_run(ଶୂନ୍ୟ) override;
      ଶୂନ୍ୟ single_step_display(ଶୂନ୍ୟ) override;
};

ଶୂନ୍ୟ propagate_vector4_event_s::run_run(ଶୂନ୍ୟ)
{
      net->send_vec4(val, 0);
}

ଶୂନ୍ୟ propagate_vector4_event_s::single_step_display(ଶୂନ୍ୟ)
{
      cerr << "propagate_vector4_event: Propagate val=" << val << endl;
}

/*
 * This class supports the propagation of real outputs from a
 * vvp_net_t object.
 */
ସଂରଚନା propagate_real_event_s : ସର୍ବସାଧାରଣ event_s {
	/* Propagate the output of this net. */
      vvp_net_t*net;
	/* value to propagate */
      ଦ୍ୱିଗୁଣ val;
	/* Action */
      ଶୂନ୍ୟ run_run(ଶୂନ୍ୟ) override;
      ଶୂନ୍ୟ single_step_display(ଶୂନ୍ୟ) override;
};

ଶୂନ୍ୟ propagate_real_event_s::run_run(ଶୂନ୍ୟ)
{
      net->send_real(val, 0);
}

ଶୂନ୍ୟ propagate_real_event_s::single_step_display(ଶୂନ୍ୟ)
{
      cerr << "propagate_real_event: Propagate val=" << val << endl;
}

ସଂରଚନା assign_array_r_word_s  : ସର୍ବସାଧାରଣ event_s {
      vvp_array_t mem;
      unsigned adr;
      ଦ୍ୱିଗୁଣ val;
      ଶୂନ୍ୟ run_run(ଶୂନ୍ୟ) override;

      ସ୍ଥିର ଶୂନ୍ୟ* operator ନୂତନ(size_t);
      ସ୍ଥିର ଶୂନ୍ୟ operator delete(ଶୂନ୍ୟ*);
};

ଶୂନ୍ୟ assign_array_r_word_s::run_run(ଶୂନ୍ୟ)
{
      ଗଣନା_assign_events += 1;
      mem->set_word(adr, val);
}
ସ୍ଥିର ଅଚଳ size_t ARRAY_R_W_CHUNK_ଗଣନା = 8192 / ଆକାର(ସଂରଚନା assign_array_r_word_s);
ସ୍ଥିର slab_t<ଆକାର(assign_array_r_word_s),ARRAY_R_W_CHUNK_ଗଣନା> array_r_w_heap;

ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ* assign_array_r_word_s::operator ନୂତନ(size_t size)
{
      ପ୍ରତିଜ୍ଞା(size == ଆକାର(assign_array_r_word_s));
      ଫେରନ୍ତୁ array_r_w_heap.alloc_slab();
}

ଶୂନ୍ୟ assign_array_r_word_s::operator delete(ଶୂନ୍ୟ*ptr)
{
      array_r_w_heap.free_slab(ptr);
}

unsigned long ଗଣନା_assign_arword_pool(ଶୂନ୍ୟ) { ଫେରନ୍ତୁ array_r_w_heap.pool; }

ସଂରଚନା generic_event_s : ସର୍ବସାଧାରଣ event_s {
      vvp_gen_event_t obj;
      ତାର୍କିକ delete_obj_when_done;
      ଶୂନ୍ୟ run_run(ଶୂନ୍ୟ) override;
      ଶୂନ୍ୟ single_step_display(ଶୂନ୍ୟ) override;

      ସ୍ଥିର ଶୂନ୍ୟ* operator ନୂତନ(size_t);
      ସ୍ଥିର ଶୂନ୍ୟ operator delete(ଶୂନ୍ୟ*);
};

ଶୂନ୍ୟ generic_event_s::run_run(ଶୂନ୍ୟ)
{
      ଗଣନା_gen_events += 1;
      ଯଦି (obj) {
	    obj->run_run();
	    ଯଦି (delete_obj_when_done)
		  delete obj;
      }
}

ଶୂନ୍ୟ generic_event_s::single_step_display(ଶୂନ୍ୟ)
{
      obj->single_step_display();
}

ସ୍ଥିର ଅଚଳ size_t GENERIC_CHUNK_ଗଣନା = 131072 / ଆକାର(ସଂରଚନା generic_event_s);
ସ୍ଥିର slab_t<ଆକାର(generic_event_s),GENERIC_CHUNK_ଗଣନା> generic_event_heap;

ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ* generic_event_s::operator ନୂତନ(size_t size)
{
      ପ୍ରତିଜ୍ଞା(size == ଆକାର(generic_event_s));
      ଫେରନ୍ତୁ generic_event_heap.alloc_slab();
}

ଶୂନ୍ୟ generic_event_s::operator delete(ଶୂନ୍ୟ*ptr)
{
      generic_event_heap.free_slab(ptr);
}

unsigned long ଗଣନା_gen_pool(ଶୂନ୍ୟ) { ଫେରନ୍ତୁ generic_event_heap.pool; }

/*
** These event_time_s will be required a lot, at high frequency.
** Once allocated, we never free them, but stash them away for next time.
*/


ସ୍ଥିର ଅଚଳ size_t TIME_CHUNK_ଗଣନା = 8192 / ଆକାର(ସଂରଚନା event_time_s);
ସ୍ଥିର slab_t<ଆକାର(event_time_s),TIME_CHUNK_ଗଣନା> event_time_heap;

ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ* event_time_s::operator ନୂତନ (size_t size)
{
      ପ୍ରତିଜ୍ଞା(size == ଆକାର(ସଂରଚନା event_time_s));
      ଶୂନ୍ୟ*ptr = event_time_heap.alloc_slab();
      ଫେରନ୍ତୁ ptr;
}

ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ event_time_s::operator delete(ଶୂନ୍ୟ*ptr, size_t)
{
      event_time_heap.free_slab(ptr);
}

unsigned long ଗଣନା_time_pool(ଶୂନ୍ୟ) { ଫେରନ୍ତୁ event_time_heap.pool; }

/*
 * This is the head of the list of pending events. This includes all
 * the events that have not been executed yet, and reaches into the
 * future.
 */
ସ୍ଥିର ସଂରଚନା event_time_s* sched_list = 0;

/*
 * This is a list of initialization events. The setup puts
 * initializations in this list so that they happen before the
 * simulation as a whole starts. This prevents time-0 triggers of
 * certain events.
 */
ସ୍ଥିର ସଂରଚନା event_s* schedule_init_list = 0;

/*
 * This is the head of the list of final events.
 */
ସ୍ଥିର ସଂରଚନା event_s* schedule_final_list = 0;

/*
 * This flag is true until a VPI task or function finishes the
 * simulation.
 */
ସ୍ଥିର ତାର୍କିକ schedule_runnable = ସତ୍ୟ;
ସ୍ଥିର ତାର୍କିକ schedule_stopped_flag  = ମିଥ୍ୟା;
ସ୍ଥିର ତାର୍କିକ schedule_single_step_flag = ମିଥ୍ୟା;
ସ୍ଥିର ତାର୍କିକ no_signals_flag = ମିଥ୍ୟା;

ଶୂନ୍ୟ schedule_finish(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)
{
      schedule_runnable = ମିଥ୍ୟା;
}

ଶୂନ୍ୟ schedule_stop(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)
{
      schedule_stopped_flag = ସତ୍ୟ;
}

ଶୂନ୍ୟ schedule_single_step(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)
{
      schedule_single_step_flag = ସତ୍ୟ;
}

ତାର୍କିକ schedule_finished(ଶୂନ୍ୟ)
{
      ଫେରନ୍ତୁ !schedule_runnable;
}

ତାର୍କିକ schedule_stopped(ଶୂନ୍ୟ)
{
      ଫେରନ୍ତୁ schedule_stopped_flag;
}

ବାହ୍ୟ "C" ଶୂନ୍ୟ vvp_no_signals(ଶୂନ୍ୟ)
{
      no_signals_flag = ସତ୍ୟ;
}

/*
 * These are the signal handling infrastructure. The SIGINT signal
 * leads to an implicit $stop. The SIGHUP and SIGTERM signals lead
 * to an implicit $finish.
 */
ବାହ୍ୟ ତାର୍କିକ stop_is_finish;

ବାହ୍ୟ "C" ଶୂନ୍ୟ signals_handler(ପୂର୍ଣ୍ଣସଂଖ୍ୟା signum)
{
#ifdef __MINGW32__
	// Windows implements the original UNIX semantics for signal,
	// so we have to re-establish the signal handler each time a
	// signal is caught.
      ସଙ୍କେତ(signum, &signals_handler);
#endif
      ଯଦି (signum != SIGINT)
	    stop_is_finish = ସତ୍ୟ;
      schedule_stopped_flag = ସତ୍ୟ;
}

ସ୍ଥିର ଶୂନ୍ୟ signals_capture(ଶୂନ୍ୟ)
{
      ଯଦି (no_signals_flag)
	   ଫେରନ୍ତୁ;
#ifndef __MINGW32__
      ସଙ୍କେତ(SIGHUP,  &signals_handler);
#endif
      ସଙ୍କେତ(SIGINT,  &signals_handler);
      ସଙ୍କେତ(SIGTERM, &signals_handler);
}

ସ୍ଥିର ଶୂନ୍ୟ signals_revert(ଶୂନ୍ୟ)
{
      ଯଦି (no_signals_flag)
	   ଫେରନ୍ତୁ;
#ifndef __MINGW32__
      ସଙ୍କେତ(SIGHUP,  SIG_DFL);
#endif
      ସଙ୍କେତ(SIGINT,  SIG_DFL);
      ସଙ୍କେତ(SIGTERM, SIG_DFL);
}

/*
 * This function puts an event on the end of the pre-simulation event queue.
 */
ସ୍ଥିର ଶୂନ୍ୟ schedule_init_event(ସଂରଚନା event_s*cur)
{
      ଯଦି (schedule_init_list == 0) {
            cur->next = cur;
      } ଅନ୍ୟଥା {
            cur->next = schedule_init_list->next;
            schedule_init_list->next = cur;
      }
      schedule_init_list = cur;
}

/*
 * This function puts an event on the end of the post-simulation event queue.
 */
ସ୍ଥିର ଶୂନ୍ୟ schedule_final_event(ସଂରଚନା event_s*cur)
{
      ଯଦି (schedule_final_list == 0) {
            cur->next = cur;
      } ଅନ୍ୟଥା {
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
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା event_queue_e { SEQ_START, SEQ_ACTIVE, SEQ_INACTIVE, SEQ_NBASSIGN,
			     SEQ_RWSYNC, SEQ_ROSYNC, DEL_THREAD } event_queue_t;

ସ୍ଥିର ଶୂନ୍ୟ schedule_event_(ସଂରଚନା event_s*cur, vvp_time64_t delay,
			    event_queue_t select_queue)
{
      cur->next = cur;
      ସଂରଚନା event_time_s*ctim = sched_list;

      ଯଦି (sched_list == 0) {
	      /* Is the event_time list completely empty? Create the
		 first event_time object. */
	    ctim = ନୂତନ ସଂରଚନା event_time_s;
	    ctim->delay = delay;
	    ctim->next  = 0;
	    sched_list = ctim;

      } ଅନ୍ୟଥା ଯଦି (sched_list->delay > delay) {

	      /* Am I looking for an event before the first event_time?
		 If so, create a new event_time to go in front. */
	    ସଂରଚନା event_time_s*tmp = ନୂତନ ସଂରଚନା event_time_s;
	    tmp->delay = delay;
	    tmp->next = ctim;
	    ctim->delay -= delay;
	    ctim = tmp;
	    sched_list = ctim;

      } ଅନ୍ୟଥା {
	    ସଂରଚନା event_time_s*prev = 0;

	    ଯେତେବେଳେ (ctim->next && (ctim->delay < delay)) {
		  delay -= ctim->delay;
		  prev = ctim;
		  ctim = ctim->next;
	    }

	    ଯଦି (ctim->delay > delay) {
		  ସଂରଚନା event_time_s*tmp = ନୂତନ ସଂରଚନା event_time_s;
		  tmp->delay = delay;
		  tmp->next  = prev->next;
		  prev->next = tmp;

		  tmp->next->delay -= delay;
		  ctim = tmp;

	    } ଅନ୍ୟଥା ଯଦି (ctim->delay == delay) {

	    } ଅନ୍ୟଥା {
		  ପ୍ରତିଜ୍ଞା(ctim->next == 0);
		  ସଂରଚନା event_time_s*tmp = ନୂତନ ସଂରଚନା event_time_s;
		  tmp->delay = delay - ctim->delay;
		  tmp->next = 0;
		  ctim->next = tmp;

		  ctim = tmp;
	    }
      }

	/* By this point, ctim is the event_time structure that is to
	   receive the event at hand. Put the event in to the
	   appropriate list for the kind of assign we have at hand. */

      ସଂରଚନା event_s** q = 0;

      ବାଛ (select_queue) {
	  ସ୍ଥିତି SEQ_START:
	    q = &ctim->start;
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି SEQ_ACTIVE:
	    q = &ctim->active;
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି SEQ_INACTIVE:
	    ପ୍ରତିଜ୍ଞା(delay == 0);
	    q = &ctim->inactive;
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି SEQ_NBASSIGN:
	    q = &ctim->nbassign;
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି SEQ_RWSYNC:
	    q = &ctim->rwsync;
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି SEQ_ROSYNC:
	    q = &ctim->rosync;
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି DEL_THREAD:
	    q = &ctim->del_thr;
	    ଭାଙ୍ଗ;
      }

      ଯଦି (q) {
	    ଯଦି (*q) {
		  /* Put the cur event on the end of the queue. */
		  cur->next = (*q)->next;
		  (*q)->next = cur;
	    }
	    *q = cur;
      }
}

ସ୍ଥିର ଶୂନ୍ୟ schedule_event_ଠେଲ(ସଂରଚନା event_s*cur)
{
      ଯଦି ((sched_list == 0) || (sched_list->delay > 0)) {
	    schedule_event_(cur, 0, SEQ_ACTIVE);
	    ଫେରନ୍ତୁ;
      }

      ସଂରଚନା event_time_s*ctim = sched_list;

      ଯଦି (ctim->active == 0) {
	    cur->next = cur;
	    ctim->active = cur;
	    ଫେରନ୍ତୁ;
      }

      cur->next = ctim->active->next;
      ctim->active->next = cur;
}

ଶୂନ୍ୟ schedule_vthread(vthread_t thr, vvp_time64_t delay, ତାର୍କିକ ଠେଲ_flag)
{
      ସଂରଚନା vthread_event_s*cur = ନୂତନ vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);

      ଯଦି (ଠେଲ_flag && (delay == 0)) {
	      /* Special case: If the delay is 0, the push_flag means
		 I can push this event in front of everything. This is
		 used by the %fork statement, for example, to perform
		 task calls. */
	    schedule_event_ଠେଲ(cur);

      } ଅନ୍ୟଥା {
	    schedule_event_(cur, delay, SEQ_ACTIVE);
      }
}

ଶୂନ୍ୟ schedule_t0_trigger(vvp_net_ptr_t ptr)
{
      vvp_vector4_t bit (1, BIT4_X);
      ସଂରଚନା assign_vector4_event_s*cur = ନୂତନ ସଂରଚନା assign_vector4_event_s(bit);
      cur->ptr = ptr;
      schedule_event_(cur, 0, SEQ_INACTIVE);
}

ଶୂନ୍ୟ schedule_inactive(vthread_t thr)
{
      ସଂରଚନା vthread_event_s*cur = ନୂତନ vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);
      schedule_event_(cur, 0, SEQ_INACTIVE);
}

ଶୂନ୍ୟ schedule_init_vthread(vthread_t thr)
{
      ସଂରଚନା vthread_event_s*cur = ନୂତନ vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);

      schedule_init_event(cur);
}

ଶୂନ୍ୟ schedule_final_vthread(vthread_t thr)
{
      ସଂରଚନା vthread_event_s*cur = ନୂତନ vthread_event_s;

      cur->thr = thr;
      vthread_mark_final(thr);
      vthread_mark_scheduled(thr);

      schedule_final_event(cur);
}

ଶୂନ୍ୟ schedule_assign_vector(vvp_net_ptr_t ptr,
			    unsigned base, unsigned vwid,
			    ଅଚଳ vvp_vector4_t&bit,
			    vvp_time64_t delay)
{
      ସଂରଚନା assign_vector4_event_s*cur = ନୂତନ ସଂରଚନା assign_vector4_event_s(bit);
      cur->ptr = ptr;
      cur->base = base;
      cur->vwid = vwid;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

ଶୂନ୍ୟ schedule_force_vector(vvp_net_t*net,
			    unsigned base, unsigned vwid,
			    ଅଚଳ vvp_vector4_t&bit,
			    vvp_time64_t delay)
{
      ସଂରଚନା force_vector4_event_s*cur = ନୂତନ ସଂରଚନା force_vector4_event_s(bit);
      cur->net = net;
      cur->base = base;
      cur->vwid = vwid;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

ଶୂନ୍ୟ schedule_propagate_vector(vvp_net_t*net,
			       vvp_time64_t delay,
			       ଅଚଳ vvp_vector4_t&src)
{
      ସଂରଚନା propagate_vector4_event_s*cur
	    = ନୂତନ ସଂରଚନା propagate_vector4_event_s(src);
      cur->net = net;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

// FIXME: This needs to create a non-blocking event, but only one per time slot.
//        Is schedule_event_ or execution actually filtering since the net is
//        already X because this is not triggering?
ଶୂନ୍ୟ schedule_propagate_event(vvp_net_t*net,
                              vvp_time64_t delay)
{
      vvp_vector4_t tmp (1, BIT4_X);
      ସଂରଚନା propagate_vector4_event_s*cur
	    = ନୂତନ ସଂରଚନା propagate_vector4_event_s(tmp);
      cur->net = net;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

ଶୂନ୍ୟ schedule_assign_array_word(vvp_array_t mem,
				unsigned word_addr,
				unsigned off,
				ଅଚଳ vvp_vector4_t&val,
				vvp_time64_t delay)
{
      ସଂରଚନା assign_array_word_s*cur = ନୂତନ ସଂରଚନା assign_array_word_s;
      cur->mem = mem;
      cur->adr = word_addr;
      cur->off = off;
      cur->val = val;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

ଶୂନ୍ୟ schedule_assign_array_word(vvp_array_t mem,
				unsigned word_addr,
				ଦ୍ୱିଗୁଣ val,
				vvp_time64_t delay)
{
      ସଂରଚନା assign_array_r_word_s*cur = ନୂତନ ସଂରଚନା assign_array_r_word_s;
      cur->mem = mem;
      cur->adr = word_addr;
      cur->val = val;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

ଶୂନ୍ୟ schedule_set_vector(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit)
{
      ସଂରଚନା assign_vector4_event_s*cur = ନୂତନ ସଂରଚନା assign_vector4_event_s(bit);
      cur->ptr = ptr;
      cur->base = 0;
      cur->vwid = 0;
      schedule_event_(cur, 0, SEQ_ACTIVE);
}

ଶୂନ୍ୟ schedule_set_vector(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector8_t&bit)
{
      ସଂରଚନା assign_vector8_event_s*cur = ନୂତନ ସଂରଚନା assign_vector8_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_event_(cur, 0, SEQ_ACTIVE);
}

ଶୂନ୍ୟ schedule_set_vector(vvp_net_ptr_t ptr, ଦ୍ୱିଗୁଣ bit)
{
      ସଂରଚନା assign_real_event_s*cur = ନୂତନ ସଂରଚନା assign_real_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_event_(cur, 0, SEQ_ACTIVE);
}

ଶୂନ୍ୟ schedule_init_vector(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit)
{
      ସଂରଚନା assign_vector4_event_s*cur = ନୂତନ ସଂରଚନା assign_vector4_event_s(bit);
      cur->ptr = ptr;
      cur->base = 0;
      cur->vwid = 0;
      schedule_init_event(cur);
}

ଶୂନ୍ୟ schedule_init_vector(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector8_t&bit)
{
      ସଂରଚନା assign_vector8_event_s*cur = ନୂତନ ସଂରଚନା assign_vector8_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_init_event(cur);
}

ଶୂନ୍ୟ schedule_init_vector(vvp_net_ptr_t ptr, ଦ୍ୱିଗୁଣ bit)
{
      ସଂରଚନା assign_real_event_s*cur = ନୂତନ ସଂରଚନା assign_real_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_init_event(cur);
}

ଶୂନ୍ୟ schedule_init_propagate(vvp_net_t*net, vvp_vector4_t bit)
{
      ସଂରଚନା propagate_vector4_event_s*cur = ନୂତନ ସଂରଚନା propagate_vector4_event_s(bit);
      cur->net = net;
      schedule_init_event(cur);
}

ଶୂନ୍ୟ schedule_init_propagate(vvp_net_t*net, ଦ୍ୱିଗୁଣ bit)
{
      ସଂରଚନା propagate_real_event_s*cur = ନୂତନ ସଂରଚନା propagate_real_event_s;
      cur->net = net;
      cur->val = bit;
      schedule_init_event(cur);
}

ଶୂନ୍ୟ schedule_del_thr(vthread_t thr)
{
      ସଂରଚନା del_thr_event_s*cur = ନୂତନ del_thr_event_s;

      cur->thr = thr;

      schedule_event_(cur, 0, DEL_THREAD);
}

ଶୂନ୍ୟ schedule_generic(vvp_gen_event_t obj, vvp_time64_t delay,
		      ତାର୍କିକ sync_flag, ତାର୍କିକ ro_flag, ତାର୍କିକ delete_when_done)
{
      ସଂରଚନା generic_event_s*cur = ନୂତନ generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = delete_when_done;
      schedule_event_(cur, delay,
		      sync_flag? (ro_flag?SEQ_ROSYNC:SEQ_RWSYNC) : SEQ_ACTIVE);

      ଯଦି (sync_flag)
	    vthread_delay_delete();
}

ସ୍ଥିର ତାର୍କିକ sim_started;

ଶୂନ୍ୟ schedule_functor(vvp_gen_event_t obj)
{
      ସଂରଚନା generic_event_s*cur = ନୂତନ generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = ମିଥ୍ୟା;
      ଯଦି (!sim_started) {
            schedule_init_event(cur);
      } ଅନ୍ୟଥା {
            schedule_event_(cur, 0, SEQ_ACTIVE);
      }
}

ଶୂନ୍ୟ schedule_at_start_of_simtime(vvp_gen_event_t obj, vvp_time64_t delay)
{
      ସଂରଚନା generic_event_s*cur = ନୂତନ generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = ମିଥ୍ୟା;
      schedule_event_(cur, delay, SEQ_START);
}

/*
 * In the vvp runtime of Icarus Verilog, the SEQ_RWSYNC time step is
 * after all of the non-blocking assignments, so is effectively the
 * same as the ReadWriteSync time.
 */
ଶୂନ୍ୟ schedule_at_end_of_simtime(vvp_gen_event_t obj, vvp_time64_t delay)
{
      ସଂରଚନା generic_event_s*cur = ନୂତନ generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = ମିଥ୍ୟା;
      schedule_event_(cur, delay, SEQ_RWSYNC);
}

ସ୍ଥିର vvp_time64_t schedule_time;
vvp_time64_t schedule_simtime(ଶୂନ୍ୟ)
{ ଫେରନ୍ତୁ schedule_time; }

ସ୍ଥିର ତାର୍କିକ sim_at_rosync = ମିଥ୍ୟା;
ତାର୍କିକ schedule_at_rosync(ଶୂନ୍ୟ)
{ ଫେରନ୍ତୁ sim_at_rosync; }

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
ସ୍ଥିର ଶୂନ୍ୟ run_rosync(ସଂରଚନା event_time_s*ctim)
{
      sim_at_rosync = ସତ୍ୟ;
      ଯେତେବେଳେ (ctim->rosync) {
	    ସଂରଚନା event_s*cur = ctim->rosync->next;
	    ଯଦି (cur->next == cur) {
		  ctim->rosync = 0;
	    } ଅନ୍ୟଥା {
		  ctim->rosync->next = cur->next;
	    }

	    cur->run_run();
	    delete cur;
      }
      sim_at_rosync = ମିଥ୍ୟା;

      ଯେତେବେଳେ (ctim->del_thr) {
	    ସଂରଚନା event_s*cur = ctim->del_thr->next;
	    ଯଦି (cur->next == cur) {
		  ctim->del_thr = 0;
	    } ଅନ୍ୟଥା {
		  ctim->del_thr->next = cur->next;
	    }

	    cur->run_run();
	    delete cur;
      }

      ଯଦି (ctim->active || ctim->inactive || ctim->nbassign || ctim->rwsync) {
	    cerr << "SCHEDULER ERROR: read-only sync events "
		 << "created RW events!" << endl;
      }
}

ଶୂନ୍ୟ schedule_simulate(ଶୂନ୍ୟ)
{
      ତାର୍କିକ run_finals;
      sim_started = ମିଥ୍ୟା;

      schedule_time = 0;

      ଯଦି (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute EndOfCompile callbacks\n");
      }

      // Execute end of compile callbacks
      vpiEndOfCompile();

      ଯଦି (verbose_flag) {
	    vpi_mcd_printf(1, " ...propagate initialization events\n");
      }

	// Execute initialization events.
      ଯେତେବେଳେ (schedule_init_list) {
	    ସଂରଚନା event_s*cur = schedule_init_list->next;
	    ଯଦି (cur->next == cur) {
		  schedule_init_list = 0;
	    } ଅନ୍ୟଥା {
		  schedule_init_list->next = cur->next;
	    }
	    cur->run_run();
	    delete cur;
      }

      sim_started = ସତ୍ୟ;

      ଯଦି (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute StartOfSim callbacks\n");
      }

      // Execute start of simulation callbacks
      vpiStartOfSim();

      signals_capture();

      ଯଦି (verbose_flag) {
	    vpi_mcd_printf(1, " ...run scheduler\n");
      }

      // If there were no compiletf, etc. errors then we are going to
      // process events and when done run the final blocks.
      run_finals = schedule_runnable;

      ଯଦି (schedule_runnable) ଯେତେବେଳେ (sched_list) {

	    ଯଦି (schedule_stopped_flag) {
		  schedule_stopped_flag = ମିଥ୍ୟା;
		  stop_handler(0);
		  // You can finish from the debugger without a time change.
		  ଯଦି (!schedule_runnable) ଭାଙ୍ଗ;
		  ଜାରି;
	    }

	      /* ctim is the current time step. */
	    ସଂରଚନା event_time_s* ctim = sched_list;

	      /* If the time is advancing, then first run the
		 postponed sync events. Run them all. */
	    ଯଦି (ctim->delay > 0) {

		  ଯଦି (!schedule_runnable) ଭାଙ୍ଗ;
		  schedule_time += ctim->delay;
		    /* When the design is being traced (we are emitting
		     * file/line information) also print any time changes. */
		  ଯଦି (show_file_ଧାଡ଼ି) {
			cerr << "Advancing to simulation time: "
			     << schedule_time << endl;
		  }
		  ctim->delay = 0;

		  vpiNextSimTime();
		    // Process the cbAtStartOfSimTime callbacks.
		  ଯେତେବେଳେ (ctim->start) {
			ସଂରଚନା event_s*cur = ctim->start->next;
			ଯଦି (cur->next == cur) {
			      ctim->start = 0;
			} ଅନ୍ୟଥା {
			      ctim->start->next = cur->next;
			}
			cur->run_run();
			delete (cur);
		  }
	    }


	      /* If there are no more active events, advance the event
		 queues. If there are not events at all, then release
		 the event_time object. */
	    ଯଦି (ctim->active == 0) {
		  ctim->active = ctim->inactive;
		  ctim->inactive = 0;

		  ଯଦି (ctim->active == 0) {
			ctim->active = ctim->nbassign;
			ctim->nbassign = 0;

			ଯଦି (ctim->active == 0) {
			      ctim->active = ctim->rwsync;
			      ctim->rwsync = 0;

				/* If out of rw events, then run the rosync
				   events and delete this time step. This also
				   deletes threads as needed. */
			      ଯଦି (ctim->active == 0) {
				    run_rosync(ctim);
				    sched_list = ctim->next;
				    delete ctim;
				    ଜାରି;
			      }
			}
		  }
	    }

	      /* Pull the first item off the list. If this is the last
		 cell in the list, then clear the list. Execute that
		 event type, and delete it. */
	    ସଂରଚନା event_s*cur = ctim->active->next;
	    ଯଦି (cur->next == cur) {
		  ctim->active = 0;
	    } ଅନ୍ୟଥା {
		  ctim->active->next = cur->next;
	    }

	    ଯଦି (schedule_single_step_flag) {
		  cur->single_step_display();
		  schedule_stopped_flag = ସତ୍ୟ;
		  schedule_single_step_flag = ମିଥ୍ୟା;
	    }

	    cur->run_run();

	    delete (cur);
      }

	// Execute final events.
      schedule_runnable = run_finals;
      ଯେତେବେଳେ (schedule_runnable && schedule_final_list) {
	    ସଂରଚନା event_s*cur = schedule_final_list->next;
	    ଯଦି (cur->next == cur) {
		  schedule_final_list = 0;
	    } ଅନ୍ୟଥା {
		  schedule_final_list->next = cur->next;
	    }
	    cur->run_run();
	    delete cur;
      }

      signals_revert();

      ଯଦି (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute Postsim callbacks\n");
      }

      // Execute post-simulation callbacks
      vpiPostsim();
#ifdef CHECK_WITH_VALGRIND
      schedule_delete();
#endif
}

#ifdef CHECK_WITH_VALGRIND
ଶୂନ୍ୟ schedule_delete(ଶୂନ୍ୟ)
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
