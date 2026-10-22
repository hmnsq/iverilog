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
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#اگر_ختم
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

استعمال نامخلا std;

/* This is the size of an unsigned long in bits. This is just a
   convenience macro. */
# define CPU_لفظ_بٹ (8*حجم(غیرعلامتی طویل))
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

ساخت vthread_s {
      vthread_s();

      خالی debug_dump(ostream&fd, ثابت حرف*label_متن);

	/* This is the program counter. */
      vvp_رمز_t pc;
	/* These hold the private thread bits. */
      شمار { FLAGS_گنتی = 512, الفاظ_گنتی = 16 };
      vvp_bit4_t flags[FLAGS_گنتی];

	/* These are the word registers. */
      اتحاد {
	    int64_t  w_int;
	    uint64_t w_uint;
      } الفاظ[الفاظ_گنتی];

	// These vectors are depths within the parent thread's
	// corresponding stack.  This is how the %ret/* instructions
	// get at parent thread arguments.
      vector<غیرعلامتی> دلائل_real;
      vector<غیرعلامتی> دلائل_str;
      vector<غیرعلامتی> دلائل_vec4;

    نجی:
      vector<vvp_vector4_t>ڈھیر_vec4;
    عوامی:
      اندرونی vvp_vector4_t نکالو_vec4(خالی)
      {
	    أكد(! ڈھیر_vec4.empty());
	    vvp_vector4_t val = ڈھیر_vec4.back();
	    ڈھیر_vec4.pop_back();
	    واپس val;
      }
      اندرونی خالی ڈالو_vec4(ثابت vvp_vector4_t&val)
      {
	    ڈھیر_vec4.push_back(val);
      }
      اندرونی ثابت vvp_vector4_t& peek_vec4(غیرعلامتی گہرائی)
      {
	    غیرعلامتی size = ڈھیر_vec4.size();
	    أكد(گہرائی < size);
	    غیرعلامتی use_اشاریہ = size-1-گہرائی;
	    واپس ڈھیر_vec4[use_اشاریہ];
      }
      اندرونی vvp_vector4_t& peek_vec4(خالی)
      {
	    غیرعلامتی use_اشاریہ = ڈھیر_vec4.size();
	    أكد(use_اشاریہ >= 1);
	    واپس ڈھیر_vec4[use_اشاریہ-1];
      }
      اندرونی خالی poke_vec4(غیرعلامتی گہرائی, ثابت vvp_vector4_t&val)
      {
	    أكد(گہرائی < ڈھیر_vec4.size());
	    غیرعلامتی use_اشاریہ = ڈھیر_vec4.size()-1-گہرائی;
	    ڈھیر_vec4[use_اشاریہ] = val;
      }
      اندرونی خالی نکالو_vec4(غیرعلامتی cnt)
      {
	    جب (cnt > 0) {
		  ڈھیر_vec4.pop_back();
		  cnt -= 1;
	    }
      }


    نجی:
      vector<دوہرا> ڈھیر_real;
    عوامی:
      اندرونی دوہرا نکالو_real(خالی)
      {
	    أكد(! ڈھیر_real.empty());
	    دوہرا val = ڈھیر_real.back();
	    ڈھیر_real.pop_back();
	    واپس val;
      }
      اندرونی خالی ڈالو_real(دوہرا val)
      {
	    ڈھیر_real.push_back(val);
      }
      اندرونی دوہرا peek_real(غیرعلامتی گہرائی)
      {
	    أكد(گہرائی < ڈھیر_real.size());
	    غیرعلامتی use_اشاریہ = ڈھیر_real.size()-1-گہرائی;
	    واپس ڈھیر_real[use_اشاریہ];
      }
      اندرونی خالی poke_real(غیرعلامتی گہرائی, دوہرا val)
      {
	    أكد(گہرائی < ڈھیر_real.size());
	    غیرعلامتی use_اشاریہ = ڈھیر_real.size()-1-گہرائی;
	    ڈھیر_real[use_اشاریہ] = val;
      }
      اندرونی خالی نکالو_real(غیرعلامتی cnt)
      {
	    جب (cnt > 0) {
		  ڈھیر_real.pop_back();
		  cnt -= 1;
	    }
      }

	/* Strings are operated on using a forth-like operator
	   set. Items at the top of the stack (back()) are the objects
	   operated on except for special cases. New objects are
	   pushed onto the top (back()) and pulled from the top
	   (back()) only. */
    نجی:
      vector<string> ڈھیر_str;
    عوامی:
      اندرونی string نکالو_str(خالی)
      {
	    أكد(! ڈھیر_str.empty());
	    string val = ڈھیر_str.back();
	    ڈھیر_str.pop_back();
	    واپس val;
      }
      اندرونی خالی ڈالو_str(ثابت string&val)
      {
	    ڈھیر_str.push_back(val);
      }
      اندرونی string&peek_str(غیرعلامتی گہرائی)
      {
	    أكد(گہرائی<ڈھیر_str.size());
	    غیرعلامتی use_اشاریہ = ڈھیر_str.size()-1-گہرائی;
	    واپس ڈھیر_str[use_اشاریہ];
      }
      اندرونی خالی poke_str(غیرعلامتی گہرائی, ثابت string&val)
      {
	    أكد(گہرائی < ڈھیر_str.size());
	    غیرعلامتی use_اشاریہ = ڈھیر_str.size()-1-گہرائی;
	    ڈھیر_str[use_اشاریہ] = val;
      }
      اندرونی خالی نکالو_str(غیرعلامتی cnt)
      {
	    جب (cnt > 0) {
		  ڈھیر_str.pop_back();
		  cnt -= 1;
	    }
      }

	/* Objects are also operated on in a stack. */
    نجی:
      شمار { ڈھیر_OBJ_MAX_سائز = 32 };
      vvp_object_t ڈھیر_obj[ڈھیر_OBJ_MAX_سائز];
      غیرعلامتی ڈھیر_obj_سائز;
    عوامی:
      اندرونی vvp_object_t& peek_object(خالی)
      {
	    أكد(ڈھیر_obj_سائز > 0);
	    واپس ڈھیر_obj[ڈھیر_obj_سائز-1];
      }
      اندرونی خالی نکالو_object(vvp_object_t&obj)
      {
	    أكد(ڈھیر_obj_سائز > 0);
	    ڈھیر_obj_سائز -= 1;
	    obj = ڈھیر_obj[ڈھیر_obj_سائز];
	    ڈھیر_obj[ڈھیر_obj_سائز].reset(0);
      }
      اندرونی خالی نکالو_object(غیرعلامتی cnt, غیرعلامتی skip =0)
      {
	    أكد((cnt+skip) <= ڈھیر_obj_سائز);
	    برائے (size_t idx = ڈھیر_obj_سائز-skip-cnt ; idx < ڈھیر_obj_سائز-skip ; idx += 1)
		  ڈھیر_obj[idx].reset(0);
	    ڈھیر_obj_سائز -= cnt;
	    برائے (size_t idx = ڈھیر_obj_سائز-skip ; idx < ڈھیر_obj_سائز ; idx += 1)
		  ڈھیر_obj[idx] = ڈھیر_obj[idx+skip];
	    برائے (size_t idx = ڈھیر_obj_سائز ; idx < ڈھیر_obj_سائز+skip ; idx += 1)
		  ڈھیر_obj[idx].reset(0);
      }
      اندرونی خالی ڈالو_object(ثابت vvp_object_t&obj)
      {
	    أكد(ڈھیر_obj_سائز < ڈھیر_OBJ_MAX_سائز);
	    ڈھیر_obj[ڈھیر_obj_سائز] = obj;
	    ڈھیر_obj_سائز += 1;
      }

	/* My parent sets this when it wants me to wake it up. */
      غیرعلامتی i_am_joining      :1;
      غیرعلامتی i_am_detached     :1;
      غیرعلامتی i_am_انتظار      :1;
      غیرعلامتی i_am_اندر_function  :1; // True if running function code
      غیرعلامتی i_have_ended      :1;
      غیرعلامتی i_was_disabled    :1;
      غیرعلامتی انتظار_for_event :1;
      غیرعلامتی is_scheduled      :1;
      غیرعلامتی delay_delete      :1;
	/* This points to the children of the thread. */
      سیٹ<ساخت vthread_s*>children;
	/* This points to the detached children of the thread. */
      سیٹ<ساخت vthread_s*>detached_children;
	/* This points to my parent, if I have one. */
      ساخت vthread_s*والد;
	/* This points to the containing scope. */
      __vpiScope*والد_scope;
	/* This is used for keeping wait queues. */
      ساخت vthread_s*wait_اگلا;
	/* These are used to access automatically allocated items. */
      vvp_context_t wt_context, rd_context;
	/* These are used to pass non-blocking event control information. */
      vvp_net_t*event;
      uint64_t ecount;
	/* Save the file/line information when available. */
    نجی:
      حرف *filenm_;
      غیرعلامتی lineno_;
    عوامی:
      خالی سیٹ_fileline(حرف *filenm, غیرعلامتی lineno);
      string get_fileline();

      اندرونی خالی cleanup()
      {
	    اگر (i_was_disabled) {
		  ڈھیر_vec4.clear();
		  ڈھیر_real.clear();
		  ڈھیر_str.clear();
		  نکالو_object(ڈھیر_obj_سائز);
	    }
	    آزاد(filenm_);
	    filenm_ = 0;
	    أكد(ڈھیر_vec4.empty());
	    أكد(ڈھیر_real.empty());
	    أكد(ڈھیر_str.empty());
	    أكد(ڈھیر_obj_سائز == 0);
      }
};

اندرونی vthread_s::vthread_s()
{
      ڈھیر_obj_سائز = 0;
      filenm_ = 0;
      lineno_ = 0;
}

خالی vthread_s::سیٹ_fileline(حرف *filenm, غیرعلامتی lineno)
{
      أكد(filenm);
      اگر (!filenm_ || (strcmp(filenm_, filenm) != 0)) {
	    آزاد(filenm_);
	    filenm_ = strdup(filenm);
      }
      lineno_ = lineno;
}

اندرونی string vthread_s::get_fileline()
{
      ostringstream buf;
      اگر (filenm_) {
	    buf << filenm_ << ":" << lineno_ << ": ";
      }
      string res = buf.str();
      واپس res;
}

