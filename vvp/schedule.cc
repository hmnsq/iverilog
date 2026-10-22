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
#اگر_تعریف_شده CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
# include  "ivl_alloc.h"
#پایان_اگر

استفاده نطاق std;

بیعلامت دراز شمار_تخصیص_بده_events = 0;
بیعلامت دراز شمار_gen_events = 0;
بیعلامت دراز شمار_thread_events = 0;
  // Count the time events (A time cell created)
بیعلامت دراز شمار_زمان_events = 0;



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
      بنية event_s*بعدی_2;
      مجازی ~event_s() { }
      مجازی فراغ پیاپی_پیاپی(فراغ) =0;

	// Write something about the event to stderr
      مجازی فراغ single_گام_display(فراغ);

	// Fallback new/delete
      ساكن فراغ*عملگر جديد (size_t size) { أرجع ::جديد حرف[size]; }
      ساكن فراغ عملگر حذف(فراغ*ptr)  { ::حذف[]( static_cast<حرف*>(ptr) ); }
};

فراغ event_s::single_گام_display(فراغ)
{
      std::cerr << "event_s: Step into event " << typeid(*هذا).name() << std::پایان_سطر;
}

بنية event_زمان_s {
      event_زمان_s() {
	    شمار_زمان_events += 1;
	    delay = 0;
	    شروع = 0;
	    فعال = 0;
	    inactive = 0;
	    nbassign = 0;
	    rwsync = 0;
	    rosync = 0;
	    del_thr = 0;
	    بعدی_2 = عدم;
      }
      vvp_time64_t delay;

      بنية event_s*شروع;
      بنية event_s*فعال;
      بنية event_s*inactive;
      بنية event_s*nbassign;
      بنية event_s*rwsync;
      بنية event_s*rosync;
      بنية event_s*del_thr;

      بنية event_زمان_s*بعدی_2;

      ساكن فراغ* عملگر جديد (size_t);
      ساكن فراغ عملگر حذف(فراغ*obj, size_t s);
};

vvp_gen_event_s::~vvp_gen_event_s()
{
}

فراغ vvp_gen_event_s::single_گام_display(فراغ)
{
      cerr << "vvp_gen_event_s: Step into event " << typeid(*هذا).name() << پایان_سطر;
}

/*
 * Derived event types
 */
بنية vthread_event_s : عام event_s {
      vthread_t thr;
      فراغ پیاپی_پیاپی(فراغ) override;
      فراغ single_گام_display(فراغ) override;

      ساكن فراغ* عملگر جديد(size_t);
      ساكن فراغ عملگر حذف(فراغ*);
};

فراغ vthread_event_s::پیاپی_پیاپی(فراغ)
{
      شمار_thread_events += 1;
      vthread_پیاپی(thr);
}

فراغ vthread_event_s::single_گام_display(فراغ)
{
      __vpiScope*scope = vthread_scope(thr);
      cerr << "vthread_event: Resume thread"
	   << " scope=" << scope->vpi_get_str(vpi_پر_Name)
	   << پایان_سطر;
}

ساكن ثابت size_t VTHR_CHUNK_شمار = 8192 / حجم(بنية vthread_event_s);
ساكن slab_t<حجم(vthread_event_s),VTHR_CHUNK_شمار> vthread_event_انباشته;

مضمن فراغ* vthread_event_s::عملگر جديد(size_t size)
{
      أكد(size == حجم(vthread_event_s));
      أرجع vthread_event_انباشته.alloc_slab();
}

فراغ vthread_event_s::عملگر حذف(فراغ*dptr)
{
      vthread_event_انباشته.free_slab(dptr);
}

بنية del_thr_event_s : عام event_s {
      vthread_t thr;
      فراغ پیاپی_پیاپی(فراغ) override;
      فراغ single_گام_display(فراغ) override;
};

فراغ del_thr_event_s::پیاپی_پیاپی(فراغ)
{
      vthread_delete(thr);
}

فراغ del_thr_event_s::single_گام_display(فراغ)
{
      __vpiScope*scope = vthread_scope(thr);
      cerr << "del_thr_event: Reap completed thread"
	   << " scope=" << scope->vpi_get_str(vpi_پر_Name) << پایان_سطر;
}

بنية تخصیص_بده_vector4_event_s  : عام event_s {
	/* The default constructor. */
      صریح تخصیص_بده_vector4_event_s(ثابت vvp_vector4_t&that) : val(that) {
	    پایه = 0;
	    vwid = 0;
	    بعدی_2 = عدم;
      }

	/* Where to do the assign. */
      vvp_net_ptr_t ptr;
	/* Value to assign. */
      vvp_vector4_t val;
	/* Offset of the part into the destination. */
      بیعلامت پایه;
	/* Width of the destination vector. */
      بیعلامت vwid;
      فراغ پیاپی_پیاپی(فراغ) override;
      فراغ single_گام_display(فراغ) override;

      ساكن فراغ* عملگر جديد(size_t);
      ساكن فراغ عملگر حذف(فراغ*);
};

فراغ تخصیص_بده_vector4_event_s::پیاپی_پیاپی(فراغ)
{
      شمار_تخصیص_بده_events += 1;
      إذا (vwid > 0)
	    vvp_send_vec4_pv(ptr, val, پایه, vwid, 0);
      وإلا
	    vvp_send_vec4(ptr, val, 0);
}

