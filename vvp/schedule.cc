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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
# include  "ivl_alloc.h"
#ਜੇ_ਅੰਤ

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ std;

ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਗਿਣਤੀ_ਸੌਂਪੋ_events = 0;
ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਗਿਣਤੀ_gen_events = 0;
ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਗਿਣਤੀ_thread_events = 0;
  // Count the time events (A time cell created)
ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਗਿਣਤੀ_ਸਮਾਂ_events = 0;



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
ਬਣਤਰ event_s {
      ਬਣਤਰ event_s*ਅਗਲਾ_2;
      ਆਭਾਸੀ ~event_s() { }
      ਆਭਾਸੀ ਖਾਲੀ ਲੜੀਵਾਰ_ਲੜੀਵਾਰ(ਖਾਲੀ) =0;

	// Write something about the event to stderr
      ਆਭਾਸੀ ਖਾਲੀ single_ਕਦਮ_display(ਖਾਲੀ);

	// Fallback new/delete
      ਸਥਿਰ ਖਾਲੀ*ਸੰਚਾਲਕ ਨਵਾਂ (size_t size) { ਵਾਪਸ ::ਨਵਾਂ ਅੱਖਰ[size]; }
      ਸਥਿਰ ਖਾਲੀ ਸੰਚਾਲਕ ਮਿਟਾਓ(ਖਾਲੀ*ptr)  { ::ਮਿਟਾਓ[]( static_cast<ਅੱਖਰ*>(ptr) ); }
};

ਖਾਲੀ event_s::single_ਕਦਮ_display(ਖਾਲੀ)
{
      std::cerr << "event_s: Step into event " << typeid(*ਇਹ).name() << std::ਸਤਰ_ਅੰਤ;
}

ਬਣਤਰ event_ਸਮਾਂ_s {
      event_ਸਮਾਂ_s() {
	    ਗਿਣਤੀ_ਸਮਾਂ_events += 1;
	    delay = 0;
	    ਸ਼ੁਰੂ = 0;
	    ਸਰਗਰਮ = 0;
	    inactive = 0;
	    nbassign = 0;
	    rwsync = 0;
	    rosync = 0;
	    del_thr = 0;
	    ਅਗਲਾ_2 = ਨਿਰਾ;
      }
      vvp_time64_t delay;

      ਬਣਤਰ event_s*ਸ਼ੁਰੂ;
      ਬਣਤਰ event_s*ਸਰਗਰਮ;
      ਬਣਤਰ event_s*inactive;
      ਬਣਤਰ event_s*nbassign;
      ਬਣਤਰ event_s*rwsync;
      ਬਣਤਰ event_s*rosync;
      ਬਣਤਰ event_s*del_thr;

      ਬਣਤਰ event_ਸਮਾਂ_s*ਅਗਲਾ_2;

      ਸਥਿਰ ਖਾਲੀ* ਸੰਚਾਲਕ ਨਵਾਂ (size_t);
      ਸਥਿਰ ਖਾਲੀ ਸੰਚਾਲਕ ਮਿਟਾਓ(ਖਾਲੀ*obj, size_t s);
};

vvp_gen_event_s::~vvp_gen_event_s()
{
}

ਖਾਲੀ vvp_gen_event_s::single_ਕਦਮ_display(ਖਾਲੀ)
{
      cerr << "vvp_gen_event_s: Step into event " << typeid(*ਇਹ).name() << ਸਤਰ_ਅੰਤ;
}

/*
 * Derived event types
 */
ਬਣਤਰ vthread_event_s : ਜਨਤਕ event_s {
      vthread_t thr;
      ਖਾਲੀ ਲੜੀਵਾਰ_ਲੜੀਵਾਰ(ਖਾਲੀ) override;
      ਖਾਲੀ single_ਕਦਮ_display(ਖਾਲੀ) override;

      ਸਥਿਰ ਖਾਲੀ* ਸੰਚਾਲਕ ਨਵਾਂ(size_t);
      ਸਥਿਰ ਖਾਲੀ ਸੰਚਾਲਕ ਮਿਟਾਓ(ਖਾਲੀ*);
};

ਖਾਲੀ vthread_event_s::ਲੜੀਵਾਰ_ਲੜੀਵਾਰ(ਖਾਲੀ)
{
      ਗਿਣਤੀ_thread_events += 1;
      vthread_ਲੜੀਵਾਰ(thr);
}

ਖਾਲੀ vthread_event_s::single_ਕਦਮ_display(ਖਾਲੀ)
{
      __vpiScope*scope = vthread_scope(thr);
      cerr << "vthread_event: Resume thread"
	   << " scope=" << scope->vpi_get_str(vpi_ਭਰਿਆ_Name)
	   << ਸਤਰ_ਅੰਤ;
}

ਸਥਿਰ ਅਚਲ size_t VTHR_CHUNK_ਗਿਣਤੀ = 8192 / ਆਕਾਰ(ਬਣਤਰ vthread_event_s);
ਸਥਿਰ slab_t<ਆਕਾਰ(vthread_event_s),VTHR_CHUNK_ਗਿਣਤੀ> vthread_event_ਢੇਰ;

ਸਤਰਅੰਦਰ ਖਾਲੀ* vthread_event_s::ਸੰਚਾਲਕ ਨਵਾਂ(size_t size)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(size == ਆਕਾਰ(vthread_event_s));
      ਵਾਪਸ vthread_event_ਢੇਰ.alloc_slab();
}

ਖਾਲੀ vthread_event_s::ਸੰਚਾਲਕ ਮਿਟਾਓ(ਖਾਲੀ*dptr)
{
      vthread_event_ਢੇਰ.free_slab(dptr);
}

ਬਣਤਰ del_thr_event_s : ਜਨਤਕ event_s {
      vthread_t thr;
      ਖਾਲੀ ਲੜੀਵਾਰ_ਲੜੀਵਾਰ(ਖਾਲੀ) override;
      ਖਾਲੀ single_ਕਦਮ_display(ਖਾਲੀ) override;
};

ਖਾਲੀ del_thr_event_s::ਲੜੀਵਾਰ_ਲੜੀਵਾਰ(ਖਾਲੀ)
{
      vthread_delete(thr);
}

ਖਾਲੀ del_thr_event_s::single_ਕਦਮ_display(ਖਾਲੀ)
{
      __vpiScope*scope = vthread_scope(thr);
      cerr << "del_thr_event: Reap completed thread"
	   << " scope=" << scope->vpi_get_str(vpi_ਭਰਿਆ_Name) << ਸਤਰ_ਅੰਤ;
}

ਬਣਤਰ ਸੌਂਪੋ_vector4_event_s  : ਜਨਤਕ event_s {
	/* The default constructor. */
      ਸਪਸ਼ਟ ਸੌਂਪੋ_vector4_event_s(ਅਚਲ vvp_vector4_t&that) : val(that) {
	    ਅਧਾਰ = 0;
	    vwid = 0;
	    ਅਗਲਾ_2 = ਨਿਰਾ;
      }

	/* Where to do the assign. */
      vvp_net_ptr_t ptr;
	/* Value to assign. */
      vvp_vector4_t val;
	/* Offset of the part into the destination. */
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ;
	/* Width of the destination vector. */
      ਬਿਨਾਨਿਸ਼ਾਨੀ vwid;
      ਖਾਲੀ ਲੜੀਵਾਰ_ਲੜੀਵਾਰ(ਖਾਲੀ) override;
      ਖਾਲੀ single_ਕਦਮ_display(ਖਾਲੀ) override;

      ਸਥਿਰ ਖਾਲੀ* ਸੰਚਾਲਕ ਨਵਾਂ(size_t);
      ਸਥਿਰ ਖਾਲੀ ਸੰਚਾਲਕ ਮਿਟਾਓ(ਖਾਲੀ*);
};

ਖਾਲੀ ਸੌਂਪੋ_vector4_event_s::ਲੜੀਵਾਰ_ਲੜੀਵਾਰ(ਖਾਲੀ)
{
      ਗਿਣਤੀ_ਸੌਂਪੋ_events += 1;
      ਜੇ (vwid > 0)
	    vvp_send_vec4_pv(ptr, val, ਅਧਾਰ, vwid, 0);
      ਨਹੀਂਤਾਂ
	    vvp_send_vec4(ptr, val, 0);
}

