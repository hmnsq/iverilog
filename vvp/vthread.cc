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

using नामाकाश std;

/* This is the size of an unsigned long in bits. This is just a
   convenience macro. */
# define CPU_WORD_BITS (8*आकार(अचिन्हित दीर्घ))
# define शीर्ष_BIT (1UL << (CPU_WORD_BITS-1))

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

संरचना vthread_s {
      vthread_s();

      शून्य debug_dump(ostream&fd, स्थिरांक अक्षर*label_text);

	/* This is the program counter. */
      vvp_code_t pc;
	/* These hold the private thread bits. */
      गणना { FLAGS_मोजणी = 512, शब्द_मोजणी = 16 };
      vvp_bit4_t flags[FLAGS_मोजणी];

	/* These are the word registers. */
      संघ {
	    int64_t  w_int;
	    uint64_t w_uint;
      } शब्द[शब्द_मोजणी];

	// These vectors are depths within the parent thread's
	// corresponding stack.  This is how the %ret/* instructions
	// get at parent thread arguments.
      vector<अचिन्हित> args_real;
      vector<अचिन्हित> args_str;
      vector<अचिन्हित> args_vec4;

    खाजगी:
      vector<vvp_vector4_t>चळत_vec4;
    सार्वजनिक:
      ओळीत vvp_vector4_t काढा_vec4(शून्य)
      {
	    प्रतिज्ञा(! चळत_vec4.empty());
	    vvp_vector4_t val = चळत_vec4.back();
	    चळत_vec4.pop_back();
	    परत val;
      }
      ओळीत शून्य ढकला_vec4(स्थिरांक vvp_vector4_t&val)
      {
	    चळत_vec4.push_back(val);
      }
      ओळीत स्थिरांक vvp_vector4_t& peek_vec4(अचिन्हित depth)
      {
	    अचिन्हित size = चळत_vec4.size();
	    प्रतिज्ञा(depth < size);
	    अचिन्हित use_index = size-1-depth;
	    परत चळत_vec4[use_index];
      }
      ओळीत vvp_vector4_t& peek_vec4(शून्य)
      {
	    अचिन्हित use_index = चळत_vec4.size();
	    प्रतिज्ञा(use_index >= 1);
	    परत चळत_vec4[use_index-1];
      }
      ओळीत शून्य poke_vec4(अचिन्हित depth, स्थिरांक vvp_vector4_t&val)
      {
	    प्रतिज्ञा(depth < चळत_vec4.size());
	    अचिन्हित use_index = चळत_vec4.size()-1-depth;
	    चळत_vec4[use_index] = val;
      }
      ओळीत शून्य काढा_vec4(अचिन्हित cnt)
      {
	    पर्यंत (cnt > 0) {
		  चळत_vec4.pop_back();
		  cnt -= 1;
	    }
      }


    खाजगी:
      vector<द्विगुण> चळत_real;
    सार्वजनिक:
      ओळीत द्विगुण काढा_real(शून्य)
      {
	    प्रतिज्ञा(! चळत_real.empty());
	    द्विगुण val = चळत_real.back();
	    चळत_real.pop_back();
	    परत val;
      }
      ओळीत शून्य ढकला_real(द्विगुण val)
      {
	    चळत_real.push_back(val);
      }
      ओळीत द्विगुण peek_real(अचिन्हित depth)
      {
	    प्रतिज्ञा(depth < चळत_real.size());
	    अचिन्हित use_index = चळत_real.size()-1-depth;
	    परत चळत_real[use_index];
      }
      ओळीत शून्य poke_real(अचिन्हित depth, द्विगुण val)
      {
	    प्रतिज्ञा(depth < चळत_real.size());
	    अचिन्हित use_index = चळत_real.size()-1-depth;
	    चळत_real[use_index] = val;
      }
      ओळीत शून्य काढा_real(अचिन्हित cnt)
      {
	    पर्यंत (cnt > 0) {
		  चळत_real.pop_back();
		  cnt -= 1;
	    }
      }

	/* Strings are operated on using a forth-like operator
	   set. Items at the top of the stack (back()) are the objects
	   operated on except for special cases. New objects are
	   pushed onto the top (back()) and pulled from the top
	   (back()) only. */
    खाजगी:
      vector<string> चळत_str;
    सार्वजनिक:
      ओळीत string काढा_str(शून्य)
      {
	    प्रतिज्ञा(! चळत_str.empty());
	    string val = चळत_str.back();
	    चळत_str.pop_back();
	    परत val;
      }
      ओळीत शून्य ढकला_str(स्थिरांक string&val)
      {
	    चळत_str.push_back(val);
      }
      ओळीत string&peek_str(अचिन्हित depth)
      {
	    प्रतिज्ञा(depth<चळत_str.size());
	    अचिन्हित use_index = चळत_str.size()-1-depth;
	    परत चळत_str[use_index];
      }
      ओळीत शून्य poke_str(अचिन्हित depth, स्थिरांक string&val)
      {
	    प्रतिज्ञा(depth < चळत_str.size());
	    अचिन्हित use_index = चळत_str.size()-1-depth;
	    चळत_str[use_index] = val;
      }
      ओळीत शून्य काढा_str(अचिन्हित cnt)
      {
	    पर्यंत (cnt > 0) {
		  चळत_str.pop_back();
		  cnt -= 1;
	    }
      }

	/* Objects are also operated on in a stack. */
    खाजगी:
      गणना { चळत_OBJ_MAX_SIZE = 32 };
      vvp_object_t चळत_obj[चळत_OBJ_MAX_SIZE];
      अचिन्हित चळत_obj_size;
    सार्वजनिक:
      ओळीत vvp_object_t& peek_object(शून्य)
      {
	    प्रतिज्ञा(चळत_obj_size > 0);
	    परत चळत_obj[चळत_obj_size-1];
      }
      ओळीत शून्य काढा_object(vvp_object_t&obj)
      {
	    प्रतिज्ञा(चळत_obj_size > 0);
	    चळत_obj_size -= 1;
	    obj = चळत_obj[चळत_obj_size];
	    चळत_obj[चळत_obj_size].reset(0);
      }
      ओळीत शून्य काढा_object(अचिन्हित cnt, अचिन्हित skip =0)
      {
	    प्रतिज्ञा((cnt+skip) <= चळत_obj_size);
	    साठी (माप_प्रकार idx = चळत_obj_size-skip-cnt ; idx < चळत_obj_size-skip ; idx += 1)
		  चळत_obj[idx].reset(0);
	    चळत_obj_size -= cnt;
	    साठी (माप_प्रकार idx = चळत_obj_size-skip ; idx < चळत_obj_size ; idx += 1)
		  चळत_obj[idx] = चळत_obj[idx+skip];
	    साठी (माप_प्रकार idx = चळत_obj_size ; idx < चळत_obj_size+skip ; idx += 1)
		  चळत_obj[idx].reset(0);
      }
      ओळीत शून्य ढकला_object(स्थिरांक vvp_object_t&obj)
      {
	    प्रतिज्ञा(चळत_obj_size < चळत_OBJ_MAX_SIZE);
	    चळत_obj[चळत_obj_size] = obj;
	    चळत_obj_size += 1;
      }

	/* My parent sets this when it wants me to wake it up. */
      अचिन्हित i_am_joining      :1;
      अचिन्हित i_am_detached     :1;
      अचिन्हित i_am_waiting      :1;
      अचिन्हित i_am_in_function  :1; // True if running function code
      अचिन्हित i_have_ended      :1;
      अचिन्हित i_was_disabled    :1;
      अचिन्हित waiting_for_event :1;
      अचिन्हित is_scheduled      :1;
      अचिन्हित delay_delete      :1;
	/* This points to the children of the thread. */
      set<संरचना vthread_s*>children;
	/* This points to the detached children of the thread. */
      set<संरचना vthread_s*>detached_children;
	/* This points to my parent, if I have one. */
      संरचना vthread_s*parent;
	/* This points to the containing scope. */
      __vpiScope*parent_scope;
	/* This is used for keeping wait queues. */
      संरचना vthread_s*wait_next;
	/* These are used to access automatically allocated items. */
      vvp_context_t wt_context, rd_context;
	/* These are used to pass non-blocking event control information. */
      vvp_net_t*event;
      uint64_t ecount;
	/* Save the file/line information when available. */
    खाजगी:
      अक्षर *filenm_;
      अचिन्हित lineno_;
    सार्वजनिक:
      शून्य set_fileline(अक्षर *filenm, अचिन्हित lineno);
      string get_fileline();

      ओळीत शून्य cleanup()
      {
	    जर (i_was_disabled) {
		  चळत_vec4.clear();
		  चळत_real.clear();
		  चळत_str.clear();
		  काढा_object(चळत_obj_size);
	    }
	    मुक्त(filenm_);
	    filenm_ = 0;
	    प्रतिज्ञा(चळत_vec4.empty());
	    प्रतिज्ञा(चळत_real.empty());
	    प्रतिज्ञा(चळत_str.empty());
	    प्रतिज्ञा(चळत_obj_size == 0);
      }
};

ओळीत vthread_s::vthread_s()
{
      चळत_obj_size = 0;
      filenm_ = 0;
      lineno_ = 0;
}

शून्य vthread_s::set_fileline(अक्षर *filenm, अचिन्हित lineno)
{
      प्रतिज्ञा(filenm);
      जर (!filenm_ || (म_भेद(filenm_, filenm) != 0)) {
	    मुक्त(filenm_);
	    filenm_ = strdup(filenm);
      }
      lineno_ = lineno;
}

ओळीत string vthread_s::get_fileline()
{
      ostringstream buf;
      जर (filenm_) {
	    buf << filenm_ << ":" << lineno_ << ": ";
      }
      string res = buf.str();
      परत res;
}

