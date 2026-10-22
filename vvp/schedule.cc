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

using ನಾಮಾಕಾಶ std;

unsigned long ಎಣಿಕೆ_assign_events = 0;
unsigned long ಎಣಿಕೆ_gen_events = 0;
unsigned long ಎಣಿಕೆ_thread_events = 0;
  // Count the time events (A time cell created)
unsigned long ಎಣಿಕೆ_time_events = 0;



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
ಸಂರಚನಾ event_s {
      ಸಂರಚನಾ event_s*next;
      ಆಭಾಸ ~event_s() { }
      ಆಭಾಸ ಶೂನ್ಯ run_run(ಶೂನ್ಯ) =0;

	// Write something about the event to stderr
      ಆಭಾಸ ಶೂನ್ಯ single_step_display(ಶೂನ್ಯ);

	// Fallback new/delete
      ಸ್ಥಿರ ಶೂನ್ಯ*operator ನೂತನ (size_t size) { ಪ್ರತ್ಯಾಗಮ ::ನೂತನ ವರ್ಣ[size]; }
      ಸ್ಥಿರ ಶೂನ್ಯ operator delete(ಶೂನ್ಯ*ptr)  { ::delete[]( static_cast<ವರ್ಣ*>(ptr) ); }
};

ಶೂನ್ಯ event_s::single_step_display(ಶೂನ್ಯ)
{
      std::cerr << "event_s: Step into event " << typeid(*ಏತತ್).name() << std::endl;
}

ಸಂರಚನಾ event_time_s {
      event_time_s() {
	    ಎಣಿಕೆ_time_events += 1;
	    delay = 0;
	    start = 0;
	    active = 0;
	    inactive = 0;
	    nbassign = 0;
	    rwsync = 0;
	    rosync = 0;
	    del_thr = 0;
	    next = ರಿಕ್ತ;
      }
      vvp_time64_t delay;

      ಸಂರಚನಾ event_s*start;
      ಸಂರಚನಾ event_s*active;
      ಸಂರಚನಾ event_s*inactive;
      ಸಂರಚನಾ event_s*nbassign;
      ಸಂರಚನಾ event_s*rwsync;
      ಸಂರಚನಾ event_s*rosync;
      ಸಂರಚನಾ event_s*del_thr;

      ಸಂರಚನಾ event_time_s*next;

      ಸ್ಥಿರ ಶೂನ್ಯ* operator ನೂತನ (size_t);
      ಸ್ಥಿರ ಶೂನ್ಯ operator delete(ಶೂನ್ಯ*obj, size_t s);
};

vvp_gen_event_s::~vvp_gen_event_s()
{
}

ಶೂನ್ಯ vvp_gen_event_s::single_step_display(ಶೂನ್ಯ)
{
      cerr << "vvp_gen_event_s: Step into event " << typeid(*ಏತತ್).name() << endl;
}

/*
 * Derived event types
 */
ಸಂರಚನಾ vthread_event_s : ಸಾರ್ವಜನಿಕ event_s {
      vthread_t thr;
      ಶೂನ್ಯ run_run(ಶೂನ್ಯ) override;
      ಶೂನ್ಯ single_step_display(ಶೂನ್ಯ) override;

      ಸ್ಥಿರ ಶೂನ್ಯ* operator ನೂತನ(size_t);
      ಸ್ಥಿರ ಶೂನ್ಯ operator delete(ಶೂನ್ಯ*);
};

ಶೂನ್ಯ vthread_event_s::run_run(ಶೂನ್ಯ)
{
      ಎಣಿಕೆ_thread_events += 1;
      vthread_run(thr);
}

ಶೂನ್ಯ vthread_event_s::single_step_display(ಶೂನ್ಯ)
{
      __vpiScope*scope = vthread_scope(thr);
      cerr << "vthread_event: Resume thread"
	   << " scope=" << scope->vpi_get_str(vpiFullName)
	   << endl;
}

ಸ್ಥಿರ ಧ್ರುವ size_t VTHR_CHUNK_ಎಣಿಕೆ = 8192 / ಪರಿಮಾಣ(ಸಂರಚನಾ vthread_event_s);
ಸ್ಥಿರ slab_t<ಪರಿಮಾಣ(vthread_event_s),VTHR_CHUNK_ಎಣಿಕೆ> vthread_event_heap;

ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ* vthread_event_s::operator ನೂತನ(size_t size)
{
      ಪ್ರತಿಜ್ಞಾ(size == ಪರಿಮಾಣ(vthread_event_s));
      ಪ್ರತ್ಯಾಗಮ vthread_event_heap.alloc_slab();
}

ಶೂನ್ಯ vthread_event_s::operator delete(ಶೂನ್ಯ*dptr)
{
      vthread_event_heap.free_slab(dptr);
}

ಸಂರಚನಾ del_thr_event_s : ಸಾರ್ವಜನಿಕ event_s {
      vthread_t thr;
      ಶೂನ್ಯ run_run(ಶೂನ್ಯ) override;
      ಶೂನ್ಯ single_step_display(ಶೂನ್ಯ) override;
};

ಶೂನ್ಯ del_thr_event_s::run_run(ಶೂನ್ಯ)
{
      vthread_delete(thr);
}

ಶೂನ್ಯ del_thr_event_s::single_step_display(ಶೂನ್ಯ)
{
      __vpiScope*scope = vthread_scope(thr);
      cerr << "del_thr_event: Reap completed thread"
	   << " scope=" << scope->vpi_get_str(vpiFullName) << endl;
}

ಸಂರಚನಾ assign_vector4_event_s  : ಸಾರ್ವಜನಿಕ event_s {
	/* The default constructor. */
      explicit assign_vector4_event_s(ಧ್ರುವ vvp_vector4_t&that) : val(that) {
	    base = 0;
	    vwid = 0;
	    next = ರಿಕ್ತ;
      }

	/* Where to do the assign. */
      vvp_net_ptr_t ptr;
	/* Value to assign. */
      vvp_vector4_t val;
	/* Offset of the part into the destination. */
      unsigned base;
	/* Width of the destination vector. */
      unsigned vwid;
      ಶೂನ್ಯ run_run(ಶೂನ್ಯ) override;
      ಶೂನ್ಯ single_step_display(ಶೂನ್ಯ) override;

      ಸ್ಥಿರ ಶೂನ್ಯ* operator ನೂತನ(size_t);
      ಸ್ಥಿರ ಶೂನ್ಯ operator delete(ಶೂನ್ಯ*);
};

