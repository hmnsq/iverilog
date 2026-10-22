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
#که_تعریف_شوی CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
# include  "ivl_alloc.h"
#د_که_پای

کارول نطاق std;

بینښه اوږد شمېر_ټاکل_events = 0;
بینښه اوږد شمېر_gen_events = 0;
بینښه اوږد شمېر_thread_events = 0;
  // Count the time events (A time cell created)
بینښه اوږد شمېر_وخت_events = 0;



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
      بنية event_s*بل_2;
      مجازي ~event_s() { }
      مجازي فراغ پرلپسې_پرلپسې(فراغ) =0;

	// Write something about the event to stderr
      مجازي فراغ single_ګام_display(فراغ);

	// Fallback new/delete
      ساكن فراغ*چلوونکی جديد (size_t size) { أرجع ::جديد حرف[size]; }
      ساكن فراغ چلوونکی ړنګول(فراغ*ptr)  { ::ړنګول[]( static_cast<حرف*>(ptr) ); }
};

فراغ event_s::single_ګام_display(فراغ)
{
      std::cerr << "event_s: Step into event " << typeid(*هذا).name() << std::کرښه_پای;
}

بنية event_وخت_s {
      event_وخت_s() {
	    شمېر_وخت_events += 1;
	    delay = 0;
	    پیل = 0;
	    فعال = 0;
	    inactive = 0;
	    nbassign = 0;
	    rwsync = 0;
	    rosync = 0;
	    del_thr = 0;
	    بل_2 = عدم;
      }
      vvp_time64_t delay;

      بنية event_s*پیل;
      بنية event_s*فعال;
      بنية event_s*inactive;
      بنية event_s*nbassign;
      بنية event_s*rwsync;
      بنية event_s*rosync;
      بنية event_s*del_thr;

      بنية event_وخت_s*بل_2;

      ساكن فراغ* چلوونکی جديد (size_t);
      ساكن فراغ چلوونکی ړنګول(فراغ*obj, size_t s);
};

vvp_gen_event_s::~vvp_gen_event_s()
{
}

فراغ vvp_gen_event_s::single_ګام_display(فراغ)
{
      cerr << "vvp_gen_event_s: Step into event " << typeid(*هذا).name() << کرښه_پای;
}

/*
 * Derived event types
 */
بنية vthread_event_s : عام event_s {
      vthread_t thr;
      فراغ پرلپسې_پرلپسې(فراغ) override;
      فراغ single_ګام_display(فراغ) override;

      ساكن فراغ* چلوونکی جديد(size_t);
      ساكن فراغ چلوونکی ړنګول(فراغ*);
};

فراغ vthread_event_s::پرلپسې_پرلپسې(فراغ)
{
      شمېر_thread_events += 1;
      vthread_پرلپسې(thr);
}

فراغ vthread_event_s::single_ګام_display(فراغ)
{
      __vpiScope*scope = vthread_scope(thr);
      cerr << "vthread_event: Resume thread"
	   << " scope=" << scope->vpi_get_str(vpi_ډک_Name)
	   << کرښه_پای;
}

ساكن ثابت size_t VTHR_CHUNK_شمېر = 8192 / حجم(بنية vthread_event_s);
ساكن slab_t<حجم(vthread_event_s),VTHR_CHUNK_شمېر> vthread_event_ډېرۍ;

مضمن فراغ* vthread_event_s::چلوونکی جديد(size_t size)
{
      أكد(size == حجم(vthread_event_s));
      أرجع vthread_event_ډېرۍ.alloc_slab();
}

فراغ vthread_event_s::چلوونکی ړنګول(فراغ*dptr)
{
      vthread_event_ډېرۍ.free_slab(dptr);
}

بنية del_thr_event_s : عام event_s {
      vthread_t thr;
      فراغ پرلپسې_پرلپسې(فراغ) override;
      فراغ single_ګام_display(فراغ) override;
};

فراغ del_thr_event_s::پرلپسې_پرلپسې(فراغ)
{
      vthread_delete(thr);
}

فراغ del_thr_event_s::single_ګام_display(فراغ)
{
      __vpiScope*scope = vthread_scope(thr);
      cerr << "del_thr_event: Reap completed thread"
	   << " scope=" << scope->vpi_get_str(vpi_ډک_Name) << کرښه_پای;
}

بنية ټاکل_vector4_event_s  : عام event_s {
	/* The default constructor. */
      څرګند ټاکل_vector4_event_s(ثابت vvp_vector4_t&that) : val(that) {
	    بنسټ = 0;
	    vwid = 0;
	    بل_2 = عدم;
      }

	/* Where to do the assign. */
      vvp_net_ptr_t ptr;
	/* Value to assign. */
      vvp_vector4_t val;
	/* Offset of the part into the destination. */
      بینښه بنسټ;
	/* Width of the destination vector. */
      بینښه vwid;
      فراغ پرلپسې_پرلپسې(فراغ) override;
      فراغ single_ګام_display(فراغ) override;

      ساكن فراغ* چلوونکی جديد(size_t);
      ساكن فراغ چلوونکی ړنګول(فراغ*);
};

فراغ ټاکل_vector4_event_s::پرلپسې_پرلپسې(فراغ)
{
      شمېر_ټاکل_events += 1;
      إذا (vwid > 0)
	    vvp_send_vec4_pv(ptr, val, بنسټ, vwid, 0);
      وإلا
	    vvp_send_vec4(ptr, val, 0);
}