शून्य vthread_s::debug_dump(ostream&fd, स्थिरांक अक्षर*label)
{
      fd << "**** " << label << endl;
      fd << "**** ThreadId: " << हे << ", parent id: " << parent << endl;

      fd << "**** Flags: ";
      साठी (पूर्णांक idx = 0 ; idx < FLAGS_मोजणी ; idx += 1)
	    fd << flags[idx];
      fd << endl;
      fd << "**** vec4 stack..." << endl;
      साठी (माप_प्रकार idx = चळत_vec4.size() ; idx > 0 ; idx -= 1)
	    fd << "    " << (चळत_vec4.size()-idx) << ": " << चळत_vec4[idx-1] << endl;
      fd << "**** str stack (" << चळत_str.size() << ")..." << endl;
      fd << "**** obj stack (" << चळत_obj_size << ")..." << endl;
      fd << "**** args_vec4 array (" << args_vec4.size() << ")..." << endl;
      साठी (माप_प्रकार idx = 0 ; idx < args_vec4.size() ; idx += 1)
	    fd << "    " << idx << ": " << args_vec4[idx] << endl;
      fd << "**** file/line (";
      जर (filenm_) fd << filenm_;
      अन्यथा fd << "<no file name>";
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
स्थिर string filter_string(स्थिरांक अक्षर*text)
{
      vector<अक्षर> tmp (म_माप(text)+1);
      माप_प्रकार dst = 0;
      साठी (स्थिरांक अक्षर*ptr = text ; *ptr ; ptr += 1) {
	    // Not an escape? Move on.
	    जर (*ptr != '\\') {
		  tmp[dst++] = *ptr;
		  सुरूठेवा;
	    }

	    // Now we know that *ptr is pointing to a \ character and we
	    // have an octal sequence coming up. Advance the ptr and start
	    // processing octal digits.
	    ptr += 1;
	    जर (*ptr == 0)
		  थांबा;

	    अक्षर byte = 0;
	    पूर्णांक cnt = 3;
	    पर्यंत (*ptr && cnt > 0 && *ptr >= '0' && *ptr <= '7') {
		  byte *= 8;
		  byte += *ptr - '0';
		  cnt -= 1;
		  ptr += 1;
	    }

	    // null-bytes are supposed to be removed when assigning a string
	    // literal to a string.
	    जर (byte != '\0')
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
      परत res;
}

स्थिर शून्य do_join(vthread_t thr, vthread_t child);

__vpiScope* vthread_scope(संरचना vthread_s*thr)
{
      परत thr->parent_scope;
}

संरचना vthread_s*running_thread = 0;

string get_fileline()
{
      परत running_thread->get_fileline();
}

शून्य vthread_ढकला(संरचना vthread_s*thr, द्विगुण val)
{
      thr->ढकला_real(val);
}

शून्य vthread_ढकला(संरचना vthread_s*thr, स्थिरांक string&val)
{
      thr->ढकला_str(val);
}

शून्य vthread_ढकला(संरचना vthread_s*thr, स्थिरांक vvp_vector4_t&val)
{
      thr->ढकला_vec4(val);
}

शून्य vthread_काढा_real(संरचना vthread_s*thr, अचिन्हित depth)
{
      thr->काढा_real(depth);
}

शून्य vthread_काढा_str(संरचना vthread_s*thr, अचिन्हित depth)
{
      thr->काढा_str(depth);
}

शून्य vthread_काढा_vec4(संरचना vthread_s*thr, अचिन्हित depth)
{
      thr->काढा_vec4(depth);
}

द्विगुण vthread_get_real_चळत(संरचना vthread_s*thr, अचिन्हित depth)
{
      परत thr->peek_real(depth);
}

स्थिरांक string&vthread_get_str_चळत(संरचना vthread_s*thr, अचिन्हित depth)
{
      परत thr->peek_str(depth);
}

स्थिरांक vvp_vector4_t& vthread_get_vec4_चळत(संरचना vthread_s*thr, अचिन्हित depth)
{
      परत thr->peek_vec4(depth);
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
ढाँचा <वर्ग VVP_QUEUE> स्थिर vvp_queue*get_queue_object(vthread_t thr, vvp_net_t*net)
{
      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      प्रतिज्ञा(obj);

      vvp_queue*queue = obj->get_object().peek<vvp_queue>();
      जर (queue == 0) {
	    प्रतिज्ञा(obj->get_object().test_nil());
	    queue = नवीन VVP_QUEUE;
	    vvp_object_t val (queue);
	    vvp_net_ptr_t ptr (net, 0);
	    vvp_send_object(ptr, val, thr->wt_context);
      }

      परत queue;
}

/*
 * Locator and size/word opcodes use the same VVP net for a queue ($) or a
 * dynamic array ([]). Queues are vvp_queue_vec4; dynamic arrays may be
 * vvp_darray_vec4, vvp_darray_atom<>, etc. (all vvp_darray, but vvp_queue
 * also derives from vvp_darray — test queue vec4 first).
 */
स्थिर शून्य get_queue_or_darray_vec4_from_net(vthread_t thr, vvp_net_t* net,
					      vvp_queue_vec4*& qsrc,
					      vvp_darray*& dsrc)
{
      vvp_fun_signal_object* obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      प्रतिज्ञा(obj);
      vvp_object_t शीर्ष = obj->get_object();
      qsrc = शीर्ष.peek<vvp_queue_vec4>();
      जर (qsrc) {
	    dsrc = 0;
	    परत;
      }
      vvp_darray* dany = शीर्ष.peek<vvp_darray>();
      जर (dany && dynamic_cast<vvp_queue*>(dany) == 0) {
	    dsrc = dany;
	    परत;
      }
      जर (शीर्ष.test_nil()) {
	    qsrc = नवीन vvp_queue_vec4();
	    vvp_object_t val(qsrc);
	    vvp_net_ptr_t ptr(net, 0);
	    vvp_send_object(ptr, val, thr->wt_context);
	    dsrc = 0;
	    परत;
      }
      /* e.g. real/string queue — not vec4 queue or plain darray */
      qsrc = 0;
      dsrc = 0;
}

स्थिर शून्य get_queue_or_darray_real_from_net(vthread_t thr, vvp_net_t* net,
					      vvp_queue_real*& qsrc,
					      vvp_darray*& dsrc)
{
      vvp_fun_signal_object* obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      प्रतिज्ञा(obj);
      vvp_object_t शीर्ष = obj->get_object();
      qsrc = शीर्ष.peek<vvp_queue_real>();
      जर (qsrc) {
	    dsrc = 0;
	    परत;
      }
      vvp_darray* dany = शीर्ष.peek<vvp_darray>();
      जर (dany && dynamic_cast<vvp_queue*>(dany) == 0) {
	    dsrc = dany;
	    परत;
      }
      जर (शीर्ष.test_nil()) {
	    qsrc = नवीन vvp_queue_real();
	    vvp_object_t val(qsrc);
	    vvp_net_ptr_t ptr(net, 0);
	    vvp_send_object(ptr, val, thr->wt_context);
	    dsrc = 0;
	    परत;
      }
      /* e.g. real/string queue — not vec4 queue or plain darray */
      qsrc = 0;
      dsrc = 0;
}

स्थिर शून्य get_queue_or_darray_vec4_from_object(vvp_object_t src_obj,
						 vvp_queue_vec4*& qsrc,
						 vvp_darray*& dsrc)
{
      qsrc = src_obj.peek<vvp_queue_vec4>();
      जर (qsrc) {
	    dsrc = 0;
	    परत;
      }
      vvp_darray* dany = src_obj.peek<vvp_darray>();
      जर (dany && dynamic_cast<vvp_queue*>(dany) == 0) {
	    dsrc = dany;
	    परत;
      }
      qsrc = 0;
      dsrc = 0;
}

/*
 * The following are used to allow a common template to be written for
 * queue real/string/vec4 operations
 */
ओळीत स्थिर शून्य काढा_मूल्य(vthread_t thr, द्विगुण&value, अचिन्हित)
{
      value = thr->काढा_real();
}

ओळीत स्थिर शून्य काढा_मूल्य(vthread_t thr, string&value, अचिन्हित)
{
      value = thr->काढा_str();
}

ओळीत स्थिर शून्य काढा_मूल्य(vthread_t thr, vvp_vector4_t&value, अचिन्हित wid)
{
      value = thr->काढा_vec4();
      प्रतिज्ञा(value.size() == wid);
}

/*
 * The following are used to allow the queue templates to print correctly.
 */
ओळीत स्थिर string get_queue_type(द्विगुण&)
{
      परत "queue<real>";
}

ओळीत स्थिर string get_queue_type(string&)
{
      परत "queue<string>";
}

ओळीत स्थिर string get_queue_type(स्थिरांक vvp_vector4_t&value)
{
      ostringstream buf;
      buf << "queue<vector[" << value.size() << "]>";
      string res = buf.str();
      परत res;
}

ओळीत स्थिर शून्य print_queue_मूल्य(द्विगुण value)
{
      cerr << value;
}

ओळीत स्थिर शून्य print_queue_मूल्य(स्थिरांक string&value)
{
      cerr << "\"" << value << "\"";
}

ओळीत स्थिर शून्य print_queue_मूल्य(स्थिरांक vvp_vector4_t&value)
{
      cerr << value;
}

/*
 * The following are used to get a darray/queue default value.
 */
ओळीत स्थिर शून्य dq_default(द्विगुण&value, अचिन्हित)
{
      value = 0.0;
}

ओळीत स्थिर शून्य dq_default(string&value, अचिन्हित)
{
      value = "";
}

ओळीत स्थिर शून्य dq_default(vvp_vector4_t&value, अचिन्हित wid)
{
      value = vvp_vector4_t(wid);
}


ढाँचा <वर्ग T> T coerce_to_width(स्थिरांक T&that, अचिन्हित width)
{
      जर (that.size() == width)
	    परत that;

      प्रतिज्ञा(that.size() > width);
      T res (width);
      साठी (अचिन्हित idx = 0 ;  idx < width ;  idx += 1)
	    res.set_bit(idx, that.value(idx));

      परत res;
}

/* Explicitly define the vvp_vector4_t version of coerce_to_width(). */
ढाँचा vvp_vector4_t coerce_to_width(स्थिरांक vvp_vector4_t&that,
                                       अचिन्हित width);


स्थिर शून्य multiply_array_imm(अचिन्हित दीर्घ*res, स्थिरांक अचिन्हित दीर्घ*val,
			       अचिन्हित शब्द, अचिन्हित दीर्घ imm)
{
      साठी (अचिन्हित idx = 0 ; idx < शब्द ; idx += 1)
	    res[idx] = 0;

      साठी (अचिन्हित mul_idx = 0 ; mul_idx < शब्द ; mul_idx += 1) {
	    अचिन्हित दीर्घ sum;
	    अचिन्हित दीर्घ tmp = multiply_with_carry(val[mul_idx], imm, sum);

	    अचिन्हित दीर्घ carry = 0;
	    res[mul_idx] = add_with_carry(res[mul_idx], tmp, carry);
	    साठी (अचिन्हित add_idx = mul_idx+1 ; add_idx < शब्द ; add_idx += 1) {
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
स्थिर vvp_context_t vthread_alloc_context(__vpiScope*scope)
{
      प्रतिज्ञा(scope->is_automatic());

      vvp_context_t context = scope->free_contexts;
      जर (context) {
            scope->free_contexts = vvp_get_next_context(context);
            साठी (अचिन्हित idx = 0 ; idx < scope->nitem ; idx += 1) {
                  scope->item[idx]->reset_instance(context);
            }
      } अन्यथा {
            context = vvp_allocate_context(scope->nitem);
            साठी (अचिन्हित idx = 0 ; idx < scope->nitem ; idx += 1) {
                  scope->item[idx]->alloc_instance(context);
            }
      }

      vvp_set_next_context(context, scope->live_contexts);
      scope->live_contexts = context;

      परत context;
}

/*
 * Free a context previously allocated to a child thread by pushing it
 * onto the freed context stack. Remove it from the list of live contexts
 * in that scope.
 */
स्थिर शून्य vthread_free_context(vvp_context_t context, __vpiScope*scope)
{
      प्रतिज्ञा(scope->is_automatic());
      प्रतिज्ञा(context);

      जर (context == scope->live_contexts) {
            scope->live_contexts = vvp_get_next_context(context);
      } अन्यथा {
            vvp_context_t tmp = scope->live_contexts;
            पर्यंत (context != vvp_get_next_context(tmp)) {
                  प्रतिज्ञा(tmp);
                  tmp = vvp_get_next_context(tmp);
            }
            vvp_set_next_context(tmp, vvp_get_next_context(context));
      }

      vvp_set_next_context(context, scope->free_contexts);
      scope->free_contexts = context;
}

#ifdef CHECK_WITH_VALGRIND
शून्य contexts_delete(वर्ग __vpiScope*scope)
{
      vvp_context_t context = scope->free_contexts;

      पर्यंत (context) {
	    scope->free_contexts = vvp_get_next_context(context);
	    साठी (अचिन्हित idx = 0; idx < scope->nitem; idx += 1) {
		  scope->item[idx]->free_instance(context);
	    }
	    मुक्त(context);
	    context = scope->free_contexts;
      }
      मुक्त(scope->item);
}
#endif

/*
 * Create a new thread with the given start address.
 */
vthread_t vthread_new(vvp_code_t pc, __vpiScope*scope)
{
      vthread_t thr = नवीन संरचना vthread_s;
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
      साठी (पूर्णांक idx = 4 ; idx < 8 ; idx += 1)
	    thr->flags[idx] = BIT4_X;

      scope->threads .insert(thr);
      परत thr;
}

#ifdef CHECK_WITH_VALGRIND
#जर 0
/*
 * These are not currently correct. If you use them you will get
 * double delete messages. There is still a leak related to a
 * waiting event that needs to be investigated.
 */

स्थिर शून्य wait_next_delete(vthread_t base)
{
      पर्यंत (base) {
	    vthread_t tmp = base->wait_next;
	    delete base;
	    base = tmp;
	    जर (base->waiting_for_event == 0) थांबा;
      }
}

स्थिर शून्य child_delete(vthread_t base)
{
      पर्यंत (base) {
	    vthread_t tmp = base->child;
	    delete base;
	    base = tmp;
      }
}
#endif

शून्य vthreads_delete(वर्ग __vpiScope*scope)
{
      साठी (std::set<vthread_t>::iterator cur = scope->threads.begin()
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
स्थिर शून्य vthread_reap(vthread_t thr)
{
      जर (! thr->children.empty()) {
	    साठी (set<vthread_t>::iterator cur = thr->children.begin()
		       ; cur != thr->children.end() ; ++cur) {
		  vthread_t child = *cur;
		  प्रतिज्ञा(child);
		  प्रतिज्ञा(child->parent == thr);
		  child->parent = thr->parent;
	    }
      }
      जर (! thr->detached_children.empty()) {
	    साठी (set<vthread_t>::iterator cur = thr->detached_children.begin()
		       ; cur != thr->detached_children.end() ; ++cur) {
		  vthread_t child = *cur;
		  प्रतिज्ञा(child);
		  प्रतिज्ञा(child->parent == thr);
		  प्रतिज्ञा(child->i_am_detached);
		  child->parent = 0;
		  child->i_am_detached = 0;
	    }
      }
      जर (thr->parent) {
	      /* assert that the given element was removed. */
	    जर (thr->i_am_detached) {
		  माप_प्रकार res = thr->parent->detached_children.erase(thr);
		  प्रतिज्ञा(res == 1);
	    } अन्यथा {
		  माप_प्रकार res = thr->parent->children.erase(thr);
		  प्रतिज्ञा(res == 1);
	    }
      }

      thr->parent = 0;

	// Remove myself from the containing scope if needed.
      thr->parent_scope->threads.erase(thr);

      thr->pc = codespace_null();

	/* If this thread is not scheduled, then is it safe to delete
	   it now. Otherwise, let the schedule event (which will
	   execute the thread at of_ZOMBIE) delete the object. */
      जर ((thr->is_scheduled == 0) && (thr->waiting_for_event == 0)) {
	    प्रतिज्ञा(thr->children.empty());
	    प्रतिज्ञा(thr->wait_next == 0);
	    जर (thr->delay_delete)
		  schedule_del_thr(thr);
	    अन्यथा
		  vthread_delete(thr);
      }
}

शून्य vthread_delete(vthread_t thr)
{
      thr->cleanup();
      delete thr;
}

शून्य vthread_mark_scheduled(vthread_t thr)
{
      पर्यंत (thr != 0) {
	    प्रतिज्ञा(thr->is_scheduled == 0);
	    thr->is_scheduled = 1;
	    thr = thr->wait_next;
      }
}

शून्य vthread_mark_final(vthread_t thr)
{
      /*
       * The behavior in a final thread is the same as in a function. Any
       * child thread will be executed immediately rather than being
       * scheduled.
       */
      पर्यंत (thr != 0) {
	    thr->i_am_in_function = 1;
	    thr = thr->wait_next;
      }
}

शून्य vthread_delay_delete()
{
      जर (running_thread)
	    running_thread->delay_delete = 1;
}

/*
 * This function runs each thread by fetching an instruction,
 * incrementing the PC, and executing the instruction. The thread may
 * be the head of a list, so each thread is run so far as possible.
 */
शून्य vthread_run(vthread_t thr)
{
      पर्यंत (thr != 0) {
	    vthread_t tmp = thr->wait_next;
	    thr->wait_next = 0;

	    प्रतिज्ञा(thr->is_scheduled);
	    thr->is_scheduled = 0;

            running_thread = thr;

	    साठी (;;) {
		  vvp_code_t cp = thr->pc;
		  thr->pc += 1;

		    /* Run the opcode implementation. If the execution of
		       the opcode returns false, then the thread is meant to
		       be paused, so break out of the loop. */
		  तार्किक rc = (cp->opcode)(thr, cp);
		  जर (rc == असत्य)
			थांबा;
	    }

	    thr = tmp;
      }
      running_thread = 0;
}

/*
 * The CHUNK_LINK instruction is a special next pointer for linking
 * chunks of code space. It's like a simplified %jmp.
 */
तार्किक of_CHUNK_LINK(vthread_t thr, vvp_code_t code)
{
      प्रतिज्ञा(code->cptr);
      thr->pc = code->cptr;
      परत सत्य;
}

/*
 * This is called by an event functor to wake up all the threads on
 * its list. I in fact created that list in the %wait instruction, and
 * I also am certain that the waiting_for_event flag is set.
 */
शून्य vthread_schedule_list(vthread_t thr)
{
      साठी (vthread_t cur = thr ;  cur ;  cur = cur->wait_next) {
	    प्रतिज्ञा(cur->waiting_for_event);
	    cur->waiting_for_event = 0;
      }

      schedule_vthread(thr, 0);
}

vvp_context_t vthread_get_wt_context()
{
      जर (running_thread)
            परत running_thread->wt_context;
      अन्यथा
            परत 0;
}

vvp_context_t vthread_get_rd_context()
{
      जर (running_thread)
            परत running_thread->rd_context;
      अन्यथा
            परत 0;
}

vvp_context_item_t vthread_get_wt_context_item(अचिन्हित context_idx)
{
      प्रतिज्ञा(running_thread && running_thread->wt_context);
      परत vvp_get_context_item(running_thread->wt_context, context_idx);
}

vvp_context_item_t vthread_get_rd_context_item(अचिन्हित context_idx)
{
      प्रतिज्ञा(running_thread && running_thread->rd_context);
      परत vvp_get_context_item(running_thread->rd_context, context_idx);
}

/*
 * %abs/wr
 */
तार्किक of_ABS_WR(vthread_t thr, vvp_code_t)
{
      thr->ढकला_real( भ_असल(thr->काढा_real()) );
      परत सत्य;
}

तार्किक of_ALLOC(vthread_t thr, vvp_code_t cp)
{
        /* Allocate a context. */
      vvp_context_t child_context = vthread_alloc_context(cp->scope);

        /* Push the allocated context onto the write context stack. */
      vvp_set_stacked_context(child_context, thr->wt_context);
      thr->wt_context = child_context;

      परत सत्य;
}

तार्किक of_AND(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valb = thr->काढा_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();
      प्रतिज्ञा(vala.size() == valb.size());
      vala &= valb;
      परत सत्य;
}

/*
 * This function must ALWAYS be called with the val set to the right
 * size, and initialized with BIT4_0 bits. Certain optimizations rely
 * on that.
 */
स्थिर शून्य get_immediate_rval(vvp_code_t cp, vvp_vector4_t&val)
{
      uint32_t vala = cp->bit_idx[0];
      uint32_t valb = cp->bit_idx[1];
      अचिन्हित wid  = cp->संख्या;

      जर (valb == 0) {
	      // Special case: if the value is zero, we are done
	      // before we start.
	    जर (vala == 0) परत;

	      // Special case: The value has no X/Z bits, so we can
	      // use the setarray method to write the value all at once.
	    अचिन्हित use_wid = 8*आकार(अचिन्हित दीर्घ);
	    जर (wid < use_wid)
		  use_wid = wid;
	    अचिन्हित दीर्घ tmp[1];
	    tmp[0] = vala;
	    val.setarray(0, use_wid, tmp);
	    परत;
      }

	// The immediate value can be values bigger then 32 bits, but
	// only if the high bits are zero. So at most we need to run
	// through the loop below 32 times. Maybe less, if the target
	// width is less. We don't have to do anything special on that
	// because vala/valb bits will shift away so (vala|valb) will
	// turn to zero at or before 32 shifts.

      साठी (अचिन्हित idx = 0 ; idx < wid && (vala|valb) ; idx += 1) {
	    uint32_t ba = 0;
	      // Convert the vala/valb bits to a ba number that
	      // matches the encoding of the vvp_bit4_t enumeration.
	    ba = (valb & 1) << 1;
	    ba |= vala & 1;

	      // Note that the val is already pre-filled with BIT4_0
	      // bits, os we only need to set non-zero bit values.
	    जर (ba) val.set_bit(idx, (vvp_bit4_t)ba);

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
तार्किक of_ADD(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t r = thr->काढा_vec4();
	// Rather then pop l, use it directly from the stack. When we
	// assign to 'l', that will edit the top of the stack, which
	// replaces a pop and a pull.
      vvp_vector4_t&l = thr->peek_vec4();

      l.add(r);

      परत सत्य;
}

/*
 * %addi <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
तार्किक of_ADDI(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित wid = cp->संख्या;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_immediate_rval (cp, r);

      l.add(r);

      परत सत्य;
}

/*
 * %add/wr
 */
तार्किक of_ADD_WR(vthread_t thr, vvp_code_t)
{
      द्विगुण r = thr->काढा_real();
      द्विगुण l = thr->काढा_real();
      thr->ढकला_real(l + r);
      परत सत्य;
}

/* %assign/ar <array>, <delay>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory. <delay>
 * is the delay in simulation time. <bit> is the index register
 * containing the real value.
 */
तार्किक of_ASSIGN_AR(vthread_t thr, vvp_code_t cp)
{
      दीर्घ adr = thr->शब्द[3].w_int;
      अचिन्हित delay = cp->bit_idx[0];
      द्विगुण value = thr->काढा_real();

      जर (adr >= 0) {
	    schedule_assign_array_word(cp->array, adr, value, delay);
      }

      परत सत्य;
}

/* %assign/ar/d <array>, <delay_idx>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory.
 * <delay_idx> is the integer register that contains the delay value.
 */
तार्किक of_ASSIGN_ARD(vthread_t thr, vvp_code_t cp)
{
      दीर्घ adr = thr->शब्द[3].w_int;
      द्विगुण value = thr->काढा_real();

      जर (adr >= 0) {
	    vvp_time64_t delay = thr->शब्द[cp->bit_idx[0]].w_uint;
	    schedule_assign_array_word(cp->array, adr, value, delay);
      }

      परत सत्य;
}

/* %assign/ar/e <array>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory. <bit>
 * is the index register containing the real value. The event
 * information is contained in the thread event control registers
 * and is set with %evctl.
 */
तार्किक of_ASSIGN_ARE(vthread_t thr, vvp_code_t cp)
{
      दीर्घ adr = thr->शब्द[3].w_int;
      द्विगुण value = thr->काढा_real();

      जर (adr >= 0) {
	    जर (thr->ecount == 0) {
		  schedule_assign_array_word(cp->array, adr, value, 0);
	    } अन्यथा {
		  schedule_evctl(cp->array, adr, value, thr->event,
		                 thr->ecount);
	    }
      }

      परत सत्य;
}

/*
 * %assign/vec4 <var>, <delay>
 */
तार्किक of_ASSIGN_VEC4(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      अचिन्हित delay = cp->bit_idx[0];
      स्थिरांक vvp_vector4_t&val = thr->peek_vec4();

      schedule_assign_vector(ptr, 0, 0, val, delay);
      thr->काढा_vec4(1);
      परत सत्य;
}

/*
 * Resizes a vector value for a partial assignment so that the value is fully
 * in-bounds of the target signal. Both `val` and `off` will be updated if
 * necessary.
 *
 * Returns false if the value is fully out-of-bounds and the assignment should
 * be skipped. Otherwise returns true.
 */
स्थिर तार्किक resize_rval_vec(vvp_vector4_t &val, int64_t &off,
			    अचिन्हित पूर्णांक sig_wid)
{
      अचिन्हित पूर्णांक wid = val.size();

        // Fully in bounds, most likely case
      जर (off >= 0 && (uint64_t)off + wid <= sig_wid)
	    परत सत्य;

      अचिन्हित पूर्णांक base = 0;
      जर (off >= 0) {
	      // Fully out-of-bounds
	    जर ((uint64_t)off >= sig_wid)
		  परत असत्य;
      } अन्यथा {
	      // Fully out-of-bounds */
	    जर ((uint64_t)(-off) >= wid)
		  परत असत्य;

	      // If the index is below the vector, then only assign the high
	      // bits that overlap with the target
	    base = -off;
	    wid += off;
		off = 0;
      }

	// If the value is partly above the target, then only assign
	// the bits that overlap
      जर ((uint64_t)off + wid > sig_wid)
	    wid = sig_wid - (uint64_t)off;

      val = val.subvalue(base, wid);

      परत सत्य;
}

/*
 * %assign/vec4/a/d <arr>, <offx>, <delx>
 */
तार्किक of_ASSIGN_VEC4_A_D(vthread_t thr, vvp_code_t cp)
{
      पूर्णांक off_idx = cp->bit_idx[0];
      पूर्णांक del_idx = cp->bit_idx[1];
      पूर्णांक adr_idx = 3;

      int64_t  off = off_idx ? thr->शब्द[off_idx].w_int : 0;
      vvp_time64_t del = del_idx? thr->शब्द[del_idx].w_uint : 0;
      दीर्घ     adr = thr->शब्द[adr_idx].w_int;

      vvp_vector4_t val = thr->काढा_vec4();

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      जर (thr->flags[4] == BIT4_1)
	    परत सत्य;

      जर (!resize_rval_vec(val, off, cp->array->get_word_size()))
	    परत सत्य;

      schedule_assign_array_word(cp->array, adr, off, val, del);

      परत सत्य;
}

/*
 * %assign/vec4/a/e <arr>, <offx>
 */
तार्किक of_ASSIGN_VEC4_A_E(vthread_t thr, vvp_code_t cp)
{
      पूर्णांक off_idx = cp->bit_idx[0];
      पूर्णांक adr_idx = 3;

      int64_t  off = off_idx ? thr->शब्द[off_idx].w_int : 0;
      दीर्घ     adr = thr->शब्द[adr_idx].w_int;

      vvp_vector4_t val = thr->काढा_vec4();

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      जर (thr->flags[4] == BIT4_1)
	    परत सत्य;

      जर (!resize_rval_vec(val, off, cp->array->get_word_size()))
	    परत सत्य;

      जर (thr->ecount == 0) {
	    schedule_assign_array_word(cp->array, adr, off, val, 0);
      } अन्यथा {
	    schedule_evctl(cp->array, adr, val, off, thr->event, thr->ecount);
      }

      परत सत्य;
}

/*
 * %assign/vec4/off/d <var>, <off>, <del>
 */
तार्किक of_ASSIGN_VEC4_OFF_D(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      अचिन्हित off_index = cp->bit_idx[0];
      अचिन्हित del_index = cp->bit_idx[1];
      vvp_vector4_t val = thr->काढा_vec4();

      int64_t off = thr->शब्द[off_index].w_int;
      vvp_time64_t del = thr->शब्द[del_index].w_uint;

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      जर (thr->flags[4] == BIT4_1)
	    परत सत्य;

      vvp_signal_मूल्य*sig = dynamic_cast<vvp_signal_मूल्य*> (cp->net->fil);
      प्रतिज्ञा(sig);

      जर (!resize_rval_vec(val, off, sig->मूल्य_size()))
	    परत सत्य;

      schedule_assign_vector(ptr, off, sig->मूल्य_size(), val, del);
      परत सत्य;
}

/*
 * %assign/vec4/off/e <var>, <off>
 */
तार्किक of_ASSIGN_VEC4_OFF_E(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      अचिन्हित off_index = cp->bit_idx[0];
      vvp_vector4_t val = thr->काढा_vec4();

      int64_t off = thr->शब्द[off_index].w_int;

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      जर (thr->flags[4] == BIT4_1)
	    परत सत्य;

      vvp_signal_मूल्य*sig = dynamic_cast<vvp_signal_मूल्य*> (cp->net->fil);
      प्रतिज्ञा(sig);

      जर (!resize_rval_vec(val, off, sig->मूल्य_size()))
	    परत सत्य;

      जर (thr->ecount == 0) {
	    schedule_assign_vector(ptr, off, sig->मूल्य_size(), val, 0);
      } अन्यथा {
	    schedule_evctl(ptr, val, off, sig->मूल्य_size(), thr->event, thr->ecount);
      }

      परत सत्य;
}

/*
 * %assign/vec4/d <var-label> <delay>
 */
तार्किक of_ASSIGN_VEC4D(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      अचिन्हित del_index = cp->bit_idx[0];
      vvp_time64_t del = thr->शब्द[del_index].w_int;

      vvp_vector4_t value = thr->काढा_vec4();

      vvp_signal_मूल्य*sig = dynamic_cast<vvp_signal_मूल्य*> (cp->net->fil);
      प्रतिज्ञा(sig);

      schedule_assign_vector(ptr, 0, sig->मूल्य_size(), value, del);

      परत सत्य;
}

/*
 * %assign/vec4/e <var-label>
 */
तार्किक of_ASSIGN_VEC4E(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_vector4_t value = thr->काढा_vec4();

      vvp_signal_मूल्य*sig = dynamic_cast<vvp_signal_मूल्य*> (cp->net->fil);
      प्रतिज्ञा(sig);

      जर (thr->ecount == 0) {
	    schedule_assign_vector(ptr, 0, sig->मूल्य_size(), value, 0);
      } अन्यथा {
	    schedule_evctl(ptr, value, 0, sig->मूल्य_size(), thr->event, thr->ecount);
      }

      परत सत्य;
}

/*
 * This is %assign/wr <vpi-label>, <delay>
 *
 * This assigns (after a delay) a value to a real variable. Use the
 * vpi_put_value function to do the assign, with the delay written
 * into the vpiInertialDelay carrying the desired delay.
 */
तार्किक of_ASSIGN_WR(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित delay = cp->bit_idx[0];
      द्विगुण value = thr->काढा_real();
      s_vpi_time del;

      del.type = vpiSimTime;
      vpip_time_to_timestruct(&del, delay);

      __vpiHandle*tmp = cp->handle;

      t_vpi_मूल्य val;
      val.format = vpiRealVal;
      val.value.real = value;
      vpi_put_मूल्य(tmp, &val, &del, vpiTransportDelay);

      परत सत्य;
}

तार्किक of_ASSIGN_WRD(vthread_t thr, vvp_code_t cp)
{
      vvp_time64_t delay = thr->शब्द[cp->bit_idx[0]].w_uint;
      द्विगुण value = thr->काढा_real();
      s_vpi_time del;

      del.type = vpiSimTime;
      vpip_time_to_timestruct(&del, delay);

      __vpiHandle*tmp = cp->handle;

      t_vpi_मूल्य val;
      val.format = vpiRealVal;
      val.value.real = value;
      vpi_put_मूल्य(tmp, &val, &del, vpiTransportDelay);

      परत सत्य;
}

तार्किक of_ASSIGN_WRE(vthread_t thr, vvp_code_t cp)
{
      प्रतिज्ञा(thr->event != 0);
      द्विगुण value = thr->काढा_real();
      __vpiHandle*tmp = cp->handle;

	// If the count is zero then just put the value.
      जर (thr->ecount == 0) {
	    t_vpi_मूल्य val;

	    val.format = vpiRealVal;
	    val.value.real = value;
	    vpi_put_मूल्य(tmp, &val, 0, vpiNoDelay);
      } अन्यथा {
	    schedule_evctl(tmp, value, thr->event, thr->ecount);
      }

      thr->event = 0;
      thr->ecount = 0;

      परत सत्य;
}

तार्किक of_BLEND(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t vala = thr->काढा_vec4();
      vvp_vector4_t valb = thr->काढा_vec4();
      प्रतिज्ञा(vala.size() == valb.size());

      साठी (अचिन्हित idx = 0 ; idx < vala.size() ; idx += 1) {
	    जर (vala.value(idx) == valb.value(idx))
		  सुरूठेवा;

	    vala.set_bit(idx, BIT4_X);
      }

      thr->ढकला_vec4(vala);
      परत सत्य;
}

तार्किक of_BLEND_STR(vthread_t thr, vvp_code_t)
{
      string f = thr->काढा_str();
      string t = thr->काढा_str();
      thr->ढकला_str((t == f) ? t : "");
      परत सत्य;
}

तार्किक of_BLEND_WR(vthread_t thr, vvp_code_t)
{
      द्विगुण f = thr->काढा_real();
      द्विगुण t = thr->काढा_real();
      thr->ढकला_real((t == f) ? t : 0.0);
      परत सत्य;
}

तार्किक of_BREAKPOINT(vthread_t, vvp_code_t)
{
      परत सत्य;
}

/*
 * %callf/void <code-label>, <scope-label>
 * Combine the %fork and %join steps for invoking a function.
 */
स्थिर तार्किक do_callf_void(vthread_t thr, vthread_t child)
{

      जर (child->parent_scope->is_automatic()) {
	      /* The context allocated for this child is the top entry
		 on the write context stack */
	    child->wt_context = thr->wt_context;
	    child->rd_context = thr->wt_context;
      }

        // Mark the function thread as a direct child of the current thread.
      child->parent = thr;
      thr->children.insert(child);
        // This should be the only child
      प्रतिज्ञा(thr->children.size()==1);

        // Execute the function. This SHOULD run the function to completion,
        // but there are some exceptional situations where it won't.
      प्रतिज्ञा(child->parent_scope->get_type_code() == vpiFunction);
      child->is_scheduled = 1;
      child->i_am_in_function = 1;
      vthread_run(child);
      running_thread = thr;

      जर (child->i_have_ended) {
	    do_join(thr, child);
	    परत सत्य;
      } अन्यथा {
	    thr->i_am_joining = 1;
	    परत असत्य;
      }
}

तार्किक of_CALLF_OBJ(vthread_t thr, vvp_code_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);
      परत do_callf_void(thr, child);

      // XXXX NOT IMPLEMENTED
}

तार्किक of_CALLF_REAL(vthread_t thr, vvp_code_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);

	// This is the return value. Push a place-holder value. The function
	// will replace this with the actual value using a %ret/real instruction.
      thr->ढकला_real(0.0);
      child->args_real.push_back(0);

      परत do_callf_void(thr, child);
}

तार्किक of_CALLF_STR(vthread_t thr, vvp_code_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);

      thr->ढकला_str("");
      child->args_str.push_back(0);

      परत do_callf_void(thr, child);
}

तार्किक of_CALLF_VEC4(vthread_t thr, vvp_code_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);

      vpiScopeFunction*scope_func = dynamic_cast<vpiScopeFunction*>(cp->scope);
      प्रतिज्ञा(scope_func);

	// This is the return value. Push a place-holder value. The function
	// will replace this with the actual value using a %ret/real instruction.
      thr->ढकला_vec4(vvp_vector4_t(scope_func->get_func_width(), scope_func->get_func_init_val()));
      child->args_vec4.push_back(0);

      परत do_callf_void(thr, child);
}

तार्किक of_CALLF_VOID(vthread_t thr, vvp_code_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);
      परत do_callf_void(thr, child);
}

/*
 * The %cassign/link instruction connects a source node to a
 * destination node. The destination node must be a signal, as it is
 * marked with the source of the cassign so that it may later be
 * unlinked without specifically knowing the source that this
 * instruction used.
 */
तार्किक of_CASSIGN_LINK(vthread_t, vvp_code_t cp)
{
      vvp_net_t*dst = cp->net;
      vvp_net_t*src = cp->net2;

      vvp_fun_signal_base*sig
	    = dynamic_cast<vvp_fun_signal_base*>(dst->fun);
      प्रतिज्ञा(sig);

	/* Any previous continuous assign should have been removed already. */
      प्रतिज्ञा(sig->cassign_link == 0);

      sig->cassign_link = src;

	/* Link the output of the src to the port[1] (the cassign
	   port) of the destination. */
      vvp_net_ptr_t dst_ptr (dst, 1);
      src->link(dst_ptr);

      परत सत्य;
}

/*
 * If there is an existing continuous assign linked to the destination
 * node, unlink it. This must be done before applying a new continuous
 * assign, otherwise the initial assigned value will be propagated to
 * any other nodes driven by the old continuous assign source.
 */
स्थिर शून्य cassign_unlink(vvp_net_t*dst)
{
      vvp_fun_signal_base*sig
	    = dynamic_cast<vvp_fun_signal_base*>(dst->fun);
      प्रतिज्ञा(sig);

      जर (sig->cassign_link == 0)
	    परत;

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
तार्किक of_CASSIGN_VEC4(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      vvp_vector4_t value = thr->काढा_vec4();

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

	/* Set the value into port 1 of the destination. */
      vvp_net_ptr_t ptr (net, 1);
      vvp_send_vec4(ptr, value, 0);

      परत सत्य;
}

/*
 * %cassign/vec4/off <var>, <off>
 */
तार्किक of_CASSIGN_VEC4_OFF(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      अचिन्हित base_idx = cp->bit_idx[0];
      दीर्घ base = thr->शब्द[base_idx].w_int;
      vvp_vector4_t value = thr->काढा_vec4();
      अचिन्हित wid = value.size();

      जर (thr->flags[4] == BIT4_1)
	    परत सत्य;

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

      vvp_signal_मूल्य*sig = dynamic_cast<vvp_signal_मूल्य*> (net->fil);
      प्रतिज्ञा(sig);

      जर (base < 0 && (wid <= (अचिन्हित)-base))
	    परत सत्य;

      जर (base >= (दीर्घ)sig->मूल्य_size())
	    परत सत्य;

      जर (base < 0) {
	    wid -= (अचिन्हित) -base;
	    base = 0;
	    value.resize(wid);
      }

      जर (base+wid > sig->मूल्य_size()) {
	    wid = sig->मूल्य_size() - base;
	    value.resize(wid);
      }

      vvp_net_ptr_t ptr (net, 1);
      vvp_send_vec4_pv(ptr, value, base, sig->मूल्य_size(), 0);
      परत सत्य;
}

तार्किक of_CASSIGN_WR(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net  = cp->net;
      द्विगुण value = thr->काढा_real();

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

	/* Set the value into port 1 of the destination. */
      vvp_net_ptr_t ptr (net, 1);
      vvp_send_real(ptr, value, 0);

      परत सत्य;
}

/*
 * %cast2
 */
तार्किक of_CAST2(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      अचिन्हित wid = val.size();

      साठी (अचिन्हित idx = 0 ; idx < wid ; idx += 1) {
	    निवड (val.value(idx)) {
		स्थिती BIT4_0:
		स्थिती BIT4_1:
		  थांबा;
		पूर्वनिर्धारित:
		  val.set_bit(idx, BIT4_0);
		  थांबा;
	    }
      }

      परत सत्य;
}

तार्किक do_cast_vec_dar(vthread_t thr, vvp_code_t cp, तार्किक as_vec4)
{
      अचिन्हित wid = cp->संख्या;

      vvp_object_t obj;
      thr->काढा_object(obj);

      vvp_darray*darray = obj.peek<vvp_darray>();
      प्रतिज्ञा(darray);

      vvp_vector4_t vec = darray->get_bitstream(as_vec4);
      जर (vec.size() != wid) {
	    cerr << thr->get_fileline()
	         << "VVP error: size mismatch when casting dynamic array to vector." << endl;
            thr->ढकला_vec4(vvp_vector4_t(wid));
            schedule_stop(0);
            परत असत्य;
      }
      thr->ढकला_vec4(vec);
      परत सत्य;
}

/*
 * %cast/vec2/dar <wid>
 */
तार्किक of_CAST_VEC2_DAR(vthread_t thr, vvp_code_t cp)
{
      परत do_cast_vec_dar(thr, cp, असत्य);
}

/*
 * %cast/vec4/dar <wid>
 */
तार्किक of_CAST_VEC4_DAR(vthread_t thr, vvp_code_t cp)
{
      परत do_cast_vec_dar(thr, cp, सत्य);
}

/*
 * %cast/vec4/str <wid>
 */
तार्किक of_CAST_VEC4_STR(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित wid = cp->संख्या;
      string str = thr->काढा_str();

      vvp_vector4_t vec(wid, BIT4_0);

      जर (wid != 8*str.length()) {
	    cerr << thr->get_fileline()
	         << "VVP error: size mismatch when casting string to vector." << endl;
            thr->ढकला_vec4(vec);
            schedule_stop(0);
            परत असत्य;
      }

      अचिन्हित sdx = 0;
      अचिन्हित vdx = wid;
      पर्यंत (vdx > 0) {
            अक्षर ch = str[sdx++];
            vdx -= 8;
            साठी (अचिन्हित bdx = 0; bdx < 8; bdx += 1) {
                  जर (ch & 1)
                        vec.set_bit(vdx+bdx, BIT4_1);
                  ch >>= 1;
            }
      }

      thr->ढकला_vec4(vec);
      परत सत्य;
}

स्थिर शून्य do_CMPE(vthread_t thr, स्थिरांक vvp_vector4_t&lval, स्थिरांक vvp_vector4_t&rval)
{
      प्रतिज्ञा(rval.size() == lval.size());

      जर (lval.has_xz() || rval.has_xz()) {

	    अचिन्हित wid = lval.size();
	    vvp_bit4_t eq  = BIT4_1;
	    vvp_bit4_t eeq = BIT4_1;

	    साठी (अचिन्हित idx = 0 ; idx < wid ; idx += 1) {
		  vvp_bit4_t lv = lval.value(idx);
		  vvp_bit4_t rv = rval.value(idx);

		  जर (lv != rv)
			eeq = BIT4_0;

		  जर (eq==BIT4_1 && (bit4_is_xz(lv) || bit4_is_xz(rv)))
			eq = BIT4_X;
		  जर ((lv == BIT4_0) && (rv==BIT4_1))
			eq = BIT4_0;
		  जर ((lv == BIT4_1) && (rv==BIT4_0))
			eq = BIT4_0;

		  जर (eq == BIT4_0)
			थांबा;
	    }

	    thr->flags[4] = eq;
	    thr->flags[6] = eeq;

      } अन्यथा {
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
तार्किक of_CMPE(vthread_t thr, vvp_code_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      स्थिरांक vvp_vector4_t&rval = thr->peek_vec4(0);
      स्थिरांक vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPE(thr, lval, rval);

      thr->काढा_vec4(2);
      परत सत्य;
}

तार्किक of_CMPNE(vthread_t thr, vvp_code_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      स्थिरांक vvp_vector4_t&rval = thr->peek_vec4(0);
      स्थिरांक vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];
      thr->flags[6] =  ~thr->flags[6];

      thr->काढा_vec4(2);
      परत सत्य;
}

/*
 * %cmpi/e <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
तार्किक of_CMPIE(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित wid = cp->संख्या;

      स्थिरांक vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_immediate_rval (cp, rval);

      do_CMPE(thr, lval, rval);

      thr->काढा_vec4(1);
      परत सत्य;
}

तार्किक of_CMPINE(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित wid = cp->संख्या;

      स्थिरांक vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_immediate_rval (cp, rval);

      do_CMPE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];
      thr->flags[6] =  ~thr->flags[6];

      thr->काढा_vec4(1);
      परत सत्य;
}



स्थिर शून्य do_CMPS(vthread_t thr, स्थिरांक vvp_vector4_t&lval, स्थिरांक vvp_vector4_t&rval)
{
      प्रतिज्ञा(rval.size() == lval.size());

	// If either value has XZ bits, then the eq and lt values are
	// known already to be X. Just calculate the eeq result as a
	// special case and short circuit the rest of the compare.
      जर (lval.has_xz() || rval.has_xz()) {
	    thr->flags[4] = BIT4_X; // eq
	    thr->flags[5] = BIT4_X; // lt
	    thr->flags[6] = lval.eeq(rval)? BIT4_1 : BIT4_0;
	    परत;
      }

	// Past this point, we know we are dealing only with fully
	// defined values.
      अचिन्हित wid = lval.size();

      स्थिरांक vvp_bit4_t sig1 = lval.value(wid-1);
      स्थिरांक vvp_bit4_t sig2 = rval.value(wid-1);

	// If the lval is <0 and the rval is >=0, then we know the result.
      जर ((sig1 == BIT4_1) && (sig2 == BIT4_0)) {
	    thr->flags[4] = BIT4_0; // eq;
	    thr->flags[5] = BIT4_1; // lt;
	    thr->flags[6] = BIT4_0; // eeq
	    परत;
      }

	// If the lval is >=0 and the rval is <0, then we know the result.
      जर ((sig1 == BIT4_0) && (sig2 == BIT4_1)) {
	    thr->flags[4] = BIT4_0; // eq;
	    thr->flags[5] = BIT4_0; // lt;
	    thr->flags[6] = BIT4_0; // eeq
	    परत;
      }

	// The values have the same sign, so we have to look at the
	// actual value. Scan from the MSB down. As soon as we find a
	// bit that differs, we know the result.

      साठी (अचिन्हित idx = 1 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t lv = lval.value(wid-1-idx);
	    vvp_bit4_t rv = rval.value(wid-1-idx);

	    जर (lv == rv)
		  सुरूठेवा;

	    thr->flags[4] = BIT4_0; // eq
	    thr->flags[6] = BIT4_0; // eeq

	    जर (lv==BIT4_0) {
		  thr->flags[5] = BIT4_1; // lt
	    } अन्यथा {
		  thr->flags[5] = BIT4_0; // lt
	    }
	    परत;
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
तार्किक of_CMPS(vthread_t thr, vvp_code_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      स्थिरांक vvp_vector4_t&rval = thr->peek_vec4(0);
      स्थिरांक vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPS(thr, lval, rval);

      thr->काढा_vec4(2);
      परत सत्य;
}

/*
 * %cmpi/s <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
तार्किक of_CMPIS(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित wid = cp->संख्या;

      स्थिरांक vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_immediate_rval (cp, rval);

      do_CMPS(thr, lval, rval);

      thr->काढा_vec4(1);
      परत सत्य;
}

तार्किक of_CMPSTR(vthread_t thr, vvp_code_t)
{
      string re = thr->काढा_str();
      string le = thr->काढा_str();

      पूर्णांक rc = म_भेद(le.c_str(), re.c_str());

      vvp_bit4_t eq;
      vvp_bit4_t lt;

      जर (rc == 0) {
	    eq = BIT4_1;
	    lt = BIT4_0;
      } अन्यथा जर (rc < 0) {
	    eq = BIT4_0;
	    lt = BIT4_1;
      } अन्यथा {
	    eq = BIT4_0;
	    lt = BIT4_0;
      }

      thr->flags[4] = eq;
      thr->flags[5] = lt;

      परत सत्य;
}

स्थिर शून्य of_CMPU_the_hard_way(vthread_t thr, अचिन्हित wid,
				 स्थिरांक vvp_vector4_t&lval,
				 स्थिरांक vvp_vector4_t&rval)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_bit4_t eeq = BIT4_1;

      साठी (अचिन्हित idx = 0 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t lv = lval.value(idx);
	    vvp_bit4_t rv = rval.value(idx);

	    जर (lv != rv)
		  eeq = BIT4_0;

	    जर (eq==BIT4_1 && (bit4_is_xz(lv) || bit4_is_xz(rv)))
		  eq = BIT4_X;
	    जर ((lv == BIT4_0) && (rv==BIT4_1))
		  eq = BIT4_0;
	    जर ((lv == BIT4_1) && (rv==BIT4_0))
		  eq = BIT4_0;

	    जर (eq == BIT4_0)
		  थांबा;

      }

      thr->flags[4] = eq;
      thr->flags[5] = BIT4_X;
      thr->flags[6] = eeq;
}

स्थिर शून्य do_CMPU(vthread_t thr, स्थिरांक vvp_vector4_t&lval, स्थिरांक vvp_vector4_t&rval)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_bit4_t lt = BIT4_0;

      जर (rval.size() != lval.size()) {
	    cerr << thr->get_fileline()
	         << "VVP ERROR: %cmp/u operand width mismatch: lval=" << lval
		 << ", rval=" << rval << endl;
      }
      प्रतिज्ञा(rval.size() == lval.size());
      अचिन्हित wid = lval.size();

      अचिन्हित दीर्घ*larray = lval.subarray(0,wid);
      जर (larray == 0) परत of_CMPU_the_hard_way(thr, wid, lval, rval);

      अचिन्हित दीर्घ*rarray = rval.subarray(0,wid);
      जर (rarray == 0) {
	    delete[]larray;
	    परत of_CMPU_the_hard_way(thr, wid, lval, rval);
      }

      अचिन्हित शब्द = (wid+CPU_WORD_BITS-1) / CPU_WORD_BITS;

      साठी (अचिन्हित wdx = 0 ; wdx < शब्द ; wdx += 1) {
	    जर (larray[wdx] == rarray[wdx])
		  सुरूठेवा;

	    eq = BIT4_0;
	    जर (larray[wdx] < rarray[wdx])
		  lt = BIT4_1;
	    अन्यथा
		  lt = BIT4_0;
      }

      delete[]larray;
      delete[]rarray;

      thr->flags[4] = eq;
      thr->flags[5] = lt;
      thr->flags[6] = eq;
}

तार्किक of_CMPU(vthread_t thr, vvp_code_t)
{

      स्थिरांक vvp_vector4_t&rval = thr->peek_vec4(0);
      स्थिरांक vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPU(thr, lval, rval);

      thr->काढा_vec4(2);
      परत सत्य;
}

/*
 * %cmpi/u <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
तार्किक of_CMPIU(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित wid = cp->संख्या;

      स्थिरांक vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_immediate_rval (cp, rval);

      do_CMPU(thr, lval, rval);

      thr->काढा_vec4(1);
      परत सत्य;
}


/*
 * %cmp/x
 */
तार्किक of_CMPX(vthread_t thr, vvp_code_t)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_vector4_t rval = thr->काढा_vec4();
      vvp_vector4_t lval = thr->काढा_vec4();

      प्रतिज्ञा(rval.size() == lval.size());
      अचिन्हित wid = lval.size();

      साठी (अचिन्हित idx = 0 ; idx < wid ; idx += 1) {
	    vvp_bit4_t lv = lval.value(idx);
	    vvp_bit4_t rv = rval.value(idx);
	    जर ((lv != rv) && !bit4_is_xz(lv) && !bit4_is_xz(rv)) {
		  eq = BIT4_0;
		  थांबा;
	    }
      }

      thr->flags[4] = eq;
      परत सत्य;
}

स्थिर शून्य do_CMPWE(vthread_t thr, स्थिरांक vvp_vector4_t&lval, स्थिरांक vvp_vector4_t&rval)
{
      प्रतिज्ञा(rval.size() == lval.size());

      जर (lval.has_xz() || rval.has_xz()) {

	    अचिन्हित wid = lval.size();
	    vvp_bit4_t eq  = BIT4_1;

	    साठी (अचिन्हित idx = 0 ; idx < wid ; idx += 1) {
		  vvp_bit4_t lv = lval.value(idx);
		  vvp_bit4_t rv = rval.value(idx);

		  जर (bit4_is_xz(rv))
			सुरूठेवा;
		  जर ((eq == BIT4_1) && bit4_is_xz(lv))
			eq = BIT4_X;
		  जर ((lv == BIT4_0) && (rv==BIT4_1))
			eq = BIT4_0;
		  जर ((lv == BIT4_1) && (rv==BIT4_0))
			eq = BIT4_0;

		  जर (eq == BIT4_0)
			थांबा;
	    }

	    thr->flags[4] = eq;

      } अन्यथा {
	      // If there are no XZ bits anywhere, then the results of
	      // ==? match the === test.
	    thr->flags[4] = (lval.eeq(rval)? BIT4_1 : BIT4_0);
      }
}

तार्किक of_CMPWE(vthread_t thr, vvp_code_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      स्थिरांक vvp_vector4_t&rval = thr->peek_vec4(0);
      स्थिरांक vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPWE(thr, lval, rval);

      thr->काढा_vec4(2);
      परत सत्य;
}

तार्किक of_CMPWNE(vthread_t thr, vvp_code_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      स्थिरांक vvp_vector4_t&rval = thr->peek_vec4(0);
      स्थिरांक vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPWE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];

      thr->काढा_vec4(2);
      परत सत्य;
}

तार्किक of_CMPWR(vthread_t thr, vvp_code_t)
{
      द्विगुण r = thr->काढा_real();
      द्विगुण l = thr->काढा_real();

      vvp_bit4_t eq = (l == r)? BIT4_1 : BIT4_0;
      vvp_bit4_t lt = (l <  r)? BIT4_1 : BIT4_0;

      thr->flags[4] = eq;
      thr->flags[5] = lt;

      परत सत्य;
}

/*
 * %cmp/z
 */
तार्किक of_CMPZ(vthread_t thr, vvp_code_t)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_vector4_t rval = thr->काढा_vec4();
      vvp_vector4_t lval = thr->काढा_vec4();

      प्रतिज्ञा(rval.size() == lval.size());
      अचिन्हित wid = lval.size();

      साठी (अचिन्हित idx = 0 ; idx < wid ; idx += 1) {
	    vvp_bit4_t lv = lval.value(idx);
	    vvp_bit4_t rv = rval.value(idx);
	    जर ((lv != rv) && (rv != BIT4_Z) && (lv != BIT4_Z)) {
		  eq = BIT4_0;
		  थांबा;
	    }
      }

      thr->flags[4] = eq;
      परत सत्य;
}

/*
 *  %concat/str;
 */
तार्किक of_CONCAT_STR(vthread_t thr, vvp_code_t)
{
      string text = thr->काढा_str();
      thr->peek_str(0).append(text);
      परत सत्य;
}

/*
 *  %concati/str <string>;
 */
तार्किक of_CONCATI_STR(vthread_t thr, vvp_code_t cp)
{
      स्थिरांक अक्षर*text = cp->text;
      thr->peek_str(0).append(filter_string(text));
      परत सत्य;
}

/*
 * %concat/vec4
 */
तार्किक of_CONCAT_VEC4(vthread_t thr, vvp_code_t)
{
      स्थिरांक vvp_vector4_t&lsb = thr->peek_vec4(0);
      स्थिरांक vvp_vector4_t&msb = thr->peek_vec4(1);

	// The result is the size of the top two vectors in the stack.
      vvp_vector4_t res (msb.size() + lsb.size(), BIT4_X);

	// Build up the result.
      res.set_vec(0, lsb);
      res.set_vec(lsb.size(), msb);

	// Rearrange the stack to pop the inputs and push the
	// result. Do that by actually popping only 1 stack position
	// and replacing the new top with the new value.
      thr->काढा_vec4(1);
      thr->peek_vec4() = res;

      परत सत्य;
}

/*
 * %concati/vec4 <vala>, <valb>, <wid>
 *
 * Concat the immediate value to the LOW bits of the concatenation.
 * Get the HIGH bits from the top of the vec4 stack.
 */
तार्किक of_CONCATI_VEC4(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित wid  = cp->संख्या;

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
      परत सत्य;
}

/*
 * %cvt/rv
 */
तार्किक of_CVT_RV(vthread_t thr, vvp_code_t)
{
      द्विगुण val;
      vvp_vector4_t val4 = thr->काढा_vec4();
      vector4_to_मूल्य(val4, val, असत्य);
      thr->ढकला_real(val);
      परत सत्य;
}

/*
 * %cvt/rv/s
 */
तार्किक of_CVT_RV_S(vthread_t thr, vvp_code_t)
{
      द्विगुण val;
      vvp_vector4_t val4 = thr->काढा_vec4();
      vector4_to_मूल्य(val4, val, सत्य);
      thr->ढकला_real(val);
      परत सत्य;
}

/*
 * %cvt/sr <idx>
 * Pop the top value from the real stack, convert it to a 64bit signed
 * and save it to the indexed register.
 */
तार्किक of_CVT_SR(vthread_t thr, vvp_code_t cp)
{
      द्विगुण r = thr->काढा_real();
      thr->शब्द[cp->bit_idx[0]].w_int = i64round(r);

      परत सत्य;
}

/*
 * %cvt/ur <idx>
 */
तार्किक of_CVT_UR(vthread_t thr, vvp_code_t cp)
{
      द्विगुण r = thr->काढा_real();
      thr->शब्द[cp->bit_idx[0]].w_uint = vlg_round_to_u64(r);

      परत सत्य;
}

/*
 * %cvt/vr <wid>
 */
तार्किक of_CVT_VR(vthread_t thr, vvp_code_t cp)
{
      द्विगुण r = thr->काढा_real();
      अचिन्हित wid = cp->संख्या;

      vvp_vector4_t tmp(wid, r);
      thr->ढकला_vec4(tmp);
      परत सत्य;
}

/*
 * This implements the %deassign instruction. All we do is write a
 * long(1) to port-3 of the addressed net. This turns off an active
 * continuous assign activated by %cassign/v
 */
तार्किक of_DEASSIGN(vthread_t, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      अचिन्हित base  = cp->bit_idx[0];
      अचिन्हित width = cp->bit_idx[1];

      vvp_signal_मूल्य*fil = dynamic_cast<vvp_signal_मूल्य*> (net->fil);
      प्रतिज्ञा(fil);
      vvp_fun_signal_vec*sig = dynamic_cast<vvp_fun_signal_vec*>(net->fun);
      प्रतिज्ञा(sig);

      जर (base >= fil->मूल्य_size()) परत सत्य;
      जर (base+width > fil->मूल्य_size()) width = fil->मूल्य_size() - base;

      तार्किक full_sig = base == 0 && width == fil->मूल्य_size();

	// This is the net that is forcing me...
      जर (vvp_net_t*src = sig->cassign_link) {
	    जर (! full_sig) {
		  ख_लिखो(मानक_त्रुटि, "Sorry: when a signal is assigning a "
		          "register, I cannot deassign part of it.\n");
		  निकास(1);
	    }
	      // And this is the pointer to be removed.
	    vvp_net_ptr_t dst_ptr (net, 1);
	    src->unlink(dst_ptr);
	    sig->cassign_link = 0;
      }

	/* Do we release all or part of the net? */
      जर (full_sig) {
	    sig->deassign();
      } अन्यथा {
	    sig->deassign_pv(base, width);
      }

      परत सत्य;
}

तार्किक of_DEASSIGN_WR(vthread_t, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_real*sig = dynamic_cast<vvp_fun_signal_real*>(net->fun);
      प्रतिज्ञा(sig);

	// This is the net that is forcing me...
      जर (vvp_net_t*src = sig->cassign_link) {
	      // And this is the pointer to be removed.
	    vvp_net_ptr_t dst_ptr (net, 1);
	    src->unlink(dst_ptr);
	    sig->cassign_link = 0;
      }

      sig->deassign();

      परत सत्य;
}

/*
 * %debug/thr
 */
तार्किक of_DEBUG_THR(vthread_t thr, vvp_code_t cp)
{
      स्थिरांक अक्षर*text = cp->text;
      thr->debug_dump(cerr, text);
      परत सत्य;
}

/*
 * The delay takes two 32bit numbers to make up a 64bit time.
 *
 *   %delay <low>, <hig>
 */
तार्किक of_DELAY(vthread_t thr, vvp_code_t cp)
{
      vvp_time64_t खालचा = cp->bit_idx[0];
      vvp_time64_t hig = cp->bit_idx[1];

      vvp_time64_t delay = (hig << 32) | खालचा;

      जर (delay == 0) schedule_inactive(thr);
      अन्यथा schedule_vthread(thr, delay);
      परत असत्य;
}

तार्किक of_DELAYX(vthread_t thr, vvp_code_t cp)
{
      vvp_time64_t delay;

      प्रतिज्ञा(cp->संख्या < vthread_s::शब्द_मोजणी);
      delay = thr->शब्द[cp->संख्या].w_uint;
      जर (delay == 0) schedule_inactive(thr);
      अन्यथा schedule_vthread(thr, delay);
      परत असत्य;
}

तार्किक of_DELETE_ELEM(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      int64_t idx_val = thr->शब्द[3].w_int;
      जर (thr->flags[4] == BIT4_1) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with undefined index."
	         << endl;
	    परत सत्य;
      }
      जर (idx_val < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with negative index."
	         << endl;
	    परत सत्य;
      }
      माप_प्रकार idx = idx_val;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      प्रतिज्ञा(obj);

      vvp_queue*queue = obj->get_object().peek<vvp_queue>();
      जर (queue == 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping delete(" << idx
	         << ") on empty queue." << endl;
      } अन्यथा {
	    माप_प्रकार size = queue->get_size();
	    जर (idx >= size) {
		  cerr << thr->get_fileline()
		       << "Warning: skipping out of range delete(" << idx
		       << ") on queue of size " << size << "." << endl;
	    } अन्यथा {
		  queue->erase(idx);
	    }
      }

      परत सत्य;
}

/* %delete/obj <label>
 *
 * This operator works by assigning a nil to the target object. This
 * causes any value that might be there to be garbage collected, thus
 * deleting the object.
 */
तार्किक of_DELETE_OBJ(vthread_t thr, vvp_code_t cp)
{
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_send_object(ptr, vvp_object_t(), thr->wt_context);

      परत सत्य;
}

/* %reverse/obj <label>
 * Reverse element order of the queue or dynamic array object at this net.
 */
तार्किक of_REVERSE_OBJ(vthread_t /*thr*/, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      प्रतिज्ञा(obj);

      vvp_object_t stor = obj->get_object();
      जर (stor.test_nil()) परत सत्य;

	/* Match other queue ops: peek queues before vvp_darray so concrete
	   queue types dispatch reverse_elems() correctly. */
      जर (vvp_queue* queue = stor.peek<vvp_queue>()) {
	    queue->reverse_elems();
	    परत सत्य;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      प्रतिज्ञा(darray);
      darray->reverse_elems();

      परत सत्य;
}

/* %reverse/prop/obj <property-index>
 * Reverse queue/darray property storage inside the class object on stack top.
 */
तार्किक of_REVERSE_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      माप_प्रकार pid = cp->संख्या;

      vvp_object_t& शीर्ष = thr->peek_object();
      vvp_cobject*cobj = शीर्ष.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      जर (arr_obj.test_nil()) परत सत्य;

      जर (vvp_queue* queue = arr_obj.peek<vvp_queue>()) {
	    queue->reverse_elems();
	    परत सत्य;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      प्रतिज्ञा(darray);
      darray->reverse_elems();

      परत सत्य;
}

/* %sort/obj — ascending sort */
तार्किक of_SORT_OBJ(vthread_t /*thr*/, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      प्रतिज्ञा(obj);

      vvp_object_t stor = obj->get_object();
      जर (stor.test_nil()) परत सत्य;

      जर (vvp_queue* queue = stor.peek<vvp_queue>()) {
	    queue->sort_elems(सत्य);
	    परत सत्य;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      प्रतिज्ञा(darray);
      darray->sort_elems(सत्य);

      परत सत्य;
}

/* %sort/prop/obj */
तार्किक of_SORT_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      माप_प्रकार pid = cp->संख्या;

      vvp_object_t& शीर्ष = thr->peek_object();
      vvp_cobject*cobj = शीर्ष.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      जर (arr_obj.test_nil()) परत सत्य;

      जर (vvp_queue* queue = arr_obj.peek<vvp_queue>()) {
	    queue->sort_elems(सत्य);
	    परत सत्य;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      प्रतिज्ञा(darray);
      darray->sort_elems(सत्य);

      परत सत्य;
}

/* %rsort/obj — descending sort */
तार्किक of_RSORT_OBJ(vthread_t /*thr*/, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      प्रतिज्ञा(obj);

      vvp_object_t stor = obj->get_object();
      जर (stor.test_nil()) परत सत्य;

      जर (vvp_queue* queue = stor.peek<vvp_queue>()) {
	    queue->sort_elems(असत्य);
	    परत सत्य;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      प्रतिज्ञा(darray);
      darray->sort_elems(असत्य);

      परत सत्य;
}

/* %rsort/prop/obj */
तार्किक of_RSORT_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      माप_प्रकार pid = cp->संख्या;

      vvp_object_t& शीर्ष = thr->peek_object();
      vvp_cobject*cobj = शीर्ष.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      जर (arr_obj.test_nil()) परत सत्य;

      जर (vvp_queue* queue = arr_obj.peek<vvp_queue>()) {
	    queue->sort_elems(असत्य);
	    परत सत्य;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      प्रतिज्ञा(darray);
      darray->sort_elems(असत्य);

      परत सत्य;
}

/* %shuffle/obj */
तार्किक of_SHUFFLE_OBJ(vthread_t /*thr*/, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      प्रतिज्ञा(obj);

      vvp_object_t stor = obj->get_object();
      जर (stor.test_nil()) परत सत्य;

      जर (vvp_queue* queue = stor.peek<vvp_queue>()) {
	    queue->shuffle_elems();
	    परत सत्य;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      प्रतिज्ञा(darray);
      darray->shuffle_elems();

      परत सत्य;
}

/* %shuffle/prop/obj */
तार्किक of_SHUFFLE_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      माप_प्रकार pid = cp->संख्या;

      vvp_object_t& शीर्ष = thr->peek_object();
      vvp_cobject*cobj = शीर्ष.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      जर (arr_obj.test_nil()) परत सत्य;

      जर (vvp_queue* queue = arr_obj.peek<vvp_queue>()) {
	    queue->shuffle_elems();
	    परत सत्य;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      प्रतिज्ञा(darray);
      darray->shuffle_elems();

      परत सत्य;
}

/* %delete/tail <label>, idx
 *
 * Remove all elements after the one specified.
 */
तार्किक of_DELETE_TAIL(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      प्रतिज्ञा(obj);

      vvp_queue*queue = obj->get_object().peek<vvp_queue>();
      प्रतिज्ञा(queue);

      अचिन्हित idx = thr->शब्द[cp->bit_idx[0]].w_int;
      queue->erase_tail(idx);

      परत सत्य;
}

स्थिर तार्किक do_disable(vthread_t thr, vthread_t match)
{
      तार्किक flag = असत्य;

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
      पर्यंत (! thr->children.empty()) {

	    vthread_t tmp = *(thr->children.begin());
	    प्रतिज्ञा(tmp);
	    प्रतिज्ञा(tmp->parent == thr);
	    thr->i_am_joining = 0;
	    जर (do_disable(tmp, match))
		  flag = सत्य;

	    vthread_reap(tmp);
      }

      vthread_t parent = thr->parent;
      जर (parent && parent->i_am_joining) {
	      // If a parent is waiting in a %join, wake it up. Note
	      // that it is possible to be waiting in a %join yet
	      // already scheduled if multiple child threads are
	      // ending. So check if the thread is already scheduled
	      // before scheduling it again.
	    parent->i_am_joining = 0;
	    जर (! parent->i_have_ended)
		  schedule_vthread(parent, 0, सत्य);

	    do_join(parent, thr);

      } अन्यथा जर (parent) {
	      /* If the parent is yet to %join me, let its %join
		 do the reaping. */
	      //assert(tmp->is_scheduled == 0);

      } अन्यथा {
	      /* No parent at all. Goodbye. */
	    vthread_reap(thr);
      }

      परत flag || (thr == match);
}

/*
 * Implement the %disable instruction by scanning the target scope for
 * all the target threads. Kill the target threads and wake up a
 * parent that is attempting a %join.
 */
तार्किक of_DISABLE(vthread_t thr, vvp_code_t cp)
{
      __vpiScope*scope = static_cast<__vpiScope*>(cp->handle);

      तार्किक disabled_myself_flag = असत्य;

      पर्यंत (! scope->threads.empty()) {
	    set<vthread_t>::iterator cur = scope->threads.begin();

	    जर (do_disable(*cur, thr))
		  disabled_myself_flag = सत्य;
      }

      परत ! disabled_myself_flag;
}

/*
 * Similar to `of_DISABLE`. But will only disable a single thread of the
 * specified scope. The disabled thread will be the thread closest to the
 * current thread in thread hierarchy. This can either be the current thread,
 * either the thread itself or one of its parents.
 * This is used for SystemVerilog flow control instructions like `return`,
 * `continue` and `break`.
 */

तार्किक of_DISABLE_FLOW(vthread_t thr, vvp_code_t cp)
{
      स्थिरांक __vpiScope*scope = static_cast<__vpiScope*>(cp->handle);
      vthread_t cur = thr;

      पर्यंत (cur && cur->parent_scope != scope)
	    cur = cur->parent;

      प्रतिज्ञा(cur);
      जर (cur)
	    परत !do_disable(cur, thr);

      परत असत्य;
}

/*
 * Implement the %disable/fork (SystemVerilog) instruction by disabling
 * all the detached children of the given thread.
 */
तार्किक of_DISABLE_FORK(vthread_t thr, vvp_code_t)
{
	/* If a %disable/fork is being executed then the parent thread
	 * cannot be waiting in a join. */
      प्रतिज्ञा(! thr->i_am_joining);

	/* There should be no active children to disable. */
      प्रतिज्ञा(thr->children.empty());

	/* Disable any detached children. */
      पर्यंत (! thr->detached_children.empty()) {
	    vthread_t child = *(thr->detached_children.begin());
	    प्रतिज्ञा(child);
	    प्रतिज्ञा(child->parent == thr);
	      /* Disabling the children can never match the parent thread. */
	    तार्किक res = do_disable(child, thr);
	    प्रतिज्ञा(! res);
	    vthread_reap(child);
      }

      परत सत्य;
}

/*
 * This function divides a 2-word number {high, a} by a 1-word
 * number. Assume that high < b.
 */
स्थिर अचिन्हित दीर्घ divide2words(अचिन्हित दीर्घ a, अचिन्हित दीर्घ b,
				  अचिन्हित दीर्घ वरचा)
{
      अचिन्हित दीर्घ result = 0;
      पर्यंत (वरचा > 0) {
	    अचिन्हित दीर्घ tmp_result = अच_दीर्घ_उच्च / b;
	    अचिन्हित दीर्घ remain = अच_दीर्घ_उच्च % b;

	    remain += 1;
	    जर (remain >= b) {
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
	    result += tmp_result * वरचा;

	      // The new iteration starts with high*remain + a.
	    remain = multiply_with_carry(वरचा, remain, वरचा);
	    a += remain;
            जर(a < remain)
              वरचा += 1;

	      // Now result*b + {high,a} == the input {high,a}. It is
	      // possible that the new high >= 1. If so, it will
	      // certainly be less than high from the previous
	      // iteration. Do another iteration and it will shrink,
	      // eventually to 0.
      }

	// high is now 0, so a is the remaining remainder, so we can
	// finish off the integer divide with a simple a/b.

      परत result + a/b;
}

स्थिर अचिन्हित दीर्घ* divide_bits(अचिन्हित दीर्घ*ap, स्थिरांक अचिन्हित दीर्घ*bp, अचिन्हित wid)
{
	// Do all our work a cpu-word at a time. The "words" variable
	// is the number of words of the wid.
      अचिन्हित शब्द = (wid+CPU_WORD_BITS-1) / CPU_WORD_BITS;

      अचिन्हित btop = शब्द-1;
      पर्यंत (btop > 0 && bp[btop] == 0)
	    btop -= 1;

	// Detect divide by 0, and exit.
      जर (btop==0 && bp[0]==0)
	    परत 0;

	// The result array will eventually accumulate the result. The
	// diff array is a difference that we use in the intermediate.
      अचिन्हित दीर्घ*diff  = नवीन अचिन्हित दीर्घ[शब्द];
      अचिन्हित दीर्घ*result= नवीन अचिन्हित दीर्घ[शब्द];
      साठी (अचिन्हित idx = 0 ; idx < शब्द ; idx += 1)
	    result[idx] = 0;

      साठी (अचिन्हित cur = शब्द-btop ; cur > 0 ; cur -= 1) {
	    अचिन्हित cur_ptr = cur-1;
	    अचिन्हित दीर्घ cur_res;
	    जर (ap[cur_ptr+btop] >= bp[btop]) {
		  अचिन्हित दीर्घ वरचा = 0;
		  जर (cur_ptr+btop+1 < शब्द)
			वरचा = ap[cur_ptr+btop+1];
		  cur_res = divide2words(ap[cur_ptr+btop], bp[btop], वरचा);

	    } अन्यथा जर (cur_ptr+btop+1 >= शब्द) {
		  सुरूठेवा;

	    } अन्यथा जर (ap[cur_ptr+btop+1] == 0) {
		  सुरूठेवा;

	    } अन्यथा {
		  cur_res = divide2words(ap[cur_ptr+btop], bp[btop],
					 ap[cur_ptr+btop+1]);
	    }

	      // cur_res is a guesstimate of the result this far. It
	      // may be 1 too big. (But it will also be >0) Try it,
	      // and if the difference comes out negative, then adjust.

	      // diff = (bp * cur_res)  << cur_ptr;
	    multiply_array_imm(diff+cur_ptr, bp, शब्द-cur_ptr, cur_res);
	      // ap -= diff
	    अचिन्हित दीर्घ carry = 1;
	    साठी (अचिन्हित idx = cur_ptr ; idx < शब्द ; idx += 1)
		  ap[idx] = add_with_carry(ap[idx], ~diff[idx], carry);

	      // ap has the diff subtracted out of it. If cur_res was
	      // too large, then ap will turn negative. (We easily
	      // tell that ap turned negative by looking at
	      // carry&1. If it is 0, then it is *negative*.) In that
	      // case, we know that cur_res was too large by 1. Correct by
	      // adding 1b back in and reducing cur_res.
	    जर ((carry&1) == 0) {
		    // Keep adding b back in until the remainder
		    // becomes positive again.
		  करा {
			cur_res -= 1;
			carry = 0;
			साठी (अचिन्हित idx = cur_ptr ; idx < शब्द ; idx += 1)
			      ap[idx] = add_with_carry(ap[idx], bp[idx-cur_ptr], carry);
		  } पर्यंत (carry == 0);
	    }

	    result[cur_ptr] = cur_res;
      }

	// Now ap contains the remainder and result contains the
	// desired result. We should find that:
	//  input-a = bp * result + ap;

      delete[]diff;
      परत result;
}

/*
 * %div
 */
तार्किक of_DIV(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valb = thr->काढा_vec4();
      vvp_vector4_t vala = thr->काढा_vec4();

      प्रतिज्ञा(vala.size()== valb.size());
      अचिन्हित wid = vala.size();

      अचिन्हित दीर्घ*ap = vala.subarray(0, wid);
      जर (ap == 0) {
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->ढकला_vec4(tmp);
	    परत सत्य;
      }

      अचिन्हित दीर्घ*bp = valb.subarray(0, wid);
      जर (bp == 0) {
	    delete[]ap;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->ढकला_vec4(tmp);
	    परत सत्य;
      }

	// If the value fits in a single CPU word, then do it the easy way.
      जर (wid <= CPU_WORD_BITS) {
	    जर (bp[0] == 0) {
		  vvp_vector4_t tmp(wid, BIT4_X);
		  thr->ढकला_vec4(tmp);
	    } अन्यथा {
		  ap[0] /= bp[0];
		  vala.setarray(0, wid, ap);
		  thr->ढकला_vec4(vala);
	    }
	    delete[]ap;
	    delete[]bp;
	    परत सत्य;
      }

      अचिन्हित दीर्घ*result = divide_bits(ap, bp, wid);
      जर (result == 0) {
	    delete[]ap;
	    delete[]bp;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->ढकला_vec4(tmp);
	    परत सत्य;
      }

	// Now ap contains the remainder and result contains the
	// desired result. We should find that:
	//  input-a = bp * result + ap;

      vala.setarray(0, wid, result);
      thr->ढकला_vec4(vala);
      delete[]ap;
      delete[]bp;
      delete[]result;

      परत सत्य;
}


स्थिर शून्य negate_शब्द(अचिन्हित दीर्घ*val, अचिन्हित शब्द)
{
      अचिन्हित दीर्घ carry = 1;
      साठी (अचिन्हित idx = 0 ; idx < शब्द ; idx += 1)
	    val[idx] = add_with_carry(0, ~val[idx], carry);
}

/*
 * %div/s
 */
तार्किक of_DIV_S(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valb = thr->काढा_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      प्रतिज्ञा(vala.size()== valb.size());
      अचिन्हित wid = vala.size();
      अचिन्हित शब्द = (wid + CPU_WORD_BITS - 1) / CPU_WORD_BITS;

	// Get the values, left in right, in binary form. If there is
	// a problem with either (caused by an X or Z bit) then we
	// know right away that the entire result is X.
      अचिन्हित दीर्घ*ap = vala.subarray(0, wid);
      जर (ap == 0) {
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    परत सत्य;
      }

      अचिन्हित दीर्घ*bp = valb.subarray(0, wid);
      जर (bp == 0) {
	    delete[]ap;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    परत सत्य;
      }

	// Sign extend the bits in the array to fill out the array.
      अचिन्हित दीर्घ sign_mask = 0;
      जर (अचिन्हित दीर्घ sign_bits = (शब्द*CPU_WORD_BITS) - wid) {
	    sign_mask = -1UL << (CPU_WORD_BITS-sign_bits);
	    जर (ap[शब्द-1] & (sign_mask>>1))
		  ap[शब्द-1] |= sign_mask;
	    जर (bp[शब्द-1] & (sign_mask>>1))
		  bp[शब्द-1] |= sign_mask;
      }

	// If the value fits in a single word, then use the native divide.
      जर (wid <= CPU_WORD_BITS) {
	    जर (bp[0] == 0) {
		  vvp_vector4_t tmp(wid, BIT4_X);
		  vala = tmp;
	    } अन्यथा जर (((दीर्घ)ap[0] == दीर्घ_न्यून) && ((दीर्घ)bp[0] == -1)) {
		  vvp_vector4_t tmp(wid, BIT4_0);
		  tmp.set_bit(wid-1, BIT4_1);
		  vala = tmp;
	    } अन्यथा {
		  दीर्घ tmpa = (दीर्घ) ap[0];
		  दीर्घ tmpb = (दीर्घ) bp[0];
		  दीर्घ res = tmpa / tmpb;
		  ap[0] = ((अचिन्हित दीर्घ)res) & ~sign_mask;
		  vala.setarray(0, wid, ap);
	    }
	    delete[]ap;
	    delete[]bp;
	    परत सत्य;
      }

	// We need to the actual division to positive integers. Make
	// them positive here, and remember the negations.
      तार्किक negate_flag = असत्य;
      जर ( ((दीर्घ) ap[शब्द-1]) < 0 ) {
	    negate_flag = सत्य;
	    negate_शब्द(ap, शब्द);
      }
      जर ( ((दीर्घ) bp[शब्द-1]) < 0 ) {
	    negate_flag ^= सत्य;
	    negate_शब्द(bp, शब्द);
      }

      अचिन्हित दीर्घ*result = divide_bits(ap, bp, wid);
      जर (result == 0) {
	    delete[]ap;
	    delete[]bp;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    परत सत्य;
      }

      जर (negate_flag) {
	    negate_शब्द(result, शब्द);
      }

      result[शब्द-1] &= ~sign_mask;

      vala.setarray(0, wid, result);
      delete[]ap;
      delete[]bp;
      delete[]result;
      परत सत्य;
}

तार्किक of_DIV_WR(vthread_t thr, vvp_code_t)
{
      द्विगुण r = thr->काढा_real();
      द्विगुण l = thr->काढा_real();
      thr->ढकला_real(l / r);

      परत सत्य;
}

/*
 * %dup/obj
 * %dup/real
 * %dup/vec4
 *
 * Push a duplicate of the object on the appropriate stack.
 */
तार्किक of_DUP_OBJ(vthread_t thr, vvp_code_t)
{
      vvp_object_t src = thr->peek_object();

        // If it is null push a new null object
      जर (src.test_nil())
	    thr->ढकला_object(vvp_object_t());
      अन्यथा
	    thr->ढकला_object(src.duplicate());

      परत सत्य;
}

तार्किक of_DUP_REAL(vthread_t thr, vvp_code_t)
{
      thr->ढकला_real(thr->peek_real(0));
      परत सत्य;
}

तार्किक of_DUP_VEC4(vthread_t thr, vvp_code_t)
{
      thr->ढकला_vec4(thr->peek_vec4(0));
      परत सत्य;
}

/*
 * This terminates the current thread. If there is a parent who is
 * waiting for me to die, then I schedule it. At any rate, I mark
 * myself as a zombie by setting my pc to 0.
 */
तार्किक of_END(vthread_t thr, vvp_code_t)
{
      प्रतिज्ञा(! thr->waiting_for_event);
      thr->i_have_ended = 1;
      thr->pc = codespace_null();

	/* Fully detach any detached children. */
      पर्यंत (! thr->detached_children.empty()) {
	    vthread_t child = *(thr->detached_children.begin());
	    प्रतिज्ञा(child);
	    प्रतिज्ञा(child->parent == thr);
	    प्रतिज्ञा(child->i_am_detached);
	    child->parent = 0;
	    child->i_am_detached = 0;
	    thr->detached_children.erase(thr->detached_children.begin());
      }

	/* It is an error to still have active children running at this
	 * point in time. They should have all been detached or joined. */
      प्रतिज्ञा(thr->children.empty());

	/* If I have a parent who is waiting for me, then mark that I
	   have ended, and schedule that parent. Also, finish the
	   %join for the parent. */
      जर (!thr->i_am_detached && thr->parent && thr->parent->i_am_joining) {
	    vthread_t tmp = thr->parent;
	    प्रतिज्ञा(! thr->i_am_detached);

	    tmp->i_am_joining = 0;
	    schedule_vthread(tmp, 0, सत्य);
	    do_join(tmp, thr);
	    परत असत्य;
      }

	/* If this thread is not fully detached then remove it from the
	 * parents detached_children set and reap it. */
      जर (thr->i_am_detached) {
	    vthread_t tmp = thr->parent;
	    प्रतिज्ञा(tmp);
	    माप_प्रकार res = tmp->detached_children.erase(thr);
	    प्रतिज्ञा(res == 1);
	      /* If the parent is waiting for the detached children to
	       * finish then the last detached child needs to tell the
	       * parent to wake up when it is finished. */
	    जर (tmp->i_am_waiting && tmp->detached_children.empty()) {
		  tmp->i_am_waiting = 0;
		  schedule_vthread(tmp, 0, सत्य);
	    }
	      /* Fully detach this thread so it will be reaped below. */
	    thr->i_am_detached = 0;
	    thr->parent = 0;
      }

	/* If I have no parent, then no one can %join me and there is
	 * no reason to stick around. This can happen, for example if
	 * I am an ``initial'' thread. */
      जर (thr->parent == 0) {
	    vthread_reap(thr);
	    परत असत्य;
      }

	/* If I make it this far, then I have a parent who may wish
	   to %join me. Remain a zombie so that it can. */

      परत असत्य;
}

/*
 * %event <var-label>
 */
तार्किक of_EVENT(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_vector4_t tmp (1, BIT4_X);
      vvp_send_vec4(ptr, tmp, thr->wt_context);
      परत सत्य;
}

/*
 * %event/nb <var-label>, <delay>
 */
तार्किक of_EVENT_NB(vthread_t thr, vvp_code_t cp)
{
      vvp_time64_t delay;

      delay = thr->शब्द[cp->bit_idx[0]].w_uint;
      schedule_propagate_event(cp->net, delay);
      परत सत्य;
}

तार्किक of_EVCTL(vthread_t thr, vvp_code_t cp)
{
      प्रतिज्ञा(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      thr->ecount = thr->शब्द[cp->bit_idx[0]].w_uint;
      परत सत्य;
}
तार्किक of_EVCTLC(vthread_t thr, vvp_code_t)
{
      thr->event = 0;
      thr->ecount = 0;
      परत सत्य;
}

तार्किक of_EVCTLI(vthread_t thr, vvp_code_t cp)
{
      प्रतिज्ञा(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      thr->ecount = cp->bit_idx[0];
      परत सत्य;
}

तार्किक of_EVCTLS(vthread_t thr, vvp_code_t cp)
{
      प्रतिज्ञा(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      int64_t val = thr->शब्द[cp->bit_idx[0]].w_int;
      जर (val < 0) val = 0;
      thr->ecount = val;
      परत सत्य;
}

तार्किक of_FLAG_GET_VEC4(vthread_t thr, vvp_code_t cp)
{
      पूर्णांक flag = cp->संख्या;
      प्रतिज्ञा(flag < vthread_s::FLAGS_मोजणी);

      vvp_vector4_t val (1, thr->flags[flag]);
      thr->ढकला_vec4(val);

      परत सत्य;
}

/*
 * %flag_inv <flag1>
 */
तार्किक of_FLAG_INV(vthread_t thr, vvp_code_t cp)
{
      पूर्णांक flag1 = cp->bit_idx[0];

      thr->flags[flag1] = ~ thr->flags[flag1];
      परत सत्य;
}

/*
 * %flag_mov <flag1>, <flag2>
 */
तार्किक of_FLAG_MOV(vthread_t thr, vvp_code_t cp)
{
      पूर्णांक flag1 = cp->bit_idx[0];
      पूर्णांक flag2 = cp->bit_idx[1];

      thr->flags[flag1] = thr->flags[flag2];
      परत सत्य;
}

/*
 * %flag_or <flag1>, <flag2>
 */
तार्किक of_FLAG_OR(vthread_t thr, vvp_code_t cp)
{
      पूर्णांक flag1 = cp->bit_idx[0];
      पूर्णांक flag2 = cp->bit_idx[1];

      thr->flags[flag1] = thr->flags[flag1] | thr->flags[flag2];
      परत सत्य;
}

तार्किक of_FLAG_SET_IMM(vthread_t thr, vvp_code_t cp)
{
      पूर्णांक flag = cp->संख्या;
      पूर्णांक vali = cp->bit_idx[0];

      प्रतिज्ञा(flag < vthread_s::FLAGS_मोजणी);
      प्रतिज्ञा(vali >= 0 && vali < 4);

      स्थिर स्थिरांक vvp_bit4_t map_bit[4] = {BIT4_0, BIT4_1, BIT4_Z, BIT4_X};
      thr->flags[flag] = map_bit[vali];
      परत सत्य;
}

तार्किक of_FLAG_SET_VEC4(vthread_t thr, vvp_code_t cp)
{
      पूर्णांक flag = cp->संख्या;
      प्रतिज्ञा(flag < vthread_s::FLAGS_मोजणी);

      स्थिरांक vvp_vector4_t&val = thr->peek_vec4();
      thr->flags[flag] = val.value(0);
      thr->काढा_vec4(1);

      परत सत्य;
}

/*
 * the %force/link instruction connects a source node to a
 * destination node. The destination node must be a signal, as it is
 * marked with the source of the force so that it may later be
 * unlinked without specifically knowing the source that this
 * instruction used.
 */
तार्किक of_FORCE_LINK(vthread_t, vvp_code_t cp)
{
      vvp_net_t*dst = cp->net;
      vvp_net_t*src = cp->net2;

      प्रतिज्ञा(dst->fil);
      dst->fil->force_link(dst, src);

      परत सत्य;
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
तार्किक of_FORCE_VEC4(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_vector4_t value = thr->काढा_vec4();

	/* Send the force value to the filter on the node. */

      प्रतिज्ञा(net->fil);
      जर (value.size() != net->fil->filter_size())
	    value = coerce_to_width(value, net->fil->filter_size());

      net->force_vec4(value, vvp_vector2_t(vvp_vector2_t::FILL1, net->fil->filter_size()));

      परत सत्य;
}

/*
 * %force/vec4/off <net>, <off>
 */
तार्किक of_FORCE_VEC4_OFF(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      अचिन्हित base_idx = cp->bit_idx[0];
      दीर्घ base = thr->शब्द[base_idx].w_int;
      vvp_vector4_t value = thr->काढा_vec4();
      अचिन्हित wid = value.size();

      प्रतिज्ञा(net->fil);

      जर (thr->flags[4] == BIT4_1)
	    परत सत्य;

	// This is the width of the target vector.
      अचिन्हित use_size = net->fil->filter_size();

      जर (base >= (दीर्घ)use_size)
	    परत सत्य;
      जर (base < -(दीर्घ)use_size)
	    परत सत्य;

      जर ((base + wid) > use_size)
	    wid = use_size - base;

	// Make a mask of which bits are to be forced, 0 for unforced
	// bits and 1 for forced bits.
      vvp_vector2_t mask (vvp_vector2_t::FILL0, use_size);
      साठी (अचिन्हित idx = 0 ; idx < wid ; idx += 1)
	    mask.set_bit(base+idx, 1);

      vvp_vector4_t tmp (use_size, BIT4_Z);

	// vvp_net_t::force_vec4 propagates all the bits of the
	// forced vector value, regardless of the mask. This
	// ensures the unforced bits retain their current value.
      vvp_signal_मूल्य*sig = dynamic_cast<vvp_signal_मूल्य*>(net->fil);
      प्रतिज्ञा(sig);
      sig->vec4_मूल्य(tmp);

      tmp.set_vec(base, value);

      net->force_vec4(tmp, mask);
      परत सत्य;
}

/*
 * %force/vec4/off/d <net>, <off>, <del>
 */
तार्किक of_FORCE_VEC4_OFF_D(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      अचिन्हित base_idx = cp->bit_idx[0];
      दीर्घ base = thr->शब्द[base_idx].w_int;

      अचिन्हित delay_idx = cp->bit_idx[1];
      vvp_time64_t delay = thr->शब्द[delay_idx].w_uint;

      vvp_vector4_t value = thr->काढा_vec4();

      प्रतिज्ञा(net->fil);

      जर (thr->flags[4] == BIT4_1)
	    परत सत्य;

	// This is the width of the target vector.
      अचिन्हित use_size = net->fil->filter_size();

      जर (base >= (दीर्घ)use_size)
	    परत सत्य;
      जर (base < -(दीर्घ)use_size)
	    परत सत्य;

      schedule_force_vector(net, base, use_size, value, delay);
      परत सत्य;
}

तार्किक of_FORCE_WR(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net  = cp->net;
      द्विगुण value = thr->काढा_real();

      net->force_real(value, vvp_vector2_t(vvp_vector2_t::FILL1, 1));

      परत सत्य;
}

/*
 * The %fork instruction causes a new child to be created and pushed
 * in front of any existing child. This causes the new child to be
 * added to the list of children, and for me to be the parent of the
 * new child.
 */
तार्किक of_FORK(vthread_t thr, vvp_code_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);

      जर (cp->scope->is_automatic()) {
              /* The context allocated for this child is the top entry
                 on the write context stack. */
            child->wt_context = thr->wt_context;
            child->rd_context = thr->wt_context;
      }

      child->parent = thr;
      thr->children.insert(child);

      जर (thr->i_am_in_function) {
	    child->is_scheduled = 1;
	    child->i_am_in_function = 1;
	    vthread_run(child);
	    running_thread = thr;
      } अन्यथा {
	    schedule_vthread(child, 0, सत्य);
      }
      परत सत्य;
}

तार्किक of_FREE(vthread_t thr, vvp_code_t cp)
{
        /* Pop the child context from the read context stack. */
      vvp_context_t child_context = thr->rd_context;
      thr->rd_context = vvp_get_stacked_context(child_context);

        /* Free the context. */
      vthread_free_context(child_context, cp->scope);

      परत सत्य;
}

/*
 * %inv
 *
 * Logically, this pops a value, inverts is (Verilog style, with Z and
 * X converted to X) and pushes the result. We can more efficiently
 * just to the invert in place.
 */
तार्किक of_INV(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val.invert();
      परत सत्य;
}


/*
 * Index registers, arithmetic.
 */

स्थिर ओळीत int64_t get_as_64_bit(uint32_t खालचा_32, uint32_t वरचा_32)
{
      int64_t खालचा = खालचा_32;
      int64_t res = वरचा_32;

      res <<= 32;
      res |= खालचा;
      परत res;
}

तार्किक of_IX_ADD(vthread_t thr, vvp_code_t cp)
{
      thr->शब्द[cp->संख्या].w_int += get_as_64_bit(cp->bit_idx[0],
                                                    cp->bit_idx[1]);
      परत सत्य;
}

तार्किक of_IX_SUB(vthread_t thr, vvp_code_t cp)
{
      thr->शब्द[cp->संख्या].w_int -= get_as_64_bit(cp->bit_idx[0],
                                                    cp->bit_idx[1]);
      परत सत्य;
}

तार्किक of_IX_MUL(vthread_t thr, vvp_code_t cp)
{
      thr->शब्द[cp->संख्या].w_int *= get_as_64_bit(cp->bit_idx[0],
                                                    cp->bit_idx[1]);
      परत सत्य;
}

तार्किक of_IX_LOAD(vthread_t thr, vvp_code_t cp)
{
      thr->शब्द[cp->संख्या].w_int = get_as_64_bit(cp->bit_idx[0],
                                                   cp->bit_idx[1]);
      परत सत्य;
}

तार्किक of_IX_MOV(vthread_t thr, vvp_code_t cp)
{
      thr->शब्द[cp->bit_idx[0]].w_int = thr->शब्द[cp->bit_idx[1]].w_int;
      परत सत्य;
}

तार्किक of_IX_GETV(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित index = cp->bit_idx[0];
      vvp_net_t*net = cp->net;

      vvp_signal_मूल्य*sig = dynamic_cast<vvp_signal_मूल्य*>(net->fil);
      जर (sig == 0) {
	    प्रतिज्ञा(net->fil);
	    cerr << thr->get_fileline()
	         << "%%ix/getv error: Net arg not a vector signal? "
		 << typeid(*net->fil).name() << endl;
      }
      प्रतिज्ञा(sig);

      vvp_vector4_t vec;
      sig->vec4_मूल्य(vec);
      तार्किक overflow_flag;
      uint64_t val = 0;
      तार्किक known_flag = vector4_to_मूल्य(vec, overflow_flag, val);

      जर (known_flag)
	    thr->शब्द[index].w_uint = val;
      अन्यथा
	    thr->शब्द[index].w_uint = 0;

	/* Set bit 4 as a flag if the input is unknown. */
      thr->flags[4] = known_flag ? (overflow_flag ? BIT4_X : BIT4_0) : BIT4_1;

      परत सत्य;
}

तार्किक of_IX_GETV_S(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित index = cp->bit_idx[0];
      vvp_net_t*net = cp->net;

      vvp_signal_मूल्य*sig = dynamic_cast<vvp_signal_मूल्य*>(net->fil);
      जर (sig == 0) {
	    प्रतिज्ञा(net->fil);
	    cerr << thr->get_fileline()
	         << "%%ix/getv/s error: Net arg not a vector signal? "
		 << "fun=" << typeid(*net->fil).name()
		 << ", fil=" << (net->fil? typeid(*net->fil).name() : "<>")
		 << endl;
      }
      प्रतिज्ञा(sig);

      vvp_vector4_t vec;
      sig->vec4_मूल्य(vec);
      int64_t val;
      तार्किक known_flag = vector4_to_मूल्य(vec, val, सत्य, सत्य);

      जर (known_flag)
	    thr->शब्द[index].w_int = val;
      अन्यथा
	    thr->शब्द[index].w_int = 0;

	/* Set bit 4 as a flag if the input is unknown. */
      thr->flags[4] = known_flag? BIT4_0 : BIT4_1;

      परत सत्य;
}

स्थिर uint64_t vec4_to_index(vthread_t thr, तार्किक signed_flag)
{
	// Get all the information we need about the vec4 vector, then
	// pop it away. We only need the bool bits and the length.
      स्थिरांक vvp_vector4_t&val = thr->peek_vec4();
      अचिन्हित val_size = val.size();
      अचिन्हित दीर्घ*bits = val.subarray(0, val_size, असत्य);
      thr->काढा_vec4(1);

	// If there are X/Z bits, then the subarray will give us a nil
	// pointer. Set a flag to indicate the error, and give up.
      जर (bits == 0) {
	    thr->flags[4] = BIT4_1;
	    परत 0;
      }

      uint64_t v = 0;
      thr->flags[4] = BIT4_0;

      प्रतिज्ञा(आकार(bits[0]) <= आकार(v));

      v = 0;
      साठी (अचिन्हित idx = 0 ; idx < val_size ; idx += 8*आकार(bits[0])) {
	    uint64_t tmp = bits[idx/8/आकार(bits[0])];
	    जर (idx < 8*आकार(v)) {
		  v |= tmp << idx;
	    } अन्यथा {
		  तार्किक overflow = signed_flag && (v >> 63) ? ~tmp != 0 : tmp != 0;
		  जर (overflow) {
			thr->flags[4] = BIT4_X;
			थांबा;
		  }
	    }
      }

	// Set the high bits that are not necessarily filled in by the
	// subarray function.
      जर (val_size < 8*आकार(v)) {
	    जर (signed_flag && (v & (static_cast<uint64_t>(1)<<(val_size-1)))) {
		    // Propagate the sign bit...
		  v |= (~static_cast<uint64_t>(0)) << val_size;

	    } अन्यथा {
		    // Fill with zeros.
		  v &= ~((~static_cast<uint64_t>(0)) << val_size);
	    }

      }

      delete[]bits;
      परत v;
}

/*
 * %ix/vec4 <idx>
 */
तार्किक of_IX_VEC4(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित use_idx = cp->संख्या;
      thr->शब्द[use_idx].w_uint = vec4_to_index(thr, असत्य);
      परत सत्य;
}

/*
 * %ix/vec4/s <idx>
 */
तार्किक of_IX_VEC4_S(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित use_idx = cp->संख्या;
      thr->शब्द[use_idx].w_uint = vec4_to_index(thr, सत्य);
      परत सत्य;
}

/*
 * The various JMP instruction work simply by pulling the new program
 * counter from the instruction and resuming. If the jump is
 * conditional, then test the bit for the expected value first.
 */
तार्किक of_JMP(vthread_t thr, vvp_code_t cp)
{
      thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      जर (schedule_stopped()) {
	    schedule_vthread(thr, 0, असत्य);
	    परत असत्य;
      }

      परत सत्य;
}

/*
 * %jmp/0 <pc>, <flag>
 */
तार्किक of_JMP0(vthread_t thr, vvp_code_t cp)
{
      जर (thr->flags[cp->bit_idx[0]] == BIT4_0)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      जर (schedule_stopped()) {
	    schedule_vthread(thr, 0, असत्य);
	    परत असत्य;
      }

      परत सत्य;
}

/*
 * %jmp/0xz <pc>, <flag>
 */
तार्किक of_JMP0XZ(vthread_t thr, vvp_code_t cp)
{
      जर (thr->flags[cp->bit_idx[0]] != BIT4_1)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      जर (schedule_stopped()) {
	    schedule_vthread(thr, 0, असत्य);
	    परत असत्य;
      }

      परत सत्य;
}

/*
 * %jmp/1 <pc>, <flag>
 */
तार्किक of_JMP1(vthread_t thr, vvp_code_t cp)
{
      जर (thr->flags[cp->bit_idx[0]] == BIT4_1)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      जर (schedule_stopped()) {
	    schedule_vthread(thr, 0, असत्य);
	    परत असत्य;
      }

      परत सत्य;
}

/*
 * %jmp/1xz <pc>, <flag>
 */
तार्किक of_JMP1XZ(vthread_t thr, vvp_code_t cp)
{
      जर (thr->flags[cp->bit_idx[0]] != BIT4_0)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      जर (schedule_stopped()) {
	    schedule_vthread(thr, 0, असत्य);
	    परत असत्य;
      }

      परत सत्य;
}

/*
 * The %join instruction causes the thread to wait for one child
 * to die.  If a child is already dead (and a zombie) then I reap
 * it and go on. Otherwise, I mark myself as waiting in a join so that
 * children know to wake me when they finish.
 */

स्थिर शून्य do_join(vthread_t thr, vthread_t child)
{
      प्रतिज्ञा(child->parent == thr);

        /* If the immediate child thread is in an automatic scope... */
      जर (child->wt_context) {
              /* and is the top level task/function thread... */
            जर (thr->wt_context != thr->rd_context) {
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

स्थिर तार्किक do_join_opcode(vthread_t thr)
{
      प्रतिज्ञा( !thr->i_am_joining );
      प्रतिज्ञा( !thr->children.empty());

	// Are there any children that have already ended? If so, then
	// join with that one.
      साठी (set<vthread_t>::iterator cur = thr->children.begin()
		 ; cur != thr->children.end() ; ++cur) {
	    vthread_t curp = *cur;
	    जर (! curp->i_have_ended)
		  सुरूठेवा;

	      // found something!
	    do_join(thr, curp);
	    परत सत्य;
      }

	// Otherwise, tell my children to awaken me when they end,
	// then pause.
      thr->i_am_joining = 1;
      परत असत्य;
}

तार्किक of_JOIN(vthread_t thr, vvp_code_t)
{
      परत do_join_opcode(thr);
}

/*
 * This %join/detach <n> instruction causes the thread to detach
 * threads that were created by an earlier %fork.
 */
तार्किक of_JOIN_DETACH(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित दीर्घ मोजणी = cp->संख्या;

      प्रतिज्ञा(मोजणी == thr->children.size());

      पर्यंत (! thr->children.empty()) {
	    vthread_t child = *thr->children.begin();
	    प्रतिज्ञा(child->parent == thr);

	      // We cannot detach automatic tasks/functions within an
	      // automatic scope. If we try to do that, we might make
	      // a mess of the allocation of the context. Note that it
	      // is OK if the child context is distinct (See %exec_ufunc.)
	    प्रतिज्ञा(child->wt_context==0 || thr->wt_context!=child->wt_context);
	    जर (child->i_have_ended) {
		    // If the child has already ended, then reap it.
		  vthread_reap(child);

	    } अन्यथा {
		  माप_प्रकार res = child->parent->children.erase(child);
		  प्रतिज्ञा(res == 1);
		  child->i_am_detached = 1;
		  thr->detached_children.insert(child);
	    }
      }

      परत सत्य;
}

/*
 * %load/ar <array-label>, <index>;
*/
तार्किक of_LOAD_AR(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित idx = cp->bit_idx[0];
      द्विगुण word;

	/* The result is 0.0 if the address is undefined. */
      जर (thr->flags[4] == BIT4_1) {
	    word = 0.0;
      } अन्यथा {
	    अचिन्हित adr = thr->शब्द[idx].w_int;
	    word = cp->array->get_word_r(adr);
      }

      thr->ढकला_real(word);
      परत सत्य;
}

ढाँचा <typename ELEM>
स्थिर तार्किक load_dar(vthread_t thr, vvp_code_t cp)
{
      int64_t adr = thr->शब्द[3].w_int;
      vvp_net_t*net = cp->net;
      प्रतिज्ञा(net);

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      प्रतिज्ञा(obj);

      vvp_darray*darray = obj->get_object().peek<vvp_darray>();

      ELEM word;
      जर (darray &&
          (adr >= 0) && (thr->flags[4] == BIT4_0)) // A defined address >= 0
	    darray->get_word(adr, word);
      अन्यथा
	    dq_default(word, obj->size());

      vthread_ढकला(thr, word);
      परत सत्य;
}

/*
 * %load/dar/r <array-label>;
 */
तार्किक of_LOAD_DAR_R(vthread_t thr, vvp_code_t cp)
{
      परत load_dar<द्विगुण>(thr, cp);
}

/*
 * %load/dar/str <array-label>;
 */
तार्किक of_LOAD_DAR_STR(vthread_t thr, vvp_code_t cp)
{
      परत load_dar<string>(thr, cp);
}

/*
 * %load/dar/vec4 <array-label>;
 */
तार्किक of_LOAD_DAR_VEC4(vthread_t thr, vvp_code_t cp)
{
      परत load_dar<vvp_vector4_t>(thr, cp);
}

/*
 * %load/prop/dar/vec4 <pid>, <wid>;
 *   Indexed read of queue/dynamic-array class property; object on stack,
 *   index in words[3] (same as %load/dar/vec4).
 */
तार्किक of_LOAD_PROP_DAR_VEC4(vthread_t thr, vvp_code_t cp)
{
      माप_प्रकार pid = cp->संख्या;
      अचिन्हित wid = cp->bit_idx[0];
      int64_t adr = thr->शब्द[3].w_int;

      vvp_object_t& शीर्ष = thr->peek_object();
      vvp_cobject*cobj = शीर्ष.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      vvp_object_t pobj;
      cobj->get_object(pid, pobj, 0);
      vvp_darray*darray = pobj.peek<vvp_darray>();

      vvp_vector4_t word;
      जर (darray && (adr >= 0) && (thr->flags[4] == BIT4_0)) {
	    darray->get_word(adr, word);
      } अन्यथा {
	    dq_default(word, wid);
      }

      thr->ढकला_vec4(word);
      परत सत्य;
}

/*
 * %load/obj <var-label>
 */
तार्किक of_LOAD_OBJ(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      प्रतिज्ञा(fun);

      vvp_object_t val = fun->get_object();
      thr->ढकला_object(val);

      परत सत्य;
}

/*
 * %load/obja <index>
 *    Loads the object from array, using index <index> as the index
 *    value. If flags[4] == 1, the calculation of <index> may have
 *    failed, so push nil.
 */
तार्किक of_LOAD_OBJA(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित idx = cp->bit_idx[0];
      vvp_object_t word;

	/* The result is 0.0 if the address is undefined. */
      जर (thr->flags[4] == BIT4_1) {
	    ; // Return nil
      } अन्यथा {
	    अचिन्हित adr = thr->शब्द[idx].w_int;
	    cp->array->get_word_obj(adr, word);
      }

      thr->ढकला_object(word);
      परत सत्य;
}

/*
 * %load/real <var-label>
 */
तार्किक of_LOAD_REAL(vthread_t thr, vvp_code_t cp)
{
      __vpiHandle*tmp = cp->handle;
      t_vpi_मूल्य val;

      val.format = vpiRealVal;
      vpi_get_मूल्य(tmp, &val);

      thr->ढकला_real(val.value.real);

      परत सत्य;
}

/*
 * %load/str <var-label>
 */
तार्किक of_LOAD_STR(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;


      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (net->fun);
      प्रतिज्ञा(fun);

      स्थिरांक string&val = fun->get_string();
      thr->ढकला_str(val);

      परत सत्य;
}

तार्किक of_LOAD_STRA(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित idx = cp->bit_idx[0];
      string word;

      जर (thr->flags[4] == BIT4_1) {
	    word = "";
      } अन्यथा {
	    अचिन्हित adr = thr->शब्द[idx].w_int;
	    word = cp->array->get_word_str(adr);
      }

      thr->ढकला_str(word);
      परत सत्य;
}


/*
 * %load/vec4 <net>
 */
तार्किक of_LOAD_VEC4(vthread_t thr, vvp_code_t cp)
{
	// Push a placeholder onto the stack in order to reserve the
	// stack space. Use a reference for the stack top as a target
	// for the load.
      thr->ढकला_vec4(vvp_vector4_t());
      vvp_vector4_t&sig_मूल्य = thr->peek_vec4();

      vvp_net_t*net = cp->net;

	// For the %load to work, the functor must actually be a
	// signal functor. Only signals save their vector value.
      vvp_signal_मूल्य*sig = dynamic_cast<vvp_signal_मूल्य*> (net->fil);
      जर (sig == 0) {
	    cerr << thr->get_fileline()
	         << "%load/v error: Net arg not a signal? "
		 << (net->fil ? typeid(*net->fil).name() :
	                        typeid(*net->fun).name())
	         << endl;
	    प्रतिज्ञा(sig);
	    परत सत्य;
      }

	// Extract the value from the signal and directly into the
	// target stack position.
      sig->vec4_मूल्य(sig_मूल्य);

      परत सत्य;
}

/*
 * %load/vec4a <arr>, <adrx>
 */
तार्किक of_LOAD_VEC4A(vthread_t thr, vvp_code_t cp)
{
      पूर्णांक adr_index = cp->bit_idx[0];

      दीर्घ adr = thr->शब्द[adr_index].w_int;

	// If flag[3] is set, then the calculation of the address
	// failed, and this load should return X instead of the actual
	// value.
      जर (thr->flags[4] == BIT4_1) {
	    vvp_vector4_t tmp (cp->array->get_word_size(), BIT4_X);
	    thr->ढकला_vec4(tmp);
	    परत सत्य;
      }

      vvp_vector4_t tmp (cp->array->get_word(adr));
      thr->ढकला_vec4(tmp);
      परत सत्य;
}

स्थिर शून्य do_verylong_mod(vvp_vector4_t&vala, स्थिरांक vvp_vector4_t&valb,
			    तार्किक डावा_is_neg, तार्किक उजवा_is_neg)
{
      तार्किक out_is_neg = डावा_is_neg;
      स्थिरांक पूर्णांक len=vala.size();
      अचिन्हित अक्षर *a, *z, *t;
      a = नवीन अचिन्हित अक्षर[len+1];
      z = नवीन अचिन्हित अक्षर[len+1];
      t = नवीन अचिन्हित अक्षर[len+1];

      अचिन्हित अक्षर carry;
      अचिन्हित अक्षर temp;

      पूर्णांक mxa = -1, mxz = -1;
      पूर्णांक i;
      पूर्णांक current, copylen;

      अचिन्हित lb_carry = डावा_is_neg? 1 : 0;
      अचिन्हित rb_carry = उजवा_is_neg? 1 : 0;
      साठी (पूर्णांक idx = 0 ;  idx < len ;  idx += 1) {
	    अचिन्हित lb = vala.value(idx);
	    अचिन्हित rb = valb.value(idx);

	    जर ((lb | rb) & 2) {
		  delete []t;
		  delete []z;
		  delete []a;
		  vvp_vector4_t tmp(len, BIT4_X);
		  vala = tmp;
		  परत;
	    }

	    जर (डावा_is_neg) {
		  lb = (1-lb) + lb_carry;
		  lb_carry = (lb & ~1)? 1 : 0;
		  lb &= 1;
	    }
	    जर (उजवा_is_neg) {
		  rb = (1-rb) + rb_carry;
		  rb_carry = (rb & ~1)? 1 : 0;
		  rb &= 1;
	    }

	    z[idx]=lb;
	    a[idx]=1-rb;	// for 2s complement add..
      }

      z[len]=0;
      a[len]=1;

      साठी(i=len-1;i>=0;i--) {
	    जर(! a[i]) {
		  mxa=i;
		  थांबा;
	    }
      }

      साठी(i=len-1;i>=0;i--) {
	    जर(z[i]) {
		  mxz=i;
		  थांबा;
	    }
      }

      जर((mxa>mxz)||(mxa==-1)) {
	    जर(mxa==-1) {
		  delete []t;
		  delete []z;
		  delete []a;
		  vvp_vector4_t tmpx (len, BIT4_X);
		  vala = tmpx;
		  परत;
	    }

	    जा tally;
      }

      copylen = mxa + 2;
      current = mxz - mxa;

      पर्यंत(current > -1) {
	    carry = 1;
	    साठी(i=0;i<copylen;i++) {
		  temp = z[i+current] + a[i] + carry;
		  t[i] = (temp&1);
		  carry = (temp>>1);
	    }

	    जर(carry) {
		  साठी(i=0;i<copylen;i++) {
			z[i+current] = t[i];
		  }
	    }

	    current--;
      }

 tally:

      vvp_vector4_t tmp (len, BIT4_X);
      carry = out_is_neg? 1 : 0;
      साठी (पूर्णांक idx = 0 ;  idx < len ;  idx += 1) {
	    अचिन्हित ob = z[idx];
	    जर (out_is_neg) {
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

तार्किक of_MAX_WR(vthread_t thr, vvp_code_t)
{
      द्विगुण r = thr->काढा_real();
      द्विगुण l = thr->काढा_real();
      जर (r != r)
	    thr->ढकला_real(l);
      अन्यथा जर (l != l)
	    thr->ढकला_real(r);
      अन्यथा जर (r < l)
	    thr->ढकला_real(l);
      अन्यथा
	    thr->ढकला_real(r);
      परत सत्य;
}

तार्किक of_MIN_WR(vthread_t thr, vvp_code_t)
{
      द्विगुण r = thr->काढा_real();
      द्विगुण l = thr->काढा_real();
      जर (r != r)
	    thr->ढकला_real(l);
      अन्यथा जर (l != l)
	    thr->ढकला_real(r);
      अन्यथा जर (r < l)
	    thr->ढकला_real(r);
      अन्यथा
	    thr->ढकला_real(l);
      परत सत्य;
}

तार्किक of_MOD(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valb = thr->काढा_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      प्रतिज्ञा(vala.size()==valb.size());
      अचिन्हित wid = vala.size();

      जर(wid <= 8*आकार(अचिन्हित दीर्घ दीर्घ)) {
	    अचिन्हित दीर्घ दीर्घ lv = 0, rv = 0;

	    साठी (अचिन्हित idx = 0 ;  idx < wid ;  idx += 1) {
		  अचिन्हित दीर्घ दीर्घ lb = vala.value(idx);
		  अचिन्हित दीर्घ दीर्घ rb = valb.value(idx);

		  जर ((lb | rb) & 2)
			जा x_out;

		  lv |= (अचिन्हित दीर्घ दीर्घ) lb << idx;
		  rv |= (अचिन्हित दीर्घ दीर्घ) rb << idx;
	    }

	    जर (rv == 0)
		  जा x_out;

	    lv %= rv;

	    साठी (अचिन्हित idx = 0 ;  idx < wid ;  idx += 1) {
		  vala.set_bit(idx, (lv&1)?BIT4_1 : BIT4_0);
		  lv >>= 1;
	    }

	    परत सत्य;

      } अन्यथा {
	    do_verylong_mod(vala, valb, असत्य, असत्य);
	    परत सत्य;
      }

 x_out:
      vala = vvp_vector4_t(wid, BIT4_X);
      परत सत्य;
}

/*
 * %mod/s
 */
तार्किक of_MOD_S(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valb = thr->काढा_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      प्रतिज्ञा(vala.size()==valb.size());
      अचिन्हित wid = vala.size();

	/* Handle the case that we can fit the bits into a long-long
	   variable. We cause use native % to do the work. */
      जर(wid <= 8*आकार(दीर्घ दीर्घ)) {
	    दीर्घ दीर्घ lv = 0, rv = 0;

	    साठी (अचिन्हित idx = 0 ;  idx < wid ;  idx += 1) {
		  दीर्घ दीर्घ lb = vala.value(idx);
		  दीर्घ दीर्घ rb = valb.value(idx);

		  जर ((lb | rb) & 2)
			जा x_out;

		  lv |= (दीर्घ दीर्घ) lb << idx;
		  rv |= (दीर्घ दीर्घ) rb << idx;
	    }

	    जर (rv == 0)
		  जा x_out;

	    जर ((lv == LLONG_MIN) && (rv == -1))
		  जा zero_out;

	      /* Sign extend the signed operands when needed. */
	    जर (wid < 8*आकार(दीर्घ दीर्घ)) {
		  जर (lv & (1LL << (wid-1)))
			lv |= -1ULL << wid;
		  जर (rv & (1LL << (wid-1)))
			rv |= -1ULL << wid;
	    }

	    lv %= rv;

	    साठी (अचिन्हित idx = 0 ;  idx < wid ;  idx += 1) {
		  vala.set_bit(idx, (lv&1)? BIT4_1 : BIT4_0);
		  lv >>= 1;
	    }

	      // vala is the top of the stack, edited in place, so we
	      // do not need to push the result.

	    परत सत्य;

      } अन्यथा {

	    तार्किक डावा_is_neg  = vala.value(vala.size()-1) == BIT4_1;
	    तार्किक उजवा_is_neg = valb.value(valb.size()-1) == BIT4_1;
	    do_verylong_mod(vala, valb, डावा_is_neg, उजवा_is_neg);
	    परत सत्य;
      }

 x_out:
      vala = vvp_vector4_t(wid, BIT4_X);
      परत सत्य;
 zero_out:
      vala = vvp_vector4_t(wid, BIT4_0);
      परत सत्य;
}

/*
 * %mod/wr
 */
तार्किक of_MOD_WR(vthread_t thr, vvp_code_t)
{
      द्विगुण r = thr->काढा_real();
      द्विगुण l = thr->काढा_real();
      thr->ढकला_real(भ_शेष(l,r));

      परत सत्य;
}

/*
 * %neg/wr
 */
तार्किक of_NEG_WR(vthread_t thr, vvp_code_t)
{
      thr->poke_real(0, -thr->peek_real(0));
      परत सत्य;
}

/*
 * %pad/s <wid>
 */
तार्किक of_PAD_S(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित wid = cp->संख्या;

      vvp_vector4_t&val = thr->peek_vec4();
      अचिन्हित old_size = val.size();

	// Sign-extend.
      जर (old_size < wid)
	    val.resize(wid, val.value(old_size-1));
      अन्यथा
	    val.resize(wid);

      परत सत्य;
}

/*
 * %pad/u <wid>
 */
तार्किक of_PAD_U(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित wid = cp->संख्या;

      vvp_vector4_t&val = thr->peek_vec4();
      val.resize(wid, BIT4_0);

      परत सत्य;
}

/*
 * %part/s <wid>
 * %part/u <wid>
 * Two values are popped from the stack. First, pop the canonical
 * index of the part select, and second is the value to be
 * selected. The result is pushed back to the stack.
 */
स्थिर तार्किक of_PART_base(vthread_t thr, vvp_code_t cp, तार्किक signed_flag)
{
      अचिन्हित wid = cp->संख्या;

      vvp_vector4_t base4 = thr->काढा_vec4();
      vvp_vector4_t&value = thr->peek_vec4();

      vvp_vector4_t res (wid, BIT4_X);

	// NOTE: This is treating the vector as signed. Is that correct?
      int32_t base;
      तार्किक मूल्य_ok = vector4_to_मूल्य(base4, base, signed_flag);
      जर (! मूल्य_ok) {
	    value = res;
	    परत सत्य;
      }

      जर (base >= (int32_t)value.size()) {
	    value = res;
	    परत सत्य;
      }

      जर ((base+(पूर्णांक)wid) <= 0) {
	    value = res;
	    परत सत्य;
      }

      दीर्घ vbase = 0;
      जर (base < 0) {
	    vbase = -base;
	    wid -= vbase;
	    base = 0;
      }

      जर ((base+wid) > value.size()) {
	    wid = value.size() - base;
      }

      res .set_vec(vbase, value.subvalue(base, wid));
      value = res;

      परत सत्य;
}

तार्किक of_PART_S(vthread_t thr, vvp_code_t cp)
{
      परत of_PART_base(thr, cp, सत्य);
}

तार्किक of_PART_U(vthread_t thr, vvp_code_t cp)
{
      परत of_PART_base(thr, cp, असत्य);
}

/*
 * %parti/s <wid>, <basei>, <base_wid>
 * %parti/u <wid>, <basei>, <base_wid>
 *
 * Pop the value to be selected. The result is pushed back to the stack.
 */
स्थिर तार्किक of_PARTI_base(vthread_t thr, vvp_code_t cp, तार्किक signed_flag)
{
      अचिन्हित wid = cp->संख्या;
      uint32_t base = cp->bit_idx[0];
      uint32_t bwid = cp->bit_idx[1];

      vvp_vector4_t&value = thr->peek_vec4();

      vvp_vector4_t res (wid, BIT4_X);

	// NOTE: This is treating the vector as signed. Is that correct?
      int32_t use_base = base;
      जर (signed_flag && bwid < 32 && (base&(1<<(bwid-1)))) {
	    use_base |= -1UL << bwid;
      }

      जर (use_base >= (int32_t)value.size()) {
	    value = res;
	    परत सत्य;
      }

      जर ((use_base+(int32_t)wid) <= 0) {
	    value = res;
	    परत सत्य;
      }

      दीर्घ vbase = 0;
      जर (use_base < 0) {
	    vbase = -use_base;
	    wid -= vbase;
	    use_base = 0;
      }

      जर ((use_base+wid) > value.size()) {
	    wid = value.size() - use_base;
      }

      res .set_vec(vbase, value.subvalue(use_base, wid));
      value = res;

      परत सत्य;
}

तार्किक of_PARTI_S(vthread_t thr, vvp_code_t cp)
{
      परत of_PARTI_base(thr, cp, सत्य);
}

तार्किक of_PARTI_U(vthread_t thr, vvp_code_t cp)
{
      परत of_PARTI_base(thr, cp, असत्य);
}

/*
 * %mul
 */
तार्किक of_MUL(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t r = thr->काढा_vec4();
	// Rather then pop l, use it directly from the stack. When we
	// assign to 'l', that will edit the top of the stack, which
	// replaces a pop and a pull.
      vvp_vector4_t&l = thr->peek_vec4();

      l.mul(r);
      परत सत्य;
}

/*
 * %muli <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
तार्किक of_MULI(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित wid = cp->संख्या;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_immediate_rval (cp, r);

      l.mul(r);
      परत सत्य;
}

तार्किक of_MUL_WR(vthread_t thr, vvp_code_t)
{
      द्विगुण r = thr->काढा_real();
      द्विगुण l = thr->काढा_real();
      thr->ढकला_real(l * r);

      परत सत्य;
}

तार्किक of_NAND(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valr = thr->काढा_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      प्रतिज्ञा(vall.size() == valr.size());

      vall &= valr;
      vall.invert();

      परत सत्य;
}

/*
 * %new/cobj <vpi_object>
 * This creates a new cobject (SystemVerilog class object) and pushes
 * it to the stack. The <vpi-object> is a __vpiHandle that is a
 * vpiClassDefn object that defines the item to be created.
 */
तार्किक of_NEW_COBJ(vthread_t thr, vvp_code_t cp)
{
      स्थिरांक class_type*defn = dynamic_cast<स्थिरांक class_type*> (cp->handle);
      प्रतिज्ञा(defn);

      vvp_object_t tmp (नवीन vvp_cobject(defn));
      thr->ढकला_object(tmp);
      परत सत्य;
}

तार्किक of_NEW_DARRAY(vthread_t thr, vvp_code_t cp)
{
      स्थिरांक अक्षर*text = cp->text;
      माप_प्रकार size = thr->शब्द[cp->bit_idx[0]].w_int;
      अचिन्हित word_wid;
      माप_प्रकार n;

      vvp_object_t obj;
      जर (म_भेद(text,"b8") == 0) {
	    obj = नवीन vvp_darray_atom<uint8_t>(size);
      } अन्यथा जर (म_भेद(text,"b16") == 0) {
	    obj = नवीन vvp_darray_atom<uint16_t>(size);
      } अन्यथा जर (म_भेद(text,"b32") == 0) {
	    obj = नवीन vvp_darray_atom<uint32_t>(size);
      } अन्यथा जर (म_भेद(text,"b64") == 0) {
	    obj = नवीन vvp_darray_atom<uint64_t>(size);
      } अन्यथा जर (म_भेद(text,"sb8") == 0) {
	    obj = नवीन vvp_darray_atom<int8_t>(size);
      } अन्यथा जर (म_भेद(text,"sb16") == 0) {
	    obj = नवीन vvp_darray_atom<int16_t>(size);
      } अन्यथा जर (म_भेद(text,"sb32") == 0) {
	    obj = नवीन vvp_darray_atom<int32_t>(size);
      } अन्यथा जर (म_भेद(text,"sb64") == 0) {
	    obj = नवीन vvp_darray_atom<int64_t>(size);
      } अन्यथा जर ((1 == माला_पूछो(text, "b%u%zn", &word_wid, &n)) &&
                 (n == म_माप(text))) {
	    obj = नवीन vvp_darray_vec2(size, word_wid);
      } अन्यथा जर ((1 == माला_पूछो(text, "sb%u%zn", &word_wid, &n)) &&
                 (n == म_माप(text))) {
	    obj = नवीन vvp_darray_vec2(size, word_wid);
      } अन्यथा जर ((1 == माला_पूछो(text, "v%u%zn", &word_wid, &n)) &&
                 (n == म_माप(text))) {
	    obj = नवीन vvp_darray_vec4(size, word_wid);
      } अन्यथा जर ((1 == माला_पूछो(text, "sv%u%zn", &word_wid, &n)) &&
                 (n == म_माप(text))) {
	    obj = नवीन vvp_darray_vec4(size, word_wid);
      } अन्यथा जर (म_भेद(text,"r") == 0) {
	    obj = नवीन vvp_darray_real(size);
      } अन्यथा जर (म_भेद(text,"S") == 0) {
	    obj = नवीन vvp_darray_string(size);
      } अन्यथा {
	    cerr << get_fileline()
	         << "Internal error: Unsupported dynamic array type: "
	         << text << "." << endl;
	    प्रतिज्ञा(0);
      }

      thr->ढकला_object(obj);

      परत सत्य;
}

तार्किक of_NOOP(vthread_t, vvp_code_t)
{
      परत सत्य;
}

/*
 * %nor/r
 */
तार्किक of_NORR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_or());

      परत सत्य;
}

/*
 * Push a null to the object stack.
 */
तार्किक of_NULL(vthread_t thr, vvp_code_t)
{
      vvp_object_t tmp;
      thr->ढकला_object(tmp);
      परत सत्य;
}

/*
 * %and/r
 */
तार्किक of_ANDR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_and());

      परत सत्य;
}

/*
 * %nand/r
 */
तार्किक of_NANDR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_and());

      परत सत्य;
}

/*
 * %or/r
 */
तार्किक of_ORR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_or());

      परत सत्य;
}

/*
 * %xor/r
 */
तार्किक of_XORR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_xor());

      परत सत्य;
}

/*
 * %xnor/r
 */
तार्किक of_XNORR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_xor());

      परत सत्य;
}

/*
 * %or
 */
तार्किक of_OR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valb = thr->काढा_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();
      vala |= valb;
      परत सत्य;
}

/*
 * %nor
 */
तार्किक of_NOR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valr = thr->काढा_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      प्रतिज्ञा(vall.size() == valr.size());

      vall |= valr;
      vall.invert();

      परत सत्य;
}

/*
 * %pop/obj <num>, <skip>
 */
तार्किक of_काढा_OBJ(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित cnt = cp->bit_idx[0];
      अचिन्हित skip = cp->bit_idx[1];

      thr->काढा_object(cnt, skip);
      परत सत्य;
}

/*
 * %pop/real <number>
 */
तार्किक of_काढा_REAL(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित cnt = cp->संख्या;
      thr->काढा_real(cnt);
      परत सत्य;
}

/*
 *  %pop/str <number>
 */
तार्किक of_काढा_STR(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित cnt = cp->संख्या;
      thr->काढा_str(cnt);
      परत सत्य;
}

/*
 *  %pop/vec4 <number>
 */
तार्किक of_काढा_VEC4(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित cnt = cp->संख्या;
      thr->काढा_vec4(cnt);
      परत सत्य;
}

/*
 * %pow
 * %pow/s
 */
स्थिर तार्किक of_POW_base(vthread_t thr, तार्किक signed_flag)
{
      vvp_vector4_t valb = thr->काढा_vec4();
      vvp_vector4_t vala = thr->काढा_vec4();

      अचिन्हित wid = vala.size();

      vvp_vector2_t xv2 = vvp_vector2_t(vala, सत्य);
      vvp_vector2_t yv2 = vvp_vector2_t(valb, सत्य);


        /* If we have an X or Z in the arguments return X. */
      जर (xv2.is_NaN() || yv2.is_NaN()) {
	    vvp_vector4_t tmp (wid, BIT4_X);
	    thr->ढकला_vec4(tmp);
	    परत सत्य;
      }

	// Is the exponent negative? If so, table 5-6 in IEEE1364-2005
	// defines what value is returned.
      जर (signed_flag && yv2.value(yv2.size()-1)) {
	    पूर्णांक a_val;
	    vvp_bit4_t pad = BIT4_0, lsb = BIT4_0;
	    जर (vector2_to_मूल्य(xv2, a_val, सत्य)) {
		  जर (a_val == 0) {
			pad = BIT4_X; lsb = BIT4_X;
		  }
		  जर (a_val == 1) {
			pad = BIT4_0; lsb = BIT4_1;
		  }
		  जर (a_val == -1) {
			जर (yv2.value(0)) {
			      pad = BIT4_1; lsb = BIT4_1;
			} अन्यथा {
			      pad = BIT4_0; lsb = BIT4_1;
			}
		  }
	    }
	    vvp_vector4_t tmp (wid, pad);
	    tmp.set_bit(0, lsb);
	    thr->ढकला_vec4(tmp);
	    परत सत्य;
      }

      vvp_vector2_t result = घात(xv2, yv2);

        /* Copy only what we need of the result. If the result is too
	   small, zero-pad it. */
      साठी (अचिन्हित jdx = 0;  jdx < wid;  jdx += 1) {
	    जर (jdx >= result.size())
		  vala.set_bit(jdx, BIT4_0);
	    अन्यथा
		  vala.set_bit(jdx, result.value(jdx) ? BIT4_1 : BIT4_0);
      }
      thr->ढकला_vec4(vala);

      परत सत्य;
}

तार्किक of_POW(vthread_t thr, vvp_code_t)
{
      परत of_POW_base(thr, असत्य);
}

तार्किक of_POW_S(vthread_t thr, vvp_code_t)
{
      परत of_POW_base(thr, सत्य);
}

तार्किक of_POW_WR(vthread_t thr, vvp_code_t)
{
      द्विगुण r = thr->काढा_real();
      द्विगुण l = thr->काढा_real();
      thr->ढकला_real(घात(l,r));

      परत सत्य;
}

/*
 * %prop/obj <pid>, <idx>
 *
 * Load an object value from the cobject and push it onto the object stack.
 */
तार्किक of_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित pid = cp->संख्या;
      अचिन्हित idx = cp->bit_idx[0];

      जर (idx != 0) {
	    प्रतिज्ञा(idx < vthread_s::शब्द_मोजणी);
	    idx = thr->शब्द[idx].w_uint;
      }

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();

      vvp_object_t val;
      cobj->get_object(pid, val, idx);

      thr->ढकला_object(val);

      परत सत्य;
}

स्थिर शून्य get_from_obj(अचिन्हित pid, vvp_cobject*cobj, द्विगुण&val)
{
      val = cobj->get_real(pid);
}

स्थिर शून्य get_from_obj(अचिन्हित pid, vvp_cobject*cobj, string&val)
{
      val = cobj->get_string(pid);
}

स्थिर शून्य get_from_obj(अचिन्हित pid, vvp_cobject*cobj, vvp_vector4_t&val)
{
      cobj->get_vec4(pid, val);
}

ढाँचा <typename ELEM>
स्थिर तार्किक prop(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित pid = cp->संख्या;

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      ELEM val;
      get_from_obj(pid, cobj, val);
      vthread_ढकला(thr, val);

      परत सत्य;
}

/*
 * %prop/r <pid>
 *
 * Load a real value from the cobject and push it onto the real value
 * stack.
 */
तार्किक of_PROP_R(vthread_t thr, vvp_code_t cp)
{
      परत prop<द्विगुण>(thr, cp);
}

/*
 * %prop/str <pid>
 *
 * Load a string value from the cobject and push it onto the real value
 * stack.
 */
तार्किक of_PROP_STR(vthread_t thr, vvp_code_t cp)
{
      परत prop<string>(thr, cp);
}

/*
 * %prop/v <pid>
 *
 * Load a property <pid> from the cobject on the top of the stack into
 * the vector space at <base>.
 */
तार्किक of_PROP_V(vthread_t thr, vvp_code_t cp)
{
      परत prop<vvp_vector4_t>(thr, cp);
}

तार्किक of_PUSHI_REAL(vthread_t thr, vvp_code_t cp)
{
      द्विगुण mant = cp->bit_idx[0];
      uint32_t imant = cp->bit_idx[0];
      पूर्णांक exp = cp->bit_idx[1];

	// Detect +infinity
      जर (exp==0x3fff && imant==0) {
	    thr->ढकला_real(अनन्त);
	    परत सत्य;
      }
	// Detect -infinity
      जर (exp==0x7fff && imant==0) {
	    thr->ढकला_real(-अनन्त);
	    परत सत्य;
      }
	// Detect NaN
      जर (exp==0x3fff) {
	    thr->ढकला_real(nan(""));
	    परत सत्य;
      }

      द्विगुण sign = (exp & 0x4000)? -1.0 : 1.0;

      exp &= 0x1fff;

      mant = sign * ldexp(mant, exp - 0x1000);
      thr->ढकला_real(mant);
      परत सत्य;
}

तार्किक of_PUSHI_STR(vthread_t thr, vvp_code_t cp)
{
      स्थिरांक अक्षर*text = cp->text;
      thr->ढकला_str(filter_string(text));
      परत सत्य;
}

/*
 * %pushi/vec4 <vala>, <valb>, <wid>
 */
तार्किक of_PUSHI_VEC4(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित wid  = cp->संख्या;

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t val (wid, BIT4_0);
      get_immediate_rval (cp, val);

      thr->ढकला_vec4(val);

      परत सत्य;
}

/*
 * %pushv/str
 *   Pops a vec4 value, and pushes a string.
 */
तार्किक of_PUSHV_STR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t vec = thr->काढा_vec4();

      माप_प्रकार slen = (vec.size() + 7)/8;
      vector<अक्षर>buf;
      buf.reserve(slen);

      साठी (माप_प्रकार idx = 0 ; idx < vec.size() ; idx += 8) {
	    अक्षर tmp = 0;
	    माप_प्रकार trans = 8;
	    जर (idx+trans > vec.size())
		  trans = vec.size() - idx;

	    साठी (माप_प्रकार bdx = 0 ; bdx < trans ; bdx += 1) {
		  जर (vec.value(idx+bdx) == BIT4_1)
			tmp |= 1 << bdx;
	    }

	    जर (tmp != 0)
		  buf.push_back(tmp);
      }

      string val;
      साठी (vector<अक्षर>::reverse_iterator cur = buf.rbegin()
		 ; cur != buf.rend() ; ++cur) {
	    val.push_back(*cur);
      }

      thr->ढकला_str(val);

      परत सत्य;
}

/*
 * %putc/str/vec4 <var>, <mux>
 */
तार्किक of_PUTC_STR_VEC4(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित muxr = cp->bit_idx[0];
      int32_t mux = muxr? thr->शब्द[muxr].w_int : 0;

      vvp_vector4_t val = thr->काढा_vec4();
      प्रतिज्ञा(val.size() == 8);

      जर (mux < 0)
	    परत सत्य;

	/* Get the existing value of the string. If we find that the
	   index is too big for the string, then give up. */
      vvp_net_t*net = cp->net;
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (net->fun);
      प्रतिज्ञा(fun);

      string tmp = fun->get_string();
      जर (tmp.size() <= (माप_प्रकार)mux)
	    परत सत्य;

      अक्षर val_str = 0;
      साठी (माप_प्रकार idx = 0 ; idx < 8 ; idx += 1) {
	    जर (val.value(idx)==BIT4_1)
		  val_str |= 1<<idx;
      }

	// It is a quirk of the Verilog standard that putc(..., 'h00)
	// has no effect. Test for that case here.
      जर (val_str == 0)
	    परत सत्य;

      tmp[mux] = val_str;

      vvp_send_string(vvp_net_ptr_t(cp->net, 0), tmp, thr->wt_context);
      परत सत्य;
}

ढाँचा <typename ELEM, वर्ग QTYPE>
स्थिर तार्किक qinsert(vthread_t thr, vvp_code_t cp, अचिन्हित wid=0)
{
      int64_t idx = thr->शब्द[3].w_int;
      ELEM value;
      vvp_net_t*net = cp->net;
      काढा_मूल्य(thr, value, wid); // Pop the value to store.

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      प्रतिज्ञा(queue);
      जर (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at a negative "
	         << get_queue_type(value)
	         << " index (" << idx << "). ";
	    print_queue_मूल्य(value);
	    cerr << " was not added." << endl;
      } अन्यथा जर (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at an undefined "
	         << get_queue_type(value) << " index. ";
	    print_queue_मूल्य(value);
	    cerr << " was not added." << endl;
      } अन्यथा {
	    अचिन्हित max_size = thr->शब्द[cp->bit_idx[0]].w_int;
	    queue->insert(idx, value, max_size);
      }
      परत सत्य;
}

/*
 * %qinsert/real <var-label>
 */
तार्किक of_QINSERT_REAL(vthread_t thr, vvp_code_t cp)
{
      परत qinsert<द्विगुण, vvp_queue_real>(thr, cp);
}

/*
 * %qinsert/str <var-label>
 */
तार्किक of_QINSERT_STR(vthread_t thr, vvp_code_t cp)
{
      परत qinsert<string, vvp_queue_string>(thr, cp);
}

/*
 * %qinsert/v <var-label>
 */
तार्किक of_QINSERT_V(vthread_t thr, vvp_code_t cp)
{
      परत qinsert<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

/*
 * Helper functions used in the queue pop templates
 */
ओळीत शून्य ढकला_मूल्य(vthread_t thr, द्विगुण value, अचिन्हित)
{
      thr->ढकला_real(value);
}

ओळीत शून्य ढकला_मूल्य(vthread_t thr, स्थिरांक string&value, अचिन्हित)
{
      thr->ढकला_str(value);
}

ओळीत शून्य ढकला_मूल्य(vthread_t thr, स्थिरांक vvp_vector4_t&value, अचिन्हित wid)
{
      प्रतिज्ञा(wid == value.size());
      thr->ढकला_vec4(value);
}

ढाँचा <typename ELEM, वर्ग QTYPE>
स्थिर तार्किक q_काढा(vthread_t thr, vvp_code_t cp,
                  शून्य (*get_val_func)(vvp_queue*, ELEM&),
                  स्थिरांक अक्षर*loc, अचिन्हित wid)
{
      vvp_net_t*net = cp->net;

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      प्रतिज्ञा(queue);

      माप_प्रकार size = queue->get_size();

      ELEM value;
      जर (size) {
	    get_val_func(queue, value);
      } अन्यथा {
	    dq_default(value, wid);
	    cerr << thr->get_fileline()
	         << "Warning: pop_" << loc << "() on empty "
	         << get_queue_type(value) << "." << endl;
      }

      ढकला_मूल्य(thr, value, wid);
      परत सत्य;
}

ढाँचा <typename ELEM>
स्थिर शून्य get_back_मूल्य(vvp_queue*queue, ELEM&value)
{
      queue->get_word(queue->get_size()-1, value);
      queue->pop_back();
}

ढाँचा <typename ELEM, वर्ग QTYPE>
स्थिर तार्किक qpop_b(vthread_t thr, vvp_code_t cp, अचिन्हित wid=0)
{
      परत q_काढा<ELEM, QTYPE>(thr, cp, get_back_मूल्य<ELEM>, "back", wid);
}

/*
 * %qpop/b/real <var-label>
 */
तार्किक of_QPOP_B_REAL(vthread_t thr, vvp_code_t cp)
{
      परत qpop_b<द्विगुण, vvp_queue_real>(thr, cp);
}

/*
 * %qpop/b/str <var-label>
 */
तार्किक of_QPOP_B_STR(vthread_t thr, vvp_code_t cp)
{
      परत qpop_b<string, vvp_queue_string>(thr, cp);
}

/*
 * %qpop/b/v <var-label>
 */
तार्किक of_QPOP_B_V(vthread_t thr, vvp_code_t cp)
{
      परत qpop_b<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[0]);
}

ढाँचा <typename ELEM>
स्थिर शून्य get_front_मूल्य(vvp_queue*queue, ELEM&value)
{
      queue->get_word(0, value);
      queue->pop_front();
}

ढाँचा <typename ELEM, वर्ग QTYPE>
स्थिर तार्किक qpop_f(vthread_t thr, vvp_code_t cp, अचिन्हित wid=0)
{
      परत q_काढा<ELEM, QTYPE>(thr, cp, get_front_मूल्य<ELEM>, "front", wid);
}


/*
 * %qpop/f/real <var-label>
 */
तार्किक of_QPOP_F_REAL(vthread_t thr, vvp_code_t cp)
{
      परत qpop_f<द्विगुण, vvp_queue_real>(thr, cp);
}

/*
 * %qpop/f/str <var-label>
 */
तार्किक of_QPOP_F_STR(vthread_t thr, vvp_code_t cp)
{
      परत qpop_f<string, vvp_queue_string>(thr, cp);
}

/*
 * %qpop/f/v <var-label>
 */
तार्किक of_QPOP_F_V(vthread_t thr, vvp_code_t cp)
{
      परत qpop_f<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[0]);
}

/*
 * These implement the %release/net and %release/reg instructions. The
 * %release/net instruction applies to a net kind of functor by
 * sending the release/net command to the command port. (See vvp_net.h
 * for details.) The %release/reg instruction is the same, but sends
 * the release/reg command instead. These are very similar to the
 * %deassign instruction.
 */
स्थिर तार्किक do_release_vec(vvp_code_t cp, तार्किक net_flag)
{
      vvp_net_t*net = cp->net;
      अचिन्हित base  = cp->bit_idx[0];
      अचिन्हित width = cp->bit_idx[1];

      प्रतिज्ञा(net->fil);

      जर (base >= net->fil->filter_size()) परत सत्य;
      जर (base+width > net->fil->filter_size())
	    width = net->fil->filter_size() - base;

      तार्किक full_sig = base == 0 && width == net->fil->filter_size();

	// XXXX Can't really do this if this is a partial release?
      net->fil->force_unlink();

	/* Do we release all or part of the net? */
      vvp_net_ptr_t ptr (net, 0);
      जर (full_sig) {
	    net->fil->release(ptr, net_flag);
      } अन्यथा {
	    net->fil->release_pv(ptr, base, width, net_flag);
      }
      net->fun->force_flag(असत्य);

      परत सत्य;
}

तार्किक of_RELEASE_NET(vthread_t, vvp_code_t cp)
{
      परत do_release_vec(cp, सत्य);
}


तार्किक of_RELEASE_REG(vthread_t, vvp_code_t cp)
{
      परत do_release_vec(cp, असत्य);
}

/* The type is 1 for registers and 0 for everything else. */
तार्किक of_RELEASE_WR(vthread_t, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      अचिन्हित type  = cp->bit_idx[0];

      प्रतिज्ञा(net->fil);
      net->fil->force_unlink();

	// Send a command to this signal to unforce itself.
      vvp_net_ptr_t ptr (net, 0);
      net->fil->release(ptr, type==0);
      परत सत्य;
}

तार्किक of_REPLICATE(vthread_t thr, vvp_code_t cp)
{
      पूर्णांक rept = cp->संख्या;
      vvp_vector4_t val = thr->काढा_vec4();
      vvp_vector4_t res (val.size() * rept, BIT4_X);

      साठी (पूर्णांक idx = 0 ; idx < rept ; idx += 1) {
	    res.set_vec(idx * val.size(), val);
      }

      thr->ढकला_vec4(res);

      परत सत्य;
}

स्थिर शून्य poke_val(vthread_t fun_thr, अचिन्हित depth, द्विगुण val)
{
      fun_thr->parent->poke_real(depth, val);
}

स्थिर शून्य poke_val(vthread_t fun_thr, अचिन्हित depth, स्थिरांक string&val)
{
      fun_thr->parent->poke_str(depth, val);
}

स्थिर माप_प्रकार get_max(vthread_t fun_thr, द्विगुण&)
{
      परत fun_thr->args_real.size();
}

स्थिर माप_प्रकार get_max(vthread_t fun_thr, string&)
{
      परत fun_thr->args_str.size();
}

स्थिर माप_प्रकार get_max(vthread_t fun_thr, vvp_vector4_t&)
{
      परत fun_thr->args_vec4.size();
}

स्थिर अचिन्हित get_depth(vthread_t fun_thr, माप_प्रकार index, द्विगुण&)
{
      परत fun_thr->args_real[index];
}

स्थिर अचिन्हित get_depth(vthread_t fun_thr, माप_प्रकार index, string&)
{
      परत fun_thr->args_str[index];
}

स्थिर अचिन्हित get_depth(vthread_t fun_thr, माप_प्रकार index, vvp_vector4_t&)
{
      परत fun_thr->args_vec4[index];
}

स्थिर vthread_t get_func(vthread_t thr)
{
      vthread_t fun_thr = thr;

      पर्यंत (fun_thr->parent_scope->get_type_code() != vpiFunction) {
	    प्रतिज्ञा(fun_thr->parent);
	    fun_thr = fun_thr->parent;
      }

      परत fun_thr;
}

ढाँचा <typename ELEM>
स्थिर तार्किक ret(vthread_t thr, vvp_code_t cp)
{
      माप_प्रकार index = cp->संख्या;
      ELEM val;
      काढा_मूल्य(thr, val, 0);

      vthread_t fun_thr = get_func(thr);
      प्रतिज्ञा(index < get_max(fun_thr, val));

      अचिन्हित depth = get_depth(fun_thr, index, val);
	// Use the depth to put the value into the stack of
	// the parent thread.
      poke_val(fun_thr, depth, val);
      परत सत्य;
}

/*
 * %ret/real <index>
 */
तार्किक of_RET_REAL(vthread_t thr, vvp_code_t cp)
{
      परत ret<द्विगुण>(thr, cp);
}

/*
 * %ret/str <index>
 */
तार्किक of_RET_STR(vthread_t thr, vvp_code_t cp)
{
      परत ret<string>(thr, cp);
}

/*
 * %ret/vec4 <index>, <offset>, <wid>
 */
तार्किक of_RET_VEC4(vthread_t thr, vvp_code_t cp)
{
      माप_प्रकार index = cp->संख्या;
      अचिन्हित off_index = cp->bit_idx[0];
      अचिन्हित पूर्णांक wid = cp->bit_idx[1];
      vvp_vector4_t&val = thr->peek_vec4();

      vthread_t fun_thr = get_func(thr);
      प्रतिज्ञा(index < get_max(fun_thr, val));
      प्रतिज्ञा(val.size() == wid);
      अचिन्हित depth = get_depth(fun_thr, index, val);

      int64_t off = off_index ? thr->शब्द[off_index].w_int : 0;
      अचिन्हित पूर्णांक sig_मूल्य_size = fun_thr->parent->peek_vec4(depth).size();

      जर (off_index!=0 && thr->flags[4] == BIT4_1) {
	    thr->काढा_vec4(1);
	    परत सत्य;
      }

      जर (!resize_rval_vec(val, off, sig_मूल्य_size)) {
	    thr->काढा_vec4(1);
	    परत सत्य;
      }

      जर (off == 0 && val.size() == sig_मूल्य_size) {
	    fun_thr->parent->poke_vec4(depth, val);
      } अन्यथा {
	    vvp_vector4_t tmp_dst = fun_thr->parent->peek_vec4(depth);
	    tmp_dst.set_vec(off, val);
	    fun_thr->parent->poke_vec4(depth, tmp_dst);
      }

      thr->काढा_vec4(1);
      परत सत्य;
}

स्थिर शून्य ढकला_from_parent(vthread_t thr, vthread_t fun_thr, अचिन्हित depth, द्विगुण&)
{
      thr->ढकला_real(fun_thr->parent->peek_real(depth));
}

स्थिर शून्य ढकला_from_parent(vthread_t thr, vthread_t fun_thr, अचिन्हित depth, string&)
{
      thr->ढकला_str(fun_thr->parent->peek_str(depth));
}

स्थिर शून्य ढकला_from_parent(vthread_t thr, vthread_t fun_thr, अचिन्हित depth, vvp_vector4_t&)
{
      thr->ढकला_vec4(fun_thr->parent->peek_vec4(depth));
}

ढाँचा <typename ELEM>
स्थिर तार्किक retload(vthread_t thr, vvp_code_t cp)
{
      माप_प्रकार index = cp->संख्या;
      ELEM type;

      vthread_t fun_thr = get_func(thr);
      प्रतिज्ञा(index < get_max(fun_thr, type));

      अचिन्हित depth = get_depth(fun_thr, index, type);
	// Use the depth to extract the values from the stack
	// of the parent thread.
      ढकला_from_parent(thr, fun_thr, depth, type);
      परत सत्य;
}

/*
 * %retload/real <index>
 */
तार्किक of_RETLOAD_REAL(vthread_t thr, vvp_code_t cp)
{
      परत retload<द्विगुण>(thr, cp);
}

/*
 * %retload/str <index>
 */
तार्किक of_RETLOAD_STR(vthread_t thr, vvp_code_t cp)
{
      परत retload<string>(thr, cp);
}

/*
 * %retload/vec4 <index>
 */
तार्किक of_RETLOAD_VEC4(vthread_t thr, vvp_code_t cp)
{
      परत retload<vvp_vector4_t>(thr, cp);
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
तार्किक of_SCOPY(vthread_t thr, vvp_code_t)
{
      vvp_object_t tmp;
      thr->काढा_object(tmp);

      vvp_object_t&dest = thr->peek_object();
        // If it is null there is nothing to copy
      जर (!tmp.test_nil())
	    dest.shallow_copy(tmp);

      परत सत्य;
}

स्थिर शून्य thread_peek(vthread_t thr, द्विगुण&value)
{
      value = thr->peek_real(0);
}

स्थिर शून्य thread_peek(vthread_t thr, string&value)
{
      value = thr->peek_str(0);
}

स्थिर शून्य thread_peek(vthread_t thr, vvp_vector4_t&value)
{
      value = thr->peek_vec4(0);
}

ढाँचा <typename ELEM>
स्थिर तार्किक set_dar_obj(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित adr = thr->शब्द[cp->संख्या].w_int;

      ELEM value;
      thread_peek(thr, value);

      vvp_object_t&शीर्ष = thr->peek_object();
      vvp_darray*darray = शीर्ष.peek<vvp_darray>();
      प्रतिज्ञा(darray);

      darray->set_word(adr, value);
      परत सत्य;
}

/*
 * %set/dar/obj/real <index>
 */
तार्किक of_SET_DAR_OBJ_REAL(vthread_t thr, vvp_code_t cp)
{
      परत set_dar_obj<द्विगुण>(thr, cp);
}

/*
 * %set/dar/obj/str <index>
 */
तार्किक of_SET_DAR_OBJ_STR(vthread_t thr, vvp_code_t cp)
{
      परत set_dar_obj<string>(thr, cp);
}

/*
 * %set/dar/obj/vec4 <index>
 */
तार्किक of_SET_DAR_OBJ_VEC4(vthread_t thr, vvp_code_t cp)
{
      परत set_dar_obj<vvp_vector4_t>(thr, cp);
}

/*
 * %shiftl <idx>
 *
 * Pop the operand, then push the result.
 */
तार्किक of_SHIFTL(vthread_t thr, vvp_code_t cp)
{
      पूर्णांक use_index = cp->संख्या;
      uint64_t shift = thr->शब्द[use_index].w_uint;

      vvp_vector4_t&val = thr->peek_vec4();
      अचिन्हित wid  = val.size();

      जर (thr->flags[4] == BIT4_1) {
	      // The result is 'bx if the shift amount is undefined
	    val = vvp_vector4_t(wid, BIT4_X);

      } अन्यथा जर (thr->flags[4] == BIT4_X || shift >= wid) {
	      // Shift is so big that all value is shifted out. Write
	      // a constant 0 result.
	    val = vvp_vector4_t(wid, BIT4_0);

      } अन्यथा जर (shift > 0) {
	    vvp_vector4_t blk = val.subvalue(0, wid-shift);
	    vvp_vector4_t tmp (shift, BIT4_0);
	    val.set_vec(0, tmp);
	    val.set_vec(shift, blk);
      }

      परत सत्य;
}

/*
 * %shiftr <idx>
 * This is an unsigned right shift. The <idx> is a number that selects
 * the index register with the amount of the shift. This instruction
 * checks flag bit 4, which will be true if the shift is invalid.
 */
तार्किक of_SHIFTR(vthread_t thr, vvp_code_t cp)
{
      पूर्णांक use_index = cp->संख्या;
      uint64_t shift = thr->शब्द[use_index].w_uint;

      vvp_vector4_t val = thr->काढा_vec4();
      अचिन्हित wid  = val.size();

      जर (thr->flags[4] == BIT4_1) {
	    val = vvp_vector4_t(wid, BIT4_X);

      } अन्यथा जर (thr->flags[4] == BIT4_X || shift > wid) {
	    val = vvp_vector4_t(wid, BIT4_0);

      } अन्यथा जर (shift > 0) {
	    vvp_vector4_t blk = val.subvalue(shift, wid-shift);
	    vvp_vector4_t tmp (shift, BIT4_0);
	    val.set_vec(0, blk);
	    val.set_vec(wid-shift, tmp);
      }

      thr->ढकला_vec4(val);
      परत सत्य;
}

/*
 *  %shiftr/s <wid>
 */
तार्किक of_SHIFTR_S(vthread_t thr, vvp_code_t cp)
{
      पूर्णांक use_index = cp->संख्या;
      uint64_t shift = thr->शब्द[use_index].w_uint;

      vvp_vector4_t val = thr->काढा_vec4();
      अचिन्हित wid  = val.size();

      vvp_bit4_t sign_bit = val.value(val.size()-1);

      जर (thr->flags[4] == BIT4_1) {
	    val = vvp_vector4_t(wid, BIT4_X);

      } अन्यथा जर (thr->flags[4] == BIT4_X || shift > wid) {
	    val = vvp_vector4_t(wid, sign_bit);

      } अन्यथा जर (shift > 0) {
	    vvp_vector4_t blk = val.subvalue(shift, wid-shift);
	    vvp_vector4_t tmp (shift, sign_bit);
	    val.set_vec(0, blk);
	    val.set_vec(wid-shift, tmp);
      }

      thr->ढकला_vec4(val);
      परत सत्य;
}

/*
 * %split/vec4 <wid>
 *   Pop 1 value,
 *   Take <wid> bits from the lsb,
 *   Push the remaining msb,
 *   Push the lsb.
 */
तार्किक of_SPLIT_VEC4(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित lsb_wid = cp->संख्या;

      vvp_vector4_t&val = thr->peek_vec4();
      प्रतिज्ञा(lsb_wid < val.size());

      vvp_vector4_t lsb = val.subvalue(0, lsb_wid);
      val = val.subvalue(lsb_wid, val.size()-lsb_wid);

      thr->ढकला_vec4(lsb);
      परत सत्य;
}

/*
 * The following are used to allow the darray templates to print correctly.
 */
ओळीत स्थिर string get_darray_type(द्विगुण&)
{
      परत "darray<real>";
}

ओळीत स्थिर string get_darray_type(string&)
{
      परत "darray<string>";
}

ओळीत स्थिर string get_darray_type(स्थिरांक vvp_vector4_t&value)
{
      ostringstream buf;
      buf << "darray<vector[" << value.size() << "]>";
      string res = buf.str();
      परत res;
}

/*
 * The following are used to allow a common template to be written for
 * darray real/string/vec4 operations
 */
ओळीत स्थिर शून्य dar_काढा_मूल्य(vthread_t thr, द्विगुण&value)
{
      value = thr->काढा_real();
}

ओळीत स्थिर शून्य dar_काढा_मूल्य(vthread_t thr, string&value)
{
      value = thr->काढा_str();
}

ओळीत स्थिर शून्य dar_काढा_मूल्य(vthread_t thr, vvp_vector4_t&value)
{
      value = thr->काढा_vec4();
}

ढाँचा <typename ELEM>
स्थिर तार्किक store_dar(vthread_t thr, vvp_code_t cp)
{
      int64_t adr = thr->शब्द[3].w_int;
      ELEM value;
	// FIXME: Can we get the size of the underlying array element
	//        and then use the normal pop_value?
      dar_काढा_मूल्य(thr, value);

      vvp_net_t*net = cp->net;
      प्रतिज्ञा(net);

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      प्रतिज्ञा(obj);

      vvp_darray*darray = obj->get_object().peek<vvp_darray>();

      जर (adr < 0)
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to a negative " << get_darray_type(value)
	         << " index (" << adr << ")." << endl;
      अन्यथा जर (thr->flags[4] != BIT4_0)
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to an undefined " << get_darray_type(value)
	         << " index." << endl;
      अन्यथा जर (darray)
	    darray->set_word(adr, value);
      अन्यथा
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to an undefined " << get_darray_type(value)
	         << "." << endl;

      परत सत्य;
}

/*
 * %store/dar/real <var>
 */
तार्किक of_STORE_DAR_R(vthread_t thr, vvp_code_t cp)
{
      परत store_dar<द्विगुण>(thr, cp);
}

/*
 * %store/dar/str <var>
 */
तार्किक of_STORE_DAR_STR(vthread_t thr, vvp_code_t cp)
{
      परत store_dar<string>(thr, cp);
}

/*
 * %store/dar/vec4 <var>
 */
तार्किक of_STORE_DAR_VEC4(vthread_t thr, vvp_code_t cp)
{
      परत store_dar<vvp_vector4_t>(thr, cp);
}

तार्किक of_STORE_OBJ(vthread_t thr, vvp_code_t cp)
{
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);

      vvp_object_t val;
      thr->काढा_object(val);

      vvp_send_object(ptr, val, thr->wt_context);

      परत सत्य;
}

/*
 * %store/obja <array-label> <index>
 */
तार्किक of_STORE_OBJA(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित idx = cp->bit_idx[0];
      अचिन्हित adr = thr->शब्द[idx].w_int;

      vvp_object_t val;
      thr->काढा_object(val);

      cp->array->set_word(adr, val);

      परत सत्य;
}


/*
 * %store/prop/obj <pid>, <idx>
 *
 * Pop an object value from the object stack, and store the value into
 * the property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
तार्किक of_STORE_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      माप_प्रकार pid = cp->संख्या;
      अचिन्हित idx = cp->bit_idx[0];

      जर (idx != 0) {
	    प्रतिज्ञा(idx < vthread_s::शब्द_मोजणी);
	    idx = thr->शब्द[idx].w_uint;
      }

      vvp_object_t val;
      thr->काढा_object(val);

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      cobj->set_object(pid, val, idx);

      परत सत्य;
}

स्थिर शून्य काढा_prop_val(vthread_t thr, द्विगुण&val, अचिन्हित)
{
      val = thr->काढा_real();
}

स्थिर शून्य काढा_prop_val(vthread_t thr, string&val, अचिन्हित)
{
      val = thr->काढा_str();
}

स्थिर शून्य काढा_prop_val(vthread_t thr, vvp_vector4_t&val, अचिन्हित wid)
{
      val = thr->काढा_vec4();
      प्रतिज्ञा(val.size() >= wid);
      val.resize(wid);
}

स्थिर शून्य set_val(vvp_cobject*cobj, माप_प्रकार pid, स्थिरांक द्विगुण&val)
{
      cobj->set_real(pid, val);
}

स्थिर शून्य set_val(vvp_cobject*cobj, माप_प्रकार pid, स्थिरांक string&val)
{
      cobj->set_string(pid, val);
}

स्थिर शून्य set_val(vvp_cobject*cobj, माप_प्रकार pid, स्थिरांक vvp_vector4_t&val)
{
      cobj->set_vec4(pid, val);
}

ढाँचा <typename ELEM>
स्थिर तार्किक store_prop(vthread_t thr, vvp_code_t cp, अचिन्हित wid=0)
{
      माप_प्रकार pid = cp->संख्या;
      ELEM val;
      काढा_prop_val(thr, val, wid); // Pop the value to store.

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      set_val(cobj, pid, val);

      परत सत्य;
}

/*
 * %store/prop/r <id>
 *
 * Pop a real value from the real stack, and store the value into the
 * property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
तार्किक of_STORE_PROP_R(vthread_t thr, vvp_code_t cp)
{
      परत store_prop<द्विगुण>(thr, cp);
}

/*
 * %store/prop/str <id>
 *
 * Pop a string value from the string stack, and store the value into
 * the property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
तार्किक of_STORE_PROP_STR(vthread_t thr, vvp_code_t cp)
{
      परत store_prop<string>(thr, cp);
}

/*
 * %store/prop/v <pid>, <wid>
 *
 * Store vector value into property <id> of cobject in the top of the
 * stack. Do NOT pop the object stack.
 */
तार्किक of_STORE_PROP_V(vthread_t thr, vvp_code_t cp)
{
      परत store_prop<vvp_vector4_t>(thr, cp, cp->bit_idx[0]);
}

ढाँचा <वर्ग QTYPE>
स्थिर QTYPE* get_queue_prop(vvp_cobject*cobj, माप_प्रकार pid)
{
      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      QTYPE* queue = qobj.peek<QTYPE>();
      जर (queue == 0) {
	    queue = नवीन QTYPE;
	    vvp_object_t val(queue);
	    cobj->set_object(pid, val, 0);
      }
      परत queue;
}

ढाँचा <typename ELEM, वर्ग QTYPE>
स्थिर तार्किक store_prop_qb(vthread_t thr, vvp_code_t cp, अचिन्हित wid)
{
      माप_प्रकार pid = cp->संख्या;
      ELEM value;
      अचिन्हित max_size = thr->शब्द[cp->bit_idx[0]].w_uint;
      काढा_मूल्य(thr, value, wid);

      vvp_object_t& शीर्ष = thr->peek_object();
      vvp_cobject*cobj = शीर्ष.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      QTYPE* queue = get_queue_prop<QTYPE>(cobj, pid);
      प्रतिज्ञा(queue);
      queue->push_back(value, max_size);
      परत सत्य;
}

ढाँचा <typename ELEM, वर्ग QTYPE>
स्थिर तार्किक store_prop_qf(vthread_t thr, vvp_code_t cp, अचिन्हित wid)
{
      माप_प्रकार pid = cp->संख्या;
      ELEM value;
      अचिन्हित max_size = thr->शब्द[cp->bit_idx[0]].w_uint;
      काढा_मूल्य(thr, value, wid);

      vvp_object_t& शीर्ष = thr->peek_object();
      vvp_cobject*cobj = शीर्ष.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      QTYPE* queue = get_queue_prop<QTYPE>(cobj, pid);
      प्रतिज्ञा(queue);
      queue->push_front(value, max_size);
      परत सत्य;
}

तार्किक of_STORE_PROP_QB_R(vthread_t thr, vvp_code_t cp)
{
      परत store_prop_qb<द्विगुण, vvp_queue_real>(thr, cp, 0);
}

तार्किक of_STORE_PROP_QB_STR(vthread_t thr, vvp_code_t cp)
{
      परत store_prop_qb<string, vvp_queue_string>(thr, cp, 0);
}

तार्किक of_STORE_PROP_QB_V(vthread_t thr, vvp_code_t cp)
{
      परत store_prop_qb<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

तार्किक of_STORE_PROP_QF_R(vthread_t thr, vvp_code_t cp)
{
      परत store_prop_qf<द्विगुण, vvp_queue_real>(thr, cp, 0);
}

तार्किक of_STORE_PROP_QF_STR(vthread_t thr, vvp_code_t cp)
{
      परत store_prop_qf<string, vvp_queue_string>(thr, cp, 0);
}

तार्किक of_STORE_PROP_QF_V(vthread_t thr, vvp_code_t cp)
{
      परत store_prop_qf<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

ढाँचा <typename ELEM, वर्ग QTYPE>
स्थिर तार्किक qinsert_prop(vthread_t thr, vvp_code_t cp, अचिन्हित wid)
{
      int64_t idx = thr->शब्द[3].w_int;
      ELEM value;
      माप_प्रकार pid = cp->संख्या;
      काढा_मूल्य(thr, value, wid);

      vvp_object_t& शीर्ष = thr->peek_object();
      vvp_cobject*cobj = शीर्ष.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      QTYPE* queue = get_queue_prop<QTYPE>(cobj, pid);
      प्रतिज्ञा(queue);
      जर (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at a negative "
	         << get_queue_type(value)
	         << " index (" << idx << "). ";
	    print_queue_मूल्य(value);
	    cerr << " was not added." << endl;
      } अन्यथा जर (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at an undefined "
	         << get_queue_type(value) << " index. ";
	    print_queue_मूल्य(value);
	    cerr << " was not added." << endl;
      } अन्यथा {
	    अचिन्हित max_size = thr->शब्द[cp->bit_idx[0]].w_int;
	    queue->insert(idx, value, max_size);
      }
      परत सत्य;
}

तार्किक of_QINSERT_PROP_R(vthread_t thr, vvp_code_t cp)
{
      परत qinsert_prop<द्विगुण, vvp_queue_real>(thr, cp, 0);
}

तार्किक of_QINSERT_PROP_STR(vthread_t thr, vvp_code_t cp)
{
      परत qinsert_prop<string, vvp_queue_string>(thr, cp, 0);
}

तार्किक of_QINSERT_PROP_V(vthread_t thr, vvp_code_t cp)
{
      परत qinsert_prop<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

ढाँचा <typename ELEM, वर्ग QTYPE>
स्थिर तार्किक q_काढा_prop(vthread_t thr, vvp_code_t cp,
                       शून्य (*get_val_func)(vvp_queue*, ELEM&),
                       स्थिरांक अक्षर*loc, अचिन्हित wid)
{
      माप_प्रकार pid = cp->संख्या;

      vvp_object_t& शीर्ष = thr->peek_object();
      vvp_cobject*cobj = शीर्ष.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      QTYPE* queue = get_queue_prop<QTYPE>(cobj, pid);
      प्रतिज्ञा(queue);

      माप_प्रकार size = queue->get_size();

      ELEM value;
      जर (size) {
	    get_val_func(queue, value);
      } अन्यथा {
	    dq_default(value, wid);
	    cerr << thr->get_fileline()
	         << "Warning: pop_" << loc << "() on empty "
	         << get_queue_type(value) << "." << endl;
      }

      ढकला_मूल्य(thr, value, wid);
      परत सत्य;
}

ढाँचा <typename ELEM, वर्ग QTYPE>
स्थिर तार्किक qpop_b_prop(vthread_t thr, vvp_code_t cp, अचिन्हित wid)
{
      परत q_काढा_prop<ELEM, QTYPE>(thr, cp, get_back_मूल्य<ELEM>, "back", wid);
}

ढाँचा <typename ELEM, वर्ग QTYPE>
स्थिर तार्किक qpop_f_prop(vthread_t thr, vvp_code_t cp, अचिन्हित wid)
{
      परत q_काढा_prop<ELEM, QTYPE>(thr, cp, get_front_मूल्य<ELEM>, "front", wid);
}

तार्किक of_QPOP_PROP_B_REAL(vthread_t thr, vvp_code_t cp)
{
      परत qpop_b_prop<द्विगुण, vvp_queue_real>(thr, cp, 0);
}

तार्किक of_QPOP_PROP_B_STR(vthread_t thr, vvp_code_t cp)
{
      परत qpop_b_prop<string, vvp_queue_string>(thr, cp, 0);
}

तार्किक of_QPOP_PROP_B_V(vthread_t thr, vvp_code_t cp)
{
      परत qpop_b_prop<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[0]);
}

तार्किक of_QPOP_PROP_F_REAL(vthread_t thr, vvp_code_t cp)
{
      परत qpop_f_prop<द्विगुण, vvp_queue_real>(thr, cp, 0);
}

तार्किक of_QPOP_PROP_F_STR(vthread_t thr, vvp_code_t cp)
{
      परत qpop_f_prop<string, vvp_queue_string>(thr, cp, 0);
}

तार्किक of_QPOP_PROP_F_V(vthread_t thr, vvp_code_t cp)
{
      परत qpop_f_prop<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[0]);
}

स्थिर vvp_vector4_t queue_unique_ulong_to_vec4(अचिन्हित दीर्घ ul, अचिन्हित bits)
{
      vvp_vector4_t val(bits, BIT4_0);
      साठी (अचिन्हित idx = 0; idx < bits && idx < 8 * आकार(अचिन्हित दीर्घ); idx += 1) {
	    जर (ul & 1UL) {
		  val.set_bit(idx, BIT4_1);
	    } अन्यथा {
		  val.set_bit(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      परत val;
}

ढाँचा <वर्ग SRC> स्थिर vvp_queue_vec4* queue_run_unique_src(SRC* src,
								 अचिन्हित wid,
								 तार्किक as_index)
{
      vvp_queue_vec4* dst = नवीन vvp_queue_vec4();
      जर (!src) परत dst;
      माप_प्रकार n = src->get_size();
      साठी (माप_प्रकार i = 0; i < n; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    तार्किक seen = असत्य;
	    साठी (माप_प्रकार j = 0; j < i; j += 1) {
		  vvp_vector4_t vj(wid);
		  src->get_word(j, vj);
		  जर (vi.eeq(vj)) {
			seen = सत्य;
			थांबा;
		  }
	    }
	    जर (seen) सुरूठेवा;
	    जर (as_index) {
		  dst->push_back(queue_unique_ulong_to_vec4((अचिन्हित दीर्घ)i, 32), 0);
	    } अन्यथा {
		  dst->push_back(vi, 0);
	    }
      }
      परत dst;
}

ढाँचा <वर्ग SRC>
स्थिर vvp_queue_vec4* queue_run_find_matches_src(SRC* src, अचिन्हित wid,
						  स्थिरांक vvp_vector4_t& cmp,
						  तार्किक as_index)
{
      vvp_queue_vec4* dst = नवीन vvp_queue_vec4();
      जर (!src) परत dst;
      माप_प्रकार n = src->get_size();
      साठी (माप_प्रकार i = 0; i < n; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    जर (vi.eeq(cmp)) {
		  जर (as_index) {
			dst->push_back(queue_unique_ulong_to_vec4((अचिन्हित दीर्घ)i, 32), 0);
		  } अन्यथा {
			dst->push_back(vi, 0);
		  }
	    }
      }
      परत dst;
}

/* Array/queue locator methods return a queue with 0 or 1 elements (LRM). */
ढाँचा <वर्ग SRC>
स्थिर vvp_queue_vec4* queue_find_first_last_to_queue_src(SRC* src,
							  अचिन्हित wid,
							  स्थिरांक vvp_vector4_t& cmp,
							  तार्किक want_first,
							  तार्किक want_index)
{
      vvp_queue_vec4* dst = नवीन vvp_queue_vec4();
      जर (!src || src->get_size() == 0) परत dst;
      माप_प्रकार n = src->get_size();
      जर (want_first) {
	    साठी (माप_प्रकार i = 0; i < n; i += 1) {
		  vvp_vector4_t vi(wid);
		  src->get_word(i, vi);
		  जर (vi.eeq(cmp)) {
			जर (want_index) {
			      dst->push_back(
				  queue_unique_ulong_to_vec4((अचिन्हित दीर्घ)i, 32),
				  0);
			} अन्यथा {
			      dst->push_back(vi, 0);
			}
			परत dst;
		  }
	    }
      } अन्यथा {
	    साठी (माप_प्रकार i = n; i > 0; ) {
		  i -= 1;
		  vvp_vector4_t vi(wid);
		  src->get_word(i, vi);
		  जर (vi.eeq(cmp)) {
			जर (want_index) {
			      dst->push_back(
				  queue_unique_ulong_to_vec4((अचिन्हित दीर्घ)i, 32),
				  0);
			} अन्यथा {
			      dst->push_back(vi, 0);
			}
			परत dst;
		  }
	    }
      }
      परत dst;
}

/* min()/max() return one element with the selected extremum. */
ढाँचा <वर्ग SRC>
स्थिर vvp_queue_vec4* queue_run_min_max_src(SRC* src, अचिन्हित wid, तार्किक want_max)
{
      vvp_queue_vec4* dst = नवीन vvp_queue_vec4();
      जर (!src || src->get_size() == 0) परत dst;

      vvp_vector4_t best(wid);
      src->get_word(0, best);

      माप_प्रकार n = src->get_size();
      साठी (माप_प्रकार i = 1 ; i < n ; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);

	    vvp_bit4_t ge = compare_gtge(vi, best, BIT4_1);
	    तार्किक replace = want_max ? (ge == BIT4_1) : (ge == BIT4_0);
	    जर (replace) best = vi;
      }

      dst->push_back(best, 0);
      परत dst;
}

तार्किक of_QUEUE_FIND_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->काढा_vec4();
      अचिन्हित wid = cp->bit_idx[0];
      प्रतिज्ञा(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      जर (qsrc) dst = queue_run_find_matches_src(qsrc, wid, cmp, असत्य);
      अन्यथा जर (dsrc) dst = queue_run_find_matches_src(dsrc, wid, cmp, असत्य);
      अन्यथा {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_run_find_matches_src(src, wid, cmp, असत्य);
      }
      thr->ढकला_object(vvp_object_t(dst));
      परत सत्य;
}

तार्किक of_QUEUE_FIND_PROP_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->काढा_vec4();
      माप_प्रकार pid = cp->संख्या;
      अचिन्हित wid = cp->bit_idx[0];
      प्रतिज्ञा(cmp.size() == wid);

      vvp_object_t& शीर्ष = thr->peek_object();
      vvp_cobject*cobj = शीर्ष.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_find_matches_src(qsrc, wid, cmp, असत्य)
				 : queue_run_find_matches_src(dsrc, wid, cmp, असत्य);
      thr->ढकला_object(vvp_object_t(dst));
      परत सत्य;
}

तार्किक of_QUEUE_FIND_INDEX_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->काढा_vec4();
      अचिन्हित wid = cp->bit_idx[0];
      प्रतिज्ञा(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      जर (qsrc) dst = queue_run_find_matches_src(qsrc, wid, cmp, सत्य);
      अन्यथा जर (dsrc) dst = queue_run_find_matches_src(dsrc, wid, cmp, सत्य);
      अन्यथा {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_run_find_matches_src(src, wid, cmp, सत्य);
      }
      thr->ढकला_object(vvp_object_t(dst));
      परत सत्य;
}

तार्किक of_QUEUE_FIND_INDEX_PROP_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->काढा_vec4();
      माप_प्रकार pid = cp->संख्या;
      अचिन्हित wid = cp->bit_idx[0];
      प्रतिज्ञा(cmp.size() == wid);

      vvp_object_t& शीर्ष = thr->peek_object();
      vvp_cobject*cobj = शीर्ष.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_find_matches_src(qsrc, wid, cmp, सत्य)
				 : queue_run_find_matches_src(dsrc, wid, cmp, सत्य);
      thr->ढकला_object(vvp_object_t(dst));
      परत सत्य;
}

तार्किक of_QUEUE_FIND_FIRST_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->काढा_vec4();
      अचिन्हित wid = cp->bit_idx[0];
      प्रतिज्ञा(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      जर (qsrc) dst = queue_find_first_last_to_queue_src(qsrc, wid, cmp, सत्य, असत्य);
      अन्यथा जर (dsrc) dst = queue_find_first_last_to_queue_src(dsrc, wid, cmp, सत्य, असत्य);
      अन्यथा {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_find_first_last_to_queue_src(src, wid, cmp, सत्य, असत्य);
      }
      thr->ढकला_object(vvp_object_t(dst));
      परत सत्य;
}

तार्किक of_QUEUE_FIND_FIRST_PROP_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->काढा_vec4();
      माप_प्रकार pid = cp->संख्या;
      अचिन्हित wid = cp->bit_idx[0];
      प्रतिज्ञा(cmp.size() == wid);

      vvp_object_t& शीर्ष = thr->peek_object();
      vvp_cobject*cobj = शीर्ष.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst =
	    qsrc ? queue_find_first_last_to_queue_src(qsrc, wid, cmp, सत्य, असत्य)
		 : queue_find_first_last_to_queue_src(dsrc, wid, cmp, सत्य, असत्य);
      thr->ढकला_object(vvp_object_t(dst));
      परत सत्य;
}

तार्किक of_QUEUE_FIND_FIRST_INDEX_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->काढा_vec4();
      अचिन्हित wid = cp->bit_idx[0];
      प्रतिज्ञा(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      जर (qsrc) dst = queue_find_first_last_to_queue_src(qsrc, wid, cmp, सत्य, सत्य);
      अन्यथा जर (dsrc) dst = queue_find_first_last_to_queue_src(dsrc, wid, cmp, सत्य, सत्य);
      अन्यथा {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_find_first_last_to_queue_src(src, wid, cmp, सत्य, सत्य);
      }
      thr->ढकला_object(vvp_object_t(dst));
      परत सत्य;
}

तार्किक of_QUEUE_FIND_FIRST_INDEX_PROP_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->काढा_vec4();
      माप_प्रकार pid = cp->संख्या;
      अचिन्हित wid = cp->bit_idx[0];
      प्रतिज्ञा(cmp.size() == wid);

      vvp_object_t& शीर्ष = thr->peek_object();
      vvp_cobject*cobj = शीर्ष.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst =
	    qsrc ? queue_find_first_last_to_queue_src(qsrc, wid, cmp, सत्य, सत्य)
		 : queue_find_first_last_to_queue_src(dsrc, wid, cmp, सत्य, सत्य);
      thr->ढकला_object(vvp_object_t(dst));
      परत सत्य;
}

