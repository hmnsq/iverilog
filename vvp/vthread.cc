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

using നാമാകാശ std;

/* This is the size of an unsigned long in bits. This is just a
   convenience macro. */
# define CPU_WORD_BITS (8*വലുപ്പം(unsigned long))
# define മുകൾ_BIT (1UL << (CPU_WORD_BITS-1))

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

ഘടന vthread_s {
      vthread_s();

      ശൂന്യം debug_dump(ostream&fd, സ്ഥിരം അക്ഷരം*label_text);

	/* This is the program counter. */
      vvp_code_t pc;
	/* These hold the private thread bits. */
      എണ്ണൽ { FLAGS_എണ്ണം = 512, വാക്കുകൾ_എണ്ണം = 16 };
      vvp_bit4_t flags[FLAGS_എണ്ണം];

	/* These are the word registers. */
      സംഘം {
	    int64_t  w_int;
	    uint64_t w_uint;
      } വാക്കുകൾ[വാക്കുകൾ_എണ്ണം];

	// These vectors are depths within the parent thread's
	// corresponding stack.  This is how the %ret/* instructions
	// get at parent thread arguments.
      vector<unsigned> args_real;
      vector<unsigned> args_str;
      vector<unsigned> args_vec4;

    സ്വകാര്യ:
      vector<vvp_vector4_t>അട്ടി_vec4;
    പൊതു:
      വരിയിൽ vvp_vector4_t എടുക്കുക_vec4(ശൂന്യം)
      {
	    പ്രതിജ്ഞാ(! അട്ടി_vec4.empty());
	    vvp_vector4_t val = അട്ടി_vec4.back();
	    അട്ടി_vec4.pop_back();
	    തിരികെ val;
      }
      വരിയിൽ ശൂന്യം തള്ളുക_vec4(സ്ഥിരം vvp_vector4_t&val)
      {
	    അട്ടി_vec4.push_back(val);
      }
      വരിയിൽ സ്ഥിരം vvp_vector4_t& peek_vec4(unsigned depth)
      {
	    unsigned size = അട്ടി_vec4.size();
	    പ്രതിജ്ഞാ(depth < size);
	    unsigned use_index = size-1-depth;
	    തിരികെ അട്ടി_vec4[use_index];
      }
      വരിയിൽ vvp_vector4_t& peek_vec4(ശൂന്യം)
      {
	    unsigned use_index = അട്ടി_vec4.size();
	    പ്രതിജ്ഞാ(use_index >= 1);
	    തിരികെ അട്ടി_vec4[use_index-1];
      }
      വരിയിൽ ശൂന്യം poke_vec4(unsigned depth, സ്ഥിരം vvp_vector4_t&val)
      {
	    പ്രതിജ്ഞാ(depth < അട്ടി_vec4.size());
	    unsigned use_index = അട്ടി_vec4.size()-1-depth;
	    അട്ടി_vec4[use_index] = val;
      }
      വരിയിൽ ശൂന്യം എടുക്കുക_vec4(unsigned cnt)
      {
	    ആകുമ്പോൾ (cnt > 0) {
		  അട്ടി_vec4.pop_back();
		  cnt -= 1;
	    }
      }


    സ്വകാര്യ:
      vector<ഇരട്ട> അട്ടി_real;
    പൊതു:
      വരിയിൽ ഇരട്ട എടുക്കുക_real(ശൂന്യം)
      {
	    പ്രതിജ്ഞാ(! അട്ടി_real.empty());
	    ഇരട്ട val = അട്ടി_real.back();
	    അട്ടി_real.pop_back();
	    തിരികെ val;
      }
      വരിയിൽ ശൂന്യം തള്ളുക_real(ഇരട്ട val)
      {
	    അട്ടി_real.push_back(val);
      }
      വരിയിൽ ഇരട്ട peek_real(unsigned depth)
      {
	    പ്രതിജ്ഞാ(depth < അട്ടി_real.size());
	    unsigned use_index = അട്ടി_real.size()-1-depth;
	    തിരികെ അട്ടി_real[use_index];
      }
      വരിയിൽ ശൂന്യം poke_real(unsigned depth, ഇരട്ട val)
      {
	    പ്രതിജ്ഞാ(depth < അട്ടി_real.size());
	    unsigned use_index = അട്ടി_real.size()-1-depth;
	    അട്ടി_real[use_index] = val;
      }
      വരിയിൽ ശൂന്യം എടുക്കുക_real(unsigned cnt)
      {
	    ആകുമ്പോൾ (cnt > 0) {
		  അട്ടി_real.pop_back();
		  cnt -= 1;
	    }
      }

	/* Strings are operated on using a forth-like operator
	   set. Items at the top of the stack (back()) are the objects
	   operated on except for special cases. New objects are
	   pushed onto the top (back()) and pulled from the top
	   (back()) only. */
    സ്വകാര്യ:
      vector<string> അട്ടി_str;
    പൊതു:
      വരിയിൽ string എടുക്കുക_str(ശൂന്യം)
      {
	    പ്രതിജ്ഞാ(! അട്ടി_str.empty());
	    string val = അട്ടി_str.back();
	    അട്ടി_str.pop_back();
	    തിരികെ val;
      }
      വരിയിൽ ശൂന്യം തള്ളുക_str(സ്ഥിരം string&val)
      {
	    അട്ടി_str.push_back(val);
      }
      വരിയിൽ string&peek_str(unsigned depth)
      {
	    പ്രതിജ്ഞാ(depth<അട്ടി_str.size());
	    unsigned use_index = അട്ടി_str.size()-1-depth;
	    തിരികെ അട്ടി_str[use_index];
      }
      വരിയിൽ ശൂന്യം poke_str(unsigned depth, സ്ഥിരം string&val)
      {
	    പ്രതിജ്ഞാ(depth < അട്ടി_str.size());
	    unsigned use_index = അട്ടി_str.size()-1-depth;
	    അട്ടി_str[use_index] = val;
      }
      വരിയിൽ ശൂന്യം എടുക്കുക_str(unsigned cnt)
      {
	    ആകുമ്പോൾ (cnt > 0) {
		  അട്ടി_str.pop_back();
		  cnt -= 1;
	    }
      }

	/* Objects are also operated on in a stack. */
    സ്വകാര്യ:
      എണ്ണൽ { അട്ടി_OBJ_MAX_SIZE = 32 };
      vvp_object_t അട്ടി_obj[അട്ടി_OBJ_MAX_SIZE];
      unsigned അട്ടി_obj_size;
    പൊതു:
      വരിയിൽ vvp_object_t& peek_object(ശൂന്യം)
      {
	    പ്രതിജ്ഞാ(അട്ടി_obj_size > 0);
	    തിരികെ അട്ടി_obj[അട്ടി_obj_size-1];
      }
      വരിയിൽ ശൂന്യം എടുക്കുക_object(vvp_object_t&obj)
      {
	    പ്രതിജ്ഞാ(അട്ടി_obj_size > 0);
	    അട്ടി_obj_size -= 1;
	    obj = അട്ടി_obj[അട്ടി_obj_size];
	    അട്ടി_obj[അട്ടി_obj_size].reset(0);
      }
      വരിയിൽ ശൂന്യം എടുക്കുക_object(unsigned cnt, unsigned skip =0)
      {
	    പ്രതിജ്ഞാ((cnt+skip) <= അട്ടി_obj_size);
	    വേണ്ടി (size_t idx = അട്ടി_obj_size-skip-cnt ; idx < അട്ടി_obj_size-skip ; idx += 1)
		  അട്ടി_obj[idx].reset(0);
	    അട്ടി_obj_size -= cnt;
	    വേണ്ടി (size_t idx = അട്ടി_obj_size-skip ; idx < അട്ടി_obj_size ; idx += 1)
		  അട്ടി_obj[idx] = അട്ടി_obj[idx+skip];
	    വേണ്ടി (size_t idx = അട്ടി_obj_size ; idx < അട്ടി_obj_size+skip ; idx += 1)
		  അട്ടി_obj[idx].reset(0);
      }
      വരിയിൽ ശൂന്യം തള്ളുക_object(സ്ഥിരം vvp_object_t&obj)
      {
	    പ്രതിജ്ഞാ(അട്ടി_obj_size < അട്ടി_OBJ_MAX_SIZE);
	    അട്ടി_obj[അട്ടി_obj_size] = obj;
	    അട്ടി_obj_size += 1;
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
      set<ഘടന vthread_s*>children;
	/* This points to the detached children of the thread. */
      set<ഘടന vthread_s*>detached_children;
	/* This points to my parent, if I have one. */
      ഘടന vthread_s*parent;
	/* This points to the containing scope. */
      __vpiScope*parent_scope;
	/* This is used for keeping wait queues. */
      ഘടന vthread_s*wait_next;
	/* These are used to access automatically allocated items. */
      vvp_context_t wt_context, rd_context;
	/* These are used to pass non-blocking event control information. */
      vvp_net_t*event;
      uint64_t ecount;
	/* Save the file/line information when available. */
    സ്വകാര്യ:
      അക്ഷരം *filenm_;
      unsigned lineno_;
    പൊതു:
      ശൂന്യം set_fileline(അക്ഷരം *filenm, unsigned lineno);
      string get_fileline();

      വരിയിൽ ശൂന്യം cleanup()
      {
	    എങ്കിൽ (i_was_disabled) {
		  അട്ടി_vec4.clear();
		  അട്ടി_real.clear();
		  അട്ടി_str.clear();
		  എടുക്കുക_object(അട്ടി_obj_size);
	    }
	    സ്വതന്ത്രം(filenm_);
	    filenm_ = 0;
	    പ്രതിജ്ഞാ(അട്ടി_vec4.empty());
	    പ്രതിജ്ഞാ(അട്ടി_real.empty());
	    പ്രതിജ്ഞാ(അട്ടി_str.empty());
	    പ്രതിജ്ഞാ(അട്ടി_obj_size == 0);
      }
};

വരിയിൽ vthread_s::vthread_s()
{
      അട്ടി_obj_size = 0;
      filenm_ = 0;
      lineno_ = 0;
}

ശൂന്യം vthread_s::set_fileline(അക്ഷരം *filenm, unsigned lineno)
{
      പ്രതിജ്ഞാ(filenm);
      എങ്കിൽ (!filenm_ || (strcmp(filenm_, filenm) != 0)) {
	    സ്വതന്ത്രം(filenm_);
	    filenm_ = strdup(filenm);
      }
      lineno_ = lineno;
}

വരിയിൽ string vthread_s::get_fileline()
{
      ostringstream buf;
      എങ്കിൽ (filenm_) {
	    buf << filenm_ << ":" << lineno_ << ": ";
      }
      string res = buf.str();
      തിരികെ res;
}

ശൂന്യം vthread_s::debug_dump(ostream&fd, സ്ഥിരം അക്ഷരം*label)
{
      fd << "**** " << label << endl;
      fd << "**** ThreadId: " << ഇത് << ", parent id: " << parent << endl;

      fd << "**** Flags: ";
      വേണ്ടി (പൂർണ്ണസംഖ്യ idx = 0 ; idx < FLAGS_എണ്ണം ; idx += 1)
	    fd << flags[idx];
      fd << endl;
      fd << "**** vec4 stack..." << endl;
      വേണ്ടി (size_t idx = അട്ടി_vec4.size() ; idx > 0 ; idx -= 1)
	    fd << "    " << (അട്ടി_vec4.size()-idx) << ": " << അട്ടി_vec4[idx-1] << endl;
      fd << "**** str stack (" << അട്ടി_str.size() << ")..." << endl;
      fd << "**** obj stack (" << അട്ടി_obj_size << ")..." << endl;
      fd << "**** args_vec4 array (" << args_vec4.size() << ")..." << endl;
      വേണ്ടി (size_t idx = 0 ; idx < args_vec4.size() ; idx += 1)
	    fd << "    " << idx << ": " << args_vec4[idx] << endl;
      fd << "**** file/line (";
      എങ്കിൽ (filenm_) fd << filenm_;
      അല്ലെങ്കിൽ fd << "<no file name>";
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
സ്ഥിതം string filter_string(സ്ഥിരം അക്ഷരം*text)
{
      vector<അക്ഷരം> tmp (strlen(text)+1);
      size_t dst = 0;
      വേണ്ടി (സ്ഥിരം അക്ഷരം*ptr = text ; *ptr ; ptr += 1) {
	    // Not an escape? Move on.
	    എങ്കിൽ (*ptr != '\\') {
		  tmp[dst++] = *ptr;
		  തുടരുക;
	    }

	    // Now we know that *ptr is pointing to a \ character and we
	    // have an octal sequence coming up. Advance the ptr and start
	    // processing octal digits.
	    ptr += 1;
	    എങ്കിൽ (*ptr == 0)
		  നിർത്തുക;

	    അക്ഷരം byte = 0;
	    പൂർണ്ണസംഖ്യ cnt = 3;
	    ആകുമ്പോൾ (*ptr && cnt > 0 && *ptr >= '0' && *ptr <= '7') {
		  byte *= 8;
		  byte += *ptr - '0';
		  cnt -= 1;
		  ptr += 1;
	    }

	    // null-bytes are supposed to be removed when assigning a string
	    // literal to a string.
	    എങ്കിൽ (byte != '\0')
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
      തിരികെ res;
}

സ്ഥിതം ശൂന്യം do_join(vthread_t thr, vthread_t child);

__vpiScope* vthread_scope(ഘടന vthread_s*thr)
{
      തിരികെ thr->parent_scope;
}

ഘടന vthread_s*running_thread = 0;

string get_fileline()
{
      തിരികെ running_thread->get_fileline();
}

ശൂന്യം vthread_തള്ളുക(ഘടന vthread_s*thr, ഇരട്ട val)
{
      thr->തള്ളുക_real(val);
}

ശൂന്യം vthread_തള്ളുക(ഘടന vthread_s*thr, സ്ഥിരം string&val)
{
      thr->തള്ളുക_str(val);
}

ശൂന്യം vthread_തള്ളുക(ഘടന vthread_s*thr, സ്ഥിരം vvp_vector4_t&val)
{
      thr->തള്ളുക_vec4(val);
}

ശൂന്യം vthread_എടുക്കുക_real(ഘടന vthread_s*thr, unsigned depth)
{
      thr->എടുക്കുക_real(depth);
}

ശൂന്യം vthread_എടുക്കുക_str(ഘടന vthread_s*thr, unsigned depth)
{
      thr->എടുക്കുക_str(depth);
}

ശൂന്യം vthread_എടുക്കുക_vec4(ഘടന vthread_s*thr, unsigned depth)
{
      thr->എടുക്കുക_vec4(depth);
}

ഇരട്ട vthread_get_real_അട്ടി(ഘടന vthread_s*thr, unsigned depth)
{
      തിരികെ thr->peek_real(depth);
}

സ്ഥിരം string&vthread_get_str_അട്ടി(ഘടന vthread_s*thr, unsigned depth)
{
      തിരികെ thr->peek_str(depth);
}

സ്ഥിരം vvp_vector4_t& vthread_get_vec4_അട്ടി(ഘടന vthread_s*thr, unsigned depth)
{
      തിരികെ thr->peek_vec4(depth);
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
template <വർഗ്ഗം VVP_QUEUE> സ്ഥിതം vvp_queue*get_queue_object(vthread_t thr, vvp_net_t*net)
{
      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      പ്രതിജ്ഞാ(obj);

      vvp_queue*queue = obj->get_object().peek<vvp_queue>();
      എങ്കിൽ (queue == 0) {
	    പ്രതിജ്ഞാ(obj->get_object().test_nil());
	    queue = പുതിയ VVP_QUEUE;
	    vvp_object_t val (queue);
	    vvp_net_ptr_t ptr (net, 0);
	    vvp_send_object(ptr, val, thr->wt_context);
      }

      തിരികെ queue;
}

/*
 * Locator and size/word opcodes use the same VVP net for a queue ($) or a
 * dynamic array ([]). Queues are vvp_queue_vec4; dynamic arrays may be
 * vvp_darray_vec4, vvp_darray_atom<>, etc. (all vvp_darray, but vvp_queue
 * also derives from vvp_darray — test queue vec4 first).
 */
സ്ഥിതം ശൂന്യം get_queue_or_darray_vec4_from_net(vthread_t thr, vvp_net_t* net,
					      vvp_queue_vec4*& qsrc,
					      vvp_darray*& dsrc)
{
      vvp_fun_signal_object* obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      പ്രതിജ്ഞാ(obj);
      vvp_object_t മുകൾ = obj->get_object();
      qsrc = മുകൾ.peek<vvp_queue_vec4>();
      എങ്കിൽ (qsrc) {
	    dsrc = 0;
	    തിരികെ;
      }
      vvp_darray* dany = മുകൾ.peek<vvp_darray>();
      എങ്കിൽ (dany && dynamic_cast<vvp_queue*>(dany) == 0) {
	    dsrc = dany;
	    തിരികെ;
      }
      എങ്കിൽ (മുകൾ.test_nil()) {
	    qsrc = പുതിയ vvp_queue_vec4();
	    vvp_object_t val(qsrc);
	    vvp_net_ptr_t ptr(net, 0);
	    vvp_send_object(ptr, val, thr->wt_context);
	    dsrc = 0;
	    തിരികെ;
      }
      /* e.g. real/string queue — not vec4 queue or plain darray */
      qsrc = 0;
      dsrc = 0;
}

സ്ഥിതം ശൂന്യം get_queue_or_darray_real_from_net(vthread_t thr, vvp_net_t* net,
					      vvp_queue_real*& qsrc,
					      vvp_darray*& dsrc)
{
      vvp_fun_signal_object* obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      പ്രതിജ്ഞാ(obj);
      vvp_object_t മുകൾ = obj->get_object();
      qsrc = മുകൾ.peek<vvp_queue_real>();
      എങ്കിൽ (qsrc) {
	    dsrc = 0;
	    തിരികെ;
      }
      vvp_darray* dany = മുകൾ.peek<vvp_darray>();
      എങ്കിൽ (dany && dynamic_cast<vvp_queue*>(dany) == 0) {
	    dsrc = dany;
	    തിരികെ;
      }
      എങ്കിൽ (മുകൾ.test_nil()) {
	    qsrc = പുതിയ vvp_queue_real();
	    vvp_object_t val(qsrc);
	    vvp_net_ptr_t ptr(net, 0);
	    vvp_send_object(ptr, val, thr->wt_context);
	    dsrc = 0;
	    തിരികെ;
      }
      /* e.g. real/string queue — not vec4 queue or plain darray */
      qsrc = 0;
      dsrc = 0;
}

സ്ഥിതം ശൂന്യം get_queue_or_darray_vec4_from_object(vvp_object_t src_obj,
						 vvp_queue_vec4*& qsrc,
						 vvp_darray*& dsrc)
{
      qsrc = src_obj.peek<vvp_queue_vec4>();
      എങ്കിൽ (qsrc) {
	    dsrc = 0;
	    തിരികെ;
      }
      vvp_darray* dany = src_obj.peek<vvp_darray>();
      എങ്കിൽ (dany && dynamic_cast<vvp_queue*>(dany) == 0) {
	    dsrc = dany;
	    തിരികെ;
      }
      qsrc = 0;
      dsrc = 0;
}

/*
 * The following are used to allow a common template to be written for
 * queue real/string/vec4 operations
 */
വരിയിൽ സ്ഥിതം ശൂന്യം എടുക്കുക_മൂല്യം(vthread_t thr, ഇരട്ട&value, unsigned)
{
      value = thr->എടുക്കുക_real();
}

വരിയിൽ സ്ഥിതം ശൂന്യം എടുക്കുക_മൂല്യം(vthread_t thr, string&value, unsigned)
{
      value = thr->എടുക്കുക_str();
}

വരിയിൽ സ്ഥിതം ശൂന്യം എടുക്കുക_മൂല്യം(vthread_t thr, vvp_vector4_t&value, unsigned wid)
{
      value = thr->എടുക്കുക_vec4();
      പ്രതിജ്ഞാ(value.size() == wid);
}

/*
 * The following are used to allow the queue templates to print correctly.
 */
വരിയിൽ സ്ഥിതം string get_queue_type(ഇരട്ട&)
{
      തിരികെ "queue<real>";
}

വരിയിൽ സ്ഥിതം string get_queue_type(string&)
{
      തിരികെ "queue<string>";
}

വരിയിൽ സ്ഥിതം string get_queue_type(സ്ഥിരം vvp_vector4_t&value)
{
      ostringstream buf;
      buf << "queue<vector[" << value.size() << "]>";
      string res = buf.str();
      തിരികെ res;
}

വരിയിൽ സ്ഥിതം ശൂന്യം print_queue_മൂല്യം(ഇരട്ട value)
{
      cerr << value;
}

വരിയിൽ സ്ഥിതം ശൂന്യം print_queue_മൂല്യം(സ്ഥിരം string&value)
{
      cerr << "\"" << value << "\"";
}

വരിയിൽ സ്ഥിതം ശൂന്യം print_queue_മൂല്യം(സ്ഥിരം vvp_vector4_t&value)
{
      cerr << value;
}

/*
 * The following are used to get a darray/queue default value.
 */
വരിയിൽ സ്ഥിതം ശൂന്യം dq_default(ഇരട്ട&value, unsigned)
{
      value = 0.0;
}

വരിയിൽ സ്ഥിതം ശൂന്യം dq_default(string&value, unsigned)
{
      value = "";
}

വരിയിൽ സ്ഥിതം ശൂന്യം dq_default(vvp_vector4_t&value, unsigned wid)
{
      value = vvp_vector4_t(wid);
}


template <വർഗ്ഗം T> T coerce_to_width(സ്ഥിരം T&that, unsigned width)
{
      എങ്കിൽ (that.size() == width)
	    തിരികെ that;

      പ്രതിജ്ഞാ(that.size() > width);
      T res (width);
      വേണ്ടി (unsigned idx = 0 ;  idx < width ;  idx += 1)
	    res.set_bit(idx, that.value(idx));

      തിരികെ res;
}

/* Explicitly define the vvp_vector4_t version of coerce_to_width(). */
template vvp_vector4_t coerce_to_width(സ്ഥിരം vvp_vector4_t&that,
                                       unsigned width);


സ്ഥിതം ശൂന്യം multiply_array_imm(unsigned long*res, സ്ഥിരം unsigned long*val,
			       unsigned വാക്കുകൾ, unsigned long imm)
{
      വേണ്ടി (unsigned idx = 0 ; idx < വാക്കുകൾ ; idx += 1)
	    res[idx] = 0;

      വേണ്ടി (unsigned mul_idx = 0 ; mul_idx < വാക്കുകൾ ; mul_idx += 1) {
	    unsigned long sum;
	    unsigned long tmp = multiply_with_carry(val[mul_idx], imm, sum);

	    unsigned long carry = 0;
	    res[mul_idx] = add_with_carry(res[mul_idx], tmp, carry);
	    വേണ്ടി (unsigned add_idx = mul_idx+1 ; add_idx < വാക്കുകൾ ; add_idx += 1) {
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
സ്ഥിതം vvp_context_t vthread_alloc_context(__vpiScope*scope)
{
      പ്രതിജ്ഞാ(scope->is_automatic());

      vvp_context_t context = scope->free_contexts;
      എങ്കിൽ (context) {
            scope->free_contexts = vvp_get_next_context(context);
            വേണ്ടി (unsigned idx = 0 ; idx < scope->nitem ; idx += 1) {
                  scope->item[idx]->reset_instance(context);
            }
      } അല്ലെങ്കിൽ {
            context = vvp_allocate_context(scope->nitem);
            വേണ്ടി (unsigned idx = 0 ; idx < scope->nitem ; idx += 1) {
                  scope->item[idx]->alloc_instance(context);
            }
      }

      vvp_set_next_context(context, scope->live_contexts);
      scope->live_contexts = context;

      തിരികെ context;
}

/*
 * Free a context previously allocated to a child thread by pushing it
 * onto the freed context stack. Remove it from the list of live contexts
 * in that scope.
 */
സ്ഥിതം ശൂന്യം vthread_free_context(vvp_context_t context, __vpiScope*scope)
{
      പ്രതിജ്ഞാ(scope->is_automatic());
      പ്രതിജ്ഞാ(context);

      എങ്കിൽ (context == scope->live_contexts) {
            scope->live_contexts = vvp_get_next_context(context);
      } അല്ലെങ്കിൽ {
            vvp_context_t tmp = scope->live_contexts;
            ആകുമ്പോൾ (context != vvp_get_next_context(tmp)) {
                  പ്രതിജ്ഞാ(tmp);
                  tmp = vvp_get_next_context(tmp);
            }
            vvp_set_next_context(tmp, vvp_get_next_context(context));
      }

      vvp_set_next_context(context, scope->free_contexts);
      scope->free_contexts = context;
}

#ifdef CHECK_WITH_VALGRIND
ശൂന്യം contexts_delete(വർഗ്ഗം __vpiScope*scope)
{
      vvp_context_t context = scope->free_contexts;

      ആകുമ്പോൾ (context) {
	    scope->free_contexts = vvp_get_next_context(context);
	    വേണ്ടി (unsigned idx = 0; idx < scope->nitem; idx += 1) {
		  scope->item[idx]->free_instance(context);
	    }
	    സ്വതന്ത്രം(context);
	    context = scope->free_contexts;
      }
      സ്വതന്ത്രം(scope->item);
}
#endif

/*
 * Create a new thread with the given start address.
 */
vthread_t vthread_new(vvp_code_t pc, __vpiScope*scope)
{
      vthread_t thr = പുതിയ ഘടന vthread_s;
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
      വേണ്ടി (പൂർണ്ണസംഖ്യ idx = 4 ; idx < 8 ; idx += 1)
	    thr->flags[idx] = BIT4_X;

      scope->threads .insert(thr);
      തിരികെ thr;
}

#ifdef CHECK_WITH_VALGRIND
#എങ്കിൽ 0
/*
 * These are not currently correct. If you use them you will get
 * double delete messages. There is still a leak related to a
 * waiting event that needs to be investigated.
 */

സ്ഥിതം ശൂന്യം wait_next_delete(vthread_t base)
{
      ആകുമ്പോൾ (base) {
	    vthread_t tmp = base->wait_next;
	    delete base;
	    base = tmp;
	    എങ്കിൽ (base->waiting_for_event == 0) നിർത്തുക;
      }
}

സ്ഥിതം ശൂന്യം child_delete(vthread_t base)
{
      ആകുമ്പോൾ (base) {
	    vthread_t tmp = base->child;
	    delete base;
	    base = tmp;
      }
}
#endif

ശൂന്യം vthreads_delete(വർഗ്ഗം __vpiScope*scope)
{
      വേണ്ടി (std::set<vthread_t>::iterator cur = scope->threads.begin()
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
സ്ഥിതം ശൂന്യം vthread_reap(vthread_t thr)
{
      എങ്കിൽ (! thr->children.empty()) {
	    വേണ്ടി (set<vthread_t>::iterator cur = thr->children.begin()
		       ; cur != thr->children.end() ; ++cur) {
		  vthread_t child = *cur;
		  പ്രതിജ്ഞാ(child);
		  പ്രതിജ്ഞാ(child->parent == thr);
		  child->parent = thr->parent;
	    }
      }
      എങ്കിൽ (! thr->detached_children.empty()) {
	    വേണ്ടി (set<vthread_t>::iterator cur = thr->detached_children.begin()
		       ; cur != thr->detached_children.end() ; ++cur) {
		  vthread_t child = *cur;
		  പ്രതിജ്ഞാ(child);
		  പ്രതിജ്ഞാ(child->parent == thr);
		  പ്രതിജ്ഞാ(child->i_am_detached);
		  child->parent = 0;
		  child->i_am_detached = 0;
	    }
      }
      എങ്കിൽ (thr->parent) {
	      /* assert that the given element was removed. */
	    എങ്കിൽ (thr->i_am_detached) {
		  size_t res = thr->parent->detached_children.erase(thr);
		  പ്രതിജ്ഞാ(res == 1);
	    } അല്ലെങ്കിൽ {
		  size_t res = thr->parent->children.erase(thr);
		  പ്രതിജ്ഞാ(res == 1);
	    }
      }

      thr->parent = 0;

	// Remove myself from the containing scope if needed.
      thr->parent_scope->threads.erase(thr);

      thr->pc = codespace_null();

	/* If this thread is not scheduled, then is it safe to delete
	   it now. Otherwise, let the schedule event (which will
	   execute the thread at of_ZOMBIE) delete the object. */
      എങ്കിൽ ((thr->is_scheduled == 0) && (thr->waiting_for_event == 0)) {
	    പ്രതിജ്ഞാ(thr->children.empty());
	    പ്രതിജ്ഞാ(thr->wait_next == 0);
	    എങ്കിൽ (thr->delay_delete)
		  schedule_del_thr(thr);
	    അല്ലെങ്കിൽ
		  vthread_delete(thr);
      }
}

ശൂന്യം vthread_delete(vthread_t thr)
{
      thr->cleanup();
      delete thr;
}

ശൂന്യം vthread_mark_scheduled(vthread_t thr)
{
      ആകുമ്പോൾ (thr != 0) {
	    പ്രതിജ്ഞാ(thr->is_scheduled == 0);
	    thr->is_scheduled = 1;
	    thr = thr->wait_next;
      }
}

ശൂന്യം vthread_mark_final(vthread_t thr)
{
      /*
       * The behavior in a final thread is the same as in a function. Any
       * child thread will be executed immediately rather than being
       * scheduled.
       */
      ആകുമ്പോൾ (thr != 0) {
	    thr->i_am_in_function = 1;
	    thr = thr->wait_next;
      }
}

ശൂന്യം vthread_delay_delete()
{
      എങ്കിൽ (running_thread)
	    running_thread->delay_delete = 1;
}

/*
 * This function runs each thread by fetching an instruction,
 * incrementing the PC, and executing the instruction. The thread may
 * be the head of a list, so each thread is run so far as possible.
 */
ശൂന്യം vthread_run(vthread_t thr)
{
      ആകുമ്പോൾ (thr != 0) {
	    vthread_t tmp = thr->wait_next;
	    thr->wait_next = 0;

	    പ്രതിജ്ഞാ(thr->is_scheduled);
	    thr->is_scheduled = 0;

            running_thread = thr;

	    വേണ്ടി (;;) {
		  vvp_code_t cp = thr->pc;
		  thr->pc += 1;

		    /* Run the opcode implementation. If the execution of
		       the opcode returns false, then the thread is meant to
		       be paused, so break out of the loop. */
		  തർക്കം rc = (cp->opcode)(thr, cp);
		  എങ്കിൽ (rc == അസത്യം)
			നിർത്തുക;
	    }

	    thr = tmp;
      }
      running_thread = 0;
}

/*
 * The CHUNK_LINK instruction is a special next pointer for linking
 * chunks of code space. It's like a simplified %jmp.
 */
തർക്കം of_CHUNK_LINK(vthread_t thr, vvp_code_t code)
{
      പ്രതിജ്ഞാ(code->cptr);
      thr->pc = code->cptr;
      തിരികെ സത്യം;
}

/*
 * This is called by an event functor to wake up all the threads on
 * its list. I in fact created that list in the %wait instruction, and
 * I also am certain that the waiting_for_event flag is set.
 */
ശൂന്യം vthread_schedule_list(vthread_t thr)
{
      വേണ്ടി (vthread_t cur = thr ;  cur ;  cur = cur->wait_next) {
	    പ്രതിജ്ഞാ(cur->waiting_for_event);
	    cur->waiting_for_event = 0;
      }

      schedule_vthread(thr, 0);
}

vvp_context_t vthread_get_wt_context()
{
      എങ്കിൽ (running_thread)
            തിരികെ running_thread->wt_context;
      അല്ലെങ്കിൽ
            തിരികെ 0;
}

vvp_context_t vthread_get_rd_context()
{
      എങ്കിൽ (running_thread)
            തിരികെ running_thread->rd_context;
      അല്ലെങ്കിൽ
            തിരികെ 0;
}

vvp_context_item_t vthread_get_wt_context_item(unsigned context_idx)
{
      പ്രതിജ്ഞാ(running_thread && running_thread->wt_context);
      തിരികെ vvp_get_context_item(running_thread->wt_context, context_idx);
}

vvp_context_item_t vthread_get_rd_context_item(unsigned context_idx)
{
      പ്രതിജ്ഞാ(running_thread && running_thread->rd_context);
      തിരികെ vvp_get_context_item(running_thread->rd_context, context_idx);
}

/*
 * %abs/wr
 */
തർക്കം of_ABS_WR(vthread_t thr, vvp_code_t)
{
      thr->തള്ളുക_real( fabs(thr->എടുക്കുക_real()) );
      തിരികെ സത്യം;
}

തർക്കം of_ALLOC(vthread_t thr, vvp_code_t cp)
{
        /* Allocate a context. */
      vvp_context_t child_context = vthread_alloc_context(cp->scope);

        /* Push the allocated context onto the write context stack. */
      vvp_set_stacked_context(child_context, thr->wt_context);
      thr->wt_context = child_context;

      തിരികെ സത്യം;
}

തർക്കം of_AND(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valb = thr->എടുക്കുക_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();
      പ്രതിജ്ഞാ(vala.size() == valb.size());
      vala &= valb;
      തിരികെ സത്യം;
}

/*
 * This function must ALWAYS be called with the val set to the right
 * size, and initialized with BIT4_0 bits. Certain optimizations rely
 * on that.
 */
സ്ഥിതം ശൂന്യം get_immediate_rval(vvp_code_t cp, vvp_vector4_t&val)
{
      uint32_t vala = cp->bit_idx[0];
      uint32_t valb = cp->bit_idx[1];
      unsigned wid  = cp->സംഖ്യ;

      എങ്കിൽ (valb == 0) {
	      // Special case: if the value is zero, we are done
	      // before we start.
	    എങ്കിൽ (vala == 0) തിരികെ;

	      // Special case: The value has no X/Z bits, so we can
	      // use the setarray method to write the value all at once.
	    unsigned use_wid = 8*വലുപ്പം(unsigned long);
	    എങ്കിൽ (wid < use_wid)
		  use_wid = wid;
	    unsigned long tmp[1];
	    tmp[0] = vala;
	    val.setarray(0, use_wid, tmp);
	    തിരികെ;
      }

	// The immediate value can be values bigger then 32 bits, but
	// only if the high bits are zero. So at most we need to run
	// through the loop below 32 times. Maybe less, if the target
	// width is less. We don't have to do anything special on that
	// because vala/valb bits will shift away so (vala|valb) will
	// turn to zero at or before 32 shifts.

      വേണ്ടി (unsigned idx = 0 ; idx < wid && (vala|valb) ; idx += 1) {
	    uint32_t ba = 0;
	      // Convert the vala/valb bits to a ba number that
	      // matches the encoding of the vvp_bit4_t enumeration.
	    ba = (valb & 1) << 1;
	    ba |= vala & 1;

	      // Note that the val is already pre-filled with BIT4_0
	      // bits, os we only need to set non-zero bit values.
	    എങ്കിൽ (ba) val.set_bit(idx, (vvp_bit4_t)ba);

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
തർക്കം of_ADD(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t r = thr->എടുക്കുക_vec4();
	// Rather then pop l, use it directly from the stack. When we
	// assign to 'l', that will edit the top of the stack, which
	// replaces a pop and a pull.
      vvp_vector4_t&l = thr->peek_vec4();

      l.add(r);

      തിരികെ സത്യം;
}

/*
 * %addi <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
തർക്കം of_ADDI(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->സംഖ്യ;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_immediate_rval (cp, r);

      l.add(r);

      തിരികെ സത്യം;
}

/*
 * %add/wr
 */
തർക്കം of_ADD_WR(vthread_t thr, vvp_code_t)
{
      ഇരട്ട r = thr->എടുക്കുക_real();
      ഇരട്ട l = thr->എടുക്കുക_real();
      thr->തള്ളുക_real(l + r);
      തിരികെ സത്യം;
}

/* %assign/ar <array>, <delay>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory. <delay>
 * is the delay in simulation time. <bit> is the index register
 * containing the real value.
 */
തർക്കം of_ASSIGN_AR(vthread_t thr, vvp_code_t cp)
{
      long adr = thr->വാക്കുകൾ[3].w_int;
      unsigned delay = cp->bit_idx[0];
      ഇരട്ട value = thr->എടുക്കുക_real();

      എങ്കിൽ (adr >= 0) {
	    schedule_assign_array_word(cp->array, adr, value, delay);
      }

      തിരികെ സത്യം;
}

/* %assign/ar/d <array>, <delay_idx>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory.
 * <delay_idx> is the integer register that contains the delay value.
 */
തർക്കം of_ASSIGN_ARD(vthread_t thr, vvp_code_t cp)
{
      long adr = thr->വാക്കുകൾ[3].w_int;
      ഇരട്ട value = thr->എടുക്കുക_real();

      എങ്കിൽ (adr >= 0) {
	    vvp_time64_t delay = thr->വാക്കുകൾ[cp->bit_idx[0]].w_uint;
	    schedule_assign_array_word(cp->array, adr, value, delay);
      }

      തിരികെ സത്യം;
}

/* %assign/ar/e <array>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory. <bit>
 * is the index register containing the real value. The event
 * information is contained in the thread event control registers
 * and is set with %evctl.
 */
തർക്കം of_ASSIGN_ARE(vthread_t thr, vvp_code_t cp)
{
      long adr = thr->വാക്കുകൾ[3].w_int;
      ഇരട്ട value = thr->എടുക്കുക_real();

      എങ്കിൽ (adr >= 0) {
	    എങ്കിൽ (thr->ecount == 0) {
		  schedule_assign_array_word(cp->array, adr, value, 0);
	    } അല്ലെങ്കിൽ {
		  schedule_evctl(cp->array, adr, value, thr->event,
		                 thr->ecount);
	    }
      }

      തിരികെ സത്യം;
}

/*
 * %assign/vec4 <var>, <delay>
 */
തർക്കം of_ASSIGN_VEC4(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      unsigned delay = cp->bit_idx[0];
      സ്ഥിരം vvp_vector4_t&val = thr->peek_vec4();

      schedule_assign_vector(ptr, 0, 0, val, delay);
      thr->എടുക്കുക_vec4(1);
      തിരികെ സത്യം;
}

/*
 * Resizes a vector value for a partial assignment so that the value is fully
 * in-bounds of the target signal. Both `val` and `off` will be updated if
 * necessary.
 *
 * Returns false if the value is fully out-of-bounds and the assignment should
 * be skipped. Otherwise returns true.
 */
സ്ഥിതം തർക്കം resize_rval_vec(vvp_vector4_t &val, int64_t &off,
			    unsigned പൂർണ്ണസംഖ്യ sig_wid)
{
      unsigned പൂർണ്ണസംഖ്യ wid = val.size();

        // Fully in bounds, most likely case
      എങ്കിൽ (off >= 0 && (uint64_t)off + wid <= sig_wid)
	    തിരികെ സത്യം;

      unsigned പൂർണ്ണസംഖ്യ base = 0;
      എങ്കിൽ (off >= 0) {
	      // Fully out-of-bounds
	    എങ്കിൽ ((uint64_t)off >= sig_wid)
		  തിരികെ അസത്യം;
      } അല്ലെങ്കിൽ {
	      // Fully out-of-bounds */
	    എങ്കിൽ ((uint64_t)(-off) >= wid)
		  തിരികെ അസത്യം;

	      // If the index is below the vector, then only assign the high
	      // bits that overlap with the target
	    base = -off;
	    wid += off;
		off = 0;
      }

	// If the value is partly above the target, then only assign
	// the bits that overlap
      എങ്കിൽ ((uint64_t)off + wid > sig_wid)
	    wid = sig_wid - (uint64_t)off;

      val = val.subvalue(base, wid);

      തിരികെ സത്യം;
}

/*
 * %assign/vec4/a/d <arr>, <offx>, <delx>
 */
തർക്കം of_ASSIGN_VEC4_A_D(vthread_t thr, vvp_code_t cp)
{
      പൂർണ്ണസംഖ്യ off_idx = cp->bit_idx[0];
      പൂർണ്ണസംഖ്യ del_idx = cp->bit_idx[1];
      പൂർണ്ണസംഖ്യ adr_idx = 3;

      int64_t  off = off_idx ? thr->വാക്കുകൾ[off_idx].w_int : 0;
      vvp_time64_t del = del_idx? thr->വാക്കുകൾ[del_idx].w_uint : 0;
      long     adr = thr->വാക്കുകൾ[adr_idx].w_int;

      vvp_vector4_t val = thr->എടുക്കുക_vec4();

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      എങ്കിൽ (thr->flags[4] == BIT4_1)
	    തിരികെ സത്യം;

      എങ്കിൽ (!resize_rval_vec(val, off, cp->array->get_word_size()))
	    തിരികെ സത്യം;

      schedule_assign_array_word(cp->array, adr, off, val, del);

      തിരികെ സത്യം;
}

/*
 * %assign/vec4/a/e <arr>, <offx>
 */
തർക്കം of_ASSIGN_VEC4_A_E(vthread_t thr, vvp_code_t cp)
{
      പൂർണ്ണസംഖ്യ off_idx = cp->bit_idx[0];
      പൂർണ്ണസംഖ്യ adr_idx = 3;

      int64_t  off = off_idx ? thr->വാക്കുകൾ[off_idx].w_int : 0;
      long     adr = thr->വാക്കുകൾ[adr_idx].w_int;

      vvp_vector4_t val = thr->എടുക്കുക_vec4();

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      എങ്കിൽ (thr->flags[4] == BIT4_1)
	    തിരികെ സത്യം;

      എങ്കിൽ (!resize_rval_vec(val, off, cp->array->get_word_size()))
	    തിരികെ സത്യം;

      എങ്കിൽ (thr->ecount == 0) {
	    schedule_assign_array_word(cp->array, adr, off, val, 0);
      } അല്ലെങ്കിൽ {
	    schedule_evctl(cp->array, adr, val, off, thr->event, thr->ecount);
      }

      തിരികെ സത്യം;
}

/*
 * %assign/vec4/off/d <var>, <off>, <del>
 */
തർക്കം of_ASSIGN_VEC4_OFF_D(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      unsigned off_index = cp->bit_idx[0];
      unsigned del_index = cp->bit_idx[1];
      vvp_vector4_t val = thr->എടുക്കുക_vec4();

      int64_t off = thr->വാക്കുകൾ[off_index].w_int;
      vvp_time64_t del = thr->വാക്കുകൾ[del_index].w_uint;

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      എങ്കിൽ (thr->flags[4] == BIT4_1)
	    തിരികെ സത്യം;

      vvp_signal_മൂല്യം*sig = dynamic_cast<vvp_signal_മൂല്യം*> (cp->net->fil);
      പ്രതിജ്ഞാ(sig);

      എങ്കിൽ (!resize_rval_vec(val, off, sig->മൂല്യം_size()))
	    തിരികെ സത്യം;

      schedule_assign_vector(ptr, off, sig->മൂല്യം_size(), val, del);
      തിരികെ സത്യം;
}

/*
 * %assign/vec4/off/e <var>, <off>
 */
തർക്കം of_ASSIGN_VEC4_OFF_E(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      unsigned off_index = cp->bit_idx[0];
      vvp_vector4_t val = thr->എടുക്കുക_vec4();

      int64_t off = thr->വാക്കുകൾ[off_index].w_int;

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      എങ്കിൽ (thr->flags[4] == BIT4_1)
	    തിരികെ സത്യം;

      vvp_signal_മൂല്യം*sig = dynamic_cast<vvp_signal_മൂല്യം*> (cp->net->fil);
      പ്രതിജ്ഞാ(sig);

      എങ്കിൽ (!resize_rval_vec(val, off, sig->മൂല്യം_size()))
	    തിരികെ സത്യം;

      എങ്കിൽ (thr->ecount == 0) {
	    schedule_assign_vector(ptr, off, sig->മൂല്യം_size(), val, 0);
      } അല്ലെങ്കിൽ {
	    schedule_evctl(ptr, val, off, sig->മൂല്യം_size(), thr->event, thr->ecount);
      }

      തിരികെ സത്യം;
}

/*
 * %assign/vec4/d <var-label> <delay>
 */
തർക്കം of_ASSIGN_VEC4D(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      unsigned del_index = cp->bit_idx[0];
      vvp_time64_t del = thr->വാക്കുകൾ[del_index].w_int;

      vvp_vector4_t value = thr->എടുക്കുക_vec4();

      vvp_signal_മൂല്യം*sig = dynamic_cast<vvp_signal_മൂല്യം*> (cp->net->fil);
      പ്രതിജ്ഞാ(sig);

      schedule_assign_vector(ptr, 0, sig->മൂല്യം_size(), value, del);

      തിരികെ സത്യം;
}

/*
 * %assign/vec4/e <var-label>
 */
തർക്കം of_ASSIGN_VEC4E(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_vector4_t value = thr->എടുക്കുക_vec4();

      vvp_signal_മൂല്യം*sig = dynamic_cast<vvp_signal_മൂല്യം*> (cp->net->fil);
      പ്രതിജ്ഞാ(sig);

      എങ്കിൽ (thr->ecount == 0) {
	    schedule_assign_vector(ptr, 0, sig->മൂല്യം_size(), value, 0);
      } അല്ലെങ്കിൽ {
	    schedule_evctl(ptr, value, 0, sig->മൂല്യം_size(), thr->event, thr->ecount);
      }

      തിരികെ സത്യം;
}

/*
 * This is %assign/wr <vpi-label>, <delay>
 *
 * This assigns (after a delay) a value to a real variable. Use the
 * vpi_put_value function to do the assign, with the delay written
 * into the vpiInertialDelay carrying the desired delay.
 */
തർക്കം of_ASSIGN_WR(vthread_t thr, vvp_code_t cp)
{
      unsigned delay = cp->bit_idx[0];
      ഇരട്ട value = thr->എടുക്കുക_real();
      s_vpi_time del;

      del.type = vpiSimTime;
      vpip_time_to_timestruct(&del, delay);

      __vpiHandle*tmp = cp->handle;

      t_vpi_മൂല്യം val;
      val.format = vpiRealVal;
      val.value.real = value;
      vpi_put_മൂല്യം(tmp, &val, &del, vpiTransportDelay);

      തിരികെ സത്യം;
}

തർക്കം of_ASSIGN_WRD(vthread_t thr, vvp_code_t cp)
{
      vvp_time64_t delay = thr->വാക്കുകൾ[cp->bit_idx[0]].w_uint;
      ഇരട്ട value = thr->എടുക്കുക_real();
      s_vpi_time del;

      del.type = vpiSimTime;
      vpip_time_to_timestruct(&del, delay);

      __vpiHandle*tmp = cp->handle;

      t_vpi_മൂല്യം val;
      val.format = vpiRealVal;
      val.value.real = value;
      vpi_put_മൂല്യം(tmp, &val, &del, vpiTransportDelay);

      തിരികെ സത്യം;
}

തർക്കം of_ASSIGN_WRE(vthread_t thr, vvp_code_t cp)
{
      പ്രതിജ്ഞാ(thr->event != 0);
      ഇരട്ട value = thr->എടുക്കുക_real();
      __vpiHandle*tmp = cp->handle;

	// If the count is zero then just put the value.
      എങ്കിൽ (thr->ecount == 0) {
	    t_vpi_മൂല്യം val;

	    val.format = vpiRealVal;
	    val.value.real = value;
	    vpi_put_മൂല്യം(tmp, &val, 0, vpiNoDelay);
      } അല്ലെങ്കിൽ {
	    schedule_evctl(tmp, value, thr->event, thr->ecount);
      }

      thr->event = 0;
      thr->ecount = 0;

      തിരികെ സത്യം;
}

തർക്കം of_BLEND(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t vala = thr->എടുക്കുക_vec4();
      vvp_vector4_t valb = thr->എടുക്കുക_vec4();
      പ്രതിജ്ഞാ(vala.size() == valb.size());

      വേണ്ടി (unsigned idx = 0 ; idx < vala.size() ; idx += 1) {
	    എങ്കിൽ (vala.value(idx) == valb.value(idx))
		  തുടരുക;

	    vala.set_bit(idx, BIT4_X);
      }

      thr->തള്ളുക_vec4(vala);
      തിരികെ സത്യം;
}

തർക്കം of_BLEND_STR(vthread_t thr, vvp_code_t)
{
      string f = thr->എടുക്കുക_str();
      string t = thr->എടുക്കുക_str();
      thr->തള്ളുക_str((t == f) ? t : "");
      തിരികെ സത്യം;
}

തർക്കം of_BLEND_WR(vthread_t thr, vvp_code_t)
{
      ഇരട്ട f = thr->എടുക്കുക_real();
      ഇരട്ട t = thr->എടുക്കുക_real();
      thr->തള്ളുക_real((t == f) ? t : 0.0);
      തിരികെ സത്യം;
}

തർക്കം of_BREAKPOINT(vthread_t, vvp_code_t)
{
      തിരികെ സത്യം;
}

/*
 * %callf/void <code-label>, <scope-label>
 * Combine the %fork and %join steps for invoking a function.
 */
സ്ഥിതം തർക്കം do_callf_void(vthread_t thr, vthread_t child)
{

      എങ്കിൽ (child->parent_scope->is_automatic()) {
	      /* The context allocated for this child is the top entry
		 on the write context stack */
	    child->wt_context = thr->wt_context;
	    child->rd_context = thr->wt_context;
      }

        // Mark the function thread as a direct child of the current thread.
      child->parent = thr;
      thr->children.insert(child);
        // This should be the only child
      പ്രതിജ്ഞാ(thr->children.size()==1);

        // Execute the function. This SHOULD run the function to completion,
        // but there are some exceptional situations where it won't.
      പ്രതിജ്ഞാ(child->parent_scope->get_type_code() == vpiFunction);
      child->is_scheduled = 1;
      child->i_am_in_function = 1;
      vthread_run(child);
      running_thread = thr;

      എങ്കിൽ (child->i_have_ended) {
	    do_join(thr, child);
	    തിരികെ സത്യം;
      } അല്ലെങ്കിൽ {
	    thr->i_am_joining = 1;
	    തിരികെ അസത്യം;
      }
}

തർക്കം of_CALLF_OBJ(vthread_t thr, vvp_code_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);
      തിരികെ do_callf_void(thr, child);

      // XXXX NOT IMPLEMENTED
}

തർക്കം of_CALLF_REAL(vthread_t thr, vvp_code_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);

	// This is the return value. Push a place-holder value. The function
	// will replace this with the actual value using a %ret/real instruction.
      thr->തള്ളുക_real(0.0);
      child->args_real.push_back(0);

      തിരികെ do_callf_void(thr, child);
}

തർക്കം of_CALLF_STR(vthread_t thr, vvp_code_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);

      thr->തള്ളുക_str("");
      child->args_str.push_back(0);

      തിരികെ do_callf_void(thr, child);
}

തർക്കം of_CALLF_VEC4(vthread_t thr, vvp_code_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);

      vpiScopeFunction*scope_func = dynamic_cast<vpiScopeFunction*>(cp->scope);
      പ്രതിജ്ഞാ(scope_func);

	// This is the return value. Push a place-holder value. The function
	// will replace this with the actual value using a %ret/real instruction.
      thr->തള്ളുക_vec4(vvp_vector4_t(scope_func->get_func_width(), scope_func->get_func_init_val()));
      child->args_vec4.push_back(0);

      തിരികെ do_callf_void(thr, child);
}

തർക്കം of_CALLF_VOID(vthread_t thr, vvp_code_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);
      തിരികെ do_callf_void(thr, child);
}

/*
 * The %cassign/link instruction connects a source node to a
 * destination node. The destination node must be a signal, as it is
 * marked with the source of the cassign so that it may later be
 * unlinked without specifically knowing the source that this
 * instruction used.
 */
തർക്കം of_CASSIGN_LINK(vthread_t, vvp_code_t cp)
{
      vvp_net_t*dst = cp->net;
      vvp_net_t*src = cp->net2;

      vvp_fun_signal_base*sig
	    = dynamic_cast<vvp_fun_signal_base*>(dst->fun);
      പ്രതിജ്ഞാ(sig);

	/* Any previous continuous assign should have been removed already. */
      പ്രതിജ്ഞാ(sig->cassign_link == 0);

      sig->cassign_link = src;

	/* Link the output of the src to the port[1] (the cassign
	   port) of the destination. */
      vvp_net_ptr_t dst_ptr (dst, 1);
      src->link(dst_ptr);

      തിരികെ സത്യം;
}

/*
 * If there is an existing continuous assign linked to the destination
 * node, unlink it. This must be done before applying a new continuous
 * assign, otherwise the initial assigned value will be propagated to
 * any other nodes driven by the old continuous assign source.
 */
സ്ഥിതം ശൂന്യം cassign_unlink(vvp_net_t*dst)
{
      vvp_fun_signal_base*sig
	    = dynamic_cast<vvp_fun_signal_base*>(dst->fun);
      പ്രതിജ്ഞാ(sig);

      എങ്കിൽ (sig->cassign_link == 0)
	    തിരികെ;

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
തർക്കം of_CASSIGN_VEC4(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      vvp_vector4_t value = thr->എടുക്കുക_vec4();

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

	/* Set the value into port 1 of the destination. */
      vvp_net_ptr_t ptr (net, 1);
      vvp_send_vec4(ptr, value, 0);

      തിരികെ സത്യം;
}

/*
 * %cassign/vec4/off <var>, <off>
 */
തർക്കം of_CASSIGN_VEC4_OFF(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      unsigned base_idx = cp->bit_idx[0];
      long base = thr->വാക്കുകൾ[base_idx].w_int;
      vvp_vector4_t value = thr->എടുക്കുക_vec4();
      unsigned wid = value.size();

      എങ്കിൽ (thr->flags[4] == BIT4_1)
	    തിരികെ സത്യം;

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

      vvp_signal_മൂല്യം*sig = dynamic_cast<vvp_signal_മൂല്യം*> (net->fil);
      പ്രതിജ്ഞാ(sig);

      എങ്കിൽ (base < 0 && (wid <= (unsigned)-base))
	    തിരികെ സത്യം;

      എങ്കിൽ (base >= (long)sig->മൂല്യം_size())
	    തിരികെ സത്യം;

      എങ്കിൽ (base < 0) {
	    wid -= (unsigned) -base;
	    base = 0;
	    value.resize(wid);
      }

      എങ്കിൽ (base+wid > sig->മൂല്യം_size()) {
	    wid = sig->മൂല്യം_size() - base;
	    value.resize(wid);
      }

      vvp_net_ptr_t ptr (net, 1);
      vvp_send_vec4_pv(ptr, value, base, sig->മൂല്യം_size(), 0);
      തിരികെ സത്യം;
}

തർക്കം of_CASSIGN_WR(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net  = cp->net;
      ഇരട്ട value = thr->എടുക്കുക_real();

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

	/* Set the value into port 1 of the destination. */
      vvp_net_ptr_t ptr (net, 1);
      vvp_send_real(ptr, value, 0);

      തിരികെ സത്യം;
}

/*
 * %cast2
 */
തർക്കം of_CAST2(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      unsigned wid = val.size();

      വേണ്ടി (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    തിരഞ്ഞെടുപ്പ് (val.value(idx)) {
		അവസ്ഥ BIT4_0:
		അവസ്ഥ BIT4_1:
		  നിർത്തുക;
		സ്ഥിരസ്ഥിതി:
		  val.set_bit(idx, BIT4_0);
		  നിർത്തുക;
	    }
      }

      തിരികെ സത്യം;
}

തർക്കം do_cast_vec_dar(vthread_t thr, vvp_code_t cp, തർക്കം as_vec4)
{
      unsigned wid = cp->സംഖ്യ;

      vvp_object_t obj;
      thr->എടുക്കുക_object(obj);

      vvp_darray*darray = obj.peek<vvp_darray>();
      പ്രതിജ്ഞാ(darray);

      vvp_vector4_t vec = darray->get_bitstream(as_vec4);
      എങ്കിൽ (vec.size() != wid) {
	    cerr << thr->get_fileline()
	         << "VVP error: size mismatch when casting dynamic array to vector." << endl;
            thr->തള്ളുക_vec4(vvp_vector4_t(wid));
            schedule_stop(0);
            തിരികെ അസത്യം;
      }
      thr->തള്ളുക_vec4(vec);
      തിരികെ സത്യം;
}

/*
 * %cast/vec2/dar <wid>
 */
തർക്കം of_CAST_VEC2_DAR(vthread_t thr, vvp_code_t cp)
{
      തിരികെ do_cast_vec_dar(thr, cp, അസത്യം);
}

/*
 * %cast/vec4/dar <wid>
 */
തർക്കം of_CAST_VEC4_DAR(vthread_t thr, vvp_code_t cp)
{
      തിരികെ do_cast_vec_dar(thr, cp, സത്യം);
}

/*
 * %cast/vec4/str <wid>
 */
തർക്കം of_CAST_VEC4_STR(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->സംഖ്യ;
      string str = thr->എടുക്കുക_str();

      vvp_vector4_t vec(wid, BIT4_0);

      എങ്കിൽ (wid != 8*str.length()) {
	    cerr << thr->get_fileline()
	         << "VVP error: size mismatch when casting string to vector." << endl;
            thr->തള്ളുക_vec4(vec);
            schedule_stop(0);
            തിരികെ അസത്യം;
      }

      unsigned sdx = 0;
      unsigned vdx = wid;
      ആകുമ്പോൾ (vdx > 0) {
            അക്ഷരം ch = str[sdx++];
            vdx -= 8;
            വേണ്ടി (unsigned bdx = 0; bdx < 8; bdx += 1) {
                  എങ്കിൽ (ch & 1)
                        vec.set_bit(vdx+bdx, BIT4_1);
                  ch >>= 1;
            }
      }

      thr->തള്ളുക_vec4(vec);
      തിരികെ സത്യം;
}

സ്ഥിതം ശൂന്യം do_CMPE(vthread_t thr, സ്ഥിരം vvp_vector4_t&lval, സ്ഥിരം vvp_vector4_t&rval)
{
      പ്രതിജ്ഞാ(rval.size() == lval.size());

      എങ്കിൽ (lval.has_xz() || rval.has_xz()) {

	    unsigned wid = lval.size();
	    vvp_bit4_t eq  = BIT4_1;
	    vvp_bit4_t eeq = BIT4_1;

	    വേണ്ടി (unsigned idx = 0 ; idx < wid ; idx += 1) {
		  vvp_bit4_t lv = lval.value(idx);
		  vvp_bit4_t rv = rval.value(idx);

		  എങ്കിൽ (lv != rv)
			eeq = BIT4_0;

		  എങ്കിൽ (eq==BIT4_1 && (bit4_is_xz(lv) || bit4_is_xz(rv)))
			eq = BIT4_X;
		  എങ്കിൽ ((lv == BIT4_0) && (rv==BIT4_1))
			eq = BIT4_0;
		  എങ്കിൽ ((lv == BIT4_1) && (rv==BIT4_0))
			eq = BIT4_0;

		  എങ്കിൽ (eq == BIT4_0)
			നിർത്തുക;
	    }

	    thr->flags[4] = eq;
	    thr->flags[6] = eeq;

      } അല്ലെങ്കിൽ {
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
തർക്കം of_CMPE(vthread_t thr, vvp_code_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      സ്ഥിരം vvp_vector4_t&rval = thr->peek_vec4(0);
      സ്ഥിരം vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPE(thr, lval, rval);

      thr->എടുക്കുക_vec4(2);
      തിരികെ സത്യം;
}

തർക്കം of_CMPNE(vthread_t thr, vvp_code_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      സ്ഥിരം vvp_vector4_t&rval = thr->peek_vec4(0);
      സ്ഥിരം vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];
      thr->flags[6] =  ~thr->flags[6];

      thr->എടുക്കുക_vec4(2);
      തിരികെ സത്യം;
}

/*
 * %cmpi/e <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
തർക്കം of_CMPIE(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->സംഖ്യ;

      സ്ഥിരം vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_immediate_rval (cp, rval);

      do_CMPE(thr, lval, rval);

      thr->എടുക്കുക_vec4(1);
      തിരികെ സത്യം;
}

തർക്കം of_CMPINE(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->സംഖ്യ;

      സ്ഥിരം vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_immediate_rval (cp, rval);

      do_CMPE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];
      thr->flags[6] =  ~thr->flags[6];

      thr->എടുക്കുക_vec4(1);
      തിരികെ സത്യം;
}



സ്ഥിതം ശൂന്യം do_CMPS(vthread_t thr, സ്ഥിരം vvp_vector4_t&lval, സ്ഥിരം vvp_vector4_t&rval)
{
      പ്രതിജ്ഞാ(rval.size() == lval.size());

	// If either value has XZ bits, then the eq and lt values are
	// known already to be X. Just calculate the eeq result as a
	// special case and short circuit the rest of the compare.
      എങ്കിൽ (lval.has_xz() || rval.has_xz()) {
	    thr->flags[4] = BIT4_X; // eq
	    thr->flags[5] = BIT4_X; // lt
	    thr->flags[6] = lval.eeq(rval)? BIT4_1 : BIT4_0;
	    തിരികെ;
      }

	// Past this point, we know we are dealing only with fully
	// defined values.
      unsigned wid = lval.size();

      സ്ഥിരം vvp_bit4_t sig1 = lval.value(wid-1);
      സ്ഥിരം vvp_bit4_t sig2 = rval.value(wid-1);

	// If the lval is <0 and the rval is >=0, then we know the result.
      എങ്കിൽ ((sig1 == BIT4_1) && (sig2 == BIT4_0)) {
	    thr->flags[4] = BIT4_0; // eq;
	    thr->flags[5] = BIT4_1; // lt;
	    thr->flags[6] = BIT4_0; // eeq
	    തിരികെ;
      }

	// If the lval is >=0 and the rval is <0, then we know the result.
      എങ്കിൽ ((sig1 == BIT4_0) && (sig2 == BIT4_1)) {
	    thr->flags[4] = BIT4_0; // eq;
	    thr->flags[5] = BIT4_0; // lt;
	    thr->flags[6] = BIT4_0; // eeq
	    തിരികെ;
      }

	// The values have the same sign, so we have to look at the
	// actual value. Scan from the MSB down. As soon as we find a
	// bit that differs, we know the result.

      വേണ്ടി (unsigned idx = 1 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t lv = lval.value(wid-1-idx);
	    vvp_bit4_t rv = rval.value(wid-1-idx);

	    എങ്കിൽ (lv == rv)
		  തുടരുക;

	    thr->flags[4] = BIT4_0; // eq
	    thr->flags[6] = BIT4_0; // eeq

	    എങ്കിൽ (lv==BIT4_0) {
		  thr->flags[5] = BIT4_1; // lt
	    } അല്ലെങ്കിൽ {
		  thr->flags[5] = BIT4_0; // lt
	    }
	    തിരികെ;
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
തർക്കം of_CMPS(vthread_t thr, vvp_code_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      സ്ഥിരം vvp_vector4_t&rval = thr->peek_vec4(0);
      സ്ഥിരം vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPS(thr, lval, rval);

      thr->എടുക്കുക_vec4(2);
      തിരികെ സത്യം;
}

/*
 * %cmpi/s <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
തർക്കം of_CMPIS(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->സംഖ്യ;

      സ്ഥിരം vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_immediate_rval (cp, rval);

      do_CMPS(thr, lval, rval);

      thr->എടുക്കുക_vec4(1);
      തിരികെ സത്യം;
}

തർക്കം of_CMPSTR(vthread_t thr, vvp_code_t)
{
      string re = thr->എടുക്കുക_str();
      string le = thr->എടുക്കുക_str();

      പൂർണ്ണസംഖ്യ rc = strcmp(le.c_str(), re.c_str());

      vvp_bit4_t eq;
      vvp_bit4_t lt;

      എങ്കിൽ (rc == 0) {
	    eq = BIT4_1;
	    lt = BIT4_0;
      } അല്ലെങ്കിൽ എങ്കിൽ (rc < 0) {
	    eq = BIT4_0;
	    lt = BIT4_1;
      } അല്ലെങ്കിൽ {
	    eq = BIT4_0;
	    lt = BIT4_0;
      }

      thr->flags[4] = eq;
      thr->flags[5] = lt;

      തിരികെ സത്യം;
}

സ്ഥിതം ശൂന്യം of_CMPU_the_hard_way(vthread_t thr, unsigned wid,
				 സ്ഥിരം vvp_vector4_t&lval,
				 സ്ഥിരം vvp_vector4_t&rval)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_bit4_t eeq = BIT4_1;

      വേണ്ടി (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t lv = lval.value(idx);
	    vvp_bit4_t rv = rval.value(idx);

	    എങ്കിൽ (lv != rv)
		  eeq = BIT4_0;

	    എങ്കിൽ (eq==BIT4_1 && (bit4_is_xz(lv) || bit4_is_xz(rv)))
		  eq = BIT4_X;
	    എങ്കിൽ ((lv == BIT4_0) && (rv==BIT4_1))
		  eq = BIT4_0;
	    എങ്കിൽ ((lv == BIT4_1) && (rv==BIT4_0))
		  eq = BIT4_0;

	    എങ്കിൽ (eq == BIT4_0)
		  നിർത്തുക;

      }

      thr->flags[4] = eq;
      thr->flags[5] = BIT4_X;
      thr->flags[6] = eeq;
}

സ്ഥിതം ശൂന്യം do_CMPU(vthread_t thr, സ്ഥിരം vvp_vector4_t&lval, സ്ഥിരം vvp_vector4_t&rval)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_bit4_t lt = BIT4_0;

      എങ്കിൽ (rval.size() != lval.size()) {
	    cerr << thr->get_fileline()
	         << "VVP ERROR: %cmp/u operand width mismatch: lval=" << lval
		 << ", rval=" << rval << endl;
      }
      പ്രതിജ്ഞാ(rval.size() == lval.size());
      unsigned wid = lval.size();

      unsigned long*larray = lval.subarray(0,wid);
      എങ്കിൽ (larray == 0) തിരികെ of_CMPU_the_hard_way(thr, wid, lval, rval);

      unsigned long*rarray = rval.subarray(0,wid);
      എങ്കിൽ (rarray == 0) {
	    delete[]larray;
	    തിരികെ of_CMPU_the_hard_way(thr, wid, lval, rval);
      }

      unsigned വാക്കുകൾ = (wid+CPU_WORD_BITS-1) / CPU_WORD_BITS;

      വേണ്ടി (unsigned wdx = 0 ; wdx < വാക്കുകൾ ; wdx += 1) {
	    എങ്കിൽ (larray[wdx] == rarray[wdx])
		  തുടരുക;

	    eq = BIT4_0;
	    എങ്കിൽ (larray[wdx] < rarray[wdx])
		  lt = BIT4_1;
	    അല്ലെങ്കിൽ
		  lt = BIT4_0;
      }

      delete[]larray;
      delete[]rarray;

      thr->flags[4] = eq;
      thr->flags[5] = lt;
      thr->flags[6] = eq;
}

തർക്കം of_CMPU(vthread_t thr, vvp_code_t)
{

      സ്ഥിരം vvp_vector4_t&rval = thr->peek_vec4(0);
      സ്ഥിരം vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPU(thr, lval, rval);

      thr->എടുക്കുക_vec4(2);
      തിരികെ സത്യം;
}

/*
 * %cmpi/u <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
തർക്കം of_CMPIU(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->സംഖ്യ;

      സ്ഥിരം vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_immediate_rval (cp, rval);

      do_CMPU(thr, lval, rval);

      thr->എടുക്കുക_vec4(1);
      തിരികെ സത്യം;
}


/*
 * %cmp/x
 */
തർക്കം of_CMPX(vthread_t thr, vvp_code_t)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_vector4_t rval = thr->എടുക്കുക_vec4();
      vvp_vector4_t lval = thr->എടുക്കുക_vec4();

      പ്രതിജ്ഞാ(rval.size() == lval.size());
      unsigned wid = lval.size();

      വേണ്ടി (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    vvp_bit4_t lv = lval.value(idx);
	    vvp_bit4_t rv = rval.value(idx);
	    എങ്കിൽ ((lv != rv) && !bit4_is_xz(lv) && !bit4_is_xz(rv)) {
		  eq = BIT4_0;
		  നിർത്തുക;
	    }
      }

      thr->flags[4] = eq;
      തിരികെ സത്യം;
}

സ്ഥിതം ശൂന്യം do_CMPWE(vthread_t thr, സ്ഥിരം vvp_vector4_t&lval, സ്ഥിരം vvp_vector4_t&rval)
{
      പ്രതിജ്ഞാ(rval.size() == lval.size());

      എങ്കിൽ (lval.has_xz() || rval.has_xz()) {

	    unsigned wid = lval.size();
	    vvp_bit4_t eq  = BIT4_1;

	    വേണ്ടി (unsigned idx = 0 ; idx < wid ; idx += 1) {
		  vvp_bit4_t lv = lval.value(idx);
		  vvp_bit4_t rv = rval.value(idx);

		  എങ്കിൽ (bit4_is_xz(rv))
			തുടരുക;
		  എങ്കിൽ ((eq == BIT4_1) && bit4_is_xz(lv))
			eq = BIT4_X;
		  എങ്കിൽ ((lv == BIT4_0) && (rv==BIT4_1))
			eq = BIT4_0;
		  എങ്കിൽ ((lv == BIT4_1) && (rv==BIT4_0))
			eq = BIT4_0;

		  എങ്കിൽ (eq == BIT4_0)
			നിർത്തുക;
	    }

	    thr->flags[4] = eq;

      } അല്ലെങ്കിൽ {
	      // If there are no XZ bits anywhere, then the results of
	      // ==? match the === test.
	    thr->flags[4] = (lval.eeq(rval)? BIT4_1 : BIT4_0);
      }
}

തർക്കം of_CMPWE(vthread_t thr, vvp_code_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      സ്ഥിരം vvp_vector4_t&rval = thr->peek_vec4(0);
      സ്ഥിരം vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPWE(thr, lval, rval);

      thr->എടുക്കുക_vec4(2);
      തിരികെ സത്യം;
}

തർക്കം of_CMPWNE(vthread_t thr, vvp_code_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      സ്ഥിരം vvp_vector4_t&rval = thr->peek_vec4(0);
      സ്ഥിരം vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPWE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];

      thr->എടുക്കുക_vec4(2);
      തിരികെ സത്യം;
}

തർക്കം of_CMPWR(vthread_t thr, vvp_code_t)
{
      ഇരട്ട r = thr->എടുക്കുക_real();
      ഇരട്ട l = thr->എടുക്കുക_real();

      vvp_bit4_t eq = (l == r)? BIT4_1 : BIT4_0;
      vvp_bit4_t lt = (l <  r)? BIT4_1 : BIT4_0;

      thr->flags[4] = eq;
      thr->flags[5] = lt;

      തിരികെ സത്യം;
}

/*
 * %cmp/z
 */
തർക്കം of_CMPZ(vthread_t thr, vvp_code_t)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_vector4_t rval = thr->എടുക്കുക_vec4();
      vvp_vector4_t lval = thr->എടുക്കുക_vec4();

      പ്രതിജ്ഞാ(rval.size() == lval.size());
      unsigned wid = lval.size();

      വേണ്ടി (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    vvp_bit4_t lv = lval.value(idx);
	    vvp_bit4_t rv = rval.value(idx);
	    എങ്കിൽ ((lv != rv) && (rv != BIT4_Z) && (lv != BIT4_Z)) {
		  eq = BIT4_0;
		  നിർത്തുക;
	    }
      }

      thr->flags[4] = eq;
      തിരികെ സത്യം;
}

/*
 *  %concat/str;
 */
തർക്കം of_CONCAT_STR(vthread_t thr, vvp_code_t)
{
      string text = thr->എടുക്കുക_str();
      thr->peek_str(0).append(text);
      തിരികെ സത്യം;
}

/*
 *  %concati/str <string>;
 */
തർക്കം of_CONCATI_STR(vthread_t thr, vvp_code_t cp)
{
      സ്ഥിരം അക്ഷരം*text = cp->text;
      thr->peek_str(0).append(filter_string(text));
      തിരികെ സത്യം;
}

/*
 * %concat/vec4
 */
തർക്കം of_CONCAT_VEC4(vthread_t thr, vvp_code_t)
{
      സ്ഥിരം vvp_vector4_t&lsb = thr->peek_vec4(0);
      സ്ഥിരം vvp_vector4_t&msb = thr->peek_vec4(1);

	// The result is the size of the top two vectors in the stack.
      vvp_vector4_t res (msb.size() + lsb.size(), BIT4_X);

	// Build up the result.
      res.set_vec(0, lsb);
      res.set_vec(lsb.size(), msb);

	// Rearrange the stack to pop the inputs and push the
	// result. Do that by actually popping only 1 stack position
	// and replacing the new top with the new value.
      thr->എടുക്കുക_vec4(1);
      thr->peek_vec4() = res;

      തിരികെ സത്യം;
}

/*
 * %concati/vec4 <vala>, <valb>, <wid>
 *
 * Concat the immediate value to the LOW bits of the concatenation.
 * Get the HIGH bits from the top of the vec4 stack.
 */
തർക്കം of_CONCATI_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned wid  = cp->സംഖ്യ;

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
      തിരികെ സത്യം;
}

/*
 * %cvt/rv
 */
തർക്കം of_CVT_RV(vthread_t thr, vvp_code_t)
{
      ഇരട്ട val;
      vvp_vector4_t val4 = thr->എടുക്കുക_vec4();
      vector4_to_മൂല്യം(val4, val, അസത്യം);
      thr->തള്ളുക_real(val);
      തിരികെ സത്യം;
}

/*
 * %cvt/rv/s
 */
തർക്കം of_CVT_RV_S(vthread_t thr, vvp_code_t)
{
      ഇരട്ട val;
      vvp_vector4_t val4 = thr->എടുക്കുക_vec4();
      vector4_to_മൂല്യം(val4, val, സത്യം);
      thr->തള്ളുക_real(val);
      തിരികെ സത്യം;
}

/*
 * %cvt/sr <idx>
 * Pop the top value from the real stack, convert it to a 64bit signed
 * and save it to the indexed register.
 */
തർക്കം of_CVT_SR(vthread_t thr, vvp_code_t cp)
{
      ഇരട്ട r = thr->എടുക്കുക_real();
      thr->വാക്കുകൾ[cp->bit_idx[0]].w_int = i64round(r);

      തിരികെ സത്യം;
}

/*
 * %cvt/ur <idx>
 */
തർക്കം of_CVT_UR(vthread_t thr, vvp_code_t cp)
{
      ഇരട്ട r = thr->എടുക്കുക_real();
      thr->വാക്കുകൾ[cp->bit_idx[0]].w_uint = vlg_round_to_u64(r);

      തിരികെ സത്യം;
}

/*
 * %cvt/vr <wid>
 */
തർക്കം of_CVT_VR(vthread_t thr, vvp_code_t cp)
{
      ഇരട്ട r = thr->എടുക്കുക_real();
      unsigned wid = cp->സംഖ്യ;

      vvp_vector4_t tmp(wid, r);
      thr->തള്ളുക_vec4(tmp);
      തിരികെ സത്യം;
}

/*
 * This implements the %deassign instruction. All we do is write a
 * long(1) to port-3 of the addressed net. This turns off an active
 * continuous assign activated by %cassign/v
 */
തർക്കം of_DEASSIGN(vthread_t, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      unsigned base  = cp->bit_idx[0];
      unsigned width = cp->bit_idx[1];

      vvp_signal_മൂല്യം*fil = dynamic_cast<vvp_signal_മൂല്യം*> (net->fil);
      പ്രതിജ്ഞാ(fil);
      vvp_fun_signal_vec*sig = dynamic_cast<vvp_fun_signal_vec*>(net->fun);
      പ്രതിജ്ഞാ(sig);

      എങ്കിൽ (base >= fil->മൂല്യം_size()) തിരികെ സത്യം;
      എങ്കിൽ (base+width > fil->മൂല്യം_size()) width = fil->മൂല്യം_size() - base;

      തർക്കം full_sig = base == 0 && width == fil->മൂല്യം_size();

	// This is the net that is forcing me...
      എങ്കിൽ (vvp_net_t*src = sig->cassign_link) {
	    എങ്കിൽ (! full_sig) {
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
      എങ്കിൽ (full_sig) {
	    sig->deassign();
      } അല്ലെങ്കിൽ {
	    sig->deassign_pv(base, width);
      }

      തിരികെ സത്യം;
}

തർക്കം of_DEASSIGN_WR(vthread_t, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_real*sig = dynamic_cast<vvp_fun_signal_real*>(net->fun);
      പ്രതിജ്ഞാ(sig);

	// This is the net that is forcing me...
      എങ്കിൽ (vvp_net_t*src = sig->cassign_link) {
	      // And this is the pointer to be removed.
	    vvp_net_ptr_t dst_ptr (net, 1);
	    src->unlink(dst_ptr);
	    sig->cassign_link = 0;
      }

      sig->deassign();

      തിരികെ സത്യം;
}

/*
 * %debug/thr
 */
തർക്കം of_DEBUG_THR(vthread_t thr, vvp_code_t cp)
{
      സ്ഥിരം അക്ഷരം*text = cp->text;
      thr->debug_dump(cerr, text);
      തിരികെ സത്യം;
}

/*
 * The delay takes two 32bit numbers to make up a 64bit time.
 *
 *   %delay <low>, <hig>
 */
തർക്കം of_DELAY(vthread_t thr, vvp_code_t cp)
{
      vvp_time64_t താഴെയുള്ള = cp->bit_idx[0];
      vvp_time64_t hig = cp->bit_idx[1];

      vvp_time64_t delay = (hig << 32) | താഴെയുള്ള;

      എങ്കിൽ (delay == 0) schedule_inactive(thr);
      അല്ലെങ്കിൽ schedule_vthread(thr, delay);
      തിരികെ അസത്യം;
}

തർക്കം of_DELAYX(vthread_t thr, vvp_code_t cp)
{
      vvp_time64_t delay;

      പ്രതിജ്ഞാ(cp->സംഖ്യ < vthread_s::വാക്കുകൾ_എണ്ണം);
      delay = thr->വാക്കുകൾ[cp->സംഖ്യ].w_uint;
      എങ്കിൽ (delay == 0) schedule_inactive(thr);
      അല്ലെങ്കിൽ schedule_vthread(thr, delay);
      തിരികെ അസത്യം;
}

തർക്കം of_DELETE_ELEM(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      int64_t idx_val = thr->വാക്കുകൾ[3].w_int;
      എങ്കിൽ (thr->flags[4] == BIT4_1) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with undefined index."
	         << endl;
	    തിരികെ സത്യം;
      }
      എങ്കിൽ (idx_val < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with negative index."
	         << endl;
	    തിരികെ സത്യം;
      }
      size_t idx = idx_val;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      പ്രതിജ്ഞാ(obj);

      vvp_queue*queue = obj->get_object().peek<vvp_queue>();
      എങ്കിൽ (queue == 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping delete(" << idx
	         << ") on empty queue." << endl;
      } അല്ലെങ്കിൽ {
	    size_t size = queue->get_size();
	    എങ്കിൽ (idx >= size) {
		  cerr << thr->get_fileline()
		       << "Warning: skipping out of range delete(" << idx
		       << ") on queue of size " << size << "." << endl;
	    } അല്ലെങ്കിൽ {
		  queue->erase(idx);
	    }
      }

      തിരികെ സത്യം;
}

/* %delete/obj <label>
 *
 * This operator works by assigning a nil to the target object. This
 * causes any value that might be there to be garbage collected, thus
 * deleting the object.
 */
തർക്കം of_DELETE_OBJ(vthread_t thr, vvp_code_t cp)
{
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_send_object(ptr, vvp_object_t(), thr->wt_context);

      തിരികെ സത്യം;
}

/* %reverse/obj <label>
 * Reverse element order of the queue or dynamic array object at this net.
 */
തർക്കം of_REVERSE_OBJ(vthread_t /*thr*/, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      പ്രതിജ്ഞാ(obj);

      vvp_object_t stor = obj->get_object();
      എങ്കിൽ (stor.test_nil()) തിരികെ സത്യം;

	/* Match other queue ops: peek queues before vvp_darray so concrete
	   queue types dispatch reverse_elems() correctly. */
      എങ്കിൽ (vvp_queue* queue = stor.peek<vvp_queue>()) {
	    queue->reverse_elems();
	    തിരികെ സത്യം;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      പ്രതിജ്ഞാ(darray);
      darray->reverse_elems();

      തിരികെ സത്യം;
}

/* %reverse/prop/obj <property-index>
 * Reverse queue/darray property storage inside the class object on stack top.
 */
തർക്കം of_REVERSE_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->സംഖ്യ;

      vvp_object_t& മുകൾ = thr->peek_object();
      vvp_cobject*cobj = മുകൾ.peek<vvp_cobject>();
      പ്രതിജ്ഞാ(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      എങ്കിൽ (arr_obj.test_nil()) തിരികെ സത്യം;

      എങ്കിൽ (vvp_queue* queue = arr_obj.peek<vvp_queue>()) {
	    queue->reverse_elems();
	    തിരികെ സത്യം;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      പ്രതിജ്ഞാ(darray);
      darray->reverse_elems();

      തിരികെ സത്യം;
}

/* %sort/obj — ascending sort */
തർക്കം of_SORT_OBJ(vthread_t /*thr*/, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      പ്രതിജ്ഞാ(obj);

      vvp_object_t stor = obj->get_object();
      എങ്കിൽ (stor.test_nil()) തിരികെ സത്യം;

      എങ്കിൽ (vvp_queue* queue = stor.peek<vvp_queue>()) {
	    queue->sort_elems(സത്യം);
	    തിരികെ സത്യം;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      പ്രതിജ്ഞാ(darray);
      darray->sort_elems(സത്യം);

      തിരികെ സത്യം;
}

/* %sort/prop/obj */
തർക്കം of_SORT_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->സംഖ്യ;

      vvp_object_t& മുകൾ = thr->peek_object();
      vvp_cobject*cobj = മുകൾ.peek<vvp_cobject>();
      പ്രതിജ്ഞാ(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      എങ്കിൽ (arr_obj.test_nil()) തിരികെ സത്യം;

      എങ്കിൽ (vvp_queue* queue = arr_obj.peek<vvp_queue>()) {
	    queue->sort_elems(സത്യം);
	    തിരികെ സത്യം;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      പ്രതിജ്ഞാ(darray);
      darray->sort_elems(സത്യം);

      തിരികെ സത്യം;
}

/* %rsort/obj — descending sort */
തർക്കം of_RSORT_OBJ(vthread_t /*thr*/, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      പ്രതിജ്ഞാ(obj);

      vvp_object_t stor = obj->get_object();
      എങ്കിൽ (stor.test_nil()) തിരികെ സത്യം;

      എങ്കിൽ (vvp_queue* queue = stor.peek<vvp_queue>()) {
	    queue->sort_elems(അസത്യം);
	    തിരികെ സത്യം;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      പ്രതിജ്ഞാ(darray);
      darray->sort_elems(അസത്യം);

      തിരികെ സത്യം;
}

/* %rsort/prop/obj */
തർക്കം of_RSORT_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->സംഖ്യ;

      vvp_object_t& മുകൾ = thr->peek_object();
      vvp_cobject*cobj = മുകൾ.peek<vvp_cobject>();
      പ്രതിജ്ഞാ(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      എങ്കിൽ (arr_obj.test_nil()) തിരികെ സത്യം;

      എങ്കിൽ (vvp_queue* queue = arr_obj.peek<vvp_queue>()) {
	    queue->sort_elems(അസത്യം);
	    തിരികെ സത്യം;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      പ്രതിജ്ഞാ(darray);
      darray->sort_elems(അസത്യം);

      തിരികെ സത്യം;
}

/* %shuffle/obj */
തർക്കം of_SHUFFLE_OBJ(vthread_t /*thr*/, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      പ്രതിജ്ഞാ(obj);

      vvp_object_t stor = obj->get_object();
      എങ്കിൽ (stor.test_nil()) തിരികെ സത്യം;

      എങ്കിൽ (vvp_queue* queue = stor.peek<vvp_queue>()) {
	    queue->shuffle_elems();
	    തിരികെ സത്യം;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      പ്രതിജ്ഞാ(darray);
      darray->shuffle_elems();

      തിരികെ സത്യം;
}

/* %shuffle/prop/obj */
തർക്കം of_SHUFFLE_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->സംഖ്യ;

      vvp_object_t& മുകൾ = thr->peek_object();
      vvp_cobject*cobj = മുകൾ.peek<vvp_cobject>();
      പ്രതിജ്ഞാ(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      എങ്കിൽ (arr_obj.test_nil()) തിരികെ സത്യം;

      എങ്കിൽ (vvp_queue* queue = arr_obj.peek<vvp_queue>()) {
	    queue->shuffle_elems();
	    തിരികെ സത്യം;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      പ്രതിജ്ഞാ(darray);
      darray->shuffle_elems();

      തിരികെ സത്യം;
}

/* %delete/tail <label>, idx
 *
 * Remove all elements after the one specified.
 */
തർക്കം of_DELETE_TAIL(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      പ്രതിജ്ഞാ(obj);

      vvp_queue*queue = obj->get_object().peek<vvp_queue>();
      പ്രതിജ്ഞാ(queue);

      unsigned idx = thr->വാക്കുകൾ[cp->bit_idx[0]].w_int;
      queue->erase_tail(idx);

      തിരികെ സത്യം;
}

സ്ഥിതം തർക്കം do_disable(vthread_t thr, vthread_t match)
{
      തർക്കം flag = അസത്യം;

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
      ആകുമ്പോൾ (! thr->children.empty()) {

	    vthread_t tmp = *(thr->children.begin());
	    പ്രതിജ്ഞാ(tmp);
	    പ്രതിജ്ഞാ(tmp->parent == thr);
	    thr->i_am_joining = 0;
	    എങ്കിൽ (do_disable(tmp, match))
		  flag = സത്യം;

	    vthread_reap(tmp);
      }

      vthread_t parent = thr->parent;
      എങ്കിൽ (parent && parent->i_am_joining) {
	      // If a parent is waiting in a %join, wake it up. Note
	      // that it is possible to be waiting in a %join yet
	      // already scheduled if multiple child threads are
	      // ending. So check if the thread is already scheduled
	      // before scheduling it again.
	    parent->i_am_joining = 0;
	    എങ്കിൽ (! parent->i_have_ended)
		  schedule_vthread(parent, 0, സത്യം);

	    do_join(parent, thr);

      } അല്ലെങ്കിൽ എങ്കിൽ (parent) {
	      /* If the parent is yet to %join me, let its %join
		 do the reaping. */
	      //assert(tmp->is_scheduled == 0);

      } അല്ലെങ്കിൽ {
	      /* No parent at all. Goodbye. */
	    vthread_reap(thr);
      }

      തിരികെ flag || (thr == match);
}

/*
 * Implement the %disable instruction by scanning the target scope for
 * all the target threads. Kill the target threads and wake up a
 * parent that is attempting a %join.
 */
തർക്കം of_DISABLE(vthread_t thr, vvp_code_t cp)
{
      __vpiScope*scope = static_cast<__vpiScope*>(cp->handle);

      തർക്കം disabled_myself_flag = അസത്യം;

      ആകുമ്പോൾ (! scope->threads.empty()) {
	    set<vthread_t>::iterator cur = scope->threads.begin();

	    എങ്കിൽ (do_disable(*cur, thr))
		  disabled_myself_flag = സത്യം;
      }

      തിരികെ ! disabled_myself_flag;
}

/*
 * Similar to `of_DISABLE`. But will only disable a single thread of the
 * specified scope. The disabled thread will be the thread closest to the
 * current thread in thread hierarchy. This can either be the current thread,
 * either the thread itself or one of its parents.
 * This is used for SystemVerilog flow control instructions like `return`,
 * `continue` and `break`.
 */

തർക്കം of_DISABLE_FLOW(vthread_t thr, vvp_code_t cp)
{
      സ്ഥിരം __vpiScope*scope = static_cast<__vpiScope*>(cp->handle);
      vthread_t cur = thr;

      ആകുമ്പോൾ (cur && cur->parent_scope != scope)
	    cur = cur->parent;

      പ്രതിജ്ഞാ(cur);
      എങ്കിൽ (cur)
	    തിരികെ !do_disable(cur, thr);

      തിരികെ അസത്യം;
}

/*
 * Implement the %disable/fork (SystemVerilog) instruction by disabling
 * all the detached children of the given thread.
 */
തർക്കം of_DISABLE_FORK(vthread_t thr, vvp_code_t)
{
	/* If a %disable/fork is being executed then the parent thread
	 * cannot be waiting in a join. */
      പ്രതിജ്ഞാ(! thr->i_am_joining);

	/* There should be no active children to disable. */
      പ്രതിജ്ഞാ(thr->children.empty());

	/* Disable any detached children. */
      ആകുമ്പോൾ (! thr->detached_children.empty()) {
	    vthread_t child = *(thr->detached_children.begin());
	    പ്രതിജ്ഞാ(child);
	    പ്രതിജ്ഞാ(child->parent == thr);
	      /* Disabling the children can never match the parent thread. */
	    തർക്കം res = do_disable(child, thr);
	    പ്രതിജ്ഞാ(! res);
	    vthread_reap(child);
      }

      തിരികെ സത്യം;
}

/*
 * This function divides a 2-word number {high, a} by a 1-word
 * number. Assume that high < b.
 */
സ്ഥിതം unsigned long divide2words(unsigned long a, unsigned long b,
				  unsigned long മുകളിലെ)
{
      unsigned long result = 0;
      ആകുമ്പോൾ (മുകളിലെ > 0) {
	    unsigned long tmp_result = ULONG_MAX / b;
	    unsigned long remain = ULONG_MAX % b;

	    remain += 1;
	    എങ്കിൽ (remain >= b) {
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
	    result += tmp_result * മുകളിലെ;

	      // The new iteration starts with high*remain + a.
	    remain = multiply_with_carry(മുകളിലെ, remain, മുകളിലെ);
	    a += remain;
            എങ്കിൽ(a < remain)
              മുകളിലെ += 1;

	      // Now result*b + {high,a} == the input {high,a}. It is
	      // possible that the new high >= 1. If so, it will
	      // certainly be less than high from the previous
	      // iteration. Do another iteration and it will shrink,
	      // eventually to 0.
      }

	// high is now 0, so a is the remaining remainder, so we can
	// finish off the integer divide with a simple a/b.

      തിരികെ result + a/b;
}

സ്ഥിതം unsigned long* divide_bits(unsigned long*ap, സ്ഥിരം unsigned long*bp, unsigned wid)
{
	// Do all our work a cpu-word at a time. The "words" variable
	// is the number of words of the wid.
      unsigned വാക്കുകൾ = (wid+CPU_WORD_BITS-1) / CPU_WORD_BITS;

      unsigned btop = വാക്കുകൾ-1;
      ആകുമ്പോൾ (btop > 0 && bp[btop] == 0)
	    btop -= 1;

	// Detect divide by 0, and exit.
      എങ്കിൽ (btop==0 && bp[0]==0)
	    തിരികെ 0;

	// The result array will eventually accumulate the result. The
	// diff array is a difference that we use in the intermediate.
      unsigned long*diff  = പുതിയ unsigned long[വാക്കുകൾ];
      unsigned long*result= പുതിയ unsigned long[വാക്കുകൾ];
      വേണ്ടി (unsigned idx = 0 ; idx < വാക്കുകൾ ; idx += 1)
	    result[idx] = 0;

      വേണ്ടി (unsigned cur = വാക്കുകൾ-btop ; cur > 0 ; cur -= 1) {
	    unsigned cur_ptr = cur-1;
	    unsigned long cur_res;
	    എങ്കിൽ (ap[cur_ptr+btop] >= bp[btop]) {
		  unsigned long മുകളിലെ = 0;
		  എങ്കിൽ (cur_ptr+btop+1 < വാക്കുകൾ)
			മുകളിലെ = ap[cur_ptr+btop+1];
		  cur_res = divide2words(ap[cur_ptr+btop], bp[btop], മുകളിലെ);

	    } അല്ലെങ്കിൽ എങ്കിൽ (cur_ptr+btop+1 >= വാക്കുകൾ) {
		  തുടരുക;

	    } അല്ലെങ്കിൽ എങ്കിൽ (ap[cur_ptr+btop+1] == 0) {
		  തുടരുക;

	    } അല്ലെങ്കിൽ {
		  cur_res = divide2words(ap[cur_ptr+btop], bp[btop],
					 ap[cur_ptr+btop+1]);
	    }

	      // cur_res is a guesstimate of the result this far. It
	      // may be 1 too big. (But it will also be >0) Try it,
	      // and if the difference comes out negative, then adjust.

	      // diff = (bp * cur_res)  << cur_ptr;
	    multiply_array_imm(diff+cur_ptr, bp, വാക്കുകൾ-cur_ptr, cur_res);
	      // ap -= diff
	    unsigned long carry = 1;
	    വേണ്ടി (unsigned idx = cur_ptr ; idx < വാക്കുകൾ ; idx += 1)
		  ap[idx] = add_with_carry(ap[idx], ~diff[idx], carry);

	      // ap has the diff subtracted out of it. If cur_res was
	      // too large, then ap will turn negative. (We easily
	      // tell that ap turned negative by looking at
	      // carry&1. If it is 0, then it is *negative*.) In that
	      // case, we know that cur_res was too large by 1. Correct by
	      // adding 1b back in and reducing cur_res.
	    എങ്കിൽ ((carry&1) == 0) {
		    // Keep adding b back in until the remainder
		    // becomes positive again.
		  ചെയ്യുക {
			cur_res -= 1;
			carry = 0;
			വേണ്ടി (unsigned idx = cur_ptr ; idx < വാക്കുകൾ ; idx += 1)
			      ap[idx] = add_with_carry(ap[idx], bp[idx-cur_ptr], carry);
		  } ആകുമ്പോൾ (carry == 0);
	    }

	    result[cur_ptr] = cur_res;
      }

	// Now ap contains the remainder and result contains the
	// desired result. We should find that:
	//  input-a = bp * result + ap;

      delete[]diff;
      തിരികെ result;
}

/*
 * %div
 */
തർക്കം of_DIV(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valb = thr->എടുക്കുക_vec4();
      vvp_vector4_t vala = thr->എടുക്കുക_vec4();

      പ്രതിജ്ഞാ(vala.size()== valb.size());
      unsigned wid = vala.size();

      unsigned long*ap = vala.subarray(0, wid);
      എങ്കിൽ (ap == 0) {
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->തള്ളുക_vec4(tmp);
	    തിരികെ സത്യം;
      }

      unsigned long*bp = valb.subarray(0, wid);
      എങ്കിൽ (bp == 0) {
	    delete[]ap;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->തള്ളുക_vec4(tmp);
	    തിരികെ സത്യം;
      }

	// If the value fits in a single CPU word, then do it the easy way.
      എങ്കിൽ (wid <= CPU_WORD_BITS) {
	    എങ്കിൽ (bp[0] == 0) {
		  vvp_vector4_t tmp(wid, BIT4_X);
		  thr->തള്ളുക_vec4(tmp);
	    } അല്ലെങ്കിൽ {
		  ap[0] /= bp[0];
		  vala.setarray(0, wid, ap);
		  thr->തള്ളുക_vec4(vala);
	    }
	    delete[]ap;
	    delete[]bp;
	    തിരികെ സത്യം;
      }

      unsigned long*result = divide_bits(ap, bp, wid);
      എങ്കിൽ (result == 0) {
	    delete[]ap;
	    delete[]bp;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->തള്ളുക_vec4(tmp);
	    തിരികെ സത്യം;
      }

	// Now ap contains the remainder and result contains the
	// desired result. We should find that:
	//  input-a = bp * result + ap;

      vala.setarray(0, wid, result);
      thr->തള്ളുക_vec4(vala);
      delete[]ap;
      delete[]bp;
      delete[]result;

      തിരികെ സത്യം;
}


സ്ഥിതം ശൂന്യം negate_വാക്കുകൾ(unsigned long*val, unsigned വാക്കുകൾ)
{
      unsigned long carry = 1;
      വേണ്ടി (unsigned idx = 0 ; idx < വാക്കുകൾ ; idx += 1)
	    val[idx] = add_with_carry(0, ~val[idx], carry);
}

/*
 * %div/s
 */
തർക്കം of_DIV_S(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valb = thr->എടുക്കുക_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      പ്രതിജ്ഞാ(vala.size()== valb.size());
      unsigned wid = vala.size();
      unsigned വാക്കുകൾ = (wid + CPU_WORD_BITS - 1) / CPU_WORD_BITS;

	// Get the values, left in right, in binary form. If there is
	// a problem with either (caused by an X or Z bit) then we
	// know right away that the entire result is X.
      unsigned long*ap = vala.subarray(0, wid);
      എങ്കിൽ (ap == 0) {
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    തിരികെ സത്യം;
      }

      unsigned long*bp = valb.subarray(0, wid);
      എങ്കിൽ (bp == 0) {
	    delete[]ap;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    തിരികെ സത്യം;
      }

	// Sign extend the bits in the array to fill out the array.
      unsigned long sign_mask = 0;
      എങ്കിൽ (unsigned long sign_bits = (വാക്കുകൾ*CPU_WORD_BITS) - wid) {
	    sign_mask = -1UL << (CPU_WORD_BITS-sign_bits);
	    എങ്കിൽ (ap[വാക്കുകൾ-1] & (sign_mask>>1))
		  ap[വാക്കുകൾ-1] |= sign_mask;
	    എങ്കിൽ (bp[വാക്കുകൾ-1] & (sign_mask>>1))
		  bp[വാക്കുകൾ-1] |= sign_mask;
      }

	// If the value fits in a single word, then use the native divide.
      എങ്കിൽ (wid <= CPU_WORD_BITS) {
	    എങ്കിൽ (bp[0] == 0) {
		  vvp_vector4_t tmp(wid, BIT4_X);
		  vala = tmp;
	    } അല്ലെങ്കിൽ എങ്കിൽ (((long)ap[0] == LONG_MIN) && ((long)bp[0] == -1)) {
		  vvp_vector4_t tmp(wid, BIT4_0);
		  tmp.set_bit(wid-1, BIT4_1);
		  vala = tmp;
	    } അല്ലെങ്കിൽ {
		  long tmpa = (long) ap[0];
		  long tmpb = (long) bp[0];
		  long res = tmpa / tmpb;
		  ap[0] = ((unsigned long)res) & ~sign_mask;
		  vala.setarray(0, wid, ap);
	    }
	    delete[]ap;
	    delete[]bp;
	    തിരികെ സത്യം;
      }

	// We need to the actual division to positive integers. Make
	// them positive here, and remember the negations.
      തർക്കം negate_flag = അസത്യം;
      എങ്കിൽ ( ((long) ap[വാക്കുകൾ-1]) < 0 ) {
	    negate_flag = സത്യം;
	    negate_വാക്കുകൾ(ap, വാക്കുകൾ);
      }
      എങ്കിൽ ( ((long) bp[വാക്കുകൾ-1]) < 0 ) {
	    negate_flag ^= സത്യം;
	    negate_വാക്കുകൾ(bp, വാക്കുകൾ);
      }

      unsigned long*result = divide_bits(ap, bp, wid);
      എങ്കിൽ (result == 0) {
	    delete[]ap;
	    delete[]bp;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    തിരികെ സത്യം;
      }

      എങ്കിൽ (negate_flag) {
	    negate_വാക്കുകൾ(result, വാക്കുകൾ);
      }

      result[വാക്കുകൾ-1] &= ~sign_mask;

      vala.setarray(0, wid, result);
      delete[]ap;
      delete[]bp;
      delete[]result;
      തിരികെ സത്യം;
}

തർക്കം of_DIV_WR(vthread_t thr, vvp_code_t)
{
      ഇരട്ട r = thr->എടുക്കുക_real();
      ഇരട്ട l = thr->എടുക്കുക_real();
      thr->തള്ളുക_real(l / r);

      തിരികെ സത്യം;
}

/*
 * %dup/obj
 * %dup/real
 * %dup/vec4
 *
 * Push a duplicate of the object on the appropriate stack.
 */
തർക്കം of_DUP_OBJ(vthread_t thr, vvp_code_t)
{
      vvp_object_t src = thr->peek_object();

        // If it is null push a new null object
      എങ്കിൽ (src.test_nil())
	    thr->തള്ളുക_object(vvp_object_t());
      അല്ലെങ്കിൽ
	    thr->തള്ളുക_object(src.duplicate());

      തിരികെ സത്യം;
}

തർക്കം of_DUP_REAL(vthread_t thr, vvp_code_t)
{
      thr->തള്ളുക_real(thr->peek_real(0));
      തിരികെ സത്യം;
}

തർക്കം of_DUP_VEC4(vthread_t thr, vvp_code_t)
{
      thr->തള്ളുക_vec4(thr->peek_vec4(0));
      തിരികെ സത്യം;
}

/*
 * This terminates the current thread. If there is a parent who is
 * waiting for me to die, then I schedule it. At any rate, I mark
 * myself as a zombie by setting my pc to 0.
 */
തർക്കം of_END(vthread_t thr, vvp_code_t)
{
      പ്രതിജ്ഞാ(! thr->waiting_for_event);
      thr->i_have_ended = 1;
      thr->pc = codespace_null();

	/* Fully detach any detached children. */
      ആകുമ്പോൾ (! thr->detached_children.empty()) {
	    vthread_t child = *(thr->detached_children.begin());
	    പ്രതിജ്ഞാ(child);
	    പ്രതിജ്ഞാ(child->parent == thr);
	    പ്രതിജ്ഞാ(child->i_am_detached);
	    child->parent = 0;
	    child->i_am_detached = 0;
	    thr->detached_children.erase(thr->detached_children.begin());
      }

	/* It is an error to still have active children running at this
	 * point in time. They should have all been detached or joined. */
      പ്രതിജ്ഞാ(thr->children.empty());

	/* If I have a parent who is waiting for me, then mark that I
	   have ended, and schedule that parent. Also, finish the
	   %join for the parent. */
      എങ്കിൽ (!thr->i_am_detached && thr->parent && thr->parent->i_am_joining) {
	    vthread_t tmp = thr->parent;
	    പ്രതിജ്ഞാ(! thr->i_am_detached);

	    tmp->i_am_joining = 0;
	    schedule_vthread(tmp, 0, സത്യം);
	    do_join(tmp, thr);
	    തിരികെ അസത്യം;
      }

	/* If this thread is not fully detached then remove it from the
	 * parents detached_children set and reap it. */
      എങ്കിൽ (thr->i_am_detached) {
	    vthread_t tmp = thr->parent;
	    പ്രതിജ്ഞാ(tmp);
	    size_t res = tmp->detached_children.erase(thr);
	    പ്രതിജ്ഞാ(res == 1);
	      /* If the parent is waiting for the detached children to
	       * finish then the last detached child needs to tell the
	       * parent to wake up when it is finished. */
	    എങ്കിൽ (tmp->i_am_waiting && tmp->detached_children.empty()) {
		  tmp->i_am_waiting = 0;
		  schedule_vthread(tmp, 0, സത്യം);
	    }
	      /* Fully detach this thread so it will be reaped below. */
	    thr->i_am_detached = 0;
	    thr->parent = 0;
      }

	/* If I have no parent, then no one can %join me and there is
	 * no reason to stick around. This can happen, for example if
	 * I am an ``initial'' thread. */
      എങ്കിൽ (thr->parent == 0) {
	    vthread_reap(thr);
	    തിരികെ അസത്യം;
      }

	/* If I make it this far, then I have a parent who may wish
	   to %join me. Remain a zombie so that it can. */

      തിരികെ അസത്യം;
}

/*
 * %event <var-label>
 */
തർക്കം of_EVENT(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_vector4_t tmp (1, BIT4_X);
      vvp_send_vec4(ptr, tmp, thr->wt_context);
      തിരികെ സത്യം;
}

/*
 * %event/nb <var-label>, <delay>
 */
തർക്കം of_EVENT_NB(vthread_t thr, vvp_code_t cp)
{
      vvp_time64_t delay;

      delay = thr->വാക്കുകൾ[cp->bit_idx[0]].w_uint;
      schedule_propagate_event(cp->net, delay);
      തിരികെ സത്യം;
}

തർക്കം of_EVCTL(vthread_t thr, vvp_code_t cp)
{
      പ്രതിജ്ഞാ(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      thr->ecount = thr->വാക്കുകൾ[cp->bit_idx[0]].w_uint;
      തിരികെ സത്യം;
}
തർക്കം of_EVCTLC(vthread_t thr, vvp_code_t)
{
      thr->event = 0;
      thr->ecount = 0;
      തിരികെ സത്യം;
}

തർക്കം of_EVCTLI(vthread_t thr, vvp_code_t cp)
{
      പ്രതിജ്ഞാ(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      thr->ecount = cp->bit_idx[0];
      തിരികെ സത്യം;
}

തർക്കം of_EVCTLS(vthread_t thr, vvp_code_t cp)
{
      പ്രതിജ്ഞാ(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      int64_t val = thr->വാക്കുകൾ[cp->bit_idx[0]].w_int;
      എങ്കിൽ (val < 0) val = 0;
      thr->ecount = val;
      തിരികെ സത്യം;
}

തർക്കം of_FLAG_GET_VEC4(vthread_t thr, vvp_code_t cp)
{
      പൂർണ്ണസംഖ്യ flag = cp->സംഖ്യ;
      പ്രതിജ്ഞാ(flag < vthread_s::FLAGS_എണ്ണം);

      vvp_vector4_t val (1, thr->flags[flag]);
      thr->തള്ളുക_vec4(val);

      തിരികെ സത്യം;
}

/*
 * %flag_inv <flag1>
 */
തർക്കം of_FLAG_INV(vthread_t thr, vvp_code_t cp)
{
      പൂർണ്ണസംഖ്യ flag1 = cp->bit_idx[0];

      thr->flags[flag1] = ~ thr->flags[flag1];
      തിരികെ സത്യം;
}

/*
 * %flag_mov <flag1>, <flag2>
 */
തർക്കം of_FLAG_MOV(vthread_t thr, vvp_code_t cp)
{
      പൂർണ്ണസംഖ്യ flag1 = cp->bit_idx[0];
      പൂർണ്ണസംഖ്യ flag2 = cp->bit_idx[1];

      thr->flags[flag1] = thr->flags[flag2];
      തിരികെ സത്യം;
}

/*
 * %flag_or <flag1>, <flag2>
 */
തർക്കം of_FLAG_OR(vthread_t thr, vvp_code_t cp)
{
      പൂർണ്ണസംഖ്യ flag1 = cp->bit_idx[0];
      പൂർണ്ണസംഖ്യ flag2 = cp->bit_idx[1];

      thr->flags[flag1] = thr->flags[flag1] | thr->flags[flag2];
      തിരികെ സത്യം;
}

തർക്കം of_FLAG_SET_IMM(vthread_t thr, vvp_code_t cp)
{
      പൂർണ്ണസംഖ്യ flag = cp->സംഖ്യ;
      പൂർണ്ണസംഖ്യ vali = cp->bit_idx[0];

      പ്രതിജ്ഞാ(flag < vthread_s::FLAGS_എണ്ണം);
      പ്രതിജ്ഞാ(vali >= 0 && vali < 4);

      സ്ഥിതം സ്ഥിരം vvp_bit4_t map_bit[4] = {BIT4_0, BIT4_1, BIT4_Z, BIT4_X};
      thr->flags[flag] = map_bit[vali];
      തിരികെ സത്യം;
}

തർക്കം of_FLAG_SET_VEC4(vthread_t thr, vvp_code_t cp)
{
      പൂർണ്ണസംഖ്യ flag = cp->സംഖ്യ;
      പ്രതിജ്ഞാ(flag < vthread_s::FLAGS_എണ്ണം);

      സ്ഥിരം vvp_vector4_t&val = thr->peek_vec4();
      thr->flags[flag] = val.value(0);
      thr->എടുക്കുക_vec4(1);

      തിരികെ സത്യം;
}

/*
 * the %force/link instruction connects a source node to a
 * destination node. The destination node must be a signal, as it is
 * marked with the source of the force so that it may later be
 * unlinked without specifically knowing the source that this
 * instruction used.
 */
തർക്കം of_FORCE_LINK(vthread_t, vvp_code_t cp)
{
      vvp_net_t*dst = cp->net;
      vvp_net_t*src = cp->net2;

      പ്രതിജ്ഞാ(dst->fil);
      dst->fil->force_link(dst, src);

      തിരികെ സത്യം;
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
തർക്കം of_FORCE_VEC4(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_vector4_t value = thr->എടുക്കുക_vec4();

	/* Send the force value to the filter on the node. */

      പ്രതിജ്ഞാ(net->fil);
      എങ്കിൽ (value.size() != net->fil->filter_size())
	    value = coerce_to_width(value, net->fil->filter_size());

      net->force_vec4(value, vvp_vector2_t(vvp_vector2_t::FILL1, net->fil->filter_size()));

      തിരികെ സത്യം;
}

/*
 * %force/vec4/off <net>, <off>
 */
തർക്കം of_FORCE_VEC4_OFF(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      unsigned base_idx = cp->bit_idx[0];
      long base = thr->വാക്കുകൾ[base_idx].w_int;
      vvp_vector4_t value = thr->എടുക്കുക_vec4();
      unsigned wid = value.size();

      പ്രതിജ്ഞാ(net->fil);

      എങ്കിൽ (thr->flags[4] == BIT4_1)
	    തിരികെ സത്യം;

	// This is the width of the target vector.
      unsigned use_size = net->fil->filter_size();

      എങ്കിൽ (base >= (long)use_size)
	    തിരികെ സത്യം;
      എങ്കിൽ (base < -(long)use_size)
	    തിരികെ സത്യം;

      എങ്കിൽ ((base + wid) > use_size)
	    wid = use_size - base;

	// Make a mask of which bits are to be forced, 0 for unforced
	// bits and 1 for forced bits.
      vvp_vector2_t mask (vvp_vector2_t::FILL0, use_size);
      വേണ്ടി (unsigned idx = 0 ; idx < wid ; idx += 1)
	    mask.set_bit(base+idx, 1);

      vvp_vector4_t tmp (use_size, BIT4_Z);

	// vvp_net_t::force_vec4 propagates all the bits of the
	// forced vector value, regardless of the mask. This
	// ensures the unforced bits retain their current value.
      vvp_signal_മൂല്യം*sig = dynamic_cast<vvp_signal_മൂല്യം*>(net->fil);
      പ്രതിജ്ഞാ(sig);
      sig->vec4_മൂല്യം(tmp);

      tmp.set_vec(base, value);

      net->force_vec4(tmp, mask);
      തിരികെ സത്യം;
}

/*
 * %force/vec4/off/d <net>, <off>, <del>
 */
തർക്കം of_FORCE_VEC4_OFF_D(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      unsigned base_idx = cp->bit_idx[0];
      long base = thr->വാക്കുകൾ[base_idx].w_int;

      unsigned delay_idx = cp->bit_idx[1];
      vvp_time64_t delay = thr->വാക്കുകൾ[delay_idx].w_uint;

      vvp_vector4_t value = thr->എടുക്കുക_vec4();

      പ്രതിജ്ഞാ(net->fil);

      എങ്കിൽ (thr->flags[4] == BIT4_1)
	    തിരികെ സത്യം;

	// This is the width of the target vector.
      unsigned use_size = net->fil->filter_size();

      എങ്കിൽ (base >= (long)use_size)
	    തിരികെ സത്യം;
      എങ്കിൽ (base < -(long)use_size)
	    തിരികെ സത്യം;

      schedule_force_vector(net, base, use_size, value, delay);
      തിരികെ സത്യം;
}

തർക്കം of_FORCE_WR(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net  = cp->net;
      ഇരട്ട value = thr->എടുക്കുക_real();

      net->force_real(value, vvp_vector2_t(vvp_vector2_t::FILL1, 1));

      തിരികെ സത്യം;
}

/*
 * The %fork instruction causes a new child to be created and pushed
 * in front of any existing child. This causes the new child to be
 * added to the list of children, and for me to be the parent of the
 * new child.
 */
തർക്കം of_FORK(vthread_t thr, vvp_code_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);

      എങ്കിൽ (cp->scope->is_automatic()) {
              /* The context allocated for this child is the top entry
                 on the write context stack. */
            child->wt_context = thr->wt_context;
            child->rd_context = thr->wt_context;
      }

      child->parent = thr;
      thr->children.insert(child);

      എങ്കിൽ (thr->i_am_in_function) {
	    child->is_scheduled = 1;
	    child->i_am_in_function = 1;
	    vthread_run(child);
	    running_thread = thr;
      } അല്ലെങ്കിൽ {
	    schedule_vthread(child, 0, സത്യം);
      }
      തിരികെ സത്യം;
}

തർക്കം of_FREE(vthread_t thr, vvp_code_t cp)
{
        /* Pop the child context from the read context stack. */
      vvp_context_t child_context = thr->rd_context;
      thr->rd_context = vvp_get_stacked_context(child_context);

        /* Free the context. */
      vthread_free_context(child_context, cp->scope);

      തിരികെ സത്യം;
}

/*
 * %inv
 *
 * Logically, this pops a value, inverts is (Verilog style, with Z and
 * X converted to X) and pushes the result. We can more efficiently
 * just to the invert in place.
 */
തർക്കം of_INV(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val.invert();
      തിരികെ സത്യം;
}


/*
 * Index registers, arithmetic.
 */

സ്ഥിതം വരിയിൽ int64_t get_as_64_bit(uint32_t താഴെയുള്ള_32, uint32_t മുകളിലെ_32)
{
      int64_t താഴെയുള്ള = താഴെയുള്ള_32;
      int64_t res = മുകളിലെ_32;

      res <<= 32;
      res |= താഴെയുള്ള;
      തിരികെ res;
}

തർക്കം of_IX_ADD(vthread_t thr, vvp_code_t cp)
{
      thr->വാക്കുകൾ[cp->സംഖ്യ].w_int += get_as_64_bit(cp->bit_idx[0],
                                                    cp->bit_idx[1]);
      തിരികെ സത്യം;
}

തർക്കം of_IX_SUB(vthread_t thr, vvp_code_t cp)
{
      thr->വാക്കുകൾ[cp->സംഖ്യ].w_int -= get_as_64_bit(cp->bit_idx[0],
                                                    cp->bit_idx[1]);
      തിരികെ സത്യം;
}

തർക്കം of_IX_MUL(vthread_t thr, vvp_code_t cp)
{
      thr->വാക്കുകൾ[cp->സംഖ്യ].w_int *= get_as_64_bit(cp->bit_idx[0],
                                                    cp->bit_idx[1]);
      തിരികെ സത്യം;
}

തർക്കം of_IX_LOAD(vthread_t thr, vvp_code_t cp)
{
      thr->വാക്കുകൾ[cp->സംഖ്യ].w_int = get_as_64_bit(cp->bit_idx[0],
                                                   cp->bit_idx[1]);
      തിരികെ സത്യം;
}

തർക്കം of_IX_MOV(vthread_t thr, vvp_code_t cp)
{
      thr->വാക്കുകൾ[cp->bit_idx[0]].w_int = thr->വാക്കുകൾ[cp->bit_idx[1]].w_int;
      തിരികെ സത്യം;
}

തർക്കം of_IX_GETV(vthread_t thr, vvp_code_t cp)
{
      unsigned index = cp->bit_idx[0];
      vvp_net_t*net = cp->net;

      vvp_signal_മൂല്യം*sig = dynamic_cast<vvp_signal_മൂല്യം*>(net->fil);
      എങ്കിൽ (sig == 0) {
	    പ്രതിജ്ഞാ(net->fil);
	    cerr << thr->get_fileline()
	         << "%%ix/getv error: Net arg not a vector signal? "
		 << typeid(*net->fil).name() << endl;
      }
      പ്രതിജ്ഞാ(sig);

      vvp_vector4_t vec;
      sig->vec4_മൂല്യം(vec);
      തർക്കം overflow_flag;
      uint64_t val = 0;
      തർക്കം known_flag = vector4_to_മൂല്യം(vec, overflow_flag, val);

      എങ്കിൽ (known_flag)
	    thr->വാക്കുകൾ[index].w_uint = val;
      അല്ലെങ്കിൽ
	    thr->വാക്കുകൾ[index].w_uint = 0;

	/* Set bit 4 as a flag if the input is unknown. */
      thr->flags[4] = known_flag ? (overflow_flag ? BIT4_X : BIT4_0) : BIT4_1;

      തിരികെ സത്യം;
}

തർക്കം of_IX_GETV_S(vthread_t thr, vvp_code_t cp)
{
      unsigned index = cp->bit_idx[0];
      vvp_net_t*net = cp->net;

      vvp_signal_മൂല്യം*sig = dynamic_cast<vvp_signal_മൂല്യം*>(net->fil);
      എങ്കിൽ (sig == 0) {
	    പ്രതിജ്ഞാ(net->fil);
	    cerr << thr->get_fileline()
	         << "%%ix/getv/s error: Net arg not a vector signal? "
		 << "fun=" << typeid(*net->fil).name()
		 << ", fil=" << (net->fil? typeid(*net->fil).name() : "<>")
		 << endl;
      }
      പ്രതിജ്ഞാ(sig);

      vvp_vector4_t vec;
      sig->vec4_മൂല്യം(vec);
      int64_t val;
      തർക്കം known_flag = vector4_to_മൂല്യം(vec, val, സത്യം, സത്യം);

      എങ്കിൽ (known_flag)
	    thr->വാക്കുകൾ[index].w_int = val;
      അല്ലെങ്കിൽ
	    thr->വാക്കുകൾ[index].w_int = 0;

	/* Set bit 4 as a flag if the input is unknown. */
      thr->flags[4] = known_flag? BIT4_0 : BIT4_1;

      തിരികെ സത്യം;
}

സ്ഥിതം uint64_t vec4_to_index(vthread_t thr, തർക്കം signed_flag)
{
	// Get all the information we need about the vec4 vector, then
	// pop it away. We only need the bool bits and the length.
      സ്ഥിരം vvp_vector4_t&val = thr->peek_vec4();
      unsigned val_size = val.size();
      unsigned long*bits = val.subarray(0, val_size, അസത്യം);
      thr->എടുക്കുക_vec4(1);

	// If there are X/Z bits, then the subarray will give us a nil
	// pointer. Set a flag to indicate the error, and give up.
      എങ്കിൽ (bits == 0) {
	    thr->flags[4] = BIT4_1;
	    തിരികെ 0;
      }

      uint64_t v = 0;
      thr->flags[4] = BIT4_0;

      പ്രതിജ്ഞാ(വലുപ്പം(bits[0]) <= വലുപ്പം(v));

      v = 0;
      വേണ്ടി (unsigned idx = 0 ; idx < val_size ; idx += 8*വലുപ്പം(bits[0])) {
	    uint64_t tmp = bits[idx/8/വലുപ്പം(bits[0])];
	    എങ്കിൽ (idx < 8*വലുപ്പം(v)) {
		  v |= tmp << idx;
	    } അല്ലെങ്കിൽ {
		  തർക്കം overflow = signed_flag && (v >> 63) ? ~tmp != 0 : tmp != 0;
		  എങ്കിൽ (overflow) {
			thr->flags[4] = BIT4_X;
			നിർത്തുക;
		  }
	    }
      }

	// Set the high bits that are not necessarily filled in by the
	// subarray function.
      എങ്കിൽ (val_size < 8*വലുപ്പം(v)) {
	    എങ്കിൽ (signed_flag && (v & (static_cast<uint64_t>(1)<<(val_size-1)))) {
		    // Propagate the sign bit...
		  v |= (~static_cast<uint64_t>(0)) << val_size;

	    } അല്ലെങ്കിൽ {
		    // Fill with zeros.
		  v &= ~((~static_cast<uint64_t>(0)) << val_size);
	    }

      }

      delete[]bits;
      തിരികെ v;
}

/*
 * %ix/vec4 <idx>
 */
തർക്കം of_IX_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned use_idx = cp->സംഖ്യ;
      thr->വാക്കുകൾ[use_idx].w_uint = vec4_to_index(thr, അസത്യം);
      തിരികെ സത്യം;
}

/*
 * %ix/vec4/s <idx>
 */
തർക്കം of_IX_VEC4_S(vthread_t thr, vvp_code_t cp)
{
      unsigned use_idx = cp->സംഖ്യ;
      thr->വാക്കുകൾ[use_idx].w_uint = vec4_to_index(thr, സത്യം);
      തിരികെ സത്യം;
}

/*
 * The various JMP instruction work simply by pulling the new program
 * counter from the instruction and resuming. If the jump is
 * conditional, then test the bit for the expected value first.
 */
തർക്കം of_JMP(vthread_t thr, vvp_code_t cp)
{
      thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      എങ്കിൽ (schedule_stopped()) {
	    schedule_vthread(thr, 0, അസത്യം);
	    തിരികെ അസത്യം;
      }

      തിരികെ സത്യം;
}

/*
 * %jmp/0 <pc>, <flag>
 */
തർക്കം of_JMP0(vthread_t thr, vvp_code_t cp)
{
      എങ്കിൽ (thr->flags[cp->bit_idx[0]] == BIT4_0)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      എങ്കിൽ (schedule_stopped()) {
	    schedule_vthread(thr, 0, അസത്യം);
	    തിരികെ അസത്യം;
      }

      തിരികെ സത്യം;
}

/*
 * %jmp/0xz <pc>, <flag>
 */
തർക്കം of_JMP0XZ(vthread_t thr, vvp_code_t cp)
{
      എങ്കിൽ (thr->flags[cp->bit_idx[0]] != BIT4_1)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      എങ്കിൽ (schedule_stopped()) {
	    schedule_vthread(thr, 0, അസത്യം);
	    തിരികെ അസത്യം;
      }

      തിരികെ സത്യം;
}

/*
 * %jmp/1 <pc>, <flag>
 */
തർക്കം of_JMP1(vthread_t thr, vvp_code_t cp)
{
      എങ്കിൽ (thr->flags[cp->bit_idx[0]] == BIT4_1)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      എങ്കിൽ (schedule_stopped()) {
	    schedule_vthread(thr, 0, അസത്യം);
	    തിരികെ അസത്യം;
      }

      തിരികെ സത്യം;
}

/*
 * %jmp/1xz <pc>, <flag>
 */
തർക്കം of_JMP1XZ(vthread_t thr, vvp_code_t cp)
{
      എങ്കിൽ (thr->flags[cp->bit_idx[0]] != BIT4_0)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      എങ്കിൽ (schedule_stopped()) {
	    schedule_vthread(thr, 0, അസത്യം);
	    തിരികെ അസത്യം;
      }

      തിരികെ സത്യം;
}

/*
 * The %join instruction causes the thread to wait for one child
 * to die.  If a child is already dead (and a zombie) then I reap
 * it and go on. Otherwise, I mark myself as waiting in a join so that
 * children know to wake me when they finish.
 */

സ്ഥിതം ശൂന്യം do_join(vthread_t thr, vthread_t child)
{
      പ്രതിജ്ഞാ(child->parent == thr);

        /* If the immediate child thread is in an automatic scope... */
      എങ്കിൽ (child->wt_context) {
              /* and is the top level task/function thread... */
            എങ്കിൽ (thr->wt_context != thr->rd_context) {
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

സ്ഥിതം തർക്കം do_join_opcode(vthread_t thr)
{
      പ്രതിജ്ഞാ( !thr->i_am_joining );
      പ്രതിജ്ഞാ( !thr->children.empty());

	// Are there any children that have already ended? If so, then
	// join with that one.
      വേണ്ടി (set<vthread_t>::iterator cur = thr->children.begin()
		 ; cur != thr->children.end() ; ++cur) {
	    vthread_t curp = *cur;
	    എങ്കിൽ (! curp->i_have_ended)
		  തുടരുക;

	      // found something!
	    do_join(thr, curp);
	    തിരികെ സത്യം;
      }

	// Otherwise, tell my children to awaken me when they end,
	// then pause.
      thr->i_am_joining = 1;
      തിരികെ അസത്യം;
}

തർക്കം of_JOIN(vthread_t thr, vvp_code_t)
{
      തിരികെ do_join_opcode(thr);
}

/*
 * This %join/detach <n> instruction causes the thread to detach
 * threads that were created by an earlier %fork.
 */
തർക്കം of_JOIN_DETACH(vthread_t thr, vvp_code_t cp)
{
      unsigned long എണ്ണം = cp->സംഖ്യ;

      പ്രതിജ്ഞാ(എണ്ണം == thr->children.size());

      ആകുമ്പോൾ (! thr->children.empty()) {
	    vthread_t child = *thr->children.begin();
	    പ്രതിജ്ഞാ(child->parent == thr);

	      // We cannot detach automatic tasks/functions within an
	      // automatic scope. If we try to do that, we might make
	      // a mess of the allocation of the context. Note that it
	      // is OK if the child context is distinct (See %exec_ufunc.)
	    പ്രതിജ്ഞാ(child->wt_context==0 || thr->wt_context!=child->wt_context);
	    എങ്കിൽ (child->i_have_ended) {
		    // If the child has already ended, then reap it.
		  vthread_reap(child);

	    } അല്ലെങ്കിൽ {
		  size_t res = child->parent->children.erase(child);
		  പ്രതിജ്ഞാ(res == 1);
		  child->i_am_detached = 1;
		  thr->detached_children.insert(child);
	    }
      }

      തിരികെ സത്യം;
}

/*
 * %load/ar <array-label>, <index>;
*/
തർക്കം of_LOAD_AR(vthread_t thr, vvp_code_t cp)
{
      unsigned idx = cp->bit_idx[0];
      ഇരട്ട word;

	/* The result is 0.0 if the address is undefined. */
      എങ്കിൽ (thr->flags[4] == BIT4_1) {
	    word = 0.0;
      } അല്ലെങ്കിൽ {
	    unsigned adr = thr->വാക്കുകൾ[idx].w_int;
	    word = cp->array->get_word_r(adr);
      }

      thr->തള്ളുക_real(word);
      തിരികെ സത്യം;
}

template <typename ELEM>
സ്ഥിതം തർക്കം load_dar(vthread_t thr, vvp_code_t cp)
{
      int64_t adr = thr->വാക്കുകൾ[3].w_int;
      vvp_net_t*net = cp->net;
      പ്രതിജ്ഞാ(net);

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      പ്രതിജ്ഞാ(obj);

      vvp_darray*darray = obj->get_object().peek<vvp_darray>();

      ELEM word;
      എങ്കിൽ (darray &&
          (adr >= 0) && (thr->flags[4] == BIT4_0)) // A defined address >= 0
	    darray->get_word(adr, word);
      അല്ലെങ്കിൽ
	    dq_default(word, obj->size());

      vthread_തള്ളുക(thr, word);
      തിരികെ സത്യം;
}

/*
 * %load/dar/r <array-label>;
 */
തർക്കം of_LOAD_DAR_R(vthread_t thr, vvp_code_t cp)
{
      തിരികെ load_dar<ഇരട്ട>(thr, cp);
}

/*
 * %load/dar/str <array-label>;
 */
തർക്കം of_LOAD_DAR_STR(vthread_t thr, vvp_code_t cp)
{
      തിരികെ load_dar<string>(thr, cp);
}

/*
 * %load/dar/vec4 <array-label>;
 */
തർക്കം of_LOAD_DAR_VEC4(vthread_t thr, vvp_code_t cp)
{
      തിരികെ load_dar<vvp_vector4_t>(thr, cp);
}

/*
 * %load/prop/dar/vec4 <pid>, <wid>;
 *   Indexed read of queue/dynamic-array class property; object on stack,
 *   index in words[3] (same as %load/dar/vec4).
 */
തർക്കം of_LOAD_PROP_DAR_VEC4(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->സംഖ്യ;
      unsigned wid = cp->bit_idx[0];
      int64_t adr = thr->വാക്കുകൾ[3].w_int;

      vvp_object_t& മുകൾ = thr->peek_object();
      vvp_cobject*cobj = മുകൾ.peek<vvp_cobject>();
      പ്രതിജ്ഞാ(cobj);

      vvp_object_t pobj;
      cobj->get_object(pid, pobj, 0);
      vvp_darray*darray = pobj.peek<vvp_darray>();

      vvp_vector4_t word;
      എങ്കിൽ (darray && (adr >= 0) && (thr->flags[4] == BIT4_0)) {
	    darray->get_word(adr, word);
      } അല്ലെങ്കിൽ {
	    dq_default(word, wid);
      }

      thr->തള്ളുക_vec4(word);
      തിരികെ സത്യം;
}

/*
 * %load/obj <var-label>
 */
തർക്കം of_LOAD_OBJ(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      പ്രതിജ്ഞാ(fun);

      vvp_object_t val = fun->get_object();
      thr->തള്ളുക_object(val);

      തിരികെ സത്യം;
}

/*
 * %load/obja <index>
 *    Loads the object from array, using index <index> as the index
 *    value. If flags[4] == 1, the calculation of <index> may have
 *    failed, so push nil.
 */
തർക്കം of_LOAD_OBJA(vthread_t thr, vvp_code_t cp)
{
      unsigned idx = cp->bit_idx[0];
      vvp_object_t word;

	/* The result is 0.0 if the address is undefined. */
      എങ്കിൽ (thr->flags[4] == BIT4_1) {
	    ; // Return nil
      } അല്ലെങ്കിൽ {
	    unsigned adr = thr->വാക്കുകൾ[idx].w_int;
	    cp->array->get_word_obj(adr, word);
      }

      thr->തള്ളുക_object(word);
      തിരികെ സത്യം;
}

/*
 * %load/real <var-label>
 */
തർക്കം of_LOAD_REAL(vthread_t thr, vvp_code_t cp)
{
      __vpiHandle*tmp = cp->handle;
      t_vpi_മൂല്യം val;

      val.format = vpiRealVal;
      vpi_get_മൂല്യം(tmp, &val);

      thr->തള്ളുക_real(val.value.real);

      തിരികെ സത്യം;
}

/*
 * %load/str <var-label>
 */
തർക്കം of_LOAD_STR(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;


      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (net->fun);
      പ്രതിജ്ഞാ(fun);

      സ്ഥിരം string&val = fun->get_string();
      thr->തള്ളുക_str(val);

      തിരികെ സത്യം;
}

തർക്കം of_LOAD_STRA(vthread_t thr, vvp_code_t cp)
{
      unsigned idx = cp->bit_idx[0];
      string word;

      എങ്കിൽ (thr->flags[4] == BIT4_1) {
	    word = "";
      } അല്ലെങ്കിൽ {
	    unsigned adr = thr->വാക്കുകൾ[idx].w_int;
	    word = cp->array->get_word_str(adr);
      }

      thr->തള്ളുക_str(word);
      തിരികെ സത്യം;
}


/*
 * %load/vec4 <net>
 */
തർക്കം of_LOAD_VEC4(vthread_t thr, vvp_code_t cp)
{
	// Push a placeholder onto the stack in order to reserve the
	// stack space. Use a reference for the stack top as a target
	// for the load.
      thr->തള്ളുക_vec4(vvp_vector4_t());
      vvp_vector4_t&sig_മൂല്യം = thr->peek_vec4();

      vvp_net_t*net = cp->net;

	// For the %load to work, the functor must actually be a
	// signal functor. Only signals save their vector value.
      vvp_signal_മൂല്യം*sig = dynamic_cast<vvp_signal_മൂല്യം*> (net->fil);
      എങ്കിൽ (sig == 0) {
	    cerr << thr->get_fileline()
	         << "%load/v error: Net arg not a signal? "
		 << (net->fil ? typeid(*net->fil).name() :
	                        typeid(*net->fun).name())
	         << endl;
	    പ്രതിജ്ഞാ(sig);
	    തിരികെ സത്യം;
      }

	// Extract the value from the signal and directly into the
	// target stack position.
      sig->vec4_മൂല്യം(sig_മൂല്യം);

      തിരികെ സത്യം;
}

/*
 * %load/vec4a <arr>, <adrx>
 */
തർക്കം of_LOAD_VEC4A(vthread_t thr, vvp_code_t cp)
{
      പൂർണ്ണസംഖ്യ adr_index = cp->bit_idx[0];

      long adr = thr->വാക്കുകൾ[adr_index].w_int;

	// If flag[3] is set, then the calculation of the address
	// failed, and this load should return X instead of the actual
	// value.
      എങ്കിൽ (thr->flags[4] == BIT4_1) {
	    vvp_vector4_t tmp (cp->array->get_word_size(), BIT4_X);
	    thr->തള്ളുക_vec4(tmp);
	    തിരികെ സത്യം;
      }

      vvp_vector4_t tmp (cp->array->get_word(adr));
      thr->തള്ളുക_vec4(tmp);
      തിരികെ സത്യം;
}

സ്ഥിതം ശൂന്യം do_verylong_mod(vvp_vector4_t&vala, സ്ഥിരം vvp_vector4_t&valb,
			    തർക്കം ഇടത്_is_neg, തർക്കം വലത്_is_neg)
{
      തർക്കം out_is_neg = ഇടത്_is_neg;
      സ്ഥിരം പൂർണ്ണസംഖ്യ len=vala.size();
      unsigned അക്ഷരം *a, *z, *t;
      a = പുതിയ unsigned അക്ഷരം[len+1];
      z = പുതിയ unsigned അക്ഷരം[len+1];
      t = പുതിയ unsigned അക്ഷരം[len+1];

      unsigned അക്ഷരം carry;
      unsigned അക്ഷരം temp;

      പൂർണ്ണസംഖ്യ mxa = -1, mxz = -1;
      പൂർണ്ണസംഖ്യ i;
      പൂർണ്ണസംഖ്യ current, copylen;

      unsigned lb_carry = ഇടത്_is_neg? 1 : 0;
      unsigned rb_carry = വലത്_is_neg? 1 : 0;
      വേണ്ടി (പൂർണ്ണസംഖ്യ idx = 0 ;  idx < len ;  idx += 1) {
	    unsigned lb = vala.value(idx);
	    unsigned rb = valb.value(idx);

	    എങ്കിൽ ((lb | rb) & 2) {
		  delete []t;
		  delete []z;
		  delete []a;
		  vvp_vector4_t tmp(len, BIT4_X);
		  vala = tmp;
		  തിരികെ;
	    }

	    എങ്കിൽ (ഇടത്_is_neg) {
		  lb = (1-lb) + lb_carry;
		  lb_carry = (lb & ~1)? 1 : 0;
		  lb &= 1;
	    }
	    എങ്കിൽ (വലത്_is_neg) {
		  rb = (1-rb) + rb_carry;
		  rb_carry = (rb & ~1)? 1 : 0;
		  rb &= 1;
	    }

	    z[idx]=lb;
	    a[idx]=1-rb;	// for 2s complement add..
      }

      z[len]=0;
      a[len]=1;

      വേണ്ടി(i=len-1;i>=0;i--) {
	    എങ്കിൽ(! a[i]) {
		  mxa=i;
		  നിർത്തുക;
	    }
      }

      വേണ്ടി(i=len-1;i>=0;i--) {
	    എങ്കിൽ(z[i]) {
		  mxz=i;
		  നിർത്തുക;
	    }
      }

      എങ്കിൽ((mxa>mxz)||(mxa==-1)) {
	    എങ്കിൽ(mxa==-1) {
		  delete []t;
		  delete []z;
		  delete []a;
		  vvp_vector4_t tmpx (len, BIT4_X);
		  vala = tmpx;
		  തിരികെ;
	    }

	    പോകുക tally;
      }

      copylen = mxa + 2;
      current = mxz - mxa;

      ആകുമ്പോൾ(current > -1) {
	    carry = 1;
	    വേണ്ടി(i=0;i<copylen;i++) {
		  temp = z[i+current] + a[i] + carry;
		  t[i] = (temp&1);
		  carry = (temp>>1);
	    }

	    എങ്കിൽ(carry) {
		  വേണ്ടി(i=0;i<copylen;i++) {
			z[i+current] = t[i];
		  }
	    }

	    current--;
      }

 tally:

      vvp_vector4_t tmp (len, BIT4_X);
      carry = out_is_neg? 1 : 0;
      വേണ്ടി (പൂർണ്ണസംഖ്യ idx = 0 ;  idx < len ;  idx += 1) {
	    unsigned ob = z[idx];
	    എങ്കിൽ (out_is_neg) {
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

തർക്കം of_MAX_WR(vthread_t thr, vvp_code_t)
{
      ഇരട്ട r = thr->എടുക്കുക_real();
      ഇരട്ട l = thr->എടുക്കുക_real();
      എങ്കിൽ (r != r)
	    thr->തള്ളുക_real(l);
      അല്ലെങ്കിൽ എങ്കിൽ (l != l)
	    thr->തള്ളുക_real(r);
      അല്ലെങ്കിൽ എങ്കിൽ (r < l)
	    thr->തള്ളുക_real(l);
      അല്ലെങ്കിൽ
	    thr->തള്ളുക_real(r);
      തിരികെ സത്യം;
}

തർക്കം of_MIN_WR(vthread_t thr, vvp_code_t)
{
      ഇരട്ട r = thr->എടുക്കുക_real();
      ഇരട്ട l = thr->എടുക്കുക_real();
      എങ്കിൽ (r != r)
	    thr->തള്ളുക_real(l);
      അല്ലെങ്കിൽ എങ്കിൽ (l != l)
	    thr->തള്ളുക_real(r);
      അല്ലെങ്കിൽ എങ്കിൽ (r < l)
	    thr->തള്ളുക_real(r);
      അല്ലെങ്കിൽ
	    thr->തള്ളുക_real(l);
      തിരികെ സത്യം;
}

തർക്കം of_MOD(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valb = thr->എടുക്കുക_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      പ്രതിജ്ഞാ(vala.size()==valb.size());
      unsigned wid = vala.size();

      എങ്കിൽ(wid <= 8*വലുപ്പം(unsigned long long)) {
	    unsigned long long lv = 0, rv = 0;

	    വേണ്ടി (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		  unsigned long long lb = vala.value(idx);
		  unsigned long long rb = valb.value(idx);

		  എങ്കിൽ ((lb | rb) & 2)
			പോകുക x_out;

		  lv |= (unsigned long long) lb << idx;
		  rv |= (unsigned long long) rb << idx;
	    }

	    എങ്കിൽ (rv == 0)
		  പോകുക x_out;

	    lv %= rv;

	    വേണ്ടി (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		  vala.set_bit(idx, (lv&1)?BIT4_1 : BIT4_0);
		  lv >>= 1;
	    }

	    തിരികെ സത്യം;

      } അല്ലെങ്കിൽ {
	    do_verylong_mod(vala, valb, അസത്യം, അസത്യം);
	    തിരികെ സത്യം;
      }

 x_out:
      vala = vvp_vector4_t(wid, BIT4_X);
      തിരികെ സത്യം;
}

/*
 * %mod/s
 */
തർക്കം of_MOD_S(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valb = thr->എടുക്കുക_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      പ്രതിജ്ഞാ(vala.size()==valb.size());
      unsigned wid = vala.size();

	/* Handle the case that we can fit the bits into a long-long
	   variable. We cause use native % to do the work. */
      എങ്കിൽ(wid <= 8*വലുപ്പം(long long)) {
	    long long lv = 0, rv = 0;

	    വേണ്ടി (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		  long long lb = vala.value(idx);
		  long long rb = valb.value(idx);

		  എങ്കിൽ ((lb | rb) & 2)
			പോകുക x_out;

		  lv |= (long long) lb << idx;
		  rv |= (long long) rb << idx;
	    }

	    എങ്കിൽ (rv == 0)
		  പോകുക x_out;

	    എങ്കിൽ ((lv == LLONG_MIN) && (rv == -1))
		  പോകുക zero_out;

	      /* Sign extend the signed operands when needed. */
	    എങ്കിൽ (wid < 8*വലുപ്പം(long long)) {
		  എങ്കിൽ (lv & (1LL << (wid-1)))
			lv |= -1ULL << wid;
		  എങ്കിൽ (rv & (1LL << (wid-1)))
			rv |= -1ULL << wid;
	    }

	    lv %= rv;

	    വേണ്ടി (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		  vala.set_bit(idx, (lv&1)? BIT4_1 : BIT4_0);
		  lv >>= 1;
	    }

	      // vala is the top of the stack, edited in place, so we
	      // do not need to push the result.

	    തിരികെ സത്യം;

      } അല്ലെങ്കിൽ {

	    തർക്കം ഇടത്_is_neg  = vala.value(vala.size()-1) == BIT4_1;
	    തർക്കം വലത്_is_neg = valb.value(valb.size()-1) == BIT4_1;
	    do_verylong_mod(vala, valb, ഇടത്_is_neg, വലത്_is_neg);
	    തിരികെ സത്യം;
      }

 x_out:
      vala = vvp_vector4_t(wid, BIT4_X);
      തിരികെ സത്യം;
 zero_out:
      vala = vvp_vector4_t(wid, BIT4_0);
      തിരികെ സത്യം;
}

/*
 * %mod/wr
 */
തർക്കം of_MOD_WR(vthread_t thr, vvp_code_t)
{
      ഇരട്ട r = thr->എടുക്കുക_real();
      ഇരട്ട l = thr->എടുക്കുക_real();
      thr->തള്ളുക_real(fmod(l,r));

      തിരികെ സത്യം;
}

/*
 * %neg/wr
 */
തർക്കം of_NEG_WR(vthread_t thr, vvp_code_t)
{
      thr->poke_real(0, -thr->peek_real(0));
      തിരികെ സത്യം;
}

/*
 * %pad/s <wid>
 */
തർക്കം of_PAD_S(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->സംഖ്യ;

      vvp_vector4_t&val = thr->peek_vec4();
      unsigned old_size = val.size();

	// Sign-extend.
      എങ്കിൽ (old_size < wid)
	    val.resize(wid, val.value(old_size-1));
      അല്ലെങ്കിൽ
	    val.resize(wid);

      തിരികെ സത്യം;
}

/*
 * %pad/u <wid>
 */
തർക്കം of_PAD_U(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->സംഖ്യ;

      vvp_vector4_t&val = thr->peek_vec4();
      val.resize(wid, BIT4_0);

      തിരികെ സത്യം;
}

/*
 * %part/s <wid>
 * %part/u <wid>
 * Two values are popped from the stack. First, pop the canonical
 * index of the part select, and second is the value to be
 * selected. The result is pushed back to the stack.
 */
സ്ഥിതം തർക്കം of_PART_base(vthread_t thr, vvp_code_t cp, തർക്കം signed_flag)
{
      unsigned wid = cp->സംഖ്യ;

      vvp_vector4_t base4 = thr->എടുക്കുക_vec4();
      vvp_vector4_t&value = thr->peek_vec4();

      vvp_vector4_t res (wid, BIT4_X);

	// NOTE: This is treating the vector as signed. Is that correct?
      int32_t base;
      തർക്കം മൂല്യം_ok = vector4_to_മൂല്യം(base4, base, signed_flag);
      എങ്കിൽ (! മൂല്യം_ok) {
	    value = res;
	    തിരികെ സത്യം;
      }

      എങ്കിൽ (base >= (int32_t)value.size()) {
	    value = res;
	    തിരികെ സത്യം;
      }

      എങ്കിൽ ((base+(പൂർണ്ണസംഖ്യ)wid) <= 0) {
	    value = res;
	    തിരികെ സത്യം;
      }

      long vbase = 0;
      എങ്കിൽ (base < 0) {
	    vbase = -base;
	    wid -= vbase;
	    base = 0;
      }

      എങ്കിൽ ((base+wid) > value.size()) {
	    wid = value.size() - base;
      }

      res .set_vec(vbase, value.subvalue(base, wid));
      value = res;

      തിരികെ സത്യം;
}

തർക്കം of_PART_S(vthread_t thr, vvp_code_t cp)
{
      തിരികെ of_PART_base(thr, cp, സത്യം);
}

തർക്കം of_PART_U(vthread_t thr, vvp_code_t cp)
{
      തിരികെ of_PART_base(thr, cp, അസത്യം);
}

/*
 * %parti/s <wid>, <basei>, <base_wid>
 * %parti/u <wid>, <basei>, <base_wid>
 *
 * Pop the value to be selected. The result is pushed back to the stack.
 */
സ്ഥിതം തർക്കം of_PARTI_base(vthread_t thr, vvp_code_t cp, തർക്കം signed_flag)
{
      unsigned wid = cp->സംഖ്യ;
      uint32_t base = cp->bit_idx[0];
      uint32_t bwid = cp->bit_idx[1];

      vvp_vector4_t&value = thr->peek_vec4();

      vvp_vector4_t res (wid, BIT4_X);

	// NOTE: This is treating the vector as signed. Is that correct?
      int32_t use_base = base;
      എങ്കിൽ (signed_flag && bwid < 32 && (base&(1<<(bwid-1)))) {
	    use_base |= -1UL << bwid;
      }

      എങ്കിൽ (use_base >= (int32_t)value.size()) {
	    value = res;
	    തിരികെ സത്യം;
      }

      എങ്കിൽ ((use_base+(int32_t)wid) <= 0) {
	    value = res;
	    തിരികെ സത്യം;
      }

      long vbase = 0;
      എങ്കിൽ (use_base < 0) {
	    vbase = -use_base;
	    wid -= vbase;
	    use_base = 0;
      }

      എങ്കിൽ ((use_base+wid) > value.size()) {
	    wid = value.size() - use_base;
      }

      res .set_vec(vbase, value.subvalue(use_base, wid));
      value = res;

      തിരികെ സത്യം;
}

തർക്കം of_PARTI_S(vthread_t thr, vvp_code_t cp)
{
      തിരികെ of_PARTI_base(thr, cp, സത്യം);
}

തർക്കം of_PARTI_U(vthread_t thr, vvp_code_t cp)
{
      തിരികെ of_PARTI_base(thr, cp, അസത്യം);
}

/*
 * %mul
 */
തർക്കം of_MUL(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t r = thr->എടുക്കുക_vec4();
	// Rather then pop l, use it directly from the stack. When we
	// assign to 'l', that will edit the top of the stack, which
	// replaces a pop and a pull.
      vvp_vector4_t&l = thr->peek_vec4();

      l.mul(r);
      തിരികെ സത്യം;
}

/*
 * %muli <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
തർക്കം of_MULI(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->സംഖ്യ;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_immediate_rval (cp, r);

      l.mul(r);
      തിരികെ സത്യം;
}

തർക്കം of_MUL_WR(vthread_t thr, vvp_code_t)
{
      ഇരട്ട r = thr->എടുക്കുക_real();
      ഇരട്ട l = thr->എടുക്കുക_real();
      thr->തള്ളുക_real(l * r);

      തിരികെ സത്യം;
}

തർക്കം of_NAND(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valr = thr->എടുക്കുക_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      പ്രതിജ്ഞാ(vall.size() == valr.size());

      vall &= valr;
      vall.invert();

      തിരികെ സത്യം;
}

/*
 * %new/cobj <vpi_object>
 * This creates a new cobject (SystemVerilog class object) and pushes
 * it to the stack. The <vpi-object> is a __vpiHandle that is a
 * vpiClassDefn object that defines the item to be created.
 */
തർക്കം of_NEW_COBJ(vthread_t thr, vvp_code_t cp)
{
      സ്ഥിരം class_type*defn = dynamic_cast<സ്ഥിരം class_type*> (cp->handle);
      പ്രതിജ്ഞാ(defn);

      vvp_object_t tmp (പുതിയ vvp_cobject(defn));
      thr->തള്ളുക_object(tmp);
      തിരികെ സത്യം;
}

തർക്കം of_NEW_DARRAY(vthread_t thr, vvp_code_t cp)
{
      സ്ഥിരം അക്ഷരം*text = cp->text;
      size_t size = thr->വാക്കുകൾ[cp->bit_idx[0]].w_int;
      unsigned word_wid;
      size_t n;

      vvp_object_t obj;
      എങ്കിൽ (strcmp(text,"b8") == 0) {
	    obj = പുതിയ vvp_darray_atom<uint8_t>(size);
      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(text,"b16") == 0) {
	    obj = പുതിയ vvp_darray_atom<uint16_t>(size);
      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(text,"b32") == 0) {
	    obj = പുതിയ vvp_darray_atom<uint32_t>(size);
      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(text,"b64") == 0) {
	    obj = പുതിയ vvp_darray_atom<uint64_t>(size);
      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(text,"sb8") == 0) {
	    obj = പുതിയ vvp_darray_atom<int8_t>(size);
      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(text,"sb16") == 0) {
	    obj = പുതിയ vvp_darray_atom<int16_t>(size);
      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(text,"sb32") == 0) {
	    obj = പുതിയ vvp_darray_atom<int32_t>(size);
      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(text,"sb64") == 0) {
	    obj = പുതിയ vvp_darray_atom<int64_t>(size);
      } അല്ലെങ്കിൽ എങ്കിൽ ((1 == sscanf(text, "b%u%zn", &word_wid, &n)) &&
                 (n == strlen(text))) {
	    obj = പുതിയ vvp_darray_vec2(size, word_wid);
      } അല്ലെങ്കിൽ എങ്കിൽ ((1 == sscanf(text, "sb%u%zn", &word_wid, &n)) &&
                 (n == strlen(text))) {
	    obj = പുതിയ vvp_darray_vec2(size, word_wid);
      } അല്ലെങ്കിൽ എങ്കിൽ ((1 == sscanf(text, "v%u%zn", &word_wid, &n)) &&
                 (n == strlen(text))) {
	    obj = പുതിയ vvp_darray_vec4(size, word_wid);
      } അല്ലെങ്കിൽ എങ്കിൽ ((1 == sscanf(text, "sv%u%zn", &word_wid, &n)) &&
                 (n == strlen(text))) {
	    obj = പുതിയ vvp_darray_vec4(size, word_wid);
      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(text,"r") == 0) {
	    obj = പുതിയ vvp_darray_real(size);
      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(text,"S") == 0) {
	    obj = പുതിയ vvp_darray_string(size);
      } അല്ലെങ്കിൽ {
	    cerr << get_fileline()
	         << "Internal error: Unsupported dynamic array type: "
	         << text << "." << endl;
	    പ്രതിജ്ഞാ(0);
      }

      thr->തള്ളുക_object(obj);

      തിരികെ സത്യം;
}

തർക്കം of_NOOP(vthread_t, vvp_code_t)
{
      തിരികെ സത്യം;
}

/*
 * %nor/r
 */
തർക്കം of_NORR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_or());

      തിരികെ സത്യം;
}

/*
 * Push a null to the object stack.
 */
തർക്കം of_NULL(vthread_t thr, vvp_code_t)
{
      vvp_object_t tmp;
      thr->തള്ളുക_object(tmp);
      തിരികെ സത്യം;
}

/*
 * %and/r
 */
തർക്കം of_ANDR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_and());

      തിരികെ സത്യം;
}

/*
 * %nand/r
 */
തർക്കം of_NANDR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_and());

      തിരികെ സത്യം;
}

/*
 * %or/r
 */
തർക്കം of_ORR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_or());

      തിരികെ സത്യം;
}

/*
 * %xor/r
 */
തർക്കം of_XORR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_xor());

      തിരികെ സത്യം;
}

/*
 * %xnor/r
 */
തർക്കം of_XNORR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_xor());

      തിരികെ സത്യം;
}

/*
 * %or
 */
തർക്കം of_OR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valb = thr->എടുക്കുക_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();
      vala |= valb;
      തിരികെ സത്യം;
}

/*
 * %nor
 */
തർക്കം of_NOR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valr = thr->എടുക്കുക_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      പ്രതിജ്ഞാ(vall.size() == valr.size());

      vall |= valr;
      vall.invert();

      തിരികെ സത്യം;
}

/*
 * %pop/obj <num>, <skip>
 */
തർക്കം of_എടുക്കുക_OBJ(vthread_t thr, vvp_code_t cp)
{
      unsigned cnt = cp->bit_idx[0];
      unsigned skip = cp->bit_idx[1];

      thr->എടുക്കുക_object(cnt, skip);
      തിരികെ സത്യം;
}

/*
 * %pop/real <number>
 */
തർക്കം of_എടുക്കുക_REAL(vthread_t thr, vvp_code_t cp)
{
      unsigned cnt = cp->സംഖ്യ;
      thr->എടുക്കുക_real(cnt);
      തിരികെ സത്യം;
}

/*
 *  %pop/str <number>
 */
തർക്കം of_എടുക്കുക_STR(vthread_t thr, vvp_code_t cp)
{
      unsigned cnt = cp->സംഖ്യ;
      thr->എടുക്കുക_str(cnt);
      തിരികെ സത്യം;
}

/*
 *  %pop/vec4 <number>
 */
തർക്കം of_എടുക്കുക_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned cnt = cp->സംഖ്യ;
      thr->എടുക്കുക_vec4(cnt);
      തിരികെ സത്യം;
}

/*
 * %pow
 * %pow/s
 */
സ്ഥിതം തർക്കം of_POW_base(vthread_t thr, തർക്കം signed_flag)
{
      vvp_vector4_t valb = thr->എടുക്കുക_vec4();
      vvp_vector4_t vala = thr->എടുക്കുക_vec4();

      unsigned wid = vala.size();

      vvp_vector2_t xv2 = vvp_vector2_t(vala, സത്യം);
      vvp_vector2_t yv2 = vvp_vector2_t(valb, സത്യം);


        /* If we have an X or Z in the arguments return X. */
      എങ്കിൽ (xv2.is_NaN() || yv2.is_NaN()) {
	    vvp_vector4_t tmp (wid, BIT4_X);
	    thr->തള്ളുക_vec4(tmp);
	    തിരികെ സത്യം;
      }

	// Is the exponent negative? If so, table 5-6 in IEEE1364-2005
	// defines what value is returned.
      എങ്കിൽ (signed_flag && yv2.value(yv2.size()-1)) {
	    പൂർണ്ണസംഖ്യ a_val;
	    vvp_bit4_t pad = BIT4_0, lsb = BIT4_0;
	    എങ്കിൽ (vector2_to_മൂല്യം(xv2, a_val, സത്യം)) {
		  എങ്കിൽ (a_val == 0) {
			pad = BIT4_X; lsb = BIT4_X;
		  }
		  എങ്കിൽ (a_val == 1) {
			pad = BIT4_0; lsb = BIT4_1;
		  }
		  എങ്കിൽ (a_val == -1) {
			എങ്കിൽ (yv2.value(0)) {
			      pad = BIT4_1; lsb = BIT4_1;
			} അല്ലെങ്കിൽ {
			      pad = BIT4_0; lsb = BIT4_1;
			}
		  }
	    }
	    vvp_vector4_t tmp (wid, pad);
	    tmp.set_bit(0, lsb);
	    thr->തള്ളുക_vec4(tmp);
	    തിരികെ സത്യം;
      }

      vvp_vector2_t result = pow(xv2, yv2);

        /* Copy only what we need of the result. If the result is too
	   small, zero-pad it. */
      വേണ്ടി (unsigned jdx = 0;  jdx < wid;  jdx += 1) {
	    എങ്കിൽ (jdx >= result.size())
		  vala.set_bit(jdx, BIT4_0);
	    അല്ലെങ്കിൽ
		  vala.set_bit(jdx, result.value(jdx) ? BIT4_1 : BIT4_0);
      }
      thr->തള്ളുക_vec4(vala);

      തിരികെ സത്യം;
}

തർക്കം of_POW(vthread_t thr, vvp_code_t)
{
      തിരികെ of_POW_base(thr, അസത്യം);
}

തർക്കം of_POW_S(vthread_t thr, vvp_code_t)
{
      തിരികെ of_POW_base(thr, സത്യം);
}

തർക്കം of_POW_WR(vthread_t thr, vvp_code_t)
{
      ഇരട്ട r = thr->എടുക്കുക_real();
      ഇരട്ട l = thr->എടുക്കുക_real();
      thr->തള്ളുക_real(pow(l,r));

      തിരികെ സത്യം;
}

/*
 * %prop/obj <pid>, <idx>
 *
 * Load an object value from the cobject and push it onto the object stack.
 */
തർക്കം of_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      unsigned pid = cp->സംഖ്യ;
      unsigned idx = cp->bit_idx[0];

      എങ്കിൽ (idx != 0) {
	    പ്രതിജ്ഞാ(idx < vthread_s::വാക്കുകൾ_എണ്ണം);
	    idx = thr->വാക്കുകൾ[idx].w_uint;
      }

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();

      vvp_object_t val;
      cobj->get_object(pid, val, idx);

      thr->തള്ളുക_object(val);

      തിരികെ സത്യം;
}

സ്ഥിതം ശൂന്യം get_from_obj(unsigned pid, vvp_cobject*cobj, ഇരട്ട&val)
{
      val = cobj->get_real(pid);
}

സ്ഥിതം ശൂന്യം get_from_obj(unsigned pid, vvp_cobject*cobj, string&val)
{
      val = cobj->get_string(pid);
}

സ്ഥിതം ശൂന്യം get_from_obj(unsigned pid, vvp_cobject*cobj, vvp_vector4_t&val)
{
      cobj->get_vec4(pid, val);
}

template <typename ELEM>
സ്ഥിതം തർക്കം prop(vthread_t thr, vvp_code_t cp)
{
      unsigned pid = cp->സംഖ്യ;

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      പ്രതിജ്ഞാ(cobj);

      ELEM val;
      get_from_obj(pid, cobj, val);
      vthread_തള്ളുക(thr, val);

      തിരികെ സത്യം;
}

/*
 * %prop/r <pid>
 *
 * Load a real value from the cobject and push it onto the real value
 * stack.
 */
തർക്കം of_PROP_R(vthread_t thr, vvp_code_t cp)
{
      തിരികെ prop<ഇരട്ട>(thr, cp);
}

/*
 * %prop/str <pid>
 *
 * Load a string value from the cobject and push it onto the real value
 * stack.
 */
തർക്കം of_PROP_STR(vthread_t thr, vvp_code_t cp)
{
      തിരികെ prop<string>(thr, cp);
}

/*
 * %prop/v <pid>
 *
 * Load a property <pid> from the cobject on the top of the stack into
 * the vector space at <base>.
 */
തർക്കം of_PROP_V(vthread_t thr, vvp_code_t cp)
{
      തിരികെ prop<vvp_vector4_t>(thr, cp);
}

തർക്കം of_PUSHI_REAL(vthread_t thr, vvp_code_t cp)
{
      ഇരട്ട mant = cp->bit_idx[0];
      uint32_t imant = cp->bit_idx[0];
      പൂർണ്ണസംഖ്യ exp = cp->bit_idx[1];

	// Detect +infinity
      എങ്കിൽ (exp==0x3fff && imant==0) {
	    thr->തള്ളുക_real(INFINITY);
	    തിരികെ സത്യം;
      }
	// Detect -infinity
      എങ്കിൽ (exp==0x7fff && imant==0) {
	    thr->തള്ളുക_real(-INFINITY);
	    തിരികെ സത്യം;
      }
	// Detect NaN
      എങ്കിൽ (exp==0x3fff) {
	    thr->തള്ളുക_real(nan(""));
	    തിരികെ സത്യം;
      }

      ഇരട്ട sign = (exp & 0x4000)? -1.0 : 1.0;

      exp &= 0x1fff;

      mant = sign * ldexp(mant, exp - 0x1000);
      thr->തള്ളുക_real(mant);
      തിരികെ സത്യം;
}

തർക്കം of_PUSHI_STR(vthread_t thr, vvp_code_t cp)
{
      സ്ഥിരം അക്ഷരം*text = cp->text;
      thr->തള്ളുക_str(filter_string(text));
      തിരികെ സത്യം;
}

/*
 * %pushi/vec4 <vala>, <valb>, <wid>
 */
തർക്കം of_PUSHI_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned wid  = cp->സംഖ്യ;

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t val (wid, BIT4_0);
      get_immediate_rval (cp, val);

      thr->തള്ളുക_vec4(val);

      തിരികെ സത്യം;
}

/*
 * %pushv/str
 *   Pops a vec4 value, and pushes a string.
 */
തർക്കം of_PUSHV_STR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t vec = thr->എടുക്കുക_vec4();

      size_t slen = (vec.size() + 7)/8;
      vector<അക്ഷരം>buf;
      buf.reserve(slen);

      വേണ്ടി (size_t idx = 0 ; idx < vec.size() ; idx += 8) {
	    അക്ഷരം tmp = 0;
	    size_t trans = 8;
	    എങ്കിൽ (idx+trans > vec.size())
		  trans = vec.size() - idx;

	    വേണ്ടി (size_t bdx = 0 ; bdx < trans ; bdx += 1) {
		  എങ്കിൽ (vec.value(idx+bdx) == BIT4_1)
			tmp |= 1 << bdx;
	    }

	    എങ്കിൽ (tmp != 0)
		  buf.push_back(tmp);
      }

      string val;
      വേണ്ടി (vector<അക്ഷരം>::reverse_iterator cur = buf.rbegin()
		 ; cur != buf.rend() ; ++cur) {
	    val.push_back(*cur);
      }

      thr->തള്ളുക_str(val);

      തിരികെ സത്യം;
}

/*
 * %putc/str/vec4 <var>, <mux>
 */
തർക്കം of_PUTC_STR_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned muxr = cp->bit_idx[0];
      int32_t mux = muxr? thr->വാക്കുകൾ[muxr].w_int : 0;

      vvp_vector4_t val = thr->എടുക്കുക_vec4();
      പ്രതിജ്ഞാ(val.size() == 8);

      എങ്കിൽ (mux < 0)
	    തിരികെ സത്യം;

	/* Get the existing value of the string. If we find that the
	   index is too big for the string, then give up. */
      vvp_net_t*net = cp->net;
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (net->fun);
      പ്രതിജ്ഞാ(fun);

      string tmp = fun->get_string();
      എങ്കിൽ (tmp.size() <= (size_t)mux)
	    തിരികെ സത്യം;

      അക്ഷരം val_str = 0;
      വേണ്ടി (size_t idx = 0 ; idx < 8 ; idx += 1) {
	    എങ്കിൽ (val.value(idx)==BIT4_1)
		  val_str |= 1<<idx;
      }

	// It is a quirk of the Verilog standard that putc(..., 'h00)
	// has no effect. Test for that case here.
      എങ്കിൽ (val_str == 0)
	    തിരികെ സത്യം;

      tmp[mux] = val_str;

      vvp_send_string(vvp_net_ptr_t(cp->net, 0), tmp, thr->wt_context);
      തിരികെ സത്യം;
}

template <typename ELEM, വർഗ്ഗം QTYPE>
സ്ഥിതം തർക്കം qinsert(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
      int64_t idx = thr->വാക്കുകൾ[3].w_int;
      ELEM value;
      vvp_net_t*net = cp->net;
      എടുക്കുക_മൂല്യം(thr, value, wid); // Pop the value to store.

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      പ്രതിജ്ഞാ(queue);
      എങ്കിൽ (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at a negative "
	         << get_queue_type(value)
	         << " index (" << idx << "). ";
	    print_queue_മൂല്യം(value);
	    cerr << " was not added." << endl;
      } അല്ലെങ്കിൽ എങ്കിൽ (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at an undefined "
	         << get_queue_type(value) << " index. ";
	    print_queue_മൂല്യം(value);
	    cerr << " was not added." << endl;
      } അല്ലെങ്കിൽ {
	    unsigned max_size = thr->വാക്കുകൾ[cp->bit_idx[0]].w_int;
	    queue->insert(idx, value, max_size);
      }
      തിരികെ സത്യം;
}

/*
 * %qinsert/real <var-label>
 */
തർക്കം of_QINSERT_REAL(vthread_t thr, vvp_code_t cp)
{
      തിരികെ qinsert<ഇരട്ട, vvp_queue_real>(thr, cp);
}

/*
 * %qinsert/str <var-label>
 */
തർക്കം of_QINSERT_STR(vthread_t thr, vvp_code_t cp)
{
      തിരികെ qinsert<string, vvp_queue_string>(thr, cp);
}

/*
 * %qinsert/v <var-label>
 */
തർക്കം of_QINSERT_V(vthread_t thr, vvp_code_t cp)
{
      തിരികെ qinsert<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

/*
 * Helper functions used in the queue pop templates
 */
വരിയിൽ ശൂന്യം തള്ളുക_മൂല്യം(vthread_t thr, ഇരട്ട value, unsigned)
{
      thr->തള്ളുക_real(value);
}

വരിയിൽ ശൂന്യം തള്ളുക_മൂല്യം(vthread_t thr, സ്ഥിരം string&value, unsigned)
{
      thr->തള്ളുക_str(value);
}

വരിയിൽ ശൂന്യം തള്ളുക_മൂല്യം(vthread_t thr, സ്ഥിരം vvp_vector4_t&value, unsigned wid)
{
      പ്രതിജ്ഞാ(wid == value.size());
      thr->തള്ളുക_vec4(value);
}

template <typename ELEM, വർഗ്ഗം QTYPE>
സ്ഥിതം തർക്കം q_എടുക്കുക(vthread_t thr, vvp_code_t cp,
                  ശൂന്യം (*get_val_func)(vvp_queue*, ELEM&),
                  സ്ഥിരം അക്ഷരം*loc, unsigned wid)
{
      vvp_net_t*net = cp->net;

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      പ്രതിജ്ഞാ(queue);

      size_t size = queue->get_size();

      ELEM value;
      എങ്കിൽ (size) {
	    get_val_func(queue, value);
      } അല്ലെങ്കിൽ {
	    dq_default(value, wid);
	    cerr << thr->get_fileline()
	         << "Warning: pop_" << loc << "() on empty "
	         << get_queue_type(value) << "." << endl;
      }

      തള്ളുക_മൂല്യം(thr, value, wid);
      തിരികെ സത്യം;
}

template <typename ELEM>
സ്ഥിതം ശൂന്യം get_back_മൂല്യം(vvp_queue*queue, ELEM&value)
{
      queue->get_word(queue->get_size()-1, value);
      queue->pop_back();
}

template <typename ELEM, വർഗ്ഗം QTYPE>
സ്ഥിതം തർക്കം qpop_b(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
      തിരികെ q_എടുക്കുക<ELEM, QTYPE>(thr, cp, get_back_മൂല്യം<ELEM>, "back", wid);
}

/*
 * %qpop/b/real <var-label>
 */
തർക്കം of_QPOP_B_REAL(vthread_t thr, vvp_code_t cp)
{
      തിരികെ qpop_b<ഇരട്ട, vvp_queue_real>(thr, cp);
}

/*
 * %qpop/b/str <var-label>
 */
തർക്കം of_QPOP_B_STR(vthread_t thr, vvp_code_t cp)
{
      തിരികെ qpop_b<string, vvp_queue_string>(thr, cp);
}

/*
 * %qpop/b/v <var-label>
 */
തർക്കം of_QPOP_B_V(vthread_t thr, vvp_code_t cp)
{
      തിരികെ qpop_b<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[0]);
}

template <typename ELEM>
സ്ഥിതം ശൂന്യം get_front_മൂല്യം(vvp_queue*queue, ELEM&value)
{
      queue->get_word(0, value);
      queue->pop_front();
}

template <typename ELEM, വർഗ്ഗം QTYPE>
സ്ഥിതം തർക്കം qpop_f(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
      തിരികെ q_എടുക്കുക<ELEM, QTYPE>(thr, cp, get_front_മൂല്യം<ELEM>, "front", wid);
}


/*
 * %qpop/f/real <var-label>
 */
തർക്കം of_QPOP_F_REAL(vthread_t thr, vvp_code_t cp)
{
      തിരികെ qpop_f<ഇരട്ട, vvp_queue_real>(thr, cp);
}

/*
 * %qpop/f/str <var-label>
 */
തർക്കം of_QPOP_F_STR(vthread_t thr, vvp_code_t cp)
{
      തിരികെ qpop_f<string, vvp_queue_string>(thr, cp);
}

/*
 * %qpop/f/v <var-label>
 */
തർക്കം of_QPOP_F_V(vthread_t thr, vvp_code_t cp)
{
      തിരികെ qpop_f<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[0]);
}

/*
 * These implement the %release/net and %release/reg instructions. The
 * %release/net instruction applies to a net kind of functor by
 * sending the release/net command to the command port. (See vvp_net.h
 * for details.) The %release/reg instruction is the same, but sends
 * the release/reg command instead. These are very similar to the
 * %deassign instruction.
 */
സ്ഥിതം തർക്കം do_release_vec(vvp_code_t cp, തർക്കം net_flag)
{
      vvp_net_t*net = cp->net;
      unsigned base  = cp->bit_idx[0];
      unsigned width = cp->bit_idx[1];

      പ്രതിജ്ഞാ(net->fil);

      എങ്കിൽ (base >= net->fil->filter_size()) തിരികെ സത്യം;
      എങ്കിൽ (base+width > net->fil->filter_size())
	    width = net->fil->filter_size() - base;

      തർക്കം full_sig = base == 0 && width == net->fil->filter_size();

	// XXXX Can't really do this if this is a partial release?
      net->fil->force_unlink();

	/* Do we release all or part of the net? */
      vvp_net_ptr_t ptr (net, 0);
      എങ്കിൽ (full_sig) {
	    net->fil->release(ptr, net_flag);
      } അല്ലെങ്കിൽ {
	    net->fil->release_pv(ptr, base, width, net_flag);
      }
      net->fun->force_flag(അസത്യം);

      തിരികെ സത്യം;
}

തർക്കം of_RELEASE_NET(vthread_t, vvp_code_t cp)
{
      തിരികെ do_release_vec(cp, സത്യം);
}


തർക്കം of_RELEASE_REG(vthread_t, vvp_code_t cp)
{
      തിരികെ do_release_vec(cp, അസത്യം);
}

/* The type is 1 for registers and 0 for everything else. */
തർക്കം of_RELEASE_WR(vthread_t, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      unsigned type  = cp->bit_idx[0];

      പ്രതിജ്ഞാ(net->fil);
      net->fil->force_unlink();

	// Send a command to this signal to unforce itself.
      vvp_net_ptr_t ptr (net, 0);
      net->fil->release(ptr, type==0);
      തിരികെ സത്യം;
}

തർക്കം of_REPLICATE(vthread_t thr, vvp_code_t cp)
{
      പൂർണ്ണസംഖ്യ rept = cp->സംഖ്യ;
      vvp_vector4_t val = thr->എടുക്കുക_vec4();
      vvp_vector4_t res (val.size() * rept, BIT4_X);

      വേണ്ടി (പൂർണ്ണസംഖ്യ idx = 0 ; idx < rept ; idx += 1) {
	    res.set_vec(idx * val.size(), val);
      }

      thr->തള്ളുക_vec4(res);

      തിരികെ സത്യം;
}

സ്ഥിതം ശൂന്യം poke_val(vthread_t fun_thr, unsigned depth, ഇരട്ട val)
{
      fun_thr->parent->poke_real(depth, val);
}

സ്ഥിതം ശൂന്യം poke_val(vthread_t fun_thr, unsigned depth, സ്ഥിരം string&val)
{
      fun_thr->parent->poke_str(depth, val);
}

സ്ഥിതം size_t get_max(vthread_t fun_thr, ഇരട്ട&)
{
      തിരികെ fun_thr->args_real.size();
}

സ്ഥിതം size_t get_max(vthread_t fun_thr, string&)
{
      തിരികെ fun_thr->args_str.size();
}

സ്ഥിതം size_t get_max(vthread_t fun_thr, vvp_vector4_t&)
{
      തിരികെ fun_thr->args_vec4.size();
}

സ്ഥിതം unsigned get_depth(vthread_t fun_thr, size_t index, ഇരട്ട&)
{
      തിരികെ fun_thr->args_real[index];
}

സ്ഥിതം unsigned get_depth(vthread_t fun_thr, size_t index, string&)
{
      തിരികെ fun_thr->args_str[index];
}

സ്ഥിതം unsigned get_depth(vthread_t fun_thr, size_t index, vvp_vector4_t&)
{
      തിരികെ fun_thr->args_vec4[index];
}

സ്ഥിതം vthread_t get_func(vthread_t thr)
{
      vthread_t fun_thr = thr;

      ആകുമ്പോൾ (fun_thr->parent_scope->get_type_code() != vpiFunction) {
	    പ്രതിജ്ഞാ(fun_thr->parent);
	    fun_thr = fun_thr->parent;
      }

      തിരികെ fun_thr;
}

template <typename ELEM>
സ്ഥിതം തർക്കം ret(vthread_t thr, vvp_code_t cp)
{
      size_t index = cp->സംഖ്യ;
      ELEM val;
      എടുക്കുക_മൂല്യം(thr, val, 0);

      vthread_t fun_thr = get_func(thr);
      പ്രതിജ്ഞാ(index < get_max(fun_thr, val));

      unsigned depth = get_depth(fun_thr, index, val);
	// Use the depth to put the value into the stack of
	// the parent thread.
      poke_val(fun_thr, depth, val);
      തിരികെ സത്യം;
}

/*
 * %ret/real <index>
 */
തർക്കം of_RET_REAL(vthread_t thr, vvp_code_t cp)
{
      തിരികെ ret<ഇരട്ട>(thr, cp);
}

/*
 * %ret/str <index>
 */
തർക്കം of_RET_STR(vthread_t thr, vvp_code_t cp)
{
      തിരികെ ret<string>(thr, cp);
}

/*
 * %ret/vec4 <index>, <offset>, <wid>
 */
തർക്കം of_RET_VEC4(vthread_t thr, vvp_code_t cp)
{
      size_t index = cp->സംഖ്യ;
      unsigned off_index = cp->bit_idx[0];
      unsigned പൂർണ്ണസംഖ്യ wid = cp->bit_idx[1];
      vvp_vector4_t&val = thr->peek_vec4();

      vthread_t fun_thr = get_func(thr);
      പ്രതിജ്ഞാ(index < get_max(fun_thr, val));
      പ്രതിജ്ഞാ(val.size() == wid);
      unsigned depth = get_depth(fun_thr, index, val);

      int64_t off = off_index ? thr->വാക്കുകൾ[off_index].w_int : 0;
      unsigned പൂർണ്ണസംഖ്യ sig_മൂല്യം_size = fun_thr->parent->peek_vec4(depth).size();

      എങ്കിൽ (off_index!=0 && thr->flags[4] == BIT4_1) {
	    thr->എടുക്കുക_vec4(1);
	    തിരികെ സത്യം;
      }

      എങ്കിൽ (!resize_rval_vec(val, off, sig_മൂല്യം_size)) {
	    thr->എടുക്കുക_vec4(1);
	    തിരികെ സത്യം;
      }

      എങ്കിൽ (off == 0 && val.size() == sig_മൂല്യം_size) {
	    fun_thr->parent->poke_vec4(depth, val);
      } അല്ലെങ്കിൽ {
	    vvp_vector4_t tmp_dst = fun_thr->parent->peek_vec4(depth);
	    tmp_dst.set_vec(off, val);
	    fun_thr->parent->poke_vec4(depth, tmp_dst);
      }

      thr->എടുക്കുക_vec4(1);
      തിരികെ സത്യം;
}

സ്ഥിതം ശൂന്യം തള്ളുക_from_parent(vthread_t thr, vthread_t fun_thr, unsigned depth, ഇരട്ട&)
{
      thr->തള്ളുക_real(fun_thr->parent->peek_real(depth));
}

സ്ഥിതം ശൂന്യം തള്ളുക_from_parent(vthread_t thr, vthread_t fun_thr, unsigned depth, string&)
{
      thr->തള്ളുക_str(fun_thr->parent->peek_str(depth));
}

സ്ഥിതം ശൂന്യം തള്ളുക_from_parent(vthread_t thr, vthread_t fun_thr, unsigned depth, vvp_vector4_t&)
{
      thr->തള്ളുക_vec4(fun_thr->parent->peek_vec4(depth));
}

template <typename ELEM>
സ്ഥിതം തർക്കം retload(vthread_t thr, vvp_code_t cp)
{
      size_t index = cp->സംഖ്യ;
      ELEM type;

      vthread_t fun_thr = get_func(thr);
      പ്രതിജ്ഞാ(index < get_max(fun_thr, type));

      unsigned depth = get_depth(fun_thr, index, type);
	// Use the depth to extract the values from the stack
	// of the parent thread.
      തള്ളുക_from_parent(thr, fun_thr, depth, type);
      തിരികെ സത്യം;
}

/*
 * %retload/real <index>
 */
തർക്കം of_RETLOAD_REAL(vthread_t thr, vvp_code_t cp)
{
      തിരികെ retload<ഇരട്ട>(thr, cp);
}

/*
 * %retload/str <index>
 */
തർക്കം of_RETLOAD_STR(vthread_t thr, vvp_code_t cp)
{
      തിരികെ retload<string>(thr, cp);
}

/*
 * %retload/vec4 <index>
 */
തർക്കം of_RETLOAD_VEC4(vthread_t thr, vvp_code_t cp)
{
      തിരികെ retload<vvp_vector4_t>(thr, cp);
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
തർക്കം of_SCOPY(vthread_t thr, vvp_code_t)
{
      vvp_object_t tmp;
      thr->എടുക്കുക_object(tmp);

      vvp_object_t&dest = thr->peek_object();
        // If it is null there is nothing to copy
      എങ്കിൽ (!tmp.test_nil())
	    dest.shallow_copy(tmp);

      തിരികെ സത്യം;
}

സ്ഥിതം ശൂന്യം thread_peek(vthread_t thr, ഇരട്ട&value)
{
      value = thr->peek_real(0);
}

സ്ഥിതം ശൂന്യം thread_peek(vthread_t thr, string&value)
{
      value = thr->peek_str(0);
}

സ്ഥിതം ശൂന്യം thread_peek(vthread_t thr, vvp_vector4_t&value)
{
      value = thr->peek_vec4(0);
}

template <typename ELEM>
സ്ഥിതം തർക്കം set_dar_obj(vthread_t thr, vvp_code_t cp)
{
      unsigned adr = thr->വാക്കുകൾ[cp->സംഖ്യ].w_int;

      ELEM value;
      thread_peek(thr, value);

      vvp_object_t&മുകൾ = thr->peek_object();
      vvp_darray*darray = മുകൾ.peek<vvp_darray>();
      പ്രതിജ്ഞാ(darray);

      darray->set_word(adr, value);
      തിരികെ സത്യം;
}

/*
 * %set/dar/obj/real <index>
 */
തർക്കം of_SET_DAR_OBJ_REAL(vthread_t thr, vvp_code_t cp)
{
      തിരികെ set_dar_obj<ഇരട്ട>(thr, cp);
}

/*
 * %set/dar/obj/str <index>
 */
തർക്കം of_SET_DAR_OBJ_STR(vthread_t thr, vvp_code_t cp)
{
      തിരികെ set_dar_obj<string>(thr, cp);
}

/*
 * %set/dar/obj/vec4 <index>
 */
തർക്കം of_SET_DAR_OBJ_VEC4(vthread_t thr, vvp_code_t cp)
{
      തിരികെ set_dar_obj<vvp_vector4_t>(thr, cp);
}

/*
 * %shiftl <idx>
 *
 * Pop the operand, then push the result.
 */
തർക്കം of_SHIFTL(vthread_t thr, vvp_code_t cp)
{
      പൂർണ്ണസംഖ്യ use_index = cp->സംഖ്യ;
      uint64_t shift = thr->വാക്കുകൾ[use_index].w_uint;

      vvp_vector4_t&val = thr->peek_vec4();
      unsigned wid  = val.size();

      എങ്കിൽ (thr->flags[4] == BIT4_1) {
	      // The result is 'bx if the shift amount is undefined
	    val = vvp_vector4_t(wid, BIT4_X);

      } അല്ലെങ്കിൽ എങ്കിൽ (thr->flags[4] == BIT4_X || shift >= wid) {
	      // Shift is so big that all value is shifted out. Write
	      // a constant 0 result.
	    val = vvp_vector4_t(wid, BIT4_0);

      } അല്ലെങ്കിൽ എങ്കിൽ (shift > 0) {
	    vvp_vector4_t blk = val.subvalue(0, wid-shift);
	    vvp_vector4_t tmp (shift, BIT4_0);
	    val.set_vec(0, tmp);
	    val.set_vec(shift, blk);
      }

      തിരികെ സത്യം;
}

/*
 * %shiftr <idx>
 * This is an unsigned right shift. The <idx> is a number that selects
 * the index register with the amount of the shift. This instruction
 * checks flag bit 4, which will be true if the shift is invalid.
 */
തർക്കം of_SHIFTR(vthread_t thr, vvp_code_t cp)
{
      പൂർണ്ണസംഖ്യ use_index = cp->സംഖ്യ;
      uint64_t shift = thr->വാക്കുകൾ[use_index].w_uint;

      vvp_vector4_t val = thr->എടുക്കുക_vec4();
      unsigned wid  = val.size();

      എങ്കിൽ (thr->flags[4] == BIT4_1) {
	    val = vvp_vector4_t(wid, BIT4_X);

      } അല്ലെങ്കിൽ എങ്കിൽ (thr->flags[4] == BIT4_X || shift > wid) {
	    val = vvp_vector4_t(wid, BIT4_0);

      } അല്ലെങ്കിൽ എങ്കിൽ (shift > 0) {
	    vvp_vector4_t blk = val.subvalue(shift, wid-shift);
	    vvp_vector4_t tmp (shift, BIT4_0);
	    val.set_vec(0, blk);
	    val.set_vec(wid-shift, tmp);
      }

      thr->തള്ളുക_vec4(val);
      തിരികെ സത്യം;
}

/*
 *  %shiftr/s <wid>
 */
തർക്കം of_SHIFTR_S(vthread_t thr, vvp_code_t cp)
{
      പൂർണ്ണസംഖ്യ use_index = cp->സംഖ്യ;
      uint64_t shift = thr->വാക്കുകൾ[use_index].w_uint;

      vvp_vector4_t val = thr->എടുക്കുക_vec4();
      unsigned wid  = val.size();

      vvp_bit4_t sign_bit = val.value(val.size()-1);

      എങ്കിൽ (thr->flags[4] == BIT4_1) {
	    val = vvp_vector4_t(wid, BIT4_X);

      } അല്ലെങ്കിൽ എങ്കിൽ (thr->flags[4] == BIT4_X || shift > wid) {
	    val = vvp_vector4_t(wid, sign_bit);

      } അല്ലെങ്കിൽ എങ്കിൽ (shift > 0) {
	    vvp_vector4_t blk = val.subvalue(shift, wid-shift);
	    vvp_vector4_t tmp (shift, sign_bit);
	    val.set_vec(0, blk);
	    val.set_vec(wid-shift, tmp);
      }

      thr->തള്ളുക_vec4(val);
      തിരികെ സത്യം;
}

/*
 * %split/vec4 <wid>
 *   Pop 1 value,
 *   Take <wid> bits from the lsb,
 *   Push the remaining msb,
 *   Push the lsb.
 */
തർക്കം of_SPLIT_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned lsb_wid = cp->സംഖ്യ;

      vvp_vector4_t&val = thr->peek_vec4();
      പ്രതിജ്ഞാ(lsb_wid < val.size());

      vvp_vector4_t lsb = val.subvalue(0, lsb_wid);
      val = val.subvalue(lsb_wid, val.size()-lsb_wid);

      thr->തള്ളുക_vec4(lsb);
      തിരികെ സത്യം;
}

/*
 * The following are used to allow the darray templates to print correctly.
 */
വരിയിൽ സ്ഥിതം string get_darray_type(ഇരട്ട&)
{
      തിരികെ "darray<real>";
}

വരിയിൽ സ്ഥിതം string get_darray_type(string&)
{
      തിരികെ "darray<string>";
}

വരിയിൽ സ്ഥിതം string get_darray_type(സ്ഥിരം vvp_vector4_t&value)
{
      ostringstream buf;
      buf << "darray<vector[" << value.size() << "]>";
      string res = buf.str();
      തിരികെ res;
}

/*
 * The following are used to allow a common template to be written for
 * darray real/string/vec4 operations
 */
വരിയിൽ സ്ഥിതം ശൂന്യം dar_എടുക്കുക_മൂല്യം(vthread_t thr, ഇരട്ട&value)
{
      value = thr->എടുക്കുക_real();
}

വരിയിൽ സ്ഥിതം ശൂന്യം dar_എടുക്കുക_മൂല്യം(vthread_t thr, string&value)
{
      value = thr->എടുക്കുക_str();
}

വരിയിൽ സ്ഥിതം ശൂന്യം dar_എടുക്കുക_മൂല്യം(vthread_t thr, vvp_vector4_t&value)
{
      value = thr->എടുക്കുക_vec4();
}

template <typename ELEM>
സ്ഥിതം തർക്കം store_dar(vthread_t thr, vvp_code_t cp)
{
      int64_t adr = thr->വാക്കുകൾ[3].w_int;
      ELEM value;
	// FIXME: Can we get the size of the underlying array element
	//        and then use the normal pop_value?
      dar_എടുക്കുക_മൂല്യം(thr, value);

      vvp_net_t*net = cp->net;
      പ്രതിജ്ഞാ(net);

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      പ്രതിജ്ഞാ(obj);

      vvp_darray*darray = obj->get_object().peek<vvp_darray>();

      എങ്കിൽ (adr < 0)
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to a negative " << get_darray_type(value)
	         << " index (" << adr << ")." << endl;
      അല്ലെങ്കിൽ എങ്കിൽ (thr->flags[4] != BIT4_0)
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to an undefined " << get_darray_type(value)
	         << " index." << endl;
      അല്ലെങ്കിൽ എങ്കിൽ (darray)
	    darray->set_word(adr, value);
      അല്ലെങ്കിൽ
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to an undefined " << get_darray_type(value)
	         << "." << endl;

      തിരികെ സത്യം;
}

/*
 * %store/dar/real <var>
 */
തർക്കം of_STORE_DAR_R(vthread_t thr, vvp_code_t cp)
{
      തിരികെ store_dar<ഇരട്ട>(thr, cp);
}

/*
 * %store/dar/str <var>
 */
തർക്കം of_STORE_DAR_STR(vthread_t thr, vvp_code_t cp)
{
      തിരികെ store_dar<string>(thr, cp);
}

/*
 * %store/dar/vec4 <var>
 */
തർക്കം of_STORE_DAR_VEC4(vthread_t thr, vvp_code_t cp)
{
      തിരികെ store_dar<vvp_vector4_t>(thr, cp);
}

തർക്കം of_STORE_OBJ(vthread_t thr, vvp_code_t cp)
{
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);

      vvp_object_t val;
      thr->എടുക്കുക_object(val);

      vvp_send_object(ptr, val, thr->wt_context);

      തിരികെ സത്യം;
}

/*
 * %store/obja <array-label> <index>
 */
തർക്കം of_STORE_OBJA(vthread_t thr, vvp_code_t cp)
{
      unsigned idx = cp->bit_idx[0];
      unsigned adr = thr->വാക്കുകൾ[idx].w_int;

      vvp_object_t val;
      thr->എടുക്കുക_object(val);

      cp->array->set_word(adr, val);

      തിരികെ സത്യം;
}


/*
 * %store/prop/obj <pid>, <idx>
 *
 * Pop an object value from the object stack, and store the value into
 * the property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
തർക്കം of_STORE_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->സംഖ്യ;
      unsigned idx = cp->bit_idx[0];

      എങ്കിൽ (idx != 0) {
	    പ്രതിജ്ഞാ(idx < vthread_s::വാക്കുകൾ_എണ്ണം);
	    idx = thr->വാക്കുകൾ[idx].w_uint;
      }

      vvp_object_t val;
      thr->എടുക്കുക_object(val);

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      പ്രതിജ്ഞാ(cobj);

      cobj->set_object(pid, val, idx);

      തിരികെ സത്യം;
}

സ്ഥിതം ശൂന്യം എടുക്കുക_prop_val(vthread_t thr, ഇരട്ട&val, unsigned)
{
      val = thr->എടുക്കുക_real();
}

സ്ഥിതം ശൂന്യം എടുക്കുക_prop_val(vthread_t thr, string&val, unsigned)
{
      val = thr->എടുക്കുക_str();
}

സ്ഥിതം ശൂന്യം എടുക്കുക_prop_val(vthread_t thr, vvp_vector4_t&val, unsigned wid)
{
      val = thr->എടുക്കുക_vec4();
      പ്രതിജ്ഞാ(val.size() >= wid);
      val.resize(wid);
}

സ്ഥിതം ശൂന്യം set_val(vvp_cobject*cobj, size_t pid, സ്ഥിരം ഇരട്ട&val)
{
      cobj->set_real(pid, val);
}

സ്ഥിതം ശൂന്യം set_val(vvp_cobject*cobj, size_t pid, സ്ഥിരം string&val)
{
      cobj->set_string(pid, val);
}

സ്ഥിതം ശൂന്യം set_val(vvp_cobject*cobj, size_t pid, സ്ഥിരം vvp_vector4_t&val)
{
      cobj->set_vec4(pid, val);
}

template <typename ELEM>
സ്ഥിതം തർക്കം store_prop(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
      size_t pid = cp->സംഖ്യ;
      ELEM val;
      എടുക്കുക_prop_val(thr, val, wid); // Pop the value to store.

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      പ്രതിജ്ഞാ(cobj);

      set_val(cobj, pid, val);

      തിരികെ സത്യം;
}

/*
 * %store/prop/r <id>
 *
 * Pop a real value from the real stack, and store the value into the
 * property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
തർക്കം of_STORE_PROP_R(vthread_t thr, vvp_code_t cp)
{
      തിരികെ store_prop<ഇരട്ട>(thr, cp);
}

/*
 * %store/prop/str <id>
 *
 * Pop a string value from the string stack, and store the value into
 * the property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
തർക്കം of_STORE_PROP_STR(vthread_t thr, vvp_code_t cp)
{
      തിരികെ store_prop<string>(thr, cp);
}

/*
 * %store/prop/v <pid>, <wid>
 *
 * Store vector value into property <id> of cobject in the top of the
 * stack. Do NOT pop the object stack.
 */
തർക്കം of_STORE_PROP_V(vthread_t thr, vvp_code_t cp)
{
      തിരികെ store_prop<vvp_vector4_t>(thr, cp, cp->bit_idx[0]);
}

template <വർഗ്ഗം QTYPE>
സ്ഥിതം QTYPE* get_queue_prop(vvp_cobject*cobj, size_t pid)
{
      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      QTYPE* queue = qobj.peek<QTYPE>();
      എങ്കിൽ (queue == 0) {
	    queue = പുതിയ QTYPE;
	    vvp_object_t val(queue);
	    cobj->set_object(pid, val, 0);
      }
      തിരികെ queue;
}

template <typename ELEM, വർഗ്ഗം QTYPE>
സ്ഥിതം തർക്കം store_prop_qb(vthread_t thr, vvp_code_t cp, unsigned wid)
{
      size_t pid = cp->സംഖ്യ;
      ELEM value;
      unsigned max_size = thr->വാക്കുകൾ[cp->bit_idx[0]].w_uint;
      എടുക്കുക_മൂല്യം(thr, value, wid);

      vvp_object_t& മുകൾ = thr->peek_object();
      vvp_cobject*cobj = മുകൾ.peek<vvp_cobject>();
      പ്രതിജ്ഞാ(cobj);

      QTYPE* queue = get_queue_prop<QTYPE>(cobj, pid);
      പ്രതിജ്ഞാ(queue);
      queue->push_back(value, max_size);
      തിരികെ സത്യം;
}

template <typename ELEM, വർഗ്ഗം QTYPE>
സ്ഥിതം തർക്കം store_prop_qf(vthread_t thr, vvp_code_t cp, unsigned wid)
{
      size_t pid = cp->സംഖ്യ;
      ELEM value;
      unsigned max_size = thr->വാക്കുകൾ[cp->bit_idx[0]].w_uint;
      എടുക്കുക_മൂല്യം(thr, value, wid);

      vvp_object_t& മുകൾ = thr->peek_object();
      vvp_cobject*cobj = മുകൾ.peek<vvp_cobject>();
      പ്രതിജ്ഞാ(cobj);

      QTYPE* queue = get_queue_prop<QTYPE>(cobj, pid);
      പ്രതിജ്ഞാ(queue);
      queue->push_front(value, max_size);
      തിരികെ സത്യം;
}

തർക്കം of_STORE_PROP_QB_R(vthread_t thr, vvp_code_t cp)
{
      തിരികെ store_prop_qb<ഇരട്ട, vvp_queue_real>(thr, cp, 0);
}

തർക്കം of_STORE_PROP_QB_STR(vthread_t thr, vvp_code_t cp)
{
      തിരികെ store_prop_qb<string, vvp_queue_string>(thr, cp, 0);
}

തർക്കം of_STORE_PROP_QB_V(vthread_t thr, vvp_code_t cp)
{
      തിരികെ store_prop_qb<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

തർക്കം of_STORE_PROP_QF_R(vthread_t thr, vvp_code_t cp)
{
      തിരികെ store_prop_qf<ഇരട്ട, vvp_queue_real>(thr, cp, 0);
}

തർക്കം of_STORE_PROP_QF_STR(vthread_t thr, vvp_code_t cp)
{
      തിരികെ store_prop_qf<string, vvp_queue_string>(thr, cp, 0);
}

തർക്കം of_STORE_PROP_QF_V(vthread_t thr, vvp_code_t cp)
{
      തിരികെ store_prop_qf<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

template <typename ELEM, വർഗ്ഗം QTYPE>
സ്ഥിതം തർക്കം qinsert_prop(vthread_t thr, vvp_code_t cp, unsigned wid)
{
      int64_t idx = thr->വാക്കുകൾ[3].w_int;
      ELEM value;
      size_t pid = cp->സംഖ്യ;
      എടുക്കുക_മൂല്യം(thr, value, wid);

      vvp_object_t& മുകൾ = thr->peek_object();
      vvp_cobject*cobj = മുകൾ.peek<vvp_cobject>();
      പ്രതിജ്ഞാ(cobj);

      QTYPE* queue = get_queue_prop<QTYPE>(cobj, pid);
      പ്രതിജ്ഞാ(queue);
      എങ്കിൽ (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at a negative "
	         << get_queue_type(value)
	         << " index (" << idx << "). ";
	    print_queue_മൂല്യം(value);
	    cerr << " was not added." << endl;
      } അല്ലെങ്കിൽ എങ്കിൽ (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at an undefined "
	         << get_queue_type(value) << " index. ";
	    print_queue_മൂല്യം(value);
	    cerr << " was not added." << endl;
      } അല്ലെങ്കിൽ {
	    unsigned max_size = thr->വാക്കുകൾ[cp->bit_idx[0]].w_int;
	    queue->insert(idx, value, max_size);
      }
      തിരികെ സത്യം;
}

തർക്കം of_QINSERT_PROP_R(vthread_t thr, vvp_code_t cp)
{
      തിരികെ qinsert_prop<ഇരട്ട, vvp_queue_real>(thr, cp, 0);
}

തർക്കം of_QINSERT_PROP_STR(vthread_t thr, vvp_code_t cp)
{
      തിരികെ qinsert_prop<string, vvp_queue_string>(thr, cp, 0);
}

തർക്കം of_QINSERT_PROP_V(vthread_t thr, vvp_code_t cp)
{
      തിരികെ qinsert_prop<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

template <typename ELEM, വർഗ്ഗം QTYPE>
സ്ഥിതം തർക്കം q_എടുക്കുക_prop(vthread_t thr, vvp_code_t cp,
                       ശൂന്യം (*get_val_func)(vvp_queue*, ELEM&),
                       സ്ഥിരം അക്ഷരം*loc, unsigned wid)
{
      size_t pid = cp->സംഖ്യ;

      vvp_object_t& മുകൾ = thr->peek_object();
      vvp_cobject*cobj = മുകൾ.peek<vvp_cobject>();
      പ്രതിജ്ഞാ(cobj);

      QTYPE* queue = get_queue_prop<QTYPE>(cobj, pid);
      പ്രതിജ്ഞാ(queue);

      size_t size = queue->get_size();

      ELEM value;
      എങ്കിൽ (size) {
	    get_val_func(queue, value);
      } അല്ലെങ്കിൽ {
	    dq_default(value, wid);
	    cerr << thr->get_fileline()
	         << "Warning: pop_" << loc << "() on empty "
	         << get_queue_type(value) << "." << endl;
      }

      തള്ളുക_മൂല്യം(thr, value, wid);
      തിരികെ സത്യം;
}

template <typename ELEM, വർഗ്ഗം QTYPE>
സ്ഥിതം തർക്കം qpop_b_prop(vthread_t thr, vvp_code_t cp, unsigned wid)
{
      തിരികെ q_എടുക്കുക_prop<ELEM, QTYPE>(thr, cp, get_back_മൂല്യം<ELEM>, "back", wid);
}

template <typename ELEM, വർഗ്ഗം QTYPE>
സ്ഥിതം തർക്കം qpop_f_prop(vthread_t thr, vvp_code_t cp, unsigned wid)
{
      തിരികെ q_എടുക്കുക_prop<ELEM, QTYPE>(thr, cp, get_front_മൂല്യം<ELEM>, "front", wid);
}

തർക്കം of_QPOP_PROP_B_REAL(vthread_t thr, vvp_code_t cp)
{
      തിരികെ qpop_b_prop<ഇരട്ട, vvp_queue_real>(thr, cp, 0);
}

തർക്കം of_QPOP_PROP_B_STR(vthread_t thr, vvp_code_t cp)
{
      തിരികെ qpop_b_prop<string, vvp_queue_string>(thr, cp, 0);
}

തർക്കം of_QPOP_PROP_B_V(vthread_t thr, vvp_code_t cp)
{
      തിരികെ qpop_b_prop<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[0]);
}

തർക്കം of_QPOP_PROP_F_REAL(vthread_t thr, vvp_code_t cp)
{
      തിരികെ qpop_f_prop<ഇരട്ട, vvp_queue_real>(thr, cp, 0);
}

തർക്കം of_QPOP_PROP_F_STR(vthread_t thr, vvp_code_t cp)
{
      തിരികെ qpop_f_prop<string, vvp_queue_string>(thr, cp, 0);
}

തർക്കം of_QPOP_PROP_F_V(vthread_t thr, vvp_code_t cp)
{
      തിരികെ qpop_f_prop<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[0]);
}

സ്ഥിതം vvp_vector4_t queue_unique_ulong_to_vec4(unsigned long ul, unsigned bits)
{
      vvp_vector4_t val(bits, BIT4_0);
      വേണ്ടി (unsigned idx = 0; idx < bits && idx < 8 * വലുപ്പം(unsigned long); idx += 1) {
	    എങ്കിൽ (ul & 1UL) {
		  val.set_bit(idx, BIT4_1);
	    } അല്ലെങ്കിൽ {
		  val.set_bit(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      തിരികെ val;
}

template <വർഗ്ഗം SRC> സ്ഥിതം vvp_queue_vec4* queue_run_unique_src(SRC* src,
								 unsigned wid,
								 തർക്കം as_index)
{
      vvp_queue_vec4* dst = പുതിയ vvp_queue_vec4();
      എങ്കിൽ (!src) തിരികെ dst;
      size_t n = src->get_size();
      വേണ്ടി (size_t i = 0; i < n; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    തർക്കം seen = അസത്യം;
	    വേണ്ടി (size_t j = 0; j < i; j += 1) {
		  vvp_vector4_t vj(wid);
		  src->get_word(j, vj);
		  എങ്കിൽ (vi.eeq(vj)) {
			seen = സത്യം;
			നിർത്തുക;
		  }
	    }
	    എങ്കിൽ (seen) തുടരുക;
	    എങ്കിൽ (as_index) {
		  dst->push_back(queue_unique_ulong_to_vec4((unsigned long)i, 32), 0);
	    } അല്ലെങ്കിൽ {
		  dst->push_back(vi, 0);
	    }
      }
      തിരികെ dst;
}

template <വർഗ്ഗം SRC>
സ്ഥിതം vvp_queue_vec4* queue_run_find_matches_src(SRC* src, unsigned wid,
						  സ്ഥിരം vvp_vector4_t& cmp,
						  തർക്കം as_index)
{
      vvp_queue_vec4* dst = പുതിയ vvp_queue_vec4();
      എങ്കിൽ (!src) തിരികെ dst;
      size_t n = src->get_size();
      വേണ്ടി (size_t i = 0; i < n; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    എങ്കിൽ (vi.eeq(cmp)) {
		  എങ്കിൽ (as_index) {
			dst->push_back(queue_unique_ulong_to_vec4((unsigned long)i, 32), 0);
		  } അല്ലെങ്കിൽ {
			dst->push_back(vi, 0);
		  }
	    }
      }
      തിരികെ dst;
}

/* Array/queue locator methods return a queue with 0 or 1 elements (LRM). */
template <വർഗ്ഗം SRC>
സ്ഥിതം vvp_queue_vec4* queue_find_first_last_to_queue_src(SRC* src,
							  unsigned wid,
							  സ്ഥിരം vvp_vector4_t& cmp,
							  തർക്കം want_first,
							  തർക്കം want_index)
{
      vvp_queue_vec4* dst = പുതിയ vvp_queue_vec4();
      എങ്കിൽ (!src || src->get_size() == 0) തിരികെ dst;
      size_t n = src->get_size();
      എങ്കിൽ (want_first) {
	    വേണ്ടി (size_t i = 0; i < n; i += 1) {
		  vvp_vector4_t vi(wid);
		  src->get_word(i, vi);
		  എങ്കിൽ (vi.eeq(cmp)) {
			എങ്കിൽ (want_index) {
			      dst->push_back(
				  queue_unique_ulong_to_vec4((unsigned long)i, 32),
				  0);
			} അല്ലെങ്കിൽ {
			      dst->push_back(vi, 0);
			}
			തിരികെ dst;
		  }
	    }
      } അല്ലെങ്കിൽ {
	    വേണ്ടി (size_t i = n; i > 0; ) {
		  i -= 1;
		  vvp_vector4_t vi(wid);
		  src->get_word(i, vi);
		  എങ്കിൽ (vi.eeq(cmp)) {
			എങ്കിൽ (want_index) {
			      dst->push_back(
				  queue_unique_ulong_to_vec4((unsigned long)i, 32),
				  0);
			} അല്ലെങ്കിൽ {
			      dst->push_back(vi, 0);
			}
			തിരികെ dst;
		  }
	    }
      }
      തിരികെ dst;
}

/* min()/max() return one element with the selected extremum. */
template <വർഗ്ഗം SRC>
സ്ഥിതം vvp_queue_vec4* queue_run_min_max_src(SRC* src, unsigned wid, തർക്കം want_max)
{
      vvp_queue_vec4* dst = പുതിയ vvp_queue_vec4();
      എങ്കിൽ (!src || src->get_size() == 0) തിരികെ dst;

      vvp_vector4_t best(wid);
      src->get_word(0, best);

      size_t n = src->get_size();
      വേണ്ടി (size_t i = 1 ; i < n ; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);

	    vvp_bit4_t ge = compare_gtge(vi, best, BIT4_1);
	    തർക്കം replace = want_max ? (ge == BIT4_1) : (ge == BIT4_0);
	    എങ്കിൽ (replace) best = vi;
      }

      dst->push_back(best, 0);
      തിരികെ dst;
}

തർക്കം of_QUEUE_FIND_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->എടുക്കുക_vec4();
      unsigned wid = cp->bit_idx[0];
      പ്രതിജ്ഞാ(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      എങ്കിൽ (qsrc) dst = queue_run_find_matches_src(qsrc, wid, cmp, അസത്യം);
      അല്ലെങ്കിൽ എങ്കിൽ (dsrc) dst = queue_run_find_matches_src(dsrc, wid, cmp, അസത്യം);
      അല്ലെങ്കിൽ {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_run_find_matches_src(src, wid, cmp, അസത്യം);
      }
      thr->തള്ളുക_object(vvp_object_t(dst));
      തിരികെ സത്യം;
}

തർക്കം of_QUEUE_FIND_PROP_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->എടുക്കുക_vec4();
      size_t pid = cp->സംഖ്യ;
      unsigned wid = cp->bit_idx[0];
      പ്രതിജ്ഞാ(cmp.size() == wid);

      vvp_object_t& മുകൾ = thr->peek_object();
      vvp_cobject*cobj = മുകൾ.peek<vvp_cobject>();
      പ്രതിജ്ഞാ(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_find_matches_src(qsrc, wid, cmp, അസത്യം)
				 : queue_run_find_matches_src(dsrc, wid, cmp, അസത്യം);
      thr->തള്ളുക_object(vvp_object_t(dst));
      തിരികെ സത്യം;
}

തർക്കം of_QUEUE_FIND_INDEX_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->എടുക്കുക_vec4();
      unsigned wid = cp->bit_idx[0];
      പ്രതിജ്ഞാ(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      എങ്കിൽ (qsrc) dst = queue_run_find_matches_src(qsrc, wid, cmp, സത്യം);
      അല്ലെങ്കിൽ എങ്കിൽ (dsrc) dst = queue_run_find_matches_src(dsrc, wid, cmp, സത്യം);
      അല്ലെങ്കിൽ {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_run_find_matches_src(src, wid, cmp, സത്യം);
      }
      thr->തള്ളുക_object(vvp_object_t(dst));
      തിരികെ സത്യം;
}

തർക്കം of_QUEUE_FIND_INDEX_PROP_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->എടുക്കുക_vec4();
      size_t pid = cp->സംഖ്യ;
      unsigned wid = cp->bit_idx[0];
      പ്രതിജ്ഞാ(cmp.size() == wid);

      vvp_object_t& മുകൾ = thr->peek_object();
      vvp_cobject*cobj = മുകൾ.peek<vvp_cobject>();
      പ്രതിജ്ഞാ(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_find_matches_src(qsrc, wid, cmp, സത്യം)
				 : queue_run_find_matches_src(dsrc, wid, cmp, സത്യം);
      thr->തള്ളുക_object(vvp_object_t(dst));
      തിരികെ സത്യം;
}

തർക്കം of_QUEUE_FIND_FIRST_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->എടുക്കുക_vec4();
      unsigned wid = cp->bit_idx[0];
      പ്രതിജ്ഞാ(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      എങ്കിൽ (qsrc) dst = queue_find_first_last_to_queue_src(qsrc, wid, cmp, സത്യം, അസത്യം);
      അല്ലെങ്കിൽ എങ്കിൽ (dsrc) dst = queue_find_first_last_to_queue_src(dsrc, wid, cmp, സത്യം, അസത്യം);
      അല്ലെങ്കിൽ {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_find_first_last_to_queue_src(src, wid, cmp, സത്യം, അസത്യം);
      }
      thr->തള്ളുക_object(vvp_object_t(dst));
      തിരികെ സത്യം;
}

തർക്കം of_QUEUE_FIND_FIRST_PROP_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->എടുക്കുക_vec4();
      size_t pid = cp->സംഖ്യ;
      unsigned wid = cp->bit_idx[0];
      പ്രതിജ്ഞാ(cmp.size() == wid);

      vvp_object_t& മുകൾ = thr->peek_object();
      vvp_cobject*cobj = മുകൾ.peek<vvp_cobject>();
      പ്രതിജ്ഞാ(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst =
	    qsrc ? queue_find_first_last_to_queue_src(qsrc, wid, cmp, സത്യം, അസത്യം)
		 : queue_find_first_last_to_queue_src(dsrc, wid, cmp, സത്യം, അസത്യം);
      thr->തള്ളുക_object(vvp_object_t(dst));
      തിരികെ സത്യം;
}

തർക്കം of_QUEUE_FIND_FIRST_INDEX_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->എടുക്കുക_vec4();
      unsigned wid = cp->bit_idx[0];
      പ്രതിജ്ഞാ(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      എങ്കിൽ (qsrc) dst = queue_find_first_last_to_queue_src(qsrc, wid, cmp, സത്യം, സത്യം);
      അല്ലെങ്കിൽ എങ്കിൽ (dsrc) dst = queue_find_first_last_to_queue_src(dsrc, wid, cmp, സത്യം, സത്യം);
      അല്ലെങ്കിൽ {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_find_first_last_to_queue_src(src, wid, cmp, സത്യം, സത്യം);
      }
      thr->തള്ളുക_object(vvp_object_t(dst));
      തിരികെ സത്യം;
}

തർക്കം of_QUEUE_FIND_FIRST_INDEX_PROP_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->എടുക്കുക_vec4();
      size_t pid = cp->സംഖ്യ;
      unsigned wid = cp->bit_idx[0];
      പ്രതിജ്ഞാ(cmp.size() == wid);

      vvp_object_t& മുകൾ = thr->peek_object();
      vvp_cobject*cobj = മുകൾ.peek<vvp_cobject>();
      പ്രതിജ്ഞാ(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst =
	    qsrc ? queue_find_first_last_to_queue_src(qsrc, wid, cmp, സത്യം, സത്യം)
		 : queue_find_first_last_to_queue_src(dsrc, wid, cmp, സത്യം, സത്യം);
      thr->തള്ളുക_object(vvp_object_t(dst));
      തിരികെ സത്യം;
}

തർക്കം of_QUEUE_FIND_LAST_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->എടുക്കുക_vec4();
      unsigned wid = cp->bit_idx[0];
      പ്രതിജ്ഞാ(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      എങ്കിൽ (qsrc) dst = queue_find_first_last_to_queue_src(qsrc, wid, cmp, അസത്യം, അസത്യം);
      അല്ലെങ്കിൽ എങ്കിൽ (dsrc) dst = queue_find_first_last_to_queue_src(dsrc, wid, cmp, അസത്യം, അസത്യം);
      അല്ലെങ്കിൽ {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_find_first_last_to_queue_src(src, wid, cmp, അസത്യം, അസത്യം);
      }
      thr->തള്ളുക_object(vvp_object_t(dst));
      തിരികെ സത്യം;
}

തർക്കം of_QUEUE_FIND_LAST_PROP_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->എടുക്കുക_vec4();
      size_t pid = cp->സംഖ്യ;
      unsigned wid = cp->bit_idx[0];
      പ്രതിജ്ഞാ(cmp.size() == wid);

      vvp_object_t& മുകൾ = thr->peek_object();
      vvp_cobject*cobj = മുകൾ.peek<vvp_cobject>();
      പ്രതിജ്ഞാ(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst =
	    qsrc ? queue_find_first_last_to_queue_src(qsrc, wid, cmp, അസത്യം, അസത്യം)
		 : queue_find_first_last_to_queue_src(dsrc, wid, cmp, അസത്യം, അസത്യം);
      thr->തള്ളുക_object(vvp_object_t(dst));
      തിരികെ സത്യം;
}

തർക്കം of_QUEUE_FIND_LAST_INDEX_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->എടുക്കുക_vec4();
      unsigned wid = cp->bit_idx[0];
      പ്രതിജ്ഞാ(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      എങ്കിൽ (qsrc) dst = queue_find_first_last_to_queue_src(qsrc, wid, cmp, അസത്യം, സത്യം);
      അല്ലെങ്കിൽ എങ്കിൽ (dsrc) dst = queue_find_first_last_to_queue_src(dsrc, wid, cmp, അസത്യം, സത്യം);
      അല്ലെങ്കിൽ {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_find_first_last_to_queue_src(src, wid, cmp, അസത്യം, സത്യം);
      }
      thr->തള്ളുക_object(vvp_object_t(dst));
      തിരികെ സത്യം;
}

തർക്കം of_QUEUE_FIND_LAST_INDEX_PROP_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->എടുക്കുക_vec4();
      size_t pid = cp->സംഖ്യ;
      unsigned wid = cp->bit_idx[0];
      പ്രതിജ്ഞാ(cmp.size() == wid);

      vvp_object_t& മുകൾ = thr->peek_object();
      vvp_cobject*cobj = മുകൾ.peek<vvp_cobject>();
      പ്രതിജ്ഞാ(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst =
	    qsrc ? queue_find_first_last_to_queue_src(qsrc, wid, cmp, അസത്യം, സത്യം)
		 : queue_find_first_last_to_queue_src(dsrc, wid, cmp, അസത്യം, സത്യം);
      thr->തള്ളുക_object(vvp_object_t(dst));
      തിരികെ സത്യം;
}

തർക്കം of_QUEUE_MIN_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      എങ്കിൽ (qsrc) dst = queue_run_min_max_src(qsrc, wid, അസത്യം);
      അല്ലെങ്കിൽ എങ്കിൽ (dsrc) dst = queue_run_min_max_src(dsrc, wid, അസത്യം);
      അല്ലെങ്കിൽ {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_run_min_max_src(src, wid, അസത്യം);
      }
      thr->തള്ളുക_object(vvp_object_t(dst));
      തിരികെ സത്യം;
}

തർക്കം of_QUEUE_MIN_OBJ_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->എടുക്കുക_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_min_max_src(qsrc, wid, അസത്യം)
				 : queue_run_min_max_src(dsrc, wid, അസത്യം);
      thr->തള്ളുക_object(vvp_object_t(dst));
      തിരികെ സത്യം;
}

തർക്കം of_QUEUE_MIN_PROP_V(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->സംഖ്യ;
      unsigned wid = cp->bit_idx[0];

      vvp_object_t& മുകൾ = thr->peek_object();
      vvp_cobject*cobj = മുകൾ.peek<vvp_cobject>();
      പ്രതിജ്ഞാ(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_min_max_src(qsrc, wid, അസത്യം)
				 : queue_run_min_max_src(dsrc, wid, അസത്യം);
      thr->തള്ളുക_object(vvp_object_t(dst));
      തിരികെ സത്യം;
}

തർക്കം of_QUEUE_MAX_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      എങ്കിൽ (qsrc) dst = queue_run_min_max_src(qsrc, wid, സത്യം);
      അല്ലെങ്കിൽ എങ്കിൽ (dsrc) dst = queue_run_min_max_src(dsrc, wid, സത്യം);
      അല്ലെങ്കിൽ {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_run_min_max_src(src, wid, സത്യം);
      }
      thr->തള്ളുക_object(vvp_object_t(dst));
      തിരികെ സത്യം;
}

തർക്കം of_QUEUE_MAX_OBJ_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->എടുക്കുക_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_min_max_src(qsrc, wid, സത്യം)
				 : queue_run_min_max_src(dsrc, wid, സത്യം);
      thr->തള്ളുക_object(vvp_object_t(dst));
      തിരികെ സത്യം;
}

തർക്കം of_QUEUE_MAX_PROP_V(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->സംഖ്യ;
      unsigned wid = cp->bit_idx[0];

      vvp_object_t& മുകൾ = thr->peek_object();
      vvp_cobject*cobj = മുകൾ.peek<vvp_cobject>();
      പ്രതിജ്ഞാ(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_min_max_src(qsrc, wid, സത്യം)
				 : queue_run_min_max_src(dsrc, wid, സത്യം);
      thr->തള്ളുക_object(vvp_object_t(dst));
      തിരികെ സത്യം;
}

തർക്കം of_QUEUE_UNIQUE_OBJ_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->എടുക്കുക_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_unique_src(qsrc, wid, അസത്യം)
				 : queue_run_unique_src(dsrc, wid, അസത്യം);
      thr->തള്ളുക_object(vvp_object_t(dst));
      തിരികെ സത്യം;
}

തർക്കം of_QUEUE_UNIQUE_INDEX_OBJ_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->എടുക്കുക_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_unique_src(qsrc, wid, സത്യം)
				 : queue_run_unique_src(dsrc, wid, സത്യം);
      thr->തള്ളുക_object(vvp_object_t(dst));
      തിരികെ സത്യം;
}

തർക്കം of_QUEUE_UNIQUE_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      എങ്കിൽ (qsrc) dst = queue_run_unique_src(qsrc, wid, അസത്യം);
      അല്ലെങ്കിൽ എങ്കിൽ (dsrc) dst = queue_run_unique_src(dsrc, wid, അസത്യം);
      അല്ലെങ്കിൽ {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_run_unique_src(src, wid, അസത്യം);
      }
      thr->തള്ളുക_object(vvp_object_t(dst));
      തിരികെ സത്യം;
}

