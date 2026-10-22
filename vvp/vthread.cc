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
#ifdef CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#endif
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

using نطاق std;

/* This is the size of an unsigned long in bits. This is just a
   convenience macro. */
# define CPU_WORD_BITS (8*حجم(unsigned long))
# define چوٹی_BIT (1UL << (CPU_WORD_BITS-1))

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

      فراغ debug_dump(ostream&fd, ثابت حرف*label_text);

	/* This is the program counter. */
      vvp_code_t pc;
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
      vector<unsigned> args_real;
      vector<unsigned> args_str;
      vector<unsigned> args_vec4;

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
      مضمن ثابت vvp_vector4_t& peek_vec4(unsigned depth)
      {
	    unsigned size = ڈھیر_vec4.size();
	    أكد(depth < size);
	    unsigned use_index = size-1-depth;
	    أرجع ڈھیر_vec4[use_index];
      }
      مضمن vvp_vector4_t& peek_vec4(فراغ)
      {
	    unsigned use_index = ڈھیر_vec4.size();
	    أكد(use_index >= 1);
	    أرجع ڈھیر_vec4[use_index-1];
      }
      مضمن فراغ poke_vec4(unsigned depth, ثابت vvp_vector4_t&val)
      {
	    أكد(depth < ڈھیر_vec4.size());
	    unsigned use_index = ڈھیر_vec4.size()-1-depth;
	    ڈھیر_vec4[use_index] = val;
      }
      مضمن فراغ نکالو_vec4(unsigned cnt)
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
      مضمن مزدوج peek_real(unsigned depth)
      {
	    أكد(depth < ڈھیر_real.size());
	    unsigned use_index = ڈھیر_real.size()-1-depth;
	    أرجع ڈھیر_real[use_index];
      }
      مضمن فراغ poke_real(unsigned depth, مزدوج val)
      {
	    أكد(depth < ڈھیر_real.size());
	    unsigned use_index = ڈھیر_real.size()-1-depth;
	    ڈھیر_real[use_index] = val;
      }
      مضمن فراغ نکالو_real(unsigned cnt)
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
      مضمن string&peek_str(unsigned depth)
      {
	    أكد(depth<ڈھیر_str.size());
	    unsigned use_index = ڈھیر_str.size()-1-depth;
	    أرجع ڈھیر_str[use_index];
      }
      مضمن فراغ poke_str(unsigned depth, ثابت string&val)
      {
	    أكد(depth < ڈھیر_str.size());
	    unsigned use_index = ڈھیر_str.size()-1-depth;
	    ڈھیر_str[use_index] = val;
      }
      مضمن فراغ نکالو_str(unsigned cnt)
      {
	    طالما (cnt > 0) {
		  ڈھیر_str.pop_back();
		  cnt -= 1;
	    }
      }

	/* Objects are also operated on in a stack. */
    خاص:
      تعداد { ڈھیر_OBJ_MAX_SIZE = 32 };
      vvp_object_t ڈھیر_obj[ڈھیر_OBJ_MAX_SIZE];
      unsigned ڈھیر_obj_size;
    عام:
      مضمن vvp_object_t& peek_object(فراغ)
      {
	    أكد(ڈھیر_obj_size > 0);
	    أرجع ڈھیر_obj[ڈھیر_obj_size-1];
      }
      مضمن فراغ نکالو_object(vvp_object_t&obj)
      {
	    أكد(ڈھیر_obj_size > 0);
	    ڈھیر_obj_size -= 1;
	    obj = ڈھیر_obj[ڈھیر_obj_size];
	    ڈھیر_obj[ڈھیر_obj_size].reset(0);
      }
      مضمن فراغ نکالو_object(unsigned cnt, unsigned skip =0)
      {
	    أكد((cnt+skip) <= ڈھیر_obj_size);
	    لكل (size_t idx = ڈھیر_obj_size-skip-cnt ; idx < ڈھیر_obj_size-skip ; idx += 1)
		  ڈھیر_obj[idx].reset(0);
	    ڈھیر_obj_size -= cnt;
	    لكل (size_t idx = ڈھیر_obj_size-skip ; idx < ڈھیر_obj_size ; idx += 1)
		  ڈھیر_obj[idx] = ڈھیر_obj[idx+skip];
	    لكل (size_t idx = ڈھیر_obj_size ; idx < ڈھیر_obj_size+skip ; idx += 1)
		  ڈھیر_obj[idx].reset(0);
      }
      مضمن فراغ ڈالو_object(ثابت vvp_object_t&obj)
      {
	    أكد(ڈھیر_obj_size < ڈھیر_OBJ_MAX_SIZE);
	    ڈھیر_obj[ڈھیر_obj_size] = obj;
	    ڈھیر_obj_size += 1;
      }

	/* My parent sets this when it wants me to wake it up. */
      unsigned i_am_joining      :1;
      unsigned i_am_detached     :1;
      unsigned i_am_waiting      :1;
      unsigned i_am_in_function  :1; // True if running function code
      unsigned i_have_ended      :1;
      unsigned i_was_disabled    :1;
      unsigned waiting_for_event :1;
      unsigned is_scheduled      :1;
      unsigned delay_delete      :1;
	/* This points to the children of the thread. */
      set<بنية vthread_s*>children;
	/* This points to the detached children of the thread. */
      set<بنية vthread_s*>detached_children;
	/* This points to my parent, if I have one. */
      بنية vthread_s*parent;
	/* This points to the containing scope. */
      __vpiScope*parent_scope;
	/* This is used for keeping wait queues. */
      بنية vthread_s*wait_next;
	/* These are used to access automatically allocated items. */
      vvp_context_t wt_context, rd_context;
	/* These are used to pass non-blocking event control information. */
      vvp_net_t*event;
      uint64_t ecount;
	/* Save the file/line information when available. */
    خاص:
      حرف *filenm_;
      unsigned lineno_;
    عام:
      فراغ set_fileline(حرف *filenm, unsigned lineno);
      string get_fileline();

      مضمن فراغ cleanup()
      {
	    إذا (i_was_disabled) {
		  ڈھیر_vec4.clear();
		  ڈھیر_real.clear();
		  ڈھیر_str.clear();
		  نکالو_object(ڈھیر_obj_size);
	    }
	    حرر(filenm_);
	    filenm_ = 0;
	    أكد(ڈھیر_vec4.empty());
	    أكد(ڈھیر_real.empty());
	    أكد(ڈھیر_str.empty());
	    أكد(ڈھیر_obj_size == 0);
      }
};

مضمن vthread_s::vthread_s()
{
      ڈھیر_obj_size = 0;
      filenm_ = 0;
      lineno_ = 0;
}