तार्किक of_QUEUE_FIND_LAST_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->काढा_vec4();
      अचिन्हित wid = cp->bit_idx[0];
      प्रतिज्ञा(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      जर (qsrc) dst = queue_find_first_last_to_queue_src(qsrc, wid, cmp, असत्य, असत्य);
      अन्यथा जर (dsrc) dst = queue_find_first_last_to_queue_src(dsrc, wid, cmp, असत्य, असत्य);
      अन्यथा {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_find_first_last_to_queue_src(src, wid, cmp, असत्य, असत्य);
      }
      thr->ढकला_object(vvp_object_t(dst));
      परत सत्य;
}

तार्किक of_QUEUE_FIND_LAST_PROP_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->काढा_vec4();
      माप_प्रकार pid = cp->संख्या;
      अचिन्हित wid = cp->bit_idx[0];
      प्रतिज्ञा(cmp.size() == wid);

      vvp_object_t& शीर्ष = thr->peek_object();
      vvp_cobject*cobj = शीर्ष.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst =
	    qsrc ? queue_find_first_last_to_queue_src(qsrc, wid, cmp, असत्य, असत्य)
		 : queue_find_first_last_to_queue_src(dsrc, wid, cmp, असत्य, असत्य);
      thr->ढकला_object(vvp_object_t(dst));
      परत सत्य;
}