فراغ تخصیص_بده_vector4_event_s::single_گام_display(فراغ)
{
      cerr << "assign_vector4_event: Propagate val=" << val
	   << ", vwid=" << vwid << ", base=" << پایه << پایان_سطر;
}

ساكن ثابت size_t ASSIGN4_CHUNK_شمار = 524288 / حجم(بنية تخصیص_بده_vector4_event_s);
ساكن slab_t<حجم(تخصیص_بده_vector4_event_s),ASSIGN4_CHUNK_شمار> assign4_انباشته;

مضمن فراغ* تخصیص_بده_vector4_event_s::عملگر جديد(size_t size)
{
      أكد(size == حجم(تخصیص_بده_vector4_event_s));
      أرجع assign4_انباشته.alloc_slab();
}

فراغ تخصیص_بده_vector4_event_s::عملگر حذف(فراغ*dptr)
{
      assign4_انباشته.free_slab(dptr);
}

بیعلامت دراز شمار_assign4_pool(فراغ) { أرجع assign4_انباشته.pool; }

بنية تخصیص_بده_vector8_event_s  : عام event_s {
      vvp_net_ptr_t ptr;
      vvp_vector8_t val;
      فراغ پیاپی_پیاپی(فراغ) override;
      فراغ single_گام_display(فراغ) override;

      ساكن فراغ* عملگر جديد(size_t);
      ساكن فراغ عملگر حذف(فراغ*);
};

فراغ تخصیص_بده_vector8_event_s::پیاپی_پیاپی(فراغ)
{
      شمار_تخصیص_بده_events += 1;
      vvp_send_vec8(ptr, val);
}

فراغ تخصیص_بده_vector8_event_s::single_گام_display(فراغ)
{
      cerr << "assign_vector8_event: Propagate val=" << val << پایان_سطر;
}

ساكن ثابت size_t ASSIGN8_CHUNK_شمار = 8192 / حجم(بنية تخصیص_بده_vector8_event_s);
ساكن slab_t<حجم(تخصیص_بده_vector8_event_s),ASSIGN8_CHUNK_شمار> assign8_انباشته;

مضمن فراغ* تخصیص_بده_vector8_event_s::عملگر جديد(size_t size)
{
      أكد(size == حجم(تخصیص_بده_vector8_event_s));
      أرجع assign8_انباشته.alloc_slab();
}

فراغ تخصیص_بده_vector8_event_s::عملگر حذف(فراغ*dptr)
{
      assign8_انباشته.free_slab(dptr);
}

بیعلامت دراز شمار_assign8_pool() { أرجع assign8_انباشته.pool; }

بنية تخصیص_بده_real_event_s  : عام event_s {
      vvp_net_ptr_t ptr;
      مزدوج val;
      فراغ پیاپی_پیاپی(فراغ) override;
      فراغ single_گام_display(فراغ) override;

      ساكن فراغ* عملگر جديد(size_t);
      ساكن فراغ عملگر حذف(فراغ*);
};

فراغ تخصیص_بده_real_event_s::پیاپی_پیاپی(فراغ)
{
      شمار_تخصیص_بده_events += 1;
      vvp_send_real(ptr, val, 0);
}

فراغ تخصیص_بده_real_event_s::single_گام_display(فراغ)
{
      cerr << "assign_real_event: Propagate val=" << val << پایان_سطر;
}

ساكن ثابت size_t ASSIGNR_CHUNK_شمار = 8192 / حجم(بنية تخصیص_بده_real_event_s);
ساكن slab_t<حجم(تخصیص_بده_real_event_s),ASSIGNR_CHUNK_شمار> assignr_انباشته;

مضمن فراغ* تخصیص_بده_real_event_s::عملگر جديد (size_t size)
{
      أكد(size == حجم(تخصیص_بده_real_event_s));
      أرجع assignr_انباشته.alloc_slab();
}

فراغ تخصیص_بده_real_event_s::عملگر حذف(فراغ*dptr)
{
      assignr_انباشته.free_slab(dptr);
}

بیعلامت دراز شمار_تخصیص_بده_real_pool(فراغ) { أرجع assignr_انباشته.pool; }

بنية تخصیص_بده_array_کلمه_s  : عام event_s {
      vvp_array_t mem;
      بیعلامت adr;
      vvp_vector4_t val;
      بیعلامت off;
      فراغ پیاپی_پیاپی(فراغ) override;

      ساكن فراغ* عملگر جديد(size_t);
      ساكن فراغ عملگر حذف(فراغ*);
};

فراغ تخصیص_بده_array_کلمه_s::پیاپی_پیاپی(فراغ)
{
      شمار_تخصیص_بده_events += 1;
      mem->مجموعه_کلمه(adr, off, val);
}

ساكن ثابت size_t ARRAY_W_CHUNK_شمار = 8192 / حجم(بنية تخصیص_بده_array_کلمه_s);
ساكن slab_t<حجم(تخصیص_بده_array_کلمه_s),ARRAY_W_CHUNK_شمار> array_w_انباشته;