فراغ vthread_s::set_fileline(حرف *filenm, unsigned lineno)
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
      fd << "**** " << label << endl;
      fd << "**** ThreadId: " << هذا << ", parent id: " << parent << endl;

      fd << "**** Flags: ";
      لكل (صحيح idx = 0 ; idx < FLAGS_گنتی ; idx += 1)
	    fd << flags[idx];
      fd << endl;
      fd << "**** vec4 stack..." << endl;
      لكل (size_t idx = ڈھیر_vec4.size() ; idx > 0 ; idx -= 1)
	    fd << "    " << (ڈھیر_vec4.size()-idx) << ": " << ڈھیر_vec4[idx-1] << endl;
      fd << "**** str stack (" << ڈھیر_str.size() << ")..." << endl;
      fd << "**** obj stack (" << ڈھیر_obj_size << ")..." << endl;
      fd << "**** args_vec4 array (" << args_vec4.size() << ")..." << endl;
      لكل (size_t idx = 0 ; idx < args_vec4.size() ; idx += 1)
	    fd << "    " << idx << ": " << args_vec4[idx] << endl;
      fd << "**** file/line (";
      إذا (filenm_) fd << filenm_;
      وإلا fd << "<no file name>";
      fd << ":" << lineno_ << ")" << endl;
      fd << "**** Done ****" << endl;
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
ساكن string filter_string(ثابت حرف*text)
{
      vector<حرف> tmp (strlen(text)+1);
      size_t dst = 0;
      لكل (ثابت حرف*ptr = text ; *ptr ; ptr += 1) {
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

ساكن فراغ do_join(vthread_t thr, vthread_t child);

__vpiScope* vthread_scope(بنية vthread_s*thr)
{
      أرجع thr->parent_scope;
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

فراغ vthread_نکالو_real(بنية vthread_s*thr, unsigned depth)
{
      thr->نکالو_real(depth);
}

فراغ vthread_نکالو_str(بنية vthread_s*thr, unsigned depth)
{
      thr->نکالو_str(depth);
}

فراغ vthread_نکالو_vec4(بنية vthread_s*thr, unsigned depth)
{
      thr->نکالو_vec4(depth);
}

مزدوج vthread_get_real_ڈھیر(بنية vthread_s*thr, unsigned depth)
{
      أرجع thr->peek_real(depth);
}

ثابت string&vthread_get_str_ڈھیر(بنية vthread_s*thr, unsigned depth)
{
      أرجع thr->peek_str(depth);
}

ثابت vvp_vector4_t& vthread_get_vec4_ڈھیر(بنية vthread_s*thr, unsigned depth)
{
      أرجع thr->peek_vec4(depth);
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
template <صنف VVP_QUEUE> ساكن vvp_queue*get_queue_object(vthread_t thr, vvp_net_t*net)
{
      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      أكد(obj);

      vvp_queue*queue = obj->get_object().peek<vvp_queue>();
      إذا (queue == 0) {
	    أكد(obj->get_object().test_nil());
	    queue = جديد VVP_QUEUE;
	    vvp_object_t val (queue);
	    vvp_net_ptr_t ptr (net, 0);
	    vvp_send_object(ptr, val, thr->wt_context);
      }

      أرجع queue;
}

/*
 * Locator and size/word opcodes use the same VVP net for a queue ($) or a
 * dynamic array ([]). Queues are vvp_queue_vec4; dynamic arrays may be
 * vvp_darray_vec4, vvp_darray_atom<>, etc. (all vvp_darray, but vvp_queue
 * also derives from vvp_darray — test queue vec4 first).
 */
ساكن فراغ get_queue_or_darray_vec4_from_net(vthread_t thr, vvp_net_t* net,
					      vvp_queue_vec4*& qsrc,
					      vvp_darray*& dsrc)
{
      vvp_fun_signal_object* obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      أكد(obj);
      vvp_object_t چوٹی = obj->get_object();
      qsrc = چوٹی.peek<vvp_queue_vec4>();
      إذا (qsrc) {
	    dsrc = 0;
	    أرجع;
      }
      vvp_darray* dany = چوٹی.peek<vvp_darray>();
      إذا (dany && dynamic_cast<vvp_queue*>(dany) == 0) {
	    dsrc = dany;
	    أرجع;
      }
      إذا (چوٹی.test_nil()) {
	    qsrc = جديد vvp_queue_vec4();
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

ساكن فراغ get_queue_or_darray_real_from_net(vthread_t thr, vvp_net_t* net,
					      vvp_queue_real*& qsrc,
					      vvp_darray*& dsrc)
{
      vvp_fun_signal_object* obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      أكد(obj);
      vvp_object_t چوٹی = obj->get_object();
      qsrc = چوٹی.peek<vvp_queue_real>();
      إذا (qsrc) {
	    dsrc = 0;
	    أرجع;
      }
      vvp_darray* dany = چوٹی.peek<vvp_darray>();
      إذا (dany && dynamic_cast<vvp_queue*>(dany) == 0) {
	    dsrc = dany;
	    أرجع;
      }
      إذا (چوٹی.test_nil()) {
	    qsrc = جديد vvp_queue_real();
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

ساكن فراغ get_queue_or_darray_vec4_from_object(vvp_object_t src_obj,
						 vvp_queue_vec4*& qsrc,
						 vvp_darray*& dsrc)
{
      qsrc = src_obj.peek<vvp_queue_vec4>();
      إذا (qsrc) {
	    dsrc = 0;
	    أرجع;
      }
      vvp_darray* dany = src_obj.peek<vvp_darray>();
      إذا (dany && dynamic_cast<vvp_queue*>(dany) == 0) {
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
مضمن ساكن فراغ نکالو_قدر(vthread_t thr, مزدوج&value, unsigned)
{
      value = thr->نکالو_real();
}

مضمن ساكن فراغ نکالو_قدر(vthread_t thr, string&value, unsigned)
{
      value = thr->نکالو_str();
}

مضمن ساكن فراغ نکالو_قدر(vthread_t thr, vvp_vector4_t&value, unsigned wid)
{
      value = thr->نکالو_vec4();
      أكد(value.size() == wid);
}

/*
 * The following are used to allow the queue templates to print correctly.
 */
مضمن ساكن string get_queue_type(مزدوج&)
{
      أرجع "queue<real>";
}

مضمن ساكن string get_queue_type(string&)
{
      أرجع "queue<string>";
}

مضمن ساكن string get_queue_type(ثابت vvp_vector4_t&value)
{
      ostringstream buf;
      buf << "queue<vector[" << value.size() << "]>";
      string res = buf.str();
      أرجع res;
}

مضمن ساكن فراغ print_queue_قدر(مزدوج value)
{
      cerr << value;
}

مضمن ساكن فراغ print_queue_قدر(ثابت string&value)
{
      cerr << "\"" << value << "\"";
}

مضمن ساكن فراغ print_queue_قدر(ثابت vvp_vector4_t&value)
{
      cerr << value;
}

/*
 * The following are used to get a darray/queue default value.
 */
مضمن ساكن فراغ dq_default(مزدوج&value, unsigned)
{
      value = 0.0;
}

مضمن ساكن فراغ dq_default(string&value, unsigned)
{
      value = "";
}

مضمن ساكن فراغ dq_default(vvp_vector4_t&value, unsigned wid)
{
      value = vvp_vector4_t(wid);
}


template <صنف T> T coerce_to_width(ثابت T&that, unsigned width)
{
      إذا (that.size() == width)
	    أرجع that;

      أكد(that.size() > width);
      T res (width);
      لكل (unsigned idx = 0 ;  idx < width ;  idx += 1)
	    res.set_bit(idx, that.value(idx));

      أرجع res;
}

/* Explicitly define the vvp_vector4_t version of coerce_to_width(). */
template vvp_vector4_t coerce_to_width(ثابت vvp_vector4_t&that,
                                       unsigned width);


ساكن فراغ multiply_array_imm(unsigned long*res, ثابت unsigned long*val,
			       unsigned الفاظ, unsigned long imm)
{
      لكل (unsigned idx = 0 ; idx < الفاظ ; idx += 1)
	    res[idx] = 0;

      لكل (unsigned mul_idx = 0 ; mul_idx < الفاظ ; mul_idx += 1) {
	    unsigned long sum;
	    unsigned long tmp = multiply_with_carry(val[mul_idx], imm, sum);

	    unsigned long carry = 0;
	    res[mul_idx] = add_with_carry(res[mul_idx], tmp, carry);
	    لكل (unsigned add_idx = mul_idx+1 ; add_idx < الفاظ ; add_idx += 1) {
		  res[add_idx] = add_with_carry(res[add_idx], sum, carry);
		  sum = 0;
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
            scope->free_contexts = vvp_get_next_context(context);
            لكل (unsigned idx = 0 ; idx < scope->nitem ; idx += 1) {
                  scope->item[idx]->reset_instance(context);
            }
      } وإلا {
            context = vvp_allocate_context(scope->nitem);
            لكل (unsigned idx = 0 ; idx < scope->nitem ; idx += 1) {
                  scope->item[idx]->alloc_instance(context);
            }
      }

      vvp_set_next_context(context, scope->live_contexts);
      scope->live_contexts = context;

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

      إذا (context == scope->live_contexts) {
            scope->live_contexts = vvp_get_next_context(context);
      } وإلا {
            vvp_context_t tmp = scope->live_contexts;
            طالما (context != vvp_get_next_context(tmp)) {
                  أكد(tmp);
                  tmp = vvp_get_next_context(tmp);
            }
            vvp_set_next_context(tmp, vvp_get_next_context(context));
      }

      vvp_set_next_context(context, scope->free_contexts);
      scope->free_contexts = context;
}

#ifdef CHECK_WITH_VALGRIND
فراغ contexts_delete(صنف __vpiScope*scope)
{
      vvp_context_t context = scope->free_contexts;

      طالما (context) {
	    scope->free_contexts = vvp_get_next_context(context);
	    لكل (unsigned idx = 0; idx < scope->nitem; idx += 1) {
		  scope->item[idx]->free_instance(context);
	    }
	    حرر(context);
	    context = scope->free_contexts;
      }
      حرر(scope->item);
}
#endif

/*
 * Create a new thread with the given start address.
 */
vthread_t vthread_new(vvp_code_t pc, __vpiScope*scope)
{
      vthread_t thr = جديد بنية vthread_s;
      thr->pc     = pc;
	//thr->bits4  = vvp_vector4_t(32);
      thr->parent = 0;
      thr->parent_scope = scope;
      thr->wait_next = 0;
      thr->wt_context = 0;
      thr->rd_context = 0;

      thr->i_am_joining  = 0;
      thr->i_am_detached = 0;
      thr->i_am_waiting  = 0;
      thr->i_am_in_function = 0;
      thr->is_scheduled  = 0;
      thr->i_have_ended  = 0;
      thr->i_was_disabled = 0;
      thr->delay_delete  = 0;
      thr->waiting_for_event = 0;
      thr->event  = 0;
      thr->ecount = 0;

      thr->flags[0] = BIT4_0;
      thr->flags[1] = BIT4_1;
      thr->flags[2] = BIT4_X;
      thr->flags[3] = BIT4_Z;
      لكل (صحيح idx = 4 ; idx < 8 ; idx += 1)
	    thr->flags[idx] = BIT4_X;

      scope->threads .insert(thr);
      أرجع thr;
}

#ifdef CHECK_WITH_VALGRIND
#إذا 0
/*
 * These are not currently correct. If you use them you will get
 * double delete messages. There is still a leak related to a
 * waiting event that needs to be investigated.
 */

ساكن فراغ wait_next_delete(vthread_t base)
{
      طالما (base) {
	    vthread_t tmp = base->wait_next;
	    delete base;
	    base = tmp;
	    إذا (base->waiting_for_event == 0) اكسر;
      }
}

ساكن فراغ child_delete(vthread_t base)
{
      طالما (base) {
	    vthread_t tmp = base->child;
	    delete base;
	    base = tmp;
      }
}
#endif

فراغ vthreads_delete(صنف __vpiScope*scope)
{
      لكل (std::set<vthread_t>::iterator cur = scope->threads.begin()
		 ; cur != scope->threads.end() ; ++ cur ) {
	    delete *cur;
      }
      scope->threads.clear();
}
#endif

/*
 * Reaping pulls the thread out of the stack of threads. If I have a
 * child, then hand it over to my parent or fully detach it.
 */
ساكن فراغ vthread_reap(vthread_t thr)
{
      إذا (! thr->children.empty()) {
	    لكل (set<vthread_t>::iterator cur = thr->children.begin()
		       ; cur != thr->children.end() ; ++cur) {
		  vthread_t child = *cur;
		  أكد(child);
		  أكد(child->parent == thr);
		  child->parent = thr->parent;
	    }
      }
      إذا (! thr->detached_children.empty()) {
	    لكل (set<vthread_t>::iterator cur = thr->detached_children.begin()
		       ; cur != thr->detached_children.end() ; ++cur) {
		  vthread_t child = *cur;
		  أكد(child);
		  أكد(child->parent == thr);
		  أكد(child->i_am_detached);
		  child->parent = 0;
		  child->i_am_detached = 0;
	    }
      }
      إذا (thr->parent) {
	      /* assert that the given element was removed. */
	    إذا (thr->i_am_detached) {
		  size_t res = thr->parent->detached_children.erase(thr);
		  أكد(res == 1);
	    } وإلا {
		  size_t res = thr->parent->children.erase(thr);
		  أكد(res == 1);
	    }
      }

      thr->parent = 0;

	// Remove myself from the containing scope if needed.
      thr->parent_scope->threads.erase(thr);

      thr->pc = codespace_null();

	/* If this thread is not scheduled, then is it safe to delete
	   it now. Otherwise, let the schedule event (which will
	   execute the thread at of_ZOMBIE) delete the object. */
      إذا ((thr->is_scheduled == 0) && (thr->waiting_for_event == 0)) {
	    أكد(thr->children.empty());
	    أكد(thr->wait_next == 0);
	    إذا (thr->delay_delete)
		  schedule_del_thr(thr);
	    وإلا
		  vthread_delete(thr);
      }
}

فراغ vthread_delete(vthread_t thr)
{
      thr->cleanup();
      delete thr;
}

فراغ vthread_mark_scheduled(vthread_t thr)
{
      طالما (thr != 0) {
	    أكد(thr->is_scheduled == 0);
	    thr->is_scheduled = 1;
	    thr = thr->wait_next;
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
	    thr->i_am_in_function = 1;
	    thr = thr->wait_next;
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
فراغ vthread_run(vthread_t thr)
{
      طالما (thr != 0) {
	    vthread_t tmp = thr->wait_next;
	    thr->wait_next = 0;

	    أكد(thr->is_scheduled);
	    thr->is_scheduled = 0;

            running_thread = thr;

	    لكل (;;) {
		  vvp_code_t cp = thr->pc;
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
منطقي of_CHUNK_LINK(vthread_t thr, vvp_code_t code)
{
      أكد(code->cptr);
      thr->pc = code->cptr;
      أرجع true;
}

/*
 * This is called by an event functor to wake up all the threads on
 * its list. I in fact created that list in the %wait instruction, and
 * I also am certain that the waiting_for_event flag is set.
 */
فراغ vthread_schedule_list(vthread_t thr)
{
      لكل (vthread_t cur = thr ;  cur ;  cur = cur->wait_next) {
	    أكد(cur->waiting_for_event);
	    cur->waiting_for_event = 0;
      }

      schedule_vthread(thr, 0);
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

vvp_context_item_t vthread_get_wt_context_item(unsigned context_idx)
{
      أكد(running_thread && running_thread->wt_context);
      أرجع vvp_get_context_item(running_thread->wt_context, context_idx);
}

vvp_context_item_t vthread_get_rd_context_item(unsigned context_idx)
{
      أكد(running_thread && running_thread->rd_context);
      أرجع vvp_get_context_item(running_thread->rd_context, context_idx);
}

/*
 * %abs/wr
 */
منطقي of_ABS_WR(vthread_t thr, vvp_code_t)
{
      thr->ڈالو_real( fabs(thr->نکالو_real()) );
      أرجع true;
}

منطقي of_ALLOC(vthread_t thr, vvp_code_t cp)
{
        /* Allocate a context. */
      vvp_context_t child_context = vthread_alloc_context(cp->scope);

        /* Push the allocated context onto the write context stack. */
      vvp_set_stacked_context(child_context, thr->wt_context);
      thr->wt_context = child_context;

      أرجع true;
}

منطقي of_AND(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valb = thr->نکالو_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();
      أكد(vala.size() == valb.size());
      vala &= valb;
      أرجع true;
}

/*
 * This function must ALWAYS be called with the val set to the right
 * size, and initialized with BIT4_0 bits. Certain optimizations rely
 * on that.
 */
ساكن فراغ get_immediate_rval(vvp_code_t cp, vvp_vector4_t&val)
{
      uint32_t vala = cp->bit_idx[0];
      uint32_t valb = cp->bit_idx[1];
      unsigned wid  = cp->نمبر;

      إذا (valb == 0) {
	      // Special case: if the value is zero, we are done
	      // before we start.
	    إذا (vala == 0) أرجع;

	      // Special case: The value has no X/Z bits, so we can
	      // use the setarray method to write the value all at once.
	    unsigned use_wid = 8*حجم(unsigned long);
	    إذا (wid < use_wid)
		  use_wid = wid;
	    unsigned long tmp[1];
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

      لكل (unsigned idx = 0 ; idx < wid && (vala|valb) ; idx += 1) {
	    uint32_t ba = 0;
	      // Convert the vala/valb bits to a ba number that
	      // matches the encoding of the vvp_bit4_t enumeration.
	    ba = (valb & 1) << 1;
	    ba |= vala & 1;

	      // Note that the val is already pre-filled with BIT4_0
	      // bits, os we only need to set non-zero bit values.
	    إذا (ba) val.set_bit(idx, (vvp_bit4_t)ba);

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
منطقي of_ADD(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t r = thr->نکالو_vec4();
	// Rather then pop l, use it directly from the stack. When we
	// assign to 'l', that will edit the top of the stack, which
	// replaces a pop and a pull.
      vvp_vector4_t&l = thr->peek_vec4();

      l.add(r);

      أرجع true;
}

/*
 * %addi <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
منطقي of_ADDI(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->نمبر;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_immediate_rval (cp, r);

      l.add(r);

      أرجع true;
}

/*
 * %add/wr
 */
منطقي of_ADD_WR(vthread_t thr, vvp_code_t)
{
      مزدوج r = thr->نکالو_real();
      مزدوج l = thr->نکالو_real();
      thr->ڈالو_real(l + r);
      أرجع true;
}

/* %assign/ar <array>, <delay>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory. <delay>
 * is the delay in simulation time. <bit> is the index register
 * containing the real value.
 */
منطقي of_ASSIGN_AR(vthread_t thr, vvp_code_t cp)
{
      long adr = thr->الفاظ[3].w_int;
      unsigned delay = cp->bit_idx[0];
      مزدوج value = thr->نکالو_real();

      إذا (adr >= 0) {
	    schedule_assign_array_word(cp->array, adr, value, delay);
      }

      أرجع true;
}

/* %assign/ar/d <array>, <delay_idx>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory.
 * <delay_idx> is the integer register that contains the delay value.
 */
منطقي of_ASSIGN_ARD(vthread_t thr, vvp_code_t cp)
{
      long adr = thr->الفاظ[3].w_int;
      مزدوج value = thr->نکالو_real();

      إذا (adr >= 0) {
	    vvp_time64_t delay = thr->الفاظ[cp->bit_idx[0]].w_uint;
	    schedule_assign_array_word(cp->array, adr, value, delay);
      }

      أرجع true;
}

/* %assign/ar/e <array>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory. <bit>
 * is the index register containing the real value. The event
 * information is contained in the thread event control registers
 * and is set with %evctl.
 */
منطقي of_ASSIGN_ARE(vthread_t thr, vvp_code_t cp)
{
      long adr = thr->الفاظ[3].w_int;
      مزدوج value = thr->نکالو_real();

      إذا (adr >= 0) {
	    إذا (thr->ecount == 0) {
		  schedule_assign_array_word(cp->array, adr, value, 0);
	    } وإلا {
		  schedule_evctl(cp->array, adr, value, thr->event,
		                 thr->ecount);
	    }
      }

      أرجع true;
}

/*
 * %assign/vec4 <var>, <delay>
 */
منطقي of_ASSIGN_VEC4(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      unsigned delay = cp->bit_idx[0];
      ثابت vvp_vector4_t&val = thr->peek_vec4();

      schedule_assign_vector(ptr, 0, 0, val, delay);
      thr->نکالو_vec4(1);
      أرجع true;
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
			    unsigned صحيح sig_wid)
{
      unsigned صحيح wid = val.size();

        // Fully in bounds, most likely case
      إذا (off >= 0 && (uint64_t)off + wid <= sig_wid)
	    أرجع true;

      unsigned صحيح base = 0;
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
	    base = -off;
	    wid += off;
		off = 0;
      }

	// If the value is partly above the target, then only assign
	// the bits that overlap
      إذا ((uint64_t)off + wid > sig_wid)
	    wid = sig_wid - (uint64_t)off;

      val = val.subvalue(base, wid);

      أرجع true;
}

/*
 * %assign/vec4/a/d <arr>, <offx>, <delx>
 */
منطقي of_ASSIGN_VEC4_A_D(vthread_t thr, vvp_code_t cp)
{
      صحيح off_idx = cp->bit_idx[0];
      صحيح del_idx = cp->bit_idx[1];
      صحيح adr_idx = 3;

      int64_t  off = off_idx ? thr->الفاظ[off_idx].w_int : 0;
      vvp_time64_t del = del_idx? thr->الفاظ[del_idx].w_uint : 0;
      long     adr = thr->الفاظ[adr_idx].w_int;

      vvp_vector4_t val = thr->نکالو_vec4();

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      إذا (thr->flags[4] == BIT4_1)
	    أرجع true;

      إذا (!resize_rval_vec(val, off, cp->array->get_word_size()))
	    أرجع true;

      schedule_assign_array_word(cp->array, adr, off, val, del);

      أرجع true;
}

/*
 * %assign/vec4/a/e <arr>, <offx>
 */
منطقي of_ASSIGN_VEC4_A_E(vthread_t thr, vvp_code_t cp)
{
      صحيح off_idx = cp->bit_idx[0];
      صحيح adr_idx = 3;

      int64_t  off = off_idx ? thr->الفاظ[off_idx].w_int : 0;
      long     adr = thr->الفاظ[adr_idx].w_int;

      vvp_vector4_t val = thr->نکالو_vec4();

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      إذا (thr->flags[4] == BIT4_1)
	    أرجع true;

      إذا (!resize_rval_vec(val, off, cp->array->get_word_size()))
	    أرجع true;

      إذا (thr->ecount == 0) {
	    schedule_assign_array_word(cp->array, adr, off, val, 0);
      } وإلا {
	    schedule_evctl(cp->array, adr, val, off, thr->event, thr->ecount);
      }

      أرجع true;
}

/*
 * %assign/vec4/off/d <var>, <off>, <del>
 */
منطقي of_ASSIGN_VEC4_OFF_D(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      unsigned off_index = cp->bit_idx[0];
      unsigned del_index = cp->bit_idx[1];
      vvp_vector4_t val = thr->نکالو_vec4();

      int64_t off = thr->الفاظ[off_index].w_int;
      vvp_time64_t del = thr->الفاظ[del_index].w_uint;

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      إذا (thr->flags[4] == BIT4_1)
	    أرجع true;

      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*> (cp->net->fil);
      أكد(sig);

      إذا (!resize_rval_vec(val, off, sig->قدر_size()))
	    أرجع true;

      schedule_assign_vector(ptr, off, sig->قدر_size(), val, del);
      أرجع true;
}

/*
 * %assign/vec4/off/e <var>, <off>
 */
منطقي of_ASSIGN_VEC4_OFF_E(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      unsigned off_index = cp->bit_idx[0];
      vvp_vector4_t val = thr->نکالو_vec4();

      int64_t off = thr->الفاظ[off_index].w_int;

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      إذا (thr->flags[4] == BIT4_1)
	    أرجع true;

      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*> (cp->net->fil);
      أكد(sig);

      إذا (!resize_rval_vec(val, off, sig->قدر_size()))
	    أرجع true;

      إذا (thr->ecount == 0) {
	    schedule_assign_vector(ptr, off, sig->قدر_size(), val, 0);
      } وإلا {
	    schedule_evctl(ptr, val, off, sig->قدر_size(), thr->event, thr->ecount);
      }

      أرجع true;
}

/*
 * %assign/vec4/d <var-label> <delay>
 */
منطقي of_ASSIGN_VEC4D(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      unsigned del_index = cp->bit_idx[0];
      vvp_time64_t del = thr->الفاظ[del_index].w_int;

      vvp_vector4_t value = thr->نکالو_vec4();

      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*> (cp->net->fil);
      أكد(sig);

      schedule_assign_vector(ptr, 0, sig->قدر_size(), value, del);

      أرجع true;
}

/*
 * %assign/vec4/e <var-label>
 */
منطقي of_ASSIGN_VEC4E(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_vector4_t value = thr->نکالو_vec4();

      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*> (cp->net->fil);
      أكد(sig);

      إذا (thr->ecount == 0) {
	    schedule_assign_vector(ptr, 0, sig->قدر_size(), value, 0);
      } وإلا {
	    schedule_evctl(ptr, value, 0, sig->قدر_size(), thr->event, thr->ecount);
      }

      أرجع true;
}

/*
 * This is %assign/wr <vpi-label>, <delay>
 *
 * This assigns (after a delay) a value to a real variable. Use the
 * vpi_put_value function to do the assign, with the delay written
 * into the vpiInertialDelay carrying the desired delay.
 */
منطقي of_ASSIGN_WR(vthread_t thr, vvp_code_t cp)
{
      unsigned delay = cp->bit_idx[0];
      مزدوج value = thr->نکالو_real();
      s_vpi_time del;

      del.type = vpiSimTime;
      vpip_time_to_timestruct(&del, delay);

      __vpiHandle*tmp = cp->handle;

      t_vpi_قدر val;
      val.format = vpiRealVal;
      val.value.real = value;
      vpi_put_قدر(tmp, &val, &del, vpiTransportDelay);

      أرجع true;
}

منطقي of_ASSIGN_WRD(vthread_t thr, vvp_code_t cp)
{
      vvp_time64_t delay = thr->الفاظ[cp->bit_idx[0]].w_uint;
      مزدوج value = thr->نکالو_real();
      s_vpi_time del;

      del.type = vpiSimTime;
      vpip_time_to_timestruct(&del, delay);

      __vpiHandle*tmp = cp->handle;

      t_vpi_قدر val;
      val.format = vpiRealVal;
      val.value.real = value;
      vpi_put_قدر(tmp, &val, &del, vpiTransportDelay);

      أرجع true;
}

منطقي of_ASSIGN_WRE(vthread_t thr, vvp_code_t cp)
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
	    schedule_evctl(tmp, value, thr->event, thr->ecount);
      }

      thr->event = 0;
      thr->ecount = 0;

      أرجع true;
}

منطقي of_BLEND(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t vala = thr->نکالو_vec4();
      vvp_vector4_t valb = thr->نکالو_vec4();
      أكد(vala.size() == valb.size());

      لكل (unsigned idx = 0 ; idx < vala.size() ; idx += 1) {
	    إذا (vala.value(idx) == valb.value(idx))
		  استمر;

	    vala.set_bit(idx, BIT4_X);
      }

      thr->ڈالو_vec4(vala);
      أرجع true;
}

منطقي of_BLEND_STR(vthread_t thr, vvp_code_t)
{
      string f = thr->نکالو_str();
      string t = thr->نکالو_str();
      thr->ڈالو_str((t == f) ? t : "");
      أرجع true;
}

منطقي of_BLEND_WR(vthread_t thr, vvp_code_t)
{
      مزدوج f = thr->نکالو_real();
      مزدوج t = thr->نکالو_real();
      thr->ڈالو_real((t == f) ? t : 0.0);
      أرجع true;
}

منطقي of_BREAKPOINT(vthread_t, vvp_code_t)
{
      أرجع true;
}

/*
 * %callf/void <code-label>, <scope-label>
 * Combine the %fork and %join steps for invoking a function.
 */
ساكن منطقي do_callf_void(vthread_t thr, vthread_t child)
{

      إذا (child->parent_scope->is_automatic()) {
	      /* The context allocated for this child is the top entry
		 on the write context stack */
	    child->wt_context = thr->wt_context;
	    child->rd_context = thr->wt_context;
      }

        // Mark the function thread as a direct child of the current thread.
      child->parent = thr;
      thr->children.insert(child);
        // This should be the only child
      أكد(thr->children.size()==1);

        // Execute the function. This SHOULD run the function to completion,
        // but there are some exceptional situations where it won't.
      أكد(child->parent_scope->get_type_code() == vpiFunction);
      child->is_scheduled = 1;
      child->i_am_in_function = 1;
      vthread_run(child);
      running_thread = thr;

      إذا (child->i_have_ended) {
	    do_join(thr, child);
	    أرجع true;
      } وإلا {
	    thr->i_am_joining = 1;
	    أرجع خطأ;
      }
}

منطقي of_CALLF_OBJ(vthread_t thr, vvp_code_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);
      أرجع do_callf_void(thr, child);

      // XXXX NOT IMPLEMENTED
}

منطقي of_CALLF_REAL(vthread_t thr, vvp_code_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);

	// This is the return value. Push a place-holder value. The function
	// will replace this with the actual value using a %ret/real instruction.
      thr->ڈالو_real(0.0);
      child->args_real.push_back(0);

      أرجع do_callf_void(thr, child);
}

منطقي of_CALLF_STR(vthread_t thr, vvp_code_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);

      thr->ڈالو_str("");
      child->args_str.push_back(0);

      أرجع do_callf_void(thr, child);
}

منطقي of_CALLF_VEC4(vthread_t thr, vvp_code_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);

      vpiScopeFunction*scope_func = dynamic_cast<vpiScopeFunction*>(cp->scope);
      أكد(scope_func);

	// This is the return value. Push a place-holder value. The function
	// will replace this with the actual value using a %ret/real instruction.
      thr->ڈالو_vec4(vvp_vector4_t(scope_func->get_func_width(), scope_func->get_func_init_val()));
      child->args_vec4.push_back(0);

      أرجع do_callf_void(thr, child);
}

منطقي of_CALLF_VOID(vthread_t thr, vvp_code_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);
      أرجع do_callf_void(thr, child);
}

/*
 * The %cassign/link instruction connects a source node to a
 * destination node. The destination node must be a signal, as it is
 * marked with the source of the cassign so that it may later be
 * unlinked without specifically knowing the source that this
 * instruction used.
 */
منطقي of_CASSIGN_LINK(vthread_t, vvp_code_t cp)
{
      vvp_net_t*dst = cp->net;
      vvp_net_t*src = cp->net2;

      vvp_fun_signal_base*sig
	    = dynamic_cast<vvp_fun_signal_base*>(dst->fun);
      أكد(sig);

	/* Any previous continuous assign should have been removed already. */
      أكد(sig->cassign_link == 0);

      sig->cassign_link = src;

	/* Link the output of the src to the port[1] (the cassign
	   port) of the destination. */
      vvp_net_ptr_t dst_ptr (dst, 1);
      src->link(dst_ptr);

      أرجع true;
}

/*
 * If there is an existing continuous assign linked to the destination
 * node, unlink it. This must be done before applying a new continuous
 * assign, otherwise the initial assigned value will be propagated to
 * any other nodes driven by the old continuous assign source.
 */
ساكن فراغ cassign_unlink(vvp_net_t*dst)
{
      vvp_fun_signal_base*sig
	    = dynamic_cast<vvp_fun_signal_base*>(dst->fun);
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
منطقي of_CASSIGN_VEC4(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      vvp_vector4_t value = thr->نکالو_vec4();

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

	/* Set the value into port 1 of the destination. */
      vvp_net_ptr_t ptr (net, 1);
      vvp_send_vec4(ptr, value, 0);

      أرجع true;
}

/*
 * %cassign/vec4/off <var>, <off>
 */
منطقي of_CASSIGN_VEC4_OFF(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      unsigned base_idx = cp->bit_idx[0];
      long base = thr->الفاظ[base_idx].w_int;
      vvp_vector4_t value = thr->نکالو_vec4();
      unsigned wid = value.size();

      إذا (thr->flags[4] == BIT4_1)
	    أرجع true;

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*> (net->fil);
      أكد(sig);

      إذا (base < 0 && (wid <= (unsigned)-base))
	    أرجع true;

      إذا (base >= (long)sig->قدر_size())
	    أرجع true;

      إذا (base < 0) {
	    wid -= (unsigned) -base;
	    base = 0;
	    value.resize(wid);
      }

      إذا (base+wid > sig->قدر_size()) {
	    wid = sig->قدر_size() - base;
	    value.resize(wid);
      }

      vvp_net_ptr_t ptr (net, 1);
      vvp_send_vec4_pv(ptr, value, base, sig->قدر_size(), 0);
      أرجع true;
}

منطقي of_CASSIGN_WR(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net  = cp->net;
      مزدوج value = thr->نکالو_real();

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

	/* Set the value into port 1 of the destination. */
      vvp_net_ptr_t ptr (net, 1);
      vvp_send_real(ptr, value, 0);

      أرجع true;
}

/*
 * %cast2
 */
منطقي of_CAST2(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      unsigned wid = val.size();

      لكل (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    بدل (val.value(idx)) {
		حالة BIT4_0:
		حالة BIT4_1:
		  اكسر;
		افتراضي:
		  val.set_bit(idx, BIT4_0);
		  اكسر;
	    }
      }

      أرجع true;
}

منطقي do_cast_vec_dar(vthread_t thr, vvp_code_t cp, منطقي as_vec4)
{
      unsigned wid = cp->نمبر;

      vvp_object_t obj;
      thr->نکالو_object(obj);

      vvp_darray*darray = obj.peek<vvp_darray>();
      أكد(darray);

      vvp_vector4_t vec = darray->get_bitstream(as_vec4);
      إذا (vec.size() != wid) {
	    cerr << thr->get_fileline()
	         << "VVP error: size mismatch when casting dynamic array to vector." << endl;
            thr->ڈالو_vec4(vvp_vector4_t(wid));
            schedule_stop(0);
            أرجع خطأ;
      }
      thr->ڈالو_vec4(vec);
      أرجع true;
}

/*
 * %cast/vec2/dar <wid>
 */
منطقي of_CAST_VEC2_DAR(vthread_t thr, vvp_code_t cp)
{
      أرجع do_cast_vec_dar(thr, cp, خطأ);
}

/*
 * %cast/vec4/dar <wid>
 */
منطقي of_CAST_VEC4_DAR(vthread_t thr, vvp_code_t cp)
{
      أرجع do_cast_vec_dar(thr, cp, true);
}

/*
 * %cast/vec4/str <wid>
 */
منطقي of_CAST_VEC4_STR(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->نمبر;
      string str = thr->نکالو_str();

      vvp_vector4_t vec(wid, BIT4_0);

      إذا (wid != 8*str.length()) {
	    cerr << thr->get_fileline()
	         << "VVP error: size mismatch when casting string to vector." << endl;
            thr->ڈالو_vec4(vec);
            schedule_stop(0);
            أرجع خطأ;
      }

      unsigned sdx = 0;
      unsigned vdx = wid;
      طالما (vdx > 0) {
            حرف ch = str[sdx++];
            vdx -= 8;
            لكل (unsigned bdx = 0; bdx < 8; bdx += 1) {
                  إذا (ch & 1)
                        vec.set_bit(vdx+bdx, BIT4_1);
                  ch >>= 1;
            }
      }

      thr->ڈالو_vec4(vec);
      أرجع true;
}

ساكن فراغ do_CMPE(vthread_t thr, ثابت vvp_vector4_t&lval, ثابت vvp_vector4_t&rval)
{
      أكد(rval.size() == lval.size());

      إذا (lval.has_xz() || rval.has_xz()) {

	    unsigned wid = lval.size();
	    vvp_bit4_t eq  = BIT4_1;
	    vvp_bit4_t eeq = BIT4_1;

	    لكل (unsigned idx = 0 ; idx < wid ; idx += 1) {
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
منطقي of_CMPE(vthread_t thr, vvp_code_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ثابت vvp_vector4_t&rval = thr->peek_vec4(0);
      ثابت vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPE(thr, lval, rval);

      thr->نکالو_vec4(2);
      أرجع true;
}

منطقي of_CMPNE(vthread_t thr, vvp_code_t)
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
      أرجع true;
}

/*
 * %cmpi/e <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
منطقي of_CMPIE(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->نمبر;

      ثابت vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_immediate_rval (cp, rval);

      do_CMPE(thr, lval, rval);

      thr->نکالو_vec4(1);
      أرجع true;
}

منطقي of_CMPINE(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->نمبر;

      ثابت vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_immediate_rval (cp, rval);

      do_CMPE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];
      thr->flags[6] =  ~thr->flags[6];

      thr->نکالو_vec4(1);
      أرجع true;
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
      unsigned wid = lval.size();

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

      لكل (unsigned idx = 1 ;  idx < wid ;  idx += 1) {
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
منطقي of_CMPS(vthread_t thr, vvp_code_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ثابت vvp_vector4_t&rval = thr->peek_vec4(0);
      ثابت vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPS(thr, lval, rval);

      thr->نکالو_vec4(2);
      أرجع true;
}

/*
 * %cmpi/s <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
منطقي of_CMPIS(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->نمبر;

      ثابت vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_immediate_rval (cp, rval);

      do_CMPS(thr, lval, rval);

      thr->نکالو_vec4(1);
      أرجع true;
}

منطقي of_CMPSTR(vthread_t thr, vvp_code_t)
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

      أرجع true;
}

ساكن فراغ of_CMPU_the_hard_way(vthread_t thr, unsigned wid,
				 ثابت vvp_vector4_t&lval,
				 ثابت vvp_vector4_t&rval)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_bit4_t eeq = BIT4_1;

      لكل (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
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
		 << ", rval=" << rval << endl;
      }
      أكد(rval.size() == lval.size());
      unsigned wid = lval.size();

      unsigned long*larray = lval.subarray(0,wid);
      إذا (larray == 0) أرجع of_CMPU_the_hard_way(thr, wid, lval, rval);

      unsigned long*rarray = rval.subarray(0,wid);
      إذا (rarray == 0) {
	    delete[]larray;
	    أرجع of_CMPU_the_hard_way(thr, wid, lval, rval);
      }

      unsigned الفاظ = (wid+CPU_WORD_BITS-1) / CPU_WORD_BITS;

      لكل (unsigned wdx = 0 ; wdx < الفاظ ; wdx += 1) {
	    إذا (larray[wdx] == rarray[wdx])
		  استمر;

	    eq = BIT4_0;
	    إذا (larray[wdx] < rarray[wdx])
		  lt = BIT4_1;
	    وإلا
		  lt = BIT4_0;
      }

      delete[]larray;
      delete[]rarray;

      thr->flags[4] = eq;
      thr->flags[5] = lt;
      thr->flags[6] = eq;
}

منطقي of_CMPU(vthread_t thr, vvp_code_t)
{

      ثابت vvp_vector4_t&rval = thr->peek_vec4(0);
      ثابت vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPU(thr, lval, rval);

      thr->نکالو_vec4(2);
      أرجع true;
}

/*
 * %cmpi/u <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
منطقي of_CMPIU(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->نمبر;

      ثابت vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_immediate_rval (cp, rval);

      do_CMPU(thr, lval, rval);

      thr->نکالو_vec4(1);
      أرجع true;
}


/*
 * %cmp/x
 */
منطقي of_CMPX(vthread_t thr, vvp_code_t)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_vector4_t rval = thr->نکالو_vec4();
      vvp_vector4_t lval = thr->نکالو_vec4();

      أكد(rval.size() == lval.size());
      unsigned wid = lval.size();

      لكل (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    vvp_bit4_t lv = lval.value(idx);
	    vvp_bit4_t rv = rval.value(idx);
	    إذا ((lv != rv) && !bit4_is_xz(lv) && !bit4_is_xz(rv)) {
		  eq = BIT4_0;
		  اكسر;
	    }
      }

      thr->flags[4] = eq;
      أرجع true;
}

ساكن فراغ do_CMPWE(vthread_t thr, ثابت vvp_vector4_t&lval, ثابت vvp_vector4_t&rval)
{
      أكد(rval.size() == lval.size());

      إذا (lval.has_xz() || rval.has_xz()) {

	    unsigned wid = lval.size();
	    vvp_bit4_t eq  = BIT4_1;

	    لكل (unsigned idx = 0 ; idx < wid ; idx += 1) {
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

منطقي of_CMPWE(vthread_t thr, vvp_code_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ثابت vvp_vector4_t&rval = thr->peek_vec4(0);
      ثابت vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPWE(thr, lval, rval);

      thr->نکالو_vec4(2);
      أرجع true;
}

منطقي of_CMPWNE(vthread_t thr, vvp_code_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ثابت vvp_vector4_t&rval = thr->peek_vec4(0);
      ثابت vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPWE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];

      thr->نکالو_vec4(2);
      أرجع true;
}

منطقي of_CMPWR(vthread_t thr, vvp_code_t)
{
      مزدوج r = thr->نکالو_real();
      مزدوج l = thr->نکالو_real();

      vvp_bit4_t eq = (l == r)? BIT4_1 : BIT4_0;
      vvp_bit4_t lt = (l <  r)? BIT4_1 : BIT4_0;

      thr->flags[4] = eq;
      thr->flags[5] = lt;

      أرجع true;
}

/*
 * %cmp/z
 */
منطقي of_CMPZ(vthread_t thr, vvp_code_t)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_vector4_t rval = thr->نکالو_vec4();
      vvp_vector4_t lval = thr->نکالو_vec4();

      أكد(rval.size() == lval.size());
      unsigned wid = lval.size();

      لكل (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    vvp_bit4_t lv = lval.value(idx);
	    vvp_bit4_t rv = rval.value(idx);
	    إذا ((lv != rv) && (rv != BIT4_Z) && (lv != BIT4_Z)) {
		  eq = BIT4_0;
		  اكسر;
	    }
      }

      thr->flags[4] = eq;
      أرجع true;
}

/*
 *  %concat/str;
 */
منطقي of_CONCAT_STR(vthread_t thr, vvp_code_t)
{
      string text = thr->نکالو_str();
      thr->peek_str(0).append(text);
      أرجع true;
}

/*
 *  %concati/str <string>;
 */
منطقي of_CONCATI_STR(vthread_t thr, vvp_code_t cp)
{
      ثابت حرف*text = cp->text;
      thr->peek_str(0).append(filter_string(text));
      أرجع true;
}

/*
 * %concat/vec4
 */
منطقي of_CONCAT_VEC4(vthread_t thr, vvp_code_t)
{
      ثابت vvp_vector4_t&lsb = thr->peek_vec4(0);
      ثابت vvp_vector4_t&msb = thr->peek_vec4(1);

	// The result is the size of the top two vectors in the stack.
      vvp_vector4_t res (msb.size() + lsb.size(), BIT4_X);

	// Build up the result.
      res.set_vec(0, lsb);
      res.set_vec(lsb.size(), msb);

	// Rearrange the stack to pop the inputs and push the
	// result. Do that by actually popping only 1 stack position
	// and replacing the new top with the new value.
      thr->نکالو_vec4(1);
      thr->peek_vec4() = res;

      أرجع true;
}

/*
 * %concati/vec4 <vala>, <valb>, <wid>
 *
 * Concat the immediate value to the LOW bits of the concatenation.
 * Get the HIGH bits from the top of the vec4 stack.
 */
منطقي of_CONCATI_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned wid  = cp->نمبر;

      vvp_vector4_t&msb = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t lsb (wid, BIT4_0);
      get_immediate_rval (cp, lsb);

      vvp_vector4_t res (msb.size()+lsb.size(), BIT4_X);
      res.set_vec(0, lsb);
      res.set_vec(lsb.size(), msb);

      msb = res;
      أرجع true;
}

/*
 * %cvt/rv
 */
منطقي of_CVT_RV(vthread_t thr, vvp_code_t)
{
      مزدوج val;
      vvp_vector4_t val4 = thr->نکالو_vec4();
      vector4_to_قدر(val4, val, خطأ);
      thr->ڈالو_real(val);
      أرجع true;
}

/*
 * %cvt/rv/s
 */
منطقي of_CVT_RV_S(vthread_t thr, vvp_code_t)
{
      مزدوج val;
      vvp_vector4_t val4 = thr->نکالو_vec4();
      vector4_to_قدر(val4, val, true);
      thr->ڈالو_real(val);
      أرجع true;
}

/*
 * %cvt/sr <idx>
 * Pop the top value from the real stack, convert it to a 64bit signed
 * and save it to the indexed register.
 */
منطقي of_CVT_SR(vthread_t thr, vvp_code_t cp)
{
      مزدوج r = thr->نکالو_real();
      thr->الفاظ[cp->bit_idx[0]].w_int = i64round(r);

      أرجع true;
}

/*
 * %cvt/ur <idx>
 */
منطقي of_CVT_UR(vthread_t thr, vvp_code_t cp)
{
      مزدوج r = thr->نکالو_real();
      thr->الفاظ[cp->bit_idx[0]].w_uint = vlg_round_to_u64(r);

      أرجع true;
}

/*
 * %cvt/vr <wid>
 */
منطقي of_CVT_VR(vthread_t thr, vvp_code_t cp)
{
      مزدوج r = thr->نکالو_real();
      unsigned wid = cp->نمبر;

      vvp_vector4_t tmp(wid, r);
      thr->ڈالو_vec4(tmp);
      أرجع true;
}

/*
 * This implements the %deassign instruction. All we do is write a
 * long(1) to port-3 of the addressed net. This turns off an active
 * continuous assign activated by %cassign/v
 */
منطقي of_DEASSIGN(vthread_t, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      unsigned base  = cp->bit_idx[0];
      unsigned width = cp->bit_idx[1];

      vvp_signal_قدر*fil = dynamic_cast<vvp_signal_قدر*> (net->fil);
      أكد(fil);
      vvp_fun_signal_vec*sig = dynamic_cast<vvp_fun_signal_vec*>(net->fun);
      أكد(sig);

      إذا (base >= fil->قدر_size()) أرجع true;
      إذا (base+width > fil->قدر_size()) width = fil->قدر_size() - base;

      منطقي full_sig = base == 0 && width == fil->قدر_size();

	// This is the net that is forcing me...
      إذا (vvp_net_t*src = sig->cassign_link) {
	    إذا (! full_sig) {
		  fprintf(stderr, "Sorry: when a signal is assigning a "
		          "register, I cannot deassign part of it.\n");
		  exit(1);
	    }
	      // And this is the pointer to be removed.
	    vvp_net_ptr_t dst_ptr (net, 1);
	    src->unlink(dst_ptr);
	    sig->cassign_link = 0;
      }

	/* Do we release all or part of the net? */
      إذا (full_sig) {
	    sig->deassign();
      } وإلا {
	    sig->deassign_pv(base, width);
      }

      أرجع true;
}

منطقي of_DEASSIGN_WR(vthread_t, vvp_code_t cp)
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

      أرجع true;
}

/*
 * %debug/thr
 */
منطقي of_DEBUG_THR(vthread_t thr, vvp_code_t cp)
{
      ثابت حرف*text = cp->text;
      thr->debug_dump(cerr, text);
      أرجع true;
}

/*
 * The delay takes two 32bit numbers to make up a 64bit time.
 *
 *   %delay <low>, <hig>
 */
منطقي of_DELAY(vthread_t thr, vvp_code_t cp)
{
      vvp_time64_t کم = cp->bit_idx[0];
      vvp_time64_t hig = cp->bit_idx[1];

      vvp_time64_t delay = (hig << 32) | کم;

      إذا (delay == 0) schedule_inactive(thr);
      وإلا schedule_vthread(thr, delay);
      أرجع خطأ;
}

منطقي of_DELAYX(vthread_t thr, vvp_code_t cp)
{
      vvp_time64_t delay;

      أكد(cp->نمبر < vthread_s::الفاظ_گنتی);
      delay = thr->الفاظ[cp->نمبر].w_uint;
      إذا (delay == 0) schedule_inactive(thr);
      وإلا schedule_vthread(thr, delay);
      أرجع خطأ;
}

منطقي of_DELETE_ELEM(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      int64_t idx_val = thr->الفاظ[3].w_int;
      إذا (thr->flags[4] == BIT4_1) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with undefined index."
	         << endl;
	    أرجع true;
      }
      إذا (idx_val < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with negative index."
	         << endl;
	    أرجع true;
      }
      size_t idx = idx_val;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      أكد(obj);

      vvp_queue*queue = obj->get_object().peek<vvp_queue>();
      إذا (queue == 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping delete(" << idx
	         << ") on empty queue." << endl;
      } وإلا {
	    size_t size = queue->get_size();
	    إذا (idx >= size) {
		  cerr << thr->get_fileline()
		       << "Warning: skipping out of range delete(" << idx
		       << ") on queue of size " << size << "." << endl;
	    } وإلا {
		  queue->erase(idx);
	    }
      }

      أرجع true;
}

/* %delete/obj <label>
 *
 * This operator works by assigning a nil to the target object. This
 * causes any value that might be there to be garbage collected, thus
 * deleting the object.
 */
منطقي of_DELETE_OBJ(vthread_t thr, vvp_code_t cp)
{
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_send_object(ptr, vvp_object_t(), thr->wt_context);

      أرجع true;
}

/* %reverse/obj <label>
 * Reverse element order of the queue or dynamic array object at this net.
 */
منطقي of_REVERSE_OBJ(vthread_t /*thr*/, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      أكد(obj);

      vvp_object_t stor = obj->get_object();
      إذا (stor.test_nil()) أرجع true;

	/* Match other queue ops: peek queues before vvp_darray so concrete
	   queue types dispatch reverse_elems() correctly. */
      إذا (vvp_queue* queue = stor.peek<vvp_queue>()) {
	    queue->reverse_elems();
	    أرجع true;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      أكد(darray);
      darray->reverse_elems();

      أرجع true;
}

/* %reverse/prop/obj <property-index>
 * Reverse queue/darray property storage inside the class object on stack top.
 */
منطقي of_REVERSE_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->نمبر;

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      إذا (arr_obj.test_nil()) أرجع true;

      إذا (vvp_queue* queue = arr_obj.peek<vvp_queue>()) {
	    queue->reverse_elems();
	    أرجع true;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      أكد(darray);
      darray->reverse_elems();

      أرجع true;
}

/* %sort/obj — ascending sort */
منطقي of_SORT_OBJ(vthread_t /*thr*/, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      أكد(obj);

      vvp_object_t stor = obj->get_object();
      إذا (stor.test_nil()) أرجع true;

      إذا (vvp_queue* queue = stor.peek<vvp_queue>()) {
	    queue->sort_elems(true);
	    أرجع true;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      أكد(darray);
      darray->sort_elems(true);

      أرجع true;
}

/* %sort/prop/obj */
منطقي of_SORT_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->نمبر;

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      إذا (arr_obj.test_nil()) أرجع true;

      إذا (vvp_queue* queue = arr_obj.peek<vvp_queue>()) {
	    queue->sort_elems(true);
	    أرجع true;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      أكد(darray);
      darray->sort_elems(true);

      أرجع true;
}

/* %rsort/obj — descending sort */
منطقي of_RSORT_OBJ(vthread_t /*thr*/, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      أكد(obj);

      vvp_object_t stor = obj->get_object();
      إذا (stor.test_nil()) أرجع true;

      إذا (vvp_queue* queue = stor.peek<vvp_queue>()) {
	    queue->sort_elems(خطأ);
	    أرجع true;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      أكد(darray);
      darray->sort_elems(خطأ);

      أرجع true;
}

/* %rsort/prop/obj */
منطقي of_RSORT_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->نمبر;

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      إذا (arr_obj.test_nil()) أرجع true;

      إذا (vvp_queue* queue = arr_obj.peek<vvp_queue>()) {
	    queue->sort_elems(خطأ);
	    أرجع true;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      أكد(darray);
      darray->sort_elems(خطأ);

      أرجع true;
}

/* %shuffle/obj */
منطقي of_SHUFFLE_OBJ(vthread_t /*thr*/, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      أكد(obj);

      vvp_object_t stor = obj->get_object();
      إذا (stor.test_nil()) أرجع true;

      إذا (vvp_queue* queue = stor.peek<vvp_queue>()) {
	    queue->shuffle_elems();
	    أرجع true;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      أكد(darray);
      darray->shuffle_elems();

      أرجع true;
}

/* %shuffle/prop/obj */
منطقي of_SHUFFLE_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->نمبر;

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      إذا (arr_obj.test_nil()) أرجع true;

      إذا (vvp_queue* queue = arr_obj.peek<vvp_queue>()) {
	    queue->shuffle_elems();
	    أرجع true;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      أكد(darray);
      darray->shuffle_elems();

      أرجع true;
}

/* %delete/tail <label>, idx
 *
 * Remove all elements after the one specified.
 */
منطقي of_DELETE_TAIL(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      أكد(obj);

      vvp_queue*queue = obj->get_object().peek<vvp_queue>();
      أكد(queue);

      unsigned idx = thr->الفاظ[cp->bit_idx[0]].w_int;
      queue->erase_tail(idx);

      أرجع true;
}

ساكن منطقي do_disable(vthread_t thr, vthread_t match)
{
      منطقي flag = خطأ;

	/* Pull the target thread out of its scope if needed. */
      thr->parent_scope->threads.erase(thr);

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
	    أكد(tmp->parent == thr);
	    thr->i_am_joining = 0;
	    إذا (do_disable(tmp, match))
		  flag = true;

	    vthread_reap(tmp);
      }

      vthread_t parent = thr->parent;
      إذا (parent && parent->i_am_joining) {
	      // If a parent is waiting in a %join, wake it up. Note
	      // that it is possible to be waiting in a %join yet
	      // already scheduled if multiple child threads are
	      // ending. So check if the thread is already scheduled
	      // before scheduling it again.
	    parent->i_am_joining = 0;
	    إذا (! parent->i_have_ended)
		  schedule_vthread(parent, 0, true);

	    do_join(parent, thr);

      } وإلا إذا (parent) {
	      /* If the parent is yet to %join me, let its %join
		 do the reaping. */
	      //assert(tmp->is_scheduled == 0);

      } وإلا {
	      /* No parent at all. Goodbye. */
	    vthread_reap(thr);
      }

      أرجع flag || (thr == match);
}

/*
 * Implement the %disable instruction by scanning the target scope for
 * all the target threads. Kill the target threads and wake up a
 * parent that is attempting a %join.
 */
منطقي of_DISABLE(vthread_t thr, vvp_code_t cp)
{
      __vpiScope*scope = static_cast<__vpiScope*>(cp->handle);

      منطقي disabled_myself_flag = خطأ;

      طالما (! scope->threads.empty()) {
	    set<vthread_t>::iterator cur = scope->threads.begin();

	    إذا (do_disable(*cur, thr))
		  disabled_myself_flag = true;
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

منطقي of_DISABLE_FLOW(vthread_t thr, vvp_code_t cp)
{
      ثابت __vpiScope*scope = static_cast<__vpiScope*>(cp->handle);
      vthread_t cur = thr;

      طالما (cur && cur->parent_scope != scope)
	    cur = cur->parent;

      أكد(cur);
      إذا (cur)
	    أرجع !do_disable(cur, thr);

      أرجع خطأ;
}

/*
 * Implement the %disable/fork (SystemVerilog) instruction by disabling
 * all the detached children of the given thread.
 */
منطقي of_DISABLE_FORK(vthread_t thr, vvp_code_t)
{
	/* If a %disable/fork is being executed then the parent thread
	 * cannot be waiting in a join. */
      أكد(! thr->i_am_joining);

	/* There should be no active children to disable. */
      أكد(thr->children.empty());

	/* Disable any detached children. */
      طالما (! thr->detached_children.empty()) {
	    vthread_t child = *(thr->detached_children.begin());
	    أكد(child);
	    أكد(child->parent == thr);
	      /* Disabling the children can never match the parent thread. */
	    منطقي res = do_disable(child, thr);
	    أكد(! res);
	    vthread_reap(child);
      }

      أرجع true;
}

/*
 * This function divides a 2-word number {high, a} by a 1-word
 * number. Assume that high < b.
 */
ساكن unsigned long divide2words(unsigned long a, unsigned long b,
				  unsigned long بلند)
{
      unsigned long result = 0;
      طالما (بلند > 0) {
	    unsigned long tmp_result = ULONG_MAX / b;
	    unsigned long remain = ULONG_MAX % b;

	    remain += 1;
	    إذا (remain >= b) {
		  remain -= b;
		  tmp_result += 1;
	    }

	      // Now 0x1_0...0 = b*tmp_result + remain
	      // high*0x1_0...0 = high*(b*tmp_result + remain)
	      // high*0x1_0...0 = high*b*tmp_result + high*remain

	      // We know that high*0x1_0...0 >= high*b*tmp_result, and
	      // we know that high*0x1_0...0 > high*remain. Use
	      // high*remain as the remainder for another iteration,
	      // and add tmp_result*high into the current estimate of
	      // the result.
	    result += tmp_result * بلند;

	      // The new iteration starts with high*remain + a.
	    remain = multiply_with_carry(بلند, remain, بلند);
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

      أرجع result + a/b;
}

ساكن unsigned long* divide_bits(unsigned long*ap, ثابت unsigned long*bp, unsigned wid)
{
	// Do all our work a cpu-word at a time. The "words" variable
	// is the number of words of the wid.
      unsigned الفاظ = (wid+CPU_WORD_BITS-1) / CPU_WORD_BITS;

      unsigned btop = الفاظ-1;
      طالما (btop > 0 && bp[btop] == 0)
	    btop -= 1;

	// Detect divide by 0, and exit.
      إذا (btop==0 && bp[0]==0)
	    أرجع 0;

	// The result array will eventually accumulate the result. The
	// diff array is a difference that we use in the intermediate.
      unsigned long*diff  = جديد unsigned long[الفاظ];
      unsigned long*result= جديد unsigned long[الفاظ];
      لكل (unsigned idx = 0 ; idx < الفاظ ; idx += 1)
	    result[idx] = 0;

      لكل (unsigned cur = الفاظ-btop ; cur > 0 ; cur -= 1) {
	    unsigned cur_ptr = cur-1;
	    unsigned long cur_res;
	    إذا (ap[cur_ptr+btop] >= bp[btop]) {
		  unsigned long بلند = 0;
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
	    unsigned long carry = 1;
	    لكل (unsigned idx = cur_ptr ; idx < الفاظ ; idx += 1)
		  ap[idx] = add_with_carry(ap[idx], ~diff[idx], carry);

	      // ap has the diff subtracted out of it. If cur_res was
	      // too large, then ap will turn negative. (We easily
	      // tell that ap turned negative by looking at
	      // carry&1. If it is 0, then it is *negative*.) In that
	      // case, we know that cur_res was too large by 1. Correct by
	      // adding 1b back in and reducing cur_res.
	    إذا ((carry&1) == 0) {
		    // Keep adding b back in until the remainder
		    // becomes positive again.
		  افعل {
			cur_res -= 1;
			carry = 0;
			لكل (unsigned idx = cur_ptr ; idx < الفاظ ; idx += 1)
			      ap[idx] = add_with_carry(ap[idx], bp[idx-cur_ptr], carry);
		  } طالما (carry == 0);
	    }

	    result[cur_ptr] = cur_res;
      }

	// Now ap contains the remainder and result contains the
	// desired result. We should find that:
	//  input-a = bp * result + ap;

      delete[]diff;
      أرجع result;
}

/*
 * %div
 */
منطقي of_DIV(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valb = thr->نکالو_vec4();
      vvp_vector4_t vala = thr->نکالو_vec4();

      أكد(vala.size()== valb.size());
      unsigned wid = vala.size();

      unsigned long*ap = vala.subarray(0, wid);
      إذا (ap == 0) {
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->ڈالو_vec4(tmp);
	    أرجع true;
      }

      unsigned long*bp = valb.subarray(0, wid);
      إذا (bp == 0) {
	    delete[]ap;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->ڈالو_vec4(tmp);
	    أرجع true;
      }

	// If the value fits in a single CPU word, then do it the easy way.
      إذا (wid <= CPU_WORD_BITS) {
	    إذا (bp[0] == 0) {
		  vvp_vector4_t tmp(wid, BIT4_X);
		  thr->ڈالو_vec4(tmp);
	    } وإلا {
		  ap[0] /= bp[0];
		  vala.setarray(0, wid, ap);
		  thr->ڈالو_vec4(vala);
	    }
	    delete[]ap;
	    delete[]bp;
	    أرجع true;
      }

      unsigned long*result = divide_bits(ap, bp, wid);
      إذا (result == 0) {
	    delete[]ap;
	    delete[]bp;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->ڈالو_vec4(tmp);
	    أرجع true;
      }

	// Now ap contains the remainder and result contains the
	// desired result. We should find that:
	//  input-a = bp * result + ap;

      vala.setarray(0, wid, result);
      thr->ڈالو_vec4(vala);
      delete[]ap;
      delete[]bp;
      delete[]result;

      أرجع true;
}


ساكن فراغ negate_الفاظ(unsigned long*val, unsigned الفاظ)
{
      unsigned long carry = 1;
      لكل (unsigned idx = 0 ; idx < الفاظ ; idx += 1)
	    val[idx] = add_with_carry(0, ~val[idx], carry);
}

/*
 * %div/s
 */
منطقي of_DIV_S(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valb = thr->نکالو_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      أكد(vala.size()== valb.size());
      unsigned wid = vala.size();
      unsigned الفاظ = (wid + CPU_WORD_BITS - 1) / CPU_WORD_BITS;

	// Get the values, left in right, in binary form. If there is
	// a problem with either (caused by an X or Z bit) then we
	// know right away that the entire result is X.
      unsigned long*ap = vala.subarray(0, wid);
      إذا (ap == 0) {
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    أرجع true;
      }

      unsigned long*bp = valb.subarray(0, wid);
      إذا (bp == 0) {
	    delete[]ap;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    أرجع true;
      }

	// Sign extend the bits in the array to fill out the array.
      unsigned long sign_mask = 0;
      إذا (unsigned long sign_bits = (الفاظ*CPU_WORD_BITS) - wid) {
	    sign_mask = -1UL << (CPU_WORD_BITS-sign_bits);
	    إذا (ap[الفاظ-1] & (sign_mask>>1))
		  ap[الفاظ-1] |= sign_mask;
	    إذا (bp[الفاظ-1] & (sign_mask>>1))
		  bp[الفاظ-1] |= sign_mask;
      }

	// If the value fits in a single word, then use the native divide.
      إذا (wid <= CPU_WORD_BITS) {
	    إذا (bp[0] == 0) {
		  vvp_vector4_t tmp(wid, BIT4_X);
		  vala = tmp;
	    } وإلا إذا (((long)ap[0] == LONG_MIN) && ((long)bp[0] == -1)) {
		  vvp_vector4_t tmp(wid, BIT4_0);
		  tmp.set_bit(wid-1, BIT4_1);
		  vala = tmp;
	    } وإلا {
		  long tmpa = (long) ap[0];
		  long tmpb = (long) bp[0];
		  long res = tmpa / tmpb;
		  ap[0] = ((unsigned long)res) & ~sign_mask;
		  vala.setarray(0, wid, ap);
	    }
	    delete[]ap;
	    delete[]bp;
	    أرجع true;
      }

	// We need to the actual division to positive integers. Make
	// them positive here, and remember the negations.
      منطقي negate_flag = خطأ;
      إذا ( ((long) ap[الفاظ-1]) < 0 ) {
	    negate_flag = true;
	    negate_الفاظ(ap, الفاظ);
      }
      إذا ( ((long) bp[الفاظ-1]) < 0 ) {
	    negate_flag ^= true;
	    negate_الفاظ(bp, الفاظ);
      }

      unsigned long*result = divide_bits(ap, bp, wid);
      إذا (result == 0) {
	    delete[]ap;
	    delete[]bp;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    أرجع true;
      }

      إذا (negate_flag) {
	    negate_الفاظ(result, الفاظ);
      }

      result[الفاظ-1] &= ~sign_mask;

      vala.setarray(0, wid, result);
      delete[]ap;
      delete[]bp;
      delete[]result;
      أرجع true;
}

منطقي of_DIV_WR(vthread_t thr, vvp_code_t)
{
      مزدوج r = thr->نکالو_real();
      مزدوج l = thr->نکالو_real();
      thr->ڈالو_real(l / r);

      أرجع true;
}

/*
 * %dup/obj
 * %dup/real
 * %dup/vec4
 *
 * Push a duplicate of the object on the appropriate stack.
 */
منطقي of_DUP_OBJ(vthread_t thr, vvp_code_t)
{
      vvp_object_t src = thr->peek_object();

        // If it is null push a new null object
      إذا (src.test_nil())
	    thr->ڈالو_object(vvp_object_t());
      وإلا
	    thr->ڈالو_object(src.duplicate());

      أرجع true;
}

منطقي of_DUP_REAL(vthread_t thr, vvp_code_t)
{
      thr->ڈالو_real(thr->peek_real(0));
      أرجع true;
}

منطقي of_DUP_VEC4(vthread_t thr, vvp_code_t)
{
      thr->ڈالو_vec4(thr->peek_vec4(0));
      أرجع true;
}

/*
 * This terminates the current thread. If there is a parent who is
 * waiting for me to die, then I schedule it. At any rate, I mark
 * myself as a zombie by setting my pc to 0.
 */
منطقي of_END(vthread_t thr, vvp_code_t)
{
      أكد(! thr->waiting_for_event);
      thr->i_have_ended = 1;
      thr->pc = codespace_null();

	/* Fully detach any detached children. */
      طالما (! thr->detached_children.empty()) {
	    vthread_t child = *(thr->detached_children.begin());
	    أكد(child);
	    أكد(child->parent == thr);
	    أكد(child->i_am_detached);
	    child->parent = 0;
	    child->i_am_detached = 0;
	    thr->detached_children.erase(thr->detached_children.begin());
      }

	/* It is an error to still have active children running at this
	 * point in time. They should have all been detached or joined. */
      أكد(thr->children.empty());

	/* If I have a parent who is waiting for me, then mark that I
	   have ended, and schedule that parent. Also, finish the
	   %join for the parent. */
      إذا (!thr->i_am_detached && thr->parent && thr->parent->i_am_joining) {
	    vthread_t tmp = thr->parent;
	    أكد(! thr->i_am_detached);

	    tmp->i_am_joining = 0;
	    schedule_vthread(tmp, 0, true);
	    do_join(tmp, thr);
	    أرجع خطأ;
      }

	/* If this thread is not fully detached then remove it from the
	 * parents detached_children set and reap it. */
      إذا (thr->i_am_detached) {
	    vthread_t tmp = thr->parent;
	    أكد(tmp);
	    size_t res = tmp->detached_children.erase(thr);
	    أكد(res == 1);
	      /* If the parent is waiting for the detached children to
	       * finish then the last detached child needs to tell the
	       * parent to wake up when it is finished. */
	    إذا (tmp->i_am_waiting && tmp->detached_children.empty()) {
		  tmp->i_am_waiting = 0;
		  schedule_vthread(tmp, 0, true);
	    }
	      /* Fully detach this thread so it will be reaped below. */
	    thr->i_am_detached = 0;
	    thr->parent = 0;
      }

	/* If I have no parent, then no one can %join me and there is
	 * no reason to stick around. This can happen, for example if
	 * I am an ``initial'' thread. */
      إذا (thr->parent == 0) {
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
منطقي of_EVENT(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_vector4_t tmp (1, BIT4_X);
      vvp_send_vec4(ptr, tmp, thr->wt_context);
      أرجع true;
}

/*
 * %event/nb <var-label>, <delay>
 */
منطقي of_EVENT_NB(vthread_t thr, vvp_code_t cp)
{
      vvp_time64_t delay;

      delay = thr->الفاظ[cp->bit_idx[0]].w_uint;
      schedule_propagate_event(cp->net, delay);
      أرجع true;
}

منطقي of_EVCTL(vthread_t thr, vvp_code_t cp)
{
      أكد(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      thr->ecount = thr->الفاظ[cp->bit_idx[0]].w_uint;
      أرجع true;
}
منطقي of_EVCTLC(vthread_t thr, vvp_code_t)
{
      thr->event = 0;
      thr->ecount = 0;
      أرجع true;
}

منطقي of_EVCTLI(vthread_t thr, vvp_code_t cp)
{
      أكد(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      thr->ecount = cp->bit_idx[0];
      أرجع true;
}

منطقي of_EVCTLS(vthread_t thr, vvp_code_t cp)
{
      أكد(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      int64_t val = thr->الفاظ[cp->bit_idx[0]].w_int;
      إذا (val < 0) val = 0;
      thr->ecount = val;
      أرجع true;
}

منطقي of_FLAG_GET_VEC4(vthread_t thr, vvp_code_t cp)
{
      صحيح flag = cp->نمبر;
      أكد(flag < vthread_s::FLAGS_گنتی);

      vvp_vector4_t val (1, thr->flags[flag]);
      thr->ڈالو_vec4(val);

      أرجع true;
}

/*
 * %flag_inv <flag1>
 */
منطقي of_FLAG_INV(vthread_t thr, vvp_code_t cp)
{
      صحيح flag1 = cp->bit_idx[0];

      thr->flags[flag1] = ~ thr->flags[flag1];
      أرجع true;
}

/*
 * %flag_mov <flag1>, <flag2>
 */
منطقي of_FLAG_MOV(vthread_t thr, vvp_code_t cp)
{
      صحيح flag1 = cp->bit_idx[0];
      صحيح flag2 = cp->bit_idx[1];

      thr->flags[flag1] = thr->flags[flag2];
      أرجع true;
}

/*
 * %flag_or <flag1>, <flag2>
 */
منطقي of_FLAG_OR(vthread_t thr, vvp_code_t cp)
{
      صحيح flag1 = cp->bit_idx[0];
      صحيح flag2 = cp->bit_idx[1];

      thr->flags[flag1] = thr->flags[flag1] | thr->flags[flag2];
      أرجع true;
}

منطقي of_FLAG_SET_IMM(vthread_t thr, vvp_code_t cp)
{
      صحيح flag = cp->نمبر;
      صحيح vali = cp->bit_idx[0];

      أكد(flag < vthread_s::FLAGS_گنتی);
      أكد(vali >= 0 && vali < 4);

      ساكن ثابت vvp_bit4_t map_bit[4] = {BIT4_0, BIT4_1, BIT4_Z, BIT4_X};
      thr->flags[flag] = map_bit[vali];
      أرجع true;
}

منطقي of_FLAG_SET_VEC4(vthread_t thr, vvp_code_t cp)
{
      صحيح flag = cp->نمبر;
      أكد(flag < vthread_s::FLAGS_گنتی);

      ثابت vvp_vector4_t&val = thr->peek_vec4();
      thr->flags[flag] = val.value(0);
      thr->نکالو_vec4(1);

      أرجع true;
}

/*
 * the %force/link instruction connects a source node to a
 * destination node. The destination node must be a signal, as it is
 * marked with the source of the force so that it may later be
 * unlinked without specifically knowing the source that this
 * instruction used.
 */
منطقي of_FORCE_LINK(vthread_t, vvp_code_t cp)
{
      vvp_net_t*dst = cp->net;
      vvp_net_t*src = cp->net2;

      أكد(dst->fil);
      dst->fil->force_link(dst, src);

      أرجع true;
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
منطقي of_FORCE_VEC4(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_vector4_t value = thr->نکالو_vec4();

	/* Send the force value to the filter on the node. */

      أكد(net->fil);
      إذا (value.size() != net->fil->filter_size())
	    value = coerce_to_width(value, net->fil->filter_size());

      net->force_vec4(value, vvp_vector2_t(vvp_vector2_t::FILL1, net->fil->filter_size()));

      أرجع true;
}

/*
 * %force/vec4/off <net>, <off>
 */
منطقي of_FORCE_VEC4_OFF(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      unsigned base_idx = cp->bit_idx[0];
      long base = thr->الفاظ[base_idx].w_int;
      vvp_vector4_t value = thr->نکالو_vec4();
      unsigned wid = value.size();

      أكد(net->fil);

      إذا (thr->flags[4] == BIT4_1)
	    أرجع true;

	// This is the width of the target vector.
      unsigned use_size = net->fil->filter_size();

      إذا (base >= (long)use_size)
	    أرجع true;
      إذا (base < -(long)use_size)
	    أرجع true;

      إذا ((base + wid) > use_size)
	    wid = use_size - base;

	// Make a mask of which bits are to be forced, 0 for unforced
	// bits and 1 for forced bits.
      vvp_vector2_t mask (vvp_vector2_t::FILL0, use_size);
      لكل (unsigned idx = 0 ; idx < wid ; idx += 1)
	    mask.set_bit(base+idx, 1);

      vvp_vector4_t tmp (use_size, BIT4_Z);

	// vvp_net_t::force_vec4 propagates all the bits of the
	// forced vector value, regardless of the mask. This
	// ensures the unforced bits retain their current value.
      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*>(net->fil);
      أكد(sig);
      sig->vec4_قدر(tmp);

      tmp.set_vec(base, value);

      net->force_vec4(tmp, mask);
      أرجع true;
}

/*
 * %force/vec4/off/d <net>, <off>, <del>
 */
منطقي of_FORCE_VEC4_OFF_D(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      unsigned base_idx = cp->bit_idx[0];
      long base = thr->الفاظ[base_idx].w_int;

      unsigned delay_idx = cp->bit_idx[1];
      vvp_time64_t delay = thr->الفاظ[delay_idx].w_uint;

      vvp_vector4_t value = thr->نکالو_vec4();

      أكد(net->fil);

      إذا (thr->flags[4] == BIT4_1)
	    أرجع true;

	// This is the width of the target vector.
      unsigned use_size = net->fil->filter_size();

      إذا (base >= (long)use_size)
	    أرجع true;
      إذا (base < -(long)use_size)
	    أرجع true;

      schedule_force_vector(net, base, use_size, value, delay);
      أرجع true;
}

منطقي of_FORCE_WR(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net  = cp->net;
      مزدوج value = thr->نکالو_real();

      net->force_real(value, vvp_vector2_t(vvp_vector2_t::FILL1, 1));

      أرجع true;
}

/*
 * The %fork instruction causes a new child to be created and pushed
 * in front of any existing child. This causes the new child to be
 * added to the list of children, and for me to be the parent of the
 * new child.
 */
منطقي of_FORK(vthread_t thr, vvp_code_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);

      إذا (cp->scope->is_automatic()) {
              /* The context allocated for this child is the top entry
                 on the write context stack. */
            child->wt_context = thr->wt_context;
            child->rd_context = thr->wt_context;
      }

      child->parent = thr;
      thr->children.insert(child);

      إذا (thr->i_am_in_function) {
	    child->is_scheduled = 1;
	    child->i_am_in_function = 1;
	    vthread_run(child);
	    running_thread = thr;
      } وإلا {
	    schedule_vthread(child, 0, true);
      }
      أرجع true;
}

منطقي of_FREE(vthread_t thr, vvp_code_t cp)
{
        /* Pop the child context from the read context stack. */
      vvp_context_t child_context = thr->rd_context;
      thr->rd_context = vvp_get_stacked_context(child_context);

        /* Free the context. */
      vthread_free_context(child_context, cp->scope);

      أرجع true;
}

/*
 * %inv
 *
 * Logically, this pops a value, inverts is (Verilog style, with Z and
 * X converted to X) and pushes the result. We can more efficiently
 * just to the invert in place.
 */
منطقي of_INV(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val.invert();
      أرجع true;
}


/*
 * Index registers, arithmetic.
 */

ساكن مضمن int64_t get_as_64_bit(uint32_t کم_32, uint32_t بلند_32)
{
      int64_t کم = کم_32;
      int64_t res = بلند_32;

      res <<= 32;
      res |= کم;
      أرجع res;
}

منطقي of_IX_ADD(vthread_t thr, vvp_code_t cp)
{
      thr->الفاظ[cp->نمبر].w_int += get_as_64_bit(cp->bit_idx[0],
                                                    cp->bit_idx[1]);
      أرجع true;
}

منطقي of_IX_SUB(vthread_t thr, vvp_code_t cp)
{
      thr->الفاظ[cp->نمبر].w_int -= get_as_64_bit(cp->bit_idx[0],
                                                    cp->bit_idx[1]);
      أرجع true;
}

منطقي of_IX_MUL(vthread_t thr, vvp_code_t cp)
{
      thr->الفاظ[cp->نمبر].w_int *= get_as_64_bit(cp->bit_idx[0],
                                                    cp->bit_idx[1]);
      أرجع true;
}

منطقي of_IX_LOAD(vthread_t thr, vvp_code_t cp)
{
      thr->الفاظ[cp->نمبر].w_int = get_as_64_bit(cp->bit_idx[0],
                                                   cp->bit_idx[1]);
      أرجع true;
}

منطقي of_IX_MOV(vthread_t thr, vvp_code_t cp)
{
      thr->الفاظ[cp->bit_idx[0]].w_int = thr->الفاظ[cp->bit_idx[1]].w_int;
      أرجع true;
}

منطقي of_IX_GETV(vthread_t thr, vvp_code_t cp)
{
      unsigned index = cp->bit_idx[0];
      vvp_net_t*net = cp->net;

      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*>(net->fil);
      إذا (sig == 0) {
	    أكد(net->fil);
	    cerr << thr->get_fileline()
	         << "%%ix/getv error: Net arg not a vector signal? "
		 << typeid(*net->fil).name() << endl;
      }
      أكد(sig);

      vvp_vector4_t vec;
      sig->vec4_قدر(vec);
      منطقي overflow_flag;
      uint64_t val = 0;
      منطقي known_flag = vector4_to_قدر(vec, overflow_flag, val);

      إذا (known_flag)
	    thr->الفاظ[index].w_uint = val;
      وإلا
	    thr->الفاظ[index].w_uint = 0;

	/* Set bit 4 as a flag if the input is unknown. */
      thr->flags[4] = known_flag ? (overflow_flag ? BIT4_X : BIT4_0) : BIT4_1;

      أرجع true;
}

منطقي of_IX_GETV_S(vthread_t thr, vvp_code_t cp)
{
      unsigned index = cp->bit_idx[0];
      vvp_net_t*net = cp->net;

      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*>(net->fil);
      إذا (sig == 0) {
	    أكد(net->fil);
	    cerr << thr->get_fileline()
	         << "%%ix/getv/s error: Net arg not a vector signal? "
		 << "fun=" << typeid(*net->fil).name()
		 << ", fil=" << (net->fil? typeid(*net->fil).name() : "<>")
		 << endl;
      }
      أكد(sig);

      vvp_vector4_t vec;
      sig->vec4_قدر(vec);
      int64_t val;
      منطقي known_flag = vector4_to_قدر(vec, val, true, true);

      إذا (known_flag)
	    thr->الفاظ[index].w_int = val;
      وإلا
	    thr->الفاظ[index].w_int = 0;

	/* Set bit 4 as a flag if the input is unknown. */
      thr->flags[4] = known_flag? BIT4_0 : BIT4_1;

      أرجع true;
}

ساكن uint64_t vec4_to_index(vthread_t thr, منطقي signed_flag)
{
	// Get all the information we need about the vec4 vector, then
	// pop it away. We only need the bool bits and the length.
      ثابت vvp_vector4_t&val = thr->peek_vec4();
      unsigned val_size = val.size();
      unsigned long*bits = val.subarray(0, val_size, خطأ);
      thr->نکالو_vec4(1);

	// If there are X/Z bits, then the subarray will give us a nil
	// pointer. Set a flag to indicate the error, and give up.
      إذا (bits == 0) {
	    thr->flags[4] = BIT4_1;
	    أرجع 0;
      }

      uint64_t v = 0;
      thr->flags[4] = BIT4_0;

      أكد(حجم(bits[0]) <= حجم(v));

      v = 0;
      لكل (unsigned idx = 0 ; idx < val_size ; idx += 8*حجم(bits[0])) {
	    uint64_t tmp = bits[idx/8/حجم(bits[0])];
	    إذا (idx < 8*حجم(v)) {
		  v |= tmp << idx;
	    } وإلا {
		  منطقي overflow = signed_flag && (v >> 63) ? ~tmp != 0 : tmp != 0;
		  إذا (overflow) {
			thr->flags[4] = BIT4_X;
			اكسر;
		  }
	    }
      }

	// Set the high bits that are not necessarily filled in by the
	// subarray function.
      إذا (val_size < 8*حجم(v)) {
	    إذا (signed_flag && (v & (static_cast<uint64_t>(1)<<(val_size-1)))) {
		    // Propagate the sign bit...
		  v |= (~static_cast<uint64_t>(0)) << val_size;

	    } وإلا {
		    // Fill with zeros.
		  v &= ~((~static_cast<uint64_t>(0)) << val_size);
	    }

      }

      delete[]bits;
      أرجع v;
}

/*
 * %ix/vec4 <idx>
 */
منطقي of_IX_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned use_idx = cp->نمبر;
      thr->الفاظ[use_idx].w_uint = vec4_to_index(thr, خطأ);
      أرجع true;
}

/*
 * %ix/vec4/s <idx>
 */
منطقي of_IX_VEC4_S(vthread_t thr, vvp_code_t cp)
{
      unsigned use_idx = cp->نمبر;
      thr->الفاظ[use_idx].w_uint = vec4_to_index(thr, true);
      أرجع true;
}

/*
 * The various JMP instruction work simply by pulling the new program
 * counter from the instruction and resuming. If the jump is
 * conditional, then test the bit for the expected value first.
 */
منطقي of_JMP(vthread_t thr, vvp_code_t cp)
{
      thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      إذا (schedule_stopped()) {
	    schedule_vthread(thr, 0, خطأ);
	    أرجع خطأ;
      }

      أرجع true;
}

/*
 * %jmp/0 <pc>, <flag>
 */
منطقي of_JMP0(vthread_t thr, vvp_code_t cp)
{
      إذا (thr->flags[cp->bit_idx[0]] == BIT4_0)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      إذا (schedule_stopped()) {
	    schedule_vthread(thr, 0, خطأ);
	    أرجع خطأ;
      }

      أرجع true;
}

/*
 * %jmp/0xz <pc>, <flag>
 */
منطقي of_JMP0XZ(vthread_t thr, vvp_code_t cp)
{
      إذا (thr->flags[cp->bit_idx[0]] != BIT4_1)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      إذا (schedule_stopped()) {
	    schedule_vthread(thr, 0, خطأ);
	    أرجع خطأ;
      }

      أرجع true;
}

/*
 * %jmp/1 <pc>, <flag>
 */
منطقي of_JMP1(vthread_t thr, vvp_code_t cp)
{
      إذا (thr->flags[cp->bit_idx[0]] == BIT4_1)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      إذا (schedule_stopped()) {
	    schedule_vthread(thr, 0, خطأ);
	    أرجع خطأ;
      }

      أرجع true;
}

/*
 * %jmp/1xz <pc>, <flag>
 */
منطقي of_JMP1XZ(vthread_t thr, vvp_code_t cp)
{
      إذا (thr->flags[cp->bit_idx[0]] != BIT4_0)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      إذا (schedule_stopped()) {
	    schedule_vthread(thr, 0, خطأ);
	    أرجع خطأ;
      }

      أرجع true;
}

/*
 * The %join instruction causes the thread to wait for one child
 * to die.  If a child is already dead (and a zombie) then I reap
 * it and go on. Otherwise, I mark myself as waiting in a join so that
 * children know to wake me when they finish.
 */

ساكن فراغ do_join(vthread_t thr, vthread_t child)
{
      أكد(child->parent == thr);

        /* If the immediate child thread is in an automatic scope... */
      إذا (child->wt_context) {
              /* and is the top level task/function thread... */
            إذا (thr->wt_context != thr->rd_context) {
                    /* Pop the child context from the write context stack. */
                  vvp_context_t child_context = thr->wt_context;
                  thr->wt_context = vvp_get_stacked_context(child_context);

                    /* Push the child context onto the read context stack */
                  vvp_set_stacked_context(child_context, thr->rd_context);
                  thr->rd_context = child_context;
            }
      }

      vthread_reap(child);
}

ساكن منطقي do_join_opcode(vthread_t thr)
{
      أكد( !thr->i_am_joining );
      أكد( !thr->children.empty());

	// Are there any children that have already ended? If so, then
	// join with that one.
      لكل (set<vthread_t>::iterator cur = thr->children.begin()
		 ; cur != thr->children.end() ; ++cur) {
	    vthread_t curp = *cur;
	    إذا (! curp->i_have_ended)
		  استمر;

	      // found something!
	    do_join(thr, curp);
	    أرجع true;
      }

	// Otherwise, tell my children to awaken me when they end,
	// then pause.
      thr->i_am_joining = 1;
      أرجع خطأ;
}

منطقي of_JOIN(vthread_t thr, vvp_code_t)
{
      أرجع do_join_opcode(thr);
}

/*
 * This %join/detach <n> instruction causes the thread to detach
 * threads that were created by an earlier %fork.
 */
منطقي of_JOIN_DETACH(vthread_t thr, vvp_code_t cp)
{
      unsigned long گنتی = cp->نمبر;

      أكد(گنتی == thr->children.size());

      طالما (! thr->children.empty()) {
	    vthread_t child = *thr->children.begin();
	    أكد(child->parent == thr);

	      // We cannot detach automatic tasks/functions within an
	      // automatic scope. If we try to do that, we might make
	      // a mess of the allocation of the context. Note that it
	      // is OK if the child context is distinct (See %exec_ufunc.)
	    أكد(child->wt_context==0 || thr->wt_context!=child->wt_context);
	    إذا (child->i_have_ended) {
		    // If the child has already ended, then reap it.
		  vthread_reap(child);

	    } وإلا {
		  size_t res = child->parent->children.erase(child);
		  أكد(res == 1);
		  child->i_am_detached = 1;
		  thr->detached_children.insert(child);
	    }
      }

      أرجع true;
}

/*
 * %load/ar <array-label>, <index>;
*/
منطقي of_LOAD_AR(vthread_t thr, vvp_code_t cp)
{
      unsigned idx = cp->bit_idx[0];
      مزدوج word;

	/* The result is 0.0 if the address is undefined. */
      إذا (thr->flags[4] == BIT4_1) {
	    word = 0.0;
      } وإلا {
	    unsigned adr = thr->الفاظ[idx].w_int;
	    word = cp->array->get_word_r(adr);
      }

      thr->ڈالو_real(word);
      أرجع true;
}

template <typename ELEM>
ساكن منطقي load_dar(vthread_t thr, vvp_code_t cp)
{
      int64_t adr = thr->الفاظ[3].w_int;
      vvp_net_t*net = cp->net;
      أكد(net);

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      أكد(obj);

      vvp_darray*darray = obj->get_object().peek<vvp_darray>();

      ELEM word;
      إذا (darray &&
          (adr >= 0) && (thr->flags[4] == BIT4_0)) // A defined address >= 0
	    darray->get_word(adr, word);
      وإلا
	    dq_default(word, obj->size());

      vthread_ڈالو(thr, word);
      أرجع true;
}

/*
 * %load/dar/r <array-label>;
 */
منطقي of_LOAD_DAR_R(vthread_t thr, vvp_code_t cp)
{
      أرجع load_dar<مزدوج>(thr, cp);
}

/*
 * %load/dar/str <array-label>;
 */
منطقي of_LOAD_DAR_STR(vthread_t thr, vvp_code_t cp)
{
      أرجع load_dar<string>(thr, cp);
}

/*
 * %load/dar/vec4 <array-label>;
 */
منطقي of_LOAD_DAR_VEC4(vthread_t thr, vvp_code_t cp)
{
      أرجع load_dar<vvp_vector4_t>(thr, cp);
}

/*
 * %load/prop/dar/vec4 <pid>, <wid>;
 *   Indexed read of queue/dynamic-array class property; object on stack,
 *   index in words[3] (same as %load/dar/vec4).
 */
منطقي of_LOAD_PROP_DAR_VEC4(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->نمبر;
      unsigned wid = cp->bit_idx[0];
      int64_t adr = thr->الفاظ[3].w_int;

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t pobj;
      cobj->get_object(pid, pobj, 0);
      vvp_darray*darray = pobj.peek<vvp_darray>();

      vvp_vector4_t word;
      إذا (darray && (adr >= 0) && (thr->flags[4] == BIT4_0)) {
	    darray->get_word(adr, word);
      } وإلا {
	    dq_default(word, wid);
      }

      thr->ڈالو_vec4(word);
      أرجع true;
}

/*
 * %load/obj <var-label>
 */
منطقي of_LOAD_OBJ(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      أكد(fun);

      vvp_object_t val = fun->get_object();
      thr->ڈالو_object(val);

      أرجع true;
}

/*
 * %load/obja <index>
 *    Loads the object from array, using index <index> as the index
 *    value. If flags[4] == 1, the calculation of <index> may have
 *    failed, so push nil.
 */
منطقي of_LOAD_OBJA(vthread_t thr, vvp_code_t cp)
{
      unsigned idx = cp->bit_idx[0];
      vvp_object_t word;

	/* The result is 0.0 if the address is undefined. */
      إذا (thr->flags[4] == BIT4_1) {
	    ; // Return nil
      } وإلا {
	    unsigned adr = thr->الفاظ[idx].w_int;
	    cp->array->get_word_obj(adr, word);
      }

      thr->ڈالو_object(word);
      أرجع true;
}

/*
 * %load/real <var-label>
 */
منطقي of_LOAD_REAL(vthread_t thr, vvp_code_t cp)
{
      __vpiHandle*tmp = cp->handle;
      t_vpi_قدر val;

      val.format = vpiRealVal;
      vpi_get_قدر(tmp, &val);

      thr->ڈالو_real(val.value.real);

      أرجع true;
}

/*
 * %load/str <var-label>
 */
منطقي of_LOAD_STR(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;


      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (net->fun);
      أكد(fun);

      ثابت string&val = fun->get_string();
      thr->ڈالو_str(val);

      أرجع true;
}

منطقي of_LOAD_STRA(vthread_t thr, vvp_code_t cp)
{
      unsigned idx = cp->bit_idx[0];
      string word;

      إذا (thr->flags[4] == BIT4_1) {
	    word = "";
      } وإلا {
	    unsigned adr = thr->الفاظ[idx].w_int;
	    word = cp->array->get_word_str(adr);
      }

      thr->ڈالو_str(word);
      أرجع true;
}


/*
 * %load/vec4 <net>
 */
منطقي of_LOAD_VEC4(vthread_t thr, vvp_code_t cp)
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
	         << endl;
	    أكد(sig);
	    أرجع true;
      }

	// Extract the value from the signal and directly into the
	// target stack position.
      sig->vec4_قدر(sig_قدر);

      أرجع true;
}

/*
 * %load/vec4a <arr>, <adrx>
 */
منطقي of_LOAD_VEC4A(vthread_t thr, vvp_code_t cp)
{
      صحيح adr_index = cp->bit_idx[0];

      long adr = thr->الفاظ[adr_index].w_int;

	// If flag[3] is set, then the calculation of the address
	// failed, and this load should return X instead of the actual
	// value.
      إذا (thr->flags[4] == BIT4_1) {
	    vvp_vector4_t tmp (cp->array->get_word_size(), BIT4_X);
	    thr->ڈالو_vec4(tmp);
	    أرجع true;
      }

      vvp_vector4_t tmp (cp->array->get_word(adr));
      thr->ڈالو_vec4(tmp);
      أرجع true;
}

ساكن فراغ do_verylong_mod(vvp_vector4_t&vala, ثابت vvp_vector4_t&valb,
			    منطقي بایاں_is_neg, منطقي دایاں_is_neg)
{
      منطقي out_is_neg = بایاں_is_neg;
      ثابت صحيح len=vala.size();
      unsigned حرف *a, *z, *t;
      a = جديد unsigned حرف[len+1];
      z = جديد unsigned حرف[len+1];
      t = جديد unsigned حرف[len+1];

      unsigned حرف carry;
      unsigned حرف temp;

      صحيح mxa = -1, mxz = -1;
      صحيح i;
      صحيح current, copylen;

      unsigned lb_carry = بایاں_is_neg? 1 : 0;
      unsigned rb_carry = دایاں_is_neg? 1 : 0;
      لكل (صحيح idx = 0 ;  idx < len ;  idx += 1) {
	    unsigned lb = vala.value(idx);
	    unsigned rb = valb.value(idx);

	    إذا ((lb | rb) & 2) {
		  delete []t;
		  delete []z;
		  delete []a;
		  vvp_vector4_t tmp(len, BIT4_X);
		  vala = tmp;
		  أرجع;
	    }

	    إذا (بایاں_is_neg) {
		  lb = (1-lb) + lb_carry;
		  lb_carry = (lb & ~1)? 1 : 0;
		  lb &= 1;
	    }
	    إذا (دایاں_is_neg) {
		  rb = (1-rb) + rb_carry;
		  rb_carry = (rb & ~1)? 1 : 0;
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
		  delete []t;
		  delete []z;
		  delete []a;
		  vvp_vector4_t tmpx (len, BIT4_X);
		  vala = tmpx;
		  أرجع;
	    }

	    اذهب tally;
      }

      copylen = mxa + 2;
      current = mxz - mxa;

      طالما(current > -1) {
	    carry = 1;
	    لكل(i=0;i<copylen;i++) {
		  temp = z[i+current] + a[i] + carry;
		  t[i] = (temp&1);
		  carry = (temp>>1);
	    }

	    إذا(carry) {
		  لكل(i=0;i<copylen;i++) {
			z[i+current] = t[i];
		  }
	    }

	    current--;
      }

 tally:

      vvp_vector4_t tmp (len, BIT4_X);
      carry = out_is_neg? 1 : 0;
      لكل (صحيح idx = 0 ;  idx < len ;  idx += 1) {
	    unsigned ob = z[idx];
	    إذا (out_is_neg) {
		  ob = (1-ob) + carry;
		  carry = (ob & ~1)? 1 : 0;
		  ob = ob & 1;
	    }
	    tmp.set_bit(idx, ob?BIT4_1:BIT4_0);
      }
      vala = tmp;
      delete []t;
      delete []z;
      delete []a;
}

منطقي of_MAX_WR(vthread_t thr, vvp_code_t)
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
      أرجع true;
}

منطقي of_MIN_WR(vthread_t thr, vvp_code_t)
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
      أرجع true;
}

منطقي of_MOD(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valb = thr->نکالو_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      أكد(vala.size()==valb.size());
      unsigned wid = vala.size();

      إذا(wid <= 8*حجم(unsigned long long)) {
	    unsigned long long lv = 0, rv = 0;

	    لكل (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		  unsigned long long lb = vala.value(idx);
		  unsigned long long rb = valb.value(idx);

		  إذا ((lb | rb) & 2)
			اذهب x_out;

		  lv |= (unsigned long long) lb << idx;
		  rv |= (unsigned long long) rb << idx;
	    }

	    إذا (rv == 0)
		  اذهب x_out;

	    lv %= rv;

	    لكل (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		  vala.set_bit(idx, (lv&1)?BIT4_1 : BIT4_0);
		  lv >>= 1;
	    }

	    أرجع true;

      } وإلا {
	    do_verylong_mod(vala, valb, خطأ, خطأ);
	    أرجع true;
      }

 x_out:
      vala = vvp_vector4_t(wid, BIT4_X);
      أرجع true;
}

/*
 * %mod/s
 */
منطقي of_MOD_S(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valb = thr->نکالو_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      أكد(vala.size()==valb.size());
      unsigned wid = vala.size();

	/* Handle the case that we can fit the bits into a long-long
	   variable. We cause use native % to do the work. */
      إذا(wid <= 8*حجم(long long)) {
	    long long lv = 0, rv = 0;

	    لكل (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		  long long lb = vala.value(idx);
		  long long rb = valb.value(idx);

		  إذا ((lb | rb) & 2)
			اذهب x_out;

		  lv |= (long long) lb << idx;
		  rv |= (long long) rb << idx;
	    }

	    إذا (rv == 0)
		  اذهب x_out;

	    إذا ((lv == LLONG_MIN) && (rv == -1))
		  اذهب zero_out;

	      /* Sign extend the signed operands when needed. */
	    إذا (wid < 8*حجم(long long)) {
		  إذا (lv & (1LL << (wid-1)))
			lv |= -1ULL << wid;
		  إذا (rv & (1LL << (wid-1)))
			rv |= -1ULL << wid;
	    }

	    lv %= rv;

	    لكل (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		  vala.set_bit(idx, (lv&1)? BIT4_1 : BIT4_0);
		  lv >>= 1;
	    }

	      // vala is the top of the stack, edited in place, so we
	      // do not need to push the result.

	    أرجع true;

      } وإلا {

	    منطقي بایاں_is_neg  = vala.value(vala.size()-1) == BIT4_1;
	    منطقي دایاں_is_neg = valb.value(valb.size()-1) == BIT4_1;
	    do_verylong_mod(vala, valb, بایاں_is_neg, دایاں_is_neg);
	    أرجع true;
      }

 x_out:
      vala = vvp_vector4_t(wid, BIT4_X);
      أرجع true;
 zero_out:
      vala = vvp_vector4_t(wid, BIT4_0);
      أرجع true;
}

/*
 * %mod/wr
 */
منطقي of_MOD_WR(vthread_t thr, vvp_code_t)
{
      مزدوج r = thr->نکالو_real();
      مزدوج l = thr->نکالو_real();
      thr->ڈالو_real(fmod(l,r));

      أرجع true;
}

/*
 * %neg/wr
 */
منطقي of_NEG_WR(vthread_t thr, vvp_code_t)
{
      thr->poke_real(0, -thr->peek_real(0));
      أرجع true;
}

/*
 * %pad/s <wid>
 */
منطقي of_PAD_S(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->نمبر;

      vvp_vector4_t&val = thr->peek_vec4();
      unsigned old_size = val.size();

	// Sign-extend.
      إذا (old_size < wid)
	    val.resize(wid, val.value(old_size-1));
      وإلا
	    val.resize(wid);

      أرجع true;
}

/*
 * %pad/u <wid>
 */
منطقي of_PAD_U(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->نمبر;

      vvp_vector4_t&val = thr->peek_vec4();
      val.resize(wid, BIT4_0);

      أرجع true;
}

/*
 * %part/s <wid>
 * %part/u <wid>
 * Two values are popped from the stack. First, pop the canonical
 * index of the part select, and second is the value to be
 * selected. The result is pushed back to the stack.
 */
ساكن منطقي of_PART_base(vthread_t thr, vvp_code_t cp, منطقي signed_flag)
{
      unsigned wid = cp->نمبر;

      vvp_vector4_t base4 = thr->نکالو_vec4();
      vvp_vector4_t&value = thr->peek_vec4();

      vvp_vector4_t res (wid, BIT4_X);

	// NOTE: This is treating the vector as signed. Is that correct?
      int32_t base;
      منطقي قدر_ok = vector4_to_قدر(base4, base, signed_flag);
      إذا (! قدر_ok) {
	    value = res;
	    أرجع true;
      }

      إذا (base >= (int32_t)value.size()) {
	    value = res;
	    أرجع true;
      }

      إذا ((base+(صحيح)wid) <= 0) {
	    value = res;
	    أرجع true;
      }

      long vbase = 0;
      إذا (base < 0) {
	    vbase = -base;
	    wid -= vbase;
	    base = 0;
      }

      إذا ((base+wid) > value.size()) {
	    wid = value.size() - base;
      }

      res .set_vec(vbase, value.subvalue(base, wid));
      value = res;

      أرجع true;
}

منطقي of_PART_S(vthread_t thr, vvp_code_t cp)
{
      أرجع of_PART_base(thr, cp, true);
}

منطقي of_PART_U(vthread_t thr, vvp_code_t cp)
{
      أرجع of_PART_base(thr, cp, خطأ);
}

/*
 * %parti/s <wid>, <basei>, <base_wid>
 * %parti/u <wid>, <basei>, <base_wid>
 *
 * Pop the value to be selected. The result is pushed back to the stack.
 */
ساكن منطقي of_PARTI_base(vthread_t thr, vvp_code_t cp, منطقي signed_flag)
{
      unsigned wid = cp->نمبر;
      uint32_t base = cp->bit_idx[0];
      uint32_t bwid = cp->bit_idx[1];

      vvp_vector4_t&value = thr->peek_vec4();

      vvp_vector4_t res (wid, BIT4_X);

	// NOTE: This is treating the vector as signed. Is that correct?
      int32_t use_base = base;
      إذا (signed_flag && bwid < 32 && (base&(1<<(bwid-1)))) {
	    use_base |= -1UL << bwid;
      }

      إذا (use_base >= (int32_t)value.size()) {
	    value = res;
	    أرجع true;
      }

      إذا ((use_base+(int32_t)wid) <= 0) {
	    value = res;
	    أرجع true;
      }

      long vbase = 0;
      إذا (use_base < 0) {
	    vbase = -use_base;
	    wid -= vbase;
	    use_base = 0;
      }

      إذا ((use_base+wid) > value.size()) {
	    wid = value.size() - use_base;
      }

      res .set_vec(vbase, value.subvalue(use_base, wid));
      value = res;

      أرجع true;
}

منطقي of_PARTI_S(vthread_t thr, vvp_code_t cp)
{
      أرجع of_PARTI_base(thr, cp, true);
}

منطقي of_PARTI_U(vthread_t thr, vvp_code_t cp)
{
      أرجع of_PARTI_base(thr, cp, خطأ);
}

/*
 * %mul
 */
منطقي of_MUL(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t r = thr->نکالو_vec4();
	// Rather then pop l, use it directly from the stack. When we
	// assign to 'l', that will edit the top of the stack, which
	// replaces a pop and a pull.
      vvp_vector4_t&l = thr->peek_vec4();

      l.mul(r);
      أرجع true;
}

/*
 * %muli <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
منطقي of_MULI(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->نمبر;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_immediate_rval (cp, r);

      l.mul(r);
      أرجع true;
}

منطقي of_MUL_WR(vthread_t thr, vvp_code_t)
{
      مزدوج r = thr->نکالو_real();
      مزدوج l = thr->نکالو_real();
      thr->ڈالو_real(l * r);

      أرجع true;
}

منطقي of_NAND(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valr = thr->نکالو_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      أكد(vall.size() == valr.size());

      vall &= valr;
      vall.invert();

      أرجع true;
}

/*
 * %new/cobj <vpi_object>
 * This creates a new cobject (SystemVerilog class object) and pushes
 * it to the stack. The <vpi-object> is a __vpiHandle that is a
 * vpiClassDefn object that defines the item to be created.
 */
منطقي of_NEW_COBJ(vthread_t thr, vvp_code_t cp)
{
      ثابت class_type*defn = dynamic_cast<ثابت class_type*> (cp->handle);
      أكد(defn);

      vvp_object_t tmp (جديد vvp_cobject(defn));
      thr->ڈالو_object(tmp);
      أرجع true;
}

منطقي of_NEW_DARRAY(vthread_t thr, vvp_code_t cp)
{
      ثابت حرف*text = cp->text;
      size_t size = thr->الفاظ[cp->bit_idx[0]].w_int;
      unsigned word_wid;
      size_t n;

      vvp_object_t obj;
      إذا (strcmp(text,"b8") == 0) {
	    obj = جديد vvp_darray_atom<uint8_t>(size);
      } وإلا إذا (strcmp(text,"b16") == 0) {
	    obj = جديد vvp_darray_atom<uint16_t>(size);
      } وإلا إذا (strcmp(text,"b32") == 0) {
	    obj = جديد vvp_darray_atom<uint32_t>(size);
      } وإلا إذا (strcmp(text,"b64") == 0) {
	    obj = جديد vvp_darray_atom<uint64_t>(size);
      } وإلا إذا (strcmp(text,"sb8") == 0) {
	    obj = جديد vvp_darray_atom<int8_t>(size);
      } وإلا إذا (strcmp(text,"sb16") == 0) {
	    obj = جديد vvp_darray_atom<int16_t>(size);
      } وإلا إذا (strcmp(text,"sb32") == 0) {
	    obj = جديد vvp_darray_atom<int32_t>(size);
      } وإلا إذا (strcmp(text,"sb64") == 0) {
	    obj = جديد vvp_darray_atom<int64_t>(size);
      } وإلا إذا ((1 == sscanf(text, "b%u%zn", &word_wid, &n)) &&
                 (n == strlen(text))) {
	    obj = جديد vvp_darray_vec2(size, word_wid);
      } وإلا إذا ((1 == sscanf(text, "sb%u%zn", &word_wid, &n)) &&
                 (n == strlen(text))) {
	    obj = جديد vvp_darray_vec2(size, word_wid);
      } وإلا إذا ((1 == sscanf(text, "v%u%zn", &word_wid, &n)) &&
                 (n == strlen(text))) {
	    obj = جديد vvp_darray_vec4(size, word_wid);
      } وإلا إذا ((1 == sscanf(text, "sv%u%zn", &word_wid, &n)) &&
                 (n == strlen(text))) {
	    obj = جديد vvp_darray_vec4(size, word_wid);
      } وإلا إذا (strcmp(text,"r") == 0) {
	    obj = جديد vvp_darray_real(size);
      } وإلا إذا (strcmp(text,"S") == 0) {
	    obj = جديد vvp_darray_string(size);
      } وإلا {
	    cerr << get_fileline()
	         << "Internal error: Unsupported dynamic array type: "
	         << text << "." << endl;
	    أكد(0);
      }

      thr->ڈالو_object(obj);

      أرجع true;
}

منطقي of_NOOP(vthread_t, vvp_code_t)
{
      أرجع true;
}

/*
 * %nor/r
 */
منطقي of_NORR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_or());

      أرجع true;
}

/*
 * Push a null to the object stack.
 */
منطقي of_NULL(vthread_t thr, vvp_code_t)
{
      vvp_object_t tmp;
      thr->ڈالو_object(tmp);
      أرجع true;
}

/*
 * %and/r
 */
منطقي of_ANDR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_and());

      أرجع true;
}

/*
 * %nand/r
 */
منطقي of_NANDR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_and());

      أرجع true;
}

/*
 * %or/r
 */
منطقي of_ORR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_or());

      أرجع true;
}

/*
 * %xor/r
 */
منطقي of_XORR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_xor());

      أرجع true;
}

/*
 * %xnor/r
 */
منطقي of_XNORR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_xor());

      أرجع true;
}

/*
 * %or
 */
منطقي of_OR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valb = thr->نکالو_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();
      vala |= valb;
      أرجع true;
}

/*
 * %nor
 */
منطقي of_NOR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valr = thr->نکالو_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      أكد(vall.size() == valr.size());

      vall |= valr;
      vall.invert();

      أرجع true;
}

/*
 * %pop/obj <num>, <skip>
 */
منطقي of_نکالو_OBJ(vthread_t thr, vvp_code_t cp)
{
      unsigned cnt = cp->bit_idx[0];
      unsigned skip = cp->bit_idx[1];

      thr->نکالو_object(cnt, skip);
      أرجع true;
}

/*
 * %pop/real <number>
 */
منطقي of_نکالو_REAL(vthread_t thr, vvp_code_t cp)
{
      unsigned cnt = cp->نمبر;
      thr->نکالو_real(cnt);
      أرجع true;
}

/*
 *  %pop/str <number>
 */
منطقي of_نکالو_STR(vthread_t thr, vvp_code_t cp)
{
      unsigned cnt = cp->نمبر;
      thr->نکالو_str(cnt);
      أرجع true;
}

/*
 *  %pop/vec4 <number>
 */
منطقي of_نکالو_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned cnt = cp->نمبر;
      thr->نکالو_vec4(cnt);
      أرجع true;
}

/*
 * %pow
 * %pow/s
 */
ساكن منطقي of_POW_base(vthread_t thr, منطقي signed_flag)
{
      vvp_vector4_t valb = thr->نکالو_vec4();
      vvp_vector4_t vala = thr->نکالو_vec4();

      unsigned wid = vala.size();

      vvp_vector2_t xv2 = vvp_vector2_t(vala, true);
      vvp_vector2_t yv2 = vvp_vector2_t(valb, true);


        /* If we have an X or Z in the arguments return X. */
      إذا (xv2.is_NaN() || yv2.is_NaN()) {
	    vvp_vector4_t tmp (wid, BIT4_X);
	    thr->ڈالو_vec4(tmp);
	    أرجع true;
      }

	// Is the exponent negative? If so, table 5-6 in IEEE1364-2005
	// defines what value is returned.
      إذا (signed_flag && yv2.value(yv2.size()-1)) {
	    صحيح a_val;
	    vvp_bit4_t pad = BIT4_0, lsb = BIT4_0;
	    إذا (vector2_to_قدر(xv2, a_val, true)) {
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
	    tmp.set_bit(0, lsb);
	    thr->ڈالو_vec4(tmp);
	    أرجع true;
      }

      vvp_vector2_t result = pow(xv2, yv2);

        /* Copy only what we need of the result. If the result is too
	   small, zero-pad it. */
      لكل (unsigned jdx = 0;  jdx < wid;  jdx += 1) {
	    إذا (jdx >= result.size())
		  vala.set_bit(jdx, BIT4_0);
	    وإلا
		  vala.set_bit(jdx, result.value(jdx) ? BIT4_1 : BIT4_0);
      }
      thr->ڈالو_vec4(vala);

      أرجع true;
}

منطقي of_POW(vthread_t thr, vvp_code_t)
{
      أرجع of_POW_base(thr, خطأ);
}

منطقي of_POW_S(vthread_t thr, vvp_code_t)
{
      أرجع of_POW_base(thr, true);
}

منطقي of_POW_WR(vthread_t thr, vvp_code_t)
{
      مزدوج r = thr->نکالو_real();
      مزدوج l = thr->نکالو_real();
      thr->ڈالو_real(pow(l,r));

      أرجع true;
}

/*
 * %prop/obj <pid>, <idx>
 *
 * Load an object value from the cobject and push it onto the object stack.
 */
منطقي of_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      unsigned pid = cp->نمبر;
      unsigned idx = cp->bit_idx[0];

      إذا (idx != 0) {
	    أكد(idx < vthread_s::الفاظ_گنتی);
	    idx = thr->الفاظ[idx].w_uint;
      }

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();

      vvp_object_t val;
      cobj->get_object(pid, val, idx);

      thr->ڈالو_object(val);

      أرجع true;
}

ساكن فراغ get_from_obj(unsigned pid, vvp_cobject*cobj, مزدوج&val)
{
      val = cobj->get_real(pid);
}

ساكن فراغ get_from_obj(unsigned pid, vvp_cobject*cobj, string&val)
{
      val = cobj->get_string(pid);
}

ساكن فراغ get_from_obj(unsigned pid, vvp_cobject*cobj, vvp_vector4_t&val)
{
      cobj->get_vec4(pid, val);
}

template <typename ELEM>
ساكن منطقي prop(vthread_t thr, vvp_code_t cp)
{
      unsigned pid = cp->نمبر;

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      أكد(cobj);

      ELEM val;
      get_from_obj(pid, cobj, val);
      vthread_ڈالو(thr, val);

      أرجع true;
}

/*
 * %prop/r <pid>
 *
 * Load a real value from the cobject and push it onto the real value
 * stack.
 */
منطقي of_PROP_R(vthread_t thr, vvp_code_t cp)
{
      أرجع prop<مزدوج>(thr, cp);
}

/*
 * %prop/str <pid>
 *
 * Load a string value from the cobject and push it onto the real value
 * stack.
 */
منطقي of_PROP_STR(vthread_t thr, vvp_code_t cp)
{
      أرجع prop<string>(thr, cp);
}

/*
 * %prop/v <pid>
 *
 * Load a property <pid> from the cobject on the top of the stack into
 * the vector space at <base>.
 */
منطقي of_PROP_V(vthread_t thr, vvp_code_t cp)
{
      أرجع prop<vvp_vector4_t>(thr, cp);
}

منطقي of_PUSHI_REAL(vthread_t thr, vvp_code_t cp)
{
      مزدوج mant = cp->bit_idx[0];
      uint32_t imant = cp->bit_idx[0];
      صحيح exp = cp->bit_idx[1];

	// Detect +infinity
      إذا (exp==0x3fff && imant==0) {
	    thr->ڈالو_real(INFINITY);
	    أرجع true;
      }
	// Detect -infinity
      إذا (exp==0x7fff && imant==0) {
	    thr->ڈالو_real(-INFINITY);
	    أرجع true;
      }
	// Detect NaN
      إذا (exp==0x3fff) {
	    thr->ڈالو_real(nan(""));
	    أرجع true;
      }

      مزدوج sign = (exp & 0x4000)? -1.0 : 1.0;

      exp &= 0x1fff;

      mant = sign * ldexp(mant, exp - 0x1000);
      thr->ڈالو_real(mant);
      أرجع true;
}

منطقي of_PUSHI_STR(vthread_t thr, vvp_code_t cp)
{
      ثابت حرف*text = cp->text;
      thr->ڈالو_str(filter_string(text));
      أرجع true;
}

/*
 * %pushi/vec4 <vala>, <valb>, <wid>
 */
منطقي of_PUSHI_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned wid  = cp->نمبر;

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t val (wid, BIT4_0);
      get_immediate_rval (cp, val);

      thr->ڈالو_vec4(val);

      أرجع true;
}

/*
 * %pushv/str
 *   Pops a vec4 value, and pushes a string.
 */
منطقي of_PUSHV_STR(vthread_t thr, vvp_code_t)
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

      أرجع true;
}

/*
 * %putc/str/vec4 <var>, <mux>
 */
منطقي of_PUTC_STR_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned muxr = cp->bit_idx[0];
      int32_t mux = muxr? thr->الفاظ[muxr].w_int : 0;

      vvp_vector4_t val = thr->نکالو_vec4();
      أكد(val.size() == 8);

      إذا (mux < 0)
	    أرجع true;

	/* Get the existing value of the string. If we find that the
	   index is too big for the string, then give up. */
      vvp_net_t*net = cp->net;
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (net->fun);
      أكد(fun);

      string tmp = fun->get_string();
      إذا (tmp.size() <= (size_t)mux)
	    أرجع true;

      حرف val_str = 0;
      لكل (size_t idx = 0 ; idx < 8 ; idx += 1) {
	    إذا (val.value(idx)==BIT4_1)
		  val_str |= 1<<idx;
      }

	// It is a quirk of the Verilog standard that putc(..., 'h00)
	// has no effect. Test for that case here.
      إذا (val_str == 0)
	    أرجع true;

      tmp[mux] = val_str;

      vvp_send_string(vvp_net_ptr_t(cp->net, 0), tmp, thr->wt_context);
      أرجع true;
}

template <typename ELEM, صنف QTYPE>
ساكن منطقي qinsert(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
      int64_t idx = thr->الفاظ[3].w_int;
      ELEM value;
      vvp_net_t*net = cp->net;
      نکالو_قدر(thr, value, wid); // Pop the value to store.

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      أكد(queue);
      إذا (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at a negative "
	         << get_queue_type(value)
	         << " index (" << idx << "). ";
	    print_queue_قدر(value);
	    cerr << " was not added." << endl;
      } وإلا إذا (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at an undefined "
	         << get_queue_type(value) << " index. ";
	    print_queue_قدر(value);
	    cerr << " was not added." << endl;
      } وإلا {
	    unsigned max_size = thr->الفاظ[cp->bit_idx[0]].w_int;
	    queue->insert(idx, value, max_size);
      }
      أرجع true;
}

/*
 * %qinsert/real <var-label>
 */
منطقي of_QINSERT_REAL(vthread_t thr, vvp_code_t cp)
{
      أرجع qinsert<مزدوج, vvp_queue_real>(thr, cp);
}

/*
 * %qinsert/str <var-label>
 */
منطقي of_QINSERT_STR(vthread_t thr, vvp_code_t cp)
{
      أرجع qinsert<string, vvp_queue_string>(thr, cp);
}

/*
 * %qinsert/v <var-label>
 */
منطقي of_QINSERT_V(vthread_t thr, vvp_code_t cp)
{
      أرجع qinsert<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

/*
 * Helper functions used in the queue pop templates
 */
مضمن فراغ ڈالو_قدر(vthread_t thr, مزدوج value, unsigned)
{
      thr->ڈالو_real(value);
}

مضمن فراغ ڈالو_قدر(vthread_t thr, ثابت string&value, unsigned)
{
      thr->ڈالو_str(value);
}

مضمن فراغ ڈالو_قدر(vthread_t thr, ثابت vvp_vector4_t&value, unsigned wid)
{
      أكد(wid == value.size());
      thr->ڈالو_vec4(value);
}

template <typename ELEM, صنف QTYPE>
ساكن منطقي q_نکالو(vthread_t thr, vvp_code_t cp,
                  فراغ (*get_val_func)(vvp_queue*, ELEM&),
                  ثابت حرف*loc, unsigned wid)
{
      vvp_net_t*net = cp->net;

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      أكد(queue);

      size_t size = queue->get_size();

      ELEM value;
      إذا (size) {
	    get_val_func(queue, value);
      } وإلا {
	    dq_default(value, wid);
	    cerr << thr->get_fileline()
	         << "Warning: pop_" << loc << "() on empty "
	         << get_queue_type(value) << "." << endl;
      }

      ڈالو_قدر(thr, value, wid);
      أرجع true;
}

template <typename ELEM>
ساكن فراغ get_back_قدر(vvp_queue*queue, ELEM&value)
{
      queue->get_word(queue->get_size()-1, value);
      queue->pop_back();
}

template <typename ELEM, صنف QTYPE>
ساكن منطقي qpop_b(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
      أرجع q_نکالو<ELEM, QTYPE>(thr, cp, get_back_قدر<ELEM>, "back", wid);
}

/*
 * %qpop/b/real <var-label>
 */
منطقي of_QPOP_B_REAL(vthread_t thr, vvp_code_t cp)
{
      أرجع qpop_b<مزدوج, vvp_queue_real>(thr, cp);
}

/*
 * %qpop/b/str <var-label>
 */
منطقي of_QPOP_B_STR(vthread_t thr, vvp_code_t cp)
{
      أرجع qpop_b<string, vvp_queue_string>(thr, cp);
}

/*
 * %qpop/b/v <var-label>
 */
منطقي of_QPOP_B_V(vthread_t thr, vvp_code_t cp)
{
      أرجع qpop_b<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[0]);
}

template <typename ELEM>
ساكن فراغ get_front_قدر(vvp_queue*queue, ELEM&value)
{
      queue->get_word(0, value);
      queue->pop_front();
}

template <typename ELEM, صنف QTYPE>
ساكن منطقي qpop_f(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
      أرجع q_نکالو<ELEM, QTYPE>(thr, cp, get_front_قدر<ELEM>, "front", wid);
}


/*
 * %qpop/f/real <var-label>
 */
منطقي of_QPOP_F_REAL(vthread_t thr, vvp_code_t cp)
{
      أرجع qpop_f<مزدوج, vvp_queue_real>(thr, cp);
}

/*
 * %qpop/f/str <var-label>
 */
منطقي of_QPOP_F_STR(vthread_t thr, vvp_code_t cp)
{
      أرجع qpop_f<string, vvp_queue_string>(thr, cp);
}

/*
 * %qpop/f/v <var-label>
 */
منطقي of_QPOP_F_V(vthread_t thr, vvp_code_t cp)
{
      أرجع qpop_f<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[0]);
}

/*
 * These implement the %release/net and %release/reg instructions. The
 * %release/net instruction applies to a net kind of functor by
 * sending the release/net command to the command port. (See vvp_net.h
 * for details.) The %release/reg instruction is the same, but sends
 * the release/reg command instead. These are very similar to the
 * %deassign instruction.
 */
ساكن منطقي do_release_vec(vvp_code_t cp, منطقي net_flag)
{
      vvp_net_t*net = cp->net;
      unsigned base  = cp->bit_idx[0];
      unsigned width = cp->bit_idx[1];

      أكد(net->fil);

      إذا (base >= net->fil->filter_size()) أرجع true;
      إذا (base+width > net->fil->filter_size())
	    width = net->fil->filter_size() - base;

      منطقي full_sig = base == 0 && width == net->fil->filter_size();

	// XXXX Can't really do this if this is a partial release?
      net->fil->force_unlink();

	/* Do we release all or part of the net? */
      vvp_net_ptr_t ptr (net, 0);
      إذا (full_sig) {
	    net->fil->release(ptr, net_flag);
      } وإلا {
	    net->fil->release_pv(ptr, base, width, net_flag);
      }
      net->fun->force_flag(خطأ);

      أرجع true;
}

منطقي of_RELEASE_NET(vthread_t, vvp_code_t cp)
{
      أرجع do_release_vec(cp, true);
}


منطقي of_RELEASE_REG(vthread_t, vvp_code_t cp)
{
      أرجع do_release_vec(cp, خطأ);
}

/* The type is 1 for registers and 0 for everything else. */
منطقي of_RELEASE_WR(vthread_t, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      unsigned type  = cp->bit_idx[0];

      أكد(net->fil);
      net->fil->force_unlink();

	// Send a command to this signal to unforce itself.
      vvp_net_ptr_t ptr (net, 0);
      net->fil->release(ptr, type==0);
      أرجع true;
}

منطقي of_REPLICATE(vthread_t thr, vvp_code_t cp)
{
      صحيح rept = cp->نمبر;
      vvp_vector4_t val = thr->نکالو_vec4();
      vvp_vector4_t res (val.size() * rept, BIT4_X);

      لكل (صحيح idx = 0 ; idx < rept ; idx += 1) {
	    res.set_vec(idx * val.size(), val);
      }

      thr->ڈالو_vec4(res);

      أرجع true;
}

ساكن فراغ poke_val(vthread_t fun_thr, unsigned depth, مزدوج val)
{
      fun_thr->parent->poke_real(depth, val);
}

ساكن فراغ poke_val(vthread_t fun_thr, unsigned depth, ثابت string&val)
{
      fun_thr->parent->poke_str(depth, val);
}

ساكن size_t get_max(vthread_t fun_thr, مزدوج&)
{
      أرجع fun_thr->args_real.size();
}

ساكن size_t get_max(vthread_t fun_thr, string&)
{
      أرجع fun_thr->args_str.size();
}

ساكن size_t get_max(vthread_t fun_thr, vvp_vector4_t&)
{
      أرجع fun_thr->args_vec4.size();
}

ساكن unsigned get_depth(vthread_t fun_thr, size_t index, مزدوج&)
{
      أرجع fun_thr->args_real[index];
}

ساكن unsigned get_depth(vthread_t fun_thr, size_t index, string&)
{
      أرجع fun_thr->args_str[index];
}

ساكن unsigned get_depth(vthread_t fun_thr, size_t index, vvp_vector4_t&)
{
      أرجع fun_thr->args_vec4[index];
}

ساكن vthread_t get_func(vthread_t thr)
{
      vthread_t fun_thr = thr;

      طالما (fun_thr->parent_scope->get_type_code() != vpiFunction) {
	    أكد(fun_thr->parent);
	    fun_thr = fun_thr->parent;
      }

      أرجع fun_thr;
}

template <typename ELEM>
ساكن منطقي ret(vthread_t thr, vvp_code_t cp)
{
      size_t index = cp->نمبر;
      ELEM val;
      نکالو_قدر(thr, val, 0);

      vthread_t fun_thr = get_func(thr);
      أكد(index < get_max(fun_thr, val));

      unsigned depth = get_depth(fun_thr, index, val);
	// Use the depth to put the value into the stack of
	// the parent thread.
      poke_val(fun_thr, depth, val);
      أرجع true;
}

/*
 * %ret/real <index>
 */
منطقي of_RET_REAL(vthread_t thr, vvp_code_t cp)
{
      أرجع ret<مزدوج>(thr, cp);
}

/*
 * %ret/str <index>
 */
منطقي of_RET_STR(vthread_t thr, vvp_code_t cp)
{
      أرجع ret<string>(thr, cp);
}

/*
 * %ret/vec4 <index>, <offset>, <wid>
 */
منطقي of_RET_VEC4(vthread_t thr, vvp_code_t cp)
{
      size_t index = cp->نمبر;
      unsigned off_index = cp->bit_idx[0];
      unsigned صحيح wid = cp->bit_idx[1];
      vvp_vector4_t&val = thr->peek_vec4();

      vthread_t fun_thr = get_func(thr);
      أكد(index < get_max(fun_thr, val));
      أكد(val.size() == wid);
      unsigned depth = get_depth(fun_thr, index, val);

      int64_t off = off_index ? thr->الفاظ[off_index].w_int : 0;
      unsigned صحيح sig_قدر_size = fun_thr->parent->peek_vec4(depth).size();

      إذا (off_index!=0 && thr->flags[4] == BIT4_1) {
	    thr->نکالو_vec4(1);
	    أرجع true;
      }

      إذا (!resize_rval_vec(val, off, sig_قدر_size)) {
	    thr->نکالو_vec4(1);
	    أرجع true;
      }

      إذا (off == 0 && val.size() == sig_قدر_size) {
	    fun_thr->parent->poke_vec4(depth, val);
      } وإلا {
	    vvp_vector4_t tmp_dst = fun_thr->parent->peek_vec4(depth);
	    tmp_dst.set_vec(off, val);
	    fun_thr->parent->poke_vec4(depth, tmp_dst);
      }

      thr->نکالو_vec4(1);
      أرجع true;
}

ساكن فراغ ڈالو_from_parent(vthread_t thr, vthread_t fun_thr, unsigned depth, مزدوج&)
{
      thr->ڈالو_real(fun_thr->parent->peek_real(depth));
}

ساكن فراغ ڈالو_from_parent(vthread_t thr, vthread_t fun_thr, unsigned depth, string&)
{
      thr->ڈالو_str(fun_thr->parent->peek_str(depth));
}

ساكن فراغ ڈالو_from_parent(vthread_t thr, vthread_t fun_thr, unsigned depth, vvp_vector4_t&)
{
      thr->ڈالو_vec4(fun_thr->parent->peek_vec4(depth));
}

template <typename ELEM>
ساكن منطقي retload(vthread_t thr, vvp_code_t cp)
{
      size_t index = cp->نمبر;
      ELEM type;

      vthread_t fun_thr = get_func(thr);
      أكد(index < get_max(fun_thr, type));

      unsigned depth = get_depth(fun_thr, index, type);
	// Use the depth to extract the values from the stack
	// of the parent thread.
      ڈالو_from_parent(thr, fun_thr, depth, type);
      أرجع true;
}

/*
 * %retload/real <index>
 */
منطقي of_RETLOAD_REAL(vthread_t thr, vvp_code_t cp)
{
      أرجع retload<مزدوج>(thr, cp);
}

/*
 * %retload/str <index>
 */
منطقي of_RETLOAD_STR(vthread_t thr, vvp_code_t cp)
{
      أرجع retload<string>(thr, cp);
}

/*
 * %retload/vec4 <index>
 */
منطقي of_RETLOAD_VEC4(vthread_t thr, vvp_code_t cp)
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
منطقي of_SCOPY(vthread_t thr, vvp_code_t)
{
      vvp_object_t tmp;
      thr->نکالو_object(tmp);

      vvp_object_t&dest = thr->peek_object();
        // If it is null there is nothing to copy
      إذا (!tmp.test_nil())
	    dest.shallow_copy(tmp);

      أرجع true;
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

template <typename ELEM>
ساكن منطقي set_dar_obj(vthread_t thr, vvp_code_t cp)
{
      unsigned adr = thr->الفاظ[cp->نمبر].w_int;

      ELEM value;
      thread_peek(thr, value);

      vvp_object_t&چوٹی = thr->peek_object();
      vvp_darray*darray = چوٹی.peek<vvp_darray>();
      أكد(darray);

      darray->set_word(adr, value);
      أرجع true;
}

/*
 * %set/dar/obj/real <index>
 */
منطقي of_SET_DAR_OBJ_REAL(vthread_t thr, vvp_code_t cp)
{
      أرجع set_dar_obj<مزدوج>(thr, cp);
}

/*
 * %set/dar/obj/str <index>
 */
منطقي of_SET_DAR_OBJ_STR(vthread_t thr, vvp_code_t cp)
{
      أرجع set_dar_obj<string>(thr, cp);
}

/*
 * %set/dar/obj/vec4 <index>
 */
منطقي of_SET_DAR_OBJ_VEC4(vthread_t thr, vvp_code_t cp)
{
      أرجع set_dar_obj<vvp_vector4_t>(thr, cp);
}

/*
 * %shiftl <idx>
 *
 * Pop the operand, then push the result.
 */
منطقي of_SHIFTL(vthread_t thr, vvp_code_t cp)
{
      صحيح use_index = cp->نمبر;
      uint64_t shift = thr->الفاظ[use_index].w_uint;

      vvp_vector4_t&val = thr->peek_vec4();
      unsigned wid  = val.size();

      إذا (thr->flags[4] == BIT4_1) {
	      // The result is 'bx if the shift amount is undefined
	    val = vvp_vector4_t(wid, BIT4_X);

      } وإلا إذا (thr->flags[4] == BIT4_X || shift >= wid) {
	      // Shift is so big that all value is shifted out. Write
	      // a constant 0 result.
	    val = vvp_vector4_t(wid, BIT4_0);

      } وإلا إذا (shift > 0) {
	    vvp_vector4_t blk = val.subvalue(0, wid-shift);
	    vvp_vector4_t tmp (shift, BIT4_0);
	    val.set_vec(0, tmp);
	    val.set_vec(shift, blk);
      }

      أرجع true;
}

/*
 * %shiftr <idx>
 * This is an unsigned right shift. The <idx> is a number that selects
 * the index register with the amount of the shift. This instruction
 * checks flag bit 4, which will be true if the shift is invalid.
 */
منطقي of_SHIFTR(vthread_t thr, vvp_code_t cp)
{
      صحيح use_index = cp->نمبر;
      uint64_t shift = thr->الفاظ[use_index].w_uint;

      vvp_vector4_t val = thr->نکالو_vec4();
      unsigned wid  = val.size();

      إذا (thr->flags[4] == BIT4_1) {
	    val = vvp_vector4_t(wid, BIT4_X);

      } وإلا إذا (thr->flags[4] == BIT4_X || shift > wid) {
	    val = vvp_vector4_t(wid, BIT4_0);

      } وإلا إذا (shift > 0) {
	    vvp_vector4_t blk = val.subvalue(shift, wid-shift);
	    vvp_vector4_t tmp (shift, BIT4_0);
	    val.set_vec(0, blk);
	    val.set_vec(wid-shift, tmp);
      }

      thr->ڈالو_vec4(val);
      أرجع true;
}

/*
 *  %shiftr/s <wid>
 */
منطقي of_SHIFTR_S(vthread_t thr, vvp_code_t cp)
{
      صحيح use_index = cp->نمبر;
      uint64_t shift = thr->الفاظ[use_index].w_uint;

      vvp_vector4_t val = thr->نکالو_vec4();
      unsigned wid  = val.size();

      vvp_bit4_t sign_bit = val.value(val.size()-1);

      إذا (thr->flags[4] == BIT4_1) {
	    val = vvp_vector4_t(wid, BIT4_X);

      } وإلا إذا (thr->flags[4] == BIT4_X || shift > wid) {
	    val = vvp_vector4_t(wid, sign_bit);

      } وإلا إذا (shift > 0) {
	    vvp_vector4_t blk = val.subvalue(shift, wid-shift);
	    vvp_vector4_t tmp (shift, sign_bit);
	    val.set_vec(0, blk);
	    val.set_vec(wid-shift, tmp);
      }

      thr->ڈالو_vec4(val);
      أرجع true;
}

/*
 * %split/vec4 <wid>
 *   Pop 1 value,
 *   Take <wid> bits from the lsb,
 *   Push the remaining msb,
 *   Push the lsb.
 */
منطقي of_SPLIT_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned lsb_wid = cp->نمبر;

      vvp_vector4_t&val = thr->peek_vec4();
      أكد(lsb_wid < val.size());

      vvp_vector4_t lsb = val.subvalue(0, lsb_wid);
      val = val.subvalue(lsb_wid, val.size()-lsb_wid);

      thr->ڈالو_vec4(lsb);
      أرجع true;
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

template <typename ELEM>
ساكن منطقي store_dar(vthread_t thr, vvp_code_t cp)
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
	         << " index (" << adr << ")." << endl;
      وإلا إذا (thr->flags[4] != BIT4_0)
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to an undefined " << get_darray_type(value)
	         << " index." << endl;
      وإلا إذا (darray)
	    darray->set_word(adr, value);
      وإلا
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to an undefined " << get_darray_type(value)
	         << "." << endl;

      أرجع true;
}

/*
 * %store/dar/real <var>
 */
منطقي of_STORE_DAR_R(vthread_t thr, vvp_code_t cp)
{
      أرجع store_dar<مزدوج>(thr, cp);
}

/*
 * %store/dar/str <var>
 */
منطقي of_STORE_DAR_STR(vthread_t thr, vvp_code_t cp)
{
      أرجع store_dar<string>(thr, cp);
}

/*
 * %store/dar/vec4 <var>
 */
منطقي of_STORE_DAR_VEC4(vthread_t thr, vvp_code_t cp)
{
      أرجع store_dar<vvp_vector4_t>(thr, cp);
}

منطقي of_STORE_OBJ(vthread_t thr, vvp_code_t cp)
{
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);

      vvp_object_t val;
      thr->نکالو_object(val);

      vvp_send_object(ptr, val, thr->wt_context);

      أرجع true;
}

