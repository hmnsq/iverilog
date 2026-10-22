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
#నిర్వచితమైతే CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#అయితే_ముగింపు
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

ఉపయోగించి నామాకాశ std;

/* This is the size of an unsigned long in bits. This is just a
   convenience macro. */
# define CPU_మాట_బిట్లు (8*పరిమాణం(గుర్తులేని దీర్ఘ))
# define శిఖరం_బిట్టు (1UL << (CPU_మాట_బిట్లు-1))

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

నిర్మాణం vthread_s {
      vthread_s();

      శూన్య debug_dump(ostream&fd, స్థిరరాశి అక్షరం*label_పాఠ్యం);

	/* This is the program counter. */
      vvp_సంకేతం_t pc;
	/* These hold the private thread bits. */
      గణన { FLAGS_లెక్క = 512, పదాలు_లెక్క = 16 };
      vvp_bit4_t flags[FLAGS_లెక్క];

	/* These are the word registers. */
      సంఘం {
	    int64_t  w_int;
	    uint64_t w_uint;
      } పదాలు[పదాలు_లెక్క];

	// These vectors are depths within the parent thread's
	// corresponding stack.  This is how the %ret/* instructions
	// get at parent thread arguments.
      vector<గుర్తులేని> వాదనలు_real;
      vector<గుర్తులేని> వాదనలు_str;
      vector<గుర్తులేని> వాదనలు_vec4;

    అంతరంగ:
      vector<vvp_vector4_t>దొంతర_vec4;
    బహిరంగ:
      అంతఃవరుస vvp_vector4_t తీయి_vec4(శూన్య)
      {
	    ప్రతిజ్ఞా(! దొంతర_vec4.empty());
	    vvp_vector4_t val = దొంతర_vec4.back();
	    దొంతర_vec4.pop_back();
	    తిరిగి val;
      }
      అంతఃవరుస శూన్య నెట్టు_vec4(స్థిరరాశి vvp_vector4_t&val)
      {
	    దొంతర_vec4.push_back(val);
      }
      అంతఃవరుస స్థిరరాశి vvp_vector4_t& peek_vec4(గుర్తులేని లోతు)
      {
	    గుర్తులేని size = దొంతర_vec4.size();
	    ప్రతిజ్ఞా(లోతు < size);
	    గుర్తులేని use_సూచీ = size-1-లోతు;
	    తిరిగి దొంతర_vec4[use_సూచీ];
      }
      అంతఃవరుస vvp_vector4_t& peek_vec4(శూన్య)
      {
	    గుర్తులేని use_సూచీ = దొంతర_vec4.size();
	    ప్రతిజ్ఞా(use_సూచీ >= 1);
	    తిరిగి దొంతర_vec4[use_సూచీ-1];
      }
      అంతఃవరుస శూన్య poke_vec4(గుర్తులేని లోతు, స్థిరరాశి vvp_vector4_t&val)
      {
	    ప్రతిజ్ఞా(లోతు < దొంతర_vec4.size());
	    గుర్తులేని use_సూచీ = దొంతర_vec4.size()-1-లోతు;
	    దొంతర_vec4[use_సూచీ] = val;
      }
      అంతఃవరుస శూన్య తీయి_vec4(గుర్తులేని cnt)
      {
	    అయితే (cnt > 0) {
		  దొంతర_vec4.pop_back();
		  cnt -= 1;
	    }
      }


    అంతరంగ:
      vector<ద్విగుణ> దొంతర_real;
    బహిరంగ:
      అంతఃవరుస ద్విగుణ తీయి_real(శూన్య)
      {
	    ప్రతిజ్ఞా(! దొంతర_real.empty());
	    ద్విగుణ val = దొంతర_real.back();
	    దొంతర_real.pop_back();
	    తిరిగి val;
      }
      అంతఃవరుస శూన్య నెట్టు_real(ద్విగుణ val)
      {
	    దొంతర_real.push_back(val);
      }
      అంతఃవరుస ద్విగుణ peek_real(గుర్తులేని లోతు)
      {
	    ప్రతిజ్ఞా(లోతు < దొంతర_real.size());
	    గుర్తులేని use_సూచీ = దొంతర_real.size()-1-లోతు;
	    తిరిగి దొంతర_real[use_సూచీ];
      }
      అంతఃవరుస శూన్య poke_real(గుర్తులేని లోతు, ద్విగుణ val)
      {
	    ప్రతిజ్ఞా(లోతు < దొంతర_real.size());
	    గుర్తులేని use_సూచీ = దొంతర_real.size()-1-లోతు;
	    దొంతర_real[use_సూచీ] = val;
      }
      అంతఃవరుస శూన్య తీయి_real(గుర్తులేని cnt)
      {
	    అయితే (cnt > 0) {
		  దొంతర_real.pop_back();
		  cnt -= 1;
	    }
      }

	/* Strings are operated on using a forth-like operator
	   set. Items at the top of the stack (back()) are the objects
	   operated on except for special cases. New objects are
	   pushed onto the top (back()) and pulled from the top
	   (back()) only. */
    అంతరంగ:
      vector<string> దొంతర_str;
    బహిరంగ:
      అంతఃవరుస string తీయి_str(శూన్య)
      {
	    ప్రతిజ్ఞా(! దొంతర_str.empty());
	    string val = దొంతర_str.back();
	    దొంతర_str.pop_back();
	    తిరిగి val;
      }
      అంతఃవరుస శూన్య నెట్టు_str(స్థిరరాశి string&val)
      {
	    దొంతర_str.push_back(val);
      }
      అంతఃవరుస string&peek_str(గుర్తులేని లోతు)
      {
	    ప్రతిజ్ఞా(లోతు<దొంతర_str.size());
	    గుర్తులేని use_సూచీ = దొంతర_str.size()-1-లోతు;
	    తిరిగి దొంతర_str[use_సూచీ];
      }
      అంతఃవరుస శూన్య poke_str(గుర్తులేని లోతు, స్థిరరాశి string&val)
      {
	    ప్రతిజ్ఞా(లోతు < దొంతర_str.size());
	    గుర్తులేని use_సూచీ = దొంతర_str.size()-1-లోతు;
	    దొంతర_str[use_సూచీ] = val;
      }
      అంతఃవరుస శూన్య తీయి_str(గుర్తులేని cnt)
      {
	    అయితే (cnt > 0) {
		  దొంతర_str.pop_back();
		  cnt -= 1;
	    }
      }

	/* Objects are also operated on in a stack. */
    అంతరంగ:
      గణన { దొంతర_OBJ_MAX_కొలత = 32 };
      vvp_object_t దొంతర_obj[దొంతర_OBJ_MAX_కొలత];
      గుర్తులేని దొంతర_obj_కొలత;
    బహిరంగ:
      అంతఃవరుస vvp_object_t& peek_object(శూన్య)
      {
	    ప్రతిజ్ఞా(దొంతర_obj_కొలత > 0);
	    తిరిగి దొంతర_obj[దొంతర_obj_కొలత-1];
      }
      అంతఃవరుస శూన్య తీయి_object(vvp_object_t&obj)
      {
	    ప్రతిజ్ఞా(దొంతర_obj_కొలత > 0);
	    దొంతర_obj_కొలత -= 1;
	    obj = దొంతర_obj[దొంతర_obj_కొలత];
	    దొంతర_obj[దొంతర_obj_కొలత].reset(0);
      }
      అంతఃవరుస శూన్య తీయి_object(గుర్తులేని cnt, గుర్తులేని skip =0)
      {
	    ప్రతిజ్ఞా((cnt+skip) <= దొంతర_obj_కొలత);
	    కొరకు (size_t idx = దొంతర_obj_కొలత-skip-cnt ; idx < దొంతర_obj_కొలత-skip ; idx += 1)
		  దొంతర_obj[idx].reset(0);
	    దొంతర_obj_కొలత -= cnt;
	    కొరకు (size_t idx = దొంతర_obj_కొలత-skip ; idx < దొంతర_obj_కొలత ; idx += 1)
		  దొంతర_obj[idx] = దొంతర_obj[idx+skip];
	    కొరకు (size_t idx = దొంతర_obj_కొలత ; idx < దొంతర_obj_కొలత+skip ; idx += 1)
		  దొంతర_obj[idx].reset(0);
      }
      అంతఃవరుస శూన్య నెట్టు_object(స్థిరరాశి vvp_object_t&obj)
      {
	    ప్రతిజ్ఞా(దొంతర_obj_కొలత < దొంతర_OBJ_MAX_కొలత);
	    దొంతర_obj[దొంతర_obj_కొలత] = obj;
	    దొంతర_obj_కొలత += 1;
      }

	/* My parent sets this when it wants me to wake it up. */
      గుర్తులేని i_am_joining      :1;
      గుర్తులేని i_am_detached     :1;
      గుర్తులేని i_am_నిరీక్షణ      :1;
      గుర్తులేని i_am_లోపల_function  :1; // True if running function code
      గుర్తులేని i_have_ended      :1;
      గుర్తులేని i_was_disabled    :1;
      గుర్తులేని నిరీక్షణ_for_event :1;
      గుర్తులేని is_scheduled      :1;
      గుర్తులేని delay_delete      :1;
	/* This points to the children of the thread. */
      సమితి<నిర్మాణం vthread_s*>children;
	/* This points to the detached children of the thread. */
      సమితి<నిర్మాణం vthread_s*>detached_children;
	/* This points to my parent, if I have one. */
      నిర్మాణం vthread_s*మాతృ;
	/* This points to the containing scope. */
      __vpiScope*మాతృ_scope;
	/* This is used for keeping wait queues. */
      నిర్మాణం vthread_s*wait_తదుపరి;
	/* These are used to access automatically allocated items. */
      vvp_context_t wt_context, rd_context;
	/* These are used to pass non-blocking event control information. */
      vvp_net_t*event;
      uint64_t ecount;
	/* Save the file/line information when available. */
    అంతరంగ:
      అక్షరం *filenm_;
      గుర్తులేని lineno_;
    బహిరంగ:
      శూన్య సమితి_fileline(అక్షరం *filenm, గుర్తులేని lineno);
      string get_fileline();

      అంతఃవరుస శూన్య cleanup()
      {
	    ఒకవేళ (i_was_disabled) {
		  దొంతర_vec4.clear();
		  దొంతర_real.clear();
		  దొంతర_str.clear();
		  తీయి_object(దొంతర_obj_కొలత);
	    }
	    విడుదల(filenm_);
	    filenm_ = 0;
	    ప్రతిజ్ఞా(దొంతర_vec4.empty());
	    ప్రతిజ్ఞా(దొంతర_real.empty());
	    ప్రతిజ్ఞా(దొంతర_str.empty());
	    ప్రతిజ్ఞా(దొంతర_obj_కొలత == 0);
      }
};

అంతఃవరుస vthread_s::vthread_s()
{
      దొంతర_obj_కొలత = 0;
      filenm_ = 0;
      lineno_ = 0;
}

శూన్య vthread_s::సమితి_fileline(అక్షరం *filenm, గుర్తులేని lineno)
{
      ప్రతిజ్ఞా(filenm);
      ఒకవేళ (!filenm_ || (strcmp(filenm_, filenm) != 0)) {
	    విడుదల(filenm_);
	    filenm_ = strdup(filenm);
      }
      lineno_ = lineno;
}

అంతఃవరుస string vthread_s::get_fileline()
{
      ostringstream buf;
      ఒకవేళ (filenm_) {
	    buf << filenm_ << ":" << lineno_ << ": ";
      }
      string res = buf.str();
      తిరిగి res;
}

