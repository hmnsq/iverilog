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
#که_تعریف_شوی CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#د_که_پای
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

کارول نطاق std;

/* This is the size of an unsigned long in bits. This is just a
   convenience macro. */
# define CPU_کلمه_بټونه (8*حجم(بینښه اوږد))
# define سر_بټ (1UL << (CPU_کلمه_بټونه-1))

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
      vvp_کوډ_t pc;
	/* These hold the private thread bits. */
      تعداد { FLAGS_شمېر = 512, ټکي_شمېر = 16 };
      vvp_bit4_t flags[FLAGS_شمېر];

	/* These are the word registers. */
      اتحاد {
	    int64_t  w_int;
	    uint64_t w_uint;
      } ټکي[ټکي_شمېر];

	// These vectors are depths within the parent thread's
	// corresponding stack.  This is how the %ret/* instructions
	// get at parent thread arguments.
      vector<بینښه> دلیلونه_real;
      vector<بینښه> دلیلونه_str;
      vector<بینښه> دلیلونه_vec4;

    خاص:
      vector<vvp_vector4_t>ډېری_vec4;
    عام:
      مضمن vvp_vector4_t راوباسه_vec4(فراغ)
      {
	    أكد(! ډېری_vec4.empty());
	    vvp_vector4_t val = ډېری_vec4.back();
	    ډېری_vec4.pop_back();
	    أرجع val;
      }
      مضمن فراغ ټېل_vec4(ثابت vvp_vector4_t&val)
      {
	    ډېری_vec4.push_back(val);
      }
      مضمن ثابت vvp_vector4_t& peek_vec4(بینښه ژوروالی)
      {
	    بینښه size = ډېری_vec4.size();
	    أكد(ژوروالی < size);
	    بینښه use_شاخص = size-1-ژوروالی;
	    أرجع ډېری_vec4[use_شاخص];
      }
      مضمن vvp_vector4_t& peek_vec4(فراغ)
      {
	    بینښه use_شاخص = ډېری_vec4.size();
	    أكد(use_شاخص >= 1);
	    أرجع ډېری_vec4[use_شاخص-1];
      }
      مضمن فراغ poke_vec4(بینښه ژوروالی, ثابت vvp_vector4_t&val)
      {
	    أكد(ژوروالی < ډېری_vec4.size());
	    بینښه use_شاخص = ډېری_vec4.size()-1-ژوروالی;
	    ډېری_vec4[use_شاخص] = val;
      }
      مضمن فراغ راوباسه_vec4(بینښه cnt)
      {
	    طالما (cnt > 0) {
		  ډېری_vec4.pop_back();
		  cnt -= 1;
	    }
      }


    خاص:
      vector<مزدوج> ډېری_real;
    عام:
      مضمن مزدوج راوباسه_real(فراغ)
      {
	    أكد(! ډېری_real.empty());
	    مزدوج val = ډېری_real.back();
	    ډېری_real.pop_back();
	    أرجع val;
      }
      مضمن فراغ ټېل_real(مزدوج val)
      {
	    ډېری_real.push_back(val);
      }
      مضمن مزدوج peek_real(بینښه ژوروالی)
      {
	    أكد(ژوروالی < ډېری_real.size());
	    بینښه use_شاخص = ډېری_real.size()-1-ژوروالی;
	    أرجع ډېری_real[use_شاخص];
      }
      مضمن فراغ poke_real(بینښه ژوروالی, مزدوج val)
      {
	    أكد(ژوروالی < ډېری_real.size());
	    بینښه use_شاخص = ډېری_real.size()-1-ژوروالی;
	    ډېری_real[use_شاخص] = val;
      }
      مضمن فراغ راوباسه_real(بینښه cnt)
      {
	    طالما (cnt > 0) {
		  ډېری_real.pop_back();
		  cnt -= 1;
	    }
      }

	/* Strings are operated on using a forth-like operator
	   set. Items at the top of the stack (back()) are the objects
	   operated on except for special cases. New objects are
	   pushed onto the top (back()) and pulled from the top
	   (back()) only. */
    خاص:
      vector<string> ډېری_str;
    عام:
      مضمن string راوباسه_str(فراغ)
      {
	    أكد(! ډېری_str.empty());
	    string val = ډېری_str.back();
	    ډېری_str.pop_back();
	    أرجع val;
      }
      مضمن فراغ ټېل_str(ثابت string&val)
      {
	    ډېری_str.push_back(val);
      }
      مضمن string&peek_str(بینښه ژوروالی)
      {
	    أكد(ژوروالی<ډېری_str.size());
	    بینښه use_شاخص = ډېری_str.size()-1-ژوروالی;
	    أرجع ډېری_str[use_شاخص];
      }
      مضمن فراغ poke_str(بینښه ژوروالی, ثابت string&val)
      {
	    أكد(ژوروالی < ډېری_str.size());
	    بینښه use_شاخص = ډېری_str.size()-1-ژوروالی;
	    ډېری_str[use_شاخص] = val;
      }
      مضمن فراغ راوباسه_str(بینښه cnt)
      {
	    طالما (cnt > 0) {
		  ډېری_str.pop_back();
		  cnt -= 1;
	    }
      }

	/* Objects are also operated on in a stack. */
    خاص:
      تعداد { ډېری_OBJ_MAX_اندازه = 32 };
      vvp_object_t ډېری_obj[ډېری_OBJ_MAX_اندازه];
      بینښه ډېری_obj_اندازه;
    عام:
      مضمن vvp_object_t& peek_object(فراغ)
      {
	    أكد(ډېری_obj_اندازه > 0);
	    أرجع ډېری_obj[ډېری_obj_اندازه-1];
      }
      مضمن فراغ راوباسه_object(vvp_object_t&obj)
      {
	    أكد(ډېری_obj_اندازه > 0);
	    ډېری_obj_اندازه -= 1;
	    obj = ډېری_obj[ډېری_obj_اندازه];
	    ډېری_obj[ډېری_obj_اندازه].reset(0);
      }
      مضمن فراغ راوباسه_object(بینښه cnt, بینښه skip =0)
      {
	    أكد((cnt+skip) <= ډېری_obj_اندازه);
	    لكل (size_t idx = ډېری_obj_اندازه-skip-cnt ; idx < ډېری_obj_اندازه-skip ; idx += 1)
		  ډېری_obj[idx].reset(0);
	    ډېری_obj_اندازه -= cnt;
	    لكل (size_t idx = ډېری_obj_اندازه-skip ; idx < ډېری_obj_اندازه ; idx += 1)
		  ډېری_obj[idx] = ډېری_obj[idx+skip];
	    لكل (size_t idx = ډېری_obj_اندازه ; idx < ډېری_obj_اندازه+skip ; idx += 1)
		  ډېری_obj[idx].reset(0);
      }
      مضمن فراغ ټېل_object(ثابت vvp_object_t&obj)
      {
	    أكد(ډېری_obj_اندازه < ډېری_OBJ_MAX_اندازه);
	    ډېری_obj[ډېری_obj_اندازه] = obj;
	    ډېری_obj_اندازه += 1;
      }

	/* My parent sets this when it wants me to wake it up. */
      بینښه i_am_joining      :1;
      بینښه i_am_detached     :1;
      بینښه i_am_انتظار      :1;
      بینښه i_am_دننه_function  :1; // True if running function code
      بینښه i_have_ended      :1;
      بینښه i_was_disabled    :1;
      بینښه انتظار_for_event :1;
      بینښه is_scheduled      :1;
      بینښه delay_delete      :1;
	/* This points to the children of the thread. */
      سېټ<بنية vthread_s*>children;
	/* This points to the detached children of the thread. */
      سېټ<بنية vthread_s*>detached_children;
	/* This points to my parent, if I have one. */
      بنية vthread_s*پلار;
	/* This points to the containing scope. */
      __vpiScope*پلار_scope;
	/* This is used for keeping wait queues. */
      بنية vthread_s*wait_بل;
	/* These are used to access automatically allocated items. */
      vvp_context_t wt_context, rd_context;
	/* These are used to pass non-blocking event control information. */
      vvp_net_t*event;
      uint64_t ecount;
	/* Save the file/line information when available. */
    خاص:
      حرف *filenm_;
      بینښه lineno_;
    عام:
      فراغ سېټ_fileline(حرف *filenm, بینښه lineno);
      string get_fileline();

      مضمن فراغ cleanup()
      {
	    إذا (i_was_disabled) {
		  ډېری_vec4.clear();
		  ډېری_real.clear();
		  ډېری_str.clear();
		  راوباسه_object(ډېری_obj_اندازه);
	    }
	    حرر(filenm_);
	    filenm_ = 0;
	    أكد(ډېری_vec4.empty());
	    أكد(ډېری_real.empty());
	    أكد(ډېری_str.empty());
	    أكد(ډېری_obj_اندازه == 0);
      }
};

مضمن vthread_s::vthread_s()
{
      ډېری_obj_اندازه = 0;
      filenm_ = 0;
      lineno_ = 0;
}

فراغ vthread_s::سېټ_fileline(حرف *filenm, بینښه lineno)
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
      fd << "**** " << label << کرښه_پای;
      fd << "**** ThreadId: " << هذا << ", parent id: " << پلار << کرښه_پای;

      fd << "**** Flags: ";
      لكل (صحيح idx = 0 ; idx < FLAGS_شمېر ; idx += 1)
	    fd << flags[idx];
      fd << کرښه_پای;
      fd << "**** vec4 stack..." << کرښه_پای;
      لكل (size_t idx = ډېری_vec4.size() ; idx > 0 ; idx -= 1)
	    fd << "    " << (ډېری_vec4.size()-idx) << ": " << ډېری_vec4[idx-1] << کرښه_پای;
      fd << "**** str stack (" << ډېری_str.size() << ")..." << کرښه_پای;
      fd << "**** obj stack (" << ډېری_obj_اندازه << ")..." << کرښه_پای;
      fd << "**** args_vec4 array (" << دلیلونه_vec4.size() << ")..." << کرښه_پای;
      لكل (size_t idx = 0 ; idx < دلیلونه_vec4.size() ; idx += 1)
	    fd << "    " << idx << ": " << دلیلونه_vec4[idx] << کرښه_پای;
      fd << "**** file/line (";
      إذا (filenm_) fd << filenm_;
      وإلا fd << "<no file name>";
      fd << ":" << lineno_ << ")" << کرښه_پای;
      fd << "**** Done ****" << کرښه_پای;
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
      vector<حرف> tmp (مزي_اوږدوالی(متن)+1);
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

ساكن فراغ do_join(vthread_t thr, vthread_t ماشوم);

__vpiScope* vthread_scope(بنية vthread_s*thr)
{
      أرجع thr->پلار_scope;
}

بنية vthread_s*running_thread = 0;

string get_fileline()
{
      أرجع running_thread->get_fileline();
}

فراغ vthread_ټېل(بنية vthread_s*thr, مزدوج val)
{
      thr->ټېل_real(val);
}

فراغ vthread_ټېل(بنية vthread_s*thr, ثابت string&val)
{
      thr->ټېل_str(val);
}

فراغ vthread_ټېل(بنية vthread_s*thr, ثابت vvp_vector4_t&val)
{
      thr->ټېل_vec4(val);
}

فراغ vthread_راوباسه_real(بنية vthread_s*thr, بینښه ژوروالی)
{
      thr->راوباسه_real(ژوروالی);
}

فراغ vthread_راوباسه_str(بنية vthread_s*thr, بینښه ژوروالی)
{
      thr->راوباسه_str(ژوروالی);
}

فراغ vthread_راوباسه_vec4(بنية vthread_s*thr, بینښه ژوروالی)
{
      thr->راوباسه_vec4(ژوروالی);
}

مزدوج vthread_get_real_ډېری(بنية vthread_s*thr, بینښه ژوروالی)
{
      أرجع thr->peek_real(ژوروالی);
}

ثابت string&vthread_get_str_ډېری(بنية vthread_s*thr, بینښه ژوروالی)
{
      أرجع thr->peek_str(ژوروالی);
}