ਖਾਲੀ ਸੌਂਪੋ_vector4_event_s::single_ਕਦਮ_display(ਖਾਲੀ)
{
      cerr << "assign_vector4_event: Propagate val=" << val
	   << ", vwid=" << vwid << ", base=" << ਅਧਾਰ << ਸਤਰ_ਅੰਤ;
}

ਸਥਿਰ ਅਚਲ size_t ASSIGN4_CHUNK_ਗਿਣਤੀ = 524288 / ਆਕਾਰ(ਬਣਤਰ ਸੌਂਪੋ_vector4_event_s);
ਸਥਿਰ slab_t<ਆਕਾਰ(ਸੌਂਪੋ_vector4_event_s),ASSIGN4_CHUNK_ਗਿਣਤੀ> assign4_ਢੇਰ;

ਸਤਰਅੰਦਰ ਖਾਲੀ* ਸੌਂਪੋ_vector4_event_s::ਸੰਚਾਲਕ ਨਵਾਂ(size_t size)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(size == ਆਕਾਰ(ਸੌਂਪੋ_vector4_event_s));
      ਵਾਪਸ assign4_ਢੇਰ.alloc_slab();
}

ਖਾਲੀ ਸੌਂਪੋ_vector4_event_s::ਸੰਚਾਲਕ ਮਿਟਾਓ(ਖਾਲੀ*dptr)
{
      assign4_ਢੇਰ.free_slab(dptr);
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਗਿਣਤੀ_assign4_pool(ਖਾਲੀ) { ਵਾਪਸ assign4_ਢੇਰ.pool; }

ਬਣਤਰ ਸੌਂਪੋ_vector8_event_s  : ਜਨਤਕ event_s {
      vvp_net_ptr_t ptr;
      vvp_vector8_t val;
      ਖਾਲੀ ਲੜੀਵਾਰ_ਲੜੀਵਾਰ(ਖਾਲੀ) override;
      ਖਾਲੀ single_ਕਦਮ_display(ਖਾਲੀ) override;

      ਸਥਿਰ ਖਾਲੀ* ਸੰਚਾਲਕ ਨਵਾਂ(size_t);
      ਸਥਿਰ ਖਾਲੀ ਸੰਚਾਲਕ ਮਿਟਾਓ(ਖਾਲੀ*);
};

ਖਾਲੀ ਸੌਂਪੋ_vector8_event_s::ਲੜੀਵਾਰ_ਲੜੀਵਾਰ(ਖਾਲੀ)
{
      ਗਿਣਤੀ_ਸੌਂਪੋ_events += 1;
      vvp_send_vec8(ptr, val);
}

ਖਾਲੀ ਸੌਂਪੋ_vector8_event_s::single_ਕਦਮ_display(ਖਾਲੀ)
{
      cerr << "assign_vector8_event: Propagate val=" << val << ਸਤਰ_ਅੰਤ;
}

ਸਥਿਰ ਅਚਲ size_t ASSIGN8_CHUNK_ਗਿਣਤੀ = 8192 / ਆਕਾਰ(ਬਣਤਰ ਸੌਂਪੋ_vector8_event_s);
ਸਥਿਰ slab_t<ਆਕਾਰ(ਸੌਂਪੋ_vector8_event_s),ASSIGN8_CHUNK_ਗਿਣਤੀ> assign8_ਢੇਰ;

ਸਤਰਅੰਦਰ ਖਾਲੀ* ਸੌਂਪੋ_vector8_event_s::ਸੰਚਾਲਕ ਨਵਾਂ(size_t size)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(size == ਆਕਾਰ(ਸੌਂਪੋ_vector8_event_s));
      ਵਾਪਸ assign8_ਢੇਰ.alloc_slab();
}

ਖਾਲੀ ਸੌਂਪੋ_vector8_event_s::ਸੰਚਾਲਕ ਮਿਟਾਓ(ਖਾਲੀ*dptr)
{
      assign8_ਢੇਰ.free_slab(dptr);
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਗਿਣਤੀ_assign8_pool() { ਵਾਪਸ assign8_ਢੇਰ.pool; }

ਬਣਤਰ ਸੌਂਪੋ_real_event_s  : ਜਨਤਕ event_s {
      vvp_net_ptr_t ptr;
      ਦੁੱਗਣਾ val;
      ਖਾਲੀ ਲੜੀਵਾਰ_ਲੜੀਵਾਰ(ਖਾਲੀ) override;
      ਖਾਲੀ single_ਕਦਮ_display(ਖਾਲੀ) override;

      ਸਥਿਰ ਖਾਲੀ* ਸੰਚਾਲਕ ਨਵਾਂ(size_t);
      ਸਥਿਰ ਖਾਲੀ ਸੰਚਾਲਕ ਮਿਟਾਓ(ਖਾਲੀ*);
};

ਖਾਲੀ ਸੌਂਪੋ_real_event_s::ਲੜੀਵਾਰ_ਲੜੀਵਾਰ(ਖਾਲੀ)
{
      ਗਿਣਤੀ_ਸੌਂਪੋ_events += 1;
      vvp_send_real(ptr, val, 0);
}

ਖਾਲੀ ਸੌਂਪੋ_real_event_s::single_ਕਦਮ_display(ਖਾਲੀ)
{
      cerr << "assign_real_event: Propagate val=" << val << ਸਤਰ_ਅੰਤ;
}

ਸਥਿਰ ਅਚਲ size_t ASSIGNR_CHUNK_ਗਿਣਤੀ = 8192 / ਆਕਾਰ(ਬਣਤਰ ਸੌਂਪੋ_real_event_s);
ਸਥਿਰ slab_t<ਆਕਾਰ(ਸੌਂਪੋ_real_event_s),ASSIGNR_CHUNK_ਗਿਣਤੀ> assignr_ਢੇਰ;

ਸਤਰਅੰਦਰ ਖਾਲੀ* ਸੌਂਪੋ_real_event_s::ਸੰਚਾਲਕ ਨਵਾਂ (size_t size)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(size == ਆਕਾਰ(ਸੌਂਪੋ_real_event_s));
      ਵਾਪਸ assignr_ਢੇਰ.alloc_slab();
}

ਖਾਲੀ ਸੌਂਪੋ_real_event_s::ਸੰਚਾਲਕ ਮਿਟਾਓ(ਖਾਲੀ*dptr)
{
      assignr_ਢੇਰ.free_slab(dptr);
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਗਿਣਤੀ_ਸੌਂਪੋ_real_pool(ਖਾਲੀ) { ਵਾਪਸ assignr_ਢੇਰ.pool; }

ਬਣਤਰ ਸੌਂਪੋ_array_ਸ਼ਬਦ_s  : ਜਨਤਕ event_s {
      vvp_array_t mem;
      ਬਿਨਾਨਿਸ਼ਾਨੀ adr;
      vvp_vector4_t val;
      ਬਿਨਾਨਿਸ਼ਾਨੀ off;
      ਖਾਲੀ ਲੜੀਵਾਰ_ਲੜੀਵਾਰ(ਖਾਲੀ) override;

      ਸਥਿਰ ਖਾਲੀ* ਸੰਚਾਲਕ ਨਵਾਂ(size_t);
      ਸਥਿਰ ਖਾਲੀ ਸੰਚਾਲਕ ਮਿਟਾਓ(ਖਾਲੀ*);
};

ਖਾਲੀ ਸੌਂਪੋ_array_ਸ਼ਬਦ_s::ਲੜੀਵਾਰ_ਲੜੀਵਾਰ(ਖਾਲੀ)
{
      ਗਿਣਤੀ_ਸੌਂਪੋ_events += 1;
      mem->ਸੈੱਟ_ਸ਼ਬਦ(adr, off, val);
}

ਸਥਿਰ ਅਚਲ size_t ARRAY_W_CHUNK_ਗਿਣਤੀ = 8192 / ਆਕਾਰ(ਬਣਤਰ ਸੌਂਪੋ_array_ਸ਼ਬਦ_s);
ਸਥਿਰ slab_t<ਆਕਾਰ(ਸੌਂਪੋ_array_ਸ਼ਬਦ_s),ARRAY_W_CHUNK_ਗਿਣਤੀ> array_w_ਢੇਰ;

ਸਤਰਅੰਦਰ ਖਾਲੀ* ਸੌਂਪੋ_array_ਸ਼ਬਦ_s::ਸੰਚਾਲਕ ਨਵਾਂ (size_t size)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(size == ਆਕਾਰ(ਸੌਂਪੋ_array_ਸ਼ਬਦ_s));
      ਵਾਪਸ array_w_ਢੇਰ.alloc_slab();
}