శూన్య vthread_s::debug_dump(ostream&fd, స్థిరరాశి అక్షరం*label)
{
      fd << "**** " << label << పంక్తి_ముగింపు;
      fd << "**** ThreadId: " << ఇది << ", parent id: " << మాతృ << పంక్తి_ముగింపు;

      fd << "**** Flags: ";
      కొరకు (పూర్ణాంకం idx = 0 ; idx < FLAGS_లెక్క ; idx += 1)
	    fd << flags[idx];
      fd << పంక్తి_ముగింపు;
      fd << "**** vec4 stack..." << పంక్తి_ముగింపు;
      కొరకు (size_t idx = దొంతర_vec4.size() ; idx > 0 ; idx -= 1)
	    fd << "    " << (దొంతర_vec4.size()-idx) << ": " << దొంతర_vec4[idx-1] << పంక్తి_ముగింపు;
      fd << "**** str stack (" << దొంతర_str.size() << ")..." << పంక్తి_ముగింపు;
      fd << "**** obj stack (" << దొంతర_obj_కొలత << ")..." << పంక్తి_ముగింపు;
      fd << "**** args_vec4 array (" << వాదనలు_vec4.size() << ")..." << పంక్తి_ముగింపు;
      కొరకు (size_t idx = 0 ; idx < వాదనలు_vec4.size() ; idx += 1)
	    fd << "    " << idx << ": " << వాదనలు_vec4[idx] << పంక్తి_ముగింపు;
      fd << "**** file/line (";
      ఒకవేళ (filenm_) fd << filenm_;
      లేకపోతే fd << "<no file name>";
      fd << ":" << lineno_ << ")" << పంక్తి_ముగింపు;
      fd << "**** Done ****" << పంక్తి_ముగింపు;
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
స్థిర string filter_string(స్థిరరాశి అక్షరం*పాఠ్యం)
{
      vector<అక్షరం> tmp (అక్షరశ్రేణి_పొడవు(పాఠ్యం)+1);
      size_t dst = 0;
      కొరకు (స్థిరరాశి అక్షరం*ptr = పాఠ్యం ; *ptr ; ptr += 1) {
	    // Not an escape? Move on.
	    ఒకవేళ (*ptr != '\\') {
		  tmp[dst++] = *ptr;
		  కొనసాగు;
	    }

	    // Now we know that *ptr is pointing to a \ character and we
	    // have an octal sequence coming up. Advance the ptr and start
	    // processing octal digits.
	    ptr += 1;
	    ఒకవేళ (*ptr == 0)
		  ఆపు;

	    అక్షరం byte = 0;
	    పూర్ణాంకం cnt = 3;
	    అయితే (*ptr && cnt > 0 && *ptr >= '0' && *ptr <= '7') {
		  byte *= 8;
		  byte += *ptr - '0';
		  cnt -= 1;
		  ptr += 1;
	    }

	    // null-bytes are supposed to be removed when assigning a string
	    // literal to a string.
	    ఒకవేళ (byte != '\0')
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
      తిరిగి res;
}

స్థిర శూన్య do_join(vthread_t thr, vthread_t బిడ్డ);

__vpiScope* vthread_scope(నిర్మాణం vthread_s*thr)
{
      తిరిగి thr->మాతృ_scope;
}

నిర్మాణం vthread_s*running_thread = 0;

string get_fileline()
{
      తిరిగి running_thread->get_fileline();
}

శూన్య vthread_నెట్టు(నిర్మాణం vthread_s*thr, ద్విగుణ val)
{
      thr->నెట్టు_real(val);
}

శూన్య vthread_నెట్టు(నిర్మాణం vthread_s*thr, స్థిరరాశి string&val)
{
      thr->నెట్టు_str(val);
}

శూన్య vthread_నెట్టు(నిర్మాణం vthread_s*thr, స్థిరరాశి vvp_vector4_t&val)
{
      thr->నెట్టు_vec4(val);
}

శూన్య vthread_తీయి_real(నిర్మాణం vthread_s*thr, గుర్తులేని లోతు)
{
      thr->తీయి_real(లోతు);
}

శూన్య vthread_తీయి_str(నిర్మాణం vthread_s*thr, గుర్తులేని లోతు)
{
      thr->తీయి_str(లోతు);
}

శూన్య vthread_తీయి_vec4(నిర్మాణం vthread_s*thr, గుర్తులేని లోతు)
{
      thr->తీయి_vec4(లోతు);
}

ద్విగుణ vthread_get_real_దొంతర(నిర్మాణం vthread_s*thr, గుర్తులేని లోతు)
{
      తిరిగి thr->peek_real(లోతు);
}

స్థిరరాశి string&vthread_get_str_దొంతర(నిర్మాణం vthread_s*thr, గుర్తులేని లోతు)
{
      తిరిగి thr->peek_str(లోతు);
}

స్థిరరాశి vvp_vector4_t& vthread_get_vec4_దొంతర(నిర్మాణం vthread_s*thr, గుర్తులేని లోతు)
{
      తిరిగి thr->peek_vec4(లోతు);
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
మూస <తరగతి VVP_వరుస> స్థిర vvp_వరుస*get_వరుస_object(vthread_t thr, vvp_net_t*net)
{
      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      ప్రతిజ్ఞా(obj);

      vvp_వరుస*వరుస = obj->get_object().peek<vvp_వరుస>();
      ఒకవేళ (వరుస == 0) {
	    ప్రతిజ్ఞా(obj->get_object().test_nil());
	    వరుస = కొత్త VVP_వరుస;
	    vvp_object_t val (వరుస);
	    vvp_net_ptr_t ptr (net, 0);
	    vvp_send_object(ptr, val, thr->wt_context);
      }

      తిరిగి వరుస;
}

/*
 * Locator and size/word opcodes use the same VVP net for a queue ($) or a
 * dynamic array ([]). Queues are vvp_queue_vec4; dynamic arrays may be
 * vvp_darray_vec4, vvp_darray_atom<>, etc. (all vvp_darray, but vvp_queue
 * also derives from vvp_darray — test queue vec4 first).
 */
స్థిర శూన్య get_వరుస_or_darray_vec4_from_net(vthread_t thr, vvp_net_t* net,
					      vvp_వరుస_vec4*& qsrc,
					      vvp_darray*& dsrc)
{
      vvp_fun_signal_object* obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      ప్రతిజ్ఞా(obj);
      vvp_object_t శిఖరం = obj->get_object();
      qsrc = శిఖరం.peek<vvp_వరుస_vec4>();
      ఒకవేళ (qsrc) {
	    dsrc = 0;
	    తిరిగి;
      }
      vvp_darray* dany = శిఖరం.peek<vvp_darray>();
      ఒకవేళ (dany && dynamic_cast<vvp_వరుస*>(dany) == 0) {
	    dsrc = dany;
	    తిరిగి;
      }
      ఒకవేళ (శిఖరం.test_nil()) {
	    qsrc = కొత్త vvp_వరుస_vec4();
	    vvp_object_t val(qsrc);
	    vvp_net_ptr_t ptr(net, 0);
	    vvp_send_object(ptr, val, thr->wt_context);
	    dsrc = 0;
	    తిరిగి;
      }
      /* e.g. real/string queue — not vec4 queue or plain darray */
      qsrc = 0;
      dsrc = 0;
}

స్థిర శూన్య get_వరుస_or_darray_real_from_net(vthread_t thr, vvp_net_t* net,
					      vvp_వరుస_real*& qsrc,
					      vvp_darray*& dsrc)
{
      vvp_fun_signal_object* obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      ప్రతిజ్ఞా(obj);
      vvp_object_t శిఖరం = obj->get_object();
      qsrc = శిఖరం.peek<vvp_వరుస_real>();
      ఒకవేళ (qsrc) {
	    dsrc = 0;
	    తిరిగి;
      }
      vvp_darray* dany = శిఖరం.peek<vvp_darray>();
      ఒకవేళ (dany && dynamic_cast<vvp_వరుస*>(dany) == 0) {
	    dsrc = dany;
	    తిరిగి;
      }
      ఒకవేళ (శిఖరం.test_nil()) {
	    qsrc = కొత్త vvp_వరుస_real();
	    vvp_object_t val(qsrc);
	    vvp_net_ptr_t ptr(net, 0);
	    vvp_send_object(ptr, val, thr->wt_context);
	    dsrc = 0;
	    తిరిగి;
      }
      /* e.g. real/string queue — not vec4 queue or plain darray */
      qsrc = 0;
      dsrc = 0;
}

స్థిర శూన్య get_వరుస_or_darray_vec4_from_object(vvp_object_t src_obj,
						 vvp_వరుస_vec4*& qsrc,
						 vvp_darray*& dsrc)
{
      qsrc = src_obj.peek<vvp_వరుస_vec4>();
      ఒకవేళ (qsrc) {
	    dsrc = 0;
	    తిరిగి;
      }
      vvp_darray* dany = src_obj.peek<vvp_darray>();
      ఒకవేళ (dany && dynamic_cast<vvp_వరుస*>(dany) == 0) {
	    dsrc = dany;
	    తిరిగి;
      }
      qsrc = 0;
      dsrc = 0;
}

/*
 * The following are used to allow a common template to be written for
 * queue real/string/vec4 operations
 */
అంతఃవరుస స్థిర శూన్య తీయి_విలువ(vthread_t thr, ద్విగుణ&value, గుర్తులేని)
{
      value = thr->తీయి_real();
}

అంతఃవరుస స్థిర శూన్య తీయి_విలువ(vthread_t thr, string&value, గుర్తులేని)
{
      value = thr->తీయి_str();
}

అంతఃవరుస స్థిర శూన్య తీయి_విలువ(vthread_t thr, vvp_vector4_t&value, గుర్తులేని wid)
{
      value = thr->తీయి_vec4();
      ప్రతిజ్ఞా(value.size() == wid);
}

/*
 * The following are used to allow the queue templates to print correctly.
 */
అంతఃవరుస స్థిర string get_వరుస_type(ద్విగుణ&)
{
      తిరిగి "queue<real>";
}

అంతఃవరుస స్థిర string get_వరుస_type(string&)
{
      తిరిగి "queue<string>";
}

అంతఃవరుస స్థిర string get_వరుస_type(స్థిరరాశి vvp_vector4_t&value)
{
      ostringstream buf;
      buf << "queue<vector[" << value.size() << "]>";
      string res = buf.str();
      తిరిగి res;
}

అంతఃవరుస స్థిర శూన్య print_వరుస_విలువ(ద్విగుణ value)
{
      cerr << value;
}

అంతఃవరుస స్థిర శూన్య print_వరుస_విలువ(స్థిరరాశి string&value)
{
      cerr << "\"" << value << "\"";
}

అంతఃవరుస స్థిర శూన్య print_వరుస_విలువ(స్థిరరాశి vvp_vector4_t&value)
{
      cerr << value;
}

/*
 * The following are used to get a darray/queue default value.
 */
అంతఃవరుస స్థిర శూన్య dq_default(ద్విగుణ&value, గుర్తులేని)
{
      value = 0.0;
}

అంతఃవరుస స్థిర శూన్య dq_default(string&value, గుర్తులేని)
{
      value = "";
}

అంతఃవరుస స్థిర శూన్య dq_default(vvp_vector4_t&value, గుర్తులేని wid)
{
      value = vvp_vector4_t(wid);
}


మూస <తరగతి T> T coerce_గమ్యం_వెడల్పు(స్థిరరాశి T&that, గుర్తులేని width)
{
      ఒకవేళ (that.size() == width)
	    తిరిగి that;

      ప్రతిజ్ఞా(that.size() > width);
      T res (width);
      కొరకు (గుర్తులేని idx = 0 ;  idx < width ;  idx += 1)
	    res.సమితి_బిట్టు(idx, that.value(idx));

      తిరిగి res;
}

/* Explicitly define the vvp_vector4_t version of coerce_to_width(). */
మూస vvp_vector4_t coerce_గమ్యం_వెడల్పు(స్థిరరాశి vvp_vector4_t&that,
                                       గుర్తులేని width);


స్థిర శూన్య multiply_array_imm(గుర్తులేని దీర్ఘ*res, స్థిరరాశి గుర్తులేని దీర్ఘ*val,
			       గుర్తులేని పదాలు, గుర్తులేని దీర్ఘ imm)
{
      కొరకు (గుర్తులేని idx = 0 ; idx < పదాలు ; idx += 1)
	    res[idx] = 0;

      కొరకు (గుర్తులేని mul_idx = 0 ; mul_idx < పదాలు ; mul_idx += 1) {
	    గుర్తులేని దీర్ఘ కూడిక;
	    గుర్తులేని దీర్ఘ tmp = multiply_with_వాహకం(val[mul_idx], imm, కూడిక);

	    గుర్తులేని దీర్ఘ వాహకం = 0;
	    res[mul_idx] = కూడు_with_వాహకం(res[mul_idx], tmp, వాహకం);
	    కొరకు (గుర్తులేని కూడు_idx = mul_idx+1 ; కూడు_idx < పదాలు ; కూడు_idx += 1) {
		  res[కూడు_idx] = కూడు_with_వాహకం(res[కూడు_idx], కూడిక, వాహకం);
		  కూడిక = 0;
	    }
      }
}

/*
 * Allocate a context for use by a child thread. By preference, use
 * the last freed context. If none available, create a new one. Add
 * it to the list of live contexts in that scope.
 */
స్థిర vvp_context_t vthread_alloc_context(__vpiScope*scope)
{
      ప్రతిజ్ఞా(scope->is_automatic());

      vvp_context_t context = scope->free_contexts;
      ఒకవేళ (context) {
            scope->free_contexts = vvp_get_తదుపరి_context(context);
            కొరకు (గుర్తులేని idx = 0 ; idx < scope->nitem ; idx += 1) {
                  scope->అంశం[idx]->reset_instance(context);
            }
      } లేకపోతే {
            context = vvp_allocate_context(scope->nitem);
            కొరకు (గుర్తులేని idx = 0 ; idx < scope->nitem ; idx += 1) {
                  scope->అంశం[idx]->alloc_instance(context);
            }
      }

      vvp_సమితి_తదుపరి_context(context, scope->సజీవ_contexts);
      scope->సజీవ_contexts = context;

      తిరిగి context;
}

/*
 * Free a context previously allocated to a child thread by pushing it
 * onto the freed context stack. Remove it from the list of live contexts
 * in that scope.
 */
స్థిర శూన్య vthread_free_context(vvp_context_t context, __vpiScope*scope)
{
      ప్రతిజ్ఞా(scope->is_automatic());
      ప్రతిజ్ఞా(context);

      ఒకవేళ (context == scope->సజీవ_contexts) {
            scope->సజీవ_contexts = vvp_get_తదుపరి_context(context);
      } లేకపోతే {
            vvp_context_t tmp = scope->సజీవ_contexts;
            అయితే (context != vvp_get_తదుపరి_context(tmp)) {
                  ప్రతిజ్ఞా(tmp);
                  tmp = vvp_get_తదుపరి_context(tmp);
            }
            vvp_సమితి_తదుపరి_context(tmp, vvp_get_తదుపరి_context(context));
      }

      vvp_సమితి_తదుపరి_context(context, scope->free_contexts);
      scope->free_contexts = context;
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
శూన్య contexts_delete(తరగతి __vpiScope*scope)
{
      vvp_context_t context = scope->free_contexts;

      అయితే (context) {
	    scope->free_contexts = vvp_get_తదుపరి_context(context);
	    కొరకు (గుర్తులేని idx = 0; idx < scope->nitem; idx += 1) {
		  scope->అంశం[idx]->free_instance(context);
	    }
	    విడుదల(context);
	    context = scope->free_contexts;
      }
      విడుదల(scope->అంశం);
}
#అయితే_ముగింపు

/*
 * Create a new thread with the given start address.
 */
vthread_t vthread_new(vvp_సంకేతం_t pc, __vpiScope*scope)
{
      vthread_t thr = కొత్త నిర్మాణం vthread_s;
      thr->pc     = pc;
	//thr->bits4  = vvp_vector4_t(32);
      thr->మాతృ = 0;
      thr->మాతృ_scope = scope;
      thr->wait_తదుపరి = 0;
      thr->wt_context = 0;
      thr->rd_context = 0;

      thr->i_am_joining  = 0;
      thr->i_am_detached = 0;
      thr->i_am_నిరీక్షణ  = 0;
      thr->i_am_లోపల_function = 0;
      thr->is_scheduled  = 0;
      thr->i_have_ended  = 0;
      thr->i_was_disabled = 0;
      thr->delay_delete  = 0;
      thr->నిరీక్షణ_for_event = 0;
      thr->event  = 0;
      thr->ecount = 0;

      thr->flags[0] = BIT4_0;
      thr->flags[1] = BIT4_1;
      thr->flags[2] = BIT4_X;
      thr->flags[3] = BIT4_Z;
      కొరకు (పూర్ణాంకం idx = 4 ; idx < 8 ; idx += 1)
	    thr->flags[idx] = BIT4_X;

      scope->దారాలు .insert(thr);
      తిరిగి thr;
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
#ఒకవేళ 0
/*
 * These are not currently correct. If you use them you will get
 * double delete messages. There is still a leak related to a
 * waiting event that needs to be investigated.
 */

స్థిర శూన్య wait_తదుపరి_delete(vthread_t పునాది)
{
      అయితే (పునాది) {
	    vthread_t tmp = పునాది->wait_తదుపరి;
	    తొలగించు పునాది;
	    పునాది = tmp;
	    ఒకవేళ (పునాది->నిరీక్షణ_for_event == 0) ఆపు;
      }
}

స్థిర శూన్య బిడ్డ_delete(vthread_t పునాది)
{
      అయితే (పునాది) {
	    vthread_t tmp = పునాది->బిడ్డ;
	    తొలగించు పునాది;
	    పునాది = tmp;
      }
}
#అయితే_ముగింపు

శూన్య vthreads_delete(తరగతి __vpiScope*scope)
{
      కొరకు (std::సమితి<vthread_t>::iterator cur = scope->దారాలు.begin()
		 ; cur != scope->దారాలు.చివర() ; ++ cur ) {
	    తొలగించు *cur;
      }
      scope->దారాలు.clear();
}
#అయితే_ముగింపు

/*
 * Reaping pulls the thread out of the stack of threads. If I have a
 * child, then hand it over to my parent or fully detach it.
 */
స్థిర శూన్య vthread_reap(vthread_t thr)
{
      ఒకవేళ (! thr->children.empty()) {
	    కొరకు (సమితి<vthread_t>::iterator cur = thr->children.begin()
		       ; cur != thr->children.చివర() ; ++cur) {
		  vthread_t బిడ్డ = *cur;
		  ప్రతిజ్ఞా(బిడ్డ);
		  ప్రతిజ్ఞా(బిడ్డ->మాతృ == thr);
		  బిడ్డ->మాతృ = thr->మాతృ;
	    }
      }
      ఒకవేళ (! thr->detached_children.empty()) {
	    కొరకు (సమితి<vthread_t>::iterator cur = thr->detached_children.begin()
		       ; cur != thr->detached_children.చివర() ; ++cur) {
		  vthread_t బిడ్డ = *cur;
		  ప్రతిజ్ఞా(బిడ్డ);
		  ప్రతిజ్ఞా(బిడ్డ->మాతృ == thr);
		  ప్రతిజ్ఞా(బిడ్డ->i_am_detached);
		  బిడ్డ->మాతృ = 0;
		  బిడ్డ->i_am_detached = 0;
	    }
      }
      ఒకవేళ (thr->మాతృ) {
	      /* assert that the given element was removed. */
	    ఒకవేళ (thr->i_am_detached) {
		  size_t res = thr->మాతృ->detached_children.erase(thr);
		  ప్రతిజ్ఞా(res == 1);
	    } లేకపోతే {
		  size_t res = thr->మాతృ->children.erase(thr);
		  ప్రతిజ్ఞా(res == 1);
	    }
      }

      thr->మాతృ = 0;

	// Remove myself from the containing scope if needed.
      thr->మాతృ_scope->దారాలు.erase(thr);

      thr->pc = codespace_null();

	/* If this thread is not scheduled, then is it safe to delete
	   it now. Otherwise, let the schedule event (which will
	   execute the thread at of_ZOMBIE) delete the object. */
      ఒకవేళ ((thr->is_scheduled == 0) && (thr->నిరీక్షణ_for_event == 0)) {
	    ప్రతిజ్ఞా(thr->children.empty());
	    ప్రతిజ్ఞా(thr->wait_తదుపరి == 0);
	    ఒకవేళ (thr->delay_delete)
		  కాలపట్టిక_del_thr(thr);
	    లేకపోతే
		  vthread_delete(thr);
      }
}

శూన్య vthread_delete(vthread_t thr)
{
      thr->cleanup();
      తొలగించు thr;
}

శూన్య vthread_mark_scheduled(vthread_t thr)
{
      అయితే (thr != 0) {
	    ప్రతిజ్ఞా(thr->is_scheduled == 0);
	    thr->is_scheduled = 1;
	    thr = thr->wait_తదుపరి;
      }
}

శూన్య vthread_mark_final(vthread_t thr)
{
      /*
       * The behavior in a final thread is the same as in a function. Any
       * child thread will be executed immediately rather than being
       * scheduled.
       */
      అయితే (thr != 0) {
	    thr->i_am_లోపల_function = 1;
	    thr = thr->wait_తదుపరి;
      }
}

శూన్య vthread_delay_delete()
{
      ఒకవేళ (running_thread)
	    running_thread->delay_delete = 1;
}

/*
 * This function runs each thread by fetching an instruction,
 * incrementing the PC, and executing the instruction. The thread may
 * be the head of a list, so each thread is run so far as possible.
 */
శూన్య vthread_నిరంతరం(vthread_t thr)
{
      అయితే (thr != 0) {
	    vthread_t tmp = thr->wait_తదుపరి;
	    thr->wait_తదుపరి = 0;

	    ప్రతిజ్ఞా(thr->is_scheduled);
	    thr->is_scheduled = 0;

            running_thread = thr;

	    కొరకు (;;) {
		  vvp_సంకేతం_t cp = thr->pc;
		  thr->pc += 1;

		    /* Run the opcode implementation. If the execution of
		       the opcode returns false, then the thread is meant to
		       be paused, so break out of the loop. */
		  తార్కిక rc = (cp->opcode)(thr, cp);
		  ఒకవేళ (rc == అబద్ధం)
			ఆపు;
	    }

	    thr = tmp;
      }
      running_thread = 0;
}

/*
 * The CHUNK_LINK instruction is a special next pointer for linking
 * chunks of code space. It's like a simplified %jmp.
 */
తార్కిక యొక్క_CHUNK_LINK(vthread_t thr, vvp_సంకేతం_t code)
{
      ప్రతిజ్ఞా(code->cptr);
      thr->pc = code->cptr;
      తిరిగి నిజం;
}

/*
 * This is called by an event functor to wake up all the threads on
 * its list. I in fact created that list in the %wait instruction, and
 * I also am certain that the waiting_for_event flag is set.
 */
శూన్య vthread_కాలపట్టిక_list(vthread_t thr)
{
      కొరకు (vthread_t cur = thr ;  cur ;  cur = cur->wait_తదుపరి) {
	    ప్రతిజ్ఞా(cur->నిరీక్షణ_for_event);
	    cur->నిరీక్షణ_for_event = 0;
      }

      కాలపట్టిక_vthread(thr, 0);
}

vvp_context_t vthread_get_wt_context()
{
      ఒకవేళ (running_thread)
            తిరిగి running_thread->wt_context;
      లేకపోతే
            తిరిగి 0;
}

vvp_context_t vthread_get_rd_context()
{
      ఒకవేళ (running_thread)
            తిరిగి running_thread->rd_context;
      లేకపోతే
            తిరిగి 0;
}

vvp_context_అంశం_t vthread_get_wt_context_అంశం(గుర్తులేని context_idx)
{
      ప్రతిజ్ఞా(running_thread && running_thread->wt_context);
      తిరిగి vvp_get_context_అంశం(running_thread->wt_context, context_idx);
}

vvp_context_అంశం_t vthread_get_rd_context_అంశం(గుర్తులేని context_idx)
{
      ప్రతిజ్ఞా(running_thread && running_thread->rd_context);
      తిరిగి vvp_get_context_అంశం(running_thread->rd_context, context_idx);
}

/*
 * %abs/wr
 */
తార్కిక యొక్క_ABS_WR(vthread_t thr, vvp_సంకేతం_t)
{
      thr->నెట్టు_real( fabs(thr->తీయి_real()) );
      తిరిగి నిజం;
}

తార్కిక యొక్క_ALLOC(vthread_t thr, vvp_సంకేతం_t cp)
{
        /* Allocate a context. */
      vvp_context_t బిడ్డ_context = vthread_alloc_context(cp->scope);

        /* Push the allocated context onto the write context stack. */
      vvp_సమితి_stacked_context(బిడ్డ_context, thr->wt_context);
      thr->wt_context = బిడ్డ_context;

      తిరిగి నిజం;
}

తార్కిక యొక్క_AND(vthread_t thr, vvp_సంకేతం_t)
{
      vvp_vector4_t valb = thr->తీయి_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();
      ప్రతిజ్ఞా(vala.size() == valb.size());
      vala &= valb;
      తిరిగి నిజం;
}

/*
 * This function must ALWAYS be called with the val set to the right
 * size, and initialized with BIT4_0 bits. Certain optimizations rely
 * on that.
 */
స్థిర శూన్య get_తక్షణ_rval(vvp_సంకేతం_t cp, vvp_vector4_t&val)
{
      uint32_t vala = cp->బిట్టు_idx[0];
      uint32_t valb = cp->బిట్టు_idx[1];
      గుర్తులేని wid  = cp->సంఖ్య;

      ఒకవేళ (valb == 0) {
	      // Special case: if the value is zero, we are done
	      // before we start.
	    ఒకవేళ (vala == 0) తిరిగి;

	      // Special case: The value has no X/Z bits, so we can
	      // use the setarray method to write the value all at once.
	    గుర్తులేని use_wid = 8*పరిమాణం(గుర్తులేని దీర్ఘ);
	    ఒకవేళ (wid < use_wid)
		  use_wid = wid;
	    గుర్తులేని దీర్ఘ tmp[1];
	    tmp[0] = vala;
	    val.setarray(0, use_wid, tmp);
	    తిరిగి;
      }

	// The immediate value can be values bigger then 32 bits, but
	// only if the high bits are zero. So at most we need to run
	// through the loop below 32 times. Maybe less, if the target
	// width is less. We don't have to do anything special on that
	// because vala/valb bits will shift away so (vala|valb) will
	// turn to zero at or before 32 shifts.

      కొరకు (గుర్తులేని idx = 0 ; idx < wid && (vala|valb) ; idx += 1) {
	    uint32_t ba = 0;
	      // Convert the vala/valb bits to a ba number that
	      // matches the encoding of the vvp_bit4_t enumeration.
	    ba = (valb & 1) << 1;
	    ba |= vala & 1;

	      // Note that the val is already pre-filled with BIT4_0
	      // bits, os we only need to set non-zero bit values.
	    ఒకవేళ (ba) val.సమితి_బిట్టు(idx, (vvp_bit4_t)ba);

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
తార్కిక యొక్క_కూడు(vthread_t thr, vvp_సంకేతం_t)
{
      vvp_vector4_t r = thr->తీయి_vec4();
	// Rather then pop l, use it directly from the stack. When we
	// assign to 'l', that will edit the top of the stack, which
	// replaces a pop and a pull.
      vvp_vector4_t&l = thr->peek_vec4();

      l.కూడు(r);

      తిరిగి నిజం;
}

/*
 * %addi <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
తార్కిక యొక్క_ADDI(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని wid = cp->సంఖ్య;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_తక్షణ_rval (cp, r);

      l.కూడు(r);

      తిరిగి నిజం;
}

/*
 * %add/wr
 */
తార్కిక యొక్క_కూడు_WR(vthread_t thr, vvp_సంకేతం_t)
{
      ద్విగుణ r = thr->తీయి_real();
      ద్విగుణ l = thr->తీయి_real();
      thr->నెట్టు_real(l + r);
      తిరిగి నిజం;
}

/* %assign/ar <array>, <delay>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory. <delay>
 * is the delay in simulation time. <bit> is the index register
 * containing the real value.
 */
తార్కిక యొక్క_కేటాయించు_AR(vthread_t thr, vvp_సంకేతం_t cp)
{
      దీర్ఘ adr = thr->పదాలు[3].w_int;
      గుర్తులేని delay = cp->బిట్టు_idx[0];
      ద్విగుణ value = thr->తీయి_real();

      ఒకవేళ (adr >= 0) {
	    కాలపట్టిక_కేటాయించు_array_మాట(cp->array, adr, value, delay);
      }

      తిరిగి నిజం;
}

/* %assign/ar/d <array>, <delay_idx>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory.
 * <delay_idx> is the integer register that contains the delay value.
 */
తార్కిక యొక్క_కేటాయించు_ARD(vthread_t thr, vvp_సంకేతం_t cp)
{
      దీర్ఘ adr = thr->పదాలు[3].w_int;
      ద్విగుణ value = thr->తీయి_real();

      ఒకవేళ (adr >= 0) {
	    vvp_time64_t delay = thr->పదాలు[cp->బిట్టు_idx[0]].w_uint;
	    కాలపట్టిక_కేటాయించు_array_మాట(cp->array, adr, value, delay);
      }

      తిరిగి నిజం;
}

/* %assign/ar/e <array>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory. <bit>
 * is the index register containing the real value. The event
 * information is contained in the thread event control registers
 * and is set with %evctl.
 */
తార్కిక యొక్క_కేటాయించు_ARE(vthread_t thr, vvp_సంకేతం_t cp)
{
      దీర్ఘ adr = thr->పదాలు[3].w_int;
      ద్విగుణ value = thr->తీయి_real();

      ఒకవేళ (adr >= 0) {
	    ఒకవేళ (thr->ecount == 0) {
		  కాలపట్టిక_కేటాయించు_array_మాట(cp->array, adr, value, 0);
	    } లేకపోతే {
		  కాలపట్టిక_evctl(cp->array, adr, value, thr->event,
		                 thr->ecount);
	    }
      }

      తిరిగి నిజం;
}

/*
 * %assign/vec4 <var>, <delay>
 */
తార్కిక యొక్క_కేటాయించు_VEC4(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      గుర్తులేని delay = cp->బిట్టు_idx[0];
      స్థిరరాశి vvp_vector4_t&val = thr->peek_vec4();

      కాలపట్టిక_కేటాయించు_సదిశ(ptr, 0, 0, val, delay);
      thr->తీయి_vec4(1);
      తిరిగి నిజం;
}

/*
 * Resizes a vector value for a partial assignment so that the value is fully
 * in-bounds of the target signal. Both `val` and `off` will be updated if
 * necessary.
 *
 * Returns false if the value is fully out-of-bounds and the assignment should
 * be skipped. Otherwise returns true.
 */
స్థిర తార్కిక resize_rval_vec(vvp_vector4_t &val, int64_t &off,
			    గుర్తులేని పూర్ణాంకం sig_wid)
{
      గుర్తులేని పూర్ణాంకం wid = val.size();

        // Fully in bounds, most likely case
      ఒకవేళ (off >= 0 && (uint64_t)off + wid <= sig_wid)
	    తిరిగి నిజం;

      గుర్తులేని పూర్ణాంకం పునాది = 0;
      ఒకవేళ (off >= 0) {
	      // Fully out-of-bounds
	    ఒకవేళ ((uint64_t)off >= sig_wid)
		  తిరిగి అబద్ధం;
      } లేకపోతే {
	      // Fully out-of-bounds */
	    ఒకవేళ ((uint64_t)(-off) >= wid)
		  తిరిగి అబద్ధం;

	      // If the index is below the vector, then only assign the high
	      // bits that overlap with the target
	    పునాది = -off;
	    wid += off;
		off = 0;
      }

	// If the value is partly above the target, then only assign
	// the bits that overlap
      ఒకవేళ ((uint64_t)off + wid > sig_wid)
	    wid = sig_wid - (uint64_t)off;

      val = val.subvalue(పునాది, wid);

      తిరిగి నిజం;
}

/*
 * %assign/vec4/a/d <arr>, <offx>, <delx>
 */
తార్కిక యొక్క_కేటాయించు_VEC4_A_D(vthread_t thr, vvp_సంకేతం_t cp)
{
      పూర్ణాంకం off_idx = cp->బిట్టు_idx[0];
      పూర్ణాంకం del_idx = cp->బిట్టు_idx[1];
      పూర్ణాంకం adr_idx = 3;

      int64_t  off = off_idx ? thr->పదాలు[off_idx].w_int : 0;
      vvp_time64_t del = del_idx? thr->పదాలు[del_idx].w_uint : 0;
      దీర్ఘ     adr = thr->పదాలు[adr_idx].w_int;

      vvp_vector4_t val = thr->తీయి_vec4();

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      ఒకవేళ (thr->flags[4] == BIT4_1)
	    తిరిగి నిజం;

      ఒకవేళ (!resize_rval_vec(val, off, cp->array->get_మాట_కొలత()))
	    తిరిగి నిజం;

      కాలపట్టిక_కేటాయించు_array_మాట(cp->array, adr, off, val, del);

      తిరిగి నిజం;
}

/*
 * %assign/vec4/a/e <arr>, <offx>
 */
తార్కిక యొక్క_కేటాయించు_VEC4_A_E(vthread_t thr, vvp_సంకేతం_t cp)
{
      పూర్ణాంకం off_idx = cp->బిట్టు_idx[0];
      పూర్ణాంకం adr_idx = 3;

      int64_t  off = off_idx ? thr->పదాలు[off_idx].w_int : 0;
      దీర్ఘ     adr = thr->పదాలు[adr_idx].w_int;

      vvp_vector4_t val = thr->తీయి_vec4();

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      ఒకవేళ (thr->flags[4] == BIT4_1)
	    తిరిగి నిజం;

      ఒకవేళ (!resize_rval_vec(val, off, cp->array->get_మాట_కొలత()))
	    తిరిగి నిజం;

      ఒకవేళ (thr->ecount == 0) {
	    కాలపట్టిక_కేటాయించు_array_మాట(cp->array, adr, off, val, 0);
      } లేకపోతే {
	    కాలపట్టిక_evctl(cp->array, adr, val, off, thr->event, thr->ecount);
      }

      తిరిగి నిజం;
}

/*
 * %assign/vec4/off/d <var>, <off>, <del>
 */
తార్కిక యొక్క_కేటాయించు_VEC4_OFF_D(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      గుర్తులేని off_సూచీ = cp->బిట్టు_idx[0];
      గుర్తులేని del_సూచీ = cp->బిట్టు_idx[1];
      vvp_vector4_t val = thr->తీయి_vec4();

      int64_t off = thr->పదాలు[off_సూచీ].w_int;
      vvp_time64_t del = thr->పదాలు[del_సూచీ].w_uint;

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      ఒకవేళ (thr->flags[4] == BIT4_1)
	    తిరిగి నిజం;

      vvp_signal_విలువ*sig = dynamic_cast<vvp_signal_విలువ*> (cp->net->fil);
      ప్రతిజ్ఞా(sig);

      ఒకవేళ (!resize_rval_vec(val, off, sig->విలువ_కొలత()))
	    తిరిగి నిజం;

      కాలపట్టిక_కేటాయించు_సదిశ(ptr, off, sig->విలువ_కొలత(), val, del);
      తిరిగి నిజం;
}

/*
 * %assign/vec4/off/e <var>, <off>
 */
తార్కిక యొక్క_కేటాయించు_VEC4_OFF_E(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      గుర్తులేని off_సూచీ = cp->బిట్టు_idx[0];
      vvp_vector4_t val = thr->తీయి_vec4();

      int64_t off = thr->పదాలు[off_సూచీ].w_int;

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      ఒకవేళ (thr->flags[4] == BIT4_1)
	    తిరిగి నిజం;

      vvp_signal_విలువ*sig = dynamic_cast<vvp_signal_విలువ*> (cp->net->fil);
      ప్రతిజ్ఞా(sig);

      ఒకవేళ (!resize_rval_vec(val, off, sig->విలువ_కొలత()))
	    తిరిగి నిజం;

      ఒకవేళ (thr->ecount == 0) {
	    కాలపట్టిక_కేటాయించు_సదిశ(ptr, off, sig->విలువ_కొలత(), val, 0);
      } లేకపోతే {
	    కాలపట్టిక_evctl(ptr, val, off, sig->విలువ_కొలత(), thr->event, thr->ecount);
      }

      తిరిగి నిజం;
}

/*
 * %assign/vec4/d <var-label> <delay>
 */
తార్కిక యొక్క_కేటాయించు_VEC4_D(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      గుర్తులేని del_సూచీ = cp->బిట్టు_idx[0];
      vvp_time64_t del = thr->పదాలు[del_సూచీ].w_int;

      vvp_vector4_t value = thr->తీయి_vec4();

      vvp_signal_విలువ*sig = dynamic_cast<vvp_signal_విలువ*> (cp->net->fil);
      ప్రతిజ్ఞా(sig);

      కాలపట్టిక_కేటాయించు_సదిశ(ptr, 0, sig->విలువ_కొలత(), value, del);

      తిరిగి నిజం;
}

/*
 * %assign/vec4/e <var-label>
 */
తార్కిక యొక్క_కేటాయించు_VEC4_E(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_vector4_t value = thr->తీయి_vec4();

      vvp_signal_విలువ*sig = dynamic_cast<vvp_signal_విలువ*> (cp->net->fil);
      ప్రతిజ్ఞా(sig);

      ఒకవేళ (thr->ecount == 0) {
	    కాలపట్టిక_కేటాయించు_సదిశ(ptr, 0, sig->విలువ_కొలత(), value, 0);
      } లేకపోతే {
	    కాలపట్టిక_evctl(ptr, value, 0, sig->విలువ_కొలత(), thr->event, thr->ecount);
      }

      తిరిగి నిజం;
}

/*
 * This is %assign/wr <vpi-label>, <delay>
 *
 * This assigns (after a delay) a value to a real variable. Use the
 * vpi_put_value function to do the assign, with the delay written
 * into the vpiInertialDelay carrying the desired delay.
 */
తార్కిక యొక్క_కేటాయించు_WR(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని delay = cp->బిట్టు_idx[0];
      ద్విగుణ value = thr->తీయి_real();
      s_vpi_సమయం del;

      del.type = vpi_Sim_సమయం;
      vpip_సమయం_గమ్యం_timestruct(&del, delay);

      __vpiHandle*tmp = cp->handle;

      t_vpi_విలువ val;
      val.format = vpiRealVal;
      val.value.real = value;
      vpi_put_విలువ(tmp, &val, &del, vpiTransportDelay);

      తిరిగి నిజం;
}

తార్కిక యొక్క_కేటాయించు_WRD(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_time64_t delay = thr->పదాలు[cp->బిట్టు_idx[0]].w_uint;
      ద్విగుణ value = thr->తీయి_real();
      s_vpi_సమయం del;

      del.type = vpi_Sim_సమయం;
      vpip_సమయం_గమ్యం_timestruct(&del, delay);

      __vpiHandle*tmp = cp->handle;

      t_vpi_విలువ val;
      val.format = vpiRealVal;
      val.value.real = value;
      vpi_put_విలువ(tmp, &val, &del, vpiTransportDelay);

      తిరిగి నిజం;
}

తార్కిక యొక్క_కేటాయించు_WRE(vthread_t thr, vvp_సంకేతం_t cp)
{
      ప్రతిజ్ఞా(thr->event != 0);
      ద్విగుణ value = thr->తీయి_real();
      __vpiHandle*tmp = cp->handle;

	// If the count is zero then just put the value.
      ఒకవేళ (thr->ecount == 0) {
	    t_vpi_విలువ val;

	    val.format = vpiRealVal;
	    val.value.real = value;
	    vpi_put_విలువ(tmp, &val, 0, vpiNoDelay);
      } లేకపోతే {
	    కాలపట్టిక_evctl(tmp, value, thr->event, thr->ecount);
      }

      thr->event = 0;
      thr->ecount = 0;

      తిరిగి నిజం;
}

తార్కిక యొక్క_BLEND(vthread_t thr, vvp_సంకేతం_t)
{
      vvp_vector4_t vala = thr->తీయి_vec4();
      vvp_vector4_t valb = thr->తీయి_vec4();
      ప్రతిజ్ఞా(vala.size() == valb.size());

      కొరకు (గుర్తులేని idx = 0 ; idx < vala.size() ; idx += 1) {
	    ఒకవేళ (vala.value(idx) == valb.value(idx))
		  కొనసాగు;

	    vala.సమితి_బిట్టు(idx, BIT4_X);
      }

      thr->నెట్టు_vec4(vala);
      తిరిగి నిజం;
}

తార్కిక యొక్క_BLEND_STR(vthread_t thr, vvp_సంకేతం_t)
{
      string f = thr->తీయి_str();
      string t = thr->తీయి_str();
      thr->నెట్టు_str((t == f) ? t : "");
      తిరిగి నిజం;
}

తార్కిక యొక్క_BLEND_WR(vthread_t thr, vvp_సంకేతం_t)
{
      ద్విగుణ f = thr->తీయి_real();
      ద్విగుణ t = thr->తీయి_real();
      thr->నెట్టు_real((t == f) ? t : 0.0);
      తిరిగి నిజం;
}

తార్కిక యొక్క_BREAKPOINT(vthread_t, vvp_సంకేతం_t)
{
      తిరిగి నిజం;
}

/*
 * %callf/void <code-label>, <scope-label>
 * Combine the %fork and %join steps for invoking a function.
 */
స్థిర తార్కిక do_callf_void(vthread_t thr, vthread_t బిడ్డ)
{

      ఒకవేళ (బిడ్డ->మాతృ_scope->is_automatic()) {
	      /* The context allocated for this child is the top entry
		 on the write context stack */
	    బిడ్డ->wt_context = thr->wt_context;
	    బిడ్డ->rd_context = thr->wt_context;
      }

        // Mark the function thread as a direct child of the current thread.
      బిడ్డ->మాతృ = thr;
      thr->children.insert(బిడ్డ);
        // This should be the only child
      ప్రతిజ్ఞా(thr->children.size()==1);

        // Execute the function. This SHOULD run the function to completion,
        // but there are some exceptional situations where it won't.
      ప్రతిజ్ఞా(బిడ్డ->మాతృ_scope->get_type_సంకేతం() == vpiFunction);
      బిడ్డ->is_scheduled = 1;
      బిడ్డ->i_am_లోపల_function = 1;
      vthread_నిరంతరం(బిడ్డ);
      running_thread = thr;

      ఒకవేళ (బిడ్డ->i_have_ended) {
	    do_join(thr, బిడ్డ);
	    తిరిగి నిజం;
      } లేకపోతే {
	    thr->i_am_joining = 1;
	    తిరిగి అబద్ధం;
      }
}

తార్కిక యొక్క_CALLF_OBJ(vthread_t thr, vvp_సంకేతం_t cp)
{
      vthread_t బిడ్డ = vthread_new(cp->cptr2, cp->scope);
      తిరిగి do_callf_void(thr, బిడ్డ);

      // XXXX NOT IMPLEMENTED
}

తార్కిక యొక్క_CALLF_REAL(vthread_t thr, vvp_సంకేతం_t cp)
{
      vthread_t బిడ్డ = vthread_new(cp->cptr2, cp->scope);

	// This is the return value. Push a place-holder value. The function
	// will replace this with the actual value using a %ret/real instruction.
      thr->నెట్టు_real(0.0);
      బిడ్డ->వాదనలు_real.push_back(0);

      తిరిగి do_callf_void(thr, బిడ్డ);
}

తార్కిక యొక్క_CALLF_STR(vthread_t thr, vvp_సంకేతం_t cp)
{
      vthread_t బిడ్డ = vthread_new(cp->cptr2, cp->scope);

      thr->నెట్టు_str("");
      బిడ్డ->వాదనలు_str.push_back(0);

      తిరిగి do_callf_void(thr, బిడ్డ);
}

తార్కిక యొక్క_CALLF_VEC4(vthread_t thr, vvp_సంకేతం_t cp)
{
      vthread_t బిడ్డ = vthread_new(cp->cptr2, cp->scope);

      vpiScopeFunction*scope_func = dynamic_cast<vpiScopeFunction*>(cp->scope);
      ప్రతిజ్ఞా(scope_func);

	// This is the return value. Push a place-holder value. The function
	// will replace this with the actual value using a %ret/real instruction.
      thr->నెట్టు_vec4(vvp_vector4_t(scope_func->get_func_వెడల్పు(), scope_func->get_func_init_val()));
      బిడ్డ->వాదనలు_vec4.push_back(0);

      తిరిగి do_callf_void(thr, బిడ్డ);
}

తార్కిక యొక్క_CALLF_VOID(vthread_t thr, vvp_సంకేతం_t cp)
{
      vthread_t బిడ్డ = vthread_new(cp->cptr2, cp->scope);
      తిరిగి do_callf_void(thr, బిడ్డ);
}

/*
 * The %cassign/link instruction connects a source node to a
 * destination node. The destination node must be a signal, as it is
 * marked with the source of the cassign so that it may later be
 * unlinked without specifically knowing the source that this
 * instruction used.
 */
తార్కిక యొక్క_CASSIGN_LINK(vthread_t, vvp_సంకేతం_t cp)
{
      vvp_net_t*dst = cp->net;
      vvp_net_t*src = cp->net2;

      vvp_fun_signal_పునాది*sig
	    = dynamic_cast<vvp_fun_signal_పునాది*>(dst->fun);
      ప్రతిజ్ఞా(sig);

	/* Any previous continuous assign should have been removed already. */
      ప్రతిజ్ఞా(sig->cassign_link == 0);

      sig->cassign_link = src;

	/* Link the output of the src to the port[1] (the cassign
	   port) of the destination. */
      vvp_net_ptr_t dst_ptr (dst, 1);
      src->link(dst_ptr);

      తిరిగి నిజం;
}

/*
 * If there is an existing continuous assign linked to the destination
 * node, unlink it. This must be done before applying a new continuous
 * assign, otherwise the initial assigned value will be propagated to
 * any other nodes driven by the old continuous assign source.
 */
స్థిర శూన్య cassign_unlink(vvp_net_t*dst)
{
      vvp_fun_signal_పునాది*sig
	    = dynamic_cast<vvp_fun_signal_పునాది*>(dst->fun);
      ప్రతిజ్ఞా(sig);

      ఒకవేళ (sig->cassign_link == 0)
	    తిరిగి;

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
తార్కిక యొక్క_CASSIGN_VEC4(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_net_t*net = cp->net;
      vvp_vector4_t value = thr->తీయి_vec4();

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

	/* Set the value into port 1 of the destination. */
      vvp_net_ptr_t ptr (net, 1);
      vvp_send_vec4(ptr, value, 0);

      తిరిగి నిజం;
}

/*
 * %cassign/vec4/off <var>, <off>
 */
తార్కిక యొక్క_CASSIGN_VEC4_OFF(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_net_t*net = cp->net;
      గుర్తులేని పునాది_idx = cp->బిట్టు_idx[0];
      దీర్ఘ పునాది = thr->పదాలు[పునాది_idx].w_int;
      vvp_vector4_t value = thr->తీయి_vec4();
      గుర్తులేని wid = value.size();

      ఒకవేళ (thr->flags[4] == BIT4_1)
	    తిరిగి నిజం;

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

      vvp_signal_విలువ*sig = dynamic_cast<vvp_signal_విలువ*> (net->fil);
      ప్రతిజ్ఞా(sig);

      ఒకవేళ (పునాది < 0 && (wid <= (గుర్తులేని)-పునాది))
	    తిరిగి నిజం;

      ఒకవేళ (పునాది >= (దీర్ఘ)sig->విలువ_కొలత())
	    తిరిగి నిజం;

      ఒకవేళ (పునాది < 0) {
	    wid -= (గుర్తులేని) -పునాది;
	    పునాది = 0;
	    value.resize(wid);
      }

      ఒకవేళ (పునాది+wid > sig->విలువ_కొలత()) {
	    wid = sig->విలువ_కొలత() - పునాది;
	    value.resize(wid);
      }

      vvp_net_ptr_t ptr (net, 1);
      vvp_send_vec4_pv(ptr, value, పునాది, sig->విలువ_కొలత(), 0);
      తిరిగి నిజం;
}

తార్కిక యొక్క_CASSIGN_WR(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_net_t*net  = cp->net;
      ద్విగుణ value = thr->తీయి_real();

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

	/* Set the value into port 1 of the destination. */
      vvp_net_ptr_t ptr (net, 1);
      vvp_send_real(ptr, value, 0);

      తిరిగి నిజం;
}

/*
 * %cast2
 */
తార్కిక యొక్క_CAST2(vthread_t thr, vvp_సంకేతం_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      గుర్తులేని wid = val.size();

      కొరకు (గుర్తులేని idx = 0 ; idx < wid ; idx += 1) {
	    ఎంపిక (val.value(idx)) {
		స్థితి BIT4_0:
		స్థితి BIT4_1:
		  ఆపు;
		అప్రమేయ:
		  val.సమితి_బిట్టు(idx, BIT4_0);
		  ఆపు;
	    }
      }

      తిరిగి నిజం;
}

తార్కిక do_cast_vec_dar(vthread_t thr, vvp_సంకేతం_t cp, తార్కిక as_vec4)
{
      గుర్తులేని wid = cp->సంఖ్య;

      vvp_object_t obj;
      thr->తీయి_object(obj);

      vvp_darray*darray = obj.peek<vvp_darray>();
      ప్రతిజ్ఞా(darray);

      vvp_vector4_t vec = darray->get_bitstream(as_vec4);
      ఒకవేళ (vec.size() != wid) {
	    cerr << thr->get_fileline()
	         << "VVP error: size mismatch when casting dynamic array to vector." << పంక్తి_ముగింపు;
            thr->నెట్టు_vec4(vvp_vector4_t(wid));
            కాలపట్టిక_stop(0);
            తిరిగి అబద్ధం;
      }
      thr->నెట్టు_vec4(vec);
      తిరిగి నిజం;
}

/*
 * %cast/vec2/dar <wid>
 */
తార్కిక యొక్క_CAST_VEC2_DAR(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి do_cast_vec_dar(thr, cp, అబద్ధం);
}

/*
 * %cast/vec4/dar <wid>
 */
తార్కిక యొక్క_CAST_VEC4_DAR(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి do_cast_vec_dar(thr, cp, నిజం);
}

/*
 * %cast/vec4/str <wid>
 */
తార్కిక యొక్క_CAST_VEC4_STR(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని wid = cp->సంఖ్య;
      string str = thr->తీయి_str();

      vvp_vector4_t vec(wid, BIT4_0);

      ఒకవేళ (wid != 8*str.length()) {
	    cerr << thr->get_fileline()
	         << "VVP error: size mismatch when casting string to vector." << పంక్తి_ముగింపు;
            thr->నెట్టు_vec4(vec);
            కాలపట్టిక_stop(0);
            తిరిగి అబద్ధం;
      }

      గుర్తులేని sdx = 0;
      గుర్తులేని vdx = wid;
      అయితే (vdx > 0) {
            అక్షరం ch = str[sdx++];
            vdx -= 8;
            కొరకు (గుర్తులేని bdx = 0; bdx < 8; bdx += 1) {
                  ఒకవేళ (ch & 1)
                        vec.సమితి_బిట్టు(vdx+bdx, BIT4_1);
                  ch >>= 1;
            }
      }

      thr->నెట్టు_vec4(vec);
      తిరిగి నిజం;
}

స్థిర శూన్య do_CMPE(vthread_t thr, స్థిరరాశి vvp_vector4_t&lval, స్థిరరాశి vvp_vector4_t&rval)
{
      ప్రతిజ్ఞా(rval.size() == lval.size());

      ఒకవేళ (lval.has_xz() || rval.has_xz()) {

	    గుర్తులేని wid = lval.size();
	    vvp_bit4_t eq  = BIT4_1;
	    vvp_bit4_t eeq = BIT4_1;

	    కొరకు (గుర్తులేని idx = 0 ; idx < wid ; idx += 1) {
		  vvp_bit4_t lv = lval.value(idx);
		  vvp_bit4_t rv = rval.value(idx);

		  ఒకవేళ (lv != rv)
			eeq = BIT4_0;

		  ఒకవేళ (eq==BIT4_1 && (bit4_is_xz(lv) || bit4_is_xz(rv)))
			eq = BIT4_X;
		  ఒకవేళ ((lv == BIT4_0) && (rv==BIT4_1))
			eq = BIT4_0;
		  ఒకవేళ ((lv == BIT4_1) && (rv==BIT4_0))
			eq = BIT4_0;

		  ఒకవేళ (eq == BIT4_0)
			ఆపు;
	    }

	    thr->flags[4] = eq;
	    thr->flags[6] = eeq;

      } లేకపోతే {
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
తార్కిక యొక్క_CMPE(vthread_t thr, vvp_సంకేతం_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      స్థిరరాశి vvp_vector4_t&rval = thr->peek_vec4(0);
      స్థిరరాశి vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPE(thr, lval, rval);

      thr->తీయి_vec4(2);
      తిరిగి నిజం;
}

తార్కిక యొక్క_CMPNE(vthread_t thr, vvp_సంకేతం_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      స్థిరరాశి vvp_vector4_t&rval = thr->peek_vec4(0);
      స్థిరరాశి vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];
      thr->flags[6] =  ~thr->flags[6];

      thr->తీయి_vec4(2);
      తిరిగి నిజం;
}

/*
 * %cmpi/e <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
తార్కిక యొక్క_CMPIE(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని wid = cp->సంఖ్య;

      స్థిరరాశి vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_తక్షణ_rval (cp, rval);

      do_CMPE(thr, lval, rval);

      thr->తీయి_vec4(1);
      తిరిగి నిజం;
}

తార్కిక యొక్క_CMPINE(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని wid = cp->సంఖ్య;

      స్థిరరాశి vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_తక్షణ_rval (cp, rval);

      do_CMPE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];
      thr->flags[6] =  ~thr->flags[6];

      thr->తీయి_vec4(1);
      తిరిగి నిజం;
}



స్థిర శూన్య do_CMPS(vthread_t thr, స్థిరరాశి vvp_vector4_t&lval, స్థిరరాశి vvp_vector4_t&rval)
{
      ప్రతిజ్ఞా(rval.size() == lval.size());

	// If either value has XZ bits, then the eq and lt values are
	// known already to be X. Just calculate the eeq result as a
	// special case and short circuit the rest of the compare.
      ఒకవేళ (lval.has_xz() || rval.has_xz()) {
	    thr->flags[4] = BIT4_X; // eq
	    thr->flags[5] = BIT4_X; // lt
	    thr->flags[6] = lval.eeq(rval)? BIT4_1 : BIT4_0;
	    తిరిగి;
      }

	// Past this point, we know we are dealing only with fully
	// defined values.
      గుర్తులేని wid = lval.size();

      స్థిరరాశి vvp_bit4_t sig1 = lval.value(wid-1);
      స్థిరరాశి vvp_bit4_t sig2 = rval.value(wid-1);

	// If the lval is <0 and the rval is >=0, then we know the result.
      ఒకవేళ ((sig1 == BIT4_1) && (sig2 == BIT4_0)) {
	    thr->flags[4] = BIT4_0; // eq;
	    thr->flags[5] = BIT4_1; // lt;
	    thr->flags[6] = BIT4_0; // eeq
	    తిరిగి;
      }

	// If the lval is >=0 and the rval is <0, then we know the result.
      ఒకవేళ ((sig1 == BIT4_0) && (sig2 == BIT4_1)) {
	    thr->flags[4] = BIT4_0; // eq;
	    thr->flags[5] = BIT4_0; // lt;
	    thr->flags[6] = BIT4_0; // eeq
	    తిరిగి;
      }

	// The values have the same sign, so we have to look at the
	// actual value. Scan from the MSB down. As soon as we find a
	// bit that differs, we know the result.

      కొరకు (గుర్తులేని idx = 1 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t lv = lval.value(wid-1-idx);
	    vvp_bit4_t rv = rval.value(wid-1-idx);

	    ఒకవేళ (lv == rv)
		  కొనసాగు;

	    thr->flags[4] = BIT4_0; // eq
	    thr->flags[6] = BIT4_0; // eeq

	    ఒకవేళ (lv==BIT4_0) {
		  thr->flags[5] = BIT4_1; // lt
	    } లేకపోతే {
		  thr->flags[5] = BIT4_0; // lt
	    }
	    తిరిగి;
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
తార్కిక యొక్క_CMPS(vthread_t thr, vvp_సంకేతం_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      స్థిరరాశి vvp_vector4_t&rval = thr->peek_vec4(0);
      స్థిరరాశి vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPS(thr, lval, rval);

      thr->తీయి_vec4(2);
      తిరిగి నిజం;
}

/*
 * %cmpi/s <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
తార్కిక యొక్క_CMPIS(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని wid = cp->సంఖ్య;

      స్థిరరాశి vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_తక్షణ_rval (cp, rval);

      do_CMPS(thr, lval, rval);

      thr->తీయి_vec4(1);
      తిరిగి నిజం;
}

తార్కిక యొక్క_CMPSTR(vthread_t thr, vvp_సంకేతం_t)
{
      string re = thr->తీయి_str();
      string le = thr->తీయి_str();

      పూర్ణాంకం rc = strcmp(le.c_str(), re.c_str());

      vvp_bit4_t eq;
      vvp_bit4_t lt;

      ఒకవేళ (rc == 0) {
	    eq = BIT4_1;
	    lt = BIT4_0;
      } లేకపోతే ఒకవేళ (rc < 0) {
	    eq = BIT4_0;
	    lt = BIT4_1;
      } లేకపోతే {
	    eq = BIT4_0;
	    lt = BIT4_0;
      }

      thr->flags[4] = eq;
      thr->flags[5] = lt;

      తిరిగి నిజం;
}

స్థిర శూన్య యొక్క_CMPU_the_hard_way(vthread_t thr, గుర్తులేని wid,
				 స్థిరరాశి vvp_vector4_t&lval,
				 స్థిరరాశి vvp_vector4_t&rval)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_bit4_t eeq = BIT4_1;

      కొరకు (గుర్తులేని idx = 0 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t lv = lval.value(idx);
	    vvp_bit4_t rv = rval.value(idx);

	    ఒకవేళ (lv != rv)
		  eeq = BIT4_0;

	    ఒకవేళ (eq==BIT4_1 && (bit4_is_xz(lv) || bit4_is_xz(rv)))
		  eq = BIT4_X;
	    ఒకవేళ ((lv == BIT4_0) && (rv==BIT4_1))
		  eq = BIT4_0;
	    ఒకవేళ ((lv == BIT4_1) && (rv==BIT4_0))
		  eq = BIT4_0;

	    ఒకవేళ (eq == BIT4_0)
		  ఆపు;

      }

      thr->flags[4] = eq;
      thr->flags[5] = BIT4_X;
      thr->flags[6] = eeq;
}

స్థిర శూన్య do_CMPU(vthread_t thr, స్థిరరాశి vvp_vector4_t&lval, స్థిరరాశి vvp_vector4_t&rval)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_bit4_t lt = BIT4_0;

      ఒకవేళ (rval.size() != lval.size()) {
	    cerr << thr->get_fileline()
	         << "VVP ERROR: %cmp/u operand width mismatch: lval=" << lval
		 << ", rval=" << rval << పంక్తి_ముగింపు;
      }
      ప్రతిజ్ఞా(rval.size() == lval.size());
      గుర్తులేని wid = lval.size();

      గుర్తులేని దీర్ఘ*larray = lval.subarray(0,wid);
      ఒకవేళ (larray == 0) తిరిగి యొక్క_CMPU_the_hard_way(thr, wid, lval, rval);

      గుర్తులేని దీర్ఘ*rarray = rval.subarray(0,wid);
      ఒకవేళ (rarray == 0) {
	    తొలగించు[]larray;
	    తిరిగి యొక్క_CMPU_the_hard_way(thr, wid, lval, rval);
      }

      గుర్తులేని పదాలు = (wid+CPU_మాట_బిట్లు-1) / CPU_మాట_బిట్లు;

      కొరకు (గుర్తులేని wdx = 0 ; wdx < పదాలు ; wdx += 1) {
	    ఒకవేళ (larray[wdx] == rarray[wdx])
		  కొనసాగు;

	    eq = BIT4_0;
	    ఒకవేళ (larray[wdx] < rarray[wdx])
		  lt = BIT4_1;
	    లేకపోతే
		  lt = BIT4_0;
      }

      తొలగించు[]larray;
      తొలగించు[]rarray;

      thr->flags[4] = eq;
      thr->flags[5] = lt;
      thr->flags[6] = eq;
}

తార్కిక యొక్క_CMPU(vthread_t thr, vvp_సంకేతం_t)
{

      స్థిరరాశి vvp_vector4_t&rval = thr->peek_vec4(0);
      స్థిరరాశి vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPU(thr, lval, rval);

      thr->తీయి_vec4(2);
      తిరిగి నిజం;
}

/*
 * %cmpi/u <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
తార్కిక యొక్క_CMPIU(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని wid = cp->సంఖ్య;

      స్థిరరాశి vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_తక్షణ_rval (cp, rval);

      do_CMPU(thr, lval, rval);

      thr->తీయి_vec4(1);
      తిరిగి నిజం;
}


/*
 * %cmp/x
 */
తార్కిక యొక్క_CMPX(vthread_t thr, vvp_సంకేతం_t)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_vector4_t rval = thr->తీయి_vec4();
      vvp_vector4_t lval = thr->తీయి_vec4();

      ప్రతిజ్ఞా(rval.size() == lval.size());
      గుర్తులేని wid = lval.size();

      కొరకు (గుర్తులేని idx = 0 ; idx < wid ; idx += 1) {
	    vvp_bit4_t lv = lval.value(idx);
	    vvp_bit4_t rv = rval.value(idx);
	    ఒకవేళ ((lv != rv) && !bit4_is_xz(lv) && !bit4_is_xz(rv)) {
		  eq = BIT4_0;
		  ఆపు;
	    }
      }

      thr->flags[4] = eq;
      తిరిగి నిజం;
}

స్థిర శూన్య do_CMPWE(vthread_t thr, స్థిరరాశి vvp_vector4_t&lval, స్థిరరాశి vvp_vector4_t&rval)
{
      ప్రతిజ్ఞా(rval.size() == lval.size());

      ఒకవేళ (lval.has_xz() || rval.has_xz()) {

	    గుర్తులేని wid = lval.size();
	    vvp_bit4_t eq  = BIT4_1;

	    కొరకు (గుర్తులేని idx = 0 ; idx < wid ; idx += 1) {
		  vvp_bit4_t lv = lval.value(idx);
		  vvp_bit4_t rv = rval.value(idx);

		  ఒకవేళ (bit4_is_xz(rv))
			కొనసాగు;
		  ఒకవేళ ((eq == BIT4_1) && bit4_is_xz(lv))
			eq = BIT4_X;
		  ఒకవేళ ((lv == BIT4_0) && (rv==BIT4_1))
			eq = BIT4_0;
		  ఒకవేళ ((lv == BIT4_1) && (rv==BIT4_0))
			eq = BIT4_0;

		  ఒకవేళ (eq == BIT4_0)
			ఆపు;
	    }

	    thr->flags[4] = eq;

      } లేకపోతే {
	      // If there are no XZ bits anywhere, then the results of
	      // ==? match the === test.
	    thr->flags[4] = (lval.eeq(rval)? BIT4_1 : BIT4_0);
      }
}

తార్కిక యొక్క_CMPWE(vthread_t thr, vvp_సంకేతం_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      స్థిరరాశి vvp_vector4_t&rval = thr->peek_vec4(0);
      స్థిరరాశి vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPWE(thr, lval, rval);

      thr->తీయి_vec4(2);
      తిరిగి నిజం;
}

తార్కిక యొక్క_CMPWNE(vthread_t thr, vvp_సంకేతం_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      స్థిరరాశి vvp_vector4_t&rval = thr->peek_vec4(0);
      స్థిరరాశి vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPWE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];

      thr->తీయి_vec4(2);
      తిరిగి నిజం;
}

తార్కిక యొక్క_CMPWR(vthread_t thr, vvp_సంకేతం_t)
{
      ద్విగుణ r = thr->తీయి_real();
      ద్విగుణ l = thr->తీయి_real();

      vvp_bit4_t eq = (l == r)? BIT4_1 : BIT4_0;
      vvp_bit4_t lt = (l <  r)? BIT4_1 : BIT4_0;

      thr->flags[4] = eq;
      thr->flags[5] = lt;

      తిరిగి నిజం;
}

/*
 * %cmp/z
 */
తార్కిక యొక్క_CMPZ(vthread_t thr, vvp_సంకేతం_t)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_vector4_t rval = thr->తీయి_vec4();
      vvp_vector4_t lval = thr->తీయి_vec4();

      ప్రతిజ్ఞా(rval.size() == lval.size());
      గుర్తులేని wid = lval.size();

      కొరకు (గుర్తులేని idx = 0 ; idx < wid ; idx += 1) {
	    vvp_bit4_t lv = lval.value(idx);
	    vvp_bit4_t rv = rval.value(idx);
	    ఒకవేళ ((lv != rv) && (rv != BIT4_Z) && (lv != BIT4_Z)) {
		  eq = BIT4_0;
		  ఆపు;
	    }
      }

      thr->flags[4] = eq;
      తిరిగి నిజం;
}

/*
 *  %concat/str;
 */
తార్కిక యొక్క_CONCAT_STR(vthread_t thr, vvp_సంకేతం_t)
{
      string పాఠ్యం = thr->తీయి_str();
      thr->peek_str(0).append(పాఠ్యం);
      తిరిగి నిజం;
}

/*
 *  %concati/str <string>;
 */
తార్కిక యొక్క_CONCATI_STR(vthread_t thr, vvp_సంకేతం_t cp)
{
      స్థిరరాశి అక్షరం*పాఠ్యం = cp->పాఠ్యం;
      thr->peek_str(0).append(filter_string(పాఠ్యం));
      తిరిగి నిజం;
}

/*
 * %concat/vec4
 */
తార్కిక యొక్క_CONCAT_VEC4(vthread_t thr, vvp_సంకేతం_t)
{
      స్థిరరాశి vvp_vector4_t&lsb = thr->peek_vec4(0);
      స్థిరరాశి vvp_vector4_t&msb = thr->peek_vec4(1);

	// The result is the size of the top two vectors in the stack.
      vvp_vector4_t res (msb.size() + lsb.size(), BIT4_X);

	// Build up the result.
      res.సమితి_vec(0, lsb);
      res.సమితి_vec(lsb.size(), msb);

	// Rearrange the stack to pop the inputs and push the
	// result. Do that by actually popping only 1 stack position
	// and replacing the new top with the new value.
      thr->తీయి_vec4(1);
      thr->peek_vec4() = res;

      తిరిగి నిజం;
}

/*
 * %concati/vec4 <vala>, <valb>, <wid>
 *
 * Concat the immediate value to the LOW bits of the concatenation.
 * Get the HIGH bits from the top of the vec4 stack.
 */
తార్కిక యొక్క_CONCATI_VEC4(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని wid  = cp->సంఖ్య;

      vvp_vector4_t&msb = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t lsb (wid, BIT4_0);
      get_తక్షణ_rval (cp, lsb);

      vvp_vector4_t res (msb.size()+lsb.size(), BIT4_X);
      res.సమితి_vec(0, lsb);
      res.సమితి_vec(lsb.size(), msb);

      msb = res;
      తిరిగి నిజం;
}

/*
 * %cvt/rv
 */
తార్కిక యొక్క_CVT_RV(vthread_t thr, vvp_సంకేతం_t)
{
      ద్విగుణ val;
      vvp_vector4_t val4 = thr->తీయి_vec4();
      vector4_గమ్యం_విలువ(val4, val, అబద్ధం);
      thr->నెట్టు_real(val);
      తిరిగి నిజం;
}

/*
 * %cvt/rv/s
 */
తార్కిక యొక్క_CVT_RV_S(vthread_t thr, vvp_సంకేతం_t)
{
      ద్విగుణ val;
      vvp_vector4_t val4 = thr->తీయి_vec4();
      vector4_గమ్యం_విలువ(val4, val, నిజం);
      thr->నెట్టు_real(val);
      తిరిగి నిజం;
}

/*
 * %cvt/sr <idx>
 * Pop the top value from the real stack, convert it to a 64bit signed
 * and save it to the indexed register.
 */
తార్కిక యొక్క_CVT_SR(vthread_t thr, vvp_సంకేతం_t cp)
{
      ద్విగుణ r = thr->తీయి_real();
      thr->పదాలు[cp->బిట్టు_idx[0]].w_int = i64round(r);

      తిరిగి నిజం;
}

/*
 * %cvt/ur <idx>
 */
తార్కిక యొక్క_CVT_UR(vthread_t thr, vvp_సంకేతం_t cp)
{
      ద్విగుణ r = thr->తీయి_real();
      thr->పదాలు[cp->బిట్టు_idx[0]].w_uint = vlg_విడత_గమ్యం_u64(r);

      తిరిగి నిజం;
}

/*
 * %cvt/vr <wid>
 */
తార్కిక యొక్క_CVT_VR(vthread_t thr, vvp_సంకేతం_t cp)
{
      ద్విగుణ r = thr->తీయి_real();
      గుర్తులేని wid = cp->సంఖ్య;

      vvp_vector4_t tmp(wid, r);
      thr->నెట్టు_vec4(tmp);
      తిరిగి నిజం;
}

/*
 * This implements the %deassign instruction. All we do is write a
 * long(1) to port-3 of the addressed net. This turns off an active
 * continuous assign activated by %cassign/v
 */
తార్కిక యొక్క_DEASSIGN(vthread_t, vvp_సంకేతం_t cp)
{
      vvp_net_t*net = cp->net;
      గుర్తులేని పునాది  = cp->బిట్టు_idx[0];
      గుర్తులేని width = cp->బిట్టు_idx[1];

      vvp_signal_విలువ*fil = dynamic_cast<vvp_signal_విలువ*> (net->fil);
      ప్రతిజ్ఞా(fil);
      vvp_fun_signal_vec*sig = dynamic_cast<vvp_fun_signal_vec*>(net->fun);
      ప్రతిజ్ఞా(sig);

      ఒకవేళ (పునాది >= fil->విలువ_కొలత()) తిరిగి నిజం;
      ఒకవేళ (పునాది+width > fil->విలువ_కొలత()) width = fil->విలువ_కొలత() - పునాది;

      తార్కిక నిండిన_sig = పునాది == 0 && width == fil->విలువ_కొలత();

	// This is the net that is forcing me...
      ఒకవేళ (vvp_net_t*src = sig->cassign_link) {
	    ఒకవేళ (! నిండిన_sig) {
		  fprintf(stderr, "Sorry: when a signal is assigning a "
		          "register, I cannot deassign part of it.\n");
		  నిష్క్రమించు(1);
	    }
	      // And this is the pointer to be removed.
	    vvp_net_ptr_t dst_ptr (net, 1);
	    src->unlink(dst_ptr);
	    sig->cassign_link = 0;
      }

	/* Do we release all or part of the net? */
      ఒకవేళ (నిండిన_sig) {
	    sig->deassign();
      } లేకపోతే {
	    sig->deassign_pv(పునాది, width);
      }

      తిరిగి నిజం;
}

తార్కిక యొక్క_DEASSIGN_WR(vthread_t, vvp_సంకేతం_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_real*sig = dynamic_cast<vvp_fun_signal_real*>(net->fun);
      ప్రతిజ్ఞా(sig);

	// This is the net that is forcing me...
      ఒకవేళ (vvp_net_t*src = sig->cassign_link) {
	      // And this is the pointer to be removed.
	    vvp_net_ptr_t dst_ptr (net, 1);
	    src->unlink(dst_ptr);
	    sig->cassign_link = 0;
      }

      sig->deassign();

      తిరిగి నిజం;
}

/*
 * %debug/thr
 */
తార్కిక యొక్క_DEBUG_THR(vthread_t thr, vvp_సంకేతం_t cp)
{
      స్థిరరాశి అక్షరం*పాఠ్యం = cp->పాఠ్యం;
      thr->debug_dump(cerr, పాఠ్యం);
      తిరిగి నిజం;
}

/*
 * The delay takes two 32bit numbers to make up a 64bit time.
 *
 *   %delay <low>, <hig>
 */
తార్కిక యొక్క_DELAY(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_time64_t దిగువ = cp->బిట్టు_idx[0];
      vvp_time64_t hig = cp->బిట్టు_idx[1];

      vvp_time64_t delay = (hig << 32) | దిగువ;

      ఒకవేళ (delay == 0) కాలపట్టిక_inactive(thr);
      లేకపోతే కాలపట్టిక_vthread(thr, delay);
      తిరిగి అబద్ధం;
}

తార్కిక యొక్క_DELAYX(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_time64_t delay;

      ప్రతిజ్ఞా(cp->సంఖ్య < vthread_s::పదాలు_లెక్క);
      delay = thr->పదాలు[cp->సంఖ్య].w_uint;
      ఒకవేళ (delay == 0) కాలపట్టిక_inactive(thr);
      లేకపోతే కాలపట్టిక_vthread(thr, delay);
      తిరిగి అబద్ధం;
}

తార్కిక యొక్క_DELETE_ELEM(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_net_t*net = cp->net;

      int64_t idx_val = thr->పదాలు[3].w_int;
      ఒకవేళ (thr->flags[4] == BIT4_1) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with undefined index."
	         << పంక్తి_ముగింపు;
	    తిరిగి నిజం;
      }
      ఒకవేళ (idx_val < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with negative index."
	         << పంక్తి_ముగింపు;
	    తిరిగి నిజం;
      }
      size_t idx = idx_val;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      ప్రతిజ్ఞా(obj);

      vvp_వరుస*వరుస = obj->get_object().peek<vvp_వరుస>();
      ఒకవేళ (వరుస == 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping delete(" << idx
	         << ") on empty queue." << పంక్తి_ముగింపు;
      } లేకపోతే {
	    size_t size = వరుస->get_కొలత();
	    ఒకవేళ (idx >= size) {
		  cerr << thr->get_fileline()
		       << "Warning: skipping out of range delete(" << idx
		       << ") on queue of size " << size << "." << పంక్తి_ముగింపు;
	    } లేకపోతే {
		  వరుస->erase(idx);
	    }
      }

      తిరిగి నిజం;
}

/* %delete/obj <label>
 *
 * This operator works by assigning a nil to the target object. This
 * causes any value that might be there to be garbage collected, thus
 * deleting the object.
 */
తార్కిక యొక్క_DELETE_OBJ(vthread_t thr, vvp_సంకేతం_t cp)
{
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_send_object(ptr, vvp_object_t(), thr->wt_context);

      తిరిగి నిజం;
}

/* %reverse/obj <label>
 * Reverse element order of the queue or dynamic array object at this net.
 */
తార్కిక యొక్క_తిరగేయి_OBJ(vthread_t /*thr*/, vvp_సంకేతం_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      ప్రతిజ్ఞా(obj);

      vvp_object_t stor = obj->get_object();
      ఒకవేళ (stor.test_nil()) తిరిగి నిజం;

	/* Match other queue ops: peek queues before vvp_darray so concrete
	   queue types dispatch reverse_elems() correctly. */
      ఒకవేళ (vvp_వరుస* వరుస = stor.peek<vvp_వరుస>()) {
	    వరుస->తిరగేయి_elems();
	    తిరిగి నిజం;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      ప్రతిజ్ఞా(darray);
      darray->తిరగేయి_elems();

      తిరిగి నిజం;
}

/* %reverse/prop/obj <property-index>
 * Reverse queue/darray property storage inside the class object on stack top.
 */
తార్కిక యొక్క_తిరగేయి_PROP_OBJ(vthread_t thr, vvp_సంకేతం_t cp)
{
      size_t pid = cp->సంఖ్య;

      vvp_object_t& శిఖరం = thr->peek_object();
      vvp_cobject*cobj = శిఖరం.peek<vvp_cobject>();
      ప్రతిజ్ఞా(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      ఒకవేళ (arr_obj.test_nil()) తిరిగి నిజం;

      ఒకవేళ (vvp_వరుస* వరుస = arr_obj.peek<vvp_వరుస>()) {
	    వరుస->తిరగేయి_elems();
	    తిరిగి నిజం;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      ప్రతిజ్ఞా(darray);
      darray->తిరగేయి_elems();

      తిరిగి నిజం;
}

/* %sort/obj — ascending sort */
తార్కిక యొక్క_క్రమీకరించు_OBJ(vthread_t /*thr*/, vvp_సంకేతం_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      ప్రతిజ్ఞా(obj);

      vvp_object_t stor = obj->get_object();
      ఒకవేళ (stor.test_nil()) తిరిగి నిజం;

      ఒకవేళ (vvp_వరుస* వరుస = stor.peek<vvp_వరుస>()) {
	    వరుస->క్రమీకరించు_elems(నిజం);
	    తిరిగి నిజం;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      ప్రతిజ్ఞా(darray);
      darray->క్రమీకరించు_elems(నిజం);

      తిరిగి నిజం;
}

/* %sort/prop/obj */
తార్కిక యొక్క_క్రమీకరించు_PROP_OBJ(vthread_t thr, vvp_సంకేతం_t cp)
{
      size_t pid = cp->సంఖ్య;

      vvp_object_t& శిఖరం = thr->peek_object();
      vvp_cobject*cobj = శిఖరం.peek<vvp_cobject>();
      ప్రతిజ్ఞా(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      ఒకవేళ (arr_obj.test_nil()) తిరిగి నిజం;

      ఒకవేళ (vvp_వరుస* వరుస = arr_obj.peek<vvp_వరుస>()) {
	    వరుస->క్రమీకరించు_elems(నిజం);
	    తిరిగి నిజం;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      ప్రతిజ్ఞా(darray);
      darray->క్రమీకరించు_elems(నిజం);

      తిరిగి నిజం;
}

/* %rsort/obj — descending sort */
తార్కిక యొక్క_RSORT_OBJ(vthread_t /*thr*/, vvp_సంకేతం_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      ప్రతిజ్ఞా(obj);

      vvp_object_t stor = obj->get_object();
      ఒకవేళ (stor.test_nil()) తిరిగి నిజం;

      ఒకవేళ (vvp_వరుస* వరుస = stor.peek<vvp_వరుస>()) {
	    వరుస->క్రమీకరించు_elems(అబద్ధం);
	    తిరిగి నిజం;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      ప్రతిజ్ఞా(darray);
      darray->క్రమీకరించు_elems(అబద్ధం);

      తిరిగి నిజం;
}

/* %rsort/prop/obj */
తార్కిక యొక్క_RSORT_PROP_OBJ(vthread_t thr, vvp_సంకేతం_t cp)
{
      size_t pid = cp->సంఖ్య;

      vvp_object_t& శిఖరం = thr->peek_object();
      vvp_cobject*cobj = శిఖరం.peek<vvp_cobject>();
      ప్రతిజ్ఞా(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      ఒకవేళ (arr_obj.test_nil()) తిరిగి నిజం;

      ఒకవేళ (vvp_వరుస* వరుస = arr_obj.peek<vvp_వరుస>()) {
	    వరుస->క్రమీకరించు_elems(అబద్ధం);
	    తిరిగి నిజం;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      ప్రతిజ్ఞా(darray);
      darray->క్రమీకరించు_elems(అబద్ధం);

      తిరిగి నిజం;
}

/* %shuffle/obj */
తార్కిక యొక్క_SHUFFLE_OBJ(vthread_t /*thr*/, vvp_సంకేతం_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      ప్రతిజ్ఞా(obj);

      vvp_object_t stor = obj->get_object();
      ఒకవేళ (stor.test_nil()) తిరిగి నిజం;

      ఒకవేళ (vvp_వరుస* వరుస = stor.peek<vvp_వరుస>()) {
	    వరుస->shuffle_elems();
	    తిరిగి నిజం;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      ప్రతిజ్ఞా(darray);
      darray->shuffle_elems();

      తిరిగి నిజం;
}

/* %shuffle/prop/obj */
తార్కిక యొక్క_SHUFFLE_PROP_OBJ(vthread_t thr, vvp_సంకేతం_t cp)
{
      size_t pid = cp->సంఖ్య;

      vvp_object_t& శిఖరం = thr->peek_object();
      vvp_cobject*cobj = శిఖరం.peek<vvp_cobject>();
      ప్రతిజ్ఞా(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      ఒకవేళ (arr_obj.test_nil()) తిరిగి నిజం;

      ఒకవేళ (vvp_వరుస* వరుస = arr_obj.peek<vvp_వరుస>()) {
	    వరుస->shuffle_elems();
	    తిరిగి నిజం;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      ప్రతిజ్ఞా(darray);
      darray->shuffle_elems();

      తిరిగి నిజం;
}

/* %delete/tail <label>, idx
 *
 * Remove all elements after the one specified.
 */
తార్కిక యొక్క_DELETE_తోక(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      ప్రతిజ్ఞా(obj);

      vvp_వరుస*వరుస = obj->get_object().peek<vvp_వరుస>();
      ప్రతిజ్ఞా(వరుస);

      గుర్తులేని idx = thr->పదాలు[cp->బిట్టు_idx[0]].w_int;
      వరుస->erase_తోక(idx);

      తిరిగి నిజం;
}

స్థిర తార్కిక do_disable(vthread_t thr, vthread_t సరిపోలిక)
{
      తార్కిక flag = అబద్ధం;

	/* Pull the target thread out of its scope if needed. */
      thr->మాతృ_scope->దారాలు.erase(thr);

	/* Turn the thread off by setting is program counter to
	   zero and setting an OFF bit. */
      thr->pc = codespace_null();
      thr->i_was_disabled = 1;
      thr->i_have_ended = 1;

	/* Turn off all the children of the thread. Simulate a %join
	   for as many times as needed to clear the results of all the
	   %forks that this thread has done. */
      అయితే (! thr->children.empty()) {

	    vthread_t tmp = *(thr->children.begin());
	    ప్రతిజ్ఞా(tmp);
	    ప్రతిజ్ఞా(tmp->మాతృ == thr);
	    thr->i_am_joining = 0;
	    ఒకవేళ (do_disable(tmp, సరిపోలిక))
		  flag = నిజం;

	    vthread_reap(tmp);
      }

      vthread_t మాతృ = thr->మాతృ;
      ఒకవేళ (మాతృ && మాతృ->i_am_joining) {
	      // If a parent is waiting in a %join, wake it up. Note
	      // that it is possible to be waiting in a %join yet
	      // already scheduled if multiple child threads are
	      // ending. So check if the thread is already scheduled
	      // before scheduling it again.
	    మాతృ->i_am_joining = 0;
	    ఒకవేళ (! మాతృ->i_have_ended)
		  కాలపట్టిక_vthread(మాతృ, 0, నిజం);

	    do_join(మాతృ, thr);

      } లేకపోతే ఒకవేళ (మాతృ) {
	      /* If the parent is yet to %join me, let its %join
		 do the reaping. */
	      //assert(tmp->is_scheduled == 0);

      } లేకపోతే {
	      /* No parent at all. Goodbye. */
	    vthread_reap(thr);
      }

      తిరిగి flag || (thr == సరిపోలిక);
}

/*
 * Implement the %disable instruction by scanning the target scope for
 * all the target threads. Kill the target threads and wake up a
 * parent that is attempting a %join.
 */
తార్కిక యొక్క_DISABLE(vthread_t thr, vvp_సంకేతం_t cp)
{
      __vpiScope*scope = static_cast<__vpiScope*>(cp->handle);

      తార్కిక disabled_myself_flag = అబద్ధం;

      అయితే (! scope->దారాలు.empty()) {
	    సమితి<vthread_t>::iterator cur = scope->దారాలు.begin();

	    ఒకవేళ (do_disable(*cur, thr))
		  disabled_myself_flag = నిజం;
      }

      తిరిగి ! disabled_myself_flag;
}

/*
 * Similar to `of_DISABLE`. But will only disable a single thread of the
 * specified scope. The disabled thread will be the thread closest to the
 * current thread in thread hierarchy. This can either be the current thread,
 * either the thread itself or one of its parents.
 * This is used for SystemVerilog flow control instructions like `return`,
 * `continue` and `break`.
 */

తార్కిక యొక్క_DISABLE_FLOW(vthread_t thr, vvp_సంకేతం_t cp)
{
      స్థిరరాశి __vpiScope*scope = static_cast<__vpiScope*>(cp->handle);
      vthread_t cur = thr;

      అయితే (cur && cur->మాతృ_scope != scope)
	    cur = cur->మాతృ;

      ప్రతిజ్ఞా(cur);
      ఒకవేళ (cur)
	    తిరిగి !do_disable(cur, thr);

      తిరిగి అబద్ధం;
}

/*
 * Implement the %disable/fork (SystemVerilog) instruction by disabling
 * all the detached children of the given thread.
 */
తార్కిక యొక్క_DISABLE_FORK(vthread_t thr, vvp_సంకేతం_t)
{
	/* If a %disable/fork is being executed then the parent thread
	 * cannot be waiting in a join. */
      ప్రతిజ్ఞా(! thr->i_am_joining);

	/* There should be no active children to disable. */
      ప్రతిజ్ఞా(thr->children.empty());

	/* Disable any detached children. */
      అయితే (! thr->detached_children.empty()) {
	    vthread_t బిడ్డ = *(thr->detached_children.begin());
	    ప్రతిజ్ఞా(బిడ్డ);
	    ప్రతిజ్ఞా(బిడ్డ->మాతృ == thr);
	      /* Disabling the children can never match the parent thread. */
	    తార్కిక res = do_disable(బిడ్డ, thr);
	    ప్రతిజ్ఞా(! res);
	    vthread_reap(బిడ్డ);
      }

      తిరిగి నిజం;
}

/*
 * This function divides a 2-word number {high, a} by a 1-word
 * number. Assume that high < b.
 */
స్థిర గుర్తులేని దీర్ఘ divide2words(గుర్తులేని దీర్ఘ a, గుర్తులేని దీర్ఘ b,
				  గుర్తులేని దీర్ఘ ఎగువ)
{
      గుర్తులేని దీర్ఘ ఫలితం = 0;
      అయితే (ఎగువ > 0) {
	    గుర్తులేని దీర్ఘ tmp_ఫలితం = ULONG_MAX / b;
	    గుర్తులేని దీర్ఘ remain = ULONG_MAX % b;

	    remain += 1;
	    ఒకవేళ (remain >= b) {
		  remain -= b;
		  tmp_ఫలితం += 1;
	    }

	      // Now 0x1_0...0 = b*tmp_result + remain
	      // high*0x1_0...0 = high*(b*tmp_result + remain)
	      // high*0x1_0...0 = high*b*tmp_result + high*remain

	      // We know that high*0x1_0...0 >= high*b*tmp_result, and
	      // we know that high*0x1_0...0 > high*remain. Use
	      // high*remain as the remainder for another iteration,
	      // and add tmp_result*high into the current estimate of
	      // the result.
	    ఫలితం += tmp_ఫలితం * ఎగువ;

	      // The new iteration starts with high*remain + a.
	    remain = multiply_with_వాహకం(ఎగువ, remain, ఎగువ);
	    a += remain;
            ఒకవేళ(a < remain)
              ఎగువ += 1;

	      // Now result*b + {high,a} == the input {high,a}. It is
	      // possible that the new high >= 1. If so, it will
	      // certainly be less than high from the previous
	      // iteration. Do another iteration and it will shrink,
	      // eventually to 0.
      }

	// high is now 0, so a is the remaining remainder, so we can
	// finish off the integer divide with a simple a/b.

      తిరిగి ఫలితం + a/b;
}

స్థిర గుర్తులేని దీర్ఘ* divide_బిట్లు(గుర్తులేని దీర్ఘ*ap, స్థిరరాశి గుర్తులేని దీర్ఘ*bp, గుర్తులేని wid)
{
	// Do all our work a cpu-word at a time. The "words" variable
	// is the number of words of the wid.
      గుర్తులేని పదాలు = (wid+CPU_మాట_బిట్లు-1) / CPU_మాట_బిట్లు;

      గుర్తులేని btop = పదాలు-1;
      అయితే (btop > 0 && bp[btop] == 0)
	    btop -= 1;

	// Detect divide by 0, and exit.
      ఒకవేళ (btop==0 && bp[0]==0)
	    తిరిగి 0;

	// The result array will eventually accumulate the result. The
	// diff array is a difference that we use in the intermediate.
      గుర్తులేని దీర్ఘ*diff  = కొత్త గుర్తులేని దీర్ఘ[పదాలు];
      గుర్తులేని దీర్ఘ*ఫలితం= కొత్త గుర్తులేని దీర్ఘ[పదాలు];
      కొరకు (గుర్తులేని idx = 0 ; idx < పదాలు ; idx += 1)
	    ఫలితం[idx] = 0;

      కొరకు (గుర్తులేని cur = పదాలు-btop ; cur > 0 ; cur -= 1) {
	    గుర్తులేని cur_ptr = cur-1;
	    గుర్తులేని దీర్ఘ cur_res;
	    ఒకవేళ (ap[cur_ptr+btop] >= bp[btop]) {
		  గుర్తులేని దీర్ఘ ఎగువ = 0;
		  ఒకవేళ (cur_ptr+btop+1 < పదాలు)
			ఎగువ = ap[cur_ptr+btop+1];
		  cur_res = divide2words(ap[cur_ptr+btop], bp[btop], ఎగువ);

	    } లేకపోతే ఒకవేళ (cur_ptr+btop+1 >= పదాలు) {
		  కొనసాగు;

	    } లేకపోతే ఒకవేళ (ap[cur_ptr+btop+1] == 0) {
		  కొనసాగు;

	    } లేకపోతే {
		  cur_res = divide2words(ap[cur_ptr+btop], bp[btop],
					 ap[cur_ptr+btop+1]);
	    }

	      // cur_res is a guesstimate of the result this far. It
	      // may be 1 too big. (But it will also be >0) Try it,
	      // and if the difference comes out negative, then adjust.

	      // diff = (bp * cur_res)  << cur_ptr;
	    multiply_array_imm(diff+cur_ptr, bp, పదాలు-cur_ptr, cur_res);
	      // ap -= diff
	    గుర్తులేని దీర్ఘ వాహకం = 1;
	    కొరకు (గుర్తులేని idx = cur_ptr ; idx < పదాలు ; idx += 1)
		  ap[idx] = కూడు_with_వాహకం(ap[idx], ~diff[idx], వాహకం);

	      // ap has the diff subtracted out of it. If cur_res was
	      // too large, then ap will turn negative. (We easily
	      // tell that ap turned negative by looking at
	      // carry&1. If it is 0, then it is *negative*.) In that
	      // case, we know that cur_res was too large by 1. Correct by
	      // adding 1b back in and reducing cur_res.
	    ఒకవేళ ((వాహకం&1) == 0) {
		    // Keep adding b back in until the remainder
		    // becomes positive again.
		  చేయి {
			cur_res -= 1;
			వాహకం = 0;
			కొరకు (గుర్తులేని idx = cur_ptr ; idx < పదాలు ; idx += 1)
			      ap[idx] = కూడు_with_వాహకం(ap[idx], bp[idx-cur_ptr], వాహకం);
		  } అయితే (వాహకం == 0);
	    }

	    ఫలితం[cur_ptr] = cur_res;
      }

	// Now ap contains the remainder and result contains the
	// desired result. We should find that:
	//  input-a = bp * result + ap;

      తొలగించు[]diff;
      తిరిగి ఫలితం;
}

/*
 * %div
 */
తార్కిక యొక్క_DIV(vthread_t thr, vvp_సంకేతం_t)
{
      vvp_vector4_t valb = thr->తీయి_vec4();
      vvp_vector4_t vala = thr->తీయి_vec4();

      ప్రతిజ్ఞా(vala.size()== valb.size());
      గుర్తులేని wid = vala.size();

      గుర్తులేని దీర్ఘ*ap = vala.subarray(0, wid);
      ఒకవేళ (ap == 0) {
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->నెట్టు_vec4(tmp);
	    తిరిగి నిజం;
      }

      గుర్తులేని దీర్ఘ*bp = valb.subarray(0, wid);
      ఒకవేళ (bp == 0) {
	    తొలగించు[]ap;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->నెట్టు_vec4(tmp);
	    తిరిగి నిజం;
      }

	// If the value fits in a single CPU word, then do it the easy way.
      ఒకవేళ (wid <= CPU_మాట_బిట్లు) {
	    ఒకవేళ (bp[0] == 0) {
		  vvp_vector4_t tmp(wid, BIT4_X);
		  thr->నెట్టు_vec4(tmp);
	    } లేకపోతే {
		  ap[0] /= bp[0];
		  vala.setarray(0, wid, ap);
		  thr->నెట్టు_vec4(vala);
	    }
	    తొలగించు[]ap;
	    తొలగించు[]bp;
	    తిరిగి నిజం;
      }

      గుర్తులేని దీర్ఘ*ఫలితం = divide_బిట్లు(ap, bp, wid);
      ఒకవేళ (ఫలితం == 0) {
	    తొలగించు[]ap;
	    తొలగించు[]bp;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->నెట్టు_vec4(tmp);
	    తిరిగి నిజం;
      }

	// Now ap contains the remainder and result contains the
	// desired result. We should find that:
	//  input-a = bp * result + ap;

      vala.setarray(0, wid, ఫలితం);
      thr->నెట్టు_vec4(vala);
      తొలగించు[]ap;
      తొలగించు[]bp;
      తొలగించు[]ఫలితం;

      తిరిగి నిజం;
}


స్థిర శూన్య negate_పదాలు(గుర్తులేని దీర్ఘ*val, గుర్తులేని పదాలు)
{
      గుర్తులేని దీర్ఘ వాహకం = 1;
      కొరకు (గుర్తులేని idx = 0 ; idx < పదాలు ; idx += 1)
	    val[idx] = కూడు_with_వాహకం(0, ~val[idx], వాహకం);
}

/*
 * %div/s
 */
తార్కిక యొక్క_DIV_S(vthread_t thr, vvp_సంకేతం_t)
{
      vvp_vector4_t valb = thr->తీయి_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      ప్రతిజ్ఞా(vala.size()== valb.size());
      గుర్తులేని wid = vala.size();
      గుర్తులేని పదాలు = (wid + CPU_మాట_బిట్లు - 1) / CPU_మాట_బిట్లు;

	// Get the values, left in right, in binary form. If there is
	// a problem with either (caused by an X or Z bit) then we
	// know right away that the entire result is X.
      గుర్తులేని దీర్ఘ*ap = vala.subarray(0, wid);
      ఒకవేళ (ap == 0) {
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    తిరిగి నిజం;
      }

      గుర్తులేని దీర్ఘ*bp = valb.subarray(0, wid);
      ఒకవేళ (bp == 0) {
	    తొలగించు[]ap;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    తిరిగి నిజం;
      }

	// Sign extend the bits in the array to fill out the array.
      గుర్తులేని దీర్ఘ sign_ముసుగు = 0;
      ఒకవేళ (గుర్తులేని దీర్ఘ sign_బిట్లు = (పదాలు*CPU_మాట_బిట్లు) - wid) {
	    sign_ముసుగు = -1UL << (CPU_మాట_బిట్లు-sign_బిట్లు);
	    ఒకవేళ (ap[పదాలు-1] & (sign_ముసుగు>>1))
		  ap[పదాలు-1] |= sign_ముసుగు;
	    ఒకవేళ (bp[పదాలు-1] & (sign_ముసుగు>>1))
		  bp[పదాలు-1] |= sign_ముసుగు;
      }

	// If the value fits in a single word, then use the native divide.
      ఒకవేళ (wid <= CPU_మాట_బిట్లు) {
	    ఒకవేళ (bp[0] == 0) {
		  vvp_vector4_t tmp(wid, BIT4_X);
		  vala = tmp;
	    } లేకపోతే ఒకవేళ (((దీర్ఘ)ap[0] == LONG_MIN) && ((దీర్ఘ)bp[0] == -1)) {
		  vvp_vector4_t tmp(wid, BIT4_0);
		  tmp.సమితి_బిట్టు(wid-1, BIT4_1);
		  vala = tmp;
	    } లేకపోతే {
		  దీర్ఘ tmpa = (దీర్ఘ) ap[0];
		  దీర్ఘ tmpb = (దీర్ఘ) bp[0];
		  దీర్ఘ res = tmpa / tmpb;
		  ap[0] = ((గుర్తులేని దీర్ఘ)res) & ~sign_ముసుగు;
		  vala.setarray(0, wid, ap);
	    }
	    తొలగించు[]ap;
	    తొలగించు[]bp;
	    తిరిగి నిజం;
      }

	// We need to the actual division to positive integers. Make
	// them positive here, and remember the negations.
      తార్కిక negate_flag = అబద్ధం;
      ఒకవేళ ( ((దీర్ఘ) ap[పదాలు-1]) < 0 ) {
	    negate_flag = నిజం;
	    negate_పదాలు(ap, పదాలు);
      }
      ఒకవేళ ( ((దీర్ఘ) bp[పదాలు-1]) < 0 ) {
	    negate_flag ^= నిజం;
	    negate_పదాలు(bp, పదాలు);
      }

      గుర్తులేని దీర్ఘ*ఫలితం = divide_బిట్లు(ap, bp, wid);
      ఒకవేళ (ఫలితం == 0) {
	    తొలగించు[]ap;
	    తొలగించు[]bp;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    తిరిగి నిజం;
      }

      ఒకవేళ (negate_flag) {
	    negate_పదాలు(ఫలితం, పదాలు);
      }

      ఫలితం[పదాలు-1] &= ~sign_ముసుగు;

      vala.setarray(0, wid, ఫలితం);
      తొలగించు[]ap;
      తొలగించు[]bp;
      తొలగించు[]ఫలితం;
      తిరిగి నిజం;
}

తార్కిక యొక్క_DIV_WR(vthread_t thr, vvp_సంకేతం_t)
{
      ద్విగుణ r = thr->తీయి_real();
      ద్విగుణ l = thr->తీయి_real();
      thr->నెట్టు_real(l / r);

      తిరిగి నిజం;
}

/*
 * %dup/obj
 * %dup/real
 * %dup/vec4
 *
 * Push a duplicate of the object on the appropriate stack.
 */
తార్కిక యొక్క_DUP_OBJ(vthread_t thr, vvp_సంకేతం_t)
{
      vvp_object_t src = thr->peek_object();

        // If it is null push a new null object
      ఒకవేళ (src.test_nil())
	    thr->నెట్టు_object(vvp_object_t());
      లేకపోతే
	    thr->నెట్టు_object(src.duplicate());

      తిరిగి నిజం;
}

తార్కిక యొక్క_DUP_REAL(vthread_t thr, vvp_సంకేతం_t)
{
      thr->నెట్టు_real(thr->peek_real(0));
      తిరిగి నిజం;
}

తార్కిక యొక్క_DUP_VEC4(vthread_t thr, vvp_సంకేతం_t)
{
      thr->నెట్టు_vec4(thr->peek_vec4(0));
      తిరిగి నిజం;
}

/*
 * This terminates the current thread. If there is a parent who is
 * waiting for me to die, then I schedule it. At any rate, I mark
 * myself as a zombie by setting my pc to 0.
 */
తార్కిక యొక్క_చివర(vthread_t thr, vvp_సంకేతం_t)
{
      ప్రతిజ్ఞా(! thr->నిరీక్షణ_for_event);
      thr->i_have_ended = 1;
      thr->pc = codespace_null();

	/* Fully detach any detached children. */
      అయితే (! thr->detached_children.empty()) {
	    vthread_t బిడ్డ = *(thr->detached_children.begin());
	    ప్రతిజ్ఞా(బిడ్డ);
	    ప్రతిజ్ఞా(బిడ్డ->మాతృ == thr);
	    ప్రతిజ్ఞా(బిడ్డ->i_am_detached);
	    బిడ్డ->మాతృ = 0;
	    బిడ్డ->i_am_detached = 0;
	    thr->detached_children.erase(thr->detached_children.begin());
      }

	/* It is an error to still have active children running at this
	 * point in time. They should have all been detached or joined. */
      ప్రతిజ్ఞా(thr->children.empty());

	/* If I have a parent who is waiting for me, then mark that I
	   have ended, and schedule that parent. Also, finish the
	   %join for the parent. */
      ఒకవేళ (!thr->i_am_detached && thr->మాతృ && thr->మాతృ->i_am_joining) {
	    vthread_t tmp = thr->మాతృ;
	    ప్రతిజ్ఞా(! thr->i_am_detached);

	    tmp->i_am_joining = 0;
	    కాలపట్టిక_vthread(tmp, 0, నిజం);
	    do_join(tmp, thr);
	    తిరిగి అబద్ధం;
      }

	/* If this thread is not fully detached then remove it from the
	 * parents detached_children set and reap it. */
      ఒకవేళ (thr->i_am_detached) {
	    vthread_t tmp = thr->మాతృ;
	    ప్రతిజ్ఞా(tmp);
	    size_t res = tmp->detached_children.erase(thr);
	    ప్రతిజ్ఞా(res == 1);
	      /* If the parent is waiting for the detached children to
	       * finish then the last detached child needs to tell the
	       * parent to wake up when it is finished. */
	    ఒకవేళ (tmp->i_am_నిరీక్షణ && tmp->detached_children.empty()) {
		  tmp->i_am_నిరీక్షణ = 0;
		  కాలపట్టిక_vthread(tmp, 0, నిజం);
	    }
	      /* Fully detach this thread so it will be reaped below. */
	    thr->i_am_detached = 0;
	    thr->మాతృ = 0;
      }

	/* If I have no parent, then no one can %join me and there is
	 * no reason to stick around. This can happen, for example if
	 * I am an ``initial'' thread. */
      ఒకవేళ (thr->మాతృ == 0) {
	    vthread_reap(thr);
	    తిరిగి అబద్ధం;
      }

	/* If I make it this far, then I have a parent who may wish
	   to %join me. Remain a zombie so that it can. */

      తిరిగి అబద్ధం;
}

/*
 * %event <var-label>
 */
తార్కిక యొక్క_EVENT(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_vector4_t tmp (1, BIT4_X);
      vvp_send_vec4(ptr, tmp, thr->wt_context);
      తిరిగి నిజం;
}

/*
 * %event/nb <var-label>, <delay>
 */
తార్కిక యొక్క_EVENT_NB(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_time64_t delay;

      delay = thr->పదాలు[cp->బిట్టు_idx[0]].w_uint;
      కాలపట్టిక_propagate_event(cp->net, delay);
      తిరిగి నిజం;
}

తార్కిక యొక్క_EVCTL(vthread_t thr, vvp_సంకేతం_t cp)
{
      ప్రతిజ్ఞా(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      thr->ecount = thr->పదాలు[cp->బిట్టు_idx[0]].w_uint;
      తిరిగి నిజం;
}
తార్కిక యొక్క_EVCTLC(vthread_t thr, vvp_సంకేతం_t)
{
      thr->event = 0;
      thr->ecount = 0;
      తిరిగి నిజం;
}

తార్కిక యొక్క_EVCTLI(vthread_t thr, vvp_సంకేతం_t cp)
{
      ప్రతిజ్ఞా(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      thr->ecount = cp->బిట్టు_idx[0];
      తిరిగి నిజం;
}

తార్కిక యొక్క_EVCTLS(vthread_t thr, vvp_సంకేతం_t cp)
{
      ప్రతిజ్ఞా(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      int64_t val = thr->పదాలు[cp->బిట్టు_idx[0]].w_int;
      ఒకవేళ (val < 0) val = 0;
      thr->ecount = val;
      తిరిగి నిజం;
}

తార్కిక యొక్క_FLAG_GET_VEC4(vthread_t thr, vvp_సంకేతం_t cp)
{
      పూర్ణాంకం flag = cp->సంఖ్య;
      ప్రతిజ్ఞా(flag < vthread_s::FLAGS_లెక్క);

      vvp_vector4_t val (1, thr->flags[flag]);
      thr->నెట్టు_vec4(val);

      తిరిగి నిజం;
}

/*
 * %flag_inv <flag1>
 */
తార్కిక యొక్క_FLAG_INV(vthread_t thr, vvp_సంకేతం_t cp)
{
      పూర్ణాంకం flag1 = cp->బిట్టు_idx[0];

      thr->flags[flag1] = ~ thr->flags[flag1];
      తిరిగి నిజం;
}

/*
 * %flag_mov <flag1>, <flag2>
 */
తార్కిక యొక్క_FLAG_MOV(vthread_t thr, vvp_సంకేతం_t cp)
{
      పూర్ణాంకం flag1 = cp->బిట్టు_idx[0];
      పూర్ణాంకం flag2 = cp->బిట్టు_idx[1];

      thr->flags[flag1] = thr->flags[flag2];
      తిరిగి నిజం;
}

/*
 * %flag_or <flag1>, <flag2>
 */
తార్కిక యొక్క_FLAG_OR(vthread_t thr, vvp_సంకేతం_t cp)
{
      పూర్ణాంకం flag1 = cp->బిట్టు_idx[0];
      పూర్ణాంకం flag2 = cp->బిట్టు_idx[1];

      thr->flags[flag1] = thr->flags[flag1] | thr->flags[flag2];
      తిరిగి నిజం;
}

తార్కిక యొక్క_FLAG_సమితి_IMM(vthread_t thr, vvp_సంకేతం_t cp)
{
      పూర్ణాంకం flag = cp->సంఖ్య;
      పూర్ణాంకం vali = cp->బిట్టు_idx[0];

      ప్రతిజ్ఞా(flag < vthread_s::FLAGS_లెక్క);
      ప్రతిజ్ఞా(vali >= 0 && vali < 4);

      స్థిర స్థిరరాశి vvp_bit4_t map_బిట్టు[4] = {BIT4_0, BIT4_1, BIT4_Z, BIT4_X};
      thr->flags[flag] = map_బిట్టు[vali];
      తిరిగి నిజం;
}

తార్కిక యొక్క_FLAG_సమితి_VEC4(vthread_t thr, vvp_సంకేతం_t cp)
{
      పూర్ణాంకం flag = cp->సంఖ్య;
      ప్రతిజ్ఞా(flag < vthread_s::FLAGS_లెక్క);

      స్థిరరాశి vvp_vector4_t&val = thr->peek_vec4();
      thr->flags[flag] = val.value(0);
      thr->తీయి_vec4(1);

      తిరిగి నిజం;
}

/*
 * the %force/link instruction connects a source node to a
 * destination node. The destination node must be a signal, as it is
 * marked with the source of the force so that it may later be
 * unlinked without specifically knowing the source that this
 * instruction used.
 */
తార్కిక యొక్క_FORCE_LINK(vthread_t, vvp_సంకేతం_t cp)
{
      vvp_net_t*dst = cp->net;
      vvp_net_t*src = cp->net2;

      ప్రతిజ్ఞా(dst->fil);
      dst->fil->force_link(dst, src);

      తిరిగి నిజం;
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
తార్కిక యొక్క_FORCE_VEC4(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_vector4_t value = thr->తీయి_vec4();

	/* Send the force value to the filter on the node. */

      ప్రతిజ్ఞా(net->fil);
      ఒకవేళ (value.size() != net->fil->filter_కొలత())
	    value = coerce_గమ్యం_వెడల్పు(value, net->fil->filter_కొలత());

      net->force_vec4(value, vvp_vector2_t(vvp_vector2_t::FILL1, net->fil->filter_కొలత()));

      తిరిగి నిజం;
}

/*
 * %force/vec4/off <net>, <off>
 */
తార్కిక యొక్క_FORCE_VEC4_OFF(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_net_t*net = cp->net;
      గుర్తులేని పునాది_idx = cp->బిట్టు_idx[0];
      దీర్ఘ పునాది = thr->పదాలు[పునాది_idx].w_int;
      vvp_vector4_t value = thr->తీయి_vec4();
      గుర్తులేని wid = value.size();

      ప్రతిజ్ఞా(net->fil);

      ఒకవేళ (thr->flags[4] == BIT4_1)
	    తిరిగి నిజం;

	// This is the width of the target vector.
      గుర్తులేని use_కొలత = net->fil->filter_కొలత();

      ఒకవేళ (పునాది >= (దీర్ఘ)use_కొలత)
	    తిరిగి నిజం;
      ఒకవేళ (పునాది < -(దీర్ఘ)use_కొలత)
	    తిరిగి నిజం;

      ఒకవేళ ((పునాది + wid) > use_కొలత)
	    wid = use_కొలత - పునాది;

	// Make a mask of which bits are to be forced, 0 for unforced
	// bits and 1 for forced bits.
      vvp_vector2_t ముసుగు (vvp_vector2_t::FILL0, use_కొలత);
      కొరకు (గుర్తులేని idx = 0 ; idx < wid ; idx += 1)
	    ముసుగు.సమితి_బిట్టు(పునాది+idx, 1);

      vvp_vector4_t tmp (use_కొలత, BIT4_Z);

	// vvp_net_t::force_vec4 propagates all the bits of the
	// forced vector value, regardless of the mask. This
	// ensures the unforced bits retain their current value.
      vvp_signal_విలువ*sig = dynamic_cast<vvp_signal_విలువ*>(net->fil);
      ప్రతిజ్ఞా(sig);
      sig->vec4_విలువ(tmp);

      tmp.సమితి_vec(పునాది, value);

      net->force_vec4(tmp, ముసుగు);
      తిరిగి నిజం;
}

/*
 * %force/vec4/off/d <net>, <off>, <del>
 */
తార్కిక యొక్క_FORCE_VEC4_OFF_D(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_net_t*net = cp->net;

      గుర్తులేని పునాది_idx = cp->బిట్టు_idx[0];
      దీర్ఘ పునాది = thr->పదాలు[పునాది_idx].w_int;

      గుర్తులేని delay_idx = cp->బిట్టు_idx[1];
      vvp_time64_t delay = thr->పదాలు[delay_idx].w_uint;

      vvp_vector4_t value = thr->తీయి_vec4();

      ప్రతిజ్ఞా(net->fil);

      ఒకవేళ (thr->flags[4] == BIT4_1)
	    తిరిగి నిజం;

	// This is the width of the target vector.
      గుర్తులేని use_కొలత = net->fil->filter_కొలత();

      ఒకవేళ (పునాది >= (దీర్ఘ)use_కొలత)
	    తిరిగి నిజం;
      ఒకవేళ (పునాది < -(దీర్ఘ)use_కొలత)
	    తిరిగి నిజం;

      కాలపట్టిక_force_సదిశ(net, పునాది, use_కొలత, value, delay);
      తిరిగి నిజం;
}

తార్కిక యొక్క_FORCE_WR(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_net_t*net  = cp->net;
      ద్విగుణ value = thr->తీయి_real();

      net->force_real(value, vvp_vector2_t(vvp_vector2_t::FILL1, 1));

      తిరిగి నిజం;
}

/*
 * The %fork instruction causes a new child to be created and pushed
 * in front of any existing child. This causes the new child to be
 * added to the list of children, and for me to be the parent of the
 * new child.
 */
తార్కిక యొక్క_FORK(vthread_t thr, vvp_సంకేతం_t cp)
{
      vthread_t బిడ్డ = vthread_new(cp->cptr2, cp->scope);

      ఒకవేళ (cp->scope->is_automatic()) {
              /* The context allocated for this child is the top entry
                 on the write context stack. */
            బిడ్డ->wt_context = thr->wt_context;
            బిడ్డ->rd_context = thr->wt_context;
      }

      బిడ్డ->మాతృ = thr;
      thr->children.insert(బిడ్డ);

      ఒకవేళ (thr->i_am_లోపల_function) {
	    బిడ్డ->is_scheduled = 1;
	    బిడ్డ->i_am_లోపల_function = 1;
	    vthread_నిరంతరం(బిడ్డ);
	    running_thread = thr;
      } లేకపోతే {
	    కాలపట్టిక_vthread(బిడ్డ, 0, నిజం);
      }
      తిరిగి నిజం;
}

తార్కిక యొక్క_FREE(vthread_t thr, vvp_సంకేతం_t cp)
{
        /* Pop the child context from the read context stack. */
      vvp_context_t బిడ్డ_context = thr->rd_context;
      thr->rd_context = vvp_get_stacked_context(బిడ్డ_context);

        /* Free the context. */
      vthread_free_context(బిడ్డ_context, cp->scope);

      తిరిగి నిజం;
}

/*
 * %inv
 *
 * Logically, this pops a value, inverts is (Verilog style, with Z and
 * X converted to X) and pushes the result. We can more efficiently
 * just to the invert in place.
 */
తార్కిక యొక్క_INV(vthread_t thr, vvp_సంకేతం_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val.invert();
      తిరిగి నిజం;
}


/*
 * Index registers, arithmetic.
 */

స్థిర అంతఃవరుస int64_t get_as_64_బిట్టు(uint32_t దిగువ_32, uint32_t ఎగువ_32)
{
      int64_t దిగువ = దిగువ_32;
      int64_t res = ఎగువ_32;

      res <<= 32;
      res |= దిగువ;
      తిరిగి res;
}

తార్కిక యొక్క_IX_కూడు(vthread_t thr, vvp_సంకేతం_t cp)
{
      thr->పదాలు[cp->సంఖ్య].w_int += get_as_64_బిట్టు(cp->బిట్టు_idx[0],
                                                    cp->బిట్టు_idx[1]);
      తిరిగి నిజం;
}

తార్కిక యొక్క_IX_SUB(vthread_t thr, vvp_సంకేతం_t cp)
{
      thr->పదాలు[cp->సంఖ్య].w_int -= get_as_64_బిట్టు(cp->బిట్టు_idx[0],
                                                    cp->బిట్టు_idx[1]);
      తిరిగి నిజం;
}

తార్కిక యొక్క_IX_MUL(vthread_t thr, vvp_సంకేతం_t cp)
{
      thr->పదాలు[cp->సంఖ్య].w_int *= get_as_64_బిట్టు(cp->బిట్టు_idx[0],
                                                    cp->బిట్టు_idx[1]);
      తిరిగి నిజం;
}

తార్కిక యొక్క_IX_ఎక్కించు(vthread_t thr, vvp_సంకేతం_t cp)
{
      thr->పదాలు[cp->సంఖ్య].w_int = get_as_64_బిట్టు(cp->బిట్టు_idx[0],
                                                   cp->బిట్టు_idx[1]);
      తిరిగి నిజం;
}

తార్కిక యొక్క_IX_MOV(vthread_t thr, vvp_సంకేతం_t cp)
{
      thr->పదాలు[cp->బిట్టు_idx[0]].w_int = thr->పదాలు[cp->బిట్టు_idx[1]].w_int;
      తిరిగి నిజం;
}

తార్కిక యొక్క_IX_GETV(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని సూచీ = cp->బిట్టు_idx[0];
      vvp_net_t*net = cp->net;

      vvp_signal_విలువ*sig = dynamic_cast<vvp_signal_విలువ*>(net->fil);
      ఒకవేళ (sig == 0) {
	    ప్రతిజ్ఞా(net->fil);
	    cerr << thr->get_fileline()
	         << "%%ix/getv error: Net arg not a vector signal? "
		 << typeid(*net->fil).name() << పంక్తి_ముగింపు;
      }
      ప్రతిజ్ఞా(sig);

      vvp_vector4_t vec;
      sig->vec4_విలువ(vec);
      తార్కిక అధికప్రవాహం_flag;
      uint64_t val = 0;
      తార్కిక తెలిసిన_flag = vector4_గమ్యం_విలువ(vec, అధికప్రవాహం_flag, val);

      ఒకవేళ (తెలిసిన_flag)
	    thr->పదాలు[సూచీ].w_uint = val;
      లేకపోతే
	    thr->పదాలు[సూచీ].w_uint = 0;

	/* Set bit 4 as a flag if the input is unknown. */
      thr->flags[4] = తెలిసిన_flag ? (అధికప్రవాహం_flag ? BIT4_X : BIT4_0) : BIT4_1;

      తిరిగి నిజం;
}

తార్కిక యొక్క_IX_GETV_S(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని సూచీ = cp->బిట్టు_idx[0];
      vvp_net_t*net = cp->net;

      vvp_signal_విలువ*sig = dynamic_cast<vvp_signal_విలువ*>(net->fil);
      ఒకవేళ (sig == 0) {
	    ప్రతిజ్ఞా(net->fil);
	    cerr << thr->get_fileline()
	         << "%%ix/getv/s error: Net arg not a vector signal? "
		 << "fun=" << typeid(*net->fil).name()
		 << ", fil=" << (net->fil? typeid(*net->fil).name() : "<>")
		 << పంక్తి_ముగింపు;
      }
      ప్రతిజ్ఞా(sig);

      vvp_vector4_t vec;
      sig->vec4_విలువ(vec);
      int64_t val;
      తార్కిక తెలిసిన_flag = vector4_గమ్యం_విలువ(vec, val, నిజం, నిజం);

      ఒకవేళ (తెలిసిన_flag)
	    thr->పదాలు[సూచీ].w_int = val;
      లేకపోతే
	    thr->పదాలు[సూచీ].w_int = 0;

	/* Set bit 4 as a flag if the input is unknown. */
      thr->flags[4] = తెలిసిన_flag? BIT4_0 : BIT4_1;

      తిరిగి నిజం;
}

స్థిర uint64_t vec4_గమ్యం_సూచీ(vthread_t thr, తార్కిక signed_flag)
{
	// Get all the information we need about the vec4 vector, then
	// pop it away. We only need the bool bits and the length.
      స్థిరరాశి vvp_vector4_t&val = thr->peek_vec4();
      గుర్తులేని val_కొలత = val.size();
      గుర్తులేని దీర్ఘ*బిట్లు = val.subarray(0, val_కొలత, అబద్ధం);
      thr->తీయి_vec4(1);

	// If there are X/Z bits, then the subarray will give us a nil
	// pointer. Set a flag to indicate the error, and give up.
      ఒకవేళ (బిట్లు == 0) {
	    thr->flags[4] = BIT4_1;
	    తిరిగి 0;
      }

      uint64_t v = 0;
      thr->flags[4] = BIT4_0;

      ప్రతిజ్ఞా(పరిమాణం(బిట్లు[0]) <= పరిమాణం(v));

      v = 0;
      కొరకు (గుర్తులేని idx = 0 ; idx < val_కొలత ; idx += 8*పరిమాణం(బిట్లు[0])) {
	    uint64_t tmp = బిట్లు[idx/8/పరిమాణం(బిట్లు[0])];
	    ఒకవేళ (idx < 8*పరిమాణం(v)) {
		  v |= tmp << idx;
	    } లేకపోతే {
		  తార్కిక అధికప్రవాహం = signed_flag && (v >> 63) ? ~tmp != 0 : tmp != 0;
		  ఒకవేళ (అధికప్రవాహం) {
			thr->flags[4] = BIT4_X;
			ఆపు;
		  }
	    }
      }

	// Set the high bits that are not necessarily filled in by the
	// subarray function.
      ఒకవేళ (val_కొలత < 8*పరిమాణం(v)) {
	    ఒకవేళ (signed_flag && (v & (static_cast<uint64_t>(1)<<(val_కొలత-1)))) {
		    // Propagate the sign bit...
		  v |= (~static_cast<uint64_t>(0)) << val_కొలత;

	    } లేకపోతే {
		    // Fill with zeros.
		  v &= ~((~static_cast<uint64_t>(0)) << val_కొలత);
	    }

      }

      తొలగించు[]బిట్లు;
      తిరిగి v;
}

/*
 * %ix/vec4 <idx>
 */
తార్కిక యొక్క_IX_VEC4(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని use_idx = cp->సంఖ్య;
      thr->పదాలు[use_idx].w_uint = vec4_గమ్యం_సూచీ(thr, అబద్ధం);
      తిరిగి నిజం;
}

/*
 * %ix/vec4/s <idx>
 */
తార్కిక యొక్క_IX_VEC4_S(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని use_idx = cp->సంఖ్య;
      thr->పదాలు[use_idx].w_uint = vec4_గమ్యం_సూచీ(thr, నిజం);
      తిరిగి నిజం;
}

/*
 * The various JMP instruction work simply by pulling the new program
 * counter from the instruction and resuming. If the jump is
 * conditional, then test the bit for the expected value first.
 */
తార్కిక యొక్క_JMP(vthread_t thr, vvp_సంకేతం_t cp)
{
      thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      ఒకవేళ (కాలపట్టిక_stopped()) {
	    కాలపట్టిక_vthread(thr, 0, అబద్ధం);
	    తిరిగి అబద్ధం;
      }

      తిరిగి నిజం;
}

/*
 * %jmp/0 <pc>, <flag>
 */
తార్కిక యొక్క_JMP0(vthread_t thr, vvp_సంకేతం_t cp)
{
      ఒకవేళ (thr->flags[cp->బిట్టు_idx[0]] == BIT4_0)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      ఒకవేళ (కాలపట్టిక_stopped()) {
	    కాలపట్టిక_vthread(thr, 0, అబద్ధం);
	    తిరిగి అబద్ధం;
      }

      తిరిగి నిజం;
}

/*
 * %jmp/0xz <pc>, <flag>
 */
తార్కిక యొక్క_JMP0_XZ(vthread_t thr, vvp_సంకేతం_t cp)
{
      ఒకవేళ (thr->flags[cp->బిట్టు_idx[0]] != BIT4_1)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      ఒకవేళ (కాలపట్టిక_stopped()) {
	    కాలపట్టిక_vthread(thr, 0, అబద్ధం);
	    తిరిగి అబద్ధం;
      }

      తిరిగి నిజం;
}

/*
 * %jmp/1 <pc>, <flag>
 */
తార్కిక యొక్క_JMP1(vthread_t thr, vvp_సంకేతం_t cp)
{
      ఒకవేళ (thr->flags[cp->బిట్టు_idx[0]] == BIT4_1)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      ఒకవేళ (కాలపట్టిక_stopped()) {
	    కాలపట్టిక_vthread(thr, 0, అబద్ధం);
	    తిరిగి అబద్ధం;
      }

      తిరిగి నిజం;
}

/*
 * %jmp/1xz <pc>, <flag>
 */
తార్కిక యొక్క_JMP1_XZ(vthread_t thr, vvp_సంకేతం_t cp)
{
      ఒకవేళ (thr->flags[cp->బిట్టు_idx[0]] != BIT4_0)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      ఒకవేళ (కాలపట్టిక_stopped()) {
	    కాలపట్టిక_vthread(thr, 0, అబద్ధం);
	    తిరిగి అబద్ధం;
      }

      తిరిగి నిజం;
}

/*
 * The %join instruction causes the thread to wait for one child
 * to die.  If a child is already dead (and a zombie) then I reap
 * it and go on. Otherwise, I mark myself as waiting in a join so that
 * children know to wake me when they finish.
 */

స్థిర శూన్య do_join(vthread_t thr, vthread_t బిడ్డ)
{
      ప్రతిజ్ఞా(బిడ్డ->మాతృ == thr);

        /* If the immediate child thread is in an automatic scope... */
      ఒకవేళ (బిడ్డ->wt_context) {
              /* and is the top level task/function thread... */
            ఒకవేళ (thr->wt_context != thr->rd_context) {
                    /* Pop the child context from the write context stack. */
                  vvp_context_t బిడ్డ_context = thr->wt_context;
                  thr->wt_context = vvp_get_stacked_context(బిడ్డ_context);

                    /* Push the child context onto the read context stack */
                  vvp_సమితి_stacked_context(బిడ్డ_context, thr->rd_context);
                  thr->rd_context = బిడ్డ_context;
            }
      }

      vthread_reap(బిడ్డ);
}

స్థిర తార్కిక do_join_opcode(vthread_t thr)
{
      ప్రతిజ్ఞా( !thr->i_am_joining );
      ప్రతిజ్ఞా( !thr->children.empty());

	// Are there any children that have already ended? If so, then
	// join with that one.
      కొరకు (సమితి<vthread_t>::iterator cur = thr->children.begin()
		 ; cur != thr->children.చివర() ; ++cur) {
	    vthread_t curp = *cur;
	    ఒకవేళ (! curp->i_have_ended)
		  కొనసాగు;

	      // found something!
	    do_join(thr, curp);
	    తిరిగి నిజం;
      }

	// Otherwise, tell my children to awaken me when they end,
	// then pause.
      thr->i_am_joining = 1;
      తిరిగి అబద్ధం;
}

తార్కిక యొక్క_JOIN(vthread_t thr, vvp_సంకేతం_t)
{
      తిరిగి do_join_opcode(thr);
}

/*
 * This %join/detach <n> instruction causes the thread to detach
 * threads that were created by an earlier %fork.
 */
తార్కిక యొక్క_JOIN_DETACH(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని దీర్ఘ లెక్క = cp->సంఖ్య;

      ప్రతిజ్ఞా(లెక్క == thr->children.size());

      అయితే (! thr->children.empty()) {
	    vthread_t బిడ్డ = *thr->children.begin();
	    ప్రతిజ్ఞా(బిడ్డ->మాతృ == thr);

	      // We cannot detach automatic tasks/functions within an
	      // automatic scope. If we try to do that, we might make
	      // a mess of the allocation of the context. Note that it
	      // is OK if the child context is distinct (See %exec_ufunc.)
	    ప్రతిజ్ఞా(బిడ్డ->wt_context==0 || thr->wt_context!=బిడ్డ->wt_context);
	    ఒకవేళ (బిడ్డ->i_have_ended) {
		    // If the child has already ended, then reap it.
		  vthread_reap(బిడ్డ);

	    } లేకపోతే {
		  size_t res = బిడ్డ->మాతృ->children.erase(బిడ్డ);
		  ప్రతిజ్ఞా(res == 1);
		  బిడ్డ->i_am_detached = 1;
		  thr->detached_children.insert(బిడ్డ);
	    }
      }

      తిరిగి నిజం;
}

/*
 * %load/ar <array-label>, <index>;
*/
తార్కిక యొక్క_ఎక్కించు_AR(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని idx = cp->బిట్టు_idx[0];
      ద్విగుణ మాట;

	/* The result is 0.0 if the address is undefined. */
      ఒకవేళ (thr->flags[4] == BIT4_1) {
	    మాట = 0.0;
      } లేకపోతే {
	    గుర్తులేని adr = thr->పదాలు[idx].w_int;
	    మాట = cp->array->get_మాట_r(adr);
      }

      thr->నెట్టు_real(మాట);
      తిరిగి నిజం;
}

మూస <రకం_పేరు ELEM>
స్థిర తార్కిక ఎక్కించు_dar(vthread_t thr, vvp_సంకేతం_t cp)
{
      int64_t adr = thr->పదాలు[3].w_int;
      vvp_net_t*net = cp->net;
      ప్రతిజ్ఞా(net);

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      ప్రతిజ్ఞా(obj);

      vvp_darray*darray = obj->get_object().peek<vvp_darray>();

      ELEM మాట;
      ఒకవేళ (darray &&
          (adr >= 0) && (thr->flags[4] == BIT4_0)) // A defined address >= 0
	    darray->get_మాట(adr, మాట);
      లేకపోతే
	    dq_default(మాట, obj->size());

      vthread_నెట్టు(thr, మాట);
      తిరిగి నిజం;
}

/*
 * %load/dar/r <array-label>;
 */
తార్కిక యొక్క_ఎక్కించు_DAR_R(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి ఎక్కించు_dar<ద్విగుణ>(thr, cp);
}

/*
 * %load/dar/str <array-label>;
 */
తార్కిక యొక్క_ఎక్కించు_DAR_STR(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి ఎక్కించు_dar<string>(thr, cp);
}

/*
 * %load/dar/vec4 <array-label>;
 */
తార్కిక యొక్క_ఎక్కించు_DAR_VEC4(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి ఎక్కించు_dar<vvp_vector4_t>(thr, cp);
}

/*
 * %load/prop/dar/vec4 <pid>, <wid>;
 *   Indexed read of queue/dynamic-array class property; object on stack,
 *   index in words[3] (same as %load/dar/vec4).
 */
తార్కిక యొక్క_ఎక్కించు_PROP_DAR_VEC4(vthread_t thr, vvp_సంకేతం_t cp)
{
      size_t pid = cp->సంఖ్య;
      గుర్తులేని wid = cp->బిట్టు_idx[0];
      int64_t adr = thr->పదాలు[3].w_int;

      vvp_object_t& శిఖరం = thr->peek_object();
      vvp_cobject*cobj = శిఖరం.peek<vvp_cobject>();
      ప్రతిజ్ఞా(cobj);

      vvp_object_t pobj;
      cobj->get_object(pid, pobj, 0);
      vvp_darray*darray = pobj.peek<vvp_darray>();

      vvp_vector4_t మాట;
      ఒకవేళ (darray && (adr >= 0) && (thr->flags[4] == BIT4_0)) {
	    darray->get_మాట(adr, మాట);
      } లేకపోతే {
	    dq_default(మాట, wid);
      }

      thr->నెట్టు_vec4(మాట);
      తిరిగి నిజం;
}

/*
 * %load/obj <var-label>
 */
తార్కిక యొక్క_ఎక్కించు_OBJ(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_net_t*net = cp->net;
      vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      ప్రతిజ్ఞా(fun);

      vvp_object_t val = fun->get_object();
      thr->నెట్టు_object(val);

      తిరిగి నిజం;
}

/*
 * %load/obja <index>
 *    Loads the object from array, using index <index> as the index
 *    value. If flags[4] == 1, the calculation of <index> may have
 *    failed, so push nil.
 */
తార్కిక యొక్క_ఎక్కించు_OBJA(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని idx = cp->బిట్టు_idx[0];
      vvp_object_t మాట;

	/* The result is 0.0 if the address is undefined. */
      ఒకవేళ (thr->flags[4] == BIT4_1) {
	    ; // Return nil
      } లేకపోతే {
	    గుర్తులేని adr = thr->పదాలు[idx].w_int;
	    cp->array->get_మాట_obj(adr, మాట);
      }

      thr->నెట్టు_object(మాట);
      తిరిగి నిజం;
}

/*
 * %load/real <var-label>
 */
తార్కిక యొక్క_ఎక్కించు_REAL(vthread_t thr, vvp_సంకేతం_t cp)
{
      __vpiHandle*tmp = cp->handle;
      t_vpi_విలువ val;

      val.format = vpiRealVal;
      vpi_get_విలువ(tmp, &val);

      thr->నెట్టు_real(val.value.real);

      తిరిగి నిజం;
}

/*
 * %load/str <var-label>
 */
తార్కిక యొక్క_ఎక్కించు_STR(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_net_t*net = cp->net;


      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (net->fun);
      ప్రతిజ్ఞా(fun);

      స్థిరరాశి string&val = fun->get_string();
      thr->నెట్టు_str(val);

      తిరిగి నిజం;
}

తార్కిక యొక్క_ఎక్కించు_STRA(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని idx = cp->బిట్టు_idx[0];
      string మాట;

      ఒకవేళ (thr->flags[4] == BIT4_1) {
	    మాట = "";
      } లేకపోతే {
	    గుర్తులేని adr = thr->పదాలు[idx].w_int;
	    మాట = cp->array->get_మాట_str(adr);
      }

      thr->నెట్టు_str(మాట);
      తిరిగి నిజం;
}


/*
 * %load/vec4 <net>
 */
తార్కిక యొక్క_ఎక్కించు_VEC4(vthread_t thr, vvp_సంకేతం_t cp)
{
	// Push a placeholder onto the stack in order to reserve the
	// stack space. Use a reference for the stack top as a target
	// for the load.
      thr->నెట్టు_vec4(vvp_vector4_t());
      vvp_vector4_t&sig_విలువ = thr->peek_vec4();

      vvp_net_t*net = cp->net;

	// For the %load to work, the functor must actually be a
	// signal functor. Only signals save their vector value.
      vvp_signal_విలువ*sig = dynamic_cast<vvp_signal_విలువ*> (net->fil);
      ఒకవేళ (sig == 0) {
	    cerr << thr->get_fileline()
	         << "%load/v error: Net arg not a signal? "
		 << (net->fil ? typeid(*net->fil).name() :
	                        typeid(*net->fun).name())
	         << పంక్తి_ముగింపు;
	    ప్రతిజ్ఞా(sig);
	    తిరిగి నిజం;
      }

	// Extract the value from the signal and directly into the
	// target stack position.
      sig->vec4_విలువ(sig_విలువ);

      తిరిగి నిజం;
}

/*
 * %load/vec4a <arr>, <adrx>
 */
తార్కిక యొక్క_ఎక్కించు_VEC4_A(vthread_t thr, vvp_సంకేతం_t cp)
{
      పూర్ణాంకం adr_సూచీ = cp->బిట్టు_idx[0];

      దీర్ఘ adr = thr->పదాలు[adr_సూచీ].w_int;

	// If flag[3] is set, then the calculation of the address
	// failed, and this load should return X instead of the actual
	// value.
      ఒకవేళ (thr->flags[4] == BIT4_1) {
	    vvp_vector4_t tmp (cp->array->get_మాట_కొలత(), BIT4_X);
	    thr->నెట్టు_vec4(tmp);
	    తిరిగి నిజం;
      }

      vvp_vector4_t tmp (cp->array->get_మాట(adr));
      thr->నెట్టు_vec4(tmp);
      తిరిగి నిజం;
}

స్థిర శూన్య do_verylong_mod(vvp_vector4_t&vala, స్థిరరాశి vvp_vector4_t&valb,
			    తార్కిక ఎడమ_is_neg, తార్కిక కుడి_is_neg)
{
      తార్కిక బహిర్గతం_is_neg = ఎడమ_is_neg;
      స్థిరరాశి పూర్ణాంకం len=vala.size();
      గుర్తులేని అక్షరం *a, *z, *t;
      a = కొత్త గుర్తులేని అక్షరం[len+1];
      z = కొత్త గుర్తులేని అక్షరం[len+1];
      t = కొత్త గుర్తులేని అక్షరం[len+1];

      గుర్తులేని అక్షరం వాహకం;
      గుర్తులేని అక్షరం తాత్కాలిక;

      పూర్ణాంకం mxa = -1, mxz = -1;
      పూర్ణాంకం i;
      పూర్ణాంకం current, copylen;

      గుర్తులేని lb_వాహకం = ఎడమ_is_neg? 1 : 0;
      గుర్తులేని rb_వాహకం = కుడి_is_neg? 1 : 0;
      కొరకు (పూర్ణాంకం idx = 0 ;  idx < len ;  idx += 1) {
	    గుర్తులేని lb = vala.value(idx);
	    గుర్తులేని rb = valb.value(idx);

	    ఒకవేళ ((lb | rb) & 2) {
		  తొలగించు []t;
		  తొలగించు []z;
		  తొలగించు []a;
		  vvp_vector4_t tmp(len, BIT4_X);
		  vala = tmp;
		  తిరిగి;
	    }

	    ఒకవేళ (ఎడమ_is_neg) {
		  lb = (1-lb) + lb_వాహకం;
		  lb_వాహకం = (lb & ~1)? 1 : 0;
		  lb &= 1;
	    }
	    ఒకవేళ (కుడి_is_neg) {
		  rb = (1-rb) + rb_వాహకం;
		  rb_వాహకం = (rb & ~1)? 1 : 0;
		  rb &= 1;
	    }

	    z[idx]=lb;
	    a[idx]=1-rb;	// for 2s complement add..
      }

      z[len]=0;
      a[len]=1;

      కొరకు(i=len-1;i>=0;i--) {
	    ఒకవేళ(! a[i]) {
		  mxa=i;
		  ఆపు;
	    }
      }

      కొరకు(i=len-1;i>=0;i--) {
	    ఒకవేళ(z[i]) {
		  mxz=i;
		  ఆపు;
	    }
      }

      ఒకవేళ((mxa>mxz)||(mxa==-1)) {
	    ఒకవేళ(mxa==-1) {
		  తొలగించు []t;
		  తొలగించు []z;
		  తొలగించు []a;
		  vvp_vector4_t tmpx (len, BIT4_X);
		  vala = tmpx;
		  తిరిగి;
	    }

	    వెళ్ళు tally;
      }

      copylen = mxa + 2;
      current = mxz - mxa;

      అయితే(current > -1) {
	    వాహకం = 1;
	    కొరకు(i=0;i<copylen;i++) {
		  తాత్కాలిక = z[i+current] + a[i] + వాహకం;
		  t[i] = (తాత్కాలిక&1);
		  వాహకం = (తాత్కాలిక>>1);
	    }

	    ఒకవేళ(వాహకం) {
		  కొరకు(i=0;i<copylen;i++) {
			z[i+current] = t[i];
		  }
	    }

	    current--;
      }

 tally:

      vvp_vector4_t tmp (len, BIT4_X);
      వాహకం = బహిర్గతం_is_neg? 1 : 0;
      కొరకు (పూర్ణాంకం idx = 0 ;  idx < len ;  idx += 1) {
	    గుర్తులేని ob = z[idx];
	    ఒకవేళ (బహిర్గతం_is_neg) {
		  ob = (1-ob) + వాహకం;
		  వాహకం = (ob & ~1)? 1 : 0;
		  ob = ob & 1;
	    }
	    tmp.సమితి_బిట్టు(idx, ob?BIT4_1:BIT4_0);
      }
      vala = tmp;
      తొలగించు []t;
      తొలగించు []z;
      తొలగించు []a;
}

తార్కిక యొక్క_MAX_WR(vthread_t thr, vvp_సంకేతం_t)
{
      ద్విగుణ r = thr->తీయి_real();
      ద్విగుణ l = thr->తీయి_real();
      ఒకవేళ (r != r)
	    thr->నెట్టు_real(l);
      లేకపోతే ఒకవేళ (l != l)
	    thr->నెట్టు_real(r);
      లేకపోతే ఒకవేళ (r < l)
	    thr->నెట్టు_real(l);
      లేకపోతే
	    thr->నెట్టు_real(r);
      తిరిగి నిజం;
}

తార్కిక యొక్క_MIN_WR(vthread_t thr, vvp_సంకేతం_t)
{
      ద్విగుణ r = thr->తీయి_real();
      ద్విగుణ l = thr->తీయి_real();
      ఒకవేళ (r != r)
	    thr->నెట్టు_real(l);
      లేకపోతే ఒకవేళ (l != l)
	    thr->నెట్టు_real(r);
      లేకపోతే ఒకవేళ (r < l)
	    thr->నెట్టు_real(r);
      లేకపోతే
	    thr->నెట్టు_real(l);
      తిరిగి నిజం;
}

తార్కిక యొక్క_MOD(vthread_t thr, vvp_సంకేతం_t)
{
      vvp_vector4_t valb = thr->తీయి_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      ప్రతిజ్ఞా(vala.size()==valb.size());
      గుర్తులేని wid = vala.size();

      ఒకవేళ(wid <= 8*పరిమాణం(గుర్తులేని దీర్ఘ దీర్ఘ)) {
	    గుర్తులేని దీర్ఘ దీర్ఘ lv = 0, rv = 0;

	    కొరకు (గుర్తులేని idx = 0 ;  idx < wid ;  idx += 1) {
		  గుర్తులేని దీర్ఘ దీర్ఘ lb = vala.value(idx);
		  గుర్తులేని దీర్ఘ దీర్ఘ rb = valb.value(idx);

		  ఒకవేళ ((lb | rb) & 2)
			వెళ్ళు x_బహిర్గతం;

		  lv |= (గుర్తులేని దీర్ఘ దీర్ఘ) lb << idx;
		  rv |= (గుర్తులేని దీర్ఘ దీర్ఘ) rb << idx;
	    }

	    ఒకవేళ (rv == 0)
		  వెళ్ళు x_బహిర్గతం;

	    lv %= rv;

	    కొరకు (గుర్తులేని idx = 0 ;  idx < wid ;  idx += 1) {
		  vala.సమితి_బిట్టు(idx, (lv&1)?BIT4_1 : BIT4_0);
		  lv >>= 1;
	    }

	    తిరిగి నిజం;

      } లేకపోతే {
	    do_verylong_mod(vala, valb, అబద్ధం, అబద్ధం);
	    తిరిగి నిజం;
      }

 x_బహిర్గతం:
      vala = vvp_vector4_t(wid, BIT4_X);
      తిరిగి నిజం;
}

/*
 * %mod/s
 */
తార్కిక యొక్క_MOD_S(vthread_t thr, vvp_సంకేతం_t)
{
      vvp_vector4_t valb = thr->తీయి_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      ప్రతిజ్ఞా(vala.size()==valb.size());
      గుర్తులేని wid = vala.size();

	/* Handle the case that we can fit the bits into a long-long
	   variable. We cause use native % to do the work. */
      ఒకవేళ(wid <= 8*పరిమాణం(దీర్ఘ దీర్ఘ)) {
	    దీర్ఘ దీర్ఘ lv = 0, rv = 0;

	    కొరకు (గుర్తులేని idx = 0 ;  idx < wid ;  idx += 1) {
		  దీర్ఘ దీర్ఘ lb = vala.value(idx);
		  దీర్ఘ దీర్ఘ rb = valb.value(idx);

		  ఒకవేళ ((lb | rb) & 2)
			వెళ్ళు x_బహిర్గతం;

		  lv |= (దీర్ఘ దీర్ఘ) lb << idx;
		  rv |= (దీర్ఘ దీర్ఘ) rb << idx;
	    }

	    ఒకవేళ (rv == 0)
		  వెళ్ళు x_బహిర్గతం;

	    ఒకవేళ ((lv == LLONG_MIN) && (rv == -1))
		  వెళ్ళు zero_బహిర్గతం;

	      /* Sign extend the signed operands when needed. */
	    ఒకవేళ (wid < 8*పరిమాణం(దీర్ఘ దీర్ఘ)) {
		  ఒకవేళ (lv & (1LL << (wid-1)))
			lv |= -1ULL << wid;
		  ఒకవేళ (rv & (1LL << (wid-1)))
			rv |= -1ULL << wid;
	    }

	    lv %= rv;

	    కొరకు (గుర్తులేని idx = 0 ;  idx < wid ;  idx += 1) {
		  vala.సమితి_బిట్టు(idx, (lv&1)? BIT4_1 : BIT4_0);
		  lv >>= 1;
	    }

	      // vala is the top of the stack, edited in place, so we
	      // do not need to push the result.

	    తిరిగి నిజం;

      } లేకపోతే {

	    తార్కిక ఎడమ_is_neg  = vala.value(vala.size()-1) == BIT4_1;
	    తార్కిక కుడి_is_neg = valb.value(valb.size()-1) == BIT4_1;
	    do_verylong_mod(vala, valb, ఎడమ_is_neg, కుడి_is_neg);
	    తిరిగి నిజం;
      }

 x_బహిర్గతం:
      vala = vvp_vector4_t(wid, BIT4_X);
      తిరిగి నిజం;
 zero_బహిర్గతం:
      vala = vvp_vector4_t(wid, BIT4_0);
      తిరిగి నిజం;
}

/*
 * %mod/wr
 */
తార్కిక యొక్క_MOD_WR(vthread_t thr, vvp_సంకేతం_t)
{
      ద్విగుణ r = thr->తీయి_real();
      ద్విగుణ l = thr->తీయి_real();
      thr->నెట్టు_real(fmod(l,r));

      తిరిగి నిజం;
}

/*
 * %neg/wr
 */
తార్కిక యొక్క_NEG_WR(vthread_t thr, vvp_సంకేతం_t)
{
      thr->poke_real(0, -thr->peek_real(0));
      తిరిగి నిజం;
}

/*
 * %pad/s <wid>
 */
తార్కిక యొక్క_PAD_S(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని wid = cp->సంఖ్య;

      vvp_vector4_t&val = thr->peek_vec4();
      గుర్తులేని old_కొలత = val.size();

	// Sign-extend.
      ఒకవేళ (old_కొలత < wid)
	    val.resize(wid, val.value(old_కొలత-1));
      లేకపోతే
	    val.resize(wid);

      తిరిగి నిజం;
}

/*
 * %pad/u <wid>
 */
తార్కిక యొక్క_PAD_U(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని wid = cp->సంఖ్య;

      vvp_vector4_t&val = thr->peek_vec4();
      val.resize(wid, BIT4_0);

      తిరిగి నిజం;
}

/*
 * %part/s <wid>
 * %part/u <wid>
 * Two values are popped from the stack. First, pop the canonical
 * index of the part select, and second is the value to be
 * selected. The result is pushed back to the stack.
 */
స్థిర తార్కిక యొక్క_PART_పునాది(vthread_t thr, vvp_సంకేతం_t cp, తార్కిక signed_flag)
{
      గుర్తులేని wid = cp->సంఖ్య;

      vvp_vector4_t base4 = thr->తీయి_vec4();
      vvp_vector4_t&value = thr->peek_vec4();

      vvp_vector4_t res (wid, BIT4_X);

	// NOTE: This is treating the vector as signed. Is that correct?
      int32_t పునాది;
      తార్కిక విలువ_ok = vector4_గమ్యం_విలువ(base4, పునాది, signed_flag);
      ఒకవేళ (! విలువ_ok) {
	    value = res;
	    తిరిగి నిజం;
      }

      ఒకవేళ (పునాది >= (int32_t)value.size()) {
	    value = res;
	    తిరిగి నిజం;
      }

      ఒకవేళ ((పునాది+(పూర్ణాంకం)wid) <= 0) {
	    value = res;
	    తిరిగి నిజం;
      }

      దీర్ఘ vbase = 0;
      ఒకవేళ (పునాది < 0) {
	    vbase = -పునాది;
	    wid -= vbase;
	    పునాది = 0;
      }

      ఒకవేళ ((పునాది+wid) > value.size()) {
	    wid = value.size() - పునాది;
      }

      res .సమితి_vec(vbase, value.subvalue(పునాది, wid));
      value = res;

      తిరిగి నిజం;
}

తార్కిక యొక్క_PART_S(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి యొక్క_PART_పునాది(thr, cp, నిజం);
}

తార్కిక యొక్క_PART_U(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి యొక్క_PART_పునాది(thr, cp, అబద్ధం);
}

/*
 * %parti/s <wid>, <basei>, <base_wid>
 * %parti/u <wid>, <basei>, <base_wid>
 *
 * Pop the value to be selected. The result is pushed back to the stack.
 */
స్థిర తార్కిక యొక్క_PARTI_పునాది(vthread_t thr, vvp_సంకేతం_t cp, తార్కిక signed_flag)
{
      గుర్తులేని wid = cp->సంఖ్య;
      uint32_t పునాది = cp->బిట్టు_idx[0];
      uint32_t bwid = cp->బిట్టు_idx[1];

      vvp_vector4_t&value = thr->peek_vec4();

      vvp_vector4_t res (wid, BIT4_X);

	// NOTE: This is treating the vector as signed. Is that correct?
      int32_t use_పునాది = పునాది;
      ఒకవేళ (signed_flag && bwid < 32 && (పునాది&(1<<(bwid-1)))) {
	    use_పునాది |= -1UL << bwid;
      }

      ఒకవేళ (use_పునాది >= (int32_t)value.size()) {
	    value = res;
	    తిరిగి నిజం;
      }

      ఒకవేళ ((use_పునాది+(int32_t)wid) <= 0) {
	    value = res;
	    తిరిగి నిజం;
      }

      దీర్ఘ vbase = 0;
      ఒకవేళ (use_పునాది < 0) {
	    vbase = -use_పునాది;
	    wid -= vbase;
	    use_పునాది = 0;
      }

      ఒకవేళ ((use_పునాది+wid) > value.size()) {
	    wid = value.size() - use_పునాది;
      }

      res .సమితి_vec(vbase, value.subvalue(use_పునాది, wid));
      value = res;

      తిరిగి నిజం;
}

తార్కిక యొక్క_PARTI_S(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి యొక్క_PARTI_పునాది(thr, cp, నిజం);
}

తార్కిక యొక్క_PARTI_U(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి యొక్క_PARTI_పునాది(thr, cp, అబద్ధం);
}

/*
 * %mul
 */
తార్కిక యొక్క_MUL(vthread_t thr, vvp_సంకేతం_t)
{
      vvp_vector4_t r = thr->తీయి_vec4();
	// Rather then pop l, use it directly from the stack. When we
	// assign to 'l', that will edit the top of the stack, which
	// replaces a pop and a pull.
      vvp_vector4_t&l = thr->peek_vec4();

      l.mul(r);
      తిరిగి నిజం;
}

/*
 * %muli <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
తార్కిక యొక్క_MULI(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని wid = cp->సంఖ్య;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_తక్షణ_rval (cp, r);

      l.mul(r);
      తిరిగి నిజం;
}

తార్కిక యొక్క_MUL_WR(vthread_t thr, vvp_సంకేతం_t)
{
      ద్విగుణ r = thr->తీయి_real();
      ద్విగుణ l = thr->తీయి_real();
      thr->నెట్టు_real(l * r);

      తిరిగి నిజం;
}

తార్కిక యొక్క_NAND(vthread_t thr, vvp_సంకేతం_t)
{
      vvp_vector4_t valr = thr->తీయి_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      ప్రతిజ్ఞా(vall.size() == valr.size());

      vall &= valr;
      vall.invert();

      తిరిగి నిజం;
}

/*
 * %new/cobj <vpi_object>
 * This creates a new cobject (SystemVerilog class object) and pushes
 * it to the stack. The <vpi-object> is a __vpiHandle that is a
 * vpiClassDefn object that defines the item to be created.
 */
తార్కిక యొక్క_NEW_COBJ(vthread_t thr, vvp_సంకేతం_t cp)
{
      స్థిరరాశి class_type*defn = dynamic_cast<స్థిరరాశి class_type*> (cp->handle);
      ప్రతిజ్ఞా(defn);

      vvp_object_t tmp (కొత్త vvp_cobject(defn));
      thr->నెట్టు_object(tmp);
      తిరిగి నిజం;
}

తార్కిక యొక్క_NEW_DARRAY(vthread_t thr, vvp_సంకేతం_t cp)
{
      స్థిరరాశి అక్షరం*పాఠ్యం = cp->పాఠ్యం;
      size_t size = thr->పదాలు[cp->బిట్టు_idx[0]].w_int;
      గుర్తులేని మాట_wid;
      size_t n;

      vvp_object_t obj;
      ఒకవేళ (strcmp(పాఠ్యం,"b8") == 0) {
	    obj = కొత్త vvp_darray_atom<uint8_t>(size);
      } లేకపోతే ఒకవేళ (strcmp(పాఠ్యం,"b16") == 0) {
	    obj = కొత్త vvp_darray_atom<uint16_t>(size);
      } లేకపోతే ఒకవేళ (strcmp(పాఠ్యం,"b32") == 0) {
	    obj = కొత్త vvp_darray_atom<uint32_t>(size);
      } లేకపోతే ఒకవేళ (strcmp(పాఠ్యం,"b64") == 0) {
	    obj = కొత్త vvp_darray_atom<uint64_t>(size);
      } లేకపోతే ఒకవేళ (strcmp(పాఠ్యం,"sb8") == 0) {
	    obj = కొత్త vvp_darray_atom<int8_t>(size);
      } లేకపోతే ఒకవేళ (strcmp(పాఠ్యం,"sb16") == 0) {
	    obj = కొత్త vvp_darray_atom<int16_t>(size);
      } లేకపోతే ఒకవేళ (strcmp(పాఠ్యం,"sb32") == 0) {
	    obj = కొత్త vvp_darray_atom<int32_t>(size);
      } లేకపోతే ఒకవేళ (strcmp(పాఠ్యం,"sb64") == 0) {
	    obj = కొత్త vvp_darray_atom<int64_t>(size);
      } లేకపోతే ఒకవేళ ((1 == అక్షరశ్రేణి_చదువు(పాఠ్యం, "b%u%zn", &మాట_wid, &n)) &&
                 (n == అక్షరశ్రేణి_పొడవు(పాఠ్యం))) {
	    obj = కొత్త vvp_darray_vec2(size, మాట_wid);
      } లేకపోతే ఒకవేళ ((1 == అక్షరశ్రేణి_చదువు(పాఠ్యం, "sb%u%zn", &మాట_wid, &n)) &&
                 (n == అక్షరశ్రేణి_పొడవు(పాఠ్యం))) {
	    obj = కొత్త vvp_darray_vec2(size, మాట_wid);
      } లేకపోతే ఒకవేళ ((1 == అక్షరశ్రేణి_చదువు(పాఠ్యం, "v%u%zn", &మాట_wid, &n)) &&
                 (n == అక్షరశ్రేణి_పొడవు(పాఠ్యం))) {
	    obj = కొత్త vvp_darray_vec4(size, మాట_wid);
      } లేకపోతే ఒకవేళ ((1 == అక్షరశ్రేణి_చదువు(పాఠ్యం, "sv%u%zn", &మాట_wid, &n)) &&
                 (n == అక్షరశ్రేణి_పొడవు(పాఠ్యం))) {
	    obj = కొత్త vvp_darray_vec4(size, మాట_wid);
      } లేకపోతే ఒకవేళ (strcmp(పాఠ్యం,"r") == 0) {
	    obj = కొత్త vvp_darray_real(size);
      } లేకపోతే ఒకవేళ (strcmp(పాఠ్యం,"S") == 0) {
	    obj = కొత్త vvp_darray_string(size);
      } లేకపోతే {
	    cerr << get_fileline()
	         << "Internal error: Unsupported dynamic array type: "
	         << పాఠ్యం << "." << పంక్తి_ముగింపు;
	    ప్రతిజ్ఞా(0);
      }

      thr->నెట్టు_object(obj);

      తిరిగి నిజం;
}

తార్కిక యొక్క_NOOP(vthread_t, vvp_సంకేతం_t)
{
      తిరిగి నిజం;
}

/*
 * %nor/r
 */
తార్కిక యొక్క_NORR(vthread_t thr, vvp_సంకేతం_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_or());

      తిరిగి నిజం;
}

/*
 * Push a null to the object stack.
 */
తార్కిక యొక్క_NULL(vthread_t thr, vvp_సంకేతం_t)
{
      vvp_object_t tmp;
      thr->నెట్టు_object(tmp);
      తిరిగి నిజం;
}

/*
 * %and/r
 */
తార్కిక యొక్క_ANDR(vthread_t thr, vvp_సంకేతం_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_and());

      తిరిగి నిజం;
}

/*
 * %nand/r
 */
తార్కిక యొక్క_NANDR(vthread_t thr, vvp_సంకేతం_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_and());

      తిరిగి నిజం;
}

/*
 * %or/r
 */
తార్కిక యొక్క_ORR(vthread_t thr, vvp_సంకేతం_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_or());

      తిరిగి నిజం;
}

/*
 * %xor/r
 */
తార్కిక యొక్క_XORR(vthread_t thr, vvp_సంకేతం_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_xor());

      తిరిగి నిజం;
}

/*
 * %xnor/r
 */
తార్కిక యొక్క_XNORR(vthread_t thr, vvp_సంకేతం_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_xor());

      తిరిగి నిజం;
}

/*
 * %or
 */
తార్కిక యొక్క_OR(vthread_t thr, vvp_సంకేతం_t)
{
      vvp_vector4_t valb = thr->తీయి_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();
      vala |= valb;
      తిరిగి నిజం;
}

/*
 * %nor
 */
తార్కిక యొక్క_NOR(vthread_t thr, vvp_సంకేతం_t)
{
      vvp_vector4_t valr = thr->తీయి_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      ప్రతిజ్ఞా(vall.size() == valr.size());

      vall |= valr;
      vall.invert();

      తిరిగి నిజం;
}

/*
 * %pop/obj <num>, <skip>
 */
తార్కిక యొక్క_తీయి_OBJ(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని cnt = cp->బిట్టు_idx[0];
      గుర్తులేని skip = cp->బిట్టు_idx[1];

      thr->తీయి_object(cnt, skip);
      తిరిగి నిజం;
}

/*
 * %pop/real <number>
 */
తార్కిక యొక్క_తీయి_REAL(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని cnt = cp->సంఖ్య;
      thr->తీయి_real(cnt);
      తిరిగి నిజం;
}

/*
 *  %pop/str <number>
 */
తార్కిక యొక్క_తీయి_STR(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని cnt = cp->సంఖ్య;
      thr->తీయి_str(cnt);
      తిరిగి నిజం;
}

/*
 *  %pop/vec4 <number>
 */
తార్కిక యొక్క_తీయి_VEC4(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని cnt = cp->సంఖ్య;
      thr->తీయి_vec4(cnt);
      తిరిగి నిజం;
}

/*
 * %pow
 * %pow/s
 */
స్థిర తార్కిక యొక్క_POW_పునాది(vthread_t thr, తార్కిక signed_flag)
{
      vvp_vector4_t valb = thr->తీయి_vec4();
      vvp_vector4_t vala = thr->తీయి_vec4();

      గుర్తులేని wid = vala.size();

      vvp_vector2_t xv2 = vvp_vector2_t(vala, నిజం);
      vvp_vector2_t yv2 = vvp_vector2_t(valb, నిజం);


        /* If we have an X or Z in the arguments return X. */
      ఒకవేళ (xv2.is_NaN() || yv2.is_NaN()) {
	    vvp_vector4_t tmp (wid, BIT4_X);
	    thr->నెట్టు_vec4(tmp);
	    తిరిగి నిజం;
      }

	// Is the exponent negative? If so, table 5-6 in IEEE1364-2005
	// defines what value is returned.
      ఒకవేళ (signed_flag && yv2.value(yv2.size()-1)) {
	    పూర్ణాంకం a_val;
	    vvp_bit4_t pad = BIT4_0, lsb = BIT4_0;
	    ఒకవేళ (vector2_గమ్యం_విలువ(xv2, a_val, నిజం)) {
		  ఒకవేళ (a_val == 0) {
			pad = BIT4_X; lsb = BIT4_X;
		  }
		  ఒకవేళ (a_val == 1) {
			pad = BIT4_0; lsb = BIT4_1;
		  }
		  ఒకవేళ (a_val == -1) {
			ఒకవేళ (yv2.value(0)) {
			      pad = BIT4_1; lsb = BIT4_1;
			} లేకపోతే {
			      pad = BIT4_0; lsb = BIT4_1;
			}
		  }
	    }
	    vvp_vector4_t tmp (wid, pad);
	    tmp.సమితి_బిట్టు(0, lsb);
	    thr->నెట్టు_vec4(tmp);
	    తిరిగి నిజం;
      }

      vvp_vector2_t ఫలితం = ఘాతం(xv2, yv2);

        /* Copy only what we need of the result. If the result is too
	   small, zero-pad it. */
      కొరకు (గుర్తులేని jdx = 0;  jdx < wid;  jdx += 1) {
	    ఒకవేళ (jdx >= ఫలితం.size())
		  vala.సమితి_బిట్టు(jdx, BIT4_0);
	    లేకపోతే
		  vala.సమితి_బిట్టు(jdx, ఫలితం.value(jdx) ? BIT4_1 : BIT4_0);
      }
      thr->నెట్టు_vec4(vala);

      తిరిగి నిజం;
}

తార్కిక యొక్క_POW(vthread_t thr, vvp_సంకేతం_t)
{
      తిరిగి యొక్క_POW_పునాది(thr, అబద్ధం);
}

తార్కిక యొక్క_POW_S(vthread_t thr, vvp_సంకేతం_t)
{
      తిరిగి యొక్క_POW_పునాది(thr, నిజం);
}

తార్కిక యొక్క_POW_WR(vthread_t thr, vvp_సంకేతం_t)
{
      ద్విగుణ r = thr->తీయి_real();
      ద్విగుణ l = thr->తీయి_real();
      thr->నెట్టు_real(ఘాతం(l,r));

      తిరిగి నిజం;
}

/*
 * %prop/obj <pid>, <idx>
 *
 * Load an object value from the cobject and push it onto the object stack.
 */
తార్కిక యొక్క_PROP_OBJ(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని pid = cp->సంఖ్య;
      గుర్తులేని idx = cp->బిట్టు_idx[0];

      ఒకవేళ (idx != 0) {
	    ప్రతిజ్ఞా(idx < vthread_s::పదాలు_లెక్క);
	    idx = thr->పదాలు[idx].w_uint;
      }

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();

      vvp_object_t val;
      cobj->get_object(pid, val, idx);

      thr->నెట్టు_object(val);

      తిరిగి నిజం;
}

స్థిర శూన్య get_from_obj(గుర్తులేని pid, vvp_cobject*cobj, ద్విగుణ&val)
{
      val = cobj->get_real(pid);
}

స్థిర శూన్య get_from_obj(గుర్తులేని pid, vvp_cobject*cobj, string&val)
{
      val = cobj->get_string(pid);
}

స్థిర శూన్య get_from_obj(గుర్తులేని pid, vvp_cobject*cobj, vvp_vector4_t&val)
{
      cobj->get_vec4(pid, val);
}

మూస <రకం_పేరు ELEM>
స్థిర తార్కిక prop(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని pid = cp->సంఖ్య;

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      ప్రతిజ్ఞా(cobj);

      ELEM val;
      get_from_obj(pid, cobj, val);
      vthread_నెట్టు(thr, val);

      తిరిగి నిజం;
}

/*
 * %prop/r <pid>
 *
 * Load a real value from the cobject and push it onto the real value
 * stack.
 */
తార్కిక యొక్క_PROP_R(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి prop<ద్విగుణ>(thr, cp);
}

/*
 * %prop/str <pid>
 *
 * Load a string value from the cobject and push it onto the real value
 * stack.
 */
తార్కిక యొక్క_PROP_STR(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి prop<string>(thr, cp);
}

/*
 * %prop/v <pid>
 *
 * Load a property <pid> from the cobject on the top of the stack into
 * the vector space at <base>.
 */
తార్కిక యొక్క_PROP_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి prop<vvp_vector4_t>(thr, cp);
}

తార్కిక యొక్క_PUSHI_REAL(vthread_t thr, vvp_సంకేతం_t cp)
{
      ద్విగుణ mant = cp->బిట్టు_idx[0];
      uint32_t imant = cp->బిట్టు_idx[0];
      పూర్ణాంకం ఘాతాంకం = cp->బిట్టు_idx[1];

	// Detect +infinity
      ఒకవేళ (ఘాతాంకం==0x3fff && imant==0) {
	    thr->నెట్టు_real(INFINITY);
	    తిరిగి నిజం;
      }
	// Detect -infinity
      ఒకవేళ (ఘాతాంకం==0x7fff && imant==0) {
	    thr->నెట్టు_real(-INFINITY);
	    తిరిగి నిజం;
      }
	// Detect NaN
      ఒకవేళ (ఘాతాంకం==0x3fff) {
	    thr->నెట్టు_real(nan(""));
	    తిరిగి నిజం;
      }

      ద్విగుణ sign = (ఘాతాంకం & 0x4000)? -1.0 : 1.0;

      ఘాతాంకం &= 0x1fff;

      mant = sign * ldexp(mant, ఘాతాంకం - 0x1000);
      thr->నెట్టు_real(mant);
      తిరిగి నిజం;
}

తార్కిక యొక్క_PUSHI_STR(vthread_t thr, vvp_సంకేతం_t cp)
{
      స్థిరరాశి అక్షరం*పాఠ్యం = cp->పాఠ్యం;
      thr->నెట్టు_str(filter_string(పాఠ్యం));
      తిరిగి నిజం;
}

/*
 * %pushi/vec4 <vala>, <valb>, <wid>
 */
తార్కిక యొక్క_PUSHI_VEC4(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని wid  = cp->సంఖ్య;

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t val (wid, BIT4_0);
      get_తక్షణ_rval (cp, val);

      thr->నెట్టు_vec4(val);

      తిరిగి నిజం;
}

/*
 * %pushv/str
 *   Pops a vec4 value, and pushes a string.
 */
తార్కిక యొక్క_PUSHV_STR(vthread_t thr, vvp_సంకేతం_t)
{
      vvp_vector4_t vec = thr->తీయి_vec4();

      size_t slen = (vec.size() + 7)/8;
      vector<అక్షరం>buf;
      buf.reserve(slen);

      కొరకు (size_t idx = 0 ; idx < vec.size() ; idx += 8) {
	    అక్షరం tmp = 0;
	    size_t trans = 8;
	    ఒకవేళ (idx+trans > vec.size())
		  trans = vec.size() - idx;

	    కొరకు (size_t bdx = 0 ; bdx < trans ; bdx += 1) {
		  ఒకవేళ (vec.value(idx+bdx) == BIT4_1)
			tmp |= 1 << bdx;
	    }

	    ఒకవేళ (tmp != 0)
		  buf.push_back(tmp);
      }

      string val;
      కొరకు (vector<అక్షరం>::reverse_iterator cur = buf.rbegin()
		 ; cur != buf.rend() ; ++cur) {
	    val.push_back(*cur);
      }

      thr->నెట్టు_str(val);

      తిరిగి నిజం;
}

/*
 * %putc/str/vec4 <var>, <mux>
 */
తార్కిక యొక్క_PUTC_STR_VEC4(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని muxr = cp->బిట్టు_idx[0];
      int32_t mux = muxr? thr->పదాలు[muxr].w_int : 0;

      vvp_vector4_t val = thr->తీయి_vec4();
      ప్రతిజ్ఞా(val.size() == 8);

      ఒకవేళ (mux < 0)
	    తిరిగి నిజం;

	/* Get the existing value of the string. If we find that the
	   index is too big for the string, then give up. */
      vvp_net_t*net = cp->net;
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (net->fun);
      ప్రతిజ్ఞా(fun);

      string tmp = fun->get_string();
      ఒకవేళ (tmp.size() <= (size_t)mux)
	    తిరిగి నిజం;

      అక్షరం val_str = 0;
      కొరకు (size_t idx = 0 ; idx < 8 ; idx += 1) {
	    ఒకవేళ (val.value(idx)==BIT4_1)
		  val_str |= 1<<idx;
      }

	// It is a quirk of the Verilog standard that putc(..., 'h00)
	// has no effect. Test for that case here.
      ఒకవేళ (val_str == 0)
	    తిరిగి నిజం;

      tmp[mux] = val_str;

      vvp_send_string(vvp_net_ptr_t(cp->net, 0), tmp, thr->wt_context);
      తిరిగి నిజం;
}

మూస <రకం_పేరు ELEM, తరగతి QTYPE>
స్థిర తార్కిక qinsert(vthread_t thr, vvp_సంకేతం_t cp, గుర్తులేని wid=0)
{
      int64_t idx = thr->పదాలు[3].w_int;
      ELEM value;
      vvp_net_t*net = cp->net;
      తీయి_విలువ(thr, value, wid); // Pop the value to store.

      vvp_వరుస*వరుస = get_వరుస_object<QTYPE>(thr, net);
      ప్రతిజ్ఞా(వరుస);
      ఒకవేళ (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at a negative "
	         << get_వరుస_type(value)
	         << " index (" << idx << "). ";
	    print_వరుస_విలువ(value);
	    cerr << " was not added." << పంక్తి_ముగింపు;
      } లేకపోతే ఒకవేళ (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at an undefined "
	         << get_వరుస_type(value) << " index. ";
	    print_వరుస_విలువ(value);
	    cerr << " was not added." << పంక్తి_ముగింపు;
      } లేకపోతే {
	    గుర్తులేని max_కొలత = thr->పదాలు[cp->బిట్టు_idx[0]].w_int;
	    వరుస->insert(idx, value, max_కొలత);
      }
      తిరిగి నిజం;
}

/*
 * %qinsert/real <var-label>
 */
తార్కిక యొక్క_QINSERT_REAL(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి qinsert<ద్విగుణ, vvp_వరుస_real>(thr, cp);
}

/*
 * %qinsert/str <var-label>
 */
తార్కిక యొక్క_QINSERT_STR(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి qinsert<string, vvp_వరుస_string>(thr, cp);
}

/*
 * %qinsert/v <var-label>
 */
తార్కిక యొక్క_QINSERT_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి qinsert<vvp_vector4_t, vvp_వరుస_vec4>(thr, cp, cp->బిట్టు_idx[1]);
}

/*
 * Helper functions used in the queue pop templates
 */
అంతఃవరుస శూన్య నెట్టు_విలువ(vthread_t thr, ద్విగుణ value, గుర్తులేని)
{
      thr->నెట్టు_real(value);
}

అంతఃవరుస శూన్య నెట్టు_విలువ(vthread_t thr, స్థిరరాశి string&value, గుర్తులేని)
{
      thr->నెట్టు_str(value);
}

అంతఃవరుస శూన్య నెట్టు_విలువ(vthread_t thr, స్థిరరాశి vvp_vector4_t&value, గుర్తులేని wid)
{
      ప్రతిజ్ఞా(wid == value.size());
      thr->నెట్టు_vec4(value);
}

మూస <రకం_పేరు ELEM, తరగతి QTYPE>
స్థిర తార్కిక q_తీయి(vthread_t thr, vvp_సంకేతం_t cp,
                  శూన్య (*get_val_func)(vvp_వరుస*, ELEM&),
                  స్థిరరాశి అక్షరం*loc, గుర్తులేని wid)
{
      vvp_net_t*net = cp->net;

      vvp_వరుస*వరుస = get_వరుస_object<QTYPE>(thr, net);
      ప్రతిజ్ఞా(వరుస);

      size_t size = వరుస->get_కొలత();

      ELEM value;
      ఒకవేళ (size) {
	    get_val_func(వరుస, value);
      } లేకపోతే {
	    dq_default(value, wid);
	    cerr << thr->get_fileline()
	         << "Warning: pop_" << loc << "() on empty "
	         << get_వరుస_type(value) << "." << పంక్తి_ముగింపు;
      }

      నెట్టు_విలువ(thr, value, wid);
      తిరిగి నిజం;
}

మూస <రకం_పేరు ELEM>
స్థిర శూన్య get_వెనుక_విలువ(vvp_వరుస*వరుస, ELEM&value)
{
      వరుస->get_మాట(వరుస->get_కొలత()-1, value);
      వరుస->pop_back();
}

మూస <రకం_పేరు ELEM, తరగతి QTYPE>
స్థిర తార్కిక qpop_b(vthread_t thr, vvp_సంకేతం_t cp, గుర్తులేని wid=0)
{
      తిరిగి q_తీయి<ELEM, QTYPE>(thr, cp, get_వెనుక_విలువ<ELEM>, "back", wid);
}

/*
 * %qpop/b/real <var-label>
 */
తార్కిక యొక్క_QPOP_B_REAL(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి qpop_b<ద్విగుణ, vvp_వరుస_real>(thr, cp);
}

/*
 * %qpop/b/str <var-label>
 */
తార్కిక యొక్క_QPOP_B_STR(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి qpop_b<string, vvp_వరుస_string>(thr, cp);
}

/*
 * %qpop/b/v <var-label>
 */
తార్కిక యొక్క_QPOP_B_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి qpop_b<vvp_vector4_t, vvp_వరుస_vec4>(thr, cp, cp->బిట్టు_idx[0]);
}

మూస <రకం_పేరు ELEM>
స్థిర శూన్య get_front_విలువ(vvp_వరుస*వరుస, ELEM&value)
{
      వరుస->get_మాట(0, value);
      వరుస->pop_front();
}

మూస <రకం_పేరు ELEM, తరగతి QTYPE>
స్థిర తార్కిక qpop_f(vthread_t thr, vvp_సంకేతం_t cp, గుర్తులేని wid=0)
{
      తిరిగి q_తీయి<ELEM, QTYPE>(thr, cp, get_front_విలువ<ELEM>, "front", wid);
}


/*
 * %qpop/f/real <var-label>
 */
తార్కిక యొక్క_QPOP_F_REAL(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి qpop_f<ద్విగుణ, vvp_వరుస_real>(thr, cp);
}

/*
 * %qpop/f/str <var-label>
 */
తార్కిక యొక్క_QPOP_F_STR(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి qpop_f<string, vvp_వరుస_string>(thr, cp);
}

/*
 * %qpop/f/v <var-label>
 */
తార్కిక యొక్క_QPOP_F_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి qpop_f<vvp_vector4_t, vvp_వరుస_vec4>(thr, cp, cp->బిట్టు_idx[0]);
}

/*
 * These implement the %release/net and %release/reg instructions. The
 * %release/net instruction applies to a net kind of functor by
 * sending the release/net command to the command port. (See vvp_net.h
 * for details.) The %release/reg instruction is the same, but sends
 * the release/reg command instead. These are very similar to the
 * %deassign instruction.
 */
స్థిర తార్కిక do_release_vec(vvp_సంకేతం_t cp, తార్కిక net_flag)
{
      vvp_net_t*net = cp->net;
      గుర్తులేని పునాది  = cp->బిట్టు_idx[0];
      గుర్తులేని width = cp->బిట్టు_idx[1];

      ప్రతిజ్ఞా(net->fil);

      ఒకవేళ (పునాది >= net->fil->filter_కొలత()) తిరిగి నిజం;
      ఒకవేళ (పునాది+width > net->fil->filter_కొలత())
	    width = net->fil->filter_కొలత() - పునాది;

      తార్కిక నిండిన_sig = పునాది == 0 && width == net->fil->filter_కొలత();

	// XXXX Can't really do this if this is a partial release?
      net->fil->force_unlink();

	/* Do we release all or part of the net? */
      vvp_net_ptr_t ptr (net, 0);
      ఒకవేళ (నిండిన_sig) {
	    net->fil->release(ptr, net_flag);
      } లేకపోతే {
	    net->fil->release_pv(ptr, పునాది, width, net_flag);
      }
      net->fun->force_flag(అబద్ధం);

      తిరిగి నిజం;
}

తార్కిక యొక్క_RELEASE_NET(vthread_t, vvp_సంకేతం_t cp)
{
      తిరిగి do_release_vec(cp, నిజం);
}


తార్కిక యొక్క_RELEASE_REG(vthread_t, vvp_సంకేతం_t cp)
{
      తిరిగి do_release_vec(cp, అబద్ధం);
}

/* The type is 1 for registers and 0 for everything else. */
తార్కిక యొక్క_RELEASE_WR(vthread_t, vvp_సంకేతం_t cp)
{
      vvp_net_t*net = cp->net;
      గుర్తులేని type  = cp->బిట్టు_idx[0];

      ప్రతిజ్ఞా(net->fil);
      net->fil->force_unlink();

	// Send a command to this signal to unforce itself.
      vvp_net_ptr_t ptr (net, 0);
      net->fil->release(ptr, type==0);
      తిరిగి నిజం;
}

తార్కిక యొక్క_REPLICATE(vthread_t thr, vvp_సంకేతం_t cp)
{
      పూర్ణాంకం rept = cp->సంఖ్య;
      vvp_vector4_t val = thr->తీయి_vec4();
      vvp_vector4_t res (val.size() * rept, BIT4_X);

      కొరకు (పూర్ణాంకం idx = 0 ; idx < rept ; idx += 1) {
	    res.సమితి_vec(idx * val.size(), val);
      }

      thr->నెట్టు_vec4(res);

      తిరిగి నిజం;
}

స్థిర శూన్య poke_val(vthread_t fun_thr, గుర్తులేని లోతు, ద్విగుణ val)
{
      fun_thr->మాతృ->poke_real(లోతు, val);
}

స్థిర శూన్య poke_val(vthread_t fun_thr, గుర్తులేని లోతు, స్థిరరాశి string&val)
{
      fun_thr->మాతృ->poke_str(లోతు, val);
}

స్థిర size_t get_max(vthread_t fun_thr, ద్విగుణ&)
{
      తిరిగి fun_thr->వాదనలు_real.size();
}

స్థిర size_t get_max(vthread_t fun_thr, string&)
{
      తిరిగి fun_thr->వాదనలు_str.size();
}

స్థిర size_t get_max(vthread_t fun_thr, vvp_vector4_t&)
{
      తిరిగి fun_thr->వాదనలు_vec4.size();
}

స్థిర గుర్తులేని get_లోతు(vthread_t fun_thr, size_t సూచీ, ద్విగుణ&)
{
      తిరిగి fun_thr->వాదనలు_real[సూచీ];
}

స్థిర గుర్తులేని get_లోతు(vthread_t fun_thr, size_t సూచీ, string&)
{
      తిరిగి fun_thr->వాదనలు_str[సూచీ];
}

స్థిర గుర్తులేని get_లోతు(vthread_t fun_thr, size_t సూచీ, vvp_vector4_t&)
{
      తిరిగి fun_thr->వాదనలు_vec4[సూచీ];
}

స్థిర vthread_t get_func(vthread_t thr)
{
      vthread_t fun_thr = thr;

      అయితే (fun_thr->మాతృ_scope->get_type_సంకేతం() != vpiFunction) {
	    ప్రతిజ్ఞా(fun_thr->మాతృ);
	    fun_thr = fun_thr->మాతృ;
      }

      తిరిగి fun_thr;
}

మూస <రకం_పేరు ELEM>
స్థిర తార్కిక ret(vthread_t thr, vvp_సంకేతం_t cp)
{
      size_t సూచీ = cp->సంఖ్య;
      ELEM val;
      తీయి_విలువ(thr, val, 0);

      vthread_t fun_thr = get_func(thr);
      ప్రతిజ్ఞా(సూచీ < get_max(fun_thr, val));

      గుర్తులేని లోతు = get_లోతు(fun_thr, సూచీ, val);
	// Use the depth to put the value into the stack of
	// the parent thread.
      poke_val(fun_thr, లోతు, val);
      తిరిగి నిజం;
}

/*
 * %ret/real <index>
 */
తార్కిక యొక్క_RET_REAL(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి ret<ద్విగుణ>(thr, cp);
}

/*
 * %ret/str <index>
 */
తార్కిక యొక్క_RET_STR(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి ret<string>(thr, cp);
}

/*
 * %ret/vec4 <index>, <offset>, <wid>
 */
తార్కిక యొక్క_RET_VEC4(vthread_t thr, vvp_సంకేతం_t cp)
{
      size_t సూచీ = cp->సంఖ్య;
      గుర్తులేని off_సూచీ = cp->బిట్టు_idx[0];
      గుర్తులేని పూర్ణాంకం wid = cp->బిట్టు_idx[1];
      vvp_vector4_t&val = thr->peek_vec4();

      vthread_t fun_thr = get_func(thr);
      ప్రతిజ్ఞా(సూచీ < get_max(fun_thr, val));
      ప్రతిజ్ఞా(val.size() == wid);
      గుర్తులేని లోతు = get_లోతు(fun_thr, సూచీ, val);

      int64_t off = off_సూచీ ? thr->పదాలు[off_సూచీ].w_int : 0;
      గుర్తులేని పూర్ణాంకం sig_విలువ_కొలత = fun_thr->మాతృ->peek_vec4(లోతు).size();

      ఒకవేళ (off_సూచీ!=0 && thr->flags[4] == BIT4_1) {
	    thr->తీయి_vec4(1);
	    తిరిగి నిజం;
      }

      ఒకవేళ (!resize_rval_vec(val, off, sig_విలువ_కొలత)) {
	    thr->తీయి_vec4(1);
	    తిరిగి నిజం;
      }

      ఒకవేళ (off == 0 && val.size() == sig_విలువ_కొలత) {
	    fun_thr->మాతృ->poke_vec4(లోతు, val);
      } లేకపోతే {
	    vvp_vector4_t tmp_dst = fun_thr->మాతృ->peek_vec4(లోతు);
	    tmp_dst.సమితి_vec(off, val);
	    fun_thr->మాతృ->poke_vec4(లోతు, tmp_dst);
      }

      thr->తీయి_vec4(1);
      తిరిగి నిజం;
}

స్థిర శూన్య నెట్టు_from_మాతృ(vthread_t thr, vthread_t fun_thr, గుర్తులేని లోతు, ద్విగుణ&)
{
      thr->నెట్టు_real(fun_thr->మాతృ->peek_real(లోతు));
}

స్థిర శూన్య నెట్టు_from_మాతృ(vthread_t thr, vthread_t fun_thr, గుర్తులేని లోతు, string&)
{
      thr->నెట్టు_str(fun_thr->మాతృ->peek_str(లోతు));
}

స్థిర శూన్య నెట్టు_from_మాతృ(vthread_t thr, vthread_t fun_thr, గుర్తులేని లోతు, vvp_vector4_t&)
{
      thr->నెట్టు_vec4(fun_thr->మాతృ->peek_vec4(లోతు));
}

మూస <రకం_పేరు ELEM>
స్థిర తార్కిక retload(vthread_t thr, vvp_సంకేతం_t cp)
{
      size_t సూచీ = cp->సంఖ్య;
      ELEM type;

      vthread_t fun_thr = get_func(thr);
      ప్రతిజ్ఞా(సూచీ < get_max(fun_thr, type));

      గుర్తులేని లోతు = get_లోతు(fun_thr, సూచీ, type);
	// Use the depth to extract the values from the stack
	// of the parent thread.
      నెట్టు_from_మాతృ(thr, fun_thr, లోతు, type);
      తిరిగి నిజం;
}

/*
 * %retload/real <index>
 */
తార్కిక యొక్క_RETLOAD_REAL(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి retload<ద్విగుణ>(thr, cp);
}

/*
 * %retload/str <index>
 */
తార్కిక యొక్క_RETLOAD_STR(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి retload<string>(thr, cp);
}

/*
 * %retload/vec4 <index>
 */
తార్కిక యొక్క_RETLOAD_VEC4(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి retload<vvp_vector4_t>(thr, cp);
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
తార్కిక యొక్క_SCOPY(vthread_t thr, vvp_సంకేతం_t)
{
      vvp_object_t tmp;
      thr->తీయి_object(tmp);

      vvp_object_t&dest = thr->peek_object();
        // If it is null there is nothing to copy
      ఒకవేళ (!tmp.test_nil())
	    dest.shallow_copy(tmp);

      తిరిగి నిజం;
}

స్థిర శూన్య thread_peek(vthread_t thr, ద్విగుణ&value)
{
      value = thr->peek_real(0);
}

స్థిర శూన్య thread_peek(vthread_t thr, string&value)
{
      value = thr->peek_str(0);
}

స్థిర శూన్య thread_peek(vthread_t thr, vvp_vector4_t&value)
{
      value = thr->peek_vec4(0);
}

మూస <రకం_పేరు ELEM>
స్థిర తార్కిక సమితి_dar_obj(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని adr = thr->పదాలు[cp->సంఖ్య].w_int;

      ELEM value;
      thread_peek(thr, value);

      vvp_object_t&శిఖరం = thr->peek_object();
      vvp_darray*darray = శిఖరం.peek<vvp_darray>();
      ప్రతిజ్ఞా(darray);

      darray->సమితి_మాట(adr, value);
      తిరిగి నిజం;
}

/*
 * %set/dar/obj/real <index>
 */
తార్కిక యొక్క_సమితి_DAR_OBJ_REAL(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి సమితి_dar_obj<ద్విగుణ>(thr, cp);
}

/*
 * %set/dar/obj/str <index>
 */
తార్కిక యొక్క_సమితి_DAR_OBJ_STR(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి సమితి_dar_obj<string>(thr, cp);
}

/*
 * %set/dar/obj/vec4 <index>
 */
తార్కిక యొక్క_సమితి_DAR_OBJ_VEC4(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి సమితి_dar_obj<vvp_vector4_t>(thr, cp);
}

/*
 * %shiftl <idx>
 *
 * Pop the operand, then push the result.
 */
తార్కిక యొక్క_SHIFTL(vthread_t thr, vvp_సంకేతం_t cp)
{
      పూర్ణాంకం use_సూచీ = cp->సంఖ్య;
      uint64_t జరుపు = thr->పదాలు[use_సూచీ].w_uint;

      vvp_vector4_t&val = thr->peek_vec4();
      గుర్తులేని wid  = val.size();

      ఒకవేళ (thr->flags[4] == BIT4_1) {
	      // The result is 'bx if the shift amount is undefined
	    val = vvp_vector4_t(wid, BIT4_X);

      } లేకపోతే ఒకవేళ (thr->flags[4] == BIT4_X || జరుపు >= wid) {
	      // Shift is so big that all value is shifted out. Write
	      // a constant 0 result.
	    val = vvp_vector4_t(wid, BIT4_0);

      } లేకపోతే ఒకవేళ (జరుపు > 0) {
	    vvp_vector4_t blk = val.subvalue(0, wid-జరుపు);
	    vvp_vector4_t tmp (జరుపు, BIT4_0);
	    val.సమితి_vec(0, tmp);
	    val.సమితి_vec(జరుపు, blk);
      }

      తిరిగి నిజం;
}

/*
 * %shiftr <idx>
 * This is an unsigned right shift. The <idx> is a number that selects
 * the index register with the amount of the shift. This instruction
 * checks flag bit 4, which will be true if the shift is invalid.
 */
తార్కిక యొక్క_SHIFTR(vthread_t thr, vvp_సంకేతం_t cp)
{
      పూర్ణాంకం use_సూచీ = cp->సంఖ్య;
      uint64_t జరుపు = thr->పదాలు[use_సూచీ].w_uint;

      vvp_vector4_t val = thr->తీయి_vec4();
      గుర్తులేని wid  = val.size();

      ఒకవేళ (thr->flags[4] == BIT4_1) {
	    val = vvp_vector4_t(wid, BIT4_X);

      } లేకపోతే ఒకవేళ (thr->flags[4] == BIT4_X || జరుపు > wid) {
	    val = vvp_vector4_t(wid, BIT4_0);

      } లేకపోతే ఒకవేళ (జరుపు > 0) {
	    vvp_vector4_t blk = val.subvalue(జరుపు, wid-జరుపు);
	    vvp_vector4_t tmp (జరుపు, BIT4_0);
	    val.సమితి_vec(0, blk);
	    val.సమితి_vec(wid-జరుపు, tmp);
      }

      thr->నెట్టు_vec4(val);
      తిరిగి నిజం;
}

/*
 *  %shiftr/s <wid>
 */
తార్కిక యొక్క_SHIFTR_S(vthread_t thr, vvp_సంకేతం_t cp)
{
      పూర్ణాంకం use_సూచీ = cp->సంఖ్య;
      uint64_t జరుపు = thr->పదాలు[use_సూచీ].w_uint;

      vvp_vector4_t val = thr->తీయి_vec4();
      గుర్తులేని wid  = val.size();

      vvp_bit4_t sign_బిట్టు = val.value(val.size()-1);

      ఒకవేళ (thr->flags[4] == BIT4_1) {
	    val = vvp_vector4_t(wid, BIT4_X);

      } లేకపోతే ఒకవేళ (thr->flags[4] == BIT4_X || జరుపు > wid) {
	    val = vvp_vector4_t(wid, sign_బిట్టు);

      } లేకపోతే ఒకవేళ (జరుపు > 0) {
	    vvp_vector4_t blk = val.subvalue(జరుపు, wid-జరుపు);
	    vvp_vector4_t tmp (జరుపు, sign_బిట్టు);
	    val.సమితి_vec(0, blk);
	    val.సమితి_vec(wid-జరుపు, tmp);
      }

      thr->నెట్టు_vec4(val);
      తిరిగి నిజం;
}

/*
 * %split/vec4 <wid>
 *   Pop 1 value,
 *   Take <wid> bits from the lsb,
 *   Push the remaining msb,
 *   Push the lsb.
 */
తార్కిక యొక్క_విభజించు_VEC4(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని lsb_wid = cp->సంఖ్య;

      vvp_vector4_t&val = thr->peek_vec4();
      ప్రతిజ్ఞా(lsb_wid < val.size());

      vvp_vector4_t lsb = val.subvalue(0, lsb_wid);
      val = val.subvalue(lsb_wid, val.size()-lsb_wid);

      thr->నెట్టు_vec4(lsb);
      తిరిగి నిజం;
}

/*
 * The following are used to allow the darray templates to print correctly.
 */
అంతఃవరుస స్థిర string get_darray_type(ద్విగుణ&)
{
      తిరిగి "darray<real>";
}

అంతఃవరుస స్థిర string get_darray_type(string&)
{
      తిరిగి "darray<string>";
}

అంతఃవరుస స్థిర string get_darray_type(స్థిరరాశి vvp_vector4_t&value)
{
      ostringstream buf;
      buf << "darray<vector[" << value.size() << "]>";
      string res = buf.str();
      తిరిగి res;
}

/*
 * The following are used to allow a common template to be written for
 * darray real/string/vec4 operations
 */
అంతఃవరుస స్థిర శూన్య dar_తీయి_విలువ(vthread_t thr, ద్విగుణ&value)
{
      value = thr->తీయి_real();
}

అంతఃవరుస స్థిర శూన్య dar_తీయి_విలువ(vthread_t thr, string&value)
{
      value = thr->తీయి_str();
}

అంతఃవరుస స్థిర శూన్య dar_తీయి_విలువ(vthread_t thr, vvp_vector4_t&value)
{
      value = thr->తీయి_vec4();
}

మూస <రకం_పేరు ELEM>
స్థిర తార్కిక store_dar(vthread_t thr, vvp_సంకేతం_t cp)
{
      int64_t adr = thr->పదాలు[3].w_int;
      ELEM value;
	// FIXME: Can we get the size of the underlying array element
	//        and then use the normal pop_value?
      dar_తీయి_విలువ(thr, value);

      vvp_net_t*net = cp->net;
      ప్రతిజ్ఞా(net);

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      ప్రతిజ్ఞా(obj);

      vvp_darray*darray = obj->get_object().peek<vvp_darray>();

      ఒకవేళ (adr < 0)
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to a negative " << get_darray_type(value)
	         << " index (" << adr << ")." << పంక్తి_ముగింపు;
      లేకపోతే ఒకవేళ (thr->flags[4] != BIT4_0)
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to an undefined " << get_darray_type(value)
	         << " index." << పంక్తి_ముగింపు;
      లేకపోతే ఒకవేళ (darray)
	    darray->సమితి_మాట(adr, value);
      లేకపోతే
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to an undefined " << get_darray_type(value)
	         << "." << పంక్తి_ముగింపు;

      తిరిగి నిజం;
}

/*
 * %store/dar/real <var>
 */
తార్కిక యొక్క_STORE_DAR_R(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి store_dar<ద్విగుణ>(thr, cp);
}

/*
 * %store/dar/str <var>
 */
తార్కిక యొక్క_STORE_DAR_STR(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి store_dar<string>(thr, cp);
}

/*
 * %store/dar/vec4 <var>
 */
తార్కిక యొక్క_STORE_DAR_VEC4(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి store_dar<vvp_vector4_t>(thr, cp);
}

తార్కిక యొక్క_STORE_OBJ(vthread_t thr, vvp_సంకేతం_t cp)
{
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);

      vvp_object_t val;
      thr->తీయి_object(val);

      vvp_send_object(ptr, val, thr->wt_context);

      తిరిగి నిజం;
}

/*
 * %store/obja <array-label> <index>
 */
తార్కిక యొక్క_STORE_OBJA(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని idx = cp->బిట్టు_idx[0];
      గుర్తులేని adr = thr->పదాలు[idx].w_int;

      vvp_object_t val;
      thr->తీయి_object(val);

      cp->array->సమితి_మాట(adr, val);

      తిరిగి నిజం;
}


/*
 * %store/prop/obj <pid>, <idx>
 *
 * Pop an object value from the object stack, and store the value into
 * the property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
తార్కిక యొక్క_STORE_PROP_OBJ(vthread_t thr, vvp_సంకేతం_t cp)
{
      size_t pid = cp->సంఖ్య;
      గుర్తులేని idx = cp->బిట్టు_idx[0];

      ఒకవేళ (idx != 0) {
	    ప్రతిజ్ఞా(idx < vthread_s::పదాలు_లెక్క);
	    idx = thr->పదాలు[idx].w_uint;
      }

      vvp_object_t val;
      thr->తీయి_object(val);

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      ప్రతిజ్ఞా(cobj);

      cobj->సమితి_object(pid, val, idx);

      తిరిగి నిజం;
}

స్థిర శూన్య తీయి_prop_val(vthread_t thr, ద్విగుణ&val, గుర్తులేని)
{
      val = thr->తీయి_real();
}

స్థిర శూన్య తీయి_prop_val(vthread_t thr, string&val, గుర్తులేని)
{
      val = thr->తీయి_str();
}

స్థిర శూన్య తీయి_prop_val(vthread_t thr, vvp_vector4_t&val, గుర్తులేని wid)
{
      val = thr->తీయి_vec4();
      ప్రతిజ్ఞా(val.size() >= wid);
      val.resize(wid);
}

స్థిర శూన్య సమితి_val(vvp_cobject*cobj, size_t pid, స్థిరరాశి ద్విగుణ&val)
{
      cobj->సమితి_real(pid, val);
}

స్థిర శూన్య సమితి_val(vvp_cobject*cobj, size_t pid, స్థిరరాశి string&val)
{
      cobj->సమితి_string(pid, val);
}

స్థిర శూన్య సమితి_val(vvp_cobject*cobj, size_t pid, స్థిరరాశి vvp_vector4_t&val)
{
      cobj->సమితి_vec4(pid, val);
}

మూస <రకం_పేరు ELEM>
స్థిర తార్కిక store_prop(vthread_t thr, vvp_సంకేతం_t cp, గుర్తులేని wid=0)
{
      size_t pid = cp->సంఖ్య;
      ELEM val;
      తీయి_prop_val(thr, val, wid); // Pop the value to store.

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      ప్రతిజ్ఞా(cobj);

      సమితి_val(cobj, pid, val);

      తిరిగి నిజం;
}

/*
 * %store/prop/r <id>
 *
 * Pop a real value from the real stack, and store the value into the
 * property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
తార్కిక యొక్క_STORE_PROP_R(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి store_prop<ద్విగుణ>(thr, cp);
}

/*
 * %store/prop/str <id>
 *
 * Pop a string value from the string stack, and store the value into
 * the property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
తార్కిక యొక్క_STORE_PROP_STR(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి store_prop<string>(thr, cp);
}

/*
 * %store/prop/v <pid>, <wid>
 *
 * Store vector value into property <id> of cobject in the top of the
 * stack. Do NOT pop the object stack.
 */
తార్కిక యొక్క_STORE_PROP_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి store_prop<vvp_vector4_t>(thr, cp, cp->బిట్టు_idx[0]);
}

మూస <తరగతి QTYPE>
స్థిర QTYPE* get_వరుస_prop(vvp_cobject*cobj, size_t pid)
{
      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      QTYPE* వరుస = qobj.peek<QTYPE>();
      ఒకవేళ (వరుస == 0) {
	    వరుస = కొత్త QTYPE;
	    vvp_object_t val(వరుస);
	    cobj->సమితి_object(pid, val, 0);
      }
      తిరిగి వరుస;
}

మూస <రకం_పేరు ELEM, తరగతి QTYPE>
స్థిర తార్కిక store_prop_qb(vthread_t thr, vvp_సంకేతం_t cp, గుర్తులేని wid)
{
      size_t pid = cp->సంఖ్య;
      ELEM value;
      గుర్తులేని max_కొలత = thr->పదాలు[cp->బిట్టు_idx[0]].w_uint;
      తీయి_విలువ(thr, value, wid);

      vvp_object_t& శిఖరం = thr->peek_object();
      vvp_cobject*cobj = శిఖరం.peek<vvp_cobject>();
      ప్రతిజ్ఞా(cobj);

      QTYPE* వరుస = get_వరుస_prop<QTYPE>(cobj, pid);
      ప్రతిజ్ఞా(వరుస);
      వరుస->push_back(value, max_కొలత);
      తిరిగి నిజం;
}

మూస <రకం_పేరు ELEM, తరగతి QTYPE>
స్థిర తార్కిక store_prop_qf(vthread_t thr, vvp_సంకేతం_t cp, గుర్తులేని wid)
{
      size_t pid = cp->సంఖ్య;
      ELEM value;
      గుర్తులేని max_కొలత = thr->పదాలు[cp->బిట్టు_idx[0]].w_uint;
      తీయి_విలువ(thr, value, wid);

      vvp_object_t& శిఖరం = thr->peek_object();
      vvp_cobject*cobj = శిఖరం.peek<vvp_cobject>();
      ప్రతిజ్ఞా(cobj);

      QTYPE* వరుస = get_వరుస_prop<QTYPE>(cobj, pid);
      ప్రతిజ్ఞా(వరుస);
      వరుస->push_front(value, max_కొలత);
      తిరిగి నిజం;
}

తార్కిక యొక్క_STORE_PROP_QB_R(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి store_prop_qb<ద్విగుణ, vvp_వరుస_real>(thr, cp, 0);
}

తార్కిక యొక్క_STORE_PROP_QB_STR(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి store_prop_qb<string, vvp_వరుస_string>(thr, cp, 0);
}

తార్కిక యొక్క_STORE_PROP_QB_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి store_prop_qb<vvp_vector4_t, vvp_వరుస_vec4>(thr, cp, cp->బిట్టు_idx[1]);
}

తార్కిక యొక్క_STORE_PROP_QF_R(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి store_prop_qf<ద్విగుణ, vvp_వరుస_real>(thr, cp, 0);
}

తార్కిక యొక్క_STORE_PROP_QF_STR(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి store_prop_qf<string, vvp_వరుస_string>(thr, cp, 0);
}

తార్కిక యొక్క_STORE_PROP_QF_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి store_prop_qf<vvp_vector4_t, vvp_వరుస_vec4>(thr, cp, cp->బిట్టు_idx[1]);
}

మూస <రకం_పేరు ELEM, తరగతి QTYPE>
స్థిర తార్కిక qinsert_prop(vthread_t thr, vvp_సంకేతం_t cp, గుర్తులేని wid)
{
      int64_t idx = thr->పదాలు[3].w_int;
      ELEM value;
      size_t pid = cp->సంఖ్య;
      తీయి_విలువ(thr, value, wid);

      vvp_object_t& శిఖరం = thr->peek_object();
      vvp_cobject*cobj = శిఖరం.peek<vvp_cobject>();
      ప్రతిజ్ఞా(cobj);

      QTYPE* వరుస = get_వరుస_prop<QTYPE>(cobj, pid);
      ప్రతిజ్ఞా(వరుస);
      ఒకవేళ (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at a negative "
	         << get_వరుస_type(value)
	         << " index (" << idx << "). ";
	    print_వరుస_విలువ(value);
	    cerr << " was not added." << పంక్తి_ముగింపు;
      } లేకపోతే ఒకవేళ (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at an undefined "
	         << get_వరుస_type(value) << " index. ";
	    print_వరుస_విలువ(value);
	    cerr << " was not added." << పంక్తి_ముగింపు;
      } లేకపోతే {
	    గుర్తులేని max_కొలత = thr->పదాలు[cp->బిట్టు_idx[0]].w_int;
	    వరుస->insert(idx, value, max_కొలత);
      }
      తిరిగి నిజం;
}

తార్కిక యొక్క_QINSERT_PROP_R(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి qinsert_prop<ద్విగుణ, vvp_వరుస_real>(thr, cp, 0);
}

తార్కిక యొక్క_QINSERT_PROP_STR(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి qinsert_prop<string, vvp_వరుస_string>(thr, cp, 0);
}

తార్కిక యొక్క_QINSERT_PROP_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి qinsert_prop<vvp_vector4_t, vvp_వరుస_vec4>(thr, cp, cp->బిట్టు_idx[1]);
}

మూస <రకం_పేరు ELEM, తరగతి QTYPE>
స్థిర తార్కిక q_తీయి_prop(vthread_t thr, vvp_సంకేతం_t cp,
                       శూన్య (*get_val_func)(vvp_వరుస*, ELEM&),
                       స్థిరరాశి అక్షరం*loc, గుర్తులేని wid)
{
      size_t pid = cp->సంఖ్య;

      vvp_object_t& శిఖరం = thr->peek_object();
      vvp_cobject*cobj = శిఖరం.peek<vvp_cobject>();
      ప్రతిజ్ఞా(cobj);

      QTYPE* వరుస = get_వరుస_prop<QTYPE>(cobj, pid);
      ప్రతిజ్ఞా(వరుస);

      size_t size = వరుస->get_కొలత();

      ELEM value;
      ఒకవేళ (size) {
	    get_val_func(వరుస, value);
      } లేకపోతే {
	    dq_default(value, wid);
	    cerr << thr->get_fileline()
	         << "Warning: pop_" << loc << "() on empty "
	         << get_వరుస_type(value) << "." << పంక్తి_ముగింపు;
      }

      నెట్టు_విలువ(thr, value, wid);
      తిరిగి నిజం;
}

మూస <రకం_పేరు ELEM, తరగతి QTYPE>
స్థిర తార్కిక qpop_b_prop(vthread_t thr, vvp_సంకేతం_t cp, గుర్తులేని wid)
{
      తిరిగి q_తీయి_prop<ELEM, QTYPE>(thr, cp, get_వెనుక_విలువ<ELEM>, "back", wid);
}

మూస <రకం_పేరు ELEM, తరగతి QTYPE>
స్థిర తార్కిక qpop_f_prop(vthread_t thr, vvp_సంకేతం_t cp, గుర్తులేని wid)
{
      తిరిగి q_తీయి_prop<ELEM, QTYPE>(thr, cp, get_front_విలువ<ELEM>, "front", wid);
}

తార్కిక యొక్క_QPOP_PROP_B_REAL(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి qpop_b_prop<ద్విగుణ, vvp_వరుస_real>(thr, cp, 0);
}

తార్కిక యొక్క_QPOP_PROP_B_STR(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి qpop_b_prop<string, vvp_వరుస_string>(thr, cp, 0);
}

తార్కిక యొక్క_QPOP_PROP_B_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి qpop_b_prop<vvp_vector4_t, vvp_వరుస_vec4>(thr, cp, cp->బిట్టు_idx[0]);
}

తార్కిక యొక్క_QPOP_PROP_F_REAL(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి qpop_f_prop<ద్విగుణ, vvp_వరుస_real>(thr, cp, 0);
}

తార్కిక యొక్క_QPOP_PROP_F_STR(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి qpop_f_prop<string, vvp_వరుస_string>(thr, cp, 0);
}

తార్కిక యొక్క_QPOP_PROP_F_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి qpop_f_prop<vvp_vector4_t, vvp_వరుస_vec4>(thr, cp, cp->బిట్టు_idx[0]);
}

స్థిర vvp_vector4_t వరుస_unique_ulong_గమ్యం_vec4(గుర్తులేని దీర్ఘ ul, గుర్తులేని బిట్లు)
{
      vvp_vector4_t val(బిట్లు, BIT4_0);
      కొరకు (గుర్తులేని idx = 0; idx < బిట్లు && idx < 8 * పరిమాణం(గుర్తులేని దీర్ఘ); idx += 1) {
	    ఒకవేళ (ul & 1UL) {
		  val.సమితి_బిట్టు(idx, BIT4_1);
	    } లేకపోతే {
		  val.సమితి_బిట్టు(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      తిరిగి val;
}

మూస <తరగతి SRC> స్థిర vvp_వరుస_vec4* వరుస_నిరంతరం_unique_src(SRC* src,
								 గుర్తులేని wid,
								 తార్కిక as_సూచీ)
{
      vvp_వరుస_vec4* dst = కొత్త vvp_వరుస_vec4();
      ఒకవేళ (!src) తిరిగి dst;
      size_t n = src->get_కొలత();
      కొరకు (size_t i = 0; i < n; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_మాట(i, vi);
	    తార్కిక చూసిన = అబద్ధం;
	    కొరకు (size_t j = 0; j < i; j += 1) {
		  vvp_vector4_t vj(wid);
		  src->get_మాట(j, vj);
		  ఒకవేళ (vi.eeq(vj)) {
			చూసిన = నిజం;
			ఆపు;
		  }
	    }
	    ఒకవేళ (చూసిన) కొనసాగు;
	    ఒకవేళ (as_సూచీ) {
		  dst->push_back(వరుస_unique_ulong_గమ్యం_vec4((గుర్తులేని దీర్ఘ)i, 32), 0);
	    } లేకపోతే {
		  dst->push_back(vi, 0);
	    }
      }
      తిరిగి dst;
}

మూస <తరగతి SRC>
స్థిర vvp_వరుస_vec4* వరుస_నిరంతరం_కనుగొను_matches_src(SRC* src, గుర్తులేని wid,
						  స్థిరరాశి vvp_vector4_t& cmp,
						  తార్కిక as_సూచీ)
{
      vvp_వరుస_vec4* dst = కొత్త vvp_వరుస_vec4();
      ఒకవేళ (!src) తిరిగి dst;
      size_t n = src->get_కొలత();
      కొరకు (size_t i = 0; i < n; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_మాట(i, vi);
	    ఒకవేళ (vi.eeq(cmp)) {
		  ఒకవేళ (as_సూచీ) {
			dst->push_back(వరుస_unique_ulong_గమ్యం_vec4((గుర్తులేని దీర్ఘ)i, 32), 0);
		  } లేకపోతే {
			dst->push_back(vi, 0);
		  }
	    }
      }
      తిరిగి dst;
}

/* Array/queue locator methods return a queue with 0 or 1 elements (LRM). */
మూస <తరగతి SRC>
స్థిర vvp_వరుస_vec4* వరుస_కనుగొను_మొదటి_చివరి_గమ్యం_వరుస_src(SRC* src,
							  గుర్తులేని wid,
							  స్థిరరాశి vvp_vector4_t& cmp,
							  తార్కిక want_మొదటి,
							  తార్కిక want_సూచీ)
{
      vvp_వరుస_vec4* dst = కొత్త vvp_వరుస_vec4();
      ఒకవేళ (!src || src->get_కొలత() == 0) తిరిగి dst;
      size_t n = src->get_కొలత();
      ఒకవేళ (want_మొదటి) {
	    కొరకు (size_t i = 0; i < n; i += 1) {
		  vvp_vector4_t vi(wid);
		  src->get_మాట(i, vi);
		  ఒకవేళ (vi.eeq(cmp)) {
			ఒకవేళ (want_సూచీ) {
			      dst->push_back(
				  వరుస_unique_ulong_గమ్యం_vec4((గుర్తులేని దీర్ఘ)i, 32),
				  0);
			} లేకపోతే {
			      dst->push_back(vi, 0);
			}
			తిరిగి dst;
		  }
	    }
      } లేకపోతే {
	    కొరకు (size_t i = n; i > 0; ) {
		  i -= 1;
		  vvp_vector4_t vi(wid);
		  src->get_మాట(i, vi);
		  ఒకవేళ (vi.eeq(cmp)) {
			ఒకవేళ (want_సూచీ) {
			      dst->push_back(
				  వరుస_unique_ulong_గమ్యం_vec4((గుర్తులేని దీర్ఘ)i, 32),
				  0);
			} లేకపోతే {
			      dst->push_back(vi, 0);
			}
			తిరిగి dst;
		  }
	    }
      }
      తిరిగి dst;
}

/* min()/max() return one element with the selected extremum. */
మూస <తరగతి SRC>
స్థిర vvp_వరుస_vec4* వరుస_నిరంతరం_min_max_src(SRC* src, గుర్తులేని wid, తార్కిక want_max)
{
      vvp_వరుస_vec4* dst = కొత్త vvp_వరుస_vec4();
      ఒకవేళ (!src || src->get_కొలత() == 0) తిరిగి dst;

      vvp_vector4_t ఉత్తమ(wid);
      src->get_మాట(0, ఉత్తమ);

      size_t n = src->get_కొలత();
      కొరకు (size_t i = 1 ; i < n ; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_మాట(i, vi);

	    vvp_bit4_t ge = compare_gtge(vi, ఉత్తమ, BIT4_1);
	    తార్కిక replace = want_max ? (ge == BIT4_1) : (ge == BIT4_0);
	    ఒకవేళ (replace) ఉత్తమ = vi;
      }

      dst->push_back(ఉత్తమ, 0);
      తిరిగి dst;
}

తార్కిక యొక్క_వరుస_కనుగొను_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_vector4_t cmp = thr->తీయి_vec4();
      గుర్తులేని wid = cp->బిట్టు_idx[0];
      ప్రతిజ్ఞా(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_వరుస_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_వరుస_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_వరుస_vec4* dst;
      ఒకవేళ (qsrc) dst = వరుస_నిరంతరం_కనుగొను_matches_src(qsrc, wid, cmp, అబద్ధం);
      లేకపోతే ఒకవేళ (dsrc) dst = వరుస_నిరంతరం_కనుగొను_matches_src(dsrc, wid, cmp, అబద్ధం);
      లేకపోతే {
	    vvp_వరుస* src_q = get_వరుస_object<vvp_వరుస_vec4>(thr, net);
	    vvp_వరుస_vec4* src = dynamic_cast<vvp_వరుస_vec4*>(src_q);
	    dst = వరుస_నిరంతరం_కనుగొను_matches_src(src, wid, cmp, అబద్ధం);
      }
      thr->నెట్టు_object(vvp_object_t(dst));
      తిరిగి నిజం;
}

తార్కిక యొక్క_వరుస_కనుగొను_PROP_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_vector4_t cmp = thr->తీయి_vec4();
      size_t pid = cp->సంఖ్య;
      గుర్తులేని wid = cp->బిట్టు_idx[0];
      ప్రతిజ్ఞా(cmp.size() == wid);

      vvp_object_t& శిఖరం = thr->peek_object();
      vvp_cobject*cobj = శిఖరం.peek<vvp_cobject>();
      ప్రతిజ్ఞా(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_వరుస_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_వరుస_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_వరుస_vec4* dst = qsrc ? వరుస_నిరంతరం_కనుగొను_matches_src(qsrc, wid, cmp, అబద్ధం)
				 : వరుస_నిరంతరం_కనుగొను_matches_src(dsrc, wid, cmp, అబద్ధం);
      thr->నెట్టు_object(vvp_object_t(dst));
      తిరిగి నిజం;
}

తార్కిక యొక్క_వరుస_కనుగొను_సూచీ_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_vector4_t cmp = thr->తీయి_vec4();
      గుర్తులేని wid = cp->బిట్టు_idx[0];
      ప్రతిజ్ఞా(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_వరుస_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_వరుస_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_వరుస_vec4* dst;
      ఒకవేళ (qsrc) dst = వరుస_నిరంతరం_కనుగొను_matches_src(qsrc, wid, cmp, నిజం);
      లేకపోతే ఒకవేళ (dsrc) dst = వరుస_నిరంతరం_కనుగొను_matches_src(dsrc, wid, cmp, నిజం);
      లేకపోతే {
	    vvp_వరుస* src_q = get_వరుస_object<vvp_వరుస_vec4>(thr, net);
	    vvp_వరుస_vec4* src = dynamic_cast<vvp_వరుస_vec4*>(src_q);
	    dst = వరుస_నిరంతరం_కనుగొను_matches_src(src, wid, cmp, నిజం);
      }
      thr->నెట్టు_object(vvp_object_t(dst));
      తిరిగి నిజం;
}

తార్కిక యొక్క_వరుస_కనుగొను_సూచీ_PROP_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_vector4_t cmp = thr->తీయి_vec4();
      size_t pid = cp->సంఖ్య;
      గుర్తులేని wid = cp->బిట్టు_idx[0];
      ప్రతిజ్ఞా(cmp.size() == wid);

      vvp_object_t& శిఖరం = thr->peek_object();
      vvp_cobject*cobj = శిఖరం.peek<vvp_cobject>();
      ప్రతిజ్ఞా(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_వరుస_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_వరుస_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_వరుస_vec4* dst = qsrc ? వరుస_నిరంతరం_కనుగొను_matches_src(qsrc, wid, cmp, నిజం)
				 : వరుస_నిరంతరం_కనుగొను_matches_src(dsrc, wid, cmp, నిజం);
      thr->నెట్టు_object(vvp_object_t(dst));
      తిరిగి నిజం;
}

తార్కిక యొక్క_వరుస_కనుగొను_మొదటి_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_vector4_t cmp = thr->తీయి_vec4();
      గుర్తులేని wid = cp->బిట్టు_idx[0];
      ప్రతిజ్ఞా(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_వరుస_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_వరుస_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_వరుస_vec4* dst;
      ఒకవేళ (qsrc) dst = వరుస_కనుగొను_మొదటి_చివరి_గమ్యం_వరుస_src(qsrc, wid, cmp, నిజం, అబద్ధం);
      లేకపోతే ఒకవేళ (dsrc) dst = వరుస_కనుగొను_మొదటి_చివరి_గమ్యం_వరుస_src(dsrc, wid, cmp, నిజం, అబద్ధం);
      లేకపోతే {
	    vvp_వరుస* src_q = get_వరుస_object<vvp_వరుస_vec4>(thr, net);
	    vvp_వరుస_vec4* src = dynamic_cast<vvp_వరుస_vec4*>(src_q);
	    dst = వరుస_కనుగొను_మొదటి_చివరి_గమ్యం_వరుస_src(src, wid, cmp, నిజం, అబద్ధం);
      }
      thr->నెట్టు_object(vvp_object_t(dst));
      తిరిగి నిజం;
}

తార్కిక యొక్క_వరుస_కనుగొను_మొదటి_PROP_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_vector4_t cmp = thr->తీయి_vec4();
      size_t pid = cp->సంఖ్య;
      గుర్తులేని wid = cp->బిట్టు_idx[0];
      ప్రతిజ్ఞా(cmp.size() == wid);

      vvp_object_t& శిఖరం = thr->peek_object();
      vvp_cobject*cobj = శిఖరం.peek<vvp_cobject>();
      ప్రతిజ్ఞా(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_వరుస_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_వరుస_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_వరుస_vec4* dst =
	    qsrc ? వరుస_కనుగొను_మొదటి_చివరి_గమ్యం_వరుస_src(qsrc, wid, cmp, నిజం, అబద్ధం)
		 : వరుస_కనుగొను_మొదటి_చివరి_గమ్యం_వరుస_src(dsrc, wid, cmp, నిజం, అబద్ధం);
      thr->నెట్టు_object(vvp_object_t(dst));
      తిరిగి నిజం;
}

తార్కిక యొక్క_వరుస_కనుగొను_మొదటి_సూచీ_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_vector4_t cmp = thr->తీయి_vec4();
      గుర్తులేని wid = cp->బిట్టు_idx[0];
      ప్రతిజ్ఞా(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_వరుస_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_వరుస_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_వరుస_vec4* dst;
      ఒకవేళ (qsrc) dst = వరుస_కనుగొను_మొదటి_చివరి_గమ్యం_వరుస_src(qsrc, wid, cmp, నిజం, నిజం);
      లేకపోతే ఒకవేళ (dsrc) dst = వరుస_కనుగొను_మొదటి_చివరి_గమ్యం_వరుస_src(dsrc, wid, cmp, నిజం, నిజం);
      లేకపోతే {
	    vvp_వరుస* src_q = get_వరుస_object<vvp_వరుస_vec4>(thr, net);
	    vvp_వరుస_vec4* src = dynamic_cast<vvp_వరుస_vec4*>(src_q);
	    dst = వరుస_కనుగొను_మొదటి_చివరి_గమ్యం_వరుస_src(src, wid, cmp, నిజం, నిజం);
      }
      thr->నెట్టు_object(vvp_object_t(dst));
      తిరిగి నిజం;
}

తార్కిక యొక్క_వరుస_కనుగొను_మొదటి_సూచీ_PROP_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_vector4_t cmp = thr->తీయి_vec4();
      size_t pid = cp->సంఖ్య;
      గుర్తులేని wid = cp->బిట్టు_idx[0];
      ప్రతిజ్ఞా(cmp.size() == wid);

      vvp_object_t& శిఖరం = thr->peek_object();
      vvp_cobject*cobj = శిఖరం.peek<vvp_cobject>();
      ప్రతిజ్ఞా(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_వరుస_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_వరుస_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_వరుస_vec4* dst =
	    qsrc ? వరుస_కనుగొను_మొదటి_చివరి_గమ్యం_వరుస_src(qsrc, wid, cmp, నిజం, నిజం)
		 : వరుస_కనుగొను_మొదటి_చివరి_గమ్యం_వరుస_src(dsrc, wid, cmp, నిజం, నిజం);
      thr->నెట్టు_object(vvp_object_t(dst));
      తిరిగి నిజం;
}

తార్కిక యొక్క_వరుస_కనుగొను_చివరి_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_vector4_t cmp = thr->తీయి_vec4();
      గుర్తులేని wid = cp->బిట్టు_idx[0];
      ప్రతిజ్ఞా(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_వరుస_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_వరుస_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_వరుస_vec4* dst;
      ఒకవేళ (qsrc) dst = వరుస_కనుగొను_మొదటి_చివరి_గమ్యం_వరుస_src(qsrc, wid, cmp, అబద్ధం, అబద్ధం);
      లేకపోతే ఒకవేళ (dsrc) dst = వరుస_కనుగొను_మొదటి_చివరి_గమ్యం_వరుస_src(dsrc, wid, cmp, అబద్ధం, అబద్ధం);
      లేకపోతే {
	    vvp_వరుస* src_q = get_వరుస_object<vvp_వరుస_vec4>(thr, net);
	    vvp_వరుస_vec4* src = dynamic_cast<vvp_వరుస_vec4*>(src_q);
	    dst = వరుస_కనుగొను_మొదటి_చివరి_గమ్యం_వరుస_src(src, wid, cmp, అబద్ధం, అబద్ధం);
      }
      thr->నెట్టు_object(vvp_object_t(dst));
      తిరిగి నిజం;
}

తార్కిక యొక్క_వరుస_కనుగొను_చివరి_PROP_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_vector4_t cmp = thr->తీయి_vec4();
      size_t pid = cp->సంఖ్య;
      గుర్తులేని wid = cp->బిట్టు_idx[0];
      ప్రతిజ్ఞా(cmp.size() == wid);

      vvp_object_t& శిఖరం = thr->peek_object();
      vvp_cobject*cobj = శిఖరం.peek<vvp_cobject>();
      ప్రతిజ్ఞా(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_వరుస_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_వరుస_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_వరుస_vec4* dst =
	    qsrc ? వరుస_కనుగొను_మొదటి_చివరి_గమ్యం_వరుస_src(qsrc, wid, cmp, అబద్ధం, అబద్ధం)
		 : వరుస_కనుగొను_మొదటి_చివరి_గమ్యం_వరుస_src(dsrc, wid, cmp, అబద్ధం, అబద్ధం);
      thr->నెట్టు_object(vvp_object_t(dst));
      తిరిగి నిజం;
}

తార్కిక యొక్క_వరుస_కనుగొను_చివరి_సూచీ_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_vector4_t cmp = thr->తీయి_vec4();
      గుర్తులేని wid = cp->బిట్టు_idx[0];
      ప్రతిజ్ఞా(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_వరుస_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_వరుస_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_వరుస_vec4* dst;
      ఒకవేళ (qsrc) dst = వరుస_కనుగొను_మొదటి_చివరి_గమ్యం_వరుస_src(qsrc, wid, cmp, అబద్ధం, నిజం);
      లేకపోతే ఒకవేళ (dsrc) dst = వరుస_కనుగొను_మొదటి_చివరి_గమ్యం_వరుస_src(dsrc, wid, cmp, అబద్ధం, నిజం);
      లేకపోతే {
	    vvp_వరుస* src_q = get_వరుస_object<vvp_వరుస_vec4>(thr, net);
	    vvp_వరుస_vec4* src = dynamic_cast<vvp_వరుస_vec4*>(src_q);
	    dst = వరుస_కనుగొను_మొదటి_చివరి_గమ్యం_వరుస_src(src, wid, cmp, అబద్ధం, నిజం);
      }
      thr->నెట్టు_object(vvp_object_t(dst));
      తిరిగి నిజం;
}

తార్కిక యొక్క_వరుస_కనుగొను_చివరి_సూచీ_PROP_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_vector4_t cmp = thr->తీయి_vec4();
      size_t pid = cp->సంఖ్య;
      గుర్తులేని wid = cp->బిట్టు_idx[0];
      ప్రతిజ్ఞా(cmp.size() == wid);

      vvp_object_t& శిఖరం = thr->peek_object();
      vvp_cobject*cobj = శిఖరం.peek<vvp_cobject>();
      ప్రతిజ్ఞా(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_వరుస_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_వరుస_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_వరుస_vec4* dst =
	    qsrc ? వరుస_కనుగొను_మొదటి_చివరి_గమ్యం_వరుస_src(qsrc, wid, cmp, అబద్ధం, నిజం)
		 : వరుస_కనుగొను_మొదటి_చివరి_గమ్యం_వరుస_src(dsrc, wid, cmp, అబద్ధం, నిజం);
      thr->నెట్టు_object(vvp_object_t(dst));
      తిరిగి నిజం;
}

తార్కిక యొక్క_వరుస_MIN_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_net_t* net = cp->net;
      గుర్తులేని wid = cp->బిట్టు_idx[0];
      vvp_వరుస_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_వరుస_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_వరుస_vec4* dst;
      ఒకవేళ (qsrc) dst = వరుస_నిరంతరం_min_max_src(qsrc, wid, అబద్ధం);
      లేకపోతే ఒకవేళ (dsrc) dst = వరుస_నిరంతరం_min_max_src(dsrc, wid, అబద్ధం);
      లేకపోతే {
	    vvp_వరుస* src_q = get_వరుస_object<vvp_వరుస_vec4>(thr, net);
	    vvp_వరుస_vec4* src = dynamic_cast<vvp_వరుస_vec4*>(src_q);
	    dst = వరుస_నిరంతరం_min_max_src(src, wid, అబద్ధం);
      }
      thr->నెట్టు_object(vvp_object_t(dst));
      తిరిగి నిజం;
}

తార్కిక యొక్క_వరుస_MIN_OBJ_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని wid = cp->బిట్టు_idx[0];
      vvp_object_t src_obj;
      thr->తీయి_object(src_obj);

      vvp_వరుస_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_వరుస_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_వరుస_vec4* dst = qsrc ? వరుస_నిరంతరం_min_max_src(qsrc, wid, అబద్ధం)
				 : వరుస_నిరంతరం_min_max_src(dsrc, wid, అబద్ధం);
      thr->నెట్టు_object(vvp_object_t(dst));
      తిరిగి నిజం;
}

తార్కిక యొక్క_వరుస_MIN_PROP_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      size_t pid = cp->సంఖ్య;
      గుర్తులేని wid = cp->బిట్టు_idx[0];

      vvp_object_t& శిఖరం = thr->peek_object();
      vvp_cobject*cobj = శిఖరం.peek<vvp_cobject>();
      ప్రతిజ్ఞా(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_వరుస_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_వరుస_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_వరుస_vec4* dst = qsrc ? వరుస_నిరంతరం_min_max_src(qsrc, wid, అబద్ధం)
				 : వరుస_నిరంతరం_min_max_src(dsrc, wid, అబద్ధం);
      thr->నెట్టు_object(vvp_object_t(dst));
      తిరిగి నిజం;
}

తార్కిక యొక్క_వరుస_MAX_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_net_t* net = cp->net;
      గుర్తులేని wid = cp->బిట్టు_idx[0];
      vvp_వరుస_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_వరుస_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_వరుస_vec4* dst;
      ఒకవేళ (qsrc) dst = వరుస_నిరంతరం_min_max_src(qsrc, wid, నిజం);
      లేకపోతే ఒకవేళ (dsrc) dst = వరుస_నిరంతరం_min_max_src(dsrc, wid, నిజం);
      లేకపోతే {
	    vvp_వరుస* src_q = get_వరుస_object<vvp_వరుస_vec4>(thr, net);
	    vvp_వరుస_vec4* src = dynamic_cast<vvp_వరుస_vec4*>(src_q);
	    dst = వరుస_నిరంతరం_min_max_src(src, wid, నిజం);
      }
      thr->నెట్టు_object(vvp_object_t(dst));
      తిరిగి నిజం;
}

తార్కిక యొక్క_వరుస_MAX_OBJ_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని wid = cp->బిట్టు_idx[0];
      vvp_object_t src_obj;
      thr->తీయి_object(src_obj);

      vvp_వరుస_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_వరుస_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_వరుస_vec4* dst = qsrc ? వరుస_నిరంతరం_min_max_src(qsrc, wid, నిజం)
				 : వరుస_నిరంతరం_min_max_src(dsrc, wid, నిజం);
      thr->నెట్టు_object(vvp_object_t(dst));
      తిరిగి నిజం;
}

తార్కిక యొక్క_వరుస_MAX_PROP_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      size_t pid = cp->సంఖ్య;
      గుర్తులేని wid = cp->బిట్టు_idx[0];

      vvp_object_t& శిఖరం = thr->peek_object();
      vvp_cobject*cobj = శిఖరం.peek<vvp_cobject>();
      ప్రతిజ్ఞా(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_వరుస_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_వరుస_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_వరుస_vec4* dst = qsrc ? వరుస_నిరంతరం_min_max_src(qsrc, wid, నిజం)
				 : వరుస_నిరంతరం_min_max_src(dsrc, wid, నిజం);
      thr->నెట్టు_object(vvp_object_t(dst));
      తిరిగి నిజం;
}

తార్కిక యొక్క_వరుస_UNIQUE_OBJ_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని wid = cp->బిట్టు_idx[0];
      vvp_object_t src_obj;
      thr->తీయి_object(src_obj);

      vvp_వరుస_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_వరుస_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_వరుస_vec4* dst = qsrc ? వరుస_నిరంతరం_unique_src(qsrc, wid, అబద్ధం)
				 : వరుస_నిరంతరం_unique_src(dsrc, wid, అబద్ధం);
      thr->నెట్టు_object(vvp_object_t(dst));
      తిరిగి నిజం;
}

తార్కిక యొక్క_వరుస_UNIQUE_సూచీ_OBJ_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని wid = cp->బిట్టు_idx[0];
      vvp_object_t src_obj;
      thr->తీయి_object(src_obj);

      vvp_వరుస_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_వరుస_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_వరుస_vec4* dst = qsrc ? వరుస_నిరంతరం_unique_src(qsrc, wid, నిజం)
				 : వరుస_నిరంతరం_unique_src(dsrc, wid, నిజం);
      thr->నెట్టు_object(vvp_object_t(dst));
      తిరిగి నిజం;
}

తార్కిక యొక్క_వరుస_UNIQUE_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_net_t*net = cp->net;
      గుర్తులేని wid = cp->బిట్టు_idx[0];
      vvp_వరుస_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_వరుస_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_వరుస_vec4* dst;
      ఒకవేళ (qsrc) dst = వరుస_నిరంతరం_unique_src(qsrc, wid, అబద్ధం);
      లేకపోతే ఒకవేళ (dsrc) dst = వరుస_నిరంతరం_unique_src(dsrc, wid, అబద్ధం);
      లేకపోతే {
	    vvp_వరుస* src_q = get_వరుస_object<vvp_వరుస_vec4>(thr, net);
	    vvp_వరుస_vec4* src = dynamic_cast<vvp_వరుస_vec4*>(src_q);
	    dst = వరుస_నిరంతరం_unique_src(src, wid, అబద్ధం);
      }
      thr->నెట్టు_object(vvp_object_t(dst));
      తిరిగి నిజం;
}

తార్కిక యొక్క_వరుస_UNIQUE_PROP_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      size_t pid = cp->సంఖ్య;
      గుర్తులేని wid = cp->బిట్టు_idx[0];

      vvp_object_t& శిఖరం = thr->peek_object();
      vvp_cobject*cobj = శిఖరం.peek<vvp_cobject>();
      ప్రతిజ్ఞా(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_వరుస_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_వరుస_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_వరుస_vec4* dst = qsrc ? వరుస_నిరంతరం_unique_src(qsrc, wid, అబద్ధం)
				 : వరుస_నిరంతరం_unique_src(dsrc, wid, అబద్ధం);
      thr->నెట్టు_object(vvp_object_t(dst));
      తిరిగి నిజం;
}

తార్కిక యొక్క_వరుస_UNIQUE_సూచీ_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_net_t*net = cp->net;
      గుర్తులేని wid = cp->బిట్టు_idx[0];
      vvp_వరుస_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_వరుస_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_వరుస_vec4* dst;
      ఒకవేళ (qsrc) dst = వరుస_నిరంతరం_unique_src(qsrc, wid, నిజం);
      లేకపోతే ఒకవేళ (dsrc) dst = వరుస_నిరంతరం_unique_src(dsrc, wid, నిజం);
      లేకపోతే {
	    vvp_వరుస* src_q = get_వరుస_object<vvp_వరుస_vec4>(thr, net);
	    vvp_వరుస_vec4* src = dynamic_cast<vvp_వరుస_vec4*>(src_q);
	    dst = వరుస_నిరంతరం_unique_src(src, wid, నిజం);
      }
      thr->నెట్టు_object(vvp_object_t(dst));
      తిరిగి నిజం;
}

తార్కిక యొక్క_వరుస_UNIQUE_సూచీ_PROP_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      size_t pid = cp->సంఖ్య;
      గుర్తులేని wid = cp->బిట్టు_idx[0];

      vvp_object_t& శిఖరం = thr->peek_object();
      vvp_cobject*cobj = శిఖరం.peek<vvp_cobject>();
      ప్రతిజ్ఞా(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_వరుస_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_వరుస_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_వరుస_vec4* dst = qsrc ? వరుస_నిరంతరం_unique_src(qsrc, wid, నిజం)
				 : వరుస_నిరంతరం_unique_src(dsrc, wid, నిజం);
      thr->నెట్టు_object(vvp_object_t(dst));
      తిరిగి నిజం;
}

తార్కిక యొక్క_PROP_వరుస_కొలత(vthread_t thr, vvp_సంకేతం_t cp)
{
      size_t pid = cp->సంఖ్య;

      vvp_object_t& శిఖరం = thr->peek_object();
      vvp_cobject*cobj = శిఖరం.peek<vvp_cobject>();
      ప్రతిజ్ఞా(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_వరుస_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_వరుస_or_darray_vec4_from_object(qobj, qsrc, dsrc);

      size_t sz = qsrc ? qsrc->get_కొలత() : (dsrc ? dsrc->get_కొలత() : 0);

      vvp_vector4_t val(32, BIT4_0);
      గుర్తులేని దీర్ఘ ul = sz;
      కొరకు (గుర్తులేని idx = 0; idx < 32; idx++) {
	    ఒకవేళ (ul & 1UL) {
		  val.సమితి_బిట్టు(idx, BIT4_1);
	    } లేకపోతే {
		  val.సమితి_బిట్టు(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      thr->నెట్టు_vec4(val);
      తిరిగి నిజం;
}

తార్కిక యొక్క_వరుస_కొలత_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_వరుస_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_వరుస_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      size_t sz =
	    qsrc ? qsrc->get_కొలత() : (dsrc ? dsrc->get_కొలత() : 0);

      vvp_vector4_t val(32, BIT4_0);
      గుర్తులేని దీర్ఘ ul = sz;
      కొరకు (గుర్తులేని idx = 0; idx < 32; idx++) {
	    ఒకవేళ (ul & 1UL) {
		  val.సమితి_బిట్టు(idx, BIT4_1);
	    } లేకపోతే {
		  val.సమితి_బిట్టు(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      thr->నెట్టు_vec4(val);
      తిరిగి నిజం;
}

తార్కిక యొక్క_వరుస_మాట_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని wid = cp->బిట్టు_idx[0];
      గుర్తులేని ix_reg = cp->బిట్టు_idx[1];
      int64_t adr = thr->పదాలు[ix_reg].w_int;
      ఒకవేళ (thr->flags[4] == BIT4_1) {
	    thr->నెట్టు_vec4(vvp_vector4_t(wid));
	    తిరిగి నిజం;
      }
      ఒకవేళ (adr < 0) {
	    thr->నెట్టు_vec4(vvp_vector4_t(wid));
	    తిరిగి నిజం;
      }
      vvp_net_t* net = cp->net;
      vvp_వరుస_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_వరుస_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t out(wid);
      ఒకవేళ (qsrc) {
	    ఒకవేళ ((size_t)adr >= qsrc->get_కొలత()) {
		  thr->నెట్టు_vec4(out);
		  తిరిగి నిజం;
	    }
	    qsrc->get_మాట((గుర్తులేని)adr, out);
      } లేకపోతే ఒకవేళ (dsrc) {
	    ఒకవేళ ((size_t)adr >= dsrc->get_కొలత()) {
		  thr->నెట్టు_vec4(out);
		  తిరిగి నిజం;
	    }
	    dsrc->get_మాట((గుర్తులేని)adr, out);
      } లేకపోతే {
	    vvp_వరుస* src_q = get_వరుస_object<vvp_వరుస_vec4>(thr, net);
	    vvp_వరుస_vec4* src = dynamic_cast<vvp_వరుస_vec4*>(src_q);
	    ఒకవేళ (!src || (size_t)adr >= src->get_కొలత()) {
		  thr->నెట్టు_vec4(out);
		  తిరిగి నిజం;
	    }
	    src->get_మాట((గుర్తులేని)adr, out);
      }
      thr->నెట్టు_vec4(out);
      తిరిగి నిజం;
}

తార్కిక యొక్క_వరుస_మాట_PROP_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      size_t pid = cp->సంఖ్య;
      గుర్తులేని wid = cp->బిట్టు_idx[0];
      గుర్తులేని ix_reg = cp->బిట్టు_idx[1];
      int64_t adr = thr->పదాలు[ix_reg].w_int;
      ఒకవేళ (thr->flags[4] == BIT4_1) {
	    thr->నెట్టు_vec4(vvp_vector4_t(wid));
	    తిరిగి నిజం;
      }
      ఒకవేళ (adr < 0) {
	    thr->నెట్టు_vec4(vvp_vector4_t(wid));
	    తిరిగి నిజం;
      }

      vvp_object_t saved_శిఖరం;
      తార్కిక popped_శిఖరం = అబద్ధం;
      vvp_object_t& శిఖరం = thr->peek_object();
      vvp_cobject*cobj = శిఖరం.peek<vvp_cobject>();
      ఒకవేళ (cobj == 0) {
	    // Array-locator with(property) keeps an accumulator queue on top.
	    // Temporarily pop it to access the class object beneath.
	    thr->తీయి_object(saved_శిఖరం);
	    popped_శిఖరం = నిజం;
	    vvp_object_t& క్రింద = thr->peek_object();
	    cobj = క్రింద.peek<vvp_cobject>();
      }
      ఒకవేళ (cobj == 0) {
	    ఒకవేళ (popped_శిఖరం) thr->నెట్టు_object(saved_శిఖరం);
	    thr->నెట్టు_vec4(vvp_vector4_t(wid));
	    తిరిగి నిజం;
      }

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_వరుస_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_వరుస_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t out(wid);
      ఒకవేళ (qsrc) {
	    ఒకవేళ ((size_t)adr >= qsrc->get_కొలత()) {
		  thr->నెట్టు_vec4(out);
		  తిరిగి నిజం;
	    }
	    qsrc->get_మాట((గుర్తులేని)adr, out);
      } లేకపోతే ఒకవేళ (dsrc) {
	    ఒకవేళ ((size_t)adr >= dsrc->get_కొలత()) {
		  thr->నెట్టు_vec4(out);
		  తిరిగి నిజం;
	    }
	    dsrc->get_మాట((గుర్తులేని)adr, out);
      } లేకపోతే {
	    thr->నెట్టు_vec4(out);
	    ఒకవేళ (popped_శిఖరం) thr->నెట్టు_object(saved_శిఖరం);
	    తిరిగి నిజం;
      }
      ఒకవేళ (popped_శిఖరం) thr->నెట్టు_object(saved_శిఖరం);
      thr->నెట్టు_vec4(out);
      తిరిగి నిజం;
}

తార్కిక యొక్క_CMPIX_LTU(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని a = cp->సంఖ్య;
      గుర్తులేని b = cp->బిట్టు_idx[0];
      ఒకవేళ (thr->పదాలు[a].w_uint < thr->పదాలు[b].w_uint) {
	    thr->flags[4] = BIT4_1;
      } లేకపోతే {
	    thr->flags[4] = BIT4_0;
      }
      తిరిగి నిజం;
}

తార్కిక యొక్క_CMPIX_SLT0(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని r = cp->సంఖ్య;
      thr->flags[4] = (thr->పదాలు[r].w_int < 0) ? BIT4_1 : BIT4_0;
      తిరిగి నిజం;
}

తార్కిక యొక్క_నెట్టు_IX_VEC4(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని ix = cp->సంఖ్య;
      గుర్తులేని wid = cp->బిట్టు_idx[0];
      గుర్తులేని sgn = cp->బిట్టు_idx[1];
      uint32_t ubits;
      ఒకవేళ (sgn) {
	    int32_t sv = (int32_t)thr->పదాలు[ix].w_int;
	    ubits = (uint32_t)sv;
      } లేకపోతే {
	    ubits = (uint32_t)thr->పదాలు[ix].w_uint;
      }
      ఒకవేళ (wid > 32) wid = 32;
      vvp_vector4_t tmp(wid, BIT4_0);
      కొరకు (గుర్తులేని idx = 0; idx < wid; idx++) {
	    ఒకవేళ (ubits & 1u) {
		  tmp.సమితి_బిట్టు(idx, BIT4_1);
	    } లేకపోతే {
		  tmp.సమితి_బిట్టు(idx, BIT4_0);
	    }
	    ubits >>= 1;
      }
      thr->నెట్టు_vec4(tmp);
      తిరిగి నిజం;
}

తార్కిక యొక్క_వరుస_NEW_ఖాళీలు_V(vthread_t thr, vvp_సంకేతం_t)
{
      thr->నెట్టు_object(vvp_object_t(కొత్త vvp_వరుస_vec4()));
      తిరిగి నిజం;
}

తార్కిక యొక్క_వరుస_APPEND_మాట_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని wid = cp->బిట్టు_idx[0];
      vvp_vector4_t elem = thr->తీయి_vec4();
      ప్రతిజ్ఞా(elem.size() == wid);
      vvp_object_t qobj;
      thr->తీయి_object(qobj);
      vvp_వరుస_vec4* q = qobj.peek<vvp_వరుస_vec4>();
      ప్రతిజ్ఞా(q);
      q->push_back(elem, 0);
      thr->నెట్టు_object(qobj);
      తిరిగి నిజం;
}

తార్కిక యొక్క_DELETE_PROP_ELEM(vthread_t thr, vvp_సంకేతం_t cp)
{
      size_t pid = cp->సంఖ్య;

      int64_t idx_val = thr->పదాలు[3].w_int;
      ఒకవేళ (thr->flags[4] == BIT4_1) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with undefined index."
	         << పంక్తి_ముగింపు;
	    తిరిగి నిజం;
      }
      ఒకవేళ (idx_val < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with negative index."
	         << పంక్తి_ముగింపు;
	    తిరిగి నిజం;
      }
      size_t idx = idx_val;

      vvp_object_t& శిఖరం = thr->peek_object();
      vvp_cobject*cobj = శిఖరం.peek<vvp_cobject>();
      ప్రతిజ్ఞా(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_వరుస* వరుస = qobj.peek<vvp_వరుస>();
      ఒకవేళ (వరుస == 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping delete(" << idx
	         << ") on empty queue." << పంక్తి_ముగింపు;
      } లేకపోతే {
	    size_t size = వరుస->get_కొలత();
	    ఒకవేళ (idx >= size) {
		  cerr << thr->get_fileline()
		       << "Warning: skipping out of range delete(" << idx
		       << ") on queue of size " << size << "." << పంక్తి_ముగింపు;
	    } లేకపోతే {
		  వరుస->erase(idx);
	    }
      }

      తిరిగి నిజం;
}

తార్కిక యొక్క_DELETE_PROP_OBJ(vthread_t thr, vvp_సంకేతం_t cp)
{
      size_t pid = cp->సంఖ్య;

      vvp_object_t& శిఖరం = thr->peek_object();
      vvp_cobject*cobj = శిఖరం.peek<vvp_cobject>();
      ప్రతిజ్ఞా(cobj);

      cobj->సమితి_object(pid, vvp_object_t(), 0);

      తిరిగి నిజం;
}

మూస <రకం_పేరు ELEM, తరగతి QTYPE>
స్థిర తార్కిక store_qb(vthread_t thr, vvp_సంకేతం_t cp, గుర్తులేని wid=0)
{
      ELEM value;
      vvp_net_t*net = cp->net;
      గుర్తులేని max_కొలత = thr->పదాలు[cp->బిట్టు_idx[0]].w_int;
      తీయి_విలువ(thr, value, wid); // Pop the value to store.

      vvp_వరుస*వరుస = get_వరుస_object<QTYPE>(thr, net);
      ప్రతిజ్ఞా(వరుస);
      వరుస->push_back(value, max_కొలత);
      తిరిగి నిజం;
}

/*
 * %store/qb/r <var-label>, <max-idx>
 */
తార్కిక యొక్క_STORE_QB_R(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి store_qb<ద్విగుణ, vvp_వరుస_real>(thr, cp);
}

/*
 * %store/qb/str <var-label>, <max-idx>
 */
తార్కిక యొక్క_STORE_QB_STR(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి store_qb<string, vvp_వరుస_string>(thr, cp);
}

/*
 * %store/qb/v <var-label>, <max-idx>, <wid>
 */
తార్కిక యొక్క_STORE_QB_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి store_qb<vvp_vector4_t, vvp_వరుస_vec4>(thr, cp, cp->బిట్టు_idx[1]);
}

మూస <రకం_పేరు ELEM, తరగతి QTYPE>
స్థిర తార్కిక store_qdar(vthread_t thr, vvp_సంకేతం_t cp, గుర్తులేని wid=0)
{
      int64_t idx = thr->పదాలు[3].w_int;
      ELEM value;
      vvp_net_t*net = cp->net;
      తీయి_విలువ(thr, value, wid); // Pop the value to store.

      vvp_వరుస*వరుస = get_వరుస_object<QTYPE>(thr, net);
      ప్రతిజ్ఞా(వరుస);
      ఒకవేళ (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot assign to a negative "
	         << get_వరుస_type(value)
	         << " index (" << idx << "). ";
	    print_వరుస_విలువ(value);
	    cerr << " was not added." << పంక్తి_ముగింపు;
      } లేకపోతే ఒకవేళ (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot assign to an undefined "
	         << get_వరుస_type(value) << " index. ";
	    print_వరుస_విలువ(value);
	    cerr << " was not added." << పంక్తి_ముగింపు;
      } లేకపోతే {
	    గుర్తులేని max_కొలత = thr->పదాలు[cp->బిట్టు_idx[0]].w_int;
	    వరుస->సమితి_మాట_max(idx, value, max_కొలత);
      }
      తిరిగి నిజం;
}

/*
 * %store/qdar/r <var>, idx
 */
తార్కిక యొక్క_STORE_QDAR_R(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి store_qdar<ద్విగుణ, vvp_వరుస_real>(thr, cp);
}

/*
 * %store/qdar/str <var>, idx
 */
తార్కిక యొక్క_STORE_QDAR_STR(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి store_qdar<string, vvp_వరుస_string>(thr, cp);
}

/*
 * %store/qdar/v <var>, idx
 */
తార్కిక యొక్క_STORE_QDAR_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి store_qdar<vvp_vector4_t, vvp_వరుస_vec4>(thr, cp, cp->బిట్టు_idx[1]);
}

మూస <రకం_పేరు ELEM, తరగతి QTYPE>
స్థిర తార్కిక store_qf(vthread_t thr, vvp_సంకేతం_t cp, గుర్తులేని wid=0)
{
      ELEM value;
      vvp_net_t*net = cp->net;
      గుర్తులేని max_కొలత = thr->పదాలు[cp->బిట్టు_idx[0]].w_int;
      తీయి_విలువ(thr, value, wid); // Pop the value to store.

      vvp_వరుస*వరుస = get_వరుస_object<QTYPE>(thr, net);
      ప్రతిజ్ఞా(వరుస);
      వరుస->push_front(value, max_కొలత);
      తిరిగి నిజం;
}
/*
 * %store/qf/r <var-label>, <max-idx>
 */
తార్కిక యొక్క_STORE_QF_R(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి store_qf<ద్విగుణ, vvp_వరుస_real>(thr, cp);
}

/*
 * %store/qf/str <var-label>, <max-idx>
 */
తార్కిక యొక్క_STORE_QF_STR(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి store_qf<string, vvp_వరుస_string>(thr, cp);
}

/*
 * %store/qb/v <var-label>, <max-idx>, <wid>
 */
తార్కిక యొక్క_STORE_QF_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి store_qf<vvp_vector4_t, vvp_వరుస_vec4>(thr, cp, cp->బిట్టు_idx[1]);
}

మూస <రకం_పేరు ELEM, తరగతి QTYPE>
స్థిర తార్కిక store_qobj(vthread_t thr, vvp_సంకేతం_t cp, గుర్తులేని wid=0)
{
// FIXME: Can we actually use wid here?
      (శూన్య)wid;
      vvp_net_t*net = cp->net;

      vvp_వరుస*వరుస = get_వరుస_object<QTYPE>(thr, net);
      ప్రతిజ్ఞా(వరుస);

      vvp_object_t src;
      thr->తీయి_object(src);

        // If it is null just clear the queue
      ఒకవేళ (src.test_nil())
	    వరుస->erase_తోక(0);
      లేకపోతే {
	    గుర్తులేని max_కొలత = thr->పదాలు[cp->బిట్టు_idx[0]].w_int;
	    వరుస->copy_elems(src, max_కొలత);
      }

      తిరిగి నిజం;
}

తార్కిక యొక్క_STORE_QOBJ_R(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి store_qobj<ద్విగుణ, vvp_వరుస_real>(thr, cp);
}

తార్కిక యొక్క_STORE_QOBJ_STR(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి store_qobj<string, vvp_వరుస_string>(thr, cp);
}

తార్కిక యొక్క_STORE_QOBJ_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి store_qobj<vvp_vector4_t, vvp_వరుస_vec4>(thr, cp, cp->బిట్టు_idx[1]);
}

స్థిర శూన్య vvp_send(vthread_t thr, vvp_net_ptr_t ptr, స్థిరరాశి ద్విగుణ&val)
{
      vvp_send_real(ptr, val, thr->wt_context);
}

స్థిర శూన్య vvp_send(vthread_t thr, vvp_net_ptr_t ptr, స్థిరరాశి string&val)
{
      vvp_send_string(ptr, val, thr->wt_context);
}

మూస <రకం_పేరు ELEM>
స్థిర తార్కిక store(vthread_t thr, vvp_సంకేతం_t cp)
{
      ELEM val;
      తీయి_విలువ(thr, val, 0);
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_send(thr, ptr, val);
      తిరిగి నిజం;
}

తార్కిక యొక్క_STORE_REAL(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి store<ద్విగుణ>(thr, cp);
}

మూస <రకం_పేరు ELEM>
స్థిర తార్కిక storea(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని idx = cp->బిట్టు_idx[0];
      ELEM val;
      తీయి_విలువ(thr, val, 0);

      ఒకవేళ (thr->flags[4] != BIT4_1) {
	    గుర్తులేని adr = thr->పదాలు[idx].w_int;
	    cp->array->సమితి_మాట(adr, val);
      }

      తిరిగి నిజం;
}

/*
 * %store/reala <var-label> <index>
 */
తార్కిక యొక్క_STORE_REALA(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి storea<ద్విగుణ>(thr, cp);
}

తార్కిక యొక్క_STORE_STR(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి store<string>(thr, cp);
}

/*
 * %store/stra <array-label> <index>
 */
తార్కిక యొక్క_STORE_STRA(vthread_t thr, vvp_సంకేతం_t cp)
{
      తిరిగి storea<string>(thr, cp);
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
తార్కిక యొక్క_STORE_VEC4(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_net_ptr_t ptr(cp->net, 0);
      స్థిరరాశి vvp_signal_విలువ*sig = dynamic_cast<vvp_signal_విలువ*> (cp->net->fil);
      గుర్తులేని off_సూచీ = cp->బిట్టు_idx[0];
      గుర్తులేని పూర్ణాంకం wid = cp->బిట్టు_idx[1];

      int64_t off = off_సూచీ ? thr->పదాలు[off_సూచీ].w_int : 0;
      గుర్తులేని పూర్ణాంకం sig_విలువ_కొలత = sig->విలువ_కొలత();

      vvp_vector4_t&val = thr->peek_vec4();
      గుర్తులేని val_కొలత = val.size();

      ఒకవేళ (val_కొలత < wid) {
	    cerr << thr->get_fileline()
	         << "XXXX Internal error: val.size()=" << val_కొలత
		 << ", expecting >= " << wid << పంక్తి_ముగింపు;
      }
      ప్రతిజ్ఞా(val_కొలత >= wid);
      ఒకవేళ (val_కొలత > wid) {
	    val.resize(wid);
      }

	// If there is a problem loading the index register, flags-4
	// will be set to 1, and we know here to skip the actual assignment.
      ఒకవేళ (off_సూచీ!=0 && thr->flags[4] == BIT4_1) {
	    thr->తీయి_vec4(1);
	    తిరిగి నిజం;
      }

      ఒకవేళ (!resize_rval_vec(val, off, sig_విలువ_కొలత)) {
	    thr->తీయి_vec4(1);
	    తిరిగి నిజం;
      }

      ఒకవేళ (off == 0 && val.size() == sig_విలువ_కొలత)
	    vvp_send_vec4(ptr, val, thr->wt_context);
      లేకపోతే
	    vvp_send_vec4_pv(ptr, val, off, sig_విలువ_కొలత, thr->wt_context);

      thr->తీయి_vec4(1);
      తిరిగి నిజం;
}

/*
 * %store/vec4a <var-label>, <addr>, <offset>
 */
తార్కిక యొక్క_STORE_VEC4_A(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని adr_సూచీ = cp->బిట్టు_idx[0];
      గుర్తులేని off_సూచీ = cp->బిట్టు_idx[1];

      దీర్ఘ adr = adr_సూచీ? thr->పదాలు[adr_సూచీ].w_int : 0;
      int64_t off = off_సూచీ ? thr->పదాలు[off_సూచీ].w_int : 0;

	// Suppress action if flags-4 is true.
      ఒకవేళ (thr->flags[4] == BIT4_1) {
	    thr->తీయి_vec4(1);
	    తిరిగి నిజం;
      }

      vvp_vector4_t &value = thr->peek_vec4();

      ఒకవేళ (!resize_rval_vec(value, off, cp->array->get_మాట_కొలత())) {
	    thr->తీయి_vec4(1);
	    తిరిగి నిజం;
      }

      cp->array->సమితి_మాట(adr, off, value);

      thr->తీయి_vec4(1);
      తిరిగి నిజం;
}

/*
 * %sub
 *   pop r;
 *   pop l;
 *   push l-r;
 */
తార్కిక యొక్క_SUB(vthread_t thr, vvp_సంకేతం_t)
{
      vvp_vector4_t r = thr->తీయి_vec4();
      vvp_vector4_t&l = thr->peek_vec4();

      l.sub(r);
      తిరిగి నిజం;
}

/*
 * %subi <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
తార్కిక యొక్క_SUBI(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని wid = cp->సంఖ్య;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_తక్షణ_rval (cp, r);

      l.sub(r);

      తిరిగి నిజం;

}

తార్కిక యొక్క_SUB_WR(vthread_t thr, vvp_సంకేతం_t)
{
      ద్విగుణ r = thr->తీయి_real();
      ద్విగుణ l = thr->తీయి_real();
      thr->నెట్టు_real(l - r);
      తిరిగి నిజం;
}

/*
 * %substr <first>, <last>
 * Pop a string, take the substring (SystemVerilog style), and return
 * the result to the stack. This opcode actually works by editing the
 * string in place.
 */
తార్కిక యొక్క_SUBSTR(vthread_t thr, vvp_సంకేతం_t cp)
{
      int32_t మొదటి = thr->పదాలు[cp->బిట్టు_idx[0]].w_int;
      int32_t చివరి = thr->పదాలు[cp->బిట్టు_idx[1]].w_int;
      string&val = thr->peek_str(0);

      ఒకవేళ (మొదటి < 0 || చివరి < మొదటి || చివరి >= (int32_t)val.size()) {
	    val = string("");
	    తిరిగి నిజం;
      }

      val = val.substr(మొదటి, చివరి-మొదటి+1);
      తిరిగి నిజం;
}

/*
 * %substr/vec4 <index>, <wid>
 */
తార్కిక యొక్క_SUBSTR_VEC4(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని sel_idx = cp->బిట్టు_idx[0];
      గుర్తులేని wid = cp->బిట్టు_idx[1];

      int32_t sel = thr->పదాలు[sel_idx].w_int;
      స్థిరరాశి string&val = thr->peek_str(0);

      ప్రతిజ్ఞా(wid%8 == 0);

      ఒకవేళ (sel < 0 || sel >= (int32_t)val.size()) {
	    vvp_vector4_t res (wid, BIT4_0);
	    thr->నెట్టు_vec4(res);
	    తిరిగి నిజం;
      }

      vvp_vector4_t res (wid, BIT4_0);

      ప్రతిజ్ఞా(wid==8);
      గుర్తులేని అక్షరం tmp = val[sel];
      కొరకు (పూర్ణాంకం idx = 0 ; idx < 8 ; idx += 1) {
	    ఒకవేళ (tmp & (1<<idx))
		  res.సమితి_బిట్టు(idx, BIT4_1);
      }

      thr->నెట్టు_vec4(res);
      తిరిగి నిజం;
}

తార్కిక యొక్క_FILE_పంక్తి(vthread_t thr, vvp_సంకేతం_t cp)
{
      vpiHandle handle = cp->handle;

	/* When it is available, keep the file/line information in the
	   thread for error/warning messages. */
      thr->సమితి_fileline(vpi_get_str(vpiFile, handle),
                        vpi_get(vpi_పంక్తి_No, handle));

      ఒకవేళ (show_file_పంక్తి)
	    cerr << thr->get_fileline()
	         << vpi_get_str(_vpiDescription, handle) << పంక్తి_ముగింపు;

      తిరిగి నిజం;
}

/*
 * %test_nul <var-label>;
 * Test if the object at the specified variable is nil. If so, write
 * "1" into flags[4], otherwise write "0" into flags[4].
 */
తార్కిక యొక్క_TEST_NUL(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_net_t*net = cp->net;

      ప్రతిజ్ఞా(net);
      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      ప్రతిజ్ఞా(obj);

      ఒకవేళ (obj->get_object().test_nil())
	    thr->flags[4] = BIT4_1;
      లేకపోతే
	    thr->flags[4] = BIT4_0;

      తిరిగి నిజం;
}

తార్కిక యొక్క_TEST_NUL_A(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని idx = cp->బిట్టు_idx[0];
      గుర్తులేని adr = thr->పదాలు[idx].w_int;
      vvp_object_t మాట;

	/* If the address is undefined, return true. */
      ఒకవేళ (thr->flags[4] == BIT4_1) {
	    తిరిగి నిజం;
      }

      cp->array->get_మాట_obj(adr, మాట);
      ఒకవేళ (మాట.test_nil())
	    thr->flags[4] = BIT4_1;
      లేకపోతే
	    thr->flags[4] = BIT4_0;

      తిరిగి నిజం;
}

తార్కిక యొక్క_TEST_NUL_OBJ(vthread_t thr, vvp_సంకేతం_t)
{
      ఒకవేళ (thr->peek_object().test_nil())
	    thr->flags[4] = BIT4_1;
      లేకపోతే
	    thr->flags[4] = BIT4_0;
      తిరిగి నిజం;
}

/*
 * %test_nul/prop <pid>, <idx>
 */
తార్కిక యొక్క_TEST_NUL_PROP(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని pid = cp->సంఖ్య;
      గుర్తులేని idx = cp->బిట్టు_idx[0];

      ఒకవేళ (idx != 0) {
	    ప్రతిజ్ఞా(idx < vthread_s::పదాలు_లెక్క);
	    idx = thr->పదాలు[idx].w_uint;
      }

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj  = obj.peek<vvp_cobject>();

      vvp_object_t val;
      cobj->get_object(pid, val, idx);

      ఒకవేళ (val.test_nil())
	    thr->flags[4] = BIT4_1;
      లేకపోతే
	    thr->flags[4] = BIT4_0;

      తిరిగి నిజం;
}

/* and() reduction returns the scalar sum (Verilog-style bitwise-AND per word). */
మూస <తరగతి SRC>
స్థిర vvp_vector4_t uparr_and_పదాలు_src(SRC* src, గుర్తులేని wid)
{
      ఒకవేళ (!src) తిరిగి vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_1);
      కొరకు (size_t i = 0; i < src->get_కొలత(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_మాట(i, vi);
	    res &= vi;
      }
      తిరిగి res;
}

తార్కిక యొక్క_UPARR_AND_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_net_t* net = cp->net;
      గుర్తులేని wid = cp->బిట్టు_idx[0];
      vvp_వరుస_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_వరుస_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      ఒకవేళ (qsrc) res = uparr_and_పదాలు_src(qsrc, wid);
      లేకపోతే ఒకవేళ (dsrc) res = uparr_and_పదాలు_src(dsrc, wid);
      లేకపోతే {
	    vvp_వరుస* src_q = get_వరుస_object<vvp_వరుస_vec4>(thr, net);
	    vvp_వరుస_vec4* src = dynamic_cast<vvp_వరుస_vec4*>(src_q);
	    res = uparr_and_పదాలు_src(src, wid);
      }
      thr->నెట్టు_vec4(res);
      తిరిగి నిజం;
}

/* or() reduction returns the scalar sum (Verilog-style bitwise-AND per word). */
మూస <తరగతి SRC>
స్థిర vvp_vector4_t uparr_or_పదాలు_src(SRC* src, గుర్తులేని wid)
{
      ఒకవేళ (!src) తిరిగి vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_0);
      కొరకు (size_t i = 0; i < src->get_కొలత(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_మాట(i, vi);
	    res |= vi;
      }
      తిరిగి res;
}

తార్కిక యొక్క_UPARR_OR_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_net_t* net = cp->net;
      గుర్తులేని wid = cp->బిట్టు_idx[0];
      vvp_వరుస_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_వరుస_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      ఒకవేళ (qsrc) res = uparr_or_పదాలు_src(qsrc, wid);
      లేకపోతే ఒకవేళ (dsrc) res = uparr_or_పదాలు_src(dsrc, wid);
      లేకపోతే {
	    vvp_వరుస* src_q = get_వరుస_object<vvp_వరుస_vec4>(thr, net);
	    vvp_వరుస_vec4* src = dynamic_cast<vvp_వరుస_vec4*>(src_q);
	    res = uparr_or_పదాలు_src(src, wid);
      }
      thr->నెట్టు_vec4(res);
      తిరిగి నిజం;
}

