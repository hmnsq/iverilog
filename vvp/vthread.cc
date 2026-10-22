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
#यदि_परिभाषितम् CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#यदि_अन्तः
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

उपयुज्य नामाकाश std;

/* This is the size of an unsigned long in bits. This is just a
   convenience macro. */
# define CPU_WORD_द्वयङ्काः (8*परिमाण(अचिह्नित दीर्घ))
# define शिखरम्_BIT (1UL << (CPU_WORD_द्वयङ्काः-1))

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

      शून्य debug_dump(ostream&fd, ध्रुव वर्ण*label_text);

	/* This is the program counter. */
      vvp_संहिता_t pc;
	/* These hold the private thread bits. */
      गणना { FLAGS_गणनम् = 512, शब्दाः_गणनम् = 16 };
      vvp_bit4_t flags[FLAGS_गणनम्];

	/* These are the word registers. */
      सङ्घ {
	    int64_t  w_int;
	    uint64_t w_uint;
      } शब्दाः[शब्दाः_गणनम्];

	// These vectors are depths within the parent thread's
	// corresponding stack.  This is how the %ret/* instructions
	// get at parent thread arguments.
      vector<अचिह्नित> तर्काः_real;
      vector<अचिह्नित> तर्काः_str;
      vector<अचिह्नित> तर्काः_vec4;

    गुप्त:
      vector<vvp_vector4_t>राशिः_vec4;
    सार्वजनिक:
      अन्तःस्थ vvp_vector4_t निष्कासय_vec4(शून्य)
      {
	    प्रतिज्ञा(! राशिः_vec4.empty());
	    vvp_vector4_t val = राशिः_vec4.back();
	    राशिः_vec4.pop_back();
	    प्रत्यागम val;
      }
      अन्तःस्थ शून्य प्रक्षिप_vec4(ध्रुव vvp_vector4_t&val)
      {
	    राशिः_vec4.push_back(val);
      }
      अन्तःस्थ ध्रुव vvp_vector4_t& peek_vec4(अचिह्नित गभीरता)
      {
	    अचिह्नित size = राशिः_vec4.size();
	    प्रतिज्ञा(गभीरता < size);
	    अचिह्नित use_सूचकांक = size-1-गभीरता;
	    प्रत्यागम राशिः_vec4[use_सूचकांक];
      }
      अन्तःस्थ vvp_vector4_t& peek_vec4(शून्य)
      {
	    अचिह्नित use_सूचकांक = राशिः_vec4.size();
	    प्रतिज्ञा(use_सूचकांक >= 1);
	    प्रत्यागम राशिः_vec4[use_सूचकांक-1];
      }
      अन्तःस्थ शून्य poke_vec4(अचिह्नित गभीरता, ध्रुव vvp_vector4_t&val)
      {
	    प्रतिज्ञा(गभीरता < राशिः_vec4.size());
	    अचिह्नित use_सूचकांक = राशिः_vec4.size()-1-गभीरता;
	    राशिः_vec4[use_सूचकांक] = val;
      }
      अन्तःस्थ शून्य निष्कासय_vec4(अचिह्नित cnt)
      {
	    यावत् (cnt > 0) {
		  राशिः_vec4.pop_back();
		  cnt -= 1;
	    }
      }


    गुप्त:
      vector<द्विगुण> राशिः_real;
    सार्वजनिक:
      अन्तःस्थ द्विगुण निष्कासय_real(शून्य)
      {
	    प्रतिज्ञा(! राशिः_real.empty());
	    द्विगुण val = राशिः_real.back();
	    राशिः_real.pop_back();
	    प्रत्यागम val;
      }
      अन्तःस्थ शून्य प्रक्षिप_real(द्विगुण val)
      {
	    राशिः_real.push_back(val);
      }
      अन्तःस्थ द्विगुण peek_real(अचिह्नित गभीरता)
      {
	    प्रतिज्ञा(गभीरता < राशिः_real.size());
	    अचिह्नित use_सूचकांक = राशिः_real.size()-1-गभीरता;
	    प्रत्यागम राशिः_real[use_सूचकांक];
      }
      अन्तःस्थ शून्य poke_real(अचिह्नित गभीरता, द्विगुण val)
      {
	    प्रतिज्ञा(गभीरता < राशिः_real.size());
	    अचिह्नित use_सूचकांक = राशिः_real.size()-1-गभीरता;
	    राशिः_real[use_सूचकांक] = val;
      }
      अन्तःस्थ शून्य निष्कासय_real(अचिह्नित cnt)
      {
	    यावत् (cnt > 0) {
		  राशिः_real.pop_back();
		  cnt -= 1;
	    }
      }

	/* Strings are operated on using a forth-like operator
	   set. Items at the top of the stack (back()) are the objects
	   operated on except for special cases. New objects are
	   pushed onto the top (back()) and pulled from the top
	   (back()) only. */
    गुप्त:
      vector<string> राशिः_str;
    सार्वजनिक:
      अन्तःस्थ string निष्कासय_str(शून्य)
      {
	    प्रतिज्ञा(! राशिः_str.empty());
	    string val = राशिः_str.back();
	    राशिः_str.pop_back();
	    प्रत्यागम val;
      }
      अन्तःस्थ शून्य प्रक्षिप_str(ध्रुव string&val)
      {
	    राशिः_str.push_back(val);
      }
      अन्तःस्थ string&peek_str(अचिह्नित गभीरता)
      {
	    प्रतिज्ञा(गभीरता<राशिः_str.size());
	    अचिह्नित use_सूचकांक = राशिः_str.size()-1-गभीरता;
	    प्रत्यागम राशिः_str[use_सूचकांक];
      }
      अन्तःस्थ शून्य poke_str(अचिह्नित गभीरता, ध्रुव string&val)
      {
	    प्रतिज्ञा(गभीरता < राशिः_str.size());
	    अचिह्नित use_सूचकांक = राशिः_str.size()-1-गभीरता;
	    राशिः_str[use_सूचकांक] = val;
      }
      अन्तःस्थ शून्य निष्कासय_str(अचिह्नित cnt)
      {
	    यावत् (cnt > 0) {
		  राशिः_str.pop_back();
		  cnt -= 1;
	    }
      }

	/* Objects are also operated on in a stack. */
    गुप्त:
      गणना { राशिः_OBJ_MAX_आकार = 32 };
      vvp_object_t राशिः_obj[राशिः_OBJ_MAX_आकार];
      अचिह्नित राशिः_obj_आकार;
    सार्वजनिक:
      अन्तःस्थ vvp_object_t& peek_object(शून्य)
      {
	    प्रतिज्ञा(राशिः_obj_आकार > 0);
	    प्रत्यागम राशिः_obj[राशिः_obj_आकार-1];
      }
      अन्तःस्थ शून्य निष्कासय_object(vvp_object_t&obj)
      {
	    प्रतिज्ञा(राशिः_obj_आकार > 0);
	    राशिः_obj_आकार -= 1;
	    obj = राशिः_obj[राशिः_obj_आकार];
	    राशिः_obj[राशिः_obj_आकार].reset(0);
      }
      अन्तःस्थ शून्य निष्कासय_object(अचिह्नित cnt, अचिह्नित skip =0)
      {
	    प्रतिज्ञा((cnt+skip) <= राशिः_obj_आकार);
	    प्रति (size_t idx = राशिः_obj_आकार-skip-cnt ; idx < राशिः_obj_आकार-skip ; idx += 1)
		  राशिः_obj[idx].reset(0);
	    राशिः_obj_आकार -= cnt;
	    प्रति (size_t idx = राशिः_obj_आकार-skip ; idx < राशिः_obj_आकार ; idx += 1)
		  राशिः_obj[idx] = राशिः_obj[idx+skip];
	    प्रति (size_t idx = राशिः_obj_आकार ; idx < राशिः_obj_आकार+skip ; idx += 1)
		  राशिः_obj[idx].reset(0);
      }
      अन्तःस्थ शून्य प्रक्षिप_object(ध्रुव vvp_object_t&obj)
      {
	    प्रतिज्ञा(राशिः_obj_आकार < राशिः_OBJ_MAX_आकार);
	    राशिः_obj[राशिः_obj_आकार] = obj;
	    राशिः_obj_आकार += 1;
      }

	/* My parent sets this when it wants me to wake it up. */
      अचिह्नित i_am_joining      :1;
      अचिह्नित i_am_detached     :1;
      अचिह्नित i_am_प्रतीक्षा      :1;
      अचिह्नित i_am_अन्तर्_function  :1; // True if running function code
      अचिह्नित i_have_ended      :1;
      अचिह्नित i_was_disabled    :1;
      अचिह्नित प्रतीक्षा_for_event :1;
      अचिह्नित is_scheduled      :1;
      अचिह्नित delay_delete      :1;
	/* This points to the children of the thread. */
      समुच्चयः<संरचना vthread_s*>children;
	/* This points to the detached children of the thread. */
      समुच्चयः<संरचना vthread_s*>detached_children;
	/* This points to my parent, if I have one. */
      संरचना vthread_s*parent;
	/* This points to the containing scope. */
      __vpiScope*parent_scope;
	/* This is used for keeping wait queues. */
      संरचना vthread_s*wait_अग्र;
	/* These are used to access automatically allocated items. */
      vvp_context_t wt_context, rd_context;
	/* These are used to pass non-blocking event control information. */
      vvp_net_t*event;
      uint64_t ecount;
	/* Save the file/line information when available. */
    गुप्त:
      वर्ण *filenm_;
      अचिह्नित lineno_;
    सार्वजनिक:
      शून्य समुच्चयः_fileline(वर्ण *filenm, अचिह्नित lineno);
      string get_fileline();

      अन्तःस्थ शून्य cleanup()
      {
	    यदि (i_was_disabled) {
		  राशिः_vec4.clear();
		  राशिः_real.clear();
		  राशिः_str.clear();
		  निष्कासय_object(राशिः_obj_आकार);
	    }
	    मुक्त(filenm_);
	    filenm_ = 0;
	    प्रतिज्ञा(राशिः_vec4.empty());
	    प्रतिज्ञा(राशिः_real.empty());
	    प्रतिज्ञा(राशिः_str.empty());
	    प्रतिज्ञा(राशिः_obj_आकार == 0);
      }
};

अन्तःस्थ vthread_s::vthread_s()
{
      राशिः_obj_आकार = 0;
      filenm_ = 0;
      lineno_ = 0;
}

शून्य vthread_s::समुच्चयः_fileline(वर्ण *filenm, अचिह्नित lineno)
{
      प्रतिज्ञा(filenm);
      यदि (!filenm_ || (वाक्यं_तुलय(filenm_, filenm) != 0)) {
	    मुक्त(filenm_);
	    filenm_ = strdup(filenm);
      }
      lineno_ = lineno;
}

अन्तःस्थ string vthread_s::get_fileline()
{
      ostringstream buf;
      यदि (filenm_) {
	    buf << filenm_ << ":" << lineno_ << ": ";
      }
      string res = buf.str();
      प्रत्यागम res;
}

