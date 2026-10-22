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

using நாமாகாஶ std;

/* This is the size of an unsigned long in bits. This is just a
   convenience macro. */
# define CPU_WORD_BITS (8*அளவு(unsigned long))
# define உச்சி_BIT (1UL << (CPU_WORD_BITS-1))

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

கட்டமைப்பு vthread_s {
      vthread_s();

      வெறுமை debug_dump(ostream&fd, மாறிலி எழுத்து*label_text);

	/* This is the program counter. */
      vvp_code_t pc;
	/* These hold the private thread bits. */
      எண்ணிக்கை { FLAGS_மொத்தம் = 512, சொற்கள்_மொத்தம் = 16 };
      vvp_bit4_t flags[FLAGS_மொத்தம்];

	/* These are the word registers. */
      ஒன்றிப்பு {
	    int64_t  w_int;
	    uint64_t w_uint;
      } சொற்கள்[சொற்கள்_மொத்தம்];

	// These vectors are depths within the parent thread's
	// corresponding stack.  This is how the %ret/* instructions
	// get at parent thread arguments.
      vector<unsigned> args_real;
      vector<unsigned> args_str;
      vector<unsigned> args_vec4;

    தனிப்பட்ட:
      vector<vvp_vector4_t>அடுக்கு_vec4;
    பொது:
      உள்வரி vvp_vector4_t எடு_vec4(வெறுமை)
      {
	    ப்ரதிஜ்ஞா(! அடுக்கு_vec4.empty());
	    vvp_vector4_t val = அடுக்கு_vec4.back();
	    அடுக்கு_vec4.pop_back();
	    திரும்பு val;
      }
      உள்வரி வெறுமை தள்ளு_vec4(மாறிலி vvp_vector4_t&val)
      {
	    அடுக்கு_vec4.push_back(val);
      }
      உள்வரி மாறிலி vvp_vector4_t& peek_vec4(unsigned depth)
      {
	    unsigned size = அடுக்கு_vec4.size();
	    ப்ரதிஜ்ஞா(depth < size);
	    unsigned use_index = size-1-depth;
	    திரும்பு அடுக்கு_vec4[use_index];
      }
      உள்வரி vvp_vector4_t& peek_vec4(வெறுமை)
      {
	    unsigned use_index = அடுக்கு_vec4.size();
	    ப்ரதிஜ்ஞா(use_index >= 1);
	    திரும்பு அடுக்கு_vec4[use_index-1];
      }
      உள்வரி வெறுமை poke_vec4(unsigned depth, மாறிலி vvp_vector4_t&val)
      {
	    ப்ரதிஜ்ஞா(depth < அடுக்கு_vec4.size());
	    unsigned use_index = அடுக்கு_vec4.size()-1-depth;
	    அடுக்கு_vec4[use_index] = val;
      }
      உள்வரி வெறுமை எடு_vec4(unsigned cnt)
      {
	    போது (cnt > 0) {
		  அடுக்கு_vec4.pop_back();
		  cnt -= 1;
	    }
      }


    தனிப்பட்ட:
      vector<இரட்டை> அடுக்கு_real;
    பொது:
      உள்வரி இரட்டை எடு_real(வெறுமை)
      {
	    ப்ரதிஜ்ஞா(! அடுக்கு_real.empty());
	    இரட்டை val = அடுக்கு_real.back();
	    அடுக்கு_real.pop_back();
	    திரும்பு val;
      }
      உள்வரி வெறுமை தள்ளு_real(இரட்டை val)
      {
	    அடுக்கு_real.push_back(val);
      }
      உள்வரி இரட்டை peek_real(unsigned depth)
      {
	    ப்ரதிஜ்ஞா(depth < அடுக்கு_real.size());
	    unsigned use_index = அடுக்கு_real.size()-1-depth;
	    திரும்பு அடுக்கு_real[use_index];
      }
      உள்வரி வெறுமை poke_real(unsigned depth, இரட்டை val)
      {
	    ப்ரதிஜ்ஞா(depth < அடுக்கு_real.size());
	    unsigned use_index = அடுக்கு_real.size()-1-depth;
	    அடுக்கு_real[use_index] = val;
      }
      உள்வரி வெறுமை எடு_real(unsigned cnt)
      {
	    போது (cnt > 0) {
		  அடுக்கு_real.pop_back();
		  cnt -= 1;
	    }
      }

	/* Strings are operated on using a forth-like operator
	   set. Items at the top of the stack (back()) are the objects
	   operated on except for special cases. New objects are
	   pushed onto the top (back()) and pulled from the top
	   (back()) only. */
    தனிப்பட்ட:
      vector<string> அடுக்கு_str;
    பொது:
      உள்வரி string எடு_str(வெறுமை)
      {
	    ப்ரதிஜ்ஞா(! அடுக்கு_str.empty());
	    string val = அடுக்கு_str.back();
	    அடுக்கு_str.pop_back();
	    திரும்பு val;
      }
      உள்வரி வெறுமை தள்ளு_str(மாறிலி string&val)
      {
	    அடுக்கு_str.push_back(val);
      }
      உள்வரி string&peek_str(unsigned depth)
      {
	    ப்ரதிஜ்ஞா(depth<அடுக்கு_str.size());
	    unsigned use_index = அடுக்கு_str.size()-1-depth;
	    திரும்பு அடுக்கு_str[use_index];
      }
      உள்வரி வெறுமை poke_str(unsigned depth, மாறிலி string&val)
      {
	    ப்ரதிஜ்ஞா(depth < அடுக்கு_str.size());
	    unsigned use_index = அடுக்கு_str.size()-1-depth;
	    அடுக்கு_str[use_index] = val;
      }
      உள்வரி வெறுமை எடு_str(unsigned cnt)
      {
	    போது (cnt > 0) {
		  அடுக்கு_str.pop_back();
		  cnt -= 1;
	    }
      }

	/* Objects are also operated on in a stack. */
    தனிப்பட்ட:
      எண்ணிக்கை { அடுக்கு_OBJ_MAX_SIZE = 32 };
      vvp_object_t அடுக்கு_obj[அடுக்கு_OBJ_MAX_SIZE];
      unsigned அடுக்கு_obj_size;
    பொது:
      உள்வரி vvp_object_t& peek_object(வெறுமை)
      {
	    ப்ரதிஜ்ஞா(அடுக்கு_obj_size > 0);
	    திரும்பு அடுக்கு_obj[அடுக்கு_obj_size-1];
      }
      உள்வரி வெறுமை எடு_object(vvp_object_t&obj)
      {
	    ப்ரதிஜ்ஞா(அடுக்கு_obj_size > 0);
	    அடுக்கு_obj_size -= 1;
	    obj = அடுக்கு_obj[அடுக்கு_obj_size];
	    அடுக்கு_obj[அடுக்கு_obj_size].reset(0);
      }
      உள்வரி வெறுமை எடு_object(unsigned cnt, unsigned skip =0)
      {
	    ப்ரதிஜ்ஞா((cnt+skip) <= அடுக்கு_obj_size);
	    க்கு (size_t idx = அடுக்கு_obj_size-skip-cnt ; idx < அடுக்கு_obj_size-skip ; idx += 1)
		  அடுக்கு_obj[idx].reset(0);
	    அடுக்கு_obj_size -= cnt;
	    க்கு (size_t idx = அடுக்கு_obj_size-skip ; idx < அடுக்கு_obj_size ; idx += 1)
		  அடுக்கு_obj[idx] = அடுக்கு_obj[idx+skip];
	    க்கு (size_t idx = அடுக்கு_obj_size ; idx < அடுக்கு_obj_size+skip ; idx += 1)
		  அடுக்கு_obj[idx].reset(0);
      }
      உள்வரி வெறுமை தள்ளு_object(மாறிலி vvp_object_t&obj)
      {
	    ப்ரதிஜ்ஞா(அடுக்கு_obj_size < அடுக்கு_OBJ_MAX_SIZE);
	    அடுக்கு_obj[அடுக்கு_obj_size] = obj;
	    அடுக்கு_obj_size += 1;
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
      set<கட்டமைப்பு vthread_s*>children;
	/* This points to the detached children of the thread. */
      set<கட்டமைப்பு vthread_s*>detached_children;
	/* This points to my parent, if I have one. */
      கட்டமைப்பு vthread_s*parent;
	/* This points to the containing scope. */
      __vpiScope*parent_scope;
	/* This is used for keeping wait queues. */
      கட்டமைப்பு vthread_s*wait_next;
	/* These are used to access automatically allocated items. */
      vvp_context_t wt_context, rd_context;
	/* These are used to pass non-blocking event control information. */
      vvp_net_t*event;
      uint64_t ecount;
	/* Save the file/line information when available. */
    தனிப்பட்ட:
      எழுத்து *filenm_;
      unsigned lineno_;
    பொது:
      வெறுமை set_fileline(எழுத்து *filenm, unsigned lineno);
      string get_fileline();

      உள்வரி வெறுமை cleanup()
      {
	    எனில் (i_was_disabled) {
		  அடுக்கு_vec4.clear();
		  அடுக்கு_real.clear();
		  அடுக்கு_str.clear();
		  எடு_object(அடுக்கு_obj_size);
	    }
	    விடுவி(filenm_);
	    filenm_ = 0;
	    ப்ரதிஜ்ஞா(அடுக்கு_vec4.empty());
	    ப்ரதிஜ்ஞா(அடுக்கு_real.empty());
	    ப்ரதிஜ்ஞா(அடுக்கு_str.empty());
	    ப்ரதிஜ்ஞா(அடுக்கு_obj_size == 0);
      }
};

உள்வரி vthread_s::vthread_s()
{
      அடுக்கு_obj_size = 0;
      filenm_ = 0;
      lineno_ = 0;
}

வெறுமை vthread_s::set_fileline(எழுத்து *filenm, unsigned lineno)
{
      ப்ரதிஜ்ஞா(filenm);
      எனில் (!filenm_ || (strcmp(filenm_, filenm) != 0)) {
	    விடுவி(filenm_);
	    filenm_ = strdup(filenm);
      }
      lineno_ = lineno;
}

உள்வரி string vthread_s::get_fileline()
{
      ostringstream buf;
      எனில் (filenm_) {
	    buf << filenm_ << ":" << lineno_ << ": ";
      }
      string res = buf.str();
      திரும்பு res;
}

வெறுமை vthread_s::debug_dump(ostream&fd, மாறிலி எழுத்து*label)
{
      fd << "**** " << label << endl;
      fd << "**** ThreadId: " << இது << ", parent id: " << parent << endl;

      fd << "**** Flags: ";
      க்கு (முழு idx = 0 ; idx < FLAGS_மொத்தம் ; idx += 1)
	    fd << flags[idx];
      fd << endl;
      fd << "**** vec4 stack..." << endl;
      க்கு (size_t idx = அடுக்கு_vec4.size() ; idx > 0 ; idx -= 1)
	    fd << "    " << (அடுக்கு_vec4.size()-idx) << ": " << அடுக்கு_vec4[idx-1] << endl;
      fd << "**** str stack (" << அடுக்கு_str.size() << ")..." << endl;
      fd << "**** obj stack (" << அடுக்கு_obj_size << ")..." << endl;
      fd << "**** args_vec4 array (" << args_vec4.size() << ")..." << endl;
      க்கு (size_t idx = 0 ; idx < args_vec4.size() ; idx += 1)
	    fd << "    " << idx << ": " << args_vec4[idx] << endl;
      fd << "**** file/line (";
      எனில் (filenm_) fd << filenm_;
      இல்லையெனில் fd << "<no file name>";
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
நிலையான string filter_string(மாறிலி எழுத்து*text)
{
      vector<எழுத்து> tmp (strlen(text)+1);
      size_t dst = 0;
      க்கு (மாறிலி எழுத்து*ptr = text ; *ptr ; ptr += 1) {
	    // Not an escape? Move on.
	    எனில் (*ptr != '\\') {
		  tmp[dst++] = *ptr;
		  தொடர்;
	    }

	    // Now we know that *ptr is pointing to a \ character and we
	    // have an octal sequence coming up. Advance the ptr and start
	    // processing octal digits.
	    ptr += 1;
	    எனில் (*ptr == 0)
		  நிறுத்து;

	    எழுத்து byte = 0;
	    முழு cnt = 3;
	    போது (*ptr && cnt > 0 && *ptr >= '0' && *ptr <= '7') {
		  byte *= 8;
		  byte += *ptr - '0';
		  cnt -= 1;
		  ptr += 1;
	    }

	    // null-bytes are supposed to be removed when assigning a string
	    // literal to a string.
	    எனில் (byte != '\0')
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
      திரும்பு res;
}

நிலையான வெறுமை do_join(vthread_t thr, vthread_t child);

__vpiScope* vthread_scope(கட்டமைப்பு vthread_s*thr)
{
      திரும்பு thr->parent_scope;
}

கட்டமைப்பு vthread_s*running_thread = 0;

string get_fileline()
{
      திரும்பு running_thread->get_fileline();
}

வெறுமை vthread_தள்ளு(கட்டமைப்பு vthread_s*thr, இரட்டை val)
{
      thr->தள்ளு_real(val);
}

வெறுமை vthread_தள்ளு(கட்டமைப்பு vthread_s*thr, மாறிலி string&val)
{
      thr->தள்ளு_str(val);
}

வெறுமை vthread_தள்ளு(கட்டமைப்பு vthread_s*thr, மாறிலி vvp_vector4_t&val)
{
      thr->தள்ளு_vec4(val);
}

வெறுமை vthread_எடு_real(கட்டமைப்பு vthread_s*thr, unsigned depth)
{
      thr->எடு_real(depth);
}

வெறுமை vthread_எடு_str(கட்டமைப்பு vthread_s*thr, unsigned depth)
{
      thr->எடு_str(depth);
}

வெறுமை vthread_எடு_vec4(கட்டமைப்பு vthread_s*thr, unsigned depth)
{
      thr->எடு_vec4(depth);
}

இரட்டை vthread_get_real_அடுக்கு(கட்டமைப்பு vthread_s*thr, unsigned depth)
{
      திரும்பு thr->peek_real(depth);
}

மாறிலி string&vthread_get_str_அடுக்கு(கட்டமைப்பு vthread_s*thr, unsigned depth)
{
      திரும்பு thr->peek_str(depth);
}

மாறிலி vvp_vector4_t& vthread_get_vec4_அடுக்கு(கட்டமைப்பு vthread_s*thr, unsigned depth)
{
      திரும்பு thr->peek_vec4(depth);
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
template <வகுப்பு VVP_QUEUE> நிலையான vvp_queue*get_queue_object(vthread_t thr, vvp_net_t*net)
{
      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      ப்ரதிஜ்ஞா(obj);

      vvp_queue*queue = obj->get_object().peek<vvp_queue>();
      எனில் (queue == 0) {
	    ப்ரதிஜ்ஞா(obj->get_object().test_nil());
	    queue = புதிய VVP_QUEUE;
	    vvp_object_t val (queue);
	    vvp_net_ptr_t ptr (net, 0);
	    vvp_send_object(ptr, val, thr->wt_context);
      }

      திரும்பு queue;
}

/*
 * Locator and size/word opcodes use the same VVP net for a queue ($) or a
 * dynamic array ([]). Queues are vvp_queue_vec4; dynamic arrays may be
 * vvp_darray_vec4, vvp_darray_atom<>, etc. (all vvp_darray, but vvp_queue
 * also derives from vvp_darray — test queue vec4 first).
 */
நிலையான வெறுமை get_queue_or_darray_vec4_from_net(vthread_t thr, vvp_net_t* net,
					      vvp_queue_vec4*& qsrc,
					      vvp_darray*& dsrc)
{
      vvp_fun_signal_object* obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      ப்ரதிஜ்ஞா(obj);
      vvp_object_t உச்சி = obj->get_object();
      qsrc = உச்சி.peek<vvp_queue_vec4>();
      எனில் (qsrc) {
	    dsrc = 0;
	    திரும்பு;
      }
      vvp_darray* dany = உச்சி.peek<vvp_darray>();
      எனில் (dany && dynamic_cast<vvp_queue*>(dany) == 0) {
	    dsrc = dany;
	    திரும்பு;
      }
      எனில் (உச்சி.test_nil()) {
	    qsrc = புதிய vvp_queue_vec4();
	    vvp_object_t val(qsrc);
	    vvp_net_ptr_t ptr(net, 0);
	    vvp_send_object(ptr, val, thr->wt_context);
	    dsrc = 0;
	    திரும்பு;
      }
      /* e.g. real/string queue — not vec4 queue or plain darray */
      qsrc = 0;
      dsrc = 0;
}

நிலையான வெறுமை get_queue_or_darray_real_from_net(vthread_t thr, vvp_net_t* net,
					      vvp_queue_real*& qsrc,
					      vvp_darray*& dsrc)
{
      vvp_fun_signal_object* obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      ப்ரதிஜ்ஞா(obj);
      vvp_object_t உச்சி = obj->get_object();
      qsrc = உச்சி.peek<vvp_queue_real>();
      எனில் (qsrc) {
	    dsrc = 0;
	    திரும்பு;
      }
      vvp_darray* dany = உச்சி.peek<vvp_darray>();
      எனில் (dany && dynamic_cast<vvp_queue*>(dany) == 0) {
	    dsrc = dany;
	    திரும்பு;
      }
      எனில் (உச்சி.test_nil()) {
	    qsrc = புதிய vvp_queue_real();
	    vvp_object_t val(qsrc);
	    vvp_net_ptr_t ptr(net, 0);
	    vvp_send_object(ptr, val, thr->wt_context);
	    dsrc = 0;
	    திரும்பு;
      }
      /* e.g. real/string queue — not vec4 queue or plain darray */
      qsrc = 0;
      dsrc = 0;
}

நிலையான வெறுமை get_queue_or_darray_vec4_from_object(vvp_object_t src_obj,
						 vvp_queue_vec4*& qsrc,
						 vvp_darray*& dsrc)
{
      qsrc = src_obj.peek<vvp_queue_vec4>();
      எனில் (qsrc) {
	    dsrc = 0;
	    திரும்பு;
      }
      vvp_darray* dany = src_obj.peek<vvp_darray>();
      எனில் (dany && dynamic_cast<vvp_queue*>(dany) == 0) {
	    dsrc = dany;
	    திரும்பு;
      }
      qsrc = 0;
      dsrc = 0;
}

/*
 * The following are used to allow a common template to be written for
 * queue real/string/vec4 operations
 */
உள்வரி நிலையான வெறுமை எடு_மதிப்பு(vthread_t thr, இரட்டை&value, unsigned)
{
      value = thr->எடு_real();
}

உள்வரி நிலையான வெறுமை எடு_மதிப்பு(vthread_t thr, string&value, unsigned)
{
      value = thr->எடு_str();
}

உள்வரி நிலையான வெறுமை எடு_மதிப்பு(vthread_t thr, vvp_vector4_t&value, unsigned wid)
{
      value = thr->எடு_vec4();
      ப்ரதிஜ்ஞா(value.size() == wid);
}

/*
 * The following are used to allow the queue templates to print correctly.
 */
உள்வரி நிலையான string get_queue_type(இரட்டை&)
{
      திரும்பு "queue<real>";
}

உள்வரி நிலையான string get_queue_type(string&)
{
      திரும்பு "queue<string>";
}

உள்வரி நிலையான string get_queue_type(மாறிலி vvp_vector4_t&value)
{
      ostringstream buf;
      buf << "queue<vector[" << value.size() << "]>";
      string res = buf.str();
      திரும்பு res;
}

உள்வரி நிலையான வெறுமை print_queue_மதிப்பு(இரட்டை value)
{
      cerr << value;
}

உள்வரி நிலையான வெறுமை print_queue_மதிப்பு(மாறிலி string&value)
{
      cerr << "\"" << value << "\"";
}

உள்வரி நிலையான வெறுமை print_queue_மதிப்பு(மாறிலி vvp_vector4_t&value)
{
      cerr << value;
}

/*
 * The following are used to get a darray/queue default value.
 */
உள்வரி நிலையான வெறுமை dq_default(இரட்டை&value, unsigned)
{
      value = 0.0;
}

உள்வரி நிலையான வெறுமை dq_default(string&value, unsigned)
{
      value = "";
}

உள்வரி நிலையான வெறுமை dq_default(vvp_vector4_t&value, unsigned wid)
{
      value = vvp_vector4_t(wid);
}


template <வகுப்பு T> T coerce_to_width(மாறிலி T&that, unsigned width)
{
      எனில் (that.size() == width)
	    திரும்பு that;

      ப்ரதிஜ்ஞா(that.size() > width);
      T res (width);
      க்கு (unsigned idx = 0 ;  idx < width ;  idx += 1)
	    res.set_bit(idx, that.value(idx));

      திரும்பு res;
}

/* Explicitly define the vvp_vector4_t version of coerce_to_width(). */
template vvp_vector4_t coerce_to_width(மாறிலி vvp_vector4_t&that,
                                       unsigned width);


நிலையான வெறுமை multiply_array_imm(unsigned long*res, மாறிலி unsigned long*val,
			       unsigned சொற்கள், unsigned long imm)
{
      க்கு (unsigned idx = 0 ; idx < சொற்கள் ; idx += 1)
	    res[idx] = 0;

      க்கு (unsigned mul_idx = 0 ; mul_idx < சொற்கள் ; mul_idx += 1) {
	    unsigned long sum;
	    unsigned long tmp = multiply_with_carry(val[mul_idx], imm, sum);

	    unsigned long carry = 0;
	    res[mul_idx] = add_with_carry(res[mul_idx], tmp, carry);
	    க்கு (unsigned add_idx = mul_idx+1 ; add_idx < சொற்கள் ; add_idx += 1) {
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
நிலையான vvp_context_t vthread_alloc_context(__vpiScope*scope)
{
      ப்ரதிஜ்ஞா(scope->is_automatic());

      vvp_context_t context = scope->free_contexts;
      எனில் (context) {
            scope->free_contexts = vvp_get_next_context(context);
            க்கு (unsigned idx = 0 ; idx < scope->nitem ; idx += 1) {
                  scope->item[idx]->reset_instance(context);
            }
      } இல்லையெனில் {
            context = vvp_allocate_context(scope->nitem);
            க்கு (unsigned idx = 0 ; idx < scope->nitem ; idx += 1) {
                  scope->item[idx]->alloc_instance(context);
            }
      }

      vvp_set_next_context(context, scope->live_contexts);
      scope->live_contexts = context;

      திரும்பு context;
}

/*
 * Free a context previously allocated to a child thread by pushing it
 * onto the freed context stack. Remove it from the list of live contexts
 * in that scope.
 */
நிலையான வெறுமை vthread_free_context(vvp_context_t context, __vpiScope*scope)
{
      ப்ரதிஜ்ஞா(scope->is_automatic());
      ப்ரதிஜ்ஞா(context);

      எனில் (context == scope->live_contexts) {
            scope->live_contexts = vvp_get_next_context(context);
      } இல்லையெனில் {
            vvp_context_t tmp = scope->live_contexts;
            போது (context != vvp_get_next_context(tmp)) {
                  ப்ரதிஜ்ஞா(tmp);
                  tmp = vvp_get_next_context(tmp);
            }
            vvp_set_next_context(tmp, vvp_get_next_context(context));
      }

      vvp_set_next_context(context, scope->free_contexts);
      scope->free_contexts = context;
}

#ifdef CHECK_WITH_VALGRIND
வெறுமை contexts_delete(வகுப்பு __vpiScope*scope)
{
      vvp_context_t context = scope->free_contexts;

      போது (context) {
	    scope->free_contexts = vvp_get_next_context(context);
	    க்கு (unsigned idx = 0; idx < scope->nitem; idx += 1) {
		  scope->item[idx]->free_instance(context);
	    }
	    விடுவி(context);
	    context = scope->free_contexts;
      }
      விடுவி(scope->item);
}
#endif

/*
 * Create a new thread with the given start address.
 */
vthread_t vthread_new(vvp_code_t pc, __vpiScope*scope)
{
      vthread_t thr = புதிய கட்டமைப்பு vthread_s;
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
      க்கு (முழு idx = 4 ; idx < 8 ; idx += 1)
	    thr->flags[idx] = BIT4_X;

      scope->threads .insert(thr);
      திரும்பு thr;
}

#ifdef CHECK_WITH_VALGRIND
#எனில் 0
/*
 * These are not currently correct. If you use them you will get
 * double delete messages. There is still a leak related to a
 * waiting event that needs to be investigated.
 */

நிலையான வெறுமை wait_next_delete(vthread_t base)
{
      போது (base) {
	    vthread_t tmp = base->wait_next;
	    delete base;
	    base = tmp;
	    எனில் (base->waiting_for_event == 0) நிறுத்து;
      }
}

நிலையான வெறுமை child_delete(vthread_t base)
{
      போது (base) {
	    vthread_t tmp = base->child;
	    delete base;
	    base = tmp;
      }
}
#endif

வெறுமை vthreads_delete(வகுப்பு __vpiScope*scope)
{
      க்கு (std::set<vthread_t>::iterator cur = scope->threads.begin()
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
நிலையான வெறுமை vthread_reap(vthread_t thr)
{
      எனில் (! thr->children.empty()) {
	    க்கு (set<vthread_t>::iterator cur = thr->children.begin()
		       ; cur != thr->children.end() ; ++cur) {
		  vthread_t child = *cur;
		  ப்ரதிஜ்ஞா(child);
		  ப்ரதிஜ்ஞா(child->parent == thr);
		  child->parent = thr->parent;
	    }
      }
      எனில் (! thr->detached_children.empty()) {
	    க்கு (set<vthread_t>::iterator cur = thr->detached_children.begin()
		       ; cur != thr->detached_children.end() ; ++cur) {
		  vthread_t child = *cur;
		  ப்ரதிஜ்ஞா(child);
		  ப்ரதிஜ்ஞா(child->parent == thr);
		  ப்ரதிஜ்ஞா(child->i_am_detached);
		  child->parent = 0;
		  child->i_am_detached = 0;
	    }
      }
      எனில் (thr->parent) {
	      /* assert that the given element was removed. */
	    எனில் (thr->i_am_detached) {
		  size_t res = thr->parent->detached_children.erase(thr);
		  ப்ரதிஜ்ஞா(res == 1);
	    } இல்லையெனில் {
		  size_t res = thr->parent->children.erase(thr);
		  ப்ரதிஜ்ஞா(res == 1);
	    }
      }

      thr->parent = 0;

	// Remove myself from the containing scope if needed.
      thr->parent_scope->threads.erase(thr);

      thr->pc = codespace_null();

	/* If this thread is not scheduled, then is it safe to delete
	   it now. Otherwise, let the schedule event (which will
	   execute the thread at of_ZOMBIE) delete the object. */
      எனில் ((thr->is_scheduled == 0) && (thr->waiting_for_event == 0)) {
	    ப்ரதிஜ்ஞா(thr->children.empty());
	    ப்ரதிஜ்ஞா(thr->wait_next == 0);
	    எனில் (thr->delay_delete)
		  schedule_del_thr(thr);
	    இல்லையெனில்
		  vthread_delete(thr);
      }
}

வெறுமை vthread_delete(vthread_t thr)
{
      thr->cleanup();
      delete thr;
}

வெறுமை vthread_mark_scheduled(vthread_t thr)
{
      போது (thr != 0) {
	    ப்ரதிஜ்ஞா(thr->is_scheduled == 0);
	    thr->is_scheduled = 1;
	    thr = thr->wait_next;
      }
}

வெறுமை vthread_mark_final(vthread_t thr)
{
      /*
       * The behavior in a final thread is the same as in a function. Any
       * child thread will be executed immediately rather than being
       * scheduled.
       */
      போது (thr != 0) {
	    thr->i_am_in_function = 1;
	    thr = thr->wait_next;
      }
}

வெறுமை vthread_delay_delete()
{
      எனில் (running_thread)
	    running_thread->delay_delete = 1;
}

/*
 * This function runs each thread by fetching an instruction,
 * incrementing the PC, and executing the instruction. The thread may
 * be the head of a list, so each thread is run so far as possible.
 */
வெறுமை vthread_run(vthread_t thr)
{
      போது (thr != 0) {
	    vthread_t tmp = thr->wait_next;
	    thr->wait_next = 0;

	    ப்ரதிஜ்ஞா(thr->is_scheduled);
	    thr->is_scheduled = 0;

            running_thread = thr;

	    க்கு (;;) {
		  vvp_code_t cp = thr->pc;
		  thr->pc += 1;

		    /* Run the opcode implementation. If the execution of
		       the opcode returns false, then the thread is meant to
		       be paused, so break out of the loop. */
		  தருக்க rc = (cp->opcode)(thr, cp);
		  எனில் (rc == பொய்)
			நிறுத்து;
	    }

	    thr = tmp;
      }
      running_thread = 0;
}

/*
 * The CHUNK_LINK instruction is a special next pointer for linking
 * chunks of code space. It's like a simplified %jmp.
 */
தருக்க of_CHUNK_LINK(vthread_t thr, vvp_code_t code)
{
      ப்ரதிஜ்ஞா(code->cptr);
      thr->pc = code->cptr;
      திரும்பு உண்மை;
}

/*
 * This is called by an event functor to wake up all the threads on
 * its list. I in fact created that list in the %wait instruction, and
 * I also am certain that the waiting_for_event flag is set.
 */
வெறுமை vthread_schedule_list(vthread_t thr)
{
      க்கு (vthread_t cur = thr ;  cur ;  cur = cur->wait_next) {
	    ப்ரதிஜ்ஞா(cur->waiting_for_event);
	    cur->waiting_for_event = 0;
      }

      schedule_vthread(thr, 0);
}

vvp_context_t vthread_get_wt_context()
{
      எனில் (running_thread)
            திரும்பு running_thread->wt_context;
      இல்லையெனில்
            திரும்பு 0;
}

vvp_context_t vthread_get_rd_context()
{
      எனில் (running_thread)
            திரும்பு running_thread->rd_context;
      இல்லையெனில்
            திரும்பு 0;
}

vvp_context_item_t vthread_get_wt_context_item(unsigned context_idx)
{
      ப்ரதிஜ்ஞா(running_thread && running_thread->wt_context);
      திரும்பு vvp_get_context_item(running_thread->wt_context, context_idx);
}

vvp_context_item_t vthread_get_rd_context_item(unsigned context_idx)
{
      ப்ரதிஜ்ஞா(running_thread && running_thread->rd_context);
      திரும்பு vvp_get_context_item(running_thread->rd_context, context_idx);
}

/*
 * %abs/wr
 */
தருக்க of_ABS_WR(vthread_t thr, vvp_code_t)
{
      thr->தள்ளு_real( fabs(thr->எடு_real()) );
      திரும்பு உண்மை;
}

தருக்க of_ALLOC(vthread_t thr, vvp_code_t cp)
{
        /* Allocate a context. */
      vvp_context_t child_context = vthread_alloc_context(cp->scope);

        /* Push the allocated context onto the write context stack. */
      vvp_set_stacked_context(child_context, thr->wt_context);
      thr->wt_context = child_context;

      திரும்பு உண்மை;
}

தருக்க of_AND(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valb = thr->எடு_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();
      ப்ரதிஜ்ஞா(vala.size() == valb.size());
      vala &= valb;
      திரும்பு உண்மை;
}

/*
 * This function must ALWAYS be called with the val set to the right
 * size, and initialized with BIT4_0 bits. Certain optimizations rely
 * on that.
 */
நிலையான வெறுமை get_immediate_rval(vvp_code_t cp, vvp_vector4_t&val)
{
      uint32_t vala = cp->bit_idx[0];
      uint32_t valb = cp->bit_idx[1];
      unsigned wid  = cp->எண்;

      எனில் (valb == 0) {
	      // Special case: if the value is zero, we are done
	      // before we start.
	    எனில் (vala == 0) திரும்பு;

	      // Special case: The value has no X/Z bits, so we can
	      // use the setarray method to write the value all at once.
	    unsigned use_wid = 8*அளவு(unsigned long);
	    எனில் (wid < use_wid)
		  use_wid = wid;
	    unsigned long tmp[1];
	    tmp[0] = vala;
	    val.setarray(0, use_wid, tmp);
	    திரும்பு;
      }

	// The immediate value can be values bigger then 32 bits, but
	// only if the high bits are zero. So at most we need to run
	// through the loop below 32 times. Maybe less, if the target
	// width is less. We don't have to do anything special on that
	// because vala/valb bits will shift away so (vala|valb) will
	// turn to zero at or before 32 shifts.

      க்கு (unsigned idx = 0 ; idx < wid && (vala|valb) ; idx += 1) {
	    uint32_t ba = 0;
	      // Convert the vala/valb bits to a ba number that
	      // matches the encoding of the vvp_bit4_t enumeration.
	    ba = (valb & 1) << 1;
	    ba |= vala & 1;

	      // Note that the val is already pre-filled with BIT4_0
	      // bits, os we only need to set non-zero bit values.
	    எனில் (ba) val.set_bit(idx, (vvp_bit4_t)ba);

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
தருக்க of_ADD(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t r = thr->எடு_vec4();
	// Rather then pop l, use it directly from the stack. When we
	// assign to 'l', that will edit the top of the stack, which
	// replaces a pop and a pull.
      vvp_vector4_t&l = thr->peek_vec4();

      l.add(r);

      திரும்பு உண்மை;
}

/*
 * %addi <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
தருக்க of_ADDI(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->எண்;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_immediate_rval (cp, r);

      l.add(r);

      திரும்பு உண்மை;
}

/*
 * %add/wr
 */
தருக்க of_ADD_WR(vthread_t thr, vvp_code_t)
{
      இரட்டை r = thr->எடு_real();
      இரட்டை l = thr->எடு_real();
      thr->தள்ளு_real(l + r);
      திரும்பு உண்மை;
}

/* %assign/ar <array>, <delay>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory. <delay>
 * is the delay in simulation time. <bit> is the index register
 * containing the real value.
 */
தருக்க of_ASSIGN_AR(vthread_t thr, vvp_code_t cp)
{
      long adr = thr->சொற்கள்[3].w_int;
      unsigned delay = cp->bit_idx[0];
      இரட்டை value = thr->எடு_real();

      எனில் (adr >= 0) {
	    schedule_assign_array_word(cp->array, adr, value, delay);
      }

      திரும்பு உண்மை;
}

/* %assign/ar/d <array>, <delay_idx>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory.
 * <delay_idx> is the integer register that contains the delay value.
 */
தருக்க of_ASSIGN_ARD(vthread_t thr, vvp_code_t cp)
{
      long adr = thr->சொற்கள்[3].w_int;
      இரட்டை value = thr->எடு_real();

      எனில் (adr >= 0) {
	    vvp_time64_t delay = thr->சொற்கள்[cp->bit_idx[0]].w_uint;
	    schedule_assign_array_word(cp->array, adr, value, delay);
      }

      திரும்பு உண்மை;
}

/* %assign/ar/e <array>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory. <bit>
 * is the index register containing the real value. The event
 * information is contained in the thread event control registers
 * and is set with %evctl.
 */
தருக்க of_ASSIGN_ARE(vthread_t thr, vvp_code_t cp)
{
      long adr = thr->சொற்கள்[3].w_int;
      இரட்டை value = thr->எடு_real();

      எனில் (adr >= 0) {
	    எனில் (thr->ecount == 0) {
		  schedule_assign_array_word(cp->array, adr, value, 0);
	    } இல்லையெனில் {
		  schedule_evctl(cp->array, adr, value, thr->event,
		                 thr->ecount);
	    }
      }

      திரும்பு உண்மை;
}

/*
 * %assign/vec4 <var>, <delay>
 */
தருக்க of_ASSIGN_VEC4(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      unsigned delay = cp->bit_idx[0];
      மாறிலி vvp_vector4_t&val = thr->peek_vec4();

      schedule_assign_vector(ptr, 0, 0, val, delay);
      thr->எடு_vec4(1);
      திரும்பு உண்மை;
}

/*
 * Resizes a vector value for a partial assignment so that the value is fully
 * in-bounds of the target signal. Both `val` and `off` will be updated if
 * necessary.
 *
 * Returns false if the value is fully out-of-bounds and the assignment should
 * be skipped. Otherwise returns true.
 */
நிலையான தருக்க resize_rval_vec(vvp_vector4_t &val, int64_t &off,
			    unsigned முழு sig_wid)
{
      unsigned முழு wid = val.size();

        // Fully in bounds, most likely case
      எனில் (off >= 0 && (uint64_t)off + wid <= sig_wid)
	    திரும்பு உண்மை;

      unsigned முழு base = 0;
      எனில் (off >= 0) {
	      // Fully out-of-bounds
	    எனில் ((uint64_t)off >= sig_wid)
		  திரும்பு பொய்;
      } இல்லையெனில் {
	      // Fully out-of-bounds */
	    எனில் ((uint64_t)(-off) >= wid)
		  திரும்பு பொய்;

	      // If the index is below the vector, then only assign the high
	      // bits that overlap with the target
	    base = -off;
	    wid += off;
		off = 0;
      }

	// If the value is partly above the target, then only assign
	// the bits that overlap
      எனில் ((uint64_t)off + wid > sig_wid)
	    wid = sig_wid - (uint64_t)off;

      val = val.subvalue(base, wid);

      திரும்பு உண்மை;
}

/*
 * %assign/vec4/a/d <arr>, <offx>, <delx>
 */
தருக்க of_ASSIGN_VEC4_A_D(vthread_t thr, vvp_code_t cp)
{
      முழு off_idx = cp->bit_idx[0];
      முழு del_idx = cp->bit_idx[1];
      முழு adr_idx = 3;

      int64_t  off = off_idx ? thr->சொற்கள்[off_idx].w_int : 0;
      vvp_time64_t del = del_idx? thr->சொற்கள்[del_idx].w_uint : 0;
      long     adr = thr->சொற்கள்[adr_idx].w_int;

      vvp_vector4_t val = thr->எடு_vec4();

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      எனில் (thr->flags[4] == BIT4_1)
	    திரும்பு உண்மை;

      எனில் (!resize_rval_vec(val, off, cp->array->get_word_size()))
	    திரும்பு உண்மை;

      schedule_assign_array_word(cp->array, adr, off, val, del);

      திரும்பு உண்மை;
}

/*
 * %assign/vec4/a/e <arr>, <offx>
 */
தருக்க of_ASSIGN_VEC4_A_E(vthread_t thr, vvp_code_t cp)
{
      முழு off_idx = cp->bit_idx[0];
      முழு adr_idx = 3;

      int64_t  off = off_idx ? thr->சொற்கள்[off_idx].w_int : 0;
      long     adr = thr->சொற்கள்[adr_idx].w_int;

      vvp_vector4_t val = thr->எடு_vec4();

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      எனில் (thr->flags[4] == BIT4_1)
	    திரும்பு உண்மை;

      எனில் (!resize_rval_vec(val, off, cp->array->get_word_size()))
	    திரும்பு உண்மை;

      எனில் (thr->ecount == 0) {
	    schedule_assign_array_word(cp->array, adr, off, val, 0);
      } இல்லையெனில் {
	    schedule_evctl(cp->array, adr, val, off, thr->event, thr->ecount);
      }

      திரும்பு உண்மை;
}

/*
 * %assign/vec4/off/d <var>, <off>, <del>
 */
தருக்க of_ASSIGN_VEC4_OFF_D(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      unsigned off_index = cp->bit_idx[0];
      unsigned del_index = cp->bit_idx[1];
      vvp_vector4_t val = thr->எடு_vec4();

      int64_t off = thr->சொற்கள்[off_index].w_int;
      vvp_time64_t del = thr->சொற்கள்[del_index].w_uint;

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      எனில் (thr->flags[4] == BIT4_1)
	    திரும்பு உண்மை;

      vvp_signal_மதிப்பு*sig = dynamic_cast<vvp_signal_மதிப்பு*> (cp->net->fil);
      ப்ரதிஜ்ஞா(sig);

      எனில் (!resize_rval_vec(val, off, sig->மதிப்பு_size()))
	    திரும்பு உண்மை;

      schedule_assign_vector(ptr, off, sig->மதிப்பு_size(), val, del);
      திரும்பு உண்மை;
}

/*
 * %assign/vec4/off/e <var>, <off>
 */
தருக்க of_ASSIGN_VEC4_OFF_E(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      unsigned off_index = cp->bit_idx[0];
      vvp_vector4_t val = thr->எடு_vec4();

      int64_t off = thr->சொற்கள்[off_index].w_int;

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      எனில் (thr->flags[4] == BIT4_1)
	    திரும்பு உண்மை;

      vvp_signal_மதிப்பு*sig = dynamic_cast<vvp_signal_மதிப்பு*> (cp->net->fil);
      ப்ரதிஜ்ஞா(sig);

      எனில் (!resize_rval_vec(val, off, sig->மதிப்பு_size()))
	    திரும்பு உண்மை;

      எனில் (thr->ecount == 0) {
	    schedule_assign_vector(ptr, off, sig->மதிப்பு_size(), val, 0);
      } இல்லையெனில் {
	    schedule_evctl(ptr, val, off, sig->மதிப்பு_size(), thr->event, thr->ecount);
      }

      திரும்பு உண்மை;
}

/*
 * %assign/vec4/d <var-label> <delay>
 */
தருக்க of_ASSIGN_VEC4D(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      unsigned del_index = cp->bit_idx[0];
      vvp_time64_t del = thr->சொற்கள்[del_index].w_int;

      vvp_vector4_t value = thr->எடு_vec4();

      vvp_signal_மதிப்பு*sig = dynamic_cast<vvp_signal_மதிப்பு*> (cp->net->fil);
      ப்ரதிஜ்ஞா(sig);

      schedule_assign_vector(ptr, 0, sig->மதிப்பு_size(), value, del);

      திரும்பு உண்மை;
}

/*
 * %assign/vec4/e <var-label>
 */
தருக்க of_ASSIGN_VEC4E(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_vector4_t value = thr->எடு_vec4();

      vvp_signal_மதிப்பு*sig = dynamic_cast<vvp_signal_மதிப்பு*> (cp->net->fil);
      ப்ரதிஜ்ஞா(sig);

      எனில் (thr->ecount == 0) {
	    schedule_assign_vector(ptr, 0, sig->மதிப்பு_size(), value, 0);
      } இல்லையெனில் {
	    schedule_evctl(ptr, value, 0, sig->மதிப்பு_size(), thr->event, thr->ecount);
      }

      திரும்பு உண்மை;
}

/*
 * This is %assign/wr <vpi-label>, <delay>
 *
 * This assigns (after a delay) a value to a real variable. Use the
 * vpi_put_value function to do the assign, with the delay written
 * into the vpiInertialDelay carrying the desired delay.
 */
தருக்க of_ASSIGN_WR(vthread_t thr, vvp_code_t cp)
{
      unsigned delay = cp->bit_idx[0];
      இரட்டை value = thr->எடு_real();
      s_vpi_time del;

      del.type = vpiSimTime;
      vpip_time_to_timestruct(&del, delay);

      __vpiHandle*tmp = cp->handle;

      t_vpi_மதிப்பு val;
      val.format = vpiRealVal;
      val.value.real = value;
      vpi_put_மதிப்பு(tmp, &val, &del, vpiTransportDelay);

      திரும்பு உண்மை;
}

தருக்க of_ASSIGN_WRD(vthread_t thr, vvp_code_t cp)
{
      vvp_time64_t delay = thr->சொற்கள்[cp->bit_idx[0]].w_uint;
      இரட்டை value = thr->எடு_real();
      s_vpi_time del;

      del.type = vpiSimTime;
      vpip_time_to_timestruct(&del, delay);

      __vpiHandle*tmp = cp->handle;

      t_vpi_மதிப்பு val;
      val.format = vpiRealVal;
      val.value.real = value;
      vpi_put_மதிப்பு(tmp, &val, &del, vpiTransportDelay);

      திரும்பு உண்மை;
}

தருக்க of_ASSIGN_WRE(vthread_t thr, vvp_code_t cp)
{
      ப்ரதிஜ்ஞா(thr->event != 0);
      இரட்டை value = thr->எடு_real();
      __vpiHandle*tmp = cp->handle;

	// If the count is zero then just put the value.
      எனில் (thr->ecount == 0) {
	    t_vpi_மதிப்பு val;

	    val.format = vpiRealVal;
	    val.value.real = value;
	    vpi_put_மதிப்பு(tmp, &val, 0, vpiNoDelay);
      } இல்லையெனில் {
	    schedule_evctl(tmp, value, thr->event, thr->ecount);
      }

      thr->event = 0;
      thr->ecount = 0;

      திரும்பு உண்மை;
}

தருக்க of_BLEND(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t vala = thr->எடு_vec4();
      vvp_vector4_t valb = thr->எடு_vec4();
      ப்ரதிஜ்ஞா(vala.size() == valb.size());

      க்கு (unsigned idx = 0 ; idx < vala.size() ; idx += 1) {
	    எனில் (vala.value(idx) == valb.value(idx))
		  தொடர்;

	    vala.set_bit(idx, BIT4_X);
      }

      thr->தள்ளு_vec4(vala);
      திரும்பு உண்மை;
}

தருக்க of_BLEND_STR(vthread_t thr, vvp_code_t)
{
      string f = thr->எடு_str();
      string t = thr->எடு_str();
      thr->தள்ளு_str((t == f) ? t : "");
      திரும்பு உண்மை;
}

தருக்க of_BLEND_WR(vthread_t thr, vvp_code_t)
{
      இரட்டை f = thr->எடு_real();
      இரட்டை t = thr->எடு_real();
      thr->தள்ளு_real((t == f) ? t : 0.0);
      திரும்பு உண்மை;
}

தருக்க of_BREAKPOINT(vthread_t, vvp_code_t)
{
      திரும்பு உண்மை;
}

/*
 * %callf/void <code-label>, <scope-label>
 * Combine the %fork and %join steps for invoking a function.
 */
நிலையான தருக்க do_callf_void(vthread_t thr, vthread_t child)
{

      எனில் (child->parent_scope->is_automatic()) {
	      /* The context allocated for this child is the top entry
		 on the write context stack */
	    child->wt_context = thr->wt_context;
	    child->rd_context = thr->wt_context;
      }

        // Mark the function thread as a direct child of the current thread.
      child->parent = thr;
      thr->children.insert(child);
        // This should be the only child
      ப்ரதிஜ்ஞா(thr->children.size()==1);

        // Execute the function. This SHOULD run the function to completion,
        // but there are some exceptional situations where it won't.
      ப்ரதிஜ்ஞா(child->parent_scope->get_type_code() == vpiFunction);
      child->is_scheduled = 1;
      child->i_am_in_function = 1;
      vthread_run(child);
      running_thread = thr;

      எனில் (child->i_have_ended) {
	    do_join(thr, child);
	    திரும்பு உண்மை;
      } இல்லையெனில் {
	    thr->i_am_joining = 1;
	    திரும்பு பொய்;
      }
}

தருக்க of_CALLF_OBJ(vthread_t thr, vvp_code_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);
      திரும்பு do_callf_void(thr, child);

      // XXXX NOT IMPLEMENTED
}

தருக்க of_CALLF_REAL(vthread_t thr, vvp_code_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);

	// This is the return value. Push a place-holder value. The function
	// will replace this with the actual value using a %ret/real instruction.
      thr->தள்ளு_real(0.0);
      child->args_real.push_back(0);

      திரும்பு do_callf_void(thr, child);
}

தருக்க of_CALLF_STR(vthread_t thr, vvp_code_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);

      thr->தள்ளு_str("");
      child->args_str.push_back(0);

      திரும்பு do_callf_void(thr, child);
}

தருக்க of_CALLF_VEC4(vthread_t thr, vvp_code_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);

      vpiScopeFunction*scope_func = dynamic_cast<vpiScopeFunction*>(cp->scope);
      ப்ரதிஜ்ஞா(scope_func);

	// This is the return value. Push a place-holder value. The function
	// will replace this with the actual value using a %ret/real instruction.
      thr->தள்ளு_vec4(vvp_vector4_t(scope_func->get_func_width(), scope_func->get_func_init_val()));
      child->args_vec4.push_back(0);

      திரும்பு do_callf_void(thr, child);
}

தருக்க of_CALLF_VOID(vthread_t thr, vvp_code_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);
      திரும்பு do_callf_void(thr, child);
}

/*
 * The %cassign/link instruction connects a source node to a
 * destination node. The destination node must be a signal, as it is
 * marked with the source of the cassign so that it may later be
 * unlinked without specifically knowing the source that this
 * instruction used.
 */
தருக்க of_CASSIGN_LINK(vthread_t, vvp_code_t cp)
{
      vvp_net_t*dst = cp->net;
      vvp_net_t*src = cp->net2;

      vvp_fun_signal_base*sig
	    = dynamic_cast<vvp_fun_signal_base*>(dst->fun);
      ப்ரதிஜ்ஞா(sig);

	/* Any previous continuous assign should have been removed already. */
      ப்ரதிஜ்ஞா(sig->cassign_link == 0);

      sig->cassign_link = src;

	/* Link the output of the src to the port[1] (the cassign
	   port) of the destination. */
      vvp_net_ptr_t dst_ptr (dst, 1);
      src->link(dst_ptr);

      திரும்பு உண்மை;
}

/*
 * If there is an existing continuous assign linked to the destination
 * node, unlink it. This must be done before applying a new continuous
 * assign, otherwise the initial assigned value will be propagated to
 * any other nodes driven by the old continuous assign source.
 */
நிலையான வெறுமை cassign_unlink(vvp_net_t*dst)
{
      vvp_fun_signal_base*sig
	    = dynamic_cast<vvp_fun_signal_base*>(dst->fun);
      ப்ரதிஜ்ஞா(sig);

      எனில் (sig->cassign_link == 0)
	    திரும்பு;

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
தருக்க of_CASSIGN_VEC4(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      vvp_vector4_t value = thr->எடு_vec4();

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

	/* Set the value into port 1 of the destination. */
      vvp_net_ptr_t ptr (net, 1);
      vvp_send_vec4(ptr, value, 0);

      திரும்பு உண்மை;
}

/*
 * %cassign/vec4/off <var>, <off>
 */
தருக்க of_CASSIGN_VEC4_OFF(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      unsigned base_idx = cp->bit_idx[0];
      long base = thr->சொற்கள்[base_idx].w_int;
      vvp_vector4_t value = thr->எடு_vec4();
      unsigned wid = value.size();

      எனில் (thr->flags[4] == BIT4_1)
	    திரும்பு உண்மை;

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

      vvp_signal_மதிப்பு*sig = dynamic_cast<vvp_signal_மதிப்பு*> (net->fil);
      ப்ரதிஜ்ஞா(sig);

      எனில் (base < 0 && (wid <= (unsigned)-base))
	    திரும்பு உண்மை;

      எனில் (base >= (long)sig->மதிப்பு_size())
	    திரும்பு உண்மை;

      எனில் (base < 0) {
	    wid -= (unsigned) -base;
	    base = 0;
	    value.resize(wid);
      }

      எனில் (base+wid > sig->மதிப்பு_size()) {
	    wid = sig->மதிப்பு_size() - base;
	    value.resize(wid);
      }

      vvp_net_ptr_t ptr (net, 1);
      vvp_send_vec4_pv(ptr, value, base, sig->மதிப்பு_size(), 0);
      திரும்பு உண்மை;
}

தருக்க of_CASSIGN_WR(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net  = cp->net;
      இரட்டை value = thr->எடு_real();

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

	/* Set the value into port 1 of the destination. */
      vvp_net_ptr_t ptr (net, 1);
      vvp_send_real(ptr, value, 0);

      திரும்பு உண்மை;
}

/*
 * %cast2
 */
தருக்க of_CAST2(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      unsigned wid = val.size();

      க்கு (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    தேர்வு (val.value(idx)) {
		நிலை BIT4_0:
		நிலை BIT4_1:
		  நிறுத்து;
		இயல்பு:
		  val.set_bit(idx, BIT4_0);
		  நிறுத்து;
	    }
      }

      திரும்பு உண்மை;
}

தருக்க do_cast_vec_dar(vthread_t thr, vvp_code_t cp, தருக்க as_vec4)
{
      unsigned wid = cp->எண்;

      vvp_object_t obj;
      thr->எடு_object(obj);

      vvp_darray*darray = obj.peek<vvp_darray>();
      ப்ரதிஜ்ஞா(darray);

      vvp_vector4_t vec = darray->get_bitstream(as_vec4);
      எனில் (vec.size() != wid) {
	    cerr << thr->get_fileline()
	         << "VVP error: size mismatch when casting dynamic array to vector." << endl;
            thr->தள்ளு_vec4(vvp_vector4_t(wid));
            schedule_stop(0);
            திரும்பு பொய்;
      }
      thr->தள்ளு_vec4(vec);
      திரும்பு உண்மை;
}

/*
 * %cast/vec2/dar <wid>
 */
தருக்க of_CAST_VEC2_DAR(vthread_t thr, vvp_code_t cp)
{
      திரும்பு do_cast_vec_dar(thr, cp, பொய்);
}

/*
 * %cast/vec4/dar <wid>
 */
தருக்க of_CAST_VEC4_DAR(vthread_t thr, vvp_code_t cp)
{
      திரும்பு do_cast_vec_dar(thr, cp, உண்மை);
}

/*
 * %cast/vec4/str <wid>
 */
தருக்க of_CAST_VEC4_STR(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->எண்;
      string str = thr->எடு_str();

      vvp_vector4_t vec(wid, BIT4_0);

      எனில் (wid != 8*str.length()) {
	    cerr << thr->get_fileline()
	         << "VVP error: size mismatch when casting string to vector." << endl;
            thr->தள்ளு_vec4(vec);
            schedule_stop(0);
            திரும்பு பொய்;
      }

      unsigned sdx = 0;
      unsigned vdx = wid;
      போது (vdx > 0) {
            எழுத்து ch = str[sdx++];
            vdx -= 8;
            க்கு (unsigned bdx = 0; bdx < 8; bdx += 1) {
                  எனில் (ch & 1)
                        vec.set_bit(vdx+bdx, BIT4_1);
                  ch >>= 1;
            }
      }

      thr->தள்ளு_vec4(vec);
      திரும்பு உண்மை;
}

நிலையான வெறுமை do_CMPE(vthread_t thr, மாறிலி vvp_vector4_t&lval, மாறிலி vvp_vector4_t&rval)
{
      ப்ரதிஜ்ஞா(rval.size() == lval.size());

      எனில் (lval.has_xz() || rval.has_xz()) {

	    unsigned wid = lval.size();
	    vvp_bit4_t eq  = BIT4_1;
	    vvp_bit4_t eeq = BIT4_1;

	    க்கு (unsigned idx = 0 ; idx < wid ; idx += 1) {
		  vvp_bit4_t lv = lval.value(idx);
		  vvp_bit4_t rv = rval.value(idx);

		  எனில் (lv != rv)
			eeq = BIT4_0;

		  எனில் (eq==BIT4_1 && (bit4_is_xz(lv) || bit4_is_xz(rv)))
			eq = BIT4_X;
		  எனில் ((lv == BIT4_0) && (rv==BIT4_1))
			eq = BIT4_0;
		  எனில் ((lv == BIT4_1) && (rv==BIT4_0))
			eq = BIT4_0;

		  எனில் (eq == BIT4_0)
			நிறுத்து;
	    }

	    thr->flags[4] = eq;
	    thr->flags[6] = eeq;

      } இல்லையெனில் {
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
தருக்க of_CMPE(vthread_t thr, vvp_code_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      மாறிலி vvp_vector4_t&rval = thr->peek_vec4(0);
      மாறிலி vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPE(thr, lval, rval);

      thr->எடு_vec4(2);
      திரும்பு உண்மை;
}

தருக்க of_CMPNE(vthread_t thr, vvp_code_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      மாறிலி vvp_vector4_t&rval = thr->peek_vec4(0);
      மாறிலி vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];
      thr->flags[6] =  ~thr->flags[6];

      thr->எடு_vec4(2);
      திரும்பு உண்மை;
}

/*
 * %cmpi/e <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
தருக்க of_CMPIE(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->எண்;

      மாறிலி vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_immediate_rval (cp, rval);

      do_CMPE(thr, lval, rval);

      thr->எடு_vec4(1);
      திரும்பு உண்மை;
}

தருக்க of_CMPINE(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->எண்;

      மாறிலி vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_immediate_rval (cp, rval);

      do_CMPE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];
      thr->flags[6] =  ~thr->flags[6];

      thr->எடு_vec4(1);
      திரும்பு உண்மை;
}



நிலையான வெறுமை do_CMPS(vthread_t thr, மாறிலி vvp_vector4_t&lval, மாறிலி vvp_vector4_t&rval)
{
      ப்ரதிஜ்ஞா(rval.size() == lval.size());

	// If either value has XZ bits, then the eq and lt values are
	// known already to be X. Just calculate the eeq result as a
	// special case and short circuit the rest of the compare.
      எனில் (lval.has_xz() || rval.has_xz()) {
	    thr->flags[4] = BIT4_X; // eq
	    thr->flags[5] = BIT4_X; // lt
	    thr->flags[6] = lval.eeq(rval)? BIT4_1 : BIT4_0;
	    திரும்பு;
      }

	// Past this point, we know we are dealing only with fully
	// defined values.
      unsigned wid = lval.size();

      மாறிலி vvp_bit4_t sig1 = lval.value(wid-1);
      மாறிலி vvp_bit4_t sig2 = rval.value(wid-1);

	// If the lval is <0 and the rval is >=0, then we know the result.
      எனில் ((sig1 == BIT4_1) && (sig2 == BIT4_0)) {
	    thr->flags[4] = BIT4_0; // eq;
	    thr->flags[5] = BIT4_1; // lt;
	    thr->flags[6] = BIT4_0; // eeq
	    திரும்பு;
      }

	// If the lval is >=0 and the rval is <0, then we know the result.
      எனில் ((sig1 == BIT4_0) && (sig2 == BIT4_1)) {
	    thr->flags[4] = BIT4_0; // eq;
	    thr->flags[5] = BIT4_0; // lt;
	    thr->flags[6] = BIT4_0; // eeq
	    திரும்பு;
      }

	// The values have the same sign, so we have to look at the
	// actual value. Scan from the MSB down. As soon as we find a
	// bit that differs, we know the result.

      க்கு (unsigned idx = 1 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t lv = lval.value(wid-1-idx);
	    vvp_bit4_t rv = rval.value(wid-1-idx);

	    எனில் (lv == rv)
		  தொடர்;

	    thr->flags[4] = BIT4_0; // eq
	    thr->flags[6] = BIT4_0; // eeq

	    எனில் (lv==BIT4_0) {
		  thr->flags[5] = BIT4_1; // lt
	    } இல்லையெனில் {
		  thr->flags[5] = BIT4_0; // lt
	    }
	    திரும்பு;
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
தருக்க of_CMPS(vthread_t thr, vvp_code_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      மாறிலி vvp_vector4_t&rval = thr->peek_vec4(0);
      மாறிலி vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPS(thr, lval, rval);

      thr->எடு_vec4(2);
      திரும்பு உண்மை;
}

/*
 * %cmpi/s <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
தருக்க of_CMPIS(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->எண்;

      மாறிலி vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_immediate_rval (cp, rval);

      do_CMPS(thr, lval, rval);

      thr->எடு_vec4(1);
      திரும்பு உண்மை;
}

தருக்க of_CMPSTR(vthread_t thr, vvp_code_t)
{
      string re = thr->எடு_str();
      string le = thr->எடு_str();

      முழு rc = strcmp(le.c_str(), re.c_str());

      vvp_bit4_t eq;
      vvp_bit4_t lt;

      எனில் (rc == 0) {
	    eq = BIT4_1;
	    lt = BIT4_0;
      } இல்லையெனில் எனில் (rc < 0) {
	    eq = BIT4_0;
	    lt = BIT4_1;
      } இல்லையெனில் {
	    eq = BIT4_0;
	    lt = BIT4_0;
      }

      thr->flags[4] = eq;
      thr->flags[5] = lt;

      திரும்பு உண்மை;
}

நிலையான வெறுமை of_CMPU_the_hard_way(vthread_t thr, unsigned wid,
				 மாறிலி vvp_vector4_t&lval,
				 மாறிலி vvp_vector4_t&rval)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_bit4_t eeq = BIT4_1;

      க்கு (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t lv = lval.value(idx);
	    vvp_bit4_t rv = rval.value(idx);

	    எனில் (lv != rv)
		  eeq = BIT4_0;

	    எனில் (eq==BIT4_1 && (bit4_is_xz(lv) || bit4_is_xz(rv)))
		  eq = BIT4_X;
	    எனில் ((lv == BIT4_0) && (rv==BIT4_1))
		  eq = BIT4_0;
	    எனில் ((lv == BIT4_1) && (rv==BIT4_0))
		  eq = BIT4_0;

	    எனில் (eq == BIT4_0)
		  நிறுத்து;

      }

      thr->flags[4] = eq;
      thr->flags[5] = BIT4_X;
      thr->flags[6] = eeq;
}

நிலையான வெறுமை do_CMPU(vthread_t thr, மாறிலி vvp_vector4_t&lval, மாறிலி vvp_vector4_t&rval)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_bit4_t lt = BIT4_0;

      எனில் (rval.size() != lval.size()) {
	    cerr << thr->get_fileline()
	         << "VVP ERROR: %cmp/u operand width mismatch: lval=" << lval
		 << ", rval=" << rval << endl;
      }
      ப்ரதிஜ்ஞா(rval.size() == lval.size());
      unsigned wid = lval.size();

      unsigned long*larray = lval.subarray(0,wid);
      எனில் (larray == 0) திரும்பு of_CMPU_the_hard_way(thr, wid, lval, rval);

      unsigned long*rarray = rval.subarray(0,wid);
      எனில் (rarray == 0) {
	    delete[]larray;
	    திரும்பு of_CMPU_the_hard_way(thr, wid, lval, rval);
      }

      unsigned சொற்கள் = (wid+CPU_WORD_BITS-1) / CPU_WORD_BITS;

      க்கு (unsigned wdx = 0 ; wdx < சொற்கள் ; wdx += 1) {
	    எனில் (larray[wdx] == rarray[wdx])
		  தொடர்;

	    eq = BIT4_0;
	    எனில் (larray[wdx] < rarray[wdx])
		  lt = BIT4_1;
	    இல்லையெனில்
		  lt = BIT4_0;
      }

      delete[]larray;
      delete[]rarray;

      thr->flags[4] = eq;
      thr->flags[5] = lt;
      thr->flags[6] = eq;
}

தருக்க of_CMPU(vthread_t thr, vvp_code_t)
{

      மாறிலி vvp_vector4_t&rval = thr->peek_vec4(0);
      மாறிலி vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPU(thr, lval, rval);

      thr->எடு_vec4(2);
      திரும்பு உண்மை;
}

/*
 * %cmpi/u <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
தருக்க of_CMPIU(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->எண்;

      மாறிலி vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_immediate_rval (cp, rval);

      do_CMPU(thr, lval, rval);

      thr->எடு_vec4(1);
      திரும்பு உண்மை;
}


/*
 * %cmp/x
 */
தருக்க of_CMPX(vthread_t thr, vvp_code_t)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_vector4_t rval = thr->எடு_vec4();
      vvp_vector4_t lval = thr->எடு_vec4();

      ப்ரதிஜ்ஞா(rval.size() == lval.size());
      unsigned wid = lval.size();

      க்கு (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    vvp_bit4_t lv = lval.value(idx);
	    vvp_bit4_t rv = rval.value(idx);
	    எனில் ((lv != rv) && !bit4_is_xz(lv) && !bit4_is_xz(rv)) {
		  eq = BIT4_0;
		  நிறுத்து;
	    }
      }

      thr->flags[4] = eq;
      திரும்பு உண்மை;
}

நிலையான வெறுமை do_CMPWE(vthread_t thr, மாறிலி vvp_vector4_t&lval, மாறிலி vvp_vector4_t&rval)
{
      ப்ரதிஜ்ஞா(rval.size() == lval.size());

      எனில் (lval.has_xz() || rval.has_xz()) {

	    unsigned wid = lval.size();
	    vvp_bit4_t eq  = BIT4_1;

	    க்கு (unsigned idx = 0 ; idx < wid ; idx += 1) {
		  vvp_bit4_t lv = lval.value(idx);
		  vvp_bit4_t rv = rval.value(idx);

		  எனில் (bit4_is_xz(rv))
			தொடர்;
		  எனில் ((eq == BIT4_1) && bit4_is_xz(lv))
			eq = BIT4_X;
		  எனில் ((lv == BIT4_0) && (rv==BIT4_1))
			eq = BIT4_0;
		  எனில் ((lv == BIT4_1) && (rv==BIT4_0))
			eq = BIT4_0;

		  எனில் (eq == BIT4_0)
			நிறுத்து;
	    }

	    thr->flags[4] = eq;

      } இல்லையெனில் {
	      // If there are no XZ bits anywhere, then the results of
	      // ==? match the === test.
	    thr->flags[4] = (lval.eeq(rval)? BIT4_1 : BIT4_0);
      }
}

தருக்க of_CMPWE(vthread_t thr, vvp_code_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      மாறிலி vvp_vector4_t&rval = thr->peek_vec4(0);
      மாறிலி vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPWE(thr, lval, rval);

      thr->எடு_vec4(2);
      திரும்பு உண்மை;
}

தருக்க of_CMPWNE(vthread_t thr, vvp_code_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      மாறிலி vvp_vector4_t&rval = thr->peek_vec4(0);
      மாறிலி vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPWE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];

      thr->எடு_vec4(2);
      திரும்பு உண்மை;
}

தருக்க of_CMPWR(vthread_t thr, vvp_code_t)
{
      இரட்டை r = thr->எடு_real();
      இரட்டை l = thr->எடு_real();

      vvp_bit4_t eq = (l == r)? BIT4_1 : BIT4_0;
      vvp_bit4_t lt = (l <  r)? BIT4_1 : BIT4_0;

      thr->flags[4] = eq;
      thr->flags[5] = lt;

      திரும்பு உண்மை;
}

/*
 * %cmp/z
 */
தருக்க of_CMPZ(vthread_t thr, vvp_code_t)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_vector4_t rval = thr->எடு_vec4();
      vvp_vector4_t lval = thr->எடு_vec4();

      ப்ரதிஜ்ஞா(rval.size() == lval.size());
      unsigned wid = lval.size();

      க்கு (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    vvp_bit4_t lv = lval.value(idx);
	    vvp_bit4_t rv = rval.value(idx);
	    எனில் ((lv != rv) && (rv != BIT4_Z) && (lv != BIT4_Z)) {
		  eq = BIT4_0;
		  நிறுத்து;
	    }
      }

      thr->flags[4] = eq;
      திரும்பு உண்மை;
}

/*
 *  %concat/str;
 */
தருக்க of_CONCAT_STR(vthread_t thr, vvp_code_t)
{
      string text = thr->எடு_str();
      thr->peek_str(0).append(text);
      திரும்பு உண்மை;
}

/*
 *  %concati/str <string>;
 */
தருக்க of_CONCATI_STR(vthread_t thr, vvp_code_t cp)
{
      மாறிலி எழுத்து*text = cp->text;
      thr->peek_str(0).append(filter_string(text));
      திரும்பு உண்மை;
}

/*
 * %concat/vec4
 */
தருக்க of_CONCAT_VEC4(vthread_t thr, vvp_code_t)
{
      மாறிலி vvp_vector4_t&lsb = thr->peek_vec4(0);
      மாறிலி vvp_vector4_t&msb = thr->peek_vec4(1);

	// The result is the size of the top two vectors in the stack.
      vvp_vector4_t res (msb.size() + lsb.size(), BIT4_X);

	// Build up the result.
      res.set_vec(0, lsb);
      res.set_vec(lsb.size(), msb);

	// Rearrange the stack to pop the inputs and push the
	// result. Do that by actually popping only 1 stack position
	// and replacing the new top with the new value.
      thr->எடு_vec4(1);
      thr->peek_vec4() = res;

      திரும்பு உண்மை;
}

/*
 * %concati/vec4 <vala>, <valb>, <wid>
 *
 * Concat the immediate value to the LOW bits of the concatenation.
 * Get the HIGH bits from the top of the vec4 stack.
 */
தருக்க of_CONCATI_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned wid  = cp->எண்;

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
      திரும்பு உண்மை;
}

/*
 * %cvt/rv
 */
தருக்க of_CVT_RV(vthread_t thr, vvp_code_t)
{
      இரட்டை val;
      vvp_vector4_t val4 = thr->எடு_vec4();
      vector4_to_மதிப்பு(val4, val, பொய்);
      thr->தள்ளு_real(val);
      திரும்பு உண்மை;
}

/*
 * %cvt/rv/s
 */
தருக்க of_CVT_RV_S(vthread_t thr, vvp_code_t)
{
      இரட்டை val;
      vvp_vector4_t val4 = thr->எடு_vec4();
      vector4_to_மதிப்பு(val4, val, உண்மை);
      thr->தள்ளு_real(val);
      திரும்பு உண்மை;
}

/*
 * %cvt/sr <idx>
 * Pop the top value from the real stack, convert it to a 64bit signed
 * and save it to the indexed register.
 */
தருக்க of_CVT_SR(vthread_t thr, vvp_code_t cp)
{
      இரட்டை r = thr->எடு_real();
      thr->சொற்கள்[cp->bit_idx[0]].w_int = i64round(r);

      திரும்பு உண்மை;
}

/*
 * %cvt/ur <idx>
 */
தருக்க of_CVT_UR(vthread_t thr, vvp_code_t cp)
{
      இரட்டை r = thr->எடு_real();
      thr->சொற்கள்[cp->bit_idx[0]].w_uint = vlg_round_to_u64(r);

      திரும்பு உண்மை;
}

/*
 * %cvt/vr <wid>
 */
தருக்க of_CVT_VR(vthread_t thr, vvp_code_t cp)
{
      இரட்டை r = thr->எடு_real();
      unsigned wid = cp->எண்;

      vvp_vector4_t tmp(wid, r);
      thr->தள்ளு_vec4(tmp);
      திரும்பு உண்மை;
}

/*
 * This implements the %deassign instruction. All we do is write a
 * long(1) to port-3 of the addressed net. This turns off an active
 * continuous assign activated by %cassign/v
 */
தருக்க of_DEASSIGN(vthread_t, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      unsigned base  = cp->bit_idx[0];
      unsigned width = cp->bit_idx[1];

      vvp_signal_மதிப்பு*fil = dynamic_cast<vvp_signal_மதிப்பு*> (net->fil);
      ப்ரதிஜ்ஞா(fil);
      vvp_fun_signal_vec*sig = dynamic_cast<vvp_fun_signal_vec*>(net->fun);
      ப்ரதிஜ்ஞா(sig);

      எனில் (base >= fil->மதிப்பு_size()) திரும்பு உண்மை;
      எனில் (base+width > fil->மதிப்பு_size()) width = fil->மதிப்பு_size() - base;

      தருக்க full_sig = base == 0 && width == fil->மதிப்பு_size();

	// This is the net that is forcing me...
      எனில் (vvp_net_t*src = sig->cassign_link) {
	    எனில் (! full_sig) {
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
      எனில் (full_sig) {
	    sig->deassign();
      } இல்லையெனில் {
	    sig->deassign_pv(base, width);
      }

      திரும்பு உண்மை;
}

தருக்க of_DEASSIGN_WR(vthread_t, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_real*sig = dynamic_cast<vvp_fun_signal_real*>(net->fun);
      ப்ரதிஜ்ஞா(sig);

	// This is the net that is forcing me...
      எனில் (vvp_net_t*src = sig->cassign_link) {
	      // And this is the pointer to be removed.
	    vvp_net_ptr_t dst_ptr (net, 1);
	    src->unlink(dst_ptr);
	    sig->cassign_link = 0;
      }

      sig->deassign();

      திரும்பு உண்மை;
}

/*
 * %debug/thr
 */
தருக்க of_DEBUG_THR(vthread_t thr, vvp_code_t cp)
{
      மாறிலி எழுத்து*text = cp->text;
      thr->debug_dump(cerr, text);
      திரும்பு உண்மை;
}

/*
 * The delay takes two 32bit numbers to make up a 64bit time.
 *
 *   %delay <low>, <hig>
 */
தருக்க of_DELAY(vthread_t thr, vvp_code_t cp)
{
      vvp_time64_t கீழ் = cp->bit_idx[0];
      vvp_time64_t hig = cp->bit_idx[1];

      vvp_time64_t delay = (hig << 32) | கீழ்;

      எனில் (delay == 0) schedule_inactive(thr);
      இல்லையெனில் schedule_vthread(thr, delay);
      திரும்பு பொய்;
}

தருக்க of_DELAYX(vthread_t thr, vvp_code_t cp)
{
      vvp_time64_t delay;

      ப்ரதிஜ்ஞா(cp->எண் < vthread_s::சொற்கள்_மொத்தம்);
      delay = thr->சொற்கள்[cp->எண்].w_uint;
      எனில் (delay == 0) schedule_inactive(thr);
      இல்லையெனில் schedule_vthread(thr, delay);
      திரும்பு பொய்;
}

தருக்க of_DELETE_ELEM(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      int64_t idx_val = thr->சொற்கள்[3].w_int;
      எனில் (thr->flags[4] == BIT4_1) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with undefined index."
	         << endl;
	    திரும்பு உண்மை;
      }
      எனில் (idx_val < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with negative index."
	         << endl;
	    திரும்பு உண்மை;
      }
      size_t idx = idx_val;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      ப்ரதிஜ்ஞா(obj);

      vvp_queue*queue = obj->get_object().peek<vvp_queue>();
      எனில் (queue == 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping delete(" << idx
	         << ") on empty queue." << endl;
      } இல்லையெனில் {
	    size_t size = queue->get_size();
	    எனில் (idx >= size) {
		  cerr << thr->get_fileline()
		       << "Warning: skipping out of range delete(" << idx
		       << ") on queue of size " << size << "." << endl;
	    } இல்லையெனில் {
		  queue->erase(idx);
	    }
      }

      திரும்பு உண்மை;
}

/* %delete/obj <label>
 *
 * This operator works by assigning a nil to the target object. This
 * causes any value that might be there to be garbage collected, thus
 * deleting the object.
 */
தருக்க of_DELETE_OBJ(vthread_t thr, vvp_code_t cp)
{
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_send_object(ptr, vvp_object_t(), thr->wt_context);

      திரும்பு உண்மை;
}

/* %reverse/obj <label>
 * Reverse element order of the queue or dynamic array object at this net.
 */
தருக்க of_REVERSE_OBJ(vthread_t /*thr*/, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      ப்ரதிஜ்ஞா(obj);

      vvp_object_t stor = obj->get_object();
      எனில் (stor.test_nil()) திரும்பு உண்மை;

	/* Match other queue ops: peek queues before vvp_darray so concrete
	   queue types dispatch reverse_elems() correctly. */
      எனில் (vvp_queue* queue = stor.peek<vvp_queue>()) {
	    queue->reverse_elems();
	    திரும்பு உண்மை;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      ப்ரதிஜ்ஞா(darray);
      darray->reverse_elems();

      திரும்பு உண்மை;
}

/* %reverse/prop/obj <property-index>
 * Reverse queue/darray property storage inside the class object on stack top.
 */
தருக்க of_REVERSE_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->எண்;

      vvp_object_t& உச்சி = thr->peek_object();
      vvp_cobject*cobj = உச்சி.peek<vvp_cobject>();
      ப்ரதிஜ்ஞா(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      எனில் (arr_obj.test_nil()) திரும்பு உண்மை;

      எனில் (vvp_queue* queue = arr_obj.peek<vvp_queue>()) {
	    queue->reverse_elems();
	    திரும்பு உண்மை;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      ப்ரதிஜ்ஞா(darray);
      darray->reverse_elems();

      திரும்பு உண்மை;
}

/* %sort/obj — ascending sort */
தருக்க of_SORT_OBJ(vthread_t /*thr*/, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      ப்ரதிஜ்ஞா(obj);

      vvp_object_t stor = obj->get_object();
      எனில் (stor.test_nil()) திரும்பு உண்மை;

      எனில் (vvp_queue* queue = stor.peek<vvp_queue>()) {
	    queue->sort_elems(உண்மை);
	    திரும்பு உண்மை;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      ப்ரதிஜ்ஞா(darray);
      darray->sort_elems(உண்மை);

      திரும்பு உண்மை;
}

/* %sort/prop/obj */
தருக்க of_SORT_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->எண்;

      vvp_object_t& உச்சி = thr->peek_object();
      vvp_cobject*cobj = உச்சி.peek<vvp_cobject>();
      ப்ரதிஜ்ஞா(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      எனில் (arr_obj.test_nil()) திரும்பு உண்மை;

      எனில் (vvp_queue* queue = arr_obj.peek<vvp_queue>()) {
	    queue->sort_elems(உண்மை);
	    திரும்பு உண்மை;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      ப்ரதிஜ்ஞா(darray);
      darray->sort_elems(உண்மை);

      திரும்பு உண்மை;
}

/* %rsort/obj — descending sort */
தருக்க of_RSORT_OBJ(vthread_t /*thr*/, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      ப்ரதிஜ்ஞா(obj);

      vvp_object_t stor = obj->get_object();
      எனில் (stor.test_nil()) திரும்பு உண்மை;

      எனில் (vvp_queue* queue = stor.peek<vvp_queue>()) {
	    queue->sort_elems(பொய்);
	    திரும்பு உண்மை;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      ப்ரதிஜ்ஞா(darray);
      darray->sort_elems(பொய்);

      திரும்பு உண்மை;
}

/* %rsort/prop/obj */
தருக்க of_RSORT_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->எண்;

      vvp_object_t& உச்சி = thr->peek_object();
      vvp_cobject*cobj = உச்சி.peek<vvp_cobject>();
      ப்ரதிஜ்ஞா(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      எனில் (arr_obj.test_nil()) திரும்பு உண்மை;

      எனில் (vvp_queue* queue = arr_obj.peek<vvp_queue>()) {
	    queue->sort_elems(பொய்);
	    திரும்பு உண்மை;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      ப்ரதிஜ்ஞா(darray);
      darray->sort_elems(பொய்);

      திரும்பு உண்மை;
}

/* %shuffle/obj */
தருக்க of_SHUFFLE_OBJ(vthread_t /*thr*/, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      ப்ரதிஜ்ஞா(obj);

      vvp_object_t stor = obj->get_object();
      எனில் (stor.test_nil()) திரும்பு உண்மை;

      எனில் (vvp_queue* queue = stor.peek<vvp_queue>()) {
	    queue->shuffle_elems();
	    திரும்பு உண்மை;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      ப்ரதிஜ்ஞா(darray);
      darray->shuffle_elems();

      திரும்பு உண்மை;
}

/* %shuffle/prop/obj */
தருக்க of_SHUFFLE_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->எண்;

      vvp_object_t& உச்சி = thr->peek_object();
      vvp_cobject*cobj = உச்சி.peek<vvp_cobject>();
      ப்ரதிஜ்ஞா(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      எனில் (arr_obj.test_nil()) திரும்பு உண்மை;

      எனில் (vvp_queue* queue = arr_obj.peek<vvp_queue>()) {
	    queue->shuffle_elems();
	    திரும்பு உண்மை;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      ப்ரதிஜ்ஞா(darray);
      darray->shuffle_elems();

      திரும்பு உண்மை;
}

/* %delete/tail <label>, idx
 *
 * Remove all elements after the one specified.
 */
தருக்க of_DELETE_TAIL(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      ப்ரதிஜ்ஞா(obj);

      vvp_queue*queue = obj->get_object().peek<vvp_queue>();
      ப்ரதிஜ்ஞா(queue);

      unsigned idx = thr->சொற்கள்[cp->bit_idx[0]].w_int;
      queue->erase_tail(idx);

      திரும்பு உண்மை;
}

நிலையான தருக்க do_disable(vthread_t thr, vthread_t match)
{
      தருக்க flag = பொய்;

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
      போது (! thr->children.empty()) {

	    vthread_t tmp = *(thr->children.begin());
	    ப்ரதிஜ்ஞா(tmp);
	    ப்ரதிஜ்ஞா(tmp->parent == thr);
	    thr->i_am_joining = 0;
	    எனில் (do_disable(tmp, match))
		  flag = உண்மை;

	    vthread_reap(tmp);
      }

      vthread_t parent = thr->parent;
      எனில் (parent && parent->i_am_joining) {
	      // If a parent is waiting in a %join, wake it up. Note
	      // that it is possible to be waiting in a %join yet
	      // already scheduled if multiple child threads are
	      // ending. So check if the thread is already scheduled
	      // before scheduling it again.
	    parent->i_am_joining = 0;
	    எனில் (! parent->i_have_ended)
		  schedule_vthread(parent, 0, உண்மை);

	    do_join(parent, thr);

      } இல்லையெனில் எனில் (parent) {
	      /* If the parent is yet to %join me, let its %join
		 do the reaping. */
	      //assert(tmp->is_scheduled == 0);

      } இல்லையெனில் {
	      /* No parent at all. Goodbye. */
	    vthread_reap(thr);
      }

      திரும்பு flag || (thr == match);
}

/*
 * Implement the %disable instruction by scanning the target scope for
 * all the target threads. Kill the target threads and wake up a
 * parent that is attempting a %join.
 */
தருக்க of_DISABLE(vthread_t thr, vvp_code_t cp)
{
      __vpiScope*scope = static_cast<__vpiScope*>(cp->handle);

      தருக்க disabled_myself_flag = பொய்;

      போது (! scope->threads.empty()) {
	    set<vthread_t>::iterator cur = scope->threads.begin();

	    எனில் (do_disable(*cur, thr))
		  disabled_myself_flag = உண்மை;
      }

      திரும்பு ! disabled_myself_flag;
}

/*
 * Similar to `of_DISABLE`. But will only disable a single thread of the
 * specified scope. The disabled thread will be the thread closest to the
 * current thread in thread hierarchy. This can either be the current thread,
 * either the thread itself or one of its parents.
 * This is used for SystemVerilog flow control instructions like `return`,
 * `continue` and `break`.
 */

தருக்க of_DISABLE_FLOW(vthread_t thr, vvp_code_t cp)
{
      மாறிலி __vpiScope*scope = static_cast<__vpiScope*>(cp->handle);
      vthread_t cur = thr;

      போது (cur && cur->parent_scope != scope)
	    cur = cur->parent;

      ப்ரதிஜ்ஞா(cur);
      எனில் (cur)
	    திரும்பு !do_disable(cur, thr);

      திரும்பு பொய்;
}

/*
 * Implement the %disable/fork (SystemVerilog) instruction by disabling
 * all the detached children of the given thread.
 */
தருக்க of_DISABLE_FORK(vthread_t thr, vvp_code_t)
{
	/* If a %disable/fork is being executed then the parent thread
	 * cannot be waiting in a join. */
      ப்ரதிஜ்ஞா(! thr->i_am_joining);

	/* There should be no active children to disable. */
      ப்ரதிஜ்ஞா(thr->children.empty());

	/* Disable any detached children. */
      போது (! thr->detached_children.empty()) {
	    vthread_t child = *(thr->detached_children.begin());
	    ப்ரதிஜ்ஞா(child);
	    ப்ரதிஜ்ஞா(child->parent == thr);
	      /* Disabling the children can never match the parent thread. */
	    தருக்க res = do_disable(child, thr);
	    ப்ரதிஜ்ஞா(! res);
	    vthread_reap(child);
      }

      திரும்பு உண்மை;
}

/*
 * This function divides a 2-word number {high, a} by a 1-word
 * number. Assume that high < b.
 */
நிலையான unsigned long divide2words(unsigned long a, unsigned long b,
				  unsigned long உயர்வு)
{
      unsigned long result = 0;
      போது (உயர்வு > 0) {
	    unsigned long tmp_result = ULONG_MAX / b;
	    unsigned long remain = ULONG_MAX % b;

	    remain += 1;
	    எனில் (remain >= b) {
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
	    result += tmp_result * உயர்வு;

	      // The new iteration starts with high*remain + a.
	    remain = multiply_with_carry(உயர்வு, remain, உயர்வு);
	    a += remain;
            எனில்(a < remain)
              உயர்வு += 1;

	      // Now result*b + {high,a} == the input {high,a}. It is
	      // possible that the new high >= 1. If so, it will
	      // certainly be less than high from the previous
	      // iteration. Do another iteration and it will shrink,
	      // eventually to 0.
      }

	// high is now 0, so a is the remaining remainder, so we can
	// finish off the integer divide with a simple a/b.

      திரும்பு result + a/b;
}

நிலையான unsigned long* divide_bits(unsigned long*ap, மாறிலி unsigned long*bp, unsigned wid)
{
	// Do all our work a cpu-word at a time. The "words" variable
	// is the number of words of the wid.
      unsigned சொற்கள் = (wid+CPU_WORD_BITS-1) / CPU_WORD_BITS;

      unsigned btop = சொற்கள்-1;
      போது (btop > 0 && bp[btop] == 0)
	    btop -= 1;

	// Detect divide by 0, and exit.
      எனில் (btop==0 && bp[0]==0)
	    திரும்பு 0;

	// The result array will eventually accumulate the result. The
	// diff array is a difference that we use in the intermediate.
      unsigned long*diff  = புதிய unsigned long[சொற்கள்];
      unsigned long*result= புதிய unsigned long[சொற்கள்];
      க்கு (unsigned idx = 0 ; idx < சொற்கள் ; idx += 1)
	    result[idx] = 0;

      க்கு (unsigned cur = சொற்கள்-btop ; cur > 0 ; cur -= 1) {
	    unsigned cur_ptr = cur-1;
	    unsigned long cur_res;
	    எனில் (ap[cur_ptr+btop] >= bp[btop]) {
		  unsigned long உயர்வு = 0;
		  எனில் (cur_ptr+btop+1 < சொற்கள்)
			உயர்வு = ap[cur_ptr+btop+1];
		  cur_res = divide2words(ap[cur_ptr+btop], bp[btop], உயர்வு);

	    } இல்லையெனில் எனில் (cur_ptr+btop+1 >= சொற்கள்) {
		  தொடர்;

	    } இல்லையெனில் எனில் (ap[cur_ptr+btop+1] == 0) {
		  தொடர்;

	    } இல்லையெனில் {
		  cur_res = divide2words(ap[cur_ptr+btop], bp[btop],
					 ap[cur_ptr+btop+1]);
	    }

	      // cur_res is a guesstimate of the result this far. It
	      // may be 1 too big. (But it will also be >0) Try it,
	      // and if the difference comes out negative, then adjust.

	      // diff = (bp * cur_res)  << cur_ptr;
	    multiply_array_imm(diff+cur_ptr, bp, சொற்கள்-cur_ptr, cur_res);
	      // ap -= diff
	    unsigned long carry = 1;
	    க்கு (unsigned idx = cur_ptr ; idx < சொற்கள் ; idx += 1)
		  ap[idx] = add_with_carry(ap[idx], ~diff[idx], carry);

	      // ap has the diff subtracted out of it. If cur_res was
	      // too large, then ap will turn negative. (We easily
	      // tell that ap turned negative by looking at
	      // carry&1. If it is 0, then it is *negative*.) In that
	      // case, we know that cur_res was too large by 1. Correct by
	      // adding 1b back in and reducing cur_res.
	    எனில் ((carry&1) == 0) {
		    // Keep adding b back in until the remainder
		    // becomes positive again.
		  செய் {
			cur_res -= 1;
			carry = 0;
			க்கு (unsigned idx = cur_ptr ; idx < சொற்கள் ; idx += 1)
			      ap[idx] = add_with_carry(ap[idx], bp[idx-cur_ptr], carry);
		  } போது (carry == 0);
	    }

	    result[cur_ptr] = cur_res;
      }

	// Now ap contains the remainder and result contains the
	// desired result. We should find that:
	//  input-a = bp * result + ap;

      delete[]diff;
      திரும்பு result;
}

/*
 * %div
 */
தருக்க of_DIV(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valb = thr->எடு_vec4();
      vvp_vector4_t vala = thr->எடு_vec4();

      ப்ரதிஜ்ஞா(vala.size()== valb.size());
      unsigned wid = vala.size();

      unsigned long*ap = vala.subarray(0, wid);
      எனில் (ap == 0) {
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->தள்ளு_vec4(tmp);
	    திரும்பு உண்மை;
      }

      unsigned long*bp = valb.subarray(0, wid);
      எனில் (bp == 0) {
	    delete[]ap;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->தள்ளு_vec4(tmp);
	    திரும்பு உண்மை;
      }

	// If the value fits in a single CPU word, then do it the easy way.
      எனில் (wid <= CPU_WORD_BITS) {
	    எனில் (bp[0] == 0) {
		  vvp_vector4_t tmp(wid, BIT4_X);
		  thr->தள்ளு_vec4(tmp);
	    } இல்லையெனில் {
		  ap[0] /= bp[0];
		  vala.setarray(0, wid, ap);
		  thr->தள்ளு_vec4(vala);
	    }
	    delete[]ap;
	    delete[]bp;
	    திரும்பு உண்மை;
      }

      unsigned long*result = divide_bits(ap, bp, wid);
      எனில் (result == 0) {
	    delete[]ap;
	    delete[]bp;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->தள்ளு_vec4(tmp);
	    திரும்பு உண்மை;
      }

	// Now ap contains the remainder and result contains the
	// desired result. We should find that:
	//  input-a = bp * result + ap;

      vala.setarray(0, wid, result);
      thr->தள்ளு_vec4(vala);
      delete[]ap;
      delete[]bp;
      delete[]result;

      திரும்பு உண்மை;
}


நிலையான வெறுமை negate_சொற்கள்(unsigned long*val, unsigned சொற்கள்)
{
      unsigned long carry = 1;
      க்கு (unsigned idx = 0 ; idx < சொற்கள் ; idx += 1)
	    val[idx] = add_with_carry(0, ~val[idx], carry);
}

/*
 * %div/s
 */
தருக்க of_DIV_S(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valb = thr->எடு_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      ப்ரதிஜ்ஞா(vala.size()== valb.size());
      unsigned wid = vala.size();
      unsigned சொற்கள் = (wid + CPU_WORD_BITS - 1) / CPU_WORD_BITS;

	// Get the values, left in right, in binary form. If there is
	// a problem with either (caused by an X or Z bit) then we
	// know right away that the entire result is X.
      unsigned long*ap = vala.subarray(0, wid);
      எனில் (ap == 0) {
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    திரும்பு உண்மை;
      }

      unsigned long*bp = valb.subarray(0, wid);
      எனில் (bp == 0) {
	    delete[]ap;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    திரும்பு உண்மை;
      }

	// Sign extend the bits in the array to fill out the array.
      unsigned long sign_mask = 0;
      எனில் (unsigned long sign_bits = (சொற்கள்*CPU_WORD_BITS) - wid) {
	    sign_mask = -1UL << (CPU_WORD_BITS-sign_bits);
	    எனில் (ap[சொற்கள்-1] & (sign_mask>>1))
		  ap[சொற்கள்-1] |= sign_mask;
	    எனில் (bp[சொற்கள்-1] & (sign_mask>>1))
		  bp[சொற்கள்-1] |= sign_mask;
      }

	// If the value fits in a single word, then use the native divide.
      எனில் (wid <= CPU_WORD_BITS) {
	    எனில் (bp[0] == 0) {
		  vvp_vector4_t tmp(wid, BIT4_X);
		  vala = tmp;
	    } இல்லையெனில் எனில் (((long)ap[0] == LONG_MIN) && ((long)bp[0] == -1)) {
		  vvp_vector4_t tmp(wid, BIT4_0);
		  tmp.set_bit(wid-1, BIT4_1);
		  vala = tmp;
	    } இல்லையெனில் {
		  long tmpa = (long) ap[0];
		  long tmpb = (long) bp[0];
		  long res = tmpa / tmpb;
		  ap[0] = ((unsigned long)res) & ~sign_mask;
		  vala.setarray(0, wid, ap);
	    }
	    delete[]ap;
	    delete[]bp;
	    திரும்பு உண்மை;
      }

	// We need to the actual division to positive integers. Make
	// them positive here, and remember the negations.
      தருக்க negate_flag = பொய்;
      எனில் ( ((long) ap[சொற்கள்-1]) < 0 ) {
	    negate_flag = உண்மை;
	    negate_சொற்கள்(ap, சொற்கள்);
      }
      எனில் ( ((long) bp[சொற்கள்-1]) < 0 ) {
	    negate_flag ^= உண்மை;
	    negate_சொற்கள்(bp, சொற்கள்);
      }

      unsigned long*result = divide_bits(ap, bp, wid);
      எனில் (result == 0) {
	    delete[]ap;
	    delete[]bp;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    திரும்பு உண்மை;
      }

      எனில் (negate_flag) {
	    negate_சொற்கள்(result, சொற்கள்);
      }

      result[சொற்கள்-1] &= ~sign_mask;

      vala.setarray(0, wid, result);
      delete[]ap;
      delete[]bp;
      delete[]result;
      திரும்பு உண்மை;
}

தருக்க of_DIV_WR(vthread_t thr, vvp_code_t)
{
      இரட்டை r = thr->எடு_real();
      இரட்டை l = thr->எடு_real();
      thr->தள்ளு_real(l / r);

      திரும்பு உண்மை;
}

/*
 * %dup/obj
 * %dup/real
 * %dup/vec4
 *
 * Push a duplicate of the object on the appropriate stack.
 */
தருக்க of_DUP_OBJ(vthread_t thr, vvp_code_t)
{
      vvp_object_t src = thr->peek_object();

        // If it is null push a new null object
      எனில் (src.test_nil())
	    thr->தள்ளு_object(vvp_object_t());
      இல்லையெனில்
	    thr->தள்ளு_object(src.duplicate());

      திரும்பு உண்மை;
}

தருக்க of_DUP_REAL(vthread_t thr, vvp_code_t)
{
      thr->தள்ளு_real(thr->peek_real(0));
      திரும்பு உண்மை;
}

தருக்க of_DUP_VEC4(vthread_t thr, vvp_code_t)
{
      thr->தள்ளு_vec4(thr->peek_vec4(0));
      திரும்பு உண்மை;
}

/*
 * This terminates the current thread. If there is a parent who is
 * waiting for me to die, then I schedule it. At any rate, I mark
 * myself as a zombie by setting my pc to 0.
 */
தருக்க of_END(vthread_t thr, vvp_code_t)
{
      ப்ரதிஜ்ஞா(! thr->waiting_for_event);
      thr->i_have_ended = 1;
      thr->pc = codespace_null();

	/* Fully detach any detached children. */
      போது (! thr->detached_children.empty()) {
	    vthread_t child = *(thr->detached_children.begin());
	    ப்ரதிஜ்ஞா(child);
	    ப்ரதிஜ்ஞா(child->parent == thr);
	    ப்ரதிஜ்ஞா(child->i_am_detached);
	    child->parent = 0;
	    child->i_am_detached = 0;
	    thr->detached_children.erase(thr->detached_children.begin());
      }

	/* It is an error to still have active children running at this
	 * point in time. They should have all been detached or joined. */
      ப்ரதிஜ்ஞா(thr->children.empty());

	/* If I have a parent who is waiting for me, then mark that I
	   have ended, and schedule that parent. Also, finish the
	   %join for the parent. */
      எனில் (!thr->i_am_detached && thr->parent && thr->parent->i_am_joining) {
	    vthread_t tmp = thr->parent;
	    ப்ரதிஜ்ஞா(! thr->i_am_detached);

	    tmp->i_am_joining = 0;
	    schedule_vthread(tmp, 0, உண்மை);
	    do_join(tmp, thr);
	    திரும்பு பொய்;
      }

	/* If this thread is not fully detached then remove it from the
	 * parents detached_children set and reap it. */
      எனில் (thr->i_am_detached) {
	    vthread_t tmp = thr->parent;
	    ப்ரதிஜ்ஞா(tmp);
	    size_t res = tmp->detached_children.erase(thr);
	    ப்ரதிஜ்ஞா(res == 1);
	      /* If the parent is waiting for the detached children to
	       * finish then the last detached child needs to tell the
	       * parent to wake up when it is finished. */
	    எனில் (tmp->i_am_waiting && tmp->detached_children.empty()) {
		  tmp->i_am_waiting = 0;
		  schedule_vthread(tmp, 0, உண்மை);
	    }
	      /* Fully detach this thread so it will be reaped below. */
	    thr->i_am_detached = 0;
	    thr->parent = 0;
      }

	/* If I have no parent, then no one can %join me and there is
	 * no reason to stick around. This can happen, for example if
	 * I am an ``initial'' thread. */
      எனில் (thr->parent == 0) {
	    vthread_reap(thr);
	    திரும்பு பொய்;
      }

	/* If I make it this far, then I have a parent who may wish
	   to %join me. Remain a zombie so that it can. */

      திரும்பு பொய்;
}

/*
 * %event <var-label>
 */
தருக்க of_EVENT(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_vector4_t tmp (1, BIT4_X);
      vvp_send_vec4(ptr, tmp, thr->wt_context);
      திரும்பு உண்மை;
}

/*
 * %event/nb <var-label>, <delay>
 */
தருக்க of_EVENT_NB(vthread_t thr, vvp_code_t cp)
{
      vvp_time64_t delay;

      delay = thr->சொற்கள்[cp->bit_idx[0]].w_uint;
      schedule_propagate_event(cp->net, delay);
      திரும்பு உண்மை;
}

தருக்க of_EVCTL(vthread_t thr, vvp_code_t cp)
{
      ப்ரதிஜ்ஞா(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      thr->ecount = thr->சொற்கள்[cp->bit_idx[0]].w_uint;
      திரும்பு உண்மை;
}
தருக்க of_EVCTLC(vthread_t thr, vvp_code_t)
{
      thr->event = 0;
      thr->ecount = 0;
      திரும்பு உண்மை;
}

தருக்க of_EVCTLI(vthread_t thr, vvp_code_t cp)
{
      ப்ரதிஜ்ஞா(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      thr->ecount = cp->bit_idx[0];
      திரும்பு உண்மை;
}

தருக்க of_EVCTLS(vthread_t thr, vvp_code_t cp)
{
      ப்ரதிஜ்ஞா(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      int64_t val = thr->சொற்கள்[cp->bit_idx[0]].w_int;
      எனில் (val < 0) val = 0;
      thr->ecount = val;
      திரும்பு உண்மை;
}

தருக்க of_FLAG_GET_VEC4(vthread_t thr, vvp_code_t cp)
{
      முழு flag = cp->எண்;
      ப்ரதிஜ்ஞா(flag < vthread_s::FLAGS_மொத்தம்);

      vvp_vector4_t val (1, thr->flags[flag]);
      thr->தள்ளு_vec4(val);

      திரும்பு உண்மை;
}

/*
 * %flag_inv <flag1>
 */
தருக்க of_FLAG_INV(vthread_t thr, vvp_code_t cp)
{
      முழு flag1 = cp->bit_idx[0];

      thr->flags[flag1] = ~ thr->flags[flag1];
      திரும்பு உண்மை;
}

/*
 * %flag_mov <flag1>, <flag2>
 */
தருக்க of_FLAG_MOV(vthread_t thr, vvp_code_t cp)
{
      முழு flag1 = cp->bit_idx[0];
      முழு flag2 = cp->bit_idx[1];

      thr->flags[flag1] = thr->flags[flag2];
      திரும்பு உண்மை;
}

/*
 * %flag_or <flag1>, <flag2>
 */
தருக்க of_FLAG_OR(vthread_t thr, vvp_code_t cp)
{
      முழு flag1 = cp->bit_idx[0];
      முழு flag2 = cp->bit_idx[1];

      thr->flags[flag1] = thr->flags[flag1] | thr->flags[flag2];
      திரும்பு உண்மை;
}

தருக்க of_FLAG_SET_IMM(vthread_t thr, vvp_code_t cp)
{
      முழு flag = cp->எண்;
      முழு vali = cp->bit_idx[0];

      ப்ரதிஜ்ஞா(flag < vthread_s::FLAGS_மொத்தம்);
      ப்ரதிஜ்ஞா(vali >= 0 && vali < 4);

      நிலையான மாறிலி vvp_bit4_t map_bit[4] = {BIT4_0, BIT4_1, BIT4_Z, BIT4_X};
      thr->flags[flag] = map_bit[vali];
      திரும்பு உண்மை;
}

தருக்க of_FLAG_SET_VEC4(vthread_t thr, vvp_code_t cp)
{
      முழு flag = cp->எண்;
      ப்ரதிஜ்ஞா(flag < vthread_s::FLAGS_மொத்தம்);

      மாறிலி vvp_vector4_t&val = thr->peek_vec4();
      thr->flags[flag] = val.value(0);
      thr->எடு_vec4(1);

      திரும்பு உண்மை;
}

/*
 * the %force/link instruction connects a source node to a
 * destination node. The destination node must be a signal, as it is
 * marked with the source of the force so that it may later be
 * unlinked without specifically knowing the source that this
 * instruction used.
 */
தருக்க of_FORCE_LINK(vthread_t, vvp_code_t cp)
{
      vvp_net_t*dst = cp->net;
      vvp_net_t*src = cp->net2;

      ப்ரதிஜ்ஞா(dst->fil);
      dst->fil->force_link(dst, src);

      திரும்பு உண்மை;
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
தருக்க of_FORCE_VEC4(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_vector4_t value = thr->எடு_vec4();

	/* Send the force value to the filter on the node. */

      ப்ரதிஜ்ஞா(net->fil);
      எனில் (value.size() != net->fil->filter_size())
	    value = coerce_to_width(value, net->fil->filter_size());

      net->force_vec4(value, vvp_vector2_t(vvp_vector2_t::FILL1, net->fil->filter_size()));

      திரும்பு உண்மை;
}

/*
 * %force/vec4/off <net>, <off>
 */
தருக்க of_FORCE_VEC4_OFF(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      unsigned base_idx = cp->bit_idx[0];
      long base = thr->சொற்கள்[base_idx].w_int;
      vvp_vector4_t value = thr->எடு_vec4();
      unsigned wid = value.size();

      ப்ரதிஜ்ஞா(net->fil);

      எனில் (thr->flags[4] == BIT4_1)
	    திரும்பு உண்மை;

	// This is the width of the target vector.
      unsigned use_size = net->fil->filter_size();

      எனில் (base >= (long)use_size)
	    திரும்பு உண்மை;
      எனில் (base < -(long)use_size)
	    திரும்பு உண்மை;

      எனில் ((base + wid) > use_size)
	    wid = use_size - base;

	// Make a mask of which bits are to be forced, 0 for unforced
	// bits and 1 for forced bits.
      vvp_vector2_t mask (vvp_vector2_t::FILL0, use_size);
      க்கு (unsigned idx = 0 ; idx < wid ; idx += 1)
	    mask.set_bit(base+idx, 1);

      vvp_vector4_t tmp (use_size, BIT4_Z);

	// vvp_net_t::force_vec4 propagates all the bits of the
	// forced vector value, regardless of the mask. This
	// ensures the unforced bits retain their current value.
      vvp_signal_மதிப்பு*sig = dynamic_cast<vvp_signal_மதிப்பு*>(net->fil);
      ப்ரதிஜ்ஞா(sig);
      sig->vec4_மதிப்பு(tmp);

      tmp.set_vec(base, value);

      net->force_vec4(tmp, mask);
      திரும்பு உண்மை;
}

/*
 * %force/vec4/off/d <net>, <off>, <del>
 */
தருக்க of_FORCE_VEC4_OFF_D(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      unsigned base_idx = cp->bit_idx[0];
      long base = thr->சொற்கள்[base_idx].w_int;

      unsigned delay_idx = cp->bit_idx[1];
      vvp_time64_t delay = thr->சொற்கள்[delay_idx].w_uint;

      vvp_vector4_t value = thr->எடு_vec4();

      ப்ரதிஜ்ஞா(net->fil);

      எனில் (thr->flags[4] == BIT4_1)
	    திரும்பு உண்மை;

	// This is the width of the target vector.
      unsigned use_size = net->fil->filter_size();

      எனில் (base >= (long)use_size)
	    திரும்பு உண்மை;
      எனில் (base < -(long)use_size)
	    திரும்பு உண்மை;

      schedule_force_vector(net, base, use_size, value, delay);
      திரும்பு உண்மை;
}

தருக்க of_FORCE_WR(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net  = cp->net;
      இரட்டை value = thr->எடு_real();

      net->force_real(value, vvp_vector2_t(vvp_vector2_t::FILL1, 1));

      திரும்பு உண்மை;
}

/*
 * The %fork instruction causes a new child to be created and pushed
 * in front of any existing child. This causes the new child to be
 * added to the list of children, and for me to be the parent of the
 * new child.
 */
தருக்க of_FORK(vthread_t thr, vvp_code_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);

      எனில் (cp->scope->is_automatic()) {
              /* The context allocated for this child is the top entry
                 on the write context stack. */
            child->wt_context = thr->wt_context;
            child->rd_context = thr->wt_context;
      }

      child->parent = thr;
      thr->children.insert(child);

      எனில் (thr->i_am_in_function) {
	    child->is_scheduled = 1;
	    child->i_am_in_function = 1;
	    vthread_run(child);
	    running_thread = thr;
      } இல்லையெனில் {
	    schedule_vthread(child, 0, உண்மை);
      }
      திரும்பு உண்மை;
}

தருக்க of_FREE(vthread_t thr, vvp_code_t cp)
{
        /* Pop the child context from the read context stack. */
      vvp_context_t child_context = thr->rd_context;
      thr->rd_context = vvp_get_stacked_context(child_context);

        /* Free the context. */
      vthread_free_context(child_context, cp->scope);

      திரும்பு உண்மை;
}

/*
 * %inv
 *
 * Logically, this pops a value, inverts is (Verilog style, with Z and
 * X converted to X) and pushes the result. We can more efficiently
 * just to the invert in place.
 */
தருக்க of_INV(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val.invert();
      திரும்பு உண்மை;
}


/*
 * Index registers, arithmetic.
 */

நிலையான உள்வரி int64_t get_as_64_bit(uint32_t கீழ்_32, uint32_t உயர்வு_32)
{
      int64_t கீழ் = கீழ்_32;
      int64_t res = உயர்வு_32;

      res <<= 32;
      res |= கீழ்;
      திரும்பு res;
}

தருக்க of_IX_ADD(vthread_t thr, vvp_code_t cp)
{
      thr->சொற்கள்[cp->எண்].w_int += get_as_64_bit(cp->bit_idx[0],
                                                    cp->bit_idx[1]);
      திரும்பு உண்மை;
}

தருக்க of_IX_SUB(vthread_t thr, vvp_code_t cp)
{
      thr->சொற்கள்[cp->எண்].w_int -= get_as_64_bit(cp->bit_idx[0],
                                                    cp->bit_idx[1]);
      திரும்பு உண்மை;
}

தருக்க of_IX_MUL(vthread_t thr, vvp_code_t cp)
{
      thr->சொற்கள்[cp->எண்].w_int *= get_as_64_bit(cp->bit_idx[0],
                                                    cp->bit_idx[1]);
      திரும்பு உண்மை;
}

தருக்க of_IX_LOAD(vthread_t thr, vvp_code_t cp)
{
      thr->சொற்கள்[cp->எண்].w_int = get_as_64_bit(cp->bit_idx[0],
                                                   cp->bit_idx[1]);
      திரும்பு உண்மை;
}

தருக்க of_IX_MOV(vthread_t thr, vvp_code_t cp)
{
      thr->சொற்கள்[cp->bit_idx[0]].w_int = thr->சொற்கள்[cp->bit_idx[1]].w_int;
      திரும்பு உண்மை;
}

தருக்க of_IX_GETV(vthread_t thr, vvp_code_t cp)
{
      unsigned index = cp->bit_idx[0];
      vvp_net_t*net = cp->net;

      vvp_signal_மதிப்பு*sig = dynamic_cast<vvp_signal_மதிப்பு*>(net->fil);
      எனில் (sig == 0) {
	    ப்ரதிஜ்ஞா(net->fil);
	    cerr << thr->get_fileline()
	         << "%%ix/getv error: Net arg not a vector signal? "
		 << typeid(*net->fil).name() << endl;
      }
      ப்ரதிஜ்ஞா(sig);

      vvp_vector4_t vec;
      sig->vec4_மதிப்பு(vec);
      தருக்க overflow_flag;
      uint64_t val = 0;
      தருக்க known_flag = vector4_to_மதிப்பு(vec, overflow_flag, val);

      எனில் (known_flag)
	    thr->சொற்கள்[index].w_uint = val;
      இல்லையெனில்
	    thr->சொற்கள்[index].w_uint = 0;

	/* Set bit 4 as a flag if the input is unknown. */
      thr->flags[4] = known_flag ? (overflow_flag ? BIT4_X : BIT4_0) : BIT4_1;

      திரும்பு உண்மை;
}

தருக்க of_IX_GETV_S(vthread_t thr, vvp_code_t cp)
{
      unsigned index = cp->bit_idx[0];
      vvp_net_t*net = cp->net;

      vvp_signal_மதிப்பு*sig = dynamic_cast<vvp_signal_மதிப்பு*>(net->fil);
      எனில் (sig == 0) {
	    ப்ரதிஜ்ஞா(net->fil);
	    cerr << thr->get_fileline()
	         << "%%ix/getv/s error: Net arg not a vector signal? "
		 << "fun=" << typeid(*net->fil).name()
		 << ", fil=" << (net->fil? typeid(*net->fil).name() : "<>")
		 << endl;
      }
      ப்ரதிஜ்ஞா(sig);

      vvp_vector4_t vec;
      sig->vec4_மதிப்பு(vec);
      int64_t val;
      தருக்க known_flag = vector4_to_மதிப்பு(vec, val, உண்மை, உண்மை);

      எனில் (known_flag)
	    thr->சொற்கள்[index].w_int = val;
      இல்லையெனில்
	    thr->சொற்கள்[index].w_int = 0;

	/* Set bit 4 as a flag if the input is unknown. */
      thr->flags[4] = known_flag? BIT4_0 : BIT4_1;

      திரும்பு உண்மை;
}

நிலையான uint64_t vec4_to_index(vthread_t thr, தருக்க signed_flag)
{
	// Get all the information we need about the vec4 vector, then
	// pop it away. We only need the bool bits and the length.
      மாறிலி vvp_vector4_t&val = thr->peek_vec4();
      unsigned val_size = val.size();
      unsigned long*bits = val.subarray(0, val_size, பொய்);
      thr->எடு_vec4(1);

	// If there are X/Z bits, then the subarray will give us a nil
	// pointer. Set a flag to indicate the error, and give up.
      எனில் (bits == 0) {
	    thr->flags[4] = BIT4_1;
	    திரும்பு 0;
      }

      uint64_t v = 0;
      thr->flags[4] = BIT4_0;

      ப்ரதிஜ்ஞா(அளவு(bits[0]) <= அளவு(v));

      v = 0;
      க்கு (unsigned idx = 0 ; idx < val_size ; idx += 8*அளவு(bits[0])) {
	    uint64_t tmp = bits[idx/8/அளவு(bits[0])];
	    எனில் (idx < 8*அளவு(v)) {
		  v |= tmp << idx;
	    } இல்லையெனில் {
		  தருக்க overflow = signed_flag && (v >> 63) ? ~tmp != 0 : tmp != 0;
		  எனில் (overflow) {
			thr->flags[4] = BIT4_X;
			நிறுத்து;
		  }
	    }
      }

	// Set the high bits that are not necessarily filled in by the
	// subarray function.
      எனில் (val_size < 8*அளவு(v)) {
	    எனில் (signed_flag && (v & (static_cast<uint64_t>(1)<<(val_size-1)))) {
		    // Propagate the sign bit...
		  v |= (~static_cast<uint64_t>(0)) << val_size;

	    } இல்லையெனில் {
		    // Fill with zeros.
		  v &= ~((~static_cast<uint64_t>(0)) << val_size);
	    }

      }

      delete[]bits;
      திரும்பு v;
}

/*
 * %ix/vec4 <idx>
 */
தருக்க of_IX_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned use_idx = cp->எண்;
      thr->சொற்கள்[use_idx].w_uint = vec4_to_index(thr, பொய்);
      திரும்பு உண்மை;
}

/*
 * %ix/vec4/s <idx>
 */
தருக்க of_IX_VEC4_S(vthread_t thr, vvp_code_t cp)
{
      unsigned use_idx = cp->எண்;
      thr->சொற்கள்[use_idx].w_uint = vec4_to_index(thr, உண்மை);
      திரும்பு உண்மை;
}

/*
 * The various JMP instruction work simply by pulling the new program
 * counter from the instruction and resuming. If the jump is
 * conditional, then test the bit for the expected value first.
 */
தருக்க of_JMP(vthread_t thr, vvp_code_t cp)
{
      thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      எனில் (schedule_stopped()) {
	    schedule_vthread(thr, 0, பொய்);
	    திரும்பு பொய்;
      }

      திரும்பு உண்மை;
}

/*
 * %jmp/0 <pc>, <flag>
 */
தருக்க of_JMP0(vthread_t thr, vvp_code_t cp)
{
      எனில் (thr->flags[cp->bit_idx[0]] == BIT4_0)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      எனில் (schedule_stopped()) {
	    schedule_vthread(thr, 0, பொய்);
	    திரும்பு பொய்;
      }

      திரும்பு உண்மை;
}

/*
 * %jmp/0xz <pc>, <flag>
 */
தருக்க of_JMP0XZ(vthread_t thr, vvp_code_t cp)
{
      எனில் (thr->flags[cp->bit_idx[0]] != BIT4_1)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      எனில் (schedule_stopped()) {
	    schedule_vthread(thr, 0, பொய்);
	    திரும்பு பொய்;
      }

      திரும்பு உண்மை;
}

/*
 * %jmp/1 <pc>, <flag>
 */
தருக்க of_JMP1(vthread_t thr, vvp_code_t cp)
{
      எனில் (thr->flags[cp->bit_idx[0]] == BIT4_1)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      எனில் (schedule_stopped()) {
	    schedule_vthread(thr, 0, பொய்);
	    திரும்பு பொய்;
      }

      திரும்பு உண்மை;
}

/*
 * %jmp/1xz <pc>, <flag>
 */
தருக்க of_JMP1XZ(vthread_t thr, vvp_code_t cp)
{
      எனில் (thr->flags[cp->bit_idx[0]] != BIT4_0)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      எனில் (schedule_stopped()) {
	    schedule_vthread(thr, 0, பொய்);
	    திரும்பு பொய்;
      }

      திரும்பு உண்மை;
}

/*
 * The %join instruction causes the thread to wait for one child
 * to die.  If a child is already dead (and a zombie) then I reap
 * it and go on. Otherwise, I mark myself as waiting in a join so that
 * children know to wake me when they finish.
 */

நிலையான வெறுமை do_join(vthread_t thr, vthread_t child)
{
      ப்ரதிஜ்ஞா(child->parent == thr);

        /* If the immediate child thread is in an automatic scope... */
      எனில் (child->wt_context) {
              /* and is the top level task/function thread... */
            எனில் (thr->wt_context != thr->rd_context) {
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

நிலையான தருக்க do_join_opcode(vthread_t thr)
{
      ப்ரதிஜ்ஞா( !thr->i_am_joining );
      ப்ரதிஜ்ஞா( !thr->children.empty());

	// Are there any children that have already ended? If so, then
	// join with that one.
      க்கு (set<vthread_t>::iterator cur = thr->children.begin()
		 ; cur != thr->children.end() ; ++cur) {
	    vthread_t curp = *cur;
	    எனில் (! curp->i_have_ended)
		  தொடர்;

	      // found something!
	    do_join(thr, curp);
	    திரும்பு உண்மை;
      }

	// Otherwise, tell my children to awaken me when they end,
	// then pause.
      thr->i_am_joining = 1;
      திரும்பு பொய்;
}

தருக்க of_JOIN(vthread_t thr, vvp_code_t)
{
      திரும்பு do_join_opcode(thr);
}

/*
 * This %join/detach <n> instruction causes the thread to detach
 * threads that were created by an earlier %fork.
 */
தருக்க of_JOIN_DETACH(vthread_t thr, vvp_code_t cp)
{
      unsigned long மொத்தம் = cp->எண்;

      ப்ரதிஜ்ஞா(மொத்தம் == thr->children.size());

      போது (! thr->children.empty()) {
	    vthread_t child = *thr->children.begin();
	    ப்ரதிஜ்ஞா(child->parent == thr);

	      // We cannot detach automatic tasks/functions within an
	      // automatic scope. If we try to do that, we might make
	      // a mess of the allocation of the context. Note that it
	      // is OK if the child context is distinct (See %exec_ufunc.)
	    ப்ரதிஜ்ஞா(child->wt_context==0 || thr->wt_context!=child->wt_context);
	    எனில் (child->i_have_ended) {
		    // If the child has already ended, then reap it.
		  vthread_reap(child);

	    } இல்லையெனில் {
		  size_t res = child->parent->children.erase(child);
		  ப்ரதிஜ்ஞா(res == 1);
		  child->i_am_detached = 1;
		  thr->detached_children.insert(child);
	    }
      }

      திரும்பு உண்மை;
}

/*
 * %load/ar <array-label>, <index>;
*/
தருக்க of_LOAD_AR(vthread_t thr, vvp_code_t cp)
{
      unsigned idx = cp->bit_idx[0];
      இரட்டை word;

	/* The result is 0.0 if the address is undefined. */
      எனில் (thr->flags[4] == BIT4_1) {
	    word = 0.0;
      } இல்லையெனில் {
	    unsigned adr = thr->சொற்கள்[idx].w_int;
	    word = cp->array->get_word_r(adr);
      }

      thr->தள்ளு_real(word);
      திரும்பு உண்மை;
}

template <typename ELEM>
நிலையான தருக்க load_dar(vthread_t thr, vvp_code_t cp)
{
      int64_t adr = thr->சொற்கள்[3].w_int;
      vvp_net_t*net = cp->net;
      ப்ரதிஜ்ஞா(net);

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      ப்ரதிஜ்ஞா(obj);

      vvp_darray*darray = obj->get_object().peek<vvp_darray>();

      ELEM word;
      எனில் (darray &&
          (adr >= 0) && (thr->flags[4] == BIT4_0)) // A defined address >= 0
	    darray->get_word(adr, word);
      இல்லையெனில்
	    dq_default(word, obj->size());

      vthread_தள்ளு(thr, word);
      திரும்பு உண்மை;
}

/*
 * %load/dar/r <array-label>;
 */
தருக்க of_LOAD_DAR_R(vthread_t thr, vvp_code_t cp)
{
      திரும்பு load_dar<இரட்டை>(thr, cp);
}

/*
 * %load/dar/str <array-label>;
 */
தருக்க of_LOAD_DAR_STR(vthread_t thr, vvp_code_t cp)
{
      திரும்பு load_dar<string>(thr, cp);
}

/*
 * %load/dar/vec4 <array-label>;
 */
தருக்க of_LOAD_DAR_VEC4(vthread_t thr, vvp_code_t cp)
{
      திரும்பு load_dar<vvp_vector4_t>(thr, cp);
}

/*
 * %load/prop/dar/vec4 <pid>, <wid>;
 *   Indexed read of queue/dynamic-array class property; object on stack,
 *   index in words[3] (same as %load/dar/vec4).
 */
தருக்க of_LOAD_PROP_DAR_VEC4(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->எண்;
      unsigned wid = cp->bit_idx[0];
      int64_t adr = thr->சொற்கள்[3].w_int;

      vvp_object_t& உச்சி = thr->peek_object();
      vvp_cobject*cobj = உச்சி.peek<vvp_cobject>();
      ப்ரதிஜ்ஞா(cobj);

      vvp_object_t pobj;
      cobj->get_object(pid, pobj, 0);
      vvp_darray*darray = pobj.peek<vvp_darray>();

      vvp_vector4_t word;
      எனில் (darray && (adr >= 0) && (thr->flags[4] == BIT4_0)) {
	    darray->get_word(adr, word);
      } இல்லையெனில் {
	    dq_default(word, wid);
      }

      thr->தள்ளு_vec4(word);
      திரும்பு உண்மை;
}

/*
 * %load/obj <var-label>
 */
தருக்க of_LOAD_OBJ(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      ப்ரதிஜ்ஞா(fun);

      vvp_object_t val = fun->get_object();
      thr->தள்ளு_object(val);

      திரும்பு உண்மை;
}

/*
 * %load/obja <index>
 *    Loads the object from array, using index <index> as the index
 *    value. If flags[4] == 1, the calculation of <index> may have
 *    failed, so push nil.
 */
தருக்க of_LOAD_OBJA(vthread_t thr, vvp_code_t cp)
{
      unsigned idx = cp->bit_idx[0];
      vvp_object_t word;

	/* The result is 0.0 if the address is undefined. */
      எனில் (thr->flags[4] == BIT4_1) {
	    ; // Return nil
      } இல்லையெனில் {
	    unsigned adr = thr->சொற்கள்[idx].w_int;
	    cp->array->get_word_obj(adr, word);
      }

      thr->தள்ளு_object(word);
      திரும்பு உண்மை;
}

/*
 * %load/real <var-label>
 */
தருக்க of_LOAD_REAL(vthread_t thr, vvp_code_t cp)
{
      __vpiHandle*tmp = cp->handle;
      t_vpi_மதிப்பு val;

      val.format = vpiRealVal;
      vpi_get_மதிப்பு(tmp, &val);

      thr->தள்ளு_real(val.value.real);

      திரும்பு உண்மை;
}

/*
 * %load/str <var-label>
 */
தருக்க of_LOAD_STR(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;


      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (net->fun);
      ப்ரதிஜ்ஞா(fun);

      மாறிலி string&val = fun->get_string();
      thr->தள்ளு_str(val);

      திரும்பு உண்மை;
}

தருக்க of_LOAD_STRA(vthread_t thr, vvp_code_t cp)
{
      unsigned idx = cp->bit_idx[0];
      string word;

      எனில் (thr->flags[4] == BIT4_1) {
	    word = "";
      } இல்லையெனில் {
	    unsigned adr = thr->சொற்கள்[idx].w_int;
	    word = cp->array->get_word_str(adr);
      }

      thr->தள்ளு_str(word);
      திரும்பு உண்மை;
}


/*
 * %load/vec4 <net>
 */
தருக்க of_LOAD_VEC4(vthread_t thr, vvp_code_t cp)
{
	// Push a placeholder onto the stack in order to reserve the
	// stack space. Use a reference for the stack top as a target
	// for the load.
      thr->தள்ளு_vec4(vvp_vector4_t());
      vvp_vector4_t&sig_மதிப்பு = thr->peek_vec4();

      vvp_net_t*net = cp->net;

	// For the %load to work, the functor must actually be a
	// signal functor. Only signals save their vector value.
      vvp_signal_மதிப்பு*sig = dynamic_cast<vvp_signal_மதிப்பு*> (net->fil);
      எனில் (sig == 0) {
	    cerr << thr->get_fileline()
	         << "%load/v error: Net arg not a signal? "
		 << (net->fil ? typeid(*net->fil).name() :
	                        typeid(*net->fun).name())
	         << endl;
	    ப்ரதிஜ்ஞா(sig);
	    திரும்பு உண்மை;
      }

	// Extract the value from the signal and directly into the
	// target stack position.
      sig->vec4_மதிப்பு(sig_மதிப்பு);

      திரும்பு உண்மை;
}

/*
 * %load/vec4a <arr>, <adrx>
 */
தருக்க of_LOAD_VEC4A(vthread_t thr, vvp_code_t cp)
{
      முழு adr_index = cp->bit_idx[0];

      long adr = thr->சொற்கள்[adr_index].w_int;

	// If flag[3] is set, then the calculation of the address
	// failed, and this load should return X instead of the actual
	// value.
      எனில் (thr->flags[4] == BIT4_1) {
	    vvp_vector4_t tmp (cp->array->get_word_size(), BIT4_X);
	    thr->தள்ளு_vec4(tmp);
	    திரும்பு உண்மை;
      }

      vvp_vector4_t tmp (cp->array->get_word(adr));
      thr->தள்ளு_vec4(tmp);
      திரும்பு உண்மை;
}

நிலையான வெறுமை do_verylong_mod(vvp_vector4_t&vala, மாறிலி vvp_vector4_t&valb,
			    தருக்க இடது_is_neg, தருக்க வலது_is_neg)
{
      தருக்க out_is_neg = இடது_is_neg;
      மாறிலி முழு len=vala.size();
      unsigned எழுத்து *a, *z, *t;
      a = புதிய unsigned எழுத்து[len+1];
      z = புதிய unsigned எழுத்து[len+1];
      t = புதிய unsigned எழுத்து[len+1];

      unsigned எழுத்து carry;
      unsigned எழுத்து temp;

      முழு mxa = -1, mxz = -1;
      முழு i;
      முழு current, copylen;

      unsigned lb_carry = இடது_is_neg? 1 : 0;
      unsigned rb_carry = வலது_is_neg? 1 : 0;
      க்கு (முழு idx = 0 ;  idx < len ;  idx += 1) {
	    unsigned lb = vala.value(idx);
	    unsigned rb = valb.value(idx);

	    எனில் ((lb | rb) & 2) {
		  delete []t;
		  delete []z;
		  delete []a;
		  vvp_vector4_t tmp(len, BIT4_X);
		  vala = tmp;
		  திரும்பு;
	    }

	    எனில் (இடது_is_neg) {
		  lb = (1-lb) + lb_carry;
		  lb_carry = (lb & ~1)? 1 : 0;
		  lb &= 1;
	    }
	    எனில் (வலது_is_neg) {
		  rb = (1-rb) + rb_carry;
		  rb_carry = (rb & ~1)? 1 : 0;
		  rb &= 1;
	    }

	    z[idx]=lb;
	    a[idx]=1-rb;	// for 2s complement add..
      }

      z[len]=0;
      a[len]=1;

      க்கு(i=len-1;i>=0;i--) {
	    எனில்(! a[i]) {
		  mxa=i;
		  நிறுத்து;
	    }
      }

      க்கு(i=len-1;i>=0;i--) {
	    எனில்(z[i]) {
		  mxz=i;
		  நிறுத்து;
	    }
      }

      எனில்((mxa>mxz)||(mxa==-1)) {
	    எனில்(mxa==-1) {
		  delete []t;
		  delete []z;
		  delete []a;
		  vvp_vector4_t tmpx (len, BIT4_X);
		  vala = tmpx;
		  திரும்பு;
	    }

	    செல் tally;
      }

      copylen = mxa + 2;
      current = mxz - mxa;

      போது(current > -1) {
	    carry = 1;
	    க்கு(i=0;i<copylen;i++) {
		  temp = z[i+current] + a[i] + carry;
		  t[i] = (temp&1);
		  carry = (temp>>1);
	    }

	    எனில்(carry) {
		  க்கு(i=0;i<copylen;i++) {
			z[i+current] = t[i];
		  }
	    }

	    current--;
      }

 tally:

      vvp_vector4_t tmp (len, BIT4_X);
      carry = out_is_neg? 1 : 0;
      க்கு (முழு idx = 0 ;  idx < len ;  idx += 1) {
	    unsigned ob = z[idx];
	    எனில் (out_is_neg) {
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

தருக்க of_MAX_WR(vthread_t thr, vvp_code_t)
{
      இரட்டை r = thr->எடு_real();
      இரட்டை l = thr->எடு_real();
      எனில் (r != r)
	    thr->தள்ளு_real(l);
      இல்லையெனில் எனில் (l != l)
	    thr->தள்ளு_real(r);
      இல்லையெனில் எனில் (r < l)
	    thr->தள்ளு_real(l);
      இல்லையெனில்
	    thr->தள்ளு_real(r);
      திரும்பு உண்மை;
}

தருக்க of_MIN_WR(vthread_t thr, vvp_code_t)
{
      இரட்டை r = thr->எடு_real();
      இரட்டை l = thr->எடு_real();
      எனில் (r != r)
	    thr->தள்ளு_real(l);
      இல்லையெனில் எனில் (l != l)
	    thr->தள்ளு_real(r);
      இல்லையெனில் எனில் (r < l)
	    thr->தள்ளு_real(r);
      இல்லையெனில்
	    thr->தள்ளு_real(l);
      திரும்பு உண்மை;
}

தருக்க of_MOD(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valb = thr->எடு_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      ப்ரதிஜ்ஞா(vala.size()==valb.size());
      unsigned wid = vala.size();

      எனில்(wid <= 8*அளவு(unsigned long long)) {
	    unsigned long long lv = 0, rv = 0;

	    க்கு (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		  unsigned long long lb = vala.value(idx);
		  unsigned long long rb = valb.value(idx);

		  எனில் ((lb | rb) & 2)
			செல் x_out;

		  lv |= (unsigned long long) lb << idx;
		  rv |= (unsigned long long) rb << idx;
	    }

	    எனில் (rv == 0)
		  செல் x_out;

	    lv %= rv;

	    க்கு (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		  vala.set_bit(idx, (lv&1)?BIT4_1 : BIT4_0);
		  lv >>= 1;
	    }

	    திரும்பு உண்மை;

      } இல்லையெனில் {
	    do_verylong_mod(vala, valb, பொய், பொய்);
	    திரும்பு உண்மை;
      }

 x_out:
      vala = vvp_vector4_t(wid, BIT4_X);
      திரும்பு உண்மை;
}

/*
 * %mod/s
 */
தருக்க of_MOD_S(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valb = thr->எடு_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      ப்ரதிஜ்ஞா(vala.size()==valb.size());
      unsigned wid = vala.size();

	/* Handle the case that we can fit the bits into a long-long
	   variable. We cause use native % to do the work. */
      எனில்(wid <= 8*அளவு(long long)) {
	    long long lv = 0, rv = 0;

	    க்கு (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		  long long lb = vala.value(idx);
		  long long rb = valb.value(idx);

		  எனில் ((lb | rb) & 2)
			செல் x_out;

		  lv |= (long long) lb << idx;
		  rv |= (long long) rb << idx;
	    }

	    எனில் (rv == 0)
		  செல் x_out;

	    எனில் ((lv == LLONG_MIN) && (rv == -1))
		  செல் zero_out;

	      /* Sign extend the signed operands when needed. */
	    எனில் (wid < 8*அளவு(long long)) {
		  எனில் (lv & (1LL << (wid-1)))
			lv |= -1ULL << wid;
		  எனில் (rv & (1LL << (wid-1)))
			rv |= -1ULL << wid;
	    }

	    lv %= rv;

	    க்கு (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		  vala.set_bit(idx, (lv&1)? BIT4_1 : BIT4_0);
		  lv >>= 1;
	    }

	      // vala is the top of the stack, edited in place, so we
	      // do not need to push the result.

	    திரும்பு உண்மை;

      } இல்லையெனில் {

	    தருக்க இடது_is_neg  = vala.value(vala.size()-1) == BIT4_1;
	    தருக்க வலது_is_neg = valb.value(valb.size()-1) == BIT4_1;
	    do_verylong_mod(vala, valb, இடது_is_neg, வலது_is_neg);
	    திரும்பு உண்மை;
      }

 x_out:
      vala = vvp_vector4_t(wid, BIT4_X);
      திரும்பு உண்மை;
 zero_out:
      vala = vvp_vector4_t(wid, BIT4_0);
      திரும்பு உண்மை;
}

/*
 * %mod/wr
 */
தருக்க of_MOD_WR(vthread_t thr, vvp_code_t)
{
      இரட்டை r = thr->எடு_real();
      இரட்டை l = thr->எடு_real();
      thr->தள்ளு_real(fmod(l,r));

      திரும்பு உண்மை;
}

/*
 * %neg/wr
 */
தருக்க of_NEG_WR(vthread_t thr, vvp_code_t)
{
      thr->poke_real(0, -thr->peek_real(0));
      திரும்பு உண்மை;
}

/*
 * %pad/s <wid>
 */
தருக்க of_PAD_S(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->எண்;

      vvp_vector4_t&val = thr->peek_vec4();
      unsigned old_size = val.size();

	// Sign-extend.
      எனில் (old_size < wid)
	    val.resize(wid, val.value(old_size-1));
      இல்லையெனில்
	    val.resize(wid);

      திரும்பு உண்மை;
}

/*
 * %pad/u <wid>
 */
தருக்க of_PAD_U(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->எண்;

      vvp_vector4_t&val = thr->peek_vec4();
      val.resize(wid, BIT4_0);

      திரும்பு உண்மை;
}

/*
 * %part/s <wid>
 * %part/u <wid>
 * Two values are popped from the stack. First, pop the canonical
 * index of the part select, and second is the value to be
 * selected. The result is pushed back to the stack.
 */
நிலையான தருக்க of_PART_base(vthread_t thr, vvp_code_t cp, தருக்க signed_flag)
{
      unsigned wid = cp->எண்;

      vvp_vector4_t base4 = thr->எடு_vec4();
      vvp_vector4_t&value = thr->peek_vec4();

      vvp_vector4_t res (wid, BIT4_X);

	// NOTE: This is treating the vector as signed. Is that correct?
      int32_t base;
      தருக்க மதிப்பு_ok = vector4_to_மதிப்பு(base4, base, signed_flag);
      எனில் (! மதிப்பு_ok) {
	    value = res;
	    திரும்பு உண்மை;
      }

      எனில் (base >= (int32_t)value.size()) {
	    value = res;
	    திரும்பு உண்மை;
      }

      எனில் ((base+(முழு)wid) <= 0) {
	    value = res;
	    திரும்பு உண்மை;
      }

      long vbase = 0;
      எனில் (base < 0) {
	    vbase = -base;
	    wid -= vbase;
	    base = 0;
      }

      எனில் ((base+wid) > value.size()) {
	    wid = value.size() - base;
      }

      res .set_vec(vbase, value.subvalue(base, wid));
      value = res;

      திரும்பு உண்மை;
}

தருக்க of_PART_S(vthread_t thr, vvp_code_t cp)
{
      திரும்பு of_PART_base(thr, cp, உண்மை);
}

தருக்க of_PART_U(vthread_t thr, vvp_code_t cp)
{
      திரும்பு of_PART_base(thr, cp, பொய்);
}

/*
 * %parti/s <wid>, <basei>, <base_wid>
 * %parti/u <wid>, <basei>, <base_wid>
 *
 * Pop the value to be selected. The result is pushed back to the stack.
 */
நிலையான தருக்க of_PARTI_base(vthread_t thr, vvp_code_t cp, தருக்க signed_flag)
{
      unsigned wid = cp->எண்;
      uint32_t base = cp->bit_idx[0];
      uint32_t bwid = cp->bit_idx[1];

      vvp_vector4_t&value = thr->peek_vec4();

      vvp_vector4_t res (wid, BIT4_X);

	// NOTE: This is treating the vector as signed. Is that correct?
      int32_t use_base = base;
      எனில் (signed_flag && bwid < 32 && (base&(1<<(bwid-1)))) {
	    use_base |= -1UL << bwid;
      }

      எனில் (use_base >= (int32_t)value.size()) {
	    value = res;
	    திரும்பு உண்மை;
      }

      எனில் ((use_base+(int32_t)wid) <= 0) {
	    value = res;
	    திரும்பு உண்மை;
      }

      long vbase = 0;
      எனில் (use_base < 0) {
	    vbase = -use_base;
	    wid -= vbase;
	    use_base = 0;
      }

      எனில் ((use_base+wid) > value.size()) {
	    wid = value.size() - use_base;
      }

      res .set_vec(vbase, value.subvalue(use_base, wid));
      value = res;

      திரும்பு உண்மை;
}

தருக்க of_PARTI_S(vthread_t thr, vvp_code_t cp)
{
      திரும்பு of_PARTI_base(thr, cp, உண்மை);
}

தருக்க of_PARTI_U(vthread_t thr, vvp_code_t cp)
{
      திரும்பு of_PARTI_base(thr, cp, பொய்);
}

/*
 * %mul
 */
தருக்க of_MUL(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t r = thr->எடு_vec4();
	// Rather then pop l, use it directly from the stack. When we
	// assign to 'l', that will edit the top of the stack, which
	// replaces a pop and a pull.
      vvp_vector4_t&l = thr->peek_vec4();

      l.mul(r);
      திரும்பு உண்மை;
}

/*
 * %muli <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
தருக்க of_MULI(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->எண்;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_immediate_rval (cp, r);

      l.mul(r);
      திரும்பு உண்மை;
}

தருக்க of_MUL_WR(vthread_t thr, vvp_code_t)
{
      இரட்டை r = thr->எடு_real();
      இரட்டை l = thr->எடு_real();
      thr->தள்ளு_real(l * r);

      திரும்பு உண்மை;
}

தருக்க of_NAND(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valr = thr->எடு_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      ப்ரதிஜ்ஞா(vall.size() == valr.size());

      vall &= valr;
      vall.invert();

      திரும்பு உண்மை;
}

/*
 * %new/cobj <vpi_object>
 * This creates a new cobject (SystemVerilog class object) and pushes
 * it to the stack. The <vpi-object> is a __vpiHandle that is a
 * vpiClassDefn object that defines the item to be created.
 */
தருக்க of_NEW_COBJ(vthread_t thr, vvp_code_t cp)
{
      மாறிலி class_type*defn = dynamic_cast<மாறிலி class_type*> (cp->handle);
      ப்ரதிஜ்ஞா(defn);

      vvp_object_t tmp (புதிய vvp_cobject(defn));
      thr->தள்ளு_object(tmp);
      திரும்பு உண்மை;
}

தருக்க of_NEW_DARRAY(vthread_t thr, vvp_code_t cp)
{
      மாறிலி எழுத்து*text = cp->text;
      size_t size = thr->சொற்கள்[cp->bit_idx[0]].w_int;
      unsigned word_wid;
      size_t n;

      vvp_object_t obj;
      எனில் (strcmp(text,"b8") == 0) {
	    obj = புதிய vvp_darray_atom<uint8_t>(size);
      } இல்லையெனில் எனில் (strcmp(text,"b16") == 0) {
	    obj = புதிய vvp_darray_atom<uint16_t>(size);
      } இல்லையெனில் எனில் (strcmp(text,"b32") == 0) {
	    obj = புதிய vvp_darray_atom<uint32_t>(size);
      } இல்லையெனில் எனில் (strcmp(text,"b64") == 0) {
	    obj = புதிய vvp_darray_atom<uint64_t>(size);
      } இல்லையெனில் எனில் (strcmp(text,"sb8") == 0) {
	    obj = புதிய vvp_darray_atom<int8_t>(size);
      } இல்லையெனில் எனில் (strcmp(text,"sb16") == 0) {
	    obj = புதிய vvp_darray_atom<int16_t>(size);
      } இல்லையெனில் எனில் (strcmp(text,"sb32") == 0) {
	    obj = புதிய vvp_darray_atom<int32_t>(size);
      } இல்லையெனில் எனில் (strcmp(text,"sb64") == 0) {
	    obj = புதிய vvp_darray_atom<int64_t>(size);
      } இல்லையெனில் எனில் ((1 == sscanf(text, "b%u%zn", &word_wid, &n)) &&
                 (n == strlen(text))) {
	    obj = புதிய vvp_darray_vec2(size, word_wid);
      } இல்லையெனில் எனில் ((1 == sscanf(text, "sb%u%zn", &word_wid, &n)) &&
                 (n == strlen(text))) {
	    obj = புதிய vvp_darray_vec2(size, word_wid);
      } இல்லையெனில் எனில் ((1 == sscanf(text, "v%u%zn", &word_wid, &n)) &&
                 (n == strlen(text))) {
	    obj = புதிய vvp_darray_vec4(size, word_wid);
      } இல்லையெனில் எனில் ((1 == sscanf(text, "sv%u%zn", &word_wid, &n)) &&
                 (n == strlen(text))) {
	    obj = புதிய vvp_darray_vec4(size, word_wid);
      } இல்லையெனில் எனில் (strcmp(text,"r") == 0) {
	    obj = புதிய vvp_darray_real(size);
      } இல்லையெனில் எனில் (strcmp(text,"S") == 0) {
	    obj = புதிய vvp_darray_string(size);
      } இல்லையெனில் {
	    cerr << get_fileline()
	         << "Internal error: Unsupported dynamic array type: "
	         << text << "." << endl;
	    ப்ரதிஜ்ஞா(0);
      }

      thr->தள்ளு_object(obj);

      திரும்பு உண்மை;
}

தருக்க of_NOOP(vthread_t, vvp_code_t)
{
      திரும்பு உண்மை;
}

/*
 * %nor/r
 */
தருக்க of_NORR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_or());

      திரும்பு உண்மை;
}

/*
 * Push a null to the object stack.
 */
தருக்க of_NULL(vthread_t thr, vvp_code_t)
{
      vvp_object_t tmp;
      thr->தள்ளு_object(tmp);
      திரும்பு உண்மை;
}

/*
 * %and/r
 */
தருக்க of_ANDR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_and());

      திரும்பு உண்மை;
}

/*
 * %nand/r
 */
தருக்க of_NANDR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_and());

      திரும்பு உண்மை;
}

/*
 * %or/r
 */
தருக்க of_ORR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_or());

      திரும்பு உண்மை;
}

/*
 * %xor/r
 */
தருக்க of_XORR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_xor());

      திரும்பு உண்மை;
}

/*
 * %xnor/r
 */
தருக்க of_XNORR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_xor());

      திரும்பு உண்மை;
}

/*
 * %or
 */
தருக்க of_OR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valb = thr->எடு_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();
      vala |= valb;
      திரும்பு உண்மை;
}

/*
 * %nor
 */
தருக்க of_NOR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valr = thr->எடு_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      ப்ரதிஜ்ஞா(vall.size() == valr.size());

      vall |= valr;
      vall.invert();

      திரும்பு உண்மை;
}

/*
 * %pop/obj <num>, <skip>
 */
தருக்க of_எடு_OBJ(vthread_t thr, vvp_code_t cp)
{
      unsigned cnt = cp->bit_idx[0];
      unsigned skip = cp->bit_idx[1];

      thr->எடு_object(cnt, skip);
      திரும்பு உண்மை;
}

/*
 * %pop/real <number>
 */
தருக்க of_எடு_REAL(vthread_t thr, vvp_code_t cp)
{
      unsigned cnt = cp->எண்;
      thr->எடு_real(cnt);
      திரும்பு உண்மை;
}

/*
 *  %pop/str <number>
 */
தருக்க of_எடு_STR(vthread_t thr, vvp_code_t cp)
{
      unsigned cnt = cp->எண்;
      thr->எடு_str(cnt);
      திரும்பு உண்மை;
}

/*
 *  %pop/vec4 <number>
 */
தருக்க of_எடு_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned cnt = cp->எண்;
      thr->எடு_vec4(cnt);
      திரும்பு உண்மை;
}

/*
 * %pow
 * %pow/s
 */
நிலையான தருக்க of_POW_base(vthread_t thr, தருக்க signed_flag)
{
      vvp_vector4_t valb = thr->எடு_vec4();
      vvp_vector4_t vala = thr->எடு_vec4();

      unsigned wid = vala.size();

      vvp_vector2_t xv2 = vvp_vector2_t(vala, உண்மை);
      vvp_vector2_t yv2 = vvp_vector2_t(valb, உண்மை);


        /* If we have an X or Z in the arguments return X. */
      எனில் (xv2.is_NaN() || yv2.is_NaN()) {
	    vvp_vector4_t tmp (wid, BIT4_X);
	    thr->தள்ளு_vec4(tmp);
	    திரும்பு உண்மை;
      }

	// Is the exponent negative? If so, table 5-6 in IEEE1364-2005
	// defines what value is returned.
      எனில் (signed_flag && yv2.value(yv2.size()-1)) {
	    முழு a_val;
	    vvp_bit4_t pad = BIT4_0, lsb = BIT4_0;
	    எனில் (vector2_to_மதிப்பு(xv2, a_val, உண்மை)) {
		  எனில் (a_val == 0) {
			pad = BIT4_X; lsb = BIT4_X;
		  }
		  எனில் (a_val == 1) {
			pad = BIT4_0; lsb = BIT4_1;
		  }
		  எனில் (a_val == -1) {
			எனில் (yv2.value(0)) {
			      pad = BIT4_1; lsb = BIT4_1;
			} இல்லையெனில் {
			      pad = BIT4_0; lsb = BIT4_1;
			}
		  }
	    }
	    vvp_vector4_t tmp (wid, pad);
	    tmp.set_bit(0, lsb);
	    thr->தள்ளு_vec4(tmp);
	    திரும்பு உண்மை;
      }

      vvp_vector2_t result = pow(xv2, yv2);

        /* Copy only what we need of the result. If the result is too
	   small, zero-pad it. */
      க்கு (unsigned jdx = 0;  jdx < wid;  jdx += 1) {
	    எனில் (jdx >= result.size())
		  vala.set_bit(jdx, BIT4_0);
	    இல்லையெனில்
		  vala.set_bit(jdx, result.value(jdx) ? BIT4_1 : BIT4_0);
      }
      thr->தள்ளு_vec4(vala);

      திரும்பு உண்மை;
}

தருக்க of_POW(vthread_t thr, vvp_code_t)
{
      திரும்பு of_POW_base(thr, பொய்);
}

தருக்க of_POW_S(vthread_t thr, vvp_code_t)
{
      திரும்பு of_POW_base(thr, உண்மை);
}

தருக்க of_POW_WR(vthread_t thr, vvp_code_t)
{
      இரட்டை r = thr->எடு_real();
      இரட்டை l = thr->எடு_real();
      thr->தள்ளு_real(pow(l,r));

      திரும்பு உண்மை;
}

/*
 * %prop/obj <pid>, <idx>
 *
 * Load an object value from the cobject and push it onto the object stack.
 */
தருக்க of_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      unsigned pid = cp->எண்;
      unsigned idx = cp->bit_idx[0];

      எனில் (idx != 0) {
	    ப்ரதிஜ்ஞா(idx < vthread_s::சொற்கள்_மொத்தம்);
	    idx = thr->சொற்கள்[idx].w_uint;
      }

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();

      vvp_object_t val;
      cobj->get_object(pid, val, idx);

      thr->தள்ளு_object(val);

      திரும்பு உண்மை;
}

நிலையான வெறுமை get_from_obj(unsigned pid, vvp_cobject*cobj, இரட்டை&val)
{
      val = cobj->get_real(pid);
}

நிலையான வெறுமை get_from_obj(unsigned pid, vvp_cobject*cobj, string&val)
{
      val = cobj->get_string(pid);
}

நிலையான வெறுமை get_from_obj(unsigned pid, vvp_cobject*cobj, vvp_vector4_t&val)
{
      cobj->get_vec4(pid, val);
}

template <typename ELEM>
நிலையான தருக்க prop(vthread_t thr, vvp_code_t cp)
{
      unsigned pid = cp->எண்;

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      ப்ரதிஜ்ஞா(cobj);

      ELEM val;
      get_from_obj(pid, cobj, val);
      vthread_தள்ளு(thr, val);

      திரும்பு உண்மை;
}

/*
 * %prop/r <pid>
 *
 * Load a real value from the cobject and push it onto the real value
 * stack.
 */
தருக்க of_PROP_R(vthread_t thr, vvp_code_t cp)
{
      திரும்பு prop<இரட்டை>(thr, cp);
}

/*
 * %prop/str <pid>
 *
 * Load a string value from the cobject and push it onto the real value
 * stack.
 */
தருக்க of_PROP_STR(vthread_t thr, vvp_code_t cp)
{
      திரும்பு prop<string>(thr, cp);
}

/*
 * %prop/v <pid>
 *
 * Load a property <pid> from the cobject on the top of the stack into
 * the vector space at <base>.
 */
தருக்க of_PROP_V(vthread_t thr, vvp_code_t cp)
{
      திரும்பு prop<vvp_vector4_t>(thr, cp);
}

தருக்க of_PUSHI_REAL(vthread_t thr, vvp_code_t cp)
{
      இரட்டை mant = cp->bit_idx[0];
      uint32_t imant = cp->bit_idx[0];
      முழு exp = cp->bit_idx[1];

	// Detect +infinity
      எனில் (exp==0x3fff && imant==0) {
	    thr->தள்ளு_real(INFINITY);
	    திரும்பு உண்மை;
      }
	// Detect -infinity
      எனில் (exp==0x7fff && imant==0) {
	    thr->தள்ளு_real(-INFINITY);
	    திரும்பு உண்மை;
      }
	// Detect NaN
      எனில் (exp==0x3fff) {
	    thr->தள்ளு_real(nan(""));
	    திரும்பு உண்மை;
      }

      இரட்டை sign = (exp & 0x4000)? -1.0 : 1.0;

      exp &= 0x1fff;

      mant = sign * ldexp(mant, exp - 0x1000);
      thr->தள்ளு_real(mant);
      திரும்பு உண்மை;
}

தருக்க of_PUSHI_STR(vthread_t thr, vvp_code_t cp)
{
      மாறிலி எழுத்து*text = cp->text;
      thr->தள்ளு_str(filter_string(text));
      திரும்பு உண்மை;
}

/*
 * %pushi/vec4 <vala>, <valb>, <wid>
 */
தருக்க of_PUSHI_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned wid  = cp->எண்;

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t val (wid, BIT4_0);
      get_immediate_rval (cp, val);

      thr->தள்ளு_vec4(val);

      திரும்பு உண்மை;
}

/*
 * %pushv/str
 *   Pops a vec4 value, and pushes a string.
 */
தருக்க of_PUSHV_STR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t vec = thr->எடு_vec4();

      size_t slen = (vec.size() + 7)/8;
      vector<எழுத்து>buf;
      buf.reserve(slen);

      க்கு (size_t idx = 0 ; idx < vec.size() ; idx += 8) {
	    எழுத்து tmp = 0;
	    size_t trans = 8;
	    எனில் (idx+trans > vec.size())
		  trans = vec.size() - idx;

	    க்கு (size_t bdx = 0 ; bdx < trans ; bdx += 1) {
		  எனில் (vec.value(idx+bdx) == BIT4_1)
			tmp |= 1 << bdx;
	    }

	    எனில் (tmp != 0)
		  buf.push_back(tmp);
      }

      string val;
      க்கு (vector<எழுத்து>::reverse_iterator cur = buf.rbegin()
		 ; cur != buf.rend() ; ++cur) {
	    val.push_back(*cur);
      }

      thr->தள்ளு_str(val);

      திரும்பு உண்மை;
}

/*
 * %putc/str/vec4 <var>, <mux>
 */
தருக்க of_PUTC_STR_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned muxr = cp->bit_idx[0];
      int32_t mux = muxr? thr->சொற்கள்[muxr].w_int : 0;

      vvp_vector4_t val = thr->எடு_vec4();
      ப்ரதிஜ்ஞா(val.size() == 8);

      எனில் (mux < 0)
	    திரும்பு உண்மை;

	/* Get the existing value of the string. If we find that the
	   index is too big for the string, then give up. */
      vvp_net_t*net = cp->net;
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (net->fun);
      ப்ரதிஜ்ஞா(fun);

      string tmp = fun->get_string();
      எனில் (tmp.size() <= (size_t)mux)
	    திரும்பு உண்மை;

      எழுத்து val_str = 0;
      க்கு (size_t idx = 0 ; idx < 8 ; idx += 1) {
	    எனில் (val.value(idx)==BIT4_1)
		  val_str |= 1<<idx;
      }

	// It is a quirk of the Verilog standard that putc(..., 'h00)
	// has no effect. Test for that case here.
      எனில் (val_str == 0)
	    திரும்பு உண்மை;

      tmp[mux] = val_str;

      vvp_send_string(vvp_net_ptr_t(cp->net, 0), tmp, thr->wt_context);
      திரும்பு உண்மை;
}

template <typename ELEM, வகுப்பு QTYPE>
நிலையான தருக்க qinsert(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
      int64_t idx = thr->சொற்கள்[3].w_int;
      ELEM value;
      vvp_net_t*net = cp->net;
      எடு_மதிப்பு(thr, value, wid); // Pop the value to store.

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      ப்ரதிஜ்ஞா(queue);
      எனில் (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at a negative "
	         << get_queue_type(value)
	         << " index (" << idx << "). ";
	    print_queue_மதிப்பு(value);
	    cerr << " was not added." << endl;
      } இல்லையெனில் எனில் (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at an undefined "
	         << get_queue_type(value) << " index. ";
	    print_queue_மதிப்பு(value);
	    cerr << " was not added." << endl;
      } இல்லையெனில் {
	    unsigned max_size = thr->சொற்கள்[cp->bit_idx[0]].w_int;
	    queue->insert(idx, value, max_size);
      }
      திரும்பு உண்மை;
}

/*
 * %qinsert/real <var-label>
 */
தருக்க of_QINSERT_REAL(vthread_t thr, vvp_code_t cp)
{
      திரும்பு qinsert<இரட்டை, vvp_queue_real>(thr, cp);
}

/*
 * %qinsert/str <var-label>
 */
தருக்க of_QINSERT_STR(vthread_t thr, vvp_code_t cp)
{
      திரும்பு qinsert<string, vvp_queue_string>(thr, cp);
}

/*
 * %qinsert/v <var-label>
 */
தருக்க of_QINSERT_V(vthread_t thr, vvp_code_t cp)
{
      திரும்பு qinsert<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

/*
 * Helper functions used in the queue pop templates
 */
உள்வரி வெறுமை தள்ளு_மதிப்பு(vthread_t thr, இரட்டை value, unsigned)
{
      thr->தள்ளு_real(value);
}

உள்வரி வெறுமை தள்ளு_மதிப்பு(vthread_t thr, மாறிலி string&value, unsigned)
{
      thr->தள்ளு_str(value);
}

உள்வரி வெறுமை தள்ளு_மதிப்பு(vthread_t thr, மாறிலி vvp_vector4_t&value, unsigned wid)
{
      ப்ரதிஜ்ஞா(wid == value.size());
      thr->தள்ளு_vec4(value);
}

template <typename ELEM, வகுப்பு QTYPE>
நிலையான தருக்க q_எடு(vthread_t thr, vvp_code_t cp,
                  வெறுமை (*get_val_func)(vvp_queue*, ELEM&),
                  மாறிலி எழுத்து*loc, unsigned wid)
{
      vvp_net_t*net = cp->net;

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      ப்ரதிஜ்ஞா(queue);

      size_t size = queue->get_size();

      ELEM value;
      எனில் (size) {
	    get_val_func(queue, value);
      } இல்லையெனில் {
	    dq_default(value, wid);
	    cerr << thr->get_fileline()
	         << "Warning: pop_" << loc << "() on empty "
	         << get_queue_type(value) << "." << endl;
      }

      தள்ளு_மதிப்பு(thr, value, wid);
      திரும்பு உண்மை;
}

template <typename ELEM>
நிலையான வெறுமை get_back_மதிப்பு(vvp_queue*queue, ELEM&value)
{
      queue->get_word(queue->get_size()-1, value);
      queue->pop_back();
}

template <typename ELEM, வகுப்பு QTYPE>
நிலையான தருக்க qpop_b(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
      திரும்பு q_எடு<ELEM, QTYPE>(thr, cp, get_back_மதிப்பு<ELEM>, "back", wid);
}

/*
 * %qpop/b/real <var-label>
 */
தருக்க of_QPOP_B_REAL(vthread_t thr, vvp_code_t cp)
{
      திரும்பு qpop_b<இரட்டை, vvp_queue_real>(thr, cp);
}

/*
 * %qpop/b/str <var-label>
 */
தருக்க of_QPOP_B_STR(vthread_t thr, vvp_code_t cp)
{
      திரும்பு qpop_b<string, vvp_queue_string>(thr, cp);
}

/*
 * %qpop/b/v <var-label>
 */
தருக்க of_QPOP_B_V(vthread_t thr, vvp_code_t cp)
{
      திரும்பு qpop_b<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[0]);
}

template <typename ELEM>
நிலையான வெறுமை get_front_மதிப்பு(vvp_queue*queue, ELEM&value)
{
      queue->get_word(0, value);
      queue->pop_front();
}

template <typename ELEM, வகுப்பு QTYPE>
நிலையான தருக்க qpop_f(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
      திரும்பு q_எடு<ELEM, QTYPE>(thr, cp, get_front_மதிப்பு<ELEM>, "front", wid);
}


/*
 * %qpop/f/real <var-label>
 */
தருக்க of_QPOP_F_REAL(vthread_t thr, vvp_code_t cp)
{
      திரும்பு qpop_f<இரட்டை, vvp_queue_real>(thr, cp);
}

/*
 * %qpop/f/str <var-label>
 */
தருக்க of_QPOP_F_STR(vthread_t thr, vvp_code_t cp)
{
      திரும்பு qpop_f<string, vvp_queue_string>(thr, cp);
}

/*
 * %qpop/f/v <var-label>
 */
தருக்க of_QPOP_F_V(vthread_t thr, vvp_code_t cp)
{
      திரும்பு qpop_f<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[0]);
}

/*
 * These implement the %release/net and %release/reg instructions. The
 * %release/net instruction applies to a net kind of functor by
 * sending the release/net command to the command port. (See vvp_net.h
 * for details.) The %release/reg instruction is the same, but sends
 * the release/reg command instead. These are very similar to the
 * %deassign instruction.
 */
நிலையான தருக்க do_release_vec(vvp_code_t cp, தருக்க net_flag)
{
      vvp_net_t*net = cp->net;
      unsigned base  = cp->bit_idx[0];
      unsigned width = cp->bit_idx[1];

      ப்ரதிஜ்ஞா(net->fil);

      எனில் (base >= net->fil->filter_size()) திரும்பு உண்மை;
      எனில் (base+width > net->fil->filter_size())
	    width = net->fil->filter_size() - base;

      தருக்க full_sig = base == 0 && width == net->fil->filter_size();

	// XXXX Can't really do this if this is a partial release?
      net->fil->force_unlink();

	/* Do we release all or part of the net? */
      vvp_net_ptr_t ptr (net, 0);
      எனில் (full_sig) {
	    net->fil->release(ptr, net_flag);
      } இல்லையெனில் {
	    net->fil->release_pv(ptr, base, width, net_flag);
      }
      net->fun->force_flag(பொய்);

      திரும்பு உண்மை;
}

தருக்க of_RELEASE_NET(vthread_t, vvp_code_t cp)
{
      திரும்பு do_release_vec(cp, உண்மை);
}


தருக்க of_RELEASE_REG(vthread_t, vvp_code_t cp)
{
      திரும்பு do_release_vec(cp, பொய்);
}

/* The type is 1 for registers and 0 for everything else. */
தருக்க of_RELEASE_WR(vthread_t, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      unsigned type  = cp->bit_idx[0];

      ப்ரதிஜ்ஞா(net->fil);
      net->fil->force_unlink();

	// Send a command to this signal to unforce itself.
      vvp_net_ptr_t ptr (net, 0);
      net->fil->release(ptr, type==0);
      திரும்பு உண்மை;
}

தருக்க of_REPLICATE(vthread_t thr, vvp_code_t cp)
{
      முழு rept = cp->எண்;
      vvp_vector4_t val = thr->எடு_vec4();
      vvp_vector4_t res (val.size() * rept, BIT4_X);

      க்கு (முழு idx = 0 ; idx < rept ; idx += 1) {
	    res.set_vec(idx * val.size(), val);
      }

      thr->தள்ளு_vec4(res);

      திரும்பு உண்மை;
}

நிலையான வெறுமை poke_val(vthread_t fun_thr, unsigned depth, இரட்டை val)
{
      fun_thr->parent->poke_real(depth, val);
}

நிலையான வெறுமை poke_val(vthread_t fun_thr, unsigned depth, மாறிலி string&val)
{
      fun_thr->parent->poke_str(depth, val);
}

நிலையான size_t get_max(vthread_t fun_thr, இரட்டை&)
{
      திரும்பு fun_thr->args_real.size();
}

நிலையான size_t get_max(vthread_t fun_thr, string&)
{
      திரும்பு fun_thr->args_str.size();
}

நிலையான size_t get_max(vthread_t fun_thr, vvp_vector4_t&)
{
      திரும்பு fun_thr->args_vec4.size();
}

நிலையான unsigned get_depth(vthread_t fun_thr, size_t index, இரட்டை&)
{
      திரும்பு fun_thr->args_real[index];
}

நிலையான unsigned get_depth(vthread_t fun_thr, size_t index, string&)
{
      திரும்பு fun_thr->args_str[index];
}

நிலையான unsigned get_depth(vthread_t fun_thr, size_t index, vvp_vector4_t&)
{
      திரும்பு fun_thr->args_vec4[index];
}

நிலையான vthread_t get_func(vthread_t thr)
{
      vthread_t fun_thr = thr;

      போது (fun_thr->parent_scope->get_type_code() != vpiFunction) {
	    ப்ரதிஜ்ஞா(fun_thr->parent);
	    fun_thr = fun_thr->parent;
      }

      திரும்பு fun_thr;
}

template <typename ELEM>
நிலையான தருக்க ret(vthread_t thr, vvp_code_t cp)
{
      size_t index = cp->எண்;
      ELEM val;
      எடு_மதிப்பு(thr, val, 0);

      vthread_t fun_thr = get_func(thr);
      ப்ரதிஜ்ஞா(index < get_max(fun_thr, val));

      unsigned depth = get_depth(fun_thr, index, val);
	// Use the depth to put the value into the stack of
	// the parent thread.
      poke_val(fun_thr, depth, val);
      திரும்பு உண்மை;
}

/*
 * %ret/real <index>
 */
தருக்க of_RET_REAL(vthread_t thr, vvp_code_t cp)
{
      திரும்பு ret<இரட்டை>(thr, cp);
}

/*
 * %ret/str <index>
 */
தருக்க of_RET_STR(vthread_t thr, vvp_code_t cp)
{
      திரும்பு ret<string>(thr, cp);
}

/*
 * %ret/vec4 <index>, <offset>, <wid>
 */
தருக்க of_RET_VEC4(vthread_t thr, vvp_code_t cp)
{
      size_t index = cp->எண்;
      unsigned off_index = cp->bit_idx[0];
      unsigned முழு wid = cp->bit_idx[1];
      vvp_vector4_t&val = thr->peek_vec4();

      vthread_t fun_thr = get_func(thr);
      ப்ரதிஜ்ஞா(index < get_max(fun_thr, val));
      ப்ரதிஜ்ஞா(val.size() == wid);
      unsigned depth = get_depth(fun_thr, index, val);

      int64_t off = off_index ? thr->சொற்கள்[off_index].w_int : 0;
      unsigned முழு sig_மதிப்பு_size = fun_thr->parent->peek_vec4(depth).size();

      எனில் (off_index!=0 && thr->flags[4] == BIT4_1) {
	    thr->எடு_vec4(1);
	    திரும்பு உண்மை;
      }

      எனில் (!resize_rval_vec(val, off, sig_மதிப்பு_size)) {
	    thr->எடு_vec4(1);
	    திரும்பு உண்மை;
      }

      எனில் (off == 0 && val.size() == sig_மதிப்பு_size) {
	    fun_thr->parent->poke_vec4(depth, val);
      } இல்லையெனில் {
	    vvp_vector4_t tmp_dst = fun_thr->parent->peek_vec4(depth);
	    tmp_dst.set_vec(off, val);
	    fun_thr->parent->poke_vec4(depth, tmp_dst);
      }

      thr->எடு_vec4(1);
      திரும்பு உண்மை;
}

நிலையான வெறுமை தள்ளு_from_parent(vthread_t thr, vthread_t fun_thr, unsigned depth, இரட்டை&)
{
      thr->தள்ளு_real(fun_thr->parent->peek_real(depth));
}

நிலையான வெறுமை தள்ளு_from_parent(vthread_t thr, vthread_t fun_thr, unsigned depth, string&)
{
      thr->தள்ளு_str(fun_thr->parent->peek_str(depth));
}

நிலையான வெறுமை தள்ளு_from_parent(vthread_t thr, vthread_t fun_thr, unsigned depth, vvp_vector4_t&)
{
      thr->தள்ளு_vec4(fun_thr->parent->peek_vec4(depth));
}

template <typename ELEM>
நிலையான தருக்க retload(vthread_t thr, vvp_code_t cp)
{
      size_t index = cp->எண்;
      ELEM type;

      vthread_t fun_thr = get_func(thr);
      ப்ரதிஜ்ஞா(index < get_max(fun_thr, type));

      unsigned depth = get_depth(fun_thr, index, type);
	// Use the depth to extract the values from the stack
	// of the parent thread.
      தள்ளு_from_parent(thr, fun_thr, depth, type);
      திரும்பு உண்மை;
}

/*
 * %retload/real <index>
 */
தருக்க of_RETLOAD_REAL(vthread_t thr, vvp_code_t cp)
{
      திரும்பு retload<இரட்டை>(thr, cp);
}

/*
 * %retload/str <index>
 */
தருக்க of_RETLOAD_STR(vthread_t thr, vvp_code_t cp)
{
      திரும்பு retload<string>(thr, cp);
}

/*
 * %retload/vec4 <index>
 */
தருக்க of_RETLOAD_VEC4(vthread_t thr, vvp_code_t cp)
{
      திரும்பு retload<vvp_vector4_t>(thr, cp);
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
தருக்க of_SCOPY(vthread_t thr, vvp_code_t)
{
      vvp_object_t tmp;
      thr->எடு_object(tmp);

      vvp_object_t&dest = thr->peek_object();
        // If it is null there is nothing to copy
      எனில் (!tmp.test_nil())
	    dest.shallow_copy(tmp);

      திரும்பு உண்மை;
}

நிலையான வெறுமை thread_peek(vthread_t thr, இரட்டை&value)
{
      value = thr->peek_real(0);
}

நிலையான வெறுமை thread_peek(vthread_t thr, string&value)
{
      value = thr->peek_str(0);
}

நிலையான வெறுமை thread_peek(vthread_t thr, vvp_vector4_t&value)
{
      value = thr->peek_vec4(0);
}

template <typename ELEM>
நிலையான தருக்க set_dar_obj(vthread_t thr, vvp_code_t cp)
{
      unsigned adr = thr->சொற்கள்[cp->எண்].w_int;

      ELEM value;
      thread_peek(thr, value);

      vvp_object_t&உச்சி = thr->peek_object();
      vvp_darray*darray = உச்சி.peek<vvp_darray>();
      ப்ரதிஜ்ஞா(darray);

      darray->set_word(adr, value);
      திரும்பு உண்மை;
}

/*
 * %set/dar/obj/real <index>
 */
தருக்க of_SET_DAR_OBJ_REAL(vthread_t thr, vvp_code_t cp)
{
      திரும்பு set_dar_obj<இரட்டை>(thr, cp);
}

/*
 * %set/dar/obj/str <index>
 */
தருக்க of_SET_DAR_OBJ_STR(vthread_t thr, vvp_code_t cp)
{
      திரும்பு set_dar_obj<string>(thr, cp);
}

/*
 * %set/dar/obj/vec4 <index>
 */
தருக்க of_SET_DAR_OBJ_VEC4(vthread_t thr, vvp_code_t cp)
{
      திரும்பு set_dar_obj<vvp_vector4_t>(thr, cp);
}

/*
 * %shiftl <idx>
 *
 * Pop the operand, then push the result.
 */
தருக்க of_SHIFTL(vthread_t thr, vvp_code_t cp)
{
      முழு use_index = cp->எண்;
      uint64_t shift = thr->சொற்கள்[use_index].w_uint;

      vvp_vector4_t&val = thr->peek_vec4();
      unsigned wid  = val.size();

      எனில் (thr->flags[4] == BIT4_1) {
	      // The result is 'bx if the shift amount is undefined
	    val = vvp_vector4_t(wid, BIT4_X);

      } இல்லையெனில் எனில் (thr->flags[4] == BIT4_X || shift >= wid) {
	      // Shift is so big that all value is shifted out. Write
	      // a constant 0 result.
	    val = vvp_vector4_t(wid, BIT4_0);

      } இல்லையெனில் எனில் (shift > 0) {
	    vvp_vector4_t blk = val.subvalue(0, wid-shift);
	    vvp_vector4_t tmp (shift, BIT4_0);
	    val.set_vec(0, tmp);
	    val.set_vec(shift, blk);
      }

      திரும்பு உண்மை;
}

/*
 * %shiftr <idx>
 * This is an unsigned right shift. The <idx> is a number that selects
 * the index register with the amount of the shift. This instruction
 * checks flag bit 4, which will be true if the shift is invalid.
 */
தருக்க of_SHIFTR(vthread_t thr, vvp_code_t cp)
{
      முழு use_index = cp->எண்;
      uint64_t shift = thr->சொற்கள்[use_index].w_uint;

      vvp_vector4_t val = thr->எடு_vec4();
      unsigned wid  = val.size();

      எனில் (thr->flags[4] == BIT4_1) {
	    val = vvp_vector4_t(wid, BIT4_X);

      } இல்லையெனில் எனில் (thr->flags[4] == BIT4_X || shift > wid) {
	    val = vvp_vector4_t(wid, BIT4_0);

      } இல்லையெனில் எனில் (shift > 0) {
	    vvp_vector4_t blk = val.subvalue(shift, wid-shift);
	    vvp_vector4_t tmp (shift, BIT4_0);
	    val.set_vec(0, blk);
	    val.set_vec(wid-shift, tmp);
      }

      thr->தள்ளு_vec4(val);
      திரும்பு உண்மை;
}

/*
 *  %shiftr/s <wid>
 */
தருக்க of_SHIFTR_S(vthread_t thr, vvp_code_t cp)
{
      முழு use_index = cp->எண்;
      uint64_t shift = thr->சொற்கள்[use_index].w_uint;

      vvp_vector4_t val = thr->எடு_vec4();
      unsigned wid  = val.size();

      vvp_bit4_t sign_bit = val.value(val.size()-1);

      எனில் (thr->flags[4] == BIT4_1) {
	    val = vvp_vector4_t(wid, BIT4_X);

      } இல்லையெனில் எனில் (thr->flags[4] == BIT4_X || shift > wid) {
	    val = vvp_vector4_t(wid, sign_bit);

      } இல்லையெனில் எனில் (shift > 0) {
	    vvp_vector4_t blk = val.subvalue(shift, wid-shift);
	    vvp_vector4_t tmp (shift, sign_bit);
	    val.set_vec(0, blk);
	    val.set_vec(wid-shift, tmp);
      }

      thr->தள்ளு_vec4(val);
      திரும்பு உண்மை;
}

/*
 * %split/vec4 <wid>
 *   Pop 1 value,
 *   Take <wid> bits from the lsb,
 *   Push the remaining msb,
 *   Push the lsb.
 */
தருக்க of_SPLIT_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned lsb_wid = cp->எண்;

      vvp_vector4_t&val = thr->peek_vec4();
      ப்ரதிஜ்ஞா(lsb_wid < val.size());

      vvp_vector4_t lsb = val.subvalue(0, lsb_wid);
      val = val.subvalue(lsb_wid, val.size()-lsb_wid);

      thr->தள்ளு_vec4(lsb);
      திரும்பு உண்மை;
}

/*
 * The following are used to allow the darray templates to print correctly.
 */
உள்வரி நிலையான string get_darray_type(இரட்டை&)
{
      திரும்பு "darray<real>";
}

உள்வரி நிலையான string get_darray_type(string&)
{
      திரும்பு "darray<string>";
}

உள்வரி நிலையான string get_darray_type(மாறிலி vvp_vector4_t&value)
{
      ostringstream buf;
      buf << "darray<vector[" << value.size() << "]>";
      string res = buf.str();
      திரும்பு res;
}

/*
 * The following are used to allow a common template to be written for
 * darray real/string/vec4 operations
 */
உள்வரி நிலையான வெறுமை dar_எடு_மதிப்பு(vthread_t thr, இரட்டை&value)
{
      value = thr->எடு_real();
}

உள்வரி நிலையான வெறுமை dar_எடு_மதிப்பு(vthread_t thr, string&value)
{
      value = thr->எடு_str();
}

உள்வரி நிலையான வெறுமை dar_எடு_மதிப்பு(vthread_t thr, vvp_vector4_t&value)
{
      value = thr->எடு_vec4();
}

template <typename ELEM>
நிலையான தருக்க store_dar(vthread_t thr, vvp_code_t cp)
{
      int64_t adr = thr->சொற்கள்[3].w_int;
      ELEM value;
	// FIXME: Can we get the size of the underlying array element
	//        and then use the normal pop_value?
      dar_எடு_மதிப்பு(thr, value);

      vvp_net_t*net = cp->net;
      ப்ரதிஜ்ஞா(net);

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      ப்ரதிஜ்ஞா(obj);

      vvp_darray*darray = obj->get_object().peek<vvp_darray>();

      எனில் (adr < 0)
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to a negative " << get_darray_type(value)
	         << " index (" << adr << ")." << endl;
      இல்லையெனில் எனில் (thr->flags[4] != BIT4_0)
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to an undefined " << get_darray_type(value)
	         << " index." << endl;
      இல்லையெனில் எனில் (darray)
	    darray->set_word(adr, value);
      இல்லையெனில்
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to an undefined " << get_darray_type(value)
	         << "." << endl;

      திரும்பு உண்மை;
}

/*
 * %store/dar/real <var>
 */
தருக்க of_STORE_DAR_R(vthread_t thr, vvp_code_t cp)
{
      திரும்பு store_dar<இரட்டை>(thr, cp);
}

/*
 * %store/dar/str <var>
 */
தருக்க of_STORE_DAR_STR(vthread_t thr, vvp_code_t cp)
{
      திரும்பு store_dar<string>(thr, cp);
}

/*
 * %store/dar/vec4 <var>
 */
தருக்க of_STORE_DAR_VEC4(vthread_t thr, vvp_code_t cp)
{
      திரும்பு store_dar<vvp_vector4_t>(thr, cp);
}

தருக்க of_STORE_OBJ(vthread_t thr, vvp_code_t cp)
{
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);

      vvp_object_t val;
      thr->எடு_object(val);

      vvp_send_object(ptr, val, thr->wt_context);

      திரும்பு உண்மை;
}

/*
 * %store/obja <array-label> <index>
 */
தருக்க of_STORE_OBJA(vthread_t thr, vvp_code_t cp)
{
      unsigned idx = cp->bit_idx[0];
      unsigned adr = thr->சொற்கள்[idx].w_int;

      vvp_object_t val;
      thr->எடு_object(val);

      cp->array->set_word(adr, val);

      திரும்பு உண்மை;
}


/*
 * %store/prop/obj <pid>, <idx>
 *
 * Pop an object value from the object stack, and store the value into
 * the property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
தருக்க of_STORE_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->எண்;
      unsigned idx = cp->bit_idx[0];

      எனில் (idx != 0) {
	    ப்ரதிஜ்ஞா(idx < vthread_s::சொற்கள்_மொத்தம்);
	    idx = thr->சொற்கள்[idx].w_uint;
      }

      vvp_object_t val;
      thr->எடு_object(val);

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      ப்ரதிஜ்ஞா(cobj);

      cobj->set_object(pid, val, idx);

      திரும்பு உண்மை;
}

நிலையான வெறுமை எடு_prop_val(vthread_t thr, இரட்டை&val, unsigned)
{
      val = thr->எடு_real();
}

நிலையான வெறுமை எடு_prop_val(vthread_t thr, string&val, unsigned)
{
      val = thr->எடு_str();
}

நிலையான வெறுமை எடு_prop_val(vthread_t thr, vvp_vector4_t&val, unsigned wid)
{
      val = thr->எடு_vec4();
      ப்ரதிஜ்ஞா(val.size() >= wid);
      val.resize(wid);
}

நிலையான வெறுமை set_val(vvp_cobject*cobj, size_t pid, மாறிலி இரட்டை&val)
{
      cobj->set_real(pid, val);
}

நிலையான வெறுமை set_val(vvp_cobject*cobj, size_t pid, மாறிலி string&val)
{
      cobj->set_string(pid, val);
}

நிலையான வெறுமை set_val(vvp_cobject*cobj, size_t pid, மாறிலி vvp_vector4_t&val)
{
      cobj->set_vec4(pid, val);
}

template <typename ELEM>
நிலையான தருக்க store_prop(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
      size_t pid = cp->எண்;
      ELEM val;
      எடு_prop_val(thr, val, wid); // Pop the value to store.

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      ப்ரதிஜ்ஞா(cobj);

      set_val(cobj, pid, val);

      திரும்பு உண்மை;
}

/*
 * %store/prop/r <id>
 *
 * Pop a real value from the real stack, and store the value into the
 * property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
தருக்க of_STORE_PROP_R(vthread_t thr, vvp_code_t cp)
{
      திரும்பு store_prop<இரட்டை>(thr, cp);
}

/*
 * %store/prop/str <id>
 *
 * Pop a string value from the string stack, and store the value into
 * the property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
தருக்க of_STORE_PROP_STR(vthread_t thr, vvp_code_t cp)
{
      திரும்பு store_prop<string>(thr, cp);
}

/*
 * %store/prop/v <pid>, <wid>
 *
 * Store vector value into property <id> of cobject in the top of the
 * stack. Do NOT pop the object stack.
 */
தருக்க of_STORE_PROP_V(vthread_t thr, vvp_code_t cp)
{
      திரும்பு store_prop<vvp_vector4_t>(thr, cp, cp->bit_idx[0]);
}

template <வகுப்பு QTYPE>
நிலையான QTYPE* get_queue_prop(vvp_cobject*cobj, size_t pid)
{
      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      QTYPE* queue = qobj.peek<QTYPE>();
      எனில் (queue == 0) {
	    queue = புதிய QTYPE;
	    vvp_object_t val(queue);
	    cobj->set_object(pid, val, 0);
      }
      திரும்பு queue;
}

template <typename ELEM, வகுப்பு QTYPE>
நிலையான தருக்க store_prop_qb(vthread_t thr, vvp_code_t cp, unsigned wid)
{
      size_t pid = cp->எண்;
      ELEM value;
      unsigned max_size = thr->சொற்கள்[cp->bit_idx[0]].w_uint;
      எடு_மதிப்பு(thr, value, wid);

      vvp_object_t& உச்சி = thr->peek_object();
      vvp_cobject*cobj = உச்சி.peek<vvp_cobject>();
      ப்ரதிஜ்ஞா(cobj);

      QTYPE* queue = get_queue_prop<QTYPE>(cobj, pid);
      ப்ரதிஜ்ஞா(queue);
      queue->push_back(value, max_size);
      திரும்பு உண்மை;
}

template <typename ELEM, வகுப்பு QTYPE>
நிலையான தருக்க store_prop_qf(vthread_t thr, vvp_code_t cp, unsigned wid)
{
      size_t pid = cp->எண்;
      ELEM value;
      unsigned max_size = thr->சொற்கள்[cp->bit_idx[0]].w_uint;
      எடு_மதிப்பு(thr, value, wid);

      vvp_object_t& உச்சி = thr->peek_object();
      vvp_cobject*cobj = உச்சி.peek<vvp_cobject>();
      ப்ரதிஜ்ஞா(cobj);

      QTYPE* queue = get_queue_prop<QTYPE>(cobj, pid);
      ப்ரதிஜ்ஞா(queue);
      queue->push_front(value, max_size);
      திரும்பு உண்மை;
}

தருக்க of_STORE_PROP_QB_R(vthread_t thr, vvp_code_t cp)
{
      திரும்பு store_prop_qb<இரட்டை, vvp_queue_real>(thr, cp, 0);
}

தருக்க of_STORE_PROP_QB_STR(vthread_t thr, vvp_code_t cp)
{
      திரும்பு store_prop_qb<string, vvp_queue_string>(thr, cp, 0);
}

தருக்க of_STORE_PROP_QB_V(vthread_t thr, vvp_code_t cp)
{
      திரும்பு store_prop_qb<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

தருக்க of_STORE_PROP_QF_R(vthread_t thr, vvp_code_t cp)
{
      திரும்பு store_prop_qf<இரட்டை, vvp_queue_real>(thr, cp, 0);
}

தருக்க of_STORE_PROP_QF_STR(vthread_t thr, vvp_code_t cp)
{
      திரும்பு store_prop_qf<string, vvp_queue_string>(thr, cp, 0);
}

தருக்க of_STORE_PROP_QF_V(vthread_t thr, vvp_code_t cp)
{
      திரும்பு store_prop_qf<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

template <typename ELEM, வகுப்பு QTYPE>
நிலையான தருக்க qinsert_prop(vthread_t thr, vvp_code_t cp, unsigned wid)
{
      int64_t idx = thr->சொற்கள்[3].w_int;
      ELEM value;
      size_t pid = cp->எண்;
      எடு_மதிப்பு(thr, value, wid);

      vvp_object_t& உச்சி = thr->peek_object();
      vvp_cobject*cobj = உச்சி.peek<vvp_cobject>();
      ப்ரதிஜ்ஞா(cobj);

      QTYPE* queue = get_queue_prop<QTYPE>(cobj, pid);
      ப்ரதிஜ்ஞா(queue);
      எனில் (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at a negative "
	         << get_queue_type(value)
	         << " index (" << idx << "). ";
	    print_queue_மதிப்பு(value);
	    cerr << " was not added." << endl;
      } இல்லையெனில் எனில் (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at an undefined "
	         << get_queue_type(value) << " index. ";
	    print_queue_மதிப்பு(value);
	    cerr << " was not added." << endl;
      } இல்லையெனில் {
	    unsigned max_size = thr->சொற்கள்[cp->bit_idx[0]].w_int;
	    queue->insert(idx, value, max_size);
      }
      திரும்பு உண்மை;
}

தருக்க of_QINSERT_PROP_R(vthread_t thr, vvp_code_t cp)
{
      திரும்பு qinsert_prop<இரட்டை, vvp_queue_real>(thr, cp, 0);
}

தருக்க of_QINSERT_PROP_STR(vthread_t thr, vvp_code_t cp)
{
      திரும்பு qinsert_prop<string, vvp_queue_string>(thr, cp, 0);
}

தருக்க of_QINSERT_PROP_V(vthread_t thr, vvp_code_t cp)
{
      திரும்பு qinsert_prop<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

template <typename ELEM, வகுப்பு QTYPE>
நிலையான தருக்க q_எடு_prop(vthread_t thr, vvp_code_t cp,
                       வெறுமை (*get_val_func)(vvp_queue*, ELEM&),
                       மாறிலி எழுத்து*loc, unsigned wid)
{
      size_t pid = cp->எண்;

      vvp_object_t& உச்சி = thr->peek_object();
      vvp_cobject*cobj = உச்சி.peek<vvp_cobject>();
      ப்ரதிஜ்ஞா(cobj);

      QTYPE* queue = get_queue_prop<QTYPE>(cobj, pid);
      ப்ரதிஜ்ஞா(queue);

      size_t size = queue->get_size();

      ELEM value;
      எனில் (size) {
	    get_val_func(queue, value);
      } இல்லையெனில் {
	    dq_default(value, wid);
	    cerr << thr->get_fileline()
	         << "Warning: pop_" << loc << "() on empty "
	         << get_queue_type(value) << "." << endl;
      }

      தள்ளு_மதிப்பு(thr, value, wid);
      திரும்பு உண்மை;
}

template <typename ELEM, வகுப்பு QTYPE>
நிலையான தருக்க qpop_b_prop(vthread_t thr, vvp_code_t cp, unsigned wid)
{
      திரும்பு q_எடு_prop<ELEM, QTYPE>(thr, cp, get_back_மதிப்பு<ELEM>, "back", wid);
}

template <typename ELEM, வகுப்பு QTYPE>
நிலையான தருக்க qpop_f_prop(vthread_t thr, vvp_code_t cp, unsigned wid)
{
      திரும்பு q_எடு_prop<ELEM, QTYPE>(thr, cp, get_front_மதிப்பு<ELEM>, "front", wid);
}

தருக்க of_QPOP_PROP_B_REAL(vthread_t thr, vvp_code_t cp)
{
      திரும்பு qpop_b_prop<இரட்டை, vvp_queue_real>(thr, cp, 0);
}

தருக்க of_QPOP_PROP_B_STR(vthread_t thr, vvp_code_t cp)
{
      திரும்பு qpop_b_prop<string, vvp_queue_string>(thr, cp, 0);
}

தருக்க of_QPOP_PROP_B_V(vthread_t thr, vvp_code_t cp)
{
      திரும்பு qpop_b_prop<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[0]);
}

தருக்க of_QPOP_PROP_F_REAL(vthread_t thr, vvp_code_t cp)
{
      திரும்பு qpop_f_prop<இரட்டை, vvp_queue_real>(thr, cp, 0);
}

தருக்க of_QPOP_PROP_F_STR(vthread_t thr, vvp_code_t cp)
{
      திரும்பு qpop_f_prop<string, vvp_queue_string>(thr, cp, 0);
}

தருக்க of_QPOP_PROP_F_V(vthread_t thr, vvp_code_t cp)
{
      திரும்பு qpop_f_prop<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[0]);
}

நிலையான vvp_vector4_t queue_unique_ulong_to_vec4(unsigned long ul, unsigned bits)
{
      vvp_vector4_t val(bits, BIT4_0);
      க்கு (unsigned idx = 0; idx < bits && idx < 8 * அளவு(unsigned long); idx += 1) {
	    எனில் (ul & 1UL) {
		  val.set_bit(idx, BIT4_1);
	    } இல்லையெனில் {
		  val.set_bit(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      திரும்பு val;
}

template <வகுப்பு SRC> நிலையான vvp_queue_vec4* queue_run_unique_src(SRC* src,
								 unsigned wid,
								 தருக்க as_index)
{
      vvp_queue_vec4* dst = புதிய vvp_queue_vec4();
      எனில் (!src) திரும்பு dst;
      size_t n = src->get_size();
      க்கு (size_t i = 0; i < n; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    தருக்க seen = பொய்;
	    க்கு (size_t j = 0; j < i; j += 1) {
		  vvp_vector4_t vj(wid);
		  src->get_word(j, vj);
		  எனில் (vi.eeq(vj)) {
			seen = உண்மை;
			நிறுத்து;
		  }
	    }
	    எனில் (seen) தொடர்;
	    எனில் (as_index) {
		  dst->push_back(queue_unique_ulong_to_vec4((unsigned long)i, 32), 0);
	    } இல்லையெனில் {
		  dst->push_back(vi, 0);
	    }
      }
      திரும்பு dst;
}

template <வகுப்பு SRC>
நிலையான vvp_queue_vec4* queue_run_find_matches_src(SRC* src, unsigned wid,
						  மாறிலி vvp_vector4_t& cmp,
						  தருக்க as_index)
{
      vvp_queue_vec4* dst = புதிய vvp_queue_vec4();
      எனில் (!src) திரும்பு dst;
      size_t n = src->get_size();
      க்கு (size_t i = 0; i < n; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    எனில் (vi.eeq(cmp)) {
		  எனில் (as_index) {
			dst->push_back(queue_unique_ulong_to_vec4((unsigned long)i, 32), 0);
		  } இல்லையெனில் {
			dst->push_back(vi, 0);
		  }
	    }
      }
      திரும்பு dst;
}

/* Array/queue locator methods return a queue with 0 or 1 elements (LRM). */
template <வகுப்பு SRC>
நிலையான vvp_queue_vec4* queue_find_first_last_to_queue_src(SRC* src,
							  unsigned wid,
							  மாறிலி vvp_vector4_t& cmp,
							  தருக்க want_first,
							  தருக்க want_index)
{
      vvp_queue_vec4* dst = புதிய vvp_queue_vec4();
      எனில் (!src || src->get_size() == 0) திரும்பு dst;
      size_t n = src->get_size();
      எனில் (want_first) {
	    க்கு (size_t i = 0; i < n; i += 1) {
		  vvp_vector4_t vi(wid);
		  src->get_word(i, vi);
		  எனில் (vi.eeq(cmp)) {
			எனில் (want_index) {
			      dst->push_back(
				  queue_unique_ulong_to_vec4((unsigned long)i, 32),
				  0);
			} இல்லையெனில் {
			      dst->push_back(vi, 0);
			}
			திரும்பு dst;
		  }
	    }
      } இல்லையெனில் {
	    க்கு (size_t i = n; i > 0; ) {
		  i -= 1;
		  vvp_vector4_t vi(wid);
		  src->get_word(i, vi);
		  எனில் (vi.eeq(cmp)) {
			எனில் (want_index) {
			      dst->push_back(
				  queue_unique_ulong_to_vec4((unsigned long)i, 32),
				  0);
			} இல்லையெனில் {
			      dst->push_back(vi, 0);
			}
			திரும்பு dst;
		  }
	    }
      }
      திரும்பு dst;
}

/* min()/max() return one element with the selected extremum. */
template <வகுப்பு SRC>
நிலையான vvp_queue_vec4* queue_run_min_max_src(SRC* src, unsigned wid, தருக்க want_max)
{
      vvp_queue_vec4* dst = புதிய vvp_queue_vec4();
      எனில் (!src || src->get_size() == 0) திரும்பு dst;

      vvp_vector4_t best(wid);
      src->get_word(0, best);

      size_t n = src->get_size();
      க்கு (size_t i = 1 ; i < n ; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);

	    vvp_bit4_t ge = compare_gtge(vi, best, BIT4_1);
	    தருக்க replace = want_max ? (ge == BIT4_1) : (ge == BIT4_0);
	    எனில் (replace) best = vi;
      }

      dst->push_back(best, 0);
      திரும்பு dst;
}

தருக்க of_QUEUE_FIND_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->எடு_vec4();
      unsigned wid = cp->bit_idx[0];
      ப்ரதிஜ்ஞா(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      எனில் (qsrc) dst = queue_run_find_matches_src(qsrc, wid, cmp, பொய்);
      இல்லையெனில் எனில் (dsrc) dst = queue_run_find_matches_src(dsrc, wid, cmp, பொய்);
      இல்லையெனில் {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_run_find_matches_src(src, wid, cmp, பொய்);
      }
      thr->தள்ளு_object(vvp_object_t(dst));
      திரும்பு உண்மை;
}

தருக்க of_QUEUE_FIND_PROP_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->எடு_vec4();
      size_t pid = cp->எண்;
      unsigned wid = cp->bit_idx[0];
      ப்ரதிஜ்ஞா(cmp.size() == wid);

      vvp_object_t& உச்சி = thr->peek_object();
      vvp_cobject*cobj = உச்சி.peek<vvp_cobject>();
      ப்ரதிஜ்ஞா(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_find_matches_src(qsrc, wid, cmp, பொய்)
				 : queue_run_find_matches_src(dsrc, wid, cmp, பொய்);
      thr->தள்ளு_object(vvp_object_t(dst));
      திரும்பு உண்மை;
}

தருக்க of_QUEUE_FIND_INDEX_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->எடு_vec4();
      unsigned wid = cp->bit_idx[0];
      ப்ரதிஜ்ஞா(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      எனில் (qsrc) dst = queue_run_find_matches_src(qsrc, wid, cmp, உண்மை);
      இல்லையெனில் எனில் (dsrc) dst = queue_run_find_matches_src(dsrc, wid, cmp, உண்மை);
      இல்லையெனில் {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_run_find_matches_src(src, wid, cmp, உண்மை);
      }
      thr->தள்ளு_object(vvp_object_t(dst));
      திரும்பு உண்மை;
}

தருக்க of_QUEUE_FIND_INDEX_PROP_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->எடு_vec4();
      size_t pid = cp->எண்;
      unsigned wid = cp->bit_idx[0];
      ப்ரதிஜ்ஞா(cmp.size() == wid);

      vvp_object_t& உச்சி = thr->peek_object();
      vvp_cobject*cobj = உச்சி.peek<vvp_cobject>();
      ப்ரதிஜ்ஞா(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_find_matches_src(qsrc, wid, cmp, உண்மை)
				 : queue_run_find_matches_src(dsrc, wid, cmp, உண்மை);
      thr->தள்ளு_object(vvp_object_t(dst));
      திரும்பு உண்மை;
}

தருக்க of_QUEUE_FIND_FIRST_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->எடு_vec4();
      unsigned wid = cp->bit_idx[0];
      ப்ரதிஜ்ஞா(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      எனில் (qsrc) dst = queue_find_first_last_to_queue_src(qsrc, wid, cmp, உண்மை, பொய்);
      இல்லையெனில் எனில் (dsrc) dst = queue_find_first_last_to_queue_src(dsrc, wid, cmp, உண்மை, பொய்);
      இல்லையெனில் {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_find_first_last_to_queue_src(src, wid, cmp, உண்மை, பொய்);
      }
      thr->தள்ளு_object(vvp_object_t(dst));
      திரும்பு உண்மை;
}

தருக்க of_QUEUE_FIND_FIRST_PROP_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->எடு_vec4();
      size_t pid = cp->எண்;
      unsigned wid = cp->bit_idx[0];
      ப்ரதிஜ்ஞா(cmp.size() == wid);

      vvp_object_t& உச்சி = thr->peek_object();
      vvp_cobject*cobj = உச்சி.peek<vvp_cobject>();
      ப்ரதிஜ்ஞா(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst =
	    qsrc ? queue_find_first_last_to_queue_src(qsrc, wid, cmp, உண்மை, பொய்)
		 : queue_find_first_last_to_queue_src(dsrc, wid, cmp, உண்மை, பொய்);
      thr->தள்ளு_object(vvp_object_t(dst));
      திரும்பு உண்மை;
}

தருக்க of_QUEUE_FIND_FIRST_INDEX_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->எடு_vec4();
      unsigned wid = cp->bit_idx[0];
      ப்ரதிஜ்ஞா(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      எனில் (qsrc) dst = queue_find_first_last_to_queue_src(qsrc, wid, cmp, உண்மை, உண்மை);
      இல்லையெனில் எனில் (dsrc) dst = queue_find_first_last_to_queue_src(dsrc, wid, cmp, உண்மை, உண்மை);
      இல்லையெனில் {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_find_first_last_to_queue_src(src, wid, cmp, உண்மை, உண்மை);
      }
      thr->தள்ளு_object(vvp_object_t(dst));
      திரும்பு உண்மை;
}

தருக்க of_QUEUE_FIND_FIRST_INDEX_PROP_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->எடு_vec4();
      size_t pid = cp->எண்;
      unsigned wid = cp->bit_idx[0];
      ப்ரதிஜ்ஞா(cmp.size() == wid);

      vvp_object_t& உச்சி = thr->peek_object();
      vvp_cobject*cobj = உச்சி.peek<vvp_cobject>();
      ப்ரதிஜ்ஞா(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst =
	    qsrc ? queue_find_first_last_to_queue_src(qsrc, wid, cmp, உண்மை, உண்மை)
		 : queue_find_first_last_to_queue_src(dsrc, wid, cmp, உண்மை, உண்மை);
      thr->தள்ளு_object(vvp_object_t(dst));
      திரும்பு உண்மை;
}

தருக்க of_QUEUE_FIND_LAST_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->எடு_vec4();
      unsigned wid = cp->bit_idx[0];
      ப்ரதிஜ்ஞா(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      எனில் (qsrc) dst = queue_find_first_last_to_queue_src(qsrc, wid, cmp, பொய், பொய்);
      இல்லையெனில் எனில் (dsrc) dst = queue_find_first_last_to_queue_src(dsrc, wid, cmp, பொய், பொய்);
      இல்லையெனில் {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_find_first_last_to_queue_src(src, wid, cmp, பொய், பொய்);
      }
      thr->தள்ளு_object(vvp_object_t(dst));
      திரும்பு உண்மை;
}

தருக்க of_QUEUE_FIND_LAST_PROP_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->எடு_vec4();
      size_t pid = cp->எண்;
      unsigned wid = cp->bit_idx[0];
      ப்ரதிஜ்ஞா(cmp.size() == wid);

      vvp_object_t& உச்சி = thr->peek_object();
      vvp_cobject*cobj = உச்சி.peek<vvp_cobject>();
      ப்ரதிஜ்ஞா(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst =
	    qsrc ? queue_find_first_last_to_queue_src(qsrc, wid, cmp, பொய், பொய்)
		 : queue_find_first_last_to_queue_src(dsrc, wid, cmp, பொய், பொய்);
      thr->தள்ளு_object(vvp_object_t(dst));
      திரும்பு உண்மை;
}

தருக்க of_QUEUE_FIND_LAST_INDEX_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->எடு_vec4();
      unsigned wid = cp->bit_idx[0];
      ப்ரதிஜ்ஞா(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      எனில் (qsrc) dst = queue_find_first_last_to_queue_src(qsrc, wid, cmp, பொய், உண்மை);
      இல்லையெனில் எனில் (dsrc) dst = queue_find_first_last_to_queue_src(dsrc, wid, cmp, பொய், உண்மை);
      இல்லையெனில் {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_find_first_last_to_queue_src(src, wid, cmp, பொய், உண்மை);
      }
      thr->தள்ளு_object(vvp_object_t(dst));
      திரும்பு உண்மை;
}

தருக்க of_QUEUE_FIND_LAST_INDEX_PROP_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->எடு_vec4();
      size_t pid = cp->எண்;
      unsigned wid = cp->bit_idx[0];
      ப்ரதிஜ்ஞா(cmp.size() == wid);

      vvp_object_t& உச்சி = thr->peek_object();
      vvp_cobject*cobj = உச்சி.peek<vvp_cobject>();
      ப்ரதிஜ்ஞா(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst =
	    qsrc ? queue_find_first_last_to_queue_src(qsrc, wid, cmp, பொய், உண்மை)
		 : queue_find_first_last_to_queue_src(dsrc, wid, cmp, பொய், உண்மை);
      thr->தள்ளு_object(vvp_object_t(dst));
      திரும்பு உண்மை;
}

தருக்க of_QUEUE_MIN_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      எனில் (qsrc) dst = queue_run_min_max_src(qsrc, wid, பொய்);
      இல்லையெனில் எனில் (dsrc) dst = queue_run_min_max_src(dsrc, wid, பொய்);
      இல்லையெனில் {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_run_min_max_src(src, wid, பொய்);
      }
      thr->தள்ளு_object(vvp_object_t(dst));
      திரும்பு உண்மை;
}

தருக்க of_QUEUE_MIN_OBJ_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->எடு_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_min_max_src(qsrc, wid, பொய்)
				 : queue_run_min_max_src(dsrc, wid, பொய்);
      thr->தள்ளு_object(vvp_object_t(dst));
      திரும்பு உண்மை;
}

தருக்க of_QUEUE_MIN_PROP_V(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->எண்;
      unsigned wid = cp->bit_idx[0];

      vvp_object_t& உச்சி = thr->peek_object();
      vvp_cobject*cobj = உச்சி.peek<vvp_cobject>();
      ப்ரதிஜ்ஞா(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_min_max_src(qsrc, wid, பொய்)
				 : queue_run_min_max_src(dsrc, wid, பொய்);
      thr->தள்ளு_object(vvp_object_t(dst));
      திரும்பு உண்மை;
}

தருக்க of_QUEUE_MAX_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      எனில் (qsrc) dst = queue_run_min_max_src(qsrc, wid, உண்மை);
      இல்லையெனில் எனில் (dsrc) dst = queue_run_min_max_src(dsrc, wid, உண்மை);
      இல்லையெனில் {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_run_min_max_src(src, wid, உண்மை);
      }
      thr->தள்ளு_object(vvp_object_t(dst));
      திரும்பு உண்மை;
}

தருக்க of_QUEUE_MAX_OBJ_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->எடு_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_min_max_src(qsrc, wid, உண்மை)
				 : queue_run_min_max_src(dsrc, wid, உண்மை);
      thr->தள்ளு_object(vvp_object_t(dst));
      திரும்பு உண்மை;
}

தருக்க of_QUEUE_MAX_PROP_V(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->எண்;
      unsigned wid = cp->bit_idx[0];

      vvp_object_t& உச்சி = thr->peek_object();
      vvp_cobject*cobj = உச்சி.peek<vvp_cobject>();
      ப்ரதிஜ்ஞா(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_min_max_src(qsrc, wid, உண்மை)
				 : queue_run_min_max_src(dsrc, wid, உண்மை);
      thr->தள்ளு_object(vvp_object_t(dst));
      திரும்பு உண்மை;
}

தருக்க of_QUEUE_UNIQUE_OBJ_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->எடு_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_unique_src(qsrc, wid, பொய்)
				 : queue_run_unique_src(dsrc, wid, பொய்);
      thr->தள்ளு_object(vvp_object_t(dst));
      திரும்பு உண்மை;
}

தருக்க of_QUEUE_UNIQUE_INDEX_OBJ_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->எடு_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_unique_src(qsrc, wid, உண்மை)
				 : queue_run_unique_src(dsrc, wid, உண்மை);
      thr->தள்ளு_object(vvp_object_t(dst));
      திரும்பு உண்மை;
}

தருக்க of_QUEUE_UNIQUE_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      எனில் (qsrc) dst = queue_run_unique_src(qsrc, wid, பொய்);
      இல்லையெனில் எனில் (dsrc) dst = queue_run_unique_src(dsrc, wid, பொய்);
      இல்லையெனில் {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_run_unique_src(src, wid, பொய்);
      }
      thr->தள்ளு_object(vvp_object_t(dst));
      திரும்பு உண்மை;
}