/* real product() reduction returns the scalar sum */
మూస <తరగతి SRC>
స్థిర ద్విగుణ uparr_లబ్ధం_పదాలు_src_real(SRC* src)
{
      ఒకవేళ (!src) తిరిగి 0.0;
      ద్విగుణ acc = 1.0;
      కొరకు (size_t i = 0; i < src->get_కొలత(); i += 1) {
	    ద్విగుణ vi;
	    src->get_మాట(i, vi);
	    acc *= vi;
      }
      తిరిగి acc;
}

తార్కిక యొక్క_UPARR_లబ్ధం_REAL(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_వరుస_real* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_వరుస_or_darray_real_from_net(thr, net, qsrc, dsrc);
      ద్విగుణ కూడిక;
      ఒకవేళ (qsrc) కూడిక = uparr_లబ్ధం_పదాలు_src_real(qsrc);
      లేకపోతే ఒకవేళ (dsrc) కూడిక = uparr_లబ్ధం_పదాలు_src_real(dsrc);
      లేకపోతే {
	    vvp_వరుస* src_q = get_వరుస_object<vvp_వరుస_real>(thr, net);
	    vvp_వరుస_real* src = dynamic_cast<vvp_వరుస_real*>(src_q);
	    కూడిక = uparr_లబ్ధం_పదాలు_src_real(src);
      }
      thr->నెట్టు_real(కూడిక);
      తిరిగి నిజం;
}

