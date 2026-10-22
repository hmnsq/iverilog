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
#جے_تعریف_شدہ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
# include  "ivl_alloc.h"
#جے_انت

ورتوں نطاق std;

بنانشانی لما گنتی_سونپو_events = 0;
بنانشانی لما گنتی_gen_events = 0;
بنانشانی لما گنتی_thread_events = 0;
  // Count the time events (A time cell created)
بنانشانی لما گنتی_ویلا_events = 0;



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
      بنية event_s*اگلا_2;
      مجازی ~event_s() { }
      مجازی فراغ لگاتار_لگاتار(فراغ) =0;

	// Write something about the event to stderr
      مجازی فراغ single_قدم_display(فراغ);

	// Fallback new/delete
      ساكن فراغ*عامل جديد (size_t size) { أرجع ::جديد حرف[size]; }
      ساكن فراغ عامل مٹاؤ(فراغ*ptr)  { ::مٹاؤ[]( static_cast<حرف*>(ptr) ); }
};

فراغ event_s::single_قدم_display(فراغ)
{
      std::cerr << "event_s: Step into event " << typeid(*هذا).name() << std::سطر_انت;
}

بنية event_ویلا_s {
      event_ویلا_s() {
	    گنتی_ویلا_events += 1;
	    delay = 0;
	    شروع = 0;
	    سرگرم = 0;
	    inactive = 0;
	    nbassign = 0;
	    rwsync = 0;
	    rosync = 0;
	    del_thr = 0;
	    اگلا_2 = عدم;
      }
      vvp_time64_t delay;

      بنية event_s*شروع;
      بنية event_s*سرگرم;
      بنية event_s*inactive;
      بنية event_s*nbassign;
      بنية event_s*rwsync;
      بنية event_s*rosync;
      بنية event_s*del_thr;

      بنية event_ویلا_s*اگلا_2;

      ساكن فراغ* عامل جديد (size_t);
      ساكن فراغ عامل مٹاؤ(فراغ*obj, size_t s);
};

vvp_gen_event_s::~vvp_gen_event_s()
{
}

فراغ vvp_gen_event_s::single_قدم_display(فراغ)
{
      cerr << "vvp_gen_event_s: Step into event " << typeid(*هذا).name() << سطر_انت;
}

/*
 * Derived event types
 */
بنية vthread_event_s : عام event_s {
      vthread_t thr;
      فراغ لگاتار_لگاتار(فراغ) override;
      فراغ single_قدم_display(فراغ) override;

      ساكن فراغ* عامل جديد(size_t);
      ساكن فراغ عامل مٹاؤ(فراغ*);
};

فراغ vthread_event_s::لگاتار_لگاتار(فراغ)
{
      گنتی_thread_events += 1;
      vthread_لگاتار(thr);
}

فراغ vthread_event_s::single_قدم_display(فراغ)
{
      __vpiScope*scope = vthread_scope(thr);
      cerr << "vthread_event: Resume thread"
	   << " scope=" << scope->vpi_get_str(vpi_بھریا_Name)
	   << سطر_انت;
}

ساكن ثابت size_t VTHR_CHUNK_گنتی = 8192 / حجم(بنية vthread_event_s);
ساكن slab_t<حجم(vthread_event_s),VTHR_CHUNK_گنتی> vthread_event_ڈھیر;

مضمن فراغ* vthread_event_s::عامل جديد(size_t size)
{
      أكد(size == حجم(vthread_event_s));
      أرجع vthread_event_ڈھیر.alloc_slab();
}

فراغ vthread_event_s::عامل مٹاؤ(فراغ*dptr)
{
      vthread_event_ڈھیر.free_slab(dptr);
}

بنية del_thr_event_s : عام event_s {
      vthread_t thr;
      فراغ لگاتار_لگاتار(فراغ) override;
      فراغ single_قدم_display(فراغ) override;
};

فراغ del_thr_event_s::لگاتار_لگاتار(فراغ)
{
      vthread_delete(thr);
}

فراغ del_thr_event_s::single_قدم_display(فراغ)
{
      __vpiScope*scope = vthread_scope(thr);
      cerr << "del_thr_event: Reap completed thread"
	   << " scope=" << scope->vpi_get_str(vpi_بھریا_Name) << سطر_انت;
}

بنية سونپو_vector4_event_s  : عام event_s {
	/* The default constructor. */
      واضح سونپو_vector4_event_s(ثابت vvp_vector4_t&that) : val(that) {
	    ادھار = 0;
	    vwid = 0;
	    اگلا_2 = عدم;
      }

	/* Where to do the assign. */
      vvp_net_ptr_t ptr;
	/* Value to assign. */
      vvp_vector4_t val;
	/* Offset of the part into the destination. */
      بنانشانی ادھار;
	/* Width of the destination vector. */
      بنانشانی vwid;
      فراغ لگاتار_لگاتار(فراغ) override;
      فراغ single_قدم_display(فراغ) override;

      ساكن فراغ* عامل جديد(size_t);
      ساكن فراغ عامل مٹاؤ(فراغ*);
};

فراغ سونپو_vector4_event_s::لگاتار_لگاتار(فراغ)
{
      گنتی_سونپو_events += 1;
      إذا (vwid > 0)
	    vvp_send_vec4_pv(ptr, val, ادھار, vwid, 0);
      وإلا
	    vvp_send_vec4(ptr, val, 0);
}

