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
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
# include  "ivl_alloc.h"
#اگر_ختم

استعمال نامخلا std;

غیرعلامتی طویل گنتی_تفویض_events = 0;
غیرعلامتی طویل گنتی_gen_events = 0;
غیرعلامتی طویل گنتی_thread_events = 0;
  // Count the time events (A time cell created)
غیرعلامتی طویل گنتی_وقت_events = 0;



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
ساخت event_s {
      ساخت event_s*اگلا_2;
      مجازی ~event_s() { }
      مجازی خالی تسلسل_تسلسل(خالی) =0;

	// Write something about the event to stderr
      مجازی خالی single_قدم_display(خالی);

	// Fallback new/delete
      ساکن خالی*عامل نیا (size_t size) { واپس ::نیا حرف[size]; }
      ساکن خالی عامل حذف(خالی*ptr)  { ::حذف[]( static_cast<حرف*>(ptr) ); }
};

خالی event_s::single_قدم_display(خالی)
{
      std::cerr << "event_s: Step into event " << typeid(*یہ).name() << std::سطر_ختم;
}

ساخت event_وقت_s {
      event_وقت_s() {
	    گنتی_وقت_events += 1;
	    delay = 0;
	    آغاز = 0;
	    فعال = 0;
	    inactive = 0;
	    nbassign = 0;
	    rwsync = 0;
	    rosync = 0;
	    del_thr = 0;
	    اگلا_2 = تہی;
      }
      vvp_time64_t delay;

      ساخت event_s*آغاز;
      ساخت event_s*فعال;
      ساخت event_s*inactive;
      ساخت event_s*nbassign;
      ساخت event_s*rwsync;
      ساخت event_s*rosync;
      ساخت event_s*del_thr;

      ساخت event_وقت_s*اگلا_2;

      ساکن خالی* عامل نیا (size_t);
      ساکن خالی عامل حذف(خالی*obj, size_t s);
};

vvp_gen_event_s::~vvp_gen_event_s()
{
}

خالی vvp_gen_event_s::single_قدم_display(خالی)
{
      cerr << "vvp_gen_event_s: Step into event " << typeid(*یہ).name() << سطر_ختم;
}

/*
 * Derived event types
 */
ساخت vthread_event_s : عوامی event_s {
      vthread_t thr;
      خالی تسلسل_تسلسل(خالی) override;
      خالی single_قدم_display(خالی) override;

      ساکن خالی* عامل نیا(size_t);
      ساکن خالی عامل حذف(خالی*);
};

خالی vthread_event_s::تسلسل_تسلسل(خالی)
{
      گنتی_thread_events += 1;
      vthread_تسلسل(thr);
}

خالی vthread_event_s::single_قدم_display(خالی)
{
      __vpiScope*scope = vthread_scope(thr);
      cerr << "vthread_event: Resume thread"
	   << " scope=" << scope->vpi_get_str(vpi_بھرا_Name)
	   << سطر_ختم;
}

ساکن ثابت size_t VTHR_CHUNK_گنتی = 8192 / حجم(ساخت vthread_event_s);
ساکن slab_t<حجم(vthread_event_s),VTHR_CHUNK_گنتی> vthread_event_ڈھیر;

اندرونی خالی* vthread_event_s::عامل نیا(size_t size)
{
      أكد(size == حجم(vthread_event_s));
      واپس vthread_event_ڈھیر.alloc_slab();
}

خالی vthread_event_s::عامل حذف(خالی*dptr)
{
      vthread_event_ڈھیر.free_slab(dptr);
}

ساخت del_thr_event_s : عوامی event_s {
      vthread_t thr;
      خالی تسلسل_تسلسل(خالی) override;
      خالی single_قدم_display(خالی) override;
};

خالی del_thr_event_s::تسلسل_تسلسل(خالی)
{
      vthread_delete(thr);
}

خالی del_thr_event_s::single_قدم_display(خالی)
{
      __vpiScope*scope = vthread_scope(thr);
      cerr << "del_thr_event: Reap completed thread"
	   << " scope=" << scope->vpi_get_str(vpi_بھرا_Name) << سطر_ختم;
}

ساخت تفویض_vector4_event_s  : عوامی event_s {
	/* The default constructor. */
      صریح تفویض_vector4_event_s(ثابت vvp_vector4_t&that) : val(that) {
	    بنیاد = 0;
	    vwid = 0;
	    اگلا_2 = تہی;
      }

	/* Where to do the assign. */
      vvp_net_ptr_t ptr;
	/* Value to assign. */
      vvp_vector4_t val;
	/* Offset of the part into the destination. */
      غیرعلامتی بنیاد;
	/* Width of the destination vector. */
      غیرعلامتی vwid;
      خالی تسلسل_تسلسل(خالی) override;
      خالی single_قدم_display(خالی) override;

      ساکن خالی* عامل نیا(size_t);
      ساکن خالی عامل حذف(خالی*);
};

خالی تفویض_vector4_event_s::تسلسل_تسلسل(خالی)
{
      گنتی_تفویض_events += 1;
      اگر (vwid > 0)
	    vvp_send_vec4_pv(ptr, val, بنیاد, vwid, 0);
      ورنہ
	    vvp_send_vec4(ptr, val, 0);
}

خالی تفویض_vector4_event_s::single_قدم_display(خالی)
{
      cerr << "assign_vector4_event: Propagate val=" << val
	   << ", vwid=" << vwid << ", base=" << بنیاد << سطر_ختم;
}