/*
 * %store/obja <array-label> <index>
 */
منطقي of_STORE_OBJA(vthread_t thr, vvp_code_t cp)
{
      unsigned idx = cp->bit_idx[0];
      unsigned adr = thr->الفاظ[idx].w_int;

      vvp_object_t val;
      thr->نکالو_object(val);

      cp->array->set_word(adr, val);

      أرجع true;
}


/*
 * %store/prop/obj <pid>, <idx>
 *
 * Pop an object value from the object stack, and store the value into
 * the property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
منطقي of_STORE_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->نمبر;
      unsigned idx = cp->bit_idx[0];

      إذا (idx != 0) {
	    أكد(idx < vthread_s::الفاظ_گنتی);
	    idx = thr->الفاظ[idx].w_uint;
      }

      vvp_object_t val;
      thr->نکالو_object(val);

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      أكد(cobj);

      cobj->set_object(pid, val, idx);

      أرجع true;
}

ساكن فراغ نکالو_prop_val(vthread_t thr, مزدوج&val, unsigned)
{
      val = thr->نکالو_real();
}

ساكن فراغ نکالو_prop_val(vthread_t thr, string&val, unsigned)
{
      val = thr->نکالو_str();
}

ساكن فراغ نکالو_prop_val(vthread_t thr, vvp_vector4_t&val, unsigned wid)
{
      val = thr->نکالو_vec4();
      أكد(val.size() >= wid);
      val.resize(wid);
}

ساكن فراغ set_val(vvp_cobject*cobj, size_t pid, ثابت مزدوج&val)
{
      cobj->set_real(pid, val);
}

ساكن فراغ set_val(vvp_cobject*cobj, size_t pid, ثابت string&val)
{
      cobj->set_string(pid, val);
}

ساكن فراغ set_val(vvp_cobject*cobj, size_t pid, ثابت vvp_vector4_t&val)
{
      cobj->set_vec4(pid, val);
}

template <typename ELEM>
ساكن منطقي store_prop(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
      size_t pid = cp->نمبر;
      ELEM val;
      نکالو_prop_val(thr, val, wid); // Pop the value to store.

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      أكد(cobj);

      set_val(cobj, pid, val);

      أرجع true;
}

/*
 * %store/prop/r <id>
 *
 * Pop a real value from the real stack, and store the value into the
 * property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
منطقي of_STORE_PROP_R(vthread_t thr, vvp_code_t cp)
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
منطقي of_STORE_PROP_STR(vthread_t thr, vvp_code_t cp)
{
      أرجع store_prop<string>(thr, cp);
}

/*
 * %store/prop/v <pid>, <wid>
 *
 * Store vector value into property <id> of cobject in the top of the
 * stack. Do NOT pop the object stack.
 */