فراغ سونپو_vector4_event_s::single_قدم_display(فراغ)
{
      cerr << "assign_vector4_event: Propagate val=" << val
	   << ", vwid=" << vwid << ", base=" << ادھار << سطر_انت;
}

ساكن ثابت size_t ASSIGN4_CHUNK_گنتی = 524288 / حجم(بنية سونپو_vector4_event_s);
ساكن slab_t<حجم(سونپو_vector4_event_s),ASSIGN4_CHUNK_گنتی> assign4_ڈھیر;

مضمن فراغ* سونپو_vector4_event_s::عامل جديد(size_t size)
{
      أكد(size == حجم(سونپو_vector4_event_s));
      أرجع assign4_ڈھیر.alloc_slab();
}

فراغ سونپو_vector4_event_s::عامل مٹاؤ(فراغ*dptr)
{
      assign4_ڈھیر.free_slab(dptr);
}

بنانشانی لما گنتی_assign4_pool(فراغ) { أرجع assign4_ڈھیر.pool; }

بنية سونپو_vector8_event_s  : عام event_s {
      vvp_net_ptr_t ptr;
      vvp_vector8_t val;
      فراغ لگاتار_لگاتار(فراغ) override;
      فراغ single_قدم_display(فراغ) override;

      ساكن فراغ* عامل جديد(size_t);
      ساكن فراغ عامل مٹاؤ(فراغ*);
};

فراغ سونپو_vector8_event_s::لگاتار_لگاتار(فراغ)
{
      گنتی_سونپو_events += 1;
      vvp_send_vec8(ptr, val);
}

فراغ سونپو_vector8_event_s::single_قدم_display(فراغ)
{
      cerr << "assign_vector8_event: Propagate val=" << val << سطر_انت;
}

ساكن ثابت size_t ASSIGN8_CHUNK_گنتی = 8192 / حجم(بنية سونپو_vector8_event_s);
ساكن slab_t<حجم(سونپو_vector8_event_s),ASSIGN8_CHUNK_گنتی> assign8_ڈھیر;

مضمن فراغ* سونپو_vector8_event_s::عامل جديد(size_t size)
{
      أكد(size == حجم(سونپو_vector8_event_s));
      أرجع assign8_ڈھیر.alloc_slab();
}

فراغ سونپو_vector8_event_s::عامل مٹاؤ(فراغ*dptr)
{
      assign8_ڈھیر.free_slab(dptr);
}

بنانشانی لما گنتی_assign8_pool() { أرجع assign8_ڈھیر.pool; }

بنية سونپو_real_event_s  : عام event_s {
      vvp_net_ptr_t ptr;
      مزدوج val;
      فراغ لگاتار_لگاتار(فراغ) override;
      فراغ single_قدم_display(فراغ) override;

      ساكن فراغ* عامل جديد(size_t);
      ساكن فراغ عامل مٹاؤ(فراغ*);
};

فراغ سونپو_real_event_s::لگاتار_لگاتار(فراغ)
{
      گنتی_سونپو_events += 1;
      vvp_send_real(ptr, val, 0);
}

فراغ سونپو_real_event_s::single_قدم_display(فراغ)
{
      cerr << "assign_real_event: Propagate val=" << val << سطر_انت;
}

ساكن ثابت size_t ASSIGNR_CHUNK_گنتی = 8192 / حجم(بنية سونپو_real_event_s);
ساكن slab_t<حجم(سونپو_real_event_s),ASSIGNR_CHUNK_گنتی> assignr_ڈھیر;

مضمن فراغ* سونپو_real_event_s::عامل جديد (size_t size)
{
      أكد(size == حجم(سونپو_real_event_s));
      أرجع assignr_ڈھیر.alloc_slab();
}

فراغ سونپو_real_event_s::عامل مٹاؤ(فراغ*dptr)
{
      assignr_ڈھیر.free_slab(dptr);
}

بنانشانی لما گنتی_سونپو_real_pool(فراغ) { أرجع assignr_ڈھیر.pool; }

بنية سونپو_array_لفظ_s  : عام event_s {
      vvp_array_t mem;
      بنانشانی adr;
      vvp_vector4_t val;
      بنانشانی off;
      فراغ لگاتار_لگاتار(فراغ) override;

      ساكن فراغ* عامل جديد(size_t);
      ساكن فراغ عامل مٹاؤ(فراغ*);
};

فراغ سونپو_array_لفظ_s::لگاتار_لگاتار(فراغ)
{
      گنتی_سونپو_events += 1;
      mem->سیٹ_لفظ(adr, off, val);
}

ساكن ثابت size_t ARRAY_W_CHUNK_گنتی = 8192 / حجم(بنية سونپو_array_لفظ_s);
ساكن slab_t<حجم(سونپو_array_لفظ_s),ARRAY_W_CHUNK_گنتی> array_w_ڈھیر;

مضمن فراغ* سونپو_array_لفظ_s::عامل جديد (size_t size)
{
      أكد(size == حجم(سونپو_array_لفظ_s));
      أرجع array_w_ڈھیر.alloc_slab();
}

فراغ سونپو_array_لفظ_s::عامل مٹاؤ(فراغ*ptr)
{
      array_w_ڈھیر.free_slab(ptr);
}

بنانشانی لما گنتی_سونپو_aword_pool(فراغ) { أرجع array_w_ڈھیر.pool; }