ساکن ثابت size_t ASSIGN4_CHUNK_گنتی = 524288 / حجم(ساخت تفویض_vector4_event_s);
ساکن slab_t<حجم(تفویض_vector4_event_s),ASSIGN4_CHUNK_گنتی> assign4_ڈھیر;

اندرونی خالی* تفویض_vector4_event_s::عامل نیا(size_t size)
{
      أكد(size == حجم(تفویض_vector4_event_s));
      واپس assign4_ڈھیر.alloc_slab();
}

خالی تفویض_vector4_event_s::عامل حذف(خالی*dptr)
{
      assign4_ڈھیر.free_slab(dptr);
}

غیرعلامتی طویل گنتی_assign4_pool(خالی) { واپس assign4_ڈھیر.pool; }

ساخت تفویض_vector8_event_s  : عوامی event_s {
      vvp_net_ptr_t ptr;
      vvp_vector8_t val;
      خالی تسلسل_تسلسل(خالی) override;
      خالی single_قدم_display(خالی) override;

      ساکن خالی* عامل نیا(size_t);
      ساکن خالی عامل حذف(خالی*);
};

خالی تفویض_vector8_event_s::تسلسل_تسلسل(خالی)
{
      گنتی_تفویض_events += 1;
      vvp_send_vec8(ptr, val);
}

خالی تفویض_vector8_event_s::single_قدم_display(خالی)
{
      cerr << "assign_vector8_event: Propagate val=" << val << سطر_ختم;
}

ساکن ثابت size_t ASSIGN8_CHUNK_گنتی = 8192 / حجم(ساخت تفویض_vector8_event_s);
ساکن slab_t<حجم(تفویض_vector8_event_s),ASSIGN8_CHUNK_گنتی> assign8_ڈھیر;

اندرونی خالی* تفویض_vector8_event_s::عامل نیا(size_t size)
{
      أكد(size == حجم(تفویض_vector8_event_s));
      واپس assign8_ڈھیر.alloc_slab();
}

خالی تفویض_vector8_event_s::عامل حذف(خالی*dptr)
{
      assign8_ڈھیر.free_slab(dptr);
}

غیرعلامتی طویل گنتی_assign8_pool() { واپس assign8_ڈھیر.pool; }

ساخت تفویض_real_event_s  : عوامی event_s {
      vvp_net_ptr_t ptr;
      دوہرا val;
      خالی تسلسل_تسلسل(خالی) override;
      خالی single_قدم_display(خالی) override;

      ساکن خالی* عامل نیا(size_t);
      ساکن خالی عامل حذف(خالی*);
};

خالی تفویض_real_event_s::تسلسل_تسلسل(خالی)
{
      گنتی_تفویض_events += 1;
      vvp_send_real(ptr, val, 0);
}

خالی تفویض_real_event_s::single_قدم_display(خالی)
{
      cerr << "assign_real_event: Propagate val=" << val << سطر_ختم;
}

ساکن ثابت size_t ASSIGNR_CHUNK_گنتی = 8192 / حجم(ساخت تفویض_real_event_s);
ساکن slab_t<حجم(تفویض_real_event_s),ASSIGNR_CHUNK_گنتی> assignr_ڈھیر;

اندرونی خالی* تفویض_real_event_s::عامل نیا (size_t size)
{
      أكد(size == حجم(تفویض_real_event_s));
      واپس assignr_ڈھیر.alloc_slab();
}

خالی تفویض_real_event_s::عامل حذف(خالی*dptr)
{
      assignr_ڈھیر.free_slab(dptr);
}

غیرعلامتی طویل گنتی_تفویض_real_pool(خالی) { واپس assignr_ڈھیر.pool; }

ساخت تفویض_array_لفظ_s  : عوامی event_s {
      vvp_array_t mem;
      غیرعلامتی adr;
      vvp_vector4_t val;
      غیرعلامتی off;
      خالی تسلسل_تسلسل(خالی) override;

      ساکن خالی* عامل نیا(size_t);
      ساکن خالی عامل حذف(خالی*);
};

خالی تفویض_array_لفظ_s::تسلسل_تسلسل(خالی)
{
      گنتی_تفویض_events += 1;
      mem->سیٹ_لفظ(adr, off, val);
}

ساکن ثابت size_t ARRAY_W_CHUNK_گنتی = 8192 / حجم(ساخت تفویض_array_لفظ_s);
ساکن slab_t<حجم(تفویض_array_لفظ_s),ARRAY_W_CHUNK_گنتی> array_w_ڈھیر;

اندرونی خالی* تفویض_array_لفظ_s::عامل نیا (size_t size)
{
      أكد(size == حجم(تفویض_array_لفظ_s));
      واپس array_w_ڈھیر.alloc_slab();
}

خالی تفویض_array_لفظ_s::عامل حذف(خالی*ptr)
{
      array_w_ڈھیر.free_slab(ptr);
}

غیرعلامتی طویل گنتی_تفویض_aword_pool(خالی) { واپس array_w_ڈھیر.pool; }

ساخت force_vector4_event_s  : عوامی event_s {
	/* The default constructor. */
      صریح force_vector4_event_s(ثابت vvp_vector4_t&that): val(that) {
	    net = تہی;
	    بنیاد = 0;
	    vwid = 0;
	    اگلا_2 = تہی;
      }
	/* Where to do the force. */
      vvp_net_t*net;
	/* Value to force. */
      vvp_vector4_t val;
	/* Offset of the part into the destination. */
      غیرعلامتی بنیاد;
	/* Width of the destination vector. */
      غیرعلامتی vwid;

      خالی تسلسل_تسلسل(خالی) override;
      خالی single_قدم_display(خالی) override;

      ساکن خالی* عامل نیا(size_t);
      ساکن خالی عامل حذف(خالی*);
};