तार्किक of_QUEUE_FIND_LAST_INDEX_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->काढा_vec4();
      अचिन्हित wid = cp->bit_idx[0];
      प्रतिज्ञा(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      जर (qsrc) dst = queue_find_first_last_to_queue_src(qsrc, wid, cmp, असत्य, सत्य);
      अन्यथा जर (dsrc) dst = queue_find_first_last_to_queue_src(dsrc, wid, cmp, असत्य, सत्य);
      अन्यथा {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_find_first_last_to_queue_src(src, wid, cmp, असत्य, सत्य);
      }
      thr->ढकला_object(vvp_object_t(dst));
      परत सत्य;
}

तार्किक of_QUEUE_FIND_LAST_INDEX_PROP_V(vthread_t thr, vvp_code_t cp)
{
      vvp_vector4_t cmp = thr->काढा_vec4();
      माप_प्रकार pid = cp->संख्या;
      अचिन्हित wid = cp->bit_idx[0];
      प्रतिज्ञा(cmp.size() == wid);

      vvp_object_t& शीर्ष = thr->peek_object();
      vvp_cobject*cobj = शीर्ष.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst =
	    qsrc ? queue_find_first_last_to_queue_src(qsrc, wid, cmp, असत्य, सत्य)
		 : queue_find_first_last_to_queue_src(dsrc, wid, cmp, असत्य, सत्य);
      thr->ढकला_object(vvp_object_t(dst));
      परत सत्य;
}