शून्य vthread_s::debug_dump(ostream&fd, ध्रुव वर्ण*label)
{
      fd << "**** " << label << पङ्क्त्यन्तः;
      fd << "**** ThreadId: " << एतत् << ", parent id: " << parent << पङ्क्त्यन्तः;

      fd << "**** Flags: ";
      प्रति (पूर्णाङ्क idx = 0 ; idx < FLAGS_गणनम् ; idx += 1)
	    fd << flags[idx];
      fd << पङ्क्त्यन्तः;
      fd << "**** vec4 stack..." << पङ्क्त्यन्तः;
      प्रति (size_t idx = राशिः_vec4.size() ; idx > 0 ; idx -= 1)
	    fd << "    " << (राशिः_vec4.size()-idx) << ": " << राशिः_vec4[idx-1] << पङ्क्त्यन्तः;
      fd << "**** str stack (" << राशिः_str.size() << ")..." << पङ्क्त्यन्तः;
      fd << "**** obj stack (" << राशिः_obj_आकार << ")..." << पङ्क्त्यन्तः;
      fd << "**** args_vec4 array (" << तर्काः_vec4.size() << ")..." << पङ्क्त्यन्तः;
      प्रति (size_t idx = 0 ; idx < तर्काः_vec4.size() ; idx += 1)
	    fd << "    " << idx << ": " << तर्काः_vec4[idx] << पङ्क्त्यन्तः;
      fd << "**** file/line (";
      यदि (filenm_) fd << filenm_;
      अन्यथा fd << "<no file name>";
      fd << ":" << lineno_ << ")" << पङ्क्त्यन्तः;
      fd << "**** Done ****" << पङ्क्त्यन्तः;
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
स्थिर string filter_string(ध्रुव वर्ण*text)
{
      vector<वर्ण> tmp (वाक्यदैर्घ्यम्(text)+1);
      size_t dst = 0;
      प्रति (ध्रुव वर्ण*ptr = text ; *ptr ; ptr += 1) {
	    // Not an escape? Move on.
	    यदि (*ptr != '\\') {
		  tmp[dst++] = *ptr;
		  अनुवर्त;
	    }

	    // Now we know that *ptr is pointing to a \ character and we
	    // have an octal sequence coming up. Advance the ptr and start
	    // processing octal digits.
	    ptr += 1;
	    यदि (*ptr == 0)
		  भङ्ग;

	    वर्ण byte = 0;
	    पूर्णाङ्क cnt = 3;
	    यावत् (*ptr && cnt > 0 && *ptr >= '0' && *ptr <= '7') {
		  byte *= 8;
		  byte += *ptr - '0';
		  cnt -= 1;
		  ptr += 1;
	    }

	    // null-bytes are supposed to be removed when assigning a string
	    // literal to a string.
	    यदि (byte != '\0')
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
      प्रत्यागम res;
}

स्थिर शून्य do_join(vthread_t thr, vthread_t child);

__vpiScope* vthread_scope(संरचना vthread_s*thr)
{
      प्रत्यागम thr->parent_scope;
}

संरचना vthread_s*running_thread = 0;

string get_fileline()
{
      प्रत्यागम running_thread->get_fileline();
}

शून्य vthread_प्रक्षिप(संरचना vthread_s*thr, द्विगुण val)
{
      thr->प्रक्षिप_real(val);
}

शून्य vthread_प्रक्षिप(संरचना vthread_s*thr, ध्रुव string&val)
{
      thr->प्रक्षिप_str(val);
}

शून्य vthread_प्रक्षिप(संरचना vthread_s*thr, ध्रुव vvp_vector4_t&val)
{
      thr->प्रक्षिप_vec4(val);
}

शून्य vthread_निष्कासय_real(संरचना vthread_s*thr, अचिह्नित गभीरता)
{
      thr->निष्कासय_real(गभीरता);
}

शून्य vthread_निष्कासय_str(संरचना vthread_s*thr, अचिह्नित गभीरता)
{
      thr->निष्कासय_str(गभीरता);
}

शून्य vthread_निष्कासय_vec4(संरचना vthread_s*thr, अचिह्नित गभीरता)
{
      thr->निष्कासय_vec4(गभीरता);
}

द्विगुण vthread_get_real_राशिः(संरचना vthread_s*thr, अचिह्नित गभीरता)
{
      प्रत्यागम thr->peek_real(गभीरता);
}

ध्रुव string&vthread_get_str_राशिः(संरचना vthread_s*thr, अचिह्नित गभीरता)
{
      प्रत्यागम thr->peek_str(गभीरता);
}

ध्रुव vvp_vector4_t& vthread_get_vec4_राशिः(संरचना vthread_s*thr, अचिह्नित गभीरता)
{
      प्रत्यागम thr->peek_vec4(गभीरता);
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
प्रतिरूपम् <वर्ग VVP_QUEUE> स्थिर vvp_queue*get_queue_object(vthread_t thr, vvp_net_t*net)
{
      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      प्रतिज्ञा(obj);

      vvp_queue*queue = obj->get_object().peek<vvp_queue>();
      यदि (queue == 0) {
	    प्रतिज्ञा(obj->get_object().test_nil());
	    queue = नूतन VVP_QUEUE;
	    vvp_object_t val (queue);
	    vvp_net_ptr_t ptr (net, 0);
	    vvp_send_object(ptr, val, thr->wt_context);
      }

      प्रत्यागम queue;
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
      vvp_object_t शिखरम् = obj->get_object();
      qsrc = शिखरम्.peek<vvp_queue_vec4>();
      यदि (qsrc) {
	    dsrc = 0;
	    प्रत्यागम;
      }
      vvp_darray* dany = शिखरम्.peek<vvp_darray>();
      यदि (dany && dynamic_cast<vvp_queue*>(dany) == 0) {
	    dsrc = dany;
	    प्रत्यागम;
      }
      यदि (शिखरम्.test_nil()) {
	    qsrc = नूतन vvp_queue_vec4();
	    vvp_object_t val(qsrc);
	    vvp_net_ptr_t ptr(net, 0);
	    vvp_send_object(ptr, val, thr->wt_context);
	    dsrc = 0;
	    प्रत्यागम;
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
      vvp_object_t शिखरम् = obj->get_object();
      qsrc = शिखरम्.peek<vvp_queue_real>();
      यदि (qsrc) {
	    dsrc = 0;
	    प्रत्यागम;
      }
      vvp_darray* dany = शिखरम्.peek<vvp_darray>();
      यदि (dany && dynamic_cast<vvp_queue*>(dany) == 0) {
	    dsrc = dany;
	    प्रत्यागम;
      }
      यदि (शिखरम्.test_nil()) {
	    qsrc = नूतन vvp_queue_real();
	    vvp_object_t val(qsrc);
	    vvp_net_ptr_t ptr(net, 0);
	    vvp_send_object(ptr, val, thr->wt_context);
	    dsrc = 0;
	    प्रत्यागम;
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
      यदि (qsrc) {
	    dsrc = 0;
	    प्रत्यागम;
      }
      vvp_darray* dany = src_obj.peek<vvp_darray>();
      यदि (dany && dynamic_cast<vvp_queue*>(dany) == 0) {
	    dsrc = dany;
	    प्रत्यागम;
      }
      qsrc = 0;
      dsrc = 0;
}

/*
 * The following are used to allow a common template to be written for
 * queue real/string/vec4 operations
 */
अन्तःस्थ स्थिर शून्य निष्कासय_मानम्(vthread_t thr, द्विगुण&value, अचिह्नित)
{
      value = thr->निष्कासय_real();
}

अन्तःस्थ स्थिर शून्य निष्कासय_मानम्(vthread_t thr, string&value, अचिह्नित)
{
      value = thr->निष्कासय_str();
}

अन्तःस्थ स्थिर शून्य निष्कासय_मानम्(vthread_t thr, vvp_vector4_t&value, अचिह्नित wid)
{
      value = thr->निष्कासय_vec4();
      प्रतिज्ञा(value.size() == wid);
}

/*
 * The following are used to allow the queue templates to print correctly.
 */
अन्तःस्थ स्थिर string get_queue_type(द्विगुण&)
{
      प्रत्यागम "queue<real>";
}

अन्तःस्थ स्थिर string get_queue_type(string&)
{
      प्रत्यागम "queue<string>";
}

अन्तःस्थ स्थिर string get_queue_type(ध्रुव vvp_vector4_t&value)
{
      ostringstream buf;
      buf << "queue<vector[" << value.size() << "]>";
      string res = buf.str();
      प्रत्यागम res;
}

अन्तःस्थ स्थिर शून्य print_queue_मानम्(द्विगुण value)
{
      cerr << value;
}

अन्तःस्थ स्थिर शून्य print_queue_मानम्(ध्रुव string&value)
{
      cerr << "\"" << value << "\"";
}

अन्तःस्थ स्थिर शून्य print_queue_मानम्(ध्रुव vvp_vector4_t&value)
{
      cerr << value;
}

/*
 * The following are used to get a darray/queue default value.
 */
अन्तःस्थ स्थिर शून्य dq_default(द्विगुण&value, अचिह्नित)
{
      value = 0.0;
}

अन्तःस्थ स्थिर शून्य dq_default(string&value, अचिह्नित)
{
      value = "";
}

अन्तःस्थ स्थिर शून्य dq_default(vvp_vector4_t&value, अचिह्नित wid)
{
      value = vvp_vector4_t(wid);
}


प्रतिरूपम् <वर्ग T> T coerce_गन्तव्यम्_विस्तारः(ध्रुव T&that, अचिह्नित width)
{
      यदि (that.size() == width)
	    प्रत्यागम that;

      प्रतिज्ञा(that.size() > width);
      T res (width);
      प्रति (अचिह्नित idx = 0 ;  idx < width ;  idx += 1)
	    res.समुच्चयः_bit(idx, that.value(idx));

      प्रत्यागम res;
}

/* Explicitly define the vvp_vector4_t version of coerce_to_width(). */
प्रतिरूपम् vvp_vector4_t coerce_गन्तव्यम्_विस्तारः(ध्रुव vvp_vector4_t&that,
                                       अचिह्नित width);


स्थिर शून्य multiply_array_imm(अचिह्नित दीर्घ*res, ध्रुव अचिह्नित दीर्घ*val,
			       अचिह्नित शब्दाः, अचिह्नित दीर्घ imm)
{
      प्रति (अचिह्नित idx = 0 ; idx < शब्दाः ; idx += 1)
	    res[idx] = 0;

      प्रति (अचिह्नित mul_idx = 0 ; mul_idx < शब्दाः ; mul_idx += 1) {
	    अचिह्नित दीर्घ योग;
	    अचिह्नित दीर्घ tmp = multiply_with_carry(val[mul_idx], imm, योग);

	    अचिह्नित दीर्घ carry = 0;
	    res[mul_idx] = सङ्कलय_with_carry(res[mul_idx], tmp, carry);
	    प्रति (अचिह्नित सङ्कलय_idx = mul_idx+1 ; सङ्कलय_idx < शब्दाः ; सङ्कलय_idx += 1) {
		  res[सङ्कलय_idx] = सङ्कलय_with_carry(res[सङ्कलय_idx], योग, carry);
		  योग = 0;
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
      यदि (context) {
            scope->free_contexts = vvp_get_अग्र_context(context);
            प्रति (अचिह्नित idx = 0 ; idx < scope->nitem ; idx += 1) {
                  scope->item[idx]->reset_instance(context);
            }
      } अन्यथा {
            context = vvp_allocate_context(scope->nitem);
            प्रति (अचिह्नित idx = 0 ; idx < scope->nitem ; idx += 1) {
                  scope->item[idx]->alloc_instance(context);
            }
      }

      vvp_समुच्चयः_अग्र_context(context, scope->जीवितम्_contexts);
      scope->जीवितम्_contexts = context;

      प्रत्यागम context;
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

      यदि (context == scope->जीवितम्_contexts) {
            scope->जीवितम्_contexts = vvp_get_अग्र_context(context);
      } अन्यथा {
            vvp_context_t tmp = scope->जीवितम्_contexts;
            यावत् (context != vvp_get_अग्र_context(tmp)) {
                  प्रतिज्ञा(tmp);
                  tmp = vvp_get_अग्र_context(tmp);
            }
            vvp_समुच्चयः_अग्र_context(tmp, vvp_get_अग्र_context(context));
      }

      vvp_समुच्चयः_अग्र_context(context, scope->free_contexts);
      scope->free_contexts = context;
}

#यदि_परिभाषितम् CHECK_WITH_VALGRIND
शून्य contexts_delete(वर्ग __vpiScope*scope)
{
      vvp_context_t context = scope->free_contexts;

      यावत् (context) {
	    scope->free_contexts = vvp_get_अग्र_context(context);
	    प्रति (अचिह्नित idx = 0; idx < scope->nitem; idx += 1) {
		  scope->item[idx]->free_instance(context);
	    }
	    मुक्त(context);
	    context = scope->free_contexts;
      }
      मुक्त(scope->item);
}
#यदि_अन्तः

/*
 * Create a new thread with the given start address.
 */
vthread_t vthread_new(vvp_संहिता_t pc, __vpiScope*scope)
{
      vthread_t thr = नूतन संरचना vthread_s;
      thr->pc     = pc;
	//thr->bits4  = vvp_vector4_t(32);
      thr->parent = 0;
      thr->parent_scope = scope;
      thr->wait_अग्र = 0;
      thr->wt_context = 0;
      thr->rd_context = 0;

      thr->i_am_joining  = 0;
      thr->i_am_detached = 0;
      thr->i_am_प्रतीक्षा  = 0;
      thr->i_am_अन्तर्_function = 0;
      thr->is_scheduled  = 0;
      thr->i_have_ended  = 0;
      thr->i_was_disabled = 0;
      thr->delay_delete  = 0;
      thr->प्रतीक्षा_for_event = 0;
      thr->event  = 0;
      thr->ecount = 0;

      thr->flags[0] = BIT4_0;
      thr->flags[1] = BIT4_1;
      thr->flags[2] = BIT4_X;
      thr->flags[3] = BIT4_Z;
      प्रति (पूर्णाङ्क idx = 4 ; idx < 8 ; idx += 1)
	    thr->flags[idx] = BIT4_X;

      scope->तन्तवः .insert(thr);
      प्रत्यागम thr;
}

#यदि_परिभाषितम् CHECK_WITH_VALGRIND
#यदि 0
/*
 * These are not currently correct. If you use them you will get
 * double delete messages. There is still a leak related to a
 * waiting event that needs to be investigated.
 */

स्थिर शून्य wait_अग्र_delete(vthread_t आधारः)
{
      यावत् (आधारः) {
	    vthread_t tmp = आधारः->wait_अग्र;
	    विलोपय आधारः;
	    आधारः = tmp;
	    यदि (आधारः->प्रतीक्षा_for_event == 0) भङ्ग;
      }
}

स्थिर शून्य child_delete(vthread_t आधारः)
{
      यावत् (आधारः) {
	    vthread_t tmp = आधारः->child;
	    विलोपय आधारः;
	    आधारः = tmp;
      }
}
#यदि_अन्तः

शून्य vthreads_delete(वर्ग __vpiScope*scope)
{
      प्रति (std::समुच्चयः<vthread_t>::iterator cur = scope->तन्तवः.begin()
		 ; cur != scope->तन्तवः.अन्तः() ; ++ cur ) {
	    विलोपय *cur;
      }
      scope->तन्तवः.clear();
}
#यदि_अन्तः

/*
 * Reaping pulls the thread out of the stack of threads. If I have a
 * child, then hand it over to my parent or fully detach it.
 */
स्थिर शून्य vthread_reap(vthread_t thr)
{
      यदि (! thr->children.empty()) {
	    प्रति (समुच्चयः<vthread_t>::iterator cur = thr->children.begin()
		       ; cur != thr->children.अन्तः() ; ++cur) {
		  vthread_t child = *cur;
		  प्रतिज्ञा(child);
		  प्रतिज्ञा(child->parent == thr);
		  child->parent = thr->parent;
	    }
      }
      यदि (! thr->detached_children.empty()) {
	    प्रति (समुच्चयः<vthread_t>::iterator cur = thr->detached_children.begin()
		       ; cur != thr->detached_children.अन्तः() ; ++cur) {
		  vthread_t child = *cur;
		  प्रतिज्ञा(child);
		  प्रतिज्ञा(child->parent == thr);
		  प्रतिज्ञा(child->i_am_detached);
		  child->parent = 0;
		  child->i_am_detached = 0;
	    }
      }
      यदि (thr->parent) {
	      /* assert that the given element was removed. */
	    यदि (thr->i_am_detached) {
		  size_t res = thr->parent->detached_children.erase(thr);
		  प्रतिज्ञा(res == 1);
	    } अन्यथा {
		  size_t res = thr->parent->children.erase(thr);
		  प्रतिज्ञा(res == 1);
	    }
      }

      thr->parent = 0;

	// Remove myself from the containing scope if needed.
      thr->parent_scope->तन्तवः.erase(thr);

      thr->pc = codespace_null();

	/* If this thread is not scheduled, then is it safe to delete
	   it now. Otherwise, let the schedule event (which will
	   execute the thread at of_ZOMBIE) delete the object. */
      यदि ((thr->is_scheduled == 0) && (thr->प्रतीक्षा_for_event == 0)) {
	    प्रतिज्ञा(thr->children.empty());
	    प्रतिज्ञा(thr->wait_अग्र == 0);
	    यदि (thr->delay_delete)
		  कालसारणी_del_thr(thr);
	    अन्यथा
		  vthread_delete(thr);
      }
}

शून्य vthread_delete(vthread_t thr)
{
      thr->cleanup();
      विलोपय thr;
}

शून्य vthread_mark_scheduled(vthread_t thr)
{
      यावत् (thr != 0) {
	    प्रतिज्ञा(thr->is_scheduled == 0);
	    thr->is_scheduled = 1;
	    thr = thr->wait_अग्र;
      }
}

शून्य vthread_mark_final(vthread_t thr)
{
      /*
       * The behavior in a final thread is the same as in a function. Any
       * child thread will be executed immediately rather than being
       * scheduled.
       */
      यावत् (thr != 0) {
	    thr->i_am_अन्तर्_function = 1;
	    thr = thr->wait_अग्र;
      }
}

शून्य vthread_delay_delete()
{
      यदि (running_thread)
	    running_thread->delay_delete = 1;
}

/*
 * This function runs each thread by fetching an instruction,
 * incrementing the PC, and executing the instruction. The thread may
 * be the head of a list, so each thread is run so far as possible.
 */
शून्य vthread_सततम्(vthread_t thr)
{
      यावत् (thr != 0) {
	    vthread_t tmp = thr->wait_अग्र;
	    thr->wait_अग्र = 0;

	    प्रतिज्ञा(thr->is_scheduled);
	    thr->is_scheduled = 0;

            running_thread = thr;

	    प्रति (;;) {
		  vvp_संहिता_t cp = thr->pc;
		  thr->pc += 1;

		    /* Run the opcode implementation. If the execution of
		       the opcode returns false, then the thread is meant to
		       be paused, so break out of the loop. */
		  तार्किक rc = (cp->opcode)(thr, cp);
		  यदि (rc == असत्य)
			भङ्ग;
	    }

	    thr = tmp;
      }
      running_thread = 0;
}

/*
 * The CHUNK_LINK instruction is a special next pointer for linking
 * chunks of code space. It's like a simplified %jmp.
 */
तार्किक सम्बन्धि_CHUNK_LINK(vthread_t thr, vvp_संहिता_t code)
{
      प्रतिज्ञा(code->cptr);
      thr->pc = code->cptr;
      प्रत्यागम सत्य;
}

/*
 * This is called by an event functor to wake up all the threads on
 * its list. I in fact created that list in the %wait instruction, and
 * I also am certain that the waiting_for_event flag is set.
 */
शून्य vthread_कालसारणी_list(vthread_t thr)
{
      प्रति (vthread_t cur = thr ;  cur ;  cur = cur->wait_अग्र) {
	    प्रतिज्ञा(cur->प्रतीक्षा_for_event);
	    cur->प्रतीक्षा_for_event = 0;
      }

      कालसारणी_vthread(thr, 0);
}

vvp_context_t vthread_get_wt_context()
{
      यदि (running_thread)
            प्रत्यागम running_thread->wt_context;
      अन्यथा
            प्रत्यागम 0;
}

vvp_context_t vthread_get_rd_context()
{
      यदि (running_thread)
            प्रत्यागम running_thread->rd_context;
      अन्यथा
            प्रत्यागम 0;
}

vvp_context_item_t vthread_get_wt_context_item(अचिह्नित context_idx)
{
      प्रतिज्ञा(running_thread && running_thread->wt_context);
      प्रत्यागम vvp_get_context_item(running_thread->wt_context, context_idx);
}

vvp_context_item_t vthread_get_rd_context_item(अचिह्नित context_idx)
{
      प्रतिज्ञा(running_thread && running_thread->rd_context);
      प्रत्यागम vvp_get_context_item(running_thread->rd_context, context_idx);
}

/*
 * %abs/wr
 */
तार्किक सम्बन्धि_ABS_WR(vthread_t thr, vvp_संहिता_t)
{
      thr->प्रक्षिप_real( fabs(thr->निष्कासय_real()) );
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_ALLOC(vthread_t thr, vvp_संहिता_t cp)
{
        /* Allocate a context. */
      vvp_context_t child_context = vthread_alloc_context(cp->scope);

        /* Push the allocated context onto the write context stack. */
      vvp_समुच्चयः_stacked_context(child_context, thr->wt_context);
      thr->wt_context = child_context;

      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_AND(vthread_t thr, vvp_संहिता_t)
{
      vvp_vector4_t valb = thr->निष्कासय_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();
      प्रतिज्ञा(vala.size() == valb.size());
      vala &= valb;
      प्रत्यागम सत्य;
}

/*
 * This function must ALWAYS be called with the val set to the right
 * size, and initialized with BIT4_0 bits. Certain optimizations rely
 * on that.
 */
स्थिर शून्य get_तात्कालिकम्_rval(vvp_संहिता_t cp, vvp_vector4_t&val)
{
      uint32_t vala = cp->bit_idx[0];
      uint32_t valb = cp->bit_idx[1];
      अचिह्नित wid  = cp->सङ्ख्या;

      यदि (valb == 0) {
	      // Special case: if the value is zero, we are done
	      // before we start.
	    यदि (vala == 0) प्रत्यागम;

	      // Special case: The value has no X/Z bits, so we can
	      // use the setarray method to write the value all at once.
	    अचिह्नित use_wid = 8*परिमाण(अचिह्नित दीर्घ);
	    यदि (wid < use_wid)
		  use_wid = wid;
	    अचिह्नित दीर्घ tmp[1];
	    tmp[0] = vala;
	    val.setarray(0, use_wid, tmp);
	    प्रत्यागम;
      }

	// The immediate value can be values bigger then 32 bits, but
	// only if the high bits are zero. So at most we need to run
	// through the loop below 32 times. Maybe less, if the target
	// width is less. We don't have to do anything special on that
	// because vala/valb bits will shift away so (vala|valb) will
	// turn to zero at or before 32 shifts.

      प्रति (अचिह्नित idx = 0 ; idx < wid && (vala|valb) ; idx += 1) {
	    uint32_t ba = 0;
	      // Convert the vala/valb bits to a ba number that
	      // matches the encoding of the vvp_bit4_t enumeration.
	    ba = (valb & 1) << 1;
	    ba |= vala & 1;

	      // Note that the val is already pre-filled with BIT4_0
	      // bits, os we only need to set non-zero bit values.
	    यदि (ba) val.समुच्चयः_bit(idx, (vvp_bit4_t)ba);

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
तार्किक सम्बन्धि_सङ्कलय(vthread_t thr, vvp_संहिता_t)
{
      vvp_vector4_t r = thr->निष्कासय_vec4();
	// Rather then pop l, use it directly from the stack. When we
	// assign to 'l', that will edit the top of the stack, which
	// replaces a pop and a pull.
      vvp_vector4_t&l = thr->peek_vec4();

      l.सङ्कलय(r);

      प्रत्यागम सत्य;
}

/*
 * %addi <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
तार्किक सम्बन्धि_ADDI(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित wid = cp->सङ्ख्या;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_तात्कालिकम्_rval (cp, r);

      l.सङ्कलय(r);

      प्रत्यागम सत्य;
}

/*
 * %add/wr
 */
तार्किक सम्बन्धि_सङ्कलय_WR(vthread_t thr, vvp_संहिता_t)
{
      द्विगुण r = thr->निष्कासय_real();
      द्विगुण l = thr->निष्कासय_real();
      thr->प्रक्षिप_real(l + r);
      प्रत्यागम सत्य;
}

/* %assign/ar <array>, <delay>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory. <delay>
 * is the delay in simulation time. <bit> is the index register
 * containing the real value.
 */
तार्किक सम्बन्धि_नियोजय_AR(vthread_t thr, vvp_संहिता_t cp)
{
      दीर्घ adr = thr->शब्दाः[3].w_int;
      अचिह्नित delay = cp->bit_idx[0];
      द्विगुण value = thr->निष्कासय_real();

      यदि (adr >= 0) {
	    कालसारणी_नियोजय_array_word(cp->array, adr, value, delay);
      }

      प्रत्यागम सत्य;
}

/* %assign/ar/d <array>, <delay_idx>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory.
 * <delay_idx> is the integer register that contains the delay value.
 */
तार्किक सम्बन्धि_नियोजय_ARD(vthread_t thr, vvp_संहिता_t cp)
{
      दीर्घ adr = thr->शब्दाः[3].w_int;
      द्विगुण value = thr->निष्कासय_real();

      यदि (adr >= 0) {
	    vvp_time64_t delay = thr->शब्दाः[cp->bit_idx[0]].w_uint;
	    कालसारणी_नियोजय_array_word(cp->array, adr, value, delay);
      }

      प्रत्यागम सत्य;
}

/* %assign/ar/e <array>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory. <bit>
 * is the index register containing the real value. The event
 * information is contained in the thread event control registers
 * and is set with %evctl.
 */
तार्किक सम्बन्धि_नियोजय_ARE(vthread_t thr, vvp_संहिता_t cp)
{
      दीर्घ adr = thr->शब्दाः[3].w_int;
      द्विगुण value = thr->निष्कासय_real();

      यदि (adr >= 0) {
	    यदि (thr->ecount == 0) {
		  कालसारणी_नियोजय_array_word(cp->array, adr, value, 0);
	    } अन्यथा {
		  कालसारणी_evctl(cp->array, adr, value, thr->event,
		                 thr->ecount);
	    }
      }

      प्रत्यागम सत्य;
}

/*
 * %assign/vec4 <var>, <delay>
 */
तार्किक सम्बन्धि_नियोजय_VEC4(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      अचिह्नित delay = cp->bit_idx[0];
      ध्रुव vvp_vector4_t&val = thr->peek_vec4();

      कालसारणी_नियोजय_सदिशम्(ptr, 0, 0, val, delay);
      thr->निष्कासय_vec4(1);
      प्रत्यागम सत्य;
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
			    अचिह्नित पूर्णाङ्क sig_wid)
{
      अचिह्नित पूर्णाङ्क wid = val.size();

        // Fully in bounds, most likely case
      यदि (off >= 0 && (uint64_t)off + wid <= sig_wid)
	    प्रत्यागम सत्य;

      अचिह्नित पूर्णाङ्क आधारः = 0;
      यदि (off >= 0) {
	      // Fully out-of-bounds
	    यदि ((uint64_t)off >= sig_wid)
		  प्रत्यागम असत्य;
      } अन्यथा {
	      // Fully out-of-bounds */
	    यदि ((uint64_t)(-off) >= wid)
		  प्रत्यागम असत्य;

	      // If the index is below the vector, then only assign the high
	      // bits that overlap with the target
	    आधारः = -off;
	    wid += off;
		off = 0;
      }

	// If the value is partly above the target, then only assign
	// the bits that overlap
      यदि ((uint64_t)off + wid > sig_wid)
	    wid = sig_wid - (uint64_t)off;

      val = val.subvalue(आधारः, wid);

      प्रत्यागम सत्य;
}

/*
 * %assign/vec4/a/d <arr>, <offx>, <delx>
 */
तार्किक सम्बन्धि_नियोजय_VEC4_A_D(vthread_t thr, vvp_संहिता_t cp)
{
      पूर्णाङ्क off_idx = cp->bit_idx[0];
      पूर्णाङ्क del_idx = cp->bit_idx[1];
      पूर्णाङ्क adr_idx = 3;

      int64_t  off = off_idx ? thr->शब्दाः[off_idx].w_int : 0;
      vvp_time64_t del = del_idx? thr->शब्दाः[del_idx].w_uint : 0;
      दीर्घ     adr = thr->शब्दाः[adr_idx].w_int;

      vvp_vector4_t val = thr->निष्कासय_vec4();

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      यदि (thr->flags[4] == BIT4_1)
	    प्रत्यागम सत्य;

      यदि (!resize_rval_vec(val, off, cp->array->get_word_आकार()))
	    प्रत्यागम सत्य;

      कालसारणी_नियोजय_array_word(cp->array, adr, off, val, del);

      प्रत्यागम सत्य;
}

/*
 * %assign/vec4/a/e <arr>, <offx>
 */
तार्किक सम्बन्धि_नियोजय_VEC4_A_E(vthread_t thr, vvp_संहिता_t cp)
{
      पूर्णाङ्क off_idx = cp->bit_idx[0];
      पूर्णाङ्क adr_idx = 3;

      int64_t  off = off_idx ? thr->शब्दाः[off_idx].w_int : 0;
      दीर्घ     adr = thr->शब्दाः[adr_idx].w_int;

      vvp_vector4_t val = thr->निष्कासय_vec4();

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      यदि (thr->flags[4] == BIT4_1)
	    प्रत्यागम सत्य;

      यदि (!resize_rval_vec(val, off, cp->array->get_word_आकार()))
	    प्रत्यागम सत्य;

      यदि (thr->ecount == 0) {
	    कालसारणी_नियोजय_array_word(cp->array, adr, off, val, 0);
      } अन्यथा {
	    कालसारणी_evctl(cp->array, adr, val, off, thr->event, thr->ecount);
      }

      प्रत्यागम सत्य;
}

/*
 * %assign/vec4/off/d <var>, <off>, <del>
 */
तार्किक सम्बन्धि_नियोजय_VEC4_OFF_D(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      अचिह्नित off_सूचकांक = cp->bit_idx[0];
      अचिह्नित del_सूचकांक = cp->bit_idx[1];
      vvp_vector4_t val = thr->निष्कासय_vec4();

      int64_t off = thr->शब्दाः[off_सूचकांक].w_int;
      vvp_time64_t del = thr->शब्दाः[del_सूचकांक].w_uint;

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      यदि (thr->flags[4] == BIT4_1)
	    प्रत्यागम सत्य;

      vvp_signal_मानम्*sig = dynamic_cast<vvp_signal_मानम्*> (cp->net->fil);
      प्रतिज्ञा(sig);

      यदि (!resize_rval_vec(val, off, sig->मानम्_आकार()))
	    प्रत्यागम सत्य;

      कालसारणी_नियोजय_सदिशम्(ptr, off, sig->मानम्_आकार(), val, del);
      प्रत्यागम सत्य;
}

/*
 * %assign/vec4/off/e <var>, <off>
 */
तार्किक सम्बन्धि_नियोजय_VEC4_OFF_E(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      अचिह्नित off_सूचकांक = cp->bit_idx[0];
      vvp_vector4_t val = thr->निष्कासय_vec4();

      int64_t off = thr->शब्दाः[off_सूचकांक].w_int;

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      यदि (thr->flags[4] == BIT4_1)
	    प्रत्यागम सत्य;

      vvp_signal_मानम्*sig = dynamic_cast<vvp_signal_मानम्*> (cp->net->fil);
      प्रतिज्ञा(sig);

      यदि (!resize_rval_vec(val, off, sig->मानम्_आकार()))
	    प्रत्यागम सत्य;

      यदि (thr->ecount == 0) {
	    कालसारणी_नियोजय_सदिशम्(ptr, off, sig->मानम्_आकार(), val, 0);
      } अन्यथा {
	    कालसारणी_evctl(ptr, val, off, sig->मानम्_आकार(), thr->event, thr->ecount);
      }

      प्रत्यागम सत्य;
}

/*
 * %assign/vec4/d <var-label> <delay>
 */
तार्किक सम्बन्धि_नियोजय_VEC4_D(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      अचिह्नित del_सूचकांक = cp->bit_idx[0];
      vvp_time64_t del = thr->शब्दाः[del_सूचकांक].w_int;

      vvp_vector4_t value = thr->निष्कासय_vec4();

      vvp_signal_मानम्*sig = dynamic_cast<vvp_signal_मानम्*> (cp->net->fil);
      प्रतिज्ञा(sig);

      कालसारणी_नियोजय_सदिशम्(ptr, 0, sig->मानम्_आकार(), value, del);

      प्रत्यागम सत्य;
}

/*
 * %assign/vec4/e <var-label>
 */
तार्किक सम्बन्धि_नियोजय_VEC4_E(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_vector4_t value = thr->निष्कासय_vec4();

      vvp_signal_मानम्*sig = dynamic_cast<vvp_signal_मानम्*> (cp->net->fil);
      प्रतिज्ञा(sig);

      यदि (thr->ecount == 0) {
	    कालसारणी_नियोजय_सदिशम्(ptr, 0, sig->मानम्_आकार(), value, 0);
      } अन्यथा {
	    कालसारणी_evctl(ptr, value, 0, sig->मानम्_आकार(), thr->event, thr->ecount);
      }

      प्रत्यागम सत्य;
}

/*
 * This is %assign/wr <vpi-label>, <delay>
 *
 * This assigns (after a delay) a value to a real variable. Use the
 * vpi_put_value function to do the assign, with the delay written
 * into the vpiInertialDelay carrying the desired delay.
 */
तार्किक सम्बन्धि_नियोजय_WR(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित delay = cp->bit_idx[0];
      द्विगुण value = thr->निष्कासय_real();
      s_vpi_time del;

      del.type = vpiSimTime;
      vpip_time_गन्तव्यम्_timestruct(&del, delay);

      __vpiHandle*tmp = cp->handle;

      t_vpi_मानम् val;
      val.format = vpiRealVal;
      val.value.real = value;
      vpi_put_मानम्(tmp, &val, &del, vpiTransportDelay);

      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_नियोजय_WRD(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_time64_t delay = thr->शब्दाः[cp->bit_idx[0]].w_uint;
      द्विगुण value = thr->निष्कासय_real();
      s_vpi_time del;

      del.type = vpiSimTime;
      vpip_time_गन्तव्यम्_timestruct(&del, delay);

      __vpiHandle*tmp = cp->handle;

      t_vpi_मानम् val;
      val.format = vpiRealVal;
      val.value.real = value;
      vpi_put_मानम्(tmp, &val, &del, vpiTransportDelay);

      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_नियोजय_WRE(vthread_t thr, vvp_संहिता_t cp)
{
      प्रतिज्ञा(thr->event != 0);
      द्विगुण value = thr->निष्कासय_real();
      __vpiHandle*tmp = cp->handle;

	// If the count is zero then just put the value.
      यदि (thr->ecount == 0) {
	    t_vpi_मानम् val;

	    val.format = vpiRealVal;
	    val.value.real = value;
	    vpi_put_मानम्(tmp, &val, 0, vpiNoDelay);
      } अन्यथा {
	    कालसारणी_evctl(tmp, value, thr->event, thr->ecount);
      }

      thr->event = 0;
      thr->ecount = 0;

      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_BLEND(vthread_t thr, vvp_संहिता_t)
{
      vvp_vector4_t vala = thr->निष्कासय_vec4();
      vvp_vector4_t valb = thr->निष्कासय_vec4();
      प्रतिज्ञा(vala.size() == valb.size());

      प्रति (अचिह्नित idx = 0 ; idx < vala.size() ; idx += 1) {
	    यदि (vala.value(idx) == valb.value(idx))
		  अनुवर्त;

	    vala.समुच्चयः_bit(idx, BIT4_X);
      }

      thr->प्रक्षिप_vec4(vala);
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_BLEND_STR(vthread_t thr, vvp_संहिता_t)
{
      string f = thr->निष्कासय_str();
      string t = thr->निष्कासय_str();
      thr->प्रक्षिप_str((t == f) ? t : "");
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_BLEND_WR(vthread_t thr, vvp_संहिता_t)
{
      द्विगुण f = thr->निष्कासय_real();
      द्विगुण t = thr->निष्कासय_real();
      thr->प्रक्षिप_real((t == f) ? t : 0.0);
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_BREAKPOINT(vthread_t, vvp_संहिता_t)
{
      प्रत्यागम सत्य;
}

/*
 * %callf/void <code-label>, <scope-label>
 * Combine the %fork and %join steps for invoking a function.
 */
स्थिर तार्किक do_callf_void(vthread_t thr, vthread_t child)
{

      यदि (child->parent_scope->is_automatic()) {
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
      प्रतिज्ञा(child->parent_scope->get_type_संहिता() == vpiFunction);
      child->is_scheduled = 1;
      child->i_am_अन्तर्_function = 1;
      vthread_सततम्(child);
      running_thread = thr;

      यदि (child->i_have_ended) {
	    do_join(thr, child);
	    प्रत्यागम सत्य;
      } अन्यथा {
	    thr->i_am_joining = 1;
	    प्रत्यागम असत्य;
      }
}

तार्किक सम्बन्धि_CALLF_OBJ(vthread_t thr, vvp_संहिता_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);
      प्रत्यागम do_callf_void(thr, child);

      // XXXX NOT IMPLEMENTED
}

तार्किक सम्बन्धि_CALLF_REAL(vthread_t thr, vvp_संहिता_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);

	// This is the return value. Push a place-holder value. The function
	// will replace this with the actual value using a %ret/real instruction.
      thr->प्रक्षिप_real(0.0);
      child->तर्काः_real.push_back(0);

      प्रत्यागम do_callf_void(thr, child);
}

तार्किक सम्बन्धि_CALLF_STR(vthread_t thr, vvp_संहिता_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);

      thr->प्रक्षिप_str("");
      child->तर्काः_str.push_back(0);

      प्रत्यागम do_callf_void(thr, child);
}

तार्किक सम्बन्धि_CALLF_VEC4(vthread_t thr, vvp_संहिता_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);

      vpiScopeFunction*scope_func = dynamic_cast<vpiScopeFunction*>(cp->scope);
      प्रतिज्ञा(scope_func);

	// This is the return value. Push a place-holder value. The function
	// will replace this with the actual value using a %ret/real instruction.
      thr->प्रक्षिप_vec4(vvp_vector4_t(scope_func->get_func_विस्तारः(), scope_func->get_func_init_val()));
      child->तर्काः_vec4.push_back(0);

      प्रत्यागम do_callf_void(thr, child);
}

तार्किक सम्बन्धि_CALLF_VOID(vthread_t thr, vvp_संहिता_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);
      प्रत्यागम do_callf_void(thr, child);
}

/*
 * The %cassign/link instruction connects a source node to a
 * destination node. The destination node must be a signal, as it is
 * marked with the source of the cassign so that it may later be
 * unlinked without specifically knowing the source that this
 * instruction used.
 */
तार्किक सम्बन्धि_CASSIGN_LINK(vthread_t, vvp_संहिता_t cp)
{
      vvp_net_t*dst = cp->net;
      vvp_net_t*src = cp->net2;

      vvp_fun_signal_आधारः*sig
	    = dynamic_cast<vvp_fun_signal_आधारः*>(dst->fun);
      प्रतिज्ञा(sig);

	/* Any previous continuous assign should have been removed already. */
      प्रतिज्ञा(sig->cassign_link == 0);

      sig->cassign_link = src;

	/* Link the output of the src to the port[1] (the cassign
	   port) of the destination. */
      vvp_net_ptr_t dst_ptr (dst, 1);
      src->link(dst_ptr);

      प्रत्यागम सत्य;
}

/*
 * If there is an existing continuous assign linked to the destination
 * node, unlink it. This must be done before applying a new continuous
 * assign, otherwise the initial assigned value will be propagated to
 * any other nodes driven by the old continuous assign source.
 */
स्थिर शून्य cassign_unlink(vvp_net_t*dst)
{
      vvp_fun_signal_आधारः*sig
	    = dynamic_cast<vvp_fun_signal_आधारः*>(dst->fun);
      प्रतिज्ञा(sig);

      यदि (sig->cassign_link == 0)
	    प्रत्यागम;

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
तार्किक सम्बन्धि_CASSIGN_VEC4(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_net_t*net = cp->net;
      vvp_vector4_t value = thr->निष्कासय_vec4();

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

	/* Set the value into port 1 of the destination. */
      vvp_net_ptr_t ptr (net, 1);
      vvp_send_vec4(ptr, value, 0);

      प्रत्यागम सत्य;
}

/*
 * %cassign/vec4/off <var>, <off>
 */
तार्किक सम्बन्धि_CASSIGN_VEC4_OFF(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_net_t*net = cp->net;
      अचिह्नित आधारः_idx = cp->bit_idx[0];
      दीर्घ आधारः = thr->शब्दाः[आधारः_idx].w_int;
      vvp_vector4_t value = thr->निष्कासय_vec4();
      अचिह्नित wid = value.size();

      यदि (thr->flags[4] == BIT4_1)
	    प्रत्यागम सत्य;

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

      vvp_signal_मानम्*sig = dynamic_cast<vvp_signal_मानम्*> (net->fil);
      प्रतिज्ञा(sig);

      यदि (आधारः < 0 && (wid <= (अचिह्नित)-आधारः))
	    प्रत्यागम सत्य;

      यदि (आधारः >= (दीर्घ)sig->मानम्_आकार())
	    प्रत्यागम सत्य;

      यदि (आधारः < 0) {
	    wid -= (अचिह्नित) -आधारः;
	    आधारः = 0;
	    value.resize(wid);
      }

      यदि (आधारः+wid > sig->मानम्_आकार()) {
	    wid = sig->मानम्_आकार() - आधारः;
	    value.resize(wid);
      }

      vvp_net_ptr_t ptr (net, 1);
      vvp_send_vec4_pv(ptr, value, आधारः, sig->मानम्_आकार(), 0);
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_CASSIGN_WR(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_net_t*net  = cp->net;
      द्विगुण value = thr->निष्कासय_real();

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

	/* Set the value into port 1 of the destination. */
      vvp_net_ptr_t ptr (net, 1);
      vvp_send_real(ptr, value, 0);

      प्रत्यागम सत्य;
}

/*
 * %cast2
 */
तार्किक सम्बन्धि_CAST2(vthread_t thr, vvp_संहिता_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      अचिह्नित wid = val.size();

      प्रति (अचिह्नित idx = 0 ; idx < wid ; idx += 1) {
	    विकल्प (val.value(idx)) {
		पक्ष BIT4_0:
		पक्ष BIT4_1:
		  भङ्ग;
		स्वभाव:
		  val.समुच्चयः_bit(idx, BIT4_0);
		  भङ्ग;
	    }
      }

      प्रत्यागम सत्य;
}

तार्किक do_cast_vec_dar(vthread_t thr, vvp_संहिता_t cp, तार्किक as_vec4)
{
      अचिह्नित wid = cp->सङ्ख्या;

      vvp_object_t obj;
      thr->निष्कासय_object(obj);

      vvp_darray*darray = obj.peek<vvp_darray>();
      प्रतिज्ञा(darray);

      vvp_vector4_t vec = darray->get_bitstream(as_vec4);
      यदि (vec.size() != wid) {
	    cerr << thr->get_fileline()
	         << "VVP error: size mismatch when casting dynamic array to vector." << पङ्क्त्यन्तः;
            thr->प्रक्षिप_vec4(vvp_vector4_t(wid));
            कालसारणी_stop(0);
            प्रत्यागम असत्य;
      }
      thr->प्रक्षिप_vec4(vec);
      प्रत्यागम सत्य;
}

/*
 * %cast/vec2/dar <wid>
 */
तार्किक सम्बन्धि_CAST_VEC2_DAR(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम do_cast_vec_dar(thr, cp, असत्य);
}

/*
 * %cast/vec4/dar <wid>
 */
तार्किक सम्बन्धि_CAST_VEC4_DAR(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम do_cast_vec_dar(thr, cp, सत्य);
}

/*
 * %cast/vec4/str <wid>
 */
तार्किक सम्बन्धि_CAST_VEC4_STR(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित wid = cp->सङ्ख्या;
      string str = thr->निष्कासय_str();

      vvp_vector4_t vec(wid, BIT4_0);

      यदि (wid != 8*str.length()) {
	    cerr << thr->get_fileline()
	         << "VVP error: size mismatch when casting string to vector." << पङ्क्त्यन्तः;
            thr->प्रक्षिप_vec4(vec);
            कालसारणी_stop(0);
            प्रत्यागम असत्य;
      }

      अचिह्नित sdx = 0;
      अचिह्नित vdx = wid;
      यावत् (vdx > 0) {
            वर्ण ch = str[sdx++];
            vdx -= 8;
            प्रति (अचिह्नित bdx = 0; bdx < 8; bdx += 1) {
                  यदि (ch & 1)
                        vec.समुच्चयः_bit(vdx+bdx, BIT4_1);
                  ch >>= 1;
            }
      }

      thr->प्रक्षिप_vec4(vec);
      प्रत्यागम सत्य;
}

स्थिर शून्य do_CMPE(vthread_t thr, ध्रुव vvp_vector4_t&lval, ध्रुव vvp_vector4_t&rval)
{
      प्रतिज्ञा(rval.size() == lval.size());

      यदि (lval.has_xz() || rval.has_xz()) {

	    अचिह्नित wid = lval.size();
	    vvp_bit4_t eq  = BIT4_1;
	    vvp_bit4_t eeq = BIT4_1;

	    प्रति (अचिह्नित idx = 0 ; idx < wid ; idx += 1) {
		  vvp_bit4_t lv = lval.value(idx);
		  vvp_bit4_t rv = rval.value(idx);

		  यदि (lv != rv)
			eeq = BIT4_0;

		  यदि (eq==BIT4_1 && (bit4_is_xz(lv) || bit4_is_xz(rv)))
			eq = BIT4_X;
		  यदि ((lv == BIT4_0) && (rv==BIT4_1))
			eq = BIT4_0;
		  यदि ((lv == BIT4_1) && (rv==BIT4_0))
			eq = BIT4_0;

		  यदि (eq == BIT4_0)
			भङ्ग;
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
तार्किक सम्बन्धि_CMPE(vthread_t thr, vvp_संहिता_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ध्रुव vvp_vector4_t&rval = thr->peek_vec4(0);
      ध्रुव vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPE(thr, lval, rval);

      thr->निष्कासय_vec4(2);
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_CMPNE(vthread_t thr, vvp_संहिता_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ध्रुव vvp_vector4_t&rval = thr->peek_vec4(0);
      ध्रुव vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];
      thr->flags[6] =  ~thr->flags[6];

      thr->निष्कासय_vec4(2);
      प्रत्यागम सत्य;
}

/*
 * %cmpi/e <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
तार्किक सम्बन्धि_CMPIE(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित wid = cp->सङ्ख्या;

      ध्रुव vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_तात्कालिकम्_rval (cp, rval);

      do_CMPE(thr, lval, rval);

      thr->निष्कासय_vec4(1);
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_CMPINE(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित wid = cp->सङ्ख्या;

      ध्रुव vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_तात्कालिकम्_rval (cp, rval);

      do_CMPE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];
      thr->flags[6] =  ~thr->flags[6];

      thr->निष्कासय_vec4(1);
      प्रत्यागम सत्य;
}



स्थिर शून्य do_CMPS(vthread_t thr, ध्रुव vvp_vector4_t&lval, ध्रुव vvp_vector4_t&rval)
{
      प्रतिज्ञा(rval.size() == lval.size());

	// If either value has XZ bits, then the eq and lt values are
	// known already to be X. Just calculate the eeq result as a
	// special case and short circuit the rest of the compare.
      यदि (lval.has_xz() || rval.has_xz()) {
	    thr->flags[4] = BIT4_X; // eq
	    thr->flags[5] = BIT4_X; // lt
	    thr->flags[6] = lval.eeq(rval)? BIT4_1 : BIT4_0;
	    प्रत्यागम;
      }

	// Past this point, we know we are dealing only with fully
	// defined values.
      अचिह्नित wid = lval.size();

      ध्रुव vvp_bit4_t sig1 = lval.value(wid-1);
      ध्रुव vvp_bit4_t sig2 = rval.value(wid-1);

	// If the lval is <0 and the rval is >=0, then we know the result.
      यदि ((sig1 == BIT4_1) && (sig2 == BIT4_0)) {
	    thr->flags[4] = BIT4_0; // eq;
	    thr->flags[5] = BIT4_1; // lt;
	    thr->flags[6] = BIT4_0; // eeq
	    प्रत्यागम;
      }

	// If the lval is >=0 and the rval is <0, then we know the result.
      यदि ((sig1 == BIT4_0) && (sig2 == BIT4_1)) {
	    thr->flags[4] = BIT4_0; // eq;
	    thr->flags[5] = BIT4_0; // lt;
	    thr->flags[6] = BIT4_0; // eeq
	    प्रत्यागम;
      }

	// The values have the same sign, so we have to look at the
	// actual value. Scan from the MSB down. As soon as we find a
	// bit that differs, we know the result.

      प्रति (अचिह्नित idx = 1 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t lv = lval.value(wid-1-idx);
	    vvp_bit4_t rv = rval.value(wid-1-idx);

	    यदि (lv == rv)
		  अनुवर्त;

	    thr->flags[4] = BIT4_0; // eq
	    thr->flags[6] = BIT4_0; // eeq

	    यदि (lv==BIT4_0) {
		  thr->flags[5] = BIT4_1; // lt
	    } अन्यथा {
		  thr->flags[5] = BIT4_0; // lt
	    }
	    प्रत्यागम;
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
तार्किक सम्बन्धि_CMPS(vthread_t thr, vvp_संहिता_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ध्रुव vvp_vector4_t&rval = thr->peek_vec4(0);
      ध्रुव vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPS(thr, lval, rval);

      thr->निष्कासय_vec4(2);
      प्रत्यागम सत्य;
}

/*
 * %cmpi/s <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
तार्किक सम्बन्धि_CMPIS(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित wid = cp->सङ्ख्या;

      ध्रुव vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_तात्कालिकम्_rval (cp, rval);

      do_CMPS(thr, lval, rval);

      thr->निष्कासय_vec4(1);
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_CMPSTR(vthread_t thr, vvp_संहिता_t)
{
      string re = thr->निष्कासय_str();
      string le = thr->निष्कासय_str();

      पूर्णाङ्क rc = वाक्यं_तुलय(le.c_str(), re.c_str());

      vvp_bit4_t eq;
      vvp_bit4_t lt;

      यदि (rc == 0) {
	    eq = BIT4_1;
	    lt = BIT4_0;
      } अन्यथा यदि (rc < 0) {
	    eq = BIT4_0;
	    lt = BIT4_1;
      } अन्यथा {
	    eq = BIT4_0;
	    lt = BIT4_0;
      }

      thr->flags[4] = eq;
      thr->flags[5] = lt;

      प्रत्यागम सत्य;
}

स्थिर शून्य सम्बन्धि_CMPU_the_hard_way(vthread_t thr, अचिह्नित wid,
				 ध्रुव vvp_vector4_t&lval,
				 ध्रुव vvp_vector4_t&rval)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_bit4_t eeq = BIT4_1;

      प्रति (अचिह्नित idx = 0 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t lv = lval.value(idx);
	    vvp_bit4_t rv = rval.value(idx);

	    यदि (lv != rv)
		  eeq = BIT4_0;

	    यदि (eq==BIT4_1 && (bit4_is_xz(lv) || bit4_is_xz(rv)))
		  eq = BIT4_X;
	    यदि ((lv == BIT4_0) && (rv==BIT4_1))
		  eq = BIT4_0;
	    यदि ((lv == BIT4_1) && (rv==BIT4_0))
		  eq = BIT4_0;

	    यदि (eq == BIT4_0)
		  भङ्ग;

      }

      thr->flags[4] = eq;
      thr->flags[5] = BIT4_X;
      thr->flags[6] = eeq;
}

स्थिर शून्य do_CMPU(vthread_t thr, ध्रुव vvp_vector4_t&lval, ध्रुव vvp_vector4_t&rval)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_bit4_t lt = BIT4_0;

      यदि (rval.size() != lval.size()) {
	    cerr << thr->get_fileline()
	         << "VVP ERROR: %cmp/u operand width mismatch: lval=" << lval
		 << ", rval=" << rval << पङ्क्त्यन्तः;
      }
      प्रतिज्ञा(rval.size() == lval.size());
      अचिह्नित wid = lval.size();

      अचिह्नित दीर्घ*larray = lval.subarray(0,wid);
      यदि (larray == 0) प्रत्यागम सम्बन्धि_CMPU_the_hard_way(thr, wid, lval, rval);

      अचिह्नित दीर्घ*rarray = rval.subarray(0,wid);
      यदि (rarray == 0) {
	    विलोपय[]larray;
	    प्रत्यागम सम्बन्धि_CMPU_the_hard_way(thr, wid, lval, rval);
      }

      अचिह्नित शब्दाः = (wid+CPU_WORD_द्वयङ्काः-1) / CPU_WORD_द्वयङ्काः;

      प्रति (अचिह्नित wdx = 0 ; wdx < शब्दाः ; wdx += 1) {
	    यदि (larray[wdx] == rarray[wdx])
		  अनुवर्त;

	    eq = BIT4_0;
	    यदि (larray[wdx] < rarray[wdx])
		  lt = BIT4_1;
	    अन्यथा
		  lt = BIT4_0;
      }

      विलोपय[]larray;
      विलोपय[]rarray;

      thr->flags[4] = eq;
      thr->flags[5] = lt;
      thr->flags[6] = eq;
}

तार्किक सम्बन्धि_CMPU(vthread_t thr, vvp_संहिता_t)
{

      ध्रुव vvp_vector4_t&rval = thr->peek_vec4(0);
      ध्रुव vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPU(thr, lval, rval);

      thr->निष्कासय_vec4(2);
      प्रत्यागम सत्य;
}

/*
 * %cmpi/u <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
तार्किक सम्बन्धि_CMPIU(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित wid = cp->सङ्ख्या;

      ध्रुव vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_तात्कालिकम्_rval (cp, rval);

      do_CMPU(thr, lval, rval);

      thr->निष्कासय_vec4(1);
      प्रत्यागम सत्य;
}


/*
 * %cmp/x
 */
तार्किक सम्बन्धि_CMPX(vthread_t thr, vvp_संहिता_t)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_vector4_t rval = thr->निष्कासय_vec4();
      vvp_vector4_t lval = thr->निष्कासय_vec4();

      प्रतिज्ञा(rval.size() == lval.size());
      अचिह्नित wid = lval.size();

      प्रति (अचिह्नित idx = 0 ; idx < wid ; idx += 1) {
	    vvp_bit4_t lv = lval.value(idx);
	    vvp_bit4_t rv = rval.value(idx);
	    यदि ((lv != rv) && !bit4_is_xz(lv) && !bit4_is_xz(rv)) {
		  eq = BIT4_0;
		  भङ्ग;
	    }
      }

      thr->flags[4] = eq;
      प्रत्यागम सत्य;
}

स्थिर शून्य do_CMPWE(vthread_t thr, ध्रुव vvp_vector4_t&lval, ध्रुव vvp_vector4_t&rval)
{
      प्रतिज्ञा(rval.size() == lval.size());

      यदि (lval.has_xz() || rval.has_xz()) {

	    अचिह्नित wid = lval.size();
	    vvp_bit4_t eq  = BIT4_1;

	    प्रति (अचिह्नित idx = 0 ; idx < wid ; idx += 1) {
		  vvp_bit4_t lv = lval.value(idx);
		  vvp_bit4_t rv = rval.value(idx);

		  यदि (bit4_is_xz(rv))
			अनुवर्त;
		  यदि ((eq == BIT4_1) && bit4_is_xz(lv))
			eq = BIT4_X;
		  यदि ((lv == BIT4_0) && (rv==BIT4_1))
			eq = BIT4_0;
		  यदि ((lv == BIT4_1) && (rv==BIT4_0))
			eq = BIT4_0;

		  यदि (eq == BIT4_0)
			भङ्ग;
	    }

	    thr->flags[4] = eq;

      } अन्यथा {
	      // If there are no XZ bits anywhere, then the results of
	      // ==? match the === test.
	    thr->flags[4] = (lval.eeq(rval)? BIT4_1 : BIT4_0);
      }
}

तार्किक सम्बन्धि_CMPWE(vthread_t thr, vvp_संहिता_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ध्रुव vvp_vector4_t&rval = thr->peek_vec4(0);
      ध्रुव vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPWE(thr, lval, rval);

      thr->निष्कासय_vec4(2);
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_CMPWNE(vthread_t thr, vvp_संहिता_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ध्रुव vvp_vector4_t&rval = thr->peek_vec4(0);
      ध्रुव vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPWE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];

      thr->निष्कासय_vec4(2);
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_CMPWR(vthread_t thr, vvp_संहिता_t)
{
      द्विगुण r = thr->निष्कासय_real();
      द्विगुण l = thr->निष्कासय_real();

      vvp_bit4_t eq = (l == r)? BIT4_1 : BIT4_0;
      vvp_bit4_t lt = (l <  r)? BIT4_1 : BIT4_0;

      thr->flags[4] = eq;
      thr->flags[5] = lt;

      प्रत्यागम सत्य;
}

/*
 * %cmp/z
 */
तार्किक सम्बन्धि_CMPZ(vthread_t thr, vvp_संहिता_t)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_vector4_t rval = thr->निष्कासय_vec4();
      vvp_vector4_t lval = thr->निष्कासय_vec4();

      प्रतिज्ञा(rval.size() == lval.size());
      अचिह्नित wid = lval.size();

      प्रति (अचिह्नित idx = 0 ; idx < wid ; idx += 1) {
	    vvp_bit4_t lv = lval.value(idx);
	    vvp_bit4_t rv = rval.value(idx);
	    यदि ((lv != rv) && (rv != BIT4_Z) && (lv != BIT4_Z)) {
		  eq = BIT4_0;
		  भङ्ग;
	    }
      }

      thr->flags[4] = eq;
      प्रत्यागम सत्य;
}

/*
 *  %concat/str;
 */
तार्किक सम्बन्धि_CONCAT_STR(vthread_t thr, vvp_संहिता_t)
{
      string text = thr->निष्कासय_str();
      thr->peek_str(0).append(text);
      प्रत्यागम सत्य;
}

/*
 *  %concati/str <string>;
 */
तार्किक सम्बन्धि_CONCATI_STR(vthread_t thr, vvp_संहिता_t cp)
{
      ध्रुव वर्ण*text = cp->text;
      thr->peek_str(0).append(filter_string(text));
      प्रत्यागम सत्य;
}

/*
 * %concat/vec4
 */
तार्किक सम्बन्धि_CONCAT_VEC4(vthread_t thr, vvp_संहिता_t)
{
      ध्रुव vvp_vector4_t&lsb = thr->peek_vec4(0);
      ध्रुव vvp_vector4_t&msb = thr->peek_vec4(1);

	// The result is the size of the top two vectors in the stack.
      vvp_vector4_t res (msb.size() + lsb.size(), BIT4_X);

	// Build up the result.
      res.समुच्चयः_vec(0, lsb);
      res.समुच्चयः_vec(lsb.size(), msb);

	// Rearrange the stack to pop the inputs and push the
	// result. Do that by actually popping only 1 stack position
	// and replacing the new top with the new value.
      thr->निष्कासय_vec4(1);
      thr->peek_vec4() = res;

      प्रत्यागम सत्य;
}

/*
 * %concati/vec4 <vala>, <valb>, <wid>
 *
 * Concat the immediate value to the LOW bits of the concatenation.
 * Get the HIGH bits from the top of the vec4 stack.
 */
तार्किक सम्बन्धि_CONCATI_VEC4(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित wid  = cp->सङ्ख्या;

      vvp_vector4_t&msb = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t lsb (wid, BIT4_0);
      get_तात्कालिकम्_rval (cp, lsb);

      vvp_vector4_t res (msb.size()+lsb.size(), BIT4_X);
      res.समुच्चयः_vec(0, lsb);
      res.समुच्चयः_vec(lsb.size(), msb);

      msb = res;
      प्रत्यागम सत्य;
}

/*
 * %cvt/rv
 */
तार्किक सम्बन्धि_CVT_RV(vthread_t thr, vvp_संहिता_t)
{
      द्विगुण val;
      vvp_vector4_t val4 = thr->निष्कासय_vec4();
      vector4_गन्तव्यम्_मानम्(val4, val, असत्य);
      thr->प्रक्षिप_real(val);
      प्रत्यागम सत्य;
}

/*
 * %cvt/rv/s
 */
तार्किक सम्बन्धि_CVT_RV_S(vthread_t thr, vvp_संहिता_t)
{
      द्विगुण val;
      vvp_vector4_t val4 = thr->निष्कासय_vec4();
      vector4_गन्तव्यम्_मानम्(val4, val, सत्य);
      thr->प्रक्षिप_real(val);
      प्रत्यागम सत्य;
}

/*
 * %cvt/sr <idx>
 * Pop the top value from the real stack, convert it to a 64bit signed
 * and save it to the indexed register.
 */
तार्किक सम्बन्धि_CVT_SR(vthread_t thr, vvp_संहिता_t cp)
{
      द्विगुण r = thr->निष्कासय_real();
      thr->शब्दाः[cp->bit_idx[0]].w_int = i64round(r);

      प्रत्यागम सत्य;
}

/*
 * %cvt/ur <idx>
 */
तार्किक सम्बन्धि_CVT_UR(vthread_t thr, vvp_संहिता_t cp)
{
      द्विगुण r = thr->निष्कासय_real();
      thr->शब्दाः[cp->bit_idx[0]].w_uint = vlg_आवर्तनम्_गन्तव्यम्_u64(r);

      प्रत्यागम सत्य;
}

/*
 * %cvt/vr <wid>
 */
तार्किक सम्बन्धि_CVT_VR(vthread_t thr, vvp_संहिता_t cp)
{
      द्विगुण r = thr->निष्कासय_real();
      अचिह्नित wid = cp->सङ्ख्या;

      vvp_vector4_t tmp(wid, r);
      thr->प्रक्षिप_vec4(tmp);
      प्रत्यागम सत्य;
}

/*
 * This implements the %deassign instruction. All we do is write a
 * long(1) to port-3 of the addressed net. This turns off an active
 * continuous assign activated by %cassign/v
 */
तार्किक सम्बन्धि_DEASSIGN(vthread_t, vvp_संहिता_t cp)
{
      vvp_net_t*net = cp->net;
      अचिह्नित आधारः  = cp->bit_idx[0];
      अचिह्नित width = cp->bit_idx[1];

      vvp_signal_मानम्*fil = dynamic_cast<vvp_signal_मानम्*> (net->fil);
      प्रतिज्ञा(fil);
      vvp_fun_signal_vec*sig = dynamic_cast<vvp_fun_signal_vec*>(net->fun);
      प्रतिज्ञा(sig);

      यदि (आधारः >= fil->मानम्_आकार()) प्रत्यागम सत्य;
      यदि (आधारः+width > fil->मानम्_आकार()) width = fil->मानम्_आकार() - आधारः;

      तार्किक पूर्णम्_sig = आधारः == 0 && width == fil->मानम्_आकार();

	// This is the net that is forcing me...
      यदि (vvp_net_t*src = sig->cassign_link) {
	    यदि (! पूर्णम्_sig) {
		  fprintf(stderr, "Sorry: when a signal is assigning a "
		          "register, I cannot deassign part of it.\n");
		  निर्गच्छ(1);
	    }
	      // And this is the pointer to be removed.
	    vvp_net_ptr_t dst_ptr (net, 1);
	    src->unlink(dst_ptr);
	    sig->cassign_link = 0;
      }

	/* Do we release all or part of the net? */
      यदि (पूर्णम्_sig) {
	    sig->deassign();
      } अन्यथा {
	    sig->deassign_pv(आधारः, width);
      }

      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_DEASSIGN_WR(vthread_t, vvp_संहिता_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_real*sig = dynamic_cast<vvp_fun_signal_real*>(net->fun);
      प्रतिज्ञा(sig);

	// This is the net that is forcing me...
      यदि (vvp_net_t*src = sig->cassign_link) {
	      // And this is the pointer to be removed.
	    vvp_net_ptr_t dst_ptr (net, 1);
	    src->unlink(dst_ptr);
	    sig->cassign_link = 0;
      }

      sig->deassign();

      प्रत्यागम सत्य;
}

/*
 * %debug/thr
 */
तार्किक सम्बन्धि_DEBUG_THR(vthread_t thr, vvp_संहिता_t cp)
{
      ध्रुव वर्ण*text = cp->text;
      thr->debug_dump(cerr, text);
      प्रत्यागम सत्य;
}

/*
 * The delay takes two 32bit numbers to make up a 64bit time.
 *
 *   %delay <low>, <hig>
 */
तार्किक सम्बन्धि_DELAY(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_time64_t नीचः = cp->bit_idx[0];
      vvp_time64_t hig = cp->bit_idx[1];

      vvp_time64_t delay = (hig << 32) | नीचः;

      यदि (delay == 0) कालसारणी_inactive(thr);
      अन्यथा कालसारणी_vthread(thr, delay);
      प्रत्यागम असत्य;
}

तार्किक सम्बन्धि_DELAYX(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_time64_t delay;

      प्रतिज्ञा(cp->सङ्ख्या < vthread_s::शब्दाः_गणनम्);
      delay = thr->शब्दाः[cp->सङ्ख्या].w_uint;
      यदि (delay == 0) कालसारणी_inactive(thr);
      अन्यथा कालसारणी_vthread(thr, delay);
      प्रत्यागम असत्य;
}

तार्किक सम्बन्धि_DELETE_ELEM(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_net_t*net = cp->net;

      int64_t idx_val = thr->शब्दाः[3].w_int;
      यदि (thr->flags[4] == BIT4_1) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with undefined index."
	         << पङ्क्त्यन्तः;
	    प्रत्यागम सत्य;
      }
      यदि (idx_val < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with negative index."
	         << पङ्क्त्यन्तः;
	    प्रत्यागम सत्य;
      }
      size_t idx = idx_val;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      प्रतिज्ञा(obj);

      vvp_queue*queue = obj->get_object().peek<vvp_queue>();
      यदि (queue == 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping delete(" << idx
	         << ") on empty queue." << पङ्क्त्यन्तः;
      } अन्यथा {
	    size_t size = queue->get_आकार();
	    यदि (idx >= size) {
		  cerr << thr->get_fileline()
		       << "Warning: skipping out of range delete(" << idx
		       << ") on queue of size " << size << "." << पङ्क्त्यन्तः;
	    } अन्यथा {
		  queue->erase(idx);
	    }
      }

      प्रत्यागम सत्य;
}

/* %delete/obj <label>
 *
 * This operator works by assigning a nil to the target object. This
 * causes any value that might be there to be garbage collected, thus
 * deleting the object.
 */
तार्किक सम्बन्धि_DELETE_OBJ(vthread_t thr, vvp_संहिता_t cp)
{
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_send_object(ptr, vvp_object_t(), thr->wt_context);

      प्रत्यागम सत्य;
}

/* %reverse/obj <label>
 * Reverse element order of the queue or dynamic array object at this net.
 */
तार्किक सम्बन्धि_REVERSE_OBJ(vthread_t /*thr*/, vvp_संहिता_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      प्रतिज्ञा(obj);

      vvp_object_t stor = obj->get_object();
      यदि (stor.test_nil()) प्रत्यागम सत्य;

	/* Match other queue ops: peek queues before vvp_darray so concrete
	   queue types dispatch reverse_elems() correctly. */
      यदि (vvp_queue* queue = stor.peek<vvp_queue>()) {
	    queue->reverse_elems();
	    प्रत्यागम सत्य;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      प्रतिज्ञा(darray);
      darray->reverse_elems();

      प्रत्यागम सत्य;
}

/* %reverse/prop/obj <property-index>
 * Reverse queue/darray property storage inside the class object on stack top.
 */
तार्किक सम्बन्धि_REVERSE_PROP_OBJ(vthread_t thr, vvp_संहिता_t cp)
{
      size_t pid = cp->सङ्ख्या;

      vvp_object_t& शिखरम् = thr->peek_object();
      vvp_cobject*cobj = शिखरम्.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      यदि (arr_obj.test_nil()) प्रत्यागम सत्य;

      यदि (vvp_queue* queue = arr_obj.peek<vvp_queue>()) {
	    queue->reverse_elems();
	    प्रत्यागम सत्य;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      प्रतिज्ञा(darray);
      darray->reverse_elems();

      प्रत्यागम सत्य;
}

/* %sort/obj — ascending sort */
तार्किक सम्बन्धि_क्रमय_OBJ(vthread_t /*thr*/, vvp_संहिता_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      प्रतिज्ञा(obj);

      vvp_object_t stor = obj->get_object();
      यदि (stor.test_nil()) प्रत्यागम सत्य;

      यदि (vvp_queue* queue = stor.peek<vvp_queue>()) {
	    queue->क्रमय_elems(सत्य);
	    प्रत्यागम सत्य;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      प्रतिज्ञा(darray);
      darray->क्रमय_elems(सत्य);

      प्रत्यागम सत्य;
}

/* %sort/prop/obj */
तार्किक सम्बन्धि_क्रमय_PROP_OBJ(vthread_t thr, vvp_संहिता_t cp)
{
      size_t pid = cp->सङ्ख्या;

      vvp_object_t& शिखरम् = thr->peek_object();
      vvp_cobject*cobj = शिखरम्.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      यदि (arr_obj.test_nil()) प्रत्यागम सत्य;

      यदि (vvp_queue* queue = arr_obj.peek<vvp_queue>()) {
	    queue->क्रमय_elems(सत्य);
	    प्रत्यागम सत्य;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      प्रतिज्ञा(darray);
      darray->क्रमय_elems(सत्य);

      प्रत्यागम सत्य;
}

/* %rsort/obj — descending sort */
तार्किक सम्बन्धि_RSORT_OBJ(vthread_t /*thr*/, vvp_संहिता_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      प्रतिज्ञा(obj);

      vvp_object_t stor = obj->get_object();
      यदि (stor.test_nil()) प्रत्यागम सत्य;

      यदि (vvp_queue* queue = stor.peek<vvp_queue>()) {
	    queue->क्रमय_elems(असत्य);
	    प्रत्यागम सत्य;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      प्रतिज्ञा(darray);
      darray->क्रमय_elems(असत्य);

      प्रत्यागम सत्य;
}

/* %rsort/prop/obj */
तार्किक सम्बन्धि_RSORT_PROP_OBJ(vthread_t thr, vvp_संहिता_t cp)
{
      size_t pid = cp->सङ्ख्या;

      vvp_object_t& शिखरम् = thr->peek_object();
      vvp_cobject*cobj = शिखरम्.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      यदि (arr_obj.test_nil()) प्रत्यागम सत्य;

      यदि (vvp_queue* queue = arr_obj.peek<vvp_queue>()) {
	    queue->क्रमय_elems(असत्य);
	    प्रत्यागम सत्य;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      प्रतिज्ञा(darray);
      darray->क्रमय_elems(असत्य);

      प्रत्यागम सत्य;
}

/* %shuffle/obj */
तार्किक सम्बन्धि_SHUFFLE_OBJ(vthread_t /*thr*/, vvp_संहिता_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      प्रतिज्ञा(obj);

      vvp_object_t stor = obj->get_object();
      यदि (stor.test_nil()) प्रत्यागम सत्य;

      यदि (vvp_queue* queue = stor.peek<vvp_queue>()) {
	    queue->shuffle_elems();
	    प्रत्यागम सत्य;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      प्रतिज्ञा(darray);
      darray->shuffle_elems();

      प्रत्यागम सत्य;
}

/* %shuffle/prop/obj */
तार्किक सम्बन्धि_SHUFFLE_PROP_OBJ(vthread_t thr, vvp_संहिता_t cp)
{
      size_t pid = cp->सङ्ख्या;

      vvp_object_t& शिखरम् = thr->peek_object();
      vvp_cobject*cobj = शिखरम्.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      यदि (arr_obj.test_nil()) प्रत्यागम सत्य;

      यदि (vvp_queue* queue = arr_obj.peek<vvp_queue>()) {
	    queue->shuffle_elems();
	    प्रत्यागम सत्य;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      प्रतिज्ञा(darray);
      darray->shuffle_elems();

      प्रत्यागम सत्य;
}

/* %delete/tail <label>, idx
 *
 * Remove all elements after the one specified.
 */
तार्किक सम्बन्धि_DELETE_पुच्छ(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      प्रतिज्ञा(obj);

      vvp_queue*queue = obj->get_object().peek<vvp_queue>();
      प्रतिज्ञा(queue);

      अचिह्नित idx = thr->शब्दाः[cp->bit_idx[0]].w_int;
      queue->erase_पुच्छ(idx);

      प्रत्यागम सत्य;
}

स्थिर तार्किक do_disable(vthread_t thr, vthread_t मेलनम्)
{
      तार्किक flag = असत्य;

	/* Pull the target thread out of its scope if needed. */
      thr->parent_scope->तन्तवः.erase(thr);

	/* Turn the thread off by setting is program counter to
	   zero and setting an OFF bit. */
      thr->pc = codespace_null();
      thr->i_was_disabled = 1;
      thr->i_have_ended = 1;

	/* Turn off all the children of the thread. Simulate a %join
	   for as many times as needed to clear the results of all the
	   %forks that this thread has done. */
      यावत् (! thr->children.empty()) {

	    vthread_t tmp = *(thr->children.begin());
	    प्रतिज्ञा(tmp);
	    प्रतिज्ञा(tmp->parent == thr);
	    thr->i_am_joining = 0;
	    यदि (do_disable(tmp, मेलनम्))
		  flag = सत्य;

	    vthread_reap(tmp);
      }

      vthread_t parent = thr->parent;
      यदि (parent && parent->i_am_joining) {
	      // If a parent is waiting in a %join, wake it up. Note
	      // that it is possible to be waiting in a %join yet
	      // already scheduled if multiple child threads are
	      // ending. So check if the thread is already scheduled
	      // before scheduling it again.
	    parent->i_am_joining = 0;
	    यदि (! parent->i_have_ended)
		  कालसारणी_vthread(parent, 0, सत्य);

	    do_join(parent, thr);

      } अन्यथा यदि (parent) {
	      /* If the parent is yet to %join me, let its %join
		 do the reaping. */
	      //assert(tmp->is_scheduled == 0);

      } अन्यथा {
	      /* No parent at all. Goodbye. */
	    vthread_reap(thr);
      }

      प्रत्यागम flag || (thr == मेलनम्);
}

/*
 * Implement the %disable instruction by scanning the target scope for
 * all the target threads. Kill the target threads and wake up a
 * parent that is attempting a %join.
 */
तार्किक सम्बन्धि_DISABLE(vthread_t thr, vvp_संहिता_t cp)
{
      __vpiScope*scope = static_cast<__vpiScope*>(cp->handle);

      तार्किक disabled_myself_flag = असत्य;

      यावत् (! scope->तन्तवः.empty()) {
	    समुच्चयः<vthread_t>::iterator cur = scope->तन्तवः.begin();

	    यदि (do_disable(*cur, thr))
		  disabled_myself_flag = सत्य;
      }

      प्रत्यागम ! disabled_myself_flag;
}

/*
 * Similar to `of_DISABLE`. But will only disable a single thread of the
 * specified scope. The disabled thread will be the thread closest to the
 * current thread in thread hierarchy. This can either be the current thread,
 * either the thread itself or one of its parents.
 * This is used for SystemVerilog flow control instructions like `return`,
 * `continue` and `break`.
 */

तार्किक सम्बन्धि_DISABLE_FLOW(vthread_t thr, vvp_संहिता_t cp)
{
      ध्रुव __vpiScope*scope = static_cast<__vpiScope*>(cp->handle);
      vthread_t cur = thr;

      यावत् (cur && cur->parent_scope != scope)
	    cur = cur->parent;

      प्रतिज्ञा(cur);
      यदि (cur)
	    प्रत्यागम !do_disable(cur, thr);

      प्रत्यागम असत्य;
}

/*
 * Implement the %disable/fork (SystemVerilog) instruction by disabling
 * all the detached children of the given thread.
 */
तार्किक सम्बन्धि_DISABLE_FORK(vthread_t thr, vvp_संहिता_t)
{
	/* If a %disable/fork is being executed then the parent thread
	 * cannot be waiting in a join. */
      प्रतिज्ञा(! thr->i_am_joining);

	/* There should be no active children to disable. */
      प्रतिज्ञा(thr->children.empty());

	/* Disable any detached children. */
      यावत् (! thr->detached_children.empty()) {
	    vthread_t child = *(thr->detached_children.begin());
	    प्रतिज्ञा(child);
	    प्रतिज्ञा(child->parent == thr);
	      /* Disabling the children can never match the parent thread. */
	    तार्किक res = do_disable(child, thr);
	    प्रतिज्ञा(! res);
	    vthread_reap(child);
      }

      प्रत्यागम सत्य;
}

/*
 * This function divides a 2-word number {high, a} by a 1-word
 * number. Assume that high < b.
 */
स्थिर अचिह्नित दीर्घ divide2words(अचिह्नित दीर्घ a, अचिह्नित दीर्घ b,
				  अचिह्नित दीर्घ उच्चः)
{
      अचिह्नित दीर्घ परिणाम = 0;
      यावत् (उच्चः > 0) {
	    अचिह्नित दीर्घ tmp_परिणाम = ULONG_MAX / b;
	    अचिह्नित दीर्घ remain = ULONG_MAX % b;

	    remain += 1;
	    यदि (remain >= b) {
		  remain -= b;
		  tmp_परिणाम += 1;
	    }

	      // Now 0x1_0...0 = b*tmp_result + remain
	      // high*0x1_0...0 = high*(b*tmp_result + remain)
	      // high*0x1_0...0 = high*b*tmp_result + high*remain

	      // We know that high*0x1_0...0 >= high*b*tmp_result, and
	      // we know that high*0x1_0...0 > high*remain. Use
	      // high*remain as the remainder for another iteration,
	      // and add tmp_result*high into the current estimate of
	      // the result.
	    परिणाम += tmp_परिणाम * उच्चः;

	      // The new iteration starts with high*remain + a.
	    remain = multiply_with_carry(उच्चः, remain, उच्चः);
	    a += remain;
            यदि(a < remain)
              उच्चः += 1;

	      // Now result*b + {high,a} == the input {high,a}. It is
	      // possible that the new high >= 1. If so, it will
	      // certainly be less than high from the previous
	      // iteration. Do another iteration and it will shrink,
	      // eventually to 0.
      }

	// high is now 0, so a is the remaining remainder, so we can
	// finish off the integer divide with a simple a/b.

      प्रत्यागम परिणाम + a/b;
}

स्थिर अचिह्नित दीर्घ* divide_द्वयङ्काः(अचिह्नित दीर्घ*ap, ध्रुव अचिह्नित दीर्घ*bp, अचिह्नित wid)
{
	// Do all our work a cpu-word at a time. The "words" variable
	// is the number of words of the wid.
      अचिह्नित शब्दाः = (wid+CPU_WORD_द्वयङ्काः-1) / CPU_WORD_द्वयङ्काः;

      अचिह्नित btop = शब्दाः-1;
      यावत् (btop > 0 && bp[btop] == 0)
	    btop -= 1;

	// Detect divide by 0, and exit.
      यदि (btop==0 && bp[0]==0)
	    प्रत्यागम 0;

	// The result array will eventually accumulate the result. The
	// diff array is a difference that we use in the intermediate.
      अचिह्नित दीर्घ*diff  = नूतन अचिह्नित दीर्घ[शब्दाः];
      अचिह्नित दीर्घ*परिणाम= नूतन अचिह्नित दीर्घ[शब्दाः];
      प्रति (अचिह्नित idx = 0 ; idx < शब्दाः ; idx += 1)
	    परिणाम[idx] = 0;

      प्रति (अचिह्नित cur = शब्दाः-btop ; cur > 0 ; cur -= 1) {
	    अचिह्नित cur_ptr = cur-1;
	    अचिह्नित दीर्घ cur_res;
	    यदि (ap[cur_ptr+btop] >= bp[btop]) {
		  अचिह्नित दीर्घ उच्चः = 0;
		  यदि (cur_ptr+btop+1 < शब्दाः)
			उच्चः = ap[cur_ptr+btop+1];
		  cur_res = divide2words(ap[cur_ptr+btop], bp[btop], उच्चः);

	    } अन्यथा यदि (cur_ptr+btop+1 >= शब्दाः) {
		  अनुवर्त;

	    } अन्यथा यदि (ap[cur_ptr+btop+1] == 0) {
		  अनुवर्त;

	    } अन्यथा {
		  cur_res = divide2words(ap[cur_ptr+btop], bp[btop],
					 ap[cur_ptr+btop+1]);
	    }

	      // cur_res is a guesstimate of the result this far. It
	      // may be 1 too big. (But it will also be >0) Try it,
	      // and if the difference comes out negative, then adjust.

	      // diff = (bp * cur_res)  << cur_ptr;
	    multiply_array_imm(diff+cur_ptr, bp, शब्दाः-cur_ptr, cur_res);
	      // ap -= diff
	    अचिह्नित दीर्घ carry = 1;
	    प्रति (अचिह्नित idx = cur_ptr ; idx < शब्दाः ; idx += 1)
		  ap[idx] = सङ्कलय_with_carry(ap[idx], ~diff[idx], carry);

	      // ap has the diff subtracted out of it. If cur_res was
	      // too large, then ap will turn negative. (We easily
	      // tell that ap turned negative by looking at
	      // carry&1. If it is 0, then it is *negative*.) In that
	      // case, we know that cur_res was too large by 1. Correct by
	      // adding 1b back in and reducing cur_res.
	    यदि ((carry&1) == 0) {
		    // Keep adding b back in until the remainder
		    // becomes positive again.
		  कुरु {
			cur_res -= 1;
			carry = 0;
			प्रति (अचिह्नित idx = cur_ptr ; idx < शब्दाः ; idx += 1)
			      ap[idx] = सङ्कलय_with_carry(ap[idx], bp[idx-cur_ptr], carry);
		  } यावत् (carry == 0);
	    }

	    परिणाम[cur_ptr] = cur_res;
      }

	// Now ap contains the remainder and result contains the
	// desired result. We should find that:
	//  input-a = bp * result + ap;

      विलोपय[]diff;
      प्रत्यागम परिणाम;
}

/*
 * %div
 */
तार्किक सम्बन्धि_DIV(vthread_t thr, vvp_संहिता_t)
{
      vvp_vector4_t valb = thr->निष्कासय_vec4();
      vvp_vector4_t vala = thr->निष्कासय_vec4();

      प्रतिज्ञा(vala.size()== valb.size());
      अचिह्नित wid = vala.size();

      अचिह्नित दीर्घ*ap = vala.subarray(0, wid);
      यदि (ap == 0) {
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->प्रक्षिप_vec4(tmp);
	    प्रत्यागम सत्य;
      }

      अचिह्नित दीर्घ*bp = valb.subarray(0, wid);
      यदि (bp == 0) {
	    विलोपय[]ap;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->प्रक्षिप_vec4(tmp);
	    प्रत्यागम सत्य;
      }

	// If the value fits in a single CPU word, then do it the easy way.
      यदि (wid <= CPU_WORD_द्वयङ्काः) {
	    यदि (bp[0] == 0) {
		  vvp_vector4_t tmp(wid, BIT4_X);
		  thr->प्रक्षिप_vec4(tmp);
	    } अन्यथा {
		  ap[0] /= bp[0];
		  vala.setarray(0, wid, ap);
		  thr->प्रक्षिप_vec4(vala);
	    }
	    विलोपय[]ap;
	    विलोपय[]bp;
	    प्रत्यागम सत्य;
      }

      अचिह्नित दीर्घ*परिणाम = divide_द्वयङ्काः(ap, bp, wid);
      यदि (परिणाम == 0) {
	    विलोपय[]ap;
	    विलोपय[]bp;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->प्रक्षिप_vec4(tmp);
	    प्रत्यागम सत्य;
      }

	// Now ap contains the remainder and result contains the
	// desired result. We should find that:
	//  input-a = bp * result + ap;

      vala.setarray(0, wid, परिणाम);
      thr->प्रक्षिप_vec4(vala);
      विलोपय[]ap;
      विलोपय[]bp;
      विलोपय[]परिणाम;

      प्रत्यागम सत्य;
}


स्थिर शून्य negate_शब्दाः(अचिह्नित दीर्घ*val, अचिह्नित शब्दाः)
{
      अचिह्नित दीर्घ carry = 1;
      प्रति (अचिह्नित idx = 0 ; idx < शब्दाः ; idx += 1)
	    val[idx] = सङ्कलय_with_carry(0, ~val[idx], carry);
}

/*
 * %div/s
 */
तार्किक सम्बन्धि_DIV_S(vthread_t thr, vvp_संहिता_t)
{
      vvp_vector4_t valb = thr->निष्कासय_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      प्रतिज्ञा(vala.size()== valb.size());
      अचिह्नित wid = vala.size();
      अचिह्नित शब्दाः = (wid + CPU_WORD_द्वयङ्काः - 1) / CPU_WORD_द्वयङ्काः;

	// Get the values, left in right, in binary form. If there is
	// a problem with either (caused by an X or Z bit) then we
	// know right away that the entire result is X.
      अचिह्नित दीर्घ*ap = vala.subarray(0, wid);
      यदि (ap == 0) {
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    प्रत्यागम सत्य;
      }

      अचिह्नित दीर्घ*bp = valb.subarray(0, wid);
      यदि (bp == 0) {
	    विलोपय[]ap;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    प्रत्यागम सत्य;
      }

	// Sign extend the bits in the array to fill out the array.
      अचिह्नित दीर्घ sign_आवरणम् = 0;
      यदि (अचिह्नित दीर्घ sign_द्वयङ्काः = (शब्दाः*CPU_WORD_द्वयङ्काः) - wid) {
	    sign_आवरणम् = -1UL << (CPU_WORD_द्वयङ्काः-sign_द्वयङ्काः);
	    यदि (ap[शब्दाः-1] & (sign_आवरणम्>>1))
		  ap[शब्दाः-1] |= sign_आवरणम्;
	    यदि (bp[शब्दाः-1] & (sign_आवरणम्>>1))
		  bp[शब्दाः-1] |= sign_आवरणम्;
      }

	// If the value fits in a single word, then use the native divide.
      यदि (wid <= CPU_WORD_द्वयङ्काः) {
	    यदि (bp[0] == 0) {
		  vvp_vector4_t tmp(wid, BIT4_X);
		  vala = tmp;
	    } अन्यथा यदि (((दीर्घ)ap[0] == LONG_MIN) && ((दीर्घ)bp[0] == -1)) {
		  vvp_vector4_t tmp(wid, BIT4_0);
		  tmp.समुच्चयः_bit(wid-1, BIT4_1);
		  vala = tmp;
	    } अन्यथा {
		  दीर्घ tmpa = (दीर्घ) ap[0];
		  दीर्घ tmpb = (दीर्घ) bp[0];
		  दीर्घ res = tmpa / tmpb;
		  ap[0] = ((अचिह्नित दीर्घ)res) & ~sign_आवरणम्;
		  vala.setarray(0, wid, ap);
	    }
	    विलोपय[]ap;
	    विलोपय[]bp;
	    प्रत्यागम सत्य;
      }

	// We need to the actual division to positive integers. Make
	// them positive here, and remember the negations.
      तार्किक negate_flag = असत्य;
      यदि ( ((दीर्घ) ap[शब्दाः-1]) < 0 ) {
	    negate_flag = सत्य;
	    negate_शब्दाः(ap, शब्दाः);
      }
      यदि ( ((दीर्घ) bp[शब्दाः-1]) < 0 ) {
	    negate_flag ^= सत्य;
	    negate_शब्दाः(bp, शब्दाः);
      }

      अचिह्नित दीर्घ*परिणाम = divide_द्वयङ्काः(ap, bp, wid);
      यदि (परिणाम == 0) {
	    विलोपय[]ap;
	    विलोपय[]bp;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    प्रत्यागम सत्य;
      }

      यदि (negate_flag) {
	    negate_शब्दाः(परिणाम, शब्दाः);
      }

      परिणाम[शब्दाः-1] &= ~sign_आवरणम्;

      vala.setarray(0, wid, परिणाम);
      विलोपय[]ap;
      विलोपय[]bp;
      विलोपय[]परिणाम;
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_DIV_WR(vthread_t thr, vvp_संहिता_t)
{
      द्विगुण r = thr->निष्कासय_real();
      द्विगुण l = thr->निष्कासय_real();
      thr->प्रक्षिप_real(l / r);

      प्रत्यागम सत्य;
}

/*
 * %dup/obj
 * %dup/real
 * %dup/vec4
 *
 * Push a duplicate of the object on the appropriate stack.
 */
तार्किक सम्बन्धि_DUP_OBJ(vthread_t thr, vvp_संहिता_t)
{
      vvp_object_t src = thr->peek_object();

        // If it is null push a new null object
      यदि (src.test_nil())
	    thr->प्रक्षिप_object(vvp_object_t());
      अन्यथा
	    thr->प्रक्षिप_object(src.duplicate());

      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_DUP_REAL(vthread_t thr, vvp_संहिता_t)
{
      thr->प्रक्षिप_real(thr->peek_real(0));
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_DUP_VEC4(vthread_t thr, vvp_संहिता_t)
{
      thr->प्रक्षिप_vec4(thr->peek_vec4(0));
      प्रत्यागम सत्य;
}

/*
 * This terminates the current thread. If there is a parent who is
 * waiting for me to die, then I schedule it. At any rate, I mark
 * myself as a zombie by setting my pc to 0.
 */
तार्किक सम्बन्धि_अन्तः(vthread_t thr, vvp_संहिता_t)
{
      प्रतिज्ञा(! thr->प्रतीक्षा_for_event);
      thr->i_have_ended = 1;
      thr->pc = codespace_null();

	/* Fully detach any detached children. */
      यावत् (! thr->detached_children.empty()) {
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
      यदि (!thr->i_am_detached && thr->parent && thr->parent->i_am_joining) {
	    vthread_t tmp = thr->parent;
	    प्रतिज्ञा(! thr->i_am_detached);

	    tmp->i_am_joining = 0;
	    कालसारणी_vthread(tmp, 0, सत्य);
	    do_join(tmp, thr);
	    प्रत्यागम असत्य;
      }

	/* If this thread is not fully detached then remove it from the
	 * parents detached_children set and reap it. */
      यदि (thr->i_am_detached) {
	    vthread_t tmp = thr->parent;
	    प्रतिज्ञा(tmp);
	    size_t res = tmp->detached_children.erase(thr);
	    प्रतिज्ञा(res == 1);
	      /* If the parent is waiting for the detached children to
	       * finish then the last detached child needs to tell the
	       * parent to wake up when it is finished. */
	    यदि (tmp->i_am_प्रतीक्षा && tmp->detached_children.empty()) {
		  tmp->i_am_प्रतीक्षा = 0;
		  कालसारणी_vthread(tmp, 0, सत्य);
	    }
	      /* Fully detach this thread so it will be reaped below. */
	    thr->i_am_detached = 0;
	    thr->parent = 0;
      }

	/* If I have no parent, then no one can %join me and there is
	 * no reason to stick around. This can happen, for example if
	 * I am an ``initial'' thread. */
      यदि (thr->parent == 0) {
	    vthread_reap(thr);
	    प्रत्यागम असत्य;
      }

	/* If I make it this far, then I have a parent who may wish
	   to %join me. Remain a zombie so that it can. */

      प्रत्यागम असत्य;
}

/*
 * %event <var-label>
 */
तार्किक सम्बन्धि_EVENT(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_vector4_t tmp (1, BIT4_X);
      vvp_send_vec4(ptr, tmp, thr->wt_context);
      प्रत्यागम सत्य;
}

/*
 * %event/nb <var-label>, <delay>
 */
तार्किक सम्बन्धि_EVENT_NB(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_time64_t delay;

      delay = thr->शब्दाः[cp->bit_idx[0]].w_uint;
      कालसारणी_propagate_event(cp->net, delay);
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_EVCTL(vthread_t thr, vvp_संहिता_t cp)
{
      प्रतिज्ञा(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      thr->ecount = thr->शब्दाः[cp->bit_idx[0]].w_uint;
      प्रत्यागम सत्य;
}
तार्किक सम्बन्धि_EVCTLC(vthread_t thr, vvp_संहिता_t)
{
      thr->event = 0;
      thr->ecount = 0;
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_EVCTLI(vthread_t thr, vvp_संहिता_t cp)
{
      प्रतिज्ञा(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      thr->ecount = cp->bit_idx[0];
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_EVCTLS(vthread_t thr, vvp_संहिता_t cp)
{
      प्रतिज्ञा(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      int64_t val = thr->शब्दाः[cp->bit_idx[0]].w_int;
      यदि (val < 0) val = 0;
      thr->ecount = val;
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_FLAG_GET_VEC4(vthread_t thr, vvp_संहिता_t cp)
{
      पूर्णाङ्क flag = cp->सङ्ख्या;
      प्रतिज्ञा(flag < vthread_s::FLAGS_गणनम्);

      vvp_vector4_t val (1, thr->flags[flag]);
      thr->प्रक्षिप_vec4(val);

      प्रत्यागम सत्य;
}

/*
 * %flag_inv <flag1>
 */
तार्किक सम्बन्धि_FLAG_INV(vthread_t thr, vvp_संहिता_t cp)
{
      पूर्णाङ्क flag1 = cp->bit_idx[0];

      thr->flags[flag1] = ~ thr->flags[flag1];
      प्रत्यागम सत्य;
}

/*
 * %flag_mov <flag1>, <flag2>
 */
तार्किक सम्बन्धि_FLAG_MOV(vthread_t thr, vvp_संहिता_t cp)
{
      पूर्णाङ्क flag1 = cp->bit_idx[0];
      पूर्णाङ्क flag2 = cp->bit_idx[1];

      thr->flags[flag1] = thr->flags[flag2];
      प्रत्यागम सत्य;
}

/*
 * %flag_or <flag1>, <flag2>
 */
तार्किक सम्बन्धि_FLAG_OR(vthread_t thr, vvp_संहिता_t cp)
{
      पूर्णाङ्क flag1 = cp->bit_idx[0];
      पूर्णाङ्क flag2 = cp->bit_idx[1];

      thr->flags[flag1] = thr->flags[flag1] | thr->flags[flag2];
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_FLAG_समुच्चयः_IMM(vthread_t thr, vvp_संहिता_t cp)
{
      पूर्णाङ्क flag = cp->सङ्ख्या;
      पूर्णाङ्क vali = cp->bit_idx[0];

      प्रतिज्ञा(flag < vthread_s::FLAGS_गणनम्);
      प्रतिज्ञा(vali >= 0 && vali < 4);

      स्थिर ध्रुव vvp_bit4_t map_bit[4] = {BIT4_0, BIT4_1, BIT4_Z, BIT4_X};
      thr->flags[flag] = map_bit[vali];
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_FLAG_समुच्चयः_VEC4(vthread_t thr, vvp_संहिता_t cp)
{
      पूर्णाङ्क flag = cp->सङ्ख्या;
      प्रतिज्ञा(flag < vthread_s::FLAGS_गणनम्);

      ध्रुव vvp_vector4_t&val = thr->peek_vec4();
      thr->flags[flag] = val.value(0);
      thr->निष्कासय_vec4(1);

      प्रत्यागम सत्य;
}

/*
 * the %force/link instruction connects a source node to a
 * destination node. The destination node must be a signal, as it is
 * marked with the source of the force so that it may later be
 * unlinked without specifically knowing the source that this
 * instruction used.
 */
तार्किक सम्बन्धि_FORCE_LINK(vthread_t, vvp_संहिता_t cp)
{
      vvp_net_t*dst = cp->net;
      vvp_net_t*src = cp->net2;

      प्रतिज्ञा(dst->fil);
      dst->fil->force_link(dst, src);

      प्रत्यागम सत्य;
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
तार्किक सम्बन्धि_FORCE_VEC4(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_vector4_t value = thr->निष्कासय_vec4();

	/* Send the force value to the filter on the node. */

      प्रतिज्ञा(net->fil);
      यदि (value.size() != net->fil->filter_आकार())
	    value = coerce_गन्तव्यम्_विस्तारः(value, net->fil->filter_आकार());

      net->force_vec4(value, vvp_vector2_t(vvp_vector2_t::FILL1, net->fil->filter_आकार()));

      प्रत्यागम सत्य;
}

/*
 * %force/vec4/off <net>, <off>
 */
तार्किक सम्बन्धि_FORCE_VEC4_OFF(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_net_t*net = cp->net;
      अचिह्नित आधारः_idx = cp->bit_idx[0];
      दीर्घ आधारः = thr->शब्दाः[आधारः_idx].w_int;
      vvp_vector4_t value = thr->निष्कासय_vec4();
      अचिह्नित wid = value.size();

      प्रतिज्ञा(net->fil);

      यदि (thr->flags[4] == BIT4_1)
	    प्रत्यागम सत्य;

	// This is the width of the target vector.
      अचिह्नित use_आकार = net->fil->filter_आकार();

      यदि (आधारः >= (दीर्घ)use_आकार)
	    प्रत्यागम सत्य;
      यदि (आधारः < -(दीर्घ)use_आकार)
	    प्रत्यागम सत्य;

      यदि ((आधारः + wid) > use_आकार)
	    wid = use_आकार - आधारः;

	// Make a mask of which bits are to be forced, 0 for unforced
	// bits and 1 for forced bits.
      vvp_vector2_t आवरणम् (vvp_vector2_t::FILL0, use_आकार);
      प्रति (अचिह्नित idx = 0 ; idx < wid ; idx += 1)
	    आवरणम्.समुच्चयः_bit(आधारः+idx, 1);

      vvp_vector4_t tmp (use_आकार, BIT4_Z);

	// vvp_net_t::force_vec4 propagates all the bits of the
	// forced vector value, regardless of the mask. This
	// ensures the unforced bits retain their current value.
      vvp_signal_मानम्*sig = dynamic_cast<vvp_signal_मानम्*>(net->fil);
      प्रतिज्ञा(sig);
      sig->vec4_मानम्(tmp);

      tmp.समुच्चयः_vec(आधारः, value);

      net->force_vec4(tmp, आवरणम्);
      प्रत्यागम सत्य;
}

/*
 * %force/vec4/off/d <net>, <off>, <del>
 */
तार्किक सम्बन्धि_FORCE_VEC4_OFF_D(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_net_t*net = cp->net;

      अचिह्नित आधारः_idx = cp->bit_idx[0];
      दीर्घ आधारः = thr->शब्दाः[आधारः_idx].w_int;

      अचिह्नित delay_idx = cp->bit_idx[1];
      vvp_time64_t delay = thr->शब्दाः[delay_idx].w_uint;

      vvp_vector4_t value = thr->निष्कासय_vec4();

      प्रतिज्ञा(net->fil);

      यदि (thr->flags[4] == BIT4_1)
	    प्रत्यागम सत्य;

	// This is the width of the target vector.
      अचिह्नित use_आकार = net->fil->filter_आकार();

      यदि (आधारः >= (दीर्घ)use_आकार)
	    प्रत्यागम सत्य;
      यदि (आधारः < -(दीर्घ)use_आकार)
	    प्रत्यागम सत्य;

      कालसारणी_force_सदिशम्(net, आधारः, use_आकार, value, delay);
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_FORCE_WR(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_net_t*net  = cp->net;
      द्विगुण value = thr->निष्कासय_real();

      net->force_real(value, vvp_vector2_t(vvp_vector2_t::FILL1, 1));

      प्रत्यागम सत्य;
}

/*
 * The %fork instruction causes a new child to be created and pushed
 * in front of any existing child. This causes the new child to be
 * added to the list of children, and for me to be the parent of the
 * new child.
 */
तार्किक सम्बन्धि_FORK(vthread_t thr, vvp_संहिता_t cp)
{
      vthread_t child = vthread_new(cp->cptr2, cp->scope);

      यदि (cp->scope->is_automatic()) {
              /* The context allocated for this child is the top entry
                 on the write context stack. */
            child->wt_context = thr->wt_context;
            child->rd_context = thr->wt_context;
      }

      child->parent = thr;
      thr->children.insert(child);

      यदि (thr->i_am_अन्तर्_function) {
	    child->is_scheduled = 1;
	    child->i_am_अन्तर्_function = 1;
	    vthread_सततम्(child);
	    running_thread = thr;
      } अन्यथा {
	    कालसारणी_vthread(child, 0, सत्य);
      }
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_FREE(vthread_t thr, vvp_संहिता_t cp)
{
        /* Pop the child context from the read context stack. */
      vvp_context_t child_context = thr->rd_context;
      thr->rd_context = vvp_get_stacked_context(child_context);

        /* Free the context. */
      vthread_free_context(child_context, cp->scope);

      प्रत्यागम सत्य;
}

/*
 * %inv
 *
 * Logically, this pops a value, inverts is (Verilog style, with Z and
 * X converted to X) and pushes the result. We can more efficiently
 * just to the invert in place.
 */
तार्किक सम्बन्धि_INV(vthread_t thr, vvp_संहिता_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val.invert();
      प्रत्यागम सत्य;
}


/*
 * Index registers, arithmetic.
 */

स्थिर अन्तःस्थ int64_t get_as_64_bit(uint32_t नीचः_32, uint32_t उच्चः_32)
{
      int64_t नीचः = नीचः_32;
      int64_t res = उच्चः_32;

      res <<= 32;
      res |= नीचः;
      प्रत्यागम res;
}

तार्किक सम्बन्धि_IX_सङ्कलय(vthread_t thr, vvp_संहिता_t cp)
{
      thr->शब्दाः[cp->सङ्ख्या].w_int += get_as_64_bit(cp->bit_idx[0],
                                                    cp->bit_idx[1]);
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_IX_SUB(vthread_t thr, vvp_संहिता_t cp)
{
      thr->शब्दाः[cp->सङ्ख्या].w_int -= get_as_64_bit(cp->bit_idx[0],
                                                    cp->bit_idx[1]);
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_IX_MUL(vthread_t thr, vvp_संहिता_t cp)
{
      thr->शब्दाः[cp->सङ्ख्या].w_int *= get_as_64_bit(cp->bit_idx[0],
                                                    cp->bit_idx[1]);
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_IX_आरोपय(vthread_t thr, vvp_संहिता_t cp)
{
      thr->शब्दाः[cp->सङ्ख्या].w_int = get_as_64_bit(cp->bit_idx[0],
                                                   cp->bit_idx[1]);
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_IX_MOV(vthread_t thr, vvp_संहिता_t cp)
{
      thr->शब्दाः[cp->bit_idx[0]].w_int = thr->शब्दाः[cp->bit_idx[1]].w_int;
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_IX_GETV(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित सूचकांक = cp->bit_idx[0];
      vvp_net_t*net = cp->net;

      vvp_signal_मानम्*sig = dynamic_cast<vvp_signal_मानम्*>(net->fil);
      यदि (sig == 0) {
	    प्रतिज्ञा(net->fil);
	    cerr << thr->get_fileline()
	         << "%%ix/getv error: Net arg not a vector signal? "
		 << typeid(*net->fil).name() << पङ्क्त्यन्तः;
      }
      प्रतिज्ञा(sig);

      vvp_vector4_t vec;
      sig->vec4_मानम्(vec);
      तार्किक अतिप्रवाहः_flag;
      uint64_t val = 0;
      तार्किक ज्ञातम्_flag = vector4_गन्तव्यम्_मानम्(vec, अतिप्रवाहः_flag, val);

      यदि (ज्ञातम्_flag)
	    thr->शब्दाः[सूचकांक].w_uint = val;
      अन्यथा
	    thr->शब्दाः[सूचकांक].w_uint = 0;

	/* Set bit 4 as a flag if the input is unknown. */
      thr->flags[4] = ज्ञातम्_flag ? (अतिप्रवाहः_flag ? BIT4_X : BIT4_0) : BIT4_1;

      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_IX_GETV_S(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित सूचकांक = cp->bit_idx[0];
      vvp_net_t*net = cp->net;

      vvp_signal_मानम्*sig = dynamic_cast<vvp_signal_मानम्*>(net->fil);
      यदि (sig == 0) {
	    प्रतिज्ञा(net->fil);
	    cerr << thr->get_fileline()
	         << "%%ix/getv/s error: Net arg not a vector signal? "
		 << "fun=" << typeid(*net->fil).name()
		 << ", fil=" << (net->fil? typeid(*net->fil).name() : "<>")
		 << पङ्क्त्यन्तः;
      }
      प्रतिज्ञा(sig);

      vvp_vector4_t vec;
      sig->vec4_मानम्(vec);
      int64_t val;
      तार्किक ज्ञातम्_flag = vector4_गन्तव्यम्_मानम्(vec, val, सत्य, सत्य);

      यदि (ज्ञातम्_flag)
	    thr->शब्दाः[सूचकांक].w_int = val;
      अन्यथा
	    thr->शब्दाः[सूचकांक].w_int = 0;

	/* Set bit 4 as a flag if the input is unknown. */
      thr->flags[4] = ज्ञातम्_flag? BIT4_0 : BIT4_1;

      प्रत्यागम सत्य;
}

स्थिर uint64_t vec4_गन्तव्यम्_सूचकांक(vthread_t thr, तार्किक signed_flag)
{
	// Get all the information we need about the vec4 vector, then
	// pop it away. We only need the bool bits and the length.
      ध्रुव vvp_vector4_t&val = thr->peek_vec4();
      अचिह्नित val_आकार = val.size();
      अचिह्नित दीर्घ*द्वयङ्काः = val.subarray(0, val_आकार, असत्य);
      thr->निष्कासय_vec4(1);

	// If there are X/Z bits, then the subarray will give us a nil
	// pointer. Set a flag to indicate the error, and give up.
      यदि (द्वयङ्काः == 0) {
	    thr->flags[4] = BIT4_1;
	    प्रत्यागम 0;
      }

      uint64_t v = 0;
      thr->flags[4] = BIT4_0;

      प्रतिज्ञा(परिमाण(द्वयङ्काः[0]) <= परिमाण(v));

      v = 0;
      प्रति (अचिह्नित idx = 0 ; idx < val_आकार ; idx += 8*परिमाण(द्वयङ्काः[0])) {
	    uint64_t tmp = द्वयङ्काः[idx/8/परिमाण(द्वयङ्काः[0])];
	    यदि (idx < 8*परिमाण(v)) {
		  v |= tmp << idx;
	    } अन्यथा {
		  तार्किक अतिप्रवाहः = signed_flag && (v >> 63) ? ~tmp != 0 : tmp != 0;
		  यदि (अतिप्रवाहः) {
			thr->flags[4] = BIT4_X;
			भङ्ग;
		  }
	    }
      }

	// Set the high bits that are not necessarily filled in by the
	// subarray function.
      यदि (val_आकार < 8*परिमाण(v)) {
	    यदि (signed_flag && (v & (static_cast<uint64_t>(1)<<(val_आकार-1)))) {
		    // Propagate the sign bit...
		  v |= (~static_cast<uint64_t>(0)) << val_आकार;

	    } अन्यथा {
		    // Fill with zeros.
		  v &= ~((~static_cast<uint64_t>(0)) << val_आकार);
	    }

      }

      विलोपय[]द्वयङ्काः;
      प्रत्यागम v;
}

/*
 * %ix/vec4 <idx>
 */
तार्किक सम्बन्धि_IX_VEC4(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित use_idx = cp->सङ्ख्या;
      thr->शब्दाः[use_idx].w_uint = vec4_गन्तव्यम्_सूचकांक(thr, असत्य);
      प्रत्यागम सत्य;
}

/*
 * %ix/vec4/s <idx>
 */
तार्किक सम्बन्धि_IX_VEC4_S(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित use_idx = cp->सङ्ख्या;
      thr->शब्दाः[use_idx].w_uint = vec4_गन्तव्यम्_सूचकांक(thr, सत्य);
      प्रत्यागम सत्य;
}

/*
 * The various JMP instruction work simply by pulling the new program
 * counter from the instruction and resuming. If the jump is
 * conditional, then test the bit for the expected value first.
 */
तार्किक सम्बन्धि_JMP(vthread_t thr, vvp_संहिता_t cp)
{
      thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      यदि (कालसारणी_stopped()) {
	    कालसारणी_vthread(thr, 0, असत्य);
	    प्रत्यागम असत्य;
      }

      प्रत्यागम सत्य;
}

/*
 * %jmp/0 <pc>, <flag>
 */
तार्किक सम्बन्धि_JMP0(vthread_t thr, vvp_संहिता_t cp)
{
      यदि (thr->flags[cp->bit_idx[0]] == BIT4_0)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      यदि (कालसारणी_stopped()) {
	    कालसारणी_vthread(thr, 0, असत्य);
	    प्रत्यागम असत्य;
      }

      प्रत्यागम सत्य;
}

/*
 * %jmp/0xz <pc>, <flag>
 */
तार्किक सम्बन्धि_JMP0_XZ(vthread_t thr, vvp_संहिता_t cp)
{
      यदि (thr->flags[cp->bit_idx[0]] != BIT4_1)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      यदि (कालसारणी_stopped()) {
	    कालसारणी_vthread(thr, 0, असत्य);
	    प्रत्यागम असत्य;
      }

      प्रत्यागम सत्य;
}

/*
 * %jmp/1 <pc>, <flag>
 */
तार्किक सम्बन्धि_JMP1(vthread_t thr, vvp_संहिता_t cp)
{
      यदि (thr->flags[cp->bit_idx[0]] == BIT4_1)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      यदि (कालसारणी_stopped()) {
	    कालसारणी_vthread(thr, 0, असत्य);
	    प्रत्यागम असत्य;
      }

      प्रत्यागम सत्य;
}

/*
 * %jmp/1xz <pc>, <flag>
 */
तार्किक सम्बन्धि_JMP1_XZ(vthread_t thr, vvp_संहिता_t cp)
{
      यदि (thr->flags[cp->bit_idx[0]] != BIT4_0)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      यदि (कालसारणी_stopped()) {
	    कालसारणी_vthread(thr, 0, असत्य);
	    प्रत्यागम असत्य;
      }

      प्रत्यागम सत्य;
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
      यदि (child->wt_context) {
              /* and is the top level task/function thread... */
            यदि (thr->wt_context != thr->rd_context) {
                    /* Pop the child context from the write context stack. */
                  vvp_context_t child_context = thr->wt_context;
                  thr->wt_context = vvp_get_stacked_context(child_context);

                    /* Push the child context onto the read context stack */
                  vvp_समुच्चयः_stacked_context(child_context, thr->rd_context);
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
      प्रति (समुच्चयः<vthread_t>::iterator cur = thr->children.begin()
		 ; cur != thr->children.अन्तः() ; ++cur) {
	    vthread_t curp = *cur;
	    यदि (! curp->i_have_ended)
		  अनुवर्त;

	      // found something!
	    do_join(thr, curp);
	    प्रत्यागम सत्य;
      }

	// Otherwise, tell my children to awaken me when they end,
	// then pause.
      thr->i_am_joining = 1;
      प्रत्यागम असत्य;
}

तार्किक सम्बन्धि_JOIN(vthread_t thr, vvp_संहिता_t)
{
      प्रत्यागम do_join_opcode(thr);
}

/*
 * This %join/detach <n> instruction causes the thread to detach
 * threads that were created by an earlier %fork.
 */
तार्किक सम्बन्धि_JOIN_DETACH(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित दीर्घ गणनम् = cp->सङ्ख्या;

      प्रतिज्ञा(गणनम् == thr->children.size());

      यावत् (! thr->children.empty()) {
	    vthread_t child = *thr->children.begin();
	    प्रतिज्ञा(child->parent == thr);

	      // We cannot detach automatic tasks/functions within an
	      // automatic scope. If we try to do that, we might make
	      // a mess of the allocation of the context. Note that it
	      // is OK if the child context is distinct (See %exec_ufunc.)
	    प्रतिज्ञा(child->wt_context==0 || thr->wt_context!=child->wt_context);
	    यदि (child->i_have_ended) {
		    // If the child has already ended, then reap it.
		  vthread_reap(child);

	    } अन्यथा {
		  size_t res = child->parent->children.erase(child);
		  प्रतिज्ञा(res == 1);
		  child->i_am_detached = 1;
		  thr->detached_children.insert(child);
	    }
      }

      प्रत्यागम सत्य;
}

/*
 * %load/ar <array-label>, <index>;
*/
तार्किक सम्बन्धि_आरोपय_AR(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित idx = cp->bit_idx[0];
      द्विगुण word;

	/* The result is 0.0 if the address is undefined. */
      यदि (thr->flags[4] == BIT4_1) {
	    word = 0.0;
      } अन्यथा {
	    अचिह्नित adr = thr->शब्दाः[idx].w_int;
	    word = cp->array->get_word_r(adr);
      }

      thr->प्रक्षिप_real(word);
      प्रत्यागम सत्य;
}

प्रतिरूपम् <प्रकारनाम ELEM>
स्थिर तार्किक आरोपय_dar(vthread_t thr, vvp_संहिता_t cp)
{
      int64_t adr = thr->शब्दाः[3].w_int;
      vvp_net_t*net = cp->net;
      प्रतिज्ञा(net);

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      प्रतिज्ञा(obj);

      vvp_darray*darray = obj->get_object().peek<vvp_darray>();

      ELEM word;
      यदि (darray &&
          (adr >= 0) && (thr->flags[4] == BIT4_0)) // A defined address >= 0
	    darray->get_word(adr, word);
      अन्यथा
	    dq_default(word, obj->size());

      vthread_प्रक्षिप(thr, word);
      प्रत्यागम सत्य;
}

/*
 * %load/dar/r <array-label>;
 */
तार्किक सम्बन्धि_आरोपय_DAR_R(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम आरोपय_dar<द्विगुण>(thr, cp);
}

/*
 * %load/dar/str <array-label>;
 */
तार्किक सम्बन्धि_आरोपय_DAR_STR(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम आरोपय_dar<string>(thr, cp);
}

/*
 * %load/dar/vec4 <array-label>;
 */
तार्किक सम्बन्धि_आरोपय_DAR_VEC4(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम आरोपय_dar<vvp_vector4_t>(thr, cp);
}

/*
 * %load/prop/dar/vec4 <pid>, <wid>;
 *   Indexed read of queue/dynamic-array class property; object on stack,
 *   index in words[3] (same as %load/dar/vec4).
 */
तार्किक सम्बन्धि_आरोपय_PROP_DAR_VEC4(vthread_t thr, vvp_संहिता_t cp)
{
      size_t pid = cp->सङ्ख्या;
      अचिह्नित wid = cp->bit_idx[0];
      int64_t adr = thr->शब्दाः[3].w_int;

      vvp_object_t& शिखरम् = thr->peek_object();
      vvp_cobject*cobj = शिखरम्.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      vvp_object_t pobj;
      cobj->get_object(pid, pobj, 0);
      vvp_darray*darray = pobj.peek<vvp_darray>();

      vvp_vector4_t word;
      यदि (darray && (adr >= 0) && (thr->flags[4] == BIT4_0)) {
	    darray->get_word(adr, word);
      } अन्यथा {
	    dq_default(word, wid);
      }

      thr->प्रक्षिप_vec4(word);
      प्रत्यागम सत्य;
}

/*
 * %load/obj <var-label>
 */
तार्किक सम्बन्धि_आरोपय_OBJ(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_net_t*net = cp->net;
      vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      प्रतिज्ञा(fun);

      vvp_object_t val = fun->get_object();
      thr->प्रक्षिप_object(val);

      प्रत्यागम सत्य;
}

/*
 * %load/obja <index>
 *    Loads the object from array, using index <index> as the index
 *    value. If flags[4] == 1, the calculation of <index> may have
 *    failed, so push nil.
 */
तार्किक सम्बन्धि_आरोपय_OBJA(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित idx = cp->bit_idx[0];
      vvp_object_t word;

	/* The result is 0.0 if the address is undefined. */
      यदि (thr->flags[4] == BIT4_1) {
	    ; // Return nil
      } अन्यथा {
	    अचिह्नित adr = thr->शब्दाः[idx].w_int;
	    cp->array->get_word_obj(adr, word);
      }

      thr->प्रक्षिप_object(word);
      प्रत्यागम सत्य;
}

/*
 * %load/real <var-label>
 */
तार्किक सम्बन्धि_आरोपय_REAL(vthread_t thr, vvp_संहिता_t cp)
{
      __vpiHandle*tmp = cp->handle;
      t_vpi_मानम् val;

      val.format = vpiRealVal;
      vpi_get_मानम्(tmp, &val);

      thr->प्रक्षिप_real(val.value.real);

      प्रत्यागम सत्य;
}

/*
 * %load/str <var-label>
 */
तार्किक सम्बन्धि_आरोपय_STR(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_net_t*net = cp->net;


      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (net->fun);
      प्रतिज्ञा(fun);

      ध्रुव string&val = fun->get_string();
      thr->प्रक्षिप_str(val);

      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_आरोपय_STRA(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित idx = cp->bit_idx[0];
      string word;

      यदि (thr->flags[4] == BIT4_1) {
	    word = "";
      } अन्यथा {
	    अचिह्नित adr = thr->शब्दाः[idx].w_int;
	    word = cp->array->get_word_str(adr);
      }

      thr->प्रक्षिप_str(word);
      प्रत्यागम सत्य;
}


/*
 * %load/vec4 <net>
 */
तार्किक सम्बन्धि_आरोपय_VEC4(vthread_t thr, vvp_संहिता_t cp)
{
	// Push a placeholder onto the stack in order to reserve the
	// stack space. Use a reference for the stack top as a target
	// for the load.
      thr->प्रक्षिप_vec4(vvp_vector4_t());
      vvp_vector4_t&sig_मानम् = thr->peek_vec4();

      vvp_net_t*net = cp->net;

	// For the %load to work, the functor must actually be a
	// signal functor. Only signals save their vector value.
      vvp_signal_मानम्*sig = dynamic_cast<vvp_signal_मानम्*> (net->fil);
      यदि (sig == 0) {
	    cerr << thr->get_fileline()
	         << "%load/v error: Net arg not a signal? "
		 << (net->fil ? typeid(*net->fil).name() :
	                        typeid(*net->fun).name())
	         << पङ्क्त्यन्तः;
	    प्रतिज्ञा(sig);
	    प्रत्यागम सत्य;
      }

	// Extract the value from the signal and directly into the
	// target stack position.
      sig->vec4_मानम्(sig_मानम्);

      प्रत्यागम सत्य;
}

/*
 * %load/vec4a <arr>, <adrx>
 */
तार्किक सम्बन्धि_आरोपय_VEC4_A(vthread_t thr, vvp_संहिता_t cp)
{
      पूर्णाङ्क adr_सूचकांक = cp->bit_idx[0];

      दीर्घ adr = thr->शब्दाः[adr_सूचकांक].w_int;

	// If flag[3] is set, then the calculation of the address
	// failed, and this load should return X instead of the actual
	// value.
      यदि (thr->flags[4] == BIT4_1) {
	    vvp_vector4_t tmp (cp->array->get_word_आकार(), BIT4_X);
	    thr->प्रक्षिप_vec4(tmp);
	    प्रत्यागम सत्य;
      }

      vvp_vector4_t tmp (cp->array->get_word(adr));
      thr->प्रक्षिप_vec4(tmp);
      प्रत्यागम सत्य;
}

स्थिर शून्य do_verylong_mod(vvp_vector4_t&vala, ध्रुव vvp_vector4_t&valb,
			    तार्किक वामः_is_neg, तार्किक दक्षिणः_is_neg)
{
      तार्किक बहिर्गमनम्_is_neg = वामः_is_neg;
      ध्रुव पूर्णाङ्क len=vala.size();
      अचिह्नित वर्ण *a, *z, *t;
      a = नूतन अचिह्नित वर्ण[len+1];
      z = नूतन अचिह्नित वर्ण[len+1];
      t = नूतन अचिह्नित वर्ण[len+1];

      अचिह्नित वर्ण carry;
      अचिह्नित वर्ण अस्थायी;

      पूर्णाङ्क mxa = -1, mxz = -1;
      पूर्णाङ्क i;
      पूर्णाङ्क current, copylen;

      अचिह्नित lb_carry = वामः_is_neg? 1 : 0;
      अचिह्नित rb_carry = दक्षिणः_is_neg? 1 : 0;
      प्रति (पूर्णाङ्क idx = 0 ;  idx < len ;  idx += 1) {
	    अचिह्नित lb = vala.value(idx);
	    अचिह्नित rb = valb.value(idx);

	    यदि ((lb | rb) & 2) {
		  विलोपय []t;
		  विलोपय []z;
		  विलोपय []a;
		  vvp_vector4_t tmp(len, BIT4_X);
		  vala = tmp;
		  प्रत्यागम;
	    }

	    यदि (वामः_is_neg) {
		  lb = (1-lb) + lb_carry;
		  lb_carry = (lb & ~1)? 1 : 0;
		  lb &= 1;
	    }
	    यदि (दक्षिणः_is_neg) {
		  rb = (1-rb) + rb_carry;
		  rb_carry = (rb & ~1)? 1 : 0;
		  rb &= 1;
	    }

	    z[idx]=lb;
	    a[idx]=1-rb;	// for 2s complement add..
      }

      z[len]=0;
      a[len]=1;

      प्रति(i=len-1;i>=0;i--) {
	    यदि(! a[i]) {
		  mxa=i;
		  भङ्ग;
	    }
      }

      प्रति(i=len-1;i>=0;i--) {
	    यदि(z[i]) {
		  mxz=i;
		  भङ्ग;
	    }
      }

      यदि((mxa>mxz)||(mxa==-1)) {
	    यदि(mxa==-1) {
		  विलोपय []t;
		  विलोपय []z;
		  विलोपय []a;
		  vvp_vector4_t tmpx (len, BIT4_X);
		  vala = tmpx;
		  प्रत्यागम;
	    }

	    गच्छ tally;
      }

      copylen = mxa + 2;
      current = mxz - mxa;

      यावत्(current > -1) {
	    carry = 1;
	    प्रति(i=0;i<copylen;i++) {
		  अस्थायी = z[i+current] + a[i] + carry;
		  t[i] = (अस्थायी&1);
		  carry = (अस्थायी>>1);
	    }

	    यदि(carry) {
		  प्रति(i=0;i<copylen;i++) {
			z[i+current] = t[i];
		  }
	    }

	    current--;
      }

 tally:

      vvp_vector4_t tmp (len, BIT4_X);
      carry = बहिर्गमनम्_is_neg? 1 : 0;
      प्रति (पूर्णाङ्क idx = 0 ;  idx < len ;  idx += 1) {
	    अचिह्नित ob = z[idx];
	    यदि (बहिर्गमनम्_is_neg) {
		  ob = (1-ob) + carry;
		  carry = (ob & ~1)? 1 : 0;
		  ob = ob & 1;
	    }
	    tmp.समुच्चयः_bit(idx, ob?BIT4_1:BIT4_0);
      }
      vala = tmp;
      विलोपय []t;
      विलोपय []z;
      विलोपय []a;
}

तार्किक सम्बन्धि_MAX_WR(vthread_t thr, vvp_संहिता_t)
{
      द्विगुण r = thr->निष्कासय_real();
      द्विगुण l = thr->निष्कासय_real();
      यदि (r != r)
	    thr->प्रक्षिप_real(l);
      अन्यथा यदि (l != l)
	    thr->प्रक्षिप_real(r);
      अन्यथा यदि (r < l)
	    thr->प्रक्षिप_real(l);
      अन्यथा
	    thr->प्रक्षिप_real(r);
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_MIN_WR(vthread_t thr, vvp_संहिता_t)
{
      द्विगुण r = thr->निष्कासय_real();
      द्विगुण l = thr->निष्कासय_real();
      यदि (r != r)
	    thr->प्रक्षिप_real(l);
      अन्यथा यदि (l != l)
	    thr->प्रक्षिप_real(r);
      अन्यथा यदि (r < l)
	    thr->प्रक्षिप_real(r);
      अन्यथा
	    thr->प्रक्षिप_real(l);
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_MOD(vthread_t thr, vvp_संहिता_t)
{
      vvp_vector4_t valb = thr->निष्कासय_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      प्रतिज्ञा(vala.size()==valb.size());
      अचिह्नित wid = vala.size();

      यदि(wid <= 8*परिमाण(अचिह्नित दीर्घ दीर्घ)) {
	    अचिह्नित दीर्घ दीर्घ lv = 0, rv = 0;

	    प्रति (अचिह्नित idx = 0 ;  idx < wid ;  idx += 1) {
		  अचिह्नित दीर्घ दीर्घ lb = vala.value(idx);
		  अचिह्नित दीर्घ दीर्घ rb = valb.value(idx);

		  यदि ((lb | rb) & 2)
			गच्छ x_बहिर्गमनम्;

		  lv |= (अचिह्नित दीर्घ दीर्घ) lb << idx;
		  rv |= (अचिह्नित दीर्घ दीर्घ) rb << idx;
	    }

	    यदि (rv == 0)
		  गच्छ x_बहिर्गमनम्;

	    lv %= rv;

	    प्रति (अचिह्नित idx = 0 ;  idx < wid ;  idx += 1) {
		  vala.समुच्चयः_bit(idx, (lv&1)?BIT4_1 : BIT4_0);
		  lv >>= 1;
	    }

	    प्रत्यागम सत्य;

      } अन्यथा {
	    do_verylong_mod(vala, valb, असत्य, असत्य);
	    प्रत्यागम सत्य;
      }

 x_बहिर्गमनम्:
      vala = vvp_vector4_t(wid, BIT4_X);
      प्रत्यागम सत्य;
}

/*
 * %mod/s
 */
तार्किक सम्बन्धि_MOD_S(vthread_t thr, vvp_संहिता_t)
{
      vvp_vector4_t valb = thr->निष्कासय_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      प्रतिज्ञा(vala.size()==valb.size());
      अचिह्नित wid = vala.size();

	/* Handle the case that we can fit the bits into a long-long
	   variable. We cause use native % to do the work. */
      यदि(wid <= 8*परिमाण(दीर्घ दीर्घ)) {
	    दीर्घ दीर्घ lv = 0, rv = 0;

	    प्रति (अचिह्नित idx = 0 ;  idx < wid ;  idx += 1) {
		  दीर्घ दीर्घ lb = vala.value(idx);
		  दीर्घ दीर्घ rb = valb.value(idx);

		  यदि ((lb | rb) & 2)
			गच्छ x_बहिर्गमनम्;

		  lv |= (दीर्घ दीर्घ) lb << idx;
		  rv |= (दीर्घ दीर्घ) rb << idx;
	    }

	    यदि (rv == 0)
		  गच्छ x_बहिर्गमनम्;

	    यदि ((lv == LLONG_MIN) && (rv == -1))
		  गच्छ zero_बहिर्गमनम्;

	      /* Sign extend the signed operands when needed. */
	    यदि (wid < 8*परिमाण(दीर्घ दीर्घ)) {
		  यदि (lv & (1LL << (wid-1)))
			lv |= -1ULL << wid;
		  यदि (rv & (1LL << (wid-1)))
			rv |= -1ULL << wid;
	    }

	    lv %= rv;

	    प्रति (अचिह्नित idx = 0 ;  idx < wid ;  idx += 1) {
		  vala.समुच्चयः_bit(idx, (lv&1)? BIT4_1 : BIT4_0);
		  lv >>= 1;
	    }

	      // vala is the top of the stack, edited in place, so we
	      // do not need to push the result.

	    प्रत्यागम सत्य;

      } अन्यथा {

	    तार्किक वामः_is_neg  = vala.value(vala.size()-1) == BIT4_1;
	    तार्किक दक्षिणः_is_neg = valb.value(valb.size()-1) == BIT4_1;
	    do_verylong_mod(vala, valb, वामः_is_neg, दक्षिणः_is_neg);
	    प्रत्यागम सत्य;
      }

 x_बहिर्गमनम्:
      vala = vvp_vector4_t(wid, BIT4_X);
      प्रत्यागम सत्य;
 zero_बहिर्गमनम्:
      vala = vvp_vector4_t(wid, BIT4_0);
      प्रत्यागम सत्य;
}

/*
 * %mod/wr
 */
तार्किक सम्बन्धि_MOD_WR(vthread_t thr, vvp_संहिता_t)
{
      द्विगुण r = thr->निष्कासय_real();
      द्विगुण l = thr->निष्कासय_real();
      thr->प्रक्षिप_real(fmod(l,r));

      प्रत्यागम सत्य;
}

/*
 * %neg/wr
 */
तार्किक सम्बन्धि_NEG_WR(vthread_t thr, vvp_संहिता_t)
{
      thr->poke_real(0, -thr->peek_real(0));
      प्रत्यागम सत्य;
}

/*
 * %pad/s <wid>
 */
तार्किक सम्बन्धि_PAD_S(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित wid = cp->सङ्ख्या;

      vvp_vector4_t&val = thr->peek_vec4();
      अचिह्नित old_आकार = val.size();

	// Sign-extend.
      यदि (old_आकार < wid)
	    val.resize(wid, val.value(old_आकार-1));
      अन्यथा
	    val.resize(wid);

      प्रत्यागम सत्य;
}

/*
 * %pad/u <wid>
 */
तार्किक सम्बन्धि_PAD_U(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित wid = cp->सङ्ख्या;

      vvp_vector4_t&val = thr->peek_vec4();
      val.resize(wid, BIT4_0);

      प्रत्यागम सत्य;
}

/*
 * %part/s <wid>
 * %part/u <wid>
 * Two values are popped from the stack. First, pop the canonical
 * index of the part select, and second is the value to be
 * selected. The result is pushed back to the stack.
 */
स्थिर तार्किक सम्बन्धि_PART_आधारः(vthread_t thr, vvp_संहिता_t cp, तार्किक signed_flag)
{
      अचिह्नित wid = cp->सङ्ख्या;

      vvp_vector4_t base4 = thr->निष्कासय_vec4();
      vvp_vector4_t&value = thr->peek_vec4();

      vvp_vector4_t res (wid, BIT4_X);

	// NOTE: This is treating the vector as signed. Is that correct?
      int32_t आधारः;
      तार्किक मानम्_ok = vector4_गन्तव्यम्_मानम्(base4, आधारः, signed_flag);
      यदि (! मानम्_ok) {
	    value = res;
	    प्रत्यागम सत्य;
      }

      यदि (आधारः >= (int32_t)value.size()) {
	    value = res;
	    प्रत्यागम सत्य;
      }

      यदि ((आधारः+(पूर्णाङ्क)wid) <= 0) {
	    value = res;
	    प्रत्यागम सत्य;
      }

      दीर्घ vbase = 0;
      यदि (आधारः < 0) {
	    vbase = -आधारः;
	    wid -= vbase;
	    आधारः = 0;
      }

      यदि ((आधारः+wid) > value.size()) {
	    wid = value.size() - आधारः;
      }

      res .समुच्चयः_vec(vbase, value.subvalue(आधारः, wid));
      value = res;

      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_PART_S(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम सम्बन्धि_PART_आधारः(thr, cp, सत्य);
}

तार्किक सम्बन्धि_PART_U(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम सम्बन्धि_PART_आधारः(thr, cp, असत्य);
}

/*
 * %parti/s <wid>, <basei>, <base_wid>
 * %parti/u <wid>, <basei>, <base_wid>
 *
 * Pop the value to be selected. The result is pushed back to the stack.
 */
स्थिर तार्किक सम्बन्धि_PARTI_आधारः(vthread_t thr, vvp_संहिता_t cp, तार्किक signed_flag)
{
      अचिह्नित wid = cp->सङ्ख्या;
      uint32_t आधारः = cp->bit_idx[0];
      uint32_t bwid = cp->bit_idx[1];

      vvp_vector4_t&value = thr->peek_vec4();

      vvp_vector4_t res (wid, BIT4_X);

	// NOTE: This is treating the vector as signed. Is that correct?
      int32_t use_आधारः = आधारः;
      यदि (signed_flag && bwid < 32 && (आधारः&(1<<(bwid-1)))) {
	    use_आधारः |= -1UL << bwid;
      }

      यदि (use_आधारः >= (int32_t)value.size()) {
	    value = res;
	    प्रत्यागम सत्य;
      }

      यदि ((use_आधारः+(int32_t)wid) <= 0) {
	    value = res;
	    प्रत्यागम सत्य;
      }

      दीर्घ vbase = 0;
      यदि (use_आधारः < 0) {
	    vbase = -use_आधारः;
	    wid -= vbase;
	    use_आधारः = 0;
      }

      यदि ((use_आधारः+wid) > value.size()) {
	    wid = value.size() - use_आधारः;
      }

      res .समुच्चयः_vec(vbase, value.subvalue(use_आधारः, wid));
      value = res;

      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_PARTI_S(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम सम्बन्धि_PARTI_आधारः(thr, cp, सत्य);
}

तार्किक सम्बन्धि_PARTI_U(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम सम्बन्धि_PARTI_आधारः(thr, cp, असत्य);
}

/*
 * %mul
 */
तार्किक सम्बन्धि_MUL(vthread_t thr, vvp_संहिता_t)
{
      vvp_vector4_t r = thr->निष्कासय_vec4();
	// Rather then pop l, use it directly from the stack. When we
	// assign to 'l', that will edit the top of the stack, which
	// replaces a pop and a pull.
      vvp_vector4_t&l = thr->peek_vec4();

      l.mul(r);
      प्रत्यागम सत्य;
}

/*
 * %muli <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
तार्किक सम्बन्धि_MULI(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित wid = cp->सङ्ख्या;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_तात्कालिकम्_rval (cp, r);

      l.mul(r);
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_MUL_WR(vthread_t thr, vvp_संहिता_t)
{
      द्विगुण r = thr->निष्कासय_real();
      द्विगुण l = thr->निष्कासय_real();
      thr->प्रक्षिप_real(l * r);

      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_NAND(vthread_t thr, vvp_संहिता_t)
{
      vvp_vector4_t valr = thr->निष्कासय_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      प्रतिज्ञा(vall.size() == valr.size());

      vall &= valr;
      vall.invert();

      प्रत्यागम सत्य;
}

/*
 * %new/cobj <vpi_object>
 * This creates a new cobject (SystemVerilog class object) and pushes
 * it to the stack. The <vpi-object> is a __vpiHandle that is a
 * vpiClassDefn object that defines the item to be created.
 */
तार्किक सम्बन्धि_NEW_COBJ(vthread_t thr, vvp_संहिता_t cp)
{
      ध्रुव class_type*defn = dynamic_cast<ध्रुव class_type*> (cp->handle);
      प्रतिज्ञा(defn);

      vvp_object_t tmp (नूतन vvp_cobject(defn));
      thr->प्रक्षिप_object(tmp);
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_NEW_DARRAY(vthread_t thr, vvp_संहिता_t cp)
{
      ध्रुव वर्ण*text = cp->text;
      size_t size = thr->शब्दाः[cp->bit_idx[0]].w_int;
      अचिह्नित word_wid;
      size_t n;

      vvp_object_t obj;
      यदि (वाक्यं_तुलय(text,"b8") == 0) {
	    obj = नूतन vvp_darray_atom<uint8_t>(size);
      } अन्यथा यदि (वाक्यं_तुलय(text,"b16") == 0) {
	    obj = नूतन vvp_darray_atom<uint16_t>(size);
      } अन्यथा यदि (वाक्यं_तुलय(text,"b32") == 0) {
	    obj = नूतन vvp_darray_atom<uint32_t>(size);
      } अन्यथा यदि (वाक्यं_तुलय(text,"b64") == 0) {
	    obj = नूतन vvp_darray_atom<uint64_t>(size);
      } अन्यथा यदि (वाक्यं_तुलय(text,"sb8") == 0) {
	    obj = नूतन vvp_darray_atom<int8_t>(size);
      } अन्यथा यदि (वाक्यं_तुलय(text,"sb16") == 0) {
	    obj = नूतन vvp_darray_atom<int16_t>(size);
      } अन्यथा यदि (वाक्यं_तुलय(text,"sb32") == 0) {
	    obj = नूतन vvp_darray_atom<int32_t>(size);
      } अन्यथा यदि (वाक्यं_तुलय(text,"sb64") == 0) {
	    obj = नूतन vvp_darray_atom<int64_t>(size);
      } अन्यथा यदि ((1 == वाक्यात्_पठ(text, "b%u%zn", &word_wid, &n)) &&
                 (n == वाक्यदैर्घ्यम्(text))) {
	    obj = नूतन vvp_darray_vec2(size, word_wid);
      } अन्यथा यदि ((1 == वाक्यात्_पठ(text, "sb%u%zn", &word_wid, &n)) &&
                 (n == वाक्यदैर्घ्यम्(text))) {
	    obj = नूतन vvp_darray_vec2(size, word_wid);
      } अन्यथा यदि ((1 == वाक्यात्_पठ(text, "v%u%zn", &word_wid, &n)) &&
                 (n == वाक्यदैर्घ्यम्(text))) {
	    obj = नूतन vvp_darray_vec4(size, word_wid);
      } अन्यथा यदि ((1 == वाक्यात्_पठ(text, "sv%u%zn", &word_wid, &n)) &&
                 (n == वाक्यदैर्घ्यम्(text))) {
	    obj = नूतन vvp_darray_vec4(size, word_wid);
      } अन्यथा यदि (वाक्यं_तुलय(text,"r") == 0) {
	    obj = नूतन vvp_darray_real(size);
      } अन्यथा यदि (वाक्यं_तुलय(text,"S") == 0) {
	    obj = नूतन vvp_darray_string(size);
      } अन्यथा {
	    cerr << get_fileline()
	         << "Internal error: Unsupported dynamic array type: "
	         << text << "." << पङ्क्त्यन्तः;
	    प्रतिज्ञा(0);
      }

      thr->प्रक्षिप_object(obj);

      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_NOOP(vthread_t, vvp_संहिता_t)
{
      प्रत्यागम सत्य;
}

/*
 * %nor/r
 */
तार्किक सम्बन्धि_NORR(vthread_t thr, vvp_संहिता_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_or());

      प्रत्यागम सत्य;
}

/*
 * Push a null to the object stack.
 */
तार्किक सम्बन्धि_NULL(vthread_t thr, vvp_संहिता_t)
{
      vvp_object_t tmp;
      thr->प्रक्षिप_object(tmp);
      प्रत्यागम सत्य;
}

/*
 * %and/r
 */
तार्किक सम्बन्धि_ANDR(vthread_t thr, vvp_संहिता_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_and());

      प्रत्यागम सत्य;
}

/*
 * %nand/r
 */
तार्किक सम्बन्धि_NANDR(vthread_t thr, vvp_संहिता_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_and());

      प्रत्यागम सत्य;
}

/*
 * %or/r
 */
तार्किक सम्बन्धि_ORR(vthread_t thr, vvp_संहिता_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_or());

      प्रत्यागम सत्य;
}

/*
 * %xor/r
 */
तार्किक सम्बन्धि_XORR(vthread_t thr, vvp_संहिता_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_xor());

      प्रत्यागम सत्य;
}

/*
 * %xnor/r
 */
तार्किक सम्बन्धि_XNORR(vthread_t thr, vvp_संहिता_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_xor());

      प्रत्यागम सत्य;
}

/*
 * %or
 */
तार्किक सम्बन्धि_OR(vthread_t thr, vvp_संहिता_t)
{
      vvp_vector4_t valb = thr->निष्कासय_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();
      vala |= valb;
      प्रत्यागम सत्य;
}

/*
 * %nor
 */
तार्किक सम्बन्धि_NOR(vthread_t thr, vvp_संहिता_t)
{
      vvp_vector4_t valr = thr->निष्कासय_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      प्रतिज्ञा(vall.size() == valr.size());

      vall |= valr;
      vall.invert();

      प्रत्यागम सत्य;
}

/*
 * %pop/obj <num>, <skip>
 */
तार्किक सम्बन्धि_निष्कासय_OBJ(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित cnt = cp->bit_idx[0];
      अचिह्नित skip = cp->bit_idx[1];

      thr->निष्कासय_object(cnt, skip);
      प्रत्यागम सत्य;
}

/*
 * %pop/real <number>
 */
तार्किक सम्बन्धि_निष्कासय_REAL(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित cnt = cp->सङ्ख्या;
      thr->निष्कासय_real(cnt);
      प्रत्यागम सत्य;
}

/*
 *  %pop/str <number>
 */
तार्किक सम्बन्धि_निष्कासय_STR(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित cnt = cp->सङ्ख्या;
      thr->निष्कासय_str(cnt);
      प्रत्यागम सत्य;
}

/*
 *  %pop/vec4 <number>
 */
तार्किक सम्बन्धि_निष्कासय_VEC4(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित cnt = cp->सङ्ख्या;
      thr->निष्कासय_vec4(cnt);
      प्रत्यागम सत्य;
}

/*
 * %pow
 * %pow/s
 */
स्थिर तार्किक सम्बन्धि_POW_आधारः(vthread_t thr, तार्किक signed_flag)
{
      vvp_vector4_t valb = thr->निष्कासय_vec4();
      vvp_vector4_t vala = thr->निष्कासय_vec4();

      अचिह्नित wid = vala.size();

      vvp_vector2_t xv2 = vvp_vector2_t(vala, सत्य);
      vvp_vector2_t yv2 = vvp_vector2_t(valb, सत्य);


        /* If we have an X or Z in the arguments return X. */
      यदि (xv2.is_NaN() || yv2.is_NaN()) {
	    vvp_vector4_t tmp (wid, BIT4_X);
	    thr->प्रक्षिप_vec4(tmp);
	    प्रत्यागम सत्य;
      }

	// Is the exponent negative? If so, table 5-6 in IEEE1364-2005
	// defines what value is returned.
      यदि (signed_flag && yv2.value(yv2.size()-1)) {
	    पूर्णाङ्क a_val;
	    vvp_bit4_t pad = BIT4_0, lsb = BIT4_0;
	    यदि (vector2_गन्तव्यम्_मानम्(xv2, a_val, सत्य)) {
		  यदि (a_val == 0) {
			pad = BIT4_X; lsb = BIT4_X;
		  }
		  यदि (a_val == 1) {
			pad = BIT4_0; lsb = BIT4_1;
		  }
		  यदि (a_val == -1) {
			यदि (yv2.value(0)) {
			      pad = BIT4_1; lsb = BIT4_1;
			} अन्यथा {
			      pad = BIT4_0; lsb = BIT4_1;
			}
		  }
	    }
	    vvp_vector4_t tmp (wid, pad);
	    tmp.समुच्चयः_bit(0, lsb);
	    thr->प्रक्षिप_vec4(tmp);
	    प्रत्यागम सत्य;
      }

      vvp_vector2_t परिणाम = घातः(xv2, yv2);

        /* Copy only what we need of the result. If the result is too
	   small, zero-pad it. */
      प्रति (अचिह्नित jdx = 0;  jdx < wid;  jdx += 1) {
	    यदि (jdx >= परिणाम.size())
		  vala.समुच्चयः_bit(jdx, BIT4_0);
	    अन्यथा
		  vala.समुच्चयः_bit(jdx, परिणाम.value(jdx) ? BIT4_1 : BIT4_0);
      }
      thr->प्रक्षिप_vec4(vala);

      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_POW(vthread_t thr, vvp_संहिता_t)
{
      प्रत्यागम सम्बन्धि_POW_आधारः(thr, असत्य);
}

तार्किक सम्बन्धि_POW_S(vthread_t thr, vvp_संहिता_t)
{
      प्रत्यागम सम्बन्धि_POW_आधारः(thr, सत्य);
}

तार्किक सम्बन्धि_POW_WR(vthread_t thr, vvp_संहिता_t)
{
      द्विगुण r = thr->निष्कासय_real();
      द्विगुण l = thr->निष्कासय_real();
      thr->प्रक्षिप_real(घातः(l,r));

      प्रत्यागम सत्य;
}

/*
 * %prop/obj <pid>, <idx>
 *
 * Load an object value from the cobject and push it onto the object stack.
 */
तार्किक सम्बन्धि_PROP_OBJ(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित pid = cp->सङ्ख्या;
      अचिह्नित idx = cp->bit_idx[0];

      यदि (idx != 0) {
	    प्रतिज्ञा(idx < vthread_s::शब्दाः_गणनम्);
	    idx = thr->शब्दाः[idx].w_uint;
      }

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();

      vvp_object_t val;
      cobj->get_object(pid, val, idx);

      thr->प्रक्षिप_object(val);

      प्रत्यागम सत्य;
}

स्थिर शून्य get_from_obj(अचिह्नित pid, vvp_cobject*cobj, द्विगुण&val)
{
      val = cobj->get_real(pid);
}

स्थिर शून्य get_from_obj(अचिह्नित pid, vvp_cobject*cobj, string&val)
{
      val = cobj->get_string(pid);
}

स्थिर शून्य get_from_obj(अचिह्नित pid, vvp_cobject*cobj, vvp_vector4_t&val)
{
      cobj->get_vec4(pid, val);
}

प्रतिरूपम् <प्रकारनाम ELEM>
स्थिर तार्किक prop(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित pid = cp->सङ्ख्या;

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      ELEM val;
      get_from_obj(pid, cobj, val);
      vthread_प्रक्षिप(thr, val);

      प्रत्यागम सत्य;
}

/*
 * %prop/r <pid>
 *
 * Load a real value from the cobject and push it onto the real value
 * stack.
 */
तार्किक सम्बन्धि_PROP_R(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम prop<द्विगुण>(thr, cp);
}

/*
 * %prop/str <pid>
 *
 * Load a string value from the cobject and push it onto the real value
 * stack.
 */
तार्किक सम्बन्धि_PROP_STR(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम prop<string>(thr, cp);
}

/*
 * %prop/v <pid>
 *
 * Load a property <pid> from the cobject on the top of the stack into
 * the vector space at <base>.
 */
तार्किक सम्बन्धि_PROP_V(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम prop<vvp_vector4_t>(thr, cp);
}

तार्किक सम्बन्धि_PUSHI_REAL(vthread_t thr, vvp_संहिता_t cp)
{
      द्विगुण mant = cp->bit_idx[0];
      uint32_t imant = cp->bit_idx[0];
      पूर्णाङ्क घातांकः = cp->bit_idx[1];

	// Detect +infinity
      यदि (घातांकः==0x3fff && imant==0) {
	    thr->प्रक्षिप_real(INFINITY);
	    प्रत्यागम सत्य;
      }
	// Detect -infinity
      यदि (घातांकः==0x7fff && imant==0) {
	    thr->प्रक्षिप_real(-INFINITY);
	    प्रत्यागम सत्य;
      }
	// Detect NaN
      यदि (घातांकः==0x3fff) {
	    thr->प्रक्षिप_real(nan(""));
	    प्रत्यागम सत्य;
      }

      द्विगुण sign = (घातांकः & 0x4000)? -1.0 : 1.0;

      घातांकः &= 0x1fff;

      mant = sign * ldexp(mant, घातांकः - 0x1000);
      thr->प्रक्षिप_real(mant);
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_PUSHI_STR(vthread_t thr, vvp_संहिता_t cp)
{
      ध्रुव वर्ण*text = cp->text;
      thr->प्रक्षिप_str(filter_string(text));
      प्रत्यागम सत्य;
}

/*
 * %pushi/vec4 <vala>, <valb>, <wid>
 */
तार्किक सम्बन्धि_PUSHI_VEC4(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित wid  = cp->सङ्ख्या;

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t val (wid, BIT4_0);
      get_तात्कालिकम्_rval (cp, val);

      thr->प्रक्षिप_vec4(val);

      प्रत्यागम सत्य;
}

/*
 * %pushv/str
 *   Pops a vec4 value, and pushes a string.
 */
तार्किक सम्बन्धि_PUSHV_STR(vthread_t thr, vvp_संहिता_t)
{
      vvp_vector4_t vec = thr->निष्कासय_vec4();

      size_t slen = (vec.size() + 7)/8;
      vector<वर्ण>buf;
      buf.reserve(slen);

      प्रति (size_t idx = 0 ; idx < vec.size() ; idx += 8) {
	    वर्ण tmp = 0;
	    size_t trans = 8;
	    यदि (idx+trans > vec.size())
		  trans = vec.size() - idx;

	    प्रति (size_t bdx = 0 ; bdx < trans ; bdx += 1) {
		  यदि (vec.value(idx+bdx) == BIT4_1)
			tmp |= 1 << bdx;
	    }

	    यदि (tmp != 0)
		  buf.push_back(tmp);
      }

      string val;
      प्रति (vector<वर्ण>::reverse_iterator cur = buf.rbegin()
		 ; cur != buf.rend() ; ++cur) {
	    val.push_back(*cur);
      }

      thr->प्रक्षिप_str(val);

      प्रत्यागम सत्य;
}

/*
 * %putc/str/vec4 <var>, <mux>
 */
तार्किक सम्बन्धि_PUTC_STR_VEC4(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित muxr = cp->bit_idx[0];
      int32_t mux = muxr? thr->शब्दाः[muxr].w_int : 0;

      vvp_vector4_t val = thr->निष्कासय_vec4();
      प्रतिज्ञा(val.size() == 8);

      यदि (mux < 0)
	    प्रत्यागम सत्य;

	/* Get the existing value of the string. If we find that the
	   index is too big for the string, then give up. */
      vvp_net_t*net = cp->net;
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (net->fun);
      प्रतिज्ञा(fun);

      string tmp = fun->get_string();
      यदि (tmp.size() <= (size_t)mux)
	    प्रत्यागम सत्य;

      वर्ण val_str = 0;
      प्रति (size_t idx = 0 ; idx < 8 ; idx += 1) {
	    यदि (val.value(idx)==BIT4_1)
		  val_str |= 1<<idx;
      }

	// It is a quirk of the Verilog standard that putc(..., 'h00)
	// has no effect. Test for that case here.
      यदि (val_str == 0)
	    प्रत्यागम सत्य;

      tmp[mux] = val_str;

      vvp_send_string(vvp_net_ptr_t(cp->net, 0), tmp, thr->wt_context);
      प्रत्यागम सत्य;
}

प्रतिरूपम् <प्रकारनाम ELEM, वर्ग QTYPE>
स्थिर तार्किक qinsert(vthread_t thr, vvp_संहिता_t cp, अचिह्नित wid=0)
{
      int64_t idx = thr->शब्दाः[3].w_int;
      ELEM value;
      vvp_net_t*net = cp->net;
      निष्कासय_मानम्(thr, value, wid); // Pop the value to store.

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      प्रतिज्ञा(queue);
      यदि (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at a negative "
	         << get_queue_type(value)
	         << " index (" << idx << "). ";
	    print_queue_मानम्(value);
	    cerr << " was not added." << पङ्क्त्यन्तः;
      } अन्यथा यदि (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at an undefined "
	         << get_queue_type(value) << " index. ";
	    print_queue_मानम्(value);
	    cerr << " was not added." << पङ्क्त्यन्तः;
      } अन्यथा {
	    अचिह्नित max_आकार = thr->शब्दाः[cp->bit_idx[0]].w_int;
	    queue->insert(idx, value, max_आकार);
      }
      प्रत्यागम सत्य;
}

/*
 * %qinsert/real <var-label>
 */
तार्किक सम्बन्धि_QINSERT_REAL(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम qinsert<द्विगुण, vvp_queue_real>(thr, cp);
}

/*
 * %qinsert/str <var-label>
 */
तार्किक सम्बन्धि_QINSERT_STR(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम qinsert<string, vvp_queue_string>(thr, cp);
}

/*
 * %qinsert/v <var-label>
 */
तार्किक सम्बन्धि_QINSERT_V(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम qinsert<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

/*
 * Helper functions used in the queue pop templates
 */
अन्तःस्थ शून्य प्रक्षिप_मानम्(vthread_t thr, द्विगुण value, अचिह्नित)
{
      thr->प्रक्षिप_real(value);
}

अन्तःस्थ शून्य प्रक्षिप_मानम्(vthread_t thr, ध्रुव string&value, अचिह्नित)
{
      thr->प्रक्षिप_str(value);
}

अन्तःस्थ शून्य प्रक्षिप_मानम्(vthread_t thr, ध्रुव vvp_vector4_t&value, अचिह्नित wid)
{
      प्रतिज्ञा(wid == value.size());
      thr->प्रक्षिप_vec4(value);
}

प्रतिरूपम् <प्रकारनाम ELEM, वर्ग QTYPE>
स्थिर तार्किक q_निष्कासय(vthread_t thr, vvp_संहिता_t cp,
                  शून्य (*get_val_func)(vvp_queue*, ELEM&),
                  ध्रुव वर्ण*loc, अचिह्नित wid)
{
      vvp_net_t*net = cp->net;

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      प्रतिज्ञा(queue);

      size_t size = queue->get_आकार();

      ELEM value;
      यदि (size) {
	    get_val_func(queue, value);
      } अन्यथा {
	    dq_default(value, wid);
	    cerr << thr->get_fileline()
	         << "Warning: pop_" << loc << "() on empty "
	         << get_queue_type(value) << "." << पङ्क्त्यन्तः;
      }

      प्रक्षिप_मानम्(thr, value, wid);
      प्रत्यागम सत्य;
}

प्रतिरूपम् <प्रकारनाम ELEM>
स्थिर शून्य get_पृष्ठभागः_मानम्(vvp_queue*queue, ELEM&value)
{
      queue->get_word(queue->get_आकार()-1, value);
      queue->pop_back();
}

प्रतिरूपम् <प्रकारनाम ELEM, वर्ग QTYPE>
स्थिर तार्किक qpop_b(vthread_t thr, vvp_संहिता_t cp, अचिह्नित wid=0)
{
      प्रत्यागम q_निष्कासय<ELEM, QTYPE>(thr, cp, get_पृष्ठभागः_मानम्<ELEM>, "back", wid);
}

/*
 * %qpop/b/real <var-label>
 */
तार्किक सम्बन्धि_QPOP_B_REAL(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम qpop_b<द्विगुण, vvp_queue_real>(thr, cp);
}

/*
 * %qpop/b/str <var-label>
 */
तार्किक सम्बन्धि_QPOP_B_STR(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम qpop_b<string, vvp_queue_string>(thr, cp);
}

/*
 * %qpop/b/v <var-label>
 */
तार्किक सम्बन्धि_QPOP_B_V(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम qpop_b<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[0]);
}

प्रतिरूपम् <प्रकारनाम ELEM>
स्थिर शून्य get_front_मानम्(vvp_queue*queue, ELEM&value)
{
      queue->get_word(0, value);
      queue->pop_front();
}

प्रतिरूपम् <प्रकारनाम ELEM, वर्ग QTYPE>
स्थिर तार्किक qpop_f(vthread_t thr, vvp_संहिता_t cp, अचिह्नित wid=0)
{
      प्रत्यागम q_निष्कासय<ELEM, QTYPE>(thr, cp, get_front_मानम्<ELEM>, "front", wid);
}


/*
 * %qpop/f/real <var-label>
 */
तार्किक सम्बन्धि_QPOP_F_REAL(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम qpop_f<द्विगुण, vvp_queue_real>(thr, cp);
}

/*
 * %qpop/f/str <var-label>
 */
तार्किक सम्बन्धि_QPOP_F_STR(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम qpop_f<string, vvp_queue_string>(thr, cp);
}

/*
 * %qpop/f/v <var-label>
 */
तार्किक सम्बन्धि_QPOP_F_V(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम qpop_f<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[0]);
}

/*
 * These implement the %release/net and %release/reg instructions. The
 * %release/net instruction applies to a net kind of functor by
 * sending the release/net command to the command port. (See vvp_net.h
 * for details.) The %release/reg instruction is the same, but sends
 * the release/reg command instead. These are very similar to the
 * %deassign instruction.
 */
स्थिर तार्किक do_release_vec(vvp_संहिता_t cp, तार्किक net_flag)
{
      vvp_net_t*net = cp->net;
      अचिह्नित आधारः  = cp->bit_idx[0];
      अचिह्नित width = cp->bit_idx[1];

      प्रतिज्ञा(net->fil);

      यदि (आधारः >= net->fil->filter_आकार()) प्रत्यागम सत्य;
      यदि (आधारः+width > net->fil->filter_आकार())
	    width = net->fil->filter_आकार() - आधारः;

      तार्किक पूर्णम्_sig = आधारः == 0 && width == net->fil->filter_आकार();

	// XXXX Can't really do this if this is a partial release?
      net->fil->force_unlink();

	/* Do we release all or part of the net? */
      vvp_net_ptr_t ptr (net, 0);
      यदि (पूर्णम्_sig) {
	    net->fil->release(ptr, net_flag);
      } अन्यथा {
	    net->fil->release_pv(ptr, आधारः, width, net_flag);
      }
      net->fun->force_flag(असत्य);

      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_RELEASE_NET(vthread_t, vvp_संहिता_t cp)
{
      प्रत्यागम do_release_vec(cp, सत्य);
}


तार्किक सम्बन्धि_RELEASE_REG(vthread_t, vvp_संहिता_t cp)
{
      प्रत्यागम do_release_vec(cp, असत्य);
}

/* The type is 1 for registers and 0 for everything else. */
तार्किक सम्बन्धि_RELEASE_WR(vthread_t, vvp_संहिता_t cp)
{
      vvp_net_t*net = cp->net;
      अचिह्नित type  = cp->bit_idx[0];

      प्रतिज्ञा(net->fil);
      net->fil->force_unlink();

	// Send a command to this signal to unforce itself.
      vvp_net_ptr_t ptr (net, 0);
      net->fil->release(ptr, type==0);
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_REPLICATE(vthread_t thr, vvp_संहिता_t cp)
{
      पूर्णाङ्क rept = cp->सङ्ख्या;
      vvp_vector4_t val = thr->निष्कासय_vec4();
      vvp_vector4_t res (val.size() * rept, BIT4_X);

      प्रति (पूर्णाङ्क idx = 0 ; idx < rept ; idx += 1) {
	    res.समुच्चयः_vec(idx * val.size(), val);
      }

      thr->प्रक्षिप_vec4(res);

      प्रत्यागम सत्य;
}

स्थिर शून्य poke_val(vthread_t fun_thr, अचिह्नित गभीरता, द्विगुण val)
{
      fun_thr->parent->poke_real(गभीरता, val);
}

स्थिर शून्य poke_val(vthread_t fun_thr, अचिह्नित गभीरता, ध्रुव string&val)
{
      fun_thr->parent->poke_str(गभीरता, val);
}

स्थिर size_t get_max(vthread_t fun_thr, द्विगुण&)
{
      प्रत्यागम fun_thr->तर्काः_real.size();
}

स्थिर size_t get_max(vthread_t fun_thr, string&)
{
      प्रत्यागम fun_thr->तर्काः_str.size();
}

स्थिर size_t get_max(vthread_t fun_thr, vvp_vector4_t&)
{
      प्रत्यागम fun_thr->तर्काः_vec4.size();
}

स्थिर अचिह्नित get_गभीरता(vthread_t fun_thr, size_t सूचकांक, द्विगुण&)
{
      प्रत्यागम fun_thr->तर्काः_real[सूचकांक];
}

स्थिर अचिह्नित get_गभीरता(vthread_t fun_thr, size_t सूचकांक, string&)
{
      प्रत्यागम fun_thr->तर्काः_str[सूचकांक];
}

स्थिर अचिह्नित get_गभीरता(vthread_t fun_thr, size_t सूचकांक, vvp_vector4_t&)
{
      प्रत्यागम fun_thr->तर्काः_vec4[सूचकांक];
}

स्थिर vthread_t get_func(vthread_t thr)
{
      vthread_t fun_thr = thr;

      यावत् (fun_thr->parent_scope->get_type_संहिता() != vpiFunction) {
	    प्रतिज्ञा(fun_thr->parent);
	    fun_thr = fun_thr->parent;
      }

      प्रत्यागम fun_thr;
}

प्रतिरूपम् <प्रकारनाम ELEM>
स्थिर तार्किक ret(vthread_t thr, vvp_संहिता_t cp)
{
      size_t सूचकांक = cp->सङ्ख्या;
      ELEM val;
      निष्कासय_मानम्(thr, val, 0);

      vthread_t fun_thr = get_func(thr);
      प्रतिज्ञा(सूचकांक < get_max(fun_thr, val));

      अचिह्नित गभीरता = get_गभीरता(fun_thr, सूचकांक, val);
	// Use the depth to put the value into the stack of
	// the parent thread.
      poke_val(fun_thr, गभीरता, val);
      प्रत्यागम सत्य;
}

/*
 * %ret/real <index>
 */
तार्किक सम्बन्धि_RET_REAL(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम ret<द्विगुण>(thr, cp);
}

/*
 * %ret/str <index>
 */
तार्किक सम्बन्धि_RET_STR(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम ret<string>(thr, cp);
}

/*
 * %ret/vec4 <index>, <offset>, <wid>
 */
तार्किक सम्बन्धि_RET_VEC4(vthread_t thr, vvp_संहिता_t cp)
{
      size_t सूचकांक = cp->सङ्ख्या;
      अचिह्नित off_सूचकांक = cp->bit_idx[0];
      अचिह्नित पूर्णाङ्क wid = cp->bit_idx[1];
      vvp_vector4_t&val = thr->peek_vec4();

      vthread_t fun_thr = get_func(thr);
      प्रतिज्ञा(सूचकांक < get_max(fun_thr, val));
      प्रतिज्ञा(val.size() == wid);
      अचिह्नित गभीरता = get_गभीरता(fun_thr, सूचकांक, val);

      int64_t off = off_सूचकांक ? thr->शब्दाः[off_सूचकांक].w_int : 0;
      अचिह्नित पूर्णाङ्क sig_मानम्_आकार = fun_thr->parent->peek_vec4(गभीरता).size();

      यदि (off_सूचकांक!=0 && thr->flags[4] == BIT4_1) {
	    thr->निष्कासय_vec4(1);
	    प्रत्यागम सत्य;
      }

      यदि (!resize_rval_vec(val, off, sig_मानम्_आकार)) {
	    thr->निष्कासय_vec4(1);
	    प्रत्यागम सत्य;
      }

      यदि (off == 0 && val.size() == sig_मानम्_आकार) {
	    fun_thr->parent->poke_vec4(गभीरता, val);
      } अन्यथा {
	    vvp_vector4_t tmp_dst = fun_thr->parent->peek_vec4(गभीरता);
	    tmp_dst.समुच्चयः_vec(off, val);
	    fun_thr->parent->poke_vec4(गभीरता, tmp_dst);
      }

      thr->निष्कासय_vec4(1);
      प्रत्यागम सत्य;
}

स्थिर शून्य प्रक्षिप_from_parent(vthread_t thr, vthread_t fun_thr, अचिह्नित गभीरता, द्विगुण&)
{
      thr->प्रक्षिप_real(fun_thr->parent->peek_real(गभीरता));
}

स्थिर शून्य प्रक्षिप_from_parent(vthread_t thr, vthread_t fun_thr, अचिह्नित गभीरता, string&)
{
      thr->प्रक्षिप_str(fun_thr->parent->peek_str(गभीरता));
}

स्थिर शून्य प्रक्षिप_from_parent(vthread_t thr, vthread_t fun_thr, अचिह्नित गभीरता, vvp_vector4_t&)
{
      thr->प्रक्षिप_vec4(fun_thr->parent->peek_vec4(गभीरता));
}

प्रतिरूपम् <प्रकारनाम ELEM>
स्थिर तार्किक retload(vthread_t thr, vvp_संहिता_t cp)
{
      size_t सूचकांक = cp->सङ्ख्या;
      ELEM type;

      vthread_t fun_thr = get_func(thr);
      प्रतिज्ञा(सूचकांक < get_max(fun_thr, type));

      अचिह्नित गभीरता = get_गभीरता(fun_thr, सूचकांक, type);
	// Use the depth to extract the values from the stack
	// of the parent thread.
      प्रक्षिप_from_parent(thr, fun_thr, गभीरता, type);
      प्रत्यागम सत्य;
}

/*
 * %retload/real <index>
 */
तार्किक सम्बन्धि_RETLOAD_REAL(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम retload<द्विगुण>(thr, cp);
}

/*
 * %retload/str <index>
 */
तार्किक सम्बन्धि_RETLOAD_STR(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम retload<string>(thr, cp);
}

/*
 * %retload/vec4 <index>
 */
तार्किक सम्बन्धि_RETLOAD_VEC4(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम retload<vvp_vector4_t>(thr, cp);
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
तार्किक सम्बन्धि_SCOPY(vthread_t thr, vvp_संहिता_t)
{
      vvp_object_t tmp;
      thr->निष्कासय_object(tmp);

      vvp_object_t&dest = thr->peek_object();
        // If it is null there is nothing to copy
      यदि (!tmp.test_nil())
	    dest.shallow_copy(tmp);

      प्रत्यागम सत्य;
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

प्रतिरूपम् <प्रकारनाम ELEM>
स्थिर तार्किक समुच्चयः_dar_obj(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित adr = thr->शब्दाः[cp->सङ्ख्या].w_int;

      ELEM value;
      thread_peek(thr, value);

      vvp_object_t&शिखरम् = thr->peek_object();
      vvp_darray*darray = शिखरम्.peek<vvp_darray>();
      प्रतिज्ञा(darray);

      darray->समुच्चयः_word(adr, value);
      प्रत्यागम सत्य;
}

/*
 * %set/dar/obj/real <index>
 */
तार्किक सम्बन्धि_समुच्चयः_DAR_OBJ_REAL(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम समुच्चयः_dar_obj<द्विगुण>(thr, cp);
}

/*
 * %set/dar/obj/str <index>
 */
तार्किक सम्बन्धि_समुच्चयः_DAR_OBJ_STR(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम समुच्चयः_dar_obj<string>(thr, cp);
}

/*
 * %set/dar/obj/vec4 <index>
 */
तार्किक सम्बन्धि_समुच्चयः_DAR_OBJ_VEC4(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम समुच्चयः_dar_obj<vvp_vector4_t>(thr, cp);
}

/*
 * %shiftl <idx>
 *
 * Pop the operand, then push the result.
 */
तार्किक सम्बन्धि_SHIFTL(vthread_t thr, vvp_संहिता_t cp)
{
      पूर्णाङ्क use_सूचकांक = cp->सङ्ख्या;
      uint64_t सरणम् = thr->शब्दाः[use_सूचकांक].w_uint;

      vvp_vector4_t&val = thr->peek_vec4();
      अचिह्नित wid  = val.size();

      यदि (thr->flags[4] == BIT4_1) {
	      // The result is 'bx if the shift amount is undefined
	    val = vvp_vector4_t(wid, BIT4_X);

      } अन्यथा यदि (thr->flags[4] == BIT4_X || सरणम् >= wid) {
	      // Shift is so big that all value is shifted out. Write
	      // a constant 0 result.
	    val = vvp_vector4_t(wid, BIT4_0);

      } अन्यथा यदि (सरणम् > 0) {
	    vvp_vector4_t blk = val.subvalue(0, wid-सरणम्);
	    vvp_vector4_t tmp (सरणम्, BIT4_0);
	    val.समुच्चयः_vec(0, tmp);
	    val.समुच्चयः_vec(सरणम्, blk);
      }

      प्रत्यागम सत्य;
}

/*
 * %shiftr <idx>
 * This is an unsigned right shift. The <idx> is a number that selects
 * the index register with the amount of the shift. This instruction
 * checks flag bit 4, which will be true if the shift is invalid.
 */
तार्किक सम्बन्धि_SHIFTR(vthread_t thr, vvp_संहिता_t cp)
{
      पूर्णाङ्क use_सूचकांक = cp->सङ्ख्या;
      uint64_t सरणम् = thr->शब्दाः[use_सूचकांक].w_uint;

      vvp_vector4_t val = thr->निष्कासय_vec4();
      अचिह्नित wid  = val.size();

      यदि (thr->flags[4] == BIT4_1) {
	    val = vvp_vector4_t(wid, BIT4_X);

      } अन्यथा यदि (thr->flags[4] == BIT4_X || सरणम् > wid) {
	    val = vvp_vector4_t(wid, BIT4_0);

      } अन्यथा यदि (सरणम् > 0) {
	    vvp_vector4_t blk = val.subvalue(सरणम्, wid-सरणम्);
	    vvp_vector4_t tmp (सरणम्, BIT4_0);
	    val.समुच्चयः_vec(0, blk);
	    val.समुच्चयः_vec(wid-सरणम्, tmp);
      }

      thr->प्रक्षिप_vec4(val);
      प्रत्यागम सत्य;
}

/*
 *  %shiftr/s <wid>
 */
तार्किक सम्बन्धि_SHIFTR_S(vthread_t thr, vvp_संहिता_t cp)
{
      पूर्णाङ्क use_सूचकांक = cp->सङ्ख्या;
      uint64_t सरणम् = thr->शब्दाः[use_सूचकांक].w_uint;

      vvp_vector4_t val = thr->निष्कासय_vec4();
      अचिह्नित wid  = val.size();

      vvp_bit4_t sign_bit = val.value(val.size()-1);

      यदि (thr->flags[4] == BIT4_1) {
	    val = vvp_vector4_t(wid, BIT4_X);

      } अन्यथा यदि (thr->flags[4] == BIT4_X || सरणम् > wid) {
	    val = vvp_vector4_t(wid, sign_bit);

      } अन्यथा यदि (सरणम् > 0) {
	    vvp_vector4_t blk = val.subvalue(सरणम्, wid-सरणम्);
	    vvp_vector4_t tmp (सरणम्, sign_bit);
	    val.समुच्चयः_vec(0, blk);
	    val.समुच्चयः_vec(wid-सरणम्, tmp);
      }

      thr->प्रक्षिप_vec4(val);
      प्रत्यागम सत्य;
}

/*
 * %split/vec4 <wid>
 *   Pop 1 value,
 *   Take <wid> bits from the lsb,
 *   Push the remaining msb,
 *   Push the lsb.
 */
तार्किक सम्बन्धि_विभज_VEC4(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित lsb_wid = cp->सङ्ख्या;

      vvp_vector4_t&val = thr->peek_vec4();
      प्रतिज्ञा(lsb_wid < val.size());

      vvp_vector4_t lsb = val.subvalue(0, lsb_wid);
      val = val.subvalue(lsb_wid, val.size()-lsb_wid);

      thr->प्रक्षिप_vec4(lsb);
      प्रत्यागम सत्य;
}

/*
 * The following are used to allow the darray templates to print correctly.
 */
अन्तःस्थ स्थिर string get_darray_type(द्विगुण&)
{
      प्रत्यागम "darray<real>";
}

अन्तःस्थ स्थिर string get_darray_type(string&)
{
      प्रत्यागम "darray<string>";
}

अन्तःस्थ स्थिर string get_darray_type(ध्रुव vvp_vector4_t&value)
{
      ostringstream buf;
      buf << "darray<vector[" << value.size() << "]>";
      string res = buf.str();
      प्रत्यागम res;
}

/*
 * The following are used to allow a common template to be written for
 * darray real/string/vec4 operations
 */
अन्तःस्थ स्थिर शून्य dar_निष्कासय_मानम्(vthread_t thr, द्विगुण&value)
{
      value = thr->निष्कासय_real();
}

अन्तःस्थ स्थिर शून्य dar_निष्कासय_मानम्(vthread_t thr, string&value)
{
      value = thr->निष्कासय_str();
}

अन्तःस्थ स्थिर शून्य dar_निष्कासय_मानम्(vthread_t thr, vvp_vector4_t&value)
{
      value = thr->निष्कासय_vec4();
}

प्रतिरूपम् <प्रकारनाम ELEM>
स्थिर तार्किक store_dar(vthread_t thr, vvp_संहिता_t cp)
{
      int64_t adr = thr->शब्दाः[3].w_int;
      ELEM value;
	// FIXME: Can we get the size of the underlying array element
	//        and then use the normal pop_value?
      dar_निष्कासय_मानम्(thr, value);

      vvp_net_t*net = cp->net;
      प्रतिज्ञा(net);

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      प्रतिज्ञा(obj);

      vvp_darray*darray = obj->get_object().peek<vvp_darray>();

      यदि (adr < 0)
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to a negative " << get_darray_type(value)
	         << " index (" << adr << ")." << पङ्क्त्यन्तः;
      अन्यथा यदि (thr->flags[4] != BIT4_0)
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to an undefined " << get_darray_type(value)
	         << " index." << पङ्क्त्यन्तः;
      अन्यथा यदि (darray)
	    darray->समुच्चयः_word(adr, value);
      अन्यथा
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to an undefined " << get_darray_type(value)
	         << "." << पङ्क्त्यन्तः;

      प्रत्यागम सत्य;
}

/*
 * %store/dar/real <var>
 */
तार्किक सम्बन्धि_STORE_DAR_R(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम store_dar<द्विगुण>(thr, cp);
}

/*
 * %store/dar/str <var>
 */
तार्किक सम्बन्धि_STORE_DAR_STR(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम store_dar<string>(thr, cp);
}

/*
 * %store/dar/vec4 <var>
 */
तार्किक सम्बन्धि_STORE_DAR_VEC4(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम store_dar<vvp_vector4_t>(thr, cp);
}

तार्किक सम्बन्धि_STORE_OBJ(vthread_t thr, vvp_संहिता_t cp)
{
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);

      vvp_object_t val;
      thr->निष्कासय_object(val);

      vvp_send_object(ptr, val, thr->wt_context);

      प्रत्यागम सत्य;
}

/*
 * %store/obja <array-label> <index>
 */
तार्किक सम्बन्धि_STORE_OBJA(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित idx = cp->bit_idx[0];
      अचिह्नित adr = thr->शब्दाः[idx].w_int;

      vvp_object_t val;
      thr->निष्कासय_object(val);

      cp->array->समुच्चयः_word(adr, val);

      प्रत्यागम सत्य;
}


/*
 * %store/prop/obj <pid>, <idx>
 *
 * Pop an object value from the object stack, and store the value into
 * the property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
तार्किक सम्बन्धि_STORE_PROP_OBJ(vthread_t thr, vvp_संहिता_t cp)
{
      size_t pid = cp->सङ्ख्या;
      अचिह्नित idx = cp->bit_idx[0];

      यदि (idx != 0) {
	    प्रतिज्ञा(idx < vthread_s::शब्दाः_गणनम्);
	    idx = thr->शब्दाः[idx].w_uint;
      }

      vvp_object_t val;
      thr->निष्कासय_object(val);

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      cobj->समुच्चयः_object(pid, val, idx);

      प्रत्यागम सत्य;
}

स्थिर शून्य निष्कासय_prop_val(vthread_t thr, द्विगुण&val, अचिह्नित)
{
      val = thr->निष्कासय_real();
}

स्थिर शून्य निष्कासय_prop_val(vthread_t thr, string&val, अचिह्नित)
{
      val = thr->निष्कासय_str();
}

स्थिर शून्य निष्कासय_prop_val(vthread_t thr, vvp_vector4_t&val, अचिह्नित wid)
{
      val = thr->निष्कासय_vec4();
      प्रतिज्ञा(val.size() >= wid);
      val.resize(wid);
}

स्थिर शून्य समुच्चयः_val(vvp_cobject*cobj, size_t pid, ध्रुव द्विगुण&val)
{
      cobj->समुच्चयः_real(pid, val);
}

स्थिर शून्य समुच्चयः_val(vvp_cobject*cobj, size_t pid, ध्रुव string&val)
{
      cobj->समुच्चयः_string(pid, val);
}

स्थिर शून्य समुच्चयः_val(vvp_cobject*cobj, size_t pid, ध्रुव vvp_vector4_t&val)
{
      cobj->समुच्चयः_vec4(pid, val);
}

प्रतिरूपम् <प्रकारनाम ELEM>
स्थिर तार्किक store_prop(vthread_t thr, vvp_संहिता_t cp, अचिह्नित wid=0)
{
      size_t pid = cp->सङ्ख्या;
      ELEM val;
      निष्कासय_prop_val(thr, val, wid); // Pop the value to store.

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      समुच्चयः_val(cobj, pid, val);

      प्रत्यागम सत्य;
}

/*
 * %store/prop/r <id>
 *
 * Pop a real value from the real stack, and store the value into the
 * property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
तार्किक सम्बन्धि_STORE_PROP_R(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम store_prop<द्विगुण>(thr, cp);
}

/*
 * %store/prop/str <id>
 *
 * Pop a string value from the string stack, and store the value into
 * the property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
तार्किक सम्बन्धि_STORE_PROP_STR(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम store_prop<string>(thr, cp);
}

/*
 * %store/prop/v <pid>, <wid>
 *
 * Store vector value into property <id> of cobject in the top of the
 * stack. Do NOT pop the object stack.
 */
तार्किक सम्बन्धि_STORE_PROP_V(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम store_prop<vvp_vector4_t>(thr, cp, cp->bit_idx[0]);
}

प्रतिरूपम् <वर्ग QTYPE>
स्थिर QTYPE* get_queue_prop(vvp_cobject*cobj, size_t pid)
{
      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      QTYPE* queue = qobj.peek<QTYPE>();
      यदि (queue == 0) {
	    queue = नूतन QTYPE;
	    vvp_object_t val(queue);
	    cobj->समुच्चयः_object(pid, val, 0);
      }
      प्रत्यागम queue;
}

प्रतिरूपम् <प्रकारनाम ELEM, वर्ग QTYPE>
स्थिर तार्किक store_prop_qb(vthread_t thr, vvp_संहिता_t cp, अचिह्नित wid)
{
      size_t pid = cp->सङ्ख्या;
      ELEM value;
      अचिह्नित max_आकार = thr->शब्दाः[cp->bit_idx[0]].w_uint;
      निष्कासय_मानम्(thr, value, wid);

      vvp_object_t& शिखरम् = thr->peek_object();
      vvp_cobject*cobj = शिखरम्.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      QTYPE* queue = get_queue_prop<QTYPE>(cobj, pid);
      प्रतिज्ञा(queue);
      queue->push_back(value, max_आकार);
      प्रत्यागम सत्य;
}

प्रतिरूपम् <प्रकारनाम ELEM, वर्ग QTYPE>
स्थिर तार्किक store_prop_qf(vthread_t thr, vvp_संहिता_t cp, अचिह्नित wid)
{
      size_t pid = cp->सङ्ख्या;
      ELEM value;
      अचिह्नित max_आकार = thr->शब्दाः[cp->bit_idx[0]].w_uint;
      निष्कासय_मानम्(thr, value, wid);

      vvp_object_t& शिखरम् = thr->peek_object();
      vvp_cobject*cobj = शिखरम्.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      QTYPE* queue = get_queue_prop<QTYPE>(cobj, pid);
      प्रतिज्ञा(queue);
      queue->push_front(value, max_आकार);
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_STORE_PROP_QB_R(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम store_prop_qb<द्विगुण, vvp_queue_real>(thr, cp, 0);
}

तार्किक सम्बन्धि_STORE_PROP_QB_STR(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम store_prop_qb<string, vvp_queue_string>(thr, cp, 0);
}

तार्किक सम्बन्धि_STORE_PROP_QB_V(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम store_prop_qb<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

तार्किक सम्बन्धि_STORE_PROP_QF_R(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम store_prop_qf<द्विगुण, vvp_queue_real>(thr, cp, 0);
}

तार्किक सम्बन्धि_STORE_PROP_QF_STR(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम store_prop_qf<string, vvp_queue_string>(thr, cp, 0);
}

तार्किक सम्बन्धि_STORE_PROP_QF_V(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम store_prop_qf<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

प्रतिरूपम् <प्रकारनाम ELEM, वर्ग QTYPE>
स्थिर तार्किक qinsert_prop(vthread_t thr, vvp_संहिता_t cp, अचिह्नित wid)
{
      int64_t idx = thr->शब्दाः[3].w_int;
      ELEM value;
      size_t pid = cp->सङ्ख्या;
      निष्कासय_मानम्(thr, value, wid);

      vvp_object_t& शिखरम् = thr->peek_object();
      vvp_cobject*cobj = शिखरम्.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      QTYPE* queue = get_queue_prop<QTYPE>(cobj, pid);
      प्रतिज्ञा(queue);
      यदि (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at a negative "
	         << get_queue_type(value)
	         << " index (" << idx << "). ";
	    print_queue_मानम्(value);
	    cerr << " was not added." << पङ्क्त्यन्तः;
      } अन्यथा यदि (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at an undefined "
	         << get_queue_type(value) << " index. ";
	    print_queue_मानम्(value);
	    cerr << " was not added." << पङ्क्त्यन्तः;
      } अन्यथा {
	    अचिह्नित max_आकार = thr->शब्दाः[cp->bit_idx[0]].w_int;
	    queue->insert(idx, value, max_आकार);
      }
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_QINSERT_PROP_R(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम qinsert_prop<द्विगुण, vvp_queue_real>(thr, cp, 0);
}

तार्किक सम्बन्धि_QINSERT_PROP_STR(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम qinsert_prop<string, vvp_queue_string>(thr, cp, 0);
}

तार्किक सम्बन्धि_QINSERT_PROP_V(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम qinsert_prop<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

प्रतिरूपम् <प्रकारनाम ELEM, वर्ग QTYPE>
स्थिर तार्किक q_निष्कासय_prop(vthread_t thr, vvp_संहिता_t cp,
                       शून्य (*get_val_func)(vvp_queue*, ELEM&),
                       ध्रुव वर्ण*loc, अचिह्नित wid)
{
      size_t pid = cp->सङ्ख्या;

      vvp_object_t& शिखरम् = thr->peek_object();
      vvp_cobject*cobj = शिखरम्.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      QTYPE* queue = get_queue_prop<QTYPE>(cobj, pid);
      प्रतिज्ञा(queue);

      size_t size = queue->get_आकार();

      ELEM value;
      यदि (size) {
	    get_val_func(queue, value);
      } अन्यथा {
	    dq_default(value, wid);
	    cerr << thr->get_fileline()
	         << "Warning: pop_" << loc << "() on empty "
	         << get_queue_type(value) << "." << पङ्क्त्यन्तः;
      }

      प्रक्षिप_मानम्(thr, value, wid);
      प्रत्यागम सत्य;
}

प्रतिरूपम् <प्रकारनाम ELEM, वर्ग QTYPE>
स्थिर तार्किक qpop_b_prop(vthread_t thr, vvp_संहिता_t cp, अचिह्नित wid)
{
      प्रत्यागम q_निष्कासय_prop<ELEM, QTYPE>(thr, cp, get_पृष्ठभागः_मानम्<ELEM>, "back", wid);
}

प्रतिरूपम् <प्रकारनाम ELEM, वर्ग QTYPE>
स्थिर तार्किक qpop_f_prop(vthread_t thr, vvp_संहिता_t cp, अचिह्नित wid)
{
      प्रत्यागम q_निष्कासय_prop<ELEM, QTYPE>(thr, cp, get_front_मानम्<ELEM>, "front", wid);
}

तार्किक सम्बन्धि_QPOP_PROP_B_REAL(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम qpop_b_prop<द्विगुण, vvp_queue_real>(thr, cp, 0);
}

तार्किक सम्बन्धि_QPOP_PROP_B_STR(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम qpop_b_prop<string, vvp_queue_string>(thr, cp, 0);
}

तार्किक सम्बन्धि_QPOP_PROP_B_V(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम qpop_b_prop<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[0]);
}

तार्किक सम्बन्धि_QPOP_PROP_F_REAL(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम qpop_f_prop<द्विगुण, vvp_queue_real>(thr, cp, 0);
}

तार्किक सम्बन्धि_QPOP_PROP_F_STR(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम qpop_f_prop<string, vvp_queue_string>(thr, cp, 0);
}

तार्किक सम्बन्धि_QPOP_PROP_F_V(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम qpop_f_prop<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[0]);
}

स्थिर vvp_vector4_t queue_unique_ulong_गन्तव्यम्_vec4(अचिह्नित दीर्घ ul, अचिह्नित द्वयङ्काः)
{
      vvp_vector4_t val(द्वयङ्काः, BIT4_0);
      प्रति (अचिह्नित idx = 0; idx < द्वयङ्काः && idx < 8 * परिमाण(अचिह्नित दीर्घ); idx += 1) {
	    यदि (ul & 1UL) {
		  val.समुच्चयः_bit(idx, BIT4_1);
	    } अन्यथा {
		  val.समुच्चयः_bit(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      प्रत्यागम val;
}

प्रतिरूपम् <वर्ग SRC> स्थिर vvp_queue_vec4* queue_सततम्_unique_src(SRC* src,
								 अचिह्नित wid,
								 तार्किक as_सूचकांक)
{
      vvp_queue_vec4* dst = नूतन vvp_queue_vec4();
      यदि (!src) प्रत्यागम dst;
      size_t n = src->get_आकार();
      प्रति (size_t i = 0; i < n; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    तार्किक आलोकितम् = असत्य;
	    प्रति (size_t j = 0; j < i; j += 1) {
		  vvp_vector4_t vj(wid);
		  src->get_word(j, vj);
		  यदि (vi.eeq(vj)) {
			आलोकितम् = सत्य;
			भङ्ग;
		  }
	    }
	    यदि (आलोकितम्) अनुवर्त;
	    यदि (as_सूचकांक) {
		  dst->push_back(queue_unique_ulong_गन्तव्यम्_vec4((अचिह्नित दीर्घ)i, 32), 0);
	    } अन्यथा {
		  dst->push_back(vi, 0);
	    }
      }
      प्रत्यागम dst;
}

प्रतिरूपम् <वर्ग SRC>
स्थिर vvp_queue_vec4* queue_सततम्_find_matches_src(SRC* src, अचिह्नित wid,
						  ध्रुव vvp_vector4_t& cmp,
						  तार्किक as_सूचकांक)
{
      vvp_queue_vec4* dst = नूतन vvp_queue_vec4();
      यदि (!src) प्रत्यागम dst;
      size_t n = src->get_आकार();
      प्रति (size_t i = 0; i < n; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    यदि (vi.eeq(cmp)) {
		  यदि (as_सूचकांक) {
			dst->push_back(queue_unique_ulong_गन्तव्यम्_vec4((अचिह्नित दीर्घ)i, 32), 0);
		  } अन्यथा {
			dst->push_back(vi, 0);
		  }
	    }
      }
      प्रत्यागम dst;
}

/* Array/queue locator methods return a queue with 0 or 1 elements (LRM). */
प्रतिरूपम् <वर्ग SRC>
स्थिर vvp_queue_vec4* queue_find_first_चरमः_गन्तव्यम्_queue_src(SRC* src,
							  अचिह्नित wid,
							  ध्रुव vvp_vector4_t& cmp,
							  तार्किक want_first,
							  तार्किक want_सूचकांक)
{
      vvp_queue_vec4* dst = नूतन vvp_queue_vec4();
      यदि (!src || src->get_आकार() == 0) प्रत्यागम dst;
      size_t n = src->get_आकार();
      यदि (want_first) {
	    प्रति (size_t i = 0; i < n; i += 1) {
		  vvp_vector4_t vi(wid);
		  src->get_word(i, vi);
		  यदि (vi.eeq(cmp)) {
			यदि (want_सूचकांक) {
			      dst->push_back(
				  queue_unique_ulong_गन्तव्यम्_vec4((अचिह्नित दीर्घ)i, 32),
				  0);
			} अन्यथा {
			      dst->push_back(vi, 0);
			}
			प्रत्यागम dst;
		  }
	    }
      } अन्यथा {
	    प्रति (size_t i = n; i > 0; ) {
		  i -= 1;
		  vvp_vector4_t vi(wid);
		  src->get_word(i, vi);
		  यदि (vi.eeq(cmp)) {
			यदि (want_सूचकांक) {
			      dst->push_back(
				  queue_unique_ulong_गन्तव्यम्_vec4((अचिह्नित दीर्घ)i, 32),
				  0);
			} अन्यथा {
			      dst->push_back(vi, 0);
			}
			प्रत्यागम dst;
		  }
	    }
      }
      प्रत्यागम dst;
}

/* min()/max() return one element with the selected extremum. */
प्रतिरूपम् <वर्ग SRC>
स्थिर vvp_queue_vec4* queue_सततम्_min_max_src(SRC* src, अचिह्नित wid, तार्किक want_max)
{
      vvp_queue_vec4* dst = नूतन vvp_queue_vec4();
      यदि (!src || src->get_आकार() == 0) प्रत्यागम dst;

      vvp_vector4_t best(wid);
      src->get_word(0, best);

      size_t n = src->get_आकार();
      प्रति (size_t i = 1 ; i < n ; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);

	    vvp_bit4_t ge = compare_gtge(vi, best, BIT4_1);
	    तार्किक replace = want_max ? (ge == BIT4_1) : (ge == BIT4_0);
	    यदि (replace) best = vi;
      }

      dst->push_back(best, 0);
      प्रत्यागम dst;
}

तार्किक सम्बन्धि_QUEUE_FIND_V(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_vector4_t cmp = thr->निष्कासय_vec4();
      अचिह्नित wid = cp->bit_idx[0];
      प्रतिज्ञा(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      यदि (qsrc) dst = queue_सततम्_find_matches_src(qsrc, wid, cmp, असत्य);
      अन्यथा यदि (dsrc) dst = queue_सततम्_find_matches_src(dsrc, wid, cmp, असत्य);
      अन्यथा {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_सततम्_find_matches_src(src, wid, cmp, असत्य);
      }
      thr->प्रक्षिप_object(vvp_object_t(dst));
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_QUEUE_FIND_PROP_V(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_vector4_t cmp = thr->निष्कासय_vec4();
      size_t pid = cp->सङ्ख्या;
      अचिह्नित wid = cp->bit_idx[0];
      प्रतिज्ञा(cmp.size() == wid);

      vvp_object_t& शिखरम् = thr->peek_object();
      vvp_cobject*cobj = शिखरम्.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_सततम्_find_matches_src(qsrc, wid, cmp, असत्य)
				 : queue_सततम्_find_matches_src(dsrc, wid, cmp, असत्य);
      thr->प्रक्षिप_object(vvp_object_t(dst));
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_QUEUE_FIND_सूचकांक_V(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_vector4_t cmp = thr->निष्कासय_vec4();
      अचिह्नित wid = cp->bit_idx[0];
      प्रतिज्ञा(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      यदि (qsrc) dst = queue_सततम्_find_matches_src(qsrc, wid, cmp, सत्य);
      अन्यथा यदि (dsrc) dst = queue_सततम्_find_matches_src(dsrc, wid, cmp, सत्य);
      अन्यथा {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_सततम्_find_matches_src(src, wid, cmp, सत्य);
      }
      thr->प्रक्षिप_object(vvp_object_t(dst));
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_QUEUE_FIND_सूचकांक_PROP_V(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_vector4_t cmp = thr->निष्कासय_vec4();
      size_t pid = cp->सङ्ख्या;
      अचिह्नित wid = cp->bit_idx[0];
      प्रतिज्ञा(cmp.size() == wid);

      vvp_object_t& शिखरम् = thr->peek_object();
      vvp_cobject*cobj = शिखरम्.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_सततम्_find_matches_src(qsrc, wid, cmp, सत्य)
				 : queue_सततम्_find_matches_src(dsrc, wid, cmp, सत्य);
      thr->प्रक्षिप_object(vvp_object_t(dst));
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_QUEUE_FIND_FIRST_V(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_vector4_t cmp = thr->निष्कासय_vec4();
      अचिह्नित wid = cp->bit_idx[0];
      प्रतिज्ञा(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      यदि (qsrc) dst = queue_find_first_चरमः_गन्तव्यम्_queue_src(qsrc, wid, cmp, सत्य, असत्य);
      अन्यथा यदि (dsrc) dst = queue_find_first_चरमः_गन्तव्यम्_queue_src(dsrc, wid, cmp, सत्य, असत्य);
      अन्यथा {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_find_first_चरमः_गन्तव्यम्_queue_src(src, wid, cmp, सत्य, असत्य);
      }
      thr->प्रक्षिप_object(vvp_object_t(dst));
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_QUEUE_FIND_FIRST_PROP_V(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_vector4_t cmp = thr->निष्कासय_vec4();
      size_t pid = cp->सङ्ख्या;
      अचिह्नित wid = cp->bit_idx[0];
      प्रतिज्ञा(cmp.size() == wid);

      vvp_object_t& शिखरम् = thr->peek_object();
      vvp_cobject*cobj = शिखरम्.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst =
	    qsrc ? queue_find_first_चरमः_गन्तव्यम्_queue_src(qsrc, wid, cmp, सत्य, असत्य)
		 : queue_find_first_चरमः_गन्तव्यम्_queue_src(dsrc, wid, cmp, सत्य, असत्य);
      thr->प्रक्षिप_object(vvp_object_t(dst));
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_QUEUE_FIND_FIRST_सूचकांक_V(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_vector4_t cmp = thr->निष्कासय_vec4();
      अचिह्नित wid = cp->bit_idx[0];
      प्रतिज्ञा(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      यदि (qsrc) dst = queue_find_first_चरमः_गन्तव्यम्_queue_src(qsrc, wid, cmp, सत्य, सत्य);
      अन्यथा यदि (dsrc) dst = queue_find_first_चरमः_गन्तव्यम्_queue_src(dsrc, wid, cmp, सत्य, सत्य);
      अन्यथा {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_find_first_चरमः_गन्तव्यम्_queue_src(src, wid, cmp, सत्य, सत्य);
      }
      thr->प्रक्षिप_object(vvp_object_t(dst));
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_QUEUE_FIND_FIRST_सूचकांक_PROP_V(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_vector4_t cmp = thr->निष्कासय_vec4();
      size_t pid = cp->सङ्ख्या;
      अचिह्नित wid = cp->bit_idx[0];
      प्रतिज्ञा(cmp.size() == wid);

      vvp_object_t& शिखरम् = thr->peek_object();
      vvp_cobject*cobj = शिखरम्.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst =
	    qsrc ? queue_find_first_चरमः_गन्तव्यम्_queue_src(qsrc, wid, cmp, सत्य, सत्य)
		 : queue_find_first_चरमः_गन्तव्यम्_queue_src(dsrc, wid, cmp, सत्य, सत्य);
      thr->प्रक्षिप_object(vvp_object_t(dst));
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_QUEUE_FIND_चरमः_V(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_vector4_t cmp = thr->निष्कासय_vec4();
      अचिह्नित wid = cp->bit_idx[0];
      प्रतिज्ञा(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      यदि (qsrc) dst = queue_find_first_चरमः_गन्तव्यम्_queue_src(qsrc, wid, cmp, असत्य, असत्य);
      अन्यथा यदि (dsrc) dst = queue_find_first_चरमः_गन्तव्यम्_queue_src(dsrc, wid, cmp, असत्य, असत्य);
      अन्यथा {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_find_first_चरमः_गन्तव्यम्_queue_src(src, wid, cmp, असत्य, असत्य);
      }
      thr->प्रक्षिप_object(vvp_object_t(dst));
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_QUEUE_FIND_चरमः_PROP_V(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_vector4_t cmp = thr->निष्कासय_vec4();
      size_t pid = cp->सङ्ख्या;
      अचिह्नित wid = cp->bit_idx[0];
      प्रतिज्ञा(cmp.size() == wid);

      vvp_object_t& शिखरम् = thr->peek_object();
      vvp_cobject*cobj = शिखरम्.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst =
	    qsrc ? queue_find_first_चरमः_गन्तव्यम्_queue_src(qsrc, wid, cmp, असत्य, असत्य)
		 : queue_find_first_चरमः_गन्तव्यम्_queue_src(dsrc, wid, cmp, असत्य, असत्य);
      thr->प्रक्षिप_object(vvp_object_t(dst));
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_QUEUE_FIND_चरमः_सूचकांक_V(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_vector4_t cmp = thr->निष्कासय_vec4();
      अचिह्नित wid = cp->bit_idx[0];
      प्रतिज्ञा(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      यदि (qsrc) dst = queue_find_first_चरमः_गन्तव्यम्_queue_src(qsrc, wid, cmp, असत्य, सत्य);
      अन्यथा यदि (dsrc) dst = queue_find_first_चरमः_गन्तव्यम्_queue_src(dsrc, wid, cmp, असत्य, सत्य);
      अन्यथा {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_find_first_चरमः_गन्तव्यम्_queue_src(src, wid, cmp, असत्य, सत्य);
      }
      thr->प्रक्षिप_object(vvp_object_t(dst));
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_QUEUE_FIND_चरमः_सूचकांक_PROP_V(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_vector4_t cmp = thr->निष्कासय_vec4();
      size_t pid = cp->सङ्ख्या;
      अचिह्नित wid = cp->bit_idx[0];
      प्रतिज्ञा(cmp.size() == wid);

      vvp_object_t& शिखरम् = thr->peek_object();
      vvp_cobject*cobj = शिखरम्.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst =
	    qsrc ? queue_find_first_चरमः_गन्तव्यम्_queue_src(qsrc, wid, cmp, असत्य, सत्य)
		 : queue_find_first_चरमः_गन्तव्यम्_queue_src(dsrc, wid, cmp, असत्य, सत्य);
      thr->प्रक्षिप_object(vvp_object_t(dst));
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_QUEUE_MIN_V(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_net_t* net = cp->net;
      अचिह्नित wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      यदि (qsrc) dst = queue_सततम्_min_max_src(qsrc, wid, असत्य);
      अन्यथा यदि (dsrc) dst = queue_सततम्_min_max_src(dsrc, wid, असत्य);
      अन्यथा {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_सततम्_min_max_src(src, wid, असत्य);
      }
      thr->प्रक्षिप_object(vvp_object_t(dst));
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_QUEUE_MIN_OBJ_V(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->निष्कासय_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_सततम्_min_max_src(qsrc, wid, असत्य)
				 : queue_सततम्_min_max_src(dsrc, wid, असत्य);
      thr->प्रक्षिप_object(vvp_object_t(dst));
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_QUEUE_MIN_PROP_V(vthread_t thr, vvp_संहिता_t cp)
{
      size_t pid = cp->सङ्ख्या;
      अचिह्नित wid = cp->bit_idx[0];

      vvp_object_t& शिखरम् = thr->peek_object();
      vvp_cobject*cobj = शिखरम्.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_सततम्_min_max_src(qsrc, wid, असत्य)
				 : queue_सततम्_min_max_src(dsrc, wid, असत्य);
      thr->प्रक्षिप_object(vvp_object_t(dst));
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_QUEUE_MAX_V(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_net_t* net = cp->net;
      अचिह्नित wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      यदि (qsrc) dst = queue_सततम्_min_max_src(qsrc, wid, सत्य);
      अन्यथा यदि (dsrc) dst = queue_सततम्_min_max_src(dsrc, wid, सत्य);
      अन्यथा {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_सततम्_min_max_src(src, wid, सत्य);
      }
      thr->प्रक्षिप_object(vvp_object_t(dst));
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_QUEUE_MAX_OBJ_V(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->निष्कासय_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_सततम्_min_max_src(qsrc, wid, सत्य)
				 : queue_सततम्_min_max_src(dsrc, wid, सत्य);
      thr->प्रक्षिप_object(vvp_object_t(dst));
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_QUEUE_MAX_PROP_V(vthread_t thr, vvp_संहिता_t cp)
{
      size_t pid = cp->सङ्ख्या;
      अचिह्नित wid = cp->bit_idx[0];

      vvp_object_t& शिखरम् = thr->peek_object();
      vvp_cobject*cobj = शिखरम्.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_सततम्_min_max_src(qsrc, wid, सत्य)
				 : queue_सततम्_min_max_src(dsrc, wid, सत्य);
      thr->प्रक्षिप_object(vvp_object_t(dst));
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_QUEUE_UNIQUE_OBJ_V(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->निष्कासय_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_सततम्_unique_src(qsrc, wid, असत्य)
				 : queue_सततम्_unique_src(dsrc, wid, असत्य);
      thr->प्रक्षिप_object(vvp_object_t(dst));
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_QUEUE_UNIQUE_सूचकांक_OBJ_V(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->निष्कासय_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_सततम्_unique_src(qsrc, wid, सत्य)
				 : queue_सततम्_unique_src(dsrc, wid, सत्य);
      thr->प्रक्षिप_object(vvp_object_t(dst));
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_QUEUE_UNIQUE_V(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_net_t*net = cp->net;
      अचिह्नित wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      यदि (qsrc) dst = queue_सततम्_unique_src(qsrc, wid, असत्य);
      अन्यथा यदि (dsrc) dst = queue_सततम्_unique_src(dsrc, wid, असत्य);
      अन्यथा {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_सततम्_unique_src(src, wid, असत्य);
      }
      thr->प्रक्षिप_object(vvp_object_t(dst));
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_QUEUE_UNIQUE_PROP_V(vthread_t thr, vvp_संहिता_t cp)
{
      size_t pid = cp->सङ्ख्या;
      अचिह्नित wid = cp->bit_idx[0];

      vvp_object_t& शिखरम् = thr->peek_object();
      vvp_cobject*cobj = शिखरम्.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_सततम्_unique_src(qsrc, wid, असत्य)
				 : queue_सततम्_unique_src(dsrc, wid, असत्य);
      thr->प्रक्षिप_object(vvp_object_t(dst));
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_QUEUE_UNIQUE_सूचकांक_V(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_net_t*net = cp->net;
      अचिह्नित wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_queue_vec4* dst;
      यदि (qsrc) dst = queue_सततम्_unique_src(qsrc, wid, सत्य);
      अन्यथा यदि (dsrc) dst = queue_सततम्_unique_src(dsrc, wid, सत्य);
      अन्यथा {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    dst = queue_सततम्_unique_src(src, wid, सत्य);
      }
      thr->प्रक्षिप_object(vvp_object_t(dst));
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_QUEUE_UNIQUE_सूचकांक_PROP_V(vthread_t thr, vvp_संहिता_t cp)
{
      size_t pid = cp->सङ्ख्या;
      अचिह्नित wid = cp->bit_idx[0];

      vvp_object_t& शिखरम् = thr->peek_object();
      vvp_cobject*cobj = शिखरम्.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_queue_vec4* dst = qsrc ? queue_सततम्_unique_src(qsrc, wid, सत्य)
				 : queue_सततम्_unique_src(dsrc, wid, सत्य);
      thr->प्रक्षिप_object(vvp_object_t(dst));
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_PROP_QUEUE_आकार(vthread_t thr, vvp_संहिता_t cp)
{
      size_t pid = cp->सङ्ख्या;

      vvp_object_t& शिखरम् = thr->peek_object();
      vvp_cobject*cobj = शिखरम्.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);

      size_t sz = qsrc ? qsrc->get_आकार() : (dsrc ? dsrc->get_आकार() : 0);

      vvp_vector4_t val(32, BIT4_0);
      अचिह्नित दीर्घ ul = sz;
      प्रति (अचिह्नित idx = 0; idx < 32; idx++) {
	    यदि (ul & 1UL) {
		  val.समुच्चयः_bit(idx, BIT4_1);
	    } अन्यथा {
		  val.समुच्चयः_bit(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      thr->प्रक्षिप_vec4(val);
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_QUEUE_आकार_V(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      size_t sz =
	    qsrc ? qsrc->get_आकार() : (dsrc ? dsrc->get_आकार() : 0);

      vvp_vector4_t val(32, BIT4_0);
      अचिह्नित दीर्घ ul = sz;
      प्रति (अचिह्नित idx = 0; idx < 32; idx++) {
	    यदि (ul & 1UL) {
		  val.समुच्चयः_bit(idx, BIT4_1);
	    } अन्यथा {
		  val.समुच्चयः_bit(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      thr->प्रक्षिप_vec4(val);
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_QUEUE_WORD_V(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित wid = cp->bit_idx[0];
      अचिह्नित ix_reg = cp->bit_idx[1];
      int64_t adr = thr->शब्दाः[ix_reg].w_int;
      यदि (thr->flags[4] == BIT4_1) {
	    thr->प्रक्षिप_vec4(vvp_vector4_t(wid));
	    प्रत्यागम सत्य;
      }
      यदि (adr < 0) {
	    thr->प्रक्षिप_vec4(vvp_vector4_t(wid));
	    प्रत्यागम सत्य;
      }
      vvp_net_t* net = cp->net;
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t out(wid);
      यदि (qsrc) {
	    यदि ((size_t)adr >= qsrc->get_आकार()) {
		  thr->प्रक्षिप_vec4(out);
		  प्रत्यागम सत्य;
	    }
	    qsrc->get_word((अचिह्नित)adr, out);
      } अन्यथा यदि (dsrc) {
	    यदि ((size_t)adr >= dsrc->get_आकार()) {
		  thr->प्रक्षिप_vec4(out);
		  प्रत्यागम सत्य;
	    }
	    dsrc->get_word((अचिह्नित)adr, out);
      } अन्यथा {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    यदि (!src || (size_t)adr >= src->get_आकार()) {
		  thr->प्रक्षिप_vec4(out);
		  प्रत्यागम सत्य;
	    }
	    src->get_word((अचिह्नित)adr, out);
      }
      thr->प्रक्षिप_vec4(out);
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_QUEUE_WORD_PROP_V(vthread_t thr, vvp_संहिता_t cp)
{
      size_t pid = cp->सङ्ख्या;
      अचिह्नित wid = cp->bit_idx[0];
      अचिह्नित ix_reg = cp->bit_idx[1];
      int64_t adr = thr->शब्दाः[ix_reg].w_int;
      यदि (thr->flags[4] == BIT4_1) {
	    thr->प्रक्षिप_vec4(vvp_vector4_t(wid));
	    प्रत्यागम सत्य;
      }
      यदि (adr < 0) {
	    thr->प्रक्षिप_vec4(vvp_vector4_t(wid));
	    प्रत्यागम सत्य;
      }

      vvp_object_t saved_शिखरम्;
      तार्किक popped_शिखरम् = असत्य;
      vvp_object_t& शिखरम् = thr->peek_object();
      vvp_cobject*cobj = शिखरम्.peek<vvp_cobject>();
      यदि (cobj == 0) {
	    // Array-locator with(property) keeps an accumulator queue on top.
	    // Temporarily pop it to access the class object beneath.
	    thr->निष्कासय_object(saved_शिखरम्);
	    popped_शिखरम् = सत्य;
	    vvp_object_t& अधः = thr->peek_object();
	    cobj = अधः.peek<vvp_cobject>();
      }
      यदि (cobj == 0) {
	    यदि (popped_शिखरम्) thr->प्रक्षिप_object(saved_शिखरम्);
	    thr->प्रक्षिप_vec4(vvp_vector4_t(wid));
	    प्रत्यागम सत्य;
      }

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t out(wid);
      यदि (qsrc) {
	    यदि ((size_t)adr >= qsrc->get_आकार()) {
		  thr->प्रक्षिप_vec4(out);
		  प्रत्यागम सत्य;
	    }
	    qsrc->get_word((अचिह्नित)adr, out);
      } अन्यथा यदि (dsrc) {
	    यदि ((size_t)adr >= dsrc->get_आकार()) {
		  thr->प्रक्षिप_vec4(out);
		  प्रत्यागम सत्य;
	    }
	    dsrc->get_word((अचिह्नित)adr, out);
      } अन्यथा {
	    thr->प्रक्षिप_vec4(out);
	    यदि (popped_शिखरम्) thr->प्रक्षिप_object(saved_शिखरम्);
	    प्रत्यागम सत्य;
      }
      यदि (popped_शिखरम्) thr->प्रक्षिप_object(saved_शिखरम्);
      thr->प्रक्षिप_vec4(out);
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_CMPIX_LTU(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित a = cp->सङ्ख्या;
      अचिह्नित b = cp->bit_idx[0];
      यदि (thr->शब्दाः[a].w_uint < thr->शब्दाः[b].w_uint) {
	    thr->flags[4] = BIT4_1;
      } अन्यथा {
	    thr->flags[4] = BIT4_0;
      }
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_CMPIX_SLT0(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित r = cp->सङ्ख्या;
      thr->flags[4] = (thr->शब्दाः[r].w_int < 0) ? BIT4_1 : BIT4_0;
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_प्रक्षिप_IX_VEC4(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित ix = cp->सङ्ख्या;
      अचिह्नित wid = cp->bit_idx[0];
      अचिह्नित sgn = cp->bit_idx[1];
      uint32_t ubits;
      यदि (sgn) {
	    int32_t sv = (int32_t)thr->शब्दाः[ix].w_int;
	    ubits = (uint32_t)sv;
      } अन्यथा {
	    ubits = (uint32_t)thr->शब्दाः[ix].w_uint;
      }
      यदि (wid > 32) wid = 32;
      vvp_vector4_t tmp(wid, BIT4_0);
      प्रति (अचिह्नित idx = 0; idx < wid; idx++) {
	    यदि (ubits & 1u) {
		  tmp.समुच्चयः_bit(idx, BIT4_1);
	    } अन्यथा {
		  tmp.समुच्चयः_bit(idx, BIT4_0);
	    }
	    ubits >>= 1;
      }
      thr->प्रक्षिप_vec4(tmp);
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_QUEUE_NEW_रिक्तानि_V(vthread_t thr, vvp_संहिता_t)
{
      thr->प्रक्षिप_object(vvp_object_t(नूतन vvp_queue_vec4()));
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_QUEUE_APPEND_WORD_V(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित wid = cp->bit_idx[0];
      vvp_vector4_t elem = thr->निष्कासय_vec4();
      प्रतिज्ञा(elem.size() == wid);
      vvp_object_t qobj;
      thr->निष्कासय_object(qobj);
      vvp_queue_vec4* q = qobj.peek<vvp_queue_vec4>();
      प्रतिज्ञा(q);
      q->push_back(elem, 0);
      thr->प्रक्षिप_object(qobj);
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_DELETE_PROP_ELEM(vthread_t thr, vvp_संहिता_t cp)
{
      size_t pid = cp->सङ्ख्या;

      int64_t idx_val = thr->शब्दाः[3].w_int;
      यदि (thr->flags[4] == BIT4_1) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with undefined index."
	         << पङ्क्त्यन्तः;
	    प्रत्यागम सत्य;
      }
      यदि (idx_val < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with negative index."
	         << पङ्क्त्यन्तः;
	    प्रत्यागम सत्य;
      }
      size_t idx = idx_val;

      vvp_object_t& शिखरम् = thr->peek_object();
      vvp_cobject*cobj = शिखरम्.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue* queue = qobj.peek<vvp_queue>();
      यदि (queue == 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping delete(" << idx
	         << ") on empty queue." << पङ्क्त्यन्तः;
      } अन्यथा {
	    size_t size = queue->get_आकार();
	    यदि (idx >= size) {
		  cerr << thr->get_fileline()
		       << "Warning: skipping out of range delete(" << idx
		       << ") on queue of size " << size << "." << पङ्क्त्यन्तः;
	    } अन्यथा {
		  queue->erase(idx);
	    }
      }

      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_DELETE_PROP_OBJ(vthread_t thr, vvp_संहिता_t cp)
{
      size_t pid = cp->सङ्ख्या;

      vvp_object_t& शिखरम् = thr->peek_object();
      vvp_cobject*cobj = शिखरम्.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      cobj->समुच्चयः_object(pid, vvp_object_t(), 0);

      प्रत्यागम सत्य;
}

प्रतिरूपम् <प्रकारनाम ELEM, वर्ग QTYPE>
स्थिर तार्किक store_qb(vthread_t thr, vvp_संहिता_t cp, अचिह्नित wid=0)
{
      ELEM value;
      vvp_net_t*net = cp->net;
      अचिह्नित max_आकार = thr->शब्दाः[cp->bit_idx[0]].w_int;
      निष्कासय_मानम्(thr, value, wid); // Pop the value to store.

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      प्रतिज्ञा(queue);
      queue->push_back(value, max_आकार);
      प्रत्यागम सत्य;
}

/*
 * %store/qb/r <var-label>, <max-idx>
 */
तार्किक सम्बन्धि_STORE_QB_R(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम store_qb<द्विगुण, vvp_queue_real>(thr, cp);
}

/*
 * %store/qb/str <var-label>, <max-idx>
 */
तार्किक सम्बन्धि_STORE_QB_STR(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम store_qb<string, vvp_queue_string>(thr, cp);
}

/*
 * %store/qb/v <var-label>, <max-idx>, <wid>
 */
तार्किक सम्बन्धि_STORE_QB_V(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम store_qb<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

प्रतिरूपम् <प्रकारनाम ELEM, वर्ग QTYPE>
स्थिर तार्किक store_qdar(vthread_t thr, vvp_संहिता_t cp, अचिह्नित wid=0)
{
      int64_t idx = thr->शब्दाः[3].w_int;
      ELEM value;
      vvp_net_t*net = cp->net;
      निष्कासय_मानम्(thr, value, wid); // Pop the value to store.

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      प्रतिज्ञा(queue);
      यदि (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot assign to a negative "
	         << get_queue_type(value)
	         << " index (" << idx << "). ";
	    print_queue_मानम्(value);
	    cerr << " was not added." << पङ्क्त्यन्तः;
      } अन्यथा यदि (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot assign to an undefined "
	         << get_queue_type(value) << " index. ";
	    print_queue_मानम्(value);
	    cerr << " was not added." << पङ्क्त्यन्तः;
      } अन्यथा {
	    अचिह्नित max_आकार = thr->शब्दाः[cp->bit_idx[0]].w_int;
	    queue->समुच्चयः_word_max(idx, value, max_आकार);
      }
      प्रत्यागम सत्य;
}

/*
 * %store/qdar/r <var>, idx
 */
तार्किक सम्बन्धि_STORE_QDAR_R(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम store_qdar<द्विगुण, vvp_queue_real>(thr, cp);
}

/*
 * %store/qdar/str <var>, idx
 */
तार्किक सम्बन्धि_STORE_QDAR_STR(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम store_qdar<string, vvp_queue_string>(thr, cp);
}

/*
 * %store/qdar/v <var>, idx
 */
तार्किक सम्बन्धि_STORE_QDAR_V(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम store_qdar<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

प्रतिरूपम् <प्रकारनाम ELEM, वर्ग QTYPE>
स्थिर तार्किक store_qf(vthread_t thr, vvp_संहिता_t cp, अचिह्नित wid=0)
{
      ELEM value;
      vvp_net_t*net = cp->net;
      अचिह्नित max_आकार = thr->शब्दाः[cp->bit_idx[0]].w_int;
      निष्कासय_मानम्(thr, value, wid); // Pop the value to store.

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      प्रतिज्ञा(queue);
      queue->push_front(value, max_आकार);
      प्रत्यागम सत्य;
}
/*
 * %store/qf/r <var-label>, <max-idx>
 */
तार्किक सम्बन्धि_STORE_QF_R(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम store_qf<द्विगुण, vvp_queue_real>(thr, cp);
}

/*
 * %store/qf/str <var-label>, <max-idx>
 */
तार्किक सम्बन्धि_STORE_QF_STR(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम store_qf<string, vvp_queue_string>(thr, cp);
}

/*
 * %store/qb/v <var-label>, <max-idx>, <wid>
 */
तार्किक सम्बन्धि_STORE_QF_V(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम store_qf<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

प्रतिरूपम् <प्रकारनाम ELEM, वर्ग QTYPE>
स्थिर तार्किक store_qobj(vthread_t thr, vvp_संहिता_t cp, अचिह्नित wid=0)
{
// FIXME: Can we actually use wid here?
      (शून्य)wid;
      vvp_net_t*net = cp->net;

      vvp_queue*queue = get_queue_object<QTYPE>(thr, net);
      प्रतिज्ञा(queue);

      vvp_object_t src;
      thr->निष्कासय_object(src);

        // If it is null just clear the queue
      यदि (src.test_nil())
	    queue->erase_पुच्छ(0);
      अन्यथा {
	    अचिह्नित max_आकार = thr->शब्दाः[cp->bit_idx[0]].w_int;
	    queue->copy_elems(src, max_आकार);
      }

      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_STORE_QOBJ_R(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम store_qobj<द्विगुण, vvp_queue_real>(thr, cp);
}

तार्किक सम्बन्धि_STORE_QOBJ_STR(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम store_qobj<string, vvp_queue_string>(thr, cp);
}

तार्किक सम्बन्धि_STORE_QOBJ_V(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम store_qobj<vvp_vector4_t, vvp_queue_vec4>(thr, cp, cp->bit_idx[1]);
}

स्थिर शून्य vvp_send(vthread_t thr, vvp_net_ptr_t ptr, ध्रुव द्विगुण&val)
{
      vvp_send_real(ptr, val, thr->wt_context);
}

स्थिर शून्य vvp_send(vthread_t thr, vvp_net_ptr_t ptr, ध्रुव string&val)
{
      vvp_send_string(ptr, val, thr->wt_context);
}

प्रतिरूपम् <प्रकारनाम ELEM>
स्थिर तार्किक store(vthread_t thr, vvp_संहिता_t cp)
{
      ELEM val;
      निष्कासय_मानम्(thr, val, 0);
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_send(thr, ptr, val);
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_STORE_REAL(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम store<द्विगुण>(thr, cp);
}

प्रतिरूपम् <प्रकारनाम ELEM>
स्थिर तार्किक storea(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित idx = cp->bit_idx[0];
      ELEM val;
      निष्कासय_मानम्(thr, val, 0);

      यदि (thr->flags[4] != BIT4_1) {
	    अचिह्नित adr = thr->शब्दाः[idx].w_int;
	    cp->array->समुच्चयः_word(adr, val);
      }

      प्रत्यागम सत्य;
}

/*
 * %store/reala <var-label> <index>
 */
तार्किक सम्बन्धि_STORE_REALA(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम storea<द्विगुण>(thr, cp);
}

तार्किक सम्बन्धि_STORE_STR(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम store<string>(thr, cp);
}

/*
 * %store/stra <array-label> <index>
 */
तार्किक सम्बन्धि_STORE_STRA(vthread_t thr, vvp_संहिता_t cp)
{
      प्रत्यागम storea<string>(thr, cp);
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
तार्किक सम्बन्धि_STORE_VEC4(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_net_ptr_t ptr(cp->net, 0);
      ध्रुव vvp_signal_मानम्*sig = dynamic_cast<vvp_signal_मानम्*> (cp->net->fil);
      अचिह्नित off_सूचकांक = cp->bit_idx[0];
      अचिह्नित पूर्णाङ्क wid = cp->bit_idx[1];

      int64_t off = off_सूचकांक ? thr->शब्दाः[off_सूचकांक].w_int : 0;
      अचिह्नित पूर्णाङ्क sig_मानम्_आकार = sig->मानम्_आकार();

      vvp_vector4_t&val = thr->peek_vec4();
      अचिह्नित val_आकार = val.size();

      यदि (val_आकार < wid) {
	    cerr << thr->get_fileline()
	         << "XXXX Internal error: val.size()=" << val_आकार
		 << ", expecting >= " << wid << पङ्क्त्यन्तः;
      }
      प्रतिज्ञा(val_आकार >= wid);
      यदि (val_आकार > wid) {
	    val.resize(wid);
      }

	// If there is a problem loading the index register, flags-4
	// will be set to 1, and we know here to skip the actual assignment.
      यदि (off_सूचकांक!=0 && thr->flags[4] == BIT4_1) {
	    thr->निष्कासय_vec4(1);
	    प्रत्यागम सत्य;
      }

      यदि (!resize_rval_vec(val, off, sig_मानम्_आकार)) {
	    thr->निष्कासय_vec4(1);
	    प्रत्यागम सत्य;
      }

      यदि (off == 0 && val.size() == sig_मानम्_आकार)
	    vvp_send_vec4(ptr, val, thr->wt_context);
      अन्यथा
	    vvp_send_vec4_pv(ptr, val, off, sig_मानम्_आकार, thr->wt_context);

      thr->निष्कासय_vec4(1);
      प्रत्यागम सत्य;
}

/*
 * %store/vec4a <var-label>, <addr>, <offset>
 */
तार्किक सम्बन्धि_STORE_VEC4_A(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित adr_सूचकांक = cp->bit_idx[0];
      अचिह्नित off_सूचकांक = cp->bit_idx[1];

      दीर्घ adr = adr_सूचकांक? thr->शब्दाः[adr_सूचकांक].w_int : 0;
      int64_t off = off_सूचकांक ? thr->शब्दाः[off_सूचकांक].w_int : 0;

	// Suppress action if flags-4 is true.
      यदि (thr->flags[4] == BIT4_1) {
	    thr->निष्कासय_vec4(1);
	    प्रत्यागम सत्य;
      }

      vvp_vector4_t &value = thr->peek_vec4();

      यदि (!resize_rval_vec(value, off, cp->array->get_word_आकार())) {
	    thr->निष्कासय_vec4(1);
	    प्रत्यागम सत्य;
      }

      cp->array->समुच्चयः_word(adr, off, value);

      thr->निष्कासय_vec4(1);
      प्रत्यागम सत्य;
}

/*
 * %sub
 *   pop r;
 *   pop l;
 *   push l-r;
 */
तार्किक सम्बन्धि_SUB(vthread_t thr, vvp_संहिता_t)
{
      vvp_vector4_t r = thr->निष्कासय_vec4();
      vvp_vector4_t&l = thr->peek_vec4();

      l.sub(r);
      प्रत्यागम सत्य;
}

/*
 * %subi <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
तार्किक सम्बन्धि_SUBI(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित wid = cp->सङ्ख्या;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_तात्कालिकम्_rval (cp, r);

      l.sub(r);

      प्रत्यागम सत्य;

}

तार्किक सम्बन्धि_SUB_WR(vthread_t thr, vvp_संहिता_t)
{
      द्विगुण r = thr->निष्कासय_real();
      द्विगुण l = thr->निष्कासय_real();
      thr->प्रक्षिप_real(l - r);
      प्रत्यागम सत्य;
}

/*
 * %substr <first>, <last>
 * Pop a string, take the substring (SystemVerilog style), and return
 * the result to the stack. This opcode actually works by editing the
 * string in place.
 */
तार्किक सम्बन्धि_SUBSTR(vthread_t thr, vvp_संहिता_t cp)
{
      int32_t first = thr->शब्दाः[cp->bit_idx[0]].w_int;
      int32_t चरमः = thr->शब्दाः[cp->bit_idx[1]].w_int;
      string&val = thr->peek_str(0);

      यदि (first < 0 || चरमः < first || चरमः >= (int32_t)val.size()) {
	    val = string("");
	    प्रत्यागम सत्य;
      }

      val = val.substr(first, चरमः-first+1);
      प्रत्यागम सत्य;
}

/*
 * %substr/vec4 <index>, <wid>
 */
तार्किक सम्बन्धि_SUBSTR_VEC4(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित sel_idx = cp->bit_idx[0];
      अचिह्नित wid = cp->bit_idx[1];

      int32_t sel = thr->शब्दाः[sel_idx].w_int;
      ध्रुव string&val = thr->peek_str(0);

      प्रतिज्ञा(wid%8 == 0);

      यदि (sel < 0 || sel >= (int32_t)val.size()) {
	    vvp_vector4_t res (wid, BIT4_0);
	    thr->प्रक्षिप_vec4(res);
	    प्रत्यागम सत्य;
      }

      vvp_vector4_t res (wid, BIT4_0);

      प्रतिज्ञा(wid==8);
      अचिह्नित वर्ण tmp = val[sel];
      प्रति (पूर्णाङ्क idx = 0 ; idx < 8 ; idx += 1) {
	    यदि (tmp & (1<<idx))
		  res.समुच्चयः_bit(idx, BIT4_1);
      }

      thr->प्रक्षिप_vec4(res);
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_FILE_पङ्क्तिः(vthread_t thr, vvp_संहिता_t cp)
{
      vpiHandle handle = cp->handle;

	/* When it is available, keep the file/line information in the
	   thread for error/warning messages. */
      thr->समुच्चयः_fileline(vpi_get_str(vpiFile, handle),
                        vpi_get(vpi_पङ्क्तिः_No, handle));

      यदि (show_file_पङ्क्तिः)
	    cerr << thr->get_fileline()
	         << vpi_get_str(_vpiDescription, handle) << पङ्क्त्यन्तः;

      प्रत्यागम सत्य;
}

/*
 * %test_nul <var-label>;
 * Test if the object at the specified variable is nil. If so, write
 * "1" into flags[4], otherwise write "0" into flags[4].
 */
तार्किक सम्बन्धि_TEST_NUL(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_net_t*net = cp->net;

      प्रतिज्ञा(net);
      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      प्रतिज्ञा(obj);

      यदि (obj->get_object().test_nil())
	    thr->flags[4] = BIT4_1;
      अन्यथा
	    thr->flags[4] = BIT4_0;

      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_TEST_NUL_A(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित idx = cp->bit_idx[0];
      अचिह्नित adr = thr->शब्दाः[idx].w_int;
      vvp_object_t word;

	/* If the address is undefined, return true. */
      यदि (thr->flags[4] == BIT4_1) {
	    प्रत्यागम सत्य;
      }

      cp->array->get_word_obj(adr, word);
      यदि (word.test_nil())
	    thr->flags[4] = BIT4_1;
      अन्यथा
	    thr->flags[4] = BIT4_0;

      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_TEST_NUL_OBJ(vthread_t thr, vvp_संहिता_t)
{
      यदि (thr->peek_object().test_nil())
	    thr->flags[4] = BIT4_1;
      अन्यथा
	    thr->flags[4] = BIT4_0;
      प्रत्यागम सत्य;
}

/*
 * %test_nul/prop <pid>, <idx>
 */
तार्किक सम्बन्धि_TEST_NUL_PROP(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित pid = cp->सङ्ख्या;
      अचिह्नित idx = cp->bit_idx[0];

      यदि (idx != 0) {
	    प्रतिज्ञा(idx < vthread_s::शब्दाः_गणनम्);
	    idx = thr->शब्दाः[idx].w_uint;
      }

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj  = obj.peek<vvp_cobject>();

      vvp_object_t val;
      cobj->get_object(pid, val, idx);

      यदि (val.test_nil())
	    thr->flags[4] = BIT4_1;
      अन्यथा
	    thr->flags[4] = BIT4_0;

      प्रत्यागम सत्य;
}

/* and() reduction returns the scalar sum (Verilog-style bitwise-AND per word). */
प्रतिरूपम् <वर्ग SRC>
स्थिर vvp_vector4_t uparr_and_शब्दाः_src(SRC* src, अचिह्नित wid)
{
      यदि (!src) प्रत्यागम vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_1);
      प्रति (size_t i = 0; i < src->get_आकार(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    res &= vi;
      }
      प्रत्यागम res;
}

तार्किक सम्बन्धि_UPARR_AND_V(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_net_t* net = cp->net;
      अचिह्नित wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      यदि (qsrc) res = uparr_and_शब्दाः_src(qsrc, wid);
      अन्यथा यदि (dsrc) res = uparr_and_शब्दाः_src(dsrc, wid);
      अन्यथा {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    res = uparr_and_शब्दाः_src(src, wid);
      }
      thr->प्रक्षिप_vec4(res);
      प्रत्यागम सत्य;
}

/* or() reduction returns the scalar sum (Verilog-style bitwise-AND per word). */
प्रतिरूपम् <वर्ग SRC>
स्थिर vvp_vector4_t uparr_or_शब्दाः_src(SRC* src, अचिह्नित wid)
{
      यदि (!src) प्रत्यागम vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_0);
      प्रति (size_t i = 0; i < src->get_आकार(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    res |= vi;
      }
      प्रत्यागम res;
}

तार्किक सम्बन्धि_UPARR_OR_V(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_net_t* net = cp->net;
      अचिह्नित wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      यदि (qsrc) res = uparr_or_शब्दाः_src(qsrc, wid);
      अन्यथा यदि (dsrc) res = uparr_or_शब्दाः_src(dsrc, wid);
      अन्यथा {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    res = uparr_or_शब्दाः_src(src, wid);
      }
      thr->प्रक्षिप_vec4(res);
      प्रत्यागम सत्य;
}

/* real product() reduction returns the scalar sum */
प्रतिरूपम् <वर्ग SRC>
स्थिर द्विगुण uparr_गुणनफलम्_शब्दाः_src_real(SRC* src)
{
      यदि (!src) प्रत्यागम 0.0;
      द्विगुण acc = 1.0;
      प्रति (size_t i = 0; i < src->get_आकार(); i += 1) {
	    द्विगुण vi;
	    src->get_word(i, vi);
	    acc *= vi;
      }
      प्रत्यागम acc;
}

तार्किक सम्बन्धि_UPARR_गुणनफलम्_REAL(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_queue_real* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_real_from_net(thr, net, qsrc, dsrc);
      द्विगुण योग;
      यदि (qsrc) योग = uparr_गुणनफलम्_शब्दाः_src_real(qsrc);
      अन्यथा यदि (dsrc) योग = uparr_गुणनफलम्_शब्दाः_src_real(dsrc);
      अन्यथा {
	    vvp_queue* src_q = get_queue_object<vvp_queue_real>(thr, net);
	    vvp_queue_real* src = dynamic_cast<vvp_queue_real*>(src_q);
	    योग = uparr_गुणनफलम्_शब्दाः_src_real(src);
      }
      thr->प्रक्षिप_real(योग);
      प्रत्यागम सत्य;
}

/* product() reduction returns the scalar product (Verilog-style multiplication per word). */
प्रतिरूपम् <वर्ग SRC>
स्थिर vvp_vector4_t uparr_गुणनफलम्_शब्दाः_src(SRC* src, अचिह्नित wid)
{
      यदि (!src) प्रत्यागम vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t acc = queue_unique_ulong_गन्तव्यम्_vec4(1UL, wid);
      प्रति (size_t i = 0; i < src->get_आकार(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    acc.mul(vi);
      }
      प्रत्यागम acc;
}

तार्किक सम्बन्धि_UPARR_गुणनफलम्_V(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_net_t* net = cp->net;
      अचिह्नित wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t prod;
      यदि (qsrc) prod = uparr_गुणनफलम्_शब्दाः_src(qsrc, wid);
      अन्यथा यदि (dsrc) prod = uparr_गुणनफलम्_शब्दाः_src(dsrc, wid);
      अन्यथा {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    prod = uparr_गुणनफलम्_शब्दाः_src(src, wid);
      }
      thr->प्रक्षिप_vec4(prod);
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_UPARR_गुणनफलम्_PROP_V(vthread_t thr, vvp_संहिता_t cp)
{
      size_t pid = cp->सङ्ख्या;
      अचिह्नित wid = cp->bit_idx[0];

      vvp_object_t& शिखरम् = thr->peek_object();
      vvp_cobject*cobj = शिखरम्.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t prod =
	    qsrc ? uparr_गुणनफलम्_शब्दाः_src(qsrc, wid)
		 : uparr_गुणनफलम्_शब्दाः_src(dsrc, wid);
      thr->प्रक्षिप_vec4(prod);
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_UPARR_गुणनफलम्_OBJ_V(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->निष्कासय_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_vector4_t prod =
	    qsrc ? uparr_गुणनफलम्_शब्दाः_src(qsrc, wid)
		 : uparr_गुणनफलम्_शब्दाः_src(dsrc, wid);
      thr->प्रक्षिप_vec4(prod);
      प्रत्यागम सत्य;
}

/* real sum() reduction returns the scalar sum */
प्रतिरूपम् <वर्ग SRC>
स्थिर द्विगुण uparr_योग_शब्दाः_src_real(SRC* src)
{
      द्विगुण acc = 0.0;
      यदि (!src) प्रत्यागम acc;
      प्रति (size_t i = 0; i < src->get_आकार(); i += 1) {
	    द्विगुण vi;
	    src->get_word(i, vi);
	    acc += vi;
      }
      प्रत्यागम acc;
}

तार्किक सम्बन्धि_UPARR_योग_REAL(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_queue_real* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_real_from_net(thr, net, qsrc, dsrc);
      द्विगुण योग;
      यदि (qsrc) योग = uparr_योग_शब्दाः_src_real(qsrc);
      अन्यथा यदि (dsrc) योग = uparr_योग_शब्दाः_src_real(dsrc);
      अन्यथा {
	    vvp_queue* src_q = get_queue_object<vvp_queue_real>(thr, net);
	    vvp_queue_real* src = dynamic_cast<vvp_queue_real*>(src_q);
	    योग = uparr_योग_शब्दाः_src_real(src);
      }
      thr->प्रक्षिप_real(योग);
      प्रत्यागम सत्य;
}

/* sum() reduction returns the scalar sum (Verilog-style addition per word). */
प्रतिरूपम् <वर्ग SRC>
स्थिर vvp_vector4_t uparr_योग_शब्दाः_src(SRC* src, अचिह्नित wid)
{
      यदि (!src) प्रत्यागम vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t acc(wid, BIT4_0);
      प्रति (size_t i = 0; i < src->get_आकार(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    acc.सङ्कलय(vi);
      }
      प्रत्यागम acc;
}

तार्किक सम्बन्धि_UPARR_योग_V(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_net_t* net = cp->net;
      अचिह्नित wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t योग;
      यदि (qsrc) योग = uparr_योग_शब्दाः_src(qsrc, wid);
      अन्यथा यदि (dsrc) योग = uparr_योग_शब्दाः_src(dsrc, wid);
      अन्यथा {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    योग = uparr_योग_शब्दाः_src(src, wid);
      }
      thr->प्रक्षिप_vec4(योग);
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_UPARR_योग_PROP_V(vthread_t thr, vvp_संहिता_t cp)
{
      size_t pid = cp->सङ्ख्या;
      अचिह्नित wid = cp->bit_idx[0];

      vvp_object_t& शिखरम् = thr->peek_object();
      vvp_cobject*cobj = शिखरम्.peek<vvp_cobject>();
      प्रतिज्ञा(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t योग =
	    qsrc ? uparr_योग_शब्दाः_src(qsrc, wid)
		 : uparr_योग_शब्दाः_src(dsrc, wid);
      thr->प्रक्षिप_vec4(योग);
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_UPARR_योग_OBJ_V(vthread_t thr, vvp_संहिता_t cp)
{
      अचिह्नित wid = cp->bit_idx[0];
      vvp_object_t src_obj;
      thr->निष्कासय_object(src_obj);

      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_vector4_t योग =
	    qsrc ? uparr_योग_शब्दाः_src(qsrc, wid)
		 : uparr_योग_शब्दाः_src(dsrc, wid);
      thr->प्रक्षिप_vec4(योग);
      प्रत्यागम सत्य;
}

/* xor() reduction returns the scalar sum (Verilog-style bitwise-XOR per word). */
प्रतिरूपम् <वर्ग SRC>
स्थिर vvp_vector4_t uparr_xor_शब्दाः_src(SRC* src, अचिह्नित wid)
{
      यदि (!src) प्रत्यागम vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_0);
      प्रति (size_t i = 0; i < src->get_आकार(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_word(i, vi);
	    res ^= vi;
      }
      प्रत्यागम res;
}

तार्किक सम्बन्धि_UPARR_XOR_V(vthread_t thr, vvp_संहिता_t cp)
{
      vvp_net_t* net = cp->net;
      अचिह्नित wid = cp->bit_idx[0];
      vvp_queue_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_queue_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      यदि (qsrc) res = uparr_xor_शब्दाः_src(qsrc, wid);
      अन्यथा यदि (dsrc) res = uparr_xor_शब्दाः_src(dsrc, wid);
      अन्यथा {
	    vvp_queue* src_q = get_queue_object<vvp_queue_vec4>(thr, net);
	    vvp_queue_vec4* src = dynamic_cast<vvp_queue_vec4*>(src_q);
	    res = uparr_xor_शब्दाः_src(src, wid);
      }
      thr->प्रक्षिप_vec4(res);
      प्रत्यागम सत्य;
}

तार्किक सम्बन्धि_VPI_CALL(vthread_t thr, vvp_संहिता_t cp)
{
      vpip_execute_vpi_call(thr, cp->handle);

      यदि (कालसारणी_stopped()) {
	    यदि (! कालसारणी_परिसमाप्तम्())
		  कालसारणी_vthread(thr, 0, असत्य);

	    प्रत्यागम असत्य;
      }

      प्रत्यागम कालसारणी_परिसमाप्तम्()? असत्य : सत्य;
}

/* %wait <label>;
 * Implement the wait by locating the vvp_net_T for the event, and
 * adding this thread to the threads list for the event. The some
 * argument is the  reference to the functor to wait for. This must be
 * an event object of some sort.
 */
तार्किक सम्बन्धि_WAIT(vthread_t thr, vvp_संहिता_t cp)
{
      प्रतिज्ञा(! thr->i_am_अन्तर्_function);
      प्रतिज्ञा(! thr->प्रतीक्षा_for_event);
      thr->प्रतीक्षा_for_event = 1;

	/* Add this thread to the list in the event. */
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (cp->net->fun);
      प्रतिज्ञा(ep);
      thr->wait_अग्र = ep->सङ्कलय_प्रतीक्षा_thread(thr);

	/* Return false to suspend this thread. */
      प्रत्यागम असत्य;
}

/*
 * Implement the %wait/fork (SystemVerilog) instruction by suspending
 * the current thread until all the detached children have finished.
 */
तार्किक सम्बन्धि_WAIT_FORK(vthread_t thr, vvp_संहिता_t)
{
	/* If a %wait/fork is being executed then the parent thread
	 * cannot be waiting in a join or already waiting. */
      प्रतिज्ञा(! thr->i_am_अन्तर्_function);
      प्रतिज्ञा(! thr->i_am_joining);
      प्रतिज्ञा(! thr->i_am_प्रतीक्षा);

	/* There should be no active children when waiting. */
      प्रतिज्ञा(thr->children.empty());

	/* If there are no detached children then there is nothing to
	 * wait for. */
      यदि (thr->detached_children.empty()) प्रत्यागम सत्य;

	/* Flag that this process is waiting for the detached children
	 * to finish and suspend it. */
      thr->i_am_प्रतीक्षा = 1;
      प्रत्यागम असत्य;
}

/*
 * %xnor
 */
तार्किक सम्बन्धि_XNOR(vthread_t thr, vvp_संहिता_t)
{
      vvp_vector4_t valr = thr->निष्कासय_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      प्रतिज्ञा(vall.size() == valr.size());

      vall ^= valr;
      vall.invert();

      प्रत्यागम सत्य;
}

/*
 * %xor
 */
तार्किक सम्बन्धि_XOR(vthread_t thr, vvp_संहिता_t)
{
      vvp_vector4_t valr = thr->निष्कासय_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      प्रतिज्ञा(vall.size() == valr.size());

      vall ^= valr;

      प्रत्यागम सत्य;
}


तार्किक सम्बन्धि_ZOMBIE(vthread_t thr, vvp_संहिता_t)
{
      thr->pc = codespace_null();
      यदि ((thr->parent == 0) && (thr->children.empty())) {
	    यदि (thr->delay_delete)
		  कालसारणी_del_thr(thr);
	    अन्यथा
		  vthread_delete(thr);
      }
      प्रत्यागम असत्य;
}

/*
 * This is a phantom opcode used to call user defined functions. It
 * is used in code generated by the .ufunc statement. It contains a
 * pointer to the executable code of the function and a pointer to
 * a ufunc_core object that has all the port information about the
 * function.
 */
स्थिर तार्किक do_exec_ufunc(vthread_t thr, vvp_संहिता_t cp, vthread_t child)
{
      __vpiScope*child_scope = cp->ufunc_core_ptr->func_scope();
      प्रतिज्ञा(child_scope);

      प्रतिज्ञा(child_scope->get_type_संहिता() == vpiFunction);
      प्रतिज्ञा(thr->children.empty());


        /* We can take a number of shortcuts because we know that a
           continuous assignment can only occur in a static scope. */
      प्रतिज्ञा(thr->wt_context == 0);
      प्रतिज्ञा(thr->rd_context == 0);

        /* If an automatic function, allocate a context for this call. */
      vvp_context_t child_context = 0;
      यदि (child_scope->is_automatic()) {
            child_context = vthread_alloc_context(child_scope);
            thr->wt_context = child_context;
            thr->rd_context = child_context;
      }

      child->wt_context = child_context;
      child->rd_context = child_context;

	/* Copy all the inputs to the ufunc object to the port
	   variables of the function. This copies all the values
	   atomically. */
      cp->ufunc_core_ptr->नियोजय_द्वयङ्काः_गन्तव्यम्_ports(child_context);
      child->delay_delete = 1;

      child->parent = thr;
      thr->children.insert(child);
	// This should be the only child
      प्रतिज्ञा(thr->children.size()==1);

      child->is_scheduled = 1;
      child->i_am_अन्तर्_function = 1;
      vthread_सततम्(child);
      running_thread = thr;

      यदि (child->i_have_ended) {
	    do_join(thr, child);
            प्रत्यागम सत्य;
      } अन्यथा {
	    thr->i_am_joining = 1;
	    प्रत्यागम असत्य;
      }
}

तार्किक सम्बन्धि_EXEC_UFUNC_REAL(vthread_t thr, vvp_संहिता_t cp)
{
      __vpiScope*child_scope = cp->ufunc_core_ptr->func_scope();
      प्रतिज्ञा(child_scope);

	/* Create a temporary thread and run it immediately. */
      vthread_t child = vthread_new(cp->cptr, child_scope);
      thr->प्रक्षिप_real(0.0);
      child->तर्काः_real.push_back(0);

      प्रत्यागम do_exec_ufunc(thr, cp, child);
}

तार्किक सम्बन्धि_EXEC_UFUNC_VEC4(vthread_t thr, vvp_संहिता_t cp)
{
      __vpiScope*child_scope = cp->ufunc_core_ptr->func_scope();
      प्रतिज्ञा(child_scope);

      vpiScopeFunction*scope_func = dynamic_cast<vpiScopeFunction*>(child_scope);
      प्रतिज्ञा(scope_func);

	/* Create a temporary thread and run it immediately. */
      vthread_t child = vthread_new(cp->cptr, child_scope);
      thr->प्रक्षिप_vec4(vvp_vector4_t(scope_func->get_func_विस्तारः(), scope_func->get_func_init_val()));
      child->तर्काः_vec4.push_back(0);

      प्रत्यागम do_exec_ufunc(thr, cp, child);
}

/*
 * This is a phantom opcode used to harvest the result of calling a user
 * defined function. It is used in code generated by the .ufunc statement.
 */
तार्किक सम्बन्धि_REAP_UFUNC(vthread_t thr, vvp_संहिता_t cp)
{
      __vpiScope*child_scope = cp->ufunc_core_ptr->func_scope();
      प्रतिज्ञा(child_scope);

	/* Copy the output from the result variable to the output
	   ports of the .ufunc device. */
      cp->ufunc_core_ptr->finish_thread();

        /* If an automatic function, free the context for this call. */
      यदि (child_scope->is_automatic()) {
            vthread_free_context(thr->rd_context, child_scope);
            thr->wt_context = 0;
            thr->rd_context = 0;
      }

      प्रत्यागम सत्य;
}