خالی force_vector4_event_s::تسلسل_تسلسل(خالی)
{
      گنتی_تفویض_events += 1;

      غیرعلامتی wid = val.size();
      اگر ((بنیاد + wid) > vwid)
	    wid = vwid - بنیاد;

	// Make a mask of which bits are to be forced, 0 for unforced
	// bits and 1 for forced bits.
      vvp_vector2_t نقاب (vvp_vector2_t::FILL0, vwid);
      برائے (غیرعلامتی idx = 0 ; idx < wid ; idx += 1)
	    نقاب.سیٹ_ثنائی_ہندسہ(بنیاد+idx, 1);

      vvp_vector4_t tmp (vwid, BIT4_Z);

	// vvp_net_t::force_vec4 propagates all the bits of the
	// forced vector value, regardless of the mask. This
	// ensures the unforced bits retain their current value.
      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*>(net->fil);
      أكد(sig);
      sig->vec4_قدر(tmp);

      tmp.سیٹ_vec(بنیاد, val);
      net->force_vec4(tmp, نقاب);
}

خالی force_vector4_event_s::single_قدم_display(خالی)
{
      cerr << "force_vector4_event: Force val=" << val
	   << ", vwid=" << vwid << ", base=" << بنیاد << سطر_ختم;
}

ساکن ثابت size_t FORCE4_CHUNK_گنتی = 8192 / حجم(ساخت force_vector4_event_s);
ساکن slab_t<حجم(force_vector4_event_s),FORCE4_CHUNK_گنتی> force4_ڈھیر;

اندرونی خالی* force_vector4_event_s::عامل نیا(size_t size)
{
      أكد(size == حجم(force_vector4_event_s));
      واپس force4_ڈھیر.alloc_slab();
}

خالی force_vector4_event_s::عامل حذف(خالی*dptr)
{
      force4_ڈھیر.free_slab(dptr);
}

غیرعلامتی طویل گنتی_force4_pool(خالی) { واپس force4_ڈھیر.pool; }

/*
 * This class supports the propagation of vec4 outputs from a
 * vvp_net_t object.
 */
ساخت propagate_vector4_event_s : عوامی event_s {
	/* The default constructor. */
      صریح propagate_vector4_event_s(ثابت vvp_vector4_t&that) : val(that) {
	    net = تہی;
	    اگلا_2 = تہی;
      }
	/* A constructor that makes the val directly. */
      propagate_vector4_event_s(ثابت vvp_vector4_t&that, غیرعلامتی adr, غیرعلامتی wid)
      : val(that,adr,wid) {
	    net = تہی;
	    اگلا_2 = تہی;
      }

	/* Propagate the output of this net. */
      vvp_net_t*net;
	/* value to propagate */
      vvp_vector4_t val;
	/* Action */
      خالی تسلسل_تسلسل(خالی) override;
      خالی single_قدم_display(خالی) override;
};

خالی propagate_vector4_event_s::تسلسل_تسلسل(خالی)
{
      net->send_vec4(val, 0);
}

خالی propagate_vector4_event_s::single_قدم_display(خالی)
{
      cerr << "propagate_vector4_event: Propagate val=" << val << سطر_ختم;
}

/*
 * This class supports the propagation of real outputs from a
 * vvp_net_t object.
 */
ساخت propagate_real_event_s : عوامی event_s {
	/* Propagate the output of this net. */
      vvp_net_t*net;
	/* value to propagate */
      دوہرا val;
	/* Action */
      خالی تسلسل_تسلسل(خالی) override;
      خالی single_قدم_display(خالی) override;
};

خالی propagate_real_event_s::تسلسل_تسلسل(خالی)
{
      net->send_real(val, 0);
}

خالی propagate_real_event_s::single_قدم_display(خالی)
{
      cerr << "propagate_real_event: Propagate val=" << val << سطر_ختم;
}

ساخت تفویض_array_r_لفظ_s  : عوامی event_s {
      vvp_array_t mem;
      غیرعلامتی adr;
      دوہرا val;
      خالی تسلسل_تسلسل(خالی) override;

      ساکن خالی* عامل نیا(size_t);
      ساکن خالی عامل حذف(خالی*);
};

خالی تفویض_array_r_لفظ_s::تسلسل_تسلسل(خالی)
{
      گنتی_تفویض_events += 1;
      mem->سیٹ_لفظ(adr, val);
}
ساکن ثابت size_t ARRAY_R_W_CHUNK_گنتی = 8192 / حجم(ساخت تفویض_array_r_لفظ_s);
ساکن slab_t<حجم(تفویض_array_r_لفظ_s),ARRAY_R_W_CHUNK_گنتی> array_r_w_ڈھیر;

اندرونی خالی* تفویض_array_r_لفظ_s::عامل نیا(size_t size)
{
      أكد(size == حجم(تفویض_array_r_لفظ_s));
      واپس array_r_w_ڈھیر.alloc_slab();
}

خالی تفویض_array_r_لفظ_s::عامل حذف(خالی*ptr)
{
      array_r_w_ڈھیر.free_slab(ptr);
}

غیرعلامتی طویل گنتی_تفویض_arword_pool(خالی) { واپس array_r_w_ڈھیر.pool; }