منطقي of_STORE_PROP_V(vthread_t thr, vvp_code_t cp)
{
      أرجع store_prop<vvp_vector4_t>(thr, cp, cp->bit_idx[0]);
}

template <صنف QTYPE>
ساكن QTYPE* get_queue_prop(vvp_cobject*cobj, size_t pid)
{
      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      QTYPE* queue = qobj.peek<QTYPE>();
      إذا (queue == 0) {
	    queue = جديد QTYPE;
	    vvp_object_t val(queue);
	    cobj->set_object(pid, val, 0);
      }
      أرجع queue;
}

template <typename ELEM, صنف QTYPE>
ساكن منطقي store_prop_qb(vthread_t thr, vvp_code_t cp, unsigned wid)
{
      size_t pid = cp->نمبر;
      ELEM value;
      unsigned max_size = thr->الفاظ[cp->bit_idx[0]].w_uint;
      نکالو_قدر(thr, value, wid);

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      QTYPE* queue = get_queue_prop<QTYPE>(cobj, pid);
      أكد(queue);
      queue->push_back(value, max_size);
      أرجع true;
}

template <typename ELEM, صنف QTYPE>
ساكن منطقي store_prop_qf(vthread_t thr, vvp_code_t cp, unsigned wid)
{
      size_t pid = cp->نمبر;
      ELEM value;
      unsigned max_size = thr->الفاظ[cp->bit_idx[0]].w_uint;
      نکالو_قدر(thr, value, wid);

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      QTYPE* queue = get_queue_prop<QTYPE>(cobj, pid);
      أكد(queue);
      queue->push_front(value, max_size);
      أرجع true;
}