ثابت vvp_vector4_t& vthread_get_vec4_ډېری(بنية vthread_s*thr, بینښه ژوروالی)
{
      أرجع thr->peek_vec4(ژوروالی);
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
کينډۍ <صنف VVP_کتار> ساكن vvp_کتار*get_کتار_object(vthread_t thr, vvp_net_t*net)
{
      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      أكد(obj);

      vvp_کتار*کتار = obj->get_object().peek<vvp_کتار>();
      إذا (کتار == 0) {
	    أكد(obj->get_object().test_nil());
	    کتار = جديد VVP_کتار;
	    vvp_object_t val (کتار);
	    vvp_net_ptr_t ptr (net, 0);
	    vvp_send_object(ptr, val, thr->wt_context);
      }

      أرجع کتار;
}

/*
 * Locator and size/word opcodes use the same VVP net for a queue ($) or a
 * dynamic array ([]). Queues are vvp_queue_vec4; dynamic arrays may be
 * vvp_darray_vec4, vvp_darray_atom<>, etc. (all vvp_darray, but vvp_queue
 * also derives from vvp_darray — test queue vec4 first).
 */
ساكن فراغ get_کتار_or_darray_vec4_from_net(vthread_t thr, vvp_net_t* net,
					      vvp_کتار_vec4*& qsrc,
					      vvp_darray*& dsrc)
{
      vvp_fun_signal_object* obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      أكد(obj);
      vvp_object_t سر = obj->get_object();
      qsrc = سر.peek<vvp_کتار_vec4>();
      إذا (qsrc) {
	    dsrc = 0;
	    أرجع;
      }
      vvp_darray* dany = سر.peek<vvp_darray>();
      إذا (dany && dynamic_cast<vvp_کتار*>(dany) == 0) {
	    dsrc = dany;
	    أرجع;
      }
      إذا (سر.test_nil()) {
	    qsrc = جديد vvp_کتار_vec4();
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

ساكن فراغ get_کتار_or_darray_real_from_net(vthread_t thr, vvp_net_t* net,
					      vvp_کتار_real*& qsrc,
					      vvp_darray*& dsrc)
{
      vvp_fun_signal_object* obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      أكد(obj);
      vvp_object_t سر = obj->get_object();
      qsrc = سر.peek<vvp_کتار_real>();
      إذا (qsrc) {
	    dsrc = 0;
	    أرجع;
      }
      vvp_darray* dany = سر.peek<vvp_darray>();
      إذا (dany && dynamic_cast<vvp_کتار*>(dany) == 0) {
	    dsrc = dany;
	    أرجع;
      }
      إذا (سر.test_nil()) {
	    qsrc = جديد vvp_کتار_real();
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

ساكن فراغ get_کتار_or_darray_vec4_from_object(vvp_object_t src_obj,
						 vvp_کتار_vec4*& qsrc,
						 vvp_darray*& dsrc)
{
      qsrc = src_obj.peek<vvp_کتار_vec4>();
      إذا (qsrc) {
	    dsrc = 0;
	    أرجع;
      }
      vvp_darray* dany = src_obj.peek<vvp_darray>();
      إذا (dany && dynamic_cast<vvp_کتار*>(dany) == 0) {
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
مضمن ساكن فراغ راوباسه_ارزښت(vthread_t thr, مزدوج&value, بینښه)
{
      value = thr->راوباسه_real();
}

مضمن ساكن فراغ راوباسه_ارزښت(vthread_t thr, string&value, بینښه)
{
      value = thr->راوباسه_str();
}

مضمن ساكن فراغ راوباسه_ارزښت(vthread_t thr, vvp_vector4_t&value, بینښه wid)
{
      value = thr->راوباسه_vec4();
      أكد(value.size() == wid);
}

/*
 * The following are used to allow the queue templates to print correctly.
 */
مضمن ساكن string get_کتار_type(مزدوج&)
{
      أرجع "queue<real>";
}

مضمن ساكن string get_کتار_type(string&)
{
      أرجع "queue<string>";
}

مضمن ساكن string get_کتار_type(ثابت vvp_vector4_t&value)
{
      ostringstream buf;
      buf << "queue<vector[" << value.size() << "]>";
      string res = buf.str();
      أرجع res;
}

مضمن ساكن فراغ print_کتار_ارزښت(مزدوج value)
{
      cerr << value;
}

مضمن ساكن فراغ print_کتار_ارزښت(ثابت string&value)
{
      cerr << "\"" << value << "\"";
}

مضمن ساكن فراغ print_کتار_ارزښت(ثابت vvp_vector4_t&value)
{
      cerr << value;
}

/*
 * The following are used to get a darray/queue default value.
 */
مضمن ساكن فراغ dq_default(مزدوج&value, بینښه)
{
      value = 0.0;
}

مضمن ساكن فراغ dq_default(string&value, بینښه)
{
      value = "";
}

مضمن ساكن فراغ dq_default(vvp_vector4_t&value, بینښه wid)
{
      value = vvp_vector4_t(wid);
}


کينډۍ <صنف T> T coerce_منزل_پلنوالی(ثابت T&that, بینښه width)
{
      إذا (that.size() == width)
	    أرجع that;

      أكد(that.size() > width);
      T res (width);
      لكل (بینښه idx = 0 ;  idx < width ;  idx += 1)
	    res.سېټ_بټ(idx, that.value(idx));

      أرجع res;
}

/* Explicitly define the vvp_vector4_t version of coerce_to_width(). */
کينډۍ vvp_vector4_t coerce_منزل_پلنوالی(ثابت vvp_vector4_t&that,
                                       بینښه width);


ساكن فراغ multiply_array_imm(بینښه اوږد*res, ثابت بینښه اوږد*val,
			       بینښه ټکي, بینښه اوږد imm)
{
      لكل (بینښه idx = 0 ; idx < ټکي ; idx += 1)
	    res[idx] = 0;

      لكل (بینښه mul_idx = 0 ; mul_idx < ټکي ; mul_idx += 1) {
	    بینښه اوږد مجموعه;
	    بینښه اوږد tmp = multiply_with_لېږد(val[mul_idx], imm, مجموعه);

	    بینښه اوږد لېږد = 0;
	    res[mul_idx] = جمع_کړه_with_لېږد(res[mul_idx], tmp, لېږد);
	    لكل (بینښه جمع_کړه_idx = mul_idx+1 ; جمع_کړه_idx < ټکي ; جمع_کړه_idx += 1) {
		  res[جمع_کړه_idx] = جمع_کړه_with_لېږد(res[جمع_کړه_idx], مجموعه, لېږد);
		  مجموعه = 0;
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
            scope->free_contexts = vvp_get_بل_context(context);
            لكل (بینښه idx = 0 ; idx < scope->nitem ; idx += 1) {
                  scope->توکی[idx]->reset_instance(context);
            }
      } وإلا {
            context = vvp_allocate_context(scope->nitem);
            لكل (بینښه idx = 0 ; idx < scope->nitem ; idx += 1) {
                  scope->توکی[idx]->alloc_instance(context);
            }
      }

      vvp_سېټ_بل_context(context, scope->ژوندی_contexts);
      scope->ژوندی_contexts = context;

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

      إذا (context == scope->ژوندی_contexts) {
            scope->ژوندی_contexts = vvp_get_بل_context(context);
      } وإلا {
            vvp_context_t tmp = scope->ژوندی_contexts;
            طالما (context != vvp_get_بل_context(tmp)) {
                  أكد(tmp);
                  tmp = vvp_get_بل_context(tmp);
            }
            vvp_سېټ_بل_context(tmp, vvp_get_بل_context(context));
      }

      vvp_سېټ_بل_context(context, scope->free_contexts);
      scope->free_contexts = context;
}

#که_تعریف_شوی CHECK_WITH_VALGRIND
فراغ contexts_delete(صنف __vpiScope*scope)
{
      vvp_context_t context = scope->free_contexts;

      طالما (context) {
	    scope->free_contexts = vvp_get_بل_context(context);
	    لكل (بینښه idx = 0; idx < scope->nitem; idx += 1) {
		  scope->توکی[idx]->free_instance(context);
	    }
	    حرر(context);
	    context = scope->free_contexts;
      }
      حرر(scope->توکی);
}
#د_که_پای

/*
 * Create a new thread with the given start address.
 */
vthread_t vthread_new(vvp_کوډ_t pc, __vpiScope*scope)
{
      vthread_t thr = جديد بنية vthread_s;
      thr->pc     = pc;
	//thr->bits4  = vvp_vector4_t(32);
      thr->پلار = 0;
      thr->پلار_scope = scope;
      thr->wait_بل = 0;
      thr->wt_context = 0;
      thr->rd_context = 0;

      thr->i_am_joining  = 0;
      thr->i_am_detached = 0;
      thr->i_am_انتظار  = 0;
      thr->i_am_دننه_function = 0;
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

      scope->تارونه .insert(thr);
      أرجع thr;
}

#که_تعریف_شوی CHECK_WITH_VALGRIND
#إذا 0
/*
 * These are not currently correct. If you use them you will get
 * double delete messages. There is still a leak related to a
 * waiting event that needs to be investigated.
 */

ساكن فراغ wait_بل_delete(vthread_t بنسټ)
{
      طالما (بنسټ) {
	    vthread_t tmp = بنسټ->wait_بل;
	    ړنګول بنسټ;
	    بنسټ = tmp;
	    إذا (بنسټ->انتظار_for_event == 0) اكسر;
      }
}

ساكن فراغ ماشوم_delete(vthread_t بنسټ)
{
      طالما (بنسټ) {
	    vthread_t tmp = بنسټ->ماشوم;
	    ړنګول بنسټ;
	    بنسټ = tmp;
      }
}
#د_که_پای

فراغ vthreads_delete(صنف __vpiScope*scope)
{
      لكل (std::سېټ<vthread_t>::iterator cur = scope->تارونه.begin()
		 ; cur != scope->تارونه.پای_ټکی() ; ++ cur ) {
	    ړنګول *cur;
      }
      scope->تارونه.clear();
}
#د_که_پای

/*
 * Reaping pulls the thread out of the stack of threads. If I have a
 * child, then hand it over to my parent or fully detach it.
 */
ساكن فراغ vthread_reap(vthread_t thr)
{
      إذا (! thr->children.empty()) {
	    لكل (سېټ<vthread_t>::iterator cur = thr->children.begin()
		       ; cur != thr->children.پای_ټکی() ; ++cur) {
		  vthread_t ماشوم = *cur;
		  أكد(ماشوم);
		  أكد(ماشوم->پلار == thr);
		  ماشوم->پلار = thr->پلار;
	    }
      }
      إذا (! thr->detached_children.empty()) {
	    لكل (سېټ<vthread_t>::iterator cur = thr->detached_children.begin()
		       ; cur != thr->detached_children.پای_ټکی() ; ++cur) {
		  vthread_t ماشوم = *cur;
		  أكد(ماشوم);
		  أكد(ماشوم->پلار == thr);
		  أكد(ماشوم->i_am_detached);
		  ماشوم->پلار = 0;
		  ماشوم->i_am_detached = 0;
	    }
      }
      إذا (thr->پلار) {
	      /* assert that the given element was removed. */
	    إذا (thr->i_am_detached) {
		  size_t res = thr->پلار->detached_children.erase(thr);
		  أكد(res == 1);
	    } وإلا {
		  size_t res = thr->پلار->children.erase(thr);
		  أكد(res == 1);
	    }
      }

      thr->پلار = 0;

	// Remove myself from the containing scope if needed.
      thr->پلار_scope->تارونه.erase(thr);

      thr->pc = codespace_null();

	/* If this thread is not scheduled, then is it safe to delete
	   it now. Otherwise, let the schedule event (which will
	   execute the thread at of_ZOMBIE) delete the object. */
      إذا ((thr->is_scheduled == 0) && (thr->انتظار_for_event == 0)) {
	    أكد(thr->children.empty());
	    أكد(thr->wait_بل == 0);
	    إذا (thr->delay_delete)
		  مهالوېش_del_thr(thr);
	    وإلا
		  vthread_delete(thr);
      }
}

فراغ vthread_delete(vthread_t thr)
{
      thr->cleanup();
      ړنګول thr;
}

فراغ vthread_mark_scheduled(vthread_t thr)
{
      طالما (thr != 0) {
	    أكد(thr->is_scheduled == 0);
	    thr->is_scheduled = 1;
	    thr = thr->wait_بل;
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
	    thr->i_am_دننه_function = 1;
	    thr = thr->wait_بل;
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
فراغ vthread_پرلپسې(vthread_t thr)
{
      طالما (thr != 0) {
	    vthread_t tmp = thr->wait_بل;
	    thr->wait_بل = 0;

	    أكد(thr->is_scheduled);
	    thr->is_scheduled = 0;

            running_thread = thr;

	    لكل (;;) {
		  vvp_کوډ_t cp = thr->pc;
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
منطقي د_CHUNK_LINK(vthread_t thr, vvp_کوډ_t code)
{
      أكد(code->cptr);
      thr->pc = code->cptr;
      أرجع ریښتیا;
}

/*
 * This is called by an event functor to wake up all the threads on
 * its list. I in fact created that list in the %wait instruction, and
 * I also am certain that the waiting_for_event flag is set.
 */
فراغ vthread_مهالوېش_list(vthread_t thr)
{
      لكل (vthread_t cur = thr ;  cur ;  cur = cur->wait_بل) {
	    أكد(cur->انتظار_for_event);
	    cur->انتظار_for_event = 0;
      }

      مهالوېش_vthread(thr, 0);
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

vvp_context_توکی_t vthread_get_wt_context_توکی(بینښه context_idx)
{
      أكد(running_thread && running_thread->wt_context);
      أرجع vvp_get_context_توکی(running_thread->wt_context, context_idx);
}

vvp_context_توکی_t vthread_get_rd_context_توکی(بینښه context_idx)
{
      أكد(running_thread && running_thread->rd_context);
      أرجع vvp_get_context_توکی(running_thread->rd_context, context_idx);
}

/*
 * %abs/wr
 */
منطقي د_ABS_WR(vthread_t thr, vvp_کوډ_t)
{
      thr->ټېل_real( fabs(thr->راوباسه_real()) );
      أرجع ریښتیا;
}

منطقي د_ALLOC(vthread_t thr, vvp_کوډ_t cp)
{
        /* Allocate a context. */
      vvp_context_t ماشوم_context = vthread_alloc_context(cp->scope);

        /* Push the allocated context onto the write context stack. */
      vvp_سېټ_stacked_context(ماشوم_context, thr->wt_context);
      thr->wt_context = ماشوم_context;

      أرجع ریښتیا;
}

منطقي د_AND(vthread_t thr, vvp_کوډ_t)
{
      vvp_vector4_t valb = thr->راوباسه_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();
      أكد(vala.size() == valb.size());
      vala &= valb;
      أرجع ریښتیا;
}

/*
 * This function must ALWAYS be called with the val set to the right
 * size, and initialized with BIT4_0 bits. Certain optimizations rely
 * on that.
 */
ساكن فراغ get_سمدستي_rval(vvp_کوډ_t cp, vvp_vector4_t&val)
{
      uint32_t vala = cp->بټ_idx[0];
      uint32_t valb = cp->بټ_idx[1];
      بینښه wid  = cp->شمېره;

      إذا (valb == 0) {
	      // Special case: if the value is zero, we are done
	      // before we start.
	    إذا (vala == 0) أرجع;

	      // Special case: The value has no X/Z bits, so we can
	      // use the setarray method to write the value all at once.
	    بینښه use_wid = 8*حجم(بینښه اوږد);
	    إذا (wid < use_wid)
		  use_wid = wid;
	    بینښه اوږد tmp[1];
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

      لكل (بینښه idx = 0 ; idx < wid && (vala|valb) ; idx += 1) {
	    uint32_t ba = 0;
	      // Convert the vala/valb bits to a ba number that
	      // matches the encoding of the vvp_bit4_t enumeration.
	    ba = (valb & 1) << 1;
	    ba |= vala & 1;

	      // Note that the val is already pre-filled with BIT4_0
	      // bits, os we only need to set non-zero bit values.
	    إذا (ba) val.سېټ_بټ(idx, (vvp_bit4_t)ba);

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
منطقي د_جمع_کړه(vthread_t thr, vvp_کوډ_t)
{
      vvp_vector4_t r = thr->راوباسه_vec4();
	// Rather then pop l, use it directly from the stack. When we
	// assign to 'l', that will edit the top of the stack, which
	// replaces a pop and a pull.
      vvp_vector4_t&l = thr->peek_vec4();

      l.جمع_کړه(r);

      أرجع ریښتیا;
}

/*
 * %addi <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
منطقي د_ADDI(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه wid = cp->شمېره;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_سمدستي_rval (cp, r);

      l.جمع_کړه(r);

      أرجع ریښتیا;
}

/*
 * %add/wr
 */
منطقي د_جمع_کړه_WR(vthread_t thr, vvp_کوډ_t)
{
      مزدوج r = thr->راوباسه_real();
      مزدوج l = thr->راوباسه_real();
      thr->ټېل_real(l + r);
      أرجع ریښتیا;
}

/* %assign/ar <array>, <delay>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory. <delay>
 * is the delay in simulation time. <bit> is the index register
 * containing the real value.
 */
منطقي د_ټاکل_AR(vthread_t thr, vvp_کوډ_t cp)
{
      اوږد adr = thr->ټکي[3].w_int;
      بینښه delay = cp->بټ_idx[0];
      مزدوج value = thr->راوباسه_real();

      إذا (adr >= 0) {
	    مهالوېش_ټاکل_array_کلمه(cp->array, adr, value, delay);
      }

      أرجع ریښتیا;
}

/* %assign/ar/d <array>, <delay_idx>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory.
 * <delay_idx> is the integer register that contains the delay value.
 */
منطقي د_ټاکل_ARD(vthread_t thr, vvp_کوډ_t cp)
{
      اوږد adr = thr->ټکي[3].w_int;
      مزدوج value = thr->راوباسه_real();

      إذا (adr >= 0) {
	    vvp_time64_t delay = thr->ټکي[cp->بټ_idx[0]].w_uint;
	    مهالوېش_ټاکل_array_کلمه(cp->array, adr, value, delay);
      }

      أرجع ریښتیا;
}

/* %assign/ar/e <array>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory. <bit>
 * is the index register containing the real value. The event
 * information is contained in the thread event control registers
 * and is set with %evctl.
 */
منطقي د_ټاکل_ARE(vthread_t thr, vvp_کوډ_t cp)
{
      اوږد adr = thr->ټکي[3].w_int;
      مزدوج value = thr->راوباسه_real();

      إذا (adr >= 0) {
	    إذا (thr->ecount == 0) {
		  مهالوېش_ټاکل_array_کلمه(cp->array, adr, value, 0);
	    } وإلا {
		  مهالوېش_evctl(cp->array, adr, value, thr->event,
		                 thr->ecount);
	    }
      }

      أرجع ریښتیا;
}

/*
 * %assign/vec4 <var>, <delay>
 */
منطقي د_ټاکل_VEC4(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      بینښه delay = cp->بټ_idx[0];
      ثابت vvp_vector4_t&val = thr->peek_vec4();

      مهالوېش_ټاکل_ویکټور(ptr, 0, 0, val, delay);
      thr->راوباسه_vec4(1);
      أرجع ریښتیا;
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
			    بینښه صحيح sig_wid)
{
      بینښه صحيح wid = val.size();

        // Fully in bounds, most likely case
      إذا (off >= 0 && (uint64_t)off + wid <= sig_wid)
	    أرجع ریښتیا;

      بینښه صحيح بنسټ = 0;
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
	    بنسټ = -off;
	    wid += off;
		off = 0;
      }

	// If the value is partly above the target, then only assign
	// the bits that overlap
      إذا ((uint64_t)off + wid > sig_wid)
	    wid = sig_wid - (uint64_t)off;

      val = val.subvalue(بنسټ, wid);

      أرجع ریښتیا;
}

/*
 * %assign/vec4/a/d <arr>, <offx>, <delx>
 */
منطقي د_ټاکل_VEC4_A_D(vthread_t thr, vvp_کوډ_t cp)
{
      صحيح off_idx = cp->بټ_idx[0];
      صحيح del_idx = cp->بټ_idx[1];
      صحيح adr_idx = 3;

      int64_t  off = off_idx ? thr->ټکي[off_idx].w_int : 0;
      vvp_time64_t del = del_idx? thr->ټکي[del_idx].w_uint : 0;
      اوږد     adr = thr->ټکي[adr_idx].w_int;

      vvp_vector4_t val = thr->راوباسه_vec4();

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      إذا (thr->flags[4] == BIT4_1)
	    أرجع ریښتیا;

      إذا (!resize_rval_vec(val, off, cp->array->get_کلمه_اندازه()))
	    أرجع ریښتیا;

      مهالوېش_ټاکل_array_کلمه(cp->array, adr, off, val, del);

      أرجع ریښتیا;
}

/*
 * %assign/vec4/a/e <arr>, <offx>
 */
منطقي د_ټاکل_VEC4_A_E(vthread_t thr, vvp_کوډ_t cp)
{
      صحيح off_idx = cp->بټ_idx[0];
      صحيح adr_idx = 3;

      int64_t  off = off_idx ? thr->ټکي[off_idx].w_int : 0;
      اوږد     adr = thr->ټکي[adr_idx].w_int;

      vvp_vector4_t val = thr->راوباسه_vec4();

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      إذا (thr->flags[4] == BIT4_1)
	    أرجع ریښتیا;

      إذا (!resize_rval_vec(val, off, cp->array->get_کلمه_اندازه()))
	    أرجع ریښتیا;

      إذا (thr->ecount == 0) {
	    مهالوېش_ټاکل_array_کلمه(cp->array, adr, off, val, 0);
      } وإلا {
	    مهالوېش_evctl(cp->array, adr, val, off, thr->event, thr->ecount);
      }

      أرجع ریښتیا;
}

/*
 * %assign/vec4/off/d <var>, <off>, <del>
 */
منطقي د_ټاکل_VEC4_OFF_D(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      بینښه off_شاخص = cp->بټ_idx[0];
      بینښه del_شاخص = cp->بټ_idx[1];
      vvp_vector4_t val = thr->راوباسه_vec4();

      int64_t off = thr->ټکي[off_شاخص].w_int;
      vvp_time64_t del = thr->ټکي[del_شاخص].w_uint;

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      إذا (thr->flags[4] == BIT4_1)
	    أرجع ریښتیا;

      vvp_signal_ارزښت*sig = dynamic_cast<vvp_signal_ارزښت*> (cp->net->fil);
      أكد(sig);

      إذا (!resize_rval_vec(val, off, sig->ارزښت_اندازه()))
	    أرجع ریښتیا;

      مهالوېش_ټاکل_ویکټور(ptr, off, sig->ارزښت_اندازه(), val, del);
      أرجع ریښتیا;
}

/*
 * %assign/vec4/off/e <var>, <off>
 */
منطقي د_ټاکل_VEC4_OFF_E(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      بینښه off_شاخص = cp->بټ_idx[0];
      vvp_vector4_t val = thr->راوباسه_vec4();

      int64_t off = thr->ټکي[off_شاخص].w_int;

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      إذا (thr->flags[4] == BIT4_1)
	    أرجع ریښتیا;

      vvp_signal_ارزښت*sig = dynamic_cast<vvp_signal_ارزښت*> (cp->net->fil);
      أكد(sig);

      إذا (!resize_rval_vec(val, off, sig->ارزښت_اندازه()))
	    أرجع ریښتیا;

      إذا (thr->ecount == 0) {
	    مهالوېش_ټاکل_ویکټور(ptr, off, sig->ارزښت_اندازه(), val, 0);
      } وإلا {
	    مهالوېش_evctl(ptr, val, off, sig->ارزښت_اندازه(), thr->event, thr->ecount);
      }

      أرجع ریښتیا;
}

/*
 * %assign/vec4/d <var-label> <delay>
 */
منطقي د_ټاکل_VEC4_D(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      بینښه del_شاخص = cp->بټ_idx[0];
      vvp_time64_t del = thr->ټکي[del_شاخص].w_int;

      vvp_vector4_t value = thr->راوباسه_vec4();

      vvp_signal_ارزښت*sig = dynamic_cast<vvp_signal_ارزښت*> (cp->net->fil);
      أكد(sig);

      مهالوېش_ټاکل_ویکټور(ptr, 0, sig->ارزښت_اندازه(), value, del);

      أرجع ریښتیا;
}

/*
 * %assign/vec4/e <var-label>
 */
منطقي د_ټاکل_VEC4_E(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_vector4_t value = thr->راوباسه_vec4();

      vvp_signal_ارزښت*sig = dynamic_cast<vvp_signal_ارزښت*> (cp->net->fil);
      أكد(sig);

      إذا (thr->ecount == 0) {
	    مهالوېش_ټاکل_ویکټور(ptr, 0, sig->ارزښت_اندازه(), value, 0);
      } وإلا {
	    مهالوېش_evctl(ptr, value, 0, sig->ارزښت_اندازه(), thr->event, thr->ecount);
      }

      أرجع ریښتیا;
}

/*
 * This is %assign/wr <vpi-label>, <delay>
 *
 * This assigns (after a delay) a value to a real variable. Use the
 * vpi_put_value function to do the assign, with the delay written
 * into the vpiInertialDelay carrying the desired delay.
 */
منطقي د_ټاکل_WR(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه delay = cp->بټ_idx[0];
      مزدوج value = thr->راوباسه_real();
      s_vpi_وخت del;

      del.type = vpi_Sim_وخت;
      vpip_وخت_منزل_timestruct(&del, delay);

      __vpiHandle*tmp = cp->handle;

      t_vpi_ارزښت val;
      val.format = vpiRealVal;
      val.value.real = value;
      vpi_put_ارزښت(tmp, &val, &del, vpiTransportDelay);

      أرجع ریښتیا;
}

منطقي د_ټاکل_WRD(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_time64_t delay = thr->ټکي[cp->بټ_idx[0]].w_uint;
      مزدوج value = thr->راوباسه_real();
      s_vpi_وخت del;

      del.type = vpi_Sim_وخت;
      vpip_وخت_منزل_timestruct(&del, delay);

      __vpiHandle*tmp = cp->handle;

      t_vpi_ارزښت val;
      val.format = vpiRealVal;
      val.value.real = value;
      vpi_put_ارزښت(tmp, &val, &del, vpiTransportDelay);

      أرجع ریښتیا;
}

منطقي د_ټاکل_WRE(vthread_t thr, vvp_کوډ_t cp)
{
      أكد(thr->event != 0);
      مزدوج value = thr->راوباسه_real();
      __vpiHandle*tmp = cp->handle;

	// If the count is zero then just put the value.
      إذا (thr->ecount == 0) {
	    t_vpi_ارزښت val;

	    val.format = vpiRealVal;
	    val.value.real = value;
	    vpi_put_ارزښت(tmp, &val, 0, vpiNoDelay);
      } وإلا {
	    مهالوېش_evctl(tmp, value, thr->event, thr->ecount);
      }

      thr->event = 0;
      thr->ecount = 0;

      أرجع ریښتیا;
}

منطقي د_BLEND(vthread_t thr, vvp_کوډ_t)
{
      vvp_vector4_t vala = thr->راوباسه_vec4();
      vvp_vector4_t valb = thr->راوباسه_vec4();
      أكد(vala.size() == valb.size());

      لكل (بینښه idx = 0 ; idx < vala.size() ; idx += 1) {
	    إذا (vala.value(idx) == valb.value(idx))
		  استمر;

	    vala.سېټ_بټ(idx, BIT4_X);
      }

      thr->ټېل_vec4(vala);
      أرجع ریښتیا;
}

منطقي د_BLEND_STR(vthread_t thr, vvp_کوډ_t)
{
      string f = thr->راوباسه_str();
      string t = thr->راوباسه_str();
      thr->ټېل_str((t == f) ? t : "");
      أرجع ریښتیا;
}

منطقي د_BLEND_WR(vthread_t thr, vvp_کوډ_t)
{
      مزدوج f = thr->راوباسه_real();
      مزدوج t = thr->راوباسه_real();
      thr->ټېل_real((t == f) ? t : 0.0);
      أرجع ریښتیا;
}

منطقي د_BREAKPOINT(vthread_t, vvp_کوډ_t)
{
      أرجع ریښتیا;
}

/*
 * %callf/void <code-label>, <scope-label>
 * Combine the %fork and %join steps for invoking a function.
 */
ساكن منطقي do_callf_void(vthread_t thr, vthread_t ماشوم)
{

      إذا (ماشوم->پلار_scope->is_automatic()) {
	      /* The context allocated for this child is the top entry
		 on the write context stack */
	    ماشوم->wt_context = thr->wt_context;
	    ماشوم->rd_context = thr->wt_context;
      }

        // Mark the function thread as a direct child of the current thread.
      ماشوم->پلار = thr;
      thr->children.insert(ماشوم);
        // This should be the only child
      أكد(thr->children.size()==1);

        // Execute the function. This SHOULD run the function to completion,
        // but there are some exceptional situations where it won't.
      أكد(ماشوم->پلار_scope->get_type_کوډ() == vpiFunction);
      ماشوم->is_scheduled = 1;
      ماشوم->i_am_دننه_function = 1;
      vthread_پرلپسې(ماشوم);
      running_thread = thr;

      إذا (ماشوم->i_have_ended) {
	    do_join(thr, ماشوم);
	    أرجع ریښتیا;
      } وإلا {
	    thr->i_am_joining = 1;
	    أرجع خطأ;
      }
}

منطقي د_CALLF_OBJ(vthread_t thr, vvp_کوډ_t cp)
{
      vthread_t ماشوم = vthread_new(cp->cptr2, cp->scope);
      أرجع do_callf_void(thr, ماشوم);

      // XXXX NOT IMPLEMENTED
}

منطقي د_CALLF_REAL(vthread_t thr, vvp_کوډ_t cp)
{
      vthread_t ماشوم = vthread_new(cp->cptr2, cp->scope);

	// This is the return value. Push a place-holder value. The function
	// will replace this with the actual value using a %ret/real instruction.
      thr->ټېل_real(0.0);
      ماشوم->دلیلونه_real.push_back(0);

      أرجع do_callf_void(thr, ماشوم);
}

منطقي د_CALLF_STR(vthread_t thr, vvp_کوډ_t cp)
{
      vthread_t ماشوم = vthread_new(cp->cptr2, cp->scope);

      thr->ټېل_str("");
      ماشوم->دلیلونه_str.push_back(0);

      أرجع do_callf_void(thr, ماشوم);
}

منطقي د_CALLF_VEC4(vthread_t thr, vvp_کوډ_t cp)
{
      vthread_t ماشوم = vthread_new(cp->cptr2, cp->scope);

      vpiScopeFunction*scope_func = dynamic_cast<vpiScopeFunction*>(cp->scope);
      أكد(scope_func);

	// This is the return value. Push a place-holder value. The function
	// will replace this with the actual value using a %ret/real instruction.
      thr->ټېل_vec4(vvp_vector4_t(scope_func->get_func_پلنوالی(), scope_func->get_func_init_val()));
      ماشوم->دلیلونه_vec4.push_back(0);

      أرجع do_callf_void(thr, ماشوم);
}

منطقي د_CALLF_VOID(vthread_t thr, vvp_کوډ_t cp)
{
      vthread_t ماشوم = vthread_new(cp->cptr2, cp->scope);
      أرجع do_callf_void(thr, ماشوم);
}

/*
 * The %cassign/link instruction connects a source node to a
 * destination node. The destination node must be a signal, as it is
 * marked with the source of the cassign so that it may later be
 * unlinked without specifically knowing the source that this
 * instruction used.
 */
منطقي د_CASSIGN_LINK(vthread_t, vvp_کوډ_t cp)
{
      vvp_net_t*dst = cp->net;
      vvp_net_t*src = cp->net2;

      vvp_fun_signal_بنسټ*sig
	    = dynamic_cast<vvp_fun_signal_بنسټ*>(dst->fun);
      أكد(sig);

	/* Any previous continuous assign should have been removed already. */
      أكد(sig->cassign_link == 0);

      sig->cassign_link = src;

	/* Link the output of the src to the port[1] (the cassign
	   port) of the destination. */
      vvp_net_ptr_t dst_ptr (dst, 1);
      src->link(dst_ptr);

      أرجع ریښتیا;
}

/*
 * If there is an existing continuous assign linked to the destination
 * node, unlink it. This must be done before applying a new continuous
 * assign, otherwise the initial assigned value will be propagated to
 * any other nodes driven by the old continuous assign source.
 */
ساكن فراغ cassign_unlink(vvp_net_t*dst)
{
      vvp_fun_signal_بنسټ*sig
	    = dynamic_cast<vvp_fun_signal_بنسټ*>(dst->fun);
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
منطقي د_CASSIGN_VEC4(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_net_t*net = cp->net;
      vvp_vector4_t value = thr->راوباسه_vec4();

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

	/* Set the value into port 1 of the destination. */
      vvp_net_ptr_t ptr (net, 1);
      vvp_send_vec4(ptr, value, 0);

      أرجع ریښتیا;
}

/*
 * %cassign/vec4/off <var>, <off>
 */
منطقي د_CASSIGN_VEC4_OFF(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_net_t*net = cp->net;
      بینښه بنسټ_idx = cp->بټ_idx[0];
      اوږد بنسټ = thr->ټکي[بنسټ_idx].w_int;
      vvp_vector4_t value = thr->راوباسه_vec4();
      بینښه wid = value.size();

      إذا (thr->flags[4] == BIT4_1)
	    أرجع ریښتیا;

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

      vvp_signal_ارزښت*sig = dynamic_cast<vvp_signal_ارزښت*> (net->fil);
      أكد(sig);

      إذا (بنسټ < 0 && (wid <= (بینښه)-بنسټ))
	    أرجع ریښتیا;

      إذا (بنسټ >= (اوږد)sig->ارزښت_اندازه())
	    أرجع ریښتیا;

      إذا (بنسټ < 0) {
	    wid -= (بینښه) -بنسټ;
	    بنسټ = 0;
	    value.resize(wid);
      }

      إذا (بنسټ+wid > sig->ارزښت_اندازه()) {
	    wid = sig->ارزښت_اندازه() - بنسټ;
	    value.resize(wid);
      }

      vvp_net_ptr_t ptr (net, 1);
      vvp_send_vec4_pv(ptr, value, بنسټ, sig->ارزښت_اندازه(), 0);
      أرجع ریښتیا;
}

منطقي د_CASSIGN_WR(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_net_t*net  = cp->net;
      مزدوج value = thr->راوباسه_real();

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

	/* Set the value into port 1 of the destination. */
      vvp_net_ptr_t ptr (net, 1);
      vvp_send_real(ptr, value, 0);

      أرجع ریښتیا;
}

/*
 * %cast2
 */
منطقي د_CAST2(vthread_t thr, vvp_کوډ_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      بینښه wid = val.size();

      لكل (بینښه idx = 0 ; idx < wid ; idx += 1) {
	    بدل (val.value(idx)) {
		حالة BIT4_0:
		حالة BIT4_1:
		  اكسر;
		افتراضي:
		  val.سېټ_بټ(idx, BIT4_0);
		  اكسر;
	    }
      }

      أرجع ریښتیا;
}

منطقي do_cast_vec_dar(vthread_t thr, vvp_کوډ_t cp, منطقي as_vec4)
{
      بینښه wid = cp->شمېره;

      vvp_object_t obj;
      thr->راوباسه_object(obj);

      vvp_darray*darray = obj.peek<vvp_darray>();
      أكد(darray);

      vvp_vector4_t vec = darray->get_bitstream(as_vec4);
      إذا (vec.size() != wid) {
	    cerr << thr->get_fileline()
	         << "VVP error: size mismatch when casting dynamic array to vector." << کرښه_پای;
            thr->ټېل_vec4(vvp_vector4_t(wid));
            مهالوېش_stop(0);
            أرجع خطأ;
      }
      thr->ټېل_vec4(vec);
      أرجع ریښتیا;
}

/*
 * %cast/vec2/dar <wid>
 */
منطقي د_CAST_VEC2_DAR(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع do_cast_vec_dar(thr, cp, خطأ);
}

/*
 * %cast/vec4/dar <wid>
 */
منطقي د_CAST_VEC4_DAR(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع do_cast_vec_dar(thr, cp, ریښتیا);
}

/*
 * %cast/vec4/str <wid>
 */
منطقي د_CAST_VEC4_STR(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه wid = cp->شمېره;
      string str = thr->راوباسه_str();

      vvp_vector4_t vec(wid, BIT4_0);

      إذا (wid != 8*str.length()) {
	    cerr << thr->get_fileline()
	         << "VVP error: size mismatch when casting string to vector." << کرښه_پای;
            thr->ټېل_vec4(vec);
            مهالوېش_stop(0);
            أرجع خطأ;
      }

      بینښه sdx = 0;
      بینښه vdx = wid;
      طالما (vdx > 0) {
            حرف ch = str[sdx++];
            vdx -= 8;
            لكل (بینښه bdx = 0; bdx < 8; bdx += 1) {
                  إذا (ch & 1)
                        vec.سېټ_بټ(vdx+bdx, BIT4_1);
                  ch >>= 1;
            }
      }

      thr->ټېل_vec4(vec);
      أرجع ریښتیا;
}

ساكن فراغ do_CMPE(vthread_t thr, ثابت vvp_vector4_t&lval, ثابت vvp_vector4_t&rval)
{
      أكد(rval.size() == lval.size());

      إذا (lval.has_xz() || rval.has_xz()) {

	    بینښه wid = lval.size();
	    vvp_bit4_t eq  = BIT4_1;
	    vvp_bit4_t eeq = BIT4_1;

	    لكل (بینښه idx = 0 ; idx < wid ; idx += 1) {
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
منطقي د_CMPE(vthread_t thr, vvp_کوډ_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ثابت vvp_vector4_t&rval = thr->peek_vec4(0);
      ثابت vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPE(thr, lval, rval);

      thr->راوباسه_vec4(2);
      أرجع ریښتیا;
}

منطقي د_CMPNE(vthread_t thr, vvp_کوډ_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ثابت vvp_vector4_t&rval = thr->peek_vec4(0);
      ثابت vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];
      thr->flags[6] =  ~thr->flags[6];

      thr->راوباسه_vec4(2);
      أرجع ریښتیا;
}

/*
 * %cmpi/e <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
منطقي د_CMPIE(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه wid = cp->شمېره;

      ثابت vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_سمدستي_rval (cp, rval);

      do_CMPE(thr, lval, rval);

      thr->راوباسه_vec4(1);
      أرجع ریښتیا;
}

منطقي د_CMPINE(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه wid = cp->شمېره;

      ثابت vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_سمدستي_rval (cp, rval);

      do_CMPE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];
      thr->flags[6] =  ~thr->flags[6];

      thr->راوباسه_vec4(1);
      أرجع ریښتیا;
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
      بینښه wid = lval.size();

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

      لكل (بینښه idx = 1 ;  idx < wid ;  idx += 1) {
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
منطقي د_CMPS(vthread_t thr, vvp_کوډ_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ثابت vvp_vector4_t&rval = thr->peek_vec4(0);
      ثابت vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPS(thr, lval, rval);

      thr->راوباسه_vec4(2);
      أرجع ریښتیا;
}

/*
 * %cmpi/s <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
منطقي د_CMPIS(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه wid = cp->شمېره;

      ثابت vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_سمدستي_rval (cp, rval);

      do_CMPS(thr, lval, rval);

      thr->راوباسه_vec4(1);
      أرجع ریښتیا;
}

منطقي د_CMPSTR(vthread_t thr, vvp_کوډ_t)
{
      string re = thr->راوباسه_str();
      string le = thr->راوباسه_str();

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

      أرجع ریښتیا;
}

ساكن فراغ د_CMPU_the_hard_way(vthread_t thr, بینښه wid,
				 ثابت vvp_vector4_t&lval,
				 ثابت vvp_vector4_t&rval)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_bit4_t eeq = BIT4_1;

      لكل (بینښه idx = 0 ;  idx < wid ;  idx += 1) {
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
		 << ", rval=" << rval << کرښه_پای;
      }
      أكد(rval.size() == lval.size());
      بینښه wid = lval.size();

      بینښه اوږد*larray = lval.subarray(0,wid);
      إذا (larray == 0) أرجع د_CMPU_the_hard_way(thr, wid, lval, rval);

      بینښه اوږد*rarray = rval.subarray(0,wid);
      إذا (rarray == 0) {
	    ړنګول[]larray;
	    أرجع د_CMPU_the_hard_way(thr, wid, lval, rval);
      }

      بینښه ټکي = (wid+CPU_کلمه_بټونه-1) / CPU_کلمه_بټونه;

      لكل (بینښه wdx = 0 ; wdx < ټکي ; wdx += 1) {
	    إذا (larray[wdx] == rarray[wdx])
		  استمر;

	    eq = BIT4_0;
	    إذا (larray[wdx] < rarray[wdx])
		  lt = BIT4_1;
	    وإلا
		  lt = BIT4_0;
      }

      ړنګول[]larray;
      ړنګول[]rarray;

      thr->flags[4] = eq;
      thr->flags[5] = lt;
      thr->flags[6] = eq;
}

منطقي د_CMPU(vthread_t thr, vvp_کوډ_t)
{

      ثابت vvp_vector4_t&rval = thr->peek_vec4(0);
      ثابت vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPU(thr, lval, rval);

      thr->راوباسه_vec4(2);
      أرجع ریښتیا;
}

/*
 * %cmpi/u <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
منطقي د_CMPIU(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه wid = cp->شمېره;

      ثابت vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_سمدستي_rval (cp, rval);

      do_CMPU(thr, lval, rval);

      thr->راوباسه_vec4(1);
      أرجع ریښتیا;
}


/*
 * %cmp/x
 */
منطقي د_CMPX(vthread_t thr, vvp_کوډ_t)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_vector4_t rval = thr->راوباسه_vec4();
      vvp_vector4_t lval = thr->راوباسه_vec4();

      أكد(rval.size() == lval.size());
      بینښه wid = lval.size();

      لكل (بینښه idx = 0 ; idx < wid ; idx += 1) {
	    vvp_bit4_t lv = lval.value(idx);
	    vvp_bit4_t rv = rval.value(idx);
	    إذا ((lv != rv) && !bit4_is_xz(lv) && !bit4_is_xz(rv)) {
		  eq = BIT4_0;
		  اكسر;
	    }
      }

      thr->flags[4] = eq;
      أرجع ریښتیا;
}

ساكن فراغ do_CMPWE(vthread_t thr, ثابت vvp_vector4_t&lval, ثابت vvp_vector4_t&rval)
{
      أكد(rval.size() == lval.size());

      إذا (lval.has_xz() || rval.has_xz()) {

	    بینښه wid = lval.size();
	    vvp_bit4_t eq  = BIT4_1;

	    لكل (بینښه idx = 0 ; idx < wid ; idx += 1) {
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

منطقي د_CMPWE(vthread_t thr, vvp_کوډ_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ثابت vvp_vector4_t&rval = thr->peek_vec4(0);
      ثابت vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPWE(thr, lval, rval);

      thr->راوباسه_vec4(2);
      أرجع ریښتیا;
}

منطقي د_CMPWNE(vthread_t thr, vvp_کوډ_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ثابت vvp_vector4_t&rval = thr->peek_vec4(0);
      ثابت vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPWE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];

      thr->راوباسه_vec4(2);
      أرجع ریښتیا;
}

منطقي د_CMPWR(vthread_t thr, vvp_کوډ_t)
{
      مزدوج r = thr->راوباسه_real();
      مزدوج l = thr->راوباسه_real();

      vvp_bit4_t eq = (l == r)? BIT4_1 : BIT4_0;
      vvp_bit4_t lt = (l <  r)? BIT4_1 : BIT4_0;

      thr->flags[4] = eq;
      thr->flags[5] = lt;

      أرجع ریښتیا;
}

/*
 * %cmp/z
 */
منطقي د_CMPZ(vthread_t thr, vvp_کوډ_t)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_vector4_t rval = thr->راوباسه_vec4();
      vvp_vector4_t lval = thr->راوباسه_vec4();

      أكد(rval.size() == lval.size());
      بینښه wid = lval.size();

      لكل (بینښه idx = 0 ; idx < wid ; idx += 1) {
	    vvp_bit4_t lv = lval.value(idx);
	    vvp_bit4_t rv = rval.value(idx);
	    إذا ((lv != rv) && (rv != BIT4_Z) && (lv != BIT4_Z)) {
		  eq = BIT4_0;
		  اكسر;
	    }
      }

      thr->flags[4] = eq;
      أرجع ریښتیا;
}

/*
 *  %concat/str;
 */
منطقي د_CONCAT_STR(vthread_t thr, vvp_کوډ_t)
{
      string متن = thr->راوباسه_str();
      thr->peek_str(0).append(متن);
      أرجع ریښتیا;
}

/*
 *  %concati/str <string>;
 */
منطقي د_CONCATI_STR(vthread_t thr, vvp_کوډ_t cp)
{
      ثابت حرف*متن = cp->متن;
      thr->peek_str(0).append(filter_string(متن));
      أرجع ریښتیا;
}

/*
 * %concat/vec4
 */
منطقي د_CONCAT_VEC4(vthread_t thr, vvp_کوډ_t)
{
      ثابت vvp_vector4_t&lsb = thr->peek_vec4(0);
      ثابت vvp_vector4_t&msb = thr->peek_vec4(1);

	// The result is the size of the top two vectors in the stack.
      vvp_vector4_t res (msb.size() + lsb.size(), BIT4_X);

	// Build up the result.
      res.سېټ_vec(0, lsb);
      res.سېټ_vec(lsb.size(), msb);

	// Rearrange the stack to pop the inputs and push the
	// result. Do that by actually popping only 1 stack position
	// and replacing the new top with the new value.
      thr->راوباسه_vec4(1);
      thr->peek_vec4() = res;

      أرجع ریښتیا;
}

/*
 * %concati/vec4 <vala>, <valb>, <wid>
 *
 * Concat the immediate value to the LOW bits of the concatenation.
 * Get the HIGH bits from the top of the vec4 stack.
 */
منطقي د_CONCATI_VEC4(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه wid  = cp->شمېره;

      vvp_vector4_t&msb = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t lsb (wid, BIT4_0);
      get_سمدستي_rval (cp, lsb);

      vvp_vector4_t res (msb.size()+lsb.size(), BIT4_X);
      res.سېټ_vec(0, lsb);
      res.سېټ_vec(lsb.size(), msb);

      msb = res;
      أرجع ریښتیا;
}

/*
 * %cvt/rv
 */
منطقي د_CVT_RV(vthread_t thr, vvp_کوډ_t)
{
      مزدوج val;
      vvp_vector4_t val4 = thr->راوباسه_vec4();
      vector4_منزل_ارزښت(val4, val, خطأ);
      thr->ټېل_real(val);
      أرجع ریښتیا;
}

/*
 * %cvt/rv/s
 */
منطقي د_CVT_RV_S(vthread_t thr, vvp_کوډ_t)
{
      مزدوج val;
      vvp_vector4_t val4 = thr->راوباسه_vec4();
      vector4_منزل_ارزښت(val4, val, ریښتیا);
      thr->ټېل_real(val);
      أرجع ریښتیا;
}

/*
 * %cvt/sr <idx>
 * Pop the top value from the real stack, convert it to a 64bit signed
 * and save it to the indexed register.
 */
منطقي د_CVT_SR(vthread_t thr, vvp_کوډ_t cp)
{
      مزدوج r = thr->راوباسه_real();
      thr->ټکي[cp->بټ_idx[0]].w_int = i64round(r);

      أرجع ریښتیا;
}

/*
 * %cvt/ur <idx>
 */
منطقي د_CVT_UR(vthread_t thr, vvp_کوډ_t cp)
{
      مزدوج r = thr->راوباسه_real();
      thr->ټکي[cp->بټ_idx[0]].w_uint = vlg_پړاو_منزل_u64(r);

      أرجع ریښتیا;
}

/*
 * %cvt/vr <wid>
 */
منطقي د_CVT_VR(vthread_t thr, vvp_کوډ_t cp)
{
      مزدوج r = thr->راوباسه_real();
      بینښه wid = cp->شمېره;

      vvp_vector4_t tmp(wid, r);
      thr->ټېل_vec4(tmp);
      أرجع ریښتیا;
}

/*
 * This implements the %deassign instruction. All we do is write a
 * long(1) to port-3 of the addressed net. This turns off an active
 * continuous assign activated by %cassign/v
 */
منطقي د_DEASSIGN(vthread_t, vvp_کوډ_t cp)
{
      vvp_net_t*net = cp->net;
      بینښه بنسټ  = cp->بټ_idx[0];
      بینښه width = cp->بټ_idx[1];

      vvp_signal_ارزښت*fil = dynamic_cast<vvp_signal_ارزښت*> (net->fil);
      أكد(fil);
      vvp_fun_signal_vec*sig = dynamic_cast<vvp_fun_signal_vec*>(net->fun);
      أكد(sig);

      إذا (بنسټ >= fil->ارزښت_اندازه()) أرجع ریښتیا;
      إذا (بنسټ+width > fil->ارزښت_اندازه()) width = fil->ارزښت_اندازه() - بنسټ;

      منطقي ډک_sig = بنسټ == 0 && width == fil->ارزښت_اندازه();

	// This is the net that is forcing me...
      إذا (vvp_net_t*src = sig->cassign_link) {
	    إذا (! ډک_sig) {
		  fprintf(stderr, "Sorry: when a signal is assigning a "
		          "register, I cannot deassign part of it.\n");
		  وتل(1);
	    }
	      // And this is the pointer to be removed.
	    vvp_net_ptr_t dst_ptr (net, 1);
	    src->unlink(dst_ptr);
	    sig->cassign_link = 0;
      }

	/* Do we release all or part of the net? */
      إذا (ډک_sig) {
	    sig->deassign();
      } وإلا {
	    sig->deassign_pv(بنسټ, width);
      }

      أرجع ریښتیا;
}

منطقي د_DEASSIGN_WR(vthread_t, vvp_کوډ_t cp)
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

      أرجع ریښتیا;
}

/*
 * %debug/thr
 */
منطقي د_DEBUG_THR(vthread_t thr, vvp_کوډ_t cp)
{
      ثابت حرف*متن = cp->متن;
      thr->debug_dump(cerr, متن);
      أرجع ریښتیا;
}

/*
 * The delay takes two 32bit numbers to make up a 64bit time.
 *
 *   %delay <low>, <hig>
 */
منطقي د_DELAY(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_time64_t ټیټ = cp->بټ_idx[0];
      vvp_time64_t hig = cp->بټ_idx[1];

      vvp_time64_t delay = (hig << 32) | ټیټ;

      إذا (delay == 0) مهالوېش_inactive(thr);
      وإلا مهالوېش_vthread(thr, delay);
      أرجع خطأ;
}

منطقي د_DELAYX(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_time64_t delay;

      أكد(cp->شمېره < vthread_s::ټکي_شمېر);
      delay = thr->ټکي[cp->شمېره].w_uint;
      إذا (delay == 0) مهالوېش_inactive(thr);
      وإلا مهالوېش_vthread(thr, delay);
      أرجع خطأ;
}

منطقي د_DELETE_ELEM(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_net_t*net = cp->net;

      int64_t idx_val = thr->ټکي[3].w_int;
      إذا (thr->flags[4] == BIT4_1) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with undefined index."
	         << کرښه_پای;
	    أرجع ریښتیا;
      }
      إذا (idx_val < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with negative index."
	         << کرښه_پای;
	    أرجع ریښتیا;
      }
      size_t idx = idx_val;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      أكد(obj);

      vvp_کتار*کتار = obj->get_object().peek<vvp_کتار>();
      إذا (کتار == 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping delete(" << idx
	         << ") on empty queue." << کرښه_پای;
      } وإلا {
	    size_t size = کتار->get_اندازه();
	    إذا (idx >= size) {
		  cerr << thr->get_fileline()
		       << "Warning: skipping out of range delete(" << idx
		       << ") on queue of size " << size << "." << کرښه_پای;
	    } وإلا {
		  کتار->erase(idx);
	    }
      }

      أرجع ریښتیا;
}

/* %delete/obj <label>
 *
 * This operator works by assigning a nil to the target object. This
 * causes any value that might be there to be garbage collected, thus
 * deleting the object.
 */
منطقي د_DELETE_OBJ(vthread_t thr, vvp_کوډ_t cp)
{
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_send_object(ptr, vvp_object_t(), thr->wt_context);

      أرجع ریښتیا;
}

/* %reverse/obj <label>
 * Reverse element order of the queue or dynamic array object at this net.
 */
منطقي د_سرچپه_OBJ(vthread_t /*thr*/, vvp_کوډ_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      أكد(obj);

      vvp_object_t stor = obj->get_object();
      إذا (stor.test_nil()) أرجع ریښتیا;

	/* Match other queue ops: peek queues before vvp_darray so concrete
	   queue types dispatch reverse_elems() correctly. */
      إذا (vvp_کتار* کتار = stor.peek<vvp_کتار>()) {
	    کتار->سرچپه_elems();
	    أرجع ریښتیا;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      أكد(darray);
      darray->سرچپه_elems();

      أرجع ریښتیا;
}

/* %reverse/prop/obj <property-index>
 * Reverse queue/darray property storage inside the class object on stack top.
 */
منطقي د_سرچپه_PROP_OBJ(vthread_t thr, vvp_کوډ_t cp)
{
      size_t pid = cp->شمېره;

      vvp_object_t& سر = thr->peek_object();
      vvp_cobject*cobj = سر.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      إذا (arr_obj.test_nil()) أرجع ریښتیا;

      إذا (vvp_کتار* کتار = arr_obj.peek<vvp_کتار>()) {
	    کتار->سرچپه_elems();
	    أرجع ریښتیا;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      أكد(darray);
      darray->سرچپه_elems();

      أرجع ریښتیا;
}

/* %sort/obj — ascending sort */
منطقي د_ترتیبول_OBJ(vthread_t /*thr*/, vvp_کوډ_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      أكد(obj);

      vvp_object_t stor = obj->get_object();
      إذا (stor.test_nil()) أرجع ریښتیا;

      إذا (vvp_کتار* کتار = stor.peek<vvp_کتار>()) {
	    کتار->ترتیبول_elems(ریښتیا);
	    أرجع ریښتیا;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      أكد(darray);
      darray->ترتیبول_elems(ریښتیا);

      أرجع ریښتیا;
}

/* %sort/prop/obj */
منطقي د_ترتیبول_PROP_OBJ(vthread_t thr, vvp_کوډ_t cp)
{
      size_t pid = cp->شمېره;

      vvp_object_t& سر = thr->peek_object();
      vvp_cobject*cobj = سر.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      إذا (arr_obj.test_nil()) أرجع ریښتیا;

      إذا (vvp_کتار* کتار = arr_obj.peek<vvp_کتار>()) {
	    کتار->ترتیبول_elems(ریښتیا);
	    أرجع ریښتیا;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      أكد(darray);
      darray->ترتیبول_elems(ریښتیا);

      أرجع ریښتیا;
}

/* %rsort/obj — descending sort */
منطقي د_RSORT_OBJ(vthread_t /*thr*/, vvp_کوډ_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      أكد(obj);

      vvp_object_t stor = obj->get_object();
      إذا (stor.test_nil()) أرجع ریښتیا;

      إذا (vvp_کتار* کتار = stor.peek<vvp_کتار>()) {
	    کتار->ترتیبول_elems(خطأ);
	    أرجع ریښتیا;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      أكد(darray);
      darray->ترتیبول_elems(خطأ);

      أرجع ریښتیا;
}

/* %rsort/prop/obj */
منطقي د_RSORT_PROP_OBJ(vthread_t thr, vvp_کوډ_t cp)
{
      size_t pid = cp->شمېره;

      vvp_object_t& سر = thr->peek_object();
      vvp_cobject*cobj = سر.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      إذا (arr_obj.test_nil()) أرجع ریښتیا;

      إذا (vvp_کتار* کتار = arr_obj.peek<vvp_کتار>()) {
	    کتار->ترتیبول_elems(خطأ);
	    أرجع ریښتیا;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      أكد(darray);
      darray->ترتیبول_elems(خطأ);

      أرجع ریښتیا;
}

/* %shuffle/obj */
منطقي د_SHUFFLE_OBJ(vthread_t /*thr*/, vvp_کوډ_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      أكد(obj);

      vvp_object_t stor = obj->get_object();
      إذا (stor.test_nil()) أرجع ریښتیا;

      إذا (vvp_کتار* کتار = stor.peek<vvp_کتار>()) {
	    کتار->shuffle_elems();
	    أرجع ریښتیا;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      أكد(darray);
      darray->shuffle_elems();

      أرجع ریښتیا;
}

/* %shuffle/prop/obj */
منطقي د_SHUFFLE_PROP_OBJ(vthread_t thr, vvp_کوډ_t cp)
{
      size_t pid = cp->شمېره;

      vvp_object_t& سر = thr->peek_object();
      vvp_cobject*cobj = سر.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      إذا (arr_obj.test_nil()) أرجع ریښتیا;

      إذا (vvp_کتار* کتار = arr_obj.peek<vvp_کتار>()) {
	    کتار->shuffle_elems();
	    أرجع ریښتیا;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      أكد(darray);
      darray->shuffle_elems();

      أرجع ریښتیا;
}

/* %delete/tail <label>, idx
 *
 * Remove all elements after the one specified.
 */
منطقي د_DELETE_لکۍ(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      أكد(obj);

      vvp_کتار*کتار = obj->get_object().peek<vvp_کتار>();
      أكد(کتار);

      بینښه idx = thr->ټکي[cp->بټ_idx[0]].w_int;
      کتار->erase_لکۍ(idx);

      أرجع ریښتیا;
}

ساكن منطقي do_disable(vthread_t thr, vthread_t سمون)
{
      منطقي flag = خطأ;

	/* Pull the target thread out of its scope if needed. */
      thr->پلار_scope->تارونه.erase(thr);

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
	    أكد(tmp->پلار == thr);
	    thr->i_am_joining = 0;
	    إذا (do_disable(tmp, سمون))
		  flag = ریښتیا;

	    vthread_reap(tmp);
      }

      vthread_t پلار = thr->پلار;
      إذا (پلار && پلار->i_am_joining) {
	      // If a parent is waiting in a %join, wake it up. Note
	      // that it is possible to be waiting in a %join yet
	      // already scheduled if multiple child threads are
	      // ending. So check if the thread is already scheduled
	      // before scheduling it again.
	    پلار->i_am_joining = 0;
	    إذا (! پلار->i_have_ended)
		  مهالوېش_vthread(پلار, 0, ریښتیا);

	    do_join(پلار, thr);

      } وإلا إذا (پلار) {
	      /* If the parent is yet to %join me, let its %join
		 do the reaping. */
	      //assert(tmp->is_scheduled == 0);

      } وإلا {
	      /* No parent at all. Goodbye. */
	    vthread_reap(thr);
      }

      أرجع flag || (thr == سمون);
}

/*
 * Implement the %disable instruction by scanning the target scope for
 * all the target threads. Kill the target threads and wake up a
 * parent that is attempting a %join.
 */
منطقي د_DISABLE(vthread_t thr, vvp_کوډ_t cp)
{
      __vpiScope*scope = static_cast<__vpiScope*>(cp->handle);

      منطقي disabled_myself_flag = خطأ;

      طالما (! scope->تارونه.empty()) {
	    سېټ<vthread_t>::iterator cur = scope->تارونه.begin();

	    إذا (do_disable(*cur, thr))
		  disabled_myself_flag = ریښتیا;
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

منطقي د_DISABLE_FLOW(vthread_t thr, vvp_کوډ_t cp)
{
      ثابت __vpiScope*scope = static_cast<__vpiScope*>(cp->handle);
      vthread_t cur = thr;

      طالما (cur && cur->پلار_scope != scope)
	    cur = cur->پلار;

      أكد(cur);
      إذا (cur)
	    أرجع !do_disable(cur, thr);

      أرجع خطأ;
}

/*
 * Implement the %disable/fork (SystemVerilog) instruction by disabling
 * all the detached children of the given thread.
 */
منطقي د_DISABLE_FORK(vthread_t thr, vvp_کوډ_t)
{
	/* If a %disable/fork is being executed then the parent thread
	 * cannot be waiting in a join. */
      أكد(! thr->i_am_joining);

	/* There should be no active children to disable. */
      أكد(thr->children.empty());

	/* Disable any detached children. */
      طالما (! thr->detached_children.empty()) {
	    vthread_t ماشوم = *(thr->detached_children.begin());
	    أكد(ماشوم);
	    أكد(ماشوم->پلار == thr);
	      /* Disabling the children can never match the parent thread. */
	    منطقي res = do_disable(ماشوم, thr);
	    أكد(! res);
	    vthread_reap(ماشوم);
      }

      أرجع ریښتیا;
}

/*
 * This function divides a 2-word number {high, a} by a 1-word
 * number. Assume that high < b.
 */
ساكن بینښه اوږد divide2words(بینښه اوږد a, بینښه اوږد b,
				  بینښه اوږد لوړ)
{
      بینښه اوږد پایله = 0;
      طالما (لوړ > 0) {
	    بینښه اوږد tmp_پایله = ULONG_MAX / b;
	    بینښه اوږد remain = ULONG_MAX % b;

	    remain += 1;
	    إذا (remain >= b) {
		  remain -= b;
		  tmp_پایله += 1;
	    }

	      // Now 0x1_0...0 = b*tmp_result + remain
	      // high*0x1_0...0 = high*(b*tmp_result + remain)
	      // high*0x1_0...0 = high*b*tmp_result + high*remain

	      // We know that high*0x1_0...0 >= high*b*tmp_result, and
	      // we know that high*0x1_0...0 > high*remain. Use
	      // high*remain as the remainder for another iteration,
	      // and add tmp_result*high into the current estimate of
	      // the result.
	    پایله += tmp_پایله * لوړ;

	      // The new iteration starts with high*remain + a.
	    remain = multiply_with_لېږد(لوړ, remain, لوړ);
	    a += remain;
            إذا(a < remain)
              لوړ += 1;

	      // Now result*b + {high,a} == the input {high,a}. It is
	      // possible that the new high >= 1. If so, it will
	      // certainly be less than high from the previous
	      // iteration. Do another iteration and it will shrink,
	      // eventually to 0.
      }

	// high is now 0, so a is the remaining remainder, so we can
	// finish off the integer divide with a simple a/b.

      أرجع پایله + a/b;
}

ساكن بینښه اوږد* divide_بټونه(بینښه اوږد*ap, ثابت بینښه اوږد*bp, بینښه wid)
{
	// Do all our work a cpu-word at a time. The "words" variable
	// is the number of words of the wid.
      بینښه ټکي = (wid+CPU_کلمه_بټونه-1) / CPU_کلمه_بټونه;

      بینښه btop = ټکي-1;
      طالما (btop > 0 && bp[btop] == 0)
	    btop -= 1;

	// Detect divide by 0, and exit.
      إذا (btop==0 && bp[0]==0)
	    أرجع 0;

	// The result array will eventually accumulate the result. The
	// diff array is a difference that we use in the intermediate.
      بینښه اوږد*diff  = جديد بینښه اوږد[ټکي];
      بینښه اوږد*پایله= جديد بینښه اوږد[ټکي];
      لكل (بینښه idx = 0 ; idx < ټکي ; idx += 1)
	    پایله[idx] = 0;

      لكل (بینښه cur = ټکي-btop ; cur > 0 ; cur -= 1) {
	    بینښه cur_ptr = cur-1;
	    بینښه اوږد cur_res;
	    إذا (ap[cur_ptr+btop] >= bp[btop]) {
		  بینښه اوږد لوړ = 0;
		  إذا (cur_ptr+btop+1 < ټکي)
			لوړ = ap[cur_ptr+btop+1];
		  cur_res = divide2words(ap[cur_ptr+btop], bp[btop], لوړ);

	    } وإلا إذا (cur_ptr+btop+1 >= ټکي) {
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
	    multiply_array_imm(diff+cur_ptr, bp, ټکي-cur_ptr, cur_res);
	      // ap -= diff
	    بینښه اوږد لېږد = 1;
	    لكل (بینښه idx = cur_ptr ; idx < ټکي ; idx += 1)
		  ap[idx] = جمع_کړه_with_لېږد(ap[idx], ~diff[idx], لېږد);

	      // ap has the diff subtracted out of it. If cur_res was
	      // too large, then ap will turn negative. (We easily
	      // tell that ap turned negative by looking at
	      // carry&1. If it is 0, then it is *negative*.) In that
	      // case, we know that cur_res was too large by 1. Correct by
	      // adding 1b back in and reducing cur_res.
	    إذا ((لېږد&1) == 0) {
		    // Keep adding b back in until the remainder
		    // becomes positive again.
		  افعل {
			cur_res -= 1;
			لېږد = 0;
			لكل (بینښه idx = cur_ptr ; idx < ټکي ; idx += 1)
			      ap[idx] = جمع_کړه_with_لېږد(ap[idx], bp[idx-cur_ptr], لېږد);
		  } طالما (لېږد == 0);
	    }

	    پایله[cur_ptr] = cur_res;
      }

	// Now ap contains the remainder and result contains the
	// desired result. We should find that:
	//  input-a = bp * result + ap;

      ړنګول[]diff;
      أرجع پایله;
}

/*
 * %div
 */
منطقي د_DIV(vthread_t thr, vvp_کوډ_t)
{
      vvp_vector4_t valb = thr->راوباسه_vec4();
      vvp_vector4_t vala = thr->راوباسه_vec4();

      أكد(vala.size()== valb.size());
      بینښه wid = vala.size();

      بینښه اوږد*ap = vala.subarray(0, wid);
      إذا (ap == 0) {
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->ټېل_vec4(tmp);
	    أرجع ریښتیا;
      }

      بینښه اوږد*bp = valb.subarray(0, wid);
      إذا (bp == 0) {
	    ړنګول[]ap;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->ټېل_vec4(tmp);
	    أرجع ریښتیا;
      }

	// If the value fits in a single CPU word, then do it the easy way.
      إذا (wid <= CPU_کلمه_بټونه) {
	    إذا (bp[0] == 0) {
		  vvp_vector4_t tmp(wid, BIT4_X);
		  thr->ټېل_vec4(tmp);
	    } وإلا {
		  ap[0] /= bp[0];
		  vala.setarray(0, wid, ap);
		  thr->ټېل_vec4(vala);
	    }
	    ړنګول[]ap;
	    ړنګول[]bp;
	    أرجع ریښتیا;
      }

      بینښه اوږد*پایله = divide_بټونه(ap, bp, wid);
      إذا (پایله == 0) {
	    ړنګول[]ap;
	    ړنګول[]bp;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->ټېل_vec4(tmp);
	    أرجع ریښتیا;
      }

	// Now ap contains the remainder and result contains the
	// desired result. We should find that:
	//  input-a = bp * result + ap;

      vala.setarray(0, wid, پایله);
      thr->ټېل_vec4(vala);
      ړنګول[]ap;
      ړنګول[]bp;
      ړنګول[]پایله;

      أرجع ریښتیا;
}


ساكن فراغ negate_ټکي(بینښه اوږد*val, بینښه ټکي)
{
      بینښه اوږد لېږد = 1;
      لكل (بینښه idx = 0 ; idx < ټکي ; idx += 1)
	    val[idx] = جمع_کړه_with_لېږد(0, ~val[idx], لېږد);
}

/*
 * %div/s
 */
منطقي د_DIV_S(vthread_t thr, vvp_کوډ_t)
{
      vvp_vector4_t valb = thr->راوباسه_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      أكد(vala.size()== valb.size());
      بینښه wid = vala.size();
      بینښه ټکي = (wid + CPU_کلمه_بټونه - 1) / CPU_کلمه_بټونه;

	// Get the values, left in right, in binary form. If there is
	// a problem with either (caused by an X or Z bit) then we
	// know right away that the entire result is X.
      بینښه اوږد*ap = vala.subarray(0, wid);
      إذا (ap == 0) {
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    أرجع ریښتیا;
      }

      بینښه اوږد*bp = valb.subarray(0, wid);
      إذا (bp == 0) {
	    ړنګول[]ap;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    أرجع ریښتیا;
      }

	// Sign extend the bits in the array to fill out the array.
      بینښه اوږد sign_ماسک = 0;
      إذا (بینښه اوږد sign_بټونه = (ټکي*CPU_کلمه_بټونه) - wid) {
	    sign_ماسک = -1UL << (CPU_کلمه_بټونه-sign_بټونه);
	    إذا (ap[ټکي-1] & (sign_ماسک>>1))
		  ap[ټکي-1] |= sign_ماسک;
	    إذا (bp[ټکي-1] & (sign_ماسک>>1))
		  bp[ټکي-1] |= sign_ماسک;
      }

	// If the value fits in a single word, then use the native divide.
      إذا (wid <= CPU_کلمه_بټونه) {
	    إذا (bp[0] == 0) {
		  vvp_vector4_t tmp(wid, BIT4_X);
		  vala = tmp;
	    } وإلا إذا (((اوږد)ap[0] == LONG_MIN) && ((اوږد)bp[0] == -1)) {
		  vvp_vector4_t tmp(wid, BIT4_0);
		  tmp.سېټ_بټ(wid-1, BIT4_1);
		  vala = tmp;
	    } وإلا {
		  اوږد tmpa = (اوږد) ap[0];
		  اوږد tmpb = (اوږد) bp[0];
		  اوږد res = tmpa / tmpb;
		  ap[0] = ((بینښه اوږد)res) & ~sign_ماسک;
		  vala.setarray(0, wid, ap);
	    }
	    ړنګول[]ap;
	    ړنګول[]bp;
	    أرجع ریښتیا;
      }

	// We need to the actual division to positive integers. Make
	// them positive here, and remember the negations.
      منطقي negate_flag = خطأ;
      إذا ( ((اوږد) ap[ټکي-1]) < 0 ) {
	    negate_flag = ریښتیا;
	    negate_ټکي(ap, ټکي);
      }
      إذا ( ((اوږد) bp[ټکي-1]) < 0 ) {
	    negate_flag ^= ریښتیا;
	    negate_ټکي(bp, ټکي);
      }

      بینښه اوږد*پایله = divide_بټونه(ap, bp, wid);
      إذا (پایله == 0) {
	    ړنګول[]ap;
	    ړنګول[]bp;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    أرجع ریښتیا;
      }

      إذا (negate_flag) {
	    negate_ټکي(پایله, ټکي);
      }

      پایله[ټکي-1] &= ~sign_ماسک;

      vala.setarray(0, wid, پایله);
      ړنګول[]ap;
      ړنګول[]bp;
      ړنګول[]پایله;
      أرجع ریښتیا;
}

منطقي د_DIV_WR(vthread_t thr, vvp_کوډ_t)
{
      مزدوج r = thr->راوباسه_real();
      مزدوج l = thr->راوباسه_real();
      thr->ټېل_real(l / r);

      أرجع ریښتیا;
}

/*
 * %dup/obj
 * %dup/real
 * %dup/vec4
 *
 * Push a duplicate of the object on the appropriate stack.
 */
منطقي د_DUP_OBJ(vthread_t thr, vvp_کوډ_t)
{
      vvp_object_t src = thr->peek_object();

        // If it is null push a new null object
      إذا (src.test_nil())
	    thr->ټېل_object(vvp_object_t());
      وإلا
	    thr->ټېل_object(src.duplicate());

      أرجع ریښتیا;
}

منطقي د_DUP_REAL(vthread_t thr, vvp_کوډ_t)
{
      thr->ټېل_real(thr->peek_real(0));
      أرجع ریښتیا;
}

منطقي د_DUP_VEC4(vthread_t thr, vvp_کوډ_t)
{
      thr->ټېل_vec4(thr->peek_vec4(0));
      أرجع ریښتیا;
}

/*
 * This terminates the current thread. If there is a parent who is
 * waiting for me to die, then I schedule it. At any rate, I mark
 * myself as a zombie by setting my pc to 0.
 */
منطقي د_پای_ټکی(vthread_t thr, vvp_کوډ_t)
{
      أكد(! thr->انتظار_for_event);
      thr->i_have_ended = 1;
      thr->pc = codespace_null();

	/* Fully detach any detached children. */
      طالما (! thr->detached_children.empty()) {
	    vthread_t ماشوم = *(thr->detached_children.begin());
	    أكد(ماشوم);
	    أكد(ماشوم->پلار == thr);
	    أكد(ماشوم->i_am_detached);
	    ماشوم->پلار = 0;
	    ماشوم->i_am_detached = 0;
	    thr->detached_children.erase(thr->detached_children.begin());
      }

	/* It is an error to still have active children running at this
	 * point in time. They should have all been detached or joined. */
      أكد(thr->children.empty());

	/* If I have a parent who is waiting for me, then mark that I
	   have ended, and schedule that parent. Also, finish the
	   %join for the parent. */
      إذا (!thr->i_am_detached && thr->پلار && thr->پلار->i_am_joining) {
	    vthread_t tmp = thr->پلار;
	    أكد(! thr->i_am_detached);

	    tmp->i_am_joining = 0;
	    مهالوېش_vthread(tmp, 0, ریښتیا);
	    do_join(tmp, thr);
	    أرجع خطأ;
      }

	/* If this thread is not fully detached then remove it from the
	 * parents detached_children set and reap it. */
      إذا (thr->i_am_detached) {
	    vthread_t tmp = thr->پلار;
	    أكد(tmp);
	    size_t res = tmp->detached_children.erase(thr);
	    أكد(res == 1);
	      /* If the parent is waiting for the detached children to
	       * finish then the last detached child needs to tell the
	       * parent to wake up when it is finished. */
	    إذا (tmp->i_am_انتظار && tmp->detached_children.empty()) {
		  tmp->i_am_انتظار = 0;
		  مهالوېش_vthread(tmp, 0, ریښتیا);
	    }
	      /* Fully detach this thread so it will be reaped below. */
	    thr->i_am_detached = 0;
	    thr->پلار = 0;
      }

	/* If I have no parent, then no one can %join me and there is
	 * no reason to stick around. This can happen, for example if
	 * I am an ``initial'' thread. */
      إذا (thr->پلار == 0) {
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
منطقي د_EVENT(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_vector4_t tmp (1, BIT4_X);
      vvp_send_vec4(ptr, tmp, thr->wt_context);
      أرجع ریښتیا;
}

/*
 * %event/nb <var-label>, <delay>
 */
منطقي د_EVENT_NB(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_time64_t delay;

      delay = thr->ټکي[cp->بټ_idx[0]].w_uint;
      مهالوېش_propagate_event(cp->net, delay);
      أرجع ریښتیا;
}

منطقي د_EVCTL(vthread_t thr, vvp_کوډ_t cp)
{
      أكد(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      thr->ecount = thr->ټکي[cp->بټ_idx[0]].w_uint;
      أرجع ریښتیا;
}
منطقي د_EVCTLC(vthread_t thr, vvp_کوډ_t)
{
      thr->event = 0;
      thr->ecount = 0;
      أرجع ریښتیا;
}

منطقي د_EVCTLI(vthread_t thr, vvp_کوډ_t cp)
{
      أكد(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      thr->ecount = cp->بټ_idx[0];
      أرجع ریښتیا;
}

منطقي د_EVCTLS(vthread_t thr, vvp_کوډ_t cp)
{
      أكد(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      int64_t val = thr->ټکي[cp->بټ_idx[0]].w_int;
      إذا (val < 0) val = 0;
      thr->ecount = val;
      أرجع ریښتیا;
}

منطقي د_FLAG_GET_VEC4(vthread_t thr, vvp_کوډ_t cp)
{
      صحيح flag = cp->شمېره;
      أكد(flag < vthread_s::FLAGS_شمېر);

      vvp_vector4_t val (1, thr->flags[flag]);
      thr->ټېل_vec4(val);

      أرجع ریښتیا;
}

/*
 * %flag_inv <flag1>
 */
منطقي د_FLAG_INV(vthread_t thr, vvp_کوډ_t cp)
{
      صحيح flag1 = cp->بټ_idx[0];

      thr->flags[flag1] = ~ thr->flags[flag1];
      أرجع ریښتیا;
}

/*
 * %flag_mov <flag1>, <flag2>
 */
منطقي د_FLAG_MOV(vthread_t thr, vvp_کوډ_t cp)
{
      صحيح flag1 = cp->بټ_idx[0];
      صحيح flag2 = cp->بټ_idx[1];

      thr->flags[flag1] = thr->flags[flag2];
      أرجع ریښتیا;
}

/*
 * %flag_or <flag1>, <flag2>
 */
منطقي د_FLAG_OR(vthread_t thr, vvp_کوډ_t cp)
{
      صحيح flag1 = cp->بټ_idx[0];
      صحيح flag2 = cp->بټ_idx[1];

      thr->flags[flag1] = thr->flags[flag1] | thr->flags[flag2];
      أرجع ریښتیا;
}

منطقي د_FLAG_سېټ_IMM(vthread_t thr, vvp_کوډ_t cp)
{
      صحيح flag = cp->شمېره;
      صحيح vali = cp->بټ_idx[0];

      أكد(flag < vthread_s::FLAGS_شمېر);
      أكد(vali >= 0 && vali < 4);

      ساكن ثابت vvp_bit4_t map_بټ[4] = {BIT4_0, BIT4_1, BIT4_Z, BIT4_X};
      thr->flags[flag] = map_بټ[vali];
      أرجع ریښتیا;
}

منطقي د_FLAG_سېټ_VEC4(vthread_t thr, vvp_کوډ_t cp)
{
      صحيح flag = cp->شمېره;
      أكد(flag < vthread_s::FLAGS_شمېر);

      ثابت vvp_vector4_t&val = thr->peek_vec4();
      thr->flags[flag] = val.value(0);
      thr->راوباسه_vec4(1);

      أرجع ریښتیا;
}

/*
 * the %force/link instruction connects a source node to a
 * destination node. The destination node must be a signal, as it is
 * marked with the source of the force so that it may later be
 * unlinked without specifically knowing the source that this
 * instruction used.
 */
منطقي د_FORCE_LINK(vthread_t, vvp_کوډ_t cp)
{
      vvp_net_t*dst = cp->net;
      vvp_net_t*src = cp->net2;

      أكد(dst->fil);
      dst->fil->force_link(dst, src);

      أرجع ریښتیا;
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
منطقي د_FORCE_VEC4(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_vector4_t value = thr->راوباسه_vec4();

	/* Send the force value to the filter on the node. */

      أكد(net->fil);
      إذا (value.size() != net->fil->filter_اندازه())
	    value = coerce_منزل_پلنوالی(value, net->fil->filter_اندازه());

      net->force_vec4(value, vvp_vector2_t(vvp_vector2_t::FILL1, net->fil->filter_اندازه()));

      أرجع ریښتیا;
}

/*
 * %force/vec4/off <net>, <off>
 */
منطقي د_FORCE_VEC4_OFF(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_net_t*net = cp->net;
      بینښه بنسټ_idx = cp->بټ_idx[0];
      اوږد بنسټ = thr->ټکي[بنسټ_idx].w_int;
      vvp_vector4_t value = thr->راوباسه_vec4();
      بینښه wid = value.size();

      أكد(net->fil);

      إذا (thr->flags[4] == BIT4_1)
	    أرجع ریښتیا;

	// This is the width of the target vector.
      بینښه use_اندازه = net->fil->filter_اندازه();

      إذا (بنسټ >= (اوږد)use_اندازه)
	    أرجع ریښتیا;
      إذا (بنسټ < -(اوږد)use_اندازه)
	    أرجع ریښتیا;

      إذا ((بنسټ + wid) > use_اندازه)
	    wid = use_اندازه - بنسټ;

	// Make a mask of which bits are to be forced, 0 for unforced
	// bits and 1 for forced bits.
      vvp_vector2_t ماسک (vvp_vector2_t::FILL0, use_اندازه);
      لكل (بینښه idx = 0 ; idx < wid ; idx += 1)
	    ماسک.سېټ_بټ(بنسټ+idx, 1);

      vvp_vector4_t tmp (use_اندازه, BIT4_Z);

	// vvp_net_t::force_vec4 propagates all the bits of the
	// forced vector value, regardless of the mask. This
	// ensures the unforced bits retain their current value.
      vvp_signal_ارزښت*sig = dynamic_cast<vvp_signal_ارزښت*>(net->fil);
      أكد(sig);
      sig->vec4_ارزښت(tmp);

      tmp.سېټ_vec(بنسټ, value);

      net->force_vec4(tmp, ماسک);
      أرجع ریښتیا;
}

/*
 * %force/vec4/off/d <net>, <off>, <del>
 */
منطقي د_FORCE_VEC4_OFF_D(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_net_t*net = cp->net;

      بینښه بنسټ_idx = cp->بټ_idx[0];
      اوږد بنسټ = thr->ټکي[بنسټ_idx].w_int;

      بینښه delay_idx = cp->بټ_idx[1];
      vvp_time64_t delay = thr->ټکي[delay_idx].w_uint;

      vvp_vector4_t value = thr->راوباسه_vec4();

      أكد(net->fil);

      إذا (thr->flags[4] == BIT4_1)
	    أرجع ریښتیا;

	// This is the width of the target vector.
      بینښه use_اندازه = net->fil->filter_اندازه();

      إذا (بنسټ >= (اوږد)use_اندازه)
	    أرجع ریښتیا;
      إذا (بنسټ < -(اوږد)use_اندازه)
	    أرجع ریښتیا;

      مهالوېش_force_ویکټور(net, بنسټ, use_اندازه, value, delay);
      أرجع ریښتیا;
}

منطقي د_FORCE_WR(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_net_t*net  = cp->net;
      مزدوج value = thr->راوباسه_real();

      net->force_real(value, vvp_vector2_t(vvp_vector2_t::FILL1, 1));

      أرجع ریښتیا;
}

/*
 * The %fork instruction causes a new child to be created and pushed
 * in front of any existing child. This causes the new child to be
 * added to the list of children, and for me to be the parent of the
 * new child.
 */
منطقي د_FORK(vthread_t thr, vvp_کوډ_t cp)
{
      vthread_t ماشوم = vthread_new(cp->cptr2, cp->scope);

      إذا (cp->scope->is_automatic()) {
              /* The context allocated for this child is the top entry
                 on the write context stack. */
            ماشوم->wt_context = thr->wt_context;
            ماشوم->rd_context = thr->wt_context;
      }

      ماشوم->پلار = thr;
      thr->children.insert(ماشوم);

      إذا (thr->i_am_دننه_function) {
	    ماشوم->is_scheduled = 1;
	    ماشوم->i_am_دننه_function = 1;
	    vthread_پرلپسې(ماشوم);
	    running_thread = thr;
      } وإلا {
	    مهالوېش_vthread(ماشوم, 0, ریښتیا);
      }
      أرجع ریښتیا;
}

منطقي د_FREE(vthread_t thr, vvp_کوډ_t cp)
{
        /* Pop the child context from the read context stack. */
      vvp_context_t ماشوم_context = thr->rd_context;
      thr->rd_context = vvp_get_stacked_context(ماشوم_context);

        /* Free the context. */
      vthread_free_context(ماشوم_context, cp->scope);

      أرجع ریښتیا;
}

/*
 * %inv
 *
 * Logically, this pops a value, inverts is (Verilog style, with Z and
 * X converted to X) and pushes the result. We can more efficiently
 * just to the invert in place.
 */
منطقي د_INV(vthread_t thr, vvp_کوډ_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val.invert();
      أرجع ریښتیا;
}


/*
 * Index registers, arithmetic.
 */

ساكن مضمن int64_t get_as_64_بټ(uint32_t ټیټ_32, uint32_t لوړ_32)
{
      int64_t ټیټ = ټیټ_32;
      int64_t res = لوړ_32;

      res <<= 32;
      res |= ټیټ;
      أرجع res;
}

منطقي د_IX_جمع_کړه(vthread_t thr, vvp_کوډ_t cp)
{
      thr->ټکي[cp->شمېره].w_int += get_as_64_بټ(cp->بټ_idx[0],
                                                    cp->بټ_idx[1]);
      أرجع ریښتیا;
}

منطقي د_IX_SUB(vthread_t thr, vvp_کوډ_t cp)
{
      thr->ټکي[cp->شمېره].w_int -= get_as_64_بټ(cp->بټ_idx[0],
                                                    cp->بټ_idx[1]);
      أرجع ریښتیا;
}

منطقي د_IX_MUL(vthread_t thr, vvp_کوډ_t cp)
{
      thr->ټکي[cp->شمېره].w_int *= get_as_64_بټ(cp->بټ_idx[0],
                                                    cp->بټ_idx[1]);
      أرجع ریښتیا;
}

منطقي د_IX_بارول(vthread_t thr, vvp_کوډ_t cp)
{
      thr->ټکي[cp->شمېره].w_int = get_as_64_بټ(cp->بټ_idx[0],
                                                   cp->بټ_idx[1]);
      أرجع ریښتیا;
}

منطقي د_IX_MOV(vthread_t thr, vvp_کوډ_t cp)
{
      thr->ټکي[cp->بټ_idx[0]].w_int = thr->ټکي[cp->بټ_idx[1]].w_int;
      أرجع ریښتیا;
}

منطقي د_IX_GETV(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه شاخص = cp->بټ_idx[0];
      vvp_net_t*net = cp->net;

      vvp_signal_ارزښت*sig = dynamic_cast<vvp_signal_ارزښت*>(net->fil);
      إذا (sig == 0) {
	    أكد(net->fil);
	    cerr << thr->get_fileline()
	         << "%%ix/getv error: Net arg not a vector signal? "
		 << typeid(*net->fil).name() << کرښه_پای;
      }
      أكد(sig);

      vvp_vector4_t vec;
      sig->vec4_ارزښت(vec);
      منطقي ډکېدنه_flag;
      uint64_t val = 0;
      منطقي معلوم_flag = vector4_منزل_ارزښت(vec, ډکېدنه_flag, val);

      إذا (معلوم_flag)
	    thr->ټکي[شاخص].w_uint = val;
      وإلا
	    thr->ټکي[شاخص].w_uint = 0;

	/* Set bit 4 as a flag if the input is unknown. */
      thr->flags[4] = معلوم_flag ? (ډکېدنه_flag ? BIT4_X : BIT4_0) : BIT4_1;

      أرجع ریښتیا;
}

منطقي د_IX_GETV_S(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه شاخص = cp->بټ_idx[0];
      vvp_net_t*net = cp->net;

      vvp_signal_ارزښت*sig = dynamic_cast<vvp_signal_ارزښت*>(net->fil);
      إذا (sig == 0) {
	    أكد(net->fil);
	    cerr << thr->get_fileline()
	         << "%%ix/getv/s error: Net arg not a vector signal? "
		 << "fun=" << typeid(*net->fil).name()
		 << ", fil=" << (net->fil? typeid(*net->fil).name() : "<>")
		 << کرښه_پای;
      }
      أكد(sig);

      vvp_vector4_t vec;
      sig->vec4_ارزښت(vec);
      int64_t val;
      منطقي معلوم_flag = vector4_منزل_ارزښت(vec, val, ریښتیا, ریښتیا);

      إذا (معلوم_flag)
	    thr->ټکي[شاخص].w_int = val;
      وإلا
	    thr->ټکي[شاخص].w_int = 0;

	/* Set bit 4 as a flag if the input is unknown. */
      thr->flags[4] = معلوم_flag? BIT4_0 : BIT4_1;

      أرجع ریښتیا;
}

ساكن uint64_t vec4_منزل_شاخص(vthread_t thr, منطقي signed_flag)
{
	// Get all the information we need about the vec4 vector, then
	// pop it away. We only need the bool bits and the length.
      ثابت vvp_vector4_t&val = thr->peek_vec4();
      بینښه val_اندازه = val.size();
      بینښه اوږد*بټونه = val.subarray(0, val_اندازه, خطأ);
      thr->راوباسه_vec4(1);

	// If there are X/Z bits, then the subarray will give us a nil
	// pointer. Set a flag to indicate the error, and give up.
      إذا (بټونه == 0) {
	    thr->flags[4] = BIT4_1;
	    أرجع 0;
      }

      uint64_t v = 0;
      thr->flags[4] = BIT4_0;

      أكد(حجم(بټونه[0]) <= حجم(v));

      v = 0;
      لكل (بینښه idx = 0 ; idx < val_اندازه ; idx += 8*حجم(بټونه[0])) {
	    uint64_t tmp = بټونه[idx/8/حجم(بټونه[0])];
	    إذا (idx < 8*حجم(v)) {
		  v |= tmp << idx;
	    } وإلا {
		  منطقي ډکېدنه = signed_flag && (v >> 63) ? ~tmp != 0 : tmp != 0;
		  إذا (ډکېدنه) {
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

      ړنګول[]بټونه;
      أرجع v;
}

/*
 * %ix/vec4 <idx>
 */
منطقي د_IX_VEC4(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه use_idx = cp->شمېره;
      thr->ټکي[use_idx].w_uint = vec4_منزل_شاخص(thr, خطأ);
      أرجع ریښتیا;
}

/*
 * %ix/vec4/s <idx>
 */
منطقي د_IX_VEC4_S(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه use_idx = cp->شمېره;
      thr->ټکي[use_idx].w_uint = vec4_منزل_شاخص(thr, ریښتیا);
      أرجع ریښتیا;
}

/*
 * The various JMP instruction work simply by pulling the new program
 * counter from the instruction and resuming. If the jump is
 * conditional, then test the bit for the expected value first.
 */
منطقي د_JMP(vthread_t thr, vvp_کوډ_t cp)
{
      thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      إذا (مهالوېش_stopped()) {
	    مهالوېش_vthread(thr, 0, خطأ);
	    أرجع خطأ;
      }

      أرجع ریښتیا;
}

/*
 * %jmp/0 <pc>, <flag>
 */
منطقي د_JMP0(vthread_t thr, vvp_کوډ_t cp)
{
      إذا (thr->flags[cp->بټ_idx[0]] == BIT4_0)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      إذا (مهالوېش_stopped()) {
	    مهالوېش_vthread(thr, 0, خطأ);
	    أرجع خطأ;
      }

      أرجع ریښتیا;
}

/*
 * %jmp/0xz <pc>, <flag>
 */
منطقي د_JMP0_XZ(vthread_t thr, vvp_کوډ_t cp)
{
      إذا (thr->flags[cp->بټ_idx[0]] != BIT4_1)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      إذا (مهالوېش_stopped()) {
	    مهالوېش_vthread(thr, 0, خطأ);
	    أرجع خطأ;
      }

      أرجع ریښتیا;
}

/*
 * %jmp/1 <pc>, <flag>
 */
منطقي د_JMP1(vthread_t thr, vvp_کوډ_t cp)
{
      إذا (thr->flags[cp->بټ_idx[0]] == BIT4_1)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      إذا (مهالوېش_stopped()) {
	    مهالوېش_vthread(thr, 0, خطأ);
	    أرجع خطأ;
      }

      أرجع ریښتیا;
}

/*
 * %jmp/1xz <pc>, <flag>
 */
منطقي د_JMP1_XZ(vthread_t thr, vvp_کوډ_t cp)
{
      إذا (thr->flags[cp->بټ_idx[0]] != BIT4_0)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      إذا (مهالوېش_stopped()) {
	    مهالوېش_vthread(thr, 0, خطأ);
	    أرجع خطأ;
      }

      أرجع ریښتیا;
}

/*
 * The %join instruction causes the thread to wait for one child
 * to die.  If a child is already dead (and a zombie) then I reap
 * it and go on. Otherwise, I mark myself as waiting in a join so that
 * children know to wake me when they finish.
 */

ساكن فراغ do_join(vthread_t thr, vthread_t ماشوم)
{
      أكد(ماشوم->پلار == thr);

        /* If the immediate child thread is in an automatic scope... */
      إذا (ماشوم->wt_context) {
              /* and is the top level task/function thread... */
            إذا (thr->wt_context != thr->rd_context) {
                    /* Pop the child context from the write context stack. */
                  vvp_context_t ماشوم_context = thr->wt_context;
                  thr->wt_context = vvp_get_stacked_context(ماشوم_context);

                    /* Push the child context onto the read context stack */
                  vvp_سېټ_stacked_context(ماشوم_context, thr->rd_context);
                  thr->rd_context = ماشوم_context;
            }
      }

      vthread_reap(ماشوم);
}

ساكن منطقي do_join_opcode(vthread_t thr)
{
      أكد( !thr->i_am_joining );
      أكد( !thr->children.empty());

	// Are there any children that have already ended? If so, then
	// join with that one.
      لكل (سېټ<vthread_t>::iterator cur = thr->children.begin()
		 ; cur != thr->children.پای_ټکی() ; ++cur) {
	    vthread_t curp = *cur;
	    إذا (! curp->i_have_ended)
		  استمر;

	      // found something!
	    do_join(thr, curp);
	    أرجع ریښتیا;
      }

	// Otherwise, tell my children to awaken me when they end,
	// then pause.
      thr->i_am_joining = 1;
      أرجع خطأ;
}

منطقي د_JOIN(vthread_t thr, vvp_کوډ_t)
{
      أرجع do_join_opcode(thr);
}

/*
 * This %join/detach <n> instruction causes the thread to detach
 * threads that were created by an earlier %fork.
 */
منطقي د_JOIN_DETACH(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه اوږد شمېر = cp->شمېره;

      أكد(شمېر == thr->children.size());

      طالما (! thr->children.empty()) {
	    vthread_t ماشوم = *thr->children.begin();
	    أكد(ماشوم->پلار == thr);

	      // We cannot detach automatic tasks/functions within an
	      // automatic scope. If we try to do that, we might make
	      // a mess of the allocation of the context. Note that it
	      // is OK if the child context is distinct (See %exec_ufunc.)
	    أكد(ماشوم->wt_context==0 || thr->wt_context!=ماشوم->wt_context);
	    إذا (ماشوم->i_have_ended) {
		    // If the child has already ended, then reap it.
		  vthread_reap(ماشوم);

	    } وإلا {
		  size_t res = ماشوم->پلار->children.erase(ماشوم);
		  أكد(res == 1);
		  ماشوم->i_am_detached = 1;
		  thr->detached_children.insert(ماشوم);
	    }
      }

      أرجع ریښتیا;
}

/*
 * %load/ar <array-label>, <index>;
*/
منطقي د_بارول_AR(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه idx = cp->بټ_idx[0];
      مزدوج کلمه;

	/* The result is 0.0 if the address is undefined. */
      إذا (thr->flags[4] == BIT4_1) {
	    کلمه = 0.0;
      } وإلا {
	    بینښه adr = thr->ټکي[idx].w_int;
	    کلمه = cp->array->get_کلمه_r(adr);
      }

      thr->ټېل_real(کلمه);
      أرجع ریښتیا;
}

کينډۍ <ډول_نوم ELEM>
ساكن منطقي بارول_dar(vthread_t thr, vvp_کوډ_t cp)
{
      int64_t adr = thr->ټکي[3].w_int;
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

      vthread_ټېل(thr, کلمه);
      أرجع ریښتیا;
}

/*
 * %load/dar/r <array-label>;
 */
منطقي د_بارول_DAR_R(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع بارول_dar<مزدوج>(thr, cp);
}

/*
 * %load/dar/str <array-label>;
 */
منطقي د_بارول_DAR_STR(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع بارول_dar<string>(thr, cp);
}

/*
 * %load/dar/vec4 <array-label>;
 */
منطقي د_بارول_DAR_VEC4(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع بارول_dar<vvp_vector4_t>(thr, cp);
}

/*
 * %load/prop/dar/vec4 <pid>, <wid>;
 *   Indexed read of queue/dynamic-array class property; object on stack,
 *   index in words[3] (same as %load/dar/vec4).
 */
منطقي د_بارول_PROP_DAR_VEC4(vthread_t thr, vvp_کوډ_t cp)
{
      size_t pid = cp->شمېره;
      بینښه wid = cp->بټ_idx[0];
      int64_t adr = thr->ټکي[3].w_int;

      vvp_object_t& سر = thr->peek_object();
      vvp_cobject*cobj = سر.peek<vvp_cobject>();
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

      thr->ټېل_vec4(کلمه);
      أرجع ریښتیا;
}

/*
 * %load/obj <var-label>
 */
منطقي د_بارول_OBJ(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_net_t*net = cp->net;
      vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      أكد(fun);

      vvp_object_t val = fun->get_object();
      thr->ټېل_object(val);

      أرجع ریښتیا;
}

/*
 * %load/obja <index>
 *    Loads the object from array, using index <index> as the index
 *    value. If flags[4] == 1, the calculation of <index> may have
 *    failed, so push nil.
 */
منطقي د_بارول_OBJA(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه idx = cp->بټ_idx[0];
      vvp_object_t کلمه;

	/* The result is 0.0 if the address is undefined. */
      إذا (thr->flags[4] == BIT4_1) {
	    ; // Return nil
      } وإلا {
	    بینښه adr = thr->ټکي[idx].w_int;
	    cp->array->get_کلمه_obj(adr, کلمه);
      }

      thr->ټېل_object(کلمه);
      أرجع ریښتیا;
}

/*
 * %load/real <var-label>
 */
منطقي د_بارول_REAL(vthread_t thr, vvp_کوډ_t cp)
{
      __vpiHandle*tmp = cp->handle;
      t_vpi_ارزښت val;

      val.format = vpiRealVal;
      vpi_get_ارزښت(tmp, &val);

      thr->ټېل_real(val.value.real);

      أرجع ریښتیا;
}

/*
 * %load/str <var-label>
 */
منطقي د_بارول_STR(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_net_t*net = cp->net;


      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (net->fun);
      أكد(fun);

      ثابت string&val = fun->get_string();
      thr->ټېل_str(val);

      أرجع ریښتیا;
}

منطقي د_بارول_STRA(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه idx = cp->بټ_idx[0];
      string کلمه;

      إذا (thr->flags[4] == BIT4_1) {
	    کلمه = "";
      } وإلا {
	    بینښه adr = thr->ټکي[idx].w_int;
	    کلمه = cp->array->get_کلمه_str(adr);
      }

      thr->ټېل_str(کلمه);
      أرجع ریښتیا;
}


/*
 * %load/vec4 <net>
 */
منطقي د_بارول_VEC4(vthread_t thr, vvp_کوډ_t cp)
{
	// Push a placeholder onto the stack in order to reserve the
	// stack space. Use a reference for the stack top as a target
	// for the load.
      thr->ټېل_vec4(vvp_vector4_t());
      vvp_vector4_t&sig_ارزښت = thr->peek_vec4();

      vvp_net_t*net = cp->net;

	// For the %load to work, the functor must actually be a
	// signal functor. Only signals save their vector value.
      vvp_signal_ارزښت*sig = dynamic_cast<vvp_signal_ارزښت*> (net->fil);
      إذا (sig == 0) {
	    cerr << thr->get_fileline()
	         << "%load/v error: Net arg not a signal? "
		 << (net->fil ? typeid(*net->fil).name() :
	                        typeid(*net->fun).name())
	         << کرښه_پای;
	    أكد(sig);
	    أرجع ریښتیا;
      }

	// Extract the value from the signal and directly into the
	// target stack position.
      sig->vec4_ارزښت(sig_ارزښت);

      أرجع ریښتیا;
}

/*
 * %load/vec4a <arr>, <adrx>
 */
منطقي د_بارول_VEC4_A(vthread_t thr, vvp_کوډ_t cp)
{
      صحيح adr_شاخص = cp->بټ_idx[0];

      اوږد adr = thr->ټکي[adr_شاخص].w_int;

	// If flag[3] is set, then the calculation of the address
	// failed, and this load should return X instead of the actual
	// value.
      إذا (thr->flags[4] == BIT4_1) {
	    vvp_vector4_t tmp (cp->array->get_کلمه_اندازه(), BIT4_X);
	    thr->ټېل_vec4(tmp);
	    أرجع ریښتیا;
      }

      vvp_vector4_t tmp (cp->array->get_کلمه(adr));
      thr->ټېل_vec4(tmp);
      أرجع ریښتیا;
}

ساكن فراغ do_verylong_mod(vvp_vector4_t&vala, ثابت vvp_vector4_t&valb,
			    منطقي کیڼ_is_neg, منطقي ښی_is_neg)
{
      منطقي بهر_is_neg = کیڼ_is_neg;
      ثابت صحيح len=vala.size();
      بینښه حرف *a, *z, *t;
      a = جديد بینښه حرف[len+1];
      z = جديد بینښه حرف[len+1];
      t = جديد بینښه حرف[len+1];

      بینښه حرف لېږد;
      بینښه حرف لنډمهاله;

      صحيح mxa = -1, mxz = -1;
      صحيح i;
      صحيح current, copylen;

      بینښه lb_لېږد = کیڼ_is_neg? 1 : 0;
      بینښه rb_لېږد = ښی_is_neg? 1 : 0;
      لكل (صحيح idx = 0 ;  idx < len ;  idx += 1) {
	    بینښه lb = vala.value(idx);
	    بینښه rb = valb.value(idx);

	    إذا ((lb | rb) & 2) {
		  ړنګول []t;
		  ړنګول []z;
		  ړنګول []a;
		  vvp_vector4_t tmp(len, BIT4_X);
		  vala = tmp;
		  أرجع;
	    }

	    إذا (کیڼ_is_neg) {
		  lb = (1-lb) + lb_لېږد;
		  lb_لېږد = (lb & ~1)? 1 : 0;
		  lb &= 1;
	    }
	    إذا (ښی_is_neg) {
		  rb = (1-rb) + rb_لېږد;
		  rb_لېږد = (rb & ~1)? 1 : 0;
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
		  ړنګول []t;
		  ړنګول []z;
		  ړنګول []a;
		  vvp_vector4_t tmpx (len, BIT4_X);
		  vala = tmpx;
		  أرجع;
	    }

	    اذهب tally;
      }

      copylen = mxa + 2;
      current = mxz - mxa;

      طالما(current > -1) {
	    لېږد = 1;
	    لكل(i=0;i<copylen;i++) {
		  لنډمهاله = z[i+current] + a[i] + لېږد;
		  t[i] = (لنډمهاله&1);
		  لېږد = (لنډمهاله>>1);
	    }

	    إذا(لېږد) {
		  لكل(i=0;i<copylen;i++) {
			z[i+current] = t[i];
		  }
	    }

	    current--;
      }

 tally:

      vvp_vector4_t tmp (len, BIT4_X);
      لېږد = بهر_is_neg? 1 : 0;
      لكل (صحيح idx = 0 ;  idx < len ;  idx += 1) {
	    بینښه ob = z[idx];
	    إذا (بهر_is_neg) {
		  ob = (1-ob) + لېږد;
		  لېږد = (ob & ~1)? 1 : 0;
		  ob = ob & 1;
	    }
	    tmp.سېټ_بټ(idx, ob?BIT4_1:BIT4_0);
      }
      vala = tmp;
      ړنګول []t;
      ړنګول []z;
      ړنګول []a;
}

منطقي د_MAX_WR(vthread_t thr, vvp_کوډ_t)
{
      مزدوج r = thr->راوباسه_real();
      مزدوج l = thr->راوباسه_real();
      إذا (r != r)
	    thr->ټېل_real(l);
      وإلا إذا (l != l)
	    thr->ټېل_real(r);
      وإلا إذا (r < l)
	    thr->ټېل_real(l);
      وإلا
	    thr->ټېل_real(r);
      أرجع ریښتیا;
}

منطقي د_MIN_WR(vthread_t thr, vvp_کوډ_t)
{
      مزدوج r = thr->راوباسه_real();
      مزدوج l = thr->راوباسه_real();
      إذا (r != r)
	    thr->ټېل_real(l);
      وإلا إذا (l != l)
	    thr->ټېل_real(r);
      وإلا إذا (r < l)
	    thr->ټېل_real(r);
      وإلا
	    thr->ټېل_real(l);
      أرجع ریښتیا;
}

منطقي د_MOD(vthread_t thr, vvp_کوډ_t)
{
      vvp_vector4_t valb = thr->راوباسه_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      أكد(vala.size()==valb.size());
      بینښه wid = vala.size();

      إذا(wid <= 8*حجم(بینښه اوږد اوږد)) {
	    بینښه اوږد اوږد lv = 0, rv = 0;

	    لكل (بینښه idx = 0 ;  idx < wid ;  idx += 1) {
		  بینښه اوږد اوږد lb = vala.value(idx);
		  بینښه اوږد اوږد rb = valb.value(idx);

		  إذا ((lb | rb) & 2)
			اذهب x_بهر;

		  lv |= (بینښه اوږد اوږد) lb << idx;
		  rv |= (بینښه اوږد اوږد) rb << idx;
	    }

	    إذا (rv == 0)
		  اذهب x_بهر;

	    lv %= rv;

	    لكل (بینښه idx = 0 ;  idx < wid ;  idx += 1) {
		  vala.سېټ_بټ(idx, (lv&1)?BIT4_1 : BIT4_0);
		  lv >>= 1;
	    }

	    أرجع ریښتیا;

      } وإلا {
	    do_verylong_mod(vala, valb, خطأ, خطأ);
	    أرجع ریښتیا;
      }

 x_بهر:
      vala = vvp_vector4_t(wid, BIT4_X);
      أرجع ریښتیا;
}

/*
 * %mod/s
 */
منطقي د_MOD_S(vthread_t thr, vvp_کوډ_t)
{
      vvp_vector4_t valb = thr->راوباسه_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      أكد(vala.size()==valb.size());
      بینښه wid = vala.size();

	/* Handle the case that we can fit the bits into a long-long
	   variable. We cause use native % to do the work. */
      إذا(wid <= 8*حجم(اوږد اوږد)) {
	    اوږد اوږد lv = 0, rv = 0;

	    لكل (بینښه idx = 0 ;  idx < wid ;  idx += 1) {
		  اوږد اوږد lb = vala.value(idx);
		  اوږد اوږد rb = valb.value(idx);

		  إذا ((lb | rb) & 2)
			اذهب x_بهر;

		  lv |= (اوږد اوږد) lb << idx;
		  rv |= (اوږد اوږد) rb << idx;
	    }

	    إذا (rv == 0)
		  اذهب x_بهر;

	    إذا ((lv == LLONG_MIN) && (rv == -1))
		  اذهب zero_بهر;

	      /* Sign extend the signed operands when needed. */
	    إذا (wid < 8*حجم(اوږد اوږد)) {
		  إذا (lv & (1LL << (wid-1)))
			lv |= -1ULL << wid;
		  إذا (rv & (1LL << (wid-1)))
			rv |= -1ULL << wid;
	    }

	    lv %= rv;

	    لكل (بینښه idx = 0 ;  idx < wid ;  idx += 1) {
		  vala.سېټ_بټ(idx, (lv&1)? BIT4_1 : BIT4_0);
		  lv >>= 1;
	    }

	      // vala is the top of the stack, edited in place, so we
	      // do not need to push the result.

	    أرجع ریښتیا;

      } وإلا {

	    منطقي کیڼ_is_neg  = vala.value(vala.size()-1) == BIT4_1;
	    منطقي ښی_is_neg = valb.value(valb.size()-1) == BIT4_1;
	    do_verylong_mod(vala, valb, کیڼ_is_neg, ښی_is_neg);
	    أرجع ریښتیا;
      }

 x_بهر:
      vala = vvp_vector4_t(wid, BIT4_X);
      أرجع ریښتیا;
 zero_بهر:
      vala = vvp_vector4_t(wid, BIT4_0);
      أرجع ریښتیا;
}

/*
 * %mod/wr
 */
منطقي د_MOD_WR(vthread_t thr, vvp_کوډ_t)
{
      مزدوج r = thr->راوباسه_real();
      مزدوج l = thr->راوباسه_real();
      thr->ټېل_real(fmod(l,r));

      أرجع ریښتیا;
}

/*
 * %neg/wr
 */
منطقي د_NEG_WR(vthread_t thr, vvp_کوډ_t)
{
      thr->poke_real(0, -thr->peek_real(0));
      أرجع ریښتیا;
}

/*
 * %pad/s <wid>
 */
منطقي د_PAD_S(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه wid = cp->شمېره;

      vvp_vector4_t&val = thr->peek_vec4();
      بینښه old_اندازه = val.size();

	// Sign-extend.
      إذا (old_اندازه < wid)
	    val.resize(wid, val.value(old_اندازه-1));
      وإلا
	    val.resize(wid);

      أرجع ریښتیا;
}

/*
 * %pad/u <wid>
 */
منطقي د_PAD_U(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه wid = cp->شمېره;

      vvp_vector4_t&val = thr->peek_vec4();
      val.resize(wid, BIT4_0);

      أرجع ریښتیا;
}

/*
 * %part/s <wid>
 * %part/u <wid>
 * Two values are popped from the stack. First, pop the canonical
 * index of the part select, and second is the value to be
 * selected. The result is pushed back to the stack.
 */
ساكن منطقي د_PART_بنسټ(vthread_t thr, vvp_کوډ_t cp, منطقي signed_flag)
{
      بینښه wid = cp->شمېره;

      vvp_vector4_t base4 = thr->راوباسه_vec4();
      vvp_vector4_t&value = thr->peek_vec4();

      vvp_vector4_t res (wid, BIT4_X);

	// NOTE: This is treating the vector as signed. Is that correct?
      int32_t بنسټ;
      منطقي ارزښت_ok = vector4_منزل_ارزښت(base4, بنسټ, signed_flag);
      إذا (! ارزښت_ok) {
	    value = res;
	    أرجع ریښتیا;
      }

      إذا (بنسټ >= (int32_t)value.size()) {
	    value = res;
	    أرجع ریښتیا;
      }

      إذا ((بنسټ+(صحيح)wid) <= 0) {
	    value = res;
	    أرجع ریښتیا;
      }

      اوږد vbase = 0;
      إذا (بنسټ < 0) {
	    vbase = -بنسټ;
	    wid -= vbase;
	    بنسټ = 0;
      }

      إذا ((بنسټ+wid) > value.size()) {
	    wid = value.size() - بنسټ;
      }

      res .سېټ_vec(vbase, value.subvalue(بنسټ, wid));
      value = res;

      أرجع ریښتیا;
}

منطقي د_PART_S(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع د_PART_بنسټ(thr, cp, ریښتیا);
}

منطقي د_PART_U(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع د_PART_بنسټ(thr, cp, خطأ);
}

/*
 * %parti/s <wid>, <basei>, <base_wid>
 * %parti/u <wid>, <basei>, <base_wid>
 *
 * Pop the value to be selected. The result is pushed back to the stack.
 */
ساكن منطقي د_PARTI_بنسټ(vthread_t thr, vvp_کوډ_t cp, منطقي signed_flag)
{
      بینښه wid = cp->شمېره;
      uint32_t بنسټ = cp->بټ_idx[0];
      uint32_t bwid = cp->بټ_idx[1];

      vvp_vector4_t&value = thr->peek_vec4();

      vvp_vector4_t res (wid, BIT4_X);

	// NOTE: This is treating the vector as signed. Is that correct?
      int32_t use_بنسټ = بنسټ;
      إذا (signed_flag && bwid < 32 && (بنسټ&(1<<(bwid-1)))) {
	    use_بنسټ |= -1UL << bwid;
      }

      إذا (use_بنسټ >= (int32_t)value.size()) {
	    value = res;
	    أرجع ریښتیا;
      }

      إذا ((use_بنسټ+(int32_t)wid) <= 0) {
	    value = res;
	    أرجع ریښتیا;
      }

      اوږد vbase = 0;
      إذا (use_بنسټ < 0) {
	    vbase = -use_بنسټ;
	    wid -= vbase;
	    use_بنسټ = 0;
      }

      إذا ((use_بنسټ+wid) > value.size()) {
	    wid = value.size() - use_بنسټ;
      }

      res .سېټ_vec(vbase, value.subvalue(use_بنسټ, wid));
      value = res;

      أرجع ریښتیا;
}

منطقي د_PARTI_S(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع د_PARTI_بنسټ(thr, cp, ریښتیا);
}

منطقي د_PARTI_U(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع د_PARTI_بنسټ(thr, cp, خطأ);
}

/*
 * %mul
 */
منطقي د_MUL(vthread_t thr, vvp_کوډ_t)
{
      vvp_vector4_t r = thr->راوباسه_vec4();
	// Rather then pop l, use it directly from the stack. When we
	// assign to 'l', that will edit the top of the stack, which
	// replaces a pop and a pull.
      vvp_vector4_t&l = thr->peek_vec4();

      l.mul(r);
      أرجع ریښتیا;
}

/*
 * %muli <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
منطقي د_MULI(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه wid = cp->شمېره;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_سمدستي_rval (cp, r);

      l.mul(r);
      أرجع ریښتیا;
}

منطقي د_MUL_WR(vthread_t thr, vvp_کوډ_t)
{
      مزدوج r = thr->راوباسه_real();
      مزدوج l = thr->راوباسه_real();
      thr->ټېل_real(l * r);

      أرجع ریښتیا;
}

منطقي د_NAND(vthread_t thr, vvp_کوډ_t)
{
      vvp_vector4_t valr = thr->راوباسه_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      أكد(vall.size() == valr.size());

      vall &= valr;
      vall.invert();

      أرجع ریښتیا;
}

/*
 * %new/cobj <vpi_object>
 * This creates a new cobject (SystemVerilog class object) and pushes
 * it to the stack. The <vpi-object> is a __vpiHandle that is a
 * vpiClassDefn object that defines the item to be created.
 */
منطقي د_NEW_COBJ(vthread_t thr, vvp_کوډ_t cp)
{
      ثابت class_type*defn = dynamic_cast<ثابت class_type*> (cp->handle);
      أكد(defn);

      vvp_object_t tmp (جديد vvp_cobject(defn));
      thr->ټېل_object(tmp);
      أرجع ریښتیا;
}

منطقي د_NEW_DARRAY(vthread_t thr, vvp_کوډ_t cp)
{
      ثابت حرف*متن = cp->متن;
      size_t size = thr->ټکي[cp->بټ_idx[0]].w_int;
      بینښه کلمه_wid;
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
      } وإلا إذا ((1 == له_مزي_ولوله(متن, "b%u%zn", &کلمه_wid, &n)) &&
                 (n == مزي_اوږدوالی(متن))) {
	    obj = جديد vvp_darray_vec2(size, کلمه_wid);
      } وإلا إذا ((1 == له_مزي_ولوله(متن, "sb%u%zn", &کلمه_wid, &n)) &&
                 (n == مزي_اوږدوالی(متن))) {
	    obj = جديد vvp_darray_vec2(size, کلمه_wid);
      } وإلا إذا ((1 == له_مزي_ولوله(متن, "v%u%zn", &کلمه_wid, &n)) &&
                 (n == مزي_اوږدوالی(متن))) {
	    obj = جديد vvp_darray_vec4(size, کلمه_wid);
      } وإلا إذا ((1 == له_مزي_ولوله(متن, "sv%u%zn", &کلمه_wid, &n)) &&
                 (n == مزي_اوږدوالی(متن))) {
	    obj = جديد vvp_darray_vec4(size, کلمه_wid);
      } وإلا إذا (strcmp(متن,"r") == 0) {
	    obj = جديد vvp_darray_real(size);
      } وإلا إذا (strcmp(متن,"S") == 0) {
	    obj = جديد vvp_darray_string(size);
      } وإلا {
	    cerr << get_fileline()
	         << "Internal error: Unsupported dynamic array type: "
	         << متن << "." << کرښه_پای;
	    أكد(0);
      }

      thr->ټېل_object(obj);

      أرجع ریښتیا;
}

منطقي د_NOOP(vthread_t, vvp_کوډ_t)
{
      أرجع ریښتیا;
}

/*
 * %nor/r
 */
منطقي د_NORR(vthread_t thr, vvp_کوډ_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_or());

      أرجع ریښتیا;
}

/*
 * Push a null to the object stack.
 */
منطقي د_NULL(vthread_t thr, vvp_کوډ_t)
{
      vvp_object_t tmp;
      thr->ټېل_object(tmp);
      أرجع ریښتیا;
}

/*
 * %and/r
 */
منطقي د_ANDR(vthread_t thr, vvp_کوډ_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_and());

      أرجع ریښتیا;
}

/*
 * %nand/r
 */
منطقي د_NANDR(vthread_t thr, vvp_کوډ_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_and());

      أرجع ریښتیا;
}

/*
 * %or/r
 */
منطقي د_ORR(vthread_t thr, vvp_کوډ_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_or());

      أرجع ریښتیا;
}

/*
 * %xor/r
 */
منطقي د_XORR(vthread_t thr, vvp_کوډ_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_xor());

      أرجع ریښتیا;
}

/*
 * %xnor/r
 */
منطقي د_XNORR(vthread_t thr, vvp_کوډ_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_xor());

      أرجع ریښتیا;
}

/*
 * %or
 */
منطقي د_OR(vthread_t thr, vvp_کوډ_t)
{
      vvp_vector4_t valb = thr->راوباسه_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();
      vala |= valb;
      أرجع ریښتیا;
}

/*
 * %nor
 */
منطقي د_NOR(vthread_t thr, vvp_کوډ_t)
{
      vvp_vector4_t valr = thr->راوباسه_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      أكد(vall.size() == valr.size());

      vall |= valr;
      vall.invert();

      أرجع ریښتیا;
}

/*
 * %pop/obj <num>, <skip>
 */
منطقي د_راوباسه_OBJ(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه cnt = cp->بټ_idx[0];
      بینښه skip = cp->بټ_idx[1];

      thr->راوباسه_object(cnt, skip);
      أرجع ریښتیا;
}

/*
 * %pop/real <number>
 */
منطقي د_راوباسه_REAL(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه cnt = cp->شمېره;
      thr->راوباسه_real(cnt);
      أرجع ریښتیا;
}

/*
 *  %pop/str <number>
 */
منطقي د_راوباسه_STR(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه cnt = cp->شمېره;
      thr->راوباسه_str(cnt);
      أرجع ریښتیا;
}

/*
 *  %pop/vec4 <number>
 */
منطقي د_راوباسه_VEC4(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه cnt = cp->شمېره;
      thr->راوباسه_vec4(cnt);
      أرجع ریښتیا;
}

/*
 * %pow
 * %pow/s
 */
ساكن منطقي د_POW_بنسټ(vthread_t thr, منطقي signed_flag)
{
      vvp_vector4_t valb = thr->راوباسه_vec4();
      vvp_vector4_t vala = thr->راوباسه_vec4();

      بینښه wid = vala.size();

      vvp_vector2_t xv2 = vvp_vector2_t(vala, ریښتیا);
      vvp_vector2_t yv2 = vvp_vector2_t(valb, ریښتیا);


        /* If we have an X or Z in the arguments return X. */
      إذا (xv2.is_NaN() || yv2.is_NaN()) {
	    vvp_vector4_t tmp (wid, BIT4_X);
	    thr->ټېل_vec4(tmp);
	    أرجع ریښتیا;
      }

	// Is the exponent negative? If so, table 5-6 in IEEE1364-2005
	// defines what value is returned.
      إذا (signed_flag && yv2.value(yv2.size()-1)) {
	    صحيح a_val;
	    vvp_bit4_t pad = BIT4_0, lsb = BIT4_0;
	    إذا (vector2_منزل_ارزښت(xv2, a_val, ریښتیا)) {
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
	    tmp.سېټ_بټ(0, lsb);
	    thr->ټېل_vec4(tmp);
	    أرجع ریښتیا;
      }

      vvp_vector2_t پایله = توان(xv2, yv2);

        /* Copy only what we need of the result. If the result is too
	   small, zero-pad it. */
      لكل (بینښه jdx = 0;  jdx < wid;  jdx += 1) {
	    إذا (jdx >= پایله.size())
		  vala.سېټ_بټ(jdx, BIT4_0);
	    وإلا
		  vala.سېټ_بټ(jdx, پایله.value(jdx) ? BIT4_1 : BIT4_0);
      }
      thr->ټېل_vec4(vala);

      أرجع ریښتیا;
}

منطقي د_POW(vthread_t thr, vvp_کوډ_t)
{
      أرجع د_POW_بنسټ(thr, خطأ);
}

منطقي د_POW_S(vthread_t thr, vvp_کوډ_t)
{
      أرجع د_POW_بنسټ(thr, ریښتیا);
}

منطقي د_POW_WR(vthread_t thr, vvp_کوډ_t)
{
      مزدوج r = thr->راوباسه_real();
      مزدوج l = thr->راوباسه_real();
      thr->ټېل_real(توان(l,r));

      أرجع ریښتیا;
}

/*
 * %prop/obj <pid>, <idx>
 *
 * Load an object value from the cobject and push it onto the object stack.
 */
منطقي د_PROP_OBJ(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه pid = cp->شمېره;
      بینښه idx = cp->بټ_idx[0];

      إذا (idx != 0) {
	    أكد(idx < vthread_s::ټکي_شمېر);
	    idx = thr->ټکي[idx].w_uint;
      }

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();

      vvp_object_t val;
      cobj->get_object(pid, val, idx);

      thr->ټېل_object(val);

      أرجع ریښتیا;
}

ساكن فراغ get_from_obj(بینښه pid, vvp_cobject*cobj, مزدوج&val)
{
      val = cobj->get_real(pid);
}

ساكن فراغ get_from_obj(بینښه pid, vvp_cobject*cobj, string&val)
{
      val = cobj->get_string(pid);
}

ساكن فراغ get_from_obj(بینښه pid, vvp_cobject*cobj, vvp_vector4_t&val)
{
      cobj->get_vec4(pid, val);
}

کينډۍ <ډول_نوم ELEM>
ساكن منطقي prop(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه pid = cp->شمېره;

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      أكد(cobj);

      ELEM val;
      get_from_obj(pid, cobj, val);
      vthread_ټېل(thr, val);

      أرجع ریښتیا;
}

/*
 * %prop/r <pid>
 *
 * Load a real value from the cobject and push it onto the real value
 * stack.
 */
منطقي د_PROP_R(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع prop<مزدوج>(thr, cp);
}

/*
 * %prop/str <pid>
 *
 * Load a string value from the cobject and push it onto the real value
 * stack.
 */
منطقي د_PROP_STR(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع prop<string>(thr, cp);
}

/*
 * %prop/v <pid>
 *
 * Load a property <pid> from the cobject on the top of the stack into
 * the vector space at <base>.
 */
منطقي د_PROP_V(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع prop<vvp_vector4_t>(thr, cp);
}

منطقي د_PUSHI_REAL(vthread_t thr, vvp_کوډ_t cp)
{
      مزدوج mant = cp->بټ_idx[0];
      uint32_t imant = cp->بټ_idx[0];
      صحيح توانیز_تابع = cp->بټ_idx[1];

	// Detect +infinity
      إذا (توانیز_تابع==0x3fff && imant==0) {
	    thr->ټېل_real(INFINITY);
	    أرجع ریښتیا;
      }
	// Detect -infinity
      إذا (توانیز_تابع==0x7fff && imant==0) {
	    thr->ټېل_real(-INFINITY);
	    أرجع ریښتیا;
      }
	// Detect NaN
      إذا (توانیز_تابع==0x3fff) {
	    thr->ټېل_real(nan(""));
	    أرجع ریښتیا;
      }

      مزدوج sign = (توانیز_تابع & 0x4000)? -1.0 : 1.0;

      توانیز_تابع &= 0x1fff;

      mant = sign * ldexp(mant, توانیز_تابع - 0x1000);
      thr->ټېل_real(mant);
      أرجع ریښتیا;
}

منطقي د_PUSHI_STR(vthread_t thr, vvp_کوډ_t cp)
{
      ثابت حرف*متن = cp->متن;
      thr->ټېل_str(filter_string(متن));
      أرجع ریښتیا;
}

/*
 * %pushi/vec4 <vala>, <valb>, <wid>
 */
منطقي د_PUSHI_VEC4(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه wid  = cp->شمېره;

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t val (wid, BIT4_0);
      get_سمدستي_rval (cp, val);

      thr->ټېل_vec4(val);

      أرجع ریښتیا;
}

/*
 * %pushv/str
 *   Pops a vec4 value, and pushes a string.
 */
منطقي د_PUSHV_STR(vthread_t thr, vvp_کوډ_t)
{
      vvp_vector4_t vec = thr->راوباسه_vec4();

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

      thr->ټېل_str(val);

      أرجع ریښتیا;
}

/*
 * %putc/str/vec4 <var>, <mux>
 */
منطقي د_PUTC_STR_VEC4(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه muxr = cp->بټ_idx[0];
      int32_t mux = muxr? thr->ټکي[muxr].w_int : 0;

      vvp_vector4_t val = thr->راوباسه_vec4();
      أكد(val.size() == 8);

      إذا (mux < 0)
	    أرجع ریښتیا;

	/* Get the existing value of the string. If we find that the
	   index is too big for the string, then give up. */
      vvp_net_t*net = cp->net;
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (net->fun);
      أكد(fun);

      string tmp = fun->get_string();
      إذا (tmp.size() <= (size_t)mux)
	    أرجع ریښتیا;

      حرف val_str = 0;
      لكل (size_t idx = 0 ; idx < 8 ; idx += 1) {
	    إذا (val.value(idx)==BIT4_1)
		  val_str |= 1<<idx;
      }

	// It is a quirk of the Verilog standard that putc(..., 'h00)
	// has no effect. Test for that case here.
      إذا (val_str == 0)
	    أرجع ریښتیا;

      tmp[mux] = val_str;

      vvp_send_string(vvp_net_ptr_t(cp->net, 0), tmp, thr->wt_context);
      أرجع ریښتیا;
}

کينډۍ <ډول_نوم ELEM, صنف QTYPE>
ساكن منطقي qinsert(vthread_t thr, vvp_کوډ_t cp, بینښه wid=0)
{
      int64_t idx = thr->ټکي[3].w_int;
      ELEM value;
      vvp_net_t*net = cp->net;
      راوباسه_ارزښت(thr, value, wid); // Pop the value to store.

      vvp_کتار*کتار = get_کتار_object<QTYPE>(thr, net);
      أكد(کتار);
      إذا (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at a negative "
	         << get_کتار_type(value)
	         << " index (" << idx << "). ";
	    print_کتار_ارزښت(value);
	    cerr << " was not added." << کرښه_پای;
      } وإلا إذا (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at an undefined "
	         << get_کتار_type(value) << " index. ";
	    print_کتار_ارزښت(value);
	    cerr << " was not added." << کرښه_پای;
      } وإلا {
	    بینښه max_اندازه = thr->ټکي[cp->بټ_idx[0]].w_int;
	    کتار->insert(idx, value, max_اندازه);
      }
      أرجع ریښتیا;
}

/*
 * %qinsert/real <var-label>
 */
منطقي د_QINSERT_REAL(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع qinsert<مزدوج, vvp_کتار_real>(thr, cp);
}

/*
 * %qinsert/str <var-label>
 */
منطقي د_QINSERT_STR(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع qinsert<string, vvp_کتار_string>(thr, cp);
}

/*
 * %qinsert/v <var-label>
 */
منطقي د_QINSERT_V(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع qinsert<vvp_vector4_t, vvp_کتار_vec4>(thr, cp, cp->بټ_idx[1]);
}

/*
 * Helper functions used in the queue pop templates
 */
مضمن فراغ ټېل_ارزښت(vthread_t thr, مزدوج value, بینښه)
{
      thr->ټېل_real(value);
}

مضمن فراغ ټېل_ارزښت(vthread_t thr, ثابت string&value, بینښه)
{
      thr->ټېل_str(value);
}

مضمن فراغ ټېل_ارزښت(vthread_t thr, ثابت vvp_vector4_t&value, بینښه wid)
{
      أكد(wid == value.size());
      thr->ټېل_vec4(value);
}

کينډۍ <ډول_نوم ELEM, صنف QTYPE>
ساكن منطقي q_راوباسه(vthread_t thr, vvp_کوډ_t cp,
                  فراغ (*get_val_func)(vvp_کتار*, ELEM&),
                  ثابت حرف*loc, بینښه wid)
{
      vvp_net_t*net = cp->net;

      vvp_کتار*کتار = get_کتار_object<QTYPE>(thr, net);
      أكد(کتار);

      size_t size = کتار->get_اندازه();

      ELEM value;
      إذا (size) {
	    get_val_func(کتار, value);
      } وإلا {
	    dq_default(value, wid);
	    cerr << thr->get_fileline()
	         << "Warning: pop_" << loc << "() on empty "
	         << get_کتار_type(value) << "." << کرښه_پای;
      }

      ټېل_ارزښت(thr, value, wid);
      أرجع ریښتیا;
}

کينډۍ <ډول_نوم ELEM>
ساكن فراغ get_شا_ارزښت(vvp_کتار*کتار, ELEM&value)
{
      کتار->get_کلمه(کتار->get_اندازه()-1, value);
      کتار->pop_back();
}

کينډۍ <ډول_نوم ELEM, صنف QTYPE>
ساكن منطقي qpop_b(vthread_t thr, vvp_کوډ_t cp, بینښه wid=0)
{
      أرجع q_راوباسه<ELEM, QTYPE>(thr, cp, get_شا_ارزښت<ELEM>, "back", wid);
}

/*
 * %qpop/b/real <var-label>
 */
منطقي د_QPOP_B_REAL(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع qpop_b<مزدوج, vvp_کتار_real>(thr, cp);
}

/*
 * %qpop/b/str <var-label>
 */
منطقي د_QPOP_B_STR(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع qpop_b<string, vvp_کتار_string>(thr, cp);
}

/*
 * %qpop/b/v <var-label>
 */
منطقي د_QPOP_B_V(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع qpop_b<vvp_vector4_t, vvp_کتار_vec4>(thr, cp, cp->بټ_idx[0]);
}

کينډۍ <ډول_نوم ELEM>
ساكن فراغ get_front_ارزښت(vvp_کتار*کتار, ELEM&value)
{
      کتار->get_کلمه(0, value);
      کتار->pop_front();
}

کينډۍ <ډول_نوم ELEM, صنف QTYPE>
ساكن منطقي qpop_f(vthread_t thr, vvp_کوډ_t cp, بینښه wid=0)
{
      أرجع q_راوباسه<ELEM, QTYPE>(thr, cp, get_front_ارزښت<ELEM>, "front", wid);
}


/*
 * %qpop/f/real <var-label>
 */
منطقي د_QPOP_F_REAL(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع qpop_f<مزدوج, vvp_کتار_real>(thr, cp);
}

/*
 * %qpop/f/str <var-label>
 */
منطقي د_QPOP_F_STR(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع qpop_f<string, vvp_کتار_string>(thr, cp);
}

/*
 * %qpop/f/v <var-label>
 */
منطقي د_QPOP_F_V(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع qpop_f<vvp_vector4_t, vvp_کتار_vec4>(thr, cp, cp->بټ_idx[0]);
}

/*
 * These implement the %release/net and %release/reg instructions. The
 * %release/net instruction applies to a net kind of functor by
 * sending the release/net command to the command port. (See vvp_net.h
 * for details.) The %release/reg instruction is the same, but sends
 * the release/reg command instead. These are very similar to the
 * %deassign instruction.
 */
ساكن منطقي do_release_vec(vvp_کوډ_t cp, منطقي net_flag)
{
      vvp_net_t*net = cp->net;
      بینښه بنسټ  = cp->بټ_idx[0];
      بینښه width = cp->بټ_idx[1];

      أكد(net->fil);

      إذا (بنسټ >= net->fil->filter_اندازه()) أرجع ریښتیا;
      إذا (بنسټ+width > net->fil->filter_اندازه())
	    width = net->fil->filter_اندازه() - بنسټ;

      منطقي ډک_sig = بنسټ == 0 && width == net->fil->filter_اندازه();

	// XXXX Can't really do this if this is a partial release?
      net->fil->force_unlink();

	/* Do we release all or part of the net? */
      vvp_net_ptr_t ptr (net, 0);
      إذا (ډک_sig) {
	    net->fil->release(ptr, net_flag);
      } وإلا {
	    net->fil->release_pv(ptr, بنسټ, width, net_flag);
      }
      net->fun->force_flag(خطأ);

      أرجع ریښتیا;
}

منطقي د_RELEASE_NET(vthread_t, vvp_کوډ_t cp)
{
      أرجع do_release_vec(cp, ریښتیا);
}


منطقي د_RELEASE_REG(vthread_t, vvp_کوډ_t cp)
{
      أرجع do_release_vec(cp, خطأ);
}

/* The type is 1 for registers and 0 for everything else. */
منطقي د_RELEASE_WR(vthread_t, vvp_کوډ_t cp)
{
      vvp_net_t*net = cp->net;
      بینښه type  = cp->بټ_idx[0];

      أكد(net->fil);
      net->fil->force_unlink();

	// Send a command to this signal to unforce itself.
      vvp_net_ptr_t ptr (net, 0);
      net->fil->release(ptr, type==0);
      أرجع ریښتیا;
}

منطقي د_REPLICATE(vthread_t thr, vvp_کوډ_t cp)
{
      صحيح rept = cp->شمېره;
      vvp_vector4_t val = thr->راوباسه_vec4();
      vvp_vector4_t res (val.size() * rept, BIT4_X);

      لكل (صحيح idx = 0 ; idx < rept ; idx += 1) {
	    res.سېټ_vec(idx * val.size(), val);
      }

      thr->ټېل_vec4(res);

      أرجع ریښتیا;
}

ساكن فراغ poke_val(vthread_t fun_thr, بینښه ژوروالی, مزدوج val)
{
      fun_thr->پلار->poke_real(ژوروالی, val);
}

ساكن فراغ poke_val(vthread_t fun_thr, بینښه ژوروالی, ثابت string&val)
{
      fun_thr->پلار->poke_str(ژوروالی, val);
}

ساكن size_t get_max(vthread_t fun_thr, مزدوج&)
{
      أرجع fun_thr->دلیلونه_real.size();
}

ساكن size_t get_max(vthread_t fun_thr, string&)
{
      أرجع fun_thr->دلیلونه_str.size();
}

ساكن size_t get_max(vthread_t fun_thr, vvp_vector4_t&)
{
      أرجع fun_thr->دلیلونه_vec4.size();
}

ساكن بینښه get_ژوروالی(vthread_t fun_thr, size_t شاخص, مزدوج&)
{
      أرجع fun_thr->دلیلونه_real[شاخص];
}

ساكن بینښه get_ژوروالی(vthread_t fun_thr, size_t شاخص, string&)
{
      أرجع fun_thr->دلیلونه_str[شاخص];
}

ساكن بینښه get_ژوروالی(vthread_t fun_thr, size_t شاخص, vvp_vector4_t&)
{
      أرجع fun_thr->دلیلونه_vec4[شاخص];
}

ساكن vthread_t get_func(vthread_t thr)
{
      vthread_t fun_thr = thr;

      طالما (fun_thr->پلار_scope->get_type_کوډ() != vpiFunction) {
	    أكد(fun_thr->پلار);
	    fun_thr = fun_thr->پلار;
      }

      أرجع fun_thr;
}

کينډۍ <ډول_نوم ELEM>
ساكن منطقي ret(vthread_t thr, vvp_کوډ_t cp)
{
      size_t شاخص = cp->شمېره;
      ELEM val;
      راوباسه_ارزښت(thr, val, 0);

      vthread_t fun_thr = get_func(thr);
      أكد(شاخص < get_max(fun_thr, val));

      بینښه ژوروالی = get_ژوروالی(fun_thr, شاخص, val);
	// Use the depth to put the value into the stack of
	// the parent thread.
      poke_val(fun_thr, ژوروالی, val);
      أرجع ریښتیا;
}

/*
 * %ret/real <index>
 */
منطقي د_RET_REAL(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع ret<مزدوج>(thr, cp);
}

/*
 * %ret/str <index>
 */
منطقي د_RET_STR(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع ret<string>(thr, cp);
}

/*
 * %ret/vec4 <index>, <offset>, <wid>
 */
منطقي د_RET_VEC4(vthread_t thr, vvp_کوډ_t cp)
{
      size_t شاخص = cp->شمېره;
      بینښه off_شاخص = cp->بټ_idx[0];
      بینښه صحيح wid = cp->بټ_idx[1];
      vvp_vector4_t&val = thr->peek_vec4();

      vthread_t fun_thr = get_func(thr);
      أكد(شاخص < get_max(fun_thr, val));
      أكد(val.size() == wid);
      بینښه ژوروالی = get_ژوروالی(fun_thr, شاخص, val);

      int64_t off = off_شاخص ? thr->ټکي[off_شاخص].w_int : 0;
      بینښه صحيح sig_ارزښت_اندازه = fun_thr->پلار->peek_vec4(ژوروالی).size();

      إذا (off_شاخص!=0 && thr->flags[4] == BIT4_1) {
	    thr->راوباسه_vec4(1);
	    أرجع ریښتیا;
      }

      إذا (!resize_rval_vec(val, off, sig_ارزښت_اندازه)) {
	    thr->راوباسه_vec4(1);
	    أرجع ریښتیا;
      }

      إذا (off == 0 && val.size() == sig_ارزښت_اندازه) {
	    fun_thr->پلار->poke_vec4(ژوروالی, val);
      } وإلا {
	    vvp_vector4_t tmp_dst = fun_thr->پلار->peek_vec4(ژوروالی);
	    tmp_dst.سېټ_vec(off, val);
	    fun_thr->پلار->poke_vec4(ژوروالی, tmp_dst);
      }

      thr->راوباسه_vec4(1);
      أرجع ریښتیا;
}

ساكن فراغ ټېل_from_پلار(vthread_t thr, vthread_t fun_thr, بینښه ژوروالی, مزدوج&)
{
      thr->ټېل_real(fun_thr->پلار->peek_real(ژوروالی));
}

ساكن فراغ ټېل_from_پلار(vthread_t thr, vthread_t fun_thr, بینښه ژوروالی, string&)
{
      thr->ټېل_str(fun_thr->پلار->peek_str(ژوروالی));
}

ساكن فراغ ټېل_from_پلار(vthread_t thr, vthread_t fun_thr, بینښه ژوروالی, vvp_vector4_t&)
{
      thr->ټېل_vec4(fun_thr->پلار->peek_vec4(ژوروالی));
}

کينډۍ <ډول_نوم ELEM>
ساكن منطقي retload(vthread_t thr, vvp_کوډ_t cp)
{
      size_t شاخص = cp->شمېره;
      ELEM type;

      vthread_t fun_thr = get_func(thr);
      أكد(شاخص < get_max(fun_thr, type));

      بینښه ژوروالی = get_ژوروالی(fun_thr, شاخص, type);
	// Use the depth to extract the values from the stack
	// of the parent thread.
      ټېل_from_پلار(thr, fun_thr, ژوروالی, type);
      أرجع ریښتیا;
}

/*
 * %retload/real <index>
 */
منطقي د_RETLOAD_REAL(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع retload<مزدوج>(thr, cp);
}

/*
 * %retload/str <index>
 */
منطقي د_RETLOAD_STR(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع retload<string>(thr, cp);
}

/*
 * %retload/vec4 <index>
 */
منطقي د_RETLOAD_VEC4(vthread_t thr, vvp_کوډ_t cp)
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
منطقي د_SCOPY(vthread_t thr, vvp_کوډ_t)
{
      vvp_object_t tmp;
      thr->راوباسه_object(tmp);

      vvp_object_t&dest = thr->peek_object();
        // If it is null there is nothing to copy
      إذا (!tmp.test_nil())
	    dest.shallow_copy(tmp);

      أرجع ریښتیا;
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

کينډۍ <ډول_نوم ELEM>
ساكن منطقي سېټ_dar_obj(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه adr = thr->ټکي[cp->شمېره].w_int;

      ELEM value;
      thread_peek(thr, value);

      vvp_object_t&سر = thr->peek_object();
      vvp_darray*darray = سر.peek<vvp_darray>();
      أكد(darray);

      darray->سېټ_کلمه(adr, value);
      أرجع ریښتیا;
}

/*
 * %set/dar/obj/real <index>
 */
منطقي د_سېټ_DAR_OBJ_REAL(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع سېټ_dar_obj<مزدوج>(thr, cp);
}

/*
 * %set/dar/obj/str <index>
 */
منطقي د_سېټ_DAR_OBJ_STR(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع سېټ_dar_obj<string>(thr, cp);
}

/*
 * %set/dar/obj/vec4 <index>
 */
منطقي د_سېټ_DAR_OBJ_VEC4(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع سېټ_dar_obj<vvp_vector4_t>(thr, cp);
}

/*
 * %shiftl <idx>
 *
 * Pop the operand, then push the result.
 */
منطقي د_SHIFTL(vthread_t thr, vvp_کوډ_t cp)
{
      صحيح use_شاخص = cp->شمېره;
      uint64_t ښويدنه = thr->ټکي[use_شاخص].w_uint;

      vvp_vector4_t&val = thr->peek_vec4();
      بینښه wid  = val.size();

      إذا (thr->flags[4] == BIT4_1) {
	      // The result is 'bx if the shift amount is undefined
	    val = vvp_vector4_t(wid, BIT4_X);

      } وإلا إذا (thr->flags[4] == BIT4_X || ښويدنه >= wid) {
	      // Shift is so big that all value is shifted out. Write
	      // a constant 0 result.
	    val = vvp_vector4_t(wid, BIT4_0);

      } وإلا إذا (ښويدنه > 0) {
	    vvp_vector4_t blk = val.subvalue(0, wid-ښويدنه);
	    vvp_vector4_t tmp (ښويدنه, BIT4_0);
	    val.سېټ_vec(0, tmp);
	    val.سېټ_vec(ښويدنه, blk);
      }

      أرجع ریښتیا;
}

/*
 * %shiftr <idx>
 * This is an unsigned right shift. The <idx> is a number that selects
 * the index register with the amount of the shift. This instruction
 * checks flag bit 4, which will be true if the shift is invalid.
 */
منطقي د_SHIFTR(vthread_t thr, vvp_کوډ_t cp)
{
      صحيح use_شاخص = cp->شمېره;
      uint64_t ښويدنه = thr->ټکي[use_شاخص].w_uint;

      vvp_vector4_t val = thr->راوباسه_vec4();
      بینښه wid  = val.size();

      إذا (thr->flags[4] == BIT4_1) {
	    val = vvp_vector4_t(wid, BIT4_X);

      } وإلا إذا (thr->flags[4] == BIT4_X || ښويدنه > wid) {
	    val = vvp_vector4_t(wid, BIT4_0);

      } وإلا إذا (ښويدنه > 0) {
	    vvp_vector4_t blk = val.subvalue(ښويدنه, wid-ښويدنه);
	    vvp_vector4_t tmp (ښويدنه, BIT4_0);
	    val.سېټ_vec(0, blk);
	    val.سېټ_vec(wid-ښويدنه, tmp);
      }

      thr->ټېل_vec4(val);
      أرجع ریښتیا;
}

/*
 *  %shiftr/s <wid>
 */
منطقي د_SHIFTR_S(vthread_t thr, vvp_کوډ_t cp)
{
      صحيح use_شاخص = cp->شمېره;
      uint64_t ښويدنه = thr->ټکي[use_شاخص].w_uint;

      vvp_vector4_t val = thr->راوباسه_vec4();
      بینښه wid  = val.size();

      vvp_bit4_t sign_بټ = val.value(val.size()-1);

      إذا (thr->flags[4] == BIT4_1) {
	    val = vvp_vector4_t(wid, BIT4_X);

      } وإلا إذا (thr->flags[4] == BIT4_X || ښويدنه > wid) {
	    val = vvp_vector4_t(wid, sign_بټ);

      } وإلا إذا (ښويدنه > 0) {
	    vvp_vector4_t blk = val.subvalue(ښويدنه, wid-ښويدنه);
	    vvp_vector4_t tmp (ښويدنه, sign_بټ);
	    val.سېټ_vec(0, blk);
	    val.سېټ_vec(wid-ښويدنه, tmp);
      }

      thr->ټېل_vec4(val);
      أرجع ریښتیا;
}

/*
 * %split/vec4 <wid>
 *   Pop 1 value,
 *   Take <wid> bits from the lsb,
 *   Push the remaining msb,
 *   Push the lsb.
 */
منطقي د_وېشل_VEC4(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه lsb_wid = cp->شمېره;

      vvp_vector4_t&val = thr->peek_vec4();
      أكد(lsb_wid < val.size());

      vvp_vector4_t lsb = val.subvalue(0, lsb_wid);
      val = val.subvalue(lsb_wid, val.size()-lsb_wid);

      thr->ټېل_vec4(lsb);
      أرجع ریښتیا;
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
مضمن ساكن فراغ dar_راوباسه_ارزښت(vthread_t thr, مزدوج&value)
{
      value = thr->راوباسه_real();
}

مضمن ساكن فراغ dar_راوباسه_ارزښت(vthread_t thr, string&value)
{
      value = thr->راوباسه_str();
}

مضمن ساكن فراغ dar_راوباسه_ارزښت(vthread_t thr, vvp_vector4_t&value)
{
      value = thr->راوباسه_vec4();
}

کينډۍ <ډول_نوم ELEM>
ساكن منطقي store_dar(vthread_t thr, vvp_کوډ_t cp)
{
      int64_t adr = thr->ټکي[3].w_int;
      ELEM value;
	// FIXME: Can we get the size of the underlying array element
	//        and then use the normal pop_value?
      dar_راوباسه_ارزښت(thr, value);

      vvp_net_t*net = cp->net;
      أكد(net);

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      أكد(obj);

      vvp_darray*darray = obj->get_object().peek<vvp_darray>();

      إذا (adr < 0)
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to a negative " << get_darray_type(value)
	         << " index (" << adr << ")." << کرښه_پای;
      وإلا إذا (thr->flags[4] != BIT4_0)
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to an undefined " << get_darray_type(value)
	         << " index." << کرښه_پای;
      وإلا إذا (darray)
	    darray->سېټ_کلمه(adr, value);
      وإلا
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to an undefined " << get_darray_type(value)
	         << "." << کرښه_پای;

      أرجع ریښتیا;
}

/*
 * %store/dar/real <var>
 */
منطقي د_STORE_DAR_R(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع store_dar<مزدوج>(thr, cp);
}

/*
 * %store/dar/str <var>
 */
منطقي د_STORE_DAR_STR(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع store_dar<string>(thr, cp);
}

/*
 * %store/dar/vec4 <var>
 */
منطقي د_STORE_DAR_VEC4(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع store_dar<vvp_vector4_t>(thr, cp);
}

منطقي د_STORE_OBJ(vthread_t thr, vvp_کوډ_t cp)
{
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);

      vvp_object_t val;
      thr->راوباسه_object(val);

      vvp_send_object(ptr, val, thr->wt_context);

      أرجع ریښتیا;
}

/*
 * %store/obja <array-label> <index>
 */
منطقي د_STORE_OBJA(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه idx = cp->بټ_idx[0];
      بینښه adr = thr->ټکي[idx].w_int;

      vvp_object_t val;
      thr->راوباسه_object(val);

      cp->array->سېټ_کلمه(adr, val);

      أرجع ریښتیا;
}


/*
 * %store/prop/obj <pid>, <idx>
 *
 * Pop an object value from the object stack, and store the value into
 * the property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
منطقي د_STORE_PROP_OBJ(vthread_t thr, vvp_کوډ_t cp)
{
      size_t pid = cp->شمېره;
      بینښه idx = cp->بټ_idx[0];

      إذا (idx != 0) {
	    أكد(idx < vthread_s::ټکي_شمېر);
	    idx = thr->ټکي[idx].w_uint;
      }

      vvp_object_t val;
      thr->راوباسه_object(val);

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      أكد(cobj);

      cobj->سېټ_object(pid, val, idx);

      أرجع ریښتیا;
}

ساكن فراغ راوباسه_prop_val(vthread_t thr, مزدوج&val, بینښه)
{
      val = thr->راوباسه_real();
}

ساكن فراغ راوباسه_prop_val(vthread_t thr, string&val, بینښه)
{
      val = thr->راوباسه_str();
}

ساكن فراغ راوباسه_prop_val(vthread_t thr, vvp_vector4_t&val, بینښه wid)
{
      val = thr->راوباسه_vec4();
      أكد(val.size() >= wid);
      val.resize(wid);
}

ساكن فراغ سېټ_val(vvp_cobject*cobj, size_t pid, ثابت مزدوج&val)
{
      cobj->سېټ_real(pid, val);
}

ساكن فراغ سېټ_val(vvp_cobject*cobj, size_t pid, ثابت string&val)
{
      cobj->سېټ_string(pid, val);
}

ساكن فراغ سېټ_val(vvp_cobject*cobj, size_t pid, ثابت vvp_vector4_t&val)
{
      cobj->سېټ_vec4(pid, val);
}

کينډۍ <ډول_نوم ELEM>
ساكن منطقي store_prop(vthread_t thr, vvp_کوډ_t cp, بینښه wid=0)
{
      size_t pid = cp->شمېره;
      ELEM val;
      راوباسه_prop_val(thr, val, wid); // Pop the value to store.

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      أكد(cobj);

      سېټ_val(cobj, pid, val);

      أرجع ریښتیا;
}

/*
 * %store/prop/r <id>
 *
 * Pop a real value from the real stack, and store the value into the
 * property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
منطقي د_STORE_PROP_R(vthread_t thr, vvp_کوډ_t cp)
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
منطقي د_STORE_PROP_STR(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع store_prop<string>(thr, cp);
}

/*
 * %store/prop/v <pid>, <wid>
 *
 * Store vector value into property <id> of cobject in the top of the
 * stack. Do NOT pop the object stack.
 */
منطقي د_STORE_PROP_V(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع store_prop<vvp_vector4_t>(thr, cp, cp->بټ_idx[0]);
}

کينډۍ <صنف QTYPE>
ساكن QTYPE* get_کتار_prop(vvp_cobject*cobj, size_t pid)
{
      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      QTYPE* کتار = qobj.peek<QTYPE>();
      إذا (کتار == 0) {
	    کتار = جديد QTYPE;
	    vvp_object_t val(کتار);
	    cobj->سېټ_object(pid, val, 0);
      }
      أرجع کتار;
}

کينډۍ <ډول_نوم ELEM, صنف QTYPE>
ساكن منطقي store_prop_qb(vthread_t thr, vvp_کوډ_t cp, بینښه wid)
{
      size_t pid = cp->شمېره;
      ELEM value;
      بینښه max_اندازه = thr->ټکي[cp->بټ_idx[0]].w_uint;
      راوباسه_ارزښت(thr, value, wid);

      vvp_object_t& سر = thr->peek_object();
      vvp_cobject*cobj = سر.peek<vvp_cobject>();
      أكد(cobj);

      QTYPE* کتار = get_کتار_prop<QTYPE>(cobj, pid);
      أكد(کتار);
      کتار->push_back(value, max_اندازه);
      أرجع ریښتیا;
}

کينډۍ <ډول_نوم ELEM, صنف QTYPE>
ساكن منطقي store_prop_qf(vthread_t thr, vvp_کوډ_t cp, بینښه wid)
{
      size_t pid = cp->شمېره;
      ELEM value;
      بینښه max_اندازه = thr->ټکي[cp->بټ_idx[0]].w_uint;
      راوباسه_ارزښت(thr, value, wid);

      vvp_object_t& سر = thr->peek_object();
      vvp_cobject*cobj = سر.peek<vvp_cobject>();
      أكد(cobj);

      QTYPE* کتار = get_کتار_prop<QTYPE>(cobj, pid);
      أكد(کتار);
      کتار->push_front(value, max_اندازه);
      أرجع ریښتیا;
}

منطقي د_STORE_PROP_QB_R(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع store_prop_qb<مزدوج, vvp_کتار_real>(thr, cp, 0);
}

منطقي د_STORE_PROP_QB_STR(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع store_prop_qb<string, vvp_کتار_string>(thr, cp, 0);
}

منطقي د_STORE_PROP_QB_V(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع store_prop_qb<vvp_vector4_t, vvp_کتار_vec4>(thr, cp, cp->بټ_idx[1]);
}

منطقي د_STORE_PROP_QF_R(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع store_prop_qf<مزدوج, vvp_کتار_real>(thr, cp, 0);
}

منطقي د_STORE_PROP_QF_STR(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع store_prop_qf<string, vvp_کتار_string>(thr, cp, 0);
}

منطقي د_STORE_PROP_QF_V(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع store_prop_qf<vvp_vector4_t, vvp_کتار_vec4>(thr, cp, cp->بټ_idx[1]);
}

کينډۍ <ډول_نوم ELEM, صنف QTYPE>
ساكن منطقي qinsert_prop(vthread_t thr, vvp_کوډ_t cp, بینښه wid)
{
      int64_t idx = thr->ټکي[3].w_int;
      ELEM value;
      size_t pid = cp->شمېره;
      راوباسه_ارزښت(thr, value, wid);

      vvp_object_t& سر = thr->peek_object();
      vvp_cobject*cobj = سر.peek<vvp_cobject>();
      أكد(cobj);

      QTYPE* کتار = get_کتار_prop<QTYPE>(cobj, pid);
      أكد(کتار);
      إذا (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at a negative "
	         << get_کتار_type(value)
	         << " index (" << idx << "). ";
	    print_کتار_ارزښت(value);
	    cerr << " was not added." << کرښه_پای;
      } وإلا إذا (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at an undefined "
	         << get_کتار_type(value) << " index. ";
	    print_کتار_ارزښت(value);
	    cerr << " was not added." << کرښه_پای;
      } وإلا {
	    بینښه max_اندازه = thr->ټکي[cp->بټ_idx[0]].w_int;
	    کتار->insert(idx, value, max_اندازه);
      }
      أرجع ریښتیا;
}

منطقي د_QINSERT_PROP_R(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع qinsert_prop<مزدوج, vvp_کتار_real>(thr, cp, 0);
}

منطقي د_QINSERT_PROP_STR(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع qinsert_prop<string, vvp_کتار_string>(thr, cp, 0);
}

منطقي د_QINSERT_PROP_V(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع qinsert_prop<vvp_vector4_t, vvp_کتار_vec4>(thr, cp, cp->بټ_idx[1]);
}

کينډۍ <ډول_نوم ELEM, صنف QTYPE>
ساكن منطقي q_راوباسه_prop(vthread_t thr, vvp_کوډ_t cp,
                       فراغ (*get_val_func)(vvp_کتار*, ELEM&),
                       ثابت حرف*loc, بینښه wid)
{
      size_t pid = cp->شمېره;

      vvp_object_t& سر = thr->peek_object();
      vvp_cobject*cobj = سر.peek<vvp_cobject>();
      أكد(cobj);

      QTYPE* کتار = get_کتار_prop<QTYPE>(cobj, pid);
      أكد(کتار);

      size_t size = کتار->get_اندازه();

      ELEM value;
      إذا (size) {
	    get_val_func(کتار, value);
      } وإلا {
	    dq_default(value, wid);
	    cerr << thr->get_fileline()
	         << "Warning: pop_" << loc << "() on empty "
	         << get_کتار_type(value) << "." << کرښه_پای;
      }

      ټېل_ارزښت(thr, value, wid);
      أرجع ریښتیا;
}

کينډۍ <ډول_نوم ELEM, صنف QTYPE>
ساكن منطقي qpop_b_prop(vthread_t thr, vvp_کوډ_t cp, بینښه wid)
{
      أرجع q_راوباسه_prop<ELEM, QTYPE>(thr, cp, get_شا_ارزښت<ELEM>, "back", wid);
}

کينډۍ <ډول_نوم ELEM, صنف QTYPE>
ساكن منطقي qpop_f_prop(vthread_t thr, vvp_کوډ_t cp, بینښه wid)
{
      أرجع q_راوباسه_prop<ELEM, QTYPE>(thr, cp, get_front_ارزښت<ELEM>, "front", wid);
}

منطقي د_QPOP_PROP_B_REAL(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع qpop_b_prop<مزدوج, vvp_کتار_real>(thr, cp, 0);
}

منطقي د_QPOP_PROP_B_STR(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع qpop_b_prop<string, vvp_کتار_string>(thr, cp, 0);
}

منطقي د_QPOP_PROP_B_V(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع qpop_b_prop<vvp_vector4_t, vvp_کتار_vec4>(thr, cp, cp->بټ_idx[0]);
}

منطقي د_QPOP_PROP_F_REAL(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع qpop_f_prop<مزدوج, vvp_کتار_real>(thr, cp, 0);
}

منطقي د_QPOP_PROP_F_STR(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع qpop_f_prop<string, vvp_کتار_string>(thr, cp, 0);
}

منطقي د_QPOP_PROP_F_V(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع qpop_f_prop<vvp_vector4_t, vvp_کتار_vec4>(thr, cp, cp->بټ_idx[0]);
}

ساكن vvp_vector4_t کتار_unique_ulong_منزل_vec4(بینښه اوږد ul, بینښه بټونه)
{
      vvp_vector4_t val(بټونه, BIT4_0);
      لكل (بینښه idx = 0; idx < بټونه && idx < 8 * حجم(بینښه اوږد); idx += 1) {
	    إذا (ul & 1UL) {
		  val.سېټ_بټ(idx, BIT4_1);
	    } وإلا {
		  val.سېټ_بټ(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      أرجع val;
}

کينډۍ <صنف SRC> ساكن vvp_کتار_vec4* کتار_پرلپسې_unique_src(SRC* src,
								 بینښه wid,
								 منطقي as_شاخص)
{
      vvp_کتار_vec4* dst = جديد vvp_کتار_vec4();
      إذا (!src) أرجع dst;
      size_t n = src->get_اندازه();
      لكل (size_t i = 0; i < n; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_کلمه(i, vi);
	    منطقي کتل_شوی = خطأ;
	    لكل (size_t j = 0; j < i; j += 1) {
		  vvp_vector4_t vj(wid);
		  src->get_کلمه(j, vj);
		  إذا (vi.eeq(vj)) {
			کتل_شوی = ریښتیا;
			اكسر;
		  }
	    }
	    إذا (کتل_شوی) استمر;
	    إذا (as_شاخص) {
		  dst->push_back(کتار_unique_ulong_منزل_vec4((بینښه اوږد)i, 32), 0);
	    } وإلا {
		  dst->push_back(vi, 0);
	    }
      }
      أرجع dst;
}

کينډۍ <صنف SRC>
ساكن vvp_کتار_vec4* کتار_پرلپسې_پیدا_کړه_matches_src(SRC* src, بینښه wid,
						  ثابت vvp_vector4_t& cmp,
						  منطقي as_شاخص)
{
      vvp_کتار_vec4* dst = جديد vvp_کتار_vec4();
      إذا (!src) أرجع dst;
      size_t n = src->get_اندازه();
      لكل (size_t i = 0; i < n; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_کلمه(i, vi);
	    إذا (vi.eeq(cmp)) {
		  إذا (as_شاخص) {
			dst->push_back(کتار_unique_ulong_منزل_vec4((بینښه اوږد)i, 32), 0);
		  } وإلا {
			dst->push_back(vi, 0);
		  }
	    }
      }
      أرجع dst;
}

/* Array/queue locator methods return a queue with 0 or 1 elements (LRM). */
کينډۍ <صنف SRC>
ساكن vvp_کتار_vec4* کتار_پیدا_کړه_لومړی_وروستی_منزل_کتار_src(SRC* src,
							  بینښه wid,
							  ثابت vvp_vector4_t& cmp,
							  منطقي want_لومړی,
							  منطقي want_شاخص)
{
      vvp_کتار_vec4* dst = جديد vvp_کتار_vec4();
      إذا (!src || src->get_اندازه() == 0) أرجع dst;
      size_t n = src->get_اندازه();
      إذا (want_لومړی) {
	    لكل (size_t i = 0; i < n; i += 1) {
		  vvp_vector4_t vi(wid);
		  src->get_کلمه(i, vi);
		  إذا (vi.eeq(cmp)) {
			إذا (want_شاخص) {
			      dst->push_back(
				  کتار_unique_ulong_منزل_vec4((بینښه اوږد)i, 32),
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
				  کتار_unique_ulong_منزل_vec4((بینښه اوږد)i, 32),
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
کينډۍ <صنف SRC>
ساكن vvp_کتار_vec4* کتار_پرلپسې_min_max_src(SRC* src, بینښه wid, منطقي want_max)
{
      vvp_کتار_vec4* dst = جديد vvp_کتار_vec4();
      إذا (!src || src->get_اندازه() == 0) أرجع dst;

      vvp_vector4_t غوره(wid);
      src->get_کلمه(0, غوره);

      size_t n = src->get_اندازه();
      لكل (size_t i = 1 ; i < n ; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_کلمه(i, vi);

	    vvp_bit4_t ge = compare_gtge(vi, غوره, BIT4_1);
	    منطقي replace = want_max ? (ge == BIT4_1) : (ge == BIT4_0);
	    إذا (replace) غوره = vi;
      }

      dst->push_back(غوره, 0);
      أرجع dst;
}

منطقي د_کتار_پیدا_کړه_V(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_vector4_t cmp = thr->راوباسه_vec4();
      بینښه wid = cp->بټ_idx[0];
      أكد(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_کتار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_کتار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_کتار_vec4* dst;
      إذا (qsrc) dst = کتار_پرلپسې_پیدا_کړه_matches_src(qsrc, wid, cmp, خطأ);
      وإلا إذا (dsrc) dst = کتار_پرلپسې_پیدا_کړه_matches_src(dsrc, wid, cmp, خطأ);
      وإلا {
	    vvp_کتار* src_q = get_کتار_object<vvp_کتار_vec4>(thr, net);
	    vvp_کتار_vec4* src = dynamic_cast<vvp_کتار_vec4*>(src_q);
	    dst = کتار_پرلپسې_پیدا_کړه_matches_src(src, wid, cmp, خطأ);
      }
      thr->ټېل_object(vvp_object_t(dst));
      أرجع ریښتیا;
}

منطقي د_کتار_پیدا_کړه_PROP_V(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_vector4_t cmp = thr->راوباسه_vec4();
      size_t pid = cp->شمېره;
      بینښه wid = cp->بټ_idx[0];
      أكد(cmp.size() == wid);

      vvp_object_t& سر = thr->peek_object();
      vvp_cobject*cobj = سر.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_کتار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_کتار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_کتار_vec4* dst = qsrc ? کتار_پرلپسې_پیدا_کړه_matches_src(qsrc, wid, cmp, خطأ)
				 : کتار_پرلپسې_پیدا_کړه_matches_src(dsrc, wid, cmp, خطأ);
      thr->ټېل_object(vvp_object_t(dst));
      أرجع ریښتیا;
}

منطقي د_کتار_پیدا_کړه_شاخص_V(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_vector4_t cmp = thr->راوباسه_vec4();
      بینښه wid = cp->بټ_idx[0];
      أكد(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_کتار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_کتار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_کتار_vec4* dst;
      إذا (qsrc) dst = کتار_پرلپسې_پیدا_کړه_matches_src(qsrc, wid, cmp, ریښتیا);
      وإلا إذا (dsrc) dst = کتار_پرلپسې_پیدا_کړه_matches_src(dsrc, wid, cmp, ریښتیا);
      وإلا {
	    vvp_کتار* src_q = get_کتار_object<vvp_کتار_vec4>(thr, net);
	    vvp_کتار_vec4* src = dynamic_cast<vvp_کتار_vec4*>(src_q);
	    dst = کتار_پرلپسې_پیدا_کړه_matches_src(src, wid, cmp, ریښتیا);
      }
      thr->ټېل_object(vvp_object_t(dst));
      أرجع ریښتیا;
}

منطقي د_کتار_پیدا_کړه_شاخص_PROP_V(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_vector4_t cmp = thr->راوباسه_vec4();
      size_t pid = cp->شمېره;
      بینښه wid = cp->بټ_idx[0];
      أكد(cmp.size() == wid);

      vvp_object_t& سر = thr->peek_object();
      vvp_cobject*cobj = سر.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_کتار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_کتار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_کتار_vec4* dst = qsrc ? کتار_پرلپسې_پیدا_کړه_matches_src(qsrc, wid, cmp, ریښتیا)
				 : کتار_پرلپسې_پیدا_کړه_matches_src(dsrc, wid, cmp, ریښتیا);
      thr->ټېل_object(vvp_object_t(dst));
      أرجع ریښتیا;
}

منطقي د_کتار_پیدا_کړه_لومړی_V(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_vector4_t cmp = thr->راوباسه_vec4();
      بینښه wid = cp->بټ_idx[0];
      أكد(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_کتار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_کتار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_کتار_vec4* dst;
      إذا (qsrc) dst = کتار_پیدا_کړه_لومړی_وروستی_منزل_کتار_src(qsrc, wid, cmp, ریښتیا, خطأ);
      وإلا إذا (dsrc) dst = کتار_پیدا_کړه_لومړی_وروستی_منزل_کتار_src(dsrc, wid, cmp, ریښتیا, خطأ);
      وإلا {
	    vvp_کتار* src_q = get_کتار_object<vvp_کتار_vec4>(thr, net);
	    vvp_کتار_vec4* src = dynamic_cast<vvp_کتار_vec4*>(src_q);
	    dst = کتار_پیدا_کړه_لومړی_وروستی_منزل_کتار_src(src, wid, cmp, ریښتیا, خطأ);
      }
      thr->ټېل_object(vvp_object_t(dst));
      أرجع ریښتیا;
}

منطقي د_کتار_پیدا_کړه_لومړی_PROP_V(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_vector4_t cmp = thr->راوباسه_vec4();
      size_t pid = cp->شمېره;
      بینښه wid = cp->بټ_idx[0];
      أكد(cmp.size() == wid);

      vvp_object_t& سر = thr->peek_object();
      vvp_cobject*cobj = سر.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_کتار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_کتار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_کتار_vec4* dst =
	    qsrc ? کتار_پیدا_کړه_لومړی_وروستی_منزل_کتار_src(qsrc, wid, cmp, ریښتیا, خطأ)
		 : کتار_پیدا_کړه_لومړی_وروستی_منزل_کتار_src(dsrc, wid, cmp, ریښتیا, خطأ);
      thr->ټېل_object(vvp_object_t(dst));
      أرجع ریښتیا;
}

منطقي د_کتار_پیدا_کړه_لومړی_شاخص_V(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_vector4_t cmp = thr->راوباسه_vec4();
      بینښه wid = cp->بټ_idx[0];
      أكد(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_کتار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_کتار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_کتار_vec4* dst;
      إذا (qsrc) dst = کتار_پیدا_کړه_لومړی_وروستی_منزل_کتار_src(qsrc, wid, cmp, ریښتیا, ریښتیا);
      وإلا إذا (dsrc) dst = کتار_پیدا_کړه_لومړی_وروستی_منزل_کتار_src(dsrc, wid, cmp, ریښتیا, ریښتیا);
      وإلا {
	    vvp_کتار* src_q = get_کتار_object<vvp_کتار_vec4>(thr, net);
	    vvp_کتار_vec4* src = dynamic_cast<vvp_کتار_vec4*>(src_q);
	    dst = کتار_پیدا_کړه_لومړی_وروستی_منزل_کتار_src(src, wid, cmp, ریښتیا, ریښتیا);
      }
      thr->ټېل_object(vvp_object_t(dst));
      أرجع ریښتیا;
}

منطقي د_کتار_پیدا_کړه_لومړی_شاخص_PROP_V(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_vector4_t cmp = thr->راوباسه_vec4();
      size_t pid = cp->شمېره;
      بینښه wid = cp->بټ_idx[0];
      أكد(cmp.size() == wid);

      vvp_object_t& سر = thr->peek_object();
      vvp_cobject*cobj = سر.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_کتار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_کتار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_کتار_vec4* dst =
	    qsrc ? کتار_پیدا_کړه_لومړی_وروستی_منزل_کتار_src(qsrc, wid, cmp, ریښتیا, ریښتیا)
		 : کتار_پیدا_کړه_لومړی_وروستی_منزل_کتار_src(dsrc, wid, cmp, ریښتیا, ریښتیا);
      thr->ټېل_object(vvp_object_t(dst));
      أرجع ریښتیا;
}

منطقي د_کتار_پیدا_کړه_وروستی_V(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_vector4_t cmp = thr->راوباسه_vec4();
      بینښه wid = cp->بټ_idx[0];
      أكد(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_کتار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_کتار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_کتار_vec4* dst;
      إذا (qsrc) dst = کتار_پیدا_کړه_لومړی_وروستی_منزل_کتار_src(qsrc, wid, cmp, خطأ, خطأ);
      وإلا إذا (dsrc) dst = کتار_پیدا_کړه_لومړی_وروستی_منزل_کتار_src(dsrc, wid, cmp, خطأ, خطأ);
      وإلا {
	    vvp_کتار* src_q = get_کتار_object<vvp_کتار_vec4>(thr, net);
	    vvp_کتار_vec4* src = dynamic_cast<vvp_کتار_vec4*>(src_q);
	    dst = کتار_پیدا_کړه_لومړی_وروستی_منزل_کتار_src(src, wid, cmp, خطأ, خطأ);
      }
      thr->ټېل_object(vvp_object_t(dst));
      أرجع ریښتیا;
}

منطقي د_کتار_پیدا_کړه_وروستی_PROP_V(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_vector4_t cmp = thr->راوباسه_vec4();
      size_t pid = cp->شمېره;
      بینښه wid = cp->بټ_idx[0];
      أكد(cmp.size() == wid);

      vvp_object_t& سر = thr->peek_object();
      vvp_cobject*cobj = سر.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_کتار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_کتار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_کتار_vec4* dst =
	    qsrc ? کتار_پیدا_کړه_لومړی_وروستی_منزل_کتار_src(qsrc, wid, cmp, خطأ, خطأ)
		 : کتار_پیدا_کړه_لومړی_وروستی_منزل_کتار_src(dsrc, wid, cmp, خطأ, خطأ);
      thr->ټېل_object(vvp_object_t(dst));
      أرجع ریښتیا;
}

منطقي د_کتار_پیدا_کړه_وروستی_شاخص_V(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_vector4_t cmp = thr->راوباسه_vec4();
      بینښه wid = cp->بټ_idx[0];
      أكد(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_کتار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_کتار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_کتار_vec4* dst;
      إذا (qsrc) dst = کتار_پیدا_کړه_لومړی_وروستی_منزل_کتار_src(qsrc, wid, cmp, خطأ, ریښتیا);
      وإلا إذا (dsrc) dst = کتار_پیدا_کړه_لومړی_وروستی_منزل_کتار_src(dsrc, wid, cmp, خطأ, ریښتیا);
      وإلا {
	    vvp_کتار* src_q = get_کتار_object<vvp_کتار_vec4>(thr, net);
	    vvp_کتار_vec4* src = dynamic_cast<vvp_کتار_vec4*>(src_q);
	    dst = کتار_پیدا_کړه_لومړی_وروستی_منزل_کتار_src(src, wid, cmp, خطأ, ریښتیا);
      }
      thr->ټېل_object(vvp_object_t(dst));
      أرجع ریښتیا;
}

منطقي د_کتار_پیدا_کړه_وروستی_شاخص_PROP_V(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_vector4_t cmp = thr->راوباسه_vec4();
      size_t pid = cp->شمېره;
      بینښه wid = cp->بټ_idx[0];
      أكد(cmp.size() == wid);

      vvp_object_t& سر = thr->peek_object();
      vvp_cobject*cobj = سر.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_کتار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_کتار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_کتار_vec4* dst =
	    qsrc ? کتار_پیدا_کړه_لومړی_وروستی_منزل_کتار_src(qsrc, wid, cmp, خطأ, ریښتیا)
		 : کتار_پیدا_کړه_لومړی_وروستی_منزل_کتار_src(dsrc, wid, cmp, خطأ, ریښتیا);
      thr->ټېل_object(vvp_object_t(dst));
      أرجع ریښتیا;
}

منطقي د_کتار_MIN_V(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_net_t* net = cp->net;
      بینښه wid = cp->بټ_idx[0];
      vvp_کتار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_کتار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_کتار_vec4* dst;
      إذا (qsrc) dst = کتار_پرلپسې_min_max_src(qsrc, wid, خطأ);
      وإلا إذا (dsrc) dst = کتار_پرلپسې_min_max_src(dsrc, wid, خطأ);
      وإلا {
	    vvp_کتار* src_q = get_کتار_object<vvp_کتار_vec4>(thr, net);
	    vvp_کتار_vec4* src = dynamic_cast<vvp_کتار_vec4*>(src_q);
	    dst = کتار_پرلپسې_min_max_src(src, wid, خطأ);
      }
      thr->ټېل_object(vvp_object_t(dst));
      أرجع ریښتیا;
}

منطقي د_کتار_MIN_OBJ_V(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه wid = cp->بټ_idx[0];
      vvp_object_t src_obj;
      thr->راوباسه_object(src_obj);

      vvp_کتار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_کتار_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_کتار_vec4* dst = qsrc ? کتار_پرلپسې_min_max_src(qsrc, wid, خطأ)
				 : کتار_پرلپسې_min_max_src(dsrc, wid, خطأ);
      thr->ټېل_object(vvp_object_t(dst));
      أرجع ریښتیا;
}

منطقي د_کتار_MIN_PROP_V(vthread_t thr, vvp_کوډ_t cp)
{
      size_t pid = cp->شمېره;
      بینښه wid = cp->بټ_idx[0];

      vvp_object_t& سر = thr->peek_object();
      vvp_cobject*cobj = سر.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_کتار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_کتار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_کتار_vec4* dst = qsrc ? کتار_پرلپسې_min_max_src(qsrc, wid, خطأ)
				 : کتار_پرلپسې_min_max_src(dsrc, wid, خطأ);
      thr->ټېل_object(vvp_object_t(dst));
      أرجع ریښتیا;
}

منطقي د_کتار_MAX_V(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_net_t* net = cp->net;
      بینښه wid = cp->بټ_idx[0];
      vvp_کتار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_کتار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_کتار_vec4* dst;
      إذا (qsrc) dst = کتار_پرلپسې_min_max_src(qsrc, wid, ریښتیا);
      وإلا إذا (dsrc) dst = کتار_پرلپسې_min_max_src(dsrc, wid, ریښتیا);
      وإلا {
	    vvp_کتار* src_q = get_کتار_object<vvp_کتار_vec4>(thr, net);
	    vvp_کتار_vec4* src = dynamic_cast<vvp_کتار_vec4*>(src_q);
	    dst = کتار_پرلپسې_min_max_src(src, wid, ریښتیا);
      }
      thr->ټېل_object(vvp_object_t(dst));
      أرجع ریښتیا;
}

منطقي د_کتار_MAX_OBJ_V(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه wid = cp->بټ_idx[0];
      vvp_object_t src_obj;
      thr->راوباسه_object(src_obj);

      vvp_کتار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_کتار_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_کتار_vec4* dst = qsrc ? کتار_پرلپسې_min_max_src(qsrc, wid, ریښتیا)
				 : کتار_پرلپسې_min_max_src(dsrc, wid, ریښتیا);
      thr->ټېل_object(vvp_object_t(dst));
      أرجع ریښتیا;
}

منطقي د_کتار_MAX_PROP_V(vthread_t thr, vvp_کوډ_t cp)
{
      size_t pid = cp->شمېره;
      بینښه wid = cp->بټ_idx[0];

      vvp_object_t& سر = thr->peek_object();
      vvp_cobject*cobj = سر.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_کتار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_کتار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_کتار_vec4* dst = qsrc ? کتار_پرلپسې_min_max_src(qsrc, wid, ریښتیا)
				 : کتار_پرلپسې_min_max_src(dsrc, wid, ریښتیا);
      thr->ټېل_object(vvp_object_t(dst));
      أرجع ریښتیا;
}

منطقي د_کتار_UNIQUE_OBJ_V(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه wid = cp->بټ_idx[0];
      vvp_object_t src_obj;
      thr->راوباسه_object(src_obj);

      vvp_کتار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_کتار_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_کتار_vec4* dst = qsrc ? کتار_پرلپسې_unique_src(qsrc, wid, خطأ)
				 : کتار_پرلپسې_unique_src(dsrc, wid, خطأ);
      thr->ټېل_object(vvp_object_t(dst));
      أرجع ریښتیا;
}

منطقي د_کتار_UNIQUE_شاخص_OBJ_V(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه wid = cp->بټ_idx[0];
      vvp_object_t src_obj;
      thr->راوباسه_object(src_obj);

      vvp_کتار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_کتار_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_کتار_vec4* dst = qsrc ? کتار_پرلپسې_unique_src(qsrc, wid, ریښتیا)
				 : کتار_پرلپسې_unique_src(dsrc, wid, ریښتیا);
      thr->ټېل_object(vvp_object_t(dst));
      أرجع ریښتیا;
}

منطقي د_کتار_UNIQUE_V(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_net_t*net = cp->net;
      بینښه wid = cp->بټ_idx[0];
      vvp_کتار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_کتار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_کتار_vec4* dst;
      إذا (qsrc) dst = کتار_پرلپسې_unique_src(qsrc, wid, خطأ);
      وإلا إذا (dsrc) dst = کتار_پرلپسې_unique_src(dsrc, wid, خطأ);
      وإلا {
	    vvp_کتار* src_q = get_کتار_object<vvp_کتار_vec4>(thr, net);
	    vvp_کتار_vec4* src = dynamic_cast<vvp_کتار_vec4*>(src_q);
	    dst = کتار_پرلپسې_unique_src(src, wid, خطأ);
      }
      thr->ټېل_object(vvp_object_t(dst));
      أرجع ریښتیا;
}

منطقي د_کتار_UNIQUE_PROP_V(vthread_t thr, vvp_کوډ_t cp)
{
      size_t pid = cp->شمېره;
      بینښه wid = cp->بټ_idx[0];

      vvp_object_t& سر = thr->peek_object();
      vvp_cobject*cobj = سر.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_کتار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_کتار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_کتار_vec4* dst = qsrc ? کتار_پرلپسې_unique_src(qsrc, wid, خطأ)
				 : کتار_پرلپسې_unique_src(dsrc, wid, خطأ);
      thr->ټېل_object(vvp_object_t(dst));
      أرجع ریښتیا;
}

منطقي د_کتار_UNIQUE_شاخص_V(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_net_t*net = cp->net;
      بینښه wid = cp->بټ_idx[0];
      vvp_کتار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_کتار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_کتار_vec4* dst;
      إذا (qsrc) dst = کتار_پرلپسې_unique_src(qsrc, wid, ریښتیا);
      وإلا إذا (dsrc) dst = کتار_پرلپسې_unique_src(dsrc, wid, ریښتیا);
      وإلا {
	    vvp_کتار* src_q = get_کتار_object<vvp_کتار_vec4>(thr, net);
	    vvp_کتار_vec4* src = dynamic_cast<vvp_کتار_vec4*>(src_q);
	    dst = کتار_پرلپسې_unique_src(src, wid, ریښتیا);
      }
      thr->ټېل_object(vvp_object_t(dst));
      أرجع ریښتیا;
}

منطقي د_کتار_UNIQUE_شاخص_PROP_V(vthread_t thr, vvp_کوډ_t cp)
{
      size_t pid = cp->شمېره;
      بینښه wid = cp->بټ_idx[0];

      vvp_object_t& سر = thr->peek_object();
      vvp_cobject*cobj = سر.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_کتار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_کتار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_کتار_vec4* dst = qsrc ? کتار_پرلپسې_unique_src(qsrc, wid, ریښتیا)
				 : کتار_پرلپسې_unique_src(dsrc, wid, ریښتیا);
      thr->ټېل_object(vvp_object_t(dst));
      أرجع ریښتیا;
}

منطقي د_PROP_کتار_اندازه(vthread_t thr, vvp_کوډ_t cp)
{
      size_t pid = cp->شمېره;

      vvp_object_t& سر = thr->peek_object();
      vvp_cobject*cobj = سر.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_کتار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_کتار_or_darray_vec4_from_object(qobj, qsrc, dsrc);

      size_t sz = qsrc ? qsrc->get_اندازه() : (dsrc ? dsrc->get_اندازه() : 0);

      vvp_vector4_t val(32, BIT4_0);
      بینښه اوږد ul = sz;
      لكل (بینښه idx = 0; idx < 32; idx++) {
	    إذا (ul & 1UL) {
		  val.سېټ_بټ(idx, BIT4_1);
	    } وإلا {
		  val.سېټ_بټ(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      thr->ټېل_vec4(val);
      أرجع ریښتیا;
}

منطقي د_کتار_اندازه_V(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_کتار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_کتار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      size_t sz =
	    qsrc ? qsrc->get_اندازه() : (dsrc ? dsrc->get_اندازه() : 0);

      vvp_vector4_t val(32, BIT4_0);
      بینښه اوږد ul = sz;
      لكل (بینښه idx = 0; idx < 32; idx++) {
	    إذا (ul & 1UL) {
		  val.سېټ_بټ(idx, BIT4_1);
	    } وإلا {
		  val.سېټ_بټ(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      thr->ټېل_vec4(val);
      أرجع ریښتیا;
}

منطقي د_کتار_کلمه_V(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه wid = cp->بټ_idx[0];
      بینښه ix_reg = cp->بټ_idx[1];
      int64_t adr = thr->ټکي[ix_reg].w_int;
      إذا (thr->flags[4] == BIT4_1) {
	    thr->ټېل_vec4(vvp_vector4_t(wid));
	    أرجع ریښتیا;
      }
      إذا (adr < 0) {
	    thr->ټېل_vec4(vvp_vector4_t(wid));
	    أرجع ریښتیا;
      }
      vvp_net_t* net = cp->net;
      vvp_کتار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_کتار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t out(wid);
      إذا (qsrc) {
	    إذا ((size_t)adr >= qsrc->get_اندازه()) {
		  thr->ټېل_vec4(out);
		  أرجع ریښتیا;
	    }
	    qsrc->get_کلمه((بینښه)adr, out);
      } وإلا إذا (dsrc) {
	    إذا ((size_t)adr >= dsrc->get_اندازه()) {
		  thr->ټېل_vec4(out);
		  أرجع ریښتیا;
	    }
	    dsrc->get_کلمه((بینښه)adr, out);
      } وإلا {
	    vvp_کتار* src_q = get_کتار_object<vvp_کتار_vec4>(thr, net);
	    vvp_کتار_vec4* src = dynamic_cast<vvp_کتار_vec4*>(src_q);
	    إذا (!src || (size_t)adr >= src->get_اندازه()) {
		  thr->ټېل_vec4(out);
		  أرجع ریښتیا;
	    }
	    src->get_کلمه((بینښه)adr, out);
      }
      thr->ټېل_vec4(out);
      أرجع ریښتیا;
}

منطقي د_کتار_کلمه_PROP_V(vthread_t thr, vvp_کوډ_t cp)
{
      size_t pid = cp->شمېره;
      بینښه wid = cp->بټ_idx[0];
      بینښه ix_reg = cp->بټ_idx[1];
      int64_t adr = thr->ټکي[ix_reg].w_int;
      إذا (thr->flags[4] == BIT4_1) {
	    thr->ټېل_vec4(vvp_vector4_t(wid));
	    أرجع ریښتیا;
      }
      إذا (adr < 0) {
	    thr->ټېل_vec4(vvp_vector4_t(wid));
	    أرجع ریښتیا;
      }

      vvp_object_t saved_سر;
      منطقي popped_سر = خطأ;
      vvp_object_t& سر = thr->peek_object();
      vvp_cobject*cobj = سر.peek<vvp_cobject>();
      إذا (cobj == 0) {
	    // Array-locator with(property) keeps an accumulator queue on top.
	    // Temporarily pop it to access the class object beneath.
	    thr->راوباسه_object(saved_سر);
	    popped_سر = ریښتیا;
	    vvp_object_t& لاندې = thr->peek_object();
	    cobj = لاندې.peek<vvp_cobject>();
      }
      إذا (cobj == 0) {
	    إذا (popped_سر) thr->ټېل_object(saved_سر);
	    thr->ټېل_vec4(vvp_vector4_t(wid));
	    أرجع ریښتیا;
      }

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_کتار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_کتار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t out(wid);
      إذا (qsrc) {
	    إذا ((size_t)adr >= qsrc->get_اندازه()) {
		  thr->ټېل_vec4(out);
		  أرجع ریښتیا;
	    }
	    qsrc->get_کلمه((بینښه)adr, out);
      } وإلا إذا (dsrc) {
	    إذا ((size_t)adr >= dsrc->get_اندازه()) {
		  thr->ټېل_vec4(out);
		  أرجع ریښتیا;
	    }
	    dsrc->get_کلمه((بینښه)adr, out);
      } وإلا {
	    thr->ټېل_vec4(out);
	    إذا (popped_سر) thr->ټېل_object(saved_سر);
	    أرجع ریښتیا;
      }
      إذا (popped_سر) thr->ټېل_object(saved_سر);
      thr->ټېل_vec4(out);
      أرجع ریښتیا;
}

منطقي د_CMPIX_LTU(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه a = cp->شمېره;
      بینښه b = cp->بټ_idx[0];
      إذا (thr->ټکي[a].w_uint < thr->ټکي[b].w_uint) {
	    thr->flags[4] = BIT4_1;
      } وإلا {
	    thr->flags[4] = BIT4_0;
      }
      أرجع ریښتیا;
}

منطقي د_CMPIX_SLT0(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه r = cp->شمېره;
      thr->flags[4] = (thr->ټکي[r].w_int < 0) ? BIT4_1 : BIT4_0;
      أرجع ریښتیا;
}

منطقي د_ټېل_IX_VEC4(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه ix = cp->شمېره;
      بینښه wid = cp->بټ_idx[0];
      بینښه sgn = cp->بټ_idx[1];
      uint32_t ubits;
      إذا (sgn) {
	    int32_t sv = (int32_t)thr->ټکي[ix].w_int;
	    ubits = (uint32_t)sv;
      } وإلا {
	    ubits = (uint32_t)thr->ټکي[ix].w_uint;
      }
      إذا (wid > 32) wid = 32;
      vvp_vector4_t tmp(wid, BIT4_0);
      لكل (بینښه idx = 0; idx < wid; idx++) {
	    إذا (ubits & 1u) {
		  tmp.سېټ_بټ(idx, BIT4_1);
	    } وإلا {
		  tmp.سېټ_بټ(idx, BIT4_0);
	    }
	    ubits >>= 1;
      }
      thr->ټېل_vec4(tmp);
      أرجع ریښتیا;
}

منطقي د_کتار_NEW_تشه_V(vthread_t thr, vvp_کوډ_t)
{
      thr->ټېل_object(vvp_object_t(جديد vvp_کتار_vec4()));
      أرجع ریښتیا;
}

منطقي د_کتار_APPEND_کلمه_V(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه wid = cp->بټ_idx[0];
      vvp_vector4_t elem = thr->راوباسه_vec4();
      أكد(elem.size() == wid);
      vvp_object_t qobj;
      thr->راوباسه_object(qobj);
      vvp_کتار_vec4* q = qobj.peek<vvp_کتار_vec4>();
      أكد(q);
      q->push_back(elem, 0);
      thr->ټېل_object(qobj);
      أرجع ریښتیا;
}

منطقي د_DELETE_PROP_ELEM(vthread_t thr, vvp_کوډ_t cp)
{
      size_t pid = cp->شمېره;

      int64_t idx_val = thr->ټکي[3].w_int;
      إذا (thr->flags[4] == BIT4_1) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with undefined index."
	         << کرښه_پای;
	    أرجع ریښتیا;
      }
      إذا (idx_val < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with negative index."
	         << کرښه_پای;
	    أرجع ریښتیا;
      }
      size_t idx = idx_val;

      vvp_object_t& سر = thr->peek_object();
      vvp_cobject*cobj = سر.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_کتار* کتار = qobj.peek<vvp_کتار>();
      إذا (کتار == 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping delete(" << idx
	         << ") on empty queue." << کرښه_پای;
      } وإلا {
	    size_t size = کتار->get_اندازه();
	    إذا (idx >= size) {
		  cerr << thr->get_fileline()
		       << "Warning: skipping out of range delete(" << idx
		       << ") on queue of size " << size << "." << کرښه_پای;
	    } وإلا {
		  کتار->erase(idx);
	    }
      }

      أرجع ریښتیا;
}

منطقي د_DELETE_PROP_OBJ(vthread_t thr, vvp_کوډ_t cp)
{
      size_t pid = cp->شمېره;

      vvp_object_t& سر = thr->peek_object();
      vvp_cobject*cobj = سر.peek<vvp_cobject>();
      أكد(cobj);

      cobj->سېټ_object(pid, vvp_object_t(), 0);

      أرجع ریښتیا;
}

کينډۍ <ډول_نوم ELEM, صنف QTYPE>
ساكن منطقي store_qb(vthread_t thr, vvp_کوډ_t cp, بینښه wid=0)
{
      ELEM value;
      vvp_net_t*net = cp->net;
      بینښه max_اندازه = thr->ټکي[cp->بټ_idx[0]].w_int;
      راوباسه_ارزښت(thr, value, wid); // Pop the value to store.

      vvp_کتار*کتار = get_کتار_object<QTYPE>(thr, net);
      أكد(کتار);
      کتار->push_back(value, max_اندازه);
      أرجع ریښتیا;
}

/*
 * %store/qb/r <var-label>, <max-idx>
 */
منطقي د_STORE_QB_R(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع store_qb<مزدوج, vvp_کتار_real>(thr, cp);
}

/*
 * %store/qb/str <var-label>, <max-idx>
 */
منطقي د_STORE_QB_STR(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع store_qb<string, vvp_کتار_string>(thr, cp);
}

/*
 * %store/qb/v <var-label>, <max-idx>, <wid>
 */
منطقي د_STORE_QB_V(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع store_qb<vvp_vector4_t, vvp_کتار_vec4>(thr, cp, cp->بټ_idx[1]);
}

کينډۍ <ډول_نوم ELEM, صنف QTYPE>
ساكن منطقي store_qdar(vthread_t thr, vvp_کوډ_t cp, بینښه wid=0)
{
      int64_t idx = thr->ټکي[3].w_int;
      ELEM value;
      vvp_net_t*net = cp->net;
      راوباسه_ارزښت(thr, value, wid); // Pop the value to store.

      vvp_کتار*کتار = get_کتار_object<QTYPE>(thr, net);
      أكد(کتار);
      إذا (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot assign to a negative "
	         << get_کتار_type(value)
	         << " index (" << idx << "). ";
	    print_کتار_ارزښت(value);
	    cerr << " was not added." << کرښه_پای;
      } وإلا إذا (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot assign to an undefined "
	         << get_کتار_type(value) << " index. ";
	    print_کتار_ارزښت(value);
	    cerr << " was not added." << کرښه_پای;
      } وإلا {
	    بینښه max_اندازه = thr->ټکي[cp->بټ_idx[0]].w_int;
	    کتار->سېټ_کلمه_max(idx, value, max_اندازه);
      }
      أرجع ریښتیا;
}

/*
 * %store/qdar/r <var>, idx
 */
منطقي د_STORE_QDAR_R(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع store_qdar<مزدوج, vvp_کتار_real>(thr, cp);
}

/*
 * %store/qdar/str <var>, idx
 */
منطقي د_STORE_QDAR_STR(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع store_qdar<string, vvp_کتار_string>(thr, cp);
}

/*
 * %store/qdar/v <var>, idx
 */
منطقي د_STORE_QDAR_V(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع store_qdar<vvp_vector4_t, vvp_کتار_vec4>(thr, cp, cp->بټ_idx[1]);
}

کينډۍ <ډول_نوم ELEM, صنف QTYPE>
ساكن منطقي store_qf(vthread_t thr, vvp_کوډ_t cp, بینښه wid=0)
{
      ELEM value;
      vvp_net_t*net = cp->net;
      بینښه max_اندازه = thr->ټکي[cp->بټ_idx[0]].w_int;
      راوباسه_ارزښت(thr, value, wid); // Pop the value to store.

      vvp_کتار*کتار = get_کتار_object<QTYPE>(thr, net);
      أكد(کتار);
      کتار->push_front(value, max_اندازه);
      أرجع ریښتیا;
}
/*
 * %store/qf/r <var-label>, <max-idx>
 */
منطقي د_STORE_QF_R(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع store_qf<مزدوج, vvp_کتار_real>(thr, cp);
}

/*
 * %store/qf/str <var-label>, <max-idx>
 */
منطقي د_STORE_QF_STR(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع store_qf<string, vvp_کتار_string>(thr, cp);
}

/*
 * %store/qb/v <var-label>, <max-idx>, <wid>
 */
منطقي د_STORE_QF_V(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع store_qf<vvp_vector4_t, vvp_کتار_vec4>(thr, cp, cp->بټ_idx[1]);
}

کينډۍ <ډول_نوم ELEM, صنف QTYPE>
ساكن منطقي store_qobj(vthread_t thr, vvp_کوډ_t cp, بینښه wid=0)
{
// FIXME: Can we actually use wid here?
      (فراغ)wid;
      vvp_net_t*net = cp->net;

      vvp_کتار*کتار = get_کتار_object<QTYPE>(thr, net);
      أكد(کتار);

      vvp_object_t src;
      thr->راوباسه_object(src);

        // If it is null just clear the queue
      إذا (src.test_nil())
	    کتار->erase_لکۍ(0);
      وإلا {
	    بینښه max_اندازه = thr->ټکي[cp->بټ_idx[0]].w_int;
	    کتار->copy_elems(src, max_اندازه);
      }

      أرجع ریښتیا;
}

منطقي د_STORE_QOBJ_R(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع store_qobj<مزدوج, vvp_کتار_real>(thr, cp);
}

منطقي د_STORE_QOBJ_STR(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع store_qobj<string, vvp_کتار_string>(thr, cp);
}

منطقي د_STORE_QOBJ_V(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع store_qobj<vvp_vector4_t, vvp_کتار_vec4>(thr, cp, cp->بټ_idx[1]);
}

ساكن فراغ vvp_send(vthread_t thr, vvp_net_ptr_t ptr, ثابت مزدوج&val)
{
      vvp_send_real(ptr, val, thr->wt_context);
}

ساكن فراغ vvp_send(vthread_t thr, vvp_net_ptr_t ptr, ثابت string&val)
{
      vvp_send_string(ptr, val, thr->wt_context);
}

کينډۍ <ډول_نوم ELEM>
ساكن منطقي store(vthread_t thr, vvp_کوډ_t cp)
{
      ELEM val;
      راوباسه_ارزښت(thr, val, 0);
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_send(thr, ptr, val);
      أرجع ریښتیا;
}

منطقي د_STORE_REAL(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع store<مزدوج>(thr, cp);
}

کينډۍ <ډول_نوم ELEM>
ساكن منطقي storea(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه idx = cp->بټ_idx[0];
      ELEM val;
      راوباسه_ارزښت(thr, val, 0);

      إذا (thr->flags[4] != BIT4_1) {
	    بینښه adr = thr->ټکي[idx].w_int;
	    cp->array->سېټ_کلمه(adr, val);
      }

      أرجع ریښتیا;
}

/*
 * %store/reala <var-label> <index>
 */
منطقي د_STORE_REALA(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع storea<مزدوج>(thr, cp);
}

منطقي د_STORE_STR(vthread_t thr, vvp_کوډ_t cp)
{
      أرجع store<string>(thr, cp);
}

/*
 * %store/stra <array-label> <index>
 */
منطقي د_STORE_STRA(vthread_t thr, vvp_کوډ_t cp)
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
منطقي د_STORE_VEC4(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_net_ptr_t ptr(cp->net, 0);
      ثابت vvp_signal_ارزښت*sig = dynamic_cast<vvp_signal_ارزښت*> (cp->net->fil);
      بینښه off_شاخص = cp->بټ_idx[0];
      بینښه صحيح wid = cp->بټ_idx[1];

      int64_t off = off_شاخص ? thr->ټکي[off_شاخص].w_int : 0;
      بینښه صحيح sig_ارزښت_اندازه = sig->ارزښت_اندازه();

      vvp_vector4_t&val = thr->peek_vec4();
      بینښه val_اندازه = val.size();

      إذا (val_اندازه < wid) {
	    cerr << thr->get_fileline()
	         << "XXXX Internal error: val.size()=" << val_اندازه
		 << ", expecting >= " << wid << کرښه_پای;
      }
      أكد(val_اندازه >= wid);
      إذا (val_اندازه > wid) {
	    val.resize(wid);
      }

	// If there is a problem loading the index register, flags-4
	// will be set to 1, and we know here to skip the actual assignment.
      إذا (off_شاخص!=0 && thr->flags[4] == BIT4_1) {
	    thr->راوباسه_vec4(1);
	    أرجع ریښتیا;
      }

      إذا (!resize_rval_vec(val, off, sig_ارزښت_اندازه)) {
	    thr->راوباسه_vec4(1);
	    أرجع ریښتیا;
      }

      إذا (off == 0 && val.size() == sig_ارزښت_اندازه)
	    vvp_send_vec4(ptr, val, thr->wt_context);
      وإلا
	    vvp_send_vec4_pv(ptr, val, off, sig_ارزښت_اندازه, thr->wt_context);

      thr->راوباسه_vec4(1);
      أرجع ریښتیا;
}

/*
 * %store/vec4a <var-label>, <addr>, <offset>
 */
منطقي د_STORE_VEC4_A(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه adr_شاخص = cp->بټ_idx[0];
      بینښه off_شاخص = cp->بټ_idx[1];

      اوږد adr = adr_شاخص? thr->ټکي[adr_شاخص].w_int : 0;
      int64_t off = off_شاخص ? thr->ټکي[off_شاخص].w_int : 0;

	// Suppress action if flags-4 is true.
      إذا (thr->flags[4] == BIT4_1) {
	    thr->راوباسه_vec4(1);
	    أرجع ریښتیا;
      }

      vvp_vector4_t &value = thr->peek_vec4();

      إذا (!resize_rval_vec(value, off, cp->array->get_کلمه_اندازه())) {
	    thr->راوباسه_vec4(1);
	    أرجع ریښتیا;
      }

      cp->array->سېټ_کلمه(adr, off, value);

      thr->راوباسه_vec4(1);
      أرجع ریښتیا;
}

/*
 * %sub
 *   pop r;
 *   pop l;
 *   push l-r;
 */
منطقي د_SUB(vthread_t thr, vvp_کوډ_t)
{
      vvp_vector4_t r = thr->راوباسه_vec4();
      vvp_vector4_t&l = thr->peek_vec4();

      l.sub(r);
      أرجع ریښتیا;
}

/*
 * %subi <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
منطقي د_SUBI(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه wid = cp->شمېره;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_سمدستي_rval (cp, r);

      l.sub(r);

      أرجع ریښتیا;

}

منطقي د_SUB_WR(vthread_t thr, vvp_کوډ_t)
{
      مزدوج r = thr->راوباسه_real();
      مزدوج l = thr->راوباسه_real();
      thr->ټېل_real(l - r);
      أرجع ریښتیا;
}

/*
 * %substr <first>, <last>
 * Pop a string, take the substring (SystemVerilog style), and return
 * the result to the stack. This opcode actually works by editing the
 * string in place.
 */
منطقي د_SUBSTR(vthread_t thr, vvp_کوډ_t cp)
{
      int32_t لومړی = thr->ټکي[cp->بټ_idx[0]].w_int;
      int32_t وروستی = thr->ټکي[cp->بټ_idx[1]].w_int;
      string&val = thr->peek_str(0);

      إذا (لومړی < 0 || وروستی < لومړی || وروستی >= (int32_t)val.size()) {
	    val = string("");
	    أرجع ریښتیا;
      }

      val = val.substr(لومړی, وروستی-لومړی+1);
      أرجع ریښتیا;
}

/*
 * %substr/vec4 <index>, <wid>
 */
منطقي د_SUBSTR_VEC4(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه sel_idx = cp->بټ_idx[0];
      بینښه wid = cp->بټ_idx[1];

      int32_t sel = thr->ټکي[sel_idx].w_int;
      ثابت string&val = thr->peek_str(0);

      أكد(wid%8 == 0);

      إذا (sel < 0 || sel >= (int32_t)val.size()) {
	    vvp_vector4_t res (wid, BIT4_0);
	    thr->ټېل_vec4(res);
	    أرجع ریښتیا;
      }

      vvp_vector4_t res (wid, BIT4_0);

      أكد(wid==8);
      بینښه حرف tmp = val[sel];
      لكل (صحيح idx = 0 ; idx < 8 ; idx += 1) {
	    إذا (tmp & (1<<idx))
		  res.سېټ_بټ(idx, BIT4_1);
      }

      thr->ټېل_vec4(res);
      أرجع ریښتیا;
}

منطقي د_FILE_کرښه(vthread_t thr, vvp_کوډ_t cp)
{
      vpiHandle handle = cp->handle;

	/* When it is available, keep the file/line information in the
	   thread for error/warning messages. */
      thr->سېټ_fileline(vpi_get_str(vpiFile, handle),
                        vpi_get(vpi_کرښه_No, handle));

      إذا (show_file_کرښه)
	    cerr << thr->get_fileline()
	         << vpi_get_str(_vpiDescription, handle) << کرښه_پای;

      أرجع ریښتیا;
}

/*
 * %test_nul <var-label>;
 * Test if the object at the specified variable is nil. If so, write
 * "1" into flags[4], otherwise write "0" into flags[4].
 */
منطقي د_TEST_NUL(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_net_t*net = cp->net;

      أكد(net);
      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      أكد(obj);

      إذا (obj->get_object().test_nil())
	    thr->flags[4] = BIT4_1;
      وإلا
	    thr->flags[4] = BIT4_0;

      أرجع ریښتیا;
}

منطقي د_TEST_NUL_A(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه idx = cp->بټ_idx[0];
      بینښه adr = thr->ټکي[idx].w_int;
      vvp_object_t کلمه;

	/* If the address is undefined, return true. */
      إذا (thr->flags[4] == BIT4_1) {
	    أرجع ریښتیا;
      }

      cp->array->get_کلمه_obj(adr, کلمه);
      إذا (کلمه.test_nil())
	    thr->flags[4] = BIT4_1;
      وإلا
	    thr->flags[4] = BIT4_0;

      أرجع ریښتیا;
}

منطقي د_TEST_NUL_OBJ(vthread_t thr, vvp_کوډ_t)
{
      إذا (thr->peek_object().test_nil())
	    thr->flags[4] = BIT4_1;
      وإلا
	    thr->flags[4] = BIT4_0;
      أرجع ریښتیا;
}

/*
 * %test_nul/prop <pid>, <idx>
 */
منطقي د_TEST_NUL_PROP(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه pid = cp->شمېره;
      بینښه idx = cp->بټ_idx[0];

      إذا (idx != 0) {
	    أكد(idx < vthread_s::ټکي_شمېر);
	    idx = thr->ټکي[idx].w_uint;
      }

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj  = obj.peek<vvp_cobject>();

      vvp_object_t val;
      cobj->get_object(pid, val, idx);

      إذا (val.test_nil())
	    thr->flags[4] = BIT4_1;
      وإلا
	    thr->flags[4] = BIT4_0;

      أرجع ریښتیا;
}

/* and() reduction returns the scalar sum (Verilog-style bitwise-AND per word). */
کينډۍ <صنف SRC>
ساكن vvp_vector4_t uparr_and_ټکي_src(SRC* src, بینښه wid)
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

منطقي د_UPARR_AND_V(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_net_t* net = cp->net;
      بینښه wid = cp->بټ_idx[0];
      vvp_کتار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_کتار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      إذا (qsrc) res = uparr_and_ټکي_src(qsrc, wid);
      وإلا إذا (dsrc) res = uparr_and_ټکي_src(dsrc, wid);
      وإلا {
	    vvp_کتار* src_q = get_کتار_object<vvp_کتار_vec4>(thr, net);
	    vvp_کتار_vec4* src = dynamic_cast<vvp_کتار_vec4*>(src_q);
	    res = uparr_and_ټکي_src(src, wid);
      }
      thr->ټېل_vec4(res);
      أرجع ریښتیا;
}

/* or() reduction returns the scalar sum (Verilog-style bitwise-AND per word). */
کينډۍ <صنف SRC>
ساكن vvp_vector4_t uparr_or_ټکي_src(SRC* src, بینښه wid)
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

منطقي د_UPARR_OR_V(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_net_t* net = cp->net;
      بینښه wid = cp->بټ_idx[0];
      vvp_کتار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_کتار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      إذا (qsrc) res = uparr_or_ټکي_src(qsrc, wid);
      وإلا إذا (dsrc) res = uparr_or_ټکي_src(dsrc, wid);
      وإلا {
	    vvp_کتار* src_q = get_کتار_object<vvp_کتار_vec4>(thr, net);
	    vvp_کتار_vec4* src = dynamic_cast<vvp_کتار_vec4*>(src_q);
	    res = uparr_or_ټکي_src(src, wid);
      }
      thr->ټېل_vec4(res);
      أرجع ریښتیا;
}

/* real product() reduction returns the scalar sum */
کينډۍ <صنف SRC>
ساكن مزدوج uparr_حاصل_ضرب_ټکي_src_real(SRC* src)
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

منطقي د_UPARR_حاصل_ضرب_REAL(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_کتار_real* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_کتار_or_darray_real_from_net(thr, net, qsrc, dsrc);
      مزدوج مجموعه;
      إذا (qsrc) مجموعه = uparr_حاصل_ضرب_ټکي_src_real(qsrc);
      وإلا إذا (dsrc) مجموعه = uparr_حاصل_ضرب_ټکي_src_real(dsrc);
      وإلا {
	    vvp_کتار* src_q = get_کتار_object<vvp_کتار_real>(thr, net);
	    vvp_کتار_real* src = dynamic_cast<vvp_کتار_real*>(src_q);
	    مجموعه = uparr_حاصل_ضرب_ټکي_src_real(src);
      }
      thr->ټېل_real(مجموعه);
      أرجع ریښتیا;
}

/* product() reduction returns the scalar product (Verilog-style multiplication per word). */
کينډۍ <صنف SRC>
ساكن vvp_vector4_t uparr_حاصل_ضرب_ټکي_src(SRC* src, بینښه wid)
{
      إذا (!src) أرجع vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t acc = کتار_unique_ulong_منزل_vec4(1UL, wid);
      لكل (size_t i = 0; i < src->get_اندازه(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_کلمه(i, vi);
	    acc.mul(vi);
      }
      أرجع acc;
}

منطقي د_UPARR_حاصل_ضرب_V(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_net_t* net = cp->net;
      بینښه wid = cp->بټ_idx[0];
      vvp_کتار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_کتار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t prod;
      إذا (qsrc) prod = uparr_حاصل_ضرب_ټکي_src(qsrc, wid);
      وإلا إذا (dsrc) prod = uparr_حاصل_ضرب_ټکي_src(dsrc, wid);
      وإلا {
	    vvp_کتار* src_q = get_کتار_object<vvp_کتار_vec4>(thr, net);
	    vvp_کتار_vec4* src = dynamic_cast<vvp_کتار_vec4*>(src_q);
	    prod = uparr_حاصل_ضرب_ټکي_src(src, wid);
      }
      thr->ټېل_vec4(prod);
      أرجع ریښتیا;
}

منطقي د_UPARR_حاصل_ضرب_PROP_V(vthread_t thr, vvp_کوډ_t cp)
{
      size_t pid = cp->شمېره;
      بینښه wid = cp->بټ_idx[0];

      vvp_object_t& سر = thr->peek_object();
      vvp_cobject*cobj = سر.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_کتار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_کتار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t prod =
	    qsrc ? uparr_حاصل_ضرب_ټکي_src(qsrc, wid)
		 : uparr_حاصل_ضرب_ټکي_src(dsrc, wid);
      thr->ټېل_vec4(prod);
      أرجع ریښتیا;
}

منطقي د_UPARR_حاصل_ضرب_OBJ_V(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه wid = cp->بټ_idx[0];
      vvp_object_t src_obj;
      thr->راوباسه_object(src_obj);

      vvp_کتار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_کتار_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_vector4_t prod =
	    qsrc ? uparr_حاصل_ضرب_ټکي_src(qsrc, wid)
		 : uparr_حاصل_ضرب_ټکي_src(dsrc, wid);
      thr->ټېل_vec4(prod);
      أرجع ریښتیا;
}

/* real sum() reduction returns the scalar sum */
کينډۍ <صنف SRC>
ساكن مزدوج uparr_مجموعه_ټکي_src_real(SRC* src)
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

منطقي د_UPARR_مجموعه_REAL(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_کتار_real* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_کتار_or_darray_real_from_net(thr, net, qsrc, dsrc);
      مزدوج مجموعه;
      إذا (qsrc) مجموعه = uparr_مجموعه_ټکي_src_real(qsrc);
      وإلا إذا (dsrc) مجموعه = uparr_مجموعه_ټکي_src_real(dsrc);
      وإلا {
	    vvp_کتار* src_q = get_کتار_object<vvp_کتار_real>(thr, net);
	    vvp_کتار_real* src = dynamic_cast<vvp_کتار_real*>(src_q);
	    مجموعه = uparr_مجموعه_ټکي_src_real(src);
      }
      thr->ټېل_real(مجموعه);
      أرجع ریښتیا;
}

/* sum() reduction returns the scalar sum (Verilog-style addition per word). */
کينډۍ <صنف SRC>
ساكن vvp_vector4_t uparr_مجموعه_ټکي_src(SRC* src, بینښه wid)
{
      إذا (!src) أرجع vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t acc(wid, BIT4_0);
      لكل (size_t i = 0; i < src->get_اندازه(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_کلمه(i, vi);
	    acc.جمع_کړه(vi);
      }
      أرجع acc;
}

منطقي د_UPARR_مجموعه_V(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_net_t* net = cp->net;
      بینښه wid = cp->بټ_idx[0];
      vvp_کتار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_کتار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t مجموعه;
      إذا (qsrc) مجموعه = uparr_مجموعه_ټکي_src(qsrc, wid);
      وإلا إذا (dsrc) مجموعه = uparr_مجموعه_ټکي_src(dsrc, wid);
      وإلا {
	    vvp_کتار* src_q = get_کتار_object<vvp_کتار_vec4>(thr, net);
	    vvp_کتار_vec4* src = dynamic_cast<vvp_کتار_vec4*>(src_q);
	    مجموعه = uparr_مجموعه_ټکي_src(src, wid);
      }
      thr->ټېل_vec4(مجموعه);
      أرجع ریښتیا;
}

منطقي د_UPARR_مجموعه_PROP_V(vthread_t thr, vvp_کوډ_t cp)
{
      size_t pid = cp->شمېره;
      بینښه wid = cp->بټ_idx[0];

      vvp_object_t& سر = thr->peek_object();
      vvp_cobject*cobj = سر.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_کتار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_کتار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t مجموعه =
	    qsrc ? uparr_مجموعه_ټکي_src(qsrc, wid)
		 : uparr_مجموعه_ټکي_src(dsrc, wid);
      thr->ټېل_vec4(مجموعه);
      أرجع ریښتیا;
}

منطقي د_UPARR_مجموعه_OBJ_V(vthread_t thr, vvp_کوډ_t cp)
{
      بینښه wid = cp->بټ_idx[0];
      vvp_object_t src_obj;
      thr->راوباسه_object(src_obj);

      vvp_کتار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_کتار_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_vector4_t مجموعه =
	    qsrc ? uparr_مجموعه_ټکي_src(qsrc, wid)
		 : uparr_مجموعه_ټکي_src(dsrc, wid);
      thr->ټېل_vec4(مجموعه);
      أرجع ریښتیا;
}

/* xor() reduction returns the scalar sum (Verilog-style bitwise-XOR per word). */
کينډۍ <صنف SRC>
ساكن vvp_vector4_t uparr_xor_ټکي_src(SRC* src, بینښه wid)
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

منطقي د_UPARR_XOR_V(vthread_t thr, vvp_کوډ_t cp)
{
      vvp_net_t* net = cp->net;
      بینښه wid = cp->بټ_idx[0];
      vvp_کتار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_کتار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      إذا (qsrc) res = uparr_xor_ټکي_src(qsrc, wid);
      وإلا إذا (dsrc) res = uparr_xor_ټکي_src(dsrc, wid);
      وإلا {
	    vvp_کتار* src_q = get_کتار_object<vvp_کتار_vec4>(thr, net);
	    vvp_کتار_vec4* src = dynamic_cast<vvp_کتار_vec4*>(src_q);
	    res = uparr_xor_ټکي_src(src, wid);
      }
      thr->ټېل_vec4(res);
      أرجع ریښتیا;
}

منطقي د_VPI_CALL(vthread_t thr, vvp_کوډ_t cp)
{
      vpip_execute_vpi_call(thr, cp->handle);

      إذا (مهالوېش_stopped()) {
	    إذا (! مهالوېش_پای_ته_رسېدلی())
		  مهالوېش_vthread(thr, 0, خطأ);

	    أرجع خطأ;
      }

      أرجع مهالوېش_پای_ته_رسېدلی()? خطأ : ریښتیا;
}

/* %wait <label>;
 * Implement the wait by locating the vvp_net_T for the event, and
 * adding this thread to the threads list for the event. The some
 * argument is the  reference to the functor to wait for. This must be
 * an event object of some sort.
 */
منطقي د_WAIT(vthread_t thr, vvp_کوډ_t cp)
{
      أكد(! thr->i_am_دننه_function);
      أكد(! thr->انتظار_for_event);
      thr->انتظار_for_event = 1;

	/* Add this thread to the list in the event. */
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (cp->net->fun);
      أكد(ep);
      thr->wait_بل = ep->جمع_کړه_انتظار_thread(thr);

	/* Return false to suspend this thread. */
      أرجع خطأ;
}

/*
 * Implement the %wait/fork (SystemVerilog) instruction by suspending
 * the current thread until all the detached children have finished.
 */
منطقي د_WAIT_FORK(vthread_t thr, vvp_کوډ_t)
{
	/* If a %wait/fork is being executed then the parent thread
	 * cannot be waiting in a join or already waiting. */
      أكد(! thr->i_am_دننه_function);
      أكد(! thr->i_am_joining);
      أكد(! thr->i_am_انتظار);

	/* There should be no active children when waiting. */
      أكد(thr->children.empty());

	/* If there are no detached children then there is nothing to
	 * wait for. */
      إذا (thr->detached_children.empty()) أرجع ریښتیا;

	/* Flag that this process is waiting for the detached children
	 * to finish and suspend it. */
      thr->i_am_انتظار = 1;
      أرجع خطأ;
}

/*
 * %xnor
 */
منطقي د_XNOR(vthread_t thr, vvp_کوډ_t)
{
      vvp_vector4_t valr = thr->راوباسه_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      أكد(vall.size() == valr.size());

      vall ^= valr;
      vall.invert();

      أرجع ریښتیا;
}

/*
 * %xor
 */
منطقي د_XOR(vthread_t thr, vvp_کوډ_t)
{
      vvp_vector4_t valr = thr->راوباسه_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      أكد(vall.size() == valr.size());

      vall ^= valr;

      أرجع ریښتیا;
}


منطقي د_ZOMBIE(vthread_t thr, vvp_کوډ_t)
{
      thr->pc = codespace_null();
      إذا ((thr->پلار == 0) && (thr->children.empty())) {
	    إذا (thr->delay_delete)
		  مهالوېش_del_thr(thr);
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
ساكن منطقي do_exec_ufunc(vthread_t thr, vvp_کوډ_t cp, vthread_t ماشوم)
{
      __vpiScope*ماشوم_scope = cp->ufunc_core_ptr->func_scope();
      أكد(ماشوم_scope);

      أكد(ماشوم_scope->get_type_کوډ() == vpiFunction);
      أكد(thr->children.empty());


        /* We can take a number of shortcuts because we know that a
           continuous assignment can only occur in a static scope. */
      أكد(thr->wt_context == 0);
      أكد(thr->rd_context == 0);

        /* If an automatic function, allocate a context for this call. */
      vvp_context_t ماشوم_context = 0;
      إذا (ماشوم_scope->is_automatic()) {
            ماشوم_context = vthread_alloc_context(ماشوم_scope);
            thr->wt_context = ماشوم_context;
            thr->rd_context = ماشوم_context;
      }

      ماشوم->wt_context = ماشوم_context;
      ماشوم->rd_context = ماشوم_context;

	/* Copy all the inputs to the ufunc object to the port
	   variables of the function. This copies all the values
	   atomically. */
      cp->ufunc_core_ptr->ټاکل_بټونه_منزل_ports(ماشوم_context);
      ماشوم->delay_delete = 1;

      ماشوم->پلار = thr;
      thr->children.insert(ماشوم);
	// This should be the only child
      أكد(thr->children.size()==1);

      ماشوم->is_scheduled = 1;
      ماشوم->i_am_دننه_function = 1;
      vthread_پرلپسې(ماشوم);
      running_thread = thr;

      إذا (ماشوم->i_have_ended) {
	    do_join(thr, ماشوم);
            أرجع ریښتیا;
      } وإلا {
	    thr->i_am_joining = 1;
	    أرجع خطأ;
      }
}

منطقي د_EXEC_UFUNC_REAL(vthread_t thr, vvp_کوډ_t cp)
{
      __vpiScope*ماشوم_scope = cp->ufunc_core_ptr->func_scope();
      أكد(ماشوم_scope);

	/* Create a temporary thread and run it immediately. */
      vthread_t ماشوم = vthread_new(cp->cptr, ماشوم_scope);
      thr->ټېل_real(0.0);
      ماشوم->دلیلونه_real.push_back(0);

      أرجع do_exec_ufunc(thr, cp, ماشوم);
}

منطقي د_EXEC_UFUNC_VEC4(vthread_t thr, vvp_کوډ_t cp)
{
      __vpiScope*ماشوم_scope = cp->ufunc_core_ptr->func_scope();
      أكد(ماشوم_scope);

      vpiScopeFunction*scope_func = dynamic_cast<vpiScopeFunction*>(ماشوم_scope);
      أكد(scope_func);

	/* Create a temporary thread and run it immediately. */
      vthread_t ماشوم = vthread_new(cp->cptr, ماشوم_scope);
      thr->ټېل_vec4(vvp_vector4_t(scope_func->get_func_پلنوالی(), scope_func->get_func_init_val()));
      ماشوم->دلیلونه_vec4.push_back(0);

      أرجع do_exec_ufunc(thr, cp, ماشوم);
}

/*
 * This is a phantom opcode used to harvest the result of calling a user
 * defined function. It is used in code generated by the .ufunc statement.
 */
منطقي د_REAP_UFUNC(vthread_t thr, vvp_کوډ_t cp)
{
      __vpiScope*ماشوم_scope = cp->ufunc_core_ptr->func_scope();
      أكد(ماشوم_scope);

	/* Copy the output from the result variable to the output
	   ports of the .ufunc device. */
      cp->ufunc_core_ptr->پای_thread();

        /* If an automatic function, free the context for this call. */
      إذا (ماشوم_scope->is_automatic()) {
            vthread_free_context(thr->rd_context, ماشوم_scope);
            thr->wt_context = 0;
            thr->rd_context = 0;
      }

      أرجع ریښتیا;
}