بنية force_vector4_event_s  : عام event_s {
	/* The default constructor. */
      واضح force_vector4_event_s(ثابت vvp_vector4_t&that): val(that) {
	    net = عدم;
	    ادھار = 0;
	    vwid = 0;
	    اگلا_2 = عدم;
      }
	/* Where to do the force. */
      vvp_net_t*net;
	/* Value to force. */
      vvp_vector4_t val;
	/* Offset of the part into the destination. */
      بنانشانی ادھار;
	/* Width of the destination vector. */
      بنانشانی vwid;

      فراغ لگاتار_لگاتار(فراغ) override;
      فراغ single_قدم_display(فراغ) override;

      ساكن فراغ* عامل جديد(size_t);
      ساكن فراغ عامل مٹاؤ(فراغ*);
};

فراغ force_vector4_event_s::لگاتار_لگاتار(فراغ)
{
      گنتی_سونپو_events += 1;

      بنانشانی wid = val.size();
      إذا ((ادھار + wid) > vwid)
	    wid = vwid - ادھار;

	// Make a mask of which bits are to be forced, 0 for unforced
	// bits and 1 for forced bits.
      vvp_vector2_t نقاب (vvp_vector2_t::FILL0, vwid);
      لكل (بنانشانی idx = 0 ; idx < wid ; idx += 1)
	    نقاب.سیٹ_ثنائی_ہندسہ(ادھار+idx, 1);

      vvp_vector4_t tmp (vwid, BIT4_Z);

	// vvp_net_t::force_vec4 propagates all the bits of the
	// forced vector value, regardless of the mask. This
	// ensures the unforced bits retain their current value.
      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*>(net->fil);
      أكد(sig);
      sig->vec4_قدر(tmp);

      tmp.سیٹ_vec(ادھار, val);
      net->force_vec4(tmp, نقاب);
}

فراغ force_vector4_event_s::single_قدم_display(فراغ)
{
      cerr << "force_vector4_event: Force val=" << val
	   << ", vwid=" << vwid << ", base=" << ادھار << سطر_انت;
}

ساكن ثابت size_t FORCE4_CHUNK_گنتی = 8192 / حجم(بنية force_vector4_event_s);
ساكن slab_t<حجم(force_vector4_event_s),FORCE4_CHUNK_گنتی> force4_ڈھیر;

مضمن فراغ* force_vector4_event_s::عامل جديد(size_t size)
{
      أكد(size == حجم(force_vector4_event_s));
      أرجع force4_ڈھیر.alloc_slab();
}

فراغ force_vector4_event_s::عامل مٹاؤ(فراغ*dptr)
{
      force4_ڈھیر.free_slab(dptr);
}

بنانشانی لما گنتی_force4_pool(فراغ) { أرجع force4_ڈھیر.pool; }

/*
 * This class supports the propagation of vec4 outputs from a
 * vvp_net_t object.
 */
بنية propagate_vector4_event_s : عام event_s {
	/* The default constructor. */
      واضح propagate_vector4_event_s(ثابت vvp_vector4_t&that) : val(that) {
	    net = عدم;
	    اگلا_2 = عدم;
      }
	/* A constructor that makes the val directly. */
      propagate_vector4_event_s(ثابت vvp_vector4_t&that, بنانشانی adr, بنانشانی wid)
      : val(that,adr,wid) {
	    net = عدم;
	    اگلا_2 = عدم;
      }

	/* Propagate the output of this net. */
      vvp_net_t*net;
	/* value to propagate */
      vvp_vector4_t val;
	/* Action */
      فراغ لگاتار_لگاتار(فراغ) override;
      فراغ single_قدم_display(فراغ) override;
};

فراغ propagate_vector4_event_s::لگاتار_لگاتار(فراغ)
{
      net->send_vec4(val, 0);
}

فراغ propagate_vector4_event_s::single_قدم_display(فراغ)
{
      cerr << "propagate_vector4_event: Propagate val=" << val << سطر_انت;
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
      فراغ لگاتار_لگاتار(فراغ) override;
      فراغ single_قدم_display(فراغ) override;
};

فراغ propagate_real_event_s::لگاتار_لگاتار(فراغ)
{
      net->send_real(val, 0);
}

فراغ propagate_real_event_s::single_قدم_display(فراغ)
{
      cerr << "propagate_real_event: Propagate val=" << val << سطر_انت;
}

بنية سونپو_array_r_لفظ_s  : عام event_s {
      vvp_array_t mem;
      بنانشانی adr;
      مزدوج val;
      فراغ لگاتار_لگاتار(فراغ) override;

      ساكن فراغ* عامل جديد(size_t);
      ساكن فراغ عامل مٹاؤ(فراغ*);
};

فراغ سونپو_array_r_لفظ_s::لگاتار_لگاتار(فراغ)
{
      گنتی_سونپو_events += 1;
      mem->سیٹ_لفظ(adr, val);
}
ساكن ثابت size_t ARRAY_R_W_CHUNK_گنتی = 8192 / حجم(بنية سونپو_array_r_لفظ_s);
ساكن slab_t<حجم(سونپو_array_r_لفظ_s),ARRAY_R_W_CHUNK_گنتی> array_r_w_ڈھیر;

مضمن فراغ* سونپو_array_r_لفظ_s::عامل جديد(size_t size)
{
      أكد(size == حجم(سونپو_array_r_لفظ_s));
      أرجع array_r_w_ڈھیر.alloc_slab();
}