ਖਾਲੀ ਸੌਂਪੋ_array_ਸ਼ਬਦ_s::ਸੰਚਾਲਕ ਮਿਟਾਓ(ਖਾਲੀ*ptr)
{
      array_w_ਢੇਰ.free_slab(ptr);
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਗਿਣਤੀ_ਸੌਂਪੋ_aword_pool(ਖਾਲੀ) { ਵਾਪਸ array_w_ਢੇਰ.pool; }

ਬਣਤਰ force_vector4_event_s  : ਜਨਤਕ event_s {
	/* The default constructor. */
      ਸਪਸ਼ਟ force_vector4_event_s(ਅਚਲ vvp_vector4_t&that): val(that) {
	    net = ਨਿਰਾ;
	    ਅਧਾਰ = 0;
	    vwid = 0;
	    ਅਗਲਾ_2 = ਨਿਰਾ;
      }
	/* Where to do the force. */
      vvp_net_t*net;
	/* Value to force. */
      vvp_vector4_t val;
	/* Offset of the part into the destination. */
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ;
	/* Width of the destination vector. */
      ਬਿਨਾਨਿਸ਼ਾਨੀ vwid;

      ਖਾਲੀ ਲੜੀਵਾਰ_ਲੜੀਵਾਰ(ਖਾਲੀ) override;
      ਖਾਲੀ single_ਕਦਮ_display(ਖਾਲੀ) override;

      ਸਥਿਰ ਖਾਲੀ* ਸੰਚਾਲਕ ਨਵਾਂ(size_t);
      ਸਥਿਰ ਖਾਲੀ ਸੰਚਾਲਕ ਮਿਟਾਓ(ਖਾਲੀ*);
};

ਖਾਲੀ force_vector4_event_s::ਲੜੀਵਾਰ_ਲੜੀਵਾਰ(ਖਾਲੀ)
{
      ਗਿਣਤੀ_ਸੌਂਪੋ_events += 1;

      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = val.size();
      ਜੇ ((ਅਧਾਰ + wid) > vwid)
	    wid = vwid - ਅਧਾਰ;

	// Make a mask of which bits are to be forced, 0 for unforced
	// bits and 1 for forced bits.
      vvp_vector2_t ਨਕਾਬ (vvp_vector2_t::FILL0, vwid);
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < wid ; idx += 1)
	    ਨਕਾਬ.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(ਅਧਾਰ+idx, 1);

      vvp_vector4_t tmp (vwid, BIT4_Z);

	// vvp_net_t::force_vec4 propagates all the bits of the
	// forced vector value, regardless of the mask. This
	// ensures the unforced bits retain their current value.
      vvp_signal_ਮੁੱਲ*sig = dynamic_cast<vvp_signal_ਮੁੱਲ*>(net->fil);
      ਪ੍ਰਤਿਜ੍ਞਾ(sig);
      sig->vec4_ਮੁੱਲ(tmp);

      tmp.ਸੈੱਟ_vec(ਅਧਾਰ, val);
      net->force_vec4(tmp, ਨਕਾਬ);
}

ਖਾਲੀ force_vector4_event_s::single_ਕਦਮ_display(ਖਾਲੀ)
{
      cerr << "force_vector4_event: Force val=" << val
	   << ", vwid=" << vwid << ", base=" << ਅਧਾਰ << ਸਤਰ_ਅੰਤ;
}

ਸਥਿਰ ਅਚਲ size_t FORCE4_CHUNK_ਗਿਣਤੀ = 8192 / ਆਕਾਰ(ਬਣਤਰ force_vector4_event_s);
ਸਥਿਰ slab_t<ਆਕਾਰ(force_vector4_event_s),FORCE4_CHUNK_ਗਿਣਤੀ> force4_ਢੇਰ;

ਸਤਰਅੰਦਰ ਖਾਲੀ* force_vector4_event_s::ਸੰਚਾਲਕ ਨਵਾਂ(size_t size)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(size == ਆਕਾਰ(force_vector4_event_s));
      ਵਾਪਸ force4_ਢੇਰ.alloc_slab();
}