منطقي of_STORE_PROP_QB_R(vthread_t thr, vvp_code_t cp)
{
      أرجع store_prop_qb<مزدوج, vvp_queue_real>(thr, cp, 0);
}

منطقي of_STORE_PROP_QB_STR(vthread_t thr, vvp_code_t cp)
{
      أرجع store_prop_qb<string, vvp_queue_string>(thr, cp, 0);
}

منطقي of_STORE_PROP_QB_V(vthread_t thr, vvp_code_t cp)
{
      أرجع store_prop_qb<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

منطقي of_STORE_PROP_QF_R(vthread_t thr, vvp_code_t cp)
{
      أرجع store_prop_qf<مزدوج, vvp_queue_real>(thr, cp, 0);
}

منطقي of_STORE_PROP_QF_STR(vthread_t thr, vvp_code_t cp)
{
      أرجع store_prop_qf<string, vvp_queue_string>(thr, cp, 0);
}

منطقي of_STORE_PROP_QF_V(vthread_t thr, vvp_code_t cp)
{
      أرجع store_prop_qf<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

template <typename ELEM, صنف QTYPE>
ساكن منطقي qinsert_prop(vthread_t thr, vvp_code_t cp, unsigned wid)
{
      int64_t idx = thr->الفاظ[3].w_int;
      ELEM value;
      size_t pid = cp->نمبر;
      نکالو_قدر(thr, value, wid);

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      QTYPE* queue = get_queue_prop<QTYPE>(cobj, pid);
      أكد(queue);
      إذا (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at a negative "
	         << get_queue_type(value)
	         << " index (" << idx << "). ";
	    print_queue_قدر(value);
	    cerr << " was not added." << endl;
      } وإلا إذا (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at an undefined "
	         << get_queue_type(value) << " index. ";
	    print_queue_قدر(value);
	    cerr << " was not added." << endl;
      } وإلا {
	    unsigned max_size = thr->الفاظ[cp->bit_idx[0]].w_int;
	    queue->insert(idx, value, max_size);
      }
      أرجع true;
}

منطقي of_QINSERT_PROP_R(vthread_t thr, vvp_code_t cp)
{
      أرجع qinsert_prop<مزدوج, vvp_queue_real>(thr, cp, 0);
}

منطقي of_QINSERT_PROP_STR(vthread_t thr, vvp_code_t cp)
{
      أرجع qinsert_prop<string, vvp_queue_string>(thr, cp, 0);
}

منطقي of_QINSERT_PROP_V(vthread_t thr, vvp_code_t cp)
{
      أرجع qinsert_prop<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

template <typename ELEM, صنف QTYPE>
ساكن منطقي q_نکالو_prop(vthread_t thr, vvp_code_t cp,
                       فراغ (*get_val_func)(vvp_queue*, ELEM&),
                       ثابت حرف*loc, unsigned wid)
{
      size_t pid = cp->نمبر;

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      QTYPE* queue = get_queue_prop<QTYPE>(cobj, pid);
      أكد(queue);

      size_t size = queue->get_size();

      ELEM value;
      إذا (size) {
	    get_val_func(queue, value);
      } وإلا {
	    dq_default(value, wid);
	    cerr << thr->get_fileline()
	         << "Warning: pop_" << loc << "() on empty "
	         << get_queue_type(value) << "." << endl;
      }

      ڈالو_قدر(thr, value, wid);
      أرجع true;
}

template <typename ELEM, صنف QTYPE>
ساكن منطقي qpop_b_prop(vthread_t thr, vvp_code_t cp, unsigned wid)
{
      أرجع q_نکالو_prop<ELEM, QTYPE>(thr, cp, get_back_قدر<ELEM>, "back", wid);
}

template <typename ELEM, صنف QTYPE>
ساكن منطقي qpop_f_prop(vthread_t thr, vvp_code_t cp, unsigned wid)
{
      أرجع q_نکالو_prop<ELEM, QTYPE>(thr, cp, get_front_قدر<ELEM>, "front", wid);
}

منطقي of_QPOP_PROP_B_REAL(vthread_t thr, vvp_code_t cp)
{
      أرجع qpop_b_prop<مزدوج, vvp_queue_real>(thr, cp, 0);
}

منطقي of_QPOP_PROP_B_STR(vthread_t thr, vvp_code_t cp)
{
      أرجع qpop_b_prop<string, vvp_queue_string>(thr, cp, 0);
}

منطقي of_QPOP_PROP_B_V(vthread_t thr, vvp_code_t cp)
{
      أرجع qpop_b_prop<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[0]);
}

منطقي of_QPOP_PROP_F_REAL(vthread_t thr, vvp_code_t cp)
{
      أرجع qpop_f_prop<مزدوج, vvp_queue_real>(thr, cp, 0);
}

منطقي of_QPOP_PROP_F_STR(vthread_t thr, vvp_code_t cp)
{
      أرجع qpop_f_prop<string, vvp_queue_string>(thr, cp, 0);
}

منطقي of_QPOP_PROP_F_V(vthread_t thr, vvp_code_t cp)
{
      أرجع qpop_f_prop<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[0]);
}

ساكن vvp_vector4_t queue_unique_ulong_to_vec4(unsigned long ul, unsigned bits)
{
      vvp_vector4_t val(bits, BIT4_0);
      لكل (unsigned idx = 0; idx < bits && idx < 8 * حجم(unsigned long); idx += 1) {
	    إذا (ul & 1UL) {
		  val.set_bit(idx, BIT4_1);
	    } وإلا {
		  val.set_bit(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      أرجع val;
}

template <صنف SRC> ساكن vvp_queue_vec4* queue_run_unique_src(SRC* src,
								 unsigned wid,
								 منطقي as_index)
{
      vvp_queue_vec4* dst = جديد vvp_queue_vec4();
      إذا (!src) أرجع dst;
      size_t n = src->get_size();
      لكل (size_t i = 0; i < n; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    منطقي seen = خطأ;
	    لكل (size_t j = 0; j < i; j += 1) {
		  vvp_vector4_t vj(wid);
		  src->get_word(j, vj);
		  إذا (vi.eeq(vj)) {
			seen = true;
			اكسر;
		  }
	    }
	    إذا (seen) استمر;
	    إذا (as_index) {
		  dst->push_back(queue_unique_ulong_to_vec4((unsigned long)i, 32), 0);
	    } وإلا {
		  dst->push_back(vi, 0);
	    }
      }
      أرجع dst;
}

template <صنف SRC>
ساكن vvp_queue_vec4* queue_run_find_matches_src(SRC* src, unsigned wid,
						  ثابت vvp_vector4_t& cmp,
						  منطقي as_index)
{
      vvp_queue_vec4* dst = جديد vvp_queue_vec4();
      إذا (!src) أرجع dst;
      size_t n = src->get_size();
      لكل (size_t i = 0; i < n; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    إذا (vi.eeq(cmp)) {
		  إذا (as_index) {
			dst->push_back(queue_unique_ulong_to_vec4((unsigned long)i, 32), 0);
		  } وإلا {
			dst->push_back(vi, 0);
		  }
	    }
      }
      أرجع dst;
}

/* Array/queue locator methods return a queue with 0 or 1 elements (LRM). */
template <صنف SRC>
ساكن vvp_queue_vec4* queue_find_first_last_to_queue_src(SRC* src,
							  unsigned wid,
							  ثابت vvp_vector4_t& cmp,
							  منطقي want_first,
							  منطقي want_index)
{
      vvp_queue_vec4* dst = جديد vvp_queue_vec4();
      إذا (!src || src->get_size() == 0) أرجع dst;
      size_t n = src->get_size();
      إذا (want_first) {
	    لكل (size_t i = 0; i < n; i += 1) {
		  vvp_vector4_t vi(wid);
		  src->get_word(i, vi);
		  إذا (vi.eeq(cmp)) {
			إذا (want_index) {
			      dst->push_back(
				  queue_unique_ulong_to_vec4((unsigned long)i, 32),
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
		  src->get_word(i, vi);
		  إذا (vi.eeq(cmp)) {
			إذا (want_index) {
			      dst->push_back(
				  queue_unique_ulong_to_vec4((unsigned long)i, 32),
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
template <صنف SRC>
ساكن vvp_queue_vec4* queue_run_min_max_src(SRC* src, unsigned wid, منطقي want_max)
{
      vvp_queue_vec4* dst = جديد vvp_queue_vec4();
      إذا (!src || src->get_size() == 0) أرجع dst;

      vvp_vector4_t best(wid);
      src->get_word(0, best);

      size_t n = src->get_size();
      لكل (size_t i = 1 ; i < n ; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);

	    vvp_bit4_t ge = compare_gtge(vi, best, BIT4_1);
	    منطقي replace = want_max ? (ge == BIT4_1) : (ge == BIT4_0);
	    إذا (replace) best = vi;
      }

      dst->push_back(best, 0);
      أرجع dst;
}

منطقي of_QUEUE_FIND_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->نکالو_vec4();
      unsigned wid = cp->bit_idx[0];
      أكد(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      إذا (qsrc) dst = queue_run_find_matches_src(qsrc, wid, cmp, خطأ);
      وإلا إذا (dsrc) dst = queue_run_find_matches_src(dsrc, wid, cmp, خطأ);
      وإلا {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_run_find_matches_src(src, wid, cmp, خطأ);
      }
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع true;
}

منطقي of_QUEUE_FIND_PROP_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->نکالو_vec4();
      size_t pid = cp->نمبر;
      unsigned wid = cp->bit_idx[0];
      أكد(cmp.size() == wid);

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_find_matches_src(qsrc, wid, cmp, خطأ)
				 : queue_run_find_matches_src(dsrc, wid, cmp, خطأ);
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع true;
}

منطقي of_QUEUE_FIND_INDEX_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->نکالو_vec4();
      unsigned wid = cp->bit_idx[0];
      أكد(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      إذا (qsrc) dst = queue_run_find_matches_src(qsrc, wid, cmp, true);
      وإلا إذا (dsrc) dst = queue_run_find_matches_src(dsrc, wid, cmp, true);
      وإلا {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_run_find_matches_src(src, wid, cmp, true);
      }
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع true;
}

منطقي of_QUEUE_FIND_INDEX_PROP_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->نکالو_vec4();
      size_t pid = cp->نمبر;
      unsigned wid = cp->bit_idx[0];
      أكد(cmp.size() == wid);

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_find_matches_src(qsrc, wid, cmp, true)
				 : queue_run_find_matches_src(dsrc, wid, cmp, true);
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع true;
}

منطقي of_QUEUE_FIND_FIRST_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->نکالو_vec4();
      unsigned wid = cp->bit_idx[0];
      أكد(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      إذا (qsrc) dst = queue_find_first_last_to_queue_src(qsrc, wid, cmp, true, خطأ);
      وإلا إذا (dsrc) dst = queue_find_first_last_to_queue_src(dsrc, wid, cmp, true, خطأ);
      وإلا {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_find_first_last_to_queue_src(src, wid, cmp, true, خطأ);
      }
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع true;
}

منطقي of_QUEUE_FIND_FIRST_PROP_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->نکالو_vec4();
      size_t pid = cp->نمبر;
      unsigned wid = cp->bit_idx[0];
      أكد(cmp.size() == wid);

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst =
	    qsrc ? queue_find_first_last_to_queue_src(qsrc, wid, cmp, true, خطأ)
		 : queue_find_first_last_to_queue_src(dsrc, wid, cmp, true, خطأ);
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع true;
}

منطقي of_QUEUE_FIND_FIRST_INDEX_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->نکالو_vec4();
      unsigned wid = cp->bit_idx[0];
      أكد(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      إذا (qsrc) dst = queue_find_first_last_to_queue_src(qsrc, wid, cmp, true, true);
      وإلا إذا (dsrc) dst = queue_find_first_last_to_queue_src(dsrc, wid, cmp, true, true);
      وإلا {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_find_first_last_to_queue_src(src, wid, cmp, true, true);
      }
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع true;
}

منطقي of_QUEUE_FIND_FIRST_INDEX_PROP_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->نکالو_vec4();
      size_t pid = cp->نمبر;
      unsigned wid = cp->bit_idx[0];
      أكد(cmp.size() == wid);

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst =
	    qsrc ? queue_find_first_last_to_queue_src(qsrc, wid, cmp, true, true)
		 : queue_find_first_last_to_queue_src(dsrc, wid, cmp, true, true);
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع true;
}

منطقي of_QUEUE_FIND_LAST_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->نکالو_vec4();
      unsigned wid = cp->bit_idx[0];
      أكد(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      إذا (qsrc) dst = queue_find_first_last_to_queue_src(qsrc, wid, cmp, خطأ, خطأ);
      وإلا إذا (dsrc) dst = queue_find_first_last_to_queue_src(dsrc, wid, cmp, خطأ, خطأ);
      وإلا {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_find_first_last_to_queue_src(src, wid, cmp, خطأ, خطأ);
      }
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع true;
}

منطقي of_QUEUE_FIND_LAST_PROP_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->نکالو_vec4();
      size_t pid = cp->نمبر;
      unsigned wid = cp->bit_idx[0];
      أكد(cmp.size() == wid);

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst =
	    qsrc ? queue_find_first_last_to_queue_src(qsrc, wid, cmp, خطأ, خطأ)
		 : queue_find_first_last_to_queue_src(dsrc, wid, cmp, خطأ, خطأ);
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع true;
}

منطقي of_QUEUE_FIND_LAST_INDEX_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->نکالو_vec4();
      unsigned wid = cp->bit_idx[0];
      أكد(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      إذا (qsrc) dst = queue_find_first_last_to_queue_src(qsrc, wid, cmp, خطأ, true);
      وإلا إذا (dsrc) dst = queue_find_first_last_to_queue_src(dsrc, wid, cmp, خطأ, true);
      وإلا {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_find_first_last_to_queue_src(src, wid, cmp, خطأ, true);
      }
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع true;
}

منطقي of_QUEUE_FIND_LAST_INDEX_PROP_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->نکالو_vec4();
      size_t pid = cp->نمبر;
      unsigned wid = cp->bit_idx[0];
      أكد(cmp.size() == wid);

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst =
	    qsrc ? queue_find_first_last_to_queue_src(qsrc, wid, cmp, خطأ, true)
		 : queue_find_first_last_to_queue_src(dsrc, wid, cmp, خطأ, true);
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع true;
}

منطقي of_QUEUE_MIN_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      إذا (qsrc) dst = queue_run_min_max_src(qsrc, wid, خطأ);
      وإلا إذا (dsrc) dst = queue_run_min_max_src(dsrc, wid, خطأ);
      وإلا {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_run_min_max_src(src, wid, خطأ);
      }
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع true;
}

منطقي of_QUEUE_MIN_OBJ_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->نکالو_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_min_max_src(qsrc, wid, خطأ)
				 : queue_run_min_max_src(dsrc, wid, خطأ);
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع true;
}

منطقي of_QUEUE_MIN_PROP_V(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->نمبر;
      unsigned wid = cp->bit_idx[0];

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_min_max_src(qsrc, wid, خطأ)
				 : queue_run_min_max_src(dsrc, wid, خطأ);
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع true;
}

منطقي of_QUEUE_MAX_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      إذا (qsrc) dst = queue_run_min_max_src(qsrc, wid, true);
      وإلا إذا (dsrc) dst = queue_run_min_max_src(dsrc, wid, true);
      وإلا {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_run_min_max_src(src, wid, true);
      }
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع true;
}

منطقي of_QUEUE_MAX_OBJ_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->نکالو_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_min_max_src(qsrc, wid, true)
				 : queue_run_min_max_src(dsrc, wid, true);
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع true;
}

منطقي of_QUEUE_MAX_PROP_V(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->نمبر;
      unsigned wid = cp->bit_idx[0];

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_min_max_src(qsrc, wid, true)
				 : queue_run_min_max_src(dsrc, wid, true);
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع true;
}

منطقي of_QUEUE_UNIQUE_OBJ_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->نکالو_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_unique_src(qsrc, wid, خطأ)
				 : queue_run_unique_src(dsrc, wid, خطأ);
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع true;
}

منطقي of_QUEUE_UNIQUE_INDEX_OBJ_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->نکالو_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_unique_src(qsrc, wid, true)
				 : queue_run_unique_src(dsrc, wid, true);
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع true;
}

