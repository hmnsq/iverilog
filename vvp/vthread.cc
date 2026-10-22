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
# include  "vthread.h"
# include  "codes.h"
# include  "schedule.h"
# include  "ufunc.h"
# include  "event.h"
# include  "vpi_priv.h"
# include  "vvp_net_sig.h"
# include  "vvp_cobject.h"
# include  "vvp_darray.h"
# include  "class_type.h"
#جے_تعریف_شدہ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#جے_انت
# include  <set>
# include  <typeinfo>
# include  <vector>
# include  <cstdlib>
# include  <climits>
# include  <cstring>
# include  <cmath>
# include  <cassert>

# include  <iostream>
# include  <sstream>
# include  <cstdio>

ورتوں نطاق std;

/* This is the size of an unsigned long in bits. This is just a
   convenience macro. */
# define CPU_لفظ_بٹ (8*حجم(بنانشانی لما))
# define چوٹی_ثنائی_ہندسہ (1UL << (CPU_لفظ_بٹ-1))

/*
 * This vthread_s structure describes all there is to know about a
 * thread, including its program counter, all the private bits it
 * holds, and its place in other lists.
 *
 *
 * ** Notes On The Interactions of %fork/%join/%end:
 *
 * The %fork instruction creates a new thread and pushes that into a
 * set of children for the thread. This new thread, then, becomes a
 * child of the current thread, and the current thread a parent of the
 * new thread. Any child can be reaped by a %join.
 *
 * Children that are detached with %join/detach need to have a different
 * parent/child relationship since the parent can still effect them if
 * it uses the %disable/fork or %wait/fork opcodes. The i_am_detached
 * flag and detached_children set are used for this relationship.
 *
 * It is a programming error for a thread that created threads to not
 * %join (or %join/detach) as many as it created before it %ends. The
 * children set will get messed up otherwise.
 *
 * the i_am_joining flag is a clue to children that the parent is
 * blocked in a %join and may need to be scheduled. The %end
 * instruction will check this flag in the parent to see if it should
 * notify the parent that something is interesting.
 *
 * The i_have_ended flag, on the other hand, is used by threads to
 * tell their parents that they are already dead. A thread that
 * executes %end will set its own i_have_ended flag and let its parent
 * reap it when the parent does the %join. If a thread has its
 * schedule_parent_on_end flag set already when it %ends, then it
 * reaps itself and simply schedules its parent. If a child has its
 * i_have_ended flag set when a thread executes %join, then it is free
 * to reap the child immediately.
 */

بنية vthread_s {
      vthread_s();

      فراغ debug_dump(ostream&fd, ثابت حرف*label_متن);

	/* This is the program counter. */
      vvp_کوڈ_t pc;
	/* These hold the private thread bits. */
      تعداد { FLAGS_گنتی = 512, الفاظ_گنتی = 16 };
      vvp_bit4_t flags[FLAGS_گنتی];

	/* These are the word registers. */
      اتحاد {
	    int64_t  w_int;
	    uint64_t w_uint;
      } الفاظ[الفاظ_گنتی];

	// These vectors are depths within the parent thread's
	// corresponding stack.  This is how the %ret/* instructions
	// get at parent thread arguments.
      vector<بنانشانی> دلیلاں_real;
      vector<بنانشانی> دلیلاں_str;
      vector<بنانشانی> دلیلاں_vec4;

    خاص:
      vector<vvp_vector4_t>ڈھیر_vec4;
    عام:
      مضمن vvp_vector4_t نکالو_vec4(فراغ)
      {
	    أكد(! ڈھیر_vec4.empty());
	    vvp_vector4_t val = ڈھیر_vec4.back();
	    ڈھیر_vec4.pop_back();
	    أرجع val;
      }
      مضمن فراغ ڈالو_vec4(ثابت vvp_vector4_t&val)
      {
	    ڈھیر_vec4.push_back(val);
      }
      مضمن ثابت vvp_vector4_t& peek_vec4(بنانشانی ڈونگھائی)
      {
	    بنانشانی size = ڈھیر_vec4.size();
	    أكد(ڈونگھائی < size);
	    بنانشانی use_اشاریہ = size-1-ڈونگھائی;
	    أرجع ڈھیر_vec4[use_اشاریہ];
      }
      مضمن vvp_vector4_t& peek_vec4(فراغ)
      {
	    بنانشانی use_اشاریہ = ڈھیر_vec4.size();
	    أكد(use_اشاریہ >= 1);
	    أرجع ڈھیر_vec4[use_اشاریہ-1];
      }
      مضمن فراغ poke_vec4(بنانشانی ڈونگھائی, ثابت vvp_vector4_t&val)
      {
	    أكد(ڈونگھائی < ڈھیر_vec4.size());
	    بنانشانی use_اشاریہ = ڈھیر_vec4.size()-1-ڈونگھائی;
	    ڈھیر_vec4[use_اشاریہ] = val;
      }
      مضمن فراغ نکالو_vec4(بنانشانی cnt)
      {
	    طالما (cnt > 0) {
		  ڈھیر_vec4.pop_back();
		  cnt -= 1;
	    }
      }


    خاص:
      vector<مزدوج> ڈھیر_real;
    عام:
      مضمن مزدوج نکالو_real(فراغ)
      {
	    أكد(! ڈھیر_real.empty());
	    مزدوج val = ڈھیر_real.back();
	    ڈھیر_real.pop_back();
	    أرجع val;
      }
      مضمن فراغ ڈالو_real(مزدوج val)
      {
	    ڈھیر_real.push_back(val);
      }
      مضمن مزدوج peek_real(بنانشانی ڈونگھائی)
      {
	    أكد(ڈونگھائی < ڈھیر_real.size());
	    بنانشانی use_اشاریہ = ڈھیر_real.size()-1-ڈونگھائی;
	    أرجع ڈھیر_real[use_اشاریہ];
      }
      مضمن فراغ poke_real(بنانشانی ڈونگھائی, مزدوج val)
      {
	    أكد(ڈونگھائی < ڈھیر_real.size());
	    بنانشانی use_اشاریہ = ڈھیر_real.size()-1-ڈونگھائی;
	    ڈھیر_real[use_اشاریہ] = val;
      }
      مضمن فراغ نکالو_real(بنانشانی cnt)
      {
	    طالما (cnt > 0) {
		  ڈھیر_real.pop_back();
		  cnt -= 1;
	    }
      }

	/* Strings are operated on using a forth-like operator
	   set. Items at the top of the stack (back()) are the objects
	   operated on except for special cases. New objects are
	   pushed onto the top (back()) and pulled from the top
	   (back()) only. */
    خاص:
      vector<string> ڈھیر_str;
    عام:
      مضمن string نکالو_str(فراغ)
      {
	    أكد(! ڈھیر_str.empty());
	    string val = ڈھیر_str.back();
	    ڈھیر_str.pop_back();
	    أرجع val;
      }
      مضمن فراغ ڈالو_str(ثابت string&val)
      {
	    ڈھیر_str.push_back(val);
      }
      مضمن string&peek_str(بنانشانی ڈونگھائی)
      {
	    أكد(ڈونگھائی<ڈھیر_str.size());
	    بنانشانی use_اشاریہ = ڈھیر_str.size()-1-ڈونگھائی;
	    أرجع ڈھیر_str[use_اشاریہ];
      }
      مضمن فراغ poke_str(بنانشانی ڈونگھائی, ثابت string&val)
      {
	    أكد(ڈونگھائی < ڈھیر_str.size());
	    بنانشانی use_اشاریہ = ڈھیر_str.size()-1-ڈونگھائی;
	    ڈھیر_str[use_اشاریہ] = val;
      }
      مضمن فراغ نکالو_str(بنانشانی cnt)
      {
	    طالما (cnt > 0) {
		  ڈھیر_str.pop_back();
		  cnt -= 1;
	    }
      }

	/* Objects are also operated on in a stack. */
    خاص:
      تعداد { ڈھیر_OBJ_MAX_ناپ = 32 };
      vvp_object_t ڈھیر_obj[ڈھیر_OBJ_MAX_ناپ];
      بنانشانی ڈھیر_obj_ناپ;
    عام:
      مضمن vvp_object_t& peek_object(فراغ)
      {
	    أكد(ڈھیر_obj_ناپ > 0);
	    أرجع ڈھیر_obj[ڈھیر_obj_ناپ-1];
      }
      مضمن فراغ نکالو_object(vvp_object_t&obj)
      {
	    أكد(ڈھیر_obj_ناپ > 0);
	    ڈھیر_obj_ناپ -= 1;
	    obj = ڈھیر_obj[ڈھیر_obj_ناپ];
	    ڈھیر_obj[ڈھیر_obj_ناپ].reset(0);
      }
      مضمن فراغ نکالو_object(بنانشانی cnt, بنانشانی skip =0)
      {
	    أكد((cnt+skip) <= ڈھیر_obj_ناپ);
	    لكل (size_t idx = ڈھیر_obj_ناپ-skip-cnt ; idx < ڈھیر_obj_ناپ-skip ; idx += 1)
		  ڈھیر_obj[idx].reset(0);
	    ڈھیر_obj_ناپ -= cnt;
	    لكل (size_t idx = ڈھیر_obj_ناپ-skip ; idx < ڈھیر_obj_ناپ ; idx += 1)
		  ڈھیر_obj[idx] = ڈھیر_obj[idx+skip];
	    لكل (size_t idx = ڈھیر_obj_ناپ ; idx < ڈھیر_obj_ناپ+skip ; idx += 1)
		  ڈھیر_obj[idx].reset(0);
      }
      مضمن فراغ ڈالو_object(ثابت vvp_object_t&obj)
      {
	    أكد(ڈھیر_obj_ناپ < ڈھیر_OBJ_MAX_ناپ);
	    ڈھیر_obj[ڈھیر_obj_ناپ] = obj;
	    ڈھیر_obj_ناپ += 1;
      }

	/* My parent sets this when it wants me to wake it up. */
      بنانشانی i_am_joining      :1;
      بنانشانی i_am_detached     :1;
      بنانشانی i_am_اڈیک      :1;
      بنانشانی i_am_وچ_function  :1; // True if running function code
      بنانشانی i_have_ended      :1;
      بنانشانی i_was_disabled    :1;
      بنانشانی اڈیک_for_event :1;
      بنانشانی is_scheduled      :1;
      بنانشانی delay_delete      :1;
	/* This points to the children of the thread. */
      سیٹ<بنية vthread_s*>children;
	/* This points to the detached children of the thread. */
      سیٹ<بنية vthread_s*>detached_children;
	/* This points to my parent, if I have one. */
      بنية vthread_s*ماپا;
	/* This points to the containing scope. */
      __vpiScope*ماپا_scope;
	/* This is used for keeping wait queues. */
      بنية vthread_s*wait_اگلا;
	/* These are used to access automatically allocated items. */
      vvp_context_t wt_context, rd_context;
	/* These are used to pass non-blocking event control information. */
      vvp_net_t*event;
      uint64_t ecount;
	/* Save the file/line information when available. */
    خاص:
      حرف *filenm_;
      بنانشانی lineno_;
    عام:
      فراغ سیٹ_fileline(حرف *filenm, بنانشانی lineno);
      string get_fileline();

      مضمن فراغ cleanup()
      {
	    إذا (i_was_disabled) {
		  ڈھیر_vec4.clear();
		  ڈھیر_real.clear();
		  ڈھیر_str.clear();
		  نکالو_object(ڈھیر_obj_ناپ);
	    }
	    حرر(filenm_);
	    filenm_ = 0;
	    أكد(ڈھیر_vec4.empty());
	    أكد(ڈھیر_real.empty());
	    أكد(ڈھیر_str.empty());
	    أكد(ڈھیر_obj_ناپ == 0);
      }
};

مضمن vthread_s::vthread_s()
{
      ڈھیر_obj_ناپ = 0;
      filenm_ = 0;
      lineno_ = 0;
}

فراغ vthread_s::سیٹ_fileline(حرف *filenm, بنانشانی lineno)
{
      أكد(filenm);
      إذا (!filenm_ || (strcmp(filenm_, filenm) != 0)) {
	    حرر(filenm_);
	    filenm_ = strdup(filenm);
      }
      lineno_ = lineno;
}

مضمن string vthread_s::get_fileline()
{
      ostringstream buf;
      إذا (filenm_) {
	    buf << filenm_ << ":" << lineno_ << ": ";
      }
      string res = buf.str();
      أرجع res;
}

فراغ vthread_s::debug_dump(ostream&fd, ثابت حرف*label)
{
      fd << "**** " << label << سطر_انت;
      fd << "**** ThreadId: " << هذا << ", parent id: " << ماپا << سطر_انت;

      fd << "**** Flags: ";
      لكل (صحيح idx = 0 ; idx < FLAGS_گنتی ; idx += 1)
	    fd << flags[idx];
      fd << سطر_انت;
      fd << "**** vec4 stack..." << سطر_انت;
      لكل (size_t idx = ڈھیر_vec4.size() ; idx > 0 ; idx -= 1)
	    fd << "    " << (ڈھیر_vec4.size()-idx) << ": " << ڈھیر_vec4[idx-1] << سطر_انت;
      fd << "**** str stack (" << ڈھیر_str.size() << ")..." << سطر_انت;
      fd << "**** obj stack (" << ڈھیر_obj_ناپ << ")..." << سطر_انت;
      fd << "**** args_vec4 array (" << دلیلاں_vec4.size() << ")..." << سطر_انت;
      لكل (size_t idx = 0 ; idx < دلیلاں_vec4.size() ; idx += 1)
	    fd << "    " << idx << ": " << دلیلاں_vec4[idx] << سطر_انت;
      fd << "**** file/line (";
      إذا (filenm_) fd << filenm_;
      وإلا fd << "<no file name>";
      fd << ":" << lineno_ << ")" << سطر_انت;
      fd << "**** Done ****" << سطر_انت;
}

/*
 * This function converts the text format of the string by interpreting
 * any octal characters (\nnn) to their single byte value. We do this here
 * because the text value in the vvp_code_t is stored as a C string. This
 * converts it to a C++ string that can hold binary values. We only have
 * to handle the octal escapes because the main compiler takes care of all
 * the other string special characters and normalizes the strings to use
 * only this format.
 */
ساكن string filter_string(ثابت حرف*متن)
{
      vector<حرف> tmp (لڑی_لمبائی(متن)+1);
      size_t dst = 0;
      لكل (ثابت حرف*ptr = متن ; *ptr ; ptr += 1) {
	    // Not an escape? Move on.
	    إذا (*ptr != '\\') {
		  tmp[dst++] = *ptr;
		  استمر;
	    }

	    // Now we know that *ptr is pointing to a \ character and we
	    // have an octal sequence coming up. Advance the ptr and start
	    // processing octal digits.
	    ptr += 1;
	    إذا (*ptr == 0)
		  اكسر;

	    حرف byte = 0;
	    صحيح cnt = 3;
	    طالما (*ptr && cnt > 0 && *ptr >= '0' && *ptr <= '7') {
		  byte *= 8;
		  byte += *ptr - '0';
		  cnt -= 1;
		  ptr += 1;
	    }

	    // null-bytes are supposed to be removed when assigning a string
	    // literal to a string.
	    إذا (byte != '\0')
		  tmp[dst++] = byte;

	    // After the while loop above, the ptr points to the next character,
	    // but the for-loop condition is assuming that ptr points to the last
	    // character, since it has the ptr+=1.
	    ptr -= 1;
      }

      // Put a nul byte at the end of the built up string, but really we are
      // using the known length in the string constructor.
      tmp[dst] = 0;
      string res (&tmp[0], dst);
      أرجع res;
}

ساكن فراغ do_join(vthread_t thr, vthread_t بچہ);

__vpiScope* vthread_scope(بنية vthread_s*thr)
{
      أرجع thr->ماپا_scope;
}

بنية vthread_s*running_thread = 0;

string get_fileline()
{
      أرجع running_thread->get_fileline();
}

فراغ vthread_ڈالو(بنية vthread_s*thr, مزدوج val)
{
      thr->ڈالو_real(val);
}

فراغ vthread_ڈالو(بنية vthread_s*thr, ثابت string&val)
{
      thr->ڈالو_str(val);
}

فراغ vthread_ڈالو(بنية vthread_s*thr, ثابت vvp_vector4_t&val)
{
      thr->ڈالو_vec4(val);
}

فراغ vthread_نکالو_real(بنية vthread_s*thr, بنانشانی ڈونگھائی)
{
      thr->نکالو_real(ڈونگھائی);
}

فراغ vthread_نکالو_str(بنية vthread_s*thr, بنانشانی ڈونگھائی)
{
      thr->نکالو_str(ڈونگھائی);
}

فراغ vthread_نکالو_vec4(بنية vthread_s*thr, بنانشانی ڈونگھائی)
{
      thr->نکالو_vec4(ڈونگھائی);
}

مزدوج vthread_get_real_ڈھیر(بنية vthread_s*thr, بنانشانی ڈونگھائی)
{
      أرجع thr->peek_real(ڈونگھائی);
}

ثابت string&vthread_get_str_ڈھیر(بنية vthread_s*thr, بنانشانی ڈونگھائی)
{
      أرجع thr->peek_str(ڈونگھائی);
}

ثابت vvp_vector4_t& vthread_get_vec4_ڈھیر(بنية vthread_s*thr, بنانشانی ڈونگھائی)
{
      أرجع thr->peek_vec4(ڈونگھائی);
}

/*
 * Some thread management functions
 */
/*
 * This is a function to get a vvp_queue handle from the variable
 * referenced by "net". If the queue is nil, then allocated it and
 * assign the value to the net. Note that this function is
 * parameterized by the queue type so that we can create the right
 * derived type of queue object.
 */
سانچا <صنف VVP_قطار> ساكن vvp_قطار*get_قطار_object(vthread_t thr, vvp_net_t*net)
{
      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      أكد(obj);

      vvp_قطار*قطار = obj->get_object().peek<vvp_قطار>();
      إذا (قطار == 0) {
	    أكد(obj->get_object().test_nil());
	    قطار = جديد VVP_قطار;
	    vvp_object_t val (قطار);
	    vvp_net_ptr_t ptr (net, 0);
	    vvp_send_object(ptr, val, thr->wt_context);
      }

      أرجع قطار;
}

/*
 * Locator and size/word opcodes use the same VVP net for a queue ($) or a
 * dynamic array ([]). Queues are vvp_queue_vec4; dynamic arrays may be
 * vvp_darray_vec4, vvp_darray_atom<>, etc. (all vvp_darray, but vvp_queue
 * also derives from vvp_darray — test queue vec4 first).
 */
ساكن فراغ get_قطار_or_darray_vec4_from_net(vthread_t thr, vvp_net_t* net,
					      vvp_قطار_vec4*& qsrc,
					      vvp_darray*& dsrc)
{
      vvp_fun_signal_object* obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      أكد(obj);
      vvp_object_t چوٹی = obj->get_object();
      qsrc = چوٹی.peek<vvp_قطار_vec4>();
      إذا (qsrc) {
	    dsrc = 0;
	    أرجع;
      }
      vvp_darray* dany = چوٹی.peek<vvp_darray>();
      إذا (dany && dynamic_cast<vvp_قطار*>(dany) == 0) {
	    dsrc = dany;
	    أرجع;
      }
      إذا (چوٹی.test_nil()) {
	    qsrc = جديد vvp_قطار_vec4();
	    vvp_object_t val(qsrc);
	    vvp_net_ptr_t ptr(net, 0);
	    vvp_send_object(ptr, val, thr->wt_context);
	    dsrc = 0;
	    أرجع;
      }
      /* e.g. real/string queue — not vec4 queue or plain darray */
      qsrc = 0;
      dsrc = 0;
}

ساكن فراغ get_قطار_or_darray_real_from_net(vthread_t thr, vvp_net_t* net,
					      vvp_قطار_real*& qsrc,
					      vvp_darray*& dsrc)
{
      vvp_fun_signal_object* obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      أكد(obj);
      vvp_object_t چوٹی = obj->get_object();
      qsrc = چوٹی.peek<vvp_قطار_real>();
      إذا (qsrc) {
	    dsrc = 0;
	    أرجع;
      }
      vvp_darray* dany = چوٹی.peek<vvp_darray>();
      إذا (dany && dynamic_cast<vvp_قطار*>(dany) == 0) {
	    dsrc = dany;
	    أرجع;
      }
      إذا (چوٹی.test_nil()) {
	    qsrc = جديد vvp_قطار_real();
	    vvp_object_t val(qsrc);
	    vvp_net_ptr_t ptr(net, 0);
	    vvp_send_object(ptr, val, thr->wt_context);
	    dsrc = 0;
	    أرجع;
      }
      /* e.g. real/string queue — not vec4 queue or plain darray */
      qsrc = 0;
      dsrc = 0;
}

ساكن فراغ get_قطار_or_darray_vec4_from_object(vvp_object_t src_obj,
						 vvp_قطار_vec4*& qsrc,
						 vvp_darray*& dsrc)
{
      qsrc = src_obj.peek<vvp_قطار_vec4>();
      إذا (qsrc) {
	    dsrc = 0;
	    أرجع;
      }
      vvp_darray* dany = src_obj.peek<vvp_darray>();
      إذا (dany && dynamic_cast<vvp_قطار*>(dany) == 0) {
	    dsrc = dany;
	    أرجع;
      }
      qsrc = 0;
      dsrc = 0;
}

/*
 * The following are used to allow a common template to be written for
 * queue real/string/vec4 operations
 */
مضمن ساكن فراغ نکالو_قدر(vthread_t thr, مزدوج&value, بنانشانی)
{
      value = thr->نکالو_real();
}

مضمن ساكن فراغ نکالو_قدر(vthread_t thr, string&value, بنانشانی)
{
      value = thr->نکالو_str();
}

مضمن ساكن فراغ نکالو_قدر(vthread_t thr, vvp_vector4_t&value, بنانشانی wid)
{
      value = thr->نکالو_vec4();
      أكد(value.size() == wid);
}

/*
 * The following are used to allow the queue templates to print correctly.
 */
مضمن ساكن string get_قطار_type(مزدوج&)
{
      أرجع "queue<real>";
}

مضمن ساكن string get_قطار_type(string&)
{
      أرجع "queue<string>";
}

مضمن ساكن string get_قطار_type(ثابت vvp_vector4_t&value)
{
      ostringstream buf;
      buf << "queue<vector[" << value.size() << "]>";
      string res = buf.str();
      أرجع res;
}

مضمن ساكن فراغ print_قطار_قدر(مزدوج value)
{
      cerr << value;
}

مضمن ساكن فراغ print_قطار_قدر(ثابت string&value)
{
      cerr << "\"" << value << "\"";
}

مضمن ساكن فراغ print_قطار_قدر(ثابت vvp_vector4_t&value)
{
      cerr << value;
}

/*
 * The following are used to get a darray/queue default value.
 */
مضمن ساكن فراغ dq_default(مزدوج&value, بنانشانی)
{
      value = 0.0;
}

مضمن ساكن فراغ dq_default(string&value, بنانشانی)
{
      value = "";
}

مضمن ساكن فراغ dq_default(vvp_vector4_t&value, بنانشانی wid)
{
      value = vvp_vector4_t(wid);
}


سانچا <صنف T> T coerce_منزل_چوڑائی(ثابت T&that, بنانشانی width)
{
      إذا (that.size() == width)
	    أرجع that;

      أكد(that.size() > width);
      T res (width);
      لكل (بنانشانی idx = 0 ;  idx < width ;  idx += 1)
	    res.سیٹ_ثنائی_ہندسہ(idx, that.value(idx));

      أرجع res;
}

/* Explicitly define the vvp_vector4_t version of coerce_to_width(). */
سانچا vvp_vector4_t coerce_منزل_چوڑائی(ثابت vvp_vector4_t&that,
                                       بنانشانی width);


ساكن فراغ multiply_array_imm(بنانشانی لما*res, ثابت بنانشانی لما*val,
			       بنانشانی الفاظ, بنانشانی لما imm)
{
      لكل (بنانشانی idx = 0 ; idx < الفاظ ; idx += 1)
	    res[idx] = 0;

      لكل (بنانشانی mul_idx = 0 ; mul_idx < الفاظ ; mul_idx += 1) {
	    بنانشانی لما جوڑ;
	    بنانشانی لما tmp = multiply_with_حاصل(val[mul_idx], imm, جوڑ);

	    بنانشانی لما حاصل = 0;
	    res[mul_idx] = جوڑو_with_حاصل(res[mul_idx], tmp, حاصل);
	    لكل (بنانشانی جوڑو_idx = mul_idx+1 ; جوڑو_idx < الفاظ ; جوڑو_idx += 1) {
		  res[جوڑو_idx] = جوڑو_with_حاصل(res[جوڑو_idx], جوڑ, حاصل);
		  جوڑ = 0;
	    }
      }
}

/*
 * Allocate a context for use by a child thread. By preference, use
 * the last freed context. If none available, create a new one. Add
 * it to the list of live contexts in that scope.
 */
ساكن vvp_context_t vthread_alloc_context(__vpiScope*scope)
{
      أكد(scope->is_automatic());

      vvp_context_t context = scope->free_contexts;
      إذا (context) {
            scope->free_contexts = vvp_get_اگلا_context(context);
            لكل (بنانشانی idx = 0 ; idx < scope->nitem ; idx += 1) {
                  scope->شے[idx]->reset_instance(context);
            }
      } وإلا {
            context = vvp_allocate_context(scope->nitem);
            لكل (بنانشانی idx = 0 ; idx < scope->nitem ; idx += 1) {
                  scope->شے[idx]->alloc_instance(context);
            }
      }

      vvp_سیٹ_اگلا_context(context, scope->جیوندا_contexts);
      scope->جیوندا_contexts = context;

      أرجع context;
}

/*
 * Free a context previously allocated to a child thread by pushing it
 * onto the freed context stack. Remove it from the list of live contexts
 * in that scope.
 */
ساكن فراغ vthread_free_context(vvp_context_t context, __vpiScope*scope)
{
      أكد(scope->is_automatic());
      أكد(context);

      إذا (context == scope->جیوندا_contexts) {
            scope->جیوندا_contexts = vvp_get_اگلا_context(context);
      } وإلا {
            vvp_context_t tmp = scope->جیوندا_contexts;
            طالما (context != vvp_get_اگلا_context(tmp)) {
                  أكد(tmp);
                  tmp = vvp_get_اگلا_context(tmp);
            }
            vvp_سیٹ_اگلا_context(tmp, vvp_get_اگلا_context(context));
      }

      vvp_سیٹ_اگلا_context(context, scope->free_contexts);
      scope->free_contexts = context;
}

#جے_تعریف_شدہ CHECK_WITH_VALGRIND
فراغ contexts_delete(صنف __vpiScope*scope)
{
      vvp_context_t context = scope->free_contexts;

      طالما (context) {
	    scope->free_contexts = vvp_get_اگلا_context(context);
	    لكل (بنانشانی idx = 0; idx < scope->nitem; idx += 1) {
		  scope->شے[idx]->free_instance(context);
	    }
	    حرر(context);
	    context = scope->free_contexts;
      }
      حرر(scope->شے);
}
#جے_انت

/*
 * Create a new thread with the given start address.
 */
vthread_t vthread_new(vvp_کوڈ_t pc, __vpiScope*scope)
{
      vthread_t thr = جديد بنية vthread_s;
      thr->pc     = pc;
	//thr->bits4  = vvp_vector4_t(32);
      thr->ماپا = 0;
      thr->ماپا_scope = scope;
      thr->wait_اگلا = 0;
      thr->wt_context = 0;
      thr->rd_context = 0;

      thr->i_am_joining  = 0;
      thr->i_am_detached = 0;
      thr->i_am_اڈیک  = 0;
      thr->i_am_وچ_function = 0;
      thr->is_scheduled  = 0;
      thr->i_have_ended  = 0;
      thr->i_was_disabled = 0;
      thr->delay_delete  = 0;
      thr->اڈیک_for_event = 0;
      thr->event  = 0;
      thr->ecount = 0;

      thr->flags[0] = BIT4_0;
      thr->flags[1] = BIT4_1;
      thr->flags[2] = BIT4_X;
      thr->flags[3] = BIT4_Z;
      لكل (صحيح idx = 4 ; idx < 8 ; idx += 1)
	    thr->flags[idx] = BIT4_X;

      scope->دھاگے .insert(thr);
      أرجع thr;
}

#جے_تعریف_شدہ CHECK_WITH_VALGRIND
#إذا 0
/*
 * These are not currently correct. If you use them you will get
 * double delete messages. There is still a leak related to a
 * waiting event that needs to be investigated.
 */

ساكن فراغ wait_اگلا_delete(vthread_t ادھار)
{
      طالما (ادھار) {
	    vthread_t tmp = ادھار->wait_اگلا;
	    مٹاؤ ادھار;
	    ادھار = tmp;
	    إذا (ادھار->اڈیک_for_event == 0) اكسر;
      }
}

ساكن فراغ بچہ_delete(vthread_t ادھار)
{
      طالما (ادھار) {
	    vthread_t tmp = ادھار->بچہ;
	    مٹاؤ ادھار;
	    ادھار = tmp;
      }
}
#جے_انت

فراغ vthreads_delete(صنف __vpiScope*scope)
{
      لكل (std::سیٹ<vthread_t>::iterator cur = scope->دھاگے.begin()
		 ; cur != scope->دھاگے.سرا() ; ++ cur ) {
	    مٹاؤ *cur;
      }
      scope->دھاگے.clear();
}
#جے_انت

/*
 * Reaping pulls the thread out of the stack of threads. If I have a
 * child, then hand it over to my parent or fully detach it.
 */
ساكن فراغ vthread_reap(vthread_t thr)
{
      إذا (! thr->children.empty()) {
	    لكل (سیٹ<vthread_t>::iterator cur = thr->children.begin()
		       ; cur != thr->children.سرا() ; ++cur) {
		  vthread_t بچہ = *cur;
		  أكد(بچہ);
		  أكد(بچہ->ماپا == thr);
		  بچہ->ماپا = thr->ماپا;
	    }
      }
      إذا (! thr->detached_children.empty()) {
	    لكل (سیٹ<vthread_t>::iterator cur = thr->detached_children.begin()
		       ; cur != thr->detached_children.سرا() ; ++cur) {
		  vthread_t بچہ = *cur;
		  أكد(بچہ);
		  أكد(بچہ->ماپا == thr);
		  أكد(بچہ->i_am_detached);
		  بچہ->ماپا = 0;
		  بچہ->i_am_detached = 0;
	    }
      }
      إذا (thr->ماپا) {
	      /* assert that the given element was removed. */
	    إذا (thr->i_am_detached) {
		  size_t res = thr->ماپا->detached_children.erase(thr);
		  أكد(res == 1);
	    } وإلا {
		  size_t res = thr->ماپا->children.erase(thr);
		  أكد(res == 1);
	    }
      }

      thr->ماپا = 0;

	// Remove myself from the containing scope if needed.
      thr->ماپا_scope->دھاگے.erase(thr);

      thr->pc = codespace_null();

	/* If this thread is not scheduled, then is it safe to delete
	   it now. Otherwise, let the schedule event (which will
	   execute the thread at of_ZOMBIE) delete the object. */
      إذا ((thr->is_scheduled == 0) && (thr->اڈیک_for_event == 0)) {
	    أكد(thr->children.empty());
	    أكد(thr->wait_اگلا == 0);
	    إذا (thr->delay_delete)
		  ویلا_سارنی_del_thr(thr);
	    وإلا
		  vthread_delete(thr);
      }
}

فراغ vthread_delete(vthread_t thr)
{
      thr->cleanup();
      مٹاؤ thr;
}