ಶೂನ್ಯ assign_vector4_event_s::run_run(ಶೂನ್ಯ)
{
      ಎಣಿಕೆ_assign_events += 1;
      ಯದಿ (vwid > 0)
	    vvp_send_vec4_pv(ptr, val, base, vwid, 0);
      ಅನ್ಯಥಾ
	    vvp_send_vec4(ptr, val, 0);
}

ಶೂನ್ಯ assign_vector4_event_s::single_step_display(ಶೂನ್ಯ)
{
      cerr << "assign_vector4_event: Propagate val=" << val
	   << ", vwid=" << vwid << ", base=" << base << endl;
}

ಸ್ಥಿರ ಧ್ರುವ size_t ASSIGN4_CHUNK_ಎಣಿಕೆ = 524288 / ಪರಿಮಾಣ(ಸಂರಚನಾ assign_vector4_event_s);
ಸ್ಥಿರ slab_t<ಪರಿಮಾಣ(assign_vector4_event_s),ASSIGN4_CHUNK_ಎಣಿಕೆ> assign4_heap;

ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ* assign_vector4_event_s::operator ನೂತನ(size_t size)
{
      ಪ್ರತಿಜ್ಞಾ(size == ಪರಿಮಾಣ(assign_vector4_event_s));
      ಪ್ರತ್ಯಾಗಮ assign4_heap.alloc_slab();
}

ಶೂನ್ಯ assign_vector4_event_s::operator delete(ಶೂನ್ಯ*dptr)
{
      assign4_heap.free_slab(dptr);
}

unsigned long ಎಣಿಕೆ_assign4_pool(ಶೂನ್ಯ) { ಪ್ರತ್ಯಾಗಮ assign4_heap.pool; }

ಸಂರಚನಾ assign_vector8_event_s  : ಸಾರ್ವಜನಿಕ event_s {
      vvp_net_ptr_t ptr;
      vvp_vector8_t val;
      ಶೂನ್ಯ run_run(ಶೂನ್ಯ) override;
      ಶೂನ್ಯ single_step_display(ಶೂನ್ಯ) override;

      ಸ್ಥಿರ ಶೂನ್ಯ* operator ನೂತನ(size_t);
      ಸ್ಥಿರ ಶೂನ್ಯ operator delete(ಶೂನ್ಯ*);
};

ಶೂನ್ಯ assign_vector8_event_s::run_run(ಶೂನ್ಯ)
{
      ಎಣಿಕೆ_assign_events += 1;
      vvp_send_vec8(ptr, val);
}

ಶೂನ್ಯ assign_vector8_event_s::single_step_display(ಶೂನ್ಯ)
{
      cerr << "assign_vector8_event: Propagate val=" << val << endl;
}

ಸ್ಥಿರ ಧ್ರುವ size_t ASSIGN8_CHUNK_ಎಣಿಕೆ = 8192 / ಪರಿಮಾಣ(ಸಂರಚನಾ assign_vector8_event_s);
ಸ್ಥಿರ slab_t<ಪರಿಮಾಣ(assign_vector8_event_s),ASSIGN8_CHUNK_ಎಣಿಕೆ> assign8_heap;

ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ* assign_vector8_event_s::operator ನೂತನ(size_t size)
{
      ಪ್ರತಿಜ್ಞಾ(size == ಪರಿಮಾಣ(assign_vector8_event_s));
      ಪ್ರತ್ಯಾಗಮ assign8_heap.alloc_slab();
}

ಶೂನ್ಯ assign_vector8_event_s::operator delete(ಶೂನ್ಯ*dptr)
{
      assign8_heap.free_slab(dptr);
}

unsigned long ಎಣಿಕೆ_assign8_pool() { ಪ್ರತ್ಯಾಗಮ assign8_heap.pool; }

ಸಂರಚನಾ assign_real_event_s  : ಸಾರ್ವಜನಿಕ event_s {
      vvp_net_ptr_t ptr;
      ದ್ವಿಗುಣ val;
      ಶೂನ್ಯ run_run(ಶೂನ್ಯ) override;
      ಶೂನ್ಯ single_step_display(ಶೂನ್ಯ) override;

      ಸ್ಥಿರ ಶೂನ್ಯ* operator ನೂತನ(size_t);
      ಸ್ಥಿರ ಶೂನ್ಯ operator delete(ಶೂನ್ಯ*);
};

ಶೂನ್ಯ assign_real_event_s::run_run(ಶೂನ್ಯ)
{
      ಎಣಿಕೆ_assign_events += 1;
      vvp_send_real(ptr, val, 0);
}

ಶೂನ್ಯ assign_real_event_s::single_step_display(ಶೂನ್ಯ)
{
      cerr << "assign_real_event: Propagate val=" << val << endl;
}

ಸ್ಥಿರ ಧ್ರುವ size_t ASSIGNR_CHUNK_ಎಣಿಕೆ = 8192 / ಪರಿಮಾಣ(ಸಂರಚನಾ assign_real_event_s);
ಸ್ಥಿರ slab_t<ಪರಿಮಾಣ(assign_real_event_s),ASSIGNR_CHUNK_ಎಣಿಕೆ> assignr_heap;

ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ* assign_real_event_s::operator ನೂತನ (size_t size)
{
      ಪ್ರತಿಜ್ಞಾ(size == ಪರಿಮಾಣ(assign_real_event_s));
      ಪ್ರತ್ಯಾಗಮ assignr_heap.alloc_slab();
}

ಶೂನ್ಯ assign_real_event_s::operator delete(ಶೂನ್ಯ*dptr)
{
      assignr_heap.free_slab(dptr);
}

unsigned long ಎಣಿಕೆ_assign_real_pool(ಶೂನ್ಯ) { ಪ್ರತ್ಯಾಗಮ assignr_heap.pool; }

ಸಂರಚನಾ assign_array_word_s  : ಸಾರ್ವಜನಿಕ event_s {
      vvp_array_t mem;
      unsigned adr;
      vvp_vector4_t val;
      unsigned off;
      ಶೂನ್ಯ run_run(ಶೂನ್ಯ) override;

      ಸ್ಥಿರ ಶೂನ್ಯ* operator ನೂತನ(size_t);
      ಸ್ಥಿರ ಶೂನ್ಯ operator delete(ಶೂನ್ಯ*);
};

