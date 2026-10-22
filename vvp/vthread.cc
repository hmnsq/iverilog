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

using ନାମାକାଶ std;

/* This is the size of an unsigned long in bits. This is just a
   convenience macro. */
# define CPU_WORD_BITS (8*ଆକାର(unsigned long))
# define ଶୀର୍ଷ_BIT (1UL << (CPU_WORD_BITS-1))

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

ସଂରଚନା vthread_s {
      vthread_s();

      ଶୂନ୍ୟ debug_dump(ostream&fd, ଅଚଳ ଅକ୍ଷର*label_text);

	/* This is the program counter. */
      vvp_code_t pc;
	/* These hold the private thread bits. */
      ଗଣନା { FLAGS_ଗଣନା = 512, ଶବ୍ଦଗୁଡ଼ିକ_ଗଣନା = 16 };
      vvp_bit4_t flags[FLAGS_ଗଣନା];

	/* These are the word registers. */
      ସଂଘ {
	    int64_t  w_int;
	    uint64_t w_uint;
      } ଶବ୍ଦଗୁଡ଼ିକ[ଶବ୍ଦଗୁଡ଼ିକ_ଗଣନା];

	// These vectors are depths within the parent thread's
	// corresponding stack.  This is how the %ret/* instructions
	// get at parent thread arguments.
      vector<unsigned> args_real;
      vector<unsigned> args_str;
      vector<unsigned> args_vec4;

    ବ୍ୟକ୍ତିଗତ:
      vector<vvp_vector4_t>ଗଦା_vec4;
    ସର୍ବସାଧାରଣ:
      ଧାଡ଼ିଭିତରେ vvp_vector4_t କାଢ଼_vec4(ଶୂନ୍ୟ)
      {
	    ପ୍ରତିଜ୍ଞା(! ଗଦା_vec4.empty());
	    vvp_vector4_t val = ଗଦା_vec4.back();
	    ଗଦା_vec4.pop_back();
	    ଫେରନ୍ତୁ val;
      }
      ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ ଠେଲ_vec4(ଅଚଳ vvp_vector4_t&val)
      {
	    ଗଦା_vec4.push_back(val);
      }
      ଧାଡ଼ିଭିତରେ ଅଚଳ vvp_vector4_t& peek_vec4(unsigned depth)
      {
	    unsigned size = ଗଦା_vec4.size();
	    ପ୍ରତିଜ୍ଞା(depth < size);
	    unsigned use_index = size-1-depth;
	    ଫେରନ୍ତୁ ଗଦା_vec4[use_index];
      }
      ଧାଡ଼ିଭିତରେ vvp_vector4_t& peek_vec4(ଶୂନ୍ୟ)
      {
	    unsigned use_index = ଗଦା_vec4.size();
	    ପ୍ରତିଜ୍ଞା(use_index >= 1);
	    ଫେରନ୍ତୁ ଗଦା_vec4[use_index-1];
      }
      ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ poke_vec4(unsigned depth, ଅଚଳ vvp_vector4_t&val)
      {
	    ପ୍ରତିଜ୍ଞା(depth < ଗଦା_vec4.size());
	    unsigned use_index = ଗଦା_vec4.size()-1-depth;
	    ଗଦା_vec4[use_index] = val;
      }
      ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ କାଢ଼_vec4(unsigned cnt)
      {
	    ଯେତେବେଳେ (cnt > 0) {
		  ଗଦା_vec4.pop_back();
		  cnt -= 1;
	    }
      }


    ବ୍ୟକ୍ତିଗତ:
      vector<ଦ୍ୱିଗୁଣ> ଗଦା_real;
    ସର୍ବସାଧାରଣ:
      ଧାଡ଼ିଭିତରେ ଦ୍ୱିଗୁଣ କାଢ଼_real(ଶୂନ୍ୟ)
      {
	    ପ୍ରତିଜ୍ଞା(! ଗଦା_real.empty());
	    ଦ୍ୱିଗୁଣ val = ଗଦା_real.back();
	    ଗଦା_real.pop_back();
	    ଫେରନ୍ତୁ val;
      }
      ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ ଠେଲ_real(ଦ୍ୱିଗୁଣ val)
      {
	    ଗଦା_real.push_back(val);
      }
      ଧାଡ଼ିଭିତରେ ଦ୍ୱିଗୁଣ peek_real(unsigned depth)
      {
	    ପ୍ରତିଜ୍ଞା(depth < ଗଦା_real.size());
	    unsigned use_index = ଗଦା_real.size()-1-depth;
	    ଫେରନ୍ତୁ ଗଦା_real[use_index];
      }
      ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ poke_real(unsigned depth, ଦ୍ୱିଗୁଣ val)
      {
	    ପ୍ରତିଜ୍ଞା(depth < ଗଦା_real.size());
	    unsigned use_index = ଗଦା_real.size()-1-depth;
	    ଗଦା_real[use_index] = val;
      }
      ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ କାଢ଼_real(unsigned cnt)
      {
	    ଯେତେବେଳେ (cnt > 0) {
		  ଗଦା_real.pop_back();
		  cnt -= 1;
	    }
      }

	/* Strings are operated on using a forth-like operator
	   set. Items at the top of the stack (back()) are the objects
	   operated on except for special cases. New objects are
	   pushed onto the top (back()) and pulled from the top
	   (back()) only. */
    ବ୍ୟକ୍ତିଗତ:
      vector<string> ଗଦା_str;
    ସର୍ବସାଧାରଣ:
      ଧାଡ଼ିଭିତରେ string କାଢ଼_str(ଶୂନ୍ୟ)
      {
	    ପ୍ରତିଜ୍ଞା(! ଗଦା_str.empty());
	    string val = ଗଦା_str.back();
	    ଗଦା_str.pop_back();
	    ଫେରନ୍ତୁ val;
      }
      ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ ଠେଲ_str(ଅଚଳ string&val)
      {
	    ଗଦା_str.push_back(val);
      }
      ଧାଡ଼ିଭିତରେ string&peek_str(unsigned depth)
      {
	    ପ୍ରତିଜ୍ଞା(depth<ଗଦା_str.size());
	    unsigned use_index = ଗଦା_str.size()-1-depth;
	    ଫେରନ୍ତୁ ଗଦା_str[use_index];
      }
      ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ poke_str(unsigned depth, ଅଚଳ string&val)
      {
	    ପ୍ରତିଜ୍ଞା(depth < ଗଦା_str.size());
	    unsigned use_index = ଗଦା_str.size()-1-depth;
	    ଗଦା_str[use_index] = val;
      }
      ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ କାଢ଼_str(unsigned cnt)
      {
	    ଯେତେବେଳେ (cnt > 0) {
		  ଗଦା_str.pop_back();
		  cnt -= 1;
	    }
      }

	/* Objects are also operated on in a stack. */
    ବ୍ୟକ୍ତିଗତ:
      ଗଣନା { ଗଦା_OBJ_MAX_SIZE = 32 };
      vvp_object_t ଗଦା_obj[ଗଦା_OBJ_MAX_SIZE];
      unsigned ଗଦା_obj_size;
    ସର୍ବସାଧାରଣ:
      ଧାଡ଼ିଭିତରେ vvp_object_t& peek_object(ଶୂନ୍ୟ)
      {
	    ପ୍ରତିଜ୍ଞା(ଗଦା_obj_size > 0);
	    ଫେରନ୍ତୁ ଗଦା_obj[ଗଦା_obj_size-1];
      }
      ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ କାଢ଼_object(vvp_object_t&obj)
      {
	    ପ୍ରତିଜ୍ଞା(ଗଦା_obj_size > 0);
	    ଗଦା_obj_size -= 1;
	    obj = ଗଦା_obj[ଗଦା_obj_size];
	    ଗଦା_obj[ଗଦା_obj_size].reset(0);
      }
      ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ କାଢ଼_object(unsigned cnt, unsigned skip =0)
      {
	    ପ୍ରତିଜ୍ଞା((cnt+skip) <= ଗଦା_obj_size);
	    ପାଇଁ (size_t idx = ଗଦା_obj_size-skip-cnt ; idx < ଗଦା_obj_size-skip ; idx += 1)
		  ଗଦା_obj[idx].reset(0);
	    ଗଦା_obj_size -= cnt;
	    ପାଇଁ (size_t idx = ଗଦା_obj_size-skip ; idx < ଗଦା_obj_size ; idx += 1)
		  ଗଦା_obj[idx] = ଗଦା_obj[idx+skip];
	    ପାଇଁ (size_t idx = ଗଦା_obj_size ; idx < ଗଦା_obj_size+skip ; idx += 1)
		  ଗଦା_obj[idx].reset(0);
      }
      ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ ଠେଲ_object(ଅଚଳ vvp_object_t&obj)
      {
	    ପ୍ରତିଜ୍ଞା(ଗଦା_obj_size < ଗଦା_OBJ_MAX_SIZE);
	    ଗଦା_obj[ଗଦା_obj_size] = obj;
	    ଗଦା_obj_size += 1;
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
      set<ସଂରଚନା vthread_s*>children;
	/* This points to the detached children of the thread. */
      set<ସଂରଚନା vthread_s*>detached_children;
	/* This points to my parent, if I have one. */
      ସଂରଚନା vthread_s*parent;
	/* This points to the containing scope. */
      __vpiScope*parent_scope;
	/* This is used for keeping wait queues. */
      ସଂରଚନା vthread_s*wait_next;
	/* These are used to access automatically allocated items. */
      vvp_context_t wt_context, rd_context;
	/* These are used to pass non-blocking event control information. */
      vvp_net_t*event;
      uint64_t ecount;
	/* Save the file/line information when available. */
    ବ୍ୟକ୍ତିଗତ:
      ଅକ୍ଷର *filenm_;
      unsigned lineno_;
    ସର୍ବସାଧାରଣ:
      ଶୂନ୍ୟ set_fileline(ଅକ୍ଷର *filenm, unsigned lineno);
      string get_fileline();

      ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ cleanup()
      {
	    ଯଦି (i_was_disabled) {
		  ଗଦା_vec4.clear();
		  ଗଦା_real.clear();
		  ଗଦା_str.clear();
		  କାଢ଼_object(ଗଦା_obj_size);
	    }
	    ମୁକ୍ତ(filenm_);
	    filenm_ = 0;
	    ପ୍ରତିଜ୍ଞା(ଗଦା_vec4.empty());
	    ପ୍ରତିଜ୍ଞା(ଗଦା_real.empty());
	    ପ୍ରତିଜ୍ଞା(ଗଦା_str.empty());
	    ପ୍ରତିଜ୍ଞା(ଗଦା_obj_size == 0);
      }
};

ଧାଡ଼ିଭିତରେ vthread_s::vthread_s()
{
      ଗଦା_obj_size = 0;
      filenm_ = 0;
      lineno_ = 0;
}

ଶୂନ୍ୟ vthread_s::set_fileline(ଅକ୍ଷର *filenm, unsigned lineno)
{
      ପ୍ରତିଜ୍ଞା(filenm);
      ଯଦି (!filenm_ || (strcmp(filenm_, filenm) != 0)) {
	    ମୁକ୍ତ(filenm_);
	    filenm_ = strdup(filenm);
      }
      lineno_ = lineno;
}

ଧାଡ଼ିଭିତରେ string vthread_s::get_fileline()
{
      ostringstream buf;
      ଯଦି (filenm_) {
	    buf << filenm_ << ":" << lineno_ << ": ";
      }
      string res = buf.str();
      ଫେରନ୍ତୁ res;
}

ଶୂନ୍ୟ vthread_s::debug_dump(ostream&fd, ଅଚଳ ଅକ୍ଷର*label)
{
      fd << "**** " << label << endl;
      fd << "**** ThreadId: " << ଏହି << ", parent id: " << parent << endl;

      fd << "**** Flags: ";
      ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = 0 ; idx < FLAGS_ଗଣନା ; idx += 1)
	    fd << flags[idx];
      fd << endl;
      fd << "**** vec4 stack..." << endl;
      ପାଇଁ (size_t idx = ଗଦା_vec4.size() ; idx > 0 ; idx -= 1)
	    fd << "    " << (ଗଦା_vec4.size()-idx) << ": " << ଗଦା_vec4[idx-1] << endl;
      fd << "**** str stack (" << ଗଦା_str.size() << ")..." << endl;
      fd << "**** obj stack (" << ଗଦା_obj_size << ")..." << endl;
      fd << "**** args_vec4 array (" << args_vec4.size() << ")..." << endl;
      ପାଇଁ (size_t idx = 0 ; idx < args_vec4.size() ; idx += 1)
	    fd << "    " << idx << ": " << args_vec4[idx] << endl;
      fd << "**** file/line (";
      ଯଦି (filenm_) fd << filenm_;
      ଅନ୍ୟଥା fd << "<no file name>";
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
ସ୍ଥିର string filter_string(ଅଚଳ ଅକ୍ଷର*text)
{
      vector<ଅକ୍ଷର> tmp (strlen(text)+1);
      size_t dst = 0;
      ପାଇଁ (ଅଚଳ ଅକ୍ଷର*ptr = text ; *ptr ; ptr += 1) {
	    // Not an escape? Move on.
	    ଯଦି (*ptr != '\\') {
		  tmp[dst++] = *ptr;
		  ଜାରି;
	    }

	    // Now we know that *ptr is pointing to a \ character and we
	    // have an octal sequence coming up. Advance the ptr and start
	    // processing octal digits.
	    ptr += 1;
	    ଯଦି (*ptr == 0)
		  ଭାଙ୍ଗ;

	    ଅକ୍ଷର byte = 0;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cnt = 3;
	    ଯେତେବେଳେ (*ptr && cnt > 0 && *ptr >= '0' && *ptr <= '7') {
		  byte *= 8;
		  byte += *ptr - '0';
		  cnt -= 1;
		  ptr += 1;
	    }

	    // null-bytes are supposed to be removed when assigning a string
	    // literal to a string.
	    ଯଦି (byte != '\0')
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
      ଫେରନ୍ତୁ res;
}

ସ୍ଥିର ଶୂନ୍ୟ do_join(vthread_t thr, vthread_t child);

__vpiScope* vthread_scope(ସଂରଚନା vthread_s*thr)
{
      ଫେରନ୍ତୁ thr->parent_scope;
}

ସଂରଚନା vthread_s*running_thread = 0;

string get_fileline()
{
      ଫେରନ୍ତୁ running_thread->get_fileline();
}

ଶୂନ୍ୟ vthread_ଠେଲ(ସଂରଚନା vthread_s*thr, ଦ୍ୱିଗୁଣ val)
{
      thr->ଠେଲ_real(val);
}

ଶୂନ୍ୟ vthread_ଠେଲ(ସଂରଚନା vthread_s*thr, ଅଚଳ string&val)
{
      thr->ଠେଲ_str(val);
}

ଶୂନ୍ୟ vthread_ଠେଲ(ସଂରଚନା vthread_s*thr, ଅଚଳ vvp_vector4_t&val)
{
      thr->ଠେଲ_vec4(val);
}

ଶୂନ୍ୟ vthread_କାଢ଼_real(ସଂରଚନା vthread_s*thr, unsigned depth)
{
      thr->କାଢ଼_real(depth);
}

ଶୂନ୍ୟ vthread_କାଢ଼_str(ସଂରଚନା vthread_s*thr, unsigned depth)
{
      thr->କାଢ଼_str(depth);
}

ଶୂନ୍ୟ vthread_କାଢ଼_vec4(ସଂରଚନା vthread_s*thr, unsigned depth)
{
      thr->କାଢ଼_vec4(depth);
}

ଦ୍ୱିଗୁଣ vthread_get_real_ଗଦା(ସଂରଚନା vthread_s*thr, unsigned depth)
{
      ଫେରନ୍ତୁ thr->peek_real(depth);
}

ଅଚଳ string&vthread_get_str_ଗଦା(ସଂରଚନା vthread_s*thr, unsigned depth)
{
      ଫେରନ୍ତୁ thr->peek_str(depth);
}

ଅଚଳ vvp_vector4_t& vthread_get_vec4_ଗଦା(ସଂରଚନା vthread_s*thr, unsigned depth)
{
      ଫେରନ୍ତୁ thr->peek_vec4(depth);
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
template <ଶ୍ରେଣୀ VVP_QUEUE> ସ୍ଥିର vvp_queue*get_queue_object(vthread_t thr, vvp_net_t*net)
{
      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      ପ୍ରତିଜ୍ଞା(obj);

      vvp_queue*queue = obj->get_object().peek<vvp_queue>();
      ଯଦି (queue == 0) {
	    ପ୍ରତିଜ୍ଞା(obj->get_object().test_nil());
	    queue = ନୂତନ VVP_QUEUE;
	    vvp_object_t val (queue);
	    vvp_net_ptr_t ptr (net, 0);
	    vvp_send_object(ptr, val, thr->wt_context);
      }

      ଫେରନ୍ତୁ queue;
}

/*
 * Locator and size/word opcodes use the same VVP net for a queue ($) or a
 * dynamic array ([]). Queues are vvp_queue_vec4; dynamic arrays may be
 * vvp_darray_vec4, vvp_darray_atom<>, etc. (all vvp_darray, but vvp_queue
 * also derives from vvp_darray — test queue vec4 first).
 */
ସ୍ଥିର ଶୂନ୍ୟ get_queue_or_darray_vec4_from_net(vthread_t thr, vvp_net_t* net,
					      vvp_queue_vec4*& qsrc,
					      vvp_darray*& dsrc)
{
      vvp_fun_signal_object* obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      ପ୍ରତିଜ୍ଞା(obj);
      vvp_object_t ଶୀର୍ଷ = obj->get_object();
      qsrc = ଶୀର୍ଷ.peek<vvp_queue_vec4>();
      ଯଦି (qsrc) {
	    dsrc = 0;
	    ଫେରନ୍ତୁ;
      }
      vvp_darray* dany = ଶୀର୍ଷ.peek<vvp_darray>();
      ଯଦି (dany && dynamic_cast<vvp_queue*>(dany) == 0) {
	    dsrc = dany;
	    ଫେରନ୍ତୁ;
      }
      ଯଦି (ଶୀର୍ଷ.test_nil()) {
	    qsrc = ନୂତନ vvp_queue_vec4();
	    vvp_object_t val(qsrc);
	    vvp_net_ptr_t ptr(net, 0);
	    vvp_send_object(ptr, val, thr->wt_context);
	    dsrc = 0;
	    ଫେରନ୍ତୁ;
      }
      /* e.g. real/string queue — not vec4 queue or plain darray */
      qsrc = 0;
      dsrc = 0;
}

ସ୍ଥିର ଶୂନ୍ୟ get_queue_or_darray_real_from_net(vthread_t thr, vvp_net_t* net,
					      vvp_queue_real*& qsrc,
					      vvp_darray*& dsrc)
{
      vvp_fun_signal_object* obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      ପ୍ରତିଜ୍ଞା(obj);
      vvp_object_t ଶୀର୍ଷ = obj->get_object();
      qsrc = ଶୀର୍ଷ.peek<vvp_queue_real>();
      ଯଦି (qsrc) {
	    dsrc = 0;
	    ଫେରନ୍ତୁ;
      }
      vvp_darray* dany = ଶୀର୍ଷ.peek<vvp_darray>();
      ଯଦି (dany && dynamic_cast<vvp_queue*>(dany) == 0) {
	    dsrc = dany;
	    ଫେରନ୍ତୁ;
      }
      ଯଦି (ଶୀର୍ଷ.test_nil()) {
	    qsrc = ନୂତନ vvp_queue_real();
	    vvp_object_t val(qsrc);
	    vvp_net_ptr_t ptr(net, 0);
	    vvp_send_object(ptr, val, thr->wt_context);
	    dsrc = 0;
	    ଫେରନ୍ତୁ;
      }
      /* e.g. real/string queue — not vec4 queue or plain darray */
      qsrc = 0;
      dsrc = 0;
}

ସ୍ଥିର ଶୂନ୍ୟ get_queue_or_darray_vec4_from_object(vvp_object_t src_obj,
						 vvp_queue_vec4*& qsrc,
						 vvp_darray*& dsrc)
{
      qsrc = src_obj.peek<vvp_queue_vec4>();
      ଯଦି (qsrc) {
	    dsrc = 0;
	    ଫେରନ୍ତୁ;
      }
      vvp_darray* dany = src_obj.peek<vvp_darray>();
      ଯଦି (dany && dynamic_cast<vvp_queue*>(dany) == 0) {
	    dsrc = dany;
	    ଫେରନ୍ତୁ;
      }
      qsrc = 0;
      dsrc = 0;
}

/*
 * The following are used to allow a common template to be written for
 * queue real/string/vec4 operations
 */
ଧାଡ଼ିଭିତରେ ସ୍ଥିର ଶୂନ୍ୟ କାଢ଼_ମୂଲ୍ୟ(vthread_t thr, ଦ୍ୱିଗୁଣ&value, unsigned)
{
      value = thr->କାଢ଼_real();
}

ଧାଡ଼ିଭିତରେ ସ୍ଥିର ଶୂନ୍ୟ କାଢ଼_ମୂଲ୍ୟ(vthread_t thr, string&value, unsigned)
{
      value = thr->କାଢ଼_str();
}

ଧାଡ଼ିଭିତରେ ସ୍ଥିର ଶୂନ୍ୟ କାଢ଼_ମୂଲ୍ୟ(vthread_t thr, vvp_vector4_t&value, unsigned wid)
{
      value = thr->କାଢ଼_vec4();
      ପ୍ରତିଜ୍ଞା(value.size() == wid);
}

/*
 * The following are used to allow the queue templates to print correctly.
 */
ଧାଡ଼ିଭିତରେ ସ୍ଥିର string get_queue_type(ଦ୍ୱିଗୁଣ&)
{
      ଫେରନ୍ତୁ "queue<real>";
}

ଧାଡ଼ିଭିତରେ ସ୍ଥିର string get_queue_type(string&)
{
      ଫେରନ୍ତୁ "queue<string>";
}

ଧାଡ଼ିଭିତରେ ସ୍ଥିର string get_queue_type(ଅଚଳ vvp_vector4_t&value)
{
      ostringstream buf;
      buf << "queue<vector[" << value.size() << "]>";
      string res = buf.str();
      ଫେରନ୍ତୁ res;
}

ଧାଡ଼ିଭିତରେ ସ୍ଥିର ଶୂନ୍ୟ print_queue_ମୂଲ୍ୟ(ଦ୍ୱିଗୁଣ value)
{
      cerr << value;
}

ଧାଡ଼ିଭିତରେ ସ୍ଥିର ଶୂନ୍ୟ print_queue_ମୂଲ୍ୟ(ଅଚଳ string&value)
{
      cerr << "\"" << value << "\"";
}

ଧାଡ଼ିଭିତରେ ସ୍ଥିର ଶୂନ୍ୟ print_queue_ମୂଲ୍ୟ(ଅଚଳ vvp_vector4_t&value)
{
      cerr << value;
}

/*
 * The following are used to get a darray/queue default value.
 */
ଧାଡ଼ିଭିତରେ ସ୍ଥିର ଶୂନ୍ୟ dq_default(ଦ୍ୱିଗୁଣ&value, unsigned)
{
      value = 0.0;
}

ଧାଡ଼ିଭିତରେ ସ୍ଥିର ଶୂନ୍ୟ dq_default(string&value, unsigned)
{
      value = "";
}

ଧାଡ଼ିଭିତରେ ସ୍ଥିର ଶୂନ୍ୟ dq_default(vvp_vector4_t&value, unsigned wid)
{
      value = vvp_vector4_t(wid);
}


template <ଶ୍ରେଣୀ T> T coerce_to_width(ଅଚଳ T&that, unsigned width)
{
      ଯଦି (that.size() == width)
	    ଫେରନ୍ତୁ that;

      ପ୍ରତିଜ୍ଞା(that.size() > width);
      T res (width);
      ପାଇଁ (unsigned idx = 0 ;  idx < width ;  idx += 1)
	    res.set_bit(idx, that.value(idx));

      ଫେରନ୍ତୁ res;
}

/* Explicitly define the vvp_vector4_t version of coerce_to_width(). */
template vvp_vector4_t coerce_to_width(ଅଚଳ vvp_vector4_t&that,
                                       unsigned width);


ସ୍ଥିର ଶୂନ୍ୟ multiply_array_imm(unsigned long*res, ଅଚଳ unsigned long*val,
			       unsigned ଶବ୍ଦଗୁଡ଼ିକ, unsigned long imm)
{
      ପାଇଁ (unsigned idx = 0 ; idx < ଶବ୍ଦଗୁଡ଼ିକ ; idx += 1)
	    res[idx] = 0;

      ପାଇଁ (unsigned mul_idx = 0 ; mul_idx < ଶବ୍ଦଗୁଡ଼ିକ ; mul_idx += 1) {
	    unsigned long sum;
	    unsigned long tmp = multiply_with_carry(val[mul_idx], imm, sum);

	    unsigned long carry = 0;
	    res[mul_idx] = add_with_carry(res[mul_idx], tmp, carry);
	    ପାଇଁ (unsigned add_idx = mul_idx+1 ; add_idx < ଶବ୍ଦଗୁଡ଼ିକ ; add_idx += 1) {
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
ସ୍ଥିର vvp_context_t vthread_alloc_context(__vpiScope*scope)
{
      ପ୍ରତିଜ୍ଞା(scope->is_automatic());

      vvp_context_t context = scope->free_contexts;
      ଯଦି (context) {
            scope->free_contexts = vvp_get_next_context(context);
            ପାଇଁ (unsigned idx = 0 ; idx < scope->nitem ; idx += 1) {
                  scope->item[idx]->reset_instance(context);
            }
      } ଅନ୍ୟଥା {
            context = vvp_allocate_context(scope->nitem);
            ପାଇଁ (unsigned idx = 0 ; idx < scope->nitem ; idx += 1) {
                  scope->item[idx]->alloc_instance(context);
            }
      }

      vvp_set_next_context(context, scope->live_contexts);
      scope->live_contexts = context;

      ଫେରନ୍ତୁ context;
}

/*
 * Free a context previously allocated to a child thread by pushing it
 * onto the freed context stack. Remove it from the list of live contexts
 * in that scope.
 */
ସ୍ଥିର ଶୂନ୍ୟ vthread_free_context(vvp_context_t context, __vpiScope*scope)
{
      ପ୍ରତିଜ୍ଞା(scope->is_automatic());
      ପ୍ରତିଜ୍ଞା(context);

      ଯଦି (context == scope->live_contexts) {
            scope->live_contexts = vvp_get_next_context(context);
      } ଅନ୍ୟଥା {
            vvp_context_t tmp = scope->live_contexts;
            ଯେତେବେଳେ (context != vvp_get_next_context(tmp)) {
                  ପ୍ରତିଜ୍ଞା(tmp);
                  tmp = vvp_get_next_context(tmp);
            }
            vvp_set_next_context(tmp, vvp_get_next_context(context));
      }

      vvp_set_next_context(context, scope->free_contexts);
      scope->free_contexts = context;
}

#ifdef CHECK_WITH_VALGRIND
ଶୂନ୍ୟ contexts_delete(ଶ୍ରେଣୀ __vpiScope*scope)
{
      vvp_context_t context = scope->free_contexts;

      ଯେତେବେଳେ (context) {
	    scope->free_contexts = vvp_get_next_context(context);
	    ପାଇଁ (unsigned idx = 0; idx < scope->nitem; idx += 1) {
		  scope->item[idx]->free_instance(context);
	    }
	    ମୁକ୍ତ(context);
	    context = scope->free_contexts;
      }
      ମୁକ୍ତ(scope->item);
}
#endif

/*
 * Create a new thread with the given start address.
 */
vthread_t vthread_new(vvp_code_t pc, __vpiScope*scope)
{
      vthread_t thr = ନୂତନ ସଂରଚନା vthread_s;
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
      ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = 4 ; idx < 8 ; idx += 1)
	    thr->flags[idx] = BIT4_X;

      scope->threads .insert(thr);
      ଫେରନ୍ତୁ thr;
}

#ifdef CHECK_WITH_VALGRIND
#ଯଦି 0
/*
 * These are not currently correct. If you use them you will get
 * double delete messages. There is still a leak related to a
 * waiting event that needs to be investigated.
 */

ସ୍ଥିର ଶୂନ୍ୟ wait_next_delete(vthread_t base)
{
      ଯେତେବେଳେ (base) {
	    vthread_t tmp = base->wait_next;
	    delete base;
	    base = tmp;
	    ଯଦି (base->waiting_for_event == 0) ଭାଙ୍ଗ;
      }
}

ସ୍ଥିର ଶୂନ୍ୟ child_delete(vthread_t base)
{
      ଯେତେବେଳେ (base) {
	    vthread_t tmp = base->child;
	    delete base;
	    base = tmp;
      }
}
#endif

ଶୂନ୍ୟ vthreads_delete(ଶ୍ରେଣୀ __vpiScope*scope)
{
      ପାଇଁ (std::set<vthread_t>::iterator cur = scope->threads.begin()
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
ସ୍ଥିର ଶୂନ୍ୟ vthread_reap(vthread_t thr)
{
      ଯଦି (! thr->children.empty()) {
	    ପାଇଁ (set<vthread_t>::iterator cur = thr->children.begin()
		       ; cur != thr->children.end() ; ++cur) {
		  vthread_t child = *cur;
		  ପ୍ରତିଜ୍ଞା(child);
		  ପ୍ରତିଜ୍ଞା(child->parent == thr);
		  child->parent = thr->parent;
	    }
      }
      ଯଦି (! thr->detached_children.empty()) {
	    ପାଇଁ (set<vthread_t>::iterator cur = thr->detached_children.begin()
		       ; cur != thr->detached_children.end() ; ++cur) {
		  vthread_t child = *cur;
		  ପ୍ରତିଜ୍ଞା(child);
		  ପ୍ରତିଜ୍ଞା(child->parent == thr);
		  ପ୍ରତିଜ୍ଞା(child->i_am_detached);
		  child->parent = 0;
		  child->i_am_detached = 0;
	    }
      }
      ଯଦି (thr->parent) {
	      /* assert that the given element was removed. */
	    ଯଦି (thr->i_am_detached) {
		  size_t res = thr->parent->detached_children.erase(thr);
		  ପ୍ରତିଜ୍ଞା(res == 1);
	    } ଅନ୍ୟଥା {
		  size_t res = thr->parent->children.erase(thr);
		  ପ୍ରତିଜ୍ଞା(res == 1);
	    }
      }

      thr->parent = 0;

	// Remove myself from the containing scope if needed.
      thr->parent_scope->threads.erase(thr);

      thr->pc = codespace_null();

	/* If this thread is not scheduled, then is it safe to delete
	   it now. Otherwise, let the schedule event (which will
	   execute the thread at of_ZOMBIE) delete the object. */
      ଯଦି ((thr->is_scheduled == 0) && (thr->waiting_for_event == 0)) {
	    ପ୍ରତିଜ୍ଞା(thr->children.empty());
	    ପ୍ରତିଜ୍ଞା(thr->wait_next == 0);
	    ଯଦି (thr->delay_delete)
		  schedule_del_thr(thr);
	    ଅନ୍ୟଥା
		  vthread_delete(thr);
      }
}

ଶୂନ୍ୟ vthread_delete(vthread_t thr)
{
      thr->cleanup();
      delete thr;
}

ଶୂନ୍ୟ vthread_mark_scheduled(vthread_t thr)
{
      ଯେତେବେଳେ (thr != 0) {
	    ପ୍ରତିଜ୍ଞା(thr->is_scheduled == 0);
	    thr->is_scheduled = 1;
	    thr = thr->wait_next;
      }
}

ଶୂନ୍ୟ vthread_mark_final(vthread_t thr)
{
      /*
       * The behavior in a final thread is the same as in a function. Any
       * child thread will be executed immediately rather than being
       * scheduled.
       */
      ଯେତେବେଳେ (thr != 0) {
	    thr->i_am_in_function = 1;
	    thr = thr->wait_next;
      }
}

ଶୂନ୍ୟ vthread_delay_delete()
{
      ଯଦି (running_thread)
	    running_thread->delay_delete = 1;
}

/*
 * This function runs each thread by fetching an instruction,
 * incrementing the PC, and executing the instruction. The thread may
 * be the head of a list, so each thread is run so far as possible.
 */
ଶୂନ୍ୟ vthread_run(vthread_t thr)
{
      ଯେତେବେଳେ (thr != 0) {
	    vthread_t tmp = thr->wait_next;
	    thr->wait_next = 0;

	    ପ୍ରତିଜ୍ଞା(thr->is_scheduled);
	    thr->is_scheduled = 0;

            running_thread = thr;

	    ପାଇଁ (;;) {
		  vvp_code_t cp = thr->pc;
		  thr->pc += 1;

		    /* Run the opcode implementation. If the execution of
		       the opcode returns false, then the thread is meant to
		       be paused, so break out of the loop. */
		  ତାର୍କିକ rc = (cp->opcode)(thr, cp);
		  ଯଦି (rc == ମିଥ୍ୟା)
			ଭାଙ୍ଗ;
	    }

	    thr = tmp;
      }
      running_thread = 0;
}

/*
 * The CHUNK_LINK instruction is a special next pointer for linking
 * chunks of code space. It's like a simplified %jmp.
 */
ତାର୍କିକ of_CHUNK_LINK(vthread_t thr, vvp_code_t code)
{
      ପ୍ରତିଜ୍ଞା(code->cptr);
      thr->pc = code->cptr;
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * This is called by an event functor to wake up all the threads on
 * its list. I in fact created that list in the %wait instruction, and
 * I also am certain that the waiting_for_event flag is set.
 */
ଶୂନ୍ୟ vthread_schedule_list(vthread_t thr)
{
      ପାଇଁ (vthread_t cur = thr ;  cur ;  cur = cur->wait_next) {
	    ପ୍ରତିଜ୍ଞା(cur->waiting_for_event);
	    cur->waiting_for_event = 0;
      }

      schedule_vthread(thr, 0);
}

vvp_context_t vthread_get_wt_context()
{
      ଯଦି (running_thread)
            ଫେରନ୍ତୁ running_thread->wt_context;
      ଅନ୍ୟଥା
            ଫେରନ୍ତୁ 0;
}

vvp_context_t vthread_get_rd_context()
{
      ଯଦି (running_thread)
            ଫେରନ୍ତୁ running_thread->rd_context;
      ଅନ୍ୟଥା
            ଫେରନ୍ତୁ 0;
}

vvp_context_item_t vthread_get_wt_context_item(unsigned context_idx)
{
      ପ୍ରତିଜ୍ଞା(running_thread && running_thread->wt_context);
      ଫେରନ୍ତୁ vvp_get_context_item(running_thread->wt_context, context_idx);
}

vvp_context_item_t vthread_get_rd_context_item(unsigned context_idx)
{
      ପ୍ରତିଜ୍ଞା(running_thread && running_thread->rd_context);
      ଫେରନ୍ତୁ vvp_get_context_item(running_thread->rd_context, context_idx);
}

/*
 * %abs/wr
 */
ତାର୍କିକ of_ABS_WR(vthread_t thr, vvp_code_t)
{
      thr->ଠେଲ_real( fabs(thr->କାଢ଼_real()) );
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_ALLOC(vthread_t thr, vvp_code_t cp)
{
        /* Allocate a context. */
      vvp_context_t child_context = vthread_alloc_context(cp->scope);

        /* Push the allocated context onto the write context stack. */
      vvp_set_stacked_context(child_context, thr->wt_context);
      thr->wt_context = child_context;

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_AND(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valb = thr->କାଢ଼_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();
      ପ୍ରତିଜ୍ଞା(vala.size() == valb.size());
      vala &= valb;
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * This function must ALWAYS be called with the val set to the right
 * size, and initialized with BIT4_0 bits. Certain optimizations rely
 * on that.
 */
ସ୍ଥିର ଶୂନ୍ୟ get_immediate_rval(vvp_code_t cp, vvp_vector4_t&val)
{
      uint32_t vala = cp->bit_idx[0];
      uint32_t valb = cp->bit_idx[1];
      unsigned wid  = cp->ସଂଖ୍ୟା;

      ଯଦି (valb == 0) {
	      // Special case: if the value is zero, we are done
	      // before we start.
	    ଯଦି (vala == 0) ଫେରନ୍ତୁ;

	      // Special case: The value has no X/Z bits, so we can
	      // use the setarray method to write the value all at once.
	    unsigned use_wid = 8*ଆକାର(unsigned long);
	    ଯଦି (wid < use_wid)
		  use_wid = wid;
	    unsigned long tmp[1];
	    tmp[0] = vala;
	    val.setarray(0, use_wid, tmp);
	    ଫେରନ୍ତୁ;
      }

	// The immediate value can be values bigger then 32 bits, but
	// only if the high bits are zero. So at most we need to run
	// through the loop below 32 times. Maybe less, if the target
	// width is less. We don't have to do anything special on that
	// because vala/valb bits will shift away so (vala|valb) will
	// turn to zero at or before 32 shifts.

      ପାଇଁ (unsigned idx = 0 ; idx < wid && (vala|valb) ; idx += 1) {
	    uint32_t ba = 0;
	      // Convert the vala/valb bits to a ba number that
	      // matches the encoding of the vvp_bit4_t enumeration.
	    ba = (valb & 1) << 1;
	    ba |= vala & 1;

	      // Note that the val is already pre-filled with BIT4_0
	      // bits, os we only need to set non-zero bit values.
	    ଯଦି (ba) val.set_bit(idx, (vvp_bit4_t)ba);

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
ତାର୍କିକ of_ADD(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t r = thr->କାଢ଼_vec4();
	// Rather then pop l, use it directly from the stack. When we
	// assign to 'l', that will edit the top of the stack, which
	// replaces a pop and a pull.
      vvp_vector4_t&l = thr->peek_vec4();

      l.add(r);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %addi <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
ତାର୍କିକ of_ADDI(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->ସଂଖ୍ୟା;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_immediate_rval (cp, r);

      l.add(r);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %add/wr
 */
ତାର୍କିକ of_ADD_WR(vthread_t thr, vvp_code_t)
{
      ଦ୍ୱିଗୁଣ r = thr->କାଢ଼_real();
      ଦ୍ୱିଗୁଣ l = thr->କାଢ଼_real();
      thr->ଠେଲ_real(l + r);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/* %assign/ar <array>, <delay>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory. <delay>
 * is the delay in simulation time. <bit> is the index register
 * containing the real value.
 */
ତାର୍କିକ of_ASSIGN_AR(vthread_t thr, vvp_code_t cp)
{
      long adr = thr->ଶବ୍ଦଗୁଡ଼ିକ[3].w_int;
      unsigned delay = cp->bit_idx[0];
      ଦ୍ୱିଗୁଣ value = thr->କାଢ଼_real();

      ଯଦି (adr >= 0) {
	    schedule_assign_array_word(cp->array, adr, value, delay);
      }

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/* %assign/ar/d <array>, <delay_idx>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory.
 * <delay_idx> is the integer register that contains the delay value.
 */
ତାର୍କିକ of_ASSIGN_ARD(vthread_t thr, vvp_code_t cp)
{
      long adr = thr->ଶବ୍ଦଗୁଡ଼ିକ[3].w_int;
      ଦ୍ୱିଗୁଣ value = thr->କାଢ଼_real();

      ଯଦି (adr >= 0) {
	    vvp_time64_t delay = thr->ଶବ୍ଦଗୁଡ଼ିକ[cp->bit_idx[0]].w_uint;
	    schedule_assign_array_word(cp->array, adr, value, delay);
      }

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/* %assign/ar/e <array>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory. <bit>
 * is the index register containing the real value. The event
 * information is contained in the thread event control registers
 * and is set with %evctl.
 */
ତାର୍କିକ of_ASSIGN_ARE(vthread_t thr, vvp_code_t cp)
{
      long adr = thr->ଶବ୍ଦଗୁଡ଼ିକ[3].w_int;
      ଦ୍ୱିଗୁଣ value = thr->କାଢ଼_real();

      ଯଦି (adr >= 0) {
	    ଯଦି (thr->ecount == 0) {
		  schedule_assign_array_word(cp->array, adr, value, 0);
	    } ଅନ୍ୟଥା {
		  schedule_evctl(cp->array, adr, value, thr->event,
		                 thr->ecount);
	    }
      }

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %assign/vec4 <var>, <delay>
 */
ତାର୍କିକ of_ASSIGN_VEC4(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      unsigned delay = cp->bit_idx[0];
      ଅଚଳ vvp_vector4_t&val = thr->peek_vec4();

      schedule_assign_vector(ptr, 0, 0, val, delay);
      thr->କାଢ଼_vec4(1);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * Resizes a vector value for a partial assignment so that the value is fully
 * in-bounds of the target signal. Both `val` and `off` will be updated if
 * necessary.
 *
 * Returns false if the value is fully out-of-bounds and the assignment should
 * be skipped. Otherwise returns true.
 */
ସ୍ଥିର ତାର୍କିକ resize_rval_vec(vvp_vector4_t &val, int64_t &off,
			    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା sig_wid)
{
      unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା wid = val.size();

        // Fully in bounds, most likely case
      ଯଦି (off >= 0 && (uint64_t)off + wid <= sig_wid)
	    ଫେରନ୍ତୁ ସତ୍ୟ;

      unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା base = 0;
      ଯଦି (off >= 0) {
	      // Fully out-of-bounds
	    ଯଦି ((uint64_t)off >= sig_wid)
		  ଫେରନ୍ତୁ ମିଥ୍ୟା;
      } ଅନ୍ୟଥା {
	      // Fully out-of-bounds */
	    ଯଦି ((uint64_t)(-off) >= wid)
		  ଫେରନ୍ତୁ ମିଥ୍ୟା;

	      // If the index is below the vector, then only assign the high
	      // bits that overlap with the target
	    base = -off;
	    wid += off;
		off = 0;
      }

	// If the value is partly above the target, then only assign
	// the bits that overlap
      ଯଦି ((uint64_t)off + wid > sig_wid)
	    wid = sig_wid - (uint64_t)off;

      val = val.subvalue(base, wid);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %assign/vec4/a/d <arr>, <offx>, <delx>
 */
ତାର୍କିକ of_ASSIGN_VEC4_A_D(vthread_t thr, vvp_code_t cp)
{
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା off_idx = cp->bit_idx[0];
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା del_idx = cp->bit_idx[1];
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା adr_idx = 3;

      int64_t  off = off_idx ? thr->ଶବ୍ଦଗୁଡ଼ିକ[off_idx].w_int : 0;
      vvp_time64_t del = del_idx? thr->ଶବ୍ଦଗୁଡ଼ିକ[del_idx].w_uint : 0;
      long     adr = thr->ଶବ୍ଦଗୁଡ଼ିକ[adr_idx].w_int;

      vvp_vector4_t val = thr->କାଢ଼_vec4();

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      ଯଦି (thr->flags[4] == BIT4_1)
	    ଫେରନ୍ତୁ ସତ୍ୟ;

      ଯଦି (!resize_rval_vec(val, off, cp->array->get_word_size()))
	    ଫେରନ୍ତୁ ସତ୍ୟ;

      schedule_assign_array_word(cp->array, adr, off, val, del);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %assign/vec4/a/e <arr>, <offx>
 */
ତାର୍କିକ of_ASSIGN_VEC4_A_E(vthread_t thr, vvp_code_t cp)
{
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା off_idx = cp->bit_idx[0];
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା adr_idx = 3;

      int64_t  off = off_idx ? thr->ଶବ୍ଦଗୁଡ଼ିକ[off_idx].w_int : 0;
      long     adr = thr->ଶବ୍ଦଗୁଡ଼ିକ[adr_idx].w_int;

      vvp_vector4_t val = thr->କାଢ଼_vec4();

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      ଯଦି (thr->flags[4] == BIT4_1)
	    ଫେରନ୍ତୁ ସତ୍ୟ;

      ଯଦି (!resize_rval_vec(val, off, cp->array->get_word_size()))
	    ଫେରନ୍ତୁ ସତ୍ୟ;

      ଯଦି (thr->ecount == 0) {
	    schedule_assign_array_word(cp->array, adr, off, val, 0);
      } ଅନ୍ୟଥା {
	    schedule_evctl(cp->array, adr, val, off, thr->event, thr->ecount);
      }

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %assign/vec4/off/d <var>, <off>, <del>
 */
ତାର୍କିକ of_ASSIGN_VEC4_OFF_D(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      unsigned off_index = cp->bit_idx[0];
      unsigned del_index = cp->bit_idx[1];
      vvp_vector4_t val = thr->କାଢ଼_vec4();

      int64_t off = thr->ଶବ୍ଦଗୁଡ଼ିକ[off_index].w_int;
      vvp_time64_t del = thr->ଶବ୍ଦଗୁଡ଼ିକ[del_index].w_uint;

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      ଯଦି (thr->flags[4] == BIT4_1)
	    ଫେରନ୍ତୁ ସତ୍ୟ;

      vvp_signal_ମୂଲ୍ୟ*sig = dynamic_cast<vvp_signal_ମୂଲ୍ୟ*> (cp->net->fil);
      ପ୍ରତିଜ୍ଞା(sig);

      ଯଦି (!resize_rval_vec(val, off, sig->ମୂଲ୍ୟ_size()))
	    ଫେରନ୍ତୁ ସତ୍ୟ;

      schedule_assign_vector(ptr, off, sig->ମୂଲ୍ୟ_size(), val, del);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %assign/vec4/off/e <var>, <off>
 */
ତାର୍କିକ of_ASSIGN_VEC4_OFF_E(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      unsigned off_index = cp->bit_idx[0];
      vvp_vector4_t val = thr->କାଢ଼_vec4();

      int64_t off = thr->ଶବ୍ଦଗୁଡ଼ିକ[off_index].w_int;

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      ଯଦି (thr->flags[4] == BIT4_1)
	    ଫେରନ୍ତୁ ସତ୍ୟ;

      vvp_signal_ମୂଲ୍ୟ*sig = dynamic_cast<vvp_signal_ମୂଲ୍ୟ*> (cp->net->fil);
      ପ୍ରତିଜ୍ଞା(sig);

      ଯଦି (!resize_rval_vec(val, off, sig->ମୂଲ୍ୟ_size()))
	    ଫେରନ୍ତୁ ସତ୍ୟ;

      ଯଦି (thr->ecount == 0) {
	    schedule_assign_vector(ptr, off, sig->ମୂଲ୍ୟ_size(), val, 0);
      } ଅନ୍ୟଥା {
	    schedule_evctl(ptr, val, off, sig->ମୂଲ୍ୟ_size(), thr->event, thr->ecount);
      }

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %assign/vec4/d <var-label> <delay>
 */
ତାର୍କିକ of_ASSIGN_VEC4D(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      unsigned del_index = cp->bit_idx[0];
      vvp_time64_t del = thr->ଶବ୍ଦଗୁଡ଼ିକ[del_index].w_int;

      vvp_vector4_t value = thr->କାଢ଼_vec4();

      vvp_signal_ମୂଲ୍ୟ*sig = dynamic_cast<vvp_signal_ମୂଲ୍ୟ*> (cp->net->fil);
      ପ୍ରତିଜ୍ଞା(sig);

      schedule_assign_vector(ptr, 0, sig->ମୂଲ୍ୟ_size(), value, del);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %assign/vec4/e <var-label>
 */
ତାର୍କିକ of_ASSIGN_VEC4E(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_vector4_t value = thr->କାଢ଼_vec4();

      vvp_signal_ମୂଲ୍ୟ*sig = dynamic_cast<vvp_signal_ମୂଲ୍ୟ*> (cp->net->fil);
      ପ୍ରତିଜ୍ଞା(sig);

      ଯଦି (thr->ecount == 0) {
	    schedule_assign_vector(ptr, 0, sig->ମୂଲ୍ୟ_size(), value, 0);
      } ଅନ୍ୟଥା {
	    schedule_evctl(ptr, value, 0, sig->ମୂଲ୍ୟ_size(), thr->event, thr->ecount);
      }

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * This is %assign/wr <vpi-label>, <delay>
 *
 * This assigns (after a delay) a value to a real variable. Use the
 * vpi_put_value function to do the assign, with the delay written
 * into the vpiInertialDelay carrying the desired delay.
 */
ତାର୍କିକ of_ASSIGN_WR(vthread_t thr, vvp_code_t cp)
{
      unsigned delay = cp->bit_idx[0];
      ଦ୍ୱିଗୁଣ value = thr->କାଢ଼_real();
      s_vpi_time del;

      del.type = vpiSimTime;
      vpip_time_to_timestruct(&del, delay);

      __vpiHandle*tmp = cp->handle;

      t_vpi_ମୂଲ୍ୟ val;
      val.format = vpiRealVal;
      val.value.real = value;
      vpi_put_ମୂଲ୍ୟ(tmp, &val, &del, vpiTransportDelay);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_ASSIGN_WRD(vthread_t thr, vvp_code_t cp)
{
      vvp_time64_t delay = thr->ଶବ୍ଦଗୁଡ଼ିକ[cp->bit_idx[0]].w_uint;
      ଦ୍ୱିଗୁଣ value = thr->କାଢ଼_real();
      s_vpi_time del;

      del.type = vpiSimTime;
      vpip_time_to_timestruct(&del, delay);

      __vpiHandle*tmp = cp->handle;

      t_vpi_ମୂଲ୍ୟ val;
      val.format = vpiRealVal;
      val.value.real = value;
      vpi_put_ମୂଲ୍ୟ(tmp, &val, &del, vpiTransportDelay);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_ASSIGN_WRE(vthread_t thr, vvp_code_t cp)
{
      ପ୍ରତିଜ୍ଞା(thr->event != 0);
      ଦ୍ୱିଗୁଣ value = thr->କାଢ଼_real();
      __vpiHandle*tmp = cp->handle;

	// If the count is zero then just put the value.
      ଯଦି (thr->ecount == 0) {
	    t_vpi_ମୂଲ୍ୟ val;

	    val.format = vpiRealVal;
	    val.value.real = value;
	    vpi_put_ମୂଲ୍ୟ(tmp, &val, 0, vpiNoDelay);
      } ଅନ୍ୟଥା {
	    schedule_evctl(tmp, value, thr->event, thr->ecount);
      }

      thr->event = 0;
      thr->ecount = 0;

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_BLEND(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t vala = thr->କାଢ଼_vec4();
      vvp_vector4_t valb = thr->କାଢ଼_vec4();
      ପ୍ରତିଜ୍ଞା(vala.size() == valb.size());

      ପାଇଁ (unsigned idx = 0 ; idx < vala.size() ; idx += 1) {
	    ଯଦି (vala.value(idx) == valb.value(idx))
		  ଜାରି;

	    vala.set_bit(idx, BIT4_X);
      }

      thr->ଠେଲ_vec4(vala);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_BLEND_STR(vthread_t thr, vvp_code_t)
{
      string f = thr->କାଢ଼_str();
      string t = thr->କାଢ଼_str();
      thr->ଠେଲ_str((t == f) ? t : "");
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_BLEND_WR(vthread_t thr, vvp_code_t)
{
      ଦ୍ୱିଗୁଣ f = thr->କାଢ଼_real();
      ଦ୍ୱିଗୁଣ t = thr->କାଢ଼_real();
      thr->ଠେଲ_real((t == f) ? t : 0.0);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_BREAKPOINT(vthread_t, vvp_code_t)
{
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %callf/void <code-label>, <scope-label>
 * Combine the %fork and %join steps for invoking a function.
 */
ସ୍ଥିର ତାର୍କିକ do_callf_void(vthread_t thr, vthread_t child)
{

      ଯଦି (child->parent_scope->is_automatic()) {
	      /* The context allocated for this child is the top entry
		 on the write context stack */
	    child->wt_context = thr->wt_context;
	    child->rd_context = thr->wt_context;
      }

        // Mark the function thread as a direct child of the current thread.
      child->parent = thr;
      thr->children.insert(child);
        // This should be the only child
      ପ୍ରତିଜ୍ଞା(thr->children.size()==1);

        // Execute the function. This SHOULD run the function to completion,
        // but there are some exceptional situations where it won't.
      ପ୍ରତିଜ୍ଞା(child->parent_scope->get_type_code() == vpiFunction);
      child->is_scheduled = 1;
      child->i_am_in_function = 1;
      vthread_run(child);
      running_thread = thr;

      ଯଦି (child->i_have_ended) {
	    do_join(thr, child);
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      } ଅନ୍ୟଥା {
	    thr->i_am_joining = 1;
	    ଫେରନ୍ତୁ ମିଥ୍ୟା;
      }
}

ତାର୍କିକ of_CALLF_OBJ(vthread_t thr, vvp_code_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);
      ଫେରନ୍ତୁ do_callf_void(thr, child);

      // XXXX NOT IMPLEMENTED
}

ତାର୍କିକ of_CALLF_REAL(vthread_t thr, vvp_code_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);

	// This is the return value. Push a place-holder value. The function
	// will replace this with the actual value using a %ret/real instruction.
      thr->ଠେଲ_real(0.0);
      child->args_real.push_back(0);

      ଫେରନ୍ତୁ do_callf_void(thr, child);
}

ତାର୍କିକ of_CALLF_STR(vthread_t thr, vvp_code_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);

      thr->ଠେଲ_str("");
      child->args_str.push_back(0);

      ଫେରନ୍ତୁ do_callf_void(thr, child);
}

ତାର୍କିକ of_CALLF_VEC4(vthread_t thr, vvp_code_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);

      vpiScopeFunction*scope_func = dynamic_cast<vpiScopeFunction*>(cp->scope);
      ପ୍ରତିଜ୍ଞା(scope_func);

	// This is the return value. Push a place-holder value. The function
	// will replace this with the actual value using a %ret/real instruction.
      thr->ଠେଲ_vec4(vvp_vector4_t(scope_func->get_func_width(), scope_func->get_func_init_val()));
      child->args_vec4.push_back(0);

      ଫେରନ୍ତୁ do_callf_void(thr, child);
}

ତାର୍କିକ of_CALLF_VOID(vthread_t thr, vvp_code_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);
      ଫେରନ୍ତୁ do_callf_void(thr, child);
}

/*
 * The %cassign/link instruction connects a source node to a
 * destination node. The destination node must be a signal, as it is
 * marked with the source of the cassign so that it may later be
 * unlinked without specifically knowing the source that this
 * instruction used.
 */
ତାର୍କିକ of_CASSIGN_LINK(vthread_t, vvp_code_t cp)
{
      vvp_net_t*dst = cp->net;
      vvp_net_t*src = cp->net2;

      vvp_fun_signal_base*sig
	    = dynamic_cast<vvp_fun_signal_base*>(dst->fun);
      ପ୍ରତିଜ୍ଞା(sig);

	/* Any previous continuous assign should have been removed already. */
      ପ୍ରତିଜ୍ଞା(sig->cassign_link == 0);

      sig->cassign_link = src;

	/* Link the output of the src to the port[1] (the cassign
	   port) of the destination. */
      vvp_net_ptr_t dst_ptr (dst, 1);
      src->link(dst_ptr);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * If there is an existing continuous assign linked to the destination
 * node, unlink it. This must be done before applying a new continuous
 * assign, otherwise the initial assigned value will be propagated to
 * any other nodes driven by the old continuous assign source.
 */
ସ୍ଥିର ଶୂନ୍ୟ cassign_unlink(vvp_net_t*dst)
{
      vvp_fun_signal_base*sig
	    = dynamic_cast<vvp_fun_signal_base*>(dst->fun);
      ପ୍ରତିଜ୍ଞା(sig);

      ଯଦି (sig->cassign_link == 0)
	    ଫେରନ୍ତୁ;

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
ତାର୍କିକ of_CASSIGN_VEC4(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      vvp_vector4_t value = thr->କାଢ଼_vec4();

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

	/* Set the value into port 1 of the destination. */
      vvp_net_ptr_t ptr (net, 1);
      vvp_send_vec4(ptr, value, 0);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %cassign/vec4/off <var>, <off>
 */
ତାର୍କିକ of_CASSIGN_VEC4_OFF(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      unsigned base_idx = cp->bit_idx[0];
      long base = thr->ଶବ୍ଦଗୁଡ଼ିକ[base_idx].w_int;
      vvp_vector4_t value = thr->କାଢ଼_vec4();
      unsigned wid = value.size();

      ଯଦି (thr->flags[4] == BIT4_1)
	    ଫେରନ୍ତୁ ସତ୍ୟ;

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

      vvp_signal_ମୂଲ୍ୟ*sig = dynamic_cast<vvp_signal_ମୂଲ୍ୟ*> (net->fil);
      ପ୍ରତିଜ୍ଞା(sig);

      ଯଦି (base < 0 && (wid <= (unsigned)-base))
	    ଫେରନ୍ତୁ ସତ୍ୟ;

      ଯଦି (base >= (long)sig->ମୂଲ୍ୟ_size())
	    ଫେରନ୍ତୁ ସତ୍ୟ;

      ଯଦି (base < 0) {
	    wid -= (unsigned) -base;
	    base = 0;
	    value.resize(wid);
      }

      ଯଦି (base+wid > sig->ମୂଲ୍ୟ_size()) {
	    wid = sig->ମୂଲ୍ୟ_size() - base;
	    value.resize(wid);
      }

      vvp_net_ptr_t ptr (net, 1);
      vvp_send_vec4_pv(ptr, value, base, sig->ମୂଲ୍ୟ_size(), 0);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_CASSIGN_WR(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net  = cp->net;
      ଦ୍ୱିଗୁଣ value = thr->କାଢ଼_real();

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

	/* Set the value into port 1 of the destination. */
      vvp_net_ptr_t ptr (net, 1);
      vvp_send_real(ptr, value, 0);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %cast2
 */
ତାର୍କିକ of_CAST2(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      unsigned wid = val.size();

      ପାଇଁ (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    ବାଛ (val.value(idx)) {
		ସ୍ଥିତି BIT4_0:
		ସ୍ଥିତି BIT4_1:
		  ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		  val.set_bit(idx, BIT4_0);
		  ଭାଙ୍ଗ;
	    }
      }

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ do_cast_vec_dar(vthread_t thr, vvp_code_t cp, ତାର୍କିକ as_vec4)
{
      unsigned wid = cp->ସଂଖ୍ୟା;

      vvp_object_t obj;
      thr->କାଢ଼_object(obj);

      vvp_darray*darray = obj.peek<vvp_darray>();
      ପ୍ରତିଜ୍ଞା(darray);

      vvp_vector4_t vec = darray->get_bitstream(as_vec4);
      ଯଦି (vec.size() != wid) {
	    cerr << thr->get_fileline()
	         << "VVP error: size mismatch when casting dynamic array to vector." << endl;
            thr->ଠେଲ_vec4(vvp_vector4_t(wid));
            schedule_stop(0);
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
      }
      thr->ଠେଲ_vec4(vec);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %cast/vec2/dar <wid>
 */
ତାର୍କିକ of_CAST_VEC2_DAR(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ do_cast_vec_dar(thr, cp, ମିଥ୍ୟା);
}

/*
 * %cast/vec4/dar <wid>
 */
ତାର୍କିକ of_CAST_VEC4_DAR(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ do_cast_vec_dar(thr, cp, ସତ୍ୟ);
}

/*
 * %cast/vec4/str <wid>
 */
ତାର୍କିକ of_CAST_VEC4_STR(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->ସଂଖ୍ୟା;
      string str = thr->କାଢ଼_str();

      vvp_vector4_t vec(wid, BIT4_0);

      ଯଦି (wid != 8*str.length()) {
	    cerr << thr->get_fileline()
	         << "VVP error: size mismatch when casting string to vector." << endl;
            thr->ଠେଲ_vec4(vec);
            schedule_stop(0);
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
      }

      unsigned sdx = 0;
      unsigned vdx = wid;
      ଯେତେବେଳେ (vdx > 0) {
            ଅକ୍ଷର ch = str[sdx++];
            vdx -= 8;
            ପାଇଁ (unsigned bdx = 0; bdx < 8; bdx += 1) {
                  ଯଦି (ch & 1)
                        vec.set_bit(vdx+bdx, BIT4_1);
                  ch >>= 1;
            }
      }

      thr->ଠେଲ_vec4(vec);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ସ୍ଥିର ଶୂନ୍ୟ do_CMPE(vthread_t thr, ଅଚଳ vvp_vector4_t&lval, ଅଚଳ vvp_vector4_t&rval)
{
      ପ୍ରତିଜ୍ଞା(rval.size() == lval.size());

      ଯଦି (lval.has_xz() || rval.has_xz()) {

	    unsigned wid = lval.size();
	    vvp_bit4_t eq  = BIT4_1;
	    vvp_bit4_t eeq = BIT4_1;

	    ପାଇଁ (unsigned idx = 0 ; idx < wid ; idx += 1) {
		  vvp_bit4_t lv = lval.value(idx);
		  vvp_bit4_t rv = rval.value(idx);

		  ଯଦି (lv != rv)
			eeq = BIT4_0;

		  ଯଦି (eq==BIT4_1 && (bit4_is_xz(lv) || bit4_is_xz(rv)))
			eq = BIT4_X;
		  ଯଦି ((lv == BIT4_0) && (rv==BIT4_1))
			eq = BIT4_0;
		  ଯଦି ((lv == BIT4_1) && (rv==BIT4_0))
			eq = BIT4_0;

		  ଯଦି (eq == BIT4_0)
			ଭାଙ୍ଗ;
	    }

	    thr->flags[4] = eq;
	    thr->flags[6] = eeq;

      } ଅନ୍ୟଥା {
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
ତାର୍କିକ of_CMPE(vthread_t thr, vvp_code_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ଅଚଳ vvp_vector4_t&rval = thr->peek_vec4(0);
      ଅଚଳ vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPE(thr, lval, rval);

      thr->କାଢ଼_vec4(2);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_CMPNE(vthread_t thr, vvp_code_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ଅଚଳ vvp_vector4_t&rval = thr->peek_vec4(0);
      ଅଚଳ vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];
      thr->flags[6] =  ~thr->flags[6];

      thr->କାଢ଼_vec4(2);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %cmpi/e <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
ତାର୍କିକ of_CMPIE(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->ସଂଖ୍ୟା;

      ଅଚଳ vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_immediate_rval (cp, rval);

      do_CMPE(thr, lval, rval);

      thr->କାଢ଼_vec4(1);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_CMPINE(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->ସଂଖ୍ୟା;

      ଅଚଳ vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_immediate_rval (cp, rval);

      do_CMPE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];
      thr->flags[6] =  ~thr->flags[6];

      thr->କାଢ଼_vec4(1);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}



ସ୍ଥିର ଶୂନ୍ୟ do_CMPS(vthread_t thr, ଅଚଳ vvp_vector4_t&lval, ଅଚଳ vvp_vector4_t&rval)
{
      ପ୍ରତିଜ୍ଞା(rval.size() == lval.size());

	// If either value has XZ bits, then the eq and lt values are
	// known already to be X. Just calculate the eeq result as a
	// special case and short circuit the rest of the compare.
      ଯଦି (lval.has_xz() || rval.has_xz()) {
	    thr->flags[4] = BIT4_X; // eq
	    thr->flags[5] = BIT4_X; // lt
	    thr->flags[6] = lval.eeq(rval)? BIT4_1 : BIT4_0;
	    ଫେରନ୍ତୁ;
      }

	// Past this point, we know we are dealing only with fully
	// defined values.
      unsigned wid = lval.size();

      ଅଚଳ vvp_bit4_t sig1 = lval.value(wid-1);
      ଅଚଳ vvp_bit4_t sig2 = rval.value(wid-1);

	// If the lval is <0 and the rval is >=0, then we know the result.
      ଯଦି ((sig1 == BIT4_1) && (sig2 == BIT4_0)) {
	    thr->flags[4] = BIT4_0; // eq;
	    thr->flags[5] = BIT4_1; // lt;
	    thr->flags[6] = BIT4_0; // eeq
	    ଫେରନ୍ତୁ;
      }

	// If the lval is >=0 and the rval is <0, then we know the result.
      ଯଦି ((sig1 == BIT4_0) && (sig2 == BIT4_1)) {
	    thr->flags[4] = BIT4_0; // eq;
	    thr->flags[5] = BIT4_0; // lt;
	    thr->flags[6] = BIT4_0; // eeq
	    ଫେରନ୍ତୁ;
      }

	// The values have the same sign, so we have to look at the
	// actual value. Scan from the MSB down. As soon as we find a
	// bit that differs, we know the result.

      ପାଇଁ (unsigned idx = 1 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t lv = lval.value(wid-1-idx);
	    vvp_bit4_t rv = rval.value(wid-1-idx);

	    ଯଦି (lv == rv)
		  ଜାରି;

	    thr->flags[4] = BIT4_0; // eq
	    thr->flags[6] = BIT4_0; // eeq

	    ଯଦି (lv==BIT4_0) {
		  thr->flags[5] = BIT4_1; // lt
	    } ଅନ୍ୟଥା {
		  thr->flags[5] = BIT4_0; // lt
	    }
	    ଫେରନ୍ତୁ;
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
ତାର୍କିକ of_CMPS(vthread_t thr, vvp_code_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ଅଚଳ vvp_vector4_t&rval = thr->peek_vec4(0);
      ଅଚଳ vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPS(thr, lval, rval);

      thr->କାଢ଼_vec4(2);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %cmpi/s <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
ତାର୍କିକ of_CMPIS(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->ସଂଖ୍ୟା;

      ଅଚଳ vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_immediate_rval (cp, rval);

      do_CMPS(thr, lval, rval);

      thr->କାଢ଼_vec4(1);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_CMPSTR(vthread_t thr, vvp_code_t)
{
      string re = thr->କାଢ଼_str();
      string le = thr->କାଢ଼_str();

      ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = strcmp(le.c_str(), re.c_str());

      vvp_bit4_t eq;
      vvp_bit4_t lt;

      ଯଦି (rc == 0) {
	    eq = BIT4_1;
	    lt = BIT4_0;
      } ଅନ୍ୟଥା ଯଦି (rc < 0) {
	    eq = BIT4_0;
	    lt = BIT4_1;
      } ଅନ୍ୟଥା {
	    eq = BIT4_0;
	    lt = BIT4_0;
      }

      thr->flags[4] = eq;
      thr->flags[5] = lt;

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ସ୍ଥିର ଶୂନ୍ୟ of_CMPU_the_hard_way(vthread_t thr, unsigned wid,
				 ଅଚଳ vvp_vector4_t&lval,
				 ଅଚଳ vvp_vector4_t&rval)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_bit4_t eeq = BIT4_1;

      ପାଇଁ (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t lv = lval.value(idx);
	    vvp_bit4_t rv = rval.value(idx);

	    ଯଦି (lv != rv)
		  eeq = BIT4_0;

	    ଯଦି (eq==BIT4_1 && (bit4_is_xz(lv) || bit4_is_xz(rv)))
		  eq = BIT4_X;
	    ଯଦି ((lv == BIT4_0) && (rv==BIT4_1))
		  eq = BIT4_0;
	    ଯଦି ((lv == BIT4_1) && (rv==BIT4_0))
		  eq = BIT4_0;

	    ଯଦି (eq == BIT4_0)
		  ଭାଙ୍ଗ;

      }

      thr->flags[4] = eq;
      thr->flags[5] = BIT4_X;
      thr->flags[6] = eeq;
}

ସ୍ଥିର ଶୂନ୍ୟ do_CMPU(vthread_t thr, ଅଚଳ vvp_vector4_t&lval, ଅଚଳ vvp_vector4_t&rval)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_bit4_t lt = BIT4_0;

      ଯଦି (rval.size() != lval.size()) {
	    cerr << thr->get_fileline()
	         << "VVP ERROR: %cmp/u operand width mismatch: lval=" << lval
		 << ", rval=" << rval << endl;
      }
      ପ୍ରତିଜ୍ଞା(rval.size() == lval.size());
      unsigned wid = lval.size();

      unsigned long*larray = lval.subarray(0,wid);
      ଯଦି (larray == 0) ଫେରନ୍ତୁ of_CMPU_the_hard_way(thr, wid, lval, rval);

      unsigned long*rarray = rval.subarray(0,wid);
      ଯଦି (rarray == 0) {
	    delete[]larray;
	    ଫେରନ୍ତୁ of_CMPU_the_hard_way(thr, wid, lval, rval);
      }

      unsigned ଶବ୍ଦଗୁଡ଼ିକ = (wid+CPU_WORD_BITS-1) / CPU_WORD_BITS;

      ପାଇଁ (unsigned wdx = 0 ; wdx < ଶବ୍ଦଗୁଡ଼ିକ ; wdx += 1) {
	    ଯଦି (larray[wdx] == rarray[wdx])
		  ଜାରି;

	    eq = BIT4_0;
	    ଯଦି (larray[wdx] < rarray[wdx])
		  lt = BIT4_1;
	    ଅନ୍ୟଥା
		  lt = BIT4_0;
      }

      delete[]larray;
      delete[]rarray;

      thr->flags[4] = eq;
      thr->flags[5] = lt;
      thr->flags[6] = eq;
}

ତାର୍କିକ of_CMPU(vthread_t thr, vvp_code_t)
{

      ଅଚଳ vvp_vector4_t&rval = thr->peek_vec4(0);
      ଅଚଳ vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPU(thr, lval, rval);

      thr->କାଢ଼_vec4(2);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %cmpi/u <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
ତାର୍କିକ of_CMPIU(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->ସଂଖ୍ୟା;

      ଅଚଳ vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_immediate_rval (cp, rval);

      do_CMPU(thr, lval, rval);

      thr->କାଢ଼_vec4(1);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}


/*
 * %cmp/x
 */
ତାର୍କିକ of_CMPX(vthread_t thr, vvp_code_t)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_vector4_t rval = thr->କାଢ଼_vec4();
      vvp_vector4_t lval = thr->କାଢ଼_vec4();

      ପ୍ରତିଜ୍ଞା(rval.size() == lval.size());
      unsigned wid = lval.size();

      ପାଇଁ (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    vvp_bit4_t lv = lval.value(idx);
	    vvp_bit4_t rv = rval.value(idx);
	    ଯଦି ((lv != rv) && !bit4_is_xz(lv) && !bit4_is_xz(rv)) {
		  eq = BIT4_0;
		  ଭାଙ୍ଗ;
	    }
      }

      thr->flags[4] = eq;
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ସ୍ଥିର ଶୂନ୍ୟ do_CMPWE(vthread_t thr, ଅଚଳ vvp_vector4_t&lval, ଅଚଳ vvp_vector4_t&rval)
{
      ପ୍ରତିଜ୍ଞା(rval.size() == lval.size());

      ଯଦି (lval.has_xz() || rval.has_xz()) {

	    unsigned wid = lval.size();
	    vvp_bit4_t eq  = BIT4_1;

	    ପାଇଁ (unsigned idx = 0 ; idx < wid ; idx += 1) {
		  vvp_bit4_t lv = lval.value(idx);
		  vvp_bit4_t rv = rval.value(idx);

		  ଯଦି (bit4_is_xz(rv))
			ଜାରି;
		  ଯଦି ((eq == BIT4_1) && bit4_is_xz(lv))
			eq = BIT4_X;
		  ଯଦି ((lv == BIT4_0) && (rv==BIT4_1))
			eq = BIT4_0;
		  ଯଦି ((lv == BIT4_1) && (rv==BIT4_0))
			eq = BIT4_0;

		  ଯଦି (eq == BIT4_0)
			ଭାଙ୍ଗ;
	    }

	    thr->flags[4] = eq;

      } ଅନ୍ୟଥା {
	      // If there are no XZ bits anywhere, then the results of
	      // ==? match the === test.
	    thr->flags[4] = (lval.eeq(rval)? BIT4_1 : BIT4_0);
      }
}

ତାର୍କିକ of_CMPWE(vthread_t thr, vvp_code_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ଅଚଳ vvp_vector4_t&rval = thr->peek_vec4(0);
      ଅଚଳ vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPWE(thr, lval, rval);

      thr->କାଢ଼_vec4(2);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_CMPWNE(vthread_t thr, vvp_code_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ଅଚଳ vvp_vector4_t&rval = thr->peek_vec4(0);
      ଅଚଳ vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPWE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];

      thr->କାଢ଼_vec4(2);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_CMPWR(vthread_t thr, vvp_code_t)
{
      ଦ୍ୱିଗୁଣ r = thr->କାଢ଼_real();
      ଦ୍ୱିଗୁଣ l = thr->କାଢ଼_real();

      vvp_bit4_t eq = (l == r)? BIT4_1 : BIT4_0;
      vvp_bit4_t lt = (l <  r)? BIT4_1 : BIT4_0;

      thr->flags[4] = eq;
      thr->flags[5] = lt;

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %cmp/z
 */
ତାର୍କିକ of_CMPZ(vthread_t thr, vvp_code_t)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_vector4_t rval = thr->କାଢ଼_vec4();
      vvp_vector4_t lval = thr->କାଢ଼_vec4();

      ପ୍ରତିଜ୍ଞା(rval.size() == lval.size());
      unsigned wid = lval.size();

      ପାଇଁ (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    vvp_bit4_t lv = lval.value(idx);
	    vvp_bit4_t rv = rval.value(idx);
	    ଯଦି ((lv != rv) && (rv != BIT4_Z) && (lv != BIT4_Z)) {
		  eq = BIT4_0;
		  ଭାଙ୍ଗ;
	    }
      }

      thr->flags[4] = eq;
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 *  %concat/str;
 */
ତାର୍କିକ of_CONCAT_STR(vthread_t thr, vvp_code_t)
{
      string text = thr->କାଢ଼_str();
      thr->peek_str(0).append(text);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 *  %concati/str <string>;
 */
ତାର୍କିକ of_CONCATI_STR(vthread_t thr, vvp_code_t cp)
{
      ଅଚଳ ଅକ୍ଷର*text = cp->text;
      thr->peek_str(0).append(filter_string(text));
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %concat/vec4
 */
ତାର୍କିକ of_CONCAT_VEC4(vthread_t thr, vvp_code_t)
{
      ଅଚଳ vvp_vector4_t&lsb = thr->peek_vec4(0);
      ଅଚଳ vvp_vector4_t&msb = thr->peek_vec4(1);

	// The result is the size of the top two vectors in the stack.
      vvp_vector4_t res (msb.size() + lsb.size(), BIT4_X);

	// Build up the result.
      res.set_vec(0, lsb);
      res.set_vec(lsb.size(), msb);

	// Rearrange the stack to pop the inputs and push the
	// result. Do that by actually popping only 1 stack position
	// and replacing the new top with the new value.
      thr->କାଢ଼_vec4(1);
      thr->peek_vec4() = res;

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %concati/vec4 <vala>, <valb>, <wid>
 *
 * Concat the immediate value to the LOW bits of the concatenation.
 * Get the HIGH bits from the top of the vec4 stack.
 */
ତାର୍କିକ of_CONCATI_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned wid  = cp->ସଂଖ୍ୟା;

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
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %cvt/rv
 */
ତାର୍କିକ of_CVT_RV(vthread_t thr, vvp_code_t)
{
      ଦ୍ୱିଗୁଣ val;
      vvp_vector4_t val4 = thr->କାଢ଼_vec4();
      vector4_to_ମୂଲ୍ୟ(val4, val, ମିଥ୍ୟା);
      thr->ଠେଲ_real(val);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %cvt/rv/s
 */
ତାର୍କିକ of_CVT_RV_S(vthread_t thr, vvp_code_t)
{
      ଦ୍ୱିଗୁଣ val;
      vvp_vector4_t val4 = thr->କାଢ଼_vec4();
      vector4_to_ମୂଲ୍ୟ(val4, val, ସତ୍ୟ);
      thr->ଠେଲ_real(val);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %cvt/sr <idx>
 * Pop the top value from the real stack, convert it to a 64bit signed
 * and save it to the indexed register.
 */
ତାର୍କିକ of_CVT_SR(vthread_t thr, vvp_code_t cp)
{
      ଦ୍ୱିଗୁଣ r = thr->କାଢ଼_real();
      thr->ଶବ୍ଦଗୁଡ଼ିକ[cp->bit_idx[0]].w_int = i64round(r);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %cvt/ur <idx>
 */
ତାର୍କିକ of_CVT_UR(vthread_t thr, vvp_code_t cp)
{
      ଦ୍ୱିଗୁଣ r = thr->କାଢ଼_real();
      thr->ଶବ୍ଦଗୁଡ଼ିକ[cp->bit_idx[0]].w_uint = vlg_round_to_u64(r);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %cvt/vr <wid>
 */
ତାର୍କିକ of_CVT_VR(vthread_t thr, vvp_code_t cp)
{
      ଦ୍ୱିଗୁଣ r = thr->କାଢ଼_real();
      unsigned wid = cp->ସଂଖ୍ୟା;

      vvp_vector4_t tmp(wid, r);
      thr->ଠେଲ_vec4(tmp);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * This implements the %deassign instruction. All we do is write a
 * long(1) to port-3 of the addressed net. This turns off an active
 * continuous assign activated by %cassign/v
 */
ତାର୍କିକ of_DEASSIGN(vthread_t, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      unsigned base  = cp->bit_idx[0];
      unsigned width = cp->bit_idx[1];

      vvp_signal_ମୂଲ୍ୟ*fil = dynamic_cast<vvp_signal_ମୂଲ୍ୟ*> (net->fil);
      ପ୍ରତିଜ୍ଞା(fil);
      vvp_fun_signal_vec*sig = dynamic_cast<vvp_fun_signal_vec*>(net->fun);
      ପ୍ରତିଜ୍ଞା(sig);

      ଯଦି (base >= fil->ମୂଲ୍ୟ_size()) ଫେରନ୍ତୁ ସତ୍ୟ;
      ଯଦି (base+width > fil->ମୂଲ୍ୟ_size()) width = fil->ମୂଲ୍ୟ_size() - base;

      ତାର୍କିକ full_sig = base == 0 && width == fil->ମୂଲ୍ୟ_size();

	// This is the net that is forcing me...
      ଯଦି (vvp_net_t*src = sig->cassign_link) {
	    ଯଦି (! full_sig) {
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
      ଯଦି (full_sig) {
	    sig->deassign();
      } ଅନ୍ୟଥା {
	    sig->deassign_pv(base, width);
      }

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_DEASSIGN_WR(vthread_t, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_real*sig = dynamic_cast<vvp_fun_signal_real*>(net->fun);
      ପ୍ରତିଜ୍ଞା(sig);

	// This is the net that is forcing me...
      ଯଦି (vvp_net_t*src = sig->cassign_link) {
	      // And this is the pointer to be removed.
	    vvp_net_ptr_t dst_ptr (net, 1);
	    src->unlink(dst_ptr);
	    sig->cassign_link = 0;
      }

      sig->deassign();

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %debug/thr
 */
ତାର୍କିକ of_DEBUG_THR(vthread_t thr, vvp_code_t cp)
{
      ଅଚଳ ଅକ୍ଷର*text = cp->text;
      thr->debug_dump(cerr, text);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * The delay takes two 32bit numbers to make up a 64bit time.
 *
 *   %delay <low>, <hig>
 */
ତାର୍କିକ of_DELAY(vthread_t thr, vvp_code_t cp)
{
      vvp_time64_t ତଳ = cp->bit_idx[0];
      vvp_time64_t hig = cp->bit_idx[1];

      vvp_time64_t delay = (hig << 32) | ତଳ;

      ଯଦି (delay == 0) schedule_inactive(thr);
      ଅନ୍ୟଥା schedule_vthread(thr, delay);
      ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ତାର୍କିକ of_DELAYX(vthread_t thr, vvp_code_t cp)
{
      vvp_time64_t delay;

      ପ୍ରତିଜ୍ଞା(cp->ସଂଖ୍ୟା < vthread_s::ଶବ୍ଦଗୁଡ଼ିକ_ଗଣନା);
      delay = thr->ଶବ୍ଦଗୁଡ଼ିକ[cp->ସଂଖ୍ୟା].w_uint;
      ଯଦି (delay == 0) schedule_inactive(thr);
      ଅନ୍ୟଥା schedule_vthread(thr, delay);
      ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ତାର୍କିକ of_DELETE_ELEM(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      int64_t idx_val = thr->ଶବ୍ଦଗୁଡ଼ିକ[3].w_int;
      ଯଦି (thr->flags[4] == BIT4_1) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with undefined index."
	         << endl;
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }
      ଯଦି (idx_val < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with negative index."
	         << endl;
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }
      size_t idx = idx_val;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      ପ୍ରତିଜ୍ଞା(obj);

      vvp_queue*queue = obj->get_object().peek<vvp_queue>();
      ଯଦି (queue == 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping delete(" << idx
	         << ") on empty queue." << endl;
      } ଅନ୍ୟଥା {
	    size_t size = queue->get_size();
	    ଯଦି (idx >= size) {
		  cerr << thr->get_fileline()
		       << "Warning: skipping out of range delete(" << idx
		       << ") on queue of size " << size << "." << endl;
	    } ଅନ୍ୟଥା {
		  queue->erase(idx);
	    }
      }

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/* %delete/obj <label>
 *
 * This operator works by assigning a nil to the target object. This
 * causes any value that might be there to be garbage collected, thus
 * deleting the object.
 */
ତାର୍କିକ of_DELETE_OBJ(vthread_t thr, vvp_code_t cp)
{
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_send_object(ptr, vvp_object_t(), thr->wt_context);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/* %reverse/obj <label>
 * Reverse element order of the queue or dynamic array object at this net.
 */
ତାର୍କିକ of_REVERSE_OBJ(vthread_t /*thr*/, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      ପ୍ରତିଜ୍ଞା(obj);

      vvp_object_t stor = obj->get_object();
      ଯଦି (stor.test_nil()) ଫେରନ୍ତୁ ସତ୍ୟ;

	/* Match other queue ops: peek queues before vvp_darray so concrete
	   queue types dispatch reverse_elems() correctly. */
      ଯଦି (vvp_queue* queue = stor.peek<vvp_queue>()) {
	    queue->reverse_elems();
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      ପ୍ରତିଜ୍ଞା(darray);
      darray->reverse_elems();

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/* %reverse/prop/obj <property-index>
 * Reverse queue/darray property storage inside the class object on stack top.
 */
ତାର୍କିକ of_REVERSE_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->ସଂଖ୍ୟା;

      vvp_object_t& ଶୀର୍ଷ = thr->peek_object();
      vvp_cobject*cobj = ଶୀର୍ଷ.peek<vvp_cobject>();
      ପ୍ରତିଜ୍ଞା(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      ଯଦି (arr_obj.test_nil()) ଫେରନ୍ତୁ ସତ୍ୟ;

      ଯଦି (vvp_queue* queue = arr_obj.peek<vvp_queue>()) {
	    queue->reverse_elems();
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      ପ୍ରତିଜ୍ଞା(darray);
      darray->reverse_elems();

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/* %sort/obj — ascending sort */
ତାର୍କିକ of_SORT_OBJ(vthread_t /*thr*/, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      ପ୍ରତିଜ୍ଞା(obj);

      vvp_object_t stor = obj->get_object();
      ଯଦି (stor.test_nil()) ଫେରନ୍ତୁ ସତ୍ୟ;

      ଯଦି (vvp_queue* queue = stor.peek<vvp_queue>()) {
	    queue->sort_elems(ସତ୍ୟ);
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      ପ୍ରତିଜ୍ଞା(darray);
      darray->sort_elems(ସତ୍ୟ);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/* %sort/prop/obj */
ତାର୍କିକ of_SORT_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->ସଂଖ୍ୟା;

      vvp_object_t& ଶୀର୍ଷ = thr->peek_object();
      vvp_cobject*cobj = ଶୀର୍ଷ.peek<vvp_cobject>();
      ପ୍ରତିଜ୍ଞା(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      ଯଦି (arr_obj.test_nil()) ଫେରନ୍ତୁ ସତ୍ୟ;

      ଯଦି (vvp_queue* queue = arr_obj.peek<vvp_queue>()) {
	    queue->sort_elems(ସତ୍ୟ);
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      ପ୍ରତିଜ୍ଞା(darray);
      darray->sort_elems(ସତ୍ୟ);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/* %rsort/obj — descending sort */
ତାର୍କିକ of_RSORT_OBJ(vthread_t /*thr*/, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      ପ୍ରତିଜ୍ଞା(obj);

      vvp_object_t stor = obj->get_object();
      ଯଦି (stor.test_nil()) ଫେରନ୍ତୁ ସତ୍ୟ;

      ଯଦି (vvp_queue* queue = stor.peek<vvp_queue>()) {
	    queue->sort_elems(ମିଥ୍ୟା);
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      ପ୍ରତିଜ୍ଞା(darray);
      darray->sort_elems(ମିଥ୍ୟା);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/* %rsort/prop/obj */
ତାର୍କିକ of_RSORT_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->ସଂଖ୍ୟା;

      vvp_object_t& ଶୀର୍ଷ = thr->peek_object();
      vvp_cobject*cobj = ଶୀର୍ଷ.peek<vvp_cobject>();
      ପ୍ରତିଜ୍ଞା(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      ଯଦି (arr_obj.test_nil()) ଫେରନ୍ତୁ ସତ୍ୟ;

      ଯଦି (vvp_queue* queue = arr_obj.peek<vvp_queue>()) {
	    queue->sort_elems(ମିଥ୍ୟା);
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      ପ୍ରତିଜ୍ଞା(darray);
      darray->sort_elems(ମିଥ୍ୟା);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/* %shuffle/obj */
ତାର୍କିକ of_SHUFFLE_OBJ(vthread_t /*thr*/, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      ପ୍ରତିଜ୍ଞା(obj);

      vvp_object_t stor = obj->get_object();
      ଯଦି (stor.test_nil()) ଫେରନ୍ତୁ ସତ୍ୟ;

      ଯଦି (vvp_queue* queue = stor.peek<vvp_queue>()) {
	    queue->shuffle_elems();
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      ପ୍ରତିଜ୍ଞା(darray);
      darray->shuffle_elems();

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/* %shuffle/prop/obj */
ତାର୍କିକ of_SHUFFLE_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->ସଂଖ୍ୟା;

      vvp_object_t& ଶୀର୍ଷ = thr->peek_object();
      vvp_cobject*cobj = ଶୀର୍ଷ.peek<vvp_cobject>();
      ପ୍ରତିଜ୍ଞା(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      ଯଦି (arr_obj.test_nil()) ଫେରନ୍ତୁ ସତ୍ୟ;

      ଯଦି (vvp_queue* queue = arr_obj.peek<vvp_queue>()) {
	    queue->shuffle_elems();
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      ପ୍ରତିଜ୍ଞା(darray);
      darray->shuffle_elems();

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/* %delete/tail <label>, idx
 *
 * Remove all elements after the one specified.
 */
ତାର୍କିକ of_DELETE_TAIL(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      ପ୍ରତିଜ୍ଞା(obj);

      vvp_queue*queue = obj->get_object().peek<vvp_queue>();
      ପ୍ରତିଜ୍ଞା(queue);

      unsigned idx = thr->ଶବ୍ଦଗୁଡ଼ିକ[cp->bit_idx[0]].w_int;
      queue->erase_tail(idx);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ସ୍ଥିର ତାର୍କିକ do_disable(vthread_t thr, vthread_t match)
{
      ତାର୍କିକ flag = ମିଥ୍ୟା;

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
      ଯେତେବେଳେ (! thr->children.empty()) {

	    vthread_t tmp = *(thr->children.begin());
	    ପ୍ରତିଜ୍ଞା(tmp);
	    ପ୍ରତିଜ୍ଞା(tmp->parent == thr);
	    thr->i_am_joining = 0;
	    ଯଦି (do_disable(tmp, match))
		  flag = ସତ୍ୟ;

	    vthread_reap(tmp);
      }

      vthread_t parent = thr->parent;
      ଯଦି (parent && parent->i_am_joining) {
	      // If a parent is waiting in a %join, wake it up. Note
	      // that it is possible to be waiting in a %join yet
	      // already scheduled if multiple child threads are
	      // ending. So check if the thread is already scheduled
	      // before scheduling it again.
	    parent->i_am_joining = 0;
	    ଯଦି (! parent->i_have_ended)
		  schedule_vthread(parent, 0, ସତ୍ୟ);

	    do_join(parent, thr);

      } ଅନ୍ୟଥା ଯଦି (parent) {
	      /* If the parent is yet to %join me, let its %join
		 do the reaping. */
	      //assert(tmp->is_scheduled == 0);

      } ଅନ୍ୟଥା {
	      /* No parent at all. Goodbye. */
	    vthread_reap(thr);
      }

      ଫେରନ୍ତୁ flag || (thr == match);
}

/*
 * Implement the %disable instruction by scanning the target scope for
 * all the target threads. Kill the target threads and wake up a
 * parent that is attempting a %join.
 */
ତାର୍କିକ of_DISABLE(vthread_t thr, vvp_code_t cp)
{
      __vpiScope*scope = static_cast<__vpiScope*>(cp->handle);

      ତାର୍କିକ disabled_myself_flag = ମିଥ୍ୟା;

      ଯେତେବେଳେ (! scope->threads.empty()) {
	    set<vthread_t>::iterator cur = scope->threads.begin();

	    ଯଦି (do_disable(*cur, thr))
		  disabled_myself_flag = ସତ୍ୟ;
      }

      ଫେରନ୍ତୁ ! disabled_myself_flag;
}

/*
 * Similar to `of_DISABLE`. But will only disable a single thread of the
 * specified scope. The disabled thread will be the thread closest to the
 * current thread in thread hierarchy. This can either be the current thread,
 * either the thread itself or one of its parents.
 * This is used for SystemVerilog flow control instructions like `return`,
 * `continue` and `break`.
 */

ତାର୍କିକ of_DISABLE_FLOW(vthread_t thr, vvp_code_t cp)
{
      ଅଚଳ __vpiScope*scope = static_cast<__vpiScope*>(cp->handle);
      vthread_t cur = thr;

      ଯେତେବେଳେ (cur && cur->parent_scope != scope)
	    cur = cur->parent;

      ପ୍ରତିଜ୍ଞା(cur);
      ଯଦି (cur)
	    ଫେରନ୍ତୁ !do_disable(cur, thr);

      ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

/*
 * Implement the %disable/fork (SystemVerilog) instruction by disabling
 * all the detached children of the given thread.
 */
ତାର୍କିକ of_DISABLE_FORK(vthread_t thr, vvp_code_t)
{
	/* If a %disable/fork is being executed then the parent thread
	 * cannot be waiting in a join. */
      ପ୍ରତିଜ୍ଞା(! thr->i_am_joining);

	/* There should be no active children to disable. */
      ପ୍ରତିଜ୍ଞା(thr->children.empty());

	/* Disable any detached children. */
      ଯେତେବେଳେ (! thr->detached_children.empty()) {
	    vthread_t child = *(thr->detached_children.begin());
	    ପ୍ରତିଜ୍ଞା(child);
	    ପ୍ରତିଜ୍ଞା(child->parent == thr);
	      /* Disabling the children can never match the parent thread. */
	    ତାର୍କିକ res = do_disable(child, thr);
	    ପ୍ରତିଜ୍ଞା(! res);
	    vthread_reap(child);
      }

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * This function divides a 2-word number {high, a} by a 1-word
 * number. Assume that high < b.
 */
ସ୍ଥିର unsigned long divide2words(unsigned long a, unsigned long b,
				  unsigned long ଉପର)
{
      unsigned long result = 0;
      ଯେତେବେଳେ (ଉପର > 0) {
	    unsigned long tmp_result = ULONG_MAX / b;
	    unsigned long remain = ULONG_MAX % b;

	    remain += 1;
	    ଯଦି (remain >= b) {
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
	    result += tmp_result * ଉପର;

	      // The new iteration starts with high*remain + a.
	    remain = multiply_with_carry(ଉପର, remain, ଉପର);
	    a += remain;
            ଯଦି(a < remain)
              ଉପର += 1;

	      // Now result*b + {high,a} == the input {high,a}. It is
	      // possible that the new high >= 1. If so, it will
	      // certainly be less than high from the previous
	      // iteration. Do another iteration and it will shrink,
	      // eventually to 0.
      }

	// high is now 0, so a is the remaining remainder, so we can
	// finish off the integer divide with a simple a/b.

      ଫେରନ୍ତୁ result + a/b;
}

ସ୍ଥିର unsigned long* divide_bits(unsigned long*ap, ଅଚଳ unsigned long*bp, unsigned wid)
{
	// Do all our work a cpu-word at a time. The "words" variable
	// is the number of words of the wid.
      unsigned ଶବ୍ଦଗୁଡ଼ିକ = (wid+CPU_WORD_BITS-1) / CPU_WORD_BITS;

      unsigned btop = ଶବ୍ଦଗୁଡ଼ିକ-1;
      ଯେତେବେଳେ (btop > 0 && bp[btop] == 0)
	    btop -= 1;

	// Detect divide by 0, and exit.
      ଯଦି (btop==0 && bp[0]==0)
	    ଫେରନ୍ତୁ 0;

	// The result array will eventually accumulate the result. The
	// diff array is a difference that we use in the intermediate.
      unsigned long*diff  = ନୂତନ unsigned long[ଶବ୍ଦଗୁଡ଼ିକ];
      unsigned long*result= ନୂତନ unsigned long[ଶବ୍ଦଗୁଡ଼ିକ];
      ପାଇଁ (unsigned idx = 0 ; idx < ଶବ୍ଦଗୁଡ଼ିକ ; idx += 1)
	    result[idx] = 0;

      ପାଇଁ (unsigned cur = ଶବ୍ଦଗୁଡ଼ିକ-btop ; cur > 0 ; cur -= 1) {
	    unsigned cur_ptr = cur-1;
	    unsigned long cur_res;
	    ଯଦି (ap[cur_ptr+btop] >= bp[btop]) {
		  unsigned long ଉପର = 0;
		  ଯଦି (cur_ptr+btop+1 < ଶବ୍ଦଗୁଡ଼ିକ)
			ଉପର = ap[cur_ptr+btop+1];
		  cur_res = divide2words(ap[cur_ptr+btop], bp[btop], ଉପର);

	    } ଅନ୍ୟଥା ଯଦି (cur_ptr+btop+1 >= ଶବ୍ଦଗୁଡ଼ିକ) {
		  ଜାରି;

	    } ଅନ୍ୟଥା ଯଦି (ap[cur_ptr+btop+1] == 0) {
		  ଜାରି;

	    } ଅନ୍ୟଥା {
		  cur_res = divide2words(ap[cur_ptr+btop], bp[btop],
					 ap[cur_ptr+btop+1]);
	    }

	      // cur_res is a guesstimate of the result this far. It
	      // may be 1 too big. (But it will also be >0) Try it,
	      // and if the difference comes out negative, then adjust.

	      // diff = (bp * cur_res)  << cur_ptr;
	    multiply_array_imm(diff+cur_ptr, bp, ଶବ୍ଦଗୁଡ଼ିକ-cur_ptr, cur_res);
	      // ap -= diff
	    unsigned long carry = 1;
	    ପାଇଁ (unsigned idx = cur_ptr ; idx < ଶବ୍ଦଗୁଡ଼ିକ ; idx += 1)
		  ap[idx] = add_with_carry(ap[idx], ~diff[idx], carry);

	      // ap has the diff subtracted out of it. If cur_res was
	      // too large, then ap will turn negative. (We easily
	      // tell that ap turned negative by looking at
	      // carry&1. If it is 0, then it is *negative*.) In that
	      // case, we know that cur_res was too large by 1. Correct by
	      // adding 1b back in and reducing cur_res.
	    ଯଦି ((carry&1) == 0) {
		    // Keep adding b back in until the remainder
		    // becomes positive again.
		  କର {
			cur_res -= 1;
			carry = 0;
			ପାଇଁ (unsigned idx = cur_ptr ; idx < ଶବ୍ଦଗୁଡ଼ିକ ; idx += 1)
			      ap[idx] = add_with_carry(ap[idx], bp[idx-cur_ptr], carry);
		  } ଯେତେବେଳେ (carry == 0);
	    }

	    result[cur_ptr] = cur_res;
      }

	// Now ap contains the remainder and result contains the
	// desired result. We should find that:
	//  input-a = bp * result + ap;

      delete[]diff;
      ଫେରନ୍ତୁ result;
}

/*
 * %div
 */
ତାର୍କିକ of_DIV(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valb = thr->କାଢ଼_vec4();
      vvp_vector4_t vala = thr->କାଢ଼_vec4();

      ପ୍ରତିଜ୍ଞା(vala.size()== valb.size());
      unsigned wid = vala.size();

      unsigned long*ap = vala.subarray(0, wid);
      ଯଦି (ap == 0) {
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->ଠେଲ_vec4(tmp);
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }

      unsigned long*bp = valb.subarray(0, wid);
      ଯଦି (bp == 0) {
	    delete[]ap;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->ଠେଲ_vec4(tmp);
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }

	// If the value fits in a single CPU word, then do it the easy way.
      ଯଦି (wid <= CPU_WORD_BITS) {
	    ଯଦି (bp[0] == 0) {
		  vvp_vector4_t tmp(wid, BIT4_X);
		  thr->ଠେଲ_vec4(tmp);
	    } ଅନ୍ୟଥା {
		  ap[0] /= bp[0];
		  vala.setarray(0, wid, ap);
		  thr->ଠେଲ_vec4(vala);
	    }
	    delete[]ap;
	    delete[]bp;
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }

      unsigned long*result = divide_bits(ap, bp, wid);
      ଯଦି (result == 0) {
	    delete[]ap;
	    delete[]bp;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->ଠେଲ_vec4(tmp);
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }

	// Now ap contains the remainder and result contains the
	// desired result. We should find that:
	//  input-a = bp * result + ap;

      vala.setarray(0, wid, result);
      thr->ଠେଲ_vec4(vala);
      delete[]ap;
      delete[]bp;
      delete[]result;

      ଫେରନ୍ତୁ ସତ୍ୟ;
}


ସ୍ଥିର ଶୂନ୍ୟ negate_ଶବ୍ଦଗୁଡ଼ିକ(unsigned long*val, unsigned ଶବ୍ଦଗୁଡ଼ିକ)
{
      unsigned long carry = 1;
      ପାଇଁ (unsigned idx = 0 ; idx < ଶବ୍ଦଗୁଡ଼ିକ ; idx += 1)
	    val[idx] = add_with_carry(0, ~val[idx], carry);
}

/*
 * %div/s
 */
ତାର୍କିକ of_DIV_S(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valb = thr->କାଢ଼_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      ପ୍ରତିଜ୍ଞା(vala.size()== valb.size());
      unsigned wid = vala.size();
      unsigned ଶବ୍ଦଗୁଡ଼ିକ = (wid + CPU_WORD_BITS - 1) / CPU_WORD_BITS;

	// Get the values, left in right, in binary form. If there is
	// a problem with either (caused by an X or Z bit) then we
	// know right away that the entire result is X.
      unsigned long*ap = vala.subarray(0, wid);
      ଯଦି (ap == 0) {
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }

      unsigned long*bp = valb.subarray(0, wid);
      ଯଦି (bp == 0) {
	    delete[]ap;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }

	// Sign extend the bits in the array to fill out the array.
      unsigned long sign_mask = 0;
      ଯଦି (unsigned long sign_bits = (ଶବ୍ଦଗୁଡ଼ିକ*CPU_WORD_BITS) - wid) {
	    sign_mask = -1UL << (CPU_WORD_BITS-sign_bits);
	    ଯଦି (ap[ଶବ୍ଦଗୁଡ଼ିକ-1] & (sign_mask>>1))
		  ap[ଶବ୍ଦଗୁଡ଼ିକ-1] |= sign_mask;
	    ଯଦି (bp[ଶବ୍ଦଗୁଡ଼ିକ-1] & (sign_mask>>1))
		  bp[ଶବ୍ଦଗୁଡ଼ିକ-1] |= sign_mask;
      }

	// If the value fits in a single word, then use the native divide.
      ଯଦି (wid <= CPU_WORD_BITS) {
	    ଯଦି (bp[0] == 0) {
		  vvp_vector4_t tmp(wid, BIT4_X);
		  vala = tmp;
	    } ଅନ୍ୟଥା ଯଦି (((long)ap[0] == LONG_MIN) && ((long)bp[0] == -1)) {
		  vvp_vector4_t tmp(wid, BIT4_0);
		  tmp.set_bit(wid-1, BIT4_1);
		  vala = tmp;
	    } ଅନ୍ୟଥା {
		  long tmpa = (long) ap[0];
		  long tmpb = (long) bp[0];
		  long res = tmpa / tmpb;
		  ap[0] = ((unsigned long)res) & ~sign_mask;
		  vala.setarray(0, wid, ap);
	    }
	    delete[]ap;
	    delete[]bp;
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }

	// We need to the actual division to positive integers. Make
	// them positive here, and remember the negations.
      ତାର୍କିକ negate_flag = ମିଥ୍ୟା;
      ଯଦି ( ((long) ap[ଶବ୍ଦଗୁଡ଼ିକ-1]) < 0 ) {
	    negate_flag = ସତ୍ୟ;
	    negate_ଶବ୍ଦଗୁଡ଼ିକ(ap, ଶବ୍ଦଗୁଡ଼ିକ);
      }
      ଯଦି ( ((long) bp[ଶବ୍ଦଗୁଡ଼ିକ-1]) < 0 ) {
	    negate_flag ^= ସତ୍ୟ;
	    negate_ଶବ୍ଦଗୁଡ଼ିକ(bp, ଶବ୍ଦଗୁଡ଼ିକ);
      }

      unsigned long*result = divide_bits(ap, bp, wid);
      ଯଦି (result == 0) {
	    delete[]ap;
	    delete[]bp;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }

      ଯଦି (negate_flag) {
	    negate_ଶବ୍ଦଗୁଡ଼ିକ(result, ଶବ୍ଦଗୁଡ଼ିକ);
      }

      result[ଶବ୍ଦଗୁଡ଼ିକ-1] &= ~sign_mask;

      vala.setarray(0, wid, result);
      delete[]ap;
      delete[]bp;
      delete[]result;
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_DIV_WR(vthread_t thr, vvp_code_t)
{
      ଦ୍ୱିଗୁଣ r = thr->କାଢ଼_real();
      ଦ୍ୱିଗୁଣ l = thr->କାଢ଼_real();
      thr->ଠେଲ_real(l / r);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %dup/obj
 * %dup/real
 * %dup/vec4
 *
 * Push a duplicate of the object on the appropriate stack.
 */
ତାର୍କିକ of_DUP_OBJ(vthread_t thr, vvp_code_t)
{
      vvp_object_t src = thr->peek_object();

        // If it is null push a new null object
      ଯଦି (src.test_nil())
	    thr->ଠେଲ_object(vvp_object_t());
      ଅନ୍ୟଥା
	    thr->ଠେଲ_object(src.duplicate());

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_DUP_REAL(vthread_t thr, vvp_code_t)
{
      thr->ଠେଲ_real(thr->peek_real(0));
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_DUP_VEC4(vthread_t thr, vvp_code_t)
{
      thr->ଠେଲ_vec4(thr->peek_vec4(0));
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * This terminates the current thread. If there is a parent who is
 * waiting for me to die, then I schedule it. At any rate, I mark
 * myself as a zombie by setting my pc to 0.
 */
ତାର୍କିକ of_END(vthread_t thr, vvp_code_t)
{
      ପ୍ରତିଜ୍ଞା(! thr->waiting_for_event);
      thr->i_have_ended = 1;
      thr->pc = codespace_null();

	/* Fully detach any detached children. */
      ଯେତେବେଳେ (! thr->detached_children.empty()) {
	    vthread_t child = *(thr->detached_children.begin());
	    ପ୍ରତିଜ୍ଞା(child);
	    ପ୍ରତିଜ୍ଞା(child->parent == thr);
	    ପ୍ରତିଜ୍ଞା(child->i_am_detached);
	    child->parent = 0;
	    child->i_am_detached = 0;
	    thr->detached_children.erase(thr->detached_children.begin());
      }

	/* It is an error to still have active children running at this
	 * point in time. They should have all been detached or joined. */
      ପ୍ରତିଜ୍ଞା(thr->children.empty());

	/* If I have a parent who is waiting for me, then mark that I
	   have ended, and schedule that parent. Also, finish the
	   %join for the parent. */
      ଯଦି (!thr->i_am_detached && thr->parent && thr->parent->i_am_joining) {
	    vthread_t tmp = thr->parent;
	    ପ୍ରତିଜ୍ଞା(! thr->i_am_detached);

	    tmp->i_am_joining = 0;
	    schedule_vthread(tmp, 0, ସତ୍ୟ);
	    do_join(tmp, thr);
	    ଫେରନ୍ତୁ ମିଥ୍ୟା;
      }

	/* If this thread is not fully detached then remove it from the
	 * parents detached_children set and reap it. */
      ଯଦି (thr->i_am_detached) {
	    vthread_t tmp = thr->parent;
	    ପ୍ରତିଜ୍ଞା(tmp);
	    size_t res = tmp->detached_children.erase(thr);
	    ପ୍ରତିଜ୍ଞା(res == 1);
	      /* If the parent is waiting for the detached children to
	       * finish then the last detached child needs to tell the
	       * parent to wake up when it is finished. */
	    ଯଦି (tmp->i_am_waiting && tmp->detached_children.empty()) {
		  tmp->i_am_waiting = 0;
		  schedule_vthread(tmp, 0, ସତ୍ୟ);
	    }
	      /* Fully detach this thread so it will be reaped below. */
	    thr->i_am_detached = 0;
	    thr->parent = 0;
      }

	/* If I have no parent, then no one can %join me and there is
	 * no reason to stick around. This can happen, for example if
	 * I am an ``initial'' thread. */
      ଯଦି (thr->parent == 0) {
	    vthread_reap(thr);
	    ଫେରନ୍ତୁ ମିଥ୍ୟା;
      }

	/* If I make it this far, then I have a parent who may wish
	   to %join me. Remain a zombie so that it can. */

      ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

/*
 * %event <var-label>
 */
ତାର୍କିକ of_EVENT(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_vector4_t tmp (1, BIT4_X);
      vvp_send_vec4(ptr, tmp, thr->wt_context);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %event/nb <var-label>, <delay>
 */
ତାର୍କିକ of_EVENT_NB(vthread_t thr, vvp_code_t cp)
{
      vvp_time64_t delay;

      delay = thr->ଶବ୍ଦଗୁଡ଼ିକ[cp->bit_idx[0]].w_uint;
      schedule_propagate_event(cp->net, delay);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_EVCTL(vthread_t thr, vvp_code_t cp)
{
      ପ୍ରତିଜ୍ଞା(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      thr->ecount = thr->ଶବ୍ଦଗୁଡ଼ିକ[cp->bit_idx[0]].w_uint;
      ଫେରନ୍ତୁ ସତ୍ୟ;
}
ତାର୍କିକ of_EVCTLC(vthread_t thr, vvp_code_t)
{
      thr->event = 0;
      thr->ecount = 0;
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_EVCTLI(vthread_t thr, vvp_code_t cp)
{
      ପ୍ରତିଜ୍ଞା(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      thr->ecount = cp->bit_idx[0];
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_EVCTLS(vthread_t thr, vvp_code_t cp)
{
      ପ୍ରତିଜ୍ଞା(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      int64_t val = thr->ଶବ୍ଦଗୁଡ଼ିକ[cp->bit_idx[0]].w_int;
      ଯଦି (val < 0) val = 0;
      thr->ecount = val;
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_FLAG_GET_VEC4(vthread_t thr, vvp_code_t cp)
{
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା flag = cp->ସଂଖ୍ୟା;
      ପ୍ରତିଜ୍ଞା(flag < vthread_s::FLAGS_ଗଣନା);

      vvp_vector4_t val (1, thr->flags[flag]);
      thr->ଠେଲ_vec4(val);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %flag_inv <flag1>
 */
ତାର୍କିକ of_FLAG_INV(vthread_t thr, vvp_code_t cp)
{
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା flag1 = cp->bit_idx[0];

      thr->flags[flag1] = ~ thr->flags[flag1];
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %flag_mov <flag1>, <flag2>
 */
ତାର୍କିକ of_FLAG_MOV(vthread_t thr, vvp_code_t cp)
{
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା flag1 = cp->bit_idx[0];
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା flag2 = cp->bit_idx[1];

      thr->flags[flag1] = thr->flags[flag2];
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %flag_or <flag1>, <flag2>
 */
ତାର୍କିକ of_FLAG_OR(vthread_t thr, vvp_code_t cp)
{
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା flag1 = cp->bit_idx[0];
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା flag2 = cp->bit_idx[1];

      thr->flags[flag1] = thr->flags[flag1] | thr->flags[flag2];
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_FLAG_SET_IMM(vthread_t thr, vvp_code_t cp)
{
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା flag = cp->ସଂଖ୍ୟା;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା vali = cp->bit_idx[0];

      ପ୍ରତିଜ୍ଞା(flag < vthread_s::FLAGS_ଗଣନା);
      ପ୍ରତିଜ୍ଞା(vali >= 0 && vali < 4);

      ସ୍ଥିର ଅଚଳ vvp_bit4_t map_bit[4] = {BIT4_0, BIT4_1, BIT4_Z, BIT4_X};
      thr->flags[flag] = map_bit[vali];
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_FLAG_SET_VEC4(vthread_t thr, vvp_code_t cp)
{
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା flag = cp->ସଂଖ୍ୟା;
      ପ୍ରତିଜ୍ଞା(flag < vthread_s::FLAGS_ଗଣନା);

      ଅଚଳ vvp_vector4_t&val = thr->peek_vec4();
      thr->flags[flag] = val.value(0);
      thr->କାଢ଼_vec4(1);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * the %force/link instruction connects a source node to a
 * destination node. The destination node must be a signal, as it is
 * marked with the source of the force so that it may later be
 * unlinked without specifically knowing the source that this
 * instruction used.
 */
ତାର୍କିକ of_FORCE_LINK(vthread_t, vvp_code_t cp)
{
      vvp_net_t*dst = cp->net;
      vvp_net_t*src = cp->net2;

      ପ୍ରତିଜ୍ଞା(dst->fil);
      dst->fil->force_link(dst, src);

      ଫେରନ୍ତୁ ସତ୍ୟ;
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
ତାର୍କିକ of_FORCE_VEC4(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_vector4_t value = thr->କାଢ଼_vec4();

	/* Send the force value to the filter on the node. */

      ପ୍ରତିଜ୍ଞା(net->fil);
      ଯଦି (value.size() != net->fil->filter_size())
	    value = coerce_to_width(value, net->fil->filter_size());

      net->force_vec4(value, vvp_vector2_t(vvp_vector2_t::FILL1, net->fil->filter_size()));

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %force/vec4/off <net>, <off>
 */
ତାର୍କିକ of_FORCE_VEC4_OFF(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      unsigned base_idx = cp->bit_idx[0];
      long base = thr->ଶବ୍ଦଗୁଡ଼ିକ[base_idx].w_int;
      vvp_vector4_t value = thr->କାଢ଼_vec4();
      unsigned wid = value.size();

      ପ୍ରତିଜ୍ଞା(net->fil);

      ଯଦି (thr->flags[4] == BIT4_1)
	    ଫେରନ୍ତୁ ସତ୍ୟ;

	// This is the width of the target vector.
      unsigned use_size = net->fil->filter_size();

      ଯଦି (base >= (long)use_size)
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      ଯଦି (base < -(long)use_size)
	    ଫେରନ୍ତୁ ସତ୍ୟ;

      ଯଦି ((base + wid) > use_size)
	    wid = use_size - base;

	// Make a mask of which bits are to be forced, 0 for unforced
	// bits and 1 for forced bits.
      vvp_vector2_t mask (vvp_vector2_t::FILL0, use_size);
      ପାଇଁ (unsigned idx = 0 ; idx < wid ; idx += 1)
	    mask.set_bit(base+idx, 1);

      vvp_vector4_t tmp (use_size, BIT4_Z);

	// vvp_net_t::force_vec4 propagates all the bits of the
	// forced vector value, regardless of the mask. This
	// ensures the unforced bits retain their current value.
      vvp_signal_ମୂଲ୍ୟ*sig = dynamic_cast<vvp_signal_ମୂଲ୍ୟ*>(net->fil);
      ପ୍ରତିଜ୍ଞା(sig);
      sig->vec4_ମୂଲ୍ୟ(tmp);

      tmp.set_vec(base, value);

      net->force_vec4(tmp, mask);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %force/vec4/off/d <net>, <off>, <del>
 */
ତାର୍କିକ of_FORCE_VEC4_OFF_D(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      unsigned base_idx = cp->bit_idx[0];
      long base = thr->ଶବ୍ଦଗୁଡ଼ିକ[base_idx].w_int;

      unsigned delay_idx = cp->bit_idx[1];
      vvp_time64_t delay = thr->ଶବ୍ଦଗୁଡ଼ିକ[delay_idx].w_uint;

      vvp_vector4_t value = thr->କାଢ଼_vec4();

      ପ୍ରତିଜ୍ଞା(net->fil);

      ଯଦି (thr->flags[4] == BIT4_1)
	    ଫେରନ୍ତୁ ସତ୍ୟ;

	// This is the width of the target vector.
      unsigned use_size = net->fil->filter_size();

      ଯଦି (base >= (long)use_size)
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      ଯଦି (base < -(long)use_size)
	    ଫେରନ୍ତୁ ସତ୍ୟ;

      schedule_force_vector(net, base, use_size, value, delay);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_FORCE_WR(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net  = cp->net;
      ଦ୍ୱିଗୁଣ value = thr->କାଢ଼_real();

      net->force_real(value, vvp_vector2_t(vvp_vector2_t::FILL1, 1));

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * The %fork instruction causes a new child to be created and pushed
 * in front of any existing child. This causes the new child to be
 * added to the list of children, and for me to be the parent of the
 * new child.
 */
ତାର୍କିକ of_FORK(vthread_t thr, vvp_code_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);

      ଯଦି (cp->scope->is_automatic()) {
              /* The context allocated for this child is the top entry
                 on the write context stack. */
            child->wt_context = thr->wt_context;
            child->rd_context = thr->wt_context;
      }

      child->parent = thr;
      thr->children.insert(child);

      ଯଦି (thr->i_am_in_function) {
	    child->is_scheduled = 1;
	    child->i_am_in_function = 1;
	    vthread_run(child);
	    running_thread = thr;
      } ଅନ୍ୟଥା {
	    schedule_vthread(child, 0, ସତ୍ୟ);
      }
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_FREE(vthread_t thr, vvp_code_t cp)
{
        /* Pop the child context from the read context stack. */
      vvp_context_t child_context = thr->rd_context;
      thr->rd_context = vvp_get_stacked_context(child_context);

        /* Free the context. */
      vthread_free_context(child_context, cp->scope);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %inv
 *
 * Logically, this pops a value, inverts is (Verilog style, with Z and
 * X converted to X) and pushes the result. We can more efficiently
 * just to the invert in place.
 */
ତାର୍କିକ of_INV(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val.invert();
      ଫେରନ୍ତୁ ସତ୍ୟ;
}


/*
 * Index registers, arithmetic.
 */

ସ୍ଥିର ଧାଡ଼ିଭିତରେ int64_t get_as_64_bit(uint32_t ତଳ_32, uint32_t ଉପର_32)
{
      int64_t ତଳ = ତଳ_32;
      int64_t res = ଉପର_32;

      res <<= 32;
      res |= ତଳ;
      ଫେରନ୍ତୁ res;
}

ତାର୍କିକ of_IX_ADD(vthread_t thr, vvp_code_t cp)
{
      thr->ଶବ୍ଦଗୁଡ଼ିକ[cp->ସଂଖ୍ୟା].w_int += get_as_64_bit(cp->bit_idx[0],
                                                    cp->bit_idx[1]);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_IX_SUB(vthread_t thr, vvp_code_t cp)
{
      thr->ଶବ୍ଦଗୁଡ଼ିକ[cp->ସଂଖ୍ୟା].w_int -= get_as_64_bit(cp->bit_idx[0],
                                                    cp->bit_idx[1]);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_IX_MUL(vthread_t thr, vvp_code_t cp)
{
      thr->ଶବ୍ଦଗୁଡ଼ିକ[cp->ସଂଖ୍ୟା].w_int *= get_as_64_bit(cp->bit_idx[0],
                                                    cp->bit_idx[1]);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_IX_LOAD(vthread_t thr, vvp_code_t cp)
{
      thr->ଶବ୍ଦଗୁଡ଼ିକ[cp->ସଂଖ୍ୟା].w_int = get_as_64_bit(cp->bit_idx[0],
                                                   cp->bit_idx[1]);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_IX_MOV(vthread_t thr, vvp_code_t cp)
{
      thr->ଶବ୍ଦଗୁଡ଼ିକ[cp->bit_idx[0]].w_int = thr->ଶବ୍ଦଗୁଡ଼ିକ[cp->bit_idx[1]].w_int;
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_IX_GETV(vthread_t thr, vvp_code_t cp)
{
      unsigned index = cp->bit_idx[0];
      vvp_net_t*net = cp->net;

      vvp_signal_ମୂଲ୍ୟ*sig = dynamic_cast<vvp_signal_ମୂଲ୍ୟ*>(net->fil);
      ଯଦି (sig == 0) {
	    ପ୍ରତିଜ୍ଞା(net->fil);
	    cerr << thr->get_fileline()
	         << "%%ix/getv error: Net arg not a vector signal? "
		 << typeid(*net->fil).name() << endl;
      }
      ପ୍ରତିଜ୍ଞା(sig);

      vvp_vector4_t vec;
      sig->vec4_ମୂଲ୍ୟ(vec);
      ତାର୍କିକ overflow_flag;
      uint64_t val = 0;
      ତାର୍କିକ known_flag = vector4_to_ମୂଲ୍ୟ(vec, overflow_flag, val);

      ଯଦି (known_flag)
	    thr->ଶବ୍ଦଗୁଡ଼ିକ[index].w_uint = val;
      ଅନ୍ୟଥା
	    thr->ଶବ୍ଦଗୁଡ଼ିକ[index].w_uint = 0;

	/* Set bit 4 as a flag if the input is unknown. */
      thr->flags[4] = known_flag ? (overflow_flag ? BIT4_X : BIT4_0) : BIT4_1;

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_IX_GETV_S(vthread_t thr, vvp_code_t cp)
{
      unsigned index = cp->bit_idx[0];
      vvp_net_t*net = cp->net;

      vvp_signal_ମୂଲ୍ୟ*sig = dynamic_cast<vvp_signal_ମୂଲ୍ୟ*>(net->fil);
      ଯଦି (sig == 0) {
	    ପ୍ରତିଜ୍ଞା(net->fil);
	    cerr << thr->get_fileline()
	         << "%%ix/getv/s error: Net arg not a vector signal? "
		 << "fun=" << typeid(*net->fil).name()
		 << ", fil=" << (net->fil? typeid(*net->fil).name() : "<>")
		 << endl;
      }
      ପ୍ରତିଜ୍ଞା(sig);

      vvp_vector4_t vec;
      sig->vec4_ମୂଲ୍ୟ(vec);
      int64_t val;
      ତାର୍କିକ known_flag = vector4_to_ମୂଲ୍ୟ(vec, val, ସତ୍ୟ, ସତ୍ୟ);

      ଯଦି (known_flag)
	    thr->ଶବ୍ଦଗୁଡ଼ିକ[index].w_int = val;
      ଅନ୍ୟଥା
	    thr->ଶବ୍ଦଗୁଡ଼ିକ[index].w_int = 0;

	/* Set bit 4 as a flag if the input is unknown. */
      thr->flags[4] = known_flag? BIT4_0 : BIT4_1;

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ସ୍ଥିର uint64_t vec4_to_index(vthread_t thr, ତାର୍କିକ signed_flag)
{
	// Get all the information we need about the vec4 vector, then
	// pop it away. We only need the bool bits and the length.
      ଅଚଳ vvp_vector4_t&val = thr->peek_vec4();
      unsigned val_size = val.size();
      unsigned long*bits = val.subarray(0, val_size, ମିଥ୍ୟା);
      thr->କାଢ଼_vec4(1);

	// If there are X/Z bits, then the subarray will give us a nil
	// pointer. Set a flag to indicate the error, and give up.
      ଯଦି (bits == 0) {
	    thr->flags[4] = BIT4_1;
	    ଫେରନ୍ତୁ 0;
      }

      uint64_t v = 0;
      thr->flags[4] = BIT4_0;

      ପ୍ରତିଜ୍ଞା(ଆକାର(bits[0]) <= ଆକାର(v));

      v = 0;
      ପାଇଁ (unsigned idx = 0 ; idx < val_size ; idx += 8*ଆକାର(bits[0])) {
	    uint64_t tmp = bits[idx/8/ଆକାର(bits[0])];
	    ଯଦି (idx < 8*ଆକାର(v)) {
		  v |= tmp << idx;
	    } ଅନ୍ୟଥା {
		  ତାର୍କିକ overflow = signed_flag && (v >> 63) ? ~tmp != 0 : tmp != 0;
		  ଯଦି (overflow) {
			thr->flags[4] = BIT4_X;
			ଭାଙ୍ଗ;
		  }
	    }
      }

	// Set the high bits that are not necessarily filled in by the
	// subarray function.
      ଯଦି (val_size < 8*ଆକାର(v)) {
	    ଯଦି (signed_flag && (v & (static_cast<uint64_t>(1)<<(val_size-1)))) {
		    // Propagate the sign bit...
		  v |= (~static_cast<uint64_t>(0)) << val_size;

	    } ଅନ୍ୟଥା {
		    // Fill with zeros.
		  v &= ~((~static_cast<uint64_t>(0)) << val_size);
	    }

      }

      delete[]bits;
      ଫେରନ୍ତୁ v;
}

/*
 * %ix/vec4 <idx>
 */
ତାର୍କିକ of_IX_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned use_idx = cp->ସଂଖ୍ୟା;
      thr->ଶବ୍ଦଗୁଡ଼ିକ[use_idx].w_uint = vec4_to_index(thr, ମିଥ୍ୟା);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %ix/vec4/s <idx>
 */
ତାର୍କିକ of_IX_VEC4_S(vthread_t thr, vvp_code_t cp)
{
      unsigned use_idx = cp->ସଂଖ୍ୟା;
      thr->ଶବ୍ଦଗୁଡ଼ିକ[use_idx].w_uint = vec4_to_index(thr, ସତ୍ୟ);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * The various JMP instruction work simply by pulling the new program
 * counter from the instruction and resuming. If the jump is
 * conditional, then test the bit for the expected value first.
 */
ତାର୍କିକ of_JMP(vthread_t thr, vvp_code_t cp)
{
      thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      ଯଦି (schedule_stopped()) {
	    schedule_vthread(thr, 0, ମିଥ୍ୟା);
	    ଫେରନ୍ତୁ ମିଥ୍ୟା;
      }

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %jmp/0 <pc>, <flag>
 */
ତାର୍କିକ of_JMP0(vthread_t thr, vvp_code_t cp)
{
      ଯଦି (thr->flags[cp->bit_idx[0]] == BIT4_0)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      ଯଦି (schedule_stopped()) {
	    schedule_vthread(thr, 0, ମିଥ୍ୟା);
	    ଫେରନ୍ତୁ ମିଥ୍ୟା;
      }

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %jmp/0xz <pc>, <flag>
 */
ତାର୍କିକ of_JMP0XZ(vthread_t thr, vvp_code_t cp)
{
      ଯଦି (thr->flags[cp->bit_idx[0]] != BIT4_1)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      ଯଦି (schedule_stopped()) {
	    schedule_vthread(thr, 0, ମିଥ୍ୟା);
	    ଫେରନ୍ତୁ ମିଥ୍ୟା;
      }

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %jmp/1 <pc>, <flag>
 */
ତାର୍କିକ of_JMP1(vthread_t thr, vvp_code_t cp)
{
      ଯଦି (thr->flags[cp->bit_idx[0]] == BIT4_1)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      ଯଦି (schedule_stopped()) {
	    schedule_vthread(thr, 0, ମିଥ୍ୟା);
	    ଫେରନ୍ତୁ ମିଥ୍ୟା;
      }

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %jmp/1xz <pc>, <flag>
 */
ତାର୍କିକ of_JMP1XZ(vthread_t thr, vvp_code_t cp)
{
      ଯଦି (thr->flags[cp->bit_idx[0]] != BIT4_0)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      ଯଦି (schedule_stopped()) {
	    schedule_vthread(thr, 0, ମିଥ୍ୟା);
	    ଫେରନ୍ତୁ ମିଥ୍ୟା;
      }

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * The %join instruction causes the thread to wait for one child
 * to die.  If a child is already dead (and a zombie) then I reap
 * it and go on. Otherwise, I mark myself as waiting in a join so that
 * children know to wake me when they finish.
 */

ସ୍ଥିର ଶୂନ୍ୟ do_join(vthread_t thr, vthread_t child)
{
      ପ୍ରତିଜ୍ଞା(child->parent == thr);

        /* If the immediate child thread is in an automatic scope... */
      ଯଦି (child->wt_context) {
              /* and is the top level task/function thread... */
            ଯଦି (thr->wt_context != thr->rd_context) {
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

ସ୍ଥିର ତାର୍କିକ do_join_opcode(vthread_t thr)
{
      ପ୍ରତିଜ୍ଞା( !thr->i_am_joining );
      ପ୍ରତିଜ୍ଞା( !thr->children.empty());

	// Are there any children that have already ended? If so, then
	// join with that one.
      ପାଇଁ (set<vthread_t>::iterator cur = thr->children.begin()
		 ; cur != thr->children.end() ; ++cur) {
	    vthread_t curp = *cur;
	    ଯଦି (! curp->i_have_ended)
		  ଜାରି;

	      // found something!
	    do_join(thr, curp);
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }

	// Otherwise, tell my children to awaken me when they end,
	// then pause.
      thr->i_am_joining = 1;
      ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ତାର୍କିକ of_JOIN(vthread_t thr, vvp_code_t)
{
      ଫେରନ୍ତୁ do_join_opcode(thr);
}

/*
 * This %join/detach <n> instruction causes the thread to detach
 * threads that were created by an earlier %fork.
 */
ତାର୍କିକ of_JOIN_DETACH(vthread_t thr, vvp_code_t cp)
{
      unsigned long ଗଣନା_2 = cp->ସଂଖ୍ୟା;

      ପ୍ରତିଜ୍ଞା(ଗଣନା_2 == thr->children.size());

      ଯେତେବେଳେ (! thr->children.empty()) {
	    vthread_t child = *thr->children.begin();
	    ପ୍ରତିଜ୍ଞା(child->parent == thr);

	      // We cannot detach automatic tasks/functions within an
	      // automatic scope. If we try to do that, we might make
	      // a mess of the allocation of the context. Note that it
	      // is OK if the child context is distinct (See %exec_ufunc.)
	    ପ୍ରତିଜ୍ଞା(child->wt_context==0 || thr->wt_context!=child->wt_context);
	    ଯଦି (child->i_have_ended) {
		    // If the child has already ended, then reap it.
		  vthread_reap(child);

	    } ଅନ୍ୟଥା {
		  size_t res = child->parent->children.erase(child);
		  ପ୍ରତିଜ୍ଞା(res == 1);
		  child->i_am_detached = 1;
		  thr->detached_children.insert(child);
	    }
      }

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %load/ar <array-label>, <index>;
*/
ତାର୍କିକ of_LOAD_AR(vthread_t thr, vvp_code_t cp)
{
      unsigned idx = cp->bit_idx[0];
      ଦ୍ୱିଗୁଣ word;

	/* The result is 0.0 if the address is undefined. */
      ଯଦି (thr->flags[4] == BIT4_1) {
	    word = 0.0;
      } ଅନ୍ୟଥା {
	    unsigned adr = thr->ଶବ୍ଦଗୁଡ଼ିକ[idx].w_int;
	    word = cp->array->get_word_r(adr);
      }

      thr->ଠେଲ_real(word);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

template <typename ELEM>
ସ୍ଥିର ତାର୍କିକ load_dar(vthread_t thr, vvp_code_t cp)
{
      int64_t adr = thr->ଶବ୍ଦଗୁଡ଼ିକ[3].w_int;
      vvp_net_t*net = cp->net;
      ପ୍ରତିଜ୍ଞା(net);

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      ପ୍ରତିଜ୍ଞା(obj);

      vvp_darray*darray = obj->get_object().peek<vvp_darray>();

      ELEM word;
      ଯଦି (darray &&
          (adr >= 0) && (thr->flags[4] == BIT4_0)) // A defined address >= 0
	    darray->get_word(adr, word);
      ଅନ୍ୟଥା
	    dq_default(word, obj->size());

      vthread_ଠେଲ(thr, word);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %load/dar/r <array-label>;
 */
ତାର୍କିକ of_LOAD_DAR_R(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ load_dar<ଦ୍ୱିଗୁଣ>(thr, cp);
}

/*
 * %load/dar/str <array-label>;
 */
ତାର୍କିକ of_LOAD_DAR_STR(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ load_dar<string>(thr, cp);
}

/*
 * %load/dar/vec4 <array-label>;
 */
ତାର୍କିକ of_LOAD_DAR_VEC4(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ load_dar<vvp_vector4_t>(thr, cp);
}

/*
 * %load/prop/dar/vec4 <pid>, <wid>;
 *   Indexed read of queue/dynamic-array class property; object on stack,
 *   index in words[3] (same as %load/dar/vec4).
 */
ତାର୍କିକ of_LOAD_PROP_DAR_VEC4(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->ସଂଖ୍ୟା;
      unsigned wid = cp->bit_idx[0];
      int64_t adr = thr->ଶବ୍ଦଗୁଡ଼ିକ[3].w_int;

      vvp_object_t& ଶୀର୍ଷ = thr->peek_object();
      vvp_cobject*cobj = ଶୀର୍ଷ.peek<vvp_cobject>();
      ପ୍ରତିଜ୍ଞା(cobj);

      vvp_object_t pobj;
      cobj->get_object(pid, pobj, 0);
      vvp_darray*darray = pobj.peek<vvp_darray>();

      vvp_vector4_t word;
      ଯଦି (darray && (adr >= 0) && (thr->flags[4] == BIT4_0)) {
	    darray->get_word(adr, word);
      } ଅନ୍ୟଥା {
	    dq_default(word, wid);
      }

      thr->ଠେଲ_vec4(word);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %load/obj <var-label>
 */
ତାର୍କିକ of_LOAD_OBJ(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      ପ୍ରତିଜ୍ଞା(fun);

      vvp_object_t val = fun->get_object();
      thr->ଠେଲ_object(val);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %load/obja <index>
 *    Loads the object from array, using index <index> as the index
 *    value. If flags[4] == 1, the calculation of <index> may have
 *    failed, so push nil.
 */
ତାର୍କିକ of_LOAD_OBJA(vthread_t thr, vvp_code_t cp)
{
      unsigned idx = cp->bit_idx[0];
      vvp_object_t word;

	/* The result is 0.0 if the address is undefined. */
      ଯଦି (thr->flags[4] == BIT4_1) {
	    ; // Return nil
      } ଅନ୍ୟଥା {
	    unsigned adr = thr->ଶବ୍ଦଗୁଡ଼ିକ[idx].w_int;
	    cp->array->get_word_obj(adr, word);
      }

      thr->ଠେଲ_object(word);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %load/real <var-label>
 */
ତାର୍କିକ of_LOAD_REAL(vthread_t thr, vvp_code_t cp)
{
      __vpiHandle*tmp = cp->handle;
      t_vpi_ମୂଲ୍ୟ val;

      val.format = vpiRealVal;
      vpi_get_ମୂଲ୍ୟ(tmp, &val);

      thr->ଠେଲ_real(val.value.real);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %load/str <var-label>
 */
ତାର୍କିକ of_LOAD_STR(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;


      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (net->fun);
      ପ୍ରତିଜ୍ଞା(fun);

      ଅଚଳ string&val = fun->get_string();
      thr->ଠେଲ_str(val);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_LOAD_STRA(vthread_t thr, vvp_code_t cp)
{
      unsigned idx = cp->bit_idx[0];
      string word;

      ଯଦି (thr->flags[4] == BIT4_1) {
	    word = "";
      } ଅନ୍ୟଥା {
	    unsigned adr = thr->ଶବ୍ଦଗୁଡ଼ିକ[idx].w_int;
	    word = cp->array->get_word_str(adr);
      }

      thr->ଠେଲ_str(word);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}


/*
 * %load/vec4 <net>
 */
ତାର୍କିକ of_LOAD_VEC4(vthread_t thr, vvp_code_t cp)
{
	// Push a placeholder onto the stack in order to reserve the
	// stack space. Use a reference for the stack top as a target
	// for the load.
      thr->ଠେଲ_vec4(vvp_vector4_t());
      vvp_vector4_t&sig_ମୂଲ୍ୟ = thr->peek_vec4();

      vvp_net_t*net = cp->net;

	// For the %load to work, the functor must actually be a
	// signal functor. Only signals save their vector value.
      vvp_signal_ମୂଲ୍ୟ*sig = dynamic_cast<vvp_signal_ମୂଲ୍ୟ*> (net->fil);
      ଯଦି (sig == 0) {
	    cerr << thr->get_fileline()
	         << "%load/v error: Net arg not a signal? "
		 << (net->fil ? typeid(*net->fil).name() :
	                        typeid(*net->fun).name())
	         << endl;
	    ପ୍ରତିଜ୍ଞା(sig);
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }

	// Extract the value from the signal and directly into the
	// target stack position.
      sig->vec4_ମୂଲ୍ୟ(sig_ମୂଲ୍ୟ);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %load/vec4a <arr>, <adrx>
 */
ତାର୍କିକ of_LOAD_VEC4A(vthread_t thr, vvp_code_t cp)
{
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା adr_index = cp->bit_idx[0];

      long adr = thr->ଶବ୍ଦଗୁଡ଼ିକ[adr_index].w_int;

	// If flag[3] is set, then the calculation of the address
	// failed, and this load should return X instead of the actual
	// value.
      ଯଦି (thr->flags[4] == BIT4_1) {
	    vvp_vector4_t tmp (cp->array->get_word_size(), BIT4_X);
	    thr->ଠେଲ_vec4(tmp);
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }

      vvp_vector4_t tmp (cp->array->get_word(adr));
      thr->ଠେଲ_vec4(tmp);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ସ୍ଥିର ଶୂନ୍ୟ do_verylong_mod(vvp_vector4_t&vala, ଅଚଳ vvp_vector4_t&valb,
			    ତାର୍କିକ ବାମ_is_neg, ତାର୍କିକ ଡାହାଣ_is_neg)
{
      ତାର୍କିକ out_is_neg = ବାମ_is_neg;
      ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା len=vala.size();
      unsigned ଅକ୍ଷର *a, *z, *t;
      a = ନୂତନ unsigned ଅକ୍ଷର[len+1];
      z = ନୂତନ unsigned ଅକ୍ଷର[len+1];
      t = ନୂତନ unsigned ଅକ୍ଷର[len+1];

      unsigned ଅକ୍ଷର carry;
      unsigned ଅକ୍ଷର temp;

      ପୂର୍ଣ୍ଣସଂଖ୍ୟା mxa = -1, mxz = -1;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା current, copylen;

      unsigned lb_carry = ବାମ_is_neg? 1 : 0;
      unsigned rb_carry = ଡାହାଣ_is_neg? 1 : 0;
      ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = 0 ;  idx < len ;  idx += 1) {
	    unsigned lb = vala.value(idx);
	    unsigned rb = valb.value(idx);

	    ଯଦି ((lb | rb) & 2) {
		  delete []t;
		  delete []z;
		  delete []a;
		  vvp_vector4_t tmp(len, BIT4_X);
		  vala = tmp;
		  ଫେରନ୍ତୁ;
	    }

	    ଯଦି (ବାମ_is_neg) {
		  lb = (1-lb) + lb_carry;
		  lb_carry = (lb & ~1)? 1 : 0;
		  lb &= 1;
	    }
	    ଯଦି (ଡାହାଣ_is_neg) {
		  rb = (1-rb) + rb_carry;
		  rb_carry = (rb & ~1)? 1 : 0;
		  rb &= 1;
	    }

	    z[idx]=lb;
	    a[idx]=1-rb;	// for 2s complement add..
      }

      z[len]=0;
      a[len]=1;

      ପାଇଁ(i=len-1;i>=0;i--) {
	    ଯଦି(! a[i]) {
		  mxa=i;
		  ଭାଙ୍ଗ;
	    }
      }

      ପାଇଁ(i=len-1;i>=0;i--) {
	    ଯଦି(z[i]) {
		  mxz=i;
		  ଭାଙ୍ଗ;
	    }
      }

      ଯଦି((mxa>mxz)||(mxa==-1)) {
	    ଯଦି(mxa==-1) {
		  delete []t;
		  delete []z;
		  delete []a;
		  vvp_vector4_t tmpx (len, BIT4_X);
		  vala = tmpx;
		  ଫେରନ୍ତୁ;
	    }

	    ଯାଅ tally;
      }

      copylen = mxa + 2;
      current = mxz - mxa;

      ଯେତେବେଳେ(current > -1) {
	    carry = 1;
	    ପାଇଁ(i=0;i<copylen;i++) {
		  temp = z[i+current] + a[i] + carry;
		  t[i] = (temp&1);
		  carry = (temp>>1);
	    }

	    ଯଦି(carry) {
		  ପାଇଁ(i=0;i<copylen;i++) {
			z[i+current] = t[i];
		  }
	    }

	    current--;
      }

 tally:

      vvp_vector4_t tmp (len, BIT4_X);
      carry = out_is_neg? 1 : 0;
      ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = 0 ;  idx < len ;  idx += 1) {
	    unsigned ob = z[idx];
	    ଯଦି (out_is_neg) {
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

ତାର୍କିକ of_MAX_WR(vthread_t thr, vvp_code_t)
{
      ଦ୍ୱିଗୁଣ r = thr->କାଢ଼_real();
      ଦ୍ୱିଗୁଣ l = thr->କାଢ଼_real();
      ଯଦି (r != r)
	    thr->ଠେଲ_real(l);
      ଅନ୍ୟଥା ଯଦି (l != l)
	    thr->ଠେଲ_real(r);
      ଅନ୍ୟଥା ଯଦି (r < l)
	    thr->ଠେଲ_real(l);
      ଅନ୍ୟଥା
	    thr->ଠେଲ_real(r);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_MIN_WR(vthread_t thr, vvp_code_t)
{
      ଦ୍ୱିଗୁଣ r = thr->କାଢ଼_real();
      ଦ୍ୱିଗୁଣ l = thr->କାଢ଼_real();
      ଯଦି (r != r)
	    thr->ଠେଲ_real(l);
      ଅନ୍ୟଥା ଯଦି (l != l)
	    thr->ଠେଲ_real(r);
      ଅନ୍ୟଥା ଯଦି (r < l)
	    thr->ଠେଲ_real(r);
      ଅନ୍ୟଥା
	    thr->ଠେଲ_real(l);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_MOD(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valb = thr->କାଢ଼_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      ପ୍ରତିଜ୍ଞା(vala.size()==valb.size());
      unsigned wid = vala.size();

      ଯଦି(wid <= 8*ଆକାର(unsigned long long)) {
	    unsigned long long lv = 0, rv = 0;

	    ପାଇଁ (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		  unsigned long long lb = vala.value(idx);
		  unsigned long long rb = valb.value(idx);

		  ଯଦି ((lb | rb) & 2)
			ଯାଅ x_out;

		  lv |= (unsigned long long) lb << idx;
		  rv |= (unsigned long long) rb << idx;
	    }

	    ଯଦି (rv == 0)
		  ଯାଅ x_out;

	    lv %= rv;

	    ପାଇଁ (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		  vala.set_bit(idx, (lv&1)?BIT4_1 : BIT4_0);
		  lv >>= 1;
	    }

	    ଫେରନ୍ତୁ ସତ୍ୟ;

      } ଅନ୍ୟଥା {
	    do_verylong_mod(vala, valb, ମିଥ୍ୟା, ମିଥ୍ୟା);
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }

 x_out:
      vala = vvp_vector4_t(wid, BIT4_X);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %mod/s
 */
ତାର୍କିକ of_MOD_S(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valb = thr->କାଢ଼_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      ପ୍ରତିଜ୍ଞା(vala.size()==valb.size());
      unsigned wid = vala.size();

	/* Handle the case that we can fit the bits into a long-long
	   variable. We cause use native % to do the work. */
      ଯଦି(wid <= 8*ଆକାର(long long)) {
	    long long lv = 0, rv = 0;

	    ପାଇଁ (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		  long long lb = vala.value(idx);
		  long long rb = valb.value(idx);

		  ଯଦି ((lb | rb) & 2)
			ଯାଅ x_out;

		  lv |= (long long) lb << idx;
		  rv |= (long long) rb << idx;
	    }

	    ଯଦି (rv == 0)
		  ଯାଅ x_out;

	    ଯଦି ((lv == LLONG_MIN) && (rv == -1))
		  ଯାଅ zero_out;

	      /* Sign extend the signed operands when needed. */
	    ଯଦି (wid < 8*ଆକାର(long long)) {
		  ଯଦି (lv & (1LL << (wid-1)))
			lv |= -1ULL << wid;
		  ଯଦି (rv & (1LL << (wid-1)))
			rv |= -1ULL << wid;
	    }

	    lv %= rv;

	    ପାଇଁ (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		  vala.set_bit(idx, (lv&1)? BIT4_1 : BIT4_0);
		  lv >>= 1;
	    }

	      // vala is the top of the stack, edited in place, so we
	      // do not need to push the result.

	    ଫେରନ୍ତୁ ସତ୍ୟ;

      } ଅନ୍ୟଥା {

	    ତାର୍କିକ ବାମ_is_neg  = vala.value(vala.size()-1) == BIT4_1;
	    ତାର୍କିକ ଡାହାଣ_is_neg = valb.value(valb.size()-1) == BIT4_1;
	    do_verylong_mod(vala, valb, ବାମ_is_neg, ଡାହାଣ_is_neg);
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }

 x_out:
      vala = vvp_vector4_t(wid, BIT4_X);
      ଫେରନ୍ତୁ ସତ୍ୟ;
 zero_out:
      vala = vvp_vector4_t(wid, BIT4_0);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %mod/wr
 */
ତାର୍କିକ of_MOD_WR(vthread_t thr, vvp_code_t)
{
      ଦ୍ୱିଗୁଣ r = thr->କାଢ଼_real();
      ଦ୍ୱିଗୁଣ l = thr->କାଢ଼_real();
      thr->ଠେଲ_real(fmod(l,r));

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %neg/wr
 */
ତାର୍କିକ of_NEG_WR(vthread_t thr, vvp_code_t)
{
      thr->poke_real(0, -thr->peek_real(0));
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %pad/s <wid>
 */
ତାର୍କିକ of_PAD_S(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->ସଂଖ୍ୟା;

      vvp_vector4_t&val = thr->peek_vec4();
      unsigned old_size = val.size();

	// Sign-extend.
      ଯଦି (old_size < wid)
	    val.resize(wid, val.value(old_size-1));
      ଅନ୍ୟଥା
	    val.resize(wid);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %pad/u <wid>
 */
ତାର୍କିକ of_PAD_U(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->ସଂଖ୍ୟା;

      vvp_vector4_t&val = thr->peek_vec4();
      val.resize(wid, BIT4_0);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %part/s <wid>
 * %part/u <wid>
 * Two values are popped from the stack. First, pop the canonical
 * index of the part select, and second is the value to be
 * selected. The result is pushed back to the stack.
 */
ସ୍ଥିର ତାର୍କିକ of_PART_base(vthread_t thr, vvp_code_t cp, ତାର୍କିକ signed_flag)
{
      unsigned wid = cp->ସଂଖ୍ୟା;

      vvp_vector4_t base4 = thr->କାଢ଼_vec4();
      vvp_vector4_t&value = thr->peek_vec4();

      vvp_vector4_t res (wid, BIT4_X);

	// NOTE: This is treating the vector as signed. Is that correct?
      int32_t base;
      ତାର୍କିକ ମୂଲ୍ୟ_ok = vector4_to_ମୂଲ୍ୟ(base4, base, signed_flag);
      ଯଦି (! ମୂଲ୍ୟ_ok) {
	    value = res;
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }

      ଯଦି (base >= (int32_t)value.size()) {
	    value = res;
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }

      ଯଦି ((base+(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)wid) <= 0) {
	    value = res;
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }

      long vbase = 0;
      ଯଦି (base < 0) {
	    vbase = -base;
	    wid -= vbase;
	    base = 0;
      }

      ଯଦି ((base+wid) > value.size()) {
	    wid = value.size() - base;
      }

      res .set_vec(vbase, value.subvalue(base, wid));
      value = res;

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_PART_S(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ of_PART_base(thr, cp, ସତ୍ୟ);
}

ତାର୍କିକ of_PART_U(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ of_PART_base(thr, cp, ମିଥ୍ୟା);
}

/*
 * %parti/s <wid>, <basei>, <base_wid>
 * %parti/u <wid>, <basei>, <base_wid>
 *
 * Pop the value to be selected. The result is pushed back to the stack.
 */
ସ୍ଥିର ତାର୍କିକ of_PARTI_base(vthread_t thr, vvp_code_t cp, ତାର୍କିକ signed_flag)
{
      unsigned wid = cp->ସଂଖ୍ୟା;
      uint32_t base = cp->bit_idx[0];
      uint32_t bwid = cp->bit_idx[1];

      vvp_vector4_t&value = thr->peek_vec4();

      vvp_vector4_t res (wid, BIT4_X);

	// NOTE: This is treating the vector as signed. Is that correct?
      int32_t use_base = base;
      ଯଦି (signed_flag && bwid < 32 && (base&(1<<(bwid-1)))) {
	    use_base |= -1UL << bwid;
      }

      ଯଦି (use_base >= (int32_t)value.size()) {
	    value = res;
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }

      ଯଦି ((use_base+(int32_t)wid) <= 0) {
	    value = res;
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }

      long vbase = 0;
      ଯଦି (use_base < 0) {
	    vbase = -use_base;
	    wid -= vbase;
	    use_base = 0;
      }

      ଯଦି ((use_base+wid) > value.size()) {
	    wid = value.size() - use_base;
      }

      res .set_vec(vbase, value.subvalue(use_base, wid));
      value = res;

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_PARTI_S(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ of_PARTI_base(thr, cp, ସତ୍ୟ);
}

ତାର୍କିକ of_PARTI_U(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ of_PARTI_base(thr, cp, ମିଥ୍ୟା);
}

/*
 * %mul
 */
ତାର୍କିକ of_MUL(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t r = thr->କାଢ଼_vec4();
	// Rather then pop l, use it directly from the stack. When we
	// assign to 'l', that will edit the top of the stack, which
	// replaces a pop and a pull.
      vvp_vector4_t&l = thr->peek_vec4();

      l.mul(r);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %muli <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
ତାର୍କିକ of_MULI(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->ସଂଖ୍ୟା;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_immediate_rval (cp, r);

      l.mul(r);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_MUL_WR(vthread_t thr, vvp_code_t)
{
      ଦ୍ୱିଗୁଣ r = thr->କାଢ଼_real();
      ଦ୍ୱିଗୁଣ l = thr->କାଢ଼_real();
      thr->ଠେଲ_real(l * r);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_NAND(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valr = thr->କାଢ଼_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      ପ୍ରତିଜ୍ଞା(vall.size() == valr.size());

      vall &= valr;
      vall.invert();

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %new/cobj <vpi_object>
 * This creates a new cobject (SystemVerilog class object) and pushes
 * it to the stack. The <vpi-object> is a __vpiHandle that is a
 * vpiClassDefn object that defines the item to be created.
 */
ତାର୍କିକ of_NEW_COBJ(vthread_t thr, vvp_code_t cp)
{
      ଅଚଳ class_type*defn = dynamic_cast<ଅଚଳ class_type*> (cp->handle);
      ପ୍ରତିଜ୍ଞା(defn);

      vvp_object_t tmp (ନୂତନ vvp_cobject(defn));
      thr->ଠେଲ_object(tmp);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_NEW_DARRAY(vthread_t thr, vvp_code_t cp)
{
      ଅଚଳ ଅକ୍ଷର*text = cp->text;
      size_t size = thr->ଶବ୍ଦଗୁଡ଼ିକ[cp->bit_idx[0]].w_int;
      unsigned word_wid;
      size_t n;

      vvp_object_t obj;
      ଯଦି (strcmp(text,"b8") == 0) {
	    obj = ନୂତନ vvp_darray_atom<uint8_t>(size);
      } ଅନ୍ୟଥା ଯଦି (strcmp(text,"b16") == 0) {
	    obj = ନୂତନ vvp_darray_atom<uint16_t>(size);
      } ଅନ୍ୟଥା ଯଦି (strcmp(text,"b32") == 0) {
	    obj = ନୂତନ vvp_darray_atom<uint32_t>(size);
      } ଅନ୍ୟଥା ଯଦି (strcmp(text,"b64") == 0) {
	    obj = ନୂତନ vvp_darray_atom<uint64_t>(size);
      } ଅନ୍ୟଥା ଯଦି (strcmp(text,"sb8") == 0) {
	    obj = ନୂତନ vvp_darray_atom<int8_t>(size);
      } ଅନ୍ୟଥା ଯଦି (strcmp(text,"sb16") == 0) {
	    obj = ନୂତନ vvp_darray_atom<int16_t>(size);
      } ଅନ୍ୟଥା ଯଦି (strcmp(text,"sb32") == 0) {
	    obj = ନୂତନ vvp_darray_atom<int32_t>(size);
      } ଅନ୍ୟଥା ଯଦି (strcmp(text,"sb64") == 0) {
	    obj = ନୂତନ vvp_darray_atom<int64_t>(size);
      } ଅନ୍ୟଥା ଯଦି ((1 == sscanf(text, "b%u%zn", &word_wid, &n)) &&
                 (n == strlen(text))) {
	    obj = ନୂତନ vvp_darray_vec2(size, word_wid);
      } ଅନ୍ୟଥା ଯଦି ((1 == sscanf(text, "sb%u%zn", &word_wid, &n)) &&
                 (n == strlen(text))) {
	    obj = ନୂତନ vvp_darray_vec2(size, word_wid);
      } ଅନ୍ୟଥା ଯଦି ((1 == sscanf(text, "v%u%zn", &word_wid, &n)) &&
                 (n == strlen(text))) {
	    obj = ନୂତନ vvp_darray_vec4(size, word_wid);
      } ଅନ୍ୟଥା ଯଦି ((1 == sscanf(text, "sv%u%zn", &word_wid, &n)) &&
                 (n == strlen(text))) {
	    obj = ନୂତନ vvp_darray_vec4(size, word_wid);
      } ଅନ୍ୟଥା ଯଦି (strcmp(text,"r") == 0) {
	    obj = ନୂତନ vvp_darray_real(size);
      } ଅନ୍ୟଥା ଯଦି (strcmp(text,"S") == 0) {
	    obj = ନୂତନ vvp_darray_string(size);
      } ଅନ୍ୟଥା {
	    cerr << get_fileline()
	         << "Internal error: Unsupported dynamic array type: "
	         << text << "." << endl;
	    ପ୍ରତିଜ୍ଞା(0);
      }

      thr->ଠେଲ_object(obj);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_NOOP(vthread_t, vvp_code_t)
{
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %nor/r
 */
ତାର୍କିକ of_NORR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_or());

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * Push a null to the object stack.
 */
ତାର୍କିକ of_NULL(vthread_t thr, vvp_code_t)
{
      vvp_object_t tmp;
      thr->ଠେଲ_object(tmp);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %and/r
 */
ତାର୍କିକ of_ANDR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_and());

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %nand/r
 */
ତାର୍କିକ of_NANDR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_and());

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %or/r
 */
ତାର୍କିକ of_ORR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_or());

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %xor/r
 */
ତାର୍କିକ of_XORR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_xor());

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %xnor/r
 */
ତାର୍କିକ of_XNORR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_xor());

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %or
 */
ତାର୍କିକ of_OR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valb = thr->କାଢ଼_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();
      vala |= valb;
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %nor
 */
ତାର୍କିକ of_NOR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valr = thr->କାଢ଼_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      ପ୍ରତିଜ୍ଞା(vall.size() == valr.size());

      vall |= valr;
      vall.invert();

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %pop/obj <num>, <skip>
 */
ତାର୍କିକ of_କାଢ଼_OBJ(vthread_t thr, vvp_code_t cp)
{
      unsigned cnt = cp->bit_idx[0];
      unsigned skip = cp->bit_idx[1];

      thr->କାଢ଼_object(cnt, skip);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %pop/real <number>
 */
ତାର୍କିକ of_କାଢ଼_REAL(vthread_t thr, vvp_code_t cp)
{
      unsigned cnt = cp->ସଂଖ୍ୟା;
      thr->କାଢ଼_real(cnt);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 *  %pop/str <number>
 */
ତାର୍କିକ of_କାଢ଼_STR(vthread_t thr, vvp_code_t cp)
{
      unsigned cnt = cp->ସଂଖ୍ୟା;
      thr->କାଢ଼_str(cnt);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 *  %pop/vec4 <number>
 */
ତାର୍କିକ of_କାଢ଼_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned cnt = cp->ସଂଖ୍ୟା;
      thr->କାଢ଼_vec4(cnt);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %pow
 * %pow/s
 */
ସ୍ଥିର ତାର୍କିକ of_POW_base(vthread_t thr, ତାର୍କିକ signed_flag)
{
      vvp_vector4_t valb = thr->କାଢ଼_vec4();
      vvp_vector4_t vala = thr->କାଢ଼_vec4();

      unsigned wid = vala.size();

      vvp_vector2_t xv2 = vvp_vector2_t(vala, ସତ୍ୟ);
      vvp_vector2_t yv2 = vvp_vector2_t(valb, ସତ୍ୟ);


        /* If we have an X or Z in the arguments return X. */
      ଯଦି (xv2.is_NaN() || yv2.is_NaN()) {
	    vvp_vector4_t tmp (wid, BIT4_X);
	    thr->ଠେଲ_vec4(tmp);
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }

	// Is the exponent negative? If so, table 5-6 in IEEE1364-2005
	// defines what value is returned.
      ଯଦି (signed_flag && yv2.value(yv2.size()-1)) {
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା a_val;
	    vvp_bit4_t pad = BIT4_0, lsb = BIT4_0;
	    ଯଦି (vector2_to_ମୂଲ୍ୟ(xv2, a_val, ସତ୍ୟ)) {
		  ଯଦି (a_val == 0) {
			pad = BIT4_X; lsb = BIT4_X;
		  }
		  ଯଦି (a_val == 1) {
			pad = BIT4_0; lsb = BIT4_1;
		  }
		  ଯଦି (a_val == -1) {
			ଯଦି (yv2.value(0)) {
			      pad = BIT4_1; lsb = BIT4_1;
			} ଅନ୍ୟଥା {
			      pad = BIT4_0; lsb = BIT4_1;
			}
		  }
	    }
	    vvp_vector4_t tmp (wid, pad);
	    tmp.set_bit(0, lsb);
	    thr->ଠେଲ_vec4(tmp);
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }

      vvp_vector2_t result = pow(xv2, yv2);

        /* Copy only what we need of the result. If the result is too
	   small, zero-pad it. */
      ପାଇଁ (unsigned jdx = 0;  jdx < wid;  jdx += 1) {
	    ଯଦି (jdx >= result.size())
		  vala.set_bit(jdx, BIT4_0);
	    ଅନ୍ୟଥା
		  vala.set_bit(jdx, result.value(jdx) ? BIT4_1 : BIT4_0);
      }
      thr->ଠେଲ_vec4(vala);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_POW(vthread_t thr, vvp_code_t)
{
      ଫେରନ୍ତୁ of_POW_base(thr, ମିଥ୍ୟା);
}

ତାର୍କିକ of_POW_S(vthread_t thr, vvp_code_t)
{
      ଫେରନ୍ତୁ of_POW_base(thr, ସତ୍ୟ);
}

ତାର୍କିକ of_POW_WR(vthread_t thr, vvp_code_t)
{
      ଦ୍ୱିଗୁଣ r = thr->କାଢ଼_real();
      ଦ୍ୱିଗୁଣ l = thr->କାଢ଼_real();
      thr->ଠେଲ_real(pow(l,r));

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %prop/obj <pid>, <idx>
 *
 * Load an object value from the cobject and push it onto the object stack.
 */
ତାର୍କିକ of_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      unsigned pid = cp->ସଂଖ୍ୟା;
      unsigned idx = cp->bit_idx[0];

      ଯଦି (idx != 0) {
	    ପ୍ରତିଜ୍ଞା(idx < vthread_s::ଶବ୍ଦଗୁଡ଼ିକ_ଗଣନା);
	    idx = thr->ଶବ୍ଦଗୁଡ଼ିକ[idx].w_uint;
      }

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();

      vvp_object_t val;
      cobj->get_object(pid, val, idx);

      thr->ଠେଲ_object(val);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ସ୍ଥିର ଶୂନ୍ୟ get_from_obj(unsigned pid, vvp_cobject*cobj, ଦ୍ୱିଗୁଣ&val)
{
      val = cobj->get_real(pid);
}

ସ୍ଥିର ଶୂନ୍ୟ get_from_obj(unsigned pid, vvp_cobject*cobj, string&val)
{
      val = cobj->get_string(pid);
}

ସ୍ଥିର ଶୂନ୍ୟ get_from_obj(unsigned pid, vvp_cobject*cobj, vvp_vector4_t&val)
{
      cobj->get_vec4(pid, val);
}

template <typename ELEM>
ସ୍ଥିର ତାର୍କିକ prop(vthread_t thr, vvp_code_t cp)
{
      unsigned pid = cp->ସଂଖ୍ୟା;

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      ପ୍ରତିଜ୍ଞା(cobj);

      ELEM val;
      get_from_obj(pid, cobj, val);
      vthread_ଠେଲ(thr, val);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %prop/r <pid>
 *
 * Load a real value from the cobject and push it onto the real value
 * stack.
 */
ତାର୍କିକ of_PROP_R(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ prop<ଦ୍ୱିଗୁଣ>(thr, cp);
}

/*
 * %prop/str <pid>
 *
 * Load a string value from the cobject and push it onto the real value
 * stack.
 */
ତାର୍କିକ of_PROP_STR(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ prop<string>(thr, cp);
}

/*
 * %prop/v <pid>
 *
 * Load a property <pid> from the cobject on the top of the stack into
 * the vector space at <base>.
 */
ତାର୍କିକ of_PROP_V(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ prop<vvp_vector4_t>(thr, cp);
}

ତାର୍କିକ of_PUSHI_REAL(vthread_t thr, vvp_code_t cp)
{
      ଦ୍ୱିଗୁଣ mant = cp->bit_idx[0];
      uint32_t imant = cp->bit_idx[0];
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା exp = cp->bit_idx[1];

	// Detect +infinity
      ଯଦି (exp==0x3fff && imant==0) {
	    thr->ଠେଲ_real(INFINITY);
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }
	// Detect -infinity
      ଯଦି (exp==0x7fff && imant==0) {
	    thr->ଠେଲ_real(-INFINITY);
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }
	// Detect NaN
      ଯଦି (exp==0x3fff) {
	    thr->ଠେଲ_real(nan(""));
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }

      ଦ୍ୱିଗୁଣ sign = (exp & 0x4000)? -1.0 : 1.0;

      exp &= 0x1fff;

      mant = sign * ldexp(mant, exp - 0x1000);
      thr->ଠେଲ_real(mant);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_PUSHI_STR(vthread_t thr, vvp_code_t cp)
{
      ଅଚଳ ଅକ୍ଷର*text = cp->text;
      thr->ଠେଲ_str(filter_string(text));
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %pushi/vec4 <vala>, <valb>, <wid>
 */
ତାର୍କିକ of_PUSHI_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned wid  = cp->ସଂଖ୍ୟା;

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t val (wid, BIT4_0);
      get_immediate_rval (cp, val);

      thr->ଠେଲ_vec4(val);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %pushv/str
 *   Pops a vec4 value, and pushes a string.
 */
ତାର୍କିକ of_PUSHV_STR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t vec = thr->କାଢ଼_vec4();

      size_t slen = (vec.size() + 7)/8;
      vector<ଅକ୍ଷର>buf;
      buf.reserve(slen);

      ପାଇଁ (size_t idx = 0 ; idx < vec.size() ; idx += 8) {
	    ଅକ୍ଷର tmp = 0;
	    size_t trans = 8;
	    ଯଦି (idx+trans > vec.size())
		  trans = vec.size() - idx;

	    ପାଇଁ (size_t bdx = 0 ; bdx < trans ; bdx += 1) {
		  ଯଦି (vec.value(idx+bdx) == BIT4_1)
			tmp |= 1 << bdx;
	    }

	    ଯଦି (tmp != 0)
		  buf.push_back(tmp);
      }

      string val;
      ପାଇଁ (vector<ଅକ୍ଷର>::reverse_iterator cur = buf.rbegin()
		 ; cur != buf.rend() ; ++cur) {
	    val.push_back(*cur);
      }

      thr->ଠେଲ_str(val);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %putc/str/vec4 <var>, <mux>
 */
ତାର୍କିକ of_PUTC_STR_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned muxr = cp->bit_idx[0];
      int32_t mux = muxr? thr->ଶବ୍ଦଗୁଡ଼ିକ[muxr].w_int : 0;

      vvp_vector4_t val = thr->କାଢ଼_vec4();
      ପ୍ରତିଜ୍ଞା(val.size() == 8);

      ଯଦି (mux < 0)
	    ଫେରନ୍ତୁ ସତ୍ୟ;

	/* Get the existing value of the string. If we find that the
	   index is too big for the string, then give up. */
      vvp_net_t*net = cp->net;
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (net->fun);
      ପ୍ରତିଜ୍ଞା(fun);

      string tmp = fun->get_string();
      ଯଦି (tmp.size() <= (size_t)mux)
	    ଫେରନ୍ତୁ ସତ୍ୟ;

      ଅକ୍ଷର val_str = 0;
      ପାଇଁ (size_t idx = 0 ; idx < 8 ; idx += 1) {
	    ଯଦି (val.value(idx)==BIT4_1)
		  val_str |= 1<<idx;
      }

	// It is a quirk of the Verilog standard that putc(..., 'h00)
	// has no effect. Test for that case here.
      ଯଦି (val_str == 0)
	    ଫେରନ୍ତୁ ସତ୍ୟ;

      tmp[mux] = val_str;

      vvp_send_string(vvp_net_ptr_t(cp->net, 0), tmp, thr->wt_context);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

template <typename ELEM, ଶ୍ରେଣୀ QTYPE>
ସ୍ଥିର ତାର୍କିକ qinsert(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
      int64_t idx = thr->ଶବ୍ଦଗୁଡ଼ିକ[3].w_int;
      ELEM value;
      vvp_net_t*net = cp->net;
      କାଢ଼_ମୂଲ୍ୟ(thr, value, wid); // Pop the value to store.

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      ପ୍ରତିଜ୍ଞା(queue);
      ଯଦି (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at a negative "
	         << get_queue_type(value)
	         << " index (" << idx << "). ";
	    print_queue_ମୂଲ୍ୟ(value);
	    cerr << " was not added." << endl;
      } ଅନ୍ୟଥା ଯଦି (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at an undefined "
	         << get_queue_type(value) << " index. ";
	    print_queue_ମୂଲ୍ୟ(value);
	    cerr << " was not added." << endl;
      } ଅନ୍ୟଥା {
	    unsigned max_size = thr->ଶବ୍ଦଗୁଡ଼ିକ[cp->bit_idx[0]].w_int;
	    queue->insert(idx, value, max_size);
      }
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %qinsert/real <var-label>
 */
ତାର୍କିକ of_QINSERT_REAL(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ qinsert<ଦ୍ୱିଗୁଣ, vvp_queue_real>(thr, cp);
}

/*
 * %qinsert/str <var-label>
 */
ତାର୍କିକ of_QINSERT_STR(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ qinsert<string, vvp_queue_string>(thr, cp);
}

/*
 * %qinsert/v <var-label>
 */
ତାର୍କିକ of_QINSERT_V(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ qinsert<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

/*
 * Helper functions used in the queue pop templates
 */
ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ ଠେଲ_ମୂଲ୍ୟ(vthread_t thr, ଦ୍ୱିଗୁଣ value, unsigned)
{
      thr->ଠେଲ_real(value);
}

ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ ଠେଲ_ମୂଲ୍ୟ(vthread_t thr, ଅଚଳ string&value, unsigned)
{
      thr->ଠେଲ_str(value);
}

ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ ଠେଲ_ମୂଲ୍ୟ(vthread_t thr, ଅଚଳ vvp_vector4_t&value, unsigned wid)
{
      ପ୍ରତିଜ୍ଞା(wid == value.size());
      thr->ଠେଲ_vec4(value);
}

template <typename ELEM, ଶ୍ରେଣୀ QTYPE>
ସ୍ଥିର ତାର୍କିକ q_କାଢ଼(vthread_t thr, vvp_code_t cp,
                  ଶୂନ୍ୟ (*get_val_func)(vvp_queue*, ELEM&),
                  ଅଚଳ ଅକ୍ଷର*loc, unsigned wid)
{
      vvp_net_t*net = cp->net;

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      ପ୍ରତିଜ୍ଞା(queue);

      size_t size = queue->get_size();

      ELEM value;
      ଯଦି (size) {
	    get_val_func(queue, value);
      } ଅନ୍ୟଥା {
	    dq_default(value, wid);
	    cerr << thr->get_fileline()
	         << "Warning: pop_" << loc << "() on empty "
	         << get_queue_type(value) << "." << endl;
      }

      ଠେଲ_ମୂଲ୍ୟ(thr, value, wid);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

template <typename ELEM>
ସ୍ଥିର ଶୂନ୍ୟ get_back_ମୂଲ୍ୟ(vvp_queue*queue, ELEM&value)
{
      queue->get_word(queue->get_size()-1, value);
      queue->pop_back();
}

template <typename ELEM, ଶ୍ରେଣୀ QTYPE>
ସ୍ଥିର ତାର୍କିକ qpop_b(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
      ଫେରନ୍ତୁ q_କାଢ଼<ELEM, QTYPE>(thr, cp, get_back_ମୂଲ୍ୟ<ELEM>, "back", wid);
}

/*
 * %qpop/b/real <var-label>
 */
ତାର୍କିକ of_QPOP_B_REAL(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ qpop_b<ଦ୍ୱିଗୁଣ, vvp_queue_real>(thr, cp);
}

/*
 * %qpop/b/str <var-label>
 */
ତାର୍କିକ of_QPOP_B_STR(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ qpop_b<string, vvp_queue_string>(thr, cp);
}

/*
 * %qpop/b/v <var-label>
 */
ତାର୍କିକ of_QPOP_B_V(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ qpop_b<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[0]);
}

template <typename ELEM>
ସ୍ଥିର ଶୂନ୍ୟ get_front_ମୂଲ୍ୟ(vvp_queue*queue, ELEM&value)
{
      queue->get_word(0, value);
      queue->pop_front();
}

template <typename ELEM, ଶ୍ରେଣୀ QTYPE>
ସ୍ଥିର ତାର୍କିକ qpop_f(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
      ଫେରନ୍ତୁ q_କାଢ଼<ELEM, QTYPE>(thr, cp, get_front_ମୂଲ୍ୟ<ELEM>, "front", wid);
}


/*
 * %qpop/f/real <var-label>
 */
ତାର୍କିକ of_QPOP_F_REAL(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ qpop_f<ଦ୍ୱିଗୁଣ, vvp_queue_real>(thr, cp);
}

/*
 * %qpop/f/str <var-label>
 */
ତାର୍କିକ of_QPOP_F_STR(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ qpop_f<string, vvp_queue_string>(thr, cp);
}

/*
 * %qpop/f/v <var-label>
 */
ତାର୍କିକ of_QPOP_F_V(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ qpop_f<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[0]);
}

/*
 * These implement the %release/net and %release/reg instructions. The
 * %release/net instruction applies to a net kind of functor by
 * sending the release/net command to the command port. (See vvp_net.h
 * for details.) The %release/reg instruction is the same, but sends
 * the release/reg command instead. These are very similar to the
 * %deassign instruction.
 */
ସ୍ଥିର ତାର୍କିକ do_release_vec(vvp_code_t cp, ତାର୍କିକ net_flag)
{
      vvp_net_t*net = cp->net;
      unsigned base  = cp->bit_idx[0];
      unsigned width = cp->bit_idx[1];

      ପ୍ରତିଜ୍ଞା(net->fil);

      ଯଦି (base >= net->fil->filter_size()) ଫେରନ୍ତୁ ସତ୍ୟ;
      ଯଦି (base+width > net->fil->filter_size())
	    width = net->fil->filter_size() - base;

      ତାର୍କିକ full_sig = base == 0 && width == net->fil->filter_size();

	// XXXX Can't really do this if this is a partial release?
      net->fil->force_unlink();

	/* Do we release all or part of the net? */
      vvp_net_ptr_t ptr (net, 0);
      ଯଦି (full_sig) {
	    net->fil->release(ptr, net_flag);
      } ଅନ୍ୟଥା {
	    net->fil->release_pv(ptr, base, width, net_flag);
      }
      net->fun->force_flag(ମିଥ୍ୟା);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_RELEASE_NET(vthread_t, vvp_code_t cp)
{
      ଫେରନ୍ତୁ do_release_vec(cp, ସତ୍ୟ);
}


ତାର୍କିକ of_RELEASE_REG(vthread_t, vvp_code_t cp)
{
      ଫେରନ୍ତୁ do_release_vec(cp, ମିଥ୍ୟା);
}

/* The type is 1 for registers and 0 for everything else. */
ତାର୍କିକ of_RELEASE_WR(vthread_t, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      unsigned type  = cp->bit_idx[0];

      ପ୍ରତିଜ୍ଞା(net->fil);
      net->fil->force_unlink();

	// Send a command to this signal to unforce itself.
      vvp_net_ptr_t ptr (net, 0);
      net->fil->release(ptr, type==0);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_REPLICATE(vthread_t thr, vvp_code_t cp)
{
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା rept = cp->ସଂଖ୍ୟା;
      vvp_vector4_t val = thr->କାଢ଼_vec4();
      vvp_vector4_t res (val.size() * rept, BIT4_X);

      ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = 0 ; idx < rept ; idx += 1) {
	    res.set_vec(idx * val.size(), val);
      }

      thr->ଠେଲ_vec4(res);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ସ୍ଥିର ଶୂନ୍ୟ poke_val(vthread_t fun_thr, unsigned depth, ଦ୍ୱିଗୁଣ val)
{
      fun_thr->parent->poke_real(depth, val);
}

ସ୍ଥିର ଶୂନ୍ୟ poke_val(vthread_t fun_thr, unsigned depth, ଅଚଳ string&val)
{
      fun_thr->parent->poke_str(depth, val);
}

ସ୍ଥିର size_t get_max(vthread_t fun_thr, ଦ୍ୱିଗୁଣ&)
{
      ଫେରନ୍ତୁ fun_thr->args_real.size();
}

ସ୍ଥିର size_t get_max(vthread_t fun_thr, string&)
{
      ଫେରନ୍ତୁ fun_thr->args_str.size();
}

ସ୍ଥିର size_t get_max(vthread_t fun_thr, vvp_vector4_t&)
{
      ଫେରନ୍ତୁ fun_thr->args_vec4.size();
}

ସ୍ଥିର unsigned get_depth(vthread_t fun_thr, size_t index, ଦ୍ୱିଗୁଣ&)
{
      ଫେରନ୍ତୁ fun_thr->args_real[index];
}

ସ୍ଥିର unsigned get_depth(vthread_t fun_thr, size_t index, string&)
{
      ଫେରନ୍ତୁ fun_thr->args_str[index];
}

ସ୍ଥିର unsigned get_depth(vthread_t fun_thr, size_t index, vvp_vector4_t&)
{
      ଫେରନ୍ତୁ fun_thr->args_vec4[index];
}

ସ୍ଥିର vthread_t get_func(vthread_t thr)
{
      vthread_t fun_thr = thr;

      ଯେତେବେଳେ (fun_thr->parent_scope->get_type_code() != vpiFunction) {
	    ପ୍ରତିଜ୍ଞା(fun_thr->parent);
	    fun_thr = fun_thr->parent;
      }

      ଫେରନ୍ତୁ fun_thr;
}

template <typename ELEM>
ସ୍ଥିର ତାର୍କିକ ret(vthread_t thr, vvp_code_t cp)
{
      size_t index = cp->ସଂଖ୍ୟା;
      ELEM val;
      କାଢ଼_ମୂଲ୍ୟ(thr, val, 0);

      vthread_t fun_thr = get_func(thr);
      ପ୍ରତିଜ୍ଞା(index < get_max(fun_thr, val));

      unsigned depth = get_depth(fun_thr, index, val);
	// Use the depth to put the value into the stack of
	// the parent thread.
      poke_val(fun_thr, depth, val);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %ret/real <index>
 */
ତାର୍କିକ of_RET_REAL(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ ret<ଦ୍ୱିଗୁଣ>(thr, cp);
}

/*
 * %ret/str <index>
 */
ତାର୍କିକ of_RET_STR(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ ret<string>(thr, cp);
}

/*
 * %ret/vec4 <index>, <offset>, <wid>
 */
ତାର୍କିକ of_RET_VEC4(vthread_t thr, vvp_code_t cp)
{
      size_t index = cp->ସଂଖ୍ୟା;
      unsigned off_index = cp->bit_idx[0];
      unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା wid = cp->bit_idx[1];
      vvp_vector4_t&val = thr->peek_vec4();

      vthread_t fun_thr = get_func(thr);
      ପ୍ରତିଜ୍ଞା(index < get_max(fun_thr, val));
      ପ୍ରତିଜ୍ଞା(val.size() == wid);
      unsigned depth = get_depth(fun_thr, index, val);

      int64_t off = off_index ? thr->ଶବ୍ଦଗୁଡ଼ିକ[off_index].w_int : 0;
      unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା sig_ମୂଲ୍ୟ_size = fun_thr->parent->peek_vec4(depth).size();

      ଯଦି (off_index!=0 && thr->flags[4] == BIT4_1) {
	    thr->କାଢ଼_vec4(1);
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }

      ଯଦି (!resize_rval_vec(val, off, sig_ମୂଲ୍ୟ_size)) {
	    thr->କାଢ଼_vec4(1);
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }

      ଯଦି (off == 0 && val.size() == sig_ମୂଲ୍ୟ_size) {
	    fun_thr->parent->poke_vec4(depth, val);
      } ଅନ୍ୟଥା {
	    vvp_vector4_t tmp_dst = fun_thr->parent->peek_vec4(depth);
	    tmp_dst.set_vec(off, val);
	    fun_thr->parent->poke_vec4(depth, tmp_dst);
      }

      thr->କାଢ଼_vec4(1);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ସ୍ଥିର ଶୂନ୍ୟ ଠେଲ_from_parent(vthread_t thr, vthread_t fun_thr, unsigned depth, ଦ୍ୱିଗୁଣ&)
{
      thr->ଠେଲ_real(fun_thr->parent->peek_real(depth));
}

ସ୍ଥିର ଶୂନ୍ୟ ଠେଲ_from_parent(vthread_t thr, vthread_t fun_thr, unsigned depth, string&)
{
      thr->ଠେଲ_str(fun_thr->parent->peek_str(depth));
}

ସ୍ଥିର ଶୂନ୍ୟ ଠେଲ_from_parent(vthread_t thr, vthread_t fun_thr, unsigned depth, vvp_vector4_t&)
{
      thr->ଠେଲ_vec4(fun_thr->parent->peek_vec4(depth));
}

template <typename ELEM>
ସ୍ଥିର ତାର୍କିକ retload(vthread_t thr, vvp_code_t cp)
{
      size_t index = cp->ସଂଖ୍ୟା;
      ELEM type;

      vthread_t fun_thr = get_func(thr);
      ପ୍ରତିଜ୍ଞା(index < get_max(fun_thr, type));

      unsigned depth = get_depth(fun_thr, index, type);
	// Use the depth to extract the values from the stack
	// of the parent thread.
      ଠେଲ_from_parent(thr, fun_thr, depth, type);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %retload/real <index>
 */
ତାର୍କିକ of_RETLOAD_REAL(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ retload<ଦ୍ୱିଗୁଣ>(thr, cp);
}

/*
 * %retload/str <index>
 */
ତାର୍କିକ of_RETLOAD_STR(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ retload<string>(thr, cp);
}

/*
 * %retload/vec4 <index>
 */
ତାର୍କିକ of_RETLOAD_VEC4(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ retload<vvp_vector4_t>(thr, cp);
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
ତାର୍କିକ of_SCOPY(vthread_t thr, vvp_code_t)
{
      vvp_object_t tmp;
      thr->କାଢ଼_object(tmp);

      vvp_object_t&dest = thr->peek_object();
        // If it is null there is nothing to copy
      ଯଦି (!tmp.test_nil())
	    dest.shallow_copy(tmp);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ସ୍ଥିର ଶୂନ୍ୟ thread_peek(vthread_t thr, ଦ୍ୱିଗୁଣ&value)
{
      value = thr->peek_real(0);
}

ସ୍ଥିର ଶୂନ୍ୟ thread_peek(vthread_t thr, string&value)
{
      value = thr->peek_str(0);
}

ସ୍ଥିର ଶୂନ୍ୟ thread_peek(vthread_t thr, vvp_vector4_t&value)
{
      value = thr->peek_vec4(0);
}

template <typename ELEM>
ସ୍ଥିର ତାର୍କିକ set_dar_obj(vthread_t thr, vvp_code_t cp)
{
      unsigned adr = thr->ଶବ୍ଦଗୁଡ଼ିକ[cp->ସଂଖ୍ୟା].w_int;

      ELEM value;
      thread_peek(thr, value);

      vvp_object_t&ଶୀର୍ଷ = thr->peek_object();
      vvp_darray*darray = ଶୀର୍ଷ.peek<vvp_darray>();
      ପ୍ରତିଜ୍ଞା(darray);

      darray->set_word(adr, value);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %set/dar/obj/real <index>
 */
ତାର୍କିକ of_SET_DAR_OBJ_REAL(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ set_dar_obj<ଦ୍ୱିଗୁଣ>(thr, cp);
}

/*
 * %set/dar/obj/str <index>
 */
ତାର୍କିକ of_SET_DAR_OBJ_STR(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ set_dar_obj<string>(thr, cp);
}

/*
 * %set/dar/obj/vec4 <index>
 */
ତାର୍କିକ of_SET_DAR_OBJ_VEC4(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ set_dar_obj<vvp_vector4_t>(thr, cp);
}

/*
 * %shiftl <idx>
 *
 * Pop the operand, then push the result.
 */
ତାର୍କିକ of_SHIFTL(vthread_t thr, vvp_code_t cp)
{
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା use_index = cp->ସଂଖ୍ୟା;
      uint64_t shift = thr->ଶବ୍ଦଗୁଡ଼ିକ[use_index].w_uint;

      vvp_vector4_t&val = thr->peek_vec4();
      unsigned wid  = val.size();

      ଯଦି (thr->flags[4] == BIT4_1) {
	      // The result is 'bx if the shift amount is undefined
	    val = vvp_vector4_t(wid, BIT4_X);

      } ଅନ୍ୟଥା ଯଦି (thr->flags[4] == BIT4_X || shift >= wid) {
	      // Shift is so big that all value is shifted out. Write
	      // a constant 0 result.
	    val = vvp_vector4_t(wid, BIT4_0);

      } ଅନ୍ୟଥା ଯଦି (shift > 0) {
	    vvp_vector4_t blk = val.subvalue(0, wid-shift);
	    vvp_vector4_t tmp (shift, BIT4_0);
	    val.set_vec(0, tmp);
	    val.set_vec(shift, blk);
      }

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %shiftr <idx>
 * This is an unsigned right shift. The <idx> is a number that selects
 * the index register with the amount of the shift. This instruction
 * checks flag bit 4, which will be true if the shift is invalid.
 */
ତାର୍କିକ of_SHIFTR(vthread_t thr, vvp_code_t cp)
{
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା use_index = cp->ସଂଖ୍ୟା;
      uint64_t shift = thr->ଶବ୍ଦଗୁଡ଼ିକ[use_index].w_uint;

      vvp_vector4_t val = thr->କାଢ଼_vec4();
      unsigned wid  = val.size();

      ଯଦି (thr->flags[4] == BIT4_1) {
	    val = vvp_vector4_t(wid, BIT4_X);

      } ଅନ୍ୟଥା ଯଦି (thr->flags[4] == BIT4_X || shift > wid) {
	    val = vvp_vector4_t(wid, BIT4_0);

      } ଅନ୍ୟଥା ଯଦି (shift > 0) {
	    vvp_vector4_t blk = val.subvalue(shift, wid-shift);
	    vvp_vector4_t tmp (shift, BIT4_0);
	    val.set_vec(0, blk);
	    val.set_vec(wid-shift, tmp);
      }

      thr->ଠେଲ_vec4(val);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 *  %shiftr/s <wid>
 */
ତାର୍କିକ of_SHIFTR_S(vthread_t thr, vvp_code_t cp)
{
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା use_index = cp->ସଂଖ୍ୟା;
      uint64_t shift = thr->ଶବ୍ଦଗୁଡ଼ିକ[use_index].w_uint;

      vvp_vector4_t val = thr->କାଢ଼_vec4();
      unsigned wid  = val.size();

      vvp_bit4_t sign_bit = val.value(val.size()-1);

      ଯଦି (thr->flags[4] == BIT4_1) {
	    val = vvp_vector4_t(wid, BIT4_X);

      } ଅନ୍ୟଥା ଯଦି (thr->flags[4] == BIT4_X || shift > wid) {
	    val = vvp_vector4_t(wid, sign_bit);

      } ଅନ୍ୟଥା ଯଦି (shift > 0) {
	    vvp_vector4_t blk = val.subvalue(shift, wid-shift);
	    vvp_vector4_t tmp (shift, sign_bit);
	    val.set_vec(0, blk);
	    val.set_vec(wid-shift, tmp);
      }

      thr->ଠେଲ_vec4(val);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %split/vec4 <wid>
 *   Pop 1 value,
 *   Take <wid> bits from the lsb,
 *   Push the remaining msb,
 *   Push the lsb.
 */
ତାର୍କିକ of_SPLIT_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned lsb_wid = cp->ସଂଖ୍ୟା;

      vvp_vector4_t&val = thr->peek_vec4();
      ପ୍ରତିଜ୍ଞା(lsb_wid < val.size());

      vvp_vector4_t lsb = val.subvalue(0, lsb_wid);
      val = val.subvalue(lsb_wid, val.size()-lsb_wid);

      thr->ଠେଲ_vec4(lsb);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * The following are used to allow the darray templates to print correctly.
 */
ଧାଡ଼ିଭିତରେ ସ୍ଥିର string get_darray_type(ଦ୍ୱିଗୁଣ&)
{
      ଫେରନ୍ତୁ "darray<real>";
}

ଧାଡ଼ିଭିତରେ ସ୍ଥିର string get_darray_type(string&)
{
      ଫେରନ୍ତୁ "darray<string>";
}

ଧାଡ଼ିଭିତରେ ସ୍ଥିର string get_darray_type(ଅଚଳ vvp_vector4_t&value)
{
      ostringstream buf;
      buf << "darray<vector[" << value.size() << "]>";
      string res = buf.str();
      ଫେରନ୍ତୁ res;
}

/*
 * The following are used to allow a common template to be written for
 * darray real/string/vec4 operations
 */
ଧାଡ଼ିଭିତରେ ସ୍ଥିର ଶୂନ୍ୟ dar_କାଢ଼_ମୂଲ୍ୟ(vthread_t thr, ଦ୍ୱିଗୁଣ&value)
{
      value = thr->କାଢ଼_real();
}

ଧାଡ଼ିଭିତରେ ସ୍ଥିର ଶୂନ୍ୟ dar_କାଢ଼_ମୂଲ୍ୟ(vthread_t thr, string&value)
{
      value = thr->କାଢ଼_str();
}

ଧାଡ଼ିଭିତରେ ସ୍ଥିର ଶୂନ୍ୟ dar_କାଢ଼_ମୂଲ୍ୟ(vthread_t thr, vvp_vector4_t&value)
{
      value = thr->କାଢ଼_vec4();
}

template <typename ELEM>
ସ୍ଥିର ତାର୍କିକ store_dar(vthread_t thr, vvp_code_t cp)
{
      int64_t adr = thr->ଶବ୍ଦଗୁଡ଼ିକ[3].w_int;
      ELEM value;
	// FIXME: Can we get the size of the underlying array element
	//        and then use the normal pop_value?
      dar_କାଢ଼_ମୂଲ୍ୟ(thr, value);

      vvp_net_t*net = cp->net;
      ପ୍ରତିଜ୍ଞା(net);

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      ପ୍ରତିଜ୍ଞା(obj);

      vvp_darray*darray = obj->get_object().peek<vvp_darray>();

      ଯଦି (adr < 0)
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to a negative " << get_darray_type(value)
	         << " index (" << adr << ")." << endl;
      ଅନ୍ୟଥା ଯଦି (thr->flags[4] != BIT4_0)
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to an undefined " << get_darray_type(value)
	         << " index." << endl;
      ଅନ୍ୟଥା ଯଦି (darray)
	    darray->set_word(adr, value);
      ଅନ୍ୟଥା
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to an undefined " << get_darray_type(value)
	         << "." << endl;

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %store/dar/real <var>
 */
ତାର୍କିକ of_STORE_DAR_R(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ store_dar<ଦ୍ୱିଗୁଣ>(thr, cp);
}

/*
 * %store/dar/str <var>
 */
ତାର୍କିକ of_STORE_DAR_STR(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ store_dar<string>(thr, cp);
}

/*
 * %store/dar/vec4 <var>
 */
ତାର୍କିକ of_STORE_DAR_VEC4(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ store_dar<vvp_vector4_t>(thr, cp);
}

ତାର୍କିକ of_STORE_OBJ(vthread_t thr, vvp_code_t cp)
{
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);

      vvp_object_t val;
      thr->କାଢ଼_object(val);

      vvp_send_object(ptr, val, thr->wt_context);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %store/obja <array-label> <index>
 */
ତାର୍କିକ of_STORE_OBJA(vthread_t thr, vvp_code_t cp)
{
      unsigned idx = cp->bit_idx[0];
      unsigned adr = thr->ଶବ୍ଦଗୁଡ଼ିକ[idx].w_int;

      vvp_object_t val;
      thr->କାଢ଼_object(val);

      cp->array->set_word(adr, val);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}


/*
 * %store/prop/obj <pid>, <idx>
 *
 * Pop an object value from the object stack, and store the value into
 * the property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
ତାର୍କିକ of_STORE_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->ସଂଖ୍ୟା;
      unsigned idx = cp->bit_idx[0];

      ଯଦି (idx != 0) {
	    ପ୍ରତିଜ୍ଞା(idx < vthread_s::ଶବ୍ଦଗୁଡ଼ିକ_ଗଣନା);
	    idx = thr->ଶବ୍ଦଗୁଡ଼ିକ[idx].w_uint;
      }

      vvp_object_t val;
      thr->କାଢ଼_object(val);

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      ପ୍ରତିଜ୍ଞା(cobj);

      cobj->set_object(pid, val, idx);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ସ୍ଥିର ଶୂନ୍ୟ କାଢ଼_prop_val(vthread_t thr, ଦ୍ୱିଗୁଣ&val, unsigned)
{
      val = thr->କାଢ଼_real();
}

ସ୍ଥିର ଶୂନ୍ୟ କାଢ଼_prop_val(vthread_t thr, string&val, unsigned)
{
      val = thr->କାଢ଼_str();
}

ସ୍ଥିର ଶୂନ୍ୟ କାଢ଼_prop_val(vthread_t thr, vvp_vector4_t&val, unsigned wid)
{
      val = thr->କାଢ଼_vec4();
      ପ୍ରତିଜ୍ଞା(val.size() >= wid);
      val.resize(wid);
}

ସ୍ଥିର ଶୂନ୍ୟ set_val(vvp_cobject*cobj, size_t pid, ଅଚଳ ଦ୍ୱିଗୁଣ&val)
{
      cobj->set_real(pid, val);
}

ସ୍ଥିର ଶୂନ୍ୟ set_val(vvp_cobject*cobj, size_t pid, ଅଚଳ string&val)
{
      cobj->set_string(pid, val);
}

ସ୍ଥିର ଶୂନ୍ୟ set_val(vvp_cobject*cobj, size_t pid, ଅଚଳ vvp_vector4_t&val)
{
      cobj->set_vec4(pid, val);
}

template <typename ELEM>
ସ୍ଥିର ତାର୍କିକ store_prop(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
      size_t pid = cp->ସଂଖ୍ୟା;
      ELEM val;
      କାଢ଼_prop_val(thr, val, wid); // Pop the value to store.

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      ପ୍ରତିଜ୍ଞା(cobj);

      set_val(cobj, pid, val);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %store/prop/r <id>
 *
 * Pop a real value from the real stack, and store the value into the
 * property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
ତାର୍କିକ of_STORE_PROP_R(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ store_prop<ଦ୍ୱିଗୁଣ>(thr, cp);
}

/*
 * %store/prop/str <id>
 *
 * Pop a string value from the string stack, and store the value into
 * the property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
ତାର୍କିକ of_STORE_PROP_STR(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ store_prop<string>(thr, cp);
}

/*
 * %store/prop/v <pid>, <wid>
 *
 * Store vector value into property <id> of cobject in the top of the
 * stack. Do NOT pop the object stack.
 */
ତାର୍କିକ of_STORE_PROP_V(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ store_prop<vvp_vector4_t>(thr, cp, cp->bit_idx[0]);
}

template <ଶ୍ରେଣୀ QTYPE>
ସ୍ଥିର QTYPE* get_queue_prop(vvp_cobject*cobj, size_t pid)
{
      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      QTYPE* queue = qobj.peek<QTYPE>();
      ଯଦି (queue == 0) {
	    queue = ନୂତନ QTYPE;
	    vvp_object_t val(queue);
	    cobj->set_object(pid, val, 0);
      }
      ଫେରନ୍ତୁ queue;
}

template <typename ELEM, ଶ୍ରେଣୀ QTYPE>
ସ୍ଥିର ତାର୍କିକ store_prop_qb(vthread_t thr, vvp_code_t cp, unsigned wid)
{
      size_t pid = cp->ସଂଖ୍ୟା;
      ELEM value;
      unsigned max_size = thr->ଶବ୍ଦଗୁଡ଼ିକ[cp->bit_idx[0]].w_uint;
      କାଢ଼_ମୂଲ୍ୟ(thr, value, wid);

      vvp_object_t& ଶୀର୍ଷ = thr->peek_object();
      vvp_cobject*cobj = ଶୀର୍ଷ.peek<vvp_cobject>();
      ପ୍ରତିଜ୍ଞା(cobj);

      QTYPE* queue = get_queue_prop<QTYPE>(cobj, pid);
      ପ୍ରତିଜ୍ଞା(queue);
      queue->push_back(value, max_size);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

template <typename ELEM, ଶ୍ରେଣୀ QTYPE>
ସ୍ଥିର ତାର୍କିକ store_prop_qf(vthread_t thr, vvp_code_t cp, unsigned wid)
{
      size_t pid = cp->ସଂଖ୍ୟା;
      ELEM value;
      unsigned max_size = thr->ଶବ୍ଦଗୁଡ଼ିକ[cp->bit_idx[0]].w_uint;
      କାଢ଼_ମୂଲ୍ୟ(thr, value, wid);

      vvp_object_t& ଶୀର୍ଷ = thr->peek_object();
      vvp_cobject*cobj = ଶୀର୍ଷ.peek<vvp_cobject>();
      ପ୍ରତିଜ୍ଞା(cobj);

      QTYPE* queue = get_queue_prop<QTYPE>(cobj, pid);
      ପ୍ରତିଜ୍ଞା(queue);
      queue->push_front(value, max_size);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_STORE_PROP_QB_R(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ store_prop_qb<ଦ୍ୱିଗୁଣ, vvp_queue_real>(thr, cp, 0);
}

ତାର୍କିକ of_STORE_PROP_QB_STR(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ store_prop_qb<string, vvp_queue_string>(thr, cp, 0);
}

ତାର୍କିକ of_STORE_PROP_QB_V(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ store_prop_qb<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

ତାର୍କିକ of_STORE_PROP_QF_R(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ store_prop_qf<ଦ୍ୱିଗୁଣ, vvp_queue_real>(thr, cp, 0);
}

ତାର୍କିକ of_STORE_PROP_QF_STR(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ store_prop_qf<string, vvp_queue_string>(thr, cp, 0);
}

ତାର୍କିକ of_STORE_PROP_QF_V(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ store_prop_qf<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

template <typename ELEM, ଶ୍ରେଣୀ QTYPE>
ସ୍ଥିର ତାର୍କିକ qinsert_prop(vthread_t thr, vvp_code_t cp, unsigned wid)
{
      int64_t idx = thr->ଶବ୍ଦଗୁଡ଼ିକ[3].w_int;
      ELEM value;
      size_t pid = cp->ସଂଖ୍ୟା;
      କାଢ଼_ମୂଲ୍ୟ(thr, value, wid);

      vvp_object_t& ଶୀର୍ଷ = thr->peek_object();
      vvp_cobject*cobj = ଶୀର୍ଷ.peek<vvp_cobject>();
      ପ୍ରତିଜ୍ଞା(cobj);

      QTYPE* queue = get_queue_prop<QTYPE>(cobj, pid);
      ପ୍ରତିଜ୍ଞା(queue);
      ଯଦି (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at a negative "
	         << get_queue_type(value)
	         << " index (" << idx << "). ";
	    print_queue_ମୂଲ୍ୟ(value);
	    cerr << " was not added." << endl;
      } ଅନ୍ୟଥା ଯଦି (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at an undefined "
	         << get_queue_type(value) << " index. ";
	    print_queue_ମୂଲ୍ୟ(value);
	    cerr << " was not added." << endl;
      } ଅନ୍ୟଥା {
	    unsigned max_size = thr->ଶବ୍ଦଗୁଡ଼ିକ[cp->bit_idx[0]].w_int;
	    queue->insert(idx, value, max_size);
      }
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_QINSERT_PROP_R(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ qinsert_prop<ଦ୍ୱିଗୁଣ, vvp_queue_real>(thr, cp, 0);
}

ତାର୍କିକ of_QINSERT_PROP_STR(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ qinsert_prop<string, vvp_queue_string>(thr, cp, 0);
}

ତାର୍କିକ of_QINSERT_PROP_V(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ qinsert_prop<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

template <typename ELEM, ଶ୍ରେଣୀ QTYPE>
ସ୍ଥିର ତାର୍କିକ q_କାଢ଼_prop(vthread_t thr, vvp_code_t cp,
                       ଶୂନ୍ୟ (*get_val_func)(vvp_queue*, ELEM&),
                       ଅଚଳ ଅକ୍ଷର*loc, unsigned wid)
{
      size_t pid = cp->ସଂଖ୍ୟା;

      vvp_object_t& ଶୀର୍ଷ = thr->peek_object();
      vvp_cobject*cobj = ଶୀର୍ଷ.peek<vvp_cobject>();
      ପ୍ରତିଜ୍ଞା(cobj);

      QTYPE* queue = get_queue_prop<QTYPE>(cobj, pid);
      ପ୍ରତିଜ୍ଞା(queue);

      size_t size = queue->get_size();

      ELEM value;
      ଯଦି (size) {
	    get_val_func(queue, value);
      } ଅନ୍ୟଥା {
	    dq_default(value, wid);
	    cerr << thr->get_fileline()
	         << "Warning: pop_" << loc << "() on empty "
	         << get_queue_type(value) << "." << endl;
      }

      ଠେଲ_ମୂଲ୍ୟ(thr, value, wid);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

template <typename ELEM, ଶ୍ରେଣୀ QTYPE>
ସ୍ଥିର ତାର୍କିକ qpop_b_prop(vthread_t thr, vvp_code_t cp, unsigned wid)
{
      ଫେରନ୍ତୁ q_କାଢ଼_prop<ELEM, QTYPE>(thr, cp, get_back_ମୂଲ୍ୟ<ELEM>, "back", wid);
}

template <typename ELEM, ଶ୍ରେଣୀ QTYPE>
ସ୍ଥିର ତାର୍କିକ qpop_f_prop(vthread_t thr, vvp_code_t cp, unsigned wid)
{
      ଫେରନ୍ତୁ q_କାଢ଼_prop<ELEM, QTYPE>(thr, cp, get_front_ମୂଲ୍ୟ<ELEM>, "front", wid);
}

ତାର୍କିକ of_QPOP_PROP_B_REAL(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ qpop_b_prop<ଦ୍ୱିଗୁଣ, vvp_queue_real>(thr, cp, 0);
}

ତାର୍କିକ of_QPOP_PROP_B_STR(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ qpop_b_prop<string, vvp_queue_string>(thr, cp, 0);
}

ତାର୍କିକ of_QPOP_PROP_B_V(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ qpop_b_prop<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[0]);
}

ତାର୍କିକ of_QPOP_PROP_F_REAL(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ qpop_f_prop<ଦ୍ୱିଗୁଣ, vvp_queue_real>(thr, cp, 0);
}

ତାର୍କିକ of_QPOP_PROP_F_STR(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ qpop_f_prop<string, vvp_queue_string>(thr, cp, 0);
}

ତାର୍କିକ of_QPOP_PROP_F_V(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ qpop_f_prop<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[0]);
}

ସ୍ଥିର vvp_vector4_t queue_unique_ulong_to_vec4(unsigned long ul, unsigned bits)
{
      vvp_vector4_t val(bits, BIT4_0);
      ପାଇଁ (unsigned idx = 0; idx < bits && idx < 8 * ଆକାର(unsigned long); idx += 1) {
	    ଯଦି (ul & 1UL) {
		  val.set_bit(idx, BIT4_1);
	    } ଅନ୍ୟଥା {
		  val.set_bit(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      ଫେରନ୍ତୁ val;
}

template <ଶ୍ରେଣୀ SRC> ସ୍ଥିର vvp_queue_vec4* queue_run_unique_src(SRC* src,
								 unsigned wid,
								 ତାର୍କିକ as_index)
{
      vvp_queue_vec4* dst = ନୂତନ vvp_queue_vec4();
      ଯଦି (!src) ଫେରନ୍ତୁ dst;
      size_t n = src->get_size();
      ପାଇଁ (size_t i = 0; i < n; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    ତାର୍କିକ seen = ମିଥ୍ୟା;
	    ପାଇଁ (size_t j = 0; j < i; j += 1) {
		  vvp_vector4_t vj(wid);
		  src->get_word(j, vj);
		  ଯଦି (vi.eeq(vj)) {
			seen = ସତ୍ୟ;
			ଭାଙ୍ଗ;
		  }
	    }
	    ଯଦି (seen) ଜାରି;
	    ଯଦି (as_index) {
		  dst->push_back(queue_unique_ulong_to_vec4((unsigned long)i, 32), 0);
	    } ଅନ୍ୟଥା {
		  dst->push_back(vi, 0);
	    }
      }
      ଫେରନ୍ତୁ dst;
}

template <ଶ୍ରେଣୀ SRC>
ସ୍ଥିର vvp_queue_vec4* queue_run_find_matches_src(SRC* src, unsigned wid,
						  ଅଚଳ vvp_vector4_t& cmp,
						  ତାର୍କିକ as_index)
{
      vvp_queue_vec4* dst = ନୂତନ vvp_queue_vec4();
      ଯଦି (!src) ଫେରନ୍ତୁ dst;
      size_t n = src->get_size();
      ପାଇଁ (size_t i = 0; i < n; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    ଯଦି (vi.eeq(cmp)) {
		  ଯଦି (as_index) {
			dst->push_back(queue_unique_ulong_to_vec4((unsigned long)i, 32), 0);
		  } ଅନ୍ୟଥା {
			dst->push_back(vi, 0);
		  }
	    }
      }
      ଫେରନ୍ତୁ dst;
}

/* Array/queue locator methods return a queue with 0 or 1 elements (LRM). */
template <ଶ୍ରେଣୀ SRC>
ସ୍ଥିର vvp_queue_vec4* queue_find_first_last_to_queue_src(SRC* src,
							  unsigned wid,
							  ଅଚଳ vvp_vector4_t& cmp,
							  ତାର୍କିକ want_first,
							  ତାର୍କିକ want_index)
{
      vvp_queue_vec4* dst = ନୂତନ vvp_queue_vec4();
      ଯଦି (!src || src->get_size() == 0) ଫେରନ୍ତୁ dst;
      size_t n = src->get_size();
      ଯଦି (want_first) {
	    ପାଇଁ (size_t i = 0; i < n; i += 1) {
		  vvp_vector4_t vi(wid);
		  src->get_word(i, vi);
		  ଯଦି (vi.eeq(cmp)) {
			ଯଦି (want_index) {
			      dst->push_back(
				  queue_unique_ulong_to_vec4((unsigned long)i, 32),
				  0);
			} ଅନ୍ୟଥା {
			      dst->push_back(vi, 0);
			}
			ଫେରନ୍ତୁ dst;
		  }
	    }
      } ଅନ୍ୟଥା {
	    ପାଇଁ (size_t i = n; i > 0; ) {
		  i -= 1;
		  vvp_vector4_t vi(wid);
		  src->get_word(i, vi);
		  ଯଦି (vi.eeq(cmp)) {
			ଯଦି (want_index) {
			      dst->push_back(
				  queue_unique_ulong_to_vec4((unsigned long)i, 32),
				  0);
			} ଅନ୍ୟଥା {
			      dst->push_back(vi, 0);
			}
			ଫେରନ୍ତୁ dst;
		  }
	    }
      }
      ଫେରନ୍ତୁ dst;
}

/* min()/max() return one element with the selected extremum. */
template <ଶ୍ରେଣୀ SRC>
ସ୍ଥିର vvp_queue_vec4* queue_run_min_max_src(SRC* src, unsigned wid, ତାର୍କିକ want_max)
{
      vvp_queue_vec4* dst = ନୂତନ vvp_queue_vec4();
      ଯଦି (!src || src->get_size() == 0) ଫେରନ୍ତୁ dst;

      vvp_vector4_t best(wid);
      src->get_word(0, best);

      size_t n = src->get_size();
      ପାଇଁ (size_t i = 1 ; i < n ; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);

	    vvp_bit4_t ge = compare_gtge(vi, best, BIT4_1);
	    ତାର୍କିକ replace = want_max ? (ge == BIT4_1) : (ge == BIT4_0);
	    ଯଦି (replace) best = vi;
      }

      dst->push_back(best, 0);
      ଫେରନ୍ତୁ dst;
}

ତାର୍କିକ of_QUEUE_FIND_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->କାଢ଼_vec4();
      unsigned wid = cp->bit_idx[0];
      ପ୍ରତିଜ୍ଞା(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      ଯଦି (qsrc) dst = queue_run_find_matches_src(qsrc, wid, cmp, ମିଥ୍ୟା);
      ଅନ୍ୟଥା ଯଦି (dsrc) dst = queue_run_find_matches_src(dsrc, wid, cmp, ମିଥ୍ୟା);
      ଅନ୍ୟଥା {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_run_find_matches_src(src, wid, cmp, ମିଥ୍ୟା);
      }
      thr->ଠେଲ_object(vvp_object_t(dst));
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_QUEUE_FIND_PROP_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->କାଢ଼_vec4();
      size_t pid = cp->ସଂଖ୍ୟା;
      unsigned wid = cp->bit_idx[0];
      ପ୍ରତିଜ୍ଞା(cmp.size() == wid);

      vvp_object_t& ଶୀର୍ଷ = thr->peek_object();
      vvp_cobject*cobj = ଶୀର୍ଷ.peek<vvp_cobject>();
      ପ୍ରତିଜ୍ଞା(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_find_matches_src(qsrc, wid, cmp, ମିଥ୍ୟା)
				 : queue_run_find_matches_src(dsrc, wid, cmp, ମିଥ୍ୟା);
      thr->ଠେଲ_object(vvp_object_t(dst));
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_QUEUE_FIND_INDEX_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->କାଢ଼_vec4();
      unsigned wid = cp->bit_idx[0];
      ପ୍ରତିଜ୍ଞା(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      ଯଦି (qsrc) dst = queue_run_find_matches_src(qsrc, wid, cmp, ସତ୍ୟ);
      ଅନ୍ୟଥା ଯଦି (dsrc) dst = queue_run_find_matches_src(dsrc, wid, cmp, ସତ୍ୟ);
      ଅନ୍ୟଥା {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_run_find_matches_src(src, wid, cmp, ସତ୍ୟ);
      }
      thr->ଠେଲ_object(vvp_object_t(dst));
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_QUEUE_FIND_INDEX_PROP_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->କାଢ଼_vec4();
      size_t pid = cp->ସଂଖ୍ୟା;
      unsigned wid = cp->bit_idx[0];
      ପ୍ରତିଜ୍ଞା(cmp.size() == wid);

      vvp_object_t& ଶୀର୍ଷ = thr->peek_object();
      vvp_cobject*cobj = ଶୀର୍ଷ.peek<vvp_cobject>();
      ପ୍ରତିଜ୍ଞା(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_find_matches_src(qsrc, wid, cmp, ସତ୍ୟ)
				 : queue_run_find_matches_src(dsrc, wid, cmp, ସତ୍ୟ);
      thr->ଠେଲ_object(vvp_object_t(dst));
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_QUEUE_FIND_FIRST_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->କାଢ଼_vec4();
      unsigned wid = cp->bit_idx[0];
      ପ୍ରତିଜ୍ଞା(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      ଯଦି (qsrc) dst = queue_find_first_last_to_queue_src(qsrc, wid, cmp, ସତ୍ୟ, ମିଥ୍ୟା);
      ଅନ୍ୟଥା ଯଦି (dsrc) dst = queue_find_first_last_to_queue_src(dsrc, wid, cmp, ସତ୍ୟ, ମିଥ୍ୟା);
      ଅନ୍ୟଥା {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_find_first_last_to_queue_src(src, wid, cmp, ସତ୍ୟ, ମିଥ୍ୟା);
      }
      thr->ଠେଲ_object(vvp_object_t(dst));
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_QUEUE_FIND_FIRST_PROP_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->କାଢ଼_vec4();
      size_t pid = cp->ସଂଖ୍ୟା;
      unsigned wid = cp->bit_idx[0];
      ପ୍ରତିଜ୍ଞା(cmp.size() == wid);

      vvp_object_t& ଶୀର୍ଷ = thr->peek_object();
      vvp_cobject*cobj = ଶୀର୍ଷ.peek<vvp_cobject>();
      ପ୍ରତିଜ୍ଞା(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst =
	    qsrc ? queue_find_first_last_to_queue_src(qsrc, wid, cmp, ସତ୍ୟ, ମିଥ୍ୟା)
		 : queue_find_first_last_to_queue_src(dsrc, wid, cmp, ସତ୍ୟ, ମିଥ୍ୟା);
      thr->ଠେଲ_object(vvp_object_t(dst));
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_QUEUE_FIND_FIRST_INDEX_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->କାଢ଼_vec4();
      unsigned wid = cp->bit_idx[0];
      ପ୍ରତିଜ୍ଞା(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      ଯଦି (qsrc) dst = queue_find_first_last_to_queue_src(qsrc, wid, cmp, ସତ୍ୟ, ସତ୍ୟ);
      ଅନ୍ୟଥା ଯଦି (dsrc) dst = queue_find_first_last_to_queue_src(dsrc, wid, cmp, ସତ୍ୟ, ସତ୍ୟ);
      ଅନ୍ୟଥା {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_find_first_last_to_queue_src(src, wid, cmp, ସତ୍ୟ, ସତ୍ୟ);
      }
      thr->ଠେଲ_object(vvp_object_t(dst));
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_QUEUE_FIND_FIRST_INDEX_PROP_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->କାଢ଼_vec4();
      size_t pid = cp->ସଂଖ୍ୟା;
      unsigned wid = cp->bit_idx[0];
      ପ୍ରତିଜ୍ଞା(cmp.size() == wid);

      vvp_object_t& ଶୀର୍ଷ = thr->peek_object();
      vvp_cobject*cobj = ଶୀର୍ଷ.peek<vvp_cobject>();
      ପ୍ରତିଜ୍ଞା(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst =
	    qsrc ? queue_find_first_last_to_queue_src(qsrc, wid, cmp, ସତ୍ୟ, ସତ୍ୟ)
		 : queue_find_first_last_to_queue_src(dsrc, wid, cmp, ସତ୍ୟ, ସତ୍ୟ);
      thr->ଠେଲ_object(vvp_object_t(dst));
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_QUEUE_FIND_LAST_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->କାଢ଼_vec4();
      unsigned wid = cp->bit_idx[0];
      ପ୍ରତିଜ୍ଞା(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      ଯଦି (qsrc) dst = queue_find_first_last_to_queue_src(qsrc, wid, cmp, ମିଥ୍ୟା, ମିଥ୍ୟା);
      ଅନ୍ୟଥା ଯଦି (dsrc) dst = queue_find_first_last_to_queue_src(dsrc, wid, cmp, ମିଥ୍ୟା, ମିଥ୍ୟା);
      ଅନ୍ୟଥା {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_find_first_last_to_queue_src(src, wid, cmp, ମିଥ୍ୟା, ମିଥ୍ୟା);
      }
      thr->ଠେଲ_object(vvp_object_t(dst));
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_QUEUE_FIND_LAST_PROP_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->କାଢ଼_vec4();
      size_t pid = cp->ସଂଖ୍ୟା;
      unsigned wid = cp->bit_idx[0];
      ପ୍ରତିଜ୍ଞା(cmp.size() == wid);

      vvp_object_t& ଶୀର୍ଷ = thr->peek_object();
      vvp_cobject*cobj = ଶୀର୍ଷ.peek<vvp_cobject>();
      ପ୍ରତିଜ୍ଞା(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst =
	    qsrc ? queue_find_first_last_to_queue_src(qsrc, wid, cmp, ମିଥ୍ୟା, ମିଥ୍ୟା)
		 : queue_find_first_last_to_queue_src(dsrc, wid, cmp, ମିଥ୍ୟା, ମିଥ୍ୟା);
      thr->ଠେଲ_object(vvp_object_t(dst));
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_QUEUE_FIND_LAST_INDEX_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->କାଢ଼_vec4();
      unsigned wid = cp->bit_idx[0];
      ପ୍ରତିଜ୍ଞା(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      ଯଦି (qsrc) dst = queue_find_first_last_to_queue_src(qsrc, wid, cmp, ମିଥ୍ୟା, ସତ୍ୟ);
      ଅନ୍ୟଥା ଯଦି (dsrc) dst = queue_find_first_last_to_queue_src(dsrc, wid, cmp, ମିଥ୍ୟା, ସତ୍ୟ);
      ଅନ୍ୟଥା {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_find_first_last_to_queue_src(src, wid, cmp, ମିଥ୍ୟା, ସତ୍ୟ);
      }
      thr->ଠେଲ_object(vvp_object_t(dst));
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_QUEUE_FIND_LAST_INDEX_PROP_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->କାଢ଼_vec4();
      size_t pid = cp->ସଂଖ୍ୟା;
      unsigned wid = cp->bit_idx[0];
      ପ୍ରତିଜ୍ଞା(cmp.size() == wid);

      vvp_object_t& ଶୀର୍ଷ = thr->peek_object();
      vvp_cobject*cobj = ଶୀର୍ଷ.peek<vvp_cobject>();
      ପ୍ରତିଜ୍ଞା(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst =
	    qsrc ? queue_find_first_last_to_queue_src(qsrc, wid, cmp, ମିଥ୍ୟା, ସତ୍ୟ)
		 : queue_find_first_last_to_queue_src(dsrc, wid, cmp, ମିଥ୍ୟା, ସତ୍ୟ);
      thr->ଠେଲ_object(vvp_object_t(dst));
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_QUEUE_MIN_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      ଯଦି (qsrc) dst = queue_run_min_max_src(qsrc, wid, ମିଥ୍ୟା);
      ଅନ୍ୟଥା ଯଦି (dsrc) dst = queue_run_min_max_src(dsrc, wid, ମିଥ୍ୟା);
      ଅନ୍ୟଥା {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_run_min_max_src(src, wid, ମିଥ୍ୟା);
      }
      thr->ଠେଲ_object(vvp_object_t(dst));
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_QUEUE_MIN_OBJ_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->କାଢ଼_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_min_max_src(qsrc, wid, ମିଥ୍ୟା)
				 : queue_run_min_max_src(dsrc, wid, ମିଥ୍ୟା);
      thr->ଠେଲ_object(vvp_object_t(dst));
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_QUEUE_MIN_PROP_V(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->ସଂଖ୍ୟା;
      unsigned wid = cp->bit_idx[0];

      vvp_object_t& ଶୀର୍ଷ = thr->peek_object();
      vvp_cobject*cobj = ଶୀର୍ଷ.peek<vvp_cobject>();
      ପ୍ରତିଜ୍ଞା(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_min_max_src(qsrc, wid, ମିଥ୍ୟା)
				 : queue_run_min_max_src(dsrc, wid, ମିଥ୍ୟା);
      thr->ଠେଲ_object(vvp_object_t(dst));
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_QUEUE_MAX_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      ଯଦି (qsrc) dst = queue_run_min_max_src(qsrc, wid, ସତ୍ୟ);
      ଅନ୍ୟଥା ଯଦି (dsrc) dst = queue_run_min_max_src(dsrc, wid, ସତ୍ୟ);
      ଅନ୍ୟଥା {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_run_min_max_src(src, wid, ସତ୍ୟ);
      }
      thr->ଠେଲ_object(vvp_object_t(dst));
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_QUEUE_MAX_OBJ_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->କାଢ଼_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_min_max_src(qsrc, wid, ସତ୍ୟ)
				 : queue_run_min_max_src(dsrc, wid, ସତ୍ୟ);
      thr->ଠେଲ_object(vvp_object_t(dst));
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_QUEUE_MAX_PROP_V(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->ସଂଖ୍ୟା;
      unsigned wid = cp->bit_idx[0];

      vvp_object_t& ଶୀର୍ଷ = thr->peek_object();
      vvp_cobject*cobj = ଶୀର୍ଷ.peek<vvp_cobject>();
      ପ୍ରତିଜ୍ଞା(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_min_max_src(qsrc, wid, ସତ୍ୟ)
				 : queue_run_min_max_src(dsrc, wid, ସତ୍ୟ);
      thr->ଠେଲ_object(vvp_object_t(dst));
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_QUEUE_UNIQUE_OBJ_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->କାଢ଼_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_unique_src(qsrc, wid, ମିଥ୍ୟା)
				 : queue_run_unique_src(dsrc, wid, ମିଥ୍ୟା);
      thr->ଠେଲ_object(vvp_object_t(dst));
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_QUEUE_UNIQUE_INDEX_OBJ_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->କାଢ଼_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_unique_src(qsrc, wid, ସତ୍ୟ)
				 : queue_run_unique_src(dsrc, wid, ସତ୍ୟ);
      thr->ଠେଲ_object(vvp_object_t(dst));
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_QUEUE_UNIQUE_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      ଯଦି (qsrc) dst = queue_run_unique_src(qsrc, wid, ମିଥ୍ୟା);
      ଅନ୍ୟଥା ଯଦି (dsrc) dst = queue_run_unique_src(dsrc, wid, ମିଥ୍ୟା);
      ଅନ୍ୟଥା {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_run_unique_src(src, wid, ମିଥ୍ୟା);
      }
      thr->ଠେଲ_object(vvp_object_t(dst));
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_QUEUE_UNIQUE_PROP_V(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->ସଂଖ୍ୟା;
      unsigned wid = cp->bit_idx[0];

      vvp_object_t& ଶୀର୍ଷ = thr->peek_object();
      vvp_cobject*cobj = ଶୀର୍ଷ.peek<vvp_cobject>();
      ପ୍ରତିଜ୍ଞା(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_unique_src(qsrc, wid, ମିଥ୍ୟା)
				 : queue_run_unique_src(dsrc, wid, ମିଥ୍ୟା);
      thr->ଠେଲ_object(vvp_object_t(dst));
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_QUEUE_UNIQUE_INDEX_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      ଯଦି (qsrc) dst = queue_run_unique_src(qsrc, wid, ସତ୍ୟ);
      ଅନ୍ୟଥା ଯଦି (dsrc) dst = queue_run_unique_src(dsrc, wid, ସତ୍ୟ);
      ଅନ୍ୟଥା {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_run_unique_src(src, wid, ସତ୍ୟ);
      }
      thr->ଠେଲ_object(vvp_object_t(dst));
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_QUEUE_UNIQUE_INDEX_PROP_V(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->ସଂଖ୍ୟା;
      unsigned wid = cp->bit_idx[0];

      vvp_object_t& ଶୀର୍ଷ = thr->peek_object();
      vvp_cobject*cobj = ଶୀର୍ଷ.peek<vvp_cobject>();
      ପ୍ରତିଜ୍ଞା(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_unique_src(qsrc, wid, ସତ୍ୟ)
				 : queue_run_unique_src(dsrc, wid, ସତ୍ୟ);
      thr->ଠେଲ_object(vvp_object_t(dst));
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_PROP_QUEUE_SIZE(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->ସଂଖ୍ୟା;

      vvp_object_t& ଶୀର୍ଷ = thr->peek_object();
      vvp_cobject*cobj = ଶୀର୍ଷ.peek<vvp_cobject>();
      ପ୍ରତିଜ୍ଞା(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);

      size_t sz = qsrc ? qsrc->get_size() : (dsrc ? dsrc->get_size() : 0);

      vvp_vector4_t val(32, BIT4_0);
      unsigned long ul = sz;
      ପାଇଁ (unsigned idx = 0; idx < 32; idx++) {
	    ଯଦି (ul & 1UL) {
		  val.set_bit(idx, BIT4_1);
	    } ଅନ୍ୟଥା {
		  val.set_bit(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      thr->ଠେଲ_vec4(val);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_QUEUE_SIZE_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      size_t sz =
	    qsrc ? qsrc->get_size() : (dsrc ? dsrc->get_size() : 0);

      vvp_vector4_t val(32, BIT4_0);
      unsigned long ul = sz;
      ପାଇଁ (unsigned idx = 0; idx < 32; idx++) {
	    ଯଦି (ul & 1UL) {
		  val.set_bit(idx, BIT4_1);
	    } ଅନ୍ୟଥା {
		  val.set_bit(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      thr->ଠେଲ_vec4(val);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_QUEUE_WORD_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      unsigned ix_reg = cp->bit_idx[1];
      int64_t adr = thr->ଶବ୍ଦଗୁଡ଼ିକ[ix_reg].w_int;
      ଯଦି (thr->flags[4] == BIT4_1) {
	    thr->ଠେଲ_vec4(vvp_vector4_t(wid));
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }
      ଯଦି (adr < 0) {
	    thr->ଠେଲ_vec4(vvp_vector4_t(wid));
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t out(wid);
      ଯଦି (qsrc) {
	    ଯଦି ((size_t)adr >= qsrc->get_size()) {
		  thr->ଠେଲ_vec4(out);
		  ଫେରନ୍ତୁ ସତ୍ୟ;
	    }
	    qsrc->get_word((unsigned)adr, out);
      } ଅନ୍ୟଥା ଯଦି (dsrc) {
	    ଯଦି ((size_t)adr >= dsrc->get_size()) {
		  thr->ଠେଲ_vec4(out);
		  ଫେରନ୍ତୁ ସତ୍ୟ;
	    }
	    dsrc->get_word((unsigned)adr, out);
      } ଅନ୍ୟଥା {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    ଯଦି (!src || (size_t)adr >= src->get_size()) {
		  thr->ଠେଲ_vec4(out);
		  ଫେରନ୍ତୁ ସତ୍ୟ;
	    }
	    src->get_word((unsigned)adr, out);
      }
      thr->ଠେଲ_vec4(out);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_QUEUE_WORD_PROP_V(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->ସଂଖ୍ୟା;
      unsigned wid = cp->bit_idx[0];
      unsigned ix_reg = cp->bit_idx[1];
      int64_t adr = thr->ଶବ୍ଦଗୁଡ଼ିକ[ix_reg].w_int;
      ଯଦି (thr->flags[4] == BIT4_1) {
	    thr->ଠେଲ_vec4(vvp_vector4_t(wid));
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }
      ଯଦି (adr < 0) {
	    thr->ଠେଲ_vec4(vvp_vector4_t(wid));
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }

      vvp_object_t saved_ଶୀର୍ଷ;
      ତାର୍କିକ popped_ଶୀର୍ଷ = ମିଥ୍ୟା;
      vvp_object_t& ଶୀର୍ଷ = thr->peek_object();
      vvp_cobject*cobj = ଶୀର୍ଷ.peek<vvp_cobject>();
      ଯଦି (cobj == 0) {
	    // Array-locator with(property) keeps an accumulator queue on top.
	    // Temporarily pop it to access the class object beneath.
	    thr->କାଢ଼_object(saved_ଶୀର୍ଷ);
	    popped_ଶୀର୍ଷ = ସତ୍ୟ;
	    vvp_object_t& below = thr->peek_object();
	    cobj = below.peek<vvp_cobject>();
      }
      ଯଦି (cobj == 0) {
	    ଯଦି (popped_ଶୀର୍ଷ) thr->ଠେଲ_object(saved_ଶୀର୍ଷ);
	    thr->ଠେଲ_vec4(vvp_vector4_t(wid));
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t out(wid);
      ଯଦି (qsrc) {
	    ଯଦି ((size_t)adr >= qsrc->get_size()) {
		  thr->ଠେଲ_vec4(out);
		  ଫେରନ୍ତୁ ସତ୍ୟ;
	    }
	    qsrc->get_word((unsigned)adr, out);
      } ଅନ୍ୟଥା ଯଦି (dsrc) {
	    ଯଦି ((size_t)adr >= dsrc->get_size()) {
		  thr->ଠେଲ_vec4(out);
		  ଫେରନ୍ତୁ ସତ୍ୟ;
	    }
	    dsrc->get_word((unsigned)adr, out);
      } ଅନ୍ୟଥା {
	    thr->ଠେଲ_vec4(out);
	    ଯଦି (popped_ଶୀର୍ଷ) thr->ଠେଲ_object(saved_ଶୀର୍ଷ);
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }
      ଯଦି (popped_ଶୀର୍ଷ) thr->ଠେଲ_object(saved_ଶୀର୍ଷ);
      thr->ଠେଲ_vec4(out);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_CMPIX_LTU(vthread_t thr, vvp_code_t cp)
{
      unsigned a = cp->ସଂଖ୍ୟା;
      unsigned b = cp->bit_idx[0];
      ଯଦି (thr->ଶବ୍ଦଗୁଡ଼ିକ[a].w_uint < thr->ଶବ୍ଦଗୁଡ଼ିକ[b].w_uint) {
	    thr->flags[4] = BIT4_1;
      } ଅନ୍ୟଥା {
	    thr->flags[4] = BIT4_0;
      }
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_CMPIX_SLT0(vthread_t thr, vvp_code_t cp)
{
      unsigned r = cp->ସଂଖ୍ୟା;
      thr->flags[4] = (thr->ଶବ୍ଦଗୁଡ଼ିକ[r].w_int < 0) ? BIT4_1 : BIT4_0;
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_ଠେଲ_IX_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned ix = cp->ସଂଖ୍ୟା;
      unsigned wid = cp->bit_idx[0];
      unsigned sgn = cp->bit_idx[1];
      uint32_t ubits;
      ଯଦି (sgn) {
	    int32_t sv = (int32_t)thr->ଶବ୍ଦଗୁଡ଼ିକ[ix].w_int;
	    ubits = (uint32_t)sv;
      } ଅନ୍ୟଥା {
	    ubits = (uint32_t)thr->ଶବ୍ଦଗୁଡ଼ିକ[ix].w_uint;
      }
      ଯଦି (wid > 32) wid = 32;
      vvp_vector4_t tmp(wid, BIT4_0);
      ପାଇଁ (unsigned idx = 0; idx < wid; idx++) {
	    ଯଦି (ubits & 1u) {
		  tmp.set_bit(idx, BIT4_1);
	    } ଅନ୍ୟଥା {
		  tmp.set_bit(idx, BIT4_0);
	    }
	    ubits >>= 1;
      }
      thr->ଠେଲ_vec4(tmp);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_QUEUE_NEW_EMPTY_V(vthread_t thr, vvp_code_t)
{
      thr->ଠେଲ_object(vvp_object_t(ନୂତନ vvp_queue_vec4()));
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_QUEUE_APPEND_WORD_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      vvp_vector4_t elem = thr->କାଢ଼_vec4();
      ପ୍ରତିଜ୍ଞା(elem.size() == wid);
      vvp_object_t qobj;
      thr->କାଢ଼_object(qobj);
      vvp_queue_vec4* q = qobj.peek<vvp_queue_vec4>();
      ପ୍ରତିଜ୍ଞା(q);
      q->push_back(elem, 0);
      thr->ଠେଲ_object(qobj);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_DELETE_PROP_ELEM(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->ସଂଖ୍ୟା;

      int64_t idx_val = thr->ଶବ୍ଦଗୁଡ଼ିକ[3].w_int;
      ଯଦି (thr->flags[4] == BIT4_1) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with undefined index."
	         << endl;
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }
      ଯଦି (idx_val < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with negative index."
	         << endl;
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }
      size_t idx = idx_val;

      vvp_object_t& ଶୀର୍ଷ = thr->peek_object();
      vvp_cobject*cobj = ଶୀର୍ଷ.peek<vvp_cobject>();
      ପ୍ରତିଜ୍ଞା(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue* queue = qobj.peek<vvp_queue>();
      ଯଦି (queue == 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping delete(" << idx
	         << ") on empty queue." << endl;
      } ଅନ୍ୟଥା {
	    size_t size = queue->get_size();
	    ଯଦି (idx >= size) {
		  cerr << thr->get_fileline()
		       << "Warning: skipping out of range delete(" << idx
		       << ") on queue of size " << size << "." << endl;
	    } ଅନ୍ୟଥା {
		  queue->erase(idx);
	    }
      }

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_DELETE_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->ସଂଖ୍ୟା;

      vvp_object_t& ଶୀର୍ଷ = thr->peek_object();
      vvp_cobject*cobj = ଶୀର୍ଷ.peek<vvp_cobject>();
      ପ୍ରତିଜ୍ଞା(cobj);

      cobj->set_object(pid, vvp_object_t(), 0);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

template <typename ELEM, ଶ୍ରେଣୀ QTYPE>
ସ୍ଥିର ତାର୍କିକ store_qb(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
      ELEM value;
      vvp_net_t*net = cp->net;
      unsigned max_size = thr->ଶବ୍ଦଗୁଡ଼ିକ[cp->bit_idx[0]].w_int;
      କାଢ଼_ମୂଲ୍ୟ(thr, value, wid); // Pop the value to store.

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      ପ୍ରତିଜ୍ଞା(queue);
      queue->push_back(value, max_size);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %store/qb/r <var-label>, <max-idx>
 */
ତାର୍କିକ of_STORE_QB_R(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ store_qb<ଦ୍ୱିଗୁଣ, vvp_queue_real>(thr, cp);
}

/*
 * %store/qb/str <var-label>, <max-idx>
 */
ତାର୍କିକ of_STORE_QB_STR(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ store_qb<string, vvp_queue_string>(thr, cp);
}

/*
 * %store/qb/v <var-label>, <max-idx>, <wid>
 */
ତାର୍କିକ of_STORE_QB_V(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ store_qb<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

template <typename ELEM, ଶ୍ରେଣୀ QTYPE>
ସ୍ଥିର ତାର୍କିକ store_qdar(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
      int64_t idx = thr->ଶବ୍ଦଗୁଡ଼ିକ[3].w_int;
      ELEM value;
      vvp_net_t*net = cp->net;
      କାଢ଼_ମୂଲ୍ୟ(thr, value, wid); // Pop the value to store.

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      ପ୍ରତିଜ୍ଞା(queue);
      ଯଦି (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot assign to a negative "
	         << get_queue_type(value)
	         << " index (" << idx << "). ";
	    print_queue_ମୂଲ୍ୟ(value);
	    cerr << " was not added." << endl;
      } ଅନ୍ୟଥା ଯଦି (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot assign to an undefined "
	         << get_queue_type(value) << " index. ";
	    print_queue_ମୂଲ୍ୟ(value);
	    cerr << " was not added." << endl;
      } ଅନ୍ୟଥା {
	    unsigned max_size = thr->ଶବ୍ଦଗୁଡ଼ିକ[cp->bit_idx[0]].w_int;
	    queue->set_word_max(idx, value, max_size);
      }
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %store/qdar/r <var>, idx
 */
ତାର୍କିକ of_STORE_QDAR_R(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ store_qdar<ଦ୍ୱିଗୁଣ, vvp_queue_real>(thr, cp);
}

/*
 * %store/qdar/str <var>, idx
 */
ତାର୍କିକ of_STORE_QDAR_STR(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ store_qdar<string, vvp_queue_string>(thr, cp);
}

/*
 * %store/qdar/v <var>, idx
 */
ତାର୍କିକ of_STORE_QDAR_V(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ store_qdar<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

template <typename ELEM, ଶ୍ରେଣୀ QTYPE>
ସ୍ଥିର ତାର୍କିକ store_qf(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
      ELEM value;
      vvp_net_t*net = cp->net;
      unsigned max_size = thr->ଶବ୍ଦଗୁଡ଼ିକ[cp->bit_idx[0]].w_int;
      କାଢ଼_ମୂଲ୍ୟ(thr, value, wid); // Pop the value to store.

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      ପ୍ରତିଜ୍ଞା(queue);
      queue->push_front(value, max_size);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}
/*
 * %store/qf/r <var-label>, <max-idx>
 */
ତାର୍କିକ of_STORE_QF_R(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ store_qf<ଦ୍ୱିଗୁଣ, vvp_queue_real>(thr, cp);
}

/*
 * %store/qf/str <var-label>, <max-idx>
 */
ତାର୍କିକ of_STORE_QF_STR(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ store_qf<string, vvp_queue_string>(thr, cp);
}

/*
 * %store/qb/v <var-label>, <max-idx>, <wid>
 */
ତାର୍କିକ of_STORE_QF_V(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ store_qf<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

template <typename ELEM, ଶ୍ରେଣୀ QTYPE>
ସ୍ଥିର ତାର୍କିକ store_qobj(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
// FIXME: Can we actually use wid here?
      (ଶୂନ୍ୟ)wid;
      vvp_net_t*net = cp->net;

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      ପ୍ରତିଜ୍ଞା(queue);

      vvp_object_t src;
      thr->କାଢ଼_object(src);

        // If it is null just clear the queue
      ଯଦି (src.test_nil())
	    queue->erase_tail(0);
      ଅନ୍ୟଥା {
	    unsigned max_size = thr->ଶବ୍ଦଗୁଡ଼ିକ[cp->bit_idx[0]].w_int;
	    queue->copy_elems(src, max_size);
      }

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_STORE_QOBJ_R(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ store_qobj<ଦ୍ୱିଗୁଣ, vvp_queue_real>(thr, cp);
}

ତାର୍କିକ of_STORE_QOBJ_STR(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ store_qobj<string, vvp_queue_string>(thr, cp);
}

ତାର୍କିକ of_STORE_QOBJ_V(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ store_qobj<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

ସ୍ଥିର ଶୂନ୍ୟ vvp_send(vthread_t thr, vvp_net_ptr_t ptr, ଅଚଳ ଦ୍ୱିଗୁଣ&val)
{
      vvp_send_real(ptr, val, thr->wt_context);
}

ସ୍ଥିର ଶୂନ୍ୟ vvp_send(vthread_t thr, vvp_net_ptr_t ptr, ଅଚଳ string&val)
{
      vvp_send_string(ptr, val, thr->wt_context);
}

template <typename ELEM>
ସ୍ଥିର ତାର୍କିକ store(vthread_t thr, vvp_code_t cp)
{
      ELEM val;
      କାଢ଼_ମୂଲ୍ୟ(thr, val, 0);
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_send(thr, ptr, val);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_STORE_REAL(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ store<ଦ୍ୱିଗୁଣ>(thr, cp);
}

template <typename ELEM>
ସ୍ଥିର ତାର୍କିକ storea(vthread_t thr, vvp_code_t cp)
{
      unsigned idx = cp->bit_idx[0];
      ELEM val;
      କାଢ଼_ମୂଲ୍ୟ(thr, val, 0);

      ଯଦି (thr->flags[4] != BIT4_1) {
	    unsigned adr = thr->ଶବ୍ଦଗୁଡ଼ିକ[idx].w_int;
	    cp->array->set_word(adr, val);
      }

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %store/reala <var-label> <index>
 */
ତାର୍କିକ of_STORE_REALA(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ storea<ଦ୍ୱିଗୁଣ>(thr, cp);
}

ତାର୍କିକ of_STORE_STR(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ store<string>(thr, cp);
}

/*
 * %store/stra <array-label> <index>
 */
ତାର୍କିକ of_STORE_STRA(vthread_t thr, vvp_code_t cp)
{
      ଫେରନ୍ତୁ storea<string>(thr, cp);
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
ତାର୍କିକ of_STORE_VEC4(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr(cp->net, 0);
      ଅଚଳ vvp_signal_ମୂଲ୍ୟ*sig = dynamic_cast<vvp_signal_ମୂଲ୍ୟ*> (cp->net->fil);
      unsigned off_index = cp->bit_idx[0];
      unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା wid = cp->bit_idx[1];

      int64_t off = off_index ? thr->ଶବ୍ଦଗୁଡ଼ିକ[off_index].w_int : 0;
      unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା sig_ମୂଲ୍ୟ_size = sig->ମୂଲ୍ୟ_size();

      vvp_vector4_t&val = thr->peek_vec4();
      unsigned val_size = val.size();

      ଯଦି (val_size < wid) {
	    cerr << thr->get_fileline()
	         << "XXXX Internal error: val.size()=" << val_size
		 << ", expecting >= " << wid << endl;
      }
      ପ୍ରତିଜ୍ଞା(val_size >= wid);
      ଯଦି (val_size > wid) {
	    val.resize(wid);
      }

	// If there is a problem loading the index register, flags-4
	// will be set to 1, and we know here to skip the actual assignment.
      ଯଦି (off_index!=0 && thr->flags[4] == BIT4_1) {
	    thr->କାଢ଼_vec4(1);
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }

      ଯଦି (!resize_rval_vec(val, off, sig_ମୂଲ୍ୟ_size)) {
	    thr->କାଢ଼_vec4(1);
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }

      ଯଦି (off == 0 && val.size() == sig_ମୂଲ୍ୟ_size)
	    vvp_send_vec4(ptr, val, thr->wt_context);
      ଅନ୍ୟଥା
	    vvp_send_vec4_pv(ptr, val, off, sig_ମୂଲ୍ୟ_size, thr->wt_context);

      thr->କାଢ଼_vec4(1);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %store/vec4a <var-label>, <addr>, <offset>
 */
ତାର୍କିକ of_STORE_VEC4A(vthread_t thr, vvp_code_t cp)
{
      unsigned adr_index = cp->bit_idx[0];
      unsigned off_index = cp->bit_idx[1];

      long adr = adr_index? thr->ଶବ୍ଦଗୁଡ଼ିକ[adr_index].w_int : 0;
      int64_t off = off_index ? thr->ଶବ୍ଦଗୁଡ଼ିକ[off_index].w_int : 0;

	// Suppress action if flags-4 is true.
      ଯଦି (thr->flags[4] == BIT4_1) {
	    thr->କାଢ଼_vec4(1);
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }

      vvp_vector4_t &value = thr->peek_vec4();

      ଯଦି (!resize_rval_vec(value, off, cp->array->get_word_size())) {
	    thr->କାଢ଼_vec4(1);
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }

      cp->array->set_word(adr, off, value);

      thr->କାଢ଼_vec4(1);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %sub
 *   pop r;
 *   pop l;
 *   push l-r;
 */
ତାର୍କିକ of_SUB(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t r = thr->କାଢ଼_vec4();
      vvp_vector4_t&l = thr->peek_vec4();

      l.sub(r);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %subi <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
ତାର୍କିକ of_SUBI(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->ସଂଖ୍ୟା;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_immediate_rval (cp, r);

      l.sub(r);

      ଫେରନ୍ତୁ ସତ୍ୟ;

}

ତାର୍କିକ of_SUB_WR(vthread_t thr, vvp_code_t)
{
      ଦ୍ୱିଗୁଣ r = thr->କାଢ଼_real();
      ଦ୍ୱିଗୁଣ l = thr->କାଢ଼_real();
      thr->ଠେଲ_real(l - r);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %substr <first>, <last>
 * Pop a string, take the substring (SystemVerilog style), and return
 * the result to the stack. This opcode actually works by editing the
 * string in place.
 */
ତାର୍କିକ of_SUBSTR(vthread_t thr, vvp_code_t cp)
{
      int32_t first = thr->ଶବ୍ଦଗୁଡ଼ିକ[cp->bit_idx[0]].w_int;
      int32_t last = thr->ଶବ୍ଦଗୁଡ଼ିକ[cp->bit_idx[1]].w_int;
      string&val = thr->peek_str(0);

      ଯଦି (first < 0 || last < first || last >= (int32_t)val.size()) {
	    val = string("");
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }

      val = val.substr(first, last-first+1);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %substr/vec4 <index>, <wid>
 */
ତାର୍କିକ of_SUBSTR_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned sel_idx = cp->bit_idx[0];
      unsigned wid = cp->bit_idx[1];

      int32_t sel = thr->ଶବ୍ଦଗୁଡ଼ିକ[sel_idx].w_int;
      ଅଚଳ string&val = thr->peek_str(0);

      ପ୍ରତିଜ୍ଞା(wid%8 == 0);

      ଯଦି (sel < 0 || sel >= (int32_t)val.size()) {
	    vvp_vector4_t res (wid, BIT4_0);
	    thr->ଠେଲ_vec4(res);
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }

      vvp_vector4_t res (wid, BIT4_0);

      ପ୍ରତିଜ୍ଞା(wid==8);
      unsigned ଅକ୍ଷର tmp = val[sel];
      ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = 0 ; idx < 8 ; idx += 1) {
	    ଯଦି (tmp & (1<<idx))
		  res.set_bit(idx, BIT4_1);
      }

      thr->ଠେଲ_vec4(res);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_FILE_ଧାଡ଼ି(vthread_t thr, vvp_code_t cp)
{
      vpiHandle handle = cp->handle;

	/* When it is available, keep the file/line information in the
	   thread for error/warning messages. */
      thr->set_fileline(vpi_get_str(vpiFile, handle),
                        vpi_get(vpi_ଧାଡ଼ି_No, handle));

      ଯଦି (show_file_ଧାଡ଼ି)
	    cerr << thr->get_fileline()
	         << vpi_get_str(_vpiDescription, handle) << endl;

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %test_nul <var-label>;
 * Test if the object at the specified variable is nil. If so, write
 * "1" into flags[4], otherwise write "0" into flags[4].
 */
ତାର୍କିକ of_TEST_NUL(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      ପ୍ରତିଜ୍ଞା(net);
      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      ପ୍ରତିଜ୍ଞା(obj);

      ଯଦି (obj->get_object().test_nil())
	    thr->flags[4] = BIT4_1;
      ଅନ୍ୟଥା
	    thr->flags[4] = BIT4_0;

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_TEST_NUL_A(vthread_t thr, vvp_code_t cp)
{
      unsigned idx = cp->bit_idx[0];
      unsigned adr = thr->ଶବ୍ଦଗୁଡ଼ିକ[idx].w_int;
      vvp_object_t word;

	/* If the address is undefined, return true. */
      ଯଦି (thr->flags[4] == BIT4_1) {
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }

      cp->array->get_word_obj(adr, word);
      ଯଦି (word.test_nil())
	    thr->flags[4] = BIT4_1;
      ଅନ୍ୟଥା
	    thr->flags[4] = BIT4_0;

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_TEST_NUL_OBJ(vthread_t thr, vvp_code_t)
{
      ଯଦି (thr->peek_object().test_nil())
	    thr->flags[4] = BIT4_1;
      ଅନ୍ୟଥା
	    thr->flags[4] = BIT4_0;
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %test_nul/prop <pid>, <idx>
 */
ତାର୍କିକ of_TEST_NUL_PROP(vthread_t thr, vvp_code_t cp)
{
      unsigned pid = cp->ସଂଖ୍ୟା;
      unsigned idx = cp->bit_idx[0];

      ଯଦି (idx != 0) {
	    ପ୍ରତିଜ୍ଞା(idx < vthread_s::ଶବ୍ଦଗୁଡ଼ିକ_ଗଣନା);
	    idx = thr->ଶବ୍ଦଗୁଡ଼ିକ[idx].w_uint;
      }

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj  = obj.peek<vvp_cobject>();

      vvp_object_t val;
      cobj->get_object(pid, val, idx);

      ଯଦି (val.test_nil())
	    thr->flags[4] = BIT4_1;
      ଅନ୍ୟଥା
	    thr->flags[4] = BIT4_0;

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/* and() reduction returns the scalar sum (Verilog-style bitwise-AND per word). */
template <ଶ୍ରେଣୀ SRC>
ସ୍ଥିର vvp_vector4_t uparr_and_ଶବ୍ଦଗୁଡ଼ିକ_src(SRC* src, unsigned wid)
{
      ଯଦି (!src) ଫେରନ୍ତୁ vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_1);
      ପାଇଁ (size_t i = 0; i < src->get_size(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    res &= vi;
      }
      ଫେରନ୍ତୁ res;
}

ତାର୍କିକ of_UPARR_AND_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      ଯଦି (qsrc) res = uparr_and_ଶବ୍ଦଗୁଡ଼ିକ_src(qsrc, wid);
      ଅନ୍ୟଥା ଯଦି (dsrc) res = uparr_and_ଶବ୍ଦଗୁଡ଼ିକ_src(dsrc, wid);
      ଅନ୍ୟଥା {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    res = uparr_and_ଶବ୍ଦଗୁଡ଼ିକ_src(src, wid);
      }
      thr->ଠେଲ_vec4(res);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/* or() reduction returns the scalar sum (Verilog-style bitwise-AND per word). */
template <ଶ୍ରେଣୀ SRC>
ସ୍ଥିର vvp_vector4_t uparr_or_ଶବ୍ଦଗୁଡ଼ିକ_src(SRC* src, unsigned wid)
{
      ଯଦି (!src) ଫେରନ୍ତୁ vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_0);
      ପାଇଁ (size_t i = 0; i < src->get_size(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    res |= vi;
      }
      ଫେରନ୍ତୁ res;
}

ତାର୍କିକ of_UPARR_OR_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      ଯଦି (qsrc) res = uparr_or_ଶବ୍ଦଗୁଡ଼ିକ_src(qsrc, wid);
      ଅନ୍ୟଥା ଯଦି (dsrc) res = uparr_or_ଶବ୍ଦଗୁଡ଼ିକ_src(dsrc, wid);
      ଅନ୍ୟଥା {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    res = uparr_or_ଶବ୍ଦଗୁଡ଼ିକ_src(src, wid);
      }
      thr->ଠେଲ_vec4(res);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/* real product() reduction returns the scalar sum */
template <ଶ୍ରେଣୀ SRC>
ସ୍ଥିର ଦ୍ୱିଗୁଣ uparr_product_ଶବ୍ଦଗୁଡ଼ିକ_src_real(SRC* src)
{
      ଯଦି (!src) ଫେରନ୍ତୁ 0.0;
      ଦ୍ୱିଗୁଣ acc = 1.0;
      ପାଇଁ (size_t i = 0; i < src->get_size(); i += 1) {
	    ଦ୍ୱିଗୁଣ vi;
	    src->get_word(i, vi);
	    acc *= vi;
      }
      ଫେରନ୍ତୁ acc;
}

ତାର୍କିକ of_UPARR_PRODUCT_REAL(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_queue_real* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_real_from_net(thr, net, qsrc, dsrc);
      ଦ୍ୱିଗୁଣ sum;
      ଯଦି (qsrc) sum = uparr_product_ଶବ୍ଦଗୁଡ଼ିକ_src_real(qsrc);
      ଅନ୍ୟଥା ଯଦି (dsrc) sum = uparr_product_ଶବ୍ଦଗୁଡ଼ିକ_src_real(dsrc);
      ଅନ୍ୟଥା {
	    vvp_queue* src_q = get_queue_object<vvp_queue_real>(thr, net);
	    vvp_queue_real* src = dynamic_cast<vvp_queue_real*>(src_q);
	    sum = uparr_product_ଶବ୍ଦଗୁଡ଼ିକ_src_real(src);
      }
      thr->ଠେଲ_real(sum);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/* product() reduction returns the scalar product (Verilog-style multiplication per word). */
template <ଶ୍ରେଣୀ SRC>
ସ୍ଥିର vvp_vector4_t uparr_product_ଶବ୍ଦଗୁଡ଼ିକ_src(SRC* src, unsigned wid)
{
      ଯଦି (!src) ଫେରନ୍ତୁ vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t acc = queue_unique_ulong_to_vec4(1UL, wid);
      ପାଇଁ (size_t i = 0; i < src->get_size(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    acc.mul(vi);
      }
      ଫେରନ୍ତୁ acc;
}

ତାର୍କିକ of_UPARR_PRODUCT_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t prod;
      ଯଦି (qsrc) prod = uparr_product_ଶବ୍ଦଗୁଡ଼ିକ_src(qsrc, wid);
      ଅନ୍ୟଥା ଯଦି (dsrc) prod = uparr_product_ଶବ୍ଦଗୁଡ଼ିକ_src(dsrc, wid);
      ଅନ୍ୟଥା {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    prod = uparr_product_ଶବ୍ଦଗୁଡ଼ିକ_src(src, wid);
      }
      thr->ଠେଲ_vec4(prod);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_UPARR_PRODUCT_PROP_V(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->ସଂଖ୍ୟା;
      unsigned wid = cp->bit_idx[0];

      vvp_object_t& ଶୀର୍ଷ = thr->peek_object();
      vvp_cobject*cobj = ଶୀର୍ଷ.peek<vvp_cobject>();
      ପ୍ରତିଜ୍ଞା(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t prod =
	    qsrc ? uparr_product_ଶବ୍ଦଗୁଡ଼ିକ_src(qsrc, wid)
		 : uparr_product_ଶବ୍ଦଗୁଡ଼ିକ_src(dsrc, wid);
      thr->ଠେଲ_vec4(prod);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_UPARR_PRODUCT_OBJ_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->କାଢ଼_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_vector4_t prod =
	    qsrc ? uparr_product_ଶବ୍ଦଗୁଡ଼ିକ_src(qsrc, wid)
		 : uparr_product_ଶବ୍ଦଗୁଡ଼ିକ_src(dsrc, wid);
      thr->ଠେଲ_vec4(prod);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/* real sum() reduction returns the scalar sum */
template <ଶ୍ରେଣୀ SRC>
ସ୍ଥିର ଦ୍ୱିଗୁଣ uparr_sum_ଶବ୍ଦଗୁଡ଼ିକ_src_real(SRC* src)
{
      ଦ୍ୱିଗୁଣ acc = 0.0;
      ଯଦି (!src) ଫେରନ୍ତୁ acc;
      ପାଇଁ (size_t i = 0; i < src->get_size(); i += 1) {
	    ଦ୍ୱିଗୁଣ vi;
	    src->get_word(i, vi);
	    acc += vi;
      }
      ଫେରନ୍ତୁ acc;
}

ତାର୍କିକ of_UPARR_SUM_REAL(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_queue_real* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_real_from_net(thr, net, qsrc, dsrc);
      ଦ୍ୱିଗୁଣ sum;
      ଯଦି (qsrc) sum = uparr_sum_ଶବ୍ଦଗୁଡ଼ିକ_src_real(qsrc);
      ଅନ୍ୟଥା ଯଦି (dsrc) sum = uparr_sum_ଶବ୍ଦଗୁଡ଼ିକ_src_real(dsrc);
      ଅନ୍ୟଥା {
	    vvp_queue* src_q = get_queue_object<vvp_queue_real>(thr, net);
	    vvp_queue_real* src = dynamic_cast<vvp_queue_real*>(src_q);
	    sum = uparr_sum_ଶବ୍ଦଗୁଡ଼ିକ_src_real(src);
      }
      thr->ଠେଲ_real(sum);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/* sum() reduction returns the scalar sum (Verilog-style addition per word). */
template <ଶ୍ରେଣୀ SRC>
ସ୍ଥିର vvp_vector4_t uparr_sum_ଶବ୍ଦଗୁଡ଼ିକ_src(SRC* src, unsigned wid)
{
      ଯଦି (!src) ଫେରନ୍ତୁ vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t acc(wid, BIT4_0);
      ପାଇଁ (size_t i = 0; i < src->get_size(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    acc.add(vi);
      }
      ଫେରନ୍ତୁ acc;
}

ତାର୍କିକ of_UPARR_SUM_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t sum;
      ଯଦି (qsrc) sum = uparr_sum_ଶବ୍ଦଗୁଡ଼ିକ_src(qsrc, wid);
      ଅନ୍ୟଥା ଯଦି (dsrc) sum = uparr_sum_ଶବ୍ଦଗୁଡ଼ିକ_src(dsrc, wid);
      ଅନ୍ୟଥା {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    sum = uparr_sum_ଶବ୍ଦଗୁଡ଼ିକ_src(src, wid);
      }
      thr->ଠେଲ_vec4(sum);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_UPARR_SUM_PROP_V(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->ସଂଖ୍ୟା;
      unsigned wid = cp->bit_idx[0];

      vvp_object_t& ଶୀର୍ଷ = thr->peek_object();
      vvp_cobject*cobj = ଶୀର୍ଷ.peek<vvp_cobject>();
      ପ୍ରତିଜ୍ଞା(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t sum =
	    qsrc ? uparr_sum_ଶବ୍ଦଗୁଡ଼ିକ_src(qsrc, wid)
		 : uparr_sum_ଶବ୍ଦଗୁଡ଼ିକ_src(dsrc, wid);
      thr->ଠେଲ_vec4(sum);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_UPARR_SUM_OBJ_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->କାଢ଼_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_vector4_t sum =
	    qsrc ? uparr_sum_ଶବ୍ଦଗୁଡ଼ିକ_src(qsrc, wid)
		 : uparr_sum_ଶବ୍ଦଗୁଡ଼ିକ_src(dsrc, wid);
      thr->ଠେଲ_vec4(sum);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/* xor() reduction returns the scalar sum (Verilog-style bitwise-XOR per word). */
template <ଶ୍ରେଣୀ SRC>
ସ୍ଥିର vvp_vector4_t uparr_xor_ଶବ୍ଦଗୁଡ଼ିକ_src(SRC* src, unsigned wid)
{
      ଯଦି (!src) ଫେରନ୍ତୁ vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_0);
      ପାଇଁ (size_t i = 0; i < src->get_size(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    res ^= vi;
      }
      ଫେରନ୍ତୁ res;
}

ତାର୍କିକ of_UPARR_XOR_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      ଯଦି (qsrc) res = uparr_xor_ଶବ୍ଦଗୁଡ଼ିକ_src(qsrc, wid);
      ଅନ୍ୟଥା ଯଦି (dsrc) res = uparr_xor_ଶବ୍ଦଗୁଡ଼ିକ_src(dsrc, wid);
      ଅନ୍ୟଥା {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    res = uparr_xor_ଶବ୍ଦଗୁଡ଼ିକ_src(src, wid);
      }
      thr->ଠେଲ_vec4(res);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ of_VPI_CALL(vthread_t thr, vvp_code_t cp)
{
      vpip_execute_vpi_call(thr, cp->handle);

      ଯଦି (schedule_stopped()) {
	    ଯଦି (! schedule_finished())
		  schedule_vthread(thr, 0, ମିଥ୍ୟା);

	    ଫେରନ୍ତୁ ମିଥ୍ୟା;
      }

      ଫେରନ୍ତୁ schedule_finished()? ମିଥ୍ୟା : ସତ୍ୟ;
}

/* %wait <label>;
 * Implement the wait by locating the vvp_net_T for the event, and
 * adding this thread to the threads list for the event. The some
 * argument is the  reference to the functor to wait for. This must be
 * an event object of some sort.
 */
ତାର୍କିକ of_WAIT(vthread_t thr, vvp_code_t cp)
{
      ପ୍ରତିଜ୍ଞା(! thr->i_am_in_function);
      ପ୍ରତିଜ୍ଞା(! thr->waiting_for_event);
      thr->waiting_for_event = 1;

	/* Add this thread to the list in the event. */
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (cp->net->fun);
      ପ୍ରତିଜ୍ଞା(ep);
      thr->wait_next = ep->add_waiting_thread(thr);

	/* Return false to suspend this thread. */
      ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

/*
 * Implement the %wait/fork (SystemVerilog) instruction by suspending
 * the current thread until all the detached children have finished.
 */
ତାର୍କିକ of_WAIT_FORK(vthread_t thr, vvp_code_t)
{
	/* If a %wait/fork is being executed then the parent thread
	 * cannot be waiting in a join or already waiting. */
      ପ୍ରତିଜ୍ଞା(! thr->i_am_in_function);
      ପ୍ରତିଜ୍ଞା(! thr->i_am_joining);
      ପ୍ରତିଜ୍ଞା(! thr->i_am_waiting);

	/* There should be no active children when waiting. */
      ପ୍ରତିଜ୍ଞା(thr->children.empty());

	/* If there are no detached children then there is nothing to
	 * wait for. */
      ଯଦି (thr->detached_children.empty()) ଫେରନ୍ତୁ ସତ୍ୟ;

	/* Flag that this process is waiting for the detached children
	 * to finish and suspend it. */
      thr->i_am_waiting = 1;
      ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

/*
 * %xnor
 */
ତାର୍କିକ of_XNOR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valr = thr->କାଢ଼_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      ପ୍ରତିଜ୍ଞା(vall.size() == valr.size());

      vall ^= valr;
      vall.invert();

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * %xor
 */
ତାର୍କିକ of_XOR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valr = thr->କାଢ଼_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      ପ୍ରତିଜ୍ଞା(vall.size() == valr.size());

      vall ^= valr;

      ଫେରନ୍ତୁ ସତ୍ୟ;
}


ତାର୍କିକ of_ZOMBIE(vthread_t thr, vvp_code_t)
{
      thr->pc = codespace_null();
      ଯଦି ((thr->parent == 0) && (thr->children.empty())) {
	    ଯଦି (thr->delay_delete)
		  schedule_del_thr(thr);
	    ଅନ୍ୟଥା
		  vthread_delete(thr);
      }
      ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

/*
 * This is a phantom opcode used to call user defined functions. It
 * is used in code generated by the .ufunc statement. It contains a
 * pointer to the executable code of the function and a pointer to
 * a ufunc_core object that has all the port information about the
 * function.
 */
ସ୍ଥିର ତାର୍କିକ do_exec_ufunc(vthread_t thr, vvp_code_t cp, vthread_t child)
{
      __vpiScope*child_scope = cp->ufunc_core_ptr->func_scope();
      ପ୍ରତିଜ୍ଞା(child_scope);

      ପ୍ରତିଜ୍ଞା(child_scope->get_type_code() == vpiFunction);
      ପ୍ରତିଜ୍ଞା(thr->children.empty());


        /* We can take a number of shortcuts because we know that a
           continuous assignment can only occur in a static scope. */
      ପ୍ରତିଜ୍ଞା(thr->wt_context == 0);
      ପ୍ରତିଜ୍ଞା(thr->rd_context == 0);

        /* If an automatic function, allocate a context for this call. */
      vvp_context_t child_context = 0;
      ଯଦି (child_scope->is_automatic()) {
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
      ପ୍ରତିଜ୍ଞା(thr->children.size()==1);

      child->is_scheduled = 1;
      child->i_am_in_function = 1;
      vthread_run(child);
      running_thread = thr;

      ଯଦି (child->i_have_ended) {
	    do_join(thr, child);
            ଫେରନ୍ତୁ ସତ୍ୟ;
      } ଅନ୍ୟଥା {
	    thr->i_am_joining = 1;
	    ଫେରନ୍ତୁ ମିଥ୍ୟା;
      }
}

ତାର୍କିକ of_EXEC_UFUNC_REAL(vthread_t thr, vvp_code_t cp)
{
      __vpiScope*child_scope = cp->ufunc_core_ptr->func_scope();
      ପ୍ରତିଜ୍ଞା(child_scope);

	/* Create a temporary thread and run it immediately. */
      vthread_t child = vthread_new(cp->cptr, child_scope);
      thr->ଠେଲ_real(0.0);
      child->args_real.push_back(0);

      ଫେରନ୍ତୁ do_exec_ufunc(thr, cp, child);
}

ତାର୍କିକ of_EXEC_UFUNC_VEC4(vthread_t thr, vvp_code_t cp)
{
      __vpiScope*child_scope = cp->ufunc_core_ptr->func_scope();
      ପ୍ରତିଜ୍ଞା(child_scope);

      vpiScopeFunction*scope_func = dynamic_cast<vpiScopeFunction*>(child_scope);
      ପ୍ରତିଜ୍ଞା(scope_func);

	/* Create a temporary thread and run it immediately. */
      vthread_t child = vthread_new(cp->cptr, child_scope);
      thr->ଠେଲ_vec4(vvp_vector4_t(scope_func->get_func_width(), scope_func->get_func_init_val()));
      child->args_vec4.push_back(0);

      ଫେରନ୍ତୁ do_exec_ufunc(thr, cp, child);
}

/*
 * This is a phantom opcode used to harvest the result of calling a user
 * defined function. It is used in code generated by the .ufunc statement.
 */
ତାର୍କିକ of_REAP_UFUNC(vthread_t thr, vvp_code_t cp)
{
      __vpiScope*child_scope = cp->ufunc_core_ptr->func_scope();
      ପ୍ରତିଜ୍ଞା(child_scope);

	/* Copy the output from the result variable to the output
	   ports of the .ufunc device. */
      cp->ufunc_core_ptr->finish_thread();

        /* If an automatic function, free the context for this call. */
      ଯଦି (child_scope->is_automatic()) {
            vthread_free_context(thr->rd_context, child_scope);
            thr->wt_context = 0;
            thr->rd_context = 0;
      }

      ଫେରନ୍ତୁ ସତ୍ୟ;
}
