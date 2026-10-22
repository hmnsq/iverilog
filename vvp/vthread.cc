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
#اگر_تعریف_شده CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#پایان_اگر
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

استفاده نطاق std;

/* This is the size of an unsigned long in bits. This is just a
   convenience macro. */
# define CPU_کلمه_بیتها (8*حجم(بیعلامت دراز))
# define بالاترین_بیت (1UL << (CPU_کلمه_بیتها-1))

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
      vvp_کد_t pc;
	/* These hold the private thread bits. */
      تعداد { FLAGS_شمار = 512, واژگان_شمار = 16 };
      vvp_bit4_t flags[FLAGS_شمار];

	/* These are the word registers. */
      اتحاد {
	    int64_t  w_int;
	    uint64_t w_uint;
      } واژگان[واژگان_شمار];

	// These vectors are depths within the parent thread's
	// corresponding stack.  This is how the %ret/* instructions
	// get at parent thread arguments.
      vector<بیعلامت> آرگومانها_real;
      vector<بیعلامت> آرگومانها_str;
      vector<بیعلامت> آرگومانها_vec4;

    خاص:
      vector<vvp_vector4_t>پشته_vec4;
    عام:
      مضمن vvp_vector4_t برداشت_vec4(فراغ)
      {
	    أكد(! پشته_vec4.empty());
	    vvp_vector4_t val = پشته_vec4.back();
	    پشته_vec4.pop_back();
	    أرجع val;
      }
      مضمن فراغ افزودن_vec4(ثابت vvp_vector4_t&val)
      {
	    پشته_vec4.push_back(val);
      }
      مضمن ثابت vvp_vector4_t& peek_vec4(بیعلامت عمق)
      {
	    بیعلامت size = پشته_vec4.size();
	    أكد(عمق < size);
	    بیعلامت use_شاخص = size-1-عمق;
	    أرجع پشته_vec4[use_شاخص];
      }
      مضمن vvp_vector4_t& peek_vec4(فراغ)
      {
	    بیعلامت use_شاخص = پشته_vec4.size();
	    أكد(use_شاخص >= 1);
	    أرجع پشته_vec4[use_شاخص-1];
      }
      مضمن فراغ poke_vec4(بیعلامت عمق, ثابت vvp_vector4_t&val)
      {
	    أكد(عمق < پشته_vec4.size());
	    بیعلامت use_شاخص = پشته_vec4.size()-1-عمق;
	    پشته_vec4[use_شاخص] = val;
      }
      مضمن فراغ برداشت_vec4(بیعلامت cnt)
      {
	    طالما (cnt > 0) {
		  پشته_vec4.pop_back();
		  cnt -= 1;
	    }
      }


    خاص:
      vector<مزدوج> پشته_real;
    عام:
      مضمن مزدوج برداشت_real(فراغ)
      {
	    أكد(! پشته_real.empty());
	    مزدوج val = پشته_real.back();
	    پشته_real.pop_back();
	    أرجع val;
      }
      مضمن فراغ افزودن_real(مزدوج val)
      {
	    پشته_real.push_back(val);
      }
      مضمن مزدوج peek_real(بیعلامت عمق)
      {
	    أكد(عمق < پشته_real.size());
	    بیعلامت use_شاخص = پشته_real.size()-1-عمق;
	    أرجع پشته_real[use_شاخص];
      }
      مضمن فراغ poke_real(بیعلامت عمق, مزدوج val)
      {
	    أكد(عمق < پشته_real.size());
	    بیعلامت use_شاخص = پشته_real.size()-1-عمق;
	    پشته_real[use_شاخص] = val;
      }
      مضمن فراغ برداشت_real(بیعلامت cnt)
      {
	    طالما (cnt > 0) {
		  پشته_real.pop_back();
		  cnt -= 1;
	    }
      }

	/* Strings are operated on using a forth-like operator
	   set. Items at the top of the stack (back()) are the objects
	   operated on except for special cases. New objects are
	   pushed onto the top (back()) and pulled from the top
	   (back()) only. */
    خاص:
      vector<string> پشته_str;
    عام:
      مضمن string برداشت_str(فراغ)
      {
	    أكد(! پشته_str.empty());
	    string val = پشته_str.back();
	    پشته_str.pop_back();
	    أرجع val;
      }
      مضمن فراغ افزودن_str(ثابت string&val)
      {
	    پشته_str.push_back(val);
      }
      مضمن string&peek_str(بیعلامت عمق)
      {
	    أكد(عمق<پشته_str.size());
	    بیعلامت use_شاخص = پشته_str.size()-1-عمق;
	    أرجع پشته_str[use_شاخص];
      }
      مضمن فراغ poke_str(بیعلامت عمق, ثابت string&val)
      {
	    أكد(عمق < پشته_str.size());
	    بیعلامت use_شاخص = پشته_str.size()-1-عمق;
	    پشته_str[use_شاخص] = val;
      }
      مضمن فراغ برداشت_str(بیعلامت cnt)
      {
	    طالما (cnt > 0) {
		  پشته_str.pop_back();
		  cnt -= 1;
	    }
      }

	/* Objects are also operated on in a stack. */
    خاص:
      تعداد { پشته_OBJ_MAX_اندازه = 32 };
      vvp_object_t پشته_obj[پشته_OBJ_MAX_اندازه];
      بیعلامت پشته_obj_اندازه;
    عام:
      مضمن vvp_object_t& peek_object(فراغ)
      {
	    أكد(پشته_obj_اندازه > 0);
	    أرجع پشته_obj[پشته_obj_اندازه-1];
      }
      مضمن فراغ برداشت_object(vvp_object_t&obj)
      {
	    أكد(پشته_obj_اندازه > 0);
	    پشته_obj_اندازه -= 1;
	    obj = پشته_obj[پشته_obj_اندازه];
	    پشته_obj[پشته_obj_اندازه].reset(0);
      }
      مضمن فراغ برداشت_object(بیعلامت cnt, بیعلامت skip =0)
      {
	    أكد((cnt+skip) <= پشته_obj_اندازه);
	    لكل (size_t idx = پشته_obj_اندازه-skip-cnt ; idx < پشته_obj_اندازه-skip ; idx += 1)
		  پشته_obj[idx].reset(0);
	    پشته_obj_اندازه -= cnt;
	    لكل (size_t idx = پشته_obj_اندازه-skip ; idx < پشته_obj_اندازه ; idx += 1)
		  پشته_obj[idx] = پشته_obj[idx+skip];
	    لكل (size_t idx = پشته_obj_اندازه ; idx < پشته_obj_اندازه+skip ; idx += 1)
		  پشته_obj[idx].reset(0);
      }
      مضمن فراغ افزودن_object(ثابت vvp_object_t&obj)
      {
	    أكد(پشته_obj_اندازه < پشته_OBJ_MAX_اندازه);
	    پشته_obj[پشته_obj_اندازه] = obj;
	    پشته_obj_اندازه += 1;
      }

	/* My parent sets this when it wants me to wake it up. */
      بیعلامت i_am_joining      :1;
      بیعلامت i_am_detached     :1;
      بیعلامت i_am_انتظار      :1;
      بیعلامت i_am_درون_function  :1; // True if running function code
      بیعلامت i_have_ended      :1;
      بیعلامت i_was_disabled    :1;
      بیعلامت انتظار_for_event :1;
      بیعلامت is_scheduled      :1;
      بیعلامت delay_delete      :1;
	/* This points to the children of the thread. */
      مجموعه<بنية vthread_s*>children;
	/* This points to the detached children of the thread. */
      مجموعه<بنية vthread_s*>detached_children;
	/* This points to my parent, if I have one. */
      بنية vthread_s*والد;
	/* This points to the containing scope. */
      __vpiScope*والد_scope;
	/* This is used for keeping wait queues. */
      بنية vthread_s*wait_بعدی;
	/* These are used to access automatically allocated items. */
      vvp_context_t wt_context, rd_context;
	/* These are used to pass non-blocking event control information. */
      vvp_net_t*event;
      uint64_t ecount;
	/* Save the file/line information when available. */
    خاص:
      حرف *filenm_;
      بیعلامت lineno_;
    عام:
      فراغ مجموعه_fileline(حرف *filenm, بیعلامت lineno);
      string get_fileline();

      مضمن فراغ cleanup()
      {
	    إذا (i_was_disabled) {
		  پشته_vec4.clear();
		  پشته_real.clear();
		  پشته_str.clear();
		  برداشت_object(پشته_obj_اندازه);
	    }
	    حرر(filenm_);
	    filenm_ = 0;
	    أكد(پشته_vec4.empty());
	    أكد(پشته_real.empty());
	    أكد(پشته_str.empty());
	    أكد(پشته_obj_اندازه == 0);
      }
};

مضمن vthread_s::vthread_s()
{
      پشته_obj_اندازه = 0;
      filenm_ = 0;
      lineno_ = 0;
}

فراغ vthread_s::مجموعه_fileline(حرف *filenm, بیعلامت lineno)
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
      fd << "**** " << label << پایان_سطر;
      fd << "**** ThreadId: " << هذا << ", parent id: " << والد << پایان_سطر;

      fd << "**** Flags: ";
      لكل (صحيح idx = 0 ; idx < FLAGS_شمار ; idx += 1)
	    fd << flags[idx];
      fd << پایان_سطر;
      fd << "**** vec4 stack..." << پایان_سطر;
      لكل (size_t idx = پشته_vec4.size() ; idx > 0 ; idx -= 1)
	    fd << "    " << (پشته_vec4.size()-idx) << ": " << پشته_vec4[idx-1] << پایان_سطر;
      fd << "**** str stack (" << پشته_str.size() << ")..." << پایان_سطر;
      fd << "**** obj stack (" << پشته_obj_اندازه << ")..." << پایان_سطر;
      fd << "**** args_vec4 array (" << آرگومانها_vec4.size() << ")..." << پایان_سطر;
      لكل (size_t idx = 0 ; idx < آرگومانها_vec4.size() ; idx += 1)
	    fd << "    " << idx << ": " << آرگومانها_vec4[idx] << پایان_سطر;
      fd << "**** file/line (";
      إذا (filenm_) fd << filenm_;
      وإلا fd << "<no file name>";
      fd << ":" << lineno_ << ")" << پایان_سطر;
      fd << "**** Done ****" << پایان_سطر;
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
      vector<حرف> tmp (طول_رشته(متن)+1);
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

ساكن فراغ do_join(vthread_t thr, vthread_t فرزند);

__vpiScope* vthread_scope(بنية vthread_s*thr)
{
      أرجع thr->والد_scope;
}

بنية vthread_s*running_thread = 0;

string get_fileline()
{
      أرجع running_thread->get_fileline();
}

فراغ vthread_افزودن(بنية vthread_s*thr, مزدوج val)
{
      thr->افزودن_real(val);
}

فراغ vthread_افزودن(بنية vthread_s*thr, ثابت string&val)
{
      thr->افزودن_str(val);
}

فراغ vthread_افزودن(بنية vthread_s*thr, ثابت vvp_vector4_t&val)
{
      thr->افزودن_vec4(val);
}

فراغ vthread_برداشت_real(بنية vthread_s*thr, بیعلامت عمق)
{
      thr->برداشت_real(عمق);
}

فراغ vthread_برداشت_str(بنية vthread_s*thr, بیعلامت عمق)
{
      thr->برداشت_str(عمق);
}

فراغ vthread_برداشت_vec4(بنية vthread_s*thr, بیعلامت عمق)
{
      thr->برداشت_vec4(عمق);
}

مزدوج vthread_get_real_پشته(بنية vthread_s*thr, بیعلامت عمق)
{
      أرجع thr->peek_real(عمق);
}

ثابت string&vthread_get_str_پشته(بنية vthread_s*thr, بیعلامت عمق)
{
      أرجع thr->peek_str(عمق);
}