ಶೂನ್ಯ assign_array_word_s::run_run(ಶೂನ್ಯ)
{
      ಎಣಿಕೆ_assign_events += 1;
      mem->set_word(adr, off, val);
}

ಸ್ಥಿರ ಧ್ರುವ size_t ARRAY_W_CHUNK_ಎಣಿಕೆ = 8192 / ಪರಿಮಾಣ(ಸಂರಚನಾ assign_array_word_s);
ಸ್ಥಿರ slab_t<ಪರಿಮಾಣ(assign_array_word_s),ARRAY_W_CHUNK_ಎಣಿಕೆ> array_w_heap;

ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ* assign_array_word_s::operator ನೂತನ (size_t size)
{
      ಪ್ರತಿಜ್ಞಾ(size == ಪರಿಮಾಣ(assign_array_word_s));
      ಪ್ರತ್ಯಾಗಮ array_w_heap.alloc_slab();
}

ಶೂನ್ಯ assign_array_word_s::operator delete(ಶೂನ್ಯ*ptr)
{
      array_w_heap.free_slab(ptr);
}

unsigned long ಎಣಿಕೆ_assign_aword_pool(ಶೂನ್ಯ) { ಪ್ರತ್ಯಾಗಮ array_w_heap.pool; }

ಸಂರಚನಾ force_vector4_event_s  : ಸಾರ್ವಜನಿಕ event_s {
	/* The default constructor. */
      explicit force_vector4_event_s(ಧ್ರುವ vvp_vector4_t&that): val(that) {
	    net = ರಿಕ್ತ;
	    base = 0;
	    vwid = 0;
	    next = ರಿಕ್ತ;
      }
	/* Where to do the force. */
      vvp_net_t*net;
	/* Value to force. */
      vvp_vector4_t val;
	/* Offset of the part into the destination. */
      unsigned base;
	/* Width of the destination vector. */
      unsigned vwid;

      ಶೂನ್ಯ run_run(ಶೂನ್ಯ) override;
      ಶೂನ್ಯ single_step_display(ಶೂನ್ಯ) override;

      ಸ್ಥಿರ ಶೂನ್ಯ* operator ನೂತನ(size_t);
      ಸ್ಥಿರ ಶೂನ್ಯ operator delete(ಶೂನ್ಯ*);
};

ಶೂನ್ಯ force_vector4_event_s::run_run(ಶೂನ್ಯ)
{
      ಎಣಿಕೆ_assign_events += 1;

      unsigned wid = val.size();
      ಯದಿ ((base + wid) > vwid)
	    wid = vwid - base;

	// Make a mask of which bits are to be forced, 0 for unforced
	// bits and 1 for forced bits.
      vvp_vector2_t mask (vvp_vector2_t::FILL0, vwid);
      ಪ್ರತಿ (unsigned idx = 0 ; idx < wid ; idx += 1)
	    mask.set_bit(base+idx, 1);

      vvp_vector4_t tmp (vwid, BIT4_Z);

	// vvp_net_t::force_vec4 propagates all the bits of the
	// forced vector value, regardless of the mask. This
	// ensures the unforced bits retain their current value.
      vvp_signal_ಮೌಲ್ಯ*sig = dynamic_cast<vvp_signal_ಮೌಲ್ಯ*>(net->fil);
      ಪ್ರತಿಜ್ಞಾ(sig);
      sig->vec4_ಮೌಲ್ಯ(tmp);

      tmp.set_vec(base, val);
      net->force_vec4(tmp, mask);
}

ಶೂನ್ಯ force_vector4_event_s::single_step_display(ಶೂನ್ಯ)
{
      cerr << "force_vector4_event: Force val=" << val
	   << ", vwid=" << vwid << ", base=" << base << endl;
}

ಸ್ಥಿರ ಧ್ರುವ size_t FORCE4_CHUNK_ಎಣಿಕೆ = 8192 / ಪರಿಮಾಣ(ಸಂರಚನಾ force_vector4_event_s);
ಸ್ಥಿರ slab_t<ಪರಿಮಾಣ(force_vector4_event_s),FORCE4_CHUNK_ಎಣಿಕೆ> force4_heap;

ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ* force_vector4_event_s::operator ನೂತನ(size_t size)
{
      ಪ್ರತಿಜ್ಞಾ(size == ಪರಿಮಾಣ(force_vector4_event_s));
      ಪ್ರತ್ಯಾಗಮ force4_heap.alloc_slab();
}

ಶೂನ್ಯ force_vector4_event_s::operator delete(ಶೂನ್ಯ*dptr)
{
      force4_heap.free_slab(dptr);
}

unsigned long ಎಣಿಕೆ_force4_pool(ಶೂನ್ಯ) { ಪ್ರತ್ಯಾಗಮ force4_heap.pool; }

/*
 * This class supports the propagation of vec4 outputs from a
 * vvp_net_t object.
 */
ಸಂರಚನಾ propagate_vector4_event_s : ಸಾರ್ವಜನಿಕ event_s {
	/* The default constructor. */
      explicit propagate_vector4_event_s(ಧ್ರುವ vvp_vector4_t&that) : val(that) {
	    net = ರಿಕ್ತ;
	    next = ರಿಕ್ತ;
      }
	/* A constructor that makes the val directly. */
      propagate_vector4_event_s(ಧ್ರುವ vvp_vector4_t&that, unsigned adr, unsigned wid)
      : val(that,adr,wid) {
	    net = ರಿಕ್ತ;
	    next = ರಿಕ್ತ;
      }

	/* Propagate the output of this net. */
      vvp_net_t*net;
	/* value to propagate */
      vvp_vector4_t val;
	/* Action */
      ಶೂನ್ಯ run_run(ಶೂನ್ಯ) override;
      ಶೂನ್ಯ single_step_display(ಶೂನ್ಯ) override;
};

ಶೂನ್ಯ propagate_vector4_event_s::run_run(ಶೂನ್ಯ)
{
      net->send_vec4(val, 0);
}

ಶೂನ್ಯ propagate_vector4_event_s::single_step_display(ಶೂನ್ಯ)
{
      cerr << "propagate_vector4_event: Propagate val=" << val << endl;
}

/*
 * This class supports the propagation of real outputs from a
 * vvp_net_t object.
 */