فراغ vthread_mark_scheduled(vthread_t thr)
{
      طالما (thr != 0) {
	    أكد(thr->is_scheduled == 0);
	    thr->is_scheduled = 1;
	    thr = thr->wait_اگلا;
      }
}

فراغ vthread_mark_final(vthread_t thr)
{
      /*
       * The behavior in a final thread is the same as in a function. Any
       * child thread will be executed immediately rather than being
       * scheduled.
       */
      طالما (thr != 0) {
	    thr->i_am_وچ_function = 1;
	    thr = thr->wait_اگلا;
      }
}

فراغ vthread_delay_delete()
{
      إذا (running_thread)
	    running_thread->delay_delete = 1;
}

/*
 * This function runs each thread by fetching an instruction,
 * incrementing the PC, and executing the instruction. The thread may
 * be the head of a list, so each thread is run so far as possible.
 */
فراغ vthread_لگاتار(vthread_t thr)
{
      طالما (thr != 0) {
	    vthread_t tmp = thr->wait_اگلا;
	    thr->wait_اگلا = 0;

	    أكد(thr->is_scheduled);
	    thr->is_scheduled = 0;

            running_thread = thr;

	    لكل (;;) {
		  vvp_کوڈ_t cp = thr->pc;
		  thr->pc += 1;

		    /* Run the opcode implementation. If the execution of
		       the opcode returns false, then the thread is meant to
		       be paused, so break out of the loop. */
		  منطقي rc = (cp->opcode)(thr, cp);
		  إذا (rc == خطأ)
			اكسر;
	    }

	    thr = tmp;
      }
      running_thread = 0;
}

/*
 * The CHUNK_LINK instruction is a special next pointer for linking
 * chunks of code space. It's like a simplified %jmp.
 */
منطقي دا_CHUNK_LINK(vthread_t thr, vvp_کوڈ_t code)
{
      أكد(code->cptr);
      thr->pc = code->cptr;
      أرجع سچ;
}

/*
 * This is called by an event functor to wake up all the threads on
 * its list. I in fact created that list in the %wait instruction, and
 * I also am certain that the waiting_for_event flag is set.
 */
فراغ vthread_ویلا_سارنی_list(vthread_t thr)
{
      لكل (vthread_t cur = thr ;  cur ;  cur = cur->wait_اگلا) {
	    أكد(cur->اڈیک_for_event);
	    cur->اڈیک_for_event = 0;
      }

      ویلا_سارنی_vthread(thr, 0);
}

vvp_context_t vthread_get_wt_context()
{
      إذا (running_thread)
            أرجع running_thread->wt_context;
      وإلا
            أرجع 0;
}

vvp_context_t vthread_get_rd_context()
{
      إذا (running_thread)
            أرجع running_thread->rd_context;
      وإلا
            أرجع 0;
}

vvp_context_شے_t vthread_get_wt_context_شے(بنانشانی context_idx)
{
      أكد(running_thread && running_thread->wt_context);
      أرجع vvp_get_context_شے(running_thread->wt_context, context_idx);
}

vvp_context_شے_t vthread_get_rd_context_شے(بنانشانی context_idx)
{
      أكد(running_thread && running_thread->rd_context);
      أرجع vvp_get_context_شے(running_thread->rd_context, context_idx);
}

/*
 * %abs/wr
 */
منطقي دا_ABS_WR(vthread_t thr, vvp_کوڈ_t)
{
      thr->ڈالو_real( fabs(thr->نکالو_real()) );
      أرجع سچ;
}

منطقي دا_ALLOC(vthread_t thr, vvp_کوڈ_t cp)
{
        /* Allocate a context. */
      vvp_context_t بچہ_context = vthread_alloc_context(cp->scope);

        /* Push the allocated context onto the write context stack. */
      vvp_سیٹ_stacked_context(بچہ_context, thr->wt_context);
      thr->wt_context = بچہ_context;

      أرجع سچ;
}

منطقي دا_AND(vthread_t thr, vvp_کوڈ_t)
{
      vvp_vector4_t valb = thr->نکالو_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();
      أكد(vala.size() == valb.size());
      vala &= valb;
      أرجع سچ;
}

/*
 * This function must ALWAYS be called with the val set to the right
 * size, and initialized with BIT4_0 bits. Certain optimizations rely
 * on that.
 */
ساكن فراغ get_فوری_rval(vvp_کوڈ_t cp, vvp_vector4_t&val)
{
      uint32_t vala = cp->ثنائی_ہندسہ_idx[0];
      uint32_t valb = cp->ثنائی_ہندسہ_idx[1];
      بنانشانی wid  = cp->نمبر;

      إذا (valb == 0) {
	      // Special case: if the value is zero, we are done
	      // before we start.
	    إذا (vala == 0) أرجع;

	      // Special case: The value has no X/Z bits, so we can
	      // use the setarray method to write the value all at once.
	    بنانشانی use_wid = 8*حجم(بنانشانی لما);
	    إذا (wid < use_wid)
		  use_wid = wid;
	    بنانشانی لما tmp[1];
	    tmp[0] = vala;
	    val.setarray(0, use_wid, tmp);
	    أرجع;
      }

	// The immediate value can be values bigger then 32 bits, but
	// only if the high bits are zero. So at most we need to run
	// through the loop below 32 times. Maybe less, if the target
	// width is less. We don't have to do anything special on that
	// because vala/valb bits will shift away so (vala|valb) will
	// turn to zero at or before 32 shifts.

      لكل (بنانشانی idx = 0 ; idx < wid && (vala|valb) ; idx += 1) {
	    uint32_t ba = 0;
	      // Convert the vala/valb bits to a ba number that
	      // matches the encoding of the vvp_bit4_t enumeration.
	    ba = (valb & 1) << 1;
	    ba |= vala & 1;

	      // Note that the val is already pre-filled with BIT4_0
	      // bits, os we only need to set non-zero bit values.
	    إذا (ba) val.سیٹ_ثنائی_ہندسہ(idx, (vvp_bit4_t)ba);

	    vala >>= 1;
	    valb >>= 1;
      }
}

/*
 * %add
 *
 * Pop r,
 * Pop l,
 * Push l+r
 *
 * Pop 2 and push 1 is the same as pop 1 and replace the remaining top
 * of the stack with a new value. That is what we will do.
 */
منطقي دا_جوڑو(vthread_t thr, vvp_کوڈ_t)
{
      vvp_vector4_t r = thr->نکالو_vec4();
	// Rather then pop l, use it directly from the stack. When we
	// assign to 'l', that will edit the top of the stack, which
	// replaces a pop and a pull.
      vvp_vector4_t&l = thr->peek_vec4();

      l.جوڑو(r);

      أرجع سچ;
}

/*
 * %addi <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
منطقي دا_ADDI(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی wid = cp->نمبر;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_فوری_rval (cp, r);

      l.جوڑو(r);

      أرجع سچ;
}

/*
 * %add/wr
 */
منطقي دا_جوڑو_WR(vthread_t thr, vvp_کوڈ_t)
{
      مزدوج r = thr->نکالو_real();
      مزدوج l = thr->نکالو_real();
      thr->ڈالو_real(l + r);
      أرجع سچ;
}

/* %assign/ar <array>, <delay>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory. <delay>
 * is the delay in simulation time. <bit> is the index register
 * containing the real value.
 */
منطقي دا_سونپو_AR(vthread_t thr, vvp_کوڈ_t cp)
{
      لما adr = thr->الفاظ[3].w_int;
      بنانشانی delay = cp->ثنائی_ہندسہ_idx[0];
      مزدوج value = thr->نکالو_real();

      إذا (adr >= 0) {
	    ویلا_سارنی_سونپو_array_لفظ(cp->array, adr, value, delay);
      }

      أرجع سچ;
}

/* %assign/ar/d <array>, <delay_idx>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory.
 * <delay_idx> is the integer register that contains the delay value.
 */
منطقي دا_سونپو_ARD(vthread_t thr, vvp_کوڈ_t cp)
{
      لما adr = thr->الفاظ[3].w_int;
      مزدوج value = thr->نکالو_real();

      إذا (adr >= 0) {
	    vvp_time64_t delay = thr->الفاظ[cp->ثنائی_ہندسہ_idx[0]].w_uint;
	    ویلا_سارنی_سونپو_array_لفظ(cp->array, adr, value, delay);
      }

      أرجع سچ;
}

/* %assign/ar/e <array>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory. <bit>
 * is the index register containing the real value. The event
 * information is contained in the thread event control registers
 * and is set with %evctl.
 */
منطقي دا_سونپو_ARE(vthread_t thr, vvp_کوڈ_t cp)
{
      لما adr = thr->الفاظ[3].w_int;
      مزدوج value = thr->نکالو_real();

      إذا (adr >= 0) {
	    إذا (thr->ecount == 0) {
		  ویلا_سارنی_سونپو_array_لفظ(cp->array, adr, value, 0);
	    } وإلا {
		  ویلا_سارنی_evctl(cp->array, adr, value, thr->event,
		                 thr->ecount);
	    }
      }

      أرجع سچ;
}

/*
 * %assign/vec4 <var>, <delay>
 */
منطقي دا_سونپو_VEC4(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      بنانشانی delay = cp->ثنائی_ہندسہ_idx[0];
      ثابت vvp_vector4_t&val = thr->peek_vec4();

      ویلا_سارنی_سونپو_سمتیہ(ptr, 0, 0, val, delay);
      thr->نکالو_vec4(1);
      أرجع سچ;
}

/*
 * Resizes a vector value for a partial assignment so that the value is fully
 * in-bounds of the target signal. Both `val` and `off` will be updated if
 * necessary.
 *
 * Returns false if the value is fully out-of-bounds and the assignment should
 * be skipped. Otherwise returns true.
 */
ساكن منطقي resize_rval_vec(vvp_vector4_t &val, int64_t &off,
			    بنانشانی صحيح sig_wid)
{
      بنانشانی صحيح wid = val.size();

        // Fully in bounds, most likely case
      إذا (off >= 0 && (uint64_t)off + wid <= sig_wid)
	    أرجع سچ;

      بنانشانی صحيح ادھار = 0;
      إذا (off >= 0) {
	      // Fully out-of-bounds
	    إذا ((uint64_t)off >= sig_wid)
		  أرجع خطأ;
      } وإلا {
	      // Fully out-of-bounds */
	    إذا ((uint64_t)(-off) >= wid)
		  أرجع خطأ;

	      // If the index is below the vector, then only assign the high
	      // bits that overlap with the target
	    ادھار = -off;
	    wid += off;
		off = 0;
      }

	// If the value is partly above the target, then only assign
	// the bits that overlap
      إذا ((uint64_t)off + wid > sig_wid)
	    wid = sig_wid - (uint64_t)off;

      val = val.subvalue(ادھار, wid);

      أرجع سچ;
}

/*
 * %assign/vec4/a/d <arr>, <offx>, <delx>
 */
منطقي دا_سونپو_VEC4_A_D(vthread_t thr, vvp_کوڈ_t cp)
{
      صحيح off_idx = cp->ثنائی_ہندسہ_idx[0];
      صحيح del_idx = cp->ثنائی_ہندسہ_idx[1];
      صحيح adr_idx = 3;

      int64_t  off = off_idx ? thr->الفاظ[off_idx].w_int : 0;
      vvp_time64_t del = del_idx? thr->الفاظ[del_idx].w_uint : 0;
      لما     adr = thr->الفاظ[adr_idx].w_int;

      vvp_vector4_t val = thr->نکالو_vec4();

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      إذا (thr->flags[4] == BIT4_1)
	    أرجع سچ;

      إذا (!resize_rval_vec(val, off, cp->array->get_لفظ_ناپ()))
	    أرجع سچ;

      ویلا_سارنی_سونپو_array_لفظ(cp->array, adr, off, val, del);

      أرجع سچ;
}

/*
 * %assign/vec4/a/e <arr>, <offx>
 */
منطقي دا_سونپو_VEC4_A_E(vthread_t thr, vvp_کوڈ_t cp)
{
      صحيح off_idx = cp->ثنائی_ہندسہ_idx[0];
      صحيح adr_idx = 3;

      int64_t  off = off_idx ? thr->الفاظ[off_idx].w_int : 0;
      لما     adr = thr->الفاظ[adr_idx].w_int;

      vvp_vector4_t val = thr->نکالو_vec4();

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      إذا (thr->flags[4] == BIT4_1)
	    أرجع سچ;

      إذا (!resize_rval_vec(val, off, cp->array->get_لفظ_ناپ()))
	    أرجع سچ;

      إذا (thr->ecount == 0) {
	    ویلا_سارنی_سونپو_array_لفظ(cp->array, adr, off, val, 0);
      } وإلا {
	    ویلا_سارنی_evctl(cp->array, adr, val, off, thr->event, thr->ecount);
      }

      أرجع سچ;
}

/*
 * %assign/vec4/off/d <var>, <off>, <del>
 */
منطقي دا_سونپو_VEC4_OFF_D(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      بنانشانی off_اشاریہ = cp->ثنائی_ہندسہ_idx[0];
      بنانشانی del_اشاریہ = cp->ثنائی_ہندسہ_idx[1];
      vvp_vector4_t val = thr->نکالو_vec4();

      int64_t off = thr->الفاظ[off_اشاریہ].w_int;
      vvp_time64_t del = thr->الفاظ[del_اشاریہ].w_uint;

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      إذا (thr->flags[4] == BIT4_1)
	    أرجع سچ;

      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*> (cp->net->fil);
      أكد(sig);

      إذا (!resize_rval_vec(val, off, sig->قدر_ناپ()))
	    أرجع سچ;

      ویلا_سارنی_سونپو_سمتیہ(ptr, off, sig->قدر_ناپ(), val, del);
      أرجع سچ;
}

/*
 * %assign/vec4/off/e <var>, <off>
 */
منطقي دا_سونپو_VEC4_OFF_E(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      بنانشانی off_اشاریہ = cp->ثنائی_ہندسہ_idx[0];
      vvp_vector4_t val = thr->نکالو_vec4();

      int64_t off = thr->الفاظ[off_اشاریہ].w_int;

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      إذا (thr->flags[4] == BIT4_1)
	    أرجع سچ;

      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*> (cp->net->fil);
      أكد(sig);

      إذا (!resize_rval_vec(val, off, sig->قدر_ناپ()))
	    أرجع سچ;

      إذا (thr->ecount == 0) {
	    ویلا_سارنی_سونپو_سمتیہ(ptr, off, sig->قدر_ناپ(), val, 0);
      } وإلا {
	    ویلا_سارنی_evctl(ptr, val, off, sig->قدر_ناپ(), thr->event, thr->ecount);
      }

      أرجع سچ;
}

/*
 * %assign/vec4/d <var-label> <delay>
 */
منطقي دا_سونپو_VEC4_D(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      بنانشانی del_اشاریہ = cp->ثنائی_ہندسہ_idx[0];
      vvp_time64_t del = thr->الفاظ[del_اشاریہ].w_int;

      vvp_vector4_t value = thr->نکالو_vec4();

      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*> (cp->net->fil);
      أكد(sig);

      ویلا_سارنی_سونپو_سمتیہ(ptr, 0, sig->قدر_ناپ(), value, del);

      أرجع سچ;
}

/*
 * %assign/vec4/e <var-label>
 */
منطقي دا_سونپو_VEC4_E(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_vector4_t value = thr->نکالو_vec4();

      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*> (cp->net->fil);
      أكد(sig);

      إذا (thr->ecount == 0) {
	    ویلا_سارنی_سونپو_سمتیہ(ptr, 0, sig->قدر_ناپ(), value, 0);
      } وإلا {
	    ویلا_سارنی_evctl(ptr, value, 0, sig->قدر_ناپ(), thr->event, thr->ecount);
      }

      أرجع سچ;
}

/*
 * This is %assign/wr <vpi-label>, <delay>
 *
 * This assigns (after a delay) a value to a real variable. Use the
 * vpi_put_value function to do the assign, with the delay written
 * into the vpiInertialDelay carrying the desired delay.
 */
منطقي دا_سونپو_WR(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی delay = cp->ثنائی_ہندسہ_idx[0];
      مزدوج value = thr->نکالو_real();
      s_vpi_ویلا del;

      del.type = vpi_Sim_ویلا;
      vpip_ویلا_منزل_timestruct(&del, delay);

      __vpiHandle*tmp = cp->handle;

      t_vpi_قدر val;
      val.format = vpiRealVal;
      val.value.real = value;
      vpi_put_قدر(tmp, &val, &del, vpiTransportDelay);

      أرجع سچ;
}

منطقي دا_سونپو_WRD(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_time64_t delay = thr->الفاظ[cp->ثنائی_ہندسہ_idx[0]].w_uint;
      مزدوج value = thr->نکالو_real();
      s_vpi_ویلا del;

      del.type = vpi_Sim_ویلا;
      vpip_ویلا_منزل_timestruct(&del, delay);

      __vpiHandle*tmp = cp->handle;

      t_vpi_قدر val;
      val.format = vpiRealVal;
      val.value.real = value;
      vpi_put_قدر(tmp, &val, &del, vpiTransportDelay);

      أرجع سچ;
}

منطقي دا_سونپو_WRE(vthread_t thr, vvp_کوڈ_t cp)
{
      أكد(thr->event != 0);
      مزدوج value = thr->نکالو_real();
      __vpiHandle*tmp = cp->handle;

	// If the count is zero then just put the value.
      إذا (thr->ecount == 0) {
	    t_vpi_قدر val;

	    val.format = vpiRealVal;
	    val.value.real = value;
	    vpi_put_قدر(tmp, &val, 0, vpiNoDelay);
      } وإلا {
	    ویلا_سارنی_evctl(tmp, value, thr->event, thr->ecount);
      }

      thr->event = 0;
      thr->ecount = 0;

      أرجع سچ;
}

منطقي دا_BLEND(vthread_t thr, vvp_کوڈ_t)
{
      vvp_vector4_t vala = thr->نکالو_vec4();
      vvp_vector4_t valb = thr->نکالو_vec4();
      أكد(vala.size() == valb.size());

      لكل (بنانشانی idx = 0 ; idx < vala.size() ; idx += 1) {
	    إذا (vala.value(idx) == valb.value(idx))
		  استمر;

	    vala.سیٹ_ثنائی_ہندسہ(idx, BIT4_X);
      }

      thr->ڈالو_vec4(vala);
      أرجع سچ;
}

منطقي دا_BLEND_STR(vthread_t thr, vvp_کوڈ_t)
{
      string f = thr->نکالو_str();
      string t = thr->نکالو_str();
      thr->ڈالو_str((t == f) ? t : "");
      أرجع سچ;
}

منطقي دا_BLEND_WR(vthread_t thr, vvp_کوڈ_t)
{
      مزدوج f = thr->نکالو_real();
      مزدوج t = thr->نکالو_real();
      thr->ڈالو_real((t == f) ? t : 0.0);
      أرجع سچ;
}

منطقي دا_BREAKPOINT(vthread_t, vvp_کوڈ_t)
{
      أرجع سچ;
}

/*
 * %callf/void <code-label>, <scope-label>
 * Combine the %fork and %join steps for invoking a function.
 */
ساكن منطقي do_callf_void(vthread_t thr, vthread_t بچہ)
{

      إذا (بچہ->ماپا_scope->is_automatic()) {
	      /* The context allocated for this child is the top entry
		 on the write context stack */
	    بچہ->wt_context = thr->wt_context;
	    بچہ->rd_context = thr->wt_context;
      }

        // Mark the function thread as a direct child of the current thread.
      بچہ->ماپا = thr;
      thr->children.insert(بچہ);
        // This should be the only child
      أكد(thr->children.size()==1);

        // Execute the function. This SHOULD run the function to completion,
        // but there are some exceptional situations where it won't.
      أكد(بچہ->ماپا_scope->get_type_کوڈ() == vpiFunction);
      بچہ->is_scheduled = 1;
      بچہ->i_am_وچ_function = 1;
      vthread_لگاتار(بچہ);
      running_thread = thr;

      إذا (بچہ->i_have_ended) {
	    do_join(thr, بچہ);
	    أرجع سچ;
      } وإلا {
	    thr->i_am_joining = 1;
	    أرجع خطأ;
      }
}

منطقي دا_CALLF_OBJ(vthread_t thr, vvp_کوڈ_t cp)
{
      vthread_t بچہ = vthread_new(cp->cptr2, cp->scope);
      أرجع do_callf_void(thr, بچہ);

      // XXXX NOT IMPLEMENTED
}

منطقي دا_CALLF_REAL(vthread_t thr, vvp_کوڈ_t cp)
{
      vthread_t بچہ = vthread_new(cp->cptr2, cp->scope);

	// This is the return value. Push a place-holder value. The function
	// will replace this with the actual value using a %ret/real instruction.
      thr->ڈالو_real(0.0);
      بچہ->دلیلاں_real.push_back(0);

      أرجع do_callf_void(thr, بچہ);
}

منطقي دا_CALLF_STR(vthread_t thr, vvp_کوڈ_t cp)
{
      vthread_t بچہ = vthread_new(cp->cptr2, cp->scope);

      thr->ڈالو_str("");
      بچہ->دلیلاں_str.push_back(0);

      أرجع do_callf_void(thr, بچہ);
}

منطقي دا_CALLF_VEC4(vthread_t thr, vvp_کوڈ_t cp)
{
      vthread_t بچہ = vthread_new(cp->cptr2, cp->scope);

      vpiScopeFunction*scope_func = dynamic_cast<vpiScopeFunction*>(cp->scope);
      أكد(scope_func);

	// This is the return value. Push a place-holder value. The function
	// will replace this with the actual value using a %ret/real instruction.
      thr->ڈالو_vec4(vvp_vector4_t(scope_func->get_func_چوڑائی(), scope_func->get_func_init_val()));
      بچہ->دلیلاں_vec4.push_back(0);

      أرجع do_callf_void(thr, بچہ);
}

منطقي دا_CALLF_VOID(vthread_t thr, vvp_کوڈ_t cp)
{
      vthread_t بچہ = vthread_new(cp->cptr2, cp->scope);
      أرجع do_callf_void(thr, بچہ);
}

/*
 * The %cassign/link instruction connects a source node to a
 * destination node. The destination node must be a signal, as it is
 * marked with the source of the cassign so that it may later be
 * unlinked without specifically knowing the source that this
 * instruction used.
 */
منطقي دا_CASSIGN_LINK(vthread_t, vvp_کوڈ_t cp)
{
      vvp_net_t*dst = cp->net;
      vvp_net_t*src = cp->net2;

      vvp_fun_signal_ادھار*sig
	    = dynamic_cast<vvp_fun_signal_ادھار*>(dst->fun);
      أكد(sig);

	/* Any previous continuous assign should have been removed already. */
      أكد(sig->cassign_link == 0);

      sig->cassign_link = src;

	/* Link the output of the src to the port[1] (the cassign
	   port) of the destination. */
      vvp_net_ptr_t dst_ptr (dst, 1);
      src->link(dst_ptr);

      أرجع سچ;
}

/*
 * If there is an existing continuous assign linked to the destination
 * node, unlink it. This must be done before applying a new continuous
 * assign, otherwise the initial assigned value will be propagated to
 * any other nodes driven by the old continuous assign source.
 */
ساكن فراغ cassign_unlink(vvp_net_t*dst)
{
      vvp_fun_signal_ادھار*sig
	    = dynamic_cast<vvp_fun_signal_ادھار*>(dst->fun);
      أكد(sig);

      إذا (sig->cassign_link == 0)
	    أرجع;

      vvp_net_ptr_t tmp (dst, 1);
      sig->cassign_link->unlink(tmp);
      sig->cassign_link = 0;
}

/*
 * The %cassign/v instruction invokes a continuous assign of a
 * constant value to a signal. The instruction arguments are:
 *
 *     %cassign/vec4 <net>;
 *
 * Where the <net> is the net label assembled into a vvp_net pointer,
 * and the <base> and <wid> are stashed in the bit_idx array.
 *
 * This instruction writes vvp_vector4_t values to port-1 of the
 * target signal.
 */
منطقي دا_CASSIGN_VEC4(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_net_t*net = cp->net;
      vvp_vector4_t value = thr->نکالو_vec4();

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

	/* Set the value into port 1 of the destination. */
      vvp_net_ptr_t ptr (net, 1);
      vvp_send_vec4(ptr, value, 0);

      أرجع سچ;
}

/*
 * %cassign/vec4/off <var>, <off>
 */
منطقي دا_CASSIGN_VEC4_OFF(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_net_t*net = cp->net;
      بنانشانی ادھار_idx = cp->ثنائی_ہندسہ_idx[0];
      لما ادھار = thr->الفاظ[ادھار_idx].w_int;
      vvp_vector4_t value = thr->نکالو_vec4();
      بنانشانی wid = value.size();

      إذا (thr->flags[4] == BIT4_1)
	    أرجع سچ;

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*> (net->fil);
      أكد(sig);

      إذا (ادھار < 0 && (wid <= (بنانشانی)-ادھار))
	    أرجع سچ;

      إذا (ادھار >= (لما)sig->قدر_ناپ())
	    أرجع سچ;

      إذا (ادھار < 0) {
	    wid -= (بنانشانی) -ادھار;
	    ادھار = 0;
	    value.resize(wid);
      }

      إذا (ادھار+wid > sig->قدر_ناپ()) {
	    wid = sig->قدر_ناپ() - ادھار;
	    value.resize(wid);
      }

      vvp_net_ptr_t ptr (net, 1);
      vvp_send_vec4_pv(ptr, value, ادھار, sig->قدر_ناپ(), 0);
      أرجع سچ;
}

منطقي دا_CASSIGN_WR(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_net_t*net  = cp->net;
      مزدوج value = thr->نکالو_real();

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

	/* Set the value into port 1 of the destination. */
      vvp_net_ptr_t ptr (net, 1);
      vvp_send_real(ptr, value, 0);

      أرجع سچ;
}

/*
 * %cast2
 */
منطقي دا_CAST2(vthread_t thr, vvp_کوڈ_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      بنانشانی wid = val.size();

      لكل (بنانشانی idx = 0 ; idx < wid ; idx += 1) {
	    بدل (val.value(idx)) {
		حالة BIT4_0:
		حالة BIT4_1:
		  اكسر;
		افتراضي:
		  val.سیٹ_ثنائی_ہندسہ(idx, BIT4_0);
		  اكسر;
	    }
      }

      أرجع سچ;
}

منطقي do_cast_vec_dar(vthread_t thr, vvp_کوڈ_t cp, منطقي as_vec4)
{
      بنانشانی wid = cp->نمبر;

      vvp_object_t obj;
      thr->نکالو_object(obj);

      vvp_darray*darray = obj.peek<vvp_darray>();
      أكد(darray);

      vvp_vector4_t vec = darray->get_bitstream(as_vec4);
      إذا (vec.size() != wid) {
	    cerr << thr->get_fileline()
	         << "VVP error: size mismatch when casting dynamic array to vector." << سطر_انت;
            thr->ڈالو_vec4(vvp_vector4_t(wid));
            ویلا_سارنی_stop(0);
            أرجع خطأ;
      }
      thr->ڈالو_vec4(vec);
      أرجع سچ;
}

/*
 * %cast/vec2/dar <wid>
 */
منطقي دا_CAST_VEC2_DAR(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع do_cast_vec_dar(thr, cp, خطأ);
}

/*
 * %cast/vec4/dar <wid>
 */
منطقي دا_CAST_VEC4_DAR(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع do_cast_vec_dar(thr, cp, سچ);
}

/*
 * %cast/vec4/str <wid>
 */
منطقي دا_CAST_VEC4_STR(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی wid = cp->نمبر;
      string str = thr->نکالو_str();

      vvp_vector4_t vec(wid, BIT4_0);

      إذا (wid != 8*str.length()) {
	    cerr << thr->get_fileline()
	         << "VVP error: size mismatch when casting string to vector." << سطر_انت;
            thr->ڈالو_vec4(vec);
            ویلا_سارنی_stop(0);
            أرجع خطأ;
      }

      بنانشانی sdx = 0;
      بنانشانی vdx = wid;
      طالما (vdx > 0) {
            حرف ch = str[sdx++];
            vdx -= 8;
            لكل (بنانشانی bdx = 0; bdx < 8; bdx += 1) {
                  إذا (ch & 1)
                        vec.سیٹ_ثنائی_ہندسہ(vdx+bdx, BIT4_1);
                  ch >>= 1;
            }
      }

      thr->ڈالو_vec4(vec);
      أرجع سچ;
}

ساكن فراغ do_CMPE(vthread_t thr, ثابت vvp_vector4_t&lval, ثابت vvp_vector4_t&rval)
{
      أكد(rval.size() == lval.size());

      إذا (lval.has_xz() || rval.has_xz()) {

	    بنانشانی wid = lval.size();
	    vvp_bit4_t eq  = BIT4_1;
	    vvp_bit4_t eeq = BIT4_1;

	    لكل (بنانشانی idx = 0 ; idx < wid ; idx += 1) {
		  vvp_bit4_t lv = lval.value(idx);
		  vvp_bit4_t rv = rval.value(idx);

		  إذا (lv != rv)
			eeq = BIT4_0;

		  إذا (eq==BIT4_1 && (bit4_is_xz(lv) || bit4_is_xz(rv)))
			eq = BIT4_X;
		  إذا ((lv == BIT4_0) && (rv==BIT4_1))
			eq = BIT4_0;
		  إذا ((lv == BIT4_1) && (rv==BIT4_0))
			eq = BIT4_0;

		  إذا (eq == BIT4_0)
			اكسر;
	    }

	    thr->flags[4] = eq;
	    thr->flags[6] = eeq;

      } وإلا {
	      // If there are no XZ bits anywhere, then the results of
	      // == match the === test.
	    thr->flags[4] = thr->flags[6] = (lval.eeq(rval)? BIT4_1 : BIT4_0);
      }
}

/*
 *  %cmp/e
 *
 * Pop the operands from the stack, and do not replace them. The
 * results are written to flag bits:
 *
 *	4: eq  (equal)
 *
 *	6: eeq (case equal)
 */
منطقي دا_CMPE(vthread_t thr, vvp_کوڈ_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ثابت vvp_vector4_t&rval = thr->peek_vec4(0);
      ثابت vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPE(thr, lval, rval);

      thr->نکالو_vec4(2);
      أرجع سچ;
}

منطقي دا_CMPNE(vthread_t thr, vvp_کوڈ_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ثابت vvp_vector4_t&rval = thr->peek_vec4(0);
      ثابت vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];
      thr->flags[6] =  ~thr->flags[6];

      thr->نکالو_vec4(2);
      أرجع سچ;
}

