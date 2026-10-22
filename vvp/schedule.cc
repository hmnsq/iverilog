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
#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
# include  "ivl_alloc.h"
#جيڪڏهن_پورو

استعمال نطاق std;

بنانشان ڊگهو ڳڻپ_سونپيو_events = 0;
بنانشان ڊگهو ڳڻپ_gen_events = 0;
بنانشان ڊگهو ڳڻپ_thread_events = 0;
  // Count the time events (A time cell created)
بنانشان ڊگهو ڳڻپ_وقت_events = 0;



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
      بنية event_s*ايندڙ_2;
      مجازي ~event_s() { }
      مجازي فراغ لاڳيتو_لاڳيتو(فراغ) =0;

	// Write something about the event to stderr
      مجازي فراغ single_قدم_display(فراغ);

	// Fallback new/delete
      ساكن فراغ*عامل جديد (size_t size) { أرجع ::جديد حرف[size]; }
      ساكن فراغ عامل ميٽيو(فراغ*ptr)  { ::ميٽيو[]( static_cast<حرف*>(ptr) ); }
};

فراغ event_s::single_قدم_display(فراغ)
{
      std::cerr << "event_s: Step into event " << typeid(*هذا).name() << std::سٽ_پڄاڻي;
}

بنية event_وقت_s {
      event_وقت_s() {
	    ڳڻپ_وقت_events += 1;
	    delay = 0;
	    شروعات = 0;
	    سرگرم = 0;
	    inactive = 0;
	    nbassign = 0;
	    rwsync = 0;
	    rosync = 0;
	    del_thr = 0;
	    ايندڙ_2 = عدم;
      }
      vvp_time64_t delay;

      بنية event_s*شروعات;
      بنية event_s*سرگرم;
      بنية event_s*inactive;
      بنية event_s*nbassign;
      بنية event_s*rwsync;
      بنية event_s*rosync;
      بنية event_s*del_thr;

      بنية event_وقت_s*ايندڙ_2;

      ساكن فراغ* عامل جديد (size_t);
      ساكن فراغ عامل ميٽيو(فراغ*obj, size_t s);
};

vvp_gen_event_s::~vvp_gen_event_s()
{
}

فراغ vvp_gen_event_s::single_قدم_display(فراغ)
{
      cerr << "vvp_gen_event_s: Step into event " << typeid(*هذا).name() << سٽ_پڄاڻي;
}

/*
 * Derived event types
 */
بنية vthread_event_s : عام event_s {
      vthread_t thr;
      فراغ لاڳيتو_لاڳيتو(فراغ) override;
      فراغ single_قدم_display(فراغ) override;

      ساكن فراغ* عامل جديد(size_t);
      ساكن فراغ عامل ميٽيو(فراغ*);
};

فراغ vthread_event_s::لاڳيتو_لاڳيتو(فراغ)
{
      ڳڻپ_thread_events += 1;
      vthread_لاڳيتو(thr);
}

فراغ vthread_event_s::single_قدم_display(فراغ)
{
      __vpiScope*scope = vthread_scope(thr);
      cerr << "vthread_event: Resume thread"
	   << " scope=" << scope->vpi_get_str(vpi_ڀريل_Name)
	   << سٽ_پڄاڻي;
}

ساكن ثابت size_t VTHR_CHUNK_ڳڻپ = 8192 / حجم(بنية vthread_event_s);
ساكن slab_t<حجم(vthread_event_s),VTHR_CHUNK_ڳڻپ> vthread_event_ڍير;

مضمن فراغ* vthread_event_s::عامل جديد(size_t size)
{
      أكد(size == حجم(vthread_event_s));
      أرجع vthread_event_ڍير.alloc_slab();
}

فراغ vthread_event_s::عامل ميٽيو(فراغ*dptr)
{
      vthread_event_ڍير.free_slab(dptr);
}

بنية del_thr_event_s : عام event_s {
      vthread_t thr;
      فراغ لاڳيتو_لاڳيتو(فراغ) override;
      فراغ single_قدم_display(فراغ) override;
};

فراغ del_thr_event_s::لاڳيتو_لاڳيتو(فراغ)
{
      vthread_delete(thr);
}

فراغ del_thr_event_s::single_قدم_display(فراغ)
{
      __vpiScope*scope = vthread_scope(thr);
      cerr << "del_thr_event: Reap completed thread"
	   << " scope=" << scope->vpi_get_str(vpi_ڀريل_Name) << سٽ_پڄاڻي;
}

بنية سونپيو_vector4_event_s  : عام event_s {
	/* The default constructor. */
      واضح سونپيو_vector4_event_s(ثابت vvp_vector4_t&that) : val(that) {
	    بنياد = 0;
	    vwid = 0;
	    ايندڙ_2 = عدم;
      }

	/* Where to do the assign. */
      vvp_net_ptr_t ptr;
	/* Value to assign. */
      vvp_vector4_t val;
	/* Offset of the part into the destination. */
      بنانشان بنياد;
	/* Width of the destination vector. */
      بنانشان vwid;
      فراغ لاڳيتو_لاڳيتو(فراغ) override;
      فراغ single_قدم_display(فراغ) override;

      ساكن فراغ* عامل جديد(size_t);
      ساكن فراغ عامل ميٽيو(فراغ*);
};

فراغ سونپيو_vector4_event_s::لاڳيتو_لاڳيتو(فراغ)
{
      ڳڻپ_سونپيو_events += 1;
      إذا (vwid > 0)
	    vvp_send_vec4_pv(ptr, val, بنياد, vwid, 0);
      وإلا
	    vvp_send_vec4(ptr, val, 0);
}