ಸಂರಚನಾ propagate_real_event_s : ಸಾರ್ವಜನಿಕ event_s {
	/* Propagate the output of this net. */
      vvp_net_t*net;
	/* value to propagate */
      ದ್ವಿಗುಣ val;
	/* Action */
      ಶೂನ್ಯ run_run(ಶೂನ್ಯ) override;
      ಶೂನ್ಯ single_step_display(ಶೂನ್ಯ) override;
};

ಶೂನ್ಯ propagate_real_event_s::run_run(ಶೂನ್ಯ)
{
      net->send_real(val, 0);
}

ಶೂನ್ಯ propagate_real_event_s::single_step_display(ಶೂನ್ಯ)
{
      cerr << "propagate_real_event: Propagate val=" << val << endl;
}

ಸಂರಚನಾ assign_array_r_word_s  : ಸಾರ್ವಜನಿಕ event_s {
      vvp_array_t mem;
      unsigned adr;
      ದ್ವಿಗುಣ val;
      ಶೂನ್ಯ run_run(ಶೂನ್ಯ) override;

      ಸ್ಥಿರ ಶೂನ್ಯ* operator ನೂತನ(size_t);
      ಸ್ಥಿರ ಶೂನ್ಯ operator delete(ಶೂನ್ಯ*);
};

ಶೂನ್ಯ assign_array_r_word_s::run_run(ಶೂನ್ಯ)
{
      ಎಣಿಕೆ_assign_events += 1;
      mem->set_word(adr, val);
}
ಸ್ಥಿರ ಧ್ರುವ size_t ARRAY_R_W_CHUNK_ಎಣಿಕೆ = 8192 / ಪರಿಮಾಣ(ಸಂರಚನಾ assign_array_r_word_s);
ಸ್ಥಿರ slab_t<ಪರಿಮಾಣ(assign_array_r_word_s),ARRAY_R_W_CHUNK_ಎಣಿಕೆ> array_r_w_heap;

ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ* assign_array_r_word_s::operator ನೂತನ(size_t size)
{
      ಪ್ರತಿಜ್ಞಾ(size == ಪರಿಮಾಣ(assign_array_r_word_s));
      ಪ್ರತ್ಯಾಗಮ array_r_w_heap.alloc_slab();
}

ಶೂನ್ಯ assign_array_r_word_s::operator delete(ಶೂನ್ಯ*ptr)
{
      array_r_w_heap.free_slab(ptr);
}

unsigned long ಎಣಿಕೆ_assign_arword_pool(ಶೂನ್ಯ) { ಪ್ರತ್ಯಾಗಮ array_r_w_heap.pool; }

ಸಂರಚನಾ generic_event_s : ಸಾರ್ವಜನಿಕ event_s {
      vvp_gen_event_t obj;
      ತಾರ್ಕಿಕ delete_obj_when_done;
      ಶೂನ್ಯ run_run(ಶೂನ್ಯ) override;
      ಶೂನ್ಯ single_step_display(ಶೂನ್ಯ) override;

      ಸ್ಥಿರ ಶೂನ್ಯ* operator ನೂತನ(size_t);
      ಸ್ಥಿರ ಶೂನ್ಯ operator delete(ಶೂನ್ಯ*);
};

ಶೂನ್ಯ generic_event_s::run_run(ಶೂನ್ಯ)
{
      ಎಣಿಕೆ_gen_events += 1;
      ಯದಿ (obj) {
	    obj->run_run();
	    ಯದಿ (delete_obj_when_done)
		  delete obj;
      }
}

ಶೂನ್ಯ generic_event_s::single_step_display(ಶೂನ್ಯ)
{
      obj->single_step_display();
}

ಸ್ಥಿರ ಧ್ರುವ size_t GENERIC_CHUNK_ಎಣಿಕೆ = 131072 / ಪರಿಮಾಣ(ಸಂರಚನಾ generic_event_s);
ಸ್ಥಿರ slab_t<ಪರಿಮಾಣ(generic_event_s),GENERIC_CHUNK_ಎಣಿಕೆ> generic_event_heap;

ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ* generic_event_s::operator ನೂತನ(size_t size)
{
      ಪ್ರತಿಜ್ಞಾ(size == ಪರಿಮಾಣ(generic_event_s));
      ಪ್ರತ್ಯಾಗಮ generic_event_heap.alloc_slab();
}

ಶೂನ್ಯ generic_event_s::operator delete(ಶೂನ್ಯ*ptr)
{
      generic_event_heap.free_slab(ptr);
}

unsigned long ಎಣಿಕೆ_gen_pool(ಶೂನ್ಯ) { ಪ್ರತ್ಯಾಗಮ generic_event_heap.pool; }

/*
** These event_time_s will be required a lot, at high frequency.
** Once allocated, we never free them, but stash them away for next time.
*/


ಸ್ಥಿರ ಧ್ರುವ size_t TIME_CHUNK_ಎಣಿಕೆ = 8192 / ಪರಿಮಾಣ(ಸಂರಚನಾ event_time_s);
ಸ್ಥಿರ slab_t<ಪರಿಮಾಣ(event_time_s),TIME_CHUNK_ಎಣಿಕೆ> event_time_heap;

ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ* event_time_s::operator ನೂತನ (size_t size)
{
      ಪ್ರತಿಜ್ಞಾ(size == ಪರಿಮಾಣ(ಸಂರಚನಾ event_time_s));
      ಶೂನ್ಯ*ptr = event_time_heap.alloc_slab();
      ಪ್ರತ್ಯಾಗಮ ptr;
}

ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ event_time_s::operator delete(ಶೂನ್ಯ*ptr, size_t)
{
      event_time_heap.free_slab(ptr);
}

unsigned long ಎಣಿಕೆ_time_pool(ಶೂನ್ಯ) { ಪ್ರತ್ಯಾಗಮ event_time_heap.pool; }

/*
 * This is the head of the list of pending events. This includes all
 * the events that have not been executed yet, and reaches into the
 * future.
 */
ಸ್ಥಿರ ಸಂರಚನಾ event_time_s* sched_list = 0;

/*
 * This is a list of initialization events. The setup puts
 * initializations in this list so that they happen before the
 * simulation as a whole starts. This prevents time-0 triggers of
 * certain events.
 */
ಸ್ಥಿರ ಸಂರಚನಾ event_s* schedule_init_list = 0;