तार्किक of_QUEUE_MIN_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      अचिन्हित wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      जर (qsrc) dst = queue_run_min_max_src(qsrc, wid, असत्य);
      अन्यथा जर (dsrc) dst = queue_run_min_max_src(dsrc, wid, असत्य);
      अन्यथा {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_run_min_max_src(src, wid, असत्य);
      }
      thr->ढकला_object(vvp_object_t(dst));
      परत सत्य;
}

तार्किक of_QUEUE_MIN_OBJ_V(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->काढा_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_min_max_src(qsrc, wid, असत्य)
				 : queue_run_min_max_src(dsrc, wid, असत्य);
      thr->ढकला_object(vvp_object_t(dst));
      परत सत्य;
}

तार्किक of_QUEUE_MIN_PROP_V(vthread_t thr, vvp_code_t cp)
{
      माप_प्रकार pid = cp->संख्या;
      अचिन्हित wid = cp->bit_idx[0];

      vvp_object_t& शीर्ष = thr->peek_object();
      vvp_cobject*cobj = शीर्ष.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_min_max_src(qsrc, wid, असत्य)
				 : queue_run_min_max_src(dsrc, wid, असत्य);
      thr->ढकला_object(vvp_object_t(dst));
      परत सत्य;
}

तार्किक of_QUEUE_MAX_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      अचिन्हित wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      जर (qsrc) dst = queue_run_min_max_src(qsrc, wid, सत्य);
      अन्यथा जर (dsrc) dst = queue_run_min_max_src(dsrc, wid, सत्य);
      अन्यथा {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_run_min_max_src(src, wid, सत्य);
      }
      thr->ढकला_object(vvp_object_t(dst));
      परत सत्य;
}