ਖਾਲੀ force_vector4_event_s::ਸੰਚਾਲਕ ਮਿਟਾਓ(ਖਾਲੀ*dptr)
{
      force4_ਢੇਰ.free_slab(dptr);
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਗਿਣਤੀ_force4_pool(ਖਾਲੀ) { ਵਾਪਸ force4_ਢੇਰ.pool; }

/*
 * This class supports the propagation of vec4 outputs from a
 * vvp_net_t object.
 */
ਬਣਤਰ propagate_vector4_event_s : ਜਨਤਕ event_s {
	/* The default constructor. */
      ਸਪਸ਼ਟ propagate_vector4_event_s(ਅਚਲ vvp_vector4_t&that) : val(that) {
	    net = ਨਿਰਾ;
	    ਅਗਲਾ_2 = ਨਿਰਾ;
      }
	/* A constructor that makes the val directly. */
      propagate_vector4_event_s(ਅਚਲ vvp_vector4_t&that, ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
      : val(that,adr,wid) {
	    net = ਨਿਰਾ;
	    ਅਗਲਾ_2 = ਨਿਰਾ;
      }

	/* Propagate the output of this net. */
      vvp_net_t*net;
	/* value to propagate */
      vvp_vector4_t val;
	/* Action */
      ਖਾਲੀ ਲੜੀਵਾਰ_ਲੜੀਵਾਰ(ਖਾਲੀ) override;
      ਖਾਲੀ single_ਕਦਮ_display(ਖਾਲੀ) override;
};

ਖਾਲੀ propagate_vector4_event_s::ਲੜੀਵਾਰ_ਲੜੀਵਾਰ(ਖਾਲੀ)
{
      net->send_vec4(val, 0);
}

ਖਾਲੀ propagate_vector4_event_s::single_ਕਦਮ_display(ਖਾਲੀ)
{
      cerr << "propagate_vector4_event: Propagate val=" << val << ਸਤਰ_ਅੰਤ;
}

/*
 * This class supports the propagation of real outputs from a
 * vvp_net_t object.
 */
ਬਣਤਰ propagate_real_event_s : ਜਨਤਕ event_s {
	/* Propagate the output of this net. */
      vvp_net_t*net;
	/* value to propagate */
      ਦੁੱਗਣਾ val;
	/* Action */
      ਖਾਲੀ ਲੜੀਵਾਰ_ਲੜੀਵਾਰ(ਖਾਲੀ) override;
      ਖਾਲੀ single_ਕਦਮ_display(ਖਾਲੀ) override;
};

ਖਾਲੀ propagate_real_event_s::ਲੜੀਵਾਰ_ਲੜੀਵਾਰ(ਖਾਲੀ)
{
      net->send_real(val, 0);
}

ਖਾਲੀ propagate_real_event_s::single_ਕਦਮ_display(ਖਾਲੀ)
{
      cerr << "propagate_real_event: Propagate val=" << val << ਸਤਰ_ਅੰਤ;
}

ਬਣਤਰ ਸੌਂਪੋ_array_r_ਸ਼ਬਦ_s  : ਜਨਤਕ event_s {
      vvp_array_t mem;
      ਬਿਨਾਨਿਸ਼ਾਨੀ adr;
      ਦੁੱਗਣਾ val;
      ਖਾਲੀ ਲੜੀਵਾਰ_ਲੜੀਵਾਰ(ਖਾਲੀ) override;

      ਸਥਿਰ ਖਾਲੀ* ਸੰਚਾਲਕ ਨਵਾਂ(size_t);
      ਸਥਿਰ ਖਾਲੀ ਸੰਚਾਲਕ ਮਿਟਾਓ(ਖਾਲੀ*);
};

ਖਾਲੀ ਸੌਂਪੋ_array_r_ਸ਼ਬਦ_s::ਲੜੀਵਾਰ_ਲੜੀਵਾਰ(ਖਾਲੀ)
{
      ਗਿਣਤੀ_ਸੌਂਪੋ_events += 1;
      mem->ਸੈੱਟ_ਸ਼ਬਦ(adr, val);
}
ਸਥਿਰ ਅਚਲ size_t ARRAY_R_W_CHUNK_ਗਿਣਤੀ = 8192 / ਆਕਾਰ(ਬਣਤਰ ਸੌਂਪੋ_array_r_ਸ਼ਬਦ_s);
ਸਥਿਰ slab_t<ਆਕਾਰ(ਸੌਂਪੋ_array_r_ਸ਼ਬਦ_s),ARRAY_R_W_CHUNK_ਗਿਣਤੀ> array_r_w_ਢੇਰ;

ਸਤਰਅੰਦਰ ਖਾਲੀ* ਸੌਂਪੋ_array_r_ਸ਼ਬਦ_s::ਸੰਚਾਲਕ ਨਵਾਂ(size_t size)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(size == ਆਕਾਰ(ਸੌਂਪੋ_array_r_ਸ਼ਬਦ_s));
      ਵਾਪਸ array_r_w_ਢੇਰ.alloc_slab();
}

ਖਾਲੀ ਸੌਂਪੋ_array_r_ਸ਼ਬਦ_s::ਸੰਚਾਲਕ ਮਿਟਾਓ(ਖਾਲੀ*ptr)
{
      array_r_w_ਢੇਰ.free_slab(ptr);
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਗਿਣਤੀ_ਸੌਂਪੋ_arword_pool(ਖਾਲੀ) { ਵਾਪਸ array_r_w_ਢੇਰ.pool; }

ਬਣਤਰ generic_event_s : ਜਨਤਕ event_s {
      vvp_gen_event_t obj;
      ਤਾਰਕਿਕ delete_obj_when_ਪੂਰਾ;
      ਖਾਲੀ ਲੜੀਵਾਰ_ਲੜੀਵਾਰ(ਖਾਲੀ) override;
      ਖਾਲੀ single_ਕਦਮ_display(ਖਾਲੀ) override;

      ਸਥਿਰ ਖਾਲੀ* ਸੰਚਾਲਕ ਨਵਾਂ(size_t);
      ਸਥਿਰ ਖਾਲੀ ਸੰਚਾਲਕ ਮਿਟਾਓ(ਖਾਲੀ*);
};

ਖਾਲੀ generic_event_s::ਲੜੀਵਾਰ_ਲੜੀਵਾਰ(ਖਾਲੀ)
{
      ਗਿਣਤੀ_gen_events += 1;
      ਜੇ (obj) {
	    obj->ਲੜੀਵਾਰ_ਲੜੀਵਾਰ();
	    ਜੇ (delete_obj_when_ਪੂਰਾ)
		  ਮਿਟਾਓ obj;
      }
}

ਖਾਲੀ generic_event_s::single_ਕਦਮ_display(ਖਾਲੀ)
{
      obj->single_ਕਦਮ_display();
}

ਸਥਿਰ ਅਚਲ size_t GENERIC_CHUNK_ਗਿਣਤੀ = 131072 / ਆਕਾਰ(ਬਣਤਰ generic_event_s);
ਸਥਿਰ slab_t<ਆਕਾਰ(generic_event_s),GENERIC_CHUNK_ਗਿਣਤੀ> generic_event_ਢੇਰ;

ਸਤਰਅੰਦਰ ਖਾਲੀ* generic_event_s::ਸੰਚਾਲਕ ਨਵਾਂ(size_t size)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(size == ਆਕਾਰ(generic_event_s));
      ਵਾਪਸ generic_event_ਢੇਰ.alloc_slab();
}

ਖਾਲੀ generic_event_s::ਸੰਚਾਲਕ ਮਿਟਾਓ(ਖਾਲੀ*ptr)
{
      generic_event_ਢੇਰ.free_slab(ptr);
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਗਿਣਤੀ_gen_pool(ਖਾਲੀ) { ਵਾਪਸ generic_event_ਢੇਰ.pool; }

/*
** These event_time_s will be required a lot, at high frequency.
** Once allocated, we never free them, but stash them away for next time.
*/


ਸਥਿਰ ਅਚਲ size_t ਸਮਾਂ_CHUNK_ਗਿਣਤੀ = 8192 / ਆਕਾਰ(ਬਣਤਰ event_ਸਮਾਂ_s);
ਸਥਿਰ slab_t<ਆਕਾਰ(event_ਸਮਾਂ_s),ਸਮਾਂ_CHUNK_ਗਿਣਤੀ> event_ਸਮਾਂ_ਢੇਰ;

ਸਤਰਅੰਦਰ ਖਾਲੀ* event_ਸਮਾਂ_s::ਸੰਚਾਲਕ ਨਵਾਂ (size_t size)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(size == ਆਕਾਰ(ਬਣਤਰ event_ਸਮਾਂ_s));
      ਖਾਲੀ*ptr = event_ਸਮਾਂ_ਢੇਰ.alloc_slab();
      ਵਾਪਸ ptr;
}

ਸਤਰਅੰਦਰ ਖਾਲੀ event_ਸਮਾਂ_s::ਸੰਚਾਲਕ ਮਿਟਾਓ(ਖਾਲੀ*ptr, size_t)
{
      event_ਸਮਾਂ_ਢੇਰ.free_slab(ptr);
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਗਿਣਤੀ_ਸਮਾਂ_pool(ਖਾਲੀ) { ਵਾਪਸ event_ਸਮਾਂ_ਢੇਰ.pool; }

/*
 * This is the head of the list of pending events. This includes all
 * the events that have not been executed yet, and reaches into the
 * future.
 */
ਸਥਿਰ ਬਣਤਰ event_ਸਮਾਂ_s* sched_list = 0;

/*
 * This is a list of initialization events. The setup puts
 * initializations in this list so that they happen before the
 * simulation as a whole starts. This prevents time-0 triggers of
 * certain events.
 */
ਸਥਿਰ ਬਣਤਰ event_s* ਸਮਾਂ_ਸਾਰਣੀ_init_list = 0;

/*
 * This is the head of the list of final events.
 */
ਸਥਿਰ ਬਣਤਰ event_s* ਸਮਾਂ_ਸਾਰਣੀ_final_list = 0;

/*
 * This flag is true until a VPI task or function finishes the
 * simulation.
 */
ਸਥਿਰ ਤਾਰਕਿਕ ਸਮਾਂ_ਸਾਰਣੀ_runnable = ਸੱਚ;
ਸਥਿਰ ਤਾਰਕਿਕ ਸਮਾਂ_ਸਾਰਣੀ_stopped_flag  = ਝੂਠ;
ਸਥਿਰ ਤਾਰਕਿਕ ਸਮਾਂ_ਸਾਰਣੀ_single_ਕਦਮ_flag = ਝੂਠ;
ਸਥਿਰ ਤਾਰਕਿਕ no_ਸੰਕੇਤ_flag = ਝੂਠ;

ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_ਅੰਤ(ਪੂਰਨ)
{
      ਸਮਾਂ_ਸਾਰਣੀ_runnable = ਝੂਠ;
}

ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_stop(ਪੂਰਨ)
{
      ਸਮਾਂ_ਸਾਰਣੀ_stopped_flag = ਸੱਚ;
}

ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_single_ਕਦਮ(ਪੂਰਨ)
{
      ਸਮਾਂ_ਸਾਰਣੀ_single_ਕਦਮ_flag = ਸੱਚ;
}

ਤਾਰਕਿਕ ਸਮਾਂ_ਸਾਰਣੀ_ਮੁਕੰਮਲ(ਖਾਲੀ)
{
      ਵਾਪਸ !ਸਮਾਂ_ਸਾਰਣੀ_runnable;
}

ਤਾਰਕਿਕ ਸਮਾਂ_ਸਾਰਣੀ_stopped(ਖਾਲੀ)
{
      ਵਾਪਸ ਸਮਾਂ_ਸਾਰਣੀ_stopped_flag;
}

ਬਾਹਰੀ "C" ਖਾਲੀ vvp_no_ਸੰਕੇਤ(ਖਾਲੀ)
{
      no_ਸੰਕੇਤ_flag = ਸੱਚ;
}

/*
 * These are the signal handling infrastructure. The SIGINT signal
 * leads to an implicit $stop. The SIGHUP and SIGTERM signals lead
 * to an implicit $finish.
 */
ਬਾਹਰੀ ਤਾਰਕਿਕ stop_is_ਅੰਤ;

ਬਾਹਰੀ "C" ਖਾਲੀ ਸੰਕੇਤ_handler(ਪੂਰਨ signum)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __MINGW32__
	// Windows implements the original UNIX semantics for signal,
	// so we have to re-establish the signal handler each time a
	// signal is caught.
      ਸੰਕੇਤ(signum, &ਸੰਕੇਤ_handler);
#ਜੇ_ਅੰਤ
      ਜੇ (signum != SIGINT)
	    stop_is_ਅੰਤ = ਸੱਚ;
      ਸਮਾਂ_ਸਾਰਣੀ_stopped_flag = ਸੱਚ;
}

ਸਥਿਰ ਖਾਲੀ ਸੰਕੇਤ_capture(ਖਾਲੀ)
{
      ਜੇ (no_ਸੰਕੇਤ_flag)
	   ਵਾਪਸ;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __MINGW32__
      ਸੰਕੇਤ(SIGHUP,  &ਸੰਕੇਤ_handler);
#ਜੇ_ਅੰਤ
      ਸੰਕੇਤ(SIGINT,  &ਸੰਕੇਤ_handler);
      ਸੰਕੇਤ(SIGTERM, &ਸੰਕੇਤ_handler);
}

ਸਥਿਰ ਖਾਲੀ ਸੰਕੇਤ_revert(ਖਾਲੀ)
{
      ਜੇ (no_ਸੰਕੇਤ_flag)
	   ਵਾਪਸ;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __MINGW32__
      ਸੰਕੇਤ(SIGHUP,  SIG_DFL);
#ਜੇ_ਅੰਤ
      ਸੰਕੇਤ(SIGINT,  SIG_DFL);
      ਸੰਕੇਤ(SIGTERM, SIG_DFL);
}

/*
 * This function puts an event on the end of the pre-simulation event queue.
 */
ਸਥਿਰ ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_init_event(ਬਣਤਰ event_s*cur)
{
      ਜੇ (ਸਮਾਂ_ਸਾਰਣੀ_init_list == 0) {
            cur->ਅਗਲਾ_2 = cur;
      } ਨਹੀਂਤਾਂ {
            cur->ਅਗਲਾ_2 = ਸਮਾਂ_ਸਾਰਣੀ_init_list->ਅਗਲਾ_2;
            ਸਮਾਂ_ਸਾਰਣੀ_init_list->ਅਗਲਾ_2 = cur;
      }
      ਸਮਾਂ_ਸਾਰਣੀ_init_list = cur;
}

/*
 * This function puts an event on the end of the post-simulation event queue.
 */
ਸਥਿਰ ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_final_event(ਬਣਤਰ event_s*cur)
{
      ਜੇ (ਸਮਾਂ_ਸਾਰਣੀ_final_list == 0) {
            cur->ਅਗਲਾ_2 = cur;
      } ਨਹੀਂਤਾਂ {
            cur->ਅਗਲਾ_2 = ਸਮਾਂ_ਸਾਰਣੀ_final_list->ਅਗਲਾ_2;
            ਸਮਾਂ_ਸਾਰਣੀ_final_list->ਅਗਲਾ_2 = cur;
      }
      ਸਮਾਂ_ਸਾਰਣੀ_final_list = cur;
}

/*
 * This function does all the hard work of putting an event into the
 * event queue. The event delay is taken from the event structure
 * itself, and the structure is placed in the right place in the
 * queue.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ event_ਕਤਾਰ_e { SEQ_ਸ਼ੁਰੂ, SEQ_ਸਰਗਰਮ, SEQ_INACTIVE, SEQ_NBASSIGN,
			     SEQ_RWSYNC, SEQ_ROSYNC, DEL_THREAD } event_ਕਤਾਰ_t;

ਸਥਿਰ ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_event(ਬਣਤਰ event_s*cur, vvp_time64_t delay,
			    event_ਕਤਾਰ_t select_ਕਤਾਰ)
{
      cur->ਅਗਲਾ_2 = cur;
      ਬਣਤਰ event_ਸਮਾਂ_s*ctim = sched_list;

      ਜੇ (sched_list == 0) {
	      /* Is the event_time list completely empty? Create the
		 first event_time object. */
	    ctim = ਨਵਾਂ ਬਣਤਰ event_ਸਮਾਂ_s;
	    ctim->delay = delay;
	    ctim->ਅਗਲਾ_2  = 0;
	    sched_list = ctim;

      } ਨਹੀਂਤਾਂ ਜੇ (sched_list->delay > delay) {

	      /* Am I looking for an event before the first event_time?
		 If so, create a new event_time to go in front. */
	    ਬਣਤਰ event_ਸਮਾਂ_s*tmp = ਨਵਾਂ ਬਣਤਰ event_ਸਮਾਂ_s;
	    tmp->delay = delay;
	    tmp->ਅਗਲਾ_2 = ctim;
	    ctim->delay -= delay;
	    ctim = tmp;
	    sched_list = ctim;

      } ਨਹੀਂਤਾਂ {
	    ਬਣਤਰ event_ਸਮਾਂ_s*prev = 0;

	    ਜਦੋਂ (ctim->ਅਗਲਾ_2 && (ctim->delay < delay)) {
		  delay -= ctim->delay;
		  prev = ctim;
		  ctim = ctim->ਅਗਲਾ_2;
	    }

	    ਜੇ (ctim->delay > delay) {
		  ਬਣਤਰ event_ਸਮਾਂ_s*tmp = ਨਵਾਂ ਬਣਤਰ event_ਸਮਾਂ_s;
		  tmp->delay = delay;
		  tmp->ਅਗਲਾ_2  = prev->ਅਗਲਾ_2;
		  prev->ਅਗਲਾ_2 = tmp;

		  tmp->ਅਗਲਾ_2->delay -= delay;
		  ctim = tmp;

	    } ਨਹੀਂਤਾਂ ਜੇ (ctim->delay == delay) {

	    } ਨਹੀਂਤਾਂ {
		  ਪ੍ਰਤਿਜ੍ਞਾ(ctim->ਅਗਲਾ_2 == 0);
		  ਬਣਤਰ event_ਸਮਾਂ_s*tmp = ਨਵਾਂ ਬਣਤਰ event_ਸਮਾਂ_s;
		  tmp->delay = delay - ctim->delay;
		  tmp->ਅਗਲਾ_2 = 0;
		  ctim->ਅਗਲਾ_2 = tmp;

		  ctim = tmp;
	    }
      }

	/* By this point, ctim is the event_time structure that is to
	   receive the event at hand. Put the event in to the
	   appropriate list for the kind of assign we have at hand. */

      ਬਣਤਰ event_s** q = 0;

      ਚੋਣ (select_ਕਤਾਰ) {
	  ਹਾਲਤ SEQ_ਸ਼ੁਰੂ:
	    q = &ctim->ਸ਼ੁਰੂ;
	    ਰੋਕੋ;

	  ਹਾਲਤ SEQ_ਸਰਗਰਮ:
	    q = &ctim->ਸਰਗਰਮ;
	    ਰੋਕੋ;

	  ਹਾਲਤ SEQ_INACTIVE:
	    ਪ੍ਰਤਿਜ੍ਞਾ(delay == 0);
	    q = &ctim->inactive;
	    ਰੋਕੋ;

	  ਹਾਲਤ SEQ_NBASSIGN:
	    q = &ctim->nbassign;
	    ਰੋਕੋ;

	  ਹਾਲਤ SEQ_RWSYNC:
	    q = &ctim->rwsync;
	    ਰੋਕੋ;

	  ਹਾਲਤ SEQ_ROSYNC:
	    q = &ctim->rosync;
	    ਰੋਕੋ;

	  ਹਾਲਤ DEL_THREAD:
	    q = &ctim->del_thr;
	    ਰੋਕੋ;
      }

      ਜੇ (q) {
	    ਜੇ (*q) {
		  /* Put the cur event on the end of the queue. */
		  cur->ਅਗਲਾ_2 = (*q)->ਅਗਲਾ_2;
		  (*q)->ਅਗਲਾ_2 = cur;
	    }
	    *q = cur;
      }
}