مضمن فراغ* تخصیص_بده_array_کلمه_s::عملگر جديد (size_t size)
{
      أكد(size == حجم(تخصیص_بده_array_کلمه_s));
      أرجع array_w_انباشته.alloc_slab();
}

فراغ تخصیص_بده_array_کلمه_s::عملگر حذف(فراغ*ptr)
{
      array_w_انباشته.free_slab(ptr);
}

بیعلامت دراز شمار_تخصیص_بده_aword_pool(فراغ) { أرجع array_w_انباشته.pool; }

بنية force_vector4_event_s  : عام event_s {
	/* The default constructor. */
      صریح force_vector4_event_s(ثابت vvp_vector4_t&that): val(that) {
	    net = عدم;
	    پایه = 0;
	    vwid = 0;
	    بعدی_2 = عدم;
      }
	/* Where to do the force. */
      vvp_net_t*net;
	/* Value to force. */
      vvp_vector4_t val;
	/* Offset of the part into the destination. */
      بیعلامت پایه;
	/* Width of the destination vector. */
      بیعلامت vwid;

      فراغ پیاپی_پیاپی(فراغ) override;
      فراغ single_گام_display(فراغ) override;

      ساكن فراغ* عملگر جديد(size_t);
      ساكن فراغ عملگر حذف(فراغ*);
};

فراغ force_vector4_event_s::پیاپی_پیاپی(فراغ)
{
      شمار_تخصیص_بده_events += 1;

      بیعلامت wid = val.size();
      إذا ((پایه + wid) > vwid)
	    wid = vwid - پایه;

	// Make a mask of which bits are to be forced, 0 for unforced
	// bits and 1 for forced bits.
      vvp_vector2_t ماسک (vvp_vector2_t::FILL0, vwid);
      لكل (بیعلامت idx = 0 ; idx < wid ; idx += 1)
	    ماسک.مجموعه_بیت(پایه+idx, 1);

      vvp_vector4_t tmp (vwid, BIT4_Z);

	// vvp_net_t::force_vec4 propagates all the bits of the
	// forced vector value, regardless of the mask. This
	// ensures the unforced bits retain their current value.
      vvp_signal_مقدار*sig = dynamic_cast<vvp_signal_مقدار*>(net->fil);
      أكد(sig);
      sig->vec4_مقدار(tmp);

      tmp.مجموعه_vec(پایه, val);
      net->force_vec4(tmp, ماسک);
}

فراغ force_vector4_event_s::single_گام_display(فراغ)
{
      cerr << "force_vector4_event: Force val=" << val
	   << ", vwid=" << vwid << ", base=" << پایه << پایان_سطر;
}

ساكن ثابت size_t FORCE4_CHUNK_شمار = 8192 / حجم(بنية force_vector4_event_s);
ساكن slab_t<حجم(force_vector4_event_s),FORCE4_CHUNK_شمار> force4_انباشته;

مضمن فراغ* force_vector4_event_s::عملگر جديد(size_t size)
{
      أكد(size == حجم(force_vector4_event_s));
      أرجع force4_انباشته.alloc_slab();
}

فراغ force_vector4_event_s::عملگر حذف(فراغ*dptr)
{
      force4_انباشته.free_slab(dptr);
}

بیعلامت دراز شمار_force4_pool(فراغ) { أرجع force4_انباشته.pool; }

/*
 * This class supports the propagation of vec4 outputs from a
 * vvp_net_t object.
 */
بنية propagate_vector4_event_s : عام event_s {
	/* The default constructor. */
      صریح propagate_vector4_event_s(ثابت vvp_vector4_t&that) : val(that) {
	    net = عدم;
	    بعدی_2 = عدم;
      }
	/* A constructor that makes the val directly. */
      propagate_vector4_event_s(ثابت vvp_vector4_t&that, بیعلامت adr, بیعلامت wid)
      : val(that,adr,wid) {
	    net = عدم;
	    بعدی_2 = عدم;
      }

	/* Propagate the output of this net. */
      vvp_net_t*net;
	/* value to propagate */
      vvp_vector4_t val;
	/* Action */
      فراغ پیاپی_پیاپی(فراغ) override;
      فراغ single_گام_display(فراغ) override;
};

فراغ propagate_vector4_event_s::پیاپی_پیاپی(فراغ)
{
      net->send_vec4(val, 0);
}

فراغ propagate_vector4_event_s::single_گام_display(فراغ)
{
      cerr << "propagate_vector4_event: Propagate val=" << val << پایان_سطر;
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
      فراغ پیاپی_پیاپی(فراغ) override;
      فراغ single_گام_display(فراغ) override;
};

فراغ propagate_real_event_s::پیاپی_پیاپی(فراغ)
{
      net->send_real(val, 0);
}

فراغ propagate_real_event_s::single_گام_display(فراغ)
{
      cerr << "propagate_real_event: Propagate val=" << val << پایان_سطر;
}

بنية تخصیص_بده_array_r_کلمه_s  : عام event_s {
      vvp_array_t mem;
      بیعلامت adr;
      مزدوج val;
      فراغ پیاپی_پیاپی(فراغ) override;

      ساكن فراغ* عملگر جديد(size_t);
      ساكن فراغ عملگر حذف(فراغ*);
};