فراغ سونپيو_vector4_event_s::single_قدم_display(فراغ)
{
      cerr << "assign_vector4_event: Propagate val=" << val
	   << ", vwid=" << vwid << ", base=" << بنياد << سٽ_پڄاڻي;
}

ساكن ثابت size_t ASSIGN4_CHUNK_ڳڻپ = 524288 / حجم(بنية سونپيو_vector4_event_s);
ساكن slab_t<حجم(سونپيو_vector4_event_s),ASSIGN4_CHUNK_ڳڻپ> assign4_ڍير;

مضمن فراغ* سونپيو_vector4_event_s::عامل جديد(size_t size)
{
      أكد(size == حجم(سونپيو_vector4_event_s));
      أرجع assign4_ڍير.alloc_slab();
}

فراغ سونپيو_vector4_event_s::عامل ميٽيو(فراغ*dptr)
{
      assign4_ڍير.free_slab(dptr);
}

بنانشان ڊگهو ڳڻپ_assign4_pool(فراغ) { أرجع assign4_ڍير.pool; }

بنية سونپيو_vector8_event_s  : عام event_s {
      vvp_net_ptr_t ptr;
      vvp_vector8_t val;
      فراغ لاڳيتو_لاڳيتو(فراغ) override;
      فراغ single_قدم_display(فراغ) override;

      ساكن فراغ* عامل جديد(size_t);
      ساكن فراغ عامل ميٽيو(فراغ*);
};

فراغ سونپيو_vector8_event_s::لاڳيتو_لاڳيتو(فراغ)
{
      ڳڻپ_سونپيو_events += 1;
      vvp_send_vec8(ptr, val);
}

فراغ سونپيو_vector8_event_s::single_قدم_display(فراغ)
{
      cerr << "assign_vector8_event: Propagate val=" << val << سٽ_پڄاڻي;
}

ساكن ثابت size_t ASSIGN8_CHUNK_ڳڻپ = 8192 / حجم(بنية سونپيو_vector8_event_s);
ساكن slab_t<حجم(سونپيو_vector8_event_s),ASSIGN8_CHUNK_ڳڻپ> assign8_ڍير;

مضمن فراغ* سونپيو_vector8_event_s::عامل جديد(size_t size)
{
      أكد(size == حجم(سونپيو_vector8_event_s));
      أرجع assign8_ڍير.alloc_slab();
}

فراغ سونپيو_vector8_event_s::عامل ميٽيو(فراغ*dptr)
{
      assign8_ڍير.free_slab(dptr);
}

بنانشان ڊگهو ڳڻپ_assign8_pool() { أرجع assign8_ڍير.pool; }

بنية سونپيو_real_event_s  : عام event_s {
      vvp_net_ptr_t ptr;
      مزدوج val;
      فراغ لاڳيتو_لاڳيتو(فراغ) override;
      فراغ single_قدم_display(فراغ) override;

      ساكن فراغ* عامل جديد(size_t);
      ساكن فراغ عامل ميٽيو(فراغ*);
};

فراغ سونپيو_real_event_s::لاڳيتو_لاڳيتو(فراغ)
{
      ڳڻپ_سونپيو_events += 1;
      vvp_send_real(ptr, val, 0);
}

فراغ سونپيو_real_event_s::single_قدم_display(فراغ)
{
      cerr << "assign_real_event: Propagate val=" << val << سٽ_پڄاڻي;
}

ساكن ثابت size_t ASSIGNR_CHUNK_ڳڻپ = 8192 / حجم(بنية سونپيو_real_event_s);
ساكن slab_t<حجم(سونپيو_real_event_s),ASSIGNR_CHUNK_ڳڻپ> assignr_ڍير;

مضمن فراغ* سونپيو_real_event_s::عامل جديد (size_t size)
{
      أكد(size == حجم(سونپيو_real_event_s));
      أرجع assignr_ڍير.alloc_slab();
}

فراغ سونپيو_real_event_s::عامل ميٽيو(فراغ*dptr)
{
      assignr_ڍير.free_slab(dptr);
}

بنانشان ڊگهو ڳڻپ_سونپيو_real_pool(فراغ) { أرجع assignr_ڍير.pool; }

بنية سونپيو_array_لفظ_s  : عام event_s {
      vvp_array_t mem;
      بنانشان adr;
      vvp_vector4_t val;
      بنانشان off;
      فراغ لاڳيتو_لاڳيتو(فراغ) override;

      ساكن فراغ* عامل جديد(size_t);
      ساكن فراغ عامل ميٽيو(فراغ*);
};

فراغ سونپيو_array_لفظ_s::لاڳيتو_لاڳيتو(فراغ)
{
      ڳڻپ_سونپيو_events += 1;
      mem->سيٽ_لفظ(adr, off, val);
}

ساكن ثابت size_t ARRAY_W_CHUNK_ڳڻپ = 8192 / حجم(بنية سونپيو_array_لفظ_s);
ساكن slab_t<حجم(سونپيو_array_لفظ_s),ARRAY_W_CHUNK_ڳڻپ> array_w_ڍير;

مضمن فراغ* سونپيو_array_لفظ_s::عامل جديد (size_t size)
{
      أكد(size == حجم(سونپيو_array_لفظ_s));
      أرجع array_w_ڍير.alloc_slab();
}

فراغ سونپيو_array_لفظ_s::عامل ميٽيو(فراغ*ptr)
{
      array_w_ڍير.free_slab(ptr);
}