/* product() reduction returns the scalar product (Verilog-style multiplication per word). */
మూస <తరగతి SRC>
స్థిర vvp_vector4_t uparr_లబ్ధం_పదాలు_src(SRC* src, గుర్తులేని wid)
{
      ఒకవేళ (!src) తిరిగి vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t acc = వరుస_unique_ulong_గమ్యం_vec4(1UL, wid);
      కొరకు (size_t i = 0; i < src->get_కొలత(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_మాట(i, vi);
	    acc.mul(vi);
      }
      తిరిగి acc;
}

తార్కిక యొక్క_UPARR_లబ్ధం_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_net_t* net = cp->net;
      గుర్తులేని wid = cp->బిట్టు_idx[0];
      vvp_వరుస_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_వరుస_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t prod;
      ఒకవేళ (qsrc) prod = uparr_లబ్ధం_పదాలు_src(qsrc, wid);
      లేకపోతే ఒకవేళ (dsrc) prod = uparr_లబ్ధం_పదాలు_src(dsrc, wid);
      లేకపోతే {
	    vvp_వరుస* src_q = get_వరుస_object<vvp_వరుస_vec4>(thr, net);
	    vvp_వరుస_vec4* src = dynamic_cast<vvp_వరుస_vec4*>(src_q);
	    prod = uparr_లబ్ధం_పదాలు_src(src, wid);
      }
      thr->నెట్టు_vec4(prod);
      తిరిగి నిజం;
}