तार्किक of_QUEUE_MAX_OBJ_V(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->काढा_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_min_max_src(qsrc, wid, सत्य)
				 : queue_run_min_max_src(dsrc, wid, सत्य);
      thr->ढकला_object(vvp_object_t(dst));
      परत सत्य;
}

तार्किक of_QUEUE_MAX_PROP_V(vthread_t thr, vvp_code_t cp)
{
      माप_प्रकार pid = cp->संख्या;
      अचिन्हित wid = cp->bit_idx[0];

      vvp_object_t& शीर्ष = thr->peek_object();
      vvp_cobject*cobj = शीर्ष.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_min_max_src(qsrc, wid, सत्य)
				 : queue_run_min_max_src(dsrc, wid, सत्य);
      thr->ढकला_object(vvp_object_t(dst));
      परत सत्य;
}

तार्किक of_QUEUE_UNIQUE_OBJ_V(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->काढा_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_unique_src(qsrc, wid, असत्य)
				 : queue_run_unique_src(dsrc, wid, असत्य);
      thr->ढकला_object(vvp_object_t(dst));
      परत सत्य;
}

तार्किक of_QUEUE_UNIQUE_INDEX_OBJ_V(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->काढा_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_unique_src(qsrc, wid, सत्य)
				 : queue_run_unique_src(dsrc, wid, सत्य);
      thr->ढकला_object(vvp_object_t(dst));
      परत सत्य;
}

