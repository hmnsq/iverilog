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

using નામાકાશ std;

/* This is the size of an unsigned long in bits. This is just a
   convenience macro. */
# define CPU_WORD_BITS (8*કદ(unsigned long))
# define ટોચ_BIT (1UL << (CPU_WORD_BITS-1))

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

સંરચના vthread_s {
      vthread_s();

      શૂન્ય debug_dump(ostream&fd, અચલ અક્ષર*label_text);

	/* This is the program counter. */
      vvp_code_t pc;
	/* These hold the private thread bits. */
      ગણના { FLAGS_ગણતરી = 512, શબ્દો_ગણતરી = 16 };
      vvp_bit4_t flags[FLAGS_ગણતરી];

	/* These are the word registers. */
      સંઘ {
	    int64_t  w_int;
	    uint64_t w_uint;
      } શબ્દો[શબ્દો_ગણતરી];

	// These vectors are depths within the parent thread's
	// corresponding stack.  This is how the %ret/* instructions
	// get at parent thread arguments.
      vector<unsigned> args_real;
      vector<unsigned> args_str;
      vector<unsigned> args_vec4;

    ખાનગી:
      vector<vvp_vector4_t>ઢગલો_vec4;
    જાહેર:
      અંતઃરેખ vvp_vector4_t કાઢો_vec4(શૂન્ય)
      {
	    પ્રતિજ્ઞા(! ઢગલો_vec4.empty());
	    vvp_vector4_t val = ઢગલો_vec4.back();
	    ઢગલો_vec4.pop_back();
	    પાછા val;
      }
      અંતઃરેખ શૂન્ય ધકેલો_vec4(અચલ vvp_vector4_t&val)
      {
	    ઢગલો_vec4.push_back(val);
      }
      અંતઃરેખ અચલ vvp_vector4_t& peek_vec4(unsigned depth)
      {
	    unsigned size = ઢગલો_vec4.size();
	    પ્રતિજ્ઞા(depth < size);
	    unsigned use_index = size-1-depth;
	    પાછા ઢગલો_vec4[use_index];
      }
      અંતઃરેખ vvp_vector4_t& peek_vec4(શૂન્ય)
      {
	    unsigned use_index = ઢગલો_vec4.size();
	    પ્રતિજ્ઞા(use_index >= 1);
	    પાછા ઢગલો_vec4[use_index-1];
      }
      અંતઃરેખ શૂન્ય poke_vec4(unsigned depth, અચલ vvp_vector4_t&val)
      {
	    પ્રતિજ્ઞા(depth < ઢગલો_vec4.size());
	    unsigned use_index = ઢગલો_vec4.size()-1-depth;
	    ઢગલો_vec4[use_index] = val;
      }
      અંતઃરેખ શૂન્ય કાઢો_vec4(unsigned cnt)
      {
	    જ્યારે (cnt > 0) {
		  ઢગલો_vec4.pop_back();
		  cnt -= 1;
	    }
      }


    ખાનગી:
      vector<દ્વિગુણ> ઢગલો_real;
    જાહેર:
      અંતઃરેખ દ્વિગુણ કાઢો_real(શૂન્ય)
      {
	    પ્રતિજ્ઞા(! ઢગલો_real.empty());
	    દ્વિગુણ val = ઢગલો_real.back();
	    ઢગલો_real.pop_back();
	    પાછા val;
      }
      અંતઃરેખ શૂન્ય ધકેલો_real(દ્વિગુણ val)
      {
	    ઢગલો_real.push_back(val);
      }
      અંતઃરેખ દ્વિગુણ peek_real(unsigned depth)
      {
	    પ્રતિજ્ઞા(depth < ઢગલો_real.size());
	    unsigned use_index = ઢગલો_real.size()-1-depth;
	    પાછા ઢગલો_real[use_index];
      }
      અંતઃરેખ શૂન્ય poke_real(unsigned depth, દ્વિગુણ val)
      {
	    પ્રતિજ્ઞા(depth < ઢગલો_real.size());
	    unsigned use_index = ઢગલો_real.size()-1-depth;
	    ઢગલો_real[use_index] = val;
      }
      અંતઃરેખ શૂન્ય કાઢો_real(unsigned cnt)
      {
	    જ્યારે (cnt > 0) {
		  ઢગલો_real.pop_back();
		  cnt -= 1;
	    }
      }

	/* Strings are operated on using a forth-like operator
	   set. Items at the top of the stack (back()) are the objects
	   operated on except for special cases. New objects are
	   pushed onto the top (back()) and pulled from the top
	   (back()) only. */
    ખાનગી:
      vector<string> ઢગલો_str;
    જાહેર:
      અંતઃરેખ string કાઢો_str(શૂન્ય)
      {
	    પ્રતિજ્ઞા(! ઢગલો_str.empty());
	    string val = ઢગલો_str.back();
	    ઢગલો_str.pop_back();
	    પાછા val;
      }
      અંતઃરેખ શૂન્ય ધકેલો_str(અચલ string&val)
      {
	    ઢગલો_str.push_back(val);
      }
      અંતઃરેખ string&peek_str(unsigned depth)
      {
	    પ્રતિજ્ઞા(depth<ઢગલો_str.size());
	    unsigned use_index = ઢગલો_str.size()-1-depth;
	    પાછા ઢગલો_str[use_index];
      }
      અંતઃરેખ શૂન્ય poke_str(unsigned depth, અચલ string&val)
      {
	    પ્રતિજ્ઞા(depth < ઢગલો_str.size());
	    unsigned use_index = ઢગલો_str.size()-1-depth;
	    ઢગલો_str[use_index] = val;
      }
      અંતઃરેખ શૂન્ય કાઢો_str(unsigned cnt)
      {
	    જ્યારે (cnt > 0) {
		  ઢગલો_str.pop_back();
		  cnt -= 1;
	    }
      }

	/* Objects are also operated on in a stack. */
    ખાનગી:
      ગણના { ઢગલો_OBJ_MAX_SIZE = 32 };
      vvp_object_t ઢગલો_obj[ઢગલો_OBJ_MAX_SIZE];
      unsigned ઢગલો_obj_size;
    જાહેર:
      અંતઃરેખ vvp_object_t& peek_object(શૂન્ય)
      {
	    પ્રતિજ્ઞા(ઢગલો_obj_size > 0);
	    પાછા ઢગલો_obj[ઢગલો_obj_size-1];
      }
      અંતઃરેખ શૂન્ય કાઢો_object(vvp_object_t&obj)
      {
	    પ્રતિજ્ઞા(ઢગલો_obj_size > 0);
	    ઢગલો_obj_size -= 1;
	    obj = ઢગલો_obj[ઢગલો_obj_size];
	    ઢગલો_obj[ઢગલો_obj_size].reset(0);
      }
      અંતઃરેખ શૂન્ય કાઢો_object(unsigned cnt, unsigned skip =0)
      {
	    પ્રતિજ્ઞા((cnt+skip) <= ઢગલો_obj_size);
	    માટે (size_t idx = ઢગલો_obj_size-skip-cnt ; idx < ઢગલો_obj_size-skip ; idx += 1)
		  ઢગલો_obj[idx].reset(0);
	    ઢગલો_obj_size -= cnt;
	    માટે (size_t idx = ઢગલો_obj_size-skip ; idx < ઢગલો_obj_size ; idx += 1)
		  ઢગલો_obj[idx] = ઢગલો_obj[idx+skip];
	    માટે (size_t idx = ઢગલો_obj_size ; idx < ઢગલો_obj_size+skip ; idx += 1)
		  ઢગલો_obj[idx].reset(0);
      }
      અંતઃરેખ શૂન્ય ધકેલો_object(અચલ vvp_object_t&obj)
      {
	    પ્રતિજ્ઞા(ઢગલો_obj_size < ઢગલો_OBJ_MAX_SIZE);
	    ઢગલો_obj[ઢગલો_obj_size] = obj;
	    ઢગલો_obj_size += 1;
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
      set<સંરચના vthread_s*>children;
	/* This points to the detached children of the thread. */
      set<સંરચના vthread_s*>detached_children;
	/* This points to my parent, if I have one. */
      સંરચના vthread_s*parent;
	/* This points to the containing scope. */
      __vpiScope*parent_scope;
	/* This is used for keeping wait queues. */
      સંરચના vthread_s*wait_next;
	/* These are used to access automatically allocated items. */
      vvp_context_t wt_context, rd_context;
	/* These are used to pass non-blocking event control information. */
      vvp_net_t*event;
      uint64_t ecount;
	/* Save the file/line information when available. */
    ખાનગી:
      અક્ષર *filenm_;
      unsigned lineno_;
    જાહેર:
      શૂન્ય set_fileline(અક્ષર *filenm, unsigned lineno);
      string get_fileline();

      અંતઃરેખ શૂન્ય cleanup()
      {
	    જો (i_was_disabled) {
		  ઢગલો_vec4.clear();
		  ઢગલો_real.clear();
		  ઢગલો_str.clear();
		  કાઢો_object(ઢગલો_obj_size);
	    }
	    મુક્ત(filenm_);
	    filenm_ = 0;
	    પ્રતિજ્ઞા(ઢગલો_vec4.empty());
	    પ્રતિજ્ઞા(ઢગલો_real.empty());
	    પ્રતિજ્ઞા(ઢગલો_str.empty());
	    પ્રતિજ્ઞા(ઢગલો_obj_size == 0);
      }
};

અંતઃરેખ vthread_s::vthread_s()
{
      ઢગલો_obj_size = 0;
      filenm_ = 0;
      lineno_ = 0;
}

શૂન્ય vthread_s::set_fileline(અક્ષર *filenm, unsigned lineno)
{
      પ્રતિજ્ઞા(filenm);
      જો (!filenm_ || (strcmp(filenm_, filenm) != 0)) {
	    મુક્ત(filenm_);
	    filenm_ = strdup(filenm);
      }
      lineno_ = lineno;
}

અંતઃરેખ string vthread_s::get_fileline()
{
      ostringstream buf;
      જો (filenm_) {
	    buf << filenm_ << ":" << lineno_ << ": ";
      }
      string res = buf.str();
      પાછા res;
}

શૂન્ય vthread_s::debug_dump(ostream&fd, અચલ અક્ષર*label)
{
      fd << "**** " << label << endl;
      fd << "**** ThreadId: " << આ << ", parent id: " << parent << endl;

      fd << "**** Flags: ";
      માટે (પૂર્ણાંક idx = 0 ; idx < FLAGS_ગણતરી ; idx += 1)
	    fd << flags[idx];
      fd << endl;
      fd << "**** vec4 stack..." << endl;
      માટે (size_t idx = ઢગલો_vec4.size() ; idx > 0 ; idx -= 1)
	    fd << "    " << (ઢગલો_vec4.size()-idx) << ": " << ઢગલો_vec4[idx-1] << endl;
      fd << "**** str stack (" << ઢગલો_str.size() << ")..." << endl;
      fd << "**** obj stack (" << ઢગલો_obj_size << ")..." << endl;
      fd << "**** args_vec4 array (" << args_vec4.size() << ")..." << endl;
      માટે (size_t idx = 0 ; idx < args_vec4.size() ; idx += 1)
	    fd << "    " << idx << ": " << args_vec4[idx] << endl;
      fd << "**** file/line (";
      જો (filenm_) fd << filenm_;
      નહીંતર fd << "<no file name>";
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
સ્થિર string filter_string(અચલ અક્ષર*text)
{
      vector<અક્ષર> tmp (strlen(text)+1);
      size_t dst = 0;
      માટે (અચલ અક્ષર*ptr = text ; *ptr ; ptr += 1) {
	    // Not an escape? Move on.
	    જો (*ptr != '\\') {
		  tmp[dst++] = *ptr;
		  ચાલુ;
	    }

	    // Now we know that *ptr is pointing to a \ character and we
	    // have an octal sequence coming up. Advance the ptr and start
	    // processing octal digits.
	    ptr += 1;
	    જો (*ptr == 0)
		  તોડો;

	    અક્ષર byte = 0;
	    પૂર્ણાંક cnt = 3;
	    જ્યારે (*ptr && cnt > 0 && *ptr >= '0' && *ptr <= '7') {
		  byte *= 8;
		  byte += *ptr - '0';
		  cnt -= 1;
		  ptr += 1;
	    }

	    // null-bytes are supposed to be removed when assigning a string
	    // literal to a string.
	    જો (byte != '\0')
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
      પાછા res;
}

સ્થિર શૂન્ય do_join(vthread_t thr, vthread_t child);

__vpiScope* vthread_scope(સંરચના vthread_s*thr)
{
      પાછા thr->parent_scope;
}

સંરચના vthread_s*running_thread = 0;

string get_fileline()
{
      પાછા running_thread->get_fileline();
}

શૂન્ય vthread_ધકેલો(સંરચના vthread_s*thr, દ્વિગુણ val)
{
      thr->ધકેલો_real(val);
}

શૂન્ય vthread_ધકેલો(સંરચના vthread_s*thr, અચલ string&val)
{
      thr->ધકેલો_str(val);
}

શૂન્ય vthread_ધકેલો(સંરચના vthread_s*thr, અચલ vvp_vector4_t&val)
{
      thr->ધકેલો_vec4(val);
}

શૂન્ય vthread_કાઢો_real(સંરચના vthread_s*thr, unsigned depth)
{
      thr->કાઢો_real(depth);
}

શૂન્ય vthread_કાઢો_str(સંરચના vthread_s*thr, unsigned depth)
{
      thr->કાઢો_str(depth);
}

શૂન્ય vthread_કાઢો_vec4(સંરચના vthread_s*thr, unsigned depth)
{
      thr->કાઢો_vec4(depth);
}

દ્વિગુણ vthread_get_real_ઢગલો(સંરચના vthread_s*thr, unsigned depth)
{
      પાછા thr->peek_real(depth);
}

અચલ string&vthread_get_str_ઢગલો(સંરચના vthread_s*thr, unsigned depth)
{
      પાછા thr->peek_str(depth);
}

અચલ vvp_vector4_t& vthread_get_vec4_ઢગલો(સંરચના vthread_s*thr, unsigned depth)
{
      પાછા thr->peek_vec4(depth);
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
template <વર્ગ VVP_QUEUE> સ્થિર vvp_queue*get_queue_object(vthread_t thr, vvp_net_t*net)
{
      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      પ્રતિજ્ઞા(obj);

      vvp_queue*queue = obj->get_object().peek<vvp_queue>();
      જો (queue == 0) {
	    પ્રતિજ્ઞા(obj->get_object().test_nil());
	    queue = નવું VVP_QUEUE;
	    vvp_object_t val (queue);
	    vvp_net_ptr_t ptr (net, 0);
	    vvp_send_object(ptr, val, thr->wt_context);
      }

      પાછા queue;
}

/*
 * Locator and size/word opcodes use the same VVP net for a queue ($) or a
 * dynamic array ([]). Queues are vvp_queue_vec4; dynamic arrays may be
 * vvp_darray_vec4, vvp_darray_atom<>, etc. (all vvp_darray, but vvp_queue
 * also derives from vvp_darray — test queue vec4 first).
 */
સ્થિર શૂન્ય get_queue_or_darray_vec4_from_net(vthread_t thr, vvp_net_t* net,
					      vvp_queue_vec4*& qsrc,
					      vvp_darray*& dsrc)
{
      vvp_fun_signal_object* obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      પ્રતિજ્ઞા(obj);
      vvp_object_t ટોચ = obj->get_object();
      qsrc = ટોચ.peek<vvp_queue_vec4>();
      જો (qsrc) {
	    dsrc = 0;
	    પાછા;
      }
      vvp_darray* dany = ટોચ.peek<vvp_darray>();
      જો (dany && dynamic_cast<vvp_queue*>(dany) == 0) {
	    dsrc = dany;
	    પાછા;
      }
      જો (ટોચ.test_nil()) {
	    qsrc = નવું vvp_queue_vec4();
	    vvp_object_t val(qsrc);
	    vvp_net_ptr_t ptr(net, 0);
	    vvp_send_object(ptr, val, thr->wt_context);
	    dsrc = 0;
	    પાછા;
      }
      /* e.g. real/string queue — not vec4 queue or plain darray */
      qsrc = 0;
      dsrc = 0;
}

સ્થિર શૂન્ય get_queue_or_darray_real_from_net(vthread_t thr, vvp_net_t* net,
					      vvp_queue_real*& qsrc,
					      vvp_darray*& dsrc)
{
      vvp_fun_signal_object* obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      પ્રતિજ્ઞા(obj);
      vvp_object_t ટોચ = obj->get_object();
      qsrc = ટોચ.peek<vvp_queue_real>();
      જો (qsrc) {
	    dsrc = 0;
	    પાછા;
      }
      vvp_darray* dany = ટોચ.peek<vvp_darray>();
      જો (dany && dynamic_cast<vvp_queue*>(dany) == 0) {
	    dsrc = dany;
	    પાછા;
      }
      જો (ટોચ.test_nil()) {
	    qsrc = નવું vvp_queue_real();
	    vvp_object_t val(qsrc);
	    vvp_net_ptr_t ptr(net, 0);
	    vvp_send_object(ptr, val, thr->wt_context);
	    dsrc = 0;
	    પાછા;
      }
      /* e.g. real/string queue — not vec4 queue or plain darray */
      qsrc = 0;
      dsrc = 0;
}

સ્થિર શૂન્ય get_queue_or_darray_vec4_from_object(vvp_object_t src_obj,
						 vvp_queue_vec4*& qsrc,
						 vvp_darray*& dsrc)
{
      qsrc = src_obj.peek<vvp_queue_vec4>();
      જો (qsrc) {
	    dsrc = 0;
	    પાછા;
      }
      vvp_darray* dany = src_obj.peek<vvp_darray>();
      જો (dany && dynamic_cast<vvp_queue*>(dany) == 0) {
	    dsrc = dany;
	    પાછા;
      }
      qsrc = 0;
      dsrc = 0;
}

/*
 * The following are used to allow a common template to be written for
 * queue real/string/vec4 operations
 */
અંતઃરેખ સ્થિર શૂન્ય કાઢો_કિંમત(vthread_t thr, દ્વિગુણ&value, unsigned)
{
      value = thr->કાઢો_real();
}

અંતઃરેખ સ્થિર શૂન્ય કાઢો_કિંમત(vthread_t thr, string&value, unsigned)
{
      value = thr->કાઢો_str();
}

અંતઃરેખ સ્થિર શૂન્ય કાઢો_કિંમત(vthread_t thr, vvp_vector4_t&value, unsigned wid)
{
      value = thr->કાઢો_vec4();
      પ્રતિજ્ઞા(value.size() == wid);
}

/*
 * The following are used to allow the queue templates to print correctly.
 */
અંતઃરેખ સ્થિર string get_queue_type(દ્વિગુણ&)
{
      પાછા "queue<real>";
}

અંતઃરેખ સ્થિર string get_queue_type(string&)
{
      પાછા "queue<string>";
}

અંતઃરેખ સ્થિર string get_queue_type(અચલ vvp_vector4_t&value)
{
      ostringstream buf;
      buf << "queue<vector[" << value.size() << "]>";
      string res = buf.str();
      પાછા res;
}

અંતઃરેખ સ્થિર શૂન્ય print_queue_કિંમત(દ્વિગુણ value)
{
      cerr << value;
}

અંતઃરેખ સ્થિર શૂન્ય print_queue_કિંમત(અચલ string&value)
{
      cerr << "\"" << value << "\"";
}

અંતઃરેખ સ્થિર શૂન્ય print_queue_કિંમત(અચલ vvp_vector4_t&value)
{
      cerr << value;
}

/*
 * The following are used to get a darray/queue default value.
 */
અંતઃરેખ સ્થિર શૂન્ય dq_default(દ્વિગુણ&value, unsigned)
{
      value = 0.0;
}

અંતઃરેખ સ્થિર શૂન્ય dq_default(string&value, unsigned)
{
      value = "";
}

અંતઃરેખ સ્થિર શૂન્ય dq_default(vvp_vector4_t&value, unsigned wid)
{
      value = vvp_vector4_t(wid);
}


template <વર્ગ T> T coerce_to_width(અચલ T&that, unsigned width)
{
      જો (that.size() == width)
	    પાછા that;

      પ્રતિજ્ઞા(that.size() > width);
      T res (width);
      માટે (unsigned idx = 0 ;  idx < width ;  idx += 1)
	    res.set_bit(idx, that.value(idx));

      પાછા res;
}

/* Explicitly define the vvp_vector4_t version of coerce_to_width(). */
template vvp_vector4_t coerce_to_width(અચલ vvp_vector4_t&that,
                                       unsigned width);


સ્થિર શૂન્ય multiply_array_imm(unsigned long*res, અચલ unsigned long*val,
			       unsigned શબ્દો, unsigned long imm)
{
      માટે (unsigned idx = 0 ; idx < શબ્દો ; idx += 1)
	    res[idx] = 0;

      માટે (unsigned mul_idx = 0 ; mul_idx < શબ્દો ; mul_idx += 1) {
	    unsigned long sum;
	    unsigned long tmp = multiply_with_carry(val[mul_idx], imm, sum);

	    unsigned long carry = 0;
	    res[mul_idx] = add_with_carry(res[mul_idx], tmp, carry);
	    માટે (unsigned add_idx = mul_idx+1 ; add_idx < શબ્દો ; add_idx += 1) {
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
સ્થિર vvp_context_t vthread_alloc_context(__vpiScope*scope)
{
      પ્રતિજ્ઞા(scope->is_automatic());

      vvp_context_t context = scope->free_contexts;
      જો (context) {
            scope->free_contexts = vvp_get_next_context(context);
            માટે (unsigned idx = 0 ; idx < scope->nitem ; idx += 1) {
                  scope->item[idx]->reset_instance(context);
            }
      } નહીંતર {
            context = vvp_allocate_context(scope->nitem);
            માટે (unsigned idx = 0 ; idx < scope->nitem ; idx += 1) {
                  scope->item[idx]->alloc_instance(context);
            }
      }

      vvp_set_next_context(context, scope->live_contexts);
      scope->live_contexts = context;

      પાછા context;
}

/*
 * Free a context previously allocated to a child thread by pushing it
 * onto the freed context stack. Remove it from the list of live contexts
 * in that scope.
 */
સ્થિર શૂન્ય vthread_free_context(vvp_context_t context, __vpiScope*scope)
{
      પ્રતિજ્ઞા(scope->is_automatic());
      પ્રતિજ્ઞા(context);

      જો (context == scope->live_contexts) {
            scope->live_contexts = vvp_get_next_context(context);
      } નહીંતર {
            vvp_context_t tmp = scope->live_contexts;
            જ્યારે (context != vvp_get_next_context(tmp)) {
                  પ્રતિજ્ઞા(tmp);
                  tmp = vvp_get_next_context(tmp);
            }
            vvp_set_next_context(tmp, vvp_get_next_context(context));
      }

      vvp_set_next_context(context, scope->free_contexts);
      scope->free_contexts = context;
}

#ifdef CHECK_WITH_VALGRIND
શૂન્ય contexts_delete(વર્ગ __vpiScope*scope)
{
      vvp_context_t context = scope->free_contexts;

      જ્યારે (context) {
	    scope->free_contexts = vvp_get_next_context(context);
	    માટે (unsigned idx = 0; idx < scope->nitem; idx += 1) {
		  scope->item[idx]->free_instance(context);
	    }
	    મુક્ત(context);
	    context = scope->free_contexts;
      }
      મુક્ત(scope->item);
}
#endif

/*
 * Create a new thread with the given start address.
 */
vthread_t vthread_new(vvp_code_t pc, __vpiScope*scope)
{
      vthread_t thr = નવું સંરચના vthread_s;
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
      માટે (પૂર્ણાંક idx = 4 ; idx < 8 ; idx += 1)
	    thr->flags[idx] = BIT4_X;

      scope->threads .insert(thr);
      પાછા thr;
}

#ifdef CHECK_WITH_VALGRIND
#જો 0
/*
 * These are not currently correct. If you use them you will get
 * double delete messages. There is still a leak related to a
 * waiting event that needs to be investigated.
 */

સ્થિર શૂન્ય wait_next_delete(vthread_t base)
{
      જ્યારે (base) {
	    vthread_t tmp = base->wait_next;
	    delete base;
	    base = tmp;
	    જો (base->waiting_for_event == 0) તોડો;
      }
}

સ્થિર શૂન્ય child_delete(vthread_t base)
{
      જ્યારે (base) {
	    vthread_t tmp = base->child;
	    delete base;
	    base = tmp;
      }
}
#endif

શૂન્ય vthreads_delete(વર્ગ __vpiScope*scope)
{
      માટે (std::set<vthread_t>::iterator cur = scope->threads.begin()
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
સ્થિર શૂન્ય vthread_reap(vthread_t thr)
{
      જો (! thr->children.empty()) {
	    માટે (set<vthread_t>::iterator cur = thr->children.begin()
		       ; cur != thr->children.end() ; ++cur) {
		  vthread_t child = *cur;
		  પ્રતિજ્ઞા(child);
		  પ્રતિજ્ઞા(child->parent == thr);
		  child->parent = thr->parent;
	    }
      }
      જો (! thr->detached_children.empty()) {
	    માટે (set<vthread_t>::iterator cur = thr->detached_children.begin()
		       ; cur != thr->detached_children.end() ; ++cur) {
		  vthread_t child = *cur;
		  પ્રતિજ્ઞા(child);
		  પ્રતિજ્ઞા(child->parent == thr);
		  પ્રતિજ્ઞા(child->i_am_detached);
		  child->parent = 0;
		  child->i_am_detached = 0;
	    }
      }
      જો (thr->parent) {
	      /* assert that the given element was removed. */
	    જો (thr->i_am_detached) {
		  size_t res = thr->parent->detached_children.erase(thr);
		  પ્રતિજ્ઞા(res == 1);
	    } નહીંતર {
		  size_t res = thr->parent->children.erase(thr);
		  પ્રતિજ્ઞા(res == 1);
	    }
      }

      thr->parent = 0;

	// Remove myself from the containing scope if needed.
      thr->parent_scope->threads.erase(thr);

      thr->pc = codespace_null();

	/* If this thread is not scheduled, then is it safe to delete
	   it now. Otherwise, let the schedule event (which will
	   execute the thread at of_ZOMBIE) delete the object. */
      જો ((thr->is_scheduled == 0) && (thr->waiting_for_event == 0)) {
	    પ્રતિજ્ઞા(thr->children.empty());
	    પ્રતિજ્ઞા(thr->wait_next == 0);
	    જો (thr->delay_delete)
		  schedule_del_thr(thr);
	    નહીંતર
		  vthread_delete(thr);
      }
}

શૂન્ય vthread_delete(vthread_t thr)
{
      thr->cleanup();
      delete thr;
}

શૂન્ય vthread_mark_scheduled(vthread_t thr)
{
      જ્યારે (thr != 0) {
	    પ્રતિજ્ઞા(thr->is_scheduled == 0);
	    thr->is_scheduled = 1;
	    thr = thr->wait_next;
      }
}

શૂન્ય vthread_mark_final(vthread_t thr)
{
      /*
       * The behavior in a final thread is the same as in a function. Any
       * child thread will be executed immediately rather than being
       * scheduled.
       */
      જ્યારે (thr != 0) {
	    thr->i_am_in_function = 1;
	    thr = thr->wait_next;
      }
}

શૂન્ય vthread_delay_delete()
{
      જો (running_thread)
	    running_thread->delay_delete = 1;
}

/*
 * This function runs each thread by fetching an instruction,
 * incrementing the PC, and executing the instruction. The thread may
 * be the head of a list, so each thread is run so far as possible.
 */
શૂન્ય vthread_run(vthread_t thr)
{
      જ્યારે (thr != 0) {
	    vthread_t tmp = thr->wait_next;
	    thr->wait_next = 0;

	    પ્રતિજ્ઞા(thr->is_scheduled);
	    thr->is_scheduled = 0;

            running_thread = thr;

	    માટે (;;) {
		  vvp_code_t cp = thr->pc;
		  thr->pc += 1;

		    /* Run the opcode implementation. If the execution of
		       the opcode returns false, then the thread is meant to
		       be paused, so break out of the loop. */
		  તાર્કિક rc = (cp->opcode)(thr, cp);
		  જો (rc == ખોટું)
			તોડો;
	    }

	    thr = tmp;
      }
      running_thread = 0;
}

/*
 * The CHUNK_LINK instruction is a special next pointer for linking
 * chunks of code space. It's like a simplified %jmp.
 */
તાર્કિક of_CHUNK_LINK(vthread_t thr, vvp_code_t code)
{
      પ્રતિજ્ઞા(code->cptr);
      thr->pc = code->cptr;
      પાછા સાચું;
}

/*
 * This is called by an event functor to wake up all the threads on
 * its list. I in fact created that list in the %wait instruction, and
 * I also am certain that the waiting_for_event flag is set.
 */
શૂન્ય vthread_schedule_list(vthread_t thr)
{
      માટે (vthread_t cur = thr ;  cur ;  cur = cur->wait_next) {
	    પ્રતિજ્ઞા(cur->waiting_for_event);
	    cur->waiting_for_event = 0;
      }

      schedule_vthread(thr, 0);
}

vvp_context_t vthread_get_wt_context()
{
      જો (running_thread)
            પાછા running_thread->wt_context;
      નહીંતર
            પાછા 0;
}

vvp_context_t vthread_get_rd_context()
{
      જો (running_thread)
            પાછા running_thread->rd_context;
      નહીંતર
            પાછા 0;
}

vvp_context_item_t vthread_get_wt_context_item(unsigned context_idx)
{
      પ્રતિજ્ઞા(running_thread && running_thread->wt_context);
      પાછા vvp_get_context_item(running_thread->wt_context, context_idx);
}

vvp_context_item_t vthread_get_rd_context_item(unsigned context_idx)
{
      પ્રતિજ્ઞા(running_thread && running_thread->rd_context);
      પાછા vvp_get_context_item(running_thread->rd_context, context_idx);
}

/*
 * %abs/wr
 */
તાર્કિક of_ABS_WR(vthread_t thr, vvp_code_t)
{
      thr->ધકેલો_real( fabs(thr->કાઢો_real()) );
      પાછા સાચું;
}

તાર્કિક of_ALLOC(vthread_t thr, vvp_code_t cp)
{
        /* Allocate a context. */
      vvp_context_t child_context = vthread_alloc_context(cp->scope);

        /* Push the allocated context onto the write context stack. */
      vvp_set_stacked_context(child_context, thr->wt_context);
      thr->wt_context = child_context;

      પાછા સાચું;
}

તાર્કિક of_AND(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valb = thr->કાઢો_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();
      પ્રતિજ્ઞા(vala.size() == valb.size());
      vala &= valb;
      પાછા સાચું;
}

/*
 * This function must ALWAYS be called with the val set to the right
 * size, and initialized with BIT4_0 bits. Certain optimizations rely
 * on that.
 */
સ્થિર શૂન્ય get_immediate_rval(vvp_code_t cp, vvp_vector4_t&val)
{
      uint32_t vala = cp->bit_idx[0];
      uint32_t valb = cp->bit_idx[1];
      unsigned wid  = cp->સંખ્યા;

      જો (valb == 0) {
	      // Special case: if the value is zero, we are done
	      // before we start.
	    જો (vala == 0) પાછા;

	      // Special case: The value has no X/Z bits, so we can
	      // use the setarray method to write the value all at once.
	    unsigned use_wid = 8*કદ(unsigned long);
	    જો (wid < use_wid)
		  use_wid = wid;
	    unsigned long tmp[1];
	    tmp[0] = vala;
	    val.setarray(0, use_wid, tmp);
	    પાછા;
      }

	// The immediate value can be values bigger then 32 bits, but
	// only if the high bits are zero. So at most we need to run
	// through the loop below 32 times. Maybe less, if the target
	// width is less. We don't have to do anything special on that
	// because vala/valb bits will shift away so (vala|valb) will
	// turn to zero at or before 32 shifts.

      માટે (unsigned idx = 0 ; idx < wid && (vala|valb) ; idx += 1) {
	    uint32_t ba = 0;
	      // Convert the vala/valb bits to a ba number that
	      // matches the encoding of the vvp_bit4_t enumeration.
	    ba = (valb & 1) << 1;
	    ba |= vala & 1;

	      // Note that the val is already pre-filled with BIT4_0
	      // bits, os we only need to set non-zero bit values.
	    જો (ba) val.set_bit(idx, (vvp_bit4_t)ba);

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
તાર્કિક of_ADD(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t r = thr->કાઢો_vec4();
	// Rather then pop l, use it directly from the stack. When we
	// assign to 'l', that will edit the top of the stack, which
	// replaces a pop and a pull.
      vvp_vector4_t&l = thr->peek_vec4();

      l.add(r);

      પાછા સાચું;
}

/*
 * %addi <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
તાર્કિક of_ADDI(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->સંખ્યા;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_immediate_rval (cp, r);

      l.add(r);

      પાછા સાચું;
}

/*
 * %add/wr
 */
તાર્કિક of_ADD_WR(vthread_t thr, vvp_code_t)
{
      દ્વિગુણ r = thr->કાઢો_real();
      દ્વિગુણ l = thr->કાઢો_real();
      thr->ધકેલો_real(l + r);
      પાછા સાચું;
}

/* %assign/ar <array>, <delay>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory. <delay>
 * is the delay in simulation time. <bit> is the index register
 * containing the real value.
 */
તાર્કિક of_ASSIGN_AR(vthread_t thr, vvp_code_t cp)
{
      long adr = thr->શબ્દો[3].w_int;
      unsigned delay = cp->bit_idx[0];
      દ્વિગુણ value = thr->કાઢો_real();

      જો (adr >= 0) {
	    schedule_assign_array_word(cp->array, adr, value, delay);
      }

      પાછા સાચું;
}

/* %assign/ar/d <array>, <delay_idx>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory.
 * <delay_idx> is the integer register that contains the delay value.
 */
તાર્કિક of_ASSIGN_ARD(vthread_t thr, vvp_code_t cp)
{
      long adr = thr->શબ્દો[3].w_int;
      દ્વિગુણ value = thr->કાઢો_real();

      જો (adr >= 0) {
	    vvp_time64_t delay = thr->શબ્દો[cp->bit_idx[0]].w_uint;
	    schedule_assign_array_word(cp->array, adr, value, delay);
      }

      પાછા સાચું;
}

/* %assign/ar/e <array>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory. <bit>
 * is the index register containing the real value. The event
 * information is contained in the thread event control registers
 * and is set with %evctl.
 */
તાર્કિક of_ASSIGN_ARE(vthread_t thr, vvp_code_t cp)
{
      long adr = thr->શબ્દો[3].w_int;
      દ્વિગુણ value = thr->કાઢો_real();

      જો (adr >= 0) {
	    જો (thr->ecount == 0) {
		  schedule_assign_array_word(cp->array, adr, value, 0);
	    } નહીંતર {
		  schedule_evctl(cp->array, adr, value, thr->event,
		                 thr->ecount);
	    }
      }

      પાછા સાચું;
}

/*
 * %assign/vec4 <var>, <delay>
 */
તાર્કિક of_ASSIGN_VEC4(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      unsigned delay = cp->bit_idx[0];
      અચલ vvp_vector4_t&val = thr->peek_vec4();

      schedule_assign_vector(ptr, 0, 0, val, delay);
      thr->કાઢો_vec4(1);
      પાછા સાચું;
}

/*
 * Resizes a vector value for a partial assignment so that the value is fully
 * in-bounds of the target signal. Both `val` and `off` will be updated if
 * necessary.
 *
 * Returns false if the value is fully out-of-bounds and the assignment should
 * be skipped. Otherwise returns true.
 */
સ્થિર તાર્કિક resize_rval_vec(vvp_vector4_t &val, int64_t &off,
			    unsigned પૂર્ણાંક sig_wid)
{
      unsigned પૂર્ણાંક wid = val.size();

        // Fully in bounds, most likely case
      જો (off >= 0 && (uint64_t)off + wid <= sig_wid)
	    પાછા સાચું;

      unsigned પૂર્ણાંક base = 0;
      જો (off >= 0) {
	      // Fully out-of-bounds
	    જો ((uint64_t)off >= sig_wid)
		  પાછા ખોટું;
      } નહીંતર {
	      // Fully out-of-bounds */
	    જો ((uint64_t)(-off) >= wid)
		  પાછા ખોટું;

	      // If the index is below the vector, then only assign the high
	      // bits that overlap with the target
	    base = -off;
	    wid += off;
		off = 0;
      }

	// If the value is partly above the target, then only assign
	// the bits that overlap
      જો ((uint64_t)off + wid > sig_wid)
	    wid = sig_wid - (uint64_t)off;

      val = val.subvalue(base, wid);

      પાછા સાચું;
}

/*
 * %assign/vec4/a/d <arr>, <offx>, <delx>
 */
તાર્કિક of_ASSIGN_VEC4_A_D(vthread_t thr, vvp_code_t cp)
{
      પૂર્ણાંક off_idx = cp->bit_idx[0];
      પૂર્ણાંક del_idx = cp->bit_idx[1];
      પૂર્ણાંક adr_idx = 3;

      int64_t  off = off_idx ? thr->શબ્દો[off_idx].w_int : 0;
      vvp_time64_t del = del_idx? thr->શબ્દો[del_idx].w_uint : 0;
      long     adr = thr->શબ્દો[adr_idx].w_int;

      vvp_vector4_t val = thr->કાઢો_vec4();

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      જો (thr->flags[4] == BIT4_1)
	    પાછા સાચું;

      જો (!resize_rval_vec(val, off, cp->array->get_word_size()))
	    પાછા સાચું;

      schedule_assign_array_word(cp->array, adr, off, val, del);

      પાછા સાચું;
}

/*
 * %assign/vec4/a/e <arr>, <offx>
 */
તાર્કિક of_ASSIGN_VEC4_A_E(vthread_t thr, vvp_code_t cp)
{
      પૂર્ણાંક off_idx = cp->bit_idx[0];
      પૂર્ણાંક adr_idx = 3;

      int64_t  off = off_idx ? thr->શબ્દો[off_idx].w_int : 0;
      long     adr = thr->શબ્દો[adr_idx].w_int;

      vvp_vector4_t val = thr->કાઢો_vec4();

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      જો (thr->flags[4] == BIT4_1)
	    પાછા સાચું;

      જો (!resize_rval_vec(val, off, cp->array->get_word_size()))
	    પાછા સાચું;

      જો (thr->ecount == 0) {
	    schedule_assign_array_word(cp->array, adr, off, val, 0);
      } નહીંતર {
	    schedule_evctl(cp->array, adr, val, off, thr->event, thr->ecount);
      }

      પાછા સાચું;
}

/*
 * %assign/vec4/off/d <var>, <off>, <del>
 */
તાર્કિક of_ASSIGN_VEC4_OFF_D(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      unsigned off_index = cp->bit_idx[0];
      unsigned del_index = cp->bit_idx[1];
      vvp_vector4_t val = thr->કાઢો_vec4();

      int64_t off = thr->શબ્દો[off_index].w_int;
      vvp_time64_t del = thr->શબ્દો[del_index].w_uint;

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      જો (thr->flags[4] == BIT4_1)
	    પાછા સાચું;

      vvp_signal_કિંમત*sig = dynamic_cast<vvp_signal_કિંમત*> (cp->net->fil);
      પ્રતિજ્ઞા(sig);

      જો (!resize_rval_vec(val, off, sig->કિંમત_size()))
	    પાછા સાચું;

      schedule_assign_vector(ptr, off, sig->કિંમત_size(), val, del);
      પાછા સાચું;
}

/*
 * %assign/vec4/off/e <var>, <off>
 */
તાર્કિક of_ASSIGN_VEC4_OFF_E(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      unsigned off_index = cp->bit_idx[0];
      vvp_vector4_t val = thr->કાઢો_vec4();

      int64_t off = thr->શબ્દો[off_index].w_int;

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      જો (thr->flags[4] == BIT4_1)
	    પાછા સાચું;

      vvp_signal_કિંમત*sig = dynamic_cast<vvp_signal_કિંમત*> (cp->net->fil);
      પ્રતિજ્ઞા(sig);

      જો (!resize_rval_vec(val, off, sig->કિંમત_size()))
	    પાછા સાચું;

      જો (thr->ecount == 0) {
	    schedule_assign_vector(ptr, off, sig->કિંમત_size(), val, 0);
      } નહીંતર {
	    schedule_evctl(ptr, val, off, sig->કિંમત_size(), thr->event, thr->ecount);
      }

      પાછા સાચું;
}

/*
 * %assign/vec4/d <var-label> <delay>
 */
તાર્કિક of_ASSIGN_VEC4D(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      unsigned del_index = cp->bit_idx[0];
      vvp_time64_t del = thr->શબ્દો[del_index].w_int;

      vvp_vector4_t value = thr->કાઢો_vec4();

      vvp_signal_કિંમત*sig = dynamic_cast<vvp_signal_કિંમત*> (cp->net->fil);
      પ્રતિજ્ઞા(sig);

      schedule_assign_vector(ptr, 0, sig->કિંમત_size(), value, del);

      પાછા સાચું;
}

/*
 * %assign/vec4/e <var-label>
 */
તાર્કિક of_ASSIGN_VEC4E(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_vector4_t value = thr->કાઢો_vec4();

      vvp_signal_કિંમત*sig = dynamic_cast<vvp_signal_કિંમત*> (cp->net->fil);
      પ્રતિજ્ઞા(sig);

      જો (thr->ecount == 0) {
	    schedule_assign_vector(ptr, 0, sig->કિંમત_size(), value, 0);
      } નહીંતર {
	    schedule_evctl(ptr, value, 0, sig->કિંમત_size(), thr->event, thr->ecount);
      }

      પાછા સાચું;
}

/*
 * This is %assign/wr <vpi-label>, <delay>
 *
 * This assigns (after a delay) a value to a real variable. Use the
 * vpi_put_value function to do the assign, with the delay written
 * into the vpiInertialDelay carrying the desired delay.
 */
તાર્કિક of_ASSIGN_WR(vthread_t thr, vvp_code_t cp)
{
      unsigned delay = cp->bit_idx[0];
      દ્વિગુણ value = thr->કાઢો_real();
      s_vpi_time del;

      del.type = vpiSimTime;
      vpip_time_to_timestruct(&del, delay);

      __vpiHandle*tmp = cp->handle;

      t_vpi_કિંમત val;
      val.format = vpiRealVal;
      val.value.real = value;
      vpi_put_કિંમત(tmp, &val, &del, vpiTransportDelay);

      પાછા સાચું;
}

તાર્કિક of_ASSIGN_WRD(vthread_t thr, vvp_code_t cp)
{
      vvp_time64_t delay = thr->શબ્દો[cp->bit_idx[0]].w_uint;
      દ્વિગુણ value = thr->કાઢો_real();
      s_vpi_time del;

      del.type = vpiSimTime;
      vpip_time_to_timestruct(&del, delay);

      __vpiHandle*tmp = cp->handle;

      t_vpi_કિંમત val;
      val.format = vpiRealVal;
      val.value.real = value;
      vpi_put_કિંમત(tmp, &val, &del, vpiTransportDelay);

      પાછા સાચું;
}

તાર્કિક of_ASSIGN_WRE(vthread_t thr, vvp_code_t cp)
{
      પ્રતિજ્ઞા(thr->event != 0);
      દ્વિગુણ value = thr->કાઢો_real();
      __vpiHandle*tmp = cp->handle;

	// If the count is zero then just put the value.
      જો (thr->ecount == 0) {
	    t_vpi_કિંમત val;

	    val.format = vpiRealVal;
	    val.value.real = value;
	    vpi_put_કિંમત(tmp, &val, 0, vpiNoDelay);
      } નહીંતર {
	    schedule_evctl(tmp, value, thr->event, thr->ecount);
      }

      thr->event = 0;
      thr->ecount = 0;

      પાછા સાચું;
}

તાર્કિક of_BLEND(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t vala = thr->કાઢો_vec4();
      vvp_vector4_t valb = thr->કાઢો_vec4();
      પ્રતિજ્ઞા(vala.size() == valb.size());

      માટે (unsigned idx = 0 ; idx < vala.size() ; idx += 1) {
	    જો (vala.value(idx) == valb.value(idx))
		  ચાલુ;

	    vala.set_bit(idx, BIT4_X);
      }

      thr->ધકેલો_vec4(vala);
      પાછા સાચું;
}

તાર્કિક of_BLEND_STR(vthread_t thr, vvp_code_t)
{
      string f = thr->કાઢો_str();
      string t = thr->કાઢો_str();
      thr->ધકેલો_str((t == f) ? t : "");
      પાછા સાચું;
}

તાર્કિક of_BLEND_WR(vthread_t thr, vvp_code_t)
{
      દ્વિગુણ f = thr->કાઢો_real();
      દ્વિગુણ t = thr->કાઢો_real();
      thr->ધકેલો_real((t == f) ? t : 0.0);
      પાછા સાચું;
}

તાર્કિક of_BREAKPOINT(vthread_t, vvp_code_t)
{
      પાછા સાચું;
}

/*
 * %callf/void <code-label>, <scope-label>
 * Combine the %fork and %join steps for invoking a function.
 */
સ્થિર તાર્કિક do_callf_void(vthread_t thr, vthread_t child)
{

      જો (child->parent_scope->is_automatic()) {
	      /* The context allocated for this child is the top entry
		 on the write context stack */
	    child->wt_context = thr->wt_context;
	    child->rd_context = thr->wt_context;
      }

        // Mark the function thread as a direct child of the current thread.
      child->parent = thr;
      thr->children.insert(child);
        // This should be the only child
      પ્રતિજ્ઞા(thr->children.size()==1);

        // Execute the function. This SHOULD run the function to completion,
        // but there are some exceptional situations where it won't.
      પ્રતિજ્ઞા(child->parent_scope->get_type_code() == vpiFunction);
      child->is_scheduled = 1;
      child->i_am_in_function = 1;
      vthread_run(child);
      running_thread = thr;

      જો (child->i_have_ended) {
	    do_join(thr, child);
	    પાછા સાચું;
      } નહીંતર {
	    thr->i_am_joining = 1;
	    પાછા ખોટું;
      }
}

તાર્કિક of_CALLF_OBJ(vthread_t thr, vvp_code_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);
      પાછા do_callf_void(thr, child);

      // XXXX NOT IMPLEMENTED
}

તાર્કિક of_CALLF_REAL(vthread_t thr, vvp_code_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);

	// This is the return value. Push a place-holder value. The function
	// will replace this with the actual value using a %ret/real instruction.
      thr->ધકેલો_real(0.0);
      child->args_real.push_back(0);

      પાછા do_callf_void(thr, child);
}

તાર્કિક of_CALLF_STR(vthread_t thr, vvp_code_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);

      thr->ધકેલો_str("");
      child->args_str.push_back(0);

      પાછા do_callf_void(thr, child);
}

તાર્કિક of_CALLF_VEC4(vthread_t thr, vvp_code_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);

      vpiScopeFunction*scope_func = dynamic_cast<vpiScopeFunction*>(cp->scope);
      પ્રતિજ્ઞા(scope_func);

	// This is the return value. Push a place-holder value. The function
	// will replace this with the actual value using a %ret/real instruction.
      thr->ધકેલો_vec4(vvp_vector4_t(scope_func->get_func_width(), scope_func->get_func_init_val()));
      child->args_vec4.push_back(0);

      પાછા do_callf_void(thr, child);
}

તાર્કિક of_CALLF_VOID(vthread_t thr, vvp_code_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);
      પાછા do_callf_void(thr, child);
}

/*
 * The %cassign/link instruction connects a source node to a
 * destination node. The destination node must be a signal, as it is
 * marked with the source of the cassign so that it may later be
 * unlinked without specifically knowing the source that this
 * instruction used.
 */
તાર્કિક of_CASSIGN_LINK(vthread_t, vvp_code_t cp)
{
      vvp_net_t*dst = cp->net;
      vvp_net_t*src = cp->net2;

      vvp_fun_signal_base*sig
	    = dynamic_cast<vvp_fun_signal_base*>(dst->fun);
      પ્રતિજ્ઞા(sig);

	/* Any previous continuous assign should have been removed already. */
      પ્રતિજ્ઞા(sig->cassign_link == 0);

      sig->cassign_link = src;

	/* Link the output of the src to the port[1] (the cassign
	   port) of the destination. */
      vvp_net_ptr_t dst_ptr (dst, 1);
      src->link(dst_ptr);

      પાછા સાચું;
}

/*
 * If there is an existing continuous assign linked to the destination
 * node, unlink it. This must be done before applying a new continuous
 * assign, otherwise the initial assigned value will be propagated to
 * any other nodes driven by the old continuous assign source.
 */
સ્થિર શૂન્ય cassign_unlink(vvp_net_t*dst)
{
      vvp_fun_signal_base*sig
	    = dynamic_cast<vvp_fun_signal_base*>(dst->fun);
      પ્રતિજ્ઞા(sig);

      જો (sig->cassign_link == 0)
	    પાછા;

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
તાર્કિક of_CASSIGN_VEC4(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      vvp_vector4_t value = thr->કાઢો_vec4();

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

	/* Set the value into port 1 of the destination. */
      vvp_net_ptr_t ptr (net, 1);
      vvp_send_vec4(ptr, value, 0);

      પાછા સાચું;
}

/*
 * %cassign/vec4/off <var>, <off>
 */
તાર્કિક of_CASSIGN_VEC4_OFF(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      unsigned base_idx = cp->bit_idx[0];
      long base = thr->શબ્દો[base_idx].w_int;
      vvp_vector4_t value = thr->કાઢો_vec4();
      unsigned wid = value.size();

      જો (thr->flags[4] == BIT4_1)
	    પાછા સાચું;

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

      vvp_signal_કિંમત*sig = dynamic_cast<vvp_signal_કિંમત*> (net->fil);
      પ્રતિજ્ઞા(sig);

      જો (base < 0 && (wid <= (unsigned)-base))
	    પાછા સાચું;

      જો (base >= (long)sig->કિંમત_size())
	    પાછા સાચું;

      જો (base < 0) {
	    wid -= (unsigned) -base;
	    base = 0;
	    value.resize(wid);
      }

      જો (base+wid > sig->કિંમત_size()) {
	    wid = sig->કિંમત_size() - base;
	    value.resize(wid);
      }

      vvp_net_ptr_t ptr (net, 1);
      vvp_send_vec4_pv(ptr, value, base, sig->કિંમત_size(), 0);
      પાછા સાચું;
}

તાર્કિક of_CASSIGN_WR(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net  = cp->net;
      દ્વિગુણ value = thr->કાઢો_real();

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

	/* Set the value into port 1 of the destination. */
      vvp_net_ptr_t ptr (net, 1);
      vvp_send_real(ptr, value, 0);

      પાછા સાચું;
}

/*
 * %cast2
 */
તાર્કિક of_CAST2(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      unsigned wid = val.size();

      માટે (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    પસંદગી (val.value(idx)) {
		સ્થિતિ BIT4_0:
		સ્થિતિ BIT4_1:
		  તોડો;
		મૂળભૂત:
		  val.set_bit(idx, BIT4_0);
		  તોડો;
	    }
      }

      પાછા સાચું;
}

તાર્કિક do_cast_vec_dar(vthread_t thr, vvp_code_t cp, તાર્કિક as_vec4)
{
      unsigned wid = cp->સંખ્યા;

      vvp_object_t obj;
      thr->કાઢો_object(obj);

      vvp_darray*darray = obj.peek<vvp_darray>();
      પ્રતિજ્ઞા(darray);

      vvp_vector4_t vec = darray->get_bitstream(as_vec4);
      જો (vec.size() != wid) {
	    cerr << thr->get_fileline()
	         << "VVP error: size mismatch when casting dynamic array to vector." << endl;
            thr->ધકેલો_vec4(vvp_vector4_t(wid));
            schedule_stop(0);
            પાછા ખોટું;
      }
      thr->ધકેલો_vec4(vec);
      પાછા સાચું;
}

/*
 * %cast/vec2/dar <wid>
 */
તાર્કિક of_CAST_VEC2_DAR(vthread_t thr, vvp_code_t cp)
{
      પાછા do_cast_vec_dar(thr, cp, ખોટું);
}

/*
 * %cast/vec4/dar <wid>
 */
તાર્કિક of_CAST_VEC4_DAR(vthread_t thr, vvp_code_t cp)
{
      પાછા do_cast_vec_dar(thr, cp, સાચું);
}

/*
 * %cast/vec4/str <wid>
 */
તાર્કિક of_CAST_VEC4_STR(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->સંખ્યા;
      string str = thr->કાઢો_str();

      vvp_vector4_t vec(wid, BIT4_0);

      જો (wid != 8*str.length()) {
	    cerr << thr->get_fileline()
	         << "VVP error: size mismatch when casting string to vector." << endl;
            thr->ધકેલો_vec4(vec);
            schedule_stop(0);
            પાછા ખોટું;
      }

      unsigned sdx = 0;
      unsigned vdx = wid;
      જ્યારે (vdx > 0) {
            અક્ષર ch = str[sdx++];
            vdx -= 8;
            માટે (unsigned bdx = 0; bdx < 8; bdx += 1) {
                  જો (ch & 1)
                        vec.set_bit(vdx+bdx, BIT4_1);
                  ch >>= 1;
            }
      }

      thr->ધકેલો_vec4(vec);
      પાછા સાચું;
}

સ્થિર શૂન્ય do_CMPE(vthread_t thr, અચલ vvp_vector4_t&lval, અચલ vvp_vector4_t&rval)
{
      પ્રતિજ્ઞા(rval.size() == lval.size());

      જો (lval.has_xz() || rval.has_xz()) {

	    unsigned wid = lval.size();
	    vvp_bit4_t eq  = BIT4_1;
	    vvp_bit4_t eeq = BIT4_1;

	    માટે (unsigned idx = 0 ; idx < wid ; idx += 1) {
		  vvp_bit4_t lv = lval.value(idx);
		  vvp_bit4_t rv = rval.value(idx);

		  જો (lv != rv)
			eeq = BIT4_0;

		  જો (eq==BIT4_1 && (bit4_is_xz(lv) || bit4_is_xz(rv)))
			eq = BIT4_X;
		  જો ((lv == BIT4_0) && (rv==BIT4_1))
			eq = BIT4_0;
		  જો ((lv == BIT4_1) && (rv==BIT4_0))
			eq = BIT4_0;

		  જો (eq == BIT4_0)
			તોડો;
	    }

	    thr->flags[4] = eq;
	    thr->flags[6] = eeq;

      } નહીંતર {
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
તાર્કિક of_CMPE(vthread_t thr, vvp_code_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      અચલ vvp_vector4_t&rval = thr->peek_vec4(0);
      અચલ vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPE(thr, lval, rval);

      thr->કાઢો_vec4(2);
      પાછા સાચું;
}

તાર્કિક of_CMPNE(vthread_t thr, vvp_code_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      અચલ vvp_vector4_t&rval = thr->peek_vec4(0);
      અચલ vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];
      thr->flags[6] =  ~thr->flags[6];

      thr->કાઢો_vec4(2);
      પાછા સાચું;
}

/*
 * %cmpi/e <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
તાર્કિક of_CMPIE(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->સંખ્યા;

      અચલ vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_immediate_rval (cp, rval);

      do_CMPE(thr, lval, rval);

      thr->કાઢો_vec4(1);
      પાછા સાચું;
}

તાર્કિક of_CMPINE(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->સંખ્યા;

      અચલ vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_immediate_rval (cp, rval);

      do_CMPE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];
      thr->flags[6] =  ~thr->flags[6];

      thr->કાઢો_vec4(1);
      પાછા સાચું;
}



સ્થિર શૂન્ય do_CMPS(vthread_t thr, અચલ vvp_vector4_t&lval, અચલ vvp_vector4_t&rval)
{
      પ્રતિજ્ઞા(rval.size() == lval.size());

	// If either value has XZ bits, then the eq and lt values are
	// known already to be X. Just calculate the eeq result as a
	// special case and short circuit the rest of the compare.
      જો (lval.has_xz() || rval.has_xz()) {
	    thr->flags[4] = BIT4_X; // eq
	    thr->flags[5] = BIT4_X; // lt
	    thr->flags[6] = lval.eeq(rval)? BIT4_1 : BIT4_0;
	    પાછા;
      }

	// Past this point, we know we are dealing only with fully
	// defined values.
      unsigned wid = lval.size();

      અચલ vvp_bit4_t sig1 = lval.value(wid-1);
      અચલ vvp_bit4_t sig2 = rval.value(wid-1);

	// If the lval is <0 and the rval is >=0, then we know the result.
      જો ((sig1 == BIT4_1) && (sig2 == BIT4_0)) {
	    thr->flags[4] = BIT4_0; // eq;
	    thr->flags[5] = BIT4_1; // lt;
	    thr->flags[6] = BIT4_0; // eeq
	    પાછા;
      }

	// If the lval is >=0 and the rval is <0, then we know the result.
      જો ((sig1 == BIT4_0) && (sig2 == BIT4_1)) {
	    thr->flags[4] = BIT4_0; // eq;
	    thr->flags[5] = BIT4_0; // lt;
	    thr->flags[6] = BIT4_0; // eeq
	    પાછા;
      }

	// The values have the same sign, so we have to look at the
	// actual value. Scan from the MSB down. As soon as we find a
	// bit that differs, we know the result.

      માટે (unsigned idx = 1 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t lv = lval.value(wid-1-idx);
	    vvp_bit4_t rv = rval.value(wid-1-idx);

	    જો (lv == rv)
		  ચાલુ;

	    thr->flags[4] = BIT4_0; // eq
	    thr->flags[6] = BIT4_0; // eeq

	    જો (lv==BIT4_0) {
		  thr->flags[5] = BIT4_1; // lt
	    } નહીંતર {
		  thr->flags[5] = BIT4_0; // lt
	    }
	    પાછા;
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
તાર્કિક of_CMPS(vthread_t thr, vvp_code_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      અચલ vvp_vector4_t&rval = thr->peek_vec4(0);
      અચલ vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPS(thr, lval, rval);

      thr->કાઢો_vec4(2);
      પાછા સાચું;
}

/*
 * %cmpi/s <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
તાર્કિક of_CMPIS(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->સંખ્યા;

      અચલ vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_immediate_rval (cp, rval);

      do_CMPS(thr, lval, rval);

      thr->કાઢો_vec4(1);
      પાછા સાચું;
}

તાર્કિક of_CMPSTR(vthread_t thr, vvp_code_t)
{
      string re = thr->કાઢો_str();
      string le = thr->કાઢો_str();

      પૂર્ણાંક rc = strcmp(le.c_str(), re.c_str());

      vvp_bit4_t eq;
      vvp_bit4_t lt;

      જો (rc == 0) {
	    eq = BIT4_1;
	    lt = BIT4_0;
      } નહીંતર જો (rc < 0) {
	    eq = BIT4_0;
	    lt = BIT4_1;
      } નહીંતર {
	    eq = BIT4_0;
	    lt = BIT4_0;
      }

      thr->flags[4] = eq;
      thr->flags[5] = lt;

      પાછા સાચું;
}

સ્થિર શૂન્ય of_CMPU_the_hard_way(vthread_t thr, unsigned wid,
				 અચલ vvp_vector4_t&lval,
				 અચલ vvp_vector4_t&rval)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_bit4_t eeq = BIT4_1;

      માટે (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t lv = lval.value(idx);
	    vvp_bit4_t rv = rval.value(idx);

	    જો (lv != rv)
		  eeq = BIT4_0;

	    જો (eq==BIT4_1 && (bit4_is_xz(lv) || bit4_is_xz(rv)))
		  eq = BIT4_X;
	    જો ((lv == BIT4_0) && (rv==BIT4_1))
		  eq = BIT4_0;
	    જો ((lv == BIT4_1) && (rv==BIT4_0))
		  eq = BIT4_0;

	    જો (eq == BIT4_0)
		  તોડો;

      }

      thr->flags[4] = eq;
      thr->flags[5] = BIT4_X;
      thr->flags[6] = eeq;
}

સ્થિર શૂન્ય do_CMPU(vthread_t thr, અચલ vvp_vector4_t&lval, અચલ vvp_vector4_t&rval)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_bit4_t lt = BIT4_0;

      જો (rval.size() != lval.size()) {
	    cerr << thr->get_fileline()
	         << "VVP ERROR: %cmp/u operand width mismatch: lval=" << lval
		 << ", rval=" << rval << endl;
      }
      પ્રતિજ્ઞા(rval.size() == lval.size());
      unsigned wid = lval.size();

      unsigned long*larray = lval.subarray(0,wid);
      જો (larray == 0) પાછા of_CMPU_the_hard_way(thr, wid, lval, rval);

      unsigned long*rarray = rval.subarray(0,wid);
      જો (rarray == 0) {
	    delete[]larray;
	    પાછા of_CMPU_the_hard_way(thr, wid, lval, rval);
      }

      unsigned શબ્દો = (wid+CPU_WORD_BITS-1) / CPU_WORD_BITS;

      માટે (unsigned wdx = 0 ; wdx < શબ્દો ; wdx += 1) {
	    જો (larray[wdx] == rarray[wdx])
		  ચાલુ;

	    eq = BIT4_0;
	    જો (larray[wdx] < rarray[wdx])
		  lt = BIT4_1;
	    નહીંતર
		  lt = BIT4_0;
      }

      delete[]larray;
      delete[]rarray;

      thr->flags[4] = eq;
      thr->flags[5] = lt;
      thr->flags[6] = eq;
}

તાર્કિક of_CMPU(vthread_t thr, vvp_code_t)
{

      અચલ vvp_vector4_t&rval = thr->peek_vec4(0);
      અચલ vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPU(thr, lval, rval);

      thr->કાઢો_vec4(2);
      પાછા સાચું;
}

/*
 * %cmpi/u <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
તાર્કિક of_CMPIU(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->સંખ્યા;

      અચલ vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_immediate_rval (cp, rval);

      do_CMPU(thr, lval, rval);

      thr->કાઢો_vec4(1);
      પાછા સાચું;
}


/*
 * %cmp/x
 */
તાર્કિક of_CMPX(vthread_t thr, vvp_code_t)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_vector4_t rval = thr->કાઢો_vec4();
      vvp_vector4_t lval = thr->કાઢો_vec4();

      પ્રતિજ્ઞા(rval.size() == lval.size());
      unsigned wid = lval.size();

      માટે (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    vvp_bit4_t lv = lval.value(idx);
	    vvp_bit4_t rv = rval.value(idx);
	    જો ((lv != rv) && !bit4_is_xz(lv) && !bit4_is_xz(rv)) {
		  eq = BIT4_0;
		  તોડો;
	    }
      }

      thr->flags[4] = eq;
      પાછા સાચું;
}

સ્થિર શૂન્ય do_CMPWE(vthread_t thr, અચલ vvp_vector4_t&lval, અચલ vvp_vector4_t&rval)
{
      પ્રતિજ્ઞા(rval.size() == lval.size());

      જો (lval.has_xz() || rval.has_xz()) {

	    unsigned wid = lval.size();
	    vvp_bit4_t eq  = BIT4_1;

	    માટે (unsigned idx = 0 ; idx < wid ; idx += 1) {
		  vvp_bit4_t lv = lval.value(idx);
		  vvp_bit4_t rv = rval.value(idx);

		  જો (bit4_is_xz(rv))
			ચાલુ;
		  જો ((eq == BIT4_1) && bit4_is_xz(lv))
			eq = BIT4_X;
		  જો ((lv == BIT4_0) && (rv==BIT4_1))
			eq = BIT4_0;
		  જો ((lv == BIT4_1) && (rv==BIT4_0))
			eq = BIT4_0;

		  જો (eq == BIT4_0)
			તોડો;
	    }

	    thr->flags[4] = eq;

      } નહીંતર {
	      // If there are no XZ bits anywhere, then the results of
	      // ==? match the === test.
	    thr->flags[4] = (lval.eeq(rval)? BIT4_1 : BIT4_0);
      }
}

તાર્કિક of_CMPWE(vthread_t thr, vvp_code_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      અચલ vvp_vector4_t&rval = thr->peek_vec4(0);
      અચલ vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPWE(thr, lval, rval);

      thr->કાઢો_vec4(2);
      પાછા સાચું;
}

તાર્કિક of_CMPWNE(vthread_t thr, vvp_code_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      અચલ vvp_vector4_t&rval = thr->peek_vec4(0);
      અચલ vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPWE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];

      thr->કાઢો_vec4(2);
      પાછા સાચું;
}

તાર્કિક of_CMPWR(vthread_t thr, vvp_code_t)
{
      દ્વિગુણ r = thr->કાઢો_real();
      દ્વિગુણ l = thr->કાઢો_real();

      vvp_bit4_t eq = (l == r)? BIT4_1 : BIT4_0;
      vvp_bit4_t lt = (l <  r)? BIT4_1 : BIT4_0;

      thr->flags[4] = eq;
      thr->flags[5] = lt;

      પાછા સાચું;
}

/*
 * %cmp/z
 */
તાર્કિક of_CMPZ(vthread_t thr, vvp_code_t)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_vector4_t rval = thr->કાઢો_vec4();
      vvp_vector4_t lval = thr->કાઢો_vec4();

      પ્રતિજ્ઞા(rval.size() == lval.size());
      unsigned wid = lval.size();

      માટે (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    vvp_bit4_t lv = lval.value(idx);
	    vvp_bit4_t rv = rval.value(idx);
	    જો ((lv != rv) && (rv != BIT4_Z) && (lv != BIT4_Z)) {
		  eq = BIT4_0;
		  તોડો;
	    }
      }

      thr->flags[4] = eq;
      પાછા સાચું;
}

/*
 *  %concat/str;
 */
તાર્કિક of_CONCAT_STR(vthread_t thr, vvp_code_t)
{
      string text = thr->કાઢો_str();
      thr->peek_str(0).append(text);
      પાછા સાચું;
}

/*
 *  %concati/str <string>;
 */
તાર્કિક of_CONCATI_STR(vthread_t thr, vvp_code_t cp)
{
      અચલ અક્ષર*text = cp->text;
      thr->peek_str(0).append(filter_string(text));
      પાછા સાચું;
}

/*
 * %concat/vec4
 */
તાર્કિક of_CONCAT_VEC4(vthread_t thr, vvp_code_t)
{
      અચલ vvp_vector4_t&lsb = thr->peek_vec4(0);
      અચલ vvp_vector4_t&msb = thr->peek_vec4(1);

	// The result is the size of the top two vectors in the stack.
      vvp_vector4_t res (msb.size() + lsb.size(), BIT4_X);

	// Build up the result.
      res.set_vec(0, lsb);
      res.set_vec(lsb.size(), msb);

	// Rearrange the stack to pop the inputs and push the
	// result. Do that by actually popping only 1 stack position
	// and replacing the new top with the new value.
      thr->કાઢો_vec4(1);
      thr->peek_vec4() = res;

      પાછા સાચું;
}

/*
 * %concati/vec4 <vala>, <valb>, <wid>
 *
 * Concat the immediate value to the LOW bits of the concatenation.
 * Get the HIGH bits from the top of the vec4 stack.
 */
તાર્કિક of_CONCATI_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned wid  = cp->સંખ્યા;

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
      પાછા સાચું;
}

/*
 * %cvt/rv
 */
તાર્કિક of_CVT_RV(vthread_t thr, vvp_code_t)
{
      દ્વિગુણ val;
      vvp_vector4_t val4 = thr->કાઢો_vec4();
      vector4_to_કિંમત(val4, val, ખોટું);
      thr->ધકેલો_real(val);
      પાછા સાચું;
}

/*
 * %cvt/rv/s
 */
તાર્કિક of_CVT_RV_S(vthread_t thr, vvp_code_t)
{
      દ્વિગુણ val;
      vvp_vector4_t val4 = thr->કાઢો_vec4();
      vector4_to_કિંમત(val4, val, સાચું);
      thr->ધકેલો_real(val);
      પાછા સાચું;
}

/*
 * %cvt/sr <idx>
 * Pop the top value from the real stack, convert it to a 64bit signed
 * and save it to the indexed register.
 */
તાર્કિક of_CVT_SR(vthread_t thr, vvp_code_t cp)
{
      દ્વિગુણ r = thr->કાઢો_real();
      thr->શબ્દો[cp->bit_idx[0]].w_int = i64round(r);

      પાછા સાચું;
}

/*
 * %cvt/ur <idx>
 */
તાર્કિક of_CVT_UR(vthread_t thr, vvp_code_t cp)
{
      દ્વિગુણ r = thr->કાઢો_real();
      thr->શબ્દો[cp->bit_idx[0]].w_uint = vlg_round_to_u64(r);

      પાછા સાચું;
}

/*
 * %cvt/vr <wid>
 */
તાર્કિક of_CVT_VR(vthread_t thr, vvp_code_t cp)
{
      દ્વિગુણ r = thr->કાઢો_real();
      unsigned wid = cp->સંખ્યા;

      vvp_vector4_t tmp(wid, r);
      thr->ધકેલો_vec4(tmp);
      પાછા સાચું;
}

/*
 * This implements the %deassign instruction. All we do is write a
 * long(1) to port-3 of the addressed net. This turns off an active
 * continuous assign activated by %cassign/v
 */
તાર્કિક of_DEASSIGN(vthread_t, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      unsigned base  = cp->bit_idx[0];
      unsigned width = cp->bit_idx[1];

      vvp_signal_કિંમત*fil = dynamic_cast<vvp_signal_કિંમત*> (net->fil);
      પ્રતિજ્ઞા(fil);
      vvp_fun_signal_vec*sig = dynamic_cast<vvp_fun_signal_vec*>(net->fun);
      પ્રતિજ્ઞા(sig);

      જો (base >= fil->કિંમત_size()) પાછા સાચું;
      જો (base+width > fil->કિંમત_size()) width = fil->કિંમત_size() - base;

      તાર્કિક full_sig = base == 0 && width == fil->કિંમત_size();

	// This is the net that is forcing me...
      જો (vvp_net_t*src = sig->cassign_link) {
	    જો (! full_sig) {
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
      જો (full_sig) {
	    sig->deassign();
      } નહીંતર {
	    sig->deassign_pv(base, width);
      }

      પાછા સાચું;
}

તાર્કિક of_DEASSIGN_WR(vthread_t, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_real*sig = dynamic_cast<vvp_fun_signal_real*>(net->fun);
      પ્રતિજ્ઞા(sig);

	// This is the net that is forcing me...
      જો (vvp_net_t*src = sig->cassign_link) {
	      // And this is the pointer to be removed.
	    vvp_net_ptr_t dst_ptr (net, 1);
	    src->unlink(dst_ptr);
	    sig->cassign_link = 0;
      }

      sig->deassign();

      પાછા સાચું;
}

/*
 * %debug/thr
 */
તાર્કિક of_DEBUG_THR(vthread_t thr, vvp_code_t cp)
{
      અચલ અક્ષર*text = cp->text;
      thr->debug_dump(cerr, text);
      પાછા સાચું;
}

/*
 * The delay takes two 32bit numbers to make up a 64bit time.
 *
 *   %delay <low>, <hig>
 */
તાર્કિક of_DELAY(vthread_t thr, vvp_code_t cp)
{
      vvp_time64_t નીચલું = cp->bit_idx[0];
      vvp_time64_t hig = cp->bit_idx[1];

      vvp_time64_t delay = (hig << 32) | નીચલું;

      જો (delay == 0) schedule_inactive(thr);
      નહીંતર schedule_vthread(thr, delay);
      પાછા ખોટું;
}

તાર્કિક of_DELAYX(vthread_t thr, vvp_code_t cp)
{
      vvp_time64_t delay;

      પ્રતિજ્ઞા(cp->સંખ્યા < vthread_s::શબ્દો_ગણતરી);
      delay = thr->શબ્દો[cp->સંખ્યા].w_uint;
      જો (delay == 0) schedule_inactive(thr);
      નહીંતર schedule_vthread(thr, delay);
      પાછા ખોટું;
}

તાર્કિક of_DELETE_ELEM(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      int64_t idx_val = thr->શબ્દો[3].w_int;
      જો (thr->flags[4] == BIT4_1) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with undefined index."
	         << endl;
	    પાછા સાચું;
      }
      જો (idx_val < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with negative index."
	         << endl;
	    પાછા સાચું;
      }
      size_t idx = idx_val;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      પ્રતિજ્ઞા(obj);

      vvp_queue*queue = obj->get_object().peek<vvp_queue>();
      જો (queue == 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping delete(" << idx
	         << ") on empty queue." << endl;
      } નહીંતર {
	    size_t size = queue->get_size();
	    જો (idx >= size) {
		  cerr << thr->get_fileline()
		       << "Warning: skipping out of range delete(" << idx
		       << ") on queue of size " << size << "." << endl;
	    } નહીંતર {
		  queue->erase(idx);
	    }
      }

      પાછા સાચું;
}

/* %delete/obj <label>
 *
 * This operator works by assigning a nil to the target object. This
 * causes any value that might be there to be garbage collected, thus
 * deleting the object.
 */
તાર્કિક of_DELETE_OBJ(vthread_t thr, vvp_code_t cp)
{
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_send_object(ptr, vvp_object_t(), thr->wt_context);

      પાછા સાચું;
}

/* %reverse/obj <label>
 * Reverse element order of the queue or dynamic array object at this net.
 */
તાર્કિક of_REVERSE_OBJ(vthread_t /*thr*/, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      પ્રતિજ્ઞા(obj);

      vvp_object_t stor = obj->get_object();
      જો (stor.test_nil()) પાછા સાચું;

	/* Match other queue ops: peek queues before vvp_darray so concrete
	   queue types dispatch reverse_elems() correctly. */
      જો (vvp_queue* queue = stor.peek<vvp_queue>()) {
	    queue->reverse_elems();
	    પાછા સાચું;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      પ્રતિજ્ઞા(darray);
      darray->reverse_elems();

      પાછા સાચું;
}

/* %reverse/prop/obj <property-index>
 * Reverse queue/darray property storage inside the class object on stack top.
 */
તાર્કિક of_REVERSE_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->સંખ્યા;

      vvp_object_t& ટોચ = thr->peek_object();
      vvp_cobject*cobj = ટોચ.peek<vvp_cobject>();
      પ્રતિજ્ઞા(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      જો (arr_obj.test_nil()) પાછા સાચું;

      જો (vvp_queue* queue = arr_obj.peek<vvp_queue>()) {
	    queue->reverse_elems();
	    પાછા સાચું;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      પ્રતિજ્ઞા(darray);
      darray->reverse_elems();

      પાછા સાચું;
}

/* %sort/obj — ascending sort */
તાર્કિક of_SORT_OBJ(vthread_t /*thr*/, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      પ્રતિજ્ઞા(obj);

      vvp_object_t stor = obj->get_object();
      જો (stor.test_nil()) પાછા સાચું;

      જો (vvp_queue* queue = stor.peek<vvp_queue>()) {
	    queue->sort_elems(સાચું);
	    પાછા સાચું;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      પ્રતિજ્ઞા(darray);
      darray->sort_elems(સાચું);

      પાછા સાચું;
}

/* %sort/prop/obj */
તાર્કિક of_SORT_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->સંખ્યા;

      vvp_object_t& ટોચ = thr->peek_object();
      vvp_cobject*cobj = ટોચ.peek<vvp_cobject>();
      પ્રતિજ્ઞા(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      જો (arr_obj.test_nil()) પાછા સાચું;

      જો (vvp_queue* queue = arr_obj.peek<vvp_queue>()) {
	    queue->sort_elems(સાચું);
	    પાછા સાચું;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      પ્રતિજ્ઞા(darray);
      darray->sort_elems(સાચું);

      પાછા સાચું;
}

/* %rsort/obj — descending sort */
તાર્કિક of_RSORT_OBJ(vthread_t /*thr*/, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      પ્રતિજ્ઞા(obj);

      vvp_object_t stor = obj->get_object();
      જો (stor.test_nil()) પાછા સાચું;

      જો (vvp_queue* queue = stor.peek<vvp_queue>()) {
	    queue->sort_elems(ખોટું);
	    પાછા સાચું;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      પ્રતિજ્ઞા(darray);
      darray->sort_elems(ખોટું);

      પાછા સાચું;
}

/* %rsort/prop/obj */
તાર્કિક of_RSORT_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->સંખ્યા;

      vvp_object_t& ટોચ = thr->peek_object();
      vvp_cobject*cobj = ટોચ.peek<vvp_cobject>();
      પ્રતિજ્ઞા(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      જો (arr_obj.test_nil()) પાછા સાચું;

      જો (vvp_queue* queue = arr_obj.peek<vvp_queue>()) {
	    queue->sort_elems(ખોટું);
	    પાછા સાચું;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      પ્રતિજ્ઞા(darray);
      darray->sort_elems(ખોટું);

      પાછા સાચું;
}

/* %shuffle/obj */
તાર્કિક of_SHUFFLE_OBJ(vthread_t /*thr*/, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      પ્રતિજ્ઞા(obj);

      vvp_object_t stor = obj->get_object();
      જો (stor.test_nil()) પાછા સાચું;

      જો (vvp_queue* queue = stor.peek<vvp_queue>()) {
	    queue->shuffle_elems();
	    પાછા સાચું;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      પ્રતિજ્ઞા(darray);
      darray->shuffle_elems();

      પાછા સાચું;
}

/* %shuffle/prop/obj */
તાર્કિક of_SHUFFLE_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->સંખ્યા;

      vvp_object_t& ટોચ = thr->peek_object();
      vvp_cobject*cobj = ટોચ.peek<vvp_cobject>();
      પ્રતિજ્ઞા(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      જો (arr_obj.test_nil()) પાછા સાચું;

      જો (vvp_queue* queue = arr_obj.peek<vvp_queue>()) {
	    queue->shuffle_elems();
	    પાછા સાચું;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      પ્રતિજ્ઞા(darray);
      darray->shuffle_elems();

      પાછા સાચું;
}

/* %delete/tail <label>, idx
 *
 * Remove all elements after the one specified.
 */
તાર્કિક of_DELETE_TAIL(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      પ્રતિજ્ઞા(obj);

      vvp_queue*queue = obj->get_object().peek<vvp_queue>();
      પ્રતિજ્ઞા(queue);

      unsigned idx = thr->શબ્દો[cp->bit_idx[0]].w_int;
      queue->erase_tail(idx);

      પાછા સાચું;
}

સ્થિર તાર્કિક do_disable(vthread_t thr, vthread_t match)
{
      તાર્કિક flag = ખોટું;

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
      જ્યારે (! thr->children.empty()) {

	    vthread_t tmp = *(thr->children.begin());
	    પ્રતિજ્ઞા(tmp);
	    પ્રતિજ્ઞા(tmp->parent == thr);
	    thr->i_am_joining = 0;
	    જો (do_disable(tmp, match))
		  flag = સાચું;

	    vthread_reap(tmp);
      }

      vthread_t parent = thr->parent;
      જો (parent && parent->i_am_joining) {
	      // If a parent is waiting in a %join, wake it up. Note
	      // that it is possible to be waiting in a %join yet
	      // already scheduled if multiple child threads are
	      // ending. So check if the thread is already scheduled
	      // before scheduling it again.
	    parent->i_am_joining = 0;
	    જો (! parent->i_have_ended)
		  schedule_vthread(parent, 0, સાચું);

	    do_join(parent, thr);

      } નહીંતર જો (parent) {
	      /* If the parent is yet to %join me, let its %join
		 do the reaping. */
	      //assert(tmp->is_scheduled == 0);

      } નહીંતર {
	      /* No parent at all. Goodbye. */
	    vthread_reap(thr);
      }

      પાછા flag || (thr == match);
}

/*
 * Implement the %disable instruction by scanning the target scope for
 * all the target threads. Kill the target threads and wake up a
 * parent that is attempting a %join.
 */
તાર્કિક of_DISABLE(vthread_t thr, vvp_code_t cp)
{
      __vpiScope*scope = static_cast<__vpiScope*>(cp->handle);

      તાર્કિક disabled_myself_flag = ખોટું;

      જ્યારે (! scope->threads.empty()) {
	    set<vthread_t>::iterator cur = scope->threads.begin();

	    જો (do_disable(*cur, thr))
		  disabled_myself_flag = સાચું;
      }

      પાછા ! disabled_myself_flag;
}

/*
 * Similar to `of_DISABLE`. But will only disable a single thread of the
 * specified scope. The disabled thread will be the thread closest to the
 * current thread in thread hierarchy. This can either be the current thread,
 * either the thread itself or one of its parents.
 * This is used for SystemVerilog flow control instructions like `return`,
 * `continue` and `break`.
 */

તાર્કિક of_DISABLE_FLOW(vthread_t thr, vvp_code_t cp)
{
      અચલ __vpiScope*scope = static_cast<__vpiScope*>(cp->handle);
      vthread_t cur = thr;

      જ્યારે (cur && cur->parent_scope != scope)
	    cur = cur->parent;

      પ્રતિજ્ઞા(cur);
      જો (cur)
	    પાછા !do_disable(cur, thr);

      પાછા ખોટું;
}

/*
 * Implement the %disable/fork (SystemVerilog) instruction by disabling
 * all the detached children of the given thread.
 */
તાર્કિક of_DISABLE_FORK(vthread_t thr, vvp_code_t)
{
	/* If a %disable/fork is being executed then the parent thread
	 * cannot be waiting in a join. */
      પ્રતિજ્ઞા(! thr->i_am_joining);

	/* There should be no active children to disable. */
      પ્રતિજ્ઞા(thr->children.empty());

	/* Disable any detached children. */
      જ્યારે (! thr->detached_children.empty()) {
	    vthread_t child = *(thr->detached_children.begin());
	    પ્રતિજ્ઞા(child);
	    પ્રતિજ્ઞા(child->parent == thr);
	      /* Disabling the children can never match the parent thread. */
	    તાર્કિક res = do_disable(child, thr);
	    પ્રતિજ્ઞા(! res);
	    vthread_reap(child);
      }

      પાછા સાચું;
}

/*
 * This function divides a 2-word number {high, a} by a 1-word
 * number. Assume that high < b.
 */
સ્થિર unsigned long divide2words(unsigned long a, unsigned long b,
				  unsigned long ઉપલું)
{
      unsigned long result = 0;
      જ્યારે (ઉપલું > 0) {
	    unsigned long tmp_result = ULONG_MAX / b;
	    unsigned long remain = ULONG_MAX % b;

	    remain += 1;
	    જો (remain >= b) {
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
	    result += tmp_result * ઉપલું;

	      // The new iteration starts with high*remain + a.
	    remain = multiply_with_carry(ઉપલું, remain, ઉપલું);
	    a += remain;
            જો(a < remain)
              ઉપલું += 1;

	      // Now result*b + {high,a} == the input {high,a}. It is
	      // possible that the new high >= 1. If so, it will
	      // certainly be less than high from the previous
	      // iteration. Do another iteration and it will shrink,
	      // eventually to 0.
      }

	// high is now 0, so a is the remaining remainder, so we can
	// finish off the integer divide with a simple a/b.

      પાછા result + a/b;
}

સ્થિર unsigned long* divide_bits(unsigned long*ap, અચલ unsigned long*bp, unsigned wid)
{
	// Do all our work a cpu-word at a time. The "words" variable
	// is the number of words of the wid.
      unsigned શબ્દો = (wid+CPU_WORD_BITS-1) / CPU_WORD_BITS;

      unsigned btop = શબ્દો-1;
      જ્યારે (btop > 0 && bp[btop] == 0)
	    btop -= 1;

	// Detect divide by 0, and exit.
      જો (btop==0 && bp[0]==0)
	    પાછા 0;

	// The result array will eventually accumulate the result. The
	// diff array is a difference that we use in the intermediate.
      unsigned long*diff  = નવું unsigned long[શબ્દો];
      unsigned long*result= નવું unsigned long[શબ્દો];
      માટે (unsigned idx = 0 ; idx < શબ્દો ; idx += 1)
	    result[idx] = 0;

      માટે (unsigned cur = શબ્દો-btop ; cur > 0 ; cur -= 1) {
	    unsigned cur_ptr = cur-1;
	    unsigned long cur_res;
	    જો (ap[cur_ptr+btop] >= bp[btop]) {
		  unsigned long ઉપલું = 0;
		  જો (cur_ptr+btop+1 < શબ્દો)
			ઉપલું = ap[cur_ptr+btop+1];
		  cur_res = divide2words(ap[cur_ptr+btop], bp[btop], ઉપલું);

	    } નહીંતર જો (cur_ptr+btop+1 >= શબ્દો) {
		  ચાલુ;

	    } નહીંતર જો (ap[cur_ptr+btop+1] == 0) {
		  ચાલુ;

	    } નહીંતર {
		  cur_res = divide2words(ap[cur_ptr+btop], bp[btop],
					 ap[cur_ptr+btop+1]);
	    }

	      // cur_res is a guesstimate of the result this far. It
	      // may be 1 too big. (But it will also be >0) Try it,
	      // and if the difference comes out negative, then adjust.

	      // diff = (bp * cur_res)  << cur_ptr;
	    multiply_array_imm(diff+cur_ptr, bp, શબ્દો-cur_ptr, cur_res);
	      // ap -= diff
	    unsigned long carry = 1;
	    માટે (unsigned idx = cur_ptr ; idx < શબ્દો ; idx += 1)
		  ap[idx] = add_with_carry(ap[idx], ~diff[idx], carry);

	      // ap has the diff subtracted out of it. If cur_res was
	      // too large, then ap will turn negative. (We easily
	      // tell that ap turned negative by looking at
	      // carry&1. If it is 0, then it is *negative*.) In that
	      // case, we know that cur_res was too large by 1. Correct by
	      // adding 1b back in and reducing cur_res.
	    જો ((carry&1) == 0) {
		    // Keep adding b back in until the remainder
		    // becomes positive again.
		  કરો {
			cur_res -= 1;
			carry = 0;
			માટે (unsigned idx = cur_ptr ; idx < શબ્દો ; idx += 1)
			      ap[idx] = add_with_carry(ap[idx], bp[idx-cur_ptr], carry);
		  } જ્યારે (carry == 0);
	    }

	    result[cur_ptr] = cur_res;
      }

	// Now ap contains the remainder and result contains the
	// desired result. We should find that:
	//  input-a = bp * result + ap;

      delete[]diff;
      પાછા result;
}

/*
 * %div
 */
તાર્કિક of_DIV(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valb = thr->કાઢો_vec4();
      vvp_vector4_t vala = thr->કાઢો_vec4();

      પ્રતિજ્ઞા(vala.size()== valb.size());
      unsigned wid = vala.size();

      unsigned long*ap = vala.subarray(0, wid);
      જો (ap == 0) {
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->ધકેલો_vec4(tmp);
	    પાછા સાચું;
      }

      unsigned long*bp = valb.subarray(0, wid);
      જો (bp == 0) {
	    delete[]ap;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->ધકેલો_vec4(tmp);
	    પાછા સાચું;
      }

	// If the value fits in a single CPU word, then do it the easy way.
      જો (wid <= CPU_WORD_BITS) {
	    જો (bp[0] == 0) {
		  vvp_vector4_t tmp(wid, BIT4_X);
		  thr->ધકેલો_vec4(tmp);
	    } નહીંતર {
		  ap[0] /= bp[0];
		  vala.setarray(0, wid, ap);
		  thr->ધકેલો_vec4(vala);
	    }
	    delete[]ap;
	    delete[]bp;
	    પાછા સાચું;
      }

      unsigned long*result = divide_bits(ap, bp, wid);
      જો (result == 0) {
	    delete[]ap;
	    delete[]bp;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->ધકેલો_vec4(tmp);
	    પાછા સાચું;
      }

	// Now ap contains the remainder and result contains the
	// desired result. We should find that:
	//  input-a = bp * result + ap;

      vala.setarray(0, wid, result);
      thr->ધકેલો_vec4(vala);
      delete[]ap;
      delete[]bp;
      delete[]result;

      પાછા સાચું;
}


સ્થિર શૂન્ય negate_શબ્દો(unsigned long*val, unsigned શબ્દો)
{
      unsigned long carry = 1;
      માટે (unsigned idx = 0 ; idx < શબ્દો ; idx += 1)
	    val[idx] = add_with_carry(0, ~val[idx], carry);
}

/*
 * %div/s
 */
તાર્કિક of_DIV_S(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valb = thr->કાઢો_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      પ્રતિજ્ઞા(vala.size()== valb.size());
      unsigned wid = vala.size();
      unsigned શબ્દો = (wid + CPU_WORD_BITS - 1) / CPU_WORD_BITS;

	// Get the values, left in right, in binary form. If there is
	// a problem with either (caused by an X or Z bit) then we
	// know right away that the entire result is X.
      unsigned long*ap = vala.subarray(0, wid);
      જો (ap == 0) {
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    પાછા સાચું;
      }

      unsigned long*bp = valb.subarray(0, wid);
      જો (bp == 0) {
	    delete[]ap;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    પાછા સાચું;
      }

	// Sign extend the bits in the array to fill out the array.
      unsigned long sign_mask = 0;
      જો (unsigned long sign_bits = (શબ્દો*CPU_WORD_BITS) - wid) {
	    sign_mask = -1UL << (CPU_WORD_BITS-sign_bits);
	    જો (ap[શબ્દો-1] & (sign_mask>>1))
		  ap[શબ્દો-1] |= sign_mask;
	    જો (bp[શબ્દો-1] & (sign_mask>>1))
		  bp[શબ્દો-1] |= sign_mask;
      }

	// If the value fits in a single word, then use the native divide.
      જો (wid <= CPU_WORD_BITS) {
	    જો (bp[0] == 0) {
		  vvp_vector4_t tmp(wid, BIT4_X);
		  vala = tmp;
	    } નહીંતર જો (((long)ap[0] == LONG_MIN) && ((long)bp[0] == -1)) {
		  vvp_vector4_t tmp(wid, BIT4_0);
		  tmp.set_bit(wid-1, BIT4_1);
		  vala = tmp;
	    } નહીંતર {
		  long tmpa = (long) ap[0];
		  long tmpb = (long) bp[0];
		  long res = tmpa / tmpb;
		  ap[0] = ((unsigned long)res) & ~sign_mask;
		  vala.setarray(0, wid, ap);
	    }
	    delete[]ap;
	    delete[]bp;
	    પાછા સાચું;
      }

	// We need to the actual division to positive integers. Make
	// them positive here, and remember the negations.
      તાર્કિક negate_flag = ખોટું;
      જો ( ((long) ap[શબ્દો-1]) < 0 ) {
	    negate_flag = સાચું;
	    negate_શબ્દો(ap, શબ્દો);
      }
      જો ( ((long) bp[શબ્દો-1]) < 0 ) {
	    negate_flag ^= સાચું;
	    negate_શબ્દો(bp, શબ્દો);
      }

      unsigned long*result = divide_bits(ap, bp, wid);
      જો (result == 0) {
	    delete[]ap;
	    delete[]bp;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    પાછા સાચું;
      }

      જો (negate_flag) {
	    negate_શબ્દો(result, શબ્દો);
      }

      result[શબ્દો-1] &= ~sign_mask;

      vala.setarray(0, wid, result);
      delete[]ap;
      delete[]bp;
      delete[]result;
      પાછા સાચું;
}

તાર્કિક of_DIV_WR(vthread_t thr, vvp_code_t)
{
      દ્વિગુણ r = thr->કાઢો_real();
      દ્વિગુણ l = thr->કાઢો_real();
      thr->ધકેલો_real(l / r);

      પાછા સાચું;
}

/*
 * %dup/obj
 * %dup/real
 * %dup/vec4
 *
 * Push a duplicate of the object on the appropriate stack.
 */
તાર્કિક of_DUP_OBJ(vthread_t thr, vvp_code_t)
{
      vvp_object_t src = thr->peek_object();

        // If it is null push a new null object
      જો (src.test_nil())
	    thr->ધકેલો_object(vvp_object_t());
      નહીંતર
	    thr->ધકેલો_object(src.duplicate());

      પાછા સાચું;
}

તાર્કિક of_DUP_REAL(vthread_t thr, vvp_code_t)
{
      thr->ધકેલો_real(thr->peek_real(0));
      પાછા સાચું;
}

તાર્કિક of_DUP_VEC4(vthread_t thr, vvp_code_t)
{
      thr->ધકેલો_vec4(thr->peek_vec4(0));
      પાછા સાચું;
}

/*
 * This terminates the current thread. If there is a parent who is
 * waiting for me to die, then I schedule it. At any rate, I mark
 * myself as a zombie by setting my pc to 0.
 */
તાર્કિક of_END(vthread_t thr, vvp_code_t)
{
      પ્રતિજ્ઞા(! thr->waiting_for_event);
      thr->i_have_ended = 1;
      thr->pc = codespace_null();

	/* Fully detach any detached children. */
      જ્યારે (! thr->detached_children.empty()) {
	    vthread_t child = *(thr->detached_children.begin());
	    પ્રતિજ્ઞા(child);
	    પ્રતિજ્ઞા(child->parent == thr);
	    પ્રતિજ્ઞા(child->i_am_detached);
	    child->parent = 0;
	    child->i_am_detached = 0;
	    thr->detached_children.erase(thr->detached_children.begin());
      }

	/* It is an error to still have active children running at this
	 * point in time. They should have all been detached or joined. */
      પ્રતિજ્ઞા(thr->children.empty());

	/* If I have a parent who is waiting for me, then mark that I
	   have ended, and schedule that parent. Also, finish the
	   %join for the parent. */
      જો (!thr->i_am_detached && thr->parent && thr->parent->i_am_joining) {
	    vthread_t tmp = thr->parent;
	    પ્રતિજ્ઞા(! thr->i_am_detached);

	    tmp->i_am_joining = 0;
	    schedule_vthread(tmp, 0, સાચું);
	    do_join(tmp, thr);
	    પાછા ખોટું;
      }

	/* If this thread is not fully detached then remove it from the
	 * parents detached_children set and reap it. */
      જો (thr->i_am_detached) {
	    vthread_t tmp = thr->parent;
	    પ્રતિજ્ઞા(tmp);
	    size_t res = tmp->detached_children.erase(thr);
	    પ્રતિજ્ઞા(res == 1);
	      /* If the parent is waiting for the detached children to
	       * finish then the last detached child needs to tell the
	       * parent to wake up when it is finished. */
	    જો (tmp->i_am_waiting && tmp->detached_children.empty()) {
		  tmp->i_am_waiting = 0;
		  schedule_vthread(tmp, 0, સાચું);
	    }
	      /* Fully detach this thread so it will be reaped below. */
	    thr->i_am_detached = 0;
	    thr->parent = 0;
      }

	/* If I have no parent, then no one can %join me and there is
	 * no reason to stick around. This can happen, for example if
	 * I am an ``initial'' thread. */
      જો (thr->parent == 0) {
	    vthread_reap(thr);
	    પાછા ખોટું;
      }

	/* If I make it this far, then I have a parent who may wish
	   to %join me. Remain a zombie so that it can. */

      પાછા ખોટું;
}

/*
 * %event <var-label>
 */
તાર્કિક of_EVENT(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_vector4_t tmp (1, BIT4_X);
      vvp_send_vec4(ptr, tmp, thr->wt_context);
      પાછા સાચું;
}

/*
 * %event/nb <var-label>, <delay>
 */
તાર્કિક of_EVENT_NB(vthread_t thr, vvp_code_t cp)
{
      vvp_time64_t delay;

      delay = thr->શબ્દો[cp->bit_idx[0]].w_uint;
      schedule_propagate_event(cp->net, delay);
      પાછા સાચું;
}

તાર્કિક of_EVCTL(vthread_t thr, vvp_code_t cp)
{
      પ્રતિજ્ઞા(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      thr->ecount = thr->શબ્દો[cp->bit_idx[0]].w_uint;
      પાછા સાચું;
}
તાર્કિક of_EVCTLC(vthread_t thr, vvp_code_t)
{
      thr->event = 0;
      thr->ecount = 0;
      પાછા સાચું;
}

તાર્કિક of_EVCTLI(vthread_t thr, vvp_code_t cp)
{
      પ્રતિજ્ઞા(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      thr->ecount = cp->bit_idx[0];
      પાછા સાચું;
}

તાર્કિક of_EVCTLS(vthread_t thr, vvp_code_t cp)
{
      પ્રતિજ્ઞા(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      int64_t val = thr->શબ્દો[cp->bit_idx[0]].w_int;
      જો (val < 0) val = 0;
      thr->ecount = val;
      પાછા સાચું;
}

તાર્કિક of_FLAG_GET_VEC4(vthread_t thr, vvp_code_t cp)
{
      પૂર્ણાંક flag = cp->સંખ્યા;
      પ્રતિજ્ઞા(flag < vthread_s::FLAGS_ગણતરી);

      vvp_vector4_t val (1, thr->flags[flag]);
      thr->ધકેલો_vec4(val);

      પાછા સાચું;
}

/*
 * %flag_inv <flag1>
 */
તાર્કિક of_FLAG_INV(vthread_t thr, vvp_code_t cp)
{
      પૂર્ણાંક flag1 = cp->bit_idx[0];

      thr->flags[flag1] = ~ thr->flags[flag1];
      પાછા સાચું;
}

/*
 * %flag_mov <flag1>, <flag2>
 */
તાર્કિક of_FLAG_MOV(vthread_t thr, vvp_code_t cp)
{
      પૂર્ણાંક flag1 = cp->bit_idx[0];
      પૂર્ણાંક flag2 = cp->bit_idx[1];

      thr->flags[flag1] = thr->flags[flag2];
      પાછા સાચું;
}

/*
 * %flag_or <flag1>, <flag2>
 */
તાર્કિક of_FLAG_OR(vthread_t thr, vvp_code_t cp)
{
      પૂર્ણાંક flag1 = cp->bit_idx[0];
      પૂર્ણાંક flag2 = cp->bit_idx[1];

      thr->flags[flag1] = thr->flags[flag1] | thr->flags[flag2];
      પાછા સાચું;
}

તાર્કિક of_FLAG_SET_IMM(vthread_t thr, vvp_code_t cp)
{
      પૂર્ણાંક flag = cp->સંખ્યા;
      પૂર્ણાંક vali = cp->bit_idx[0];

      પ્રતિજ્ઞા(flag < vthread_s::FLAGS_ગણતરી);
      પ્રતિજ્ઞા(vali >= 0 && vali < 4);

      સ્થિર અચલ vvp_bit4_t map_bit[4] = {BIT4_0, BIT4_1, BIT4_Z, BIT4_X};
      thr->flags[flag] = map_bit[vali];
      પાછા સાચું;
}

તાર્કિક of_FLAG_SET_VEC4(vthread_t thr, vvp_code_t cp)
{
      પૂર્ણાંક flag = cp->સંખ્યા;
      પ્રતિજ્ઞા(flag < vthread_s::FLAGS_ગણતરી);

      અચલ vvp_vector4_t&val = thr->peek_vec4();
      thr->flags[flag] = val.value(0);
      thr->કાઢો_vec4(1);

      પાછા સાચું;
}

/*
 * the %force/link instruction connects a source node to a
 * destination node. The destination node must be a signal, as it is
 * marked with the source of the force so that it may later be
 * unlinked without specifically knowing the source that this
 * instruction used.
 */
તાર્કિક of_FORCE_LINK(vthread_t, vvp_code_t cp)
{
      vvp_net_t*dst = cp->net;
      vvp_net_t*src = cp->net2;

      પ્રતિજ્ઞા(dst->fil);
      dst->fil->force_link(dst, src);

      પાછા સાચું;
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
તાર્કિક of_FORCE_VEC4(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_vector4_t value = thr->કાઢો_vec4();

	/* Send the force value to the filter on the node. */

      પ્રતિજ્ઞા(net->fil);
      જો (value.size() != net->fil->filter_size())
	    value = coerce_to_width(value, net->fil->filter_size());

      net->force_vec4(value, vvp_vector2_t(vvp_vector2_t::FILL1, net->fil->filter_size()));

      પાછા સાચું;
}

/*
 * %force/vec4/off <net>, <off>
 */
તાર્કિક of_FORCE_VEC4_OFF(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      unsigned base_idx = cp->bit_idx[0];
      long base = thr->શબ્દો[base_idx].w_int;
      vvp_vector4_t value = thr->કાઢો_vec4();
      unsigned wid = value.size();

      પ્રતિજ્ઞા(net->fil);

      જો (thr->flags[4] == BIT4_1)
	    પાછા સાચું;

	// This is the width of the target vector.
      unsigned use_size = net->fil->filter_size();

      જો (base >= (long)use_size)
	    પાછા સાચું;
      જો (base < -(long)use_size)
	    પાછા સાચું;

      જો ((base + wid) > use_size)
	    wid = use_size - base;

	// Make a mask of which bits are to be forced, 0 for unforced
	// bits and 1 for forced bits.
      vvp_vector2_t mask (vvp_vector2_t::FILL0, use_size);
      માટે (unsigned idx = 0 ; idx < wid ; idx += 1)
	    mask.set_bit(base+idx, 1);

      vvp_vector4_t tmp (use_size, BIT4_Z);

	// vvp_net_t::force_vec4 propagates all the bits of the
	// forced vector value, regardless of the mask. This
	// ensures the unforced bits retain their current value.
      vvp_signal_કિંમત*sig = dynamic_cast<vvp_signal_કિંમત*>(net->fil);
      પ્રતિજ્ઞા(sig);
      sig->vec4_કિંમત(tmp);

      tmp.set_vec(base, value);

      net->force_vec4(tmp, mask);
      પાછા સાચું;
}

/*
 * %force/vec4/off/d <net>, <off>, <del>
 */
તાર્કિક of_FORCE_VEC4_OFF_D(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      unsigned base_idx = cp->bit_idx[0];
      long base = thr->શબ્દો[base_idx].w_int;

      unsigned delay_idx = cp->bit_idx[1];
      vvp_time64_t delay = thr->શબ્દો[delay_idx].w_uint;

      vvp_vector4_t value = thr->કાઢો_vec4();

      પ્રતિજ્ઞા(net->fil);

      જો (thr->flags[4] == BIT4_1)
	    પાછા સાચું;

	// This is the width of the target vector.
      unsigned use_size = net->fil->filter_size();

      જો (base >= (long)use_size)
	    પાછા સાચું;
      જો (base < -(long)use_size)
	    પાછા સાચું;

      schedule_force_vector(net, base, use_size, value, delay);
      પાછા સાચું;
}

તાર્કિક of_FORCE_WR(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net  = cp->net;
      દ્વિગુણ value = thr->કાઢો_real();

      net->force_real(value, vvp_vector2_t(vvp_vector2_t::FILL1, 1));

      પાછા સાચું;
}

/*
 * The %fork instruction causes a new child to be created and pushed
 * in front of any existing child. This causes the new child to be
 * added to the list of children, and for me to be the parent of the
 * new child.
 */
તાર્કિક of_FORK(vthread_t thr, vvp_code_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);

      જો (cp->scope->is_automatic()) {
              /* The context allocated for this child is the top entry
                 on the write context stack. */
            child->wt_context = thr->wt_context;
            child->rd_context = thr->wt_context;
      }

      child->parent = thr;
      thr->children.insert(child);

      જો (thr->i_am_in_function) {
	    child->is_scheduled = 1;
	    child->i_am_in_function = 1;
	    vthread_run(child);
	    running_thread = thr;
      } નહીંતર {
	    schedule_vthread(child, 0, સાચું);
      }
      પાછા સાચું;
}

તાર્કિક of_FREE(vthread_t thr, vvp_code_t cp)
{
        /* Pop the child context from the read context stack. */
      vvp_context_t child_context = thr->rd_context;
      thr->rd_context = vvp_get_stacked_context(child_context);

        /* Free the context. */
      vthread_free_context(child_context, cp->scope);

      પાછા સાચું;
}

/*
 * %inv
 *
 * Logically, this pops a value, inverts is (Verilog style, with Z and
 * X converted to X) and pushes the result. We can more efficiently
 * just to the invert in place.
 */
તાર્કિક of_INV(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val.invert();
      પાછા સાચું;
}


/*
 * Index registers, arithmetic.
 */

સ્થિર અંતઃરેખ int64_t get_as_64_bit(uint32_t નીચલું_32, uint32_t ઉપલું_32)
{
      int64_t નીચલું = નીચલું_32;
      int64_t res = ઉપલું_32;

      res <<= 32;
      res |= નીચલું;
      પાછા res;
}

તાર્કિક of_IX_ADD(vthread_t thr, vvp_code_t cp)
{
      thr->શબ્દો[cp->સંખ્યા].w_int += get_as_64_bit(cp->bit_idx[0],
                                                    cp->bit_idx[1]);
      પાછા સાચું;
}

તાર્કિક of_IX_SUB(vthread_t thr, vvp_code_t cp)
{
      thr->શબ્દો[cp->સંખ્યા].w_int -= get_as_64_bit(cp->bit_idx[0],
                                                    cp->bit_idx[1]);
      પાછા સાચું;
}

તાર્કિક of_IX_MUL(vthread_t thr, vvp_code_t cp)
{
      thr->શબ્દો[cp->સંખ્યા].w_int *= get_as_64_bit(cp->bit_idx[0],
                                                    cp->bit_idx[1]);
      પાછા સાચું;
}

તાર્કિક of_IX_LOAD(vthread_t thr, vvp_code_t cp)
{
      thr->શબ્દો[cp->સંખ્યા].w_int = get_as_64_bit(cp->bit_idx[0],
                                                   cp->bit_idx[1]);
      પાછા સાચું;
}

તાર્કિક of_IX_MOV(vthread_t thr, vvp_code_t cp)
{
      thr->શબ્દો[cp->bit_idx[0]].w_int = thr->શબ્દો[cp->bit_idx[1]].w_int;
      પાછા સાચું;
}

તાર્કિક of_IX_GETV(vthread_t thr, vvp_code_t cp)
{
      unsigned index = cp->bit_idx[0];
      vvp_net_t*net = cp->net;

      vvp_signal_કિંમત*sig = dynamic_cast<vvp_signal_કિંમત*>(net->fil);
      જો (sig == 0) {
	    પ્રતિજ્ઞા(net->fil);
	    cerr << thr->get_fileline()
	         << "%%ix/getv error: Net arg not a vector signal? "
		 << typeid(*net->fil).name() << endl;
      }
      પ્રતિજ્ઞા(sig);

      vvp_vector4_t vec;
      sig->vec4_કિંમત(vec);
      તાર્કિક overflow_flag;
      uint64_t val = 0;
      તાર્કિક known_flag = vector4_to_કિંમત(vec, overflow_flag, val);

      જો (known_flag)
	    thr->શબ્દો[index].w_uint = val;
      નહીંતર
	    thr->શબ્દો[index].w_uint = 0;

	/* Set bit 4 as a flag if the input is unknown. */
      thr->flags[4] = known_flag ? (overflow_flag ? BIT4_X : BIT4_0) : BIT4_1;

      પાછા સાચું;
}

તાર્કિક of_IX_GETV_S(vthread_t thr, vvp_code_t cp)
{
      unsigned index = cp->bit_idx[0];
      vvp_net_t*net = cp->net;

      vvp_signal_કિંમત*sig = dynamic_cast<vvp_signal_કિંમત*>(net->fil);
      જો (sig == 0) {
	    પ્રતિજ્ઞા(net->fil);
	    cerr << thr->get_fileline()
	         << "%%ix/getv/s error: Net arg not a vector signal? "
		 << "fun=" << typeid(*net->fil).name()
		 << ", fil=" << (net->fil? typeid(*net->fil).name() : "<>")
		 << endl;
      }
      પ્રતિજ્ઞા(sig);

      vvp_vector4_t vec;
      sig->vec4_કિંમત(vec);
      int64_t val;
      તાર્કિક known_flag = vector4_to_કિંમત(vec, val, સાચું, સાચું);

      જો (known_flag)
	    thr->શબ્દો[index].w_int = val;
      નહીંતર
	    thr->શબ્દો[index].w_int = 0;

	/* Set bit 4 as a flag if the input is unknown. */
      thr->flags[4] = known_flag? BIT4_0 : BIT4_1;

      પાછા સાચું;
}

સ્થિર uint64_t vec4_to_index(vthread_t thr, તાર્કિક signed_flag)
{
	// Get all the information we need about the vec4 vector, then
	// pop it away. We only need the bool bits and the length.
      અચલ vvp_vector4_t&val = thr->peek_vec4();
      unsigned val_size = val.size();
      unsigned long*bits = val.subarray(0, val_size, ખોટું);
      thr->કાઢો_vec4(1);

	// If there are X/Z bits, then the subarray will give us a nil
	// pointer. Set a flag to indicate the error, and give up.
      જો (bits == 0) {
	    thr->flags[4] = BIT4_1;
	    પાછા 0;
      }

      uint64_t v = 0;
      thr->flags[4] = BIT4_0;

      પ્રતિજ્ઞા(કદ(bits[0]) <= કદ(v));

      v = 0;
      માટે (unsigned idx = 0 ; idx < val_size ; idx += 8*કદ(bits[0])) {
	    uint64_t tmp = bits[idx/8/કદ(bits[0])];
	    જો (idx < 8*કદ(v)) {
		  v |= tmp << idx;
	    } નહીંતર {
		  તાર્કિક overflow = signed_flag && (v >> 63) ? ~tmp != 0 : tmp != 0;
		  જો (overflow) {
			thr->flags[4] = BIT4_X;
			તોડો;
		  }
	    }
      }

	// Set the high bits that are not necessarily filled in by the
	// subarray function.
      જો (val_size < 8*કદ(v)) {
	    જો (signed_flag && (v & (static_cast<uint64_t>(1)<<(val_size-1)))) {
		    // Propagate the sign bit...
		  v |= (~static_cast<uint64_t>(0)) << val_size;

	    } નહીંતર {
		    // Fill with zeros.
		  v &= ~((~static_cast<uint64_t>(0)) << val_size);
	    }

      }

      delete[]bits;
      પાછા v;
}

/*
 * %ix/vec4 <idx>
 */
તાર્કિક of_IX_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned use_idx = cp->સંખ્યા;
      thr->શબ્દો[use_idx].w_uint = vec4_to_index(thr, ખોટું);
      પાછા સાચું;
}

/*
 * %ix/vec4/s <idx>
 */
તાર્કિક of_IX_VEC4_S(vthread_t thr, vvp_code_t cp)
{
      unsigned use_idx = cp->સંખ્યા;
      thr->શબ્દો[use_idx].w_uint = vec4_to_index(thr, સાચું);
      પાછા સાચું;
}

/*
 * The various JMP instruction work simply by pulling the new program
 * counter from the instruction and resuming. If the jump is
 * conditional, then test the bit for the expected value first.
 */
તાર્કિક of_JMP(vthread_t thr, vvp_code_t cp)
{
      thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      જો (schedule_stopped()) {
	    schedule_vthread(thr, 0, ખોટું);
	    પાછા ખોટું;
      }

      પાછા સાચું;
}

/*
 * %jmp/0 <pc>, <flag>
 */
તાર્કિક of_JMP0(vthread_t thr, vvp_code_t cp)
{
      જો (thr->flags[cp->bit_idx[0]] == BIT4_0)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      જો (schedule_stopped()) {
	    schedule_vthread(thr, 0, ખોટું);
	    પાછા ખોટું;
      }

      પાછા સાચું;
}

/*
 * %jmp/0xz <pc>, <flag>
 */
તાર્કિક of_JMP0XZ(vthread_t thr, vvp_code_t cp)
{
      જો (thr->flags[cp->bit_idx[0]] != BIT4_1)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      જો (schedule_stopped()) {
	    schedule_vthread(thr, 0, ખોટું);
	    પાછા ખોટું;
      }

      પાછા સાચું;
}

/*
 * %jmp/1 <pc>, <flag>
 */
તાર્કિક of_JMP1(vthread_t thr, vvp_code_t cp)
{
      જો (thr->flags[cp->bit_idx[0]] == BIT4_1)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      જો (schedule_stopped()) {
	    schedule_vthread(thr, 0, ખોટું);
	    પાછા ખોટું;
      }

      પાછા સાચું;
}

/*
 * %jmp/1xz <pc>, <flag>
 */
તાર્કિક of_JMP1XZ(vthread_t thr, vvp_code_t cp)
{
      જો (thr->flags[cp->bit_idx[0]] != BIT4_0)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      જો (schedule_stopped()) {
	    schedule_vthread(thr, 0, ખોટું);
	    પાછા ખોટું;
      }

      પાછા સાચું;
}

/*
 * The %join instruction causes the thread to wait for one child
 * to die.  If a child is already dead (and a zombie) then I reap
 * it and go on. Otherwise, I mark myself as waiting in a join so that
 * children know to wake me when they finish.
 */

સ્થિર શૂન્ય do_join(vthread_t thr, vthread_t child)
{
      પ્રતિજ્ઞા(child->parent == thr);

        /* If the immediate child thread is in an automatic scope... */
      જો (child->wt_context) {
              /* and is the top level task/function thread... */
            જો (thr->wt_context != thr->rd_context) {
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

સ્થિર તાર્કિક do_join_opcode(vthread_t thr)
{
      પ્રતિજ્ઞા( !thr->i_am_joining );
      પ્રતિજ્ઞા( !thr->children.empty());

	// Are there any children that have already ended? If so, then
	// join with that one.
      માટે (set<vthread_t>::iterator cur = thr->children.begin()
		 ; cur != thr->children.end() ; ++cur) {
	    vthread_t curp = *cur;
	    જો (! curp->i_have_ended)
		  ચાલુ;

	      // found something!
	    do_join(thr, curp);
	    પાછા સાચું;
      }

	// Otherwise, tell my children to awaken me when they end,
	// then pause.
      thr->i_am_joining = 1;
      પાછા ખોટું;
}

તાર્કિક of_JOIN(vthread_t thr, vvp_code_t)
{
      પાછા do_join_opcode(thr);
}

/*
 * This %join/detach <n> instruction causes the thread to detach
 * threads that were created by an earlier %fork.
 */
તાર્કિક of_JOIN_DETACH(vthread_t thr, vvp_code_t cp)
{
      unsigned long ગણતરી = cp->સંખ્યા;

      પ્રતિજ્ઞા(ગણતરી == thr->children.size());

      જ્યારે (! thr->children.empty()) {
	    vthread_t child = *thr->children.begin();
	    પ્રતિજ્ઞા(child->parent == thr);

	      // We cannot detach automatic tasks/functions within an
	      // automatic scope. If we try to do that, we might make
	      // a mess of the allocation of the context. Note that it
	      // is OK if the child context is distinct (See %exec_ufunc.)
	    પ્રતિજ્ઞા(child->wt_context==0 || thr->wt_context!=child->wt_context);
	    જો (child->i_have_ended) {
		    // If the child has already ended, then reap it.
		  vthread_reap(child);

	    } નહીંતર {
		  size_t res = child->parent->children.erase(child);
		  પ્રતિજ્ઞા(res == 1);
		  child->i_am_detached = 1;
		  thr->detached_children.insert(child);
	    }
      }

      પાછા સાચું;
}

/*
 * %load/ar <array-label>, <index>;
*/
તાર્કિક of_LOAD_AR(vthread_t thr, vvp_code_t cp)
{
      unsigned idx = cp->bit_idx[0];
      દ્વિગુણ word;

	/* The result is 0.0 if the address is undefined. */
      જો (thr->flags[4] == BIT4_1) {
	    word = 0.0;
      } નહીંતર {
	    unsigned adr = thr->શબ્દો[idx].w_int;
	    word = cp->array->get_word_r(adr);
      }

      thr->ધકેલો_real(word);
      પાછા સાચું;
}

template <typename ELEM>
સ્થિર તાર્કિક load_dar(vthread_t thr, vvp_code_t cp)
{
      int64_t adr = thr->શબ્દો[3].w_int;
      vvp_net_t*net = cp->net;
      પ્રતિજ્ઞા(net);

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      પ્રતિજ્ઞા(obj);

      vvp_darray*darray = obj->get_object().peek<vvp_darray>();

      ELEM word;
      જો (darray &&
          (adr >= 0) && (thr->flags[4] == BIT4_0)) // A defined address >= 0
	    darray->get_word(adr, word);
      નહીંતર
	    dq_default(word, obj->size());

      vthread_ધકેલો(thr, word);
      પાછા સાચું;
}

/*
 * %load/dar/r <array-label>;
 */
તાર્કિક of_LOAD_DAR_R(vthread_t thr, vvp_code_t cp)
{
      પાછા load_dar<દ્વિગુણ>(thr, cp);
}

/*
 * %load/dar/str <array-label>;
 */
તાર્કિક of_LOAD_DAR_STR(vthread_t thr, vvp_code_t cp)
{
      પાછા load_dar<string>(thr, cp);
}

/*
 * %load/dar/vec4 <array-label>;
 */
તાર્કિક of_LOAD_DAR_VEC4(vthread_t thr, vvp_code_t cp)
{
      પાછા load_dar<vvp_vector4_t>(thr, cp);
}

/*
 * %load/prop/dar/vec4 <pid>, <wid>;
 *   Indexed read of queue/dynamic-array class property; object on stack,
 *   index in words[3] (same as %load/dar/vec4).
 */
તાર્કિક of_LOAD_PROP_DAR_VEC4(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->સંખ્યા;
      unsigned wid = cp->bit_idx[0];
      int64_t adr = thr->શબ્દો[3].w_int;

      vvp_object_t& ટોચ = thr->peek_object();
      vvp_cobject*cobj = ટોચ.peek<vvp_cobject>();
      પ્રતિજ્ઞા(cobj);

      vvp_object_t pobj;
      cobj->get_object(pid, pobj, 0);
      vvp_darray*darray = pobj.peek<vvp_darray>();

      vvp_vector4_t word;
      જો (darray && (adr >= 0) && (thr->flags[4] == BIT4_0)) {
	    darray->get_word(adr, word);
      } નહીંતર {
	    dq_default(word, wid);
      }

      thr->ધકેલો_vec4(word);
      પાછા સાચું;
}

/*
 * %load/obj <var-label>
 */
તાર્કિક of_LOAD_OBJ(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      પ્રતિજ્ઞા(fun);

      vvp_object_t val = fun->get_object();
      thr->ધકેલો_object(val);

      પાછા સાચું;
}

/*
 * %load/obja <index>
 *    Loads the object from array, using index <index> as the index
 *    value. If flags[4] == 1, the calculation of <index> may have
 *    failed, so push nil.
 */
તાર્કિક of_LOAD_OBJA(vthread_t thr, vvp_code_t cp)
{
      unsigned idx = cp->bit_idx[0];
      vvp_object_t word;

	/* The result is 0.0 if the address is undefined. */
      જો (thr->flags[4] == BIT4_1) {
	    ; // Return nil
      } નહીંતર {
	    unsigned adr = thr->શબ્દો[idx].w_int;
	    cp->array->get_word_obj(adr, word);
      }

      thr->ધકેલો_object(word);
      પાછા સાચું;
}

/*
 * %load/real <var-label>
 */
તાર્કિક of_LOAD_REAL(vthread_t thr, vvp_code_t cp)
{
      __vpiHandle*tmp = cp->handle;
      t_vpi_કિંમત val;

      val.format = vpiRealVal;
      vpi_get_કિંમત(tmp, &val);

      thr->ધકેલો_real(val.value.real);

      પાછા સાચું;
}

/*
 * %load/str <var-label>
 */
તાર્કિક of_LOAD_STR(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;


      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (net->fun);
      પ્રતિજ્ઞા(fun);

      અચલ string&val = fun->get_string();
      thr->ધકેલો_str(val);

      પાછા સાચું;
}

તાર્કિક of_LOAD_STRA(vthread_t thr, vvp_code_t cp)
{
      unsigned idx = cp->bit_idx[0];
      string word;

      જો (thr->flags[4] == BIT4_1) {
	    word = "";
      } નહીંતર {
	    unsigned adr = thr->શબ્દો[idx].w_int;
	    word = cp->array->get_word_str(adr);
      }

      thr->ધકેલો_str(word);
      પાછા સાચું;
}


/*
 * %load/vec4 <net>
 */
તાર્કિક of_LOAD_VEC4(vthread_t thr, vvp_code_t cp)
{
	// Push a placeholder onto the stack in order to reserve the
	// stack space. Use a reference for the stack top as a target
	// for the load.
      thr->ધકેલો_vec4(vvp_vector4_t());
      vvp_vector4_t&sig_કિંમત = thr->peek_vec4();

      vvp_net_t*net = cp->net;

	// For the %load to work, the functor must actually be a
	// signal functor. Only signals save their vector value.
      vvp_signal_કિંમત*sig = dynamic_cast<vvp_signal_કિંમત*> (net->fil);
      જો (sig == 0) {
	    cerr << thr->get_fileline()
	         << "%load/v error: Net arg not a signal? "
		 << (net->fil ? typeid(*net->fil).name() :
	                        typeid(*net->fun).name())
	         << endl;
	    પ્રતિજ્ઞા(sig);
	    પાછા સાચું;
      }

	// Extract the value from the signal and directly into the
	// target stack position.
      sig->vec4_કિંમત(sig_કિંમત);

      પાછા સાચું;
}

/*
 * %load/vec4a <arr>, <adrx>
 */
તાર્કિક of_LOAD_VEC4A(vthread_t thr, vvp_code_t cp)
{
      પૂર્ણાંક adr_index = cp->bit_idx[0];

      long adr = thr->શબ્દો[adr_index].w_int;

	// If flag[3] is set, then the calculation of the address
	// failed, and this load should return X instead of the actual
	// value.
      જો (thr->flags[4] == BIT4_1) {
	    vvp_vector4_t tmp (cp->array->get_word_size(), BIT4_X);
	    thr->ધકેલો_vec4(tmp);
	    પાછા સાચું;
      }

      vvp_vector4_t tmp (cp->array->get_word(adr));
      thr->ધકેલો_vec4(tmp);
      પાછા સાચું;
}

સ્થિર શૂન્ય do_verylong_mod(vvp_vector4_t&vala, અચલ vvp_vector4_t&valb,
			    તાર્કિક ડાબું_is_neg, તાર્કિક જમણું_is_neg)
{
      તાર્કિક out_is_neg = ડાબું_is_neg;
      અચલ પૂર્ણાંક len=vala.size();
      unsigned અક્ષર *a, *z, *t;
      a = નવું unsigned અક્ષર[len+1];
      z = નવું unsigned અક્ષર[len+1];
      t = નવું unsigned અક્ષર[len+1];

      unsigned અક્ષર carry;
      unsigned અક્ષર temp;

      પૂર્ણાંક mxa = -1, mxz = -1;
      પૂર્ણાંક i;
      પૂર્ણાંક current, copylen;

      unsigned lb_carry = ડાબું_is_neg? 1 : 0;
      unsigned rb_carry = જમણું_is_neg? 1 : 0;
      માટે (પૂર્ણાંક idx = 0 ;  idx < len ;  idx += 1) {
	    unsigned lb = vala.value(idx);
	    unsigned rb = valb.value(idx);

	    જો ((lb | rb) & 2) {
		  delete []t;
		  delete []z;
		  delete []a;
		  vvp_vector4_t tmp(len, BIT4_X);
		  vala = tmp;
		  પાછા;
	    }

	    જો (ડાબું_is_neg) {
		  lb = (1-lb) + lb_carry;
		  lb_carry = (lb & ~1)? 1 : 0;
		  lb &= 1;
	    }
	    જો (જમણું_is_neg) {
		  rb = (1-rb) + rb_carry;
		  rb_carry = (rb & ~1)? 1 : 0;
		  rb &= 1;
	    }

	    z[idx]=lb;
	    a[idx]=1-rb;	// for 2s complement add..
      }

      z[len]=0;
      a[len]=1;

      માટે(i=len-1;i>=0;i--) {
	    જો(! a[i]) {
		  mxa=i;
		  તોડો;
	    }
      }

      માટે(i=len-1;i>=0;i--) {
	    જો(z[i]) {
		  mxz=i;
		  તોડો;
	    }
      }

      જો((mxa>mxz)||(mxa==-1)) {
	    જો(mxa==-1) {
		  delete []t;
		  delete []z;
		  delete []a;
		  vvp_vector4_t tmpx (len, BIT4_X);
		  vala = tmpx;
		  પાછા;
	    }

	    જાઓ tally;
      }

      copylen = mxa + 2;
      current = mxz - mxa;

      જ્યારે(current > -1) {
	    carry = 1;
	    માટે(i=0;i<copylen;i++) {
		  temp = z[i+current] + a[i] + carry;
		  t[i] = (temp&1);
		  carry = (temp>>1);
	    }

	    જો(carry) {
		  માટે(i=0;i<copylen;i++) {
			z[i+current] = t[i];
		  }
	    }

	    current--;
      }

 tally:

      vvp_vector4_t tmp (len, BIT4_X);
      carry = out_is_neg? 1 : 0;
      માટે (પૂર્ણાંક idx = 0 ;  idx < len ;  idx += 1) {
	    unsigned ob = z[idx];
	    જો (out_is_neg) {
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

તાર્કિક of_MAX_WR(vthread_t thr, vvp_code_t)
{
      દ્વિગુણ r = thr->કાઢો_real();
      દ્વિગુણ l = thr->કાઢો_real();
      જો (r != r)
	    thr->ધકેલો_real(l);
      નહીંતર જો (l != l)
	    thr->ધકેલો_real(r);
      નહીંતર જો (r < l)
	    thr->ધકેલો_real(l);
      નહીંતર
	    thr->ધકેલો_real(r);
      પાછા સાચું;
}

તાર્કિક of_MIN_WR(vthread_t thr, vvp_code_t)
{
      દ્વિગુણ r = thr->કાઢો_real();
      દ્વિગુણ l = thr->કાઢો_real();
      જો (r != r)
	    thr->ધકેલો_real(l);
      નહીંતર જો (l != l)
	    thr->ધકેલો_real(r);
      નહીંતર જો (r < l)
	    thr->ધકેલો_real(r);
      નહીંતર
	    thr->ધકેલો_real(l);
      પાછા સાચું;
}

તાર્કિક of_MOD(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valb = thr->કાઢો_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      પ્રતિજ્ઞા(vala.size()==valb.size());
      unsigned wid = vala.size();

      જો(wid <= 8*કદ(unsigned long long)) {
	    unsigned long long lv = 0, rv = 0;

	    માટે (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		  unsigned long long lb = vala.value(idx);
		  unsigned long long rb = valb.value(idx);

		  જો ((lb | rb) & 2)
			જાઓ x_out;

		  lv |= (unsigned long long) lb << idx;
		  rv |= (unsigned long long) rb << idx;
	    }

	    જો (rv == 0)
		  જાઓ x_out;

	    lv %= rv;

	    માટે (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		  vala.set_bit(idx, (lv&1)?BIT4_1 : BIT4_0);
		  lv >>= 1;
	    }

	    પાછા સાચું;

      } નહીંતર {
	    do_verylong_mod(vala, valb, ખોટું, ખોટું);
	    પાછા સાચું;
      }

 x_out:
      vala = vvp_vector4_t(wid, BIT4_X);
      પાછા સાચું;
}

/*
 * %mod/s
 */
તાર્કિક of_MOD_S(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valb = thr->કાઢો_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      પ્રતિજ્ઞા(vala.size()==valb.size());
      unsigned wid = vala.size();

	/* Handle the case that we can fit the bits into a long-long
	   variable. We cause use native % to do the work. */
      જો(wid <= 8*કદ(long long)) {
	    long long lv = 0, rv = 0;

	    માટે (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		  long long lb = vala.value(idx);
		  long long rb = valb.value(idx);

		  જો ((lb | rb) & 2)
			જાઓ x_out;

		  lv |= (long long) lb << idx;
		  rv |= (long long) rb << idx;
	    }

	    જો (rv == 0)
		  જાઓ x_out;

	    જો ((lv == LLONG_MIN) && (rv == -1))
		  જાઓ zero_out;

	      /* Sign extend the signed operands when needed. */
	    જો (wid < 8*કદ(long long)) {
		  જો (lv & (1LL << (wid-1)))
			lv |= -1ULL << wid;
		  જો (rv & (1LL << (wid-1)))
			rv |= -1ULL << wid;
	    }

	    lv %= rv;

	    માટે (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		  vala.set_bit(idx, (lv&1)? BIT4_1 : BIT4_0);
		  lv >>= 1;
	    }

	      // vala is the top of the stack, edited in place, so we
	      // do not need to push the result.

	    પાછા સાચું;

      } નહીંતર {

	    તાર્કિક ડાબું_is_neg  = vala.value(vala.size()-1) == BIT4_1;
	    તાર્કિક જમણું_is_neg = valb.value(valb.size()-1) == BIT4_1;
	    do_verylong_mod(vala, valb, ડાબું_is_neg, જમણું_is_neg);
	    પાછા સાચું;
      }

 x_out:
      vala = vvp_vector4_t(wid, BIT4_X);
      પાછા સાચું;
 zero_out:
      vala = vvp_vector4_t(wid, BIT4_0);
      પાછા સાચું;
}

/*
 * %mod/wr
 */
તાર્કિક of_MOD_WR(vthread_t thr, vvp_code_t)
{
      દ્વિગુણ r = thr->કાઢો_real();
      દ્વિગુણ l = thr->કાઢો_real();
      thr->ધકેલો_real(fmod(l,r));

      પાછા સાચું;
}

/*
 * %neg/wr
 */
તાર્કિક of_NEG_WR(vthread_t thr, vvp_code_t)
{
      thr->poke_real(0, -thr->peek_real(0));
      પાછા સાચું;
}

/*
 * %pad/s <wid>
 */
તાર્કિક of_PAD_S(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->સંખ્યા;

      vvp_vector4_t&val = thr->peek_vec4();
      unsigned old_size = val.size();

	// Sign-extend.
      જો (old_size < wid)
	    val.resize(wid, val.value(old_size-1));
      નહીંતર
	    val.resize(wid);

      પાછા સાચું;
}

/*
 * %pad/u <wid>
 */
તાર્કિક of_PAD_U(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->સંખ્યા;

      vvp_vector4_t&val = thr->peek_vec4();
      val.resize(wid, BIT4_0);

      પાછા સાચું;
}

/*
 * %part/s <wid>
 * %part/u <wid>
 * Two values are popped from the stack. First, pop the canonical
 * index of the part select, and second is the value to be
 * selected. The result is pushed back to the stack.
 */
સ્થિર તાર્કિક of_PART_base(vthread_t thr, vvp_code_t cp, તાર્કિક signed_flag)
{
      unsigned wid = cp->સંખ્યા;

      vvp_vector4_t base4 = thr->કાઢો_vec4();
      vvp_vector4_t&value = thr->peek_vec4();

      vvp_vector4_t res (wid, BIT4_X);

	// NOTE: This is treating the vector as signed. Is that correct?
      int32_t base;
      તાર્કિક કિંમત_ok = vector4_to_કિંમત(base4, base, signed_flag);
      જો (! કિંમત_ok) {
	    value = res;
	    પાછા સાચું;
      }

      જો (base >= (int32_t)value.size()) {
	    value = res;
	    પાછા સાચું;
      }

      જો ((base+(પૂર્ણાંક)wid) <= 0) {
	    value = res;
	    પાછા સાચું;
      }

      long vbase = 0;
      જો (base < 0) {
	    vbase = -base;
	    wid -= vbase;
	    base = 0;
      }

      જો ((base+wid) > value.size()) {
	    wid = value.size() - base;
      }

      res .set_vec(vbase, value.subvalue(base, wid));
      value = res;

      પાછા સાચું;
}

તાર્કિક of_PART_S(vthread_t thr, vvp_code_t cp)
{
      પાછા of_PART_base(thr, cp, સાચું);
}

તાર્કિક of_PART_U(vthread_t thr, vvp_code_t cp)
{
      પાછા of_PART_base(thr, cp, ખોટું);
}

/*
 * %parti/s <wid>, <basei>, <base_wid>
 * %parti/u <wid>, <basei>, <base_wid>
 *
 * Pop the value to be selected. The result is pushed back to the stack.
 */
સ્થિર તાર્કિક of_PARTI_base(vthread_t thr, vvp_code_t cp, તાર્કિક signed_flag)
{
      unsigned wid = cp->સંખ્યા;
      uint32_t base = cp->bit_idx[0];
      uint32_t bwid = cp->bit_idx[1];

      vvp_vector4_t&value = thr->peek_vec4();

      vvp_vector4_t res (wid, BIT4_X);

	// NOTE: This is treating the vector as signed. Is that correct?
      int32_t use_base = base;
      જો (signed_flag && bwid < 32 && (base&(1<<(bwid-1)))) {
	    use_base |= -1UL << bwid;
      }

      જો (use_base >= (int32_t)value.size()) {
	    value = res;
	    પાછા સાચું;
      }

      જો ((use_base+(int32_t)wid) <= 0) {
	    value = res;
	    પાછા સાચું;
      }

      long vbase = 0;
      જો (use_base < 0) {
	    vbase = -use_base;
	    wid -= vbase;
	    use_base = 0;
      }

      જો ((use_base+wid) > value.size()) {
	    wid = value.size() - use_base;
      }

      res .set_vec(vbase, value.subvalue(use_base, wid));
      value = res;

      પાછા સાચું;
}

તાર્કિક of_PARTI_S(vthread_t thr, vvp_code_t cp)
{
      પાછા of_PARTI_base(thr, cp, સાચું);
}

તાર્કિક of_PARTI_U(vthread_t thr, vvp_code_t cp)
{
      પાછા of_PARTI_base(thr, cp, ખોટું);
}

/*
 * %mul
 */
તાર્કિક of_MUL(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t r = thr->કાઢો_vec4();
	// Rather then pop l, use it directly from the stack. When we
	// assign to 'l', that will edit the top of the stack, which
	// replaces a pop and a pull.
      vvp_vector4_t&l = thr->peek_vec4();

      l.mul(r);
      પાછા સાચું;
}

/*
 * %muli <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
તાર્કિક of_MULI(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->સંખ્યા;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_immediate_rval (cp, r);

      l.mul(r);
      પાછા સાચું;
}

તાર્કિક of_MUL_WR(vthread_t thr, vvp_code_t)
{
      દ્વિગુણ r = thr->કાઢો_real();
      દ્વિગુણ l = thr->કાઢો_real();
      thr->ધકેલો_real(l * r);

      પાછા સાચું;
}

તાર્કિક of_NAND(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valr = thr->કાઢો_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      પ્રતિજ્ઞા(vall.size() == valr.size());

      vall &= valr;
      vall.invert();

      પાછા સાચું;
}

/*
 * %new/cobj <vpi_object>
 * This creates a new cobject (SystemVerilog class object) and pushes
 * it to the stack. The <vpi-object> is a __vpiHandle that is a
 * vpiClassDefn object that defines the item to be created.
 */
તાર્કિક of_NEW_COBJ(vthread_t thr, vvp_code_t cp)
{
      અચલ class_type*defn = dynamic_cast<અચલ class_type*> (cp->handle);
      પ્રતિજ્ઞા(defn);

      vvp_object_t tmp (નવું vvp_cobject(defn));
      thr->ધકેલો_object(tmp);
      પાછા સાચું;
}

તાર્કિક of_NEW_DARRAY(vthread_t thr, vvp_code_t cp)
{
      અચલ અક્ષર*text = cp->text;
      size_t size = thr->શબ્દો[cp->bit_idx[0]].w_int;
      unsigned word_wid;
      size_t n;

      vvp_object_t obj;
      જો (strcmp(text,"b8") == 0) {
	    obj = નવું vvp_darray_atom<uint8_t>(size);
      } નહીંતર જો (strcmp(text,"b16") == 0) {
	    obj = નવું vvp_darray_atom<uint16_t>(size);
      } નહીંતર જો (strcmp(text,"b32") == 0) {
	    obj = નવું vvp_darray_atom<uint32_t>(size);
      } નહીંતર જો (strcmp(text,"b64") == 0) {
	    obj = નવું vvp_darray_atom<uint64_t>(size);
      } નહીંતર જો (strcmp(text,"sb8") == 0) {
	    obj = નવું vvp_darray_atom<int8_t>(size);
      } નહીંતર જો (strcmp(text,"sb16") == 0) {
	    obj = નવું vvp_darray_atom<int16_t>(size);
      } નહીંતર જો (strcmp(text,"sb32") == 0) {
	    obj = નવું vvp_darray_atom<int32_t>(size);
      } નહીંતર જો (strcmp(text,"sb64") == 0) {
	    obj = નવું vvp_darray_atom<int64_t>(size);
      } નહીંતર જો ((1 == sscanf(text, "b%u%zn", &word_wid, &n)) &&
                 (n == strlen(text))) {
	    obj = નવું vvp_darray_vec2(size, word_wid);
      } નહીંતર જો ((1 == sscanf(text, "sb%u%zn", &word_wid, &n)) &&
                 (n == strlen(text))) {
	    obj = નવું vvp_darray_vec2(size, word_wid);
      } નહીંતર જો ((1 == sscanf(text, "v%u%zn", &word_wid, &n)) &&
                 (n == strlen(text))) {
	    obj = નવું vvp_darray_vec4(size, word_wid);
      } નહીંતર જો ((1 == sscanf(text, "sv%u%zn", &word_wid, &n)) &&
                 (n == strlen(text))) {
	    obj = નવું vvp_darray_vec4(size, word_wid);
      } નહીંતર જો (strcmp(text,"r") == 0) {
	    obj = નવું vvp_darray_real(size);
      } નહીંતર જો (strcmp(text,"S") == 0) {
	    obj = નવું vvp_darray_string(size);
      } નહીંતર {
	    cerr << get_fileline()
	         << "Internal error: Unsupported dynamic array type: "
	         << text << "." << endl;
	    પ્રતિજ્ઞા(0);
      }

      thr->ધકેલો_object(obj);

      પાછા સાચું;
}

તાર્કિક of_NOOP(vthread_t, vvp_code_t)
{
      પાછા સાચું;
}

/*
 * %nor/r
 */
તાર્કિક of_NORR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_or());

      પાછા સાચું;
}

/*
 * Push a null to the object stack.
 */
તાર્કિક of_NULL(vthread_t thr, vvp_code_t)
{
      vvp_object_t tmp;
      thr->ધકેલો_object(tmp);
      પાછા સાચું;
}

/*
 * %and/r
 */
તાર્કિક of_ANDR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_and());

      પાછા સાચું;
}

/*
 * %nand/r
 */
તાર્કિક of_NANDR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_and());

      પાછા સાચું;
}

/*
 * %or/r
 */
તાર્કિક of_ORR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_or());

      પાછા સાચું;
}

/*
 * %xor/r
 */
તાર્કિક of_XORR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_xor());

      પાછા સાચું;
}

/*
 * %xnor/r
 */
તાર્કિક of_XNORR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_xor());

      પાછા સાચું;
}

/*
 * %or
 */
તાર્કિક of_OR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valb = thr->કાઢો_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();
      vala |= valb;
      પાછા સાચું;
}

/*
 * %nor
 */
તાર્કિક of_NOR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valr = thr->કાઢો_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      પ્રતિજ્ઞા(vall.size() == valr.size());

      vall |= valr;
      vall.invert();

      પાછા સાચું;
}

/*
 * %pop/obj <num>, <skip>
 */
તાર્કિક of_કાઢો_OBJ(vthread_t thr, vvp_code_t cp)
{
      unsigned cnt = cp->bit_idx[0];
      unsigned skip = cp->bit_idx[1];

      thr->કાઢો_object(cnt, skip);
      પાછા સાચું;
}

/*
 * %pop/real <number>
 */
તાર્કિક of_કાઢો_REAL(vthread_t thr, vvp_code_t cp)
{
      unsigned cnt = cp->સંખ્યા;
      thr->કાઢો_real(cnt);
      પાછા સાચું;
}

/*
 *  %pop/str <number>
 */
તાર્કિક of_કાઢો_STR(vthread_t thr, vvp_code_t cp)
{
      unsigned cnt = cp->સંખ્યા;
      thr->કાઢો_str(cnt);
      પાછા સાચું;
}

/*
 *  %pop/vec4 <number>
 */
તાર્કિક of_કાઢો_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned cnt = cp->સંખ્યા;
      thr->કાઢો_vec4(cnt);
      પાછા સાચું;
}

/*
 * %pow
 * %pow/s
 */
સ્થિર તાર્કિક of_POW_base(vthread_t thr, તાર્કિક signed_flag)
{
      vvp_vector4_t valb = thr->કાઢો_vec4();
      vvp_vector4_t vala = thr->કાઢો_vec4();

      unsigned wid = vala.size();

      vvp_vector2_t xv2 = vvp_vector2_t(vala, સાચું);
      vvp_vector2_t yv2 = vvp_vector2_t(valb, સાચું);


        /* If we have an X or Z in the arguments return X. */
      જો (xv2.is_NaN() || yv2.is_NaN()) {
	    vvp_vector4_t tmp (wid, BIT4_X);
	    thr->ધકેલો_vec4(tmp);
	    પાછા સાચું;
      }

	// Is the exponent negative? If so, table 5-6 in IEEE1364-2005
	// defines what value is returned.
      જો (signed_flag && yv2.value(yv2.size()-1)) {
	    પૂર્ણાંક a_val;
	    vvp_bit4_t pad = BIT4_0, lsb = BIT4_0;
	    જો (vector2_to_કિંમત(xv2, a_val, સાચું)) {
		  જો (a_val == 0) {
			pad = BIT4_X; lsb = BIT4_X;
		  }
		  જો (a_val == 1) {
			pad = BIT4_0; lsb = BIT4_1;
		  }
		  જો (a_val == -1) {
			જો (yv2.value(0)) {
			      pad = BIT4_1; lsb = BIT4_1;
			} નહીંતર {
			      pad = BIT4_0; lsb = BIT4_1;
			}
		  }
	    }
	    vvp_vector4_t tmp (wid, pad);
	    tmp.set_bit(0, lsb);
	    thr->ધકેલો_vec4(tmp);
	    પાછા સાચું;
      }

      vvp_vector2_t result = pow(xv2, yv2);

        /* Copy only what we need of the result. If the result is too
	   small, zero-pad it. */
      માટે (unsigned jdx = 0;  jdx < wid;  jdx += 1) {
	    જો (jdx >= result.size())
		  vala.set_bit(jdx, BIT4_0);
	    નહીંતર
		  vala.set_bit(jdx, result.value(jdx) ? BIT4_1 : BIT4_0);
      }
      thr->ધકેલો_vec4(vala);

      પાછા સાચું;
}

તાર્કિક of_POW(vthread_t thr, vvp_code_t)
{
      પાછા of_POW_base(thr, ખોટું);
}

તાર્કિક of_POW_S(vthread_t thr, vvp_code_t)
{
      પાછા of_POW_base(thr, સાચું);
}

તાર્કિક of_POW_WR(vthread_t thr, vvp_code_t)
{
      દ્વિગુણ r = thr->કાઢો_real();
      દ્વિગુણ l = thr->કાઢો_real();
      thr->ધકેલો_real(pow(l,r));

      પાછા સાચું;
}

/*
 * %prop/obj <pid>, <idx>
 *
 * Load an object value from the cobject and push it onto the object stack.
 */
તાર્કિક of_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      unsigned pid = cp->સંખ્યા;
      unsigned idx = cp->bit_idx[0];

      જો (idx != 0) {
	    પ્રતિજ્ઞા(idx < vthread_s::શબ્દો_ગણતરી);
	    idx = thr->શબ્દો[idx].w_uint;
      }

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();

      vvp_object_t val;
      cobj->get_object(pid, val, idx);

      thr->ધકેલો_object(val);

      પાછા સાચું;
}

સ્થિર શૂન્ય get_from_obj(unsigned pid, vvp_cobject*cobj, દ્વિગુણ&val)
{
      val = cobj->get_real(pid);
}

સ્થિર શૂન્ય get_from_obj(unsigned pid, vvp_cobject*cobj, string&val)
{
      val = cobj->get_string(pid);
}

સ્થિર શૂન્ય get_from_obj(unsigned pid, vvp_cobject*cobj, vvp_vector4_t&val)
{
      cobj->get_vec4(pid, val);
}

template <typename ELEM>
સ્થિર તાર્કિક prop(vthread_t thr, vvp_code_t cp)
{
      unsigned pid = cp->સંખ્યા;

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      પ્રતિજ્ઞા(cobj);

      ELEM val;
      get_from_obj(pid, cobj, val);
      vthread_ધકેલો(thr, val);

      પાછા સાચું;
}

/*
 * %prop/r <pid>
 *
 * Load a real value from the cobject and push it onto the real value
 * stack.
 */
તાર્કિક of_PROP_R(vthread_t thr, vvp_code_t cp)
{
      પાછા prop<દ્વિગુણ>(thr, cp);
}

/*
 * %prop/str <pid>
 *
 * Load a string value from the cobject and push it onto the real value
 * stack.
 */
તાર્કિક of_PROP_STR(vthread_t thr, vvp_code_t cp)
{
      પાછા prop<string>(thr, cp);
}

/*
 * %prop/v <pid>
 *
 * Load a property <pid> from the cobject on the top of the stack into
 * the vector space at <base>.
 */
તાર્કિક of_PROP_V(vthread_t thr, vvp_code_t cp)
{
      પાછા prop<vvp_vector4_t>(thr, cp);
}

તાર્કિક of_PUSHI_REAL(vthread_t thr, vvp_code_t cp)
{
      દ્વિગુણ mant = cp->bit_idx[0];
      uint32_t imant = cp->bit_idx[0];
      પૂર્ણાંક exp = cp->bit_idx[1];

	// Detect +infinity
      જો (exp==0x3fff && imant==0) {
	    thr->ધકેલો_real(INFINITY);
	    પાછા સાચું;
      }
	// Detect -infinity
      જો (exp==0x7fff && imant==0) {
	    thr->ધકેલો_real(-INFINITY);
	    પાછા સાચું;
      }
	// Detect NaN
      જો (exp==0x3fff) {
	    thr->ધકેલો_real(nan(""));
	    પાછા સાચું;
      }

      દ્વિગુણ sign = (exp & 0x4000)? -1.0 : 1.0;

      exp &= 0x1fff;

      mant = sign * ldexp(mant, exp - 0x1000);
      thr->ધકેલો_real(mant);
      પાછા સાચું;
}

તાર્કિક of_PUSHI_STR(vthread_t thr, vvp_code_t cp)
{
      અચલ અક્ષર*text = cp->text;
      thr->ધકેલો_str(filter_string(text));
      પાછા સાચું;
}

/*
 * %pushi/vec4 <vala>, <valb>, <wid>
 */
તાર્કિક of_PUSHI_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned wid  = cp->સંખ્યા;

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t val (wid, BIT4_0);
      get_immediate_rval (cp, val);

      thr->ધકેલો_vec4(val);

      પાછા સાચું;
}

/*
 * %pushv/str
 *   Pops a vec4 value, and pushes a string.
 */
તાર્કિક of_PUSHV_STR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t vec = thr->કાઢો_vec4();

      size_t slen = (vec.size() + 7)/8;
      vector<અક્ષર>buf;
      buf.reserve(slen);

      માટે (size_t idx = 0 ; idx < vec.size() ; idx += 8) {
	    અક્ષર tmp = 0;
	    size_t trans = 8;
	    જો (idx+trans > vec.size())
		  trans = vec.size() - idx;

	    માટે (size_t bdx = 0 ; bdx < trans ; bdx += 1) {
		  જો (vec.value(idx+bdx) == BIT4_1)
			tmp |= 1 << bdx;
	    }

	    જો (tmp != 0)
		  buf.push_back(tmp);
      }

      string val;
      માટે (vector<અક્ષર>::reverse_iterator cur = buf.rbegin()
		 ; cur != buf.rend() ; ++cur) {
	    val.push_back(*cur);
      }

      thr->ધકેલો_str(val);

      પાછા સાચું;
}

/*
 * %putc/str/vec4 <var>, <mux>
 */
તાર્કિક of_PUTC_STR_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned muxr = cp->bit_idx[0];
      int32_t mux = muxr? thr->શબ્દો[muxr].w_int : 0;

      vvp_vector4_t val = thr->કાઢો_vec4();
      પ્રતિજ્ઞા(val.size() == 8);

      જો (mux < 0)
	    પાછા સાચું;

	/* Get the existing value of the string. If we find that the
	   index is too big for the string, then give up. */
      vvp_net_t*net = cp->net;
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (net->fun);
      પ્રતિજ્ઞા(fun);

      string tmp = fun->get_string();
      જો (tmp.size() <= (size_t)mux)
	    પાછા સાચું;

      અક્ષર val_str = 0;
      માટે (size_t idx = 0 ; idx < 8 ; idx += 1) {
	    જો (val.value(idx)==BIT4_1)
		  val_str |= 1<<idx;
      }

	// It is a quirk of the Verilog standard that putc(..., 'h00)
	// has no effect. Test for that case here.
      જો (val_str == 0)
	    પાછા સાચું;

      tmp[mux] = val_str;

      vvp_send_string(vvp_net_ptr_t(cp->net, 0), tmp, thr->wt_context);
      પાછા સાચું;
}

template <typename ELEM, વર્ગ QTYPE>
સ્થિર તાર્કિક qinsert(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
      int64_t idx = thr->શબ્દો[3].w_int;
      ELEM value;
      vvp_net_t*net = cp->net;
      કાઢો_કિંમત(thr, value, wid); // Pop the value to store.

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      પ્રતિજ્ઞા(queue);
      જો (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at a negative "
	         << get_queue_type(value)
	         << " index (" << idx << "). ";
	    print_queue_કિંમત(value);
	    cerr << " was not added." << endl;
      } નહીંતર જો (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at an undefined "
	         << get_queue_type(value) << " index. ";
	    print_queue_કિંમત(value);
	    cerr << " was not added." << endl;
      } નહીંતર {
	    unsigned max_size = thr->શબ્દો[cp->bit_idx[0]].w_int;
	    queue->insert(idx, value, max_size);
      }
      પાછા સાચું;
}

/*
 * %qinsert/real <var-label>
 */
તાર્કિક of_QINSERT_REAL(vthread_t thr, vvp_code_t cp)
{
      પાછા qinsert<દ્વિગુણ, vvp_queue_real>(thr, cp);
}

/*
 * %qinsert/str <var-label>
 */
તાર્કિક of_QINSERT_STR(vthread_t thr, vvp_code_t cp)
{
      પાછા qinsert<string, vvp_queue_string>(thr, cp);
}

/*
 * %qinsert/v <var-label>
 */
તાર્કિક of_QINSERT_V(vthread_t thr, vvp_code_t cp)
{
      પાછા qinsert<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

/*
 * Helper functions used in the queue pop templates
 */
અંતઃરેખ શૂન્ય ધકેલો_કિંમત(vthread_t thr, દ્વિગુણ value, unsigned)
{
      thr->ધકેલો_real(value);
}

અંતઃરેખ શૂન્ય ધકેલો_કિંમત(vthread_t thr, અચલ string&value, unsigned)
{
      thr->ધકેલો_str(value);
}

અંતઃરેખ શૂન્ય ધકેલો_કિંમત(vthread_t thr, અચલ vvp_vector4_t&value, unsigned wid)
{
      પ્રતિજ્ઞા(wid == value.size());
      thr->ધકેલો_vec4(value);
}

template <typename ELEM, વર્ગ QTYPE>
સ્થિર તાર્કિક q_કાઢો(vthread_t thr, vvp_code_t cp,
                  શૂન્ય (*get_val_func)(vvp_queue*, ELEM&),
                  અચલ અક્ષર*loc, unsigned wid)
{
      vvp_net_t*net = cp->net;

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      પ્રતિજ્ઞા(queue);

      size_t size = queue->get_size();

      ELEM value;
      જો (size) {
	    get_val_func(queue, value);
      } નહીંતર {
	    dq_default(value, wid);
	    cerr << thr->get_fileline()
	         << "Warning: pop_" << loc << "() on empty "
	         << get_queue_type(value) << "." << endl;
      }

      ધકેલો_કિંમત(thr, value, wid);
      પાછા સાચું;
}

template <typename ELEM>
સ્થિર શૂન્ય get_back_કિંમત(vvp_queue*queue, ELEM&value)
{
      queue->get_word(queue->get_size()-1, value);
      queue->pop_back();
}

template <typename ELEM, વર્ગ QTYPE>
સ્થિર તાર્કિક qpop_b(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
      પાછા q_કાઢો<ELEM, QTYPE>(thr, cp, get_back_કિંમત<ELEM>, "back", wid);
}

/*
 * %qpop/b/real <var-label>
 */
તાર્કિક of_QPOP_B_REAL(vthread_t thr, vvp_code_t cp)
{
      પાછા qpop_b<દ્વિગુણ, vvp_queue_real>(thr, cp);
}

/*
 * %qpop/b/str <var-label>
 */
તાર્કિક of_QPOP_B_STR(vthread_t thr, vvp_code_t cp)
{
      પાછા qpop_b<string, vvp_queue_string>(thr, cp);
}

/*
 * %qpop/b/v <var-label>
 */
તાર્કિક of_QPOP_B_V(vthread_t thr, vvp_code_t cp)
{
      પાછા qpop_b<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[0]);
}

template <typename ELEM>
સ્થિર શૂન્ય get_front_કિંમત(vvp_queue*queue, ELEM&value)
{
      queue->get_word(0, value);
      queue->pop_front();
}

template <typename ELEM, વર્ગ QTYPE>
સ્થિર તાર્કિક qpop_f(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
      પાછા q_કાઢો<ELEM, QTYPE>(thr, cp, get_front_કિંમત<ELEM>, "front", wid);
}


/*
 * %qpop/f/real <var-label>
 */
તાર્કિક of_QPOP_F_REAL(vthread_t thr, vvp_code_t cp)
{
      પાછા qpop_f<દ્વિગુણ, vvp_queue_real>(thr, cp);
}

/*
 * %qpop/f/str <var-label>
 */
તાર્કિક of_QPOP_F_STR(vthread_t thr, vvp_code_t cp)
{
      પાછા qpop_f<string, vvp_queue_string>(thr, cp);
}

/*
 * %qpop/f/v <var-label>
 */
તાર્કિક of_QPOP_F_V(vthread_t thr, vvp_code_t cp)
{
      પાછા qpop_f<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[0]);
}

/*
 * These implement the %release/net and %release/reg instructions. The
 * %release/net instruction applies to a net kind of functor by
 * sending the release/net command to the command port. (See vvp_net.h
 * for details.) The %release/reg instruction is the same, but sends
 * the release/reg command instead. These are very similar to the
 * %deassign instruction.
 */
સ્થિર તાર્કિક do_release_vec(vvp_code_t cp, તાર્કિક net_flag)
{
      vvp_net_t*net = cp->net;
      unsigned base  = cp->bit_idx[0];
      unsigned width = cp->bit_idx[1];

      પ્રતિજ્ઞા(net->fil);

      જો (base >= net->fil->filter_size()) પાછા સાચું;
      જો (base+width > net->fil->filter_size())
	    width = net->fil->filter_size() - base;

      તાર્કિક full_sig = base == 0 && width == net->fil->filter_size();

	// XXXX Can't really do this if this is a partial release?
      net->fil->force_unlink();

	/* Do we release all or part of the net? */
      vvp_net_ptr_t ptr (net, 0);
      જો (full_sig) {
	    net->fil->release(ptr, net_flag);
      } નહીંતર {
	    net->fil->release_pv(ptr, base, width, net_flag);
      }
      net->fun->force_flag(ખોટું);

      પાછા સાચું;
}

તાર્કિક of_RELEASE_NET(vthread_t, vvp_code_t cp)
{
      પાછા do_release_vec(cp, સાચું);
}


તાર્કિક of_RELEASE_REG(vthread_t, vvp_code_t cp)
{
      પાછા do_release_vec(cp, ખોટું);
}

/* The type is 1 for registers and 0 for everything else. */
તાર્કિક of_RELEASE_WR(vthread_t, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      unsigned type  = cp->bit_idx[0];

      પ્રતિજ્ઞા(net->fil);
      net->fil->force_unlink();

	// Send a command to this signal to unforce itself.
      vvp_net_ptr_t ptr (net, 0);
      net->fil->release(ptr, type==0);
      પાછા સાચું;
}

તાર્કિક of_REPLICATE(vthread_t thr, vvp_code_t cp)
{
      પૂર્ણાંક rept = cp->સંખ્યા;
      vvp_vector4_t val = thr->કાઢો_vec4();
      vvp_vector4_t res (val.size() * rept, BIT4_X);

      માટે (પૂર્ણાંક idx = 0 ; idx < rept ; idx += 1) {
	    res.set_vec(idx * val.size(), val);
      }

      thr->ધકેલો_vec4(res);

      પાછા સાચું;
}

સ્થિર શૂન્ય poke_val(vthread_t fun_thr, unsigned depth, દ્વિગુણ val)
{
      fun_thr->parent->poke_real(depth, val);
}

સ્થિર શૂન્ય poke_val(vthread_t fun_thr, unsigned depth, અચલ string&val)
{
      fun_thr->parent->poke_str(depth, val);
}

સ્થિર size_t get_max(vthread_t fun_thr, દ્વિગુણ&)
{
      પાછા fun_thr->args_real.size();
}

સ્થિર size_t get_max(vthread_t fun_thr, string&)
{
      પાછા fun_thr->args_str.size();
}

સ્થિર size_t get_max(vthread_t fun_thr, vvp_vector4_t&)
{
      પાછા fun_thr->args_vec4.size();
}

સ્થિર unsigned get_depth(vthread_t fun_thr, size_t index, દ્વિગુણ&)
{
      પાછા fun_thr->args_real[index];
}

સ્થિર unsigned get_depth(vthread_t fun_thr, size_t index, string&)
{
      પાછા fun_thr->args_str[index];
}

સ્થિર unsigned get_depth(vthread_t fun_thr, size_t index, vvp_vector4_t&)
{
      પાછા fun_thr->args_vec4[index];
}

સ્થિર vthread_t get_func(vthread_t thr)
{
      vthread_t fun_thr = thr;

      જ્યારે (fun_thr->parent_scope->get_type_code() != vpiFunction) {
	    પ્રતિજ્ઞા(fun_thr->parent);
	    fun_thr = fun_thr->parent;
      }

      પાછા fun_thr;
}

template <typename ELEM>
સ્થિર તાર્કિક ret(vthread_t thr, vvp_code_t cp)
{
      size_t index = cp->સંખ્યા;
      ELEM val;
      કાઢો_કિંમત(thr, val, 0);

      vthread_t fun_thr = get_func(thr);
      પ્રતિજ્ઞા(index < get_max(fun_thr, val));

      unsigned depth = get_depth(fun_thr, index, val);
	// Use the depth to put the value into the stack of
	// the parent thread.
      poke_val(fun_thr, depth, val);
      પાછા સાચું;
}

/*
 * %ret/real <index>
 */
તાર્કિક of_RET_REAL(vthread_t thr, vvp_code_t cp)
{
      પાછા ret<દ્વિગુણ>(thr, cp);
}

/*
 * %ret/str <index>
 */
તાર્કિક of_RET_STR(vthread_t thr, vvp_code_t cp)
{
      પાછા ret<string>(thr, cp);
}

/*
 * %ret/vec4 <index>, <offset>, <wid>
 */
તાર્કિક of_RET_VEC4(vthread_t thr, vvp_code_t cp)
{
      size_t index = cp->સંખ્યા;
      unsigned off_index = cp->bit_idx[0];
      unsigned પૂર્ણાંક wid = cp->bit_idx[1];
      vvp_vector4_t&val = thr->peek_vec4();

      vthread_t fun_thr = get_func(thr);
      પ્રતિજ્ઞા(index < get_max(fun_thr, val));
      પ્રતિજ્ઞા(val.size() == wid);
      unsigned depth = get_depth(fun_thr, index, val);

      int64_t off = off_index ? thr->શબ્દો[off_index].w_int : 0;
      unsigned પૂર્ણાંક sig_કિંમત_size = fun_thr->parent->peek_vec4(depth).size();

      જો (off_index!=0 && thr->flags[4] == BIT4_1) {
	    thr->કાઢો_vec4(1);
	    પાછા સાચું;
      }

      જો (!resize_rval_vec(val, off, sig_કિંમત_size)) {
	    thr->કાઢો_vec4(1);
	    પાછા સાચું;
      }

      જો (off == 0 && val.size() == sig_કિંમત_size) {
	    fun_thr->parent->poke_vec4(depth, val);
      } નહીંતર {
	    vvp_vector4_t tmp_dst = fun_thr->parent->peek_vec4(depth);
	    tmp_dst.set_vec(off, val);
	    fun_thr->parent->poke_vec4(depth, tmp_dst);
      }

      thr->કાઢો_vec4(1);
      પાછા સાચું;
}

સ્થિર શૂન્ય ધકેલો_from_parent(vthread_t thr, vthread_t fun_thr, unsigned depth, દ્વિગુણ&)
{
      thr->ધકેલો_real(fun_thr->parent->peek_real(depth));
}

સ્થિર શૂન્ય ધકેલો_from_parent(vthread_t thr, vthread_t fun_thr, unsigned depth, string&)
{
      thr->ધકેલો_str(fun_thr->parent->peek_str(depth));
}

સ્થિર શૂન્ય ધકેલો_from_parent(vthread_t thr, vthread_t fun_thr, unsigned depth, vvp_vector4_t&)
{
      thr->ધકેલો_vec4(fun_thr->parent->peek_vec4(depth));
}

template <typename ELEM>
સ્થિર તાર્કિક retload(vthread_t thr, vvp_code_t cp)
{
      size_t index = cp->સંખ્યા;
      ELEM type;

      vthread_t fun_thr = get_func(thr);
      પ્રતિજ્ઞા(index < get_max(fun_thr, type));

      unsigned depth = get_depth(fun_thr, index, type);
	// Use the depth to extract the values from the stack
	// of the parent thread.
      ધકેલો_from_parent(thr, fun_thr, depth, type);
      પાછા સાચું;
}

/*
 * %retload/real <index>
 */
તાર્કિક of_RETLOAD_REAL(vthread_t thr, vvp_code_t cp)
{
      પાછા retload<દ્વિગુણ>(thr, cp);
}

/*
 * %retload/str <index>
 */
તાર્કિક of_RETLOAD_STR(vthread_t thr, vvp_code_t cp)
{
      પાછા retload<string>(thr, cp);
}

/*
 * %retload/vec4 <index>
 */
તાર્કિક of_RETLOAD_VEC4(vthread_t thr, vvp_code_t cp)
{
      પાછા retload<vvp_vector4_t>(thr, cp);
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
તાર્કિક of_SCOPY(vthread_t thr, vvp_code_t)
{
      vvp_object_t tmp;
      thr->કાઢો_object(tmp);

      vvp_object_t&dest = thr->peek_object();
        // If it is null there is nothing to copy
      જો (!tmp.test_nil())
	    dest.shallow_copy(tmp);

      પાછા સાચું;
}

સ્થિર શૂન્ય thread_peek(vthread_t thr, દ્વિગુણ&value)
{
      value = thr->peek_real(0);
}

સ્થિર શૂન્ય thread_peek(vthread_t thr, string&value)
{
      value = thr->peek_str(0);
}

સ્થિર શૂન્ય thread_peek(vthread_t thr, vvp_vector4_t&value)
{
      value = thr->peek_vec4(0);
}

template <typename ELEM>
સ્થિર તાર્કિક set_dar_obj(vthread_t thr, vvp_code_t cp)
{
      unsigned adr = thr->શબ્દો[cp->સંખ્યા].w_int;

      ELEM value;
      thread_peek(thr, value);

      vvp_object_t&ટોચ = thr->peek_object();
      vvp_darray*darray = ટોચ.peek<vvp_darray>();
      પ્રતિજ્ઞા(darray);

      darray->set_word(adr, value);
      પાછા સાચું;
}

/*
 * %set/dar/obj/real <index>
 */
તાર્કિક of_SET_DAR_OBJ_REAL(vthread_t thr, vvp_code_t cp)
{
      પાછા set_dar_obj<દ્વિગુણ>(thr, cp);
}

/*
 * %set/dar/obj/str <index>
 */
તાર્કિક of_SET_DAR_OBJ_STR(vthread_t thr, vvp_code_t cp)
{
      પાછા set_dar_obj<string>(thr, cp);
}

/*
 * %set/dar/obj/vec4 <index>
 */
તાર્કિક of_SET_DAR_OBJ_VEC4(vthread_t thr, vvp_code_t cp)
{
      પાછા set_dar_obj<vvp_vector4_t>(thr, cp);
}

/*
 * %shiftl <idx>
 *
 * Pop the operand, then push the result.
 */
તાર્કિક of_SHIFTL(vthread_t thr, vvp_code_t cp)
{
      પૂર્ણાંક use_index = cp->સંખ્યા;
      uint64_t shift = thr->શબ્દો[use_index].w_uint;

      vvp_vector4_t&val = thr->peek_vec4();
      unsigned wid  = val.size();

      જો (thr->flags[4] == BIT4_1) {
	      // The result is 'bx if the shift amount is undefined
	    val = vvp_vector4_t(wid, BIT4_X);

      } નહીંતર જો (thr->flags[4] == BIT4_X || shift >= wid) {
	      // Shift is so big that all value is shifted out. Write
	      // a constant 0 result.
	    val = vvp_vector4_t(wid, BIT4_0);

      } નહીંતર જો (shift > 0) {
	    vvp_vector4_t blk = val.subvalue(0, wid-shift);
	    vvp_vector4_t tmp (shift, BIT4_0);
	    val.set_vec(0, tmp);
	    val.set_vec(shift, blk);
      }

      પાછા સાચું;
}

/*
 * %shiftr <idx>
 * This is an unsigned right shift. The <idx> is a number that selects
 * the index register with the amount of the shift. This instruction
 * checks flag bit 4, which will be true if the shift is invalid.
 */
તાર્કિક of_SHIFTR(vthread_t thr, vvp_code_t cp)
{
      પૂર્ણાંક use_index = cp->સંખ્યા;
      uint64_t shift = thr->શબ્દો[use_index].w_uint;

      vvp_vector4_t val = thr->કાઢો_vec4();
      unsigned wid  = val.size();

      જો (thr->flags[4] == BIT4_1) {
	    val = vvp_vector4_t(wid, BIT4_X);

      } નહીંતર જો (thr->flags[4] == BIT4_X || shift > wid) {
	    val = vvp_vector4_t(wid, BIT4_0);

      } નહીંતર જો (shift > 0) {
	    vvp_vector4_t blk = val.subvalue(shift, wid-shift);
	    vvp_vector4_t tmp (shift, BIT4_0);
	    val.set_vec(0, blk);
	    val.set_vec(wid-shift, tmp);
      }

      thr->ધકેલો_vec4(val);
      પાછા સાચું;
}

/*
 *  %shiftr/s <wid>
 */
તાર્કિક of_SHIFTR_S(vthread_t thr, vvp_code_t cp)
{
      પૂર્ણાંક use_index = cp->સંખ્યા;
      uint64_t shift = thr->શબ્દો[use_index].w_uint;

      vvp_vector4_t val = thr->કાઢો_vec4();
      unsigned wid  = val.size();

      vvp_bit4_t sign_bit = val.value(val.size()-1);

      જો (thr->flags[4] == BIT4_1) {
	    val = vvp_vector4_t(wid, BIT4_X);

      } નહીંતર જો (thr->flags[4] == BIT4_X || shift > wid) {
	    val = vvp_vector4_t(wid, sign_bit);

      } નહીંતર જો (shift > 0) {
	    vvp_vector4_t blk = val.subvalue(shift, wid-shift);
	    vvp_vector4_t tmp (shift, sign_bit);
	    val.set_vec(0, blk);
	    val.set_vec(wid-shift, tmp);
      }

      thr->ધકેલો_vec4(val);
      પાછા સાચું;
}

/*
 * %split/vec4 <wid>
 *   Pop 1 value,
 *   Take <wid> bits from the lsb,
 *   Push the remaining msb,
 *   Push the lsb.
 */
તાર્કિક of_SPLIT_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned lsb_wid = cp->સંખ્યા;

      vvp_vector4_t&val = thr->peek_vec4();
      પ્રતિજ્ઞા(lsb_wid < val.size());

      vvp_vector4_t lsb = val.subvalue(0, lsb_wid);
      val = val.subvalue(lsb_wid, val.size()-lsb_wid);

      thr->ધકેલો_vec4(lsb);
      પાછા સાચું;
}

/*
 * The following are used to allow the darray templates to print correctly.
 */
અંતઃરેખ સ્થિર string get_darray_type(દ્વિગુણ&)
{
      પાછા "darray<real>";
}

અંતઃરેખ સ્થિર string get_darray_type(string&)
{
      પાછા "darray<string>";
}

અંતઃરેખ સ્થિર string get_darray_type(અચલ vvp_vector4_t&value)
{
      ostringstream buf;
      buf << "darray<vector[" << value.size() << "]>";
      string res = buf.str();
      પાછા res;
}

/*
 * The following are used to allow a common template to be written for
 * darray real/string/vec4 operations
 */
અંતઃરેખ સ્થિર શૂન્ય dar_કાઢો_કિંમત(vthread_t thr, દ્વિગુણ&value)
{
      value = thr->કાઢો_real();
}

અંતઃરેખ સ્થિર શૂન્ય dar_કાઢો_કિંમત(vthread_t thr, string&value)
{
      value = thr->કાઢો_str();
}

અંતઃરેખ સ્થિર શૂન્ય dar_કાઢો_કિંમત(vthread_t thr, vvp_vector4_t&value)
{
      value = thr->કાઢો_vec4();
}

template <typename ELEM>
સ્થિર તાર્કિક store_dar(vthread_t thr, vvp_code_t cp)
{
      int64_t adr = thr->શબ્દો[3].w_int;
      ELEM value;
	// FIXME: Can we get the size of the underlying array element
	//        and then use the normal pop_value?
      dar_કાઢો_કિંમત(thr, value);

      vvp_net_t*net = cp->net;
      પ્રતિજ્ઞા(net);

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      પ્રતિજ્ઞા(obj);

      vvp_darray*darray = obj->get_object().peek<vvp_darray>();

      જો (adr < 0)
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to a negative " << get_darray_type(value)
	         << " index (" << adr << ")." << endl;
      નહીંતર જો (thr->flags[4] != BIT4_0)
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to an undefined " << get_darray_type(value)
	         << " index." << endl;
      નહીંતર જો (darray)
	    darray->set_word(adr, value);
      નહીંતર
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to an undefined " << get_darray_type(value)
	         << "." << endl;

      પાછા સાચું;
}

/*
 * %store/dar/real <var>
 */
તાર્કિક of_STORE_DAR_R(vthread_t thr, vvp_code_t cp)
{
      પાછા store_dar<દ્વિગુણ>(thr, cp);
}

/*
 * %store/dar/str <var>
 */
તાર્કિક of_STORE_DAR_STR(vthread_t thr, vvp_code_t cp)
{
      પાછા store_dar<string>(thr, cp);
}

/*
 * %store/dar/vec4 <var>
 */
તાર્કિક of_STORE_DAR_VEC4(vthread_t thr, vvp_code_t cp)
{
      પાછા store_dar<vvp_vector4_t>(thr, cp);
}

તાર્કિક of_STORE_OBJ(vthread_t thr, vvp_code_t cp)
{
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);

      vvp_object_t val;
      thr->કાઢો_object(val);

      vvp_send_object(ptr, val, thr->wt_context);

      પાછા સાચું;
}

/*
 * %store/obja <array-label> <index>
 */
તાર્કિક of_STORE_OBJA(vthread_t thr, vvp_code_t cp)
{
      unsigned idx = cp->bit_idx[0];
      unsigned adr = thr->શબ્દો[idx].w_int;

      vvp_object_t val;
      thr->કાઢો_object(val);

      cp->array->set_word(adr, val);

      પાછા સાચું;
}


/*
 * %store/prop/obj <pid>, <idx>
 *
 * Pop an object value from the object stack, and store the value into
 * the property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
તાર્કિક of_STORE_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->સંખ્યા;
      unsigned idx = cp->bit_idx[0];

      જો (idx != 0) {
	    પ્રતિજ્ઞા(idx < vthread_s::શબ્દો_ગણતરી);
	    idx = thr->શબ્દો[idx].w_uint;
      }

      vvp_object_t val;
      thr->કાઢો_object(val);

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      પ્રતિજ્ઞા(cobj);

      cobj->set_object(pid, val, idx);

      પાછા સાચું;
}

સ્થિર શૂન્ય કાઢો_prop_val(vthread_t thr, દ્વિગુણ&val, unsigned)
{
      val = thr->કાઢો_real();
}

સ્થિર શૂન્ય કાઢો_prop_val(vthread_t thr, string&val, unsigned)
{
      val = thr->કાઢો_str();
}

સ્થિર શૂન્ય કાઢો_prop_val(vthread_t thr, vvp_vector4_t&val, unsigned wid)
{
      val = thr->કાઢો_vec4();
      પ્રતિજ્ઞા(val.size() >= wid);
      val.resize(wid);
}

સ્થિર શૂન્ય set_val(vvp_cobject*cobj, size_t pid, અચલ દ્વિગુણ&val)
{
      cobj->set_real(pid, val);
}

સ્થિર શૂન્ય set_val(vvp_cobject*cobj, size_t pid, અચલ string&val)
{
      cobj->set_string(pid, val);
}

સ્થિર શૂન્ય set_val(vvp_cobject*cobj, size_t pid, અચલ vvp_vector4_t&val)
{
      cobj->set_vec4(pid, val);
}

template <typename ELEM>
સ્થિર તાર્કિક store_prop(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
      size_t pid = cp->સંખ્યા;
      ELEM val;
      કાઢો_prop_val(thr, val, wid); // Pop the value to store.

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      પ્રતિજ્ઞા(cobj);

      set_val(cobj, pid, val);

      પાછા સાચું;
}

/*
 * %store/prop/r <id>
 *
 * Pop a real value from the real stack, and store the value into the
 * property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
તાર્કિક of_STORE_PROP_R(vthread_t thr, vvp_code_t cp)
{
      પાછા store_prop<દ્વિગુણ>(thr, cp);
}

/*
 * %store/prop/str <id>
 *
 * Pop a string value from the string stack, and store the value into
 * the property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
તાર્કિક of_STORE_PROP_STR(vthread_t thr, vvp_code_t cp)
{
      પાછા store_prop<string>(thr, cp);
}

/*
 * %store/prop/v <pid>, <wid>
 *
 * Store vector value into property <id> of cobject in the top of the
 * stack. Do NOT pop the object stack.
 */
તાર્કિક of_STORE_PROP_V(vthread_t thr, vvp_code_t cp)
{
      પાછા store_prop<vvp_vector4_t>(thr, cp, cp->bit_idx[0]);
}

template <વર્ગ QTYPE>
સ્થિર QTYPE* get_queue_prop(vvp_cobject*cobj, size_t pid)
{
      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      QTYPE* queue = qobj.peek<QTYPE>();
      જો (queue == 0) {
	    queue = નવું QTYPE;
	    vvp_object_t val(queue);
	    cobj->set_object(pid, val, 0);
      }
      પાછા queue;
}

template <typename ELEM, વર્ગ QTYPE>
સ્થિર તાર્કિક store_prop_qb(vthread_t thr, vvp_code_t cp, unsigned wid)
{
      size_t pid = cp->સંખ્યા;
      ELEM value;
      unsigned max_size = thr->શબ્દો[cp->bit_idx[0]].w_uint;
      કાઢો_કિંમત(thr, value, wid);

      vvp_object_t& ટોચ = thr->peek_object();
      vvp_cobject*cobj = ટોચ.peek<vvp_cobject>();
      પ્રતિજ્ઞા(cobj);

      QTYPE* queue = get_queue_prop<QTYPE>(cobj, pid);
      પ્રતિજ્ઞા(queue);
      queue->push_back(value, max_size);
      પાછા સાચું;
}

template <typename ELEM, વર્ગ QTYPE>
સ્થિર તાર્કિક store_prop_qf(vthread_t thr, vvp_code_t cp, unsigned wid)
{
      size_t pid = cp->સંખ્યા;
      ELEM value;
      unsigned max_size = thr->શબ્દો[cp->bit_idx[0]].w_uint;
      કાઢો_કિંમત(thr, value, wid);

      vvp_object_t& ટોચ = thr->peek_object();
      vvp_cobject*cobj = ટોચ.peek<vvp_cobject>();
      પ્રતિજ્ઞા(cobj);

      QTYPE* queue = get_queue_prop<QTYPE>(cobj, pid);
      પ્રતિજ્ઞા(queue);
      queue->push_front(value, max_size);
      પાછા સાચું;
}

તાર્કિક of_STORE_PROP_QB_R(vthread_t thr, vvp_code_t cp)
{
      પાછા store_prop_qb<દ્વિગુણ, vvp_queue_real>(thr, cp, 0);
}

તાર્કિક of_STORE_PROP_QB_STR(vthread_t thr, vvp_code_t cp)
{
      પાછા store_prop_qb<string, vvp_queue_string>(thr, cp, 0);
}

તાર્કિક of_STORE_PROP_QB_V(vthread_t thr, vvp_code_t cp)
{
      પાછા store_prop_qb<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

તાર્કિક of_STORE_PROP_QF_R(vthread_t thr, vvp_code_t cp)
{
      પાછા store_prop_qf<દ્વિગુણ, vvp_queue_real>(thr, cp, 0);
}

તાર્કિક of_STORE_PROP_QF_STR(vthread_t thr, vvp_code_t cp)
{
      પાછા store_prop_qf<string, vvp_queue_string>(thr, cp, 0);
}

તાર્કિક of_STORE_PROP_QF_V(vthread_t thr, vvp_code_t cp)
{
      પાછા store_prop_qf<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

template <typename ELEM, વર્ગ QTYPE>
સ્થિર તાર્કિક qinsert_prop(vthread_t thr, vvp_code_t cp, unsigned wid)
{
      int64_t idx = thr->શબ્દો[3].w_int;
      ELEM value;
      size_t pid = cp->સંખ્યા;
      કાઢો_કિંમત(thr, value, wid);

      vvp_object_t& ટોચ = thr->peek_object();
      vvp_cobject*cobj = ટોચ.peek<vvp_cobject>();
      પ્રતિજ્ઞા(cobj);

      QTYPE* queue = get_queue_prop<QTYPE>(cobj, pid);
      પ્રતિજ્ઞા(queue);
      જો (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at a negative "
	         << get_queue_type(value)
	         << " index (" << idx << "). ";
	    print_queue_કિંમત(value);
	    cerr << " was not added." << endl;
      } નહીંતર જો (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at an undefined "
	         << get_queue_type(value) << " index. ";
	    print_queue_કિંમત(value);
	    cerr << " was not added." << endl;
      } નહીંતર {
	    unsigned max_size = thr->શબ્દો[cp->bit_idx[0]].w_int;
	    queue->insert(idx, value, max_size);
      }
      પાછા સાચું;
}

તાર્કિક of_QINSERT_PROP_R(vthread_t thr, vvp_code_t cp)
{
      પાછા qinsert_prop<દ્વિગુણ, vvp_queue_real>(thr, cp, 0);
}

તાર્કિક of_QINSERT_PROP_STR(vthread_t thr, vvp_code_t cp)
{
      પાછા qinsert_prop<string, vvp_queue_string>(thr, cp, 0);
}

તાર્કિક of_QINSERT_PROP_V(vthread_t thr, vvp_code_t cp)
{
      પાછા qinsert_prop<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

template <typename ELEM, વર્ગ QTYPE>
સ્થિર તાર્કિક q_કાઢો_prop(vthread_t thr, vvp_code_t cp,
                       શૂન્ય (*get_val_func)(vvp_queue*, ELEM&),
                       અચલ અક્ષર*loc, unsigned wid)
{
      size_t pid = cp->સંખ્યા;

      vvp_object_t& ટોચ = thr->peek_object();
      vvp_cobject*cobj = ટોચ.peek<vvp_cobject>();
      પ્રતિજ્ઞા(cobj);

      QTYPE* queue = get_queue_prop<QTYPE>(cobj, pid);
      પ્રતિજ્ઞા(queue);

      size_t size = queue->get_size();

      ELEM value;
      જો (size) {
	    get_val_func(queue, value);
      } નહીંતર {
	    dq_default(value, wid);
	    cerr << thr->get_fileline()
	         << "Warning: pop_" << loc << "() on empty "
	         << get_queue_type(value) << "." << endl;
      }

      ધકેલો_કિંમત(thr, value, wid);
      પાછા સાચું;
}

template <typename ELEM, વર્ગ QTYPE>
સ્થિર તાર્કિક qpop_b_prop(vthread_t thr, vvp_code_t cp, unsigned wid)
{
      પાછા q_કાઢો_prop<ELEM, QTYPE>(thr, cp, get_back_કિંમત<ELEM>, "back", wid);
}

template <typename ELEM, વર્ગ QTYPE>
સ્થિર તાર્કિક qpop_f_prop(vthread_t thr, vvp_code_t cp, unsigned wid)
{
      પાછા q_કાઢો_prop<ELEM, QTYPE>(thr, cp, get_front_કિંમત<ELEM>, "front", wid);
}

તાર્કિક of_QPOP_PROP_B_REAL(vthread_t thr, vvp_code_t cp)
{
      પાછા qpop_b_prop<દ્વિગુણ, vvp_queue_real>(thr, cp, 0);
}

તાર્કિક of_QPOP_PROP_B_STR(vthread_t thr, vvp_code_t cp)
{
      પાછા qpop_b_prop<string, vvp_queue_string>(thr, cp, 0);
}

તાર્કિક of_QPOP_PROP_B_V(vthread_t thr, vvp_code_t cp)
{
      પાછા qpop_b_prop<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[0]);
}

તાર્કિક of_QPOP_PROP_F_REAL(vthread_t thr, vvp_code_t cp)
{
      પાછા qpop_f_prop<દ્વિગુણ, vvp_queue_real>(thr, cp, 0);
}

તાર્કિક of_QPOP_PROP_F_STR(vthread_t thr, vvp_code_t cp)
{
      પાછા qpop_f_prop<string, vvp_queue_string>(thr, cp, 0);
}

તાર્કિક of_QPOP_PROP_F_V(vthread_t thr, vvp_code_t cp)
{
      પાછા qpop_f_prop<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[0]);
}

સ્થિર vvp_vector4_t queue_unique_ulong_to_vec4(unsigned long ul, unsigned bits)
{
      vvp_vector4_t val(bits, BIT4_0);
      માટે (unsigned idx = 0; idx < bits && idx < 8 * કદ(unsigned long); idx += 1) {
	    જો (ul & 1UL) {
		  val.set_bit(idx, BIT4_1);
	    } નહીંતર {
		  val.set_bit(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      પાછા val;
}

template <વર્ગ SRC> સ્થિર vvp_queue_vec4* queue_run_unique_src(SRC* src,
								 unsigned wid,
								 તાર્કિક as_index)
{
      vvp_queue_vec4* dst = નવું vvp_queue_vec4();
      જો (!src) પાછા dst;
      size_t n = src->get_size();
      માટે (size_t i = 0; i < n; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    તાર્કિક seen = ખોટું;
	    માટે (size_t j = 0; j < i; j += 1) {
		  vvp_vector4_t vj(wid);
		  src->get_word(j, vj);
		  જો (vi.eeq(vj)) {
			seen = સાચું;
			તોડો;
		  }
	    }
	    જો (seen) ચાલુ;
	    જો (as_index) {
		  dst->push_back(queue_unique_ulong_to_vec4((unsigned long)i, 32), 0);
	    } નહીંતર {
		  dst->push_back(vi, 0);
	    }
      }
      પાછા dst;
}

template <વર્ગ SRC>
સ્થિર vvp_queue_vec4* queue_run_find_matches_src(SRC* src, unsigned wid,
						  અચલ vvp_vector4_t& cmp,
						  તાર્કિક as_index)
{
      vvp_queue_vec4* dst = નવું vvp_queue_vec4();
      જો (!src) પાછા dst;
      size_t n = src->get_size();
      માટે (size_t i = 0; i < n; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    જો (vi.eeq(cmp)) {
		  જો (as_index) {
			dst->push_back(queue_unique_ulong_to_vec4((unsigned long)i, 32), 0);
		  } નહીંતર {
			dst->push_back(vi, 0);
		  }
	    }
      }
      પાછા dst;
}

/* Array/queue locator methods return a queue with 0 or 1 elements (LRM). */
template <વર્ગ SRC>
સ્થિર vvp_queue_vec4* queue_find_first_last_to_queue_src(SRC* src,
							  unsigned wid,
							  અચલ vvp_vector4_t& cmp,
							  તાર્કિક want_first,
							  તાર્કિક want_index)
{
      vvp_queue_vec4* dst = નવું vvp_queue_vec4();
      જો (!src || src->get_size() == 0) પાછા dst;
      size_t n = src->get_size();
      જો (want_first) {
	    માટે (size_t i = 0; i < n; i += 1) {
		  vvp_vector4_t vi(wid);
		  src->get_word(i, vi);
		  જો (vi.eeq(cmp)) {
			જો (want_index) {
			      dst->push_back(
				  queue_unique_ulong_to_vec4((unsigned long)i, 32),
				  0);
			} નહીંતર {
			      dst->push_back(vi, 0);
			}
			પાછા dst;
		  }
	    }
      } નહીંતર {
	    માટે (size_t i = n; i > 0; ) {
		  i -= 1;
		  vvp_vector4_t vi(wid);
		  src->get_word(i, vi);
		  જો (vi.eeq(cmp)) {
			જો (want_index) {
			      dst->push_back(
				  queue_unique_ulong_to_vec4((unsigned long)i, 32),
				  0);
			} નહીંતર {
			      dst->push_back(vi, 0);
			}
			પાછા dst;
		  }
	    }
      }
      પાછા dst;
}

/* min()/max() return one element with the selected extremum. */
template <વર્ગ SRC>
સ્થિર vvp_queue_vec4* queue_run_min_max_src(SRC* src, unsigned wid, તાર્કિક want_max)
{
      vvp_queue_vec4* dst = નવું vvp_queue_vec4();
      જો (!src || src->get_size() == 0) પાછા dst;

      vvp_vector4_t best(wid);
      src->get_word(0, best);

      size_t n = src->get_size();
      માટે (size_t i = 1 ; i < n ; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);

	    vvp_bit4_t ge = compare_gtge(vi, best, BIT4_1);
	    તાર્કિક replace = want_max ? (ge == BIT4_1) : (ge == BIT4_0);
	    જો (replace) best = vi;
      }

      dst->push_back(best, 0);
      પાછા dst;
}

તાર્કિક of_QUEUE_FIND_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->કાઢો_vec4();
      unsigned wid = cp->bit_idx[0];
      પ્રતિજ્ઞા(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      જો (qsrc) dst = queue_run_find_matches_src(qsrc, wid, cmp, ખોટું);
      નહીંતર જો (dsrc) dst = queue_run_find_matches_src(dsrc, wid, cmp, ખોટું);
      નહીંતર {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_run_find_matches_src(src, wid, cmp, ખોટું);
      }
      thr->ધકેલો_object(vvp_object_t(dst));
      પાછા સાચું;
}

તાર્કિક of_QUEUE_FIND_PROP_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->કાઢો_vec4();
      size_t pid = cp->સંખ્યા;
      unsigned wid = cp->bit_idx[0];
      પ્રતિજ્ઞા(cmp.size() == wid);

      vvp_object_t& ટોચ = thr->peek_object();
      vvp_cobject*cobj = ટોચ.peek<vvp_cobject>();
      પ્રતિજ્ઞા(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_find_matches_src(qsrc, wid, cmp, ખોટું)
				 : queue_run_find_matches_src(dsrc, wid, cmp, ખોટું);
      thr->ધકેલો_object(vvp_object_t(dst));
      પાછા સાચું;
}

તાર્કિક of_QUEUE_FIND_INDEX_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->કાઢો_vec4();
      unsigned wid = cp->bit_idx[0];
      પ્રતિજ્ઞા(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      જો (qsrc) dst = queue_run_find_matches_src(qsrc, wid, cmp, સાચું);
      નહીંતર જો (dsrc) dst = queue_run_find_matches_src(dsrc, wid, cmp, સાચું);
      નહીંતર {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_run_find_matches_src(src, wid, cmp, સાચું);
      }
      thr->ધકેલો_object(vvp_object_t(dst));
      પાછા સાચું;
}

તાર્કિક of_QUEUE_FIND_INDEX_PROP_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->કાઢો_vec4();
      size_t pid = cp->સંખ્યા;
      unsigned wid = cp->bit_idx[0];
      પ્રતિજ્ઞા(cmp.size() == wid);

      vvp_object_t& ટોચ = thr->peek_object();
      vvp_cobject*cobj = ટોચ.peek<vvp_cobject>();
      પ્રતિજ્ઞા(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_find_matches_src(qsrc, wid, cmp, સાચું)
				 : queue_run_find_matches_src(dsrc, wid, cmp, સાચું);
      thr->ધકેલો_object(vvp_object_t(dst));
      પાછા સાચું;
}

તાર્કિક of_QUEUE_FIND_FIRST_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->કાઢો_vec4();
      unsigned wid = cp->bit_idx[0];
      પ્રતિજ્ઞા(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      જો (qsrc) dst = queue_find_first_last_to_queue_src(qsrc, wid, cmp, સાચું, ખોટું);
      નહીંતર જો (dsrc) dst = queue_find_first_last_to_queue_src(dsrc, wid, cmp, સાચું, ખોટું);
      નહીંતર {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_find_first_last_to_queue_src(src, wid, cmp, સાચું, ખોટું);
      }
      thr->ધકેલો_object(vvp_object_t(dst));
      પાછા સાચું;
}

તાર્કિક of_QUEUE_FIND_FIRST_PROP_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->કાઢો_vec4();
      size_t pid = cp->સંખ્યા;
      unsigned wid = cp->bit_idx[0];
      પ્રતિજ્ઞા(cmp.size() == wid);

      vvp_object_t& ટોચ = thr->peek_object();
      vvp_cobject*cobj = ટોચ.peek<vvp_cobject>();
      પ્રતિજ્ઞા(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst =
	    qsrc ? queue_find_first_last_to_queue_src(qsrc, wid, cmp, સાચું, ખોટું)
		 : queue_find_first_last_to_queue_src(dsrc, wid, cmp, સાચું, ખોટું);
      thr->ધકેલો_object(vvp_object_t(dst));
      પાછા સાચું;
}

તાર્કિક of_QUEUE_FIND_FIRST_INDEX_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->કાઢો_vec4();
      unsigned wid = cp->bit_idx[0];
      પ્રતિજ્ઞા(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      જો (qsrc) dst = queue_find_first_last_to_queue_src(qsrc, wid, cmp, સાચું, સાચું);
      નહીંતર જો (dsrc) dst = queue_find_first_last_to_queue_src(dsrc, wid, cmp, સાચું, સાચું);
      નહીંતર {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_find_first_last_to_queue_src(src, wid, cmp, સાચું, સાચું);
      }
      thr->ધકેલો_object(vvp_object_t(dst));
      પાછા સાચું;
}

તાર્કિક of_QUEUE_FIND_FIRST_INDEX_PROP_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->કાઢો_vec4();
      size_t pid = cp->સંખ્યા;
      unsigned wid = cp->bit_idx[0];
      પ્રતિજ્ઞા(cmp.size() == wid);

      vvp_object_t& ટોચ = thr->peek_object();
      vvp_cobject*cobj = ટોચ.peek<vvp_cobject>();
      પ્રતિજ્ઞા(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst =
	    qsrc ? queue_find_first_last_to_queue_src(qsrc, wid, cmp, સાચું, સાચું)
		 : queue_find_first_last_to_queue_src(dsrc, wid, cmp, સાચું, સાચું);
      thr->ધકેલો_object(vvp_object_t(dst));
      પાછા સાચું;
}

તાર્કિક of_QUEUE_FIND_LAST_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->કાઢો_vec4();
      unsigned wid = cp->bit_idx[0];
      પ્રતિજ્ઞા(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      જો (qsrc) dst = queue_find_first_last_to_queue_src(qsrc, wid, cmp, ખોટું, ખોટું);
      નહીંતર જો (dsrc) dst = queue_find_first_last_to_queue_src(dsrc, wid, cmp, ખોટું, ખોટું);
      નહીંતર {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_find_first_last_to_queue_src(src, wid, cmp, ખોટું, ખોટું);
      }
      thr->ધકેલો_object(vvp_object_t(dst));
      પાછા સાચું;
}

તાર્કિક of_QUEUE_FIND_LAST_PROP_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->કાઢો_vec4();
      size_t pid = cp->સંખ્યા;
      unsigned wid = cp->bit_idx[0];
      પ્રતિજ્ઞા(cmp.size() == wid);

      vvp_object_t& ટોચ = thr->peek_object();
      vvp_cobject*cobj = ટોચ.peek<vvp_cobject>();
      પ્રતિજ્ઞા(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst =
	    qsrc ? queue_find_first_last_to_queue_src(qsrc, wid, cmp, ખોટું, ખોટું)
		 : queue_find_first_last_to_queue_src(dsrc, wid, cmp, ખોટું, ખોટું);
      thr->ધકેલો_object(vvp_object_t(dst));
      પાછા સાચું;
}

તાર્કિક of_QUEUE_FIND_LAST_INDEX_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->કાઢો_vec4();
      unsigned wid = cp->bit_idx[0];
      પ્રતિજ્ઞા(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      જો (qsrc) dst = queue_find_first_last_to_queue_src(qsrc, wid, cmp, ખોટું, સાચું);
      નહીંતર જો (dsrc) dst = queue_find_first_last_to_queue_src(dsrc, wid, cmp, ખોટું, સાચું);
      નહીંતર {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_find_first_last_to_queue_src(src, wid, cmp, ખોટું, સાચું);
      }
      thr->ધકેલો_object(vvp_object_t(dst));
      પાછા સાચું;
}

તાર્કિક of_QUEUE_FIND_LAST_INDEX_PROP_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->કાઢો_vec4();
      size_t pid = cp->સંખ્યા;
      unsigned wid = cp->bit_idx[0];
      પ્રતિજ્ઞા(cmp.size() == wid);

      vvp_object_t& ટોચ = thr->peek_object();
      vvp_cobject*cobj = ટોચ.peek<vvp_cobject>();
      પ્રતિજ્ઞા(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst =
	    qsrc ? queue_find_first_last_to_queue_src(qsrc, wid, cmp, ખોટું, સાચું)
		 : queue_find_first_last_to_queue_src(dsrc, wid, cmp, ખોટું, સાચું);
      thr->ધકેલો_object(vvp_object_t(dst));
      પાછા સાચું;
}

તાર્કિક of_QUEUE_MIN_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      જો (qsrc) dst = queue_run_min_max_src(qsrc, wid, ખોટું);
      નહીંતર જો (dsrc) dst = queue_run_min_max_src(dsrc, wid, ખોટું);
      નહીંતર {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_run_min_max_src(src, wid, ખોટું);
      }
      thr->ધકેલો_object(vvp_object_t(dst));
      પાછા સાચું;
}

તાર્કિક of_QUEUE_MIN_OBJ_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->કાઢો_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_min_max_src(qsrc, wid, ખોટું)
				 : queue_run_min_max_src(dsrc, wid, ખોટું);
      thr->ધકેલો_object(vvp_object_t(dst));
      પાછા સાચું;
}

તાર્કિક of_QUEUE_MIN_PROP_V(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->સંખ્યા;
      unsigned wid = cp->bit_idx[0];

      vvp_object_t& ટોચ = thr->peek_object();
      vvp_cobject*cobj = ટોચ.peek<vvp_cobject>();
      પ્રતિજ્ઞા(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_min_max_src(qsrc, wid, ખોટું)
				 : queue_run_min_max_src(dsrc, wid, ખોટું);
      thr->ધકેલો_object(vvp_object_t(dst));
      પાછા સાચું;
}

તાર્કિક of_QUEUE_MAX_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      જો (qsrc) dst = queue_run_min_max_src(qsrc, wid, સાચું);
      નહીંતર જો (dsrc) dst = queue_run_min_max_src(dsrc, wid, સાચું);
      નહીંતર {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_run_min_max_src(src, wid, સાચું);
      }
      thr->ધકેલો_object(vvp_object_t(dst));
      પાછા સાચું;
}

તાર્કિક of_QUEUE_MAX_OBJ_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->કાઢો_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_min_max_src(qsrc, wid, સાચું)
				 : queue_run_min_max_src(dsrc, wid, સાચું);
      thr->ધકેલો_object(vvp_object_t(dst));
      પાછા સાચું;
}

તાર્કિક of_QUEUE_MAX_PROP_V(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->સંખ્યા;
      unsigned wid = cp->bit_idx[0];

      vvp_object_t& ટોચ = thr->peek_object();
      vvp_cobject*cobj = ટોચ.peek<vvp_cobject>();
      પ્રતિજ્ઞા(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_min_max_src(qsrc, wid, સાચું)
				 : queue_run_min_max_src(dsrc, wid, સાચું);
      thr->ધકેલો_object(vvp_object_t(dst));
      પાછા સાચું;
}

તાર્કિક of_QUEUE_UNIQUE_OBJ_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->કાઢો_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_unique_src(qsrc, wid, ખોટું)
				 : queue_run_unique_src(dsrc, wid, ખોટું);
      thr->ધકેલો_object(vvp_object_t(dst));
      પાછા સાચું;
}

તાર્કિક of_QUEUE_UNIQUE_INDEX_OBJ_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->કાઢો_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_unique_src(qsrc, wid, સાચું)
				 : queue_run_unique_src(dsrc, wid, સાચું);
      thr->ધકેલો_object(vvp_object_t(dst));
      પાછા સાચું;
}

તાર્કિક of_QUEUE_UNIQUE_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      જો (qsrc) dst = queue_run_unique_src(qsrc, wid, ખોટું);
      નહીંતર જો (dsrc) dst = queue_run_unique_src(dsrc, wid, ખોટું);
      નહીંતર {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_run_unique_src(src, wid, ખોટું);
      }
      thr->ધકેલો_object(vvp_object_t(dst));
      પાછા સાચું;
}