తార్కిక యొక్క_UPARR_లబ్ధం_PROP_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      size_t pid = cp->సంఖ్య;
      గుర్తులేని wid = cp->బిట్టు_idx[0];

      vvp_object_t& శిఖరం = thr->peek_object();
      vvp_cobject*cobj = శిఖరం.peek<vvp_cobject>();
      ప్రతిజ్ఞా(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_వరుస_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_వరుస_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t prod =
	    qsrc ? uparr_లబ్ధం_పదాలు_src(qsrc, wid)
		 : uparr_లబ్ధం_పదాలు_src(dsrc, wid);
      thr->నెట్టు_vec4(prod);
      తిరిగి నిజం;
}

తార్కిక యొక్క_UPARR_లబ్ధం_OBJ_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని wid = cp->బిట్టు_idx[0];
      vvp_object_t src_obj;
      thr->తీయి_object(src_obj);

      vvp_వరుస_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_వరుస_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_vector4_t prod =
	    qsrc ? uparr_లబ్ధం_పదాలు_src(qsrc, wid)
		 : uparr_లబ్ధం_పదాలు_src(dsrc, wid);
      thr->నెట్టు_vec4(prod);
      తిరిగి నిజం;
}

/* real sum() reduction returns the scalar sum */
మూస <తరగతి SRC>
స్థిర ద్విగుణ uparr_కూడిక_పదాలు_src_real(SRC* src)
{
      ద్విగుణ acc = 0.0;
      ఒకవేళ (!src) తిరిగి acc;
      కొరకు (size_t i = 0; i < src->get_కొలత(); i += 1) {
	    ద్విగుణ vi;
	    src->get_మాట(i, vi);
	    acc += vi;
      }
      తిరిగి acc;
}