فراغ تخصیص_بده_array_r_کلمه_s::پیاپی_پیاپی(فراغ)
{
      شمار_تخصیص_بده_events += 1;
      mem->مجموعه_کلمه(adr, val);
}
ساكن ثابت size_t ARRAY_R_W_CHUNK_شمار = 8192 / حجم(بنية تخصیص_بده_array_r_کلمه_s);
ساكن slab_t<حجم(تخصیص_بده_array_r_کلمه_s),ARRAY_R_W_CHUNK_شمار> array_r_w_انباشته;

مضمن فراغ* تخصیص_بده_array_r_کلمه_s::عملگر جديد(size_t size)
{
      أكد(size == حجم(تخصیص_بده_array_r_کلمه_s));
      أرجع array_r_w_انباشته.alloc_slab();
}

فراغ تخصیص_بده_array_r_کلمه_s::عملگر حذف(فراغ*ptr)
{
      array_r_w_انباشته.free_slab(ptr);
}

بیعلامت دراز شمار_تخصیص_بده_arword_pool(فراغ) { أرجع array_r_w_انباشته.pool; }

بنية generic_event_s : عام event_s {
      vvp_gen_event_t obj;
      منطقي delete_obj_when_انجامشده;
      فراغ پیاپی_پیاپی(فراغ) override;
      فراغ single_گام_display(فراغ) override;

      ساكن فراغ* عملگر جديد(size_t);
      ساكن فراغ عملگر حذف(فراغ*);
};

فراغ generic_event_s::پیاپی_پیاپی(فراغ)
{
      شمار_gen_events += 1;
      إذا (obj) {
	    obj->پیاپی_پیاپی();
	    إذا (delete_obj_when_انجامشده)
		  حذف obj;
      }
}

فراغ generic_event_s::single_گام_display(فراغ)
{
      obj->single_گام_display();
}

ساكن ثابت size_t GENERIC_CHUNK_شمار = 131072 / حجم(بنية generic_event_s);
ساكن slab_t<حجم(generic_event_s),GENERIC_CHUNK_شمار> generic_event_انباشته;

مضمن فراغ* generic_event_s::عملگر جديد(size_t size)
{
      أكد(size == حجم(generic_event_s));
      أرجع generic_event_انباشته.alloc_slab();
}

فراغ generic_event_s::عملگر حذف(فراغ*ptr)
{
      generic_event_انباشته.free_slab(ptr);
}

بیعلامت دراز شمار_gen_pool(فراغ) { أرجع generic_event_انباشته.pool; }

/*
** These event_time_s will be required a lot, at high frequency.
** Once allocated, we never free them, but stash them away for next time.
*/


ساكن ثابت size_t زمان_CHUNK_شمار = 8192 / حجم(بنية event_زمان_s);
ساكن slab_t<حجم(event_زمان_s),زمان_CHUNK_شمار> event_زمان_انباشته;

مضمن فراغ* event_زمان_s::عملگر جديد (size_t size)
{
      أكد(size == حجم(بنية event_زمان_s));
      فراغ*ptr = event_زمان_انباشته.alloc_slab();
      أرجع ptr;
}

مضمن فراغ event_زمان_s::عملگر حذف(فراغ*ptr, size_t)
{
      event_زمان_انباشته.free_slab(ptr);
}

بیعلامت دراز شمار_زمان_pool(فراغ) { أرجع event_زمان_انباشته.pool; }

/*
 * This is the head of the list of pending events. This includes all
 * the events that have not been executed yet, and reaches into the
 * future.
 */
ساكن بنية event_زمان_s* sched_list = 0;

/*
 * This is a list of initialization events. The setup puts
 * initializations in this list so that they happen before the
 * simulation as a whole starts. This prevents time-0 triggers of
 * certain events.
 */
ساكن بنية event_s* زمانبندی_init_list = 0;

/*
 * This is the head of the list of final events.
 */
ساكن بنية event_s* زمانبندی_final_list = 0;

/*
 * This flag is true until a VPI task or function finishes the
 * simulation.
 */
ساكن منطقي زمانبندی_runnable = درست;
ساكن منطقي زمانبندی_stopped_flag  = خطأ;
ساكن منطقي زمانبندی_single_گام_flag = خطأ;
ساكن منطقي no_سیگنالها_flag = خطأ;

فراغ زمانبندی_پایان(صحيح)
{
      زمانبندی_runnable = خطأ;
}

فراغ زمانبندی_stop(صحيح)
{
      زمانبندی_stopped_flag = درست;
}

فراغ زمانبندی_single_گام(صحيح)
{
      زمانبندی_single_گام_flag = درست;
}

منطقي زمانبندی_پایانیافته(فراغ)
{
      أرجع !زمانبندی_runnable;
}

منطقي زمانبندی_stopped(فراغ)
{
      أرجع زمانبندی_stopped_flag;
}

خارجي "C" فراغ vvp_no_سیگنالها(فراغ)
{
      no_سیگنالها_flag = درست;
}

/*
 * These are the signal handling infrastructure. The SIGINT signal
 * leads to an implicit $stop. The SIGHUP and SIGTERM signals lead
 * to an implicit $finish.
 */
خارجي منطقي stop_is_پایان;