ثابت vvp_vector4_t& vthread_get_vec4_پشته(بنية vthread_s*thr, بیعلامت عمق)
{
      أرجع thr->peek_vec4(عمق);
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
قالب <صنف VVP_صف> ساكن vvp_صف*get_صف_object(vthread_t thr, vvp_net_t*net)
{
      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      أكد(obj);

      vvp_صف*صف = obj->get_object().peek<vvp_صف>();
      إذا (صف == 0) {
	    أكد(obj->get_object().test_nil());
	    صف = جديد VVP_صف;
	    vvp_object_t val (صف);
	    vvp_net_ptr_t ptr (net, 0);
	    vvp_send_object(ptr, val, thr->wt_context);
      }

      أرجع صف;
}

/*
 * Locator and size/word opcodes use the same VVP net for a queue ($) or a
 * dynamic array ([]). Queues are vvp_queue_vec4; dynamic arrays may be
 * vvp_darray_vec4, vvp_darray_atom<>, etc. (all vvp_darray, but vvp_queue
 * also derives from vvp_darray — test queue vec4 first).
 */
ساكن فراغ get_صف_or_darray_vec4_from_net(vthread_t thr, vvp_net_t* net,
					      vvp_صف_vec4*& qsrc,
					      vvp_darray*& dsrc)
{
      vvp_fun_signal_object* obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      أكد(obj);
      vvp_object_t بالاترین = obj->get_object();
      qsrc = بالاترین.peek<vvp_صف_vec4>();
      إذا (qsrc) {
	    dsrc = 0;
	    أرجع;
      }
      vvp_darray* dany = بالاترین.peek<vvp_darray>();
      إذا (dany && dynamic_cast<vvp_صف*>(dany) == 0) {
	    dsrc = dany;
	    أرجع;
      }
      إذا (بالاترین.test_nil()) {
	    qsrc = جديد vvp_صف_vec4();
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

ساكن فراغ get_صف_or_darray_real_from_net(vthread_t thr, vvp_net_t* net,
					      vvp_صف_real*& qsrc,
					      vvp_darray*& dsrc)
{
      vvp_fun_signal_object* obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      أكد(obj);
      vvp_object_t بالاترین = obj->get_object();
      qsrc = بالاترین.peek<vvp_صف_real>();
      إذا (qsrc) {
	    dsrc = 0;
	    أرجع;
      }
      vvp_darray* dany = بالاترین.peek<vvp_darray>();
      إذا (dany && dynamic_cast<vvp_صف*>(dany) == 0) {
	    dsrc = dany;
	    أرجع;
      }
      إذا (بالاترین.test_nil()) {
	    qsrc = جديد vvp_صف_real();
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

ساكن فراغ get_صف_or_darray_vec4_from_object(vvp_object_t src_obj,
						 vvp_صف_vec4*& qsrc,
						 vvp_darray*& dsrc)
{
      qsrc = src_obj.peek<vvp_صف_vec4>();
      إذا (qsrc) {
	    dsrc = 0;
	    أرجع;
      }
      vvp_darray* dany = src_obj.peek<vvp_darray>();
      إذا (dany && dynamic_cast<vvp_صف*>(dany) == 0) {
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
مضمن ساكن فراغ برداشت_مقدار(vthread_t thr, مزدوج&value, بیعلامت)
{
      value = thr->برداشت_real();
}

مضمن ساكن فراغ برداشت_مقدار(vthread_t thr, string&value, بیعلامت)
{
      value = thr->برداشت_str();
}

مضمن ساكن فراغ برداشت_مقدار(vthread_t thr, vvp_vector4_t&value, بیعلامت wid)
{
      value = thr->برداشت_vec4();
      أكد(value.size() == wid);
}

/*
 * The following are used to allow the queue templates to print correctly.
 */
مضمن ساكن string get_صف_type(مزدوج&)
{
      أرجع "queue<real>";
}

مضمن ساكن string get_صف_type(string&)
{
      أرجع "queue<string>";
}

مضمن ساكن string get_صف_type(ثابت vvp_vector4_t&value)
{
      ostringstream buf;
      buf << "queue<vector[" << value.size() << "]>";
      string res = buf.str();
      أرجع res;
}

مضمن ساكن فراغ print_صف_مقدار(مزدوج value)
{
      cerr << value;
}

مضمن ساكن فراغ print_صف_مقدار(ثابت string&value)
{
      cerr << "\"" << value << "\"";
}

مضمن ساكن فراغ print_صف_مقدار(ثابت vvp_vector4_t&value)
{
      cerr << value;
}

/*
 * The following are used to get a darray/queue default value.
 */
مضمن ساكن فراغ dq_default(مزدوج&value, بیعلامت)
{
      value = 0.0;
}

مضمن ساكن فراغ dq_default(string&value, بیعلامت)
{
      value = "";
}

مضمن ساكن فراغ dq_default(vvp_vector4_t&value, بیعلامت wid)
{
      value = vvp_vector4_t(wid);
}


قالب <صنف T> T coerce_مقصد_عرض(ثابت T&that, بیعلامت width)
{
      إذا (that.size() == width)
	    أرجع that;

      أكد(that.size() > width);
      T res (width);
      لكل (بیعلامت idx = 0 ;  idx < width ;  idx += 1)
	    res.مجموعه_بیت(idx, that.value(idx));

      أرجع res;
}

/* Explicitly define the vvp_vector4_t version of coerce_to_width(). */
قالب vvp_vector4_t coerce_مقصد_عرض(ثابت vvp_vector4_t&that,
                                       بیعلامت width);


ساكن فراغ multiply_array_imm(بیعلامت دراز*res, ثابت بیعلامت دراز*val,
			       بیعلامت واژگان, بیعلامت دراز imm)
{
      لكل (بیعلامت idx = 0 ; idx < واژگان ; idx += 1)
	    res[idx] = 0;

      لكل (بیعلامت mul_idx = 0 ; mul_idx < واژگان ; mul_idx += 1) {
	    بیعلامت دراز جمع;
	    بیعلامت دراز tmp = multiply_with_انتقال(val[mul_idx], imm, جمع);

	    بیعلامت دراز انتقال = 0;
	    res[mul_idx] = جمع_کن_with_انتقال(res[mul_idx], tmp, انتقال);
	    لكل (بیعلامت جمع_کن_idx = mul_idx+1 ; جمع_کن_idx < واژگان ; جمع_کن_idx += 1) {
		  res[جمع_کن_idx] = جمع_کن_with_انتقال(res[جمع_کن_idx], جمع, انتقال);
		  جمع = 0;
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
            scope->free_contexts = vvp_get_بعدی_context(context);
            لكل (بیعلامت idx = 0 ; idx < scope->nitem ; idx += 1) {
                  scope->قلم[idx]->reset_instance(context);
            }
      } وإلا {
            context = vvp_allocate_context(scope->nitem);
            لكل (بیعلامت idx = 0 ; idx < scope->nitem ; idx += 1) {
                  scope->قلم[idx]->alloc_instance(context);
            }
      }

      vvp_مجموعه_بعدی_context(context, scope->زنده_contexts);
      scope->زنده_contexts = context;

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

      إذا (context == scope->زنده_contexts) {
            scope->زنده_contexts = vvp_get_بعدی_context(context);
      } وإلا {
            vvp_context_t tmp = scope->زنده_contexts;
            طالما (context != vvp_get_بعدی_context(tmp)) {
                  أكد(tmp);
                  tmp = vvp_get_بعدی_context(tmp);
            }
            vvp_مجموعه_بعدی_context(tmp, vvp_get_بعدی_context(context));
      }

      vvp_مجموعه_بعدی_context(context, scope->free_contexts);
      scope->free_contexts = context;
}

#اگر_تعریف_شده CHECK_WITH_VALGRIND
فراغ contexts_delete(صنف __vpiScope*scope)
{
      vvp_context_t context = scope->free_contexts;

      طالما (context) {
	    scope->free_contexts = vvp_get_بعدی_context(context);
	    لكل (بیعلامت idx = 0; idx < scope->nitem; idx += 1) {
		  scope->قلم[idx]->free_instance(context);
	    }
	    حرر(context);
	    context = scope->free_contexts;
      }
      حرر(scope->قلم);
}
#پایان_اگر

/*
 * Create a new thread with the given start address.
 */
vthread_t vthread_new(vvp_کد_t pc, __vpiScope*scope)
{
      vthread_t thr = جديد بنية vthread_s;
      thr->pc     = pc;
	//thr->bits4  = vvp_vector4_t(32);
      thr->والد = 0;
      thr->والد_scope = scope;
      thr->wait_بعدی = 0;
      thr->wt_context = 0;
      thr->rd_context = 0;

      thr->i_am_joining  = 0;
      thr->i_am_detached = 0;
      thr->i_am_انتظار  = 0;
      thr->i_am_درون_function = 0;
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
      لكل (صحيح idx = 4 ; idx < 8 ; idx += 1)
	    thr->flags[idx] = BIT4_X;

      scope->نخها .insert(thr);
      أرجع thr;
}

#اگر_تعریف_شده CHECK_WITH_VALGRIND
#إذا 0
/*
 * These are not currently correct. If you use them you will get
 * double delete messages. There is still a leak related to a
 * waiting event that needs to be investigated.
 */

ساكن فراغ wait_بعدی_delete(vthread_t پایه)
{
      طالما (پایه) {
	    vthread_t tmp = پایه->wait_بعدی;
	    حذف پایه;
	    پایه = tmp;
	    إذا (پایه->انتظار_for_event == 0) اكسر;
      }
}

ساكن فراغ فرزند_delete(vthread_t پایه)
{
      طالما (پایه) {
	    vthread_t tmp = پایه->فرزند;
	    حذف پایه;
	    پایه = tmp;
      }
}
#پایان_اگر

فراغ vthreads_delete(صنف __vpiScope*scope)
{
      لكل (std::مجموعه<vthread_t>::iterator cur = scope->نخها.begin()
		 ; cur != scope->نخها.انتها() ; ++ cur ) {
	    حذف *cur;
      }
      scope->نخها.clear();
}
#پایان_اگر

/*
 * Reaping pulls the thread out of the stack of threads. If I have a
 * child, then hand it over to my parent or fully detach it.
 */
ساكن فراغ vthread_reap(vthread_t thr)
{
      إذا (! thr->children.empty()) {
	    لكل (مجموعه<vthread_t>::iterator cur = thr->children.begin()
		       ; cur != thr->children.انتها() ; ++cur) {
		  vthread_t فرزند = *cur;
		  أكد(فرزند);
		  أكد(فرزند->والد == thr);
		  فرزند->والد = thr->والد;
	    }
      }
      إذا (! thr->detached_children.empty()) {
	    لكل (مجموعه<vthread_t>::iterator cur = thr->detached_children.begin()
		       ; cur != thr->detached_children.انتها() ; ++cur) {
		  vthread_t فرزند = *cur;
		  أكد(فرزند);
		  أكد(فرزند->والد == thr);
		  أكد(فرزند->i_am_detached);
		  فرزند->والد = 0;
		  فرزند->i_am_detached = 0;
	    }
      }
      إذا (thr->والد) {
	      /* assert that the given element was removed. */
	    إذا (thr->i_am_detached) {
		  size_t res = thr->والد->detached_children.erase(thr);
		  أكد(res == 1);
	    } وإلا {
		  size_t res = thr->والد->children.erase(thr);
		  أكد(res == 1);
	    }
      }

      thr->والد = 0;

	// Remove myself from the containing scope if needed.
      thr->والد_scope->نخها.erase(thr);

      thr->pc = codespace_null();

	/* If this thread is not scheduled, then is it safe to delete
	   it now. Otherwise, let the schedule event (which will
	   execute the thread at of_ZOMBIE) delete the object. */
      إذا ((thr->is_scheduled == 0) && (thr->انتظار_for_event == 0)) {
	    أكد(thr->children.empty());
	    أكد(thr->wait_بعدی == 0);
	    إذا (thr->delay_delete)
		  زمانبندی_del_thr(thr);
	    وإلا
		  vthread_delete(thr);
      }
}

فراغ vthread_delete(vthread_t thr)
{
      thr->cleanup();
      حذف thr;
}

فراغ vthread_mark_scheduled(vthread_t thr)
{
      طالما (thr != 0) {
	    أكد(thr->is_scheduled == 0);
	    thr->is_scheduled = 1;
	    thr = thr->wait_بعدی;
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
	    thr->i_am_درون_function = 1;
	    thr = thr->wait_بعدی;
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
فراغ vthread_پیاپی(vthread_t thr)
{
      طالما (thr != 0) {
	    vthread_t tmp = thr->wait_بعدی;
	    thr->wait_بعدی = 0;

	    أكد(thr->is_scheduled);
	    thr->is_scheduled = 0;

            running_thread = thr;

	    لكل (;;) {
		  vvp_کد_t cp = thr->pc;
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
منطقي از_CHUNK_LINK(vthread_t thr, vvp_کد_t code)
{
      أكد(code->cptr);
      thr->pc = code->cptr;
      أرجع درست;
}

/*
 * This is called by an event functor to wake up all the threads on
 * its list. I in fact created that list in the %wait instruction, and
 * I also am certain that the waiting_for_event flag is set.
 */
فراغ vthread_زمانبندی_list(vthread_t thr)
{
      لكل (vthread_t cur = thr ;  cur ;  cur = cur->wait_بعدی) {
	    أكد(cur->انتظار_for_event);
	    cur->انتظار_for_event = 0;
      }

      زمانبندی_vthread(thr, 0);
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

vvp_context_قلم_t vthread_get_wt_context_قلم(بیعلامت context_idx)
{
      أكد(running_thread && running_thread->wt_context);
      أرجع vvp_get_context_قلم(running_thread->wt_context, context_idx);
}

vvp_context_قلم_t vthread_get_rd_context_قلم(بیعلامت context_idx)
{
      أكد(running_thread && running_thread->rd_context);
      أرجع vvp_get_context_قلم(running_thread->rd_context, context_idx);
}

/*
 * %abs/wr
 */
منطقي از_ABS_WR(vthread_t thr, vvp_کد_t)
{
      thr->افزودن_real( fabs(thr->برداشت_real()) );
      أرجع درست;
}

منطقي از_ALLOC(vthread_t thr, vvp_کد_t cp)
{
        /* Allocate a context. */
      vvp_context_t فرزند_context = vthread_alloc_context(cp->scope);

        /* Push the allocated context onto the write context stack. */
      vvp_مجموعه_stacked_context(فرزند_context, thr->wt_context);
      thr->wt_context = فرزند_context;

      أرجع درست;
}

منطقي از_AND(vthread_t thr, vvp_کد_t)
{
      vvp_vector4_t valb = thr->برداشت_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();
      أكد(vala.size() == valb.size());
      vala &= valb;
      أرجع درست;
}

/*
 * This function must ALWAYS be called with the val set to the right
 * size, and initialized with BIT4_0 bits. Certain optimizations rely
 * on that.
 */
ساكن فراغ get_فوری_rval(vvp_کد_t cp, vvp_vector4_t&val)
{
      uint32_t vala = cp->بیت_idx[0];
      uint32_t valb = cp->بیت_idx[1];
      بیعلامت wid  = cp->عدد;

      إذا (valb == 0) {
	      // Special case: if the value is zero, we are done
	      // before we start.
	    إذا (vala == 0) أرجع;

	      // Special case: The value has no X/Z bits, so we can
	      // use the setarray method to write the value all at once.
	    بیعلامت use_wid = 8*حجم(بیعلامت دراز);
	    إذا (wid < use_wid)
		  use_wid = wid;
	    بیعلامت دراز tmp[1];
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

      لكل (بیعلامت idx = 0 ; idx < wid && (vala|valb) ; idx += 1) {
	    uint32_t ba = 0;
	      // Convert the vala/valb bits to a ba number that
	      // matches the encoding of the vvp_bit4_t enumeration.
	    ba = (valb & 1) << 1;
	    ba |= vala & 1;

	      // Note that the val is already pre-filled with BIT4_0
	      // bits, os we only need to set non-zero bit values.
	    إذا (ba) val.مجموعه_بیت(idx, (vvp_bit4_t)ba);

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
منطقي از_جمع_کن(vthread_t thr, vvp_کد_t)
{
      vvp_vector4_t r = thr->برداشت_vec4();
	// Rather then pop l, use it directly from the stack. When we
	// assign to 'l', that will edit the top of the stack, which
	// replaces a pop and a pull.
      vvp_vector4_t&l = thr->peek_vec4();

      l.جمع_کن(r);

      أرجع درست;
}

/*
 * %addi <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
منطقي از_ADDI(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت wid = cp->عدد;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_فوری_rval (cp, r);

      l.جمع_کن(r);

      أرجع درست;
}

/*
 * %add/wr
 */
منطقي از_جمع_کن_WR(vthread_t thr, vvp_کد_t)
{
      مزدوج r = thr->برداشت_real();
      مزدوج l = thr->برداشت_real();
      thr->افزودن_real(l + r);
      أرجع درست;
}

/* %assign/ar <array>, <delay>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory. <delay>
 * is the delay in simulation time. <bit> is the index register
 * containing the real value.
 */
منطقي از_تخصیص_بده_AR(vthread_t thr, vvp_کد_t cp)
{
      دراز adr = thr->واژگان[3].w_int;
      بیعلامت delay = cp->بیت_idx[0];
      مزدوج value = thr->برداشت_real();

      إذا (adr >= 0) {
	    زمانبندی_تخصیص_بده_array_کلمه(cp->array, adr, value, delay);
      }

      أرجع درست;
}

/* %assign/ar/d <array>, <delay_idx>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory.
 * <delay_idx> is the integer register that contains the delay value.
 */
منطقي از_تخصیص_بده_ARD(vthread_t thr, vvp_کد_t cp)
{
      دراز adr = thr->واژگان[3].w_int;
      مزدوج value = thr->برداشت_real();

      إذا (adr >= 0) {
	    vvp_time64_t delay = thr->واژگان[cp->بیت_idx[0]].w_uint;
	    زمانبندی_تخصیص_بده_array_کلمه(cp->array, adr, value, delay);
      }

      أرجع درست;
}

/* %assign/ar/e <array>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory. <bit>
 * is the index register containing the real value. The event
 * information is contained in the thread event control registers
 * and is set with %evctl.
 */
منطقي از_تخصیص_بده_ARE(vthread_t thr, vvp_کد_t cp)
{
      دراز adr = thr->واژگان[3].w_int;
      مزدوج value = thr->برداشت_real();

      إذا (adr >= 0) {
	    إذا (thr->ecount == 0) {
		  زمانبندی_تخصیص_بده_array_کلمه(cp->array, adr, value, 0);
	    } وإلا {
		  زمانبندی_evctl(cp->array, adr, value, thr->event,
		                 thr->ecount);
	    }
      }

      أرجع درست;
}

/*
 * %assign/vec4 <var>, <delay>
 */
منطقي از_تخصیص_بده_VEC4(vthread_t thr, vvp_کد_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      بیعلامت delay = cp->بیت_idx[0];
      ثابت vvp_vector4_t&val = thr->peek_vec4();

      زمانبندی_تخصیص_بده_وکتور(ptr, 0, 0, val, delay);
      thr->برداشت_vec4(1);
      أرجع درست;
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
			    بیعلامت صحيح sig_wid)
{
      بیعلامت صحيح wid = val.size();

        // Fully in bounds, most likely case
      إذا (off >= 0 && (uint64_t)off + wid <= sig_wid)
	    أرجع درست;

      بیعلامت صحيح پایه = 0;
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
	    پایه = -off;
	    wid += off;
		off = 0;
      }

	// If the value is partly above the target, then only assign
	// the bits that overlap
      إذا ((uint64_t)off + wid > sig_wid)
	    wid = sig_wid - (uint64_t)off;

      val = val.subvalue(پایه, wid);

      أرجع درست;
}

/*
 * %assign/vec4/a/d <arr>, <offx>, <delx>
 */
منطقي از_تخصیص_بده_VEC4_A_D(vthread_t thr, vvp_کد_t cp)
{
      صحيح off_idx = cp->بیت_idx[0];
      صحيح del_idx = cp->بیت_idx[1];
      صحيح adr_idx = 3;

      int64_t  off = off_idx ? thr->واژگان[off_idx].w_int : 0;
      vvp_time64_t del = del_idx? thr->واژگان[del_idx].w_uint : 0;
      دراز     adr = thr->واژگان[adr_idx].w_int;

      vvp_vector4_t val = thr->برداشت_vec4();

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      إذا (thr->flags[4] == BIT4_1)
	    أرجع درست;

      إذا (!resize_rval_vec(val, off, cp->array->get_کلمه_اندازه()))
	    أرجع درست;

      زمانبندی_تخصیص_بده_array_کلمه(cp->array, adr, off, val, del);

      أرجع درست;
}

/*
 * %assign/vec4/a/e <arr>, <offx>
 */
منطقي از_تخصیص_بده_VEC4_A_E(vthread_t thr, vvp_کد_t cp)
{
      صحيح off_idx = cp->بیت_idx[0];
      صحيح adr_idx = 3;

      int64_t  off = off_idx ? thr->واژگان[off_idx].w_int : 0;
      دراز     adr = thr->واژگان[adr_idx].w_int;

      vvp_vector4_t val = thr->برداشت_vec4();

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      إذا (thr->flags[4] == BIT4_1)
	    أرجع درست;

      إذا (!resize_rval_vec(val, off, cp->array->get_کلمه_اندازه()))
	    أرجع درست;

      إذا (thr->ecount == 0) {
	    زمانبندی_تخصیص_بده_array_کلمه(cp->array, adr, off, val, 0);
      } وإلا {
	    زمانبندی_evctl(cp->array, adr, val, off, thr->event, thr->ecount);
      }

      أرجع درست;
}

/*
 * %assign/vec4/off/d <var>, <off>, <del>
 */
منطقي از_تخصیص_بده_VEC4_OFF_D(vthread_t thr, vvp_کد_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      بیعلامت off_شاخص = cp->بیت_idx[0];
      بیعلامت del_شاخص = cp->بیت_idx[1];
      vvp_vector4_t val = thr->برداشت_vec4();

      int64_t off = thr->واژگان[off_شاخص].w_int;
      vvp_time64_t del = thr->واژگان[del_شاخص].w_uint;

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      إذا (thr->flags[4] == BIT4_1)
	    أرجع درست;

      vvp_signal_مقدار*sig = dynamic_cast<vvp_signal_مقدار*> (cp->net->fil);
      أكد(sig);

      إذا (!resize_rval_vec(val, off, sig->مقدار_اندازه()))
	    أرجع درست;

      زمانبندی_تخصیص_بده_وکتور(ptr, off, sig->مقدار_اندازه(), val, del);
      أرجع درست;
}

/*
 * %assign/vec4/off/e <var>, <off>
 */
منطقي از_تخصیص_بده_VEC4_OFF_E(vthread_t thr, vvp_کد_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      بیعلامت off_شاخص = cp->بیت_idx[0];
      vvp_vector4_t val = thr->برداشت_vec4();

      int64_t off = thr->واژگان[off_شاخص].w_int;

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      إذا (thr->flags[4] == BIT4_1)
	    أرجع درست;

      vvp_signal_مقدار*sig = dynamic_cast<vvp_signal_مقدار*> (cp->net->fil);
      أكد(sig);

      إذا (!resize_rval_vec(val, off, sig->مقدار_اندازه()))
	    أرجع درست;

      إذا (thr->ecount == 0) {
	    زمانبندی_تخصیص_بده_وکتور(ptr, off, sig->مقدار_اندازه(), val, 0);
      } وإلا {
	    زمانبندی_evctl(ptr, val, off, sig->مقدار_اندازه(), thr->event, thr->ecount);
      }

      أرجع درست;
}

/*
 * %assign/vec4/d <var-label> <delay>
 */
منطقي از_تخصیص_بده_VEC4_D(vthread_t thr, vvp_کد_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      بیعلامت del_شاخص = cp->بیت_idx[0];
      vvp_time64_t del = thr->واژگان[del_شاخص].w_int;

      vvp_vector4_t value = thr->برداشت_vec4();

      vvp_signal_مقدار*sig = dynamic_cast<vvp_signal_مقدار*> (cp->net->fil);
      أكد(sig);

      زمانبندی_تخصیص_بده_وکتور(ptr, 0, sig->مقدار_اندازه(), value, del);

      أرجع درست;
}

/*
 * %assign/vec4/e <var-label>
 */
منطقي از_تخصیص_بده_VEC4_E(vthread_t thr, vvp_کد_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_vector4_t value = thr->برداشت_vec4();

      vvp_signal_مقدار*sig = dynamic_cast<vvp_signal_مقدار*> (cp->net->fil);
      أكد(sig);

      إذا (thr->ecount == 0) {
	    زمانبندی_تخصیص_بده_وکتور(ptr, 0, sig->مقدار_اندازه(), value, 0);
      } وإلا {
	    زمانبندی_evctl(ptr, value, 0, sig->مقدار_اندازه(), thr->event, thr->ecount);
      }

      أرجع درست;
}

/*
 * This is %assign/wr <vpi-label>, <delay>
 *
 * This assigns (after a delay) a value to a real variable. Use the
 * vpi_put_value function to do the assign, with the delay written
 * into the vpiInertialDelay carrying the desired delay.
 */
منطقي از_تخصیص_بده_WR(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت delay = cp->بیت_idx[0];
      مزدوج value = thr->برداشت_real();
      s_vpi_زمان del;

      del.type = vpi_Sim_زمان;
      vpip_زمان_مقصد_timestruct(&del, delay);

      __vpiHandle*tmp = cp->handle;

      t_vpi_مقدار val;
      val.format = vpiRealVal;
      val.value.real = value;
      vpi_put_مقدار(tmp, &val, &del, vpiTransportDelay);

      أرجع درست;
}

منطقي از_تخصیص_بده_WRD(vthread_t thr, vvp_کد_t cp)
{
      vvp_time64_t delay = thr->واژگان[cp->بیت_idx[0]].w_uint;
      مزدوج value = thr->برداشت_real();
      s_vpi_زمان del;

      del.type = vpi_Sim_زمان;
      vpip_زمان_مقصد_timestruct(&del, delay);

      __vpiHandle*tmp = cp->handle;

      t_vpi_مقدار val;
      val.format = vpiRealVal;
      val.value.real = value;
      vpi_put_مقدار(tmp, &val, &del, vpiTransportDelay);

      أرجع درست;
}

منطقي از_تخصیص_بده_WRE(vthread_t thr, vvp_کد_t cp)
{
      أكد(thr->event != 0);
      مزدوج value = thr->برداشت_real();
      __vpiHandle*tmp = cp->handle;

	// If the count is zero then just put the value.
      إذا (thr->ecount == 0) {
	    t_vpi_مقدار val;

	    val.format = vpiRealVal;
	    val.value.real = value;
	    vpi_put_مقدار(tmp, &val, 0, vpiNoDelay);
      } وإلا {
	    زمانبندی_evctl(tmp, value, thr->event, thr->ecount);
      }

      thr->event = 0;
      thr->ecount = 0;

      أرجع درست;
}

منطقي از_BLEND(vthread_t thr, vvp_کد_t)
{
      vvp_vector4_t vala = thr->برداشت_vec4();
      vvp_vector4_t valb = thr->برداشت_vec4();
      أكد(vala.size() == valb.size());

      لكل (بیعلامت idx = 0 ; idx < vala.size() ; idx += 1) {
	    إذا (vala.value(idx) == valb.value(idx))
		  استمر;

	    vala.مجموعه_بیت(idx, BIT4_X);
      }

      thr->افزودن_vec4(vala);
      أرجع درست;
}

منطقي از_BLEND_STR(vthread_t thr, vvp_کد_t)
{
      string f = thr->برداشت_str();
      string t = thr->برداشت_str();
      thr->افزودن_str((t == f) ? t : "");
      أرجع درست;
}

منطقي از_BLEND_WR(vthread_t thr, vvp_کد_t)
{
      مزدوج f = thr->برداشت_real();
      مزدوج t = thr->برداشت_real();
      thr->افزودن_real((t == f) ? t : 0.0);
      أرجع درست;
}

منطقي از_BREAKPOINT(vthread_t, vvp_کد_t)
{
      أرجع درست;
}

/*
 * %callf/void <code-label>, <scope-label>
 * Combine the %fork and %join steps for invoking a function.
 */
ساكن منطقي do_callf_void(vthread_t thr, vthread_t فرزند)
{

      إذا (فرزند->والد_scope->is_automatic()) {
	      /* The context allocated for this child is the top entry
		 on the write context stack */
	    فرزند->wt_context = thr->wt_context;
	    فرزند->rd_context = thr->wt_context;
      }

        // Mark the function thread as a direct child of the current thread.
      فرزند->والد = thr;
      thr->children.insert(فرزند);
        // This should be the only child
      أكد(thr->children.size()==1);

        // Execute the function. This SHOULD run the function to completion,
        // but there are some exceptional situations where it won't.
      أكد(فرزند->والد_scope->get_type_کد() == vpiFunction);
      فرزند->is_scheduled = 1;
      فرزند->i_am_درون_function = 1;
      vthread_پیاپی(فرزند);
      running_thread = thr;

      إذا (فرزند->i_have_ended) {
	    do_join(thr, فرزند);
	    أرجع درست;
      } وإلا {
	    thr->i_am_joining = 1;
	    أرجع خطأ;
      }
}

منطقي از_CALLF_OBJ(vthread_t thr, vvp_کد_t cp)
{
      vthread_t فرزند = vthread_new(cp->cptr2, cp->scope);
      أرجع do_callf_void(thr, فرزند);

      // XXXX NOT IMPLEMENTED
}

منطقي از_CALLF_REAL(vthread_t thr, vvp_کد_t cp)
{
      vthread_t فرزند = vthread_new(cp->cptr2, cp->scope);

	// This is the return value. Push a place-holder value. The function
	// will replace this with the actual value using a %ret/real instruction.
      thr->افزودن_real(0.0);
      فرزند->آرگومانها_real.push_back(0);

      أرجع do_callf_void(thr, فرزند);
}

منطقي از_CALLF_STR(vthread_t thr, vvp_کد_t cp)
{
      vthread_t فرزند = vthread_new(cp->cptr2, cp->scope);

      thr->افزودن_str("");
      فرزند->آرگومانها_str.push_back(0);

      أرجع do_callf_void(thr, فرزند);
}

منطقي از_CALLF_VEC4(vthread_t thr, vvp_کد_t cp)
{
      vthread_t فرزند = vthread_new(cp->cptr2, cp->scope);

      vpiScopeFunction*scope_func = dynamic_cast<vpiScopeFunction*>(cp->scope);
      أكد(scope_func);

	// This is the return value. Push a place-holder value. The function
	// will replace this with the actual value using a %ret/real instruction.
      thr->افزودن_vec4(vvp_vector4_t(scope_func->get_func_عرض(), scope_func->get_func_init_val()));
      فرزند->آرگومانها_vec4.push_back(0);

      أرجع do_callf_void(thr, فرزند);
}

منطقي از_CALLF_VOID(vthread_t thr, vvp_کد_t cp)
{
      vthread_t فرزند = vthread_new(cp->cptr2, cp->scope);
      أرجع do_callf_void(thr, فرزند);
}

/*
 * The %cassign/link instruction connects a source node to a
 * destination node. The destination node must be a signal, as it is
 * marked with the source of the cassign so that it may later be
 * unlinked without specifically knowing the source that this
 * instruction used.
 */
منطقي از_CASSIGN_LINK(vthread_t, vvp_کد_t cp)
{
      vvp_net_t*dst = cp->net;
      vvp_net_t*src = cp->net2;

      vvp_fun_signal_پایه*sig
	    = dynamic_cast<vvp_fun_signal_پایه*>(dst->fun);
      أكد(sig);

	/* Any previous continuous assign should have been removed already. */
      أكد(sig->cassign_link == 0);

      sig->cassign_link = src;

	/* Link the output of the src to the port[1] (the cassign
	   port) of the destination. */
      vvp_net_ptr_t dst_ptr (dst, 1);
      src->link(dst_ptr);

      أرجع درست;
}

/*
 * If there is an existing continuous assign linked to the destination
 * node, unlink it. This must be done before applying a new continuous
 * assign, otherwise the initial assigned value will be propagated to
 * any other nodes driven by the old continuous assign source.
 */
ساكن فراغ cassign_unlink(vvp_net_t*dst)
{
      vvp_fun_signal_پایه*sig
	    = dynamic_cast<vvp_fun_signal_پایه*>(dst->fun);
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
منطقي از_CASSIGN_VEC4(vthread_t thr, vvp_کد_t cp)
{
      vvp_net_t*net = cp->net;
      vvp_vector4_t value = thr->برداشت_vec4();

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

	/* Set the value into port 1 of the destination. */
      vvp_net_ptr_t ptr (net, 1);
      vvp_send_vec4(ptr, value, 0);

      أرجع درست;
}

/*
 * %cassign/vec4/off <var>, <off>
 */
منطقي از_CASSIGN_VEC4_OFF(vthread_t thr, vvp_کد_t cp)
{
      vvp_net_t*net = cp->net;
      بیعلامت پایه_idx = cp->بیت_idx[0];
      دراز پایه = thr->واژگان[پایه_idx].w_int;
      vvp_vector4_t value = thr->برداشت_vec4();
      بیعلامت wid = value.size();

      إذا (thr->flags[4] == BIT4_1)
	    أرجع درست;

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

      vvp_signal_مقدار*sig = dynamic_cast<vvp_signal_مقدار*> (net->fil);
      أكد(sig);

      إذا (پایه < 0 && (wid <= (بیعلامت)-پایه))
	    أرجع درست;

      إذا (پایه >= (دراز)sig->مقدار_اندازه())
	    أرجع درست;

      إذا (پایه < 0) {
	    wid -= (بیعلامت) -پایه;
	    پایه = 0;
	    value.resize(wid);
      }

      إذا (پایه+wid > sig->مقدار_اندازه()) {
	    wid = sig->مقدار_اندازه() - پایه;
	    value.resize(wid);
      }

      vvp_net_ptr_t ptr (net, 1);
      vvp_send_vec4_pv(ptr, value, پایه, sig->مقدار_اندازه(), 0);
      أرجع درست;
}

منطقي از_CASSIGN_WR(vthread_t thr, vvp_کد_t cp)
{
      vvp_net_t*net  = cp->net;
      مزدوج value = thr->برداشت_real();

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

	/* Set the value into port 1 of the destination. */
      vvp_net_ptr_t ptr (net, 1);
      vvp_send_real(ptr, value, 0);

      أرجع درست;
}

/*
 * %cast2
 */
منطقي از_CAST2(vthread_t thr, vvp_کد_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      بیعلامت wid = val.size();

      لكل (بیعلامت idx = 0 ; idx < wid ; idx += 1) {
	    بدل (val.value(idx)) {
		حالة BIT4_0:
		حالة BIT4_1:
		  اكسر;
		افتراضي:
		  val.مجموعه_بیت(idx, BIT4_0);
		  اكسر;
	    }
      }

      أرجع درست;
}

منطقي do_cast_vec_dar(vthread_t thr, vvp_کد_t cp, منطقي as_vec4)
{
      بیعلامت wid = cp->عدد;

      vvp_object_t obj;
      thr->برداشت_object(obj);

      vvp_darray*darray = obj.peek<vvp_darray>();
      أكد(darray);

      vvp_vector4_t vec = darray->get_bitstream(as_vec4);
      إذا (vec.size() != wid) {
	    cerr << thr->get_fileline()
	         << "VVP error: size mismatch when casting dynamic array to vector." << پایان_سطر;
            thr->افزودن_vec4(vvp_vector4_t(wid));
            زمانبندی_stop(0);
            أرجع خطأ;
      }
      thr->افزودن_vec4(vec);
      أرجع درست;
}

/*
 * %cast/vec2/dar <wid>
 */
منطقي از_CAST_VEC2_DAR(vthread_t thr, vvp_کد_t cp)
{
      أرجع do_cast_vec_dar(thr, cp, خطأ);
}

/*
 * %cast/vec4/dar <wid>
 */
منطقي از_CAST_VEC4_DAR(vthread_t thr, vvp_کد_t cp)
{
      أرجع do_cast_vec_dar(thr, cp, درست);
}

/*
 * %cast/vec4/str <wid>
 */
منطقي از_CAST_VEC4_STR(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت wid = cp->عدد;
      string str = thr->برداشت_str();

      vvp_vector4_t vec(wid, BIT4_0);

      إذا (wid != 8*str.length()) {
	    cerr << thr->get_fileline()
	         << "VVP error: size mismatch when casting string to vector." << پایان_سطر;
            thr->افزودن_vec4(vec);
            زمانبندی_stop(0);
            أرجع خطأ;
      }

      بیعلامت sdx = 0;
      بیعلامت vdx = wid;
      طالما (vdx > 0) {
            حرف ch = str[sdx++];
            vdx -= 8;
            لكل (بیعلامت bdx = 0; bdx < 8; bdx += 1) {
                  إذا (ch & 1)
                        vec.مجموعه_بیت(vdx+bdx, BIT4_1);
                  ch >>= 1;
            }
      }

      thr->افزودن_vec4(vec);
      أرجع درست;
}

ساكن فراغ do_CMPE(vthread_t thr, ثابت vvp_vector4_t&lval, ثابت vvp_vector4_t&rval)
{
      أكد(rval.size() == lval.size());

      إذا (lval.has_xz() || rval.has_xz()) {

	    بیعلامت wid = lval.size();
	    vvp_bit4_t eq  = BIT4_1;
	    vvp_bit4_t eeq = BIT4_1;

	    لكل (بیعلامت idx = 0 ; idx < wid ; idx += 1) {
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
منطقي از_CMPE(vthread_t thr, vvp_کد_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ثابت vvp_vector4_t&rval = thr->peek_vec4(0);
      ثابت vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPE(thr, lval, rval);

      thr->برداشت_vec4(2);
      أرجع درست;
}

منطقي از_CMPNE(vthread_t thr, vvp_کد_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ثابت vvp_vector4_t&rval = thr->peek_vec4(0);
      ثابت vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];
      thr->flags[6] =  ~thr->flags[6];

      thr->برداشت_vec4(2);
      أرجع درست;
}

/*
 * %cmpi/e <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
منطقي از_CMPIE(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت wid = cp->عدد;

      ثابت vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_فوری_rval (cp, rval);

      do_CMPE(thr, lval, rval);

      thr->برداشت_vec4(1);
      أرجع درست;
}

منطقي از_CMPINE(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت wid = cp->عدد;

      ثابت vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_فوری_rval (cp, rval);

      do_CMPE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];
      thr->flags[6] =  ~thr->flags[6];

      thr->برداشت_vec4(1);
      أرجع درست;
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
      بیعلامت wid = lval.size();

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

      لكل (بیعلامت idx = 1 ;  idx < wid ;  idx += 1) {
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
منطقي از_CMPS(vthread_t thr, vvp_کد_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ثابت vvp_vector4_t&rval = thr->peek_vec4(0);
      ثابت vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPS(thr, lval, rval);

      thr->برداشت_vec4(2);
      أرجع درست;
}

/*
 * %cmpi/s <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
منطقي از_CMPIS(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت wid = cp->عدد;

      ثابت vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_فوری_rval (cp, rval);

      do_CMPS(thr, lval, rval);

      thr->برداشت_vec4(1);
      أرجع درست;
}

منطقي از_CMPSTR(vthread_t thr, vvp_کد_t)
{
      string re = thr->برداشت_str();
      string le = thr->برداشت_str();

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

      أرجع درست;
}

ساكن فراغ از_CMPU_the_hard_way(vthread_t thr, بیعلامت wid,
				 ثابت vvp_vector4_t&lval,
				 ثابت vvp_vector4_t&rval)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_bit4_t eeq = BIT4_1;

      لكل (بیعلامت idx = 0 ;  idx < wid ;  idx += 1) {
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
		 << ", rval=" << rval << پایان_سطر;
      }
      أكد(rval.size() == lval.size());
      بیعلامت wid = lval.size();

      بیعلامت دراز*larray = lval.subarray(0,wid);
      إذا (larray == 0) أرجع از_CMPU_the_hard_way(thr, wid, lval, rval);

      بیعلامت دراز*rarray = rval.subarray(0,wid);
      إذا (rarray == 0) {
	    حذف[]larray;
	    أرجع از_CMPU_the_hard_way(thr, wid, lval, rval);
      }

      بیعلامت واژگان = (wid+CPU_کلمه_بیتها-1) / CPU_کلمه_بیتها;

      لكل (بیعلامت wdx = 0 ; wdx < واژگان ; wdx += 1) {
	    إذا (larray[wdx] == rarray[wdx])
		  استمر;

	    eq = BIT4_0;
	    إذا (larray[wdx] < rarray[wdx])
		  lt = BIT4_1;
	    وإلا
		  lt = BIT4_0;
      }

      حذف[]larray;
      حذف[]rarray;

      thr->flags[4] = eq;
      thr->flags[5] = lt;
      thr->flags[6] = eq;
}

منطقي از_CMPU(vthread_t thr, vvp_کد_t)
{

      ثابت vvp_vector4_t&rval = thr->peek_vec4(0);
      ثابت vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPU(thr, lval, rval);

      thr->برداشت_vec4(2);
      أرجع درست;
}

/*
 * %cmpi/u <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
منطقي از_CMPIU(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت wid = cp->عدد;

      ثابت vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_فوری_rval (cp, rval);

      do_CMPU(thr, lval, rval);

      thr->برداشت_vec4(1);
      أرجع درست;
}


/*
 * %cmp/x
 */
منطقي از_CMPX(vthread_t thr, vvp_کد_t)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_vector4_t rval = thr->برداشت_vec4();
      vvp_vector4_t lval = thr->برداشت_vec4();

      أكد(rval.size() == lval.size());
      بیعلامت wid = lval.size();

      لكل (بیعلامت idx = 0 ; idx < wid ; idx += 1) {
	    vvp_bit4_t lv = lval.value(idx);
	    vvp_bit4_t rv = rval.value(idx);
	    إذا ((lv != rv) && !bit4_is_xz(lv) && !bit4_is_xz(rv)) {
		  eq = BIT4_0;
		  اكسر;
	    }
      }

      thr->flags[4] = eq;
      أرجع درست;
}

ساكن فراغ do_CMPWE(vthread_t thr, ثابت vvp_vector4_t&lval, ثابت vvp_vector4_t&rval)
{
      أكد(rval.size() == lval.size());

      إذا (lval.has_xz() || rval.has_xz()) {

	    بیعلامت wid = lval.size();
	    vvp_bit4_t eq  = BIT4_1;

	    لكل (بیعلامت idx = 0 ; idx < wid ; idx += 1) {
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

منطقي از_CMPWE(vthread_t thr, vvp_کد_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ثابت vvp_vector4_t&rval = thr->peek_vec4(0);
      ثابت vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPWE(thr, lval, rval);

      thr->برداشت_vec4(2);
      أرجع درست;
}

منطقي از_CMPWNE(vthread_t thr, vvp_کد_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ثابت vvp_vector4_t&rval = thr->peek_vec4(0);
      ثابت vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPWE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];

      thr->برداشت_vec4(2);
      أرجع درست;
}

منطقي از_CMPWR(vthread_t thr, vvp_کد_t)
{
      مزدوج r = thr->برداشت_real();
      مزدوج l = thr->برداشت_real();

      vvp_bit4_t eq = (l == r)? BIT4_1 : BIT4_0;
      vvp_bit4_t lt = (l <  r)? BIT4_1 : BIT4_0;

      thr->flags[4] = eq;
      thr->flags[5] = lt;

      أرجع درست;
}

/*
 * %cmp/z
 */
منطقي از_CMPZ(vthread_t thr, vvp_کد_t)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_vector4_t rval = thr->برداشت_vec4();
      vvp_vector4_t lval = thr->برداشت_vec4();

      أكد(rval.size() == lval.size());
      بیعلامت wid = lval.size();

      لكل (بیعلامت idx = 0 ; idx < wid ; idx += 1) {
	    vvp_bit4_t lv = lval.value(idx);
	    vvp_bit4_t rv = rval.value(idx);
	    إذا ((lv != rv) && (rv != BIT4_Z) && (lv != BIT4_Z)) {
		  eq = BIT4_0;
		  اكسر;
	    }
      }

      thr->flags[4] = eq;
      أرجع درست;
}

/*
 *  %concat/str;
 */
منطقي از_CONCAT_STR(vthread_t thr, vvp_کد_t)
{
      string متن = thr->برداشت_str();
      thr->peek_str(0).append(متن);
      أرجع درست;
}

/*
 *  %concati/str <string>;
 */
منطقي از_CONCATI_STR(vthread_t thr, vvp_کد_t cp)
{
      ثابت حرف*متن = cp->متن;
      thr->peek_str(0).append(filter_string(متن));
      أرجع درست;
}

/*
 * %concat/vec4
 */
منطقي از_CONCAT_VEC4(vthread_t thr, vvp_کد_t)
{
      ثابت vvp_vector4_t&lsb = thr->peek_vec4(0);
      ثابت vvp_vector4_t&msb = thr->peek_vec4(1);

	// The result is the size of the top two vectors in the stack.
      vvp_vector4_t res (msb.size() + lsb.size(), BIT4_X);

	// Build up the result.
      res.مجموعه_vec(0, lsb);
      res.مجموعه_vec(lsb.size(), msb);

	// Rearrange the stack to pop the inputs and push the
	// result. Do that by actually popping only 1 stack position
	// and replacing the new top with the new value.
      thr->برداشت_vec4(1);
      thr->peek_vec4() = res;

      أرجع درست;
}

/*
 * %concati/vec4 <vala>, <valb>, <wid>
 *
 * Concat the immediate value to the LOW bits of the concatenation.
 * Get the HIGH bits from the top of the vec4 stack.
 */
منطقي از_CONCATI_VEC4(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت wid  = cp->عدد;

      vvp_vector4_t&msb = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t lsb (wid, BIT4_0);
      get_فوری_rval (cp, lsb);

      vvp_vector4_t res (msb.size()+lsb.size(), BIT4_X);
      res.مجموعه_vec(0, lsb);
      res.مجموعه_vec(lsb.size(), msb);

      msb = res;
      أرجع درست;
}

/*
 * %cvt/rv
 */
منطقي از_CVT_RV(vthread_t thr, vvp_کد_t)
{
      مزدوج val;
      vvp_vector4_t val4 = thr->برداشت_vec4();
      vector4_مقصد_مقدار(val4, val, خطأ);
      thr->افزودن_real(val);
      أرجع درست;
}

/*
 * %cvt/rv/s
 */
منطقي از_CVT_RV_S(vthread_t thr, vvp_کد_t)
{
      مزدوج val;
      vvp_vector4_t val4 = thr->برداشت_vec4();
      vector4_مقصد_مقدار(val4, val, درست);
      thr->افزودن_real(val);
      أرجع درست;
}

/*
 * %cvt/sr <idx>
 * Pop the top value from the real stack, convert it to a 64bit signed
 * and save it to the indexed register.
 */
منطقي از_CVT_SR(vthread_t thr, vvp_کد_t cp)
{
      مزدوج r = thr->برداشت_real();
      thr->واژگان[cp->بیت_idx[0]].w_int = i64round(r);

      أرجع درست;
}

/*
 * %cvt/ur <idx>
 */
منطقي از_CVT_UR(vthread_t thr, vvp_کد_t cp)
{
      مزدوج r = thr->برداشت_real();
      thr->واژگان[cp->بیت_idx[0]].w_uint = vlg_دور_مقصد_u64(r);

      أرجع درست;
}

/*
 * %cvt/vr <wid>
 */
منطقي از_CVT_VR(vthread_t thr, vvp_کد_t cp)
{
      مزدوج r = thr->برداشت_real();
      بیعلامت wid = cp->عدد;

      vvp_vector4_t tmp(wid, r);
      thr->افزودن_vec4(tmp);
      أرجع درست;
}

/*
 * This implements the %deassign instruction. All we do is write a
 * long(1) to port-3 of the addressed net. This turns off an active
 * continuous assign activated by %cassign/v
 */
منطقي از_DEASSIGN(vthread_t, vvp_کد_t cp)
{
      vvp_net_t*net = cp->net;
      بیعلامت پایه  = cp->بیت_idx[0];
      بیعلامت width = cp->بیت_idx[1];

      vvp_signal_مقدار*fil = dynamic_cast<vvp_signal_مقدار*> (net->fil);
      أكد(fil);
      vvp_fun_signal_vec*sig = dynamic_cast<vvp_fun_signal_vec*>(net->fun);
      أكد(sig);

      إذا (پایه >= fil->مقدار_اندازه()) أرجع درست;
      إذا (پایه+width > fil->مقدار_اندازه()) width = fil->مقدار_اندازه() - پایه;

      منطقي پر_sig = پایه == 0 && width == fil->مقدار_اندازه();

	// This is the net that is forcing me...
      إذا (vvp_net_t*src = sig->cassign_link) {
	    إذا (! پر_sig) {
		  fprintf(stderr, "Sorry: when a signal is assigning a "
		          "register, I cannot deassign part of it.\n");
		  خروج(1);
	    }
	      // And this is the pointer to be removed.
	    vvp_net_ptr_t dst_ptr (net, 1);
	    src->unlink(dst_ptr);
	    sig->cassign_link = 0;
      }

	/* Do we release all or part of the net? */
      إذا (پر_sig) {
	    sig->deassign();
      } وإلا {
	    sig->deassign_pv(پایه, width);
      }

      أرجع درست;
}

منطقي از_DEASSIGN_WR(vthread_t, vvp_کد_t cp)
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

      أرجع درست;
}

/*
 * %debug/thr
 */
منطقي از_DEBUG_THR(vthread_t thr, vvp_کد_t cp)
{
      ثابت حرف*متن = cp->متن;
      thr->debug_dump(cerr, متن);
      أرجع درست;
}

/*
 * The delay takes two 32bit numbers to make up a 64bit time.
 *
 *   %delay <low>, <hig>
 */
منطقي از_DELAY(vthread_t thr, vvp_کد_t cp)
{
      vvp_time64_t کم = cp->بیت_idx[0];
      vvp_time64_t hig = cp->بیت_idx[1];

      vvp_time64_t delay = (hig << 32) | کم;

      إذا (delay == 0) زمانبندی_inactive(thr);
      وإلا زمانبندی_vthread(thr, delay);
      أرجع خطأ;
}

منطقي از_DELAYX(vthread_t thr, vvp_کد_t cp)
{
      vvp_time64_t delay;

      أكد(cp->عدد < vthread_s::واژگان_شمار);
      delay = thr->واژگان[cp->عدد].w_uint;
      إذا (delay == 0) زمانبندی_inactive(thr);
      وإلا زمانبندی_vthread(thr, delay);
      أرجع خطأ;
}

منطقي از_DELETE_ELEM(vthread_t thr, vvp_کد_t cp)
{
      vvp_net_t*net = cp->net;

      int64_t idx_val = thr->واژگان[3].w_int;
      إذا (thr->flags[4] == BIT4_1) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with undefined index."
	         << پایان_سطر;
	    أرجع درست;
      }
      إذا (idx_val < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with negative index."
	         << پایان_سطر;
	    أرجع درست;
      }
      size_t idx = idx_val;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      أكد(obj);

      vvp_صف*صف = obj->get_object().peek<vvp_صف>();
      إذا (صف == 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping delete(" << idx
	         << ") on empty queue." << پایان_سطر;
      } وإلا {
	    size_t size = صف->get_اندازه();
	    إذا (idx >= size) {
		  cerr << thr->get_fileline()
		       << "Warning: skipping out of range delete(" << idx
		       << ") on queue of size " << size << "." << پایان_سطر;
	    } وإلا {
		  صف->erase(idx);
	    }
      }

      أرجع درست;
}

/* %delete/obj <label>
 *
 * This operator works by assigning a nil to the target object. This
 * causes any value that might be there to be garbage collected, thus
 * deleting the object.
 */
منطقي از_DELETE_OBJ(vthread_t thr, vvp_کد_t cp)
{
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_send_object(ptr, vvp_object_t(), thr->wt_context);

      أرجع درست;
}

/* %reverse/obj <label>
 * Reverse element order of the queue or dynamic array object at this net.
 */
منطقي از_معکوس_OBJ(vthread_t /*thr*/, vvp_کد_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      أكد(obj);

      vvp_object_t stor = obj->get_object();
      إذا (stor.test_nil()) أرجع درست;

	/* Match other queue ops: peek queues before vvp_darray so concrete
	   queue types dispatch reverse_elems() correctly. */
      إذا (vvp_صف* صف = stor.peek<vvp_صف>()) {
	    صف->معکوس_elems();
	    أرجع درست;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      أكد(darray);
      darray->معکوس_elems();

      أرجع درست;
}

/* %reverse/prop/obj <property-index>
 * Reverse queue/darray property storage inside the class object on stack top.
 */
منطقي از_معکوس_PROP_OBJ(vthread_t thr, vvp_کد_t cp)
{
      size_t pid = cp->عدد;

      vvp_object_t& بالاترین = thr->peek_object();
      vvp_cobject*cobj = بالاترین.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      إذا (arr_obj.test_nil()) أرجع درست;

      إذا (vvp_صف* صف = arr_obj.peek<vvp_صف>()) {
	    صف->معکوس_elems();
	    أرجع درست;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      أكد(darray);
      darray->معکوس_elems();

      أرجع درست;
}

/* %sort/obj — ascending sort */
منطقي از_مرتب_سازی_OBJ(vthread_t /*thr*/, vvp_کد_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      أكد(obj);

      vvp_object_t stor = obj->get_object();
      إذا (stor.test_nil()) أرجع درست;

      إذا (vvp_صف* صف = stor.peek<vvp_صف>()) {
	    صف->مرتب_سازی_elems(درست);
	    أرجع درست;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      أكد(darray);
      darray->مرتب_سازی_elems(درست);

      أرجع درست;
}

/* %sort/prop/obj */
منطقي از_مرتب_سازی_PROP_OBJ(vthread_t thr, vvp_کد_t cp)
{
      size_t pid = cp->عدد;

      vvp_object_t& بالاترین = thr->peek_object();
      vvp_cobject*cobj = بالاترین.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      إذا (arr_obj.test_nil()) أرجع درست;

      إذا (vvp_صف* صف = arr_obj.peek<vvp_صف>()) {
	    صف->مرتب_سازی_elems(درست);
	    أرجع درست;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      أكد(darray);
      darray->مرتب_سازی_elems(درست);

      أرجع درست;
}

/* %rsort/obj — descending sort */
منطقي از_RSORT_OBJ(vthread_t /*thr*/, vvp_کد_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      أكد(obj);

      vvp_object_t stor = obj->get_object();
      إذا (stor.test_nil()) أرجع درست;

      إذا (vvp_صف* صف = stor.peek<vvp_صف>()) {
	    صف->مرتب_سازی_elems(خطأ);
	    أرجع درست;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      أكد(darray);
      darray->مرتب_سازی_elems(خطأ);

      أرجع درست;
}

/* %rsort/prop/obj */
منطقي از_RSORT_PROP_OBJ(vthread_t thr, vvp_کد_t cp)
{
      size_t pid = cp->عدد;

      vvp_object_t& بالاترین = thr->peek_object();
      vvp_cobject*cobj = بالاترین.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      إذا (arr_obj.test_nil()) أرجع درست;

      إذا (vvp_صف* صف = arr_obj.peek<vvp_صف>()) {
	    صف->مرتب_سازی_elems(خطأ);
	    أرجع درست;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      أكد(darray);
      darray->مرتب_سازی_elems(خطأ);

      أرجع درست;
}

/* %shuffle/obj */
منطقي از_SHUFFLE_OBJ(vthread_t /*thr*/, vvp_کد_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      أكد(obj);

      vvp_object_t stor = obj->get_object();
      إذا (stor.test_nil()) أرجع درست;

      إذا (vvp_صف* صف = stor.peek<vvp_صف>()) {
	    صف->shuffle_elems();
	    أرجع درست;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      أكد(darray);
      darray->shuffle_elems();

      أرجع درست;
}

/* %shuffle/prop/obj */
منطقي از_SHUFFLE_PROP_OBJ(vthread_t thr, vvp_کد_t cp)
{
      size_t pid = cp->عدد;

      vvp_object_t& بالاترین = thr->peek_object();
      vvp_cobject*cobj = بالاترین.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      إذا (arr_obj.test_nil()) أرجع درست;

      إذا (vvp_صف* صف = arr_obj.peek<vvp_صف>()) {
	    صف->shuffle_elems();
	    أرجع درست;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      أكد(darray);
      darray->shuffle_elems();

      أرجع درست;
}

/* %delete/tail <label>, idx
 *
 * Remove all elements after the one specified.
 */
منطقي از_DELETE_دم(vthread_t thr, vvp_کد_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      أكد(obj);

      vvp_صف*صف = obj->get_object().peek<vvp_صف>();
      أكد(صف);

      بیعلامت idx = thr->واژگان[cp->بیت_idx[0]].w_int;
      صف->erase_دم(idx);

      أرجع درست;
}

ساكن منطقي do_disable(vthread_t thr, vthread_t تطابق)
{
      منطقي flag = خطأ;

	/* Pull the target thread out of its scope if needed. */
      thr->والد_scope->نخها.erase(thr);

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
	    أكد(tmp->والد == thr);
	    thr->i_am_joining = 0;
	    إذا (do_disable(tmp, تطابق))
		  flag = درست;

	    vthread_reap(tmp);
      }

      vthread_t والد = thr->والد;
      إذا (والد && والد->i_am_joining) {
	      // If a parent is waiting in a %join, wake it up. Note
	      // that it is possible to be waiting in a %join yet
	      // already scheduled if multiple child threads are
	      // ending. So check if the thread is already scheduled
	      // before scheduling it again.
	    والد->i_am_joining = 0;
	    إذا (! والد->i_have_ended)
		  زمانبندی_vthread(والد, 0, درست);

	    do_join(والد, thr);

      } وإلا إذا (والد) {
	      /* If the parent is yet to %join me, let its %join
		 do the reaping. */
	      //assert(tmp->is_scheduled == 0);

      } وإلا {
	      /* No parent at all. Goodbye. */
	    vthread_reap(thr);
      }

      أرجع flag || (thr == تطابق);
}

/*
 * Implement the %disable instruction by scanning the target scope for
 * all the target threads. Kill the target threads and wake up a
 * parent that is attempting a %join.
 */
منطقي از_DISABLE(vthread_t thr, vvp_کد_t cp)
{
      __vpiScope*scope = static_cast<__vpiScope*>(cp->handle);

      منطقي disabled_myself_flag = خطأ;

      طالما (! scope->نخها.empty()) {
	    مجموعه<vthread_t>::iterator cur = scope->نخها.begin();

	    إذا (do_disable(*cur, thr))
		  disabled_myself_flag = درست;
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

منطقي از_DISABLE_FLOW(vthread_t thr, vvp_کد_t cp)
{
      ثابت __vpiScope*scope = static_cast<__vpiScope*>(cp->handle);
      vthread_t cur = thr;

      طالما (cur && cur->والد_scope != scope)
	    cur = cur->والد;

      أكد(cur);
      إذا (cur)
	    أرجع !do_disable(cur, thr);

      أرجع خطأ;
}

/*
 * Implement the %disable/fork (SystemVerilog) instruction by disabling
 * all the detached children of the given thread.
 */
منطقي از_DISABLE_FORK(vthread_t thr, vvp_کد_t)
{
	/* If a %disable/fork is being executed then the parent thread
	 * cannot be waiting in a join. */
      أكد(! thr->i_am_joining);

	/* There should be no active children to disable. */
      أكد(thr->children.empty());

	/* Disable any detached children. */
      طالما (! thr->detached_children.empty()) {
	    vthread_t فرزند = *(thr->detached_children.begin());
	    أكد(فرزند);
	    أكد(فرزند->والد == thr);
	      /* Disabling the children can never match the parent thread. */
	    منطقي res = do_disable(فرزند, thr);
	    أكد(! res);
	    vthread_reap(فرزند);
      }

      أرجع درست;
}

/*
 * This function divides a 2-word number {high, a} by a 1-word
 * number. Assume that high < b.
 */
ساكن بیعلامت دراز divide2words(بیعلامت دراز a, بیعلامت دراز b,
				  بیعلامت دراز بالا)
{
      بیعلامت دراز نتیجه = 0;
      طالما (بالا > 0) {
	    بیعلامت دراز tmp_نتیجه = ULONG_MAX / b;
	    بیعلامت دراز remain = ULONG_MAX % b;

	    remain += 1;
	    إذا (remain >= b) {
		  remain -= b;
		  tmp_نتیجه += 1;
	    }

	      // Now 0x1_0...0 = b*tmp_result + remain
	      // high*0x1_0...0 = high*(b*tmp_result + remain)
	      // high*0x1_0...0 = high*b*tmp_result + high*remain

	      // We know that high*0x1_0...0 >= high*b*tmp_result, and
	      // we know that high*0x1_0...0 > high*remain. Use
	      // high*remain as the remainder for another iteration,
	      // and add tmp_result*high into the current estimate of
	      // the result.
	    نتیجه += tmp_نتیجه * بالا;

	      // The new iteration starts with high*remain + a.
	    remain = multiply_with_انتقال(بالا, remain, بالا);
	    a += remain;
            إذا(a < remain)
              بالا += 1;

	      // Now result*b + {high,a} == the input {high,a}. It is
	      // possible that the new high >= 1. If so, it will
	      // certainly be less than high from the previous
	      // iteration. Do another iteration and it will shrink,
	      // eventually to 0.
      }

	// high is now 0, so a is the remaining remainder, so we can
	// finish off the integer divide with a simple a/b.

      أرجع نتیجه + a/b;
}

ساكن بیعلامت دراز* divide_بیتها(بیعلامت دراز*ap, ثابت بیعلامت دراز*bp, بیعلامت wid)
{
	// Do all our work a cpu-word at a time. The "words" variable
	// is the number of words of the wid.
      بیعلامت واژگان = (wid+CPU_کلمه_بیتها-1) / CPU_کلمه_بیتها;

      بیعلامت btop = واژگان-1;
      طالما (btop > 0 && bp[btop] == 0)
	    btop -= 1;

	// Detect divide by 0, and exit.
      إذا (btop==0 && bp[0]==0)
	    أرجع 0;

	// The result array will eventually accumulate the result. The
	// diff array is a difference that we use in the intermediate.
      بیعلامت دراز*diff  = جديد بیعلامت دراز[واژگان];
      بیعلامت دراز*نتیجه= جديد بیعلامت دراز[واژگان];
      لكل (بیعلامت idx = 0 ; idx < واژگان ; idx += 1)
	    نتیجه[idx] = 0;

      لكل (بیعلامت cur = واژگان-btop ; cur > 0 ; cur -= 1) {
	    بیعلامت cur_ptr = cur-1;
	    بیعلامت دراز cur_res;
	    إذا (ap[cur_ptr+btop] >= bp[btop]) {
		  بیعلامت دراز بالا = 0;
		  إذا (cur_ptr+btop+1 < واژگان)
			بالا = ap[cur_ptr+btop+1];
		  cur_res = divide2words(ap[cur_ptr+btop], bp[btop], بالا);

	    } وإلا إذا (cur_ptr+btop+1 >= واژگان) {
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
	    multiply_array_imm(diff+cur_ptr, bp, واژگان-cur_ptr, cur_res);
	      // ap -= diff
	    بیعلامت دراز انتقال = 1;
	    لكل (بیعلامت idx = cur_ptr ; idx < واژگان ; idx += 1)
		  ap[idx] = جمع_کن_with_انتقال(ap[idx], ~diff[idx], انتقال);

	      // ap has the diff subtracted out of it. If cur_res was
	      // too large, then ap will turn negative. (We easily
	      // tell that ap turned negative by looking at
	      // carry&1. If it is 0, then it is *negative*.) In that
	      // case, we know that cur_res was too large by 1. Correct by
	      // adding 1b back in and reducing cur_res.
	    إذا ((انتقال&1) == 0) {
		    // Keep adding b back in until the remainder
		    // becomes positive again.
		  افعل {
			cur_res -= 1;
			انتقال = 0;
			لكل (بیعلامت idx = cur_ptr ; idx < واژگان ; idx += 1)
			      ap[idx] = جمع_کن_with_انتقال(ap[idx], bp[idx-cur_ptr], انتقال);
		  } طالما (انتقال == 0);
	    }

	    نتیجه[cur_ptr] = cur_res;
      }

	// Now ap contains the remainder and result contains the
	// desired result. We should find that:
	//  input-a = bp * result + ap;

      حذف[]diff;
      أرجع نتیجه;
}

/*
 * %div
 */
منطقي از_DIV(vthread_t thr, vvp_کد_t)
{
      vvp_vector4_t valb = thr->برداشت_vec4();
      vvp_vector4_t vala = thr->برداشت_vec4();

      أكد(vala.size()== valb.size());
      بیعلامت wid = vala.size();

      بیعلامت دراز*ap = vala.subarray(0, wid);
      إذا (ap == 0) {
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->افزودن_vec4(tmp);
	    أرجع درست;
      }

      بیعلامت دراز*bp = valb.subarray(0, wid);
      إذا (bp == 0) {
	    حذف[]ap;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->افزودن_vec4(tmp);
	    أرجع درست;
      }

	// If the value fits in a single CPU word, then do it the easy way.
      إذا (wid <= CPU_کلمه_بیتها) {
	    إذا (bp[0] == 0) {
		  vvp_vector4_t tmp(wid, BIT4_X);
		  thr->افزودن_vec4(tmp);
	    } وإلا {
		  ap[0] /= bp[0];
		  vala.setarray(0, wid, ap);
		  thr->افزودن_vec4(vala);
	    }
	    حذف[]ap;
	    حذف[]bp;
	    أرجع درست;
      }

      بیعلامت دراز*نتیجه = divide_بیتها(ap, bp, wid);
      إذا (نتیجه == 0) {
	    حذف[]ap;
	    حذف[]bp;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->افزودن_vec4(tmp);
	    أرجع درست;
      }

	// Now ap contains the remainder and result contains the
	// desired result. We should find that:
	//  input-a = bp * result + ap;

      vala.setarray(0, wid, نتیجه);
      thr->افزودن_vec4(vala);
      حذف[]ap;
      حذف[]bp;
      حذف[]نتیجه;

      أرجع درست;
}


ساكن فراغ negate_واژگان(بیعلامت دراز*val, بیعلامت واژگان)
{
      بیعلامت دراز انتقال = 1;
      لكل (بیعلامت idx = 0 ; idx < واژگان ; idx += 1)
	    val[idx] = جمع_کن_with_انتقال(0, ~val[idx], انتقال);
}

/*
 * %div/s
 */
منطقي از_DIV_S(vthread_t thr, vvp_کد_t)
{
      vvp_vector4_t valb = thr->برداشت_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      أكد(vala.size()== valb.size());
      بیعلامت wid = vala.size();
      بیعلامت واژگان = (wid + CPU_کلمه_بیتها - 1) / CPU_کلمه_بیتها;

	// Get the values, left in right, in binary form. If there is
	// a problem with either (caused by an X or Z bit) then we
	// know right away that the entire result is X.
      بیعلامت دراز*ap = vala.subarray(0, wid);
      إذا (ap == 0) {
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    أرجع درست;
      }

      بیعلامت دراز*bp = valb.subarray(0, wid);
      إذا (bp == 0) {
	    حذف[]ap;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    أرجع درست;
      }

	// Sign extend the bits in the array to fill out the array.
      بیعلامت دراز sign_ماسک = 0;
      إذا (بیعلامت دراز sign_بیتها = (واژگان*CPU_کلمه_بیتها) - wid) {
	    sign_ماسک = -1UL << (CPU_کلمه_بیتها-sign_بیتها);
	    إذا (ap[واژگان-1] & (sign_ماسک>>1))
		  ap[واژگان-1] |= sign_ماسک;
	    إذا (bp[واژگان-1] & (sign_ماسک>>1))
		  bp[واژگان-1] |= sign_ماسک;
      }

	// If the value fits in a single word, then use the native divide.
      إذا (wid <= CPU_کلمه_بیتها) {
	    إذا (bp[0] == 0) {
		  vvp_vector4_t tmp(wid, BIT4_X);
		  vala = tmp;
	    } وإلا إذا (((دراز)ap[0] == LONG_MIN) && ((دراز)bp[0] == -1)) {
		  vvp_vector4_t tmp(wid, BIT4_0);
		  tmp.مجموعه_بیت(wid-1, BIT4_1);
		  vala = tmp;
	    } وإلا {
		  دراز tmpa = (دراز) ap[0];
		  دراز tmpb = (دراز) bp[0];
		  دراز res = tmpa / tmpb;
		  ap[0] = ((بیعلامت دراز)res) & ~sign_ماسک;
		  vala.setarray(0, wid, ap);
	    }
	    حذف[]ap;
	    حذف[]bp;
	    أرجع درست;
      }

	// We need to the actual division to positive integers. Make
	// them positive here, and remember the negations.
      منطقي negate_flag = خطأ;
      إذا ( ((دراز) ap[واژگان-1]) < 0 ) {
	    negate_flag = درست;
	    negate_واژگان(ap, واژگان);
      }
      إذا ( ((دراز) bp[واژگان-1]) < 0 ) {
	    negate_flag ^= درست;
	    negate_واژگان(bp, واژگان);
      }

      بیعلامت دراز*نتیجه = divide_بیتها(ap, bp, wid);
      إذا (نتیجه == 0) {
	    حذف[]ap;
	    حذف[]bp;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    أرجع درست;
      }

      إذا (negate_flag) {
	    negate_واژگان(نتیجه, واژگان);
      }

      نتیجه[واژگان-1] &= ~sign_ماسک;

      vala.setarray(0, wid, نتیجه);
      حذف[]ap;
      حذف[]bp;
      حذف[]نتیجه;
      أرجع درست;
}

منطقي از_DIV_WR(vthread_t thr, vvp_کد_t)
{
      مزدوج r = thr->برداشت_real();
      مزدوج l = thr->برداشت_real();
      thr->افزودن_real(l / r);

      أرجع درست;
}

/*
 * %dup/obj
 * %dup/real
 * %dup/vec4
 *
 * Push a duplicate of the object on the appropriate stack.
 */
منطقي از_DUP_OBJ(vthread_t thr, vvp_کد_t)
{
      vvp_object_t src = thr->peek_object();

        // If it is null push a new null object
      إذا (src.test_nil())
	    thr->افزودن_object(vvp_object_t());
      وإلا
	    thr->افزودن_object(src.duplicate());

      أرجع درست;
}

منطقي از_DUP_REAL(vthread_t thr, vvp_کد_t)
{
      thr->افزودن_real(thr->peek_real(0));
      أرجع درست;
}

منطقي از_DUP_VEC4(vthread_t thr, vvp_کد_t)
{
      thr->افزودن_vec4(thr->peek_vec4(0));
      أرجع درست;
}

/*
 * This terminates the current thread. If there is a parent who is
 * waiting for me to die, then I schedule it. At any rate, I mark
 * myself as a zombie by setting my pc to 0.
 */
منطقي از_انتها(vthread_t thr, vvp_کد_t)
{
      أكد(! thr->انتظار_for_event);
      thr->i_have_ended = 1;
      thr->pc = codespace_null();

	/* Fully detach any detached children. */
      طالما (! thr->detached_children.empty()) {
	    vthread_t فرزند = *(thr->detached_children.begin());
	    أكد(فرزند);
	    أكد(فرزند->والد == thr);
	    أكد(فرزند->i_am_detached);
	    فرزند->والد = 0;
	    فرزند->i_am_detached = 0;
	    thr->detached_children.erase(thr->detached_children.begin());
      }

	/* It is an error to still have active children running at this
	 * point in time. They should have all been detached or joined. */
      أكد(thr->children.empty());

	/* If I have a parent who is waiting for me, then mark that I
	   have ended, and schedule that parent. Also, finish the
	   %join for the parent. */
      إذا (!thr->i_am_detached && thr->والد && thr->والد->i_am_joining) {
	    vthread_t tmp = thr->والد;
	    أكد(! thr->i_am_detached);

	    tmp->i_am_joining = 0;
	    زمانبندی_vthread(tmp, 0, درست);
	    do_join(tmp, thr);
	    أرجع خطأ;
      }

	/* If this thread is not fully detached then remove it from the
	 * parents detached_children set and reap it. */
      إذا (thr->i_am_detached) {
	    vthread_t tmp = thr->والد;
	    أكد(tmp);
	    size_t res = tmp->detached_children.erase(thr);
	    أكد(res == 1);
	      /* If the parent is waiting for the detached children to
	       * finish then the last detached child needs to tell the
	       * parent to wake up when it is finished. */
	    إذا (tmp->i_am_انتظار && tmp->detached_children.empty()) {
		  tmp->i_am_انتظار = 0;
		  زمانبندی_vthread(tmp, 0, درست);
	    }
	      /* Fully detach this thread so it will be reaped below. */
	    thr->i_am_detached = 0;
	    thr->والد = 0;
      }

	/* If I have no parent, then no one can %join me and there is
	 * no reason to stick around. This can happen, for example if
	 * I am an ``initial'' thread. */
      إذا (thr->والد == 0) {
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
منطقي از_EVENT(vthread_t thr, vvp_کد_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_vector4_t tmp (1, BIT4_X);
      vvp_send_vec4(ptr, tmp, thr->wt_context);
      أرجع درست;
}

/*
 * %event/nb <var-label>, <delay>
 */
منطقي از_EVENT_NB(vthread_t thr, vvp_کد_t cp)
{
      vvp_time64_t delay;

      delay = thr->واژگان[cp->بیت_idx[0]].w_uint;
      زمانبندی_propagate_event(cp->net, delay);
      أرجع درست;
}

منطقي از_EVCTL(vthread_t thr, vvp_کد_t cp)
{
      أكد(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      thr->ecount = thr->واژگان[cp->بیت_idx[0]].w_uint;
      أرجع درست;
}
منطقي از_EVCTLC(vthread_t thr, vvp_کد_t)
{
      thr->event = 0;
      thr->ecount = 0;
      أرجع درست;
}

منطقي از_EVCTLI(vthread_t thr, vvp_کد_t cp)
{
      أكد(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      thr->ecount = cp->بیت_idx[0];
      أرجع درست;
}

منطقي از_EVCTLS(vthread_t thr, vvp_کد_t cp)
{
      أكد(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      int64_t val = thr->واژگان[cp->بیت_idx[0]].w_int;
      إذا (val < 0) val = 0;
      thr->ecount = val;
      أرجع درست;
}

منطقي از_FLAG_GET_VEC4(vthread_t thr, vvp_کد_t cp)
{
      صحيح flag = cp->عدد;
      أكد(flag < vthread_s::FLAGS_شمار);

      vvp_vector4_t val (1, thr->flags[flag]);
      thr->افزودن_vec4(val);

      أرجع درست;
}

/*
 * %flag_inv <flag1>
 */
منطقي از_FLAG_INV(vthread_t thr, vvp_کد_t cp)
{
      صحيح flag1 = cp->بیت_idx[0];

      thr->flags[flag1] = ~ thr->flags[flag1];
      أرجع درست;
}

/*
 * %flag_mov <flag1>, <flag2>
 */
منطقي از_FLAG_MOV(vthread_t thr, vvp_کد_t cp)
{
      صحيح flag1 = cp->بیت_idx[0];
      صحيح flag2 = cp->بیت_idx[1];

      thr->flags[flag1] = thr->flags[flag2];
      أرجع درست;
}

/*
 * %flag_or <flag1>, <flag2>
 */
منطقي از_FLAG_OR(vthread_t thr, vvp_کد_t cp)
{
      صحيح flag1 = cp->بیت_idx[0];
      صحيح flag2 = cp->بیت_idx[1];

      thr->flags[flag1] = thr->flags[flag1] | thr->flags[flag2];
      أرجع درست;
}

منطقي از_FLAG_مجموعه_IMM(vthread_t thr, vvp_کد_t cp)
{
      صحيح flag = cp->عدد;
      صحيح vali = cp->بیت_idx[0];

      أكد(flag < vthread_s::FLAGS_شمار);
      أكد(vali >= 0 && vali < 4);

      ساكن ثابت vvp_bit4_t map_بیت[4] = {BIT4_0, BIT4_1, BIT4_Z, BIT4_X};
      thr->flags[flag] = map_بیت[vali];
      أرجع درست;
}

منطقي از_FLAG_مجموعه_VEC4(vthread_t thr, vvp_کد_t cp)
{
      صحيح flag = cp->عدد;
      أكد(flag < vthread_s::FLAGS_شمار);

      ثابت vvp_vector4_t&val = thr->peek_vec4();
      thr->flags[flag] = val.value(0);
      thr->برداشت_vec4(1);

      أرجع درست;
}

/*
 * the %force/link instruction connects a source node to a
 * destination node. The destination node must be a signal, as it is
 * marked with the source of the force so that it may later be
 * unlinked without specifically knowing the source that this
 * instruction used.
 */
منطقي از_FORCE_LINK(vthread_t, vvp_کد_t cp)
{
      vvp_net_t*dst = cp->net;
      vvp_net_t*src = cp->net2;

      أكد(dst->fil);
      dst->fil->force_link(dst, src);

      أرجع درست;
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
منطقي از_FORCE_VEC4(vthread_t thr, vvp_کد_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_vector4_t value = thr->برداشت_vec4();

	/* Send the force value to the filter on the node. */

      أكد(net->fil);
      إذا (value.size() != net->fil->filter_اندازه())
	    value = coerce_مقصد_عرض(value, net->fil->filter_اندازه());

      net->force_vec4(value, vvp_vector2_t(vvp_vector2_t::FILL1, net->fil->filter_اندازه()));

      أرجع درست;
}

/*
 * %force/vec4/off <net>, <off>
 */
منطقي از_FORCE_VEC4_OFF(vthread_t thr, vvp_کد_t cp)
{
      vvp_net_t*net = cp->net;
      بیعلامت پایه_idx = cp->بیت_idx[0];
      دراز پایه = thr->واژگان[پایه_idx].w_int;
      vvp_vector4_t value = thr->برداشت_vec4();
      بیعلامت wid = value.size();

      أكد(net->fil);

      إذا (thr->flags[4] == BIT4_1)
	    أرجع درست;

	// This is the width of the target vector.
      بیعلامت use_اندازه = net->fil->filter_اندازه();

      إذا (پایه >= (دراز)use_اندازه)
	    أرجع درست;
      إذا (پایه < -(دراز)use_اندازه)
	    أرجع درست;

      إذا ((پایه + wid) > use_اندازه)
	    wid = use_اندازه - پایه;

	// Make a mask of which bits are to be forced, 0 for unforced
	// bits and 1 for forced bits.
      vvp_vector2_t ماسک (vvp_vector2_t::FILL0, use_اندازه);
      لكل (بیعلامت idx = 0 ; idx < wid ; idx += 1)
	    ماسک.مجموعه_بیت(پایه+idx, 1);

      vvp_vector4_t tmp (use_اندازه, BIT4_Z);

	// vvp_net_t::force_vec4 propagates all the bits of the
	// forced vector value, regardless of the mask. This
	// ensures the unforced bits retain their current value.
      vvp_signal_مقدار*sig = dynamic_cast<vvp_signal_مقدار*>(net->fil);
      أكد(sig);
      sig->vec4_مقدار(tmp);

      tmp.مجموعه_vec(پایه, value);

      net->force_vec4(tmp, ماسک);
      أرجع درست;
}

/*
 * %force/vec4/off/d <net>, <off>, <del>
 */
منطقي از_FORCE_VEC4_OFF_D(vthread_t thr, vvp_کد_t cp)
{
      vvp_net_t*net = cp->net;

      بیعلامت پایه_idx = cp->بیت_idx[0];
      دراز پایه = thr->واژگان[پایه_idx].w_int;

      بیعلامت delay_idx = cp->بیت_idx[1];
      vvp_time64_t delay = thr->واژگان[delay_idx].w_uint;

      vvp_vector4_t value = thr->برداشت_vec4();

      أكد(net->fil);

      إذا (thr->flags[4] == BIT4_1)
	    أرجع درست;

	// This is the width of the target vector.
      بیعلامت use_اندازه = net->fil->filter_اندازه();

      إذا (پایه >= (دراز)use_اندازه)
	    أرجع درست;
      إذا (پایه < -(دراز)use_اندازه)
	    أرجع درست;

      زمانبندی_force_وکتور(net, پایه, use_اندازه, value, delay);
      أرجع درست;
}

منطقي از_FORCE_WR(vthread_t thr, vvp_کد_t cp)
{
      vvp_net_t*net  = cp->net;
      مزدوج value = thr->برداشت_real();

      net->force_real(value, vvp_vector2_t(vvp_vector2_t::FILL1, 1));

      أرجع درست;
}

/*
 * The %fork instruction causes a new child to be created and pushed
 * in front of any existing child. This causes the new child to be
 * added to the list of children, and for me to be the parent of the
 * new child.
 */
منطقي از_FORK(vthread_t thr, vvp_کد_t cp)
{
      vthread_t فرزند = vthread_new(cp->cptr2, cp->scope);

      إذا (cp->scope->is_automatic()) {
              /* The context allocated for this child is the top entry
                 on the write context stack. */
            فرزند->wt_context = thr->wt_context;
            فرزند->rd_context = thr->wt_context;
      }

      فرزند->والد = thr;
      thr->children.insert(فرزند);

      إذا (thr->i_am_درون_function) {
	    فرزند->is_scheduled = 1;
	    فرزند->i_am_درون_function = 1;
	    vthread_پیاپی(فرزند);
	    running_thread = thr;
      } وإلا {
	    زمانبندی_vthread(فرزند, 0, درست);
      }
      أرجع درست;
}

منطقي از_FREE(vthread_t thr, vvp_کد_t cp)
{
        /* Pop the child context from the read context stack. */
      vvp_context_t فرزند_context = thr->rd_context;
      thr->rd_context = vvp_get_stacked_context(فرزند_context);

        /* Free the context. */
      vthread_free_context(فرزند_context, cp->scope);

      أرجع درست;
}

/*
 * %inv
 *
 * Logically, this pops a value, inverts is (Verilog style, with Z and
 * X converted to X) and pushes the result. We can more efficiently
 * just to the invert in place.
 */
منطقي از_INV(vthread_t thr, vvp_کد_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val.invert();
      أرجع درست;
}


/*
 * Index registers, arithmetic.
 */

ساكن مضمن int64_t get_as_64_بیت(uint32_t کم_32, uint32_t بالا_32)
{
      int64_t کم = کم_32;
      int64_t res = بالا_32;

      res <<= 32;
      res |= کم;
      أرجع res;
}

منطقي از_IX_جمع_کن(vthread_t thr, vvp_کد_t cp)
{
      thr->واژگان[cp->عدد].w_int += get_as_64_بیت(cp->بیت_idx[0],
                                                    cp->بیت_idx[1]);
      أرجع درست;
}

منطقي از_IX_SUB(vthread_t thr, vvp_کد_t cp)
{
      thr->واژگان[cp->عدد].w_int -= get_as_64_بیت(cp->بیت_idx[0],
                                                    cp->بیت_idx[1]);
      أرجع درست;
}

منطقي از_IX_MUL(vthread_t thr, vvp_کد_t cp)
{
      thr->واژگان[cp->عدد].w_int *= get_as_64_بیت(cp->بیت_idx[0],
                                                    cp->بیت_idx[1]);
      أرجع درست;
}

منطقي از_IX_بارگیری(vthread_t thr, vvp_کد_t cp)
{
      thr->واژگان[cp->عدد].w_int = get_as_64_بیت(cp->بیت_idx[0],
                                                   cp->بیت_idx[1]);
      أرجع درست;
}

منطقي از_IX_MOV(vthread_t thr, vvp_کد_t cp)
{
      thr->واژگان[cp->بیت_idx[0]].w_int = thr->واژگان[cp->بیت_idx[1]].w_int;
      أرجع درست;
}

منطقي از_IX_GETV(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت شاخص = cp->بیت_idx[0];
      vvp_net_t*net = cp->net;

      vvp_signal_مقدار*sig = dynamic_cast<vvp_signal_مقدار*>(net->fil);
      إذا (sig == 0) {
	    أكد(net->fil);
	    cerr << thr->get_fileline()
	         << "%%ix/getv error: Net arg not a vector signal? "
		 << typeid(*net->fil).name() << پایان_سطر;
      }
      أكد(sig);

      vvp_vector4_t vec;
      sig->vec4_مقدار(vec);
      منطقي سرریز_flag;
      uint64_t val = 0;
      منطقي معلوم_flag = vector4_مقصد_مقدار(vec, سرریز_flag, val);

      إذا (معلوم_flag)
	    thr->واژگان[شاخص].w_uint = val;
      وإلا
	    thr->واژگان[شاخص].w_uint = 0;

	/* Set bit 4 as a flag if the input is unknown. */
      thr->flags[4] = معلوم_flag ? (سرریز_flag ? BIT4_X : BIT4_0) : BIT4_1;

      أرجع درست;
}

منطقي از_IX_GETV_S(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت شاخص = cp->بیت_idx[0];
      vvp_net_t*net = cp->net;

      vvp_signal_مقدار*sig = dynamic_cast<vvp_signal_مقدار*>(net->fil);
      إذا (sig == 0) {
	    أكد(net->fil);
	    cerr << thr->get_fileline()
	         << "%%ix/getv/s error: Net arg not a vector signal? "
		 << "fun=" << typeid(*net->fil).name()
		 << ", fil=" << (net->fil? typeid(*net->fil).name() : "<>")
		 << پایان_سطر;
      }
      أكد(sig);

      vvp_vector4_t vec;
      sig->vec4_مقدار(vec);
      int64_t val;
      منطقي معلوم_flag = vector4_مقصد_مقدار(vec, val, درست, درست);

      إذا (معلوم_flag)
	    thr->واژگان[شاخص].w_int = val;
      وإلا
	    thr->واژگان[شاخص].w_int = 0;

	/* Set bit 4 as a flag if the input is unknown. */
      thr->flags[4] = معلوم_flag? BIT4_0 : BIT4_1;

      أرجع درست;
}

ساكن uint64_t vec4_مقصد_شاخص(vthread_t thr, منطقي signed_flag)
{
	// Get all the information we need about the vec4 vector, then
	// pop it away. We only need the bool bits and the length.
      ثابت vvp_vector4_t&val = thr->peek_vec4();
      بیعلامت val_اندازه = val.size();
      بیعلامت دراز*بیتها = val.subarray(0, val_اندازه, خطأ);
      thr->برداشت_vec4(1);

	// If there are X/Z bits, then the subarray will give us a nil
	// pointer. Set a flag to indicate the error, and give up.
      إذا (بیتها == 0) {
	    thr->flags[4] = BIT4_1;
	    أرجع 0;
      }

      uint64_t v = 0;
      thr->flags[4] = BIT4_0;

      أكد(حجم(بیتها[0]) <= حجم(v));

      v = 0;
      لكل (بیعلامت idx = 0 ; idx < val_اندازه ; idx += 8*حجم(بیتها[0])) {
	    uint64_t tmp = بیتها[idx/8/حجم(بیتها[0])];
	    إذا (idx < 8*حجم(v)) {
		  v |= tmp << idx;
	    } وإلا {
		  منطقي سرریز = signed_flag && (v >> 63) ? ~tmp != 0 : tmp != 0;
		  إذا (سرریز) {
			thr->flags[4] = BIT4_X;
			اكسر;
		  }
	    }
      }

	// Set the high bits that are not necessarily filled in by the
	// subarray function.
      إذا (val_اندازه < 8*حجم(v)) {
	    إذا (signed_flag && (v & (static_cast<uint64_t>(1)<<(val_اندازه-1)))) {
		    // Propagate the sign bit...
		  v |= (~static_cast<uint64_t>(0)) << val_اندازه;

	    } وإلا {
		    // Fill with zeros.
		  v &= ~((~static_cast<uint64_t>(0)) << val_اندازه);
	    }

      }

      حذف[]بیتها;
      أرجع v;
}

/*
 * %ix/vec4 <idx>
 */
منطقي از_IX_VEC4(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت use_idx = cp->عدد;
      thr->واژگان[use_idx].w_uint = vec4_مقصد_شاخص(thr, خطأ);
      أرجع درست;
}

/*
 * %ix/vec4/s <idx>
 */
منطقي از_IX_VEC4_S(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت use_idx = cp->عدد;
      thr->واژگان[use_idx].w_uint = vec4_مقصد_شاخص(thr, درست);
      أرجع درست;
}

/*
 * The various JMP instruction work simply by pulling the new program
 * counter from the instruction and resuming. If the jump is
 * conditional, then test the bit for the expected value first.
 */
منطقي از_JMP(vthread_t thr, vvp_کد_t cp)
{
      thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      إذا (زمانبندی_stopped()) {
	    زمانبندی_vthread(thr, 0, خطأ);
	    أرجع خطأ;
      }

      أرجع درست;
}

/*
 * %jmp/0 <pc>, <flag>
 */
منطقي از_JMP0(vthread_t thr, vvp_کد_t cp)
{
      إذا (thr->flags[cp->بیت_idx[0]] == BIT4_0)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      إذا (زمانبندی_stopped()) {
	    زمانبندی_vthread(thr, 0, خطأ);
	    أرجع خطأ;
      }

      أرجع درست;
}

/*
 * %jmp/0xz <pc>, <flag>
 */
منطقي از_JMP0_XZ(vthread_t thr, vvp_کد_t cp)
{
      إذا (thr->flags[cp->بیت_idx[0]] != BIT4_1)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      إذا (زمانبندی_stopped()) {
	    زمانبندی_vthread(thr, 0, خطأ);
	    أرجع خطأ;
      }

      أرجع درست;
}

/*
 * %jmp/1 <pc>, <flag>
 */
منطقي از_JMP1(vthread_t thr, vvp_کد_t cp)
{
      إذا (thr->flags[cp->بیت_idx[0]] == BIT4_1)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      إذا (زمانبندی_stopped()) {
	    زمانبندی_vthread(thr, 0, خطأ);
	    أرجع خطأ;
      }

      أرجع درست;
}

/*
 * %jmp/1xz <pc>, <flag>
 */
منطقي از_JMP1_XZ(vthread_t thr, vvp_کد_t cp)
{
      إذا (thr->flags[cp->بیت_idx[0]] != BIT4_0)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      إذا (زمانبندی_stopped()) {
	    زمانبندی_vthread(thr, 0, خطأ);
	    أرجع خطأ;
      }

      أرجع درست;
}

/*
 * The %join instruction causes the thread to wait for one child
 * to die.  If a child is already dead (and a zombie) then I reap
 * it and go on. Otherwise, I mark myself as waiting in a join so that
 * children know to wake me when they finish.
 */

ساكن فراغ do_join(vthread_t thr, vthread_t فرزند)
{
      أكد(فرزند->والد == thr);

        /* If the immediate child thread is in an automatic scope... */
      إذا (فرزند->wt_context) {
              /* and is the top level task/function thread... */
            إذا (thr->wt_context != thr->rd_context) {
                    /* Pop the child context from the write context stack. */
                  vvp_context_t فرزند_context = thr->wt_context;
                  thr->wt_context = vvp_get_stacked_context(فرزند_context);

                    /* Push the child context onto the read context stack */
                  vvp_مجموعه_stacked_context(فرزند_context, thr->rd_context);
                  thr->rd_context = فرزند_context;
            }
      }

      vthread_reap(فرزند);
}

ساكن منطقي do_join_opcode(vthread_t thr)
{
      أكد( !thr->i_am_joining );
      أكد( !thr->children.empty());

	// Are there any children that have already ended? If so, then
	// join with that one.
      لكل (مجموعه<vthread_t>::iterator cur = thr->children.begin()
		 ; cur != thr->children.انتها() ; ++cur) {
	    vthread_t curp = *cur;
	    إذا (! curp->i_have_ended)
		  استمر;

	      // found something!
	    do_join(thr, curp);
	    أرجع درست;
      }

	// Otherwise, tell my children to awaken me when they end,
	// then pause.
      thr->i_am_joining = 1;
      أرجع خطأ;
}

منطقي از_JOIN(vthread_t thr, vvp_کد_t)
{
      أرجع do_join_opcode(thr);
}

/*
 * This %join/detach <n> instruction causes the thread to detach
 * threads that were created by an earlier %fork.
 */
منطقي از_JOIN_DETACH(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت دراز شمار = cp->عدد;

      أكد(شمار == thr->children.size());

      طالما (! thr->children.empty()) {
	    vthread_t فرزند = *thr->children.begin();
	    أكد(فرزند->والد == thr);

	      // We cannot detach automatic tasks/functions within an
	      // automatic scope. If we try to do that, we might make
	      // a mess of the allocation of the context. Note that it
	      // is OK if the child context is distinct (See %exec_ufunc.)
	    أكد(فرزند->wt_context==0 || thr->wt_context!=فرزند->wt_context);
	    إذا (فرزند->i_have_ended) {
		    // If the child has already ended, then reap it.
		  vthread_reap(فرزند);

	    } وإلا {
		  size_t res = فرزند->والد->children.erase(فرزند);
		  أكد(res == 1);
		  فرزند->i_am_detached = 1;
		  thr->detached_children.insert(فرزند);
	    }
      }

      أرجع درست;
}

/*
 * %load/ar <array-label>, <index>;
*/
منطقي از_بارگیری_AR(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت idx = cp->بیت_idx[0];
      مزدوج کلمه;

	/* The result is 0.0 if the address is undefined. */
      إذا (thr->flags[4] == BIT4_1) {
	    کلمه = 0.0;
      } وإلا {
	    بیعلامت adr = thr->واژگان[idx].w_int;
	    کلمه = cp->array->get_کلمه_r(adr);
      }

      thr->افزودن_real(کلمه);
      أرجع درست;
}

قالب <نام_نوع ELEM>
ساكن منطقي بارگیری_dar(vthread_t thr, vvp_کد_t cp)
{
      int64_t adr = thr->واژگان[3].w_int;
      vvp_net_t*net = cp->net;
      أكد(net);

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      أكد(obj);

      vvp_darray*darray = obj->get_object().peek<vvp_darray>();

      ELEM کلمه;
      إذا (darray &&
          (adr >= 0) && (thr->flags[4] == BIT4_0)) // A defined address >= 0
	    darray->get_کلمه(adr, کلمه);
      وإلا
	    dq_default(کلمه, obj->size());

      vthread_افزودن(thr, کلمه);
      أرجع درست;
}

/*
 * %load/dar/r <array-label>;
 */
منطقي از_بارگیری_DAR_R(vthread_t thr, vvp_کد_t cp)
{
      أرجع بارگیری_dar<مزدوج>(thr, cp);
}

/*
 * %load/dar/str <array-label>;
 */
منطقي از_بارگیری_DAR_STR(vthread_t thr, vvp_کد_t cp)
{
      أرجع بارگیری_dar<string>(thr, cp);
}

/*
 * %load/dar/vec4 <array-label>;
 */
منطقي از_بارگیری_DAR_VEC4(vthread_t thr, vvp_کد_t cp)
{
      أرجع بارگیری_dar<vvp_vector4_t>(thr, cp);
}

/*
 * %load/prop/dar/vec4 <pid>, <wid>;
 *   Indexed read of queue/dynamic-array class property; object on stack,
 *   index in words[3] (same as %load/dar/vec4).
 */
منطقي از_بارگیری_PROP_DAR_VEC4(vthread_t thr, vvp_کد_t cp)
{
      size_t pid = cp->عدد;
      بیعلامت wid = cp->بیت_idx[0];
      int64_t adr = thr->واژگان[3].w_int;

      vvp_object_t& بالاترین = thr->peek_object();
      vvp_cobject*cobj = بالاترین.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t pobj;
      cobj->get_object(pid, pobj, 0);
      vvp_darray*darray = pobj.peek<vvp_darray>();

      vvp_vector4_t کلمه;
      إذا (darray && (adr >= 0) && (thr->flags[4] == BIT4_0)) {
	    darray->get_کلمه(adr, کلمه);
      } وإلا {
	    dq_default(کلمه, wid);
      }

      thr->افزودن_vec4(کلمه);
      أرجع درست;
}

/*
 * %load/obj <var-label>
 */
منطقي از_بارگیری_OBJ(vthread_t thr, vvp_کد_t cp)
{
      vvp_net_t*net = cp->net;
      vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      أكد(fun);

      vvp_object_t val = fun->get_object();
      thr->افزودن_object(val);

      أرجع درست;
}

/*
 * %load/obja <index>
 *    Loads the object from array, using index <index> as the index
 *    value. If flags[4] == 1, the calculation of <index> may have
 *    failed, so push nil.
 */
منطقي از_بارگیری_OBJA(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت idx = cp->بیت_idx[0];
      vvp_object_t کلمه;

	/* The result is 0.0 if the address is undefined. */
      إذا (thr->flags[4] == BIT4_1) {
	    ; // Return nil
      } وإلا {
	    بیعلامت adr = thr->واژگان[idx].w_int;
	    cp->array->get_کلمه_obj(adr, کلمه);
      }

      thr->افزودن_object(کلمه);
      أرجع درست;
}

/*
 * %load/real <var-label>
 */
منطقي از_بارگیری_REAL(vthread_t thr, vvp_کد_t cp)
{
      __vpiHandle*tmp = cp->handle;
      t_vpi_مقدار val;

      val.format = vpiRealVal;
      vpi_get_مقدار(tmp, &val);

      thr->افزودن_real(val.value.real);

      أرجع درست;
}

/*
 * %load/str <var-label>
 */
منطقي از_بارگیری_STR(vthread_t thr, vvp_کد_t cp)
{
      vvp_net_t*net = cp->net;


      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (net->fun);
      أكد(fun);

      ثابت string&val = fun->get_string();
      thr->افزودن_str(val);

      أرجع درست;
}

منطقي از_بارگیری_STRA(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت idx = cp->بیت_idx[0];
      string کلمه;

      إذا (thr->flags[4] == BIT4_1) {
	    کلمه = "";
      } وإلا {
	    بیعلامت adr = thr->واژگان[idx].w_int;
	    کلمه = cp->array->get_کلمه_str(adr);
      }

      thr->افزودن_str(کلمه);
      أرجع درست;
}


/*
 * %load/vec4 <net>
 */
منطقي از_بارگیری_VEC4(vthread_t thr, vvp_کد_t cp)
{
	// Push a placeholder onto the stack in order to reserve the
	// stack space. Use a reference for the stack top as a target
	// for the load.
      thr->افزودن_vec4(vvp_vector4_t());
      vvp_vector4_t&sig_مقدار = thr->peek_vec4();

      vvp_net_t*net = cp->net;

	// For the %load to work, the functor must actually be a
	// signal functor. Only signals save their vector value.
      vvp_signal_مقدار*sig = dynamic_cast<vvp_signal_مقدار*> (net->fil);
      إذا (sig == 0) {
	    cerr << thr->get_fileline()
	         << "%load/v error: Net arg not a signal? "
		 << (net->fil ? typeid(*net->fil).name() :
	                        typeid(*net->fun).name())
	         << پایان_سطر;
	    أكد(sig);
	    أرجع درست;
      }

	// Extract the value from the signal and directly into the
	// target stack position.
      sig->vec4_مقدار(sig_مقدار);

      أرجع درست;
}

/*
 * %load/vec4a <arr>, <adrx>
 */
منطقي از_بارگیری_VEC4_A(vthread_t thr, vvp_کد_t cp)
{
      صحيح adr_شاخص = cp->بیت_idx[0];

      دراز adr = thr->واژگان[adr_شاخص].w_int;

	// If flag[3] is set, then the calculation of the address
	// failed, and this load should return X instead of the actual
	// value.
      إذا (thr->flags[4] == BIT4_1) {
	    vvp_vector4_t tmp (cp->array->get_کلمه_اندازه(), BIT4_X);
	    thr->افزودن_vec4(tmp);
	    أرجع درست;
      }

      vvp_vector4_t tmp (cp->array->get_کلمه(adr));
      thr->افزودن_vec4(tmp);
      أرجع درست;
}

ساكن فراغ do_verylong_mod(vvp_vector4_t&vala, ثابت vvp_vector4_t&valb,
			    منطقي چپ_is_neg, منطقي راست_is_neg)
{
      منطقي بیرون_is_neg = چپ_is_neg;
      ثابت صحيح len=vala.size();
      بیعلامت حرف *a, *z, *t;
      a = جديد بیعلامت حرف[len+1];
      z = جديد بیعلامت حرف[len+1];
      t = جديد بیعلامت حرف[len+1];

      بیعلامت حرف انتقال;
      بیعلامت حرف موقت;

      صحيح mxa = -1, mxz = -1;
      صحيح i;
      صحيح current, copylen;

      بیعلامت lb_انتقال = چپ_is_neg? 1 : 0;
      بیعلامت rb_انتقال = راست_is_neg? 1 : 0;
      لكل (صحيح idx = 0 ;  idx < len ;  idx += 1) {
	    بیعلامت lb = vala.value(idx);
	    بیعلامت rb = valb.value(idx);

	    إذا ((lb | rb) & 2) {
		  حذف []t;
		  حذف []z;
		  حذف []a;
		  vvp_vector4_t tmp(len, BIT4_X);
		  vala = tmp;
		  أرجع;
	    }

	    إذا (چپ_is_neg) {
		  lb = (1-lb) + lb_انتقال;
		  lb_انتقال = (lb & ~1)? 1 : 0;
		  lb &= 1;
	    }
	    إذا (راست_is_neg) {
		  rb = (1-rb) + rb_انتقال;
		  rb_انتقال = (rb & ~1)? 1 : 0;
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
		  حذف []t;
		  حذف []z;
		  حذف []a;
		  vvp_vector4_t tmpx (len, BIT4_X);
		  vala = tmpx;
		  أرجع;
	    }

	    اذهب tally;
      }

      copylen = mxa + 2;
      current = mxz - mxa;

      طالما(current > -1) {
	    انتقال = 1;
	    لكل(i=0;i<copylen;i++) {
		  موقت = z[i+current] + a[i] + انتقال;
		  t[i] = (موقت&1);
		  انتقال = (موقت>>1);
	    }

	    إذا(انتقال) {
		  لكل(i=0;i<copylen;i++) {
			z[i+current] = t[i];
		  }
	    }

	    current--;
      }

 tally:

      vvp_vector4_t tmp (len, BIT4_X);
      انتقال = بیرون_is_neg? 1 : 0;
      لكل (صحيح idx = 0 ;  idx < len ;  idx += 1) {
	    بیعلامت ob = z[idx];
	    إذا (بیرون_is_neg) {
		  ob = (1-ob) + انتقال;
		  انتقال = (ob & ~1)? 1 : 0;
		  ob = ob & 1;
	    }
	    tmp.مجموعه_بیت(idx, ob?BIT4_1:BIT4_0);
      }
      vala = tmp;
      حذف []t;
      حذف []z;
      حذف []a;
}

منطقي از_MAX_WR(vthread_t thr, vvp_کد_t)
{
      مزدوج r = thr->برداشت_real();
      مزدوج l = thr->برداشت_real();
      إذا (r != r)
	    thr->افزودن_real(l);
      وإلا إذا (l != l)
	    thr->افزودن_real(r);
      وإلا إذا (r < l)
	    thr->افزودن_real(l);
      وإلا
	    thr->افزودن_real(r);
      أرجع درست;
}

منطقي از_MIN_WR(vthread_t thr, vvp_کد_t)
{
      مزدوج r = thr->برداشت_real();
      مزدوج l = thr->برداشت_real();
      إذا (r != r)
	    thr->افزودن_real(l);
      وإلا إذا (l != l)
	    thr->افزودن_real(r);
      وإلا إذا (r < l)
	    thr->افزودن_real(r);
      وإلا
	    thr->افزودن_real(l);
      أرجع درست;
}

منطقي از_MOD(vthread_t thr, vvp_کد_t)
{
      vvp_vector4_t valb = thr->برداشت_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      أكد(vala.size()==valb.size());
      بیعلامت wid = vala.size();

      إذا(wid <= 8*حجم(بیعلامت دراز دراز)) {
	    بیعلامت دراز دراز lv = 0, rv = 0;

	    لكل (بیعلامت idx = 0 ;  idx < wid ;  idx += 1) {
		  بیعلامت دراز دراز lb = vala.value(idx);
		  بیعلامت دراز دراز rb = valb.value(idx);

		  إذا ((lb | rb) & 2)
			اذهب x_بیرون;

		  lv |= (بیعلامت دراز دراز) lb << idx;
		  rv |= (بیعلامت دراز دراز) rb << idx;
	    }

	    إذا (rv == 0)
		  اذهب x_بیرون;

	    lv %= rv;

	    لكل (بیعلامت idx = 0 ;  idx < wid ;  idx += 1) {
		  vala.مجموعه_بیت(idx, (lv&1)?BIT4_1 : BIT4_0);
		  lv >>= 1;
	    }

	    أرجع درست;

      } وإلا {
	    do_verylong_mod(vala, valb, خطأ, خطأ);
	    أرجع درست;
      }

 x_بیرون:
      vala = vvp_vector4_t(wid, BIT4_X);
      أرجع درست;
}

/*
 * %mod/s
 */
منطقي از_MOD_S(vthread_t thr, vvp_کد_t)
{
      vvp_vector4_t valb = thr->برداشت_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      أكد(vala.size()==valb.size());
      بیعلامت wid = vala.size();

	/* Handle the case that we can fit the bits into a long-long
	   variable. We cause use native % to do the work. */
      إذا(wid <= 8*حجم(دراز دراز)) {
	    دراز دراز lv = 0, rv = 0;

	    لكل (بیعلامت idx = 0 ;  idx < wid ;  idx += 1) {
		  دراز دراز lb = vala.value(idx);
		  دراز دراز rb = valb.value(idx);

		  إذا ((lb | rb) & 2)
			اذهب x_بیرون;

		  lv |= (دراز دراز) lb << idx;
		  rv |= (دراز دراز) rb << idx;
	    }

	    إذا (rv == 0)
		  اذهب x_بیرون;

	    إذا ((lv == LLONG_MIN) && (rv == -1))
		  اذهب zero_بیرون;

	      /* Sign extend the signed operands when needed. */
	    إذا (wid < 8*حجم(دراز دراز)) {
		  إذا (lv & (1LL << (wid-1)))
			lv |= -1ULL << wid;
		  إذا (rv & (1LL << (wid-1)))
			rv |= -1ULL << wid;
	    }

	    lv %= rv;

	    لكل (بیعلامت idx = 0 ;  idx < wid ;  idx += 1) {
		  vala.مجموعه_بیت(idx, (lv&1)? BIT4_1 : BIT4_0);
		  lv >>= 1;
	    }

	      // vala is the top of the stack, edited in place, so we
	      // do not need to push the result.

	    أرجع درست;

      } وإلا {

	    منطقي چپ_is_neg  = vala.value(vala.size()-1) == BIT4_1;
	    منطقي راست_is_neg = valb.value(valb.size()-1) == BIT4_1;
	    do_verylong_mod(vala, valb, چپ_is_neg, راست_is_neg);
	    أرجع درست;
      }

 x_بیرون:
      vala = vvp_vector4_t(wid, BIT4_X);
      أرجع درست;
 zero_بیرون:
      vala = vvp_vector4_t(wid, BIT4_0);
      أرجع درست;
}

/*
 * %mod/wr
 */
منطقي از_MOD_WR(vthread_t thr, vvp_کد_t)
{
      مزدوج r = thr->برداشت_real();
      مزدوج l = thr->برداشت_real();
      thr->افزودن_real(fmod(l,r));

      أرجع درست;
}

/*
 * %neg/wr
 */
منطقي از_NEG_WR(vthread_t thr, vvp_کد_t)
{
      thr->poke_real(0, -thr->peek_real(0));
      أرجع درست;
}

/*
 * %pad/s <wid>
 */
منطقي از_PAD_S(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت wid = cp->عدد;

      vvp_vector4_t&val = thr->peek_vec4();
      بیعلامت old_اندازه = val.size();

	// Sign-extend.
      إذا (old_اندازه < wid)
	    val.resize(wid, val.value(old_اندازه-1));
      وإلا
	    val.resize(wid);

      أرجع درست;
}

/*
 * %pad/u <wid>
 */
منطقي از_PAD_U(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت wid = cp->عدد;

      vvp_vector4_t&val = thr->peek_vec4();
      val.resize(wid, BIT4_0);

      أرجع درست;
}

/*
 * %part/s <wid>
 * %part/u <wid>
 * Two values are popped from the stack. First, pop the canonical
 * index of the part select, and second is the value to be
 * selected. The result is pushed back to the stack.
 */
ساكن منطقي از_PART_پایه(vthread_t thr, vvp_کد_t cp, منطقي signed_flag)
{
      بیعلامت wid = cp->عدد;

      vvp_vector4_t base4 = thr->برداشت_vec4();
      vvp_vector4_t&value = thr->peek_vec4();

      vvp_vector4_t res (wid, BIT4_X);

	// NOTE: This is treating the vector as signed. Is that correct?
      int32_t پایه;
      منطقي مقدار_ok = vector4_مقصد_مقدار(base4, پایه, signed_flag);
      إذا (! مقدار_ok) {
	    value = res;
	    أرجع درست;
      }

      إذا (پایه >= (int32_t)value.size()) {
	    value = res;
	    أرجع درست;
      }

      إذا ((پایه+(صحيح)wid) <= 0) {
	    value = res;
	    أرجع درست;
      }

      دراز vbase = 0;
      إذا (پایه < 0) {
	    vbase = -پایه;
	    wid -= vbase;
	    پایه = 0;
      }

      إذا ((پایه+wid) > value.size()) {
	    wid = value.size() - پایه;
      }

      res .مجموعه_vec(vbase, value.subvalue(پایه, wid));
      value = res;

      أرجع درست;
}

منطقي از_PART_S(vthread_t thr, vvp_کد_t cp)
{
      أرجع از_PART_پایه(thr, cp, درست);
}

منطقي از_PART_U(vthread_t thr, vvp_کد_t cp)
{
      أرجع از_PART_پایه(thr, cp, خطأ);
}

/*
 * %parti/s <wid>, <basei>, <base_wid>
 * %parti/u <wid>, <basei>, <base_wid>
 *
 * Pop the value to be selected. The result is pushed back to the stack.
 */
ساكن منطقي از_PARTI_پایه(vthread_t thr, vvp_کد_t cp, منطقي signed_flag)
{
      بیعلامت wid = cp->عدد;
      uint32_t پایه = cp->بیت_idx[0];
      uint32_t bwid = cp->بیت_idx[1];

      vvp_vector4_t&value = thr->peek_vec4();

      vvp_vector4_t res (wid, BIT4_X);

	// NOTE: This is treating the vector as signed. Is that correct?
      int32_t use_پایه = پایه;
      إذا (signed_flag && bwid < 32 && (پایه&(1<<(bwid-1)))) {
	    use_پایه |= -1UL << bwid;
      }

      إذا (use_پایه >= (int32_t)value.size()) {
	    value = res;
	    أرجع درست;
      }

      إذا ((use_پایه+(int32_t)wid) <= 0) {
	    value = res;
	    أرجع درست;
      }

      دراز vbase = 0;
      إذا (use_پایه < 0) {
	    vbase = -use_پایه;
	    wid -= vbase;
	    use_پایه = 0;
      }

      إذا ((use_پایه+wid) > value.size()) {
	    wid = value.size() - use_پایه;
      }

      res .مجموعه_vec(vbase, value.subvalue(use_پایه, wid));
      value = res;

      أرجع درست;
}

منطقي از_PARTI_S(vthread_t thr, vvp_کد_t cp)
{
      أرجع از_PARTI_پایه(thr, cp, درست);
}

منطقي از_PARTI_U(vthread_t thr, vvp_کد_t cp)
{
      أرجع از_PARTI_پایه(thr, cp, خطأ);
}

/*
 * %mul
 */
منطقي از_MUL(vthread_t thr, vvp_کد_t)
{
      vvp_vector4_t r = thr->برداشت_vec4();
	// Rather then pop l, use it directly from the stack. When we
	// assign to 'l', that will edit the top of the stack, which
	// replaces a pop and a pull.
      vvp_vector4_t&l = thr->peek_vec4();

      l.mul(r);
      أرجع درست;
}

/*
 * %muli <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
منطقي از_MULI(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت wid = cp->عدد;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_فوری_rval (cp, r);

      l.mul(r);
      أرجع درست;
}

منطقي از_MUL_WR(vthread_t thr, vvp_کد_t)
{
      مزدوج r = thr->برداشت_real();
      مزدوج l = thr->برداشت_real();
      thr->افزودن_real(l * r);

      أرجع درست;
}

منطقي از_NAND(vthread_t thr, vvp_کد_t)
{
      vvp_vector4_t valr = thr->برداشت_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      أكد(vall.size() == valr.size());

      vall &= valr;
      vall.invert();

      أرجع درست;
}

/*
 * %new/cobj <vpi_object>
 * This creates a new cobject (SystemVerilog class object) and pushes
 * it to the stack. The <vpi-object> is a __vpiHandle that is a
 * vpiClassDefn object that defines the item to be created.
 */
منطقي از_NEW_COBJ(vthread_t thr, vvp_کد_t cp)
{
      ثابت class_type*defn = dynamic_cast<ثابت class_type*> (cp->handle);
      أكد(defn);

      vvp_object_t tmp (جديد vvp_cobject(defn));
      thr->افزودن_object(tmp);
      أرجع درست;
}

منطقي از_NEW_DARRAY(vthread_t thr, vvp_کد_t cp)
{
      ثابت حرف*متن = cp->متن;
      size_t size = thr->واژگان[cp->بیت_idx[0]].w_int;
      بیعلامت کلمه_wid;
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
      } وإلا إذا ((1 == از_رشته_بخوان(متن, "b%u%zn", &کلمه_wid, &n)) &&
                 (n == طول_رشته(متن))) {
	    obj = جديد vvp_darray_vec2(size, کلمه_wid);
      } وإلا إذا ((1 == از_رشته_بخوان(متن, "sb%u%zn", &کلمه_wid, &n)) &&
                 (n == طول_رشته(متن))) {
	    obj = جديد vvp_darray_vec2(size, کلمه_wid);
      } وإلا إذا ((1 == از_رشته_بخوان(متن, "v%u%zn", &کلمه_wid, &n)) &&
                 (n == طول_رشته(متن))) {
	    obj = جديد vvp_darray_vec4(size, کلمه_wid);
      } وإلا إذا ((1 == از_رشته_بخوان(متن, "sv%u%zn", &کلمه_wid, &n)) &&
                 (n == طول_رشته(متن))) {
	    obj = جديد vvp_darray_vec4(size, کلمه_wid);
      } وإلا إذا (strcmp(متن,"r") == 0) {
	    obj = جديد vvp_darray_real(size);
      } وإلا إذا (strcmp(متن,"S") == 0) {
	    obj = جديد vvp_darray_string(size);
      } وإلا {
	    cerr << get_fileline()
	         << "Internal error: Unsupported dynamic array type: "
	         << متن << "." << پایان_سطر;
	    أكد(0);
      }

      thr->افزودن_object(obj);

      أرجع درست;
}

منطقي از_NOOP(vthread_t, vvp_کد_t)
{
      أرجع درست;
}

/*
 * %nor/r
 */
منطقي از_NORR(vthread_t thr, vvp_کد_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_or());

      أرجع درست;
}

/*
 * Push a null to the object stack.
 */
منطقي از_NULL(vthread_t thr, vvp_کد_t)
{
      vvp_object_t tmp;
      thr->افزودن_object(tmp);
      أرجع درست;
}

/*
 * %and/r
 */
منطقي از_ANDR(vthread_t thr, vvp_کد_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_and());

      أرجع درست;
}

/*
 * %nand/r
 */
منطقي از_NANDR(vthread_t thr, vvp_کد_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_and());

      أرجع درست;
}

/*
 * %or/r
 */
منطقي از_ORR(vthread_t thr, vvp_کد_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_or());

      أرجع درست;
}

/*
 * %xor/r
 */
منطقي از_XORR(vthread_t thr, vvp_کد_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_xor());

      أرجع درست;
}

/*
 * %xnor/r
 */
منطقي از_XNORR(vthread_t thr, vvp_کد_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_xor());

      أرجع درست;
}

/*
 * %or
 */
منطقي از_OR(vthread_t thr, vvp_کد_t)
{
      vvp_vector4_t valb = thr->برداشت_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();
      vala |= valb;
      أرجع درست;
}

/*
 * %nor
 */
منطقي از_NOR(vthread_t thr, vvp_کد_t)
{
      vvp_vector4_t valr = thr->برداشت_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      أكد(vall.size() == valr.size());

      vall |= valr;
      vall.invert();

      أرجع درست;
}

/*
 * %pop/obj <num>, <skip>
 */
منطقي از_برداشت_OBJ(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت cnt = cp->بیت_idx[0];
      بیعلامت skip = cp->بیت_idx[1];

      thr->برداشت_object(cnt, skip);
      أرجع درست;
}

/*
 * %pop/real <number>
 */
منطقي از_برداشت_REAL(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت cnt = cp->عدد;
      thr->برداشت_real(cnt);
      أرجع درست;
}

/*
 *  %pop/str <number>
 */
منطقي از_برداشت_STR(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت cnt = cp->عدد;
      thr->برداشت_str(cnt);
      أرجع درست;
}

/*
 *  %pop/vec4 <number>
 */
منطقي از_برداشت_VEC4(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت cnt = cp->عدد;
      thr->برداشت_vec4(cnt);
      أرجع درست;
}

/*
 * %pow
 * %pow/s
 */
ساكن منطقي از_POW_پایه(vthread_t thr, منطقي signed_flag)
{
      vvp_vector4_t valb = thr->برداشت_vec4();
      vvp_vector4_t vala = thr->برداشت_vec4();

      بیعلامت wid = vala.size();

      vvp_vector2_t xv2 = vvp_vector2_t(vala, درست);
      vvp_vector2_t yv2 = vvp_vector2_t(valb, درست);


        /* If we have an X or Z in the arguments return X. */
      إذا (xv2.is_NaN() || yv2.is_NaN()) {
	    vvp_vector4_t tmp (wid, BIT4_X);
	    thr->افزودن_vec4(tmp);
	    أرجع درست;
      }

	// Is the exponent negative? If so, table 5-6 in IEEE1364-2005
	// defines what value is returned.
      إذا (signed_flag && yv2.value(yv2.size()-1)) {
	    صحيح a_val;
	    vvp_bit4_t pad = BIT4_0, lsb = BIT4_0;
	    إذا (vector2_مقصد_مقدار(xv2, a_val, درست)) {
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
	    tmp.مجموعه_بیت(0, lsb);
	    thr->افزودن_vec4(tmp);
	    أرجع درست;
      }

      vvp_vector2_t نتیجه = توان(xv2, yv2);

        /* Copy only what we need of the result. If the result is too
	   small, zero-pad it. */
      لكل (بیعلامت jdx = 0;  jdx < wid;  jdx += 1) {
	    إذا (jdx >= نتیجه.size())
		  vala.مجموعه_بیت(jdx, BIT4_0);
	    وإلا
		  vala.مجموعه_بیت(jdx, نتیجه.value(jdx) ? BIT4_1 : BIT4_0);
      }
      thr->افزودن_vec4(vala);

      أرجع درست;
}

منطقي از_POW(vthread_t thr, vvp_کد_t)
{
      أرجع از_POW_پایه(thr, خطأ);
}

منطقي از_POW_S(vthread_t thr, vvp_کد_t)
{
      أرجع از_POW_پایه(thr, درست);
}

منطقي از_POW_WR(vthread_t thr, vvp_کد_t)
{
      مزدوج r = thr->برداشت_real();
      مزدوج l = thr->برداشت_real();
      thr->افزودن_real(توان(l,r));

      أرجع درست;
}

/*
 * %prop/obj <pid>, <idx>
 *
 * Load an object value from the cobject and push it onto the object stack.
 */
منطقي از_PROP_OBJ(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت pid = cp->عدد;
      بیعلامت idx = cp->بیت_idx[0];

      إذا (idx != 0) {
	    أكد(idx < vthread_s::واژگان_شمار);
	    idx = thr->واژگان[idx].w_uint;
      }

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();

      vvp_object_t val;
      cobj->get_object(pid, val, idx);

      thr->افزودن_object(val);

      أرجع درست;
}

ساكن فراغ get_from_obj(بیعلامت pid, vvp_cobject*cobj, مزدوج&val)
{
      val = cobj->get_real(pid);
}

ساكن فراغ get_from_obj(بیعلامت pid, vvp_cobject*cobj, string&val)
{
      val = cobj->get_string(pid);
}

ساكن فراغ get_from_obj(بیعلامت pid, vvp_cobject*cobj, vvp_vector4_t&val)
{
      cobj->get_vec4(pid, val);
}

قالب <نام_نوع ELEM>
ساكن منطقي prop(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت pid = cp->عدد;

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      أكد(cobj);

      ELEM val;
      get_from_obj(pid, cobj, val);
      vthread_افزودن(thr, val);

      أرجع درست;
}

/*
 * %prop/r <pid>
 *
 * Load a real value from the cobject and push it onto the real value
 * stack.
 */
منطقي از_PROP_R(vthread_t thr, vvp_کد_t cp)
{
      أرجع prop<مزدوج>(thr, cp);
}

/*
 * %prop/str <pid>
 *
 * Load a string value from the cobject and push it onto the real value
 * stack.
 */
منطقي از_PROP_STR(vthread_t thr, vvp_کد_t cp)
{
      أرجع prop<string>(thr, cp);
}

/*
 * %prop/v <pid>
 *
 * Load a property <pid> from the cobject on the top of the stack into
 * the vector space at <base>.
 */
منطقي از_PROP_V(vthread_t thr, vvp_کد_t cp)
{
      أرجع prop<vvp_vector4_t>(thr, cp);
}

منطقي از_PUSHI_REAL(vthread_t thr, vvp_کد_t cp)
{
      مزدوج mant = cp->بیت_idx[0];
      uint32_t imant = cp->بیت_idx[0];
      صحيح نمایی_تابع = cp->بیت_idx[1];

	// Detect +infinity
      إذا (نمایی_تابع==0x3fff && imant==0) {
	    thr->افزودن_real(INFINITY);
	    أرجع درست;
      }
	// Detect -infinity
      إذا (نمایی_تابع==0x7fff && imant==0) {
	    thr->افزودن_real(-INFINITY);
	    أرجع درست;
      }
	// Detect NaN
      إذا (نمایی_تابع==0x3fff) {
	    thr->افزودن_real(nan(""));
	    أرجع درست;
      }

      مزدوج sign = (نمایی_تابع & 0x4000)? -1.0 : 1.0;

      نمایی_تابع &= 0x1fff;

      mant = sign * ldexp(mant, نمایی_تابع - 0x1000);
      thr->افزودن_real(mant);
      أرجع درست;
}

منطقي از_PUSHI_STR(vthread_t thr, vvp_کد_t cp)
{
      ثابت حرف*متن = cp->متن;
      thr->افزودن_str(filter_string(متن));
      أرجع درست;
}

/*
 * %pushi/vec4 <vala>, <valb>, <wid>
 */
منطقي از_PUSHI_VEC4(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت wid  = cp->عدد;

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t val (wid, BIT4_0);
      get_فوری_rval (cp, val);

      thr->افزودن_vec4(val);

      أرجع درست;
}

/*
 * %pushv/str
 *   Pops a vec4 value, and pushes a string.
 */
منطقي از_PUSHV_STR(vthread_t thr, vvp_کد_t)
{
      vvp_vector4_t vec = thr->برداشت_vec4();

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

      thr->افزودن_str(val);

      أرجع درست;
}

/*
 * %putc/str/vec4 <var>, <mux>
 */
منطقي از_PUTC_STR_VEC4(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت muxr = cp->بیت_idx[0];
      int32_t mux = muxr? thr->واژگان[muxr].w_int : 0;

      vvp_vector4_t val = thr->برداشت_vec4();
      أكد(val.size() == 8);

      إذا (mux < 0)
	    أرجع درست;

	/* Get the existing value of the string. If we find that the
	   index is too big for the string, then give up. */
      vvp_net_t*net = cp->net;
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (net->fun);
      أكد(fun);

      string tmp = fun->get_string();
      إذا (tmp.size() <= (size_t)mux)
	    أرجع درست;

      حرف val_str = 0;
      لكل (size_t idx = 0 ; idx < 8 ; idx += 1) {
	    إذا (val.value(idx)==BIT4_1)
		  val_str |= 1<<idx;
      }

	// It is a quirk of the Verilog standard that putc(..., 'h00)
	// has no effect. Test for that case here.
      إذا (val_str == 0)
	    أرجع درست;

      tmp[mux] = val_str;

      vvp_send_string(vvp_net_ptr_t(cp->net, 0), tmp, thr->wt_context);
      أرجع درست;
}

قالب <نام_نوع ELEM, صنف QTYPE>
ساكن منطقي qinsert(vthread_t thr, vvp_کد_t cp, بیعلامت wid=0)
{
      int64_t idx = thr->واژگان[3].w_int;
      ELEM value;
      vvp_net_t*net = cp->net;
      برداشت_مقدار(thr, value, wid); // Pop the value to store.

      vvp_صف*صف = get_صف_object<QTYPE>(thr, net);
      أكد(صف);
      إذا (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at a negative "
	         << get_صف_type(value)
	         << " index (" << idx << "). ";
	    print_صف_مقدار(value);
	    cerr << " was not added." << پایان_سطر;
      } وإلا إذا (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at an undefined "
	         << get_صف_type(value) << " index. ";
	    print_صف_مقدار(value);
	    cerr << " was not added." << پایان_سطر;
      } وإلا {
	    بیعلامت max_اندازه = thr->واژگان[cp->بیت_idx[0]].w_int;
	    صف->insert(idx, value, max_اندازه);
      }
      أرجع درست;
}

/*
 * %qinsert/real <var-label>
 */
منطقي از_QINSERT_REAL(vthread_t thr, vvp_کد_t cp)
{
      أرجع qinsert<مزدوج, vvp_صف_real>(thr, cp);
}

/*
 * %qinsert/str <var-label>
 */
منطقي از_QINSERT_STR(vthread_t thr, vvp_کد_t cp)
{
      أرجع qinsert<string, vvp_صف_string>(thr, cp);
}

/*
 * %qinsert/v <var-label>
 */
منطقي از_QINSERT_V(vthread_t thr, vvp_کد_t cp)
{
      أرجع qinsert<vvp_vector4_t, vvp_صف_vec4>(thr, cp, cp->بیت_idx[1]);
}

/*
 * Helper functions used in the queue pop templates
 */
مضمن فراغ افزودن_مقدار(vthread_t thr, مزدوج value, بیعلامت)
{
      thr->افزودن_real(value);
}

مضمن فراغ افزودن_مقدار(vthread_t thr, ثابت string&value, بیعلامت)
{
      thr->افزودن_str(value);
}

مضمن فراغ افزودن_مقدار(vthread_t thr, ثابت vvp_vector4_t&value, بیعلامت wid)
{
      أكد(wid == value.size());
      thr->افزودن_vec4(value);
}

قالب <نام_نوع ELEM, صنف QTYPE>
ساكن منطقي q_برداشت(vthread_t thr, vvp_کد_t cp,
                  فراغ (*get_val_func)(vvp_صف*, ELEM&),
                  ثابت حرف*loc, بیعلامت wid)
{
      vvp_net_t*net = cp->net;

      vvp_صف*صف = get_صف_object<QTYPE>(thr, net);
      أكد(صف);

      size_t size = صف->get_اندازه();

      ELEM value;
      إذا (size) {
	    get_val_func(صف, value);
      } وإلا {
	    dq_default(value, wid);
	    cerr << thr->get_fileline()
	         << "Warning: pop_" << loc << "() on empty "
	         << get_صف_type(value) << "." << پایان_سطر;
      }

      افزودن_مقدار(thr, value, wid);
      أرجع درست;
}

قالب <نام_نوع ELEM>
ساكن فراغ get_پشت_مقدار(vvp_صف*صف, ELEM&value)
{
      صف->get_کلمه(صف->get_اندازه()-1, value);
      صف->pop_back();
}

قالب <نام_نوع ELEM, صنف QTYPE>
ساكن منطقي qpop_b(vthread_t thr, vvp_کد_t cp, بیعلامت wid=0)
{
      أرجع q_برداشت<ELEM, QTYPE>(thr, cp, get_پشت_مقدار<ELEM>, "back", wid);
}

/*
 * %qpop/b/real <var-label>
 */
منطقي از_QPOP_B_REAL(vthread_t thr, vvp_کد_t cp)
{
      أرجع qpop_b<مزدوج, vvp_صف_real>(thr, cp);
}

/*
 * %qpop/b/str <var-label>
 */
منطقي از_QPOP_B_STR(vthread_t thr, vvp_کد_t cp)
{
      أرجع qpop_b<string, vvp_صف_string>(thr, cp);
}

/*
 * %qpop/b/v <var-label>
 */
منطقي از_QPOP_B_V(vthread_t thr, vvp_کد_t cp)
{
      أرجع qpop_b<vvp_vector4_t, vvp_صف_vec4>(thr, cp, cp->بیت_idx[0]);
}

قالب <نام_نوع ELEM>
ساكن فراغ get_front_مقدار(vvp_صف*صف, ELEM&value)
{
      صف->get_کلمه(0, value);
      صف->pop_front();
}

قالب <نام_نوع ELEM, صنف QTYPE>
ساكن منطقي qpop_f(vthread_t thr, vvp_کد_t cp, بیعلامت wid=0)
{
      أرجع q_برداشت<ELEM, QTYPE>(thr, cp, get_front_مقدار<ELEM>, "front", wid);
}


/*
 * %qpop/f/real <var-label>
 */
منطقي از_QPOP_F_REAL(vthread_t thr, vvp_کد_t cp)
{
      أرجع qpop_f<مزدوج, vvp_صف_real>(thr, cp);
}

/*
 * %qpop/f/str <var-label>
 */
منطقي از_QPOP_F_STR(vthread_t thr, vvp_کد_t cp)
{
      أرجع qpop_f<string, vvp_صف_string>(thr, cp);
}

/*
 * %qpop/f/v <var-label>
 */
منطقي از_QPOP_F_V(vthread_t thr, vvp_کد_t cp)
{
      أرجع qpop_f<vvp_vector4_t, vvp_صف_vec4>(thr, cp, cp->بیت_idx[0]);
}

/*
 * These implement the %release/net and %release/reg instructions. The
 * %release/net instruction applies to a net kind of functor by
 * sending the release/net command to the command port. (See vvp_net.h
 * for details.) The %release/reg instruction is the same, but sends
 * the release/reg command instead. These are very similar to the
 * %deassign instruction.
 */
ساكن منطقي do_release_vec(vvp_کد_t cp, منطقي net_flag)
{
      vvp_net_t*net = cp->net;
      بیعلامت پایه  = cp->بیت_idx[0];
      بیعلامت width = cp->بیت_idx[1];

      أكد(net->fil);

      إذا (پایه >= net->fil->filter_اندازه()) أرجع درست;
      إذا (پایه+width > net->fil->filter_اندازه())
	    width = net->fil->filter_اندازه() - پایه;

      منطقي پر_sig = پایه == 0 && width == net->fil->filter_اندازه();

	// XXXX Can't really do this if this is a partial release?
      net->fil->force_unlink();

	/* Do we release all or part of the net? */
      vvp_net_ptr_t ptr (net, 0);
      إذا (پر_sig) {
	    net->fil->release(ptr, net_flag);
      } وإلا {
	    net->fil->release_pv(ptr, پایه, width, net_flag);
      }
      net->fun->force_flag(خطأ);

      أرجع درست;
}

منطقي از_RELEASE_NET(vthread_t, vvp_کد_t cp)
{
      أرجع do_release_vec(cp, درست);
}


منطقي از_RELEASE_REG(vthread_t, vvp_کد_t cp)
{
      أرجع do_release_vec(cp, خطأ);
}

/* The type is 1 for registers and 0 for everything else. */
منطقي از_RELEASE_WR(vthread_t, vvp_کد_t cp)
{
      vvp_net_t*net = cp->net;
      بیعلامت type  = cp->بیت_idx[0];

      أكد(net->fil);
      net->fil->force_unlink();

	// Send a command to this signal to unforce itself.
      vvp_net_ptr_t ptr (net, 0);
      net->fil->release(ptr, type==0);
      أرجع درست;
}

منطقي از_REPLICATE(vthread_t thr, vvp_کد_t cp)
{
      صحيح rept = cp->عدد;
      vvp_vector4_t val = thr->برداشت_vec4();
      vvp_vector4_t res (val.size() * rept, BIT4_X);

      لكل (صحيح idx = 0 ; idx < rept ; idx += 1) {
	    res.مجموعه_vec(idx * val.size(), val);
      }

      thr->افزودن_vec4(res);

      أرجع درست;
}

ساكن فراغ poke_val(vthread_t fun_thr, بیعلامت عمق, مزدوج val)
{
      fun_thr->والد->poke_real(عمق, val);
}

ساكن فراغ poke_val(vthread_t fun_thr, بیعلامت عمق, ثابت string&val)
{
      fun_thr->والد->poke_str(عمق, val);
}

ساكن size_t get_max(vthread_t fun_thr, مزدوج&)
{
      أرجع fun_thr->آرگومانها_real.size();
}

ساكن size_t get_max(vthread_t fun_thr, string&)
{
      أرجع fun_thr->آرگومانها_str.size();
}

ساكن size_t get_max(vthread_t fun_thr, vvp_vector4_t&)
{
      أرجع fun_thr->آرگومانها_vec4.size();
}

ساكن بیعلامت get_عمق(vthread_t fun_thr, size_t شاخص, مزدوج&)
{
      أرجع fun_thr->آرگومانها_real[شاخص];
}

ساكن بیعلامت get_عمق(vthread_t fun_thr, size_t شاخص, string&)
{
      أرجع fun_thr->آرگومانها_str[شاخص];
}

ساكن بیعلامت get_عمق(vthread_t fun_thr, size_t شاخص, vvp_vector4_t&)
{
      أرجع fun_thr->آرگومانها_vec4[شاخص];
}

ساكن vthread_t get_func(vthread_t thr)
{
      vthread_t fun_thr = thr;

      طالما (fun_thr->والد_scope->get_type_کد() != vpiFunction) {
	    أكد(fun_thr->والد);
	    fun_thr = fun_thr->والد;
      }

      أرجع fun_thr;
}

قالب <نام_نوع ELEM>
ساكن منطقي ret(vthread_t thr, vvp_کد_t cp)
{
      size_t شاخص = cp->عدد;
      ELEM val;
      برداشت_مقدار(thr, val, 0);

      vthread_t fun_thr = get_func(thr);
      أكد(شاخص < get_max(fun_thr, val));

      بیعلامت عمق = get_عمق(fun_thr, شاخص, val);
	// Use the depth to put the value into the stack of
	// the parent thread.
      poke_val(fun_thr, عمق, val);
      أرجع درست;
}

/*
 * %ret/real <index>
 */
منطقي از_RET_REAL(vthread_t thr, vvp_کد_t cp)
{
      أرجع ret<مزدوج>(thr, cp);
}

/*
 * %ret/str <index>
 */
منطقي از_RET_STR(vthread_t thr, vvp_کد_t cp)
{
      أرجع ret<string>(thr, cp);
}

/*
 * %ret/vec4 <index>, <offset>, <wid>
 */
منطقي از_RET_VEC4(vthread_t thr, vvp_کد_t cp)
{
      size_t شاخص = cp->عدد;
      بیعلامت off_شاخص = cp->بیت_idx[0];
      بیعلامت صحيح wid = cp->بیت_idx[1];
      vvp_vector4_t&val = thr->peek_vec4();

      vthread_t fun_thr = get_func(thr);
      أكد(شاخص < get_max(fun_thr, val));
      أكد(val.size() == wid);
      بیعلامت عمق = get_عمق(fun_thr, شاخص, val);

      int64_t off = off_شاخص ? thr->واژگان[off_شاخص].w_int : 0;
      بیعلامت صحيح sig_مقدار_اندازه = fun_thr->والد->peek_vec4(عمق).size();

      إذا (off_شاخص!=0 && thr->flags[4] == BIT4_1) {
	    thr->برداشت_vec4(1);
	    أرجع درست;
      }

      إذا (!resize_rval_vec(val, off, sig_مقدار_اندازه)) {
	    thr->برداشت_vec4(1);
	    أرجع درست;
      }

      إذا (off == 0 && val.size() == sig_مقدار_اندازه) {
	    fun_thr->والد->poke_vec4(عمق, val);
      } وإلا {
	    vvp_vector4_t tmp_dst = fun_thr->والد->peek_vec4(عمق);
	    tmp_dst.مجموعه_vec(off, val);
	    fun_thr->والد->poke_vec4(عمق, tmp_dst);
      }

      thr->برداشت_vec4(1);
      أرجع درست;
}

ساكن فراغ افزودن_from_والد(vthread_t thr, vthread_t fun_thr, بیعلامت عمق, مزدوج&)
{
      thr->افزودن_real(fun_thr->والد->peek_real(عمق));
}

ساكن فراغ افزودن_from_والد(vthread_t thr, vthread_t fun_thr, بیعلامت عمق, string&)
{
      thr->افزودن_str(fun_thr->والد->peek_str(عمق));
}

ساكن فراغ افزودن_from_والد(vthread_t thr, vthread_t fun_thr, بیعلامت عمق, vvp_vector4_t&)
{
      thr->افزودن_vec4(fun_thr->والد->peek_vec4(عمق));
}

قالب <نام_نوع ELEM>
ساكن منطقي retload(vthread_t thr, vvp_کد_t cp)
{
      size_t شاخص = cp->عدد;
      ELEM type;

      vthread_t fun_thr = get_func(thr);
      أكد(شاخص < get_max(fun_thr, type));

      بیعلامت عمق = get_عمق(fun_thr, شاخص, type);
	// Use the depth to extract the values from the stack
	// of the parent thread.
      افزودن_from_والد(thr, fun_thr, عمق, type);
      أرجع درست;
}

/*
 * %retload/real <index>
 */
منطقي از_RETLOAD_REAL(vthread_t thr, vvp_کد_t cp)
{
      أرجع retload<مزدوج>(thr, cp);
}

/*
 * %retload/str <index>
 */
منطقي از_RETLOAD_STR(vthread_t thr, vvp_کد_t cp)
{
      أرجع retload<string>(thr, cp);
}

/*
 * %retload/vec4 <index>
 */
منطقي از_RETLOAD_VEC4(vthread_t thr, vvp_کد_t cp)
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
منطقي از_SCOPY(vthread_t thr, vvp_کد_t)
{
      vvp_object_t tmp;
      thr->برداشت_object(tmp);

      vvp_object_t&dest = thr->peek_object();
        // If it is null there is nothing to copy
      إذا (!tmp.test_nil())
	    dest.shallow_copy(tmp);

      أرجع درست;
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

قالب <نام_نوع ELEM>
ساكن منطقي مجموعه_dar_obj(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت adr = thr->واژگان[cp->عدد].w_int;

      ELEM value;
      thread_peek(thr, value);

      vvp_object_t&بالاترین = thr->peek_object();
      vvp_darray*darray = بالاترین.peek<vvp_darray>();
      أكد(darray);

      darray->مجموعه_کلمه(adr, value);
      أرجع درست;
}

/*
 * %set/dar/obj/real <index>
 */
منطقي از_مجموعه_DAR_OBJ_REAL(vthread_t thr, vvp_کد_t cp)
{
      أرجع مجموعه_dar_obj<مزدوج>(thr, cp);
}

/*
 * %set/dar/obj/str <index>
 */
منطقي از_مجموعه_DAR_OBJ_STR(vthread_t thr, vvp_کد_t cp)
{
      أرجع مجموعه_dar_obj<string>(thr, cp);
}

/*
 * %set/dar/obj/vec4 <index>
 */
منطقي از_مجموعه_DAR_OBJ_VEC4(vthread_t thr, vvp_کد_t cp)
{
      أرجع مجموعه_dar_obj<vvp_vector4_t>(thr, cp);
}

/*
 * %shiftl <idx>
 *
 * Pop the operand, then push the result.
 */
منطقي از_SHIFTL(vthread_t thr, vvp_کد_t cp)
{
      صحيح use_شاخص = cp->عدد;
      uint64_t لغزش = thr->واژگان[use_شاخص].w_uint;

      vvp_vector4_t&val = thr->peek_vec4();
      بیعلامت wid  = val.size();

      إذا (thr->flags[4] == BIT4_1) {
	      // The result is 'bx if the shift amount is undefined
	    val = vvp_vector4_t(wid, BIT4_X);

      } وإلا إذا (thr->flags[4] == BIT4_X || لغزش >= wid) {
	      // Shift is so big that all value is shifted out. Write
	      // a constant 0 result.
	    val = vvp_vector4_t(wid, BIT4_0);

      } وإلا إذا (لغزش > 0) {
	    vvp_vector4_t blk = val.subvalue(0, wid-لغزش);
	    vvp_vector4_t tmp (لغزش, BIT4_0);
	    val.مجموعه_vec(0, tmp);
	    val.مجموعه_vec(لغزش, blk);
      }

      أرجع درست;
}

/*
 * %shiftr <idx>
 * This is an unsigned right shift. The <idx> is a number that selects
 * the index register with the amount of the shift. This instruction
 * checks flag bit 4, which will be true if the shift is invalid.
 */
منطقي از_SHIFTR(vthread_t thr, vvp_کد_t cp)
{
      صحيح use_شاخص = cp->عدد;
      uint64_t لغزش = thr->واژگان[use_شاخص].w_uint;

      vvp_vector4_t val = thr->برداشت_vec4();
      بیعلامت wid  = val.size();

      إذا (thr->flags[4] == BIT4_1) {
	    val = vvp_vector4_t(wid, BIT4_X);

      } وإلا إذا (thr->flags[4] == BIT4_X || لغزش > wid) {
	    val = vvp_vector4_t(wid, BIT4_0);

      } وإلا إذا (لغزش > 0) {
	    vvp_vector4_t blk = val.subvalue(لغزش, wid-لغزش);
	    vvp_vector4_t tmp (لغزش, BIT4_0);
	    val.مجموعه_vec(0, blk);
	    val.مجموعه_vec(wid-لغزش, tmp);
      }

      thr->افزودن_vec4(val);
      أرجع درست;
}

/*
 *  %shiftr/s <wid>
 */
منطقي از_SHIFTR_S(vthread_t thr, vvp_کد_t cp)
{
      صحيح use_شاخص = cp->عدد;
      uint64_t لغزش = thr->واژگان[use_شاخص].w_uint;

      vvp_vector4_t val = thr->برداشت_vec4();
      بیعلامت wid  = val.size();

      vvp_bit4_t sign_بیت = val.value(val.size()-1);

      إذا (thr->flags[4] == BIT4_1) {
	    val = vvp_vector4_t(wid, BIT4_X);

      } وإلا إذا (thr->flags[4] == BIT4_X || لغزش > wid) {
	    val = vvp_vector4_t(wid, sign_بیت);

      } وإلا إذا (لغزش > 0) {
	    vvp_vector4_t blk = val.subvalue(لغزش, wid-لغزش);
	    vvp_vector4_t tmp (لغزش, sign_بیت);
	    val.مجموعه_vec(0, blk);
	    val.مجموعه_vec(wid-لغزش, tmp);
      }

      thr->افزودن_vec4(val);
      أرجع درست;
}

/*
 * %split/vec4 <wid>
 *   Pop 1 value,
 *   Take <wid> bits from the lsb,
 *   Push the remaining msb,
 *   Push the lsb.
 */
منطقي از_تقسیم_VEC4(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت lsb_wid = cp->عدد;

      vvp_vector4_t&val = thr->peek_vec4();
      أكد(lsb_wid < val.size());

      vvp_vector4_t lsb = val.subvalue(0, lsb_wid);
      val = val.subvalue(lsb_wid, val.size()-lsb_wid);

      thr->افزودن_vec4(lsb);
      أرجع درست;
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
مضمن ساكن فراغ dar_برداشت_مقدار(vthread_t thr, مزدوج&value)
{
      value = thr->برداشت_real();
}

مضمن ساكن فراغ dar_برداشت_مقدار(vthread_t thr, string&value)
{
      value = thr->برداشت_str();
}

مضمن ساكن فراغ dar_برداشت_مقدار(vthread_t thr, vvp_vector4_t&value)
{
      value = thr->برداشت_vec4();
}

قالب <نام_نوع ELEM>
ساكن منطقي store_dar(vthread_t thr, vvp_کد_t cp)
{
      int64_t adr = thr->واژگان[3].w_int;
      ELEM value;
	// FIXME: Can we get the size of the underlying array element
	//        and then use the normal pop_value?
      dar_برداشت_مقدار(thr, value);

      vvp_net_t*net = cp->net;
      أكد(net);

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      أكد(obj);

      vvp_darray*darray = obj->get_object().peek<vvp_darray>();

      إذا (adr < 0)
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to a negative " << get_darray_type(value)
	         << " index (" << adr << ")." << پایان_سطر;
      وإلا إذا (thr->flags[4] != BIT4_0)
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to an undefined " << get_darray_type(value)
	         << " index." << پایان_سطر;
      وإلا إذا (darray)
	    darray->مجموعه_کلمه(adr, value);
      وإلا
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to an undefined " << get_darray_type(value)
	         << "." << پایان_سطر;

      أرجع درست;
}

/*
 * %store/dar/real <var>
 */
منطقي از_STORE_DAR_R(vthread_t thr, vvp_کد_t cp)
{
      أرجع store_dar<مزدوج>(thr, cp);
}

/*
 * %store/dar/str <var>
 */
منطقي از_STORE_DAR_STR(vthread_t thr, vvp_کد_t cp)
{
      أرجع store_dar<string>(thr, cp);
}

/*
 * %store/dar/vec4 <var>
 */
منطقي از_STORE_DAR_VEC4(vthread_t thr, vvp_کد_t cp)
{
      أرجع store_dar<vvp_vector4_t>(thr, cp);
}

منطقي از_STORE_OBJ(vthread_t thr, vvp_کد_t cp)
{
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);

      vvp_object_t val;
      thr->برداشت_object(val);

      vvp_send_object(ptr, val, thr->wt_context);

      أرجع درست;
}

/*
 * %store/obja <array-label> <index>
 */
منطقي از_STORE_OBJA(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت idx = cp->بیت_idx[0];
      بیعلامت adr = thr->واژگان[idx].w_int;

      vvp_object_t val;
      thr->برداشت_object(val);

      cp->array->مجموعه_کلمه(adr, val);

      أرجع درست;
}


/*
 * %store/prop/obj <pid>, <idx>
 *
 * Pop an object value from the object stack, and store the value into
 * the property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
منطقي از_STORE_PROP_OBJ(vthread_t thr, vvp_کد_t cp)
{
      size_t pid = cp->عدد;
      بیعلامت idx = cp->بیت_idx[0];

      إذا (idx != 0) {
	    أكد(idx < vthread_s::واژگان_شمار);
	    idx = thr->واژگان[idx].w_uint;
      }

      vvp_object_t val;
      thr->برداشت_object(val);

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      أكد(cobj);

      cobj->مجموعه_object(pid, val, idx);

      أرجع درست;
}

ساكن فراغ برداشت_prop_val(vthread_t thr, مزدوج&val, بیعلامت)
{
      val = thr->برداشت_real();
}

ساكن فراغ برداشت_prop_val(vthread_t thr, string&val, بیعلامت)
{
      val = thr->برداشت_str();
}

ساكن فراغ برداشت_prop_val(vthread_t thr, vvp_vector4_t&val, بیعلامت wid)
{
      val = thr->برداشت_vec4();
      أكد(val.size() >= wid);
      val.resize(wid);
}

ساكن فراغ مجموعه_val(vvp_cobject*cobj, size_t pid, ثابت مزدوج&val)
{
      cobj->مجموعه_real(pid, val);
}

ساكن فراغ مجموعه_val(vvp_cobject*cobj, size_t pid, ثابت string&val)
{
      cobj->مجموعه_string(pid, val);
}

ساكن فراغ مجموعه_val(vvp_cobject*cobj, size_t pid, ثابت vvp_vector4_t&val)
{
      cobj->مجموعه_vec4(pid, val);
}

قالب <نام_نوع ELEM>
ساكن منطقي store_prop(vthread_t thr, vvp_کد_t cp, بیعلامت wid=0)
{
      size_t pid = cp->عدد;
      ELEM val;
      برداشت_prop_val(thr, val, wid); // Pop the value to store.

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      أكد(cobj);

      مجموعه_val(cobj, pid, val);

      أرجع درست;
}

/*
 * %store/prop/r <id>
 *
 * Pop a real value from the real stack, and store the value into the
 * property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
منطقي از_STORE_PROP_R(vthread_t thr, vvp_کد_t cp)
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
منطقي از_STORE_PROP_STR(vthread_t thr, vvp_کد_t cp)
{
      أرجع store_prop<string>(thr, cp);
}

/*
 * %store/prop/v <pid>, <wid>
 *
 * Store vector value into property <id> of cobject in the top of the
 * stack. Do NOT pop the object stack.
 */
منطقي از_STORE_PROP_V(vthread_t thr, vvp_کد_t cp)
{
      أرجع store_prop<vvp_vector4_t>(thr, cp, cp->بیت_idx[0]);
}

قالب <صنف QTYPE>
ساكن QTYPE* get_صف_prop(vvp_cobject*cobj, size_t pid)
{
      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      QTYPE* صف = qobj.peek<QTYPE>();
      إذا (صف == 0) {
	    صف = جديد QTYPE;
	    vvp_object_t val(صف);
	    cobj->مجموعه_object(pid, val, 0);
      }
      أرجع صف;
}

قالب <نام_نوع ELEM, صنف QTYPE>
ساكن منطقي store_prop_qb(vthread_t thr, vvp_کد_t cp, بیعلامت wid)
{
      size_t pid = cp->عدد;
      ELEM value;
      بیعلامت max_اندازه = thr->واژگان[cp->بیت_idx[0]].w_uint;
      برداشت_مقدار(thr, value, wid);

      vvp_object_t& بالاترین = thr->peek_object();
      vvp_cobject*cobj = بالاترین.peek<vvp_cobject>();
      أكد(cobj);

      QTYPE* صف = get_صف_prop<QTYPE>(cobj, pid);
      أكد(صف);
      صف->push_back(value, max_اندازه);
      أرجع درست;
}

قالب <نام_نوع ELEM, صنف QTYPE>
ساكن منطقي store_prop_qf(vthread_t thr, vvp_کد_t cp, بیعلامت wid)
{
      size_t pid = cp->عدد;
      ELEM value;
      بیعلامت max_اندازه = thr->واژگان[cp->بیت_idx[0]].w_uint;
      برداشت_مقدار(thr, value, wid);

      vvp_object_t& بالاترین = thr->peek_object();
      vvp_cobject*cobj = بالاترین.peek<vvp_cobject>();
      أكد(cobj);

      QTYPE* صف = get_صف_prop<QTYPE>(cobj, pid);
      أكد(صف);
      صف->push_front(value, max_اندازه);
      أرجع درست;
}

منطقي از_STORE_PROP_QB_R(vthread_t thr, vvp_کد_t cp)
{
      أرجع store_prop_qb<مزدوج, vvp_صف_real>(thr, cp, 0);
}

منطقي از_STORE_PROP_QB_STR(vthread_t thr, vvp_کد_t cp)
{
      أرجع store_prop_qb<string, vvp_صف_string>(thr, cp, 0);
}

منطقي از_STORE_PROP_QB_V(vthread_t thr, vvp_کد_t cp)
{
      أرجع store_prop_qb<vvp_vector4_t, vvp_صف_vec4>(thr, cp, cp->بیت_idx[1]);
}

منطقي از_STORE_PROP_QF_R(vthread_t thr, vvp_کد_t cp)
{
      أرجع store_prop_qf<مزدوج, vvp_صف_real>(thr, cp, 0);
}

منطقي از_STORE_PROP_QF_STR(vthread_t thr, vvp_کد_t cp)
{
      أرجع store_prop_qf<string, vvp_صف_string>(thr, cp, 0);
}

منطقي از_STORE_PROP_QF_V(vthread_t thr, vvp_کد_t cp)
{
      أرجع store_prop_qf<vvp_vector4_t, vvp_صف_vec4>(thr, cp, cp->بیت_idx[1]);
}

قالب <نام_نوع ELEM, صنف QTYPE>
ساكن منطقي qinsert_prop(vthread_t thr, vvp_کد_t cp, بیعلامت wid)
{
      int64_t idx = thr->واژگان[3].w_int;
      ELEM value;
      size_t pid = cp->عدد;
      برداشت_مقدار(thr, value, wid);

      vvp_object_t& بالاترین = thr->peek_object();
      vvp_cobject*cobj = بالاترین.peek<vvp_cobject>();
      أكد(cobj);

      QTYPE* صف = get_صف_prop<QTYPE>(cobj, pid);
      أكد(صف);
      إذا (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at a negative "
	         << get_صف_type(value)
	         << " index (" << idx << "). ";
	    print_صف_مقدار(value);
	    cerr << " was not added." << پایان_سطر;
      } وإلا إذا (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at an undefined "
	         << get_صف_type(value) << " index. ";
	    print_صف_مقدار(value);
	    cerr << " was not added." << پایان_سطر;
      } وإلا {
	    بیعلامت max_اندازه = thr->واژگان[cp->بیت_idx[0]].w_int;
	    صف->insert(idx, value, max_اندازه);
      }
      أرجع درست;
}

منطقي از_QINSERT_PROP_R(vthread_t thr, vvp_کد_t cp)
{
      أرجع qinsert_prop<مزدوج, vvp_صف_real>(thr, cp, 0);
}

منطقي از_QINSERT_PROP_STR(vthread_t thr, vvp_کد_t cp)
{
      أرجع qinsert_prop<string, vvp_صف_string>(thr, cp, 0);
}

منطقي از_QINSERT_PROP_V(vthread_t thr, vvp_کد_t cp)
{
      أرجع qinsert_prop<vvp_vector4_t, vvp_صف_vec4>(thr, cp, cp->بیت_idx[1]);
}

قالب <نام_نوع ELEM, صنف QTYPE>
ساكن منطقي q_برداشت_prop(vthread_t thr, vvp_کد_t cp,
                       فراغ (*get_val_func)(vvp_صف*, ELEM&),
                       ثابت حرف*loc, بیعلامت wid)
{
      size_t pid = cp->عدد;

      vvp_object_t& بالاترین = thr->peek_object();
      vvp_cobject*cobj = بالاترین.peek<vvp_cobject>();
      أكد(cobj);

      QTYPE* صف = get_صف_prop<QTYPE>(cobj, pid);
      أكد(صف);

      size_t size = صف->get_اندازه();

      ELEM value;
      إذا (size) {
	    get_val_func(صف, value);
      } وإلا {
	    dq_default(value, wid);
	    cerr << thr->get_fileline()
	         << "Warning: pop_" << loc << "() on empty "
	         << get_صف_type(value) << "." << پایان_سطر;
      }

      افزودن_مقدار(thr, value, wid);
      أرجع درست;
}

قالب <نام_نوع ELEM, صنف QTYPE>
ساكن منطقي qpop_b_prop(vthread_t thr, vvp_کد_t cp, بیعلامت wid)
{
      أرجع q_برداشت_prop<ELEM, QTYPE>(thr, cp, get_پشت_مقدار<ELEM>, "back", wid);
}

قالب <نام_نوع ELEM, صنف QTYPE>
ساكن منطقي qpop_f_prop(vthread_t thr, vvp_کد_t cp, بیعلامت wid)
{
      أرجع q_برداشت_prop<ELEM, QTYPE>(thr, cp, get_front_مقدار<ELEM>, "front", wid);
}

منطقي از_QPOP_PROP_B_REAL(vthread_t thr, vvp_کد_t cp)
{
      أرجع qpop_b_prop<مزدوج, vvp_صف_real>(thr, cp, 0);
}

منطقي از_QPOP_PROP_B_STR(vthread_t thr, vvp_کد_t cp)
{
      أرجع qpop_b_prop<string, vvp_صف_string>(thr, cp, 0);
}

منطقي از_QPOP_PROP_B_V(vthread_t thr, vvp_کد_t cp)
{
      أرجع qpop_b_prop<vvp_vector4_t, vvp_صف_vec4>(thr, cp, cp->بیت_idx[0]);
}

منطقي از_QPOP_PROP_F_REAL(vthread_t thr, vvp_کد_t cp)
{
      أرجع qpop_f_prop<مزدوج, vvp_صف_real>(thr, cp, 0);
}

منطقي از_QPOP_PROP_F_STR(vthread_t thr, vvp_کد_t cp)
{
      أرجع qpop_f_prop<string, vvp_صف_string>(thr, cp, 0);
}

منطقي از_QPOP_PROP_F_V(vthread_t thr, vvp_کد_t cp)
{
      أرجع qpop_f_prop<vvp_vector4_t, vvp_صف_vec4>(thr, cp, cp->بیت_idx[0]);
}

ساكن vvp_vector4_t صف_unique_ulong_مقصد_vec4(بیعلامت دراز ul, بیعلامت بیتها)
{
      vvp_vector4_t val(بیتها, BIT4_0);
      لكل (بیعلامت idx = 0; idx < بیتها && idx < 8 * حجم(بیعلامت دراز); idx += 1) {
	    إذا (ul & 1UL) {
		  val.مجموعه_بیت(idx, BIT4_1);
	    } وإلا {
		  val.مجموعه_بیت(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      أرجع val;
}

قالب <صنف SRC> ساكن vvp_صف_vec4* صف_پیاپی_unique_src(SRC* src,
								 بیعلامت wid,
								 منطقي as_شاخص)
{
      vvp_صف_vec4* dst = جديد vvp_صف_vec4();
      إذا (!src) أرجع dst;
      size_t n = src->get_اندازه();
      لكل (size_t i = 0; i < n; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_کلمه(i, vi);
	    منطقي دیده = خطأ;
	    لكل (size_t j = 0; j < i; j += 1) {
		  vvp_vector4_t vj(wid);
		  src->get_کلمه(j, vj);
		  إذا (vi.eeq(vj)) {
			دیده = درست;
			اكسر;
		  }
	    }
	    إذا (دیده) استمر;
	    إذا (as_شاخص) {
		  dst->push_back(صف_unique_ulong_مقصد_vec4((بیعلامت دراز)i, 32), 0);
	    } وإلا {
		  dst->push_back(vi, 0);
	    }
      }
      أرجع dst;
}

قالب <صنف SRC>
ساكن vvp_صف_vec4* صف_پیاپی_پیدا_کن_matches_src(SRC* src, بیعلامت wid,
						  ثابت vvp_vector4_t& cmp,
						  منطقي as_شاخص)
{
      vvp_صف_vec4* dst = جديد vvp_صف_vec4();
      إذا (!src) أرجع dst;
      size_t n = src->get_اندازه();
      لكل (size_t i = 0; i < n; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_کلمه(i, vi);
	    إذا (vi.eeq(cmp)) {
		  إذا (as_شاخص) {
			dst->push_back(صف_unique_ulong_مقصد_vec4((بیعلامت دراز)i, 32), 0);
		  } وإلا {
			dst->push_back(vi, 0);
		  }
	    }
      }
      أرجع dst;
}

/* Array/queue locator methods return a queue with 0 or 1 elements (LRM). */
قالب <صنف SRC>
ساكن vvp_صف_vec4* صف_پیدا_کن_اولی_آخرین_مقصد_صف_src(SRC* src,
							  بیعلامت wid,
							  ثابت vvp_vector4_t& cmp,
							  منطقي want_اولی,
							  منطقي want_شاخص)
{
      vvp_صف_vec4* dst = جديد vvp_صف_vec4();
      إذا (!src || src->get_اندازه() == 0) أرجع dst;
      size_t n = src->get_اندازه();
      إذا (want_اولی) {
	    لكل (size_t i = 0; i < n; i += 1) {
		  vvp_vector4_t vi(wid);
		  src->get_کلمه(i, vi);
		  إذا (vi.eeq(cmp)) {
			إذا (want_شاخص) {
			      dst->push_back(
				  صف_unique_ulong_مقصد_vec4((بیعلامت دراز)i, 32),
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
		  src->get_کلمه(i, vi);
		  إذا (vi.eeq(cmp)) {
			إذا (want_شاخص) {
			      dst->push_back(
				  صف_unique_ulong_مقصد_vec4((بیعلامت دراز)i, 32),
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
قالب <صنف SRC>
ساكن vvp_صف_vec4* صف_پیاپی_min_max_src(SRC* src, بیعلامت wid, منطقي want_max)
{
      vvp_صف_vec4* dst = جديد vvp_صف_vec4();
      إذا (!src || src->get_اندازه() == 0) أرجع dst;

      vvp_vector4_t بهترین(wid);
      src->get_کلمه(0, بهترین);

      size_t n = src->get_اندازه();
      لكل (size_t i = 1 ; i < n ; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_کلمه(i, vi);

	    vvp_bit4_t ge = compare_gtge(vi, بهترین, BIT4_1);
	    منطقي replace = want_max ? (ge == BIT4_1) : (ge == BIT4_0);
	    إذا (replace) بهترین = vi;
      }

      dst->push_back(بهترین, 0);
      أرجع dst;
}

منطقي از_صف_پیدا_کن_V(vthread_t thr, vvp_کد_t cp)
{
      vvp_vector4_t cmp = thr->برداشت_vec4();
      بیعلامت wid = cp->بیت_idx[0];
      أكد(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_صف_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_صف_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_صف_vec4* dst;
      إذا (qsrc) dst = صف_پیاپی_پیدا_کن_matches_src(qsrc, wid, cmp, خطأ);
      وإلا إذا (dsrc) dst = صف_پیاپی_پیدا_کن_matches_src(dsrc, wid, cmp, خطأ);
      وإلا {
	    vvp_صف* src_q = get_صف_object<vvp_صف_vec4>(thr, net);
	    vvp_صف_vec4* src = dynamic_cast<vvp_صف_vec4*>(src_q);
	    dst = صف_پیاپی_پیدا_کن_matches_src(src, wid, cmp, خطأ);
      }
      thr->افزودن_object(vvp_object_t(dst));
      أرجع درست;
}

منطقي از_صف_پیدا_کن_PROP_V(vthread_t thr, vvp_کد_t cp)
{
      vvp_vector4_t cmp = thr->برداشت_vec4();
      size_t pid = cp->عدد;
      بیعلامت wid = cp->بیت_idx[0];
      أكد(cmp.size() == wid);

      vvp_object_t& بالاترین = thr->peek_object();
      vvp_cobject*cobj = بالاترین.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_صف_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_صف_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_صف_vec4* dst = qsrc ? صف_پیاپی_پیدا_کن_matches_src(qsrc, wid, cmp, خطأ)
				 : صف_پیاپی_پیدا_کن_matches_src(dsrc, wid, cmp, خطأ);
      thr->افزودن_object(vvp_object_t(dst));
      أرجع درست;
}

منطقي از_صف_پیدا_کن_شاخص_V(vthread_t thr, vvp_کد_t cp)
{
      vvp_vector4_t cmp = thr->برداشت_vec4();
      بیعلامت wid = cp->بیت_idx[0];
      أكد(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_صف_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_صف_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_صف_vec4* dst;
      إذا (qsrc) dst = صف_پیاپی_پیدا_کن_matches_src(qsrc, wid, cmp, درست);
      وإلا إذا (dsrc) dst = صف_پیاپی_پیدا_کن_matches_src(dsrc, wid, cmp, درست);
      وإلا {
	    vvp_صف* src_q = get_صف_object<vvp_صف_vec4>(thr, net);
	    vvp_صف_vec4* src = dynamic_cast<vvp_صف_vec4*>(src_q);
	    dst = صف_پیاپی_پیدا_کن_matches_src(src, wid, cmp, درست);
      }
      thr->افزودن_object(vvp_object_t(dst));
      أرجع درست;
}

منطقي از_صف_پیدا_کن_شاخص_PROP_V(vthread_t thr, vvp_کد_t cp)
{
      vvp_vector4_t cmp = thr->برداشت_vec4();
      size_t pid = cp->عدد;
      بیعلامت wid = cp->بیت_idx[0];
      أكد(cmp.size() == wid);

      vvp_object_t& بالاترین = thr->peek_object();
      vvp_cobject*cobj = بالاترین.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_صف_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_صف_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_صف_vec4* dst = qsrc ? صف_پیاپی_پیدا_کن_matches_src(qsrc, wid, cmp, درست)
				 : صف_پیاپی_پیدا_کن_matches_src(dsrc, wid, cmp, درست);
      thr->افزودن_object(vvp_object_t(dst));
      أرجع درست;
}

منطقي از_صف_پیدا_کن_اولی_V(vthread_t thr, vvp_کد_t cp)
{
      vvp_vector4_t cmp = thr->برداشت_vec4();
      بیعلامت wid = cp->بیت_idx[0];
      أكد(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_صف_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_صف_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_صف_vec4* dst;
      إذا (qsrc) dst = صف_پیدا_کن_اولی_آخرین_مقصد_صف_src(qsrc, wid, cmp, درست, خطأ);
      وإلا إذا (dsrc) dst = صف_پیدا_کن_اولی_آخرین_مقصد_صف_src(dsrc, wid, cmp, درست, خطأ);
      وإلا {
	    vvp_صف* src_q = get_صف_object<vvp_صف_vec4>(thr, net);
	    vvp_صف_vec4* src = dynamic_cast<vvp_صف_vec4*>(src_q);
	    dst = صف_پیدا_کن_اولی_آخرین_مقصد_صف_src(src, wid, cmp, درست, خطأ);
      }
      thr->افزودن_object(vvp_object_t(dst));
      أرجع درست;
}

منطقي از_صف_پیدا_کن_اولی_PROP_V(vthread_t thr, vvp_کد_t cp)
{
      vvp_vector4_t cmp = thr->برداشت_vec4();
      size_t pid = cp->عدد;
      بیعلامت wid = cp->بیت_idx[0];
      أكد(cmp.size() == wid);

      vvp_object_t& بالاترین = thr->peek_object();
      vvp_cobject*cobj = بالاترین.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_صف_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_صف_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_صف_vec4* dst =
	    qsrc ? صف_پیدا_کن_اولی_آخرین_مقصد_صف_src(qsrc, wid, cmp, درست, خطأ)
		 : صف_پیدا_کن_اولی_آخرین_مقصد_صف_src(dsrc, wid, cmp, درست, خطأ);
      thr->افزودن_object(vvp_object_t(dst));
      أرجع درست;
}

منطقي از_صف_پیدا_کن_اولی_شاخص_V(vthread_t thr, vvp_کد_t cp)
{
      vvp_vector4_t cmp = thr->برداشت_vec4();
      بیعلامت wid = cp->بیت_idx[0];
      أكد(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_صف_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_صف_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_صف_vec4* dst;
      إذا (qsrc) dst = صف_پیدا_کن_اولی_آخرین_مقصد_صف_src(qsrc, wid, cmp, درست, درست);
      وإلا إذا (dsrc) dst = صف_پیدا_کن_اولی_آخرین_مقصد_صف_src(dsrc, wid, cmp, درست, درست);
      وإلا {
	    vvp_صف* src_q = get_صف_object<vvp_صف_vec4>(thr, net);
	    vvp_صف_vec4* src = dynamic_cast<vvp_صف_vec4*>(src_q);
	    dst = صف_پیدا_کن_اولی_آخرین_مقصد_صف_src(src, wid, cmp, درست, درست);
      }
      thr->افزودن_object(vvp_object_t(dst));
      أرجع درست;
}

منطقي از_صف_پیدا_کن_اولی_شاخص_PROP_V(vthread_t thr, vvp_کد_t cp)
{
      vvp_vector4_t cmp = thr->برداشت_vec4();
      size_t pid = cp->عدد;
      بیعلامت wid = cp->بیت_idx[0];
      أكد(cmp.size() == wid);

      vvp_object_t& بالاترین = thr->peek_object();
      vvp_cobject*cobj = بالاترین.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_صف_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_صف_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_صف_vec4* dst =
	    qsrc ? صف_پیدا_کن_اولی_آخرین_مقصد_صف_src(qsrc, wid, cmp, درست, درست)
		 : صف_پیدا_کن_اولی_آخرین_مقصد_صف_src(dsrc, wid, cmp, درست, درست);
      thr->افزودن_object(vvp_object_t(dst));
      أرجع درست;
}

منطقي از_صف_پیدا_کن_آخرین_V(vthread_t thr, vvp_کد_t cp)
{
      vvp_vector4_t cmp = thr->برداشت_vec4();
      بیعلامت wid = cp->بیت_idx[0];
      أكد(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_صف_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_صف_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_صف_vec4* dst;
      إذا (qsrc) dst = صف_پیدا_کن_اولی_آخرین_مقصد_صف_src(qsrc, wid, cmp, خطأ, خطأ);
      وإلا إذا (dsrc) dst = صف_پیدا_کن_اولی_آخرین_مقصد_صف_src(dsrc, wid, cmp, خطأ, خطأ);
      وإلا {
	    vvp_صف* src_q = get_صف_object<vvp_صف_vec4>(thr, net);
	    vvp_صف_vec4* src = dynamic_cast<vvp_صف_vec4*>(src_q);
	    dst = صف_پیدا_کن_اولی_آخرین_مقصد_صف_src(src, wid, cmp, خطأ, خطأ);
      }
      thr->افزودن_object(vvp_object_t(dst));
      أرجع درست;
}

منطقي از_صف_پیدا_کن_آخرین_PROP_V(vthread_t thr, vvp_کد_t cp)
{
      vvp_vector4_t cmp = thr->برداشت_vec4();
      size_t pid = cp->عدد;
      بیعلامت wid = cp->بیت_idx[0];
      أكد(cmp.size() == wid);

      vvp_object_t& بالاترین = thr->peek_object();
      vvp_cobject*cobj = بالاترین.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_صف_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_صف_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_صف_vec4* dst =
	    qsrc ? صف_پیدا_کن_اولی_آخرین_مقصد_صف_src(qsrc, wid, cmp, خطأ, خطأ)
		 : صف_پیدا_کن_اولی_آخرین_مقصد_صف_src(dsrc, wid, cmp, خطأ, خطأ);
      thr->افزودن_object(vvp_object_t(dst));
      أرجع درست;
}

منطقي از_صف_پیدا_کن_آخرین_شاخص_V(vthread_t thr, vvp_کد_t cp)
{
      vvp_vector4_t cmp = thr->برداشت_vec4();
      بیعلامت wid = cp->بیت_idx[0];
      أكد(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_صف_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_صف_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_صف_vec4* dst;
      إذا (qsrc) dst = صف_پیدا_کن_اولی_آخرین_مقصد_صف_src(qsrc, wid, cmp, خطأ, درست);
      وإلا إذا (dsrc) dst = صف_پیدا_کن_اولی_آخرین_مقصد_صف_src(dsrc, wid, cmp, خطأ, درست);
      وإلا {
	    vvp_صف* src_q = get_صف_object<vvp_صف_vec4>(thr, net);
	    vvp_صف_vec4* src = dynamic_cast<vvp_صف_vec4*>(src_q);
	    dst = صف_پیدا_کن_اولی_آخرین_مقصد_صف_src(src, wid, cmp, خطأ, درست);
      }
      thr->افزودن_object(vvp_object_t(dst));
      أرجع درست;
}

منطقي از_صف_پیدا_کن_آخرین_شاخص_PROP_V(vthread_t thr, vvp_کد_t cp)
{
      vvp_vector4_t cmp = thr->برداشت_vec4();
      size_t pid = cp->عدد;
      بیعلامت wid = cp->بیت_idx[0];
      أكد(cmp.size() == wid);

      vvp_object_t& بالاترین = thr->peek_object();
      vvp_cobject*cobj = بالاترین.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_صف_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_صف_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_صف_vec4* dst =
	    qsrc ? صف_پیدا_کن_اولی_آخرین_مقصد_صف_src(qsrc, wid, cmp, خطأ, درست)
		 : صف_پیدا_کن_اولی_آخرین_مقصد_صف_src(dsrc, wid, cmp, خطأ, درست);
      thr->افزودن_object(vvp_object_t(dst));
      أرجع درست;
}

منطقي از_صف_MIN_V(vthread_t thr, vvp_کد_t cp)
{
      vvp_net_t* net = cp->net;
      بیعلامت wid = cp->بیت_idx[0];
      vvp_صف_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_صف_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_صف_vec4* dst;
      إذا (qsrc) dst = صف_پیاپی_min_max_src(qsrc, wid, خطأ);
      وإلا إذا (dsrc) dst = صف_پیاپی_min_max_src(dsrc, wid, خطأ);
      وإلا {
	    vvp_صف* src_q = get_صف_object<vvp_صف_vec4>(thr, net);
	    vvp_صف_vec4* src = dynamic_cast<vvp_صف_vec4*>(src_q);
	    dst = صف_پیاپی_min_max_src(src, wid, خطأ);
      }
      thr->افزودن_object(vvp_object_t(dst));
      أرجع درست;
}

منطقي از_صف_MIN_OBJ_V(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت wid = cp->بیت_idx[0];
      vvp_object_t src_obj;
      thr->برداشت_object(src_obj);

      vvp_صف_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_صف_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_صف_vec4* dst = qsrc ? صف_پیاپی_min_max_src(qsrc, wid, خطأ)
				 : صف_پیاپی_min_max_src(dsrc, wid, خطأ);
      thr->افزودن_object(vvp_object_t(dst));
      أرجع درست;
}

منطقي از_صف_MIN_PROP_V(vthread_t thr, vvp_کد_t cp)
{
      size_t pid = cp->عدد;
      بیعلامت wid = cp->بیت_idx[0];

      vvp_object_t& بالاترین = thr->peek_object();
      vvp_cobject*cobj = بالاترین.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_صف_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_صف_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_صف_vec4* dst = qsrc ? صف_پیاپی_min_max_src(qsrc, wid, خطأ)
				 : صف_پیاپی_min_max_src(dsrc, wid, خطأ);
      thr->افزودن_object(vvp_object_t(dst));
      أرجع درست;
}

منطقي از_صف_MAX_V(vthread_t thr, vvp_کد_t cp)
{
      vvp_net_t* net = cp->net;
      بیعلامت wid = cp->بیت_idx[0];
      vvp_صف_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_صف_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_صف_vec4* dst;
      إذا (qsrc) dst = صف_پیاپی_min_max_src(qsrc, wid, درست);
      وإلا إذا (dsrc) dst = صف_پیاپی_min_max_src(dsrc, wid, درست);
      وإلا {
	    vvp_صف* src_q = get_صف_object<vvp_صف_vec4>(thr, net);
	    vvp_صف_vec4* src = dynamic_cast<vvp_صف_vec4*>(src_q);
	    dst = صف_پیاپی_min_max_src(src, wid, درست);
      }
      thr->افزودن_object(vvp_object_t(dst));
      أرجع درست;
}

منطقي از_صف_MAX_OBJ_V(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت wid = cp->بیت_idx[0];
      vvp_object_t src_obj;
      thr->برداشت_object(src_obj);

      vvp_صف_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_صف_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_صف_vec4* dst = qsrc ? صف_پیاپی_min_max_src(qsrc, wid, درست)
				 : صف_پیاپی_min_max_src(dsrc, wid, درست);
      thr->افزودن_object(vvp_object_t(dst));
      أرجع درست;
}

منطقي از_صف_MAX_PROP_V(vthread_t thr, vvp_کد_t cp)
{
      size_t pid = cp->عدد;
      بیعلامت wid = cp->بیت_idx[0];

      vvp_object_t& بالاترین = thr->peek_object();
      vvp_cobject*cobj = بالاترین.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_صف_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_صف_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_صف_vec4* dst = qsrc ? صف_پیاپی_min_max_src(qsrc, wid, درست)
				 : صف_پیاپی_min_max_src(dsrc, wid, درست);
      thr->افزودن_object(vvp_object_t(dst));
      أرجع درست;
}

منطقي از_صف_UNIQUE_OBJ_V(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت wid = cp->بیت_idx[0];
      vvp_object_t src_obj;
      thr->برداشت_object(src_obj);

      vvp_صف_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_صف_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_صف_vec4* dst = qsrc ? صف_پیاپی_unique_src(qsrc, wid, خطأ)
				 : صف_پیاپی_unique_src(dsrc, wid, خطأ);
      thr->افزودن_object(vvp_object_t(dst));
      أرجع درست;
}

منطقي از_صف_UNIQUE_شاخص_OBJ_V(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت wid = cp->بیت_idx[0];
      vvp_object_t src_obj;
      thr->برداشت_object(src_obj);

      vvp_صف_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_صف_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_صف_vec4* dst = qsrc ? صف_پیاپی_unique_src(qsrc, wid, درست)
				 : صف_پیاپی_unique_src(dsrc, wid, درست);
      thr->افزودن_object(vvp_object_t(dst));
      أرجع درست;
}

منطقي از_صف_UNIQUE_V(vthread_t thr, vvp_کد_t cp)
{
      vvp_net_t*net = cp->net;
      بیعلامت wid = cp->بیت_idx[0];
      vvp_صف_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_صف_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_صف_vec4* dst;
      إذا (qsrc) dst = صف_پیاپی_unique_src(qsrc, wid, خطأ);
      وإلا إذا (dsrc) dst = صف_پیاپی_unique_src(dsrc, wid, خطأ);
      وإلا {
	    vvp_صف* src_q = get_صف_object<vvp_صف_vec4>(thr, net);
	    vvp_صف_vec4* src = dynamic_cast<vvp_صف_vec4*>(src_q);
	    dst = صف_پیاپی_unique_src(src, wid, خطأ);
      }
      thr->افزودن_object(vvp_object_t(dst));
      أرجع درست;
}

منطقي از_صف_UNIQUE_PROP_V(vthread_t thr, vvp_کد_t cp)
{
      size_t pid = cp->عدد;
      بیعلامت wid = cp->بیت_idx[0];

      vvp_object_t& بالاترین = thr->peek_object();
      vvp_cobject*cobj = بالاترین.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_صف_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_صف_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_صف_vec4* dst = qsrc ? صف_پیاپی_unique_src(qsrc, wid, خطأ)
				 : صف_پیاپی_unique_src(dsrc, wid, خطأ);
      thr->افزودن_object(vvp_object_t(dst));
      أرجع درست;
}

منطقي از_صف_UNIQUE_شاخص_V(vthread_t thr, vvp_کد_t cp)
{
      vvp_net_t*net = cp->net;
      بیعلامت wid = cp->بیت_idx[0];
      vvp_صف_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_صف_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_صف_vec4* dst;
      إذا (qsrc) dst = صف_پیاپی_unique_src(qsrc, wid, درست);
      وإلا إذا (dsrc) dst = صف_پیاپی_unique_src(dsrc, wid, درست);
      وإلا {
	    vvp_صف* src_q = get_صف_object<vvp_صف_vec4>(thr, net);
	    vvp_صف_vec4* src = dynamic_cast<vvp_صف_vec4*>(src_q);
	    dst = صف_پیاپی_unique_src(src, wid, درست);
      }
      thr->افزودن_object(vvp_object_t(dst));
      أرجع درست;
}

منطقي از_صف_UNIQUE_شاخص_PROP_V(vthread_t thr, vvp_کد_t cp)
{
      size_t pid = cp->عدد;
      بیعلامت wid = cp->بیت_idx[0];

      vvp_object_t& بالاترین = thr->peek_object();
      vvp_cobject*cobj = بالاترین.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_صف_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_صف_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_صف_vec4* dst = qsrc ? صف_پیاپی_unique_src(qsrc, wid, درست)
				 : صف_پیاپی_unique_src(dsrc, wid, درست);
      thr->افزودن_object(vvp_object_t(dst));
      أرجع درست;
}

منطقي از_PROP_صف_اندازه(vthread_t thr, vvp_کد_t cp)
{
      size_t pid = cp->عدد;

      vvp_object_t& بالاترین = thr->peek_object();
      vvp_cobject*cobj = بالاترین.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_صف_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_صف_or_darray_vec4_from_object(qobj, qsrc, dsrc);

      size_t sz = qsrc ? qsrc->get_اندازه() : (dsrc ? dsrc->get_اندازه() : 0);

      vvp_vector4_t val(32, BIT4_0);
      بیعلامت دراز ul = sz;
      لكل (بیعلامت idx = 0; idx < 32; idx++) {
	    إذا (ul & 1UL) {
		  val.مجموعه_بیت(idx, BIT4_1);
	    } وإلا {
		  val.مجموعه_بیت(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      thr->افزودن_vec4(val);
      أرجع درست;
}

منطقي از_صف_اندازه_V(vthread_t thr, vvp_کد_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_صف_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_صف_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      size_t sz =
	    qsrc ? qsrc->get_اندازه() : (dsrc ? dsrc->get_اندازه() : 0);

      vvp_vector4_t val(32, BIT4_0);
      بیعلامت دراز ul = sz;
      لكل (بیعلامت idx = 0; idx < 32; idx++) {
	    إذا (ul & 1UL) {
		  val.مجموعه_بیت(idx, BIT4_1);
	    } وإلا {
		  val.مجموعه_بیت(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      thr->افزودن_vec4(val);
      أرجع درست;
}

منطقي از_صف_کلمه_V(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت wid = cp->بیت_idx[0];
      بیعلامت ix_reg = cp->بیت_idx[1];
      int64_t adr = thr->واژگان[ix_reg].w_int;
      إذا (thr->flags[4] == BIT4_1) {
	    thr->افزودن_vec4(vvp_vector4_t(wid));
	    أرجع درست;
      }
      إذا (adr < 0) {
	    thr->افزودن_vec4(vvp_vector4_t(wid));
	    أرجع درست;
      }
      vvp_net_t* net = cp->net;
      vvp_صف_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_صف_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t out(wid);
      إذا (qsrc) {
	    إذا ((size_t)adr >= qsrc->get_اندازه()) {
		  thr->افزودن_vec4(out);
		  أرجع درست;
	    }
	    qsrc->get_کلمه((بیعلامت)adr, out);
      } وإلا إذا (dsrc) {
	    إذا ((size_t)adr >= dsrc->get_اندازه()) {
		  thr->افزودن_vec4(out);
		  أرجع درست;
	    }
	    dsrc->get_کلمه((بیعلامت)adr, out);
      } وإلا {
	    vvp_صف* src_q = get_صف_object<vvp_صف_vec4>(thr, net);
	    vvp_صف_vec4* src = dynamic_cast<vvp_صف_vec4*>(src_q);
	    إذا (!src || (size_t)adr >= src->get_اندازه()) {
		  thr->افزودن_vec4(out);
		  أرجع درست;
	    }
	    src->get_کلمه((بیعلامت)adr, out);
      }
      thr->افزودن_vec4(out);
      أرجع درست;
}

منطقي از_صف_کلمه_PROP_V(vthread_t thr, vvp_کد_t cp)
{
      size_t pid = cp->عدد;
      بیعلامت wid = cp->بیت_idx[0];
      بیعلامت ix_reg = cp->بیت_idx[1];
      int64_t adr = thr->واژگان[ix_reg].w_int;
      إذا (thr->flags[4] == BIT4_1) {
	    thr->افزودن_vec4(vvp_vector4_t(wid));
	    أرجع درست;
      }
      إذا (adr < 0) {
	    thr->افزودن_vec4(vvp_vector4_t(wid));
	    أرجع درست;
      }

      vvp_object_t saved_بالاترین;
      منطقي popped_بالاترین = خطأ;
      vvp_object_t& بالاترین = thr->peek_object();
      vvp_cobject*cobj = بالاترین.peek<vvp_cobject>();
      إذا (cobj == 0) {
	    // Array-locator with(property) keeps an accumulator queue on top.
	    // Temporarily pop it to access the class object beneath.
	    thr->برداشت_object(saved_بالاترین);
	    popped_بالاترین = درست;
	    vvp_object_t& زیر = thr->peek_object();
	    cobj = زیر.peek<vvp_cobject>();
      }
      إذا (cobj == 0) {
	    إذا (popped_بالاترین) thr->افزودن_object(saved_بالاترین);
	    thr->افزودن_vec4(vvp_vector4_t(wid));
	    أرجع درست;
      }

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_صف_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_صف_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t out(wid);
      إذا (qsrc) {
	    إذا ((size_t)adr >= qsrc->get_اندازه()) {
		  thr->افزودن_vec4(out);
		  أرجع درست;
	    }
	    qsrc->get_کلمه((بیعلامت)adr, out);
      } وإلا إذا (dsrc) {
	    إذا ((size_t)adr >= dsrc->get_اندازه()) {
		  thr->افزودن_vec4(out);
		  أرجع درست;
	    }
	    dsrc->get_کلمه((بیعلامت)adr, out);
      } وإلا {
	    thr->افزودن_vec4(out);
	    إذا (popped_بالاترین) thr->افزودن_object(saved_بالاترین);
	    أرجع درست;
      }
      إذا (popped_بالاترین) thr->افزودن_object(saved_بالاترین);
      thr->افزودن_vec4(out);
      أرجع درست;
}

منطقي از_CMPIX_LTU(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت a = cp->عدد;
      بیعلامت b = cp->بیت_idx[0];
      إذا (thr->واژگان[a].w_uint < thr->واژگان[b].w_uint) {
	    thr->flags[4] = BIT4_1;
      } وإلا {
	    thr->flags[4] = BIT4_0;
      }
      أرجع درست;
}

منطقي از_CMPIX_SLT0(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت r = cp->عدد;
      thr->flags[4] = (thr->واژگان[r].w_int < 0) ? BIT4_1 : BIT4_0;
      أرجع درست;
}

منطقي از_افزودن_IX_VEC4(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت ix = cp->عدد;
      بیعلامت wid = cp->بیت_idx[0];
      بیعلامت sgn = cp->بیت_idx[1];
      uint32_t ubits;
      إذا (sgn) {
	    int32_t sv = (int32_t)thr->واژگان[ix].w_int;
	    ubits = (uint32_t)sv;
      } وإلا {
	    ubits = (uint32_t)thr->واژگان[ix].w_uint;
      }
      إذا (wid > 32) wid = 32;
      vvp_vector4_t tmp(wid, BIT4_0);
      لكل (بیعلامت idx = 0; idx < wid; idx++) {
	    إذا (ubits & 1u) {
		  tmp.مجموعه_بیت(idx, BIT4_1);
	    } وإلا {
		  tmp.مجموعه_بیت(idx, BIT4_0);
	    }
	    ubits >>= 1;
      }
      thr->افزودن_vec4(tmp);
      أرجع درست;
}

منطقي از_صف_NEW_خالی_V(vthread_t thr, vvp_کد_t)
{
      thr->افزودن_object(vvp_object_t(جديد vvp_صف_vec4()));
      أرجع درست;
}

منطقي از_صف_APPEND_کلمه_V(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت wid = cp->بیت_idx[0];
      vvp_vector4_t elem = thr->برداشت_vec4();
      أكد(elem.size() == wid);
      vvp_object_t qobj;
      thr->برداشت_object(qobj);
      vvp_صف_vec4* q = qobj.peek<vvp_صف_vec4>();
      أكد(q);
      q->push_back(elem, 0);
      thr->افزودن_object(qobj);
      أرجع درست;
}

منطقي از_DELETE_PROP_ELEM(vthread_t thr, vvp_کد_t cp)
{
      size_t pid = cp->عدد;

      int64_t idx_val = thr->واژگان[3].w_int;
      إذا (thr->flags[4] == BIT4_1) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with undefined index."
	         << پایان_سطر;
	    أرجع درست;
      }
      إذا (idx_val < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with negative index."
	         << پایان_سطر;
	    أرجع درست;
      }
      size_t idx = idx_val;

      vvp_object_t& بالاترین = thr->peek_object();
      vvp_cobject*cobj = بالاترین.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_صف* صف = qobj.peek<vvp_صف>();
      إذا (صف == 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping delete(" << idx
	         << ") on empty queue." << پایان_سطر;
      } وإلا {
	    size_t size = صف->get_اندازه();
	    إذا (idx >= size) {
		  cerr << thr->get_fileline()
		       << "Warning: skipping out of range delete(" << idx
		       << ") on queue of size " << size << "." << پایان_سطر;
	    } وإلا {
		  صف->erase(idx);
	    }
      }

      أرجع درست;
}

منطقي از_DELETE_PROP_OBJ(vthread_t thr, vvp_کد_t cp)
{
      size_t pid = cp->عدد;

      vvp_object_t& بالاترین = thr->peek_object();
      vvp_cobject*cobj = بالاترین.peek<vvp_cobject>();
      أكد(cobj);

      cobj->مجموعه_object(pid, vvp_object_t(), 0);

      أرجع درست;
}

قالب <نام_نوع ELEM, صنف QTYPE>
ساكن منطقي store_qb(vthread_t thr, vvp_کد_t cp, بیعلامت wid=0)
{
      ELEM value;
      vvp_net_t*net = cp->net;
      بیعلامت max_اندازه = thr->واژگان[cp->بیت_idx[0]].w_int;
      برداشت_مقدار(thr, value, wid); // Pop the value to store.

      vvp_صف*صف = get_صف_object<QTYPE>(thr, net);
      أكد(صف);
      صف->push_back(value, max_اندازه);
      أرجع درست;
}

/*
 * %store/qb/r <var-label>, <max-idx>
 */
منطقي از_STORE_QB_R(vthread_t thr, vvp_کد_t cp)
{
      أرجع store_qb<مزدوج, vvp_صف_real>(thr, cp);
}

/*
 * %store/qb/str <var-label>, <max-idx>
 */
منطقي از_STORE_QB_STR(vthread_t thr, vvp_کد_t cp)
{
      أرجع store_qb<string, vvp_صف_string>(thr, cp);
}

/*
 * %store/qb/v <var-label>, <max-idx>, <wid>
 */
منطقي از_STORE_QB_V(vthread_t thr, vvp_کد_t cp)
{
      أرجع store_qb<vvp_vector4_t, vvp_صف_vec4>(thr, cp, cp->بیت_idx[1]);
}

قالب <نام_نوع ELEM, صنف QTYPE>
ساكن منطقي store_qdar(vthread_t thr, vvp_کد_t cp, بیعلامت wid=0)
{
      int64_t idx = thr->واژگان[3].w_int;
      ELEM value;
      vvp_net_t*net = cp->net;
      برداشت_مقدار(thr, value, wid); // Pop the value to store.

      vvp_صف*صف = get_صف_object<QTYPE>(thr, net);
      أكد(صف);
      إذا (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot assign to a negative "
	         << get_صف_type(value)
	         << " index (" << idx << "). ";
	    print_صف_مقدار(value);
	    cerr << " was not added." << پایان_سطر;
      } وإلا إذا (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot assign to an undefined "
	         << get_صف_type(value) << " index. ";
	    print_صف_مقدار(value);
	    cerr << " was not added." << پایان_سطر;
      } وإلا {
	    بیعلامت max_اندازه = thr->واژگان[cp->بیت_idx[0]].w_int;
	    صف->مجموعه_کلمه_max(idx, value, max_اندازه);
      }
      أرجع درست;
}

/*
 * %store/qdar/r <var>, idx
 */
منطقي از_STORE_QDAR_R(vthread_t thr, vvp_کد_t cp)
{
      أرجع store_qdar<مزدوج, vvp_صف_real>(thr, cp);
}

/*
 * %store/qdar/str <var>, idx
 */
منطقي از_STORE_QDAR_STR(vthread_t thr, vvp_کد_t cp)
{
      أرجع store_qdar<string, vvp_صف_string>(thr, cp);
}

/*
 * %store/qdar/v <var>, idx
 */
منطقي از_STORE_QDAR_V(vthread_t thr, vvp_کد_t cp)
{
      أرجع store_qdar<vvp_vector4_t, vvp_صف_vec4>(thr, cp, cp->بیت_idx[1]);
}

قالب <نام_نوع ELEM, صنف QTYPE>
ساكن منطقي store_qf(vthread_t thr, vvp_کد_t cp, بیعلامت wid=0)
{
      ELEM value;
      vvp_net_t*net = cp->net;
      بیعلامت max_اندازه = thr->واژگان[cp->بیت_idx[0]].w_int;
      برداشت_مقدار(thr, value, wid); // Pop the value to store.

      vvp_صف*صف = get_صف_object<QTYPE>(thr, net);
      أكد(صف);
      صف->push_front(value, max_اندازه);
      أرجع درست;
}
/*
 * %store/qf/r <var-label>, <max-idx>
 */
منطقي از_STORE_QF_R(vthread_t thr, vvp_کد_t cp)
{
      أرجع store_qf<مزدوج, vvp_صف_real>(thr, cp);
}

/*
 * %store/qf/str <var-label>, <max-idx>
 */
منطقي از_STORE_QF_STR(vthread_t thr, vvp_کد_t cp)
{
      أرجع store_qf<string, vvp_صف_string>(thr, cp);
}

/*
 * %store/qb/v <var-label>, <max-idx>, <wid>
 */
منطقي از_STORE_QF_V(vthread_t thr, vvp_کد_t cp)
{
      أرجع store_qf<vvp_vector4_t, vvp_صف_vec4>(thr, cp, cp->بیت_idx[1]);
}

قالب <نام_نوع ELEM, صنف QTYPE>
ساكن منطقي store_qobj(vthread_t thr, vvp_کد_t cp, بیعلامت wid=0)
{
// FIXME: Can we actually use wid here?
      (فراغ)wid;
      vvp_net_t*net = cp->net;

      vvp_صف*صف = get_صف_object<QTYPE>(thr, net);
      أكد(صف);

      vvp_object_t src;
      thr->برداشت_object(src);

        // If it is null just clear the queue
      إذا (src.test_nil())
	    صف->erase_دم(0);
      وإلا {
	    بیعلامت max_اندازه = thr->واژگان[cp->بیت_idx[0]].w_int;
	    صف->copy_elems(src, max_اندازه);
      }

      أرجع درست;
}

منطقي از_STORE_QOBJ_R(vthread_t thr, vvp_کد_t cp)
{
      أرجع store_qobj<مزدوج, vvp_صف_real>(thr, cp);
}

منطقي از_STORE_QOBJ_STR(vthread_t thr, vvp_کد_t cp)
{
      أرجع store_qobj<string, vvp_صف_string>(thr, cp);
}

منطقي از_STORE_QOBJ_V(vthread_t thr, vvp_کد_t cp)
{
      أرجع store_qobj<vvp_vector4_t, vvp_صف_vec4>(thr, cp, cp->بیت_idx[1]);
}

ساكن فراغ vvp_send(vthread_t thr, vvp_net_ptr_t ptr, ثابت مزدوج&val)
{
      vvp_send_real(ptr, val, thr->wt_context);
}

ساكن فراغ vvp_send(vthread_t thr, vvp_net_ptr_t ptr, ثابت string&val)
{
      vvp_send_string(ptr, val, thr->wt_context);
}

قالب <نام_نوع ELEM>
ساكن منطقي store(vthread_t thr, vvp_کد_t cp)
{
      ELEM val;
      برداشت_مقدار(thr, val, 0);
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_send(thr, ptr, val);
      أرجع درست;
}

منطقي از_STORE_REAL(vthread_t thr, vvp_کد_t cp)
{
      أرجع store<مزدوج>(thr, cp);
}

قالب <نام_نوع ELEM>
ساكن منطقي storea(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت idx = cp->بیت_idx[0];
      ELEM val;
      برداشت_مقدار(thr, val, 0);

      إذا (thr->flags[4] != BIT4_1) {
	    بیعلامت adr = thr->واژگان[idx].w_int;
	    cp->array->مجموعه_کلمه(adr, val);
      }

      أرجع درست;
}

/*
 * %store/reala <var-label> <index>
 */
منطقي از_STORE_REALA(vthread_t thr, vvp_کد_t cp)
{
      أرجع storea<مزدوج>(thr, cp);
}

منطقي از_STORE_STR(vthread_t thr, vvp_کد_t cp)
{
      أرجع store<string>(thr, cp);
}

/*
 * %store/stra <array-label> <index>
 */
منطقي از_STORE_STRA(vthread_t thr, vvp_کد_t cp)
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
منطقي از_STORE_VEC4(vthread_t thr, vvp_کد_t cp)
{
      vvp_net_ptr_t ptr(cp->net, 0);
      ثابت vvp_signal_مقدار*sig = dynamic_cast<vvp_signal_مقدار*> (cp->net->fil);
      بیعلامت off_شاخص = cp->بیت_idx[0];
      بیعلامت صحيح wid = cp->بیت_idx[1];

      int64_t off = off_شاخص ? thr->واژگان[off_شاخص].w_int : 0;
      بیعلامت صحيح sig_مقدار_اندازه = sig->مقدار_اندازه();

      vvp_vector4_t&val = thr->peek_vec4();
      بیعلامت val_اندازه = val.size();

      إذا (val_اندازه < wid) {
	    cerr << thr->get_fileline()
	         << "XXXX Internal error: val.size()=" << val_اندازه
		 << ", expecting >= " << wid << پایان_سطر;
      }
      أكد(val_اندازه >= wid);
      إذا (val_اندازه > wid) {
	    val.resize(wid);
      }

	// If there is a problem loading the index register, flags-4
	// will be set to 1, and we know here to skip the actual assignment.
      إذا (off_شاخص!=0 && thr->flags[4] == BIT4_1) {
	    thr->برداشت_vec4(1);
	    أرجع درست;
      }

      إذا (!resize_rval_vec(val, off, sig_مقدار_اندازه)) {
	    thr->برداشت_vec4(1);
	    أرجع درست;
      }

      إذا (off == 0 && val.size() == sig_مقدار_اندازه)
	    vvp_send_vec4(ptr, val, thr->wt_context);
      وإلا
	    vvp_send_vec4_pv(ptr, val, off, sig_مقدار_اندازه, thr->wt_context);

      thr->برداشت_vec4(1);
      أرجع درست;
}

/*
 * %store/vec4a <var-label>, <addr>, <offset>
 */
منطقي از_STORE_VEC4_A(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت adr_شاخص = cp->بیت_idx[0];
      بیعلامت off_شاخص = cp->بیت_idx[1];

      دراز adr = adr_شاخص? thr->واژگان[adr_شاخص].w_int : 0;
      int64_t off = off_شاخص ? thr->واژگان[off_شاخص].w_int : 0;

	// Suppress action if flags-4 is true.
      إذا (thr->flags[4] == BIT4_1) {
	    thr->برداشت_vec4(1);
	    أرجع درست;
      }

      vvp_vector4_t &value = thr->peek_vec4();

      إذا (!resize_rval_vec(value, off, cp->array->get_کلمه_اندازه())) {
	    thr->برداشت_vec4(1);
	    أرجع درست;
      }

      cp->array->مجموعه_کلمه(adr, off, value);

      thr->برداشت_vec4(1);
      أرجع درست;
}

/*
 * %sub
 *   pop r;
 *   pop l;
 *   push l-r;
 */
منطقي از_SUB(vthread_t thr, vvp_کد_t)
{
      vvp_vector4_t r = thr->برداشت_vec4();
      vvp_vector4_t&l = thr->peek_vec4();

      l.sub(r);
      أرجع درست;
}

/*
 * %subi <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
منطقي از_SUBI(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت wid = cp->عدد;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_فوری_rval (cp, r);

      l.sub(r);

      أرجع درست;

}

منطقي از_SUB_WR(vthread_t thr, vvp_کد_t)
{
      مزدوج r = thr->برداشت_real();
      مزدوج l = thr->برداشت_real();
      thr->افزودن_real(l - r);
      أرجع درست;
}

/*
 * %substr <first>, <last>
 * Pop a string, take the substring (SystemVerilog style), and return
 * the result to the stack. This opcode actually works by editing the
 * string in place.
 */
منطقي از_SUBSTR(vthread_t thr, vvp_کد_t cp)
{
      int32_t اولی = thr->واژگان[cp->بیت_idx[0]].w_int;
      int32_t آخرین = thr->واژگان[cp->بیت_idx[1]].w_int;
      string&val = thr->peek_str(0);

      إذا (اولی < 0 || آخرین < اولی || آخرین >= (int32_t)val.size()) {
	    val = string("");
	    أرجع درست;
      }

      val = val.substr(اولی, آخرین-اولی+1);
      أرجع درست;
}

/*
 * %substr/vec4 <index>, <wid>
 */
منطقي از_SUBSTR_VEC4(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت sel_idx = cp->بیت_idx[0];
      بیعلامت wid = cp->بیت_idx[1];

      int32_t sel = thr->واژگان[sel_idx].w_int;
      ثابت string&val = thr->peek_str(0);

      أكد(wid%8 == 0);

      إذا (sel < 0 || sel >= (int32_t)val.size()) {
	    vvp_vector4_t res (wid, BIT4_0);
	    thr->افزودن_vec4(res);
	    أرجع درست;
      }

      vvp_vector4_t res (wid, BIT4_0);

      أكد(wid==8);
      بیعلامت حرف tmp = val[sel];
      لكل (صحيح idx = 0 ; idx < 8 ; idx += 1) {
	    إذا (tmp & (1<<idx))
		  res.مجموعه_بیت(idx, BIT4_1);
      }

      thr->افزودن_vec4(res);
      أرجع درست;
}

منطقي از_FILE_سطر(vthread_t thr, vvp_کد_t cp)
{
      vpiHandle handle = cp->handle;

	/* When it is available, keep the file/line information in the
	   thread for error/warning messages. */
      thr->مجموعه_fileline(vpi_get_str(vpiFile, handle),
                        vpi_get(vpi_سطر_No, handle));

      إذا (show_file_سطر)
	    cerr << thr->get_fileline()
	         << vpi_get_str(_vpiDescription, handle) << پایان_سطر;

      أرجع درست;
}

/*
 * %test_nul <var-label>;
 * Test if the object at the specified variable is nil. If so, write
 * "1" into flags[4], otherwise write "0" into flags[4].
 */
منطقي از_TEST_NUL(vthread_t thr, vvp_کد_t cp)
{
      vvp_net_t*net = cp->net;

      أكد(net);
      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      أكد(obj);

      إذا (obj->get_object().test_nil())
	    thr->flags[4] = BIT4_1;
      وإلا
	    thr->flags[4] = BIT4_0;

      أرجع درست;
}

منطقي از_TEST_NUL_A(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت idx = cp->بیت_idx[0];
      بیعلامت adr = thr->واژگان[idx].w_int;
      vvp_object_t کلمه;

	/* If the address is undefined, return true. */
      إذا (thr->flags[4] == BIT4_1) {
	    أرجع درست;
      }

      cp->array->get_کلمه_obj(adr, کلمه);
      إذا (کلمه.test_nil())
	    thr->flags[4] = BIT4_1;
      وإلا
	    thr->flags[4] = BIT4_0;

      أرجع درست;
}

منطقي از_TEST_NUL_OBJ(vthread_t thr, vvp_کد_t)
{
      إذا (thr->peek_object().test_nil())
	    thr->flags[4] = BIT4_1;
      وإلا
	    thr->flags[4] = BIT4_0;
      أرجع درست;
}

/*
 * %test_nul/prop <pid>, <idx>
 */
منطقي از_TEST_NUL_PROP(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت pid = cp->عدد;
      بیعلامت idx = cp->بیت_idx[0];

      إذا (idx != 0) {
	    أكد(idx < vthread_s::واژگان_شمار);
	    idx = thr->واژگان[idx].w_uint;
      }

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj  = obj.peek<vvp_cobject>();

      vvp_object_t val;
      cobj->get_object(pid, val, idx);

      إذا (val.test_nil())
	    thr->flags[4] = BIT4_1;
      وإلا
	    thr->flags[4] = BIT4_0;

      أرجع درست;
}

/* and() reduction returns the scalar sum (Verilog-style bitwise-AND per word). */
قالب <صنف SRC>
ساكن vvp_vector4_t uparr_and_واژگان_src(SRC* src, بیعلامت wid)
{
      إذا (!src) أرجع vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_1);
      لكل (size_t i = 0; i < src->get_اندازه(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_کلمه(i, vi);
	    res &= vi;
      }
      أرجع res;
}

منطقي از_UPARR_AND_V(vthread_t thr, vvp_کد_t cp)
{
      vvp_net_t* net = cp->net;
      بیعلامت wid = cp->بیت_idx[0];
      vvp_صف_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_صف_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      إذا (qsrc) res = uparr_and_واژگان_src(qsrc, wid);
      وإلا إذا (dsrc) res = uparr_and_واژگان_src(dsrc, wid);
      وإلا {
	    vvp_صف* src_q = get_صف_object<vvp_صف_vec4>(thr, net);
	    vvp_صف_vec4* src = dynamic_cast<vvp_صف_vec4*>(src_q);
	    res = uparr_and_واژگان_src(src, wid);
      }
      thr->افزودن_vec4(res);
      أرجع درست;
}

/* or() reduction returns the scalar sum (Verilog-style bitwise-AND per word). */
قالب <صنف SRC>
ساكن vvp_vector4_t uparr_or_واژگان_src(SRC* src, بیعلامت wid)
{
      إذا (!src) أرجع vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_0);
      لكل (size_t i = 0; i < src->get_اندازه(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_کلمه(i, vi);
	    res |= vi;
      }
      أرجع res;
}

منطقي از_UPARR_OR_V(vthread_t thr, vvp_کد_t cp)
{
      vvp_net_t* net = cp->net;
      بیعلامت wid = cp->بیت_idx[0];
      vvp_صف_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_صف_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      إذا (qsrc) res = uparr_or_واژگان_src(qsrc, wid);
      وإلا إذا (dsrc) res = uparr_or_واژگان_src(dsrc, wid);
      وإلا {
	    vvp_صف* src_q = get_صف_object<vvp_صف_vec4>(thr, net);
	    vvp_صف_vec4* src = dynamic_cast<vvp_صف_vec4*>(src_q);
	    res = uparr_or_واژگان_src(src, wid);
      }
      thr->افزودن_vec4(res);
      أرجع درست;
}

/* real product() reduction returns the scalar sum */
قالب <صنف SRC>
ساكن مزدوج uparr_حاصلضرب_واژگان_src_real(SRC* src)
{
      إذا (!src) أرجع 0.0;
      مزدوج acc = 1.0;
      لكل (size_t i = 0; i < src->get_اندازه(); i += 1) {
	    مزدوج vi;
	    src->get_کلمه(i, vi);
	    acc *= vi;
      }
      أرجع acc;
}

منطقي از_UPARR_حاصلضرب_REAL(vthread_t thr, vvp_کد_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_صف_real* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_صف_or_darray_real_from_net(thr, net, qsrc, dsrc);
      مزدوج جمع;
      إذا (qsrc) جمع = uparr_حاصلضرب_واژگان_src_real(qsrc);
      وإلا إذا (dsrc) جمع = uparr_حاصلضرب_واژگان_src_real(dsrc);
      وإلا {
	    vvp_صف* src_q = get_صف_object<vvp_صف_real>(thr, net);
	    vvp_صف_real* src = dynamic_cast<vvp_صف_real*>(src_q);
	    جمع = uparr_حاصلضرب_واژگان_src_real(src);
      }
      thr->افزودن_real(جمع);
      أرجع درست;
}

/* product() reduction returns the scalar product (Verilog-style multiplication per word). */
قالب <صنف SRC>
ساكن vvp_vector4_t uparr_حاصلضرب_واژگان_src(SRC* src, بیعلامت wid)
{
      إذا (!src) أرجع vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t acc = صف_unique_ulong_مقصد_vec4(1UL, wid);
      لكل (size_t i = 0; i < src->get_اندازه(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_کلمه(i, vi);
	    acc.mul(vi);
      }
      أرجع acc;
}

منطقي از_UPARR_حاصلضرب_V(vthread_t thr, vvp_کد_t cp)
{
      vvp_net_t* net = cp->net;
      بیعلامت wid = cp->بیت_idx[0];
      vvp_صف_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_صف_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t prod;
      إذا (qsrc) prod = uparr_حاصلضرب_واژگان_src(qsrc, wid);
      وإلا إذا (dsrc) prod = uparr_حاصلضرب_واژگان_src(dsrc, wid);
      وإلا {
	    vvp_صف* src_q = get_صف_object<vvp_صف_vec4>(thr, net);
	    vvp_صف_vec4* src = dynamic_cast<vvp_صف_vec4*>(src_q);
	    prod = uparr_حاصلضرب_واژگان_src(src, wid);
      }
      thr->افزودن_vec4(prod);
      أرجع درست;
}

منطقي از_UPARR_حاصلضرب_PROP_V(vthread_t thr, vvp_کد_t cp)
{
      size_t pid = cp->عدد;
      بیعلامت wid = cp->بیت_idx[0];

      vvp_object_t& بالاترین = thr->peek_object();
      vvp_cobject*cobj = بالاترین.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_صف_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_صف_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t prod =
	    qsrc ? uparr_حاصلضرب_واژگان_src(qsrc, wid)
		 : uparr_حاصلضرب_واژگان_src(dsrc, wid);
      thr->افزودن_vec4(prod);
      أرجع درست;
}

منطقي از_UPARR_حاصلضرب_OBJ_V(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت wid = cp->بیت_idx[0];
      vvp_object_t src_obj;
      thr->برداشت_object(src_obj);

      vvp_صف_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_صف_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_vector4_t prod =
	    qsrc ? uparr_حاصلضرب_واژگان_src(qsrc, wid)
		 : uparr_حاصلضرب_واژگان_src(dsrc, wid);
      thr->افزودن_vec4(prod);
      أرجع درست;
}

/* real sum() reduction returns the scalar sum */
قالب <صنف SRC>
ساكن مزدوج uparr_جمع_واژگان_src_real(SRC* src)
{
      مزدوج acc = 0.0;
      إذا (!src) أرجع acc;
      لكل (size_t i = 0; i < src->get_اندازه(); i += 1) {
	    مزدوج vi;
	    src->get_کلمه(i, vi);
	    acc += vi;
      }
      أرجع acc;
}

منطقي از_UPARR_جمع_REAL(vthread_t thr, vvp_کد_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_صف_real* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_صف_or_darray_real_from_net(thr, net, qsrc, dsrc);
      مزدوج جمع;
      إذا (qsrc) جمع = uparr_جمع_واژگان_src_real(qsrc);
      وإلا إذا (dsrc) جمع = uparr_جمع_واژگان_src_real(dsrc);
      وإلا {
	    vvp_صف* src_q = get_صف_object<vvp_صف_real>(thr, net);
	    vvp_صف_real* src = dynamic_cast<vvp_صف_real*>(src_q);
	    جمع = uparr_جمع_واژگان_src_real(src);
      }
      thr->افزودن_real(جمع);
      أرجع درست;
}

/* sum() reduction returns the scalar sum (Verilog-style addition per word). */
قالب <صنف SRC>
ساكن vvp_vector4_t uparr_جمع_واژگان_src(SRC* src, بیعلامت wid)
{
      إذا (!src) أرجع vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t acc(wid, BIT4_0);
      لكل (size_t i = 0; i < src->get_اندازه(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_کلمه(i, vi);
	    acc.جمع_کن(vi);
      }
      أرجع acc;
}

منطقي از_UPARR_جمع_V(vthread_t thr, vvp_کد_t cp)
{
      vvp_net_t* net = cp->net;
      بیعلامت wid = cp->بیت_idx[0];
      vvp_صف_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_صف_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t جمع;
      إذا (qsrc) جمع = uparr_جمع_واژگان_src(qsrc, wid);
      وإلا إذا (dsrc) جمع = uparr_جمع_واژگان_src(dsrc, wid);
      وإلا {
	    vvp_صف* src_q = get_صف_object<vvp_صف_vec4>(thr, net);
	    vvp_صف_vec4* src = dynamic_cast<vvp_صف_vec4*>(src_q);
	    جمع = uparr_جمع_واژگان_src(src, wid);
      }
      thr->افزودن_vec4(جمع);
      أرجع درست;
}

منطقي از_UPARR_جمع_PROP_V(vthread_t thr, vvp_کد_t cp)
{
      size_t pid = cp->عدد;
      بیعلامت wid = cp->بیت_idx[0];

      vvp_object_t& بالاترین = thr->peek_object();
      vvp_cobject*cobj = بالاترین.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_صف_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_صف_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t جمع =
	    qsrc ? uparr_جمع_واژگان_src(qsrc, wid)
		 : uparr_جمع_واژگان_src(dsrc, wid);
      thr->افزودن_vec4(جمع);
      أرجع درست;
}

منطقي از_UPARR_جمع_OBJ_V(vthread_t thr, vvp_کد_t cp)
{
      بیعلامت wid = cp->بیت_idx[0];
      vvp_object_t src_obj;
      thr->برداشت_object(src_obj);

      vvp_صف_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_صف_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_vector4_t جمع =
	    qsrc ? uparr_جمع_واژگان_src(qsrc, wid)
		 : uparr_جمع_واژگان_src(dsrc, wid);
      thr->افزودن_vec4(جمع);
      أرجع درست;
}

/* xor() reduction returns the scalar sum (Verilog-style bitwise-XOR per word). */
قالب <صنف SRC>
ساكن vvp_vector4_t uparr_xor_واژگان_src(SRC* src, بیعلامت wid)
{
      إذا (!src) أرجع vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_0);
      لكل (size_t i = 0; i < src->get_اندازه(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_کلمه(i, vi);
	    res ^= vi;
      }
      أرجع res;
}

منطقي از_UPARR_XOR_V(vthread_t thr, vvp_کد_t cp)
{
      vvp_net_t* net = cp->net;
      بیعلامت wid = cp->بیت_idx[0];
      vvp_صف_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_صف_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      إذا (qsrc) res = uparr_xor_واژگان_src(qsrc, wid);
      وإلا إذا (dsrc) res = uparr_xor_واژگان_src(dsrc, wid);
      وإلا {
	    vvp_صف* src_q = get_صف_object<vvp_صف_vec4>(thr, net);
	    vvp_صف_vec4* src = dynamic_cast<vvp_صف_vec4*>(src_q);
	    res = uparr_xor_واژگان_src(src, wid);
      }
      thr->افزودن_vec4(res);
      أرجع درست;
}

منطقي از_VPI_CALL(vthread_t thr, vvp_کد_t cp)
{
      vpip_execute_vpi_call(thr, cp->handle);

      إذا (زمانبندی_stopped()) {
	    إذا (! زمانبندی_پایانیافته())
		  زمانبندی_vthread(thr, 0, خطأ);

	    أرجع خطأ;
      }

      أرجع زمانبندی_پایانیافته()? خطأ : درست;
}

/* %wait <label>;
 * Implement the wait by locating the vvp_net_T for the event, and
 * adding this thread to the threads list for the event. The some
 * argument is the  reference to the functor to wait for. This must be
 * an event object of some sort.
 */
منطقي از_WAIT(vthread_t thr, vvp_کد_t cp)
{
      أكد(! thr->i_am_درون_function);
      أكد(! thr->انتظار_for_event);
      thr->انتظار_for_event = 1;

	/* Add this thread to the list in the event. */
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (cp->net->fun);
      أكد(ep);
      thr->wait_بعدی = ep->جمع_کن_انتظار_thread(thr);

	/* Return false to suspend this thread. */
      أرجع خطأ;
}

/*
 * Implement the %wait/fork (SystemVerilog) instruction by suspending
 * the current thread until all the detached children have finished.
 */
منطقي از_WAIT_FORK(vthread_t thr, vvp_کد_t)
{
	/* If a %wait/fork is being executed then the parent thread
	 * cannot be waiting in a join or already waiting. */
      أكد(! thr->i_am_درون_function);
      أكد(! thr->i_am_joining);
      أكد(! thr->i_am_انتظار);

	/* There should be no active children when waiting. */
      أكد(thr->children.empty());

	/* If there are no detached children then there is nothing to
	 * wait for. */
      إذا (thr->detached_children.empty()) أرجع درست;

	/* Flag that this process is waiting for the detached children
	 * to finish and suspend it. */
      thr->i_am_انتظار = 1;
      أرجع خطأ;
}

/*
 * %xnor
 */
منطقي از_XNOR(vthread_t thr, vvp_کد_t)
{
      vvp_vector4_t valr = thr->برداشت_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      أكد(vall.size() == valr.size());

      vall ^= valr;
      vall.invert();

      أرجع درست;
}

/*
 * %xor
 */
منطقي از_XOR(vthread_t thr, vvp_کد_t)
{
      vvp_vector4_t valr = thr->برداشت_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      أكد(vall.size() == valr.size());

      vall ^= valr;

      أرجع درست;
}


منطقي از_ZOMBIE(vthread_t thr, vvp_کد_t)
{
      thr->pc = codespace_null();
      إذا ((thr->والد == 0) && (thr->children.empty())) {
	    إذا (thr->delay_delete)
		  زمانبندی_del_thr(thr);
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
ساكن منطقي do_exec_ufunc(vthread_t thr, vvp_کد_t cp, vthread_t فرزند)
{
      __vpiScope*فرزند_scope = cp->ufunc_core_ptr->func_scope();
      أكد(فرزند_scope);

      أكد(فرزند_scope->get_type_کد() == vpiFunction);
      أكد(thr->children.empty());


        /* We can take a number of shortcuts because we know that a
           continuous assignment can only occur in a static scope. */
      أكد(thr->wt_context == 0);
      أكد(thr->rd_context == 0);

        /* If an automatic function, allocate a context for this call. */
      vvp_context_t فرزند_context = 0;
      إذا (فرزند_scope->is_automatic()) {
            فرزند_context = vthread_alloc_context(فرزند_scope);
            thr->wt_context = فرزند_context;
            thr->rd_context = فرزند_context;
      }

      فرزند->wt_context = فرزند_context;
      فرزند->rd_context = فرزند_context;

	/* Copy all the inputs to the ufunc object to the port
	   variables of the function. This copies all the values
	   atomically. */
      cp->ufunc_core_ptr->تخصیص_بده_بیتها_مقصد_ports(فرزند_context);
      فرزند->delay_delete = 1;

      فرزند->والد = thr;
      thr->children.insert(فرزند);
	// This should be the only child
      أكد(thr->children.size()==1);

      فرزند->is_scheduled = 1;
      فرزند->i_am_درون_function = 1;
      vthread_پیاپی(فرزند);
      running_thread = thr;

      إذا (فرزند->i_have_ended) {
	    do_join(thr, فرزند);
            أرجع درست;
      } وإلا {
	    thr->i_am_joining = 1;
	    أرجع خطأ;
      }
}

منطقي از_EXEC_UFUNC_REAL(vthread_t thr, vvp_کد_t cp)
{
      __vpiScope*فرزند_scope = cp->ufunc_core_ptr->func_scope();
      أكد(فرزند_scope);

	/* Create a temporary thread and run it immediately. */
      vthread_t فرزند = vthread_new(cp->cptr, فرزند_scope);
      thr->افزودن_real(0.0);
      فرزند->آرگومانها_real.push_back(0);

      أرجع do_exec_ufunc(thr, cp, فرزند);
}

منطقي از_EXEC_UFUNC_VEC4(vthread_t thr, vvp_کد_t cp)
{
      __vpiScope*فرزند_scope = cp->ufunc_core_ptr->func_scope();
      أكد(فرزند_scope);

      vpiScopeFunction*scope_func = dynamic_cast<vpiScopeFunction*>(فرزند_scope);
      أكد(scope_func);

	/* Create a temporary thread and run it immediately. */
      vthread_t فرزند = vthread_new(cp->cptr, فرزند_scope);
      thr->افزودن_vec4(vvp_vector4_t(scope_func->get_func_عرض(), scope_func->get_func_init_val()));
      فرزند->آرگومانها_vec4.push_back(0);

      أرجع do_exec_ufunc(thr, cp, فرزند);
}

/*
 * This is a phantom opcode used to harvest the result of calling a user
 * defined function. It is used in code generated by the .ufunc statement.
 */
منطقي از_REAP_UFUNC(vthread_t thr, vvp_کد_t cp)
{
      __vpiScope*فرزند_scope = cp->ufunc_core_ptr->func_scope();
      أكد(فرزند_scope);

	/* Copy the output from the result variable to the output
	   ports of the .ufunc device. */
      cp->ufunc_core_ptr->پایان_thread();

        /* If an automatic function, free the context for this call. */
      إذا (فرزند_scope->is_automatic()) {
            vthread_free_context(thr->rd_context, فرزند_scope);
            thr->wt_context = 0;
            thr->rd_context = 0;
      }

      أرجع درست;
}