فراغ ټاکل_vector4_event_s::single_ګام_display(فراغ)
{
      cerr << "assign_vector4_event: Propagate val=" << val
	   << ", vwid=" << vwid << ", base=" << بنسټ << کرښه_پای;
}

ساكن ثابت size_t ASSIGN4_CHUNK_شمېر = 524288 / حجم(بنية ټاکل_vector4_event_s);
ساكن slab_t<حجم(ټاکل_vector4_event_s),ASSIGN4_CHUNK_شمېر> assign4_ډېرۍ;

مضمن فراغ* ټاکل_vector4_event_s::چلوونکی جديد(size_t size)
{
      أكد(size == حجم(ټاکل_vector4_event_s));
      أرجع assign4_ډېرۍ.alloc_slab();
}

فراغ ټاکل_vector4_event_s::چلوونکی ړنګول(فراغ*dptr)
{
      assign4_ډېرۍ.free_slab(dptr);
}

بینښه اوږد شمېر_assign4_pool(فراغ) { أرجع assign4_ډېرۍ.pool; }

بنية ټاکل_vector8_event_s  : عام event_s {
      vvp_net_ptr_t ptr;
      vvp_vector8_t val;
      فراغ پرلپسې_پرلپسې(فراغ) override;
      فراغ single_ګام_display(فراغ) override;

      ساكن فراغ* چلوونکی جديد(size_t);
      ساكن فراغ چلوونکی ړنګول(فراغ*);
};

فراغ ټاکل_vector8_event_s::پرلپسې_پرلپسې(فراغ)
{
      شمېر_ټاکل_events += 1;
      vvp_send_vec8(ptr, val);
}

فراغ ټاکل_vector8_event_s::single_ګام_display(فراغ)
{
      cerr << "assign_vector8_event: Propagate val=" << val << کرښه_پای;
}

ساكن ثابت size_t ASSIGN8_CHUNK_شمېر = 8192 / حجم(بنية ټاکل_vector8_event_s);
ساكن slab_t<حجم(ټاکل_vector8_event_s),ASSIGN8_CHUNK_شمېر> assign8_ډېرۍ;

مضمن فراغ* ټاکل_vector8_event_s::چلوونکی جديد(size_t size)
{
      أكد(size == حجم(ټاکل_vector8_event_s));
      أرجع assign8_ډېرۍ.alloc_slab();
}

فراغ ټاکل_vector8_event_s::چلوونکی ړنګول(فراغ*dptr)
{
      assign8_ډېرۍ.free_slab(dptr);
}

بینښه اوږد شمېر_assign8_pool() { أرجع assign8_ډېرۍ.pool; }

بنية ټاکل_real_event_s  : عام event_s {
      vvp_net_ptr_t ptr;
      مزدوج val;
      فراغ پرلپسې_پرلپسې(فراغ) override;
      فراغ single_ګام_display(فراغ) override;

      ساكن فراغ* چلوونکی جديد(size_t);
      ساكن فراغ چلوونکی ړنګول(فراغ*);
};

فراغ ټاکل_real_event_s::پرلپسې_پرلپسې(فراغ)
{
      شمېر_ټاکل_events += 1;
      vvp_send_real(ptr, val, 0);
}

فراغ ټاکل_real_event_s::single_ګام_display(فراغ)
{
      cerr << "assign_real_event: Propagate val=" << val << کرښه_پای;
}

ساكن ثابت size_t ASSIGNR_CHUNK_شمېر = 8192 / حجم(بنية ټاکل_real_event_s);
ساكن slab_t<حجم(ټاکل_real_event_s),ASSIGNR_CHUNK_شمېر> assignr_ډېرۍ;

مضمن فراغ* ټاکل_real_event_s::چلوونکی جديد (size_t size)
{
      أكد(size == حجم(ټاکل_real_event_s));
      أرجع assignr_ډېرۍ.alloc_slab();
}

فراغ ټاکل_real_event_s::چلوونکی ړنګول(فراغ*dptr)
{
      assignr_ډېرۍ.free_slab(dptr);
}

بینښه اوږد شمېر_ټاکل_real_pool(فراغ) { أرجع assignr_ډېرۍ.pool; }

بنية ټاکل_array_کلمه_s  : عام event_s {
      vvp_array_t mem;
      بینښه adr;
      vvp_vector4_t val;
      بینښه off;
      فراغ پرلپسې_پرلپسې(فراغ) override;

      ساكن فراغ* چلوونکی جديد(size_t);
      ساكن فراغ چلوونکی ړنګول(فراغ*);
};

فراغ ټاکل_array_کلمه_s::پرلپسې_پرلپسې(فراغ)
{
      شمېر_ټاکل_events += 1;
      mem->سېټ_کلمه(adr, off, val);
}

ساكن ثابت size_t ARRAY_W_CHUNK_شمېر = 8192 / حجم(بنية ټاکل_array_کلمه_s);
ساكن slab_t<حجم(ټاکل_array_کلمه_s),ARRAY_W_CHUNK_شمېر> array_w_ډېرۍ;

مضمن فراغ* ټاکل_array_کلمه_s::چلوونکی جديد (size_t size)
{
      أكد(size == حجم(ټاکل_array_کلمه_s));
      أرجع array_w_ډېرۍ.alloc_slab();
}