ساخت generic_event_s : عوامی event_s {
      vvp_gen_event_t obj;
      منطقی delete_obj_when_مکمل;
      خالی تسلسل_تسلسل(خالی) override;
      خالی single_قدم_display(خالی) override;

      ساکن خالی* عامل نیا(size_t);
      ساکن خالی عامل حذف(خالی*);
};

خالی generic_event_s::تسلسل_تسلسل(خالی)
{
      گنتی_gen_events += 1;
      اگر (obj) {
	    obj->تسلسل_تسلسل();
	    اگر (delete_obj_when_مکمل)
		  حذف obj;
      }
}

خالی generic_event_s::single_قدم_display(خالی)
{
      obj->single_قدم_display();
}

ساکن ثابت size_t GENERIC_CHUNK_گنتی = 131072 / حجم(ساخت generic_event_s);
ساکن slab_t<حجم(generic_event_s),GENERIC_CHUNK_گنتی> generic_event_ڈھیر;

اندرونی خالی* generic_event_s::عامل نیا(size_t size)
{
      أكد(size == حجم(generic_event_s));
      واپس generic_event_ڈھیر.alloc_slab();
}

خالی generic_event_s::عامل حذف(خالی*ptr)
{
      generic_event_ڈھیر.free_slab(ptr);
}

غیرعلامتی طویل گنتی_gen_pool(خالی) { واپس generic_event_ڈھیر.pool; }

/*
** These event_time_s will be required a lot, at high frequency.
** Once allocated, we never free them, but stash them away for next time.
*/


ساکن ثابت size_t وقت_CHUNK_گنتی = 8192 / حجم(ساخت event_وقت_s);
ساکن slab_t<حجم(event_وقت_s),وقت_CHUNK_گنتی> event_وقت_ڈھیر;

اندرونی خالی* event_وقت_s::عامل نیا (size_t size)
{
      أكد(size == حجم(ساخت event_وقت_s));
      خالی*ptr = event_وقت_ڈھیر.alloc_slab();
      واپس ptr;
}

اندرونی خالی event_وقت_s::عامل حذف(خالی*ptr, size_t)
{
      event_وقت_ڈھیر.free_slab(ptr);
}

غیرعلامتی طویل گنتی_وقت_pool(خالی) { واپس event_وقت_ڈھیر.pool; }

/*
 * This is the head of the list of pending events. This includes all
 * the events that have not been executed yet, and reaches into the
 * future.
 */
ساکن ساخت event_وقت_s* sched_list = 0;

/*
 * This is a list of initialization events. The setup puts
 * initializations in this list so that they happen before the
 * simulation as a whole starts. This prevents time-0 triggers of
 * certain events.
 */
ساکن ساخت event_s* جدول_وقت_init_list = 0;

/*
 * This is the head of the list of final events.
 */
ساکن ساخت event_s* جدول_وقت_final_list = 0;

/*
 * This flag is true until a VPI task or function finishes the
 * simulation.
 */
ساکن منطقی جدول_وقت_runnable = سچ;
ساکن منطقی جدول_وقت_stopped_flag  = جھوٹ;
ساکن منطقی جدول_وقت_single_قدم_flag = جھوٹ;
ساکن منطقی no_اشارات_flag = جھوٹ;

خالی جدول_وقت_اختتام(صحیح)
{
      جدول_وقت_runnable = جھوٹ;
}

خالی جدول_وقت_stop(صحیح)
{
      جدول_وقت_stopped_flag = سچ;
}

خالی جدول_وقت_single_قدم(صحیح)
{
      جدول_وقت_single_قدم_flag = سچ;
}

منطقی جدول_وقت_ختم_شدہ(خالی)
{
      واپس !جدول_وقت_runnable;
}

منطقی جدول_وقت_stopped(خالی)
{
      واپس جدول_وقت_stopped_flag;
}

خارجی "C" خالی vvp_no_اشارات(خالی)
{
      no_اشارات_flag = سچ;
}

/*
 * These are the signal handling infrastructure. The SIGINT signal
 * leads to an implicit $stop. The SIGHUP and SIGTERM signals lead
 * to an implicit $finish.
 */
خارجی منطقی stop_is_اختتام;

خارجی "C" خالی اشارات_handler(صحیح signum)
{
#اگر_تعریف_شدہ __MINGW32__
	// Windows implements the original UNIX semantics for signal,
	// so we have to re-establish the signal handler each time a
	// signal is caught.
      اشارہ(signum, &اشارات_handler);
#اگر_ختم
      اگر (signum != SIGINT)
	    stop_is_اختتام = سچ;
      جدول_وقت_stopped_flag = سچ;
}

ساکن خالی اشارات_capture(خالی)
{
      اگر (no_اشارات_flag)
	   واپس;
#اگر_غیر_تعریف_شدہ __MINGW32__
      اشارہ(SIGHUP,  &اشارات_handler);
#اگر_ختم
      اشارہ(SIGINT,  &اشارات_handler);
      اشارہ(SIGTERM, &اشارات_handler);
}

ساکن خالی اشارات_revert(خالی)
{
      اگر (no_اشارات_flag)
	   واپس;
#اگر_غیر_تعریف_شدہ __MINGW32__
      اشارہ(SIGHUP,  SIG_DFL);
#اگر_ختم
      اشارہ(SIGINT,  SIG_DFL);
      اشارہ(SIGTERM, SIG_DFL);
}