خارجي "C" فراغ سیگنالها_handler(صحيح signum)
{
#اگر_تعریف_شده __MINGW32__
	// Windows implements the original UNIX semantics for signal,
	// so we have to re-establish the signal handler each time a
	// signal is caught.
      إشارة(signum, &سیگنالها_handler);
#پایان_اگر
      إذا (signum != SIGINT)
	    stop_is_پایان = درست;
      زمانبندی_stopped_flag = درست;
}

ساكن فراغ سیگنالها_capture(فراغ)
{
      إذا (no_سیگنالها_flag)
	   أرجع;
#اگر_تعریف_نشده __MINGW32__
      إشارة(SIGHUP,  &سیگنالها_handler);
#پایان_اگر
      إشارة(SIGINT,  &سیگنالها_handler);
      إشارة(SIGTERM, &سیگنالها_handler);
}

ساكن فراغ سیگنالها_revert(فراغ)
{
      إذا (no_سیگنالها_flag)
	   أرجع;
#اگر_تعریف_نشده __MINGW32__
      إشارة(SIGHUP,  SIG_DFL);
#پایان_اگر
      إشارة(SIGINT,  SIG_DFL);
      إشارة(SIGTERM, SIG_DFL);
}

/*
 * This function puts an event on the end of the pre-simulation event queue.
 */
ساكن فراغ زمانبندی_init_event(بنية event_s*cur)
{
      إذا (زمانبندی_init_list == 0) {
            cur->بعدی_2 = cur;
      } وإلا {
            cur->بعدی_2 = زمانبندی_init_list->بعدی_2;
            زمانبندی_init_list->بعدی_2 = cur;
      }
      زمانبندی_init_list = cur;
}

/*
 * This function puts an event on the end of the post-simulation event queue.
 */
ساكن فراغ زمانبندی_final_event(بنية event_s*cur)
{
      إذا (زمانبندی_final_list == 0) {
            cur->بعدی_2 = cur;
      } وإلا {
            cur->بعدی_2 = زمانبندی_final_list->بعدی_2;
            زمانبندی_final_list->بعدی_2 = cur;
      }
      زمانبندی_final_list = cur;
}

/*
 * This function does all the hard work of putting an event into the
 * event queue. The event delay is taken from the event structure
 * itself, and the structure is placed in the right place in the
 * queue.
 */
تعريفنوع تعداد event_صف_e { SEQ_شروع, SEQ_فعال, SEQ_INACTIVE, SEQ_NBASSIGN,
			     SEQ_RWSYNC, SEQ_ROSYNC, DEL_THREAD } event_صف_t;

ساكن فراغ زمانبندی_event(بنية event_s*cur, vvp_time64_t delay,
			    event_صف_t select_صف)
{
      cur->بعدی_2 = cur;
      بنية event_زمان_s*ctim = sched_list;

      إذا (sched_list == 0) {
	      /* Is the event_time list completely empty? Create the
		 first event_time object. */
	    ctim = جديد بنية event_زمان_s;
	    ctim->delay = delay;
	    ctim->بعدی_2  = 0;
	    sched_list = ctim;

      } وإلا إذا (sched_list->delay > delay) {

	      /* Am I looking for an event before the first event_time?
		 If so, create a new event_time to go in front. */
	    بنية event_زمان_s*tmp = جديد بنية event_زمان_s;
	    tmp->delay = delay;
	    tmp->بعدی_2 = ctim;
	    ctim->delay -= delay;
	    ctim = tmp;
	    sched_list = ctim;

      } وإلا {
	    بنية event_زمان_s*prev = 0;

	    طالما (ctim->بعدی_2 && (ctim->delay < delay)) {
		  delay -= ctim->delay;
		  prev = ctim;
		  ctim = ctim->بعدی_2;
	    }

	    إذا (ctim->delay > delay) {
		  بنية event_زمان_s*tmp = جديد بنية event_زمان_s;
		  tmp->delay = delay;
		  tmp->بعدی_2  = prev->بعدی_2;
		  prev->بعدی_2 = tmp;

		  tmp->بعدی_2->delay -= delay;
		  ctim = tmp;

	    } وإلا إذا (ctim->delay == delay) {

	    } وإلا {
		  أكد(ctim->بعدی_2 == 0);
		  بنية event_زمان_s*tmp = جديد بنية event_زمان_s;
		  tmp->delay = delay - ctim->delay;
		  tmp->بعدی_2 = 0;
		  ctim->بعدی_2 = tmp;

		  ctim = tmp;
	    }
      }

	/* By this point, ctim is the event_time structure that is to
	   receive the event at hand. Put the event in to the
	   appropriate list for the kind of assign we have at hand. */

      بنية event_s** q = 0;

      بدل (select_صف) {
	  حالة SEQ_شروع:
	    q = &ctim->شروع;
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
		  cur->بعدی_2 = (*q)->بعدی_2;
		  (*q)->بعدی_2 = cur;
	    }
	    *q = cur;
      }
}

ساكن فراغ زمانبندی_event_افزودن(بنية event_s*cur)
{
      إذا ((sched_list == 0) || (sched_list->delay > 0)) {
	    زمانبندی_event(cur, 0, SEQ_فعال);
	    أرجع;
      }

      بنية event_زمان_s*ctim = sched_list;

      إذا (ctim->فعال == 0) {
	    cur->بعدی_2 = cur;
	    ctim->فعال = cur;
	    أرجع;
      }

      cur->بعدی_2 = ctim->فعال->بعدی_2;
      ctim->فعال->بعدی_2 = cur;
}