فراغ ټاکل_array_کلمه_s::چلوونکی ړنګول(فراغ*ptr)
{
      array_w_ډېرۍ.free_slab(ptr);
}

بینښه اوږد شمېر_ټاکل_aword_pool(فراغ) { أرجع array_w_ډېرۍ.pool; }

بنية force_vector4_event_s  : عام event_s {
	/* The default constructor. */
      څرګند force_vector4_event_s(ثابت vvp_vector4_t&that): val(that) {
	    net = عدم;
	    بنسټ = 0;
	    vwid = 0;
	    بل_2 = عدم;
      }
	/* Where to do the force. */
      vvp_net_t*net;
	/* Value to force. */
      vvp_vector4_t val;
	/* Offset of the part into the destination. */
      بینښه بنسټ;
	/* Width of the destination vector. */
      بینښه vwid;

      فراغ پرلپسې_پرلپسې(فراغ) override;
      فراغ single_ګام_display(فراغ) override;

      ساكن فراغ* چلوونکی جديد(size_t);
      ساكن فراغ چلوونکی ړنګول(فراغ*);
};

فراغ force_vector4_event_s::پرلپسې_پرلپسې(فراغ)
{
      شمېر_ټاکل_events += 1;

      بینښه wid = val.size();
      إذا ((بنسټ + wid) > vwid)
	    wid = vwid - بنسټ;

	// Make a mask of which bits are to be forced, 0 for unforced
	// bits and 1 for forced bits.
      vvp_vector2_t ماسک (vvp_vector2_t::FILL0, vwid);
      لكل (بینښه idx = 0 ; idx < wid ; idx += 1)
	    ماسک.سېټ_بټ(بنسټ+idx, 1);

      vvp_vector4_t tmp (vwid, BIT4_Z);

	// vvp_net_t::force_vec4 propagates all the bits of the
	// forced vector value, regardless of the mask. This
	// ensures the unforced bits retain their current value.
      vvp_signal_ارزښت*sig = dynamic_cast<vvp_signal_ارزښت*>(net->fil);
      أكد(sig);
      sig->vec4_ارزښت(tmp);

      tmp.سېټ_vec(بنسټ, val);
      net->force_vec4(tmp, ماسک);
}

فراغ force_vector4_event_s::single_ګام_display(فراغ)
{
      cerr << "force_vector4_event: Force val=" << val
	   << ", vwid=" << vwid << ", base=" << بنسټ << کرښه_پای;
}

ساكن ثابت size_t FORCE4_CHUNK_شمېر = 8192 / حجم(بنية force_vector4_event_s);
ساكن slab_t<حجم(force_vector4_event_s),FORCE4_CHUNK_شمېر> force4_ډېرۍ;

مضمن فراغ* force_vector4_event_s::چلوونکی جديد(size_t size)
{
      أكد(size == حجم(force_vector4_event_s));
      أرجع force4_ډېرۍ.alloc_slab();
}

فراغ force_vector4_event_s::چلوونکی ړنګول(فراغ*dptr)
{
      force4_ډېرۍ.free_slab(dptr);
}

بینښه اوږد شمېر_force4_pool(فراغ) { أرجع force4_ډېرۍ.pool; }

/*
 * This class supports the propagation of vec4 outputs from a
 * vvp_net_t object.
 */
بنية propagate_vector4_event_s : عام event_s {
	/* The default constructor. */
      څرګند propagate_vector4_event_s(ثابت vvp_vector4_t&that) : val(that) {
	    net = عدم;
	    بل_2 = عدم;
      }
	/* A constructor that makes the val directly. */
      propagate_vector4_event_s(ثابت vvp_vector4_t&that, بینښه adr, بینښه wid)
      : val(that,adr,wid) {
	    net = عدم;
	    بل_2 = عدم;
      }

	/* Propagate the output of this net. */
      vvp_net_t*net;
	/* value to propagate */
      vvp_vector4_t val;
	/* Action */
      فراغ پرلپسې_پرلپسې(فراغ) override;
      فراغ single_ګام_display(فراغ) override;
};

فراغ propagate_vector4_event_s::پرلپسې_پرلپسې(فراغ)
{
      net->send_vec4(val, 0);
}

فراغ propagate_vector4_event_s::single_ګام_display(فراغ)
{
      cerr << "propagate_vector4_event: Propagate val=" << val << کرښه_پای;
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
      فراغ پرلپسې_پرلپسې(فراغ) override;
      فراغ single_ګام_display(فراغ) override;
};

فراغ propagate_real_event_s::پرلپسې_پرلپسې(فراغ)
{
      net->send_real(val, 0);
}

فراغ propagate_real_event_s::single_ګام_display(فراغ)
{
      cerr << "propagate_real_event: Propagate val=" << val << کرښه_پای;
}

بنية ټاکل_array_r_کلمه_s  : عام event_s {
      vvp_array_t mem;
      بینښه adr;
      مزدوج val;
      فراغ پرلپسې_پرلپسې(فراغ) override;

      ساكن فراغ* چلوونکی جديد(size_t);
      ساكن فراغ چلوونکی ړنګول(فراغ*);
};