தருக்க of_QUEUE_UNIQUE_PROP_V(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->எண்;
      unsigned wid = cp->bit_idx[0];

      vvp_object_t& உச்சி = thr->peek_object();
      vvp_cobject*cobj = உச்சி.peek<vvp_cobject>();
      ப்ரதிஜ்ஞா(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_unique_src(qsrc, wid, பொய்)
				 : queue_run_unique_src(dsrc, wid, பொய்);
      thr->தள்ளு_object(vvp_object_t(dst));
      திரும்பு உண்மை;
}

தருக்க of_QUEUE_UNIQUE_INDEX_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      எனில் (qsrc) dst = queue_run_unique_src(qsrc, wid, உண்மை);
      இல்லையெனில் எனில் (dsrc) dst = queue_run_unique_src(dsrc, wid, உண்மை);
      இல்லையெனில் {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_run_unique_src(src, wid, உண்மை);
      }
      thr->தள்ளு_object(vvp_object_t(dst));
      திரும்பு உண்மை;
}

தருக்க of_QUEUE_UNIQUE_INDEX_PROP_V(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->எண்;
      unsigned wid = cp->bit_idx[0];

      vvp_object_t& உச்சி = thr->peek_object();
      vvp_cobject*cobj = உச்சி.peek<vvp_cobject>();
      ப்ரதிஜ்ஞா(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_unique_src(qsrc, wid, உண்மை)
				 : queue_run_unique_src(dsrc, wid, உண்மை);
      thr->தள்ளு_object(vvp_object_t(dst));
      திரும்பு உண்மை;
}

தருக்க of_PROP_QUEUE_SIZE(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->எண்;

      vvp_object_t& உச்சி = thr->peek_object();
      vvp_cobject*cobj = உச்சி.peek<vvp_cobject>();
      ப்ரதிஜ்ஞா(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);

      size_t sz = qsrc ? qsrc->get_size() : (dsrc ? dsrc->get_size() : 0);

      vvp_vector4_t val(32, BIT4_0);
      unsigned long ul = sz;
      க்கு (unsigned idx = 0; idx < 32; idx++) {
	    எனில் (ul & 1UL) {
		  val.set_bit(idx, BIT4_1);
	    } இல்லையெனில் {
		  val.set_bit(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      thr->தள்ளு_vec4(val);
      திரும்பு உண்மை;
}

தருக்க of_QUEUE_SIZE_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      size_t sz =
	    qsrc ? qsrc->get_size() : (dsrc ? dsrc->get_size() : 0);

      vvp_vector4_t val(32, BIT4_0);
      unsigned long ul = sz;
      க்கு (unsigned idx = 0; idx < 32; idx++) {
	    எனில் (ul & 1UL) {
		  val.set_bit(idx, BIT4_1);
	    } இல்லையெனில் {
		  val.set_bit(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      thr->தள்ளு_vec4(val);
      திரும்பு உண்மை;
}

தருக்க of_QUEUE_WORD_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      unsigned ix_reg = cp->bit_idx[1];
      int64_t adr = thr->சொற்கள்[ix_reg].w_int;
      எனில் (thr->flags[4] == BIT4_1) {
	    thr->தள்ளு_vec4(vvp_vector4_t(wid));
	    திரும்பு உண்மை;
      }
      எனில் (adr < 0) {
	    thr->தள்ளு_vec4(vvp_vector4_t(wid));
	    திரும்பு உண்மை;
      }
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t out(wid);
      எனில் (qsrc) {
	    எனில் ((size_t)adr >= qsrc->get_size()) {
		  thr->தள்ளு_vec4(out);
		  திரும்பு உண்மை;
	    }
	    qsrc->get_word((unsigned)adr, out);
      } இல்லையெனில் எனில் (dsrc) {
	    எனில் ((size_t)adr >= dsrc->get_size()) {
		  thr->தள்ளு_vec4(out);
		  திரும்பு உண்மை;
	    }
	    dsrc->get_word((unsigned)adr, out);
      } இல்லையெனில் {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    எனில் (!src || (size_t)adr >= src->get_size()) {
		  thr->தள்ளு_vec4(out);
		  திரும்பு உண்மை;
	    }
	    src->get_word((unsigned)adr, out);
      }
      thr->தள்ளு_vec4(out);
      திரும்பு உண்மை;
}

தருக்க of_QUEUE_WORD_PROP_V(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->எண்;
      unsigned wid = cp->bit_idx[0];
      unsigned ix_reg = cp->bit_idx[1];
      int64_t adr = thr->சொற்கள்[ix_reg].w_int;
      எனில் (thr->flags[4] == BIT4_1) {
	    thr->தள்ளு_vec4(vvp_vector4_t(wid));
	    திரும்பு உண்மை;
      }
      எனில் (adr < 0) {
	    thr->தள்ளு_vec4(vvp_vector4_t(wid));
	    திரும்பு உண்மை;
      }

      vvp_object_t saved_உச்சி;
      தருக்க popped_உச்சி = பொய்;
      vvp_object_t& உச்சி = thr->peek_object();
      vvp_cobject*cobj = உச்சி.peek<vvp_cobject>();
      எனில் (cobj == 0) {
	    // Array-locator with(property) keeps an accumulator queue on top.
	    // Temporarily pop it to access the class object beneath.
	    thr->எடு_object(saved_உச்சி);
	    popped_உச்சி = உண்மை;
	    vvp_object_t& below = thr->peek_object();
	    cobj = below.peek<vvp_cobject>();
      }
      எனில் (cobj == 0) {
	    எனில் (popped_உச்சி) thr->தள்ளு_object(saved_உச்சி);
	    thr->தள்ளு_vec4(vvp_vector4_t(wid));
	    திரும்பு உண்மை;
      }

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t out(wid);
      எனில் (qsrc) {
	    எனில் ((size_t)adr >= qsrc->get_size()) {
		  thr->தள்ளு_vec4(out);
		  திரும்பு உண்மை;
	    }
	    qsrc->get_word((unsigned)adr, out);
      } இல்லையெனில் எனில் (dsrc) {
	    எனில் ((size_t)adr >= dsrc->get_size()) {
		  thr->தள்ளு_vec4(out);
		  திரும்பு உண்மை;
	    }
	    dsrc->get_word((unsigned)adr, out);
      } இல்லையெனில் {
	    thr->தள்ளு_vec4(out);
	    எனில் (popped_உச்சி) thr->தள்ளு_object(saved_உச்சி);
	    திரும்பு உண்மை;
      }
      எனில் (popped_உச்சி) thr->தள்ளு_object(saved_உச்சி);
      thr->தள்ளு_vec4(out);
      திரும்பு உண்மை;
}

தருக்க of_CMPIX_LTU(vthread_t thr, vvp_code_t cp)
{
      unsigned a = cp->எண்;
      unsigned b = cp->bit_idx[0];
      எனில் (thr->சொற்கள்[a].w_uint < thr->சொற்கள்[b].w_uint) {
	    thr->flags[4] = BIT4_1;
      } இல்லையெனில் {
	    thr->flags[4] = BIT4_0;
      }
      திரும்பு உண்மை;
}

தருக்க of_CMPIX_SLT0(vthread_t thr, vvp_code_t cp)
{
      unsigned r = cp->எண்;
      thr->flags[4] = (thr->சொற்கள்[r].w_int < 0) ? BIT4_1 : BIT4_0;
      திரும்பு உண்மை;
}

தருக்க of_தள்ளு_IX_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned ix = cp->எண்;
      unsigned wid = cp->bit_idx[0];
      unsigned sgn = cp->bit_idx[1];
      uint32_t ubits;
      எனில் (sgn) {
	    int32_t sv = (int32_t)thr->சொற்கள்[ix].w_int;
	    ubits = (uint32_t)sv;
      } இல்லையெனில் {
	    ubits = (uint32_t)thr->சொற்கள்[ix].w_uint;
      }
      எனில் (wid > 32) wid = 32;
      vvp_vector4_t tmp(wid, BIT4_0);
      க்கு (unsigned idx = 0; idx < wid; idx++) {
	    எனில் (ubits & 1u) {
		  tmp.set_bit(idx, BIT4_1);
	    } இல்லையெனில் {
		  tmp.set_bit(idx, BIT4_0);
	    }
	    ubits >>= 1;
      }
      thr->தள்ளு_vec4(tmp);
      திரும்பு உண்மை;
}

தருக்க of_QUEUE_NEW_EMPTY_V(vthread_t thr, vvp_code_t)
{
      thr->தள்ளு_object(vvp_object_t(புதிய vvp_queue_vec4()));
      திரும்பு உண்மை;
}

தருக்க of_QUEUE_APPEND_WORD_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      vvp_vector4_t elem = thr->எடு_vec4();
      ப்ரதிஜ்ஞா(elem.size() == wid);
      vvp_object_t qobj;
      thr->எடு_object(qobj);
      vvp_queue_vec4* q = qobj.peek<vvp_queue_vec4>();
      ப்ரதிஜ்ஞா(q);
      q->push_back(elem, 0);
      thr->தள்ளு_object(qobj);
      திரும்பு உண்மை;
}

தருக்க of_DELETE_PROP_ELEM(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->எண்;

      int64_t idx_val = thr->சொற்கள்[3].w_int;
      எனில் (thr->flags[4] == BIT4_1) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with undefined index."
	         << endl;
	    திரும்பு உண்மை;
      }
      எனில் (idx_val < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with negative index."
	         << endl;
	    திரும்பு உண்மை;
      }
      size_t idx = idx_val;

      vvp_object_t& உச்சி = thr->peek_object();
      vvp_cobject*cobj = உச்சி.peek<vvp_cobject>();
      ப்ரதிஜ்ஞா(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue* queue = qobj.peek<vvp_queue>();
      எனில் (queue == 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping delete(" << idx
	         << ") on empty queue." << endl;
      } இல்லையெனில் {
	    size_t size = queue->get_size();
	    எனில் (idx >= size) {
		  cerr << thr->get_fileline()
		       << "Warning: skipping out of range delete(" << idx
		       << ") on queue of size " << size << "." << endl;
	    } இல்லையெனில் {
		  queue->erase(idx);
	    }
      }

      திரும்பு உண்மை;
}

தருக்க of_DELETE_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->எண்;

      vvp_object_t& உச்சி = thr->peek_object();
      vvp_cobject*cobj = உச்சி.peek<vvp_cobject>();
      ப்ரதிஜ்ஞா(cobj);

      cobj->set_object(pid, vvp_object_t(), 0);

      திரும்பு உண்மை;
}

template <typename ELEM, வகுப்பு QTYPE>
நிலையான தருக்க store_qb(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
      ELEM value;
      vvp_net_t*net = cp->net;
      unsigned max_size = thr->சொற்கள்[cp->bit_idx[0]].w_int;
      எடு_மதிப்பு(thr, value, wid); // Pop the value to store.

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      ப்ரதிஜ்ஞா(queue);
      queue->push_back(value, max_size);
      திரும்பு உண்மை;
}

/*
 * %store/qb/r <var-label>, <max-idx>
 */
தருக்க of_STORE_QB_R(vthread_t thr, vvp_code_t cp)
{
      திரும்பு store_qb<இரட்டை, vvp_queue_real>(thr, cp);
}

/*
 * %store/qb/str <var-label>, <max-idx>
 */
தருக்க of_STORE_QB_STR(vthread_t thr, vvp_code_t cp)
{
      திரும்பு store_qb<string, vvp_queue_string>(thr, cp);
}

/*
 * %store/qb/v <var-label>, <max-idx>, <wid>
 */
தருக்க of_STORE_QB_V(vthread_t thr, vvp_code_t cp)
{
      திரும்பு store_qb<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

template <typename ELEM, வகுப்பு QTYPE>
நிலையான தருக்க store_qdar(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
      int64_t idx = thr->சொற்கள்[3].w_int;
      ELEM value;
      vvp_net_t*net = cp->net;
      எடு_மதிப்பு(thr, value, wid); // Pop the value to store.

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      ப்ரதிஜ்ஞா(queue);
      எனில் (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot assign to a negative "
	         << get_queue_type(value)
	         << " index (" << idx << "). ";
	    print_queue_மதிப்பு(value);
	    cerr << " was not added." << endl;
      } இல்லையெனில் எனில் (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot assign to an undefined "
	         << get_queue_type(value) << " index. ";
	    print_queue_மதிப்பு(value);
	    cerr << " was not added." << endl;
      } இல்லையெனில் {
	    unsigned max_size = thr->சொற்கள்[cp->bit_idx[0]].w_int;
	    queue->set_word_max(idx, value, max_size);
      }
      திரும்பு உண்மை;
}

/*
 * %store/qdar/r <var>, idx
 */
தருக்க of_STORE_QDAR_R(vthread_t thr, vvp_code_t cp)
{
      திரும்பு store_qdar<இரட்டை, vvp_queue_real>(thr, cp);
}

/*
 * %store/qdar/str <var>, idx
 */
தருக்க of_STORE_QDAR_STR(vthread_t thr, vvp_code_t cp)
{
      திரும்பு store_qdar<string, vvp_queue_string>(thr, cp);
}

/*
 * %store/qdar/v <var>, idx
 */
தருக்க of_STORE_QDAR_V(vthread_t thr, vvp_code_t cp)
{
      திரும்பு store_qdar<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

template <typename ELEM, வகுப்பு QTYPE>
நிலையான தருக்க store_qf(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
      ELEM value;
      vvp_net_t*net = cp->net;
      unsigned max_size = thr->சொற்கள்[cp->bit_idx[0]].w_int;
      எடு_மதிப்பு(thr, value, wid); // Pop the value to store.

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      ப்ரதிஜ்ஞா(queue);
      queue->push_front(value, max_size);
      திரும்பு உண்மை;
}
/*
 * %store/qf/r <var-label>, <max-idx>
 */
தருக்க of_STORE_QF_R(vthread_t thr, vvp_code_t cp)
{
      திரும்பு store_qf<இரட்டை, vvp_queue_real>(thr, cp);
}

/*
 * %store/qf/str <var-label>, <max-idx>
 */
தருக்க of_STORE_QF_STR(vthread_t thr, vvp_code_t cp)
{
      திரும்பு store_qf<string, vvp_queue_string>(thr, cp);
}

/*
 * %store/qb/v <var-label>, <max-idx>, <wid>
 */
தருக்க of_STORE_QF_V(vthread_t thr, vvp_code_t cp)
{
      திரும்பு store_qf<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

template <typename ELEM, வகுப்பு QTYPE>
நிலையான தருக்க store_qobj(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
// FIXME: Can we actually use wid here?
      (வெறுமை)wid;
      vvp_net_t*net = cp->net;

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      ப்ரதிஜ்ஞா(queue);

      vvp_object_t src;
      thr->எடு_object(src);

        // If it is null just clear the queue
      எனில் (src.test_nil())
	    queue->erase_tail(0);
      இல்லையெனில் {
	    unsigned max_size = thr->சொற்கள்[cp->bit_idx[0]].w_int;
	    queue->copy_elems(src, max_size);
      }

      திரும்பு உண்மை;
}

தருக்க of_STORE_QOBJ_R(vthread_t thr, vvp_code_t cp)
{
      திரும்பு store_qobj<இரட்டை, vvp_queue_real>(thr, cp);
}

தருக்க of_STORE_QOBJ_STR(vthread_t thr, vvp_code_t cp)
{
      திரும்பு store_qobj<string, vvp_queue_string>(thr, cp);
}

தருக்க of_STORE_QOBJ_V(vthread_t thr, vvp_code_t cp)
{
      திரும்பு store_qobj<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

நிலையான வெறுமை vvp_send(vthread_t thr, vvp_net_ptr_t ptr, மாறிலி இரட்டை&val)
{
      vvp_send_real(ptr, val, thr->wt_context);
}

நிலையான வெறுமை vvp_send(vthread_t thr, vvp_net_ptr_t ptr, மாறிலி string&val)
{
      vvp_send_string(ptr, val, thr->wt_context);
}

template <typename ELEM>
நிலையான தருக்க store(vthread_t thr, vvp_code_t cp)
{
      ELEM val;
      எடு_மதிப்பு(thr, val, 0);
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_send(thr, ptr, val);
      திரும்பு உண்மை;
}

தருக்க of_STORE_REAL(vthread_t thr, vvp_code_t cp)
{
      திரும்பு store<இரட்டை>(thr, cp);
}

template <typename ELEM>
நிலையான தருக்க storea(vthread_t thr, vvp_code_t cp)
{
      unsigned idx = cp->bit_idx[0];
      ELEM val;
      எடு_மதிப்பு(thr, val, 0);

      எனில் (thr->flags[4] != BIT4_1) {
	    unsigned adr = thr->சொற்கள்[idx].w_int;
	    cp->array->set_word(adr, val);
      }

      திரும்பு உண்மை;
}

/*
 * %store/reala <var-label> <index>
 */
தருக்க of_STORE_REALA(vthread_t thr, vvp_code_t cp)
{
      திரும்பு storea<இரட்டை>(thr, cp);
}

தருக்க of_STORE_STR(vthread_t thr, vvp_code_t cp)
{
      திரும்பு store<string>(thr, cp);
}

/*
 * %store/stra <array-label> <index>
 */
தருக்க of_STORE_STRA(vthread_t thr, vvp_code_t cp)
{
      திரும்பு storea<string>(thr, cp);
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
தருக்க of_STORE_VEC4(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr(cp->net, 0);
      மாறிலி vvp_signal_மதிப்பு*sig = dynamic_cast<vvp_signal_மதிப்பு*> (cp->net->fil);
      unsigned off_index = cp->bit_idx[0];
      unsigned முழு wid = cp->bit_idx[1];

      int64_t off = off_index ? thr->சொற்கள்[off_index].w_int : 0;
      unsigned முழு sig_மதிப்பு_size = sig->மதிப்பு_size();

      vvp_vector4_t&val = thr->peek_vec4();
      unsigned val_size = val.size();

      எனில் (val_size < wid) {
	    cerr << thr->get_fileline()
	         << "XXXX Internal error: val.size()=" << val_size
		 << ", expecting >= " << wid << endl;
      }
      ப்ரதிஜ்ஞா(val_size >= wid);
      எனில் (val_size > wid) {
	    val.resize(wid);
      }

	// If there is a problem loading the index register, flags-4
	// will be set to 1, and we know here to skip the actual assignment.
      எனில் (off_index!=0 && thr->flags[4] == BIT4_1) {
	    thr->எடு_vec4(1);
	    திரும்பு உண்மை;
      }

      எனில் (!resize_rval_vec(val, off, sig_மதிப்பு_size)) {
	    thr->எடு_vec4(1);
	    திரும்பு உண்மை;
      }

      எனில் (off == 0 && val.size() == sig_மதிப்பு_size)
	    vvp_send_vec4(ptr, val, thr->wt_context);
      இல்லையெனில்
	    vvp_send_vec4_pv(ptr, val, off, sig_மதிப்பு_size, thr->wt_context);

      thr->எடு_vec4(1);
      திரும்பு உண்மை;
}

/*
 * %store/vec4a <var-label>, <addr>, <offset>
 */
தருக்க of_STORE_VEC4A(vthread_t thr, vvp_code_t cp)
{
      unsigned adr_index = cp->bit_idx[0];
      unsigned off_index = cp->bit_idx[1];

      long adr = adr_index? thr->சொற்கள்[adr_index].w_int : 0;
      int64_t off = off_index ? thr->சொற்கள்[off_index].w_int : 0;

	// Suppress action if flags-4 is true.
      எனில் (thr->flags[4] == BIT4_1) {
	    thr->எடு_vec4(1);
	    திரும்பு உண்மை;
      }

      vvp_vector4_t &value = thr->peek_vec4();

      எனில் (!resize_rval_vec(value, off, cp->array->get_word_size())) {
	    thr->எடு_vec4(1);
	    திரும்பு உண்மை;
      }

      cp->array->set_word(adr, off, value);

      thr->எடு_vec4(1);
      திரும்பு உண்மை;
}

/*
 * %sub
 *   pop r;
 *   pop l;
 *   push l-r;
 */
தருக்க of_SUB(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t r = thr->எடு_vec4();
      vvp_vector4_t&l = thr->peek_vec4();

      l.sub(r);
      திரும்பு உண்மை;
}

/*
 * %subi <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
தருக்க of_SUBI(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->எண்;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_immediate_rval (cp, r);

      l.sub(r);

      திரும்பு உண்மை;

}

தருக்க of_SUB_WR(vthread_t thr, vvp_code_t)
{
      இரட்டை r = thr->எடு_real();
      இரட்டை l = thr->எடு_real();
      thr->தள்ளு_real(l - r);
      திரும்பு உண்மை;
}

/*
 * %substr <first>, <last>
 * Pop a string, take the substring (SystemVerilog style), and return
 * the result to the stack. This opcode actually works by editing the
 * string in place.
 */
தருக்க of_SUBSTR(vthread_t thr, vvp_code_t cp)
{
      int32_t first = thr->சொற்கள்[cp->bit_idx[0]].w_int;
      int32_t last = thr->சொற்கள்[cp->bit_idx[1]].w_int;
      string&val = thr->peek_str(0);

      எனில் (first < 0 || last < first || last >= (int32_t)val.size()) {
	    val = string("");
	    திரும்பு உண்மை;
      }

      val = val.substr(first, last-first+1);
      திரும்பு உண்மை;
}

/*
 * %substr/vec4 <index>, <wid>
 */
தருக்க of_SUBSTR_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned sel_idx = cp->bit_idx[0];
      unsigned wid = cp->bit_idx[1];

      int32_t sel = thr->சொற்கள்[sel_idx].w_int;
      மாறிலி string&val = thr->peek_str(0);

      ப்ரதிஜ்ஞா(wid%8 == 0);

      எனில் (sel < 0 || sel >= (int32_t)val.size()) {
	    vvp_vector4_t res (wid, BIT4_0);
	    thr->தள்ளு_vec4(res);
	    திரும்பு உண்மை;
      }

      vvp_vector4_t res (wid, BIT4_0);

      ப்ரதிஜ்ஞா(wid==8);
      unsigned எழுத்து tmp = val[sel];
      க்கு (முழு idx = 0 ; idx < 8 ; idx += 1) {
	    எனில் (tmp & (1<<idx))
		  res.set_bit(idx, BIT4_1);
      }

      thr->தள்ளு_vec4(res);
      திரும்பு உண்மை;
}

தருக்க of_FILE_வரி(vthread_t thr, vvp_code_t cp)
{
      vpiHandle handle = cp->handle;

	/* When it is available, keep the file/line information in the
	   thread for error/warning messages. */
      thr->set_fileline(vpi_get_str(vpiFile, handle),
                        vpi_get(vpi_வரி_No, handle));

      எனில் (show_file_வரி)
	    cerr << thr->get_fileline()
	         << vpi_get_str(_vpiDescription, handle) << endl;

      திரும்பு உண்மை;
}

/*
 * %test_nul <var-label>;
 * Test if the object at the specified variable is nil. If so, write
 * "1" into flags[4], otherwise write "0" into flags[4].
 */
தருக்க of_TEST_NUL(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      ப்ரதிஜ்ஞா(net);
      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      ப்ரதிஜ்ஞா(obj);

      எனில் (obj->get_object().test_nil())
	    thr->flags[4] = BIT4_1;
      இல்லையெனில்
	    thr->flags[4] = BIT4_0;

      திரும்பு உண்மை;
}

தருக்க of_TEST_NUL_A(vthread_t thr, vvp_code_t cp)
{
      unsigned idx = cp->bit_idx[0];
      unsigned adr = thr->சொற்கள்[idx].w_int;
      vvp_object_t word;

	/* If the address is undefined, return true. */
      எனில் (thr->flags[4] == BIT4_1) {
	    திரும்பு உண்மை;
      }

      cp->array->get_word_obj(adr, word);
      எனில் (word.test_nil())
	    thr->flags[4] = BIT4_1;
      இல்லையெனில்
	    thr->flags[4] = BIT4_0;

      திரும்பு உண்மை;
}

தருக்க of_TEST_NUL_OBJ(vthread_t thr, vvp_code_t)
{
      எனில் (thr->peek_object().test_nil())
	    thr->flags[4] = BIT4_1;
      இல்லையெனில்
	    thr->flags[4] = BIT4_0;
      திரும்பு உண்மை;
}

/*
 * %test_nul/prop <pid>, <idx>
 */
தருக்க of_TEST_NUL_PROP(vthread_t thr, vvp_code_t cp)
{
      unsigned pid = cp->எண்;
      unsigned idx = cp->bit_idx[0];

      எனில் (idx != 0) {
	    ப்ரதிஜ்ஞா(idx < vthread_s::சொற்கள்_மொத்தம்);
	    idx = thr->சொற்கள்[idx].w_uint;
      }

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj  = obj.peek<vvp_cobject>();

      vvp_object_t val;
      cobj->get_object(pid, val, idx);

      எனில் (val.test_nil())
	    thr->flags[4] = BIT4_1;
      இல்லையெனில்
	    thr->flags[4] = BIT4_0;

      திரும்பு உண்மை;
}

/* and() reduction returns the scalar sum (Verilog-style bitwise-AND per word). */
template <வகுப்பு SRC>
நிலையான vvp_vector4_t uparr_and_சொற்கள்_src(SRC* src, unsigned wid)
{
      எனில் (!src) திரும்பு vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_1);
      க்கு (size_t i = 0; i < src->get_size(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    res &= vi;
      }
      திரும்பு res;
}

தருக்க of_UPARR_AND_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      எனில் (qsrc) res = uparr_and_சொற்கள்_src(qsrc, wid);
      இல்லையெனில் எனில் (dsrc) res = uparr_and_சொற்கள்_src(dsrc, wid);
      இல்லையெனில் {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    res = uparr_and_சொற்கள்_src(src, wid);
      }
      thr->தள்ளு_vec4(res);
      திரும்பு உண்மை;
}

/* or() reduction returns the scalar sum (Verilog-style bitwise-AND per word). */
template <வகுப்பு SRC>
நிலையான vvp_vector4_t uparr_or_சொற்கள்_src(SRC* src, unsigned wid)
{
      எனில் (!src) திரும்பு vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_0);
      க்கு (size_t i = 0; i < src->get_size(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    res |= vi;
      }
      திரும்பு res;
}

தருக்க of_UPARR_OR_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      எனில் (qsrc) res = uparr_or_சொற்கள்_src(qsrc, wid);
      இல்லையெனில் எனில் (dsrc) res = uparr_or_சொற்கள்_src(dsrc, wid);
      இல்லையெனில் {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    res = uparr_or_சொற்கள்_src(src, wid);
      }
      thr->தள்ளு_vec4(res);
      திரும்பு உண்மை;
}

/* real product() reduction returns the scalar sum */
template <வகுப்பு SRC>
நிலையான இரட்டை uparr_product_சொற்கள்_src_real(SRC* src)
{
      எனில் (!src) திரும்பு 0.0;
      இரட்டை acc = 1.0;
      க்கு (size_t i = 0; i < src->get_size(); i += 1) {
	    இரட்டை vi;
	    src->get_word(i, vi);
	    acc *= vi;
      }
      திரும்பு acc;
}

தருக்க of_UPARR_PRODUCT_REAL(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_queue_real* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_real_from_net(thr, net, qsrc, dsrc);
      இரட்டை sum;
      எனில் (qsrc) sum = uparr_product_சொற்கள்_src_real(qsrc);
      இல்லையெனில் எனில் (dsrc) sum = uparr_product_சொற்கள்_src_real(dsrc);
      இல்லையெனில் {
	    vvp_queue* src_q = get_queue_object<vvp_queue_real>(thr, net);
	    vvp_queue_real* src = dynamic_cast<vvp_queue_real*>(src_q);
	    sum = uparr_product_சொற்கள்_src_real(src);
      }
      thr->தள்ளு_real(sum);
      திரும்பு உண்மை;
}

/* product() reduction returns the scalar product (Verilog-style multiplication per word). */
template <வகுப்பு SRC>
நிலையான vvp_vector4_t uparr_product_சொற்கள்_src(SRC* src, unsigned wid)
{
      எனில் (!src) திரும்பு vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t acc = queue_unique_ulong_to_vec4(1UL, wid);
      க்கு (size_t i = 0; i < src->get_size(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    acc.mul(vi);
      }
      திரும்பு acc;
}

தருக்க of_UPARR_PRODUCT_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t prod;
      எனில் (qsrc) prod = uparr_product_சொற்கள்_src(qsrc, wid);
      இல்லையெனில் எனில் (dsrc) prod = uparr_product_சொற்கள்_src(dsrc, wid);
      இல்லையெனில் {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    prod = uparr_product_சொற்கள்_src(src, wid);
      }
      thr->தள்ளு_vec4(prod);
      திரும்பு உண்மை;
}

தருக்க of_UPARR_PRODUCT_PROP_V(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->எண்;
      unsigned wid = cp->bit_idx[0];

      vvp_object_t& உச்சி = thr->peek_object();
      vvp_cobject*cobj = உச்சி.peek<vvp_cobject>();
      ப்ரதிஜ்ஞா(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t prod =
	    qsrc ? uparr_product_சொற்கள்_src(qsrc, wid)
		 : uparr_product_சொற்கள்_src(dsrc, wid);
      thr->தள்ளு_vec4(prod);
      திரும்பு உண்மை;
}

தருக்க of_UPARR_PRODUCT_OBJ_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->எடு_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_vector4_t prod =
	    qsrc ? uparr_product_சொற்கள்_src(qsrc, wid)
		 : uparr_product_சொற்கள்_src(dsrc, wid);
      thr->தள்ளு_vec4(prod);
      திரும்பு உண்மை;
}

/* real sum() reduction returns the scalar sum */
template <வகுப்பு SRC>
நிலையான இரட்டை uparr_sum_சொற்கள்_src_real(SRC* src)
{
      இரட்டை acc = 0.0;
      எனில் (!src) திரும்பு acc;
      க்கு (size_t i = 0; i < src->get_size(); i += 1) {
	    இரட்டை vi;
	    src->get_word(i, vi);
	    acc += vi;
      }
      திரும்பு acc;
}

தருக்க of_UPARR_SUM_REAL(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_queue_real* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_real_from_net(thr, net, qsrc, dsrc);
      இரட்டை sum;
      எனில் (qsrc) sum = uparr_sum_சொற்கள்_src_real(qsrc);
      இல்லையெனில் எனில் (dsrc) sum = uparr_sum_சொற்கள்_src_real(dsrc);
      இல்லையெனில் {
	    vvp_queue* src_q = get_queue_object<vvp_queue_real>(thr, net);
	    vvp_queue_real* src = dynamic_cast<vvp_queue_real*>(src_q);
	    sum = uparr_sum_சொற்கள்_src_real(src);
      }
      thr->தள்ளு_real(sum);
      திரும்பு உண்மை;
}

/* sum() reduction returns the scalar sum (Verilog-style addition per word). */
template <வகுப்பு SRC>
நிலையான vvp_vector4_t uparr_sum_சொற்கள்_src(SRC* src, unsigned wid)
{
      எனில் (!src) திரும்பு vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t acc(wid, BIT4_0);
      க்கு (size_t i = 0; i < src->get_size(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    acc.add(vi);
      }
      திரும்பு acc;
}

தருக்க of_UPARR_SUM_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t sum;
      எனில் (qsrc) sum = uparr_sum_சொற்கள்_src(qsrc, wid);
      இல்லையெனில் எனில் (dsrc) sum = uparr_sum_சொற்கள்_src(dsrc, wid);
      இல்லையெனில் {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    sum = uparr_sum_சொற்கள்_src(src, wid);
      }
      thr->தள்ளு_vec4(sum);
      திரும்பு உண்மை;
}

தருக்க of_UPARR_SUM_PROP_V(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->எண்;
      unsigned wid = cp->bit_idx[0];

      vvp_object_t& உச்சி = thr->peek_object();
      vvp_cobject*cobj = உச்சி.peek<vvp_cobject>();
      ப்ரதிஜ்ஞா(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t sum =
	    qsrc ? uparr_sum_சொற்கள்_src(qsrc, wid)
		 : uparr_sum_சொற்கள்_src(dsrc, wid);
      thr->தள்ளு_vec4(sum);
      திரும்பு உண்மை;
}

தருக்க of_UPARR_SUM_OBJ_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->எடு_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_vector4_t sum =
	    qsrc ? uparr_sum_சொற்கள்_src(qsrc, wid)
		 : uparr_sum_சொற்கள்_src(dsrc, wid);
      thr->தள்ளு_vec4(sum);
      திரும்பு உண்மை;
}

/* xor() reduction returns the scalar sum (Verilog-style bitwise-XOR per word). */
template <வகுப்பு SRC>
நிலையான vvp_vector4_t uparr_xor_சொற்கள்_src(SRC* src, unsigned wid)
{
      எனில் (!src) திரும்பு vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_0);
      க்கு (size_t i = 0; i < src->get_size(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    res ^= vi;
      }
      திரும்பு res;
}

தருக்க of_UPARR_XOR_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      எனில் (qsrc) res = uparr_xor_சொற்கள்_src(qsrc, wid);
      இல்லையெனில் எனில் (dsrc) res = uparr_xor_சொற்கள்_src(dsrc, wid);
      இல்லையெனில் {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    res = uparr_xor_சொற்கள்_src(src, wid);
      }
      thr->தள்ளு_vec4(res);
      திரும்பு உண்மை;
}

தருக்க of_VPI_CALL(vthread_t thr, vvp_code_t cp)
{
      vpip_execute_vpi_call(thr, cp->handle);

      எனில் (schedule_stopped()) {
	    எனில் (! schedule_finished())
		  schedule_vthread(thr, 0, பொய்);

	    திரும்பு பொய்;
      }

      திரும்பு schedule_finished()? பொய் : உண்மை;
}

/* %wait <label>;
 * Implement the wait by locating the vvp_net_T for the event, and
 * adding this thread to the threads list for the event. The some
 * argument is the  reference to the functor to wait for. This must be
 * an event object of some sort.
 */
தருக்க of_WAIT(vthread_t thr, vvp_code_t cp)
{
      ப்ரதிஜ்ஞா(! thr->i_am_in_function);
      ப்ரதிஜ்ஞா(! thr->waiting_for_event);
      thr->waiting_for_event = 1;

	/* Add this thread to the list in the event. */
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (cp->net->fun);
      ப்ரதிஜ்ஞா(ep);
      thr->wait_next = ep->add_waiting_thread(thr);

	/* Return false to suspend this thread. */
      திரும்பு பொய்;
}

/*
 * Implement the %wait/fork (SystemVerilog) instruction by suspending
 * the current thread until all the detached children have finished.
 */
தருக்க of_WAIT_FORK(vthread_t thr, vvp_code_t)
{
	/* If a %wait/fork is being executed then the parent thread
	 * cannot be waiting in a join or already waiting. */
      ப்ரதிஜ்ஞா(! thr->i_am_in_function);
      ப்ரதிஜ்ஞா(! thr->i_am_joining);
      ப்ரதிஜ்ஞா(! thr->i_am_waiting);

	/* There should be no active children when waiting. */
      ப்ரதிஜ்ஞா(thr->children.empty());

	/* If there are no detached children then there is nothing to
	 * wait for. */
      எனில் (thr->detached_children.empty()) திரும்பு உண்மை;

	/* Flag that this process is waiting for the detached children
	 * to finish and suspend it. */
      thr->i_am_waiting = 1;
      திரும்பு பொய்;
}

/*
 * %xnor
 */
தருக்க of_XNOR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valr = thr->எடு_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      ப்ரதிஜ்ஞா(vall.size() == valr.size());

      vall ^= valr;
      vall.invert();

      திரும்பு உண்மை;
}

/*
 * %xor
 */
தருக்க of_XOR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valr = thr->எடு_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      ப்ரதிஜ்ஞா(vall.size() == valr.size());

      vall ^= valr;

      திரும்பு உண்மை;
}


தருக்க of_ZOMBIE(vthread_t thr, vvp_code_t)
{
      thr->pc = codespace_null();
      எனில் ((thr->parent == 0) && (thr->children.empty())) {
	    எனில் (thr->delay_delete)
		  schedule_del_thr(thr);
	    இல்லையெனில்
		  vthread_delete(thr);
      }
      திரும்பு பொய்;
}

/*
 * This is a phantom opcode used to call user defined functions. It
 * is used in code generated by the .ufunc statement. It contains a
 * pointer to the executable code of the function and a pointer to
 * a ufunc_core object that has all the port information about the
 * function.
 */
நிலையான தருக்க do_exec_ufunc(vthread_t thr, vvp_code_t cp, vthread_t child)
{
      __vpiScope*child_scope = cp->ufunc_core_ptr->func_scope();
      ப்ரதிஜ்ஞா(child_scope);

      ப்ரதிஜ்ஞா(child_scope->get_type_code() == vpiFunction);
      ப்ரதிஜ்ஞா(thr->children.empty());


        /* We can take a number of shortcuts because we know that a
           continuous assignment can only occur in a static scope. */
      ப்ரதிஜ்ஞா(thr->wt_context == 0);
      ப்ரதிஜ்ஞா(thr->rd_context == 0);

        /* If an automatic function, allocate a context for this call. */
      vvp_context_t child_context = 0;
      எனில் (child_scope->is_automatic()) {
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
      ப்ரதிஜ்ஞா(thr->children.size()==1);

      child->is_scheduled = 1;
      child->i_am_in_function = 1;
      vthread_run(child);
      running_thread = thr;

      எனில் (child->i_have_ended) {
	    do_join(thr, child);
            திரும்பு உண்மை;
      } இல்லையெனில் {
	    thr->i_am_joining = 1;
	    திரும்பு பொய்;
      }
}

தருக்க of_EXEC_UFUNC_REAL(vthread_t thr, vvp_code_t cp)
{
      __vpiScope*child_scope = cp->ufunc_core_ptr->func_scope();
      ப்ரதிஜ்ஞா(child_scope);

	/* Create a temporary thread and run it immediately. */
      vthread_t child = vthread_new(cp->cptr, child_scope);
      thr->தள்ளு_real(0.0);
      child->args_real.push_back(0);

      திரும்பு do_exec_ufunc(thr, cp, child);
}

தருக்க of_EXEC_UFUNC_VEC4(vthread_t thr, vvp_code_t cp)
{
      __vpiScope*child_scope = cp->ufunc_core_ptr->func_scope();
      ப்ரதிஜ்ஞா(child_scope);

      vpiScopeFunction*scope_func = dynamic_cast<vpiScopeFunction*>(child_scope);
      ப்ரதிஜ்ஞா(scope_func);

	/* Create a temporary thread and run it immediately. */
      vthread_t child = vthread_new(cp->cptr, child_scope);
      thr->தள்ளு_vec4(vvp_vector4_t(scope_func->get_func_width(), scope_func->get_func_init_val()));
      child->args_vec4.push_back(0);

      திரும்பு do_exec_ufunc(thr, cp, child);
}

/*
 * This is a phantom opcode used to harvest the result of calling a user
 * defined function. It is used in code generated by the .ufunc statement.
 */
தருக்க of_REAP_UFUNC(vthread_t thr, vvp_code_t cp)
{
      __vpiScope*child_scope = cp->ufunc_core_ptr->func_scope();
      ப்ரதிஜ்ஞா(child_scope);

	/* Copy the output from the result variable to the output
	   ports of the .ufunc device. */
      cp->ufunc_core_ptr->finish_thread();

        /* If an automatic function, free the context for this call. */
      எனில் (child_scope->is_automatic()) {
            vthread_free_context(thr->rd_context, child_scope);
            thr->wt_context = 0;
            thr->rd_context = 0;
      }

      திரும்பு உண்மை;
}