فراغ زمانبندی_vthread(vthread_t thr, vvp_time64_t delay, منطقي افزودن_flag)
{
      بنية vthread_event_s*cur = جديد vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);

      إذا (افزودن_flag && (delay == 0)) {
	      /* Special case: If the delay is 0, the push_flag means
		 I can push this event in front of everything. This is
		 used by the %fork statement, for example, to perform
		 task calls. */
	    زمانبندی_event_افزودن(cur);

      } وإلا {
	    زمانبندی_event(cur, delay, SEQ_فعال);
      }
}

فراغ زمانبندی_t0_trigger(vvp_net_ptr_t ptr)
{
      vvp_vector4_t بیت (1, BIT4_X);
      بنية تخصیص_بده_vector4_event_s*cur = جديد بنية تخصیص_بده_vector4_event_s(بیت);
      cur->ptr = ptr;
      زمانبندی_event(cur, 0, SEQ_INACTIVE);
}

فراغ زمانبندی_inactive(vthread_t thr)
{
      بنية vthread_event_s*cur = جديد vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);
      زمانبندی_event(cur, 0, SEQ_INACTIVE);
}

فراغ زمانبندی_init_vthread(vthread_t thr)
{
      بنية vthread_event_s*cur = جديد vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);

      زمانبندی_init_event(cur);
}

فراغ زمانبندی_final_vthread(vthread_t thr)
{
      بنية vthread_event_s*cur = جديد vthread_event_s;

      cur->thr = thr;
      vthread_mark_final(thr);
      vthread_mark_scheduled(thr);

      زمانبندی_final_event(cur);
}

فراغ زمانبندی_تخصیص_بده_وکتور(vvp_net_ptr_t ptr,
			    بیعلامت پایه, بیعلامت vwid,
			    ثابت vvp_vector4_t&بیت,
			    vvp_time64_t delay)
{
      بنية تخصیص_بده_vector4_event_s*cur = جديد بنية تخصیص_بده_vector4_event_s(بیت);
      cur->ptr = ptr;
      cur->پایه = پایه;
      cur->vwid = vwid;
      زمانبندی_event(cur, delay, SEQ_NBASSIGN);
}

فراغ زمانبندی_force_وکتور(vvp_net_t*net,
			    بیعلامت پایه, بیعلامت vwid,
			    ثابت vvp_vector4_t&بیت,
			    vvp_time64_t delay)
{
      بنية force_vector4_event_s*cur = جديد بنية force_vector4_event_s(بیت);
      cur->net = net;
      cur->پایه = پایه;
      cur->vwid = vwid;
      زمانبندی_event(cur, delay, SEQ_NBASSIGN);
}

فراغ زمانبندی_propagate_وکتور(vvp_net_t*net,
			       vvp_time64_t delay,
			       ثابت vvp_vector4_t&src)
{
      بنية propagate_vector4_event_s*cur
	    = جديد بنية propagate_vector4_event_s(src);
      cur->net = net;
      زمانبندی_event(cur, delay, SEQ_NBASSIGN);
}

// FIXME: This needs to create a non-blocking event, but only one per time slot.
//        Is schedule_event_ or execution actually filtering since the net is
//        already X because this is not triggering?
فراغ زمانبندی_propagate_event(vvp_net_t*net,
                              vvp_time64_t delay)
{
      vvp_vector4_t tmp (1, BIT4_X);
      بنية propagate_vector4_event_s*cur
	    = جديد بنية propagate_vector4_event_s(tmp);
      cur->net = net;
      زمانبندی_event(cur, delay, SEQ_NBASSIGN);
}

فراغ زمانبندی_تخصیص_بده_array_کلمه(vvp_array_t mem,
				بیعلامت کلمه_addr,
				بیعلامت off,
				ثابت vvp_vector4_t&val,
				vvp_time64_t delay)
{
      بنية تخصیص_بده_array_کلمه_s*cur = جديد بنية تخصیص_بده_array_کلمه_s;
      cur->mem = mem;
      cur->adr = کلمه_addr;
      cur->off = off;
      cur->val = val;
      زمانبندی_event(cur, delay, SEQ_NBASSIGN);
}

فراغ زمانبندی_تخصیص_بده_array_کلمه(vvp_array_t mem,
				بیعلامت کلمه_addr,
				مزدوج val,
				vvp_time64_t delay)
{
      بنية تخصیص_بده_array_r_کلمه_s*cur = جديد بنية تخصیص_بده_array_r_کلمه_s;
      cur->mem = mem;
      cur->adr = کلمه_addr;
      cur->val = val;
      زمانبندی_event(cur, delay, SEQ_NBASSIGN);
}

فراغ زمانبندی_مجموعه_وکتور(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت)
{
      بنية تخصیص_بده_vector4_event_s*cur = جديد بنية تخصیص_بده_vector4_event_s(بیت);
      cur->ptr = ptr;
      cur->پایه = 0;
      cur->vwid = 0;
      زمانبندی_event(cur, 0, SEQ_فعال);
}