തർക്കം of_QUEUE_UNIQUE_PROP_V(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->സംഖ്യ;
      unsigned wid = cp->bit_idx[0];

      vvp_object_t& മുകൾ = thr->peek_object();
      vvp_cobject*cobj = മുകൾ.peek<vvp_cobject>();
      പ്രതിജ്ഞാ(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_unique_src(qsrc, wid, അസത്യം)
				 : queue_run_unique_src(dsrc, wid, അസത്യം);
      thr->തള്ളുക_object(vvp_object_t(dst));
      തിരികെ സത്യം;
}

തർക്കം of_QUEUE_UNIQUE_INDEX_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      എങ്കിൽ (qsrc) dst = queue_run_unique_src(qsrc, wid, സത്യം);
      അല്ലെങ്കിൽ എങ്കിൽ (dsrc) dst = queue_run_unique_src(dsrc, wid, സത്യം);
      അല്ലെങ്കിൽ {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_run_unique_src(src, wid, സത്യം);
      }
      thr->തള്ളുക_object(vvp_object_t(dst));
      തിരികെ സത്യം;
}

തർക്കം of_QUEUE_UNIQUE_INDEX_PROP_V(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->സംഖ്യ;
      unsigned wid = cp->bit_idx[0];

      vvp_object_t& മുകൾ = thr->peek_object();
      vvp_cobject*cobj = മുകൾ.peek<vvp_cobject>();
      പ്രതിജ്ഞാ(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_unique_src(qsrc, wid, സത്യം)
				 : queue_run_unique_src(dsrc, wid, സത്യം);
      thr->തള്ളുക_object(vvp_object_t(dst));
      തിരികെ സത്യം;
}

തർക്കം of_PROP_QUEUE_SIZE(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->സംഖ്യ;

      vvp_object_t& മുകൾ = thr->peek_object();
      vvp_cobject*cobj = മുകൾ.peek<vvp_cobject>();
      പ്രതിജ്ഞാ(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);

      size_t sz = qsrc ? qsrc->get_size() : (dsrc ? dsrc->get_size() : 0);

      vvp_vector4_t val(32, BIT4_0);
      unsigned long ul = sz;
      വേണ്ടി (unsigned idx = 0; idx < 32; idx++) {
	    എങ്കിൽ (ul & 1UL) {
		  val.set_bit(idx, BIT4_1);
	    } അല്ലെങ്കിൽ {
		  val.set_bit(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      thr->തള്ളുക_vec4(val);
      തിരികെ സത്യം;
}

തർക്കം of_QUEUE_SIZE_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      size_t sz =
	    qsrc ? qsrc->get_size() : (dsrc ? dsrc->get_size() : 0);

      vvp_vector4_t val(32, BIT4_0);
      unsigned long ul = sz;
      വേണ്ടി (unsigned idx = 0; idx < 32; idx++) {
	    എങ്കിൽ (ul & 1UL) {
		  val.set_bit(idx, BIT4_1);
	    } അല്ലെങ്കിൽ {
		  val.set_bit(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      thr->തള്ളുക_vec4(val);
      തിരികെ സത്യം;
}

തർക്കം of_QUEUE_WORD_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      unsigned ix_reg = cp->bit_idx[1];
      int64_t adr = thr->വാക്കുകൾ[ix_reg].w_int;
      എങ്കിൽ (thr->flags[4] == BIT4_1) {
	    thr->തള്ളുക_vec4(vvp_vector4_t(wid));
	    തിരികെ സത്യം;
      }
      എങ്കിൽ (adr < 0) {
	    thr->തള്ളുക_vec4(vvp_vector4_t(wid));
	    തിരികെ സത്യം;
      }
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t out(wid);
      എങ്കിൽ (qsrc) {
	    എങ്കിൽ ((size_t)adr >= qsrc->get_size()) {
		  thr->തള്ളുക_vec4(out);
		  തിരികെ സത്യം;
	    }
	    qsrc->get_word((unsigned)adr, out);
      } അല്ലെങ്കിൽ എങ്കിൽ (dsrc) {
	    എങ്കിൽ ((size_t)adr >= dsrc->get_size()) {
		  thr->തള്ളുക_vec4(out);
		  തിരികെ സത്യം;
	    }
	    dsrc->get_word((unsigned)adr, out);
      } അല്ലെങ്കിൽ {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    എങ്കിൽ (!src || (size_t)adr >= src->get_size()) {
		  thr->തള്ളുക_vec4(out);
		  തിരികെ സത്യം;
	    }
	    src->get_word((unsigned)adr, out);
      }
      thr->തള്ളുക_vec4(out);
      തിരികെ സത്യം;
}

തർക്കം of_QUEUE_WORD_PROP_V(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->സംഖ്യ;
      unsigned wid = cp->bit_idx[0];
      unsigned ix_reg = cp->bit_idx[1];
      int64_t adr = thr->വാക്കുകൾ[ix_reg].w_int;
      എങ്കിൽ (thr->flags[4] == BIT4_1) {
	    thr->തള്ളുക_vec4(vvp_vector4_t(wid));
	    തിരികെ സത്യം;
      }
      എങ്കിൽ (adr < 0) {
	    thr->തള്ളുക_vec4(vvp_vector4_t(wid));
	    തിരികെ സത്യം;
      }

      vvp_object_t saved_മുകൾ;
      തർക്കം popped_മുകൾ = അസത്യം;
      vvp_object_t& മുകൾ = thr->peek_object();
      vvp_cobject*cobj = മുകൾ.peek<vvp_cobject>();
      എങ്കിൽ (cobj == 0) {
	    // Array-locator with(property) keeps an accumulator queue on top.
	    // Temporarily pop it to access the class object beneath.
	    thr->എടുക്കുക_object(saved_മുകൾ);
	    popped_മുകൾ = സത്യം;
	    vvp_object_t& below = thr->peek_object();
	    cobj = below.peek<vvp_cobject>();
      }
      എങ്കിൽ (cobj == 0) {
	    എങ്കിൽ (popped_മുകൾ) thr->തള്ളുക_object(saved_മുകൾ);
	    thr->തള്ളുക_vec4(vvp_vector4_t(wid));
	    തിരികെ സത്യം;
      }

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t out(wid);
      എങ്കിൽ (qsrc) {
	    എങ്കിൽ ((size_t)adr >= qsrc->get_size()) {
		  thr->തള്ളുക_vec4(out);
		  തിരികെ സത്യം;
	    }
	    qsrc->get_word((unsigned)adr, out);
      } അല്ലെങ്കിൽ എങ്കിൽ (dsrc) {
	    എങ്കിൽ ((size_t)adr >= dsrc->get_size()) {
		  thr->തള്ളുക_vec4(out);
		  തിരികെ സത്യം;
	    }
	    dsrc->get_word((unsigned)adr, out);
      } അല്ലെങ്കിൽ {
	    thr->തള്ളുക_vec4(out);
	    എങ്കിൽ (popped_മുകൾ) thr->തള്ളുക_object(saved_മുകൾ);
	    തിരികെ സത്യം;
      }
      എങ്കിൽ (popped_മുകൾ) thr->തള്ളുക_object(saved_മുകൾ);
      thr->തള്ളുക_vec4(out);
      തിരികെ സത്യം;
}

തർക്കം of_CMPIX_LTU(vthread_t thr, vvp_code_t cp)
{
      unsigned a = cp->സംഖ്യ;
      unsigned b = cp->bit_idx[0];
      എങ്കിൽ (thr->വാക്കുകൾ[a].w_uint < thr->വാക്കുകൾ[b].w_uint) {
	    thr->flags[4] = BIT4_1;
      } അല്ലെങ്കിൽ {
	    thr->flags[4] = BIT4_0;
      }
      തിരികെ സത്യം;
}

തർക്കം of_CMPIX_SLT0(vthread_t thr, vvp_code_t cp)
{
      unsigned r = cp->സംഖ്യ;
      thr->flags[4] = (thr->വാക്കുകൾ[r].w_int < 0) ? BIT4_1 : BIT4_0;
      തിരികെ സത്യം;
}

തർക്കം of_തള്ളുക_IX_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned ix = cp->സംഖ്യ;
      unsigned wid = cp->bit_idx[0];
      unsigned sgn = cp->bit_idx[1];
      uint32_t ubits;
      എങ്കിൽ (sgn) {
	    int32_t sv = (int32_t)thr->വാക്കുകൾ[ix].w_int;
	    ubits = (uint32_t)sv;
      } അല്ലെങ്കിൽ {
	    ubits = (uint32_t)thr->വാക്കുകൾ[ix].w_uint;
      }
      എങ്കിൽ (wid > 32) wid = 32;
      vvp_vector4_t tmp(wid, BIT4_0);
      വേണ്ടി (unsigned idx = 0; idx < wid; idx++) {
	    എങ്കിൽ (ubits & 1u) {
		  tmp.set_bit(idx, BIT4_1);
	    } അല്ലെങ്കിൽ {
		  tmp.set_bit(idx, BIT4_0);
	    }
	    ubits >>= 1;
      }
      thr->തള്ളുക_vec4(tmp);
      തിരികെ സത്യം;
}

തർക്കം of_QUEUE_NEW_EMPTY_V(vthread_t thr, vvp_code_t)
{
      thr->തള്ളുക_object(vvp_object_t(പുതിയ vvp_queue_vec4()));
      തിരികെ സത്യം;
}

തർക്കം of_QUEUE_APPEND_WORD_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      vvp_vector4_t elem = thr->എടുക്കുക_vec4();
      പ്രതിജ്ഞാ(elem.size() == wid);
      vvp_object_t qobj;
      thr->എടുക്കുക_object(qobj);
      vvp_queue_vec4* q = qobj.peek<vvp_queue_vec4>();
      പ്രതിജ്ഞാ(q);
      q->push_back(elem, 0);
      thr->തള്ളുക_object(qobj);
      തിരികെ സത്യം;
}

തർക്കം of_DELETE_PROP_ELEM(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->സംഖ്യ;

      int64_t idx_val = thr->വാക്കുകൾ[3].w_int;
      എങ്കിൽ (thr->flags[4] == BIT4_1) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with undefined index."
	         << endl;
	    തിരികെ സത്യം;
      }
      എങ്കിൽ (idx_val < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with negative index."
	         << endl;
	    തിരികെ സത്യം;
      }
      size_t idx = idx_val;

      vvp_object_t& മുകൾ = thr->peek_object();
      vvp_cobject*cobj = മുകൾ.peek<vvp_cobject>();
      പ്രതിജ്ഞാ(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue* queue = qobj.peek<vvp_queue>();
      എങ്കിൽ (queue == 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping delete(" << idx
	         << ") on empty queue." << endl;
      } അല്ലെങ്കിൽ {
	    size_t size = queue->get_size();
	    എങ്കിൽ (idx >= size) {
		  cerr << thr->get_fileline()
		       << "Warning: skipping out of range delete(" << idx
		       << ") on queue of size " << size << "." << endl;
	    } അല്ലെങ്കിൽ {
		  queue->erase(idx);
	    }
      }

      തിരികെ സത്യം;
}

തർക്കം of_DELETE_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->സംഖ്യ;

      vvp_object_t& മുകൾ = thr->peek_object();
      vvp_cobject*cobj = മുകൾ.peek<vvp_cobject>();
      പ്രതിജ്ഞാ(cobj);

      cobj->set_object(pid, vvp_object_t(), 0);

      തിരികെ സത്യം;
}

template <typename ELEM, വർഗ്ഗം QTYPE>
സ്ഥിതം തർക്കം store_qb(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
      ELEM value;
      vvp_net_t*net = cp->net;
      unsigned max_size = thr->വാക്കുകൾ[cp->bit_idx[0]].w_int;
      എടുക്കുക_മൂല്യം(thr, value, wid); // Pop the value to store.

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      പ്രതിജ്ഞാ(queue);
      queue->push_back(value, max_size);
      തിരികെ സത്യം;
}

/*
 * %store/qb/r <var-label>, <max-idx>
 */
തർക്കം of_STORE_QB_R(vthread_t thr, vvp_code_t cp)
{
      തിരികെ store_qb<ഇരട്ട, vvp_queue_real>(thr, cp);
}

/*
 * %store/qb/str <var-label>, <max-idx>
 */
തർക്കം of_STORE_QB_STR(vthread_t thr, vvp_code_t cp)
{
      തിരികെ store_qb<string, vvp_queue_string>(thr, cp);
}

/*
 * %store/qb/v <var-label>, <max-idx>, <wid>
 */
തർക്കം of_STORE_QB_V(vthread_t thr, vvp_code_t cp)
{
      തിരികെ store_qb<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

template <typename ELEM, വർഗ്ഗം QTYPE>
സ്ഥിതം തർക്കം store_qdar(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
      int64_t idx = thr->വാക്കുകൾ[3].w_int;
      ELEM value;
      vvp_net_t*net = cp->net;
      എടുക്കുക_മൂല്യം(thr, value, wid); // Pop the value to store.

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      പ്രതിജ്ഞാ(queue);
      എങ്കിൽ (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot assign to a negative "
	         << get_queue_type(value)
	         << " index (" << idx << "). ";
	    print_queue_മൂല്യം(value);
	    cerr << " was not added." << endl;
      } അല്ലെങ്കിൽ എങ്കിൽ (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot assign to an undefined "
	         << get_queue_type(value) << " index. ";
	    print_queue_മൂല്യം(value);
	    cerr << " was not added." << endl;
      } അല്ലെങ്കിൽ {
	    unsigned max_size = thr->വാക്കുകൾ[cp->bit_idx[0]].w_int;
	    queue->set_word_max(idx, value, max_size);
      }
      തിരികെ സത്യം;
}

/*
 * %store/qdar/r <var>, idx
 */
തർക്കം of_STORE_QDAR_R(vthread_t thr, vvp_code_t cp)
{
      തിരികെ store_qdar<ഇരട്ട, vvp_queue_real>(thr, cp);
}

/*
 * %store/qdar/str <var>, idx
 */
തർക്കം of_STORE_QDAR_STR(vthread_t thr, vvp_code_t cp)
{
      തിരികെ store_qdar<string, vvp_queue_string>(thr, cp);
}

/*
 * %store/qdar/v <var>, idx
 */
തർക്കം of_STORE_QDAR_V(vthread_t thr, vvp_code_t cp)
{
      തിരികെ store_qdar<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

template <typename ELEM, വർഗ്ഗം QTYPE>
സ്ഥിതം തർക്കം store_qf(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
      ELEM value;
      vvp_net_t*net = cp->net;
      unsigned max_size = thr->വാക്കുകൾ[cp->bit_idx[0]].w_int;
      എടുക്കുക_മൂല്യം(thr, value, wid); // Pop the value to store.

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      പ്രതിജ്ഞാ(queue);
      queue->push_front(value, max_size);
      തിരികെ സത്യം;
}
/*
 * %store/qf/r <var-label>, <max-idx>
 */
തർക്കം of_STORE_QF_R(vthread_t thr, vvp_code_t cp)
{
      തിരികെ store_qf<ഇരട്ട, vvp_queue_real>(thr, cp);
}

/*
 * %store/qf/str <var-label>, <max-idx>
 */
തർക്കം of_STORE_QF_STR(vthread_t thr, vvp_code_t cp)
{
      തിരികെ store_qf<string, vvp_queue_string>(thr, cp);
}

/*
 * %store/qb/v <var-label>, <max-idx>, <wid>
 */
തർക്കം of_STORE_QF_V(vthread_t thr, vvp_code_t cp)
{
      തിരികെ store_qf<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

template <typename ELEM, വർഗ്ഗം QTYPE>
സ്ഥിതം തർക്കം store_qobj(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
// FIXME: Can we actually use wid here?
      (ശൂന്യം)wid;
      vvp_net_t*net = cp->net;

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      പ്രതിജ്ഞാ(queue);

      vvp_object_t src;
      thr->എടുക്കുക_object(src);

        // If it is null just clear the queue
      എങ്കിൽ (src.test_nil())
	    queue->erase_tail(0);
      അല്ലെങ്കിൽ {
	    unsigned max_size = thr->വാക്കുകൾ[cp->bit_idx[0]].w_int;
	    queue->copy_elems(src, max_size);
      }

      തിരികെ സത്യം;
}

തർക്കം of_STORE_QOBJ_R(vthread_t thr, vvp_code_t cp)
{
      തിരികെ store_qobj<ഇരട്ട, vvp_queue_real>(thr, cp);
}

തർക്കം of_STORE_QOBJ_STR(vthread_t thr, vvp_code_t cp)
{
      തിരികെ store_qobj<string, vvp_queue_string>(thr, cp);
}

തർക്കം of_STORE_QOBJ_V(vthread_t thr, vvp_code_t cp)
{
      തിരികെ store_qobj<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

സ്ഥിതം ശൂന്യം vvp_send(vthread_t thr, vvp_net_ptr_t ptr, സ്ഥിരം ഇരട്ട&val)
{
      vvp_send_real(ptr, val, thr->wt_context);
}

സ്ഥിതം ശൂന്യം vvp_send(vthread_t thr, vvp_net_ptr_t ptr, സ്ഥിരം string&val)
{
      vvp_send_string(ptr, val, thr->wt_context);
}

template <typename ELEM>
സ്ഥിതം തർക്കം store(vthread_t thr, vvp_code_t cp)
{
      ELEM val;
      എടുക്കുക_മൂല്യം(thr, val, 0);
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_send(thr, ptr, val);
      തിരികെ സത്യം;
}

തർക്കം of_STORE_REAL(vthread_t thr, vvp_code_t cp)
{
      തിരികെ store<ഇരട്ട>(thr, cp);
}

template <typename ELEM>
സ്ഥിതം തർക്കം storea(vthread_t thr, vvp_code_t cp)
{
      unsigned idx = cp->bit_idx[0];
      ELEM val;
      എടുക്കുക_മൂല്യം(thr, val, 0);

      എങ്കിൽ (thr->flags[4] != BIT4_1) {
	    unsigned adr = thr->വാക്കുകൾ[idx].w_int;
	    cp->array->set_word(adr, val);
      }

      തിരികെ സത്യം;
}

/*
 * %store/reala <var-label> <index>
 */
തർക്കം of_STORE_REALA(vthread_t thr, vvp_code_t cp)
{
      തിരികെ storea<ഇരട്ട>(thr, cp);
}

തർക്കം of_STORE_STR(vthread_t thr, vvp_code_t cp)
{
      തിരികെ store<string>(thr, cp);
}

/*
 * %store/stra <array-label> <index>
 */
തർക്കം of_STORE_STRA(vthread_t thr, vvp_code_t cp)
{
      തിരികെ storea<string>(thr, cp);
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
തർക്കം of_STORE_VEC4(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr(cp->net, 0);
      സ്ഥിരം vvp_signal_മൂല്യം*sig = dynamic_cast<vvp_signal_മൂല്യം*> (cp->net->fil);
      unsigned off_index = cp->bit_idx[0];
      unsigned പൂർണ്ണസംഖ്യ wid = cp->bit_idx[1];

      int64_t off = off_index ? thr->വാക്കുകൾ[off_index].w_int : 0;
      unsigned പൂർണ്ണസംഖ്യ sig_മൂല്യം_size = sig->മൂല്യം_size();

      vvp_vector4_t&val = thr->peek_vec4();
      unsigned val_size = val.size();

      എങ്കിൽ (val_size < wid) {
	    cerr << thr->get_fileline()
	         << "XXXX Internal error: val.size()=" << val_size
		 << ", expecting >= " << wid << endl;
      }
      പ്രതിജ്ഞാ(val_size >= wid);
      എങ്കിൽ (val_size > wid) {
	    val.resize(wid);
      }

	// If there is a problem loading the index register, flags-4
	// will be set to 1, and we know here to skip the actual assignment.
      എങ്കിൽ (off_index!=0 && thr->flags[4] == BIT4_1) {
	    thr->എടുക്കുക_vec4(1);
	    തിരികെ സത്യം;
      }

      എങ്കിൽ (!resize_rval_vec(val, off, sig_മൂല്യം_size)) {
	    thr->എടുക്കുക_vec4(1);
	    തിരികെ സത്യം;
      }

      എങ്കിൽ (off == 0 && val.size() == sig_മൂല്യം_size)
	    vvp_send_vec4(ptr, val, thr->wt_context);
      അല്ലെങ്കിൽ
	    vvp_send_vec4_pv(ptr, val, off, sig_മൂല്യം_size, thr->wt_context);

      thr->എടുക്കുക_vec4(1);
      തിരികെ സത്യം;
}

/*
 * %store/vec4a <var-label>, <addr>, <offset>
 */
തർക്കം of_STORE_VEC4A(vthread_t thr, vvp_code_t cp)
{
      unsigned adr_index = cp->bit_idx[0];
      unsigned off_index = cp->bit_idx[1];

      long adr = adr_index? thr->വാക്കുകൾ[adr_index].w_int : 0;
      int64_t off = off_index ? thr->വാക്കുകൾ[off_index].w_int : 0;

	// Suppress action if flags-4 is true.
      എങ്കിൽ (thr->flags[4] == BIT4_1) {
	    thr->എടുക്കുക_vec4(1);
	    തിരികെ സത്യം;
      }

      vvp_vector4_t &value = thr->peek_vec4();

      എങ്കിൽ (!resize_rval_vec(value, off, cp->array->get_word_size())) {
	    thr->എടുക്കുക_vec4(1);
	    തിരികെ സത്യം;
      }

      cp->array->set_word(adr, off, value);

      thr->എടുക്കുക_vec4(1);
      തിരികെ സത്യം;
}

/*
 * %sub
 *   pop r;
 *   pop l;
 *   push l-r;
 */
തർക്കം of_SUB(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t r = thr->എടുക്കുക_vec4();
      vvp_vector4_t&l = thr->peek_vec4();

      l.sub(r);
      തിരികെ സത്യം;
}

/*
 * %subi <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
തർക്കം of_SUBI(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->സംഖ്യ;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_immediate_rval (cp, r);

      l.sub(r);

      തിരികെ സത്യം;

}

തർക്കം of_SUB_WR(vthread_t thr, vvp_code_t)
{
      ഇരട്ട r = thr->എടുക്കുക_real();
      ഇരട്ട l = thr->എടുക്കുക_real();
      thr->തള്ളുക_real(l - r);
      തിരികെ സത്യം;
}

/*
 * %substr <first>, <last>
 * Pop a string, take the substring (SystemVerilog style), and return
 * the result to the stack. This opcode actually works by editing the
 * string in place.
 */
തർക്കം of_SUBSTR(vthread_t thr, vvp_code_t cp)
{
      int32_t first = thr->വാക്കുകൾ[cp->bit_idx[0]].w_int;
      int32_t last = thr->വാക്കുകൾ[cp->bit_idx[1]].w_int;
      string&val = thr->peek_str(0);

      എങ്കിൽ (first < 0 || last < first || last >= (int32_t)val.size()) {
	    val = string("");
	    തിരികെ സത്യം;
      }

      val = val.substr(first, last-first+1);
      തിരികെ സത്യം;
}

/*
 * %substr/vec4 <index>, <wid>
 */
തർക്കം of_SUBSTR_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned sel_idx = cp->bit_idx[0];
      unsigned wid = cp->bit_idx[1];

      int32_t sel = thr->വാക്കുകൾ[sel_idx].w_int;
      സ്ഥിരം string&val = thr->peek_str(0);

      പ്രതിജ്ഞാ(wid%8 == 0);

      എങ്കിൽ (sel < 0 || sel >= (int32_t)val.size()) {
	    vvp_vector4_t res (wid, BIT4_0);
	    thr->തള്ളുക_vec4(res);
	    തിരികെ സത്യം;
      }

      vvp_vector4_t res (wid, BIT4_0);

      പ്രതിജ്ഞാ(wid==8);
      unsigned അക്ഷരം tmp = val[sel];
      വേണ്ടി (പൂർണ്ണസംഖ്യ idx = 0 ; idx < 8 ; idx += 1) {
	    എങ്കിൽ (tmp & (1<<idx))
		  res.set_bit(idx, BIT4_1);
      }

      thr->തള്ളുക_vec4(res);
      തിരികെ സത്യം;
}

തർക്കം of_FILE_വരി(vthread_t thr, vvp_code_t cp)
{
      vpiHandle handle = cp->handle;

	/* When it is available, keep the file/line information in the
	   thread for error/warning messages. */
      thr->set_fileline(vpi_get_str(vpiFile, handle),
                        vpi_get(vpi_വരി_No, handle));

      എങ്കിൽ (show_file_വരി)
	    cerr << thr->get_fileline()
	         << vpi_get_str(_vpiDescription, handle) << endl;

      തിരികെ സത്യം;
}

/*
 * %test_nul <var-label>;
 * Test if the object at the specified variable is nil. If so, write
 * "1" into flags[4], otherwise write "0" into flags[4].
 */
തർക്കം of_TEST_NUL(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      പ്രതിജ്ഞാ(net);
      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      പ്രതിജ്ഞാ(obj);

      എങ്കിൽ (obj->get_object().test_nil())
	    thr->flags[4] = BIT4_1;
      അല്ലെങ്കിൽ
	    thr->flags[4] = BIT4_0;

      തിരികെ സത്യം;
}

തർക്കം of_TEST_NUL_A(vthread_t thr, vvp_code_t cp)
{
      unsigned idx = cp->bit_idx[0];
      unsigned adr = thr->വാക്കുകൾ[idx].w_int;
      vvp_object_t word;

	/* If the address is undefined, return true. */
      എങ്കിൽ (thr->flags[4] == BIT4_1) {
	    തിരികെ സത്യം;
      }

      cp->array->get_word_obj(adr, word);
      എങ്കിൽ (word.test_nil())
	    thr->flags[4] = BIT4_1;
      അല്ലെങ്കിൽ
	    thr->flags[4] = BIT4_0;

      തിരികെ സത്യം;
}

തർക്കം of_TEST_NUL_OBJ(vthread_t thr, vvp_code_t)
{
      എങ്കിൽ (thr->peek_object().test_nil())
	    thr->flags[4] = BIT4_1;
      അല്ലെങ്കിൽ
	    thr->flags[4] = BIT4_0;
      തിരികെ സത്യം;
}

/*
 * %test_nul/prop <pid>, <idx>
 */
തർക്കം of_TEST_NUL_PROP(vthread_t thr, vvp_code_t cp)
{
      unsigned pid = cp->സംഖ്യ;
      unsigned idx = cp->bit_idx[0];

      എങ്കിൽ (idx != 0) {
	    പ്രതിജ്ഞാ(idx < vthread_s::വാക്കുകൾ_എണ്ണം);
	    idx = thr->വാക്കുകൾ[idx].w_uint;
      }

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj  = obj.peek<vvp_cobject>();

      vvp_object_t val;
      cobj->get_object(pid, val, idx);

      എങ്കിൽ (val.test_nil())
	    thr->flags[4] = BIT4_1;
      അല്ലെങ്കിൽ
	    thr->flags[4] = BIT4_0;

      തിരികെ സത്യം;
}

/* and() reduction returns the scalar sum (Verilog-style bitwise-AND per word). */
template <വർഗ്ഗം SRC>
സ്ഥിതം vvp_vector4_t uparr_and_വാക്കുകൾ_src(SRC* src, unsigned wid)
{
      എങ്കിൽ (!src) തിരികെ vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_1);
      വേണ്ടി (size_t i = 0; i < src->get_size(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    res &= vi;
      }
      തിരികെ res;
}

തർക്കം of_UPARR_AND_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      എങ്കിൽ (qsrc) res = uparr_and_വാക്കുകൾ_src(qsrc, wid);
      അല്ലെങ്കിൽ എങ്കിൽ (dsrc) res = uparr_and_വാക്കുകൾ_src(dsrc, wid);
      അല്ലെങ്കിൽ {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    res = uparr_and_വാക്കുകൾ_src(src, wid);
      }
      thr->തള്ളുക_vec4(res);
      തിരികെ സത്യം;
}

/* or() reduction returns the scalar sum (Verilog-style bitwise-AND per word). */
template <വർഗ്ഗം SRC>
സ്ഥിതം vvp_vector4_t uparr_or_വാക്കുകൾ_src(SRC* src, unsigned wid)
{
      എങ്കിൽ (!src) തിരികെ vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_0);
      വേണ്ടി (size_t i = 0; i < src->get_size(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    res |= vi;
      }
      തിരികെ res;
}

തർക്കം of_UPARR_OR_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      എങ്കിൽ (qsrc) res = uparr_or_വാക്കുകൾ_src(qsrc, wid);
      അല്ലെങ്കിൽ എങ്കിൽ (dsrc) res = uparr_or_വാക്കുകൾ_src(dsrc, wid);
      അല്ലെങ്കിൽ {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    res = uparr_or_വാക്കുകൾ_src(src, wid);
      }
      thr->തള്ളുക_vec4(res);
      തിരികെ സത്യം;
}

/* real product() reduction returns the scalar sum */
template <വർഗ്ഗം SRC>
സ്ഥിതം ഇരട്ട uparr_product_വാക്കുകൾ_src_real(SRC* src)
{
      എങ്കിൽ (!src) തിരികെ 0.0;
      ഇരട്ട acc = 1.0;
      വേണ്ടി (size_t i = 0; i < src->get_size(); i += 1) {
	    ഇരട്ട vi;
	    src->get_word(i, vi);
	    acc *= vi;
      }
      തിരികെ acc;
}

തർക്കം of_UPARR_PRODUCT_REAL(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_queue_real* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_real_from_net(thr, net, qsrc, dsrc);
      ഇരട്ട sum;
      എങ്കിൽ (qsrc) sum = uparr_product_വാക്കുകൾ_src_real(qsrc);
      അല്ലെങ്കിൽ എങ്കിൽ (dsrc) sum = uparr_product_വാക്കുകൾ_src_real(dsrc);
      അല്ലെങ്കിൽ {
	    vvp_queue* src_q = get_queue_object<vvp_queue_real>(thr, net);
	    vvp_queue_real* src = dynamic_cast<vvp_queue_real*>(src_q);
	    sum = uparr_product_വാക്കുകൾ_src_real(src);
      }
      thr->തള്ളുക_real(sum);
      തിരികെ സത്യം;
}

/* product() reduction returns the scalar product (Verilog-style multiplication per word). */
template <വർഗ്ഗം SRC>
സ്ഥിതം vvp_vector4_t uparr_product_വാക്കുകൾ_src(SRC* src, unsigned wid)
{
      എങ്കിൽ (!src) തിരികെ vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t acc = queue_unique_ulong_to_vec4(1UL, wid);
      വേണ്ടി (size_t i = 0; i < src->get_size(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    acc.mul(vi);
      }
      തിരികെ acc;
}

തർക്കം of_UPARR_PRODUCT_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t prod;
      എങ്കിൽ (qsrc) prod = uparr_product_വാക്കുകൾ_src(qsrc, wid);
      അല്ലെങ്കിൽ എങ്കിൽ (dsrc) prod = uparr_product_വാക്കുകൾ_src(dsrc, wid);
      അല്ലെങ്കിൽ {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    prod = uparr_product_വാക്കുകൾ_src(src, wid);
      }
      thr->തള്ളുക_vec4(prod);
      തിരികെ സത്യം;
}

തർക്കം of_UPARR_PRODUCT_PROP_V(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->സംഖ്യ;
      unsigned wid = cp->bit_idx[0];

      vvp_object_t& മുകൾ = thr->peek_object();
      vvp_cobject*cobj = മുകൾ.peek<vvp_cobject>();
      പ്രതിജ്ഞാ(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t prod =
	    qsrc ? uparr_product_വാക്കുകൾ_src(qsrc, wid)
		 : uparr_product_വാക്കുകൾ_src(dsrc, wid);
      thr->തള്ളുക_vec4(prod);
      തിരികെ സത്യം;
}

തർക്കം of_UPARR_PRODUCT_OBJ_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->എടുക്കുക_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_vector4_t prod =
	    qsrc ? uparr_product_വാക്കുകൾ_src(qsrc, wid)
		 : uparr_product_വാക്കുകൾ_src(dsrc, wid);
      thr->തള്ളുക_vec4(prod);
      തിരികെ സത്യം;
}

/* real sum() reduction returns the scalar sum */
template <വർഗ്ഗം SRC>
സ്ഥിതം ഇരട്ട uparr_sum_വാക്കുകൾ_src_real(SRC* src)
{
      ഇരട്ട acc = 0.0;
      എങ്കിൽ (!src) തിരികെ acc;
      വേണ്ടി (size_t i = 0; i < src->get_size(); i += 1) {
	    ഇരട്ട vi;
	    src->get_word(i, vi);
	    acc += vi;
      }
      തിരികെ acc;
}

തർക്കം of_UPARR_SUM_REAL(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_queue_real* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_real_from_net(thr, net, qsrc, dsrc);
      ഇരട്ട sum;
      എങ്കിൽ (qsrc) sum = uparr_sum_വാക്കുകൾ_src_real(qsrc);
      അല്ലെങ്കിൽ എങ്കിൽ (dsrc) sum = uparr_sum_വാക്കുകൾ_src_real(dsrc);
      അല്ലെങ്കിൽ {
	    vvp_queue* src_q = get_queue_object<vvp_queue_real>(thr, net);
	    vvp_queue_real* src = dynamic_cast<vvp_queue_real*>(src_q);
	    sum = uparr_sum_വാക്കുകൾ_src_real(src);
      }
      thr->തള്ളുക_real(sum);
      തിരികെ സത്യം;
}

/* sum() reduction returns the scalar sum (Verilog-style addition per word). */
template <വർഗ്ഗം SRC>
സ്ഥിതം vvp_vector4_t uparr_sum_വാക്കുകൾ_src(SRC* src, unsigned wid)
{
      എങ്കിൽ (!src) തിരികെ vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t acc(wid, BIT4_0);
      വേണ്ടി (size_t i = 0; i < src->get_size(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    acc.add(vi);
      }
      തിരികെ acc;
}

തർക്കം of_UPARR_SUM_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t sum;
      എങ്കിൽ (qsrc) sum = uparr_sum_വാക്കുകൾ_src(qsrc, wid);
      അല്ലെങ്കിൽ എങ്കിൽ (dsrc) sum = uparr_sum_വാക്കുകൾ_src(dsrc, wid);
      അല്ലെങ്കിൽ {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    sum = uparr_sum_വാക്കുകൾ_src(src, wid);
      }
      thr->തള്ളുക_vec4(sum);
      തിരികെ സത്യം;
}

തർക്കം of_UPARR_SUM_PROP_V(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->സംഖ്യ;
      unsigned wid = cp->bit_idx[0];

      vvp_object_t& മുകൾ = thr->peek_object();
      vvp_cobject*cobj = മുകൾ.peek<vvp_cobject>();
      പ്രതിജ്ഞാ(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t sum =
	    qsrc ? uparr_sum_വാക്കുകൾ_src(qsrc, wid)
		 : uparr_sum_വാക്കുകൾ_src(dsrc, wid);
      thr->തള്ളുക_vec4(sum);
      തിരികെ സത്യം;
}

തർക്കം of_UPARR_SUM_OBJ_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->എടുക്കുക_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_vector4_t sum =
	    qsrc ? uparr_sum_വാക്കുകൾ_src(qsrc, wid)
		 : uparr_sum_വാക്കുകൾ_src(dsrc, wid);
      thr->തള്ളുക_vec4(sum);
      തിരികെ സത്യം;
}

/* xor() reduction returns the scalar sum (Verilog-style bitwise-XOR per word). */
template <വർഗ്ഗം SRC>
സ്ഥിതം vvp_vector4_t uparr_xor_വാക്കുകൾ_src(SRC* src, unsigned wid)
{
      എങ്കിൽ (!src) തിരികെ vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_0);
      വേണ്ടി (size_t i = 0; i < src->get_size(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    res ^= vi;
      }
      തിരികെ res;
}

തർക്കം of_UPARR_XOR_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      എങ്കിൽ (qsrc) res = uparr_xor_വാക്കുകൾ_src(qsrc, wid);
      അല്ലെങ്കിൽ എങ്കിൽ (dsrc) res = uparr_xor_വാക്കുകൾ_src(dsrc, wid);
      അല്ലെങ്കിൽ {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    res = uparr_xor_വാക്കുകൾ_src(src, wid);
      }
      thr->തള്ളുക_vec4(res);
      തിരികെ സത്യം;
}

തർക്കം of_VPI_CALL(vthread_t thr, vvp_code_t cp)
{
      vpip_execute_vpi_call(thr, cp->handle);

      എങ്കിൽ (schedule_stopped()) {
	    എങ്കിൽ (! schedule_finished())
		  schedule_vthread(thr, 0, അസത്യം);

	    തിരികെ അസത്യം;
      }

      തിരികെ schedule_finished()? അസത്യം : സത്യം;
}

/* %wait <label>;
 * Implement the wait by locating the vvp_net_T for the event, and
 * adding this thread to the threads list for the event. The some
 * argument is the  reference to the functor to wait for. This must be
 * an event object of some sort.
 */
തർക്കം of_WAIT(vthread_t thr, vvp_code_t cp)
{
      പ്രതിജ്ഞാ(! thr->i_am_in_function);
      പ്രതിജ്ഞാ(! thr->waiting_for_event);
      thr->waiting_for_event = 1;

	/* Add this thread to the list in the event. */
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (cp->net->fun);
      പ്രതിജ്ഞാ(ep);
      thr->wait_next = ep->add_waiting_thread(thr);

	/* Return false to suspend this thread. */
      തിരികെ അസത്യം;
}

/*
 * Implement the %wait/fork (SystemVerilog) instruction by suspending
 * the current thread until all the detached children have finished.
 */
തർക്കം of_WAIT_FORK(vthread_t thr, vvp_code_t)
{
	/* If a %wait/fork is being executed then the parent thread
	 * cannot be waiting in a join or already waiting. */
      പ്രതിജ്ഞാ(! thr->i_am_in_function);
      പ്രതിജ്ഞാ(! thr->i_am_joining);
      പ്രതിജ്ഞാ(! thr->i_am_waiting);

	/* There should be no active children when waiting. */
      പ്രതിജ്ഞാ(thr->children.empty());

	/* If there are no detached children then there is nothing to
	 * wait for. */
      എങ്കിൽ (thr->detached_children.empty()) തിരികെ സത്യം;

	/* Flag that this process is waiting for the detached children
	 * to finish and suspend it. */
      thr->i_am_waiting = 1;
      തിരികെ അസത്യം;
}

/*
 * %xnor
 */
തർക്കം of_XNOR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valr = thr->എടുക്കുക_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      പ്രതിജ്ഞാ(vall.size() == valr.size());

      vall ^= valr;
      vall.invert();

      തിരികെ സത്യം;
}

/*
 * %xor
 */
തർക്കം of_XOR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valr = thr->എടുക്കുക_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      പ്രതിജ്ഞാ(vall.size() == valr.size());

      vall ^= valr;

      തിരികെ സത്യം;
}


തർക്കം of_ZOMBIE(vthread_t thr, vvp_code_t)
{
      thr->pc = codespace_null();
      എങ്കിൽ ((thr->parent == 0) && (thr->children.empty())) {
	    എങ്കിൽ (thr->delay_delete)
		  schedule_del_thr(thr);
	    അല്ലെങ്കിൽ
		  vthread_delete(thr);
      }
      തിരികെ അസത്യം;
}

/*
 * This is a phantom opcode used to call user defined functions. It
 * is used in code generated by the .ufunc statement. It contains a
 * pointer to the executable code of the function and a pointer to
 * a ufunc_core object that has all the port information about the
 * function.
 */
സ്ഥിതം തർക്കം do_exec_ufunc(vthread_t thr, vvp_code_t cp, vthread_t child)
{
      __vpiScope*child_scope = cp->ufunc_core_ptr->func_scope();
      പ്രതിജ്ഞാ(child_scope);

      പ്രതിജ്ഞാ(child_scope->get_type_code() == vpiFunction);
      പ്രതിജ്ഞാ(thr->children.empty());


        /* We can take a number of shortcuts because we know that a
           continuous assignment can only occur in a static scope. */
      പ്രതിജ്ഞാ(thr->wt_context == 0);
      പ്രതിജ്ഞാ(thr->rd_context == 0);

        /* If an automatic function, allocate a context for this call. */
      vvp_context_t child_context = 0;
      എങ്കിൽ (child_scope->is_automatic()) {
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
      പ്രതിജ്ഞാ(thr->children.size()==1);

      child->is_scheduled = 1;
      child->i_am_in_function = 1;
      vthread_run(child);
      running_thread = thr;

      എങ്കിൽ (child->i_have_ended) {
	    do_join(thr, child);
            തിരികെ സത്യം;
      } അല്ലെങ്കിൽ {
	    thr->i_am_joining = 1;
	    തിരികെ അസത്യം;
      }
}

തർക്കം of_EXEC_UFUNC_REAL(vthread_t thr, vvp_code_t cp)
{
      __vpiScope*child_scope = cp->ufunc_core_ptr->func_scope();
      പ്രതിജ്ഞാ(child_scope);

	/* Create a temporary thread and run it immediately. */
      vthread_t child = vthread_new(cp->cptr, child_scope);
      thr->തള്ളുക_real(0.0);
      child->args_real.push_back(0);

      തിരികെ do_exec_ufunc(thr, cp, child);
}

തർക്കം of_EXEC_UFUNC_VEC4(vthread_t thr, vvp_code_t cp)
{
      __vpiScope*child_scope = cp->ufunc_core_ptr->func_scope();
      പ്രതിജ്ഞാ(child_scope);

      vpiScopeFunction*scope_func = dynamic_cast<vpiScopeFunction*>(child_scope);
      പ്രതിജ്ഞാ(scope_func);

	/* Create a temporary thread and run it immediately. */
      vthread_t child = vthread_new(cp->cptr, child_scope);
      thr->തള്ളുക_vec4(vvp_vector4_t(scope_func->get_func_width(), scope_func->get_func_init_val()));
      child->args_vec4.push_back(0);

      തിരികെ do_exec_ufunc(thr, cp, child);
}

/*
 * This is a phantom opcode used to harvest the result of calling a user
 * defined function. It is used in code generated by the .ufunc statement.
 */
തർക്കം of_REAP_UFUNC(vthread_t thr, vvp_code_t cp)
{
      __vpiScope*child_scope = cp->ufunc_core_ptr->func_scope();
      പ്രതിജ്ഞാ(child_scope);

	/* Copy the output from the result variable to the output
	   ports of the .ufunc device. */
      cp->ufunc_core_ptr->finish_thread();

        /* If an automatic function, free the context for this call. */
      എങ്കിൽ (child_scope->is_automatic()) {
            vthread_free_context(thr->rd_context, child_scope);
            thr->wt_context = 0;
            thr->rd_context = 0;
      }

      തിരികെ സത്യം;
}