તાર્કિક of_QUEUE_UNIQUE_PROP_V(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->સંખ્યા;
      unsigned wid = cp->bit_idx[0];

      vvp_object_t& ટોચ = thr->peek_object();
      vvp_cobject*cobj = ટોચ.peek<vvp_cobject>();
      પ્રતિજ્ઞા(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_unique_src(qsrc, wid, ખોટું)
				 : queue_run_unique_src(dsrc, wid, ખોટું);
      thr->ધકેલો_object(vvp_object_t(dst));
      પાછા સાચું;
}

તાર્કિક of_QUEUE_UNIQUE_INDEX_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      જો (qsrc) dst = queue_run_unique_src(qsrc, wid, સાચું);
      નહીંતર જો (dsrc) dst = queue_run_unique_src(dsrc, wid, સાચું);
      નહીંતર {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_run_unique_src(src, wid, સાચું);
      }
      thr->ધકેલો_object(vvp_object_t(dst));
      પાછા સાચું;
}

તાર્કિક of_QUEUE_UNIQUE_INDEX_PROP_V(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->સંખ્યા;
      unsigned wid = cp->bit_idx[0];

      vvp_object_t& ટોચ = thr->peek_object();
      vvp_cobject*cobj = ટોચ.peek<vvp_cobject>();
      પ્રતિજ્ઞા(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_unique_src(qsrc, wid, સાચું)
				 : queue_run_unique_src(dsrc, wid, સાચું);
      thr->ધકેલો_object(vvp_object_t(dst));
      પાછા સાચું;
}

તાર્કિક of_PROP_QUEUE_SIZE(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->સંખ્યા;

      vvp_object_t& ટોચ = thr->peek_object();
      vvp_cobject*cobj = ટોચ.peek<vvp_cobject>();
      પ્રતિજ્ઞા(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);

      size_t sz = qsrc ? qsrc->get_size() : (dsrc ? dsrc->get_size() : 0);

      vvp_vector4_t val(32, BIT4_0);
      unsigned long ul = sz;
      માટે (unsigned idx = 0; idx < 32; idx++) {
	    જો (ul & 1UL) {
		  val.set_bit(idx, BIT4_1);
	    } નહીંતર {
		  val.set_bit(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      thr->ધકેલો_vec4(val);
      પાછા સાચું;
}

તાર્કિક of_QUEUE_SIZE_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      size_t sz =
	    qsrc ? qsrc->get_size() : (dsrc ? dsrc->get_size() : 0);

      vvp_vector4_t val(32, BIT4_0);
      unsigned long ul = sz;
      માટે (unsigned idx = 0; idx < 32; idx++) {
	    જો (ul & 1UL) {
		  val.set_bit(idx, BIT4_1);
	    } નહીંતર {
		  val.set_bit(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      thr->ધકેલો_vec4(val);
      પાછા સાચું;
}

તાર્કિક of_QUEUE_WORD_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      unsigned ix_reg = cp->bit_idx[1];
      int64_t adr = thr->શબ્દો[ix_reg].w_int;
      જો (thr->flags[4] == BIT4_1) {
	    thr->ધકેલો_vec4(vvp_vector4_t(wid));
	    પાછા સાચું;
      }
      જો (adr < 0) {
	    thr->ધકેલો_vec4(vvp_vector4_t(wid));
	    પાછા સાચું;
      }
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t out(wid);
      જો (qsrc) {
	    જો ((size_t)adr >= qsrc->get_size()) {
		  thr->ધકેલો_vec4(out);
		  પાછા સાચું;
	    }
	    qsrc->get_word((unsigned)adr, out);
      } નહીંતર જો (dsrc) {
	    જો ((size_t)adr >= dsrc->get_size()) {
		  thr->ધકેલો_vec4(out);
		  પાછા સાચું;
	    }
	    dsrc->get_word((unsigned)adr, out);
      } નહીંતર {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    જો (!src || (size_t)adr >= src->get_size()) {
		  thr->ધકેલો_vec4(out);
		  પાછા સાચું;
	    }
	    src->get_word((unsigned)adr, out);
      }
      thr->ધકેલો_vec4(out);
      પાછા સાચું;
}

તાર્કિક of_QUEUE_WORD_PROP_V(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->સંખ્યા;
      unsigned wid = cp->bit_idx[0];
      unsigned ix_reg = cp->bit_idx[1];
      int64_t adr = thr->શબ્દો[ix_reg].w_int;
      જો (thr->flags[4] == BIT4_1) {
	    thr->ધકેલો_vec4(vvp_vector4_t(wid));
	    પાછા સાચું;
      }
      જો (adr < 0) {
	    thr->ધકેલો_vec4(vvp_vector4_t(wid));
	    પાછા સાચું;
      }

      vvp_object_t saved_ટોચ;
      તાર્કિક popped_ટોચ = ખોટું;
      vvp_object_t& ટોચ = thr->peek_object();
      vvp_cobject*cobj = ટોચ.peek<vvp_cobject>();
      જો (cobj == 0) {
	    // Array-locator with(property) keeps an accumulator queue on top.
	    // Temporarily pop it to access the class object beneath.
	    thr->કાઢો_object(saved_ટોચ);
	    popped_ટોચ = સાચું;
	    vvp_object_t& below = thr->peek_object();
	    cobj = below.peek<vvp_cobject>();
      }
      જો (cobj == 0) {
	    જો (popped_ટોચ) thr->ધકેલો_object(saved_ટોચ);
	    thr->ધકેલો_vec4(vvp_vector4_t(wid));
	    પાછા સાચું;
      }

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t out(wid);
      જો (qsrc) {
	    જો ((size_t)adr >= qsrc->get_size()) {
		  thr->ધકેલો_vec4(out);
		  પાછા સાચું;
	    }
	    qsrc->get_word((unsigned)adr, out);
      } નહીંતર જો (dsrc) {
	    જો ((size_t)adr >= dsrc->get_size()) {
		  thr->ધકેલો_vec4(out);
		  પાછા સાચું;
	    }
	    dsrc->get_word((unsigned)adr, out);
      } નહીંતર {
	    thr->ધકેલો_vec4(out);
	    જો (popped_ટોચ) thr->ધકેલો_object(saved_ટોચ);
	    પાછા સાચું;
      }
      જો (popped_ટોચ) thr->ધકેલો_object(saved_ટોચ);
      thr->ધકેલો_vec4(out);
      પાછા સાચું;
}

તાર્કિક of_CMPIX_LTU(vthread_t thr, vvp_code_t cp)
{
      unsigned a = cp->સંખ્યા;
      unsigned b = cp->bit_idx[0];
      જો (thr->શબ્દો[a].w_uint < thr->શબ્દો[b].w_uint) {
	    thr->flags[4] = BIT4_1;
      } નહીંતર {
	    thr->flags[4] = BIT4_0;
      }
      પાછા સાચું;
}

તાર્કિક of_CMPIX_SLT0(vthread_t thr, vvp_code_t cp)
{
      unsigned r = cp->સંખ્યા;
      thr->flags[4] = (thr->શબ્દો[r].w_int < 0) ? BIT4_1 : BIT4_0;
      પાછા સાચું;
}

તાર્કિક of_ધકેલો_IX_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned ix = cp->સંખ્યા;
      unsigned wid = cp->bit_idx[0];
      unsigned sgn = cp->bit_idx[1];
      uint32_t ubits;
      જો (sgn) {
	    int32_t sv = (int32_t)thr->શબ્દો[ix].w_int;
	    ubits = (uint32_t)sv;
      } નહીંતર {
	    ubits = (uint32_t)thr->શબ્દો[ix].w_uint;
      }
      જો (wid > 32) wid = 32;
      vvp_vector4_t tmp(wid, BIT4_0);
      માટે (unsigned idx = 0; idx < wid; idx++) {
	    જો (ubits & 1u) {
		  tmp.set_bit(idx, BIT4_1);
	    } નહીંતર {
		  tmp.set_bit(idx, BIT4_0);
	    }
	    ubits >>= 1;
      }
      thr->ધકેલો_vec4(tmp);
      પાછા સાચું;
}

તાર્કિક of_QUEUE_NEW_EMPTY_V(vthread_t thr, vvp_code_t)
{
      thr->ધકેલો_object(vvp_object_t(નવું vvp_queue_vec4()));
      પાછા સાચું;
}

તાર્કિક of_QUEUE_APPEND_WORD_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      vvp_vector4_t elem = thr->કાઢો_vec4();
      પ્રતિજ્ઞા(elem.size() == wid);
      vvp_object_t qobj;
      thr->કાઢો_object(qobj);
      vvp_queue_vec4* q = qobj.peek<vvp_queue_vec4>();
      પ્રતિજ્ઞા(q);
      q->push_back(elem, 0);
      thr->ધકેલો_object(qobj);
      પાછા સાચું;
}

તાર્કિક of_DELETE_PROP_ELEM(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->સંખ્યા;

      int64_t idx_val = thr->શબ્દો[3].w_int;
      જો (thr->flags[4] == BIT4_1) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with undefined index."
	         << endl;
	    પાછા સાચું;
      }
      જો (idx_val < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with negative index."
	         << endl;
	    પાછા સાચું;
      }
      size_t idx = idx_val;

      vvp_object_t& ટોચ = thr->peek_object();
      vvp_cobject*cobj = ટોચ.peek<vvp_cobject>();
      પ્રતિજ્ઞા(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue* queue = qobj.peek<vvp_queue>();
      જો (queue == 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping delete(" << idx
	         << ") on empty queue." << endl;
      } નહીંતર {
	    size_t size = queue->get_size();
	    જો (idx >= size) {
		  cerr << thr->get_fileline()
		       << "Warning: skipping out of range delete(" << idx
		       << ") on queue of size " << size << "." << endl;
	    } નહીંતર {
		  queue->erase(idx);
	    }
      }

      પાછા સાચું;
}

તાર્કિક of_DELETE_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->સંખ્યા;

      vvp_object_t& ટોચ = thr->peek_object();
      vvp_cobject*cobj = ટોચ.peek<vvp_cobject>();
      પ્રતિજ્ઞા(cobj);

      cobj->set_object(pid, vvp_object_t(), 0);

      પાછા સાચું;
}

template <typename ELEM, વર્ગ QTYPE>
સ્થિર તાર્કિક store_qb(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
      ELEM value;
      vvp_net_t*net = cp->net;
      unsigned max_size = thr->શબ્દો[cp->bit_idx[0]].w_int;
      કાઢો_કિંમત(thr, value, wid); // Pop the value to store.

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      પ્રતિજ્ઞા(queue);
      queue->push_back(value, max_size);
      પાછા સાચું;
}

/*
 * %store/qb/r <var-label>, <max-idx>
 */
તાર્કિક of_STORE_QB_R(vthread_t thr, vvp_code_t cp)
{
      પાછા store_qb<દ્વિગુણ, vvp_queue_real>(thr, cp);
}

/*
 * %store/qb/str <var-label>, <max-idx>
 */
તાર્કિક of_STORE_QB_STR(vthread_t thr, vvp_code_t cp)
{
      પાછા store_qb<string, vvp_queue_string>(thr, cp);
}

/*
 * %store/qb/v <var-label>, <max-idx>, <wid>
 */
તાર્કિક of_STORE_QB_V(vthread_t thr, vvp_code_t cp)
{
      પાછા store_qb<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

template <typename ELEM, વર્ગ QTYPE>
સ્થિર તાર્કિક store_qdar(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
      int64_t idx = thr->શબ્દો[3].w_int;
      ELEM value;
      vvp_net_t*net = cp->net;
      કાઢો_કિંમત(thr, value, wid); // Pop the value to store.

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      પ્રતિજ્ઞા(queue);
      જો (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot assign to a negative "
	         << get_queue_type(value)
	         << " index (" << idx << "). ";
	    print_queue_કિંમત(value);
	    cerr << " was not added." << endl;
      } નહીંતર જો (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot assign to an undefined "
	         << get_queue_type(value) << " index. ";
	    print_queue_કિંમત(value);
	    cerr << " was not added." << endl;
      } નહીંતર {
	    unsigned max_size = thr->શબ્દો[cp->bit_idx[0]].w_int;
	    queue->set_word_max(idx, value, max_size);
      }
      પાછા સાચું;
}

/*
 * %store/qdar/r <var>, idx
 */
તાર્કિક of_STORE_QDAR_R(vthread_t thr, vvp_code_t cp)
{
      પાછા store_qdar<દ્વિગુણ, vvp_queue_real>(thr, cp);
}

/*
 * %store/qdar/str <var>, idx
 */
તાર્કિક of_STORE_QDAR_STR(vthread_t thr, vvp_code_t cp)
{
      પાછા store_qdar<string, vvp_queue_string>(thr, cp);
}

/*
 * %store/qdar/v <var>, idx
 */
તાર્કિક of_STORE_QDAR_V(vthread_t thr, vvp_code_t cp)
{
      પાછા store_qdar<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

template <typename ELEM, વર્ગ QTYPE>
સ્થિર તાર્કિક store_qf(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
      ELEM value;
      vvp_net_t*net = cp->net;
      unsigned max_size = thr->શબ્દો[cp->bit_idx[0]].w_int;
      કાઢો_કિંમત(thr, value, wid); // Pop the value to store.

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      પ્રતિજ્ઞા(queue);
      queue->push_front(value, max_size);
      પાછા સાચું;
}
/*
 * %store/qf/r <var-label>, <max-idx>
 */
તાર્કિક of_STORE_QF_R(vthread_t thr, vvp_code_t cp)
{
      પાછા store_qf<દ્વિગુણ, vvp_queue_real>(thr, cp);
}

/*
 * %store/qf/str <var-label>, <max-idx>
 */
તાર્કિક of_STORE_QF_STR(vthread_t thr, vvp_code_t cp)
{
      પાછા store_qf<string, vvp_queue_string>(thr, cp);
}

/*
 * %store/qb/v <var-label>, <max-idx>, <wid>
 */
તાર્કિક of_STORE_QF_V(vthread_t thr, vvp_code_t cp)
{
      પાછા store_qf<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

template <typename ELEM, વર્ગ QTYPE>
સ્થિર તાર્કિક store_qobj(vthread_t thr, vvp_code_t cp, unsigned wid=0)
{
// FIXME: Can we actually use wid here?
      (શૂન્ય)wid;
      vvp_net_t*net = cp->net;

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      પ્રતિજ્ઞા(queue);

      vvp_object_t src;
      thr->કાઢો_object(src);

        // If it is null just clear the queue
      જો (src.test_nil())
	    queue->erase_tail(0);
      નહીંતર {
	    unsigned max_size = thr->શબ્દો[cp->bit_idx[0]].w_int;
	    queue->copy_elems(src, max_size);
      }

      પાછા સાચું;
}

તાર્કિક of_STORE_QOBJ_R(vthread_t thr, vvp_code_t cp)
{
      પાછા store_qobj<દ્વિગુણ, vvp_queue_real>(thr, cp);
}

તાર્કિક of_STORE_QOBJ_STR(vthread_t thr, vvp_code_t cp)
{
      પાછા store_qobj<string, vvp_queue_string>(thr, cp);
}

તાર્કિક of_STORE_QOBJ_V(vthread_t thr, vvp_code_t cp)
{
      પાછા store_qobj<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

સ્થિર શૂન્ય vvp_send(vthread_t thr, vvp_net_ptr_t ptr, અચલ દ્વિગુણ&val)
{
      vvp_send_real(ptr, val, thr->wt_context);
}

સ્થિર શૂન્ય vvp_send(vthread_t thr, vvp_net_ptr_t ptr, અચલ string&val)
{
      vvp_send_string(ptr, val, thr->wt_context);
}

template <typename ELEM>
સ્થિર તાર્કિક store(vthread_t thr, vvp_code_t cp)
{
      ELEM val;
      કાઢો_કિંમત(thr, val, 0);
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_send(thr, ptr, val);
      પાછા સાચું;
}

તાર્કિક of_STORE_REAL(vthread_t thr, vvp_code_t cp)
{
      પાછા store<દ્વિગુણ>(thr, cp);
}

template <typename ELEM>
સ્થિર તાર્કિક storea(vthread_t thr, vvp_code_t cp)
{
      unsigned idx = cp->bit_idx[0];
      ELEM val;
      કાઢો_કિંમત(thr, val, 0);

      જો (thr->flags[4] != BIT4_1) {
	    unsigned adr = thr->શબ્દો[idx].w_int;
	    cp->array->set_word(adr, val);
      }

      પાછા સાચું;
}

/*
 * %store/reala <var-label> <index>
 */
તાર્કિક of_STORE_REALA(vthread_t thr, vvp_code_t cp)
{
      પાછા storea<દ્વિગુણ>(thr, cp);
}

તાર્કિક of_STORE_STR(vthread_t thr, vvp_code_t cp)
{
      પાછા store<string>(thr, cp);
}

/*
 * %store/stra <array-label> <index>
 */
તાર્કિક of_STORE_STRA(vthread_t thr, vvp_code_t cp)
{
      પાછા storea<string>(thr, cp);
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
તાર્કિક of_STORE_VEC4(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr(cp->net, 0);
      અચલ vvp_signal_કિંમત*sig = dynamic_cast<vvp_signal_કિંમત*> (cp->net->fil);
      unsigned off_index = cp->bit_idx[0];
      unsigned પૂર્ણાંક wid = cp->bit_idx[1];

      int64_t off = off_index ? thr->શબ્દો[off_index].w_int : 0;
      unsigned પૂર્ણાંક sig_કિંમત_size = sig->કિંમત_size();

      vvp_vector4_t&val = thr->peek_vec4();
      unsigned val_size = val.size();

      જો (val_size < wid) {
	    cerr << thr->get_fileline()
	         << "XXXX Internal error: val.size()=" << val_size
		 << ", expecting >= " << wid << endl;
      }
      પ્રતિજ્ઞા(val_size >= wid);
      જો (val_size > wid) {
	    val.resize(wid);
      }

	// If there is a problem loading the index register, flags-4
	// will be set to 1, and we know here to skip the actual assignment.
      જો (off_index!=0 && thr->flags[4] == BIT4_1) {
	    thr->કાઢો_vec4(1);
	    પાછા સાચું;
      }

      જો (!resize_rval_vec(val, off, sig_કિંમત_size)) {
	    thr->કાઢો_vec4(1);
	    પાછા સાચું;
      }

      જો (off == 0 && val.size() == sig_કિંમત_size)
	    vvp_send_vec4(ptr, val, thr->wt_context);
      નહીંતર
	    vvp_send_vec4_pv(ptr, val, off, sig_કિંમત_size, thr->wt_context);

      thr->કાઢો_vec4(1);
      પાછા સાચું;
}

/*
 * %store/vec4a <var-label>, <addr>, <offset>
 */
તાર્કિક of_STORE_VEC4A(vthread_t thr, vvp_code_t cp)
{
      unsigned adr_index = cp->bit_idx[0];
      unsigned off_index = cp->bit_idx[1];

      long adr = adr_index? thr->શબ્દો[adr_index].w_int : 0;
      int64_t off = off_index ? thr->શબ્દો[off_index].w_int : 0;

	// Suppress action if flags-4 is true.
      જો (thr->flags[4] == BIT4_1) {
	    thr->કાઢો_vec4(1);
	    પાછા સાચું;
      }

      vvp_vector4_t &value = thr->peek_vec4();

      જો (!resize_rval_vec(value, off, cp->array->get_word_size())) {
	    thr->કાઢો_vec4(1);
	    પાછા સાચું;
      }

      cp->array->set_word(adr, off, value);

      thr->કાઢો_vec4(1);
      પાછા સાચું;
}

/*
 * %sub
 *   pop r;
 *   pop l;
 *   push l-r;
 */
તાર્કિક of_SUB(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t r = thr->કાઢો_vec4();
      vvp_vector4_t&l = thr->peek_vec4();

      l.sub(r);
      પાછા સાચું;
}

/*
 * %subi <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
તાર્કિક of_SUBI(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->સંખ્યા;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_immediate_rval (cp, r);

      l.sub(r);

      પાછા સાચું;

}

તાર્કિક of_SUB_WR(vthread_t thr, vvp_code_t)
{
      દ્વિગુણ r = thr->કાઢો_real();
      દ્વિગુણ l = thr->કાઢો_real();
      thr->ધકેલો_real(l - r);
      પાછા સાચું;
}

/*
 * %substr <first>, <last>
 * Pop a string, take the substring (SystemVerilog style), and return
 * the result to the stack. This opcode actually works by editing the
 * string in place.
 */
તાર્કિક of_SUBSTR(vthread_t thr, vvp_code_t cp)
{
      int32_t first = thr->શબ્દો[cp->bit_idx[0]].w_int;
      int32_t last = thr->શબ્દો[cp->bit_idx[1]].w_int;
      string&val = thr->peek_str(0);

      જો (first < 0 || last < first || last >= (int32_t)val.size()) {
	    val = string("");
	    પાછા સાચું;
      }

      val = val.substr(first, last-first+1);
      પાછા સાચું;
}

/*
 * %substr/vec4 <index>, <wid>
 */
તાર્કિક of_SUBSTR_VEC4(vthread_t thr, vvp_code_t cp)
{
      unsigned sel_idx = cp->bit_idx[0];
      unsigned wid = cp->bit_idx[1];

      int32_t sel = thr->શબ્દો[sel_idx].w_int;
      અચલ string&val = thr->peek_str(0);

      પ્રતિજ્ઞા(wid%8 == 0);

      જો (sel < 0 || sel >= (int32_t)val.size()) {
	    vvp_vector4_t res (wid, BIT4_0);
	    thr->ધકેલો_vec4(res);
	    પાછા સાચું;
      }

      vvp_vector4_t res (wid, BIT4_0);

      પ્રતિજ્ઞા(wid==8);
      unsigned અક્ષર tmp = val[sel];
      માટે (પૂર્ણાંક idx = 0 ; idx < 8 ; idx += 1) {
	    જો (tmp & (1<<idx))
		  res.set_bit(idx, BIT4_1);
      }

      thr->ધકેલો_vec4(res);
      પાછા સાચું;
}

તાર્કિક of_FILE_લીટી(vthread_t thr, vvp_code_t cp)
{
      vpiHandle handle = cp->handle;

	/* When it is available, keep the file/line information in the
	   thread for error/warning messages. */
      thr->set_fileline(vpi_get_str(vpiFile, handle),
                        vpi_get(vpi_લીટી_No, handle));

      જો (show_file_લીટી)
	    cerr << thr->get_fileline()
	         << vpi_get_str(_vpiDescription, handle) << endl;

      પાછા સાચું;
}

/*
 * %test_nul <var-label>;
 * Test if the object at the specified variable is nil. If so, write
 * "1" into flags[4], otherwise write "0" into flags[4].
 */
તાર્કિક of_TEST_NUL(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      પ્રતિજ્ઞા(net);
      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      પ્રતિજ્ઞા(obj);

      જો (obj->get_object().test_nil())
	    thr->flags[4] = BIT4_1;
      નહીંતર
	    thr->flags[4] = BIT4_0;

      પાછા સાચું;
}

તાર્કિક of_TEST_NUL_A(vthread_t thr, vvp_code_t cp)
{
      unsigned idx = cp->bit_idx[0];
      unsigned adr = thr->શબ્દો[idx].w_int;
      vvp_object_t word;

	/* If the address is undefined, return true. */
      જો (thr->flags[4] == BIT4_1) {
	    પાછા સાચું;
      }

      cp->array->get_word_obj(adr, word);
      જો (word.test_nil())
	    thr->flags[4] = BIT4_1;
      નહીંતર
	    thr->flags[4] = BIT4_0;

      પાછા સાચું;
}

તાર્કિક of_TEST_NUL_OBJ(vthread_t thr, vvp_code_t)
{
      જો (thr->peek_object().test_nil())
	    thr->flags[4] = BIT4_1;
      નહીંતર
	    thr->flags[4] = BIT4_0;
      પાછા સાચું;
}

/*
 * %test_nul/prop <pid>, <idx>
 */
તાર્કિક of_TEST_NUL_PROP(vthread_t thr, vvp_code_t cp)
{
      unsigned pid = cp->સંખ્યા;
      unsigned idx = cp->bit_idx[0];

      જો (idx != 0) {
	    પ્રતિજ્ઞા(idx < vthread_s::શબ્દો_ગણતરી);
	    idx = thr->શબ્દો[idx].w_uint;
      }

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj  = obj.peek<vvp_cobject>();

      vvp_object_t val;
      cobj->get_object(pid, val, idx);

      જો (val.test_nil())
	    thr->flags[4] = BIT4_1;
      નહીંતર
	    thr->flags[4] = BIT4_0;

      પાછા સાચું;
}

/* and() reduction returns the scalar sum (Verilog-style bitwise-AND per word). */
template <વર્ગ SRC>
સ્થિર vvp_vector4_t uparr_and_શબ્દો_src(SRC* src, unsigned wid)
{
      જો (!src) પાછા vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_1);
      માટે (size_t i = 0; i < src->get_size(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    res &= vi;
      }
      પાછા res;
}

તાર્કિક of_UPARR_AND_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      જો (qsrc) res = uparr_and_શબ્દો_src(qsrc, wid);
      નહીંતર જો (dsrc) res = uparr_and_શબ્દો_src(dsrc, wid);
      નહીંતર {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    res = uparr_and_શબ્દો_src(src, wid);
      }
      thr->ધકેલો_vec4(res);
      પાછા સાચું;
}

/* or() reduction returns the scalar sum (Verilog-style bitwise-AND per word). */
template <વર્ગ SRC>
સ્થિર vvp_vector4_t uparr_or_શબ્દો_src(SRC* src, unsigned wid)
{
      જો (!src) પાછા vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_0);
      માટે (size_t i = 0; i < src->get_size(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    res |= vi;
      }
      પાછા res;
}

તાર્કિક of_UPARR_OR_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      જો (qsrc) res = uparr_or_શબ્દો_src(qsrc, wid);
      નહીંતર જો (dsrc) res = uparr_or_શબ્દો_src(dsrc, wid);
      નહીંતર {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    res = uparr_or_શબ્દો_src(src, wid);
      }
      thr->ધકેલો_vec4(res);
      પાછા સાચું;
}

/* real product() reduction returns the scalar sum */
template <વર્ગ SRC>
સ્થિર દ્વિગુણ uparr_product_શબ્દો_src_real(SRC* src)
{
      જો (!src) પાછા 0.0;
      દ્વિગુણ acc = 1.0;
      માટે (size_t i = 0; i < src->get_size(); i += 1) {
	    દ્વિગુણ vi;
	    src->get_word(i, vi);
	    acc *= vi;
      }
      પાછા acc;
}

તાર્કિક of_UPARR_PRODUCT_REAL(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_queue_real* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_real_from_net(thr, net, qsrc, dsrc);
      દ્વિગુણ sum;
      જો (qsrc) sum = uparr_product_શબ્દો_src_real(qsrc);
      નહીંતર જો (dsrc) sum = uparr_product_શબ્દો_src_real(dsrc);
      નહીંતર {
	    vvp_queue* src_q = get_queue_object<vvp_queue_real>(thr, net);
	    vvp_queue_real* src = dynamic_cast<vvp_queue_real*>(src_q);
	    sum = uparr_product_શબ્દો_src_real(src);
      }
      thr->ધકેલો_real(sum);
      પાછા સાચું;
}

/* product() reduction returns the scalar product (Verilog-style multiplication per word). */
template <વર્ગ SRC>
સ્થિર vvp_vector4_t uparr_product_શબ્દો_src(SRC* src, unsigned wid)
{
      જો (!src) પાછા vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t acc = queue_unique_ulong_to_vec4(1UL, wid);
      માટે (size_t i = 0; i < src->get_size(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    acc.mul(vi);
      }
      પાછા acc;
}

તાર્કિક of_UPARR_PRODUCT_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t prod;
      જો (qsrc) prod = uparr_product_શબ્દો_src(qsrc, wid);
      નહીંતર જો (dsrc) prod = uparr_product_શબ્દો_src(dsrc, wid);
      નહીંતર {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    prod = uparr_product_શબ્દો_src(src, wid);
      }
      thr->ધકેલો_vec4(prod);
      પાછા સાચું;
}

તાર્કિક of_UPARR_PRODUCT_PROP_V(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->સંખ્યા;
      unsigned wid = cp->bit_idx[0];

      vvp_object_t& ટોચ = thr->peek_object();
      vvp_cobject*cobj = ટોચ.peek<vvp_cobject>();
      પ્રતિજ્ઞા(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t prod =
	    qsrc ? uparr_product_શબ્દો_src(qsrc, wid)
		 : uparr_product_શબ્દો_src(dsrc, wid);
      thr->ધકેલો_vec4(prod);
      પાછા સાચું;
}

તાર્કિક of_UPARR_PRODUCT_OBJ_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->કાઢો_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_vector4_t prod =
	    qsrc ? uparr_product_શબ્દો_src(qsrc, wid)
		 : uparr_product_શબ્દો_src(dsrc, wid);
      thr->ધકેલો_vec4(prod);
      પાછા સાચું;
}

/* real sum() reduction returns the scalar sum */
template <વર્ગ SRC>
સ્થિર દ્વિગુણ uparr_sum_શબ્દો_src_real(SRC* src)
{
      દ્વિગુણ acc = 0.0;
      જો (!src) પાછા acc;
      માટે (size_t i = 0; i < src->get_size(); i += 1) {
	    દ્વિગુણ vi;
	    src->get_word(i, vi);
	    acc += vi;
      }
      પાછા acc;
}

તાર્કિક of_UPARR_SUM_REAL(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_queue_real* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_real_from_net(thr, net, qsrc, dsrc);
      દ્વિગુણ sum;
      જો (qsrc) sum = uparr_sum_શબ્દો_src_real(qsrc);
      નહીંતર જો (dsrc) sum = uparr_sum_શબ્દો_src_real(dsrc);
      નહીંતર {
	    vvp_queue* src_q = get_queue_object<vvp_queue_real>(thr, net);
	    vvp_queue_real* src = dynamic_cast<vvp_queue_real*>(src_q);
	    sum = uparr_sum_શબ્દો_src_real(src);
      }
      thr->ધકેલો_real(sum);
      પાછા સાચું;
}

/* sum() reduction returns the scalar sum (Verilog-style addition per word). */
template <વર્ગ SRC>
સ્થિર vvp_vector4_t uparr_sum_શબ્દો_src(SRC* src, unsigned wid)
{
      જો (!src) પાછા vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t acc(wid, BIT4_0);
      માટે (size_t i = 0; i < src->get_size(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    acc.add(vi);
      }
      પાછા acc;
}

તાર્કિક of_UPARR_SUM_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t sum;
      જો (qsrc) sum = uparr_sum_શબ્દો_src(qsrc, wid);
      નહીંતર જો (dsrc) sum = uparr_sum_શબ્દો_src(dsrc, wid);
      નહીંતર {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    sum = uparr_sum_શબ્દો_src(src, wid);
      }
      thr->ધકેલો_vec4(sum);
      પાછા સાચું;
}

તાર્કિક of_UPARR_SUM_PROP_V(vthread_t thr, vvp_code_t cp)
{
      size_t pid = cp->સંખ્યા;
      unsigned wid = cp->bit_idx[0];

      vvp_object_t& ટોચ = thr->peek_object();
      vvp_cobject*cobj = ટોચ.peek<vvp_cobject>();
      પ્રતિજ્ઞા(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t sum =
	    qsrc ? uparr_sum_શબ્દો_src(qsrc, wid)
		 : uparr_sum_શબ્દો_src(dsrc, wid);
      thr->ધકેલો_vec4(sum);
      પાછા સાચું;
}

તાર્કિક of_UPARR_SUM_OBJ_V(vthread_t thr, vvp_code_t cp)
{
      unsigned wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->કાઢો_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_vector4_t sum =
	    qsrc ? uparr_sum_શબ્દો_src(qsrc, wid)
		 : uparr_sum_શબ્દો_src(dsrc, wid);
      thr->ધકેલો_vec4(sum);
      પાછા સાચું;
}

/* xor() reduction returns the scalar sum (Verilog-style bitwise-XOR per word). */
template <વર્ગ SRC>
સ્થિર vvp_vector4_t uparr_xor_શબ્દો_src(SRC* src, unsigned wid)
{
      જો (!src) પાછા vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_0);
      માટે (size_t i = 0; i < src->get_size(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    res ^= vi;
      }
      પાછા res;
}

તાર્કિક of_UPARR_XOR_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      unsigned wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      જો (qsrc) res = uparr_xor_શબ્દો_src(qsrc, wid);
      નહીંતર જો (dsrc) res = uparr_xor_શબ્દો_src(dsrc, wid);
      નહીંતર {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    res = uparr_xor_શબ્દો_src(src, wid);
      }
      thr->ધકેલો_vec4(res);
      પાછા સાચું;
}

તાર્કિક of_VPI_CALL(vthread_t thr, vvp_code_t cp)
{
      vpip_execute_vpi_call(thr, cp->handle);

      જો (schedule_stopped()) {
	    જો (! schedule_finished())
		  schedule_vthread(thr, 0, ખોટું);

	    પાછા ખોટું;
      }

      પાછા schedule_finished()? ખોટું : સાચું;
}

/* %wait <label>;
 * Implement the wait by locating the vvp_net_T for the event, and
 * adding this thread to the threads list for the event. The some
 * argument is the  reference to the functor to wait for. This must be
 * an event object of some sort.
 */
તાર્કિક of_WAIT(vthread_t thr, vvp_code_t cp)
{
      પ્રતિજ્ઞા(! thr->i_am_in_function);
      પ્રતિજ્ઞા(! thr->waiting_for_event);
      thr->waiting_for_event = 1;

	/* Add this thread to the list in the event. */
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (cp->net->fun);
      પ્રતિજ્ઞા(ep);
      thr->wait_next = ep->add_waiting_thread(thr);

	/* Return false to suspend this thread. */
      પાછા ખોટું;
}

/*
 * Implement the %wait/fork (SystemVerilog) instruction by suspending
 * the current thread until all the detached children have finished.
 */
તાર્કિક of_WAIT_FORK(vthread_t thr, vvp_code_t)
{
	/* If a %wait/fork is being executed then the parent thread
	 * cannot be waiting in a join or already waiting. */
      પ્રતિજ્ઞા(! thr->i_am_in_function);
      પ્રતિજ્ઞા(! thr->i_am_joining);
      પ્રતિજ્ઞા(! thr->i_am_waiting);

	/* There should be no active children when waiting. */
      પ્રતિજ્ઞા(thr->children.empty());

	/* If there are no detached children then there is nothing to
	 * wait for. */
      જો (thr->detached_children.empty()) પાછા સાચું;

	/* Flag that this process is waiting for the detached children
	 * to finish and suspend it. */
      thr->i_am_waiting = 1;
      પાછા ખોટું;
}

/*
 * %xnor
 */
તાર્કિક of_XNOR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valr = thr->કાઢો_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      પ્રતિજ્ઞા(vall.size() == valr.size());

      vall ^= valr;
      vall.invert();

      પાછા સાચું;
}

/*
 * %xor
 */
તાર્કિક of_XOR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valr = thr->કાઢો_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      પ્રતિજ્ઞા(vall.size() == valr.size());

      vall ^= valr;

      પાછા સાચું;
}


તાર્કિક of_ZOMBIE(vthread_t thr, vvp_code_t)
{
      thr->pc = codespace_null();
      જો ((thr->parent == 0) && (thr->children.empty())) {
	    જો (thr->delay_delete)
		  schedule_del_thr(thr);
	    નહીંતર
		  vthread_delete(thr);
      }
      પાછા ખોટું;
}

/*
 * This is a phantom opcode used to call user defined functions. It
 * is used in code generated by the .ufunc statement. It contains a
 * pointer to the executable code of the function and a pointer to
 * a ufunc_core object that has all the port information about the
 * function.
 */
સ્થિર તાર્કિક do_exec_ufunc(vthread_t thr, vvp_code_t cp, vthread_t child)
{
      __vpiScope*child_scope = cp->ufunc_core_ptr->func_scope();
      પ્રતિજ્ઞા(child_scope);

      પ્રતિજ્ઞા(child_scope->get_type_code() == vpiFunction);
      પ્રતિજ્ઞા(thr->children.empty());


        /* We can take a number of shortcuts because we know that a
           continuous assignment can only occur in a static scope. */
      પ્રતિજ્ઞા(thr->wt_context == 0);
      પ્રતિજ્ઞા(thr->rd_context == 0);

        /* If an automatic function, allocate a context for this call. */
      vvp_context_t child_context = 0;
      જો (child_scope->is_automatic()) {
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
      પ્રતિજ્ઞા(thr->children.size()==1);

      child->is_scheduled = 1;
      child->i_am_in_function = 1;
      vthread_run(child);
      running_thread = thr;

      જો (child->i_have_ended) {
	    do_join(thr, child);
            પાછા સાચું;
      } નહીંતર {
	    thr->i_am_joining = 1;
	    પાછા ખોટું;
      }
}

તાર્કિક of_EXEC_UFUNC_REAL(vthread_t thr, vvp_code_t cp)
{
      __vpiScope*child_scope = cp->ufunc_core_ptr->func_scope();
      પ્રતિજ્ઞા(child_scope);

	/* Create a temporary thread and run it immediately. */
      vthread_t child = vthread_new(cp->cptr, child_scope);
      thr->ધકેલો_real(0.0);
      child->args_real.push_back(0);

      પાછા do_exec_ufunc(thr, cp, child);
}

તાર્કિક of_EXEC_UFUNC_VEC4(vthread_t thr, vvp_code_t cp)
{
      __vpiScope*child_scope = cp->ufunc_core_ptr->func_scope();
      પ્રતિજ્ઞા(child_scope);

      vpiScopeFunction*scope_func = dynamic_cast<vpiScopeFunction*>(child_scope);
      પ્રતિજ્ઞા(scope_func);

	/* Create a temporary thread and run it immediately. */
      vthread_t child = vthread_new(cp->cptr, child_scope);
      thr->ધકેલો_vec4(vvp_vector4_t(scope_func->get_func_width(), scope_func->get_func_init_val()));
      child->args_vec4.push_back(0);

      પાછા do_exec_ufunc(thr, cp, child);
}

/*
 * This is a phantom opcode used to harvest the result of calling a user
 * defined function. It is used in code generated by the .ufunc statement.
 */
તાર્કિક of_REAP_UFUNC(vthread_t thr, vvp_code_t cp)
{
      __vpiScope*child_scope = cp->ufunc_core_ptr->func_scope();
      પ્રતિજ્ઞા(child_scope);

	/* Copy the output from the result variable to the output
	   ports of the .ufunc device. */
      cp->ufunc_core_ptr->finish_thread();

        /* If an automatic function, free the context for this call. */
      જો (child_scope->is_automatic()) {
            vthread_free_context(thr->rd_context, child_scope);
            thr->wt_context = 0;
            thr->rd_context = 0;
      }

      પાછા સાચું;
}
