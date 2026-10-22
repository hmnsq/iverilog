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
#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#جيڪڏهن_پورو
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

استعمال نطاق std;

/* This is the size of an unsigned long in bits. This is just a
   convenience macro. */
# define CPU_لفظ_بٽ (8*حجم(بنانشان ڊگهو))
# define چوٽي_ٻٽو_انگ (1UL << (CPU_لفظ_بٽ-1))

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
      vvp_ڪوڊ_t pc;
	/* These hold the private thread bits. */
      تعداد { FLAGS_ڳڻپ = 512, لفظ_ڳڻپ = 16 };
      vvp_bit4_t flags[FLAGS_ڳڻپ];

	/* These are the word registers. */
      اتحاد {
	    int64_t  w_int;
	    uint64_t w_uint;
      } لفظ[لفظ_ڳڻپ];

	// These vectors are depths within the parent thread's
	// corresponding stack.  This is how the %ret/* instructions
	// get at parent thread arguments.
      vector<بنانشان> دليلون_real;
      vector<بنانشان> دليلون_str;
      vector<بنانشان> دليلون_vec4;

    خاص:
      vector<vvp_vector4_t>ڍير_vec4;
    عام:
      مضمن vvp_vector4_t ڪڍو_vec4(فراغ)
      {
	    أكد(! ڍير_vec4.empty());
	    vvp_vector4_t val = ڍير_vec4.back();
	    ڍير_vec4.pop_back();
	    أرجع val;
      }
      مضمن فراغ ڌڪو_vec4(ثابت vvp_vector4_t&val)
      {
	    ڍير_vec4.push_back(val);
      }
      مضمن ثابت vvp_vector4_t& peek_vec4(بنانشان اونهائي)
      {
	    بنانشان size = ڍير_vec4.size();
	    أكد(اونهائي < size);
	    بنانشان use_اشاريو = size-1-اونهائي;
	    أرجع ڍير_vec4[use_اشاريو];
      }
      مضمن vvp_vector4_t& peek_vec4(فراغ)
      {
	    بنانشان use_اشاريو = ڍير_vec4.size();
	    أكد(use_اشاريو >= 1);
	    أرجع ڍير_vec4[use_اشاريو-1];
      }
      مضمن فراغ poke_vec4(بنانشان اونهائي, ثابت vvp_vector4_t&val)
      {
	    أكد(اونهائي < ڍير_vec4.size());
	    بنانشان use_اشاريو = ڍير_vec4.size()-1-اونهائي;
	    ڍير_vec4[use_اشاريو] = val;
      }
      مضمن فراغ ڪڍو_vec4(بنانشان cnt)
      {
	    طالما (cnt > 0) {
		  ڍير_vec4.pop_back();
		  cnt -= 1;
	    }
      }


    خاص:
      vector<مزدوج> ڍير_real;
    عام:
      مضمن مزدوج ڪڍو_real(فراغ)
      {
	    أكد(! ڍير_real.empty());
	    مزدوج val = ڍير_real.back();
	    ڍير_real.pop_back();
	    أرجع val;
      }
      مضمن فراغ ڌڪو_real(مزدوج val)
      {
	    ڍير_real.push_back(val);
      }
      مضمن مزدوج peek_real(بنانشان اونهائي)
      {
	    أكد(اونهائي < ڍير_real.size());
	    بنانشان use_اشاريو = ڍير_real.size()-1-اونهائي;
	    أرجع ڍير_real[use_اشاريو];
      }
      مضمن فراغ poke_real(بنانشان اونهائي, مزدوج val)
      {
	    أكد(اونهائي < ڍير_real.size());
	    بنانشان use_اشاريو = ڍير_real.size()-1-اونهائي;
	    ڍير_real[use_اشاريو] = val;
      }
      مضمن فراغ ڪڍو_real(بنانشان cnt)
      {
	    طالما (cnt > 0) {
		  ڍير_real.pop_back();
		  cnt -= 1;
	    }
      }

	/* Strings are operated on using a forth-like operator
	   set. Items at the top of the stack (back()) are the objects
	   operated on except for special cases. New objects are
	   pushed onto the top (back()) and pulled from the top
	   (back()) only. */
    خاص:
      vector<string> ڍير_str;
    عام:
      مضمن string ڪڍو_str(فراغ)
      {
	    أكد(! ڍير_str.empty());
	    string val = ڍير_str.back();
	    ڍير_str.pop_back();
	    أرجع val;
      }
      مضمن فراغ ڌڪو_str(ثابت string&val)
      {
	    ڍير_str.push_back(val);
      }
      مضمن string&peek_str(بنانشان اونهائي)
      {
	    أكد(اونهائي<ڍير_str.size());
	    بنانشان use_اشاريو = ڍير_str.size()-1-اونهائي;
	    أرجع ڍير_str[use_اشاريو];
      }
      مضمن فراغ poke_str(بنانشان اونهائي, ثابت string&val)
      {
	    أكد(اونهائي < ڍير_str.size());
	    بنانشان use_اشاريو = ڍير_str.size()-1-اونهائي;
	    ڍير_str[use_اشاريو] = val;
      }
      مضمن فراغ ڪڍو_str(بنانشان cnt)
      {
	    طالما (cnt > 0) {
		  ڍير_str.pop_back();
		  cnt -= 1;
	    }
      }

	/* Objects are also operated on in a stack. */
    خاص:
      تعداد { ڍير_OBJ_MAX_سائيز = 32 };
      vvp_object_t ڍير_obj[ڍير_OBJ_MAX_سائيز];
      بنانشان ڍير_obj_سائيز;
    عام:
      مضمن vvp_object_t& peek_object(فراغ)
      {
	    أكد(ڍير_obj_سائيز > 0);
	    أرجع ڍير_obj[ڍير_obj_سائيز-1];
      }
      مضمن فراغ ڪڍو_object(vvp_object_t&obj)
      {
	    أكد(ڍير_obj_سائيز > 0);
	    ڍير_obj_سائيز -= 1;
	    obj = ڍير_obj[ڍير_obj_سائيز];
	    ڍير_obj[ڍير_obj_سائيز].reset(0);
      }
      مضمن فراغ ڪڍو_object(بنانشان cnt, بنانشان skip =0)
      {
	    أكد((cnt+skip) <= ڍير_obj_سائيز);
	    لكل (size_t idx = ڍير_obj_سائيز-skip-cnt ; idx < ڍير_obj_سائيز-skip ; idx += 1)
		  ڍير_obj[idx].reset(0);
	    ڍير_obj_سائيز -= cnt;
	    لكل (size_t idx = ڍير_obj_سائيز-skip ; idx < ڍير_obj_سائيز ; idx += 1)
		  ڍير_obj[idx] = ڍير_obj[idx+skip];
	    لكل (size_t idx = ڍير_obj_سائيز ; idx < ڍير_obj_سائيز+skip ; idx += 1)
		  ڍير_obj[idx].reset(0);
      }
      مضمن فراغ ڌڪو_object(ثابت vvp_object_t&obj)
      {
	    أكد(ڍير_obj_سائيز < ڍير_OBJ_MAX_سائيز);
	    ڍير_obj[ڍير_obj_سائيز] = obj;
	    ڍير_obj_سائيز += 1;
      }

	/* My parent sets this when it wants me to wake it up. */
      بنانشان i_am_joining      :1;
      بنانشان i_am_detached     :1;
      بنانشان i_am_انتظار      :1;
      بنانشان i_am_اندريون_function  :1; // True if running function code
      بنانشان i_have_ended      :1;
      بنانشان i_was_disabled    :1;
      بنانشان انتظار_for_event :1;
      بنانشان is_scheduled      :1;
      بنانشان delay_delete      :1;
	/* This points to the children of the thread. */
      سيٽ<بنية vthread_s*>children;
	/* This points to the detached children of the thread. */
      سيٽ<بنية vthread_s*>detached_children;
	/* This points to my parent, if I have one. */
      بنية vthread_s*والد;
	/* This points to the containing scope. */
      __vpiScope*والد_scope;
	/* This is used for keeping wait queues. */
      بنية vthread_s*wait_ايندڙ;
	/* These are used to access automatically allocated items. */
      vvp_context_t wt_context, rd_context;
	/* These are used to pass non-blocking event control information. */
      vvp_net_t*event;
      uint64_t ecount;
	/* Save the file/line information when available. */
    خاص:
      حرف *filenm_;
      بنانشان lineno_;
    عام:
      فراغ سيٽ_fileline(حرف *filenm, بنانشان lineno);
      string get_fileline();

      مضمن فراغ cleanup()
      {
	    إذا (i_was_disabled) {
		  ڍير_vec4.clear();
		  ڍير_real.clear();
		  ڍير_str.clear();
		  ڪڍو_object(ڍير_obj_سائيز);
	    }
	    حرر(filenm_);
	    filenm_ = 0;
	    أكد(ڍير_vec4.empty());
	    أكد(ڍير_real.empty());
	    أكد(ڍير_str.empty());
	    أكد(ڍير_obj_سائيز == 0);
      }
};

مضمن vthread_s::vthread_s()
{
      ڍير_obj_سائيز = 0;
      filenm_ = 0;
      lineno_ = 0;
}

فراغ vthread_s::سيٽ_fileline(حرف *filenm, بنانشان lineno)
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
      fd << "**** " << label << سٽ_پڄاڻي;
      fd << "**** ThreadId: " << هذا << ", parent id: " << والد << سٽ_پڄاڻي;

      fd << "**** Flags: ";
      لكل (صحيح idx = 0 ; idx < FLAGS_ڳڻپ ; idx += 1)
	    fd << flags[idx];
      fd << سٽ_پڄاڻي;
      fd << "**** vec4 stack..." << سٽ_پڄاڻي;
      لكل (size_t idx = ڍير_vec4.size() ; idx > 0 ; idx -= 1)
	    fd << "    " << (ڍير_vec4.size()-idx) << ": " << ڍير_vec4[idx-1] << سٽ_پڄاڻي;
      fd << "**** str stack (" << ڍير_str.size() << ")..." << سٽ_پڄاڻي;
      fd << "**** obj stack (" << ڍير_obj_سائيز << ")..." << سٽ_پڄاڻي;
      fd << "**** args_vec4 array (" << دليلون_vec4.size() << ")..." << سٽ_پڄاڻي;
      لكل (size_t idx = 0 ; idx < دليلون_vec4.size() ; idx += 1)
	    fd << "    " << idx << ": " << دليلون_vec4[idx] << سٽ_پڄاڻي;
      fd << "**** file/line (";
      إذا (filenm_) fd << filenm_;
      وإلا fd << "<no file name>";
      fd << ":" << lineno_ << ")" << سٽ_پڄاڻي;
      fd << "**** Done ****" << سٽ_پڄاڻي;
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
      vector<حرف> tmp (لڙي_ڊيگهه(متن)+1);
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

ساكن فراغ do_join(vthread_t thr, vthread_t ٻار);

__vpiScope* vthread_scope(بنية vthread_s*thr)
{
      أرجع thr->والد_scope;
}

بنية vthread_s*running_thread = 0;

string get_fileline()
{
      أرجع running_thread->get_fileline();
}

فراغ vthread_ڌڪو(بنية vthread_s*thr, مزدوج val)
{
      thr->ڌڪو_real(val);
}

فراغ vthread_ڌڪو(بنية vthread_s*thr, ثابت string&val)
{
      thr->ڌڪو_str(val);
}

فراغ vthread_ڌڪو(بنية vthread_s*thr, ثابت vvp_vector4_t&val)
{
      thr->ڌڪو_vec4(val);
}

فراغ vthread_ڪڍو_real(بنية vthread_s*thr, بنانشان اونهائي)
{
      thr->ڪڍو_real(اونهائي);
}

فراغ vthread_ڪڍو_str(بنية vthread_s*thr, بنانشان اونهائي)
{
      thr->ڪڍو_str(اونهائي);
}

فراغ vthread_ڪڍو_vec4(بنية vthread_s*thr, بنانشان اونهائي)
{
      thr->ڪڍو_vec4(اونهائي);
}

مزدوج vthread_get_real_ڍير(بنية vthread_s*thr, بنانشان اونهائي)
{
      أرجع thr->peek_real(اونهائي);
}

ثابت string&vthread_get_str_ڍير(بنية vthread_s*thr, بنانشان اونهائي)
{
      أرجع thr->peek_str(اونهائي);
}