فراغ سونپو_array_r_لفظ_s::عامل مٹاؤ(فراغ*ptr)
{
      array_r_w_ڈھیر.free_slab(ptr);
}

بنانشانی لما گنتی_سونپو_arword_pool(فراغ) { أرجع array_r_w_ڈھیر.pool; }

بنية generic_event_s : عام event_s {
      vvp_gen_event_t obj;
      منطقي delete_obj_when_پورا;
      فراغ لگاتار_لگاتار(فراغ) override;
      فراغ single_قدم_display(فراغ) override;

      ساكن فراغ* عامل جديد(size_t);
      ساكن فراغ عامل مٹاؤ(فراغ*);
};

فراغ generic_event_s::لگاتار_لگاتار(فراغ)
{
      گنتی_gen_events += 1;
      إذا (obj) {
	    obj->لگاتار_لگاتار();
	    إذا (delete_obj_when_پورا)
		  مٹاؤ obj;
      }
}

فراغ generic_event_s::single_قدم_display(فراغ)
{
      obj->single_قدم_display();
}

ساكن ثابت size_t GENERIC_CHUNK_گنتی = 131072 / حجم(بنية generic_event_s);
ساكن slab_t<حجم(generic_event_s),GENERIC_CHUNK_گنتی> generic_event_ڈھیر;

مضمن فراغ* generic_event_s::عامل جديد(size_t size)
{
      أكد(size == حجم(generic_event_s));
      أرجع generic_event_ڈھیر.alloc_slab();
}

فراغ generic_event_s::عامل مٹاؤ(فراغ*ptr)
{
      generic_event_ڈھیر.free_slab(ptr);
}

بنانشانی لما گنتی_gen_pool(فراغ) { أرجع generic_event_ڈھیر.pool; }

/*
** These event_time_s will be required a lot, at high frequency.
** Once allocated, we never free them, but stash them away for next time.
*/


ساكن ثابت size_t ویلا_CHUNK_گنتی = 8192 / حجم(بنية event_ویلا_s);
ساكن slab_t<حجم(event_ویلا_s),ویلا_CHUNK_گنتی> event_ویلا_ڈھیر;

مضمن فراغ* event_ویلا_s::عامل جديد (size_t size)
{
      أكد(size == حجم(بنية event_ویلا_s));
      فراغ*ptr = event_ویلا_ڈھیر.alloc_slab();
      أرجع ptr;
}

مضمن فراغ event_ویلا_s::عامل مٹاؤ(فراغ*ptr, size_t)
{
      event_ویلا_ڈھیر.free_slab(ptr);
}

بنانشانی لما گنتی_ویلا_pool(فراغ) { أرجع event_ویلا_ڈھیر.pool; }

/*
 * This is the head of the list of pending events. This includes all
 * the events that have not been executed yet, and reaches into the
 * future.
 */
ساكن بنية event_ویلا_s* sched_list = 0;

/*
 * This is a list of initialization events. The setup puts
 * initializations in this list so that they happen before the
 * simulation as a whole starts. This prevents time-0 triggers of
 * certain events.
 */
ساكن بنية event_s* ویلا_سارنی_init_list = 0;

/*
 * This is the head of the list of final events.
 */
ساكن بنية event_s* ویلا_سارنی_final_list = 0;

/*
 * This flag is true until a VPI task or function finishes the
 * simulation.
 */
ساكن منطقي ویلا_سارنی_runnable = سچ;
ساكن منطقي ویلا_سارنی_stopped_flag  = خطأ;
ساكن منطقي ویلا_سارنی_single_قدم_flag = خطأ;
ساكن منطقي no_اشارے_flag = خطأ;

فراغ ویلا_سارنی_انت(صحيح)
{
      ویلا_سارنی_runnable = خطأ;
}

فراغ ویلا_سارنی_stop(صحيح)
{
      ویلا_سارنی_stopped_flag = سچ;
}

فراغ ویلا_سارنی_single_قدم(صحيح)
{
      ویلا_سارنی_single_قدم_flag = سچ;
}

منطقي ویلا_سارنی_مکمل(فراغ)
{
      أرجع !ویلا_سارنی_runnable;
}

منطقي ویلا_سارنی_stopped(فراغ)
{
      أرجع ویلا_سارنی_stopped_flag;
}

خارجي "C" فراغ vvp_no_اشارے(فراغ)
{
      no_اشارے_flag = سچ;
}

/*
 * These are the signal handling infrastructure. The SIGINT signal
 * leads to an implicit $stop. The SIGHUP and SIGTERM signals lead
 * to an implicit $finish.
 */
خارجي منطقي stop_is_انت;

خارجي "C" فراغ اشارے_handler(صحيح signum)
{
#جے_تعریف_شدہ __MINGW32__
	// Windows implements the original UNIX semantics for signal,
	// so we have to re-establish the signal handler each time a
	// signal is caught.
      إشارة(signum, &اشارے_handler);
#جے_انت
      إذا (signum != SIGINT)
	    stop_is_انت = سچ;
      ویلا_سارنی_stopped_flag = سچ;
}

ساكن فراغ اشارے_capture(فراغ)
{
      إذا (no_اشارے_flag)
	   أرجع;
#جے_بنا_تعریف __MINGW32__
      إشارة(SIGHUP,  &اشارے_handler);
#جے_انت
      إشارة(SIGINT,  &اشارے_handler);
      إشارة(SIGTERM, &اشارے_handler);
}