بنانشان ڊگهو ڳڻپ_سونپيو_aword_pool(فراغ) { أرجع array_w_ڍير.pool; }

بنية force_vector4_event_s  : عام event_s {
	/* The default constructor. */
      واضح force_vector4_event_s(ثابت vvp_vector4_t&that): val(that) {
	    net = عدم;
	    بنياد = 0;
	    vwid = 0;
	    ايندڙ_2 = عدم;
      }
	/* Where to do the force. */
      vvp_net_t*net;
	/* Value to force. */
      vvp_vector4_t val;
	/* Offset of the part into the destination. */
      بنانشان بنياد;
	/* Width of the destination vector. */
      بنانشان vwid;

      فراغ لاڳيتو_لاڳيتو(فراغ) override;
      فراغ single_قدم_display(فراغ) override;

      ساكن فراغ* عامل جديد(size_t);
      ساكن فراغ عامل ميٽيو(فراغ*);
};

فراغ force_vector4_event_s::لاڳيتو_لاڳيتو(فراغ)
{
      ڳڻپ_سونپيو_events += 1;

      بنانشان wid = val.size();
      إذا ((بنياد + wid) > vwid)
	    wid = vwid - بنياد;

	// Make a mask of which bits are to be forced, 0 for unforced
	// bits and 1 for forced bits.
      vvp_vector2_t نقاب (vvp_vector2_t::FILL0, vwid);
      لكل (بنانشان idx = 0 ; idx < wid ; idx += 1)
	    نقاب.سيٽ_ٻٽو_انگ(بنياد+idx, 1);

      vvp_vector4_t tmp (vwid, BIT4_Z);

	// vvp_net_t::force_vec4 propagates all the bits of the
	// forced vector value, regardless of the mask. This
	// ensures the unforced bits retain their current value.
      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*>(net->fil);
      أكد(sig);
      sig->vec4_قدر(tmp);

      tmp.سيٽ_vec(بنياد, val);
      net->force_vec4(tmp, نقاب);
}

فراغ force_vector4_event_s::single_قدم_display(فراغ)
{
      cerr << "force_vector4_event: Force val=" << val
	   << ", vwid=" << vwid << ", base=" << بنياد << سٽ_پڄاڻي;
}

ساكن ثابت size_t FORCE4_CHUNK_ڳڻپ = 8192 / حجم(بنية force_vector4_event_s);
ساكن slab_t<حجم(force_vector4_event_s),FORCE4_CHUNK_ڳڻپ> force4_ڍير;

مضمن فراغ* force_vector4_event_s::عامل جديد(size_t size)
{
      أكد(size == حجم(force_vector4_event_s));
      أرجع force4_ڍير.alloc_slab();
}

فراغ force_vector4_event_s::عامل ميٽيو(فراغ*dptr)
{
      force4_ڍير.free_slab(dptr);
}

بنانشان ڊگهو ڳڻپ_force4_pool(فراغ) { أرجع force4_ڍير.pool; }

/*
 * This class supports the propagation of vec4 outputs from a
 * vvp_net_t object.
 */
بنية propagate_vector4_event_s : عام event_s {
	/* The default constructor. */
      واضح propagate_vector4_event_s(ثابت vvp_vector4_t&that) : val(that) {
	    net = عدم;
	    ايندڙ_2 = عدم;
      }
	/* A constructor that makes the val directly. */
      propagate_vector4_event_s(ثابت vvp_vector4_t&that, بنانشان adr, بنانشان wid)
      : val(that,adr,wid) {
	    net = عدم;
	    ايندڙ_2 = عدم;
      }

	/* Propagate the output of this net. */
      vvp_net_t*net;
	/* value to propagate */
      vvp_vector4_t val;
	/* Action */
      فراغ لاڳيتو_لاڳيتو(فراغ) override;
      فراغ single_قدم_display(فراغ) override;
};

فراغ propagate_vector4_event_s::لاڳيتو_لاڳيتو(فراغ)
{
      net->send_vec4(val, 0);
}

فراغ propagate_vector4_event_s::single_قدم_display(فراغ)
{
      cerr << "propagate_vector4_event: Propagate val=" << val << سٽ_پڄاڻي;
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
      فراغ لاڳيتو_لاڳيتو(فراغ) override;
      فراغ single_قدم_display(فراغ) override;
};

فراغ propagate_real_event_s::لاڳيتو_لاڳيتو(فراغ)
{
      net->send_real(val, 0);
}

فراغ propagate_real_event_s::single_قدم_display(فراغ)
{
      cerr << "propagate_real_event: Propagate val=" << val << سٽ_پڄاڻي;
}

بنية سونپيو_array_r_لفظ_s  : عام event_s {
      vvp_array_t mem;
      بنانشان adr;
      مزدوج val;
      فراغ لاڳيتو_لاڳيتو(فراغ) override;

      ساكن فراغ* عامل جديد(size_t);
      ساكن فراغ عامل ميٽيو(فراغ*);
};

فراغ سونپيو_array_r_لفظ_s::لاڳيتو_لاڳيتو(فراغ)
{
      ڳڻپ_سونپيو_events += 1;
      mem->سيٽ_لفظ(adr, val);
}
ساكن ثابت size_t ARRAY_R_W_CHUNK_ڳڻپ = 8192 / حجم(بنية سونپيو_array_r_لفظ_s);
ساكن slab_t<حجم(سونپيو_array_r_لفظ_s),ARRAY_R_W_CHUNK_ڳڻپ> array_r_w_ڍير;