ਸਥਿਰ ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_event_ਧੱਕੋ(ਬਣਤਰ event_s*cur)
{
      ਜੇ ((sched_list == 0) || (sched_list->delay > 0)) {
	    ਸਮਾਂ_ਸਾਰਣੀ_event(cur, 0, SEQ_ਸਰਗਰਮ);
	    ਵਾਪਸ;
      }

      ਬਣਤਰ event_ਸਮਾਂ_s*ctim = sched_list;

      ਜੇ (ctim->ਸਰਗਰਮ == 0) {
	    cur->ਅਗਲਾ_2 = cur;
	    ctim->ਸਰਗਰਮ = cur;
	    ਵਾਪਸ;
      }

      cur->ਅਗਲਾ_2 = ctim->ਸਰਗਰਮ->ਅਗਲਾ_2;
      ctim->ਸਰਗਰਮ->ਅਗਲਾ_2 = cur;
}

ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_vthread(vthread_t thr, vvp_time64_t delay, ਤਾਰਕਿਕ ਧੱਕੋ_flag)
{
      ਬਣਤਰ vthread_event_s*cur = ਨਵਾਂ vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);

      ਜੇ (ਧੱਕੋ_flag && (delay == 0)) {
	      /* Special case: If the delay is 0, the push_flag means
		 I can push this event in front of everything. This is
		 used by the %fork statement, for example, to perform
		 task calls. */
	    ਸਮਾਂ_ਸਾਰਣੀ_event_ਧੱਕੋ(cur);

      } ਨਹੀਂਤਾਂ {
	    ਸਮਾਂ_ਸਾਰਣੀ_event(cur, delay, SEQ_ਸਰਗਰਮ);
      }
}

ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_t0_trigger(vvp_net_ptr_t ptr)
{
      vvp_vector4_t ਦੋਆਧਾਰੀ_ਅੰਕ (1, BIT4_X);
      ਬਣਤਰ ਸੌਂਪੋ_vector4_event_s*cur = ਨਵਾਂ ਬਣਤਰ ਸੌਂਪੋ_vector4_event_s(ਦੋਆਧਾਰੀ_ਅੰਕ);
      cur->ptr = ptr;
      ਸਮਾਂ_ਸਾਰਣੀ_event(cur, 0, SEQ_INACTIVE);
}

ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_inactive(vthread_t thr)
{
      ਬਣਤਰ vthread_event_s*cur = ਨਵਾਂ vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);
      ਸਮਾਂ_ਸਾਰਣੀ_event(cur, 0, SEQ_INACTIVE);
}

ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_init_vthread(vthread_t thr)
{
      ਬਣਤਰ vthread_event_s*cur = ਨਵਾਂ vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);

      ਸਮਾਂ_ਸਾਰਣੀ_init_event(cur);
}

ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_final_vthread(vthread_t thr)
{
      ਬਣਤਰ vthread_event_s*cur = ਨਵਾਂ vthread_event_s;

      cur->thr = thr;
      vthread_mark_final(thr);
      vthread_mark_scheduled(thr);

      ਸਮਾਂ_ਸਾਰਣੀ_final_event(cur);
}

ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_ਸੌਂਪੋ_ਸਦਿਸ਼(vvp_net_ptr_t ptr,
			    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid,
			    ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			    vvp_time64_t delay)
{
      ਬਣਤਰ ਸੌਂਪੋ_vector4_event_s*cur = ਨਵਾਂ ਬਣਤਰ ਸੌਂਪੋ_vector4_event_s(ਦੋਆਧਾਰੀ_ਅੰਕ);
      cur->ptr = ptr;
      cur->ਅਧਾਰ = ਅਧਾਰ;
      cur->vwid = vwid;
      ਸਮਾਂ_ਸਾਰਣੀ_event(cur, delay, SEQ_NBASSIGN);
}

ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_force_ਸਦਿਸ਼(vvp_net_t*net,
			    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid,
			    ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			    vvp_time64_t delay)
{
      ਬਣਤਰ force_vector4_event_s*cur = ਨਵਾਂ ਬਣਤਰ force_vector4_event_s(ਦੋਆਧਾਰੀ_ਅੰਕ);
      cur->net = net;
      cur->ਅਧਾਰ = ਅਧਾਰ;
      cur->vwid = vwid;
      ਸਮਾਂ_ਸਾਰਣੀ_event(cur, delay, SEQ_NBASSIGN);
}

ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_propagate_ਸਦਿਸ਼(vvp_net_t*net,
			       vvp_time64_t delay,
			       ਅਚਲ vvp_vector4_t&src)
{
      ਬਣਤਰ propagate_vector4_event_s*cur
	    = ਨਵਾਂ ਬਣਤਰ propagate_vector4_event_s(src);
      cur->net = net;
      ਸਮਾਂ_ਸਾਰਣੀ_event(cur, delay, SEQ_NBASSIGN);
}