ساكن فراغ اشارے_revert(فراغ)
{
      إذا (no_اشارے_flag)
	   أرجع;
#جے_بنا_تعریف __MINGW32__
      إشارة(SIGHUP,  SIG_DFL);
#جے_انت
      إشارة(SIGINT,  SIG_DFL);
      إشارة(SIGTERM, SIG_DFL);
}

/*
 * This function puts an event on the end of the pre-simulation event queue.
 */
ساكن فراغ ویلا_سارنی_init_event(بنية event_s*cur)
{
      إذا (ویلا_سارنی_init_list == 0) {
            cur->اگلا_2 = cur;
      } وإلا {
            cur->اگلا_2 = ویلا_سارنی_init_list->اگلا_2;
            ویلا_سارنی_init_list->اگلا_2 = cur;
      }
      ویلا_سارنی_init_list = cur;
}

/*
 * This function puts an event on the end of the post-simulation event queue.
 */
ساكن فراغ ویلا_سارنی_final_event(بنية event_s*cur)
{
      إذا (ویلا_سارنی_final_list == 0) {
            cur->اگلا_2 = cur;
      } وإلا {
            cur->اگلا_2 = ویلا_سارنی_final_list->اگلا_2;
            ویلا_سارنی_final_list->اگلا_2 = cur;
      }
      ویلا_سارنی_final_list = cur;
}

/*
 * This function does all the hard work of putting an event into the
 * event queue. The event delay is taken from the event structure
 * itself, and the structure is placed in the right place in the
 * queue.
 */
تعريفنوع تعداد event_قطار_e { SEQ_شروع, SEQ_سرگرم, SEQ_INACTIVE, SEQ_NBASSIGN,
			     SEQ_RWSYNC, SEQ_ROSYNC, DEL_THREAD } event_قطار_t;

ساكن فراغ ویلا_سارنی_event(بنية event_s*cur, vvp_time64_t delay,
			    event_قطار_t select_قطار)
{
      cur->اگلا_2 = cur;
      بنية event_ویلا_s*ctim = sched_list;

      إذا (sched_list == 0) {
	      /* Is the event_time list completely empty? Create the
		 first event_time object. */
	    ctim = جديد بنية event_ویلا_s;
	    ctim->delay = delay;
	    ctim->اگلا_2  = 0;
	    sched_list = ctim;

      } وإلا إذا (sched_list->delay > delay) {

	      /* Am I looking for an event before the first event_time?
		 If so, create a new event_time to go in front. */
	    بنية event_ویلا_s*tmp = جديد بنية event_ویلا_s;
	    tmp->delay = delay;
	    tmp->اگلا_2 = ctim;
	    ctim->delay -= delay;
	    ctim = tmp;
	    sched_list = ctim;

      } وإلا {
	    بنية event_ویلا_s*prev = 0;

	    طالما (ctim->اگلا_2 && (ctim->delay < delay)) {
		  delay -= ctim->delay;
		  prev = ctim;
		  ctim = ctim->اگلا_2;
	    }

	    إذا (ctim->delay > delay) {
		  بنية event_ویلا_s*tmp = جديد بنية event_ویلا_s;
		  tmp->delay = delay;
		  tmp->اگلا_2  = prev->اگلا_2;
		  prev->اگلا_2 = tmp;

		  tmp->اگلا_2->delay -= delay;
		  ctim = tmp;

	    } وإلا إذا (ctim->delay == delay) {

	    } وإلا {
		  أكد(ctim->اگلا_2 == 0);
		  بنية event_ویلا_s*tmp = جديد بنية event_ویلا_s;
		  tmp->delay = delay - ctim->delay;
		  tmp->اگلا_2 = 0;
		  ctim->اگلا_2 = tmp;

		  ctim = tmp;
	    }
      }

	/* By this point, ctim is the event_time structure that is to
	   receive the event at hand. Put the event in to the
	   appropriate list for the kind of assign we have at hand. */

      بنية event_s** q = 0;

      بدل (select_قطار) {
	  حالة SEQ_شروع:
	    q = &ctim->شروع;
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
		  cur->اگلا_2 = (*q)->اگلا_2;
		  (*q)->اگلا_2 = cur;
	    }
	    *q = cur;
      }
}

ساكن فراغ ویلا_سارنی_event_ڈالو(بنية event_s*cur)
{
      إذا ((sched_list == 0) || (sched_list->delay > 0)) {
	    ویلا_سارنی_event(cur, 0, SEQ_سرگرم);
	    أرجع;
      }

      بنية event_ویلا_s*ctim = sched_list;

      إذا (ctim->سرگرم == 0) {
	    cur->اگلا_2 = cur;
	    ctim->سرگرم = cur;
	    أرجع;
      }

      cur->اگلا_2 = ctim->سرگرم->اگلا_2;
      ctim->سرگرم->اگلا_2 = cur;
}

فراغ ویلا_سارنی_vthread(vthread_t thr, vvp_time64_t delay, منطقي ڈالو_flag)
{
      بنية vthread_event_s*cur = جديد vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);

      إذا (ڈالو_flag && (delay == 0)) {
	      /* Special case: If the delay is 0, the push_flag means
		 I can push this event in front of everything. This is
		 used by the %fork statement, for example, to perform
		 task calls. */
	    ویلا_سارنی_event_ڈالو(cur);

      } وإلا {
	    ویلا_سارنی_event(cur, delay, SEQ_سرگرم);
      }
}