/*
 * This function puts an event on the end of the pre-simulation event queue.
 */
ساکن خالی جدول_وقت_init_event(ساخت event_s*cur)
{
      اگر (جدول_وقت_init_list == 0) {
            cur->اگلا_2 = cur;
      } ورنہ {
            cur->اگلا_2 = جدول_وقت_init_list->اگلا_2;
            جدول_وقت_init_list->اگلا_2 = cur;
      }
      جدول_وقت_init_list = cur;
}

/*
 * This function puts an event on the end of the post-simulation event queue.
 */
ساکن خالی جدول_وقت_final_event(ساخت event_s*cur)
{
      اگر (جدول_وقت_final_list == 0) {
            cur->اگلا_2 = cur;
      } ورنہ {
            cur->اگلا_2 = جدول_وقت_final_list->اگلا_2;
            جدول_وقت_final_list->اگلا_2 = cur;
      }
      جدول_وقت_final_list = cur;
}

/*
 * This function does all the hard work of putting an event into the
 * event queue. The event delay is taken from the event structure
 * itself, and the structure is placed in the right place in the
 * queue.
 */
قسمتعریف شمار event_قطار_e { SEQ_آغاز, SEQ_فعال, SEQ_INACTIVE, SEQ_NBASSIGN,
			     SEQ_RWSYNC, SEQ_ROSYNC, DEL_THREAD } event_قطار_t;

ساکن خالی جدول_وقت_event(ساخت event_s*cur, vvp_time64_t delay,
			    event_قطار_t select_قطار)
{
      cur->اگلا_2 = cur;
      ساخت event_وقت_s*ctim = sched_list;

      اگر (sched_list == 0) {
	      /* Is the event_time list completely empty? Create the
		 first event_time object. */
	    ctim = نیا ساخت event_وقت_s;
	    ctim->delay = delay;
	    ctim->اگلا_2  = 0;
	    sched_list = ctim;

      } ورنہ اگر (sched_list->delay > delay) {

	      /* Am I looking for an event before the first event_time?
		 If so, create a new event_time to go in front. */
	    ساخت event_وقت_s*tmp = نیا ساخت event_وقت_s;
	    tmp->delay = delay;
	    tmp->اگلا_2 = ctim;
	    ctim->delay -= delay;
	    ctim = tmp;
	    sched_list = ctim;

      } ورنہ {
	    ساخت event_وقت_s*prev = 0;

	    جب (ctim->اگلا_2 && (ctim->delay < delay)) {
		  delay -= ctim->delay;
		  prev = ctim;
		  ctim = ctim->اگلا_2;
	    }

	    اگر (ctim->delay > delay) {
		  ساخت event_وقت_s*tmp = نیا ساخت event_وقت_s;
		  tmp->delay = delay;
		  tmp->اگلا_2  = prev->اگلا_2;
		  prev->اگلا_2 = tmp;

		  tmp->اگلا_2->delay -= delay;
		  ctim = tmp;

	    } ورنہ اگر (ctim->delay == delay) {

	    } ورنہ {
		  أكد(ctim->اگلا_2 == 0);
		  ساخت event_وقت_s*tmp = نیا ساخت event_وقت_s;
		  tmp->delay = delay - ctim->delay;
		  tmp->اگلا_2 = 0;
		  ctim->اگلا_2 = tmp;

		  ctim = tmp;
	    }
      }

	/* By this point, ctim is the event_time structure that is to
	   receive the event at hand. Put the event in to the
	   appropriate list for the kind of assign we have at hand. */

      ساخت event_s** q = 0;

      انتخاب (select_قطار) {
	  حالت SEQ_آغاز:
	    q = &ctim->آغاز;
	    توڑو;

	  حالت SEQ_فعال:
	    q = &ctim->فعال;
	    توڑو;

	  حالت SEQ_INACTIVE:
	    أكد(delay == 0);
	    q = &ctim->inactive;
	    توڑو;

	  حالت SEQ_NBASSIGN:
	    q = &ctim->nbassign;
	    توڑو;

	  حالت SEQ_RWSYNC:
	    q = &ctim->rwsync;
	    توڑو;

	  حالت SEQ_ROSYNC:
	    q = &ctim->rosync;
	    توڑو;

	  حالت DEL_THREAD:
	    q = &ctim->del_thr;
	    توڑو;
      }

      اگر (q) {
	    اگر (*q) {
		  /* Put the cur event on the end of the queue. */
		  cur->اگلا_2 = (*q)->اگلا_2;
		  (*q)->اگلا_2 = cur;
	    }
	    *q = cur;
      }
}

ساکن خالی جدول_وقت_event_ڈالو(ساخت event_s*cur)
{
      اگر ((sched_list == 0) || (sched_list->delay > 0)) {
	    جدول_وقت_event(cur, 0, SEQ_فعال);
	    واپس;
      }

      ساخت event_وقت_s*ctim = sched_list;

      اگر (ctim->فعال == 0) {
	    cur->اگلا_2 = cur;
	    ctim->فعال = cur;
	    واپس;
      }

      cur->اگلا_2 = ctim->فعال->اگلا_2;
      ctim->فعال->اگلا_2 = cur;
}

خالی جدول_وقت_vthread(vthread_t thr, vvp_time64_t delay, منطقی ڈالو_flag)
{
      ساخت vthread_event_s*cur = نیا vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);

      اگر (ڈالو_flag && (delay == 0)) {
	      /* Special case: If the delay is 0, the push_flag means
		 I can push this event in front of everything. This is
		 used by the %fork statement, for example, to perform
		 task calls. */
	    جدول_وقت_event_ڈالو(cur);

      } ورنہ {
	    جدول_وقت_event(cur, delay, SEQ_فعال);
      }
}