منطقي of_QUEUE_UNIQUE_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      إذا (qsrc) dst = queue_run_unique_src(qsrc, wid, خطأ);
      وإلا إذا (dsrc) dst = queue_run_unique_src(dsrc, wid, خطأ);
      وإلا {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_run_unique_src(src, wid, خطأ);
      }
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع true;
}

منطقي of_QUEUE_UNIQUE_PROP_V(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->نمبر;
      unsigned wid = cp->bit_idx[0];

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_unique_src(qsrc, wid, خطأ)
				 : queue_run_unique_src(dsrc, wid, خطأ);
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع true;
}

منطقي of_QUEUE_UNIQUE_INDEX_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      إذا (qsrc) dst = queue_run_unique_src(qsrc, wid, true);
      وإلا إذا (dsrc) dst = queue_run_unique_src(dsrc, wid, true);
      وإلا {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_run_unique_src(src, wid, true);
      }
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع true;
}

منطقي of_QUEUE_UNIQUE_INDEX_PROP_V(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->نمبر;
      unsigned wid = cp->bit_idx[0];

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_unique_src(qsrc, wid, true)
				 : queue_run_unique_src(dsrc, wid, true);
      thr->ڈالو_object(vvp_object_t(dst));
      أرجع true;
}

منطقي of_PROP_QUEUE_SIZE(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->نمبر;

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);

      size_t sz = qsrc ? qsrc->get_size() : (dsrc ? dsrc->get_size() : 0);

      vvp_vector4_t val(32, BIT4_0);
      unsigned long ul = sz;
      لكل (unsigned idx = 0; idx < 32; idx++) {
	    إذا (ul & 1UL) {
		  val.set_bit(idx, BIT4_1);
	    } وإلا {
		  val.set_bit(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      thr->ڈالو_vec4(val);
      أرجع true;
}

منطقي of_QUEUE_SIZE_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      size_t sz =
	    qsrc ? qsrc->get_size() : (dsrc ? dsrc->get_size() : 0);

      vvp_vector4_t val(32, BIT4_0);
      unsigned long ul = sz;
      لكل (unsigned idx = 0; idx < 32; idx++) {
	    إذا (ul & 1UL) {
		  val.set_bit(idx, BIT4_1);
	    } وإلا {
		  val.set_bit(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      thr->ڈالو_vec4(val);
      أرجع true;
}

منطقي of_QUEUE_WORD_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      unsigned ix_reg = cp->bit_idx[1];
      int64_t adr = thr->الفاظ[ix_reg].w_int;
      إذا (thr->flags[4] == BIT4_1) {
	    thr->ڈالو_vec4(vvp_vector4_t(wid));
	    أرجع true;
      }
      إذا (adr < 0) {
	    thr->ڈالو_vec4(vvp_vector4_t(wid));
	    أرجع true;
      }
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t out(wid);
      إذا (qsrc) {
	    إذا ((size_t)adr >= qsrc->get_size()) {
		  thr->ڈالو_vec4(out);
		  أرجع true;
	    }
	    qsrc->get_word((unsigned)adr, out);
      } وإلا إذا (dsrc) {
	    إذا ((size_t)adr >= dsrc->get_size()) {
		  thr->ڈالو_vec4(out);
		  أرجع true;
	    }
	    dsrc->get_word((unsigned)adr, out);
      } وإلا {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    إذا (!src || (size_t)adr >= src->get_size()) {
		  thr->ڈالو_vec4(out);
		  أرجع true;
	    }
	    src->get_word((unsigned)adr, out);
      }
      thr->ڈالو_vec4(out);
      أرجع true;
}

منطقي of_QUEUE_WORD_PROP_V(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->نمبر;
      unsigned wid = cp->bit_idx[0];
      unsigned ix_reg = cp->bit_idx[1];
      int64_t adr = thr->الفاظ[ix_reg].w_int;
      إذا (thr->flags[4] == BIT4_1) {
	    thr->ڈالو_vec4(vvp_vector4_t(wid));
	    أرجع true;
      }
      إذا (adr < 0) {
	    thr->ڈالو_vec4(vvp_vector4_t(wid));
	    أرجع true;
      }

      vvp_object_t saved_چوٹی;
      منطقي popped_چوٹی = خطأ;
      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      إذا (cobj == 0) {
	    // Array-locator with(property) keeps an accumulator queue on top.
	    // Temporarily pop it to access the class object beneath.
	    thr->نکالو_object(saved_چوٹی);
	    popped_چوٹی = true;
	    vvp_object_t& below = thr->peek_object();
	    cobj = below.peek<vvp_cobject>();
      }
      إذا (cobj == 0) {
	    إذا (popped_چوٹی) thr->ڈالو_object(saved_چوٹی);
	    thr->ڈالو_vec4(vvp_vector4_t(wid));
	    أرجع true;
      }

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t out(wid);
      إذا (qsrc) {
	    إذا ((size_t)adr >= qsrc->get_size()) {
		  thr->ڈالو_vec4(out);
		  أرجع true;
	    }
	    qsrc->get_word((unsigned)adr, out);
      } وإلا إذا (dsrc) {
	    إذا ((size_t)adr >= dsrc->get_size()) {
		  thr->ڈالو_vec4(out);
		  أرجع true;
	    }
	    dsrc->get_word((unsigned)adr, out);
      } وإلا {
	    thr->ڈالو_vec4(out);
	    إذا (popped_چوٹی) thr->ڈالو_object(saved_چوٹی);
	    أرجع true;
      }
      إذا (popped_چوٹی) thr->ڈالو_object(saved_چوٹی);
      thr->ڈالو_vec4(out);
      أرجع true;
}

منطقي of_CMPIX_LTU(vthread_t thr, vvp_code_t cp)
{
      unsigned a = cp->نمبر;
      unsigned b = cp->bit_idx[0];
      إذا (thr->الفاظ[a].w_uint < thr->الفاظ[b].w_uint) {
	    thr->flags[4] = BIT4_1;
      } وإلا {
	    thr->flags[4] = BIT4_0;
      }
      أرجع true;
}

منطقي of_CMPIX_SLT0(vthread_t thr, vvp_code_t cp)
{
      unsigned r = cp->نمبر;
      thr->flags[4] = (thr->الفاظ[r].w_int < 0) ? BIT4_1 : BIT4_0;
      أرجع true;
}

منطقي of_ڈالو_IX_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned ix = cp->نمبر;
      unsigned wid = cp->bit_idx[0];
      unsigned sgn = cp->bit_idx[1];
      uint32_t ubits;
      إذا (sgn) {
	    int32_t sv = (int32_t)thr->الفاظ[ix].w_int;
	    ubits = (uint32_t)sv;
      } وإلا {
	    ubits = (uint32_t)thr->الفاظ[ix].w_uint;
      }
      إذا (wid > 32) wid = 32;
      vvp_vector4_t tmp(wid, BIT4_0);
      لكل (unsigned idx = 0; idx < wid; idx++) {
	    إذا (ubits & 1u) {
		  tmp.set_bit(idx, BIT4_1);
	    } وإلا {
		  tmp.set_bit(idx, BIT4_0);
	    }
	    ubits >>= 1;
      }
      thr->ڈالو_vec4(tmp);
      أرجع true;
}

منطقي of_QUEUE_NEW_EMPTY_V(vthread_t thr, vvp_code_t)
{
      thr->ڈالو_object(vvp_object_t(جديد vvp_queue_vec4()));
      أرجع true;
}

منطقي of_QUEUE_APPEND_WORD_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      vvp_vector4_t elem = thr->نکالو_vec4();
      أكد(elem.size() == wid);
      vvp_object_t qobj;
      thr->نکالو_object(qobj);
      vvp_queue_vec4* q = qobj.peek<vvp_queue_vec4>();
      أكد(q);
      q->push_back(elem, 0);
      thr->ڈالو_object(qobj);
      أرجع true;
}

منطقي of_DELETE_PROP_ELEM(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->نمبر;

      int64_t idx_val = thr->الفاظ[3].w_int;
      إذا (thr->flags[4] == BIT4_1) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with undefined index."
	         << endl;
	    أرجع true;
      }
      إذا (idx_val < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with negative index."
	         << endl;
	    أرجع true;
      }
      size_t idx = idx_val;

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue* queue = qobj.peek<vvp_queue>();
      إذا (queue == 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping delete(" << idx
	         << ") on empty queue." << endl;
      } وإلا {
	    size_t size = queue->get_size();
	    إذا (idx >= size) {
		  cerr << thr->get_fileline()
		       << "Warning: skipping out of range delete(" << idx
		       << ") on queue of size " << size << "." << endl;
	    } وإلا {
		  queue->erase(idx);
	    }
      }

      أرجع true;
}

منطقي of_DELETE_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->نمبر;

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      cobj->set_object(pid, vvp_object_t(), 0);

      أرجع true;
}