مضمن فراغ* سونپيو_array_r_لفظ_s::عامل جديد(size_t size)
{
      أكد(size == حجم(سونپيو_array_r_لفظ_s));
      أرجع array_r_w_ڍير.alloc_slab();
}

فراغ سونپيو_array_r_لفظ_s::عامل ميٽيو(فراغ*ptr)
{
      array_r_w_ڍير.free_slab(ptr);
}

بنانشان ڊگهو ڳڻپ_سونپيو_arword_pool(فراغ) { أرجع array_r_w_ڍير.pool; }

بنية generic_event_s : عام event_s {
      vvp_gen_event_t obj;
      منطقي delete_obj_when_پورو;
      فراغ لاڳيتو_لاڳيتو(فراغ) override;
      فراغ single_قدم_display(فراغ) override;

      ساكن فراغ* عامل جديد(size_t);
      ساكن فراغ عامل ميٽيو(فراغ*);
};

فراغ generic_event_s::لاڳيتو_لاڳيتو(فراغ)
{
      ڳڻپ_gen_events += 1;
      إذا (obj) {
	    obj->لاڳيتو_لاڳيتو();
	    إذا (delete_obj_when_پورو)
		  ميٽيو obj;
      }
}

فراغ generic_event_s::single_قدم_display(فراغ)
{
      obj->single_قدم_display();
}

ساكن ثابت size_t GENERIC_CHUNK_ڳڻپ = 131072 / حجم(بنية generic_event_s);
ساكن slab_t<حجم(generic_event_s),GENERIC_CHUNK_ڳڻپ> generic_event_ڍير;

مضمن فراغ* generic_event_s::عامل جديد(size_t size)
{
      أكد(size == حجم(generic_event_s));
      أرجع generic_event_ڍير.alloc_slab();
}

فراغ generic_event_s::عامل ميٽيو(فراغ*ptr)
{
      generic_event_ڍير.free_slab(ptr);
}

بنانشان ڊگهو ڳڻپ_gen_pool(فراغ) { أرجع generic_event_ڍير.pool; }

/*
** These event_time_s will be required a lot, at high frequency.
** Once allocated, we never free them, but stash them away for next time.
*/


ساكن ثابت size_t وقت_CHUNK_ڳڻپ = 8192 / حجم(بنية event_وقت_s);
ساكن slab_t<حجم(event_وقت_s),وقت_CHUNK_ڳڻپ> event_وقت_ڍير;

مضمن فراغ* event_وقت_s::عامل جديد (size_t size)
{
      أكد(size == حجم(بنية event_وقت_s));
      فراغ*ptr = event_وقت_ڍير.alloc_slab();
      أرجع ptr;
}

مضمن فراغ event_وقت_s::عامل ميٽيو(فراغ*ptr, size_t)
{
      event_وقت_ڍير.free_slab(ptr);
}

بنانشان ڊگهو ڳڻپ_وقت_pool(فراغ) { أرجع event_وقت_ڍير.pool; }

/*
 * This is the head of the list of pending events. This includes all
 * the events that have not been executed yet, and reaches into the
 * future.
 */
ساكن بنية event_وقت_s* sched_list = 0;

/*
 * This is a list of initialization events. The setup puts
 * initializations in this list so that they happen before the
 * simulation as a whole starts. This prevents time-0 triggers of
 * certain events.
 */
ساكن بنية event_s* وقت_جدول_init_list = 0;

/*
 * This is the head of the list of final events.
 */
ساكن بنية event_s* وقت_جدول_final_list = 0;

/*
 * This flag is true until a VPI task or function finishes the
 * simulation.
 */
ساكن منطقي وقت_جدول_runnable = سچ;
ساكن منطقي وقت_جدول_stopped_flag  = خطأ;
ساكن منطقي وقت_جدول_single_قدم_flag = خطأ;
ساكن منطقي no_سگنل_flag = خطأ;

فراغ وقت_جدول_پڄاڻي(صحيح)
{
      وقت_جدول_runnable = خطأ;
}

فراغ وقت_جدول_stop(صحيح)
{
      وقت_جدول_stopped_flag = سچ;
}

فراغ وقت_جدول_single_قدم(صحيح)
{
      وقت_جدول_single_قدم_flag = سچ;
}

منطقي وقت_جدول_ختم(فراغ)
{
      أرجع !وقت_جدول_runnable;
}

منطقي وقت_جدول_stopped(فراغ)
{
      أرجع وقت_جدول_stopped_flag;
}

خارجي "C" فراغ vvp_no_سگنل(فراغ)
{
      no_سگنل_flag = سچ;
}

/*
 * These are the signal handling infrastructure. The SIGINT signal
 * leads to an implicit $stop. The SIGHUP and SIGTERM signals lead
 * to an implicit $finish.
 */
خارجي منطقي stop_is_پڄاڻي;

خارجي "C" فراغ سگنل_handler(صحيح signum)
{
#جيڪڏهن_وصف_ٿيل __MINGW32__
	// Windows implements the original UNIX semantics for signal,
	// so we have to re-establish the signal handler each time a
	// signal is caught.
      إشارة(signum, &سگنل_handler);
#جيڪڏهن_پورو
      إذا (signum != SIGINT)
	    stop_is_پڄاڻي = سچ;
      وقت_جدول_stopped_flag = سچ;
}

ساكن فراغ سگنل_capture(فراغ)
{
      إذا (no_سگنل_flag)
	   أرجع;
#جيڪڏهن_وصف_نه_ٿيل __MINGW32__
      إشارة(SIGHUP,  &سگنل_handler);
#جيڪڏهن_پورو
      إشارة(SIGINT,  &سگنل_handler);
      إشارة(SIGTERM, &سگنل_handler);
}