خالی جدول_وقت_t0_trigger(vvp_net_ptr_t ptr)
{
      vvp_vector4_t ثنائی_ہندسہ (1, BIT4_X);
      ساخت تفویض_vector4_event_s*cur = نیا ساخت تفویض_vector4_event_s(ثنائی_ہندسہ);
      cur->ptr = ptr;
      جدول_وقت_event(cur, 0, SEQ_INACTIVE);
}

خالی جدول_وقت_inactive(vthread_t thr)
{
      ساخت vthread_event_s*cur = نیا vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);
      جدول_وقت_event(cur, 0, SEQ_INACTIVE);
}

خالی جدول_وقت_init_vthread(vthread_t thr)
{
      ساخت vthread_event_s*cur = نیا vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);

      جدول_وقت_init_event(cur);
}

خالی جدول_وقت_final_vthread(vthread_t thr)
{
      ساخت vthread_event_s*cur = نیا vthread_event_s;

      cur->thr = thr;
      vthread_mark_final(thr);
      vthread_mark_scheduled(thr);

      جدول_وقت_final_event(cur);
}

خالی جدول_وقت_تفویض_سمتیہ(vvp_net_ptr_t ptr,
			    غیرعلامتی بنیاد, غیرعلامتی vwid,
			    ثابت vvp_vector4_t&ثنائی_ہندسہ,
			    vvp_time64_t delay)
{
      ساخت تفویض_vector4_event_s*cur = نیا ساخت تفویض_vector4_event_s(ثنائی_ہندسہ);
      cur->ptr = ptr;
      cur->بنیاد = بنیاد;
      cur->vwid = vwid;
      جدول_وقت_event(cur, delay, SEQ_NBASSIGN);
}

خالی جدول_وقت_force_سمتیہ(vvp_net_t*net,
			    غیرعلامتی بنیاد, غیرعلامتی vwid,
			    ثابت vvp_vector4_t&ثنائی_ہندسہ,
			    vvp_time64_t delay)
{
      ساخت force_vector4_event_s*cur = نیا ساخت force_vector4_event_s(ثنائی_ہندسہ);
      cur->net = net;
      cur->بنیاد = بنیاد;
      cur->vwid = vwid;
      جدول_وقت_event(cur, delay, SEQ_NBASSIGN);
}

خالی جدول_وقت_propagate_سمتیہ(vvp_net_t*net,
			       vvp_time64_t delay,
			       ثابت vvp_vector4_t&src)
{
      ساخت propagate_vector4_event_s*cur
	    = نیا ساخت propagate_vector4_event_s(src);
      cur->net = net;
      جدول_وقت_event(cur, delay, SEQ_NBASSIGN);
}

// FIXME: This needs to create a non-blocking event, but only one per time slot.
//        Is schedule_event_ or execution actually filtering since the net is
//        already X because this is not triggering?
خالی جدول_وقت_propagate_event(vvp_net_t*net,
                              vvp_time64_t delay)
{
      vvp_vector4_t tmp (1, BIT4_X);
      ساخت propagate_vector4_event_s*cur
	    = نیا ساخت propagate_vector4_event_s(tmp);
      cur->net = net;
      جدول_وقت_event(cur, delay, SEQ_NBASSIGN);
}

خالی جدول_وقت_تفویض_array_لفظ(vvp_array_t mem,
				غیرعلامتی لفظ_addr,
				غیرعلامتی off,
				ثابت vvp_vector4_t&val,
				vvp_time64_t delay)
{
      ساخت تفویض_array_لفظ_s*cur = نیا ساخت تفویض_array_لفظ_s;
      cur->mem = mem;
      cur->adr = لفظ_addr;
      cur->off = off;
      cur->val = val;
      جدول_وقت_event(cur, delay, SEQ_NBASSIGN);
}

خالی جدول_وقت_تفویض_array_لفظ(vvp_array_t mem,
				غیرعلامتی لفظ_addr,
				دوہرا val,
				vvp_time64_t delay)
{
      ساخت تفویض_array_r_لفظ_s*cur = نیا ساخت تفویض_array_r_لفظ_s;
      cur->mem = mem;
      cur->adr = لفظ_addr;
      cur->val = val;
      جدول_وقت_event(cur, delay, SEQ_NBASSIGN);
}

خالی جدول_وقت_سیٹ_سمتیہ(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ)
{
      ساخت تفویض_vector4_event_s*cur = نیا ساخت تفویض_vector4_event_s(ثنائی_ہندسہ);
      cur->ptr = ptr;
      cur->بنیاد = 0;
      cur->vwid = 0;
      جدول_وقت_event(cur, 0, SEQ_فعال);
}

خالی جدول_وقت_سیٹ_سمتیہ(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&ثنائی_ہندسہ)
{
      ساخت تفویض_vector8_event_s*cur = نیا ساخت تفویض_vector8_event_s;
      cur->ptr = ptr;
      cur->val = ثنائی_ہندسہ;
      جدول_وقت_event(cur, 0, SEQ_فعال);
}