// FIXME: This needs to create a non-blocking event, but only one per time slot.
//        Is schedule_event_ or execution actually filtering since the net is
//        already X because this is not triggering?
ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_propagate_event(vvp_net_t*net,
                              vvp_time64_t delay)
{
      vvp_vector4_t tmp (1, BIT4_X);
      ਬਣਤਰ propagate_vector4_event_s*cur
	    = ਨਵਾਂ ਬਣਤਰ propagate_vector4_event_s(tmp);
      cur->net = net;
      ਸਮਾਂ_ਸਾਰਣੀ_event(cur, delay, SEQ_NBASSIGN);
}

ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_ਸੌਂਪੋ_array_ਸ਼ਬਦ(vvp_array_t mem,
				ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ_addr,
				ਬਿਨਾਨਿਸ਼ਾਨੀ off,
				ਅਚਲ vvp_vector4_t&val,
				vvp_time64_t delay)
{
      ਬਣਤਰ ਸੌਂਪੋ_array_ਸ਼ਬਦ_s*cur = ਨਵਾਂ ਬਣਤਰ ਸੌਂਪੋ_array_ਸ਼ਬਦ_s;
      cur->mem = mem;
      cur->adr = ਸ਼ਬਦ_addr;
      cur->off = off;
      cur->val = val;
      ਸਮਾਂ_ਸਾਰਣੀ_event(cur, delay, SEQ_NBASSIGN);
}

ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_ਸੌਂਪੋ_array_ਸ਼ਬਦ(vvp_array_t mem,
				ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ_addr,
				ਦੁੱਗਣਾ val,
				vvp_time64_t delay)
{
      ਬਣਤਰ ਸੌਂਪੋ_array_r_ਸ਼ਬਦ_s*cur = ਨਵਾਂ ਬਣਤਰ ਸੌਂਪੋ_array_r_ਸ਼ਬਦ_s;
      cur->mem = mem;
      cur->adr = ਸ਼ਬਦ_addr;
      cur->val = val;
      ਸਮਾਂ_ਸਾਰਣੀ_event(cur, delay, SEQ_NBASSIGN);
}

ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_ਸੈੱਟ_ਸਦਿਸ਼(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ)
{
      ਬਣਤਰ ਸੌਂਪੋ_vector4_event_s*cur = ਨਵਾਂ ਬਣਤਰ ਸੌਂਪੋ_vector4_event_s(ਦੋਆਧਾਰੀ_ਅੰਕ);
      cur->ptr = ptr;
      cur->ਅਧਾਰ = 0;
      cur->vwid = 0;
      ਸਮਾਂ_ਸਾਰਣੀ_event(cur, 0, SEQ_ਸਰਗਰਮ);
}

ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_ਸੈੱਟ_ਸਦਿਸ਼(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ)
{
      ਬਣਤਰ ਸੌਂਪੋ_vector8_event_s*cur = ਨਵਾਂ ਬਣਤਰ ਸੌਂਪੋ_vector8_event_s;
      cur->ptr = ptr;
      cur->val = ਦੋਆਧਾਰੀ_ਅੰਕ;
      ਸਮਾਂ_ਸਾਰਣੀ_event(cur, 0, SEQ_ਸਰਗਰਮ);
}

ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_ਸੈੱਟ_ਸਦਿਸ਼(vvp_net_ptr_t ptr, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ)
{
      ਬਣਤਰ ਸੌਂਪੋ_real_event_s*cur = ਨਵਾਂ ਬਣਤਰ ਸੌਂਪੋ_real_event_s;
      cur->ptr = ptr;
      cur->val = ਦੋਆਧਾਰੀ_ਅੰਕ;
      ਸਮਾਂ_ਸਾਰਣੀ_event(cur, 0, SEQ_ਸਰਗਰਮ);
}

ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_init_ਸਦਿਸ਼(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ)
{
      ਬਣਤਰ ਸੌਂਪੋ_vector4_event_s*cur = ਨਵਾਂ ਬਣਤਰ ਸੌਂਪੋ_vector4_event_s(ਦੋਆਧਾਰੀ_ਅੰਕ);
      cur->ptr = ptr;
      cur->ਅਧਾਰ = 0;
      cur->vwid = 0;
      ਸਮਾਂ_ਸਾਰਣੀ_init_event(cur);
}

ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_init_ਸਦਿਸ਼(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ)
{
      ਬਣਤਰ ਸੌਂਪੋ_vector8_event_s*cur = ਨਵਾਂ ਬਣਤਰ ਸੌਂਪੋ_vector8_event_s;
      cur->ptr = ptr;
      cur->val = ਦੋਆਧਾਰੀ_ਅੰਕ;
      ਸਮਾਂ_ਸਾਰਣੀ_init_event(cur);
}

ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_init_ਸਦਿਸ਼(vvp_net_ptr_t ptr, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ)
{
      ਬਣਤਰ ਸੌਂਪੋ_real_event_s*cur = ਨਵਾਂ ਬਣਤਰ ਸੌਂਪੋ_real_event_s;
      cur->ptr = ptr;
      cur->val = ਦੋਆਧਾਰੀ_ਅੰਕ;
      ਸਮਾਂ_ਸਾਰਣੀ_init_event(cur);
}

ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_init_propagate(vvp_net_t*net, vvp_vector4_t ਦੋਆਧਾਰੀ_ਅੰਕ)
{
      ਬਣਤਰ propagate_vector4_event_s*cur = ਨਵਾਂ ਬਣਤਰ propagate_vector4_event_s(ਦੋਆਧਾਰੀ_ਅੰਕ);
      cur->net = net;
      ਸਮਾਂ_ਸਾਰਣੀ_init_event(cur);
}

ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_init_propagate(vvp_net_t*net, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ)
{
      ਬਣਤਰ propagate_real_event_s*cur = ਨਵਾਂ ਬਣਤਰ propagate_real_event_s;
      cur->net = net;
      cur->val = ਦੋਆਧਾਰੀ_ਅੰਕ;
      ਸਮਾਂ_ਸਾਰਣੀ_init_event(cur);
}

ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_del_thr(vthread_t thr)
{
      ਬਣਤਰ del_thr_event_s*cur = ਨਵਾਂ del_thr_event_s;

      cur->thr = thr;

      ਸਮਾਂ_ਸਾਰਣੀ_event(cur, 0, DEL_THREAD);
}

ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_generic(vvp_gen_event_t obj, vvp_time64_t delay,
		      ਤਾਰਕਿਕ sync_flag, ਤਾਰਕਿਕ ro_flag, ਤਾਰਕਿਕ delete_when_ਪੂਰਾ)
{
      ਬਣਤਰ generic_event_s*cur = ਨਵਾਂ generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_ਪੂਰਾ = delete_when_ਪੂਰਾ;
      ਸਮਾਂ_ਸਾਰਣੀ_event(cur, delay,
		      sync_flag? (ro_flag?SEQ_ROSYNC:SEQ_RWSYNC) : SEQ_ਸਰਗਰਮ);

      ਜੇ (sync_flag)
	    vthread_delay_delete();
}

