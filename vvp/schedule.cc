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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
# include  "ivl_alloc.h"
#ಆದರೆ_ಅಂತ್ಯ

ಬಳಸಿ ನಾಮಾಕಾಶ std;

ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_ನಿಯೋಜಿಸು_events = 0;
ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_gen_events = 0;
ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_thread_events = 0;
  // Count the time events (A time cell created)
ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_ಸಮಯ_events = 0;



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
ರಚನೆ event_s {
      ರಚನೆ event_s*ಮುಂದಿನ_2;
      ವಾಸ್ತವಿಕ ~event_s() { }
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಸತತ_ಸತತ(ಶೂನ್ಯ) =0;

	// Write something about the event to stderr
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ single_ಹೆಜ್ಜೆ_display(ಶೂನ್ಯ);

	// Fallback new/delete
      ಸ್ಥಿರ ಶೂನ್ಯ*ಕ್ರಿಯಾಕಾರಕ ಹೊಸ (size_t size) { ಹಿಂತಿರುಗಿ ::ಹೊಸ ಅಕ್ಷರ[size]; }
      ಸ್ಥಿರ ಶೂನ್ಯ ಕ್ರಿಯಾಕಾರಕ ಅಳಿಸು(ಶೂನ್ಯ*ptr)  { ::ಅಳಿಸು[]( static_cast<ಅಕ್ಷರ*>(ptr) ); }
};

ಶೂನ್ಯ event_s::single_ಹೆಜ್ಜೆ_display(ಶೂನ್ಯ)
{
      std::cerr << "event_s: Step into event " << typeid(*ಇದು).name() << std::ಸಾಲು_ಅಂತ್ಯ;
}

ರಚನೆ event_ಸಮಯ_s {
      event_ಸಮಯ_s() {
	    ಎಣಿಕೆ_ಸಮಯ_events += 1;
	    delay = 0;
	    ಆರಂಭ = 0;
	    ಸಕ್ರಿಯ = 0;
	    inactive = 0;
	    nbassign = 0;
	    rwsync = 0;
	    rosync = 0;
	    del_thr = 0;
	    ಮುಂದಿನ_2 = ಖಾಲಿ;
      }
      vvp_time64_t delay;

      ರಚನೆ event_s*ಆರಂಭ;
      ರಚನೆ event_s*ಸಕ್ರಿಯ;
      ರಚನೆ event_s*inactive;
      ರಚನೆ event_s*nbassign;
      ರಚನೆ event_s*rwsync;
      ರಚನೆ event_s*rosync;
      ರಚನೆ event_s*del_thr;

      ರಚನೆ event_ಸಮಯ_s*ಮುಂದಿನ_2;

      ಸ್ಥಿರ ಶೂನ್ಯ* ಕ್ರಿಯಾಕಾರಕ ಹೊಸ (size_t);
      ಸ್ಥಿರ ಶೂನ್ಯ ಕ್ರಿಯಾಕಾರಕ ಅಳಿಸು(ಶೂನ್ಯ*obj, size_t s);
};

vvp_gen_event_s::~vvp_gen_event_s()
{
}

ಶೂನ್ಯ vvp_gen_event_s::single_ಹೆಜ್ಜೆ_display(ಶೂನ್ಯ)
{
      cerr << "vvp_gen_event_s: Step into event " << typeid(*ಇದು).name() << ಸಾಲು_ಅಂತ್ಯ;
}

/*
 * Derived event types
 */
ರಚನೆ vthread_event_s : ಸಾರ್ವಜನಿಕ event_s {
      vthread_t thr;
      ಶೂನ್ಯ ಸತತ_ಸತತ(ಶೂನ್ಯ) override;
      ಶೂನ್ಯ single_ಹೆಜ್ಜೆ_display(ಶೂನ್ಯ) override;

      ಸ್ಥಿರ ಶೂನ್ಯ* ಕ್ರಿಯಾಕಾರಕ ಹೊಸ(size_t);
      ಸ್ಥಿರ ಶೂನ್ಯ ಕ್ರಿಯಾಕಾರಕ ಅಳಿಸು(ಶೂನ್ಯ*);
};

ಶೂನ್ಯ vthread_event_s::ಸತತ_ಸತತ(ಶೂನ್ಯ)
{
      ಎಣಿಕೆ_thread_events += 1;
      vthread_ಸತತ(thr);
}