فراغ ټاکل_array_r_کلمه_s::پرلپسې_پرلپسې(فراغ)
{
      شمېر_ټاکل_events += 1;
      mem->سېټ_کلمه(adr, val);
}
ساكن ثابت size_t ARRAY_R_W_CHUNK_شمېر = 8192 / حجم(بنية ټاکل_array_r_کلمه_s);
ساكن slab_t<حجم(ټاکل_array_r_کلمه_s),ARRAY_R_W_CHUNK_شمېر> array_r_w_ډېرۍ;

مضمن فراغ* ټاکل_array_r_کلمه_s::چلوونکی جديد(size_t size)
{
      أكد(size == حجم(ټاکل_array_r_کلمه_s));
      أرجع array_r_w_ډېرۍ.alloc_slab();
}

فراغ ټاکل_array_r_کلمه_s::چلوونکی ړنګول(فراغ*ptr)
{
      array_r_w_ډېرۍ.free_slab(ptr);
}

بینښه اوږد شمېر_ټاکل_arword_pool(فراغ) { أرجع array_r_w_ډېرۍ.pool; }

بنية generic_event_s : عام event_s {
      vvp_gen_event_t obj;
      منطقي delete_obj_when_بشپړ;
      فراغ پرلپسې_پرلپسې(فراغ) override;
      فراغ single_ګام_display(فراغ) override;

      ساكن فراغ* چلوونکی جديد(size_t);
      ساكن فراغ چلوونکی ړنګول(فراغ*);
};

فراغ generic_event_s::پرلپسې_پرلپسې(فراغ)
{
      شمېر_gen_events += 1;
      إذا (obj) {
	    obj->پرلپسې_پرلپسې();
	    إذا (delete_obj_when_بشپړ)
		  ړنګول obj;
      }
}

فراغ generic_event_s::single_ګام_display(فراغ)
{
      obj->single_ګام_display();
}

ساكن ثابت size_t GENERIC_CHUNK_شمېر = 131072 / حجم(بنية generic_event_s);
ساكن slab_t<حجم(generic_event_s),GENERIC_CHUNK_شمېر> generic_event_ډېرۍ;

مضمن فراغ* generic_event_s::چلوونکی جديد(size_t size)
{
      أكد(size == حجم(generic_event_s));
      أرجع generic_event_ډېرۍ.alloc_slab();
}

فراغ generic_event_s::چلوونکی ړنګول(فراغ*ptr)
{
      generic_event_ډېرۍ.free_slab(ptr);
}

بینښه اوږد شمېر_gen_pool(فراغ) { أرجع generic_event_ډېرۍ.pool; }

/*
** These event_time_s will be required a lot, at high frequency.
** Once allocated, we never free them, but stash them away for next time.
*/


ساكن ثابت size_t وخت_CHUNK_شمېر = 8192 / حجم(بنية event_وخت_s);
ساكن slab_t<حجم(event_وخت_s),وخت_CHUNK_شمېر> event_وخت_ډېرۍ;

مضمن فراغ* event_وخت_s::چلوونکی جديد (size_t size)
{
      أكد(size == حجم(بنية event_وخت_s));
      فراغ*ptr = event_وخت_ډېرۍ.alloc_slab();
      أرجع ptr;
}

مضمن فراغ event_وخت_s::چلوونکی ړنګول(فراغ*ptr, size_t)
{
      event_وخت_ډېرۍ.free_slab(ptr);
}

بینښه اوږد شمېر_وخت_pool(فراغ) { أرجع event_وخت_ډېرۍ.pool; }

/*
 * This is the head of the list of pending events. This includes all
 * the events that have not been executed yet, and reaches into the
 * future.
 */
ساكن بنية event_وخت_s* sched_list = 0;

/*
 * This is a list of initialization events. The setup puts
 * initializations in this list so that they happen before the
 * simulation as a whole starts. This prevents time-0 triggers of
 * certain events.
 */
ساكن بنية event_s* مهالوېش_init_list = 0;

/*
 * This is the head of the list of final events.
 */
ساكن بنية event_s* مهالوېش_final_list = 0;

/*
 * This flag is true until a VPI task or function finishes the
 * simulation.
 */
ساكن منطقي مهالوېش_runnable = ریښتیا;
ساكن منطقي مهالوېش_stopped_flag  = خطأ;
ساكن منطقي مهالوېش_single_ګام_flag = خطأ;
ساكن منطقي no_سیګنالونه_flag = خطأ;

فراغ مهالوېش_پای(صحيح)
{
      مهالوېش_runnable = خطأ;
}

فراغ مهالوېش_stop(صحيح)
{
      مهالوېش_stopped_flag = ریښتیا;
}

فراغ مهالوېش_single_ګام(صحيح)
{
      مهالوېش_single_ګام_flag = ریښتیا;
}

منطقي مهالوېش_پای_ته_رسېدلی(فراغ)
{
      أرجع !مهالوېش_runnable;
}

منطقي مهالوېش_stopped(فراغ)
{
      أرجع مهالوېش_stopped_flag;
}

خارجي "C" فراغ vvp_no_سیګنالونه(فراغ)
{
      no_سیګنالونه_flag = ریښتیا;
}

/*
 * These are the signal handling infrastructure. The SIGINT signal
 * leads to an implicit $stop. The SIGHUP and SIGTERM signals lead
 * to an implicit $finish.
 */
خارجي منطقي stop_is_پای;