فراغ ویلا_سارنی_t0_trigger(vvp_net_ptr_t ptr)
{
      vvp_vector4_t ثنائی_ہندسہ (1, BIT4_X);
      بنية سونپو_vector4_event_s*cur = جديد بنية سونپو_vector4_event_s(ثنائی_ہندسہ);
      cur->ptr = ptr;
      ویلا_سارنی_event(cur, 0, SEQ_INACTIVE);
}

فراغ ویلا_سارنی_inactive(vthread_t thr)
{
      بنية vthread_event_s*cur = جديد vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);
      ویلا_سارنی_event(cur, 0, SEQ_INACTIVE);
}

فراغ ویلا_سارنی_init_vthread(vthread_t thr)
{
      بنية vthread_event_s*cur = جديد vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);

      ویلا_سارنی_init_event(cur);
}

فراغ ویلا_سارنی_final_vthread(vthread_t thr)
{
      بنية vthread_event_s*cur = جديد vthread_event_s;

      cur->thr = thr;
      vthread_mark_final(thr);
      vthread_mark_scheduled(thr);

      ویلا_سارنی_final_event(cur);
}

فراغ ویلا_سارنی_سونپو_سمتیہ(vvp_net_ptr_t ptr,
			    بنانشانی ادھار, بنانشانی vwid,
			    ثابت vvp_vector4_t&ثنائی_ہندسہ,
			    vvp_time64_t delay)
{
      بنية سونپو_vector4_event_s*cur = جديد بنية سونپو_vector4_event_s(ثنائی_ہندسہ);
      cur->ptr = ptr;
      cur->ادھار = ادھار;
      cur->vwid = vwid;
      ویلا_سارنی_event(cur, delay, SEQ_NBASSIGN);
}

فراغ ویلا_سارنی_force_سمتیہ(vvp_net_t*net,
			    بنانشانی ادھار, بنانشانی vwid,
			    ثابت vvp_vector4_t&ثنائی_ہندسہ,
			    vvp_time64_t delay)
{
      بنية force_vector4_event_s*cur = جديد بنية force_vector4_event_s(ثنائی_ہندسہ);
      cur->net = net;
      cur->ادھار = ادھار;
      cur->vwid = vwid;
      ویلا_سارنی_event(cur, delay, SEQ_NBASSIGN);
}

فراغ ویلا_سارنی_propagate_سمتیہ(vvp_net_t*net,
			       vvp_time64_t delay,
			       ثابت vvp_vector4_t&src)
{
      بنية propagate_vector4_event_s*cur
	    = جديد بنية propagate_vector4_event_s(src);
      cur->net = net;
      ویلا_سارنی_event(cur, delay, SEQ_NBASSIGN);
}

// FIXME: This needs to create a non-blocking event, but only one per time slot.
//        Is schedule_event_ or execution actually filtering since the net is
//        already X because this is not triggering?
فراغ ویلا_سارنی_propagate_event(vvp_net_t*net,
                              vvp_time64_t delay)
{
      vvp_vector4_t tmp (1, BIT4_X);
      بنية propagate_vector4_event_s*cur
	    = جديد بنية propagate_vector4_event_s(tmp);
      cur->net = net;
      ویلا_سارنی_event(cur, delay, SEQ_NBASSIGN);
}

فراغ ویلا_سارنی_سونپو_array_لفظ(vvp_array_t mem,
				بنانشانی لفظ_addr,
				بنانشانی off,
				ثابت vvp_vector4_t&val,
				vvp_time64_t delay)
{
      بنية سونپو_array_لفظ_s*cur = جديد بنية سونپو_array_لفظ_s;
      cur->mem = mem;
      cur->adr = لفظ_addr;
      cur->off = off;
      cur->val = val;
      ویلا_سارنی_event(cur, delay, SEQ_NBASSIGN);
}

فراغ ویلا_سارنی_سونپو_array_لفظ(vvp_array_t mem,
				بنانشانی لفظ_addr,
				مزدوج val,
				vvp_time64_t delay)
{
      بنية سونپو_array_r_لفظ_s*cur = جديد بنية سونپو_array_r_لفظ_s;
      cur->mem = mem;
      cur->adr = لفظ_addr;
      cur->val = val;
      ویلا_سارنی_event(cur, delay, SEQ_NBASSIGN);
}

فراغ ویلا_سارنی_سیٹ_سمتیہ(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ)
{
      بنية سونپو_vector4_event_s*cur = جديد بنية سونپو_vector4_event_s(ثنائی_ہندسہ);
      cur->ptr = ptr;
      cur->ادھار = 0;
      cur->vwid = 0;
      ویلا_سارنی_event(cur, 0, SEQ_سرگرم);
}

فراغ ویلا_سارنی_سیٹ_سمتیہ(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&ثنائی_ہندسہ)
{
      بنية سونپو_vector8_event_s*cur = جديد بنية سونپو_vector8_event_s;
      cur->ptr = ptr;
      cur->val = ثنائی_ہندسہ;
      ویلا_سارنی_event(cur, 0, SEQ_سرگرم);
}