ساكن فراغ سگنل_revert(فراغ)
{
      إذا (no_سگنل_flag)
	   أرجع;
#جيڪڏهن_وصف_نه_ٿيل __MINGW32__
      إشارة(SIGHUP,  SIG_DFL);
#جيڪڏهن_پورو
      إشارة(SIGINT,  SIG_DFL);
      إشارة(SIGTERM, SIG_DFL);
}

/*
 * This function puts an event on the end of the pre-simulation event queue.
 */
ساكن فراغ وقت_جدول_init_event(بنية event_s*cur)
{
      إذا (وقت_جدول_init_list == 0) {
            cur->ايندڙ_2 = cur;
      } وإلا {
            cur->ايندڙ_2 = وقت_جدول_init_list->ايندڙ_2;
            وقت_جدول_init_list->ايندڙ_2 = cur;
      }
      وقت_جدول_init_list = cur;
}

/*
 * This function puts an event on the end of the post-simulation event queue.
 */
ساكن فراغ وقت_جدول_final_event(بنية event_s*cur)
{
      إذا (وقت_جدول_final_list == 0) {
            cur->ايندڙ_2 = cur;
      } وإلا {
            cur->ايندڙ_2 = وقت_جدول_final_list->ايندڙ_2;
            وقت_جدول_final_list->ايندڙ_2 = cur;
      }
      وقت_جدول_final_list = cur;
}

/*
 * This function does all the hard work of putting an event into the
 * event queue. The event delay is taken from the event structure
 * itself, and the structure is placed in the right place in the
 * queue.
 */
تعريفنوع تعداد event_قطار_e { SEQ_شروعات, SEQ_سرگرم, SEQ_INACTIVE, SEQ_NBASSIGN,
			     SEQ_RWSYNC, SEQ_ROSYNC, DEL_THREAD } event_قطار_t;

ساكن فراغ وقت_جدول_event(بنية event_s*cur, vvp_time64_t delay,
			    event_قطار_t select_قطار)
{
      cur->ايندڙ_2 = cur;
      بنية event_وقت_s*ctim = sched_list;

      إذا (sched_list == 0) {
	      /* Is the event_time list completely empty? Create the
		 first event_time object. */
	    ctim = جديد بنية event_وقت_s;
	    ctim->delay = delay;
	    ctim->ايندڙ_2  = 0;
	    sched_list = ctim;

      } وإلا إذا (sched_list->delay > delay) {

	      /* Am I looking for an event before the first event_time?
		 If so, create a new event_time to go in front. */
	    بنية event_وقت_s*tmp = جديد بنية event_وقت_s;
	    tmp->delay = delay;
	    tmp->ايندڙ_2 = ctim;
	    ctim->delay -= delay;
	    ctim = tmp;
	    sched_list = ctim;

      } وإلا {
	    بنية event_وقت_s*prev = 0;

	    طالما (ctim->ايندڙ_2 && (ctim->delay < delay)) {
		  delay -= ctim->delay;
		  prev = ctim;
		  ctim = ctim->ايندڙ_2;
	    }

	    إذا (ctim->delay > delay) {
		  بنية event_وقت_s*tmp = جديد بنية event_وقت_s;
		  tmp->delay = delay;
		  tmp->ايندڙ_2  = prev->ايندڙ_2;
		  prev->ايندڙ_2 = tmp;

		  tmp->ايندڙ_2->delay -= delay;
		  ctim = tmp;

	    } وإلا إذا (ctim->delay == delay) {

	    } وإلا {
		  أكد(ctim->ايندڙ_2 == 0);
		  بنية event_وقت_s*tmp = جديد بنية event_وقت_s;
		  tmp->delay = delay - ctim->delay;
		  tmp->ايندڙ_2 = 0;
		  ctim->ايندڙ_2 = tmp;

		  ctim = tmp;
	    }
      }

	/* By this point, ctim is the event_time structure that is to
	   receive the event at hand. Put the event in to the
	   appropriate list for the kind of assign we have at hand. */

      بنية event_s** q = 0;

      بدل (select_قطار) {
	  حالة SEQ_شروعات:
	    q = &ctim->شروعات;
	    اكسر;

	  حالة SEQ_سرگرم:
	    q = &ctim->سرگرم;
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
		  cur->ايندڙ_2 = (*q)->ايندڙ_2;
		  (*q)->ايندڙ_2 = cur;
	    }
	    *q = cur;
      }
}

ساكن فراغ وقت_جدول_event_ڌڪو(بنية event_s*cur)
{
      إذا ((sched_list == 0) || (sched_list->delay > 0)) {
	    وقت_جدول_event(cur, 0, SEQ_سرگرم);
	    أرجع;
      }

      بنية event_وقت_s*ctim = sched_list;

      إذا (ctim->سرگرم == 0) {
	    cur->ايندڙ_2 = cur;
	    ctim->سرگرم = cur;
	    أرجع;
      }

      cur->ايندڙ_2 = ctim->سرگرم->ايندڙ_2;
      ctim->سرگرم->ايندڙ_2 = cur;
}

فراغ وقت_جدول_vthread(vthread_t thr, vvp_time64_t delay, منطقي ڌڪو_flag)
{
      بنية vthread_event_s*cur = جديد vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);

      إذا (ڌڪو_flag && (delay == 0)) {
	      /* Special case: If the delay is 0, the push_flag means
		 I can push this event in front of everything. This is
		 used by the %fork statement, for example, to perform
		 task calls. */
	    وقت_جدول_event_ڌڪو(cur);

      } وإلا {
	    وقت_جدول_event(cur, delay, SEQ_سرگرم);
      }
}