/*
 * This is the head of the list of final events.
 */
ಸ್ಥಿರ ಸಂರಚನಾ event_s* schedule_final_list = 0;

/*
 * This flag is true until a VPI task or function finishes the
 * simulation.
 */
ಸ್ಥಿರ ತಾರ್ಕಿಕ schedule_runnable = ಸತ್ಯ;
ಸ್ಥಿರ ತಾರ್ಕಿಕ schedule_stopped_flag  = ಅಸತ್ಯ;
ಸ್ಥಿರ ತಾರ್ಕಿಕ schedule_single_step_flag = ಅಸತ್ಯ;
ಸ್ಥಿರ ತಾರ್ಕಿಕ no_signals_flag = ಅಸತ್ಯ;

ಶೂನ್ಯ schedule_finish(ಪೂರ್ಣಾಙ್ಕ)
{
      schedule_runnable = ಅಸತ್ಯ;
}

ಶೂನ್ಯ schedule_stop(ಪೂರ್ಣಾಙ್ಕ)
{
      schedule_stopped_flag = ಸತ್ಯ;
}

ಶೂನ್ಯ schedule_single_step(ಪೂರ್ಣಾಙ್ಕ)
{
      schedule_single_step_flag = ಸತ್ಯ;
}

ತಾರ್ಕಿಕ schedule_finished(ಶೂನ್ಯ)
{
      ಪ್ರತ್ಯಾಗಮ !schedule_runnable;
}

ತಾರ್ಕಿಕ schedule_stopped(ಶೂನ್ಯ)
{
      ಪ್ರತ್ಯಾಗಮ schedule_stopped_flag;
}

ಬಾಹ್ಯ "C" ಶೂನ್ಯ vvp_no_signals(ಶೂನ್ಯ)
{
      no_signals_flag = ಸತ್ಯ;
}

/*
 * These are the signal handling infrastructure. The SIGINT signal
 * leads to an implicit $stop. The SIGHUP and SIGTERM signals lead
 * to an implicit $finish.
 */
ಬಾಹ್ಯ ತಾರ್ಕಿಕ stop_is_finish;

ಬಾಹ್ಯ "C" ಶೂನ್ಯ signals_handler(ಪೂರ್ಣಾಙ್ಕ signum)
{
#ifdef __MINGW32__
	// Windows implements the original UNIX semantics for signal,
	// so we have to re-establish the signal handler each time a
	// signal is caught.
      ಸಙ್ಕೇತ(signum, &signals_handler);
#endif
      ಯದಿ (signum != SIGINT)
	    stop_is_finish = ಸತ್ಯ;
      schedule_stopped_flag = ಸತ್ಯ;
}

ಸ್ಥಿರ ಶೂನ್ಯ signals_capture(ಶೂನ್ಯ)
{
      ಯದಿ (no_signals_flag)
	   ಪ್ರತ್ಯಾಗಮ;
#ifndef __MINGW32__
      ಸಙ್ಕೇತ(SIGHUP,  &signals_handler);
#endif
      ಸಙ್ಕೇತ(SIGINT,  &signals_handler);
      ಸಙ್ಕೇತ(SIGTERM, &signals_handler);
}

ಸ್ಥಿರ ಶೂನ್ಯ signals_revert(ಶೂನ್ಯ)
{
      ಯದಿ (no_signals_flag)
	   ಪ್ರತ್ಯಾಗಮ;
#ifndef __MINGW32__
      ಸಙ್ಕೇತ(SIGHUP,  SIG_DFL);
#endif
      ಸಙ್ಕೇತ(SIGINT,  SIG_DFL);
      ಸಙ್ಕೇತ(SIGTERM, SIG_DFL);
}

/*
 * This function puts an event on the end of the pre-simulation event queue.
 */
ಸ್ಥಿರ ಶೂನ್ಯ schedule_init_event(ಸಂರಚನಾ event_s*cur)
{
      ಯದಿ (schedule_init_list == 0) {
            cur->next = cur;
      } ಅನ್ಯಥಾ {
            cur->next = schedule_init_list->next;
            schedule_init_list->next = cur;
      }
      schedule_init_list = cur;
}

/*
 * This function puts an event on the end of the post-simulation event queue.
 */