तार्किक of_QUEUE_UNIQUE_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      अचिन्हित wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      जर (qsrc) dst = queue_run_unique_src(qsrc, wid, असत्य);
      अन्यथा जर (dsrc) dst = queue_run_unique_src(dsrc, wid, असत्य);
      अन्यथा {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_run_unique_src(src, wid, असत्य);
      }
      thr->ढकला_object(vvp_object_t(dst));
      परत सत्य;
}

तार्किक of_QUEUE_UNIQUE_PROP_V(vthread_t thr, vvp_code_t cp)
{
      माप_प्रकार pid = cp->संख्या;
      अचिन्हित wid = cp->bit_idx[0];

      vvp_object_t& शीर्ष = thr->peek_object();
      vvp_cobject*cobj = शीर्ष.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_unique_src(qsrc, wid, असत्य)
				 : queue_run_unique_src(dsrc, wid, असत्य);
      thr->ढकला_object(vvp_object_t(dst));
      परत सत्य;
}

तार्किक of_QUEUE_UNIQUE_INDEX_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;
      अचिन्हित wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      जर (qsrc) dst = queue_run_unique_src(qsrc, wid, सत्य);
      अन्यथा जर (dsrc) dst = queue_run_unique_src(dsrc, wid, सत्य);
      अन्यथा {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_run_unique_src(src, wid, सत्य);
      }
      thr->ढकला_object(vvp_object_t(dst));
      परत सत्य;
}

तार्किक of_QUEUE_UNIQUE_INDEX_PROP_V(vthread_t thr, vvp_code_t cp)
{
      माप_प्रकार pid = cp->संख्या;
      अचिन्हित wid = cp->bit_idx[0];

      vvp_object_t& शीर्ष = thr->peek_object();
      vvp_cobject*cobj = शीर्ष.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_run_unique_src(qsrc, wid, सत्य)
				 : queue_run_unique_src(dsrc, wid, सत्य);
      thr->ढकला_object(vvp_object_t(dst));
      परत सत्य;
}