فراغ وقت_جدول_t0_trigger(vvp_net_ptr_t ptr)
{
      vvp_vector4_t ٻٽو_انگ (1, BIT4_X);
      بنية سونپيو_vector4_event_s*cur = جديد بنية سونپيو_vector4_event_s(ٻٽو_انگ);
      cur->ptr = ptr;
      وقت_جدول_event(cur, 0, SEQ_INACTIVE);
}

فراغ وقت_جدول_inactive(vthread_t thr)
{
      بنية vthread_event_s*cur = جديد vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);
      وقت_جدول_event(cur, 0, SEQ_INACTIVE);
}

فراغ وقت_جدول_init_vthread(vthread_t thr)
{
      بنية vthread_event_s*cur = جديد vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);

      وقت_جدول_init_event(cur);
}

فراغ وقت_جدول_final_vthread(vthread_t thr)
{
      بنية vthread_event_s*cur = جديد vthread_event_s;

      cur->thr = thr;
      vthread_mark_final(thr);
      vthread_mark_scheduled(thr);

      وقت_جدول_final_event(cur);
}

فراغ وقت_جدول_سونپيو_ويڪٽر(vvp_net_ptr_t ptr,
			    بنانشان بنياد, بنانشان vwid,
			    ثابت vvp_vector4_t&ٻٽو_انگ,
			    vvp_time64_t delay)
{
      بنية سونپيو_vector4_event_s*cur = جديد بنية سونپيو_vector4_event_s(ٻٽو_انگ);
      cur->ptr = ptr;
      cur->بنياد = بنياد;
      cur->vwid = vwid;
      وقت_جدول_event(cur, delay, SEQ_NBASSIGN);
}

فراغ وقت_جدول_force_ويڪٽر(vvp_net_t*net,
			    بنانشان بنياد, بنانشان vwid,
			    ثابت vvp_vector4_t&ٻٽو_انگ,
			    vvp_time64_t delay)
{
      بنية force_vector4_event_s*cur = جديد بنية force_vector4_event_s(ٻٽو_انگ);
      cur->net = net;
      cur->بنياد = بنياد;
      cur->vwid = vwid;
      وقت_جدول_event(cur, delay, SEQ_NBASSIGN);
}

فراغ وقت_جدول_propagate_ويڪٽر(vvp_net_t*net,
			       vvp_time64_t delay,
			       ثابت vvp_vector4_t&src)
{
      بنية propagate_vector4_event_s*cur
	    = جديد بنية propagate_vector4_event_s(src);
      cur->net = net;
      وقت_جدول_event(cur, delay, SEQ_NBASSIGN);
}

// FIXME: This needs to create a non-blocking event, but only one per time slot.
//        Is schedule_event_ or execution actually filtering since the net is
//        already X because this is not triggering?
فراغ وقت_جدول_propagate_event(vvp_net_t*net,
                              vvp_time64_t delay)
{
      vvp_vector4_t tmp (1, BIT4_X);
      بنية propagate_vector4_event_s*cur
	    = جديد بنية propagate_vector4_event_s(tmp);
      cur->net = net;
      وقت_جدول_event(cur, delay, SEQ_NBASSIGN);
}

فراغ وقت_جدول_سونپيو_array_لفظ(vvp_array_t mem,
				بنانشان لفظ_addr,
				بنانشان off,
				ثابت vvp_vector4_t&val,
				vvp_time64_t delay)
{
      بنية سونپيو_array_لفظ_s*cur = جديد بنية سونپيو_array_لفظ_s;
      cur->mem = mem;
      cur->adr = لفظ_addr;
      cur->off = off;
      cur->val = val;
      وقت_جدول_event(cur, delay, SEQ_NBASSIGN);
}

فراغ وقت_جدول_سونپيو_array_لفظ(vvp_array_t mem,
				بنانشان لفظ_addr,
				مزدوج val,
				vvp_time64_t delay)
{
      بنية سونپيو_array_r_لفظ_s*cur = جديد بنية سونپيو_array_r_لفظ_s;
      cur->mem = mem;
      cur->adr = لفظ_addr;
      cur->val = val;
      وقت_جدول_event(cur, delay, SEQ_NBASSIGN);
}

فراغ وقت_جدول_سيٽ_ويڪٽر(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ٻٽو_انگ)
{
      بنية سونپيو_vector4_event_s*cur = جديد بنية سونپيو_vector4_event_s(ٻٽو_انگ);
      cur->ptr = ptr;
      cur->بنياد = 0;
      cur->vwid = 0;
      وقت_جدول_event(cur, 0, SEQ_سرگرم);
}

فراغ وقت_جدول_سيٽ_ويڪٽر(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&ٻٽو_انگ)
{
      بنية سونپيو_vector8_event_s*cur = جديد بنية سونپيو_vector8_event_s;
      cur->ptr = ptr;
      cur->val = ٻٽو_انگ;
      وقت_جدول_event(cur, 0, SEQ_سرگرم);
}

فراغ وقت_جدول_سيٽ_ويڪٽر(vvp_net_ptr_t ptr, مزدوج ٻٽو_انگ)
{
      بنية سونپيو_real_event_s*cur = جديد بنية سونپيو_real_event_s;
      cur->ptr = ptr;
      cur->val = ٻٽو_انگ;
      وقت_جدول_event(cur, 0, SEQ_سرگرم);
}