فراغ ویلا_سارنی_سیٹ_سمتیہ(vvp_net_ptr_t ptr, مزدوج ثنائی_ہندسہ)
{
      بنية سونپو_real_event_s*cur = جديد بنية سونپو_real_event_s;
      cur->ptr = ptr;
      cur->val = ثنائی_ہندسہ;
      ویلا_سارنی_event(cur, 0, SEQ_سرگرم);
}

فراغ ویلا_سارنی_init_سمتیہ(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ)
{
      بنية سونپو_vector4_event_s*cur = جديد بنية سونپو_vector4_event_s(ثنائی_ہندسہ);
      cur->ptr = ptr;
      cur->ادھار = 0;
      cur->vwid = 0;
      ویلا_سارنی_init_event(cur);
}

فراغ ویلا_سارنی_init_سمتیہ(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&ثنائی_ہندسہ)
{
      بنية سونپو_vector8_event_s*cur = جديد بنية سونپو_vector8_event_s;
      cur->ptr = ptr;
      cur->val = ثنائی_ہندسہ;
      ویلا_سارنی_init_event(cur);
}

فراغ ویلا_سارنی_init_سمتیہ(vvp_net_ptr_t ptr, مزدوج ثنائی_ہندسہ)
{
      بنية سونپو_real_event_s*cur = جديد بنية سونپو_real_event_s;
      cur->ptr = ptr;
      cur->val = ثنائی_ہندسہ;
      ویلا_سارنی_init_event(cur);
}

فراغ ویلا_سارنی_init_propagate(vvp_net_t*net, vvp_vector4_t ثنائی_ہندسہ)
{
      بنية propagate_vector4_event_s*cur = جديد بنية propagate_vector4_event_s(ثنائی_ہندسہ);
      cur->net = net;
      ویلا_سارنی_init_event(cur);
}

فراغ ویلا_سارنی_init_propagate(vvp_net_t*net, مزدوج ثنائی_ہندسہ)
{
      بنية propagate_real_event_s*cur = جديد بنية propagate_real_event_s;
      cur->net = net;
      cur->val = ثنائی_ہندسہ;
      ویلا_سارنی_init_event(cur);
}

فراغ ویلا_سارنی_del_thr(vthread_t thr)
{
      بنية del_thr_event_s*cur = جديد del_thr_event_s;

      cur->thr = thr;

      ویلا_سارنی_event(cur, 0, DEL_THREAD);
}

فراغ ویلا_سارنی_generic(vvp_gen_event_t obj, vvp_time64_t delay,
		      منطقي sync_flag, منطقي ro_flag, منطقي delete_when_پورا)
{
      بنية generic_event_s*cur = جديد generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_پورا = delete_when_پورا;
      ویلا_سارنی_event(cur, delay,
		      sync_flag? (ro_flag?SEQ_ROSYNC:SEQ_RWSYNC) : SEQ_سرگرم);

      إذا (sync_flag)
	    vthread_delay_delete();
}

ساكن منطقي sim_started;

فراغ ویلا_سارنی_functor(vvp_gen_event_t obj)
{
      بنية generic_event_s*cur = جديد generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_پورا = خطأ;
      إذا (!sim_started) {
            ویلا_سارنی_init_event(cur);
      } وإلا {
            ویلا_سارنی_event(cur, 0, SEQ_سرگرم);
      }
}

فراغ ویلا_سارنی_at_شروع_دا_simtime(vvp_gen_event_t obj, vvp_time64_t delay)
{
      بنية generic_event_s*cur = جديد generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_پورا = خطأ;
      ویلا_سارنی_event(cur, delay, SEQ_شروع);
}

/*
 * In the vvp runtime of Icarus Verilog, the SEQ_RWSYNC time step is
 * after all of the non-blocking assignments, so is effectively the
 * same as the ReadWriteSync time.
 */
فراغ ویلا_سارنی_at_سرا_دا_simtime(vvp_gen_event_t obj, vvp_time64_t delay)
{
      بنية generic_event_s*cur = جديد generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_پورا = خطأ;
      ویلا_سارنی_event(cur, delay, SEQ_RWSYNC);
}

ساكن vvp_time64_t ویلا_سارنی_ویلا;
vvp_time64_t ویلا_سارنی_simtime(فراغ)
{ أرجع ویلا_سارنی_ویلا; }

ساكن منطقي sim_at_rosync = خطأ;
منطقي ویلا_سارنی_at_rosync(فراغ)
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
ساكن فراغ لگاتار_rosync(بنية event_ویلا_s*ctim)
{
      sim_at_rosync = سچ;
      طالما (ctim->rosync) {
	    بنية event_s*cur = ctim->rosync->اگلا_2;
	    إذا (cur->اگلا_2 == cur) {
		  ctim->rosync = 0;
	    } وإلا {
		  ctim->rosync->اگلا_2 = cur->اگلا_2;
	    }

	    cur->لگاتار_لگاتار();
	    مٹاؤ cur;
      }
      sim_at_rosync = خطأ;

      طالما (ctim->del_thr) {
	    بنية event_s*cur = ctim->del_thr->اگلا_2;
	    إذا (cur->اگلا_2 == cur) {
		  ctim->del_thr = 0;
	    } وإلا {
		  ctim->del_thr->اگلا_2 = cur->اگلا_2;
	    }

	    cur->لگاتار_لگاتار();
	    مٹاؤ cur;
      }

      إذا (ctim->سرگرم || ctim->inactive || ctim->nbassign || ctim->rwsync) {
	    cerr << "SCHEDULER ERROR: read-only sync events "
		 << "created RW events!" << سطر_انت;
      }
}