తార్కిక యొక్క_UPARR_కూడిక_REAL(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_వరుస_real* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_వరుస_or_darray_real_from_net(thr, net, qsrc, dsrc);
      ద్విగుణ కూడిక;
      ఒకవేళ (qsrc) కూడిక = uparr_కూడిక_పదాలు_src_real(qsrc);
      లేకపోతే ఒకవేళ (dsrc) కూడిక = uparr_కూడిక_పదాలు_src_real(dsrc);
      లేకపోతే {
	    vvp_వరుస* src_q = get_వరుస_object<vvp_వరుస_real>(thr, net);
	    vvp_వరుస_real* src = dynamic_cast<vvp_వరుస_real*>(src_q);
	    కూడిక = uparr_కూడిక_పదాలు_src_real(src);
      }
      thr->నెట్టు_real(కూడిక);
      తిరిగి నిజం;
}

/* sum() reduction returns the scalar sum (Verilog-style addition per word). */
మూస <తరగతి SRC>
స్థిర vvp_vector4_t uparr_కూడిక_పదాలు_src(SRC* src, గుర్తులేని wid)
{
      ఒకవేళ (!src) తిరిగి vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t acc(wid, BIT4_0);
      కొరకు (size_t i = 0; i < src->get_కొలత(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_మాట(i, vi);
	    acc.కూడు(vi);
      }
      తిరిగి acc;
}

తార్కిక యొక్క_UPARR_కూడిక_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_net_t* net = cp->net;
      గుర్తులేని wid = cp->బిట్టు_idx[0];
      vvp_వరుస_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_వరుస_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t కూడిక;
      ఒకవేళ (qsrc) కూడిక = uparr_కూడిక_పదాలు_src(qsrc, wid);
      లేకపోతే ఒకవేళ (dsrc) కూడిక = uparr_కూడిక_పదాలు_src(dsrc, wid);
      లేకపోతే {
	    vvp_వరుస* src_q = get_వరుస_object<vvp_వరుస_vec4>(thr, net);
	    vvp_వరుస_vec4* src = dynamic_cast<vvp_వరుస_vec4*>(src_q);
	    కూడిక = uparr_కూడిక_పదాలు_src(src, wid);
      }
      thr->నెట్టు_vec4(కూడిక);
      తిరిగి నిజం;
}