خارجي "C" فراغ سیګنالونه_handler(صحيح signum)
{
#که_تعریف_شوی __MINGW32__
	// Windows implements the original UNIX semantics for signal,
	// so we have to re-establish the signal handler each time a
	// signal is caught.
      إشارة(signum, &سیګنالونه_handler);
#د_که_پای
      إذا (signum != SIGINT)
	    stop_is_پای = ریښتیا;
      مهالوېش_stopped_flag = ریښتیا;
}

ساكن فراغ سیګنالونه_capture(فراغ)
{
      إذا (no_سیګنالونه_flag)
	   أرجع;
#که_نه_تعریف_شوی __MINGW32__
      إشارة(SIGHUP,  &سیګنالونه_handler);
#د_که_پای
      إشارة(SIGINT,  &سیګنالونه_handler);
      إشارة(SIGTERM, &سیګنالونه_handler);
}

ساكن فراغ سیګنالونه_revert(فراغ)
{
      إذا (no_سیګنالونه_flag)
	   أرجع;
#که_نه_تعریف_شوی __MINGW32__
      إشارة(SIGHUP,  SIG_DFL);
#د_که_پای
      إشارة(SIGINT,  SIG_DFL);
      إشارة(SIGTERM, SIG_DFL);
}

/*
 * This function puts an event on the end of the pre-simulation event queue.
 */
ساكن فراغ مهالوېش_init_event(بنية event_s*cur)
{
      إذا (مهالوېش_init_list == 0) {
            cur->بل_2 = cur;
      } وإلا {
            cur->بل_2 = مهالوېش_init_list->بل_2;
            مهالوېش_init_list->بل_2 = cur;
      }
      مهالوېش_init_list = cur;
}

/*
 * This function puts an event on the end of the post-simulation event queue.
 */
ساكن فراغ مهالوېش_final_event(بنية event_s*cur)
{
      إذا (مهالوېش_final_list == 0) {
            cur->بل_2 = cur;
      } وإلا {
            cur->بل_2 = مهالوېش_final_list->بل_2;
            مهالوېش_final_list->بل_2 = cur;
      }
      مهالوېش_final_list = cur;
}

/*
 * This function does all the hard work of putting an event into the
 * event queue. The event delay is taken from the event structure
 * itself, and the structure is placed in the right place in the
 * queue.
 */
تعريفنوع تعداد event_کتار_e { SEQ_پیل, SEQ_فعال, SEQ_INACTIVE, SEQ_NBASSIGN,
			     SEQ_RWSYNC, SEQ_ROSYNC, DEL_THREAD } event_کتار_t;

ساكن فراغ مهالوېش_event(بنية event_s*cur, vvp_time64_t delay,
			    event_کتار_t select_کتار)
{
      cur->بل_2 = cur;
      بنية event_وخت_s*ctim = sched_list;

      إذا (sched_list == 0) {
	      /* Is the event_time list completely empty? Create the
		 first event_time object. */
	    ctim = جديد بنية event_وخت_s;
	    ctim->delay = delay;
	    ctim->بل_2  = 0;
	    sched_list = ctim;

      } وإلا إذا (sched_list->delay > delay) {

	      /* Am I looking for an event before the first event_time?
		 If so, create a new event_time to go in front. */
	    بنية event_وخت_s*tmp = جديد بنية event_وخت_s;
	    tmp->delay = delay;
	    tmp->بل_2 = ctim;
	    ctim->delay -= delay;
	    ctim = tmp;
	    sched_list = ctim;

      } وإلا {
	    بنية event_وخت_s*prev = 0;

	    طالما (ctim->بل_2 && (ctim->delay < delay)) {
		  delay -= ctim->delay;
		  prev = ctim;
		  ctim = ctim->بل_2;
	    }

	    إذا (ctim->delay > delay) {
		  بنية event_وخت_s*tmp = جديد بنية event_وخت_s;
		  tmp->delay = delay;
		  tmp->بل_2  = prev->بل_2;
		  prev->بل_2 = tmp;

		  tmp->بل_2->delay -= delay;
		  ctim = tmp;

	    } وإلا إذا (ctim->delay == delay) {

	    } وإلا {
		  أكد(ctim->بل_2 == 0);
		  بنية event_وخت_s*tmp = جديد بنية event_وخت_s;
		  tmp->delay = delay - ctim->delay;
		  tmp->بل_2 = 0;
		  ctim->بل_2 = tmp;

		  ctim = tmp;
	    }
      }

	/* By this point, ctim is the event_time structure that is to
	   receive the event at hand. Put the event in to the
	   appropriate list for the kind of assign we have at hand. */

      بنية event_s** q = 0;

      بدل (select_کتار) {
	  حالة SEQ_پیل:
	    q = &ctim->پیل;
	    اكسر;

	  حالة SEQ_فعال:
	    q = &ctim->فعال;
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
		  cur->بل_2 = (*q)->بل_2;
		  (*q)->بل_2 = cur;
	    }
	    *q = cur;
      }
}

ساكن فراغ مهالوېش_event_ټېل(بنية event_s*cur)
{
      إذا ((sched_list == 0) || (sched_list->delay > 0)) {
	    مهالوېش_event(cur, 0, SEQ_فعال);
	    أرجع;
      }

      بنية event_وخت_s*ctim = sched_list;

      إذا (ctim->فعال == 0) {
	    cur->بل_2 = cur;
	    ctim->فعال = cur;
	    أرجع;
      }

      cur->بل_2 = ctim->فعال->بل_2;
      ctim->فعال->بل_2 = cur;
}