ਸਥਿਰ ਤਾਰਕਿਕ sim_started;

ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_functor(vvp_gen_event_t obj)
{
      ਬਣਤਰ generic_event_s*cur = ਨਵਾਂ generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_ਪੂਰਾ = ਝੂਠ;
      ਜੇ (!sim_started) {
            ਸਮਾਂ_ਸਾਰਣੀ_init_event(cur);
      } ਨਹੀਂਤਾਂ {
            ਸਮਾਂ_ਸਾਰਣੀ_event(cur, 0, SEQ_ਸਰਗਰਮ);
      }
}

ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_at_ਸ਼ੁਰੂ_ਦਾ_simtime(vvp_gen_event_t obj, vvp_time64_t delay)
{
      ਬਣਤਰ generic_event_s*cur = ਨਵਾਂ generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_ਪੂਰਾ = ਝੂਠ;
      ਸਮਾਂ_ਸਾਰਣੀ_event(cur, delay, SEQ_ਸ਼ੁਰੂ);
}

/*
 * In the vvp runtime of Icarus Verilog, the SEQ_RWSYNC time step is
 * after all of the non-blocking assignments, so is effectively the
 * same as the ReadWriteSync time.
 */
ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_at_ਸਿਰਾ_ਦਾ_simtime(vvp_gen_event_t obj, vvp_time64_t delay)
{
      ਬਣਤਰ generic_event_s*cur = ਨਵਾਂ generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_ਪੂਰਾ = ਝੂਠ;
      ਸਮਾਂ_ਸਾਰਣੀ_event(cur, delay, SEQ_RWSYNC);
}

ਸਥਿਰ vvp_time64_t ਸਮਾਂ_ਸਾਰਣੀ_ਸਮਾਂ;
vvp_time64_t ਸਮਾਂ_ਸਾਰਣੀ_simtime(ਖਾਲੀ)
{ ਵਾਪਸ ਸਮਾਂ_ਸਾਰਣੀ_ਸਮਾਂ; }