ಸ್ಥಿರ ಶೂನ್ಯ schedule_final_event(ಸಂರಚನಾ event_s*cur)
{
      ಯದಿ (schedule_final_list == 0) {
            cur->next = cur;
      } ಅನ್ಯಥಾ {
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
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ event_queue_e { SEQ_START, SEQ_ACTIVE, SEQ_INACTIVE, SEQ_NBASSIGN,
			     SEQ_RWSYNC, SEQ_ROSYNC, DEL_THREAD } event_queue_t;

ಸ್ಥಿರ ಶೂನ್ಯ schedule_event_(ಸಂರಚನಾ event_s*cur, vvp_time64_t delay,
			    event_queue_t select_queue)
{
      cur->next = cur;
      ಸಂರಚನಾ event_time_s*ctim = sched_list;

      ಯದಿ (sched_list == 0) {
	      /* Is the event_time list completely empty? Create the
		 first event_time object. */
	    ctim = ನೂತನ ಸಂರಚನಾ event_time_s;
	    ctim->delay = delay;
	    ctim->next  = 0;
	    sched_list = ctim;

      } ಅನ್ಯಥಾ ಯದಿ (sched_list->delay > delay) {

	      /* Am I looking for an event before the first event_time?
		 If so, create a new event_time to go in front. */
	    ಸಂರಚನಾ event_time_s*tmp = ನೂತನ ಸಂರಚನಾ event_time_s;
	    tmp->delay = delay;
	    tmp->next = ctim;
	    ctim->delay -= delay;
	    ctim = tmp;
	    sched_list = ctim;

      } ಅನ್ಯಥಾ {
	    ಸಂರಚನಾ event_time_s*prev = 0;

	    ಯಾವತ್ (ctim->next && (ctim->delay < delay)) {
		  delay -= ctim->delay;
		  prev = ctim;
		  ctim = ctim->next;
	    }

	    ಯದಿ (ctim->delay > delay) {
		  ಸಂರಚನಾ event_time_s*tmp = ನೂತನ ಸಂರಚನಾ event_time_s;
		  tmp->delay = delay;
		  tmp->next  = prev->next;
		  prev->next = tmp;

		  tmp->next->delay -= delay;
		  ctim = tmp;

	    } ಅನ್ಯಥಾ ಯದಿ (ctim->delay == delay) {

	    } ಅನ್ಯಥಾ {
		  ಪ್ರತಿಜ್ಞಾ(ctim->next == 0);
		  ಸಂರಚನಾ event_time_s*tmp = ನೂತನ ಸಂರಚನಾ event_time_s;
		  tmp->delay = delay - ctim->delay;
		  tmp->next = 0;
		  ctim->next = tmp;

		  ctim = tmp;
	    }
      }

	/* By this point, ctim is the event_time structure that is to
	   receive the event at hand. Put the event in to the
	   appropriate list for the kind of assign we have at hand. */

      ಸಂರಚನಾ event_s** q = 0;

      ವಿಕಲ್ಪ (select_queue) {
	  ಪಕ್ಷ SEQ_START:
	    q = &ctim->start;
	    ಭಙ್ಗ;

	  ಪಕ್ಷ SEQ_ACTIVE:
	    q = &ctim->active;
	    ಭಙ್ಗ;

	  ಪಕ್ಷ SEQ_INACTIVE:
	    ಪ್ರತಿಜ್ಞಾ(delay == 0);
	    q = &ctim->inactive;
	    ಭಙ್ಗ;

	  ಪಕ್ಷ SEQ_NBASSIGN:
	    q = &ctim->nbassign;
	    ಭಙ್ಗ;

	  ಪಕ್ಷ SEQ_RWSYNC:
	    q = &ctim->rwsync;
	    ಭಙ್ಗ;

	  ಪಕ್ಷ SEQ_ROSYNC:
	    q = &ctim->rosync;
	    ಭಙ್ಗ;

	  ಪಕ್ಷ DEL_THREAD:
	    q = &ctim->del_thr;
	    ಭಙ್ಗ;
      }

      ಯದಿ (q) {
	    ಯದಿ (*q) {
		  /* Put the cur event on the end of the queue. */
		  cur->next = (*q)->next;
		  (*q)->next = cur;
	    }
	    *q = cur;
      }
}

ಸ್ಥಿರ ಶೂನ್ಯ schedule_event_ತಳ್ಳು(ಸಂರಚನಾ event_s*cur)
{
      ಯದಿ ((sched_list == 0) || (sched_list->delay > 0)) {
	    schedule_event_(cur, 0, SEQ_ACTIVE);
	    ಪ್ರತ್ಯಾಗಮ;
      }

      ಸಂರಚನಾ event_time_s*ctim = sched_list;

      ಯದಿ (ctim->active == 0) {
	    cur->next = cur;
	    ctim->active = cur;
	    ಪ್ರತ್ಯಾಗಮ;
      }

      cur->next = ctim->active->next;
      ctim->active->next = cur;
}

ಶೂನ್ಯ schedule_vthread(vthread_t thr, vvp_time64_t delay, ತಾರ್ಕಿಕ ತಳ್ಳು_flag)
{
      ಸಂರಚನಾ vthread_event_s*cur = ನೂತನ vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);

      ಯದಿ (ತಳ್ಳು_flag && (delay == 0)) {
	      /* Special case: If the delay is 0, the push_flag means
		 I can push this event in front of everything. This is
		 used by the %fork statement, for example, to perform
		 task calls. */
	    schedule_event_ತಳ್ಳು(cur);

      } ಅನ್ಯಥಾ {
	    schedule_event_(cur, delay, SEQ_ACTIVE);
      }
}

ಶೂನ್ಯ schedule_t0_trigger(vvp_net_ptr_t ptr)
{
      vvp_vector4_t bit (1, BIT4_X);
      ಸಂರಚನಾ assign_vector4_event_s*cur = ನೂತನ ಸಂರಚನಾ assign_vector4_event_s(bit);
      cur->ptr = ptr;
      schedule_event_(cur, 0, SEQ_INACTIVE);
}

ಶೂನ್ಯ schedule_inactive(vthread_t thr)
{
      ಸಂರಚನಾ vthread_event_s*cur = ನೂತನ vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);
      schedule_event_(cur, 0, SEQ_INACTIVE);
}

ಶೂನ್ಯ schedule_init_vthread(vthread_t thr)
{
      ಸಂರಚನಾ vthread_event_s*cur = ನೂತನ vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);

      schedule_init_event(cur);
}

ಶೂನ್ಯ schedule_final_vthread(vthread_t thr)
{
      ಸಂರಚನಾ vthread_event_s*cur = ನೂತನ vthread_event_s;

      cur->thr = thr;
      vthread_mark_final(thr);
      vthread_mark_scheduled(thr);

      schedule_final_event(cur);
}