خالی جدول_وقت_سیٹ_سمتیہ(vvp_net_ptr_t ptr, دوہرا ثنائی_ہندسہ)
{
      ساخت تفویض_real_event_s*cur = نیا ساخت تفویض_real_event_s;
      cur->ptr = ptr;
      cur->val = ثنائی_ہندسہ;
      جدول_وقت_event(cur, 0, SEQ_فعال);
}

خالی جدول_وقت_init_سمتیہ(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ)
{
      ساخت تفویض_vector4_event_s*cur = نیا ساخت تفویض_vector4_event_s(ثنائی_ہندسہ);
      cur->ptr = ptr;
      cur->بنیاد = 0;
      cur->vwid = 0;
      جدول_وقت_init_event(cur);
}

خالی جدول_وقت_init_سمتیہ(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&ثنائی_ہندسہ)
{
      ساخت تفویض_vector8_event_s*cur = نیا ساخت تفویض_vector8_event_s;
      cur->ptr = ptr;
      cur->val = ثنائی_ہندسہ;
      جدول_وقت_init_event(cur);
}

خالی جدول_وقت_init_سمتیہ(vvp_net_ptr_t ptr, دوہرا ثنائی_ہندسہ)
{
      ساخت تفویض_real_event_s*cur = نیا ساخت تفویض_real_event_s;
      cur->ptr = ptr;
      cur->val = ثنائی_ہندسہ;
      جدول_وقت_init_event(cur);
}

خالی جدول_وقت_init_propagate(vvp_net_t*net, vvp_vector4_t ثنائی_ہندسہ)
{
      ساخت propagate_vector4_event_s*cur = نیا ساخت propagate_vector4_event_s(ثنائی_ہندسہ);
      cur->net = net;
      جدول_وقت_init_event(cur);
}

خالی جدول_وقت_init_propagate(vvp_net_t*net, دوہرا ثنائی_ہندسہ)
{
      ساخت propagate_real_event_s*cur = نیا ساخت propagate_real_event_s;
      cur->net = net;
      cur->val = ثنائی_ہندسہ;
      جدول_وقت_init_event(cur);
}

خالی جدول_وقت_del_thr(vthread_t thr)
{
      ساخت del_thr_event_s*cur = نیا del_thr_event_s;

      cur->thr = thr;

      جدول_وقت_event(cur, 0, DEL_THREAD);
}

خالی جدول_وقت_generic(vvp_gen_event_t obj, vvp_time64_t delay,
		      منطقی sync_flag, منطقی ro_flag, منطقی delete_when_مکمل)
{
      ساخت generic_event_s*cur = نیا generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_مکمل = delete_when_مکمل;
      جدول_وقت_event(cur, delay,
		      sync_flag? (ro_flag?SEQ_ROSYNC:SEQ_RWSYNC) : SEQ_فعال);

      اگر (sync_flag)
	    vthread_delay_delete();
}

ساکن منطقی sim_started;

خالی جدول_وقت_functor(vvp_gen_event_t obj)
{
      ساخت generic_event_s*cur = نیا generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_مکمل = جھوٹ;
      اگر (!sim_started) {
            جدول_وقت_init_event(cur);
      } ورنہ {
            جدول_وقت_event(cur, 0, SEQ_فعال);
      }
}

خالی جدول_وقت_at_آغاز_کا_simtime(vvp_gen_event_t obj, vvp_time64_t delay)
{
      ساخت generic_event_s*cur = نیا generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_مکمل = جھوٹ;
      جدول_وقت_event(cur, delay, SEQ_آغاز);
}

/*
 * In the vvp runtime of Icarus Verilog, the SEQ_RWSYNC time step is
 * after all of the non-blocking assignments, so is effectively the
 * same as the ReadWriteSync time.
 */
خالی جدول_وقت_at_انتہا_کا_simtime(vvp_gen_event_t obj, vvp_time64_t delay)
{
      ساخت generic_event_s*cur = نیا generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_مکمل = جھوٹ;
      جدول_وقت_event(cur, delay, SEQ_RWSYNC);
}

ساکن vvp_time64_t جدول_وقت_وقت;
vvp_time64_t جدول_وقت_simtime(خالی)
{ واپس جدول_وقت_وقت; }

ساکن منطقی sim_at_rosync = جھوٹ;
منطقی جدول_وقت_at_rosync(خالی)
{ واپس sim_at_rosync; }

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
ساکن خالی تسلسل_rosync(ساخت event_وقت_s*ctim)
{
      sim_at_rosync = سچ;
      جب (ctim->rosync) {
	    ساخت event_s*cur = ctim->rosync->اگلا_2;
	    اگر (cur->اگلا_2 == cur) {
		  ctim->rosync = 0;
	    } ورنہ {
		  ctim->rosync->اگلا_2 = cur->اگلا_2;
	    }

	    cur->تسلسل_تسلسل();
	    حذف cur;
      }
      sim_at_rosync = جھوٹ;

      جب (ctim->del_thr) {
	    ساخت event_s*cur = ctim->del_thr->اگلا_2;
	    اگر (cur->اگلا_2 == cur) {
		  ctim->del_thr = 0;
	    } ورنہ {
		  ctim->del_thr->اگلا_2 = cur->اگلا_2;
	    }

	    cur->تسلسل_تسلسل();
	    حذف cur;
      }

      اگر (ctim->فعال || ctim->inactive || ctim->nbassign || ctim->rwsync) {
	    cerr << "SCHEDULER ERROR: read-only sync events "
		 << "created RW events!" << سطر_ختم;
      }
}