template <typename ELEM, صنف QTYPE>
ساكن منطقي store_qb(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
      ELEM value;
      vvp_net_t*net = cp->net;
      unsigned max_size = thr->الفاظ[cp->bit_idx[0]].w_int;
      نکالو_قدر(thr, value, wid); // Pop the value to store.

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      أكد(queue);
      queue->push_back(value, max_size);
      أرجع true;
}

/*
 * %store/qb/r <var-label>, <max-idx>
 */
منطقي of_STORE_QB_R(vthread_t thr, vvp_code_t cp)
{
      أرجع store_qb<مزدوج, vvp_queue_real>(thr, cp);
}

/*
 * %store/qb/str <var-label>, <max-idx>
 */
منطقي of_STORE_QB_STR(vthread_t thr, vvp_code_t cp)
{
      أرجع store_qb<string, vvp_queue_string>(thr, cp);
}

/*
 * %store/qb/v <var-label>, <max-idx>, <wid>
 */
منطقي of_STORE_QB_V(vthread_t thr, vvp_code_t cp)
{
      أرجع store_qb<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

template <typename ELEM, صنف QTYPE>
ساكن منطقي store_qdar(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
      int64_t idx = thr->الفاظ[3].w_int;
      ELEM value;
      vvp_net_t*net = cp->net;
      نکالو_قدر(thr, value, wid); // Pop the value to store.

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      أكد(queue);
      إذا (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot assign to a negative "
	         << get_queue_type(value)
	         << " index (" << idx << "). ";
	    print_queue_قدر(value);
	    cerr << " was not added." << endl;
      } وإلا إذا (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot assign to an undefined "
	         << get_queue_type(value) << " index. ";
	    print_queue_قدر(value);
	    cerr << " was not added." << endl;
      } وإلا {
	    unsigned max_size = thr->الفاظ[cp->bit_idx[0]].w_int;
	    queue->set_word_max(idx, value, max_size);
      }
      أرجع true;
}

/*
 * %store/qdar/r <var>, idx
 */
منطقي of_STORE_QDAR_R(vthread_t thr, vvp_code_t cp)
{
      أرجع store_qdar<مزدوج, vvp_queue_real>(thr, cp);
}

/*
 * %store/qdar/str <var>, idx
 */
منطقي of_STORE_QDAR_STR(vthread_t thr, vvp_code_t cp)
{
      أرجع store_qdar<string, vvp_queue_string>(thr, cp);
}

/*
 * %store/qdar/v <var>, idx
 */
منطقي of_STORE_QDAR_V(vthread_t thr, vvp_code_t cp)
{
      أرجع store_qdar<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

template <typename ELEM, صنف QTYPE>
ساكن منطقي store_qf(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
      ELEM value;
      vvp_net_t*net = cp->net;
      unsigned max_size = thr->الفاظ[cp->bit_idx[0]].w_int;
      نکالو_قدر(thr, value, wid); // Pop the value to store.

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      أكد(queue);
      queue->push_front(value, max_size);
      أرجع true;
}
/*
 * %store/qf/r <var-label>, <max-idx>
 */
منطقي of_STORE_QF_R(vthread_t thr, vvp_code_t cp)
{
      أرجع store_qf<مزدوج, vvp_queue_real>(thr, cp);
}

/*
 * %store/qf/str <var-label>, <max-idx>
 */
منطقي of_STORE_QF_STR(vthread_t thr, vvp_code_t cp)
{
      أرجع store_qf<string, vvp_queue_string>(thr, cp);
}

/*
 * %store/qb/v <var-label>, <max-idx>, <wid>
 */
منطقي of_STORE_QF_V(vthread_t thr, vvp_code_t cp)
{
      أرجع store_qf<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

template <typename ELEM, صنف QTYPE>
ساكن منطقي store_qobj(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
// FIXME: Can we actually use wid here?
      (فراغ)wid;
      vvp_net_t*net = cp->net;

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      أكد(queue);

      vvp_object_t src;
      thr->نکالو_object(src);

        // If it is null just clear the queue
      إذا (src.test_nil())
	    queue->erase_tail(0);
      وإلا {
	    unsigned max_size = thr->الفاظ[cp->bit_idx[0]].w_int;
	    queue->copy_elems(src, max_size);
      }

      أرجع true;
}

منطقي of_STORE_QOBJ_R(vthread_t thr, vvp_code_t cp)
{
      أرجع store_qobj<مزدوج, vvp_queue_real>(thr, cp);
}

منطقي of_STORE_QOBJ_STR(vthread_t thr, vvp_code_t cp)
{
      أرجع store_qobj<string, vvp_queue_string>(thr, cp);
}

منطقي of_STORE_QOBJ_V(vthread_t thr, vvp_code_t cp)
{
      أرجع store_qobj<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

ساكن فراغ vvp_send(vthread_t thr, vvp_net_ptr_t ptr, ثابت مزدوج&val)
{
      vvp_send_real(ptr, val, thr->wt_context);
}

ساكن فراغ vvp_send(vthread_t thr, vvp_net_ptr_t ptr, ثابت string&val)
{
      vvp_send_string(ptr, val, thr->wt_context);
}

template <typename ELEM>
ساكن منطقي store(vthread_t thr, vvp_code_t cp)
{
      ELEM val;
      نکالو_قدر(thr, val, 0);
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_send(thr, ptr, val);
      أرجع true;
}

منطقي of_STORE_REAL(vthread_t thr, vvp_code_t cp)
{
      أرجع store<مزدوج>(thr, cp);
}

template <typename ELEM>
ساكن منطقي storea(vthread_t thr, vvp_code_t cp)
{
      unsigned idx = cp->bit_idx[0];
      ELEM val;
      نکالو_قدر(thr, val, 0);

      إذا (thr->flags[4] != BIT4_1) {
	    unsigned adr = thr->الفاظ[idx].w_int;
	    cp->array->set_word(adr, val);
      }

      أرجع true;
}

/*
 * %store/reala <var-label> <index>
 */
منطقي of_STORE_REALA(vthread_t thr, vvp_code_t cp)
{
      أرجع storea<مزدوج>(thr, cp);
}

منطقي of_STORE_STR(vthread_t thr, vvp_code_t cp)
{
      أرجع store<string>(thr, cp);
}

/*
 * %store/stra <array-label> <index>
 */
منطقي of_STORE_STRA(vthread_t thr, vvp_code_t cp)
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
منطقي of_STORE_VEC4(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr(cp->net, 0);
      ثابت vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*> (cp->net->fil);
      unsigned off_index = cp->bit_idx[0];
      unsigned صحيح wid = cp->bit_idx[1];

      int64_t off = off_index ? thr->الفاظ[off_index].w_int : 0;
      unsigned صحيح sig_قدر_size = sig->قدر_size();

      vvp_vector4_t&val = thr->peek_vec4();
      unsigned val_size = val.size();

      إذا (val_size < wid) {
	    cerr << thr->get_fileline()
	         << "XXXX Internal error: val.size()=" << val_size
		 << ", expecting >= " << wid << endl;
      }
      أكد(val_size >= wid);
      إذا (val_size > wid) {
	    val.resize(wid);
      }

	// If there is a problem loading the index register, flags-4
	// will be set to 1, and we know here to skip the actual assignment.
      إذا (off_index!=0 && thr->flags[4] == BIT4_1) {
	    thr->نکالو_vec4(1);
	    أرجع true;
      }

      إذا (!resize_rval_vec(val, off, sig_قدر_size)) {
	    thr->نکالو_vec4(1);
	    أرجع true;
      }

      إذا (off == 0 && val.size() == sig_قدر_size)
	    vvp_send_vec4(ptr, val, thr->wt_context);
      وإلا
	    vvp_send_vec4_pv(ptr, val, off, sig_قدر_size, thr->wt_context);

      thr->نکالو_vec4(1);
      أرجع true;
}

/*
 * %store/vec4a <var-label>, <addr>, <offset>
 */
منطقي of_STORE_VEC4A(vthread_t thr, vvp_code_t cp)
{
      unsigned adr_index = cp->bit_idx[0];
      unsigned off_index = cp->bit_idx[1];

      long adr = adr_index? thr->الفاظ[adr_index].w_int : 0;
      int64_t off = off_index ? thr->الفاظ[off_index].w_int : 0;

	// Suppress action if flags-4 is true.
      إذا (thr->flags[4] == BIT4_1) {
	    thr->نکالو_vec4(1);
	    أرجع true;
      }

      vvp_vector4_t &value = thr->peek_vec4();

      إذا (!resize_rval_vec(value, off, cp->array->get_word_size())) {
	    thr->نکالو_vec4(1);
	    أرجع true;
      }

      cp->array->set_word(adr, off, value);

      thr->نکالو_vec4(1);
      أرجع true;
}

/*
 * %sub
 *   pop r;
 *   pop l;
 *   push l-r;
 */
منطقي of_SUB(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t r = thr->نکالو_vec4();
      vvp_vector4_t&l = thr->peek_vec4();

      l.sub(r);
      أرجع true;
}

/*
 * %subi <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
منطقي of_SUBI(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->نمبر;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_immediate_rval (cp, r);

      l.sub(r);

      أرجع true;

}

منطقي of_SUB_WR(vthread_t thr, vvp_code_t)
{
      مزدوج r = thr->نکالو_real();
      مزدوج l = thr->نکالو_real();
      thr->ڈالو_real(l - r);
      أرجع true;
}

/*
 * %substr <first>, <last>
 * Pop a string, take the substring (SystemVerilog style), and return
 * the result to the stack. This opcode actually works by editing the
 * string in place.
 */
منطقي of_SUBSTR(vthread_t thr, vvp_code_t cp)
{
      int32_t first = thr->الفاظ[cp->bit_idx[0]].w_int;
      int32_t last = thr->الفاظ[cp->bit_idx[1]].w_int;
      string&val = thr->peek_str(0);

      إذا (first < 0 || last < first || last >= (int32_t)val.size()) {
	    val = string("");
	    أرجع true;
      }

      val = val.substr(first, last-first+1);
      أرجع true;
}

/*
 * %substr/vec4 <index>, <wid>
 */
منطقي of_SUBSTR_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned sel_idx = cp->bit_idx[0];
      unsigned wid = cp->bit_idx[1];

      int32_t sel = thr->الفاظ[sel_idx].w_int;
      ثابت string&val = thr->peek_str(0);

      أكد(wid%8 == 0);

      إذا (sel < 0 || sel >= (int32_t)val.size()) {
	    vvp_vector4_t res (wid, BIT4_0);
	    thr->ڈالو_vec4(res);
	    أرجع true;
      }

      vvp_vector4_t res (wid, BIT4_0);

      أكد(wid==8);
      unsigned حرف tmp = val[sel];
      لكل (صحيح idx = 0 ; idx < 8 ; idx += 1) {
	    إذا (tmp & (1<<idx))
		  res.set_bit(idx, BIT4_1);
      }

      thr->ڈالو_vec4(res);
      أرجع true;
}

منطقي of_FILE_سطر(vthread_t thr, vvp_code_t cp)
{
      vpiHandle handle = cp->handle;

	/* When it is available, keep the file/line information in the
	   thread for error/warning messages. */
      thr->set_fileline(vpi_get_str(vpiFile, handle),
                        vpi_get(vpi_سطر_No, handle));

      إذا (show_file_سطر)
	    cerr << thr->get_fileline()
	         << vpi_get_str(_vpiDescription, handle) << endl;

      أرجع true;
}

/*
 * %test_nul <var-label>;
 * Test if the object at the specified variable is nil. If so, write
 * "1" into flags[4], otherwise write "0" into flags[4].
 */
منطقي of_TEST_NUL(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      أكد(net);
      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      أكد(obj);

      إذا (obj->get_object().test_nil())
	    thr->flags[4] = BIT4_1;
      وإلا
	    thr->flags[4] = BIT4_0;

      أرجع true;
}

منطقي of_TEST_NUL_A(vthread_t thr, vvp_code_t cp)
{
      unsigned idx = cp->bit_idx[0];
      unsigned adr = thr->الفاظ[idx].w_int;
      vvp_object_t word;

	/* If the address is undefined, return true. */
      إذا (thr->flags[4] == BIT4_1) {
	    أرجع true;
      }

      cp->array->get_word_obj(adr, word);
      إذا (word.test_nil())
	    thr->flags[4] = BIT4_1;
      وإلا
	    thr->flags[4] = BIT4_0;

      أرجع true;
}

منطقي of_TEST_NUL_OBJ(vthread_t thr, vvp_code_t)
{
      إذا (thr->peek_object().test_nil())
	    thr->flags[4] = BIT4_1;
      وإلا
	    thr->flags[4] = BIT4_0;
      أرجع true;
}

/*
 * %test_nul/prop <pid>, <idx>
 */
منطقي of_TEST_NUL_PROP(vthread_t thr, vvp_code_t cp)
{
      unsigned pid = cp->نمبر;
      unsigned idx = cp->bit_idx[0];

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

      أرجع true;
}

/* and() reduction returns the scalar sum (Verilog-style bitwise-AND per word). */
template <صنف SRC>
ساكن vvp_vector4_t uparr_and_الفاظ_src(SRC* src, unsigned wid)
{
      إذا (!src) أرجع vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_1);
      لكل (size_t i = 0; i < src->get_size(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    res &= vi;
      }
      أرجع res;
}

منطقي of_UPARR_AND_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      إذا (qsrc) res = uparr_and_الفاظ_src(qsrc, wid);
      وإلا إذا (dsrc) res = uparr_and_الفاظ_src(dsrc, wid);
      وإلا {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    res = uparr_and_الفاظ_src(src, wid);
      }
      thr->ڈالو_vec4(res);
      أرجع true;
}

/* or() reduction returns the scalar sum (Verilog-style bitwise-AND per word). */
template <صنف SRC>
ساكن vvp_vector4_t uparr_or_الفاظ_src(SRC* src, unsigned wid)
{
      إذا (!src) أرجع vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_0);
      لكل (size_t i = 0; i < src->get_size(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    res |= vi;
      }
      أرجع res;
}

منطقي of_UPARR_OR_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      إذا (qsrc) res = uparr_or_الفاظ_src(qsrc, wid);
      وإلا إذا (dsrc) res = uparr_or_الفاظ_src(dsrc, wid);
      وإلا {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    res = uparr_or_الفاظ_src(src, wid);
      }
      thr->ڈالو_vec4(res);
      أرجع true;
}

/* real product() reduction returns the scalar sum */
template <صنف SRC>
ساكن مزدوج uparr_product_الفاظ_src_real(SRC* src)
{
      إذا (!src) أرجع 0.0;
      مزدوج acc = 1.0;
      لكل (size_t i = 0; i < src->get_size(); i += 1) {
	    مزدوج vi;
	    src->get_word(i, vi);
	    acc *= vi;
      }
      أرجع acc;
}

منطقي of_UPARR_PRODUCT_REAL(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_queue_real* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_real_from_net(thr, net, qsrc, dsrc);
      مزدوج sum;
      إذا (qsrc) sum = uparr_product_الفاظ_src_real(qsrc);
      وإلا إذا (dsrc) sum = uparr_product_الفاظ_src_real(dsrc);
      وإلا {
	    vvp_queue* src_q = get_queue_object<vvp_queue_real>(thr, net);
	    vvp_queue_real* src = dynamic_cast<vvp_queue_real*>(src_q);
	    sum = uparr_product_الفاظ_src_real(src);
      }
      thr->ڈالو_real(sum);
      أرجع true;
}

/* product() reduction returns the scalar product (Verilog-style multiplication per word). */
template <صنف SRC>
ساكن vvp_vector4_t uparr_product_الفاظ_src(SRC* src, unsigned wid)
{
      إذا (!src) أرجع vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t acc = queue_unique_ulong_to_vec4(1UL, wid);
      لكل (size_t i = 0; i < src->get_size(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    acc.mul(vi);
      }
      أرجع acc;
}

منطقي of_UPARR_PRODUCT_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t prod;
      إذا (qsrc) prod = uparr_product_الفاظ_src(qsrc, wid);
      وإلا إذا (dsrc) prod = uparr_product_الفاظ_src(dsrc, wid);
      وإلا {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    prod = uparr_product_الفاظ_src(src, wid);
      }
      thr->ڈالو_vec4(prod);
      أرجع true;
}

منطقي of_UPARR_PRODUCT_PROP_V(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->نمبر;
      unsigned wid = cp->bit_idx[0];

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t prod =
	    qsrc ? uparr_product_الفاظ_src(qsrc, wid)
		 : uparr_product_الفاظ_src(dsrc, wid);
      thr->ڈالو_vec4(prod);
      أرجع true;
}

منطقي of_UPARR_PRODUCT_OBJ_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->نکالو_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_vector4_t prod =
	    qsrc ? uparr_product_الفاظ_src(qsrc, wid)
		 : uparr_product_الفاظ_src(dsrc, wid);
      thr->ڈالو_vec4(prod);
      أرجع true;
}