فراغ زمانبندی_مجموعه_وکتور(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&بیت)
{
      بنية تخصیص_بده_vector8_event_s*cur = جديد بنية تخصیص_بده_vector8_event_s;
      cur->ptr = ptr;
      cur->val = بیت;
      زمانبندی_event(cur, 0, SEQ_فعال);
}

فراغ زمانبندی_مجموعه_وکتور(vvp_net_ptr_t ptr, مزدوج بیت)
{
      بنية تخصیص_بده_real_event_s*cur = جديد بنية تخصیص_بده_real_event_s;
      cur->ptr = ptr;
      cur->val = بیت;
      زمانبندی_event(cur, 0, SEQ_فعال);
}

فراغ زمانبندی_init_وکتور(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت)
{
      بنية تخصیص_بده_vector4_event_s*cur = جديد بنية تخصیص_بده_vector4_event_s(بیت);
      cur->ptr = ptr;
      cur->پایه = 0;
      cur->vwid = 0;
      زمانبندی_init_event(cur);
}

فراغ زمانبندی_init_وکتور(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&بیت)
{
      بنية تخصیص_بده_vector8_event_s*cur = جديد بنية تخصیص_بده_vector8_event_s;
      cur->ptr = ptr;
      cur->val = بیت;
      زمانبندی_init_event(cur);
}

فراغ زمانبندی_init_وکتور(vvp_net_ptr_t ptr, مزدوج بیت)
{
      بنية تخصیص_بده_real_event_s*cur = جديد بنية تخصیص_بده_real_event_s;
      cur->ptr = ptr;
      cur->val = بیت;
      زمانبندی_init_event(cur);
}

فراغ زمانبندی_init_propagate(vvp_net_t*net, vvp_vector4_t بیت)
{
      بنية propagate_vector4_event_s*cur = جديد بنية propagate_vector4_event_s(بیت);
      cur->net = net;
      زمانبندی_init_event(cur);
}

فراغ زمانبندی_init_propagate(vvp_net_t*net, مزدوج بیت)
{
      بنية propagate_real_event_s*cur = جديد بنية propagate_real_event_s;
      cur->net = net;
      cur->val = بیت;
      زمانبندی_init_event(cur);
}

فراغ زمانبندی_del_thr(vthread_t thr)
{
      بنية del_thr_event_s*cur = جديد del_thr_event_s;

      cur->thr = thr;

      زمانبندی_event(cur, 0, DEL_THREAD);
}

فراغ زمانبندی_generic(vvp_gen_event_t obj, vvp_time64_t delay,
		      منطقي sync_flag, منطقي ro_flag, منطقي delete_when_انجامشده)
{
      بنية generic_event_s*cur = جديد generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_انجامشده = delete_when_انجامشده;
      زمانبندی_event(cur, delay,
		      sync_flag? (ro_flag?SEQ_ROSYNC:SEQ_RWSYNC) : SEQ_فعال);

      إذا (sync_flag)
	    vthread_delay_delete();
}

ساكن منطقي sim_started;

فراغ زمانبندی_functor(vvp_gen_event_t obj)
{
      بنية generic_event_s*cur = جديد generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_انجامشده = خطأ;
      إذا (!sim_started) {
            زمانبندی_init_event(cur);
      } وإلا {
            زمانبندی_event(cur, 0, SEQ_فعال);
      }
}

فراغ زمانبندی_at_شروع_از_simtime(vvp_gen_event_t obj, vvp_time64_t delay)
{
      بنية generic_event_s*cur = جديد generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_انجامشده = خطأ;
      زمانبندی_event(cur, delay, SEQ_شروع);
}

/*
 * In the vvp runtime of Icarus Verilog, the SEQ_RWSYNC time step is
 * after all of the non-blocking assignments, so is effectively the
 * same as the ReadWriteSync time.
 */
فراغ زمانبندی_at_انتها_از_simtime(vvp_gen_event_t obj, vvp_time64_t delay)
{
      بنية generic_event_s*cur = جديد generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_انجامشده = خطأ;
      زمانبندی_event(cur, delay, SEQ_RWSYNC);
}

ساكن vvp_time64_t زمانبندی_زمان;
vvp_time64_t زمانبندی_simtime(فراغ)
{ أرجع زمانبندی_زمان; }

ساكن منطقي sim_at_rosync = خطأ;
منطقي زمانبندی_at_rosync(فراغ)
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
ساكن فراغ پیاپی_rosync(بنية event_زمان_s*ctim)
{
      sim_at_rosync = درست;
      طالما (ctim->rosync) {
	    بنية event_s*cur = ctim->rosync->بعدی_2;
	    إذا (cur->بعدی_2 == cur) {
		  ctim->rosync = 0;
	    } وإلا {
		  ctim->rosync->بعدی_2 = cur->بعدی_2;
	    }

	    cur->پیاپی_پیاپی();
	    حذف cur;
      }
      sim_at_rosync = خطأ;

      طالما (ctim->del_thr) {
	    بنية event_s*cur = ctim->del_thr->بعدی_2;
	    إذا (cur->بعدی_2 == cur) {
		  ctim->del_thr = 0;
	    } وإلا {
		  ctim->del_thr->بعدی_2 = cur->بعدی_2;
	    }

	    cur->پیاپی_پیاپی();
	    حذف cur;
      }

      إذا (ctim->فعال || ctim->inactive || ctim->nbassign || ctim->rwsync) {
	    cerr << "SCHEDULER ERROR: read-only sync events "
		 << "created RW events!" << پایان_سطر;
      }
}