فراغ مهالوېش_vthread(vthread_t thr, vvp_time64_t delay, منطقي ټېل_flag)
{
      بنية vthread_event_s*cur = جديد vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);

      إذا (ټېل_flag && (delay == 0)) {
	      /* Special case: If the delay is 0, the push_flag means
		 I can push this event in front of everything. This is
		 used by the %fork statement, for example, to perform
		 task calls. */
	    مهالوېش_event_ټېل(cur);

      } وإلا {
	    مهالوېش_event(cur, delay, SEQ_فعال);
      }
}

فراغ مهالوېش_t0_trigger(vvp_net_ptr_t ptr)
{
      vvp_vector4_t بټ (1, BIT4_X);
      بنية ټاکل_vector4_event_s*cur = جديد بنية ټاکل_vector4_event_s(بټ);
      cur->ptr = ptr;
      مهالوېش_event(cur, 0, SEQ_INACTIVE);
}

فراغ مهالوېش_inactive(vthread_t thr)
{
      بنية vthread_event_s*cur = جديد vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);
      مهالوېش_event(cur, 0, SEQ_INACTIVE);
}

فراغ مهالوېش_init_vthread(vthread_t thr)
{
      بنية vthread_event_s*cur = جديد vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);

      مهالوېش_init_event(cur);
}

فراغ مهالوېش_final_vthread(vthread_t thr)
{
      بنية vthread_event_s*cur = جديد vthread_event_s;

      cur->thr = thr;
      vthread_mark_final(thr);
      vthread_mark_scheduled(thr);

      مهالوېش_final_event(cur);
}

فراغ مهالوېش_ټاکل_ویکټور(vvp_net_ptr_t ptr,
			    بینښه بنسټ, بینښه vwid,
			    ثابت vvp_vector4_t&بټ,
			    vvp_time64_t delay)
{
      بنية ټاکل_vector4_event_s*cur = جديد بنية ټاکل_vector4_event_s(بټ);
      cur->ptr = ptr;
      cur->بنسټ = بنسټ;
      cur->vwid = vwid;
      مهالوېش_event(cur, delay, SEQ_NBASSIGN);
}

فراغ مهالوېش_force_ویکټور(vvp_net_t*net,
			    بینښه بنسټ, بینښه vwid,
			    ثابت vvp_vector4_t&بټ,
			    vvp_time64_t delay)
{
      بنية force_vector4_event_s*cur = جديد بنية force_vector4_event_s(بټ);
      cur->net = net;
      cur->بنسټ = بنسټ;
      cur->vwid = vwid;
      مهالوېش_event(cur, delay, SEQ_NBASSIGN);
}

فراغ مهالوېش_propagate_ویکټور(vvp_net_t*net,
			       vvp_time64_t delay,
			       ثابت vvp_vector4_t&src)
{
      بنية propagate_vector4_event_s*cur
	    = جديد بنية propagate_vector4_event_s(src);
      cur->net = net;
      مهالوېش_event(cur, delay, SEQ_NBASSIGN);
}

// FIXME: This needs to create a non-blocking event, but only one per time slot.
//        Is schedule_event_ or execution actually filtering since the net is
//        already X because this is not triggering?
فراغ مهالوېش_propagate_event(vvp_net_t*net,
                              vvp_time64_t delay)
{
      vvp_vector4_t tmp (1, BIT4_X);
      بنية propagate_vector4_event_s*cur
	    = جديد بنية propagate_vector4_event_s(tmp);
      cur->net = net;
      مهالوېش_event(cur, delay, SEQ_NBASSIGN);
}

فراغ مهالوېش_ټاکل_array_کلمه(vvp_array_t mem,
				بینښه کلمه_addr,
				بینښه off,
				ثابت vvp_vector4_t&val,
				vvp_time64_t delay)
{
      بنية ټاکل_array_کلمه_s*cur = جديد بنية ټاکل_array_کلمه_s;
      cur->mem = mem;
      cur->adr = کلمه_addr;
      cur->off = off;
      cur->val = val;
      مهالوېش_event(cur, delay, SEQ_NBASSIGN);
}

فراغ مهالوېش_ټاکل_array_کلمه(vvp_array_t mem,
				بینښه کلمه_addr,
				مزدوج val,
				vvp_time64_t delay)
{
      بنية ټاکل_array_r_کلمه_s*cur = جديد بنية ټاکل_array_r_کلمه_s;
      cur->mem = mem;
      cur->adr = کلمه_addr;
      cur->val = val;
      مهالوېش_event(cur, delay, SEQ_NBASSIGN);
}

فراغ مهالوېش_سېټ_ویکټور(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بټ)
{
      بنية ټاکل_vector4_event_s*cur = جديد بنية ټاکل_vector4_event_s(بټ);
      cur->ptr = ptr;
      cur->بنسټ = 0;
      cur->vwid = 0;
      مهالوېش_event(cur, 0, SEQ_فعال);
}

فراغ مهالوېش_سېټ_ویکټور(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&بټ)
{
      بنية ټاکل_vector8_event_s*cur = جديد بنية ټاکل_vector8_event_s;
      cur->ptr = ptr;
      cur->val = بټ;
      مهالوېش_event(cur, 0, SEQ_فعال);
}