తార్కిక యొక్క_UPARR_కూడిక_PROP_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      size_t pid = cp->సంఖ్య;
      గుర్తులేని wid = cp->బిట్టు_idx[0];

      vvp_object_t& శిఖరం = thr->peek_object();
      vvp_cobject*cobj = శిఖరం.peek<vvp_cobject>();
      ప్రతిజ్ఞా(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_వరుస_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_వరుస_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t కూడిక =
	    qsrc ? uparr_కూడిక_పదాలు_src(qsrc, wid)
		 : uparr_కూడిక_పదాలు_src(dsrc, wid);
      thr->నెట్టు_vec4(కూడిక);
      తిరిగి నిజం;
}

తార్కిక యొక్క_UPARR_కూడిక_OBJ_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      గుర్తులేని wid = cp->బిట్టు_idx[0];
      vvp_object_t src_obj;
      thr->తీయి_object(src_obj);

      vvp_వరుస_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_వరుస_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_vector4_t కూడిక =
	    qsrc ? uparr_కూడిక_పదాలు_src(qsrc, wid)
		 : uparr_కూడిక_పదాలు_src(dsrc, wid);
      thr->నెట్టు_vec4(కూడిక);
      తిరిగి నిజం;
}

/* xor() reduction returns the scalar sum (Verilog-style bitwise-XOR per word). */
మూస <తరగతి SRC>
స్థిర vvp_vector4_t uparr_xor_పదాలు_src(SRC* src, గుర్తులేని wid)
{
      ఒకవేళ (!src) తిరిగి vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_0);
      కొరకు (size_t i = 0; i < src->get_కొలత(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_మాట(i, vi);
	    res ^= vi;
      }
      తిరిగి res;
}