ثابت vvp_vector4_t& vthread_get_vec4_ڍير(بنية vthread_s*thr, بنانشان اونهائي)
{
      أرجع thr->peek_vec4(اونهائي);
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
سانچو <صنف VVP_قطار> ساكن vvp_قطار*get_قطار_object(vthread_t thr, vvp_net_t*net)
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
      vvp_object_t چوٽي = obj->get_object();
      qsrc = چوٽي.peek<vvp_قطار_vec4>();
      إذا (qsrc) {
	    dsrc = 0;
	    أرجع;
      }
      vvp_darray* dany = چوٽي.peek<vvp_darray>();
      إذا (dany && dynamic_cast<vvp_قطار*>(dany) == 0) {
	    dsrc = dany;
	    أرجع;
      }
      إذا (چوٽي.test_nil()) {
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
      vvp_object_t چوٽي = obj->get_object();
      qsrc = چوٽي.peek<vvp_قطار_real>();
      إذا (qsrc) {
	    dsrc = 0;
	    أرجع;
      }
      vvp_darray* dany = چوٽي.peek<vvp_darray>();
      إذا (dany && dynamic_cast<vvp_قطار*>(dany) == 0) {
	    dsrc = dany;
	    أرجع;
      }
      إذا (چوٽي.test_nil()) {
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
مضمن ساكن فراغ ڪڍو_قدر(vthread_t thr, مزدوج&value, بنانشان)
{
      value = thr->ڪڍو_real();
}

مضمن ساكن فراغ ڪڍو_قدر(vthread_t thr, string&value, بنانشان)
{
      value = thr->ڪڍو_str();
}

مضمن ساكن فراغ ڪڍو_قدر(vthread_t thr, vvp_vector4_t&value, بنانشان wid)
{
      value = thr->ڪڍو_vec4();
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
مضمن ساكن فراغ dq_default(مزدوج&value, بنانشان)
{
      value = 0.0;
}

مضمن ساكن فراغ dq_default(string&value, بنانشان)
{
      value = "";
}

مضمن ساكن فراغ dq_default(vvp_vector4_t&value, بنانشان wid)
{
      value = vvp_vector4_t(wid);
}


سانچو <صنف T> T coerce_منزل_ويڪر(ثابت T&that, بنانشان width)
{
      إذا (that.size() == width)
	    أرجع that;

      أكد(that.size() > width);
      T res (width);
      لكل (بنانشان idx = 0 ;  idx < width ;  idx += 1)
	    res.سيٽ_ٻٽو_انگ(idx, that.value(idx));

      أرجع res;
}

/* Explicitly define the vvp_vector4_t version of coerce_to_width(). */
سانچو vvp_vector4_t coerce_منزل_ويڪر(ثابت vvp_vector4_t&that,
                                       بنانشان width);


ساكن فراغ multiply_array_imm(بنانشان ڊگهو*res, ثابت بنانشان ڊگهو*val,
			       بنانشان لفظ, بنانشان ڊگهو imm)
{
      لكل (بنانشان idx = 0 ; idx < لفظ ; idx += 1)
	    res[idx] = 0;

      لكل (بنانشان mul_idx = 0 ; mul_idx < لفظ ; mul_idx += 1) {
	    بنانشان ڊگهو جوڙ;
	    بنانشان ڊگهو tmp = multiply_with_حاصل(val[mul_idx], imm, جوڙ);

	    بنانشان ڊگهو حاصل = 0;
	    res[mul_idx] = جوڙيو_with_حاصل(res[mul_idx], tmp, حاصل);
	    لكل (بنانشان جوڙيو_idx = mul_idx+1 ; جوڙيو_idx < لفظ ; جوڙيو_idx += 1) {
		  res[جوڙيو_idx] = جوڙيو_with_حاصل(res[جوڙيو_idx], جوڙ, حاصل);
		  جوڙ = 0;
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
            scope->free_contexts = vvp_get_ايندڙ_context(context);
            لكل (بنانشان idx = 0 ; idx < scope->nitem ; idx += 1) {
                  scope->شي[idx]->reset_instance(context);
            }
      } وإلا {
            context = vvp_allocate_context(scope->nitem);
            لكل (بنانشان idx = 0 ; idx < scope->nitem ; idx += 1) {
                  scope->شي[idx]->alloc_instance(context);
            }
      }

      vvp_سيٽ_ايندڙ_context(context, scope->جيئرو_contexts);
      scope->جيئرو_contexts = context;

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

      إذا (context == scope->جيئرو_contexts) {
            scope->جيئرو_contexts = vvp_get_ايندڙ_context(context);
      } وإلا {
            vvp_context_t tmp = scope->جيئرو_contexts;
            طالما (context != vvp_get_ايندڙ_context(tmp)) {
                  أكد(tmp);
                  tmp = vvp_get_ايندڙ_context(tmp);
            }
            vvp_سيٽ_ايندڙ_context(tmp, vvp_get_ايندڙ_context(context));
      }

      vvp_سيٽ_ايندڙ_context(context, scope->free_contexts);
      scope->free_contexts = context;
}

#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
فراغ contexts_delete(صنف __vpiScope*scope)
{
      vvp_context_t context = scope->free_contexts;

      طالما (context) {
	    scope->free_contexts = vvp_get_ايندڙ_context(context);
	    لكل (بنانشان idx = 0; idx < scope->nitem; idx += 1) {
		  scope->شي[idx]->free_instance(context);
	    }
	    حرر(context);
	    context = scope->free_contexts;
      }
      حرر(scope->شي);
}
#جيڪڏهن_پورو

/*
 * Create a new thread with the given start address.
 */
vthread_t vthread_new(vvp_ڪوڊ_t pc, __vpiScope*scope)
{
      vthread_t thr = جديد بنية vthread_s;
      thr->pc     = pc;
	//thr->bits4  = vvp_vector4_t(32);
      thr->والد = 0;
      thr->والد_scope = scope;
      thr->wait_ايندڙ = 0;
      thr->wt_context = 0;
      thr->rd_context = 0;

      thr->i_am_joining  = 0;
      thr->i_am_detached = 0;
      thr->i_am_انتظار  = 0;
      thr->i_am_اندريون_function = 0;
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

      scope->ڌاڳا .insert(thr);
      أرجع thr;
}

#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
#إذا 0
/*
 * These are not currently correct. If you use them you will get
 * double delete messages. There is still a leak related to a
 * waiting event that needs to be investigated.
 */

ساكن فراغ wait_ايندڙ_delete(vthread_t بنياد)
{
      طالما (بنياد) {
	    vthread_t tmp = بنياد->wait_ايندڙ;
	    ميٽيو بنياد;
	    بنياد = tmp;
	    إذا (بنياد->انتظار_for_event == 0) اكسر;
      }
}

ساكن فراغ ٻار_delete(vthread_t بنياد)
{
      طالما (بنياد) {
	    vthread_t tmp = بنياد->ٻار;
	    ميٽيو بنياد;
	    بنياد = tmp;
      }
}
#جيڪڏهن_پورو

فراغ vthreads_delete(صنف __vpiScope*scope)
{
      لكل (std::سيٽ<vthread_t>::iterator cur = scope->ڌاڳا.begin()
		 ; cur != scope->ڌاڳا.پڇاڙي() ; ++ cur ) {
	    ميٽيو *cur;
      }
      scope->ڌاڳا.clear();
}
#جيڪڏهن_پورو

/*
 * Reaping pulls the thread out of the stack of threads. If I have a
 * child, then hand it over to my parent or fully detach it.
 */
ساكن فراغ vthread_reap(vthread_t thr)
{
      إذا (! thr->children.empty()) {
	    لكل (سيٽ<vthread_t>::iterator cur = thr->children.begin()
		       ; cur != thr->children.پڇاڙي() ; ++cur) {
		  vthread_t ٻار = *cur;
		  أكد(ٻار);
		  أكد(ٻار->والد == thr);
		  ٻار->والد = thr->والد;
	    }
      }
      إذا (! thr->detached_children.empty()) {
	    لكل (سيٽ<vthread_t>::iterator cur = thr->detached_children.begin()
		       ; cur != thr->detached_children.پڇاڙي() ; ++cur) {
		  vthread_t ٻار = *cur;
		  أكد(ٻار);
		  أكد(ٻار->والد == thr);
		  أكد(ٻار->i_am_detached);
		  ٻار->والد = 0;
		  ٻار->i_am_detached = 0;
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
      thr->والد_scope->ڌاڳا.erase(thr);

      thr->pc = codespace_null();

	/* If this thread is not scheduled, then is it safe to delete
	   it now. Otherwise, let the schedule event (which will
	   execute the thread at of_ZOMBIE) delete the object. */
      إذا ((thr->is_scheduled == 0) && (thr->انتظار_for_event == 0)) {
	    أكد(thr->children.empty());
	    أكد(thr->wait_ايندڙ == 0);
	    إذا (thr->delay_delete)
		  وقت_جدول_del_thr(thr);
	    وإلا
		  vthread_delete(thr);
      }
}

فراغ vthread_delete(vthread_t thr)
{
      thr->cleanup();
      ميٽيو thr;
}

فراغ vthread_mark_scheduled(vthread_t thr)
{
      طالما (thr != 0) {
	    أكد(thr->is_scheduled == 0);
	    thr->is_scheduled = 1;
	    thr = thr->wait_ايندڙ;
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
	    thr->i_am_اندريون_function = 1;
	    thr = thr->wait_ايندڙ;
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
فراغ vthread_لاڳيتو(vthread_t thr)
{
      طالما (thr != 0) {
	    vthread_t tmp = thr->wait_ايندڙ;
	    thr->wait_ايندڙ = 0;

	    أكد(thr->is_scheduled);
	    thr->is_scheduled = 0;

            running_thread = thr;

	    لكل (;;) {
		  vvp_ڪوڊ_t cp = thr->pc;
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
منطقي جو_CHUNK_LINK(vthread_t thr, vvp_ڪوڊ_t code)
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
فراغ vthread_وقت_جدول_list(vthread_t thr)
{
      لكل (vthread_t cur = thr ;  cur ;  cur = cur->wait_ايندڙ) {
	    أكد(cur->انتظار_for_event);
	    cur->انتظار_for_event = 0;
      }

      وقت_جدول_vthread(thr, 0);
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

vvp_context_شي_t vthread_get_wt_context_شي(بنانشان context_idx)
{
      أكد(running_thread && running_thread->wt_context);
      أرجع vvp_get_context_شي(running_thread->wt_context, context_idx);
}

vvp_context_شي_t vthread_get_rd_context_شي(بنانشان context_idx)
{
      أكد(running_thread && running_thread->rd_context);
      أرجع vvp_get_context_شي(running_thread->rd_context, context_idx);
}

/*
 * %abs/wr
 */
منطقي جو_ABS_WR(vthread_t thr, vvp_ڪوڊ_t)
{
      thr->ڌڪو_real( fabs(thr->ڪڍو_real()) );
      أرجع سچ;
}

منطقي جو_ALLOC(vthread_t thr, vvp_ڪوڊ_t cp)
{
        /* Allocate a context. */
      vvp_context_t ٻار_context = vthread_alloc_context(cp->scope);

        /* Push the allocated context onto the write context stack. */
      vvp_سيٽ_stacked_context(ٻار_context, thr->wt_context);
      thr->wt_context = ٻار_context;

      أرجع سچ;
}

منطقي جو_AND(vthread_t thr, vvp_ڪوڊ_t)
{
      vvp_vector4_t valb = thr->ڪڍو_vec4();
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
ساكن فراغ get_فوري_rval(vvp_ڪوڊ_t cp, vvp_vector4_t&val)
{
      uint32_t vala = cp->ٻٽو_انگ_idx[0];
      uint32_t valb = cp->ٻٽو_انگ_idx[1];
      بنانشان wid  = cp->عدد;

      إذا (valb == 0) {
	      // Special case: if the value is zero, we are done
	      // before we start.
	    إذا (vala == 0) أرجع;

	      // Special case: The value has no X/Z bits, so we can
	      // use the setarray method to write the value all at once.
	    بنانشان use_wid = 8*حجم(بنانشان ڊگهو);
	    إذا (wid < use_wid)
		  use_wid = wid;
	    بنانشان ڊگهو tmp[1];
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

      لكل (بنانشان idx = 0 ; idx < wid && (vala|valb) ; idx += 1) {
	    uint32_t ba = 0;
	      // Convert the vala/valb bits to a ba number that
	      // matches the encoding of the vvp_bit4_t enumeration.
	    ba = (valb & 1) << 1;
	    ba |= vala & 1;

	      // Note that the val is already pre-filled with BIT4_0
	      // bits, os we only need to set non-zero bit values.
	    إذا (ba) val.سيٽ_ٻٽو_انگ(idx, (vvp_bit4_t)ba);

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
منطقي جو_جوڙيو(vthread_t thr, vvp_ڪوڊ_t)
{
      vvp_vector4_t r = thr->ڪڍو_vec4();
	// Rather then pop l, use it directly from the stack. When we
	// assign to 'l', that will edit the top of the stack, which
	// replaces a pop and a pull.
      vvp_vector4_t&l = thr->peek_vec4();

      l.جوڙيو(r);

      أرجع سچ;
}

/*
 * %addi <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
منطقي جو_ADDI(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان wid = cp->عدد;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_فوري_rval (cp, r);

      l.جوڙيو(r);

      أرجع سچ;
}

/*
 * %add/wr
 */
منطقي جو_جوڙيو_WR(vthread_t thr, vvp_ڪوڊ_t)
{
      مزدوج r = thr->ڪڍو_real();
      مزدوج l = thr->ڪڍو_real();
      thr->ڌڪو_real(l + r);
      أرجع سچ;
}

/* %assign/ar <array>, <delay>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory. <delay>
 * is the delay in simulation time. <bit> is the index register
 * containing the real value.
 */
منطقي جو_سونپيو_AR(vthread_t thr, vvp_ڪوڊ_t cp)
{
      ڊگهو adr = thr->لفظ[3].w_int;
      بنانشان delay = cp->ٻٽو_انگ_idx[0];
      مزدوج value = thr->ڪڍو_real();

      إذا (adr >= 0) {
	    وقت_جدول_سونپيو_array_لفظ(cp->array, adr, value, delay);
      }

      أرجع سچ;
}

/* %assign/ar/d <array>, <delay_idx>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory.
 * <delay_idx> is the integer register that contains the delay value.
 */
منطقي جو_سونپيو_ARD(vthread_t thr, vvp_ڪوڊ_t cp)
{
      ڊگهو adr = thr->لفظ[3].w_int;
      مزدوج value = thr->ڪڍو_real();

      إذا (adr >= 0) {
	    vvp_time64_t delay = thr->لفظ[cp->ٻٽو_انگ_idx[0]].w_uint;
	    وقت_جدول_سونپيو_array_لفظ(cp->array, adr, value, delay);
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
منطقي جو_سونپيو_ARE(vthread_t thr, vvp_ڪوڊ_t cp)
{
      ڊگهو adr = thr->لفظ[3].w_int;
      مزدوج value = thr->ڪڍو_real();

      إذا (adr >= 0) {
	    إذا (thr->ecount == 0) {
		  وقت_جدول_سونپيو_array_لفظ(cp->array, adr, value, 0);
	    } وإلا {
		  وقت_جدول_evctl(cp->array, adr, value, thr->event,
		                 thr->ecount);
	    }
      }

      أرجع سچ;
}

/*
 * %assign/vec4 <var>, <delay>
 */
منطقي جو_سونپيو_VEC4(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      بنانشان delay = cp->ٻٽو_انگ_idx[0];
      ثابت vvp_vector4_t&val = thr->peek_vec4();

      وقت_جدول_سونپيو_ويڪٽر(ptr, 0, 0, val, delay);
      thr->ڪڍو_vec4(1);
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
			    بنانشان صحيح sig_wid)
{
      بنانشان صحيح wid = val.size();

        // Fully in bounds, most likely case
      إذا (off >= 0 && (uint64_t)off + wid <= sig_wid)
	    أرجع سچ;

      بنانشان صحيح بنياد = 0;
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
	    بنياد = -off;
	    wid += off;
		off = 0;
      }

	// If the value is partly above the target, then only assign
	// the bits that overlap
      إذا ((uint64_t)off + wid > sig_wid)
	    wid = sig_wid - (uint64_t)off;

      val = val.subvalue(بنياد, wid);

      أرجع سچ;
}

/*
 * %assign/vec4/a/d <arr>, <offx>, <delx>
 */
منطقي جو_سونپيو_VEC4_A_D(vthread_t thr, vvp_ڪوڊ_t cp)
{
      صحيح off_idx = cp->ٻٽو_انگ_idx[0];
      صحيح del_idx = cp->ٻٽو_انگ_idx[1];
      صحيح adr_idx = 3;

      int64_t  off = off_idx ? thr->لفظ[off_idx].w_int : 0;
      vvp_time64_t del = del_idx? thr->لفظ[del_idx].w_uint : 0;
      ڊگهو     adr = thr->لفظ[adr_idx].w_int;

      vvp_vector4_t val = thr->ڪڍو_vec4();

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      إذا (thr->flags[4] == BIT4_1)
	    أرجع سچ;

      إذا (!resize_rval_vec(val, off, cp->array->get_لفظ_سائيز()))
	    أرجع سچ;

      وقت_جدول_سونپيو_array_لفظ(cp->array, adr, off, val, del);

      أرجع سچ;
}

/*
 * %assign/vec4/a/e <arr>, <offx>
 */
منطقي جو_سونپيو_VEC4_A_E(vthread_t thr, vvp_ڪوڊ_t cp)
{
      صحيح off_idx = cp->ٻٽو_انگ_idx[0];
      صحيح adr_idx = 3;

      int64_t  off = off_idx ? thr->لفظ[off_idx].w_int : 0;
      ڊگهو     adr = thr->لفظ[adr_idx].w_int;

      vvp_vector4_t val = thr->ڪڍو_vec4();

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      إذا (thr->flags[4] == BIT4_1)
	    أرجع سچ;

      إذا (!resize_rval_vec(val, off, cp->array->get_لفظ_سائيز()))
	    أرجع سچ;

      إذا (thr->ecount == 0) {
	    وقت_جدول_سونپيو_array_لفظ(cp->array, adr, off, val, 0);
      } وإلا {
	    وقت_جدول_evctl(cp->array, adr, val, off, thr->event, thr->ecount);
      }

      أرجع سچ;
}

/*
 * %assign/vec4/off/d <var>, <off>, <del>
 */
منطقي جو_سونپيو_VEC4_OFF_D(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      بنانشان off_اشاريو = cp->ٻٽو_انگ_idx[0];
      بنانشان del_اشاريو = cp->ٻٽو_انگ_idx[1];
      vvp_vector4_t val = thr->ڪڍو_vec4();

      int64_t off = thr->لفظ[off_اشاريو].w_int;
      vvp_time64_t del = thr->لفظ[del_اشاريو].w_uint;

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      إذا (thr->flags[4] == BIT4_1)
	    أرجع سچ;

      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*> (cp->net->fil);
      أكد(sig);

      إذا (!resize_rval_vec(val, off, sig->قدر_سائيز()))
	    أرجع سچ;

      وقت_جدول_سونپيو_ويڪٽر(ptr, off, sig->قدر_سائيز(), val, del);
      أرجع سچ;
}

/*
 * %assign/vec4/off/e <var>, <off>
 */
منطقي جو_سونپيو_VEC4_OFF_E(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      بنانشان off_اشاريو = cp->ٻٽو_انگ_idx[0];
      vvp_vector4_t val = thr->ڪڍو_vec4();

      int64_t off = thr->لفظ[off_اشاريو].w_int;

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      إذا (thr->flags[4] == BIT4_1)
	    أرجع سچ;

      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*> (cp->net->fil);
      أكد(sig);

      إذا (!resize_rval_vec(val, off, sig->قدر_سائيز()))
	    أرجع سچ;

      إذا (thr->ecount == 0) {
	    وقت_جدول_سونپيو_ويڪٽر(ptr, off, sig->قدر_سائيز(), val, 0);
      } وإلا {
	    وقت_جدول_evctl(ptr, val, off, sig->قدر_سائيز(), thr->event, thr->ecount);
      }

      أرجع سچ;
}

/*
 * %assign/vec4/d <var-label> <delay>
 */
منطقي جو_سونپيو_VEC4_D(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      بنانشان del_اشاريو = cp->ٻٽو_انگ_idx[0];
      vvp_time64_t del = thr->لفظ[del_اشاريو].w_int;

      vvp_vector4_t value = thr->ڪڍو_vec4();

      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*> (cp->net->fil);
      أكد(sig);

      وقت_جدول_سونپيو_ويڪٽر(ptr, 0, sig->قدر_سائيز(), value, del);

      أرجع سچ;
}

/*
 * %assign/vec4/e <var-label>
 */
منطقي جو_سونپيو_VEC4_E(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_vector4_t value = thr->ڪڍو_vec4();

      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*> (cp->net->fil);
      أكد(sig);

      إذا (thr->ecount == 0) {
	    وقت_جدول_سونپيو_ويڪٽر(ptr, 0, sig->قدر_سائيز(), value, 0);
      } وإلا {
	    وقت_جدول_evctl(ptr, value, 0, sig->قدر_سائيز(), thr->event, thr->ecount);
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
منطقي جو_سونپيو_WR(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان delay = cp->ٻٽو_انگ_idx[0];
      مزدوج value = thr->ڪڍو_real();
      s_vpi_وقت del;

      del.type = vpi_Sim_وقت;
      vpip_وقت_منزل_timestruct(&del, delay);

      __vpiHandle*tmp = cp->handle;

      t_vpi_قدر val;
      val.format = vpiRealVal;
      val.value.real = value;
      vpi_put_قدر(tmp, &val, &del, vpiTransportDelay);

      أرجع سچ;
}

منطقي جو_سونپيو_WRD(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_time64_t delay = thr->لفظ[cp->ٻٽو_انگ_idx[0]].w_uint;
      مزدوج value = thr->ڪڍو_real();
      s_vpi_وقت del;

      del.type = vpi_Sim_وقت;
      vpip_وقت_منزل_timestruct(&del, delay);

      __vpiHandle*tmp = cp->handle;

      t_vpi_قدر val;
      val.format = vpiRealVal;
      val.value.real = value;
      vpi_put_قدر(tmp, &val, &del, vpiTransportDelay);

      أرجع سچ;
}

منطقي جو_سونپيو_WRE(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أكد(thr->event != 0);
      مزدوج value = thr->ڪڍو_real();
      __vpiHandle*tmp = cp->handle;

	// If the count is zero then just put the value.
      إذا (thr->ecount == 0) {
	    t_vpi_قدر val;

	    val.format = vpiRealVal;
	    val.value.real = value;
	    vpi_put_قدر(tmp, &val, 0, vpiNoDelay);
      } وإلا {
	    وقت_جدول_evctl(tmp, value, thr->event, thr->ecount);
      }

      thr->event = 0;
      thr->ecount = 0;

      أرجع سچ;
}

منطقي جو_BLEND(vthread_t thr, vvp_ڪوڊ_t)
{
      vvp_vector4_t vala = thr->ڪڍو_vec4();
      vvp_vector4_t valb = thr->ڪڍو_vec4();
      أكد(vala.size() == valb.size());

      لكل (بنانشان idx = 0 ; idx < vala.size() ; idx += 1) {
	    إذا (vala.value(idx) == valb.value(idx))
		  استمر;

	    vala.سيٽ_ٻٽو_انگ(idx, BIT4_X);
      }

      thr->ڌڪو_vec4(vala);
      أرجع سچ;
}

منطقي جو_BLEND_STR(vthread_t thr, vvp_ڪوڊ_t)
{
      string f = thr->ڪڍو_str();
      string t = thr->ڪڍو_str();
      thr->ڌڪو_str((t == f) ? t : "");
      أرجع سچ;
}

منطقي جو_BLEND_WR(vthread_t thr, vvp_ڪوڊ_t)
{
      مزدوج f = thr->ڪڍو_real();
      مزدوج t = thr->ڪڍو_real();
      thr->ڌڪو_real((t == f) ? t : 0.0);
      أرجع سچ;
}

منطقي جو_BREAKPOINT(vthread_t, vvp_ڪوڊ_t)
{
      أرجع سچ;
}

/*
 * %callf/void <code-label>, <scope-label>
 * Combine the %fork and %join steps for invoking a function.
 */
ساكن منطقي do_callf_void(vthread_t thr, vthread_t ٻار)
{

      إذا (ٻار->والد_scope->is_automatic()) {
	      /* The context allocated for this child is the top entry
		 on the write context stack */
	    ٻار->wt_context = thr->wt_context;
	    ٻار->rd_context = thr->wt_context;
      }

        // Mark the function thread as a direct child of the current thread.
      ٻار->والد = thr;
      thr->children.insert(ٻار);
        // This should be the only child
      أكد(thr->children.size()==1);

        // Execute the function. This SHOULD run the function to completion,
        // but there are some exceptional situations where it won't.
      أكد(ٻار->والد_scope->get_type_ڪوڊ() == vpiFunction);
      ٻار->is_scheduled = 1;
      ٻار->i_am_اندريون_function = 1;
      vthread_لاڳيتو(ٻار);
      running_thread = thr;

      إذا (ٻار->i_have_ended) {
	    do_join(thr, ٻار);
	    أرجع سچ;
      } وإلا {
	    thr->i_am_joining = 1;
	    أرجع خطأ;
      }
}

منطقي جو_CALLF_OBJ(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vthread_t ٻار = vthread_new(cp->cptr2, cp->scope);
      أرجع do_callf_void(thr, ٻار);

      // XXXX NOT IMPLEMENTED
}

منطقي جو_CALLF_REAL(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vthread_t ٻار = vthread_new(cp->cptr2, cp->scope);

	// This is the return value. Push a place-holder value. The function
	// will replace this with the actual value using a %ret/real instruction.
      thr->ڌڪو_real(0.0);
      ٻار->دليلون_real.push_back(0);

      أرجع do_callf_void(thr, ٻار);
}

منطقي جو_CALLF_STR(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vthread_t ٻار = vthread_new(cp->cptr2, cp->scope);

      thr->ڌڪو_str("");
      ٻار->دليلون_str.push_back(0);

      أرجع do_callf_void(thr, ٻار);
}

منطقي جو_CALLF_VEC4(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vthread_t ٻار = vthread_new(cp->cptr2, cp->scope);

      vpiScopeFunction*scope_func = dynamic_cast<vpiScopeFunction*>(cp->scope);
      أكد(scope_func);

	// This is the return value. Push a place-holder value. The function
	// will replace this with the actual value using a %ret/real instruction.
      thr->ڌڪو_vec4(vvp_vector4_t(scope_func->get_func_ويڪر(), scope_func->get_func_init_val()));
      ٻار->دليلون_vec4.push_back(0);

      أرجع do_callf_void(thr, ٻار);
}

منطقي جو_CALLF_VOID(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vthread_t ٻار = vthread_new(cp->cptr2, cp->scope);
      أرجع do_callf_void(thr, ٻار);
}

/*
 * The %cassign/link instruction connects a source node to a
 * destination node. The destination node must be a signal, as it is
 * marked with the source of the cassign so that it may later be
 * unlinked without specifically knowing the source that this
 * instruction used.
 */
منطقي جو_CASSIGN_LINK(vthread_t, vvp_ڪوڊ_t cp)
{
      vvp_net_t*dst = cp->net;
      vvp_net_t*src = cp->net2;

      vvp_fun_signal_بنياد*sig
	    = dynamic_cast<vvp_fun_signal_بنياد*>(dst->fun);
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
      vvp_fun_signal_بنياد*sig
	    = dynamic_cast<vvp_fun_signal_بنياد*>(dst->fun);
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
منطقي جو_CASSIGN_VEC4(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_net_t*net = cp->net;
      vvp_vector4_t value = thr->ڪڍو_vec4();

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
منطقي جو_CASSIGN_VEC4_OFF(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_net_t*net = cp->net;
      بنانشان بنياد_idx = cp->ٻٽو_انگ_idx[0];
      ڊگهو بنياد = thr->لفظ[بنياد_idx].w_int;
      vvp_vector4_t value = thr->ڪڍو_vec4();
      بنانشان wid = value.size();

      إذا (thr->flags[4] == BIT4_1)
	    أرجع سچ;

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*> (net->fil);
      أكد(sig);

      إذا (بنياد < 0 && (wid <= (بنانشان)-بنياد))
	    أرجع سچ;

      إذا (بنياد >= (ڊگهو)sig->قدر_سائيز())
	    أرجع سچ;

      إذا (بنياد < 0) {
	    wid -= (بنانشان) -بنياد;
	    بنياد = 0;
	    value.resize(wid);
      }

      إذا (بنياد+wid > sig->قدر_سائيز()) {
	    wid = sig->قدر_سائيز() - بنياد;
	    value.resize(wid);
      }

      vvp_net_ptr_t ptr (net, 1);
      vvp_send_vec4_pv(ptr, value, بنياد, sig->قدر_سائيز(), 0);
      أرجع سچ;
}

منطقي جو_CASSIGN_WR(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_net_t*net  = cp->net;
      مزدوج value = thr->ڪڍو_real();

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
منطقي جو_CAST2(vthread_t thr, vvp_ڪوڊ_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      بنانشان wid = val.size();

      لكل (بنانشان idx = 0 ; idx < wid ; idx += 1) {
	    بدل (val.value(idx)) {
		حالة BIT4_0:
		حالة BIT4_1:
		  اكسر;
		افتراضي:
		  val.سيٽ_ٻٽو_انگ(idx, BIT4_0);
		  اكسر;
	    }
      }

      أرجع سچ;
}

منطقي do_cast_vec_dar(vthread_t thr, vvp_ڪوڊ_t cp, منطقي as_vec4)
{
      بنانشان wid = cp->عدد;

      vvp_object_t obj;
      thr->ڪڍو_object(obj);

      vvp_darray*darray = obj.peek<vvp_darray>();
      أكد(darray);

      vvp_vector4_t vec = darray->get_bitstream(as_vec4);
      إذا (vec.size() != wid) {
	    cerr << thr->get_fileline()
	         << "VVP error: size mismatch when casting dynamic array to vector." << سٽ_پڄاڻي;
            thr->ڌڪو_vec4(vvp_vector4_t(wid));
            وقت_جدول_stop(0);
            أرجع خطأ;
      }
      thr->ڌڪو_vec4(vec);
      أرجع سچ;
}

/*
 * %cast/vec2/dar <wid>
 */
منطقي جو_CAST_VEC2_DAR(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع do_cast_vec_dar(thr, cp, خطأ);
}

/*
 * %cast/vec4/dar <wid>
 */
منطقي جو_CAST_VEC4_DAR(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع do_cast_vec_dar(thr, cp, سچ);
}

/*
 * %cast/vec4/str <wid>
 */
منطقي جو_CAST_VEC4_STR(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان wid = cp->عدد;
      string str = thr->ڪڍو_str();

      vvp_vector4_t vec(wid, BIT4_0);

      إذا (wid != 8*str.length()) {
	    cerr << thr->get_fileline()
	         << "VVP error: size mismatch when casting string to vector." << سٽ_پڄاڻي;
            thr->ڌڪو_vec4(vec);
            وقت_جدول_stop(0);
            أرجع خطأ;
      }

      بنانشان sdx = 0;
      بنانشان vdx = wid;
      طالما (vdx > 0) {
            حرف ch = str[sdx++];
            vdx -= 8;
            لكل (بنانشان bdx = 0; bdx < 8; bdx += 1) {
                  إذا (ch & 1)
                        vec.سيٽ_ٻٽو_انگ(vdx+bdx, BIT4_1);
                  ch >>= 1;
            }
      }

      thr->ڌڪو_vec4(vec);
      أرجع سچ;
}

ساكن فراغ do_CMPE(vthread_t thr, ثابت vvp_vector4_t&lval, ثابت vvp_vector4_t&rval)
{
      أكد(rval.size() == lval.size());

      إذا (lval.has_xz() || rval.has_xz()) {

	    بنانشان wid = lval.size();
	    vvp_bit4_t eq  = BIT4_1;
	    vvp_bit4_t eeq = BIT4_1;

	    لكل (بنانشان idx = 0 ; idx < wid ; idx += 1) {
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
منطقي جو_CMPE(vthread_t thr, vvp_ڪوڊ_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ثابت vvp_vector4_t&rval = thr->peek_vec4(0);
      ثابت vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPE(thr, lval, rval);

      thr->ڪڍو_vec4(2);
      أرجع سچ;
}

منطقي جو_CMPNE(vthread_t thr, vvp_ڪوڊ_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ثابت vvp_vector4_t&rval = thr->peek_vec4(0);
      ثابت vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];
      thr->flags[6] =  ~thr->flags[6];

      thr->ڪڍو_vec4(2);
      أرجع سچ;
}

/*
 * %cmpi/e <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
منطقي جو_CMPIE(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان wid = cp->عدد;

      ثابت vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_فوري_rval (cp, rval);

      do_CMPE(thr, lval, rval);

      thr->ڪڍو_vec4(1);
      أرجع سچ;
}

منطقي جو_CMPINE(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان wid = cp->عدد;

      ثابت vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_فوري_rval (cp, rval);

      do_CMPE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];
      thr->flags[6] =  ~thr->flags[6];

      thr->ڪڍو_vec4(1);
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
      بنانشان wid = lval.size();

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

      لكل (بنانشان idx = 1 ;  idx < wid ;  idx += 1) {
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
منطقي جو_CMPS(vthread_t thr, vvp_ڪوڊ_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ثابت vvp_vector4_t&rval = thr->peek_vec4(0);
      ثابت vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPS(thr, lval, rval);

      thr->ڪڍو_vec4(2);
      أرجع سچ;
}

/*
 * %cmpi/s <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
منطقي جو_CMPIS(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان wid = cp->عدد;

      ثابت vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_فوري_rval (cp, rval);

      do_CMPS(thr, lval, rval);

      thr->ڪڍو_vec4(1);
      أرجع سچ;
}

منطقي جو_CMPSTR(vthread_t thr, vvp_ڪوڊ_t)
{
      string re = thr->ڪڍو_str();
      string le = thr->ڪڍو_str();

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

ساكن فراغ جو_CMPU_the_hard_way(vthread_t thr, بنانشان wid,
				 ثابت vvp_vector4_t&lval,
				 ثابت vvp_vector4_t&rval)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_bit4_t eeq = BIT4_1;

      لكل (بنانشان idx = 0 ;  idx < wid ;  idx += 1) {
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
		 << ", rval=" << rval << سٽ_پڄاڻي;
      }
      أكد(rval.size() == lval.size());
      بنانشان wid = lval.size();

      بنانشان ڊگهو*larray = lval.subarray(0,wid);
      إذا (larray == 0) أرجع جو_CMPU_the_hard_way(thr, wid, lval, rval);

      بنانشان ڊگهو*rarray = rval.subarray(0,wid);
      إذا (rarray == 0) {
	    ميٽيو[]larray;
	    أرجع جو_CMPU_the_hard_way(thr, wid, lval, rval);
      }

      بنانشان لفظ = (wid+CPU_لفظ_بٽ-1) / CPU_لفظ_بٽ;

      لكل (بنانشان wdx = 0 ; wdx < لفظ ; wdx += 1) {
	    إذا (larray[wdx] == rarray[wdx])
		  استمر;

	    eq = BIT4_0;
	    إذا (larray[wdx] < rarray[wdx])
		  lt = BIT4_1;
	    وإلا
		  lt = BIT4_0;
      }

      ميٽيو[]larray;
      ميٽيو[]rarray;

      thr->flags[4] = eq;
      thr->flags[5] = lt;
      thr->flags[6] = eq;
}

منطقي جو_CMPU(vthread_t thr, vvp_ڪوڊ_t)
{

      ثابت vvp_vector4_t&rval = thr->peek_vec4(0);
      ثابت vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPU(thr, lval, rval);

      thr->ڪڍو_vec4(2);
      أرجع سچ;
}

/*
 * %cmpi/u <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
منطقي جو_CMPIU(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان wid = cp->عدد;

      ثابت vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_فوري_rval (cp, rval);

      do_CMPU(thr, lval, rval);

      thr->ڪڍو_vec4(1);
      أرجع سچ;
}


/*
 * %cmp/x
 */
منطقي جو_CMPX(vthread_t thr, vvp_ڪوڊ_t)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_vector4_t rval = thr->ڪڍو_vec4();
      vvp_vector4_t lval = thr->ڪڍو_vec4();

      أكد(rval.size() == lval.size());
      بنانشان wid = lval.size();

      لكل (بنانشان idx = 0 ; idx < wid ; idx += 1) {
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

	    بنانشان wid = lval.size();
	    vvp_bit4_t eq  = BIT4_1;

	    لكل (بنانشان idx = 0 ; idx < wid ; idx += 1) {
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

منطقي جو_CMPWE(vthread_t thr, vvp_ڪوڊ_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ثابت vvp_vector4_t&rval = thr->peek_vec4(0);
      ثابت vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPWE(thr, lval, rval);

      thr->ڪڍو_vec4(2);
      أرجع سچ;
}

منطقي جو_CMPWNE(vthread_t thr, vvp_ڪوڊ_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ثابت vvp_vector4_t&rval = thr->peek_vec4(0);
      ثابت vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPWE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];

      thr->ڪڍو_vec4(2);
      أرجع سچ;
}

منطقي جو_CMPWR(vthread_t thr, vvp_ڪوڊ_t)
{
      مزدوج r = thr->ڪڍو_real();
      مزدوج l = thr->ڪڍو_real();

      vvp_bit4_t eq = (l == r)? BIT4_1 : BIT4_0;
      vvp_bit4_t lt = (l <  r)? BIT4_1 : BIT4_0;

      thr->flags[4] = eq;
      thr->flags[5] = lt;

      أرجع سچ;
}

/*
 * %cmp/z
 */
منطقي جو_CMPZ(vthread_t thr, vvp_ڪوڊ_t)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_vector4_t rval = thr->ڪڍو_vec4();
      vvp_vector4_t lval = thr->ڪڍو_vec4();

      أكد(rval.size() == lval.size());
      بنانشان wid = lval.size();

      لكل (بنانشان idx = 0 ; idx < wid ; idx += 1) {
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
منطقي جو_CONCAT_STR(vthread_t thr, vvp_ڪوڊ_t)
{
      string متن = thr->ڪڍو_str();
      thr->peek_str(0).append(متن);
      أرجع سچ;
}

/*
 *  %concati/str <string>;
 */
منطقي جو_CONCATI_STR(vthread_t thr, vvp_ڪوڊ_t cp)
{
      ثابت حرف*متن = cp->متن;
      thr->peek_str(0).append(filter_string(متن));
      أرجع سچ;
}

/*
 * %concat/vec4
 */
منطقي جو_CONCAT_VEC4(vthread_t thr, vvp_ڪوڊ_t)
{
      ثابت vvp_vector4_t&lsb = thr->peek_vec4(0);
      ثابت vvp_vector4_t&msb = thr->peek_vec4(1);

	// The result is the size of the top two vectors in the stack.
      vvp_vector4_t res (msb.size() + lsb.size(), BIT4_X);

	// Build up the result.
      res.سيٽ_vec(0, lsb);
      res.سيٽ_vec(lsb.size(), msb);

	// Rearrange the stack to pop the inputs and push the
	// result. Do that by actually popping only 1 stack position
	// and replacing the new top with the new value.
      thr->ڪڍو_vec4(1);
      thr->peek_vec4() = res;

      أرجع سچ;
}

/*
 * %concati/vec4 <vala>, <valb>, <wid>
 *
 * Concat the immediate value to the LOW bits of the concatenation.
 * Get the HIGH bits from the top of the vec4 stack.
 */
منطقي جو_CONCATI_VEC4(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان wid  = cp->عدد;

      vvp_vector4_t&msb = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t lsb (wid, BIT4_0);
      get_فوري_rval (cp, lsb);

      vvp_vector4_t res (msb.size()+lsb.size(), BIT4_X);
      res.سيٽ_vec(0, lsb);
      res.سيٽ_vec(lsb.size(), msb);

      msb = res;
      أرجع سچ;
}

/*
 * %cvt/rv
 */
منطقي جو_CVT_RV(vthread_t thr, vvp_ڪوڊ_t)
{
      مزدوج val;
      vvp_vector4_t val4 = thr->ڪڍو_vec4();
      vector4_منزل_قدر(val4, val, خطأ);
      thr->ڌڪو_real(val);
      أرجع سچ;
}

/*
 * %cvt/rv/s
 */
منطقي جو_CVT_RV_S(vthread_t thr, vvp_ڪوڊ_t)
{
      مزدوج val;
      vvp_vector4_t val4 = thr->ڪڍو_vec4();
      vector4_منزل_قدر(val4, val, سچ);
      thr->ڌڪو_real(val);
      أرجع سچ;
}

/*
 * %cvt/sr <idx>
 * Pop the top value from the real stack, convert it to a 64bit signed
 * and save it to the indexed register.
 */
منطقي جو_CVT_SR(vthread_t thr, vvp_ڪوڊ_t cp)
{
      مزدوج r = thr->ڪڍو_real();
      thr->لفظ[cp->ٻٽو_انگ_idx[0]].w_int = i64round(r);

      أرجع سچ;
}

/*
 * %cvt/ur <idx>
 */
منطقي جو_CVT_UR(vthread_t thr, vvp_ڪوڊ_t cp)
{
      مزدوج r = thr->ڪڍو_real();
      thr->لفظ[cp->ٻٽو_انگ_idx[0]].w_uint = vlg_ڦيرو_منزل_u64(r);

      أرجع سچ;
}

/*
 * %cvt/vr <wid>
 */
منطقي جو_CVT_VR(vthread_t thr, vvp_ڪوڊ_t cp)
{
      مزدوج r = thr->ڪڍو_real();
      بنانشان wid = cp->عدد;

      vvp_vector4_t tmp(wid, r);
      thr->ڌڪو_vec4(tmp);
      أرجع سچ;
}

/*
 * This implements the %deassign instruction. All we do is write a
 * long(1) to port-3 of the addressed net. This turns off an active
 * continuous assign activated by %cassign/v
 */
منطقي جو_DEASSIGN(vthread_t, vvp_ڪوڊ_t cp)
{
      vvp_net_t*net = cp->net;
      بنانشان بنياد  = cp->ٻٽو_انگ_idx[0];
      بنانشان width = cp->ٻٽو_انگ_idx[1];

      vvp_signal_قدر*fil = dynamic_cast<vvp_signal_قدر*> (net->fil);
      أكد(fil);
      vvp_fun_signal_vec*sig = dynamic_cast<vvp_fun_signal_vec*>(net->fun);
      أكد(sig);

      إذا (بنياد >= fil->قدر_سائيز()) أرجع سچ;
      إذا (بنياد+width > fil->قدر_سائيز()) width = fil->قدر_سائيز() - بنياد;

      منطقي ڀريل_sig = بنياد == 0 && width == fil->قدر_سائيز();

	// This is the net that is forcing me...
      إذا (vvp_net_t*src = sig->cassign_link) {
	    إذا (! ڀريل_sig) {
		  fprintf(stderr, "Sorry: when a signal is assigning a "
		          "register, I cannot deassign part of it.\n");
		  ٻاهر_نڪرو(1);
	    }
	      // And this is the pointer to be removed.
	    vvp_net_ptr_t dst_ptr (net, 1);
	    src->unlink(dst_ptr);
	    sig->cassign_link = 0;
      }

	/* Do we release all or part of the net? */
      إذا (ڀريل_sig) {
	    sig->deassign();
      } وإلا {
	    sig->deassign_pv(بنياد, width);
      }

      أرجع سچ;
}

منطقي جو_DEASSIGN_WR(vthread_t, vvp_ڪوڊ_t cp)
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
منطقي جو_DEBUG_THR(vthread_t thr, vvp_ڪوڊ_t cp)
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
منطقي جو_DELAY(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_time64_t هيٺيون = cp->ٻٽو_انگ_idx[0];
      vvp_time64_t hig = cp->ٻٽو_انگ_idx[1];

      vvp_time64_t delay = (hig << 32) | هيٺيون;

      إذا (delay == 0) وقت_جدول_inactive(thr);
      وإلا وقت_جدول_vthread(thr, delay);
      أرجع خطأ;
}

منطقي جو_DELAYX(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_time64_t delay;

      أكد(cp->عدد < vthread_s::لفظ_ڳڻپ);
      delay = thr->لفظ[cp->عدد].w_uint;
      إذا (delay == 0) وقت_جدول_inactive(thr);
      وإلا وقت_جدول_vthread(thr, delay);
      أرجع خطأ;
}

منطقي جو_DELETE_ELEM(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_net_t*net = cp->net;

      int64_t idx_val = thr->لفظ[3].w_int;
      إذا (thr->flags[4] == BIT4_1) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with undefined index."
	         << سٽ_پڄاڻي;
	    أرجع سچ;
      }
      إذا (idx_val < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with negative index."
	         << سٽ_پڄاڻي;
	    أرجع سچ;
      }
      size_t idx = idx_val;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      أكد(obj);

      vvp_قطار*قطار = obj->get_object().peek<vvp_قطار>();
      إذا (قطار == 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping delete(" << idx
	         << ") on empty queue." << سٽ_پڄاڻي;
      } وإلا {
	    size_t size = قطار->get_سائيز();
	    إذا (idx >= size) {
		  cerr << thr->get_fileline()
		       << "Warning: skipping out of range delete(" << idx
		       << ") on queue of size " << size << "." << سٽ_پڄاڻي;
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
منطقي جو_DELETE_OBJ(vthread_t thr, vvp_ڪوڊ_t cp)
{
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_send_object(ptr, vvp_object_t(), thr->wt_context);

      أرجع سچ;
}

/* %reverse/obj <label>
 * Reverse element order of the queue or dynamic array object at this net.
 */
منطقي جو_ابتو_OBJ(vthread_t /*thr*/, vvp_ڪوڊ_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      أكد(obj);

      vvp_object_t stor = obj->get_object();
      إذا (stor.test_nil()) أرجع سچ;

	/* Match other queue ops: peek queues before vvp_darray so concrete
	   queue types dispatch reverse_elems() correctly. */
      إذا (vvp_قطار* قطار = stor.peek<vvp_قطار>()) {
	    قطار->ابتو_elems();
	    أرجع سچ;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      أكد(darray);
      darray->ابتو_elems();

      أرجع سچ;
}

/* %reverse/prop/obj <property-index>
 * Reverse queue/darray property storage inside the class object on stack top.
 */
منطقي جو_ابتو_PROP_OBJ(vthread_t thr, vvp_ڪوڊ_t cp)
{
      size_t pid = cp->عدد;

      vvp_object_t& چوٽي = thr->peek_object();
      vvp_cobject*cobj = چوٽي.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      إذا (arr_obj.test_nil()) أرجع سچ;

      إذا (vvp_قطار* قطار = arr_obj.peek<vvp_قطار>()) {
	    قطار->ابتو_elems();
	    أرجع سچ;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      أكد(darray);
      darray->ابتو_elems();

      أرجع سچ;
}

/* %sort/obj — ascending sort */
منطقي جو_ترتيب_ڏيو_OBJ(vthread_t /*thr*/, vvp_ڪوڊ_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      أكد(obj);

      vvp_object_t stor = obj->get_object();
      إذا (stor.test_nil()) أرجع سچ;

      إذا (vvp_قطار* قطار = stor.peek<vvp_قطار>()) {
	    قطار->ترتيب_ڏيو_elems(سچ);
	    أرجع سچ;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      أكد(darray);
      darray->ترتيب_ڏيو_elems(سچ);

      أرجع سچ;
}

/* %sort/prop/obj */
منطقي جو_ترتيب_ڏيو_PROP_OBJ(vthread_t thr, vvp_ڪوڊ_t cp)
{
      size_t pid = cp->عدد;

      vvp_object_t& چوٽي = thr->peek_object();
      vvp_cobject*cobj = چوٽي.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      إذا (arr_obj.test_nil()) أرجع سچ;

      إذا (vvp_قطار* قطار = arr_obj.peek<vvp_قطار>()) {
	    قطار->ترتيب_ڏيو_elems(سچ);
	    أرجع سچ;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      أكد(darray);
      darray->ترتيب_ڏيو_elems(سچ);

      أرجع سچ;
}

/* %rsort/obj — descending sort */
منطقي جو_RSORT_OBJ(vthread_t /*thr*/, vvp_ڪوڊ_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      أكد(obj);

      vvp_object_t stor = obj->get_object();
      إذا (stor.test_nil()) أرجع سچ;

      إذا (vvp_قطار* قطار = stor.peek<vvp_قطار>()) {
	    قطار->ترتيب_ڏيو_elems(خطأ);
	    أرجع سچ;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      أكد(darray);
      darray->ترتيب_ڏيو_elems(خطأ);

      أرجع سچ;
}

/* %rsort/prop/obj */
منطقي جو_RSORT_PROP_OBJ(vthread_t thr, vvp_ڪوڊ_t cp)
{
      size_t pid = cp->عدد;

      vvp_object_t& چوٽي = thr->peek_object();
      vvp_cobject*cobj = چوٽي.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      إذا (arr_obj.test_nil()) أرجع سچ;

      إذا (vvp_قطار* قطار = arr_obj.peek<vvp_قطار>()) {
	    قطار->ترتيب_ڏيو_elems(خطأ);
	    أرجع سچ;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      أكد(darray);
      darray->ترتيب_ڏيو_elems(خطأ);

      أرجع سچ;
}

/* %shuffle/obj */
منطقي جو_SHUFFLE_OBJ(vthread_t /*thr*/, vvp_ڪوڊ_t cp)
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
منطقي جو_SHUFFLE_PROP_OBJ(vthread_t thr, vvp_ڪوڊ_t cp)
{
      size_t pid = cp->عدد;

      vvp_object_t& چوٽي = thr->peek_object();
      vvp_cobject*cobj = چوٽي.peek<vvp_cobject>();
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
منطقي جو_DELETE_پڇ(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      أكد(obj);

      vvp_قطار*قطار = obj->get_object().peek<vvp_قطار>();
      أكد(قطار);

      بنانشان idx = thr->لفظ[cp->ٻٽو_انگ_idx[0]].w_int;
      قطار->erase_پڇ(idx);

      أرجع سچ;
}

ساكن منطقي do_disable(vthread_t thr, vthread_t ميلاپ)
{
      منطقي flag = خطأ;

	/* Pull the target thread out of its scope if needed. */
      thr->والد_scope->ڌاڳا.erase(thr);

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
	    إذا (do_disable(tmp, ميلاپ))
		  flag = سچ;

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
		  وقت_جدول_vthread(والد, 0, سچ);

	    do_join(والد, thr);

      } وإلا إذا (والد) {
	      /* If the parent is yet to %join me, let its %join
		 do the reaping. */
	      //assert(tmp->is_scheduled == 0);

      } وإلا {
	      /* No parent at all. Goodbye. */
	    vthread_reap(thr);
      }

      أرجع flag || (thr == ميلاپ);
}

/*
 * Implement the %disable instruction by scanning the target scope for
 * all the target threads. Kill the target threads and wake up a
 * parent that is attempting a %join.
 */
منطقي جو_DISABLE(vthread_t thr, vvp_ڪوڊ_t cp)
{
      __vpiScope*scope = static_cast<__vpiScope*>(cp->handle);

      منطقي disabled_myself_flag = خطأ;

      طالما (! scope->ڌاڳا.empty()) {
	    سيٽ<vthread_t>::iterator cur = scope->ڌاڳا.begin();

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

منطقي جو_DISABLE_FLOW(vthread_t thr, vvp_ڪوڊ_t cp)
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
منطقي جو_DISABLE_FORK(vthread_t thr, vvp_ڪوڊ_t)
{
	/* If a %disable/fork is being executed then the parent thread
	 * cannot be waiting in a join. */
      أكد(! thr->i_am_joining);

	/* There should be no active children to disable. */
      أكد(thr->children.empty());

	/* Disable any detached children. */
      طالما (! thr->detached_children.empty()) {
	    vthread_t ٻار = *(thr->detached_children.begin());
	    أكد(ٻار);
	    أكد(ٻار->والد == thr);
	      /* Disabling the children can never match the parent thread. */
	    منطقي res = do_disable(ٻار, thr);
	    أكد(! res);
	    vthread_reap(ٻار);
      }

      أرجع سچ;
}

/*
 * This function divides a 2-word number {high, a} by a 1-word
 * number. Assume that high < b.
 */
ساكن بنانشان ڊگهو divide2words(بنانشان ڊگهو a, بنانشان ڊگهو b,
				  بنانشان ڊگهو مٿاهون)
{
      بنانشان ڊگهو نتيجو = 0;
      طالما (مٿاهون > 0) {
	    بنانشان ڊگهو tmp_نتيجو = ULONG_MAX / b;
	    بنانشان ڊگهو remain = ULONG_MAX % b;

	    remain += 1;
	    إذا (remain >= b) {
		  remain -= b;
		  tmp_نتيجو += 1;
	    }

	      // Now 0x1_0...0 = b*tmp_result + remain
	      // high*0x1_0...0 = high*(b*tmp_result + remain)
	      // high*0x1_0...0 = high*b*tmp_result + high*remain

	      // We know that high*0x1_0...0 >= high*b*tmp_result, and
	      // we know that high*0x1_0...0 > high*remain. Use
	      // high*remain as the remainder for another iteration,
	      // and add tmp_result*high into the current estimate of
	      // the result.
	    نتيجو += tmp_نتيجو * مٿاهون;

	      // The new iteration starts with high*remain + a.
	    remain = multiply_with_حاصل(مٿاهون, remain, مٿاهون);
	    a += remain;
            إذا(a < remain)
              مٿاهون += 1;

	      // Now result*b + {high,a} == the input {high,a}. It is
	      // possible that the new high >= 1. If so, it will
	      // certainly be less than high from the previous
	      // iteration. Do another iteration and it will shrink,
	      // eventually to 0.
      }

	// high is now 0, so a is the remaining remainder, so we can
	// finish off the integer divide with a simple a/b.

      أرجع نتيجو + a/b;
}

ساكن بنانشان ڊگهو* divide_بٽ(بنانشان ڊگهو*ap, ثابت بنانشان ڊگهو*bp, بنانشان wid)
{
	// Do all our work a cpu-word at a time. The "words" variable
	// is the number of words of the wid.
      بنانشان لفظ = (wid+CPU_لفظ_بٽ-1) / CPU_لفظ_بٽ;

      بنانشان btop = لفظ-1;
      طالما (btop > 0 && bp[btop] == 0)
	    btop -= 1;

	// Detect divide by 0, and exit.
      إذا (btop==0 && bp[0]==0)
	    أرجع 0;

	// The result array will eventually accumulate the result. The
	// diff array is a difference that we use in the intermediate.
      بنانشان ڊگهو*diff  = جديد بنانشان ڊگهو[لفظ];
      بنانشان ڊگهو*نتيجو= جديد بنانشان ڊگهو[لفظ];
      لكل (بنانشان idx = 0 ; idx < لفظ ; idx += 1)
	    نتيجو[idx] = 0;

      لكل (بنانشان cur = لفظ-btop ; cur > 0 ; cur -= 1) {
	    بنانشان cur_ptr = cur-1;
	    بنانشان ڊگهو cur_res;
	    إذا (ap[cur_ptr+btop] >= bp[btop]) {
		  بنانشان ڊگهو مٿاهون = 0;
		  إذا (cur_ptr+btop+1 < لفظ)
			مٿاهون = ap[cur_ptr+btop+1];
		  cur_res = divide2words(ap[cur_ptr+btop], bp[btop], مٿاهون);

	    } وإلا إذا (cur_ptr+btop+1 >= لفظ) {
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
	    multiply_array_imm(diff+cur_ptr, bp, لفظ-cur_ptr, cur_res);
	      // ap -= diff
	    بنانشان ڊگهو حاصل = 1;
	    لكل (بنانشان idx = cur_ptr ; idx < لفظ ; idx += 1)
		  ap[idx] = جوڙيو_with_حاصل(ap[idx], ~diff[idx], حاصل);

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
			لكل (بنانشان idx = cur_ptr ; idx < لفظ ; idx += 1)
			      ap[idx] = جوڙيو_with_حاصل(ap[idx], bp[idx-cur_ptr], حاصل);
		  } طالما (حاصل == 0);
	    }

	    نتيجو[cur_ptr] = cur_res;
      }

	// Now ap contains the remainder and result contains the
	// desired result. We should find that:
	//  input-a = bp * result + ap;

      ميٽيو[]diff;
      أرجع نتيجو;
}

/*
 * %div
 */
منطقي جو_DIV(vthread_t thr, vvp_ڪوڊ_t)
{
      vvp_vector4_t valb = thr->ڪڍو_vec4();
      vvp_vector4_t vala = thr->ڪڍو_vec4();

      أكد(vala.size()== valb.size());
      بنانشان wid = vala.size();

      بنانشان ڊگهو*ap = vala.subarray(0, wid);
      إذا (ap == 0) {
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->ڌڪو_vec4(tmp);
	    أرجع سچ;
      }

      بنانشان ڊگهو*bp = valb.subarray(0, wid);
      إذا (bp == 0) {
	    ميٽيو[]ap;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->ڌڪو_vec4(tmp);
	    أرجع سچ;
      }

	// If the value fits in a single CPU word, then do it the easy way.
      إذا (wid <= CPU_لفظ_بٽ) {
	    إذا (bp[0] == 0) {
		  vvp_vector4_t tmp(wid, BIT4_X);
		  thr->ڌڪو_vec4(tmp);
	    } وإلا {
		  ap[0] /= bp[0];
		  vala.setarray(0, wid, ap);
		  thr->ڌڪو_vec4(vala);
	    }
	    ميٽيو[]ap;
	    ميٽيو[]bp;
	    أرجع سچ;
      }

      بنانشان ڊگهو*نتيجو = divide_بٽ(ap, bp, wid);
      إذا (نتيجو == 0) {
	    ميٽيو[]ap;
	    ميٽيو[]bp;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->ڌڪو_vec4(tmp);
	    أرجع سچ;
      }

	// Now ap contains the remainder and result contains the
	// desired result. We should find that:
	//  input-a = bp * result + ap;

      vala.setarray(0, wid, نتيجو);
      thr->ڌڪو_vec4(vala);
      ميٽيو[]ap;
      ميٽيو[]bp;
      ميٽيو[]نتيجو;

      أرجع سچ;
}


ساكن فراغ negate_لفظ(بنانشان ڊگهو*val, بنانشان لفظ)
{
      بنانشان ڊگهو حاصل = 1;
      لكل (بنانشان idx = 0 ; idx < لفظ ; idx += 1)
	    val[idx] = جوڙيو_with_حاصل(0, ~val[idx], حاصل);
}

/*
 * %div/s
 */
منطقي جو_DIV_S(vthread_t thr, vvp_ڪوڊ_t)
{
      vvp_vector4_t valb = thr->ڪڍو_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      أكد(vala.size()== valb.size());
      بنانشان wid = vala.size();
      بنانشان لفظ = (wid + CPU_لفظ_بٽ - 1) / CPU_لفظ_بٽ;

	// Get the values, left in right, in binary form. If there is
	// a problem with either (caused by an X or Z bit) then we
	// know right away that the entire result is X.
      بنانشان ڊگهو*ap = vala.subarray(0, wid);
      إذا (ap == 0) {
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    أرجع سچ;
      }

      بنانشان ڊگهو*bp = valb.subarray(0, wid);
      إذا (bp == 0) {
	    ميٽيو[]ap;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    أرجع سچ;
      }

	// Sign extend the bits in the array to fill out the array.
      بنانشان ڊگهو sign_نقاب = 0;
      إذا (بنانشان ڊگهو sign_بٽ = (لفظ*CPU_لفظ_بٽ) - wid) {
	    sign_نقاب = -1UL << (CPU_لفظ_بٽ-sign_بٽ);
	    إذا (ap[لفظ-1] & (sign_نقاب>>1))
		  ap[لفظ-1] |= sign_نقاب;
	    إذا (bp[لفظ-1] & (sign_نقاب>>1))
		  bp[لفظ-1] |= sign_نقاب;
      }

	// If the value fits in a single word, then use the native divide.
      إذا (wid <= CPU_لفظ_بٽ) {
	    إذا (bp[0] == 0) {
		  vvp_vector4_t tmp(wid, BIT4_X);
		  vala = tmp;
	    } وإلا إذا (((ڊگهو)ap[0] == LONG_MIN) && ((ڊگهو)bp[0] == -1)) {
		  vvp_vector4_t tmp(wid, BIT4_0);
		  tmp.سيٽ_ٻٽو_انگ(wid-1, BIT4_1);
		  vala = tmp;
	    } وإلا {
		  ڊگهو tmpa = (ڊگهو) ap[0];
		  ڊگهو tmpb = (ڊگهو) bp[0];
		  ڊگهو res = tmpa / tmpb;
		  ap[0] = ((بنانشان ڊگهو)res) & ~sign_نقاب;
		  vala.setarray(0, wid, ap);
	    }
	    ميٽيو[]ap;
	    ميٽيو[]bp;
	    أرجع سچ;
      }

	// We need to the actual division to positive integers. Make
	// them positive here, and remember the negations.
      منطقي negate_flag = خطأ;
      إذا ( ((ڊگهو) ap[لفظ-1]) < 0 ) {
	    negate_flag = سچ;
	    negate_لفظ(ap, لفظ);
      }
      إذا ( ((ڊگهو) bp[لفظ-1]) < 0 ) {
	    negate_flag ^= سچ;
	    negate_لفظ(bp, لفظ);
      }

      بنانشان ڊگهو*نتيجو = divide_بٽ(ap, bp, wid);
      إذا (نتيجو == 0) {
	    ميٽيو[]ap;
	    ميٽيو[]bp;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    أرجع سچ;
      }

      إذا (negate_flag) {
	    negate_لفظ(نتيجو, لفظ);
      }

      نتيجو[لفظ-1] &= ~sign_نقاب;

      vala.setarray(0, wid, نتيجو);
      ميٽيو[]ap;
      ميٽيو[]bp;
      ميٽيو[]نتيجو;
      أرجع سچ;
}

منطقي جو_DIV_WR(vthread_t thr, vvp_ڪوڊ_t)
{
      مزدوج r = thr->ڪڍو_real();
      مزدوج l = thr->ڪڍو_real();
      thr->ڌڪو_real(l / r);

      أرجع سچ;
}

/*
 * %dup/obj
 * %dup/real
 * %dup/vec4
 *
 * Push a duplicate of the object on the appropriate stack.
 */
منطقي جو_DUP_OBJ(vthread_t thr, vvp_ڪوڊ_t)
{
      vvp_object_t src = thr->peek_object();

        // If it is null push a new null object
      إذا (src.test_nil())
	    thr->ڌڪو_object(vvp_object_t());
      وإلا
	    thr->ڌڪو_object(src.duplicate());

      أرجع سچ;
}

منطقي جو_DUP_REAL(vthread_t thr, vvp_ڪوڊ_t)
{
      thr->ڌڪو_real(thr->peek_real(0));
      أرجع سچ;
}

منطقي جو_DUP_VEC4(vthread_t thr, vvp_ڪوڊ_t)
{
      thr->ڌڪو_vec4(thr->peek_vec4(0));
      أرجع سچ;
}

/*
 * This terminates the current thread. If there is a parent who is
 * waiting for me to die, then I schedule it. At any rate, I mark
 * myself as a zombie by setting my pc to 0.
 */
منطقي جو_پڇاڙي(vthread_t thr, vvp_ڪوڊ_t)
{
      أكد(! thr->انتظار_for_event);
      thr->i_have_ended = 1;
      thr->pc = codespace_null();

	/* Fully detach any detached children. */
      طالما (! thr->detached_children.empty()) {
	    vthread_t ٻار = *(thr->detached_children.begin());
	    أكد(ٻار);
	    أكد(ٻار->والد == thr);
	    أكد(ٻار->i_am_detached);
	    ٻار->والد = 0;
	    ٻار->i_am_detached = 0;
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
	    وقت_جدول_vthread(tmp, 0, سچ);
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
		  وقت_جدول_vthread(tmp, 0, سچ);
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
منطقي جو_EVENT(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_vector4_t tmp (1, BIT4_X);
      vvp_send_vec4(ptr, tmp, thr->wt_context);
      أرجع سچ;
}

/*
 * %event/nb <var-label>, <delay>
 */
منطقي جو_EVENT_NB(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_time64_t delay;

      delay = thr->لفظ[cp->ٻٽو_انگ_idx[0]].w_uint;
      وقت_جدول_propagate_event(cp->net, delay);
      أرجع سچ;
}

منطقي جو_EVCTL(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أكد(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      thr->ecount = thr->لفظ[cp->ٻٽو_انگ_idx[0]].w_uint;
      أرجع سچ;
}
منطقي جو_EVCTLC(vthread_t thr, vvp_ڪوڊ_t)
{
      thr->event = 0;
      thr->ecount = 0;
      أرجع سچ;
}

منطقي جو_EVCTLI(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أكد(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      thr->ecount = cp->ٻٽو_انگ_idx[0];
      أرجع سچ;
}

منطقي جو_EVCTLS(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أكد(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      int64_t val = thr->لفظ[cp->ٻٽو_انگ_idx[0]].w_int;
      إذا (val < 0) val = 0;
      thr->ecount = val;
      أرجع سچ;
}

منطقي جو_FLAG_GET_VEC4(vthread_t thr, vvp_ڪوڊ_t cp)
{
      صحيح flag = cp->عدد;
      أكد(flag < vthread_s::FLAGS_ڳڻپ);

      vvp_vector4_t val (1, thr->flags[flag]);
      thr->ڌڪو_vec4(val);

      أرجع سچ;
}

/*
 * %flag_inv <flag1>
 */
منطقي جو_FLAG_INV(vthread_t thr, vvp_ڪوڊ_t cp)
{
      صحيح flag1 = cp->ٻٽو_انگ_idx[0];

      thr->flags[flag1] = ~ thr->flags[flag1];
      أرجع سچ;
}

/*
 * %flag_mov <flag1>, <flag2>
 */
منطقي جو_FLAG_MOV(vthread_t thr, vvp_ڪوڊ_t cp)
{
      صحيح flag1 = cp->ٻٽو_انگ_idx[0];
      صحيح flag2 = cp->ٻٽو_انگ_idx[1];

      thr->flags[flag1] = thr->flags[flag2];
      أرجع سچ;
}

/*
 * %flag_or <flag1>, <flag2>
 */
منطقي جو_FLAG_OR(vthread_t thr, vvp_ڪوڊ_t cp)
{
      صحيح flag1 = cp->ٻٽو_انگ_idx[0];
      صحيح flag2 = cp->ٻٽو_انگ_idx[1];

      thr->flags[flag1] = thr->flags[flag1] | thr->flags[flag2];
      أرجع سچ;
}

منطقي جو_FLAG_سيٽ_IMM(vthread_t thr, vvp_ڪوڊ_t cp)
{
      صحيح flag = cp->عدد;
      صحيح vali = cp->ٻٽو_انگ_idx[0];

      أكد(flag < vthread_s::FLAGS_ڳڻپ);
      أكد(vali >= 0 && vali < 4);

      ساكن ثابت vvp_bit4_t map_ٻٽو_انگ[4] = {BIT4_0, BIT4_1, BIT4_Z, BIT4_X};
      thr->flags[flag] = map_ٻٽو_انگ[vali];
      أرجع سچ;
}

منطقي جو_FLAG_سيٽ_VEC4(vthread_t thr, vvp_ڪوڊ_t cp)
{
      صحيح flag = cp->عدد;
      أكد(flag < vthread_s::FLAGS_ڳڻپ);

      ثابت vvp_vector4_t&val = thr->peek_vec4();
      thr->flags[flag] = val.value(0);
      thr->ڪڍو_vec4(1);

      أرجع سچ;
}

/*
 * the %force/link instruction connects a source node to a
 * destination node. The destination node must be a signal, as it is
 * marked with the source of the force so that it may later be
 * unlinked without specifically knowing the source that this
 * instruction used.
 */
منطقي جو_FORCE_LINK(vthread_t, vvp_ڪوڊ_t cp)
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
منطقي جو_FORCE_VEC4(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_vector4_t value = thr->ڪڍو_vec4();

	/* Send the force value to the filter on the node. */

      أكد(net->fil);
      إذا (value.size() != net->fil->filter_سائيز())
	    value = coerce_منزل_ويڪر(value, net->fil->filter_سائيز());

      net->force_vec4(value, vvp_vector2_t(vvp_vector2_t::FILL1, net->fil->filter_سائيز()));

      أرجع سچ;
}

/*
 * %force/vec4/off <net>, <off>
 */
منطقي جو_FORCE_VEC4_OFF(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_net_t*net = cp->net;
      بنانشان بنياد_idx = cp->ٻٽو_انگ_idx[0];
      ڊگهو بنياد = thr->لفظ[بنياد_idx].w_int;
      vvp_vector4_t value = thr->ڪڍو_vec4();
      بنانشان wid = value.size();

      أكد(net->fil);

      إذا (thr->flags[4] == BIT4_1)
	    أرجع سچ;

	// This is the width of the target vector.
      بنانشان use_سائيز = net->fil->filter_سائيز();

      إذا (بنياد >= (ڊگهو)use_سائيز)
	    أرجع سچ;
      إذا (بنياد < -(ڊگهو)use_سائيز)
	    أرجع سچ;

      إذا ((بنياد + wid) > use_سائيز)
	    wid = use_سائيز - بنياد;

	// Make a mask of which bits are to be forced, 0 for unforced
	// bits and 1 for forced bits.
      vvp_vector2_t نقاب (vvp_vector2_t::FILL0, use_سائيز);
      لكل (بنانشان idx = 0 ; idx < wid ; idx += 1)
	    نقاب.سيٽ_ٻٽو_انگ(بنياد+idx, 1);

      vvp_vector4_t tmp (use_سائيز, BIT4_Z);

	// vvp_net_t::force_vec4 propagates all the bits of the
	// forced vector value, regardless of the mask. This
	// ensures the unforced bits retain their current value.
      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*>(net->fil);
      أكد(sig);
      sig->vec4_قدر(tmp);

      tmp.سيٽ_vec(بنياد, value);

      net->force_vec4(tmp, نقاب);
      أرجع سچ;
}

/*
 * %force/vec4/off/d <net>, <off>, <del>
 */
منطقي جو_FORCE_VEC4_OFF_D(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_net_t*net = cp->net;

      بنانشان بنياد_idx = cp->ٻٽو_انگ_idx[0];
      ڊگهو بنياد = thr->لفظ[بنياد_idx].w_int;

      بنانشان delay_idx = cp->ٻٽو_انگ_idx[1];
      vvp_time64_t delay = thr->لفظ[delay_idx].w_uint;

      vvp_vector4_t value = thr->ڪڍو_vec4();

      أكد(net->fil);

      إذا (thr->flags[4] == BIT4_1)
	    أرجع سچ;

	// This is the width of the target vector.
      بنانشان use_سائيز = net->fil->filter_سائيز();

      إذا (بنياد >= (ڊگهو)use_سائيز)
	    أرجع سچ;
      إذا (بنياد < -(ڊگهو)use_سائيز)
	    أرجع سچ;

      وقت_جدول_force_ويڪٽر(net, بنياد, use_سائيز, value, delay);
      أرجع سچ;
}

منطقي جو_FORCE_WR(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_net_t*net  = cp->net;
      مزدوج value = thr->ڪڍو_real();

      net->force_real(value, vvp_vector2_t(vvp_vector2_t::FILL1, 1));

      أرجع سچ;
}

/*
 * The %fork instruction causes a new child to be created and pushed
 * in front of any existing child. This causes the new child to be
 * added to the list of children, and for me to be the parent of the
 * new child.
 */
منطقي جو_FORK(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vthread_t ٻار = vthread_new(cp->cptr2, cp->scope);

      إذا (cp->scope->is_automatic()) {
              /* The context allocated for this child is the top entry
                 on the write context stack. */
            ٻار->wt_context = thr->wt_context;
            ٻار->rd_context = thr->wt_context;
      }

      ٻار->والد = thr;
      thr->children.insert(ٻار);

      إذا (thr->i_am_اندريون_function) {
	    ٻار->is_scheduled = 1;
	    ٻار->i_am_اندريون_function = 1;
	    vthread_لاڳيتو(ٻار);
	    running_thread = thr;
      } وإلا {
	    وقت_جدول_vthread(ٻار, 0, سچ);
      }
      أرجع سچ;
}

منطقي جو_FREE(vthread_t thr, vvp_ڪوڊ_t cp)
{
        /* Pop the child context from the read context stack. */
      vvp_context_t ٻار_context = thr->rd_context;
      thr->rd_context = vvp_get_stacked_context(ٻار_context);

        /* Free the context. */
      vthread_free_context(ٻار_context, cp->scope);

      أرجع سچ;
}

/*
 * %inv
 *
 * Logically, this pops a value, inverts is (Verilog style, with Z and
 * X converted to X) and pushes the result. We can more efficiently
 * just to the invert in place.
 */
منطقي جو_INV(vthread_t thr, vvp_ڪوڊ_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val.invert();
      أرجع سچ;
}


/*
 * Index registers, arithmetic.
 */

ساكن مضمن int64_t get_as_64_ٻٽو_انگ(uint32_t هيٺيون_32, uint32_t مٿاهون_32)
{
      int64_t هيٺيون = هيٺيون_32;
      int64_t res = مٿاهون_32;

      res <<= 32;
      res |= هيٺيون;
      أرجع res;
}

منطقي جو_IX_جوڙيو(vthread_t thr, vvp_ڪوڊ_t cp)
{
      thr->لفظ[cp->عدد].w_int += get_as_64_ٻٽو_انگ(cp->ٻٽو_انگ_idx[0],
                                                    cp->ٻٽو_انگ_idx[1]);
      أرجع سچ;
}

منطقي جو_IX_SUB(vthread_t thr, vvp_ڪوڊ_t cp)
{
      thr->لفظ[cp->عدد].w_int -= get_as_64_ٻٽو_انگ(cp->ٻٽو_انگ_idx[0],
                                                    cp->ٻٽو_انگ_idx[1]);
      أرجع سچ;
}

منطقي جو_IX_MUL(vthread_t thr, vvp_ڪوڊ_t cp)
{
      thr->لفظ[cp->عدد].w_int *= get_as_64_ٻٽو_انگ(cp->ٻٽو_انگ_idx[0],
                                                    cp->ٻٽو_انگ_idx[1]);
      أرجع سچ;
}

منطقي جو_IX_ڀريو(vthread_t thr, vvp_ڪوڊ_t cp)
{
      thr->لفظ[cp->عدد].w_int = get_as_64_ٻٽو_انگ(cp->ٻٽو_انگ_idx[0],
                                                   cp->ٻٽو_انگ_idx[1]);
      أرجع سچ;
}

منطقي جو_IX_MOV(vthread_t thr, vvp_ڪوڊ_t cp)
{
      thr->لفظ[cp->ٻٽو_انگ_idx[0]].w_int = thr->لفظ[cp->ٻٽو_انگ_idx[1]].w_int;
      أرجع سچ;
}

منطقي جو_IX_GETV(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان اشاريو = cp->ٻٽو_انگ_idx[0];
      vvp_net_t*net = cp->net;

      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*>(net->fil);
      إذا (sig == 0) {
	    أكد(net->fil);
	    cerr << thr->get_fileline()
	         << "%%ix/getv error: Net arg not a vector signal? "
		 << typeid(*net->fil).name() << سٽ_پڄاڻي;
      }
      أكد(sig);

      vvp_vector4_t vec;
      sig->vec4_قدر(vec);
      منطقي حد_کان_مٿي_flag;
      uint64_t val = 0;
      منطقي معلوم_flag = vector4_منزل_قدر(vec, حد_کان_مٿي_flag, val);

      إذا (معلوم_flag)
	    thr->لفظ[اشاريو].w_uint = val;
      وإلا
	    thr->لفظ[اشاريو].w_uint = 0;

	/* Set bit 4 as a flag if the input is unknown. */
      thr->flags[4] = معلوم_flag ? (حد_کان_مٿي_flag ? BIT4_X : BIT4_0) : BIT4_1;

      أرجع سچ;
}

منطقي جو_IX_GETV_S(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان اشاريو = cp->ٻٽو_انگ_idx[0];
      vvp_net_t*net = cp->net;

      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*>(net->fil);
      إذا (sig == 0) {
	    أكد(net->fil);
	    cerr << thr->get_fileline()
	         << "%%ix/getv/s error: Net arg not a vector signal? "
		 << "fun=" << typeid(*net->fil).name()
		 << ", fil=" << (net->fil? typeid(*net->fil).name() : "<>")
		 << سٽ_پڄاڻي;
      }
      أكد(sig);

      vvp_vector4_t vec;
      sig->vec4_قدر(vec);
      int64_t val;
      منطقي معلوم_flag = vector4_منزل_قدر(vec, val, سچ, سچ);

      إذا (معلوم_flag)
	    thr->لفظ[اشاريو].w_int = val;
      وإلا
	    thr->لفظ[اشاريو].w_int = 0;

	/* Set bit 4 as a flag if the input is unknown. */
      thr->flags[4] = معلوم_flag? BIT4_0 : BIT4_1;

      أرجع سچ;
}

ساكن uint64_t vec4_منزل_اشاريو(vthread_t thr, منطقي signed_flag)
{
	// Get all the information we need about the vec4 vector, then
	// pop it away. We only need the bool bits and the length.
      ثابت vvp_vector4_t&val = thr->peek_vec4();
      بنانشان val_سائيز = val.size();
      بنانشان ڊگهو*بٽ = val.subarray(0, val_سائيز, خطأ);
      thr->ڪڍو_vec4(1);

	// If there are X/Z bits, then the subarray will give us a nil
	// pointer. Set a flag to indicate the error, and give up.
      إذا (بٽ == 0) {
	    thr->flags[4] = BIT4_1;
	    أرجع 0;
      }

      uint64_t v = 0;
      thr->flags[4] = BIT4_0;

      أكد(حجم(بٽ[0]) <= حجم(v));

      v = 0;
      لكل (بنانشان idx = 0 ; idx < val_سائيز ; idx += 8*حجم(بٽ[0])) {
	    uint64_t tmp = بٽ[idx/8/حجم(بٽ[0])];
	    إذا (idx < 8*حجم(v)) {
		  v |= tmp << idx;
	    } وإلا {
		  منطقي حد_کان_مٿي = signed_flag && (v >> 63) ? ~tmp != 0 : tmp != 0;
		  إذا (حد_کان_مٿي) {
			thr->flags[4] = BIT4_X;
			اكسر;
		  }
	    }
      }

	// Set the high bits that are not necessarily filled in by the
	// subarray function.
      إذا (val_سائيز < 8*حجم(v)) {
	    إذا (signed_flag && (v & (static_cast<uint64_t>(1)<<(val_سائيز-1)))) {
		    // Propagate the sign bit...
		  v |= (~static_cast<uint64_t>(0)) << val_سائيز;

	    } وإلا {
		    // Fill with zeros.
		  v &= ~((~static_cast<uint64_t>(0)) << val_سائيز);
	    }

      }

      ميٽيو[]بٽ;
      أرجع v;
}

/*
 * %ix/vec4 <idx>
 */
منطقي جو_IX_VEC4(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان use_idx = cp->عدد;
      thr->لفظ[use_idx].w_uint = vec4_منزل_اشاريو(thr, خطأ);
      أرجع سچ;
}

/*
 * %ix/vec4/s <idx>
 */
منطقي جو_IX_VEC4_S(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان use_idx = cp->عدد;
      thr->لفظ[use_idx].w_uint = vec4_منزل_اشاريو(thr, سچ);
      أرجع سچ;
}

/*
 * The various JMP instruction work simply by pulling the new program
 * counter from the instruction and resuming. If the jump is
 * conditional, then test the bit for the expected value first.
 */
منطقي جو_JMP(vthread_t thr, vvp_ڪوڊ_t cp)
{
      thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      إذا (وقت_جدول_stopped()) {
	    وقت_جدول_vthread(thr, 0, خطأ);
	    أرجع خطأ;
      }

      أرجع سچ;
}

/*
 * %jmp/0 <pc>, <flag>
 */
منطقي جو_JMP0(vthread_t thr, vvp_ڪوڊ_t cp)
{
      إذا (thr->flags[cp->ٻٽو_انگ_idx[0]] == BIT4_0)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      إذا (وقت_جدول_stopped()) {
	    وقت_جدول_vthread(thr, 0, خطأ);
	    أرجع خطأ;
      }

      أرجع سچ;
}

/*
 * %jmp/0xz <pc>, <flag>
 */
منطقي جو_JMP0_XZ(vthread_t thr, vvp_ڪوڊ_t cp)
{
      إذا (thr->flags[cp->ٻٽو_انگ_idx[0]] != BIT4_1)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      إذا (وقت_جدول_stopped()) {
	    وقت_جدول_vthread(thr, 0, خطأ);
	    أرجع خطأ;
      }

      أرجع سچ;
}

/*
 * %jmp/1 <pc>, <flag>
 */
منطقي جو_JMP1(vthread_t thr, vvp_ڪوڊ_t cp)
{
      إذا (thr->flags[cp->ٻٽو_انگ_idx[0]] == BIT4_1)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      إذا (وقت_جدول_stopped()) {
	    وقت_جدول_vthread(thr, 0, خطأ);
	    أرجع خطأ;
      }

      أرجع سچ;
}

/*
 * %jmp/1xz <pc>, <flag>
 */
منطقي جو_JMP1_XZ(vthread_t thr, vvp_ڪوڊ_t cp)
{
      إذا (thr->flags[cp->ٻٽو_انگ_idx[0]] != BIT4_0)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      إذا (وقت_جدول_stopped()) {
	    وقت_جدول_vthread(thr, 0, خطأ);
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

ساكن فراغ do_join(vthread_t thr, vthread_t ٻار)
{
      أكد(ٻار->والد == thr);

        /* If the immediate child thread is in an automatic scope... */
      إذا (ٻار->wt_context) {
              /* and is the top level task/function thread... */
            إذا (thr->wt_context != thr->rd_context) {
                    /* Pop the child context from the write context stack. */
                  vvp_context_t ٻار_context = thr->wt_context;
                  thr->wt_context = vvp_get_stacked_context(ٻار_context);

                    /* Push the child context onto the read context stack */
                  vvp_سيٽ_stacked_context(ٻار_context, thr->rd_context);
                  thr->rd_context = ٻار_context;
            }
      }

      vthread_reap(ٻار);
}

ساكن منطقي do_join_opcode(vthread_t thr)
{
      أكد( !thr->i_am_joining );
      أكد( !thr->children.empty());

	// Are there any children that have already ended? If so, then
	// join with that one.
      لكل (سيٽ<vthread_t>::iterator cur = thr->children.begin()
		 ; cur != thr->children.پڇاڙي() ; ++cur) {
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

منطقي جو_JOIN(vthread_t thr, vvp_ڪوڊ_t)
{
      أرجع do_join_opcode(thr);
}

/*
 * This %join/detach <n> instruction causes the thread to detach
 * threads that were created by an earlier %fork.
 */
منطقي جو_JOIN_DETACH(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان ڊگهو ڳڻپ = cp->عدد;

      أكد(ڳڻپ == thr->children.size());

      طالما (! thr->children.empty()) {
	    vthread_t ٻار = *thr->children.begin();
	    أكد(ٻار->والد == thr);

	      // We cannot detach automatic tasks/functions within an
	      // automatic scope. If we try to do that, we might make
	      // a mess of the allocation of the context. Note that it
	      // is OK if the child context is distinct (See %exec_ufunc.)
	    أكد(ٻار->wt_context==0 || thr->wt_context!=ٻار->wt_context);
	    إذا (ٻار->i_have_ended) {
		    // If the child has already ended, then reap it.
		  vthread_reap(ٻار);

	    } وإلا {
		  size_t res = ٻار->والد->children.erase(ٻار);
		  أكد(res == 1);
		  ٻار->i_am_detached = 1;
		  thr->detached_children.insert(ٻار);
	    }
      }

      أرجع سچ;
}

/*
 * %load/ar <array-label>, <index>;
*/
منطقي جو_ڀريو_AR(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان idx = cp->ٻٽو_انگ_idx[0];
      مزدوج لفظ_2;

	/* The result is 0.0 if the address is undefined. */
      إذا (thr->flags[4] == BIT4_1) {
	    لفظ_2 = 0.0;
      } وإلا {
	    بنانشان adr = thr->لفظ[idx].w_int;
	    لفظ_2 = cp->array->get_لفظ_r(adr);
      }

      thr->ڌڪو_real(لفظ_2);
      أرجع سچ;
}

سانچو <قسم_نالو ELEM>
ساكن منطقي ڀريو_dar(vthread_t thr, vvp_ڪوڊ_t cp)
{
      int64_t adr = thr->لفظ[3].w_int;
      vvp_net_t*net = cp->net;
      أكد(net);

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      أكد(obj);

      vvp_darray*darray = obj->get_object().peek<vvp_darray>();

      ELEM لفظ_2;
      إذا (darray &&
          (adr >= 0) && (thr->flags[4] == BIT4_0)) // A defined address >= 0
	    darray->get_لفظ(adr, لفظ_2);
      وإلا
	    dq_default(لفظ_2, obj->size());

      vthread_ڌڪو(thr, لفظ_2);
      أرجع سچ;
}

/*
 * %load/dar/r <array-label>;
 */
منطقي جو_ڀريو_DAR_R(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع ڀريو_dar<مزدوج>(thr, cp);
}

/*
 * %load/dar/str <array-label>;
 */
منطقي جو_ڀريو_DAR_STR(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع ڀريو_dar<string>(thr, cp);
}

/*
 * %load/dar/vec4 <array-label>;
 */
منطقي جو_ڀريو_DAR_VEC4(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع ڀريو_dar<vvp_vector4_t>(thr, cp);
}

/*
 * %load/prop/dar/vec4 <pid>, <wid>;
 *   Indexed read of queue/dynamic-array class property; object on stack,
 *   index in words[3] (same as %load/dar/vec4).
 */
منطقي جو_ڀريو_PROP_DAR_VEC4(vthread_t thr, vvp_ڪوڊ_t cp)
{
      size_t pid = cp->عدد;
      بنانشان wid = cp->ٻٽو_انگ_idx[0];
      int64_t adr = thr->لفظ[3].w_int;

      vvp_object_t& چوٽي = thr->peek_object();
      vvp_cobject*cobj = چوٽي.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t pobj;
      cobj->get_object(pid, pobj, 0);
      vvp_darray*darray = pobj.peek<vvp_darray>();

      vvp_vector4_t لفظ_2;
      إذا (darray && (adr >= 0) && (thr->flags[4] == BIT4_0)) {
	    darray->get_لفظ(adr, لفظ_2);
      } وإلا {
	    dq_default(لفظ_2, wid);
      }

      thr->ڌڪو_vec4(لفظ_2);
      أرجع سچ;
}

/*
 * %load/obj <var-label>
 */
منطقي جو_ڀريو_OBJ(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_net_t*net = cp->net;
      vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      أكد(fun);

      vvp_object_t val = fun->get_object();
      thr->ڌڪو_object(val);

      أرجع سچ;
}

/*
 * %load/obja <index>
 *    Loads the object from array, using index <index> as the index
 *    value. If flags[4] == 1, the calculation of <index> may have
 *    failed, so push nil.
 */
منطقي جو_ڀريو_OBJA(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان idx = cp->ٻٽو_انگ_idx[0];
      vvp_object_t لفظ_2;

	/* The result is 0.0 if the address is undefined. */
      إذا (thr->flags[4] == BIT4_1) {
	    ; // Return nil
      } وإلا {
	    بنانشان adr = thr->لفظ[idx].w_int;
	    cp->array->get_لفظ_obj(adr, لفظ_2);
      }

      thr->ڌڪو_object(لفظ_2);
      أرجع سچ;
}

/*
 * %load/real <var-label>
 */
منطقي جو_ڀريو_REAL(vthread_t thr, vvp_ڪوڊ_t cp)
{
      __vpiHandle*tmp = cp->handle;
      t_vpi_قدر val;

      val.format = vpiRealVal;
      vpi_get_قدر(tmp, &val);

      thr->ڌڪو_real(val.value.real);

      أرجع سچ;
}

/*
 * %load/str <var-label>
 */
منطقي جو_ڀريو_STR(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_net_t*net = cp->net;


      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (net->fun);
      أكد(fun);

      ثابت string&val = fun->get_string();
      thr->ڌڪو_str(val);

      أرجع سچ;
}

منطقي جو_ڀريو_STRA(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان idx = cp->ٻٽو_انگ_idx[0];
      string لفظ_2;

      إذا (thr->flags[4] == BIT4_1) {
	    لفظ_2 = "";
      } وإلا {
	    بنانشان adr = thr->لفظ[idx].w_int;
	    لفظ_2 = cp->array->get_لفظ_str(adr);
      }

      thr->ڌڪو_str(لفظ_2);
      أرجع سچ;
}


/*
 * %load/vec4 <net>
 */
منطقي جو_ڀريو_VEC4(vthread_t thr, vvp_ڪوڊ_t cp)
{
	// Push a placeholder onto the stack in order to reserve the
	// stack space. Use a reference for the stack top as a target
	// for the load.
      thr->ڌڪو_vec4(vvp_vector4_t());
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
	         << سٽ_پڄاڻي;
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
منطقي جو_ڀريو_VEC4_A(vthread_t thr, vvp_ڪوڊ_t cp)
{
      صحيح adr_اشاريو = cp->ٻٽو_انگ_idx[0];

      ڊگهو adr = thr->لفظ[adr_اشاريو].w_int;

	// If flag[3] is set, then the calculation of the address
	// failed, and this load should return X instead of the actual
	// value.
      إذا (thr->flags[4] == BIT4_1) {
	    vvp_vector4_t tmp (cp->array->get_لفظ_سائيز(), BIT4_X);
	    thr->ڌڪو_vec4(tmp);
	    أرجع سچ;
      }

      vvp_vector4_t tmp (cp->array->get_لفظ(adr));
      thr->ڌڪو_vec4(tmp);
      أرجع سچ;
}

ساكن فراغ do_verylong_mod(vvp_vector4_t&vala, ثابت vvp_vector4_t&valb,
			    منطقي کاٻو_is_neg, منطقي ساڄو_is_neg)
{
      منطقي ٻاهريون_is_neg = کاٻو_is_neg;
      ثابت صحيح len=vala.size();
      بنانشان حرف *a, *z, *t;
      a = جديد بنانشان حرف[len+1];
      z = جديد بنانشان حرف[len+1];
      t = جديد بنانشان حرف[len+1];

      بنانشان حرف حاصل;
      بنانشان حرف عارضي;

      صحيح mxa = -1, mxz = -1;
      صحيح i;
      صحيح current, copylen;

      بنانشان lb_حاصل = کاٻو_is_neg? 1 : 0;
      بنانشان rb_حاصل = ساڄو_is_neg? 1 : 0;
      لكل (صحيح idx = 0 ;  idx < len ;  idx += 1) {
	    بنانشان lb = vala.value(idx);
	    بنانشان rb = valb.value(idx);

	    إذا ((lb | rb) & 2) {
		  ميٽيو []t;
		  ميٽيو []z;
		  ميٽيو []a;
		  vvp_vector4_t tmp(len, BIT4_X);
		  vala = tmp;
		  أرجع;
	    }

	    إذا (کاٻو_is_neg) {
		  lb = (1-lb) + lb_حاصل;
		  lb_حاصل = (lb & ~1)? 1 : 0;
		  lb &= 1;
	    }
	    إذا (ساڄو_is_neg) {
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
		  ميٽيو []t;
		  ميٽيو []z;
		  ميٽيو []a;
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
		  عارضي = z[i+current] + a[i] + حاصل;
		  t[i] = (عارضي&1);
		  حاصل = (عارضي>>1);
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
      حاصل = ٻاهريون_is_neg? 1 : 0;
      لكل (صحيح idx = 0 ;  idx < len ;  idx += 1) {
	    بنانشان ob = z[idx];
	    إذا (ٻاهريون_is_neg) {
		  ob = (1-ob) + حاصل;
		  حاصل = (ob & ~1)? 1 : 0;
		  ob = ob & 1;
	    }
	    tmp.سيٽ_ٻٽو_انگ(idx, ob?BIT4_1:BIT4_0);
      }
      vala = tmp;
      ميٽيو []t;
      ميٽيو []z;
      ميٽيو []a;
}

منطقي جو_MAX_WR(vthread_t thr, vvp_ڪوڊ_t)
{
      مزدوج r = thr->ڪڍو_real();
      مزدوج l = thr->ڪڍو_real();
      إذا (r != r)
	    thr->ڌڪو_real(l);
      وإلا إذا (l != l)
	    thr->ڌڪو_real(r);
      وإلا إذا (r < l)
	    thr->ڌڪو_real(l);
      وإلا
	    thr->ڌڪو_real(r);
      أرجع سچ;
}

منطقي جو_MIN_WR(vthread_t thr, vvp_ڪوڊ_t)
{
      مزدوج r = thr->ڪڍو_real();
      مزدوج l = thr->ڪڍو_real();
      إذا (r != r)
	    thr->ڌڪو_real(l);
      وإلا إذا (l != l)
	    thr->ڌڪو_real(r);
      وإلا إذا (r < l)
	    thr->ڌڪو_real(r);
      وإلا
	    thr->ڌڪو_real(l);
      أرجع سچ;
}

منطقي جو_MOD(vthread_t thr, vvp_ڪوڊ_t)
{
      vvp_vector4_t valb = thr->ڪڍو_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      أكد(vala.size()==valb.size());
      بنانشان wid = vala.size();

      إذا(wid <= 8*حجم(بنانشان ڊگهو ڊگهو)) {
	    بنانشان ڊگهو ڊگهو lv = 0, rv = 0;

	    لكل (بنانشان idx = 0 ;  idx < wid ;  idx += 1) {
		  بنانشان ڊگهو ڊگهو lb = vala.value(idx);
		  بنانشان ڊگهو ڊگهو rb = valb.value(idx);

		  إذا ((lb | rb) & 2)
			اذهب x_ٻاهريون;

		  lv |= (بنانشان ڊگهو ڊگهو) lb << idx;
		  rv |= (بنانشان ڊگهو ڊگهو) rb << idx;
	    }

	    إذا (rv == 0)
		  اذهب x_ٻاهريون;

	    lv %= rv;

	    لكل (بنانشان idx = 0 ;  idx < wid ;  idx += 1) {
		  vala.سيٽ_ٻٽو_انگ(idx, (lv&1)?BIT4_1 : BIT4_0);
		  lv >>= 1;
	    }

	    أرجع سچ;

      } وإلا {
	    do_verylong_mod(vala, valb, خطأ, خطأ);
	    أرجع سچ;
      }

 x_ٻاهريون:
      vala = vvp_vector4_t(wid, BIT4_X);
      أرجع سچ;
}

/*
 * %mod/s
 */
منطقي جو_MOD_S(vthread_t thr, vvp_ڪوڊ_t)
{
      vvp_vector4_t valb = thr->ڪڍو_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      أكد(vala.size()==valb.size());
      بنانشان wid = vala.size();

	/* Handle the case that we can fit the bits into a long-long
	   variable. We cause use native % to do the work. */
      إذا(wid <= 8*حجم(ڊگهو ڊگهو)) {
	    ڊگهو ڊگهو lv = 0, rv = 0;

	    لكل (بنانشان idx = 0 ;  idx < wid ;  idx += 1) {
		  ڊگهو ڊگهو lb = vala.value(idx);
		  ڊگهو ڊگهو rb = valb.value(idx);

		  إذا ((lb | rb) & 2)
			اذهب x_ٻاهريون;

		  lv |= (ڊگهو ڊگهو) lb << idx;
		  rv |= (ڊگهو ڊگهو) rb << idx;
	    }

	    إذا (rv == 0)
		  اذهب x_ٻاهريون;

	    إذا ((lv == LLONG_MIN) && (rv == -1))
		  اذهب zero_ٻاهريون;

	      /* Sign extend the signed operands when needed. */
	    إذا (wid < 8*حجم(ڊگهو ڊگهو)) {
		  إذا (lv & (1LL << (wid-1)))
			lv |= -1ULL << wid;
		  إذا (rv & (1LL << (wid-1)))
			rv |= -1ULL << wid;
	    }

	    lv %= rv;

	    لكل (بنانشان idx = 0 ;  idx < wid ;  idx += 1) {
		  vala.سيٽ_ٻٽو_انگ(idx, (lv&1)? BIT4_1 : BIT4_0);
		  lv >>= 1;
	    }

	      // vala is the top of the stack, edited in place, so we
	      // do not need to push the result.

	    أرجع سچ;

      } وإلا {

	    منطقي کاٻو_is_neg  = vala.value(vala.size()-1) == BIT4_1;
	    منطقي ساڄو_is_neg = valb.value(valb.size()-1) == BIT4_1;
	    do_verylong_mod(vala, valb, کاٻو_is_neg, ساڄو_is_neg);
	    أرجع سچ;
      }

 x_ٻاهريون:
      vala = vvp_vector4_t(wid, BIT4_X);
      أرجع سچ;
 zero_ٻاهريون:
      vala = vvp_vector4_t(wid, BIT4_0);
      أرجع سچ;
}

/*
 * %mod/wr
 */
منطقي جو_MOD_WR(vthread_t thr, vvp_ڪوڊ_t)
{
      مزدوج r = thr->ڪڍو_real();
      مزدوج l = thr->ڪڍو_real();
      thr->ڌڪو_real(fmod(l,r));

      أرجع سچ;
}

/*
 * %neg/wr
 */
منطقي جو_NEG_WR(vthread_t thr, vvp_ڪوڊ_t)
{
      thr->poke_real(0, -thr->peek_real(0));
      أرجع سچ;
}

/*
 * %pad/s <wid>
 */
منطقي جو_PAD_S(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان wid = cp->عدد;

      vvp_vector4_t&val = thr->peek_vec4();
      بنانشان old_سائيز = val.size();

	// Sign-extend.
      إذا (old_سائيز < wid)
	    val.resize(wid, val.value(old_سائيز-1));
      وإلا
	    val.resize(wid);

      أرجع سچ;
}

/*
 * %pad/u <wid>
 */
منطقي جو_PAD_U(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان wid = cp->عدد;

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
ساكن منطقي جو_PART_بنياد(vthread_t thr, vvp_ڪوڊ_t cp, منطقي signed_flag)
{
      بنانشان wid = cp->عدد;

      vvp_vector4_t base4 = thr->ڪڍو_vec4();
      vvp_vector4_t&value = thr->peek_vec4();

      vvp_vector4_t res (wid, BIT4_X);

	// NOTE: This is treating the vector as signed. Is that correct?
      int32_t بنياد;
      منطقي قدر_ok = vector4_منزل_قدر(base4, بنياد, signed_flag);
      إذا (! قدر_ok) {
	    value = res;
	    أرجع سچ;
      }

      إذا (بنياد >= (int32_t)value.size()) {
	    value = res;
	    أرجع سچ;
      }

      إذا ((بنياد+(صحيح)wid) <= 0) {
	    value = res;
	    أرجع سچ;
      }

      ڊگهو vbase = 0;
      إذا (بنياد < 0) {
	    vbase = -بنياد;
	    wid -= vbase;
	    بنياد = 0;
      }

      إذا ((بنياد+wid) > value.size()) {
	    wid = value.size() - بنياد;
      }

      res .سيٽ_vec(vbase, value.subvalue(بنياد, wid));
      value = res;

      أرجع سچ;
}

منطقي جو_PART_S(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع جو_PART_بنياد(thr, cp, سچ);
}

منطقي جو_PART_U(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع جو_PART_بنياد(thr, cp, خطأ);
}

/*
 * %parti/s <wid>, <basei>, <base_wid>
 * %parti/u <wid>, <basei>, <base_wid>
 *
 * Pop the value to be selected. The result is pushed back to the stack.
 */
ساكن منطقي جو_PARTI_بنياد(vthread_t thr, vvp_ڪوڊ_t cp, منطقي signed_flag)
{
      بنانشان wid = cp->عدد;
      uint32_t بنياد = cp->ٻٽو_انگ_idx[0];
      uint32_t bwid = cp->ٻٽو_انگ_idx[1];

      vvp_vector4_t&value = thr->peek_vec4();

      vvp_vector4_t res (wid, BIT4_X);

	// NOTE: This is treating the vector as signed. Is that correct?
      int32_t use_بنياد = بنياد;
      إذا (signed_flag && bwid < 32 && (بنياد&(1<<(bwid-1)))) {
	    use_بنياد |= -1UL << bwid;
      }

      إذا (use_بنياد >= (int32_t)value.size()) {
	    value = res;
	    أرجع سچ;
      }

      إذا ((use_بنياد+(int32_t)wid) <= 0) {
	    value = res;
	    أرجع سچ;
      }

      ڊگهو vbase = 0;
      إذا (use_بنياد < 0) {
	    vbase = -use_بنياد;
	    wid -= vbase;
	    use_بنياد = 0;
      }

      إذا ((use_بنياد+wid) > value.size()) {
	    wid = value.size() - use_بنياد;
      }

      res .سيٽ_vec(vbase, value.subvalue(use_بنياد, wid));
      value = res;

      أرجع سچ;
}

منطقي جو_PARTI_S(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع جو_PARTI_بنياد(thr, cp, سچ);
}

منطقي جو_PARTI_U(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع جو_PARTI_بنياد(thr, cp, خطأ);
}

/*
 * %mul
 */
منطقي جو_MUL(vthread_t thr, vvp_ڪوڊ_t)
{
      vvp_vector4_t r = thr->ڪڍو_vec4();
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
منطقي جو_MULI(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان wid = cp->عدد;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_فوري_rval (cp, r);

      l.mul(r);
      أرجع سچ;
}

منطقي جو_MUL_WR(vthread_t thr, vvp_ڪوڊ_t)
{
      مزدوج r = thr->ڪڍو_real();
      مزدوج l = thr->ڪڍو_real();
      thr->ڌڪو_real(l * r);

      أرجع سچ;
}

منطقي جو_NAND(vthread_t thr, vvp_ڪوڊ_t)
{
      vvp_vector4_t valr = thr->ڪڍو_vec4();
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
منطقي جو_NEW_COBJ(vthread_t thr, vvp_ڪوڊ_t cp)
{
      ثابت class_type*defn = dynamic_cast<ثابت class_type*> (cp->handle);
      أكد(defn);

      vvp_object_t tmp (جديد vvp_cobject(defn));
      thr->ڌڪو_object(tmp);
      أرجع سچ;
}

منطقي جو_NEW_DARRAY(vthread_t thr, vvp_ڪوڊ_t cp)
{
      ثابت حرف*متن = cp->متن;
      size_t size = thr->لفظ[cp->ٻٽو_انگ_idx[0]].w_int;
      بنانشان لفظ_wid;
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
      } وإلا إذا ((1 == لڙي_مان_پڙهو(متن, "b%u%zn", &لفظ_wid, &n)) &&
                 (n == لڙي_ڊيگهه(متن))) {
	    obj = جديد vvp_darray_vec2(size, لفظ_wid);
      } وإلا إذا ((1 == لڙي_مان_پڙهو(متن, "sb%u%zn", &لفظ_wid, &n)) &&
                 (n == لڙي_ڊيگهه(متن))) {
	    obj = جديد vvp_darray_vec2(size, لفظ_wid);
      } وإلا إذا ((1 == لڙي_مان_پڙهو(متن, "v%u%zn", &لفظ_wid, &n)) &&
                 (n == لڙي_ڊيگهه(متن))) {
	    obj = جديد vvp_darray_vec4(size, لفظ_wid);
      } وإلا إذا ((1 == لڙي_مان_پڙهو(متن, "sv%u%zn", &لفظ_wid, &n)) &&
                 (n == لڙي_ڊيگهه(متن))) {
	    obj = جديد vvp_darray_vec4(size, لفظ_wid);
      } وإلا إذا (strcmp(متن,"r") == 0) {
	    obj = جديد vvp_darray_real(size);
      } وإلا إذا (strcmp(متن,"S") == 0) {
	    obj = جديد vvp_darray_string(size);
      } وإلا {
	    cerr << get_fileline()
	         << "Internal error: Unsupported dynamic array type: "
	         << متن << "." << سٽ_پڄاڻي;
	    أكد(0);
      }

      thr->ڌڪو_object(obj);

      أرجع سچ;
}

منطقي جو_NOOP(vthread_t, vvp_ڪوڊ_t)
{
      أرجع سچ;
}

/*
 * %nor/r
 */
منطقي جو_NORR(vthread_t thr, vvp_ڪوڊ_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_or());

      أرجع سچ;
}

/*
 * Push a null to the object stack.
 */
منطقي جو_NULL(vthread_t thr, vvp_ڪوڊ_t)
{
      vvp_object_t tmp;
      thr->ڌڪو_object(tmp);
      أرجع سچ;
}

/*
 * %and/r
 */
منطقي جو_ANDR(vthread_t thr, vvp_ڪوڊ_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_and());

      أرجع سچ;
}

/*
 * %nand/r
 */
منطقي جو_NANDR(vthread_t thr, vvp_ڪوڊ_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_and());

      أرجع سچ;
}

/*
 * %or/r
 */
منطقي جو_ORR(vthread_t thr, vvp_ڪوڊ_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_or());

      أرجع سچ;
}

/*
 * %xor/r
 */
منطقي جو_XORR(vthread_t thr, vvp_ڪوڊ_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_xor());

      أرجع سچ;
}

/*
 * %xnor/r
 */
منطقي جو_XNORR(vthread_t thr, vvp_ڪوڊ_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_xor());

      أرجع سچ;
}

/*
 * %or
 */
منطقي جو_OR(vthread_t thr, vvp_ڪوڊ_t)
{
      vvp_vector4_t valb = thr->ڪڍو_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();
      vala |= valb;
      أرجع سچ;
}

/*
 * %nor
 */
منطقي جو_NOR(vthread_t thr, vvp_ڪوڊ_t)
{
      vvp_vector4_t valr = thr->ڪڍو_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      أكد(vall.size() == valr.size());

      vall |= valr;
      vall.invert();

      أرجع سچ;
}

/*
 * %pop/obj <num>, <skip>
 */
منطقي جو_ڪڍو_OBJ(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان cnt = cp->ٻٽو_انگ_idx[0];
      بنانشان skip = cp->ٻٽو_انگ_idx[1];

      thr->ڪڍو_object(cnt, skip);
      أرجع سچ;
}

/*
 * %pop/real <number>
 */
منطقي جو_ڪڍو_REAL(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان cnt = cp->عدد;
      thr->ڪڍو_real(cnt);
      أرجع سچ;
}

/*
 *  %pop/str <number>
 */
منطقي جو_ڪڍو_STR(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان cnt = cp->عدد;
      thr->ڪڍو_str(cnt);
      أرجع سچ;
}

/*
 *  %pop/vec4 <number>
 */
منطقي جو_ڪڍو_VEC4(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان cnt = cp->عدد;
      thr->ڪڍو_vec4(cnt);
      أرجع سچ;
}

/*
 * %pow
 * %pow/s
 */
ساكن منطقي جو_POW_بنياد(vthread_t thr, منطقي signed_flag)
{
      vvp_vector4_t valb = thr->ڪڍو_vec4();
      vvp_vector4_t vala = thr->ڪڍو_vec4();

      بنانشان wid = vala.size();

      vvp_vector2_t xv2 = vvp_vector2_t(vala, سچ);
      vvp_vector2_t yv2 = vvp_vector2_t(valb, سچ);


        /* If we have an X or Z in the arguments return X. */
      إذا (xv2.is_NaN() || yv2.is_NaN()) {
	    vvp_vector4_t tmp (wid, BIT4_X);
	    thr->ڌڪو_vec4(tmp);
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
	    tmp.سيٽ_ٻٽو_انگ(0, lsb);
	    thr->ڌڪو_vec4(tmp);
	    أرجع سچ;
      }

      vvp_vector2_t نتيجو = قوت(xv2, yv2);

        /* Copy only what we need of the result. If the result is too
	   small, zero-pad it. */
      لكل (بنانشان jdx = 0;  jdx < wid;  jdx += 1) {
	    إذا (jdx >= نتيجو.size())
		  vala.سيٽ_ٻٽو_انگ(jdx, BIT4_0);
	    وإلا
		  vala.سيٽ_ٻٽو_انگ(jdx, نتيجو.value(jdx) ? BIT4_1 : BIT4_0);
      }
      thr->ڌڪو_vec4(vala);

      أرجع سچ;
}

منطقي جو_POW(vthread_t thr, vvp_ڪوڊ_t)
{
      أرجع جو_POW_بنياد(thr, خطأ);
}

منطقي جو_POW_S(vthread_t thr, vvp_ڪوڊ_t)
{
      أرجع جو_POW_بنياد(thr, سچ);
}

منطقي جو_POW_WR(vthread_t thr, vvp_ڪوڊ_t)
{
      مزدوج r = thr->ڪڍو_real();
      مزدوج l = thr->ڪڍو_real();
      thr->ڌڪو_real(قوت(l,r));

      أرجع سچ;
}

/*
 * %prop/obj <pid>, <idx>
 *
 * Load an object value from the cobject and push it onto the object stack.
 */
منطقي جو_PROP_OBJ(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان pid = cp->عدد;
      بنانشان idx = cp->ٻٽو_انگ_idx[0];

      إذا (idx != 0) {
	    أكد(idx < vthread_s::لفظ_ڳڻپ);
	    idx = thr->لفظ[idx].w_uint;
      }

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();

      vvp_object_t val;
      cobj->get_object(pid, val, idx);

      thr->ڌڪو_object(val);

      أرجع سچ;
}

ساكن فراغ get_from_obj(بنانشان pid, vvp_cobject*cobj, مزدوج&val)
{
      val = cobj->get_real(pid);
}

ساكن فراغ get_from_obj(بنانشان pid, vvp_cobject*cobj, string&val)
{
      val = cobj->get_string(pid);
}

ساكن فراغ get_from_obj(بنانشان pid, vvp_cobject*cobj, vvp_vector4_t&val)
{
      cobj->get_vec4(pid, val);
}

سانچو <قسم_نالو ELEM>
ساكن منطقي prop(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان pid = cp->عدد;

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      أكد(cobj);

      ELEM val;
      get_from_obj(pid, cobj, val);
      vthread_ڌڪو(thr, val);

      أرجع سچ;
}

/*
 * %prop/r <pid>
 *
 * Load a real value from the cobject and push it onto the real value
 * stack.
 */
منطقي جو_PROP_R(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع prop<مزدوج>(thr, cp);
}

/*
 * %prop/str <pid>
 *
 * Load a string value from the cobject and push it onto the real value
 * stack.
 */
منطقي جو_PROP_STR(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع prop<string>(thr, cp);
}

/*
 * %prop/v <pid>
 *
 * Load a property <pid> from the cobject on the top of the stack into
 * the vector space at <base>.
 */
منطقي جو_PROP_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع prop<vvp_vector4_t>(thr, cp);
}

منطقي جو_PUSHI_REAL(vthread_t thr, vvp_ڪوڊ_t cp)
{
      مزدوج mant = cp->ٻٽو_انگ_idx[0];
      uint32_t imant = cp->ٻٽو_انگ_idx[0];
      صحيح قوت_نما = cp->ٻٽو_انگ_idx[1];

	// Detect +infinity
      إذا (قوت_نما==0x3fff && imant==0) {
	    thr->ڌڪو_real(INFINITY);
	    أرجع سچ;
      }
	// Detect -infinity
      إذا (قوت_نما==0x7fff && imant==0) {
	    thr->ڌڪو_real(-INFINITY);
	    أرجع سچ;
      }
	// Detect NaN
      إذا (قوت_نما==0x3fff) {
	    thr->ڌڪو_real(nan(""));
	    أرجع سچ;
      }

      مزدوج sign = (قوت_نما & 0x4000)? -1.0 : 1.0;

      قوت_نما &= 0x1fff;

      mant = sign * ldexp(mant, قوت_نما - 0x1000);
      thr->ڌڪو_real(mant);
      أرجع سچ;
}

منطقي جو_PUSHI_STR(vthread_t thr, vvp_ڪوڊ_t cp)
{
      ثابت حرف*متن = cp->متن;
      thr->ڌڪو_str(filter_string(متن));
      أرجع سچ;
}

/*
 * %pushi/vec4 <vala>, <valb>, <wid>
 */
منطقي جو_PUSHI_VEC4(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان wid  = cp->عدد;

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t val (wid, BIT4_0);
      get_فوري_rval (cp, val);

      thr->ڌڪو_vec4(val);

      أرجع سچ;
}

/*
 * %pushv/str
 *   Pops a vec4 value, and pushes a string.
 */
منطقي جو_PUSHV_STR(vthread_t thr, vvp_ڪوڊ_t)
{
      vvp_vector4_t vec = thr->ڪڍو_vec4();

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

      thr->ڌڪو_str(val);

      أرجع سچ;
}

/*
 * %putc/str/vec4 <var>, <mux>
 */
منطقي جو_PUTC_STR_VEC4(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان muxr = cp->ٻٽو_انگ_idx[0];
      int32_t mux = muxr? thr->لفظ[muxr].w_int : 0;

      vvp_vector4_t val = thr->ڪڍو_vec4();
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

سانچو <قسم_نالو ELEM, صنف QTYPE>
ساكن منطقي qinsert(vthread_t thr, vvp_ڪوڊ_t cp, بنانشان wid=0)
{
      int64_t idx = thr->لفظ[3].w_int;
      ELEM value;
      vvp_net_t*net = cp->net;
      ڪڍو_قدر(thr, value, wid); // Pop the value to store.

      vvp_قطار*قطار = get_قطار_object<QTYPE>(thr, net);
      أكد(قطار);
      إذا (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at a negative "
	         << get_قطار_type(value)
	         << " index (" << idx << "). ";
	    print_قطار_قدر(value);
	    cerr << " was not added." << سٽ_پڄاڻي;
      } وإلا إذا (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at an undefined "
	         << get_قطار_type(value) << " index. ";
	    print_قطار_قدر(value);
	    cerr << " was not added." << سٽ_پڄاڻي;
      } وإلا {
	    بنانشان max_سائيز = thr->لفظ[cp->ٻٽو_انگ_idx[0]].w_int;
	    قطار->insert(idx, value, max_سائيز);
      }
      أرجع سچ;
}

/*
 * %qinsert/real <var-label>
 */
منطقي جو_QINSERT_REAL(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع qinsert<مزدوج, vvp_قطار_real>(thr, cp);
}

/*
 * %qinsert/str <var-label>
 */
منطقي جو_QINSERT_STR(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع qinsert<string, vvp_قطار_string>(thr, cp);
}

/*
 * %qinsert/v <var-label>
 */
منطقي جو_QINSERT_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع qinsert<vvp_vector4_t, vvp_قطار_vec4>(thr, cp, cp->ٻٽو_انگ_idx[1]);
}

/*
 * Helper functions used in the queue pop templates
 */
مضمن فراغ ڌڪو_قدر(vthread_t thr, مزدوج value, بنانشان)
{
      thr->ڌڪو_real(value);
}

مضمن فراغ ڌڪو_قدر(vthread_t thr, ثابت string&value, بنانشان)
{
      thr->ڌڪو_str(value);
}

مضمن فراغ ڌڪو_قدر(vthread_t thr, ثابت vvp_vector4_t&value, بنانشان wid)
{
      أكد(wid == value.size());
      thr->ڌڪو_vec4(value);
}

سانچو <قسم_نالو ELEM, صنف QTYPE>
ساكن منطقي q_ڪڍو(vthread_t thr, vvp_ڪوڊ_t cp,
                  فراغ (*get_val_func)(vvp_قطار*, ELEM&),
                  ثابت حرف*loc, بنانشان wid)
{
      vvp_net_t*net = cp->net;

      vvp_قطار*قطار = get_قطار_object<QTYPE>(thr, net);
      أكد(قطار);

      size_t size = قطار->get_سائيز();

      ELEM value;
      إذا (size) {
	    get_val_func(قطار, value);
      } وإلا {
	    dq_default(value, wid);
	    cerr << thr->get_fileline()
	         << "Warning: pop_" << loc << "() on empty "
	         << get_قطار_type(value) << "." << سٽ_پڄاڻي;
      }

      ڌڪو_قدر(thr, value, wid);
      أرجع سچ;
}

سانچو <قسم_نالو ELEM>
ساكن فراغ get_پٺ_قدر(vvp_قطار*قطار, ELEM&value)
{
      قطار->get_لفظ(قطار->get_سائيز()-1, value);
      قطار->pop_back();
}

سانچو <قسم_نالو ELEM, صنف QTYPE>
ساكن منطقي qpop_b(vthread_t thr, vvp_ڪوڊ_t cp, بنانشان wid=0)
{
      أرجع q_ڪڍو<ELEM, QTYPE>(thr, cp, get_پٺ_قدر<ELEM>, "back", wid);
}

/*
 * %qpop/b/real <var-label>
 */
منطقي جو_QPOP_B_REAL(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع qpop_b<مزدوج, vvp_قطار_real>(thr, cp);
}

/*
 * %qpop/b/str <var-label>
 */
منطقي جو_QPOP_B_STR(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع qpop_b<string, vvp_قطار_string>(thr, cp);
}

/*
 * %qpop/b/v <var-label>
 */
منطقي جو_QPOP_B_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع qpop_b<vvp_vector4_t, vvp_قطار_vec4>(thr, cp, cp->ٻٽو_انگ_idx[0]);
}

سانچو <قسم_نالو ELEM>
ساكن فراغ get_front_قدر(vvp_قطار*قطار, ELEM&value)
{
      قطار->get_لفظ(0, value);
      قطار->pop_front();
}

سانچو <قسم_نالو ELEM, صنف QTYPE>
ساكن منطقي qpop_f(vthread_t thr, vvp_ڪوڊ_t cp, بنانشان wid=0)
{
      أرجع q_ڪڍو<ELEM, QTYPE>(thr, cp, get_front_قدر<ELEM>, "front", wid);
}


/*
 * %qpop/f/real <var-label>
 */
منطقي جو_QPOP_F_REAL(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع qpop_f<مزدوج, vvp_قطار_real>(thr, cp);
}

/*
 * %qpop/f/str <var-label>
 */
منطقي جو_QPOP_F_STR(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع qpop_f<string, vvp_قطار_string>(thr, cp);
}

/*
 * %qpop/f/v <var-label>
 */
منطقي جو_QPOP_F_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع qpop_f<vvp_vector4_t, vvp_قطار_vec4>(thr, cp, cp->ٻٽو_انگ_idx[0]);
}

/*
 * These implement the %release/net and %release/reg instructions. The
 * %release/net instruction applies to a net kind of functor by
 * sending the release/net command to the command port. (See vvp_net.h
 * for details.) The %release/reg instruction is the same, but sends
 * the release/reg command instead. These are very similar to the
 * %deassign instruction.
 */
ساكن منطقي do_release_vec(vvp_ڪوڊ_t cp, منطقي net_flag)
{
      vvp_net_t*net = cp->net;
      بنانشان بنياد  = cp->ٻٽو_انگ_idx[0];
      بنانشان width = cp->ٻٽو_انگ_idx[1];

      أكد(net->fil);

      إذا (بنياد >= net->fil->filter_سائيز()) أرجع سچ;
      إذا (بنياد+width > net->fil->filter_سائيز())
	    width = net->fil->filter_سائيز() - بنياد;

      منطقي ڀريل_sig = بنياد == 0 && width == net->fil->filter_سائيز();

	// XXXX Can't really do this if this is a partial release?
      net->fil->force_unlink();

	/* Do we release all or part of the net? */
      vvp_net_ptr_t ptr (net, 0);
      إذا (ڀريل_sig) {
	    net->fil->release(ptr, net_flag);
      } وإلا {
	    net->fil->release_pv(ptr, بنياد, width, net_flag);
      }
      net->fun->force_flag(خطأ);

      أرجع سچ;
}

منطقي جو_RELEASE_NET(vthread_t, vvp_ڪوڊ_t cp)
{
      أرجع do_release_vec(cp, سچ);
}


منطقي جو_RELEASE_REG(vthread_t, vvp_ڪوڊ_t cp)
{
      أرجع do_release_vec(cp, خطأ);
}

/* The type is 1 for registers and 0 for everything else. */
منطقي جو_RELEASE_WR(vthread_t, vvp_ڪوڊ_t cp)
{
      vvp_net_t*net = cp->net;
      بنانشان type  = cp->ٻٽو_انگ_idx[0];

      أكد(net->fil);
      net->fil->force_unlink();

	// Send a command to this signal to unforce itself.
      vvp_net_ptr_t ptr (net, 0);
      net->fil->release(ptr, type==0);
      أرجع سچ;
}

منطقي جو_REPLICATE(vthread_t thr, vvp_ڪوڊ_t cp)
{
      صحيح rept = cp->عدد;
      vvp_vector4_t val = thr->ڪڍو_vec4();
      vvp_vector4_t res (val.size() * rept, BIT4_X);

      لكل (صحيح idx = 0 ; idx < rept ; idx += 1) {
	    res.سيٽ_vec(idx * val.size(), val);
      }

      thr->ڌڪو_vec4(res);

      أرجع سچ;
}

ساكن فراغ poke_val(vthread_t fun_thr, بنانشان اونهائي, مزدوج val)
{
      fun_thr->والد->poke_real(اونهائي, val);
}

ساكن فراغ poke_val(vthread_t fun_thr, بنانشان اونهائي, ثابت string&val)
{
      fun_thr->والد->poke_str(اونهائي, val);
}

ساكن size_t get_max(vthread_t fun_thr, مزدوج&)
{
      أرجع fun_thr->دليلون_real.size();
}

ساكن size_t get_max(vthread_t fun_thr, string&)
{
      أرجع fun_thr->دليلون_str.size();
}

ساكن size_t get_max(vthread_t fun_thr, vvp_vector4_t&)
{
      أرجع fun_thr->دليلون_vec4.size();
}

ساكن بنانشان get_اونهائي(vthread_t fun_thr, size_t اشاريو, مزدوج&)
{
      أرجع fun_thr->دليلون_real[اشاريو];
}

ساكن بنانشان get_اونهائي(vthread_t fun_thr, size_t اشاريو, string&)
{
      أرجع fun_thr->دليلون_str[اشاريو];
}

ساكن بنانشان get_اونهائي(vthread_t fun_thr, size_t اشاريو, vvp_vector4_t&)
{
      أرجع fun_thr->دليلون_vec4[اشاريو];
}

ساكن vthread_t get_func(vthread_t thr)
{
      vthread_t fun_thr = thr;

      طالما (fun_thr->والد_scope->get_type_ڪوڊ() != vpiFunction) {
	    أكد(fun_thr->والد);
	    fun_thr = fun_thr->والد;
      }

      أرجع fun_thr;
}

سانچو <قسم_نالو ELEM>
ساكن منطقي ret(vthread_t thr, vvp_ڪوڊ_t cp)
{
      size_t اشاريو = cp->عدد;
      ELEM val;
      ڪڍو_قدر(thr, val, 0);

      vthread_t fun_thr = get_func(thr);
      أكد(اشاريو < get_max(fun_thr, val));

      بنانشان اونهائي = get_اونهائي(fun_thr, اشاريو, val);
	// Use the depth to put the value into the stack of
	// the parent thread.
      poke_val(fun_thr, اونهائي, val);
      أرجع سچ;
}

/*
 * %ret/real <index>
 */
منطقي جو_RET_REAL(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع ret<مزدوج>(thr, cp);
}

/*
 * %ret/str <index>
 */
منطقي جو_RET_STR(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع ret<string>(thr, cp);
}

/*
 * %ret/vec4 <index>, <offset>, <wid>
 */
منطقي جو_RET_VEC4(vthread_t thr, vvp_ڪوڊ_t cp)
{
      size_t اشاريو = cp->عدد;
      بنانشان off_اشاريو = cp->ٻٽو_انگ_idx[0];
      بنانشان صحيح wid = cp->ٻٽو_انگ_idx[1];
      vvp_vector4_t&val = thr->peek_vec4();

      vthread_t fun_thr = get_func(thr);
      أكد(اشاريو < get_max(fun_thr, val));
      أكد(val.size() == wid);
      بنانشان اونهائي = get_اونهائي(fun_thr, اشاريو, val);

      int64_t off = off_اشاريو ? thr->لفظ[off_اشاريو].w_int : 0;
      بنانشان صحيح sig_قدر_سائيز = fun_thr->والد->peek_vec4(اونهائي).size();

      إذا (off_اشاريو!=0 && thr->flags[4] == BIT4_1) {
	    thr->ڪڍو_vec4(1);
	    أرجع سچ;
      }

      إذا (!resize_rval_vec(val, off, sig_قدر_سائيز)) {
	    thr->ڪڍو_vec4(1);
	    أرجع سچ;
      }

      إذا (off == 0 && val.size() == sig_قدر_سائيز) {
	    fun_thr->والد->poke_vec4(اونهائي, val);
      } وإلا {
	    vvp_vector4_t tmp_dst = fun_thr->والد->peek_vec4(اونهائي);
	    tmp_dst.سيٽ_vec(off, val);
	    fun_thr->والد->poke_vec4(اونهائي, tmp_dst);
      }

      thr->ڪڍو_vec4(1);
      أرجع سچ;
}

ساكن فراغ ڌڪو_from_والد(vthread_t thr, vthread_t fun_thr, بنانشان اونهائي, مزدوج&)
{
      thr->ڌڪو_real(fun_thr->والد->peek_real(اونهائي));
}

ساكن فراغ ڌڪو_from_والد(vthread_t thr, vthread_t fun_thr, بنانشان اونهائي, string&)
{
      thr->ڌڪو_str(fun_thr->والد->peek_str(اونهائي));
}

ساكن فراغ ڌڪو_from_والد(vthread_t thr, vthread_t fun_thr, بنانشان اونهائي, vvp_vector4_t&)
{
      thr->ڌڪو_vec4(fun_thr->والد->peek_vec4(اونهائي));
}

سانچو <قسم_نالو ELEM>
ساكن منطقي retload(vthread_t thr, vvp_ڪوڊ_t cp)
{
      size_t اشاريو = cp->عدد;
      ELEM type;

      vthread_t fun_thr = get_func(thr);
      أكد(اشاريو < get_max(fun_thr, type));

      بنانشان اونهائي = get_اونهائي(fun_thr, اشاريو, type);
	// Use the depth to extract the values from the stack
	// of the parent thread.
      ڌڪو_from_والد(thr, fun_thr, اونهائي, type);
      أرجع سچ;
}

/*
 * %retload/real <index>
 */
منطقي جو_RETLOAD_REAL(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع retload<مزدوج>(thr, cp);
}

/*
 * %retload/str <index>
 */
منطقي جو_RETLOAD_STR(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع retload<string>(thr, cp);
}

/*
 * %retload/vec4 <index>
 */
منطقي جو_RETLOAD_VEC4(vthread_t thr, vvp_ڪوڊ_t cp)
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
منطقي جو_SCOPY(vthread_t thr, vvp_ڪوڊ_t)
{
      vvp_object_t tmp;
      thr->ڪڍو_object(tmp);

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

سانچو <قسم_نالو ELEM>
ساكن منطقي سيٽ_dar_obj(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان adr = thr->لفظ[cp->عدد].w_int;

      ELEM value;
      thread_peek(thr, value);

      vvp_object_t&چوٽي = thr->peek_object();
      vvp_darray*darray = چوٽي.peek<vvp_darray>();
      أكد(darray);

      darray->سيٽ_لفظ(adr, value);
      أرجع سچ;
}

/*
 * %set/dar/obj/real <index>
 */
منطقي جو_سيٽ_DAR_OBJ_REAL(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع سيٽ_dar_obj<مزدوج>(thr, cp);
}

/*
 * %set/dar/obj/str <index>
 */
منطقي جو_سيٽ_DAR_OBJ_STR(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع سيٽ_dar_obj<string>(thr, cp);
}

/*
 * %set/dar/obj/vec4 <index>
 */
منطقي جو_سيٽ_DAR_OBJ_VEC4(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع سيٽ_dar_obj<vvp_vector4_t>(thr, cp);
}

/*
 * %shiftl <idx>
 *
 * Pop the operand, then push the result.
 */
منطقي جو_SHIFTL(vthread_t thr, vvp_ڪوڊ_t cp)
{
      صحيح use_اشاريو = cp->عدد;
      uint64_t سرڪايو = thr->لفظ[use_اشاريو].w_uint;

      vvp_vector4_t&val = thr->peek_vec4();
      بنانشان wid  = val.size();

      إذا (thr->flags[4] == BIT4_1) {
	      // The result is 'bx if the shift amount is undefined
	    val = vvp_vector4_t(wid, BIT4_X);

      } وإلا إذا (thr->flags[4] == BIT4_X || سرڪايو >= wid) {
	      // Shift is so big that all value is shifted out. Write
	      // a constant 0 result.
	    val = vvp_vector4_t(wid, BIT4_0);

      } وإلا إذا (سرڪايو > 0) {
	    vvp_vector4_t blk = val.subvalue(0, wid-سرڪايو);
	    vvp_vector4_t tmp (سرڪايو, BIT4_0);
	    val.سيٽ_vec(0, tmp);
	    val.سيٽ_vec(سرڪايو, blk);
      }

      أرجع سچ;
}

/*
 * %shiftr <idx>
 * This is an unsigned right shift. The <idx> is a number that selects
 * the index register with the amount of the shift. This instruction
 * checks flag bit 4, which will be true if the shift is invalid.
 */
منطقي جو_SHIFTR(vthread_t thr, vvp_ڪوڊ_t cp)
{
      صحيح use_اشاريو = cp->عدد;
      uint64_t سرڪايو = thr->لفظ[use_اشاريو].w_uint;

      vvp_vector4_t val = thr->ڪڍو_vec4();
      بنانشان wid  = val.size();

      إذا (thr->flags[4] == BIT4_1) {
	    val = vvp_vector4_t(wid, BIT4_X);

      } وإلا إذا (thr->flags[4] == BIT4_X || سرڪايو > wid) {
	    val = vvp_vector4_t(wid, BIT4_0);

      } وإلا إذا (سرڪايو > 0) {
	    vvp_vector4_t blk = val.subvalue(سرڪايو, wid-سرڪايو);
	    vvp_vector4_t tmp (سرڪايو, BIT4_0);
	    val.سيٽ_vec(0, blk);
	    val.سيٽ_vec(wid-سرڪايو, tmp);
      }

      thr->ڌڪو_vec4(val);
      أرجع سچ;
}

/*
 *  %shiftr/s <wid>
 */
منطقي جو_SHIFTR_S(vthread_t thr, vvp_ڪوڊ_t cp)
{
      صحيح use_اشاريو = cp->عدد;
      uint64_t سرڪايو = thr->لفظ[use_اشاريو].w_uint;

      vvp_vector4_t val = thr->ڪڍو_vec4();
      بنانشان wid  = val.size();

      vvp_bit4_t sign_ٻٽو_انگ = val.value(val.size()-1);

      إذا (thr->flags[4] == BIT4_1) {
	    val = vvp_vector4_t(wid, BIT4_X);

      } وإلا إذا (thr->flags[4] == BIT4_X || سرڪايو > wid) {
	    val = vvp_vector4_t(wid, sign_ٻٽو_انگ);

      } وإلا إذا (سرڪايو > 0) {
	    vvp_vector4_t blk = val.subvalue(سرڪايو, wid-سرڪايو);
	    vvp_vector4_t tmp (سرڪايو, sign_ٻٽو_انگ);
	    val.سيٽ_vec(0, blk);
	    val.سيٽ_vec(wid-سرڪايو, tmp);
      }

      thr->ڌڪو_vec4(val);
      أرجع سچ;
}

/*
 * %split/vec4 <wid>
 *   Pop 1 value,
 *   Take <wid> bits from the lsb,
 *   Push the remaining msb,
 *   Push the lsb.
 */
منطقي جو_ورهايو_VEC4(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان lsb_wid = cp->عدد;

      vvp_vector4_t&val = thr->peek_vec4();
      أكد(lsb_wid < val.size());

      vvp_vector4_t lsb = val.subvalue(0, lsb_wid);
      val = val.subvalue(lsb_wid, val.size()-lsb_wid);

      thr->ڌڪو_vec4(lsb);
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
مضمن ساكن فراغ dar_ڪڍو_قدر(vthread_t thr, مزدوج&value)
{
      value = thr->ڪڍو_real();
}

مضمن ساكن فراغ dar_ڪڍو_قدر(vthread_t thr, string&value)
{
      value = thr->ڪڍو_str();
}

مضمن ساكن فراغ dar_ڪڍو_قدر(vthread_t thr, vvp_vector4_t&value)
{
      value = thr->ڪڍو_vec4();
}

سانچو <قسم_نالو ELEM>
ساكن منطقي store_dar(vthread_t thr, vvp_ڪوڊ_t cp)
{
      int64_t adr = thr->لفظ[3].w_int;
      ELEM value;
	// FIXME: Can we get the size of the underlying array element
	//        and then use the normal pop_value?
      dar_ڪڍو_قدر(thr, value);

      vvp_net_t*net = cp->net;
      أكد(net);

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      أكد(obj);

      vvp_darray*darray = obj->get_object().peek<vvp_darray>();

      إذا (adr < 0)
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to a negative " << get_darray_type(value)
	         << " index (" << adr << ")." << سٽ_پڄاڻي;
      وإلا إذا (thr->flags[4] != BIT4_0)
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to an undefined " << get_darray_type(value)
	         << " index." << سٽ_پڄاڻي;
      وإلا إذا (darray)
	    darray->سيٽ_لفظ(adr, value);
      وإلا
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to an undefined " << get_darray_type(value)
	         << "." << سٽ_پڄاڻي;

      أرجع سچ;
}

/*
 * %store/dar/real <var>
 */
منطقي جو_STORE_DAR_R(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع store_dar<مزدوج>(thr, cp);
}

/*
 * %store/dar/str <var>
 */
منطقي جو_STORE_DAR_STR(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع store_dar<string>(thr, cp);
}

/*
 * %store/dar/vec4 <var>
 */
منطقي جو_STORE_DAR_VEC4(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع store_dar<vvp_vector4_t>(thr, cp);
}

منطقي جو_STORE_OBJ(vthread_t thr, vvp_ڪوڊ_t cp)
{
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);

      vvp_object_t val;
      thr->ڪڍو_object(val);

      vvp_send_object(ptr, val, thr->wt_context);

      أرجع سچ;
}

/*
 * %store/obja <array-label> <index>
 */
منطقي جو_STORE_OBJA(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان idx = cp->ٻٽو_انگ_idx[0];
      بنانشان adr = thr->لفظ[idx].w_int;

      vvp_object_t val;
      thr->ڪڍو_object(val);

      cp->array->سيٽ_لفظ(adr, val);

      أرجع سچ;
}


/*
 * %store/prop/obj <pid>, <idx>
 *
 * Pop an object value from the object stack, and store the value into
 * the property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
منطقي جو_STORE_PROP_OBJ(vthread_t thr, vvp_ڪوڊ_t cp)
{
      size_t pid = cp->عدد;
      بنانشان idx = cp->ٻٽو_انگ_idx[0];

      إذا (idx != 0) {
	    أكد(idx < vthread_s::لفظ_ڳڻپ);
	    idx = thr->لفظ[idx].w_uint;
      }

      vvp_object_t val;
      thr->ڪڍو_object(val);

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      أكد(cobj);

      cobj->سيٽ_object(pid, val, idx);

      أرجع سچ;
}

ساكن فراغ ڪڍو_prop_val(vthread_t thr, مزدوج&val, بنانشان)
{
      val = thr->ڪڍو_real();
}

ساكن فراغ ڪڍو_prop_val(vthread_t thr, string&val, بنانشان)
{
      val = thr->ڪڍو_str();
}

ساكن فراغ ڪڍو_prop_val(vthread_t thr, vvp_vector4_t&val, بنانشان wid)
{
      val = thr->ڪڍو_vec4();
      أكد(val.size() >= wid);
      val.resize(wid);
}

ساكن فراغ سيٽ_val(vvp_cobject*cobj, size_t pid, ثابت مزدوج&val)
{
      cobj->سيٽ_real(pid, val);
}

ساكن فراغ سيٽ_val(vvp_cobject*cobj, size_t pid, ثابت string&val)
{
      cobj->سيٽ_string(pid, val);
}

ساكن فراغ سيٽ_val(vvp_cobject*cobj, size_t pid, ثابت vvp_vector4_t&val)
{
      cobj->سيٽ_vec4(pid, val);
}

سانچو <قسم_نالو ELEM>
ساكن منطقي store_prop(vthread_t thr, vvp_ڪوڊ_t cp, بنانشان wid=0)
{
      size_t pid = cp->عدد;
      ELEM val;
      ڪڍو_prop_val(thr, val, wid); // Pop the value to store.

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      أكد(cobj);

      سيٽ_val(cobj, pid, val);

      أرجع سچ;
}

/*
 * %store/prop/r <id>
 *
 * Pop a real value from the real stack, and store the value into the
 * property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
منطقي جو_STORE_PROP_R(vthread_t thr, vvp_ڪوڊ_t cp)
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
منطقي جو_STORE_PROP_STR(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع store_prop<string>(thr, cp);
}

/*
 * %store/prop/v <pid>, <wid>
 *
 * Store vector value into property <id> of cobject in the top of the
 * stack. Do NOT pop the object stack.
 */
منطقي جو_STORE_PROP_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع store_prop<vvp_vector4_t>(thr, cp, cp->ٻٽو_انگ_idx[0]);
}

سانچو <صنف QTYPE>
ساكن QTYPE* get_قطار_prop(vvp_cobject*cobj, size_t pid)
{
      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      QTYPE* قطار = qobj.peek<QTYPE>();
      إذا (قطار == 0) {
	    قطار = جديد QTYPE;
	    vvp_object_t val(قطار);
	    cobj->سيٽ_object(pid, val, 0);
      }
      أرجع قطار;
}

سانچو <قسم_نالو ELEM, صنف QTYPE>
ساكن منطقي store_prop_qb(vthread_t thr, vvp_ڪوڊ_t cp, بنانشان wid)
{
      size_t pid = cp->عدد;
      ELEM value;
      بنانشان max_سائيز = thr->لفظ[cp->ٻٽو_انگ_idx[0]].w_uint;
      ڪڍو_قدر(thr, value, wid);

      vvp_object_t& چوٽي = thr->peek_object();
      vvp_cobject*cobj = چوٽي.peek<vvp_cobject>();
      أكد(cobj);

      QTYPE* قطار = get_قطار_prop<QTYPE>(cobj, pid);
      أكد(قطار);
      قطار->push_back(value, max_سائيز);
      أرجع سچ;
}

سانچو <قسم_نالو ELEM, صنف QTYPE>
ساكن منطقي store_prop_qf(vthread_t thr, vvp_ڪوڊ_t cp, بنانشان wid)
{
      size_t pid = cp->عدد;
      ELEM value;
      بنانشان max_سائيز = thr->لفظ[cp->ٻٽو_انگ_idx[0]].w_uint;
      ڪڍو_قدر(thr, value, wid);

      vvp_object_t& چوٽي = thr->peek_object();
      vvp_cobject*cobj = چوٽي.peek<vvp_cobject>();
      أكد(cobj);

      QTYPE* قطار = get_قطار_prop<QTYPE>(cobj, pid);
      أكد(قطار);
      قطار->push_front(value, max_سائيز);
      أرجع سچ;
}

منطقي جو_STORE_PROP_QB_R(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع store_prop_qb<مزدوج, vvp_قطار_real>(thr, cp, 0);
}

منطقي جو_STORE_PROP_QB_STR(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع store_prop_qb<string, vvp_قطار_string>(thr, cp, 0);
}

منطقي جو_STORE_PROP_QB_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع store_prop_qb<vvp_vector4_t, vvp_قطار_vec4>(thr, cp, cp->ٻٽو_انگ_idx[1]);
}

منطقي جو_STORE_PROP_QF_R(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع store_prop_qf<مزدوج, vvp_قطار_real>(thr, cp, 0);
}

منطقي جو_STORE_PROP_QF_STR(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع store_prop_qf<string, vvp_قطار_string>(thr, cp, 0);
}

منطقي جو_STORE_PROP_QF_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع store_prop_qf<vvp_vector4_t, vvp_قطار_vec4>(thr, cp, cp->ٻٽو_انگ_idx[1]);
}

سانچو <قسم_نالو ELEM, صنف QTYPE>
ساكن منطقي qinsert_prop(vthread_t thr, vvp_ڪوڊ_t cp, بنانشان wid)
{
      int64_t idx = thr->لفظ[3].w_int;
      ELEM value;
      size_t pid = cp->عدد;
      ڪڍو_قدر(thr, value, wid);

      vvp_object_t& چوٽي = thr->peek_object();
      vvp_cobject*cobj = چوٽي.peek<vvp_cobject>();
      أكد(cobj);

      QTYPE* قطار = get_قطار_prop<QTYPE>(cobj, pid);
      أكد(قطار);
      إذا (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at a negative "
	         << get_قطار_type(value)
	         << " index (" << idx << "). ";
	    print_قطار_قدر(value);
	    cerr << " was not added." << سٽ_پڄاڻي;
      } وإلا إذا (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at an undefined "
	         << get_قطار_type(value) << " index. ";
	    print_قطار_قدر(value);
	    cerr << " was not added." << سٽ_پڄاڻي;
      } وإلا {
	    بنانشان max_سائيز = thr->لفظ[cp->ٻٽو_انگ_idx[0]].w_int;
	    قطار->insert(idx, value, max_سائيز);
      }
      أرجع سچ;
}

منطقي جو_QINSERT_PROP_R(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع qinsert_prop<مزدوج, vvp_قطار_real>(thr, cp, 0);
}

منطقي جو_QINSERT_PROP_STR(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع qinsert_prop<string, vvp_قطار_string>(thr, cp, 0);
}

منطقي جو_QINSERT_PROP_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع qinsert_prop<vvp_vector4_t, vvp_قطار_vec4>(thr, cp, cp->ٻٽو_انگ_idx[1]);
}

سانچو <قسم_نالو ELEM, صنف QTYPE>
ساكن منطقي q_ڪڍو_prop(vthread_t thr, vvp_ڪوڊ_t cp,
                       فراغ (*get_val_func)(vvp_قطار*, ELEM&),
                       ثابت حرف*loc, بنانشان wid)
{
      size_t pid = cp->عدد;

      vvp_object_t& چوٽي = thr->peek_object();
      vvp_cobject*cobj = چوٽي.peek<vvp_cobject>();
      أكد(cobj);

      QTYPE* قطار = get_قطار_prop<QTYPE>(cobj, pid);
      أكد(قطار);

      size_t size = قطار->get_سائيز();

      ELEM value;
      إذا (size) {
	    get_val_func(قطار, value);
      } وإلا {
	    dq_default(value, wid);
	    cerr << thr->get_fileline()
	         << "Warning: pop_" << loc << "() on empty "
	         << get_قطار_type(value) << "." << سٽ_پڄاڻي;
      }

      ڌڪو_قدر(thr, value, wid);
      أرجع سچ;
}

سانچو <قسم_نالو ELEM, صنف QTYPE>
ساكن منطقي qpop_b_prop(vthread_t thr, vvp_ڪوڊ_t cp, بنانشان wid)
{
      أرجع q_ڪڍو_prop<ELEM, QTYPE>(thr, cp, get_پٺ_قدر<ELEM>, "back", wid);
}

سانچو <قسم_نالو ELEM, صنف QTYPE>
ساكن منطقي qpop_f_prop(vthread_t thr, vvp_ڪوڊ_t cp, بنانشان wid)
{
      أرجع q_ڪڍو_prop<ELEM, QTYPE>(thr, cp, get_front_قدر<ELEM>, "front", wid);
}

منطقي جو_QPOP_PROP_B_REAL(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع qpop_b_prop<مزدوج, vvp_قطار_real>(thr, cp, 0);
}

منطقي جو_QPOP_PROP_B_STR(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع qpop_b_prop<string, vvp_قطار_string>(thr, cp, 0);
}

منطقي جو_QPOP_PROP_B_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع qpop_b_prop<vvp_vector4_t, vvp_قطار_vec4>(thr, cp, cp->ٻٽو_انگ_idx[0]);
}

منطقي جو_QPOP_PROP_F_REAL(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع qpop_f_prop<مزدوج, vvp_قطار_real>(thr, cp, 0);
}

منطقي جو_QPOP_PROP_F_STR(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع qpop_f_prop<string, vvp_قطار_string>(thr, cp, 0);
}

منطقي جو_QPOP_PROP_F_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع qpop_f_prop<vvp_vector4_t, vvp_قطار_vec4>(thr, cp, cp->ٻٽو_انگ_idx[0]);
}

ساكن vvp_vector4_t قطار_unique_ulong_منزل_vec4(بنانشان ڊگهو ul, بنانشان بٽ)
{
      vvp_vector4_t val(بٽ, BIT4_0);
      لكل (بنانشان idx = 0; idx < بٽ && idx < 8 * حجم(بنانشان ڊگهو); idx += 1) {
	    إذا (ul & 1UL) {
		  val.سيٽ_ٻٽو_انگ(idx, BIT4_1);
	    } وإلا {
		  val.سيٽ_ٻٽو_انگ(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      أرجع val;
}

سانچو <صنف SRC> ساكن vvp_قطار_vec4* قطار_لاڳيتو_unique_src(SRC* src,
								 بنانشان wid,
								 منطقي as_اشاريو)
{
      vvp_قطار_vec4* dst = جديد vvp_قطار_vec4();
      إذا (!src) أرجع dst;
      size_t n = src->get_سائيز();
      لكل (size_t i = 0; i < n; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_لفظ(i, vi);
	    منطقي ڏٺل = خطأ;
	    لكل (size_t j = 0; j < i; j += 1) {
		  vvp_vector4_t vj(wid);
		  src->get_لفظ(j, vj);
		  إذا (vi.eeq(vj)) {
			ڏٺل = سچ;
			اكسر;
		  }
	    }
	    إذا (ڏٺل) استمر;
	    إذا (as_اشاريو) {
		  dst->push_back(قطار_unique_ulong_منزل_vec4((بنانشان ڊگهو)i, 32), 0);
	    } وإلا {
		  dst->push_back(vi, 0);
	    }
      }
      أرجع dst;
}

سانچو <صنف SRC>
ساكن vvp_قطار_vec4* قطار_لاڳيتو_ڳوليو_matches_src(SRC* src, بنانشان wid,
						  ثابت vvp_vector4_t& cmp,
						  منطقي as_اشاريو)
{
      vvp_قطار_vec4* dst = جديد vvp_قطار_vec4();
      إذا (!src) أرجع dst;
      size_t n = src->get_سائيز();
      لكل (size_t i = 0; i < n; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_لفظ(i, vi);
	    إذا (vi.eeq(cmp)) {
		  إذا (as_اشاريو) {
			dst->push_back(قطار_unique_ulong_منزل_vec4((بنانشان ڊگهو)i, 32), 0);
		  } وإلا {
			dst->push_back(vi, 0);
		  }
	    }
      }
      أرجع dst;
}

/* Array/queue locator methods return a queue with 0 or 1 elements (LRM). */
سانچو <صنف SRC>
ساكن vvp_قطار_vec4* قطار_ڳوليو_پهريون_آخري_منزل_قطار_src(SRC* src,
							  بنانشان wid,
							  ثابت vvp_vector4_t& cmp,
							  منطقي want_پهريون,
							  منطقي want_اشاريو)
{
      vvp_قطار_vec4* dst = جديد vvp_قطار_vec4();
      إذا (!src || src->get_سائيز() == 0) أرجع dst;
      size_t n = src->get_سائيز();
      إذا (want_پهريون) {
	    لكل (size_t i = 0; i < n; i += 1) {
		  vvp_vector4_t vi(wid);
		  src->get_لفظ(i, vi);
		  إذا (vi.eeq(cmp)) {
			إذا (want_اشاريو) {
			      dst->push_back(
				  قطار_unique_ulong_منزل_vec4((بنانشان ڊگهو)i, 32),
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
			إذا (want_اشاريو) {
			      dst->push_back(
				  قطار_unique_ulong_منزل_vec4((بنانشان ڊگهو)i, 32),
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
سانچو <صنف SRC>
ساكن vvp_قطار_vec4* قطار_لاڳيتو_min_max_src(SRC* src, بنانشان wid, منطقي want_max)
{
      vvp_قطار_vec4* dst = جديد vvp_قطار_vec4();
      إذا (!src || src->get_سائيز() == 0) أرجع dst;

      vvp_vector4_t بهترين(wid);
      src->get_لفظ(0, بهترين);

      size_t n = src->get_سائيز();
      لكل (size_t i = 1 ; i < n ; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_لفظ(i, vi);

	    vvp_bit4_t ge = compare_gtge(vi, بهترين, BIT4_1);
	    منطقي replace = want_max ? (ge == BIT4_1) : (ge == BIT4_0);
	    إذا (replace) بهترين = vi;
      }

      dst->push_back(بهترين, 0);
      أرجع dst;
}

منطقي جو_قطار_ڳوليو_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_vector4_t cmp = thr->ڪڍو_vec4();
      بنانشان wid = cp->ٻٽو_انگ_idx[0];
      أكد(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_قطار_vec4* dst;
      إذا (qsrc) dst = قطار_لاڳيتو_ڳوليو_matches_src(qsrc, wid, cmp, خطأ);
      وإلا إذا (dsrc) dst = قطار_لاڳيتو_ڳوليو_matches_src(dsrc, wid, cmp, خطأ);
      وإلا {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    dst = قطار_لاڳيتو_ڳوليو_matches_src(src, wid, cmp, خطأ);
      }
      thr->ڌڪو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي جو_قطار_ڳوليو_PROP_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_vector4_t cmp = thr->ڪڍو_vec4();
      size_t pid = cp->عدد;
      بنانشان wid = cp->ٻٽو_انگ_idx[0];
      أكد(cmp.size() == wid);

      vvp_object_t& چوٽي = thr->peek_object();
      vvp_cobject*cobj = چوٽي.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_قطار_vec4* dst = qsrc ? قطار_لاڳيتو_ڳوليو_matches_src(qsrc, wid, cmp, خطأ)
				 : قطار_لاڳيتو_ڳوليو_matches_src(dsrc, wid, cmp, خطأ);
      thr->ڌڪو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي جو_قطار_ڳوليو_اشاريو_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_vector4_t cmp = thr->ڪڍو_vec4();
      بنانشان wid = cp->ٻٽو_انگ_idx[0];
      أكد(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_قطار_vec4* dst;
      إذا (qsrc) dst = قطار_لاڳيتو_ڳوليو_matches_src(qsrc, wid, cmp, سچ);
      وإلا إذا (dsrc) dst = قطار_لاڳيتو_ڳوليو_matches_src(dsrc, wid, cmp, سچ);
      وإلا {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    dst = قطار_لاڳيتو_ڳوليو_matches_src(src, wid, cmp, سچ);
      }
      thr->ڌڪو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي جو_قطار_ڳوليو_اشاريو_PROP_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_vector4_t cmp = thr->ڪڍو_vec4();
      size_t pid = cp->عدد;
      بنانشان wid = cp->ٻٽو_انگ_idx[0];
      أكد(cmp.size() == wid);

      vvp_object_t& چوٽي = thr->peek_object();
      vvp_cobject*cobj = چوٽي.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_قطار_vec4* dst = qsrc ? قطار_لاڳيتو_ڳوليو_matches_src(qsrc, wid, cmp, سچ)
				 : قطار_لاڳيتو_ڳوليو_matches_src(dsrc, wid, cmp, سچ);
      thr->ڌڪو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي جو_قطار_ڳوليو_پهريون_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_vector4_t cmp = thr->ڪڍو_vec4();
      بنانشان wid = cp->ٻٽو_انگ_idx[0];
      أكد(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_قطار_vec4* dst;
      إذا (qsrc) dst = قطار_ڳوليو_پهريون_آخري_منزل_قطار_src(qsrc, wid, cmp, سچ, خطأ);
      وإلا إذا (dsrc) dst = قطار_ڳوليو_پهريون_آخري_منزل_قطار_src(dsrc, wid, cmp, سچ, خطأ);
      وإلا {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    dst = قطار_ڳوليو_پهريون_آخري_منزل_قطار_src(src, wid, cmp, سچ, خطأ);
      }
      thr->ڌڪو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي جو_قطار_ڳوليو_پهريون_PROP_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_vector4_t cmp = thr->ڪڍو_vec4();
      size_t pid = cp->عدد;
      بنانشان wid = cp->ٻٽو_انگ_idx[0];
      أكد(cmp.size() == wid);

      vvp_object_t& چوٽي = thr->peek_object();
      vvp_cobject*cobj = چوٽي.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_قطار_vec4* dst =
	    qsrc ? قطار_ڳوليو_پهريون_آخري_منزل_قطار_src(qsrc, wid, cmp, سچ, خطأ)
		 : قطار_ڳوليو_پهريون_آخري_منزل_قطار_src(dsrc, wid, cmp, سچ, خطأ);
      thr->ڌڪو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي جو_قطار_ڳوليو_پهريون_اشاريو_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_vector4_t cmp = thr->ڪڍو_vec4();
      بنانشان wid = cp->ٻٽو_انگ_idx[0];
      أكد(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_قطار_vec4* dst;
      إذا (qsrc) dst = قطار_ڳوليو_پهريون_آخري_منزل_قطار_src(qsrc, wid, cmp, سچ, سچ);
      وإلا إذا (dsrc) dst = قطار_ڳوليو_پهريون_آخري_منزل_قطار_src(dsrc, wid, cmp, سچ, سچ);
      وإلا {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    dst = قطار_ڳوليو_پهريون_آخري_منزل_قطار_src(src, wid, cmp, سچ, سچ);
      }
      thr->ڌڪو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي جو_قطار_ڳوليو_پهريون_اشاريو_PROP_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_vector4_t cmp = thr->ڪڍو_vec4();
      size_t pid = cp->عدد;
      بنانشان wid = cp->ٻٽو_انگ_idx[0];
      أكد(cmp.size() == wid);

      vvp_object_t& چوٽي = thr->peek_object();
      vvp_cobject*cobj = چوٽي.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_قطار_vec4* dst =
	    qsrc ? قطار_ڳوليو_پهريون_آخري_منزل_قطار_src(qsrc, wid, cmp, سچ, سچ)
		 : قطار_ڳوليو_پهريون_آخري_منزل_قطار_src(dsrc, wid, cmp, سچ, سچ);
      thr->ڌڪو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي جو_قطار_ڳوليو_آخري_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_vector4_t cmp = thr->ڪڍو_vec4();
      بنانشان wid = cp->ٻٽو_انگ_idx[0];
      أكد(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_قطار_vec4* dst;
      إذا (qsrc) dst = قطار_ڳوليو_پهريون_آخري_منزل_قطار_src(qsrc, wid, cmp, خطأ, خطأ);
      وإلا إذا (dsrc) dst = قطار_ڳوليو_پهريون_آخري_منزل_قطار_src(dsrc, wid, cmp, خطأ, خطأ);
      وإلا {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    dst = قطار_ڳوليو_پهريون_آخري_منزل_قطار_src(src, wid, cmp, خطأ, خطأ);
      }
      thr->ڌڪو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي جو_قطار_ڳوليو_آخري_PROP_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_vector4_t cmp = thr->ڪڍو_vec4();
      size_t pid = cp->عدد;
      بنانشان wid = cp->ٻٽو_انگ_idx[0];
      أكد(cmp.size() == wid);

      vvp_object_t& چوٽي = thr->peek_object();
      vvp_cobject*cobj = چوٽي.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_قطار_vec4* dst =
	    qsrc ? قطار_ڳوليو_پهريون_آخري_منزل_قطار_src(qsrc, wid, cmp, خطأ, خطأ)
		 : قطار_ڳوليو_پهريون_آخري_منزل_قطار_src(dsrc, wid, cmp, خطأ, خطأ);
      thr->ڌڪو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي جو_قطار_ڳوليو_آخري_اشاريو_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_vector4_t cmp = thr->ڪڍو_vec4();
      بنانشان wid = cp->ٻٽو_انگ_idx[0];
      أكد(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_قطار_vec4* dst;
      إذا (qsrc) dst = قطار_ڳوليو_پهريون_آخري_منزل_قطار_src(qsrc, wid, cmp, خطأ, سچ);
      وإلا إذا (dsrc) dst = قطار_ڳوليو_پهريون_آخري_منزل_قطار_src(dsrc, wid, cmp, خطأ, سچ);
      وإلا {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    dst = قطار_ڳوليو_پهريون_آخري_منزل_قطار_src(src, wid, cmp, خطأ, سچ);
      }
      thr->ڌڪو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي جو_قطار_ڳوليو_آخري_اشاريو_PROP_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_vector4_t cmp = thr->ڪڍو_vec4();
      size_t pid = cp->عدد;
      بنانشان wid = cp->ٻٽو_انگ_idx[0];
      أكد(cmp.size() == wid);

      vvp_object_t& چوٽي = thr->peek_object();
      vvp_cobject*cobj = چوٽي.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_قطار_vec4* dst =
	    qsrc ? قطار_ڳوليو_پهريون_آخري_منزل_قطار_src(qsrc, wid, cmp, خطأ, سچ)
		 : قطار_ڳوليو_پهريون_آخري_منزل_قطار_src(dsrc, wid, cmp, خطأ, سچ);
      thr->ڌڪو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي جو_قطار_MIN_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_net_t* net = cp->net;
      بنانشان wid = cp->ٻٽو_انگ_idx[0];
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_قطار_vec4* dst;
      إذا (qsrc) dst = قطار_لاڳيتو_min_max_src(qsrc, wid, خطأ);
      وإلا إذا (dsrc) dst = قطار_لاڳيتو_min_max_src(dsrc, wid, خطأ);
      وإلا {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    dst = قطار_لاڳيتو_min_max_src(src, wid, خطأ);
      }
      thr->ڌڪو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي جو_قطار_MIN_OBJ_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان wid = cp->ٻٽو_انگ_idx[0];
      vvp_object_t src_obj;
      thr->ڪڍو_object(src_obj);

      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_قطار_vec4* dst = qsrc ? قطار_لاڳيتو_min_max_src(qsrc, wid, خطأ)
				 : قطار_لاڳيتو_min_max_src(dsrc, wid, خطأ);
      thr->ڌڪو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي جو_قطار_MIN_PROP_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      size_t pid = cp->عدد;
      بنانشان wid = cp->ٻٽو_انگ_idx[0];

      vvp_object_t& چوٽي = thr->peek_object();
      vvp_cobject*cobj = چوٽي.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_قطار_vec4* dst = qsrc ? قطار_لاڳيتو_min_max_src(qsrc, wid, خطأ)
				 : قطار_لاڳيتو_min_max_src(dsrc, wid, خطأ);
      thr->ڌڪو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي جو_قطار_MAX_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_net_t* net = cp->net;
      بنانشان wid = cp->ٻٽو_انگ_idx[0];
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_قطار_vec4* dst;
      إذا (qsrc) dst = قطار_لاڳيتو_min_max_src(qsrc, wid, سچ);
      وإلا إذا (dsrc) dst = قطار_لاڳيتو_min_max_src(dsrc, wid, سچ);
      وإلا {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    dst = قطار_لاڳيتو_min_max_src(src, wid, سچ);
      }
      thr->ڌڪو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي جو_قطار_MAX_OBJ_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان wid = cp->ٻٽو_انگ_idx[0];
      vvp_object_t src_obj;
      thr->ڪڍو_object(src_obj);

      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_قطار_vec4* dst = qsrc ? قطار_لاڳيتو_min_max_src(qsrc, wid, سچ)
				 : قطار_لاڳيتو_min_max_src(dsrc, wid, سچ);
      thr->ڌڪو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي جو_قطار_MAX_PROP_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      size_t pid = cp->عدد;
      بنانشان wid = cp->ٻٽو_انگ_idx[0];

      vvp_object_t& چوٽي = thr->peek_object();
      vvp_cobject*cobj = چوٽي.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_قطار_vec4* dst = qsrc ? قطار_لاڳيتو_min_max_src(qsrc, wid, سچ)
				 : قطار_لاڳيتو_min_max_src(dsrc, wid, سچ);
      thr->ڌڪو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي جو_قطار_UNIQUE_OBJ_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان wid = cp->ٻٽو_انگ_idx[0];
      vvp_object_t src_obj;
      thr->ڪڍو_object(src_obj);

      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_قطار_vec4* dst = qsrc ? قطار_لاڳيتو_unique_src(qsrc, wid, خطأ)
				 : قطار_لاڳيتو_unique_src(dsrc, wid, خطأ);
      thr->ڌڪو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي جو_قطار_UNIQUE_اشاريو_OBJ_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان wid = cp->ٻٽو_انگ_idx[0];
      vvp_object_t src_obj;
      thr->ڪڍو_object(src_obj);

      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_قطار_vec4* dst = qsrc ? قطار_لاڳيتو_unique_src(qsrc, wid, سچ)
				 : قطار_لاڳيتو_unique_src(dsrc, wid, سچ);
      thr->ڌڪو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي جو_قطار_UNIQUE_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_net_t*net = cp->net;
      بنانشان wid = cp->ٻٽو_انگ_idx[0];
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_قطار_vec4* dst;
      إذا (qsrc) dst = قطار_لاڳيتو_unique_src(qsrc, wid, خطأ);
      وإلا إذا (dsrc) dst = قطار_لاڳيتو_unique_src(dsrc, wid, خطأ);
      وإلا {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    dst = قطار_لاڳيتو_unique_src(src, wid, خطأ);
      }
      thr->ڌڪو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي جو_قطار_UNIQUE_PROP_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      size_t pid = cp->عدد;
      بنانشان wid = cp->ٻٽو_انگ_idx[0];

      vvp_object_t& چوٽي = thr->peek_object();
      vvp_cobject*cobj = چوٽي.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_قطار_vec4* dst = qsrc ? قطار_لاڳيتو_unique_src(qsrc, wid, خطأ)
				 : قطار_لاڳيتو_unique_src(dsrc, wid, خطأ);
      thr->ڌڪو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي جو_قطار_UNIQUE_اشاريو_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_net_t*net = cp->net;
      بنانشان wid = cp->ٻٽو_انگ_idx[0];
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_قطار_vec4* dst;
      إذا (qsrc) dst = قطار_لاڳيتو_unique_src(qsrc, wid, سچ);
      وإلا إذا (dsrc) dst = قطار_لاڳيتو_unique_src(dsrc, wid, سچ);
      وإلا {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    dst = قطار_لاڳيتو_unique_src(src, wid, سچ);
      }
      thr->ڌڪو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي جو_قطار_UNIQUE_اشاريو_PROP_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      size_t pid = cp->عدد;
      بنانشان wid = cp->ٻٽو_انگ_idx[0];

      vvp_object_t& چوٽي = thr->peek_object();
      vvp_cobject*cobj = چوٽي.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_قطار_vec4* dst = qsrc ? قطار_لاڳيتو_unique_src(qsrc, wid, سچ)
				 : قطار_لاڳيتو_unique_src(dsrc, wid, سچ);
      thr->ڌڪو_object(vvp_object_t(dst));
      أرجع سچ;
}

منطقي جو_PROP_قطار_سائيز(vthread_t thr, vvp_ڪوڊ_t cp)
{
      size_t pid = cp->عدد;

      vvp_object_t& چوٽي = thr->peek_object();
      vvp_cobject*cobj = چوٽي.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(qobj, qsrc, dsrc);

      size_t sz = qsrc ? qsrc->get_سائيز() : (dsrc ? dsrc->get_سائيز() : 0);

      vvp_vector4_t val(32, BIT4_0);
      بنانشان ڊگهو ul = sz;
      لكل (بنانشان idx = 0; idx < 32; idx++) {
	    إذا (ul & 1UL) {
		  val.سيٽ_ٻٽو_انگ(idx, BIT4_1);
	    } وإلا {
		  val.سيٽ_ٻٽو_انگ(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      thr->ڌڪو_vec4(val);
      أرجع سچ;
}

منطقي جو_قطار_سائيز_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      size_t sz =
	    qsrc ? qsrc->get_سائيز() : (dsrc ? dsrc->get_سائيز() : 0);

      vvp_vector4_t val(32, BIT4_0);
      بنانشان ڊگهو ul = sz;
      لكل (بنانشان idx = 0; idx < 32; idx++) {
	    إذا (ul & 1UL) {
		  val.سيٽ_ٻٽو_انگ(idx, BIT4_1);
	    } وإلا {
		  val.سيٽ_ٻٽو_انگ(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      thr->ڌڪو_vec4(val);
      أرجع سچ;
}

منطقي جو_قطار_لفظ_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان wid = cp->ٻٽو_انگ_idx[0];
      بنانشان ix_reg = cp->ٻٽو_انگ_idx[1];
      int64_t adr = thr->لفظ[ix_reg].w_int;
      إذا (thr->flags[4] == BIT4_1) {
	    thr->ڌڪو_vec4(vvp_vector4_t(wid));
	    أرجع سچ;
      }
      إذا (adr < 0) {
	    thr->ڌڪو_vec4(vvp_vector4_t(wid));
	    أرجع سچ;
      }
      vvp_net_t* net = cp->net;
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t out(wid);
      إذا (qsrc) {
	    إذا ((size_t)adr >= qsrc->get_سائيز()) {
		  thr->ڌڪو_vec4(out);
		  أرجع سچ;
	    }
	    qsrc->get_لفظ((بنانشان)adr, out);
      } وإلا إذا (dsrc) {
	    إذا ((size_t)adr >= dsrc->get_سائيز()) {
		  thr->ڌڪو_vec4(out);
		  أرجع سچ;
	    }
	    dsrc->get_لفظ((بنانشان)adr, out);
      } وإلا {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    إذا (!src || (size_t)adr >= src->get_سائيز()) {
		  thr->ڌڪو_vec4(out);
		  أرجع سچ;
	    }
	    src->get_لفظ((بنانشان)adr, out);
      }
      thr->ڌڪو_vec4(out);
      أرجع سچ;
}

منطقي جو_قطار_لفظ_PROP_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      size_t pid = cp->عدد;
      بنانشان wid = cp->ٻٽو_انگ_idx[0];
      بنانشان ix_reg = cp->ٻٽو_انگ_idx[1];
      int64_t adr = thr->لفظ[ix_reg].w_int;
      إذا (thr->flags[4] == BIT4_1) {
	    thr->ڌڪو_vec4(vvp_vector4_t(wid));
	    أرجع سچ;
      }
      إذا (adr < 0) {
	    thr->ڌڪو_vec4(vvp_vector4_t(wid));
	    أرجع سچ;
      }

      vvp_object_t saved_چوٽي;
      منطقي popped_چوٽي = خطأ;
      vvp_object_t& چوٽي = thr->peek_object();
      vvp_cobject*cobj = چوٽي.peek<vvp_cobject>();
      إذا (cobj == 0) {
	    // Array-locator with(property) keeps an accumulator queue on top.
	    // Temporarily pop it to access the class object beneath.
	    thr->ڪڍو_object(saved_چوٽي);
	    popped_چوٽي = سچ;
	    vvp_object_t& هيٺ = thr->peek_object();
	    cobj = هيٺ.peek<vvp_cobject>();
      }
      إذا (cobj == 0) {
	    إذا (popped_چوٽي) thr->ڌڪو_object(saved_چوٽي);
	    thr->ڌڪو_vec4(vvp_vector4_t(wid));
	    أرجع سچ;
      }

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t out(wid);
      إذا (qsrc) {
	    إذا ((size_t)adr >= qsrc->get_سائيز()) {
		  thr->ڌڪو_vec4(out);
		  أرجع سچ;
	    }
	    qsrc->get_لفظ((بنانشان)adr, out);
      } وإلا إذا (dsrc) {
	    إذا ((size_t)adr >= dsrc->get_سائيز()) {
		  thr->ڌڪو_vec4(out);
		  أرجع سچ;
	    }
	    dsrc->get_لفظ((بنانشان)adr, out);
      } وإلا {
	    thr->ڌڪو_vec4(out);
	    إذا (popped_چوٽي) thr->ڌڪو_object(saved_چوٽي);
	    أرجع سچ;
      }
      إذا (popped_چوٽي) thr->ڌڪو_object(saved_چوٽي);
      thr->ڌڪو_vec4(out);
      أرجع سچ;
}

منطقي جو_CMPIX_LTU(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان a = cp->عدد;
      بنانشان b = cp->ٻٽو_انگ_idx[0];
      إذا (thr->لفظ[a].w_uint < thr->لفظ[b].w_uint) {
	    thr->flags[4] = BIT4_1;
      } وإلا {
	    thr->flags[4] = BIT4_0;
      }
      أرجع سچ;
}

منطقي جو_CMPIX_SLT0(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان r = cp->عدد;
      thr->flags[4] = (thr->لفظ[r].w_int < 0) ? BIT4_1 : BIT4_0;
      أرجع سچ;
}

منطقي جو_ڌڪو_IX_VEC4(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان ix = cp->عدد;
      بنانشان wid = cp->ٻٽو_انگ_idx[0];
      بنانشان sgn = cp->ٻٽو_انگ_idx[1];
      uint32_t ubits;
      إذا (sgn) {
	    int32_t sv = (int32_t)thr->لفظ[ix].w_int;
	    ubits = (uint32_t)sv;
      } وإلا {
	    ubits = (uint32_t)thr->لفظ[ix].w_uint;
      }
      إذا (wid > 32) wid = 32;
      vvp_vector4_t tmp(wid, BIT4_0);
      لكل (بنانشان idx = 0; idx < wid; idx++) {
	    إذا (ubits & 1u) {
		  tmp.سيٽ_ٻٽو_انگ(idx, BIT4_1);
	    } وإلا {
		  tmp.سيٽ_ٻٽو_انگ(idx, BIT4_0);
	    }
	    ubits >>= 1;
      }
      thr->ڌڪو_vec4(tmp);
      أرجع سچ;
}

منطقي جو_قطار_NEW_خالي_جايون_V(vthread_t thr, vvp_ڪوڊ_t)
{
      thr->ڌڪو_object(vvp_object_t(جديد vvp_قطار_vec4()));
      أرجع سچ;
}

منطقي جو_قطار_APPEND_لفظ_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان wid = cp->ٻٽو_انگ_idx[0];
      vvp_vector4_t elem = thr->ڪڍو_vec4();
      أكد(elem.size() == wid);
      vvp_object_t qobj;
      thr->ڪڍو_object(qobj);
      vvp_قطار_vec4* q = qobj.peek<vvp_قطار_vec4>();
      أكد(q);
      q->push_back(elem, 0);
      thr->ڌڪو_object(qobj);
      أرجع سچ;
}

منطقي جو_DELETE_PROP_ELEM(vthread_t thr, vvp_ڪوڊ_t cp)
{
      size_t pid = cp->عدد;

      int64_t idx_val = thr->لفظ[3].w_int;
      إذا (thr->flags[4] == BIT4_1) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with undefined index."
	         << سٽ_پڄاڻي;
	    أرجع سچ;
      }
      إذا (idx_val < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with negative index."
	         << سٽ_پڄاڻي;
	    أرجع سچ;
      }
      size_t idx = idx_val;

      vvp_object_t& چوٽي = thr->peek_object();
      vvp_cobject*cobj = چوٽي.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_قطار* قطار = qobj.peek<vvp_قطار>();
      إذا (قطار == 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping delete(" << idx
	         << ") on empty queue." << سٽ_پڄاڻي;
      } وإلا {
	    size_t size = قطار->get_سائيز();
	    إذا (idx >= size) {
		  cerr << thr->get_fileline()
		       << "Warning: skipping out of range delete(" << idx
		       << ") on queue of size " << size << "." << سٽ_پڄاڻي;
	    } وإلا {
		  قطار->erase(idx);
	    }
      }

      أرجع سچ;
}

منطقي جو_DELETE_PROP_OBJ(vthread_t thr, vvp_ڪوڊ_t cp)
{
      size_t pid = cp->عدد;

      vvp_object_t& چوٽي = thr->peek_object();
      vvp_cobject*cobj = چوٽي.peek<vvp_cobject>();
      أكد(cobj);

      cobj->سيٽ_object(pid, vvp_object_t(), 0);

      أرجع سچ;
}

سانچو <قسم_نالو ELEM, صنف QTYPE>
ساكن منطقي store_qb(vthread_t thr, vvp_ڪوڊ_t cp, بنانشان wid=0)
{
      ELEM value;
      vvp_net_t*net = cp->net;
      بنانشان max_سائيز = thr->لفظ[cp->ٻٽو_انگ_idx[0]].w_int;
      ڪڍو_قدر(thr, value, wid); // Pop the value to store.

      vvp_قطار*قطار = get_قطار_object<QTYPE>(thr, net);
      أكد(قطار);
      قطار->push_back(value, max_سائيز);
      أرجع سچ;
}

/*
 * %store/qb/r <var-label>, <max-idx>
 */
منطقي جو_STORE_QB_R(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع store_qb<مزدوج, vvp_قطار_real>(thr, cp);
}

/*
 * %store/qb/str <var-label>, <max-idx>
 */
منطقي جو_STORE_QB_STR(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع store_qb<string, vvp_قطار_string>(thr, cp);
}

/*
 * %store/qb/v <var-label>, <max-idx>, <wid>
 */
منطقي جو_STORE_QB_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع store_qb<vvp_vector4_t, vvp_قطار_vec4>(thr, cp, cp->ٻٽو_انگ_idx[1]);
}

سانچو <قسم_نالو ELEM, صنف QTYPE>
ساكن منطقي store_qdar(vthread_t thr, vvp_ڪوڊ_t cp, بنانشان wid=0)
{
      int64_t idx = thr->لفظ[3].w_int;
      ELEM value;
      vvp_net_t*net = cp->net;
      ڪڍو_قدر(thr, value, wid); // Pop the value to store.

      vvp_قطار*قطار = get_قطار_object<QTYPE>(thr, net);
      أكد(قطار);
      إذا (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot assign to a negative "
	         << get_قطار_type(value)
	         << " index (" << idx << "). ";
	    print_قطار_قدر(value);
	    cerr << " was not added." << سٽ_پڄاڻي;
      } وإلا إذا (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot assign to an undefined "
	         << get_قطار_type(value) << " index. ";
	    print_قطار_قدر(value);
	    cerr << " was not added." << سٽ_پڄاڻي;
      } وإلا {
	    بنانشان max_سائيز = thr->لفظ[cp->ٻٽو_انگ_idx[0]].w_int;
	    قطار->سيٽ_لفظ_max(idx, value, max_سائيز);
      }
      أرجع سچ;
}

/*
 * %store/qdar/r <var>, idx
 */
منطقي جو_STORE_QDAR_R(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع store_qdar<مزدوج, vvp_قطار_real>(thr, cp);
}

/*
 * %store/qdar/str <var>, idx
 */
منطقي جو_STORE_QDAR_STR(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع store_qdar<string, vvp_قطار_string>(thr, cp);
}

/*
 * %store/qdar/v <var>, idx
 */
منطقي جو_STORE_QDAR_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع store_qdar<vvp_vector4_t, vvp_قطار_vec4>(thr, cp, cp->ٻٽو_انگ_idx[1]);
}

سانچو <قسم_نالو ELEM, صنف QTYPE>
ساكن منطقي store_qf(vthread_t thr, vvp_ڪوڊ_t cp, بنانشان wid=0)
{
      ELEM value;
      vvp_net_t*net = cp->net;
      بنانشان max_سائيز = thr->لفظ[cp->ٻٽو_انگ_idx[0]].w_int;
      ڪڍو_قدر(thr, value, wid); // Pop the value to store.

      vvp_قطار*قطار = get_قطار_object<QTYPE>(thr, net);
      أكد(قطار);
      قطار->push_front(value, max_سائيز);
      أرجع سچ;
}
/*
 * %store/qf/r <var-label>, <max-idx>
 */
منطقي جو_STORE_QF_R(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع store_qf<مزدوج, vvp_قطار_real>(thr, cp);
}

/*
 * %store/qf/str <var-label>, <max-idx>
 */
منطقي جو_STORE_QF_STR(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع store_qf<string, vvp_قطار_string>(thr, cp);
}

/*
 * %store/qb/v <var-label>, <max-idx>, <wid>
 */
منطقي جو_STORE_QF_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع store_qf<vvp_vector4_t, vvp_قطار_vec4>(thr, cp, cp->ٻٽو_انگ_idx[1]);
}

سانچو <قسم_نالو ELEM, صنف QTYPE>
ساكن منطقي store_qobj(vthread_t thr, vvp_ڪوڊ_t cp, بنانشان wid=0)
{
// FIXME: Can we actually use wid here?
      (فراغ)wid;
      vvp_net_t*net = cp->net;

      vvp_قطار*قطار = get_قطار_object<QTYPE>(thr, net);
      أكد(قطار);

      vvp_object_t src;
      thr->ڪڍو_object(src);

        // If it is null just clear the queue
      إذا (src.test_nil())
	    قطار->erase_پڇ(0);
      وإلا {
	    بنانشان max_سائيز = thr->لفظ[cp->ٻٽو_انگ_idx[0]].w_int;
	    قطار->copy_elems(src, max_سائيز);
      }

      أرجع سچ;
}

منطقي جو_STORE_QOBJ_R(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع store_qobj<مزدوج, vvp_قطار_real>(thr, cp);
}

منطقي جو_STORE_QOBJ_STR(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع store_qobj<string, vvp_قطار_string>(thr, cp);
}

منطقي جو_STORE_QOBJ_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع store_qobj<vvp_vector4_t, vvp_قطار_vec4>(thr, cp, cp->ٻٽو_انگ_idx[1]);
}

ساكن فراغ vvp_send(vthread_t thr, vvp_net_ptr_t ptr, ثابت مزدوج&val)
{
      vvp_send_real(ptr, val, thr->wt_context);
}

ساكن فراغ vvp_send(vthread_t thr, vvp_net_ptr_t ptr, ثابت string&val)
{
      vvp_send_string(ptr, val, thr->wt_context);
}

سانچو <قسم_نالو ELEM>
ساكن منطقي store(vthread_t thr, vvp_ڪوڊ_t cp)
{
      ELEM val;
      ڪڍو_قدر(thr, val, 0);
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_send(thr, ptr, val);
      أرجع سچ;
}

منطقي جو_STORE_REAL(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع store<مزدوج>(thr, cp);
}

سانچو <قسم_نالو ELEM>
ساكن منطقي storea(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان idx = cp->ٻٽو_انگ_idx[0];
      ELEM val;
      ڪڍو_قدر(thr, val, 0);

      إذا (thr->flags[4] != BIT4_1) {
	    بنانشان adr = thr->لفظ[idx].w_int;
	    cp->array->سيٽ_لفظ(adr, val);
      }

      أرجع سچ;
}

/*
 * %store/reala <var-label> <index>
 */
منطقي جو_STORE_REALA(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع storea<مزدوج>(thr, cp);
}

منطقي جو_STORE_STR(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أرجع store<string>(thr, cp);
}

/*
 * %store/stra <array-label> <index>
 */
منطقي جو_STORE_STRA(vthread_t thr, vvp_ڪوڊ_t cp)
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
منطقي جو_STORE_VEC4(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_net_ptr_t ptr(cp->net, 0);
      ثابت vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*> (cp->net->fil);
      بنانشان off_اشاريو = cp->ٻٽو_انگ_idx[0];
      بنانشان صحيح wid = cp->ٻٽو_انگ_idx[1];

      int64_t off = off_اشاريو ? thr->لفظ[off_اشاريو].w_int : 0;
      بنانشان صحيح sig_قدر_سائيز = sig->قدر_سائيز();

      vvp_vector4_t&val = thr->peek_vec4();
      بنانشان val_سائيز = val.size();

      إذا (val_سائيز < wid) {
	    cerr << thr->get_fileline()
	         << "XXXX Internal error: val.size()=" << val_سائيز
		 << ", expecting >= " << wid << سٽ_پڄاڻي;
      }
      أكد(val_سائيز >= wid);
      إذا (val_سائيز > wid) {
	    val.resize(wid);
      }

	// If there is a problem loading the index register, flags-4
	// will be set to 1, and we know here to skip the actual assignment.
      إذا (off_اشاريو!=0 && thr->flags[4] == BIT4_1) {
	    thr->ڪڍو_vec4(1);
	    أرجع سچ;
      }

      إذا (!resize_rval_vec(val, off, sig_قدر_سائيز)) {
	    thr->ڪڍو_vec4(1);
	    أرجع سچ;
      }

      إذا (off == 0 && val.size() == sig_قدر_سائيز)
	    vvp_send_vec4(ptr, val, thr->wt_context);
      وإلا
	    vvp_send_vec4_pv(ptr, val, off, sig_قدر_سائيز, thr->wt_context);

      thr->ڪڍو_vec4(1);
      أرجع سچ;
}

/*
 * %store/vec4a <var-label>, <addr>, <offset>
 */
منطقي جو_STORE_VEC4_A(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان adr_اشاريو = cp->ٻٽو_انگ_idx[0];
      بنانشان off_اشاريو = cp->ٻٽو_انگ_idx[1];

      ڊگهو adr = adr_اشاريو? thr->لفظ[adr_اشاريو].w_int : 0;
      int64_t off = off_اشاريو ? thr->لفظ[off_اشاريو].w_int : 0;

	// Suppress action if flags-4 is true.
      إذا (thr->flags[4] == BIT4_1) {
	    thr->ڪڍو_vec4(1);
	    أرجع سچ;
      }

      vvp_vector4_t &value = thr->peek_vec4();

      إذا (!resize_rval_vec(value, off, cp->array->get_لفظ_سائيز())) {
	    thr->ڪڍو_vec4(1);
	    أرجع سچ;
      }

      cp->array->سيٽ_لفظ(adr, off, value);

      thr->ڪڍو_vec4(1);
      أرجع سچ;
}

/*
 * %sub
 *   pop r;
 *   pop l;
 *   push l-r;
 */
منطقي جو_SUB(vthread_t thr, vvp_ڪوڊ_t)
{
      vvp_vector4_t r = thr->ڪڍو_vec4();
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
منطقي جو_SUBI(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان wid = cp->عدد;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_فوري_rval (cp, r);

      l.sub(r);

      أرجع سچ;

}

منطقي جو_SUB_WR(vthread_t thr, vvp_ڪوڊ_t)
{
      مزدوج r = thr->ڪڍو_real();
      مزدوج l = thr->ڪڍو_real();
      thr->ڌڪو_real(l - r);
      أرجع سچ;
}

/*
 * %substr <first>, <last>
 * Pop a string, take the substring (SystemVerilog style), and return
 * the result to the stack. This opcode actually works by editing the
 * string in place.
 */
منطقي جو_SUBSTR(vthread_t thr, vvp_ڪوڊ_t cp)
{
      int32_t پهريون = thr->لفظ[cp->ٻٽو_انگ_idx[0]].w_int;
      int32_t آخري = thr->لفظ[cp->ٻٽو_انگ_idx[1]].w_int;
      string&val = thr->peek_str(0);

      إذا (پهريون < 0 || آخري < پهريون || آخري >= (int32_t)val.size()) {
	    val = string("");
	    أرجع سچ;
      }

      val = val.substr(پهريون, آخري-پهريون+1);
      أرجع سچ;
}

/*
 * %substr/vec4 <index>, <wid>
 */
منطقي جو_SUBSTR_VEC4(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان sel_idx = cp->ٻٽو_انگ_idx[0];
      بنانشان wid = cp->ٻٽو_انگ_idx[1];

      int32_t sel = thr->لفظ[sel_idx].w_int;
      ثابت string&val = thr->peek_str(0);

      أكد(wid%8 == 0);

      إذا (sel < 0 || sel >= (int32_t)val.size()) {
	    vvp_vector4_t res (wid, BIT4_0);
	    thr->ڌڪو_vec4(res);
	    أرجع سچ;
      }

      vvp_vector4_t res (wid, BIT4_0);

      أكد(wid==8);
      بنانشان حرف tmp = val[sel];
      لكل (صحيح idx = 0 ; idx < 8 ; idx += 1) {
	    إذا (tmp & (1<<idx))
		  res.سيٽ_ٻٽو_انگ(idx, BIT4_1);
      }

      thr->ڌڪو_vec4(res);
      أرجع سچ;
}

منطقي جو_FILE_سٽ(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vpiHandle handle = cp->handle;

	/* When it is available, keep the file/line information in the
	   thread for error/warning messages. */
      thr->سيٽ_fileline(vpi_get_str(vpiFile, handle),
                        vpi_get(vpi_سٽ_No, handle));

      إذا (show_file_سٽ)
	    cerr << thr->get_fileline()
	         << vpi_get_str(_vpiDescription, handle) << سٽ_پڄاڻي;

      أرجع سچ;
}

/*
 * %test_nul <var-label>;
 * Test if the object at the specified variable is nil. If so, write
 * "1" into flags[4], otherwise write "0" into flags[4].
 */
منطقي جو_TEST_NUL(vthread_t thr, vvp_ڪوڊ_t cp)
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

منطقي جو_TEST_NUL_A(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان idx = cp->ٻٽو_انگ_idx[0];
      بنانشان adr = thr->لفظ[idx].w_int;
      vvp_object_t لفظ_2;

	/* If the address is undefined, return true. */
      إذا (thr->flags[4] == BIT4_1) {
	    أرجع سچ;
      }

      cp->array->get_لفظ_obj(adr, لفظ_2);
      إذا (لفظ_2.test_nil())
	    thr->flags[4] = BIT4_1;
      وإلا
	    thr->flags[4] = BIT4_0;

      أرجع سچ;
}

منطقي جو_TEST_NUL_OBJ(vthread_t thr, vvp_ڪوڊ_t)
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
منطقي جو_TEST_NUL_PROP(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان pid = cp->عدد;
      بنانشان idx = cp->ٻٽو_انگ_idx[0];

      إذا (idx != 0) {
	    أكد(idx < vthread_s::لفظ_ڳڻپ);
	    idx = thr->لفظ[idx].w_uint;
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
سانچو <صنف SRC>
ساكن vvp_vector4_t uparr_and_لفظ_src(SRC* src, بنانشان wid)
{
      إذا (!src) أرجع vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_1);
      لكل (size_t i = 0; i < src->get_سائيز(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_لفظ(i, vi);
	    res &= vi;
      }
      أرجع res;
}

منطقي جو_UPARR_AND_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_net_t* net = cp->net;
      بنانشان wid = cp->ٻٽو_انگ_idx[0];
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      إذا (qsrc) res = uparr_and_لفظ_src(qsrc, wid);
      وإلا إذا (dsrc) res = uparr_and_لفظ_src(dsrc, wid);
      وإلا {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    res = uparr_and_لفظ_src(src, wid);
      }
      thr->ڌڪو_vec4(res);
      أرجع سچ;
}

/* or() reduction returns the scalar sum (Verilog-style bitwise-AND per word). */
سانچو <صنف SRC>
ساكن vvp_vector4_t uparr_or_لفظ_src(SRC* src, بنانشان wid)
{
      إذا (!src) أرجع vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_0);
      لكل (size_t i = 0; i < src->get_سائيز(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_لفظ(i, vi);
	    res |= vi;
      }
      أرجع res;
}

منطقي جو_UPARR_OR_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_net_t* net = cp->net;
      بنانشان wid = cp->ٻٽو_انگ_idx[0];
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      إذا (qsrc) res = uparr_or_لفظ_src(qsrc, wid);
      وإلا إذا (dsrc) res = uparr_or_لفظ_src(dsrc, wid);
      وإلا {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    res = uparr_or_لفظ_src(src, wid);
      }
      thr->ڌڪو_vec4(res);
      أرجع سچ;
}

/* real product() reduction returns the scalar sum */
سانچو <صنف SRC>
ساكن مزدوج uparr_حاصل_ضرب_لفظ_src_real(SRC* src)
{
      إذا (!src) أرجع 0.0;
      مزدوج acc = 1.0;
      لكل (size_t i = 0; i < src->get_سائيز(); i += 1) {
	    مزدوج vi;
	    src->get_لفظ(i, vi);
	    acc *= vi;
      }
      أرجع acc;
}

منطقي جو_UPARR_حاصل_ضرب_REAL(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_قطار_real* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_real_from_net(thr, net, qsrc, dsrc);
      مزدوج جوڙ;
      إذا (qsrc) جوڙ = uparr_حاصل_ضرب_لفظ_src_real(qsrc);
      وإلا إذا (dsrc) جوڙ = uparr_حاصل_ضرب_لفظ_src_real(dsrc);
      وإلا {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_real>(thr, net);
	    vvp_قطار_real* src = dynamic_cast<vvp_قطار_real*>(src_q);
	    جوڙ = uparr_حاصل_ضرب_لفظ_src_real(src);
      }
      thr->ڌڪو_real(جوڙ);
      أرجع سچ;
}

/* product() reduction returns the scalar product (Verilog-style multiplication per word). */
سانچو <صنف SRC>
ساكن vvp_vector4_t uparr_حاصل_ضرب_لفظ_src(SRC* src, بنانشان wid)
{
      إذا (!src) أرجع vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t acc = قطار_unique_ulong_منزل_vec4(1UL, wid);
      لكل (size_t i = 0; i < src->get_سائيز(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_لفظ(i, vi);
	    acc.mul(vi);
      }
      أرجع acc;
}

منطقي جو_UPARR_حاصل_ضرب_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_net_t* net = cp->net;
      بنانشان wid = cp->ٻٽو_انگ_idx[0];
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t prod;
      إذا (qsrc) prod = uparr_حاصل_ضرب_لفظ_src(qsrc, wid);
      وإلا إذا (dsrc) prod = uparr_حاصل_ضرب_لفظ_src(dsrc, wid);
      وإلا {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    prod = uparr_حاصل_ضرب_لفظ_src(src, wid);
      }
      thr->ڌڪو_vec4(prod);
      أرجع سچ;
}

منطقي جو_UPARR_حاصل_ضرب_PROP_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      size_t pid = cp->عدد;
      بنانشان wid = cp->ٻٽو_انگ_idx[0];

      vvp_object_t& چوٽي = thr->peek_object();
      vvp_cobject*cobj = چوٽي.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t prod =
	    qsrc ? uparr_حاصل_ضرب_لفظ_src(qsrc, wid)
		 : uparr_حاصل_ضرب_لفظ_src(dsrc, wid);
      thr->ڌڪو_vec4(prod);
      أرجع سچ;
}

منطقي جو_UPARR_حاصل_ضرب_OBJ_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان wid = cp->ٻٽو_انگ_idx[0];
      vvp_object_t src_obj;
      thr->ڪڍو_object(src_obj);

      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_vector4_t prod =
	    qsrc ? uparr_حاصل_ضرب_لفظ_src(qsrc, wid)
		 : uparr_حاصل_ضرب_لفظ_src(dsrc, wid);
      thr->ڌڪو_vec4(prod);
      أرجع سچ;
}

/* real sum() reduction returns the scalar sum */
سانچو <صنف SRC>
ساكن مزدوج uparr_جوڙ_لفظ_src_real(SRC* src)
{
      مزدوج acc = 0.0;
      إذا (!src) أرجع acc;
      لكل (size_t i = 0; i < src->get_سائيز(); i += 1) {
	    مزدوج vi;
	    src->get_لفظ(i, vi);
	    acc += vi;
      }
      أرجع acc;
}

منطقي جو_UPARR_جوڙ_REAL(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_قطار_real* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_real_from_net(thr, net, qsrc, dsrc);
      مزدوج جوڙ;
      إذا (qsrc) جوڙ = uparr_جوڙ_لفظ_src_real(qsrc);
      وإلا إذا (dsrc) جوڙ = uparr_جوڙ_لفظ_src_real(dsrc);
      وإلا {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_real>(thr, net);
	    vvp_قطار_real* src = dynamic_cast<vvp_قطار_real*>(src_q);
	    جوڙ = uparr_جوڙ_لفظ_src_real(src);
      }
      thr->ڌڪو_real(جوڙ);
      أرجع سچ;
}

/* sum() reduction returns the scalar sum (Verilog-style addition per word). */
سانچو <صنف SRC>
ساكن vvp_vector4_t uparr_جوڙ_لفظ_src(SRC* src, بنانشان wid)
{
      إذا (!src) أرجع vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t acc(wid, BIT4_0);
      لكل (size_t i = 0; i < src->get_سائيز(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_لفظ(i, vi);
	    acc.جوڙيو(vi);
      }
      أرجع acc;
}

منطقي جو_UPARR_جوڙ_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_net_t* net = cp->net;
      بنانشان wid = cp->ٻٽو_انگ_idx[0];
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t جوڙ;
      إذا (qsrc) جوڙ = uparr_جوڙ_لفظ_src(qsrc, wid);
      وإلا إذا (dsrc) جوڙ = uparr_جوڙ_لفظ_src(dsrc, wid);
      وإلا {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    جوڙ = uparr_جوڙ_لفظ_src(src, wid);
      }
      thr->ڌڪو_vec4(جوڙ);
      أرجع سچ;
}

منطقي جو_UPARR_جوڙ_PROP_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      size_t pid = cp->عدد;
      بنانشان wid = cp->ٻٽو_انگ_idx[0];

      vvp_object_t& چوٽي = thr->peek_object();
      vvp_cobject*cobj = چوٽي.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t جوڙ =
	    qsrc ? uparr_جوڙ_لفظ_src(qsrc, wid)
		 : uparr_جوڙ_لفظ_src(dsrc, wid);
      thr->ڌڪو_vec4(جوڙ);
      أرجع سچ;
}

منطقي جو_UPARR_جوڙ_OBJ_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      بنانشان wid = cp->ٻٽو_انگ_idx[0];
      vvp_object_t src_obj;
      thr->ڪڍو_object(src_obj);

      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_vector4_t جوڙ =
	    qsrc ? uparr_جوڙ_لفظ_src(qsrc, wid)
		 : uparr_جوڙ_لفظ_src(dsrc, wid);
      thr->ڌڪو_vec4(جوڙ);
      أرجع سچ;
}

/* xor() reduction returns the scalar sum (Verilog-style bitwise-XOR per word). */
سانچو <صنف SRC>
ساكن vvp_vector4_t uparr_xor_لفظ_src(SRC* src, بنانشان wid)
{
      إذا (!src) أرجع vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_0);
      لكل (size_t i = 0; i < src->get_سائيز(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_لفظ(i, vi);
	    res ^= vi;
      }
      أرجع res;
}

منطقي جو_UPARR_XOR_V(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vvp_net_t* net = cp->net;
      بنانشان wid = cp->ٻٽو_انگ_idx[0];
      vvp_قطار_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_قطار_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      إذا (qsrc) res = uparr_xor_لفظ_src(qsrc, wid);
      وإلا إذا (dsrc) res = uparr_xor_لفظ_src(dsrc, wid);
      وإلا {
	    vvp_قطار* src_q = get_قطار_object<vvp_قطار_vec4>(thr, net);
	    vvp_قطار_vec4* src = dynamic_cast<vvp_قطار_vec4*>(src_q);
	    res = uparr_xor_لفظ_src(src, wid);
      }
      thr->ڌڪو_vec4(res);
      أرجع سچ;
}

منطقي جو_VPI_CALL(vthread_t thr, vvp_ڪوڊ_t cp)
{
      vpip_execute_vpi_call(thr, cp->handle);

      إذا (وقت_جدول_stopped()) {
	    إذا (! وقت_جدول_ختم())
		  وقت_جدول_vthread(thr, 0, خطأ);

	    أرجع خطأ;
      }

      أرجع وقت_جدول_ختم()? خطأ : سچ;
}

/* %wait <label>;
 * Implement the wait by locating the vvp_net_T for the event, and
 * adding this thread to the threads list for the event. The some
 * argument is the  reference to the functor to wait for. This must be
 * an event object of some sort.
 */
منطقي جو_WAIT(vthread_t thr, vvp_ڪوڊ_t cp)
{
      أكد(! thr->i_am_اندريون_function);
      أكد(! thr->انتظار_for_event);
      thr->انتظار_for_event = 1;

	/* Add this thread to the list in the event. */
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (cp->net->fun);
      أكد(ep);
      thr->wait_ايندڙ = ep->جوڙيو_انتظار_thread(thr);

	/* Return false to suspend this thread. */
      أرجع خطأ;
}

/*
 * Implement the %wait/fork (SystemVerilog) instruction by suspending
 * the current thread until all the detached children have finished.
 */
منطقي جو_WAIT_FORK(vthread_t thr, vvp_ڪوڊ_t)
{
	/* If a %wait/fork is being executed then the parent thread
	 * cannot be waiting in a join or already waiting. */
      أكد(! thr->i_am_اندريون_function);
      أكد(! thr->i_am_joining);
      أكد(! thr->i_am_انتظار);

	/* There should be no active children when waiting. */
      أكد(thr->children.empty());

	/* If there are no detached children then there is nothing to
	 * wait for. */
      إذا (thr->detached_children.empty()) أرجع سچ;

	/* Flag that this process is waiting for the detached children
	 * to finish and suspend it. */
      thr->i_am_انتظار = 1;
      أرجع خطأ;
}

/*
 * %xnor
 */
منطقي جو_XNOR(vthread_t thr, vvp_ڪوڊ_t)
{
      vvp_vector4_t valr = thr->ڪڍو_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      أكد(vall.size() == valr.size());

      vall ^= valr;
      vall.invert();

      أرجع سچ;
}

/*
 * %xor
 */
منطقي جو_XOR(vthread_t thr, vvp_ڪوڊ_t)
{
      vvp_vector4_t valr = thr->ڪڍو_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      أكد(vall.size() == valr.size());

      vall ^= valr;

      أرجع سچ;
}


منطقي جو_ZOMBIE(vthread_t thr, vvp_ڪوڊ_t)
{
      thr->pc = codespace_null();
      إذا ((thr->والد == 0) && (thr->children.empty())) {
	    إذا (thr->delay_delete)
		  وقت_جدول_del_thr(thr);
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
ساكن منطقي do_exec_ufunc(vthread_t thr, vvp_ڪوڊ_t cp, vthread_t ٻار)
{
      __vpiScope*ٻار_scope = cp->ufunc_core_ptr->func_scope();
      أكد(ٻار_scope);

      أكد(ٻار_scope->get_type_ڪوڊ() == vpiFunction);
      أكد(thr->children.empty());


        /* We can take a number of shortcuts because we know that a
           continuous assignment can only occur in a static scope. */
      أكد(thr->wt_context == 0);
      أكد(thr->rd_context == 0);

        /* If an automatic function, allocate a context for this call. */
      vvp_context_t ٻار_context = 0;
      إذا (ٻار_scope->is_automatic()) {
            ٻار_context = vthread_alloc_context(ٻار_scope);
            thr->wt_context = ٻار_context;
            thr->rd_context = ٻار_context;
      }

      ٻار->wt_context = ٻار_context;
      ٻار->rd_context = ٻار_context;

	/* Copy all the inputs to the ufunc object to the port
	   variables of the function. This copies all the values
	   atomically. */
      cp->ufunc_core_ptr->سونپيو_بٽ_منزل_ports(ٻار_context);
      ٻار->delay_delete = 1;

      ٻار->والد = thr;
      thr->children.insert(ٻار);
	// This should be the only child
      أكد(thr->children.size()==1);

      ٻار->is_scheduled = 1;
      ٻار->i_am_اندريون_function = 1;
      vthread_لاڳيتو(ٻار);
      running_thread = thr;

      إذا (ٻار->i_have_ended) {
	    do_join(thr, ٻار);
            أرجع سچ;
      } وإلا {
	    thr->i_am_joining = 1;
	    أرجع خطأ;
      }
}

منطقي جو_EXEC_UFUNC_REAL(vthread_t thr, vvp_ڪوڊ_t cp)
{
      __vpiScope*ٻار_scope = cp->ufunc_core_ptr->func_scope();
      أكد(ٻار_scope);

	/* Create a temporary thread and run it immediately. */
      vthread_t ٻار = vthread_new(cp->cptr, ٻار_scope);
      thr->ڌڪو_real(0.0);
      ٻار->دليلون_real.push_back(0);

      أرجع do_exec_ufunc(thr, cp, ٻار);
}

منطقي جو_EXEC_UFUNC_VEC4(vthread_t thr, vvp_ڪوڊ_t cp)
{
      __vpiScope*ٻار_scope = cp->ufunc_core_ptr->func_scope();
      أكد(ٻار_scope);

      vpiScopeFunction*scope_func = dynamic_cast<vpiScopeFunction*>(ٻار_scope);
      أكد(scope_func);

	/* Create a temporary thread and run it immediately. */
      vthread_t ٻار = vthread_new(cp->cptr, ٻار_scope);
      thr->ڌڪو_vec4(vvp_vector4_t(scope_func->get_func_ويڪر(), scope_func->get_func_init_val()));
      ٻار->دليلون_vec4.push_back(0);

      أرجع do_exec_ufunc(thr, cp, ٻار);
}

/*
 * This is a phantom opcode used to harvest the result of calling a user
 * defined function. It is used in code generated by the .ufunc statement.
 */
منطقي جو_REAP_UFUNC(vthread_t thr, vvp_ڪوڊ_t cp)
{
      __vpiScope*ٻار_scope = cp->ufunc_core_ptr->func_scope();
      أكد(ٻار_scope);

	/* Copy the output from the result variable to the output
	   ports of the .ufunc device. */
      cp->ufunc_core_ptr->پڄاڻي_thread();

        /* If an automatic function, free the context for this call. */
      إذا (ٻار_scope->is_automatic()) {
            vthread_free_context(thr->rd_context, ٻار_scope);
            thr->wt_context = 0;
            thr->rd_context = 0;
      }

      أرجع سچ;
}