ਸਥਿਰ ਤਾਰਕਿਕ sim_at_rosync = ਝੂਠ;
ਤਾਰਕਿਕ ਸਮਾਂ_ਸਾਰਣੀ_at_rosync(ਖਾਲੀ)
{ ਵਾਪਸ sim_at_rosync; }

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
ਸਥਿਰ ਖਾਲੀ ਲੜੀਵਾਰ_rosync(ਬਣਤਰ event_ਸਮਾਂ_s*ctim)
{
      sim_at_rosync = ਸੱਚ;
      ਜਦੋਂ (ctim->rosync) {
	    ਬਣਤਰ event_s*cur = ctim->rosync->ਅਗਲਾ_2;
	    ਜੇ (cur->ਅਗਲਾ_2 == cur) {
		  ctim->rosync = 0;
	    } ਨਹੀਂਤਾਂ {
		  ctim->rosync->ਅਗਲਾ_2 = cur->ਅਗਲਾ_2;
	    }

	    cur->ਲੜੀਵਾਰ_ਲੜੀਵਾਰ();
	    ਮਿਟਾਓ cur;
      }
      sim_at_rosync = ਝੂਠ;

      ਜਦੋਂ (ctim->del_thr) {
	    ਬਣਤਰ event_s*cur = ctim->del_thr->ਅਗਲਾ_2;
	    ਜੇ (cur->ਅਗਲਾ_2 == cur) {
		  ctim->del_thr = 0;
	    } ਨਹੀਂਤਾਂ {
		  ctim->del_thr->ਅਗਲਾ_2 = cur->ਅਗਲਾ_2;
	    }

	    cur->ਲੜੀਵਾਰ_ਲੜੀਵਾਰ();
	    ਮਿਟਾਓ cur;
      }

      ਜੇ (ctim->ਸਰਗਰਮ || ctim->inactive || ctim->nbassign || ctim->rwsync) {
	    cerr << "SCHEDULER ERROR: read-only sync events "
		 << "created RW events!" << ਸਤਰ_ਅੰਤ;
      }
}

ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_simulate(ਖਾਲੀ)
{
      ਤਾਰਕਿਕ ਲੜੀਵਾਰ_finals;
      sim_started = ਝੂਠ;

      ਸਮਾਂ_ਸਾਰਣੀ_ਸਮਾਂ = 0;

      ਜੇ (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute EndOfCompile callbacks\n");
      }

      // Execute end of compile callbacks
      vpi_ਸਿਰਾ_ਦਾ_Compile();

      ਜੇ (verbose_flag) {
	    vpi_mcd_printf(1, " ...propagate initialization events\n");
      }

	// Execute initialization events.
      ਜਦੋਂ (ਸਮਾਂ_ਸਾਰਣੀ_init_list) {
	    ਬਣਤਰ event_s*cur = ਸਮਾਂ_ਸਾਰਣੀ_init_list->ਅਗਲਾ_2;
	    ਜੇ (cur->ਅਗਲਾ_2 == cur) {
		  ਸਮਾਂ_ਸਾਰਣੀ_init_list = 0;
	    } ਨਹੀਂਤਾਂ {
		  ਸਮਾਂ_ਸਾਰਣੀ_init_list->ਅਗਲਾ_2 = cur->ਅਗਲਾ_2;
	    }
	    cur->ਲੜੀਵਾਰ_ਲੜੀਵਾਰ();
	    ਮਿਟਾਓ cur;
      }

      sim_started = ਸੱਚ;

      ਜੇ (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute StartOfSim callbacks\n");
      }

      // Execute start of simulation callbacks
      vpi_ਸ਼ੁਰੂ_ਦਾ_Sim();

      ਸੰਕੇਤ_capture();

      ਜੇ (verbose_flag) {
	    vpi_mcd_printf(1, " ...run scheduler\n");
      }

      // If there were no compiletf, etc. errors then we are going to
      // process events and when done run the final blocks.
      ਲੜੀਵਾਰ_finals = ਸਮਾਂ_ਸਾਰਣੀ_runnable;

      ਜੇ (ਸਮਾਂ_ਸਾਰਣੀ_runnable) ਜਦੋਂ (sched_list) {

	    ਜੇ (ਸਮਾਂ_ਸਾਰਣੀ_stopped_flag) {
		  ਸਮਾਂ_ਸਾਰਣੀ_stopped_flag = ਝੂਠ;
		  stop_handler(0);
		  // You can finish from the debugger without a time change.
		  ਜੇ (!ਸਮਾਂ_ਸਾਰਣੀ_runnable) ਰੋਕੋ;
		  ਜਾਰੀ;
	    }

	      /* ctim is the current time step. */
	    ਬਣਤਰ event_ਸਮਾਂ_s* ctim = sched_list;

	      /* If the time is advancing, then first run the
		 postponed sync events. Run them all. */
	    ਜੇ (ctim->delay > 0) {

		  ਜੇ (!ਸਮਾਂ_ਸਾਰਣੀ_runnable) ਰੋਕੋ;
		  ਸਮਾਂ_ਸਾਰਣੀ_ਸਮਾਂ += ctim->delay;
		    /* When the design is being traced (we are emitting
		     * file/line information) also print any time changes. */
		  ਜੇ (show_file_ਸਤਰ) {
			cerr << "Advancing to simulation time: "
			     << ਸਮਾਂ_ਸਾਰਣੀ_ਸਮਾਂ << ਸਤਰ_ਅੰਤ;
		  }
		  ctim->delay = 0;

		  vpi_ਅਗਲਾ_Sim_ਸਮਾਂ();
		    // Process the cbAtStartOfSimTime callbacks.
		  ਜਦੋਂ (ctim->ਸ਼ੁਰੂ) {
			ਬਣਤਰ event_s*cur = ctim->ਸ਼ੁਰੂ->ਅਗਲਾ_2;
			ਜੇ (cur->ਅਗਲਾ_2 == cur) {
			      ctim->ਸ਼ੁਰੂ = 0;
			} ਨਹੀਂਤਾਂ {
			      ctim->ਸ਼ੁਰੂ->ਅਗਲਾ_2 = cur->ਅਗਲਾ_2;
			}
			cur->ਲੜੀਵਾਰ_ਲੜੀਵਾਰ();
			ਮਿਟਾਓ (cur);
		  }
	    }


	      /* If there are no more active events, advance the event
		 queues. If there are not events at all, then release
		 the event_time object. */
	    ਜੇ (ctim->ਸਰਗਰਮ == 0) {
		  ctim->ਸਰਗਰਮ = ctim->inactive;
		  ctim->inactive = 0;

		  ਜੇ (ctim->ਸਰਗਰਮ == 0) {
			ctim->ਸਰਗਰਮ = ctim->nbassign;
			ctim->nbassign = 0;

			ਜੇ (ctim->ਸਰਗਰਮ == 0) {
			      ctim->ਸਰਗਰਮ = ctim->rwsync;
			      ctim->rwsync = 0;

				/* If out of rw events, then run the rosync
				   events and delete this time step. This also
				   deletes threads as needed. */
			      ਜੇ (ctim->ਸਰਗਰਮ == 0) {
				    ਲੜੀਵਾਰ_rosync(ctim);
				    sched_list = ctim->ਅਗਲਾ_2;
				    ਮਿਟਾਓ ctim;
				    ਜਾਰੀ;
			      }
			}
		  }
	    }

	      /* Pull the first item off the list. If this is the last
		 cell in the list, then clear the list. Execute that
		 event type, and delete it. */
	    ਬਣਤਰ event_s*cur = ctim->ਸਰਗਰਮ->ਅਗਲਾ_2;
	    ਜੇ (cur->ਅਗਲਾ_2 == cur) {
		  ctim->ਸਰਗਰਮ = 0;
	    } ਨਹੀਂਤਾਂ {
		  ctim->ਸਰਗਰਮ->ਅਗਲਾ_2 = cur->ਅਗਲਾ_2;
	    }

	    ਜੇ (ਸਮਾਂ_ਸਾਰਣੀ_single_ਕਦਮ_flag) {
		  cur->single_ਕਦਮ_display();
		  ਸਮਾਂ_ਸਾਰਣੀ_stopped_flag = ਸੱਚ;
		  ਸਮਾਂ_ਸਾਰਣੀ_single_ਕਦਮ_flag = ਝੂਠ;
	    }

	    cur->ਲੜੀਵਾਰ_ਲੜੀਵਾਰ();

	    ਮਿਟਾਓ (cur);
      }

	// Execute final events.
      ਸਮਾਂ_ਸਾਰਣੀ_runnable = ਲੜੀਵਾਰ_finals;
      ਜਦੋਂ (ਸਮਾਂ_ਸਾਰਣੀ_runnable && ਸਮਾਂ_ਸਾਰਣੀ_final_list) {
	    ਬਣਤਰ event_s*cur = ਸਮਾਂ_ਸਾਰਣੀ_final_list->ਅਗਲਾ_2;
	    ਜੇ (cur->ਅਗਲਾ_2 == cur) {
		  ਸਮਾਂ_ਸਾਰਣੀ_final_list = 0;
	    } ਨਹੀਂਤਾਂ {
		  ਸਮਾਂ_ਸਾਰਣੀ_final_list->ਅਗਲਾ_2 = cur->ਅਗਲਾ_2;
	    }
	    cur->ਲੜੀਵਾਰ_ਲੜੀਵਾਰ();
	    ਮਿਟਾਓ cur;
      }

      ਸੰਕੇਤ_revert();

      ਜੇ (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute Postsim callbacks\n");
      }

      // Execute post-simulation callbacks
      vpiPostsim();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
      ਸਮਾਂ_ਸਾਰਣੀ_delete();
#ਜੇ_ਅੰਤ
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_delete(ਖਾਲੀ)
{
      vthread_event_ਢੇਰ.delete_pool();
      assign4_ਢੇਰ.delete_pool();
      assign8_ਢੇਰ.delete_pool();
      assignr_ਢੇਰ.delete_pool();
      array_w_ਢੇਰ.delete_pool();
      array_r_w_ਢੇਰ.delete_pool();
      generic_event_ਢੇਰ.delete_pool();
      event_ਸਮਾਂ_ਢੇਰ.delete_pool();
}
#ਜੇ_ਅੰਤ