तार्किक of_PROP_QUEUE_SIZE(vthread_t thr, vvp_code_t cp)
{
      माप_प्रकार pid = cp->संख्या;

      vvp_object_t& शीर्ष = thr->peek_object();
      vvp_cobject*cobj = शीर्ष.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);

      माप_प्रकार sz = qsrc ? qsrc->get_size() : (dsrc ? dsrc->get_size() : 0);

      vvp_vector4_t val(32, BIT4_0);
      अचिन्हित दीर्घ ul = sz;
      साठी (अचिन्हित idx = 0; idx < 32; idx++) {
	    जर (ul & 1UL) {
		  val.set_bit(idx, BIT4_1);
	    } अन्यथा {
		  val.set_bit(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      thr->ढकला_vec4(val);
      परत सत्य;
}

तार्किक of_QUEUE_SIZE_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      माप_प्रकार sz =
	    qsrc ? qsrc->get_size() : (dsrc ? dsrc->get_size() : 0);

      vvp_vector4_t val(32, BIT4_0);
      अचिन्हित दीर्घ ul = sz;
      साठी (अचिन्हित idx = 0; idx < 32; idx++) {
	    जर (ul & 1UL) {
		  val.set_bit(idx, BIT4_1);
	    } अन्यथा {
		  val.set_bit(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      thr->ढकला_vec4(val);
      परत सत्य;
}

तार्किक of_QUEUE_WORD_V(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित wid = cp->bit_idx[0];
      अचिन्हित ix_reg = cp->bit_idx[1];
      int64_t adr = thr->शब्द[ix_reg].w_int;
      जर (thr->flags[4] == BIT4_1) {
	    thr->ढकला_vec4(vvp_vector4_t(wid));
	    परत सत्य;
      }
      जर (adr < 0) {
	    thr->ढकला_vec4(vvp_vector4_t(wid));
	    परत सत्य;
      }
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t out(wid);
      जर (qsrc) {
	    जर ((माप_प्रकार)adr >= qsrc->get_size()) {
		  thr->ढकला_vec4(out);
		  परत सत्य;
	    }
	    qsrc->get_word((अचिन्हित)adr, out);
      } अन्यथा जर (dsrc) {
	    जर ((माप_प्रकार)adr >= dsrc->get_size()) {
		  thr->ढकला_vec4(out);
		  परत सत्य;
	    }
	    dsrc->get_word((अचिन्हित)adr, out);
      } अन्यथा {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    जर (!src || (माप_प्रकार)adr >= src->get_size()) {
		  thr->ढकला_vec4(out);
		  परत सत्य;
	    }
	    src->get_word((अचिन्हित)adr, out);
      }
      thr->ढकला_vec4(out);
      परत सत्य;
}

तार्किक of_QUEUE_WORD_PROP_V(vthread_t thr, vvp_code_t cp)
{
      माप_प्रकार pid = cp->संख्या;
      अचिन्हित wid = cp->bit_idx[0];
      अचिन्हित ix_reg = cp->bit_idx[1];
      int64_t adr = thr->शब्द[ix_reg].w_int;
      जर (thr->flags[4] == BIT4_1) {
	    thr->ढकला_vec4(vvp_vector4_t(wid));
	    परत सत्य;
      }
      जर (adr < 0) {
	    thr->ढकला_vec4(vvp_vector4_t(wid));
	    परत सत्य;
      }

      vvp_object_t saved_शीर्ष;
      तार्किक popped_शीर्ष = असत्य;
      vvp_object_t& शीर्ष = thr->peek_object();
      vvp_cobject*cobj = शीर्ष.peek<vvp_cobject>();
      जर (cobj == 0) {
	    // Array-locator with(property) keeps an accumulator queue on top.
	    // Temporarily pop it to access the class object beneath.
	    thr->काढा_object(saved_शीर्ष);
	    popped_शीर्ष = सत्य;
	    vvp_object_t& below = thr->peek_object();
	    cobj = below.peek<vvp_cobject>();
      }
      जर (cobj == 0) {
	    जर (popped_शीर्ष) thr->ढकला_object(saved_शीर्ष);
	    thr->ढकला_vec4(vvp_vector4_t(wid));
	    परत सत्य;
      }

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t out(wid);
      जर (qsrc) {
	    जर ((माप_प्रकार)adr >= qsrc->get_size()) {
		  thr->ढकला_vec4(out);
		  परत सत्य;
	    }
	    qsrc->get_word((अचिन्हित)adr, out);
      } अन्यथा जर (dsrc) {
	    जर ((माप_प्रकार)adr >= dsrc->get_size()) {
		  thr->ढकला_vec4(out);
		  परत सत्य;
	    }
	    dsrc->get_word((अचिन्हित)adr, out);
      } अन्यथा {
	    thr->ढकला_vec4(out);
	    जर (popped_शीर्ष) thr->ढकला_object(saved_शीर्ष);
	    परत सत्य;
      }
      जर (popped_शीर्ष) thr->ढकला_object(saved_शीर्ष);
      thr->ढकला_vec4(out);
      परत सत्य;
}

तार्किक of_CMPIX_LTU(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित a = cp->संख्या;
      अचिन्हित b = cp->bit_idx[0];
      जर (thr->शब्द[a].w_uint < thr->शब्द[b].w_uint) {
	    thr->flags[4] = BIT4_1;
      } अन्यथा {
	    thr->flags[4] = BIT4_0;
      }
      परत सत्य;
}

तार्किक of_CMPIX_SLT0(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित r = cp->संख्या;
      thr->flags[4] = (thr->शब्द[r].w_int < 0) ? BIT4_1 : BIT4_0;
      परत सत्य;
}

तार्किक of_ढकला_IX_VEC4(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित ix = cp->संख्या;
      अचिन्हित wid = cp->bit_idx[0];
      अचिन्हित sgn = cp->bit_idx[1];
      uint32_t ubits;
      जर (sgn) {
	    int32_t sv = (int32_t)thr->शब्द[ix].w_int;
	    ubits = (uint32_t)sv;
      } अन्यथा {
	    ubits = (uint32_t)thr->शब्द[ix].w_uint;
      }
      जर (wid > 32) wid = 32;
      vvp_vector4_t tmp(wid, BIT4_0);
      साठी (अचिन्हित idx = 0; idx < wid; idx++) {
	    जर (ubits & 1u) {
		  tmp.set_bit(idx, BIT4_1);
	    } अन्यथा {
		  tmp.set_bit(idx, BIT4_0);
	    }
	    ubits >>= 1;
      }
      thr->ढकला_vec4(tmp);
      परत सत्य;
}

तार्किक of_QUEUE_NEW_EMPTY_V(vthread_t thr, vvp_code_t)
{
      thr->ढकला_object(vvp_object_t(नवीन vvp_queue_vec4()));
      परत सत्य;
}

तार्किक of_QUEUE_APPEND_WORD_V(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित wid = cp->bit_idx[0];
      vvp_vector4_t elem = thr->काढा_vec4();
      प्रतिज्ञा(elem.size() == wid);
      vvp_object_t qobj;
      thr->काढा_object(qobj);
      vvp_queue_vec4* q = qobj.peek<vvp_queue_vec4>();
      प्रतिज्ञा(q);
      q->push_back(elem, 0);
      thr->ढकला_object(qobj);
      परत सत्य;
}

तार्किक of_DELETE_PROP_ELEM(vthread_t thr, vvp_code_t cp)
{
      माप_प्रकार pid = cp->संख्या;

      int64_t idx_val = thr->शब्द[3].w_int;
      जर (thr->flags[4] == BIT4_1) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with undefined index."
	         << endl;
	    परत सत्य;
      }
      जर (idx_val < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with negative index."
	         << endl;
	    परत सत्य;
      }
      माप_प्रकार idx = idx_val;

      vvp_object_t& शीर्ष = thr->peek_object();
      vvp_cobject*cobj = शीर्ष.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue* queue = qobj.peek<vvp_queue>();
      जर (queue == 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping delete(" << idx
	         << ") on empty queue." << endl;
      } अन्यथा {
	    माप_प्रकार size = queue->get_size();
	    जर (idx >= size) {
		  cerr << thr->get_fileline()
		       << "Warning: skipping out of range delete(" << idx
		       << ") on queue of size " << size << "." << endl;
	    } अन्यथा {
		  queue->erase(idx);
	    }
      }

      परत सत्य;
}

तार्किक of_DELETE_PROP_OBJ(vthread_t thr, vvp_code_t cp)
{
      माप_प्रकार pid = cp->संख्या;

      vvp_object_t& शीर्ष = thr->peek_object();
      vvp_cobject*cobj = शीर्ष.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      cobj->set_object(pid, vvp_object_t(), 0);

      परत सत्य;
}

ढाँचा <typename ELEM, वर्ग QTYPE>
स्थिर तार्किक store_qb(vthread_t thr, vvp_code_t cp, अचिन्हित wid=0)
{
      ELEM value;
      vvp_net_t*net = cp->net;
      अचिन्हित max_size = thr->शब्द[cp->bit_idx[0]].w_int;
      काढा_मूल्य(thr, value, wid); // Pop the value to store.

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      प्रतिज्ञा(queue);
      queue->push_back(value, max_size);
      परत सत्य;
}

/*
 * %store/qb/r <var-label>, <max-idx>
 */
तार्किक of_STORE_QB_R(vthread_t thr, vvp_code_t cp)
{
      परत store_qb<द्विगुण, vvp_queue_real>(thr, cp);
}

/*
 * %store/qb/str <var-label>, <max-idx>
 */
तार्किक of_STORE_QB_STR(vthread_t thr, vvp_code_t cp)
{
      परत store_qb<string, vvp_queue_string>(thr, cp);
}

/*
 * %store/qb/v <var-label>, <max-idx>, <wid>
 */
तार्किक of_STORE_QB_V(vthread_t thr, vvp_code_t cp)
{
      परत store_qb<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

ढाँचा <typename ELEM, वर्ग QTYPE>
स्थिर तार्किक store_qdar(vthread_t thr, vvp_code_t cp, अचिन्हित wid=0)
{
      int64_t idx = thr->शब्द[3].w_int;
      ELEM value;
      vvp_net_t*net = cp->net;
      काढा_मूल्य(thr, value, wid); // Pop the value to store.

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      प्रतिज्ञा(queue);
      जर (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot assign to a negative "
	         << get_queue_type(value)
	         << " index (" << idx << "). ";
	    print_queue_मूल्य(value);
	    cerr << " was not added." << endl;
      } अन्यथा जर (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot assign to an undefined "
	         << get_queue_type(value) << " index. ";
	    print_queue_मूल्य(value);
	    cerr << " was not added." << endl;
      } अन्यथा {
	    अचिन्हित max_size = thr->शब्द[cp->bit_idx[0]].w_int;
	    queue->set_word_max(idx, value, max_size);
      }
      परत सत्य;
}

/*
 * %store/qdar/r <var>, idx
 */
तार्किक of_STORE_QDAR_R(vthread_t thr, vvp_code_t cp)
{
      परत store_qdar<द्विगुण, vvp_queue_real>(thr, cp);
}

/*
 * %store/qdar/str <var>, idx
 */
तार्किक of_STORE_QDAR_STR(vthread_t thr, vvp_code_t cp)
{
      परत store_qdar<string, vvp_queue_string>(thr, cp);
}

/*
 * %store/qdar/v <var>, idx
 */
तार्किक of_STORE_QDAR_V(vthread_t thr, vvp_code_t cp)
{
      परत store_qdar<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

ढाँचा <typename ELEM, वर्ग QTYPE>
स्थिर तार्किक store_qf(vthread_t thr, vvp_code_t cp, अचिन्हित wid=0)
{
      ELEM value;
      vvp_net_t*net = cp->net;
      अचिन्हित max_size = thr->शब्द[cp->bit_idx[0]].w_int;
      काढा_मूल्य(thr, value, wid); // Pop the value to store.

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      प्रतिज्ञा(queue);
      queue->push_front(value, max_size);
      परत सत्य;
}
/*
 * %store/qf/r <var-label>, <max-idx>
 */
तार्किक of_STORE_QF_R(vthread_t thr, vvp_code_t cp)
{
      परत store_qf<द्विगुण, vvp_queue_real>(thr, cp);
}

/*
 * %store/qf/str <var-label>, <max-idx>
 */
तार्किक of_STORE_QF_STR(vthread_t thr, vvp_code_t cp)
{
      परत store_qf<string, vvp_queue_string>(thr, cp);
}

/*
 * %store/qb/v <var-label>, <max-idx>, <wid>
 */
तार्किक of_STORE_QF_V(vthread_t thr, vvp_code_t cp)
{
      परत store_qf<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

ढाँचा <typename ELEM, वर्ग QTYPE>
स्थिर तार्किक store_qobj(vthread_t thr, vvp_code_t cp, अचिन्हित wid=0)
{
// FIXME: Can we actually use wid here?
      (शून्य)wid;
      vvp_net_t*net = cp->net;

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      प्रतिज्ञा(queue);

      vvp_object_t src;
      thr->काढा_object(src);

        // If it is null just clear the queue
      जर (src.test_nil())
	    queue->erase_tail(0);
      अन्यथा {
	    अचिन्हित max_size = thr->शब्द[cp->bit_idx[0]].w_int;
	    queue->copy_elems(src, max_size);
      }

      परत सत्य;
}

तार्किक of_STORE_QOBJ_R(vthread_t thr, vvp_code_t cp)
{
      परत store_qobj<द्विगुण, vvp_queue_real>(thr, cp);
}

तार्किक of_STORE_QOBJ_STR(vthread_t thr, vvp_code_t cp)
{
      परत store_qobj<string, vvp_queue_string>(thr, cp);
}

तार्किक of_STORE_QOBJ_V(vthread_t thr, vvp_code_t cp)
{
      परत store_qobj<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

स्थिर शून्य vvp_send(vthread_t thr, vvp_net_ptr_t ptr, स्थिरांक द्विगुण&val)
{
      vvp_send_real(ptr, val, thr->wt_context);
}

स्थिर शून्य vvp_send(vthread_t thr, vvp_net_ptr_t ptr, स्थिरांक string&val)
{
      vvp_send_string(ptr, val, thr->wt_context);
}

ढाँचा <typename ELEM>
स्थिर तार्किक store(vthread_t thr, vvp_code_t cp)
{
      ELEM val;
      काढा_मूल्य(thr, val, 0);
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_send(thr, ptr, val);
      परत सत्य;
}

तार्किक of_STORE_REAL(vthread_t thr, vvp_code_t cp)
{
      परत store<द्विगुण>(thr, cp);
}

ढाँचा <typename ELEM>
स्थिर तार्किक storea(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित idx = cp->bit_idx[0];
      ELEM val;
      काढा_मूल्य(thr, val, 0);

      जर (thr->flags[4] != BIT4_1) {
	    अचिन्हित adr = thr->शब्द[idx].w_int;
	    cp->array->set_word(adr, val);
      }

      परत सत्य;
}

/*
 * %store/reala <var-label> <index>
 */
तार्किक of_STORE_REALA(vthread_t thr, vvp_code_t cp)
{
      परत storea<द्विगुण>(thr, cp);
}

तार्किक of_STORE_STR(vthread_t thr, vvp_code_t cp)
{
      परत store<string>(thr, cp);
}

/*
 * %store/stra <array-label> <index>
 */
तार्किक of_STORE_STRA(vthread_t thr, vvp_code_t cp)
{
      परत storea<string>(thr, cp);
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
तार्किक of_STORE_VEC4(vthread_t thr, vvp_code_t cp)
{
      vvp_net_ptr_t ptr(cp->net, 0);
      स्थिरांक vvp_signal_मूल्य*sig = dynamic_cast<vvp_signal_मूल्य*> (cp->net->fil);
      अचिन्हित off_index = cp->bit_idx[0];
      अचिन्हित पूर्णांक wid = cp->bit_idx[1];

      int64_t off = off_index ? thr->शब्द[off_index].w_int : 0;
      अचिन्हित पूर्णांक sig_मूल्य_size = sig->मूल्य_size();

      vvp_vector4_t&val = thr->peek_vec4();
      अचिन्हित val_size = val.size();

      जर (val_size < wid) {
	    cerr << thr->get_fileline()
	         << "XXXX Internal error: val.size()=" << val_size
		 << ", expecting >= " << wid << endl;
      }
      प्रतिज्ञा(val_size >= wid);
      जर (val_size > wid) {
	    val.resize(wid);
      }

	// If there is a problem loading the index register, flags-4
	// will be set to 1, and we know here to skip the actual assignment.
      जर (off_index!=0 && thr->flags[4] == BIT4_1) {
	    thr->काढा_vec4(1);
	    परत सत्य;
      }

      जर (!resize_rval_vec(val, off, sig_मूल्य_size)) {
	    thr->काढा_vec4(1);
	    परत सत्य;
      }

      जर (off == 0 && val.size() == sig_मूल्य_size)
	    vvp_send_vec4(ptr, val, thr->wt_context);
      अन्यथा
	    vvp_send_vec4_pv(ptr, val, off, sig_मूल्य_size, thr->wt_context);

      thr->काढा_vec4(1);
      परत सत्य;
}

/*
 * %store/vec4a <var-label>, <addr>, <offset>
 */
तार्किक of_STORE_VEC4A(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित adr_index = cp->bit_idx[0];
      अचिन्हित off_index = cp->bit_idx[1];

      दीर्घ adr = adr_index? thr->शब्द[adr_index].w_int : 0;
      int64_t off = off_index ? thr->शब्द[off_index].w_int : 0;

	// Suppress action if flags-4 is true.
      जर (thr->flags[4] == BIT4_1) {
	    thr->काढा_vec4(1);
	    परत सत्य;
      }

      vvp_vector4_t &value = thr->peek_vec4();

      जर (!resize_rval_vec(value, off, cp->array->get_word_size())) {
	    thr->काढा_vec4(1);
	    परत सत्य;
      }

      cp->array->set_word(adr, off, value);

      thr->काढा_vec4(1);
      परत सत्य;
}

/*
 * %sub
 *   pop r;
 *   pop l;
 *   push l-r;
 */
तार्किक of_SUB(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t r = thr->काढा_vec4();
      vvp_vector4_t&l = thr->peek_vec4();

      l.sub(r);
      परत सत्य;
}

/*
 * %subi <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
तार्किक of_SUBI(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित wid = cp->संख्या;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_immediate_rval (cp, r);

      l.sub(r);

      परत सत्य;

}

तार्किक of_SUB_WR(vthread_t thr, vvp_code_t)
{
      द्विगुण r = thr->काढा_real();
      द्विगुण l = thr->काढा_real();
      thr->ढकला_real(l - r);
      परत सत्य;
}

/*
 * %substr <first>, <last>
 * Pop a string, take the substring (SystemVerilog style), and return
 * the result to the stack. This opcode actually works by editing the
 * string in place.
 */
तार्किक of_SUBSTR(vthread_t thr, vvp_code_t cp)
{
      int32_t first = thr->शब्द[cp->bit_idx[0]].w_int;
      int32_t last = thr->शब्द[cp->bit_idx[1]].w_int;
      string&val = thr->peek_str(0);

      जर (first < 0 || last < first || last >= (int32_t)val.size()) {
	    val = string("");
	    परत सत्य;
      }

      val = val.substr(first, last-first+1);
      परत सत्य;
}

/*
 * %substr/vec4 <index>, <wid>
 */
तार्किक of_SUBSTR_VEC4(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित sel_idx = cp->bit_idx[0];
      अचिन्हित wid = cp->bit_idx[1];

      int32_t sel = thr->शब्द[sel_idx].w_int;
      स्थिरांक string&val = thr->peek_str(0);

      प्रतिज्ञा(wid%8 == 0);

      जर (sel < 0 || sel >= (int32_t)val.size()) {
	    vvp_vector4_t res (wid, BIT4_0);
	    thr->ढकला_vec4(res);
	    परत सत्य;
      }

      vvp_vector4_t res (wid, BIT4_0);

      प्रतिज्ञा(wid==8);
      अचिन्हित अक्षर tmp = val[sel];
      साठी (पूर्णांक idx = 0 ; idx < 8 ; idx += 1) {
	    जर (tmp & (1<<idx))
		  res.set_bit(idx, BIT4_1);
      }

      thr->ढकला_vec4(res);
      परत सत्य;
}

तार्किक of_FILE_ओळ(vthread_t thr, vvp_code_t cp)
{
      vpiHandle handle = cp->handle;

	/* When it is available, keep the file/line information in the
	   thread for error/warning messages. */
      thr->set_fileline(vpi_get_str(vpiFile, handle),
                        vpi_get(vpi_ओळ_No, handle));

      जर (show_file_ओळ)
	    cerr << thr->get_fileline()
	         << vpi_get_str(_vpiDescription, handle) << endl;

      परत सत्य;
}

/*
 * %test_nul <var-label>;
 * Test if the object at the specified variable is nil. If so, write
 * "1" into flags[4], otherwise write "0" into flags[4].
 */
तार्किक of_TEST_NUL(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t*net = cp->net;

      प्रतिज्ञा(net);
      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      प्रतिज्ञा(obj);

      जर (obj->get_object().test_nil())
	    thr->flags[4] = BIT4_1;
      अन्यथा
	    thr->flags[4] = BIT4_0;

      परत सत्य;
}

तार्किक of_TEST_NUL_A(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित idx = cp->bit_idx[0];
      अचिन्हित adr = thr->शब्द[idx].w_int;
      vvp_object_t word;

	/* If the address is undefined, return true. */
      जर (thr->flags[4] == BIT4_1) {
	    परत सत्य;
      }

      cp->array->get_word_obj(adr, word);
      जर (word.test_nil())
	    thr->flags[4] = BIT4_1;
      अन्यथा
	    thr->flags[4] = BIT4_0;

      परत सत्य;
}

तार्किक of_TEST_NUL_OBJ(vthread_t thr, vvp_code_t)
{
      जर (thr->peek_object().test_nil())
	    thr->flags[4] = BIT4_1;
      अन्यथा
	    thr->flags[4] = BIT4_0;
      परत सत्य;
}

/*
 * %test_nul/prop <pid>, <idx>
 */
तार्किक of_TEST_NUL_PROP(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित pid = cp->संख्या;
      अचिन्हित idx = cp->bit_idx[0];

      जर (idx != 0) {
	    प्रतिज्ञा(idx < vthread_s::शब्द_मोजणी);
	    idx = thr->शब्द[idx].w_uint;
      }

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj  = obj.peek<vvp_cobject>();

      vvp_object_t val;
      cobj->get_object(pid, val, idx);

      जर (val.test_nil())
	    thr->flags[4] = BIT4_1;
      अन्यथा
	    thr->flags[4] = BIT4_0;

      परत सत्य;
}

/* and() reduction returns the scalar sum (Verilog-style bitwise-AND per word). */
ढाँचा <वर्ग SRC>
स्थिर vvp_vector4_t uparr_and_शब्द_src(SRC* src, अचिन्हित wid)
{
      जर (!src) परत vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_1);
      साठी (माप_प्रकार i = 0; i < src->get_size(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    res &= vi;
      }
      परत res;
}

तार्किक of_UPARR_AND_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      अचिन्हित wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      जर (qsrc) res = uparr_and_शब्द_src(qsrc, wid);
      अन्यथा जर (dsrc) res = uparr_and_शब्द_src(dsrc, wid);
      अन्यथा {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    res = uparr_and_शब्द_src(src, wid);
      }
      thr->ढकला_vec4(res);
      परत सत्य;
}

/* or() reduction returns the scalar sum (Verilog-style bitwise-AND per word). */
ढाँचा <वर्ग SRC>
स्थिर vvp_vector4_t uparr_or_शब्द_src(SRC* src, अचिन्हित wid)
{
      जर (!src) परत vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_0);
      साठी (माप_प्रकार i = 0; i < src->get_size(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    res |= vi;
      }
      परत res;
}

तार्किक of_UPARR_OR_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      अचिन्हित wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      जर (qsrc) res = uparr_or_शब्द_src(qsrc, wid);
      अन्यथा जर (dsrc) res = uparr_or_शब्द_src(dsrc, wid);
      अन्यथा {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    res = uparr_or_शब्द_src(src, wid);
      }
      thr->ढकला_vec4(res);
      परत सत्य;
}

/* real product() reduction returns the scalar sum */
ढाँचा <वर्ग SRC>
स्थिर द्विगुण uparr_product_शब्द_src_real(SRC* src)
{
      जर (!src) परत 0.0;
      द्विगुण acc = 1.0;
      साठी (माप_प्रकार i = 0; i < src->get_size(); i += 1) {
	    द्विगुण vi;
	    src->get_word(i, vi);
	    acc *= vi;
      }
      परत acc;
}

तार्किक of_UPARR_PRODUCT_REAL(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_queue_real* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_real_from_net(thr, net, qsrc, dsrc);
      द्विगुण sum;
      जर (qsrc) sum = uparr_product_शब्द_src_real(qsrc);
      अन्यथा जर (dsrc) sum = uparr_product_शब्द_src_real(dsrc);
      अन्यथा {
	    vvp_queue* src_q = get_queue_object<vvp_queue_real>(thr, net);
	    vvp_queue_real* src = dynamic_cast<vvp_queue_real*>(src_q);
	    sum = uparr_product_शब्द_src_real(src);
      }
      thr->ढकला_real(sum);
      परत सत्य;
}

/* product() reduction returns the scalar product (Verilog-style multiplication per word). */
ढाँचा <वर्ग SRC>
स्थिर vvp_vector4_t uparr_product_शब्द_src(SRC* src, अचिन्हित wid)
{
      जर (!src) परत vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t acc = queue_unique_ulong_to_vec4(1UL, wid);
      साठी (माप_प्रकार i = 0; i < src->get_size(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    acc.mul(vi);
      }
      परत acc;
}

तार्किक of_UPARR_PRODUCT_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      अचिन्हित wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t prod;
      जर (qsrc) prod = uparr_product_शब्द_src(qsrc, wid);
      अन्यथा जर (dsrc) prod = uparr_product_शब्द_src(dsrc, wid);
      अन्यथा {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    prod = uparr_product_शब्द_src(src, wid);
      }
      thr->ढकला_vec4(prod);
      परत सत्य;
}

तार्किक of_UPARR_PRODUCT_PROP_V(vthread_t thr, vvp_code_t cp)
{
      माप_प्रकार pid = cp->संख्या;
      अचिन्हित wid = cp->bit_idx[0];

      vvp_object_t& शीर्ष = thr->peek_object();
      vvp_cobject*cobj = शीर्ष.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t prod =
	    qsrc ? uparr_product_शब्द_src(qsrc, wid)
		 : uparr_product_शब्द_src(dsrc, wid);
      thr->ढकला_vec4(prod);
      परत सत्य;
}

तार्किक of_UPARR_PRODUCT_OBJ_V(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->काढा_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_vector4_t prod =
	    qsrc ? uparr_product_शब्द_src(qsrc, wid)
		 : uparr_product_शब्द_src(dsrc, wid);
      thr->ढकला_vec4(prod);
      परत सत्य;
}

/* real sum() reduction returns the scalar sum */
ढाँचा <वर्ग SRC>
स्थिर द्विगुण uparr_sum_शब्द_src_real(SRC* src)
{
      द्विगुण acc = 0.0;
      जर (!src) परत acc;
      साठी (माप_प्रकार i = 0; i < src->get_size(); i += 1) {
	    द्विगुण vi;
	    src->get_word(i, vi);
	    acc += vi;
      }
      परत acc;
}

तार्किक of_UPARR_SUM_REAL(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_queue_real* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_real_from_net(thr, net, qsrc, dsrc);
      द्विगुण sum;
      जर (qsrc) sum = uparr_sum_शब्द_src_real(qsrc);
      अन्यथा जर (dsrc) sum = uparr_sum_शब्द_src_real(dsrc);
      अन्यथा {
	    vvp_queue* src_q = get_queue_object<vvp_queue_real>(thr, net);
	    vvp_queue_real* src = dynamic_cast<vvp_queue_real*>(src_q);
	    sum = uparr_sum_शब्द_src_real(src);
      }
      thr->ढकला_real(sum);
      परत सत्य;
}

/* sum() reduction returns the scalar sum (Verilog-style addition per word). */
ढाँचा <वर्ग SRC>
स्थिर vvp_vector4_t uparr_sum_शब्द_src(SRC* src, अचिन्हित wid)
{
      जर (!src) परत vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t acc(wid, BIT4_0);
      साठी (माप_प्रकार i = 0; i < src->get_size(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    acc.add(vi);
      }
      परत acc;
}

तार्किक of_UPARR_SUM_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      अचिन्हित wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t sum;
      जर (qsrc) sum = uparr_sum_शब्द_src(qsrc, wid);
      अन्यथा जर (dsrc) sum = uparr_sum_शब्द_src(dsrc, wid);
      अन्यथा {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    sum = uparr_sum_शब्द_src(src, wid);
      }
      thr->ढकला_vec4(sum);
      परत सत्य;
}

तार्किक of_UPARR_SUM_PROP_V(vthread_t thr, vvp_code_t cp)
{
      माप_प्रकार pid = cp->संख्या;
      अचिन्हित wid = cp->bit_idx[0];

      vvp_object_t& शीर्ष = thr->peek_object();
      vvp_cobject*cobj = शीर्ष.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t sum =
	    qsrc ? uparr_sum_शब्द_src(qsrc, wid)
		 : uparr_sum_शब्द_src(dsrc, wid);
      thr->ढकला_vec4(sum);
      परत सत्य;
}

तार्किक of_UPARR_SUM_OBJ_V(vthread_t thr, vvp_code_t cp)
{
      अचिन्हित wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->काढा_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_vector4_t sum =
	    qsrc ? uparr_sum_शब्द_src(qsrc, wid)
		 : uparr_sum_शब्द_src(dsrc, wid);
      thr->ढकला_vec4(sum);
      परत सत्य;
}

/* xor() reduction returns the scalar sum (Verilog-style bitwise-XOR per word). */
ढाँचा <वर्ग SRC>
स्थिर vvp_vector4_t uparr_xor_शब्द_src(SRC* src, अचिन्हित wid)
{
      जर (!src) परत vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_0);
      साठी (माप_प्रकार i = 0; i < src->get_size(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    res ^= vi;
      }
      परत res;
}

तार्किक of_UPARR_XOR_V(vthread_t thr, vvp_code_t cp)
{
      vvp_net_t* net = cp->net;
      अचिन्हित wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      जर (qsrc) res = uparr_xor_शब्द_src(qsrc, wid);
      अन्यथा जर (dsrc) res = uparr_xor_शब्द_src(dsrc, wid);
      अन्यथा {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    res = uparr_xor_शब्द_src(src, wid);
      }
      thr->ढकला_vec4(res);
      परत सत्य;
}

तार्किक of_VPI_CALL(vthread_t thr, vvp_code_t cp)
{
      vpip_execute_vpi_call(thr, cp->handle);

      जर (schedule_stopped()) {
	    जर (! schedule_finished())
		  schedule_vthread(thr, 0, असत्य);

	    परत असत्य;
      }

      परत schedule_finished()? असत्य : सत्य;
}

/* %wait <label>;
 * Implement the wait by locating the vvp_net_T for the event, and
 * adding this thread to the threads list for the event. The some
 * argument is the  reference to the functor to wait for. This must be
 * an event object of some sort.
 */
तार्किक of_WAIT(vthread_t thr, vvp_code_t cp)
{
      प्रतिज्ञा(! thr->i_am_in_function);
      प्रतिज्ञा(! thr->waiting_for_event);
      thr->waiting_for_event = 1;

	/* Add this thread to the list in the event. */
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (cp->net->fun);
      प्रतिज्ञा(ep);
      thr->wait_next = ep->add_waiting_thread(thr);

	/* Return false to suspend this thread. */
      परत असत्य;
}

/*
 * Implement the %wait/fork (SystemVerilog) instruction by suspending
 * the current thread until all the detached children have finished.
 */
तार्किक of_WAIT_FORK(vthread_t thr, vvp_code_t)
{
	/* If a %wait/fork is being executed then the parent thread
	 * cannot be waiting in a join or already waiting. */
      प्रतिज्ञा(! thr->i_am_in_function);
      प्रतिज्ञा(! thr->i_am_joining);
      प्रतिज्ञा(! thr->i_am_waiting);

	/* There should be no active children when waiting. */
      प्रतिज्ञा(thr->children.empty());

	/* If there are no detached children then there is nothing to
	 * wait for. */
      जर (thr->detached_children.empty()) परत सत्य;

	/* Flag that this process is waiting for the detached children
	 * to finish and suspend it. */
      thr->i_am_waiting = 1;
      परत असत्य;
}

/*
 * %xnor
 */
तार्किक of_XNOR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valr = thr->काढा_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      प्रतिज्ञा(vall.size() == valr.size());

      vall ^= valr;
      vall.invert();

      परत सत्य;
}

/*
 * %xor
 */
तार्किक of_XOR(vthread_t thr, vvp_code_t)
{
      vvp_vector4_t valr = thr->काढा_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      प्रतिज्ञा(vall.size() == valr.size());

      vall ^= valr;

      परत सत्य;
}


तार्किक of_ZOMBIE(vthread_t thr, vvp_code_t)
{
      thr->pc = codespace_null();
      जर ((thr->parent == 0) && (thr->children.empty())) {
	    जर (thr->delay_delete)
		  schedule_del_thr(thr);
	    अन्यथा
		  vthread_delete(thr);
      }
      परत असत्य;
}

/*
 * This is a phantom opcode used to call user defined functions. It
 * is used in code generated by the .ufunc statement. It contains a
 * pointer to the executable code of the function and a pointer to
 * a ufunc_core object that has all the port information about the
 * function.
 */
स्थिर तार्किक do_exec_ufunc(vthread_t thr, vvp_code_t cp, vthread_t child)
{
      __vpiScope*child_scope = cp->ufunc_core_ptr->func_scope();
      प्रतिज्ञा(child_scope);

      प्रतिज्ञा(child_scope->get_type_code() == vpiFunction);
      प्रतिज्ञा(thr->children.empty());


        /* We can take a number of shortcuts because we know that a
           continuous assignment can only occur in a static scope. */
      प्रतिज्ञा(thr->wt_context == 0);
      प्रतिज्ञा(thr->rd_context == 0);

        /* If an automatic function, allocate a context for this call. */
      vvp_context_t child_context = 0;
      जर (child_scope->is_automatic()) {
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
      प्रतिज्ञा(thr->children.size()==1);

      child->is_scheduled = 1;
      child->i_am_in_function = 1;
      vthread_run(child);
      running_thread = thr;

      जर (child->i_have_ended) {
	    do_join(thr, child);
            परत सत्य;
      } अन्यथा {
	    thr->i_am_joining = 1;
	    परत असत्य;
      }
}

तार्किक of_EXEC_UFUNC_REAL(vthread_t thr, vvp_code_t cp)
{
      __vpiScope*child_scope = cp->ufunc_core_ptr->func_scope();
      प्रतिज्ञा(child_scope);

	/* Create a temporary thread and run it immediately. */
      vthread_t child = vthread_new(cp->cptr, child_scope);
      thr->ढकला_real(0.0);
      child->args_real.push_back(0);

      परत do_exec_ufunc(thr, cp, child);
}

तार्किक of_EXEC_UFUNC_VEC4(vthread_t thr, vvp_code_t cp)
{
      __vpiScope*child_scope = cp->ufunc_core_ptr->func_scope();
      प्रतिज्ञा(child_scope);

      vpiScopeFunction*scope_func = dynamic_cast<vpiScopeFunction*>(child_scope);
      प्रतिज्ञा(scope_func);

	/* Create a temporary thread and run it immediately. */
      vthread_t child = vthread_new(cp->cptr, child_scope);
      thr->ढकला_vec4(vvp_vector4_t(scope_func->get_func_width(), scope_func->get_func_init_val()));
      child->args_vec4.push_back(0);

      परत do_exec_ufunc(thr, cp, child);
}

/*
 * This is a phantom opcode used to harvest the result of calling a user
 * defined function. It is used in code generated by the .ufunc statement.
 */
तार्किक of_REAP_UFUNC(vthread_t thr, vvp_code_t cp)
{
      __vpiScope*child_scope = cp->ufunc_core_ptr->func_scope();
      प्रतिज्ञा(child_scope);

	/* Copy the output from the result variable to the output
	   ports of the .ufunc device. */
      cp->ufunc_core_ptr->finish_thread();

        /* If an automatic function, free the context for this call. */
      जर (child_scope->is_automatic()) {
            vthread_free_context(thr->rd_context, child_scope);
            thr->wt_context = 0;
            thr->rd_context = 0;
      }

      परत सत्य;
}