فراغ زمانبندی_simulate(فراغ)
{
      منطقي پیاپی_finals;
      sim_started = خطأ;

      زمانبندی_زمان = 0;

      إذا (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute EndOfCompile callbacks\n");
      }

      // Execute end of compile callbacks
      vpi_انتها_از_Compile();

      إذا (verbose_flag) {
	    vpi_mcd_printf(1, " ...propagate initialization events\n");
      }

	// Execute initialization events.
      طالما (زمانبندی_init_list) {
	    بنية event_s*cur = زمانبندی_init_list->بعدی_2;
	    إذا (cur->بعدی_2 == cur) {
		  زمانبندی_init_list = 0;
	    } وإلا {
		  زمانبندی_init_list->بعدی_2 = cur->بعدی_2;
	    }
	    cur->پیاپی_پیاپی();
	    حذف cur;
      }

      sim_started = درست;

      إذا (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute StartOfSim callbacks\n");
      }

      // Execute start of simulation callbacks
      vpi_شروع_از_Sim();

      سیگنالها_capture();

      إذا (verbose_flag) {
	    vpi_mcd_printf(1, " ...run scheduler\n");
      }

      // If there were no compiletf, etc. errors then we are going to
      // process events and when done run the final blocks.
      پیاپی_finals = زمانبندی_runnable;

      إذا (زمانبندی_runnable) طالما (sched_list) {

	    إذا (زمانبندی_stopped_flag) {
		  زمانبندی_stopped_flag = خطأ;
		  stop_handler(0);
		  // You can finish from the debugger without a time change.
		  إذا (!زمانبندی_runnable) اكسر;
		  استمر;
	    }

	      /* ctim is the current time step. */
	    بنية event_زمان_s* ctim = sched_list;

	      /* If the time is advancing, then first run the
		 postponed sync events. Run them all. */
	    إذا (ctim->delay > 0) {

		  إذا (!زمانبندی_runnable) اكسر;
		  زمانبندی_زمان += ctim->delay;
		    /* When the design is being traced (we are emitting
		     * file/line information) also print any time changes. */
		  إذا (show_file_سطر) {
			cerr << "Advancing to simulation time: "
			     << زمانبندی_زمان << پایان_سطر;
		  }
		  ctim->delay = 0;

		  vpi_بعدی_Sim_زمان();
		    // Process the cbAtStartOfSimTime callbacks.
		  طالما (ctim->شروع) {
			بنية event_s*cur = ctim->شروع->بعدی_2;
			إذا (cur->بعدی_2 == cur) {
			      ctim->شروع = 0;
			} وإلا {
			      ctim->شروع->بعدی_2 = cur->بعدی_2;
			}
			cur->پیاپی_پیاپی();
			حذف (cur);
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
				    پیاپی_rosync(ctim);
				    sched_list = ctim->بعدی_2;
				    حذف ctim;
				    استمر;
			      }
			}
		  }
	    }

	      /* Pull the first item off the list. If this is the last
		 cell in the list, then clear the list. Execute that
		 event type, and delete it. */
	    بنية event_s*cur = ctim->فعال->بعدی_2;
	    إذا (cur->بعدی_2 == cur) {
		  ctim->فعال = 0;
	    } وإلا {
		  ctim->فعال->بعدی_2 = cur->بعدی_2;
	    }

	    إذا (زمانبندی_single_گام_flag) {
		  cur->single_گام_display();
		  زمانبندی_stopped_flag = درست;
		  زمانبندی_single_گام_flag = خطأ;
	    }

	    cur->پیاپی_پیاپی();

	    حذف (cur);
      }

	// Execute final events.
      زمانبندی_runnable = پیاپی_finals;
      طالما (زمانبندی_runnable && زمانبندی_final_list) {
	    بنية event_s*cur = زمانبندی_final_list->بعدی_2;
	    إذا (cur->بعدی_2 == cur) {
		  زمانبندی_final_list = 0;
	    } وإلا {
		  زمانبندی_final_list->بعدی_2 = cur->بعدی_2;
	    }
	    cur->پیاپی_پیاپی();
	    حذف cur;
      }

      سیگنالها_revert();

      إذا (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute Postsim callbacks\n");
      }

      // Execute post-simulation callbacks
      vpiPostsim();
#اگر_تعریف_شده CHECK_WITH_VALGRIND
      زمانبندی_delete();
#پایان_اگر
}

#اگر_تعریف_شده CHECK_WITH_VALGRIND
فراغ زمانبندی_delete(فراغ)
{
      vthread_event_انباشته.delete_pool();
      assign4_انباشته.delete_pool();
      assign8_انباشته.delete_pool();
      assignr_انباشته.delete_pool();
      array_w_انباشته.delete_pool();
      array_r_w_انباشته.delete_pool();
      generic_event_انباشته.delete_pool();
      event_زمان_انباشته.delete_pool();
}
#پایان_اگر