فراغ مهالوېش_سېټ_ویکټور(vvp_net_ptr_t ptr, مزدوج بټ)
{
      بنية ټاکل_real_event_s*cur = جديد بنية ټاکل_real_event_s;
      cur->ptr = ptr;
      cur->val = بټ;
      مهالوېش_event(cur, 0, SEQ_فعال);
}

فراغ مهالوېش_init_ویکټور(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بټ)
{
      بنية ټاکل_vector4_event_s*cur = جديد بنية ټاکل_vector4_event_s(بټ);
      cur->ptr = ptr;
      cur->بنسټ = 0;
      cur->vwid = 0;
      مهالوېش_init_event(cur);
}

فراغ مهالوېش_init_ویکټور(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&بټ)
{
      بنية ټاکل_vector8_event_s*cur = جديد بنية ټاکل_vector8_event_s;
      cur->ptr = ptr;
      cur->val = بټ;
      مهالوېش_init_event(cur);
}

فراغ مهالوېش_init_ویکټور(vvp_net_ptr_t ptr, مزدوج بټ)
{
      بنية ټاکل_real_event_s*cur = جديد بنية ټاکل_real_event_s;
      cur->ptr = ptr;
      cur->val = بټ;
      مهالوېش_init_event(cur);
}

فراغ مهالوېش_init_propagate(vvp_net_t*net, vvp_vector4_t بټ)
{
      بنية propagate_vector4_event_s*cur = جديد بنية propagate_vector4_event_s(بټ);
      cur->net = net;
      مهالوېش_init_event(cur);
}

فراغ مهالوېش_init_propagate(vvp_net_t*net, مزدوج بټ)
{
      بنية propagate_real_event_s*cur = جديد بنية propagate_real_event_s;
      cur->net = net;
      cur->val = بټ;
      مهالوېش_init_event(cur);
}

فراغ مهالوېش_del_thr(vthread_t thr)
{
      بنية del_thr_event_s*cur = جديد del_thr_event_s;

      cur->thr = thr;

      مهالوېش_event(cur, 0, DEL_THREAD);
}

فراغ مهالوېش_generic(vvp_gen_event_t obj, vvp_time64_t delay,
		      منطقي sync_flag, منطقي ro_flag, منطقي delete_when_بشپړ)
{
      بنية generic_event_s*cur = جديد generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_بشپړ = delete_when_بشپړ;
      مهالوېش_event(cur, delay,
		      sync_flag? (ro_flag?SEQ_ROSYNC:SEQ_RWSYNC) : SEQ_فعال);

      إذا (sync_flag)
	    vthread_delay_delete();
}

ساكن منطقي sim_started;

فراغ مهالوېش_functor(vvp_gen_event_t obj)
{
      بنية generic_event_s*cur = جديد generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_بشپړ = خطأ;
      إذا (!sim_started) {
            مهالوېش_init_event(cur);
      } وإلا {
            مهالوېش_event(cur, 0, SEQ_فعال);
      }
}

فراغ مهالوېش_at_پیل_د_simtime(vvp_gen_event_t obj, vvp_time64_t delay)
{
      بنية generic_event_s*cur = جديد generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_بشپړ = خطأ;
      مهالوېش_event(cur, delay, SEQ_پیل);
}

/*
 * In the vvp runtime of Icarus Verilog, the SEQ_RWSYNC time step is
 * after all of the non-blocking assignments, so is effectively the
 * same as the ReadWriteSync time.
 */
فراغ مهالوېش_at_پای_ټکی_د_simtime(vvp_gen_event_t obj, vvp_time64_t delay)
{
      بنية generic_event_s*cur = جديد generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_بشپړ = خطأ;
      مهالوېش_event(cur, delay, SEQ_RWSYNC);
}

ساكن vvp_time64_t مهالوېش_وخت;
vvp_time64_t مهالوېش_simtime(فراغ)
{ أرجع مهالوېش_وخت; }

ساكن منطقي sim_at_rosync = خطأ;
منطقي مهالوېش_at_rosync(فراغ)
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
ساكن فراغ پرلپسې_rosync(بنية event_وخت_s*ctim)
{
      sim_at_rosync = ریښتیا;
      طالما (ctim->rosync) {
	    بنية event_s*cur = ctim->rosync->بل_2;
	    إذا (cur->بل_2 == cur) {
		  ctim->rosync = 0;
	    } وإلا {
		  ctim->rosync->بل_2 = cur->بل_2;
	    }

	    cur->پرلپسې_پرلپسې();
	    ړنګول cur;
      }
      sim_at_rosync = خطأ;

      طالما (ctim->del_thr) {
	    بنية event_s*cur = ctim->del_thr->بل_2;
	    إذا (cur->بل_2 == cur) {
		  ctim->del_thr = 0;
	    } وإلا {
		  ctim->del_thr->بل_2 = cur->بل_2;
	    }

	    cur->پرلپسې_پرلپسې();
	    ړنګول cur;
      }

      إذا (ctim->فعال || ctim->inactive || ctim->nbassign || ctim->rwsync) {
	    cerr << "SCHEDULER ERROR: read-only sync events "
		 << "created RW events!" << کرښه_پای;
      }
}

