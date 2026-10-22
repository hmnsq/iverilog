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

using নামাকাশ std;

/* This is the size of an unsigned long in bits. This is just a
   convenience macro. */
# define CPU_WORD_BITS (8*পরিমাণ(unsigned long))
# define শীর্ষ_BIT (1UL << (CPU_WORD_BITS-1))

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

সংরচনা vthread_s {
      vthread_s();

      শূন্য debug_dump(ostream&fd, ধ্রুব বর্ণ*label_text);

	/* This is the program counter. */
      vvp_code_t pc;
	/* These hold the private thread bits. */
      গণনা { FLAGS_গুনতি = 512, শব্দগুলি_গুনতি = 16 };
      vvp_bit4_t flags[FLAGS_গুনতি];

	/* These are the word registers. */
      সঙ্ঘ {
	    int64_t  w_int;
	    uint64_t w_uint;
      } শব্দগুলি[শব্দগুলি_গুনতি];

	// These vectors are depths within the parent thread's
	// corresponding stack.  This is how the %ret/* instructions
	// get at parent thread arguments.
      vector<unsigned> args_real;
      vector<unsigned> args_str;
      vector<unsigned> args_vec4;

    গুপ্ত:
      vector<vvp_vector4_t>স্তূপ_vec4;
    সার্বজনিক:
      অন্তঃস্থ vvp_vector4_t বের_করো_vec4(শূন্য)
      {
	    প্রতিজ্ঞা(! স্তূপ_vec4.empty());
	    vvp_vector4_t val = স্তূপ_vec4.back();
	    স্তূপ_vec4.pop_back();
	    প্রত্যাগম val;
      }
      অন্তঃস্থ শূন্য ঠেলো_vec4(ধ্রুব vvp_vector4_t&val)
      {
	    স্তূপ_vec4.push_back(val);
      }
      অন্তঃস্থ ধ্রুব vvp_vector4_t& peek_vec4(unsigned depth)
      {
	    unsigned size = স্তূপ_vec4.size();
	    প্রতিজ্ঞা(depth < size);
	    unsigned use_index = size-1-depth;
	    প্রত্যাগম স্তূপ_vec4[use_index];
      }
      অন্তঃস্থ vvp_vector4_t& peek_vec4(শূন্য)
      {
	    unsigned use_index = স্তূপ_vec4.size();
	    প্রতিজ্ঞা(use_index >= 1);
	    প্রত্যাগম স্তূপ_vec4[use_index-1];
      }
      অন্তঃস্থ শূন্য poke_vec4(unsigned depth, ধ্রুব vvp_vector4_t&val)
      {
	    প্রতিজ্ঞা(depth < স্তূপ_vec4.size());
	    unsigned use_index = স্তূপ_vec4.size()-1-depth;
	    স্তূপ_vec4[use_index] = val;
      }
      অন্তঃস্থ শূন্য বের_করো_vec4(unsigned cnt)
      {
	    যাবত্ (cnt > 0) {
		  স্তূপ_vec4.pop_back();
		  cnt -= 1;
	    }
      }


    গুপ্ত:
      vector<দ্বিগুণ> স্তূপ_real;
    সার্বজনিক:
      অন্তঃস্থ দ্বিগুণ বের_করো_real(শূন্য)
      {
	    প্রতিজ্ঞা(! স্তূপ_real.empty());
	    দ্বিগুণ val = স্তূপ_real.back();
	    স্তূপ_real.pop_back();
	    প্রত্যাগম val;
      }
      অন্তঃস্থ শূন্য ঠেলো_real(দ্বিগুণ val)
      {
	    স্তূপ_real.push_back(val);
      }
      অন্তঃস্থ দ্বিগুণ peek_real(unsigned depth)
      {
	    প্রতিজ্ঞা(depth < স্তূপ_real.size());
	    unsigned use_index = স্তূপ_real.size()-1-depth;
	    প্রত্যাগম স্তূপ_real[use_index];
      }
      অন্তঃস্থ শূন্য poke_real(unsigned depth, দ্বিগুণ val)
      {
	    প্রতিজ্ঞা(depth < স্তূপ_real.size());
	    unsigned use_index = স্তূপ_real.size()-1-depth;
	    স্তূপ_real[use_index] = val;
      }
      অন্তঃস্থ শূন্য বের_করো_real(unsigned cnt)
      {
	    যাবত্ (cnt > 0) {
		  স্তূপ_real.pop_back();
		  cnt -= 1;
	    }
      }

	/* Strings are operated on using a forth-like operator
	   set. Items at the top of the stack (back()) are the objects
	   operated on except for special cases. New objects are
	   pushed onto the top (back()) and pulled from the top
	   (back()) only. */
    গুপ্ত:
      vector<string> স্তূপ_str;
    সার্বজনিক:
      অন্তঃস্থ string বের_করো_str(শূন্য)
      {
	    প্রতিজ্ঞা(! স্তূপ_str.empty());
	    string val = স্তূপ_str.back();
	    স্তূপ_str.pop_back();
	    প্রত্যাগম val;
      }
      অন্তঃস্থ শূন্য ঠেলো_str(ধ্রুব string&val)
      {
	    স্তূপ_str.push_back(val);
      }
      অন্তঃস্থ string&peek_str(unsigned depth)
      {
	    প্রতিজ্ঞা(depth<স্তূপ_str.size());
	    unsigned use_index = স্তূপ_str.size()-1-depth;
	    প্রত্যাগম স্তূপ_str[use_index];
      }
      অন্তঃস্থ শূন্য poke_str(unsigned depth, ধ্রুব string&val)
      {
	    প্রতিজ্ঞা(depth < স্তূপ_str.size());
	    unsigned use_index = স্তূপ_str.size()-1-depth;
	    স্তূপ_str[use_index] = val;
      }
      অন্তঃস্থ শূন্য বের_করো_str(unsigned cnt)
      {
	    যাবত্ (cnt > 0) {
		  স্তূপ_str.pop_back();
		  cnt -= 1;
	    }
      }

	/* Objects are also operated on in a stack. */
    গুপ্ত:
      গণনা { স্তূপ_OBJ_MAX_SIZE = 32 };
      vvp_object_t স্তূপ_obj[স্তূপ_OBJ_MAX_SIZE];
      unsigned স্তূপ_obj_size;
    সার্বজনিক:
      অন্তঃস্থ vvp_object_t& peek_object(শূন্য)
      {
	    প্রতিজ্ঞা(স্তূপ_obj_size > 0);
	    প্রত্যাগম স্তূপ_obj[স্তূপ_obj_size-1];
      }
      অন্তঃস্থ শূন্য বের_করো_object(vvp_object_t&obj)
      {
	    প্রতিজ্ঞা(স্তূপ_obj_size > 0);
	    স্তূপ_obj_size -= 1;
	    obj = স্তূপ_obj[স্তূপ_obj_size];
	    স্তূপ_obj[স্তূপ_obj_size].reset(0);
      }
      অন্তঃস্থ শূন্য বের_করো_object(unsigned cnt, unsigned skip =0)
      {
	    প্রতিজ্ঞা((cnt+skip) <= স্তূপ_obj_size);
	    প্রতি (size_t idx = স্তূপ_obj_size-skip-cnt ; idx < স্তূপ_obj_size-skip ; idx += 1)
		  স্তূপ_obj[idx].reset(0);
	    স্তূপ_obj_size -= cnt;
	    প্রতি (size_t idx = স্তূপ_obj_size-skip ; idx < স্তূপ_obj_size ; idx += 1)
		  স্তূপ_obj[idx] = স্তূপ_obj[idx+skip];
	    প্রতি (size_t idx = স্তূপ_obj_size ; idx < স্তূপ_obj_size+skip ; idx += 1)
		  স্তূপ_obj[idx].reset(0);
      }
      অন্তঃস্থ শূন্য ঠেলো_object(ধ্রুব vvp_object_t&obj)
      {
	    প্রতিজ্ঞা(স্তূপ_obj_size < স্তূপ_OBJ_MAX_SIZE);
	    স্তূপ_obj[স্তূপ_obj_size] = obj;
	    স্তূপ_obj_size += 1;
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
      set<সংরচনা vthread_s*>children;
	/* This points to the detached children of the thread. */
      set<সংরচনা vthread_s*>detached_children;
	/* This points to my parent, if I have one. */
      সংরচনা vthread_s*parent;
	/* This points to the containing scope. */
      __vpiScope*parent_scope;
	/* This is used for keeping wait queues. */
      সংরচনা vthread_s*wait_next;
	/* These are used to access automatically allocated items. */
      vvp_context_t wt_context, rd_context;
	/* These are used to pass non-blocking event control information. */
      vvp_net_t*event;
      uint64_t ecount;
	/* Save the file/line information when available. */
    গুপ্ত:
      বর্ণ *filenm_;
      unsigned lineno_;
    সার্বজনিক:
      শূন্য set_fileline(বর্ণ *filenm, unsigned lineno);
      string get_fileline();

      অন্তঃস্থ শূন্য cleanup()
      {
	    যদি (i_was_disabled) {
		  স্তূপ_vec4.clear();
		  স্তূপ_real.clear();
		  স্তূপ_str.clear();
		  বের_করো_object(স্তূপ_obj_size);
	    }
	    মুক্ত(filenm_);
	    filenm_ = 0;
	    প্রতিজ্ঞা(স্তূপ_vec4.empty());
	    প্রতিজ্ঞা(স্তূপ_real.empty());
	    প্রতিজ্ঞা(স্তূপ_str.empty());
	    প্রতিজ্ঞা(স্তূপ_obj_size == 0);
      }
};

অন্তঃস্থ vthread_s::vthread_s()
{
      স্তূপ_obj_size = 0;
      filenm_ = 0;
      lineno_ = 0;
}

শূন্য vthread_s::set_fileline(বর্ণ *filenm, unsigned lineno)
{
      প্রতিজ্ঞা(filenm);
      যদি (!filenm_ || (strcmp(filenm_, filenm) != 0)) {
	    মুক্ত(filenm_);
	    filenm_ = strdup(filenm);
      }
      lineno_ = lineno;
}

অন্তঃস্থ string vthread_s::get_fileline()
{
      ostringstream buf;
      যদি (filenm_) {
	    buf << filenm_ << ":" << lineno_ << ": ";
      }
      string res = buf.str();
      প্রত্যাগম res;
}

শূন্য vthread_s::debug_dump(ostream&fd, ধ্রুব বর্ণ*label)
{
      fd << "**** " << label << endl;
      fd << "**** ThreadId: " << এতত্ << ", parent id: " << parent << endl;

      fd << "**** Flags: ";
      প্রতি (পূর্ণাঙ্ক idx = 0 ; idx < FLAGS_গুনতি ; idx += 1)
	    fd << flags[idx];
      fd << endl;
      fd << "**** vec4 stack..." << endl;
      প্রতি (size_t idx = স্তূপ_vec4.size() ; idx > 0 ; idx -= 1)
	    fd << "    " << (স্তূপ_vec4.size()-idx) << ": " << স্তূপ_vec4[idx-1] << endl;
      fd << "**** str stack (" << স্তূপ_str.size() << ")..." << endl;
      fd << "**** obj stack (" << স্তূপ_obj_size << ")..." << endl;
      fd << "**** args_vec4 array (" << args_vec4.size() << ")..." << endl;
      প্রতি (size_t idx = 0 ; idx < args_vec4.size() ; idx += 1)
	    fd << "    " << idx << ": " << args_vec4[idx] << endl;
      fd << "**** file/line (";
      যদি (filenm_) fd << filenm_;
      অন্যথা fd << "<no file name>";
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
স্থির string filter_string(ধ্রুব বর্ণ*text)
{
      vector<বর্ণ> tmp (strlen(text)+1);
      size_t dst = 0;
      প্রতি (ধ্রুব বর্ণ*ptr = text ; *ptr ; ptr += 1) {
	    // Not an escape? Move on.
	    যদি (*ptr != '\\') {
		  tmp[dst++] = *ptr;
		  অনুবর্ত;
	    }

	    // Now we know that *ptr is pointing to a \ character and we
	    // have an octal sequence coming up. Advance the ptr and start
	    // processing octal digits.
	    ptr += 1;
	    যদি (*ptr == 0)
		  ভঙ্গ;

	    বর্ণ byte = 0;
	    পূর্ণাঙ্ক cnt = 3;
	    যাবত্ (*ptr && cnt > 0 && *ptr >= '0' && *ptr <= '7') {
		  byte *= 8;
		  byte += *ptr - '0';
		  cnt -= 1;
		  ptr += 1;
	    }

	    // null-bytes are supposed to be removed when assigning a string
	    // literal to a string.
	    যদি (byte != '\0')
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
      প্রত্যাগম res;
}

স্থির শূন্য do_join(vthread_t thr, vthread_t child);

__vpiScope* vthread_scope(সংরচনা vthread_s*thr)
{
      প্রত্যাগম thr->parent_scope;
}

সংরচনা vthread_s*running_thread = 0;

string get_fileline()
{
      প্রত্যাগম running_thread->get_fileline();
}

শূন্য vthread_ঠেলো(সংরচনা vthread_s*thr, দ্বিগুণ val)
{
      thr->ঠেলো_real(val);
}

শূন্য vthread_ঠেলো(সংরচনা vthread_s*thr, ধ্রুব string&val)
{
      thr->ঠেলো_str(val);
}

শূন্য vthread_ঠেলো(সংরচনা vthread_s*thr, ধ্রুব vvp_vector4_t&val)
{
      thr->ঠেলো_vec4(val);
}

শূন্য vthread_বের_করো_real(সংরচনা vthread_s*thr, unsigned depth)
{
      thr->বের_করো_real(depth);
}

শূন্য vthread_বের_করো_str(সংরচনা vthread_s*thr, unsigned depth)
{
      thr->বের_করো_str(depth);
}

শূন্য vthread_বের_করো_vec4(সংরচনা vthread_s*thr, unsigned depth)
{
      thr->বের_করো_vec4(depth);
}

দ্বিগুণ vthread_get_real_স্তূপ(সংরচনা vthread_s*thr, unsigned depth)
{
      প্রত্যাগম thr->peek_real(depth);
}

ধ্রুব string&vthread_get_str_স্তূপ(সংরচনা vthread_s*thr, unsigned depth)
{
      প্রত্যাগম thr->peek_str(depth);
}

ধ্রুব vvp_vector4_t& vthread_get_vec4_স্তূপ(সংরচনা vthread_s*thr, unsigned depth)
{
      প্রত্যাগম thr->peek_vec4(depth);
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
template <বর্গ VVP_QUEUE> স্থির vvp_queue*get_queue_object(vthread_t thr, vvp_net_t*net)
{
      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      প্রতিজ্ঞা(obj);

      vvp_queue*queue = obj->get_object().peek<vvp_queue>();
      যদি (queue == 0) {
	    প্রতিজ্ঞা(obj->get_object().test_nil());
	    queue = নূতন VVP_QUEUE;
	    vvp_object_t val (queue);
	    vvp_net_ptr_t ptr (net, 0);
	    vvp_send_object(ptr, val, thr->wt_context);
      }

      প্রত্যাগম queue;
}

/*
 * Locator and size/word opcodes use the same VVP net for a queue ($) or a
 * dynamic array ([]). Queues are vvp_queue_vec4; dynamic arrays may be
 * vvp_darray_vec4, vvp_darray_atom<>, etc. (all vvp_darray, but vvp_queue
 * also derives from vvp_darray — test queue vec4 first).
 */
স্থির শূন্য get_queue_or_darray_vec4_from_net(vthread_t thr, vvp_net_t* net,
					      vvp_queue_vec4*& qsrc,
					      vvp_darray*& dsrc)
{
      vvp_fun_signal_object* obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      প্রতিজ্ঞা(obj);
      vvp_object_t শীর্ষ = obj->get_object();
      qsrc = শীর্ষ.peek<vvp_queue_vec4>();
      যদি (qsrc) {
	    dsrc = 0;
	    প্রত্যাগম;
      }
      vvp_darray* dany = শীর্ষ.peek<vvp_darray>();
      যদি (dany && dynamic_cast<vvp_queue*>(dany) == 0) {
	    dsrc = dany;
	    প্রত্যাগম;
      }
      যদি (শীর্ষ.test_nil()) {
	    qsrc = নূতন vvp_queue_vec4();
	    vvp_object_t val(qsrc);
	    vvp_net_ptr_t ptr(net, 0);
	    vvp_send_object(ptr, val, thr->wt_context);
	    dsrc = 0;
	    প্রত্যাগম;
      }
      /* e.g. real/string queue — not vec4 queue or plain darray */
      qsrc = 0;
      dsrc = 0;
}

স্থির শূন্য get_queue_or_darray_real_from_net(vthread_t thr, vvp_net_t* net,
					      vvp_queue_real*& qsrc,
					      vvp_darray*& dsrc)
{
      vvp_fun_signal_object* obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      প্রতিজ্ঞা(obj);
      vvp_object_t শীর্ষ = obj->get_object();
      qsrc = শীর্ষ.peek<vvp_queue_real>();
      যদি (qsrc) {
	    dsrc = 0;
	    প্রত্যাগম;
      }
      vvp_darray* dany = শীর্ষ.peek<vvp_darray>();
      যদি (dany && dynamic_cast<vvp_queue*>(dany) == 0) {
	    dsrc = dany;
	    প্রত্যাগম;
      }
      যদি (শীর্ষ.test_nil()) {
	    qsrc = নূতন vvp_queue_real();
	    vvp_object_t val(qsrc);
	    vvp_net_ptr_t ptr(net, 0);
	    vvp_send_object(ptr, val, thr->wt_context);
	    dsrc = 0;
	    প্রত্যাগম;
      }
      /* e.g. real/string queue — not vec4 queue or plain darray */
      qsrc = 0;
      dsrc = 0;
}

স্থির শূন্য get_queue_or_darray_vec4_from_object(vvp_object_t src_obj,
						 vvp_queue_vec4*& qsrc,
						 vvp_darray*& dsrc)
{
      qsrc = src_obj.peek<vvp_queue_vec4>();
      যদি (qsrc) {
	    dsrc = 0;
	    প্রত্যাগম;
      }
      vvp_darray* dany = src_obj.peek<vvp_darray>();
      যদি (dany && dynamic_cast<vvp_queue*>(dany) == 0) {
	    dsrc = dany;
	    প্রত্যাগম;
      }
      qsrc = 0;
      dsrc = 0;
}

/*
 * The following are used to allow a common template to be written for
 * queue real/string/vec4 operations
 */
অন্তঃস্থ স্থির শূন্য বের_করো_মান(vthread_t thr, দ্বিগুণ&value, unsigned)
{
      value = thr->বের_করো_real();
}

অন্তঃস্থ স্থির শূন্য বের_করো_মান(vthread_t thr, string&value, unsigned)
{
      value = thr->বের_করো_str();
}

অন্তঃস্থ স্থির শূন্য বের_করো_মান(vthread_t thr, vvp_vector4_t&value, unsigned wid)
{
      value = thr->বের_করো_vec4();
      প্রতিজ্ঞা(value.size() == wid);
}

/*
 * The following are used to allow the queue templates to print correctly.
 */
অন্তঃস্থ স্থির string get_queue_type(দ্বিগুণ&)
{
      প্রত্যাগম "queue<real>";
}

অন্তঃস্থ স্থির string get_queue_type(string&)
{
      প্রত্যাগম "queue<string>";
}

অন্তঃস্থ স্থির string get_queue_type(ধ্রুব vvp_vector4_t&value)
{
      ostringstream buf;
      buf << "queue<vector[" << value.size() << "]>";
      string res = buf.str();
      প্রত্যাগম res;
}

অন্তঃস্থ স্থির শূন্য print_queue_মান(দ্বিগুণ value)
{
      cerr << value;
}

অন্তঃস্থ স্থির শূন্য print_queue_মান(ধ্রুব string&value)
{
      cerr << "\"" << value << "\"";
}

অন্তঃস্থ স্থির শূন্য print_queue_মান(ধ্রুব vvp_vector4_t&value)
{
      cerr << value;
}

/*
 * The following are used to get a darray/queue default value.
 */
অন্তঃস্থ স্থির শূন্য dq_default(দ্বিগুণ&value, unsigned)
{
      value = 0.0;
}

অন্তঃস্থ স্থির শূন্য dq_default(string&value, unsigned)
{
      value = "";
}

অন্তঃস্থ স্থির শূন্য dq_default(vvp_vector4_t&value, unsigned wid)
{
      value = vvp_vector4_t(wid);
}


template <বর্গ T> T coerce_to_width(ধ্রুব T&that, unsigned width)
{
      যদি (that.size() == width)
	    প্রত্যাগম that;

      প্রতিজ্ঞা(that.size() > width);
      T res (width);
      প্রতি (unsigned idx = 0 ;  idx < width ;  idx += 1)
	    res.set_bit(idx, that.value(idx));

      প্রত্যাগম res;
}

/* Explicitly define the vvp_vector4_t version of coerce_to_width(). */
template vvp_vector4_t coerce_to_width(ধ্রুব vvp_vector4_t&that,
                                       unsigned width);


স্থির শূন্য multiply_array_imm(unsigned long*res, ধ্রুব unsigned long*val,
			       unsigned শব্দগুলি, unsigned long imm)
{
      প্রতি (unsigned idx = 0 ; idx < শব্দগুলি ; idx += 1)
	    res[idx] = 0;

      প্রতি (unsigned mul_idx = 0 ; mul_idx < শব্দগুলি ; mul_idx += 1) {
	    unsigned long sum;
	    unsigned long tmp = multiply_with_carry(val[mul_idx], imm, sum);

	    unsigned long carry = 0;
	    res[mul_idx] = add_with_carry(res[mul_idx], tmp, carry);
	    প্রতি (unsigned add_idx = mul_idx+1 ; add_idx < শব্দগুলি ; add_idx += 1) {
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
স্থির vvp_context_t vthread_alloc_context(__vpiScope*scope)
{
      প্রতিজ্ঞা(scope->is_automatic());

      vvp_context_t context = scope->free_contexts;
      যদি (context) {
            scope->free_contexts = vvp_get_next_context(context);
            প্রতি (unsigned idx = 0 ; idx < scope->nitem ; idx += 1) {
                  scope->item[idx]->reset_instance(context);
            }
      } অন্যথা {
            context = vvp_allocate_context(scope->nitem);
            প্রতি (unsigned idx = 0 ; idx < scope->nitem ; idx += 1) {
                  scope->item[idx]->alloc_instance(context);
            }
      }

      vvp_set_next_context(context, scope->live_contexts);
      scope->live_contexts = context;

      প্রত্যাগম context;
}

/*
 * Free a context previously allocated to a child thread by pushing it
 * onto the freed context stack. Remove it from the list of live contexts
 * in that scope.
 */
স্থির শূন্য vthread_free_context(vvp_context_t context, __vpiScope*scope)
{
      প্রতিজ্ঞা(scope->is_automatic());
      প্রতিজ্ঞা(context);

      যদি (context == scope->live_contexts) {
            scope->live_contexts = vvp_get_next_context(context);
      } অন্যথা {
            vvp_context_t tmp = scope->live_contexts;
            যাবত্ (context != vvp_get_next_context(tmp)) {
                  প্রতিজ্ঞা(tmp);
                  tmp = vvp_get_next_context(tmp);
            }
            vvp_set_next_context(tmp, vvp_get_next_context(context));
      }

      vvp_set_next_context(context, scope->free_contexts);
      scope->free_contexts = context;
}

#ifdef CHECK_WITH_VALGRIND
শূন্য contexts_delete(বর্গ __vpiScope*scope)
{
      vvp_context_t context = scope->free_contexts;

      যাবত্ (context) {
	    scope->free_contexts = vvp_get_next_context(context);
	    প্রতি (unsigned idx = 0; idx < scope->nitem; idx += 1) {
		  scope->item[idx]->free_instance(context);
	    }
	    মুক্ত(context);
	    context = scope->free_contexts;
      }
      মুক্ত(scope->item);
}
#endif

/*
 * Create a new thread with the given start address.
 */
vthread_t vthread_new(vvp_code_t pc, __vpiScope*scope)
{
      vthread_t thr = নূতন সংরচনা vthread_s;
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
      প্রতি (পূর্ণাঙ্ক idx = 4 ; idx < 8 ; idx += 1)
	    thr->flags[idx] = BIT4_X;

      scope->threads .insert(thr);
      প্রত্যাগম thr;
}

#ifdef CHECK_WITH_VALGRIND
#যদি 0
/*
 * These are not currently correct. If you use them you will get
 * double delete messages. There is still a leak related to a
 * waiting event that needs to be investigated.
 */

স্থির শূন্য wait_next_delete(vthread_t base)
{
      যাবত্ (base) {
	    vthread_t tmp = base->wait_next;
	    delete base;
	    base = tmp;
	    যদি (base->waiting_for_event == 0) ভঙ্গ;
      }
}

স্থির শূন্য child_delete(vthread_t base)
{
      যাবত্ (base) {
	    vthread_t tmp = base->child;
	    delete base;
	    base = tmp;
      }
}
#endif

শূন্য vthreads_delete(বর্গ __vpiScope*scope)
{
      প্রতি (std::set<vthread_t>::iterator cur = scope->threads.begin()
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
স্থির শূন্য vthread_reap(vthread_t thr)
{
      যদি (! thr->children.empty()) {
	    প্রতি (set<vthread_t>::iterator cur = thr->children.begin()
		       ; cur != thr->children.end() ; ++cur) {
		  vthread_t child = *cur;
		  প্রতিজ্ঞা(child);
		  প্রতিজ্ঞা(child->parent == thr);
		  child->parent = thr->parent;
	    }
      }
      যদি (! thr->detached_children.empty()) {
	    প্রতি (set<vthread_t>::iterator cur = thr->detached_children.begin()
		       ; cur != thr->detached_children.end() ; ++cur) {
		  vthread_t child = *cur;
		  প্রতিজ্ঞা(child);
		  প্রতিজ্ঞা(child->parent == thr);
		  প্রতিজ্ঞা(child->i_am_detached);
		  child->parent = 0;
		  child->i_am_detached = 0;
	    }
      }
      যদি (thr->parent) {
	      /* assert that the given element was removed. */
	    যদি (thr->i_am_detached) {
		  size_t res = thr->parent->detached_children.erase(thr);
		  প্রতিজ্ঞা(res == 1);
	    } অন্যথা {
		  size_t res = thr->parent->children.erase(thr);
		  প্রতিজ্ঞা(res == 1);
	    }
      }

      thr->parent = 0;

	// Remove myself from the containing scope if needed.
      thr->parent_scope->threads.erase(thr);

      thr->pc = codespace_null();

	/* If this thread is not scheduled, then is it safe to delete
	   it now. Otherwise, let the schedule event (which will
	   execute the thread at of_ZOMBIE) delete the object. */
      যদি ((thr->is_scheduled == 0) && (thr->waiting_for_event == 0)) {
	    প্রতিজ্ঞা(thr->children.empty());
	    প্রতিজ্ঞা(thr->wait_next == 0);
	    যদি (thr->delay_delete)
		  schedule_del_thr(thr);
	    অন্যথা
		  vthread_delete(thr);
      }
}

শূন্য vthread_delete(vthread_t thr)
{
      thr->cleanup();
      delete thr;
}

শূন্য vthread_mark_scheduled(vthread_t thr)
{
      যাবত্ (thr != 0) {
	    প্রতিজ্ঞা(thr->is_scheduled == 0);
	    thr->is_scheduled = 1;
	    thr = thr->wait_next;
      }
}

শূন্য vthread_mark_final(vthread_t thr)
{
      /*
       * The behavior in a final thread is the same as in a function. Any
       * child thread will be executed immediately rather than being
       * scheduled.
       */
      যাবত্ (thr != 0) {
	    thr->i_am_in_function = 1;
	    thr = thr->wait_next;
      }
}

শূন্য vthread_delay_delete()
{
      যদি (running_thread)
	    running_thread->delay_delete = 1;
}

/*
 * This function runs each thread by fetching an instruction,
 * incrementing the PC, and executing the instruction. The thread may
 * be the head of a list, so each thread is run so far as possible.
 */
শূন্য vthread_run(vthread_t thr)
{
      যাবত্ (thr != 0) {
	    vthread_t tmp = thr->wait_next;
	    thr->wait_next = 0;

	    প্রতিজ্ঞা(thr->is_scheduled);
	    thr->is_scheduled = 0;

            running_thread = thr;

	    প্রতি (;;) {
		  vvp_code_t cp = thr->pc;
		  thr->pc += 1;

		    /* Run the opcode implementation. If the execution of
		       the opcode returns false, then the thread is meant to
		       be paused, so break out of the loop. */
		  তার্কিক rc = (cp->opcode)(thr, cp);
		  যদি (rc == অসত্য)
			ভঙ্গ;
	    }

	    thr = tmp;
      }
      running_thread = 0;
}

/*
 * The CHUNK_LINK instruction is a special next pointer for linking
 * chunks of code space. It's like a simplified %jmp.
 */
তার্কিক of_CHUNK_LINK(vthread_t thr, vvp_code_t code)
{
      প্রতিজ্ঞা(code->cptr);
      thr->pc = code->cptr;
      প্রত্যাগম সত্য;
}

/*
 * This is called by an event functor to wake up all the threads on
 * its list. I in fact created that list in the %wait instruction, and
 * I also am certain that the waiting_for_event flag is set.
 */
শূন্য vthread_schedule_list(vthread_t thr)
{
      প্রতি (vthread_t cur = thr ;  cur ;  cur = cur->wait_next) {
	    প্রতিজ্ঞা(cur->waiting_for_event);
	    cur->waiting_for_event = 0;
      }

      schedule_vthread(thr, 0);
}

vvp_context_t vthread_get_wt_context()
{
      যদি (running_thread)
            প্রত্যাগম running_thread->wt_context;
      অন্যথা
            প্রত্যাগম 0;
}

vvp_context_t vthread_get_rd_context()
{
      যদি (running_thread)
            প্রত্যাগম running_thread->rd_context;
      অন্যথা
            প্রত্যাগম 0;
}

vvp_context_item_t vthread_get_wt_context_item(unsigned context_idx)
{
      প্রতিজ্ঞা(running_thread && running_thread->wt_context);
      প্রত্যাগম vvp_get_context_item(running_thread->wt_context, context_idx);
}

vvp_context_item_t vthread_get_rd_context_item(unsigned context_idx)
{
      প্রতিজ্ঞা(running_thread && running_thread->rd_context);
      প্রত্যাগম vvp_get_context_item(running_thread->rd_context, context_idx);
}

/*
 * %abs/wr
 */
তার্কিক of_ABS_WR(vthread_t thr, vvp_code_t)
{
      thr->ঠেলো_real( fabs(thr->বের_করো_real()) );
      প্রত্যাগম সত্য;
}

তার্কিক of_ALLOC(vthread_t thr, vvp_code_t cp)
{
        /* Allocate a context. */
      vvp_context_t child_context = vthread_alloc_context(cp->scope);

        /* Push the allocated context onto the write context stack. */
      vvp_set_stacked_context(child_context, thr->wt_context);
      thr->wt_context = child_context;

      প্রত্যাগম সত্য;
}

তার্কিক of_AND(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valb = thr->বের_করো_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();
      প্রতিজ্ঞা(vala.size() == valb.size());
      vala &= valb;
      প্রত্যাগম সত্য;
}

/*
 * This function must ALWAYS be called with the val set to the right
 * size, and initialized with BIT4_0 bits. Certain optimizations rely
 * on that.
 */
স্থির শূন্য get_immediate_rval(vvp_code_t cp, vvp_vector4_t&val)
{
      uint32_t vala = cp->bit_idx[0];
      uint32_t valb = cp->bit_idx[1];
      unsigned wid  = cp->সংখ্যা;

      যদি (valb == 0) {
	      // Special case: if the value is zero, we are done
	      // before we start.
	    যদি (vala == 0) প্রত্যাগম;

	      // Special case: The value has no X/Z bits, so we can
	      // use the setarray method to write the value all at once.
	    unsigned use_wid = 8*পরিমাণ(unsigned long);
	    যদি (wid < use_wid)
		  use_wid = wid;
	    unsigned long tmp[1];
	    tmp[0] = vala;
	    val.setarray(0, use_wid, tmp);
	    প্রত্যাগম;
      }

	// The immediate value can be values bigger then 32 bits, but
	// only if the high bits are zero. So at most we need to run
	// through the loop below 32 times. Maybe less, if the target
	// width is less. We don't have to do anything special on that
	// because vala/valb bits will shift away so (vala|valb) will
	// turn to zero at or before 32 shifts.

      প্রতি (unsigned idx = 0 ; idx < wid && (vala|valb) ; idx += 1) {
	    uint32_t ba = 0;
	      // Convert the vala/valb bits to a ba number that
	      // matches the encoding of the vvp_bit4_t enumeration.
	    ba = (valb & 1) << 1;
	    ba |= vala & 1;

	      // Note that the val is already pre-filled with BIT4_0
	      // bits, os we only need to set non-zero bit values.
	    যদি (ba) val.set_bit(idx, (vvp_bit4_t)ba);

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
তার্কিক of_ADD(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t r = thr->বের_করো_vec4();
	// Rather then pop l, use it directly from the stack. When we
	// assign to 'l', that will edit the top of the stack, which
	// replaces a pop and a pull.
      vvp_vector4_t&l = thr->peek_vec4();

      l.add(r);

      প্রত্যাগম সত্য;
}

/*
 * %addi <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
তার্কিক of_ADDI(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->সংখ্যা;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_immediate_rval (cp, r);

      l.add(r);

      প্রত্যাগম সত্য;
}

/*
 * %add/wr
 */
তার্কিক of_ADD_WR(vthread_t thr, vvp_code_t)
{
      দ্বিগুণ r = thr->বের_করো_real();
      দ্বিগুণ l = thr->বের_করো_real();
      thr->ঠেলো_real(l + r);
      প্রত্যাগম সত্য;
}

/* %assign/ar <array>, <delay>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory. <delay>
 * is the delay in simulation time. <bit> is the index register
 * containing the real value.
 */
তার্কিক of_ASSIGN_AR(vthread_t thr, vvp_code_t cp)
{
      long adr = thr->শব্দগুলি[3].w_int;
      unsigned delay = cp->bit_idx[0];
      দ্বিগুণ value = thr->বের_করো_real();

      যদি (adr >= 0) {
	    schedule_assign_array_word(cp->array, adr, value, delay);
      }

      প্রত্যাগম সত্য;
}

/* %assign/ar/d <array>, <delay_idx>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory.
 * <delay_idx> is the integer register that contains the delay value.
 */
তার্কিক of_ASSIGN_ARD(vthread_t thr, vvp_code_t cp)
{
      long adr = thr->শব্দগুলি[3].w_int;
      দ্বিগুণ value = thr->বের_করো_real();

      যদি (adr >= 0) {
	    vvp_time64_t delay = thr->শব্দগুলি[cp->bit_idx[0]].w_uint;
	    schedule_assign_array_word(cp->array, adr, value, delay);
      }

      প্রত্যাগম সত্য;
}

/* %assign/ar/e <array>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory. <bit>
 * is the index register containing the real value. The event
 * information is contained in the thread event control registers
 * and is set with %evctl.
 */
তার্কিক of_ASSIGN_ARE(vthread_t thr, vvp_code_t cp)
{
      long adr = thr->শব্দগুলি[3].w_int;
      দ্বিগুণ value = thr->বের_করো_real();

      যদি (adr >= 0) {
	    যদি (thr->ecount == 0) {
		  schedule_assign_array_word(cp->array, adr, value, 0);
	    } অন্যথা {
		  schedule_evctl(cp->array, adr, value, thr->event,
		                 thr->ecount);
	    }
      }

      প্রত্যাগম সত্য;
}

/*
 * %assign/vec4 <var>, <delay>
 */
তার্কিক of_ASSIGN_VEC4(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      unsigned delay = cp->bit_idx[0];
      ধ্রুব vvp_vector4_t&val = thr->peek_vec4();

      schedule_assign_vector(ptr, 0, 0, val, delay);
      thr->বের_করো_vec4(1);
      প্রত্যাগম সত্য;
}

/*
 * Resizes a vector value for a partial assignment so that the value is fully
 * in-bounds of the target signal. Both `val` and `off` will be updated if
 * necessary.
 *
 * Returns false if the value is fully out-of-bounds and the assignment should
 * be skipped. Otherwise returns true.
 */
স্থির তার্কিক resize_rval_vec(vvp_vector4_t &val, int64_t &off,
			    unsigned পূর্ণাঙ্ক sig_wid)
{
      unsigned পূর্ণাঙ্ক wid = val.size();

        // Fully in bounds, most likely case
      যদি (off >= 0 && (uint64_t)off + wid <= sig_wid)
	    প্রত্যাগম সত্য;

      unsigned পূর্ণাঙ্ক base = 0;
      যদি (off >= 0) {
	      // Fully out-of-bounds
	    যদি ((uint64_t)off >= sig_wid)
		  প্রত্যাগম অসত্য;
      } অন্যথা {
	      // Fully out-of-bounds */
	    যদি ((uint64_t)(-off) >= wid)
		  প্রত্যাগম অসত্য;

	      // If the index is below the vector, then only assign the high
	      // bits that overlap with the target
	    base = -off;
	    wid += off;
		off = 0;
      }

	// If the value is partly above the target, then only assign
	// the bits that overlap
      যদি ((uint64_t)off + wid > sig_wid)
	    wid = sig_wid - (uint64_t)off;

      val = val.subvalue(base, wid);

      প্রত্যাগম সত্য;
}

/*
 * %assign/vec4/a/d <arr>, <offx>, <delx>
 */
তার্কিক of_ASSIGN_VEC4_A_D(vthread_t thr, vvp_code_t cp)
{
      পূর্ণাঙ্ক off_idx = cp->bit_idx[0];
      পূর্ণাঙ্ক del_idx = cp->bit_idx[1];
      পূর্ণাঙ্ক adr_idx = 3;

      int64_t  off = off_idx ? thr->শব্দগুলি[off_idx].w_int : 0;
      vvp_time64_t del = del_idx? thr->শব্দগুলি[del_idx].w_uint : 0;
      long     adr = thr->শব্দগুলি[adr_idx].w_int;

      vvp_vector4_t val = thr->বের_করো_vec4();

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      যদি (thr->flags[4] == BIT4_1)
	    প্রত্যাগম সত্য;

      যদি (!resize_rval_vec(val, off, cp->array->get_word_size()))
	    প্রত্যাগম সত্য;

      schedule_assign_array_word(cp->array, adr, off, val, del);

      প্রত্যাগম সত্য;
}

/*
 * %assign/vec4/a/e <arr>, <offx>
 */
তার্কিক of_ASSIGN_VEC4_A_E(vthread_t thr, vvp_code_t cp)
{
      পূর্ণাঙ্ক off_idx = cp->bit_idx[0];
      পূর্ণাঙ্ক adr_idx = 3;

      int64_t  off = off_idx ? thr->শব্দগুলি[off_idx].w_int : 0;
      long     adr = thr->শব্দগুলি[adr_idx].w_int;

      vvp_vector4_t val = thr->বের_করো_vec4();

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      যদি (thr->flags[4] == BIT4_1)
	    প্রত্যাগম সত্য;

      যদি (!resize_rval_vec(val, off, cp->array->get_word_size()))
	    প্রত্যাগম সত্য;

      যদি (thr->ecount == 0) {
	    schedule_assign_array_word(cp->array, adr, off, val, 0);
      } অন্যথা {
	    schedule_evctl(cp->array, adr, val, off, thr->event, thr->ecount);
      }

      প্রত্যাগম সত্য;
}

/*
 * %assign/vec4/off/d <var>, <off>, <del>
 */
তার্কিক of_ASSIGN_VEC4_OFF_D(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      unsigned off_index = cp->bit_idx[0];
      unsigned del_index = cp->bit_idx[1];
      vvp_vector4_t val = thr->বের_করো_vec4();

      int64_t off = thr->শব্দগুলি[off_index].w_int;
      vvp_time64_t del = thr->শব্দগুলি[del_index].w_uint;

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      যদি (thr->flags[4] == BIT4_1)
	    প্রত্যাগম সত্য;

      vvp_signal_মান*sig = dynamic_cast<vvp_signal_মান*> (cp->net->fil);
      প্রতিজ্ঞা(sig);

      যদি (!resize_rval_vec(val, off, sig->মান_size()))
	    প্রত্যাগম সত্য;

      schedule_assign_vector(ptr, off, sig->মান_size(), val, del);
      প্রত্যাগম সত্য;
}

/*
 * %assign/vec4/off/e <var>, <off>
 */
তার্কিক of_ASSIGN_VEC4_OFF_E(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      unsigned off_index = cp->bit_idx[0];
      vvp_vector4_t val = thr->বের_করো_vec4();

      int64_t off = thr->শব্দগুলি[off_index].w_int;

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      যদি (thr->flags[4] == BIT4_1)
	    প্রত্যাগম সত্য;

      vvp_signal_মান*sig = dynamic_cast<vvp_signal_মান*> (cp->net->fil);
      প্রতিজ্ঞা(sig);

      যদি (!resize_rval_vec(val, off, sig->মান_size()))
	    প্রত্যাগম সত্য;

      যদি (thr->ecount == 0) {
	    schedule_assign_vector(ptr, off, sig->মান_size(), val, 0);
      } অন্যথা {
	    schedule_evctl(ptr, val, off, sig->মান_size(), thr->event, thr->ecount);
      }

      প্রত্যাগম সত্য;
}

/*
 * %assign/vec4/d <var-label> <delay>
 */
তার্কিক of_ASSIGN_VEC4D(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      unsigned del_index = cp->bit_idx[0];
      vvp_time64_t del = thr->শব্দগুলি[del_index].w_int;

      vvp_vector4_t value = thr->বের_করো_vec4();

      vvp_signal_মান*sig = dynamic_cast<vvp_signal_মান*> (cp->net->fil);
      প্রতিজ্ঞা(sig);

      schedule_assign_vector(ptr, 0, sig->মান_size(), value, del);

      প্রত্যাগম সত্য;
}

/*
 * %assign/vec4/e <var-label>
 */
তার্কিক of_ASSIGN_VEC4E(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_vector4_t value = thr->বের_করো_vec4();

      vvp_signal_মান*sig = dynamic_cast<vvp_signal_মান*> (cp->net->fil);
      প্রতিজ্ঞা(sig);

      যদি (thr->ecount == 0) {
	    schedule_assign_vector(ptr, 0, sig->মান_size(), value, 0);
      } অন্যথা {
	    schedule_evctl(ptr, value, 0, sig->মান_size(), thr->event, thr->ecount);
      }

      প্রত্যাগম সত্য;
}

/*
 * This is %assign/wr <vpi-label>, <delay>
 *
 * This assigns (after a delay) a value to a real variable. Use the
 * vpi_put_value function to do the assign, with the delay written
 * into the vpiInertialDelay carrying the desired delay.
 */
তার্কিক of_ASSIGN_WR(vthread_t thr, vvp_code_t cp)
{
      unsigned delay = cp->bit_idx[0];
      দ্বিগুণ value = thr->বের_করো_real();
      s_vpi_time del;

      del.type = vpiSimTime;
      vpip_time_to_timestruct(&del, delay);

      __vpiHandle*tmp = cp->handle;

      t_vpi_মান val;
      val.format = vpiRealVal;
      val.value.real = value;
      vpi_put_মান(tmp, &val, &del, vpiTransportDelay);

      প্রত্যাগম সত্য;
}

তার্কিক of_ASSIGN_WRD(vthread_t thr, vvp_code_t cp)
{
      vvp_time64_t delay = thr->শব্দগুলি[cp->bit_idx[0]].w_uint;
      দ্বিগুণ value = thr->বের_করো_real();
      s_vpi_time del;

      del.type = vpiSimTime;
      vpip_time_to_timestruct(&del, delay);

      __vpiHandle*tmp = cp->handle;

      t_vpi_মান val;
      val.format = vpiRealVal;
      val.value.real = value;
      vpi_put_মান(tmp, &val, &del, vpiTransportDelay);

      প্রত্যাগম সত্য;
}

তার্কিক of_ASSIGN_WRE(vthread_t thr, vvp_code_t cp)
{
      প্রতিজ্ঞা(thr->event != 0);
      দ্বিগুণ value = thr->বের_করো_real();
      __vpiHandle*tmp = cp->handle;

	// If the count is zero then just put the value.
      যদি (thr->ecount == 0) {
	    t_vpi_মান val;

	    val.format = vpiRealVal;
	    val.value.real = value;
	    vpi_put_মান(tmp, &val, 0, vpiNoDelay);
      } অন্যথা {
	    schedule_evctl(tmp, value, thr->event, thr->ecount);
      }

      thr->event = 0;
      thr->ecount = 0;

      প্রত্যাগম সত্য;
}

তার্কিক of_BLEND(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t vala = thr->বের_করো_vec4();
      vvp_vector4_t valb = thr->বের_করো_vec4();
      প্রতিজ্ঞা(vala.size() == valb.size());

      প্রতি (unsigned idx = 0 ; idx < vala.size() ; idx += 1) {
	    যদি (vala.value(idx) == valb.value(idx))
		  অনুবর্ত;

	    vala.set_bit(idx, BIT4_X);
      }

      thr->ঠেলো_vec4(vala);
      প্রত্যাগম সত্য;
}

তার্কিক of_BLEND_STR(vthread_t thr, vvp_code_t)
{
      string f = thr->বের_করো_str();
      string t = thr->বের_করো_str();
      thr->ঠেলো_str((t == f) ? t : "");
      প্রত্যাগম সত্য;
}

তার্কিক of_BLEND_WR(vthread_t thr, vvp_code_t)
{
      দ্বিগুণ f = thr->বের_করো_real();
      দ্বিগুণ t = thr->বের_করো_real();
      thr->ঠেলো_real((t == f) ? t : 0.0);
      প্রত্যাগম সত্য;
}

তার্কিক of_BREAKPOINT(vthread_t, vvp_code_t)
{
      প্রত্যাগম সত্য;
}

/*
 * %callf/void <code-label>, <scope-label>
 * Combine the %fork and %join steps for invoking a function.
 */
স্থির তার্কিক do_callf_void(vthread_t thr, vthread_t child)
{

      যদি (child->parent_scope->is_automatic()) {
	      /* The context allocated for this child is the top entry
		 on the write context stack */
	    child->wt_context = thr->wt_context;
	    child->rd_context = thr->wt_context;
      }

        // Mark the function thread as a direct child of the current thread.
      child->parent = thr;
      thr->children.insert(child);
        // This should be the only child
      প্রতিজ্ঞা(thr->children.size()==1);

        // Execute the function. This SHOULD run the function to completion,
        // but there are some exceptional situations where it won't.
      প্রতিজ্ঞা(child->parent_scope->get_type_code() == vpiFunction);
      child->is_scheduled = 1;
      child->i_am_in_function = 1;
      vthread_run(child);
      running_thread = thr;

      যদি (child->i_have_ended) {
	    do_join(thr, child);
	    প্রত্যাগম সত্য;
      } অন্যথা {
	    thr->i_am_joining = 1;
	    প্রত্যাগম অসত্য;
      }
}

তার্কিক of_CALLF_OBJ(vthread_t thr, vvp_code_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);
      প্রত্যাগম do_callf_void(thr, child);

      // XXXX NOT IMPLEMENTED
}

তার্কিক of_CALLF_REAL(vthread_t thr, vvp_code_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);

	// This is the return value. Push a place-holder value. The function
	// will replace this with the actual value using a %ret/real instruction.
      thr->ঠেলো_real(0.0);
      child->args_real.push_back(0);

      প্রত্যাগম do_callf_void(thr, child);
}

তার্কিক of_CALLF_STR(vthread_t thr, vvp_code_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);

      thr->ঠেলো_str("");
      child->args_str.push_back(0);

      প্রত্যাগম do_callf_void(thr, child);
}

তার্কিক of_CALLF_VEC4(vthread_t thr, vvp_code_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);

      vpiScopeFunction*scope_func = dynamic_cast<vpiScopeFunction*>(cp->scope);
      প্রতিজ্ঞা(scope_func);

	// This is the return value. Push a place-holder value. The function
	// will replace this with the actual value using a %ret/real instruction.
      thr->ঠেলো_vec4(vvp_vector4_t(scope_func->get_func_width(), scope_func->get_func_init_val()));
      child->args_vec4.push_back(0);

      প্রত্যাগম do_callf_void(thr, child);
}

তার্কিক of_CALLF_VOID(vthread_t thr, vvp_code_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);
      প্রত্যাগম do_callf_void(thr, child);
}

/*
 * The %cassign/link instruction connects a source node to a
 * destination node. The destination node must be a signal, as it is
 * marked with the source of the cassign so that it may later be
 * unlinked without specifically knowing the source that this
 * instruction used.
 */
তার্কিক of_CASSIGN_LINK(vthread_t, vvp_code_t cp)
{
      vvp_net_t*dst = cp->net;
      vvp_net_t*src = cp->net2;

      vvp_fun_signal_base*sig
	    = dynamic_cast<vvp_fun_signal_base*>(dst->fun);
      প্রতিজ্ঞা(sig);

	/* Any previous continuous assign should have been removed already. */
      প্রতিজ্ঞা(sig->cassign_link == 0);

      sig->cassign_link = src;

	/* Link the output of the src to the port[1] (the cassign
	   port) of the destination. */
      vvp_net_ptr_t dst_ptr (dst, 1);
      src->link(dst_ptr);

      প্রত্যাগম সত্য;
}

/*
 * If there is an existing continuous assign linked to the destination
 * node, unlink it. This must be done before applying a new continuous
 * assign, otherwise the initial assigned value will be propagated to
 * any other nodes driven by the old continuous assign source.
 */
স্থির শূন্য cassign_unlink(vvp_net_t*dst)
{
      vvp_fun_signal_base*sig
	    = dynamic_cast<vvp_fun_signal_base*>(dst->fun);
      প্রতিজ্ঞা(sig);

      যদি (sig->cassign_link == 0)
	    প্রত্যাগম;

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
তার্কিক of_CASSIGN_VEC4(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      vvp_vector4_t value = thr->বের_করো_vec4();

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

	/* Set the value into port 1 of the destination. */
      vvp_net_ptr_t ptr (net, 1);
      vvp_send_vec4(ptr, value, 0);

      প্রত্যাগম সত্য;
}

/*
 * %cassign/vec4/off <var>, <off>
 */
তার্কিক of_CASSIGN_VEC4_OFF(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      unsigned base_idx = cp->bit_idx[0];
      long base = thr->শব্দগুলি[base_idx].w_int;
      vvp_vector4_t value = thr->বের_করো_vec4();
      unsigned wid = value.size();

      যদি (thr->flags[4] == BIT4_1)
	    প্রত্যাগম সত্য;

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

      vvp_signal_মান*sig = dynamic_cast<vvp_signal_মান*> (net->fil);
      প্রতিজ্ঞা(sig);

      যদি (base < 0 && (wid <= (unsigned)-base))
	    প্রত্যাগম সত্য;

      যদি (base >= (long)sig->মান_size())
	    প্রত্যাগম সত্য;

      যদি (base < 0) {
	    wid -= (unsigned) -base;
	    base = 0;
	    value.resize(wid);
      }

      যদি (base+wid > sig->মান_size()) {
	    wid = sig->মান_size() - base;
	    value.resize(wid);
      }

      vvp_net_ptr_t ptr (net, 1);
      vvp_send_vec4_pv(ptr, value, base, sig->মান_size(), 0);
      প্রত্যাগম সত্য;
}

তার্কিক of_CASSIGN_WR(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net  = cp->net;
      দ্বিগুণ value = thr->বের_করো_real();

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

	/* Set the value into port 1 of the destination. */
      vvp_net_ptr_t ptr (net, 1);
      vvp_send_real(ptr, value, 0);

      প্রত্যাগম সত্য;
}

/*
 * %cast2
 */
তার্কিক of_CAST2(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      unsigned wid = val.size();

      প্রতি (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    বিকল্প (val.value(idx)) {
		পক্ষ BIT4_0:
		পক্ষ BIT4_1:
		  ভঙ্গ;
		স্বভাব:
		  val.set_bit(idx, BIT4_0);
		  ভঙ্গ;
	    }
      }

      প্রত্যাগম সত্য;
}

তার্কিক do_cast_vec_dar(vthread_t thr, vvp_code_t cp, তার্কিক as_vec4)
{
      unsigned wid = cp->সংখ্যা;

      vvp_object_t obj;
      thr->বের_করো_object(obj);

      vvp_darray*darray = obj.peek<vvp_darray>();
      প্রতিজ্ঞা(darray);

      vvp_vector4_t vec = darray->get_bitstream(as_vec4);
      যদি (vec.size() != wid) {
	    cerr << thr->get_fileline()
	         << "VVP error: size mismatch when casting dynamic array to vector." << endl;
            thr->ঠেলো_vec4(vvp_vector4_t(wid));
            schedule_stop(0);
            প্রত্যাগম অসত্য;
      }
      thr->ঠেলো_vec4(vec);
      প্রত্যাগম সত্য;
}

/*
 * %cast/vec2/dar <wid>
 */
তার্কিক of_CAST_VEC2_DAR(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম do_cast_vec_dar(thr, cp, অসত্য);
}

/*
 * %cast/vec4/dar <wid>
 */
তার্কিক of_CAST_VEC4_DAR(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম do_cast_vec_dar(thr, cp, সত্য);
}

/*
 * %cast/vec4/str <wid>
 */
তার্কিক of_CAST_VEC4_STR(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->সংখ্যা;
      string str = thr->বের_করো_str();

      vvp_vector4_t vec(wid, BIT4_0);

      যদি (wid != 8*str.length()) {
	    cerr << thr->get_fileline()
	         << "VVP error: size mismatch when casting string to vector." << endl;
            thr->ঠেলো_vec4(vec);
            schedule_stop(0);
            প্রত্যাগম অসত্য;
      }

      unsigned sdx = 0;
      unsigned vdx = wid;
      যাবত্ (vdx > 0) {
            বর্ণ ch = str[sdx++];
            vdx -= 8;
            প্রতি (unsigned bdx = 0; bdx < 8; bdx += 1) {
                  যদি (ch & 1)
                        vec.set_bit(vdx+bdx, BIT4_1);
                  ch >>= 1;
            }
      }

      thr->ঠেলো_vec4(vec);
      প্রত্যাগম সত্য;
}

স্থির শূন্য do_CMPE(vthread_t thr, ধ্রুব vvp_vector4_t&lval, ধ্রুব vvp_vector4_t&rval)
{
      প্রতিজ্ঞা(rval.size() == lval.size());

      যদি (lval.has_xz() || rval.has_xz()) {

	    unsigned wid = lval.size();
	    vvp_bit4_t eq  = BIT4_1;
	    vvp_bit4_t eeq = BIT4_1;

	    প্রতি (unsigned idx = 0 ; idx < wid ; idx += 1) {
		  vvp_bit4_t lv = lval.value(idx);
		  vvp_bit4_t rv = rval.value(idx);

		  যদি (lv != rv)
			eeq = BIT4_0;

		  যদি (eq==BIT4_1 && (bit4_is_xz(lv) || bit4_is_xz(rv)))
			eq = BIT4_X;
		  যদি ((lv == BIT4_0) && (rv==BIT4_1))
			eq = BIT4_0;
		  যদি ((lv == BIT4_1) && (rv==BIT4_0))
			eq = BIT4_0;

		  যদি (eq == BIT4_0)
			ভঙ্গ;
	    }

	    thr->flags[4] = eq;
	    thr->flags[6] = eeq;

      } অন্যথা {
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
তার্কিক of_CMPE(vthread_t thr, vvp_code_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ধ্রুব vvp_vector4_t&rval = thr->peek_vec4(0);
      ধ্রুব vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPE(thr, lval, rval);

      thr->বের_করো_vec4(2);
      প্রত্যাগম সত্য;
}

তার্কিক of_CMPNE(vthread_t thr, vvp_code_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ধ্রুব vvp_vector4_t&rval = thr->peek_vec4(0);
      ধ্রুব vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];
      thr->flags[6] =  ~thr->flags[6];

      thr->বের_করো_vec4(2);
      প্রত্যাগম সত্য;
}

/*
 * %cmpi/e <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
তার্কিক of_CMPIE(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->সংখ্যা;

      ধ্রুব vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_immediate_rval (cp, rval);

      do_CMPE(thr, lval, rval);

      thr->বের_করো_vec4(1);
      প্রত্যাগম সত্য;
}

তার্কিক of_CMPINE(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->সংখ্যা;

      ধ্রুব vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_immediate_rval (cp, rval);

      do_CMPE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];
      thr->flags[6] =  ~thr->flags[6];

      thr->বের_করো_vec4(1);
      প্রত্যাগম সত্য;
}



স্থির শূন্য do_CMPS(vthread_t thr, ধ্রুব vvp_vector4_t&lval, ধ্রুব vvp_vector4_t&rval)
{
      প্রতিজ্ঞা(rval.size() == lval.size());

	// If either value has XZ bits, then the eq and lt values are
	// known already to be X. Just calculate the eeq result as a
	// special case and short circuit the rest of the compare.
      যদি (lval.has_xz() || rval.has_xz()) {
	    thr->flags[4] = BIT4_X; // eq
	    thr->flags[5] = BIT4_X; // lt
	    thr->flags[6] = lval.eeq(rval)? BIT4_1 : BIT4_0;
	    প্রত্যাগম;
      }

	// Past this point, we know we are dealing only with fully
	// defined values.
      unsigned wid = lval.size();

      ধ্রুব vvp_bit4_t sig1 = lval.value(wid-1);
      ধ্রুব vvp_bit4_t sig2 = rval.value(wid-1);

	// If the lval is <0 and the rval is >=0, then we know the result.
      যদি ((sig1 == BIT4_1) && (sig2 == BIT4_0)) {
	    thr->flags[4] = BIT4_0; // eq;
	    thr->flags[5] = BIT4_1; // lt;
	    thr->flags[6] = BIT4_0; // eeq
	    প্রত্যাগম;
      }

	// If the lval is >=0 and the rval is <0, then we know the result.
      যদি ((sig1 == BIT4_0) && (sig2 == BIT4_1)) {
	    thr->flags[4] = BIT4_0; // eq;
	    thr->flags[5] = BIT4_0; // lt;
	    thr->flags[6] = BIT4_0; // eeq
	    প্রত্যাগম;
      }

	// The values have the same sign, so we have to look at the
	// actual value. Scan from the MSB down. As soon as we find a
	// bit that differs, we know the result.

      প্রতি (unsigned idx = 1 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t lv = lval.value(wid-1-idx);
	    vvp_bit4_t rv = rval.value(wid-1-idx);

	    যদি (lv == rv)
		  অনুবর্ত;

	    thr->flags[4] = BIT4_0; // eq
	    thr->flags[6] = BIT4_0; // eeq

	    যদি (lv==BIT4_0) {
		  thr->flags[5] = BIT4_1; // lt
	    } অন্যথা {
		  thr->flags[5] = BIT4_0; // lt
	    }
	    প্রত্যাগম;
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
তার্কিক of_CMPS(vthread_t thr, vvp_code_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ধ্রুব vvp_vector4_t&rval = thr->peek_vec4(0);
      ধ্রুব vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPS(thr, lval, rval);

      thr->বের_করো_vec4(2);
      প্রত্যাগম সত্য;
}

/*
 * %cmpi/s <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
তার্কিক of_CMPIS(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->সংখ্যা;

      ধ্রুব vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_immediate_rval (cp, rval);

      do_CMPS(thr, lval, rval);

      thr->বের_করো_vec4(1);
      প্রত্যাগম সত্য;
}

তার্কিক of_CMPSTR(vthread_t thr, vvp_code_t)
{
      string re = thr->বের_করো_str();
      string le = thr->বের_করো_str();

      পূর্ণাঙ্ক rc = strcmp(le.c_str(), re.c_str());

      vvp_bit4_t eq;
      vvp_bit4_t lt;

      যদি (rc == 0) {
	    eq = BIT4_1;
	    lt = BIT4_0;
      } অন্যথা যদি (rc < 0) {
	    eq = BIT4_0;
	    lt = BIT4_1;
      } অন্যথা {
	    eq = BIT4_0;
	    lt = BIT4_0;
      }

      thr->flags[4] = eq;
      thr->flags[5] = lt;

      প্রত্যাগম সত্য;
}

স্থির শূন্য of_CMPU_the_hard_way(vthread_t thr, unsigned wid,
				 ধ্রুব vvp_vector4_t&lval,
				 ধ্রুব vvp_vector4_t&rval)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_bit4_t eeq = BIT4_1;

      প্রতি (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t lv = lval.value(idx);
	    vvp_bit4_t rv = rval.value(idx);

	    যদি (lv != rv)
		  eeq = BIT4_0;

	    যদি (eq==BIT4_1 && (bit4_is_xz(lv) || bit4_is_xz(rv)))
		  eq = BIT4_X;
	    যদি ((lv == BIT4_0) && (rv==BIT4_1))
		  eq = BIT4_0;
	    যদি ((lv == BIT4_1) && (rv==BIT4_0))
		  eq = BIT4_0;

	    যদি (eq == BIT4_0)
		  ভঙ্গ;

      }

      thr->flags[4] = eq;
      thr->flags[5] = BIT4_X;
      thr->flags[6] = eeq;
}

স্থির শূন্য do_CMPU(vthread_t thr, ধ্রুব vvp_vector4_t&lval, ধ্রুব vvp_vector4_t&rval)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_bit4_t lt = BIT4_0;

      যদি (rval.size() != lval.size()) {
	    cerr << thr->get_fileline()
	         << "VVP ERROR: %cmp/u operand width mismatch: lval=" << lval
		 << ", rval=" << rval << endl;
      }
      প্রতিজ্ঞা(rval.size() == lval.size());
      unsigned wid = lval.size();

      unsigned long*larray = lval.subarray(0,wid);
      যদি (larray == 0) প্রত্যাগম of_CMPU_the_hard_way(thr, wid, lval, rval);

      unsigned long*rarray = rval.subarray(0,wid);
      যদি (rarray == 0) {
	    delete[]larray;
	    প্রত্যাগম of_CMPU_the_hard_way(thr, wid, lval, rval);
      }

      unsigned শব্দগুলি = (wid+CPU_WORD_BITS-1) / CPU_WORD_BITS;

      প্রতি (unsigned wdx = 0 ; wdx < শব্দগুলি ; wdx += 1) {
	    যদি (larray[wdx] == rarray[wdx])
		  অনুবর্ত;

	    eq = BIT4_0;
	    যদি (larray[wdx] < rarray[wdx])
		  lt = BIT4_1;
	    অন্যথা
		  lt = BIT4_0;
      }

      delete[]larray;
      delete[]rarray;

      thr->flags[4] = eq;
      thr->flags[5] = lt;
      thr->flags[6] = eq;
}

তার্কিক of_CMPU(vthread_t thr, vvp_code_t)
{

      ধ্রুব vvp_vector4_t&rval = thr->peek_vec4(0);
      ধ্রুব vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPU(thr, lval, rval);

      thr->বের_করো_vec4(2);
      প্রত্যাগম সত্য;
}

/*
 * %cmpi/u <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
তার্কিক of_CMPIU(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->সংখ্যা;

      ধ্রুব vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_immediate_rval (cp, rval);

      do_CMPU(thr, lval, rval);

      thr->বের_করো_vec4(1);
      প্রত্যাগম সত্য;
}


/*
 * %cmp/x
 */
তার্কিক of_CMPX(vthread_t thr, vvp_code_t)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_vector4_t rval = thr->বের_করো_vec4();
      vvp_vector4_t lval = thr->বের_করো_vec4();

      প্রতিজ্ঞা(rval.size() == lval.size());
      unsigned wid = lval.size();

      প্রতি (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    vvp_bit4_t lv = lval.value(idx);
	    vvp_bit4_t rv = rval.value(idx);
	    যদি ((lv != rv) && !bit4_is_xz(lv) && !bit4_is_xz(rv)) {
		  eq = BIT4_0;
		  ভঙ্গ;
	    }
      }

      thr->flags[4] = eq;
      প্রত্যাগম সত্য;
}

স্থির শূন্য do_CMPWE(vthread_t thr, ধ্রুব vvp_vector4_t&lval, ধ্রুব vvp_vector4_t&rval)
{
      প্রতিজ্ঞা(rval.size() == lval.size());

      যদি (lval.has_xz() || rval.has_xz()) {

	    unsigned wid = lval.size();
	    vvp_bit4_t eq  = BIT4_1;

	    প্রতি (unsigned idx = 0 ; idx < wid ; idx += 1) {
		  vvp_bit4_t lv = lval.value(idx);
		  vvp_bit4_t rv = rval.value(idx);

		  যদি (bit4_is_xz(rv))
			অনুবর্ত;
		  যদি ((eq == BIT4_1) && bit4_is_xz(lv))
			eq = BIT4_X;
		  যদি ((lv == BIT4_0) && (rv==BIT4_1))
			eq = BIT4_0;
		  যদি ((lv == BIT4_1) && (rv==BIT4_0))
			eq = BIT4_0;

		  যদি (eq == BIT4_0)
			ভঙ্গ;
	    }

	    thr->flags[4] = eq;

      } অন্যথা {
	      // If there are no XZ bits anywhere, then the results of
	      // ==? match the === test.
	    thr->flags[4] = (lval.eeq(rval)? BIT4_1 : BIT4_0);
      }
}

তার্কিক of_CMPWE(vthread_t thr, vvp_code_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ধ্রুব vvp_vector4_t&rval = thr->peek_vec4(0);
      ধ্রুব vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPWE(thr, lval, rval);

      thr->বের_করো_vec4(2);
      প্রত্যাগম সত্য;
}

তার্কিক of_CMPWNE(vthread_t thr, vvp_code_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ধ্রুব vvp_vector4_t&rval = thr->peek_vec4(0);
      ধ্রুব vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPWE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];

      thr->বের_করো_vec4(2);
      প্রত্যাগম সত্য;
}

তার্কিক of_CMPWR(vthread_t thr, vvp_code_t)
{
      দ্বিগুণ r = thr->বের_করো_real();
      দ্বিগুণ l = thr->বের_করো_real();

      vvp_bit4_t eq = (l == r)? BIT4_1 : BIT4_0;
      vvp_bit4_t lt = (l <  r)? BIT4_1 : BIT4_0;

      thr->flags[4] = eq;
      thr->flags[5] = lt;

      প্রত্যাগম সত্য;
}

/*
 * %cmp/z
 */
তার্কিক of_CMPZ(vthread_t thr, vvp_code_t)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_vector4_t rval = thr->বের_করো_vec4();
      vvp_vector4_t lval = thr->বের_করো_vec4();

      প্রতিজ্ঞা(rval.size() == lval.size());
      unsigned wid = lval.size();

      প্রতি (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    vvp_bit4_t lv = lval.value(idx);
	    vvp_bit4_t rv = rval.value(idx);
	    যদি ((lv != rv) && (rv != BIT4_Z) && (lv != BIT4_Z)) {
		  eq = BIT4_0;
		  ভঙ্গ;
	    }
      }

      thr->flags[4] = eq;
      প্রত্যাগম সত্য;
}

/*
 *  %concat/str;
 */
তার্কিক of_CONCAT_STR(vthread_t thr, vvp_code_t)
{
      string text = thr->বের_করো_str();
      thr->peek_str(0).append(text);
      প্রত্যাগম সত্য;
}

/*
 *  %concati/str <string>;
 */
তার্কিক of_CONCATI_STR(vthread_t thr, vvp_code_t cp)
{
      ধ্রুব বর্ণ*text = cp->text;
      thr->peek_str(0).append(filter_string(text));
      প্রত্যাগম সত্য;
}

/*
 * %concat/vec4
 */
তার্কিক of_CONCAT_VEC4(vthread_t thr, vvp_code_t)
{
      ধ্রুব vvp_vector4_t&lsb = thr->peek_vec4(0);
      ধ্রুব vvp_vector4_t&msb = thr->peek_vec4(1);

	// The result is the size of the top two vectors in the stack.
      vvp_vector4_t res (msb.size() + lsb.size(), BIT4_X);

	// Build up the result.
      res.set_vec(0, lsb);
      res.set_vec(lsb.size(), msb);

	// Rearrange the stack to pop the inputs and push the
	// result. Do that by actually popping only 1 stack position
	// and replacing the new top with the new value.
      thr->বের_করো_vec4(1);
      thr->peek_vec4() = res;

      প্রত্যাগম সত্য;
}

/*
 * %concati/vec4 <vala>, <valb>, <wid>
 *
 * Concat the immediate value to the LOW bits of the concatenation.
 * Get the HIGH bits from the top of the vec4 stack.
 */
তার্কিক of_CONCATI_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned wid  = cp->সংখ্যা;

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
      প্রত্যাগম সত্য;
}

/*
 * %cvt/rv
 */
তার্কিক of_CVT_RV(vthread_t thr, vvp_code_t)
{
      দ্বিগুণ val;
      vvp_vector4_t val4 = thr->বের_করো_vec4();
      vector4_to_মান(val4, val, অসত্য);
      thr->ঠেলো_real(val);
      প্রত্যাগম সত্য;
}

/*
 * %cvt/rv/s
 */
তার্কিক of_CVT_RV_S(vthread_t thr, vvp_code_t)
{
      দ্বিগুণ val;
      vvp_vector4_t val4 = thr->বের_করো_vec4();
      vector4_to_মান(val4, val, সত্য);
      thr->ঠেলো_real(val);
      প্রত্যাগম সত্য;
}

/*
 * %cvt/sr <idx>
 * Pop the top value from the real stack, convert it to a 64bit signed
 * and save it to the indexed register.
 */
তার্কিক of_CVT_SR(vthread_t thr, vvp_code_t cp)
{
      দ্বিগুণ r = thr->বের_করো_real();
      thr->শব্দগুলি[cp->bit_idx[0]].w_int = i64round(r);

      প্রত্যাগম সত্য;
}

/*
 * %cvt/ur <idx>
 */
তার্কিক of_CVT_UR(vthread_t thr, vvp_code_t cp)
{
      দ্বিগুণ r = thr->বের_করো_real();
      thr->শব্দগুলি[cp->bit_idx[0]].w_uint = vlg_round_to_u64(r);

      প্রত্যাগম সত্য;
}

/*
 * %cvt/vr <wid>
 */
তার্কিক of_CVT_VR(vthread_t thr, vvp_code_t cp)
{
      দ্বিগুণ r = thr->বের_করো_real();
      unsigned wid = cp->সংখ্যা;

      vvp_vector4_t tmp(wid, r);
      thr->ঠেলো_vec4(tmp);
      প্রত্যাগম সত্য;
}

/*
 * This implements the %deassign instruction. All we do is write a
 * long(1) to port-3 of the addressed net. This turns off an active
 * continuous assign activated by %cassign/v
 */
তার্কিক of_DEASSIGN(vthread_t, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      unsigned base  = cp->bit_idx[0];
      unsigned width = cp->bit_idx[1];

      vvp_signal_মান*fil = dynamic_cast<vvp_signal_মান*> (net->fil);
      প্রতিজ্ঞা(fil);
      vvp_fun_signal_vec*sig = dynamic_cast<vvp_fun_signal_vec*>(net->fun);
      প্রতিজ্ঞা(sig);

      যদি (base >= fil->মান_size()) প্রত্যাগম সত্য;
      যদি (base+width > fil->মান_size()) width = fil->মান_size() - base;

      তার্কিক full_sig = base == 0 && width == fil->মান_size();

	// This is the net that is forcing me...
      যদি (vvp_net_t*src = sig->cassign_link) {
	    যদি (! full_sig) {
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
      যদি (full_sig) {
	    sig->deassign();
      } অন্যথা {
	    sig->deassign_pv(base, width);
      }

      প্রত্যাগম সত্য;
}

তার্কিক of_DEASSIGN_WR(vthread_t, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_real*sig = dynamic_cast<vvp_fun_signal_real*>(net->fun);
      প্রতিজ্ঞা(sig);

	// This is the net that is forcing me...
      যদি (vvp_net_t*src = sig->cassign_link) {
	      // And this is the pointer to be removed.
	    vvp_net_ptr_t dst_ptr (net, 1);
	    src->unlink(dst_ptr);
	    sig->cassign_link = 0;
      }

      sig->deassign();

      প্রত্যাগম সত্য;
}

/*
 * %debug/thr
 */
তার্কিক of_DEBUG_THR(vthread_t thr, vvp_code_t cp)
{
      ধ্রুব বর্ণ*text = cp->text;
      thr->debug_dump(cerr, text);
      প্রত্যাগম সত্য;
}

/*
 * The delay takes two 32bit numbers to make up a 64bit time.
 *
 *   %delay <low>, <hig>
 */
তার্কিক of_DELAY(vthread_t thr, vvp_code_t cp)
{
      vvp_time64_t নিচের = cp->bit_idx[0];
      vvp_time64_t hig = cp->bit_idx[1];

      vvp_time64_t delay = (hig << 32) | নিচের;

      যদি (delay == 0) schedule_inactive(thr);
      অন্যথা schedule_vthread(thr, delay);
      প্রত্যাগম অসত্য;
}

তার্কিক of_DELAYX(vthread_t thr, vvp_code_t cp)
{
      vvp_time64_t delay;

      প্রতিজ্ঞা(cp->সংখ্যা < vthread_s::শব্দগুলি_গুনতি);
      delay = thr->শব্দগুলি[cp->সংখ্যা].w_uint;
      যদি (delay == 0) schedule_inactive(thr);
      অন্যথা schedule_vthread(thr, delay);
      প্রত্যাগম অসত্য;
}

তার্কিক of_DELETE_ELEM(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      int64_t idx_val = thr->শব্দগুলি[3].w_int;
      যদি (thr->flags[4] == BIT4_1) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with undefined index."
	         << endl;
	    প্রত্যাগম সত্য;
      }
      যদি (idx_val < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with negative index."
	         << endl;
	    প্রত্যাগম সত্য;
      }
      size_t idx = idx_val;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      প্রতিজ্ঞা(obj);

      vvp_queue*queue = obj->get_object().peek<vvp_queue>();
      যদি (queue == 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping delete(" << idx
	         << ") on empty queue." << endl;
      } অন্যথা {
	    size_t size = queue->get_size();
	    যদি (idx >= size) {
		  cerr << thr->get_fileline()
		       << "Warning: skipping out of range delete(" << idx
		       << ") on queue of size " << size << "." << endl;
	    } অন্যথা {
		  queue->erase(idx);
	    }
      }

      প্রত্যাগম সত্য;
}

/* %delete/obj <label>
 *
 * This operator works by assigning a nil to the target object. This
 * causes any value that might be there to be garbage collected, thus
 * deleting the object.
 */
তার্কিক of_DELETE_OBJ(vthread_t thr, vvp_code_t cp)
{
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_send_object(ptr, vvp_object_t(), thr->wt_context);

      প্রত্যাগম সত্য;
}

/* %reverse/obj <label>
 * Reverse element order of the queue or dynamic array object at this net.
 */
তার্কিক of_REVERSE_OBJ(vthread_t /*thr*/, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      প্রতিজ্ঞা(obj);

      vvp_object_t stor = obj->get_object();
      যদি (stor.test_nil()) প্রত্যাগম সত্য;

	/* Match other queue ops: peek queues before vvp_darray so concrete
	   queue types dispatch reverse_elems() correctly. */
      যদি (vvp_queue* queue = stor.peek<vvp_queue>()) {
	    queue->reverse_elems();
	    প্রত্যাগম সত্য;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      প্রতিজ্ঞা(darray);
      darray->reverse_elems();

      প্রত্যাগম সত্য;
}

/* %reverse/prop/obj <property-index>
 * Reverse queue/darray property storage inside the class object on stack top.
 */
তার্কিক of_REVERSE_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->সংখ্যা;

      vvp_object_t& শীর্ষ = thr->peek_object();
      vvp_cobject*cobj = শীর্ষ.peek<vvp_cobject>();
      প্রতিজ্ঞা(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      যদি (arr_obj.test_nil()) প্রত্যাগম সত্য;

      যদি (vvp_queue* queue = arr_obj.peek<vvp_queue>()) {
	    queue->reverse_elems();
	    প্রত্যাগম সত্য;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      প্রতিজ্ঞা(darray);
      darray->reverse_elems();

      প্রত্যাগম সত্য;
}

/* %sort/obj — ascending sort */
তার্কিক of_SORT_OBJ(vthread_t /*thr*/, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      প্রতিজ্ঞা(obj);

      vvp_object_t stor = obj->get_object();
      যদি (stor.test_nil()) প্রত্যাগম সত্য;

      যদি (vvp_queue* queue = stor.peek<vvp_queue>()) {
	    queue->sort_elems(সত্য);
	    প্রত্যাগম সত্য;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      প্রতিজ্ঞা(darray);
      darray->sort_elems(সত্য);

      প্রত্যাগম সত্য;
}

/* %sort/prop/obj */
তার্কিক of_SORT_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->সংখ্যা;

      vvp_object_t& শীর্ষ = thr->peek_object();
      vvp_cobject*cobj = শীর্ষ.peek<vvp_cobject>();
      প্রতিজ্ঞা(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      যদি (arr_obj.test_nil()) প্রত্যাগম সত্য;

      যদি (vvp_queue* queue = arr_obj.peek<vvp_queue>()) {
	    queue->sort_elems(সত্য);
	    প্রত্যাগম সত্য;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      প্রতিজ্ঞা(darray);
      darray->sort_elems(সত্য);

      প্রত্যাগম সত্য;
}

/* %rsort/obj — descending sort */
তার্কিক of_RSORT_OBJ(vthread_t /*thr*/, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      প্রতিজ্ঞা(obj);

      vvp_object_t stor = obj->get_object();
      যদি (stor.test_nil()) প্রত্যাগম সত্য;

      যদি (vvp_queue* queue = stor.peek<vvp_queue>()) {
	    queue->sort_elems(অসত্য);
	    প্রত্যাগম সত্য;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      প্রতিজ্ঞা(darray);
      darray->sort_elems(অসত্য);

      প্রত্যাগম সত্য;
}

/* %rsort/prop/obj */
তার্কিক of_RSORT_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->সংখ্যা;

      vvp_object_t& শীর্ষ = thr->peek_object();
      vvp_cobject*cobj = শীর্ষ.peek<vvp_cobject>();
      প্রতিজ্ঞা(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      যদি (arr_obj.test_nil()) প্রত্যাগম সত্য;

      যদি (vvp_queue* queue = arr_obj.peek<vvp_queue>()) {
	    queue->sort_elems(অসত্য);
	    প্রত্যাগম সত্য;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      প্রতিজ্ঞা(darray);
      darray->sort_elems(অসত্য);

      প্রত্যাগম সত্য;
}

/* %shuffle/obj */
তার্কিক of_SHUFFLE_OBJ(vthread_t /*thr*/, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      প্রতিজ্ঞা(obj);

      vvp_object_t stor = obj->get_object();
      যদি (stor.test_nil()) প্রত্যাগম সত্য;

      যদি (vvp_queue* queue = stor.peek<vvp_queue>()) {
	    queue->shuffle_elems();
	    প্রত্যাগম সত্য;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      প্রতিজ্ঞা(darray);
      darray->shuffle_elems();

      প্রত্যাগম সত্য;
}

/* %shuffle/prop/obj */
তার্কিক of_SHUFFLE_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->সংখ্যা;

      vvp_object_t& শীর্ষ = thr->peek_object();
      vvp_cobject*cobj = শীর্ষ.peek<vvp_cobject>();
      প্রতিজ্ঞা(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      যদি (arr_obj.test_nil()) প্রত্যাগম সত্য;

      যদি (vvp_queue* queue = arr_obj.peek<vvp_queue>()) {
	    queue->shuffle_elems();
	    প্রত্যাগম সত্য;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      প্রতিজ্ঞা(darray);
      darray->shuffle_elems();

      প্রত্যাগম সত্য;
}

/* %delete/tail <label>, idx
 *
 * Remove all elements after the one specified.
 */
তার্কিক of_DELETE_TAIL(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      প্রতিজ্ঞা(obj);

      vvp_queue*queue = obj->get_object().peek<vvp_queue>();
      প্রতিজ্ঞা(queue);

      unsigned idx = thr->শব্দগুলি[cp->bit_idx[0]].w_int;
      queue->erase_tail(idx);

      প্রত্যাগম সত্য;
}

স্থির তার্কিক do_disable(vthread_t thr, vthread_t match)
{
      তার্কিক flag = অসত্য;

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
      যাবত্ (! thr->children.empty()) {

	    vthread_t tmp = *(thr->children.begin());
	    প্রতিজ্ঞা(tmp);
	    প্রতিজ্ঞা(tmp->parent == thr);
	    thr->i_am_joining = 0;
	    যদি (do_disable(tmp, match))
		  flag = সত্য;

	    vthread_reap(tmp);
      }

      vthread_t parent = thr->parent;
      যদি (parent && parent->i_am_joining) {
	      // If a parent is waiting in a %join, wake it up. Note
	      // that it is possible to be waiting in a %join yet
	      // already scheduled if multiple child threads are
	      // ending. So check if the thread is already scheduled
	      // before scheduling it again.
	    parent->i_am_joining = 0;
	    যদি (! parent->i_have_ended)
		  schedule_vthread(parent, 0, সত্য);

	    do_join(parent, thr);

      } অন্যথা যদি (parent) {
	      /* If the parent is yet to %join me, let its %join
		 do the reaping. */
	      //assert(tmp->is_scheduled == 0);

      } অন্যথা {
	      /* No parent at all. Goodbye. */
	    vthread_reap(thr);
      }

      প্রত্যাগম flag || (thr == match);
}

/*
 * Implement the %disable instruction by scanning the target scope for
 * all the target threads. Kill the target threads and wake up a
 * parent that is attempting a %join.
 */
তার্কিক of_DISABLE(vthread_t thr, vvp_code_t cp)
{
      __vpiScope*scope = static_cast<__vpiScope*>(cp->handle);

      তার্কিক disabled_myself_flag = অসত্য;

      যাবত্ (! scope->threads.empty()) {
	    set<vthread_t>::iterator cur = scope->threads.begin();

	    যদি (do_disable(*cur, thr))
		  disabled_myself_flag = সত্য;
      }

      প্রত্যাগম ! disabled_myself_flag;
}

/*
 * Similar to `of_DISABLE`. But will only disable a single thread of the
 * specified scope. The disabled thread will be the thread closest to the
 * current thread in thread hierarchy. This can either be the current thread,
 * either the thread itself or one of its parents.
 * This is used for SystemVerilog flow control instructions like `return`,
 * `continue` and `break`.
 */

তার্কিক of_DISABLE_FLOW(vthread_t thr, vvp_code_t cp)
{
      ধ্রুব __vpiScope*scope = static_cast<__vpiScope*>(cp->handle);
      vthread_t cur = thr;

      যাবত্ (cur && cur->parent_scope != scope)
	    cur = cur->parent;

      প্রতিজ্ঞা(cur);
      যদি (cur)
	    প্রত্যাগম !do_disable(cur, thr);

      প্রত্যাগম অসত্য;
}

/*
 * Implement the %disable/fork (SystemVerilog) instruction by disabling
 * all the detached children of the given thread.
 */
তার্কিক of_DISABLE_FORK(vthread_t thr, vvp_code_t)
{
	/* If a %disable/fork is being executed then the parent thread
	 * cannot be waiting in a join. */
      প্রতিজ্ঞা(! thr->i_am_joining);

	/* There should be no active children to disable. */
      প্রতিজ্ঞা(thr->children.empty());

	/* Disable any detached children. */
      যাবত্ (! thr->detached_children.empty()) {
	    vthread_t child = *(thr->detached_children.begin());
	    প্রতিজ্ঞা(child);
	    প্রতিজ্ঞা(child->parent == thr);
	      /* Disabling the children can never match the parent thread. */
	    তার্কিক res = do_disable(child, thr);
	    প্রতিজ্ঞা(! res);
	    vthread_reap(child);
      }

      প্রত্যাগম সত্য;
}

/*
 * This function divides a 2-word number {high, a} by a 1-word
 * number. Assume that high < b.
 */
স্থির unsigned long divide2words(unsigned long a, unsigned long b,
				  unsigned long উপরের)
{
      unsigned long result = 0;
      যাবত্ (উপরের > 0) {
	    unsigned long tmp_result = ULONG_MAX / b;
	    unsigned long remain = ULONG_MAX % b;

	    remain += 1;
	    যদি (remain >= b) {
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
	    result += tmp_result * উপরের;

	      // The new iteration starts with high*remain + a.
	    remain = multiply_with_carry(উপরের, remain, উপরের);
	    a += remain;
            যদি(a < remain)
              উপরের += 1;

	      // Now result*b + {high,a} == the input {high,a}. It is
	      // possible that the new high >= 1. If so, it will
	      // certainly be less than high from the previous
	      // iteration. Do another iteration and it will shrink,
	      // eventually to 0.
      }

	// high is now 0, so a is the remaining remainder, so we can
	// finish off the integer divide with a simple a/b.

      প্রত্যাগম result + a/b;
}

স্থির unsigned long* divide_bits(unsigned long*ap, ধ্রুব unsigned long*bp, unsigned wid)
{
	// Do all our work a cpu-word at a time. The "words" variable
	// is the number of words of the wid.
      unsigned শব্দগুলি = (wid+CPU_WORD_BITS-1) / CPU_WORD_BITS;

      unsigned btop = শব্দগুলি-1;
      যাবত্ (btop > 0 && bp[btop] == 0)
	    btop -= 1;

	// Detect divide by 0, and exit.
      যদি (btop==0 && bp[0]==0)
	    প্রত্যাগম 0;

	// The result array will eventually accumulate the result. The
	// diff array is a difference that we use in the intermediate.
      unsigned long*diff  = নূতন unsigned long[শব্দগুলি];
      unsigned long*result= নূতন unsigned long[শব্দগুলি];
      প্রতি (unsigned idx = 0 ; idx < শব্দগুলি ; idx += 1)
	    result[idx] = 0;

      প্রতি (unsigned cur = শব্দগুলি-btop ; cur > 0 ; cur -= 1) {
	    unsigned cur_ptr = cur-1;
	    unsigned long cur_res;
	    যদি (ap[cur_ptr+btop] >= bp[btop]) {
		  unsigned long উপরের = 0;
		  যদি (cur_ptr+btop+1 < শব্দগুলি)
			উপরের = ap[cur_ptr+btop+1];
		  cur_res = divide2words(ap[cur_ptr+btop], bp[btop], উপরের);

	    } অন্যথা যদি (cur_ptr+btop+1 >= শব্দগুলি) {
		  অনুবর্ত;

	    } অন্যথা যদি (ap[cur_ptr+btop+1] == 0) {
		  অনুবর্ত;

	    } অন্যথা {
		  cur_res = divide2words(ap[cur_ptr+btop], bp[btop],
					 ap[cur_ptr+btop+1]);
	    }

	      // cur_res is a guesstimate of the result this far. It
	      // may be 1 too big. (But it will also be >0) Try it,
	      // and if the difference comes out negative, then adjust.

	      // diff = (bp * cur_res)  << cur_ptr;
	    multiply_array_imm(diff+cur_ptr, bp, শব্দগুলি-cur_ptr, cur_res);
	      // ap -= diff
	    unsigned long carry = 1;
	    প্রতি (unsigned idx = cur_ptr ; idx < শব্দগুলি ; idx += 1)
		  ap[idx] = add_with_carry(ap[idx], ~diff[idx], carry);

	      // ap has the diff subtracted out of it. If cur_res was
	      // too large, then ap will turn negative. (We easily
	      // tell that ap turned negative by looking at
	      // carry&1. If it is 0, then it is *negative*.) In that
	      // case, we know that cur_res was too large by 1. Correct by
	      // adding 1b back in and reducing cur_res.
	    যদি ((carry&1) == 0) {
		    // Keep adding b back in until the remainder
		    // becomes positive again.
		  কুরু {
			cur_res -= 1;
			carry = 0;
			প্রতি (unsigned idx = cur_ptr ; idx < শব্দগুলি ; idx += 1)
			      ap[idx] = add_with_carry(ap[idx], bp[idx-cur_ptr], carry);
		  } যাবত্ (carry == 0);
	    }

	    result[cur_ptr] = cur_res;
      }

	// Now ap contains the remainder and result contains the
	// desired result. We should find that:
	//  input-a = bp * result + ap;

      delete[]diff;
      প্রত্যাগম result;
}

/*
 * %div
 */
তার্কিক of_DIV(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valb = thr->বের_করো_vec4();
      vvp_vector4_t vala = thr->বের_করো_vec4();

      প্রতিজ্ঞা(vala.size()== valb.size());
      unsigned wid = vala.size();

      unsigned long*ap = vala.subarray(0, wid);
      যদি (ap == 0) {
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->ঠেলো_vec4(tmp);
	    প্রত্যাগম সত্য;
      }

      unsigned long*bp = valb.subarray(0, wid);
      যদি (bp == 0) {
	    delete[]ap;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->ঠেলো_vec4(tmp);
	    প্রত্যাগম সত্য;
      }

	// If the value fits in a single CPU word, then do it the easy way.
      যদি (wid <= CPU_WORD_BITS) {
	    যদি (bp[0] == 0) {
		  vvp_vector4_t tmp(wid, BIT4_X);
		  thr->ঠেলো_vec4(tmp);
	    } অন্যথা {
		  ap[0] /= bp[0];
		  vala.setarray(0, wid, ap);
		  thr->ঠেলো_vec4(vala);
	    }
	    delete[]ap;
	    delete[]bp;
	    প্রত্যাগম সত্য;
      }

      unsigned long*result = divide_bits(ap, bp, wid);
      যদি (result == 0) {
	    delete[]ap;
	    delete[]bp;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->ঠেলো_vec4(tmp);
	    প্রত্যাগম সত্য;
      }

	// Now ap contains the remainder and result contains the
	// desired result. We should find that:
	//  input-a = bp * result + ap;

      vala.setarray(0, wid, result);
      thr->ঠেলো_vec4(vala);
      delete[]ap;
      delete[]bp;
      delete[]result;

      প্রত্যাগম সত্য;
}


স্থির শূন্য negate_শব্দগুলি(unsigned long*val, unsigned শব্দগুলি)
{
      unsigned long carry = 1;
      প্রতি (unsigned idx = 0 ; idx < শব্দগুলি ; idx += 1)
	    val[idx] = add_with_carry(0, ~val[idx], carry);
}

/*
 * %div/s
 */
তার্কিক of_DIV_S(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valb = thr->বের_করো_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      প্রতিজ্ঞা(vala.size()== valb.size());
      unsigned wid = vala.size();
      unsigned শব্দগুলি = (wid + CPU_WORD_BITS - 1) / CPU_WORD_BITS;

	// Get the values, left in right, in binary form. If there is
	// a problem with either (caused by an X or Z bit) then we
	// know right away that the entire result is X.
      unsigned long*ap = vala.subarray(0, wid);
      যদি (ap == 0) {
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    প্রত্যাগম সত্য;
      }

      unsigned long*bp = valb.subarray(0, wid);
      যদি (bp == 0) {
	    delete[]ap;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    প্রত্যাগম সত্য;
      }

	// Sign extend the bits in the array to fill out the array.
      unsigned long sign_mask = 0;
      যদি (unsigned long sign_bits = (শব্দগুলি*CPU_WORD_BITS) - wid) {
	    sign_mask = -1UL << (CPU_WORD_BITS-sign_bits);
	    যদি (ap[শব্দগুলি-1] & (sign_mask>>1))
		  ap[শব্দগুলি-1] |= sign_mask;
	    যদি (bp[শব্দগুলি-1] & (sign_mask>>1))
		  bp[শব্দগুলি-1] |= sign_mask;
      }

	// If the value fits in a single word, then use the native divide.
      যদি (wid <= CPU_WORD_BITS) {
	    যদি (bp[0] == 0) {
		  vvp_vector4_t tmp(wid, BIT4_X);
		  vala = tmp;
	    } অন্যথা যদি (((long)ap[0] == LONG_MIN) && ((long)bp[0] == -1)) {
		  vvp_vector4_t tmp(wid, BIT4_0);
		  tmp.set_bit(wid-1, BIT4_1);
		  vala = tmp;
	    } অন্যথা {
		  long tmpa = (long) ap[0];
		  long tmpb = (long) bp[0];
		  long res = tmpa / tmpb;
		  ap[0] = ((unsigned long)res) & ~sign_mask;
		  vala.setarray(0, wid, ap);
	    }
	    delete[]ap;
	    delete[]bp;
	    প্রত্যাগম সত্য;
      }

	// We need to the actual division to positive integers. Make
	// them positive here, and remember the negations.
      তার্কিক negate_flag = অসত্য;
      যদি ( ((long) ap[শব্দগুলি-1]) < 0 ) {
	    negate_flag = সত্য;
	    negate_শব্দগুলি(ap, শব্দগুলি);
      }
      যদি ( ((long) bp[শব্দগুলি-1]) < 0 ) {
	    negate_flag ^= সত্য;
	    negate_শব্দগুলি(bp, শব্দগুলি);
      }

      unsigned long*result = divide_bits(ap, bp, wid);
      যদি (result == 0) {
	    delete[]ap;
	    delete[]bp;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    প্রত্যাগম সত্য;
      }

      যদি (negate_flag) {
	    negate_শব্দগুলি(result, শব্দগুলি);
      }

      result[শব্দগুলি-1] &= ~sign_mask;

      vala.setarray(0, wid, result);
      delete[]ap;
      delete[]bp;
      delete[]result;
      প্রত্যাগম সত্য;
}

তার্কিক of_DIV_WR(vthread_t thr, vvp_code_t)
{
      দ্বিগুণ r = thr->বের_করো_real();
      দ্বিগুণ l = thr->বের_করো_real();
      thr->ঠেলো_real(l / r);

      প্রত্যাগম সত্য;
}

/*
 * %dup/obj
 * %dup/real
 * %dup/vec4
 *
 * Push a duplicate of the object on the appropriate stack.
 */
তার্কিক of_DUP_OBJ(vthread_t thr, vvp_code_t)
{
      vvp_object_t src = thr->peek_object();

        // If it is null push a new null object
      যদি (src.test_nil())
	    thr->ঠেলো_object(vvp_object_t());
      অন্যথা
	    thr->ঠেলো_object(src.duplicate());

      প্রত্যাগম সত্য;
}

তার্কিক of_DUP_REAL(vthread_t thr, vvp_code_t)
{
      thr->ঠেলো_real(thr->peek_real(0));
      প্রত্যাগম সত্য;
}

তার্কিক of_DUP_VEC4(vthread_t thr, vvp_code_t)
{
      thr->ঠেলো_vec4(thr->peek_vec4(0));
      প্রত্যাগম সত্য;
}

/*
 * This terminates the current thread. If there is a parent who is
 * waiting for me to die, then I schedule it. At any rate, I mark
 * myself as a zombie by setting my pc to 0.
 */
তার্কিক of_END(vthread_t thr, vvp_code_t)
{
      প্রতিজ্ঞা(! thr->waiting_for_event);
      thr->i_have_ended = 1;
      thr->pc = codespace_null();

	/* Fully detach any detached children. */
      যাবত্ (! thr->detached_children.empty()) {
	    vthread_t child = *(thr->detached_children.begin());
	    প্রতিজ্ঞা(child);
	    প্রতিজ্ঞা(child->parent == thr);
	    প্রতিজ্ঞা(child->i_am_detached);
	    child->parent = 0;
	    child->i_am_detached = 0;
	    thr->detached_children.erase(thr->detached_children.begin());
      }

	/* It is an error to still have active children running at this
	 * point in time. They should have all been detached or joined. */
      প্রতিজ্ঞা(thr->children.empty());

	/* If I have a parent who is waiting for me, then mark that I
	   have ended, and schedule that parent. Also, finish the
	   %join for the parent. */
      যদি (!thr->i_am_detached && thr->parent && thr->parent->i_am_joining) {
	    vthread_t tmp = thr->parent;
	    প্রতিজ্ঞা(! thr->i_am_detached);

	    tmp->i_am_joining = 0;
	    schedule_vthread(tmp, 0, সত্য);
	    do_join(tmp, thr);
	    প্রত্যাগম অসত্য;
      }

	/* If this thread is not fully detached then remove it from the
	 * parents detached_children set and reap it. */
      যদি (thr->i_am_detached) {
	    vthread_t tmp = thr->parent;
	    প্রতিজ্ঞা(tmp);
	    size_t res = tmp->detached_children.erase(thr);
	    প্রতিজ্ঞা(res == 1);
	      /* If the parent is waiting for the detached children to
	       * finish then the last detached child needs to tell the
	       * parent to wake up when it is finished. */
	    যদি (tmp->i_am_waiting && tmp->detached_children.empty()) {
		  tmp->i_am_waiting = 0;
		  schedule_vthread(tmp, 0, সত্য);
	    }
	      /* Fully detach this thread so it will be reaped below. */
	    thr->i_am_detached = 0;
	    thr->parent = 0;
      }

	/* If I have no parent, then no one can %join me and there is
	 * no reason to stick around. This can happen, for example if
	 * I am an ``initial'' thread. */
      যদি (thr->parent == 0) {
	    vthread_reap(thr);
	    প্রত্যাগম অসত্য;
      }

	/* If I make it this far, then I have a parent who may wish
	   to %join me. Remain a zombie so that it can. */

      প্রত্যাগম অসত্য;
}

/*
 * %event <var-label>
 */
তার্কিক of_EVENT(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_vector4_t tmp (1, BIT4_X);
      vvp_send_vec4(ptr, tmp, thr->wt_context);
      প্রত্যাগম সত্য;
}

/*
 * %event/nb <var-label>, <delay>
 */
তার্কিক of_EVENT_NB(vthread_t thr, vvp_code_t cp)
{
      vvp_time64_t delay;

      delay = thr->শব্দগুলি[cp->bit_idx[0]].w_uint;
      schedule_propagate_event(cp->net, delay);
      প্রত্যাগম সত্য;
}

তার্কিক of_EVCTL(vthread_t thr, vvp_code_t cp)
{
      প্রতিজ্ঞা(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      thr->ecount = thr->শব্দগুলি[cp->bit_idx[0]].w_uint;
      প্রত্যাগম সত্য;
}
তার্কিক of_EVCTLC(vthread_t thr, vvp_code_t)
{
      thr->event = 0;
      thr->ecount = 0;
      প্রত্যাগম সত্য;
}

তার্কিক of_EVCTLI(vthread_t thr, vvp_code_t cp)
{
      প্রতিজ্ঞা(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      thr->ecount = cp->bit_idx[0];
      প্রত্যাগম সত্য;
}

তার্কিক of_EVCTLS(vthread_t thr, vvp_code_t cp)
{
      প্রতিজ্ঞা(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      int64_t val = thr->শব্দগুলি[cp->bit_idx[0]].w_int;
      যদি (val < 0) val = 0;
      thr->ecount = val;
      প্রত্যাগম সত্য;
}

তার্কিক of_FLAG_GET_VEC4(vthread_t thr, vvp_code_t cp)
{
      পূর্ণাঙ্ক flag = cp->সংখ্যা;
      প্রতিজ্ঞা(flag < vthread_s::FLAGS_গুনতি);

      vvp_vector4_t val (1, thr->flags[flag]);
      thr->ঠেলো_vec4(val);

      প্রত্যাগম সত্য;
}

/*
 * %flag_inv <flag1>
 */
তার্কিক of_FLAG_INV(vthread_t thr, vvp_code_t cp)
{
      পূর্ণাঙ্ক flag1 = cp->bit_idx[0];

      thr->flags[flag1] = ~ thr->flags[flag1];
      প্রত্যাগম সত্য;
}

/*
 * %flag_mov <flag1>, <flag2>
 */
তার্কিক of_FLAG_MOV(vthread_t thr, vvp_code_t cp)
{
      পূর্ণাঙ্ক flag1 = cp->bit_idx[0];
      পূর্ণাঙ্ক flag2 = cp->bit_idx[1];

      thr->flags[flag1] = thr->flags[flag2];
      প্রত্যাগম সত্য;
}

/*
 * %flag_or <flag1>, <flag2>
 */
তার্কিক of_FLAG_OR(vthread_t thr, vvp_code_t cp)
{
      পূর্ণাঙ্ক flag1 = cp->bit_idx[0];
      পূর্ণাঙ্ক flag2 = cp->bit_idx[1];

      thr->flags[flag1] = thr->flags[flag1] | thr->flags[flag2];
      প্রত্যাগম সত্য;
}

তার্কিক of_FLAG_SET_IMM(vthread_t thr, vvp_code_t cp)
{
      পূর্ণাঙ্ক flag = cp->সংখ্যা;
      পূর্ণাঙ্ক vali = cp->bit_idx[0];

      প্রতিজ্ঞা(flag < vthread_s::FLAGS_গুনতি);
      প্রতিজ্ঞা(vali >= 0 && vali < 4);

      স্থির ধ্রুব vvp_bit4_t map_bit[4] = {BIT4_0, BIT4_1, BIT4_Z, BIT4_X};
      thr->flags[flag] = map_bit[vali];
      প্রত্যাগম সত্য;
}

তার্কিক of_FLAG_SET_VEC4(vthread_t thr, vvp_code_t cp)
{
      পূর্ণাঙ্ক flag = cp->সংখ্যা;
      প্রতিজ্ঞা(flag < vthread_s::FLAGS_গুনতি);

      ধ্রুব vvp_vector4_t&val = thr->peek_vec4();
      thr->flags[flag] = val.value(0);
      thr->বের_করো_vec4(1);

      প্রত্যাগম সত্য;
}

/*
 * the %force/link instruction connects a source node to a
 * destination node. The destination node must be a signal, as it is
 * marked with the source of the force so that it may later be
 * unlinked without specifically knowing the source that this
 * instruction used.
 */
তার্কিক of_FORCE_LINK(vthread_t, vvp_code_t cp)
{
      vvp_net_t*dst = cp->net;
      vvp_net_t*src = cp->net2;

      প্রতিজ্ঞা(dst->fil);
      dst->fil->force_link(dst, src);

      প্রত্যাগম সত্য;
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
তার্কিক of_FORCE_VEC4(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_vector4_t value = thr->বের_করো_vec4();

	/* Send the force value to the filter on the node. */

      প্রতিজ্ঞা(net->fil);
      যদি (value.size() != net->fil->filter_size())
	    value = coerce_to_width(value, net->fil->filter_size());

      net->force_vec4(value, vvp_vector2_t(vvp_vector2_t::FILL1, net->fil->filter_size()));

      প্রত্যাগম সত্য;
}

/*
 * %force/vec4/off <net>, <off>
 */
তার্কিক of_FORCE_VEC4_OFF(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      unsigned base_idx = cp->bit_idx[0];
      long base = thr->শব্দগুলি[base_idx].w_int;
      vvp_vector4_t value = thr->বের_করো_vec4();
      unsigned wid = value.size();

      প্রতিজ্ঞা(net->fil);

      যদি (thr->flags[4] == BIT4_1)
	    প্রত্যাগম সত্য;

	// This is the width of the target vector.
      unsigned use_size = net->fil->filter_size();

      যদি (base >= (long)use_size)
	    প্রত্যাগম সত্য;
      যদি (base < -(long)use_size)
	    প্রত্যাগম সত্য;

      যদি ((base + wid) > use_size)
	    wid = use_size - base;

	// Make a mask of which bits are to be forced, 0 for unforced
	// bits and 1 for forced bits.
      vvp_vector2_t mask (vvp_vector2_t::FILL0, use_size);
      প্রতি (unsigned idx = 0 ; idx < wid ; idx += 1)
	    mask.set_bit(base+idx, 1);

      vvp_vector4_t tmp (use_size, BIT4_Z);

	// vvp_net_t::force_vec4 propagates all the bits of the
	// forced vector value, regardless of the mask. This
	// ensures the unforced bits retain their current value.
      vvp_signal_মান*sig = dynamic_cast<vvp_signal_মান*>(net->fil);
      প্রতিজ্ঞা(sig);
      sig->vec4_মান(tmp);

      tmp.set_vec(base, value);

      net->force_vec4(tmp, mask);
      প্রত্যাগম সত্য;
}

/*
 * %force/vec4/off/d <net>, <off>, <del>
 */
তার্কিক of_FORCE_VEC4_OFF_D(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      unsigned base_idx = cp->bit_idx[0];
      long base = thr->শব্দগুলি[base_idx].w_int;

      unsigned delay_idx = cp->bit_idx[1];
      vvp_time64_t delay = thr->শব্দগুলি[delay_idx].w_uint;

      vvp_vector4_t value = thr->বের_করো_vec4();

      প্রতিজ্ঞা(net->fil);

      যদি (thr->flags[4] == BIT4_1)
	    প্রত্যাগম সত্য;

	// This is the width of the target vector.
      unsigned use_size = net->fil->filter_size();

      যদি (base >= (long)use_size)
	    প্রত্যাগম সত্য;
      যদি (base < -(long)use_size)
	    প্রত্যাগম সত্য;

      schedule_force_vector(net, base, use_size, value, delay);
      প্রত্যাগম সত্য;
}

তার্কিক of_FORCE_WR(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net  = cp->net;
      দ্বিগুণ value = thr->বের_করো_real();

      net->force_real(value, vvp_vector2_t(vvp_vector2_t::FILL1, 1));

      প্রত্যাগম সত্য;
}

/*
 * The %fork instruction causes a new child to be created and pushed
 * in front of any existing child. This causes the new child to be
 * added to the list of children, and for me to be the parent of the
 * new child.
 */
তার্কিক of_FORK(vthread_t thr, vvp_code_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);

      যদি (cp->scope->is_automatic()) {
              /* The context allocated for this child is the top entry
                 on the write context stack. */
            child->wt_context = thr->wt_context;
            child->rd_context = thr->wt_context;
      }

      child->parent = thr;
      thr->children.insert(child);

      যদি (thr->i_am_in_function) {
	    child->is_scheduled = 1;
	    child->i_am_in_function = 1;
	    vthread_run(child);
	    running_thread = thr;
      } অন্যথা {
	    schedule_vthread(child, 0, সত্য);
      }
      প্রত্যাগম সত্য;
}

তার্কিক of_FREE(vthread_t thr, vvp_code_t cp)
{
        /* Pop the child context from the read context stack. */
      vvp_context_t child_context = thr->rd_context;
      thr->rd_context = vvp_get_stacked_context(child_context);

        /* Free the context. */
      vthread_free_context(child_context, cp->scope);

      প্রত্যাগম সত্য;
}

/*
 * %inv
 *
 * Logically, this pops a value, inverts is (Verilog style, with Z and
 * X converted to X) and pushes the result. We can more efficiently
 * just to the invert in place.
 */
তার্কিক of_INV(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val.invert();
      প্রত্যাগম সত্য;
}


/*
 * Index registers, arithmetic.
 */

স্থির অন্তঃস্থ int64_t get_as_64_bit(uint32_t নিচের_32, uint32_t উপরের_32)
{
      int64_t নিচের = নিচের_32;
      int64_t res = উপরের_32;

      res <<= 32;
      res |= নিচের;
      প্রত্যাগম res;
}

তার্কিক of_IX_ADD(vthread_t thr, vvp_code_t cp)
{
      thr->শব্দগুলি[cp->সংখ্যা].w_int += get_as_64_bit(cp->bit_idx[0],
                                                    cp->bit_idx[1]);
      প্রত্যাগম সত্য;
}

তার্কিক of_IX_SUB(vthread_t thr, vvp_code_t cp)
{
      thr->শব্দগুলি[cp->সংখ্যা].w_int -= get_as_64_bit(cp->bit_idx[0],
                                                    cp->bit_idx[1]);
      প্রত্যাগম সত্য;
}

তার্কিক of_IX_MUL(vthread_t thr, vvp_code_t cp)
{
      thr->শব্দগুলি[cp->সংখ্যা].w_int *= get_as_64_bit(cp->bit_idx[0],
                                                    cp->bit_idx[1]);
      প্রত্যাগম সত্য;
}

তার্কিক of_IX_LOAD(vthread_t thr, vvp_code_t cp)
{
      thr->শব্দগুলি[cp->সংখ্যা].w_int = get_as_64_bit(cp->bit_idx[0],
                                                   cp->bit_idx[1]);
      প্রত্যাগম সত্য;
}

তার্কিক of_IX_MOV(vthread_t thr, vvp_code_t cp)
{
      thr->শব্দগুলি[cp->bit_idx[0]].w_int = thr->শব্দগুলি[cp->bit_idx[1]].w_int;
      প্রত্যাগম সত্য;
}

তার্কিক of_IX_GETV(vthread_t thr, vvp_code_t cp)
{
      unsigned index = cp->bit_idx[0];
      vvp_net_t*net = cp->net;

      vvp_signal_মান*sig = dynamic_cast<vvp_signal_মান*>(net->fil);
      যদি (sig == 0) {
	    প্রতিজ্ঞা(net->fil);
	    cerr << thr->get_fileline()
	         << "%%ix/getv error: Net arg not a vector signal? "
		 << typeid(*net->fil).name() << endl;
      }
      প্রতিজ্ঞা(sig);

      vvp_vector4_t vec;
      sig->vec4_মান(vec);
      তার্কিক overflow_flag;
      uint64_t val = 0;
      তার্কিক known_flag = vector4_to_মান(vec, overflow_flag, val);

      যদি (known_flag)
	    thr->শব্দগুলি[index].w_uint = val;
      অন্যথা
	    thr->শব্দগুলি[index].w_uint = 0;

	/* Set bit 4 as a flag if the input is unknown. */
      thr->flags[4] = known_flag ? (overflow_flag ? BIT4_X : BIT4_0) : BIT4_1;

      প্রত্যাগম সত্য;
}

তার্কিক of_IX_GETV_S(vthread_t thr, vvp_code_t cp)
{
      unsigned index = cp->bit_idx[0];
      vvp_net_t*net = cp->net;

      vvp_signal_মান*sig = dynamic_cast<vvp_signal_মান*>(net->fil);
      যদি (sig == 0) {
	    প্রতিজ্ঞা(net->fil);
	    cerr << thr->get_fileline()
	         << "%%ix/getv/s error: Net arg not a vector signal? "
		 << "fun=" << typeid(*net->fil).name()
		 << ", fil=" << (net->fil? typeid(*net->fil).name() : "<>")
		 << endl;
      }
      প্রতিজ্ঞা(sig);

      vvp_vector4_t vec;
      sig->vec4_মান(vec);
      int64_t val;
      তার্কিক known_flag = vector4_to_মান(vec, val, সত্য, সত্য);

      যদি (known_flag)
	    thr->শব্দগুলি[index].w_int = val;
      অন্যথা
	    thr->শব্দগুলি[index].w_int = 0;

	/* Set bit 4 as a flag if the input is unknown. */
      thr->flags[4] = known_flag? BIT4_0 : BIT4_1;

      প্রত্যাগম সত্য;
}

স্থির uint64_t vec4_to_index(vthread_t thr, তার্কিক signed_flag)
{
	// Get all the information we need about the vec4 vector, then
	// pop it away. We only need the bool bits and the length.
      ধ্রুব vvp_vector4_t&val = thr->peek_vec4();
      unsigned val_size = val.size();
      unsigned long*bits = val.subarray(0, val_size, অসত্য);
      thr->বের_করো_vec4(1);

	// If there are X/Z bits, then the subarray will give us a nil
	// pointer. Set a flag to indicate the error, and give up.
      যদি (bits == 0) {
	    thr->flags[4] = BIT4_1;
	    প্রত্যাগম 0;
      }

      uint64_t v = 0;
      thr->flags[4] = BIT4_0;

      প্রতিজ্ঞা(পরিমাণ(bits[0]) <= পরিমাণ(v));

      v = 0;
      প্রতি (unsigned idx = 0 ; idx < val_size ; idx += 8*পরিমাণ(bits[0])) {
	    uint64_t tmp = bits[idx/8/পরিমাণ(bits[0])];
	    যদি (idx < 8*পরিমাণ(v)) {
		  v |= tmp << idx;
	    } অন্যথা {
		  তার্কিক overflow = signed_flag && (v >> 63) ? ~tmp != 0 : tmp != 0;
		  যদি (overflow) {
			thr->flags[4] = BIT4_X;
			ভঙ্গ;
		  }
	    }
      }

	// Set the high bits that are not necessarily filled in by the
	// subarray function.
      যদি (val_size < 8*পরিমাণ(v)) {
	    যদি (signed_flag && (v & (static_cast<uint64_t>(1)<<(val_size-1)))) {
		    // Propagate the sign bit...
		  v |= (~static_cast<uint64_t>(0)) << val_size;

	    } অন্যথা {
		    // Fill with zeros.
		  v &= ~((~static_cast<uint64_t>(0)) << val_size);
	    }

      }

      delete[]bits;
      প্রত্যাগম v;
}

/*
 * %ix/vec4 <idx>
 */
তার্কিক of_IX_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned use_idx = cp->সংখ্যা;
      thr->শব্দগুলি[use_idx].w_uint = vec4_to_index(thr, অসত্য);
      প্রত্যাগম সত্য;
}

/*
 * %ix/vec4/s <idx>
 */
তার্কিক of_IX_VEC4_S(vthread_t thr, vvp_code_t cp)
{
      unsigned use_idx = cp->সংখ্যা;
      thr->শব্দগুলি[use_idx].w_uint = vec4_to_index(thr, সত্য);
      প্রত্যাগম সত্য;
}

/*
 * The various JMP instruction work simply by pulling the new program
 * counter from the instruction and resuming. If the jump is
 * conditional, then test the bit for the expected value first.
 */
তার্কিক of_JMP(vthread_t thr, vvp_code_t cp)
{
      thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      যদি (schedule_stopped()) {
	    schedule_vthread(thr, 0, অসত্য);
	    প্রত্যাগম অসত্য;
      }

      প্রত্যাগম সত্য;
}

/*
 * %jmp/0 <pc>, <flag>
 */
তার্কিক of_JMP0(vthread_t thr, vvp_code_t cp)
{
      যদি (thr->flags[cp->bit_idx[0]] == BIT4_0)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      যদি (schedule_stopped()) {
	    schedule_vthread(thr, 0, অসত্য);
	    প্রত্যাগম অসত্য;
      }

      প্রত্যাগম সত্য;
}

/*
 * %jmp/0xz <pc>, <flag>
 */
তার্কিক of_JMP0XZ(vthread_t thr, vvp_code_t cp)
{
      যদি (thr->flags[cp->bit_idx[0]] != BIT4_1)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      যদি (schedule_stopped()) {
	    schedule_vthread(thr, 0, অসত্য);
	    প্রত্যাগম অসত্য;
      }

      প্রত্যাগম সত্য;
}

/*
 * %jmp/1 <pc>, <flag>
 */
তার্কিক of_JMP1(vthread_t thr, vvp_code_t cp)
{
      যদি (thr->flags[cp->bit_idx[0]] == BIT4_1)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      যদি (schedule_stopped()) {
	    schedule_vthread(thr, 0, অসত্য);
	    প্রত্যাগম অসত্য;
      }

      প্রত্যাগম সত্য;
}

/*
 * %jmp/1xz <pc>, <flag>
 */
তার্কিক of_JMP1XZ(vthread_t thr, vvp_code_t cp)
{
      যদি (thr->flags[cp->bit_idx[0]] != BIT4_0)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      যদি (schedule_stopped()) {
	    schedule_vthread(thr, 0, অসত্য);
	    প্রত্যাগম অসত্য;
      }

      প্রত্যাগম সত্য;
}

/*
 * The %join instruction causes the thread to wait for one child
 * to die.  If a child is already dead (and a zombie) then I reap
 * it and go on. Otherwise, I mark myself as waiting in a join so that
 * children know to wake me when they finish.
 */

স্থির শূন্য do_join(vthread_t thr, vthread_t child)
{
      প্রতিজ্ঞা(child->parent == thr);

        /* If the immediate child thread is in an automatic scope... */
      যদি (child->wt_context) {
              /* and is the top level task/function thread... */
            যদি (thr->wt_context != thr->rd_context) {
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

স্থির তার্কিক do_join_opcode(vthread_t thr)
{
      প্রতিজ্ঞা( !thr->i_am_joining );
      প্রতিজ্ঞা( !thr->children.empty());

	// Are there any children that have already ended? If so, then
	// join with that one.
      প্রতি (set<vthread_t>::iterator cur = thr->children.begin()
		 ; cur != thr->children.end() ; ++cur) {
	    vthread_t curp = *cur;
	    যদি (! curp->i_have_ended)
		  অনুবর্ত;

	      // found something!
	    do_join(thr, curp);
	    প্রত্যাগম সত্য;
      }

	// Otherwise, tell my children to awaken me when they end,
	// then pause.
      thr->i_am_joining = 1;
      প্রত্যাগম অসত্য;
}

তার্কিক of_JOIN(vthread_t thr, vvp_code_t)
{
      প্রত্যাগম do_join_opcode(thr);
}

/*
 * This %join/detach <n> instruction causes the thread to detach
 * threads that were created by an earlier %fork.
 */
তার্কিক of_JOIN_DETACH(vthread_t thr, vvp_code_t cp)
{
      unsigned long গুনতি = cp->সংখ্যা;

      প্রতিজ্ঞা(গুনতি == thr->children.size());

      যাবত্ (! thr->children.empty()) {
	    vthread_t child = *thr->children.begin();
	    প্রতিজ্ঞা(child->parent == thr);

	      // We cannot detach automatic tasks/functions within an
	      // automatic scope. If we try to do that, we might make
	      // a mess of the allocation of the context. Note that it
	      // is OK if the child context is distinct (See %exec_ufunc.)
	    প্রতিজ্ঞা(child->wt_context==0 || thr->wt_context!=child->wt_context);
	    যদি (child->i_have_ended) {
		    // If the child has already ended, then reap it.
		  vthread_reap(child);

	    } অন্যথা {
		  size_t res = child->parent->children.erase(child);
		  প্রতিজ্ঞা(res == 1);
		  child->i_am_detached = 1;
		  thr->detached_children.insert(child);
	    }
      }

      প্রত্যাগম সত্য;
}

/*
 * %load/ar <array-label>, <index>;
*/
তার্কিক of_LOAD_AR(vthread_t thr, vvp_code_t cp)
{
      unsigned idx = cp->bit_idx[0];
      দ্বিগুণ word;

	/* The result is 0.0 if the address is undefined. */
      যদি (thr->flags[4] == BIT4_1) {
	    word = 0.0;
      } অন্যথা {
	    unsigned adr = thr->শব্দগুলি[idx].w_int;
	    word = cp->array->get_word_r(adr);
      }

      thr->ঠেলো_real(word);
      প্রত্যাগম সত্য;
}

template <typename ELEM>
স্থির তার্কিক load_dar(vthread_t thr, vvp_code_t cp)
{
      int64_t adr = thr->শব্দগুলি[3].w_int;
      vvp_net_t*net = cp->net;
      প্রতিজ্ঞা(net);

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      প্রতিজ্ঞা(obj);

      vvp_darray*darray = obj->get_object().peek<vvp_darray>();

      ELEM word;
      যদি (darray &&
          (adr >= 0) && (thr->flags[4] == BIT4_0)) // A defined address >= 0
	    darray->get_word(adr, word);
      অন্যথা
	    dq_default(word, obj->size());

      vthread_ঠেলো(thr, word);
      প্রত্যাগম সত্য;
}

/*
 * %load/dar/r <array-label>;
 */
তার্কিক of_LOAD_DAR_R(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম load_dar<দ্বিগুণ>(thr, cp);
}

/*
 * %load/dar/str <array-label>;
 */
তার্কিক of_LOAD_DAR_STR(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম load_dar<string>(thr, cp);
}

/*
 * %load/dar/vec4 <array-label>;
 */
তার্কিক of_LOAD_DAR_VEC4(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম load_dar<vvp_vector4_t>(thr, cp);
}

/*
 * %load/prop/dar/vec4 <pid>, <wid>;
 *   Indexed read of queue/dynamic-array class property; object on stack,
 *   index in words[3] (same as %load/dar/vec4).
 */
তার্কিক of_LOAD_PROP_DAR_VEC4(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->সংখ্যা;
      unsigned wid = cp->bit_idx[0];
      int64_t adr = thr->শব্দগুলি[3].w_int;

      vvp_object_t& শীর্ষ = thr->peek_object();
      vvp_cobject*cobj = শীর্ষ.peek<vvp_cobject>();
      প্রতিজ্ঞা(cobj);

      vvp_object_t pobj;
      cobj->get_object(pid, pobj, 0);
      vvp_darray*darray = pobj.peek<vvp_darray>();

      vvp_vector4_t word;
      যদি (darray && (adr >= 0) && (thr->flags[4] == BIT4_0)) {
	    darray->get_word(adr, word);
      } অন্যথা {
	    dq_default(word, wid);
      }

      thr->ঠেলো_vec4(word);
      প্রত্যাগম সত্য;
}

/*
 * %load/obj <var-label>
 */
তার্কিক of_LOAD_OBJ(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      প্রতিজ্ঞা(fun);

      vvp_object_t val = fun->get_object();
      thr->ঠেলো_object(val);

      প্রত্যাগম সত্য;
}

/*
 * %load/obja <index>
 *    Loads the object from array, using index <index> as the index
 *    value. If flags[4] == 1, the calculation of <index> may have
 *    failed, so push nil.
 */
তার্কিক of_LOAD_OBJA(vthread_t thr, vvp_code_t cp)
{
      unsigned idx = cp->bit_idx[0];
      vvp_object_t word;

	/* The result is 0.0 if the address is undefined. */
      যদি (thr->flags[4] == BIT4_1) {
	    ; // Return nil
      } অন্যথা {
	    unsigned adr = thr->শব্দগুলি[idx].w_int;
	    cp->array->get_word_obj(adr, word);
      }

      thr->ঠেলো_object(word);
      প্রত্যাগম সত্য;
}

/*
 * %load/real <var-label>
 */
তার্কিক of_LOAD_REAL(vthread_t thr, vvp_code_t cp)
{
      __vpiHandle*tmp = cp->handle;
      t_vpi_মান val;

      val.format = vpiRealVal;
      vpi_get_মান(tmp, &val);

      thr->ঠেলো_real(val.value.real);

      প্রত্যাগম সত্য;
}

/*
 * %load/str <var-label>
 */
তার্কিক of_LOAD_STR(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;


      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (net->fun);
      প্রতিজ্ঞা(fun);

      ধ্রুব string&val = fun->get_string();
      thr->ঠেলো_str(val);

      প্রত্যাগম সত্য;
}

তার্কিক of_LOAD_STRA(vthread_t thr, vvp_code_t cp)
{
      unsigned idx = cp->bit_idx[0];
      string word;

      যদি (thr->flags[4] == BIT4_1) {
	    word = "";
      } অন্যথা {
	    unsigned adr = thr->শব্দগুলি[idx].w_int;
	    word = cp->array->get_word_str(adr);
      }

      thr->ঠেলো_str(word);
      প্রত্যাগম সত্য;
}


/*
 * %load/vec4 <net>
 */
তার্কিক of_LOAD_VEC4(vthread_t thr, vvp_code_t cp)
{
	// Push a placeholder onto the stack in order to reserve the
	// stack space. Use a reference for the stack top as a target
	// for the load.
      thr->ঠেলো_vec4(vvp_vector4_t());
      vvp_vector4_t&sig_মান = thr->peek_vec4();

      vvp_net_t*net = cp->net;

	// For the %load to work, the functor must actually be a
	// signal functor. Only signals save their vector value.
      vvp_signal_মান*sig = dynamic_cast<vvp_signal_মান*> (net->fil);
      যদি (sig == 0) {
	    cerr << thr->get_fileline()
	         << "%load/v error: Net arg not a signal? "
		 << (net->fil ? typeid(*net->fil).name() :
	                        typeid(*net->fun).name())
	         << endl;
	    প্রতিজ্ঞা(sig);
	    প্রত্যাগম সত্য;
      }

	// Extract the value from the signal and directly into the
	// target stack position.
      sig->vec4_মান(sig_মান);

      প্রত্যাগম সত্য;
}

/*
 * %load/vec4a <arr>, <adrx>
 */
তার্কিক of_LOAD_VEC4A(vthread_t thr, vvp_code_t cp)
{
      পূর্ণাঙ্ক adr_index = cp->bit_idx[0];

      long adr = thr->শব্দগুলি[adr_index].w_int;

	// If flag[3] is set, then the calculation of the address
	// failed, and this load should return X instead of the actual
	// value.
      যদি (thr->flags[4] == BIT4_1) {
	    vvp_vector4_t tmp (cp->array->get_word_size(), BIT4_X);
	    thr->ঠেলো_vec4(tmp);
	    প্রত্যাগম সত্য;
      }

      vvp_vector4_t tmp (cp->array->get_word(adr));
      thr->ঠেলো_vec4(tmp);
      প্রত্যাগম সত্য;
}

স্থির শূন্য do_verylong_mod(vvp_vector4_t&vala, ধ্রুব vvp_vector4_t&valb,
			    তার্কিক বাম_is_neg, তার্কিক ডান_is_neg)
{
      তার্কিক out_is_neg = বাম_is_neg;
      ধ্রুব পূর্ণাঙ্ক len=vala.size();
      unsigned বর্ণ *a, *z, *t;
      a = নূতন unsigned বর্ণ[len+1];
      z = নূতন unsigned বর্ণ[len+1];
      t = নূতন unsigned বর্ণ[len+1];

      unsigned বর্ণ carry;
      unsigned বর্ণ temp;

      পূর্ণাঙ্ক mxa = -1, mxz = -1;
      পূর্ণাঙ্ক i;
      পূর্ণাঙ্ক current, copylen;

      unsigned lb_carry = বাম_is_neg? 1 : 0;
      unsigned rb_carry = ডান_is_neg? 1 : 0;
      প্রতি (পূর্ণাঙ্ক idx = 0 ;  idx < len ;  idx += 1) {
	    unsigned lb = vala.value(idx);
	    unsigned rb = valb.value(idx);

	    যদি ((lb | rb) & 2) {
		  delete []t;
		  delete []z;
		  delete []a;
		  vvp_vector4_t tmp(len, BIT4_X);
		  vala = tmp;
		  প্রত্যাগম;
	    }

	    যদি (বাম_is_neg) {
		  lb = (1-lb) + lb_carry;
		  lb_carry = (lb & ~1)? 1 : 0;
		  lb &= 1;
	    }
	    যদি (ডান_is_neg) {
		  rb = (1-rb) + rb_carry;
		  rb_carry = (rb & ~1)? 1 : 0;
		  rb &= 1;
	    }

	    z[idx]=lb;
	    a[idx]=1-rb;	// for 2s complement add..
      }

      z[len]=0;
      a[len]=1;

      প্রতি(i=len-1;i>=0;i--) {
	    যদি(! a[i]) {
		  mxa=i;
		  ভঙ্গ;
	    }
      }

      প্রতি(i=len-1;i>=0;i--) {
	    যদি(z[i]) {
		  mxz=i;
		  ভঙ্গ;
	    }
      }

      যদি((mxa>mxz)||(mxa==-1)) {
	    যদি(mxa==-1) {
		  delete []t;
		  delete []z;
		  delete []a;
		  vvp_vector4_t tmpx (len, BIT4_X);
		  vala = tmpx;
		  প্রত্যাগম;
	    }

	    গচ্ছ tally;
      }

      copylen = mxa + 2;
      current = mxz - mxa;

      যাবত্(current > -1) {
	    carry = 1;
	    প্রতি(i=0;i<copylen;i++) {
		  temp = z[i+current] + a[i] + carry;
		  t[i] = (temp&1);
		  carry = (temp>>1);
	    }

	    যদি(carry) {
		  প্রতি(i=0;i<copylen;i++) {
			z[i+current] = t[i];
		  }
	    }

	    current--;
      }

 tally:

      vvp_vector4_t tmp (len, BIT4_X);
      carry = out_is_neg? 1 : 0;
      প্রতি (পূর্ণাঙ্ক idx = 0 ;  idx < len ;  idx += 1) {
	    unsigned ob = z[idx];
	    যদি (out_is_neg) {
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

তার্কিক of_MAX_WR(vthread_t thr, vvp_code_t)
{
      দ্বিগুণ r = thr->বের_করো_real();
      দ্বিগুণ l = thr->বের_করো_real();
      যদি (r != r)
	    thr->ঠেলো_real(l);
      অন্যথা যদি (l != l)
	    thr->ঠেলো_real(r);
      অন্যথা যদি (r < l)
	    thr->ঠেলো_real(l);
      অন্যথা
	    thr->ঠেলো_real(r);
      প্রত্যাগম সত্য;
}

তার্কিক of_MIN_WR(vthread_t thr, vvp_code_t)
{
      দ্বিগুণ r = thr->বের_করো_real();
      দ্বিগুণ l = thr->বের_করো_real();
      যদি (r != r)
	    thr->ঠেলো_real(l);
      অন্যথা যদি (l != l)
	    thr->ঠেলো_real(r);
      অন্যথা যদি (r < l)
	    thr->ঠেলো_real(r);
      অন্যথা
	    thr->ঠেলো_real(l);
      প্রত্যাগম সত্য;
}

তার্কিক of_MOD(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valb = thr->বের_করো_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      প্রতিজ্ঞা(vala.size()==valb.size());
      unsigned wid = vala.size();

      যদি(wid <= 8*পরিমাণ(unsigned long long)) {
	    unsigned long long lv = 0, rv = 0;

	    প্রতি (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		  unsigned long long lb = vala.value(idx);
		  unsigned long long rb = valb.value(idx);

		  যদি ((lb | rb) & 2)
			গচ্ছ x_out;

		  lv |= (unsigned long long) lb << idx;
		  rv |= (unsigned long long) rb << idx;
	    }

	    যদি (rv == 0)
		  গচ্ছ x_out;

	    lv %= rv;

	    প্রতি (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		  vala.set_bit(idx, (lv&1)?BIT4_1 : BIT4_0);
		  lv >>= 1;
	    }

	    প্রত্যাগম সত্য;

      } অন্যথা {
	    do_verylong_mod(vala, valb, অসত্য, অসত্য);
	    প্রত্যাগম সত্য;
      }

 x_out:
      vala = vvp_vector4_t(wid, BIT4_X);
      প্রত্যাগম সত্য;
}

/*
 * %mod/s
 */
তার্কিক of_MOD_S(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valb = thr->বের_করো_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      প্রতিজ্ঞা(vala.size()==valb.size());
      unsigned wid = vala.size();

	/* Handle the case that we can fit the bits into a long-long
	   variable. We cause use native % to do the work. */
      যদি(wid <= 8*পরিমাণ(long long)) {
	    long long lv = 0, rv = 0;

	    প্রতি (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		  long long lb = vala.value(idx);
		  long long rb = valb.value(idx);

		  যদি ((lb | rb) & 2)
			গচ্ছ x_out;

		  lv |= (long long) lb << idx;
		  rv |= (long long) rb << idx;
	    }

	    যদি (rv == 0)
		  গচ্ছ x_out;

	    যদি ((lv == LLONG_MIN) && (rv == -1))
		  গচ্ছ zero_out;

	      /* Sign extend the signed operands when needed. */
	    যদি (wid < 8*পরিমাণ(long long)) {
		  যদি (lv & (1LL << (wid-1)))
			lv |= -1ULL << wid;
		  যদি (rv & (1LL << (wid-1)))
			rv |= -1ULL << wid;
	    }

	    lv %= rv;

	    প্রতি (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		  vala.set_bit(idx, (lv&1)? BIT4_1 : BIT4_0);
		  lv >>= 1;
	    }

	      // vala is the top of the stack, edited in place, so we
	      // do not need to push the result.

	    প্রত্যাগম সত্য;

      } অন্যথা {

	    তার্কিক বাম_is_neg  = vala.value(vala.size()-1) == BIT4_1;
	    তার্কিক ডান_is_neg = valb.value(valb.size()-1) == BIT4_1;
	    do_verylong_mod(vala, valb, বাম_is_neg, ডান_is_neg);
	    প্রত্যাগম সত্য;
      }

 x_out:
      vala = vvp_vector4_t(wid, BIT4_X);
      প্রত্যাগম সত্য;
 zero_out:
      vala = vvp_vector4_t(wid, BIT4_0);
      প্রত্যাগম সত্য;
}

/*
 * %mod/wr
 */
তার্কিক of_MOD_WR(vthread_t thr, vvp_code_t)
{
      দ্বিগুণ r = thr->বের_করো_real();
      দ্বিগুণ l = thr->বের_করো_real();
      thr->ঠেলো_real(fmod(l,r));

      প্রত্যাগম সত্য;
}

/*
 * %neg/wr
 */
তার্কিক of_NEG_WR(vthread_t thr, vvp_code_t)
{
      thr->poke_real(0, -thr->peek_real(0));
      প্রত্যাগম সত্য;
}

/*
 * %pad/s <wid>
 */
তার্কিক of_PAD_S(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->সংখ্যা;

      vvp_vector4_t&val = thr->peek_vec4();
      unsigned old_size = val.size();

	// Sign-extend.
      যদি (old_size < wid)
	    val.resize(wid, val.value(old_size-1));
      অন্যথা
	    val.resize(wid);

      প্রত্যাগম সত্য;
}

/*
 * %pad/u <wid>
 */
তার্কিক of_PAD_U(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->সংখ্যা;

      vvp_vector4_t&val = thr->peek_vec4();
      val.resize(wid, BIT4_0);

      প্রত্যাগম সত্য;
}

/*
 * %part/s <wid>
 * %part/u <wid>
 * Two values are popped from the stack. First, pop the canonical
 * index of the part select, and second is the value to be
 * selected. The result is pushed back to the stack.
 */
স্থির তার্কিক of_PART_base(vthread_t thr, vvp_code_t cp, তার্কিক signed_flag)
{
      unsigned wid = cp->সংখ্যা;

      vvp_vector4_t base4 = thr->বের_করো_vec4();
      vvp_vector4_t&value = thr->peek_vec4();

      vvp_vector4_t res (wid, BIT4_X);

	// NOTE: This is treating the vector as signed. Is that correct?
      int32_t base;
      তার্কিক মান_ok = vector4_to_মান(base4, base, signed_flag);
      যদি (! মান_ok) {
	    value = res;
	    প্রত্যাগম সত্য;
      }

      যদি (base >= (int32_t)value.size()) {
	    value = res;
	    প্রত্যাগম সত্য;
      }

      যদি ((base+(পূর্ণাঙ্ক)wid) <= 0) {
	    value = res;
	    প্রত্যাগম সত্য;
      }

      long vbase = 0;
      যদি (base < 0) {
	    vbase = -base;
	    wid -= vbase;
	    base = 0;
      }

      যদি ((base+wid) > value.size()) {
	    wid = value.size() - base;
      }

      res .set_vec(vbase, value.subvalue(base, wid));
      value = res;

      প্রত্যাগম সত্য;
}

তার্কিক of_PART_S(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম of_PART_base(thr, cp, সত্য);
}

তার্কিক of_PART_U(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম of_PART_base(thr, cp, অসত্য);
}

/*
 * %parti/s <wid>, <basei>, <base_wid>
 * %parti/u <wid>, <basei>, <base_wid>
 *
 * Pop the value to be selected. The result is pushed back to the stack.
 */
স্থির তার্কিক of_PARTI_base(vthread_t thr, vvp_code_t cp, তার্কিক signed_flag)
{
      unsigned wid = cp->সংখ্যা;
      uint32_t base = cp->bit_idx[0];
      uint32_t bwid = cp->bit_idx[1];

      vvp_vector4_t&value = thr->peek_vec4();

      vvp_vector4_t res (wid, BIT4_X);

	// NOTE: This is treating the vector as signed. Is that correct?
      int32_t use_base = base;
      যদি (signed_flag && bwid < 32 && (base&(1<<(bwid-1)))) {
	    use_base |= -1UL << bwid;
      }

      যদি (use_base >= (int32_t)value.size()) {
	    value = res;
	    প্রত্যাগম সত্য;
      }

      যদি ((use_base+(int32_t)wid) <= 0) {
	    value = res;
	    প্রত্যাগম সত্য;
      }

      long vbase = 0;
      যদি (use_base < 0) {
	    vbase = -use_base;
	    wid -= vbase;
	    use_base = 0;
      }

      যদি ((use_base+wid) > value.size()) {
	    wid = value.size() - use_base;
      }

      res .set_vec(vbase, value.subvalue(use_base, wid));
      value = res;

      প্রত্যাগম সত্য;
}

তার্কিক of_PARTI_S(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম of_PARTI_base(thr, cp, সত্য);
}

তার্কিক of_PARTI_U(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম of_PARTI_base(thr, cp, অসত্য);
}

/*
 * %mul
 */
তার্কিক of_MUL(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t r = thr->বের_করো_vec4();
	// Rather then pop l, use it directly from the stack. When we
	// assign to 'l', that will edit the top of the stack, which
	// replaces a pop and a pull.
      vvp_vector4_t&l = thr->peek_vec4();

      l.mul(r);
      প্রত্যাগম সত্য;
}

/*
 * %muli <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
তার্কিক of_MULI(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->সংখ্যা;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_immediate_rval (cp, r);

      l.mul(r);
      প্রত্যাগম সত্য;
}

তার্কিক of_MUL_WR(vthread_t thr, vvp_code_t)
{
      দ্বিগুণ r = thr->বের_করো_real();
      দ্বিগুণ l = thr->বের_করো_real();
      thr->ঠেলো_real(l * r);

      প্রত্যাগম সত্য;
}

তার্কিক of_NAND(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valr = thr->বের_করো_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      প্রতিজ্ঞা(vall.size() == valr.size());

      vall &= valr;
      vall.invert();

      প্রত্যাগম সত্য;
}

/*
 * %new/cobj <vpi_object>
 * This creates a new cobject (SystemVerilog class object) and pushes
 * it to the stack. The <vpi-object> is a __vpiHandle that is a
 * vpiClassDefn object that defines the item to be created.
 */
তার্কিক of_NEW_COBJ(vthread_t thr, vvp_code_t cp)
{
      ধ্রুব class_type*defn = dynamic_cast<ধ্রুব class_type*> (cp->handle);
      প্রতিজ্ঞা(defn);

      vvp_object_t tmp (নূতন vvp_cobject(defn));
      thr->ঠেলো_object(tmp);
      প্রত্যাগম সত্য;
}

তার্কিক of_NEW_DARRAY(vthread_t thr, vvp_code_t cp)
{
      ধ্রুব বর্ণ*text = cp->text;
      size_t size = thr->শব্দগুলি[cp->bit_idx[0]].w_int;
      unsigned word_wid;
      size_t n;

      vvp_object_t obj;
      যদি (strcmp(text,"b8") == 0) {
	    obj = নূতন vvp_darray_atom<uint8_t>(size);
      } অন্যথা যদি (strcmp(text,"b16") == 0) {
	    obj = নূতন vvp_darray_atom<uint16_t>(size);
      } অন্যথা যদি (strcmp(text,"b32") == 0) {
	    obj = নূতন vvp_darray_atom<uint32_t>(size);
      } অন্যথা যদি (strcmp(text,"b64") == 0) {
	    obj = নূতন vvp_darray_atom<uint64_t>(size);
      } অন্যথা যদি (strcmp(text,"sb8") == 0) {
	    obj = নূতন vvp_darray_atom<int8_t>(size);
      } অন্যথা যদি (strcmp(text,"sb16") == 0) {
	    obj = নূতন vvp_darray_atom<int16_t>(size);
      } অন্যথা যদি (strcmp(text,"sb32") == 0) {
	    obj = নূতন vvp_darray_atom<int32_t>(size);
      } অন্যথা যদি (strcmp(text,"sb64") == 0) {
	    obj = নূতন vvp_darray_atom<int64_t>(size);
      } অন্যথা যদি ((1 == sscanf(text, "b%u%zn", &word_wid, &n)) &&
                 (n == strlen(text))) {
	    obj = নূতন vvp_darray_vec2(size, word_wid);
      } অন্যথা যদি ((1 == sscanf(text, "sb%u%zn", &word_wid, &n)) &&
                 (n == strlen(text))) {
	    obj = নূতন vvp_darray_vec2(size, word_wid);
      } অন্যথা যদি ((1 == sscanf(text, "v%u%zn", &word_wid, &n)) &&
                 (n == strlen(text))) {
	    obj = নূতন vvp_darray_vec4(size, word_wid);
      } অন্যথা যদি ((1 == sscanf(text, "sv%u%zn", &word_wid, &n)) &&
                 (n == strlen(text))) {
	    obj = নূতন vvp_darray_vec4(size, word_wid);
      } অন্যথা যদি (strcmp(text,"r") == 0) {
	    obj = নূতন vvp_darray_real(size);
      } অন্যথা যদি (strcmp(text,"S") == 0) {
	    obj = নূতন vvp_darray_string(size);
      } অন্যথা {
	    cerr << get_fileline()
	         << "Internal error: Unsupported dynamic array type: "
	         << text << "." << endl;
	    প্রতিজ্ঞা(0);
      }

      thr->ঠেলো_object(obj);

      প্রত্যাগম সত্য;
}

তার্কিক of_NOOP(vthread_t, vvp_code_t)
{
      প্রত্যাগম সত্য;
}

/*
 * %nor/r
 */
তার্কিক of_NORR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_or());

      প্রত্যাগম সত্য;
}

/*
 * Push a null to the object stack.
 */
তার্কিক of_NULL(vthread_t thr, vvp_code_t)
{
      vvp_object_t tmp;
      thr->ঠেলো_object(tmp);
      প্রত্যাগম সত্য;
}

/*
 * %and/r
 */
তার্কিক of_ANDR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_and());

      প্রত্যাগম সত্য;
}

/*
 * %nand/r
 */
তার্কিক of_NANDR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_and());

      প্রত্যাগম সত্য;
}

/*
 * %or/r
 */
তার্কিক of_ORR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_or());

      প্রত্যাগম সত্য;
}

/*
 * %xor/r
 */
তার্কিক of_XORR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_xor());

      প্রত্যাগম সত্য;
}

/*
 * %xnor/r
 */
তার্কিক of_XNORR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_xor());

      প্রত্যাগম সত্য;
}

/*
 * %or
 */
তার্কিক of_OR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valb = thr->বের_করো_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();
      vala |= valb;
      প্রত্যাগম সত্য;
}

/*
 * %nor
 */
তার্কিক of_NOR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valr = thr->বের_করো_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      প্রতিজ্ঞা(vall.size() == valr.size());

      vall |= valr;
      vall.invert();

      প্রত্যাগম সত্য;
}

/*
 * %pop/obj <num>, <skip>
 */
তার্কিক of_বের_করো_OBJ(vthread_t thr, vvp_code_t cp)
{
      unsigned cnt = cp->bit_idx[0];
      unsigned skip = cp->bit_idx[1];

      thr->বের_করো_object(cnt, skip);
      প্রত্যাগম সত্য;
}

/*
 * %pop/real <number>
 */
তার্কিক of_বের_করো_REAL(vthread_t thr, vvp_code_t cp)
{
      unsigned cnt = cp->সংখ্যা;
      thr->বের_করো_real(cnt);
      প্রত্যাগম সত্য;
}

/*
 *  %pop/str <number>
 */
তার্কিক of_বের_করো_STR(vthread_t thr, vvp_code_t cp)
{
      unsigned cnt = cp->সংখ্যা;
      thr->বের_করো_str(cnt);
      প্রত্যাগম সত্য;
}

/*
 *  %pop/vec4 <number>
 */
তার্কিক of_বের_করো_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned cnt = cp->সংখ্যা;
      thr->বের_করো_vec4(cnt);
      প্রত্যাগম সত্য;
}

/*
 * %pow
 * %pow/s
 */
স্থির তার্কিক of_POW_base(vthread_t thr, তার্কিক signed_flag)
{
      vvp_vector4_t valb = thr->বের_করো_vec4();
      vvp_vector4_t vala = thr->বের_করো_vec4();

      unsigned wid = vala.size();

      vvp_vector2_t xv2 = vvp_vector2_t(vala, সত্য);
      vvp_vector2_t yv2 = vvp_vector2_t(valb, সত্য);


        /* If we have an X or Z in the arguments return X. */
      যদি (xv2.is_NaN() || yv2.is_NaN()) {
	    vvp_vector4_t tmp (wid, BIT4_X);
	    thr->ঠেলো_vec4(tmp);
	    প্রত্যাগম সত্য;
      }

	// Is the exponent negative? If so, table 5-6 in IEEE1364-2005
	// defines what value is returned.
      যদি (signed_flag && yv2.value(yv2.size()-1)) {
	    পূর্ণাঙ্ক a_val;
	    vvp_bit4_t pad = BIT4_0, lsb = BIT4_0;
	    যদি (vector2_to_মান(xv2, a_val, সত্য)) {
		  যদি (a_val == 0) {
			pad = BIT4_X; lsb = BIT4_X;
		  }
		  যদি (a_val == 1) {
			pad = BIT4_0; lsb = BIT4_1;
		  }
		  যদি (a_val == -1) {
			যদি (yv2.value(0)) {
			      pad = BIT4_1; lsb = BIT4_1;
			} অন্যথা {
			      pad = BIT4_0; lsb = BIT4_1;
			}
		  }
	    }
	    vvp_vector4_t tmp (wid, pad);
	    tmp.set_bit(0, lsb);
	    thr->ঠেলো_vec4(tmp);
	    প্রত্যাগম সত্য;
      }

      vvp_vector2_t result = pow(xv2, yv2);

        /* Copy only what we need of the result. If the result is too
	   small, zero-pad it. */
      প্রতি (unsigned jdx = 0;  jdx < wid;  jdx += 1) {
	    যদি (jdx >= result.size())
		  vala.set_bit(jdx, BIT4_0);
	    অন্যথা
		  vala.set_bit(jdx, result.value(jdx) ? BIT4_1 : BIT4_0);
      }
      thr->ঠেলো_vec4(vala);

      প্রত্যাগম সত্য;
}

তার্কিক of_POW(vthread_t thr, vvp_code_t)
{
      প্রত্যাগম of_POW_base(thr, অসত্য);
}

তার্কিক of_POW_S(vthread_t thr, vvp_code_t)
{
      প্রত্যাগম of_POW_base(thr, সত্য);
}

তার্কিক of_POW_WR(vthread_t thr, vvp_code_t)
{
      দ্বিগুণ r = thr->বের_করো_real();
      দ্বিগুণ l = thr->বের_করো_real();
      thr->ঠেলো_real(pow(l,r));

      প্রত্যাগম সত্য;
}

/*
 * %prop/obj <pid>, <idx>
 *
 * Load an object value from the cobject and push it onto the object stack.
 */
তার্কিক of_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      unsigned pid = cp->সংখ্যা;
      unsigned idx = cp->bit_idx[0];

      যদি (idx != 0) {
	    প্রতিজ্ঞা(idx < vthread_s::শব্দগুলি_গুনতি);
	    idx = thr->শব্দগুলি[idx].w_uint;
      }

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();

      vvp_object_t val;
      cobj->get_object(pid, val, idx);

      thr->ঠেলো_object(val);

      প্রত্যাগম সত্য;
}

স্থির শূন্য get_from_obj(unsigned pid, vvp_cobject*cobj, দ্বিগুণ&val)
{
      val = cobj->get_real(pid);
}

স্থির শূন্য get_from_obj(unsigned pid, vvp_cobject*cobj, string&val)
{
      val = cobj->get_string(pid);
}

স্থির শূন্য get_from_obj(unsigned pid, vvp_cobject*cobj, vvp_vector4_t&val)
{
      cobj->get_vec4(pid, val);
}

template <typename ELEM>
স্থির তার্কিক prop(vthread_t thr, vvp_code_t cp)
{
      unsigned pid = cp->সংখ্যা;

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      প্রতিজ্ঞা(cobj);

      ELEM val;
      get_from_obj(pid, cobj, val);
      vthread_ঠেলো(thr, val);

      প্রত্যাগম সত্য;
}

/*
 * %prop/r <pid>
 *
 * Load a real value from the cobject and push it onto the real value
 * stack.
 */
তার্কিক of_PROP_R(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম prop<দ্বিগুণ>(thr, cp);
}

/*
 * %prop/str <pid>
 *
 * Load a string value from the cobject and push it onto the real value
 * stack.
 */
তার্কিক of_PROP_STR(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম prop<string>(thr, cp);
}

/*
 * %prop/v <pid>
 *
 * Load a property <pid> from the cobject on the top of the stack into
 * the vector space at <base>.
 */
তার্কিক of_PROP_V(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম prop<vvp_vector4_t>(thr, cp);
}

তার্কিক of_PUSHI_REAL(vthread_t thr, vvp_code_t cp)
{
      দ্বিগুণ mant = cp->bit_idx[0];
      uint32_t imant = cp->bit_idx[0];
      পূর্ণাঙ্ক exp = cp->bit_idx[1];

	// Detect +infinity
      যদি (exp==0x3fff && imant==0) {
	    thr->ঠেলো_real(INFINITY);
	    প্রত্যাগম সত্য;
      }
	// Detect -infinity
      যদি (exp==0x7fff && imant==0) {
	    thr->ঠেলো_real(-INFINITY);
	    প্রত্যাগম সত্য;
      }
	// Detect NaN
      যদি (exp==0x3fff) {
	    thr->ঠেলো_real(nan(""));
	    প্রত্যাগম সত্য;
      }

      দ্বিগুণ sign = (exp & 0x4000)? -1.0 : 1.0;

      exp &= 0x1fff;

      mant = sign * ldexp(mant, exp - 0x1000);
      thr->ঠেলো_real(mant);
      প্রত্যাগম সত্য;
}

তার্কিক of_PUSHI_STR(vthread_t thr, vvp_code_t cp)
{
      ধ্রুব বর্ণ*text = cp->text;
      thr->ঠেলো_str(filter_string(text));
      প্রত্যাগম সত্য;
}

/*
 * %pushi/vec4 <vala>, <valb>, <wid>
 */
তার্কিক of_PUSHI_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned wid  = cp->সংখ্যা;

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t val (wid, BIT4_0);
      get_immediate_rval (cp, val);

      thr->ঠেলো_vec4(val);

      প্রত্যাগম সত্য;
}

/*
 * %pushv/str
 *   Pops a vec4 value, and pushes a string.
 */
তার্কিক of_PUSHV_STR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t vec = thr->বের_করো_vec4();

      size_t slen = (vec.size() + 7)/8;
      vector<বর্ণ>buf;
      buf.reserve(slen);

      প্রতি (size_t idx = 0 ; idx < vec.size() ; idx += 8) {
	    বর্ণ tmp = 0;
	    size_t trans = 8;
	    যদি (idx+trans > vec.size())
		  trans = vec.size() - idx;

	    প্রতি (size_t bdx = 0 ; bdx < trans ; bdx += 1) {
		  যদি (vec.value(idx+bdx) == BIT4_1)
			tmp |= 1 << bdx;
	    }

	    যদি (tmp != 0)
		  buf.push_back(tmp);
      }

      string val;
      প্রতি (vector<বর্ণ>::reverse_iterator cur = buf.rbegin()
		 ; cur != buf.rend() ; ++cur) {
	    val.push_back(*cur);
      }

      thr->ঠেলো_str(val);

      প্রত্যাগম সত্য;
}

/*
 * %putc/str/vec4 <var>, <mux>
 */
তার্কিক of_PUTC_STR_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned muxr = cp->bit_idx[0];
      int32_t mux = muxr? thr->শব্দগুলি[muxr].w_int : 0;

      vvp_vector4_t val = thr->বের_করো_vec4();
      প্রতিজ্ঞা(val.size() == 8);

      যদি (mux < 0)
	    প্রত্যাগম সত্য;

	/* Get the existing value of the string. If we find that the
	   index is too big for the string, then give up. */
      vvp_net_t*net = cp->net;
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (net->fun);
      প্রতিজ্ঞা(fun);

      string tmp = fun->get_string();
      যদি (tmp.size() <= (size_t)mux)
	    প্রত্যাগম সত্য;

      বর্ণ val_str = 0;
      প্রতি (size_t idx = 0 ; idx < 8 ; idx += 1) {
	    যদি (val.value(idx)==BIT4_1)
		  val_str |= 1<<idx;
      }

	// It is a quirk of the Verilog standard that putc(..., 'h00)
	// has no effect. Test for that case here.
      যদি (val_str == 0)
	    প্রত্যাগম সত্য;

      tmp[mux] = val_str;

      vvp_send_string(vvp_net_ptr_t(cp->net, 0), tmp, thr->wt_context);
      প্রত্যাগম সত্য;
}

template <typename ELEM, বর্গ QTYPE>
স্থির তার্কিক qinsert(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
      int64_t idx = thr->শব্দগুলি[3].w_int;
      ELEM value;
      vvp_net_t*net = cp->net;
      বের_করো_মান(thr, value, wid); // Pop the value to store.

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      প্রতিজ্ঞা(queue);
      যদি (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at a negative "
	         << get_queue_type(value)
	         << " index (" << idx << "). ";
	    print_queue_মান(value);
	    cerr << " was not added." << endl;
      } অন্যথা যদি (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at an undefined "
	         << get_queue_type(value) << " index. ";
	    print_queue_মান(value);
	    cerr << " was not added." << endl;
      } অন্যথা {
	    unsigned max_size = thr->শব্দগুলি[cp->bit_idx[0]].w_int;
	    queue->insert(idx, value, max_size);
      }
      প্রত্যাগম সত্য;
}

/*
 * %qinsert/real <var-label>
 */
তার্কিক of_QINSERT_REAL(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম qinsert<দ্বিগুণ, vvp_queue_real>(thr, cp);
}

/*
 * %qinsert/str <var-label>
 */
তার্কিক of_QINSERT_STR(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম qinsert<string, vvp_queue_string>(thr, cp);
}

/*
 * %qinsert/v <var-label>
 */
তার্কিক of_QINSERT_V(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম qinsert<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

/*
 * Helper functions used in the queue pop templates
 */
অন্তঃস্থ শূন্য ঠেলো_মান(vthread_t thr, দ্বিগুণ value, unsigned)
{
      thr->ঠেলো_real(value);
}

অন্তঃস্থ শূন্য ঠেলো_মান(vthread_t thr, ধ্রুব string&value, unsigned)
{
      thr->ঠেলো_str(value);
}

অন্তঃস্থ শূন্য ঠেলো_মান(vthread_t thr, ধ্রুব vvp_vector4_t&value, unsigned wid)
{
      প্রতিজ্ঞা(wid == value.size());
      thr->ঠেলো_vec4(value);
}

template <typename ELEM, বর্গ QTYPE>
স্থির তার্কিক q_বের_করো(vthread_t thr, vvp_code_t cp,
                  শূন্য (*get_val_func)(vvp_queue*, ELEM&),
                  ধ্রুব বর্ণ*loc, unsigned wid)
{
      vvp_net_t*net = cp->net;

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      প্রতিজ্ঞা(queue);

      size_t size = queue->get_size();

      ELEM value;
      যদি (size) {
	    get_val_func(queue, value);
      } অন্যথা {
	    dq_default(value, wid);
	    cerr << thr->get_fileline()
	         << "Warning: pop_" << loc << "() on empty "
	         << get_queue_type(value) << "." << endl;
      }

      ঠেলো_মান(thr, value, wid);
      প্রত্যাগম সত্য;
}

template <typename ELEM>
স্থির শূন্য get_back_মান(vvp_queue*queue, ELEM&value)
{
      queue->get_word(queue->get_size()-1, value);
      queue->pop_back();
}

template <typename ELEM, বর্গ QTYPE>
স্থির তার্কিক qpop_b(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
      প্রত্যাগম q_বের_করো<ELEM, QTYPE>(thr, cp, get_back_মান<ELEM>, "back", wid);
}

/*
 * %qpop/b/real <var-label>
 */
তার্কিক of_QPOP_B_REAL(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম qpop_b<দ্বিগুণ, vvp_queue_real>(thr, cp);
}

/*
 * %qpop/b/str <var-label>
 */
তার্কিক of_QPOP_B_STR(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম qpop_b<string, vvp_queue_string>(thr, cp);
}

/*
 * %qpop/b/v <var-label>
 */
তার্কিক of_QPOP_B_V(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম qpop_b<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[0]);
}

template <typename ELEM>
স্থির শূন্য get_front_মান(vvp_queue*queue, ELEM&value)
{
      queue->get_word(0, value);
      queue->pop_front();
}

template <typename ELEM, বর্গ QTYPE>
স্থির তার্কিক qpop_f(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
      প্রত্যাগম q_বের_করো<ELEM, QTYPE>(thr, cp, get_front_মান<ELEM>, "front", wid);
}


/*
 * %qpop/f/real <var-label>
 */
তার্কিক of_QPOP_F_REAL(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম qpop_f<দ্বিগুণ, vvp_queue_real>(thr, cp);
}

/*
 * %qpop/f/str <var-label>
 */
তার্কিক of_QPOP_F_STR(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম qpop_f<string, vvp_queue_string>(thr, cp);
}

/*
 * %qpop/f/v <var-label>
 */
তার্কিক of_QPOP_F_V(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম qpop_f<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[0]);
}

/*
 * These implement the %release/net and %release/reg instructions. The
 * %release/net instruction applies to a net kind of functor by
 * sending the release/net command to the command port. (See vvp_net.h
 * for details.) The %release/reg instruction is the same, but sends
 * the release/reg command instead. These are very similar to the
 * %deassign instruction.
 */
স্থির তার্কিক do_release_vec(vvp_code_t cp, তার্কিক net_flag)
{
      vvp_net_t*net = cp->net;
      unsigned base  = cp->bit_idx[0];
      unsigned width = cp->bit_idx[1];

      প্রতিজ্ঞা(net->fil);

      যদি (base >= net->fil->filter_size()) প্রত্যাগম সত্য;
      যদি (base+width > net->fil->filter_size())
	    width = net->fil->filter_size() - base;

      তার্কিক full_sig = base == 0 && width == net->fil->filter_size();

	// XXXX Can't really do this if this is a partial release?
      net->fil->force_unlink();

	/* Do we release all or part of the net? */
      vvp_net_ptr_t ptr (net, 0);
      যদি (full_sig) {
	    net->fil->release(ptr, net_flag);
      } অন্যথা {
	    net->fil->release_pv(ptr, base, width, net_flag);
      }
      net->fun->force_flag(অসত্য);

      প্রত্যাগম সত্য;
}

তার্কিক of_RELEASE_NET(vthread_t, vvp_code_t cp)
{
      প্রত্যাগম do_release_vec(cp, সত্য);
}


তার্কিক of_RELEASE_REG(vthread_t, vvp_code_t cp)
{
      প্রত্যাগম do_release_vec(cp, অসত্য);
}

/* The type is 1 for registers and 0 for everything else. */
তার্কিক of_RELEASE_WR(vthread_t, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      unsigned type  = cp->bit_idx[0];

      প্রতিজ্ঞা(net->fil);
      net->fil->force_unlink();

	// Send a command to this signal to unforce itself.
      vvp_net_ptr_t ptr (net, 0);
      net->fil->release(ptr, type==0);
      প্রত্যাগম সত্য;
}

তার্কিক of_REPLICATE(vthread_t thr, vvp_code_t cp)
{
      পূর্ণাঙ্ক rept = cp->সংখ্যা;
      vvp_vector4_t val = thr->বের_করো_vec4();
      vvp_vector4_t res (val.size() * rept, BIT4_X);

      প্রতি (পূর্ণাঙ্ক idx = 0 ; idx < rept ; idx += 1) {
	    res.set_vec(idx * val.size(), val);
      }

      thr->ঠেলো_vec4(res);

      প্রত্যাগম সত্য;
}

স্থির শূন্য poke_val(vthread_t fun_thr, unsigned depth, দ্বিগুণ val)
{
      fun_thr->parent->poke_real(depth, val);
}

স্থির শূন্য poke_val(vthread_t fun_thr, unsigned depth, ধ্রুব string&val)
{
      fun_thr->parent->poke_str(depth, val);
}

স্থির size_t get_max(vthread_t fun_thr, দ্বিগুণ&)
{
      প্রত্যাগম fun_thr->args_real.size();
}

স্থির size_t get_max(vthread_t fun_thr, string&)
{
      প্রত্যাগম fun_thr->args_str.size();
}

স্থির size_t get_max(vthread_t fun_thr, vvp_vector4_t&)
{
      প্রত্যাগম fun_thr->args_vec4.size();
}

স্থির unsigned get_depth(vthread_t fun_thr, size_t index, দ্বিগুণ&)
{
      প্রত্যাগম fun_thr->args_real[index];
}

স্থির unsigned get_depth(vthread_t fun_thr, size_t index, string&)
{
      প্রত্যাগম fun_thr->args_str[index];
}

স্থির unsigned get_depth(vthread_t fun_thr, size_t index, vvp_vector4_t&)
{
      প্রত্যাগম fun_thr->args_vec4[index];
}

স্থির vthread_t get_func(vthread_t thr)
{
      vthread_t fun_thr = thr;

      যাবত্ (fun_thr->parent_scope->get_type_code() != vpiFunction) {
	    প্রতিজ্ঞা(fun_thr->parent);
	    fun_thr = fun_thr->parent;
      }

      প্রত্যাগম fun_thr;
}

template <typename ELEM>
স্থির তার্কিক ret(vthread_t thr, vvp_code_t cp)
{
      size_t index = cp->সংখ্যা;
      ELEM val;
      বের_করো_মান(thr, val, 0);

      vthread_t fun_thr = get_func(thr);
      প্রতিজ্ঞা(index < get_max(fun_thr, val));

      unsigned depth = get_depth(fun_thr, index, val);
	// Use the depth to put the value into the stack of
	// the parent thread.
      poke_val(fun_thr, depth, val);
      প্রত্যাগম সত্য;
}

/*
 * %ret/real <index>
 */
তার্কিক of_RET_REAL(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম ret<দ্বিগুণ>(thr, cp);
}

/*
 * %ret/str <index>
 */
তার্কিক of_RET_STR(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম ret<string>(thr, cp);
}

/*
 * %ret/vec4 <index>, <offset>, <wid>
 */
তার্কিক of_RET_VEC4(vthread_t thr, vvp_code_t cp)
{
      size_t index = cp->সংখ্যা;
      unsigned off_index = cp->bit_idx[0];
      unsigned পূর্ণাঙ্ক wid = cp->bit_idx[1];
      vvp_vector4_t&val = thr->peek_vec4();

      vthread_t fun_thr = get_func(thr);
      প্রতিজ্ঞা(index < get_max(fun_thr, val));
      প্রতিজ্ঞা(val.size() == wid);
      unsigned depth = get_depth(fun_thr, index, val);

      int64_t off = off_index ? thr->শব্দগুলি[off_index].w_int : 0;
      unsigned পূর্ণাঙ্ক sig_মান_size = fun_thr->parent->peek_vec4(depth).size();

      যদি (off_index!=0 && thr->flags[4] == BIT4_1) {
	    thr->বের_করো_vec4(1);
	    প্রত্যাগম সত্য;
      }

      যদি (!resize_rval_vec(val, off, sig_মান_size)) {
	    thr->বের_করো_vec4(1);
	    প্রত্যাগম সত্য;
      }

      যদি (off == 0 && val.size() == sig_মান_size) {
	    fun_thr->parent->poke_vec4(depth, val);
      } অন্যথা {
	    vvp_vector4_t tmp_dst = fun_thr->parent->peek_vec4(depth);
	    tmp_dst.set_vec(off, val);
	    fun_thr->parent->poke_vec4(depth, tmp_dst);
      }

      thr->বের_করো_vec4(1);
      প্রত্যাগম সত্য;
}

স্থির শূন্য ঠেলো_from_parent(vthread_t thr, vthread_t fun_thr, unsigned depth, দ্বিগুণ&)
{
      thr->ঠেলো_real(fun_thr->parent->peek_real(depth));
}

স্থির শূন্য ঠেলো_from_parent(vthread_t thr, vthread_t fun_thr, unsigned depth, string&)
{
      thr->ঠেলো_str(fun_thr->parent->peek_str(depth));
}

স্থির শূন্য ঠেলো_from_parent(vthread_t thr, vthread_t fun_thr, unsigned depth, vvp_vector4_t&)
{
      thr->ঠেলো_vec4(fun_thr->parent->peek_vec4(depth));
}

template <typename ELEM>
স্থির তার্কিক retload(vthread_t thr, vvp_code_t cp)
{
      size_t index = cp->সংখ্যা;
      ELEM type;

      vthread_t fun_thr = get_func(thr);
      প্রতিজ্ঞা(index < get_max(fun_thr, type));

      unsigned depth = get_depth(fun_thr, index, type);
	// Use the depth to extract the values from the stack
	// of the parent thread.
      ঠেলো_from_parent(thr, fun_thr, depth, type);
      প্রত্যাগম সত্য;
}

/*
 * %retload/real <index>
 */
তার্কিক of_RETLOAD_REAL(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম retload<দ্বিগুণ>(thr, cp);
}

/*
 * %retload/str <index>
 */
তার্কিক of_RETLOAD_STR(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম retload<string>(thr, cp);
}

/*
 * %retload/vec4 <index>
 */
তার্কিক of_RETLOAD_VEC4(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম retload<vvp_vector4_t>(thr, cp);
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
তার্কিক of_SCOPY(vthread_t thr, vvp_code_t)
{
      vvp_object_t tmp;
      thr->বের_করো_object(tmp);

      vvp_object_t&dest = thr->peek_object();
        // If it is null there is nothing to copy
      যদি (!tmp.test_nil())
	    dest.shallow_copy(tmp);

      প্রত্যাগম সত্য;
}

স্থির শূন্য thread_peek(vthread_t thr, দ্বিগুণ&value)
{
      value = thr->peek_real(0);
}

স্থির শূন্য thread_peek(vthread_t thr, string&value)
{
      value = thr->peek_str(0);
}

স্থির শূন্য thread_peek(vthread_t thr, vvp_vector4_t&value)
{
      value = thr->peek_vec4(0);
}

template <typename ELEM>
স্থির তার্কিক set_dar_obj(vthread_t thr, vvp_code_t cp)
{
      unsigned adr = thr->শব্দগুলি[cp->সংখ্যা].w_int;

      ELEM value;
      thread_peek(thr, value);

      vvp_object_t&শীর্ষ = thr->peek_object();
      vvp_darray*darray = শীর্ষ.peek<vvp_darray>();
      প্রতিজ্ঞা(darray);

      darray->set_word(adr, value);
      প্রত্যাগম সত্য;
}

/*
 * %set/dar/obj/real <index>
 */
তার্কিক of_SET_DAR_OBJ_REAL(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম set_dar_obj<দ্বিগুণ>(thr, cp);
}

/*
 * %set/dar/obj/str <index>
 */
তার্কিক of_SET_DAR_OBJ_STR(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম set_dar_obj<string>(thr, cp);
}

/*
 * %set/dar/obj/vec4 <index>
 */
তার্কিক of_SET_DAR_OBJ_VEC4(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম set_dar_obj<vvp_vector4_t>(thr, cp);
}

/*
 * %shiftl <idx>
 *
 * Pop the operand, then push the result.
 */
তার্কিক of_SHIFTL(vthread_t thr, vvp_code_t cp)
{
      পূর্ণাঙ্ক use_index = cp->সংখ্যা;
      uint64_t shift = thr->শব্দগুলি[use_index].w_uint;

      vvp_vector4_t&val = thr->peek_vec4();
      unsigned wid  = val.size();

      যদি (thr->flags[4] == BIT4_1) {
	      // The result is 'bx if the shift amount is undefined
	    val = vvp_vector4_t(wid, BIT4_X);

      } অন্যথা যদি (thr->flags[4] == BIT4_X || shift >= wid) {
	      // Shift is so big that all value is shifted out. Write
	      // a constant 0 result.
	    val = vvp_vector4_t(wid, BIT4_0);

      } অন্যথা যদি (shift > 0) {
	    vvp_vector4_t blk = val.subvalue(0, wid-shift);
	    vvp_vector4_t tmp (shift, BIT4_0);
	    val.set_vec(0, tmp);
	    val.set_vec(shift, blk);
      }

      প্রত্যাগম সত্য;
}

/*
 * %shiftr <idx>
 * This is an unsigned right shift. The <idx> is a number that selects
 * the index register with the amount of the shift. This instruction
 * checks flag bit 4, which will be true if the shift is invalid.
 */
তার্কিক of_SHIFTR(vthread_t thr, vvp_code_t cp)
{
      পূর্ণাঙ্ক use_index = cp->সংখ্যা;
      uint64_t shift = thr->শব্দগুলি[use_index].w_uint;

      vvp_vector4_t val = thr->বের_করো_vec4();
      unsigned wid  = val.size();

      যদি (thr->flags[4] == BIT4_1) {
	    val = vvp_vector4_t(wid, BIT4_X);

      } অন্যথা যদি (thr->flags[4] == BIT4_X || shift > wid) {
	    val = vvp_vector4_t(wid, BIT4_0);

      } অন্যথা যদি (shift > 0) {
	    vvp_vector4_t blk = val.subvalue(shift, wid-shift);
	    vvp_vector4_t tmp (shift, BIT4_0);
	    val.set_vec(0, blk);
	    val.set_vec(wid-shift, tmp);
      }

      thr->ঠেলো_vec4(val);
      প্রত্যাগম সত্য;
}

/*
 *  %shiftr/s <wid>
 */
তার্কিক of_SHIFTR_S(vthread_t thr, vvp_code_t cp)
{
      পূর্ণাঙ্ক use_index = cp->সংখ্যা;
      uint64_t shift = thr->শব্দগুলি[use_index].w_uint;

      vvp_vector4_t val = thr->বের_করো_vec4();
      unsigned wid  = val.size();

      vvp_bit4_t sign_bit = val.value(val.size()-1);

      যদি (thr->flags[4] == BIT4_1) {
	    val = vvp_vector4_t(wid, BIT4_X);

      } অন্যথা যদি (thr->flags[4] == BIT4_X || shift > wid) {
	    val = vvp_vector4_t(wid, sign_bit);

      } অন্যথা যদি (shift > 0) {
	    vvp_vector4_t blk = val.subvalue(shift, wid-shift);
	    vvp_vector4_t tmp (shift, sign_bit);
	    val.set_vec(0, blk);
	    val.set_vec(wid-shift, tmp);
      }

      thr->ঠেলো_vec4(val);
      প্রত্যাগম সত্য;
}

/*
 * %split/vec4 <wid>
 *   Pop 1 value,
 *   Take <wid> bits from the lsb,
 *   Push the remaining msb,
 *   Push the lsb.
 */
তার্কিক of_SPLIT_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned lsb_wid = cp->সংখ্যা;

      vvp_vector4_t&val = thr->peek_vec4();
      প্রতিজ্ঞা(lsb_wid < val.size());

      vvp_vector4_t lsb = val.subvalue(0, lsb_wid);
      val = val.subvalue(lsb_wid, val.size()-lsb_wid);

      thr->ঠেলো_vec4(lsb);
      প্রত্যাগম সত্য;
}

/*
 * The following are used to allow the darray templates to print correctly.
 */
অন্তঃস্থ স্থির string get_darray_type(দ্বিগুণ&)
{
      প্রত্যাগম "darray<real>";
}

অন্তঃস্থ স্থির string get_darray_type(string&)
{
      প্রত্যাগম "darray<string>";
}

অন্তঃস্থ স্থির string get_darray_type(ধ্রুব vvp_vector4_t&value)
{
      ostringstream buf;
      buf << "darray<vector[" << value.size() << "]>";
      string res = buf.str();
      প্রত্যাগম res;
}

/*
 * The following are used to allow a common template to be written for
 * darray real/string/vec4 operations
 */
অন্তঃস্থ স্থির শূন্য dar_বের_করো_মান(vthread_t thr, দ্বিগুণ&value)
{
      value = thr->বের_করো_real();
}

অন্তঃস্থ স্থির শূন্য dar_বের_করো_মান(vthread_t thr, string&value)
{
      value = thr->বের_করো_str();
}

অন্তঃস্থ স্থির শূন্য dar_বের_করো_মান(vthread_t thr, vvp_vector4_t&value)
{
      value = thr->বের_করো_vec4();
}

template <typename ELEM>
স্থির তার্কিক store_dar(vthread_t thr, vvp_code_t cp)
{
      int64_t adr = thr->শব্দগুলি[3].w_int;
      ELEM value;
	// FIXME: Can we get the size of the underlying array element
	//        and then use the normal pop_value?
      dar_বের_করো_মান(thr, value);

      vvp_net_t*net = cp->net;
      প্রতিজ্ঞা(net);

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      প্রতিজ্ঞা(obj);

      vvp_darray*darray = obj->get_object().peek<vvp_darray>();

      যদি (adr < 0)
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to a negative " << get_darray_type(value)
	         << " index (" << adr << ")." << endl;
      অন্যথা যদি (thr->flags[4] != BIT4_0)
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to an undefined " << get_darray_type(value)
	         << " index." << endl;
      অন্যথা যদি (darray)
	    darray->set_word(adr, value);
      অন্যথা
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to an undefined " << get_darray_type(value)
	         << "." << endl;

      প্রত্যাগম সত্য;
}

/*
 * %store/dar/real <var>
 */
তার্কিক of_STORE_DAR_R(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম store_dar<দ্বিগুণ>(thr, cp);
}

/*
 * %store/dar/str <var>
 */
তার্কিক of_STORE_DAR_STR(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম store_dar<string>(thr, cp);
}

/*
 * %store/dar/vec4 <var>
 */
তার্কিক of_STORE_DAR_VEC4(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম store_dar<vvp_vector4_t>(thr, cp);
}

তার্কিক of_STORE_OBJ(vthread_t thr, vvp_code_t cp)
{
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);

      vvp_object_t val;
      thr->বের_করো_object(val);

      vvp_send_object(ptr, val, thr->wt_context);

      প্রত্যাগম সত্য;
}

/*
 * %store/obja <array-label> <index>
 */
তার্কিক of_STORE_OBJA(vthread_t thr, vvp_code_t cp)
{
      unsigned idx = cp->bit_idx[0];
      unsigned adr = thr->শব্দগুলি[idx].w_int;

      vvp_object_t val;
      thr->বের_করো_object(val);

      cp->array->set_word(adr, val);

      প্রত্যাগম সত্য;
}


/*
 * %store/prop/obj <pid>, <idx>
 *
 * Pop an object value from the object stack, and store the value into
 * the property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
তার্কিক of_STORE_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->সংখ্যা;
      unsigned idx = cp->bit_idx[0];

      যদি (idx != 0) {
	    প্রতিজ্ঞা(idx < vthread_s::শব্দগুলি_গুনতি);
	    idx = thr->শব্দগুলি[idx].w_uint;
      }

      vvp_object_t val;
      thr->বের_করো_object(val);

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      প্রতিজ্ঞা(cobj);

      cobj->set_object(pid, val, idx);

      প্রত্যাগম সত্য;
}

স্থির শূন্য বের_করো_prop_val(vthread_t thr, দ্বিগুণ&val, unsigned)
{
      val = thr->বের_করো_real();
}

স্থির শূন্য বের_করো_prop_val(vthread_t thr, string&val, unsigned)
{
      val = thr->বের_করো_str();
}

স্থির শূন্য বের_করো_prop_val(vthread_t thr, vvp_vector4_t&val, unsigned wid)
{
      val = thr->বের_করো_vec4();
      প্রতিজ্ঞা(val.size() >= wid);
      val.resize(wid);
}

স্থির শূন্য set_val(vvp_cobject*cobj, size_t pid, ধ্রুব দ্বিগুণ&val)
{
      cobj->set_real(pid, val);
}

স্থির শূন্য set_val(vvp_cobject*cobj, size_t pid, ধ্রুব string&val)
{
      cobj->set_string(pid, val);
}

স্থির শূন্য set_val(vvp_cobject*cobj, size_t pid, ধ্রুব vvp_vector4_t&val)
{
      cobj->set_vec4(pid, val);
}

template <typename ELEM>
স্থির তার্কিক store_prop(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
      size_t pid = cp->সংখ্যা;
      ELEM val;
      বের_করো_prop_val(thr, val, wid); // Pop the value to store.

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      প্রতিজ্ঞা(cobj);

      set_val(cobj, pid, val);

      প্রত্যাগম সত্য;
}

/*
 * %store/prop/r <id>
 *
 * Pop a real value from the real stack, and store the value into the
 * property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
তার্কিক of_STORE_PROP_R(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম store_prop<দ্বিগুণ>(thr, cp);
}

/*
 * %store/prop/str <id>
 *
 * Pop a string value from the string stack, and store the value into
 * the property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
তার্কিক of_STORE_PROP_STR(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম store_prop<string>(thr, cp);
}

/*
 * %store/prop/v <pid>, <wid>
 *
 * Store vector value into property <id> of cobject in the top of the
 * stack. Do NOT pop the object stack.
 */
তার্কিক of_STORE_PROP_V(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম store_prop<vvp_vector4_t>(thr, cp, cp->bit_idx[0]);
}

template <বর্গ QTYPE>
স্থির QTYPE* get_queue_prop(vvp_cobject*cobj, size_t pid)
{
      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      QTYPE* queue = qobj.peek<QTYPE>();
      যদি (queue == 0) {
	    queue = নূতন QTYPE;
	    vvp_object_t val(queue);
	    cobj->set_object(pid, val, 0);
      }
      প্রত্যাগম queue;
}

template <typename ELEM, বর্গ QTYPE>
স্থির তার্কিক store_prop_qb(vthread_t thr, vvp_code_t cp, unsigned wid)
{
      size_t pid = cp->সংখ্যা;
      ELEM value;
      unsigned max_size = thr->শব্দগুলি[cp->bit_idx[0]].w_uint;
      বের_করো_মান(thr, value, wid);

      vvp_object_t& শীর্ষ = thr->peek_object();
      vvp_cobject*cobj = শীর্ষ.peek<vvp_cobject>();
      প্রতিজ্ঞা(cobj);

      QTYPE* queue = get_queue_prop<QTYPE>(cobj, pid);
      প্রতিজ্ঞা(queue);
      queue->push_back(value, max_size);
      প্রত্যাগম সত্য;
}

template <typename ELEM, বর্গ QTYPE>
স্থির তার্কিক store_prop_qf(vthread_t thr, vvp_code_t cp, unsigned wid)
{
      size_t pid = cp->সংখ্যা;
      ELEM value;
      unsigned max_size = thr->শব্দগুলি[cp->bit_idx[0]].w_uint;
      বের_করো_মান(thr, value, wid);

      vvp_object_t& শীর্ষ = thr->peek_object();
      vvp_cobject*cobj = শীর্ষ.peek<vvp_cobject>();
      প্রতিজ্ঞা(cobj);

      QTYPE* queue = get_queue_prop<QTYPE>(cobj, pid);
      প্রতিজ্ঞা(queue);
      queue->push_front(value, max_size);
      প্রত্যাগম সত্য;
}

তার্কিক of_STORE_PROP_QB_R(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম store_prop_qb<দ্বিগুণ, vvp_queue_real>(thr, cp, 0);
}

তার্কিক of_STORE_PROP_QB_STR(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম store_prop_qb<string, vvp_queue_string>(thr, cp, 0);
}

তার্কিক of_STORE_PROP_QB_V(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম store_prop_qb<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

তার্কিক of_STORE_PROP_QF_R(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম store_prop_qf<দ্বিগুণ, vvp_queue_real>(thr, cp, 0);
}

তার্কিক of_STORE_PROP_QF_STR(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম store_prop_qf<string, vvp_queue_string>(thr, cp, 0);
}

তার্কিক of_STORE_PROP_QF_V(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম store_prop_qf<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

template <typename ELEM, বর্গ QTYPE>
স্থির তার্কিক qinsert_prop(vthread_t thr, vvp_code_t cp, unsigned wid)
{
      int64_t idx = thr->শব্দগুলি[3].w_int;
      ELEM value;
      size_t pid = cp->সংখ্যা;
      বের_করো_মান(thr, value, wid);

      vvp_object_t& শীর্ষ = thr->peek_object();
      vvp_cobject*cobj = শীর্ষ.peek<vvp_cobject>();
      প্রতিজ্ঞা(cobj);

      QTYPE* queue = get_queue_prop<QTYPE>(cobj, pid);
      প্রতিজ্ঞা(queue);
      যদি (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at a negative "
	         << get_queue_type(value)
	         << " index (" << idx << "). ";
	    print_queue_মান(value);
	    cerr << " was not added." << endl;
      } অন্যথা যদি (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at an undefined "
	         << get_queue_type(value) << " index. ";
	    print_queue_মান(value);
	    cerr << " was not added." << endl;
      } অন্যথা {
	    unsigned max_size = thr->শব্দগুলি[cp->bit_idx[0]].w_int;
	    queue->insert(idx, value, max_size);
      }
      প্রত্যাগম সত্য;
}

তার্কিক of_QINSERT_PROP_R(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম qinsert_prop<দ্বিগুণ, vvp_queue_real>(thr, cp, 0);
}

তার্কিক of_QINSERT_PROP_STR(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম qinsert_prop<string, vvp_queue_string>(thr, cp, 0);
}

তার্কিক of_QINSERT_PROP_V(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম qinsert_prop<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

template <typename ELEM, বর্গ QTYPE>
স্থির তার্কিক q_বের_করো_prop(vthread_t thr, vvp_code_t cp,
                       শূন্য (*get_val_func)(vvp_queue*, ELEM&),
                       ধ্রুব বর্ণ*loc, unsigned wid)
{
      size_t pid = cp->সংখ্যা;

      vvp_object_t& শীর্ষ = thr->peek_object();
      vvp_cobject*cobj = শীর্ষ.peek<vvp_cobject>();
      প্রতিজ্ঞা(cobj);

      QTYPE* queue = get_queue_prop<QTYPE>(cobj, pid);
      প্রতিজ্ঞা(queue);

      size_t size = queue->get_size();

      ELEM value;
      যদি (size) {
	    get_val_func(queue, value);
      } অন্যথা {
	    dq_default(value, wid);
	    cerr << thr->get_fileline()
	         << "Warning: pop_" << loc << "() on empty "
	         << get_queue_type(value) << "." << endl;
      }

      ঠেলো_মান(thr, value, wid);
      প্রত্যাগম সত্য;
}

template <typename ELEM, বর্গ QTYPE>
স্থির তার্কিক qpop_b_prop(vthread_t thr, vvp_code_t cp, unsigned wid)
{
      প্রত্যাগম q_বের_করো_prop<ELEM, QTYPE>(thr, cp, get_back_মান<ELEM>, "back", wid);
}

template <typename ELEM, বর্গ QTYPE>
স্থির তার্কিক qpop_f_prop(vthread_t thr, vvp_code_t cp, unsigned wid)
{
      প্রত্যাগম q_বের_করো_prop<ELEM, QTYPE>(thr, cp, get_front_মান<ELEM>, "front", wid);
}

তার্কিক of_QPOP_PROP_B_REAL(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম qpop_b_prop<দ্বিগুণ, vvp_queue_real>(thr, cp, 0);
}

তার্কিক of_QPOP_PROP_B_STR(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম qpop_b_prop<string, vvp_queue_string>(thr, cp, 0);
}

তার্কিক of_QPOP_PROP_B_V(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম qpop_b_prop<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[0]);
}

তার্কিক of_QPOP_PROP_F_REAL(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম qpop_f_prop<দ্বিগুণ, vvp_queue_real>(thr, cp, 0);
}

তার্কিক of_QPOP_PROP_F_STR(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম qpop_f_prop<string, vvp_queue_string>(thr, cp, 0);
}

তার্কিক of_QPOP_PROP_F_V(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম qpop_f_prop<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[0]);
}

স্থির vvp_vector4_t queue_unique_ulong_to_vec4(unsigned long ul, unsigned bits)
{
      vvp_vector4_t val(bits, BIT4_0);
      প্রতি (unsigned idx = 0; idx < bits && idx < 8 * পরিমাণ(unsigned long); idx += 1) {
	    যদি (ul & 1UL) {
		  val.set_bit(idx, BIT4_1);
	    } অন্যথা {
		  val.set_bit(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      প্রত্যাগম val;
}

template <বর্গ SRC> স্থির vvp_queue_vec4* queue_run_unique_src(SRC* src,
								 unsigned wid,
								 তার্কিক as_index)
{
      vvp_queue_vec4* dst = নূতন vvp_queue_vec4();
      যদি (!src) প্রত্যাগম dst;
      size_t n = src->get_size();
      প্রতি (size_t i = 0; i < n; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    তার্কিক seen = অসত্য;
	    প্রতি (size_t j = 0; j < i; j += 1) {
		  vvp_vector4_t vj(wid);
		  src->get_word(j, vj);
		  যদি (vi.eeq(vj)) {
			seen = সত্য;
			ভঙ্গ;
		  }
	    }
	    যদি (seen) অনুবর্ত;
	    যদি (as_index) {
		  dst->push_back(queue_unique_ulong_to_vec4((unsigned long)i, 32), 0);
	    } অন্যথা {
		  dst->push_back(vi, 0);
	    }
      }
      প্রত্যাগম dst;
}

template <বর্গ SRC>
স্থির vvp_queue_vec4* queue_run_find_matches_src(SRC* src, unsigned wid,
						  ধ্রুব vvp_vector4_t& cmp,
						  তার্কিক as_index)
{
      vvp_queue_vec4* dst = নূতন vvp_queue_vec4();
      যদি (!src) প্রত্যাগম dst;
      size_t n = src->get_size();
      প্রতি (size_t i = 0; i < n; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    যদি (vi.eeq(cmp)) {
		  যদি (as_index) {
			dst->push_back(queue_unique_ulong_to_vec4((unsigned long)i, 32), 0);
		  } অন্যথা {
			dst->push_back(vi, 0);
		  }
	    }
      }
      প্রত্যাগম dst;
}

/* Array/queue locator methods return a queue with 0 or 1 elements (LRM). */
template <বর্গ SRC>
স্থির vvp_queue_vec4* queue_find_first_last_to_queue_src(SRC* src,
							  unsigned wid,
							  ধ্রুব vvp_vector4_t& cmp,
							  তার্কিক want_first,
							  তার্কিক want_index)
{
      vvp_queue_vec4* dst = নূতন vvp_queue_vec4();
      যদি (!src || src->get_size() == 0) প্রত্যাগম dst;
      size_t n = src->get_size();
      যদি (want_first) {
	    প্রতি (size_t i = 0; i < n; i += 1) {
		  vvp_vector4_t vi(wid);
		  src->get_word(i, vi);
		  যদি (vi.eeq(cmp)) {
			যদি (want_index) {
			      dst->push_back(
				  queue_unique_ulong_to_vec4((unsigned long)i, 32),
				  0);
			} অন্যথা {
			      dst->push_back(vi, 0);
			}
			প্রত্যাগম dst;
		  }
	    }
      } অন্যথা {
	    প্রতি (size_t i = n; i > 0; ) {
		  i -= 1;
		  vvp_vector4_t vi(wid);
		  src->get_word(i, vi);
		  যদি (vi.eeq(cmp)) {
			যদি (want_index) {
			      dst->push_back(
				  queue_unique_ulong_to_vec4((unsigned long)i, 32),
				  0);
			} অন্যথা {
			      dst->push_back(vi, 0);
			}
			প্রত্যাগম dst;
		  }
	    }
      }
      প্রত্যাগম dst;
}

/* min()/max() return one element with the selected extremum. */
template <বর্গ SRC>
স্থির vvp_queue_vec4* queue_run_min_max_src(SRC* src, unsigned wid, তার্কিক want_max)
{
      vvp_queue_vec4* dst = নূতন vvp_queue_vec4();
      যদি (!src || src->get_size() == 0) প্রত্যাগম dst;

      vvp_vector4_t best(wid);
      src->get_word(0, best);

      size_t n = src->get_size();
      প্রতি (size_t i = 1 ; i < n ; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);

	    vvp_bit4_t ge = compare_gtge(vi, best, BIT4_1);
	    তার্কিক replace = want_max ? (ge == BIT4_1) : (ge == BIT4_0);
	    যদি (replace) best = vi;
      }

      dst->push_back(best, 0);
      প্রত্যাগম dst;
}

তার্কিক of_QUEUE_FIND_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->বের_করো_vec4();
      unsigned wid = cp->bit_idx[0];
      প্রতিজ্ঞা(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      যদি (qsrc) dst = queue_run_find_matches_src(qsrc, wid, cmp, অসত্য);
      অন্যথা যদি (dsrc) dst = queue_run_find_matches_src(dsrc, wid, cmp, অসত্য);
      অন্যথা {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_run_find_matches_src(src, wid, cmp, অসত্য);
      }
      thr->ঠেলো_object(vvp_object_t(dst));
      প্রত্যাগম সত্য;
}

তার্কিক of_QUEUE_FIND_PROP_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->বের_করো_vec4();
      size_t pid = cp->সংখ্যা;
      unsigned wid = cp->bit_idx[0];
      প্রতিজ্ঞা(cmp.size() == wid);

      vvp_object_t& শীর্ষ = thr->peek_object();
      vvp_cobject*cobj = শীর্ষ.peek<vvp_cobject>();
      প্রতিজ্ঞা(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_find_matches_src(qsrc, wid, cmp, অসত্য)
				 : queue_run_find_matches_src(dsrc, wid, cmp, অসত্য);
      thr->ঠেলো_object(vvp_object_t(dst));
      প্রত্যাগম সত্য;
}

তার্কিক of_QUEUE_FIND_INDEX_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->বের_করো_vec4();
      unsigned wid = cp->bit_idx[0];
      প্রতিজ্ঞা(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      যদি (qsrc) dst = queue_run_find_matches_src(qsrc, wid, cmp, সত্য);
      অন্যথা যদি (dsrc) dst = queue_run_find_matches_src(dsrc, wid, cmp, সত্য);
      অন্যথা {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_run_find_matches_src(src, wid, cmp, সত্য);
      }
      thr->ঠেলো_object(vvp_object_t(dst));
      প্রত্যাগম সত্য;
}

তার্কিক of_QUEUE_FIND_INDEX_PROP_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->বের_করো_vec4();
      size_t pid = cp->সংখ্যা;
      unsigned wid = cp->bit_idx[0];
      প্রতিজ্ঞা(cmp.size() == wid);

      vvp_object_t& শীর্ষ = thr->peek_object();
      vvp_cobject*cobj = শীর্ষ.peek<vvp_cobject>();
      প্রতিজ্ঞা(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_find_matches_src(qsrc, wid, cmp, সত্য)
				 : queue_run_find_matches_src(dsrc, wid, cmp, সত্য);
      thr->ঠেলো_object(vvp_object_t(dst));
      প্রত্যাগম সত্য;
}

তার্কিক of_QUEUE_FIND_FIRST_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->বের_করো_vec4();
      unsigned wid = cp->bit_idx[0];
      প্রতিজ্ঞা(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      যদি (qsrc) dst = queue_find_first_last_to_queue_src(qsrc, wid, cmp, সত্য, অসত্য);
      অন্যথা যদি (dsrc) dst = queue_find_first_last_to_queue_src(dsrc, wid, cmp, সত্য, অসত্য);
      অন্যথা {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_find_first_last_to_queue_src(src, wid, cmp, সত্য, অসত্য);
      }
      thr->ঠেলো_object(vvp_object_t(dst));
      প্রত্যাগম সত্য;
}

তার্কিক of_QUEUE_FIND_FIRST_PROP_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->বের_করো_vec4();
      size_t pid = cp->সংখ্যা;
      unsigned wid = cp->bit_idx[0];
      প্রতিজ্ঞা(cmp.size() == wid);

      vvp_object_t& শীর্ষ = thr->peek_object();
      vvp_cobject*cobj = শীর্ষ.peek<vvp_cobject>();
      প্রতিজ্ঞা(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst =
	    qsrc ? queue_find_first_last_to_queue_src(qsrc, wid, cmp, সত্য, অসত্য)
		 : queue_find_first_last_to_queue_src(dsrc, wid, cmp, সত্য, অসত্য);
      thr->ঠেলো_object(vvp_object_t(dst));
      প্রত্যাগম সত্য;
}

তার্কিক of_QUEUE_FIND_FIRST_INDEX_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->বের_করো_vec4();
      unsigned wid = cp->bit_idx[0];
      প্রতিজ্ঞা(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      যদি (qsrc) dst = queue_find_first_last_to_queue_src(qsrc, wid, cmp, সত্য, সত্য);
      অন্যথা যদি (dsrc) dst = queue_find_first_last_to_queue_src(dsrc, wid, cmp, সত্য, সত্য);
      অন্যথা {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_find_first_last_to_queue_src(src, wid, cmp, সত্য, সত্য);
      }
      thr->ঠেলো_object(vvp_object_t(dst));
      প্রত্যাগম সত্য;
}

তার্কিক of_QUEUE_FIND_FIRST_INDEX_PROP_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->বের_করো_vec4();
      size_t pid = cp->সংখ্যা;
      unsigned wid = cp->bit_idx[0];
      প্রতিজ্ঞা(cmp.size() == wid);

      vvp_object_t& শীর্ষ = thr->peek_object();
      vvp_cobject*cobj = শীর্ষ.peek<vvp_cobject>();
      প্রতিজ্ঞা(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst =
	    qsrc ? queue_find_first_last_to_queue_src(qsrc, wid, cmp, সত্য, সত্য)
		 : queue_find_first_last_to_queue_src(dsrc, wid, cmp, সত্য, সত্য);
      thr->ঠেলো_object(vvp_object_t(dst));
      প্রত্যাগম সত্য;
}

তার্কিক of_QUEUE_FIND_LAST_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->বের_করো_vec4();
      unsigned wid = cp->bit_idx[0];
      প্রতিজ্ঞা(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      যদি (qsrc) dst = queue_find_first_last_to_queue_src(qsrc, wid, cmp, অসত্য, অসত্য);
      অন্যথা যদি (dsrc) dst = queue_find_first_last_to_queue_src(dsrc, wid, cmp, অসত্য, অসত্য);
      অন্যথা {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_find_first_last_to_queue_src(src, wid, cmp, অসত্য, অসত্য);
      }
      thr->ঠেলো_object(vvp_object_t(dst));
      প্রত্যাগম সত্য;
}

তার্কিক of_QUEUE_FIND_LAST_PROP_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->বের_করো_vec4();
      size_t pid = cp->সংখ্যা;
      unsigned wid = cp->bit_idx[0];
      প্রতিজ্ঞা(cmp.size() == wid);

      vvp_object_t& শীর্ষ = thr->peek_object();
      vvp_cobject*cobj = শীর্ষ.peek<vvp_cobject>();
      প্রতিজ্ঞা(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst =
	    qsrc ? queue_find_first_last_to_queue_src(qsrc, wid, cmp, অসত্য, অসত্য)
		 : queue_find_first_last_to_queue_src(dsrc, wid, cmp, অসত্য, অসত্য);
      thr->ঠেলো_object(vvp_object_t(dst));
      প্রত্যাগম সত্য;
}

তার্কিক of_QUEUE_FIND_LAST_INDEX_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->বের_করো_vec4();
      unsigned wid = cp->bit_idx[0];
      প্রতিজ্ঞা(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      যদি (qsrc) dst = queue_find_first_last_to_queue_src(qsrc, wid, cmp, অসত্য, সত্য);
      অন্যথা যদি (dsrc) dst = queue_find_first_last_to_queue_src(dsrc, wid, cmp, অসত্য, সত্য);
      অন্যথা {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_find_first_last_to_queue_src(src, wid, cmp, অসত্য, সত্য);
      }
      thr->ঠেলো_object(vvp_object_t(dst));
      প্রত্যাগম সত্য;
}

তার্কিক of_QUEUE_FIND_LAST_INDEX_PROP_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->বের_করো_vec4();
      size_t pid = cp->সংখ্যা;
      unsigned wid = cp->bit_idx[0];
      প্রতিজ্ঞা(cmp.size() == wid);

      vvp_object_t& শীর্ষ = thr->peek_object();
      vvp_cobject*cobj = শীর্ষ.peek<vvp_cobject>();
      প্রতিজ্ঞা(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst =
	    qsrc ? queue_find_first_last_to_queue_src(qsrc, wid, cmp, অসত্য, সত্য)
		 : queue_find_first_last_to_queue_src(dsrc, wid, cmp, অসত্য, সত্য);
      thr->ঠেলো_object(vvp_object_t(dst));
      প্রত্যাগম সত্য;
}

তার্কিক of_QUEUE_MIN_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      যদি (qsrc) dst = queue_run_min_max_src(qsrc, wid, অসত্য);
      অন্যথা যদি (dsrc) dst = queue_run_min_max_src(dsrc, wid, অসত্য);
      অন্যথা {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_run_min_max_src(src, wid, অসত্য);
      }
      thr->ঠেলো_object(vvp_object_t(dst));
      প্রত্যাগম সত্য;
}

তার্কিক of_QUEUE_MIN_OBJ_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->বের_করো_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_min_max_src(qsrc, wid, অসত্য)
				 : queue_run_min_max_src(dsrc, wid, অসত্য);
      thr->ঠেলো_object(vvp_object_t(dst));
      প্রত্যাগম সত্য;
}

তার্কিক of_QUEUE_MIN_PROP_V(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->সংখ্যা;
      unsigned wid = cp->bit_idx[0];

      vvp_object_t& শীর্ষ = thr->peek_object();
      vvp_cobject*cobj = শীর্ষ.peek<vvp_cobject>();
      প্রতিজ্ঞা(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_min_max_src(qsrc, wid, অসত্য)
				 : queue_run_min_max_src(dsrc, wid, অসত্য);
      thr->ঠেলো_object(vvp_object_t(dst));
      প্রত্যাগম সত্য;
}

তার্কিক of_QUEUE_MAX_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      যদি (qsrc) dst = queue_run_min_max_src(qsrc, wid, সত্য);
      অন্যথা যদি (dsrc) dst = queue_run_min_max_src(dsrc, wid, সত্য);
      অন্যথা {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_run_min_max_src(src, wid, সত্য);
      }
      thr->ঠেলো_object(vvp_object_t(dst));
      প্রত্যাগম সত্য;
}

তার্কিক of_QUEUE_MAX_OBJ_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->বের_করো_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_min_max_src(qsrc, wid, সত্য)
				 : queue_run_min_max_src(dsrc, wid, সত্য);
      thr->ঠেলো_object(vvp_object_t(dst));
      প্রত্যাগম সত্য;
}

তার্কিক of_QUEUE_MAX_PROP_V(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->সংখ্যা;
      unsigned wid = cp->bit_idx[0];

      vvp_object_t& শীর্ষ = thr->peek_object();
      vvp_cobject*cobj = শীর্ষ.peek<vvp_cobject>();
      প্রতিজ্ঞা(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_min_max_src(qsrc, wid, সত্য)
				 : queue_run_min_max_src(dsrc, wid, সত্য);
      thr->ঠেলো_object(vvp_object_t(dst));
      প্রত্যাগম সত্য;
}

তার্কিক of_QUEUE_UNIQUE_OBJ_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->বের_করো_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_unique_src(qsrc, wid, অসত্য)
				 : queue_run_unique_src(dsrc, wid, অসত্য);
      thr->ঠেলো_object(vvp_object_t(dst));
      প্রত্যাগম সত্য;
}

তার্কিক of_QUEUE_UNIQUE_INDEX_OBJ_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->বের_করো_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_unique_src(qsrc, wid, সত্য)
				 : queue_run_unique_src(dsrc, wid, সত্য);
      thr->ঠেলো_object(vvp_object_t(dst));
      প্রত্যাগম সত্য;
}

তার্কিক of_QUEUE_UNIQUE_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      যদি (qsrc) dst = queue_run_unique_src(qsrc, wid, অসত্য);
      অন্যথা যদি (dsrc) dst = queue_run_unique_src(dsrc, wid, অসত্য);
      অন্যথা {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_run_unique_src(src, wid, অসত্য);
      }
      thr->ঠেলো_object(vvp_object_t(dst));
      প্রত্যাগম সত্য;
}