خالی جدول_وقت_simulate(خالی)
{
      منطقی تسلسل_finals;
      sim_started = جھوٹ;

      جدول_وقت_وقت = 0;

      اگر (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute EndOfCompile callbacks\n");
      }

      // Execute end of compile callbacks
      vpi_انتہا_کا_Compile();

      اگر (verbose_flag) {
	    vpi_mcd_printf(1, " ...propagate initialization events\n");
      }

	// Execute initialization events.
      جب (جدول_وقت_init_list) {
	    ساخت event_s*cur = جدول_وقت_init_list->اگلا_2;
	    اگر (cur->اگلا_2 == cur) {
		  جدول_وقت_init_list = 0;
	    } ورنہ {
		  جدول_وقت_init_list->اگلا_2 = cur->اگلا_2;
	    }
	    cur->تسلسل_تسلسل();
	    حذف cur;
      }

      sim_started = سچ;

      اگر (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute StartOfSim callbacks\n");
      }

      // Execute start of simulation callbacks
      vpi_آغاز_کا_Sim();

      اشارات_capture();

      اگر (verbose_flag) {
	    vpi_mcd_printf(1, " ...run scheduler\n");
      }

      // If there were no compiletf, etc. errors then we are going to
      // process events and when done run the final blocks.
      تسلسل_finals = جدول_وقت_runnable;

      اگر (جدول_وقت_runnable) جب (sched_list) {

	    اگر (جدول_وقت_stopped_flag) {
		  جدول_وقت_stopped_flag = جھوٹ;
		  stop_handler(0);
		  // You can finish from the debugger without a time change.
		  اگر (!جدول_وقت_runnable) توڑو;
		  جاری;
	    }

	      /* ctim is the current time step. */
	    ساخت event_وقت_s* ctim = sched_list;

	      /* If the time is advancing, then first run the
		 postponed sync events. Run them all. */
	    اگر (ctim->delay > 0) {

		  اگر (!جدول_وقت_runnable) توڑو;
		  جدول_وقت_وقت += ctim->delay;
		    /* When the design is being traced (we are emitting
		     * file/line information) also print any time changes. */
		  اگر (show_file_سطر) {
			cerr << "Advancing to simulation time: "
			     << جدول_وقت_وقت << سطر_ختم;
		  }
		  ctim->delay = 0;

		  vpi_اگلا_Sim_وقت();
		    // Process the cbAtStartOfSimTime callbacks.
		  جب (ctim->آغاز) {
			ساخت event_s*cur = ctim->آغاز->اگلا_2;
			اگر (cur->اگلا_2 == cur) {
			      ctim->آغاز = 0;
			} ورنہ {
			      ctim->آغاز->اگلا_2 = cur->اگلا_2;
			}
			cur->تسلسل_تسلسل();
			حذف (cur);
		  }
	    }


	      /* If there are no more active events, advance the event
		 queues. If there are not events at all, then release
		 the event_time object. */
	    اگر (ctim->فعال == 0) {
		  ctim->فعال = ctim->inactive;
		  ctim->inactive = 0;

		  اگر (ctim->فعال == 0) {
			ctim->فعال = ctim->nbassign;
			ctim->nbassign = 0;

			اگر (ctim->فعال == 0) {
			      ctim->فعال = ctim->rwsync;
			      ctim->rwsync = 0;

				/* If out of rw events, then run the rosync
				   events and delete this time step. This also
				   deletes threads as needed. */
			      اگر (ctim->فعال == 0) {
				    تسلسل_rosync(ctim);
				    sched_list = ctim->اگلا_2;
				    حذف ctim;
				    جاری;
			      }
			}
		  }
	    }

	      /* Pull the first item off the list. If this is the last
		 cell in the list, then clear the list. Execute that
		 event type, and delete it. */
	    ساخت event_s*cur = ctim->فعال->اگلا_2;
	    اگر (cur->اگلا_2 == cur) {
		  ctim->فعال = 0;
	    } ورنہ {
		  ctim->فعال->اگلا_2 = cur->اگلا_2;
	    }

	    اگر (جدول_وقت_single_قدم_flag) {
		  cur->single_قدم_display();
		  جدول_وقت_stopped_flag = سچ;
		  جدول_وقت_single_قدم_flag = جھوٹ;
	    }

	    cur->تسلسل_تسلسل();

	    حذف (cur);
      }

	// Execute final events.
      جدول_وقت_runnable = تسلسل_finals;
      جب (جدول_وقت_runnable && جدول_وقت_final_list) {
	    ساخت event_s*cur = جدول_وقت_final_list->اگلا_2;
	    اگر (cur->اگلا_2 == cur) {
		  جدول_وقت_final_list = 0;
	    } ورنہ {
		  جدول_وقت_final_list->اگلا_2 = cur->اگلا_2;
	    }
	    cur->تسلسل_تسلسل();
	    حذف cur;
      }

      اشارات_revert();

      اگر (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute Postsim callbacks\n");
      }

      // Execute post-simulation callbacks
      vpiPostsim();
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
      جدول_وقت_delete();
#اگر_ختم
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
خالی جدول_وقت_delete(خالی)
{
      vthread_event_ڈھیر.delete_pool();
      assign4_ڈھیر.delete_pool();
      assign8_ڈھیر.delete_pool();
      assignr_ڈھیر.delete_pool();
      array_w_ڈھیر.delete_pool();
      array_r_w_ڈھیر.delete_pool();
      generic_event_ڈھیر.delete_pool();
      event_وقت_ڈھیر.delete_pool();
}
#اگر_ختم