فراغ ویلا_سارنی_simulate(فراغ)
{
      منطقي لگاتار_finals;
      sim_started = خطأ;

      ویلا_سارنی_ویلا = 0;

      إذا (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute EndOfCompile callbacks\n");
      }

      // Execute end of compile callbacks
      vpi_سرا_دا_Compile();

      إذا (verbose_flag) {
	    vpi_mcd_printf(1, " ...propagate initialization events\n");
      }

	// Execute initialization events.
      طالما (ویلا_سارنی_init_list) {
	    بنية event_s*cur = ویلا_سارنی_init_list->اگلا_2;
	    إذا (cur->اگلا_2 == cur) {
		  ویلا_سارنی_init_list = 0;
	    } وإلا {
		  ویلا_سارنی_init_list->اگلا_2 = cur->اگلا_2;
	    }
	    cur->لگاتار_لگاتار();
	    مٹاؤ cur;
      }

      sim_started = سچ;

      إذا (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute StartOfSim callbacks\n");
      }

      // Execute start of simulation callbacks
      vpi_شروع_دا_Sim();

      اشارے_capture();

      إذا (verbose_flag) {
	    vpi_mcd_printf(1, " ...run scheduler\n");
      }

      // If there were no compiletf, etc. errors then we are going to
      // process events and when done run the final blocks.
      لگاتار_finals = ویلا_سارنی_runnable;

      إذا (ویلا_سارنی_runnable) طالما (sched_list) {

	    إذا (ویلا_سارنی_stopped_flag) {
		  ویلا_سارنی_stopped_flag = خطأ;
		  stop_handler(0);
		  // You can finish from the debugger without a time change.
		  إذا (!ویلا_سارنی_runnable) اكسر;
		  استمر;
	    }

	      /* ctim is the current time step. */
	    بنية event_ویلا_s* ctim = sched_list;

	      /* If the time is advancing, then first run the
		 postponed sync events. Run them all. */
	    إذا (ctim->delay > 0) {

		  إذا (!ویلا_سارنی_runnable) اكسر;
		  ویلا_سارنی_ویلا += ctim->delay;
		    /* When the design is being traced (we are emitting
		     * file/line information) also print any time changes. */
		  إذا (show_file_سطر) {
			cerr << "Advancing to simulation time: "
			     << ویلا_سارنی_ویلا << سطر_انت;
		  }
		  ctim->delay = 0;

		  vpi_اگلا_Sim_ویلا();
		    // Process the cbAtStartOfSimTime callbacks.
		  طالما (ctim->شروع) {
			بنية event_s*cur = ctim->شروع->اگلا_2;
			إذا (cur->اگلا_2 == cur) {
			      ctim->شروع = 0;
			} وإلا {
			      ctim->شروع->اگلا_2 = cur->اگلا_2;
			}
			cur->لگاتار_لگاتار();
			مٹاؤ (cur);
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
				    لگاتار_rosync(ctim);
				    sched_list = ctim->اگلا_2;
				    مٹاؤ ctim;
				    استمر;
			      }
			}
		  }
	    }

	      /* Pull the first item off the list. If this is the last
		 cell in the list, then clear the list. Execute that
		 event type, and delete it. */
	    بنية event_s*cur = ctim->سرگرم->اگلا_2;
	    إذا (cur->اگلا_2 == cur) {
		  ctim->سرگرم = 0;
	    } وإلا {
		  ctim->سرگرم->اگلا_2 = cur->اگلا_2;
	    }

	    إذا (ویلا_سارنی_single_قدم_flag) {
		  cur->single_قدم_display();
		  ویلا_سارنی_stopped_flag = سچ;
		  ویلا_سارنی_single_قدم_flag = خطأ;
	    }

	    cur->لگاتار_لگاتار();

	    مٹاؤ (cur);
      }

	// Execute final events.
      ویلا_سارنی_runnable = لگاتار_finals;
      طالما (ویلا_سارنی_runnable && ویلا_سارنی_final_list) {
	    بنية event_s*cur = ویلا_سارنی_final_list->اگلا_2;
	    إذا (cur->اگلا_2 == cur) {
		  ویلا_سارنی_final_list = 0;
	    } وإلا {
		  ویلا_سارنی_final_list->اگلا_2 = cur->اگلا_2;
	    }
	    cur->لگاتار_لگاتار();
	    مٹاؤ cur;
      }

      اشارے_revert();

      إذا (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute Postsim callbacks\n");
      }

      // Execute post-simulation callbacks
      vpiPostsim();
#جے_تعریف_شدہ CHECK_WITH_VALGRIND
      ویلا_سارنی_delete();
#جے_انت
}

#جے_تعریف_شدہ CHECK_WITH_VALGRIND
فراغ ویلا_سارنی_delete(فراغ)
{
      vthread_event_ڈھیر.delete_pool();
      assign4_ڈھیر.delete_pool();
      assign8_ڈھیر.delete_pool();
      assignr_ڈھیر.delete_pool();
      array_w_ڈھیر.delete_pool();
      array_r_w_ڈھیر.delete_pool();
      generic_event_ڈھیر.delete_pool();
      event_ویلا_ڈھیر.delete_pool();
}
#جے_انت