/*
 * %cmpi/e <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
منطقي دا_CMPIE(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی wid = cp->نمبر;

      ثابت vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_فوری_rval (cp, rval);

      do_CMPE(thr, lval, rval);

      thr->نکالو_vec4(1);
      أرجع سچ;
}

منطقي دا_CMPINE(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی wid = cp->نمبر;

      ثابت vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_فوری_rval (cp, rval);

      do_CMPE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];
      thr->flags[6] =  ~thr->flags[6];

      thr->نکالو_vec4(1);
      أرجع سچ;
}



ساكن فراغ do_CMPS(vthread_t thr, ثابت vvp_vector4_t&lval, ثابت vvp_vector4_t&rval)
{
      أكد(rval.size() == lval.size());

	// If either value has XZ bits, then the eq and lt values are
	// known already to be X. Just calculate the eeq result as a
	// special case and short circuit the rest of the compare.
      إذا (lval.has_xz() || rval.has_xz()) {
	    thr->flags[4] = BIT4_X; // eq
	    thr->flags[5] = BIT4_X; // lt
	    thr->flags[6] = lval.eeq(rval)? BIT4_1 : BIT4_0;
	    أرجع;
      }

	// Past this point, we know we are dealing only with fully
	// defined values.
      بنانشانی wid = lval.size();

      ثابت vvp_bit4_t sig1 = lval.value(wid-1);
      ثابت vvp_bit4_t sig2 = rval.value(wid-1);

	// If the lval is <0 and the rval is >=0, then we know the result.
      إذا ((sig1 == BIT4_1) && (sig2 == BIT4_0)) {
	    thr->flags[4] = BIT4_0; // eq;
	    thr->flags[5] = BIT4_1; // lt;
	    thr->flags[6] = BIT4_0; // eeq
	    أرجع;
      }

	// If the lval is >=0 and the rval is <0, then we know the result.
      إذا ((sig1 == BIT4_0) && (sig2 == BIT4_1)) {
	    thr->flags[4] = BIT4_0; // eq;
	    thr->flags[5] = BIT4_0; // lt;
	    thr->flags[6] = BIT4_0; // eeq
	    أرجع;
      }

	// The values have the same sign, so we have to look at the
	// actual value. Scan from the MSB down. As soon as we find a
	// bit that differs, we know the result.

      لكل (بنانشانی idx = 1 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t lv = lval.value(wid-1-idx);
	    vvp_bit4_t rv = rval.value(wid-1-idx);

	    إذا (lv == rv)
		  استمر;

	    thr->flags[4] = BIT4_0; // eq
	    thr->flags[6] = BIT4_0; // eeq

	    إذا (lv==BIT4_0) {
		  thr->flags[5] = BIT4_1; // lt
	    } وإلا {
		  thr->flags[5] = BIT4_0; // lt
	    }
	    أرجع;
      }

	// If we survive the loop above, then the values must be equal.
      thr->flags[4] = BIT4_1;
      thr->flags[5] = BIT4_0;
      thr->flags[6] = BIT4_1;
}

/*
 *  %cmp/s
 *
 * Pop the operands from the stack, and do not replace them. The
 * results are written to flag bits:
 *
 *	4: eq  (equal)
 *	5: lt  (less than)
 *	6: eeq (case equal)
 */
منطقي دا_CMPS(vthread_t thr, vvp_کوڈ_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ثابت vvp_vector4_t&rval = thr->peek_vec4(0);
      ثابت vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPS(thr, lval, rval);

      thr->نکالو_vec4(2);
      أرجع سچ;
}

/*
 * %cmpi/s <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
منطقي دا_CMPIS(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی wid = cp->نمبر;

      ثابت vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_فوری_rval (cp, rval);

      do_CMPS(thr, lval, rval);

      thr->نکالو_vec4(1);
      أرجع سچ;
}

منطقي دا_CMPSTR(vthread_t thr, vvp_کوڈ_t)
{
      string re = thr->نکالو_str();
      string le = thr->نکالو_str();

      صحيح rc = strcmp(le.c_str(), re.c_str());

      vvp_bit4_t eq;
      vvp_bit4_t lt;

      إذا (rc == 0) {
	    eq = BIT4_1;
	    lt = BIT4_0;
      } وإلا إذا (rc < 0) {
	    eq = BIT4_0;
	    lt = BIT4_1;
      } وإلا {
	    eq = BIT4_0;
	    lt = BIT4_0;
      }

      thr->flags[4] = eq;
      thr->flags[5] = lt;

      أرجع سچ;
}

ساكن فراغ دا_CMPU_the_hard_way(vthread_t thr, بنانشانی wid,
				 ثابت vvp_vector4_t&lval,
				 ثابت vvp_vector4_t&rval)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_bit4_t eeq = BIT4_1;

      لكل (بنانشانی idx = 0 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t lv = lval.value(idx);
	    vvp_bit4_t rv = rval.value(idx);

	    إذا (lv != rv)
		  eeq = BIT4_0;

	    إذا (eq==BIT4_1 && (bit4_is_xz(lv) || bit4_is_xz(rv)))
		  eq = BIT4_X;
	    إذا ((lv == BIT4_0) && (rv==BIT4_1))
		  eq = BIT4_0;
	    إذا ((lv == BIT4_1) && (rv==BIT4_0))
		  eq = BIT4_0;

	    إذا (eq == BIT4_0)
		  اكسر;

      }

      thr->flags[4] = eq;
      thr->flags[5] = BIT4_X;
      thr->flags[6] = eeq;
}

ساكن فراغ do_CMPU(vthread_t thr, ثابت vvp_vector4_t&lval, ثابت vvp_vector4_t&rval)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_bit4_t lt = BIT4_0;

      إذا (rval.size() != lval.size()) {
	    cerr << thr->get_fileline()
	         << "VVP ERROR: %cmp/u operand width mismatch: lval=" << lval
		 << ", rval=" << rval << سطر_انت;
      }
      أكد(rval.size() == lval.size());
      بنانشانی wid = lval.size();

      بنانشانی لما*larray = lval.subarray(0,wid);
      إذا (larray == 0) أرجع دا_CMPU_the_hard_way(thr, wid, lval, rval);

      بنانشانی لما*rarray = rval.subarray(0,wid);
      إذا (rarray == 0) {
	    مٹاؤ[]larray;
	    أرجع دا_CMPU_the_hard_way(thr, wid, lval, rval);
      }

      بنانشانی الفاظ = (wid+CPU_لفظ_بٹ-1) / CPU_لفظ_بٹ;

      لكل (بنانشانی wdx = 0 ; wdx < الفاظ ; wdx += 1) {
	    إذا (larray[wdx] == rarray[wdx])
		  استمر;

	    eq = BIT4_0;
	    إذا (larray[wdx] < rarray[wdx])
		  lt = BIT4_1;
	    وإلا
		  lt = BIT4_0;
      }

      مٹاؤ[]larray;
      مٹاؤ[]rarray;

      thr->flags[4] = eq;
      thr->flags[5] = lt;
      thr->flags[6] = eq;
}

منطقي دا_CMPU(vthread_t thr, vvp_کوڈ_t)
{

      ثابت vvp_vector4_t&rval = thr->peek_vec4(0);
      ثابت vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPU(thr, lval, rval);

      thr->نکالو_vec4(2);
      أرجع سچ;
}

/*
 * %cmpi/u <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
منطقي دا_CMPIU(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی wid = cp->نمبر;

      ثابت vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_فوری_rval (cp, rval);

      do_CMPU(thr, lval, rval);

      thr->نکالو_vec4(1);
      أرجع سچ;
}


/*
 * %cmp/x
 */
منطقي دا_CMPX(vthread_t thr, vvp_کوڈ_t)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_vector4_t rval = thr->نکالو_vec4();
      vvp_vector4_t lval = thr->نکالو_vec4();

      أكد(rval.size() == lval.size());
      بنانشانی wid = lval.size();

      لكل (بنانشانی idx = 0 ; idx < wid ; idx += 1) {
	    vvp_bit4_t lv = lval.value(idx);
	    vvp_bit4_t rv = rval.value(idx);
	    إذا ((lv != rv) && !bit4_is_xz(lv) && !bit4_is_xz(rv)) {
		  eq = BIT4_0;
		  اكسر;
	    }
      }

      thr->flags[4] = eq;
      أرجع سچ;
}

ساكن فراغ do_CMPWE(vthread_t thr, ثابت vvp_vector4_t&lval, ثابت vvp_vector4_t&rval)
{
      أكد(rval.size() == lval.size());

      إذا (lval.has_xz() || rval.has_xz()) {

	    بنانشانی wid = lval.size();
	    vvp_bit4_t eq  = BIT4_1;

	    لكل (بنانشانی idx = 0 ; idx < wid ; idx += 1) {
		  vvp_bit4_t lv = lval.value(idx);
		  vvp_bit4_t rv = rval.value(idx);

		  إذا (bit4_is_xz(rv))
			استمر;
		  إذا ((eq == BIT4_1) && bit4_is_xz(lv))
			eq = BIT4_X;
		  إذا ((lv == BIT4_0) && (rv==BIT4_1))
			eq = BIT4_0;
		  إذا ((lv == BIT4_1) && (rv==BIT4_0))
			eq = BIT4_0;

		  إذا (eq == BIT4_0)
			اكسر;
	    }

	    thr->flags[4] = eq;

      } وإلا {
	      // If there are no XZ bits anywhere, then the results of
	      // ==? match the === test.
	    thr->flags[4] = (lval.eeq(rval)? BIT4_1 : BIT4_0);
      }
}

منطقي دا_CMPWE(vthread_t thr, vvp_کوڈ_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ثابت vvp_vector4_t&rval = thr->peek_vec4(0);
      ثابت vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPWE(thr, lval, rval);

      thr->نکالو_vec4(2);
      أرجع سچ;
}

منطقي دا_CMPWNE(vthread_t thr, vvp_کوڈ_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ثابت vvp_vector4_t&rval = thr->peek_vec4(0);
      ثابت vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPWE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];

      thr->نکالو_vec4(2);
      أرجع سچ;
}

منطقي دا_CMPWR(vthread_t thr, vvp_کوڈ_t)
{
      مزدوج r = thr->نکالو_real();
      مزدوج l = thr->نکالو_real();

      vvp_bit4_t eq = (l == r)? BIT4_1 : BIT4_0;
      vvp_bit4_t lt = (l <  r)? BIT4_1 : BIT4_0;

      thr->flags[4] = eq;
      thr->flags[5] = lt;

      أرجع سچ;
}

/*
 * %cmp/z
 */
منطقي دا_CMPZ(vthread_t thr, vvp_کوڈ_t)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_vector4_t rval = thr->نکالو_vec4();
      vvp_vector4_t lval = thr->نکالو_vec4();

      أكد(rval.size() == lval.size());
      بنانشانی wid = lval.size();

      لكل (بنانشانی idx = 0 ; idx < wid ; idx += 1) {
	    vvp_bit4_t lv = lval.value(idx);
	    vvp_bit4_t rv = rval.value(idx);
	    إذا ((lv != rv) && (rv != BIT4_Z) && (lv != BIT4_Z)) {
		  eq = BIT4_0;
		  اكسر;
	    }
      }

      thr->flags[4] = eq;
      أرجع سچ;
}

/*
 *  %concat/str;
 */
منطقي دا_CONCAT_STR(vthread_t thr, vvp_کوڈ_t)
{
      string متن = thr->نکالو_str();
      thr->peek_str(0).append(متن);
      أرجع سچ;
}

/*
 *  %concati/str <string>;
 */
منطقي دا_CONCATI_STR(vthread_t thr, vvp_کوڈ_t cp)
{
      ثابت حرف*متن = cp->متن;
      thr->peek_str(0).append(filter_string(متن));
      أرجع سچ;
}

/*
 * %concat/vec4
 */
منطقي دا_CONCAT_VEC4(vthread_t thr, vvp_کوڈ_t)
{
      ثابت vvp_vector4_t&lsb = thr->peek_vec4(0);
      ثابت vvp_vector4_t&msb = thr->peek_vec4(1);

	// The result is the size of the top two vectors in the stack.
      vvp_vector4_t res (msb.size() + lsb.size(), BIT4_X);

	// Build up the result.
      res.سیٹ_vec(0, lsb);
      res.سیٹ_vec(lsb.size(), msb);

	// Rearrange the stack to pop the inputs and push the
	// result. Do that by actually popping only 1 stack position
	// and replacing the new top with the new value.
      thr->نکالو_vec4(1);
      thr->peek_vec4() = res;

      أرجع سچ;
}

/*
 * %concati/vec4 <vala>, <valb>, <wid>
 *
 * Concat the immediate value to the LOW bits of the concatenation.
 * Get the HIGH bits from the top of the vec4 stack.
 */
منطقي دا_CONCATI_VEC4(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی wid  = cp->نمبر;

      vvp_vector4_t&msb = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t lsb (wid, BIT4_0);
      get_فوری_rval (cp, lsb);

      vvp_vector4_t res (msb.size()+lsb.size(), BIT4_X);
      res.سیٹ_vec(0, lsb);
      res.سیٹ_vec(lsb.size(), msb);

      msb = res;
      أرجع سچ;
}

/*
 * %cvt/rv
 */
منطقي دا_CVT_RV(vthread_t thr, vvp_کوڈ_t)
{
      مزدوج val;
      vvp_vector4_t val4 = thr->نکالو_vec4();
      vector4_منزل_قدر(val4, val, خطأ);
      thr->ڈالو_real(val);
      أرجع سچ;
}

/*
 * %cvt/rv/s
 */
منطقي دا_CVT_RV_S(vthread_t thr, vvp_کوڈ_t)
{
      مزدوج val;
      vvp_vector4_t val4 = thr->نکالو_vec4();
      vector4_منزل_قدر(val4, val, سچ);
      thr->ڈالو_real(val);
      أرجع سچ;
}

/*
 * %cvt/sr <idx>
 * Pop the top value from the real stack, convert it to a 64bit signed
 * and save it to the indexed register.
 */
منطقي دا_CVT_SR(vthread_t thr, vvp_کوڈ_t cp)
{
      مزدوج r = thr->نکالو_real();
      thr->الفاظ[cp->ثنائی_ہندسہ_idx[0]].w_int = i64round(r);

      أرجع سچ;
}

/*
 * %cvt/ur <idx>
 */
منطقي دا_CVT_UR(vthread_t thr, vvp_کوڈ_t cp)
{
      مزدوج r = thr->نکالو_real();
      thr->الفاظ[cp->ثنائی_ہندسہ_idx[0]].w_uint = vlg_گیڑا_منزل_u64(r);

      أرجع سچ;
}

/*
 * %cvt/vr <wid>
 */
منطقي دا_CVT_VR(vthread_t thr, vvp_کوڈ_t cp)
{
      مزدوج r = thr->نکالو_real();
      بنانشانی wid = cp->نمبر;

      vvp_vector4_t tmp(wid, r);
      thr->ڈالو_vec4(tmp);
      أرجع سچ;
}

/*
 * This implements the %deassign instruction. All we do is write a
 * long(1) to port-3 of the addressed net. This turns off an active
 * continuous assign activated by %cassign/v
 */
منطقي دا_DEASSIGN(vthread_t, vvp_کوڈ_t cp)
{
      vvp_net_t*net = cp->net;
      بنانشانی ادھار  = cp->ثنائی_ہندسہ_idx[0];
      بنانشانی width = cp->ثنائی_ہندسہ_idx[1];

      vvp_signal_قدر*fil = dynamic_cast<vvp_signal_قدر*> (net->fil);
      أكد(fil);
      vvp_fun_signal_vec*sig = dynamic_cast<vvp_fun_signal_vec*>(net->fun);
      أكد(sig);

      إذا (ادھار >= fil->قدر_ناپ()) أرجع سچ;
      إذا (ادھار+width > fil->قدر_ناپ()) width = fil->قدر_ناپ() - ادھار;

      منطقي بھریا_sig = ادھار == 0 && width == fil->قدر_ناپ();

	// This is the net that is forcing me...
      إذا (vvp_net_t*src = sig->cassign_link) {
	    إذا (! بھریا_sig) {
		  fprintf(stderr, "Sorry: when a signal is assigning a "
		          "register, I cannot deassign part of it.\n");
		  باہر_نکلو(1);
	    }
	      // And this is the pointer to be removed.
	    vvp_net_ptr_t dst_ptr (net, 1);
	    src->unlink(dst_ptr);
	    sig->cassign_link = 0;
      }

	/* Do we release all or part of the net? */
      إذا (بھریا_sig) {
	    sig->deassign();
      } وإلا {
	    sig->deassign_pv(ادھار, width);
      }

      أرجع سچ;
}

منطقي دا_DEASSIGN_WR(vthread_t, vvp_کوڈ_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_real*sig = dynamic_cast<vvp_fun_signal_real*>(net->fun);
      أكد(sig);

	// This is the net that is forcing me...
      إذا (vvp_net_t*src = sig->cassign_link) {
	      // And this is the pointer to be removed.
	    vvp_net_ptr_t dst_ptr (net, 1);
	    src->unlink(dst_ptr);
	    sig->cassign_link = 0;
      }

      sig->deassign();

      أرجع سچ;
}

/*
 * %debug/thr
 */
منطقي دا_DEBUG_THR(vthread_t thr, vvp_کوڈ_t cp)
{
      ثابت حرف*متن = cp->متن;
      thr->debug_dump(cerr, متن);
      أرجع سچ;
}

/*
 * The delay takes two 32bit numbers to make up a 64bit time.
 *
 *   %delay <low>, <hig>
 */
منطقي دا_DELAY(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_time64_t کم = cp->ثنائی_ہندسہ_idx[0];
      vvp_time64_t hig = cp->ثنائی_ہندسہ_idx[1];

      vvp_time64_t delay = (hig << 32) | کم;

      إذا (delay == 0) ویلا_سارنی_inactive(thr);
      وإلا ویلا_سارنی_vthread(thr, delay);
      أرجع خطأ;
}

منطقي دا_DELAYX(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_time64_t delay;

      أكد(cp->نمبر < vthread_s::الفاظ_گنتی);
      delay = thr->الفاظ[cp->نمبر].w_uint;
      إذا (delay == 0) ویلا_سارنی_inactive(thr);
      وإلا ویلا_سارنی_vthread(thr, delay);
      أرجع خطأ;
}

منطقي دا_DELETE_ELEM(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_net_t*net = cp->net;

      int64_t idx_val = thr->الفاظ[3].w_int;
      إذا (thr->flags[4] == BIT4_1) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with undefined index."
	         << سطر_انت;
	    أرجع سچ;
      }
      إذا (idx_val < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with negative index."
	         << سطر_انت;
	    أرجع سچ;
      }
      size_t idx = idx_val;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      أكد(obj);

      vvp_قطار*قطار = obj->get_object().peek<vvp_قطار>();
      إذا (قطار == 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping delete(" << idx
	         << ") on empty queue." << سطر_انت;
      } وإلا {
	    size_t size = قطار->get_ناپ();
	    إذا (idx >= size) {
		  cerr << thr->get_fileline()
		       << "Warning: skipping out of range delete(" << idx
		       << ") on queue of size " << size << "." << سطر_انت;
	    } وإلا {
		  قطار->erase(idx);
	    }
      }

      أرجع سچ;
}

/* %delete/obj <label>
 *
 * This operator works by assigning a nil to the target object. This
 * causes any value that might be there to be garbage collected, thus
 * deleting the object.
 */
منطقي دا_DELETE_OBJ(vthread_t thr, vvp_کوڈ_t cp)
{
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_send_object(ptr, vvp_object_t(), thr->wt_context);

      أرجع سچ;
}

/* %reverse/obj <label>
 * Reverse element order of the queue or dynamic array object at this net.
 */