خالی vthread_s::debug_dump(ostream&fd, ثابت حرف*label)
{
      fd << "**** " << label << سطر_ختم;
      fd << "**** ThreadId: " << یہ << ", parent id: " << والد << سطر_ختم;

      fd << "**** Flags: ";
      برائے (صحیح idx = 0 ; idx < FLAGS_گنتی ; idx += 1)
	    fd << flags[idx];
      fd << سطر_ختم;
      fd << "**** vec4 stack..." << سطر_ختم;
      برائے (size_t idx = ڈھیر_vec4.size() ; idx > 0 ; idx -= 1)
	    fd << "    " << (ڈھیر_vec4.size()-idx) << ": " << ڈھیر_vec4[idx-1] << سطر_ختم;
      fd << "**** str stack (" << ڈھیر_str.size() << ")..." << سطر_ختم;
      fd << "**** obj stack (" << ڈھیر_obj_سائز << ")..." << سطر_ختم;
      fd << "**** args_vec4 array (" << دلائل_vec4.size() << ")..." << سطر_ختم;
      برائے (size_t idx = 0 ; idx < دلائل_vec4.size() ; idx += 1)
	    fd << "    " << idx << ": " << دلائل_vec4[idx] << سطر_ختم;
      fd << "**** file/line (";
      اگر (filenm_) fd << filenm_;
      ورنہ fd << "<no file name>";
      fd << ":" << lineno_ << ")" << سطر_ختم;
      fd << "**** Done ****" << سطر_ختم;
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
ساکن string filter_string(ثابت حرف*متن)
{
      vector<حرف> tmp (لڑی_لمبائی(متن)+1);
      size_t dst = 0;
      برائے (ثابت حرف*ptr = متن ; *ptr ; ptr += 1) {
	    // Not an escape? Move on.
	    اگر (*ptr != '\\') {
		  tmp[dst++] = *ptr;
		  جاری;
	    }

	    // Now we know that *ptr is pointing to a \ character and we
	    // have an octal sequence coming up. Advance the ptr and start
	    // processing octal digits.
	    ptr += 1;
	    اگر (*ptr == 0)
		  توڑو;

	    حرف byte = 0;
	    صحیح cnt = 3;
	    جب (*ptr && cnt > 0 && *ptr >= '0' && *ptr <= '7') {
		  byte *= 8;
		  byte += *ptr - '0';
		  cnt -= 1;
		  ptr += 1;
	    }

	    // null-bytes are supposed to be removed when assigning a string
	    // literal to a string.
	    اگر (byte != '\0')
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
      واپس res;
}

ساکن خالی do_join(vthread_t thr, vthread_t بچہ);

__vpiScope* vthread_scope(ساخت vthread_s*thr)
{
      واپس thr->والد_scope;
}

ساخت vthread_s*running_thread = 0;

string get_fileline()
{
      واپس running_thread->get_fileline();
}

خالی vthread_ڈالو(ساخت vthread_s*thr, دوہرا val)
{
      thr->ڈالو_real(val);
}

خالی vthread_ڈالو(ساخت vthread_s*thr, ثابت string&val)
{
      thr->ڈالو_str(val);
}

خالی vthread_ڈالو(ساخت vthread_s*thr, ثابت vvp_vector4_t&val)
{
      thr->ڈالو_vec4(val);
}

خالی vthread_نکالو_real(ساخت vthread_s*thr, غیرعلامتی گہرائی)
{
      thr->نکالو_real(گہرائی);
}

خالی vthread_نکالو_str(ساخت vthread_s*thr, غیرعلامتی گہرائی)
{
      thr->نکالو_str(گہرائی);
}

خالی vthread_نکالو_vec4(ساخت vthread_s*thr, غیرعلامتی گہرائی)
{
      thr->نکالو_vec4(گہرائی);
}

دوہرا vthread_get_real_ڈھیر(ساخت vthread_s*thr, غیرعلامتی گہرائی)
{
      واپس thr->peek_real(گہرائی);
}

ثابت string&vthread_get_str_ڈھیر(ساخت vthread_s*thr, غیرعلامتی گہرائی)
{
      واپس thr->peek_str(گہرائی);
}

ثابت vvp_vector4_t& vthread_get_vec4_ڈھیر(ساخت vthread_s*thr, غیرعلامتی گہرائی)
{
      واپس thr->peek_vec4(گہرائی);
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
سانچہ <جماعت VVP_قطار> ساکن vvp_قطار*get_قطار_object(vthread_t thr, vvp_net_t*net)
{
      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      أكد(obj);

      vvp_قطار*قطار = obj->get_object().peek<vvp_قطار>();
      اگر (قطار == 0) {
	    أكد(obj->get_object().test_nil());
	    قطار = نیا VVP_قطار;
	    vvp_object_t val (قطار);
	    vvp_net_ptr_t ptr (net, 0);
	    vvp_send_object(ptr, val, thr->wt_context);
      }

      واپس قطار;
}

/*
 * Locator and size/word opcodes use the same VVP net for a queue ($) or a
 * dynamic array ([]). Queues are vvp_queue_vec4; dynamic arrays may be
 * vvp_darray_vec4, vvp_darray_atom<>, etc. (all vvp_darray, but vvp_queue
 * also derives from vvp_darray — test queue vec4 first).
 */
ساکن خالی get_قطار_or_darray_vec4_from_net(vthread_t thr, vvp_net_t* net,
					      vvp_قطار_vec4*& qsrc,
					      vvp_darray*& dsrc)
{
      vvp_fun_signal_object* obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      أكد(obj);
      vvp_object_t چوٹی = obj->get_object();
      qsrc = چوٹی.peek<vvp_قطار_vec4>();
      اگر (qsrc) {
	    dsrc = 0;
	    واپس;
      }
      vvp_darray* dany = چوٹی.peek<vvp_darray>();
      اگر (dany && dynamic_cast<vvp_قطار*>(dany) == 0) {
	    dsrc = dany;
	    واپس;
      }
      اگر (چوٹی.test_nil()) {
	    qsrc = نیا vvp_قطار_vec4();
	    vvp_object_t val(qsrc);
	    vvp_net_ptr_t ptr(net, 0);
	    vvp_send_object(ptr, val, thr->wt_context);
	    dsrc = 0;
	    واپس;
      }
      /* e.g. real/string queue — not vec4 queue or plain darray */
      qsrc = 0;
      dsrc = 0;
}

ساکن خالی get_قطار_or_darray_real_from_net(vthread_t thr, vvp_net_t* net,
					      vvp_قطار_real*& qsrc,
					      vvp_darray*& dsrc)
{
      vvp_fun_signal_object* obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      أكد(obj);
      vvp_object_t چوٹی = obj->get_object();
      qsrc = چوٹی.peek<vvp_قطار_real>();
      اگر (qsrc) {
	    dsrc = 0;
	    واپس;
      }
      vvp_darray* dany = چوٹی.peek<vvp_darray>();
      اگر (dany && dynamic_cast<vvp_قطار*>(dany) == 0) {
	    dsrc = dany;
	    واپس;
      }
      اگر (چوٹی.test_nil()) {
	    qsrc = نیا vvp_قطار_real();
	    vvp_object_t val(qsrc);
	    vvp_net_ptr_t ptr(net, 0);
	    vvp_send_object(ptr, val, thr->wt_context);
	    dsrc = 0;
	    واپس;
      }
      /* e.g. real/string queue — not vec4 queue or plain darray */
      qsrc = 0;
      dsrc = 0;
}

ساکن خالی get_قطار_or_darray_vec4_from_object(vvp_object_t src_obj,
						 vvp_قطار_vec4*& qsrc,
						 vvp_darray*& dsrc)
{
      qsrc = src_obj.peek<vvp_قطار_vec4>();
      اگر (qsrc) {
	    dsrc = 0;
	    واپس;
      }
      vvp_darray* dany = src_obj.peek<vvp_darray>();
      اگر (dany && dynamic_cast<vvp_قطار*>(dany) == 0) {
	    dsrc = dany;
	    واپس;
      }
      qsrc = 0;
      dsrc = 0;
}

/*
 * The following are used to allow a common template to be written for
 * queue real/string/vec4 operations
 */
اندرونی ساکن خالی نکالو_قدر(vthread_t thr, دوہرا&value, غیرعلامتی)
{
      value = thr->نکالو_real();
}

اندرونی ساکن خالی نکالو_قدر(vthread_t thr, string&value, غیرعلامتی)
{
      value = thr->نکالو_str();
}

اندرونی ساکن خالی نکالو_قدر(vthread_t thr, vvp_vector4_t&value, غیرعلامتی wid)
{
      value = thr->نکالو_vec4();
      أكد(value.size() == wid);
}

/*
 * The following are used to allow the queue templates to print correctly.
 */
اندرونی ساکن string get_قطار_type(دوہرا&)
{
      واپس "queue<real>";
}

اندرونی ساکن string get_قطار_type(string&)
{
      واپس "queue<string>";
}

اندرونی ساکن string get_قطار_type(ثابت vvp_vector4_t&value)
{
      ostringstream buf;
      buf << "queue<vector[" << value.size() << "]>";
      string res = buf.str();
      واپس res;
}

اندرونی ساکن خالی print_قطار_قدر(دوہرا value)
{
      cerr << value;
}

اندرونی ساکن خالی print_قطار_قدر(ثابت string&value)
{
      cerr << "\"" << value << "\"";
}

اندرونی ساکن خالی print_قطار_قدر(ثابت vvp_vector4_t&value)
{
      cerr << value;
}

/*
 * The following are used to get a darray/queue default value.
 */
اندرونی ساکن خالی dq_default(دوہرا&value, غیرعلامتی)
{
      value = 0.0;
}

اندرونی ساکن خالی dq_default(string&value, غیرعلامتی)
{
      value = "";
}

اندرونی ساکن خالی dq_default(vvp_vector4_t&value, غیرعلامتی wid)
{
      value = vvp_vector4_t(wid);
}


سانچہ <جماعت T> T coerce_مقصد_چوڑائی(ثابت T&that, غیرعلامتی width)
{
      اگر (that.size() == width)
	    واپس that;

      أكد(that.size() > width);
      T res (width);
      برائے (غیرعلامتی idx = 0 ;  idx < width ;  idx += 1)
	    res.سیٹ_ثنائی_ہندسہ(idx, that.value(idx));

      واپس res;
}

/* Explicitly define the vvp_vector4_t version of coerce_to_width(). */
سانچہ vvp_vector4_t coerce_مقصد_چوڑائی(ثابت vvp_vector4_t&that,
                                       غیرعلامتی width);


ساکن خالی multiply_array_imm(غیرعلامتی طویل*res, ثابت غیرعلامتی طویل*val,
			       غیرعلامتی الفاظ, غیرعلامتی طویل imm)
{
      برائے (غیرعلامتی idx = 0 ; idx < الفاظ ; idx += 1)
	    res[idx] = 0;

      برائے (غیرعلامتی mul_idx = 0 ; mul_idx < الفاظ ; mul_idx += 1) {
	    غیرعلامتی طویل مجموع;
	    غیرعلامتی طویل tmp = multiply_with_حاصل(val[mul_idx], imm, مجموع);

	    غیرعلامتی طویل حاصل = 0;
	    res[mul_idx] = جوڑو_with_حاصل(res[mul_idx], tmp, حاصل);
	    برائے (غیرعلامتی جوڑو_idx = mul_idx+1 ; جوڑو_idx < الفاظ ; جوڑو_idx += 1) {
		  res[جوڑو_idx] = جوڑو_with_حاصل(res[جوڑو_idx], مجموع, حاصل);
		  مجموع = 0;
	    }
      }
}

/*
 * Allocate a context for use by a child thread. By preference, use
 * the last freed context. If none available, create a new one. Add
 * it to the list of live contexts in that scope.
 */
ساکن vvp_context_t vthread_alloc_context(__vpiScope*scope)
{
      أكد(scope->is_automatic());

      vvp_context_t context = scope->free_contexts;
      اگر (context) {
            scope->free_contexts = vvp_get_اگلا_context(context);
            برائے (غیرعلامتی idx = 0 ; idx < scope->nitem ; idx += 1) {
                  scope->شے[idx]->reset_instance(context);
            }
      } ورنہ {
            context = vvp_allocate_context(scope->nitem);
            برائے (غیرعلامتی idx = 0 ; idx < scope->nitem ; idx += 1) {
                  scope->شے[idx]->alloc_instance(context);
            }
      }

      vvp_سیٹ_اگلا_context(context, scope->زندہ_contexts);
      scope->زندہ_contexts = context;

      واپس context;
}

/*
 * Free a context previously allocated to a child thread by pushing it
 * onto the freed context stack. Remove it from the list of live contexts
 * in that scope.
 */
ساکن خالی vthread_free_context(vvp_context_t context, __vpiScope*scope)
{
      أكد(scope->is_automatic());
      أكد(context);

      اگر (context == scope->زندہ_contexts) {
            scope->زندہ_contexts = vvp_get_اگلا_context(context);
      } ورنہ {
            vvp_context_t tmp = scope->زندہ_contexts;
            جب (context != vvp_get_اگلا_context(tmp)) {
                  أكد(tmp);
                  tmp = vvp_get_اگلا_context(tmp);
            }
            vvp_سیٹ_اگلا_context(tmp, vvp_get_اگلا_context(context));
      }

      vvp_سیٹ_اگلا_context(context, scope->free_contexts);
      scope->free_contexts = context;
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
خالی contexts_delete(جماعت __vpiScope*scope)
{
      vvp_context_t context = scope->free_contexts;

      جب (context) {
	    scope->free_contexts = vvp_get_اگلا_context(context);
	    برائے (غیرعلامتی idx = 0; idx < scope->nitem; idx += 1) {
		  scope->شے[idx]->free_instance(context);
	    }
	    آزاد(context);
	    context = scope->free_contexts;
      }
      آزاد(scope->شے);
}
#اگر_ختم

/*
 * Create a new thread with the given start address.
 */
vthread_t vthread_new(vvp_رمز_t pc, __vpiScope*scope)
{
      vthread_t thr = نیا ساخت vthread_s;
      thr->pc     = pc;
	//thr->bits4  = vvp_vector4_t(32);
      thr->والد = 0;
      thr->والد_scope = scope;
      thr->wait_اگلا = 0;
      thr->wt_context = 0;
      thr->rd_context = 0;

      thr->i_am_joining  = 0;
      thr->i_am_detached = 0;
      thr->i_am_انتظار  = 0;
      thr->i_am_اندر_function = 0;
      thr->is_scheduled  = 0;
      thr->i_have_ended  = 0;
      thr->i_was_disabled = 0;
      thr->delay_delete  = 0;
      thr->انتظار_for_event = 0;
      thr->event  = 0;
      thr->ecount = 0;

      thr->flags[0] = BIT4_0;
      thr->flags[1] = BIT4_1;
      thr->flags[2] = BIT4_X;
      thr->flags[3] = BIT4_Z;
      برائے (صحیح idx = 4 ; idx < 8 ; idx += 1)
	    thr->flags[idx] = BIT4_X;

      scope->دھاگے .insert(thr);
      واپس thr;
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
#اگر 0
/*
 * These are not currently correct. If you use them you will get
 * double delete messages. There is still a leak related to a
 * waiting event that needs to be investigated.
 */

ساکن خالی wait_اگلا_delete(vthread_t بنیاد)
{
      جب (بنیاد) {
	    vthread_t tmp = بنیاد->wait_اگلا;
	    حذف بنیاد;
	    بنیاد = tmp;
	    اگر (بنیاد->انتظار_for_event == 0) توڑو;
      }
}

ساکن خالی بچہ_delete(vthread_t بنیاد)
{
      جب (بنیاد) {
	    vthread_t tmp = بنیاد->بچہ;
	    حذف بنیاد;
	    بنیاد = tmp;
      }
}
#اگر_ختم

خالی vthreads_delete(جماعت __vpiScope*scope)
{
      برائے (std::سیٹ<vthread_t>::iterator cur = scope->دھاگے.begin()
		 ; cur != scope->دھاگے.انتہا() ; ++ cur ) {
	    حذف *cur;
      }
      scope->دھاگے.clear();
}
#اگر_ختم

/*
 * Reaping pulls the thread out of the stack of threads. If I have a
 * child, then hand it over to my parent or fully detach it.
 */
ساکن خالی vthread_reap(vthread_t thr)
{
      اگر (! thr->children.empty()) {
	    برائے (سیٹ<vthread_t>::iterator cur = thr->children.begin()
		       ; cur != thr->children.انتہا() ; ++cur) {
		  vthread_t بچہ = *cur;
		  أكد(بچہ);
		  أكد(بچہ->والد == thr);
		  بچہ->والد = thr->والد;
	    }
      }
      اگر (! thr->detached_children.empty()) {
	    برائے (سیٹ<vthread_t>::iterator cur = thr->detached_children.begin()
		       ; cur != thr->detached_children.انتہا() ; ++cur) {
		  vthread_t بچہ = *cur;
		  أكد(بچہ);
		  أكد(بچہ->والد == thr);
		  أكد(بچہ->i_am_detached);
		  بچہ->والد = 0;
		  بچہ->i_am_detached = 0;
	    }
      }
      اگر (thr->والد) {
	      /* assert that the given element was removed. */
	    اگر (thr->i_am_detached) {
		  size_t res = thr->والد->detached_children.erase(thr);
		  أكد(res == 1);
	    } ورنہ {
		  size_t res = thr->والد->children.erase(thr);
		  أكد(res == 1);
	    }
      }

      thr->والد = 0;

	// Remove myself from the containing scope if needed.
      thr->والد_scope->دھاگے.erase(thr);

      thr->pc = codespace_null();

	/* If this thread is not scheduled, then is it safe to delete
	   it now. Otherwise, let the schedule event (which will
	   execute the thread at of_ZOMBIE) delete the object. */
      اگر ((thr->is_scheduled == 0) && (thr->انتظار_for_event == 0)) {
	    أكد(thr->children.empty());
	    أكد(thr->wait_اگلا == 0);
	    اگر (thr->delay_delete)
		  جدول_وقت_del_thr(thr);
	    ورنہ
		  vthread_delete(thr);
      }
}

خالی vthread_delete(vthread_t thr)
{
      thr->cleanup();
      حذف thr;
}

خالی vthread_mark_scheduled(vthread_t thr)
{
      جب (thr != 0) {
	    أكد(thr->is_scheduled == 0);
	    thr->is_scheduled = 1;
	    thr = thr->wait_اگلا;
      }
}

خالی vthread_mark_final(vthread_t thr)
{
      /*
       * The behavior in a final thread is the same as in a function. Any
       * child thread will be executed immediately rather than being
       * scheduled.
       */
      جب (thr != 0) {
	    thr->i_am_اندر_function = 1;
	    thr = thr->wait_اگلا;
      }
}

خالی vthread_delay_delete()
{
      اگر (running_thread)
	    running_thread->delay_delete = 1;
}

/*
 * This function runs each thread by fetching an instruction,
 * incrementing the PC, and executing the instruction. The thread may
 * be the head of a list, so each thread is run so far as possible.
 */
خالی vthread_تسلسل(vthread_t thr)
{
      جب (thr != 0) {
	    vthread_t tmp = thr->wait_اگلا;
	    thr->wait_اگلا = 0;

	    أكد(thr->is_scheduled);
	    thr->is_scheduled = 0;

            running_thread = thr;

	    برائے (;;) {
		  vvp_رمز_t cp = thr->pc;
		  thr->pc += 1;

		    /* Run the opcode implementation. If the execution of
		       the opcode returns false, then the thread is meant to
		       be paused, so break out of the loop. */
		  منطقی rc = (cp->opcode)(thr, cp);
		  اگر (rc == جھوٹ)
			توڑو;
	    }

	    thr = tmp;
      }
      running_thread = 0;
}

/*
 * The CHUNK_LINK instruction is a special next pointer for linking
 * chunks of code space. It's like a simplified %jmp.
 */
منطقی کا_CHUNK_LINK(vthread_t thr, vvp_رمز_t code)
{
      أكد(code->cptr);
      thr->pc = code->cptr;
      واپس سچ;
}

/*
 * This is called by an event functor to wake up all the threads on
 * its list. I in fact created that list in the %wait instruction, and
 * I also am certain that the waiting_for_event flag is set.
 */
خالی vthread_جدول_وقت_list(vthread_t thr)
{
      برائے (vthread_t cur = thr ;  cur ;  cur = cur->wait_اگلا) {
	    أكد(cur->انتظار_for_event);
	    cur->انتظار_for_event = 0;
      }

      جدول_وقت_vthread(thr, 0);
}

vvp_context_t vthread_get_wt_context()
{
      اگر (running_thread)
            واپس running_thread->wt_context;
      ورنہ
            واپس 0;
}

vvp_context_t vthread_get_rd_context()
{
      اگر (running_thread)
            واپس running_thread->rd_context;
      ورنہ
            واپس 0;
}

vvp_context_شے_t vthread_get_wt_context_شے(غیرعلامتی context_idx)
{
      أكد(running_thread && running_thread->wt_context);
      واپس vvp_get_context_شے(running_thread->wt_context, context_idx);
}

vvp_context_شے_t vthread_get_rd_context_شے(غیرعلامتی context_idx)
{
      أكد(running_thread && running_thread->rd_context);
      واپس vvp_get_context_شے(running_thread->rd_context, context_idx);
}

/*
 * %abs/wr
 */
منطقی کا_ABS_WR(vthread_t thr, vvp_رمز_t)
{
      thr->ڈالو_real( fabs(thr->نکالو_real()) );
      واپس سچ;
}

منطقی کا_ALLOC(vthread_t thr, vvp_رمز_t cp)
{
        /* Allocate a context. */
      vvp_context_t بچہ_context = vthread_alloc_context(cp->scope);

        /* Push the allocated context onto the write context stack. */
      vvp_سیٹ_stacked_context(بچہ_context, thr->wt_context);
      thr->wt_context = بچہ_context;

      واپس سچ;
}

منطقی کا_AND(vthread_t thr, vvp_رمز_t)
{
      vvp_vector4_t valb = thr->نکالو_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();
      أكد(vala.size() == valb.size());
      vala &= valb;
      واپس سچ;
}

/*
 * This function must ALWAYS be called with the val set to the right
 * size, and initialized with BIT4_0 bits. Certain optimizations rely
 * on that.
 */
ساکن خالی get_فوری_rval(vvp_رمز_t cp, vvp_vector4_t&val)
{
      uint32_t vala = cp->ثنائی_ہندسہ_idx[0];
      uint32_t valb = cp->ثنائی_ہندسہ_idx[1];
      غیرعلامتی wid  = cp->نمبر;

      اگر (valb == 0) {
	      // Special case: if the value is zero, we are done
	      // before we start.
	    اگر (vala == 0) واپس;

	      // Special case: The value has no X/Z bits, so we can
	      // use the setarray method to write the value all at once.
	    غیرعلامتی use_wid = 8*حجم(غیرعلامتی طویل);
	    اگر (wid < use_wid)
		  use_wid = wid;
	    غیرعلامتی طویل tmp[1];
	    tmp[0] = vala;
	    val.setarray(0, use_wid, tmp);
	    واپس;
      }

	// The immediate value can be values bigger then 32 bits, but
	// only if the high bits are zero. So at most we need to run
	// through the loop below 32 times. Maybe less, if the target
	// width is less. We don't have to do anything special on that
	// because vala/valb bits will shift away so (vala|valb) will
	// turn to zero at or before 32 shifts.

      برائے (غیرعلامتی idx = 0 ; idx < wid && (vala|valb) ; idx += 1) {
	    uint32_t ba = 0;
	      // Convert the vala/valb bits to a ba number that
	      // matches the encoding of the vvp_bit4_t enumeration.
	    ba = (valb & 1) << 1;
	    ba |= vala & 1;

	      // Note that the val is already pre-filled with BIT4_0
	      // bits, os we only need to set non-zero bit values.
	    اگر (ba) val.سیٹ_ثنائی_ہندسہ(idx, (vvp_bit4_t)ba);

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
منطقی کا_جوڑو(vthread_t thr, vvp_رمز_t)
{
      vvp_vector4_t r = thr->نکالو_vec4();
	// Rather then pop l, use it directly from the stack. When we
	// assign to 'l', that will edit the top of the stack, which
	// replaces a pop and a pull.
      vvp_vector4_t&l = thr->peek_vec4();

      l.جوڑو(r);

      واپس سچ;
}

/*
 * %addi <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
منطقی کا_ADDI(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی wid = cp->نمبر;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_فوری_rval (cp, r);

      l.جوڑو(r);

      واپس سچ;
}

/*
 * %add/wr
 */
منطقی کا_جوڑو_WR(vthread_t thr, vvp_رمز_t)
{
      دوہرا r = thr->نکالو_real();
      دوہرا l = thr->نکالو_real();
      thr->ڈالو_real(l + r);
      واپس سچ;
}

/* %assign/ar <array>, <delay>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory. <delay>
 * is the delay in simulation time. <bit> is the index register
 * containing the real value.
 */
منطقی کا_تفویض_AR(vthread_t thr, vvp_رمز_t cp)
{
      طویل adr = thr->الفاظ[3].w_int;
      غیرعلامتی delay = cp->ثنائی_ہندسہ_idx[0];
      دوہرا value = thr->نکالو_real();

      اگر (adr >= 0) {
	    جدول_وقت_تفویض_array_لفظ(cp->array, adr, value, delay);
      }

      واپس سچ;
}

/* %assign/ar/d <array>, <delay_idx>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory.
 * <delay_idx> is the integer register that contains the delay value.
 */
منطقی کا_تفویض_ARD(vthread_t thr, vvp_رمز_t cp)
{
      طویل adr = thr->الفاظ[3].w_int;
      دوہرا value = thr->نکالو_real();

      اگر (adr >= 0) {
	    vvp_time64_t delay = thr->الفاظ[cp->ثنائی_ہندسہ_idx[0]].w_uint;
	    جدول_وقت_تفویض_array_لفظ(cp->array, adr, value, delay);
      }

      واپس سچ;
}

/* %assign/ar/e <array>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory. <bit>
 * is the index register containing the real value. The event
 * information is contained in the thread event control registers
 * and is set with %evctl.
 */
منطقی کا_تفویض_ARE(vthread_t thr, vvp_رمز_t cp)
{
      طویل adr = thr->الفاظ[3].w_int;
      دوہرا value = thr->نکالو_real();

      اگر (adr >= 0) {
	    اگر (thr->ecount == 0) {
		  جدول_وقت_تفویض_array_لفظ(cp->array, adr, value, 0);
	    } ورنہ {
		  جدول_وقت_evctl(cp->array, adr, value, thr->event,
		                 thr->ecount);
	    }
      }

      واپس سچ;
}

/*
 * %assign/vec4 <var>, <delay>
 */
منطقی کا_تفویض_VEC4(vthread_t thr, vvp_رمز_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      غیرعلامتی delay = cp->ثنائی_ہندسہ_idx[0];
      ثابت vvp_vector4_t&val = thr->peek_vec4();

      جدول_وقت_تفویض_سمتیہ(ptr, 0, 0, val, delay);
      thr->نکالو_vec4(1);
      واپس سچ;
}

/*
 * Resizes a vector value for a partial assignment so that the value is fully
 * in-bounds of the target signal. Both `val` and `off` will be updated if
 * necessary.
 *
 * Returns false if the value is fully out-of-bounds and the assignment should
 * be skipped. Otherwise returns true.
 */
ساکن منطقی resize_rval_vec(vvp_vector4_t &val, int64_t &off,
			    غیرعلامتی صحیح sig_wid)
{
      غیرعلامتی صحیح wid = val.size();

        // Fully in bounds, most likely case
      اگر (off >= 0 && (uint64_t)off + wid <= sig_wid)
	    واپس سچ;

      غیرعلامتی صحیح بنیاد = 0;
      اگر (off >= 0) {
	      // Fully out-of-bounds
	    اگر ((uint64_t)off >= sig_wid)
		  واپس جھوٹ;
      } ورنہ {
	      // Fully out-of-bounds */
	    اگر ((uint64_t)(-off) >= wid)
		  واپس جھوٹ;

	      // If the index is below the vector, then only assign the high
	      // bits that overlap with the target
	    بنیاد = -off;
	    wid += off;
		off = 0;
      }

	// If the value is partly above the target, then only assign
	// the bits that overlap
      اگر ((uint64_t)off + wid > sig_wid)
	    wid = sig_wid - (uint64_t)off;

      val = val.subvalue(بنیاد, wid);

      واپس سچ;
}

/*
 * %assign/vec4/a/d <arr>, <offx>, <delx>
 */
منطقی کا_تفویض_VEC4_A_D(vthread_t thr, vvp_رمز_t cp)
{
      صحیح off_idx = cp->ثنائی_ہندسہ_idx[0];
      صحیح del_idx = cp->ثنائی_ہندسہ_idx[1];
      صحیح adr_idx = 3;

      int64_t  off = off_idx ? thr->الفاظ[off_idx].w_int : 0;
      vvp_time64_t del = del_idx? thr->الفاظ[del_idx].w_uint : 0;
      طویل     adr = thr->الفاظ[adr_idx].w_int;

      vvp_vector4_t val = thr->نکالو_vec4();

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      اگر (thr->flags[4] == BIT4_1)
	    واپس سچ;

      اگر (!resize_rval_vec(val, off, cp->array->get_لفظ_سائز()))
	    واپس سچ;

      جدول_وقت_تفویض_array_لفظ(cp->array, adr, off, val, del);

      واپس سچ;
}

/*
 * %assign/vec4/a/e <arr>, <offx>
 */
منطقی کا_تفویض_VEC4_A_E(vthread_t thr, vvp_رمز_t cp)
{
      صحیح off_idx = cp->ثنائی_ہندسہ_idx[0];
      صحیح adr_idx = 3;

      int64_t  off = off_idx ? thr->الفاظ[off_idx].w_int : 0;
      طویل     adr = thr->الفاظ[adr_idx].w_int;

      vvp_vector4_t val = thr->نکالو_vec4();

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      اگر (thr->flags[4] == BIT4_1)
	    واپس سچ;

      اگر (!resize_rval_vec(val, off, cp->array->get_لفظ_سائز()))
	    واپس سچ;

      اگر (thr->ecount == 0) {
	    جدول_وقت_تفویض_array_لفظ(cp->array, adr, off, val, 0);
      } ورنہ {
	    جدول_وقت_evctl(cp->array, adr, val, off, thr->event, thr->ecount);
      }

      واپس سچ;
}

/*
 * %assign/vec4/off/d <var>, <off>, <del>
 */
منطقی کا_تفویض_VEC4_OFF_D(vthread_t thr, vvp_رمز_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      غیرعلامتی off_اشاریہ = cp->ثنائی_ہندسہ_idx[0];
      غیرعلامتی del_اشاریہ = cp->ثنائی_ہندسہ_idx[1];
      vvp_vector4_t val = thr->نکالو_vec4();

      int64_t off = thr->الفاظ[off_اشاریہ].w_int;
      vvp_time64_t del = thr->الفاظ[del_اشاریہ].w_uint;

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      اگر (thr->flags[4] == BIT4_1)
	    واپس سچ;

      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*> (cp->net->fil);
      أكد(sig);

      اگر (!resize_rval_vec(val, off, sig->قدر_سائز()))
	    واپس سچ;

      جدول_وقت_تفویض_سمتیہ(ptr, off, sig->قدر_سائز(), val, del);
      واپس سچ;
}

/*
 * %assign/vec4/off/e <var>, <off>
 */
منطقی کا_تفویض_VEC4_OFF_E(vthread_t thr, vvp_رمز_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      غیرعلامتی off_اشاریہ = cp->ثنائی_ہندسہ_idx[0];
      vvp_vector4_t val = thr->نکالو_vec4();

      int64_t off = thr->الفاظ[off_اشاریہ].w_int;

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      اگر (thr->flags[4] == BIT4_1)
	    واپس سچ;

      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*> (cp->net->fil);
      أكد(sig);

      اگر (!resize_rval_vec(val, off, sig->قدر_سائز()))
	    واپس سچ;

      اگر (thr->ecount == 0) {
	    جدول_وقت_تفویض_سمتیہ(ptr, off, sig->قدر_سائز(), val, 0);
      } ورنہ {
	    جدول_وقت_evctl(ptr, val, off, sig->قدر_سائز(), thr->event, thr->ecount);
      }

      واپس سچ;
}

/*
 * %assign/vec4/d <var-label> <delay>
 */
منطقی کا_تفویض_VEC4_D(vthread_t thr, vvp_رمز_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      غیرعلامتی del_اشاریہ = cp->ثنائی_ہندسہ_idx[0];
      vvp_time64_t del = thr->الفاظ[del_اشاریہ].w_int;

      vvp_vector4_t value = thr->نکالو_vec4();

      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*> (cp->net->fil);
      أكد(sig);

      جدول_وقت_تفویض_سمتیہ(ptr, 0, sig->قدر_سائز(), value, del);

      واپس سچ;
}

/*
 * %assign/vec4/e <var-label>
 */
منطقی کا_تفویض_VEC4_E(vthread_t thr, vvp_رمز_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_vector4_t value = thr->نکالو_vec4();

      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*> (cp->net->fil);
      أكد(sig);

      اگر (thr->ecount == 0) {
	    جدول_وقت_تفویض_سمتیہ(ptr, 0, sig->قدر_سائز(), value, 0);
      } ورنہ {
	    جدول_وقت_evctl(ptr, value, 0, sig->قدر_سائز(), thr->event, thr->ecount);
      }

      واپس سچ;
}

/*
 * This is %assign/wr <vpi-label>, <delay>
 *
 * This assigns (after a delay) a value to a real variable. Use the
 * vpi_put_value function to do the assign, with the delay written
 * into the vpiInertialDelay carrying the desired delay.
 */
منطقی کا_تفویض_WR(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی delay = cp->ثنائی_ہندسہ_idx[0];
      دوہرا value = thr->نکالو_real();
      s_vpi_وقت del;

      del.type = vpi_Sim_وقت;
      vpip_وقت_مقصد_timestruct(&del, delay);

      __vpiHandle*tmp = cp->handle;

      t_vpi_قدر val;
      val.format = vpiRealVal;
      val.value.real = value;
      vpi_put_قدر(tmp, &val, &del, vpiTransportDelay);

      واپس سچ;
}

منطقی کا_تفویض_WRD(vthread_t thr, vvp_رمز_t cp)
{
      vvp_time64_t delay = thr->الفاظ[cp->ثنائی_ہندسہ_idx[0]].w_uint;
      دوہرا value = thr->نکالو_real();
      s_vpi_وقت del;

      del.type = vpi_Sim_وقت;
      vpip_وقت_مقصد_timestruct(&del, delay);

      __vpiHandle*tmp = cp->handle;

      t_vpi_قدر val;
      val.format = vpiRealVal;
      val.value.real = value;
      vpi_put_قدر(tmp, &val, &del, vpiTransportDelay);

      واپس سچ;
}

منطقی کا_تفویض_WRE(vthread_t thr, vvp_رمز_t cp)
{
      أكد(thr->event != 0);
      دوہرا value = thr->نکالو_real();
      __vpiHandle*tmp = cp->handle;

	// If the count is zero then just put the value.
      اگر (thr->ecount == 0) {
	    t_vpi_قدر val;

	    val.format = vpiRealVal;
	    val.value.real = value;
	    vpi_put_قدر(tmp, &val, 0, vpiNoDelay);
      } ورنہ {
	    جدول_وقت_evctl(tmp, value, thr->event, thr->ecount);
      }

      thr->event = 0;
      thr->ecount = 0;

      واپس سچ;
}

منطقی کا_BLEND(vthread_t thr, vvp_رمز_t)
{
      vvp_vector4_t vala = thr->نکالو_vec4();
      vvp_vector4_t valb = thr->نکالو_vec4();
      أكد(vala.size() == valb.size());

      برائے (غیرعلامتی idx = 0 ; idx < vala.size() ; idx += 1) {
	    اگر (vala.value(idx) == valb.value(idx))
		  جاری;

	    vala.سیٹ_ثنائی_ہندسہ(idx, BIT4_X);
      }

      thr->ڈالو_vec4(vala);
      واپس سچ;
}

منطقی کا_BLEND_STR(vthread_t thr, vvp_رمز_t)
{
      string f = thr->نکالو_str();
      string t = thr->نکالو_str();
      thr->ڈالو_str((t == f) ? t : "");
      واپس سچ;
}

منطقی کا_BLEND_WR(vthread_t thr, vvp_رمز_t)
{
      دوہرا f = thr->نکالو_real();
      دوہرا t = thr->نکالو_real();
      thr->ڈالو_real((t == f) ? t : 0.0);
      واپس سچ;
}

منطقی کا_BREAKPOINT(vthread_t, vvp_رمز_t)
{
      واپس سچ;
}

/*
 * %callf/void <code-label>, <scope-label>
 * Combine the %fork and %join steps for invoking a function.
 */
ساکن منطقی do_callf_void(vthread_t thr, vthread_t بچہ)
{

      اگر (بچہ->والد_scope->is_automatic()) {
	      /* The context allocated for this child is the top entry
		 on the write context stack */
	    بچہ->wt_context = thr->wt_context;
	    بچہ->rd_context = thr->wt_context;
      }

        // Mark the function thread as a direct child of the current thread.
      بچہ->والد = thr;
      thr->children.insert(بچہ);
        // This should be the only child
      أكد(thr->children.size()==1);

        // Execute the function. This SHOULD run the function to completion,
        // but there are some exceptional situations where it won't.
      أكد(بچہ->والد_scope->get_type_رمز() == vpiFunction);
      بچہ->is_scheduled = 1;
      بچہ->i_am_اندر_function = 1;
      vthread_تسلسل(بچہ);
      running_thread = thr;

      اگر (بچہ->i_have_ended) {
	    do_join(thr, بچہ);
	    واپس سچ;
      } ورنہ {
	    thr->i_am_joining = 1;
	    واپس جھوٹ;
      }
}

منطقی کا_CALLF_OBJ(vthread_t thr, vvp_رمز_t cp)
{
      vthread_t بچہ = vthread_new(cp->cptr2, cp->scope);
      واپس do_callf_void(thr, بچہ);

      // XXXX NOT IMPLEMENTED
}

منطقی کا_CALLF_REAL(vthread_t thr, vvp_رمز_t cp)
{
      vthread_t بچہ = vthread_new(cp->cptr2, cp->scope);

	// This is the return value. Push a place-holder value. The function
	// will replace this with the actual value using a %ret/real instruction.
      thr->ڈالو_real(0.0);
      بچہ->دلائل_real.push_back(0);

      واپس do_callf_void(thr, بچہ);
}

منطقی کا_CALLF_STR(vthread_t thr, vvp_رمز_t cp)
{
      vthread_t بچہ = vthread_new(cp->cptr2, cp->scope);

      thr->ڈالو_str("");
      بچہ->دلائل_str.push_back(0);

      واپس do_callf_void(thr, بچہ);
}

منطقی کا_CALLF_VEC4(vthread_t thr, vvp_رمز_t cp)
{
      vthread_t بچہ = vthread_new(cp->cptr2, cp->scope);

      vpiScopeFunction*scope_func = dynamic_cast<vpiScopeFunction*>(cp->scope);
      أكد(scope_func);

	// This is the return value. Push a place-holder value. The function
	// will replace this with the actual value using a %ret/real instruction.
      thr->ڈالو_vec4(vvp_vector4_t(scope_func->get_func_چوڑائی(), scope_func->get_func_init_val()));
      بچہ->دلائل_vec4.push_back(0);

      واپس do_callf_void(thr, بچہ);
}

منطقی کا_CALLF_VOID(vthread_t thr, vvp_رمز_t cp)
{
      vthread_t بچہ = vthread_new(cp->cptr2, cp->scope);
      واپس do_callf_void(thr, بچہ);
}

/*
 * The %cassign/link instruction connects a source node to a
 * destination node. The destination node must be a signal, as it is
 * marked with the source of the cassign so that it may later be
 * unlinked without specifically knowing the source that this
 * instruction used.
 */
منطقی کا_CASSIGN_LINK(vthread_t, vvp_رمز_t cp)
{
      vvp_net_t*dst = cp->net;
      vvp_net_t*src = cp->net2;

      vvp_fun_signal_بنیاد*sig
	    = dynamic_cast<vvp_fun_signal_بنیاد*>(dst->fun);
      أكد(sig);

	/* Any previous continuous assign should have been removed already. */
      أكد(sig->cassign_link == 0);

      sig->cassign_link = src;

	/* Link the output of the src to the port[1] (the cassign
	   port) of the destination. */
      vvp_net_ptr_t dst_ptr (dst, 1);
      src->link(dst_ptr);

      واپس سچ;
}

/*
 * If there is an existing continuous assign linked to the destination
 * node, unlink it. This must be done before applying a new continuous
 * assign, otherwise the initial assigned value will be propagated to
 * any other nodes driven by the old continuous assign source.
 */
ساکن خالی cassign_unlink(vvp_net_t*dst)
{
      vvp_fun_signal_بنیاد*sig
	    = dynamic_cast<vvp_fun_signal_بنیاد*>(dst->fun);
      أكد(sig);

      اگر (sig->cassign_link == 0)
	    واپس;

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
منطقی کا_CASSIGN_VEC4(vthread_t thr, vvp_رمز_t cp)
{
      vvp_net_t*net = cp->net;
      vvp_vector4_t value = thr->نکالو_vec4();

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

	/* Set the value into port 1 of the destination. */
      vvp_net_ptr_t ptr (net, 1);
      vvp_send_vec4(ptr, value, 0);

      واپس سچ;
}

/*
 * %cassign/vec4/off <var>, <off>
 */
منطقی کا_CASSIGN_VEC4_OFF(vthread_t thr, vvp_رمز_t cp)
{
      vvp_net_t*net = cp->net;
      غیرعلامتی بنیاد_idx = cp->ثنائی_ہندسہ_idx[0];
      طویل بنیاد = thr->الفاظ[بنیاد_idx].w_int;
      vvp_vector4_t value = thr->نکالو_vec4();
      غیرعلامتی wid = value.size();

      اگر (thr->flags[4] == BIT4_1)
	    واپس سچ;

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*> (net->fil);
      أكد(sig);

      اگر (بنیاد < 0 && (wid <= (غیرعلامتی)-بنیاد))
	    واپس سچ;

      اگر (بنیاد >= (طویل)sig->قدر_سائز())
	    واپس سچ;

      اگر (بنیاد < 0) {
	    wid -= (غیرعلامتی) -بنیاد;
	    بنیاد = 0;
	    value.resize(wid);
      }

      اگر (بنیاد+wid > sig->قدر_سائز()) {
	    wid = sig->قدر_سائز() - بنیاد;
	    value.resize(wid);
      }

      vvp_net_ptr_t ptr (net, 1);
      vvp_send_vec4_pv(ptr, value, بنیاد, sig->قدر_سائز(), 0);
      واپس سچ;
}

منطقی کا_CASSIGN_WR(vthread_t thr, vvp_رمز_t cp)
{
      vvp_net_t*net  = cp->net;
      دوہرا value = thr->نکالو_real();

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

	/* Set the value into port 1 of the destination. */
      vvp_net_ptr_t ptr (net, 1);
      vvp_send_real(ptr, value, 0);

      واپس سچ;
}

/*
 * %cast2
 */
منطقی کا_CAST2(vthread_t thr, vvp_رمز_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      غیرعلامتی wid = val.size();

      برائے (غیرعلامتی idx = 0 ; idx < wid ; idx += 1) {
	    انتخاب (val.value(idx)) {
		حالت BIT4_0:
		حالت BIT4_1:
		  توڑو;
		طےشدہ:
		  val.سیٹ_ثنائی_ہندسہ(idx, BIT4_0);
		  توڑو;
	    }
      }

      واپس سچ;
}

منطقی do_cast_vec_dar(vthread_t thr, vvp_رمز_t cp, منطقی as_vec4)
{
      غیرعلامتی wid = cp->نمبر;

      vvp_object_t obj;
      thr->نکالو_object(obj);

      vvp_darray*darray = obj.peek<vvp_darray>();
      أكد(darray);

      vvp_vector4_t vec = darray->get_bitstream(as_vec4);
      اگر (vec.size() != wid) {
	    cerr << thr->get_fileline()
	         << "VVP error: size mismatch when casting dynamic array to vector." << سطر_ختم;
            thr->ڈالو_vec4(vvp_vector4_t(wid));
            جدول_وقت_stop(0);
            واپس جھوٹ;
      }
      thr->ڈالو_vec4(vec);
      واپس سچ;
}

/*
 * %cast/vec2/dar <wid>
 */
منطقی کا_CAST_VEC2_DAR(vthread_t thr, vvp_رمز_t cp)
{
      واپس do_cast_vec_dar(thr, cp, جھوٹ);
}

/*
 * %cast/vec4/dar <wid>
 */
منطقی کا_CAST_VEC4_DAR(vthread_t thr, vvp_رمز_t cp)
{
      واپس do_cast_vec_dar(thr, cp, سچ);
}

/*
 * %cast/vec4/str <wid>
 */
منطقی کا_CAST_VEC4_STR(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی wid = cp->نمبر;
      string str = thr->نکالو_str();

      vvp_vector4_t vec(wid, BIT4_0);

      اگر (wid != 8*str.length()) {
	    cerr << thr->get_fileline()
	         << "VVP error: size mismatch when casting string to vector." << سطر_ختم;
            thr->ڈالو_vec4(vec);
            جدول_وقت_stop(0);
            واپس جھوٹ;
      }

      غیرعلامتی sdx = 0;
      غیرعلامتی vdx = wid;
      جب (vdx > 0) {
            حرف ch = str[sdx++];
            vdx -= 8;
            برائے (غیرعلامتی bdx = 0; bdx < 8; bdx += 1) {
                  اگر (ch & 1)
                        vec.سیٹ_ثنائی_ہندسہ(vdx+bdx, BIT4_1);
                  ch >>= 1;
            }
      }

      thr->ڈالو_vec4(vec);
      واپس سچ;
}

ساکن خالی do_CMPE(vthread_t thr, ثابت vvp_vector4_t&lval, ثابت vvp_vector4_t&rval)
{
      أكد(rval.size() == lval.size());

      اگر (lval.has_xz() || rval.has_xz()) {

	    غیرعلامتی wid = lval.size();
	    vvp_bit4_t eq  = BIT4_1;
	    vvp_bit4_t eeq = BIT4_1;

	    برائے (غیرعلامتی idx = 0 ; idx < wid ; idx += 1) {
		  vvp_bit4_t lv = lval.value(idx);
		  vvp_bit4_t rv = rval.value(idx);

		  اگر (lv != rv)
			eeq = BIT4_0;

		  اگر (eq==BIT4_1 && (bit4_is_xz(lv) || bit4_is_xz(rv)))
			eq = BIT4_X;
		  اگر ((lv == BIT4_0) && (rv==BIT4_1))
			eq = BIT4_0;
		  اگر ((lv == BIT4_1) && (rv==BIT4_0))
			eq = BIT4_0;

		  اگر (eq == BIT4_0)
			توڑو;
	    }

	    thr->flags[4] = eq;
	    thr->flags[6] = eeq;

      } ورنہ {
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
منطقی کا_CMPE(vthread_t thr, vvp_رمز_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ثابت vvp_vector4_t&rval = thr->peek_vec4(0);
      ثابت vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPE(thr, lval, rval);

      thr->نکالو_vec4(2);
      واپس سچ;
}

منطقی کا_CMPNE(vthread_t thr, vvp_رمز_t)
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
      واپس سچ;
}

/*
 * %cmpi/e <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
منطقی کا_CMPIE(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی wid = cp->نمبر;

      ثابت vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_فوری_rval (cp, rval);

      do_CMPE(thr, lval, rval);

      thr->نکالو_vec4(1);
      واپس سچ;
}

منطقی کا_CMPINE(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی wid = cp->نمبر;

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
      واپس سچ;
}



ساکن خالی do_CMPS(vthread_t thr, ثابت vvp_vector4_t&lval, ثابت vvp_vector4_t&rval)
{
      أكد(rval.size() == lval.size());

	// If either value has XZ bits, then the eq and lt values are
	// known already to be X. Just calculate the eeq result as a
	// special case and short circuit the rest of the compare.
      اگر (lval.has_xz() || rval.has_xz()) {
	    thr->flags[4] = BIT4_X; // eq
	    thr->flags[5] = BIT4_X; // lt
	    thr->flags[6] = lval.eeq(rval)? BIT4_1 : BIT4_0;
	    واپس;
      }

	// Past this point, we know we are dealing only with fully
	// defined values.
      غیرعلامتی wid = lval.size();

      ثابت vvp_bit4_t sig1 = lval.value(wid-1);
      ثابت vvp_bit4_t sig2 = rval.value(wid-1);

	// If the lval is <0 and the rval is >=0, then we know the result.
      اگر ((sig1 == BIT4_1) && (sig2 == BIT4_0)) {
	    thr->flags[4] = BIT4_0; // eq;
	    thr->flags[5] = BIT4_1; // lt;
	    thr->flags[6] = BIT4_0; // eeq
	    واپس;
      }

	// If the lval is >=0 and the rval is <0, then we know the result.
      اگر ((sig1 == BIT4_0) && (sig2 == BIT4_1)) {
	    thr->flags[4] = BIT4_0; // eq;
	    thr->flags[5] = BIT4_0; // lt;
	    thr->flags[6] = BIT4_0; // eeq
	    واپس;
      }

	// The values have the same sign, so we have to look at the
	// actual value. Scan from the MSB down. As soon as we find a
	// bit that differs, we know the result.

      برائے (غیرعلامتی idx = 1 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t lv = lval.value(wid-1-idx);
	    vvp_bit4_t rv = rval.value(wid-1-idx);

	    اگر (lv == rv)
		  جاری;

	    thr->flags[4] = BIT4_0; // eq
	    thr->flags[6] = BIT4_0; // eeq

	    اگر (lv==BIT4_0) {
		  thr->flags[5] = BIT4_1; // lt
	    } ورنہ {
		  thr->flags[5] = BIT4_0; // lt
	    }
	    واپس;
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
منطقی کا_CMPS(vthread_t thr, vvp_رمز_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ثابت vvp_vector4_t&rval = thr->peek_vec4(0);
      ثابت vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPS(thr, lval, rval);

      thr->نکالو_vec4(2);
      واپس سچ;
}

/*
 * %cmpi/s <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
منطقی کا_CMPIS(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی wid = cp->نمبر;

      ثابت vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_فوری_rval (cp, rval);

      do_CMPS(thr, lval, rval);

      thr->نکالو_vec4(1);
      واپس سچ;
}

منطقی کا_CMPSTR(vthread_t thr, vvp_رمز_t)
{
      string re = thr->نکالو_str();
      string le = thr->نکالو_str();

      صحیح rc = strcmp(le.c_str(), re.c_str());

      vvp_bit4_t eq;
      vvp_bit4_t lt;

      اگر (rc == 0) {
	    eq = BIT4_1;
	    lt = BIT4_0;
      } ورنہ اگر (rc < 0) {
	    eq = BIT4_0;
	    lt = BIT4_1;
      } ورنہ {
	    eq = BIT4_0;
	    lt = BIT4_0;
      }

      thr->flags[4] = eq;
      thr->flags[5] = lt;

      واپس سچ;
}

ساکن خالی کا_CMPU_the_hard_way(vthread_t thr, غیرعلامتی wid,
				 ثابت vvp_vector4_t&lval,
				 ثابت vvp_vector4_t&rval)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_bit4_t eeq = BIT4_1;

      برائے (غیرعلامتی idx = 0 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t lv = lval.value(idx);
	    vvp_bit4_t rv = rval.value(idx);

	    اگر (lv != rv)
		  eeq = BIT4_0;

	    اگر (eq==BIT4_1 && (bit4_is_xz(lv) || bit4_is_xz(rv)))
		  eq = BIT4_X;
	    اگر ((lv == BIT4_0) && (rv==BIT4_1))
		  eq = BIT4_0;
	    اگر ((lv == BIT4_1) && (rv==BIT4_0))
		  eq = BIT4_0;

	    اگر (eq == BIT4_0)
		  توڑو;

      }

      thr->flags[4] = eq;
      thr->flags[5] = BIT4_X;
      thr->flags[6] = eeq;
}

ساکن خالی do_CMPU(vthread_t thr, ثابت vvp_vector4_t&lval, ثابت vvp_vector4_t&rval)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_bit4_t lt = BIT4_0;

      اگر (rval.size() != lval.size()) {
	    cerr << thr->get_fileline()
	         << "VVP ERROR: %cmp/u operand width mismatch: lval=" << lval
		 << ", rval=" << rval << سطر_ختم;
      }
      أكد(rval.size() == lval.size());
      غیرعلامتی wid = lval.size();

      غیرعلامتی طویل*larray = lval.subarray(0,wid);
      اگر (larray == 0) واپس کا_CMPU_the_hard_way(thr, wid, lval, rval);

      غیرعلامتی طویل*rarray = rval.subarray(0,wid);
      اگر (rarray == 0) {
	    حذف[]larray;
	    واپس کا_CMPU_the_hard_way(thr, wid, lval, rval);
      }

      غیرعلامتی الفاظ = (wid+CPU_لفظ_بٹ-1) / CPU_لفظ_بٹ;

      برائے (غیرعلامتی wdx = 0 ; wdx < الفاظ ; wdx += 1) {
	    اگر (larray[wdx] == rarray[wdx])
		  جاری;

	    eq = BIT4_0;
	    اگر (larray[wdx] < rarray[wdx])
		  lt = BIT4_1;
	    ورنہ
		  lt = BIT4_0;
      }

      حذف[]larray;
      حذف[]rarray;

      thr->flags[4] = eq;
      thr->flags[5] = lt;
      thr->flags[6] = eq;
}

منطقی کا_CMPU(vthread_t thr, vvp_رمز_t)
{

      ثابت vvp_vector4_t&rval = thr->peek_vec4(0);
      ثابت vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPU(thr, lval, rval);

      thr->نکالو_vec4(2);
      واپس سچ;
}

/*
 * %cmpi/u <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
منطقی کا_CMPIU(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی wid = cp->نمبر;

      ثابت vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_فوری_rval (cp, rval);

      do_CMPU(thr, lval, rval);

      thr->نکالو_vec4(1);
      واپس سچ;
}


/*
 * %cmp/x
 */
منطقی کا_CMPX(vthread_t thr, vvp_رمز_t)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_vector4_t rval = thr->نکالو_vec4();
      vvp_vector4_t lval = thr->نکالو_vec4();

      أكد(rval.size() == lval.size());
      غیرعلامتی wid = lval.size();

      برائے (غیرعلامتی idx = 0 ; idx < wid ; idx += 1) {
	    vvp_bit4_t lv = lval.value(idx);
	    vvp_bit4_t rv = rval.value(idx);
	    اگر ((lv != rv) && !bit4_is_xz(lv) && !bit4_is_xz(rv)) {
		  eq = BIT4_0;
		  توڑو;
	    }
      }

      thr->flags[4] = eq;
      واپس سچ;
}

ساکن خالی do_CMPWE(vthread_t thr, ثابت vvp_vector4_t&lval, ثابت vvp_vector4_t&rval)
{
      أكد(rval.size() == lval.size());

      اگر (lval.has_xz() || rval.has_xz()) {

	    غیرعلامتی wid = lval.size();
	    vvp_bit4_t eq  = BIT4_1;

	    برائے (غیرعلامتی idx = 0 ; idx < wid ; idx += 1) {
		  vvp_bit4_t lv = lval.value(idx);
		  vvp_bit4_t rv = rval.value(idx);

		  اگر (bit4_is_xz(rv))
			جاری;
		  اگر ((eq == BIT4_1) && bit4_is_xz(lv))
			eq = BIT4_X;
		  اگر ((lv == BIT4_0) && (rv==BIT4_1))
			eq = BIT4_0;
		  اگر ((lv == BIT4_1) && (rv==BIT4_0))
			eq = BIT4_0;

		  اگر (eq == BIT4_0)
			توڑو;
	    }

	    thr->flags[4] = eq;

      } ورنہ {
	      // If there are no XZ bits anywhere, then the results of
	      // ==? match the === test.
	    thr->flags[4] = (lval.eeq(rval)? BIT4_1 : BIT4_0);
      }
}

منطقی کا_CMPWE(vthread_t thr, vvp_رمز_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ثابت vvp_vector4_t&rval = thr->peek_vec4(0);
      ثابت vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPWE(thr, lval, rval);

      thr->نکالو_vec4(2);
      واپس سچ;
}

منطقی کا_CMPWNE(vthread_t thr, vvp_رمز_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ثابت vvp_vector4_t&rval = thr->peek_vec4(0);
      ثابت vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPWE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];

      thr->نکالو_vec4(2);
      واپس سچ;
}

منطقی کا_CMPWR(vthread_t thr, vvp_رمز_t)
{
      دوہرا r = thr->نکالو_real();
      دوہرا l = thr->نکالو_real();

      vvp_bit4_t eq = (l == r)? BIT4_1 : BIT4_0;
      vvp_bit4_t lt = (l <  r)? BIT4_1 : BIT4_0;

      thr->flags[4] = eq;
      thr->flags[5] = lt;

      واپس سچ;
}

/*
 * %cmp/z
 */
منطقی کا_CMPZ(vthread_t thr, vvp_رمز_t)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_vector4_t rval = thr->نکالو_vec4();
      vvp_vector4_t lval = thr->نکالو_vec4();

      أكد(rval.size() == lval.size());
      غیرعلامتی wid = lval.size();

      برائے (غیرعلامتی idx = 0 ; idx < wid ; idx += 1) {
	    vvp_bit4_t lv = lval.value(idx);
	    vvp_bit4_t rv = rval.value(idx);
	    اگر ((lv != rv) && (rv != BIT4_Z) && (lv != BIT4_Z)) {
		  eq = BIT4_0;
		  توڑو;
	    }
      }

      thr->flags[4] = eq;
      واپس سچ;
}

/*
 *  %concat/str;
 */
منطقی کا_CONCAT_STR(vthread_t thr, vvp_رمز_t)
{
      string متن = thr->نکالو_str();
      thr->peek_str(0).append(متن);
      واپس سچ;
}

/*
 *  %concati/str <string>;
 */
منطقی کا_CONCATI_STR(vthread_t thr, vvp_رمز_t cp)
{
      ثابت حرف*متن = cp->متن;
      thr->peek_str(0).append(filter_string(متن));
      واپس سچ;
}

/*
 * %concat/vec4
 */
منطقی کا_CONCAT_VEC4(vthread_t thr, vvp_رمز_t)
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

      واپس سچ;
}

/*
 * %concati/vec4 <vala>, <valb>, <wid>
 *
 * Concat the immediate value to the LOW bits of the concatenation.
 * Get the HIGH bits from the top of the vec4 stack.
 */
منطقی کا_CONCATI_VEC4(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی wid  = cp->نمبر;

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
      واپس سچ;
}

/*
 * %cvt/rv
 */
منطقی کا_CVT_RV(vthread_t thr, vvp_رمز_t)
{
      دوہرا val;
      vvp_vector4_t val4 = thr->نکالو_vec4();
      vector4_مقصد_قدر(val4, val, جھوٹ);
      thr->ڈالو_real(val);
      واپس سچ;
}

/*
 * %cvt/rv/s
 */
منطقی کا_CVT_RV_S(vthread_t thr, vvp_رمز_t)
{
      دوہرا val;
      vvp_vector4_t val4 = thr->نکالو_vec4();
      vector4_مقصد_قدر(val4, val, سچ);
      thr->ڈالو_real(val);
      واپس سچ;
}

/*
 * %cvt/sr <idx>
 * Pop the top value from the real stack, convert it to a 64bit signed
 * and save it to the indexed register.
 */
منطقی کا_CVT_SR(vthread_t thr, vvp_رمز_t cp)
{
      دوہرا r = thr->نکالو_real();
      thr->الفاظ[cp->ثنائی_ہندسہ_idx[0]].w_int = i64round(r);

      واپس سچ;
}

/*
 * %cvt/ur <idx>
 */
منطقی کا_CVT_UR(vthread_t thr, vvp_رمز_t cp)
{
      دوہرا r = thr->نکالو_real();
      thr->الفاظ[cp->ثنائی_ہندسہ_idx[0]].w_uint = vlg_باری_مقصد_u64(r);

      واپس سچ;
}

/*
 * %cvt/vr <wid>
 */
منطقی کا_CVT_VR(vthread_t thr, vvp_رمز_t cp)
{
      دوہرا r = thr->نکالو_real();
      غیرعلامتی wid = cp->نمبر;

      vvp_vector4_t tmp(wid, r);
      thr->ڈالو_vec4(tmp);
      واپس سچ;
}

/*
 * This implements the %deassign instruction. All we do is write a
 * long(1) to port-3 of the addressed net. This turns off an active
 * continuous assign activated by %cassign/v
 */
منطقی کا_DEASSIGN(vthread_t, vvp_رمز_t cp)
{
      vvp_net_t*net = cp->net;
      غیرعلامتی بنیاد  = cp->ثنائی_ہندسہ_idx[0];
      غیرعلامتی width = cp->ثنائی_ہندسہ_idx[1];

      vvp_signal_قدر*fil = dynamic_cast<vvp_signal_قدر*> (net->fil);
      أكد(fil);
      vvp_fun_signal_vec*sig = dynamic_cast<vvp_fun_signal_vec*>(net->fun);
      أكد(sig);

      اگر (بنیاد >= fil->قدر_سائز()) واپس سچ;
      اگر (بنیاد+width > fil->قدر_سائز()) width = fil->قدر_سائز() - بنیاد;

      منطقی بھرا_sig = بنیاد == 0 && width == fil->قدر_سائز();

	// This is the net that is forcing me...
      اگر (vvp_net_t*src = sig->cassign_link) {
	    اگر (! بھرا_sig) {
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
      اگر (بھرا_sig) {
	    sig->deassign();
      } ورنہ {
	    sig->deassign_pv(بنیاد, width);
      }

      واپس سچ;
}

منطقی کا_DEASSIGN_WR(vthread_t, vvp_رمز_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_real*sig = dynamic_cast<vvp_fun_signal_real*>(net->fun);
      أكد(sig);

	// This is the net that is forcing me...
      اگر (vvp_net_t*src = sig->cassign_link) {
	      // And this is the pointer to be removed.
	    vvp_net_ptr_t dst_ptr (net, 1);
	    src->unlink(dst_ptr);
	    sig->cassign_link = 0;
      }

      sig->deassign();

      واپس سچ;
}

/*
 * %debug/thr
 */
منطقی کا_DEBUG_THR(vthread_t thr, vvp_رمز_t cp)
{
      ثابت حرف*متن = cp->متن;
      thr->debug_dump(cerr, متن);
      واپس سچ;
}

/*
 * The delay takes two 32bit numbers to make up a 64bit time.
 *
 *   %delay <low>, <hig>
 */
منطقی کا_DELAY(vthread_t thr, vvp_رمز_t cp)
{
      vvp_time64_t کم = cp->ثنائی_ہندسہ_idx[0];
      vvp_time64_t hig = cp->ثنائی_ہندسہ_idx[1];

      vvp_time64_t delay = (hig << 32) | کم;

      اگر (delay == 0) جدول_وقت_inactive(thr);
      ورنہ جدول_وقت_vthread(thr, delay);
      واپس جھوٹ;
}

منطقی کا_DELAYX(vthread_t thr, vvp_رمز_t cp)
{
      vvp_time64_t delay;

      أكد(cp->نمبر < vthread_s::الفاظ_گنتی);
      delay = thr->الفاظ[cp->نمبر].w_uint;
      اگر (delay == 0) جدول_وقت_inactive(thr);
      ورنہ جدول_وقت_vthread(thr, delay);
      واپس جھوٹ;
}

منطقی کا_DELETE_ELEM(vthread_t thr, vvp_رمز_t cp)
{
      vvp_net_t*net = cp->net;

      int64_t idx_val = thr->الفاظ[3].w_int;
      اگر (thr->flags[4] == BIT4_1) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with undefined index."
	         << سطر_ختم;
	    واپس سچ;
      }
      اگر (idx_val < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with negative index."
	         << سطر_ختم;
	    واپس سچ;
      }
      size_t idx = idx_val;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      أكد(obj);

      vvp_قطار*قطار = obj->get_object().peek<vvp_قطار>();
      اگر (قطار == 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping delete(" << idx
	         << ") on empty queue." << سطر_ختم;
      } ورنہ {
	    size_t size = قطار->get_سائز();
	    اگر (idx >= size) {
		  cerr << thr->get_fileline()
		       << "Warning: skipping out of range delete(" << idx
		       << ") on queue of size " << size << "." << سطر_ختم;
	    } ورنہ {
		  قطار->erase(idx);
	    }
      }

      واپس سچ;
}

/* %delete/obj <label>
 *
 * This operator works by assigning a nil to the target object. This
 * causes any value that might be there to be garbage collected, thus
 * deleting the object.
 */
منطقی کا_DELETE_OBJ(vthread_t thr, vvp_رمز_t cp)
{
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_send_object(ptr, vvp_object_t(), thr->wt_context);

      واپس سچ;
}

/* %reverse/obj <label>
 * Reverse element order of the queue or dynamic array object at this net.
 */
منطقی کا_الٹ_OBJ(vthread_t /*thr*/, vvp_رمز_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      أكد(obj);

      vvp_object_t stor = obj->get_object();
      اگر (stor.test_nil()) واپس سچ;

	/* Match other queue ops: peek queues before vvp_darray so concrete
	   queue types dispatch reverse_elems() correctly. */
      اگر (vvp_قطار* قطار = stor.peek<vvp_قطار>()) {
	    قطار->الٹ_elems();
	    واپس سچ;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      أكد(darray);
      darray->الٹ_elems();

      واپس سچ;
}

/* %reverse/prop/obj <property-index>
 * Reverse queue/darray property storage inside the class object on stack top.
 */
منطقی کا_الٹ_PROP_OBJ(vthread_t thr, vvp_رمز_t cp)
{
      size_t pid = cp->نمبر;

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      اگر (arr_obj.test_nil()) واپس سچ;

      اگر (vvp_قطار* قطار = arr_obj.peek<vvp_قطار>()) {
	    قطار->الٹ_elems();
	    واپس سچ;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      أكد(darray);
      darray->الٹ_elems();

      واپس سچ;
}

/* %sort/obj — ascending sort */
منطقی کا_ترتیب_دو_OBJ(vthread_t /*thr*/, vvp_رمز_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      أكد(obj);

      vvp_object_t stor = obj->get_object();
      اگر (stor.test_nil()) واپس سچ;

      اگر (vvp_قطار* قطار = stor.peek<vvp_قطار>()) {
	    قطار->ترتیب_دو_elems(سچ);
	    واپس سچ;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      أكد(darray);
      darray->ترتیب_دو_elems(سچ);

      واپس سچ;
}

/* %sort/prop/obj */
منطقی کا_ترتیب_دو_PROP_OBJ(vthread_t thr, vvp_رمز_t cp)
{
      size_t pid = cp->نمبر;

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      اگر (arr_obj.test_nil()) واپس سچ;

      اگر (vvp_قطار* قطار = arr_obj.peek<vvp_قطار>()) {
	    قطار->ترتیب_دو_elems(سچ);
	    واپس سچ;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      أكد(darray);
      darray->ترتیب_دو_elems(سچ);

      واپس سچ;
}

/* %rsort/obj — descending sort */
منطقی کا_RSORT_OBJ(vthread_t /*thr*/, vvp_رمز_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      أكد(obj);

      vvp_object_t stor = obj->get_object();
      اگر (stor.test_nil()) واپس سچ;

      اگر (vvp_قطار* قطار = stor.peek<vvp_قطار>()) {
	    قطار->ترتیب_دو_elems(جھوٹ);
	    واپس سچ;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      أكد(darray);
      darray->ترتیب_دو_elems(جھوٹ);

      واپس سچ;
}

/* %rsort/prop/obj */
منطقی کا_RSORT_PROP_OBJ(vthread_t thr, vvp_رمز_t cp)
{
      size_t pid = cp->نمبر;

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      اگر (arr_obj.test_nil()) واپس سچ;

      اگر (vvp_قطار* قطار = arr_obj.peek<vvp_قطار>()) {
	    قطار->ترتیب_دو_elems(جھوٹ);
	    واپس سچ;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      أكد(darray);
      darray->ترتیب_دو_elems(جھوٹ);

      واپس سچ;
}

/* %shuffle/obj */
منطقی کا_SHUFFLE_OBJ(vthread_t /*thr*/, vvp_رمز_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      أكد(obj);

      vvp_object_t stor = obj->get_object();
      اگر (stor.test_nil()) واپس سچ;

      اگر (vvp_قطار* قطار = stor.peek<vvp_قطار>()) {
	    قطار->shuffle_elems();
	    واپس سچ;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      أكد(darray);
      darray->shuffle_elems();

      واپس سچ;
}

/* %shuffle/prop/obj */
منطقی کا_SHUFFLE_PROP_OBJ(vthread_t thr, vvp_رمز_t cp)
{
      size_t pid = cp->نمبر;

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      اگر (arr_obj.test_nil()) واپس سچ;

      اگر (vvp_قطار* قطار = arr_obj.peek<vvp_قطار>()) {
	    قطار->shuffle_elems();
	    واپس سچ;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      أكد(darray);
      darray->shuffle_elems();

      واپس سچ;
}

/* %delete/tail <label>, idx
 *
 * Remove all elements after the one specified.
 */
منطقی کا_DELETE_دم(vthread_t thr, vvp_رمز_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      أكد(obj);

      vvp_قطار*قطار = obj->get_object().peek<vvp_قطار>();
      أكد(قطار);

      غیرعلامتی idx = thr->الفاظ[cp->ثنائی_ہندسہ_idx[0]].w_int;
      قطار->erase_دم(idx);

      واپس سچ;
}

ساکن منطقی do_disable(vthread_t thr, vthread_t مطابقت)
{
      منطقی flag = جھوٹ;

	/* Pull the target thread out of its scope if needed. */
      thr->والد_scope->دھاگے.erase(thr);

	/* Turn the thread off by setting is program counter to
	   zero and setting an OFF bit. */
      thr->pc = codespace_null();
      thr->i_was_disabled = 1;
      thr->i_have_ended = 1;

	/* Turn off all the children of the thread. Simulate a %join
	   for as many times as needed to clear the results of all the
	   %forks that this thread has done. */
      جب (! thr->children.empty()) {

	    vthread_t tmp = *(thr->children.begin());
	    أكد(tmp);
	    أكد(tmp->والد == thr);
	    thr->i_am_joining = 0;
	    اگر (do_disable(tmp, مطابقت))
		  flag = سچ;

	    vthread_reap(tmp);
      }

      vthread_t والد = thr->والد;
      اگر (والد && والد->i_am_joining) {
	      // If a parent is waiting in a %join, wake it up. Note
	      // that it is possible to be waiting in a %join yet
	      // already scheduled if multiple child threads are
	      // ending. So check if the thread is already scheduled
	      // before scheduling it again.
	    والد->i_am_joining = 0;
	    اگر (! والد->i_have_ended)
		  جدول_وقت_vthread(والد, 0, سچ);

	    do_join(والد, thr);

      } ورنہ اگر (والد) {
	      /* If the parent is yet to %join me, let its %join
		 do the reaping. */
	      //assert(tmp->is_scheduled == 0);

      } ورنہ {
	      /* No parent at all. Goodbye. */
	    vthread_reap(thr);
      }

      واپس flag || (thr == مطابقت);
}

/*
 * Implement the %disable instruction by scanning the target scope for
 * all the target threads. Kill the target threads and wake up a
 * parent that is attempting a %join.
 */
منطقی کا_DISABLE(vthread_t thr, vvp_رمز_t cp)
{
      __vpiScope*scope = static_cast<__vpiScope*>(cp->handle);

      منطقی disabled_myself_flag = جھوٹ;

      جب (! scope->دھاگے.empty()) {
	    سیٹ<vthread_t>::iterator cur = scope->دھاگے.begin();

	    اگر (do_disable(*cur, thr))
		  disabled_myself_flag = سچ;
      }

      واپس ! disabled_myself_flag;
}

/*
 * Similar to `of_DISABLE`. But will only disable a single thread of the
 * specified scope. The disabled thread will be the thread closest to the
 * current thread in thread hierarchy. This can either be the current thread,
 * either the thread itself or one of its parents.
 * This is used for SystemVerilog flow control instructions like `return`,
 * `continue` and `break`.
 */

منطقی کا_DISABLE_FLOW(vthread_t thr, vvp_رمز_t cp)
{
      ثابت __vpiScope*scope = static_cast<__vpiScope*>(cp->handle);
      vthread_t cur = thr;

      جب (cur && cur->والد_scope != scope)
	    cur = cur->والد;

      أكد(cur);
      اگر (cur)
	    واپس !do_disable(cur, thr);

      واپس جھوٹ;
}

/*
 * Implement the %disable/fork (SystemVerilog) instruction by disabling
 * all the detached children of the given thread.
 */
منطقی کا_DISABLE_FORK(vthread_t thr, vvp_رمز_t)
{
	/* If a %disable/fork is being executed then the parent thread
	 * cannot be waiting in a join. */
      أكد(! thr->i_am_joining);

	/* There should be no active children to disable. */
      أكد(thr->children.empty());

	/* Disable any detached children. */
      جب (! thr->detached_children.empty()) {
	    vthread_t بچہ = *(thr->detached_children.begin());
	    أكد(بچہ);
	    أكد(بچہ->والد == thr);
	      /* Disabling the children can never match the parent thread. */
	    منطقی res = do_disable(بچہ, thr);
	    أكد(! res);
	    vthread_reap(بچہ);
      }

      واپس سچ;
}

/*
 * This function divides a 2-word number {high, a} by a 1-word
 * number. Assume that high < b.
 */
ساکن غیرعلامتی طویل divide2words(غیرعلامتی طویل a, غیرعلامتی طویل b,
				  غیرعلامتی طویل بلند)
{
      غیرعلامتی طویل نتیجہ = 0;
      جب (بلند > 0) {
	    غیرعلامتی طویل tmp_نتیجہ = ULONG_MAX / b;
	    غیرعلامتی طویل remain = ULONG_MAX % b;

	    remain += 1;
	    اگر (remain >= b) {
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
            اگر(a < remain)
              بلند += 1;

	      // Now result*b + {high,a} == the input {high,a}. It is
	      // possible that the new high >= 1. If so, it will
	      // certainly be less than high from the previous
	      // iteration. Do another iteration and it will shrink,
	      // eventually to 0.
      }

	// high is now 0, so a is the remaining remainder, so we can
	// finish off the integer divide with a simple a/b.

      واپس نتیجہ + a/b;
}

ساکن غیرعلامتی طویل* divide_بٹ(غیرعلامتی طویل*ap, ثابت غیرعلامتی طویل*bp, غیرعلامتی wid)
{
	// Do all our work a cpu-word at a time. The "words" variable
	// is the number of words of the wid.
      غیرعلامتی الفاظ = (wid+CPU_لفظ_بٹ-1) / CPU_لفظ_بٹ;

      غیرعلامتی btop = الفاظ-1;
      جب (btop > 0 && bp[btop] == 0)
	    btop -= 1;

	// Detect divide by 0, and exit.
      اگر (btop==0 && bp[0]==0)
	    واپس 0;

	// The result array will eventually accumulate the result. The
	// diff array is a difference that we use in the intermediate.
      غیرعلامتی طویل*diff  = نیا غیرعلامتی طویل[الفاظ];
      غیرعلامتی طویل*نتیجہ= نیا غیرعلامتی طویل[الفاظ];
      برائے (غیرعلامتی idx = 0 ; idx < الفاظ ; idx += 1)
	    نتیجہ[idx] = 0;

      برائے (غیرعلامتی cur = الفاظ-btop ; cur > 0 ; cur -= 1) {
	    غیرعلامتی cur_ptr = cur-1;
	    غیرعلامتی طویل cur_res;
	    اگر (ap[cur_ptr+btop] >= bp[btop]) {
		  غیرعلامتی طویل بلند = 0;
		  اگر (cur_ptr+btop+1 < الفاظ)
			بلند = ap[cur_ptr+btop+1];
		  cur_res = divide2words(ap[cur_ptr+btop], bp[btop], بلند);

	    } ورنہ اگر (cur_ptr+btop+1 >= الفاظ) {
		  جاری;

	    } ورنہ اگر (ap[cur_ptr+btop+1] == 0) {
		  جاری;

	    } ورنہ {
		  cur_res = divide2words(ap[cur_ptr+btop], bp[btop],
					 ap[cur_ptr+btop+1]);
	    }

	      // cur_res is a guesstimate of the result this far. It
	      // may be 1 too big. (But it will also be >0) Try it,
	      // and if the difference comes out negative, then adjust.

	      // diff = (bp * cur_res)  << cur_ptr;
	    multiply_array_imm(diff+cur_ptr, bp, الفاظ-cur_ptr, cur_res);
	      // ap -= diff
	    غیرعلامتی طویل حاصل = 1;
	    برائے (غیرعلامتی idx = cur_ptr ; idx < الفاظ ; idx += 1)
		  ap[idx] = جوڑو_with_حاصل(ap[idx], ~diff[idx], حاصل);

	      // ap has the diff subtracted out of it. If cur_res was
	      // too large, then ap will turn negative. (We easily
	      // tell that ap turned negative by looking at
	      // carry&1. If it is 0, then it is *negative*.) In that
	      // case, we know that cur_res was too large by 1. Correct by
	      // adding 1b back in and reducing cur_res.
	    اگر ((حاصل&1) == 0) {
		    // Keep adding b back in until the remainder
		    // becomes positive again.
		  کرو {
			cur_res -= 1;
			حاصل = 0;
			برائے (غیرعلامتی idx = cur_ptr ; idx < الفاظ ; idx += 1)
			      ap[idx] = جوڑو_with_حاصل(ap[idx], bp[idx-cur_ptr], حاصل);
		  } جب (حاصل == 0);
	    }

	    نتیجہ[cur_ptr] = cur_res;
      }

	// Now ap contains the remainder and result contains the
	// desired result. We should find that:
	//  input-a = bp * result + ap;

      حذف[]diff;
      واپس نتیجہ;
}

/*
 * %div
 */
منطقی کا_DIV(vthread_t thr, vvp_رمز_t)
{
      vvp_vector4_t valb = thr->نکالو_vec4();
      vvp_vector4_t vala = thr->نکالو_vec4();

      أكد(vala.size()== valb.size());
      غیرعلامتی wid = vala.size();

      غیرعلامتی طویل*ap = vala.subarray(0, wid);
      اگر (ap == 0) {
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->ڈالو_vec4(tmp);
	    واپس سچ;
      }

      غیرعلامتی طویل*bp = valb.subarray(0, wid);
      اگر (bp == 0) {
	    حذف[]ap;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->ڈالو_vec4(tmp);
	    واپس سچ;
      }

	// If the value fits in a single CPU word, then do it the easy way.
      اگر (wid <= CPU_لفظ_بٹ) {
	    اگر (bp[0] == 0) {
		  vvp_vector4_t tmp(wid, BIT4_X);
		  thr->ڈالو_vec4(tmp);
	    } ورنہ {
		  ap[0] /= bp[0];
		  vala.setarray(0, wid, ap);
		  thr->ڈالو_vec4(vala);
	    }
	    حذف[]ap;
	    حذف[]bp;
	    واپس سچ;
      }

      غیرعلامتی طویل*نتیجہ = divide_بٹ(ap, bp, wid);
      اگر (نتیجہ == 0) {
	    حذف[]ap;
	    حذف[]bp;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->ڈالو_vec4(tmp);
	    واپس سچ;
      }

	// Now ap contains the remainder and result contains the
	// desired result. We should find that:
	//  input-a = bp * result + ap;

      vala.setarray(0, wid, نتیجہ);
      thr->ڈالو_vec4(vala);
      حذف[]ap;
      حذف[]bp;
      حذف[]نتیجہ;

      واپس سچ;
}


ساکن خالی negate_الفاظ(غیرعلامتی طویل*val, غیرعلامتی الفاظ)
{
      غیرعلامتی طویل حاصل = 1;
      برائے (غیرعلامتی idx = 0 ; idx < الفاظ ; idx += 1)
	    val[idx] = جوڑو_with_حاصل(0, ~val[idx], حاصل);
}

/*
 * %div/s
 */
منطقی کا_DIV_S(vthread_t thr, vvp_رمز_t)
{
      vvp_vector4_t valb = thr->نکالو_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      أكد(vala.size()== valb.size());
      غیرعلامتی wid = vala.size();
      غیرعلامتی الفاظ = (wid + CPU_لفظ_بٹ - 1) / CPU_لفظ_بٹ;

	// Get the values, left in right, in binary form. If there is
	// a problem with either (caused by an X or Z bit) then we
	// know right away that the entire result is X.
      غیرعلامتی طویل*ap = vala.subarray(0, wid);
      اگر (ap == 0) {
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    واپس سچ;
      }

      غیرعلامتی طویل*bp = valb.subarray(0, wid);
      اگر (bp == 0) {
	    حذف[]ap;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    واپس سچ;
      }

	// Sign extend the bits in the array to fill out the array.
      غیرعلامتی طویل sign_نقاب = 0;
      اگر (غیرعلامتی طویل sign_بٹ = (الفاظ*CPU_لفظ_بٹ) - wid) {
	    sign_نقاب = -1UL << (CPU_لفظ_بٹ-sign_بٹ);
	    اگر (ap[الفاظ-1] & (sign_نقاب>>1))
		  ap[الفاظ-1] |= sign_نقاب;
	    اگر (bp[الفاظ-1] & (sign_نقاب>>1))
		  bp[الفاظ-1] |= sign_نقاب;
      }

	// If the value fits in a single word, then use the native divide.
      اگر (wid <= CPU_لفظ_بٹ) {
	    اگر (bp[0] == 0) {
		  vvp_vector4_t tmp(wid, BIT4_X);
		  vala = tmp;
	    } ورنہ اگر (((طویل)ap[0] == LONG_MIN) && ((طویل)bp[0] == -1)) {
		  vvp_vector4_t tmp(wid, BIT4_0);
		  tmp.سیٹ_ثنائی_ہندسہ(wid-1, BIT4_1);
		  vala = tmp;
	    } ورنہ {
		  طویل tmpa = (طویل) ap[0];
		  طویل tmpb = (طویل) bp[0];
		  طویل res = tmpa / tmpb;
		  ap[0] = ((غیرعلامتی طویل)res) & ~sign_نقاب;
		  vala.setarray(0, wid, ap);
	    }
	    حذف[]ap;
	    حذف[]bp;
	    واپس سچ;
      }

	// We need to the actual division to positive integers. Make
	// them positive here, and remember the negations.
      منطقی negate_flag = جھوٹ;
      اگر ( ((طویل) ap[الفاظ-1]) < 0 ) {
	    negate_flag = سچ;
	    negate_الفاظ(ap, الفاظ);
      }
      اگر ( ((طویل) bp[الفاظ-1]) < 0 ) {
	    negate_flag ^= سچ;
	    negate_الفاظ(bp, الفاظ);
      }

      غیرعلامتی طویل*نتیجہ = divide_بٹ(ap, bp, wid);
      اگر (نتیجہ == 0) {
	    حذف[]ap;
	    حذف[]bp;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    واپس سچ;
      }

      اگر (negate_flag) {
	    negate_الفاظ(نتیجہ, الفاظ);
      }

      نتیجہ[الفاظ-1] &= ~sign_نقاب;

      vala.setarray(0, wid, نتیجہ);
      حذف[]ap;
      حذف[]bp;
      حذف[]نتیجہ;
      واپس سچ;
}

منطقی کا_DIV_WR(vthread_t thr, vvp_رمز_t)
{
      دوہرا r = thr->نکالو_real();
      دوہرا l = thr->نکالو_real();
      thr->ڈالو_real(l / r);

      واپس سچ;
}

/*
 * %dup/obj
 * %dup/real
 * %dup/vec4
 *
 * Push a duplicate of the object on the appropriate stack.
 */
منطقی کا_DUP_OBJ(vthread_t thr, vvp_رمز_t)
{
      vvp_object_t src = thr->peek_object();

        // If it is null push a new null object
      اگر (src.test_nil())
	    thr->ڈالو_object(vvp_object_t());
      ورنہ
	    thr->ڈالو_object(src.duplicate());

      واپس سچ;
}

منطقی کا_DUP_REAL(vthread_t thr, vvp_رمز_t)
{
      thr->ڈالو_real(thr->peek_real(0));
      واپس سچ;
}

منطقی کا_DUP_VEC4(vthread_t thr, vvp_رمز_t)
{
      thr->ڈالو_vec4(thr->peek_vec4(0));
      واپس سچ;
}

/*
 * This terminates the current thread. If there is a parent who is
 * waiting for me to die, then I schedule it. At any rate, I mark
 * myself as a zombie by setting my pc to 0.
 */
منطقی کا_انتہا(vthread_t thr, vvp_رمز_t)
{
      أكد(! thr->انتظار_for_event);
      thr->i_have_ended = 1;
      thr->pc = codespace_null();

	/* Fully detach any detached children. */
      جب (! thr->detached_children.empty()) {
	    vthread_t بچہ = *(thr->detached_children.begin());
	    أكد(بچہ);
	    أكد(بچہ->والد == thr);
	    أكد(بچہ->i_am_detached);
	    بچہ->والد = 0;
	    بچہ->i_am_detached = 0;
	    thr->detached_children.erase(thr->detached_children.begin());
      }

	/* It is an error to still have active children running at this
	 * point in time. They should have all been detached or joined. */
      أكد(thr->children.empty());

	/* If I have a parent who is waiting for me, then mark that I
	   have ended, and schedule that parent. Also, finish the
	   %join for the parent. */
      اگر (!thr->i_am_detached && thr->والد && thr->والد->i_am_joining) {
	    vthread_t tmp = thr->والد;
	    أكد(! thr->i_am_detached);

	    tmp->i_am_joining = 0;
	    جدول_وقت_vthread(tmp, 0, سچ);
	    do_join(tmp, thr);
	    واپس جھوٹ;
      }

	/* If this thread is not fully detached then remove it from the
	 * parents detached_children set and reap it. */
      اگر (thr->i_am_detached) {
	    vthread_t tmp = thr->والد;
	    أكد(tmp);
	    size_t res = tmp->detached_children.erase(thr);
	    أكد(res == 1);
	      /* If the parent is waiting for the detached children to
	       * finish then the last detached child needs to tell the
	       * parent to wake up when it is finished. */
	    اگر (tmp->i_am_انتظار && tmp->detached_children.empty()) {
		  tmp->i_am_انتظار = 0;
		  جدول_وقت_vthread(tmp, 0, سچ);
	    }
	      /* Fully detach this thread so it will be reaped below. */
	    thr->i_am_detached = 0;
	    thr->والد = 0;
      }

	/* If I have no parent, then no one can %join me and there is
	 * no reason to stick around. This can happen, for example if
	 * I am an ``initial'' thread. */
      اگر (thr->والد == 0) {
	    vthread_reap(thr);
	    واپس جھوٹ;
      }

	/* If I make it this far, then I have a parent who may wish
	   to %join me. Remain a zombie so that it can. */

      واپس جھوٹ;
}

/*
 * %event <var-label>
 */
منطقی کا_EVENT(vthread_t thr, vvp_رمز_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_vector4_t tmp (1, BIT4_X);
      vvp_send_vec4(ptr, tmp, thr->wt_context);
      واپس سچ;
}

/*
 * %event/nb <var-label>, <delay>
 */
منطقی کا_EVENT_NB(vthread_t thr, vvp_رمز_t cp)
{
      vvp_time64_t delay;

      delay = thr->الفاظ[cp->ثنائی_ہندسہ_idx[0]].w_uint;
      جدول_وقت_propagate_event(cp->net, delay);
      واپس سچ;
}

منطقی کا_EVCTL(vthread_t thr, vvp_رمز_t cp)
{
      أكد(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      thr->ecount = thr->الفاظ[cp->ثنائی_ہندسہ_idx[0]].w_uint;
      واپس سچ;
}
منطقی کا_EVCTLC(vthread_t thr, vvp_رمز_t)
{
      thr->event = 0;
      thr->ecount = 0;
      واپس سچ;
}

منطقی کا_EVCTLI(vthread_t thr, vvp_رمز_t cp)
{
      أكد(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      thr->ecount = cp->ثنائی_ہندسہ_idx[0];
      واپس سچ;
}

منطقی کا_EVCTLS(vthread_t thr, vvp_رمز_t cp)
{
      أكد(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      int64_t val = thr->الفاظ[cp->ثنائی_ہندسہ_idx[0]].w_int;
      اگر (val < 0) val = 0;
      thr->ecount = val;
      واپس سچ;
}

منطقی کا_FLAG_GET_VEC4(vthread_t thr, vvp_رمز_t cp)
{
      صحیح flag = cp->نمبر;
      أكد(flag < vthread_s::FLAGS_گنتی);

      vvp_vector4_t val (1, thr->flags[flag]);
      thr->ڈالو_vec4(val);

      واپس سچ;
}

/*
 * %flag_inv <flag1>
 */
منطقی کا_FLAG_INV(vthread_t thr, vvp_رمز_t cp)
{
      صحیح flag1 = cp->ثنائی_ہندسہ_idx[0];

      thr->flags[flag1] = ~ thr->flags[flag1];
      واپس سچ;
}

/*
 * %flag_mov <flag1>, <flag2>
 */
منطقی کا_FLAG_MOV(vthread_t thr, vvp_رمز_t cp)
{
      صحیح flag1 = cp->ثنائی_ہندسہ_idx[0];
      صحیح flag2 = cp->ثنائی_ہندسہ_idx[1];

      thr->flags[flag1] = thr->flags[flag2];
      واپس سچ;
}

/*
 * %flag_or <flag1>, <flag2>
 */
منطقی کا_FLAG_OR(vthread_t thr, vvp_رمز_t cp)
{
      صحیح flag1 = cp->ثنائی_ہندسہ_idx[0];
      صحیح flag2 = cp->ثنائی_ہندسہ_idx[1];

      thr->flags[flag1] = thr->flags[flag1] | thr->flags[flag2];
      واپس سچ;
}

منطقی کا_FLAG_سیٹ_IMM(vthread_t thr, vvp_رمز_t cp)
{
      صحیح flag = cp->نمبر;
      صحیح vali = cp->ثنائی_ہندسہ_idx[0];

      أكد(flag < vthread_s::FLAGS_گنتی);
      أكد(vali >= 0 && vali < 4);

      ساکن ثابت vvp_bit4_t map_ثنائی_ہندسہ[4] = {BIT4_0, BIT4_1, BIT4_Z, BIT4_X};
      thr->flags[flag] = map_ثنائی_ہندسہ[vali];
      واپس سچ;
}

منطقی کا_FLAG_سیٹ_VEC4(vthread_t thr, vvp_رمز_t cp)
{
      صحیح flag = cp->نمبر;
      أكد(flag < vthread_s::FLAGS_گنتی);

      ثابت vvp_vector4_t&val = thr->peek_vec4();
      thr->flags[flag] = val.value(0);
      thr->نکالو_vec4(1);

      واپس سچ;
}

/*
 * the %force/link instruction connects a source node to a
 * destination node. The destination node must be a signal, as it is
 * marked with the source of the force so that it may later be
 * unlinked without specifically knowing the source that this
 * instruction used.
 */
منطقی کا_FORCE_LINK(vthread_t, vvp_رمز_t cp)
{
      vvp_net_t*dst = cp->net;
      vvp_net_t*src = cp->net2;

      أكد(dst->fil);
      dst->fil->force_link(dst, src);

      واپس سچ;
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
منطقی کا_FORCE_VEC4(vthread_t thr, vvp_رمز_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_vector4_t value = thr->نکالو_vec4();

	/* Send the force value to the filter on the node. */

      أكد(net->fil);
      اگر (value.size() != net->fil->filter_سائز())
	    value = coerce_مقصد_چوڑائی(value, net->fil->filter_سائز());

      net->force_vec4(value, vvp_vector2_t(vvp_vector2_t::FILL1, net->fil->filter_سائز()));

      واپس سچ;
}

/*
 * %force/vec4/off <net>, <off>
 */
منطقی کا_FORCE_VEC4_OFF(vthread_t thr, vvp_رمز_t cp)
{
      vvp_net_t*net = cp->net;
      غیرعلامتی بنیاد_idx = cp->ثنائی_ہندسہ_idx[0];
      طویل بنیاد = thr->الفاظ[بنیاد_idx].w_int;
      vvp_vector4_t value = thr->نکالو_vec4();
      غیرعلامتی wid = value.size();

      أكد(net->fil);

      اگر (thr->flags[4] == BIT4_1)
	    واپس سچ;

	// This is the width of the target vector.
      غیرعلامتی use_سائز = net->fil->filter_سائز();

      اگر (بنیاد >= (طویل)use_سائز)
	    واپس سچ;
      اگر (بنیاد < -(طویل)use_سائز)
	    واپس سچ;

      اگر ((بنیاد + wid) > use_سائز)
	    wid = use_سائز - بنیاد;

	// Make a mask of which bits are to be forced, 0 for unforced
	// bits and 1 for forced bits.
      vvp_vector2_t نقاب (vvp_vector2_t::FILL0, use_سائز);
      برائے (غیرعلامتی idx = 0 ; idx < wid ; idx += 1)
	    نقاب.سیٹ_ثنائی_ہندسہ(بنیاد+idx, 1);

      vvp_vector4_t tmp (use_سائز, BIT4_Z);

	// vvp_net_t::force_vec4 propagates all the bits of the
	// forced vector value, regardless of the mask. This
	// ensures the unforced bits retain their current value.
      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*>(net->fil);
      أكد(sig);
      sig->vec4_قدر(tmp);

      tmp.سیٹ_vec(بنیاد, value);

      net->force_vec4(tmp, نقاب);
      واپس سچ;
}

/*
 * %force/vec4/off/d <net>, <off>, <del>
 */
منطقی کا_FORCE_VEC4_OFF_D(vthread_t thr, vvp_رمز_t cp)
{
      vvp_net_t*net = cp->net;

      غیرعلامتی بنیاد_idx = cp->ثنائی_ہندسہ_idx[0];
      طویل بنیاد = thr->الفاظ[بنیاد_idx].w_int;

      غیرعلامتی delay_idx = cp->ثنائی_ہندسہ_idx[1];
      vvp_time64_t delay = thr->الفاظ[delay_idx].w_uint;

      vvp_vector4_t value = thr->نکالو_vec4();

      أكد(net->fil);

      اگر (thr->flags[4] == BIT4_1)
	    واپس سچ;

	// This is the width of the target vector.
      غیرعلامتی use_سائز = net->fil->filter_سائز();

      اگر (بنیاد >= (طویل)use_سائز)
	    واپس سچ;
      اگر (بنیاد < -(طویل)use_سائز)
	    واپس سچ;

      جدول_وقت_force_سمتیہ(net, بنیاد, use_سائز, value, delay);
      واپس سچ;
}

منطقی کا_FORCE_WR(vthread_t thr, vvp_رمز_t cp)
{
      vvp_net_t*net  = cp->net;
      دوہرا value = thr->نکالو_real();

      net->force_real(value, vvp_vector2_t(vvp_vector2_t::FILL1, 1));

      واپس سچ;
}

/*
 * The %fork instruction causes a new child to be created and pushed
 * in front of any existing child. This causes the new child to be
 * added to the list of children, and for me to be the parent of the
 * new child.
 */
منطقی کا_FORK(vthread_t thr, vvp_رمز_t cp)
{
      vthread_t بچہ = vthread_new(cp->cptr2, cp->scope);

      اگر (cp->scope->is_automatic()) {
              /* The context allocated for this child is the top entry
                 on the write context stack. */
            بچہ->wt_context = thr->wt_context;
            بچہ->rd_context = thr->wt_context;
      }

      بچہ->والد = thr;
      thr->children.insert(بچہ);

      اگر (thr->i_am_اندر_function) {
	    بچہ->is_scheduled = 1;
	    بچہ->i_am_اندر_function = 1;
	    vthread_تسلسل(بچہ);
	    running_thread = thr;
      } ورنہ {
	    جدول_وقت_vthread(بچہ, 0, سچ);
      }
      واپس سچ;
}

منطقی کا_FREE(vthread_t thr, vvp_رمز_t cp)
{
        /* Pop the child context from the read context stack. */
      vvp_context_t بچہ_context = thr->rd_context;
      thr->rd_context = vvp_get_stacked_context(بچہ_context);

        /* Free the context. */
      vthread_free_context(بچہ_context, cp->scope);

      واپس سچ;
}

/*
 * %inv
 *
 * Logically, this pops a value, inverts is (Verilog style, with Z and
 * X converted to X) and pushes the result. We can more efficiently
 * just to the invert in place.
 */
منطقی کا_INV(vthread_t thr, vvp_رمز_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val.invert();
      واپس سچ;
}


/*
 * Index registers, arithmetic.
 */

ساکن اندرونی int64_t get_as_64_ثنائی_ہندسہ(uint32_t کم_32, uint32_t بلند_32)
{
      int64_t کم = کم_32;
      int64_t res = بلند_32;

      res <<= 32;
      res |= کم;
      واپس res;
}

منطقی کا_IX_جوڑو(vthread_t thr, vvp_رمز_t cp)
{
      thr->الفاظ[cp->نمبر].w_int += get_as_64_ثنائی_ہندسہ(cp->ثنائی_ہندسہ_idx[0],
                                                    cp->ثنائی_ہندسہ_idx[1]);
      واپس سچ;
}

منطقی کا_IX_SUB(vthread_t thr, vvp_رمز_t cp)
{
      thr->الفاظ[cp->نمبر].w_int -= get_as_64_ثنائی_ہندسہ(cp->ثنائی_ہندسہ_idx[0],
                                                    cp->ثنائی_ہندسہ_idx[1]);
      واپس سچ;
}

منطقی کا_IX_MUL(vthread_t thr, vvp_رمز_t cp)
{
      thr->الفاظ[cp->نمبر].w_int *= get_as_64_ثنائی_ہندسہ(cp->ثنائی_ہندسہ_idx[0],
                                                    cp->ثنائی_ہندسہ_idx[1]);
      واپس سچ;
}

منطقی کا_IX_بھرو(vthread_t thr, vvp_رمز_t cp)
{
      thr->الفاظ[cp->نمبر].w_int = get_as_64_ثنائی_ہندسہ(cp->ثنائی_ہندسہ_idx[0],
                                                   cp->ثنائی_ہندسہ_idx[1]);
      واپس سچ;
}

منطقی کا_IX_MOV(vthread_t thr, vvp_رمز_t cp)
{
      thr->الفاظ[cp->ثنائی_ہندسہ_idx[0]].w_int = thr->الفاظ[cp->ثنائی_ہندسہ_idx[1]].w_int;
      واپس سچ;
}

منطقی کا_IX_GETV(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی اشاریہ = cp->ثنائی_ہندسہ_idx[0];
      vvp_net_t*net = cp->net;

      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*>(net->fil);
      اگر (sig == 0) {
	    أكد(net->fil);
	    cerr << thr->get_fileline()
	         << "%%ix/getv error: Net arg not a vector signal? "
		 << typeid(*net->fil).name() << سطر_ختم;
      }
      أكد(sig);

      vvp_vector4_t vec;
      sig->vec4_قدر(vec);
      منطقی حد_سے_تجاوز_flag;
      uint64_t val = 0;
      منطقی معلوم_flag = vector4_مقصد_قدر(vec, حد_سے_تجاوز_flag, val);

      اگر (معلوم_flag)
	    thr->الفاظ[اشاریہ].w_uint = val;
      ورنہ
	    thr->الفاظ[اشاریہ].w_uint = 0;

	/* Set bit 4 as a flag if the input is unknown. */
      thr->flags[4] = معلوم_flag ? (حد_سے_تجاوز_flag ? BIT4_X : BIT4_0) : BIT4_1;

      واپس سچ;
}

منطقی کا_IX_GETV_S(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی اشاریہ = cp->ثنائی_ہندسہ_idx[0];
      vvp_net_t*net = cp->net;

      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*>(net->fil);
      اگر (sig == 0) {
	    أكد(net->fil);
	    cerr << thr->get_fileline()
	         << "%%ix/getv/s error: Net arg not a vector signal? "
		 << "fun=" << typeid(*net->fil).name()
		 << ", fil=" << (net->fil? typeid(*net->fil).name() : "<>")
		 << سطر_ختم;
      }
      أكد(sig);

      vvp_vector4_t vec;
      sig->vec4_قدر(vec);
      int64_t val;
      منطقی معلوم_flag = vector4_مقصد_قدر(vec, val, سچ, سچ);

      اگر (معلوم_flag)
	    thr->الفاظ[اشاریہ].w_int = val;
      ورنہ
	    thr->الفاظ[اشاریہ].w_int = 0;

	/* Set bit 4 as a flag if the input is unknown. */
      thr->flags[4] = معلوم_flag? BIT4_0 : BIT4_1;

      واپس سچ;
}

ساکن uint64_t vec4_مقصد_اشاریہ(vthread_t thr, منطقی signed_flag)
{
	// Get all the information we need about the vec4 vector, then
	// pop it away. We only need the bool bits and the length.
      ثابت vvp_vector4_t&val = thr->peek_vec4();
      غیرعلامتی val_سائز = val.size();
      غیرعلامتی طویل*بٹ = val.subarray(0, val_سائز, جھوٹ);
      thr->نکالو_vec4(1);

	// If there are X/Z bits, then the subarray will give us a nil
	// pointer. Set a flag to indicate the error, and give up.
      اگر (بٹ == 0) {
	    thr->flags[4] = BIT4_1;
	    واپس 0;
      }

      uint64_t v = 0;
      thr->flags[4] = BIT4_0;

      أكد(حجم(بٹ[0]) <= حجم(v));

      v = 0;
      برائے (غیرعلامتی idx = 0 ; idx < val_سائز ; idx += 8*حجم(بٹ[0])) {
	    uint64_t tmp = بٹ[idx/8/حجم(بٹ[0])];
	    اگر (idx < 8*حجم(v)) {
		  v |= tmp << idx;
	    } ورنہ {
		  منطقی حد_سے_تجاوز = signed_flag && (v >> 63) ? ~tmp != 0 : tmp != 0;
		  اگر (حد_سے_تجاوز) {
			thr->flags[4] = BIT4_X;
			توڑو;
		  }
	    }
      }

	// Set the high bits that are not necessarily filled in by the
	// subarray function.
      اگر (val_سائز < 8*حجم(v)) {
	    اگر (signed_flag && (v & (static_cast<uint64_t>(1)<<(val_سائز-1)))) {
		    // Propagate the sign bit...
		  v |= (~static_cast<uint64_t>(0)) << val_سائز;

	    } ورنہ {
		    // Fill with zeros.
		  v &= ~((~static_cast<uint64_t>(0)) << val_سائز);
	    }

      }

      حذف[]بٹ;
      واپس v;
}

/*
 * %ix/vec4 <idx>
 */
منطقی کا_IX_VEC4(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی use_idx = cp->نمبر;
      thr->الفاظ[use_idx].w_uint = vec4_مقصد_اشاریہ(thr, جھوٹ);
      واپس سچ;
}

/*
 * %ix/vec4/s <idx>
 */
منطقی کا_IX_VEC4_S(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی use_idx = cp->نمبر;
      thr->الفاظ[use_idx].w_uint = vec4_مقصد_اشاریہ(thr, سچ);
      واپس سچ;
}

/*
 * The various JMP instruction work simply by pulling the new program
 * counter from the instruction and resuming. If the jump is
 * conditional, then test the bit for the expected value first.
 */
منطقی کا_JMP(vthread_t thr, vvp_رمز_t cp)
{
      thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      اگر (جدول_وقت_stopped()) {
	    جدول_وقت_vthread(thr, 0, جھوٹ);
	    واپس جھوٹ;
      }

      واپس سچ;
}

/*
 * %jmp/0 <pc>, <flag>
 */
منطقی کا_JMP0(vthread_t thr, vvp_رمز_t cp)
{
      اگر (thr->flags[cp->ثنائی_ہندسہ_idx[0]] == BIT4_0)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      اگر (جدول_وقت_stopped()) {
	    جدول_وقت_vthread(thr, 0, جھوٹ);
	    واپس جھوٹ;
      }

      واپس سچ;
}

/*
 * %jmp/0xz <pc>, <flag>
 */
منطقی کا_JMP0_XZ(vthread_t thr, vvp_رمز_t cp)
{
      اگر (thr->flags[cp->ثنائی_ہندسہ_idx[0]] != BIT4_1)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      اگر (جدول_وقت_stopped()) {
	    جدول_وقت_vthread(thr, 0, جھوٹ);
	    واپس جھوٹ;
      }

      واپس سچ;
}

/*
 * %jmp/1 <pc>, <flag>
 */
منطقی کا_JMP1(vthread_t thr, vvp_رمز_t cp)
{
      اگر (thr->flags[cp->ثنائی_ہندسہ_idx[0]] == BIT4_1)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      اگر (جدول_وقت_stopped()) {
	    جدول_وقت_vthread(thr, 0, جھوٹ);
	    واپس جھوٹ;
      }

      واپس سچ;
}

/*
 * %jmp/1xz <pc>, <flag>
 */
منطقی کا_JMP1_XZ(vthread_t thr, vvp_رمز_t cp)
{
      اگر (thr->flags[cp->ثنائی_ہندسہ_idx[0]] != BIT4_0)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      اگر (جدول_وقت_stopped()) {
	    جدول_وقت_vthread(thr, 0, جھوٹ);
	    واپس جھوٹ;
      }

      واپس سچ;
}

/*
 * The %join instruction causes the thread to wait for one child
 * to die.  If a child is already dead (and a zombie) then I reap
 * it and go on. Otherwise, I mark myself as waiting in a join so that
 * children know to wake me when they finish.
 */

ساکن خالی do_join(vthread_t thr, vthread_t بچہ)
{
      أكد(بچہ->والد == thr);

        /* If the immediate child thread is in an automatic scope... */
      اگر (بچہ->wt_context) {
              /* and is the top level task/function thread... */
            اگر (thr->wt_context != thr->rd_context) {
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

ساکن منطقی do_join_opcode(vthread_t thr)
{
      أكد( !thr->i_am_joining );
      أكد( !thr->children.empty());

	// Are there any children that have already ended? If so, then
	// join with that one.
      برائے (سیٹ<vthread_t>::iterator cur = thr->children.begin()
		 ; cur != thr->children.انتہا() ; ++cur) {
	    vthread_t curp = *cur;
	    اگر (! curp->i_have_ended)
		  جاری;

	      // found something!
	    do_join(thr, curp);
	    واپس سچ;
      }

	// Otherwise, tell my children to awaken me when they end,
	// then pause.
      thr->i_am_joining = 1;
      واپس جھوٹ;
}

منطقی کا_JOIN(vthread_t thr, vvp_رمز_t)
{
      واپس do_join_opcode(thr);
}

/*
 * This %join/detach <n> instruction causes the thread to detach
 * threads that were created by an earlier %fork.
 */
منطقی کا_JOIN_DETACH(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی طویل گنتی = cp->نمبر;

      أكد(گنتی == thr->children.size());

      جب (! thr->children.empty()) {
	    vthread_t بچہ = *thr->children.begin();
	    أكد(بچہ->والد == thr);

	      // We cannot detach automatic tasks/functions within an
	      // automatic scope. If we try to do that, we might make
	      // a mess of the allocation of the context. Note that it
	      // is OK if the child context is distinct (See %exec_ufunc.)
	    أكد(بچہ->wt_context==0 || thr->wt_context!=بچہ->wt_context);
	    اگر (بچہ->i_have_ended) {
		    // If the child has already ended, then reap it.
		  vthread_reap(بچہ);

	    } ورنہ {
		  size_t res = بچہ->والد->children.erase(بچہ);
		  أكد(res == 1);
		  بچہ->i_am_detached = 1;
		  thr->detached_children.insert(بچہ);
	    }
      }

      واپس سچ;
}

/*
 * %load/ar <array-label>, <index>;
*/
منطقی کا_بھرو_AR(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی idx = cp->ثنائی_ہندسہ_idx[0];
      دوہرا لفظ;

	/* The result is 0.0 if the address is undefined. */
      اگر (thr->flags[4] == BIT4_1) {
	    لفظ = 0.0;
      } ورنہ {
	    غیرعلامتی adr = thr->الفاظ[idx].w_int;
	    لفظ = cp->array->get_لفظ_r(adr);
      }

      thr->ڈالو_real(لفظ);
      واپس سچ;
}

سانچہ <قسم_نام ELEM>
ساکن منطقی بھرو_dar(vthread_t thr, vvp_رمز_t cp)
{
      int64_t adr = thr->الفاظ[3].w_int;
      vvp_net_t*net = cp->net;
      أكد(net);

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      أكد(obj);

      vvp_darray*darray = obj->get_object().peek<vvp_darray>();

      ELEM لفظ;
      اگر (darray &&
          (adr >= 0) && (thr->flags[4] == BIT4_0)) // A defined address >= 0
	    darray->get_لفظ(adr, لفظ);
      ورنہ
	    dq_default(لفظ, obj->size());

      vthread_ڈالو(thr, لفظ);
      واپس سچ;
}

/*
 * %load/dar/r <array-label>;
 */
منطقی کا_بھرو_DAR_R(vthread_t thr, vvp_رمز_t cp)
{
      واپس بھرو_dar<دوہرا>(thr, cp);
}

/*
 * %load/dar/str <array-label>;
 */
منطقی کا_بھرو_DAR_STR(vthread_t thr, vvp_رمز_t cp)
{
      واپس بھرو_dar<string>(thr, cp);
}

/*
 * %load/dar/vec4 <array-label>;
 */
منطقی کا_بھرو_DAR_VEC4(vthread_t thr, vvp_رمز_t cp)
{
      واپس بھرو_dar<vvp_vector4_t>(thr, cp);
}

/*
 * %load/prop/dar/vec4 <pid>, <wid>;
 *   Indexed read of queue/dynamic-array class property; object on stack,
 *   index in words[3] (same as %load/dar/vec4).
 */
منطقی کا_بھرو_PROP_DAR_VEC4(vthread_t thr, vvp_رمز_t cp)
{
      size_t pid = cp->نمبر;
      غیرعلامتی wid = cp->ثنائی_ہندسہ_idx[0];
      int64_t adr = thr->الفاظ[3].w_int;

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t pobj;
      cobj->get_object(pid, pobj, 0);
      vvp_darray*darray = pobj.peek<vvp_darray>();

      vvp_vector4_t لفظ;
      اگر (darray && (adr >= 0) && (thr->flags[4] == BIT4_0)) {
	    darray->get_لفظ(adr, لفظ);
      } ورنہ {
	    dq_default(لفظ, wid);
      }

      thr->ڈالو_vec4(لفظ);
      واپس سچ;
}

/*
 * %load/obj <var-label>
 */
منطقی کا_بھرو_OBJ(vthread_t thr, vvp_رمز_t cp)
{
      vvp_net_t*net = cp->net;
      vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      أكد(fun);

      vvp_object_t val = fun->get_object();
      thr->ڈالو_object(val);

      واپس سچ;
}

/*
 * %load/obja <index>
 *    Loads the object from array, using index <index> as the index
 *    value. If flags[4] == 1, the calculation of <index> may have
 *    failed, so push nil.
 */
منطقی کا_بھرو_OBJA(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی idx = cp->ثنائی_ہندسہ_idx[0];
      vvp_object_t لفظ;

	/* The result is 0.0 if the address is undefined. */
      اگر (thr->flags[4] == BIT4_1) {
	    ; // Return nil
      } ورنہ {
	    غیرعلامتی adr = thr->الفاظ[idx].w_int;
	    cp->array->get_لفظ_obj(adr, لفظ);
      }

      thr->ڈالو_object(لفظ);
      واپس سچ;
}

/*
 * %load/real <var-label>
 */
منطقی کا_بھرو_REAL(vthread_t thr, vvp_رمز_t cp)
{
      __vpiHandle*tmp = cp->handle;
      t_vpi_قدر val;

      val.format = vpiRealVal;
      vpi_get_قدر(tmp, &val);

      thr->ڈالو_real(val.value.real);

      واپس سچ;
}

/*
 * %load/str <var-label>
 */
منطقی کا_بھرو_STR(vthread_t thr, vvp_رمز_t cp)
{
      vvp_net_t*net = cp->net;


      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (net->fun);
      أكد(fun);

      ثابت string&val = fun->get_string();
      thr->ڈالو_str(val);

      واپس سچ;
}

منطقی کا_بھرو_STRA(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی idx = cp->ثنائی_ہندسہ_idx[0];
      string لفظ;

      اگر (thr->flags[4] == BIT4_1) {
	    لفظ = "";
      } ورنہ {
	    غیرعلامتی adr = thr->الفاظ[idx].w_int;
	    لفظ = cp->array->get_لفظ_str(adr);
      }

      thr->ڈالو_str(لفظ);
      واپس سچ;
}


/*
 * %load/vec4 <net>
 */
منطقی کا_بھرو_VEC4(vthread_t thr, vvp_رمز_t cp)
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
      اگر (sig == 0) {
	    cerr << thr->get_fileline()
	         << "%load/v error: Net arg not a signal? "
		 << (net->fil ? typeid(*net->fil).name() :
	                        typeid(*net->fun).name())
	         << سطر_ختم;
	    أكد(sig);
	    واپس سچ;
      }

	// Extract the value from the signal and directly into the
	// target stack position.
      sig->vec4_قدر(sig_قدر);

      واپس سچ;
}

/*
 * %load/vec4a <arr>, <adrx>
 */
منطقی کا_بھرو_VEC4_A(vthread_t thr, vvp_رمز_t cp)
{
      صحیح adr_اشاریہ = cp->ثنائی_ہندسہ_idx[0];

      طویل adr = thr->الفاظ[adr_اشاریہ].w_int;

	// If flag[3] is set, then the calculation of the address
	// failed, and this load should return X instead of the actual
	// value.
      اگر (thr->flags[4] == BIT4_1) {
	    vvp_vector4_t tmp (cp->array->get_لفظ_سائز(), BIT4_X);
	    thr->ڈالو_vec4(tmp);
	    واپس سچ;
      }

      vvp_vector4_t tmp (cp->array->get_لفظ(adr));
      thr->ڈالو_vec4(tmp);
      واپس سچ;
}

ساکن خالی do_verylong_mod(vvp_vector4_t&vala, ثابت vvp_vector4_t&valb,
			    منطقی بایاں_is_neg, منطقی دایاں_is_neg)
{
      منطقی برآمد_is_neg = بایاں_is_neg;
      ثابت صحیح len=vala.size();
      غیرعلامتی حرف *a, *z, *t;
      a = نیا غیرعلامتی حرف[len+1];
      z = نیا غیرعلامتی حرف[len+1];
      t = نیا غیرعلامتی حرف[len+1];

      غیرعلامتی حرف حاصل;
      غیرعلامتی حرف عارضی;

      صحیح mxa = -1, mxz = -1;
      صحیح i;
      صحیح current, copylen;

      غیرعلامتی lb_حاصل = بایاں_is_neg? 1 : 0;
      غیرعلامتی rb_حاصل = دایاں_is_neg? 1 : 0;
      برائے (صحیح idx = 0 ;  idx < len ;  idx += 1) {
	    غیرعلامتی lb = vala.value(idx);
	    غیرعلامتی rb = valb.value(idx);

	    اگر ((lb | rb) & 2) {
		  حذف []t;
		  حذف []z;
		  حذف []a;
		  vvp_vector4_t tmp(len, BIT4_X);
		  vala = tmp;
		  واپس;
	    }

	    اگر (بایاں_is_neg) {
		  lb = (1-lb) + lb_حاصل;
		  lb_حاصل = (lb & ~1)? 1 : 0;
		  lb &= 1;
	    }
	    اگر (دایاں_is_neg) {
		  rb = (1-rb) + rb_حاصل;
		  rb_حاصل = (rb & ~1)? 1 : 0;
		  rb &= 1;
	    }

	    z[idx]=lb;
	    a[idx]=1-rb;	// for 2s complement add..
      }

      z[len]=0;
      a[len]=1;

      برائے(i=len-1;i>=0;i--) {
	    اگر(! a[i]) {
		  mxa=i;
		  توڑو;
	    }
      }

      برائے(i=len-1;i>=0;i--) {
	    اگر(z[i]) {
		  mxz=i;
		  توڑو;
	    }
      }

      اگر((mxa>mxz)||(mxa==-1)) {
	    اگر(mxa==-1) {
		  حذف []t;
		  حذف []z;
		  حذف []a;
		  vvp_vector4_t tmpx (len, BIT4_X);
		  vala = tmpx;
		  واپس;
	    }

	    جاؤ tally;
      }

      copylen = mxa + 2;
      current = mxz - mxa;

      جب(current > -1) {
	    حاصل = 1;
	    برائے(i=0;i<copylen;i++) {
		  عارضی = z[i+current] + a[i] + حاصل;
		  t[i] = (عارضی&1);
		  حاصل = (عارضی>>1);
	    }

	    اگر(حاصل) {
		  برائے(i=0;i<copylen;i++) {
			z[i+current] = t[i];
		  }
	    }

	    current--;
      }

 tally:

      vvp_vector4_t tmp (len, BIT4_X);
      حاصل = برآمد_is_neg? 1 : 0;
      برائے (صحیح idx = 0 ;  idx < len ;  idx += 1) {
	    غیرعلامتی ob = z[idx];
	    اگر (برآمد_is_neg) {
		  ob = (1-ob) + حاصل;
		  حاصل = (ob & ~1)? 1 : 0;
		  ob = ob & 1;
	    }
	    tmp.سیٹ_ثنائی_ہندسہ(idx, ob?BIT4_1:BIT4_0);
      }
      vala = tmp;
      حذف []t;
      حذف []z;
      حذف []a;
}

منطقی کا_MAX_WR(vthread_t thr, vvp_رمز_t)
{
      دوہرا r = thr->نکالو_real();
      دوہرا l = thr->نکالو_real();
      اگر (r != r)
	    thr->ڈالو_real(l);
      ورنہ اگر (l != l)
	    thr->ڈالو_real(r);
      ورنہ اگر (r < l)
	    thr->ڈالو_real(l);
      ورنہ
	    thr->ڈالو_real(r);
      واپس سچ;
}

منطقی کا_MIN_WR(vthread_t thr, vvp_رمز_t)
{
      دوہرا r = thr->نکالو_real();
      دوہرا l = thr->نکالو_real();
      اگر (r != r)
	    thr->ڈالو_real(l);
      ورنہ اگر (l != l)
	    thr->ڈالو_real(r);
      ورنہ اگر (r < l)
	    thr->ڈالو_real(r);
      ورنہ
	    thr->ڈالو_real(l);
      واپس سچ;
}

منطقی کا_MOD(vthread_t thr, vvp_رمز_t)
{
      vvp_vector4_t valb = thr->نکالو_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      أكد(vala.size()==valb.size());
      غیرعلامتی wid = vala.size();

      اگر(wid <= 8*حجم(غیرعلامتی طویل طویل)) {
	    غیرعلامتی طویل طویل lv = 0, rv = 0;

	    برائے (غیرعلامتی idx = 0 ;  idx < wid ;  idx += 1) {
		  غیرعلامتی طویل طویل lb = vala.value(idx);
		  غیرعلامتی طویل طویل rb = valb.value(idx);

		  اگر ((lb | rb) & 2)
			جاؤ x_برآمد;

		  lv |= (غیرعلامتی طویل طویل) lb << idx;
		  rv |= (غیرعلامتی طویل طویل) rb << idx;
	    }

	    اگر (rv == 0)
		  جاؤ x_برآمد;

	    lv %= rv;

	    برائے (غیرعلامتی idx = 0 ;  idx < wid ;  idx += 1) {
		  vala.سیٹ_ثنائی_ہندسہ(idx, (lv&1)?BIT4_1 : BIT4_0);
		  lv >>= 1;
	    }

	    واپس سچ;

      } ورنہ {
	    do_verylong_mod(vala, valb, جھوٹ, جھوٹ);
	    واپس سچ;
      }

 x_برآمد:
      vala = vvp_vector4_t(wid, BIT4_X);
      واپس سچ;
}

/*
 * %mod/s
 */
منطقی کا_MOD_S(vthread_t thr, vvp_رمز_t)
{
      vvp_vector4_t valb = thr->نکالو_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      أكد(vala.size()==valb.size());
      غیرعلامتی wid = vala.size();

	/* Handle the case that we can fit the bits into a long-long
	   variable. We cause use native % to do the work. */
      اگر(wid <= 8*حجم(طویل طویل)) {
	    طویل طویل lv = 0, rv = 0;

	    برائے (غیرعلامتی idx = 0 ;  idx < wid ;  idx += 1) {
		  طویل طویل lb = vala.value(idx);
		  طویل طویل rb = valb.value(idx);

		  اگر ((lb | rb) & 2)
			جاؤ x_برآمد;

		  lv |= (طویل طویل) lb << idx;
		  rv |= (طویل طویل) rb << idx;
	    }

	    اگر (rv == 0)
		  جاؤ x_برآمد;

	    اگر ((lv == LLONG_MIN) && (rv == -1))
		  جاؤ zero_برآمد;

	      /* Sign extend the signed operands when needed. */
	    اگر (wid < 8*حجم(طویل طویل)) {
		  اگر (lv & (1LL << (wid-1)))
			lv |= -1ULL << wid;
		  اگر (rv & (1LL << (wid-1)))
			rv |= -1ULL << wid;
	    }

	    lv %= rv;

	    برائے (غیرعلامتی idx = 0 ;  idx < wid ;  idx += 1) {
		  vala.سیٹ_ثنائی_ہندسہ(idx, (lv&1)? BIT4_1 : BIT4_0);
		  lv >>= 1;
	    }

	      // vala is the top of the stack, edited in place, so we
	      // do not need to push the result.

	    واپس سچ;

      } ورنہ {

	    منطقی بایاں_is_neg  = vala.value(vala.size()-1) == BIT4_1;
	    منطقی دایاں_is_neg = valb.value(valb.size()-1) == BIT4_1;
	    do_verylong_mod(vala, valb, بایاں_is_neg, دایاں_is_neg);
	    واپس سچ;
      }

 x_برآمد:
      vala = vvp_vector4_t(wid, BIT4_X);
      واپس سچ;
 zero_برآمد:
      vala = vvp_vector4_t(wid, BIT4_0);
      واپس سچ;
}

/*
 * %mod/wr
 */
منطقی کا_MOD_WR(vthread_t thr, vvp_رمز_t)
{
      دوہرا r = thr->نکالو_real();
      دوہرا l = thr->نکالو_real();
      thr->ڈالو_real(fmod(l,r));

      واپس سچ;
}

/*
 * %neg/wr
 */
منطقی کا_NEG_WR(vthread_t thr, vvp_رمز_t)
{
      thr->poke_real(0, -thr->peek_real(0));
      واپس سچ;
}

/*
 * %pad/s <wid>
 */
منطقی کا_PAD_S(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی wid = cp->نمبر;

      vvp_vector4_t&val = thr->peek_vec4();
      غیرعلامتی old_سائز = val.size();

	// Sign-extend.
      اگر (old_سائز < wid)
	    val.resize(wid, val.value(old_سائز-1));
      ورنہ
	    val.resize(wid);

      واپس سچ;
}

/*
 * %pad/u <wid>
 */
منطقی کا_PAD_U(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی wid = cp->نمبر;

      vvp_vector4_t&val = thr->peek_vec4();
      val.resize(wid, BIT4_0);

      واپس سچ;
}

/*
 * %part/s <wid>
 * %part/u <wid>
 * Two values are popped from the stack. First, pop the canonical
 * index of the part select, and second is the value to be
 * selected. The result is pushed back to the stack.
 */
ساکن منطقی کا_PART_بنیاد(vthread_t thr, vvp_رمز_t cp, منطقی signed_flag)
{
      غیرعلامتی wid = cp->نمبر;

      vvp_vector4_t base4 = thr->نکالو_vec4();
      vvp_vector4_t&value = thr->peek_vec4();

      vvp_vector4_t res (wid, BIT4_X);

	// NOTE: This is treating the vector as signed. Is that correct?
      int32_t بنیاد;
      منطقی قدر_ok = vector4_مقصد_قدر(base4, بنیاد, signed_flag);
      اگر (! قدر_ok) {
	    value = res;
	    واپس سچ;
      }

      اگر (بنیاد >= (int32_t)value.size()) {
	    value = res;
	    واپس سچ;
      }

      اگر ((بنیاد+(صحیح)wid) <= 0) {
	    value = res;
	    واپس سچ;
      }

      طویل vbase = 0;
      اگر (بنیاد < 0) {
	    vbase = -بنیاد;
	    wid -= vbase;
	    بنیاد = 0;
      }

      اگر ((بنیاد+wid) > value.size()) {
	    wid = value.size() - بنیاد;
      }

      res .سیٹ_vec(vbase, value.subvalue(بنیاد, wid));
      value = res;

      واپس سچ;
}

منطقی کا_PART_S(vthread_t thr, vvp_رمز_t cp)
{
      واپس کا_PART_بنیاد(thr, cp, سچ);
}

منطقی کا_PART_U(vthread_t thr, vvp_رمز_t cp)
{
      واپس کا_PART_بنیاد(thr, cp, جھوٹ);
}

/*
 * %parti/s <wid>, <basei>, <base_wid>
 * %parti/u <wid>, <basei>, <base_wid>
 *
 * Pop the value to be selected. The result is pushed back to the stack.
 */
ساکن منطقی کا_PARTI_بنیاد(vthread_t thr, vvp_رمز_t cp, منطقی signed_flag)
{
      غیرعلامتی wid = cp->نمبر;
      uint32_t بنیاد = cp->ثنائی_ہندسہ_idx[0];
      uint32_t bwid = cp->ثنائی_ہندسہ_idx[1];

      vvp_vector4_t&value = thr->peek_vec4();

      vvp_vector4_t res (wid, BIT4_X);

	// NOTE: This is treating the vector as signed. Is that correct?
      int32_t use_بنیاد = بنیاد;
      اگر (signed_flag && bwid < 32 && (بنیاد&(1<<(bwid-1)))) {
	    use_بنیاد |= -1UL << bwid;
      }

      اگر (use_بنیاد >= (int32_t)value.size()) {
	    value = res;
	    واپس سچ;
      }

      اگر ((use_بنیاد+(int32_t)wid) <= 0) {
	    value = res;
	    واپس سچ;
      }

      طویل vbase = 0;
      اگر (use_بنیاد < 0) {
	    vbase = -use_بنیاد;
	    wid -= vbase;
	    use_بنیاد = 0;
      }

      اگر ((use_بنیاد+wid) > value.size()) {
	    wid = value.size() - use_بنیاد;
      }

      res .سیٹ_vec(vbase, value.subvalue(use_بنیاد, wid));
      value = res;

      واپس سچ;
}

منطقی کا_PARTI_S(vthread_t thr, vvp_رمز_t cp)
{
      واپس کا_PARTI_بنیاد(thr, cp, سچ);
}

منطقی کا_PARTI_U(vthread_t thr, vvp_رمز_t cp)
{
      واپس کا_PARTI_بنیاد(thr, cp, جھوٹ);
}

/*
 * %mul
 */
منطقی کا_MUL(vthread_t thr, vvp_رمز_t)
{
      vvp_vector4_t r = thr->نکالو_vec4();
	// Rather then pop l, use it directly from the stack. When we
	// assign to 'l', that will edit the top of the stack, which
	// replaces a pop and a pull.
      vvp_vector4_t&l = thr->peek_vec4();

      l.mul(r);
      واپس سچ;
}

/*
 * %muli <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
منطقی کا_MULI(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی wid = cp->نمبر;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_فوری_rval (cp, r);

      l.mul(r);
      واپس سچ;
}

منطقی کا_MUL_WR(vthread_t thr, vvp_رمز_t)
{
      دوہرا r = thr->نکالو_real();
      دوہرا l = thr->نکالو_real();
      thr->ڈالو_real(l * r);

      واپس سچ;
}

منطقی کا_NAND(vthread_t thr, vvp_رمز_t)
{
      vvp_vector4_t valr = thr->نکالو_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      أكد(vall.size() == valr.size());

      vall &= valr;
      vall.invert();

      واپس سچ;
}

/*
 * %new/cobj <vpi_object>
 * This creates a new cobject (SystemVerilog class object) and pushes
 * it to the stack. The <vpi-object> is a __vpiHandle that is a
 * vpiClassDefn object that defines the item to be created.
 */
منطقی کا_NEW_COBJ(vthread_t thr, vvp_رمز_t cp)
{
      ثابت class_type*defn = dynamic_cast<ثابت class_type*> (cp->handle);
      أكد(defn);

      vvp_object_t tmp (نیا vvp_cobject(defn));
      thr->ڈالو_object(tmp);
      واپس سچ;
}

منطقی کا_NEW_DARRAY(vthread_t thr, vvp_رمز_t cp)
{
      ثابت حرف*متن = cp->متن;
      size_t size = thr->الفاظ[cp->ثنائی_ہندسہ_idx[0]].w_int;
      غیرعلامتی لفظ_wid;
      size_t n;

      vvp_object_t obj;
      اگر (strcmp(متن,"b8") == 0) {
	    obj = نیا vvp_darray_atom<uint8_t>(size);
      } ورنہ اگر (strcmp(متن,"b16") == 0) {
	    obj = نیا vvp_darray_atom<uint16_t>(size);
      } ورنہ اگر (strcmp(متن,"b32") == 0) {
	    obj = نیا vvp_darray_atom<uint32_t>(size);
      } ورنہ اگر (strcmp(متن,"b64") == 0) {
	    obj = نیا vvp_darray_atom<uint64_t>(size);
      } ورنہ اگر (strcmp(متن,"sb8") == 0) {
	    obj = نیا vvp_darray_atom<int8_t>(size);
      } ورنہ اگر (strcmp(متن,"sb16") == 0) {
	    obj = نیا vvp_darray_atom<int16_t>(size);
      } ورنہ اگر (strcmp(متن,"sb32") == 0) {
	    obj = نیا vvp_darray_atom<int32_t>(size);
      } ورنہ اگر (strcmp(متن,"sb64") == 0) {
	    obj = نیا vvp_darray_atom<int64_t>(size);
      } ورنہ اگر ((1 == لڑی_سے_پڑھو(متن, "b%u%zn", &لفظ_wid, &n)) &&
                 (n == لڑی_لمبائی(متن))) {
	    obj = نیا vvp_darray_vec2(size, لفظ_wid);
      } ورنہ اگر ((1 == لڑی_سے_پڑھو(متن, "sb%u%zn", &لفظ_wid, &n)) &&
                 (n == لڑی_لمبائی(متن))) {
	    obj = نیا vvp_darray_vec2(size, لفظ_wid);
      } ورنہ اگر ((1 == لڑی_سے_پڑھو(متن, "v%u%zn", &لفظ_wid, &n)) &&
                 (n == لڑی_لمبائی(متن))) {
	    obj = نیا vvp_darray_vec4(size, لفظ_wid);
      } ورنہ اگر ((1 == لڑی_سے_پڑھو(متن, "sv%u%zn", &لفظ_wid, &n)) &&
                 (n == لڑی_لمبائی(متن))) {
	    obj = نیا vvp_darray_vec4(size, لفظ_wid);
      } ورنہ اگر (strcmp(متن,"r") == 0) {
	    obj = نیا vvp_darray_real(size);
      } ورنہ اگر (strcmp(متن,"S") == 0) {
	    obj = نیا vvp_darray_string(size);
      } ورنہ {
	    cerr << get_fileline()
	         << "Internal error: Unsupported dynamic array type: "
	         << متن << "." << سطر_ختم;
	    أكد(0);
      }

      thr->ڈالو_object(obj);

      واپس سچ;
}

منطقی کا_NOOP(vthread_t, vvp_رمز_t)
{
      واپس سچ;
}

/*
 * %nor/r
 */
منطقی کا_NORR(vthread_t thr, vvp_رمز_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_or());

      واپس سچ;
}

/*
 * Push a null to the object stack.
 */
منطقی کا_NULL(vthread_t thr, vvp_رمز_t)
{
      vvp_object_t tmp;
      thr->ڈالو_object(tmp);
      واپس سچ;
}

/*
 * %and/r
 */
منطقی کا_ANDR(vthread_t thr, vvp_رمز_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_and());

      واپس سچ;
}

/*
 * %nand/r
 */
منطقی کا_NANDR(vthread_t thr, vvp_رمز_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_and());

      واپس سچ;
}

/*
 * %or/r
 */
منطقی کا_ORR(vthread_t thr, vvp_رمز_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_or());

      واپس سچ;
}

/*
 * %xor/r
 */
منطقی کا_XORR(vthread_t thr, vvp_رمز_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_xor());

      واپس سچ;
}

/*
 * %xnor/r
 */
منطقی کا_XNORR(vthread_t thr, vvp_رمز_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_xor());

      واپس سچ;
}

/*
 * %or
 */
منطقی کا_OR(vthread_t thr, vvp_رمز_t)
{
      vvp_vector4_t valb = thr->نکالو_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();
      vala |= valb;
      واپس سچ;
}

/*
 * %nor
 */
منطقی کا_NOR(vthread_t thr, vvp_رمز_t)
{
      vvp_vector4_t valr = thr->نکالو_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      أكد(vall.size() == valr.size());

      vall |= valr;
      vall.invert();

      واپس سچ;
}

/*
 * %pop/obj <num>, <skip>
 */
منطقی کا_نکالو_OBJ(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی cnt = cp->ثنائی_ہندسہ_idx[0];
      غیرعلامتی skip = cp->ثنائی_ہندسہ_idx[1];

      thr->نکالو_object(cnt, skip);
      واپس سچ;
}

/*
 * %pop/real <number>
 */
منطقی کا_نکالو_REAL(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی cnt = cp->نمبر;
      thr->نکالو_real(cnt);
      واپس سچ;
}

/*
 *  %pop/str <number>
 */
منطقی کا_نکالو_STR(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی cnt = cp->نمبر;
      thr->نکالو_str(cnt);
      واپس سچ;
}

/*
 *  %pop/vec4 <number>
 */
منطقی کا_نکالو_VEC4(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی cnt = cp->نمبر;
      thr->نکالو_vec4(cnt);
      واپس سچ;
}

/*
 * %pow
 * %pow/s
 */
ساکن منطقی کا_POW_بنیاد(vthread_t thr, منطقی signed_flag)
{
      vvp_vector4_t valb = thr->نکالو_vec4();
      vvp_vector4_t vala = thr->نکالو_vec4();

      غیرعلامتی wid = vala.size();

      vvp_vector2_t xv2 = vvp_vector2_t(vala, سچ);
      vvp_vector2_t yv2 = vvp_vector2_t(valb, سچ);


        /* If we have an X or Z in the arguments return X. */
      اگر (xv2.is_NaN() || yv2.is_NaN()) {
	    vvp_vector4_t tmp (wid, BIT4_X);
	    thr->ڈالو_vec4(tmp);
	    واپس سچ;
      }

	// Is the exponent negative? If so, table 5-6 in IEEE1364-2005
	// defines what value is returned.
      اگر (signed_flag && yv2.value(yv2.size()-1)) {
	    صحیح a_val;
	    vvp_bit4_t pad = BIT4_0, lsb = BIT4_0;
	    اگر (vector2_مقصد_قدر(xv2, a_val, سچ)) {
		  اگر (a_val == 0) {
			pad = BIT4_X; lsb = BIT4_X;
		  }
		  اگر (a_val == 1) {
			pad = BIT4_0; lsb = BIT4_1;
		  }
		  اگر (a_val == -1) {
			اگر (yv2.value(0)) {
			      pad = BIT4_1; lsb = BIT4_1;
			} ورنہ {
			      pad = BIT4_0; lsb = BIT4_1;
			}
		  }
	    }
	    vvp_vector4_t tmp (wid, pad);
	    tmp.سیٹ_ثنائی_ہندسہ(0, lsb);
	    thr->ڈالو_vec4(tmp);
	    واپس سچ;
      }

      vvp_vector2_t نتیجہ = قوت(xv2, yv2);

        /* Copy only what we need of the result. If the result is too
	   small, zero-pad it. */
      برائے (غیرعلامتی jdx = 0;  jdx < wid;  jdx += 1) {
	    اگر (jdx >= نتیجہ.size())
		  vala.سیٹ_ثنائی_ہندسہ(jdx, BIT4_0);
	    ورنہ
		  vala.سیٹ_ثنائی_ہندسہ(jdx, نتیجہ.value(jdx) ? BIT4_1 : BIT4_0);
      }
      thr->ڈالو_vec4(vala);

      واپس سچ;
}

منطقی کا_POW(vthread_t thr, vvp_رمز_t)
{
      واپس کا_POW_بنیاد(thr, جھوٹ);
}

منطقی کا_POW_S(vthread_t thr, vvp_رمز_t)
{
      واپس کا_POW_بنیاد(thr, سچ);
}

منطقی کا_POW_WR(vthread_t thr, vvp_رمز_t)
{
      دوہرا r = thr->نکالو_real();
      دوہرا l = thr->نکالو_real();
      thr->ڈالو_real(قوت(l,r));

      واپس سچ;
}

/*
 * %prop/obj <pid>, <idx>
 *
 * Load an object value from the cobject and push it onto the object stack.
 */
منطقی کا_PROP_OBJ(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی pid = cp->نمبر;
      غیرعلامتی idx = cp->ثنائی_ہندسہ_idx[0];

      اگر (idx != 0) {
	    أكد(idx < vthread_s::الفاظ_گنتی);
	    idx = thr->الفاظ[idx].w_uint;
      }

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();

      vvp_object_t val;
      cobj->get_object(pid, val, idx);

      thr->ڈالو_object(val);

      واپس سچ;
}

ساکن خالی get_from_obj(غیرعلامتی pid, vvp_cobject*cobj, دوہرا&val)
{
      val = cobj->get_real(pid);
}

ساکن خالی get_from_obj(غیرعلامتی pid, vvp_cobject*cobj, string&val)
{
      val = cobj->get_string(pid);
}

ساکن خالی get_from_obj(غیرعلامتی pid, vvp_cobject*cobj, vvp_vector4_t&val)
{
      cobj->get_vec4(pid, val);
}

سانچہ <قسم_نام ELEM>
ساکن منطقی prop(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی pid = cp->نمبر;

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      أكد(cobj);

      ELEM val;
      get_from_obj(pid, cobj, val);
      vthread_ڈالو(thr, val);

      واپس سچ;
}

/*
 * %prop/r <pid>
 *
 * Load a real value from the cobject and push it onto the real value
 * stack.
 */
منطقی کا_PROP_R(vthread_t thr, vvp_رمز_t cp)
{
      واپس prop<دوہرا>(thr, cp);
}

/*
 * %prop/str <pid>
 *
 * Load a string value from the cobject and push it onto the real value
 * stack.
 */
منطقی کا_PROP_STR(vthread_t thr, vvp_رمز_t cp)
{
      واپس prop<string>(thr, cp);
}

/*
 * %prop/v <pid>
 *
 * Load a property <pid> from the cobject on the top of the stack into
 * the vector space at <base>.
 */
منطقی کا_PROP_V(vthread_t thr, vvp_رمز_t cp)
{
      واپس prop<vvp_vector4_t>(thr, cp);
}

منطقی کا_PUSHI_REAL(vthread_t thr, vvp_رمز_t cp)
{
      دوہرا mant = cp->ثنائی_ہندسہ_idx[0];
      uint32_t imant = cp->ثنائی_ہندسہ_idx[0];
      صحیح قوت_نما = cp->ثنائی_ہندسہ_idx[1];

	// Detect +infinity
      اگر (قوت_نما==0x3fff && imant==0) {
	    thr->ڈالو_real(INFINITY);
	    واپس سچ;
      }
	// Detect -infinity
      اگر (قوت_نما==0x7fff && imant==0) {
	    thr->ڈالو_real(-INFINITY);
	    واپس سچ;
      }
	// Detect NaN
      اگر (قوت_نما==0x3fff) {
	    thr->ڈالو_real(nan(""));
	    واپس سچ;
      }

      دوہرا sign = (قوت_نما & 0x4000)? -1.0 : 1.0;

      قوت_نما &= 0x1fff;

      mant = sign * ldexp(mant, قوت_نما - 0x1000);
      thr->ڈالو_real(mant);
      واپس سچ;
}

منطقی کا_PUSHI_STR(vthread_t thr, vvp_رمز_t cp)
{
      ثابت حرف*متن = cp->متن;
      thr->ڈالو_str(filter_string(متن));
      واپس سچ;
}

/*
 * %pushi/vec4 <vala>, <valb>, <wid>
 */
منطقی کا_PUSHI_VEC4(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی wid  = cp->نمبر;

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t val (wid, BIT4_0);
      get_فوری_rval (cp, val);

      thr->ڈالو_vec4(val);

      واپس سچ;
}

/*
 * %pushv/str
 *   Pops a vec4 value, and pushes a string.
 */
منطقی کا_PUSHV_STR(vthread_t thr, vvp_رمز_t)
{
      vvp_vector4_t vec = thr->نکالو_vec4();

      size_t slen = (vec.size() + 7)/8;
      vector<حرف>buf;
      buf.reserve(slen);

      برائے (size_t idx = 0 ; idx < vec.size() ; idx += 8) {
	    حرف tmp = 0;
	    size_t trans = 8;
	    اگر (idx+trans > vec.size())
		  trans = vec.size() - idx;

	    برائے (size_t bdx = 0 ; bdx < trans ; bdx += 1) {
		  اگر (vec.value(idx+bdx) == BIT4_1)
			tmp |= 1 << bdx;
	    }

	    اگر (tmp != 0)
		  buf.push_back(tmp);
      }

      string val;
      برائے (vector<حرف>::reverse_iterator cur = buf.rbegin()
		 ; cur != buf.rend() ; ++cur) {
	    val.push_back(*cur);
      }

      thr->ڈالو_str(val);

      واپس سچ;
}

/*
 * %putc/str/vec4 <var>, <mux>
 */
منطقی کا_PUTC_STR_VEC4(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی muxr = cp->ثنائی_ہندسہ_idx[0];
      int32_t mux = muxr? thr->الفاظ[muxr].w_int : 0;

      vvp_vector4_t val = thr->نکالو_vec4();
      أكد(val.size() == 8);

      اگر (mux < 0)
	    واپس سچ;

	/* Get the existing value of the string. If we find that the
	   index is too big for the string, then give up. */
      vvp_net_t*net = cp->net;
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (net->fun);
      أكد(fun);

      string tmp = fun->get_string();
      اگر (tmp.size() <= (size_t)mux)
	    واپس سچ;

      حرف val_str = 0;
      برائے (size_t idx = 0 ; idx < 8 ; idx += 1) {
	    اگر (val.value(idx)==BIT4_1)
		  val_str |= 1<<idx;
      }

	// It is a quirk of the Verilog standard that putc(..., 'h00)
	// has no effect. Test for that case here.
      اگر (val_str == 0)
	    واپس سچ;

      tmp[mux] = val_str;

      vvp_send_string(vvp_net_ptr_t(cp->net, 0), tmp, thr->wt_context);
      واپس سچ;
}

سانچہ <قسم_نام ELEM, جماعت QTYPE>
ساکن منطقی qinsert(vthread_t thr, vvp_رمز_t cp, غیرعلامتی wid=0)
{
      int64_t idx = thr->الفاظ[3].w_int;
      ELEM value;
      vvp_net_t*net = cp->net;
      نکالو_قدر(thr, value, wid); // Pop the value to store.

      vvp_قطار*قطار = get_قطار_object<QTYPE>(thr, net);
      أكد(قطار);
      اگر (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at a negative "
	         << get_قطار_type(value)
	         << " index (" << idx << "). ";
	    print_قطار_قدر(value);
	    cerr << " was not added." << سطر_ختم;
      } ورنہ اگر (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at an undefined "
	         << get_قطار_type(value) << " index. ";
	    print_قطار_قدر(value);
	    cerr << " was not added." << سطر_ختم;
      } ورنہ {
	    غیرعلامتی max_سائز = thr->الفاظ[cp->ثنائی_ہندسہ_idx[0]].w_int;
	    قطار->insert(idx, value, max_سائز);
      }
      واپس سچ;
}

/*
 * %qinsert/real <var-label>
 */
منطقی کا_QINSERT_REAL(vthread_t thr, vvp_رمز_t cp)
{
      واپس qinsert<دوہرا, vvp_قطار_real>(thr, cp);
}

/*
 * %qinsert/str <var-label>
 */
منطقی کا_QINSERT_STR(vthread_t thr, vvp_رمز_t cp)
{
      واپس qinsert<string, vvp_قطار_string>(thr, cp);
}

/*
 * %qinsert/v <var-label>
 */
منطقی کا_QINSERT_V(vthread_t thr, vvp_رمز_t cp)
{
      واپس qinsert<vvp_vector4_t, vvp_قطار_vec4>(thr, cp, cp->ثنائی_ہندسہ_idx[1]);
}

/*
 * Helper functions used in the queue pop templates
 */
اندرونی خالی ڈالو_قدر(vthread_t thr, دوہرا value, غیرعلامتی)
{
      thr->ڈالو_real(value);
}

اندرونی خالی ڈالو_قدر(vthread_t thr, ثابت string&value, غیرعلامتی)
{
      thr->ڈالو_str(value);
}

اندرونی خالی ڈالو_قدر(vthread_t thr, ثابت vvp_vector4_t&value, غیرعلامتی wid)
{
      أكد(wid == value.size());
      thr->ڈالو_vec4(value);
}

سانچہ <قسم_نام ELEM, جماعت QTYPE>
ساکن منطقی q_نکالو(vthread_t thr, vvp_رمز_t cp,
                  خالی (*get_val_func)(vvp_قطار*, ELEM&),
                  ثابت حرف*loc, غیرعلامتی wid)
{
      vvp_net_t*net = cp->net;

      vvp_قطار*قطار = get_قطار_object<QTYPE>(thr, net);
      أكد(قطار);

      size_t size = قطار->get_سائز();

      ELEM value;
      اگر (size) {
	    get_val_func(قطار, value);
      } ورنہ {
	    dq_default(value, wid);
	    cerr << thr->get_fileline()
	         << "Warning: pop_" << loc << "() on empty "
	         << get_قطار_type(value) << "." << سطر_ختم;
      }

      ڈالو_قدر(thr, value, wid);
      واپس سچ;
}

سانچہ <قسم_نام ELEM>
ساکن خالی get_عقب_قدر(vvp_قطار*قطار, ELEM&value)
{
      قطار->get_لفظ(قطار->get_سائز()-1, value);
      قطار->pop_back();
}

سانچہ <قسم_نام ELEM, جماعت QTYPE>
ساکن منطقی qpop_b(vthread_t thr, vvp_رمز_t cp, غیرعلامتی wid=0)
{
      واپس q_نکالو<ELEM, QTYPE>(thr, cp, get_عقب_قدر<ELEM>, "back", wid);
}

/*
 * %qpop/b/real <var-label>
 */
منطقی کا_QPOP_B_REAL(vthread_t thr, vvp_رمز_t cp)
{
      واپس qpop_b<دوہرا, vvp_قطار_real>(thr, cp);
}

/*
 * %qpop/b/str <var-label>
 */
منطقی کا_QPOP_B_STR(vthread_t thr, vvp_رمز_t cp)
{
      واپس qpop_b<string, vvp_قطار_string>(thr, cp);
}

/*
 * %qpop/b/v <var-label>
 */
منطقی کا_QPOP_B_V(vthread_t thr, vvp_رمز_t cp)
{
      واپس qpop_b<vvp_vector4_t, vvp_قطار_vec4>(thr, cp, cp->ثنائی_ہندسہ_idx[0]);
}

سانچہ <قسم_نام ELEM>
ساکن خالی get_front_قدر(vvp_قطار*قطار, ELEM&value)
{
      قطار->get_لفظ(0, value);
      قطار->pop_front();
}

سانچہ <قسم_نام ELEM, جماعت QTYPE>
ساکن منطقی qpop_f(vthread_t thr, vvp_رمز_t cp, غیرعلامتی wid=0)
{
      واپس q_نکالو<ELEM, QTYPE>(thr, cp, get_front_قدر<ELEM>, "front", wid);
}


/*
 * %qpop/f/real <var-label>
 */
منطقی کا_QPOP_F_REAL(vthread_t thr, vvp_رمز_t cp)
{
      واپس qpop_f<دوہرا, vvp_قطار_real>(thr, cp);
}

/*
 * %qpop/f/str <var-label>
 */
منطقی کا_QPOP_F_STR(vthread_t thr, vvp_رمز_t cp)
{
      واپس qpop_f<string, vvp_قطار_string>(thr, cp);
}

/*
 * %qpop/f/v <var-label>
 */
منطقی کا_QPOP_F_V(vthread_t thr, vvp_رمز_t cp)
{
      واپس qpop_f<vvp_vector4_t, vvp_قطار_vec4>(thr, cp, cp->ثنائی_ہندسہ_idx[0]);
}

/*
 * These implement the %release/net and %release/reg instructions. The
 * %release/net instruction applies to a net kind of functor by
 * sending the release/net command to the command port. (See vvp_net.h
 * for details.) The %release/reg instruction is the same, but sends
 * the release/reg command instead. These are very similar to the
 * %deassign instruction.
 */
ساکن منطقی do_release_vec(vvp_رمز_t cp, منطقی net_flag)
{
      vvp_net_t*net = cp->net;
      غیرعلامتی بنیاد  = cp->ثنائی_ہندسہ_idx[0];
      غیرعلامتی width = cp->ثنائی_ہندسہ_idx[1];

      أكد(net->fil);

      اگر (بنیاد >= net->fil->filter_سائز()) واپس سچ;
      اگر (بنیاد+width > net->fil->filter_سائز())
	    width = net->fil->filter_سائز() - بنیاد;

      منطقی بھرا_sig = بنیاد == 0 && width == net->fil->filter_سائز();

	// XXXX Can't really do this if this is a partial release?
      net->fil->force_unlink();

	/* Do we release all or part of the net? */
      vvp_net_ptr_t ptr (net, 0);
      اگر (بھرا_sig) {
	    net->fil->release(ptr, net_flag);
      } ورنہ {
	    net->fil->release_pv(ptr, بنیاد, width, net_flag);
      }
      net->fun->force_flag(جھوٹ);

      واپس سچ;
}

منطقی کا_RELEASE_NET(vthread_t, vvp_رمز_t cp)
{
      واپس do_release_vec(cp, سچ);
}


منطقی کا_RELEASE_REG(vthread_t, vvp_رمز_t cp)
{
      واپس do_release_vec(cp, جھوٹ);
}

/* The type is 1 for registers and 0 for everything else. */
منطقی کا_RELEASE_WR(vthread_t, vvp_رمز_t cp)
{
      vvp_net_t*net = cp->net;
      غیرعلامتی type  = cp->ثنائی_ہندسہ_idx[0];

      أكد(net->fil);
      net->fil->force_unlink();

	// Send a command to this signal to unforce itself.
      vvp_net_ptr_t ptr (net, 0);
      net->fil->release(ptr, type==0);
      واپس سچ;
}

منطقی کا_REPLICATE(vthread_t thr, vvp_رمز_t cp)
{
      صحیح rept = cp->نمبر;
      vvp_vector4_t val = thr->نکالو_vec4();
      vvp_vector4_t res (val.size() * rept, BIT4_X);

      برائے (صحیح idx = 0 ; idx < rept ; idx += 1) {
	    res.سیٹ_vec(idx * val.size(), val);
      }

      thr->ڈالو_vec4(res);

      واپس سچ;
}

ساکن خالی poke_val(vthread_t fun_thr, غیرعلامتی گہرائی, دوہرا val)
{
      fun_thr->والد->poke_real(گہرائی, val);
}

ساکن خالی poke_val(vthread_t fun_thr, غیرعلامتی گہرائی, ثابت string&val)
{
      fun_thr->والد->poke_str(گہرائی, val);
}

ساکن size_t get_max(vthread_t fun_thr, دوہرا&)
{
      واپس fun_thr->دلائل_real.size();
}

ساکن size_t get_max(vthread_t fun_thr, string&)
{
      واپس fun_thr->دلائل_str.size();
}

ساکن size_t get_max(vthread_t fun_thr, vvp_vector4_t&)
{
      واپس fun_thr->دلائل_vec4.size();
}

ساکن غیرعلامتی get_گہرائی(vthread_t fun_thr, size_t اشاریہ, دوہرا&)
{
      واپس fun_thr->دلائل_real[اشاریہ];
}

ساکن غیرعلامتی get_گہرائی(vthread_t fun_thr, size_t اشاریہ, string&)
{
      واپس fun_thr->دلائل_str[اشاریہ];
}

ساکن غیرعلامتی get_گہرائی(vthread_t fun_thr, size_t اشاریہ, vvp_vector4_t&)
{
      واپس fun_thr->دلائل_vec4[اشاریہ];
}

ساکن vthread_t get_func(vthread_t thr)
{
      vthread_t fun_thr = thr;

      جب (fun_thr->والد_scope->get_type_رمز() != vpiFunction) {
	    أكد(fun_thr->والد);
	    fun_thr = fun_thr->والد;
      }

      واپس fun_thr;
}

سانچہ <قسم_نام ELEM>
ساکن منطقی ret(vthread_t thr, vvp_رمز_t cp)
{
      size_t اشاریہ = cp->نمبر;
      ELEM val;
      نکالو_قدر(thr, val, 0);

      vthread_t fun_thr = get_func(thr);
      أكد(اشاریہ < get_max(fun_thr, val));

      غیرعلامتی گہرائی = get_گہرائی(fun_thr, اشاریہ, val);
	// Use the depth to put the value into the stack of
	// the parent thread.
      poke_val(fun_thr, گہرائی, val);
      واپس سچ;
}

/*
 * %ret/real <index>
 */
منطقی کا_RET_REAL(vthread_t thr, vvp_رمز_t cp)
{
      واپس ret<دوہرا>(thr, cp);
}

/*
 * %ret/str <index>
 */
منطقی کا_RET_STR(vthread_t thr, vvp_رمز_t cp)
{
      واپس ret<string>(thr, cp);
}

/*
 * %ret/vec4 <index>, <offset>, <wid>
 */
منطقی کا_RET_VEC4(vthread_t thr, vvp_رمز_t cp)
{
      size_t اشاریہ = cp->نمبر;
      غیرعلامتی off_اشاریہ = cp->ثنائی_ہندسہ_idx[0];
      غیرعلامتی صحیح wid = cp->ثنائی_ہندسہ_idx[1];
      vvp_vector4_t&val = thr->peek_vec4();

      vthread_t fun_thr = get_func(thr);
      أكد(اشاریہ < get_max(fun_thr, val));
      أكد(val.size() == wid);
      غیرعلامتی گہرائی = get_گہرائی(fun_thr, اشاریہ, val);

      int64_t off = off_اشاریہ ? thr->الفاظ[off_اشاریہ].w_int : 0;
      غیرعلامتی صحیح sig_قدر_سائز = fun_thr->والد->peek_vec4(گہرائی).size();

      اگر (off_اشاریہ!=0 && thr->flags[4] == BIT4_1) {
	    thr->نکالو_vec4(1);
	    واپس سچ;
      }

      اگر (!resize_rval_vec(val, off, sig_قدر_سائز)) {
	    thr->نکالو_vec4(1);
	    واپس سچ;
      }

      اگر (off == 0 && val.size() == sig_قدر_سائز) {
	    fun_thr->والد->poke_vec4(گہرائی, val);
      } ورنہ {
	    vvp_vector4_t tmp_dst = fun_thr->والد->peek_vec4(گہرائی);
	    tmp_dst.سیٹ_vec(off, val);
	    fun_thr->والد->poke_vec4(گہرائی, tmp_dst);
      }

      thr->نکالو_vec4(1);
      واپس سچ;
}

ساکن خالی ڈالو_from_والد(vthread_t thr, vthread_t fun_thr, غیرعلامتی گہرائی, دوہرا&)
{
      thr->ڈالو_real(fun_thr->والد->peek_real(گہرائی));
}

ساکن خالی ڈالو_from_والد(vthread_t thr, vthread_t fun_thr, غیرعلامتی گہرائی, string&)
{
      thr->ڈالو_str(fun_thr->والد->peek_str(گہرائی));
}

ساکن خالی ڈالو_from_والد(vthread_t thr, vthread_t fun_thr, غیرعلامتی گہرائی, vvp_vector4_t&)
{
      thr->ڈالو_vec4(fun_thr->والد->peek_vec4(گہرائی));
}

سانچہ <قسم_نام ELEM>
ساکن منطقی retload(vthread_t thr, vvp_رمز_t cp)
{
      size_t اشاریہ = cp->نمبر;
      ELEM type;

      vthread_t fun_thr = get_func(thr);
      أكد(اشاریہ < get_max(fun_thr, type));

      غیرعلامتی گہرائی = get_گہرائی(fun_thr, اشاریہ, type);
	// Use the depth to extract the values from the stack
	// of the parent thread.
      ڈالو_from_والد(thr, fun_thr, گہرائی, type);
      واپس سچ;
}

/*
 * %retload/real <index>
 */
منطقی کا_RETLOAD_REAL(vthread_t thr, vvp_رمز_t cp)
{
      واپس retload<دوہرا>(thr, cp);
}

/*
 * %retload/str <index>
 */
منطقی کا_RETLOAD_STR(vthread_t thr, vvp_رمز_t cp)
{
      واپس retload<string>(thr, cp);
}

/*
 * %retload/vec4 <index>
 */
منطقی کا_RETLOAD_VEC4(vthread_t thr, vvp_رمز_t cp)
{
      واپس retload<vvp_vector4_t>(thr, cp);
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
منطقی کا_SCOPY(vthread_t thr, vvp_رمز_t)
{
      vvp_object_t tmp;
      thr->نکالو_object(tmp);

      vvp_object_t&dest = thr->peek_object();
        // If it is null there is nothing to copy
      اگر (!tmp.test_nil())
	    dest.shallow_copy(tmp);

      واپس سچ;
}

ساکن خالی thread_peek(vthread_t thr, دوہرا&value)
{
      value = thr->peek_real(0);
}

ساکن خالی thread_peek(vthread_t thr, string&value)
{
      value = thr->peek_str(0);
}

ساکن خالی thread_peek(vthread_t thr, vvp_vector4_t&value)
{
      value = thr->peek_vec4(0);
}

سانچہ <قسم_نام ELEM>
ساکن منطقی سیٹ_dar_obj(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی adr = thr->الفاظ[cp->نمبر].w_int;

      ELEM value;
      thread_peek(thr, value);

      vvp_object_t&چوٹی = thr->peek_object();
      vvp_darray*darray = چوٹی.peek<vvp_darray>();
      أكد(darray);

      darray->سیٹ_لفظ(adr, value);
      واپس سچ;
}

/*
 * %set/dar/obj/real <index>
 */
منطقی کا_سیٹ_DAR_OBJ_REAL(vthread_t thr, vvp_رمز_t cp)
{
      واپس سیٹ_dar_obj<دوہرا>(thr, cp);
}

/*
 * %set/dar/obj/str <index>
 */
منطقی کا_سیٹ_DAR_OBJ_STR(vthread_t thr, vvp_رمز_t cp)
{
      واپس سیٹ_dar_obj<string>(thr, cp);
}

/*
 * %set/dar/obj/vec4 <index>
 */
منطقی کا_سیٹ_DAR_OBJ_VEC4(vthread_t thr, vvp_رمز_t cp)
{
      واپس سیٹ_dar_obj<vvp_vector4_t>(thr, cp);
}

/*
 * %shiftl <idx>
 *
 * Pop the operand, then push the result.
 */
منطقی کا_SHIFTL(vthread_t thr, vvp_رمز_t cp)
{
      صحیح use_اشاریہ = cp->نمبر;
      uint64_t سرکاؤ = thr->الفاظ[use_اشاریہ].w_uint;

      vvp_vector4_t&val = thr->peek_vec4();
      غیرعلامتی wid  = val.size();

      اگر (thr->flags[4] == BIT4_1) {
	      // The result is 'bx if the shift amount is undefined
	    val = vvp_vector4_t(wid, BIT4_X);

      } ورنہ اگر (thr->flags[4] == BIT4_X || سرکاؤ >= wid) {
	      // Shift is so big that all value is shifted out. Write
	      // a constant 0 result.
	    val = vvp_vector4_t(wid, BIT4_0);

      } ورنہ اگر (سرکاؤ > 0) {
	    vvp_vector4_t blk = val.subvalue(0, wid-سرکاؤ);
	    vvp_vector4_t tmp (سرکاؤ, BIT4_0);
	    val.سیٹ_vec(0, tmp);
	    val.سیٹ_vec(سرکاؤ, blk);
      }

      واپس سچ;
}

/*
 * %shiftr <idx>
 * This is an unsigned right shift. The <idx> is a number that selects
 * the index register with the amount of the shift. This instruction
 * checks flag bit 4, which will be true if the shift is invalid.
 */
منطقی کا_SHIFTR(vthread_t thr, vvp_رمز_t cp)
{
      صحیح use_اشاریہ = cp->نمبر;
      uint64_t سرکاؤ = thr->الفاظ[use_اشاریہ].w_uint;

      vvp_vector4_t val = thr->نکالو_vec4();
      غیرعلامتی wid  = val.size();

      اگر (thr->flags[4] == BIT4_1) {
	    val = vvp_vector4_t(wid, BIT4_X);

      } ورنہ اگر (thr->flags[4] == BIT4_X || سرکاؤ > wid) {
	    val = vvp_vector4_t(wid, BIT4_0);

      } ورنہ اگر (سرکاؤ > 0) {
	    vvp_vector4_t blk = val.subvalue(سرکاؤ, wid-سرکاؤ);
	    vvp_vector4_t tmp (سرکاؤ, BIT4_0);
	    val.سیٹ_vec(0, blk);
	    val.سیٹ_vec(wid-سرکاؤ, tmp);
      }

      thr->ڈالو_vec4(val);
      واپس سچ;
}

/*
 *  %shiftr/s <wid>
 */
منطقی کا_SHIFTR_S(vthread_t thr, vvp_رمز_t cp)
{
      صحیح use_اشاریہ = cp->نمبر;
      uint64_t سرکاؤ = thr->الفاظ[use_اشاریہ].w_uint;

      vvp_vector4_t val = thr->نکالو_vec4();
      غیرعلامتی wid  = val.size();

      vvp_bit4_t sign_ثنائی_ہندسہ = val.value(val.size()-1);

      اگر (thr->flags[4] == BIT4_1) {
	    val = vvp_vector4_t(wid, BIT4_X);

      } ورنہ اگر (thr->flags[4] == BIT4_X || سرکاؤ > wid) {
	    val = vvp_vector4_t(wid, sign_ثنائی_ہندسہ);

      } ورنہ اگر (سرکاؤ > 0) {
	    vvp_vector4_t blk = val.subvalue(سرکاؤ, wid-سرکاؤ);
	    vvp_vector4_t tmp (سرکاؤ, sign_ثنائی_ہندسہ);
	    val.سیٹ_vec(0, blk);
	    val.سیٹ_vec(wid-سرکاؤ, tmp);
      }

      thr->ڈالو_vec4(val);
      واپس سچ;
}

/*
 * %split/vec4 <wid>
 *   Pop 1 value,
 *   Take <wid> bits from the lsb,
 *   Push the remaining msb,
 *   Push the lsb.
 */
منطقی کا_بانٹو_VEC4(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی lsb_wid = cp->نمبر;

      vvp_vector4_t&val = thr->peek_vec4();
      أكد(lsb_wid < val.size());

      vvp_vector4_t lsb = val.subvalue(0, lsb_wid);
      val = val.subvalue(lsb_wid, val.size()-lsb_wid);

      thr->ڈالو_vec4(lsb);
      واپس سچ;
}

/*
 * The following are used to allow the darray templates to print correctly.
 */
اندرونی ساکن string get_darray_type(دوہرا&)
{
      واپس "darray<real>";
}

اندرونی ساکن string get_darray_type(string&)
{
      واپس "darray<string>";
}

اندرونی ساکن string get_darray_type(ثابت vvp_vector4_t&value)
{
      ostringstream buf;
      buf << "darray<vector[" << value.size() << "]>";
      string res = buf.str();
      واپس res;
}

/*
 * The following are used to allow a common template to be written for
 * darray real/string/vec4 operations
 */
اندرونی ساکن خالی dar_نکالو_قدر(vthread_t thr, دوہرا&value)
{
      value = thr->نکالو_real();
}

اندرونی ساکن خالی dar_نکالو_قدر(vthread_t thr, string&value)
{
      value = thr->نکالو_str();
}

اندرونی ساکن خالی dar_نکالو_قدر(vthread_t thr, vvp_vector4_t&value)
{
      value = thr->نکالو_vec4();
}

سانچہ <قسم_نام ELEM>
ساکن منطقی store_dar(vthread_t thr, vvp_رمز_t cp)
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

      اگر (adr < 0)
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to a negative " << get_darray_type(value)
	         << " index (" << adr << ")." << سطر_ختم;
      ورنہ اگر (thr->flags[4] != BIT4_0)
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to an undefined " << get_darray_type(value)
	         << " index." << سطر_ختم;
      ورنہ اگر (darray)
	    darray->سیٹ_لفظ(adr, value);
      ورنہ
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to an undefined " << get_darray_type(value)
	         << "." << سطر_ختم;

      واپس سچ;
}

/*
 * %store/dar/real <var>
 */
منطقی کا_STORE_DAR_R(vthread_t thr, vvp_رمز_t cp)
{
      واپس store_dar<دوہرا>(thr, cp);
}

/*
 * %store/dar/str <var>
 */
منطقی کا_STORE_DAR_STR(vthread_t thr, vvp_رمز_t cp)
{
      واپس store_dar<string>(thr, cp);
}

/*
 * %store/dar/vec4 <var>
 */
منطقی کا_STORE_DAR_VEC4(vthread_t thr, vvp_رمز_t cp)
{
      واپس store_dar<vvp_vector4_t>(thr, cp);
}

منطقی کا_STORE_OBJ(vthread_t thr, vvp_رمز_t cp)
{
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);

      vvp_object_t val;
      thr->نکالو_object(val);

      vvp_send_object(ptr, val, thr->wt_context);

      واپس سچ;
}

/*
 * %store/obja <array-label> <index>
 */
منطقی کا_STORE_OBJA(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی idx = cp->ثنائی_ہندسہ_idx[0];
      غیرعلامتی adr = thr->الفاظ[idx].w_int;

      vvp_object_t val;
      thr->نکالو_object(val);

      cp->array->سیٹ_لفظ(adr, val);

      واپس سچ;
}


/*
 * %store/prop/obj <pid>, <idx>
 *
 * Pop an object value from the object stack, and store the value into
 * the property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
منطقی کا_STORE_PROP_OBJ(vthread_t thr, vvp_رمز_t cp)
{
      size_t pid = cp->نمبر;
      غیرعلامتی idx = cp->ثنائی_ہندسہ_idx[0];

      اگر (idx != 0) {
	    أكد(idx < vthread_s::الفاظ_گنتی);
	    idx = thr->الفاظ[idx].w_uint;
      }

      vvp_object_t val;
      thr->نکالو_object(val);

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      أكد(cobj);

      cobj->سیٹ_object(pid, val, idx);

      واپس سچ;
}

ساکن خالی نکالو_prop_val(vthread_t thr, دوہرا&val, غیرعلامتی)
{
      val = thr->نکالو_real();
}

ساکن خالی نکالو_prop_val(vthread_t thr, string&val, غیرعلامتی)
{
      val = thr->نکالو_str();
}

ساکن خالی نکالو_prop_val(vthread_t thr, vvp_vector4_t&val, غیرعلامتی wid)
{
      val = thr->نکالو_vec4();
      أكد(val.size() >= wid);
      val.resize(wid);
}

ساکن خالی سیٹ_val(vvp_cobject*cobj, size_t pid, ثابت دوہرا&val)
{
      cobj->سیٹ_real(pid, val);
}

ساکن خالی سیٹ_val(vvp_cobject*cobj, size_t pid, ثابت string&val)
{
      cobj->سیٹ_string(pid, val);
}

ساکن خالی سیٹ_val(vvp_cobject*cobj, size_t pid, ثابت vvp_vector4_t&val)
{
      cobj->سیٹ_vec4(pid, val);
}

سانچہ <قسم_نام ELEM>
ساکن منطقی store_prop(vthread_t thr, vvp_رمز_t cp, غیرعلامتی wid=0)
{
      size_t pid = cp->نمبر;
      ELEM val;
      نکالو_prop_val(thr, val, wid); // Pop the value to store.

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      أكد(cobj);

      سیٹ_val(cobj, pid, val);

      واپس سچ;
}

/*
 * %store/prop/r <id>
 *
 * Pop a real value from the real stack, and store the value into the
 * property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
منطقی کا_STORE_PROP_R(vthread_t thr, vvp_رمز_t cp)
{
      واپس store_prop<دوہرا>(thr, cp);
}

/*
 * %store/prop/str <id>
 *
 * Pop a string value from the string stack, and store the value into
 * the property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
منطقی کا_STORE_PROP_STR(vthread_t thr, vvp_رمز_t cp)
{
      واپس store_prop<string>(thr, cp);
}

/*
 * %store/prop/v <pid>, <wid>
 *
 * Store vector value into property <id> of cobject in the top of the
 * stack. Do NOT pop the object stack.
 */
منطقی کا_STORE_PROP_V(vthread_t thr, vvp_رمز_t cp)
{
      واپس store_prop<vvp_vector4_t>(thr, cp, cp->ثنائی_ہندسہ_idx[0]);
}

سانچہ <جماعت QTYPE>
ساکن QTYPE* get_قطار_prop(vvp_cobject*cobj, size_t pid)
{
      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      QTYPE* قطار = qobj.peek<QTYPE>();
      اگر (قطار == 0) {
	    قطار = نیا QTYPE;
	    vvp_object_t val(قطار);
	    cobj->سیٹ_object(pid, val, 0);
      }
      واپس قطار;
}

سانچہ <قسم_نام ELEM, جماعت QTYPE>
ساکن منطقی store_prop_qb(vthread_t thr, vvp_رمز_t cp, غیرعلامتی wid)
{
      size_t pid = cp->نمبر;
      ELEM value;
      غیرعلامتی max_سائز = thr->الفاظ[cp->ثنائی_ہندسہ_idx[0]].w_uint;
      نکالو_قدر(thr, value, wid);

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      QTYPE* قطار = get_قطار_prop<QTYPE>(cobj, pid);
      أكد(قطار);
      قطار->push_back(value, max_سائز);
      واپس سچ;
}

سانچہ <قسم_نام ELEM, جماعت QTYPE>
ساکن منطقی store_prop_qf(vthread_t thr, vvp_رمز_t cp, غیرعلامتی wid)
{
      size_t pid = cp->نمبر;
      ELEM value;
      غیرعلامتی max_سائز = thr->الفاظ[cp->ثنائی_ہندسہ_idx[0]].w_uint;
      نکالو_قدر(thr, value, wid);

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      QTYPE* قطار = get_قطار_prop<QTYPE>(cobj, pid);
      أكد(قطار);
      قطار->push_front(value, max_سائز);
      واپس سچ;
}

منطقی کا_STORE_PROP_QB_R(vthread_t thr, vvp_رمز_t cp)
{
      واپس store_prop_qb<دوہرا, vvp_قطار_real>(thr, cp, 0);
}

منطقی کا_STORE_PROP_QB_STR(vthread_t thr, vvp_رمز_t cp)
{
      واپس store_prop_qb<string, vvp_قطار_string>(thr, cp, 0);
}

منطقی کا_STORE_PROP_QB_V(vthread_t thr, vvp_رمز_t cp)
{
      واپس store_prop_qb<vvp_vector4_t, vvp_قطار_vec4>(thr, cp, cp->ثنائی_ہندسہ_idx[1]);
}

منطقی کا_STORE_PROP_QF_R(vthread_t thr, vvp_رمز_t cp)
{
      واپس store_prop_qf<دوہرا, vvp_قطار_real>(thr, cp, 0);
}

منطقی کا_STORE_PROP_QF_STR(vthread_t thr, vvp_رمز_t cp)
{
      واپس store_prop_qf<string, vvp_قطار_string>(thr, cp, 0);
}

منطقی کا_STORE_PROP_QF_V(vthread_t thr, vvp_رمز_t cp)
{
      واپس store_prop_qf<vvp_vector4_t, vvp_قطار_vec4>(thr, cp, cp->ثنائی_ہندسہ_idx[1]);
}

سانچہ <قسم_نام ELEM, جماعت QTYPE>
ساکن منطقی qinsert_prop(vthread_t thr, vvp_رمز_t cp, غیرعلامتی wid)
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
      اگر (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at a negative "
	         << get_قطار_type(value)
	         << " index (" << idx << "). ";
	    print_قطار_قدر(value);
	    cerr << " was not added." << سطر_ختم;
      } ورنہ اگر (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at an undefined "
	         << get_قطار_type(value) << " index. ";
	    print_قطار_قدر(value);
	    cerr << " was not added." << سطر_ختم;
      } ورنہ {
	    غیرعلامتی max_سائز = thr->الفاظ[cp->ثنائی_ہندسہ_idx[0]].w_int;
	    قطار->insert(idx, value, max_سائز);
      }
      واپس سچ;
}

منطقی کا_QINSERT_PROP_R(vthread_t thr, vvp_رمز_t cp)
{
      واپس qinsert_prop<دوہرا, vvp_قطار_real>(thr, cp, 0);
}

منطقی کا_QINSERT_PROP_STR(vthread_t thr, vvp_رمز_t cp)
{
      واپس qinsert_prop<string, vvp_قطار_string>(thr, cp, 0);
}

منطقی کا_QINSERT_PROP_V(vthread_t thr, vvp_رمز_t cp)
{
      واپس qinsert_prop<vvp_vector4_t, vvp_قطار_vec4>(thr, cp, cp->ثنائی_ہندسہ_idx[1]);
}

سانچہ <قسم_نام ELEM, جماعت QTYPE>
ساکن منطقی q_نکالو_prop(vthread_t thr, vvp_رمز_t cp,
                       خالی (*get_val_func)(vvp_قطار*, ELEM&),
                       ثابت حرف*loc, غیرعلامتی wid)
{
      size_t pid = cp->نمبر;

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      QTYPE* قطار = get_قطار_prop<QTYPE>(cobj, pid);
      أكد(قطار);

      size_t size = قطار->get_سائز();

      ELEM value;
      اگر (size) {
	    get_val_func(قطار, value);
      } ورنہ {
	    dq_default(value, wid);
	    cerr << thr->get_fileline()
	         << "Warning: pop_" << loc << "() on empty "
	         << get_قطار_type(value) << "." << سطر_ختم;
      }

      ڈالو_قدر(thr, value, wid);
      واپس سچ;
}

سانچہ <قسم_نام ELEM, جماعت QTYPE>
ساکن منطقی qpop_b_prop(vthread_t thr, vvp_رمز_t cp, غیرعلامتی wid)
{
      واپس q_نکالو_prop<ELEM, QTYPE>(thr, cp, get_عقب_قدر<ELEM>, "back", wid);
}

سانچہ <قسم_نام ELEM, جماعت QTYPE>
ساکن منطقی qpop_f_prop(vthread_t thr, vvp_رمز_t cp, غیرعلامتی wid)
{
      واپس q_نکالو_prop<ELEM, QTYPE>(thr, cp, get_front_قدر<ELEM>, "front", wid);
}

منطقی کا_QPOP_PROP_B_REAL(vthread_t thr, vvp_رمز_t cp)
{
      واپس qpop_b_prop<دوہرا, vvp_قطار_real>(thr, cp, 0);
}

منطقی کا_QPOP_PROP_B_STR(vthread_t thr, vvp_رمز_t cp)
{
      واپس qpop_b_prop<string, vvp_قطار_string>(thr, cp, 0);
}

منطقی کا_QPOP_PROP_B_V(vthread_t thr, vvp_رمز_t cp)
{
      واپس qpop_b_prop<vvp_vector4_t, vvp_قطار_vec4>(thr, cp, cp->ثنائی_ہندسہ_idx[0]);
}

منطقی کا_QPOP_PROP_F_REAL(vthread_t thr, vvp_رمز_t cp)
{
      واپس qpop_f_prop<دوہرا, vvp_قطار_real>(thr, cp, 0);
}

منطقی کا_QPOP_PROP_F_STR(vthread_t thr, vvp_رمز_t cp)
{
      واپس qpop_f_prop<string, vvp_قطار_string>(thr, cp, 0);
}

منطقی کا_QPOP_PROP_F_V(vthread_t thr, vvp_رمز_t cp)
{
      واپس qpop_f_prop<vvp_vector4_t, vvp_قطار_vec4>(thr, cp, cp->ثنائی_ہندسہ_idx[0]);
}

ساکن vvp_vector4_t قطار_unique_ulong_مقصد_vec4(غیرعلامتی طویل ul, غیرعلامتی بٹ)
{
      vvp_vector4_t val(بٹ, BIT4_0);
      برائے (غیرعلامتی idx = 0; idx < بٹ && idx < 8 * حجم(غیرعلامتی طویل); idx += 1) {
	    اگر (ul & 1UL) {
		  val.سیٹ_ثنائی_ہندسہ(idx, BIT4_1);
	    } ورنہ {
		  val.سیٹ_ثنائی_ہندسہ(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      واپس val;
}

سانچہ <جماعت SRC> ساکن vvp_قطار_vec4* قطار_تسلسل_unique_src(SRC* src,
								 غیرعلامتی wid,
								 منطقی as_اشاریہ)
{
      vvp_قطار_vec4* dst = نیا vvp_قطار_vec4();
      اگر (!src) واپس dst;
      size_t n = src->get_سائز();
      برائے (size_t i = 0; i < n; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_لفظ(i, vi);
	    منطقی نظر_آیا = جھوٹ;
	    برائے (size_t j = 0; j < i; j += 1) {
		  vvp_vector4_t vj(wid);
		  src->get_لفظ(j, vj);
		  اگر (vi.eeq(vj)) {
			نظر_آیا = سچ;
			توڑو;
		  }
	    }
	    اگر (نظر_آیا) جاری;
	    اگر (as_اشاریہ) {
		  dst->push_back(قطار_unique_ulong_مقصد_vec4((غیرعلامتی طویل)i, 32), 0);
	    } ورنہ {
		  dst->push_back(vi, 0);
	    }
      }
      واپس dst;
}

سانچہ <جماعت SRC>
ساکن vvp_قطار_vec4* قطار_تسلسل_ڈھونڈو_matches_src(SRC* src, غیرعلامتی wid,
						  ثابت vvp_vector4_t& cmp,
						  منطقی as_اشاریہ)
{
      vvp_قطار_vec4* dst = نیا vvp_قطار_vec4();
      اگر (!src) واپس dst;
      size_t n = src->get_سائز();
      برائے (size_t i = 0; i < n; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_لفظ(i, vi);
	    اگر (vi.eeq(cmp)) {
		  اگر (as_اشاریہ) {
			dst->push_back(قطار_unique_ulong_مقصد_vec4((غیرعلامتی طویل)i, 32), 0);
		  } ورنہ {
			dst->push_back(vi, 0);
		  }
	    }
      }
      واپس dst;
}

/* Array/queue locator methods return a queue with 0 or 1 elements (LRM). */
سانچہ <جماعت SRC>
ساکن vvp_قطار_vec4* قطار_ڈھونڈو_پہلا_آخری_مقصد_قطار_src(SRC* src,
							  غیرعلامتی wid,
							  ثابت vvp_vector4_t& cmp,
							  منطقی want_پہلا,
							  منطقی want_اشاریہ)
{
      vvp_قطار_vec4* dst = نیا vvp_قطار_vec4();
      اگر (!src || src->get_سائز() == 0) واپس dst;
      size_t n = src->get_سائز();
      اگر (want_پہلا) {
	    برائے (size_t i = 0; i < n; i += 1) {
		  vvp_vector4_t vi(wid);
		  src->get_لفظ(i, vi);
		  اگر (vi.eeq(cmp)) {
			اگر (want_اشاریہ) {
			      dst->push_back(
				  قطار_unique_ulong_مقصد_vec4((غیرعلامتی طویل)i, 32),
				  0);
			} ورنہ {
			      dst->push_back(vi, 0);
			}
			واپس dst;
		  }
	    }
      } ورنہ {
	    برائے (size_t i = n; i > 0; ) {
		  i -= 1;
		  vvp_vector4_t vi(wid);
		  src->get_لفظ(i, vi);
		  اگر (vi.eeq(cmp)) {
			اگر (want_اشاریہ) {
			      dst->push_back(
				  قطار_unique_ulong_مقصد_vec4((غیرعلامتی طویل)i, 32),
				  0);
			} ورنہ {
			      dst->push_back(vi, 0);
			}
			واپس dst;
		  }
	    }
      }
      واپس dst;
}

/* min()/max() return one element with the selected extremum. */
سانچہ <جماعت SRC>
ساکن vvp_قطار_vec4* قطار_تسلسل_min_max_src(SRC* src, غیرعلامتی wid, منطقی want_max)
{
      vvp_قطار_vec4* dst = نیا vvp_قطار_vec4();
      اگر (!src || src->get_سائز() == 0) واپس dst;

      vvp_vector4_t بہترین(wid);
      src->get_لفظ(0, بہترین);

      size_t n = src->get_سائز();
      برائے (size_t i = 1 ; i < n ; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_لفظ(i, vi);

	    vvp_bit4_t ge = compare_gtge(vi, بہترین, BIT4_1);
	    منطقی replace = want_max ? (ge == BIT4_1) : (ge == BIT4_0);
	    اگر (replace) بہترین = vi;
      }

      dst->push_back(بہترین, 0);
      واپس dst;
}

منطقی کا_قطار_ڈھونڈو_V(vthread_t thr, vvp_رمز_t cp)
{
      vvp_vector4_t cmp = thr->نکالو_vec4();
      غیرعلامتی wid = cp->ثنائی_ہندسہ_idx[0];
      أكد(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_قطار_vec4* dst;
      اگر (qsrc) dst = قطار_تسلسل_ڈھونڈو_matches_src(qsrc, wid, cmp, جھوٹ);
      ورنہ اگر (dsrc) dst = قطار_تسلسل_ڈھونڈو_matches_src(dsrc, wid, cmp, جھوٹ);
      ورنہ {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    dst = قطار_تسلسل_ڈھونڈو_matches_src(src, wid, cmp, جھوٹ);
      }
      thr->ڈالو_object(vvp_object_t(dst));
      واپس سچ;
}

منطقی کا_قطار_ڈھونڈو_PROP_V(vthread_t thr, vvp_رمز_t cp)
{
      vvp_vector4_t cmp = thr->نکالو_vec4();
      size_t pid = cp->نمبر;
      غیرعلامتی wid = cp->ثنائی_ہندسہ_idx[0];
      أكد(cmp.size() == wid);

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_قطار_vec4* dst = qsrc ? قطار_تسلسل_ڈھونڈو_matches_src(qsrc, wid, cmp, جھوٹ)
				 : قطار_تسلسل_ڈھونڈو_matches_src(dsrc, wid, cmp, جھوٹ);
      thr->ڈالو_object(vvp_object_t(dst));
      واپس سچ;
}

منطقی کا_قطار_ڈھونڈو_اشاریہ_V(vthread_t thr, vvp_رمز_t cp)
{
      vvp_vector4_t cmp = thr->نکالو_vec4();
      غیرعلامتی wid = cp->ثنائی_ہندسہ_idx[0];
      أكد(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_قطار_vec4* dst;
      اگر (qsrc) dst = قطار_تسلسل_ڈھونڈو_matches_src(qsrc, wid, cmp, سچ);
      ورنہ اگر (dsrc) dst = قطار_تسلسل_ڈھونڈو_matches_src(dsrc, wid, cmp, سچ);
      ورنہ {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    dst = قطار_تسلسل_ڈھونڈو_matches_src(src, wid, cmp, سچ);
      }
      thr->ڈالو_object(vvp_object_t(dst));
      واپس سچ;
}

منطقی کا_قطار_ڈھونڈو_اشاریہ_PROP_V(vthread_t thr, vvp_رمز_t cp)
{
      vvp_vector4_t cmp = thr->نکالو_vec4();
      size_t pid = cp->نمبر;
      غیرعلامتی wid = cp->ثنائی_ہندسہ_idx[0];
      أكد(cmp.size() == wid);

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_قطار_vec4* dst = qsrc ? قطار_تسلسل_ڈھونڈو_matches_src(qsrc, wid, cmp, سچ)
				 : قطار_تسلسل_ڈھونڈو_matches_src(dsrc, wid, cmp, سچ);
      thr->ڈالو_object(vvp_object_t(dst));
      واپس سچ;
}

منطقی کا_قطار_ڈھونڈو_پہلا_V(vthread_t thr, vvp_رمز_t cp)
{
      vvp_vector4_t cmp = thr->نکالو_vec4();
      غیرعلامتی wid = cp->ثنائی_ہندسہ_idx[0];
      أكد(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_قطار_vec4* dst;
      اگر (qsrc) dst = قطار_ڈھونڈو_پہلا_آخری_مقصد_قطار_src(qsrc, wid, cmp, سچ, جھوٹ);
      ورنہ اگر (dsrc) dst = قطار_ڈھونڈو_پہلا_آخری_مقصد_قطار_src(dsrc, wid, cmp, سچ, جھوٹ);
      ورنہ {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    dst = قطار_ڈھونڈو_پہلا_آخری_مقصد_قطار_src(src, wid, cmp, سچ, جھوٹ);
      }
      thr->ڈالو_object(vvp_object_t(dst));
      واپس سچ;
}

منطقی کا_قطار_ڈھونڈو_پہلا_PROP_V(vthread_t thr, vvp_رمز_t cp)
{
      vvp_vector4_t cmp = thr->نکالو_vec4();
      size_t pid = cp->نمبر;
      غیرعلامتی wid = cp->ثنائی_ہندسہ_idx[0];
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
	    qsrc ? قطار_ڈھونڈو_پہلا_آخری_مقصد_قطار_src(qsrc, wid, cmp, سچ, جھوٹ)
		 : قطار_ڈھونڈو_پہلا_آخری_مقصد_قطار_src(dsrc, wid, cmp, سچ, جھوٹ);
      thr->ڈالو_object(vvp_object_t(dst));
      واپس سچ;
}

منطقی کا_قطار_ڈھونڈو_پہلا_اشاریہ_V(vthread_t thr, vvp_رمز_t cp)
{
      vvp_vector4_t cmp = thr->نکالو_vec4();
      غیرعلامتی wid = cp->ثنائی_ہندسہ_idx[0];
      أكد(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_قطار_vec4* dst;
      اگر (qsrc) dst = قطار_ڈھونڈو_پہلا_آخری_مقصد_قطار_src(qsrc, wid, cmp, سچ, سچ);
      ورنہ اگر (dsrc) dst = قطار_ڈھونڈو_پہلا_آخری_مقصد_قطار_src(dsrc, wid, cmp, سچ, سچ);
      ورنہ {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    dst = قطار_ڈھونڈو_پہلا_آخری_مقصد_قطار_src(src, wid, cmp, سچ, سچ);
      }
      thr->ڈالو_object(vvp_object_t(dst));
      واپس سچ;
}

منطقی کا_قطار_ڈھونڈو_پہلا_اشاریہ_PROP_V(vthread_t thr, vvp_رمز_t cp)
{
      vvp_vector4_t cmp = thr->نکالو_vec4();
      size_t pid = cp->نمبر;
      غیرعلامتی wid = cp->ثنائی_ہندسہ_idx[0];
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
	    qsrc ? قطار_ڈھونڈو_پہلا_آخری_مقصد_قطار_src(qsrc, wid, cmp, سچ, سچ)
		 : قطار_ڈھونڈو_پہلا_آخری_مقصد_قطار_src(dsrc, wid, cmp, سچ, سچ);
      thr->ڈالو_object(vvp_object_t(dst));
      واپس سچ;
}

منطقی کا_قطار_ڈھونڈو_آخری_V(vthread_t thr, vvp_رمز_t cp)
{
      vvp_vector4_t cmp = thr->نکالو_vec4();
      غیرعلامتی wid = cp->ثنائی_ہندسہ_idx[0];
      أكد(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_قطار_vec4* dst;
      اگر (qsrc) dst = قطار_ڈھونڈو_پہلا_آخری_مقصد_قطار_src(qsrc, wid, cmp, جھوٹ, جھوٹ);
      ورنہ اگر (dsrc) dst = قطار_ڈھونڈو_پہلا_آخری_مقصد_قطار_src(dsrc, wid, cmp, جھوٹ, جھوٹ);
      ورنہ {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    dst = قطار_ڈھونڈو_پہلا_آخری_مقصد_قطار_src(src, wid, cmp, جھوٹ, جھوٹ);
      }
      thr->ڈالو_object(vvp_object_t(dst));
      واپس سچ;
}

منطقی کا_قطار_ڈھونڈو_آخری_PROP_V(vthread_t thr, vvp_رمز_t cp)
{
      vvp_vector4_t cmp = thr->نکالو_vec4();
      size_t pid = cp->نمبر;
      غیرعلامتی wid = cp->ثنائی_ہندسہ_idx[0];
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
	    qsrc ? قطار_ڈھونڈو_پہلا_آخری_مقصد_قطار_src(qsrc, wid, cmp, جھوٹ, جھوٹ)
		 : قطار_ڈھونڈو_پہلا_آخری_مقصد_قطار_src(dsrc, wid, cmp, جھوٹ, جھوٹ);
      thr->ڈالو_object(vvp_object_t(dst));
      واپس سچ;
}

منطقی کا_قطار_ڈھونڈو_آخری_اشاریہ_V(vthread_t thr, vvp_رمز_t cp)
{
      vvp_vector4_t cmp = thr->نکالو_vec4();
      غیرعلامتی wid = cp->ثنائی_ہندسہ_idx[0];
      أكد(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_قطار_vec4* dst;
      اگر (qsrc) dst = قطار_ڈھونڈو_پہلا_آخری_مقصد_قطار_src(qsrc, wid, cmp, جھوٹ, سچ);
      ورنہ اگر (dsrc) dst = قطار_ڈھونڈو_پہلا_آخری_مقصد_قطار_src(dsrc, wid, cmp, جھوٹ, سچ);
      ورنہ {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    dst = قطار_ڈھونڈو_پہلا_آخری_مقصد_قطار_src(src, wid, cmp, جھوٹ, سچ);
      }
      thr->ڈالو_object(vvp_object_t(dst));
      واپس سچ;
}

منطقی کا_قطار_ڈھونڈو_آخری_اشاریہ_PROP_V(vthread_t thr, vvp_رمز_t cp)
{
      vvp_vector4_t cmp = thr->نکالو_vec4();
      size_t pid = cp->نمبر;
      غیرعلامتی wid = cp->ثنائی_ہندسہ_idx[0];
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
	    qsrc ? قطار_ڈھونڈو_پہلا_آخری_مقصد_قطار_src(qsrc, wid, cmp, جھوٹ, سچ)
		 : قطار_ڈھونڈو_پہلا_آخری_مقصد_قطار_src(dsrc, wid, cmp, جھوٹ, سچ);
      thr->ڈالو_object(vvp_object_t(dst));
      واپس سچ;
}

منطقی کا_قطار_MIN_V(vthread_t thr, vvp_رمز_t cp)
{
      vvp_net_t* net = cp->net;
      غیرعلامتی wid = cp->ثنائی_ہندسہ_idx[0];
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_قطار_vec4* dst;
      اگر (qsrc) dst = قطار_تسلسل_min_max_src(qsrc, wid, جھوٹ);
      ورنہ اگر (dsrc) dst = قطار_تسلسل_min_max_src(dsrc, wid, جھوٹ);
      ورنہ {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    dst = قطار_تسلسل_min_max_src(src, wid, جھوٹ);
      }
      thr->ڈالو_object(vvp_object_t(dst));
      واپس سچ;
}

منطقی کا_قطار_MIN_OBJ_V(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی wid = cp->ثنائی_ہندسہ_idx[0];
      vvp_object_t src_obj;
      thr->نکالو_object(src_obj);

      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_قطار_vec4* dst = qsrc ? قطار_تسلسل_min_max_src(qsrc, wid, جھوٹ)
				 : قطار_تسلسل_min_max_src(dsrc, wid, جھوٹ);
      thr->ڈالو_object(vvp_object_t(dst));
      واپس سچ;
}

منطقی کا_قطار_MIN_PROP_V(vthread_t thr, vvp_رمز_t cp)
{
      size_t pid = cp->نمبر;
      غیرعلامتی wid = cp->ثنائی_ہندسہ_idx[0];

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_قطار_vec4* dst = qsrc ? قطار_تسلسل_min_max_src(qsrc, wid, جھوٹ)
				 : قطار_تسلسل_min_max_src(dsrc, wid, جھوٹ);
      thr->ڈالو_object(vvp_object_t(dst));
      واپس سچ;
}

منطقی کا_قطار_MAX_V(vthread_t thr, vvp_رمز_t cp)
{
      vvp_net_t* net = cp->net;
      غیرعلامتی wid = cp->ثنائی_ہندسہ_idx[0];
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_قطار_vec4* dst;
      اگر (qsrc) dst = قطار_تسلسل_min_max_src(qsrc, wid, سچ);
      ورنہ اگر (dsrc) dst = قطار_تسلسل_min_max_src(dsrc, wid, سچ);
      ورنہ {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    dst = قطار_تسلسل_min_max_src(src, wid, سچ);
      }
      thr->ڈالو_object(vvp_object_t(dst));
      واپس سچ;
}

منطقی کا_قطار_MAX_OBJ_V(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی wid = cp->ثنائی_ہندسہ_idx[0];
      vvp_object_t src_obj;
      thr->نکالو_object(src_obj);

      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_قطار_vec4* dst = qsrc ? قطار_تسلسل_min_max_src(qsrc, wid, سچ)
				 : قطار_تسلسل_min_max_src(dsrc, wid, سچ);
      thr->ڈالو_object(vvp_object_t(dst));
      واپس سچ;
}

منطقی کا_قطار_MAX_PROP_V(vthread_t thr, vvp_رمز_t cp)
{
      size_t pid = cp->نمبر;
      غیرعلامتی wid = cp->ثنائی_ہندسہ_idx[0];

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_قطار_vec4* dst = qsrc ? قطار_تسلسل_min_max_src(qsrc, wid, سچ)
				 : قطار_تسلسل_min_max_src(dsrc, wid, سچ);
      thr->ڈالو_object(vvp_object_t(dst));
      واپس سچ;
}

منطقی کا_قطار_UNIQUE_OBJ_V(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی wid = cp->ثنائی_ہندسہ_idx[0];
      vvp_object_t src_obj;
      thr->نکالو_object(src_obj);

      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_قطار_vec4* dst = qsrc ? قطار_تسلسل_unique_src(qsrc, wid, جھوٹ)
				 : قطار_تسلسل_unique_src(dsrc, wid, جھوٹ);
      thr->ڈالو_object(vvp_object_t(dst));
      واپس سچ;
}

منطقی کا_قطار_UNIQUE_اشاریہ_OBJ_V(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی wid = cp->ثنائی_ہندسہ_idx[0];
      vvp_object_t src_obj;
      thr->نکالو_object(src_obj);

      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_قطار_vec4* dst = qsrc ? قطار_تسلسل_unique_src(qsrc, wid, سچ)
				 : قطار_تسلسل_unique_src(dsrc, wid, سچ);
      thr->ڈالو_object(vvp_object_t(dst));
      واپس سچ;
}

منطقی کا_قطار_UNIQUE_V(vthread_t thr, vvp_رمز_t cp)
{
      vvp_net_t*net = cp->net;
      غیرعلامتی wid = cp->ثنائی_ہندسہ_idx[0];
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_قطار_vec4* dst;
      اگر (qsrc) dst = قطار_تسلسل_unique_src(qsrc, wid, جھوٹ);
      ورنہ اگر (dsrc) dst = قطار_تسلسل_unique_src(dsrc, wid, جھوٹ);
      ورنہ {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    dst = قطار_تسلسل_unique_src(src, wid, جھوٹ);
      }
      thr->ڈالو_object(vvp_object_t(dst));
      واپس سچ;
}

منطقی کا_قطار_UNIQUE_PROP_V(vthread_t thr, vvp_رمز_t cp)
{
      size_t pid = cp->نمبر;
      غیرعلامتی wid = cp->ثنائی_ہندسہ_idx[0];

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_قطار_vec4* dst = qsrc ? قطار_تسلسل_unique_src(qsrc, wid, جھوٹ)
				 : قطار_تسلسل_unique_src(dsrc, wid, جھوٹ);
      thr->ڈالو_object(vvp_object_t(dst));
      واپس سچ;
}

منطقی کا_قطار_UNIQUE_اشاریہ_V(vthread_t thr, vvp_رمز_t cp)
{
      vvp_net_t*net = cp->net;
      غیرعلامتی wid = cp->ثنائی_ہندسہ_idx[0];
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_قطار_vec4* dst;
      اگر (qsrc) dst = قطار_تسلسل_unique_src(qsrc, wid, سچ);
      ورنہ اگر (dsrc) dst = قطار_تسلسل_unique_src(dsrc, wid, سچ);
      ورنہ {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    dst = قطار_تسلسل_unique_src(src, wid, سچ);
      }
      thr->ڈالو_object(vvp_object_t(dst));
      واپس سچ;
}

منطقی کا_قطار_UNIQUE_اشاریہ_PROP_V(vthread_t thr, vvp_رمز_t cp)
{
      size_t pid = cp->نمبر;
      غیرعلامتی wid = cp->ثنائی_ہندسہ_idx[0];

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_قطار_vec4* dst = qsrc ? قطار_تسلسل_unique_src(qsrc, wid, سچ)
				 : قطار_تسلسل_unique_src(dsrc, wid, سچ);
      thr->ڈالو_object(vvp_object_t(dst));
      واپس سچ;
}

منطقی کا_PROP_قطار_سائز(vthread_t thr, vvp_رمز_t cp)
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

      size_t sz = qsrc ? qsrc->get_سائز() : (dsrc ? dsrc->get_سائز() : 0);

      vvp_vector4_t val(32, BIT4_0);
      غیرعلامتی طویل ul = sz;
      برائے (غیرعلامتی idx = 0; idx < 32; idx++) {
	    اگر (ul & 1UL) {
		  val.سیٹ_ثنائی_ہندسہ(idx, BIT4_1);
	    } ورنہ {
		  val.سیٹ_ثنائی_ہندسہ(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      thr->ڈالو_vec4(val);
      واپس سچ;
}

منطقی کا_قطار_سائز_V(vthread_t thr, vvp_رمز_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      size_t sz =
	    qsrc ? qsrc->get_سائز() : (dsrc ? dsrc->get_سائز() : 0);

      vvp_vector4_t val(32, BIT4_0);
      غیرعلامتی طویل ul = sz;
      برائے (غیرعلامتی idx = 0; idx < 32; idx++) {
	    اگر (ul & 1UL) {
		  val.سیٹ_ثنائی_ہندسہ(idx, BIT4_1);
	    } ورنہ {
		  val.سیٹ_ثنائی_ہندسہ(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      thr->ڈالو_vec4(val);
      واپس سچ;
}

منطقی کا_قطار_لفظ_V(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی wid = cp->ثنائی_ہندسہ_idx[0];
      غیرعلامتی ix_reg = cp->ثنائی_ہندسہ_idx[1];
      int64_t adr = thr->الفاظ[ix_reg].w_int;
      اگر (thr->flags[4] == BIT4_1) {
	    thr->ڈالو_vec4(vvp_vector4_t(wid));
	    واپس سچ;
      }
      اگر (adr < 0) {
	    thr->ڈالو_vec4(vvp_vector4_t(wid));
	    واپس سچ;
      }
      vvp_net_t* net = cp->net;
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t out(wid);
      اگر (qsrc) {
	    اگر ((size_t)adr >= qsrc->get_سائز()) {
		  thr->ڈالو_vec4(out);
		  واپس سچ;
	    }
	    qsrc->get_لفظ((غیرعلامتی)adr, out);
      } ورنہ اگر (dsrc) {
	    اگر ((size_t)adr >= dsrc->get_سائز()) {
		  thr->ڈالو_vec4(out);
		  واپس سچ;
	    }
	    dsrc->get_لفظ((غیرعلامتی)adr, out);
      } ورنہ {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    اگر (!src || (size_t)adr >= src->get_سائز()) {
		  thr->ڈالو_vec4(out);
		  واپس سچ;
	    }
	    src->get_لفظ((غیرعلامتی)adr, out);
      }
      thr->ڈالو_vec4(out);
      واپس سچ;
}

منطقی کا_قطار_لفظ_PROP_V(vthread_t thr, vvp_رمز_t cp)
{
      size_t pid = cp->نمبر;
      غیرعلامتی wid = cp->ثنائی_ہندسہ_idx[0];
      غیرعلامتی ix_reg = cp->ثنائی_ہندسہ_idx[1];
      int64_t adr = thr->الفاظ[ix_reg].w_int;
      اگر (thr->flags[4] == BIT4_1) {
	    thr->ڈالو_vec4(vvp_vector4_t(wid));
	    واپس سچ;
      }
      اگر (adr < 0) {
	    thr->ڈالو_vec4(vvp_vector4_t(wid));
	    واپس سچ;
      }

      vvp_object_t saved_چوٹی;
      منطقی popped_چوٹی = جھوٹ;
      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      اگر (cobj == 0) {
	    // Array-locator with(property) keeps an accumulator queue on top.
	    // Temporarily pop it to access the class object beneath.
	    thr->نکالو_object(saved_چوٹی);
	    popped_چوٹی = سچ;
	    vvp_object_t& نیچے = thr->peek_object();
	    cobj = نیچے.peek<vvp_cobject>();
      }
      اگر (cobj == 0) {
	    اگر (popped_چوٹی) thr->ڈالو_object(saved_چوٹی);
	    thr->ڈالو_vec4(vvp_vector4_t(wid));
	    واپس سچ;
      }

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t out(wid);
      اگر (qsrc) {
	    اگر ((size_t)adr >= qsrc->get_سائز()) {
		  thr->ڈالو_vec4(out);
		  واپس سچ;
	    }
	    qsrc->get_لفظ((غیرعلامتی)adr, out);
      } ورنہ اگر (dsrc) {
	    اگر ((size_t)adr >= dsrc->get_سائز()) {
		  thr->ڈالو_vec4(out);
		  واپس سچ;
	    }
	    dsrc->get_لفظ((غیرعلامتی)adr, out);
      } ورنہ {
	    thr->ڈالو_vec4(out);
	    اگر (popped_چوٹی) thr->ڈالو_object(saved_چوٹی);
	    واپس سچ;
      }
      اگر (popped_چوٹی) thr->ڈالو_object(saved_چوٹی);
      thr->ڈالو_vec4(out);
      واپس سچ;
}

منطقی کا_CMPIX_LTU(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی a = cp->نمبر;
      غیرعلامتی b = cp->ثنائی_ہندسہ_idx[0];
      اگر (thr->الفاظ[a].w_uint < thr->الفاظ[b].w_uint) {
	    thr->flags[4] = BIT4_1;
      } ورنہ {
	    thr->flags[4] = BIT4_0;
      }
      واپس سچ;
}

منطقی کا_CMPIX_SLT0(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی r = cp->نمبر;
      thr->flags[4] = (thr->الفاظ[r].w_int < 0) ? BIT4_1 : BIT4_0;
      واپس سچ;
}

منطقی کا_ڈالو_IX_VEC4(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی ix = cp->نمبر;
      غیرعلامتی wid = cp->ثنائی_ہندسہ_idx[0];
      غیرعلامتی sgn = cp->ثنائی_ہندسہ_idx[1];
      uint32_t ubits;
      اگر (sgn) {
	    int32_t sv = (int32_t)thr->الفاظ[ix].w_int;
	    ubits = (uint32_t)sv;
      } ورنہ {
	    ubits = (uint32_t)thr->الفاظ[ix].w_uint;
      }
      اگر (wid > 32) wid = 32;
      vvp_vector4_t tmp(wid, BIT4_0);
      برائے (غیرعلامتی idx = 0; idx < wid; idx++) {
	    اگر (ubits & 1u) {
		  tmp.سیٹ_ثنائی_ہندسہ(idx, BIT4_1);
	    } ورنہ {
		  tmp.سیٹ_ثنائی_ہندسہ(idx, BIT4_0);
	    }
	    ubits >>= 1;
      }
      thr->ڈالو_vec4(tmp);
      واپس سچ;
}

منطقی کا_قطار_NEW_خالی_خانے_V(vthread_t thr, vvp_رمز_t)
{
      thr->ڈالو_object(vvp_object_t(نیا vvp_قطار_vec4()));
      واپس سچ;
}

منطقی کا_قطار_APPEND_لفظ_V(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی wid = cp->ثنائی_ہندسہ_idx[0];
      vvp_vector4_t elem = thr->نکالو_vec4();
      أكد(elem.size() == wid);
      vvp_object_t qobj;
      thr->نکالو_object(qobj);
      vvp_قطار_vec4* q = qobj.peek<vvp_قطار_vec4>();
      أكد(q);
      q->push_back(elem, 0);
      thr->ڈالو_object(qobj);
      واپس سچ;
}

منطقی کا_DELETE_PROP_ELEM(vthread_t thr, vvp_رمز_t cp)
{
      size_t pid = cp->نمبر;

      int64_t idx_val = thr->الفاظ[3].w_int;
      اگر (thr->flags[4] == BIT4_1) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with undefined index."
	         << سطر_ختم;
	    واپس سچ;
      }
      اگر (idx_val < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with negative index."
	         << سطر_ختم;
	    واپس سچ;
      }
      size_t idx = idx_val;

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_قطار* قطار = qobj.peek<vvp_قطار>();
      اگر (قطار == 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping delete(" << idx
	         << ") on empty queue." << سطر_ختم;
      } ورنہ {
	    size_t size = قطار->get_سائز();
	    اگر (idx >= size) {
		  cerr << thr->get_fileline()
		       << "Warning: skipping out of range delete(" << idx
		       << ") on queue of size " << size << "." << سطر_ختم;
	    } ورنہ {
		  قطار->erase(idx);
	    }
      }

      واپس سچ;
}

منطقی کا_DELETE_PROP_OBJ(vthread_t thr, vvp_رمز_t cp)
{
      size_t pid = cp->نمبر;

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      cobj->سیٹ_object(pid, vvp_object_t(), 0);

      واپس سچ;
}

سانچہ <قسم_نام ELEM, جماعت QTYPE>
ساکن منطقی store_qb(vthread_t thr, vvp_رمز_t cp, غیرعلامتی wid=0)
{
      ELEM value;
      vvp_net_t*net = cp->net;
      غیرعلامتی max_سائز = thr->الفاظ[cp->ثنائی_ہندسہ_idx[0]].w_int;
      نکالو_قدر(thr, value, wid); // Pop the value to store.

      vvp_قطار*قطار = get_قطار_object<QTYPE>(thr, net);
      أكد(قطار);
      قطار->push_back(value, max_سائز);
      واپس سچ;
}

/*
 * %store/qb/r <var-label>, <max-idx>
 */
منطقی کا_STORE_QB_R(vthread_t thr, vvp_رمز_t cp)
{
      واپس store_qb<دوہرا, vvp_قطار_real>(thr, cp);
}

/*
 * %store/qb/str <var-label>, <max-idx>
 */
منطقی کا_STORE_QB_STR(vthread_t thr, vvp_رمز_t cp)
{
      واپس store_qb<string, vvp_قطار_string>(thr, cp);
}

/*
 * %store/qb/v <var-label>, <max-idx>, <wid>
 */
منطقی کا_STORE_QB_V(vthread_t thr, vvp_رمز_t cp)
{
      واپس store_qb<vvp_vector4_t, vvp_قطار_vec4>(thr, cp, cp->ثنائی_ہندسہ_idx[1]);
}

سانچہ <قسم_نام ELEM, جماعت QTYPE>
ساکن منطقی store_qdar(vthread_t thr, vvp_رمز_t cp, غیرعلامتی wid=0)
{
      int64_t idx = thr->الفاظ[3].w_int;
      ELEM value;
      vvp_net_t*net = cp->net;
      نکالو_قدر(thr, value, wid); // Pop the value to store.

      vvp_قطار*قطار = get_قطار_object<QTYPE>(thr, net);
      أكد(قطار);
      اگر (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot assign to a negative "
	         << get_قطار_type(value)
	         << " index (" << idx << "). ";
	    print_قطار_قدر(value);
	    cerr << " was not added." << سطر_ختم;
      } ورنہ اگر (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot assign to an undefined "
	         << get_قطار_type(value) << " index. ";
	    print_قطار_قدر(value);
	    cerr << " was not added." << سطر_ختم;
      } ورنہ {
	    غیرعلامتی max_سائز = thr->الفاظ[cp->ثنائی_ہندسہ_idx[0]].w_int;
	    قطار->سیٹ_لفظ_max(idx, value, max_سائز);
      }
      واپس سچ;
}

/*
 * %store/qdar/r <var>, idx
 */
منطقی کا_STORE_QDAR_R(vthread_t thr, vvp_رمز_t cp)
{
      واپس store_qdar<دوہرا, vvp_قطار_real>(thr, cp);
}

/*
 * %store/qdar/str <var>, idx
 */
منطقی کا_STORE_QDAR_STR(vthread_t thr, vvp_رمز_t cp)
{
      واپس store_qdar<string, vvp_قطار_string>(thr, cp);
}

/*
 * %store/qdar/v <var>, idx
 */
منطقی کا_STORE_QDAR_V(vthread_t thr, vvp_رمز_t cp)
{
      واپس store_qdar<vvp_vector4_t, vvp_قطار_vec4>(thr, cp, cp->ثنائی_ہندسہ_idx[1]);
}

سانچہ <قسم_نام ELEM, جماعت QTYPE>
ساکن منطقی store_qf(vthread_t thr, vvp_رمز_t cp, غیرعلامتی wid=0)
{
      ELEM value;
      vvp_net_t*net = cp->net;
      غیرعلامتی max_سائز = thr->الفاظ[cp->ثنائی_ہندسہ_idx[0]].w_int;
      نکالو_قدر(thr, value, wid); // Pop the value to store.

      vvp_قطار*قطار = get_قطار_object<QTYPE>(thr, net);
      أكد(قطار);
      قطار->push_front(value, max_سائز);
      واپس سچ;
}
/*
 * %store/qf/r <var-label>, <max-idx>
 */
منطقی کا_STORE_QF_R(vthread_t thr, vvp_رمز_t cp)
{
      واپس store_qf<دوہرا, vvp_قطار_real>(thr, cp);
}

/*
 * %store/qf/str <var-label>, <max-idx>
 */
منطقی کا_STORE_QF_STR(vthread_t thr, vvp_رمز_t cp)
{
      واپس store_qf<string, vvp_قطار_string>(thr, cp);
}

/*
 * %store/qb/v <var-label>, <max-idx>, <wid>
 */
منطقی کا_STORE_QF_V(vthread_t thr, vvp_رمز_t cp)
{
      واپس store_qf<vvp_vector4_t, vvp_قطار_vec4>(thr, cp, cp->ثنائی_ہندسہ_idx[1]);
}

سانچہ <قسم_نام ELEM, جماعت QTYPE>
ساکن منطقی store_qobj(vthread_t thr, vvp_رمز_t cp, غیرعلامتی wid=0)
{
// FIXME: Can we actually use wid here?
      (خالی)wid;
      vvp_net_t*net = cp->net;

      vvp_قطار*قطار = get_قطار_object<QTYPE>(thr, net);
      أكد(قطار);

      vvp_object_t src;
      thr->نکالو_object(src);

        // If it is null just clear the queue
      اگر (src.test_nil())
	    قطار->erase_دم(0);
      ورنہ {
	    غیرعلامتی max_سائز = thr->الفاظ[cp->ثنائی_ہندسہ_idx[0]].w_int;
	    قطار->copy_elems(src, max_سائز);
      }

      واپس سچ;
}

منطقی کا_STORE_QOBJ_R(vthread_t thr, vvp_رمز_t cp)
{
      واپس store_qobj<دوہرا, vvp_قطار_real>(thr, cp);
}

منطقی کا_STORE_QOBJ_STR(vthread_t thr, vvp_رمز_t cp)
{
      واپس store_qobj<string, vvp_قطار_string>(thr, cp);
}

منطقی کا_STORE_QOBJ_V(vthread_t thr, vvp_رمز_t cp)
{
      واپس store_qobj<vvp_vector4_t, vvp_قطار_vec4>(thr, cp, cp->ثنائی_ہندسہ_idx[1]);
}

ساکن خالی vvp_send(vthread_t thr, vvp_net_ptr_t ptr, ثابت دوہرا&val)
{
      vvp_send_real(ptr, val, thr->wt_context);
}

ساکن خالی vvp_send(vthread_t thr, vvp_net_ptr_t ptr, ثابت string&val)
{
      vvp_send_string(ptr, val, thr->wt_context);
}

سانچہ <قسم_نام ELEM>
ساکن منطقی store(vthread_t thr, vvp_رمز_t cp)
{
      ELEM val;
      نکالو_قدر(thr, val, 0);
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_send(thr, ptr, val);
      واپس سچ;
}

منطقی کا_STORE_REAL(vthread_t thr, vvp_رمز_t cp)
{
      واپس store<دوہرا>(thr, cp);
}

سانچہ <قسم_نام ELEM>
ساکن منطقی storea(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی idx = cp->ثنائی_ہندسہ_idx[0];
      ELEM val;
      نکالو_قدر(thr, val, 0);

      اگر (thr->flags[4] != BIT4_1) {
	    غیرعلامتی adr = thr->الفاظ[idx].w_int;
	    cp->array->سیٹ_لفظ(adr, val);
      }

      واپس سچ;
}

/*
 * %store/reala <var-label> <index>
 */
منطقی کا_STORE_REALA(vthread_t thr, vvp_رمز_t cp)
{
      واپس storea<دوہرا>(thr, cp);
}

منطقی کا_STORE_STR(vthread_t thr, vvp_رمز_t cp)
{
      واپس store<string>(thr, cp);
}

/*
 * %store/stra <array-label> <index>
 */
منطقی کا_STORE_STRA(vthread_t thr, vvp_رمز_t cp)
{
      واپس storea<string>(thr, cp);
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
منطقی کا_STORE_VEC4(vthread_t thr, vvp_رمز_t cp)
{
      vvp_net_ptr_t ptr(cp->net, 0);
      ثابت vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*> (cp->net->fil);
      غیرعلامتی off_اشاریہ = cp->ثنائی_ہندسہ_idx[0];
      غیرعلامتی صحیح wid = cp->ثنائی_ہندسہ_idx[1];

      int64_t off = off_اشاریہ ? thr->الفاظ[off_اشاریہ].w_int : 0;
      غیرعلامتی صحیح sig_قدر_سائز = sig->قدر_سائز();

      vvp_vector4_t&val = thr->peek_vec4();
      غیرعلامتی val_سائز = val.size();

      اگر (val_سائز < wid) {
	    cerr << thr->get_fileline()
	         << "XXXX Internal error: val.size()=" << val_سائز
		 << ", expecting >= " << wid << سطر_ختم;
      }
      أكد(val_سائز >= wid);
      اگر (val_سائز > wid) {
	    val.resize(wid);
      }

	// If there is a problem loading the index register, flags-4
	// will be set to 1, and we know here to skip the actual assignment.
      اگر (off_اشاریہ!=0 && thr->flags[4] == BIT4_1) {
	    thr->نکالو_vec4(1);
	    واپس سچ;
      }

      اگر (!resize_rval_vec(val, off, sig_قدر_سائز)) {
	    thr->نکالو_vec4(1);
	    واپس سچ;
      }

      اگر (off == 0 && val.size() == sig_قدر_سائز)
	    vvp_send_vec4(ptr, val, thr->wt_context);
      ورنہ
	    vvp_send_vec4_pv(ptr, val, off, sig_قدر_سائز, thr->wt_context);

      thr->نکالو_vec4(1);
      واپس سچ;
}

/*
 * %store/vec4a <var-label>, <addr>, <offset>
 */
منطقی کا_STORE_VEC4_A(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی adr_اشاریہ = cp->ثنائی_ہندسہ_idx[0];
      غیرعلامتی off_اشاریہ = cp->ثنائی_ہندسہ_idx[1];

      طویل adr = adr_اشاریہ? thr->الفاظ[adr_اشاریہ].w_int : 0;
      int64_t off = off_اشاریہ ? thr->الفاظ[off_اشاریہ].w_int : 0;

	// Suppress action if flags-4 is true.
      اگر (thr->flags[4] == BIT4_1) {
	    thr->نکالو_vec4(1);
	    واپس سچ;
      }

      vvp_vector4_t &value = thr->peek_vec4();

      اگر (!resize_rval_vec(value, off, cp->array->get_لفظ_سائز())) {
	    thr->نکالو_vec4(1);
	    واپس سچ;
      }

      cp->array->سیٹ_لفظ(adr, off, value);

      thr->نکالو_vec4(1);
      واپس سچ;
}

/*
 * %sub
 *   pop r;
 *   pop l;
 *   push l-r;
 */
منطقی کا_SUB(vthread_t thr, vvp_رمز_t)
{
      vvp_vector4_t r = thr->نکالو_vec4();
      vvp_vector4_t&l = thr->peek_vec4();

      l.sub(r);
      واپس سچ;
}

/*
 * %subi <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
منطقی کا_SUBI(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی wid = cp->نمبر;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_فوری_rval (cp, r);

      l.sub(r);

      واپس سچ;

}

منطقی کا_SUB_WR(vthread_t thr, vvp_رمز_t)
{
      دوہرا r = thr->نکالو_real();
      دوہرا l = thr->نکالو_real();
      thr->ڈالو_real(l - r);
      واپس سچ;
}

/*
 * %substr <first>, <last>
 * Pop a string, take the substring (SystemVerilog style), and return
 * the result to the stack. This opcode actually works by editing the
 * string in place.
 */
منطقی کا_SUBSTR(vthread_t thr, vvp_رمز_t cp)
{
      int32_t پہلا = thr->الفاظ[cp->ثنائی_ہندسہ_idx[0]].w_int;
      int32_t آخری = thr->الفاظ[cp->ثنائی_ہندسہ_idx[1]].w_int;
      string&val = thr->peek_str(0);

      اگر (پہلا < 0 || آخری < پہلا || آخری >= (int32_t)val.size()) {
	    val = string("");
	    واپس سچ;
      }

      val = val.substr(پہلا, آخری-پہلا+1);
      واپس سچ;
}

/*
 * %substr/vec4 <index>, <wid>
 */
منطقی کا_SUBSTR_VEC4(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی sel_idx = cp->ثنائی_ہندسہ_idx[0];
      غیرعلامتی wid = cp->ثنائی_ہندسہ_idx[1];

      int32_t sel = thr->الفاظ[sel_idx].w_int;
      ثابت string&val = thr->peek_str(0);

      أكد(wid%8 == 0);

      اگر (sel < 0 || sel >= (int32_t)val.size()) {
	    vvp_vector4_t res (wid, BIT4_0);
	    thr->ڈالو_vec4(res);
	    واپس سچ;
      }

      vvp_vector4_t res (wid, BIT4_0);

      أكد(wid==8);
      غیرعلامتی حرف tmp = val[sel];
      برائے (صحیح idx = 0 ; idx < 8 ; idx += 1) {
	    اگر (tmp & (1<<idx))
		  res.سیٹ_ثنائی_ہندسہ(idx, BIT4_1);
      }

      thr->ڈالو_vec4(res);
      واپس سچ;
}

منطقی کا_FILE_سطر(vthread_t thr, vvp_رمز_t cp)
{
      vpiHandle handle = cp->handle;

	/* When it is available, keep the file/line information in the
	   thread for error/warning messages. */
      thr->سیٹ_fileline(vpi_get_str(vpiFile, handle),
                        vpi_get(vpi_سطر_No, handle));

      اگر (show_file_سطر)
	    cerr << thr->get_fileline()
	         << vpi_get_str(_vpiDescription, handle) << سطر_ختم;

      واپس سچ;
}

/*
 * %test_nul <var-label>;
 * Test if the object at the specified variable is nil. If so, write
 * "1" into flags[4], otherwise write "0" into flags[4].
 */
منطقی کا_TEST_NUL(vthread_t thr, vvp_رمز_t cp)
{
      vvp_net_t*net = cp->net;

      أكد(net);
      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      أكد(obj);

      اگر (obj->get_object().test_nil())
	    thr->flags[4] = BIT4_1;
      ورنہ
	    thr->flags[4] = BIT4_0;

      واپس سچ;
}

منطقی کا_TEST_NUL_A(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی idx = cp->ثنائی_ہندسہ_idx[0];
      غیرعلامتی adr = thr->الفاظ[idx].w_int;
      vvp_object_t لفظ;

	/* If the address is undefined, return true. */
      اگر (thr->flags[4] == BIT4_1) {
	    واپس سچ;
      }

      cp->array->get_لفظ_obj(adr, لفظ);
      اگر (لفظ.test_nil())
	    thr->flags[4] = BIT4_1;
      ورنہ
	    thr->flags[4] = BIT4_0;

      واپس سچ;
}

منطقی کا_TEST_NUL_OBJ(vthread_t thr, vvp_رمز_t)
{
      اگر (thr->peek_object().test_nil())
	    thr->flags[4] = BIT4_1;
      ورنہ
	    thr->flags[4] = BIT4_0;
      واپس سچ;
}

/*
 * %test_nul/prop <pid>, <idx>
 */
منطقی کا_TEST_NUL_PROP(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی pid = cp->نمبر;
      غیرعلامتی idx = cp->ثنائی_ہندسہ_idx[0];

      اگر (idx != 0) {
	    أكد(idx < vthread_s::الفاظ_گنتی);
	    idx = thr->الفاظ[idx].w_uint;
      }

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj  = obj.peek<vvp_cobject>();

      vvp_object_t val;
      cobj->get_object(pid, val, idx);

      اگر (val.test_nil())
	    thr->flags[4] = BIT4_1;
      ورنہ
	    thr->flags[4] = BIT4_0;

      واپس سچ;
}

/* and() reduction returns the scalar sum (Verilog-style bitwise-AND per word). */
سانچہ <جماعت SRC>
ساکن vvp_vector4_t uparr_and_الفاظ_src(SRC* src, غیرعلامتی wid)
{
      اگر (!src) واپس vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_1);
      برائے (size_t i = 0; i < src->get_سائز(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_لفظ(i, vi);
	    res &= vi;
      }
      واپس res;
}

منطقی کا_UPARR_AND_V(vthread_t thr, vvp_رمز_t cp)
{
      vvp_net_t* net = cp->net;
      غیرعلامتی wid = cp->ثنائی_ہندسہ_idx[0];
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      اگر (qsrc) res = uparr_and_الفاظ_src(qsrc, wid);
      ورنہ اگر (dsrc) res = uparr_and_الفاظ_src(dsrc, wid);
      ورنہ {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    res = uparr_and_الفاظ_src(src, wid);
      }
      thr->ڈالو_vec4(res);
      واپس سچ;
}

/* or() reduction returns the scalar sum (Verilog-style bitwise-AND per word). */
سانچہ <جماعت SRC>
ساکن vvp_vector4_t uparr_or_الفاظ_src(SRC* src, غیرعلامتی wid)
{
      اگر (!src) واپس vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_0);
      برائے (size_t i = 0; i < src->get_سائز(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_لفظ(i, vi);
	    res |= vi;
      }
      واپس res;
}

منطقی کا_UPARR_OR_V(vthread_t thr, vvp_رمز_t cp)
{
      vvp_net_t* net = cp->net;
      غیرعلامتی wid = cp->ثنائی_ہندسہ_idx[0];
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      اگر (qsrc) res = uparr_or_الفاظ_src(qsrc, wid);
      ورنہ اگر (dsrc) res = uparr_or_الفاظ_src(dsrc, wid);
      ورنہ {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    res = uparr_or_الفاظ_src(src, wid);
      }
      thr->ڈالو_vec4(res);
      واپس سچ;
}

/* real product() reduction returns the scalar sum */
سانچہ <جماعت SRC>
ساکن دوہرا uparr_حاصل_ضرب_الفاظ_src_real(SRC* src)
{
      اگر (!src) واپس 0.0;
      دوہرا acc = 1.0;
      برائے (size_t i = 0; i < src->get_سائز(); i += 1) {
	    دوہرا vi;
	    src->get_لفظ(i, vi);
	    acc *= vi;
      }
      واپس acc;
}

منطقی کا_UPARR_حاصل_ضرب_REAL(vthread_t thr, vvp_رمز_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_قطار_real* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_real_from_net(thr, net, qsrc, dsrc);
      دوہرا مجموع;
      اگر (qsrc) مجموع = uparr_حاصل_ضرب_الفاظ_src_real(qsrc);
      ورنہ اگر (dsrc) مجموع = uparr_حاصل_ضرب_الفاظ_src_real(dsrc);
      ورنہ {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_real>(thr, net);
	    vvp_قطار_real* src = dynamic_cast<vvp_قطار_real*>(src_q);
	    مجموع = uparr_حاصل_ضرب_الفاظ_src_real(src);
      }
      thr->ڈالو_real(مجموع);
      واپس سچ;
}

/* product() reduction returns the scalar product (Verilog-style multiplication per word). */
سانچہ <جماعت SRC>
ساکن vvp_vector4_t uparr_حاصل_ضرب_الفاظ_src(SRC* src, غیرعلامتی wid)
{
      اگر (!src) واپس vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t acc = قطار_unique_ulong_مقصد_vec4(1UL, wid);
      برائے (size_t i = 0; i < src->get_سائز(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_لفظ(i, vi);
	    acc.mul(vi);
      }
      واپس acc;
}

منطقی کا_UPARR_حاصل_ضرب_V(vthread_t thr, vvp_رمز_t cp)
{
      vvp_net_t* net = cp->net;
      غیرعلامتی wid = cp->ثنائی_ہندسہ_idx[0];
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t prod;
      اگر (qsrc) prod = uparr_حاصل_ضرب_الفاظ_src(qsrc, wid);
      ورنہ اگر (dsrc) prod = uparr_حاصل_ضرب_الفاظ_src(dsrc, wid);
      ورنہ {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    prod = uparr_حاصل_ضرب_الفاظ_src(src, wid);
      }
      thr->ڈالو_vec4(prod);
      واپس سچ;
}

منطقی کا_UPARR_حاصل_ضرب_PROP_V(vthread_t thr, vvp_رمز_t cp)
{
      size_t pid = cp->نمبر;
      غیرعلامتی wid = cp->ثنائی_ہندسہ_idx[0];

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t prod =
	    qsrc ? uparr_حاصل_ضرب_الفاظ_src(qsrc, wid)
		 : uparr_حاصل_ضرب_الفاظ_src(dsrc, wid);
      thr->ڈالو_vec4(prod);
      واپس سچ;
}

منطقی کا_UPARR_حاصل_ضرب_OBJ_V(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی wid = cp->ثنائی_ہندسہ_idx[0];
      vvp_object_t src_obj;
      thr->نکالو_object(src_obj);

      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_vector4_t prod =
	    qsrc ? uparr_حاصل_ضرب_الفاظ_src(qsrc, wid)
		 : uparr_حاصل_ضرب_الفاظ_src(dsrc, wid);
      thr->ڈالو_vec4(prod);
      واپس سچ;
}

/* real sum() reduction returns the scalar sum */
سانچہ <جماعت SRC>
ساکن دوہرا uparr_مجموع_الفاظ_src_real(SRC* src)
{
      دوہرا acc = 0.0;
      اگر (!src) واپس acc;
      برائے (size_t i = 0; i < src->get_سائز(); i += 1) {
	    دوہرا vi;
	    src->get_لفظ(i, vi);
	    acc += vi;
      }
      واپس acc;
}

منطقی کا_UPARR_مجموع_REAL(vthread_t thr, vvp_رمز_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_قطار_real* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_real_from_net(thr, net, qsrc, dsrc);
      دوہرا مجموع;
      اگر (qsrc) مجموع = uparr_مجموع_الفاظ_src_real(qsrc);
      ورنہ اگر (dsrc) مجموع = uparr_مجموع_الفاظ_src_real(dsrc);
      ورنہ {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_real>(thr, net);
	    vvp_قطار_real* src = dynamic_cast<vvp_قطار_real*>(src_q);
	    مجموع = uparr_مجموع_الفاظ_src_real(src);
      }
      thr->ڈالو_real(مجموع);
      واپس سچ;
}

/* sum() reduction returns the scalar sum (Verilog-style addition per word). */
سانچہ <جماعت SRC>
ساکن vvp_vector4_t uparr_مجموع_الفاظ_src(SRC* src, غیرعلامتی wid)
{
      اگر (!src) واپس vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t acc(wid, BIT4_0);
      برائے (size_t i = 0; i < src->get_سائز(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_لفظ(i, vi);
	    acc.جوڑو(vi);
      }
      واپس acc;
}

منطقی کا_UPARR_مجموع_V(vthread_t thr, vvp_رمز_t cp)
{
      vvp_net_t* net = cp->net;
      غیرعلامتی wid = cp->ثنائی_ہندسہ_idx[0];
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t مجموع;
      اگر (qsrc) مجموع = uparr_مجموع_الفاظ_src(qsrc, wid);
      ورنہ اگر (dsrc) مجموع = uparr_مجموع_الفاظ_src(dsrc, wid);
      ورنہ {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    مجموع = uparr_مجموع_الفاظ_src(src, wid);
      }
      thr->ڈالو_vec4(مجموع);
      واپس سچ;
}

منطقی کا_UPARR_مجموع_PROP_V(vthread_t thr, vvp_رمز_t cp)
{
      size_t pid = cp->نمبر;
      غیرعلامتی wid = cp->ثنائی_ہندسہ_idx[0];

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t مجموع =
	    qsrc ? uparr_مجموع_الفاظ_src(qsrc, wid)
		 : uparr_مجموع_الفاظ_src(dsrc, wid);
      thr->ڈالو_vec4(مجموع);
      واپس سچ;
}

منطقی کا_UPARR_مجموع_OBJ_V(vthread_t thr, vvp_رمز_t cp)
{
      غیرعلامتی wid = cp->ثنائی_ہندسہ_idx[0];
      vvp_object_t src_obj;
      thr->نکالو_object(src_obj);

      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_vector4_t مجموع =
	    qsrc ? uparr_مجموع_الفاظ_src(qsrc, wid)
		 : uparr_مجموع_الفاظ_src(dsrc, wid);
      thr->ڈالو_vec4(مجموع);
      واپس سچ;
}

/* xor() reduction returns the scalar sum (Verilog-style bitwise-XOR per word). */
سانچہ <جماعت SRC>
ساکن vvp_vector4_t uparr_xor_الفاظ_src(SRC* src, غیرعلامتی wid)
{
      اگر (!src) واپس vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_0);
      برائے (size_t i = 0; i < src->get_سائز(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_لفظ(i, vi);
	    res ^= vi;
      }
      واپس res;
}

منطقی کا_UPARR_XOR_V(vthread_t thr, vvp_رمز_t cp)
{
      vvp_net_t* net = cp->net;
      غیرعلامتی wid = cp->ثنائی_ہندسہ_idx[0];
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      اگر (qsrc) res = uparr_xor_الفاظ_src(qsrc, wid);
      ورنہ اگر (dsrc) res = uparr_xor_الفاظ_src(dsrc, wid);
      ورنہ {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    res = uparr_xor_الفاظ_src(src, wid);
      }
      thr->ڈالو_vec4(res);
      واپس سچ;
}

منطقی کا_VPI_CALL(vthread_t thr, vvp_رمز_t cp)
{
      vpip_execute_vpi_call(thr, cp->handle);

      اگر (جدول_وقت_stopped()) {
	    اگر (! جدول_وقت_ختم_شدہ())
		  جدول_وقت_vthread(thr, 0, جھوٹ);

	    واپس جھوٹ;
      }

      واپس جدول_وقت_ختم_شدہ()? جھوٹ : سچ;
}

/* %wait <label>;
 * Implement the wait by locating the vvp_net_T for the event, and
 * adding this thread to the threads list for the event. The some
 * argument is the  reference to the functor to wait for. This must be
 * an event object of some sort.
 */
منطقی کا_WAIT(vthread_t thr, vvp_رمز_t cp)
{
      أكد(! thr->i_am_اندر_function);
      أكد(! thr->انتظار_for_event);
      thr->انتظار_for_event = 1;

	/* Add this thread to the list in the event. */
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (cp->net->fun);
      أكد(ep);
      thr->wait_اگلا = ep->جوڑو_انتظار_thread(thr);

	/* Return false to suspend this thread. */
      واپس جھوٹ;
}

/*
 * Implement the %wait/fork (SystemVerilog) instruction by suspending
 * the current thread until all the detached children have finished.
 */
منطقی کا_WAIT_FORK(vthread_t thr, vvp_رمز_t)
{
	/* If a %wait/fork is being executed then the parent thread
	 * cannot be waiting in a join or already waiting. */
      أكد(! thr->i_am_اندر_function);
      أكد(! thr->i_am_joining);
      أكد(! thr->i_am_انتظار);

	/* There should be no active children when waiting. */
      أكد(thr->children.empty());

	/* If there are no detached children then there is nothing to
	 * wait for. */
      اگر (thr->detached_children.empty()) واپس سچ;

	/* Flag that this process is waiting for the detached children
	 * to finish and suspend it. */
      thr->i_am_انتظار = 1;
      واپس جھوٹ;
}

/*
 * %xnor
 */
منطقی کا_XNOR(vthread_t thr, vvp_رمز_t)
{
      vvp_vector4_t valr = thr->نکالو_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      أكد(vall.size() == valr.size());

      vall ^= valr;
      vall.invert();

      واپس سچ;
}

/*
 * %xor
 */
منطقی کا_XOR(vthread_t thr, vvp_رمز_t)
{
      vvp_vector4_t valr = thr->نکالو_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      أكد(vall.size() == valr.size());

      vall ^= valr;

      واپس سچ;
}


منطقی کا_ZOMBIE(vthread_t thr, vvp_رمز_t)
{
      thr->pc = codespace_null();
      اگر ((thr->والد == 0) && (thr->children.empty())) {
	    اگر (thr->delay_delete)
		  جدول_وقت_del_thr(thr);
	    ورنہ
		  vthread_delete(thr);
      }
      واپس جھوٹ;
}

/*
 * This is a phantom opcode used to call user defined functions. It
 * is used in code generated by the .ufunc statement. It contains a
 * pointer to the executable code of the function and a pointer to
 * a ufunc_core object that has all the port information about the
 * function.
 */
ساکن منطقی do_exec_ufunc(vthread_t thr, vvp_رمز_t cp, vthread_t بچہ)
{
      __vpiScope*بچہ_scope = cp->ufunc_core_ptr->func_scope();
      أكد(بچہ_scope);

      أكد(بچہ_scope->get_type_رمز() == vpiFunction);
      أكد(thr->children.empty());


        /* We can take a number of shortcuts because we know that a
           continuous assignment can only occur in a static scope. */
      أكد(thr->wt_context == 0);
      أكد(thr->rd_context == 0);

        /* If an automatic function, allocate a context for this call. */
      vvp_context_t بچہ_context = 0;
      اگر (بچہ_scope->is_automatic()) {
            بچہ_context = vthread_alloc_context(بچہ_scope);
            thr->wt_context = بچہ_context;
            thr->rd_context = بچہ_context;
      }

      بچہ->wt_context = بچہ_context;
      بچہ->rd_context = بچہ_context;

	/* Copy all the inputs to the ufunc object to the port
	   variables of the function. This copies all the values
	   atomically. */
      cp->ufunc_core_ptr->تفویض_بٹ_مقصد_ports(بچہ_context);
      بچہ->delay_delete = 1;

      بچہ->والد = thr;
      thr->children.insert(بچہ);
	// This should be the only child
      أكد(thr->children.size()==1);

      بچہ->is_scheduled = 1;
      بچہ->i_am_اندر_function = 1;
      vthread_تسلسل(بچہ);
      running_thread = thr;

      اگر (بچہ->i_have_ended) {
	    do_join(thr, بچہ);
            واپس سچ;
      } ورنہ {
	    thr->i_am_joining = 1;
	    واپس جھوٹ;
      }
}

منطقی کا_EXEC_UFUNC_REAL(vthread_t thr, vvp_رمز_t cp)
{
      __vpiScope*بچہ_scope = cp->ufunc_core_ptr->func_scope();
      أكد(بچہ_scope);

	/* Create a temporary thread and run it immediately. */
      vthread_t بچہ = vthread_new(cp->cptr, بچہ_scope);
      thr->ڈالو_real(0.0);
      بچہ->دلائل_real.push_back(0);

      واپس do_exec_ufunc(thr, cp, بچہ);
}

منطقی کا_EXEC_UFUNC_VEC4(vthread_t thr, vvp_رمز_t cp)
{
      __vpiScope*بچہ_scope = cp->ufunc_core_ptr->func_scope();
      أكد(بچہ_scope);

      vpiScopeFunction*scope_func = dynamic_cast<vpiScopeFunction*>(بچہ_scope);
      أكد(scope_func);

	/* Create a temporary thread and run it immediately. */
      vthread_t بچہ = vthread_new(cp->cptr, بچہ_scope);
      thr->ڈالو_vec4(vvp_vector4_t(scope_func->get_func_چوڑائی(), scope_func->get_func_init_val()));
      بچہ->دلائل_vec4.push_back(0);

      واپس do_exec_ufunc(thr, cp, بچہ);
}

/*
 * This is a phantom opcode used to harvest the result of calling a user
 * defined function. It is used in code generated by the .ufunc statement.
 */
منطقی کا_REAP_UFUNC(vthread_t thr, vvp_رمز_t cp)
{
      __vpiScope*بچہ_scope = cp->ufunc_core_ptr->func_scope();
      أكد(بچہ_scope);

	/* Copy the output from the result variable to the output
	   ports of the .ufunc device. */
      cp->ufunc_core_ptr->اختتام_thread();

        /* If an automatic function, free the context for this call. */
      اگر (بچہ_scope->is_automatic()) {
            vthread_free_context(thr->rd_context, بچہ_scope);
            thr->wt_context = 0;
            thr->rd_context = 0;
      }

      واپس سچ;
}
