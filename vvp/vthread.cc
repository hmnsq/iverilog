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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#ਜੇ_ਅੰਤ
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

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ std;

/* This is the size of an unsigned long in bits. This is just a
   convenience macro. */
# define CPU_ਸ਼ਬਦ_ਬਿੱਟ (8*ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ))
# define ਸਿਖਰ_ਦੋਆਧਾਰੀ_ਅੰਕ (1UL << (CPU_ਸ਼ਬਦ_ਬਿੱਟ-1))

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

ਬਣਤਰ vthread_s {
      vthread_s();

      ਖਾਲੀ debug_dump(ostream&fd, ਅਚਲ ਅੱਖਰ*label_ਪਾਠ);

	/* This is the program counter. */
      vvp_ਕੋਡ_t pc;
	/* These hold the private thread bits. */
      ਗਿਣਤੀ { FLAGS_ਗਿਣਤੀ = 512, ਸ਼ਬਦ_ਗਿਣਤੀ = 16 };
      vvp_bit4_t flags[FLAGS_ਗਿਣਤੀ];

	/* These are the word registers. */
      ਸੰਘ {
	    int64_t  w_int;
	    uint64_t w_uint;
      } ਸ਼ਬਦ[ਸ਼ਬਦ_ਗਿਣਤੀ];

	// These vectors are depths within the parent thread's
	// corresponding stack.  This is how the %ret/* instructions
	// get at parent thread arguments.
      vector<ਬਿਨਾਨਿਸ਼ਾਨੀ> ਦਲੀਲਾਂ_real;
      vector<ਬਿਨਾਨਿਸ਼ਾਨੀ> ਦਲੀਲਾਂ_str;
      vector<ਬਿਨਾਨਿਸ਼ਾਨੀ> ਦਲੀਲਾਂ_vec4;

    ਨਿਜੀ:
      vector<vvp_vector4_t>ਢੇਰ_vec4;
    ਜਨਤਕ:
      ਸਤਰਅੰਦਰ vvp_vector4_t ਕੱਢੋ_vec4(ਖਾਲੀ)
      {
	    ਪ੍ਰਤਿਜ੍ਞਾ(! ਢੇਰ_vec4.empty());
	    vvp_vector4_t val = ਢੇਰ_vec4.back();
	    ਢੇਰ_vec4.pop_back();
	    ਵਾਪਸ val;
      }
      ਸਤਰਅੰਦਰ ਖਾਲੀ ਧੱਕੋ_vec4(ਅਚਲ vvp_vector4_t&val)
      {
	    ਢੇਰ_vec4.push_back(val);
      }
      ਸਤਰਅੰਦਰ ਅਚਲ vvp_vector4_t& peek_vec4(ਬਿਨਾਨਿਸ਼ਾਨੀ ਡੂੰਘਾਈ)
      {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ size = ਢੇਰ_vec4.size();
	    ਪ੍ਰਤਿਜ੍ਞਾ(ਡੂੰਘਾਈ < size);
	    ਬਿਨਾਨਿਸ਼ਾਨੀ use_ਸੂਚਕਾਂਕ = size-1-ਡੂੰਘਾਈ;
	    ਵਾਪਸ ਢੇਰ_vec4[use_ਸੂਚਕਾਂਕ];
      }
      ਸਤਰਅੰਦਰ vvp_vector4_t& peek_vec4(ਖਾਲੀ)
      {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ use_ਸੂਚਕਾਂਕ = ਢੇਰ_vec4.size();
	    ਪ੍ਰਤਿਜ੍ਞਾ(use_ਸੂਚਕਾਂਕ >= 1);
	    ਵਾਪਸ ਢੇਰ_vec4[use_ਸੂਚਕਾਂਕ-1];
      }
      ਸਤਰਅੰਦਰ ਖਾਲੀ poke_vec4(ਬਿਨਾਨਿਸ਼ਾਨੀ ਡੂੰਘਾਈ, ਅਚਲ vvp_vector4_t&val)
      {
	    ਪ੍ਰਤਿਜ੍ਞਾ(ਡੂੰਘਾਈ < ਢੇਰ_vec4.size());
	    ਬਿਨਾਨਿਸ਼ਾਨੀ use_ਸੂਚਕਾਂਕ = ਢੇਰ_vec4.size()-1-ਡੂੰਘਾਈ;
	    ਢੇਰ_vec4[use_ਸੂਚਕਾਂਕ] = val;
      }
      ਸਤਰਅੰਦਰ ਖਾਲੀ ਕੱਢੋ_vec4(ਬਿਨਾਨਿਸ਼ਾਨੀ cnt)
      {
	    ਜਦੋਂ (cnt > 0) {
		  ਢੇਰ_vec4.pop_back();
		  cnt -= 1;
	    }
      }


    ਨਿਜੀ:
      vector<ਦੁੱਗਣਾ> ਢੇਰ_real;
    ਜਨਤਕ:
      ਸਤਰਅੰਦਰ ਦੁੱਗਣਾ ਕੱਢੋ_real(ਖਾਲੀ)
      {
	    ਪ੍ਰਤਿਜ੍ਞਾ(! ਢੇਰ_real.empty());
	    ਦੁੱਗਣਾ val = ਢੇਰ_real.back();
	    ਢੇਰ_real.pop_back();
	    ਵਾਪਸ val;
      }
      ਸਤਰਅੰਦਰ ਖਾਲੀ ਧੱਕੋ_real(ਦੁੱਗਣਾ val)
      {
	    ਢੇਰ_real.push_back(val);
      }
      ਸਤਰਅੰਦਰ ਦੁੱਗਣਾ peek_real(ਬਿਨਾਨਿਸ਼ਾਨੀ ਡੂੰਘਾਈ)
      {
	    ਪ੍ਰਤਿਜ੍ਞਾ(ਡੂੰਘਾਈ < ਢੇਰ_real.size());
	    ਬਿਨਾਨਿਸ਼ਾਨੀ use_ਸੂਚਕਾਂਕ = ਢੇਰ_real.size()-1-ਡੂੰਘਾਈ;
	    ਵਾਪਸ ਢੇਰ_real[use_ਸੂਚਕਾਂਕ];
      }
      ਸਤਰਅੰਦਰ ਖਾਲੀ poke_real(ਬਿਨਾਨਿਸ਼ਾਨੀ ਡੂੰਘਾਈ, ਦੁੱਗਣਾ val)
      {
	    ਪ੍ਰਤਿਜ੍ਞਾ(ਡੂੰਘਾਈ < ਢੇਰ_real.size());
	    ਬਿਨਾਨਿਸ਼ਾਨੀ use_ਸੂਚਕਾਂਕ = ਢੇਰ_real.size()-1-ਡੂੰਘਾਈ;
	    ਢੇਰ_real[use_ਸੂਚਕਾਂਕ] = val;
      }
      ਸਤਰਅੰਦਰ ਖਾਲੀ ਕੱਢੋ_real(ਬਿਨਾਨਿਸ਼ਾਨੀ cnt)
      {
	    ਜਦੋਂ (cnt > 0) {
		  ਢੇਰ_real.pop_back();
		  cnt -= 1;
	    }
      }

	/* Strings are operated on using a forth-like operator
	   set. Items at the top of the stack (back()) are the objects
	   operated on except for special cases. New objects are
	   pushed onto the top (back()) and pulled from the top
	   (back()) only. */
    ਨਿਜੀ:
      vector<string> ਢੇਰ_str;
    ਜਨਤਕ:
      ਸਤਰਅੰਦਰ string ਕੱਢੋ_str(ਖਾਲੀ)
      {
	    ਪ੍ਰਤਿਜ੍ਞਾ(! ਢੇਰ_str.empty());
	    string val = ਢੇਰ_str.back();
	    ਢੇਰ_str.pop_back();
	    ਵਾਪਸ val;
      }
      ਸਤਰਅੰਦਰ ਖਾਲੀ ਧੱਕੋ_str(ਅਚਲ string&val)
      {
	    ਢੇਰ_str.push_back(val);
      }
      ਸਤਰਅੰਦਰ string&peek_str(ਬਿਨਾਨਿਸ਼ਾਨੀ ਡੂੰਘਾਈ)
      {
	    ਪ੍ਰਤਿਜ੍ਞਾ(ਡੂੰਘਾਈ<ਢੇਰ_str.size());
	    ਬਿਨਾਨਿਸ਼ਾਨੀ use_ਸੂਚਕਾਂਕ = ਢੇਰ_str.size()-1-ਡੂੰਘਾਈ;
	    ਵਾਪਸ ਢੇਰ_str[use_ਸੂਚਕਾਂਕ];
      }
      ਸਤਰਅੰਦਰ ਖਾਲੀ poke_str(ਬਿਨਾਨਿਸ਼ਾਨੀ ਡੂੰਘਾਈ, ਅਚਲ string&val)
      {
	    ਪ੍ਰਤਿਜ੍ਞਾ(ਡੂੰਘਾਈ < ਢੇਰ_str.size());
	    ਬਿਨਾਨਿਸ਼ਾਨੀ use_ਸੂਚਕਾਂਕ = ਢੇਰ_str.size()-1-ਡੂੰਘਾਈ;
	    ਢੇਰ_str[use_ਸੂਚਕਾਂਕ] = val;
      }
      ਸਤਰਅੰਦਰ ਖਾਲੀ ਕੱਢੋ_str(ਬਿਨਾਨਿਸ਼ਾਨੀ cnt)
      {
	    ਜਦੋਂ (cnt > 0) {
		  ਢੇਰ_str.pop_back();
		  cnt -= 1;
	    }
      }

	/* Objects are also operated on in a stack. */
    ਨਿਜੀ:
      ਗਿਣਤੀ { ਢੇਰ_OBJ_MAX_ਮਾਪ = 32 };
      vvp_object_t ਢੇਰ_obj[ਢੇਰ_OBJ_MAX_ਮਾਪ];
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਢੇਰ_obj_ਮਾਪ;
    ਜਨਤਕ:
      ਸਤਰਅੰਦਰ vvp_object_t& peek_object(ਖਾਲੀ)
      {
	    ਪ੍ਰਤਿਜ੍ਞਾ(ਢੇਰ_obj_ਮਾਪ > 0);
	    ਵਾਪਸ ਢੇਰ_obj[ਢੇਰ_obj_ਮਾਪ-1];
      }
      ਸਤਰਅੰਦਰ ਖਾਲੀ ਕੱਢੋ_object(vvp_object_t&obj)
      {
	    ਪ੍ਰਤਿਜ੍ਞਾ(ਢੇਰ_obj_ਮਾਪ > 0);
	    ਢੇਰ_obj_ਮਾਪ -= 1;
	    obj = ਢੇਰ_obj[ਢੇਰ_obj_ਮਾਪ];
	    ਢੇਰ_obj[ਢੇਰ_obj_ਮਾਪ].reset(0);
      }
      ਸਤਰਅੰਦਰ ਖਾਲੀ ਕੱਢੋ_object(ਬਿਨਾਨਿਸ਼ਾਨੀ cnt, ਬਿਨਾਨਿਸ਼ਾਨੀ skip =0)
      {
	    ਪ੍ਰਤਿਜ੍ਞਾ((cnt+skip) <= ਢੇਰ_obj_ਮਾਪ);
	    ਲਈ (size_t idx = ਢੇਰ_obj_ਮਾਪ-skip-cnt ; idx < ਢੇਰ_obj_ਮਾਪ-skip ; idx += 1)
		  ਢੇਰ_obj[idx].reset(0);
	    ਢੇਰ_obj_ਮਾਪ -= cnt;
	    ਲਈ (size_t idx = ਢੇਰ_obj_ਮਾਪ-skip ; idx < ਢੇਰ_obj_ਮਾਪ ; idx += 1)
		  ਢੇਰ_obj[idx] = ਢੇਰ_obj[idx+skip];
	    ਲਈ (size_t idx = ਢੇਰ_obj_ਮਾਪ ; idx < ਢੇਰ_obj_ਮਾਪ+skip ; idx += 1)
		  ਢੇਰ_obj[idx].reset(0);
      }
      ਸਤਰਅੰਦਰ ਖਾਲੀ ਧੱਕੋ_object(ਅਚਲ vvp_object_t&obj)
      {
	    ਪ੍ਰਤਿਜ੍ਞਾ(ਢੇਰ_obj_ਮਾਪ < ਢੇਰ_OBJ_MAX_ਮਾਪ);
	    ਢੇਰ_obj[ਢੇਰ_obj_ਮਾਪ] = obj;
	    ਢੇਰ_obj_ਮਾਪ += 1;
      }

	/* My parent sets this when it wants me to wake it up. */
      ਬਿਨਾਨਿਸ਼ਾਨੀ i_am_joining      :1;
      ਬਿਨਾਨਿਸ਼ਾਨੀ i_am_detached     :1;
      ਬਿਨਾਨਿਸ਼ਾਨੀ i_am_ਉਡੀਕ      :1;
      ਬਿਨਾਨਿਸ਼ਾਨੀ i_am_ਵਿੱਚ_function  :1; // True if running function code
      ਬਿਨਾਨਿਸ਼ਾਨੀ i_have_ended      :1;
      ਬਿਨਾਨਿਸ਼ਾਨੀ i_was_disabled    :1;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਉਡੀਕ_for_event :1;
      ਬਿਨਾਨਿਸ਼ਾਨੀ is_scheduled      :1;
      ਬਿਨਾਨਿਸ਼ਾਨੀ delay_delete      :1;
	/* This points to the children of the thread. */
      ਸੈੱਟ<ਬਣਤਰ vthread_s*>children;
	/* This points to the detached children of the thread. */
      ਸੈੱਟ<ਬਣਤਰ vthread_s*>detached_children;
	/* This points to my parent, if I have one. */
      ਬਣਤਰ vthread_s*ਮਾਪਾ;
	/* This points to the containing scope. */
      __vpiScope*ਮਾਪਾ_scope;
	/* This is used for keeping wait queues. */
      ਬਣਤਰ vthread_s*wait_ਅਗਲਾ;
	/* These are used to access automatically allocated items. */
      vvp_context_t wt_context, rd_context;
	/* These are used to pass non-blocking event control information. */
      vvp_net_t*event;
      uint64_t ecount;
	/* Save the file/line information when available. */
    ਨਿਜੀ:
      ਅੱਖਰ *filenm_;
      ਬਿਨਾਨਿਸ਼ਾਨੀ lineno_;
    ਜਨਤਕ:
      ਖਾਲੀ ਸੈੱਟ_fileline(ਅੱਖਰ *filenm, ਬਿਨਾਨਿਸ਼ਾਨੀ lineno);
      string get_fileline();

      ਸਤਰਅੰਦਰ ਖਾਲੀ cleanup()
      {
	    ਜੇ (i_was_disabled) {
		  ਢੇਰ_vec4.clear();
		  ਢੇਰ_real.clear();
		  ਢੇਰ_str.clear();
		  ਕੱਢੋ_object(ਢੇਰ_obj_ਮਾਪ);
	    }
	    ਮੁਕਤ(filenm_);
	    filenm_ = 0;
	    ਪ੍ਰਤਿਜ੍ਞਾ(ਢੇਰ_vec4.empty());
	    ਪ੍ਰਤਿਜ੍ਞਾ(ਢੇਰ_real.empty());
	    ਪ੍ਰਤਿਜ੍ਞਾ(ਢੇਰ_str.empty());
	    ਪ੍ਰਤਿਜ੍ਞਾ(ਢੇਰ_obj_ਮਾਪ == 0);
      }
};

ਸਤਰਅੰਦਰ vthread_s::vthread_s()
{
      ਢੇਰ_obj_ਮਾਪ = 0;
      filenm_ = 0;
      lineno_ = 0;
}

ਖਾਲੀ vthread_s::ਸੈੱਟ_fileline(ਅੱਖਰ *filenm, ਬਿਨਾਨਿਸ਼ਾਨੀ lineno)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(filenm);
      ਜੇ (!filenm_ || (strcmp(filenm_, filenm) != 0)) {
	    ਮੁਕਤ(filenm_);
	    filenm_ = strdup(filenm);
      }
      lineno_ = lineno;
}

ਸਤਰਅੰਦਰ string vthread_s::get_fileline()
{
      ostringstream buf;
      ਜੇ (filenm_) {
	    buf << filenm_ << ":" << lineno_ << ": ";
      }
      string res = buf.str();
      ਵਾਪਸ res;
}

ਖਾਲੀ vthread_s::debug_dump(ostream&fd, ਅਚਲ ਅੱਖਰ*label)
{
      fd << "**** " << label << ਸਤਰ_ਅੰਤ;
      fd << "**** ThreadId: " << ਇਹ << ", parent id: " << ਮਾਪਾ << ਸਤਰ_ਅੰਤ;

      fd << "**** Flags: ";
      ਲਈ (ਪੂਰਨ idx = 0 ; idx < FLAGS_ਗਿਣਤੀ ; idx += 1)
	    fd << flags[idx];
      fd << ਸਤਰ_ਅੰਤ;
      fd << "**** vec4 stack..." << ਸਤਰ_ਅੰਤ;
      ਲਈ (size_t idx = ਢੇਰ_vec4.size() ; idx > 0 ; idx -= 1)
	    fd << "    " << (ਢੇਰ_vec4.size()-idx) << ": " << ਢੇਰ_vec4[idx-1] << ਸਤਰ_ਅੰਤ;
      fd << "**** str stack (" << ਢੇਰ_str.size() << ")..." << ਸਤਰ_ਅੰਤ;
      fd << "**** obj stack (" << ਢੇਰ_obj_ਮਾਪ << ")..." << ਸਤਰ_ਅੰਤ;
      fd << "**** args_vec4 array (" << ਦਲੀਲਾਂ_vec4.size() << ")..." << ਸਤਰ_ਅੰਤ;
      ਲਈ (size_t idx = 0 ; idx < ਦਲੀਲਾਂ_vec4.size() ; idx += 1)
	    fd << "    " << idx << ": " << ਦਲੀਲਾਂ_vec4[idx] << ਸਤਰ_ਅੰਤ;
      fd << "**** file/line (";
      ਜੇ (filenm_) fd << filenm_;
      ਨਹੀਂਤਾਂ fd << "<no file name>";
      fd << ":" << lineno_ << ")" << ਸਤਰ_ਅੰਤ;
      fd << "**** Done ****" << ਸਤਰ_ਅੰਤ;
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
ਸਥਿਰ string filter_string(ਅਚਲ ਅੱਖਰ*ਪਾਠ)
{
      vector<ਅੱਖਰ> tmp (ਲੜੀ_ਲੰਬਾਈ(ਪਾਠ)+1);
      size_t dst = 0;
      ਲਈ (ਅਚਲ ਅੱਖਰ*ptr = ਪਾਠ ; *ptr ; ptr += 1) {
	    // Not an escape? Move on.
	    ਜੇ (*ptr != '\\') {
		  tmp[dst++] = *ptr;
		  ਜਾਰੀ;
	    }

	    // Now we know that *ptr is pointing to a \ character and we
	    // have an octal sequence coming up. Advance the ptr and start
	    // processing octal digits.
	    ptr += 1;
	    ਜੇ (*ptr == 0)
		  ਰੋਕੋ;

	    ਅੱਖਰ byte = 0;
	    ਪੂਰਨ cnt = 3;
	    ਜਦੋਂ (*ptr && cnt > 0 && *ptr >= '0' && *ptr <= '7') {
		  byte *= 8;
		  byte += *ptr - '0';
		  cnt -= 1;
		  ptr += 1;
	    }

	    // null-bytes are supposed to be removed when assigning a string
	    // literal to a string.
	    ਜੇ (byte != '\0')
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
      ਵਾਪਸ res;
}

ਸਥਿਰ ਖਾਲੀ do_join(vthread_t thr, vthread_t ਬੱਚਾ);

__vpiScope* vthread_scope(ਬਣਤਰ vthread_s*thr)
{
      ਵਾਪਸ thr->ਮਾਪਾ_scope;
}

ਬਣਤਰ vthread_s*running_thread = 0;

string get_fileline()
{
      ਵਾਪਸ running_thread->get_fileline();
}

ਖਾਲੀ vthread_ਧੱਕੋ(ਬਣਤਰ vthread_s*thr, ਦੁੱਗਣਾ val)
{
      thr->ਧੱਕੋ_real(val);
}

ਖਾਲੀ vthread_ਧੱਕੋ(ਬਣਤਰ vthread_s*thr, ਅਚਲ string&val)
{
      thr->ਧੱਕੋ_str(val);
}

ਖਾਲੀ vthread_ਧੱਕੋ(ਬਣਤਰ vthread_s*thr, ਅਚਲ vvp_vector4_t&val)
{
      thr->ਧੱਕੋ_vec4(val);
}

ਖਾਲੀ vthread_ਕੱਢੋ_real(ਬਣਤਰ vthread_s*thr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਡੂੰਘਾਈ)
{
      thr->ਕੱਢੋ_real(ਡੂੰਘਾਈ);
}

ਖਾਲੀ vthread_ਕੱਢੋ_str(ਬਣਤਰ vthread_s*thr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਡੂੰਘਾਈ)
{
      thr->ਕੱਢੋ_str(ਡੂੰਘਾਈ);
}

ਖਾਲੀ vthread_ਕੱਢੋ_vec4(ਬਣਤਰ vthread_s*thr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਡੂੰਘਾਈ)
{
      thr->ਕੱਢੋ_vec4(ਡੂੰਘਾਈ);
}

ਦੁੱਗਣਾ vthread_get_real_ਢੇਰ(ਬਣਤਰ vthread_s*thr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਡੂੰਘਾਈ)
{
      ਵਾਪਸ thr->peek_real(ਡੂੰਘਾਈ);
}

ਅਚਲ string&vthread_get_str_ਢੇਰ(ਬਣਤਰ vthread_s*thr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਡੂੰਘਾਈ)
{
      ਵਾਪਸ thr->peek_str(ਡੂੰਘਾਈ);
}

ਅਚਲ vvp_vector4_t& vthread_get_vec4_ਢੇਰ(ਬਣਤਰ vthread_s*thr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਡੂੰਘਾਈ)
{
      ਵਾਪਸ thr->peek_vec4(ਡੂੰਘਾਈ);
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
ਸਾਂਚਾ <ਵਰਗ VVP_ਕਤਾਰ> ਸਥਿਰ vvp_ਕਤਾਰ*get_ਕਤਾਰ_object(vthread_t thr, vvp_net_t*net)
{
      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      ਪ੍ਰਤਿਜ੍ਞਾ(obj);

      vvp_ਕਤਾਰ*ਕਤਾਰ = obj->get_object().peek<vvp_ਕਤਾਰ>();
      ਜੇ (ਕਤਾਰ == 0) {
	    ਪ੍ਰਤਿਜ੍ਞਾ(obj->get_object().test_nil());
	    ਕਤਾਰ = ਨਵਾਂ VVP_ਕਤਾਰ;
	    vvp_object_t val (ਕਤਾਰ);
	    vvp_net_ptr_t ptr (net, 0);
	    vvp_send_object(ptr, val, thr->wt_context);
      }

      ਵਾਪਸ ਕਤਾਰ;
}

/*
 * Locator and size/word opcodes use the same VVP net for a queue ($) or a
 * dynamic array ([]). Queues are vvp_queue_vec4; dynamic arrays may be
 * vvp_darray_vec4, vvp_darray_atom<>, etc. (all vvp_darray, but vvp_queue
 * also derives from vvp_darray — test queue vec4 first).
 */
ਸਥਿਰ ਖਾਲੀ get_ਕਤਾਰ_or_darray_vec4_from_net(vthread_t thr, vvp_net_t* net,
					      vvp_ਕਤਾਰ_vec4*& qsrc,
					      vvp_darray*& dsrc)
{
      vvp_fun_signal_object* obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      ਪ੍ਰਤਿਜ੍ਞਾ(obj);
      vvp_object_t ਸਿਖਰ = obj->get_object();
      qsrc = ਸਿਖਰ.peek<vvp_ਕਤਾਰ_vec4>();
      ਜੇ (qsrc) {
	    dsrc = 0;
	    ਵਾਪਸ;
      }
      vvp_darray* dany = ਸਿਖਰ.peek<vvp_darray>();
      ਜੇ (dany && dynamic_cast<vvp_ਕਤਾਰ*>(dany) == 0) {
	    dsrc = dany;
	    ਵਾਪਸ;
      }
      ਜੇ (ਸਿਖਰ.test_nil()) {
	    qsrc = ਨਵਾਂ vvp_ਕਤਾਰ_vec4();
	    vvp_object_t val(qsrc);
	    vvp_net_ptr_t ptr(net, 0);
	    vvp_send_object(ptr, val, thr->wt_context);
	    dsrc = 0;
	    ਵਾਪਸ;
      }
      /* e.g. real/string queue — not vec4 queue or plain darray */
      qsrc = 0;
      dsrc = 0;
}

ਸਥਿਰ ਖਾਲੀ get_ਕਤਾਰ_or_darray_real_from_net(vthread_t thr, vvp_net_t* net,
					      vvp_ਕਤਾਰ_real*& qsrc,
					      vvp_darray*& dsrc)
{
      vvp_fun_signal_object* obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      ਪ੍ਰਤਿਜ੍ਞਾ(obj);
      vvp_object_t ਸਿਖਰ = obj->get_object();
      qsrc = ਸਿਖਰ.peek<vvp_ਕਤਾਰ_real>();
      ਜੇ (qsrc) {
	    dsrc = 0;
	    ਵਾਪਸ;
      }
      vvp_darray* dany = ਸਿਖਰ.peek<vvp_darray>();
      ਜੇ (dany && dynamic_cast<vvp_ਕਤਾਰ*>(dany) == 0) {
	    dsrc = dany;
	    ਵਾਪਸ;
      }
      ਜੇ (ਸਿਖਰ.test_nil()) {
	    qsrc = ਨਵਾਂ vvp_ਕਤਾਰ_real();
	    vvp_object_t val(qsrc);
	    vvp_net_ptr_t ptr(net, 0);
	    vvp_send_object(ptr, val, thr->wt_context);
	    dsrc = 0;
	    ਵਾਪਸ;
      }
      /* e.g. real/string queue — not vec4 queue or plain darray */
      qsrc = 0;
      dsrc = 0;
}

ਸਥਿਰ ਖਾਲੀ get_ਕਤਾਰ_or_darray_vec4_from_object(vvp_object_t src_obj,
						 vvp_ਕਤਾਰ_vec4*& qsrc,
						 vvp_darray*& dsrc)
{
      qsrc = src_obj.peek<vvp_ਕਤਾਰ_vec4>();
      ਜੇ (qsrc) {
	    dsrc = 0;
	    ਵਾਪਸ;
      }
      vvp_darray* dany = src_obj.peek<vvp_darray>();
      ਜੇ (dany && dynamic_cast<vvp_ਕਤਾਰ*>(dany) == 0) {
	    dsrc = dany;
	    ਵਾਪਸ;
      }
      qsrc = 0;
      dsrc = 0;
}

/*
 * The following are used to allow a common template to be written for
 * queue real/string/vec4 operations
 */
ਸਤਰਅੰਦਰ ਸਥਿਰ ਖਾਲੀ ਕੱਢੋ_ਮੁੱਲ(vthread_t thr, ਦੁੱਗਣਾ&value, ਬਿਨਾਨਿਸ਼ਾਨੀ)
{
      value = thr->ਕੱਢੋ_real();
}

ਸਤਰਅੰਦਰ ਸਥਿਰ ਖਾਲੀ ਕੱਢੋ_ਮੁੱਲ(vthread_t thr, string&value, ਬਿਨਾਨਿਸ਼ਾਨੀ)
{
      value = thr->ਕੱਢੋ_str();
}

ਸਤਰਅੰਦਰ ਸਥਿਰ ਖਾਲੀ ਕੱਢੋ_ਮੁੱਲ(vthread_t thr, vvp_vector4_t&value, ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
{
      value = thr->ਕੱਢੋ_vec4();
      ਪ੍ਰਤਿਜ੍ਞਾ(value.size() == wid);
}

/*
 * The following are used to allow the queue templates to print correctly.
 */
ਸਤਰਅੰਦਰ ਸਥਿਰ string get_ਕਤਾਰ_type(ਦੁੱਗਣਾ&)
{
      ਵਾਪਸ "queue<real>";
}

ਸਤਰਅੰਦਰ ਸਥਿਰ string get_ਕਤਾਰ_type(string&)
{
      ਵਾਪਸ "queue<string>";
}

ਸਤਰਅੰਦਰ ਸਥਿਰ string get_ਕਤਾਰ_type(ਅਚਲ vvp_vector4_t&value)
{
      ostringstream buf;
      buf << "queue<vector[" << value.size() << "]>";
      string res = buf.str();
      ਵਾਪਸ res;
}

ਸਤਰਅੰਦਰ ਸਥਿਰ ਖਾਲੀ print_ਕਤਾਰ_ਮੁੱਲ(ਦੁੱਗਣਾ value)
{
      cerr << value;
}

ਸਤਰਅੰਦਰ ਸਥਿਰ ਖਾਲੀ print_ਕਤਾਰ_ਮੁੱਲ(ਅਚਲ string&value)
{
      cerr << "\"" << value << "\"";
}

ਸਤਰਅੰਦਰ ਸਥਿਰ ਖਾਲੀ print_ਕਤਾਰ_ਮੁੱਲ(ਅਚਲ vvp_vector4_t&value)
{
      cerr << value;
}

/*
 * The following are used to get a darray/queue default value.
 */
ਸਤਰਅੰਦਰ ਸਥਿਰ ਖਾਲੀ dq_default(ਦੁੱਗਣਾ&value, ਬਿਨਾਨਿਸ਼ਾਨੀ)
{
      value = 0.0;
}

ਸਤਰਅੰਦਰ ਸਥਿਰ ਖਾਲੀ dq_default(string&value, ਬਿਨਾਨਿਸ਼ਾਨੀ)
{
      value = "";
}

ਸਤਰਅੰਦਰ ਸਥਿਰ ਖਾਲੀ dq_default(vvp_vector4_t&value, ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
{
      value = vvp_vector4_t(wid);
}


ਸਾਂਚਾ <ਵਰਗ T> T coerce_ਮੰਜ਼ਿਲ_ਚੌੜਾਈ(ਅਚਲ T&that, ਬਿਨਾਨਿਸ਼ਾਨੀ width)
{
      ਜੇ (that.size() == width)
	    ਵਾਪਸ that;

      ਪ੍ਰਤਿਜ੍ਞਾ(that.size() > width);
      T res (width);
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < width ;  idx += 1)
	    res.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, that.value(idx));

      ਵਾਪਸ res;
}

/* Explicitly define the vvp_vector4_t version of coerce_to_width(). */
ਸਾਂਚਾ vvp_vector4_t coerce_ਮੰਜ਼ਿਲ_ਚੌੜਾਈ(ਅਚਲ vvp_vector4_t&that,
                                       ਬਿਨਾਨਿਸ਼ਾਨੀ width);


ਸਥਿਰ ਖਾਲੀ multiply_array_imm(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ*res, ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ*val,
			       ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ imm)
{
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < ਸ਼ਬਦ ; idx += 1)
	    res[idx] = 0;

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ mul_idx = 0 ; mul_idx < ਸ਼ਬਦ ; mul_idx += 1) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਜੋੜ;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tmp = multiply_with_ਹਾਸਲ(val[mul_idx], imm, ਜੋੜ);

	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਹਾਸਲ = 0;
	    res[mul_idx] = ਜੋੜੋ_with_ਹਾਸਲ(res[mul_idx], tmp, ਹਾਸਲ);
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ ਜੋੜੋ_idx = mul_idx+1 ; ਜੋੜੋ_idx < ਸ਼ਬਦ ; ਜੋੜੋ_idx += 1) {
		  res[ਜੋੜੋ_idx] = ਜੋੜੋ_with_ਹਾਸਲ(res[ਜੋੜੋ_idx], ਜੋੜ, ਹਾਸਲ);
		  ਜੋੜ = 0;
	    }
      }
}

/*
 * Allocate a context for use by a child thread. By preference, use
 * the last freed context. If none available, create a new one. Add
 * it to the list of live contexts in that scope.
 */
ਸਥਿਰ vvp_context_t vthread_alloc_context(__vpiScope*scope)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(scope->is_automatic());

      vvp_context_t context = scope->free_contexts;
      ਜੇ (context) {
            scope->free_contexts = vvp_get_ਅਗਲਾ_context(context);
            ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < scope->nitem ; idx += 1) {
                  scope->ਵਸਤੂ[idx]->reset_instance(context);
            }
      } ਨਹੀਂਤਾਂ {
            context = vvp_allocate_context(scope->nitem);
            ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < scope->nitem ; idx += 1) {
                  scope->ਵਸਤੂ[idx]->alloc_instance(context);
            }
      }

      vvp_ਸੈੱਟ_ਅਗਲਾ_context(context, scope->ਜੀਵਤ_contexts);
      scope->ਜੀਵਤ_contexts = context;

      ਵਾਪਸ context;
}

/*
 * Free a context previously allocated to a child thread by pushing it
 * onto the freed context stack. Remove it from the list of live contexts
 * in that scope.
 */
ਸਥਿਰ ਖਾਲੀ vthread_free_context(vvp_context_t context, __vpiScope*scope)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(scope->is_automatic());
      ਪ੍ਰਤਿਜ੍ਞਾ(context);

      ਜੇ (context == scope->ਜੀਵਤ_contexts) {
            scope->ਜੀਵਤ_contexts = vvp_get_ਅਗਲਾ_context(context);
      } ਨਹੀਂਤਾਂ {
            vvp_context_t tmp = scope->ਜੀਵਤ_contexts;
            ਜਦੋਂ (context != vvp_get_ਅਗਲਾ_context(tmp)) {
                  ਪ੍ਰਤਿਜ੍ਞਾ(tmp);
                  tmp = vvp_get_ਅਗਲਾ_context(tmp);
            }
            vvp_ਸੈੱਟ_ਅਗਲਾ_context(tmp, vvp_get_ਅਗਲਾ_context(context));
      }

      vvp_ਸੈੱਟ_ਅਗਲਾ_context(context, scope->free_contexts);
      scope->free_contexts = context;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਖਾਲੀ contexts_delete(ਵਰਗ __vpiScope*scope)
{
      vvp_context_t context = scope->free_contexts;

      ਜਦੋਂ (context) {
	    scope->free_contexts = vvp_get_ਅਗਲਾ_context(context);
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0; idx < scope->nitem; idx += 1) {
		  scope->ਵਸਤੂ[idx]->free_instance(context);
	    }
	    ਮੁਕਤ(context);
	    context = scope->free_contexts;
      }
      ਮੁਕਤ(scope->ਵਸਤੂ);
}
#ਜੇ_ਅੰਤ

/*
 * Create a new thread with the given start address.
 */
vthread_t vthread_new(vvp_ਕੋਡ_t pc, __vpiScope*scope)
{
      vthread_t thr = ਨਵਾਂ ਬਣਤਰ vthread_s;
      thr->pc     = pc;
	//thr->bits4  = vvp_vector4_t(32);
      thr->ਮਾਪਾ = 0;
      thr->ਮਾਪਾ_scope = scope;
      thr->wait_ਅਗਲਾ = 0;
      thr->wt_context = 0;
      thr->rd_context = 0;

      thr->i_am_joining  = 0;
      thr->i_am_detached = 0;
      thr->i_am_ਉਡੀਕ  = 0;
      thr->i_am_ਵਿੱਚ_function = 0;
      thr->is_scheduled  = 0;
      thr->i_have_ended  = 0;
      thr->i_was_disabled = 0;
      thr->delay_delete  = 0;
      thr->ਉਡੀਕ_for_event = 0;
      thr->event  = 0;
      thr->ecount = 0;

      thr->flags[0] = BIT4_0;
      thr->flags[1] = BIT4_1;
      thr->flags[2] = BIT4_X;
      thr->flags[3] = BIT4_Z;
      ਲਈ (ਪੂਰਨ idx = 4 ; idx < 8 ; idx += 1)
	    thr->flags[idx] = BIT4_X;

      scope->ਧਾਗੇ .insert(thr);
      ਵਾਪਸ thr;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
#ਜੇ 0
/*
 * These are not currently correct. If you use them you will get
 * double delete messages. There is still a leak related to a
 * waiting event that needs to be investigated.
 */

ਸਥਿਰ ਖਾਲੀ wait_ਅਗਲਾ_delete(vthread_t ਅਧਾਰ)
{
      ਜਦੋਂ (ਅਧਾਰ) {
	    vthread_t tmp = ਅਧਾਰ->wait_ਅਗਲਾ;
	    ਮਿਟਾਓ ਅਧਾਰ;
	    ਅਧਾਰ = tmp;
	    ਜੇ (ਅਧਾਰ->ਉਡੀਕ_for_event == 0) ਰੋਕੋ;
      }
}

ਸਥਿਰ ਖਾਲੀ ਬੱਚਾ_delete(vthread_t ਅਧਾਰ)
{
      ਜਦੋਂ (ਅਧਾਰ) {
	    vthread_t tmp = ਅਧਾਰ->ਬੱਚਾ;
	    ਮਿਟਾਓ ਅਧਾਰ;
	    ਅਧਾਰ = tmp;
      }
}
#ਜੇ_ਅੰਤ

ਖਾਲੀ vthreads_delete(ਵਰਗ __vpiScope*scope)
{
      ਲਈ (std::ਸੈੱਟ<vthread_t>::iterator cur = scope->ਧਾਗੇ.begin()
		 ; cur != scope->ਧਾਗੇ.ਸਿਰਾ() ; ++ cur ) {
	    ਮਿਟਾਓ *cur;
      }
      scope->ਧਾਗੇ.clear();
}
#ਜੇ_ਅੰਤ

/*
 * Reaping pulls the thread out of the stack of threads. If I have a
 * child, then hand it over to my parent or fully detach it.
 */
ਸਥਿਰ ਖਾਲੀ vthread_reap(vthread_t thr)
{
      ਜੇ (! thr->children.empty()) {
	    ਲਈ (ਸੈੱਟ<vthread_t>::iterator cur = thr->children.begin()
		       ; cur != thr->children.ਸਿਰਾ() ; ++cur) {
		  vthread_t ਬੱਚਾ = *cur;
		  ਪ੍ਰਤਿਜ੍ਞਾ(ਬੱਚਾ);
		  ਪ੍ਰਤਿਜ੍ਞਾ(ਬੱਚਾ->ਮਾਪਾ == thr);
		  ਬੱਚਾ->ਮਾਪਾ = thr->ਮਾਪਾ;
	    }
      }
      ਜੇ (! thr->detached_children.empty()) {
	    ਲਈ (ਸੈੱਟ<vthread_t>::iterator cur = thr->detached_children.begin()
		       ; cur != thr->detached_children.ਸਿਰਾ() ; ++cur) {
		  vthread_t ਬੱਚਾ = *cur;
		  ਪ੍ਰਤਿਜ੍ਞਾ(ਬੱਚਾ);
		  ਪ੍ਰਤਿਜ੍ਞਾ(ਬੱਚਾ->ਮਾਪਾ == thr);
		  ਪ੍ਰਤਿਜ੍ਞਾ(ਬੱਚਾ->i_am_detached);
		  ਬੱਚਾ->ਮਾਪਾ = 0;
		  ਬੱਚਾ->i_am_detached = 0;
	    }
      }
      ਜੇ (thr->ਮਾਪਾ) {
	      /* assert that the given element was removed. */
	    ਜੇ (thr->i_am_detached) {
		  size_t res = thr->ਮਾਪਾ->detached_children.erase(thr);
		  ਪ੍ਰਤਿਜ੍ਞਾ(res == 1);
	    } ਨਹੀਂਤਾਂ {
		  size_t res = thr->ਮਾਪਾ->children.erase(thr);
		  ਪ੍ਰਤਿਜ੍ਞਾ(res == 1);
	    }
      }

      thr->ਮਾਪਾ = 0;

	// Remove myself from the containing scope if needed.
      thr->ਮਾਪਾ_scope->ਧਾਗੇ.erase(thr);

      thr->pc = codespace_null();

	/* If this thread is not scheduled, then is it safe to delete
	   it now. Otherwise, let the schedule event (which will
	   execute the thread at of_ZOMBIE) delete the object. */
      ਜੇ ((thr->is_scheduled == 0) && (thr->ਉਡੀਕ_for_event == 0)) {
	    ਪ੍ਰਤਿਜ੍ਞਾ(thr->children.empty());
	    ਪ੍ਰਤਿਜ੍ਞਾ(thr->wait_ਅਗਲਾ == 0);
	    ਜੇ (thr->delay_delete)
		  ਸਮਾਂ_ਸਾਰਣੀ_del_thr(thr);
	    ਨਹੀਂਤਾਂ
		  vthread_delete(thr);
      }
}

ਖਾਲੀ vthread_delete(vthread_t thr)
{
      thr->cleanup();
      ਮਿਟਾਓ thr;
}

ਖਾਲੀ vthread_mark_scheduled(vthread_t thr)
{
      ਜਦੋਂ (thr != 0) {
	    ਪ੍ਰਤਿਜ੍ਞਾ(thr->is_scheduled == 0);
	    thr->is_scheduled = 1;
	    thr = thr->wait_ਅਗਲਾ;
      }
}

ਖਾਲੀ vthread_mark_final(vthread_t thr)
{
      /*
       * The behavior in a final thread is the same as in a function. Any
       * child thread will be executed immediately rather than being
       * scheduled.
       */
      ਜਦੋਂ (thr != 0) {
	    thr->i_am_ਵਿੱਚ_function = 1;
	    thr = thr->wait_ਅਗਲਾ;
      }
}

ਖਾਲੀ vthread_delay_delete()
{
      ਜੇ (running_thread)
	    running_thread->delay_delete = 1;
}

/*
 * This function runs each thread by fetching an instruction,
 * incrementing the PC, and executing the instruction. The thread may
 * be the head of a list, so each thread is run so far as possible.
 */
ਖਾਲੀ vthread_ਲੜੀਵਾਰ(vthread_t thr)
{
      ਜਦੋਂ (thr != 0) {
	    vthread_t tmp = thr->wait_ਅਗਲਾ;
	    thr->wait_ਅਗਲਾ = 0;

	    ਪ੍ਰਤਿਜ੍ਞਾ(thr->is_scheduled);
	    thr->is_scheduled = 0;

            running_thread = thr;

	    ਲਈ (;;) {
		  vvp_ਕੋਡ_t cp = thr->pc;
		  thr->pc += 1;

		    /* Run the opcode implementation. If the execution of
		       the opcode returns false, then the thread is meant to
		       be paused, so break out of the loop. */
		  ਤਾਰਕਿਕ rc = (cp->opcode)(thr, cp);
		  ਜੇ (rc == ਝੂਠ)
			ਰੋਕੋ;
	    }

	    thr = tmp;
      }
      running_thread = 0;
}

/*
 * The CHUNK_LINK instruction is a special next pointer for linking
 * chunks of code space. It's like a simplified %jmp.
 */
ਤਾਰਕਿਕ ਦਾ_CHUNK_LINK(vthread_t thr, vvp_ਕੋਡ_t code)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(code->cptr);
      thr->pc = code->cptr;
      ਵਾਪਸ ਸੱਚ;
}

/*
 * This is called by an event functor to wake up all the threads on
 * its list. I in fact created that list in the %wait instruction, and
 * I also am certain that the waiting_for_event flag is set.
 */
ਖਾਲੀ vthread_ਸਮਾਂ_ਸਾਰਣੀ_list(vthread_t thr)
{
      ਲਈ (vthread_t cur = thr ;  cur ;  cur = cur->wait_ਅਗਲਾ) {
	    ਪ੍ਰਤਿਜ੍ਞਾ(cur->ਉਡੀਕ_for_event);
	    cur->ਉਡੀਕ_for_event = 0;
      }

      ਸਮਾਂ_ਸਾਰਣੀ_vthread(thr, 0);
}

vvp_context_t vthread_get_wt_context()
{
      ਜੇ (running_thread)
            ਵਾਪਸ running_thread->wt_context;
      ਨਹੀਂਤਾਂ
            ਵਾਪਸ 0;
}

vvp_context_t vthread_get_rd_context()
{
      ਜੇ (running_thread)
            ਵਾਪਸ running_thread->rd_context;
      ਨਹੀਂਤਾਂ
            ਵਾਪਸ 0;
}

vvp_context_ਵਸਤੂ_t vthread_get_wt_context_ਵਸਤੂ(ਬਿਨਾਨਿਸ਼ਾਨੀ context_idx)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(running_thread && running_thread->wt_context);
      ਵਾਪਸ vvp_get_context_ਵਸਤੂ(running_thread->wt_context, context_idx);
}

vvp_context_ਵਸਤੂ_t vthread_get_rd_context_ਵਸਤੂ(ਬਿਨਾਨਿਸ਼ਾਨੀ context_idx)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(running_thread && running_thread->rd_context);
      ਵਾਪਸ vvp_get_context_ਵਸਤੂ(running_thread->rd_context, context_idx);
}

/*
 * %abs/wr
 */
ਤਾਰਕਿਕ ਦਾ_ABS_WR(vthread_t thr, vvp_ਕੋਡ_t)
{
      thr->ਧੱਕੋ_real( fabs(thr->ਕੱਢੋ_real()) );
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_ALLOC(vthread_t thr, vvp_ਕੋਡ_t cp)
{
        /* Allocate a context. */
      vvp_context_t ਬੱਚਾ_context = vthread_alloc_context(cp->scope);

        /* Push the allocated context onto the write context stack. */
      vvp_ਸੈੱਟ_stacked_context(ਬੱਚਾ_context, thr->wt_context);
      thr->wt_context = ਬੱਚਾ_context;

      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_AND(vthread_t thr, vvp_ਕੋਡ_t)
{
      vvp_vector4_t valb = thr->ਕੱਢੋ_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();
      ਪ੍ਰਤਿਜ੍ਞਾ(vala.size() == valb.size());
      vala &= valb;
      ਵਾਪਸ ਸੱਚ;
}

/*
 * This function must ALWAYS be called with the val set to the right
 * size, and initialized with BIT4_0 bits. Certain optimizations rely
 * on that.
 */
ਸਥਿਰ ਖਾਲੀ get_ਤੁਰੰਤ_rval(vvp_ਕੋਡ_t cp, vvp_vector4_t&val)
{
      uint32_t vala = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      uint32_t valb = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[1];
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid  = cp->ਸੰਖਿਆ;

      ਜੇ (valb == 0) {
	      // Special case: if the value is zero, we are done
	      // before we start.
	    ਜੇ (vala == 0) ਵਾਪਸ;

	      // Special case: The value has no X/Z bits, so we can
	      // use the setarray method to write the value all at once.
	    ਬਿਨਾਨਿਸ਼ਾਨੀ use_wid = 8*ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ);
	    ਜੇ (wid < use_wid)
		  use_wid = wid;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tmp[1];
	    tmp[0] = vala;
	    val.setarray(0, use_wid, tmp);
	    ਵਾਪਸ;
      }

	// The immediate value can be values bigger then 32 bits, but
	// only if the high bits are zero. So at most we need to run
	// through the loop below 32 times. Maybe less, if the target
	// width is less. We don't have to do anything special on that
	// because vala/valb bits will shift away so (vala|valb) will
	// turn to zero at or before 32 shifts.

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < wid && (vala|valb) ; idx += 1) {
	    uint32_t ba = 0;
	      // Convert the vala/valb bits to a ba number that
	      // matches the encoding of the vvp_bit4_t enumeration.
	    ba = (valb & 1) << 1;
	    ba |= vala & 1;

	      // Note that the val is already pre-filled with BIT4_0
	      // bits, os we only need to set non-zero bit values.
	    ਜੇ (ba) val.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, (vvp_bit4_t)ba);

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
ਤਾਰਕਿਕ ਦਾ_ਜੋੜੋ(vthread_t thr, vvp_ਕੋਡ_t)
{
      vvp_vector4_t r = thr->ਕੱਢੋ_vec4();
	// Rather then pop l, use it directly from the stack. When we
	// assign to 'l', that will edit the top of the stack, which
	// replaces a pop and a pull.
      vvp_vector4_t&l = thr->peek_vec4();

      l.ਜੋੜੋ(r);

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %addi <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
ਤਾਰਕਿਕ ਦਾ_ADDI(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਸੰਖਿਆ;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_ਤੁਰੰਤ_rval (cp, r);

      l.ਜੋੜੋ(r);

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %add/wr
 */
ਤਾਰਕਿਕ ਦਾ_ਜੋੜੋ_WR(vthread_t thr, vvp_ਕੋਡ_t)
{
      ਦੁੱਗਣਾ r = thr->ਕੱਢੋ_real();
      ਦੁੱਗਣਾ l = thr->ਕੱਢੋ_real();
      thr->ਧੱਕੋ_real(l + r);
      ਵਾਪਸ ਸੱਚ;
}

/* %assign/ar <array>, <delay>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory. <delay>
 * is the delay in simulation time. <bit> is the index register
 * containing the real value.
 */
ਤਾਰਕਿਕ ਦਾ_ਸੌਂਪੋ_AR(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਲੰਮਾ adr = thr->ਸ਼ਬਦ[3].w_int;
      ਬਿਨਾਨਿਸ਼ਾਨੀ delay = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      ਦੁੱਗਣਾ value = thr->ਕੱਢੋ_real();

      ਜੇ (adr >= 0) {
	    ਸਮਾਂ_ਸਾਰਣੀ_ਸੌਂਪੋ_array_ਸ਼ਬਦ(cp->array, adr, value, delay);
      }

      ਵਾਪਸ ਸੱਚ;
}

/* %assign/ar/d <array>, <delay_idx>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory.
 * <delay_idx> is the integer register that contains the delay value.
 */
ਤਾਰਕਿਕ ਦਾ_ਸੌਂਪੋ_ARD(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਲੰਮਾ adr = thr->ਸ਼ਬਦ[3].w_int;
      ਦੁੱਗਣਾ value = thr->ਕੱਢੋ_real();

      ਜੇ (adr >= 0) {
	    vvp_time64_t delay = thr->ਸ਼ਬਦ[cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0]].w_uint;
	    ਸਮਾਂ_ਸਾਰਣੀ_ਸੌਂਪੋ_array_ਸ਼ਬਦ(cp->array, adr, value, delay);
      }

      ਵਾਪਸ ਸੱਚ;
}

/* %assign/ar/e <array>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory. <bit>
 * is the index register containing the real value. The event
 * information is contained in the thread event control registers
 * and is set with %evctl.
 */
ਤਾਰਕਿਕ ਦਾ_ਸੌਂਪੋ_ARE(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਲੰਮਾ adr = thr->ਸ਼ਬਦ[3].w_int;
      ਦੁੱਗਣਾ value = thr->ਕੱਢੋ_real();

      ਜੇ (adr >= 0) {
	    ਜੇ (thr->ecount == 0) {
		  ਸਮਾਂ_ਸਾਰਣੀ_ਸੌਂਪੋ_array_ਸ਼ਬਦ(cp->array, adr, value, 0);
	    } ਨਹੀਂਤਾਂ {
		  ਸਮਾਂ_ਸਾਰਣੀ_evctl(cp->array, adr, value, thr->event,
		                 thr->ecount);
	    }
      }

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %assign/vec4 <var>, <delay>
 */
ਤਾਰਕਿਕ ਦਾ_ਸੌਂਪੋ_VEC4(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      ਬਿਨਾਨਿਸ਼ਾਨੀ delay = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      ਅਚਲ vvp_vector4_t&val = thr->peek_vec4();

      ਸਮਾਂ_ਸਾਰਣੀ_ਸੌਂਪੋ_ਸਦਿਸ਼(ptr, 0, 0, val, delay);
      thr->ਕੱਢੋ_vec4(1);
      ਵਾਪਸ ਸੱਚ;
}

/*
 * Resizes a vector value for a partial assignment so that the value is fully
 * in-bounds of the target signal. Both `val` and `off` will be updated if
 * necessary.
 *
 * Returns false if the value is fully out-of-bounds and the assignment should
 * be skipped. Otherwise returns true.
 */
ਸਥਿਰ ਤਾਰਕਿਕ resize_rval_vec(vvp_vector4_t &val, int64_t &off,
			    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ sig_wid)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ wid = val.size();

        // Fully in bounds, most likely case
      ਜੇ (off >= 0 && (uint64_t)off + wid <= sig_wid)
	    ਵਾਪਸ ਸੱਚ;

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਅਧਾਰ = 0;
      ਜੇ (off >= 0) {
	      // Fully out-of-bounds
	    ਜੇ ((uint64_t)off >= sig_wid)
		  ਵਾਪਸ ਝੂਠ;
      } ਨਹੀਂਤਾਂ {
	      // Fully out-of-bounds */
	    ਜੇ ((uint64_t)(-off) >= wid)
		  ਵਾਪਸ ਝੂਠ;

	      // If the index is below the vector, then only assign the high
	      // bits that overlap with the target
	    ਅਧਾਰ = -off;
	    wid += off;
		off = 0;
      }

	// If the value is partly above the target, then only assign
	// the bits that overlap
      ਜੇ ((uint64_t)off + wid > sig_wid)
	    wid = sig_wid - (uint64_t)off;

      val = val.subvalue(ਅਧਾਰ, wid);

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %assign/vec4/a/d <arr>, <offx>, <delx>
 */
ਤਾਰਕਿਕ ਦਾ_ਸੌਂਪੋ_VEC4_A_D(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਪੂਰਨ off_idx = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      ਪੂਰਨ del_idx = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[1];
      ਪੂਰਨ adr_idx = 3;

      int64_t  off = off_idx ? thr->ਸ਼ਬਦ[off_idx].w_int : 0;
      vvp_time64_t del = del_idx? thr->ਸ਼ਬਦ[del_idx].w_uint : 0;
      ਲੰਮਾ     adr = thr->ਸ਼ਬਦ[adr_idx].w_int;

      vvp_vector4_t val = thr->ਕੱਢੋ_vec4();

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      ਜੇ (thr->flags[4] == BIT4_1)
	    ਵਾਪਸ ਸੱਚ;

      ਜੇ (!resize_rval_vec(val, off, cp->array->get_ਸ਼ਬਦ_ਮਾਪ()))
	    ਵਾਪਸ ਸੱਚ;

      ਸਮਾਂ_ਸਾਰਣੀ_ਸੌਂਪੋ_array_ਸ਼ਬਦ(cp->array, adr, off, val, del);

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %assign/vec4/a/e <arr>, <offx>
 */
ਤਾਰਕਿਕ ਦਾ_ਸੌਂਪੋ_VEC4_A_E(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਪੂਰਨ off_idx = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      ਪੂਰਨ adr_idx = 3;

      int64_t  off = off_idx ? thr->ਸ਼ਬਦ[off_idx].w_int : 0;
      ਲੰਮਾ     adr = thr->ਸ਼ਬਦ[adr_idx].w_int;

      vvp_vector4_t val = thr->ਕੱਢੋ_vec4();

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      ਜੇ (thr->flags[4] == BIT4_1)
	    ਵਾਪਸ ਸੱਚ;

      ਜੇ (!resize_rval_vec(val, off, cp->array->get_ਸ਼ਬਦ_ਮਾਪ()))
	    ਵਾਪਸ ਸੱਚ;

      ਜੇ (thr->ecount == 0) {
	    ਸਮਾਂ_ਸਾਰਣੀ_ਸੌਂਪੋ_array_ਸ਼ਬਦ(cp->array, adr, off, val, 0);
      } ਨਹੀਂਤਾਂ {
	    ਸਮਾਂ_ਸਾਰਣੀ_evctl(cp->array, adr, val, off, thr->event, thr->ecount);
      }

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %assign/vec4/off/d <var>, <off>, <del>
 */
ਤਾਰਕਿਕ ਦਾ_ਸੌਂਪੋ_VEC4_OFF_D(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      ਬਿਨਾਨਿਸ਼ਾਨੀ off_ਸੂਚਕਾਂਕ = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      ਬਿਨਾਨਿਸ਼ਾਨੀ del_ਸੂਚਕਾਂਕ = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[1];
      vvp_vector4_t val = thr->ਕੱਢੋ_vec4();

      int64_t off = thr->ਸ਼ਬਦ[off_ਸੂਚਕਾਂਕ].w_int;
      vvp_time64_t del = thr->ਸ਼ਬਦ[del_ਸੂਚਕਾਂਕ].w_uint;

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      ਜੇ (thr->flags[4] == BIT4_1)
	    ਵਾਪਸ ਸੱਚ;

      vvp_signal_ਮੁੱਲ*sig = dynamic_cast<vvp_signal_ਮੁੱਲ*> (cp->net->fil);
      ਪ੍ਰਤਿਜ੍ਞਾ(sig);

      ਜੇ (!resize_rval_vec(val, off, sig->ਮੁੱਲ_ਮਾਪ()))
	    ਵਾਪਸ ਸੱਚ;

      ਸਮਾਂ_ਸਾਰਣੀ_ਸੌਂਪੋ_ਸਦਿਸ਼(ptr, off, sig->ਮੁੱਲ_ਮਾਪ(), val, del);
      ਵਾਪਸ ਸੱਚ;
}

/*
 * %assign/vec4/off/e <var>, <off>
 */
ਤਾਰਕਿਕ ਦਾ_ਸੌਂਪੋ_VEC4_OFF_E(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      ਬਿਨਾਨਿਸ਼ਾਨੀ off_ਸੂਚਕਾਂਕ = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      vvp_vector4_t val = thr->ਕੱਢੋ_vec4();

      int64_t off = thr->ਸ਼ਬਦ[off_ਸੂਚਕਾਂਕ].w_int;

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      ਜੇ (thr->flags[4] == BIT4_1)
	    ਵਾਪਸ ਸੱਚ;

      vvp_signal_ਮੁੱਲ*sig = dynamic_cast<vvp_signal_ਮੁੱਲ*> (cp->net->fil);
      ਪ੍ਰਤਿਜ੍ਞਾ(sig);

      ਜੇ (!resize_rval_vec(val, off, sig->ਮੁੱਲ_ਮਾਪ()))
	    ਵਾਪਸ ਸੱਚ;

      ਜੇ (thr->ecount == 0) {
	    ਸਮਾਂ_ਸਾਰਣੀ_ਸੌਂਪੋ_ਸਦਿਸ਼(ptr, off, sig->ਮੁੱਲ_ਮਾਪ(), val, 0);
      } ਨਹੀਂਤਾਂ {
	    ਸਮਾਂ_ਸਾਰਣੀ_evctl(ptr, val, off, sig->ਮੁੱਲ_ਮਾਪ(), thr->event, thr->ecount);
      }

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %assign/vec4/d <var-label> <delay>
 */
ਤਾਰਕਿਕ ਦਾ_ਸੌਂਪੋ_VEC4_D(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      ਬਿਨਾਨਿਸ਼ਾਨੀ del_ਸੂਚਕਾਂਕ = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      vvp_time64_t del = thr->ਸ਼ਬਦ[del_ਸੂਚਕਾਂਕ].w_int;

      vvp_vector4_t value = thr->ਕੱਢੋ_vec4();

      vvp_signal_ਮੁੱਲ*sig = dynamic_cast<vvp_signal_ਮੁੱਲ*> (cp->net->fil);
      ਪ੍ਰਤਿਜ੍ਞਾ(sig);

      ਸਮਾਂ_ਸਾਰਣੀ_ਸੌਂਪੋ_ਸਦਿਸ਼(ptr, 0, sig->ਮੁੱਲ_ਮਾਪ(), value, del);

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %assign/vec4/e <var-label>
 */
ਤਾਰਕਿਕ ਦਾ_ਸੌਂਪੋ_VEC4_E(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_vector4_t value = thr->ਕੱਢੋ_vec4();

      vvp_signal_ਮੁੱਲ*sig = dynamic_cast<vvp_signal_ਮੁੱਲ*> (cp->net->fil);
      ਪ੍ਰਤਿਜ੍ਞਾ(sig);

      ਜੇ (thr->ecount == 0) {
	    ਸਮਾਂ_ਸਾਰਣੀ_ਸੌਂਪੋ_ਸਦਿਸ਼(ptr, 0, sig->ਮੁੱਲ_ਮਾਪ(), value, 0);
      } ਨਹੀਂਤਾਂ {
	    ਸਮਾਂ_ਸਾਰਣੀ_evctl(ptr, value, 0, sig->ਮੁੱਲ_ਮਾਪ(), thr->event, thr->ecount);
      }

      ਵਾਪਸ ਸੱਚ;
}

/*
 * This is %assign/wr <vpi-label>, <delay>
 *
 * This assigns (after a delay) a value to a real variable. Use the
 * vpi_put_value function to do the assign, with the delay written
 * into the vpiInertialDelay carrying the desired delay.
 */
ਤਾਰਕਿਕ ਦਾ_ਸੌਂਪੋ_WR(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ delay = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      ਦੁੱਗਣਾ value = thr->ਕੱਢੋ_real();
      s_vpi_ਸਮਾਂ del;

      del.type = vpi_Sim_ਸਮਾਂ;
      vpip_ਸਮਾਂ_ਮੰਜ਼ਿਲ_timestruct(&del, delay);

      __vpiHandle*tmp = cp->handle;

      t_vpi_ਮੁੱਲ val;
      val.format = vpiRealVal;
      val.value.real = value;
      vpi_put_ਮੁੱਲ(tmp, &val, &del, vpiTransportDelay);

      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_ਸੌਂਪੋ_WRD(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_time64_t delay = thr->ਸ਼ਬਦ[cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0]].w_uint;
      ਦੁੱਗਣਾ value = thr->ਕੱਢੋ_real();
      s_vpi_ਸਮਾਂ del;

      del.type = vpi_Sim_ਸਮਾਂ;
      vpip_ਸਮਾਂ_ਮੰਜ਼ਿਲ_timestruct(&del, delay);

      __vpiHandle*tmp = cp->handle;

      t_vpi_ਮੁੱਲ val;
      val.format = vpiRealVal;
      val.value.real = value;
      vpi_put_ਮੁੱਲ(tmp, &val, &del, vpiTransportDelay);

      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_ਸੌਂਪੋ_WRE(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(thr->event != 0);
      ਦੁੱਗਣਾ value = thr->ਕੱਢੋ_real();
      __vpiHandle*tmp = cp->handle;

	// If the count is zero then just put the value.
      ਜੇ (thr->ecount == 0) {
	    t_vpi_ਮੁੱਲ val;

	    val.format = vpiRealVal;
	    val.value.real = value;
	    vpi_put_ਮੁੱਲ(tmp, &val, 0, vpiNoDelay);
      } ਨਹੀਂਤਾਂ {
	    ਸਮਾਂ_ਸਾਰਣੀ_evctl(tmp, value, thr->event, thr->ecount);
      }

      thr->event = 0;
      thr->ecount = 0;

      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_BLEND(vthread_t thr, vvp_ਕੋਡ_t)
{
      vvp_vector4_t vala = thr->ਕੱਢੋ_vec4();
      vvp_vector4_t valb = thr->ਕੱਢੋ_vec4();
      ਪ੍ਰਤਿਜ੍ਞਾ(vala.size() == valb.size());

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < vala.size() ; idx += 1) {
	    ਜੇ (vala.value(idx) == valb.value(idx))
		  ਜਾਰੀ;

	    vala.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_X);
      }

      thr->ਧੱਕੋ_vec4(vala);
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_BLEND_STR(vthread_t thr, vvp_ਕੋਡ_t)
{
      string f = thr->ਕੱਢੋ_str();
      string t = thr->ਕੱਢੋ_str();
      thr->ਧੱਕੋ_str((t == f) ? t : "");
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_BLEND_WR(vthread_t thr, vvp_ਕੋਡ_t)
{
      ਦੁੱਗਣਾ f = thr->ਕੱਢੋ_real();
      ਦੁੱਗਣਾ t = thr->ਕੱਢੋ_real();
      thr->ਧੱਕੋ_real((t == f) ? t : 0.0);
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_BREAKPOINT(vthread_t, vvp_ਕੋਡ_t)
{
      ਵਾਪਸ ਸੱਚ;
}

/*
 * %callf/void <code-label>, <scope-label>
 * Combine the %fork and %join steps for invoking a function.
 */
ਸਥਿਰ ਤਾਰਕਿਕ do_callf_void(vthread_t thr, vthread_t ਬੱਚਾ)
{

      ਜੇ (ਬੱਚਾ->ਮਾਪਾ_scope->is_automatic()) {
	      /* The context allocated for this child is the top entry
		 on the write context stack */
	    ਬੱਚਾ->wt_context = thr->wt_context;
	    ਬੱਚਾ->rd_context = thr->wt_context;
      }

        // Mark the function thread as a direct child of the current thread.
      ਬੱਚਾ->ਮਾਪਾ = thr;
      thr->children.insert(ਬੱਚਾ);
        // This should be the only child
      ਪ੍ਰਤਿਜ੍ਞਾ(thr->children.size()==1);

        // Execute the function. This SHOULD run the function to completion,
        // but there are some exceptional situations where it won't.
      ਪ੍ਰਤਿਜ੍ਞਾ(ਬੱਚਾ->ਮਾਪਾ_scope->get_type_ਕੋਡ() == vpiFunction);
      ਬੱਚਾ->is_scheduled = 1;
      ਬੱਚਾ->i_am_ਵਿੱਚ_function = 1;
      vthread_ਲੜੀਵਾਰ(ਬੱਚਾ);
      running_thread = thr;

      ਜੇ (ਬੱਚਾ->i_have_ended) {
	    do_join(thr, ਬੱਚਾ);
	    ਵਾਪਸ ਸੱਚ;
      } ਨਹੀਂਤਾਂ {
	    thr->i_am_joining = 1;
	    ਵਾਪਸ ਝੂਠ;
      }
}

ਤਾਰਕਿਕ ਦਾ_CALLF_OBJ(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vthread_t ਬੱਚਾ = vthread_new(cp->cptr2, cp->scope);
      ਵਾਪਸ do_callf_void(thr, ਬੱਚਾ);

      // XXXX NOT IMPLEMENTED
}

ਤਾਰਕਿਕ ਦਾ_CALLF_REAL(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vthread_t ਬੱਚਾ = vthread_new(cp->cptr2, cp->scope);

	// This is the return value. Push a place-holder value. The function
	// will replace this with the actual value using a %ret/real instruction.
      thr->ਧੱਕੋ_real(0.0);
      ਬੱਚਾ->ਦਲੀਲਾਂ_real.push_back(0);

      ਵਾਪਸ do_callf_void(thr, ਬੱਚਾ);
}

ਤਾਰਕਿਕ ਦਾ_CALLF_STR(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vthread_t ਬੱਚਾ = vthread_new(cp->cptr2, cp->scope);

      thr->ਧੱਕੋ_str("");
      ਬੱਚਾ->ਦਲੀਲਾਂ_str.push_back(0);

      ਵਾਪਸ do_callf_void(thr, ਬੱਚਾ);
}

ਤਾਰਕਿਕ ਦਾ_CALLF_VEC4(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vthread_t ਬੱਚਾ = vthread_new(cp->cptr2, cp->scope);

      vpiScopeFunction*scope_func = dynamic_cast<vpiScopeFunction*>(cp->scope);
      ਪ੍ਰਤਿਜ੍ਞਾ(scope_func);

	// This is the return value. Push a place-holder value. The function
	// will replace this with the actual value using a %ret/real instruction.
      thr->ਧੱਕੋ_vec4(vvp_vector4_t(scope_func->get_func_ਚੌੜਾਈ(), scope_func->get_func_init_val()));
      ਬੱਚਾ->ਦਲੀਲਾਂ_vec4.push_back(0);

      ਵਾਪਸ do_callf_void(thr, ਬੱਚਾ);
}

ਤਾਰਕਿਕ ਦਾ_CALLF_VOID(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vthread_t ਬੱਚਾ = vthread_new(cp->cptr2, cp->scope);
      ਵਾਪਸ do_callf_void(thr, ਬੱਚਾ);
}

/*
 * The %cassign/link instruction connects a source node to a
 * destination node. The destination node must be a signal, as it is
 * marked with the source of the cassign so that it may later be
 * unlinked without specifically knowing the source that this
 * instruction used.
 */
ਤਾਰਕਿਕ ਦਾ_CASSIGN_LINK(vthread_t, vvp_ਕੋਡ_t cp)
{
      vvp_net_t*dst = cp->net;
      vvp_net_t*src = cp->net2;

      vvp_fun_signal_ਅਧਾਰ*sig
	    = dynamic_cast<vvp_fun_signal_ਅਧਾਰ*>(dst->fun);
      ਪ੍ਰਤਿਜ੍ਞਾ(sig);

	/* Any previous continuous assign should have been removed already. */
      ਪ੍ਰਤਿਜ੍ਞਾ(sig->cassign_link == 0);

      sig->cassign_link = src;

	/* Link the output of the src to the port[1] (the cassign
	   port) of the destination. */
      vvp_net_ptr_t dst_ptr (dst, 1);
      src->link(dst_ptr);

      ਵਾਪਸ ਸੱਚ;
}

/*
 * If there is an existing continuous assign linked to the destination
 * node, unlink it. This must be done before applying a new continuous
 * assign, otherwise the initial assigned value will be propagated to
 * any other nodes driven by the old continuous assign source.
 */
ਸਥਿਰ ਖਾਲੀ cassign_unlink(vvp_net_t*dst)
{
      vvp_fun_signal_ਅਧਾਰ*sig
	    = dynamic_cast<vvp_fun_signal_ਅਧਾਰ*>(dst->fun);
      ਪ੍ਰਤਿਜ੍ਞਾ(sig);

      ਜੇ (sig->cassign_link == 0)
	    ਵਾਪਸ;

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
ਤਾਰਕਿਕ ਦਾ_CASSIGN_VEC4(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_net_t*net = cp->net;
      vvp_vector4_t value = thr->ਕੱਢੋ_vec4();

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

	/* Set the value into port 1 of the destination. */
      vvp_net_ptr_t ptr (net, 1);
      vvp_send_vec4(ptr, value, 0);

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %cassign/vec4/off <var>, <off>
 */
ਤਾਰਕਿਕ ਦਾ_CASSIGN_VEC4_OFF(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_net_t*net = cp->net;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ_idx = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      ਲੰਮਾ ਅਧਾਰ = thr->ਸ਼ਬਦ[ਅਧਾਰ_idx].w_int;
      vvp_vector4_t value = thr->ਕੱਢੋ_vec4();
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = value.size();

      ਜੇ (thr->flags[4] == BIT4_1)
	    ਵਾਪਸ ਸੱਚ;

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

      vvp_signal_ਮੁੱਲ*sig = dynamic_cast<vvp_signal_ਮੁੱਲ*> (net->fil);
      ਪ੍ਰਤਿਜ੍ਞਾ(sig);

      ਜੇ (ਅਧਾਰ < 0 && (wid <= (ਬਿਨਾਨਿਸ਼ਾਨੀ)-ਅਧਾਰ))
	    ਵਾਪਸ ਸੱਚ;

      ਜੇ (ਅਧਾਰ >= (ਲੰਮਾ)sig->ਮੁੱਲ_ਮਾਪ())
	    ਵਾਪਸ ਸੱਚ;

      ਜੇ (ਅਧਾਰ < 0) {
	    wid -= (ਬਿਨਾਨਿਸ਼ਾਨੀ) -ਅਧਾਰ;
	    ਅਧਾਰ = 0;
	    value.resize(wid);
      }

      ਜੇ (ਅਧਾਰ+wid > sig->ਮੁੱਲ_ਮਾਪ()) {
	    wid = sig->ਮੁੱਲ_ਮਾਪ() - ਅਧਾਰ;
	    value.resize(wid);
      }

      vvp_net_ptr_t ptr (net, 1);
      vvp_send_vec4_pv(ptr, value, ਅਧਾਰ, sig->ਮੁੱਲ_ਮਾਪ(), 0);
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_CASSIGN_WR(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_net_t*net  = cp->net;
      ਦੁੱਗਣਾ value = thr->ਕੱਢੋ_real();

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

	/* Set the value into port 1 of the destination. */
      vvp_net_ptr_t ptr (net, 1);
      vvp_send_real(ptr, value, 0);

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %cast2
 */
ਤਾਰਕਿਕ ਦਾ_CAST2(vthread_t thr, vvp_ਕੋਡ_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = val.size();

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < wid ; idx += 1) {
	    ਚੋਣ (val.value(idx)) {
		ਹਾਲਤ BIT4_0:
		ਹਾਲਤ BIT4_1:
		  ਰੋਕੋ;
		ਮੂਲ:
		  val.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_0);
		  ਰੋਕੋ;
	    }
      }

      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ do_cast_vec_dar(vthread_t thr, vvp_ਕੋਡ_t cp, ਤਾਰਕਿਕ as_vec4)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਸੰਖਿਆ;

      vvp_object_t obj;
      thr->ਕੱਢੋ_object(obj);

      vvp_darray*darray = obj.peek<vvp_darray>();
      ਪ੍ਰਤਿਜ੍ਞਾ(darray);

      vvp_vector4_t vec = darray->get_bitstream(as_vec4);
      ਜੇ (vec.size() != wid) {
	    cerr << thr->get_fileline()
	         << "VVP error: size mismatch when casting dynamic array to vector." << ਸਤਰ_ਅੰਤ;
            thr->ਧੱਕੋ_vec4(vvp_vector4_t(wid));
            ਸਮਾਂ_ਸਾਰਣੀ_stop(0);
            ਵਾਪਸ ਝੂਠ;
      }
      thr->ਧੱਕੋ_vec4(vec);
      ਵਾਪਸ ਸੱਚ;
}

/*
 * %cast/vec2/dar <wid>
 */
ਤਾਰਕਿਕ ਦਾ_CAST_VEC2_DAR(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ do_cast_vec_dar(thr, cp, ਝੂਠ);
}

/*
 * %cast/vec4/dar <wid>
 */
ਤਾਰਕਿਕ ਦਾ_CAST_VEC4_DAR(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ do_cast_vec_dar(thr, cp, ਸੱਚ);
}

/*
 * %cast/vec4/str <wid>
 */
ਤਾਰਕਿਕ ਦਾ_CAST_VEC4_STR(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਸੰਖਿਆ;
      string str = thr->ਕੱਢੋ_str();

      vvp_vector4_t vec(wid, BIT4_0);

      ਜੇ (wid != 8*str.length()) {
	    cerr << thr->get_fileline()
	         << "VVP error: size mismatch when casting string to vector." << ਸਤਰ_ਅੰਤ;
            thr->ਧੱਕੋ_vec4(vec);
            ਸਮਾਂ_ਸਾਰਣੀ_stop(0);
            ਵਾਪਸ ਝੂਠ;
      }

      ਬਿਨਾਨਿਸ਼ਾਨੀ sdx = 0;
      ਬਿਨਾਨਿਸ਼ਾਨੀ vdx = wid;
      ਜਦੋਂ (vdx > 0) {
            ਅੱਖਰ ch = str[sdx++];
            vdx -= 8;
            ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ bdx = 0; bdx < 8; bdx += 1) {
                  ਜੇ (ch & 1)
                        vec.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(vdx+bdx, BIT4_1);
                  ch >>= 1;
            }
      }

      thr->ਧੱਕੋ_vec4(vec);
      ਵਾਪਸ ਸੱਚ;
}

ਸਥਿਰ ਖਾਲੀ do_CMPE(vthread_t thr, ਅਚਲ vvp_vector4_t&lval, ਅਚਲ vvp_vector4_t&rval)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(rval.size() == lval.size());

      ਜੇ (lval.has_xz() || rval.has_xz()) {

	    ਬਿਨਾਨਿਸ਼ਾਨੀ wid = lval.size();
	    vvp_bit4_t eq  = BIT4_1;
	    vvp_bit4_t eeq = BIT4_1;

	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < wid ; idx += 1) {
		  vvp_bit4_t lv = lval.value(idx);
		  vvp_bit4_t rv = rval.value(idx);

		  ਜੇ (lv != rv)
			eeq = BIT4_0;

		  ਜੇ (eq==BIT4_1 && (bit4_is_xz(lv) || bit4_is_xz(rv)))
			eq = BIT4_X;
		  ਜੇ ((lv == BIT4_0) && (rv==BIT4_1))
			eq = BIT4_0;
		  ਜੇ ((lv == BIT4_1) && (rv==BIT4_0))
			eq = BIT4_0;

		  ਜੇ (eq == BIT4_0)
			ਰੋਕੋ;
	    }

	    thr->flags[4] = eq;
	    thr->flags[6] = eeq;

      } ਨਹੀਂਤਾਂ {
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
ਤਾਰਕਿਕ ਦਾ_CMPE(vthread_t thr, vvp_ਕੋਡ_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ਅਚਲ vvp_vector4_t&rval = thr->peek_vec4(0);
      ਅਚਲ vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPE(thr, lval, rval);

      thr->ਕੱਢੋ_vec4(2);
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_CMPNE(vthread_t thr, vvp_ਕੋਡ_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ਅਚਲ vvp_vector4_t&rval = thr->peek_vec4(0);
      ਅਚਲ vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];
      thr->flags[6] =  ~thr->flags[6];

      thr->ਕੱਢੋ_vec4(2);
      ਵਾਪਸ ਸੱਚ;
}

/*
 * %cmpi/e <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
ਤਾਰਕਿਕ ਦਾ_CMPIE(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਸੰਖਿਆ;

      ਅਚਲ vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_ਤੁਰੰਤ_rval (cp, rval);

      do_CMPE(thr, lval, rval);

      thr->ਕੱਢੋ_vec4(1);
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_CMPINE(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਸੰਖਿਆ;

      ਅਚਲ vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_ਤੁਰੰਤ_rval (cp, rval);

      do_CMPE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];
      thr->flags[6] =  ~thr->flags[6];

      thr->ਕੱਢੋ_vec4(1);
      ਵਾਪਸ ਸੱਚ;
}



ਸਥਿਰ ਖਾਲੀ do_CMPS(vthread_t thr, ਅਚਲ vvp_vector4_t&lval, ਅਚਲ vvp_vector4_t&rval)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(rval.size() == lval.size());

	// If either value has XZ bits, then the eq and lt values are
	// known already to be X. Just calculate the eeq result as a
	// special case and short circuit the rest of the compare.
      ਜੇ (lval.has_xz() || rval.has_xz()) {
	    thr->flags[4] = BIT4_X; // eq
	    thr->flags[5] = BIT4_X; // lt
	    thr->flags[6] = lval.eeq(rval)? BIT4_1 : BIT4_0;
	    ਵਾਪਸ;
      }

	// Past this point, we know we are dealing only with fully
	// defined values.
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = lval.size();

      ਅਚਲ vvp_bit4_t sig1 = lval.value(wid-1);
      ਅਚਲ vvp_bit4_t sig2 = rval.value(wid-1);

	// If the lval is <0 and the rval is >=0, then we know the result.
      ਜੇ ((sig1 == BIT4_1) && (sig2 == BIT4_0)) {
	    thr->flags[4] = BIT4_0; // eq;
	    thr->flags[5] = BIT4_1; // lt;
	    thr->flags[6] = BIT4_0; // eeq
	    ਵਾਪਸ;
      }

	// If the lval is >=0 and the rval is <0, then we know the result.
      ਜੇ ((sig1 == BIT4_0) && (sig2 == BIT4_1)) {
	    thr->flags[4] = BIT4_0; // eq;
	    thr->flags[5] = BIT4_0; // lt;
	    thr->flags[6] = BIT4_0; // eeq
	    ਵਾਪਸ;
      }

	// The values have the same sign, so we have to look at the
	// actual value. Scan from the MSB down. As soon as we find a
	// bit that differs, we know the result.

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 1 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t lv = lval.value(wid-1-idx);
	    vvp_bit4_t rv = rval.value(wid-1-idx);

	    ਜੇ (lv == rv)
		  ਜਾਰੀ;

	    thr->flags[4] = BIT4_0; // eq
	    thr->flags[6] = BIT4_0; // eeq

	    ਜੇ (lv==BIT4_0) {
		  thr->flags[5] = BIT4_1; // lt
	    } ਨਹੀਂਤਾਂ {
		  thr->flags[5] = BIT4_0; // lt
	    }
	    ਵਾਪਸ;
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
ਤਾਰਕਿਕ ਦਾ_CMPS(vthread_t thr, vvp_ਕੋਡ_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ਅਚਲ vvp_vector4_t&rval = thr->peek_vec4(0);
      ਅਚਲ vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPS(thr, lval, rval);

      thr->ਕੱਢੋ_vec4(2);
      ਵਾਪਸ ਸੱਚ;
}

/*
 * %cmpi/s <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
ਤਾਰਕਿਕ ਦਾ_CMPIS(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਸੰਖਿਆ;

      ਅਚਲ vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_ਤੁਰੰਤ_rval (cp, rval);

      do_CMPS(thr, lval, rval);

      thr->ਕੱਢੋ_vec4(1);
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_CMPSTR(vthread_t thr, vvp_ਕੋਡ_t)
{
      string re = thr->ਕੱਢੋ_str();
      string le = thr->ਕੱਢੋ_str();

      ਪੂਰਨ rc = strcmp(le.c_str(), re.c_str());

      vvp_bit4_t eq;
      vvp_bit4_t lt;

      ਜੇ (rc == 0) {
	    eq = BIT4_1;
	    lt = BIT4_0;
      } ਨਹੀਂਤਾਂ ਜੇ (rc < 0) {
	    eq = BIT4_0;
	    lt = BIT4_1;
      } ਨਹੀਂਤਾਂ {
	    eq = BIT4_0;
	    lt = BIT4_0;
      }

      thr->flags[4] = eq;
      thr->flags[5] = lt;

      ਵਾਪਸ ਸੱਚ;
}

ਸਥਿਰ ਖਾਲੀ ਦਾ_CMPU_the_hard_way(vthread_t thr, ਬਿਨਾਨਿਸ਼ਾਨੀ wid,
				 ਅਚਲ vvp_vector4_t&lval,
				 ਅਚਲ vvp_vector4_t&rval)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_bit4_t eeq = BIT4_1;

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t lv = lval.value(idx);
	    vvp_bit4_t rv = rval.value(idx);

	    ਜੇ (lv != rv)
		  eeq = BIT4_0;

	    ਜੇ (eq==BIT4_1 && (bit4_is_xz(lv) || bit4_is_xz(rv)))
		  eq = BIT4_X;
	    ਜੇ ((lv == BIT4_0) && (rv==BIT4_1))
		  eq = BIT4_0;
	    ਜੇ ((lv == BIT4_1) && (rv==BIT4_0))
		  eq = BIT4_0;

	    ਜੇ (eq == BIT4_0)
		  ਰੋਕੋ;

      }

      thr->flags[4] = eq;
      thr->flags[5] = BIT4_X;
      thr->flags[6] = eeq;
}

ਸਥਿਰ ਖਾਲੀ do_CMPU(vthread_t thr, ਅਚਲ vvp_vector4_t&lval, ਅਚਲ vvp_vector4_t&rval)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_bit4_t lt = BIT4_0;

      ਜੇ (rval.size() != lval.size()) {
	    cerr << thr->get_fileline()
	         << "VVP ERROR: %cmp/u operand width mismatch: lval=" << lval
		 << ", rval=" << rval << ਸਤਰ_ਅੰਤ;
      }
      ਪ੍ਰਤਿਜ੍ਞਾ(rval.size() == lval.size());
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = lval.size();

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ*larray = lval.subarray(0,wid);
      ਜੇ (larray == 0) ਵਾਪਸ ਦਾ_CMPU_the_hard_way(thr, wid, lval, rval);

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ*rarray = rval.subarray(0,wid);
      ਜੇ (rarray == 0) {
	    ਮਿਟਾਓ[]larray;
	    ਵਾਪਸ ਦਾ_CMPU_the_hard_way(thr, wid, lval, rval);
      }

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ = (wid+CPU_ਸ਼ਬਦ_ਬਿੱਟ-1) / CPU_ਸ਼ਬਦ_ਬਿੱਟ;

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ wdx = 0 ; wdx < ਸ਼ਬਦ ; wdx += 1) {
	    ਜੇ (larray[wdx] == rarray[wdx])
		  ਜਾਰੀ;

	    eq = BIT4_0;
	    ਜੇ (larray[wdx] < rarray[wdx])
		  lt = BIT4_1;
	    ਨਹੀਂਤਾਂ
		  lt = BIT4_0;
      }

      ਮਿਟਾਓ[]larray;
      ਮਿਟਾਓ[]rarray;

      thr->flags[4] = eq;
      thr->flags[5] = lt;
      thr->flags[6] = eq;
}

ਤਾਰਕਿਕ ਦਾ_CMPU(vthread_t thr, vvp_ਕੋਡ_t)
{

      ਅਚਲ vvp_vector4_t&rval = thr->peek_vec4(0);
      ਅਚਲ vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPU(thr, lval, rval);

      thr->ਕੱਢੋ_vec4(2);
      ਵਾਪਸ ਸੱਚ;
}

/*
 * %cmpi/u <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
ਤਾਰਕਿਕ ਦਾ_CMPIU(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਸੰਖਿਆ;

      ਅਚਲ vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_ਤੁਰੰਤ_rval (cp, rval);

      do_CMPU(thr, lval, rval);

      thr->ਕੱਢੋ_vec4(1);
      ਵਾਪਸ ਸੱਚ;
}


/*
 * %cmp/x
 */
ਤਾਰਕਿਕ ਦਾ_CMPX(vthread_t thr, vvp_ਕੋਡ_t)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_vector4_t rval = thr->ਕੱਢੋ_vec4();
      vvp_vector4_t lval = thr->ਕੱਢੋ_vec4();

      ਪ੍ਰਤਿਜ੍ਞਾ(rval.size() == lval.size());
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = lval.size();

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < wid ; idx += 1) {
	    vvp_bit4_t lv = lval.value(idx);
	    vvp_bit4_t rv = rval.value(idx);
	    ਜੇ ((lv != rv) && !bit4_is_xz(lv) && !bit4_is_xz(rv)) {
		  eq = BIT4_0;
		  ਰੋਕੋ;
	    }
      }

      thr->flags[4] = eq;
      ਵਾਪਸ ਸੱਚ;
}

ਸਥਿਰ ਖਾਲੀ do_CMPWE(vthread_t thr, ਅਚਲ vvp_vector4_t&lval, ਅਚਲ vvp_vector4_t&rval)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(rval.size() == lval.size());

      ਜੇ (lval.has_xz() || rval.has_xz()) {

	    ਬਿਨਾਨਿਸ਼ਾਨੀ wid = lval.size();
	    vvp_bit4_t eq  = BIT4_1;

	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < wid ; idx += 1) {
		  vvp_bit4_t lv = lval.value(idx);
		  vvp_bit4_t rv = rval.value(idx);

		  ਜੇ (bit4_is_xz(rv))
			ਜਾਰੀ;
		  ਜੇ ((eq == BIT4_1) && bit4_is_xz(lv))
			eq = BIT4_X;
		  ਜੇ ((lv == BIT4_0) && (rv==BIT4_1))
			eq = BIT4_0;
		  ਜੇ ((lv == BIT4_1) && (rv==BIT4_0))
			eq = BIT4_0;

		  ਜੇ (eq == BIT4_0)
			ਰੋਕੋ;
	    }

	    thr->flags[4] = eq;

      } ਨਹੀਂਤਾਂ {
	      // If there are no XZ bits anywhere, then the results of
	      // ==? match the === test.
	    thr->flags[4] = (lval.eeq(rval)? BIT4_1 : BIT4_0);
      }
}

ਤਾਰਕਿਕ ਦਾ_CMPWE(vthread_t thr, vvp_ਕੋਡ_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ਅਚਲ vvp_vector4_t&rval = thr->peek_vec4(0);
      ਅਚਲ vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPWE(thr, lval, rval);

      thr->ਕੱਢੋ_vec4(2);
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_CMPWNE(vthread_t thr, vvp_ਕੋਡ_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ਅਚਲ vvp_vector4_t&rval = thr->peek_vec4(0);
      ਅਚਲ vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPWE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];

      thr->ਕੱਢੋ_vec4(2);
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_CMPWR(vthread_t thr, vvp_ਕੋਡ_t)
{
      ਦੁੱਗਣਾ r = thr->ਕੱਢੋ_real();
      ਦੁੱਗਣਾ l = thr->ਕੱਢੋ_real();

      vvp_bit4_t eq = (l == r)? BIT4_1 : BIT4_0;
      vvp_bit4_t lt = (l <  r)? BIT4_1 : BIT4_0;

      thr->flags[4] = eq;
      thr->flags[5] = lt;

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %cmp/z
 */
ਤਾਰਕਿਕ ਦਾ_CMPZ(vthread_t thr, vvp_ਕੋਡ_t)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_vector4_t rval = thr->ਕੱਢੋ_vec4();
      vvp_vector4_t lval = thr->ਕੱਢੋ_vec4();

      ਪ੍ਰਤਿਜ੍ਞਾ(rval.size() == lval.size());
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = lval.size();

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < wid ; idx += 1) {
	    vvp_bit4_t lv = lval.value(idx);
	    vvp_bit4_t rv = rval.value(idx);
	    ਜੇ ((lv != rv) && (rv != BIT4_Z) && (lv != BIT4_Z)) {
		  eq = BIT4_0;
		  ਰੋਕੋ;
	    }
      }

      thr->flags[4] = eq;
      ਵਾਪਸ ਸੱਚ;
}

/*
 *  %concat/str;
 */
ਤਾਰਕਿਕ ਦਾ_CONCAT_STR(vthread_t thr, vvp_ਕੋਡ_t)
{
      string ਪਾਠ = thr->ਕੱਢੋ_str();
      thr->peek_str(0).append(ਪਾਠ);
      ਵਾਪਸ ਸੱਚ;
}

/*
 *  %concati/str <string>;
 */
ਤਾਰਕਿਕ ਦਾ_CONCATI_STR(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਅਚਲ ਅੱਖਰ*ਪਾਠ = cp->ਪਾਠ;
      thr->peek_str(0).append(filter_string(ਪਾਠ));
      ਵਾਪਸ ਸੱਚ;
}

/*
 * %concat/vec4
 */
ਤਾਰਕਿਕ ਦਾ_CONCAT_VEC4(vthread_t thr, vvp_ਕੋਡ_t)
{
      ਅਚਲ vvp_vector4_t&lsb = thr->peek_vec4(0);
      ਅਚਲ vvp_vector4_t&msb = thr->peek_vec4(1);

	// The result is the size of the top two vectors in the stack.
      vvp_vector4_t res (msb.size() + lsb.size(), BIT4_X);

	// Build up the result.
      res.ਸੈੱਟ_vec(0, lsb);
      res.ਸੈੱਟ_vec(lsb.size(), msb);

	// Rearrange the stack to pop the inputs and push the
	// result. Do that by actually popping only 1 stack position
	// and replacing the new top with the new value.
      thr->ਕੱਢੋ_vec4(1);
      thr->peek_vec4() = res;

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %concati/vec4 <vala>, <valb>, <wid>
 *
 * Concat the immediate value to the LOW bits of the concatenation.
 * Get the HIGH bits from the top of the vec4 stack.
 */
ਤਾਰਕਿਕ ਦਾ_CONCATI_VEC4(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid  = cp->ਸੰਖਿਆ;

      vvp_vector4_t&msb = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t lsb (wid, BIT4_0);
      get_ਤੁਰੰਤ_rval (cp, lsb);

      vvp_vector4_t res (msb.size()+lsb.size(), BIT4_X);
      res.ਸੈੱਟ_vec(0, lsb);
      res.ਸੈੱਟ_vec(lsb.size(), msb);

      msb = res;
      ਵਾਪਸ ਸੱਚ;
}

/*
 * %cvt/rv
 */
ਤਾਰਕਿਕ ਦਾ_CVT_RV(vthread_t thr, vvp_ਕੋਡ_t)
{
      ਦੁੱਗਣਾ val;
      vvp_vector4_t val4 = thr->ਕੱਢੋ_vec4();
      vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(val4, val, ਝੂਠ);
      thr->ਧੱਕੋ_real(val);
      ਵਾਪਸ ਸੱਚ;
}

/*
 * %cvt/rv/s
 */
ਤਾਰਕਿਕ ਦਾ_CVT_RV_S(vthread_t thr, vvp_ਕੋਡ_t)
{
      ਦੁੱਗਣਾ val;
      vvp_vector4_t val4 = thr->ਕੱਢੋ_vec4();
      vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(val4, val, ਸੱਚ);
      thr->ਧੱਕੋ_real(val);
      ਵਾਪਸ ਸੱਚ;
}

/*
 * %cvt/sr <idx>
 * Pop the top value from the real stack, convert it to a 64bit signed
 * and save it to the indexed register.
 */
ਤਾਰਕਿਕ ਦਾ_CVT_SR(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਦੁੱਗਣਾ r = thr->ਕੱਢੋ_real();
      thr->ਸ਼ਬਦ[cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0]].w_int = i64round(r);

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %cvt/ur <idx>
 */
ਤਾਰਕਿਕ ਦਾ_CVT_UR(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਦੁੱਗਣਾ r = thr->ਕੱਢੋ_real();
      thr->ਸ਼ਬਦ[cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0]].w_uint = vlg_ਗੇੜਾ_ਮੰਜ਼ਿਲ_u64(r);

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %cvt/vr <wid>
 */
ਤਾਰਕਿਕ ਦਾ_CVT_VR(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਦੁੱਗਣਾ r = thr->ਕੱਢੋ_real();
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਸੰਖਿਆ;

      vvp_vector4_t tmp(wid, r);
      thr->ਧੱਕੋ_vec4(tmp);
      ਵਾਪਸ ਸੱਚ;
}

/*
 * This implements the %deassign instruction. All we do is write a
 * long(1) to port-3 of the addressed net. This turns off an active
 * continuous assign activated by %cassign/v
 */
ਤਾਰਕਿਕ ਦਾ_DEASSIGN(vthread_t, vvp_ਕੋਡ_t cp)
{
      vvp_net_t*net = cp->net;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ  = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      ਬਿਨਾਨਿਸ਼ਾਨੀ width = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[1];

      vvp_signal_ਮੁੱਲ*fil = dynamic_cast<vvp_signal_ਮੁੱਲ*> (net->fil);
      ਪ੍ਰਤਿਜ੍ਞਾ(fil);
      vvp_fun_signal_vec*sig = dynamic_cast<vvp_fun_signal_vec*>(net->fun);
      ਪ੍ਰਤਿਜ੍ਞਾ(sig);

      ਜੇ (ਅਧਾਰ >= fil->ਮੁੱਲ_ਮਾਪ()) ਵਾਪਸ ਸੱਚ;
      ਜੇ (ਅਧਾਰ+width > fil->ਮੁੱਲ_ਮਾਪ()) width = fil->ਮੁੱਲ_ਮਾਪ() - ਅਧਾਰ;

      ਤਾਰਕਿਕ ਭਰਿਆ_sig = ਅਧਾਰ == 0 && width == fil->ਮੁੱਲ_ਮਾਪ();

	// This is the net that is forcing me...
      ਜੇ (vvp_net_t*src = sig->cassign_link) {
	    ਜੇ (! ਭਰਿਆ_sig) {
		  fprintf(stderr, "Sorry: when a signal is assigning a "
		          "register, I cannot deassign part of it.\n");
		  ਬਾਹਰ_ਨਿਕਲੋ(1);
	    }
	      // And this is the pointer to be removed.
	    vvp_net_ptr_t dst_ptr (net, 1);
	    src->unlink(dst_ptr);
	    sig->cassign_link = 0;
      }

	/* Do we release all or part of the net? */
      ਜੇ (ਭਰਿਆ_sig) {
	    sig->deassign();
      } ਨਹੀਂਤਾਂ {
	    sig->deassign_pv(ਅਧਾਰ, width);
      }

      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_DEASSIGN_WR(vthread_t, vvp_ਕੋਡ_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_real*sig = dynamic_cast<vvp_fun_signal_real*>(net->fun);
      ਪ੍ਰਤਿਜ੍ਞਾ(sig);

	// This is the net that is forcing me...
      ਜੇ (vvp_net_t*src = sig->cassign_link) {
	      // And this is the pointer to be removed.
	    vvp_net_ptr_t dst_ptr (net, 1);
	    src->unlink(dst_ptr);
	    sig->cassign_link = 0;
      }

      sig->deassign();

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %debug/thr
 */
ਤਾਰਕਿਕ ਦਾ_DEBUG_THR(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਅਚਲ ਅੱਖਰ*ਪਾਠ = cp->ਪਾਠ;
      thr->debug_dump(cerr, ਪਾਠ);
      ਵਾਪਸ ਸੱਚ;
}

/*
 * The delay takes two 32bit numbers to make up a 64bit time.
 *
 *   %delay <low>, <hig>
 */
ਤਾਰਕਿਕ ਦਾ_DELAY(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_time64_t ਨੀਵਾਂ = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      vvp_time64_t hig = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[1];

      vvp_time64_t delay = (hig << 32) | ਨੀਵਾਂ;

      ਜੇ (delay == 0) ਸਮਾਂ_ਸਾਰਣੀ_inactive(thr);
      ਨਹੀਂਤਾਂ ਸਮਾਂ_ਸਾਰਣੀ_vthread(thr, delay);
      ਵਾਪਸ ਝੂਠ;
}

ਤਾਰਕਿਕ ਦਾ_DELAYX(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_time64_t delay;

      ਪ੍ਰਤਿਜ੍ਞਾ(cp->ਸੰਖਿਆ < vthread_s::ਸ਼ਬਦ_ਗਿਣਤੀ);
      delay = thr->ਸ਼ਬਦ[cp->ਸੰਖਿਆ].w_uint;
      ਜੇ (delay == 0) ਸਮਾਂ_ਸਾਰਣੀ_inactive(thr);
      ਨਹੀਂਤਾਂ ਸਮਾਂ_ਸਾਰਣੀ_vthread(thr, delay);
      ਵਾਪਸ ਝੂਠ;
}

ਤਾਰਕਿਕ ਦਾ_DELETE_ELEM(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_net_t*net = cp->net;

      int64_t idx_val = thr->ਸ਼ਬਦ[3].w_int;
      ਜੇ (thr->flags[4] == BIT4_1) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with undefined index."
	         << ਸਤਰ_ਅੰਤ;
	    ਵਾਪਸ ਸੱਚ;
      }
      ਜੇ (idx_val < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with negative index."
	         << ਸਤਰ_ਅੰਤ;
	    ਵਾਪਸ ਸੱਚ;
      }
      size_t idx = idx_val;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      ਪ੍ਰਤਿਜ੍ਞਾ(obj);

      vvp_ਕਤਾਰ*ਕਤਾਰ = obj->get_object().peek<vvp_ਕਤਾਰ>();
      ਜੇ (ਕਤਾਰ == 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping delete(" << idx
	         << ") on empty queue." << ਸਤਰ_ਅੰਤ;
      } ਨਹੀਂਤਾਂ {
	    size_t size = ਕਤਾਰ->get_ਮਾਪ();
	    ਜੇ (idx >= size) {
		  cerr << thr->get_fileline()
		       << "Warning: skipping out of range delete(" << idx
		       << ") on queue of size " << size << "." << ਸਤਰ_ਅੰਤ;
	    } ਨਹੀਂਤਾਂ {
		  ਕਤਾਰ->erase(idx);
	    }
      }

      ਵਾਪਸ ਸੱਚ;
}

/* %delete/obj <label>
 *
 * This operator works by assigning a nil to the target object. This
 * causes any value that might be there to be garbage collected, thus
 * deleting the object.
 */
ਤਾਰਕਿਕ ਦਾ_DELETE_OBJ(vthread_t thr, vvp_ਕੋਡ_t cp)
{
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_send_object(ptr, vvp_object_t(), thr->wt_context);

      ਵਾਪਸ ਸੱਚ;
}

/* %reverse/obj <label>
 * Reverse element order of the queue or dynamic array object at this net.
 */
ਤਾਰਕਿਕ ਦਾ_ਉਲਟ_OBJ(vthread_t /*thr*/, vvp_ਕੋਡ_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      ਪ੍ਰਤਿਜ੍ਞਾ(obj);

      vvp_object_t stor = obj->get_object();
      ਜੇ (stor.test_nil()) ਵਾਪਸ ਸੱਚ;

	/* Match other queue ops: peek queues before vvp_darray so concrete
	   queue types dispatch reverse_elems() correctly. */
      ਜੇ (vvp_ਕਤਾਰ* ਕਤਾਰ = stor.peek<vvp_ਕਤਾਰ>()) {
	    ਕਤਾਰ->ਉਲਟ_elems();
	    ਵਾਪਸ ਸੱਚ;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      ਪ੍ਰਤਿਜ੍ਞਾ(darray);
      darray->ਉਲਟ_elems();

      ਵਾਪਸ ਸੱਚ;
}

/* %reverse/prop/obj <property-index>
 * Reverse queue/darray property storage inside the class object on stack top.
 */
ਤਾਰਕਿਕ ਦਾ_ਉਲਟ_PROP_OBJ(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      size_t pid = cp->ਸੰਖਿਆ;

      vvp_object_t& ਸਿਖਰ = thr->peek_object();
      vvp_cobject*cobj = ਸਿਖਰ.peek<vvp_cobject>();
      ਪ੍ਰਤਿਜ੍ਞਾ(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      ਜੇ (arr_obj.test_nil()) ਵਾਪਸ ਸੱਚ;

      ਜੇ (vvp_ਕਤਾਰ* ਕਤਾਰ = arr_obj.peek<vvp_ਕਤਾਰ>()) {
	    ਕਤਾਰ->ਉਲਟ_elems();
	    ਵਾਪਸ ਸੱਚ;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      ਪ੍ਰਤਿਜ੍ਞਾ(darray);
      darray->ਉਲਟ_elems();

      ਵਾਪਸ ਸੱਚ;
}

/* %sort/obj — ascending sort */
ਤਾਰਕਿਕ ਦਾ_ਛਾਂਟੋ_OBJ(vthread_t /*thr*/, vvp_ਕੋਡ_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      ਪ੍ਰਤਿਜ੍ਞਾ(obj);

      vvp_object_t stor = obj->get_object();
      ਜੇ (stor.test_nil()) ਵਾਪਸ ਸੱਚ;

      ਜੇ (vvp_ਕਤਾਰ* ਕਤਾਰ = stor.peek<vvp_ਕਤਾਰ>()) {
	    ਕਤਾਰ->ਛਾਂਟੋ_elems(ਸੱਚ);
	    ਵਾਪਸ ਸੱਚ;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      ਪ੍ਰਤਿਜ੍ਞਾ(darray);
      darray->ਛਾਂਟੋ_elems(ਸੱਚ);

      ਵਾਪਸ ਸੱਚ;
}

/* %sort/prop/obj */
ਤਾਰਕਿਕ ਦਾ_ਛਾਂਟੋ_PROP_OBJ(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      size_t pid = cp->ਸੰਖਿਆ;

      vvp_object_t& ਸਿਖਰ = thr->peek_object();
      vvp_cobject*cobj = ਸਿਖਰ.peek<vvp_cobject>();
      ਪ੍ਰਤਿਜ੍ਞਾ(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      ਜੇ (arr_obj.test_nil()) ਵਾਪਸ ਸੱਚ;

      ਜੇ (vvp_ਕਤਾਰ* ਕਤਾਰ = arr_obj.peek<vvp_ਕਤਾਰ>()) {
	    ਕਤਾਰ->ਛਾਂਟੋ_elems(ਸੱਚ);
	    ਵਾਪਸ ਸੱਚ;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      ਪ੍ਰਤਿਜ੍ਞਾ(darray);
      darray->ਛਾਂਟੋ_elems(ਸੱਚ);

      ਵਾਪਸ ਸੱਚ;
}

/* %rsort/obj — descending sort */
ਤਾਰਕਿਕ ਦਾ_RSORT_OBJ(vthread_t /*thr*/, vvp_ਕੋਡ_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      ਪ੍ਰਤਿਜ੍ਞਾ(obj);

      vvp_object_t stor = obj->get_object();
      ਜੇ (stor.test_nil()) ਵਾਪਸ ਸੱਚ;

      ਜੇ (vvp_ਕਤਾਰ* ਕਤਾਰ = stor.peek<vvp_ਕਤਾਰ>()) {
	    ਕਤਾਰ->ਛਾਂਟੋ_elems(ਝੂਠ);
	    ਵਾਪਸ ਸੱਚ;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      ਪ੍ਰਤਿਜ੍ਞਾ(darray);
      darray->ਛਾਂਟੋ_elems(ਝੂਠ);

      ਵਾਪਸ ਸੱਚ;
}

/* %rsort/prop/obj */
ਤਾਰਕਿਕ ਦਾ_RSORT_PROP_OBJ(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      size_t pid = cp->ਸੰਖਿਆ;

      vvp_object_t& ਸਿਖਰ = thr->peek_object();
      vvp_cobject*cobj = ਸਿਖਰ.peek<vvp_cobject>();
      ਪ੍ਰਤਿਜ੍ਞਾ(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      ਜੇ (arr_obj.test_nil()) ਵਾਪਸ ਸੱਚ;

      ਜੇ (vvp_ਕਤਾਰ* ਕਤਾਰ = arr_obj.peek<vvp_ਕਤਾਰ>()) {
	    ਕਤਾਰ->ਛਾਂਟੋ_elems(ਝੂਠ);
	    ਵਾਪਸ ਸੱਚ;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      ਪ੍ਰਤਿਜ੍ਞਾ(darray);
      darray->ਛਾਂਟੋ_elems(ਝੂਠ);

      ਵਾਪਸ ਸੱਚ;
}

/* %shuffle/obj */
ਤਾਰਕਿਕ ਦਾ_SHUFFLE_OBJ(vthread_t /*thr*/, vvp_ਕੋਡ_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      ਪ੍ਰਤਿਜ੍ਞਾ(obj);

      vvp_object_t stor = obj->get_object();
      ਜੇ (stor.test_nil()) ਵਾਪਸ ਸੱਚ;

      ਜੇ (vvp_ਕਤਾਰ* ਕਤਾਰ = stor.peek<vvp_ਕਤਾਰ>()) {
	    ਕਤਾਰ->shuffle_elems();
	    ਵਾਪਸ ਸੱਚ;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      ਪ੍ਰਤਿਜ੍ਞਾ(darray);
      darray->shuffle_elems();

      ਵਾਪਸ ਸੱਚ;
}

/* %shuffle/prop/obj */
ਤਾਰਕਿਕ ਦਾ_SHUFFLE_PROP_OBJ(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      size_t pid = cp->ਸੰਖਿਆ;

      vvp_object_t& ਸਿਖਰ = thr->peek_object();
      vvp_cobject*cobj = ਸਿਖਰ.peek<vvp_cobject>();
      ਪ੍ਰਤਿਜ੍ਞਾ(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      ਜੇ (arr_obj.test_nil()) ਵਾਪਸ ਸੱਚ;

      ਜੇ (vvp_ਕਤਾਰ* ਕਤਾਰ = arr_obj.peek<vvp_ਕਤਾਰ>()) {
	    ਕਤਾਰ->shuffle_elems();
	    ਵਾਪਸ ਸੱਚ;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      ਪ੍ਰਤਿਜ੍ਞਾ(darray);
      darray->shuffle_elems();

      ਵਾਪਸ ਸੱਚ;
}

/* %delete/tail <label>, idx
 *
 * Remove all elements after the one specified.
 */
ਤਾਰਕਿਕ ਦਾ_DELETE_ਪੂਛ(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      ਪ੍ਰਤਿਜ੍ਞਾ(obj);

      vvp_ਕਤਾਰ*ਕਤਾਰ = obj->get_object().peek<vvp_ਕਤਾਰ>();
      ਪ੍ਰਤਿਜ੍ਞਾ(ਕਤਾਰ);

      ਬਿਨਾਨਿਸ਼ਾਨੀ idx = thr->ਸ਼ਬਦ[cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0]].w_int;
      ਕਤਾਰ->erase_ਪੂਛ(idx);

      ਵਾਪਸ ਸੱਚ;
}

ਸਥਿਰ ਤਾਰਕਿਕ do_disable(vthread_t thr, vthread_t ਮੇਲ)
{
      ਤਾਰਕਿਕ flag = ਝੂਠ;

	/* Pull the target thread out of its scope if needed. */
      thr->ਮਾਪਾ_scope->ਧਾਗੇ.erase(thr);

	/* Turn the thread off by setting is program counter to
	   zero and setting an OFF bit. */
      thr->pc = codespace_null();
      thr->i_was_disabled = 1;
      thr->i_have_ended = 1;

	/* Turn off all the children of the thread. Simulate a %join
	   for as many times as needed to clear the results of all the
	   %forks that this thread has done. */
      ਜਦੋਂ (! thr->children.empty()) {

	    vthread_t tmp = *(thr->children.begin());
	    ਪ੍ਰਤਿਜ੍ਞਾ(tmp);
	    ਪ੍ਰਤਿਜ੍ਞਾ(tmp->ਮਾਪਾ == thr);
	    thr->i_am_joining = 0;
	    ਜੇ (do_disable(tmp, ਮੇਲ))
		  flag = ਸੱਚ;

	    vthread_reap(tmp);
      }

      vthread_t ਮਾਪਾ = thr->ਮਾਪਾ;
      ਜੇ (ਮਾਪਾ && ਮਾਪਾ->i_am_joining) {
	      // If a parent is waiting in a %join, wake it up. Note
	      // that it is possible to be waiting in a %join yet
	      // already scheduled if multiple child threads are
	      // ending. So check if the thread is already scheduled
	      // before scheduling it again.
	    ਮਾਪਾ->i_am_joining = 0;
	    ਜੇ (! ਮਾਪਾ->i_have_ended)
		  ਸਮਾਂ_ਸਾਰਣੀ_vthread(ਮਾਪਾ, 0, ਸੱਚ);

	    do_join(ਮਾਪਾ, thr);

      } ਨਹੀਂਤਾਂ ਜੇ (ਮਾਪਾ) {
	      /* If the parent is yet to %join me, let its %join
		 do the reaping. */
	      //assert(tmp->is_scheduled == 0);

      } ਨਹੀਂਤਾਂ {
	      /* No parent at all. Goodbye. */
	    vthread_reap(thr);
      }

      ਵਾਪਸ flag || (thr == ਮੇਲ);
}

/*
 * Implement the %disable instruction by scanning the target scope for
 * all the target threads. Kill the target threads and wake up a
 * parent that is attempting a %join.
 */
ਤਾਰਕਿਕ ਦਾ_DISABLE(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      __vpiScope*scope = static_cast<__vpiScope*>(cp->handle);

      ਤਾਰਕਿਕ disabled_myself_flag = ਝੂਠ;

      ਜਦੋਂ (! scope->ਧਾਗੇ.empty()) {
	    ਸੈੱਟ<vthread_t>::iterator cur = scope->ਧਾਗੇ.begin();

	    ਜੇ (do_disable(*cur, thr))
		  disabled_myself_flag = ਸੱਚ;
      }

      ਵਾਪਸ ! disabled_myself_flag;
}

/*
 * Similar to `of_DISABLE`. But will only disable a single thread of the
 * specified scope. The disabled thread will be the thread closest to the
 * current thread in thread hierarchy. This can either be the current thread,
 * either the thread itself or one of its parents.
 * This is used for SystemVerilog flow control instructions like `return`,
 * `continue` and `break`.
 */

ਤਾਰਕਿਕ ਦਾ_DISABLE_FLOW(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਅਚਲ __vpiScope*scope = static_cast<__vpiScope*>(cp->handle);
      vthread_t cur = thr;

      ਜਦੋਂ (cur && cur->ਮਾਪਾ_scope != scope)
	    cur = cur->ਮਾਪਾ;

      ਪ੍ਰਤਿਜ੍ਞਾ(cur);
      ਜੇ (cur)
	    ਵਾਪਸ !do_disable(cur, thr);

      ਵਾਪਸ ਝੂਠ;
}

/*
 * Implement the %disable/fork (SystemVerilog) instruction by disabling
 * all the detached children of the given thread.
 */
ਤਾਰਕਿਕ ਦਾ_DISABLE_FORK(vthread_t thr, vvp_ਕੋਡ_t)
{
	/* If a %disable/fork is being executed then the parent thread
	 * cannot be waiting in a join. */
      ਪ੍ਰਤਿਜ੍ਞਾ(! thr->i_am_joining);

	/* There should be no active children to disable. */
      ਪ੍ਰਤਿਜ੍ਞਾ(thr->children.empty());

	/* Disable any detached children. */
      ਜਦੋਂ (! thr->detached_children.empty()) {
	    vthread_t ਬੱਚਾ = *(thr->detached_children.begin());
	    ਪ੍ਰਤਿਜ੍ਞਾ(ਬੱਚਾ);
	    ਪ੍ਰਤਿਜ੍ਞਾ(ਬੱਚਾ->ਮਾਪਾ == thr);
	      /* Disabling the children can never match the parent thread. */
	    ਤਾਰਕਿਕ res = do_disable(ਬੱਚਾ, thr);
	    ਪ੍ਰਤਿਜ੍ਞਾ(! res);
	    vthread_reap(ਬੱਚਾ);
      }

      ਵਾਪਸ ਸੱਚ;
}

/*
 * This function divides a 2-word number {high, a} by a 1-word
 * number. Assume that high < b.
 */
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ divide2words(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ a, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ b,
				  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਉੱਚਾ)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਤੀਜਾ = 0;
      ਜਦੋਂ (ਉੱਚਾ > 0) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tmp_ਨਤੀਜਾ = ULONG_MAX / b;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ remain = ULONG_MAX % b;

	    remain += 1;
	    ਜੇ (remain >= b) {
		  remain -= b;
		  tmp_ਨਤੀਜਾ += 1;
	    }

	      // Now 0x1_0...0 = b*tmp_result + remain
	      // high*0x1_0...0 = high*(b*tmp_result + remain)
	      // high*0x1_0...0 = high*b*tmp_result + high*remain

	      // We know that high*0x1_0...0 >= high*b*tmp_result, and
	      // we know that high*0x1_0...0 > high*remain. Use
	      // high*remain as the remainder for another iteration,
	      // and add tmp_result*high into the current estimate of
	      // the result.
	    ਨਤੀਜਾ += tmp_ਨਤੀਜਾ * ਉੱਚਾ;

	      // The new iteration starts with high*remain + a.
	    remain = multiply_with_ਹਾਸਲ(ਉੱਚਾ, remain, ਉੱਚਾ);
	    a += remain;
            ਜੇ(a < remain)
              ਉੱਚਾ += 1;

	      // Now result*b + {high,a} == the input {high,a}. It is
	      // possible that the new high >= 1. If so, it will
	      // certainly be less than high from the previous
	      // iteration. Do another iteration and it will shrink,
	      // eventually to 0.
      }

	// high is now 0, so a is the remaining remainder, so we can
	// finish off the integer divide with a simple a/b.

      ਵਾਪਸ ਨਤੀਜਾ + a/b;
}

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ* divide_ਬਿੱਟ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ*ap, ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ*bp, ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
{
	// Do all our work a cpu-word at a time. The "words" variable
	// is the number of words of the wid.
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ = (wid+CPU_ਸ਼ਬਦ_ਬਿੱਟ-1) / CPU_ਸ਼ਬਦ_ਬਿੱਟ;

      ਬਿਨਾਨਿਸ਼ਾਨੀ btop = ਸ਼ਬਦ-1;
      ਜਦੋਂ (btop > 0 && bp[btop] == 0)
	    btop -= 1;

	// Detect divide by 0, and exit.
      ਜੇ (btop==0 && bp[0]==0)
	    ਵਾਪਸ 0;

	// The result array will eventually accumulate the result. The
	// diff array is a difference that we use in the intermediate.
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ*diff  = ਨਵਾਂ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ[ਸ਼ਬਦ];
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ*ਨਤੀਜਾ= ਨਵਾਂ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ[ਸ਼ਬਦ];
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < ਸ਼ਬਦ ; idx += 1)
	    ਨਤੀਜਾ[idx] = 0;

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ cur = ਸ਼ਬਦ-btop ; cur > 0 ; cur -= 1) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ cur_ptr = cur-1;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ cur_res;
	    ਜੇ (ap[cur_ptr+btop] >= bp[btop]) {
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਉੱਚਾ = 0;
		  ਜੇ (cur_ptr+btop+1 < ਸ਼ਬਦ)
			ਉੱਚਾ = ap[cur_ptr+btop+1];
		  cur_res = divide2words(ap[cur_ptr+btop], bp[btop], ਉੱਚਾ);

	    } ਨਹੀਂਤਾਂ ਜੇ (cur_ptr+btop+1 >= ਸ਼ਬਦ) {
		  ਜਾਰੀ;

	    } ਨਹੀਂਤਾਂ ਜੇ (ap[cur_ptr+btop+1] == 0) {
		  ਜਾਰੀ;

	    } ਨਹੀਂਤਾਂ {
		  cur_res = divide2words(ap[cur_ptr+btop], bp[btop],
					 ap[cur_ptr+btop+1]);
	    }

	      // cur_res is a guesstimate of the result this far. It
	      // may be 1 too big. (But it will also be >0) Try it,
	      // and if the difference comes out negative, then adjust.

	      // diff = (bp * cur_res)  << cur_ptr;
	    multiply_array_imm(diff+cur_ptr, bp, ਸ਼ਬਦ-cur_ptr, cur_res);
	      // ap -= diff
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਹਾਸਲ = 1;
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = cur_ptr ; idx < ਸ਼ਬਦ ; idx += 1)
		  ap[idx] = ਜੋੜੋ_with_ਹਾਸਲ(ap[idx], ~diff[idx], ਹਾਸਲ);

	      // ap has the diff subtracted out of it. If cur_res was
	      // too large, then ap will turn negative. (We easily
	      // tell that ap turned negative by looking at
	      // carry&1. If it is 0, then it is *negative*.) In that
	      // case, we know that cur_res was too large by 1. Correct by
	      // adding 1b back in and reducing cur_res.
	    ਜੇ ((ਹਾਸਲ&1) == 0) {
		    // Keep adding b back in until the remainder
		    // becomes positive again.
		  ਕਰੋ {
			cur_res -= 1;
			ਹਾਸਲ = 0;
			ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = cur_ptr ; idx < ਸ਼ਬਦ ; idx += 1)
			      ap[idx] = ਜੋੜੋ_with_ਹਾਸਲ(ap[idx], bp[idx-cur_ptr], ਹਾਸਲ);
		  } ਜਦੋਂ (ਹਾਸਲ == 0);
	    }

	    ਨਤੀਜਾ[cur_ptr] = cur_res;
      }

	// Now ap contains the remainder and result contains the
	// desired result. We should find that:
	//  input-a = bp * result + ap;

      ਮਿਟਾਓ[]diff;
      ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * %div
 */
ਤਾਰਕਿਕ ਦਾ_DIV(vthread_t thr, vvp_ਕੋਡ_t)
{
      vvp_vector4_t valb = thr->ਕੱਢੋ_vec4();
      vvp_vector4_t vala = thr->ਕੱਢੋ_vec4();

      ਪ੍ਰਤਿਜ੍ਞਾ(vala.size()== valb.size());
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = vala.size();

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ*ap = vala.subarray(0, wid);
      ਜੇ (ap == 0) {
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->ਧੱਕੋ_vec4(tmp);
	    ਵਾਪਸ ਸੱਚ;
      }

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ*bp = valb.subarray(0, wid);
      ਜੇ (bp == 0) {
	    ਮਿਟਾਓ[]ap;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->ਧੱਕੋ_vec4(tmp);
	    ਵਾਪਸ ਸੱਚ;
      }

	// If the value fits in a single CPU word, then do it the easy way.
      ਜੇ (wid <= CPU_ਸ਼ਬਦ_ਬਿੱਟ) {
	    ਜੇ (bp[0] == 0) {
		  vvp_vector4_t tmp(wid, BIT4_X);
		  thr->ਧੱਕੋ_vec4(tmp);
	    } ਨਹੀਂਤਾਂ {
		  ap[0] /= bp[0];
		  vala.setarray(0, wid, ap);
		  thr->ਧੱਕੋ_vec4(vala);
	    }
	    ਮਿਟਾਓ[]ap;
	    ਮਿਟਾਓ[]bp;
	    ਵਾਪਸ ਸੱਚ;
      }

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ*ਨਤੀਜਾ = divide_ਬਿੱਟ(ap, bp, wid);
      ਜੇ (ਨਤੀਜਾ == 0) {
	    ਮਿਟਾਓ[]ap;
	    ਮਿਟਾਓ[]bp;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->ਧੱਕੋ_vec4(tmp);
	    ਵਾਪਸ ਸੱਚ;
      }

	// Now ap contains the remainder and result contains the
	// desired result. We should find that:
	//  input-a = bp * result + ap;

      vala.setarray(0, wid, ਨਤੀਜਾ);
      thr->ਧੱਕੋ_vec4(vala);
      ਮਿਟਾਓ[]ap;
      ਮਿਟਾਓ[]bp;
      ਮਿਟਾਓ[]ਨਤੀਜਾ;

      ਵਾਪਸ ਸੱਚ;
}


ਸਥਿਰ ਖਾਲੀ negate_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ*val, ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਹਾਸਲ = 1;
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < ਸ਼ਬਦ ; idx += 1)
	    val[idx] = ਜੋੜੋ_with_ਹਾਸਲ(0, ~val[idx], ਹਾਸਲ);
}

/*
 * %div/s
 */
ਤਾਰਕਿਕ ਦਾ_DIV_S(vthread_t thr, vvp_ਕੋਡ_t)
{
      vvp_vector4_t valb = thr->ਕੱਢੋ_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      ਪ੍ਰਤਿਜ੍ਞਾ(vala.size()== valb.size());
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = vala.size();
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ = (wid + CPU_ਸ਼ਬਦ_ਬਿੱਟ - 1) / CPU_ਸ਼ਬਦ_ਬਿੱਟ;

	// Get the values, left in right, in binary form. If there is
	// a problem with either (caused by an X or Z bit) then we
	// know right away that the entire result is X.
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ*ap = vala.subarray(0, wid);
      ਜੇ (ap == 0) {
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    ਵਾਪਸ ਸੱਚ;
      }

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ*bp = valb.subarray(0, wid);
      ਜੇ (bp == 0) {
	    ਮਿਟਾਓ[]ap;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    ਵਾਪਸ ਸੱਚ;
      }

	// Sign extend the bits in the array to fill out the array.
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sign_ਨਕਾਬ = 0;
      ਜੇ (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sign_ਬਿੱਟ = (ਸ਼ਬਦ*CPU_ਸ਼ਬਦ_ਬਿੱਟ) - wid) {
	    sign_ਨਕਾਬ = -1UL << (CPU_ਸ਼ਬਦ_ਬਿੱਟ-sign_ਬਿੱਟ);
	    ਜੇ (ap[ਸ਼ਬਦ-1] & (sign_ਨਕਾਬ>>1))
		  ap[ਸ਼ਬਦ-1] |= sign_ਨਕਾਬ;
	    ਜੇ (bp[ਸ਼ਬਦ-1] & (sign_ਨਕਾਬ>>1))
		  bp[ਸ਼ਬਦ-1] |= sign_ਨਕਾਬ;
      }

	// If the value fits in a single word, then use the native divide.
      ਜੇ (wid <= CPU_ਸ਼ਬਦ_ਬਿੱਟ) {
	    ਜੇ (bp[0] == 0) {
		  vvp_vector4_t tmp(wid, BIT4_X);
		  vala = tmp;
	    } ਨਹੀਂਤਾਂ ਜੇ (((ਲੰਮਾ)ap[0] == LONG_MIN) && ((ਲੰਮਾ)bp[0] == -1)) {
		  vvp_vector4_t tmp(wid, BIT4_0);
		  tmp.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(wid-1, BIT4_1);
		  vala = tmp;
	    } ਨਹੀਂਤਾਂ {
		  ਲੰਮਾ tmpa = (ਲੰਮਾ) ap[0];
		  ਲੰਮਾ tmpb = (ਲੰਮਾ) bp[0];
		  ਲੰਮਾ res = tmpa / tmpb;
		  ap[0] = ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)res) & ~sign_ਨਕਾਬ;
		  vala.setarray(0, wid, ap);
	    }
	    ਮਿਟਾਓ[]ap;
	    ਮਿਟਾਓ[]bp;
	    ਵਾਪਸ ਸੱਚ;
      }

	// We need to the actual division to positive integers. Make
	// them positive here, and remember the negations.
      ਤਾਰਕਿਕ negate_flag = ਝੂਠ;
      ਜੇ ( ((ਲੰਮਾ) ap[ਸ਼ਬਦ-1]) < 0 ) {
	    negate_flag = ਸੱਚ;
	    negate_ਸ਼ਬਦ(ap, ਸ਼ਬਦ);
      }
      ਜੇ ( ((ਲੰਮਾ) bp[ਸ਼ਬਦ-1]) < 0 ) {
	    negate_flag ^= ਸੱਚ;
	    negate_ਸ਼ਬਦ(bp, ਸ਼ਬਦ);
      }

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ*ਨਤੀਜਾ = divide_ਬਿੱਟ(ap, bp, wid);
      ਜੇ (ਨਤੀਜਾ == 0) {
	    ਮਿਟਾਓ[]ap;
	    ਮਿਟਾਓ[]bp;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    ਵਾਪਸ ਸੱਚ;
      }

      ਜੇ (negate_flag) {
	    negate_ਸ਼ਬਦ(ਨਤੀਜਾ, ਸ਼ਬਦ);
      }

      ਨਤੀਜਾ[ਸ਼ਬਦ-1] &= ~sign_ਨਕਾਬ;

      vala.setarray(0, wid, ਨਤੀਜਾ);
      ਮਿਟਾਓ[]ap;
      ਮਿਟਾਓ[]bp;
      ਮਿਟਾਓ[]ਨਤੀਜਾ;
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_DIV_WR(vthread_t thr, vvp_ਕੋਡ_t)
{
      ਦੁੱਗਣਾ r = thr->ਕੱਢੋ_real();
      ਦੁੱਗਣਾ l = thr->ਕੱਢੋ_real();
      thr->ਧੱਕੋ_real(l / r);

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %dup/obj
 * %dup/real
 * %dup/vec4
 *
 * Push a duplicate of the object on the appropriate stack.
 */
ਤਾਰਕਿਕ ਦਾ_DUP_OBJ(vthread_t thr, vvp_ਕੋਡ_t)
{
      vvp_object_t src = thr->peek_object();

        // If it is null push a new null object
      ਜੇ (src.test_nil())
	    thr->ਧੱਕੋ_object(vvp_object_t());
      ਨਹੀਂਤਾਂ
	    thr->ਧੱਕੋ_object(src.duplicate());

      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_DUP_REAL(vthread_t thr, vvp_ਕੋਡ_t)
{
      thr->ਧੱਕੋ_real(thr->peek_real(0));
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_DUP_VEC4(vthread_t thr, vvp_ਕੋਡ_t)
{
      thr->ਧੱਕੋ_vec4(thr->peek_vec4(0));
      ਵਾਪਸ ਸੱਚ;
}

/*
 * This terminates the current thread. If there is a parent who is
 * waiting for me to die, then I schedule it. At any rate, I mark
 * myself as a zombie by setting my pc to 0.
 */
ਤਾਰਕਿਕ ਦਾ_ਸਿਰਾ(vthread_t thr, vvp_ਕੋਡ_t)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(! thr->ਉਡੀਕ_for_event);
      thr->i_have_ended = 1;
      thr->pc = codespace_null();

	/* Fully detach any detached children. */
      ਜਦੋਂ (! thr->detached_children.empty()) {
	    vthread_t ਬੱਚਾ = *(thr->detached_children.begin());
	    ਪ੍ਰਤਿਜ੍ਞਾ(ਬੱਚਾ);
	    ਪ੍ਰਤਿਜ੍ਞਾ(ਬੱਚਾ->ਮਾਪਾ == thr);
	    ਪ੍ਰਤਿਜ੍ਞਾ(ਬੱਚਾ->i_am_detached);
	    ਬੱਚਾ->ਮਾਪਾ = 0;
	    ਬੱਚਾ->i_am_detached = 0;
	    thr->detached_children.erase(thr->detached_children.begin());
      }

	/* It is an error to still have active children running at this
	 * point in time. They should have all been detached or joined. */
      ਪ੍ਰਤਿਜ੍ਞਾ(thr->children.empty());

	/* If I have a parent who is waiting for me, then mark that I
	   have ended, and schedule that parent. Also, finish the
	   %join for the parent. */
      ਜੇ (!thr->i_am_detached && thr->ਮਾਪਾ && thr->ਮਾਪਾ->i_am_joining) {
	    vthread_t tmp = thr->ਮਾਪਾ;
	    ਪ੍ਰਤਿਜ੍ਞਾ(! thr->i_am_detached);

	    tmp->i_am_joining = 0;
	    ਸਮਾਂ_ਸਾਰਣੀ_vthread(tmp, 0, ਸੱਚ);
	    do_join(tmp, thr);
	    ਵਾਪਸ ਝੂਠ;
      }

	/* If this thread is not fully detached then remove it from the
	 * parents detached_children set and reap it. */
      ਜੇ (thr->i_am_detached) {
	    vthread_t tmp = thr->ਮਾਪਾ;
	    ਪ੍ਰਤਿਜ੍ਞਾ(tmp);
	    size_t res = tmp->detached_children.erase(thr);
	    ਪ੍ਰਤਿਜ੍ਞਾ(res == 1);
	      /* If the parent is waiting for the detached children to
	       * finish then the last detached child needs to tell the
	       * parent to wake up when it is finished. */
	    ਜੇ (tmp->i_am_ਉਡੀਕ && tmp->detached_children.empty()) {
		  tmp->i_am_ਉਡੀਕ = 0;
		  ਸਮਾਂ_ਸਾਰਣੀ_vthread(tmp, 0, ਸੱਚ);
	    }
	      /* Fully detach this thread so it will be reaped below. */
	    thr->i_am_detached = 0;
	    thr->ਮਾਪਾ = 0;
      }

	/* If I have no parent, then no one can %join me and there is
	 * no reason to stick around. This can happen, for example if
	 * I am an ``initial'' thread. */
      ਜੇ (thr->ਮਾਪਾ == 0) {
	    vthread_reap(thr);
	    ਵਾਪਸ ਝੂਠ;
      }

	/* If I make it this far, then I have a parent who may wish
	   to %join me. Remain a zombie so that it can. */

      ਵਾਪਸ ਝੂਠ;
}

/*
 * %event <var-label>
 */
ਤਾਰਕਿਕ ਦਾ_EVENT(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_vector4_t tmp (1, BIT4_X);
      vvp_send_vec4(ptr, tmp, thr->wt_context);
      ਵਾਪਸ ਸੱਚ;
}

/*
 * %event/nb <var-label>, <delay>
 */
ਤਾਰਕਿਕ ਦਾ_EVENT_NB(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_time64_t delay;

      delay = thr->ਸ਼ਬਦ[cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0]].w_uint;
      ਸਮਾਂ_ਸਾਰਣੀ_propagate_event(cp->net, delay);
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_EVCTL(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      thr->ecount = thr->ਸ਼ਬਦ[cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0]].w_uint;
      ਵਾਪਸ ਸੱਚ;
}
ਤਾਰਕਿਕ ਦਾ_EVCTLC(vthread_t thr, vvp_ਕੋਡ_t)
{
      thr->event = 0;
      thr->ecount = 0;
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_EVCTLI(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      thr->ecount = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_EVCTLS(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      int64_t val = thr->ਸ਼ਬਦ[cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0]].w_int;
      ਜੇ (val < 0) val = 0;
      thr->ecount = val;
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_FLAG_GET_VEC4(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਪੂਰਨ flag = cp->ਸੰਖਿਆ;
      ਪ੍ਰਤਿਜ੍ਞਾ(flag < vthread_s::FLAGS_ਗਿਣਤੀ);

      vvp_vector4_t val (1, thr->flags[flag]);
      thr->ਧੱਕੋ_vec4(val);

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %flag_inv <flag1>
 */
ਤਾਰਕਿਕ ਦਾ_FLAG_INV(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਪੂਰਨ flag1 = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];

      thr->flags[flag1] = ~ thr->flags[flag1];
      ਵਾਪਸ ਸੱਚ;
}

/*
 * %flag_mov <flag1>, <flag2>
 */
ਤਾਰਕਿਕ ਦਾ_FLAG_MOV(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਪੂਰਨ flag1 = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      ਪੂਰਨ flag2 = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[1];

      thr->flags[flag1] = thr->flags[flag2];
      ਵਾਪਸ ਸੱਚ;
}

/*
 * %flag_or <flag1>, <flag2>
 */
ਤਾਰਕਿਕ ਦਾ_FLAG_OR(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਪੂਰਨ flag1 = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      ਪੂਰਨ flag2 = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[1];

      thr->flags[flag1] = thr->flags[flag1] | thr->flags[flag2];
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_FLAG_ਸੈੱਟ_IMM(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਪੂਰਨ flag = cp->ਸੰਖਿਆ;
      ਪੂਰਨ vali = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];

      ਪ੍ਰਤਿਜ੍ਞਾ(flag < vthread_s::FLAGS_ਗਿਣਤੀ);
      ਪ੍ਰਤਿਜ੍ਞਾ(vali >= 0 && vali < 4);

      ਸਥਿਰ ਅਚਲ vvp_bit4_t map_ਦੋਆਧਾਰੀ_ਅੰਕ[4] = {BIT4_0, BIT4_1, BIT4_Z, BIT4_X};
      thr->flags[flag] = map_ਦੋਆਧਾਰੀ_ਅੰਕ[vali];
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_FLAG_ਸੈੱਟ_VEC4(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਪੂਰਨ flag = cp->ਸੰਖਿਆ;
      ਪ੍ਰਤਿਜ੍ਞਾ(flag < vthread_s::FLAGS_ਗਿਣਤੀ);

      ਅਚਲ vvp_vector4_t&val = thr->peek_vec4();
      thr->flags[flag] = val.value(0);
      thr->ਕੱਢੋ_vec4(1);

      ਵਾਪਸ ਸੱਚ;
}

/*
 * the %force/link instruction connects a source node to a
 * destination node. The destination node must be a signal, as it is
 * marked with the source of the force so that it may later be
 * unlinked without specifically knowing the source that this
 * instruction used.
 */
ਤਾਰਕਿਕ ਦਾ_FORCE_LINK(vthread_t, vvp_ਕੋਡ_t cp)
{
      vvp_net_t*dst = cp->net;
      vvp_net_t*src = cp->net2;

      ਪ੍ਰਤਿਜ੍ਞਾ(dst->fil);
      dst->fil->force_link(dst, src);

      ਵਾਪਸ ਸੱਚ;
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
ਤਾਰਕਿਕ ਦਾ_FORCE_VEC4(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_vector4_t value = thr->ਕੱਢੋ_vec4();

	/* Send the force value to the filter on the node. */

      ਪ੍ਰਤਿਜ੍ਞਾ(net->fil);
      ਜੇ (value.size() != net->fil->filter_ਮਾਪ())
	    value = coerce_ਮੰਜ਼ਿਲ_ਚੌੜਾਈ(value, net->fil->filter_ਮਾਪ());

      net->force_vec4(value, vvp_vector2_t(vvp_vector2_t::FILL1, net->fil->filter_ਮਾਪ()));

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %force/vec4/off <net>, <off>
 */
ਤਾਰਕਿਕ ਦਾ_FORCE_VEC4_OFF(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_net_t*net = cp->net;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ_idx = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      ਲੰਮਾ ਅਧਾਰ = thr->ਸ਼ਬਦ[ਅਧਾਰ_idx].w_int;
      vvp_vector4_t value = thr->ਕੱਢੋ_vec4();
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = value.size();

      ਪ੍ਰਤਿਜ੍ਞਾ(net->fil);

      ਜੇ (thr->flags[4] == BIT4_1)
	    ਵਾਪਸ ਸੱਚ;

	// This is the width of the target vector.
      ਬਿਨਾਨਿਸ਼ਾਨੀ use_ਮਾਪ = net->fil->filter_ਮਾਪ();

      ਜੇ (ਅਧਾਰ >= (ਲੰਮਾ)use_ਮਾਪ)
	    ਵਾਪਸ ਸੱਚ;
      ਜੇ (ਅਧਾਰ < -(ਲੰਮਾ)use_ਮਾਪ)
	    ਵਾਪਸ ਸੱਚ;

      ਜੇ ((ਅਧਾਰ + wid) > use_ਮਾਪ)
	    wid = use_ਮਾਪ - ਅਧਾਰ;

	// Make a mask of which bits are to be forced, 0 for unforced
	// bits and 1 for forced bits.
      vvp_vector2_t ਨਕਾਬ (vvp_vector2_t::FILL0, use_ਮਾਪ);
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < wid ; idx += 1)
	    ਨਕਾਬ.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(ਅਧਾਰ+idx, 1);

      vvp_vector4_t tmp (use_ਮਾਪ, BIT4_Z);

	// vvp_net_t::force_vec4 propagates all the bits of the
	// forced vector value, regardless of the mask. This
	// ensures the unforced bits retain their current value.
      vvp_signal_ਮੁੱਲ*sig = dynamic_cast<vvp_signal_ਮੁੱਲ*>(net->fil);
      ਪ੍ਰਤਿਜ੍ਞਾ(sig);
      sig->vec4_ਮੁੱਲ(tmp);

      tmp.ਸੈੱਟ_vec(ਅਧਾਰ, value);

      net->force_vec4(tmp, ਨਕਾਬ);
      ਵਾਪਸ ਸੱਚ;
}

/*
 * %force/vec4/off/d <net>, <off>, <del>
 */
ਤਾਰਕਿਕ ਦਾ_FORCE_VEC4_OFF_D(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_net_t*net = cp->net;

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ_idx = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      ਲੰਮਾ ਅਧਾਰ = thr->ਸ਼ਬਦ[ਅਧਾਰ_idx].w_int;

      ਬਿਨਾਨਿਸ਼ਾਨੀ delay_idx = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[1];
      vvp_time64_t delay = thr->ਸ਼ਬਦ[delay_idx].w_uint;

      vvp_vector4_t value = thr->ਕੱਢੋ_vec4();

      ਪ੍ਰਤਿਜ੍ਞਾ(net->fil);

      ਜੇ (thr->flags[4] == BIT4_1)
	    ਵਾਪਸ ਸੱਚ;

	// This is the width of the target vector.
      ਬਿਨਾਨਿਸ਼ਾਨੀ use_ਮਾਪ = net->fil->filter_ਮਾਪ();

      ਜੇ (ਅਧਾਰ >= (ਲੰਮਾ)use_ਮਾਪ)
	    ਵਾਪਸ ਸੱਚ;
      ਜੇ (ਅਧਾਰ < -(ਲੰਮਾ)use_ਮਾਪ)
	    ਵਾਪਸ ਸੱਚ;

      ਸਮਾਂ_ਸਾਰਣੀ_force_ਸਦਿਸ਼(net, ਅਧਾਰ, use_ਮਾਪ, value, delay);
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_FORCE_WR(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_net_t*net  = cp->net;
      ਦੁੱਗਣਾ value = thr->ਕੱਢੋ_real();

      net->force_real(value, vvp_vector2_t(vvp_vector2_t::FILL1, 1));

      ਵਾਪਸ ਸੱਚ;
}

/*
 * The %fork instruction causes a new child to be created and pushed
 * in front of any existing child. This causes the new child to be
 * added to the list of children, and for me to be the parent of the
 * new child.
 */
ਤਾਰਕਿਕ ਦਾ_FORK(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vthread_t ਬੱਚਾ = vthread_new(cp->cptr2, cp->scope);

      ਜੇ (cp->scope->is_automatic()) {
              /* The context allocated for this child is the top entry
                 on the write context stack. */
            ਬੱਚਾ->wt_context = thr->wt_context;
            ਬੱਚਾ->rd_context = thr->wt_context;
      }

      ਬੱਚਾ->ਮਾਪਾ = thr;
      thr->children.insert(ਬੱਚਾ);

      ਜੇ (thr->i_am_ਵਿੱਚ_function) {
	    ਬੱਚਾ->is_scheduled = 1;
	    ਬੱਚਾ->i_am_ਵਿੱਚ_function = 1;
	    vthread_ਲੜੀਵਾਰ(ਬੱਚਾ);
	    running_thread = thr;
      } ਨਹੀਂਤਾਂ {
	    ਸਮਾਂ_ਸਾਰਣੀ_vthread(ਬੱਚਾ, 0, ਸੱਚ);
      }
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_FREE(vthread_t thr, vvp_ਕੋਡ_t cp)
{
        /* Pop the child context from the read context stack. */
      vvp_context_t ਬੱਚਾ_context = thr->rd_context;
      thr->rd_context = vvp_get_stacked_context(ਬੱਚਾ_context);

        /* Free the context. */
      vthread_free_context(ਬੱਚਾ_context, cp->scope);

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %inv
 *
 * Logically, this pops a value, inverts is (Verilog style, with Z and
 * X converted to X) and pushes the result. We can more efficiently
 * just to the invert in place.
 */
ਤਾਰਕਿਕ ਦਾ_INV(vthread_t thr, vvp_ਕੋਡ_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val.invert();
      ਵਾਪਸ ਸੱਚ;
}


/*
 * Index registers, arithmetic.
 */

ਸਥਿਰ ਸਤਰਅੰਦਰ int64_t get_as_64_ਦੋਆਧਾਰੀ_ਅੰਕ(uint32_t ਨੀਵਾਂ_32, uint32_t ਉੱਚਾ_32)
{
      int64_t ਨੀਵਾਂ = ਨੀਵਾਂ_32;
      int64_t res = ਉੱਚਾ_32;

      res <<= 32;
      res |= ਨੀਵਾਂ;
      ਵਾਪਸ res;
}

ਤਾਰਕਿਕ ਦਾ_IX_ਜੋੜੋ(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      thr->ਸ਼ਬਦ[cp->ਸੰਖਿਆ].w_int += get_as_64_ਦੋਆਧਾਰੀ_ਅੰਕ(cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0],
                                                    cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[1]);
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_IX_SUB(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      thr->ਸ਼ਬਦ[cp->ਸੰਖਿਆ].w_int -= get_as_64_ਦੋਆਧਾਰੀ_ਅੰਕ(cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0],
                                                    cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[1]);
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_IX_MUL(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      thr->ਸ਼ਬਦ[cp->ਸੰਖਿਆ].w_int *= get_as_64_ਦੋਆਧਾਰੀ_ਅੰਕ(cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0],
                                                    cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[1]);
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_IX_ਲੱਦੋ(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      thr->ਸ਼ਬਦ[cp->ਸੰਖਿਆ].w_int = get_as_64_ਦੋਆਧਾਰੀ_ਅੰਕ(cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0],
                                                   cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[1]);
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_IX_MOV(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      thr->ਸ਼ਬਦ[cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0]].w_int = thr->ਸ਼ਬਦ[cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[1]].w_int;
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_IX_GETV(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੂਚਕਾਂਕ = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      vvp_net_t*net = cp->net;

      vvp_signal_ਮੁੱਲ*sig = dynamic_cast<vvp_signal_ਮੁੱਲ*>(net->fil);
      ਜੇ (sig == 0) {
	    ਪ੍ਰਤਿਜ੍ਞਾ(net->fil);
	    cerr << thr->get_fileline()
	         << "%%ix/getv error: Net arg not a vector signal? "
		 << typeid(*net->fil).name() << ਸਤਰ_ਅੰਤ;
      }
      ਪ੍ਰਤਿਜ੍ਞਾ(sig);

      vvp_vector4_t vec;
      sig->vec4_ਮੁੱਲ(vec);
      ਤਾਰਕਿਕ ਉਛਾਲ_flag;
      uint64_t val = 0;
      ਤਾਰਕਿਕ ਗਿਆਤ_flag = vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(vec, ਉਛਾਲ_flag, val);

      ਜੇ (ਗਿਆਤ_flag)
	    thr->ਸ਼ਬਦ[ਸੂਚਕਾਂਕ].w_uint = val;
      ਨਹੀਂਤਾਂ
	    thr->ਸ਼ਬਦ[ਸੂਚਕਾਂਕ].w_uint = 0;

	/* Set bit 4 as a flag if the input is unknown. */
      thr->flags[4] = ਗਿਆਤ_flag ? (ਉਛਾਲ_flag ? BIT4_X : BIT4_0) : BIT4_1;

      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_IX_GETV_S(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੂਚਕਾਂਕ = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      vvp_net_t*net = cp->net;

      vvp_signal_ਮੁੱਲ*sig = dynamic_cast<vvp_signal_ਮੁੱਲ*>(net->fil);
      ਜੇ (sig == 0) {
	    ਪ੍ਰਤਿਜ੍ਞਾ(net->fil);
	    cerr << thr->get_fileline()
	         << "%%ix/getv/s error: Net arg not a vector signal? "
		 << "fun=" << typeid(*net->fil).name()
		 << ", fil=" << (net->fil? typeid(*net->fil).name() : "<>")
		 << ਸਤਰ_ਅੰਤ;
      }
      ਪ੍ਰਤਿਜ੍ਞਾ(sig);

      vvp_vector4_t vec;
      sig->vec4_ਮੁੱਲ(vec);
      int64_t val;
      ਤਾਰਕਿਕ ਗਿਆਤ_flag = vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(vec, val, ਸੱਚ, ਸੱਚ);

      ਜੇ (ਗਿਆਤ_flag)
	    thr->ਸ਼ਬਦ[ਸੂਚਕਾਂਕ].w_int = val;
      ਨਹੀਂਤਾਂ
	    thr->ਸ਼ਬਦ[ਸੂਚਕਾਂਕ].w_int = 0;

	/* Set bit 4 as a flag if the input is unknown. */
      thr->flags[4] = ਗਿਆਤ_flag? BIT4_0 : BIT4_1;

      ਵਾਪਸ ਸੱਚ;
}

ਸਥਿਰ uint64_t vec4_ਮੰਜ਼ਿਲ_ਸੂਚਕਾਂਕ(vthread_t thr, ਤਾਰਕਿਕ signed_flag)
{
	// Get all the information we need about the vec4 vector, then
	// pop it away. We only need the bool bits and the length.
      ਅਚਲ vvp_vector4_t&val = thr->peek_vec4();
      ਬਿਨਾਨਿਸ਼ਾਨੀ val_ਮਾਪ = val.size();
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ*ਬਿੱਟ = val.subarray(0, val_ਮਾਪ, ਝੂਠ);
      thr->ਕੱਢੋ_vec4(1);

	// If there are X/Z bits, then the subarray will give us a nil
	// pointer. Set a flag to indicate the error, and give up.
      ਜੇ (ਬਿੱਟ == 0) {
	    thr->flags[4] = BIT4_1;
	    ਵਾਪਸ 0;
      }

      uint64_t v = 0;
      thr->flags[4] = BIT4_0;

      ਪ੍ਰਤਿਜ੍ਞਾ(ਆਕਾਰ(ਬਿੱਟ[0]) <= ਆਕਾਰ(v));

      v = 0;
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < val_ਮਾਪ ; idx += 8*ਆਕਾਰ(ਬਿੱਟ[0])) {
	    uint64_t tmp = ਬਿੱਟ[idx/8/ਆਕਾਰ(ਬਿੱਟ[0])];
	    ਜੇ (idx < 8*ਆਕਾਰ(v)) {
		  v |= tmp << idx;
	    } ਨਹੀਂਤਾਂ {
		  ਤਾਰਕਿਕ ਉਛਾਲ = signed_flag && (v >> 63) ? ~tmp != 0 : tmp != 0;
		  ਜੇ (ਉਛਾਲ) {
			thr->flags[4] = BIT4_X;
			ਰੋਕੋ;
		  }
	    }
      }

	// Set the high bits that are not necessarily filled in by the
	// subarray function.
      ਜੇ (val_ਮਾਪ < 8*ਆਕਾਰ(v)) {
	    ਜੇ (signed_flag && (v & (static_cast<uint64_t>(1)<<(val_ਮਾਪ-1)))) {
		    // Propagate the sign bit...
		  v |= (~static_cast<uint64_t>(0)) << val_ਮਾਪ;

	    } ਨਹੀਂਤਾਂ {
		    // Fill with zeros.
		  v &= ~((~static_cast<uint64_t>(0)) << val_ਮਾਪ);
	    }

      }

      ਮਿਟਾਓ[]ਬਿੱਟ;
      ਵਾਪਸ v;
}

/*
 * %ix/vec4 <idx>
 */
ਤਾਰਕਿਕ ਦਾ_IX_VEC4(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ use_idx = cp->ਸੰਖਿਆ;
      thr->ਸ਼ਬਦ[use_idx].w_uint = vec4_ਮੰਜ਼ਿਲ_ਸੂਚਕਾਂਕ(thr, ਝੂਠ);
      ਵਾਪਸ ਸੱਚ;
}

/*
 * %ix/vec4/s <idx>
 */
ਤਾਰਕਿਕ ਦਾ_IX_VEC4_S(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ use_idx = cp->ਸੰਖਿਆ;
      thr->ਸ਼ਬਦ[use_idx].w_uint = vec4_ਮੰਜ਼ਿਲ_ਸੂਚਕਾਂਕ(thr, ਸੱਚ);
      ਵਾਪਸ ਸੱਚ;
}

/*
 * The various JMP instruction work simply by pulling the new program
 * counter from the instruction and resuming. If the jump is
 * conditional, then test the bit for the expected value first.
 */
ਤਾਰਕਿਕ ਦਾ_JMP(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      ਜੇ (ਸਮਾਂ_ਸਾਰਣੀ_stopped()) {
	    ਸਮਾਂ_ਸਾਰਣੀ_vthread(thr, 0, ਝੂਠ);
	    ਵਾਪਸ ਝੂਠ;
      }

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %jmp/0 <pc>, <flag>
 */
ਤਾਰਕਿਕ ਦਾ_JMP0(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਜੇ (thr->flags[cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0]] == BIT4_0)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      ਜੇ (ਸਮਾਂ_ਸਾਰਣੀ_stopped()) {
	    ਸਮਾਂ_ਸਾਰਣੀ_vthread(thr, 0, ਝੂਠ);
	    ਵਾਪਸ ਝੂਠ;
      }

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %jmp/0xz <pc>, <flag>
 */
ਤਾਰਕਿਕ ਦਾ_JMP0_XZ(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਜੇ (thr->flags[cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0]] != BIT4_1)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      ਜੇ (ਸਮਾਂ_ਸਾਰਣੀ_stopped()) {
	    ਸਮਾਂ_ਸਾਰਣੀ_vthread(thr, 0, ਝੂਠ);
	    ਵਾਪਸ ਝੂਠ;
      }

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %jmp/1 <pc>, <flag>
 */
ਤਾਰਕਿਕ ਦਾ_JMP1(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਜੇ (thr->flags[cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0]] == BIT4_1)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      ਜੇ (ਸਮਾਂ_ਸਾਰਣੀ_stopped()) {
	    ਸਮਾਂ_ਸਾਰਣੀ_vthread(thr, 0, ਝੂਠ);
	    ਵਾਪਸ ਝੂਠ;
      }

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %jmp/1xz <pc>, <flag>
 */
ਤਾਰਕਿਕ ਦਾ_JMP1_XZ(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਜੇ (thr->flags[cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0]] != BIT4_0)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      ਜੇ (ਸਮਾਂ_ਸਾਰਣੀ_stopped()) {
	    ਸਮਾਂ_ਸਾਰਣੀ_vthread(thr, 0, ਝੂਠ);
	    ਵਾਪਸ ਝੂਠ;
      }

      ਵਾਪਸ ਸੱਚ;
}

/*
 * The %join instruction causes the thread to wait for one child
 * to die.  If a child is already dead (and a zombie) then I reap
 * it and go on. Otherwise, I mark myself as waiting in a join so that
 * children know to wake me when they finish.
 */

ਸਥਿਰ ਖਾਲੀ do_join(vthread_t thr, vthread_t ਬੱਚਾ)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ਬੱਚਾ->ਮਾਪਾ == thr);

        /* If the immediate child thread is in an automatic scope... */
      ਜੇ (ਬੱਚਾ->wt_context) {
              /* and is the top level task/function thread... */
            ਜੇ (thr->wt_context != thr->rd_context) {
                    /* Pop the child context from the write context stack. */
                  vvp_context_t ਬੱਚਾ_context = thr->wt_context;
                  thr->wt_context = vvp_get_stacked_context(ਬੱਚਾ_context);

                    /* Push the child context onto the read context stack */
                  vvp_ਸੈੱਟ_stacked_context(ਬੱਚਾ_context, thr->rd_context);
                  thr->rd_context = ਬੱਚਾ_context;
            }
      }

      vthread_reap(ਬੱਚਾ);
}

ਸਥਿਰ ਤਾਰਕਿਕ do_join_opcode(vthread_t thr)
{
      ਪ੍ਰਤਿਜ੍ਞਾ( !thr->i_am_joining );
      ਪ੍ਰਤਿਜ੍ਞਾ( !thr->children.empty());

	// Are there any children that have already ended? If so, then
	// join with that one.
      ਲਈ (ਸੈੱਟ<vthread_t>::iterator cur = thr->children.begin()
		 ; cur != thr->children.ਸਿਰਾ() ; ++cur) {
	    vthread_t curp = *cur;
	    ਜੇ (! curp->i_have_ended)
		  ਜਾਰੀ;

	      // found something!
	    do_join(thr, curp);
	    ਵਾਪਸ ਸੱਚ;
      }

	// Otherwise, tell my children to awaken me when they end,
	// then pause.
      thr->i_am_joining = 1;
      ਵਾਪਸ ਝੂਠ;
}

ਤਾਰਕਿਕ ਦਾ_JOIN(vthread_t thr, vvp_ਕੋਡ_t)
{
      ਵਾਪਸ do_join_opcode(thr);
}

/*
 * This %join/detach <n> instruction causes the thread to detach
 * threads that were created by an earlier %fork.
 */
ਤਾਰਕਿਕ ਦਾ_JOIN_DETACH(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਗਿਣਤੀ_2 = cp->ਸੰਖਿਆ;

      ਪ੍ਰਤਿਜ੍ਞਾ(ਗਿਣਤੀ_2 == thr->children.size());

      ਜਦੋਂ (! thr->children.empty()) {
	    vthread_t ਬੱਚਾ = *thr->children.begin();
	    ਪ੍ਰਤਿਜ੍ਞਾ(ਬੱਚਾ->ਮਾਪਾ == thr);

	      // We cannot detach automatic tasks/functions within an
	      // automatic scope. If we try to do that, we might make
	      // a mess of the allocation of the context. Note that it
	      // is OK if the child context is distinct (See %exec_ufunc.)
	    ਪ੍ਰਤਿਜ੍ਞਾ(ਬੱਚਾ->wt_context==0 || thr->wt_context!=ਬੱਚਾ->wt_context);
	    ਜੇ (ਬੱਚਾ->i_have_ended) {
		    // If the child has already ended, then reap it.
		  vthread_reap(ਬੱਚਾ);

	    } ਨਹੀਂਤਾਂ {
		  size_t res = ਬੱਚਾ->ਮਾਪਾ->children.erase(ਬੱਚਾ);
		  ਪ੍ਰਤਿਜ੍ਞਾ(res == 1);
		  ਬੱਚਾ->i_am_detached = 1;
		  thr->detached_children.insert(ਬੱਚਾ);
	    }
      }

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %load/ar <array-label>, <index>;
*/
ਤਾਰਕਿਕ ਦਾ_ਲੱਦੋ_AR(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ idx = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      ਦੁੱਗਣਾ ਸ਼ਬਦ_2;

	/* The result is 0.0 if the address is undefined. */
      ਜੇ (thr->flags[4] == BIT4_1) {
	    ਸ਼ਬਦ_2 = 0.0;
      } ਨਹੀਂਤਾਂ {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ adr = thr->ਸ਼ਬਦ[idx].w_int;
	    ਸ਼ਬਦ_2 = cp->array->get_ਸ਼ਬਦ_r(adr);
      }

      thr->ਧੱਕੋ_real(ਸ਼ਬਦ_2);
      ਵਾਪਸ ਸੱਚ;
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ ELEM>
ਸਥਿਰ ਤਾਰਕਿਕ ਲੱਦੋ_dar(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      int64_t adr = thr->ਸ਼ਬਦ[3].w_int;
      vvp_net_t*net = cp->net;
      ਪ੍ਰਤਿਜ੍ਞਾ(net);

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      ਪ੍ਰਤਿਜ੍ਞਾ(obj);

      vvp_darray*darray = obj->get_object().peek<vvp_darray>();

      ELEM ਸ਼ਬਦ_2;
      ਜੇ (darray &&
          (adr >= 0) && (thr->flags[4] == BIT4_0)) // A defined address >= 0
	    darray->get_ਸ਼ਬਦ(adr, ਸ਼ਬਦ_2);
      ਨਹੀਂਤਾਂ
	    dq_default(ਸ਼ਬਦ_2, obj->size());

      vthread_ਧੱਕੋ(thr, ਸ਼ਬਦ_2);
      ਵਾਪਸ ਸੱਚ;
}

/*
 * %load/dar/r <array-label>;
 */
ਤਾਰਕਿਕ ਦਾ_ਲੱਦੋ_DAR_R(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ ਲੱਦੋ_dar<ਦੁੱਗਣਾ>(thr, cp);
}

/*
 * %load/dar/str <array-label>;
 */
ਤਾਰਕਿਕ ਦਾ_ਲੱਦੋ_DAR_STR(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ ਲੱਦੋ_dar<string>(thr, cp);
}

/*
 * %load/dar/vec4 <array-label>;
 */
ਤਾਰਕਿਕ ਦਾ_ਲੱਦੋ_DAR_VEC4(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ ਲੱਦੋ_dar<vvp_vector4_t>(thr, cp);
}

/*
 * %load/prop/dar/vec4 <pid>, <wid>;
 *   Indexed read of queue/dynamic-array class property; object on stack,
 *   index in words[3] (same as %load/dar/vec4).
 */
ਤਾਰਕਿਕ ਦਾ_ਲੱਦੋ_PROP_DAR_VEC4(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      size_t pid = cp->ਸੰਖਿਆ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      int64_t adr = thr->ਸ਼ਬਦ[3].w_int;

      vvp_object_t& ਸਿਖਰ = thr->peek_object();
      vvp_cobject*cobj = ਸਿਖਰ.peek<vvp_cobject>();
      ਪ੍ਰਤਿਜ੍ਞਾ(cobj);

      vvp_object_t pobj;
      cobj->get_object(pid, pobj, 0);
      vvp_darray*darray = pobj.peek<vvp_darray>();

      vvp_vector4_t ਸ਼ਬਦ_2;
      ਜੇ (darray && (adr >= 0) && (thr->flags[4] == BIT4_0)) {
	    darray->get_ਸ਼ਬਦ(adr, ਸ਼ਬਦ_2);
      } ਨਹੀਂਤਾਂ {
	    dq_default(ਸ਼ਬਦ_2, wid);
      }

      thr->ਧੱਕੋ_vec4(ਸ਼ਬਦ_2);
      ਵਾਪਸ ਸੱਚ;
}

/*
 * %load/obj <var-label>
 */
ਤਾਰਕਿਕ ਦਾ_ਲੱਦੋ_OBJ(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_net_t*net = cp->net;
      vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      ਪ੍ਰਤਿਜ੍ਞਾ(fun);

      vvp_object_t val = fun->get_object();
      thr->ਧੱਕੋ_object(val);

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %load/obja <index>
 *    Loads the object from array, using index <index> as the index
 *    value. If flags[4] == 1, the calculation of <index> may have
 *    failed, so push nil.
 */
ਤਾਰਕਿਕ ਦਾ_ਲੱਦੋ_OBJA(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ idx = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      vvp_object_t ਸ਼ਬਦ_2;

	/* The result is 0.0 if the address is undefined. */
      ਜੇ (thr->flags[4] == BIT4_1) {
	    ; // Return nil
      } ਨਹੀਂਤਾਂ {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ adr = thr->ਸ਼ਬਦ[idx].w_int;
	    cp->array->get_ਸ਼ਬਦ_obj(adr, ਸ਼ਬਦ_2);
      }

      thr->ਧੱਕੋ_object(ਸ਼ਬਦ_2);
      ਵਾਪਸ ਸੱਚ;
}

/*
 * %load/real <var-label>
 */
ਤਾਰਕਿਕ ਦਾ_ਲੱਦੋ_REAL(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      __vpiHandle*tmp = cp->handle;
      t_vpi_ਮੁੱਲ val;

      val.format = vpiRealVal;
      vpi_get_ਮੁੱਲ(tmp, &val);

      thr->ਧੱਕੋ_real(val.value.real);

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %load/str <var-label>
 */
ਤਾਰਕਿਕ ਦਾ_ਲੱਦੋ_STR(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_net_t*net = cp->net;


      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (net->fun);
      ਪ੍ਰਤਿਜ੍ਞਾ(fun);

      ਅਚਲ string&val = fun->get_string();
      thr->ਧੱਕੋ_str(val);

      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_ਲੱਦੋ_STRA(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ idx = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      string ਸ਼ਬਦ_2;

      ਜੇ (thr->flags[4] == BIT4_1) {
	    ਸ਼ਬਦ_2 = "";
      } ਨਹੀਂਤਾਂ {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ adr = thr->ਸ਼ਬਦ[idx].w_int;
	    ਸ਼ਬਦ_2 = cp->array->get_ਸ਼ਬਦ_str(adr);
      }

      thr->ਧੱਕੋ_str(ਸ਼ਬਦ_2);
      ਵਾਪਸ ਸੱਚ;
}


/*
 * %load/vec4 <net>
 */
ਤਾਰਕਿਕ ਦਾ_ਲੱਦੋ_VEC4(vthread_t thr, vvp_ਕੋਡ_t cp)
{
	// Push a placeholder onto the stack in order to reserve the
	// stack space. Use a reference for the stack top as a target
	// for the load.
      thr->ਧੱਕੋ_vec4(vvp_vector4_t());
      vvp_vector4_t&sig_ਮੁੱਲ = thr->peek_vec4();

      vvp_net_t*net = cp->net;

	// For the %load to work, the functor must actually be a
	// signal functor. Only signals save their vector value.
      vvp_signal_ਮੁੱਲ*sig = dynamic_cast<vvp_signal_ਮੁੱਲ*> (net->fil);
      ਜੇ (sig == 0) {
	    cerr << thr->get_fileline()
	         << "%load/v error: Net arg not a signal? "
		 << (net->fil ? typeid(*net->fil).name() :
	                        typeid(*net->fun).name())
	         << ਸਤਰ_ਅੰਤ;
	    ਪ੍ਰਤਿਜ੍ਞਾ(sig);
	    ਵਾਪਸ ਸੱਚ;
      }

	// Extract the value from the signal and directly into the
	// target stack position.
      sig->vec4_ਮੁੱਲ(sig_ਮੁੱਲ);

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %load/vec4a <arr>, <adrx>
 */
ਤਾਰਕਿਕ ਦਾ_ਲੱਦੋ_VEC4_A(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਪੂਰਨ adr_ਸੂਚਕਾਂਕ = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];

      ਲੰਮਾ adr = thr->ਸ਼ਬਦ[adr_ਸੂਚਕਾਂਕ].w_int;

	// If flag[3] is set, then the calculation of the address
	// failed, and this load should return X instead of the actual
	// value.
      ਜੇ (thr->flags[4] == BIT4_1) {
	    vvp_vector4_t tmp (cp->array->get_ਸ਼ਬਦ_ਮਾਪ(), BIT4_X);
	    thr->ਧੱਕੋ_vec4(tmp);
	    ਵਾਪਸ ਸੱਚ;
      }

      vvp_vector4_t tmp (cp->array->get_ਸ਼ਬਦ(adr));
      thr->ਧੱਕੋ_vec4(tmp);
      ਵਾਪਸ ਸੱਚ;
}

ਸਥਿਰ ਖਾਲੀ do_verylong_mod(vvp_vector4_t&vala, ਅਚਲ vvp_vector4_t&valb,
			    ਤਾਰਕਿਕ ਖੱਬਾ_is_neg, ਤਾਰਕਿਕ ਸੱਜਾ_is_neg)
{
      ਤਾਰਕਿਕ ਬਾਹਰਲਾ_is_neg = ਖੱਬਾ_is_neg;
      ਅਚਲ ਪੂਰਨ len=vala.size();
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *a, *z, *t;
      a = ਨਵਾਂ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ[len+1];
      z = ਨਵਾਂ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ[len+1];
      t = ਨਵਾਂ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ[len+1];

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਹਾਸਲ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਆਰਜ਼ੀ;

      ਪੂਰਨ mxa = -1, mxz = -1;
      ਪੂਰਨ i;
      ਪੂਰਨ current, copylen;

      ਬਿਨਾਨਿਸ਼ਾਨੀ lb_ਹਾਸਲ = ਖੱਬਾ_is_neg? 1 : 0;
      ਬਿਨਾਨਿਸ਼ਾਨੀ rb_ਹਾਸਲ = ਸੱਜਾ_is_neg? 1 : 0;
      ਲਈ (ਪੂਰਨ idx = 0 ;  idx < len ;  idx += 1) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ lb = vala.value(idx);
	    ਬਿਨਾਨਿਸ਼ਾਨੀ rb = valb.value(idx);

	    ਜੇ ((lb | rb) & 2) {
		  ਮਿਟਾਓ []t;
		  ਮਿਟਾਓ []z;
		  ਮਿਟਾਓ []a;
		  vvp_vector4_t tmp(len, BIT4_X);
		  vala = tmp;
		  ਵਾਪਸ;
	    }

	    ਜੇ (ਖੱਬਾ_is_neg) {
		  lb = (1-lb) + lb_ਹਾਸਲ;
		  lb_ਹਾਸਲ = (lb & ~1)? 1 : 0;
		  lb &= 1;
	    }
	    ਜੇ (ਸੱਜਾ_is_neg) {
		  rb = (1-rb) + rb_ਹਾਸਲ;
		  rb_ਹਾਸਲ = (rb & ~1)? 1 : 0;
		  rb &= 1;
	    }

	    z[idx]=lb;
	    a[idx]=1-rb;	// for 2s complement add..
      }

      z[len]=0;
      a[len]=1;

      ਲਈ(i=len-1;i>=0;i--) {
	    ਜੇ(! a[i]) {
		  mxa=i;
		  ਰੋਕੋ;
	    }
      }

      ਲਈ(i=len-1;i>=0;i--) {
	    ਜੇ(z[i]) {
		  mxz=i;
		  ਰੋਕੋ;
	    }
      }

      ਜੇ((mxa>mxz)||(mxa==-1)) {
	    ਜੇ(mxa==-1) {
		  ਮਿਟਾਓ []t;
		  ਮਿਟਾਓ []z;
		  ਮਿਟਾਓ []a;
		  vvp_vector4_t tmpx (len, BIT4_X);
		  vala = tmpx;
		  ਵਾਪਸ;
	    }

	    ਜਾਓ tally;
      }

      copylen = mxa + 2;
      current = mxz - mxa;

      ਜਦੋਂ(current > -1) {
	    ਹਾਸਲ = 1;
	    ਲਈ(i=0;i<copylen;i++) {
		  ਆਰਜ਼ੀ = z[i+current] + a[i] + ਹਾਸਲ;
		  t[i] = (ਆਰਜ਼ੀ&1);
		  ਹਾਸਲ = (ਆਰਜ਼ੀ>>1);
	    }

	    ਜੇ(ਹਾਸਲ) {
		  ਲਈ(i=0;i<copylen;i++) {
			z[i+current] = t[i];
		  }
	    }

	    current--;
      }

 tally:

      vvp_vector4_t tmp (len, BIT4_X);
      ਹਾਸਲ = ਬਾਹਰਲਾ_is_neg? 1 : 0;
      ਲਈ (ਪੂਰਨ idx = 0 ;  idx < len ;  idx += 1) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ob = z[idx];
	    ਜੇ (ਬਾਹਰਲਾ_is_neg) {
		  ob = (1-ob) + ਹਾਸਲ;
		  ਹਾਸਲ = (ob & ~1)? 1 : 0;
		  ob = ob & 1;
	    }
	    tmp.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, ob?BIT4_1:BIT4_0);
      }
      vala = tmp;
      ਮਿਟਾਓ []t;
      ਮਿਟਾਓ []z;
      ਮਿਟਾਓ []a;
}

ਤਾਰਕਿਕ ਦਾ_MAX_WR(vthread_t thr, vvp_ਕੋਡ_t)
{
      ਦੁੱਗਣਾ r = thr->ਕੱਢੋ_real();
      ਦੁੱਗਣਾ l = thr->ਕੱਢੋ_real();
      ਜੇ (r != r)
	    thr->ਧੱਕੋ_real(l);
      ਨਹੀਂਤਾਂ ਜੇ (l != l)
	    thr->ਧੱਕੋ_real(r);
      ਨਹੀਂਤਾਂ ਜੇ (r < l)
	    thr->ਧੱਕੋ_real(l);
      ਨਹੀਂਤਾਂ
	    thr->ਧੱਕੋ_real(r);
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_MIN_WR(vthread_t thr, vvp_ਕੋਡ_t)
{
      ਦੁੱਗਣਾ r = thr->ਕੱਢੋ_real();
      ਦੁੱਗਣਾ l = thr->ਕੱਢੋ_real();
      ਜੇ (r != r)
	    thr->ਧੱਕੋ_real(l);
      ਨਹੀਂਤਾਂ ਜੇ (l != l)
	    thr->ਧੱਕੋ_real(r);
      ਨਹੀਂਤਾਂ ਜੇ (r < l)
	    thr->ਧੱਕੋ_real(r);
      ਨਹੀਂਤਾਂ
	    thr->ਧੱਕੋ_real(l);
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_MOD(vthread_t thr, vvp_ਕੋਡ_t)
{
      vvp_vector4_t valb = thr->ਕੱਢੋ_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      ਪ੍ਰਤਿਜ੍ਞਾ(vala.size()==valb.size());
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = vala.size();

      ਜੇ(wid <= 8*ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ)) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ lv = 0, rv = 0;

	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < wid ;  idx += 1) {
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ lb = vala.value(idx);
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ rb = valb.value(idx);

		  ਜੇ ((lb | rb) & 2)
			ਜਾਓ x_ਬਾਹਰਲਾ;

		  lv |= (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ) lb << idx;
		  rv |= (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ) rb << idx;
	    }

	    ਜੇ (rv == 0)
		  ਜਾਓ x_ਬਾਹਰਲਾ;

	    lv %= rv;

	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < wid ;  idx += 1) {
		  vala.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, (lv&1)?BIT4_1 : BIT4_0);
		  lv >>= 1;
	    }

	    ਵਾਪਸ ਸੱਚ;

      } ਨਹੀਂਤਾਂ {
	    do_verylong_mod(vala, valb, ਝੂਠ, ਝੂਠ);
	    ਵਾਪਸ ਸੱਚ;
      }

 x_ਬਾਹਰਲਾ:
      vala = vvp_vector4_t(wid, BIT4_X);
      ਵਾਪਸ ਸੱਚ;
}

/*
 * %mod/s
 */
ਤਾਰਕਿਕ ਦਾ_MOD_S(vthread_t thr, vvp_ਕੋਡ_t)
{
      vvp_vector4_t valb = thr->ਕੱਢੋ_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      ਪ੍ਰਤਿਜ੍ਞਾ(vala.size()==valb.size());
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = vala.size();

	/* Handle the case that we can fit the bits into a long-long
	   variable. We cause use native % to do the work. */
      ਜੇ(wid <= 8*ਆਕਾਰ(ਲੰਮਾ ਲੰਮਾ)) {
	    ਲੰਮਾ ਲੰਮਾ lv = 0, rv = 0;

	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < wid ;  idx += 1) {
		  ਲੰਮਾ ਲੰਮਾ lb = vala.value(idx);
		  ਲੰਮਾ ਲੰਮਾ rb = valb.value(idx);

		  ਜੇ ((lb | rb) & 2)
			ਜਾਓ x_ਬਾਹਰਲਾ;

		  lv |= (ਲੰਮਾ ਲੰਮਾ) lb << idx;
		  rv |= (ਲੰਮਾ ਲੰਮਾ) rb << idx;
	    }

	    ਜੇ (rv == 0)
		  ਜਾਓ x_ਬਾਹਰਲਾ;

	    ਜੇ ((lv == LLONG_MIN) && (rv == -1))
		  ਜਾਓ zero_ਬਾਹਰਲਾ;

	      /* Sign extend the signed operands when needed. */
	    ਜੇ (wid < 8*ਆਕਾਰ(ਲੰਮਾ ਲੰਮਾ)) {
		  ਜੇ (lv & (1LL << (wid-1)))
			lv |= -1ULL << wid;
		  ਜੇ (rv & (1LL << (wid-1)))
			rv |= -1ULL << wid;
	    }

	    lv %= rv;

	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < wid ;  idx += 1) {
		  vala.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, (lv&1)? BIT4_1 : BIT4_0);
		  lv >>= 1;
	    }

	      // vala is the top of the stack, edited in place, so we
	      // do not need to push the result.

	    ਵਾਪਸ ਸੱਚ;

      } ਨਹੀਂਤਾਂ {

	    ਤਾਰਕਿਕ ਖੱਬਾ_is_neg  = vala.value(vala.size()-1) == BIT4_1;
	    ਤਾਰਕਿਕ ਸੱਜਾ_is_neg = valb.value(valb.size()-1) == BIT4_1;
	    do_verylong_mod(vala, valb, ਖੱਬਾ_is_neg, ਸੱਜਾ_is_neg);
	    ਵਾਪਸ ਸੱਚ;
      }

 x_ਬਾਹਰਲਾ:
      vala = vvp_vector4_t(wid, BIT4_X);
      ਵਾਪਸ ਸੱਚ;
 zero_ਬਾਹਰਲਾ:
      vala = vvp_vector4_t(wid, BIT4_0);
      ਵਾਪਸ ਸੱਚ;
}

/*
 * %mod/wr
 */
ਤਾਰਕਿਕ ਦਾ_MOD_WR(vthread_t thr, vvp_ਕੋਡ_t)
{
      ਦੁੱਗਣਾ r = thr->ਕੱਢੋ_real();
      ਦੁੱਗਣਾ l = thr->ਕੱਢੋ_real();
      thr->ਧੱਕੋ_real(fmod(l,r));

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %neg/wr
 */
ਤਾਰਕਿਕ ਦਾ_NEG_WR(vthread_t thr, vvp_ਕੋਡ_t)
{
      thr->poke_real(0, -thr->peek_real(0));
      ਵਾਪਸ ਸੱਚ;
}

/*
 * %pad/s <wid>
 */
ਤਾਰਕਿਕ ਦਾ_PAD_S(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਸੰਖਿਆ;

      vvp_vector4_t&val = thr->peek_vec4();
      ਬਿਨਾਨਿਸ਼ਾਨੀ old_ਮਾਪ = val.size();

	// Sign-extend.
      ਜੇ (old_ਮਾਪ < wid)
	    val.resize(wid, val.value(old_ਮਾਪ-1));
      ਨਹੀਂਤਾਂ
	    val.resize(wid);

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %pad/u <wid>
 */
ਤਾਰਕਿਕ ਦਾ_PAD_U(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਸੰਖਿਆ;

      vvp_vector4_t&val = thr->peek_vec4();
      val.resize(wid, BIT4_0);

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %part/s <wid>
 * %part/u <wid>
 * Two values are popped from the stack. First, pop the canonical
 * index of the part select, and second is the value to be
 * selected. The result is pushed back to the stack.
 */
ਸਥਿਰ ਤਾਰਕਿਕ ਦਾ_PART_ਅਧਾਰ(vthread_t thr, vvp_ਕੋਡ_t cp, ਤਾਰਕਿਕ signed_flag)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਸੰਖਿਆ;

      vvp_vector4_t base4 = thr->ਕੱਢੋ_vec4();
      vvp_vector4_t&value = thr->peek_vec4();

      vvp_vector4_t res (wid, BIT4_X);

	// NOTE: This is treating the vector as signed. Is that correct?
      int32_t ਅਧਾਰ;
      ਤਾਰਕਿਕ ਮੁੱਲ_ok = vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(base4, ਅਧਾਰ, signed_flag);
      ਜੇ (! ਮੁੱਲ_ok) {
	    value = res;
	    ਵਾਪਸ ਸੱਚ;
      }

      ਜੇ (ਅਧਾਰ >= (int32_t)value.size()) {
	    value = res;
	    ਵਾਪਸ ਸੱਚ;
      }

      ਜੇ ((ਅਧਾਰ+(ਪੂਰਨ)wid) <= 0) {
	    value = res;
	    ਵਾਪਸ ਸੱਚ;
      }

      ਲੰਮਾ vbase = 0;
      ਜੇ (ਅਧਾਰ < 0) {
	    vbase = -ਅਧਾਰ;
	    wid -= vbase;
	    ਅਧਾਰ = 0;
      }

      ਜੇ ((ਅਧਾਰ+wid) > value.size()) {
	    wid = value.size() - ਅਧਾਰ;
      }

      res .ਸੈੱਟ_vec(vbase, value.subvalue(ਅਧਾਰ, wid));
      value = res;

      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_PART_S(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ ਦਾ_PART_ਅਧਾਰ(thr, cp, ਸੱਚ);
}

ਤਾਰਕਿਕ ਦਾ_PART_U(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ ਦਾ_PART_ਅਧਾਰ(thr, cp, ਝੂਠ);
}

/*
 * %parti/s <wid>, <basei>, <base_wid>
 * %parti/u <wid>, <basei>, <base_wid>
 *
 * Pop the value to be selected. The result is pushed back to the stack.
 */
ਸਥਿਰ ਤਾਰਕਿਕ ਦਾ_PARTI_ਅਧਾਰ(vthread_t thr, vvp_ਕੋਡ_t cp, ਤਾਰਕਿਕ signed_flag)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਸੰਖਿਆ;
      uint32_t ਅਧਾਰ = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      uint32_t bwid = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[1];

      vvp_vector4_t&value = thr->peek_vec4();

      vvp_vector4_t res (wid, BIT4_X);

	// NOTE: This is treating the vector as signed. Is that correct?
      int32_t use_ਅਧਾਰ = ਅਧਾਰ;
      ਜੇ (signed_flag && bwid < 32 && (ਅਧਾਰ&(1<<(bwid-1)))) {
	    use_ਅਧਾਰ |= -1UL << bwid;
      }

      ਜੇ (use_ਅਧਾਰ >= (int32_t)value.size()) {
	    value = res;
	    ਵਾਪਸ ਸੱਚ;
      }

      ਜੇ ((use_ਅਧਾਰ+(int32_t)wid) <= 0) {
	    value = res;
	    ਵਾਪਸ ਸੱਚ;
      }

      ਲੰਮਾ vbase = 0;
      ਜੇ (use_ਅਧਾਰ < 0) {
	    vbase = -use_ਅਧਾਰ;
	    wid -= vbase;
	    use_ਅਧਾਰ = 0;
      }

      ਜੇ ((use_ਅਧਾਰ+wid) > value.size()) {
	    wid = value.size() - use_ਅਧਾਰ;
      }

      res .ਸੈੱਟ_vec(vbase, value.subvalue(use_ਅਧਾਰ, wid));
      value = res;

      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_PARTI_S(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ ਦਾ_PARTI_ਅਧਾਰ(thr, cp, ਸੱਚ);
}

ਤਾਰਕਿਕ ਦਾ_PARTI_U(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ ਦਾ_PARTI_ਅਧਾਰ(thr, cp, ਝੂਠ);
}

/*
 * %mul
 */
ਤਾਰਕਿਕ ਦਾ_MUL(vthread_t thr, vvp_ਕੋਡ_t)
{
      vvp_vector4_t r = thr->ਕੱਢੋ_vec4();
	// Rather then pop l, use it directly from the stack. When we
	// assign to 'l', that will edit the top of the stack, which
	// replaces a pop and a pull.
      vvp_vector4_t&l = thr->peek_vec4();

      l.mul(r);
      ਵਾਪਸ ਸੱਚ;
}

/*
 * %muli <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
ਤਾਰਕਿਕ ਦਾ_MULI(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਸੰਖਿਆ;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_ਤੁਰੰਤ_rval (cp, r);

      l.mul(r);
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_MUL_WR(vthread_t thr, vvp_ਕੋਡ_t)
{
      ਦੁੱਗਣਾ r = thr->ਕੱਢੋ_real();
      ਦੁੱਗਣਾ l = thr->ਕੱਢੋ_real();
      thr->ਧੱਕੋ_real(l * r);

      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_NAND(vthread_t thr, vvp_ਕੋਡ_t)
{
      vvp_vector4_t valr = thr->ਕੱਢੋ_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      ਪ੍ਰਤਿਜ੍ਞਾ(vall.size() == valr.size());

      vall &= valr;
      vall.invert();

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %new/cobj <vpi_object>
 * This creates a new cobject (SystemVerilog class object) and pushes
 * it to the stack. The <vpi-object> is a __vpiHandle that is a
 * vpiClassDefn object that defines the item to be created.
 */
ਤਾਰਕਿਕ ਦਾ_NEW_COBJ(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਅਚਲ class_type*defn = dynamic_cast<ਅਚਲ class_type*> (cp->handle);
      ਪ੍ਰਤਿਜ੍ਞਾ(defn);

      vvp_object_t tmp (ਨਵਾਂ vvp_cobject(defn));
      thr->ਧੱਕੋ_object(tmp);
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_NEW_DARRAY(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਅਚਲ ਅੱਖਰ*ਪਾਠ = cp->ਪਾਠ;
      size_t size = thr->ਸ਼ਬਦ[cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0]].w_int;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ_wid;
      size_t n;

      vvp_object_t obj;
      ਜੇ (strcmp(ਪਾਠ,"b8") == 0) {
	    obj = ਨਵਾਂ vvp_darray_atom<uint8_t>(size);
      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(ਪਾਠ,"b16") == 0) {
	    obj = ਨਵਾਂ vvp_darray_atom<uint16_t>(size);
      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(ਪਾਠ,"b32") == 0) {
	    obj = ਨਵਾਂ vvp_darray_atom<uint32_t>(size);
      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(ਪਾਠ,"b64") == 0) {
	    obj = ਨਵਾਂ vvp_darray_atom<uint64_t>(size);
      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(ਪਾਠ,"sb8") == 0) {
	    obj = ਨਵਾਂ vvp_darray_atom<int8_t>(size);
      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(ਪਾਠ,"sb16") == 0) {
	    obj = ਨਵਾਂ vvp_darray_atom<int16_t>(size);
      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(ਪਾਠ,"sb32") == 0) {
	    obj = ਨਵਾਂ vvp_darray_atom<int32_t>(size);
      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(ਪਾਠ,"sb64") == 0) {
	    obj = ਨਵਾਂ vvp_darray_atom<int64_t>(size);
      } ਨਹੀਂਤਾਂ ਜੇ ((1 == ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(ਪਾਠ, "b%u%zn", &ਸ਼ਬਦ_wid, &n)) &&
                 (n == ਲੜੀ_ਲੰਬਾਈ(ਪਾਠ))) {
	    obj = ਨਵਾਂ vvp_darray_vec2(size, ਸ਼ਬਦ_wid);
      } ਨਹੀਂਤਾਂ ਜੇ ((1 == ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(ਪਾਠ, "sb%u%zn", &ਸ਼ਬਦ_wid, &n)) &&
                 (n == ਲੜੀ_ਲੰਬਾਈ(ਪਾਠ))) {
	    obj = ਨਵਾਂ vvp_darray_vec2(size, ਸ਼ਬਦ_wid);
      } ਨਹੀਂਤਾਂ ਜੇ ((1 == ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(ਪਾਠ, "v%u%zn", &ਸ਼ਬਦ_wid, &n)) &&
                 (n == ਲੜੀ_ਲੰਬਾਈ(ਪਾਠ))) {
	    obj = ਨਵਾਂ vvp_darray_vec4(size, ਸ਼ਬਦ_wid);
      } ਨਹੀਂਤਾਂ ਜੇ ((1 == ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(ਪਾਠ, "sv%u%zn", &ਸ਼ਬਦ_wid, &n)) &&
                 (n == ਲੜੀ_ਲੰਬਾਈ(ਪਾਠ))) {
	    obj = ਨਵਾਂ vvp_darray_vec4(size, ਸ਼ਬਦ_wid);
      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(ਪਾਠ,"r") == 0) {
	    obj = ਨਵਾਂ vvp_darray_real(size);
      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(ਪਾਠ,"S") == 0) {
	    obj = ਨਵਾਂ vvp_darray_string(size);
      } ਨਹੀਂਤਾਂ {
	    cerr << get_fileline()
	         << "Internal error: Unsupported dynamic array type: "
	         << ਪਾਠ << "." << ਸਤਰ_ਅੰਤ;
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
      }

      thr->ਧੱਕੋ_object(obj);

      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_NOOP(vthread_t, vvp_ਕੋਡ_t)
{
      ਵਾਪਸ ਸੱਚ;
}

/*
 * %nor/r
 */
ਤਾਰਕਿਕ ਦਾ_NORR(vthread_t thr, vvp_ਕੋਡ_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_or());

      ਵਾਪਸ ਸੱਚ;
}

/*
 * Push a null to the object stack.
 */
ਤਾਰਕਿਕ ਦਾ_NULL(vthread_t thr, vvp_ਕੋਡ_t)
{
      vvp_object_t tmp;
      thr->ਧੱਕੋ_object(tmp);
      ਵਾਪਸ ਸੱਚ;
}

/*
 * %and/r
 */
ਤਾਰਕਿਕ ਦਾ_ANDR(vthread_t thr, vvp_ਕੋਡ_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_and());

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %nand/r
 */
ਤਾਰਕਿਕ ਦਾ_NANDR(vthread_t thr, vvp_ਕੋਡ_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_and());

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %or/r
 */
ਤਾਰਕਿਕ ਦਾ_ORR(vthread_t thr, vvp_ਕੋਡ_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_or());

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %xor/r
 */
ਤਾਰਕਿਕ ਦਾ_XORR(vthread_t thr, vvp_ਕੋਡ_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_xor());

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %xnor/r
 */
ਤਾਰਕਿਕ ਦਾ_XNORR(vthread_t thr, vvp_ਕੋਡ_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_xor());

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %or
 */
ਤਾਰਕਿਕ ਦਾ_OR(vthread_t thr, vvp_ਕੋਡ_t)
{
      vvp_vector4_t valb = thr->ਕੱਢੋ_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();
      vala |= valb;
      ਵਾਪਸ ਸੱਚ;
}

/*
 * %nor
 */
ਤਾਰਕਿਕ ਦਾ_NOR(vthread_t thr, vvp_ਕੋਡ_t)
{
      vvp_vector4_t valr = thr->ਕੱਢੋ_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      ਪ੍ਰਤਿਜ੍ਞਾ(vall.size() == valr.size());

      vall |= valr;
      vall.invert();

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %pop/obj <num>, <skip>
 */
ਤਾਰਕਿਕ ਦਾ_ਕੱਢੋ_OBJ(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ cnt = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      ਬਿਨਾਨਿਸ਼ਾਨੀ skip = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[1];

      thr->ਕੱਢੋ_object(cnt, skip);
      ਵਾਪਸ ਸੱਚ;
}

/*
 * %pop/real <number>
 */
ਤਾਰਕਿਕ ਦਾ_ਕੱਢੋ_REAL(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ cnt = cp->ਸੰਖਿਆ;
      thr->ਕੱਢੋ_real(cnt);
      ਵਾਪਸ ਸੱਚ;
}

/*
 *  %pop/str <number>
 */
ਤਾਰਕਿਕ ਦਾ_ਕੱਢੋ_STR(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ cnt = cp->ਸੰਖਿਆ;
      thr->ਕੱਢੋ_str(cnt);
      ਵਾਪਸ ਸੱਚ;
}

/*
 *  %pop/vec4 <number>
 */
ਤਾਰਕਿਕ ਦਾ_ਕੱਢੋ_VEC4(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ cnt = cp->ਸੰਖਿਆ;
      thr->ਕੱਢੋ_vec4(cnt);
      ਵਾਪਸ ਸੱਚ;
}

/*
 * %pow
 * %pow/s
 */
ਸਥਿਰ ਤਾਰਕਿਕ ਦਾ_POW_ਅਧਾਰ(vthread_t thr, ਤਾਰਕਿਕ signed_flag)
{
      vvp_vector4_t valb = thr->ਕੱਢੋ_vec4();
      vvp_vector4_t vala = thr->ਕੱਢੋ_vec4();

      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = vala.size();

      vvp_vector2_t xv2 = vvp_vector2_t(vala, ਸੱਚ);
      vvp_vector2_t yv2 = vvp_vector2_t(valb, ਸੱਚ);


        /* If we have an X or Z in the arguments return X. */
      ਜੇ (xv2.is_NaN() || yv2.is_NaN()) {
	    vvp_vector4_t tmp (wid, BIT4_X);
	    thr->ਧੱਕੋ_vec4(tmp);
	    ਵਾਪਸ ਸੱਚ;
      }

	// Is the exponent negative? If so, table 5-6 in IEEE1364-2005
	// defines what value is returned.
      ਜੇ (signed_flag && yv2.value(yv2.size()-1)) {
	    ਪੂਰਨ a_val;
	    vvp_bit4_t pad = BIT4_0, lsb = BIT4_0;
	    ਜੇ (vector2_ਮੰਜ਼ਿਲ_ਮੁੱਲ(xv2, a_val, ਸੱਚ)) {
		  ਜੇ (a_val == 0) {
			pad = BIT4_X; lsb = BIT4_X;
		  }
		  ਜੇ (a_val == 1) {
			pad = BIT4_0; lsb = BIT4_1;
		  }
		  ਜੇ (a_val == -1) {
			ਜੇ (yv2.value(0)) {
			      pad = BIT4_1; lsb = BIT4_1;
			} ਨਹੀਂਤਾਂ {
			      pad = BIT4_0; lsb = BIT4_1;
			}
		  }
	    }
	    vvp_vector4_t tmp (wid, pad);
	    tmp.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, lsb);
	    thr->ਧੱਕੋ_vec4(tmp);
	    ਵਾਪਸ ਸੱਚ;
      }

      vvp_vector2_t ਨਤੀਜਾ = ਘਾਤ(xv2, yv2);

        /* Copy only what we need of the result. If the result is too
	   small, zero-pad it. */
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ jdx = 0;  jdx < wid;  jdx += 1) {
	    ਜੇ (jdx >= ਨਤੀਜਾ.size())
		  vala.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(jdx, BIT4_0);
	    ਨਹੀਂਤਾਂ
		  vala.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(jdx, ਨਤੀਜਾ.value(jdx) ? BIT4_1 : BIT4_0);
      }
      thr->ਧੱਕੋ_vec4(vala);

      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_POW(vthread_t thr, vvp_ਕੋਡ_t)
{
      ਵਾਪਸ ਦਾ_POW_ਅਧਾਰ(thr, ਝੂਠ);
}

ਤਾਰਕਿਕ ਦਾ_POW_S(vthread_t thr, vvp_ਕੋਡ_t)
{
      ਵਾਪਸ ਦਾ_POW_ਅਧਾਰ(thr, ਸੱਚ);
}

ਤਾਰਕਿਕ ਦਾ_POW_WR(vthread_t thr, vvp_ਕੋਡ_t)
{
      ਦੁੱਗਣਾ r = thr->ਕੱਢੋ_real();
      ਦੁੱਗਣਾ l = thr->ਕੱਢੋ_real();
      thr->ਧੱਕੋ_real(ਘਾਤ(l,r));

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %prop/obj <pid>, <idx>
 *
 * Load an object value from the cobject and push it onto the object stack.
 */
ਤਾਰਕਿਕ ਦਾ_PROP_OBJ(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ pid = cp->ਸੰਖਿਆ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ idx = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];

      ਜੇ (idx != 0) {
	    ਪ੍ਰਤਿਜ੍ਞਾ(idx < vthread_s::ਸ਼ਬਦ_ਗਿਣਤੀ);
	    idx = thr->ਸ਼ਬਦ[idx].w_uint;
      }

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();

      vvp_object_t val;
      cobj->get_object(pid, val, idx);

      thr->ਧੱਕੋ_object(val);

      ਵਾਪਸ ਸੱਚ;
}

ਸਥਿਰ ਖਾਲੀ get_from_obj(ਬਿਨਾਨਿਸ਼ਾਨੀ pid, vvp_cobject*cobj, ਦੁੱਗਣਾ&val)
{
      val = cobj->get_real(pid);
}

ਸਥਿਰ ਖਾਲੀ get_from_obj(ਬਿਨਾਨਿਸ਼ਾਨੀ pid, vvp_cobject*cobj, string&val)
{
      val = cobj->get_string(pid);
}

ਸਥਿਰ ਖਾਲੀ get_from_obj(ਬਿਨਾਨਿਸ਼ਾਨੀ pid, vvp_cobject*cobj, vvp_vector4_t&val)
{
      cobj->get_vec4(pid, val);
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ ELEM>
ਸਥਿਰ ਤਾਰਕਿਕ prop(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ pid = cp->ਸੰਖਿਆ;

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      ਪ੍ਰਤਿਜ੍ਞਾ(cobj);

      ELEM val;
      get_from_obj(pid, cobj, val);
      vthread_ਧੱਕੋ(thr, val);

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %prop/r <pid>
 *
 * Load a real value from the cobject and push it onto the real value
 * stack.
 */
ਤਾਰਕਿਕ ਦਾ_PROP_R(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ prop<ਦੁੱਗਣਾ>(thr, cp);
}

/*
 * %prop/str <pid>
 *
 * Load a string value from the cobject and push it onto the real value
 * stack.
 */
ਤਾਰਕਿਕ ਦਾ_PROP_STR(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ prop<string>(thr, cp);
}

/*
 * %prop/v <pid>
 *
 * Load a property <pid> from the cobject on the top of the stack into
 * the vector space at <base>.
 */
ਤਾਰਕਿਕ ਦਾ_PROP_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ prop<vvp_vector4_t>(thr, cp);
}

ਤਾਰਕਿਕ ਦਾ_PUSHI_REAL(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਦੁੱਗਣਾ mant = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      uint32_t imant = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      ਪੂਰਨ ਘਾਤਾਂਕ = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[1];

	// Detect +infinity
      ਜੇ (ਘਾਤਾਂਕ==0x3fff && imant==0) {
	    thr->ਧੱਕੋ_real(INFINITY);
	    ਵਾਪਸ ਸੱਚ;
      }
	// Detect -infinity
      ਜੇ (ਘਾਤਾਂਕ==0x7fff && imant==0) {
	    thr->ਧੱਕੋ_real(-INFINITY);
	    ਵਾਪਸ ਸੱਚ;
      }
	// Detect NaN
      ਜੇ (ਘਾਤਾਂਕ==0x3fff) {
	    thr->ਧੱਕੋ_real(nan(""));
	    ਵਾਪਸ ਸੱਚ;
      }

      ਦੁੱਗਣਾ sign = (ਘਾਤਾਂਕ & 0x4000)? -1.0 : 1.0;

      ਘਾਤਾਂਕ &= 0x1fff;

      mant = sign * ldexp(mant, ਘਾਤਾਂਕ - 0x1000);
      thr->ਧੱਕੋ_real(mant);
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_PUSHI_STR(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਅਚਲ ਅੱਖਰ*ਪਾਠ = cp->ਪਾਠ;
      thr->ਧੱਕੋ_str(filter_string(ਪਾਠ));
      ਵਾਪਸ ਸੱਚ;
}

/*
 * %pushi/vec4 <vala>, <valb>, <wid>
 */
ਤਾਰਕਿਕ ਦਾ_PUSHI_VEC4(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid  = cp->ਸੰਖਿਆ;

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t val (wid, BIT4_0);
      get_ਤੁਰੰਤ_rval (cp, val);

      thr->ਧੱਕੋ_vec4(val);

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %pushv/str
 *   Pops a vec4 value, and pushes a string.
 */
ਤਾਰਕਿਕ ਦਾ_PUSHV_STR(vthread_t thr, vvp_ਕੋਡ_t)
{
      vvp_vector4_t vec = thr->ਕੱਢੋ_vec4();

      size_t slen = (vec.size() + 7)/8;
      vector<ਅੱਖਰ>buf;
      buf.reserve(slen);

      ਲਈ (size_t idx = 0 ; idx < vec.size() ; idx += 8) {
	    ਅੱਖਰ tmp = 0;
	    size_t trans = 8;
	    ਜੇ (idx+trans > vec.size())
		  trans = vec.size() - idx;

	    ਲਈ (size_t bdx = 0 ; bdx < trans ; bdx += 1) {
		  ਜੇ (vec.value(idx+bdx) == BIT4_1)
			tmp |= 1 << bdx;
	    }

	    ਜੇ (tmp != 0)
		  buf.push_back(tmp);
      }

      string val;
      ਲਈ (vector<ਅੱਖਰ>::reverse_iterator cur = buf.rbegin()
		 ; cur != buf.rend() ; ++cur) {
	    val.push_back(*cur);
      }

      thr->ਧੱਕੋ_str(val);

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %putc/str/vec4 <var>, <mux>
 */
ਤਾਰਕਿਕ ਦਾ_PUTC_STR_VEC4(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ muxr = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      int32_t mux = muxr? thr->ਸ਼ਬਦ[muxr].w_int : 0;

      vvp_vector4_t val = thr->ਕੱਢੋ_vec4();
      ਪ੍ਰਤਿਜ੍ਞਾ(val.size() == 8);

      ਜੇ (mux < 0)
	    ਵਾਪਸ ਸੱਚ;

	/* Get the existing value of the string. If we find that the
	   index is too big for the string, then give up. */
      vvp_net_t*net = cp->net;
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (net->fun);
      ਪ੍ਰਤਿਜ੍ਞਾ(fun);

      string tmp = fun->get_string();
      ਜੇ (tmp.size() <= (size_t)mux)
	    ਵਾਪਸ ਸੱਚ;

      ਅੱਖਰ val_str = 0;
      ਲਈ (size_t idx = 0 ; idx < 8 ; idx += 1) {
	    ਜੇ (val.value(idx)==BIT4_1)
		  val_str |= 1<<idx;
      }

	// It is a quirk of the Verilog standard that putc(..., 'h00)
	// has no effect. Test for that case here.
      ਜੇ (val_str == 0)
	    ਵਾਪਸ ਸੱਚ;

      tmp[mux] = val_str;

      vvp_send_string(vvp_net_ptr_t(cp->net, 0), tmp, thr->wt_context);
      ਵਾਪਸ ਸੱਚ;
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ ELEM, ਵਰਗ QTYPE>
ਸਥਿਰ ਤਾਰਕਿਕ qinsert(vthread_t thr, vvp_ਕੋਡ_t cp, ਬਿਨਾਨਿਸ਼ਾਨੀ wid=0)
{
      int64_t idx = thr->ਸ਼ਬਦ[3].w_int;
      ELEM value;
      vvp_net_t*net = cp->net;
      ਕੱਢੋ_ਮੁੱਲ(thr, value, wid); // Pop the value to store.

      vvp_ਕਤਾਰ*ਕਤਾਰ = get_ਕਤਾਰ_object<QTYPE>(thr, net);
      ਪ੍ਰਤਿਜ੍ਞਾ(ਕਤਾਰ);
      ਜੇ (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at a negative "
	         << get_ਕਤਾਰ_type(value)
	         << " index (" << idx << "). ";
	    print_ਕਤਾਰ_ਮੁੱਲ(value);
	    cerr << " was not added." << ਸਤਰ_ਅੰਤ;
      } ਨਹੀਂਤਾਂ ਜੇ (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at an undefined "
	         << get_ਕਤਾਰ_type(value) << " index. ";
	    print_ਕਤਾਰ_ਮੁੱਲ(value);
	    cerr << " was not added." << ਸਤਰ_ਅੰਤ;
      } ਨਹੀਂਤਾਂ {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ = thr->ਸ਼ਬਦ[cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0]].w_int;
	    ਕਤਾਰ->insert(idx, value, max_ਮਾਪ);
      }
      ਵਾਪਸ ਸੱਚ;
}

/*
 * %qinsert/real <var-label>
 */
ਤਾਰਕਿਕ ਦਾ_QINSERT_REAL(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ qinsert<ਦੁੱਗਣਾ, vvp_ਕਤਾਰ_real>(thr, cp);
}

/*
 * %qinsert/str <var-label>
 */
ਤਾਰਕਿਕ ਦਾ_QINSERT_STR(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ qinsert<string, vvp_ਕਤਾਰ_string>(thr, cp);
}

/*
 * %qinsert/v <var-label>
 */
ਤਾਰਕਿਕ ਦਾ_QINSERT_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ qinsert<vvp_vector4_t, vvp_ਕਤਾਰ_vec4>(thr, cp, cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[1]);
}

/*
 * Helper functions used in the queue pop templates
 */
ਸਤਰਅੰਦਰ ਖਾਲੀ ਧੱਕੋ_ਮੁੱਲ(vthread_t thr, ਦੁੱਗਣਾ value, ਬਿਨਾਨਿਸ਼ਾਨੀ)
{
      thr->ਧੱਕੋ_real(value);
}

ਸਤਰਅੰਦਰ ਖਾਲੀ ਧੱਕੋ_ਮੁੱਲ(vthread_t thr, ਅਚਲ string&value, ਬਿਨਾਨਿਸ਼ਾਨੀ)
{
      thr->ਧੱਕੋ_str(value);
}

ਸਤਰਅੰਦਰ ਖਾਲੀ ਧੱਕੋ_ਮੁੱਲ(vthread_t thr, ਅਚਲ vvp_vector4_t&value, ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(wid == value.size());
      thr->ਧੱਕੋ_vec4(value);
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ ELEM, ਵਰਗ QTYPE>
ਸਥਿਰ ਤਾਰਕਿਕ q_ਕੱਢੋ(vthread_t thr, vvp_ਕੋਡ_t cp,
                  ਖਾਲੀ (*get_val_func)(vvp_ਕਤਾਰ*, ELEM&),
                  ਅਚਲ ਅੱਖਰ*loc, ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
{
      vvp_net_t*net = cp->net;

      vvp_ਕਤਾਰ*ਕਤਾਰ = get_ਕਤਾਰ_object<QTYPE>(thr, net);
      ਪ੍ਰਤਿਜ੍ਞਾ(ਕਤਾਰ);

      size_t size = ਕਤਾਰ->get_ਮਾਪ();

      ELEM value;
      ਜੇ (size) {
	    get_val_func(ਕਤਾਰ, value);
      } ਨਹੀਂਤਾਂ {
	    dq_default(value, wid);
	    cerr << thr->get_fileline()
	         << "Warning: pop_" << loc << "() on empty "
	         << get_ਕਤਾਰ_type(value) << "." << ਸਤਰ_ਅੰਤ;
      }

      ਧੱਕੋ_ਮੁੱਲ(thr, value, wid);
      ਵਾਪਸ ਸੱਚ;
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ ELEM>
ਸਥਿਰ ਖਾਲੀ get_ਪਿੱਠ_ਮੁੱਲ(vvp_ਕਤਾਰ*ਕਤਾਰ, ELEM&value)
{
      ਕਤਾਰ->get_ਸ਼ਬਦ(ਕਤਾਰ->get_ਮਾਪ()-1, value);
      ਕਤਾਰ->pop_back();
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ ELEM, ਵਰਗ QTYPE>
ਸਥਿਰ ਤਾਰਕਿਕ qpop_b(vthread_t thr, vvp_ਕੋਡ_t cp, ਬਿਨਾਨਿਸ਼ਾਨੀ wid=0)
{
      ਵਾਪਸ q_ਕੱਢੋ<ELEM, QTYPE>(thr, cp, get_ਪਿੱਠ_ਮੁੱਲ<ELEM>, "back", wid);
}

/*
 * %qpop/b/real <var-label>
 */
ਤਾਰਕਿਕ ਦਾ_QPOP_B_REAL(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ qpop_b<ਦੁੱਗਣਾ, vvp_ਕਤਾਰ_real>(thr, cp);
}

/*
 * %qpop/b/str <var-label>
 */
ਤਾਰਕਿਕ ਦਾ_QPOP_B_STR(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ qpop_b<string, vvp_ਕਤਾਰ_string>(thr, cp);
}

/*
 * %qpop/b/v <var-label>
 */
ਤਾਰਕਿਕ ਦਾ_QPOP_B_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ qpop_b<vvp_vector4_t, vvp_ਕਤਾਰ_vec4>(thr, cp, cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0]);
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ ELEM>
ਸਥਿਰ ਖਾਲੀ get_front_ਮੁੱਲ(vvp_ਕਤਾਰ*ਕਤਾਰ, ELEM&value)
{
      ਕਤਾਰ->get_ਸ਼ਬਦ(0, value);
      ਕਤਾਰ->pop_front();
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ ELEM, ਵਰਗ QTYPE>
ਸਥਿਰ ਤਾਰਕਿਕ qpop_f(vthread_t thr, vvp_ਕੋਡ_t cp, ਬਿਨਾਨਿਸ਼ਾਨੀ wid=0)
{
      ਵਾਪਸ q_ਕੱਢੋ<ELEM, QTYPE>(thr, cp, get_front_ਮੁੱਲ<ELEM>, "front", wid);
}


/*
 * %qpop/f/real <var-label>
 */
ਤਾਰਕਿਕ ਦਾ_QPOP_F_REAL(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ qpop_f<ਦੁੱਗਣਾ, vvp_ਕਤਾਰ_real>(thr, cp);
}

/*
 * %qpop/f/str <var-label>
 */
ਤਾਰਕਿਕ ਦਾ_QPOP_F_STR(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ qpop_f<string, vvp_ਕਤਾਰ_string>(thr, cp);
}

/*
 * %qpop/f/v <var-label>
 */
ਤਾਰਕਿਕ ਦਾ_QPOP_F_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ qpop_f<vvp_vector4_t, vvp_ਕਤਾਰ_vec4>(thr, cp, cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0]);
}

/*
 * These implement the %release/net and %release/reg instructions. The
 * %release/net instruction applies to a net kind of functor by
 * sending the release/net command to the command port. (See vvp_net.h
 * for details.) The %release/reg instruction is the same, but sends
 * the release/reg command instead. These are very similar to the
 * %deassign instruction.
 */
ਸਥਿਰ ਤਾਰਕਿਕ do_release_vec(vvp_ਕੋਡ_t cp, ਤਾਰਕਿਕ net_flag)
{
      vvp_net_t*net = cp->net;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ  = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      ਬਿਨਾਨਿਸ਼ਾਨੀ width = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[1];

      ਪ੍ਰਤਿਜ੍ਞਾ(net->fil);

      ਜੇ (ਅਧਾਰ >= net->fil->filter_ਮਾਪ()) ਵਾਪਸ ਸੱਚ;
      ਜੇ (ਅਧਾਰ+width > net->fil->filter_ਮਾਪ())
	    width = net->fil->filter_ਮਾਪ() - ਅਧਾਰ;

      ਤਾਰਕਿਕ ਭਰਿਆ_sig = ਅਧਾਰ == 0 && width == net->fil->filter_ਮਾਪ();

	// XXXX Can't really do this if this is a partial release?
      net->fil->force_unlink();

	/* Do we release all or part of the net? */
      vvp_net_ptr_t ptr (net, 0);
      ਜੇ (ਭਰਿਆ_sig) {
	    net->fil->release(ptr, net_flag);
      } ਨਹੀਂਤਾਂ {
	    net->fil->release_pv(ptr, ਅਧਾਰ, width, net_flag);
      }
      net->fun->force_flag(ਝੂਠ);

      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_RELEASE_NET(vthread_t, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ do_release_vec(cp, ਸੱਚ);
}


ਤਾਰਕਿਕ ਦਾ_RELEASE_REG(vthread_t, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ do_release_vec(cp, ਝੂਠ);
}

/* The type is 1 for registers and 0 for everything else. */
ਤਾਰਕਿਕ ਦਾ_RELEASE_WR(vthread_t, vvp_ਕੋਡ_t cp)
{
      vvp_net_t*net = cp->net;
      ਬਿਨਾਨਿਸ਼ਾਨੀ type  = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];

      ਪ੍ਰਤਿਜ੍ਞਾ(net->fil);
      net->fil->force_unlink();

	// Send a command to this signal to unforce itself.
      vvp_net_ptr_t ptr (net, 0);
      net->fil->release(ptr, type==0);
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_REPLICATE(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਪੂਰਨ rept = cp->ਸੰਖਿਆ;
      vvp_vector4_t val = thr->ਕੱਢੋ_vec4();
      vvp_vector4_t res (val.size() * rept, BIT4_X);

      ਲਈ (ਪੂਰਨ idx = 0 ; idx < rept ; idx += 1) {
	    res.ਸੈੱਟ_vec(idx * val.size(), val);
      }

      thr->ਧੱਕੋ_vec4(res);

      ਵਾਪਸ ਸੱਚ;
}

ਸਥਿਰ ਖਾਲੀ poke_val(vthread_t fun_thr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਡੂੰਘਾਈ, ਦੁੱਗਣਾ val)
{
      fun_thr->ਮਾਪਾ->poke_real(ਡੂੰਘਾਈ, val);
}

ਸਥਿਰ ਖਾਲੀ poke_val(vthread_t fun_thr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਡੂੰਘਾਈ, ਅਚਲ string&val)
{
      fun_thr->ਮਾਪਾ->poke_str(ਡੂੰਘਾਈ, val);
}

ਸਥਿਰ size_t get_max(vthread_t fun_thr, ਦੁੱਗਣਾ&)
{
      ਵਾਪਸ fun_thr->ਦਲੀਲਾਂ_real.size();
}

ਸਥਿਰ size_t get_max(vthread_t fun_thr, string&)
{
      ਵਾਪਸ fun_thr->ਦਲੀਲਾਂ_str.size();
}

ਸਥਿਰ size_t get_max(vthread_t fun_thr, vvp_vector4_t&)
{
      ਵਾਪਸ fun_thr->ਦਲੀਲਾਂ_vec4.size();
}

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ get_ਡੂੰਘਾਈ(vthread_t fun_thr, size_t ਸੂਚਕਾਂਕ, ਦੁੱਗਣਾ&)
{
      ਵਾਪਸ fun_thr->ਦਲੀਲਾਂ_real[ਸੂਚਕਾਂਕ];
}

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ get_ਡੂੰਘਾਈ(vthread_t fun_thr, size_t ਸੂਚਕਾਂਕ, string&)
{
      ਵਾਪਸ fun_thr->ਦਲੀਲਾਂ_str[ਸੂਚਕਾਂਕ];
}

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ get_ਡੂੰਘਾਈ(vthread_t fun_thr, size_t ਸੂਚਕਾਂਕ, vvp_vector4_t&)
{
      ਵਾਪਸ fun_thr->ਦਲੀਲਾਂ_vec4[ਸੂਚਕਾਂਕ];
}

ਸਥਿਰ vthread_t get_func(vthread_t thr)
{
      vthread_t fun_thr = thr;

      ਜਦੋਂ (fun_thr->ਮਾਪਾ_scope->get_type_ਕੋਡ() != vpiFunction) {
	    ਪ੍ਰਤਿਜ੍ਞਾ(fun_thr->ਮਾਪਾ);
	    fun_thr = fun_thr->ਮਾਪਾ;
      }

      ਵਾਪਸ fun_thr;
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ ELEM>
ਸਥਿਰ ਤਾਰਕਿਕ ret(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      size_t ਸੂਚਕਾਂਕ = cp->ਸੰਖਿਆ;
      ELEM val;
      ਕੱਢੋ_ਮੁੱਲ(thr, val, 0);

      vthread_t fun_thr = get_func(thr);
      ਪ੍ਰਤਿਜ੍ਞਾ(ਸੂਚਕਾਂਕ < get_max(fun_thr, val));

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਡੂੰਘਾਈ = get_ਡੂੰਘਾਈ(fun_thr, ਸੂਚਕਾਂਕ, val);
	// Use the depth to put the value into the stack of
	// the parent thread.
      poke_val(fun_thr, ਡੂੰਘਾਈ, val);
      ਵਾਪਸ ਸੱਚ;
}

/*
 * %ret/real <index>
 */
ਤਾਰਕਿਕ ਦਾ_RET_REAL(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ ret<ਦੁੱਗਣਾ>(thr, cp);
}

/*
 * %ret/str <index>
 */
ਤਾਰਕਿਕ ਦਾ_RET_STR(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ ret<string>(thr, cp);
}

/*
 * %ret/vec4 <index>, <offset>, <wid>
 */
ਤਾਰਕਿਕ ਦਾ_RET_VEC4(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      size_t ਸੂਚਕਾਂਕ = cp->ਸੰਖਿਆ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ off_ਸੂਚਕਾਂਕ = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ wid = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[1];
      vvp_vector4_t&val = thr->peek_vec4();

      vthread_t fun_thr = get_func(thr);
      ਪ੍ਰਤਿਜ੍ਞਾ(ਸੂਚਕਾਂਕ < get_max(fun_thr, val));
      ਪ੍ਰਤਿਜ੍ਞਾ(val.size() == wid);
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਡੂੰਘਾਈ = get_ਡੂੰਘਾਈ(fun_thr, ਸੂਚਕਾਂਕ, val);

      int64_t off = off_ਸੂਚਕਾਂਕ ? thr->ਸ਼ਬਦ[off_ਸੂਚਕਾਂਕ].w_int : 0;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ sig_ਮੁੱਲ_ਮਾਪ = fun_thr->ਮਾਪਾ->peek_vec4(ਡੂੰਘਾਈ).size();

      ਜੇ (off_ਸੂਚਕਾਂਕ!=0 && thr->flags[4] == BIT4_1) {
	    thr->ਕੱਢੋ_vec4(1);
	    ਵਾਪਸ ਸੱਚ;
      }

      ਜੇ (!resize_rval_vec(val, off, sig_ਮੁੱਲ_ਮਾਪ)) {
	    thr->ਕੱਢੋ_vec4(1);
	    ਵਾਪਸ ਸੱਚ;
      }

      ਜੇ (off == 0 && val.size() == sig_ਮੁੱਲ_ਮਾਪ) {
	    fun_thr->ਮਾਪਾ->poke_vec4(ਡੂੰਘਾਈ, val);
      } ਨਹੀਂਤਾਂ {
	    vvp_vector4_t tmp_dst = fun_thr->ਮਾਪਾ->peek_vec4(ਡੂੰਘਾਈ);
	    tmp_dst.ਸੈੱਟ_vec(off, val);
	    fun_thr->ਮਾਪਾ->poke_vec4(ਡੂੰਘਾਈ, tmp_dst);
      }

      thr->ਕੱਢੋ_vec4(1);
      ਵਾਪਸ ਸੱਚ;
}

ਸਥਿਰ ਖਾਲੀ ਧੱਕੋ_from_ਮਾਪਾ(vthread_t thr, vthread_t fun_thr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਡੂੰਘਾਈ, ਦੁੱਗਣਾ&)
{
      thr->ਧੱਕੋ_real(fun_thr->ਮਾਪਾ->peek_real(ਡੂੰਘਾਈ));
}

ਸਥਿਰ ਖਾਲੀ ਧੱਕੋ_from_ਮਾਪਾ(vthread_t thr, vthread_t fun_thr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਡੂੰਘਾਈ, string&)
{
      thr->ਧੱਕੋ_str(fun_thr->ਮਾਪਾ->peek_str(ਡੂੰਘਾਈ));
}

ਸਥਿਰ ਖਾਲੀ ਧੱਕੋ_from_ਮਾਪਾ(vthread_t thr, vthread_t fun_thr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਡੂੰਘਾਈ, vvp_vector4_t&)
{
      thr->ਧੱਕੋ_vec4(fun_thr->ਮਾਪਾ->peek_vec4(ਡੂੰਘਾਈ));
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ ELEM>
ਸਥਿਰ ਤਾਰਕਿਕ retload(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      size_t ਸੂਚਕਾਂਕ = cp->ਸੰਖਿਆ;
      ELEM type;

      vthread_t fun_thr = get_func(thr);
      ਪ੍ਰਤਿਜ੍ਞਾ(ਸੂਚਕਾਂਕ < get_max(fun_thr, type));

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਡੂੰਘਾਈ = get_ਡੂੰਘਾਈ(fun_thr, ਸੂਚਕਾਂਕ, type);
	// Use the depth to extract the values from the stack
	// of the parent thread.
      ਧੱਕੋ_from_ਮਾਪਾ(thr, fun_thr, ਡੂੰਘਾਈ, type);
      ਵਾਪਸ ਸੱਚ;
}

/*
 * %retload/real <index>
 */
ਤਾਰਕਿਕ ਦਾ_RETLOAD_REAL(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ retload<ਦੁੱਗਣਾ>(thr, cp);
}

/*
 * %retload/str <index>
 */
ਤਾਰਕਿਕ ਦਾ_RETLOAD_STR(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ retload<string>(thr, cp);
}

/*
 * %retload/vec4 <index>
 */
ਤਾਰਕਿਕ ਦਾ_RETLOAD_VEC4(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ retload<vvp_vector4_t>(thr, cp);
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
ਤਾਰਕਿਕ ਦਾ_SCOPY(vthread_t thr, vvp_ਕੋਡ_t)
{
      vvp_object_t tmp;
      thr->ਕੱਢੋ_object(tmp);

      vvp_object_t&dest = thr->peek_object();
        // If it is null there is nothing to copy
      ਜੇ (!tmp.test_nil())
	    dest.shallow_copy(tmp);

      ਵਾਪਸ ਸੱਚ;
}

ਸਥਿਰ ਖਾਲੀ thread_peek(vthread_t thr, ਦੁੱਗਣਾ&value)
{
      value = thr->peek_real(0);
}

ਸਥਿਰ ਖਾਲੀ thread_peek(vthread_t thr, string&value)
{
      value = thr->peek_str(0);
}

ਸਥਿਰ ਖਾਲੀ thread_peek(vthread_t thr, vvp_vector4_t&value)
{
      value = thr->peek_vec4(0);
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ ELEM>
ਸਥਿਰ ਤਾਰਕਿਕ ਸੈੱਟ_dar_obj(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ adr = thr->ਸ਼ਬਦ[cp->ਸੰਖਿਆ].w_int;

      ELEM value;
      thread_peek(thr, value);

      vvp_object_t&ਸਿਖਰ = thr->peek_object();
      vvp_darray*darray = ਸਿਖਰ.peek<vvp_darray>();
      ਪ੍ਰਤਿਜ੍ਞਾ(darray);

      darray->ਸੈੱਟ_ਸ਼ਬਦ(adr, value);
      ਵਾਪਸ ਸੱਚ;
}

/*
 * %set/dar/obj/real <index>
 */
ਤਾਰਕਿਕ ਦਾ_ਸੈੱਟ_DAR_OBJ_REAL(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ ਸੈੱਟ_dar_obj<ਦੁੱਗਣਾ>(thr, cp);
}

/*
 * %set/dar/obj/str <index>
 */
ਤਾਰਕਿਕ ਦਾ_ਸੈੱਟ_DAR_OBJ_STR(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ ਸੈੱਟ_dar_obj<string>(thr, cp);
}

/*
 * %set/dar/obj/vec4 <index>
 */
ਤਾਰਕਿਕ ਦਾ_ਸੈੱਟ_DAR_OBJ_VEC4(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ ਸੈੱਟ_dar_obj<vvp_vector4_t>(thr, cp);
}

/*
 * %shiftl <idx>
 *
 * Pop the operand, then push the result.
 */
ਤਾਰਕਿਕ ਦਾ_SHIFTL(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਪੂਰਨ use_ਸੂਚਕਾਂਕ = cp->ਸੰਖਿਆ;
      uint64_t ਖਿਸਕਾਓ = thr->ਸ਼ਬਦ[use_ਸੂਚਕਾਂਕ].w_uint;

      vvp_vector4_t&val = thr->peek_vec4();
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid  = val.size();

      ਜੇ (thr->flags[4] == BIT4_1) {
	      // The result is 'bx if the shift amount is undefined
	    val = vvp_vector4_t(wid, BIT4_X);

      } ਨਹੀਂਤਾਂ ਜੇ (thr->flags[4] == BIT4_X || ਖਿਸਕਾਓ >= wid) {
	      // Shift is so big that all value is shifted out. Write
	      // a constant 0 result.
	    val = vvp_vector4_t(wid, BIT4_0);

      } ਨਹੀਂਤਾਂ ਜੇ (ਖਿਸਕਾਓ > 0) {
	    vvp_vector4_t blk = val.subvalue(0, wid-ਖਿਸਕਾਓ);
	    vvp_vector4_t tmp (ਖਿਸਕਾਓ, BIT4_0);
	    val.ਸੈੱਟ_vec(0, tmp);
	    val.ਸੈੱਟ_vec(ਖਿਸਕਾਓ, blk);
      }

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %shiftr <idx>
 * This is an unsigned right shift. The <idx> is a number that selects
 * the index register with the amount of the shift. This instruction
 * checks flag bit 4, which will be true if the shift is invalid.
 */
ਤਾਰਕਿਕ ਦਾ_SHIFTR(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਪੂਰਨ use_ਸੂਚਕਾਂਕ = cp->ਸੰਖਿਆ;
      uint64_t ਖਿਸਕਾਓ = thr->ਸ਼ਬਦ[use_ਸੂਚਕਾਂਕ].w_uint;

      vvp_vector4_t val = thr->ਕੱਢੋ_vec4();
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid  = val.size();

      ਜੇ (thr->flags[4] == BIT4_1) {
	    val = vvp_vector4_t(wid, BIT4_X);

      } ਨਹੀਂਤਾਂ ਜੇ (thr->flags[4] == BIT4_X || ਖਿਸਕਾਓ > wid) {
	    val = vvp_vector4_t(wid, BIT4_0);

      } ਨਹੀਂਤਾਂ ਜੇ (ਖਿਸਕਾਓ > 0) {
	    vvp_vector4_t blk = val.subvalue(ਖਿਸਕਾਓ, wid-ਖਿਸਕਾਓ);
	    vvp_vector4_t tmp (ਖਿਸਕਾਓ, BIT4_0);
	    val.ਸੈੱਟ_vec(0, blk);
	    val.ਸੈੱਟ_vec(wid-ਖਿਸਕਾਓ, tmp);
      }

      thr->ਧੱਕੋ_vec4(val);
      ਵਾਪਸ ਸੱਚ;
}

/*
 *  %shiftr/s <wid>
 */
ਤਾਰਕਿਕ ਦਾ_SHIFTR_S(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਪੂਰਨ use_ਸੂਚਕਾਂਕ = cp->ਸੰਖਿਆ;
      uint64_t ਖਿਸਕਾਓ = thr->ਸ਼ਬਦ[use_ਸੂਚਕਾਂਕ].w_uint;

      vvp_vector4_t val = thr->ਕੱਢੋ_vec4();
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid  = val.size();

      vvp_bit4_t sign_ਦੋਆਧਾਰੀ_ਅੰਕ = val.value(val.size()-1);

      ਜੇ (thr->flags[4] == BIT4_1) {
	    val = vvp_vector4_t(wid, BIT4_X);

      } ਨਹੀਂਤਾਂ ਜੇ (thr->flags[4] == BIT4_X || ਖਿਸਕਾਓ > wid) {
	    val = vvp_vector4_t(wid, sign_ਦੋਆਧਾਰੀ_ਅੰਕ);

      } ਨਹੀਂਤਾਂ ਜੇ (ਖਿਸਕਾਓ > 0) {
	    vvp_vector4_t blk = val.subvalue(ਖਿਸਕਾਓ, wid-ਖਿਸਕਾਓ);
	    vvp_vector4_t tmp (ਖਿਸਕਾਓ, sign_ਦੋਆਧਾਰੀ_ਅੰਕ);
	    val.ਸੈੱਟ_vec(0, blk);
	    val.ਸੈੱਟ_vec(wid-ਖਿਸਕਾਓ, tmp);
      }

      thr->ਧੱਕੋ_vec4(val);
      ਵਾਪਸ ਸੱਚ;
}

/*
 * %split/vec4 <wid>
 *   Pop 1 value,
 *   Take <wid> bits from the lsb,
 *   Push the remaining msb,
 *   Push the lsb.
 */
ਤਾਰਕਿਕ ਦਾ_ਵੰਡੋ_VEC4(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ lsb_wid = cp->ਸੰਖਿਆ;

      vvp_vector4_t&val = thr->peek_vec4();
      ਪ੍ਰਤਿਜ੍ਞਾ(lsb_wid < val.size());

      vvp_vector4_t lsb = val.subvalue(0, lsb_wid);
      val = val.subvalue(lsb_wid, val.size()-lsb_wid);

      thr->ਧੱਕੋ_vec4(lsb);
      ਵਾਪਸ ਸੱਚ;
}

/*
 * The following are used to allow the darray templates to print correctly.
 */
ਸਤਰਅੰਦਰ ਸਥਿਰ string get_darray_type(ਦੁੱਗਣਾ&)
{
      ਵਾਪਸ "darray<real>";
}

ਸਤਰਅੰਦਰ ਸਥਿਰ string get_darray_type(string&)
{
      ਵਾਪਸ "darray<string>";
}

ਸਤਰਅੰਦਰ ਸਥਿਰ string get_darray_type(ਅਚਲ vvp_vector4_t&value)
{
      ostringstream buf;
      buf << "darray<vector[" << value.size() << "]>";
      string res = buf.str();
      ਵਾਪਸ res;
}

/*
 * The following are used to allow a common template to be written for
 * darray real/string/vec4 operations
 */
ਸਤਰਅੰਦਰ ਸਥਿਰ ਖਾਲੀ dar_ਕੱਢੋ_ਮੁੱਲ(vthread_t thr, ਦੁੱਗਣਾ&value)
{
      value = thr->ਕੱਢੋ_real();
}

ਸਤਰਅੰਦਰ ਸਥਿਰ ਖਾਲੀ dar_ਕੱਢੋ_ਮੁੱਲ(vthread_t thr, string&value)
{
      value = thr->ਕੱਢੋ_str();
}

ਸਤਰਅੰਦਰ ਸਥਿਰ ਖਾਲੀ dar_ਕੱਢੋ_ਮੁੱਲ(vthread_t thr, vvp_vector4_t&value)
{
      value = thr->ਕੱਢੋ_vec4();
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ ELEM>
ਸਥਿਰ ਤਾਰਕਿਕ store_dar(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      int64_t adr = thr->ਸ਼ਬਦ[3].w_int;
      ELEM value;
	// FIXME: Can we get the size of the underlying array element
	//        and then use the normal pop_value?
      dar_ਕੱਢੋ_ਮੁੱਲ(thr, value);

      vvp_net_t*net = cp->net;
      ਪ੍ਰਤਿਜ੍ਞਾ(net);

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      ਪ੍ਰਤਿਜ੍ਞਾ(obj);

      vvp_darray*darray = obj->get_object().peek<vvp_darray>();

      ਜੇ (adr < 0)
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to a negative " << get_darray_type(value)
	         << " index (" << adr << ")." << ਸਤਰ_ਅੰਤ;
      ਨਹੀਂਤਾਂ ਜੇ (thr->flags[4] != BIT4_0)
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to an undefined " << get_darray_type(value)
	         << " index." << ਸਤਰ_ਅੰਤ;
      ਨਹੀਂਤਾਂ ਜੇ (darray)
	    darray->ਸੈੱਟ_ਸ਼ਬਦ(adr, value);
      ਨਹੀਂਤਾਂ
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to an undefined " << get_darray_type(value)
	         << "." << ਸਤਰ_ਅੰਤ;

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %store/dar/real <var>
 */
ਤਾਰਕਿਕ ਦਾ_STORE_DAR_R(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ store_dar<ਦੁੱਗਣਾ>(thr, cp);
}

/*
 * %store/dar/str <var>
 */
ਤਾਰਕਿਕ ਦਾ_STORE_DAR_STR(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ store_dar<string>(thr, cp);
}

/*
 * %store/dar/vec4 <var>
 */
ਤਾਰਕਿਕ ਦਾ_STORE_DAR_VEC4(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ store_dar<vvp_vector4_t>(thr, cp);
}

ਤਾਰਕਿਕ ਦਾ_STORE_OBJ(vthread_t thr, vvp_ਕੋਡ_t cp)
{
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);

      vvp_object_t val;
      thr->ਕੱਢੋ_object(val);

      vvp_send_object(ptr, val, thr->wt_context);

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %store/obja <array-label> <index>
 */
ਤਾਰਕਿਕ ਦਾ_STORE_OBJA(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ idx = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      ਬਿਨਾਨਿਸ਼ਾਨੀ adr = thr->ਸ਼ਬਦ[idx].w_int;

      vvp_object_t val;
      thr->ਕੱਢੋ_object(val);

      cp->array->ਸੈੱਟ_ਸ਼ਬਦ(adr, val);

      ਵਾਪਸ ਸੱਚ;
}


/*
 * %store/prop/obj <pid>, <idx>
 *
 * Pop an object value from the object stack, and store the value into
 * the property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
ਤਾਰਕਿਕ ਦਾ_STORE_PROP_OBJ(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      size_t pid = cp->ਸੰਖਿਆ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ idx = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];

      ਜੇ (idx != 0) {
	    ਪ੍ਰਤਿਜ੍ਞਾ(idx < vthread_s::ਸ਼ਬਦ_ਗਿਣਤੀ);
	    idx = thr->ਸ਼ਬਦ[idx].w_uint;
      }

      vvp_object_t val;
      thr->ਕੱਢੋ_object(val);

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      ਪ੍ਰਤਿਜ੍ਞਾ(cobj);

      cobj->ਸੈੱਟ_object(pid, val, idx);

      ਵਾਪਸ ਸੱਚ;
}

ਸਥਿਰ ਖਾਲੀ ਕੱਢੋ_prop_val(vthread_t thr, ਦੁੱਗਣਾ&val, ਬਿਨਾਨਿਸ਼ਾਨੀ)
{
      val = thr->ਕੱਢੋ_real();
}

ਸਥਿਰ ਖਾਲੀ ਕੱਢੋ_prop_val(vthread_t thr, string&val, ਬਿਨਾਨਿਸ਼ਾਨੀ)
{
      val = thr->ਕੱਢੋ_str();
}

ਸਥਿਰ ਖਾਲੀ ਕੱਢੋ_prop_val(vthread_t thr, vvp_vector4_t&val, ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
{
      val = thr->ਕੱਢੋ_vec4();
      ਪ੍ਰਤਿਜ੍ਞਾ(val.size() >= wid);
      val.resize(wid);
}

ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_val(vvp_cobject*cobj, size_t pid, ਅਚਲ ਦੁੱਗਣਾ&val)
{
      cobj->ਸੈੱਟ_real(pid, val);
}

ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_val(vvp_cobject*cobj, size_t pid, ਅਚਲ string&val)
{
      cobj->ਸੈੱਟ_string(pid, val);
}

ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_val(vvp_cobject*cobj, size_t pid, ਅਚਲ vvp_vector4_t&val)
{
      cobj->ਸੈੱਟ_vec4(pid, val);
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ ELEM>
ਸਥਿਰ ਤਾਰਕਿਕ store_prop(vthread_t thr, vvp_ਕੋਡ_t cp, ਬਿਨਾਨਿਸ਼ਾਨੀ wid=0)
{
      size_t pid = cp->ਸੰਖਿਆ;
      ELEM val;
      ਕੱਢੋ_prop_val(thr, val, wid); // Pop the value to store.

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      ਪ੍ਰਤਿਜ੍ਞਾ(cobj);

      ਸੈੱਟ_val(cobj, pid, val);

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %store/prop/r <id>
 *
 * Pop a real value from the real stack, and store the value into the
 * property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
ਤਾਰਕਿਕ ਦਾ_STORE_PROP_R(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ store_prop<ਦੁੱਗਣਾ>(thr, cp);
}

/*
 * %store/prop/str <id>
 *
 * Pop a string value from the string stack, and store the value into
 * the property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
ਤਾਰਕਿਕ ਦਾ_STORE_PROP_STR(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ store_prop<string>(thr, cp);
}

/*
 * %store/prop/v <pid>, <wid>
 *
 * Store vector value into property <id> of cobject in the top of the
 * stack. Do NOT pop the object stack.
 */
ਤਾਰਕਿਕ ਦਾ_STORE_PROP_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ store_prop<vvp_vector4_t>(thr, cp, cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0]);
}

ਸਾਂਚਾ <ਵਰਗ QTYPE>
ਸਥਿਰ QTYPE* get_ਕਤਾਰ_prop(vvp_cobject*cobj, size_t pid)
{
      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      QTYPE* ਕਤਾਰ = qobj.peek<QTYPE>();
      ਜੇ (ਕਤਾਰ == 0) {
	    ਕਤਾਰ = ਨਵਾਂ QTYPE;
	    vvp_object_t val(ਕਤਾਰ);
	    cobj->ਸੈੱਟ_object(pid, val, 0);
      }
      ਵਾਪਸ ਕਤਾਰ;
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ ELEM, ਵਰਗ QTYPE>
ਸਥਿਰ ਤਾਰਕਿਕ store_prop_qb(vthread_t thr, vvp_ਕੋਡ_t cp, ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
{
      size_t pid = cp->ਸੰਖਿਆ;
      ELEM value;
      ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ = thr->ਸ਼ਬਦ[cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0]].w_uint;
      ਕੱਢੋ_ਮੁੱਲ(thr, value, wid);

      vvp_object_t& ਸਿਖਰ = thr->peek_object();
      vvp_cobject*cobj = ਸਿਖਰ.peek<vvp_cobject>();
      ਪ੍ਰਤਿਜ੍ਞਾ(cobj);

      QTYPE* ਕਤਾਰ = get_ਕਤਾਰ_prop<QTYPE>(cobj, pid);
      ਪ੍ਰਤਿਜ੍ਞਾ(ਕਤਾਰ);
      ਕਤਾਰ->push_back(value, max_ਮਾਪ);
      ਵਾਪਸ ਸੱਚ;
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ ELEM, ਵਰਗ QTYPE>
ਸਥਿਰ ਤਾਰਕਿਕ store_prop_qf(vthread_t thr, vvp_ਕੋਡ_t cp, ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
{
      size_t pid = cp->ਸੰਖਿਆ;
      ELEM value;
      ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ = thr->ਸ਼ਬਦ[cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0]].w_uint;
      ਕੱਢੋ_ਮੁੱਲ(thr, value, wid);

      vvp_object_t& ਸਿਖਰ = thr->peek_object();
      vvp_cobject*cobj = ਸਿਖਰ.peek<vvp_cobject>();
      ਪ੍ਰਤਿਜ੍ਞਾ(cobj);

      QTYPE* ਕਤਾਰ = get_ਕਤਾਰ_prop<QTYPE>(cobj, pid);
      ਪ੍ਰਤਿਜ੍ਞਾ(ਕਤਾਰ);
      ਕਤਾਰ->push_front(value, max_ਮਾਪ);
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_STORE_PROP_QB_R(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ store_prop_qb<ਦੁੱਗਣਾ, vvp_ਕਤਾਰ_real>(thr, cp, 0);
}

ਤਾਰਕਿਕ ਦਾ_STORE_PROP_QB_STR(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ store_prop_qb<string, vvp_ਕਤਾਰ_string>(thr, cp, 0);
}

ਤਾਰਕਿਕ ਦਾ_STORE_PROP_QB_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ store_prop_qb<vvp_vector4_t, vvp_ਕਤਾਰ_vec4>(thr, cp, cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[1]);
}

ਤਾਰਕਿਕ ਦਾ_STORE_PROP_QF_R(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ store_prop_qf<ਦੁੱਗਣਾ, vvp_ਕਤਾਰ_real>(thr, cp, 0);
}

ਤਾਰਕਿਕ ਦਾ_STORE_PROP_QF_STR(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ store_prop_qf<string, vvp_ਕਤਾਰ_string>(thr, cp, 0);
}

ਤਾਰਕਿਕ ਦਾ_STORE_PROP_QF_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ store_prop_qf<vvp_vector4_t, vvp_ਕਤਾਰ_vec4>(thr, cp, cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[1]);
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ ELEM, ਵਰਗ QTYPE>
ਸਥਿਰ ਤਾਰਕਿਕ qinsert_prop(vthread_t thr, vvp_ਕੋਡ_t cp, ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
{
      int64_t idx = thr->ਸ਼ਬਦ[3].w_int;
      ELEM value;
      size_t pid = cp->ਸੰਖਿਆ;
      ਕੱਢੋ_ਮੁੱਲ(thr, value, wid);

      vvp_object_t& ਸਿਖਰ = thr->peek_object();
      vvp_cobject*cobj = ਸਿਖਰ.peek<vvp_cobject>();
      ਪ੍ਰਤਿਜ੍ਞਾ(cobj);

      QTYPE* ਕਤਾਰ = get_ਕਤਾਰ_prop<QTYPE>(cobj, pid);
      ਪ੍ਰਤਿਜ੍ਞਾ(ਕਤਾਰ);
      ਜੇ (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at a negative "
	         << get_ਕਤਾਰ_type(value)
	         << " index (" << idx << "). ";
	    print_ਕਤਾਰ_ਮੁੱਲ(value);
	    cerr << " was not added." << ਸਤਰ_ਅੰਤ;
      } ਨਹੀਂਤਾਂ ਜੇ (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at an undefined "
	         << get_ਕਤਾਰ_type(value) << " index. ";
	    print_ਕਤਾਰ_ਮੁੱਲ(value);
	    cerr << " was not added." << ਸਤਰ_ਅੰਤ;
      } ਨਹੀਂਤਾਂ {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ = thr->ਸ਼ਬਦ[cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0]].w_int;
	    ਕਤਾਰ->insert(idx, value, max_ਮਾਪ);
      }
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_QINSERT_PROP_R(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ qinsert_prop<ਦੁੱਗਣਾ, vvp_ਕਤਾਰ_real>(thr, cp, 0);
}

ਤਾਰਕਿਕ ਦਾ_QINSERT_PROP_STR(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ qinsert_prop<string, vvp_ਕਤਾਰ_string>(thr, cp, 0);
}

ਤਾਰਕਿਕ ਦਾ_QINSERT_PROP_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ qinsert_prop<vvp_vector4_t, vvp_ਕਤਾਰ_vec4>(thr, cp, cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[1]);
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ ELEM, ਵਰਗ QTYPE>
ਸਥਿਰ ਤਾਰਕਿਕ q_ਕੱਢੋ_prop(vthread_t thr, vvp_ਕੋਡ_t cp,
                       ਖਾਲੀ (*get_val_func)(vvp_ਕਤਾਰ*, ELEM&),
                       ਅਚਲ ਅੱਖਰ*loc, ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
{
      size_t pid = cp->ਸੰਖਿਆ;

      vvp_object_t& ਸਿਖਰ = thr->peek_object();
      vvp_cobject*cobj = ਸਿਖਰ.peek<vvp_cobject>();
      ਪ੍ਰਤਿਜ੍ਞਾ(cobj);

      QTYPE* ਕਤਾਰ = get_ਕਤਾਰ_prop<QTYPE>(cobj, pid);
      ਪ੍ਰਤਿਜ੍ਞਾ(ਕਤਾਰ);

      size_t size = ਕਤਾਰ->get_ਮਾਪ();

      ELEM value;
      ਜੇ (size) {
	    get_val_func(ਕਤਾਰ, value);
      } ਨਹੀਂਤਾਂ {
	    dq_default(value, wid);
	    cerr << thr->get_fileline()
	         << "Warning: pop_" << loc << "() on empty "
	         << get_ਕਤਾਰ_type(value) << "." << ਸਤਰ_ਅੰਤ;
      }

      ਧੱਕੋ_ਮੁੱਲ(thr, value, wid);
      ਵਾਪਸ ਸੱਚ;
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ ELEM, ਵਰਗ QTYPE>
ਸਥਿਰ ਤਾਰਕਿਕ qpop_b_prop(vthread_t thr, vvp_ਕੋਡ_t cp, ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
{
      ਵਾਪਸ q_ਕੱਢੋ_prop<ELEM, QTYPE>(thr, cp, get_ਪਿੱਠ_ਮੁੱਲ<ELEM>, "back", wid);
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ ELEM, ਵਰਗ QTYPE>
ਸਥਿਰ ਤਾਰਕਿਕ qpop_f_prop(vthread_t thr, vvp_ਕੋਡ_t cp, ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
{
      ਵਾਪਸ q_ਕੱਢੋ_prop<ELEM, QTYPE>(thr, cp, get_front_ਮੁੱਲ<ELEM>, "front", wid);
}

ਤਾਰਕਿਕ ਦਾ_QPOP_PROP_B_REAL(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ qpop_b_prop<ਦੁੱਗਣਾ, vvp_ਕਤਾਰ_real>(thr, cp, 0);
}

ਤਾਰਕਿਕ ਦਾ_QPOP_PROP_B_STR(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ qpop_b_prop<string, vvp_ਕਤਾਰ_string>(thr, cp, 0);
}

ਤਾਰਕਿਕ ਦਾ_QPOP_PROP_B_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ qpop_b_prop<vvp_vector4_t, vvp_ਕਤਾਰ_vec4>(thr, cp, cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0]);
}

ਤਾਰਕਿਕ ਦਾ_QPOP_PROP_F_REAL(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ qpop_f_prop<ਦੁੱਗਣਾ, vvp_ਕਤਾਰ_real>(thr, cp, 0);
}

ਤਾਰਕਿਕ ਦਾ_QPOP_PROP_F_STR(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ qpop_f_prop<string, vvp_ਕਤਾਰ_string>(thr, cp, 0);
}

ਤਾਰਕਿਕ ਦਾ_QPOP_PROP_F_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ qpop_f_prop<vvp_vector4_t, vvp_ਕਤਾਰ_vec4>(thr, cp, cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0]);
}

ਸਥਿਰ vvp_vector4_t ਕਤਾਰ_unique_ulong_ਮੰਜ਼ਿਲ_vec4(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul, ਬਿਨਾਨਿਸ਼ਾਨੀ ਬਿੱਟ)
{
      vvp_vector4_t val(ਬਿੱਟ, BIT4_0);
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0; idx < ਬਿੱਟ && idx < 8 * ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ); idx += 1) {
	    ਜੇ (ul & 1UL) {
		  val.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_1);
	    } ਨਹੀਂਤਾਂ {
		  val.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      ਵਾਪਸ val;
}

ਸਾਂਚਾ <ਵਰਗ SRC> ਸਥਿਰ vvp_ਕਤਾਰ_vec4* ਕਤਾਰ_ਲੜੀਵਾਰ_unique_src(SRC* src,
								 ਬਿਨਾਨਿਸ਼ਾਨੀ wid,
								 ਤਾਰਕਿਕ as_ਸੂਚਕਾਂਕ)
{
      vvp_ਕਤਾਰ_vec4* dst = ਨਵਾਂ vvp_ਕਤਾਰ_vec4();
      ਜੇ (!src) ਵਾਪਸ dst;
      size_t n = src->get_ਮਾਪ();
      ਲਈ (size_t i = 0; i < n; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_ਸ਼ਬਦ(i, vi);
	    ਤਾਰਕਿਕ ਵੇਖਿਆ = ਝੂਠ;
	    ਲਈ (size_t j = 0; j < i; j += 1) {
		  vvp_vector4_t vj(wid);
		  src->get_ਸ਼ਬਦ(j, vj);
		  ਜੇ (vi.eeq(vj)) {
			ਵੇਖਿਆ = ਸੱਚ;
			ਰੋਕੋ;
		  }
	    }
	    ਜੇ (ਵੇਖਿਆ) ਜਾਰੀ;
	    ਜੇ (as_ਸੂਚਕਾਂਕ) {
		  dst->push_back(ਕਤਾਰ_unique_ulong_ਮੰਜ਼ਿਲ_vec4((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)i, 32), 0);
	    } ਨਹੀਂਤਾਂ {
		  dst->push_back(vi, 0);
	    }
      }
      ਵਾਪਸ dst;
}

ਸਾਂਚਾ <ਵਰਗ SRC>
ਸਥਿਰ vvp_ਕਤਾਰ_vec4* ਕਤਾਰ_ਲੜੀਵਾਰ_ਲੱਭੋ_matches_src(SRC* src, ਬਿਨਾਨਿਸ਼ਾਨੀ wid,
						  ਅਚਲ vvp_vector4_t& cmp,
						  ਤਾਰਕਿਕ as_ਸੂਚਕਾਂਕ)
{
      vvp_ਕਤਾਰ_vec4* dst = ਨਵਾਂ vvp_ਕਤਾਰ_vec4();
      ਜੇ (!src) ਵਾਪਸ dst;
      size_t n = src->get_ਮਾਪ();
      ਲਈ (size_t i = 0; i < n; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_ਸ਼ਬਦ(i, vi);
	    ਜੇ (vi.eeq(cmp)) {
		  ਜੇ (as_ਸੂਚਕਾਂਕ) {
			dst->push_back(ਕਤਾਰ_unique_ulong_ਮੰਜ਼ਿਲ_vec4((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)i, 32), 0);
		  } ਨਹੀਂਤਾਂ {
			dst->push_back(vi, 0);
		  }
	    }
      }
      ਵਾਪਸ dst;
}

/* Array/queue locator methods return a queue with 0 or 1 elements (LRM). */
ਸਾਂਚਾ <ਵਰਗ SRC>
ਸਥਿਰ vvp_ਕਤਾਰ_vec4* ਕਤਾਰ_ਲੱਭੋ_ਪਹਿਲਾ_ਆਖਰੀ_ਮੰਜ਼ਿਲ_ਕਤਾਰ_src(SRC* src,
							  ਬਿਨਾਨਿਸ਼ਾਨੀ wid,
							  ਅਚਲ vvp_vector4_t& cmp,
							  ਤਾਰਕਿਕ want_ਪਹਿਲਾ,
							  ਤਾਰਕਿਕ want_ਸੂਚਕਾਂਕ)
{
      vvp_ਕਤਾਰ_vec4* dst = ਨਵਾਂ vvp_ਕਤਾਰ_vec4();
      ਜੇ (!src || src->get_ਮਾਪ() == 0) ਵਾਪਸ dst;
      size_t n = src->get_ਮਾਪ();
      ਜੇ (want_ਪਹਿਲਾ) {
	    ਲਈ (size_t i = 0; i < n; i += 1) {
		  vvp_vector4_t vi(wid);
		  src->get_ਸ਼ਬਦ(i, vi);
		  ਜੇ (vi.eeq(cmp)) {
			ਜੇ (want_ਸੂਚਕਾਂਕ) {
			      dst->push_back(
				  ਕਤਾਰ_unique_ulong_ਮੰਜ਼ਿਲ_vec4((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)i, 32),
				  0);
			} ਨਹੀਂਤਾਂ {
			      dst->push_back(vi, 0);
			}
			ਵਾਪਸ dst;
		  }
	    }
      } ਨਹੀਂਤਾਂ {
	    ਲਈ (size_t i = n; i > 0; ) {
		  i -= 1;
		  vvp_vector4_t vi(wid);
		  src->get_ਸ਼ਬਦ(i, vi);
		  ਜੇ (vi.eeq(cmp)) {
			ਜੇ (want_ਸੂਚਕਾਂਕ) {
			      dst->push_back(
				  ਕਤਾਰ_unique_ulong_ਮੰਜ਼ਿਲ_vec4((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)i, 32),
				  0);
			} ਨਹੀਂਤਾਂ {
			      dst->push_back(vi, 0);
			}
			ਵਾਪਸ dst;
		  }
	    }
      }
      ਵਾਪਸ dst;
}

/* min()/max() return one element with the selected extremum. */
ਸਾਂਚਾ <ਵਰਗ SRC>
ਸਥਿਰ vvp_ਕਤਾਰ_vec4* ਕਤਾਰ_ਲੜੀਵਾਰ_min_max_src(SRC* src, ਬਿਨਾਨਿਸ਼ਾਨੀ wid, ਤਾਰਕਿਕ want_max)
{
      vvp_ਕਤਾਰ_vec4* dst = ਨਵਾਂ vvp_ਕਤਾਰ_vec4();
      ਜੇ (!src || src->get_ਮਾਪ() == 0) ਵਾਪਸ dst;

      vvp_vector4_t ਉੱਤਮ(wid);
      src->get_ਸ਼ਬਦ(0, ਉੱਤਮ);

      size_t n = src->get_ਮਾਪ();
      ਲਈ (size_t i = 1 ; i < n ; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_ਸ਼ਬਦ(i, vi);

	    vvp_bit4_t ge = compare_gtge(vi, ਉੱਤਮ, BIT4_1);
	    ਤਾਰਕਿਕ replace = want_max ? (ge == BIT4_1) : (ge == BIT4_0);
	    ਜੇ (replace) ਉੱਤਮ = vi;
      }

      dst->push_back(ਉੱਤਮ, 0);
      ਵਾਪਸ dst;
}

ਤਾਰਕਿਕ ਦਾ_ਕਤਾਰ_ਲੱਭੋ_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_vector4_t cmp = thr->ਕੱਢੋ_vec4();
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      ਪ੍ਰਤਿਜ੍ਞਾ(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_ਕਤਾਰ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ਕਤਾਰ_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_ਕਤਾਰ_vec4* dst;
      ਜੇ (qsrc) dst = ਕਤਾਰ_ਲੜੀਵਾਰ_ਲੱਭੋ_matches_src(qsrc, wid, cmp, ਝੂਠ);
      ਨਹੀਂਤਾਂ ਜੇ (dsrc) dst = ਕਤਾਰ_ਲੜੀਵਾਰ_ਲੱਭੋ_matches_src(dsrc, wid, cmp, ਝੂਠ);
      ਨਹੀਂਤਾਂ {
	    vvp_ਕਤਾਰ* src_q = get_ਕਤਾਰ_object<vvp_ਕਤਾਰ_vec4>(thr, net);
	    vvp_ਕਤਾਰ_vec4* src = dynamic_cast<vvp_ਕਤਾਰ_vec4*>(src_q);
	    dst = ਕਤਾਰ_ਲੜੀਵਾਰ_ਲੱਭੋ_matches_src(src, wid, cmp, ਝੂਠ);
      }
      thr->ਧੱਕੋ_object(vvp_object_t(dst));
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_ਕਤਾਰ_ਲੱਭੋ_PROP_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_vector4_t cmp = thr->ਕੱਢੋ_vec4();
      size_t pid = cp->ਸੰਖਿਆ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      ਪ੍ਰਤਿਜ੍ਞਾ(cmp.size() == wid);

      vvp_object_t& ਸਿਖਰ = thr->peek_object();
      vvp_cobject*cobj = ਸਿਖਰ.peek<vvp_cobject>();
      ਪ੍ਰਤਿਜ੍ਞਾ(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_ਕਤਾਰ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ਕਤਾਰ_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_ਕਤਾਰ_vec4* dst = qsrc ? ਕਤਾਰ_ਲੜੀਵਾਰ_ਲੱਭੋ_matches_src(qsrc, wid, cmp, ਝੂਠ)
				 : ਕਤਾਰ_ਲੜੀਵਾਰ_ਲੱਭੋ_matches_src(dsrc, wid, cmp, ਝੂਠ);
      thr->ਧੱਕੋ_object(vvp_object_t(dst));
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_ਕਤਾਰ_ਲੱਭੋ_ਸੂਚਕਾਂਕ_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_vector4_t cmp = thr->ਕੱਢੋ_vec4();
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      ਪ੍ਰਤਿਜ੍ਞਾ(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_ਕਤਾਰ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ਕਤਾਰ_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_ਕਤਾਰ_vec4* dst;
      ਜੇ (qsrc) dst = ਕਤਾਰ_ਲੜੀਵਾਰ_ਲੱਭੋ_matches_src(qsrc, wid, cmp, ਸੱਚ);
      ਨਹੀਂਤਾਂ ਜੇ (dsrc) dst = ਕਤਾਰ_ਲੜੀਵਾਰ_ਲੱਭੋ_matches_src(dsrc, wid, cmp, ਸੱਚ);
      ਨਹੀਂਤਾਂ {
	    vvp_ਕਤਾਰ* src_q = get_ਕਤਾਰ_object<vvp_ਕਤਾਰ_vec4>(thr, net);
	    vvp_ਕਤਾਰ_vec4* src = dynamic_cast<vvp_ਕਤਾਰ_vec4*>(src_q);
	    dst = ਕਤਾਰ_ਲੜੀਵਾਰ_ਲੱਭੋ_matches_src(src, wid, cmp, ਸੱਚ);
      }
      thr->ਧੱਕੋ_object(vvp_object_t(dst));
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_ਕਤਾਰ_ਲੱਭੋ_ਸੂਚਕਾਂਕ_PROP_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_vector4_t cmp = thr->ਕੱਢੋ_vec4();
      size_t pid = cp->ਸੰਖਿਆ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      ਪ੍ਰਤਿਜ੍ਞਾ(cmp.size() == wid);

      vvp_object_t& ਸਿਖਰ = thr->peek_object();
      vvp_cobject*cobj = ਸਿਖਰ.peek<vvp_cobject>();
      ਪ੍ਰਤਿਜ੍ਞਾ(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_ਕਤਾਰ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ਕਤਾਰ_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_ਕਤਾਰ_vec4* dst = qsrc ? ਕਤਾਰ_ਲੜੀਵਾਰ_ਲੱਭੋ_matches_src(qsrc, wid, cmp, ਸੱਚ)
				 : ਕਤਾਰ_ਲੜੀਵਾਰ_ਲੱਭੋ_matches_src(dsrc, wid, cmp, ਸੱਚ);
      thr->ਧੱਕੋ_object(vvp_object_t(dst));
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_ਕਤਾਰ_ਲੱਭੋ_ਪਹਿਲਾ_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_vector4_t cmp = thr->ਕੱਢੋ_vec4();
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      ਪ੍ਰਤਿਜ੍ਞਾ(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_ਕਤਾਰ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ਕਤਾਰ_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_ਕਤਾਰ_vec4* dst;
      ਜੇ (qsrc) dst = ਕਤਾਰ_ਲੱਭੋ_ਪਹਿਲਾ_ਆਖਰੀ_ਮੰਜ਼ਿਲ_ਕਤਾਰ_src(qsrc, wid, cmp, ਸੱਚ, ਝੂਠ);
      ਨਹੀਂਤਾਂ ਜੇ (dsrc) dst = ਕਤਾਰ_ਲੱਭੋ_ਪਹਿਲਾ_ਆਖਰੀ_ਮੰਜ਼ਿਲ_ਕਤਾਰ_src(dsrc, wid, cmp, ਸੱਚ, ਝੂਠ);
      ਨਹੀਂਤਾਂ {
	    vvp_ਕਤਾਰ* src_q = get_ਕਤਾਰ_object<vvp_ਕਤਾਰ_vec4>(thr, net);
	    vvp_ਕਤਾਰ_vec4* src = dynamic_cast<vvp_ਕਤਾਰ_vec4*>(src_q);
	    dst = ਕਤਾਰ_ਲੱਭੋ_ਪਹਿਲਾ_ਆਖਰੀ_ਮੰਜ਼ਿਲ_ਕਤਾਰ_src(src, wid, cmp, ਸੱਚ, ਝੂਠ);
      }
      thr->ਧੱਕੋ_object(vvp_object_t(dst));
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_ਕਤਾਰ_ਲੱਭੋ_ਪਹਿਲਾ_PROP_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_vector4_t cmp = thr->ਕੱਢੋ_vec4();
      size_t pid = cp->ਸੰਖਿਆ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      ਪ੍ਰਤਿਜ੍ਞਾ(cmp.size() == wid);

      vvp_object_t& ਸਿਖਰ = thr->peek_object();
      vvp_cobject*cobj = ਸਿਖਰ.peek<vvp_cobject>();
      ਪ੍ਰਤਿਜ੍ਞਾ(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_ਕਤਾਰ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ਕਤਾਰ_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_ਕਤਾਰ_vec4* dst =
	    qsrc ? ਕਤਾਰ_ਲੱਭੋ_ਪਹਿਲਾ_ਆਖਰੀ_ਮੰਜ਼ਿਲ_ਕਤਾਰ_src(qsrc, wid, cmp, ਸੱਚ, ਝੂਠ)
		 : ਕਤਾਰ_ਲੱਭੋ_ਪਹਿਲਾ_ਆਖਰੀ_ਮੰਜ਼ਿਲ_ਕਤਾਰ_src(dsrc, wid, cmp, ਸੱਚ, ਝੂਠ);
      thr->ਧੱਕੋ_object(vvp_object_t(dst));
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_ਕਤਾਰ_ਲੱਭੋ_ਪਹਿਲਾ_ਸੂਚਕਾਂਕ_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_vector4_t cmp = thr->ਕੱਢੋ_vec4();
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      ਪ੍ਰਤਿਜ੍ਞਾ(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_ਕਤਾਰ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ਕਤਾਰ_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_ਕਤਾਰ_vec4* dst;
      ਜੇ (qsrc) dst = ਕਤਾਰ_ਲੱਭੋ_ਪਹਿਲਾ_ਆਖਰੀ_ਮੰਜ਼ਿਲ_ਕਤਾਰ_src(qsrc, wid, cmp, ਸੱਚ, ਸੱਚ);
      ਨਹੀਂਤਾਂ ਜੇ (dsrc) dst = ਕਤਾਰ_ਲੱਭੋ_ਪਹਿਲਾ_ਆਖਰੀ_ਮੰਜ਼ਿਲ_ਕਤਾਰ_src(dsrc, wid, cmp, ਸੱਚ, ਸੱਚ);
      ਨਹੀਂਤਾਂ {
	    vvp_ਕਤਾਰ* src_q = get_ਕਤਾਰ_object<vvp_ਕਤਾਰ_vec4>(thr, net);
	    vvp_ਕਤਾਰ_vec4* src = dynamic_cast<vvp_ਕਤਾਰ_vec4*>(src_q);
	    dst = ਕਤਾਰ_ਲੱਭੋ_ਪਹਿਲਾ_ਆਖਰੀ_ਮੰਜ਼ਿਲ_ਕਤਾਰ_src(src, wid, cmp, ਸੱਚ, ਸੱਚ);
      }
      thr->ਧੱਕੋ_object(vvp_object_t(dst));
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_ਕਤਾਰ_ਲੱਭੋ_ਪਹਿਲਾ_ਸੂਚਕਾਂਕ_PROP_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_vector4_t cmp = thr->ਕੱਢੋ_vec4();
      size_t pid = cp->ਸੰਖਿਆ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      ਪ੍ਰਤਿਜ੍ਞਾ(cmp.size() == wid);

      vvp_object_t& ਸਿਖਰ = thr->peek_object();
      vvp_cobject*cobj = ਸਿਖਰ.peek<vvp_cobject>();
      ਪ੍ਰਤਿਜ੍ਞਾ(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_ਕਤਾਰ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ਕਤਾਰ_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_ਕਤਾਰ_vec4* dst =
	    qsrc ? ਕਤਾਰ_ਲੱਭੋ_ਪਹਿਲਾ_ਆਖਰੀ_ਮੰਜ਼ਿਲ_ਕਤਾਰ_src(qsrc, wid, cmp, ਸੱਚ, ਸੱਚ)
		 : ਕਤਾਰ_ਲੱਭੋ_ਪਹਿਲਾ_ਆਖਰੀ_ਮੰਜ਼ਿਲ_ਕਤਾਰ_src(dsrc, wid, cmp, ਸੱਚ, ਸੱਚ);
      thr->ਧੱਕੋ_object(vvp_object_t(dst));
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_ਕਤਾਰ_ਲੱਭੋ_ਆਖਰੀ_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_vector4_t cmp = thr->ਕੱਢੋ_vec4();
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      ਪ੍ਰਤਿਜ੍ਞਾ(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_ਕਤਾਰ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ਕਤਾਰ_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_ਕਤਾਰ_vec4* dst;
      ਜੇ (qsrc) dst = ਕਤਾਰ_ਲੱਭੋ_ਪਹਿਲਾ_ਆਖਰੀ_ਮੰਜ਼ਿਲ_ਕਤਾਰ_src(qsrc, wid, cmp, ਝੂਠ, ਝੂਠ);
      ਨਹੀਂਤਾਂ ਜੇ (dsrc) dst = ਕਤਾਰ_ਲੱਭੋ_ਪਹਿਲਾ_ਆਖਰੀ_ਮੰਜ਼ਿਲ_ਕਤਾਰ_src(dsrc, wid, cmp, ਝੂਠ, ਝੂਠ);
      ਨਹੀਂਤਾਂ {
	    vvp_ਕਤਾਰ* src_q = get_ਕਤਾਰ_object<vvp_ਕਤਾਰ_vec4>(thr, net);
	    vvp_ਕਤਾਰ_vec4* src = dynamic_cast<vvp_ਕਤਾਰ_vec4*>(src_q);
	    dst = ਕਤਾਰ_ਲੱਭੋ_ਪਹਿਲਾ_ਆਖਰੀ_ਮੰਜ਼ਿਲ_ਕਤਾਰ_src(src, wid, cmp, ਝੂਠ, ਝੂਠ);
      }
      thr->ਧੱਕੋ_object(vvp_object_t(dst));
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_ਕਤਾਰ_ਲੱਭੋ_ਆਖਰੀ_PROP_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_vector4_t cmp = thr->ਕੱਢੋ_vec4();
      size_t pid = cp->ਸੰਖਿਆ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      ਪ੍ਰਤਿਜ੍ਞਾ(cmp.size() == wid);

      vvp_object_t& ਸਿਖਰ = thr->peek_object();
      vvp_cobject*cobj = ਸਿਖਰ.peek<vvp_cobject>();
      ਪ੍ਰਤਿਜ੍ਞਾ(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_ਕਤਾਰ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ਕਤਾਰ_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_ਕਤਾਰ_vec4* dst =
	    qsrc ? ਕਤਾਰ_ਲੱਭੋ_ਪਹਿਲਾ_ਆਖਰੀ_ਮੰਜ਼ਿਲ_ਕਤਾਰ_src(qsrc, wid, cmp, ਝੂਠ, ਝੂਠ)
		 : ਕਤਾਰ_ਲੱਭੋ_ਪਹਿਲਾ_ਆਖਰੀ_ਮੰਜ਼ਿਲ_ਕਤਾਰ_src(dsrc, wid, cmp, ਝੂਠ, ਝੂਠ);
      thr->ਧੱਕੋ_object(vvp_object_t(dst));
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_ਕਤਾਰ_ਲੱਭੋ_ਆਖਰੀ_ਸੂਚਕਾਂਕ_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_vector4_t cmp = thr->ਕੱਢੋ_vec4();
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      ਪ੍ਰਤਿਜ੍ਞਾ(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_ਕਤਾਰ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ਕਤਾਰ_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_ਕਤਾਰ_vec4* dst;
      ਜੇ (qsrc) dst = ਕਤਾਰ_ਲੱਭੋ_ਪਹਿਲਾ_ਆਖਰੀ_ਮੰਜ਼ਿਲ_ਕਤਾਰ_src(qsrc, wid, cmp, ਝੂਠ, ਸੱਚ);
      ਨਹੀਂਤਾਂ ਜੇ (dsrc) dst = ਕਤਾਰ_ਲੱਭੋ_ਪਹਿਲਾ_ਆਖਰੀ_ਮੰਜ਼ਿਲ_ਕਤਾਰ_src(dsrc, wid, cmp, ਝੂਠ, ਸੱਚ);
      ਨਹੀਂਤਾਂ {
	    vvp_ਕਤਾਰ* src_q = get_ਕਤਾਰ_object<vvp_ਕਤਾਰ_vec4>(thr, net);
	    vvp_ਕਤਾਰ_vec4* src = dynamic_cast<vvp_ਕਤਾਰ_vec4*>(src_q);
	    dst = ਕਤਾਰ_ਲੱਭੋ_ਪਹਿਲਾ_ਆਖਰੀ_ਮੰਜ਼ਿਲ_ਕਤਾਰ_src(src, wid, cmp, ਝੂਠ, ਸੱਚ);
      }
      thr->ਧੱਕੋ_object(vvp_object_t(dst));
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_ਕਤਾਰ_ਲੱਭੋ_ਆਖਰੀ_ਸੂਚਕਾਂਕ_PROP_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_vector4_t cmp = thr->ਕੱਢੋ_vec4();
      size_t pid = cp->ਸੰਖਿਆ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      ਪ੍ਰਤਿਜ੍ਞਾ(cmp.size() == wid);

      vvp_object_t& ਸਿਖਰ = thr->peek_object();
      vvp_cobject*cobj = ਸਿਖਰ.peek<vvp_cobject>();
      ਪ੍ਰਤਿਜ੍ਞਾ(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_ਕਤਾਰ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ਕਤਾਰ_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_ਕਤਾਰ_vec4* dst =
	    qsrc ? ਕਤਾਰ_ਲੱਭੋ_ਪਹਿਲਾ_ਆਖਰੀ_ਮੰਜ਼ਿਲ_ਕਤਾਰ_src(qsrc, wid, cmp, ਝੂਠ, ਸੱਚ)
		 : ਕਤਾਰ_ਲੱਭੋ_ਪਹਿਲਾ_ਆਖਰੀ_ਮੰਜ਼ਿਲ_ਕਤਾਰ_src(dsrc, wid, cmp, ਝੂਠ, ਸੱਚ);
      thr->ਧੱਕੋ_object(vvp_object_t(dst));
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_ਕਤਾਰ_MIN_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_net_t* net = cp->net;
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      vvp_ਕਤਾਰ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ਕਤਾਰ_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_ਕਤਾਰ_vec4* dst;
      ਜੇ (qsrc) dst = ਕਤਾਰ_ਲੜੀਵਾਰ_min_max_src(qsrc, wid, ਝੂਠ);
      ਨਹੀਂਤਾਂ ਜੇ (dsrc) dst = ਕਤਾਰ_ਲੜੀਵਾਰ_min_max_src(dsrc, wid, ਝੂਠ);
      ਨਹੀਂਤਾਂ {
	    vvp_ਕਤਾਰ* src_q = get_ਕਤਾਰ_object<vvp_ਕਤਾਰ_vec4>(thr, net);
	    vvp_ਕਤਾਰ_vec4* src = dynamic_cast<vvp_ਕਤਾਰ_vec4*>(src_q);
	    dst = ਕਤਾਰ_ਲੜੀਵਾਰ_min_max_src(src, wid, ਝੂਠ);
      }
      thr->ਧੱਕੋ_object(vvp_object_t(dst));
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_ਕਤਾਰ_MIN_OBJ_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      vvp_object_t src_obj;
      thr->ਕੱਢੋ_object(src_obj);

      vvp_ਕਤਾਰ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ਕਤਾਰ_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_ਕਤਾਰ_vec4* dst = qsrc ? ਕਤਾਰ_ਲੜੀਵਾਰ_min_max_src(qsrc, wid, ਝੂਠ)
				 : ਕਤਾਰ_ਲੜੀਵਾਰ_min_max_src(dsrc, wid, ਝੂਠ);
      thr->ਧੱਕੋ_object(vvp_object_t(dst));
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_ਕਤਾਰ_MIN_PROP_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      size_t pid = cp->ਸੰਖਿਆ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];

      vvp_object_t& ਸਿਖਰ = thr->peek_object();
      vvp_cobject*cobj = ਸਿਖਰ.peek<vvp_cobject>();
      ਪ੍ਰਤਿਜ੍ਞਾ(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_ਕਤਾਰ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ਕਤਾਰ_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_ਕਤਾਰ_vec4* dst = qsrc ? ਕਤਾਰ_ਲੜੀਵਾਰ_min_max_src(qsrc, wid, ਝੂਠ)
				 : ਕਤਾਰ_ਲੜੀਵਾਰ_min_max_src(dsrc, wid, ਝੂਠ);
      thr->ਧੱਕੋ_object(vvp_object_t(dst));
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_ਕਤਾਰ_MAX_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_net_t* net = cp->net;
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      vvp_ਕਤਾਰ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ਕਤਾਰ_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_ਕਤਾਰ_vec4* dst;
      ਜੇ (qsrc) dst = ਕਤਾਰ_ਲੜੀਵਾਰ_min_max_src(qsrc, wid, ਸੱਚ);
      ਨਹੀਂਤਾਂ ਜੇ (dsrc) dst = ਕਤਾਰ_ਲੜੀਵਾਰ_min_max_src(dsrc, wid, ਸੱਚ);
      ਨਹੀਂਤਾਂ {
	    vvp_ਕਤਾਰ* src_q = get_ਕਤਾਰ_object<vvp_ਕਤਾਰ_vec4>(thr, net);
	    vvp_ਕਤਾਰ_vec4* src = dynamic_cast<vvp_ਕਤਾਰ_vec4*>(src_q);
	    dst = ਕਤਾਰ_ਲੜੀਵਾਰ_min_max_src(src, wid, ਸੱਚ);
      }
      thr->ਧੱਕੋ_object(vvp_object_t(dst));
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_ਕਤਾਰ_MAX_OBJ_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      vvp_object_t src_obj;
      thr->ਕੱਢੋ_object(src_obj);

      vvp_ਕਤਾਰ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ਕਤਾਰ_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_ਕਤਾਰ_vec4* dst = qsrc ? ਕਤਾਰ_ਲੜੀਵਾਰ_min_max_src(qsrc, wid, ਸੱਚ)
				 : ਕਤਾਰ_ਲੜੀਵਾਰ_min_max_src(dsrc, wid, ਸੱਚ);
      thr->ਧੱਕੋ_object(vvp_object_t(dst));
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_ਕਤਾਰ_MAX_PROP_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      size_t pid = cp->ਸੰਖਿਆ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];

      vvp_object_t& ਸਿਖਰ = thr->peek_object();
      vvp_cobject*cobj = ਸਿਖਰ.peek<vvp_cobject>();
      ਪ੍ਰਤਿਜ੍ਞਾ(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_ਕਤਾਰ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ਕਤਾਰ_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_ਕਤਾਰ_vec4* dst = qsrc ? ਕਤਾਰ_ਲੜੀਵਾਰ_min_max_src(qsrc, wid, ਸੱਚ)
				 : ਕਤਾਰ_ਲੜੀਵਾਰ_min_max_src(dsrc, wid, ਸੱਚ);
      thr->ਧੱਕੋ_object(vvp_object_t(dst));
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_ਕਤਾਰ_UNIQUE_OBJ_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      vvp_object_t src_obj;
      thr->ਕੱਢੋ_object(src_obj);

      vvp_ਕਤਾਰ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ਕਤਾਰ_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_ਕਤਾਰ_vec4* dst = qsrc ? ਕਤਾਰ_ਲੜੀਵਾਰ_unique_src(qsrc, wid, ਝੂਠ)
				 : ਕਤਾਰ_ਲੜੀਵਾਰ_unique_src(dsrc, wid, ਝੂਠ);
      thr->ਧੱਕੋ_object(vvp_object_t(dst));
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_ਕਤਾਰ_UNIQUE_ਸੂਚਕਾਂਕ_OBJ_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      vvp_object_t src_obj;
      thr->ਕੱਢੋ_object(src_obj);

      vvp_ਕਤਾਰ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ਕਤਾਰ_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_ਕਤਾਰ_vec4* dst = qsrc ? ਕਤਾਰ_ਲੜੀਵਾਰ_unique_src(qsrc, wid, ਸੱਚ)
				 : ਕਤਾਰ_ਲੜੀਵਾਰ_unique_src(dsrc, wid, ਸੱਚ);
      thr->ਧੱਕੋ_object(vvp_object_t(dst));
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_ਕਤਾਰ_UNIQUE_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_net_t*net = cp->net;
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      vvp_ਕਤਾਰ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ਕਤਾਰ_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_ਕਤਾਰ_vec4* dst;
      ਜੇ (qsrc) dst = ਕਤਾਰ_ਲੜੀਵਾਰ_unique_src(qsrc, wid, ਝੂਠ);
      ਨਹੀਂਤਾਂ ਜੇ (dsrc) dst = ਕਤਾਰ_ਲੜੀਵਾਰ_unique_src(dsrc, wid, ਝੂਠ);
      ਨਹੀਂਤਾਂ {
	    vvp_ਕਤਾਰ* src_q = get_ਕਤਾਰ_object<vvp_ਕਤਾਰ_vec4>(thr, net);
	    vvp_ਕਤਾਰ_vec4* src = dynamic_cast<vvp_ਕਤਾਰ_vec4*>(src_q);
	    dst = ਕਤਾਰ_ਲੜੀਵਾਰ_unique_src(src, wid, ਝੂਠ);
      }
      thr->ਧੱਕੋ_object(vvp_object_t(dst));
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_ਕਤਾਰ_UNIQUE_PROP_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      size_t pid = cp->ਸੰਖਿਆ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];

      vvp_object_t& ਸਿਖਰ = thr->peek_object();
      vvp_cobject*cobj = ਸਿਖਰ.peek<vvp_cobject>();
      ਪ੍ਰਤਿਜ੍ਞਾ(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_ਕਤਾਰ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ਕਤਾਰ_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_ਕਤਾਰ_vec4* dst = qsrc ? ਕਤਾਰ_ਲੜੀਵਾਰ_unique_src(qsrc, wid, ਝੂਠ)
				 : ਕਤਾਰ_ਲੜੀਵਾਰ_unique_src(dsrc, wid, ਝੂਠ);
      thr->ਧੱਕੋ_object(vvp_object_t(dst));
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_ਕਤਾਰ_UNIQUE_ਸੂਚਕਾਂਕ_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_net_t*net = cp->net;
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      vvp_ਕਤਾਰ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ਕਤਾਰ_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_ਕਤਾਰ_vec4* dst;
      ਜੇ (qsrc) dst = ਕਤਾਰ_ਲੜੀਵਾਰ_unique_src(qsrc, wid, ਸੱਚ);
      ਨਹੀਂਤਾਂ ਜੇ (dsrc) dst = ਕਤਾਰ_ਲੜੀਵਾਰ_unique_src(dsrc, wid, ਸੱਚ);
      ਨਹੀਂਤਾਂ {
	    vvp_ਕਤਾਰ* src_q = get_ਕਤਾਰ_object<vvp_ਕਤਾਰ_vec4>(thr, net);
	    vvp_ਕਤਾਰ_vec4* src = dynamic_cast<vvp_ਕਤਾਰ_vec4*>(src_q);
	    dst = ਕਤਾਰ_ਲੜੀਵਾਰ_unique_src(src, wid, ਸੱਚ);
      }
      thr->ਧੱਕੋ_object(vvp_object_t(dst));
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_ਕਤਾਰ_UNIQUE_ਸੂਚਕਾਂਕ_PROP_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      size_t pid = cp->ਸੰਖਿਆ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];

      vvp_object_t& ਸਿਖਰ = thr->peek_object();
      vvp_cobject*cobj = ਸਿਖਰ.peek<vvp_cobject>();
      ਪ੍ਰਤਿਜ੍ਞਾ(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_ਕਤਾਰ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ਕਤਾਰ_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_ਕਤਾਰ_vec4* dst = qsrc ? ਕਤਾਰ_ਲੜੀਵਾਰ_unique_src(qsrc, wid, ਸੱਚ)
				 : ਕਤਾਰ_ਲੜੀਵਾਰ_unique_src(dsrc, wid, ਸੱਚ);
      thr->ਧੱਕੋ_object(vvp_object_t(dst));
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_PROP_ਕਤਾਰ_ਮਾਪ(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      size_t pid = cp->ਸੰਖਿਆ;

      vvp_object_t& ਸਿਖਰ = thr->peek_object();
      vvp_cobject*cobj = ਸਿਖਰ.peek<vvp_cobject>();
      ਪ੍ਰਤਿਜ੍ਞਾ(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_ਕਤਾਰ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ਕਤਾਰ_or_darray_vec4_from_object(qobj, qsrc, dsrc);

      size_t sz = qsrc ? qsrc->get_ਮਾਪ() : (dsrc ? dsrc->get_ਮਾਪ() : 0);

      vvp_vector4_t val(32, BIT4_0);
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul = sz;
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0; idx < 32; idx++) {
	    ਜੇ (ul & 1UL) {
		  val.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_1);
	    } ਨਹੀਂਤਾਂ {
		  val.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      thr->ਧੱਕੋ_vec4(val);
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_ਕਤਾਰ_ਮਾਪ_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_ਕਤਾਰ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ਕਤਾਰ_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      size_t sz =
	    qsrc ? qsrc->get_ਮਾਪ() : (dsrc ? dsrc->get_ਮਾਪ() : 0);

      vvp_vector4_t val(32, BIT4_0);
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul = sz;
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0; idx < 32; idx++) {
	    ਜੇ (ul & 1UL) {
		  val.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_1);
	    } ਨਹੀਂਤਾਂ {
		  val.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      thr->ਧੱਕੋ_vec4(val);
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_ਕਤਾਰ_ਸ਼ਬਦ_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      ਬਿਨਾਨਿਸ਼ਾਨੀ ix_reg = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[1];
      int64_t adr = thr->ਸ਼ਬਦ[ix_reg].w_int;
      ਜੇ (thr->flags[4] == BIT4_1) {
	    thr->ਧੱਕੋ_vec4(vvp_vector4_t(wid));
	    ਵਾਪਸ ਸੱਚ;
      }
      ਜੇ (adr < 0) {
	    thr->ਧੱਕੋ_vec4(vvp_vector4_t(wid));
	    ਵਾਪਸ ਸੱਚ;
      }
      vvp_net_t* net = cp->net;
      vvp_ਕਤਾਰ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ਕਤਾਰ_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t out(wid);
      ਜੇ (qsrc) {
	    ਜੇ ((size_t)adr >= qsrc->get_ਮਾਪ()) {
		  thr->ਧੱਕੋ_vec4(out);
		  ਵਾਪਸ ਸੱਚ;
	    }
	    qsrc->get_ਸ਼ਬਦ((ਬਿਨਾਨਿਸ਼ਾਨੀ)adr, out);
      } ਨਹੀਂਤਾਂ ਜੇ (dsrc) {
	    ਜੇ ((size_t)adr >= dsrc->get_ਮਾਪ()) {
		  thr->ਧੱਕੋ_vec4(out);
		  ਵਾਪਸ ਸੱਚ;
	    }
	    dsrc->get_ਸ਼ਬਦ((ਬਿਨਾਨਿਸ਼ਾਨੀ)adr, out);
      } ਨਹੀਂਤਾਂ {
	    vvp_ਕਤਾਰ* src_q = get_ਕਤਾਰ_object<vvp_ਕਤਾਰ_vec4>(thr, net);
	    vvp_ਕਤਾਰ_vec4* src = dynamic_cast<vvp_ਕਤਾਰ_vec4*>(src_q);
	    ਜੇ (!src || (size_t)adr >= src->get_ਮਾਪ()) {
		  thr->ਧੱਕੋ_vec4(out);
		  ਵਾਪਸ ਸੱਚ;
	    }
	    src->get_ਸ਼ਬਦ((ਬਿਨਾਨਿਸ਼ਾਨੀ)adr, out);
      }
      thr->ਧੱਕੋ_vec4(out);
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_ਕਤਾਰ_ਸ਼ਬਦ_PROP_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      size_t pid = cp->ਸੰਖਿਆ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      ਬਿਨਾਨਿਸ਼ਾਨੀ ix_reg = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[1];
      int64_t adr = thr->ਸ਼ਬਦ[ix_reg].w_int;
      ਜੇ (thr->flags[4] == BIT4_1) {
	    thr->ਧੱਕੋ_vec4(vvp_vector4_t(wid));
	    ਵਾਪਸ ਸੱਚ;
      }
      ਜੇ (adr < 0) {
	    thr->ਧੱਕੋ_vec4(vvp_vector4_t(wid));
	    ਵਾਪਸ ਸੱਚ;
      }

      vvp_object_t saved_ਸਿਖਰ;
      ਤਾਰਕਿਕ popped_ਸਿਖਰ = ਝੂਠ;
      vvp_object_t& ਸਿਖਰ = thr->peek_object();
      vvp_cobject*cobj = ਸਿਖਰ.peek<vvp_cobject>();
      ਜੇ (cobj == 0) {
	    // Array-locator with(property) keeps an accumulator queue on top.
	    // Temporarily pop it to access the class object beneath.
	    thr->ਕੱਢੋ_object(saved_ਸਿਖਰ);
	    popped_ਸਿਖਰ = ਸੱਚ;
	    vvp_object_t& ਹੇਠਾਂ = thr->peek_object();
	    cobj = ਹੇਠਾਂ.peek<vvp_cobject>();
      }
      ਜੇ (cobj == 0) {
	    ਜੇ (popped_ਸਿਖਰ) thr->ਧੱਕੋ_object(saved_ਸਿਖਰ);
	    thr->ਧੱਕੋ_vec4(vvp_vector4_t(wid));
	    ਵਾਪਸ ਸੱਚ;
      }

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_ਕਤਾਰ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ਕਤਾਰ_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t out(wid);
      ਜੇ (qsrc) {
	    ਜੇ ((size_t)adr >= qsrc->get_ਮਾਪ()) {
		  thr->ਧੱਕੋ_vec4(out);
		  ਵਾਪਸ ਸੱਚ;
	    }
	    qsrc->get_ਸ਼ਬਦ((ਬਿਨਾਨਿਸ਼ਾਨੀ)adr, out);
      } ਨਹੀਂਤਾਂ ਜੇ (dsrc) {
	    ਜੇ ((size_t)adr >= dsrc->get_ਮਾਪ()) {
		  thr->ਧੱਕੋ_vec4(out);
		  ਵਾਪਸ ਸੱਚ;
	    }
	    dsrc->get_ਸ਼ਬਦ((ਬਿਨਾਨਿਸ਼ਾਨੀ)adr, out);
      } ਨਹੀਂਤਾਂ {
	    thr->ਧੱਕੋ_vec4(out);
	    ਜੇ (popped_ਸਿਖਰ) thr->ਧੱਕੋ_object(saved_ਸਿਖਰ);
	    ਵਾਪਸ ਸੱਚ;
      }
      ਜੇ (popped_ਸਿਖਰ) thr->ਧੱਕੋ_object(saved_ਸਿਖਰ);
      thr->ਧੱਕੋ_vec4(out);
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_CMPIX_LTU(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ a = cp->ਸੰਖਿਆ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ b = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      ਜੇ (thr->ਸ਼ਬਦ[a].w_uint < thr->ਸ਼ਬਦ[b].w_uint) {
	    thr->flags[4] = BIT4_1;
      } ਨਹੀਂਤਾਂ {
	    thr->flags[4] = BIT4_0;
      }
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_CMPIX_SLT0(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ r = cp->ਸੰਖਿਆ;
      thr->flags[4] = (thr->ਸ਼ਬਦ[r].w_int < 0) ? BIT4_1 : BIT4_0;
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_ਧੱਕੋ_IX_VEC4(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ ix = cp->ਸੰਖਿਆ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      ਬਿਨਾਨਿਸ਼ਾਨੀ sgn = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[1];
      uint32_t ubits;
      ਜੇ (sgn) {
	    int32_t sv = (int32_t)thr->ਸ਼ਬਦ[ix].w_int;
	    ubits = (uint32_t)sv;
      } ਨਹੀਂਤਾਂ {
	    ubits = (uint32_t)thr->ਸ਼ਬਦ[ix].w_uint;
      }
      ਜੇ (wid > 32) wid = 32;
      vvp_vector4_t tmp(wid, BIT4_0);
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0; idx < wid; idx++) {
	    ਜੇ (ubits & 1u) {
		  tmp.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_1);
	    } ਨਹੀਂਤਾਂ {
		  tmp.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_0);
	    }
	    ubits >>= 1;
      }
      thr->ਧੱਕੋ_vec4(tmp);
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_ਕਤਾਰ_NEW_ਖਾਲੀ_ਥਾਵਾਂ_V(vthread_t thr, vvp_ਕੋਡ_t)
{
      thr->ਧੱਕੋ_object(vvp_object_t(ਨਵਾਂ vvp_ਕਤਾਰ_vec4()));
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_ਕਤਾਰ_APPEND_ਸ਼ਬਦ_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      vvp_vector4_t elem = thr->ਕੱਢੋ_vec4();
      ਪ੍ਰਤਿਜ੍ਞਾ(elem.size() == wid);
      vvp_object_t qobj;
      thr->ਕੱਢੋ_object(qobj);
      vvp_ਕਤਾਰ_vec4* q = qobj.peek<vvp_ਕਤਾਰ_vec4>();
      ਪ੍ਰਤਿਜ੍ਞਾ(q);
      q->push_back(elem, 0);
      thr->ਧੱਕੋ_object(qobj);
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_DELETE_PROP_ELEM(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      size_t pid = cp->ਸੰਖਿਆ;

      int64_t idx_val = thr->ਸ਼ਬਦ[3].w_int;
      ਜੇ (thr->flags[4] == BIT4_1) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with undefined index."
	         << ਸਤਰ_ਅੰਤ;
	    ਵਾਪਸ ਸੱਚ;
      }
      ਜੇ (idx_val < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with negative index."
	         << ਸਤਰ_ਅੰਤ;
	    ਵਾਪਸ ਸੱਚ;
      }
      size_t idx = idx_val;

      vvp_object_t& ਸਿਖਰ = thr->peek_object();
      vvp_cobject*cobj = ਸਿਖਰ.peek<vvp_cobject>();
      ਪ੍ਰਤਿਜ੍ਞਾ(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_ਕਤਾਰ* ਕਤਾਰ = qobj.peek<vvp_ਕਤਾਰ>();
      ਜੇ (ਕਤਾਰ == 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping delete(" << idx
	         << ") on empty queue." << ਸਤਰ_ਅੰਤ;
      } ਨਹੀਂਤਾਂ {
	    size_t size = ਕਤਾਰ->get_ਮਾਪ();
	    ਜੇ (idx >= size) {
		  cerr << thr->get_fileline()
		       << "Warning: skipping out of range delete(" << idx
		       << ") on queue of size " << size << "." << ਸਤਰ_ਅੰਤ;
	    } ਨਹੀਂਤਾਂ {
		  ਕਤਾਰ->erase(idx);
	    }
      }

      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_DELETE_PROP_OBJ(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      size_t pid = cp->ਸੰਖਿਆ;

      vvp_object_t& ਸਿਖਰ = thr->peek_object();
      vvp_cobject*cobj = ਸਿਖਰ.peek<vvp_cobject>();
      ਪ੍ਰਤਿਜ੍ਞਾ(cobj);

      cobj->ਸੈੱਟ_object(pid, vvp_object_t(), 0);

      ਵਾਪਸ ਸੱਚ;
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ ELEM, ਵਰਗ QTYPE>
ਸਥਿਰ ਤਾਰਕਿਕ store_qb(vthread_t thr, vvp_ਕੋਡ_t cp, ਬਿਨਾਨਿਸ਼ਾਨੀ wid=0)
{
      ELEM value;
      vvp_net_t*net = cp->net;
      ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ = thr->ਸ਼ਬਦ[cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0]].w_int;
      ਕੱਢੋ_ਮੁੱਲ(thr, value, wid); // Pop the value to store.

      vvp_ਕਤਾਰ*ਕਤਾਰ = get_ਕਤਾਰ_object<QTYPE>(thr, net);
      ਪ੍ਰਤਿਜ੍ਞਾ(ਕਤਾਰ);
      ਕਤਾਰ->push_back(value, max_ਮਾਪ);
      ਵਾਪਸ ਸੱਚ;
}

/*
 * %store/qb/r <var-label>, <max-idx>
 */
ਤਾਰਕਿਕ ਦਾ_STORE_QB_R(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ store_qb<ਦੁੱਗਣਾ, vvp_ਕਤਾਰ_real>(thr, cp);
}

/*
 * %store/qb/str <var-label>, <max-idx>
 */
ਤਾਰਕਿਕ ਦਾ_STORE_QB_STR(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ store_qb<string, vvp_ਕਤਾਰ_string>(thr, cp);
}

/*
 * %store/qb/v <var-label>, <max-idx>, <wid>
 */
ਤਾਰਕਿਕ ਦਾ_STORE_QB_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ store_qb<vvp_vector4_t, vvp_ਕਤਾਰ_vec4>(thr, cp, cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[1]);
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ ELEM, ਵਰਗ QTYPE>
ਸਥਿਰ ਤਾਰਕਿਕ store_qdar(vthread_t thr, vvp_ਕੋਡ_t cp, ਬਿਨਾਨਿਸ਼ਾਨੀ wid=0)
{
      int64_t idx = thr->ਸ਼ਬਦ[3].w_int;
      ELEM value;
      vvp_net_t*net = cp->net;
      ਕੱਢੋ_ਮੁੱਲ(thr, value, wid); // Pop the value to store.

      vvp_ਕਤਾਰ*ਕਤਾਰ = get_ਕਤਾਰ_object<QTYPE>(thr, net);
      ਪ੍ਰਤਿਜ੍ਞਾ(ਕਤਾਰ);
      ਜੇ (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot assign to a negative "
	         << get_ਕਤਾਰ_type(value)
	         << " index (" << idx << "). ";
	    print_ਕਤਾਰ_ਮੁੱਲ(value);
	    cerr << " was not added." << ਸਤਰ_ਅੰਤ;
      } ਨਹੀਂਤਾਂ ਜੇ (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot assign to an undefined "
	         << get_ਕਤਾਰ_type(value) << " index. ";
	    print_ਕਤਾਰ_ਮੁੱਲ(value);
	    cerr << " was not added." << ਸਤਰ_ਅੰਤ;
      } ਨਹੀਂਤਾਂ {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ = thr->ਸ਼ਬਦ[cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0]].w_int;
	    ਕਤਾਰ->ਸੈੱਟ_ਸ਼ਬਦ_max(idx, value, max_ਮਾਪ);
      }
      ਵਾਪਸ ਸੱਚ;
}

/*
 * %store/qdar/r <var>, idx
 */
ਤਾਰਕਿਕ ਦਾ_STORE_QDAR_R(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ store_qdar<ਦੁੱਗਣਾ, vvp_ਕਤਾਰ_real>(thr, cp);
}

/*
 * %store/qdar/str <var>, idx
 */
ਤਾਰਕਿਕ ਦਾ_STORE_QDAR_STR(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ store_qdar<string, vvp_ਕਤਾਰ_string>(thr, cp);
}

/*
 * %store/qdar/v <var>, idx
 */
ਤਾਰਕਿਕ ਦਾ_STORE_QDAR_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ store_qdar<vvp_vector4_t, vvp_ਕਤਾਰ_vec4>(thr, cp, cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[1]);
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ ELEM, ਵਰਗ QTYPE>
ਸਥਿਰ ਤਾਰਕਿਕ store_qf(vthread_t thr, vvp_ਕੋਡ_t cp, ਬਿਨਾਨਿਸ਼ਾਨੀ wid=0)
{
      ELEM value;
      vvp_net_t*net = cp->net;
      ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ = thr->ਸ਼ਬਦ[cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0]].w_int;
      ਕੱਢੋ_ਮੁੱਲ(thr, value, wid); // Pop the value to store.

      vvp_ਕਤਾਰ*ਕਤਾਰ = get_ਕਤਾਰ_object<QTYPE>(thr, net);
      ਪ੍ਰਤਿਜ੍ਞਾ(ਕਤਾਰ);
      ਕਤਾਰ->push_front(value, max_ਮਾਪ);
      ਵਾਪਸ ਸੱਚ;
}
/*
 * %store/qf/r <var-label>, <max-idx>
 */
ਤਾਰਕਿਕ ਦਾ_STORE_QF_R(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ store_qf<ਦੁੱਗਣਾ, vvp_ਕਤਾਰ_real>(thr, cp);
}

/*
 * %store/qf/str <var-label>, <max-idx>
 */
ਤਾਰਕਿਕ ਦਾ_STORE_QF_STR(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ store_qf<string, vvp_ਕਤਾਰ_string>(thr, cp);
}

/*
 * %store/qb/v <var-label>, <max-idx>, <wid>
 */
ਤਾਰਕਿਕ ਦਾ_STORE_QF_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ store_qf<vvp_vector4_t, vvp_ਕਤਾਰ_vec4>(thr, cp, cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[1]);
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ ELEM, ਵਰਗ QTYPE>
ਸਥਿਰ ਤਾਰਕਿਕ store_qobj(vthread_t thr, vvp_ਕੋਡ_t cp, ਬਿਨਾਨਿਸ਼ਾਨੀ wid=0)
{
// FIXME: Can we actually use wid here?
      (ਖਾਲੀ)wid;
      vvp_net_t*net = cp->net;

      vvp_ਕਤਾਰ*ਕਤਾਰ = get_ਕਤਾਰ_object<QTYPE>(thr, net);
      ਪ੍ਰਤਿਜ੍ਞਾ(ਕਤਾਰ);

      vvp_object_t src;
      thr->ਕੱਢੋ_object(src);

        // If it is null just clear the queue
      ਜੇ (src.test_nil())
	    ਕਤਾਰ->erase_ਪੂਛ(0);
      ਨਹੀਂਤਾਂ {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ = thr->ਸ਼ਬਦ[cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0]].w_int;
	    ਕਤਾਰ->copy_elems(src, max_ਮਾਪ);
      }

      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_STORE_QOBJ_R(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ store_qobj<ਦੁੱਗਣਾ, vvp_ਕਤਾਰ_real>(thr, cp);
}

ਤਾਰਕਿਕ ਦਾ_STORE_QOBJ_STR(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ store_qobj<string, vvp_ਕਤਾਰ_string>(thr, cp);
}

ਤਾਰਕਿਕ ਦਾ_STORE_QOBJ_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ store_qobj<vvp_vector4_t, vvp_ਕਤਾਰ_vec4>(thr, cp, cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[1]);
}

ਸਥਿਰ ਖਾਲੀ vvp_send(vthread_t thr, vvp_net_ptr_t ptr, ਅਚਲ ਦੁੱਗਣਾ&val)
{
      vvp_send_real(ptr, val, thr->wt_context);
}

ਸਥਿਰ ਖਾਲੀ vvp_send(vthread_t thr, vvp_net_ptr_t ptr, ਅਚਲ string&val)
{
      vvp_send_string(ptr, val, thr->wt_context);
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ ELEM>
ਸਥਿਰ ਤਾਰਕਿਕ store(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ELEM val;
      ਕੱਢੋ_ਮੁੱਲ(thr, val, 0);
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_send(thr, ptr, val);
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_STORE_REAL(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ store<ਦੁੱਗਣਾ>(thr, cp);
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ ELEM>
ਸਥਿਰ ਤਾਰਕਿਕ storea(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ idx = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      ELEM val;
      ਕੱਢੋ_ਮੁੱਲ(thr, val, 0);

      ਜੇ (thr->flags[4] != BIT4_1) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ adr = thr->ਸ਼ਬਦ[idx].w_int;
	    cp->array->ਸੈੱਟ_ਸ਼ਬਦ(adr, val);
      }

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %store/reala <var-label> <index>
 */
ਤਾਰਕਿਕ ਦਾ_STORE_REALA(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ storea<ਦੁੱਗਣਾ>(thr, cp);
}

ਤਾਰਕਿਕ ਦਾ_STORE_STR(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ store<string>(thr, cp);
}

/*
 * %store/stra <array-label> <index>
 */
ਤਾਰਕਿਕ ਦਾ_STORE_STRA(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਵਾਪਸ storea<string>(thr, cp);
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
ਤਾਰਕਿਕ ਦਾ_STORE_VEC4(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_net_ptr_t ptr(cp->net, 0);
      ਅਚਲ vvp_signal_ਮੁੱਲ*sig = dynamic_cast<vvp_signal_ਮੁੱਲ*> (cp->net->fil);
      ਬਿਨਾਨਿਸ਼ਾਨੀ off_ਸੂਚਕਾਂਕ = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ wid = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[1];

      int64_t off = off_ਸੂਚਕਾਂਕ ? thr->ਸ਼ਬਦ[off_ਸੂਚਕਾਂਕ].w_int : 0;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ sig_ਮੁੱਲ_ਮਾਪ = sig->ਮੁੱਲ_ਮਾਪ();

      vvp_vector4_t&val = thr->peek_vec4();
      ਬਿਨਾਨਿਸ਼ਾਨੀ val_ਮਾਪ = val.size();

      ਜੇ (val_ਮਾਪ < wid) {
	    cerr << thr->get_fileline()
	         << "XXXX Internal error: val.size()=" << val_ਮਾਪ
		 << ", expecting >= " << wid << ਸਤਰ_ਅੰਤ;
      }
      ਪ੍ਰਤਿਜ੍ਞਾ(val_ਮਾਪ >= wid);
      ਜੇ (val_ਮਾਪ > wid) {
	    val.resize(wid);
      }

	// If there is a problem loading the index register, flags-4
	// will be set to 1, and we know here to skip the actual assignment.
      ਜੇ (off_ਸੂਚਕਾਂਕ!=0 && thr->flags[4] == BIT4_1) {
	    thr->ਕੱਢੋ_vec4(1);
	    ਵਾਪਸ ਸੱਚ;
      }

      ਜੇ (!resize_rval_vec(val, off, sig_ਮੁੱਲ_ਮਾਪ)) {
	    thr->ਕੱਢੋ_vec4(1);
	    ਵਾਪਸ ਸੱਚ;
      }

      ਜੇ (off == 0 && val.size() == sig_ਮੁੱਲ_ਮਾਪ)
	    vvp_send_vec4(ptr, val, thr->wt_context);
      ਨਹੀਂਤਾਂ
	    vvp_send_vec4_pv(ptr, val, off, sig_ਮੁੱਲ_ਮਾਪ, thr->wt_context);

      thr->ਕੱਢੋ_vec4(1);
      ਵਾਪਸ ਸੱਚ;
}

/*
 * %store/vec4a <var-label>, <addr>, <offset>
 */
ਤਾਰਕਿਕ ਦਾ_STORE_VEC4_A(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ adr_ਸੂਚਕਾਂਕ = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      ਬਿਨਾਨਿਸ਼ਾਨੀ off_ਸੂਚਕਾਂਕ = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[1];

      ਲੰਮਾ adr = adr_ਸੂਚਕਾਂਕ? thr->ਸ਼ਬਦ[adr_ਸੂਚਕਾਂਕ].w_int : 0;
      int64_t off = off_ਸੂਚਕਾਂਕ ? thr->ਸ਼ਬਦ[off_ਸੂਚਕਾਂਕ].w_int : 0;

	// Suppress action if flags-4 is true.
      ਜੇ (thr->flags[4] == BIT4_1) {
	    thr->ਕੱਢੋ_vec4(1);
	    ਵਾਪਸ ਸੱਚ;
      }

      vvp_vector4_t &value = thr->peek_vec4();

      ਜੇ (!resize_rval_vec(value, off, cp->array->get_ਸ਼ਬਦ_ਮਾਪ())) {
	    thr->ਕੱਢੋ_vec4(1);
	    ਵਾਪਸ ਸੱਚ;
      }

      cp->array->ਸੈੱਟ_ਸ਼ਬਦ(adr, off, value);

      thr->ਕੱਢੋ_vec4(1);
      ਵਾਪਸ ਸੱਚ;
}

/*
 * %sub
 *   pop r;
 *   pop l;
 *   push l-r;
 */
ਤਾਰਕਿਕ ਦਾ_SUB(vthread_t thr, vvp_ਕੋਡ_t)
{
      vvp_vector4_t r = thr->ਕੱਢੋ_vec4();
      vvp_vector4_t&l = thr->peek_vec4();

      l.sub(r);
      ਵਾਪਸ ਸੱਚ;
}

/*
 * %subi <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
ਤਾਰਕਿਕ ਦਾ_SUBI(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਸੰਖਿਆ;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_ਤੁਰੰਤ_rval (cp, r);

      l.sub(r);

      ਵਾਪਸ ਸੱਚ;

}

ਤਾਰਕਿਕ ਦਾ_SUB_WR(vthread_t thr, vvp_ਕੋਡ_t)
{
      ਦੁੱਗਣਾ r = thr->ਕੱਢੋ_real();
      ਦੁੱਗਣਾ l = thr->ਕੱਢੋ_real();
      thr->ਧੱਕੋ_real(l - r);
      ਵਾਪਸ ਸੱਚ;
}

/*
 * %substr <first>, <last>
 * Pop a string, take the substring (SystemVerilog style), and return
 * the result to the stack. This opcode actually works by editing the
 * string in place.
 */
ਤਾਰਕਿਕ ਦਾ_SUBSTR(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      int32_t ਪਹਿਲਾ = thr->ਸ਼ਬਦ[cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0]].w_int;
      int32_t ਆਖਰੀ = thr->ਸ਼ਬਦ[cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[1]].w_int;
      string&val = thr->peek_str(0);

      ਜੇ (ਪਹਿਲਾ < 0 || ਆਖਰੀ < ਪਹਿਲਾ || ਆਖਰੀ >= (int32_t)val.size()) {
	    val = string("");
	    ਵਾਪਸ ਸੱਚ;
      }

      val = val.substr(ਪਹਿਲਾ, ਆਖਰੀ-ਪਹਿਲਾ+1);
      ਵਾਪਸ ਸੱਚ;
}

/*
 * %substr/vec4 <index>, <wid>
 */
ਤਾਰਕਿਕ ਦਾ_SUBSTR_VEC4(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ sel_idx = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[1];

      int32_t sel = thr->ਸ਼ਬਦ[sel_idx].w_int;
      ਅਚਲ string&val = thr->peek_str(0);

      ਪ੍ਰਤਿਜ੍ਞਾ(wid%8 == 0);

      ਜੇ (sel < 0 || sel >= (int32_t)val.size()) {
	    vvp_vector4_t res (wid, BIT4_0);
	    thr->ਧੱਕੋ_vec4(res);
	    ਵਾਪਸ ਸੱਚ;
      }

      vvp_vector4_t res (wid, BIT4_0);

      ਪ੍ਰਤਿਜ੍ਞਾ(wid==8);
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ tmp = val[sel];
      ਲਈ (ਪੂਰਨ idx = 0 ; idx < 8 ; idx += 1) {
	    ਜੇ (tmp & (1<<idx))
		  res.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_1);
      }

      thr->ਧੱਕੋ_vec4(res);
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_FILE_ਸਤਰ(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vpiHandle handle = cp->handle;

	/* When it is available, keep the file/line information in the
	   thread for error/warning messages. */
      thr->ਸੈੱਟ_fileline(vpi_get_str(vpiFile, handle),
                        vpi_get(vpi_ਸਤਰ_No, handle));

      ਜੇ (show_file_ਸਤਰ)
	    cerr << thr->get_fileline()
	         << vpi_get_str(_vpiDescription, handle) << ਸਤਰ_ਅੰਤ;

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %test_nul <var-label>;
 * Test if the object at the specified variable is nil. If so, write
 * "1" into flags[4], otherwise write "0" into flags[4].
 */
ਤਾਰਕਿਕ ਦਾ_TEST_NUL(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_net_t*net = cp->net;

      ਪ੍ਰਤਿਜ੍ਞਾ(net);
      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      ਪ੍ਰਤਿਜ੍ਞਾ(obj);

      ਜੇ (obj->get_object().test_nil())
	    thr->flags[4] = BIT4_1;
      ਨਹੀਂਤਾਂ
	    thr->flags[4] = BIT4_0;

      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_TEST_NUL_A(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ idx = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      ਬਿਨਾਨਿਸ਼ਾਨੀ adr = thr->ਸ਼ਬਦ[idx].w_int;
      vvp_object_t ਸ਼ਬਦ_2;

	/* If the address is undefined, return true. */
      ਜੇ (thr->flags[4] == BIT4_1) {
	    ਵਾਪਸ ਸੱਚ;
      }

      cp->array->get_ਸ਼ਬਦ_obj(adr, ਸ਼ਬਦ_2);
      ਜੇ (ਸ਼ਬਦ_2.test_nil())
	    thr->flags[4] = BIT4_1;
      ਨਹੀਂਤਾਂ
	    thr->flags[4] = BIT4_0;

      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_TEST_NUL_OBJ(vthread_t thr, vvp_ਕੋਡ_t)
{
      ਜੇ (thr->peek_object().test_nil())
	    thr->flags[4] = BIT4_1;
      ਨਹੀਂਤਾਂ
	    thr->flags[4] = BIT4_0;
      ਵਾਪਸ ਸੱਚ;
}

/*
 * %test_nul/prop <pid>, <idx>
 */
ਤਾਰਕਿਕ ਦਾ_TEST_NUL_PROP(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ pid = cp->ਸੰਖਿਆ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ idx = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];

      ਜੇ (idx != 0) {
	    ਪ੍ਰਤਿਜ੍ਞਾ(idx < vthread_s::ਸ਼ਬਦ_ਗਿਣਤੀ);
	    idx = thr->ਸ਼ਬਦ[idx].w_uint;
      }

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj  = obj.peek<vvp_cobject>();

      vvp_object_t val;
      cobj->get_object(pid, val, idx);

      ਜੇ (val.test_nil())
	    thr->flags[4] = BIT4_1;
      ਨਹੀਂਤਾਂ
	    thr->flags[4] = BIT4_0;

      ਵਾਪਸ ਸੱਚ;
}

/* and() reduction returns the scalar sum (Verilog-style bitwise-AND per word). */
ਸਾਂਚਾ <ਵਰਗ SRC>
ਸਥਿਰ vvp_vector4_t uparr_and_ਸ਼ਬਦ_src(SRC* src, ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
{
      ਜੇ (!src) ਵਾਪਸ vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_1);
      ਲਈ (size_t i = 0; i < src->get_ਮਾਪ(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_ਸ਼ਬਦ(i, vi);
	    res &= vi;
      }
      ਵਾਪਸ res;
}

ਤਾਰਕਿਕ ਦਾ_UPARR_AND_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_net_t* net = cp->net;
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      vvp_ਕਤਾਰ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ਕਤਾਰ_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      ਜੇ (qsrc) res = uparr_and_ਸ਼ਬਦ_src(qsrc, wid);
      ਨਹੀਂਤਾਂ ਜੇ (dsrc) res = uparr_and_ਸ਼ਬਦ_src(dsrc, wid);
      ਨਹੀਂਤਾਂ {
	    vvp_ਕਤਾਰ* src_q = get_ਕਤਾਰ_object<vvp_ਕਤਾਰ_vec4>(thr, net);
	    vvp_ਕਤਾਰ_vec4* src = dynamic_cast<vvp_ਕਤਾਰ_vec4*>(src_q);
	    res = uparr_and_ਸ਼ਬਦ_src(src, wid);
      }
      thr->ਧੱਕੋ_vec4(res);
      ਵਾਪਸ ਸੱਚ;
}

/* or() reduction returns the scalar sum (Verilog-style bitwise-AND per word). */
ਸਾਂਚਾ <ਵਰਗ SRC>
ਸਥਿਰ vvp_vector4_t uparr_or_ਸ਼ਬਦ_src(SRC* src, ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
{
      ਜੇ (!src) ਵਾਪਸ vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_0);
      ਲਈ (size_t i = 0; i < src->get_ਮਾਪ(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_ਸ਼ਬਦ(i, vi);
	    res |= vi;
      }
      ਵਾਪਸ res;
}

ਤਾਰਕਿਕ ਦਾ_UPARR_OR_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_net_t* net = cp->net;
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      vvp_ਕਤਾਰ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ਕਤਾਰ_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      ਜੇ (qsrc) res = uparr_or_ਸ਼ਬਦ_src(qsrc, wid);
      ਨਹੀਂਤਾਂ ਜੇ (dsrc) res = uparr_or_ਸ਼ਬਦ_src(dsrc, wid);
      ਨਹੀਂਤਾਂ {
	    vvp_ਕਤਾਰ* src_q = get_ਕਤਾਰ_object<vvp_ਕਤਾਰ_vec4>(thr, net);
	    vvp_ਕਤਾਰ_vec4* src = dynamic_cast<vvp_ਕਤਾਰ_vec4*>(src_q);
	    res = uparr_or_ਸ਼ਬਦ_src(src, wid);
      }
      thr->ਧੱਕੋ_vec4(res);
      ਵਾਪਸ ਸੱਚ;
}

/* real product() reduction returns the scalar sum */
ਸਾਂਚਾ <ਵਰਗ SRC>
ਸਥਿਰ ਦੁੱਗਣਾ uparr_ਗੁਣਨਫਲ_ਸ਼ਬਦ_src_real(SRC* src)
{
      ਜੇ (!src) ਵਾਪਸ 0.0;
      ਦੁੱਗਣਾ acc = 1.0;
      ਲਈ (size_t i = 0; i < src->get_ਮਾਪ(); i += 1) {
	    ਦੁੱਗਣਾ vi;
	    src->get_ਸ਼ਬਦ(i, vi);
	    acc *= vi;
      }
      ਵਾਪਸ acc;
}

ਤਾਰਕਿਕ ਦਾ_UPARR_ਗੁਣਨਫਲ_REAL(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_ਕਤਾਰ_real* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ਕਤਾਰ_or_darray_real_from_net(thr, net, qsrc, dsrc);
      ਦੁੱਗਣਾ ਜੋੜ;
      ਜੇ (qsrc) ਜੋੜ = uparr_ਗੁਣਨਫਲ_ਸ਼ਬਦ_src_real(qsrc);
      ਨਹੀਂਤਾਂ ਜੇ (dsrc) ਜੋੜ = uparr_ਗੁਣਨਫਲ_ਸ਼ਬਦ_src_real(dsrc);
      ਨਹੀਂਤਾਂ {
	    vvp_ਕਤਾਰ* src_q = get_ਕਤਾਰ_object<vvp_ਕਤਾਰ_real>(thr, net);
	    vvp_ਕਤਾਰ_real* src = dynamic_cast<vvp_ਕਤਾਰ_real*>(src_q);
	    ਜੋੜ = uparr_ਗੁਣਨਫਲ_ਸ਼ਬਦ_src_real(src);
      }
      thr->ਧੱਕੋ_real(ਜੋੜ);
      ਵਾਪਸ ਸੱਚ;
}

/* product() reduction returns the scalar product (Verilog-style multiplication per word). */
ਸਾਂਚਾ <ਵਰਗ SRC>
ਸਥਿਰ vvp_vector4_t uparr_ਗੁਣਨਫਲ_ਸ਼ਬਦ_src(SRC* src, ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
{
      ਜੇ (!src) ਵਾਪਸ vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t acc = ਕਤਾਰ_unique_ulong_ਮੰਜ਼ਿਲ_vec4(1UL, wid);
      ਲਈ (size_t i = 0; i < src->get_ਮਾਪ(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_ਸ਼ਬਦ(i, vi);
	    acc.mul(vi);
      }
      ਵਾਪਸ acc;
}

ਤਾਰਕਿਕ ਦਾ_UPARR_ਗੁਣਨਫਲ_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_net_t* net = cp->net;
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      vvp_ਕਤਾਰ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ਕਤਾਰ_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t prod;
      ਜੇ (qsrc) prod = uparr_ਗੁਣਨਫਲ_ਸ਼ਬਦ_src(qsrc, wid);
      ਨਹੀਂਤਾਂ ਜੇ (dsrc) prod = uparr_ਗੁਣਨਫਲ_ਸ਼ਬਦ_src(dsrc, wid);
      ਨਹੀਂਤਾਂ {
	    vvp_ਕਤਾਰ* src_q = get_ਕਤਾਰ_object<vvp_ਕਤਾਰ_vec4>(thr, net);
	    vvp_ਕਤਾਰ_vec4* src = dynamic_cast<vvp_ਕਤਾਰ_vec4*>(src_q);
	    prod = uparr_ਗੁਣਨਫਲ_ਸ਼ਬਦ_src(src, wid);
      }
      thr->ਧੱਕੋ_vec4(prod);
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_UPARR_ਗੁਣਨਫਲ_PROP_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      size_t pid = cp->ਸੰਖਿਆ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];

      vvp_object_t& ਸਿਖਰ = thr->peek_object();
      vvp_cobject*cobj = ਸਿਖਰ.peek<vvp_cobject>();
      ਪ੍ਰਤਿਜ੍ਞਾ(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_ਕਤਾਰ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ਕਤਾਰ_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t prod =
	    qsrc ? uparr_ਗੁਣਨਫਲ_ਸ਼ਬਦ_src(qsrc, wid)
		 : uparr_ਗੁਣਨਫਲ_ਸ਼ਬਦ_src(dsrc, wid);
      thr->ਧੱਕੋ_vec4(prod);
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_UPARR_ਗੁਣਨਫਲ_OBJ_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      vvp_object_t src_obj;
      thr->ਕੱਢੋ_object(src_obj);

      vvp_ਕਤਾਰ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ਕਤਾਰ_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_vector4_t prod =
	    qsrc ? uparr_ਗੁਣਨਫਲ_ਸ਼ਬਦ_src(qsrc, wid)
		 : uparr_ਗੁਣਨਫਲ_ਸ਼ਬਦ_src(dsrc, wid);
      thr->ਧੱਕੋ_vec4(prod);
      ਵਾਪਸ ਸੱਚ;
}

/* real sum() reduction returns the scalar sum */
ਸਾਂਚਾ <ਵਰਗ SRC>
ਸਥਿਰ ਦੁੱਗਣਾ uparr_ਜੋੜ_ਸ਼ਬਦ_src_real(SRC* src)
{
      ਦੁੱਗਣਾ acc = 0.0;
      ਜੇ (!src) ਵਾਪਸ acc;
      ਲਈ (size_t i = 0; i < src->get_ਮਾਪ(); i += 1) {
	    ਦੁੱਗਣਾ vi;
	    src->get_ਸ਼ਬਦ(i, vi);
	    acc += vi;
      }
      ਵਾਪਸ acc;
}

ਤਾਰਕਿਕ ਦਾ_UPARR_ਜੋੜ_REAL(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_ਕਤਾਰ_real* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ਕਤਾਰ_or_darray_real_from_net(thr, net, qsrc, dsrc);
      ਦੁੱਗਣਾ ਜੋੜ;
      ਜੇ (qsrc) ਜੋੜ = uparr_ਜੋੜ_ਸ਼ਬਦ_src_real(qsrc);
      ਨਹੀਂਤਾਂ ਜੇ (dsrc) ਜੋੜ = uparr_ਜੋੜ_ਸ਼ਬਦ_src_real(dsrc);
      ਨਹੀਂਤਾਂ {
	    vvp_ਕਤਾਰ* src_q = get_ਕਤਾਰ_object<vvp_ਕਤਾਰ_real>(thr, net);
	    vvp_ਕਤਾਰ_real* src = dynamic_cast<vvp_ਕਤਾਰ_real*>(src_q);
	    ਜੋੜ = uparr_ਜੋੜ_ਸ਼ਬਦ_src_real(src);
      }
      thr->ਧੱਕੋ_real(ਜੋੜ);
      ਵਾਪਸ ਸੱਚ;
}

/* sum() reduction returns the scalar sum (Verilog-style addition per word). */
ਸਾਂਚਾ <ਵਰਗ SRC>
ਸਥਿਰ vvp_vector4_t uparr_ਜੋੜ_ਸ਼ਬਦ_src(SRC* src, ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
{
      ਜੇ (!src) ਵਾਪਸ vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t acc(wid, BIT4_0);
      ਲਈ (size_t i = 0; i < src->get_ਮਾਪ(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_ਸ਼ਬਦ(i, vi);
	    acc.ਜੋੜੋ(vi);
      }
      ਵਾਪਸ acc;
}

ਤਾਰਕਿਕ ਦਾ_UPARR_ਜੋੜ_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_net_t* net = cp->net;
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      vvp_ਕਤਾਰ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ਕਤਾਰ_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t ਜੋੜ;
      ਜੇ (qsrc) ਜੋੜ = uparr_ਜੋੜ_ਸ਼ਬਦ_src(qsrc, wid);
      ਨਹੀਂਤਾਂ ਜੇ (dsrc) ਜੋੜ = uparr_ਜੋੜ_ਸ਼ਬਦ_src(dsrc, wid);
      ਨਹੀਂਤਾਂ {
	    vvp_ਕਤਾਰ* src_q = get_ਕਤਾਰ_object<vvp_ਕਤਾਰ_vec4>(thr, net);
	    vvp_ਕਤਾਰ_vec4* src = dynamic_cast<vvp_ਕਤਾਰ_vec4*>(src_q);
	    ਜੋੜ = uparr_ਜੋੜ_ਸ਼ਬਦ_src(src, wid);
      }
      thr->ਧੱਕੋ_vec4(ਜੋੜ);
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_UPARR_ਜੋੜ_PROP_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      size_t pid = cp->ਸੰਖਿਆ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];

      vvp_object_t& ਸਿਖਰ = thr->peek_object();
      vvp_cobject*cobj = ਸਿਖਰ.peek<vvp_cobject>();
      ਪ੍ਰਤਿਜ੍ਞਾ(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_ਕਤਾਰ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ਕਤਾਰ_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t ਜੋੜ =
	    qsrc ? uparr_ਜੋੜ_ਸ਼ਬਦ_src(qsrc, wid)
		 : uparr_ਜੋੜ_ਸ਼ਬਦ_src(dsrc, wid);
      thr->ਧੱਕੋ_vec4(ਜੋੜ);
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_UPARR_ਜੋੜ_OBJ_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      vvp_object_t src_obj;
      thr->ਕੱਢੋ_object(src_obj);

      vvp_ਕਤਾਰ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ਕਤਾਰ_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_vector4_t ਜੋੜ =
	    qsrc ? uparr_ਜੋੜ_ਸ਼ਬਦ_src(qsrc, wid)
		 : uparr_ਜੋੜ_ਸ਼ਬਦ_src(dsrc, wid);
      thr->ਧੱਕੋ_vec4(ਜੋੜ);
      ਵਾਪਸ ਸੱਚ;
}

/* xor() reduction returns the scalar sum (Verilog-style bitwise-XOR per word). */
ਸਾਂਚਾ <ਵਰਗ SRC>
ਸਥਿਰ vvp_vector4_t uparr_xor_ਸ਼ਬਦ_src(SRC* src, ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
{
      ਜੇ (!src) ਵਾਪਸ vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_0);
      ਲਈ (size_t i = 0; i < src->get_ਮਾਪ(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_ਸ਼ਬਦ(i, vi);
	    res ^= vi;
      }
      ਵਾਪਸ res;
}

ਤਾਰਕਿਕ ਦਾ_UPARR_XOR_V(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vvp_net_t* net = cp->net;
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = cp->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0];
      vvp_ਕਤਾਰ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ਕਤਾਰ_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      ਜੇ (qsrc) res = uparr_xor_ਸ਼ਬਦ_src(qsrc, wid);
      ਨਹੀਂਤਾਂ ਜੇ (dsrc) res = uparr_xor_ਸ਼ਬਦ_src(dsrc, wid);
      ਨਹੀਂਤਾਂ {
	    vvp_ਕਤਾਰ* src_q = get_ਕਤਾਰ_object<vvp_ਕਤਾਰ_vec4>(thr, net);
	    vvp_ਕਤਾਰ_vec4* src = dynamic_cast<vvp_ਕਤਾਰ_vec4*>(src_q);
	    res = uparr_xor_ਸ਼ਬਦ_src(src, wid);
      }
      thr->ਧੱਕੋ_vec4(res);
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਦਾ_VPI_CALL(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      vpip_execute_vpi_call(thr, cp->handle);

      ਜੇ (ਸਮਾਂ_ਸਾਰਣੀ_stopped()) {
	    ਜੇ (! ਸਮਾਂ_ਸਾਰਣੀ_ਮੁਕੰਮਲ())
		  ਸਮਾਂ_ਸਾਰਣੀ_vthread(thr, 0, ਝੂਠ);

	    ਵਾਪਸ ਝੂਠ;
      }

      ਵਾਪਸ ਸਮਾਂ_ਸਾਰਣੀ_ਮੁਕੰਮਲ()? ਝੂਠ : ਸੱਚ;
}

/* %wait <label>;
 * Implement the wait by locating the vvp_net_T for the event, and
 * adding this thread to the threads list for the event. The some
 * argument is the  reference to the functor to wait for. This must be
 * an event object of some sort.
 */
ਤਾਰਕਿਕ ਦਾ_WAIT(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(! thr->i_am_ਵਿੱਚ_function);
      ਪ੍ਰਤਿਜ੍ਞਾ(! thr->ਉਡੀਕ_for_event);
      thr->ਉਡੀਕ_for_event = 1;

	/* Add this thread to the list in the event. */
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (cp->net->fun);
      ਪ੍ਰਤਿਜ੍ਞਾ(ep);
      thr->wait_ਅਗਲਾ = ep->ਜੋੜੋ_ਉਡੀਕ_thread(thr);

	/* Return false to suspend this thread. */
      ਵਾਪਸ ਝੂਠ;
}

/*
 * Implement the %wait/fork (SystemVerilog) instruction by suspending
 * the current thread until all the detached children have finished.
 */
ਤਾਰਕਿਕ ਦਾ_WAIT_FORK(vthread_t thr, vvp_ਕੋਡ_t)
{
	/* If a %wait/fork is being executed then the parent thread
	 * cannot be waiting in a join or already waiting. */
      ਪ੍ਰਤਿਜ੍ਞਾ(! thr->i_am_ਵਿੱਚ_function);
      ਪ੍ਰਤਿਜ੍ਞਾ(! thr->i_am_joining);
      ਪ੍ਰਤਿਜ੍ਞਾ(! thr->i_am_ਉਡੀਕ);

	/* There should be no active children when waiting. */
      ਪ੍ਰਤਿਜ੍ਞਾ(thr->children.empty());

	/* If there are no detached children then there is nothing to
	 * wait for. */
      ਜੇ (thr->detached_children.empty()) ਵਾਪਸ ਸੱਚ;

	/* Flag that this process is waiting for the detached children
	 * to finish and suspend it. */
      thr->i_am_ਉਡੀਕ = 1;
      ਵਾਪਸ ਝੂਠ;
}

/*
 * %xnor
 */
ਤਾਰਕਿਕ ਦਾ_XNOR(vthread_t thr, vvp_ਕੋਡ_t)
{
      vvp_vector4_t valr = thr->ਕੱਢੋ_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      ਪ੍ਰਤਿਜ੍ਞਾ(vall.size() == valr.size());

      vall ^= valr;
      vall.invert();

      ਵਾਪਸ ਸੱਚ;
}

/*
 * %xor
 */
ਤਾਰਕਿਕ ਦਾ_XOR(vthread_t thr, vvp_ਕੋਡ_t)
{
      vvp_vector4_t valr = thr->ਕੱਢੋ_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      ਪ੍ਰਤਿਜ੍ਞਾ(vall.size() == valr.size());

      vall ^= valr;

      ਵਾਪਸ ਸੱਚ;
}


ਤਾਰਕਿਕ ਦਾ_ZOMBIE(vthread_t thr, vvp_ਕੋਡ_t)
{
      thr->pc = codespace_null();
      ਜੇ ((thr->ਮਾਪਾ == 0) && (thr->children.empty())) {
	    ਜੇ (thr->delay_delete)
		  ਸਮਾਂ_ਸਾਰਣੀ_del_thr(thr);
	    ਨਹੀਂਤਾਂ
		  vthread_delete(thr);
      }
      ਵਾਪਸ ਝੂਠ;
}

/*
 * This is a phantom opcode used to call user defined functions. It
 * is used in code generated by the .ufunc statement. It contains a
 * pointer to the executable code of the function and a pointer to
 * a ufunc_core object that has all the port information about the
 * function.
 */
ਸਥਿਰ ਤਾਰਕਿਕ do_exec_ufunc(vthread_t thr, vvp_ਕੋਡ_t cp, vthread_t ਬੱਚਾ)
{
      __vpiScope*ਬੱਚਾ_scope = cp->ufunc_core_ptr->func_scope();
      ਪ੍ਰਤਿਜ੍ਞਾ(ਬੱਚਾ_scope);

      ਪ੍ਰਤਿਜ੍ਞਾ(ਬੱਚਾ_scope->get_type_ਕੋਡ() == vpiFunction);
      ਪ੍ਰਤਿਜ੍ਞਾ(thr->children.empty());


        /* We can take a number of shortcuts because we know that a
           continuous assignment can only occur in a static scope. */
      ਪ੍ਰਤਿਜ੍ਞਾ(thr->wt_context == 0);
      ਪ੍ਰਤਿਜ੍ਞਾ(thr->rd_context == 0);

        /* If an automatic function, allocate a context for this call. */
      vvp_context_t ਬੱਚਾ_context = 0;
      ਜੇ (ਬੱਚਾ_scope->is_automatic()) {
            ਬੱਚਾ_context = vthread_alloc_context(ਬੱਚਾ_scope);
            thr->wt_context = ਬੱਚਾ_context;
            thr->rd_context = ਬੱਚਾ_context;
      }

      ਬੱਚਾ->wt_context = ਬੱਚਾ_context;
      ਬੱਚਾ->rd_context = ਬੱਚਾ_context;

	/* Copy all the inputs to the ufunc object to the port
	   variables of the function. This copies all the values
	   atomically. */
      cp->ufunc_core_ptr->ਸੌਂਪੋ_ਬਿੱਟ_ਮੰਜ਼ਿਲ_ports(ਬੱਚਾ_context);
      ਬੱਚਾ->delay_delete = 1;

      ਬੱਚਾ->ਮਾਪਾ = thr;
      thr->children.insert(ਬੱਚਾ);
	// This should be the only child
      ਪ੍ਰਤਿਜ੍ਞਾ(thr->children.size()==1);

      ਬੱਚਾ->is_scheduled = 1;
      ਬੱਚਾ->i_am_ਵਿੱਚ_function = 1;
      vthread_ਲੜੀਵਾਰ(ਬੱਚਾ);
      running_thread = thr;

      ਜੇ (ਬੱਚਾ->i_have_ended) {
	    do_join(thr, ਬੱਚਾ);
            ਵਾਪਸ ਸੱਚ;
      } ਨਹੀਂਤਾਂ {
	    thr->i_am_joining = 1;
	    ਵਾਪਸ ਝੂਠ;
      }
}

ਤਾਰਕਿਕ ਦਾ_EXEC_UFUNC_REAL(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      __vpiScope*ਬੱਚਾ_scope = cp->ufunc_core_ptr->func_scope();
      ਪ੍ਰਤਿਜ੍ਞਾ(ਬੱਚਾ_scope);

	/* Create a temporary thread and run it immediately. */
      vthread_t ਬੱਚਾ = vthread_new(cp->cptr, ਬੱਚਾ_scope);
      thr->ਧੱਕੋ_real(0.0);
      ਬੱਚਾ->ਦਲੀਲਾਂ_real.push_back(0);

      ਵਾਪਸ do_exec_ufunc(thr, cp, ਬੱਚਾ);
}

ਤਾਰਕਿਕ ਦਾ_EXEC_UFUNC_VEC4(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      __vpiScope*ਬੱਚਾ_scope = cp->ufunc_core_ptr->func_scope();
      ਪ੍ਰਤਿਜ੍ਞਾ(ਬੱਚਾ_scope);

      vpiScopeFunction*scope_func = dynamic_cast<vpiScopeFunction*>(ਬੱਚਾ_scope);
      ਪ੍ਰਤਿਜ੍ਞਾ(scope_func);

	/* Create a temporary thread and run it immediately. */
      vthread_t ਬੱਚਾ = vthread_new(cp->cptr, ਬੱਚਾ_scope);
      thr->ਧੱਕੋ_vec4(vvp_vector4_t(scope_func->get_func_ਚੌੜਾਈ(), scope_func->get_func_init_val()));
      ਬੱਚਾ->ਦਲੀਲਾਂ_vec4.push_back(0);

      ਵਾਪਸ do_exec_ufunc(thr, cp, ਬੱਚਾ);
}

/*
 * This is a phantom opcode used to harvest the result of calling a user
 * defined function. It is used in code generated by the .ufunc statement.
 */
ਤਾਰਕਿਕ ਦਾ_REAP_UFUNC(vthread_t thr, vvp_ਕੋਡ_t cp)
{
      __vpiScope*ਬੱਚਾ_scope = cp->ufunc_core_ptr->func_scope();
      ਪ੍ਰਤਿਜ੍ਞਾ(ਬੱਚਾ_scope);

	/* Copy the output from the result variable to the output
	   ports of the .ufunc device. */
      cp->ufunc_core_ptr->ਅੰਤ_thread();

        /* If an automatic function, free the context for this call. */
      ਜੇ (ਬੱਚਾ_scope->is_automatic()) {
            vthread_free_context(thr->rd_context, ਬੱਚਾ_scope);
            thr->wt_context = 0;
            thr->rd_context = 0;
      }

      ਵਾਪਸ ਸੱਚ;
}