منطقي دا_الٹ_OBJ(vthread_t /*thr*/, vvp_کوڈ_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      أكد(obj);

      vvp_object_t stor = obj->get_object();
      إذا (stor.test_nil()) أرجع سچ;

	/* Match other queue ops: peek queues before vvp_darray so concrete
	   queue types dispatch reverse_elems() correctly. */
      إذا (vvp_قطار* قطار = stor.peek<vvp_قطار>()) {
	    قطار->الٹ_elems();
	    أرجع سچ;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      أكد(darray);
      darray->الٹ_elems();

      أرجع سچ;
}

/* %reverse/prop/obj <property-index>
 * Reverse queue/darray property storage inside the class object on stack top.
 */
منطقي دا_الٹ_PROP_OBJ(vthread_t thr, vvp_کوڈ_t cp)
{
      size_t pid = cp->نمبر;

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      إذا (arr_obj.test_nil()) أرجع سچ;

      إذا (vvp_قطار* قطار = arr_obj.peek<vvp_قطار>()) {
	    قطار->الٹ_elems();
	    أرجع سچ;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      أكد(darray);
      darray->الٹ_elems();

      أرجع سچ;
}

/* %sort/obj — ascending sort */
منطقي دا_ترتیب_دیو_OBJ(vthread_t /*thr*/, vvp_کوڈ_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      أكد(obj);

      vvp_object_t stor = obj->get_object();
      إذا (stor.test_nil()) أرجع سچ;

      إذا (vvp_قطار* قطار = stor.peek<vvp_قطار>()) {
	    قطار->ترتیب_دیو_elems(سچ);
	    أرجع سچ;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      أكد(darray);
      darray->ترتیب_دیو_elems(سچ);

      أرجع سچ;
}

/* %sort/prop/obj */
منطقي دا_ترتیب_دیو_PROP_OBJ(vthread_t thr, vvp_کوڈ_t cp)
{
      size_t pid = cp->نمبر;

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      إذا (arr_obj.test_nil()) أرجع سچ;

      إذا (vvp_قطار* قطار = arr_obj.peek<vvp_قطار>()) {
	    قطار->ترتیب_دیو_elems(سچ);
	    أرجع سچ;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      أكد(darray);
      darray->ترتیب_دیو_elems(سچ);

      أرجع سچ;
}

/* %rsort/obj — descending sort */
منطقي دا_RSORT_OBJ(vthread_t /*thr*/, vvp_کوڈ_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      أكد(obj);

      vvp_object_t stor = obj->get_object();
      إذا (stor.test_nil()) أرجع سچ;

      إذا (vvp_قطار* قطار = stor.peek<vvp_قطار>()) {
	    قطار->ترتیب_دیو_elems(خطأ);
	    أرجع سچ;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      أكد(darray);
      darray->ترتیب_دیو_elems(خطأ);

      أرجع سچ;
}

/* %rsort/prop/obj */
منطقي دا_RSORT_PROP_OBJ(vthread_t thr, vvp_کوڈ_t cp)
{
      size_t pid = cp->نمبر;

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      إذا (arr_obj.test_nil()) أرجع سچ;

      إذا (vvp_قطار* قطار = arr_obj.peek<vvp_قطار>()) {
	    قطار->ترتیب_دیو_elems(خطأ);
	    أرجع سچ;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      أكد(darray);
      darray->ترتیب_دیو_elems(خطأ);

      أرجع سچ;
}

/* %shuffle/obj */
منطقي دا_SHUFFLE_OBJ(vthread_t /*thr*/, vvp_کوڈ_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      أكد(obj);

      vvp_object_t stor = obj->get_object();
      إذا (stor.test_nil()) أرجع سچ;

      إذا (vvp_قطار* قطار = stor.peek<vvp_قطار>()) {
	    قطار->shuffle_elems();
	    أرجع سچ;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      أكد(darray);
      darray->shuffle_elems();

      أرجع سچ;
}

/* %shuffle/prop/obj */
منطقي دا_SHUFFLE_PROP_OBJ(vthread_t thr, vvp_کوڈ_t cp)
{
      size_t pid = cp->نمبر;

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      إذا (arr_obj.test_nil()) أرجع سچ;

      إذا (vvp_قطار* قطار = arr_obj.peek<vvp_قطار>()) {
	    قطار->shuffle_elems();
	    أرجع سچ;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      أكد(darray);
      darray->shuffle_elems();

      أرجع سچ;
}

/* %delete/tail <label>, idx
 *
 * Remove all elements after the one specified.
 */
منطقي دا_DELETE_پوچھ(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      أكد(obj);

      vvp_قطار*قطار = obj->get_object().peek<vvp_قطار>();
      أكد(قطار);

      بنانشانی idx = thr->الفاظ[cp->ثنائی_ہندسہ_idx[0]].w_int;
      قطار->erase_پوچھ(idx);

      أرجع سچ;
}

ساكن منطقي do_disable(vthread_t thr, vthread_t میل)
{
      منطقي flag = خطأ;

	/* Pull the target thread out of its scope if needed. */
      thr->ماپا_scope->دھاگے.erase(thr);

	/* Turn the thread off by setting is program counter to
	   zero and setting an OFF bit. */
      thr->pc = codespace_null();
      thr->i_was_disabled = 1;
      thr->i_have_ended = 1;

	/* Turn off all the children of the thread. Simulate a %join
	   for as many times as needed to clear the results of all the
	   %forks that this thread has done. */
      طالما (! thr->children.empty()) {

	    vthread_t tmp = *(thr->children.begin());
	    أكد(tmp);
	    أكد(tmp->ماپا == thr);
	    thr->i_am_joining = 0;
	    إذا (do_disable(tmp, میل))
		  flag = سچ;

	    vthread_reap(tmp);
      }

      vthread_t ماپا = thr->ماپا;
      إذا (ماپا && ماپا->i_am_joining) {
	      // If a parent is waiting in a %join, wake it up. Note
	      // that it is possible to be waiting in a %join yet
	      // already scheduled if multiple child threads are
	      // ending. So check if the thread is already scheduled
	      // before scheduling it again.
	    ماپا->i_am_joining = 0;
	    إذا (! ماپا->i_have_ended)
		  ویلا_سارنی_vthread(ماپا, 0, سچ);

	    do_join(ماپا, thr);

      } وإلا إذا (ماپا) {
	      /* If the parent is yet to %join me, let its %join
		 do the reaping. */
	      //assert(tmp->is_scheduled == 0);

      } وإلا {
	      /* No parent at all. Goodbye. */
	    vthread_reap(thr);
      }

      أرجع flag || (thr == میل);
}

/*
 * Implement the %disable instruction by scanning the target scope for
 * all the target threads. Kill the target threads and wake up a
 * parent that is attempting a %join.
 */
منطقي دا_DISABLE(vthread_t thr, vvp_کوڈ_t cp)
{
      __vpiScope*scope = static_cast<__vpiScope*>(cp->handle);

      منطقي disabled_myself_flag = خطأ;

      طالما (! scope->دھاگے.empty()) {
	    سیٹ<vthread_t>::iterator cur = scope->دھاگے.begin();

	    إذا (do_disable(*cur, thr))
		  disabled_myself_flag = سچ;
      }

      أرجع ! disabled_myself_flag;
}

/*
 * Similar to `of_DISABLE`. But will only disable a single thread of the
 * specified scope. The disabled thread will be the thread closest to the
 * current thread in thread hierarchy. This can either be the current thread,
 * either the thread itself or one of its parents.
 * This is used for SystemVerilog flow control instructions like `return`,
 * `continue` and `break`.
 */

منطقي دا_DISABLE_FLOW(vthread_t thr, vvp_کوڈ_t cp)
{
      ثابت __vpiScope*scope = static_cast<__vpiScope*>(cp->handle);
      vthread_t cur = thr;

      طالما (cur && cur->ماپا_scope != scope)
	    cur = cur->ماپا;

      أكد(cur);
      إذا (cur)
	    أرجع !do_disable(cur, thr);

      أرجع خطأ;
}

/*
 * Implement the %disable/fork (SystemVerilog) instruction by disabling
 * all the detached children of the given thread.
 */
منطقي دا_DISABLE_FORK(vthread_t thr, vvp_کوڈ_t)
{
	/* If a %disable/fork is being executed then the parent thread
	 * cannot be waiting in a join. */
      أكد(! thr->i_am_joining);

	/* There should be no active children to disable. */
      أكد(thr->children.empty());

	/* Disable any detached children. */
      طالما (! thr->detached_children.empty()) {
	    vthread_t بچہ = *(thr->detached_children.begin());
	    أكد(بچہ);
	    أكد(بچہ->ماپا == thr);
	      /* Disabling the children can never match the parent thread. */
	    منطقي res = do_disable(بچہ, thr);
	    أكد(! res);
	    vthread_reap(بچہ);
      }

      أرجع سچ;
}

/*
 * This function divides a 2-word number {high, a} by a 1-word
 * number. Assume that high < b.
 */
ساكن بنانشانی لما divide2words(بنانشانی لما a, بنانشانی لما b,
				  بنانشانی لما بلند)
{
      بنانشانی لما نتیجہ = 0;
      طالما (بلند > 0) {
	    بنانشانی لما tmp_نتیجہ = ULONG_MAX / b;
	    بنانشانی لما remain = ULONG_MAX % b;

	    remain += 1;
	    إذا (remain >= b) {
		  remain -= b;
		  tmp_نتیجہ += 1;
	    }

	      // Now 0x1_0...0 = b*tmp_result + remain
	      // high*0x1_0...0 = high*(b*tmp_result + remain)
	      // high*0x1_0...0 = high*b*tmp_result + high*remain

	      // We know that high*0x1_0...0 >= high*b*tmp_result, and
	      // we know that high*0x1_0...0 > high*remain. Use
	      // high*remain as the remainder for another iteration,
	      // and add tmp_result*high into the current estimate of
	      // the result.
	    نتیجہ += tmp_نتیجہ * بلند;

	      // The new iteration starts with high*remain + a.
	    remain = multiply_with_حاصل(بلند, remain, بلند);
	    a += remain;
            إذا(a < remain)
              بلند += 1;

	      // Now result*b + {high,a} == the input {high,a}. It is
	      // possible that the new high >= 1. If so, it will
	      // certainly be less than high from the previous
	      // iteration. Do another iteration and it will shrink,
	      // eventually to 0.
      }

	// high is now 0, so a is the remaining remainder, so we can
	// finish off the integer divide with a simple a/b.

      أرجع نتیجہ + a/b;
}

ساكن بنانشانی لما* divide_بٹ(بنانشانی لما*ap, ثابت بنانشانی لما*bp, بنانشانی wid)
{
	// Do all our work a cpu-word at a time. The "words" variable
	// is the number of words of the wid.
      بنانشانی الفاظ = (wid+CPU_لفظ_بٹ-1) / CPU_لفظ_بٹ;

      بنانشانی btop = الفاظ-1;
      طالما (btop > 0 && bp[btop] == 0)
	    btop -= 1;

	// Detect divide by 0, and exit.
      إذا (btop==0 && bp[0]==0)
	    أرجع 0;

	// The result array will eventually accumulate the result. The
	// diff array is a difference that we use in the intermediate.
      بنانشانی لما*diff  = جديد بنانشانی لما[الفاظ];
      بنانشانی لما*نتیجہ= جديد بنانشانی لما[الفاظ];
      لكل (بنانشانی idx = 0 ; idx < الفاظ ; idx += 1)
	    نتیجہ[idx] = 0;

      لكل (بنانشانی cur = الفاظ-btop ; cur > 0 ; cur -= 1) {
	    بنانشانی cur_ptr = cur-1;
	    بنانشانی لما cur_res;
	    إذا (ap[cur_ptr+btop] >= bp[btop]) {
		  بنانشانی لما بلند = 0;
		  إذا (cur_ptr+btop+1 < الفاظ)
			بلند = ap[cur_ptr+btop+1];
		  cur_res = divide2words(ap[cur_ptr+btop], bp[btop], بلند);

	    } وإلا إذا (cur_ptr+btop+1 >= الفاظ) {
		  استمر;

	    } وإلا إذا (ap[cur_ptr+btop+1] == 0) {
		  استمر;

	    } وإلا {
		  cur_res = divide2words(ap[cur_ptr+btop], bp[btop],
					 ap[cur_ptr+btop+1]);
	    }

	      // cur_res is a guesstimate of the result this far. It
	      // may be 1 too big. (But it will also be >0) Try it,
	      // and if the difference comes out negative, then adjust.

	      // diff = (bp * cur_res)  << cur_ptr;
	    multiply_array_imm(diff+cur_ptr, bp, الفاظ-cur_ptr, cur_res);
	      // ap -= diff
	    بنانشانی لما حاصل = 1;
	    لكل (بنانشانی idx = cur_ptr ; idx < الفاظ ; idx += 1)
		  ap[idx] = جوڑو_with_حاصل(ap[idx], ~diff[idx], حاصل);

	      // ap has the diff subtracted out of it. If cur_res was
	      // too large, then ap will turn negative. (We easily
	      // tell that ap turned negative by looking at
	      // carry&1. If it is 0, then it is *negative*.) In that
	      // case, we know that cur_res was too large by 1. Correct by
	      // adding 1b back in and reducing cur_res.
	    إذا ((حاصل&1) == 0) {
		    // Keep adding b back in until the remainder
		    // becomes positive again.
		  افعل {
			cur_res -= 1;
			حاصل = 0;
			لكل (بنانشانی idx = cur_ptr ; idx < الفاظ ; idx += 1)
			      ap[idx] = جوڑو_with_حاصل(ap[idx], bp[idx-cur_ptr], حاصل);
		  } طالما (حاصل == 0);
	    }

	    نتیجہ[cur_ptr] = cur_res;
      }

	// Now ap contains the remainder and result contains the
	// desired result. We should find that:
	//  input-a = bp * result + ap;

      مٹاؤ[]diff;
      أرجع نتیجہ;
}

/*
 * %div
 */
منطقي دا_DIV(vthread_t thr, vvp_کوڈ_t)
{
      vvp_vector4_t valb = thr->نکالو_vec4();
      vvp_vector4_t vala = thr->نکالو_vec4();

      أكد(vala.size()== valb.size());
      بنانشانی wid = vala.size();

      بنانشانی لما*ap = vala.subarray(0, wid);
      إذا (ap == 0) {
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->ڈالو_vec4(tmp);
	    أرجع سچ;
      }

      بنانشانی لما*bp = valb.subarray(0, wid);
      إذا (bp == 0) {
	    مٹاؤ[]ap;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->ڈالو_vec4(tmp);
	    أرجع سچ;
      }

	// If the value fits in a single CPU word, then do it the easy way.
      إذا (wid <= CPU_لفظ_بٹ) {
	    إذا (bp[0] == 0) {
		  vvp_vector4_t tmp(wid, BIT4_X);
		  thr->ڈالو_vec4(tmp);
	    } وإلا {
		  ap[0] /= bp[0];
		  vala.setarray(0, wid, ap);
		  thr->ڈالو_vec4(vala);
	    }
	    مٹاؤ[]ap;
	    مٹاؤ[]bp;
	    أرجع سچ;
      }

      بنانشانی لما*نتیجہ = divide_بٹ(ap, bp, wid);
      إذا (نتیجہ == 0) {
	    مٹاؤ[]ap;
	    مٹاؤ[]bp;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->ڈالو_vec4(tmp);
	    أرجع سچ;
      }

	// Now ap contains the remainder and result contains the
	// desired result. We should find that:
	//  input-a = bp * result + ap;

      vala.setarray(0, wid, نتیجہ);
      thr->ڈالو_vec4(vala);
      مٹاؤ[]ap;
      مٹاؤ[]bp;
      مٹاؤ[]نتیجہ;

      أرجع سچ;
}


ساكن فراغ negate_الفاظ(بنانشانی لما*val, بنانشانی الفاظ)
{
      بنانشانی لما حاصل = 1;
      لكل (بنانشانی idx = 0 ; idx < الفاظ ; idx += 1)
	    val[idx] = جوڑو_with_حاصل(0, ~val[idx], حاصل);
}

/*
 * %div/s
 */
منطقي دا_DIV_S(vthread_t thr, vvp_کوڈ_t)
{
      vvp_vector4_t valb = thr->نکالو_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      أكد(vala.size()== valb.size());
      بنانشانی wid = vala.size();
      بنانشانی الفاظ = (wid + CPU_لفظ_بٹ - 1) / CPU_لفظ_بٹ;

	// Get the values, left in right, in binary form. If there is
	// a problem with either (caused by an X or Z bit) then we
	// know right away that the entire result is X.
      بنانشانی لما*ap = vala.subarray(0, wid);
      إذا (ap == 0) {
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    أرجع سچ;
      }

      بنانشانی لما*bp = valb.subarray(0, wid);
      إذا (bp == 0) {
	    مٹاؤ[]ap;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    أرجع سچ;
      }

	// Sign extend the bits in the array to fill out the array.
      بنانشانی لما sign_نقاب = 0;
      إذا (بنانشانی لما sign_بٹ = (الفاظ*CPU_لفظ_بٹ) - wid) {
	    sign_نقاب = -1UL << (CPU_لفظ_بٹ-sign_بٹ);
	    إذا (ap[الفاظ-1] & (sign_نقاب>>1))
		  ap[الفاظ-1] |= sign_نقاب;
	    إذا (bp[الفاظ-1] & (sign_نقاب>>1))
		  bp[الفاظ-1] |= sign_نقاب;
      }

	// If the value fits in a single word, then use the native divide.
      إذا (wid <= CPU_لفظ_بٹ) {
	    إذا (bp[0] == 0) {
		  vvp_vector4_t tmp(wid, BIT4_X);
		  vala = tmp;
	    } وإلا إذا (((لما)ap[0] == LONG_MIN) && ((لما)bp[0] == -1)) {
		  vvp_vector4_t tmp(wid, BIT4_0);
		  tmp.سیٹ_ثنائی_ہندسہ(wid-1, BIT4_1);
		  vala = tmp;
	    } وإلا {
		  لما tmpa = (لما) ap[0];
		  لما tmpb = (لما) bp[0];
		  لما res = tmpa / tmpb;
		  ap[0] = ((بنانشانی لما)res) & ~sign_نقاب;
		  vala.setarray(0, wid, ap);
	    }
	    مٹاؤ[]ap;
	    مٹاؤ[]bp;
	    أرجع سچ;
      }

	// We need to the actual division to positive integers. Make
	// them positive here, and remember the negations.
      منطقي negate_flag = خطأ;
      إذا ( ((لما) ap[الفاظ-1]) < 0 ) {
	    negate_flag = سچ;
	    negate_الفاظ(ap, الفاظ);
      }
      إذا ( ((لما) bp[الفاظ-1]) < 0 ) {
	    negate_flag ^= سچ;
	    negate_الفاظ(bp, الفاظ);
      }

      بنانشانی لما*نتیجہ = divide_بٹ(ap, bp, wid);
      إذا (نتیجہ == 0) {
	    مٹاؤ[]ap;
	    مٹاؤ[]bp;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    أرجع سچ;
      }

      إذا (negate_flag) {
	    negate_الفاظ(نتیجہ, الفاظ);
      }

      نتیجہ[الفاظ-1] &= ~sign_نقاب;

      vala.setarray(0, wid, نتیجہ);
      مٹاؤ[]ap;
      مٹاؤ[]bp;
      مٹاؤ[]نتیجہ;
      أرجع سچ;
}

منطقي دا_DIV_WR(vthread_t thr, vvp_کوڈ_t)
{
      مزدوج r = thr->نکالو_real();
      مزدوج l = thr->نکالو_real();
      thr->ڈالو_real(l / r);

      أرجع سچ;
}

/*
 * %dup/obj
 * %dup/real
 * %dup/vec4
 *
 * Push a duplicate of the object on the appropriate stack.
 */
منطقي دا_DUP_OBJ(vthread_t thr, vvp_کوڈ_t)
{
      vvp_object_t src = thr->peek_object();

        // If it is null push a new null object
      إذا (src.test_nil())
	    thr->ڈالو_object(vvp_object_t());
      وإلا
	    thr->ڈالو_object(src.duplicate());

      أرجع سچ;
}

منطقي دا_DUP_REAL(vthread_t thr, vvp_کوڈ_t)
{
      thr->ڈالو_real(thr->peek_real(0));
      أرجع سچ;
}

منطقي دا_DUP_VEC4(vthread_t thr, vvp_کوڈ_t)
{
      thr->ڈالو_vec4(thr->peek_vec4(0));
      أرجع سچ;
}

/*
 * This terminates the current thread. If there is a parent who is
 * waiting for me to die, then I schedule it. At any rate, I mark
 * myself as a zombie by setting my pc to 0.
 */
منطقي دا_سرا(vthread_t thr, vvp_کوڈ_t)
{
      أكد(! thr->اڈیک_for_event);
      thr->i_have_ended = 1;
      thr->pc = codespace_null();

	/* Fully detach any detached children. */
      طالما (! thr->detached_children.empty()) {
	    vthread_t بچہ = *(thr->detached_children.begin());
	    أكد(بچہ);
	    أكد(بچہ->ماپا == thr);
	    أكد(بچہ->i_am_detached);
	    بچہ->ماپا = 0;
	    بچہ->i_am_detached = 0;
	    thr->detached_children.erase(thr->detached_children.begin());
      }

	/* It is an error to still have active children running at this
	 * point in time. They should have all been detached or joined. */
      أكد(thr->children.empty());

	/* If I have a parent who is waiting for me, then mark that I
	   have ended, and schedule that parent. Also, finish the
	   %join for the parent. */
      إذا (!thr->i_am_detached && thr->ماپا && thr->ماپا->i_am_joining) {
	    vthread_t tmp = thr->ماپا;
	    أكد(! thr->i_am_detached);

	    tmp->i_am_joining = 0;
	    ویلا_سارنی_vthread(tmp, 0, سچ);
	    do_join(tmp, thr);
	    أرجع خطأ;
      }

	/* If this thread is not fully detached then remove it from the
	 * parents detached_children set and reap it. */
      إذا (thr->i_am_detached) {
	    vthread_t tmp = thr->ماپا;
	    أكد(tmp);
	    size_t res = tmp->detached_children.erase(thr);
	    أكد(res == 1);
	      /* If the parent is waiting for the detached children to
	       * finish then the last detached child needs to tell the
	       * parent to wake up when it is finished. */
	    إذا (tmp->i_am_اڈیک && tmp->detached_children.empty()) {
		  tmp->i_am_اڈیک = 0;
		  ویلا_سارنی_vthread(tmp, 0, سچ);
	    }
	      /* Fully detach this thread so it will be reaped below. */
	    thr->i_am_detached = 0;
	    thr->ماپا = 0;
      }

	/* If I have no parent, then no one can %join me and there is
	 * no reason to stick around. This can happen, for example if
	 * I am an ``initial'' thread. */
      إذا (thr->ماپا == 0) {
	    vthread_reap(thr);
	    أرجع خطأ;
      }

	/* If I make it this far, then I have a parent who may wish
	   to %join me. Remain a zombie so that it can. */

      أرجع خطأ;
}

/*
 * %event <var-label>
 */
منطقي دا_EVENT(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_vector4_t tmp (1, BIT4_X);
      vvp_send_vec4(ptr, tmp, thr->wt_context);
      أرجع سچ;
}

/*
 * %event/nb <var-label>, <delay>
 */
منطقي دا_EVENT_NB(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_time64_t delay;

      delay = thr->الفاظ[cp->ثنائی_ہندسہ_idx[0]].w_uint;
      ویلا_سارنی_propagate_event(cp->net, delay);
      أرجع سچ;
}

منطقي دا_EVCTL(vthread_t thr, vvp_کوڈ_t cp)
{
      أكد(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      thr->ecount = thr->الفاظ[cp->ثنائی_ہندسہ_idx[0]].w_uint;
      أرجع سچ;
}
منطقي دا_EVCTLC(vthread_t thr, vvp_کوڈ_t)
{
      thr->event = 0;
      thr->ecount = 0;
      أرجع سچ;
}

منطقي دا_EVCTLI(vthread_t thr, vvp_کوڈ_t cp)
{
      أكد(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      thr->ecount = cp->ثنائی_ہندسہ_idx[0];
      أرجع سچ;
}

منطقي دا_EVCTLS(vthread_t thr, vvp_کوڈ_t cp)
{
      أكد(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      int64_t val = thr->الفاظ[cp->ثنائی_ہندسہ_idx[0]].w_int;
      إذا (val < 0) val = 0;
      thr->ecount = val;
      أرجع سچ;
}

منطقي دا_FLAG_GET_VEC4(vthread_t thr, vvp_کوڈ_t cp)
{
      صحيح flag = cp->نمبر;
      أكد(flag < vthread_s::FLAGS_گنتی);

      vvp_vector4_t val (1, thr->flags[flag]);
      thr->ڈالو_vec4(val);

      أرجع سچ;
}

/*
 * %flag_inv <flag1>
 */
منطقي دا_FLAG_INV(vthread_t thr, vvp_کوڈ_t cp)
{
      صحيح flag1 = cp->ثنائی_ہندسہ_idx[0];

      thr->flags[flag1] = ~ thr->flags[flag1];
      أرجع سچ;
}

/*
 * %flag_mov <flag1>, <flag2>
 */
منطقي دا_FLAG_MOV(vthread_t thr, vvp_کوڈ_t cp)
{
      صحيح flag1 = cp->ثنائی_ہندسہ_idx[0];
      صحيح flag2 = cp->ثنائی_ہندسہ_idx[1];

      thr->flags[flag1] = thr->flags[flag2];
      أرجع سچ;
}

/*
 * %flag_or <flag1>, <flag2>
 */
منطقي دا_FLAG_OR(vthread_t thr, vvp_کوڈ_t cp)
{
      صحيح flag1 = cp->ثنائی_ہندسہ_idx[0];
      صحيح flag2 = cp->ثنائی_ہندسہ_idx[1];

      thr->flags[flag1] = thr->flags[flag1] | thr->flags[flag2];
      أرجع سچ;
}

منطقي دا_FLAG_سیٹ_IMM(vthread_t thr, vvp_کوڈ_t cp)
{
      صحيح flag = cp->نمبر;
      صحيح vali = cp->ثنائی_ہندسہ_idx[0];

      أكد(flag < vthread_s::FLAGS_گنتی);
      أكد(vali >= 0 && vali < 4);

      ساكن ثابت vvp_bit4_t map_ثنائی_ہندسہ[4] = {BIT4_0, BIT4_1, BIT4_Z, BIT4_X};
      thr->flags[flag] = map_ثنائی_ہندسہ[vali];
      أرجع سچ;
}

منطقي دا_FLAG_سیٹ_VEC4(vthread_t thr, vvp_کوڈ_t cp)
{
      صحيح flag = cp->نمبر;
      أكد(flag < vthread_s::FLAGS_گنتی);

      ثابت vvp_vector4_t&val = thr->peek_vec4();
      thr->flags[flag] = val.value(0);
      thr->نکالو_vec4(1);

      أرجع سچ;
}

/*
 * the %force/link instruction connects a source node to a
 * destination node. The destination node must be a signal, as it is
 * marked with the source of the force so that it may later be
 * unlinked without specifically knowing the source that this
 * instruction used.
 */
منطقي دا_FORCE_LINK(vthread_t, vvp_کوڈ_t cp)
{
      vvp_net_t*dst = cp->net;
      vvp_net_t*src = cp->net2;

      أكد(dst->fil);
      dst->fil->force_link(dst, src);

      أرجع سچ;
}

/*
 * The %force/vec4 instruction invokes a force assign of a constant value
 * to a signal. The instruction arguments are:
 *
 *     %force/vec4 <net> ;
 *
 * where the <net> is the net label assembled into a vvp_net pointer,
 * and the value to be forced is popped from the vec4 stack.\.
 *
 * The instruction writes a vvp_vector4_t value to port-2 of the
 * target signal.
 */
منطقي دا_FORCE_VEC4(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_vector4_t value = thr->نکالو_vec4();

	/* Send the force value to the filter on the node. */

      أكد(net->fil);
      إذا (value.size() != net->fil->filter_ناپ())
	    value = coerce_منزل_چوڑائی(value, net->fil->filter_ناپ());

      net->force_vec4(value, vvp_vector2_t(vvp_vector2_t::FILL1, net->fil->filter_ناپ()));

      أرجع سچ;
}

/*
 * %force/vec4/off <net>, <off>
 */
منطقي دا_FORCE_VEC4_OFF(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_net_t*net = cp->net;
      بنانشانی ادھار_idx = cp->ثنائی_ہندسہ_idx[0];
      لما ادھار = thr->الفاظ[ادھار_idx].w_int;
      vvp_vector4_t value = thr->نکالو_vec4();
      بنانشانی wid = value.size();

      أكد(net->fil);

      إذا (thr->flags[4] == BIT4_1)
	    أرجع سچ;

	// This is the width of the target vector.
      بنانشانی use_ناپ = net->fil->filter_ناپ();

      إذا (ادھار >= (لما)use_ناپ)
	    أرجع سچ;
      إذا (ادھار < -(لما)use_ناپ)
	    أرجع سچ;

      إذا ((ادھار + wid) > use_ناپ)
	    wid = use_ناپ - ادھار;

	// Make a mask of which bits are to be forced, 0 for unforced
	// bits and 1 for forced bits.
      vvp_vector2_t نقاب (vvp_vector2_t::FILL0, use_ناپ);
      لكل (بنانشانی idx = 0 ; idx < wid ; idx += 1)
	    نقاب.سیٹ_ثنائی_ہندسہ(ادھار+idx, 1);

      vvp_vector4_t tmp (use_ناپ, BIT4_Z);

	// vvp_net_t::force_vec4 propagates all the bits of the
	// forced vector value, regardless of the mask. This
	// ensures the unforced bits retain their current value.
      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*>(net->fil);
      أكد(sig);
      sig->vec4_قدر(tmp);

      tmp.سیٹ_vec(ادھار, value);

      net->force_vec4(tmp, نقاب);
      أرجع سچ;
}

/*
 * %force/vec4/off/d <net>, <off>, <del>
 */
منطقي دا_FORCE_VEC4_OFF_D(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_net_t*net = cp->net;

      بنانشانی ادھار_idx = cp->ثنائی_ہندسہ_idx[0];
      لما ادھار = thr->الفاظ[ادھار_idx].w_int;

      بنانشانی delay_idx = cp->ثنائی_ہندسہ_idx[1];
      vvp_time64_t delay = thr->الفاظ[delay_idx].w_uint;

      vvp_vector4_t value = thr->نکالو_vec4();

      أكد(net->fil);

      إذا (thr->flags[4] == BIT4_1)
	    أرجع سچ;

	// This is the width of the target vector.
      بنانشانی use_ناپ = net->fil->filter_ناپ();

      إذا (ادھار >= (لما)use_ناپ)
	    أرجع سچ;
      إذا (ادھار < -(لما)use_ناپ)
	    أرجع سچ;

      ویلا_سارنی_force_سمتیہ(net, ادھار, use_ناپ, value, delay);
      أرجع سچ;
}

منطقي دا_FORCE_WR(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_net_t*net  = cp->net;
      مزدوج value = thr->نکالو_real();

      net->force_real(value, vvp_vector2_t(vvp_vector2_t::FILL1, 1));

      أرجع سچ;
}

/*
 * The %fork instruction causes a new child to be created and pushed
 * in front of any existing child. This causes the new child to be
 * added to the list of children, and for me to be the parent of the
 * new child.
 */
منطقي دا_FORK(vthread_t thr, vvp_کوڈ_t cp)
{
      vthread_t بچہ = vthread_new(cp->cptr2, cp->scope);

      إذا (cp->scope->is_automatic()) {
              /* The context allocated for this child is the top entry
                 on the write context stack. */
            بچہ->wt_context = thr->wt_context;
            بچہ->rd_context = thr->wt_context;
      }

      بچہ->ماپا = thr;
      thr->children.insert(بچہ);

      إذا (thr->i_am_وچ_function) {
	    بچہ->is_scheduled = 1;
	    بچہ->i_am_وچ_function = 1;
	    vthread_لگاتار(بچہ);
	    running_thread = thr;
      } وإلا {
	    ویلا_سارنی_vthread(بچہ, 0, سچ);
      }
      أرجع سچ;
}

منطقي دا_FREE(vthread_t thr, vvp_کوڈ_t cp)
{
        /* Pop the child context from the read context stack. */
      vvp_context_t بچہ_context = thr->rd_context;
      thr->rd_context = vvp_get_stacked_context(بچہ_context);

        /* Free the context. */
      vthread_free_context(بچہ_context, cp->scope);

      أرجع سچ;
}

/*
 * %inv
 *
 * Logically, this pops a value, inverts is (Verilog style, with Z and
 * X converted to X) and pushes the result. We can more efficiently
 * just to the invert in place.
 */
منطقي دا_INV(vthread_t thr, vvp_کوڈ_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val.invert();
      أرجع سچ;
}


/*
 * Index registers, arithmetic.
 */

ساكن مضمن int64_t get_as_64_ثنائی_ہندسہ(uint32_t کم_32, uint32_t بلند_32)
{
      int64_t کم = کم_32;
      int64_t res = بلند_32;

      res <<= 32;
      res |= کم;
      أرجع res;
}

منطقي دا_IX_جوڑو(vthread_t thr, vvp_کوڈ_t cp)
{
      thr->الفاظ[cp->نمبر].w_int += get_as_64_ثنائی_ہندسہ(cp->ثنائی_ہندسہ_idx[0],
                                                    cp->ثنائی_ہندسہ_idx[1]);
      أرجع سچ;
}

منطقي دا_IX_SUB(vthread_t thr, vvp_کوڈ_t cp)
{
      thr->الفاظ[cp->نمبر].w_int -= get_as_64_ثنائی_ہندسہ(cp->ثنائی_ہندسہ_idx[0],
                                                    cp->ثنائی_ہندسہ_idx[1]);
      أرجع سچ;
}

منطقي دا_IX_MUL(vthread_t thr, vvp_کوڈ_t cp)
{
      thr->الفاظ[cp->نمبر].w_int *= get_as_64_ثنائی_ہندسہ(cp->ثنائی_ہندسہ_idx[0],
                                                    cp->ثنائی_ہندسہ_idx[1]);
      أرجع سچ;
}

منطقي دا_IX_لدو(vthread_t thr, vvp_کوڈ_t cp)
{
      thr->الفاظ[cp->نمبر].w_int = get_as_64_ثنائی_ہندسہ(cp->ثنائی_ہندسہ_idx[0],
                                                   cp->ثنائی_ہندسہ_idx[1]);
      أرجع سچ;
}

منطقي دا_IX_MOV(vthread_t thr, vvp_کوڈ_t cp)
{
      thr->الفاظ[cp->ثنائی_ہندسہ_idx[0]].w_int = thr->الفاظ[cp->ثنائی_ہندسہ_idx[1]].w_int;
      أرجع سچ;
}

منطقي دا_IX_GETV(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی اشاریہ = cp->ثنائی_ہندسہ_idx[0];
      vvp_net_t*net = cp->net;

      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*>(net->fil);
      إذا (sig == 0) {
	    أكد(net->fil);
	    cerr << thr->get_fileline()
	         << "%%ix/getv error: Net arg not a vector signal? "
		 << typeid(*net->fil).name() << سطر_انت;
      }
      أكد(sig);

      vvp_vector4_t vec;
      sig->vec4_قدر(vec);
      منطقي حد_توں_ودھ_flag;
      uint64_t val = 0;
      منطقي معلوم_flag = vector4_منزل_قدر(vec, حد_توں_ودھ_flag, val);

      إذا (معلوم_flag)
	    thr->الفاظ[اشاریہ].w_uint = val;
      وإلا
	    thr->الفاظ[اشاریہ].w_uint = 0;

	/* Set bit 4 as a flag if the input is unknown. */
      thr->flags[4] = معلوم_flag ? (حد_توں_ودھ_flag ? BIT4_X : BIT4_0) : BIT4_1;

      أرجع سچ;
}

منطقي دا_IX_GETV_S(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی اشاریہ = cp->ثنائی_ہندسہ_idx[0];
      vvp_net_t*net = cp->net;

      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*>(net->fil);
      إذا (sig == 0) {
	    أكد(net->fil);
	    cerr << thr->get_fileline()
	         << "%%ix/getv/s error: Net arg not a vector signal? "
		 << "fun=" << typeid(*net->fil).name()
		 << ", fil=" << (net->fil? typeid(*net->fil).name() : "<>")
		 << سطر_انت;
      }
      أكد(sig);

      vvp_vector4_t vec;
      sig->vec4_قدر(vec);
      int64_t val;
      منطقي معلوم_flag = vector4_منزل_قدر(vec, val, سچ, سچ);

      إذا (معلوم_flag)
	    thr->الفاظ[اشاریہ].w_int = val;
      وإلا
	    thr->الفاظ[اشاریہ].w_int = 0;

	/* Set bit 4 as a flag if the input is unknown. */
      thr->flags[4] = معلوم_flag? BIT4_0 : BIT4_1;

      أرجع سچ;
}

ساكن uint64_t vec4_منزل_اشاریہ(vthread_t thr, منطقي signed_flag)
{
	// Get all the information we need about the vec4 vector, then
	// pop it away. We only need the bool bits and the length.
      ثابت vvp_vector4_t&val = thr->peek_vec4();
      بنانشانی val_ناپ = val.size();
      بنانشانی لما*بٹ = val.subarray(0, val_ناپ, خطأ);
      thr->نکالو_vec4(1);

	// If there are X/Z bits, then the subarray will give us a nil
	// pointer. Set a flag to indicate the error, and give up.
      إذا (بٹ == 0) {
	    thr->flags[4] = BIT4_1;
	    أرجع 0;
      }

      uint64_t v = 0;
      thr->flags[4] = BIT4_0;

      أكد(حجم(بٹ[0]) <= حجم(v));

      v = 0;
      لكل (بنانشانی idx = 0 ; idx < val_ناپ ; idx += 8*حجم(بٹ[0])) {
	    uint64_t tmp = بٹ[idx/8/حجم(بٹ[0])];
	    إذا (idx < 8*حجم(v)) {
		  v |= tmp << idx;
	    } وإلا {
		  منطقي حد_توں_ودھ = signed_flag && (v >> 63) ? ~tmp != 0 : tmp != 0;
		  إذا (حد_توں_ودھ) {
			thr->flags[4] = BIT4_X;
			اكسر;
		  }
	    }
      }

	// Set the high bits that are not necessarily filled in by the
	// subarray function.
      إذا (val_ناپ < 8*حجم(v)) {
	    إذا (signed_flag && (v & (static_cast<uint64_t>(1)<<(val_ناپ-1)))) {
		    // Propagate the sign bit...
		  v |= (~static_cast<uint64_t>(0)) << val_ناپ;

	    } وإلا {
		    // Fill with zeros.
		  v &= ~((~static_cast<uint64_t>(0)) << val_ناپ);
	    }

      }

      مٹاؤ[]بٹ;
      أرجع v;
}

/*
 * %ix/vec4 <idx>
 */
منطقي دا_IX_VEC4(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی use_idx = cp->نمبر;
      thr->الفاظ[use_idx].w_uint = vec4_منزل_اشاریہ(thr, خطأ);
      أرجع سچ;
}

/*
 * %ix/vec4/s <idx>
 */
منطقي دا_IX_VEC4_S(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی use_idx = cp->نمبر;
      thr->الفاظ[use_idx].w_uint = vec4_منزل_اشاریہ(thr, سچ);
      أرجع سچ;
}

/*
 * The various JMP instruction work simply by pulling the new program
 * counter from the instruction and resuming. If the jump is
 * conditional, then test the bit for the expected value first.
 */
منطقي دا_JMP(vthread_t thr, vvp_کوڈ_t cp)
{
      thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      إذا (ویلا_سارنی_stopped()) {
	    ویلا_سارنی_vthread(thr, 0, خطأ);
	    أرجع خطأ;
      }

      أرجع سچ;
}

/*
 * %jmp/0 <pc>, <flag>
 */
منطقي دا_JMP0(vthread_t thr, vvp_کوڈ_t cp)
{
      إذا (thr->flags[cp->ثنائی_ہندسہ_idx[0]] == BIT4_0)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      إذا (ویلا_سارنی_stopped()) {
	    ویلا_سارنی_vthread(thr, 0, خطأ);
	    أرجع خطأ;
      }

      أرجع سچ;
}

/*
 * %jmp/0xz <pc>, <flag>
 */
منطقي دا_JMP0_XZ(vthread_t thr, vvp_کوڈ_t cp)
{
      إذا (thr->flags[cp->ثنائی_ہندسہ_idx[0]] != BIT4_1)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      إذا (ویلا_سارنی_stopped()) {
	    ویلا_سارنی_vthread(thr, 0, خطأ);
	    أرجع خطأ;
      }

      أرجع سچ;
}

/*
 * %jmp/1 <pc>, <flag>
 */
منطقي دا_JMP1(vthread_t thr, vvp_کوڈ_t cp)
{
      إذا (thr->flags[cp->ثنائی_ہندسہ_idx[0]] == BIT4_1)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      إذا (ویلا_سارنی_stopped()) {
	    ویلا_سارنی_vthread(thr, 0, خطأ);
	    أرجع خطأ;
      }

      أرجع سچ;
}

/*
 * %jmp/1xz <pc>, <flag>
 */
منطقي دا_JMP1_XZ(vthread_t thr, vvp_کوڈ_t cp)
{
      إذا (thr->flags[cp->ثنائی_ہندسہ_idx[0]] != BIT4_0)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      إذا (ویلا_سارنی_stopped()) {
	    ویلا_سارنی_vthread(thr, 0, خطأ);
	    أرجع خطأ;
      }

      أرجع سچ;
}

/*
 * The %join instruction causes the thread to wait for one child
 * to die.  If a child is already dead (and a zombie) then I reap
 * it and go on. Otherwise, I mark myself as waiting in a join so that
 * children know to wake me when they finish.
 */

ساكن فراغ do_join(vthread_t thr, vthread_t بچہ)
{
      أكد(بچہ->ماپا == thr);

        /* If the immediate child thread is in an automatic scope... */
      إذا (بچہ->wt_context) {
              /* and is the top level task/function thread... */
            إذا (thr->wt_context != thr->rd_context) {
                    /* Pop the child context from the write context stack. */
                  vvp_context_t بچہ_context = thr->wt_context;
                  thr->wt_context = vvp_get_stacked_context(بچہ_context);

                    /* Push the child context onto the read context stack */
                  vvp_سیٹ_stacked_context(بچہ_context, thr->rd_context);
                  thr->rd_context = بچہ_context;
            }
      }

      vthread_reap(بچہ);
}

ساكن منطقي do_join_opcode(vthread_t thr)
{
      أكد( !thr->i_am_joining );
      أكد( !thr->children.empty());

	// Are there any children that have already ended? If so, then
	// join with that one.
      لكل (سیٹ<vthread_t>::iterator cur = thr->children.begin()
		 ; cur != thr->children.سرا() ; ++cur) {
	    vthread_t curp = *cur;
	    إذا (! curp->i_have_ended)
		  استمر;

	      // found something!
	    do_join(thr, curp);
	    أرجع سچ;
      }

	// Otherwise, tell my children to awaken me when they end,
	// then pause.
      thr->i_am_joining = 1;
      أرجع خطأ;
}

منطقي دا_JOIN(vthread_t thr, vvp_کوڈ_t)
{
      أرجع do_join_opcode(thr);
}

/*
 * This %join/detach <n> instruction causes the thread to detach
 * threads that were created by an earlier %fork.
 */
منطقي دا_JOIN_DETACH(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی لما گنتی = cp->نمبر;

      أكد(گنتی == thr->children.size());

      طالما (! thr->children.empty()) {
	    vthread_t بچہ = *thr->children.begin();
	    أكد(بچہ->ماپا == thr);

	      // We cannot detach automatic tasks/functions within an
	      // automatic scope. If we try to do that, we might make
	      // a mess of the allocation of the context. Note that it
	      // is OK if the child context is distinct (See %exec_ufunc.)
	    أكد(بچہ->wt_context==0 || thr->wt_context!=بچہ->wt_context);
	    إذا (بچہ->i_have_ended) {
		    // If the child has already ended, then reap it.
		  vthread_reap(بچہ);

	    } وإلا {
		  size_t res = بچہ->ماپا->children.erase(بچہ);
		  أكد(res == 1);
		  بچہ->i_am_detached = 1;
		  thr->detached_children.insert(بچہ);
	    }
      }

      أرجع سچ;
}

/*
 * %load/ar <array-label>, <index>;
*/
منطقي دا_لدو_AR(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی idx = cp->ثنائی_ہندسہ_idx[0];
      مزدوج لفظ;

	/* The result is 0.0 if the address is undefined. */
      إذا (thr->flags[4] == BIT4_1) {
	    لفظ = 0.0;
      } وإلا {
	    بنانشانی adr = thr->الفاظ[idx].w_int;
	    لفظ = cp->array->get_لفظ_r(adr);
      }

      thr->ڈالو_real(لفظ);
      أرجع سچ;
}

سانچا <قسم_ناں ELEM>
ساكن منطقي لدو_dar(vthread_t thr, vvp_کوڈ_t cp)
{
      int64_t adr = thr->الفاظ[3].w_int;
      vvp_net_t*net = cp->net;
      أكد(net);

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      أكد(obj);

      vvp_darray*darray = obj->get_object().peek<vvp_darray>();

      ELEM لفظ;
      إذا (darray &&
          (adr >= 0) && (thr->flags[4] == BIT4_0)) // A defined address >= 0
	    darray->get_لفظ(adr, لفظ);
      وإلا
	    dq_default(لفظ, obj->size());

      vthread_ڈالو(thr, لفظ);
      أرجع سچ;
}

/*
 * %load/dar/r <array-label>;
 */
منطقي دا_لدو_DAR_R(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع لدو_dar<مزدوج>(thr, cp);
}

/*
 * %load/dar/str <array-label>;
 */
منطقي دا_لدو_DAR_STR(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع لدو_dar<string>(thr, cp);
}

/*
 * %load/dar/vec4 <array-label>;
 */
منطقي دا_لدو_DAR_VEC4(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع لدو_dar<vvp_vector4_t>(thr, cp);
}

/*
 * %load/prop/dar/vec4 <pid>, <wid>;
 *   Indexed read of queue/dynamic-array class property; object on stack,
 *   index in words[3] (same as %load/dar/vec4).
 */
منطقي دا_لدو_PROP_DAR_VEC4(vthread_t thr, vvp_کوڈ_t cp)
{
      size_t pid = cp->نمبر;
      بنانشانی wid = cp->ثنائی_ہندسہ_idx[0];
      int64_t adr = thr->الفاظ[3].w_int;

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t pobj;
      cobj->get_object(pid, pobj, 0);
      vvp_darray*darray = pobj.peek<vvp_darray>();

      vvp_vector4_t لفظ;
      إذا (darray && (adr >= 0) && (thr->flags[4] == BIT4_0)) {
	    darray->get_لفظ(adr, لفظ);
      } وإلا {
	    dq_default(لفظ, wid);
      }

      thr->ڈالو_vec4(لفظ);
      أرجع سچ;
}

/*
 * %load/obj <var-label>
 */
منطقي دا_لدو_OBJ(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_net_t*net = cp->net;
      vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      أكد(fun);

      vvp_object_t val = fun->get_object();
      thr->ڈالو_object(val);

      أرجع سچ;
}

/*
 * %load/obja <index>
 *    Loads the object from array, using index <index> as the index
 *    value. If flags[4] == 1, the calculation of <index> may have
 *    failed, so push nil.
 */
منطقي دا_لدو_OBJA(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی idx = cp->ثنائی_ہندسہ_idx[0];
      vvp_object_t لفظ;

	/* The result is 0.0 if the address is undefined. */
      إذا (thr->flags[4] == BIT4_1) {
	    ; // Return nil
      } وإلا {
	    بنانشانی adr = thr->الفاظ[idx].w_int;
	    cp->array->get_لفظ_obj(adr, لفظ);
      }

      thr->ڈالو_object(لفظ);
      أرجع سچ;
}

/*
 * %load/real <var-label>
 */
منطقي دا_لدو_REAL(vthread_t thr, vvp_کوڈ_t cp)
{
      __vpiHandle*tmp = cp->handle;
      t_vpi_قدر val;

      val.format = vpiRealVal;
      vpi_get_قدر(tmp, &val);

      thr->ڈالو_real(val.value.real);

      أرجع سچ;
}

/*
 * %load/str <var-label>
 */
منطقي دا_لدو_STR(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_net_t*net = cp->net;


      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (net->fun);
      أكد(fun);

      ثابت string&val = fun->get_string();
      thr->ڈالو_str(val);

      أرجع سچ;
}

منطقي دا_لدو_STRA(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی idx = cp->ثنائی_ہندسہ_idx[0];
      string لفظ;

      إذا (thr->flags[4] == BIT4_1) {
	    لفظ = "";
      } وإلا {
	    بنانشانی adr = thr->الفاظ[idx].w_int;
	    لفظ = cp->array->get_لفظ_str(adr);
      }

      thr->ڈالو_str(لفظ);
      أرجع سچ;
}


/*
 * %load/vec4 <net>
 */
منطقي دا_لدو_VEC4(vthread_t thr, vvp_کوڈ_t cp)
{
	// Push a placeholder onto the stack in order to reserve the
	// stack space. Use a reference for the stack top as a target
	// for the load.
      thr->ڈالو_vec4(vvp_vector4_t());
      vvp_vector4_t&sig_قدر = thr->peek_vec4();

      vvp_net_t*net = cp->net;

	// For the %load to work, the functor must actually be a
	// signal functor. Only signals save their vector value.
      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*> (net->fil);
      إذا (sig == 0) {
	    cerr << thr->get_fileline()
	         << "%load/v error: Net arg not a signal? "
		 << (net->fil ? typeid(*net->fil).name() :
	                        typeid(*net->fun).name())
	         << سطر_انت;
	    أكد(sig);
	    أرجع سچ;
      }

	// Extract the value from the signal and directly into the
	// target stack position.
      sig->vec4_قدر(sig_قدر);

      أرجع سچ;
}

/*
 * %load/vec4a <arr>, <adrx>
 */
منطقي دا_لدو_VEC4_A(vthread_t thr, vvp_کوڈ_t cp)
{
      صحيح adr_اشاریہ = cp->ثنائی_ہندسہ_idx[0];

      لما adr = thr->الفاظ[adr_اشاریہ].w_int;

	// If flag[3] is set, then the calculation of the address
	// failed, and this load should return X instead of the actual
	// value.
      إذا (thr->flags[4] == BIT4_1) {
	    vvp_vector4_t tmp (cp->array->get_لفظ_ناپ(), BIT4_X);
	    thr->ڈالو_vec4(tmp);
	    أرجع سچ;
      }

      vvp_vector4_t tmp (cp->array->get_لفظ(adr));
      thr->ڈالو_vec4(tmp);
      أرجع سچ;
}

ساكن فراغ do_verylong_mod(vvp_vector4_t&vala, ثابت vvp_vector4_t&valb,
			    منطقي بایاں_is_neg, منطقي دایاں_is_neg)
{
      منطقي باہرلا_is_neg = بایاں_is_neg;
      ثابت صحيح len=vala.size();
      بنانشانی حرف *a, *z, *t;
      a = جديد بنانشانی حرف[len+1];
      z = جديد بنانشانی حرف[len+1];
      t = جديد بنانشانی حرف[len+1];

      بنانشانی حرف حاصل;
      بنانشانی حرف عارضی;

      صحيح mxa = -1, mxz = -1;
      صحيح i;
      صحيح current, copylen;

      بنانشانی lb_حاصل = بایاں_is_neg? 1 : 0;
      بنانشانی rb_حاصل = دایاں_is_neg? 1 : 0;
      لكل (صحيح idx = 0 ;  idx < len ;  idx += 1) {
	    بنانشانی lb = vala.value(idx);
	    بنانشانی rb = valb.value(idx);

	    إذا ((lb | rb) & 2) {
		  مٹاؤ []t;
		  مٹاؤ []z;
		  مٹاؤ []a;
		  vvp_vector4_t tmp(len, BIT4_X);
		  vala = tmp;
		  أرجع;
	    }

	    إذا (بایاں_is_neg) {
		  lb = (1-lb) + lb_حاصل;
		  lb_حاصل = (lb & ~1)? 1 : 0;
		  lb &= 1;
	    }
	    إذا (دایاں_is_neg) {
		  rb = (1-rb) + rb_حاصل;
		  rb_حاصل = (rb & ~1)? 1 : 0;
		  rb &= 1;
	    }

	    z[idx]=lb;
	    a[idx]=1-rb;	// for 2s complement add..
      }

      z[len]=0;
      a[len]=1;

      لكل(i=len-1;i>=0;i--) {
	    إذا(! a[i]) {
		  mxa=i;
		  اكسر;
	    }
      }

      لكل(i=len-1;i>=0;i--) {
	    إذا(z[i]) {
		  mxz=i;
		  اكسر;
	    }
      }

      إذا((mxa>mxz)||(mxa==-1)) {
	    إذا(mxa==-1) {
		  مٹاؤ []t;
		  مٹاؤ []z;
		  مٹاؤ []a;
		  vvp_vector4_t tmpx (len, BIT4_X);
		  vala = tmpx;
		  أرجع;
	    }

	    اذهب tally;
      }

      copylen = mxa + 2;
      current = mxz - mxa;

      طالما(current > -1) {
	    حاصل = 1;
	    لكل(i=0;i<copylen;i++) {
		  عارضی = z[i+current] + a[i] + حاصل;
		  t[i] = (عارضی&1);
		  حاصل = (عارضی>>1);
	    }

	    إذا(حاصل) {
		  لكل(i=0;i<copylen;i++) {
			z[i+current] = t[i];
		  }
	    }

	    current--;
      }

 tally:

      vvp_vector4_t tmp (len, BIT4_X);
      حاصل = باہرلا_is_neg? 1 : 0;
      لكل (صحيح idx = 0 ;  idx < len ;  idx += 1) {
	    بنانشانی ob = z[idx];
	    إذا (باہرلا_is_neg) {
		  ob = (1-ob) + حاصل;
		  حاصل = (ob & ~1)? 1 : 0;
		  ob = ob & 1;
	    }
	    tmp.سیٹ_ثنائی_ہندسہ(idx, ob?BIT4_1:BIT4_0);
      }
      vala = tmp;
      مٹاؤ []t;
      مٹاؤ []z;
      مٹاؤ []a;
}

منطقي دا_MAX_WR(vthread_t thr, vvp_کوڈ_t)
{
      مزدوج r = thr->نکالو_real();
      مزدوج l = thr->نکالو_real();
      إذا (r != r)
	    thr->ڈالو_real(l);
      وإلا إذا (l != l)
	    thr->ڈالو_real(r);
      وإلا إذا (r < l)
	    thr->ڈالو_real(l);
      وإلا
	    thr->ڈالو_real(r);
      أرجع سچ;
}

منطقي دا_MIN_WR(vthread_t thr, vvp_کوڈ_t)
{
      مزدوج r = thr->نکالو_real();
      مزدوج l = thr->نکالو_real();
      إذا (r != r)
	    thr->ڈالو_real(l);
      وإلا إذا (l != l)
	    thr->ڈالو_real(r);
      وإلا إذا (r < l)
	    thr->ڈالو_real(r);
      وإلا
	    thr->ڈالو_real(l);
      أرجع سچ;
}

منطقي دا_MOD(vthread_t thr, vvp_کوڈ_t)
{
      vvp_vector4_t valb = thr->نکالو_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      أكد(vala.size()==valb.size());
      بنانشانی wid = vala.size();

      إذا(wid <= 8*حجم(بنانشانی لما لما)) {
	    بنانشانی لما لما lv = 0, rv = 0;

	    لكل (بنانشانی idx = 0 ;  idx < wid ;  idx += 1) {
		  بنانشانی لما لما lb = vala.value(idx);
		  بنانشانی لما لما rb = valb.value(idx);

		  إذا ((lb | rb) & 2)
			اذهب x_باہرلا;

		  lv |= (بنانشانی لما لما) lb << idx;
		  rv |= (بنانشانی لما لما) rb << idx;
	    }

	    إذا (rv == 0)
		  اذهب x_باہرلا;

	    lv %= rv;

	    لكل (بنانشانی idx = 0 ;  idx < wid ;  idx += 1) {
		  vala.سیٹ_ثنائی_ہندسہ(idx, (lv&1)?BIT4_1 : BIT4_0);
		  lv >>= 1;
	    }

	    أرجع سچ;

      } وإلا {
	    do_verylong_mod(vala, valb, خطأ, خطأ);
	    أرجع سچ;
      }

 x_باہرلا:
      vala = vvp_vector4_t(wid, BIT4_X);
      أرجع سچ;
}

/*
 * %mod/s
 */
منطقي دا_MOD_S(vthread_t thr, vvp_کوڈ_t)
{
      vvp_vector4_t valb = thr->نکالو_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      أكد(vala.size()==valb.size());
      بنانشانی wid = vala.size();

	/* Handle the case that we can fit the bits into a long-long
	   variable. We cause use native % to do the work. */
      إذا(wid <= 8*حجم(لما لما)) {
	    لما لما lv = 0, rv = 0;

	    لكل (بنانشانی idx = 0 ;  idx < wid ;  idx += 1) {
		  لما لما lb = vala.value(idx);
		  لما لما rb = valb.value(idx);

		  إذا ((lb | rb) & 2)
			اذهب x_باہرلا;

		  lv |= (لما لما) lb << idx;
		  rv |= (لما لما) rb << idx;
	    }

	    إذا (rv == 0)
		  اذهب x_باہرلا;

	    إذا ((lv == LLONG_MIN) && (rv == -1))
		  اذهب zero_باہرلا;

	      /* Sign extend the signed operands when needed. */
	    إذا (wid < 8*حجم(لما لما)) {
		  إذا (lv & (1LL << (wid-1)))
			lv |= -1ULL << wid;
		  إذا (rv & (1LL << (wid-1)))
			rv |= -1ULL << wid;
	    }

	    lv %= rv;

	    لكل (بنانشانی idx = 0 ;  idx < wid ;  idx += 1) {
		  vala.سیٹ_ثنائی_ہندسہ(idx, (lv&1)? BIT4_1 : BIT4_0);
		  lv >>= 1;
	    }

	      // vala is the top of the stack, edited in place, so we
	      // do not need to push the result.

	    أرجع سچ;

      } وإلا {

	    منطقي بایاں_is_neg  = vala.value(vala.size()-1) == BIT4_1;
	    منطقي دایاں_is_neg = valb.value(valb.size()-1) == BIT4_1;
	    do_verylong_mod(vala, valb, بایاں_is_neg, دایاں_is_neg);
	    أرجع سچ;
      }

 x_باہرلا:
      vala = vvp_vector4_t(wid, BIT4_X);
      أرجع سچ;
 zero_باہرلا:
      vala = vvp_vector4_t(wid, BIT4_0);
      أرجع سچ;
}

/*
 * %mod/wr
 */
منطقي دا_MOD_WR(vthread_t thr, vvp_کوڈ_t)
{
      مزدوج r = thr->نکالو_real();
      مزدوج l = thr->نکالو_real();
      thr->ڈالو_real(fmod(l,r));

      أرجع سچ;
}

/*
 * %neg/wr
 */
منطقي دا_NEG_WR(vthread_t thr, vvp_کوڈ_t)
{
      thr->poke_real(0, -thr->peek_real(0));
      أرجع سچ;
}

/*
 * %pad/s <wid>
 */
منطقي دا_PAD_S(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی wid = cp->نمبر;

      vvp_vector4_t&val = thr->peek_vec4();
      بنانشانی old_ناپ = val.size();

	// Sign-extend.
      إذا (old_ناپ < wid)
	    val.resize(wid, val.value(old_ناپ-1));
      وإلا
	    val.resize(wid);

      أرجع سچ;
}

/*
 * %pad/u <wid>
 */
منطقي دا_PAD_U(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی wid = cp->نمبر;

      vvp_vector4_t&val = thr->peek_vec4();
      val.resize(wid, BIT4_0);

      أرجع سچ;
}

/*
 * %part/s <wid>
 * %part/u <wid>
 * Two values are popped from the stack. First, pop the canonical
 * index of the part select, and second is the value to be
 * selected. The result is pushed back to the stack.
 */
ساكن منطقي دا_PART_ادھار(vthread_t thr, vvp_کوڈ_t cp, منطقي signed_flag)
{
      بنانشانی wid = cp->نمبر;

      vvp_vector4_t base4 = thr->نکالو_vec4();
      vvp_vector4_t&value = thr->peek_vec4();

      vvp_vector4_t res (wid, BIT4_X);

	// NOTE: This is treating the vector as signed. Is that correct?
      int32_t ادھار;
      منطقي قدر_ok = vector4_منزل_قدر(base4, ادھار, signed_flag);
      إذا (! قدر_ok) {
	    value = res;
	    أرجع سچ;
      }

      إذا (ادھار >= (int32_t)value.size()) {
	    value = res;
	    أرجع سچ;
      }

      إذا ((ادھار+(صحيح)wid) <= 0) {
	    value = res;
	    أرجع سچ;
      }

      لما vbase = 0;
      إذا (ادھار < 0) {
	    vbase = -ادھار;
	    wid -= vbase;
	    ادھار = 0;
      }

      إذا ((ادھار+wid) > value.size()) {
	    wid = value.size() - ادھار;
      }

      res .سیٹ_vec(vbase, value.subvalue(ادھار, wid));
      value = res;

      أرجع سچ;
}

منطقي دا_PART_S(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع دا_PART_ادھار(thr, cp, سچ);
}

منطقي دا_PART_U(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع دا_PART_ادھار(thr, cp, خطأ);
}

/*
 * %parti/s <wid>, <basei>, <base_wid>
 * %parti/u <wid>, <basei>, <base_wid>
 *
 * Pop the value to be selected. The result is pushed back to the stack.
 */
ساكن منطقي دا_PARTI_ادھار(vthread_t thr, vvp_کوڈ_t cp, منطقي signed_flag)
{
      بنانشانی wid = cp->نمبر;
      uint32_t ادھار = cp->ثنائی_ہندسہ_idx[0];
      uint32_t bwid = cp->ثنائی_ہندسہ_idx[1];

      vvp_vector4_t&value = thr->peek_vec4();

      vvp_vector4_t res (wid, BIT4_X);

	// NOTE: This is treating the vector as signed. Is that correct?
      int32_t use_ادھار = ادھار;
      إذا (signed_flag && bwid < 32 && (ادھار&(1<<(bwid-1)))) {
	    use_ادھار |= -1UL << bwid;
      }

      إذا (use_ادھار >= (int32_t)value.size()) {
	    value = res;
	    أرجع سچ;
      }

      إذا ((use_ادھار+(int32_t)wid) <= 0) {
	    value = res;
	    أرجع سچ;
      }

      لما vbase = 0;
      إذا (use_ادھار < 0) {
	    vbase = -use_ادھار;
	    wid -= vbase;
	    use_ادھار = 0;
      }

      إذا ((use_ادھار+wid) > value.size()) {
	    wid = value.size() - use_ادھار;
      }

      res .سیٹ_vec(vbase, value.subvalue(use_ادھار, wid));
      value = res;

      أرجع سچ;
}

منطقي دا_PARTI_S(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع دا_PARTI_ادھار(thr, cp, سچ);
}

منطقي دا_PARTI_U(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع دا_PARTI_ادھار(thr, cp, خطأ);
}

/*
 * %mul
 */
منطقي دا_MUL(vthread_t thr, vvp_کوڈ_t)
{
      vvp_vector4_t r = thr->نکالو_vec4();
	// Rather then pop l, use it directly from the stack. When we
	// assign to 'l', that will edit the top of the stack, which
	// replaces a pop and a pull.
      vvp_vector4_t&l = thr->peek_vec4();

      l.mul(r);
      أرجع سچ;
}

/*
 * %muli <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
منطقي دا_MULI(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی wid = cp->نمبر;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_فوری_rval (cp, r);

      l.mul(r);
      أرجع سچ;
}

منطقي دا_MUL_WR(vthread_t thr, vvp_کوڈ_t)
{
      مزدوج r = thr->نکالو_real();
      مزدوج l = thr->نکالو_real();
      thr->ڈالو_real(l * r);

      أرجع سچ;
}

منطقي دا_NAND(vthread_t thr, vvp_کوڈ_t)
{
      vvp_vector4_t valr = thr->نکالو_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      أكد(vall.size() == valr.size());

      vall &= valr;
      vall.invert();

      أرجع سچ;
}

/*
 * %new/cobj <vpi_object>
 * This creates a new cobject (SystemVerilog class object) and pushes
 * it to the stack. The <vpi-object> is a __vpiHandle that is a
 * vpiClassDefn object that defines the item to be created.
 */
منطقي دا_NEW_COBJ(vthread_t thr, vvp_کوڈ_t cp)
{
      ثابت class_type*defn = dynamic_cast<ثابت class_type*> (cp->handle);
      أكد(defn);

      vvp_object_t tmp (جديد vvp_cobject(defn));
      thr->ڈالو_object(tmp);
      أرجع سچ;
}

منطقي دا_NEW_DARRAY(vthread_t thr, vvp_کوڈ_t cp)
{
      ثابت حرف*متن = cp->متن;
      size_t size = thr->الفاظ[cp->ثنائی_ہندسہ_idx[0]].w_int;
      بنانشانی لفظ_wid;
      size_t n;

      vvp_object_t obj;
      إذا (strcmp(متن,"b8") == 0) {
	    obj = جديد vvp_darray_atom<uint8_t>(size);
      } وإلا إذا (strcmp(متن,"b16") == 0) {
	    obj = جديد vvp_darray_atom<uint16_t>(size);
      } وإلا إذا (strcmp(متن,"b32") == 0) {
	    obj = جديد vvp_darray_atom<uint32_t>(size);
      } وإلا إذا (strcmp(متن,"b64") == 0) {
	    obj = جديد vvp_darray_atom<uint64_t>(size);
      } وإلا إذا (strcmp(متن,"sb8") == 0) {
	    obj = جديد vvp_darray_atom<int8_t>(size);
      } وإلا إذا (strcmp(متن,"sb16") == 0) {
	    obj = جديد vvp_darray_atom<int16_t>(size);
      } وإلا إذا (strcmp(متن,"sb32") == 0) {
	    obj = جديد vvp_darray_atom<int32_t>(size);
      } وإلا إذا (strcmp(متن,"sb64") == 0) {
	    obj = جديد vvp_darray_atom<int64_t>(size);
      } وإلا إذا ((1 == لڑی_توں_پڑھو(متن, "b%u%zn", &لفظ_wid, &n)) &&
                 (n == لڑی_لمبائی(متن))) {
	    obj = جديد vvp_darray_vec2(size, لفظ_wid);
      } وإلا إذا ((1 == لڑی_توں_پڑھو(متن, "sb%u%zn", &لفظ_wid, &n)) &&
                 (n == لڑی_لمبائی(متن))) {
	    obj = جديد vvp_darray_vec2(size, لفظ_wid);
      } وإلا إذا ((1 == لڑی_توں_پڑھو(متن, "v%u%zn", &لفظ_wid, &n)) &&
                 (n == لڑی_لمبائی(متن))) {
	    obj = جديد vvp_darray_vec4(size, لفظ_wid);
      } وإلا إذا ((1 == لڑی_توں_پڑھو(متن, "sv%u%zn", &لفظ_wid, &n)) &&
                 (n == لڑی_لمبائی(متن))) {
	    obj = جديد vvp_darray_vec4(size, لفظ_wid);
      } وإلا إذا (strcmp(متن,"r") == 0) {
	    obj = جديد vvp_darray_real(size);
      } وإلا إذا (strcmp(متن,"S") == 0) {
	    obj = جديد vvp_darray_string(size);
      } وإلا {
	    cerr << get_fileline()
	         << "Internal error: Unsupported dynamic array type: "
	         << متن << "." << سطر_انت;
	    أكد(0);
      }

      thr->ڈالو_object(obj);

      أرجع سچ;
}

منطقي دا_NOOP(vthread_t, vvp_کوڈ_t)
{
      أرجع سچ;
}

/*
 * %nor/r
 */
منطقي دا_NORR(vthread_t thr, vvp_کوڈ_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_or());

      أرجع سچ;
}

/*
 * Push a null to the object stack.
 */
منطقي دا_NULL(vthread_t thr, vvp_کوڈ_t)
{
      vvp_object_t tmp;
      thr->ڈالو_object(tmp);
      أرجع سچ;
}

/*
 * %and/r
 */
منطقي دا_ANDR(vthread_t thr, vvp_کوڈ_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_and());

      أرجع سچ;
}

/*
 * %nand/r
 */
منطقي دا_NANDR(vthread_t thr, vvp_کوڈ_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_and());

      أرجع سچ;
}

/*
 * %or/r
 */
منطقي دا_ORR(vthread_t thr, vvp_کوڈ_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_or());

      أرجع سچ;
}

/*
 * %xor/r
 */
منطقي دا_XORR(vthread_t thr, vvp_کوڈ_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_xor());

      أرجع سچ;
}

/*
 * %xnor/r
 */
منطقي دا_XNORR(vthread_t thr, vvp_کوڈ_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_xor());

      أرجع سچ;
}

/*
 * %or
 */
منطقي دا_OR(vthread_t thr, vvp_کوڈ_t)
{
      vvp_vector4_t valb = thr->نکالو_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();
      vala |= valb;
      أرجع سچ;
}

/*
 * %nor
 */
منطقي دا_NOR(vthread_t thr, vvp_کوڈ_t)
{
      vvp_vector4_t valr = thr->نکالو_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      أكد(vall.size() == valr.size());

      vall |= valr;
      vall.invert();

      أرجع سچ;
}

/*
 * %pop/obj <num>, <skip>
 */
منطقي دا_نکالو_OBJ(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی cnt = cp->ثنائی_ہندسہ_idx[0];
      بنانشانی skip = cp->ثنائی_ہندسہ_idx[1];

      thr->نکالو_object(cnt, skip);
      أرجع سچ;
}

/*
 * %pop/real <number>
 */
منطقي دا_نکالو_REAL(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی cnt = cp->نمبر;
      thr->نکالو_real(cnt);
      أرجع سچ;
}

/*
 *  %pop/str <number>
 */
منطقي دا_نکالو_STR(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی cnt = cp->نمبر;
      thr->نکالو_str(cnt);
      أرجع سچ;
}

/*
 *  %pop/vec4 <number>
 */
منطقي دا_نکالو_VEC4(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی cnt = cp->نمبر;
      thr->نکالو_vec4(cnt);
      أرجع سچ;
}

/*
 * %pow
 * %pow/s
 */
ساكن منطقي دا_POW_ادھار(vthread_t thr, منطقي signed_flag)
{
      vvp_vector4_t valb = thr->نکالو_vec4();
      vvp_vector4_t vala = thr->نکالو_vec4();

      بنانشانی wid = vala.size();

      vvp_vector2_t xv2 = vvp_vector2_t(vala, سچ);
      vvp_vector2_t yv2 = vvp_vector2_t(valb, سچ);


        /* If we have an X or Z in the arguments return X. */
      إذا (xv2.is_NaN() || yv2.is_NaN()) {
	    vvp_vector4_t tmp (wid, BIT4_X);
	    thr->ڈالو_vec4(tmp);
	    أرجع سچ;
      }

	// Is the exponent negative? If so, table 5-6 in IEEE1364-2005
	// defines what value is returned.
      إذا (signed_flag && yv2.value(yv2.size()-1)) {
	    صحيح a_val;
	    vvp_bit4_t pad = BIT4_0, lsb = BIT4_0;
	    إذا (vector2_منزل_قدر(xv2, a_val, سچ)) {
		  إذا (a_val == 0) {
			pad = BIT4_X; lsb = BIT4_X;
		  }
		  إذا (a_val == 1) {
			pad = BIT4_0; lsb = BIT4_1;
		  }
		  إذا (a_val == -1) {
			إذا (yv2.value(0)) {
			      pad = BIT4_1; lsb = BIT4_1;
			} وإلا {
			      pad = BIT4_0; lsb = BIT4_1;
			}
		  }
	    }
	    vvp_vector4_t tmp (wid, pad);
	    tmp.سیٹ_ثنائی_ہندسہ(0, lsb);
	    thr->ڈالو_vec4(tmp);
	    أرجع سچ;
      }

      vvp_vector2_t نتیجہ = قوت(xv2, yv2);

        /* Copy only what we need of the result. If the result is too
	   small, zero-pad it. */
      لكل (بنانشانی jdx = 0;  jdx < wid;  jdx += 1) {
	    إذا (jdx >= نتیجہ.size())
		  vala.سیٹ_ثنائی_ہندسہ(jdx, BIT4_0);
	    وإلا
		  vala.سیٹ_ثنائی_ہندسہ(jdx, نتیجہ.value(jdx) ? BIT4_1 : BIT4_0);
      }
      thr->ڈالو_vec4(vala);

      أرجع سچ;
}

منطقي دا_POW(vthread_t thr, vvp_کوڈ_t)
{
      أرجع دا_POW_ادھار(thr, خطأ);
}

منطقي دا_POW_S(vthread_t thr, vvp_کوڈ_t)
{
      أرجع دا_POW_ادھار(thr, سچ);
}

منطقي دا_POW_WR(vthread_t thr, vvp_کوڈ_t)
{
      مزدوج r = thr->نکالو_real();
      مزدوج l = thr->نکالو_real();
      thr->ڈالو_real(قوت(l,r));

      أرجع سچ;
}

/*
 * %prop/obj <pid>, <idx>
 *
 * Load an object value from the cobject and push it onto the object stack.
 */
منطقي دا_PROP_OBJ(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی pid = cp->نمبر;
      بنانشانی idx = cp->ثنائی_ہندسہ_idx[0];

      إذا (idx != 0) {
	    أكد(idx < vthread_s::الفاظ_گنتی);
	    idx = thr->الفاظ[idx].w_uint;
      }

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();

      vvp_object_t val;
      cobj->get_object(pid, val, idx);

      thr->ڈالو_object(val);

      أرجع سچ;
}

ساكن فراغ get_from_obj(بنانشانی pid, vvp_cobject*cobj, مزدوج&val)
{
      val = cobj->get_real(pid);
}

ساكن فراغ get_from_obj(بنانشانی pid, vvp_cobject*cobj, string&val)
{
      val = cobj->get_string(pid);
}

ساكن فراغ get_from_obj(بنانشانی pid, vvp_cobject*cobj, vvp_vector4_t&val)
{
      cobj->get_vec4(pid, val);
}

سانچا <قسم_ناں ELEM>
ساكن منطقي prop(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی pid = cp->نمبر;

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      أكد(cobj);

      ELEM val;
      get_from_obj(pid, cobj, val);
      vthread_ڈالو(thr, val);

      أرجع سچ;
}

/*
 * %prop/r <pid>
 *
 * Load a real value from the cobject and push it onto the real value
 * stack.
 */
منطقي دا_PROP_R(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع prop<مزدوج>(thr, cp);
}

/*
 * %prop/str <pid>
 *
 * Load a string value from the cobject and push it onto the real value
 * stack.
 */
منطقي دا_PROP_STR(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع prop<string>(thr, cp);
}

/*
 * %prop/v <pid>
 *
 * Load a property <pid> from the cobject on the top of the stack into
 * the vector space at <base>.
 */
منطقي دا_PROP_V(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع prop<vvp_vector4_t>(thr, cp);
}

منطقي دا_PUSHI_REAL(vthread_t thr, vvp_کوڈ_t cp)
{
      مزدوج mant = cp->ثنائی_ہندسہ_idx[0];
      uint32_t imant = cp->ثنائی_ہندسہ_idx[0];
      صحيح قوت_نما = cp->ثنائی_ہندسہ_idx[1];

	// Detect +infinity
      إذا (قوت_نما==0x3fff && imant==0) {
	    thr->ڈالو_real(INFINITY);
	    أرجع سچ;
      }
	// Detect -infinity
      إذا (قوت_نما==0x7fff && imant==0) {
	    thr->ڈالو_real(-INFINITY);
	    أرجع سچ;
      }
	// Detect NaN
      إذا (قوت_نما==0x3fff) {
	    thr->ڈالو_real(nan(""));
	    أرجع سچ;
      }

      مزدوج sign = (قوت_نما & 0x4000)? -1.0 : 1.0;

      قوت_نما &= 0x1fff;

      mant = sign * ldexp(mant, قوت_نما - 0x1000);
      thr->ڈالو_real(mant);
      أرجع سچ;
}

منطقي دا_PUSHI_STR(vthread_t thr, vvp_کوڈ_t cp)
{
      ثابت حرف*متن = cp->متن;
      thr->ڈالو_str(filter_string(متن));
      أرجع سچ;
}

/*
 * %pushi/vec4 <vala>, <valb>, <wid>
 */
منطقي دا_PUSHI_VEC4(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی wid  = cp->نمبر;

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t val (wid, BIT4_0);
      get_فوری_rval (cp, val);

      thr->ڈالو_vec4(val);

      أرجع سچ;
}

/*
 * %pushv/str
 *   Pops a vec4 value, and pushes a string.
 */
منطقي دا_PUSHV_STR(vthread_t thr, vvp_کوڈ_t)
{
      vvp_vector4_t vec = thr->نکالو_vec4();

      size_t slen = (vec.size() + 7)/8;
      vector<حرف>buf;
      buf.reserve(slen);

      لكل (size_t idx = 0 ; idx < vec.size() ; idx += 8) {
	    حرف tmp = 0;
	    size_t trans = 8;
	    إذا (idx+trans > vec.size())
		  trans = vec.size() - idx;

	    لكل (size_t bdx = 0 ; bdx < trans ; bdx += 1) {
		  إذا (vec.value(idx+bdx) == BIT4_1)
			tmp |= 1 << bdx;
	    }

	    إذا (tmp != 0)
		  buf.push_back(tmp);
      }

      string val;
      لكل (vector<حرف>::reverse_iterator cur = buf.rbegin()
		 ; cur != buf.rend() ; ++cur) {
	    val.push_back(*cur);
      }

      thr->ڈالو_str(val);

      أرجع سچ;
}

/*
 * %putc/str/vec4 <var>, <mux>
 */
منطقي دا_PUTC_STR_VEC4(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی muxr = cp->ثنائی_ہندسہ_idx[0];
      int32_t mux = muxr? thr->الفاظ[muxr].w_int : 0;

      vvp_vector4_t val = thr->نکالو_vec4();
      أكد(val.size() == 8);

      إذا (mux < 0)
	    أرجع سچ;

	/* Get the existing value of the string. If we find that the
	   index is too big for the string, then give up. */
      vvp_net_t*net = cp->net;
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (net->fun);
      أكد(fun);

      string tmp = fun->get_string();
      إذا (tmp.size() <= (size_t)mux)
	    أرجع سچ;

      حرف val_str = 0;
      لكل (size_t idx = 0 ; idx < 8 ; idx += 1) {
	    إذا (val.value(idx)==BIT4_1)
		  val_str |= 1<<idx;
      }

	// It is a quirk of the Verilog standard that putc(..., 'h00)
	// has no effect. Test for that case here.
      إذا (val_str == 0)
	    أرجع سچ;

      tmp[mux] = val_str;

      vvp_send_string(vvp_net_ptr_t(cp->net, 0), tmp, thr->wt_context);
      أرجع سچ;
}

سانچا <قسم_ناں ELEM, صنف QTYPE>
ساكن منطقي qinsert(vthread_t thr, vvp_کوڈ_t cp, بنانشانی wid=0)
{
      int64_t idx = thr->الفاظ[3].w_int;
      ELEM value;
      vvp_net_t*net = cp->net;
      نکالو_قدر(thr, value, wid); // Pop the value to store.

      vvp_قطار*قطار = get_قطار_object<QTYPE>(thr, net);
      أكد(قطار);
      إذا (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at a negative "
	         << get_قطار_type(value)
	         << " index (" << idx << "). ";
	    print_قطار_قدر(value);
	    cerr << " was not added." << سطر_انت;
      } وإلا إذا (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at an undefined "
	         << get_قطار_type(value) << " index. ";
	    print_قطار_قدر(value);
	    cerr << " was not added." << سطر_انت;
      } وإلا {
	    بنانشانی max_ناپ = thr->الفاظ[cp->ثنائی_ہندسہ_idx[0]].w_int;
	    قطار->insert(idx, value, max_ناپ);
      }
      أرجع سچ;
}

/*
 * %qinsert/real <var-label>
 */
منطقي دا_QINSERT_REAL(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع qinsert<مزدوج, vvp_قطار_real>(thr, cp);
}

/*
 * %qinsert/str <var-label>
 */
منطقي دا_QINSERT_STR(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع qinsert<string, vvp_قطار_string>(thr, cp);
}

/*
 * %qinsert/v <var-label>
 */
منطقي دا_QINSERT_V(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع qinsert<vvp_vector4_t, vvp_قطار_vec4>(thr, cp, cp->ثنائی_ہندسہ_idx[1]);
}

/*
 * Helper functions used in the queue pop templates
 */
مضمن فراغ ڈالو_قدر(vthread_t thr, مزدوج value, بنانشانی)
{
      thr->ڈالو_real(value);
}

مضمن فراغ ڈالو_قدر(vthread_t thr, ثابت string&value, بنانشانی)
{
      thr->ڈالو_str(value);
}

مضمن فراغ ڈالو_قدر(vthread_t thr, ثابت vvp_vector4_t&value, بنانشانی wid)
{
      أكد(wid == value.size());
      thr->ڈالو_vec4(value);
}

سانچا <قسم_ناں ELEM, صنف QTYPE>
ساكن منطقي q_نکالو(vthread_t thr, vvp_کوڈ_t cp,
                  فراغ (*get_val_func)(vvp_قطار*, ELEM&),
                  ثابت حرف*loc, بنانشانی wid)
{
      vvp_net_t*net = cp->net;

      vvp_قطار*قطار = get_قطار_object<QTYPE>(thr, net);
      أكد(قطار);

      size_t size = قطار->get_ناپ();

      ELEM value;
      إذا (size) {
	    get_val_func(قطار, value);
      } وإلا {
	    dq_default(value, wid);
	    cerr << thr->get_fileline()
	         << "Warning: pop_" << loc << "() on empty "
	         << get_قطار_type(value) << "." << سطر_انت;
      }

      ڈالو_قدر(thr, value, wid);
      أرجع سچ;
}

سانچا <قسم_ناں ELEM>
ساكن فراغ get_پٹھ_قدر(vvp_قطار*قطار, ELEM&value)
{
      قطار->get_لفظ(قطار->get_ناپ()-1, value);
      قطار->pop_back();
}

سانچا <قسم_ناں ELEM, صنف QTYPE>
ساكن منطقي qpop_b(vthread_t thr, vvp_کوڈ_t cp, بنانشانی wid=0)
{
      أرجع q_نکالو<ELEM, QTYPE>(thr, cp, get_پٹھ_قدر<ELEM>, "back", wid);
}

/*
 * %qpop/b/real <var-label>
 */
منطقي دا_QPOP_B_REAL(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع qpop_b<مزدوج, vvp_قطار_real>(thr, cp);
}

/*
 * %qpop/b/str <var-label>
 */
منطقي دا_QPOP_B_STR(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع qpop_b<string, vvp_قطار_string>(thr, cp);
}

/*
 * %qpop/b/v <var-label>
 */
منطقي دا_QPOP_B_V(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع qpop_b<vvp_vector4_t, vvp_قطار_vec4>(thr, cp, cp->ثنائی_ہندسہ_idx[0]);
}

سانچا <قسم_ناں ELEM>
ساكن فراغ get_front_قدر(vvp_قطار*قطار, ELEM&value)
{
      قطار->get_لفظ(0, value);
      قطار->pop_front();
}

سانچا <قسم_ناں ELEM, صنف QTYPE>
ساكن منطقي qpop_f(vthread_t thr, vvp_کوڈ_t cp, بنانشانی wid=0)
{
      أرجع q_نکالو<ELEM, QTYPE>(thr, cp, get_front_قدر<ELEM>, "front", wid);
}


/*
 * %qpop/f/real <var-label>
 */
منطقي دا_QPOP_F_REAL(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع qpop_f<مزدوج, vvp_قطار_real>(thr, cp);
}

/*
 * %qpop/f/str <var-label>
 */
منطقي دا_QPOP_F_STR(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع qpop_f<string, vvp_قطار_string>(thr, cp);
}

/*
 * %qpop/f/v <var-label>
 */
منطقي دا_QPOP_F_V(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع qpop_f<vvp_vector4_t, vvp_قطار_vec4>(thr, cp, cp->ثنائی_ہندسہ_idx[0]);
}

/*
 * These implement the %release/net and %release/reg instructions. The
 * %release/net instruction applies to a net kind of functor by
 * sending the release/net command to the command port. (See vvp_net.h
 * for details.) The %release/reg instruction is the same, but sends
 * the release/reg command instead. These are very similar to the
 * %deassign instruction.
 */
ساكن منطقي do_release_vec(vvp_کوڈ_t cp, منطقي net_flag)
{
      vvp_net_t*net = cp->net;
      بنانشانی ادھار  = cp->ثنائی_ہندسہ_idx[0];
      بنانشانی width = cp->ثنائی_ہندسہ_idx[1];

      أكد(net->fil);

      إذا (ادھار >= net->fil->filter_ناپ()) أرجع سچ;
      إذا (ادھار+width > net->fil->filter_ناپ())
	    width = net->fil->filter_ناپ() - ادھار;

      منطقي بھریا_sig = ادھار == 0 && width == net->fil->filter_ناپ();

	// XXXX Can't really do this if this is a partial release?
      net->fil->force_unlink();

	/* Do we release all or part of the net? */
      vvp_net_ptr_t ptr (net, 0);
      إذا (بھریا_sig) {
	    net->fil->release(ptr, net_flag);
      } وإلا {
	    net->fil->release_pv(ptr, ادھار, width, net_flag);
      }
      net->fun->force_flag(خطأ);

      أرجع سچ;
}

منطقي دا_RELEASE_NET(vthread_t, vvp_کوڈ_t cp)
{
      أرجع do_release_vec(cp, سچ);
}


منطقي دا_RELEASE_REG(vthread_t, vvp_کوڈ_t cp)
{
      أرجع do_release_vec(cp, خطأ);
}

/* The type is 1 for registers and 0 for everything else. */
منطقي دا_RELEASE_WR(vthread_t, vvp_کوڈ_t cp)
{
      vvp_net_t*net = cp->net;
      بنانشانی type  = cp->ثنائی_ہندسہ_idx[0];

      أكد(net->fil);
      net->fil->force_unlink();

	// Send a command to this signal to unforce itself.
      vvp_net_ptr_t ptr (net, 0);
      net->fil->release(ptr, type==0);
      أرجع سچ;
}

منطقي دا_REPLICATE(vthread_t thr, vvp_کوڈ_t cp)
{
      صحيح rept = cp->نمبر;
      vvp_vector4_t val = thr->نکالو_vec4();
      vvp_vector4_t res (val.size() * rept, BIT4_X);

      لكل (صحيح idx = 0 ; idx < rept ; idx += 1) {
	    res.سیٹ_vec(idx * val.size(), val);
      }

      thr->ڈالو_vec4(res);

      أرجع سچ;
}

ساكن فراغ poke_val(vthread_t fun_thr, بنانشانی ڈونگھائی, مزدوج val)
{
      fun_thr->ماپا->poke_real(ڈونگھائی, val);
}

ساكن فراغ poke_val(vthread_t fun_thr, بنانشانی ڈونگھائی, ثابت string&val)
{
      fun_thr->ماپا->poke_str(ڈونگھائی, val);
}

ساكن size_t get_max(vthread_t fun_thr, مزدوج&)
{
      أرجع fun_thr->دلیلاں_real.size();
}

ساكن size_t get_max(vthread_t fun_thr, string&)
{
      أرجع fun_thr->دلیلاں_str.size();
}

ساكن size_t get_max(vthread_t fun_thr, vvp_vector4_t&)
{
      أرجع fun_thr->دلیلاں_vec4.size();
}

ساكن بنانشانی get_ڈونگھائی(vthread_t fun_thr, size_t اشاریہ, مزدوج&)
{
      أرجع fun_thr->دلیلاں_real[اشاریہ];
}

ساكن بنانشانی get_ڈونگھائی(vthread_t fun_thr, size_t اشاریہ, string&)
{
      أرجع fun_thr->دلیلاں_str[اشاریہ];
}

ساكن بنانشانی get_ڈونگھائی(vthread_t fun_thr, size_t اشاریہ, vvp_vector4_t&)
{
      أرجع fun_thr->دلیلاں_vec4[اشاریہ];
}

ساكن vthread_t get_func(vthread_t thr)
{
      vthread_t fun_thr = thr;

      طالما (fun_thr->ماپا_scope->get_type_کوڈ() != vpiFunction) {
	    أكد(fun_thr->ماپا);
	    fun_thr = fun_thr->ماپا;
      }

      أرجع fun_thr;
}

سانچا <قسم_ناں ELEM>
ساكن منطقي ret(vthread_t thr, vvp_کوڈ_t cp)
{
      size_t اشاریہ = cp->نمبر;
      ELEM val;
      نکالو_قدر(thr, val, 0);

      vthread_t fun_thr = get_func(thr);
      أكد(اشاریہ < get_max(fun_thr, val));

      بنانشانی ڈونگھائی = get_ڈونگھائی(fun_thr, اشاریہ, val);
	// Use the depth to put the value into the stack of
	// the parent thread.
      poke_val(fun_thr, ڈونگھائی, val);
      أرجع سچ;
}

/*
 * %ret/real <index>
 */
منطقي دا_RET_REAL(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع ret<مزدوج>(thr, cp);
}

/*
 * %ret/str <index>
 */
منطقي دا_RET_STR(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع ret<string>(thr, cp);
}

/*
 * %ret/vec4 <index>, <offset>, <wid>
 */
منطقي دا_RET_VEC4(vthread_t thr, vvp_کوڈ_t cp)
{
      size_t اشاریہ = cp->نمبر;
      بنانشانی off_اشاریہ = cp->ثنائی_ہندسہ_idx[0];
      بنانشانی صحيح wid = cp->ثنائی_ہندسہ_idx[1];
      vvp_vector4_t&val = thr->peek_vec4();

      vthread_t fun_thr = get_func(thr);
      أكد(اشاریہ < get_max(fun_thr, val));
      أكد(val.size() == wid);
      بنانشانی ڈونگھائی = get_ڈونگھائی(fun_thr, اشاریہ, val);

      int64_t off = off_اشاریہ ? thr->الفاظ[off_اشاریہ].w_int : 0;
      بنانشانی صحيح sig_قدر_ناپ = fun_thr->ماپا->peek_vec4(ڈونگھائی).size();

      إذا (off_اشاریہ!=0 && thr->flags[4] == BIT4_1) {
	    thr->نکالو_vec4(1);
	    أرجع سچ;
      }

      إذا (!resize_rval_vec(val, off, sig_قدر_ناپ)) {
	    thr->نکالو_vec4(1);
	    أرجع سچ;
      }

      إذا (off == 0 && val.size() == sig_قدر_ناپ) {
	    fun_thr->ماپا->poke_vec4(ڈونگھائی, val);
      } وإلا {
	    vvp_vector4_t tmp_dst = fun_thr->ماپا->peek_vec4(ڈونگھائی);
	    tmp_dst.سیٹ_vec(off, val);
	    fun_thr->ماپا->poke_vec4(ڈونگھائی, tmp_dst);
      }

      thr->نکالو_vec4(1);
      أرجع سچ;
}

ساكن فراغ ڈالو_from_ماپا(vthread_t thr, vthread_t fun_thr, بنانشانی ڈونگھائی, مزدوج&)
{
      thr->ڈالو_real(fun_thr->ماپا->peek_real(ڈونگھائی));
}

ساكن فراغ ڈالو_from_ماپا(vthread_t thr, vthread_t fun_thr, بنانشانی ڈونگھائی, string&)
{
      thr->ڈالو_str(fun_thr->ماپا->peek_str(ڈونگھائی));
}

ساكن فراغ ڈالو_from_ماپا(vthread_t thr, vthread_t fun_thr, بنانشانی ڈونگھائی, vvp_vector4_t&)
{
      thr->ڈالو_vec4(fun_thr->ماپا->peek_vec4(ڈونگھائی));
}

سانچا <قسم_ناں ELEM>
ساكن منطقي retload(vthread_t thr, vvp_کوڈ_t cp)
{
      size_t اشاریہ = cp->نمبر;
      ELEM type;

      vthread_t fun_thr = get_func(thr);
      أكد(اشاریہ < get_max(fun_thr, type));

      بنانشانی ڈونگھائی = get_ڈونگھائی(fun_thr, اشاریہ, type);
	// Use the depth to extract the values from the stack
	// of the parent thread.
      ڈالو_from_ماپا(thr, fun_thr, ڈونگھائی, type);
      أرجع سچ;
}

/*
 * %retload/real <index>
 */
منطقي دا_RETLOAD_REAL(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع retload<مزدوج>(thr, cp);
}

/*
 * %retload/str <index>
 */
منطقي دا_RETLOAD_STR(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع retload<string>(thr, cp);
}

/*
 * %retload/vec4 <index>
 */
منطقي دا_RETLOAD_VEC4(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع retload<vvp_vector4_t>(thr, cp);
}

/*
 * %scopy
 *
 * Pop the top item from the object stack, and shallow_copy() that item into
 * the new top of the object stack. This will copy at many items as needed
 * from the source object to fill the target object. If the target object is
 * larger then the source object, then some items will be left unchanged.
 *
 * The object may be any kind of object that supports shallow_copy(),
 * including dynamic arrays and class objects.
 */
منطقي دا_SCOPY(vthread_t thr, vvp_کوڈ_t)
{
      vvp_object_t tmp;
      thr->نکالو_object(tmp);

      vvp_object_t&dest = thr->peek_object();
        // If it is null there is nothing to copy
      إذا (!tmp.test_nil())
	    dest.shallow_copy(tmp);

      أرجع سچ;
}

ساكن فراغ thread_peek(vthread_t thr, مزدوج&value)
{
      value = thr->peek_real(0);
}

ساكن فراغ thread_peek(vthread_t thr, string&value)
{
      value = thr->peek_str(0);
}

ساكن فراغ thread_peek(vthread_t thr, vvp_vector4_t&value)
{
      value = thr->peek_vec4(0);
}

سانچا <قسم_ناں ELEM>
ساكن منطقي سیٹ_dar_obj(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی adr = thr->الفاظ[cp->نمبر].w_int;

      ELEM value;
      thread_peek(thr, value);

      vvp_object_t&چوٹی = thr->peek_object();
      vvp_darray*darray = چوٹی.peek<vvp_darray>();
      أكد(darray);

      darray->سیٹ_لفظ(adr, value);
      أرجع سچ;
}

/*
 * %set/dar/obj/real <index>
 */
منطقي دا_سیٹ_DAR_OBJ_REAL(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع سیٹ_dar_obj<مزدوج>(thr, cp);
}

/*
 * %set/dar/obj/str <index>
 */
منطقي دا_سیٹ_DAR_OBJ_STR(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع سیٹ_dar_obj<string>(thr, cp);
}

/*
 * %set/dar/obj/vec4 <index>
 */
منطقي دا_سیٹ_DAR_OBJ_VEC4(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع سیٹ_dar_obj<vvp_vector4_t>(thr, cp);
}

/*
 * %shiftl <idx>
 *
 * Pop the operand, then push the result.
 */
منطقي دا_SHIFTL(vthread_t thr, vvp_کوڈ_t cp)
{
      صحيح use_اشاریہ = cp->نمبر;
      uint64_t کھسکاؤ = thr->الفاظ[use_اشاریہ].w_uint;

      vvp_vector4_t&val = thr->peek_vec4();
      بنانشانی wid  = val.size();

      إذا (thr->flags[4] == BIT4_1) {
	      // The result is 'bx if the shift amount is undefined
	    val = vvp_vector4_t(wid, BIT4_X);

      } وإلا إذا (thr->flags[4] == BIT4_X || کھسکاؤ >= wid) {
	      // Shift is so big that all value is shifted out. Write
	      // a constant 0 result.
	    val = vvp_vector4_t(wid, BIT4_0);

      } وإلا إذا (کھسکاؤ > 0) {
	    vvp_vector4_t blk = val.subvalue(0, wid-کھسکاؤ);
	    vvp_vector4_t tmp (کھسکاؤ, BIT4_0);
	    val.سیٹ_vec(0, tmp);
	    val.سیٹ_vec(کھسکاؤ, blk);
      }

      أرجع سچ;
}

/*
 * %shiftr <idx>
 * This is an unsigned right shift. The <idx> is a number that selects
 * the index register with the amount of the shift. This instruction
 * checks flag bit 4, which will be true if the shift is invalid.
 */
منطقي دا_SHIFTR(vthread_t thr, vvp_کوڈ_t cp)
{
      صحيح use_اشاریہ = cp->نمبر;
      uint64_t کھسکاؤ = thr->الفاظ[use_اشاریہ].w_uint;

      vvp_vector4_t val = thr->نکالو_vec4();
      بنانشانی wid  = val.size();

      إذا (thr->flags[4] == BIT4_1) {
	    val = vvp_vector4_t(wid, BIT4_X);

      } وإلا إذا (thr->flags[4] == BIT4_X || کھسکاؤ > wid) {
	    val = vvp_vector4_t(wid, BIT4_0);

      } وإلا إذا (کھسکاؤ > 0) {
	    vvp_vector4_t blk = val.subvalue(کھسکاؤ, wid-کھسکاؤ);
	    vvp_vector4_t tmp (کھسکاؤ, BIT4_0);
	    val.سیٹ_vec(0, blk);
	    val.سیٹ_vec(wid-کھسکاؤ, tmp);
      }

      thr->ڈالو_vec4(val);
      أرجع سچ;
}

/*
 *  %shiftr/s <wid>
 */
منطقي دا_SHIFTR_S(vthread_t thr, vvp_کوڈ_t cp)
{
      صحيح use_اشاریہ = cp->نمبر;
      uint64_t کھسکاؤ = thr->الفاظ[use_اشاریہ].w_uint;

      vvp_vector4_t val = thr->نکالو_vec4();
      بنانشانی wid  = val.size();

      vvp_bit4_t sign_ثنائی_ہندسہ = val.value(val.size()-1);

      إذا (thr->flags[4] == BIT4_1) {
	    val = vvp_vector4_t(wid, BIT4_X);

      } وإلا إذا (thr->flags[4] == BIT4_X || کھسکاؤ > wid) {
	    val = vvp_vector4_t(wid, sign_ثنائی_ہندسہ);

      } وإلا إذا (کھسکاؤ > 0) {
	    vvp_vector4_t blk = val.subvalue(کھسکاؤ, wid-کھسکاؤ);
	    vvp_vector4_t tmp (کھسکاؤ, sign_ثنائی_ہندسہ);
	    val.سیٹ_vec(0, blk);
	    val.سیٹ_vec(wid-کھسکاؤ, tmp);
      }

      thr->ڈالو_vec4(val);
      أرجع سچ;
}

/*
 * %split/vec4 <wid>
 *   Pop 1 value,
 *   Take <wid> bits from the lsb,
 *   Push the remaining msb,
 *   Push the lsb.
 */
منطقي دا_ونڈو_VEC4(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی lsb_wid = cp->نمبر;

      vvp_vector4_t&val = thr->peek_vec4();
      أكد(lsb_wid < val.size());

      vvp_vector4_t lsb = val.subvalue(0, lsb_wid);
      val = val.subvalue(lsb_wid, val.size()-lsb_wid);

      thr->ڈالو_vec4(lsb);
      أرجع سچ;
}

/*
 * The following are used to allow the darray templates to print correctly.
 */
مضمن ساكن string get_darray_type(مزدوج&)
{
      أرجع "darray<real>";
}

مضمن ساكن string get_darray_type(string&)
{
      أرجع "darray<string>";
}

مضمن ساكن string get_darray_type(ثابت vvp_vector4_t&value)
{
      ostringstream buf;
      buf << "darray<vector[" << value.size() << "]>";
      string res = buf.str();
      أرجع res;
}

/*
 * The following are used to allow a common template to be written for
 * darray real/string/vec4 operations
 */
مضمن ساكن فراغ dar_نکالو_قدر(vthread_t thr, مزدوج&value)
{
      value = thr->نکالو_real();
}

مضمن ساكن فراغ dar_نکالو_قدر(vthread_t thr, string&value)
{
      value = thr->نکالو_str();
}

مضمن ساكن فراغ dar_نکالو_قدر(vthread_t thr, vvp_vector4_t&value)
{
      value = thr->نکالو_vec4();
}

سانچا <قسم_ناں ELEM>
ساكن منطقي store_dar(vthread_t thr, vvp_کوڈ_t cp)
{
      int64_t adr = thr->الفاظ[3].w_int;
      ELEM value;
	// FIXME: Can we get the size of the underlying array element
	//        and then use the normal pop_value?
      dar_نکالو_قدر(thr, value);

      vvp_net_t*net = cp->net;
      أكد(net);

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      أكد(obj);

      vvp_darray*darray = obj->get_object().peek<vvp_darray>();

      إذا (adr < 0)
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to a negative " << get_darray_type(value)
	         << " index (" << adr << ")." << سطر_انت;
      وإلا إذا (thr->flags[4] != BIT4_0)
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to an undefined " << get_darray_type(value)
	         << " index." << سطر_انت;
      وإلا إذا (darray)
	    darray->سیٹ_لفظ(adr, value);
      وإلا
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to an undefined " << get_darray_type(value)
	         << "." << سطر_انت;

      أرجع سچ;
}

/*
 * %store/dar/real <var>
 */
منطقي دا_STORE_DAR_R(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع store_dar<مزدوج>(thr, cp);
}

/*
 * %store/dar/str <var>
 */
منطقي دا_STORE_DAR_STR(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع store_dar<string>(thr, cp);
}

/*
 * %store/dar/vec4 <var>
 */
منطقي دا_STORE_DAR_VEC4(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع store_dar<vvp_vector4_t>(thr, cp);
}

منطقي دا_STORE_OBJ(vthread_t thr, vvp_کوڈ_t cp)
{
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);

      vvp_object_t val;
      thr->نکالو_object(val);

      vvp_send_object(ptr, val, thr->wt_context);

      أرجع سچ;
}

/*
 * %store/obja <array-label> <index>
 */
منطقي دا_STORE_OBJA(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی idx = cp->ثنائی_ہندسہ_idx[0];
      بنانشانی adr = thr->الفاظ[idx].w_int;

      vvp_object_t val;
      thr->نکالو_object(val);

      cp->array->سیٹ_لفظ(adr, val);

      أرجع سچ;
}


/*
 * %store/prop/obj <pid>, <idx>
 *
 * Pop an object value from the object stack, and store the value into
 * the property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
منطقي دا_STORE_PROP_OBJ(vthread_t thr, vvp_کوڈ_t cp)
{
      size_t pid = cp->نمبر;
      بنانشانی idx = cp->ثنائی_ہندسہ_idx[0];

      إذا (idx != 0) {
	    أكد(idx < vthread_s::الفاظ_گنتی);
	    idx = thr->الفاظ[idx].w_uint;
      }

      vvp_object_t val;
      thr->نکالو_object(val);

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      أكد(cobj);

      cobj->سیٹ_object(pid, val, idx);

      أرجع سچ;
}

ساكن فراغ نکالو_prop_val(vthread_t thr, مزدوج&val, بنانشانی)
{
      val = thr->نکالو_real();
}

ساكن فراغ نکالو_prop_val(vthread_t thr, string&val, بنانشانی)
{
      val = thr->نکالو_str();
}

ساكن فراغ نکالو_prop_val(vthread_t thr, vvp_vector4_t&val, بنانشانی wid)
{
      val = thr->نکالو_vec4();
      أكد(val.size() >= wid);
      val.resize(wid);
}

ساكن فراغ سیٹ_val(vvp_cobject*cobj, size_t pid, ثابت مزدوج&val)
{
      cobj->سیٹ_real(pid, val);
}

ساكن فراغ سیٹ_val(vvp_cobject*cobj, size_t pid, ثابت string&val)
{
      cobj->سیٹ_string(pid, val);
}

ساكن فراغ سیٹ_val(vvp_cobject*cobj, size_t pid, ثابت vvp_vector4_t&val)
{
      cobj->سیٹ_vec4(pid, val);
}

سانچا <قسم_ناں ELEM>
ساكن منطقي store_prop(vthread_t thr, vvp_کوڈ_t cp, بنانشانی wid=0)
{
      size_t pid = cp->نمبر;
      ELEM val;
      نکالو_prop_val(thr, val, wid); // Pop the value to store.

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      أكد(cobj);

      سیٹ_val(cobj, pid, val);

      أرجع سچ;
}

/*
 * %store/prop/r <id>
 *
 * Pop a real value from the real stack, and store the value into the
 * property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
منطقي دا_STORE_PROP_R(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع store_prop<مزدوج>(thr, cp);
}

/*
 * %store/prop/str <id>
 *
 * Pop a string value from the string stack, and store the value into
 * the property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
منطقي دا_STORE_PROP_STR(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع store_prop<string>(thr, cp);
}

/*
 * %store/prop/v <pid>, <wid>
 *
 * Store vector value into property <id> of cobject in the top of the
 * stack. Do NOT pop the object stack.
 */
منطقي دا_STORE_PROP_V(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع store_prop<vvp_vector4_t>(thr, cp, cp->ثنائی_ہندسہ_idx[0]);
}

سانچا <صنف QTYPE>
ساكن QTYPE* get_قطار_prop(vvp_cobject*cobj, size_t pid)
{
      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      QTYPE* قطار = qobj.peek<QTYPE>();
      إذا (قطار == 0) {
	    قطار = جديد QTYPE;
	    vvp_object_t val(قطار);
	    cobj->سیٹ_object(pid, val, 0);
      }
      أرجع قطار;
}

سانچا <قسم_ناں ELEM, صنف QTYPE>
ساكن منطقي store_prop_qb(vthread_t thr, vvp_کوڈ_t cp, بنانشانی wid)
{
      size_t pid = cp->نمبر;
      ELEM value;
      بنانشانی max_ناپ = thr->الفاظ[cp->ثنائی_ہندسہ_idx[0]].w_uint;
      نکالو_قدر(thr, value, wid);

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      QTYPE* قطار = get_قطار_prop<QTYPE>(cobj, pid);
      أكد(قطار);
      قطار->push_back(value, max_ناپ);
      أرجع سچ;
}

سانچا <قسم_ناں ELEM, صنف QTYPE>
ساكن منطقي store_prop_qf(vthread_t thr, vvp_کوڈ_t cp, بنانشانی wid)
{
      size_t pid = cp->نمبر;
      ELEM value;
      بنانشانی max_ناپ = thr->الفاظ[cp->ثنائی_ہندسہ_idx[0]].w_uint;
      نکالو_قدر(thr, value, wid);

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      QTYPE* قطار = get_قطار_prop<QTYPE>(cobj, pid);
      أكد(قطار);
      قطار->push_front(value, max_ناپ);
      أرجع سچ;
}

منطقي دا_STORE_PROP_QB_R(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع store_prop_qb<مزدوج, vvp_قطار_real>(thr, cp, 0);
}

منطقي دا_STORE_PROP_QB_STR(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع store_prop_qb<string, vvp_قطار_string>(thr, cp, 0);
}

منطقي دا_STORE_PROP_QB_V(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع store_prop_qb<vvp_vector4_t, vvp_قطار_vec4>(thr, cp, cp->ثنائی_ہندسہ_idx[1]);
}

منطقي دا_STORE_PROP_QF_R(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع store_prop_qf<مزدوج, vvp_قطار_real>(thr, cp, 0);
}

منطقي دا_STORE_PROP_QF_STR(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع store_prop_qf<string, vvp_قطار_string>(thr, cp, 0);
}

منطقي دا_STORE_PROP_QF_V(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع store_prop_qf<vvp_vector4_t, vvp_قطار_vec4>(thr, cp, cp->ثنائی_ہندسہ_idx[1]);
}

سانچا <قسم_ناں ELEM, صنف QTYPE>
ساكن منطقي qinsert_prop(vthread_t thr, vvp_کوڈ_t cp, بنانشانی wid)
{
      int64_t idx = thr->الفاظ[3].w_int;
      ELEM value;
      size_t pid = cp->نمبر;
      نکالو_قدر(thr, value, wid);

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      QTYPE* قطار = get_قطار_prop<QTYPE>(cobj, pid);
      أكد(قطار);
      إذا (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at a negative "
	         << get_قطار_type(value)
	         << " index (" << idx << "). ";
	    print_قطار_قدر(value);
	    cerr << " was not added." << سطر_انت;
      } وإلا إذا (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at an undefined "
	         << get_قطار_type(value) << " index. ";
	    print_قطار_قدر(value);
	    cerr << " was not added." << سطر_انت;
      } وإلا {
	    بنانشانی max_ناپ = thr->الفاظ[cp->ثنائی_ہندسہ_idx[0]].w_int;
	    قطار->insert(idx, value, max_ناپ);
      }
      أرجع سچ;
}

منطقي دا_QINSERT_PROP_R(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع qinsert_prop<مزدوج, vvp_قطار_real>(thr, cp, 0);
}

منطقي دا_QINSERT_PROP_STR(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع qinsert_prop<string, vvp_قطار_string>(thr, cp, 0);
}

منطقي دا_QINSERT_PROP_V(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع qinsert_prop<vvp_vector4_t, vvp_قطار_vec4>(thr, cp, cp->ثنائی_ہندسہ_idx[1]);
}

سانچا <قسم_ناں ELEM, صنف QTYPE>
ساكن منطقي q_نکالو_prop(vthread_t thr, vvp_کوڈ_t cp,
                       فراغ (*get_val_func)(vvp_قطار*, ELEM&),
                       ثابت حرف*loc, بنانشانی wid)
{
      size_t pid = cp->نمبر;

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      QTYPE* قطار = get_قطار_prop<QTYPE>(cobj, pid);
      أكد(قطار);

      size_t size = قطار->get_ناپ();

      ELEM value;
      إذا (size) {
	    get_val_func(قطار, value);
      } وإلا {
	    dq_default(value, wid);
	    cerr << thr->get_fileline()
	         << "Warning: pop_" << loc << "() on empty "
	         << get_قطار_type(value) << "." << سطر_انت;
      }

      ڈالو_قدر(thr, value, wid);
      أرجع سچ;
}

سانچا <قسم_ناں ELEM, صنف QTYPE>
ساكن منطقي qpop_b_prop(vthread_t thr, vvp_کوڈ_t cp, بنانشانی wid)
{
      أرجع q_نکالو_prop<ELEM, QTYPE>(thr, cp, get_پٹھ_قدر<ELEM>, "back", wid);
}

سانچا <قسم_ناں ELEM, صنف QTYPE>
ساكن منطقي qpop_f_prop(vthread_t thr, vvp_کوڈ_t cp, بنانشانی wid)
{
      أرجع q_نکالو_prop<ELEM, QTYPE>(thr, cp, get_front_قدر<ELEM>, "front", wid);
}

منطقي دا_QPOP_PROP_B_REAL(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع qpop_b_prop<مزدوج, vvp_قطار_real>(thr, cp, 0);
}

منطقي دا_QPOP_PROP_B_STR(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع qpop_b_prop<string, vvp_قطار_string>(thr, cp, 0);
}

منطقي دا_QPOP_PROP_B_V(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع qpop_b_prop<vvp_vector4_t, vvp_قطار_vec4>(thr, cp, cp->ثنائی_ہندسہ_idx[0]);
}

منطقي دا_QPOP_PROP_F_REAL(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع qpop_f_prop<مزدوج, vvp_قطار_real>(thr, cp, 0);
}

منطقي دا_QPOP_PROP_F_STR(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع qpop_f_prop<string, vvp_قطار_string>(thr, cp, 0);
}

منطقي دا_QPOP_PROP_F_V(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع qpop_f_prop<vvp_vector4_t, vvp_قطار_vec4>(thr, cp, cp->ثنائی_ہندسہ_idx[0]);
}

ساكن vvp_vector4_t قطار_unique_ulong_منزل_vec4(بنانشانی لما ul, بنانشانی بٹ)
{
      vvp_vector4_t val(بٹ, BIT4_0);
      لكل (بنانشانی idx = 0; idx < بٹ && idx < 8 * حجم(بنانشانی لما); idx += 1) {
	    إذا (ul & 1UL) {
		  val.سیٹ_ثنائی_ہندسہ(idx, BIT4_1);
	    } وإلا {
		  val.سیٹ_ثنائی_ہندسہ(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      أرجع val;
}

سانچا <صنف SRC> ساكن vvp_قطار_vec4* قطار_لگاتار_unique_src(SRC* src,
								 بنانشانی wid,
								 منطقي as_اشاریہ)
{
      vvp_قطار_vec4* dst = جديد vvp_قطار_vec4();
      إذا (!src) أرجع dst;
      size_t n = src->get_ناپ();
      لكل (size_t i = 0; i < n; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_لفظ(i, vi);
	    منطقي تکیا = خطأ;
	    لكل (size_t j = 0; j < i; j += 1) {
		  vvp_vector4_t vj(wid);
		  src->get_لفظ(j, vj);
		  إذا (vi.eeq(vj)) {
			تکیا = سچ;
			اكسر;
		  }
	    }
	    إذا (تکیا) استمر;
	    إذا (as_اشاریہ) {
		  dst->push_back(قطار_unique_ulong_منزل_vec4((بنانشانی لما)i, 32), 0);
	    } وإلا {
		  dst->push_back(vi, 0);
	    }
      }
      أرجع dst;
}

سانچا <صنف SRC>
ساكن vvp_قطار_vec4* قطار_لگاتار_لبھو_matches_src(SRC* src, بنانشانی wid,
						  ثابت vvp_vector4_t& cmp,
						  منطقي as_اشاریہ)
{
      vvp_قطار_vec4* dst = جديد vvp_قطار_vec4();
      إذا (!src) أرجع dst;
      size_t n = src->get_ناپ();
      لكل (size_t i = 0; i < n; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_لفظ(i, vi);
	    إذا (vi.eeq(cmp)) {
		  إذا (as_اشاریہ) {
			dst->push_back(قطار_unique_ulong_منزل_vec4((بنانشانی لما)i, 32), 0);
		  } وإلا {
			dst->push_back(vi, 0);
		  }
	    }
      }
      أرجع dst;
}

/* Array/queue locator methods return a queue with 0 or 1 elements (LRM). */
سانچا <صنف SRC>
ساكن vvp_قطار_vec4* قطار_لبھو_پہلا_آخری_منزل_قطار_src(SRC* src,
							  بنانشانی wid,
							  ثابت vvp_vector4_t& cmp,
							  منطقي want_پہلا,
							  منطقي want_اشاریہ)
{
      vvp_قطار_vec4* dst = جديد vvp_قطار_vec4();
      إذا (!src || src->get_ناپ() == 0) أرجع dst;
      size_t n = src->get_ناپ();
      إذا (want_پہلا) {
	    لكل (size_t i = 0; i < n; i += 1) {
		  vvp_vector4_t vi(wid);
		  src->get_لفظ(i, vi);
		  إذا (vi.eeq(cmp)) {
			إذا (want_اشاریہ) {
			      dst->push_back(
				  قطار_unique_ulong_منزل_vec4((بنانشانی لما)i, 32),
				  0);
			} وإلا {
			      dst->push_back(vi, 0);
			}
			أرجع dst;
		  }
	    }
      } وإلا {
	    لكل (size_t i = n; i > 0; ) {
		  i -= 1;
		  vvp_vector4_t vi(wid);
		  src->get_لفظ(i, vi);
		  إذا (vi.eeq(cmp)) {
			إذا (want_اشاریہ) {
			      dst->push_back(
				  قطار_unique_ulong_منزل_vec4((بنانشانی لما)i, 32),
				  0);
			} وإلا {
			      dst->push_back(vi, 0);
			}
			أرجع dst;
		  }
	    }
      }
      أرجع dst;
}

/* min()/max() return one element with the selected extremum. */
سانچا <صنف SRC>
ساكن vvp_قطار_vec4* قطار_لگاتار_min_max_src(SRC* src, بنانشانی wid, منطقي want_max)
{
      vvp_قطار_vec4* dst = جديد vvp_قطار_vec4();
      إذا (!src || src->get_ناپ() == 0) أرجع dst;

      vvp_vector4_t بہترین(wid);
      src->get_لفظ(0, بہترین);

      size_t n = src->get_ناپ();
      لكل (size_t i = 1 ; i < n ; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_لفظ(i, vi);

	    vvp_bit4_t ge = compare_gtge(vi, بہترین, BIT4_1);
	    منطقي replace = want_max ? (ge == BIT4_1) : (ge == BIT4_0);
	    إذا (replace) بہترین = vi;
      }

      dst->push_back(بہترین, 0);
      أرجع dst;
}

منطقي دا_قطار_لبھو_V(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_vector4_t cmp = thr->نکالو_vec4();
      بنانشانی wid = cp->ثنائی_ہندسہ_idx[0];
      أكد(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_قطار_vec4* dst;
      إذا (qsrc) dst = قطار_لگاتار_لبھو_matches_src(qsrc, wid, cmp, خطأ);
      وإلا إذا (dsrc) dst = قطار_لگاتار_لبھو_matches_src(dsrc, wid, cmp, خطأ);
      وإلا {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    dst = قطار_لگاتار_لبھو_matches_src(src, wid, cmp, خطأ);
      }
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي دا_قطار_لبھو_PROP_V(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_vector4_t cmp = thr->نکالو_vec4();
      size_t pid = cp->نمبر;
      بنانشانی wid = cp->ثنائی_ہندسہ_idx[0];
      أكد(cmp.size() == wid);

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_قطار_vec4* dst = qsrc ? قطار_لگاتار_لبھو_matches_src(qsrc, wid, cmp, خطأ)
				 : قطار_لگاتار_لبھو_matches_src(dsrc, wid, cmp, خطأ);
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي دا_قطار_لبھو_اشاریہ_V(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_vector4_t cmp = thr->نکالو_vec4();
      بنانشانی wid = cp->ثنائی_ہندسہ_idx[0];
      أكد(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_قطار_vec4* dst;
      إذا (qsrc) dst = قطار_لگاتار_لبھو_matches_src(qsrc, wid, cmp, سچ);
      وإلا إذا (dsrc) dst = قطار_لگاتار_لبھو_matches_src(dsrc, wid, cmp, سچ);
      وإلا {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    dst = قطار_لگاتار_لبھو_matches_src(src, wid, cmp, سچ);
      }
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي دا_قطار_لبھو_اشاریہ_PROP_V(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_vector4_t cmp = thr->نکالو_vec4();
      size_t pid = cp->نمبر;
      بنانشانی wid = cp->ثنائی_ہندسہ_idx[0];
      أكد(cmp.size() == wid);

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_قطار_vec4* dst = qsrc ? قطار_لگاتار_لبھو_matches_src(qsrc, wid, cmp, سچ)
				 : قطار_لگاتار_لبھو_matches_src(dsrc, wid, cmp, سچ);
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي دا_قطار_لبھو_پہلا_V(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_vector4_t cmp = thr->نکالو_vec4();
      بنانشانی wid = cp->ثنائی_ہندسہ_idx[0];
      أكد(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_قطار_vec4* dst;
      إذا (qsrc) dst = قطار_لبھو_پہلا_آخری_منزل_قطار_src(qsrc, wid, cmp, سچ, خطأ);
      وإلا إذا (dsrc) dst = قطار_لبھو_پہلا_آخری_منزل_قطار_src(dsrc, wid, cmp, سچ, خطأ);
      وإلا {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    dst = قطار_لبھو_پہلا_آخری_منزل_قطار_src(src, wid, cmp, سچ, خطأ);
      }
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي دا_قطار_لبھو_پہلا_PROP_V(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_vector4_t cmp = thr->نکالو_vec4();
      size_t pid = cp->نمبر;
      بنانشانی wid = cp->ثنائی_ہندسہ_idx[0];
      أكد(cmp.size() == wid);

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_قطار_vec4* dst =
	    qsrc ? قطار_لبھو_پہلا_آخری_منزل_قطار_src(qsrc, wid, cmp, سچ, خطأ)
		 : قطار_لبھو_پہلا_آخری_منزل_قطار_src(dsrc, wid, cmp, سچ, خطأ);
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي دا_قطار_لبھو_پہلا_اشاریہ_V(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_vector4_t cmp = thr->نکالو_vec4();
      بنانشانی wid = cp->ثنائی_ہندسہ_idx[0];
      أكد(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_قطار_vec4* dst;
      إذا (qsrc) dst = قطار_لبھو_پہلا_آخری_منزل_قطار_src(qsrc, wid, cmp, سچ, سچ);
      وإلا إذا (dsrc) dst = قطار_لبھو_پہلا_آخری_منزل_قطار_src(dsrc, wid, cmp, سچ, سچ);
      وإلا {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    dst = قطار_لبھو_پہلا_آخری_منزل_قطار_src(src, wid, cmp, سچ, سچ);
      }
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي دا_قطار_لبھو_پہلا_اشاریہ_PROP_V(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_vector4_t cmp = thr->نکالو_vec4();
      size_t pid = cp->نمبر;
      بنانشانی wid = cp->ثنائی_ہندسہ_idx[0];
      أكد(cmp.size() == wid);

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_قطار_vec4* dst =
	    qsrc ? قطار_لبھو_پہلا_آخری_منزل_قطار_src(qsrc, wid, cmp, سچ, سچ)
		 : قطار_لبھو_پہلا_آخری_منزل_قطار_src(dsrc, wid, cmp, سچ, سچ);
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي دا_قطار_لبھو_آخری_V(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_vector4_t cmp = thr->نکالو_vec4();
      بنانشانی wid = cp->ثنائی_ہندسہ_idx[0];
      أكد(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_قطار_vec4* dst;
      إذا (qsrc) dst = قطار_لبھو_پہلا_آخری_منزل_قطار_src(qsrc, wid, cmp, خطأ, خطأ);
      وإلا إذا (dsrc) dst = قطار_لبھو_پہلا_آخری_منزل_قطار_src(dsrc, wid, cmp, خطأ, خطأ);
      وإلا {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    dst = قطار_لبھو_پہلا_آخری_منزل_قطار_src(src, wid, cmp, خطأ, خطأ);
      }
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي دا_قطار_لبھو_آخری_PROP_V(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_vector4_t cmp = thr->نکالو_vec4();
      size_t pid = cp->نمبر;
      بنانشانی wid = cp->ثنائی_ہندسہ_idx[0];
      أكد(cmp.size() == wid);

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_قطار_vec4* dst =
	    qsrc ? قطار_لبھو_پہلا_آخری_منزل_قطار_src(qsrc, wid, cmp, خطأ, خطأ)
		 : قطار_لبھو_پہلا_آخری_منزل_قطار_src(dsrc, wid, cmp, خطأ, خطأ);
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي دا_قطار_لبھو_آخری_اشاریہ_V(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_vector4_t cmp = thr->نکالو_vec4();
      بنانشانی wid = cp->ثنائی_ہندسہ_idx[0];
      أكد(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_قطار_vec4* dst;
      إذا (qsrc) dst = قطار_لبھو_پہلا_آخری_منزل_قطار_src(qsrc, wid, cmp, خطأ, سچ);
      وإلا إذا (dsrc) dst = قطار_لبھو_پہلا_آخری_منزل_قطار_src(dsrc, wid, cmp, خطأ, سچ);
      وإلا {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    dst = قطار_لبھو_پہلا_آخری_منزل_قطار_src(src, wid, cmp, خطأ, سچ);
      }
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي دا_قطار_لبھو_آخری_اشاریہ_PROP_V(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_vector4_t cmp = thr->نکالو_vec4();
      size_t pid = cp->نمبر;
      بنانشانی wid = cp->ثنائی_ہندسہ_idx[0];
      أكد(cmp.size() == wid);

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_قطار_vec4* dst =
	    qsrc ? قطار_لبھو_پہلا_آخری_منزل_قطار_src(qsrc, wid, cmp, خطأ, سچ)
		 : قطار_لبھو_پہلا_آخری_منزل_قطار_src(dsrc, wid, cmp, خطأ, سچ);
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي دا_قطار_MIN_V(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_net_t* net = cp->net;
      بنانشانی wid = cp->ثنائی_ہندسہ_idx[0];
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_قطار_vec4* dst;
      إذا (qsrc) dst = قطار_لگاتار_min_max_src(qsrc, wid, خطأ);
      وإلا إذا (dsrc) dst = قطار_لگاتار_min_max_src(dsrc, wid, خطأ);
      وإلا {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    dst = قطار_لگاتار_min_max_src(src, wid, خطأ);
      }
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي دا_قطار_MIN_OBJ_V(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی wid = cp->ثنائی_ہندسہ_idx[0];
      vvp_object_t src_obj;
      thr->نکالو_object(src_obj);

      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_قطار_vec4* dst = qsrc ? قطار_لگاتار_min_max_src(qsrc, wid, خطأ)
				 : قطار_لگاتار_min_max_src(dsrc, wid, خطأ);
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي دا_قطار_MIN_PROP_V(vthread_t thr, vvp_کوڈ_t cp)
{
      size_t pid = cp->نمبر;
      بنانشانی wid = cp->ثنائی_ہندسہ_idx[0];

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_قطار_vec4* dst = qsrc ? قطار_لگاتار_min_max_src(qsrc, wid, خطأ)
				 : قطار_لگاتار_min_max_src(dsrc, wid, خطأ);
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي دا_قطار_MAX_V(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_net_t* net = cp->net;
      بنانشانی wid = cp->ثنائی_ہندسہ_idx[0];
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_قطار_vec4* dst;
      إذا (qsrc) dst = قطار_لگاتار_min_max_src(qsrc, wid, سچ);
      وإلا إذا (dsrc) dst = قطار_لگاتار_min_max_src(dsrc, wid, سچ);
      وإلا {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    dst = قطار_لگاتار_min_max_src(src, wid, سچ);
      }
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي دا_قطار_MAX_OBJ_V(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی wid = cp->ثنائی_ہندسہ_idx[0];
      vvp_object_t src_obj;
      thr->نکالو_object(src_obj);

      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_قطار_vec4* dst = qsrc ? قطار_لگاتار_min_max_src(qsrc, wid, سچ)
				 : قطار_لگاتار_min_max_src(dsrc, wid, سچ);
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي دا_قطار_MAX_PROP_V(vthread_t thr, vvp_کوڈ_t cp)
{
      size_t pid = cp->نمبر;
      بنانشانی wid = cp->ثنائی_ہندسہ_idx[0];

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_قطار_vec4* dst = qsrc ? قطار_لگاتار_min_max_src(qsrc, wid, سچ)
				 : قطار_لگاتار_min_max_src(dsrc, wid, سچ);
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي دا_قطار_UNIQUE_OBJ_V(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی wid = cp->ثنائی_ہندسہ_idx[0];
      vvp_object_t src_obj;
      thr->نکالو_object(src_obj);

      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_قطار_vec4* dst = qsrc ? قطار_لگاتار_unique_src(qsrc, wid, خطأ)
				 : قطار_لگاتار_unique_src(dsrc, wid, خطأ);
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي دا_قطار_UNIQUE_اشاریہ_OBJ_V(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی wid = cp->ثنائی_ہندسہ_idx[0];
      vvp_object_t src_obj;
      thr->نکالو_object(src_obj);

      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_قطار_vec4* dst = qsrc ? قطار_لگاتار_unique_src(qsrc, wid, سچ)
				 : قطار_لگاتار_unique_src(dsrc, wid, سچ);
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي دا_قطار_UNIQUE_V(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_net_t*net = cp->net;
      بنانشانی wid = cp->ثنائی_ہندسہ_idx[0];
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_قطار_vec4* dst;
      إذا (qsrc) dst = قطار_لگاتار_unique_src(qsrc, wid, خطأ);
      وإلا إذا (dsrc) dst = قطار_لگاتار_unique_src(dsrc, wid, خطأ);
      وإلا {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    dst = قطار_لگاتار_unique_src(src, wid, خطأ);
      }
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي دا_قطار_UNIQUE_PROP_V(vthread_t thr, vvp_کوڈ_t cp)
{
      size_t pid = cp->نمبر;
      بنانشانی wid = cp->ثنائی_ہندسہ_idx[0];

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_قطار_vec4* dst = qsrc ? قطار_لگاتار_unique_src(qsrc, wid, خطأ)
				 : قطار_لگاتار_unique_src(dsrc, wid, خطأ);
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي دا_قطار_UNIQUE_اشاریہ_V(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_net_t*net = cp->net;
      بنانشانی wid = cp->ثنائی_ہندسہ_idx[0];
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_قطار_vec4* dst;
      إذا (qsrc) dst = قطار_لگاتار_unique_src(qsrc, wid, سچ);
      وإلا إذا (dsrc) dst = قطار_لگاتار_unique_src(dsrc, wid, سچ);
      وإلا {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    dst = قطار_لگاتار_unique_src(src, wid, سچ);
      }
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي دا_قطار_UNIQUE_اشاریہ_PROP_V(vthread_t thr, vvp_کوڈ_t cp)
{
      size_t pid = cp->نمبر;
      بنانشانی wid = cp->ثنائی_ہندسہ_idx[0];

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_قطار_vec4* dst = qsrc ? قطار_لگاتار_unique_src(qsrc, wid, سچ)
				 : قطار_لگاتار_unique_src(dsrc, wid, سچ);
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي دا_PROP_قطار_ناپ(vthread_t thr, vvp_کوڈ_t cp)
{
      size_t pid = cp->نمبر;

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(qobj, qsrc, dsrc);

      size_t sz = qsrc ? qsrc->get_ناپ() : (dsrc ? dsrc->get_ناپ() : 0);

      vvp_vector4_t val(32, BIT4_0);
      بنانشانی لما ul = sz;
      لكل (بنانشانی idx = 0; idx < 32; idx++) {
	    إذا (ul & 1UL) {
		  val.سیٹ_ثنائی_ہندسہ(idx, BIT4_1);
	    } وإلا {
		  val.سیٹ_ثنائی_ہندسہ(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      thr->ڈالو_vec4(val);
      أرجع سچ;
}

منطقي دا_قطار_ناپ_V(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      size_t sz =
	    qsrc ? qsrc->get_ناپ() : (dsrc ? dsrc->get_ناپ() : 0);

      vvp_vector4_t val(32, BIT4_0);
      بنانشانی لما ul = sz;
      لكل (بنانشانی idx = 0; idx < 32; idx++) {
	    إذا (ul & 1UL) {
		  val.سیٹ_ثنائی_ہندسہ(idx, BIT4_1);
	    } وإلا {
		  val.سیٹ_ثنائی_ہندسہ(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      thr->ڈالو_vec4(val);
      أرجع سچ;
}

منطقي دا_قطار_لفظ_V(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی wid = cp->ثنائی_ہندسہ_idx[0];
      بنانشانی ix_reg = cp->ثنائی_ہندسہ_idx[1];
      int64_t adr = thr->الفاظ[ix_reg].w_int;
      إذا (thr->flags[4] == BIT4_1) {
	    thr->ڈالو_vec4(vvp_vector4_t(wid));
	    أرجع سچ;
      }
      إذا (adr < 0) {
	    thr->ڈالو_vec4(vvp_vector4_t(wid));
	    أرجع سچ;
      }
      vvp_net_t* net = cp->net;
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t out(wid);
      إذا (qsrc) {
	    إذا ((size_t)adr >= qsrc->get_ناپ()) {
		  thr->ڈالو_vec4(out);
		  أرجع سچ;
	    }
	    qsrc->get_لفظ((بنانشانی)adr, out);
      } وإلا إذا (dsrc) {
	    إذا ((size_t)adr >= dsrc->get_ناپ()) {
		  thr->ڈالو_vec4(out);
		  أرجع سچ;
	    }
	    dsrc->get_لفظ((بنانشانی)adr, out);
      } وإلا {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    إذا (!src || (size_t)adr >= src->get_ناپ()) {
		  thr->ڈالو_vec4(out);
		  أرجع سچ;
	    }
	    src->get_لفظ((بنانشانی)adr, out);
      }
      thr->ڈالو_vec4(out);
      أرجع سچ;
}

منطقي دا_قطار_لفظ_PROP_V(vthread_t thr, vvp_کوڈ_t cp)
{
      size_t pid = cp->نمبر;
      بنانشانی wid = cp->ثنائی_ہندسہ_idx[0];
      بنانشانی ix_reg = cp->ثنائی_ہندسہ_idx[1];
      int64_t adr = thr->الفاظ[ix_reg].w_int;
      إذا (thr->flags[4] == BIT4_1) {
	    thr->ڈالو_vec4(vvp_vector4_t(wid));
	    أرجع سچ;
      }
      إذا (adr < 0) {
	    thr->ڈالو_vec4(vvp_vector4_t(wid));
	    أرجع سچ;
      }

      vvp_object_t saved_چوٹی;
      منطقي popped_چوٹی = خطأ;
      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      إذا (cobj == 0) {
	    // Array-locator with(property) keeps an accumulator queue on top.
	    // Temporarily pop it to access the class object beneath.
	    thr->نکالو_object(saved_چوٹی);
	    popped_چوٹی = سچ;
	    vvp_object_t& ہیٹھاں = thr->peek_object();
	    cobj = ہیٹھاں.peek<vvp_cobject>();
      }
      إذا (cobj == 0) {
	    إذا (popped_چوٹی) thr->ڈالو_object(saved_چوٹی);
	    thr->ڈالو_vec4(vvp_vector4_t(wid));
	    أرجع سچ;
      }

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t out(wid);
      إذا (qsrc) {
	    إذا ((size_t)adr >= qsrc->get_ناپ()) {
		  thr->ڈالو_vec4(out);
		  أرجع سچ;
	    }
	    qsrc->get_لفظ((بنانشانی)adr, out);
      } وإلا إذا (dsrc) {
	    إذا ((size_t)adr >= dsrc->get_ناپ()) {
		  thr->ڈالو_vec4(out);
		  أرجع سچ;
	    }
	    dsrc->get_لفظ((بنانشانی)adr, out);
      } وإلا {
	    thr->ڈالو_vec4(out);
	    إذا (popped_چوٹی) thr->ڈالو_object(saved_چوٹی);
	    أرجع سچ;
      }
      إذا (popped_چوٹی) thr->ڈالو_object(saved_چوٹی);
      thr->ڈالو_vec4(out);
      أرجع سچ;
}

منطقي دا_CMPIX_LTU(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی a = cp->نمبر;
      بنانشانی b = cp->ثنائی_ہندسہ_idx[0];
      إذا (thr->الفاظ[a].w_uint < thr->الفاظ[b].w_uint) {
	    thr->flags[4] = BIT4_1;
      } وإلا {
	    thr->flags[4] = BIT4_0;
      }
      أرجع سچ;
}

منطقي دا_CMPIX_SLT0(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی r = cp->نمبر;
      thr->flags[4] = (thr->الفاظ[r].w_int < 0) ? BIT4_1 : BIT4_0;
      أرجع سچ;
}

منطقي دا_ڈالو_IX_VEC4(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی ix = cp->نمبر;
      بنانشانی wid = cp->ثنائی_ہندسہ_idx[0];
      بنانشانی sgn = cp->ثنائی_ہندسہ_idx[1];
      uint32_t ubits;
      إذا (sgn) {
	    int32_t sv = (int32_t)thr->الفاظ[ix].w_int;
	    ubits = (uint32_t)sv;
      } وإلا {
	    ubits = (uint32_t)thr->الفاظ[ix].w_uint;
      }
      إذا (wid > 32) wid = 32;
      vvp_vector4_t tmp(wid, BIT4_0);
      لكل (بنانشانی idx = 0; idx < wid; idx++) {
	    إذا (ubits & 1u) {
		  tmp.سیٹ_ثنائی_ہندسہ(idx, BIT4_1);
	    } وإلا {
		  tmp.سیٹ_ثنائی_ہندسہ(idx, BIT4_0);
	    }
	    ubits >>= 1;
      }
      thr->ڈالو_vec4(tmp);
      أرجع سچ;
}

منطقي دا_قطار_NEW_خالی_تھاواں_V(vthread_t thr, vvp_کوڈ_t)
{
      thr->ڈالو_object(vvp_object_t(جديد vvp_قطار_vec4()));
      أرجع سچ;
}

منطقي دا_قطار_APPEND_لفظ_V(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی wid = cp->ثنائی_ہندسہ_idx[0];
      vvp_vector4_t elem = thr->نکالو_vec4();
      أكد(elem.size() == wid);
      vvp_object_t qobj;
      thr->نکالو_object(qobj);
      vvp_قطار_vec4* q = qobj.peek<vvp_قطار_vec4>();
      أكد(q);
      q->push_back(elem, 0);
      thr->ڈالو_object(qobj);
      أرجع سچ;
}

منطقي دا_DELETE_PROP_ELEM(vthread_t thr, vvp_کوڈ_t cp)
{
      size_t pid = cp->نمبر;

      int64_t idx_val = thr->الفاظ[3].w_int;
      إذا (thr->flags[4] == BIT4_1) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with undefined index."
	         << سطر_انت;
	    أرجع سچ;
      }
      إذا (idx_val < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with negative index."
	         << سطر_انت;
	    أرجع سچ;
      }
      size_t idx = idx_val;

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_قطار* قطار = qobj.peek<vvp_قطار>();
      إذا (قطار == 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping delete(" << idx
	         << ") on empty queue." << سطر_انت;
      } وإلا {
	    size_t size = قطار->get_ناپ();
	    إذا (idx >= size) {
		  cerr << thr->get_fileline()
		       << "Warning: skipping out of range delete(" << idx
		       << ") on queue of size " << size << "." << سطر_انت;
	    } وإلا {
		  قطار->erase(idx);
	    }
      }

      أرجع سچ;
}

منطقي دا_DELETE_PROP_OBJ(vthread_t thr, vvp_کوڈ_t cp)
{
      size_t pid = cp->نمبر;

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      cobj->سیٹ_object(pid, vvp_object_t(), 0);

      أرجع سچ;
}

سانچا <قسم_ناں ELEM, صنف QTYPE>
ساكن منطقي store_qb(vthread_t thr, vvp_کوڈ_t cp, بنانشانی wid=0)
{
      ELEM value;
      vvp_net_t*net = cp->net;
      بنانشانی max_ناپ = thr->الفاظ[cp->ثنائی_ہندسہ_idx[0]].w_int;
      نکالو_قدر(thr, value, wid); // Pop the value to store.

      vvp_قطار*قطار = get_قطار_object<QTYPE>(thr, net);
      أكد(قطار);
      قطار->push_back(value, max_ناپ);
      أرجع سچ;
}

/*
 * %store/qb/r <var-label>, <max-idx>
 */
منطقي دا_STORE_QB_R(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع store_qb<مزدوج, vvp_قطار_real>(thr, cp);
}

/*
 * %store/qb/str <var-label>, <max-idx>
 */
منطقي دا_STORE_QB_STR(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع store_qb<string, vvp_قطار_string>(thr, cp);
}

/*
 * %store/qb/v <var-label>, <max-idx>, <wid>
 */
منطقي دا_STORE_QB_V(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع store_qb<vvp_vector4_t, vvp_قطار_vec4>(thr, cp, cp->ثنائی_ہندسہ_idx[1]);
}

سانچا <قسم_ناں ELEM, صنف QTYPE>
ساكن منطقي store_qdar(vthread_t thr, vvp_کوڈ_t cp, بنانشانی wid=0)
{
      int64_t idx = thr->الفاظ[3].w_int;
      ELEM value;
      vvp_net_t*net = cp->net;
      نکالو_قدر(thr, value, wid); // Pop the value to store.

      vvp_قطار*قطار = get_قطار_object<QTYPE>(thr, net);
      أكد(قطار);
      إذا (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot assign to a negative "
	         << get_قطار_type(value)
	         << " index (" << idx << "). ";
	    print_قطار_قدر(value);
	    cerr << " was not added." << سطر_انت;
      } وإلا إذا (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot assign to an undefined "
	         << get_قطار_type(value) << " index. ";
	    print_قطار_قدر(value);
	    cerr << " was not added." << سطر_انت;
      } وإلا {
	    بنانشانی max_ناپ = thr->الفاظ[cp->ثنائی_ہندسہ_idx[0]].w_int;
	    قطار->سیٹ_لفظ_max(idx, value, max_ناپ);
      }
      أرجع سچ;
}

/*
 * %store/qdar/r <var>, idx
 */
منطقي دا_STORE_QDAR_R(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع store_qdar<مزدوج, vvp_قطار_real>(thr, cp);
}

/*
 * %store/qdar/str <var>, idx
 */
منطقي دا_STORE_QDAR_STR(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع store_qdar<string, vvp_قطار_string>(thr, cp);
}

/*
 * %store/qdar/v <var>, idx
 */
منطقي دا_STORE_QDAR_V(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع store_qdar<vvp_vector4_t, vvp_قطار_vec4>(thr, cp, cp->ثنائی_ہندسہ_idx[1]);
}

سانچا <قسم_ناں ELEM, صنف QTYPE>
ساكن منطقي store_qf(vthread_t thr, vvp_کوڈ_t cp, بنانشانی wid=0)
{
      ELEM value;
      vvp_net_t*net = cp->net;
      بنانشانی max_ناپ = thr->الفاظ[cp->ثنائی_ہندسہ_idx[0]].w_int;
      نکالو_قدر(thr, value, wid); // Pop the value to store.

      vvp_قطار*قطار = get_قطار_object<QTYPE>(thr, net);
      أكد(قطار);
      قطار->push_front(value, max_ناپ);
      أرجع سچ;
}
/*
 * %store/qf/r <var-label>, <max-idx>
 */
منطقي دا_STORE_QF_R(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع store_qf<مزدوج, vvp_قطار_real>(thr, cp);
}

/*
 * %store/qf/str <var-label>, <max-idx>
 */
منطقي دا_STORE_QF_STR(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع store_qf<string, vvp_قطار_string>(thr, cp);
}

/*
 * %store/qb/v <var-label>, <max-idx>, <wid>
 */
منطقي دا_STORE_QF_V(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع store_qf<vvp_vector4_t, vvp_قطار_vec4>(thr, cp, cp->ثنائی_ہندسہ_idx[1]);
}

سانچا <قسم_ناں ELEM, صنف QTYPE>
ساكن منطقي store_qobj(vthread_t thr, vvp_کوڈ_t cp, بنانشانی wid=0)
{
// FIXME: Can we actually use wid here?
      (فراغ)wid;
      vvp_net_t*net = cp->net;

      vvp_قطار*قطار = get_قطار_object<QTYPE>(thr, net);
      أكد(قطار);

      vvp_object_t src;
      thr->نکالو_object(src);

        // If it is null just clear the queue
      إذا (src.test_nil())
	    قطار->erase_پوچھ(0);
      وإلا {
	    بنانشانی max_ناپ = thr->الفاظ[cp->ثنائی_ہندسہ_idx[0]].w_int;
	    قطار->copy_elems(src, max_ناپ);
      }

      أرجع سچ;
}

منطقي دا_STORE_QOBJ_R(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع store_qobj<مزدوج, vvp_قطار_real>(thr, cp);
}

منطقي دا_STORE_QOBJ_STR(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع store_qobj<string, vvp_قطار_string>(thr, cp);
}

منطقي دا_STORE_QOBJ_V(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع store_qobj<vvp_vector4_t, vvp_قطار_vec4>(thr, cp, cp->ثنائی_ہندسہ_idx[1]);
}

ساكن فراغ vvp_send(vthread_t thr, vvp_net_ptr_t ptr, ثابت مزدوج&val)
{
      vvp_send_real(ptr, val, thr->wt_context);
}

ساكن فراغ vvp_send(vthread_t thr, vvp_net_ptr_t ptr, ثابت string&val)
{
      vvp_send_string(ptr, val, thr->wt_context);
}

سانچا <قسم_ناں ELEM>
ساكن منطقي store(vthread_t thr, vvp_کوڈ_t cp)
{
      ELEM val;
      نکالو_قدر(thr, val, 0);
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_send(thr, ptr, val);
      أرجع سچ;
}

منطقي دا_STORE_REAL(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع store<مزدوج>(thr, cp);
}

سانچا <قسم_ناں ELEM>
ساكن منطقي storea(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی idx = cp->ثنائی_ہندسہ_idx[0];
      ELEM val;
      نکالو_قدر(thr, val, 0);

      إذا (thr->flags[4] != BIT4_1) {
	    بنانشانی adr = thr->الفاظ[idx].w_int;
	    cp->array->سیٹ_لفظ(adr, val);
      }

      أرجع سچ;
}

/*
 * %store/reala <var-label> <index>
 */
منطقي دا_STORE_REALA(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع storea<مزدوج>(thr, cp);
}

منطقي دا_STORE_STR(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع store<string>(thr, cp);
}

/*
 * %store/stra <array-label> <index>
 */
منطقي دا_STORE_STRA(vthread_t thr, vvp_کوڈ_t cp)
{
      أرجع storea<string>(thr, cp);
}

/*
 * %store/vec4 <var-label>, <offset>, <wid>
 *
 * <offset> is the index register that contains the base offset into
 * the destination. If zero, the offset of 0 is used instead of index
 * register zero. The offset value is SIGNED, and can be negative.
 *
 * <wid> is the actual width, an unsigned number.
 *
 * This function tests flag bit 4. If that flag is set, and <offset>
 * is an actual index register (not zero) then this assumes that the
 * calculation of the <offset> contents failed, and the store is
 * aborted.
 *
 * NOTE: This instruction may loose the <wid> argument because it is
 * not consistent with the %store/vec4/<etc> instructions which have
 * no <wid>.
 */
منطقي دا_STORE_VEC4(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_net_ptr_t ptr(cp->net, 0);
      ثابت vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*> (cp->net->fil);
      بنانشانی off_اشاریہ = cp->ثنائی_ہندسہ_idx[0];
      بنانشانی صحيح wid = cp->ثنائی_ہندسہ_idx[1];

      int64_t off = off_اشاریہ ? thr->الفاظ[off_اشاریہ].w_int : 0;
      بنانشانی صحيح sig_قدر_ناپ = sig->قدر_ناپ();

      vvp_vector4_t&val = thr->peek_vec4();
      بنانشانی val_ناپ = val.size();

      إذا (val_ناپ < wid) {
	    cerr << thr->get_fileline()
	         << "XXXX Internal error: val.size()=" << val_ناپ
		 << ", expecting >= " << wid << سطر_انت;
      }
      أكد(val_ناپ >= wid);
      إذا (val_ناپ > wid) {
	    val.resize(wid);
      }

	// If there is a problem loading the index register, flags-4
	// will be set to 1, and we know here to skip the actual assignment.
      إذا (off_اشاریہ!=0 && thr->flags[4] == BIT4_1) {
	    thr->نکالو_vec4(1);
	    أرجع سچ;
      }

      إذا (!resize_rval_vec(val, off, sig_قدر_ناپ)) {
	    thr->نکالو_vec4(1);
	    أرجع سچ;
      }

      إذا (off == 0 && val.size() == sig_قدر_ناپ)
	    vvp_send_vec4(ptr, val, thr->wt_context);
      وإلا
	    vvp_send_vec4_pv(ptr, val, off, sig_قدر_ناپ, thr->wt_context);

      thr->نکالو_vec4(1);
      أرجع سچ;
}

/*
 * %store/vec4a <var-label>, <addr>, <offset>
 */
منطقي دا_STORE_VEC4_A(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی adr_اشاریہ = cp->ثنائی_ہندسہ_idx[0];
      بنانشانی off_اشاریہ = cp->ثنائی_ہندسہ_idx[1];

      لما adr = adr_اشاریہ? thr->الفاظ[adr_اشاریہ].w_int : 0;
      int64_t off = off_اشاریہ ? thr->الفاظ[off_اشاریہ].w_int : 0;

	// Suppress action if flags-4 is true.
      إذا (thr->flags[4] == BIT4_1) {
	    thr->نکالو_vec4(1);
	    أرجع سچ;
      }

      vvp_vector4_t &value = thr->peek_vec4();

      إذا (!resize_rval_vec(value, off, cp->array->get_لفظ_ناپ())) {
	    thr->نکالو_vec4(1);
	    أرجع سچ;
      }

      cp->array->سیٹ_لفظ(adr, off, value);

      thr->نکالو_vec4(1);
      أرجع سچ;
}

/*
 * %sub
 *   pop r;
 *   pop l;
 *   push l-r;
 */
منطقي دا_SUB(vthread_t thr, vvp_کوڈ_t)
{
      vvp_vector4_t r = thr->نکالو_vec4();
      vvp_vector4_t&l = thr->peek_vec4();

      l.sub(r);
      أرجع سچ;
}

/*
 * %subi <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
منطقي دا_SUBI(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی wid = cp->نمبر;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_فوری_rval (cp, r);

      l.sub(r);

      أرجع سچ;

}

منطقي دا_SUB_WR(vthread_t thr, vvp_کوڈ_t)
{
      مزدوج r = thr->نکالو_real();
      مزدوج l = thr->نکالو_real();
      thr->ڈالو_real(l - r);
      أرجع سچ;
}

/*
 * %substr <first>, <last>
 * Pop a string, take the substring (SystemVerilog style), and return
 * the result to the stack. This opcode actually works by editing the
 * string in place.
 */
منطقي دا_SUBSTR(vthread_t thr, vvp_کوڈ_t cp)
{
      int32_t پہلا = thr->الفاظ[cp->ثنائی_ہندسہ_idx[0]].w_int;
      int32_t آخری = thr->الفاظ[cp->ثنائی_ہندسہ_idx[1]].w_int;
      string&val = thr->peek_str(0);

      إذا (پہلا < 0 || آخری < پہلا || آخری >= (int32_t)val.size()) {
	    val = string("");
	    أرجع سچ;
      }

      val = val.substr(پہلا, آخری-پہلا+1);
      أرجع سچ;
}

/*
 * %substr/vec4 <index>, <wid>
 */
منطقي دا_SUBSTR_VEC4(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی sel_idx = cp->ثنائی_ہندسہ_idx[0];
      بنانشانی wid = cp->ثنائی_ہندسہ_idx[1];

      int32_t sel = thr->الفاظ[sel_idx].w_int;
      ثابت string&val = thr->peek_str(0);

      أكد(wid%8 == 0);

      إذا (sel < 0 || sel >= (int32_t)val.size()) {
	    vvp_vector4_t res (wid, BIT4_0);
	    thr->ڈالو_vec4(res);
	    أرجع سچ;
      }

      vvp_vector4_t res (wid, BIT4_0);

      أكد(wid==8);
      بنانشانی حرف tmp = val[sel];
      لكل (صحيح idx = 0 ; idx < 8 ; idx += 1) {
	    إذا (tmp & (1<<idx))
		  res.سیٹ_ثنائی_ہندسہ(idx, BIT4_1);
      }

      thr->ڈالو_vec4(res);
      أرجع سچ;
}

منطقي دا_FILE_سطر(vthread_t thr, vvp_کوڈ_t cp)
{
      vpiHandle handle = cp->handle;

	/* When it is available, keep the file/line information in the
	   thread for error/warning messages. */
      thr->سیٹ_fileline(vpi_get_str(vpiFile, handle),
                        vpi_get(vpi_سطر_No, handle));

      إذا (show_file_سطر)
	    cerr << thr->get_fileline()
	         << vpi_get_str(_vpiDescription, handle) << سطر_انت;

      أرجع سچ;
}

/*
 * %test_nul <var-label>;
 * Test if the object at the specified variable is nil. If so, write
 * "1" into flags[4], otherwise write "0" into flags[4].
 */
منطقي دا_TEST_NUL(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_net_t*net = cp->net;

      أكد(net);
      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      أكد(obj);

      إذا (obj->get_object().test_nil())
	    thr->flags[4] = BIT4_1;
      وإلا
	    thr->flags[4] = BIT4_0;

      أرجع سچ;
}

منطقي دا_TEST_NUL_A(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی idx = cp->ثنائی_ہندسہ_idx[0];
      بنانشانی adr = thr->الفاظ[idx].w_int;
      vvp_object_t لفظ;

	/* If the address is undefined, return true. */
      إذا (thr->flags[4] == BIT4_1) {
	    أرجع سچ;
      }

      cp->array->get_لفظ_obj(adr, لفظ);
      إذا (لفظ.test_nil())
	    thr->flags[4] = BIT4_1;
      وإلا
	    thr->flags[4] = BIT4_0;

      أرجع سچ;
}

منطقي دا_TEST_NUL_OBJ(vthread_t thr, vvp_کوڈ_t)
{
      إذا (thr->peek_object().test_nil())
	    thr->flags[4] = BIT4_1;
      وإلا
	    thr->flags[4] = BIT4_0;
      أرجع سچ;
}

/*
 * %test_nul/prop <pid>, <idx>
 */
منطقي دا_TEST_NUL_PROP(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی pid = cp->نمبر;
      بنانشانی idx = cp->ثنائی_ہندسہ_idx[0];

      إذا (idx != 0) {
	    أكد(idx < vthread_s::الفاظ_گنتی);
	    idx = thr->الفاظ[idx].w_uint;
      }

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj  = obj.peek<vvp_cobject>();

      vvp_object_t val;
      cobj->get_object(pid, val, idx);

      إذا (val.test_nil())
	    thr->flags[4] = BIT4_1;
      وإلا
	    thr->flags[4] = BIT4_0;

      أرجع سچ;
}

/* and() reduction returns the scalar sum (Verilog-style bitwise-AND per word). */
سانچا <صنف SRC>
ساكن vvp_vector4_t uparr_and_الفاظ_src(SRC* src, بنانشانی wid)
{
      إذا (!src) أرجع vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_1);
      لكل (size_t i = 0; i < src->get_ناپ(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_لفظ(i, vi);
	    res &= vi;
      }
      أرجع res;
}

منطقي دا_UPARR_AND_V(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_net_t* net = cp->net;
      بنانشانی wid = cp->ثنائی_ہندسہ_idx[0];
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      إذا (qsrc) res = uparr_and_الفاظ_src(qsrc, wid);
      وإلا إذا (dsrc) res = uparr_and_الفاظ_src(dsrc, wid);
      وإلا {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    res = uparr_and_الفاظ_src(src, wid);
      }
      thr->ڈالو_vec4(res);
      أرجع سچ;
}

/* or() reduction returns the scalar sum (Verilog-style bitwise-AND per word). */
سانچا <صنف SRC>
ساكن vvp_vector4_t uparr_or_الفاظ_src(SRC* src, بنانشانی wid)
{
      إذا (!src) أرجع vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_0);
      لكل (size_t i = 0; i < src->get_ناپ(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_لفظ(i, vi);
	    res |= vi;
      }
      أرجع res;
}

منطقي دا_UPARR_OR_V(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_net_t* net = cp->net;
      بنانشانی wid = cp->ثنائی_ہندسہ_idx[0];
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      إذا (qsrc) res = uparr_or_الفاظ_src(qsrc, wid);
      وإلا إذا (dsrc) res = uparr_or_الفاظ_src(dsrc, wid);
      وإلا {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    res = uparr_or_الفاظ_src(src, wid);
      }
      thr->ڈالو_vec4(res);
      أرجع سچ;
}

/* real product() reduction returns the scalar sum */
سانچا <صنف SRC>
ساكن مزدوج uparr_ضرب_حاصل_الفاظ_src_real(SRC* src)
{
      إذا (!src) أرجع 0.0;
      مزدوج acc = 1.0;
      لكل (size_t i = 0; i < src->get_ناپ(); i += 1) {
	    مزدوج vi;
	    src->get_لفظ(i, vi);
	    acc *= vi;
      }
      أرجع acc;
}

منطقي دا_UPARR_ضرب_حاصل_REAL(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_قطار_real* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_real_from_net(thr, net, qsrc, dsrc);
      مزدوج جوڑ;
      إذا (qsrc) جوڑ = uparr_ضرب_حاصل_الفاظ_src_real(qsrc);
      وإلا إذا (dsrc) جوڑ = uparr_ضرب_حاصل_الفاظ_src_real(dsrc);
      وإلا {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_real>(thr, net);
	    vvp_قطار_real* src = dynamic_cast<vvp_قطار_real*>(src_q);
	    جوڑ = uparr_ضرب_حاصل_الفاظ_src_real(src);
      }
      thr->ڈالو_real(جوڑ);
      أرجع سچ;
}

/* product() reduction returns the scalar product (Verilog-style multiplication per word). */
سانچا <صنف SRC>
ساكن vvp_vector4_t uparr_ضرب_حاصل_الفاظ_src(SRC* src, بنانشانی wid)
{
      إذا (!src) أرجع vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t acc = قطار_unique_ulong_منزل_vec4(1UL, wid);
      لكل (size_t i = 0; i < src->get_ناپ(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_لفظ(i, vi);
	    acc.mul(vi);
      }
      أرجع acc;
}

منطقي دا_UPARR_ضرب_حاصل_V(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_net_t* net = cp->net;
      بنانشانی wid = cp->ثنائی_ہندسہ_idx[0];
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t prod;
      إذا (qsrc) prod = uparr_ضرب_حاصل_الفاظ_src(qsrc, wid);
      وإلا إذا (dsrc) prod = uparr_ضرب_حاصل_الفاظ_src(dsrc, wid);
      وإلا {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    prod = uparr_ضرب_حاصل_الفاظ_src(src, wid);
      }
      thr->ڈالو_vec4(prod);
      أرجع سچ;
}

منطقي دا_UPARR_ضرب_حاصل_PROP_V(vthread_t thr, vvp_کوڈ_t cp)
{
      size_t pid = cp->نمبر;
      بنانشانی wid = cp->ثنائی_ہندسہ_idx[0];

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t prod =
	    qsrc ? uparr_ضرب_حاصل_الفاظ_src(qsrc, wid)
		 : uparr_ضرب_حاصل_الفاظ_src(dsrc, wid);
      thr->ڈالو_vec4(prod);
      أرجع سچ;
}

منطقي دا_UPARR_ضرب_حاصل_OBJ_V(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی wid = cp->ثنائی_ہندسہ_idx[0];
      vvp_object_t src_obj;
      thr->نکالو_object(src_obj);

      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_vector4_t prod =
	    qsrc ? uparr_ضرب_حاصل_الفاظ_src(qsrc, wid)
		 : uparr_ضرب_حاصل_الفاظ_src(dsrc, wid);
      thr->ڈالو_vec4(prod);
      أرجع سچ;
}

/* real sum() reduction returns the scalar sum */
سانچا <صنف SRC>
ساكن مزدوج uparr_جوڑ_الفاظ_src_real(SRC* src)
{
      مزدوج acc = 0.0;
      إذا (!src) أرجع acc;
      لكل (size_t i = 0; i < src->get_ناپ(); i += 1) {
	    مزدوج vi;
	    src->get_لفظ(i, vi);
	    acc += vi;
      }
      أرجع acc;
}

منطقي دا_UPARR_جوڑ_REAL(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_قطار_real* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_real_from_net(thr, net, qsrc, dsrc);
      مزدوج جوڑ;
      إذا (qsrc) جوڑ = uparr_جوڑ_الفاظ_src_real(qsrc);
      وإلا إذا (dsrc) جوڑ = uparr_جوڑ_الفاظ_src_real(dsrc);
      وإلا {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_real>(thr, net);
	    vvp_قطار_real* src = dynamic_cast<vvp_قطار_real*>(src_q);
	    جوڑ = uparr_جوڑ_الفاظ_src_real(src);
      }
      thr->ڈالو_real(جوڑ);
      أرجع سچ;
}

/* sum() reduction returns the scalar sum (Verilog-style addition per word). */
سانچا <صنف SRC>
ساكن vvp_vector4_t uparr_جوڑ_الفاظ_src(SRC* src, بنانشانی wid)
{
      إذا (!src) أرجع vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t acc(wid, BIT4_0);
      لكل (size_t i = 0; i < src->get_ناپ(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_لفظ(i, vi);
	    acc.جوڑو(vi);
      }
      أرجع acc;
}

منطقي دا_UPARR_جوڑ_V(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_net_t* net = cp->net;
      بنانشانی wid = cp->ثنائی_ہندسہ_idx[0];
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t جوڑ;
      إذا (qsrc) جوڑ = uparr_جوڑ_الفاظ_src(qsrc, wid);
      وإلا إذا (dsrc) جوڑ = uparr_جوڑ_الفاظ_src(dsrc, wid);
      وإلا {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    جوڑ = uparr_جوڑ_الفاظ_src(src, wid);
      }
      thr->ڈالو_vec4(جوڑ);
      أرجع سچ;
}

منطقي دا_UPARR_جوڑ_PROP_V(vthread_t thr, vvp_کوڈ_t cp)
{
      size_t pid = cp->نمبر;
      بنانشانی wid = cp->ثنائی_ہندسہ_idx[0];

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t جوڑ =
	    qsrc ? uparr_جوڑ_الفاظ_src(qsrc, wid)
		 : uparr_جوڑ_الفاظ_src(dsrc, wid);
      thr->ڈالو_vec4(جوڑ);
      أرجع سچ;
}

منطقي دا_UPARR_جوڑ_OBJ_V(vthread_t thr, vvp_کوڈ_t cp)
{
      بنانشانی wid = cp->ثنائی_ہندسہ_idx[0];
      vvp_object_t src_obj;
      thr->نکالو_object(src_obj);

      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_vector4_t جوڑ =
	    qsrc ? uparr_جوڑ_الفاظ_src(qsrc, wid)
		 : uparr_جوڑ_الفاظ_src(dsrc, wid);
      thr->ڈالو_vec4(جوڑ);
      أرجع سچ;
}

/* xor() reduction returns the scalar sum (Verilog-style bitwise-XOR per word). */
سانچا <صنف SRC>
ساكن vvp_vector4_t uparr_xor_الفاظ_src(SRC* src, بنانشانی wid)
{
      إذا (!src) أرجع vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_0);
      لكل (size_t i = 0; i < src->get_ناپ(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_لفظ(i, vi);
	    res ^= vi;
      }
      أرجع res;
}

منطقي دا_UPARR_XOR_V(vthread_t thr, vvp_کوڈ_t cp)
{
      vvp_net_t* net = cp->net;
      بنانشانی wid = cp->ثنائی_ہندسہ_idx[0];
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      إذا (qsrc) res = uparr_xor_الفاظ_src(qsrc, wid);
      وإلا إذا (dsrc) res = uparr_xor_الفاظ_src(dsrc, wid);
      وإلا {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    res = uparr_xor_الفاظ_src(src, wid);
      }
      thr->ڈالو_vec4(res);
      أرجع سچ;
}

منطقي دا_VPI_CALL(vthread_t thr, vvp_کوڈ_t cp)
{
      vpip_execute_vpi_call(thr, cp->handle);

      إذا (ویلا_سارنی_stopped()) {
	    إذا (! ویلا_سارنی_مکمل())
		  ویلا_سارنی_vthread(thr, 0, خطأ);

	    أرجع خطأ;
      }

      أرجع ویلا_سارنی_مکمل()? خطأ : سچ;
}

/* %wait <label>;
 * Implement the wait by locating the vvp_net_T for the event, and
 * adding this thread to the threads list for the event. The some
 * argument is the  reference to the functor to wait for. This must be
 * an event object of some sort.
 */
منطقي دا_WAIT(vthread_t thr, vvp_کوڈ_t cp)
{
      أكد(! thr->i_am_وچ_function);
      أكد(! thr->اڈیک_for_event);
      thr->اڈیک_for_event = 1;

	/* Add this thread to the list in the event. */
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (cp->net->fun);
      أكد(ep);
      thr->wait_اگلا = ep->جوڑو_اڈیک_thread(thr);

	/* Return false to suspend this thread. */
      أرجع خطأ;
}

/*
 * Implement the %wait/fork (SystemVerilog) instruction by suspending
 * the current thread until all the detached children have finished.
 */
منطقي دا_WAIT_FORK(vthread_t thr, vvp_کوڈ_t)
{
	/* If a %wait/fork is being executed then the parent thread
	 * cannot be waiting in a join or already waiting. */
      أكد(! thr->i_am_وچ_function);
      أكد(! thr->i_am_joining);
      أكد(! thr->i_am_اڈیک);

	/* There should be no active children when waiting. */
      أكد(thr->children.empty());

	/* If there are no detached children then there is nothing to
	 * wait for. */
      إذا (thr->detached_children.empty()) أرجع سچ;

	/* Flag that this process is waiting for the detached children
	 * to finish and suspend it. */
      thr->i_am_اڈیک = 1;
      أرجع خطأ;
}

/*
 * %xnor
 */
منطقي دا_XNOR(vthread_t thr, vvp_کوڈ_t)
{
      vvp_vector4_t valr = thr->نکالو_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      أكد(vall.size() == valr.size());

      vall ^= valr;
      vall.invert();

      أرجع سچ;
}

/*
 * %xor
 */
منطقي دا_XOR(vthread_t thr, vvp_کوڈ_t)
{
      vvp_vector4_t valr = thr->نکالو_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      أكد(vall.size() == valr.size());

      vall ^= valr;

      أرجع سچ;
}


منطقي دا_ZOMBIE(vthread_t thr, vvp_کوڈ_t)
{
      thr->pc = codespace_null();
      إذا ((thr->ماپا == 0) && (thr->children.empty())) {
	    إذا (thr->delay_delete)
		  ویلا_سارنی_del_thr(thr);
	    وإلا
		  vthread_delete(thr);
      }
      أرجع خطأ;
}

/*
 * This is a phantom opcode used to call user defined functions. It
 * is used in code generated by the .ufunc statement. It contains a
 * pointer to the executable code of the function and a pointer to
 * a ufunc_core object that has all the port information about the
 * function.
 */
ساكن منطقي do_exec_ufunc(vthread_t thr, vvp_کوڈ_t cp, vthread_t بچہ)
{
      __vpiScope*بچہ_scope = cp->ufunc_core_ptr->func_scope();
      أكد(بچہ_scope);

      أكد(بچہ_scope->get_type_کوڈ() == vpiFunction);
      أكد(thr->children.empty());


        /* We can take a number of shortcuts because we know that a
           continuous assignment can only occur in a static scope. */
      أكد(thr->wt_context == 0);
      أكد(thr->rd_context == 0);

        /* If an automatic function, allocate a context for this call. */
      vvp_context_t بچہ_context = 0;
      إذا (بچہ_scope->is_automatic()) {
            بچہ_context = vthread_alloc_context(بچہ_scope);
            thr->wt_context = بچہ_context;
            thr->rd_context = بچہ_context;
      }

      بچہ->wt_context = بچہ_context;
      بچہ->rd_context = بچہ_context;

	/* Copy all the inputs to the ufunc object to the port
	   variables of the function. This copies all the values
	   atomically. */
      cp->ufunc_core_ptr->سونپو_بٹ_منزل_ports(بچہ_context);
      بچہ->delay_delete = 1;

      بچہ->ماپا = thr;
      thr->children.insert(بچہ);
	// This should be the only child
      أكد(thr->children.size()==1);

      بچہ->is_scheduled = 1;
      بچہ->i_am_وچ_function = 1;
      vthread_لگاتار(بچہ);
      running_thread = thr;

      إذا (بچہ->i_have_ended) {
	    do_join(thr, بچہ);
            أرجع سچ;
      } وإلا {
	    thr->i_am_joining = 1;
	    أرجع خطأ;
      }
}

منطقي دا_EXEC_UFUNC_REAL(vthread_t thr, vvp_کوڈ_t cp)
{
      __vpiScope*بچہ_scope = cp->ufunc_core_ptr->func_scope();
      أكد(بچہ_scope);

	/* Create a temporary thread and run it immediately. */
      vthread_t بچہ = vthread_new(cp->cptr, بچہ_scope);
      thr->ڈالو_real(0.0);
      بچہ->دلیلاں_real.push_back(0);

      أرجع do_exec_ufunc(thr, cp, بچہ);
}

منطقي دا_EXEC_UFUNC_VEC4(vthread_t thr, vvp_کوڈ_t cp)
{
      __vpiScope*بچہ_scope = cp->ufunc_core_ptr->func_scope();
      أكد(بچہ_scope);

      vpiScopeFunction*scope_func = dynamic_cast<vpiScopeFunction*>(بچہ_scope);
      أكد(scope_func);

	/* Create a temporary thread and run it immediately. */
      vthread_t بچہ = vthread_new(cp->cptr, بچہ_scope);
      thr->ڈالو_vec4(vvp_vector4_t(scope_func->get_func_چوڑائی(), scope_func->get_func_init_val()));
      بچہ->دلیلاں_vec4.push_back(0);

      أرجع do_exec_ufunc(thr, cp, بچہ);
}

/*
 * This is a phantom opcode used to harvest the result of calling a user
 * defined function. It is used in code generated by the .ufunc statement.
 */
منطقي دا_REAP_UFUNC(vthread_t thr, vvp_کوڈ_t cp)
{
      __vpiScope*بچہ_scope = cp->ufunc_core_ptr->func_scope();
      أكد(بچہ_scope);

	/* Copy the output from the result variable to the output
	   ports of the .ufunc device. */
      cp->ufunc_core_ptr->انت_thread();

        /* If an automatic function, free the context for this call. */
      إذا (بچہ_scope->is_automatic()) {
            vthread_free_context(thr->rd_context, بچہ_scope);
            thr->wt_context = 0;
            thr->rd_context = 0;
      }

      أرجع سچ;
}