/* real sum() reduction returns the scalar sum */
template <صنف SRC>
ساكن مزدوج uparr_sum_الفاظ_src_real(SRC* src)
{
      مزدوج acc = 0.0;
      إذا (!src) أرجع acc;
      لكل (size_t i = 0; i < src->get_size(); i += 1) {
	    مزدوج vi;
	    src->get_word(i, vi);
	    acc += vi;
      }
      أرجع acc;
}

منطقي of_UPARR_SUM_REAL(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_queue_real* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_real_from_net(thr, net, qsrc, dsrc);
      مزدوج sum;
      إذا (qsrc) sum = uparr_sum_الفاظ_src_real(qsrc);
      وإلا إذا (dsrc) sum = uparr_sum_الفاظ_src_real(dsrc);
      وإلا {
	    vvp_queue* src_q = get_queue_object<vvp_queue_real>(thr, net);
	    vvp_queue_real* src = dynamic_cast<vvp_queue_real*>(src_q);
	    sum = uparr_sum_الفاظ_src_real(src);
      }
      thr->ڈالو_real(sum);
      أرجع true;
}

/* sum() reduction returns the scalar sum (Verilog-style addition per word). */
template <صنف SRC>
ساكن vvp_vector4_t uparr_sum_الفاظ_src(SRC* src, unsigned wid)
{
      إذا (!src) أرجع vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t acc(wid, BIT4_0);
      لكل (size_t i = 0; i < src->get_size(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    acc.add(vi);
      }
      أرجع acc;
}

منطقي of_UPARR_SUM_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t sum;
      إذا (qsrc) sum = uparr_sum_الفاظ_src(qsrc, wid);
      وإلا إذا (dsrc) sum = uparr_sum_الفاظ_src(dsrc, wid);
      وإلا {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    sum = uparr_sum_الفاظ_src(src, wid);
      }
      thr->ڈالو_vec4(sum);
      أرجع true;
}

منطقي of_UPARR_SUM_PROP_V(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->نمبر;
      unsigned wid = cp->bit_idx[0];

      vvp_object_t& چوٹی = thr->peek_object();
      vvp_cobject*cobj = چوٹی.peek<vvp_cobject>();
      أكد(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t sum =
	    qsrc ? uparr_sum_الفاظ_src(qsrc, wid)
		 : uparr_sum_الفاظ_src(dsrc, wid);
      thr->ڈالو_vec4(sum);
      أرجع true;
}

منطقي of_UPARR_SUM_OBJ_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->نکالو_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_vector4_t sum =
	    qsrc ? uparr_sum_الفاظ_src(qsrc, wid)
		 : uparr_sum_الفاظ_src(dsrc, wid);
      thr->ڈالو_vec4(sum);
      أرجع true;
}

/* xor() reduction returns the scalar sum (Verilog-style bitwise-XOR per word). */
template <صنف SRC>
ساكن vvp_vector4_t uparr_xor_الفاظ_src(SRC* src, unsigned wid)
{
      إذا (!src) أرجع vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_0);
      لكل (size_t i = 0; i < src->get_size(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    res ^= vi;
      }
      أرجع res;
}

منطقي of_UPARR_XOR_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      إذا (qsrc) res = uparr_xor_الفاظ_src(qsrc, wid);
      وإلا إذا (dsrc) res = uparr_xor_الفاظ_src(dsrc, wid);
      وإلا {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    res = uparr_xor_الفاظ_src(src, wid);
      }
      thr->ڈالو_vec4(res);
      أرجع true;
}

منطقي of_VPI_CALL(vthread_t thr, vvp_code_t cp)
{
      vpip_execute_vpi_call(thr, cp->handle);

      إذا (schedule_stopped()) {
	    إذا (! schedule_finished())
		  schedule_vthread(thr, 0, خطأ);

	    أرجع خطأ;
      }

      أرجع schedule_finished()? خطأ : true;
}

/* %wait <label>;
 * Implement the wait by locating the vvp_net_T for the event, and
 * adding this thread to the threads list for the event. The some
 * argument is the  reference to the functor to wait for. This must be
 * an event object of some sort.
 */
منطقي of_WAIT(vthread_t thr, vvp_code_t cp)
{
      أكد(! thr->i_am_in_function);
      أكد(! thr->waiting_for_event);
      thr->waiting_for_event = 1;

	/* Add this thread to the list in the event. */
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (cp->net->fun);
      أكد(ep);
      thr->wait_next = ep->add_waiting_thread(thr);

	/* Return false to suspend this thread. */
      أرجع خطأ;
}

/*
 * Implement the %wait/fork (SystemVerilog) instruction by suspending
 * the current thread until all the detached children have finished.
 */
منطقي of_WAIT_FORK(vthread_t thr, vvp_code_t)
{
	/* If a %wait/fork is being executed then the parent thread
	 * cannot be waiting in a join or already waiting. */
      أكد(! thr->i_am_in_function);
      أكد(! thr->i_am_joining);
      أكد(! thr->i_am_waiting);

	/* There should be no active children when waiting. */
      أكد(thr->children.empty());

	/* If there are no detached children then there is nothing to
	 * wait for. */
      إذا (thr->detached_children.empty()) أرجع true;

	/* Flag that this process is waiting for the detached children
	 * to finish and suspend it. */
      thr->i_am_waiting = 1;
      أرجع خطأ;
}

/*
 * %xnor
 */
منطقي of_XNOR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valr = thr->نکالو_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      أكد(vall.size() == valr.size());

      vall ^= valr;
      vall.invert();

      أرجع true;
}

/*
 * %xor
 */
منطقي of_XOR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valr = thr->نکالو_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      أكد(vall.size() == valr.size());

      vall ^= valr;

      أرجع true;
}


منطقي of_ZOMBIE(vthread_t thr, vvp_code_t)
{
      thr->pc = codespace_null();
      إذا ((thr->parent == 0) && (thr->children.empty())) {
	    إذا (thr->delay_delete)
		  schedule_del_thr(thr);
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
ساكن منطقي do_exec_ufunc(vthread_t thr, vvp_code_t cp, vthread_t child)
{
      __vpiScope*child_scope = cp->ufunc_core_ptr->func_scope();
      أكد(child_scope);

      أكد(child_scope->get_type_code() == vpiFunction);
      أكد(thr->children.empty());


        /* We can take a number of shortcuts because we know that a
           continuous assignment can only occur in a static scope. */
      أكد(thr->wt_context == 0);
      أكد(thr->rd_context == 0);

        /* If an automatic function, allocate a context for this call. */
      vvp_context_t child_context = 0;
      إذا (child_scope->is_automatic()) {
            child_context = vthread_alloc_context(child_scope);
            thr->wt_context = child_context;
            thr->rd_context = child_context;
      }

      child->wt_context = child_context;
      child->rd_context = child_context;

	/* Copy all the inputs to the ufunc object to the port
	   variables of the function. This copies all the values
	   atomically. */
      cp->ufunc_core_ptr->assign_bits_to_ports(child_context);
      child->delay_delete = 1;

      child->parent = thr;
      thr->children.insert(child);
	// This should be the only child
      أكد(thr->children.size()==1);

      child->is_scheduled = 1;
      child->i_am_in_function = 1;
      vthread_run(child);
      running_thread = thr;

      إذا (child->i_have_ended) {
	    do_join(thr, child);
            أرجع true;
      } وإلا {
	    thr->i_am_joining = 1;
	    أرجع خطأ;
      }
}

منطقي of_EXEC_UFUNC_REAL(vthread_t thr, vvp_code_t cp)
{
      __vpiScope*child_scope = cp->ufunc_core_ptr->func_scope();
      أكد(child_scope);

	/* Create a temporary thread and run it immediately. */
      vthread_t child = vthread_new(cp->cptr, child_scope);
      thr->ڈالو_real(0.0);
      child->args_real.push_back(0);

      أرجع do_exec_ufunc(thr, cp, child);
}

منطقي of_EXEC_UFUNC_VEC4(vthread_t thr, vvp_code_t cp)
{
      __vpiScope*child_scope = cp->ufunc_core_ptr->func_scope();
      أكد(child_scope);

      vpiScopeFunction*scope_func = dynamic_cast<vpiScopeFunction*>(child_scope);
      أكد(scope_func);

	/* Create a temporary thread and run it immediately. */
      vthread_t child = vthread_new(cp->cptr, child_scope);
      thr->ڈالو_vec4(vvp_vector4_t(scope_func->get_func_width(), scope_func->get_func_init_val()));
      child->args_vec4.push_back(0);

      أرجع do_exec_ufunc(thr, cp, child);
}

/*
 * This is a phantom opcode used to harvest the result of calling a user
 * defined function. It is used in code generated by the .ufunc statement.
 */
منطقي of_REAP_UFUNC(vthread_t thr, vvp_code_t cp)
{
      __vpiScope*child_scope = cp->ufunc_core_ptr->func_scope();
      أكد(child_scope);

	/* Copy the output from the result variable to the output
	   ports of the .ufunc device. */
      cp->ufunc_core_ptr->finish_thread();

        /* If an automatic function, free the context for this call. */
      إذا (child_scope->is_automatic()) {
            vthread_free_context(thr->rd_context, child_scope);
            thr->wt_context = 0;
            thr->rd_context = 0;
      }

      أرجع true;
}