ಶೂನ್ಯ schedule_assign_vector(vvp_net_ptr_t ptr,
			    unsigned base, unsigned vwid,
			    ಧ್ರುವ vvp_vector4_t&bit,
			    vvp_time64_t delay)
{
      ಸಂರಚನಾ assign_vector4_event_s*cur = ನೂತನ ಸಂರಚನಾ assign_vector4_event_s(bit);
      cur->ptr = ptr;
      cur->base = base;
      cur->vwid = vwid;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

ಶೂನ್ಯ schedule_force_vector(vvp_net_t*net,
			    unsigned base, unsigned vwid,
			    ಧ್ರುವ vvp_vector4_t&bit,
			    vvp_time64_t delay)
{
      ಸಂರಚನಾ force_vector4_event_s*cur = ನೂತನ ಸಂರಚನಾ force_vector4_event_s(bit);
      cur->net = net;
      cur->base = base;
      cur->vwid = vwid;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

ಶೂನ್ಯ schedule_propagate_vector(vvp_net_t*net,
			       vvp_time64_t delay,
			       ಧ್ರುವ vvp_vector4_t&src)
{
      ಸಂರಚನಾ propagate_vector4_event_s*cur
	    = ನೂತನ ಸಂರಚನಾ propagate_vector4_event_s(src);
      cur->net = net;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

// FIXME: This needs to create a non-blocking event, but only one per time slot.
//        Is schedule_event_ or execution actually filtering since the net is
//        already X because this is not triggering?
ಶೂನ್ಯ schedule_propagate_event(vvp_net_t*net,
                              vvp_time64_t delay)
{
      vvp_vector4_t tmp (1, BIT4_X);
      ಸಂರಚನಾ propagate_vector4_event_s*cur
	    = ನೂತನ ಸಂರಚನಾ propagate_vector4_event_s(tmp);
      cur->net = net;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

ಶೂನ್ಯ schedule_assign_array_word(vvp_array_t mem,
				unsigned word_addr,
				unsigned off,
				ಧ್ರುವ vvp_vector4_t&val,
				vvp_time64_t delay)
{
      ಸಂರಚನಾ assign_array_word_s*cur = ನೂತನ ಸಂರಚನಾ assign_array_word_s;
      cur->mem = mem;
      cur->adr = word_addr;
      cur->off = off;
      cur->val = val;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

ಶೂನ್ಯ schedule_assign_array_word(vvp_array_t mem,
				unsigned word_addr,
				ದ್ವಿಗುಣ val,
				vvp_time64_t delay)
{
      ಸಂರಚನಾ assign_array_r_word_s*cur = ನೂತನ ಸಂರಚನಾ assign_array_r_word_s;
      cur->mem = mem;
      cur->adr = word_addr;
      cur->val = val;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

ಶೂನ್ಯ schedule_set_vector(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit)
{
      ಸಂರಚನಾ assign_vector4_event_s*cur = ನೂತನ ಸಂರಚನಾ assign_vector4_event_s(bit);
      cur->ptr = ptr;
      cur->base = 0;
      cur->vwid = 0;
      schedule_event_(cur, 0, SEQ_ACTIVE);
}

ಶೂನ್ಯ schedule_set_vector(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector8_t&bit)
{
      ಸಂರಚನಾ assign_vector8_event_s*cur = ನೂತನ ಸಂರಚನಾ assign_vector8_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_event_(cur, 0, SEQ_ACTIVE);
}

ಶೂನ್ಯ schedule_set_vector(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ bit)
{
      ಸಂರಚನಾ assign_real_event_s*cur = ನೂತನ ಸಂರಚನಾ assign_real_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_event_(cur, 0, SEQ_ACTIVE);
}

ಶೂನ್ಯ schedule_init_vector(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit)
{
      ಸಂರಚನಾ assign_vector4_event_s*cur = ನೂತನ ಸಂರಚನಾ assign_vector4_event_s(bit);
      cur->ptr = ptr;
      cur->base = 0;
      cur->vwid = 0;
      schedule_init_event(cur);
}

ಶೂನ್ಯ schedule_init_vector(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector8_t&bit)
{
      ಸಂರಚನಾ assign_vector8_event_s*cur = ನೂತನ ಸಂರಚನಾ assign_vector8_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_init_event(cur);
}

ಶೂನ್ಯ schedule_init_vector(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ bit)
{
      ಸಂರಚನಾ assign_real_event_s*cur = ನೂತನ ಸಂರಚನಾ assign_real_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_init_event(cur);
}

ಶೂನ್ಯ schedule_init_propagate(vvp_net_t*net, vvp_vector4_t bit)
{
      ಸಂರಚನಾ propagate_vector4_event_s*cur = ನೂತನ ಸಂರಚನಾ propagate_vector4_event_s(bit);
      cur->net = net;
      schedule_init_event(cur);
}

ಶೂನ್ಯ schedule_init_propagate(vvp_net_t*net, ದ್ವಿಗುಣ bit)
{
      ಸಂರಚನಾ propagate_real_event_s*cur = ನೂತನ ಸಂರಚನಾ propagate_real_event_s;
      cur->net = net;
      cur->val = bit;
      schedule_init_event(cur);
}

ಶೂನ್ಯ schedule_del_thr(vthread_t thr)
{
      ಸಂರಚನಾ del_thr_event_s*cur = ನೂತನ del_thr_event_s;

      cur->thr = thr;

      schedule_event_(cur, 0, DEL_THREAD);
}

ಶೂನ್ಯ schedule_generic(vvp_gen_event_t obj, vvp_time64_t delay,
		      ತಾರ್ಕಿಕ sync_flag, ತಾರ್ಕಿಕ ro_flag, ತಾರ್ಕಿಕ delete_when_done)
{
      ಸಂರಚನಾ generic_event_s*cur = ನೂತನ generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = delete_when_done;
      schedule_event_(cur, delay,
		      sync_flag? (ro_flag?SEQ_ROSYNC:SEQ_RWSYNC) : SEQ_ACTIVE);

      ಯದಿ (sync_flag)
	    vthread_delay_delete();
}

ಸ್ಥಿರ ತಾರ್ಕಿಕ sim_started;

ಶೂನ್ಯ schedule_functor(vvp_gen_event_t obj)
{
      ಸಂರಚನಾ generic_event_s*cur = ನೂತನ generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = ಅಸತ್ಯ;
      ಯದಿ (!sim_started) {
            schedule_init_event(cur);
      } ಅನ್ಯಥಾ {
            schedule_event_(cur, 0, SEQ_ACTIVE);
      }
}

ಶೂನ್ಯ schedule_at_start_of_simtime(vvp_gen_event_t obj, vvp_time64_t delay)
{
      ಸಂರಚನಾ generic_event_s*cur = ನೂತನ generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = ಅಸತ್ಯ;
      schedule_event_(cur, delay, SEQ_START);
}

/*
 * In the vvp runtime of Icarus Verilog, the SEQ_RWSYNC time step is
 * after all of the non-blocking assignments, so is effectively the
 * same as the ReadWriteSync time.
 */
ಶೂನ್ಯ schedule_at_end_of_simtime(vvp_gen_event_t obj, vvp_time64_t delay)
{
      ಸಂರಚನಾ generic_event_s*cur = ನೂತನ generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = ಅಸತ್ಯ;
      schedule_event_(cur, delay, SEQ_RWSYNC);
}

ಸ್ಥಿರ vvp_time64_t schedule_time;
vvp_time64_t schedule_simtime(ಶೂನ್ಯ)
{ ಪ್ರತ್ಯಾಗಮ schedule_time; }

ಸ್ಥಿರ ತಾರ್ಕಿಕ sim_at_rosync = ಅಸತ್ಯ;
ತಾರ್ಕಿಕ schedule_at_rosync(ಶೂನ್ಯ)
{ ಪ್ರತ್ಯಾಗಮ sim_at_rosync; }

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
ಸ್ಥಿರ ಶೂನ್ಯ run_rosync(ಸಂರಚನಾ event_time_s*ctim)
{
      sim_at_rosync = ಸತ್ಯ;
      ಯಾವತ್ (ctim->rosync) {
	    ಸಂರಚನಾ event_s*cur = ctim->rosync->next;
	    ಯದಿ (cur->next == cur) {
		  ctim->rosync = 0;
	    } ಅನ್ಯಥಾ {
		  ctim->rosync->next = cur->next;
	    }

	    cur->run_run();
	    delete cur;
      }
      sim_at_rosync = ಅಸತ್ಯ;

      ಯಾವತ್ (ctim->del_thr) {
	    ಸಂರಚನಾ event_s*cur = ctim->del_thr->next;
	    ಯದಿ (cur->next == cur) {
		  ctim->del_thr = 0;
	    } ಅನ್ಯಥಾ {
		  ctim->del_thr->next = cur->next;
	    }

	    cur->run_run();
	    delete cur;
      }

      ಯದಿ (ctim->active || ctim->inactive || ctim->nbassign || ctim->rwsync) {
	    cerr << "SCHEDULER ERROR: read-only sync events "
		 << "created RW events!" << endl;
      }
}

ಶೂನ್ಯ schedule_simulate(ಶೂನ್ಯ)
{
      ತಾರ್ಕಿಕ run_finals;
      sim_started = ಅಸತ್ಯ;

      schedule_time = 0;

      ಯದಿ (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute EndOfCompile callbacks\n");
      }

      // Execute end of compile callbacks
      vpiEndOfCompile();

      ಯದಿ (verbose_flag) {
	    vpi_mcd_printf(1, " ...propagate initialization events\n");
      }

	// Execute initialization events.
      ಯಾವತ್ (schedule_init_list) {
	    ಸಂರಚನಾ event_s*cur = schedule_init_list->next;
	    ಯದಿ (cur->next == cur) {
		  schedule_init_list = 0;
	    } ಅನ್ಯಥಾ {
		  schedule_init_list->next = cur->next;
	    }
	    cur->run_run();
	    delete cur;
      }

      sim_started = ಸತ್ಯ;

      ಯದಿ (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute StartOfSim callbacks\n");
      }

      // Execute start of simulation callbacks
      vpiStartOfSim();

      signals_capture();

      ಯದಿ (verbose_flag) {
	    vpi_mcd_printf(1, " ...run scheduler\n");
      }

      // If there were no compiletf, etc. errors then we are going to
      // process events and when done run the final blocks.
      run_finals = schedule_runnable;

      ಯದಿ (schedule_runnable) ಯಾವತ್ (sched_list) {

	    ಯದಿ (schedule_stopped_flag) {
		  schedule_stopped_flag = ಅಸತ್ಯ;
		  stop_handler(0);
		  // You can finish from the debugger without a time change.
		  ಯದಿ (!schedule_runnable) ಭಙ್ಗ;
		  ಅನುವರ್ತ;
	    }

	      /* ctim is the current time step. */
	    ಸಂರಚನಾ event_time_s* ctim = sched_list;

	      /* If the time is advancing, then first run the
		 postponed sync events. Run them all. */
	    ಯದಿ (ctim->delay > 0) {

		  ಯದಿ (!schedule_runnable) ಭಙ್ಗ;
		  schedule_time += ctim->delay;
		    /* When the design is being traced (we are emitting
		     * file/line information) also print any time changes. */
		  ಯದಿ (show_file_ಸಾಲು) {
			cerr << "Advancing to simulation time: "
			     << schedule_time << endl;
		  }
		  ctim->delay = 0;

		  vpiNextSimTime();
		    // Process the cbAtStartOfSimTime callbacks.
		  ಯಾವತ್ (ctim->start) {
			ಸಂರಚನಾ event_s*cur = ctim->start->next;
			ಯದಿ (cur->next == cur) {
			      ctim->start = 0;
			} ಅನ್ಯಥಾ {
			      ctim->start->next = cur->next;
			}
			cur->run_run();
			delete (cur);
		  }
	    }


	      /* If there are no more active events, advance the event
		 queues. If there are not events at all, then release
		 the event_time object. */
	    ಯದಿ (ctim->active == 0) {
		  ctim->active = ctim->inactive;
		  ctim->inactive = 0;

		  ಯದಿ (ctim->active == 0) {
			ctim->active = ctim->nbassign;
			ctim->nbassign = 0;

			ಯದಿ (ctim->active == 0) {
			      ctim->active = ctim->rwsync;
			      ctim->rwsync = 0;

				/* If out of rw events, then run the rosync
				   events and delete this time step. This also
				   deletes threads as needed. */
			      ಯದಿ (ctim->active == 0) {
				    run_rosync(ctim);
				    sched_list = ctim->next;
				    delete ctim;
				    ಅನುವರ್ತ;
			      }
			}
		  }
	    }

	      /* Pull the first item off the list. If this is the last
		 cell in the list, then clear the list. Execute that
		 event type, and delete it. */
	    ಸಂರಚನಾ event_s*cur = ctim->active->next;
	    ಯದಿ (cur->next == cur) {
		  ctim->active = 0;
	    } ಅನ್ಯಥಾ {
		  ctim->active->next = cur->next;
	    }

	    ಯದಿ (schedule_single_step_flag) {
		  cur->single_step_display();
		  schedule_stopped_flag = ಸತ್ಯ;
		  schedule_single_step_flag = ಅಸತ್ಯ;
	    }

	    cur->run_run();

	    delete (cur);
      }

	// Execute final events.
      schedule_runnable = run_finals;
      ಯಾವತ್ (schedule_runnable && schedule_final_list) {
	    ಸಂರಚನಾ event_s*cur = schedule_final_list->next;
	    ಯದಿ (cur->next == cur) {
		  schedule_final_list = 0;
	    } ಅನ್ಯಥಾ {
		  schedule_final_list->next = cur->next;
	    }
	    cur->run_run();
	    delete cur;
      }

      signals_revert();

      ಯದಿ (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute Postsim callbacks\n");
      }

      // Execute post-simulation callbacks
      vpiPostsim();
#ifdef CHECK_WITH_VALGRIND
      schedule_delete();
#endif
}

#ifdef CHECK_WITH_VALGRIND
ಶೂನ್ಯ schedule_delete(ಶೂನ್ಯ)
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