তার্কিক of_QUEUE_UNIQUE_PROP_V(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->সংখ্যা;
      unsigned wid = cp->bit_idx[0];

      vvp_object_t& শীর্ষ = thr->peek_object();
      vvp_cobject*cobj = শীর্ষ.peek<vvp_cobject>();
      প্রতিজ্ঞা(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_unique_src(qsrc, wid, অসত্য)
				 : queue_run_unique_src(dsrc, wid, অসত্য);
      thr->ঠেলো_object(vvp_object_t(dst));
      প্রত্যাগম সত্য;
}

তার্কিক of_QUEUE_UNIQUE_INDEX_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      যদি (qsrc) dst = queue_run_unique_src(qsrc, wid, সত্য);
      অন্যথা যদি (dsrc) dst = queue_run_unique_src(dsrc, wid, সত্য);
      অন্যথা {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_run_unique_src(src, wid, সত্য);
      }
      thr->ঠেলো_object(vvp_object_t(dst));
      প্রত্যাগম সত্য;
}

তার্কিক of_QUEUE_UNIQUE_INDEX_PROP_V(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->সংখ্যা;
      unsigned wid = cp->bit_idx[0];

      vvp_object_t& শীর্ষ = thr->peek_object();
      vvp_cobject*cobj = শীর্ষ.peek<vvp_cobject>();
      প্রতিজ্ঞা(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_unique_src(qsrc, wid, সত্য)
				 : queue_run_unique_src(dsrc, wid, সত্য);
      thr->ঠেলো_object(vvp_object_t(dst));
      প্রত্যাগম সত্য;
}

তার্কিক of_PROP_QUEUE_SIZE(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->সংখ্যা;

      vvp_object_t& শীর্ষ = thr->peek_object();
      vvp_cobject*cobj = শীর্ষ.peek<vvp_cobject>();
      প্রতিজ্ঞা(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);

      size_t sz = qsrc ? qsrc->get_size() : (dsrc ? dsrc->get_size() : 0);

      vvp_vector4_t val(32, BIT4_0);
      unsigned long ul = sz;
      প্রতি (unsigned idx = 0; idx < 32; idx++) {
	    যদি (ul & 1UL) {
		  val.set_bit(idx, BIT4_1);
	    } অন্যথা {
		  val.set_bit(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      thr->ঠেলো_vec4(val);
      প্রত্যাগম সত্য;
}

তার্কিক of_QUEUE_SIZE_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      size_t sz =
	    qsrc ? qsrc->get_size() : (dsrc ? dsrc->get_size() : 0);

      vvp_vector4_t val(32, BIT4_0);
      unsigned long ul = sz;
      প্রতি (unsigned idx = 0; idx < 32; idx++) {
	    যদি (ul & 1UL) {
		  val.set_bit(idx, BIT4_1);
	    } অন্যথা {
		  val.set_bit(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      thr->ঠেলো_vec4(val);
      প্রত্যাগম সত্য;
}

তার্কিক of_QUEUE_WORD_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      unsigned ix_reg = cp->bit_idx[1];
      int64_t adr = thr->শব্দগুলি[ix_reg].w_int;
      যদি (thr->flags[4] == BIT4_1) {
	    thr->ঠেলো_vec4(vvp_vector4_t(wid));
	    প্রত্যাগম সত্য;
      }
      যদি (adr < 0) {
	    thr->ঠেলো_vec4(vvp_vector4_t(wid));
	    প্রত্যাগম সত্য;
      }
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t out(wid);
      যদি (qsrc) {
	    যদি ((size_t)adr >= qsrc->get_size()) {
		  thr->ঠেলো_vec4(out);
		  প্রত্যাগম সত্য;
	    }
	    qsrc->get_word((unsigned)adr, out);
      } অন্যথা যদি (dsrc) {
	    যদি ((size_t)adr >= dsrc->get_size()) {
		  thr->ঠেলো_vec4(out);
		  প্রত্যাগম সত্য;
	    }
	    dsrc->get_word((unsigned)adr, out);
      } অন্যথা {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    যদি (!src || (size_t)adr >= src->get_size()) {
		  thr->ঠেলো_vec4(out);
		  প্রত্যাগম সত্য;
	    }
	    src->get_word((unsigned)adr, out);
      }
      thr->ঠেলো_vec4(out);
      প্রত্যাগম সত্য;
}

তার্কিক of_QUEUE_WORD_PROP_V(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->সংখ্যা;
      unsigned wid = cp->bit_idx[0];
      unsigned ix_reg = cp->bit_idx[1];
      int64_t adr = thr->শব্দগুলি[ix_reg].w_int;
      যদি (thr->flags[4] == BIT4_1) {
	    thr->ঠেলো_vec4(vvp_vector4_t(wid));
	    প্রত্যাগম সত্য;
      }
      যদি (adr < 0) {
	    thr->ঠেলো_vec4(vvp_vector4_t(wid));
	    প্রত্যাগম সত্য;
      }

      vvp_object_t saved_শীর্ষ;
      তার্কিক popped_শীর্ষ = অসত্য;
      vvp_object_t& শীর্ষ = thr->peek_object();
      vvp_cobject*cobj = শীর্ষ.peek<vvp_cobject>();
      যদি (cobj == 0) {
	    // Array-locator with(property) keeps an accumulator queue on top.
	    // Temporarily pop it to access the class object beneath.
	    thr->বের_করো_object(saved_শীর্ষ);
	    popped_শীর্ষ = সত্য;
	    vvp_object_t& below = thr->peek_object();
	    cobj = below.peek<vvp_cobject>();
      }
      যদি (cobj == 0) {
	    যদি (popped_শীর্ষ) thr->ঠেলো_object(saved_শীর্ষ);
	    thr->ঠেলো_vec4(vvp_vector4_t(wid));
	    প্রত্যাগম সত্য;
      }

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t out(wid);
      যদি (qsrc) {
	    যদি ((size_t)adr >= qsrc->get_size()) {
		  thr->ঠেলো_vec4(out);
		  প্রত্যাগম সত্য;
	    }
	    qsrc->get_word((unsigned)adr, out);
      } অন্যথা যদি (dsrc) {
	    যদি ((size_t)adr >= dsrc->get_size()) {
		  thr->ঠেলো_vec4(out);
		  প্রত্যাগম সত্য;
	    }
	    dsrc->get_word((unsigned)adr, out);
      } অন্যথা {
	    thr->ঠেলো_vec4(out);
	    যদি (popped_শীর্ষ) thr->ঠেলো_object(saved_শীর্ষ);
	    প্রত্যাগম সত্য;
      }
      যদি (popped_শীর্ষ) thr->ঠেলো_object(saved_শীর্ষ);
      thr->ঠেলো_vec4(out);
      প্রত্যাগম সত্য;
}

তার্কিক of_CMPIX_LTU(vthread_t thr, vvp_code_t cp)
{
      unsigned a = cp->সংখ্যা;
      unsigned b = cp->bit_idx[0];
      যদি (thr->শব্দগুলি[a].w_uint < thr->শব্দগুলি[b].w_uint) {
	    thr->flags[4] = BIT4_1;
      } অন্যথা {
	    thr->flags[4] = BIT4_0;
      }
      প্রত্যাগম সত্য;
}

তার্কিক of_CMPIX_SLT0(vthread_t thr, vvp_code_t cp)
{
      unsigned r = cp->সংখ্যা;
      thr->flags[4] = (thr->শব্দগুলি[r].w_int < 0) ? BIT4_1 : BIT4_0;
      প্রত্যাগম সত্য;
}

তার্কিক of_ঠেলো_IX_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned ix = cp->সংখ্যা;
      unsigned wid = cp->bit_idx[0];
      unsigned sgn = cp->bit_idx[1];
      uint32_t ubits;
      যদি (sgn) {
	    int32_t sv = (int32_t)thr->শব্দগুলি[ix].w_int;
	    ubits = (uint32_t)sv;
      } অন্যথা {
	    ubits = (uint32_t)thr->শব্দগুলি[ix].w_uint;
      }
      যদি (wid > 32) wid = 32;
      vvp_vector4_t tmp(wid, BIT4_0);
      প্রতি (unsigned idx = 0; idx < wid; idx++) {
	    যদি (ubits & 1u) {
		  tmp.set_bit(idx, BIT4_1);
	    } অন্যথা {
		  tmp.set_bit(idx, BIT4_0);
	    }
	    ubits >>= 1;
      }
      thr->ঠেলো_vec4(tmp);
      প্রত্যাগম সত্য;
}

তার্কিক of_QUEUE_NEW_EMPTY_V(vthread_t thr, vvp_code_t)
{
      thr->ঠেলো_object(vvp_object_t(নূতন vvp_queue_vec4()));
      প্রত্যাগম সত্য;
}

তার্কিক of_QUEUE_APPEND_WORD_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      vvp_vector4_t elem = thr->বের_করো_vec4();
      প্রতিজ্ঞা(elem.size() == wid);
      vvp_object_t qobj;
      thr->বের_করো_object(qobj);
      vvp_queue_vec4* q = qobj.peek<vvp_queue_vec4>();
      প্রতিজ্ঞা(q);
      q->push_back(elem, 0);
      thr->ঠেলো_object(qobj);
      প্রত্যাগম সত্য;
}

তার্কিক of_DELETE_PROP_ELEM(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->সংখ্যা;

      int64_t idx_val = thr->শব্দগুলি[3].w_int;
      যদি (thr->flags[4] == BIT4_1) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with undefined index."
	         << endl;
	    প্রত্যাগম সত্য;
      }
      যদি (idx_val < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with negative index."
	         << endl;
	    প্রত্যাগম সত্য;
      }
      size_t idx = idx_val;

      vvp_object_t& শীর্ষ = thr->peek_object();
      vvp_cobject*cobj = শীর্ষ.peek<vvp_cobject>();
      প্রতিজ্ঞা(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue* queue = qobj.peek<vvp_queue>();
      যদি (queue == 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping delete(" << idx
	         << ") on empty queue." << endl;
      } অন্যথা {
	    size_t size = queue->get_size();
	    যদি (idx >= size) {
		  cerr << thr->get_fileline()
		       << "Warning: skipping out of range delete(" << idx
		       << ") on queue of size " << size << "." << endl;
	    } অন্যথা {
		  queue->erase(idx);
	    }
      }

      প্রত্যাগম সত্য;
}

তার্কিক of_DELETE_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->সংখ্যা;

      vvp_object_t& শীর্ষ = thr->peek_object();
      vvp_cobject*cobj = শীর্ষ.peek<vvp_cobject>();
      প্রতিজ্ঞা(cobj);

      cobj->set_object(pid, vvp_object_t(), 0);

      প্রত্যাগম সত্য;
}

template <typename ELEM, বর্গ QTYPE>
স্থির তার্কিক store_qb(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
      ELEM value;
      vvp_net_t*net = cp->net;
      unsigned max_size = thr->শব্দগুলি[cp->bit_idx[0]].w_int;
      বের_করো_মান(thr, value, wid); // Pop the value to store.

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      প্রতিজ্ঞা(queue);
      queue->push_back(value, max_size);
      প্রত্যাগম সত্য;
}

/*
 * %store/qb/r <var-label>, <max-idx>
 */
তার্কিক of_STORE_QB_R(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম store_qb<দ্বিগুণ, vvp_queue_real>(thr, cp);
}

/*
 * %store/qb/str <var-label>, <max-idx>
 */
তার্কিক of_STORE_QB_STR(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম store_qb<string, vvp_queue_string>(thr, cp);
}

/*
 * %store/qb/v <var-label>, <max-idx>, <wid>
 */
তার্কিক of_STORE_QB_V(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম store_qb<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

template <typename ELEM, বর্গ QTYPE>
স্থির তার্কিক store_qdar(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
      int64_t idx = thr->শব্দগুলি[3].w_int;
      ELEM value;
      vvp_net_t*net = cp->net;
      বের_করো_মান(thr, value, wid); // Pop the value to store.

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      প্রতিজ্ঞা(queue);
      যদি (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot assign to a negative "
	         << get_queue_type(value)
	         << " index (" << idx << "). ";
	    print_queue_মান(value);
	    cerr << " was not added." << endl;
      } অন্যথা যদি (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot assign to an undefined "
	         << get_queue_type(value) << " index. ";
	    print_queue_মান(value);
	    cerr << " was not added." << endl;
      } অন্যথা {
	    unsigned max_size = thr->শব্দগুলি[cp->bit_idx[0]].w_int;
	    queue->set_word_max(idx, value, max_size);
      }
      প্রত্যাগম সত্য;
}

/*
 * %store/qdar/r <var>, idx
 */
তার্কিক of_STORE_QDAR_R(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম store_qdar<দ্বিগুণ, vvp_queue_real>(thr, cp);
}

/*
 * %store/qdar/str <var>, idx
 */
তার্কিক of_STORE_QDAR_STR(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম store_qdar<string, vvp_queue_string>(thr, cp);
}

/*
 * %store/qdar/v <var>, idx
 */
তার্কিক of_STORE_QDAR_V(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম store_qdar<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

template <typename ELEM, বর্গ QTYPE>
স্থির তার্কিক store_qf(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
      ELEM value;
      vvp_net_t*net = cp->net;
      unsigned max_size = thr->শব্দগুলি[cp->bit_idx[0]].w_int;
      বের_করো_মান(thr, value, wid); // Pop the value to store.

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      প্রতিজ্ঞা(queue);
      queue->push_front(value, max_size);
      প্রত্যাগম সত্য;
}
/*
 * %store/qf/r <var-label>, <max-idx>
 */
তার্কিক of_STORE_QF_R(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম store_qf<দ্বিগুণ, vvp_queue_real>(thr, cp);
}

/*
 * %store/qf/str <var-label>, <max-idx>
 */
তার্কিক of_STORE_QF_STR(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম store_qf<string, vvp_queue_string>(thr, cp);
}

/*
 * %store/qb/v <var-label>, <max-idx>, <wid>
 */
তার্কিক of_STORE_QF_V(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম store_qf<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

template <typename ELEM, বর্গ QTYPE>
স্থির তার্কিক store_qobj(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
// FIXME: Can we actually use wid here?
      (শূন্য)wid;
      vvp_net_t*net = cp->net;

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      প্রতিজ্ঞা(queue);

      vvp_object_t src;
      thr->বের_করো_object(src);

        // If it is null just clear the queue
      যদি (src.test_nil())
	    queue->erase_tail(0);
      অন্যথা {
	    unsigned max_size = thr->শব্দগুলি[cp->bit_idx[0]].w_int;
	    queue->copy_elems(src, max_size);
      }

      প্রত্যাগম সত্য;
}

তার্কিক of_STORE_QOBJ_R(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম store_qobj<দ্বিগুণ, vvp_queue_real>(thr, cp);
}

তার্কিক of_STORE_QOBJ_STR(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম store_qobj<string, vvp_queue_string>(thr, cp);
}

তার্কিক of_STORE_QOBJ_V(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম store_qobj<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

স্থির শূন্য vvp_send(vthread_t thr, vvp_net_ptr_t ptr, ধ্রুব দ্বিগুণ&val)
{
      vvp_send_real(ptr, val, thr->wt_context);
}

স্থির শূন্য vvp_send(vthread_t thr, vvp_net_ptr_t ptr, ধ্রুব string&val)
{
      vvp_send_string(ptr, val, thr->wt_context);
}

template <typename ELEM>
স্থির তার্কিক store(vthread_t thr, vvp_code_t cp)
{
      ELEM val;
      বের_করো_মান(thr, val, 0);
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_send(thr, ptr, val);
      প্রত্যাগম সত্য;
}

তার্কিক of_STORE_REAL(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম store<দ্বিগুণ>(thr, cp);
}

template <typename ELEM>
স্থির তার্কিক storea(vthread_t thr, vvp_code_t cp)
{
      unsigned idx = cp->bit_idx[0];
      ELEM val;
      বের_করো_মান(thr, val, 0);

      যদি (thr->flags[4] != BIT4_1) {
	    unsigned adr = thr->শব্দগুলি[idx].w_int;
	    cp->array->set_word(adr, val);
      }

      প্রত্যাগম সত্য;
}

/*
 * %store/reala <var-label> <index>
 */
তার্কিক of_STORE_REALA(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম storea<দ্বিগুণ>(thr, cp);
}

তার্কিক of_STORE_STR(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম store<string>(thr, cp);
}

/*
 * %store/stra <array-label> <index>
 */
তার্কিক of_STORE_STRA(vthread_t thr, vvp_code_t cp)
{
      প্রত্যাগম storea<string>(thr, cp);
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
তার্কিক of_STORE_VEC4(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr(cp->net, 0);
      ধ্রুব vvp_signal_মান*sig = dynamic_cast<vvp_signal_মান*> (cp->net->fil);
      unsigned off_index = cp->bit_idx[0];
      unsigned পূর্ণাঙ্ক wid = cp->bit_idx[1];

      int64_t off = off_index ? thr->শব্দগুলি[off_index].w_int : 0;
      unsigned পূর্ণাঙ্ক sig_মান_size = sig->মান_size();

      vvp_vector4_t&val = thr->peek_vec4();
      unsigned val_size = val.size();

      যদি (val_size < wid) {
	    cerr << thr->get_fileline()
	         << "XXXX Internal error: val.size()=" << val_size
		 << ", expecting >= " << wid << endl;
      }
      প্রতিজ্ঞা(val_size >= wid);
      যদি (val_size > wid) {
	    val.resize(wid);
      }

	// If there is a problem loading the index register, flags-4
	// will be set to 1, and we know here to skip the actual assignment.
      যদি (off_index!=0 && thr->flags[4] == BIT4_1) {
	    thr->বের_করো_vec4(1);
	    প্রত্যাগম সত্য;
      }

      যদি (!resize_rval_vec(val, off, sig_মান_size)) {
	    thr->বের_করো_vec4(1);
	    প্রত্যাগম সত্য;
      }

      যদি (off == 0 && val.size() == sig_মান_size)
	    vvp_send_vec4(ptr, val, thr->wt_context);
      অন্যথা
	    vvp_send_vec4_pv(ptr, val, off, sig_মান_size, thr->wt_context);

      thr->বের_করো_vec4(1);
      প্রত্যাগম সত্য;
}

/*
 * %store/vec4a <var-label>, <addr>, <offset>
 */
তার্কিক of_STORE_VEC4A(vthread_t thr, vvp_code_t cp)
{
      unsigned adr_index = cp->bit_idx[0];
      unsigned off_index = cp->bit_idx[1];

      long adr = adr_index? thr->শব্দগুলি[adr_index].w_int : 0;
      int64_t off = off_index ? thr->শব্দগুলি[off_index].w_int : 0;

	// Suppress action if flags-4 is true.
      যদি (thr->flags[4] == BIT4_1) {
	    thr->বের_করো_vec4(1);
	    প্রত্যাগম সত্য;
      }

      vvp_vector4_t &value = thr->peek_vec4();

      যদি (!resize_rval_vec(value, off, cp->array->get_word_size())) {
	    thr->বের_করো_vec4(1);
	    প্রত্যাগম সত্য;
      }

      cp->array->set_word(adr, off, value);

      thr->বের_করো_vec4(1);
      প্রত্যাগম সত্য;
}

/*
 * %sub
 *   pop r;
 *   pop l;
 *   push l-r;
 */
তার্কিক of_SUB(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t r = thr->বের_করো_vec4();
      vvp_vector4_t&l = thr->peek_vec4();

      l.sub(r);
      প্রত্যাগম সত্য;
}

/*
 * %subi <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
তার্কিক of_SUBI(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->সংখ্যা;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_immediate_rval (cp, r);

      l.sub(r);

      প্রত্যাগম সত্য;

}

তার্কিক of_SUB_WR(vthread_t thr, vvp_code_t)
{
      দ্বিগুণ r = thr->বের_করো_real();
      দ্বিগুণ l = thr->বের_করো_real();
      thr->ঠেলো_real(l - r);
      প্রত্যাগম সত্য;
}

/*
 * %substr <first>, <last>
 * Pop a string, take the substring (SystemVerilog style), and return
 * the result to the stack. This opcode actually works by editing the
 * string in place.
 */
তার্কিক of_SUBSTR(vthread_t thr, vvp_code_t cp)
{
      int32_t first = thr->শব্দগুলি[cp->bit_idx[0]].w_int;
      int32_t last = thr->শব্দগুলি[cp->bit_idx[1]].w_int;
      string&val = thr->peek_str(0);

      যদি (first < 0 || last < first || last >= (int32_t)val.size()) {
	    val = string("");
	    প্রত্যাগম সত্য;
      }

      val = val.substr(first, last-first+1);
      প্রত্যাগম সত্য;
}

/*
 * %substr/vec4 <index>, <wid>
 */
তার্কিক of_SUBSTR_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned sel_idx = cp->bit_idx[0];
      unsigned wid = cp->bit_idx[1];

      int32_t sel = thr->শব্দগুলি[sel_idx].w_int;
      ধ্রুব string&val = thr->peek_str(0);

      প্রতিজ্ঞা(wid%8 == 0);

      যদি (sel < 0 || sel >= (int32_t)val.size()) {
	    vvp_vector4_t res (wid, BIT4_0);
	    thr->ঠেলো_vec4(res);
	    প্রত্যাগম সত্য;
      }

      vvp_vector4_t res (wid, BIT4_0);

      প্রতিজ্ঞা(wid==8);
      unsigned বর্ণ tmp = val[sel];
      প্রতি (পূর্ণাঙ্ক idx = 0 ; idx < 8 ; idx += 1) {
	    যদি (tmp & (1<<idx))
		  res.set_bit(idx, BIT4_1);
      }

      thr->ঠেলো_vec4(res);
      প্রত্যাগম সত্য;
}

তার্কিক of_FILE_সারি(vthread_t thr, vvp_code_t cp)
{
      vpiHandle handle = cp->handle;

	/* When it is available, keep the file/line information in the
	   thread for error/warning messages. */
      thr->set_fileline(vpi_get_str(vpiFile, handle),
                        vpi_get(vpi_সারি_No, handle));

      যদি (show_file_সারি)
	    cerr << thr->get_fileline()
	         << vpi_get_str(_vpiDescription, handle) << endl;

      প্রত্যাগম সত্য;
}

/*
 * %test_nul <var-label>;
 * Test if the object at the specified variable is nil. If so, write
 * "1" into flags[4], otherwise write "0" into flags[4].
 */
তার্কিক of_TEST_NUL(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      প্রতিজ্ঞা(net);
      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      প্রতিজ্ঞা(obj);

      যদি (obj->get_object().test_nil())
	    thr->flags[4] = BIT4_1;
      অন্যথা
	    thr->flags[4] = BIT4_0;

      প্রত্যাগম সত্য;
}

তার্কিক of_TEST_NUL_A(vthread_t thr, vvp_code_t cp)
{
      unsigned idx = cp->bit_idx[0];
      unsigned adr = thr->শব্দগুলি[idx].w_int;
      vvp_object_t word;

	/* If the address is undefined, return true. */
      যদি (thr->flags[4] == BIT4_1) {
	    প্রত্যাগম সত্য;
      }

      cp->array->get_word_obj(adr, word);
      যদি (word.test_nil())
	    thr->flags[4] = BIT4_1;
      অন্যথা
	    thr->flags[4] = BIT4_0;

      প্রত্যাগম সত্য;
}

তার্কিক of_TEST_NUL_OBJ(vthread_t thr, vvp_code_t)
{
      যদি (thr->peek_object().test_nil())
	    thr->flags[4] = BIT4_1;
      অন্যথা
	    thr->flags[4] = BIT4_0;
      প্রত্যাগম সত্য;
}

/*
 * %test_nul/prop <pid>, <idx>
 */
তার্কিক of_TEST_NUL_PROP(vthread_t thr, vvp_code_t cp)
{
      unsigned pid = cp->সংখ্যা;
      unsigned idx = cp->bit_idx[0];

      যদি (idx != 0) {
	    প্রতিজ্ঞা(idx < vthread_s::শব্দগুলি_গুনতি);
	    idx = thr->শব্দগুলি[idx].w_uint;
      }

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj  = obj.peek<vvp_cobject>();

      vvp_object_t val;
      cobj->get_object(pid, val, idx);

      যদি (val.test_nil())
	    thr->flags[4] = BIT4_1;
      অন্যথা
	    thr->flags[4] = BIT4_0;

      প্রত্যাগম সত্য;
}

/* and() reduction returns the scalar sum (Verilog-style bitwise-AND per word). */
template <বর্গ SRC>
স্থির vvp_vector4_t uparr_and_শব্দগুলি_src(SRC* src, unsigned wid)
{
      যদি (!src) প্রত্যাগম vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_1);
      প্রতি (size_t i = 0; i < src->get_size(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    res &= vi;
      }
      প্রত্যাগম res;
}

তার্কিক of_UPARR_AND_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      যদি (qsrc) res = uparr_and_শব্দগুলি_src(qsrc, wid);
      অন্যথা যদি (dsrc) res = uparr_and_শব্দগুলি_src(dsrc, wid);
      অন্যথা {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    res = uparr_and_শব্দগুলি_src(src, wid);
      }
      thr->ঠেলো_vec4(res);
      প্রত্যাগম সত্য;
}

/* or() reduction returns the scalar sum (Verilog-style bitwise-AND per word). */
template <বর্গ SRC>
স্থির vvp_vector4_t uparr_or_শব্দগুলি_src(SRC* src, unsigned wid)
{
      যদি (!src) প্রত্যাগম vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_0);
      প্রতি (size_t i = 0; i < src->get_size(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    res |= vi;
      }
      প্রত্যাগম res;
}

তার্কিক of_UPARR_OR_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      যদি (qsrc) res = uparr_or_শব্দগুলি_src(qsrc, wid);
      অন্যথা যদি (dsrc) res = uparr_or_শব্দগুলি_src(dsrc, wid);
      অন্যথা {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    res = uparr_or_শব্দগুলি_src(src, wid);
      }
      thr->ঠেলো_vec4(res);
      প্রত্যাগম সত্য;
}

/* real product() reduction returns the scalar sum */
template <বর্গ SRC>
স্থির দ্বিগুণ uparr_product_শব্দগুলি_src_real(SRC* src)
{
      যদি (!src) প্রত্যাগম 0.0;
      দ্বিগুণ acc = 1.0;
      প্রতি (size_t i = 0; i < src->get_size(); i += 1) {
	    দ্বিগুণ vi;
	    src->get_word(i, vi);
	    acc *= vi;
      }
      প্রত্যাগম acc;
}

তার্কিক of_UPARR_PRODUCT_REAL(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_queue_real* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_real_from_net(thr, net, qsrc, dsrc);
      দ্বিগুণ sum;
      যদি (qsrc) sum = uparr_product_শব্দগুলি_src_real(qsrc);
      অন্যথা যদি (dsrc) sum = uparr_product_শব্দগুলি_src_real(dsrc);
      অন্যথা {
	    vvp_queue* src_q = get_queue_object<vvp_queue_real>(thr, net);
	    vvp_queue_real* src = dynamic_cast<vvp_queue_real*>(src_q);
	    sum = uparr_product_শব্দগুলি_src_real(src);
      }
      thr->ঠেলো_real(sum);
      প্রত্যাগম সত্য;
}

/* product() reduction returns the scalar product (Verilog-style multiplication per word). */
template <বর্গ SRC>
স্থির vvp_vector4_t uparr_product_শব্দগুলি_src(SRC* src, unsigned wid)
{
      যদি (!src) প্রত্যাগম vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t acc = queue_unique_ulong_to_vec4(1UL, wid);
      প্রতি (size_t i = 0; i < src->get_size(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    acc.mul(vi);
      }
      প্রত্যাগম acc;
}

তার্কিক of_UPARR_PRODUCT_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t prod;
      যদি (qsrc) prod = uparr_product_শব্দগুলি_src(qsrc, wid);
      অন্যথা যদি (dsrc) prod = uparr_product_শব্দগুলি_src(dsrc, wid);
      অন্যথা {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    prod = uparr_product_শব্দগুলি_src(src, wid);
      }
      thr->ঠেলো_vec4(prod);
      প্রত্যাগম সত্য;
}

তার্কিক of_UPARR_PRODUCT_PROP_V(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->সংখ্যা;
      unsigned wid = cp->bit_idx[0];

      vvp_object_t& শীর্ষ = thr->peek_object();
      vvp_cobject*cobj = শীর্ষ.peek<vvp_cobject>();
      প্রতিজ্ঞা(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t prod =
	    qsrc ? uparr_product_শব্দগুলি_src(qsrc, wid)
		 : uparr_product_শব্দগুলি_src(dsrc, wid);
      thr->ঠেলো_vec4(prod);
      প্রত্যাগম সত্য;
}

তার্কিক of_UPARR_PRODUCT_OBJ_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->বের_করো_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_vector4_t prod =
	    qsrc ? uparr_product_শব্দগুলি_src(qsrc, wid)
		 : uparr_product_শব্দগুলি_src(dsrc, wid);
      thr->ঠেলো_vec4(prod);
      প্রত্যাগম সত্য;
}

/* real sum() reduction returns the scalar sum */
template <বর্গ SRC>
স্থির দ্বিগুণ uparr_sum_শব্দগুলি_src_real(SRC* src)
{
      দ্বিগুণ acc = 0.0;
      যদি (!src) প্রত্যাগম acc;
      প্রতি (size_t i = 0; i < src->get_size(); i += 1) {
	    দ্বিগুণ vi;
	    src->get_word(i, vi);
	    acc += vi;
      }
      প্রত্যাগম acc;
}

তার্কিক of_UPARR_SUM_REAL(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_queue_real* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_real_from_net(thr, net, qsrc, dsrc);
      দ্বিগুণ sum;
      যদি (qsrc) sum = uparr_sum_শব্দগুলি_src_real(qsrc);
      অন্যথা যদি (dsrc) sum = uparr_sum_শব্দগুলি_src_real(dsrc);
      অন্যথা {
	    vvp_queue* src_q = get_queue_object<vvp_queue_real>(thr, net);
	    vvp_queue_real* src = dynamic_cast<vvp_queue_real*>(src_q);
	    sum = uparr_sum_শব্দগুলি_src_real(src);
      }
      thr->ঠেলো_real(sum);
      প্রত্যাগম সত্য;
}

/* sum() reduction returns the scalar sum (Verilog-style addition per word). */
template <বর্গ SRC>
স্থির vvp_vector4_t uparr_sum_শব্দগুলি_src(SRC* src, unsigned wid)
{
      যদি (!src) প্রত্যাগম vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t acc(wid, BIT4_0);
      প্রতি (size_t i = 0; i < src->get_size(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    acc.add(vi);
      }
      প্রত্যাগম acc;
}

তার্কিক of_UPARR_SUM_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t sum;
      যদি (qsrc) sum = uparr_sum_শব্দগুলি_src(qsrc, wid);
      অন্যথা যদি (dsrc) sum = uparr_sum_শব্দগুলি_src(dsrc, wid);
      অন্যথা {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    sum = uparr_sum_শব্দগুলি_src(src, wid);
      }
      thr->ঠেলো_vec4(sum);
      প্রত্যাগম সত্য;
}

তার্কিক of_UPARR_SUM_PROP_V(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->সংখ্যা;
      unsigned wid = cp->bit_idx[0];

      vvp_object_t& শীর্ষ = thr->peek_object();
      vvp_cobject*cobj = শীর্ষ.peek<vvp_cobject>();
      প্রতিজ্ঞা(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t sum =
	    qsrc ? uparr_sum_শব্দগুলি_src(qsrc, wid)
		 : uparr_sum_শব্দগুলি_src(dsrc, wid);
      thr->ঠেলো_vec4(sum);
      প্রত্যাগম সত্য;
}

তার্কিক of_UPARR_SUM_OBJ_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->বের_করো_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_vector4_t sum =
	    qsrc ? uparr_sum_শব্দগুলি_src(qsrc, wid)
		 : uparr_sum_শব্দগুলি_src(dsrc, wid);
      thr->ঠেলো_vec4(sum);
      প্রত্যাগম সত্য;
}

/* xor() reduction returns the scalar sum (Verilog-style bitwise-XOR per word). */
template <বর্গ SRC>
স্থির vvp_vector4_t uparr_xor_শব্দগুলি_src(SRC* src, unsigned wid)
{
      যদি (!src) প্রত্যাগম vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_0);
      প্রতি (size_t i = 0; i < src->get_size(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    res ^= vi;
      }
      প্রত্যাগম res;
}

তার্কিক of_UPARR_XOR_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      যদি (qsrc) res = uparr_xor_শব্দগুলি_src(qsrc, wid);
      অন্যথা যদি (dsrc) res = uparr_xor_শব্দগুলি_src(dsrc, wid);
      অন্যথা {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    res = uparr_xor_শব্দগুলি_src(src, wid);
      }
      thr->ঠেলো_vec4(res);
      প্রত্যাগম সত্য;
}

তার্কিক of_VPI_CALL(vthread_t thr, vvp_code_t cp)
{
      vpip_execute_vpi_call(thr, cp->handle);

      যদি (schedule_stopped()) {
	    যদি (! schedule_finished())
		  schedule_vthread(thr, 0, অসত্য);

	    প্রত্যাগম অসত্য;
      }

      প্রত্যাগম schedule_finished()? অসত্য : সত্য;
}

/* %wait <label>;
 * Implement the wait by locating the vvp_net_T for the event, and
 * adding this thread to the threads list for the event. The some
 * argument is the  reference to the functor to wait for. This must be
 * an event object of some sort.
 */
তার্কিক of_WAIT(vthread_t thr, vvp_code_t cp)
{
      প্রতিজ্ঞা(! thr->i_am_in_function);
      প্রতিজ্ঞা(! thr->waiting_for_event);
      thr->waiting_for_event = 1;

	/* Add this thread to the list in the event. */
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (cp->net->fun);
      প্রতিজ্ঞা(ep);
      thr->wait_next = ep->add_waiting_thread(thr);

	/* Return false to suspend this thread. */
      প্রত্যাগম অসত্য;
}

/*
 * Implement the %wait/fork (SystemVerilog) instruction by suspending
 * the current thread until all the detached children have finished.
 */
তার্কিক of_WAIT_FORK(vthread_t thr, vvp_code_t)
{
	/* If a %wait/fork is being executed then the parent thread
	 * cannot be waiting in a join or already waiting. */
      প্রতিজ্ঞা(! thr->i_am_in_function);
      প্রতিজ্ঞা(! thr->i_am_joining);
      প্রতিজ্ঞা(! thr->i_am_waiting);

	/* There should be no active children when waiting. */
      প্রতিজ্ঞা(thr->children.empty());

	/* If there are no detached children then there is nothing to
	 * wait for. */
      যদি (thr->detached_children.empty()) প্রত্যাগম সত্য;

	/* Flag that this process is waiting for the detached children
	 * to finish and suspend it. */
      thr->i_am_waiting = 1;
      প্রত্যাগম অসত্য;
}

/*
 * %xnor
 */
তার্কিক of_XNOR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valr = thr->বের_করো_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      প্রতিজ্ঞা(vall.size() == valr.size());

      vall ^= valr;
      vall.invert();

      প্রত্যাগম সত্য;
}

/*
 * %xor
 */
তার্কিক of_XOR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valr = thr->বের_করো_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      প্রতিজ্ঞা(vall.size() == valr.size());

      vall ^= valr;

      প্রত্যাগম সত্য;
}


তার্কিক of_ZOMBIE(vthread_t thr, vvp_code_t)
{
      thr->pc = codespace_null();
      যদি ((thr->parent == 0) && (thr->children.empty())) {
	    যদি (thr->delay_delete)
		  schedule_del_thr(thr);
	    অন্যথা
		  vthread_delete(thr);
      }
      প্রত্যাগম অসত্য;
}

/*
 * This is a phantom opcode used to call user defined functions. It
 * is used in code generated by the .ufunc statement. It contains a
 * pointer to the executable code of the function and a pointer to
 * a ufunc_core object that has all the port information about the
 * function.
 */
স্থির তার্কিক do_exec_ufunc(vthread_t thr, vvp_code_t cp, vthread_t child)
{
      __vpiScope*child_scope = cp->ufunc_core_ptr->func_scope();
      প্রতিজ্ঞা(child_scope);

      প্রতিজ্ঞা(child_scope->get_type_code() == vpiFunction);
      প্রতিজ্ঞা(thr->children.empty());


        /* We can take a number of shortcuts because we know that a
           continuous assignment can only occur in a static scope. */
      প্রতিজ্ঞা(thr->wt_context == 0);
      প্রতিজ্ঞা(thr->rd_context == 0);

        /* If an automatic function, allocate a context for this call. */
      vvp_context_t child_context = 0;
      যদি (child_scope->is_automatic()) {
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
      প্রতিজ্ঞা(thr->children.size()==1);

      child->is_scheduled = 1;
      child->i_am_in_function = 1;
      vthread_run(child);
      running_thread = thr;

      যদি (child->i_have_ended) {
	    do_join(thr, child);
            প্রত্যাগম সত্য;
      } অন্যথা {
	    thr->i_am_joining = 1;
	    প্রত্যাগম অসত্য;
      }
}

তার্কিক of_EXEC_UFUNC_REAL(vthread_t thr, vvp_code_t cp)
{
      __vpiScope*child_scope = cp->ufunc_core_ptr->func_scope();
      প্রতিজ্ঞা(child_scope);

	/* Create a temporary thread and run it immediately. */
      vthread_t child = vthread_new(cp->cptr, child_scope);
      thr->ঠেলো_real(0.0);
      child->args_real.push_back(0);

      প্রত্যাগম do_exec_ufunc(thr, cp, child);
}

তার্কিক of_EXEC_UFUNC_VEC4(vthread_t thr, vvp_code_t cp)
{
      __vpiScope*child_scope = cp->ufunc_core_ptr->func_scope();
      প্রতিজ্ঞা(child_scope);

      vpiScopeFunction*scope_func = dynamic_cast<vpiScopeFunction*>(child_scope);
      প্রতিজ্ঞা(scope_func);

	/* Create a temporary thread and run it immediately. */
      vthread_t child = vthread_new(cp->cptr, child_scope);
      thr->ঠেলো_vec4(vvp_vector4_t(scope_func->get_func_width(), scope_func->get_func_init_val()));
      child->args_vec4.push_back(0);

      প্রত্যাগম do_exec_ufunc(thr, cp, child);
}

/*
 * This is a phantom opcode used to harvest the result of calling a user
 * defined function. It is used in code generated by the .ufunc statement.
 */
তার্কিক of_REAP_UFUNC(vthread_t thr, vvp_code_t cp)
{
      __vpiScope*child_scope = cp->ufunc_core_ptr->func_scope();
      প্রতিজ্ঞা(child_scope);

	/* Copy the output from the result variable to the output
	   ports of the .ufunc device. */
      cp->ufunc_core_ptr->finish_thread();

        /* If an automatic function, free the context for this call. */
      যদি (child_scope->is_automatic()) {
            vthread_free_context(thr->rd_context, child_scope);
            thr->wt_context = 0;
            thr->rd_context = 0;
      }

      প্রত্যাগম সত্য;
}