فراغ وقت_جدول_init_ويڪٽر(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ٻٽو_انگ)
{
      بنية سونپيو_vector4_event_s*cur = جديد بنية سونپيو_vector4_event_s(ٻٽو_انگ);
      cur->ptr = ptr;
      cur->بنياد = 0;
      cur->vwid = 0;
      وقت_جدول_init_event(cur);
}

فراغ وقت_جدول_init_ويڪٽر(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&ٻٽو_انگ)
{
      بنية سونپيو_vector8_event_s*cur = جديد بنية سونپيو_vector8_event_s;
      cur->ptr = ptr;
      cur->val = ٻٽو_انگ;
      وقت_جدول_init_event(cur);
}

فراغ وقت_جدول_init_ويڪٽر(vvp_net_ptr_t ptr, مزدوج ٻٽو_انگ)
{
      بنية سونپيو_real_event_s*cur = جديد بنية سونپيو_real_event_s;
      cur->ptr = ptr;
      cur->val = ٻٽو_انگ;
      وقت_جدول_init_event(cur);
}

فراغ وقت_جدول_init_propagate(vvp_net_t*net, vvp_vector4_t ٻٽو_انگ)
{
      بنية propagate_vector4_event_s*cur = جديد بنية propagate_vector4_event_s(ٻٽو_انگ);
      cur->net = net;
      وقت_جدول_init_event(cur);
}

فراغ وقت_جدول_init_propagate(vvp_net_t*net, مزدوج ٻٽو_انگ)
{
      بنية propagate_real_event_s*cur = جديد بنية propagate_real_event_s;
      cur->net = net;
      cur->val = ٻٽو_انگ;
      وقت_جدول_init_event(cur);
}

فراغ وقت_جدول_del_thr(vthread_t thr)
{
      بنية del_thr_event_s*cur = جديد del_thr_event_s;

      cur->thr = thr;

      وقت_جدول_event(cur, 0, DEL_THREAD);
}

فراغ وقت_جدول_generic(vvp_gen_event_t obj, vvp_time64_t delay,
		      منطقي sync_flag, منطقي ro_flag, منطقي delete_when_پورو)
{
      بنية generic_event_s*cur = جديد generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_پورو = delete_when_پورو;
      وقت_جدول_event(cur, delay,
		      sync_flag? (ro_flag?SEQ_ROSYNC:SEQ_RWSYNC) : SEQ_سرگرم);

      إذا (sync_flag)
	    vthread_delay_delete();
}

ساكن منطقي sim_started;

فراغ وقت_جدول_functor(vvp_gen_event_t obj)
{
      بنية generic_event_s*cur = جديد generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_پورو = خطأ;
      إذا (!sim_started) {
            وقت_جدول_init_event(cur);
      } وإلا {
            وقت_جدول_event(cur, 0, SEQ_سرگرم);
      }
}

فراغ وقت_جدول_at_شروعات_جو_simtime(vvp_gen_event_t obj, vvp_time64_t delay)
{
      بنية generic_event_s*cur = جديد generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_پورو = خطأ;
      وقت_جدول_event(cur, delay, SEQ_شروعات);
}

/*
 * In the vvp runtime of Icarus Verilog, the SEQ_RWSYNC time step is
 * after all of the non-blocking assignments, so is effectively the
 * same as the ReadWriteSync time.
 */
فراغ وقت_جدول_at_پڇاڙي_جو_simtime(vvp_gen_event_t obj, vvp_time64_t delay)
{
      بنية generic_event_s*cur = جديد generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_پورو = خطأ;
      وقت_جدول_event(cur, delay, SEQ_RWSYNC);
}

ساكن vvp_time64_t وقت_جدول_وقت;
vvp_time64_t وقت_جدول_simtime(فراغ)
{ أرجع وقت_جدول_وقت; }

ساكن منطقي sim_at_rosync = خطأ;
منطقي وقت_جدول_at_rosync(فراغ)
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
ساكن فراغ لاڳيتو_rosync(بنية event_وقت_s*ctim)
{
      sim_at_rosync = سچ;
      طالما (ctim->rosync) {
	    بنية event_s*cur = ctim->rosync->ايندڙ_2;
	    إذا (cur->ايندڙ_2 == cur) {
		  ctim->rosync = 0;
	    } وإلا {
		  ctim->rosync->ايندڙ_2 = cur->ايندڙ_2;
	    }

	    cur->لاڳيتو_لاڳيتو();
	    ميٽيو cur;
      }
      sim_at_rosync = خطأ;

      طالما (ctim->del_thr) {
	    بنية event_s*cur = ctim->del_thr->ايندڙ_2;
	    إذا (cur->ايندڙ_2 == cur) {
		  ctim->del_thr = 0;
	    } وإلا {
		  ctim->del_thr->ايندڙ_2 = cur->ايندڙ_2;
	    }

	    cur->لاڳيتو_لاڳيتو();
	    ميٽيو cur;
      }

      إذا (ctim->سرگرم || ctim->inactive || ctim->nbassign || ctim->rwsync) {
	    cerr << "SCHEDULER ERROR: read-only sync events "
		 << "created RW events!" << سٽ_پڄاڻي;
      }
}