فراغ مهالوېش_simulate(فراغ)
{
      منطقي پرلپسې_finals;
      sim_started = خطأ;

      مهالوېش_وخت = 0;

      إذا (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute EndOfCompile callbacks\n");
      }

      // Execute end of compile callbacks
      vpi_پای_ټکی_د_Compile();

      إذا (verbose_flag) {
	    vpi_mcd_printf(1, " ...propagate initialization events\n");
      }

	// Execute initialization events.
      طالما (مهالوېش_init_list) {
	    بنية event_s*cur = مهالوېش_init_list->بل_2;
	    إذا (cur->بل_2 == cur) {
		  مهالوېش_init_list = 0;
	    } وإلا {
		  مهالوېش_init_list->بل_2 = cur->بل_2;
	    }
	    cur->پرلپسې_پرلپسې();
	    ړنګول cur;
      }

      sim_started = ریښتیا;

      إذا (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute StartOfSim callbacks\n");
      }

      // Execute start of simulation callbacks
      vpi_پیل_د_Sim();

      سیګنالونه_capture();

      إذا (verbose_flag) {
	    vpi_mcd_printf(1, " ...run scheduler\n");
      }

      // If there were no compiletf, etc. errors then we are going to
      // process events and when done run the final blocks.
      پرلپسې_finals = مهالوېش_runnable;

      إذا (مهالوېش_runnable) طالما (sched_list) {

	    إذا (مهالوېش_stopped_flag) {
		  مهالوېش_stopped_flag = خطأ;
		  stop_handler(0);
		  // You can finish from the debugger without a time change.
		  إذا (!مهالوېش_runnable) اكسر;
		  استمر;
	    }

	      /* ctim is the current time step. */
	    بنية event_وخت_s* ctim = sched_list;

	      /* If the time is advancing, then first run the
		 postponed sync events. Run them all. */
	    إذا (ctim->delay > 0) {

		  إذا (!مهالوېش_runnable) اكسر;
		  مهالوېش_وخت += ctim->delay;
		    /* When the design is being traced (we are emitting
		     * file/line information) also print any time changes. */
		  إذا (show_file_کرښه) {
			cerr << "Advancing to simulation time: "
			     << مهالوېش_وخت << کرښه_پای;
		  }
		  ctim->delay = 0;

		  vpi_بل_Sim_وخت();
		    // Process the cbAtStartOfSimTime callbacks.
		  طالما (ctim->پیل) {
			بنية event_s*cur = ctim->پیل->بل_2;
			إذا (cur->بل_2 == cur) {
			      ctim->پیل = 0;
			} وإلا {
			      ctim->پیل->بل_2 = cur->بل_2;
			}
			cur->پرلپسې_پرلپسې();
			ړنګول (cur);
		  }
	    }


	      /* If there are no more active events, advance the event
		 queues. If there are not events at all, then release
		 the event_time object. */
	    إذا (ctim->فعال == 0) {
		  ctim->فعال = ctim->inactive;
		  ctim->inactive = 0;

		  إذا (ctim->فعال == 0) {
			ctim->فعال = ctim->nbassign;
			ctim->nbassign = 0;

			إذا (ctim->فعال == 0) {
			      ctim->فعال = ctim->rwsync;
			      ctim->rwsync = 0;

				/* If out of rw events, then run the rosync
				   events and delete this time step. This also
				   deletes threads as needed. */
			      إذا (ctim->فعال == 0) {
				    پرلپسې_rosync(ctim);
				    sched_list = ctim->بل_2;
				    ړنګول ctim;
				    استمر;
			      }
			}
		  }
	    }

	      /* Pull the first item off the list. If this is the last
		 cell in the list, then clear the list. Execute that
		 event type, and delete it. */
	    بنية event_s*cur = ctim->فعال->بل_2;
	    إذا (cur->بل_2 == cur) {
		  ctim->فعال = 0;
	    } وإلا {
		  ctim->فعال->بل_2 = cur->بل_2;
	    }

	    إذا (مهالوېش_single_ګام_flag) {
		  cur->single_ګام_display();
		  مهالوېش_stopped_flag = ریښتیا;
		  مهالوېش_single_ګام_flag = خطأ;
	    }

	    cur->پرلپسې_پرلپسې();

	    ړنګول (cur);
      }

	// Execute final events.
      مهالوېش_runnable = پرلپسې_finals;
      طالما (مهالوېش_runnable && مهالوېش_final_list) {
	    بنية event_s*cur = مهالوېش_final_list->بل_2;
	    إذا (cur->بل_2 == cur) {
		  مهالوېش_final_list = 0;
	    } وإلا {
		  مهالوېش_final_list->بل_2 = cur->بل_2;
	    }
	    cur->پرلپسې_پرلپسې();
	    ړنګول cur;
      }

      سیګنالونه_revert();

      إذا (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute Postsim callbacks\n");
      }

      // Execute post-simulation callbacks
      vpiPostsim();
#که_تعریف_شوی CHECK_WITH_VALGRIND
      مهالوېش_delete();
#د_که_پای
}

#که_تعریف_شوی CHECK_WITH_VALGRIND
فراغ مهالوېش_delete(فراغ)
{
      vthread_event_ډېرۍ.delete_pool();
      assign4_ډېرۍ.delete_pool();
      assign8_ډېرۍ.delete_pool();
      assignr_ډېرۍ.delete_pool();
      array_w_ډېرۍ.delete_pool();
      array_r_w_ډېرۍ.delete_pool();
      generic_event_ډېرۍ.delete_pool();
      event_وخت_ډېرۍ.delete_pool();
}
#د_که_پای