తార్కిక యొక్క_UPARR_XOR_V(vthread_t thr, vvp_సంకేతం_t cp)
{
      vvp_net_t* net = cp->net;
      గుర్తులేని wid = cp->బిట్టు_idx[0];
      vvp_వరుస_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_వరుస_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      ఒకవేళ (qsrc) res = uparr_xor_పదాలు_src(qsrc, wid);
      లేకపోతే ఒకవేళ (dsrc) res = uparr_xor_పదాలు_src(dsrc, wid);
      లేకపోతే {
	    vvp_వరుస* src_q = get_వరుస_object<vvp_వరుస_vec4>(thr, net);
	    vvp_వరుస_vec4* src = dynamic_cast<vvp_వరుస_vec4*>(src_q);
	    res = uparr_xor_పదాలు_src(src, wid);
      }
      thr->నెట్టు_vec4(res);
      తిరిగి నిజం;
}

తార్కిక యొక్క_VPI_CALL(vthread_t thr, vvp_సంకేతం_t cp)
{
      vpip_execute_vpi_call(thr, cp->handle);

      ఒకవేళ (కాలపట్టిక_stopped()) {
	    ఒకవేళ (! కాలపట్టిక_ముగిసిన())
		  కాలపట్టిక_vthread(thr, 0, అబద్ధం);

	    తిరిగి అబద్ధం;
      }

      తిరిగి కాలపట్టిక_ముగిసిన()? అబద్ధం : నిజం;
}

/* %wait <label>;
 * Implement the wait by locating the vvp_net_T for the event, and
 * adding this thread to the threads list for the event. The some
 * argument is the  reference to the functor to wait for. This must be
 * an event object of some sort.
 */
తార్కిక యొక్క_WAIT(vthread_t thr, vvp_సంకేతం_t cp)
{
      ప్రతిజ్ఞా(! thr->i_am_లోపల_function);
      ప్రతిజ్ఞా(! thr->నిరీక్షణ_for_event);
      thr->నిరీక్షణ_for_event = 1;

	/* Add this thread to the list in the event. */
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (cp->net->fun);
      ప్రతిజ్ఞా(ep);
      thr->wait_తదుపరి = ep->కూడు_నిరీక్షణ_thread(thr);

	/* Return false to suspend this thread. */
      తిరిగి అబద్ధం;
}

/*
 * Implement the %wait/fork (SystemVerilog) instruction by suspending
 * the current thread until all the detached children have finished.
 */
తార్కిక యొక్క_WAIT_FORK(vthread_t thr, vvp_సంకేతం_t)
{
	/* If a %wait/fork is being executed then the parent thread
	 * cannot be waiting in a join or already waiting. */
      ప్రతిజ్ఞా(! thr->i_am_లోపల_function);
      ప్రతిజ్ఞా(! thr->i_am_joining);
      ప్రతిజ్ఞా(! thr->i_am_నిరీక్షణ);

	/* There should be no active children when waiting. */
      ప్రతిజ్ఞా(thr->children.empty());

	/* If there are no detached children then there is nothing to
	 * wait for. */
      ఒకవేళ (thr->detached_children.empty()) తిరిగి నిజం;

	/* Flag that this process is waiting for the detached children
	 * to finish and suspend it. */
      thr->i_am_నిరీక్షణ = 1;
      తిరిగి అబద్ధం;
}

/*
 * %xnor
 */
తార్కిక యొక్క_XNOR(vthread_t thr, vvp_సంకేతం_t)
{
      vvp_vector4_t valr = thr->తీయి_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      ప్రతిజ్ఞా(vall.size() == valr.size());

      vall ^= valr;
      vall.invert();

      తిరిగి నిజం;
}

/*
 * %xor
 */
తార్కిక యొక్క_XOR(vthread_t thr, vvp_సంకేతం_t)
{
      vvp_vector4_t valr = thr->తీయి_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      ప్రతిజ్ఞా(vall.size() == valr.size());

      vall ^= valr;

      తిరిగి నిజం;
}


తార్కిక యొక్క_ZOMBIE(vthread_t thr, vvp_సంకేతం_t)
{
      thr->pc = codespace_null();
      ఒకవేళ ((thr->మాతృ == 0) && (thr->children.empty())) {
	    ఒకవేళ (thr->delay_delete)
		  కాలపట్టిక_del_thr(thr);
	    లేకపోతే
		  vthread_delete(thr);
      }
      తిరిగి అబద్ధం;
}

/*
 * This is a phantom opcode used to call user defined functions. It
 * is used in code generated by the .ufunc statement. It contains a
 * pointer to the executable code of the function and a pointer to
 * a ufunc_core object that has all the port information about the
 * function.
 */
స్థిర తార్కిక do_exec_ufunc(vthread_t thr, vvp_సంకేతం_t cp, vthread_t బిడ్డ)
{
      __vpiScope*బిడ్డ_scope = cp->ufunc_core_ptr->func_scope();
      ప్రతిజ్ఞా(బిడ్డ_scope);

      ప్రతిజ్ఞా(బిడ్డ_scope->get_type_సంకేతం() == vpiFunction);
      ప్రతిజ్ఞా(thr->children.empty());


        /* We can take a number of shortcuts because we know that a
           continuous assignment can only occur in a static scope. */
      ప్రతిజ్ఞా(thr->wt_context == 0);
      ప్రతిజ్ఞా(thr->rd_context == 0);

        /* If an automatic function, allocate a context for this call. */
      vvp_context_t బిడ్డ_context = 0;
      ఒకవేళ (బిడ్డ_scope->is_automatic()) {
            బిడ్డ_context = vthread_alloc_context(బిడ్డ_scope);
            thr->wt_context = బిడ్డ_context;
            thr->rd_context = బిడ్డ_context;
      }

      బిడ్డ->wt_context = బిడ్డ_context;
      బిడ్డ->rd_context = బిడ్డ_context;

	/* Copy all the inputs to the ufunc object to the port
	   variables of the function. This copies all the values
	   atomically. */
      cp->ufunc_core_ptr->కేటాయించు_బిట్లు_గమ్యం_ports(బిడ్డ_context);
      బిడ్డ->delay_delete = 1;

      బిడ్డ->మాతృ = thr;
      thr->children.insert(బిడ్డ);
	// This should be the only child
      ప్రతిజ్ఞా(thr->children.size()==1);

      బిడ్డ->is_scheduled = 1;
      బిడ్డ->i_am_లోపల_function = 1;
      vthread_నిరంతరం(బిడ్డ);
      running_thread = thr;

      ఒకవేళ (బిడ్డ->i_have_ended) {
	    do_join(thr, బిడ్డ);
            తిరిగి నిజం;
      } లేకపోతే {
	    thr->i_am_joining = 1;
	    తిరిగి అబద్ధం;
      }
}

తార్కిక యొక్క_EXEC_UFUNC_REAL(vthread_t thr, vvp_సంకేతం_t cp)
{
      __vpiScope*బిడ్డ_scope = cp->ufunc_core_ptr->func_scope();
      ప్రతిజ్ఞా(బిడ్డ_scope);

	/* Create a temporary thread and run it immediately. */
      vthread_t బిడ్డ = vthread_new(cp->cptr, బిడ్డ_scope);
      thr->నెట్టు_real(0.0);
      బిడ్డ->వాదనలు_real.push_back(0);

      తిరిగి do_exec_ufunc(thr, cp, బిడ్డ);
}

తార్కిక యొక్క_EXEC_UFUNC_VEC4(vthread_t thr, vvp_సంకేతం_t cp)
{
      __vpiScope*బిడ్డ_scope = cp->ufunc_core_ptr->func_scope();
      ప్రతిజ్ఞా(బిడ్డ_scope);

      vpiScopeFunction*scope_func = dynamic_cast<vpiScopeFunction*>(బిడ్డ_scope);
      ప్రతిజ్ఞా(scope_func);

	/* Create a temporary thread and run it immediately. */
      vthread_t బిడ్డ = vthread_new(cp->cptr, బిడ్డ_scope);
      thr->నెట్టు_vec4(vvp_vector4_t(scope_func->get_func_వెడల్పు(), scope_func->get_func_init_val()));
      బిడ్డ->వాదనలు_vec4.push_back(0);

      తిరిగి do_exec_ufunc(thr, cp, బిడ్డ);
}

/*
 * This is a phantom opcode used to harvest the result of calling a user
 * defined function. It is used in code generated by the .ufunc statement.
 */
తార్కిక యొక్క_REAP_UFUNC(vthread_t thr, vvp_సంకేతం_t cp)
{
      __vpiScope*బిడ్డ_scope = cp->ufunc_core_ptr->func_scope();
      ప్రతిజ్ఞా(బిడ్డ_scope);

	/* Copy the output from the result variable to the output
	   ports of the .ufunc device. */
      cp->ufunc_core_ptr->ముగింపు_thread();

        /* If an automatic function, free the context for this call. */
      ఒకవేళ (బిడ్డ_scope->is_automatic()) {
            vthread_free_context(thr->rd_context, బిడ్డ_scope);
            thr->wt_context = 0;
            thr->rd_context = 0;
      }

      తిరిగి నిజం;
}