فراغ وقت_جدول_simulate(فراغ)
{
      منطقي لاڳيتو_finals;
      sim_started = خطأ;

      وقت_جدول_وقت = 0;

      إذا (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute EndOfCompile callbacks\n");
      }

      // Execute end of compile callbacks
      vpi_پڇاڙي_جو_Compile();

      إذا (verbose_flag) {
	    vpi_mcd_printf(1, " ...propagate initialization events\n");
      }

	// Execute initialization events.
      طالما (وقت_جدول_init_list) {
	    بنية event_s*cur = وقت_جدول_init_list->ايندڙ_2;
	    إذا (cur->ايندڙ_2 == cur) {
		  وقت_جدول_init_list = 0;
	    } وإلا {
		  وقت_جدول_init_list->ايندڙ_2 = cur->ايندڙ_2;
	    }
	    cur->لاڳيتو_لاڳيتو();
	    ميٽيو cur;
      }

      sim_started = سچ;

      إذا (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute StartOfSim callbacks\n");
      }

      // Execute start of simulation callbacks
      vpi_شروعات_جو_Sim();

      سگنل_capture();

      إذا (verbose_flag) {
	    vpi_mcd_printf(1, " ...run scheduler\n");
      }

      // If there were no compiletf, etc. errors then we are going to
      // process events and when done run the final blocks.
      لاڳيتو_finals = وقت_جدول_runnable;

      إذا (وقت_جدول_runnable) طالما (sched_list) {

	    إذا (وقت_جدول_stopped_flag) {
		  وقت_جدول_stopped_flag = خطأ;
		  stop_handler(0);
		  // You can finish from the debugger without a time change.
		  إذا (!وقت_جدول_runnable) اكسر;
		  استمر;
	    }

	      /* ctim is the current time step. */
	    بنية event_وقت_s* ctim = sched_list;

	      /* If the time is advancing, then first run the
		 postponed sync events. Run them all. */
	    إذا (ctim->delay > 0) {

		  إذا (!وقت_جدول_runnable) اكسر;
		  وقت_جدول_وقت += ctim->delay;
		    /* When the design is being traced (we are emitting
		     * file/line information) also print any time changes. */
		  إذا (show_file_سٽ) {
			cerr << "Advancing to simulation time: "
			     << وقت_جدول_وقت << سٽ_پڄاڻي;
		  }
		  ctim->delay = 0;

		  vpi_ايندڙ_Sim_وقت();
		    // Process the cbAtStartOfSimTime callbacks.
		  طالما (ctim->شروعات) {
			بنية event_s*cur = ctim->شروعات->ايندڙ_2;
			إذا (cur->ايندڙ_2 == cur) {
			      ctim->شروعات = 0;
			} وإلا {
			      ctim->شروعات->ايندڙ_2 = cur->ايندڙ_2;
			}
			cur->لاڳيتو_لاڳيتو();
			ميٽيو (cur);
		  }
	    }


	      /* If there are no more active events, advance the event
		 queues. If there are not events at all, then release
		 the event_time object. */
	    إذا (ctim->سرگرم == 0) {
		  ctim->سرگرم = ctim->inactive;
		  ctim->inactive = 0;

		  إذا (ctim->سرگرم == 0) {
			ctim->سرگرم = ctim->nbassign;
			ctim->nbassign = 0;

			إذا (ctim->سرگرم == 0) {
			      ctim->سرگرم = ctim->rwsync;
			      ctim->rwsync = 0;

				/* If out of rw events, then run the rosync
				   events and delete this time step. This also
				   deletes threads as needed. */
			      إذا (ctim->سرگرم == 0) {
				    لاڳيتو_rosync(ctim);
				    sched_list = ctim->ايندڙ_2;
				    ميٽيو ctim;
				    استمر;
			      }
			}
		  }
	    }

	      /* Pull the first item off the list. If this is the last
		 cell in the list, then clear the list. Execute that
		 event type, and delete it. */
	    بنية event_s*cur = ctim->سرگرم->ايندڙ_2;
	    إذا (cur->ايندڙ_2 == cur) {
		  ctim->سرگرم = 0;
	    } وإلا {
		  ctim->سرگرم->ايندڙ_2 = cur->ايندڙ_2;
	    }

	    إذا (وقت_جدول_single_قدم_flag) {
		  cur->single_قدم_display();
		  وقت_جدول_stopped_flag = سچ;
		  وقت_جدول_single_قدم_flag = خطأ;
	    }

	    cur->لاڳيتو_لاڳيتو();

	    ميٽيو (cur);
      }

	// Execute final events.
      وقت_جدول_runnable = لاڳيتو_finals;
      طالما (وقت_جدول_runnable && وقت_جدول_final_list) {
	    بنية event_s*cur = وقت_جدول_final_list->ايندڙ_2;
	    إذا (cur->ايندڙ_2 == cur) {
		  وقت_جدول_final_list = 0;
	    } وإلا {
		  وقت_جدول_final_list->ايندڙ_2 = cur->ايندڙ_2;
	    }
	    cur->لاڳيتو_لاڳيتو();
	    ميٽيو cur;
      }

      سگنل_revert();

      إذا (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute Postsim callbacks\n");
      }

      // Execute post-simulation callbacks
      vpiPostsim();
#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
      وقت_جدول_delete();
#جيڪڏهن_پورو
}

#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
فراغ وقت_جدول_delete(فراغ)
{
      vthread_event_ڍير.delete_pool();
      assign4_ڍير.delete_pool();
      assign8_ڍير.delete_pool();
      assignr_ڍير.delete_pool();
      array_w_ڍير.delete_pool();
      array_r_w_ڍير.delete_pool();
      generic_event_ڍير.delete_pool();
      event_وقت_ڍير.delete_pool();
}
#جيڪڏهن_پورو