ಶೂನ್ಯ vthread_event_s::single_ಹೆಜ್ಜೆ_display(ಶೂನ್ಯ)
{
      __vpiScope*scope = vthread_scope(thr);
      cerr << "vthread_event: Resume thread"
	   << " scope=" << scope->vpi_get_str(vpi_ತುಂಬಿದ_Name)
	   << ಸಾಲು_ಅಂತ್ಯ;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ size_t VTHR_CHUNK_ಎಣಿಕೆ = 8192 / ಗಾತ್ರ(ರಚನೆ vthread_event_s);
ಸ್ಥಿರ slab_t<ಗಾತ್ರ(vthread_event_s),VTHR_CHUNK_ಎಣಿಕೆ> vthread_event_ರಾಶಿ;

ಸಾಲೊಳಗೆ ಶೂನ್ಯ* vthread_event_s::ಕ್ರಿಯಾಕಾರಕ ಹೊಸ(size_t size)
{
      ಪ್ರತಿಜ್ಞಾ(size == ಗಾತ್ರ(vthread_event_s));
      ಹಿಂತಿರುಗಿ vthread_event_ರಾಶಿ.alloc_slab();
}

ಶೂನ್ಯ vthread_event_s::ಕ್ರಿಯಾಕಾರಕ ಅಳಿಸು(ಶೂನ್ಯ*dptr)
{
      vthread_event_ರಾಶಿ.free_slab(dptr);
}

ರಚನೆ del_thr_event_s : ಸಾರ್ವಜನಿಕ event_s {
      vthread_t thr;
      ಶೂನ್ಯ ಸತತ_ಸತತ(ಶೂನ್ಯ) override;
      ಶೂನ್ಯ single_ಹೆಜ್ಜೆ_display(ಶೂನ್ಯ) override;
};

ಶೂನ್ಯ del_thr_event_s::ಸತತ_ಸತತ(ಶೂನ್ಯ)
{
      vthread_delete(thr);
}

ಶೂನ್ಯ del_thr_event_s::single_ಹೆಜ್ಜೆ_display(ಶೂನ್ಯ)
{
      __vpiScope*scope = vthread_scope(thr);
      cerr << "del_thr_event: Reap completed thread"
	   << " scope=" << scope->vpi_get_str(vpi_ತುಂಬಿದ_Name) << ಸಾಲು_ಅಂತ್ಯ;
}

ರಚನೆ ನಿಯೋಜಿಸು_vector4_event_s  : ಸಾರ್ವಜನಿಕ event_s {
	/* The default constructor. */
      ಸ್ಪಷ್ಟ ನಿಯೋಜಿಸು_vector4_event_s(ಸ್ಥಿರಾಂಕ vvp_vector4_t&that) : val(that) {
	    ತಳ = 0;
	    vwid = 0;
	    ಮುಂದಿನ_2 = ಖಾಲಿ;
      }

	/* Where to do the assign. */
      vvp_net_ptr_t ptr;
	/* Value to assign. */
      vvp_vector4_t val;
	/* Offset of the part into the destination. */
      ಚಿಹ್ನೆರಹಿತ ತಳ;
	/* Width of the destination vector. */
      ಚಿಹ್ನೆರಹಿತ vwid;
      ಶೂನ್ಯ ಸತತ_ಸತತ(ಶೂನ್ಯ) override;
      ಶೂನ್ಯ single_ಹೆಜ್ಜೆ_display(ಶೂನ್ಯ) override;

      ಸ್ಥಿರ ಶೂನ್ಯ* ಕ್ರಿಯಾಕಾರಕ ಹೊಸ(size_t);
      ಸ್ಥಿರ ಶೂನ್ಯ ಕ್ರಿಯಾಕಾರಕ ಅಳಿಸು(ಶೂನ್ಯ*);
};

ಶೂನ್ಯ ನಿಯೋಜಿಸು_vector4_event_s::ಸತತ_ಸತತ(ಶೂನ್ಯ)
{
      ಎಣಿಕೆ_ನಿಯೋಜಿಸು_events += 1;
      ಆದರೆ (vwid > 0)
	    vvp_send_vec4_pv(ptr, val, ತಳ, vwid, 0);
      ಇಲ್ಲದಿದ್ದರೆ
	    vvp_send_vec4(ptr, val, 0);
}

ಶೂನ್ಯ ನಿಯೋಜಿಸು_vector4_event_s::single_ಹೆಜ್ಜೆ_display(ಶೂನ್ಯ)
{
      cerr << "assign_vector4_event: Propagate val=" << val
	   << ", vwid=" << vwid << ", base=" << ತಳ << ಸಾಲು_ಅಂತ್ಯ;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ size_t ASSIGN4_CHUNK_ಎಣಿಕೆ = 524288 / ಗಾತ್ರ(ರಚನೆ ನಿಯೋಜಿಸು_vector4_event_s);
ಸ್ಥಿರ slab_t<ಗಾತ್ರ(ನಿಯೋಜಿಸು_vector4_event_s),ASSIGN4_CHUNK_ಎಣಿಕೆ> assign4_ರಾಶಿ;

ಸಾಲೊಳಗೆ ಶೂನ್ಯ* ನಿಯೋಜಿಸು_vector4_event_s::ಕ್ರಿಯಾಕಾರಕ ಹೊಸ(size_t size)
{
      ಪ್ರತಿಜ್ಞಾ(size == ಗಾತ್ರ(ನಿಯೋಜಿಸು_vector4_event_s));
      ಹಿಂತಿರುಗಿ assign4_ರಾಶಿ.alloc_slab();
}

ಶೂನ್ಯ ನಿಯೋಜಿಸು_vector4_event_s::ಕ್ರಿಯಾಕಾರಕ ಅಳಿಸು(ಶೂನ್ಯ*dptr)
{
      assign4_ರಾಶಿ.free_slab(dptr);
}

ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_assign4_pool(ಶೂನ್ಯ) { ಹಿಂತಿರುಗಿ assign4_ರಾಶಿ.pool; }

ರಚನೆ ನಿಯೋಜಿಸು_vector8_event_s  : ಸಾರ್ವಜನಿಕ event_s {
      vvp_net_ptr_t ptr;
      vvp_vector8_t val;
      ಶೂನ್ಯ ಸತತ_ಸತತ(ಶೂನ್ಯ) override;
      ಶೂನ್ಯ single_ಹೆಜ್ಜೆ_display(ಶೂನ್ಯ) override;

      ಸ್ಥಿರ ಶೂನ್ಯ* ಕ್ರಿಯಾಕಾರಕ ಹೊಸ(size_t);
      ಸ್ಥಿರ ಶೂನ್ಯ ಕ್ರಿಯಾಕಾರಕ ಅಳಿಸು(ಶೂನ್ಯ*);
};

ಶೂನ್ಯ ನಿಯೋಜಿಸು_vector8_event_s::ಸತತ_ಸತತ(ಶೂನ್ಯ)
{
      ಎಣಿಕೆ_ನಿಯೋಜಿಸು_events += 1;
      vvp_send_vec8(ptr, val);
}

ಶೂನ್ಯ ನಿಯೋಜಿಸು_vector8_event_s::single_ಹೆಜ್ಜೆ_display(ಶೂನ್ಯ)
{
      cerr << "assign_vector8_event: Propagate val=" << val << ಸಾಲು_ಅಂತ್ಯ;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ size_t ASSIGN8_CHUNK_ಎಣಿಕೆ = 8192 / ಗಾತ್ರ(ರಚನೆ ನಿಯೋಜಿಸು_vector8_event_s);
ಸ್ಥಿರ slab_t<ಗಾತ್ರ(ನಿಯೋಜಿಸು_vector8_event_s),ASSIGN8_CHUNK_ಎಣಿಕೆ> assign8_ರಾಶಿ;

ಸಾಲೊಳಗೆ ಶೂನ್ಯ* ನಿಯೋಜಿಸು_vector8_event_s::ಕ್ರಿಯಾಕಾರಕ ಹೊಸ(size_t size)
{
      ಪ್ರತಿಜ್ಞಾ(size == ಗಾತ್ರ(ನಿಯೋಜಿಸು_vector8_event_s));
      ಹಿಂತಿರುಗಿ assign8_ರಾಶಿ.alloc_slab();
}

ಶೂನ್ಯ ನಿಯೋಜಿಸು_vector8_event_s::ಕ್ರಿಯಾಕಾರಕ ಅಳಿಸು(ಶೂನ್ಯ*dptr)
{
      assign8_ರಾಶಿ.free_slab(dptr);
}

ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_assign8_pool() { ಹಿಂತಿರುಗಿ assign8_ರಾಶಿ.pool; }

ರಚನೆ ನಿಯೋಜಿಸು_real_event_s  : ಸಾರ್ವಜನಿಕ event_s {
      vvp_net_ptr_t ptr;
      ದ್ವಿಗುಣ val;
      ಶೂನ್ಯ ಸತತ_ಸತತ(ಶೂನ್ಯ) override;
      ಶೂನ್ಯ single_ಹೆಜ್ಜೆ_display(ಶೂನ್ಯ) override;

      ಸ್ಥಿರ ಶೂನ್ಯ* ಕ್ರಿಯಾಕಾರಕ ಹೊಸ(size_t);
      ಸ್ಥಿರ ಶೂನ್ಯ ಕ್ರಿಯಾಕಾರಕ ಅಳಿಸು(ಶೂನ್ಯ*);
};

ಶೂನ್ಯ ನಿಯೋಜಿಸು_real_event_s::ಸತತ_ಸತತ(ಶೂನ್ಯ)
{
      ಎಣಿಕೆ_ನಿಯೋಜಿಸು_events += 1;
      vvp_send_real(ptr, val, 0);
}

ಶೂನ್ಯ ನಿಯೋಜಿಸು_real_event_s::single_ಹೆಜ್ಜೆ_display(ಶೂನ್ಯ)
{
      cerr << "assign_real_event: Propagate val=" << val << ಸಾಲು_ಅಂತ್ಯ;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ size_t ASSIGNR_CHUNK_ಎಣಿಕೆ = 8192 / ಗಾತ್ರ(ರಚನೆ ನಿಯೋಜಿಸು_real_event_s);
ಸ್ಥಿರ slab_t<ಗಾತ್ರ(ನಿಯೋಜಿಸು_real_event_s),ASSIGNR_CHUNK_ಎಣಿಕೆ> assignr_ರಾಶಿ;

ಸಾಲೊಳಗೆ ಶೂನ್ಯ* ನಿಯೋಜಿಸು_real_event_s::ಕ್ರಿಯಾಕಾರಕ ಹೊಸ (size_t size)
{
      ಪ್ರತಿಜ್ಞಾ(size == ಗಾತ್ರ(ನಿಯೋಜಿಸು_real_event_s));
      ಹಿಂತಿರುಗಿ assignr_ರಾಶಿ.alloc_slab();
}

ಶೂನ್ಯ ನಿಯೋಜಿಸು_real_event_s::ಕ್ರಿಯಾಕಾರಕ ಅಳಿಸು(ಶೂನ್ಯ*dptr)
{
      assignr_ರಾಶಿ.free_slab(dptr);
}

ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_ನಿಯೋಜಿಸು_real_pool(ಶೂನ್ಯ) { ಹಿಂತಿರುಗಿ assignr_ರಾಶಿ.pool; }

ರಚನೆ ನಿಯೋಜಿಸು_array_ಶಬ್ದ_s  : ಸಾರ್ವಜನಿಕ event_s {
      vvp_array_t mem;
      ಚಿಹ್ನೆರಹಿತ adr;
      vvp_vector4_t val;
      ಚಿಹ್ನೆರಹಿತ off;
      ಶೂನ್ಯ ಸತತ_ಸತತ(ಶೂನ್ಯ) override;

      ಸ್ಥಿರ ಶೂನ್ಯ* ಕ್ರಿಯಾಕಾರಕ ಹೊಸ(size_t);
      ಸ್ಥಿರ ಶೂನ್ಯ ಕ್ರಿಯಾಕಾರಕ ಅಳಿಸು(ಶೂನ್ಯ*);
};

ಶೂನ್ಯ ನಿಯೋಜಿಸು_array_ಶಬ್ದ_s::ಸತತ_ಸತತ(ಶೂನ್ಯ)
{
      ಎಣಿಕೆ_ನಿಯೋಜಿಸು_events += 1;
      mem->ಗಣ_ಶಬ್ದ(adr, off, val);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ size_t ARRAY_W_CHUNK_ಎಣಿಕೆ = 8192 / ಗಾತ್ರ(ರಚನೆ ನಿಯೋಜಿಸು_array_ಶಬ್ದ_s);
ಸ್ಥಿರ slab_t<ಗಾತ್ರ(ನಿಯೋಜಿಸು_array_ಶಬ್ದ_s),ARRAY_W_CHUNK_ಎಣಿಕೆ> array_w_ರಾಶಿ;

ಸಾಲೊಳಗೆ ಶೂನ್ಯ* ನಿಯೋಜಿಸು_array_ಶಬ್ದ_s::ಕ್ರಿಯಾಕಾರಕ ಹೊಸ (size_t size)
{
      ಪ್ರತಿಜ್ಞಾ(size == ಗಾತ್ರ(ನಿಯೋಜಿಸು_array_ಶಬ್ದ_s));
      ಹಿಂತಿರುಗಿ array_w_ರಾಶಿ.alloc_slab();
}

ಶೂನ್ಯ ನಿಯೋಜಿಸು_array_ಶಬ್ದ_s::ಕ್ರಿಯಾಕಾರಕ ಅಳಿಸು(ಶೂನ್ಯ*ptr)
{
      array_w_ರಾಶಿ.free_slab(ptr);
}

ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_ನಿಯೋಜಿಸು_aword_pool(ಶೂನ್ಯ) { ಹಿಂತಿರುಗಿ array_w_ರಾಶಿ.pool; }

ರಚನೆ force_vector4_event_s  : ಸಾರ್ವಜನಿಕ event_s {
	/* The default constructor. */
      ಸ್ಪಷ್ಟ force_vector4_event_s(ಸ್ಥಿರಾಂಕ vvp_vector4_t&that): val(that) {
	    net = ಖಾಲಿ;
	    ತಳ = 0;
	    vwid = 0;
	    ಮುಂದಿನ_2 = ಖಾಲಿ;
      }
	/* Where to do the force. */
      vvp_net_t*net;
	/* Value to force. */
      vvp_vector4_t val;
	/* Offset of the part into the destination. */
      ಚಿಹ್ನೆರಹಿತ ತಳ;
	/* Width of the destination vector. */
      ಚಿಹ್ನೆರಹಿತ vwid;

      ಶೂನ್ಯ ಸತತ_ಸತತ(ಶೂನ್ಯ) override;
      ಶೂನ್ಯ single_ಹೆಜ್ಜೆ_display(ಶೂನ್ಯ) override;

      ಸ್ಥಿರ ಶೂನ್ಯ* ಕ್ರಿಯಾಕಾರಕ ಹೊಸ(size_t);
      ಸ್ಥಿರ ಶೂನ್ಯ ಕ್ರಿಯಾಕಾರಕ ಅಳಿಸು(ಶೂನ್ಯ*);
};

ಶೂನ್ಯ force_vector4_event_s::ಸತತ_ಸತತ(ಶೂನ್ಯ)
{
      ಎಣಿಕೆ_ನಿಯೋಜಿಸು_events += 1;

      ಚಿಹ್ನೆರಹಿತ wid = val.size();
      ಆದರೆ ((ತಳ + wid) > vwid)
	    wid = vwid - ತಳ;

	// Make a mask of which bits are to be forced, 0 for unforced
	// bits and 1 for forced bits.
      vvp_vector2_t ಮುಖವಾಡ (vvp_vector2_t::FILL0, vwid);
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < wid ; idx += 1)
	    ಮುಖವಾಡ.ಗಣ_ಬಿಟ್(ತಳ+idx, 1);

      vvp_vector4_t tmp (vwid, BIT4_Z);

	// vvp_net_t::force_vec4 propagates all the bits of the
	// forced vector value, regardless of the mask. This
	// ensures the unforced bits retain their current value.
      vvp_signal_ಮೌಲ್ಯ*sig = dynamic_cast<vvp_signal_ಮೌಲ್ಯ*>(net->fil);
      ಪ್ರತಿಜ್ಞಾ(sig);
      sig->vec4_ಮೌಲ್ಯ(tmp);

      tmp.ಗಣ_vec(ತಳ, val);
      net->force_vec4(tmp, ಮುಖವಾಡ);
}

ಶೂನ್ಯ force_vector4_event_s::single_ಹೆಜ್ಜೆ_display(ಶೂನ್ಯ)
{
      cerr << "force_vector4_event: Force val=" << val
	   << ", vwid=" << vwid << ", base=" << ತಳ << ಸಾಲು_ಅಂತ್ಯ;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ size_t FORCE4_CHUNK_ಎಣಿಕೆ = 8192 / ಗಾತ್ರ(ರಚನೆ force_vector4_event_s);
ಸ್ಥಿರ slab_t<ಗಾತ್ರ(force_vector4_event_s),FORCE4_CHUNK_ಎಣಿಕೆ> force4_ರಾಶಿ;

ಸಾಲೊಳಗೆ ಶೂನ್ಯ* force_vector4_event_s::ಕ್ರಿಯಾಕಾರಕ ಹೊಸ(size_t size)
{
      ಪ್ರತಿಜ್ಞಾ(size == ಗಾತ್ರ(force_vector4_event_s));
      ಹಿಂತಿರುಗಿ force4_ರಾಶಿ.alloc_slab();
}

ಶೂನ್ಯ force_vector4_event_s::ಕ್ರಿಯಾಕಾರಕ ಅಳಿಸು(ಶೂನ್ಯ*dptr)
{
      force4_ರಾಶಿ.free_slab(dptr);
}

ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_force4_pool(ಶೂನ್ಯ) { ಹಿಂತಿರುಗಿ force4_ರಾಶಿ.pool; }

/*
 * This class supports the propagation of vec4 outputs from a
 * vvp_net_t object.
 */
ರಚನೆ propagate_vector4_event_s : ಸಾರ್ವಜನಿಕ event_s {
	/* The default constructor. */
      ಸ್ಪಷ್ಟ propagate_vector4_event_s(ಸ್ಥಿರಾಂಕ vvp_vector4_t&that) : val(that) {
	    net = ಖಾಲಿ;
	    ಮುಂದಿನ_2 = ಖಾಲಿ;
      }
	/* A constructor that makes the val directly. */
      propagate_vector4_event_s(ಸ್ಥಿರಾಂಕ vvp_vector4_t&that, ಚಿಹ್ನೆರಹಿತ adr, ಚಿಹ್ನೆರಹಿತ wid)
      : val(that,adr,wid) {
	    net = ಖಾಲಿ;
	    ಮುಂದಿನ_2 = ಖಾಲಿ;
      }

	/* Propagate the output of this net. */
      vvp_net_t*net;
	/* value to propagate */
      vvp_vector4_t val;
	/* Action */
      ಶೂನ್ಯ ಸತತ_ಸತತ(ಶೂನ್ಯ) override;
      ಶೂನ್ಯ single_ಹೆಜ್ಜೆ_display(ಶೂನ್ಯ) override;
};

ಶೂನ್ಯ propagate_vector4_event_s::ಸತತ_ಸತತ(ಶೂನ್ಯ)
{
      net->send_vec4(val, 0);
}

ಶೂನ್ಯ propagate_vector4_event_s::single_ಹೆಜ್ಜೆ_display(ಶೂನ್ಯ)
{
      cerr << "propagate_vector4_event: Propagate val=" << val << ಸಾಲು_ಅಂತ್ಯ;
}

/*
 * This class supports the propagation of real outputs from a
 * vvp_net_t object.
 */
ರಚನೆ propagate_real_event_s : ಸಾರ್ವಜನಿಕ event_s {
	/* Propagate the output of this net. */
      vvp_net_t*net;
	/* value to propagate */
      ದ್ವಿಗುಣ val;
	/* Action */
      ಶೂನ್ಯ ಸತತ_ಸತತ(ಶೂನ್ಯ) override;
      ಶೂನ್ಯ single_ಹೆಜ್ಜೆ_display(ಶೂನ್ಯ) override;
};

ಶೂನ್ಯ propagate_real_event_s::ಸತತ_ಸತತ(ಶೂನ್ಯ)
{
      net->send_real(val, 0);
}

ಶೂನ್ಯ propagate_real_event_s::single_ಹೆಜ್ಜೆ_display(ಶೂನ್ಯ)
{
      cerr << "propagate_real_event: Propagate val=" << val << ಸಾಲು_ಅಂತ್ಯ;
}

ರಚನೆ ನಿಯೋಜಿಸು_array_r_ಶಬ್ದ_s  : ಸಾರ್ವಜನಿಕ event_s {
      vvp_array_t mem;
      ಚಿಹ್ನೆರಹಿತ adr;
      ದ್ವಿಗುಣ val;
      ಶೂನ್ಯ ಸತತ_ಸತತ(ಶೂನ್ಯ) override;

      ಸ್ಥಿರ ಶೂನ್ಯ* ಕ್ರಿಯಾಕಾರಕ ಹೊಸ(size_t);
      ಸ್ಥಿರ ಶೂನ್ಯ ಕ್ರಿಯಾಕಾರಕ ಅಳಿಸು(ಶೂನ್ಯ*);
};

ಶೂನ್ಯ ನಿಯೋಜಿಸು_array_r_ಶಬ್ದ_s::ಸತತ_ಸತತ(ಶೂನ್ಯ)
{
      ಎಣಿಕೆ_ನಿಯೋಜಿಸು_events += 1;
      mem->ಗಣ_ಶಬ್ದ(adr, val);
}
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ size_t ARRAY_R_W_CHUNK_ಎಣಿಕೆ = 8192 / ಗಾತ್ರ(ರಚನೆ ನಿಯೋಜಿಸು_array_r_ಶಬ್ದ_s);
ಸ್ಥಿರ slab_t<ಗಾತ್ರ(ನಿಯೋಜಿಸು_array_r_ಶಬ್ದ_s),ARRAY_R_W_CHUNK_ಎಣಿಕೆ> array_r_w_ರಾಶಿ;

ಸಾಲೊಳಗೆ ಶೂನ್ಯ* ನಿಯೋಜಿಸು_array_r_ಶಬ್ದ_s::ಕ್ರಿಯಾಕಾರಕ ಹೊಸ(size_t size)
{
      ಪ್ರತಿಜ್ಞಾ(size == ಗಾತ್ರ(ನಿಯೋಜಿಸು_array_r_ಶಬ್ದ_s));
      ಹಿಂತಿರುಗಿ array_r_w_ರಾಶಿ.alloc_slab();
}

ಶೂನ್ಯ ನಿಯೋಜಿಸು_array_r_ಶಬ್ದ_s::ಕ್ರಿಯಾಕಾರಕ ಅಳಿಸು(ಶೂನ್ಯ*ptr)
{
      array_r_w_ರಾಶಿ.free_slab(ptr);
}

ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_ನಿಯೋಜಿಸು_arword_pool(ಶೂನ್ಯ) { ಹಿಂತಿರುಗಿ array_r_w_ರಾಶಿ.pool; }

ರಚನೆ generic_event_s : ಸಾರ್ವಜನಿಕ event_s {
      vvp_gen_event_t obj;
      ತಾರ್ಕಿಕ delete_obj_when_ಮುಗಿದ;
      ಶೂನ್ಯ ಸತತ_ಸತತ(ಶೂನ್ಯ) override;
      ಶೂನ್ಯ single_ಹೆಜ್ಜೆ_display(ಶೂನ್ಯ) override;

      ಸ್ಥಿರ ಶೂನ್ಯ* ಕ್ರಿಯಾಕಾರಕ ಹೊಸ(size_t);
      ಸ್ಥಿರ ಶೂನ್ಯ ಕ್ರಿಯಾಕಾರಕ ಅಳಿಸು(ಶೂನ್ಯ*);
};

ಶೂನ್ಯ generic_event_s::ಸತತ_ಸತತ(ಶೂನ್ಯ)
{
      ಎಣಿಕೆ_gen_events += 1;
      ಆದರೆ (obj) {
	    obj->ಸತತ_ಸತತ();
	    ಆದರೆ (delete_obj_when_ಮುಗಿದ)
		  ಅಳಿಸು obj;
      }
}

ಶೂನ್ಯ generic_event_s::single_ಹೆಜ್ಜೆ_display(ಶೂನ್ಯ)
{
      obj->single_ಹೆಜ್ಜೆ_display();
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ size_t GENERIC_CHUNK_ಎಣಿಕೆ = 131072 / ಗಾತ್ರ(ರಚನೆ generic_event_s);
ಸ್ಥಿರ slab_t<ಗಾತ್ರ(generic_event_s),GENERIC_CHUNK_ಎಣಿಕೆ> generic_event_ರಾಶಿ;

ಸಾಲೊಳಗೆ ಶೂನ್ಯ* generic_event_s::ಕ್ರಿಯಾಕಾರಕ ಹೊಸ(size_t size)
{
      ಪ್ರತಿಜ್ಞಾ(size == ಗಾತ್ರ(generic_event_s));
      ಹಿಂತಿರುಗಿ generic_event_ರಾಶಿ.alloc_slab();
}

ಶೂನ್ಯ generic_event_s::ಕ್ರಿಯಾಕಾರಕ ಅಳಿಸು(ಶೂನ್ಯ*ptr)
{
      generic_event_ರಾಶಿ.free_slab(ptr);
}

ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_gen_pool(ಶೂನ್ಯ) { ಹಿಂತಿರುಗಿ generic_event_ರಾಶಿ.pool; }

/*
** These event_time_s will be required a lot, at high frequency.
** Once allocated, we never free them, but stash them away for next time.
*/


ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ size_t ಸಮಯ_CHUNK_ಎಣಿಕೆ = 8192 / ಗಾತ್ರ(ರಚನೆ event_ಸಮಯ_s);
ಸ್ಥಿರ slab_t<ಗಾತ್ರ(event_ಸಮಯ_s),ಸಮಯ_CHUNK_ಎಣಿಕೆ> event_ಸಮಯ_ರಾಶಿ;

ಸಾಲೊಳಗೆ ಶೂನ್ಯ* event_ಸಮಯ_s::ಕ್ರಿಯಾಕಾರಕ ಹೊಸ (size_t size)
{
      ಪ್ರತಿಜ್ಞಾ(size == ಗಾತ್ರ(ರಚನೆ event_ಸಮಯ_s));
      ಶೂನ್ಯ*ptr = event_ಸಮಯ_ರಾಶಿ.alloc_slab();
      ಹಿಂತಿರುಗಿ ptr;
}

ಸಾಲೊಳಗೆ ಶೂನ್ಯ event_ಸಮಯ_s::ಕ್ರಿಯಾಕಾರಕ ಅಳಿಸು(ಶೂನ್ಯ*ptr, size_t)
{
      event_ಸಮಯ_ರಾಶಿ.free_slab(ptr);
}

ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_ಸಮಯ_pool(ಶೂನ್ಯ) { ಹಿಂತಿರುಗಿ event_ಸಮಯ_ರಾಶಿ.pool; }

/*
 * This is the head of the list of pending events. This includes all
 * the events that have not been executed yet, and reaches into the
 * future.
 */
ಸ್ಥಿರ ರಚನೆ event_ಸಮಯ_s* sched_list = 0;

/*
 * This is a list of initialization events. The setup puts
 * initializations in this list so that they happen before the
 * simulation as a whole starts. This prevents time-0 triggers of
 * certain events.
 */
ಸ್ಥಿರ ರಚನೆ event_s* ವೇಳಾಪಟ್ಟಿ_init_list = 0;

/*
 * This is the head of the list of final events.
 */
ಸ್ಥಿರ ರಚನೆ event_s* ವೇಳಾಪಟ್ಟಿ_final_list = 0;

/*
 * This flag is true until a VPI task or function finishes the
 * simulation.
 */
ಸ್ಥಿರ ತಾರ್ಕಿಕ ವೇಳಾಪಟ್ಟಿ_runnable = ನಿಜ;
ಸ್ಥಿರ ತಾರ್ಕಿಕ ವೇಳಾಪಟ್ಟಿ_stopped_flag  = ಸುಳ್ಳು;
ಸ್ಥಿರ ತಾರ್ಕಿಕ ವೇಳಾಪಟ್ಟಿ_single_ಹೆಜ್ಜೆ_flag = ಸುಳ್ಳು;
ಸ್ಥಿರ ತಾರ್ಕಿಕ no_ಸಂಕೇತಗಳು_flag = ಸುಳ್ಳು;

ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_ಮುಕ್ತಾಯ(ಪೂರ್ಣಾಂಕ)
{
      ವೇಳಾಪಟ್ಟಿ_runnable = ಸುಳ್ಳು;
}

ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_stop(ಪೂರ್ಣಾಂಕ)
{
      ವೇಳಾಪಟ್ಟಿ_stopped_flag = ನಿಜ;
}

ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_single_ಹೆಜ್ಜೆ(ಪೂರ್ಣಾಂಕ)
{
      ವೇಳಾಪಟ್ಟಿ_single_ಹೆಜ್ಜೆ_flag = ನಿಜ;
}

ತಾರ್ಕಿಕ ವೇಳಾಪಟ್ಟಿ_ಪೂರ್ಣಗೊಂಡ(ಶೂನ್ಯ)
{
      ಹಿಂತಿರುಗಿ !ವೇಳಾಪಟ್ಟಿ_runnable;
}

ತಾರ್ಕಿಕ ವೇಳಾಪಟ್ಟಿ_stopped(ಶೂನ್ಯ)
{
      ಹಿಂತಿರುಗಿ ವೇಳಾಪಟ್ಟಿ_stopped_flag;
}

ಬಾಹ್ಯ "C" ಶೂನ್ಯ vvp_no_ಸಂಕೇತಗಳು(ಶೂನ್ಯ)
{
      no_ಸಂಕೇತಗಳು_flag = ನಿಜ;
}

/*
 * These are the signal handling infrastructure. The SIGINT signal
 * leads to an implicit $stop. The SIGHUP and SIGTERM signals lead
 * to an implicit $finish.
 */
ಬಾಹ್ಯ ತಾರ್ಕಿಕ stop_is_ಮುಕ್ತಾಯ;

ಬಾಹ್ಯ "C" ಶೂನ್ಯ ಸಂಕೇತಗಳು_handler(ಪೂರ್ಣಾಂಕ signum)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __MINGW32__
	// Windows implements the original UNIX semantics for signal,
	// so we have to re-establish the signal handler each time a
	// signal is caught.
      ಸಂಕೇತ(signum, &ಸಂಕೇತಗಳು_handler);
#ಆದರೆ_ಅಂತ್ಯ
      ಆದರೆ (signum != SIGINT)
	    stop_is_ಮುಕ್ತಾಯ = ನಿಜ;
      ವೇಳಾಪಟ್ಟಿ_stopped_flag = ನಿಜ;
}

ಸ್ಥಿರ ಶೂನ್ಯ ಸಂಕೇತಗಳು_capture(ಶೂನ್ಯ)
{
      ಆದರೆ (no_ಸಂಕೇತಗಳು_flag)
	   ಹಿಂತಿರುಗಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __MINGW32__
      ಸಂಕೇತ(SIGHUP,  &ಸಂಕೇತಗಳು_handler);
#ಆದರೆ_ಅಂತ್ಯ
      ಸಂಕೇತ(SIGINT,  &ಸಂಕೇತಗಳು_handler);
      ಸಂಕೇತ(SIGTERM, &ಸಂಕೇತಗಳು_handler);
}

ಸ್ಥಿರ ಶೂನ್ಯ ಸಂಕೇತಗಳು_revert(ಶೂನ್ಯ)
{
      ಆದರೆ (no_ಸಂಕೇತಗಳು_flag)
	   ಹಿಂತಿರುಗಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __MINGW32__
      ಸಂಕೇತ(SIGHUP,  SIG_DFL);
#ಆದರೆ_ಅಂತ್ಯ
      ಸಂಕೇತ(SIGINT,  SIG_DFL);
      ಸಂಕೇತ(SIGTERM, SIG_DFL);
}

/*
 * This function puts an event on the end of the pre-simulation event queue.
 */
ಸ್ಥಿರ ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_init_event(ರಚನೆ event_s*cur)
{
      ಆದರೆ (ವೇಳಾಪಟ್ಟಿ_init_list == 0) {
            cur->ಮುಂದಿನ_2 = cur;
      } ಇಲ್ಲದಿದ್ದರೆ {
            cur->ಮುಂದಿನ_2 = ವೇಳಾಪಟ್ಟಿ_init_list->ಮುಂದಿನ_2;
            ವೇಳಾಪಟ್ಟಿ_init_list->ಮುಂದಿನ_2 = cur;
      }
      ವೇಳಾಪಟ್ಟಿ_init_list = cur;
}

/*
 * This function puts an event on the end of the post-simulation event queue.
 */
ಸ್ಥಿರ ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_final_event(ರಚನೆ event_s*cur)
{
      ಆದರೆ (ವೇಳಾಪಟ್ಟಿ_final_list == 0) {
            cur->ಮುಂದಿನ_2 = cur;
      } ಇಲ್ಲದಿದ್ದರೆ {
            cur->ಮುಂದಿನ_2 = ವೇಳಾಪಟ್ಟಿ_final_list->ಮುಂದಿನ_2;
            ವೇಳಾಪಟ್ಟಿ_final_list->ಮುಂದಿನ_2 = cur;
      }
      ವೇಳಾಪಟ್ಟಿ_final_list = cur;
}

/*
 * This function does all the hard work of putting an event into the
 * event queue. The event delay is taken from the event structure
 * itself, and the structure is placed in the right place in the
 * queue.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ event_ಸರತಿ_e { SEQ_ಆರಂಭ, SEQ_ಸಕ್ರಿಯ, SEQ_INACTIVE, SEQ_NBASSIGN,
			     SEQ_RWSYNC, SEQ_ROSYNC, DEL_THREAD } event_ಸರತಿ_t;

ಸ್ಥಿರ ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_event(ರಚನೆ event_s*cur, vvp_time64_t delay,
			    event_ಸರತಿ_t select_ಸರತಿ)
{
      cur->ಮುಂದಿನ_2 = cur;
      ರಚನೆ event_ಸಮಯ_s*ctim = sched_list;

      ಆದರೆ (sched_list == 0) {
	      /* Is the event_time list completely empty? Create the
		 first event_time object. */
	    ctim = ಹೊಸ ರಚನೆ event_ಸಮಯ_s;
	    ctim->delay = delay;
	    ctim->ಮುಂದಿನ_2  = 0;
	    sched_list = ctim;

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (sched_list->delay > delay) {

	      /* Am I looking for an event before the first event_time?
		 If so, create a new event_time to go in front. */
	    ರಚನೆ event_ಸಮಯ_s*tmp = ಹೊಸ ರಚನೆ event_ಸಮಯ_s;
	    tmp->delay = delay;
	    tmp->ಮುಂದಿನ_2 = ctim;
	    ctim->delay -= delay;
	    ctim = tmp;
	    sched_list = ctim;

      } ಇಲ್ಲದಿದ್ದರೆ {
	    ರಚನೆ event_ಸಮಯ_s*prev = 0;

	    ಆಗ (ctim->ಮುಂದಿನ_2 && (ctim->delay < delay)) {
		  delay -= ctim->delay;
		  prev = ctim;
		  ctim = ctim->ಮುಂದಿನ_2;
	    }

	    ಆದರೆ (ctim->delay > delay) {
		  ರಚನೆ event_ಸಮಯ_s*tmp = ಹೊಸ ರಚನೆ event_ಸಮಯ_s;
		  tmp->delay = delay;
		  tmp->ಮುಂದಿನ_2  = prev->ಮುಂದಿನ_2;
		  prev->ಮುಂದಿನ_2 = tmp;

		  tmp->ಮುಂದಿನ_2->delay -= delay;
		  ctim = tmp;

	    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ctim->delay == delay) {

	    } ಇಲ್ಲದಿದ್ದರೆ {
		  ಪ್ರತಿಜ್ಞಾ(ctim->ಮುಂದಿನ_2 == 0);
		  ರಚನೆ event_ಸಮಯ_s*tmp = ಹೊಸ ರಚನೆ event_ಸಮಯ_s;
		  tmp->delay = delay - ctim->delay;
		  tmp->ಮುಂದಿನ_2 = 0;
		  ctim->ಮುಂದಿನ_2 = tmp;

		  ctim = tmp;
	    }
      }

	/* By this point, ctim is the event_time structure that is to
	   receive the event at hand. Put the event in to the
	   appropriate list for the kind of assign we have at hand. */

      ರಚನೆ event_s** q = 0;

      ಆಯ್ಕೆ (select_ಸರತಿ) {
	  ಸ್ಥಿತಿ SEQ_ಆರಂಭ:
	    q = &ctim->ಆರಂಭ;
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ SEQ_ಸಕ್ರಿಯ:
	    q = &ctim->ಸಕ್ರಿಯ;
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ SEQ_INACTIVE:
	    ಪ್ರತಿಜ್ಞಾ(delay == 0);
	    q = &ctim->inactive;
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ SEQ_NBASSIGN:
	    q = &ctim->nbassign;
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ SEQ_RWSYNC:
	    q = &ctim->rwsync;
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ SEQ_ROSYNC:
	    q = &ctim->rosync;
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ DEL_THREAD:
	    q = &ctim->del_thr;
	    ನಿಲ್ಲಿಸು;
      }

      ಆದರೆ (q) {
	    ಆದರೆ (*q) {
		  /* Put the cur event on the end of the queue. */
		  cur->ಮುಂದಿನ_2 = (*q)->ಮುಂದಿನ_2;
		  (*q)->ಮುಂದಿನ_2 = cur;
	    }
	    *q = cur;
      }
}

ಸ್ಥಿರ ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_event_ತಳ್ಳು(ರಚನೆ event_s*cur)
{
      ಆದರೆ ((sched_list == 0) || (sched_list->delay > 0)) {
	    ವೇಳಾಪಟ್ಟಿ_event(cur, 0, SEQ_ಸಕ್ರಿಯ);
	    ಹಿಂತಿರುಗಿ;
      }

      ರಚನೆ event_ಸಮಯ_s*ctim = sched_list;

      ಆದರೆ (ctim->ಸಕ್ರಿಯ == 0) {
	    cur->ಮುಂದಿನ_2 = cur;
	    ctim->ಸಕ್ರಿಯ = cur;
	    ಹಿಂತಿರುಗಿ;
      }

      cur->ಮುಂದಿನ_2 = ctim->ಸಕ್ರಿಯ->ಮುಂದಿನ_2;
      ctim->ಸಕ್ರಿಯ->ಮುಂದಿನ_2 = cur;
}

ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_vthread(vthread_t thr, vvp_time64_t delay, ತಾರ್ಕಿಕ ತಳ್ಳು_flag)
{
      ರಚನೆ vthread_event_s*cur = ಹೊಸ vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);

      ಆದರೆ (ತಳ್ಳು_flag && (delay == 0)) {
	      /* Special case: If the delay is 0, the push_flag means
		 I can push this event in front of everything. This is
		 used by the %fork statement, for example, to perform
		 task calls. */
	    ವೇಳಾಪಟ್ಟಿ_event_ತಳ್ಳು(cur);

      } ಇಲ್ಲದಿದ್ದರೆ {
	    ವೇಳಾಪಟ್ಟಿ_event(cur, delay, SEQ_ಸಕ್ರಿಯ);
      }
}

ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_t0_trigger(vvp_net_ptr_t ptr)
{
      vvp_vector4_t ಬಿಟ್ (1, BIT4_X);
      ರಚನೆ ನಿಯೋಜಿಸು_vector4_event_s*cur = ಹೊಸ ರಚನೆ ನಿಯೋಜಿಸು_vector4_event_s(ಬಿಟ್);
      cur->ptr = ptr;
      ವೇಳಾಪಟ್ಟಿ_event(cur, 0, SEQ_INACTIVE);
}

ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_inactive(vthread_t thr)
{
      ರಚನೆ vthread_event_s*cur = ಹೊಸ vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);
      ವೇಳಾಪಟ್ಟಿ_event(cur, 0, SEQ_INACTIVE);
}

ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_init_vthread(vthread_t thr)
{
      ರಚನೆ vthread_event_s*cur = ಹೊಸ vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);

      ವೇಳಾಪಟ್ಟಿ_init_event(cur);
}

ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_final_vthread(vthread_t thr)
{
      ರಚನೆ vthread_event_s*cur = ಹೊಸ vthread_event_s;

      cur->thr = thr;
      vthread_mark_final(thr);
      vthread_mark_scheduled(thr);

      ವೇಳಾಪಟ್ಟಿ_final_event(cur);
}

ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_ನಿಯೋಜಿಸು_ಸದಿಶ(vvp_net_ptr_t ptr,
			    ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid,
			    ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
			    vvp_time64_t delay)
{
      ರಚನೆ ನಿಯೋಜಿಸು_vector4_event_s*cur = ಹೊಸ ರಚನೆ ನಿಯೋಜಿಸು_vector4_event_s(ಬಿಟ್);
      cur->ptr = ptr;
      cur->ತಳ = ತಳ;
      cur->vwid = vwid;
      ವೇಳಾಪಟ್ಟಿ_event(cur, delay, SEQ_NBASSIGN);
}

ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_force_ಸದಿಶ(vvp_net_t*net,
			    ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid,
			    ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
			    vvp_time64_t delay)
{
      ರಚನೆ force_vector4_event_s*cur = ಹೊಸ ರಚನೆ force_vector4_event_s(ಬಿಟ್);
      cur->net = net;
      cur->ತಳ = ತಳ;
      cur->vwid = vwid;
      ವೇಳಾಪಟ್ಟಿ_event(cur, delay, SEQ_NBASSIGN);
}

ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_propagate_ಸದಿಶ(vvp_net_t*net,
			       vvp_time64_t delay,
			       ಸ್ಥಿರಾಂಕ vvp_vector4_t&src)
{
      ರಚನೆ propagate_vector4_event_s*cur
	    = ಹೊಸ ರಚನೆ propagate_vector4_event_s(src);
      cur->net = net;
      ವೇಳಾಪಟ್ಟಿ_event(cur, delay, SEQ_NBASSIGN);
}

// FIXME: This needs to create a non-blocking event, but only one per time slot.
//        Is schedule_event_ or execution actually filtering since the net is
//        already X because this is not triggering?
ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_propagate_event(vvp_net_t*net,
                              vvp_time64_t delay)
{
      vvp_vector4_t tmp (1, BIT4_X);
      ರಚನೆ propagate_vector4_event_s*cur
	    = ಹೊಸ ರಚನೆ propagate_vector4_event_s(tmp);
      cur->net = net;
      ವೇಳಾಪಟ್ಟಿ_event(cur, delay, SEQ_NBASSIGN);
}

ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_ನಿಯೋಜಿಸು_array_ಶಬ್ದ(vvp_array_t mem,
				ಚಿಹ್ನೆರಹಿತ ಶಬ್ದ_addr,
				ಚಿಹ್ನೆರಹಿತ off,
				ಸ್ಥಿರಾಂಕ vvp_vector4_t&val,
				vvp_time64_t delay)
{
      ರಚನೆ ನಿಯೋಜಿಸು_array_ಶಬ್ದ_s*cur = ಹೊಸ ರಚನೆ ನಿಯೋಜಿಸು_array_ಶಬ್ದ_s;
      cur->mem = mem;
      cur->adr = ಶಬ್ದ_addr;
      cur->off = off;
      cur->val = val;
      ವೇಳಾಪಟ್ಟಿ_event(cur, delay, SEQ_NBASSIGN);
}

ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_ನಿಯೋಜಿಸು_array_ಶಬ್ದ(vvp_array_t mem,
				ಚಿಹ್ನೆರಹಿತ ಶಬ್ದ_addr,
				ದ್ವಿಗುಣ val,
				vvp_time64_t delay)
{
      ರಚನೆ ನಿಯೋಜಿಸು_array_r_ಶಬ್ದ_s*cur = ಹೊಸ ರಚನೆ ನಿಯೋಜಿಸು_array_r_ಶಬ್ದ_s;
      cur->mem = mem;
      cur->adr = ಶಬ್ದ_addr;
      cur->val = val;
      ವೇಳಾಪಟ್ಟಿ_event(cur, delay, SEQ_NBASSIGN);
}

ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_ಗಣ_ಸದಿಶ(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್)
{
      ರಚನೆ ನಿಯೋಜಿಸು_vector4_event_s*cur = ಹೊಸ ರಚನೆ ನಿಯೋಜಿಸು_vector4_event_s(ಬಿಟ್);
      cur->ptr = ptr;
      cur->ತಳ = 0;
      cur->vwid = 0;
      ವೇಳಾಪಟ್ಟಿ_event(cur, 0, SEQ_ಸಕ್ರಿಯ);
}

ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_ಗಣ_ಸದಿಶ(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector8_t&ಬಿಟ್)
{
      ರಚನೆ ನಿಯೋಜಿಸು_vector8_event_s*cur = ಹೊಸ ರಚನೆ ನಿಯೋಜಿಸು_vector8_event_s;
      cur->ptr = ptr;
      cur->val = ಬಿಟ್;
      ವೇಳಾಪಟ್ಟಿ_event(cur, 0, SEQ_ಸಕ್ರಿಯ);
}

ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_ಗಣ_ಸದಿಶ(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ ಬಿಟ್)
{
      ರಚನೆ ನಿಯೋಜಿಸು_real_event_s*cur = ಹೊಸ ರಚನೆ ನಿಯೋಜಿಸು_real_event_s;
      cur->ptr = ptr;
      cur->val = ಬಿಟ್;
      ವೇಳಾಪಟ್ಟಿ_event(cur, 0, SEQ_ಸಕ್ರಿಯ);
}

ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_init_ಸದಿಶ(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್)
{
      ರಚನೆ ನಿಯೋಜಿಸು_vector4_event_s*cur = ಹೊಸ ರಚನೆ ನಿಯೋಜಿಸು_vector4_event_s(ಬಿಟ್);
      cur->ptr = ptr;
      cur->ತಳ = 0;
      cur->vwid = 0;
      ವೇಳಾಪಟ್ಟಿ_init_event(cur);
}

ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_init_ಸದಿಶ(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector8_t&ಬಿಟ್)
{
      ರಚನೆ ನಿಯೋಜಿಸು_vector8_event_s*cur = ಹೊಸ ರಚನೆ ನಿಯೋಜಿಸು_vector8_event_s;
      cur->ptr = ptr;
      cur->val = ಬಿಟ್;
      ವೇಳಾಪಟ್ಟಿ_init_event(cur);
}

ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_init_ಸದಿಶ(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ ಬಿಟ್)
{
      ರಚನೆ ನಿಯೋಜಿಸು_real_event_s*cur = ಹೊಸ ರಚನೆ ನಿಯೋಜಿಸು_real_event_s;
      cur->ptr = ptr;
      cur->val = ಬಿಟ್;
      ವೇಳಾಪಟ್ಟಿ_init_event(cur);
}

ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_init_propagate(vvp_net_t*net, vvp_vector4_t ಬಿಟ್)
{
      ರಚನೆ propagate_vector4_event_s*cur = ಹೊಸ ರಚನೆ propagate_vector4_event_s(ಬಿಟ್);
      cur->net = net;
      ವೇಳಾಪಟ್ಟಿ_init_event(cur);
}

ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_init_propagate(vvp_net_t*net, ದ್ವಿಗುಣ ಬಿಟ್)
{
      ರಚನೆ propagate_real_event_s*cur = ಹೊಸ ರಚನೆ propagate_real_event_s;
      cur->net = net;
      cur->val = ಬಿಟ್;
      ವೇಳಾಪಟ್ಟಿ_init_event(cur);
}

ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_del_thr(vthread_t thr)
{
      ರಚನೆ del_thr_event_s*cur = ಹೊಸ del_thr_event_s;

      cur->thr = thr;

      ವೇಳಾಪಟ್ಟಿ_event(cur, 0, DEL_THREAD);
}

ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_generic(vvp_gen_event_t obj, vvp_time64_t delay,
		      ತಾರ್ಕಿಕ sync_flag, ತಾರ್ಕಿಕ ro_flag, ತಾರ್ಕಿಕ delete_when_ಮುಗಿದ)
{
      ರಚನೆ generic_event_s*cur = ಹೊಸ generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_ಮುಗಿದ = delete_when_ಮುಗಿದ;
      ವೇಳಾಪಟ್ಟಿ_event(cur, delay,
		      sync_flag? (ro_flag?SEQ_ROSYNC:SEQ_RWSYNC) : SEQ_ಸಕ್ರಿಯ);

      ಆದರೆ (sync_flag)
	    vthread_delay_delete();
}

ಸ್ಥಿರ ತಾರ್ಕಿಕ sim_started;

ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_functor(vvp_gen_event_t obj)
{
      ರಚನೆ generic_event_s*cur = ಹೊಸ generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_ಮುಗಿದ = ಸುಳ್ಳು;
      ಆದರೆ (!sim_started) {
            ವೇಳಾಪಟ್ಟಿ_init_event(cur);
      } ಇಲ್ಲದಿದ್ದರೆ {
            ವೇಳಾಪಟ್ಟಿ_event(cur, 0, SEQ_ಸಕ್ರಿಯ);
      }
}

ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_at_ಆರಂಭ_ದ_simtime(vvp_gen_event_t obj, vvp_time64_t delay)
{
      ರಚನೆ generic_event_s*cur = ಹೊಸ generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_ಮುಗಿದ = ಸುಳ್ಳು;
      ವೇಳಾಪಟ್ಟಿ_event(cur, delay, SEQ_ಆರಂಭ);
}

/*
 * In the vvp runtime of Icarus Verilog, the SEQ_RWSYNC time step is
 * after all of the non-blocking assignments, so is effectively the
 * same as the ReadWriteSync time.
 */
ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_at_ಕೊನೆ_ದ_simtime(vvp_gen_event_t obj, vvp_time64_t delay)
{
      ರಚನೆ generic_event_s*cur = ಹೊಸ generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_ಮುಗಿದ = ಸುಳ್ಳು;
      ವೇಳಾಪಟ್ಟಿ_event(cur, delay, SEQ_RWSYNC);
}

ಸ್ಥಿರ vvp_time64_t ವೇಳಾಪಟ್ಟಿ_ಸಮಯ;
vvp_time64_t ವೇಳಾಪಟ್ಟಿ_simtime(ಶೂನ್ಯ)
{ ಹಿಂತಿರುಗಿ ವೇಳಾಪಟ್ಟಿ_ಸಮಯ; }

ಸ್ಥಿರ ತಾರ್ಕಿಕ sim_at_rosync = ಸುಳ್ಳು;
ತಾರ್ಕಿಕ ವೇಳಾಪಟ್ಟಿ_at_rosync(ಶೂನ್ಯ)
{ ಹಿಂತಿರುಗಿ sim_at_rosync; }

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
ಸ್ಥಿರ ಶೂನ್ಯ ಸತತ_rosync(ರಚನೆ event_ಸಮಯ_s*ctim)
{
      sim_at_rosync = ನಿಜ;
      ಆಗ (ctim->rosync) {
	    ರಚನೆ event_s*cur = ctim->rosync->ಮುಂದಿನ_2;
	    ಆದರೆ (cur->ಮುಂದಿನ_2 == cur) {
		  ctim->rosync = 0;
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  ctim->rosync->ಮುಂದಿನ_2 = cur->ಮುಂದಿನ_2;
	    }

	    cur->ಸತತ_ಸತತ();
	    ಅಳಿಸು cur;
      }
      sim_at_rosync = ಸುಳ್ಳು;

      ಆಗ (ctim->del_thr) {
	    ರಚನೆ event_s*cur = ctim->del_thr->ಮುಂದಿನ_2;
	    ಆದರೆ (cur->ಮುಂದಿನ_2 == cur) {
		  ctim->del_thr = 0;
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  ctim->del_thr->ಮುಂದಿನ_2 = cur->ಮುಂದಿನ_2;
	    }

	    cur->ಸತತ_ಸತತ();
	    ಅಳಿಸು cur;
      }

      ಆದರೆ (ctim->ಸಕ್ರಿಯ || ctim->inactive || ctim->nbassign || ctim->rwsync) {
	    cerr << "SCHEDULER ERROR: read-only sync events "
		 << "created RW events!" << ಸಾಲು_ಅಂತ್ಯ;
      }
}

ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_simulate(ಶೂನ್ಯ)
{
      ತಾರ್ಕಿಕ ಸತತ_finals;
      sim_started = ಸುಳ್ಳು;

      ವೇಳಾಪಟ್ಟಿ_ಸಮಯ = 0;

      ಆದರೆ (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute EndOfCompile callbacks\n");
      }

      // Execute end of compile callbacks
      vpi_ಕೊನೆ_ದ_Compile();

      ಆದರೆ (verbose_flag) {
	    vpi_mcd_printf(1, " ...propagate initialization events\n");
      }

	// Execute initialization events.
      ಆಗ (ವೇಳಾಪಟ್ಟಿ_init_list) {
	    ರಚನೆ event_s*cur = ವೇಳಾಪಟ್ಟಿ_init_list->ಮುಂದಿನ_2;
	    ಆದರೆ (cur->ಮುಂದಿನ_2 == cur) {
		  ವೇಳಾಪಟ್ಟಿ_init_list = 0;
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  ವೇಳಾಪಟ್ಟಿ_init_list->ಮುಂದಿನ_2 = cur->ಮುಂದಿನ_2;
	    }
	    cur->ಸತತ_ಸತತ();
	    ಅಳಿಸು cur;
      }

      sim_started = ನಿಜ;

      ಆದರೆ (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute StartOfSim callbacks\n");
      }

      // Execute start of simulation callbacks
      vpi_ಆರಂಭ_ದ_Sim();

      ಸಂಕೇತಗಳು_capture();

      ಆದರೆ (verbose_flag) {
	    vpi_mcd_printf(1, " ...run scheduler\n");
      }

      // If there were no compiletf, etc. errors then we are going to
      // process events and when done run the final blocks.
      ಸತತ_finals = ವೇಳಾಪಟ್ಟಿ_runnable;

      ಆದರೆ (ವೇಳಾಪಟ್ಟಿ_runnable) ಆಗ (sched_list) {

	    ಆದರೆ (ವೇಳಾಪಟ್ಟಿ_stopped_flag) {
		  ವೇಳಾಪಟ್ಟಿ_stopped_flag = ಸುಳ್ಳು;
		  stop_handler(0);
		  // You can finish from the debugger without a time change.
		  ಆದರೆ (!ವೇಳಾಪಟ್ಟಿ_runnable) ನಿಲ್ಲಿಸು;
		  ಮುಂದುವರಿ;
	    }

	      /* ctim is the current time step. */
	    ರಚನೆ event_ಸಮಯ_s* ctim = sched_list;

	      /* If the time is advancing, then first run the
		 postponed sync events. Run them all. */
	    ಆದರೆ (ctim->delay > 0) {

		  ಆದರೆ (!ವೇಳಾಪಟ್ಟಿ_runnable) ನಿಲ್ಲಿಸು;
		  ವೇಳಾಪಟ್ಟಿ_ಸಮಯ += ctim->delay;
		    /* When the design is being traced (we are emitting
		     * file/line information) also print any time changes. */
		  ಆದರೆ (show_file_ಸಾಲು) {
			cerr << "Advancing to simulation time: "
			     << ವೇಳಾಪಟ್ಟಿ_ಸಮಯ << ಸಾಲು_ಅಂತ್ಯ;
		  }
		  ctim->delay = 0;

		  vpi_ಮುಂದಿನ_Sim_ಸಮಯ();
		    // Process the cbAtStartOfSimTime callbacks.
		  ಆಗ (ctim->ಆರಂಭ) {
			ರಚನೆ event_s*cur = ctim->ಆರಂಭ->ಮುಂದಿನ_2;
			ಆದರೆ (cur->ಮುಂದಿನ_2 == cur) {
			      ctim->ಆರಂಭ = 0;
			} ಇಲ್ಲದಿದ್ದರೆ {
			      ctim->ಆರಂಭ->ಮುಂದಿನ_2 = cur->ಮುಂದಿನ_2;
			}
			cur->ಸತತ_ಸತತ();
			ಅಳಿಸು (cur);
		  }
	    }


	      /* If there are no more active events, advance the event
		 queues. If there are not events at all, then release
		 the event_time object. */
	    ಆದರೆ (ctim->ಸಕ್ರಿಯ == 0) {
		  ctim->ಸಕ್ರಿಯ = ctim->inactive;
		  ctim->inactive = 0;

		  ಆದರೆ (ctim->ಸಕ್ರಿಯ == 0) {
			ctim->ಸಕ್ರಿಯ = ctim->nbassign;
			ctim->nbassign = 0;

			ಆದರೆ (ctim->ಸಕ್ರಿಯ == 0) {
			      ctim->ಸಕ್ರಿಯ = ctim->rwsync;
			      ctim->rwsync = 0;

				/* If out of rw events, then run the rosync
				   events and delete this time step. This also
				   deletes threads as needed. */
			      ಆದರೆ (ctim->ಸಕ್ರಿಯ == 0) {
				    ಸತತ_rosync(ctim);
				    sched_list = ctim->ಮುಂದಿನ_2;
				    ಅಳಿಸು ctim;
				    ಮುಂದುವರಿ;
			      }
			}
		  }
	    }

	      /* Pull the first item off the list. If this is the last
		 cell in the list, then clear the list. Execute that
		 event type, and delete it. */
	    ರಚನೆ event_s*cur = ctim->ಸಕ್ರಿಯ->ಮುಂದಿನ_2;
	    ಆದರೆ (cur->ಮುಂದಿನ_2 == cur) {
		  ctim->ಸಕ್ರಿಯ = 0;
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  ctim->ಸಕ್ರಿಯ->ಮುಂದಿನ_2 = cur->ಮುಂದಿನ_2;
	    }

	    ಆದರೆ (ವೇಳಾಪಟ್ಟಿ_single_ಹೆಜ್ಜೆ_flag) {
		  cur->single_ಹೆಜ್ಜೆ_display();
		  ವೇಳಾಪಟ್ಟಿ_stopped_flag = ನಿಜ;
		  ವೇಳಾಪಟ್ಟಿ_single_ಹೆಜ್ಜೆ_flag = ಸುಳ್ಳು;
	    }

	    cur->ಸತತ_ಸತತ();

	    ಅಳಿಸು (cur);
      }

	// Execute final events.
      ವೇಳಾಪಟ್ಟಿ_runnable = ಸತತ_finals;
      ಆಗ (ವೇಳಾಪಟ್ಟಿ_runnable && ವೇಳಾಪಟ್ಟಿ_final_list) {
	    ರಚನೆ event_s*cur = ವೇಳಾಪಟ್ಟಿ_final_list->ಮುಂದಿನ_2;
	    ಆದರೆ (cur->ಮುಂದಿನ_2 == cur) {
		  ವೇಳಾಪಟ್ಟಿ_final_list = 0;
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  ವೇಳಾಪಟ್ಟಿ_final_list->ಮುಂದಿನ_2 = cur->ಮುಂದಿನ_2;
	    }
	    cur->ಸತತ_ಸತತ();
	    ಅಳಿಸು cur;
      }

      ಸಂಕೇತಗಳು_revert();

      ಆದರೆ (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute Postsim callbacks\n");
      }

      // Execute post-simulation callbacks
      vpiPostsim();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
      ವೇಳಾಪಟ್ಟಿ_delete();
#ಆದರೆ_ಅಂತ್ಯ
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_delete(ಶೂನ್ಯ)
{
      vthread_event_ರಾಶಿ.delete_pool();
      assign4_ರಾಶಿ.delete_pool();
      assign8_ರಾಶಿ.delete_pool();
      assignr_ರಾಶಿ.delete_pool();
      array_w_ರಾಶಿ.delete_pool();
      array_r_w_ರಾಶಿ.delete_pool();
      generic_event_ರಾಶಿ.delete_pool();
      event_ಸಮಯ_ರಾಶಿ.delete_pool();
}
#ಆದರೆ_ಅಂತ್ಯ
