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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#ಆದರೆ_ಅಂತ್ಯ
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

ಬಳಸಿ ನಾಮಾಕಾಶ std;

/* This is the size of an unsigned long in bits. This is just a
   convenience macro. */
# define CPU_ಶಬ್ದ_ಬಿಟ್ಟುಗಳು (8*ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ))
# define ತುದಿ_ಬಿಟ್ (1UL << (CPU_ಶಬ್ದ_ಬಿಟ್ಟುಗಳು-1))

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

ರಚನೆ vthread_s {
      vthread_s();

      ಶೂನ್ಯ debug_dump(ostream&fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*label_ಪಠ್ಯ);

	/* This is the program counter. */
      vvp_ಸಂಕೇತ_t pc;
	/* These hold the private thread bits. */
      ಎಣಿಕೆ { FLAGS_ಎಣಿಕೆ = 512, ಪದಗಳು_ಎಣಿಕೆ = 16 };
      vvp_bit4_t flags[FLAGS_ಎಣಿಕೆ];

	/* These are the word registers. */
      ಒಕ್ಕೂಟ {
	    int64_t  w_int;
	    uint64_t w_uint;
      } ಪದಗಳು[ಪದಗಳು_ಎಣಿಕೆ];

	// These vectors are depths within the parent thread's
	// corresponding stack.  This is how the %ret/* instructions
	// get at parent thread arguments.
      vector<ಚಿಹ್ನೆರಹಿತ> ವಾದಗಳು_real;
      vector<ಚಿಹ್ನೆರಹಿತ> ವಾದಗಳು_str;
      vector<ಚಿಹ್ನೆರಹಿತ> ವಾದಗಳು_vec4;

    ಖಾಸಗಿ:
      vector<vvp_vector4_t>ರಾಶಿ_vec4;
    ಸಾರ್ವಜನಿಕ:
      ಸಾಲೊಳಗೆ vvp_vector4_t ತೆಗೆ_vec4(ಶೂನ್ಯ)
      {
	    ಪ್ರತಿಜ್ಞಾ(! ರಾಶಿ_vec4.empty());
	    vvp_vector4_t val = ರಾಶಿ_vec4.back();
	    ರಾಶಿ_vec4.pop_back();
	    ಹಿಂತಿರುಗಿ val;
      }
      ಸಾಲೊಳಗೆ ಶೂನ್ಯ ತಳ್ಳು_vec4(ಸ್ಥಿರಾಂಕ vvp_vector4_t&val)
      {
	    ರಾಶಿ_vec4.push_back(val);
      }
      ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ vvp_vector4_t& peek_vec4(ಚಿಹ್ನೆರಹಿತ ಆಳ)
      {
	    ಚಿಹ್ನೆರಹಿತ size = ರಾಶಿ_vec4.size();
	    ಪ್ರತಿಜ್ಞಾ(ಆಳ < size);
	    ಚಿಹ್ನೆರಹಿತ use_ಸೂಚ್ಯಂಕ = size-1-ಆಳ;
	    ಹಿಂತಿರುಗಿ ರಾಶಿ_vec4[use_ಸೂಚ್ಯಂಕ];
      }
      ಸಾಲೊಳಗೆ vvp_vector4_t& peek_vec4(ಶೂನ್ಯ)
      {
	    ಚಿಹ್ನೆರಹಿತ use_ಸೂಚ್ಯಂಕ = ರಾಶಿ_vec4.size();
	    ಪ್ರತಿಜ್ಞಾ(use_ಸೂಚ್ಯಂಕ >= 1);
	    ಹಿಂತಿರುಗಿ ರಾಶಿ_vec4[use_ಸೂಚ್ಯಂಕ-1];
      }
      ಸಾಲೊಳಗೆ ಶೂನ್ಯ poke_vec4(ಚಿಹ್ನೆರಹಿತ ಆಳ, ಸ್ಥಿರಾಂಕ vvp_vector4_t&val)
      {
	    ಪ್ರತಿಜ್ಞಾ(ಆಳ < ರಾಶಿ_vec4.size());
	    ಚಿಹ್ನೆರಹಿತ use_ಸೂಚ್ಯಂಕ = ರಾಶಿ_vec4.size()-1-ಆಳ;
	    ರಾಶಿ_vec4[use_ಸೂಚ್ಯಂಕ] = val;
      }
      ಸಾಲೊಳಗೆ ಶೂನ್ಯ ತೆಗೆ_vec4(ಚಿಹ್ನೆರಹಿತ cnt)
      {
	    ಆಗ (cnt > 0) {
		  ರಾಶಿ_vec4.pop_back();
		  cnt -= 1;
	    }
      }


    ಖಾಸಗಿ:
      vector<ದ್ವಿಗುಣ> ರಾಶಿ_real;
    ಸಾರ್ವಜನಿಕ:
      ಸಾಲೊಳಗೆ ದ್ವಿಗುಣ ತೆಗೆ_real(ಶೂನ್ಯ)
      {
	    ಪ್ರತಿಜ್ಞಾ(! ರಾಶಿ_real.empty());
	    ದ್ವಿಗುಣ val = ರಾಶಿ_real.back();
	    ರಾಶಿ_real.pop_back();
	    ಹಿಂತಿರುಗಿ val;
      }
      ಸಾಲೊಳಗೆ ಶೂನ್ಯ ತಳ್ಳು_real(ದ್ವಿಗುಣ val)
      {
	    ರಾಶಿ_real.push_back(val);
      }
      ಸಾಲೊಳಗೆ ದ್ವಿಗುಣ peek_real(ಚಿಹ್ನೆರಹಿತ ಆಳ)
      {
	    ಪ್ರತಿಜ್ಞಾ(ಆಳ < ರಾಶಿ_real.size());
	    ಚಿಹ್ನೆರಹಿತ use_ಸೂಚ್ಯಂಕ = ರಾಶಿ_real.size()-1-ಆಳ;
	    ಹಿಂತಿರುಗಿ ರಾಶಿ_real[use_ಸೂಚ್ಯಂಕ];
      }
      ಸಾಲೊಳಗೆ ಶೂನ್ಯ poke_real(ಚಿಹ್ನೆರಹಿತ ಆಳ, ದ್ವಿಗುಣ val)
      {
	    ಪ್ರತಿಜ್ಞಾ(ಆಳ < ರಾಶಿ_real.size());
	    ಚಿಹ್ನೆರಹಿತ use_ಸೂಚ್ಯಂಕ = ರಾಶಿ_real.size()-1-ಆಳ;
	    ರಾಶಿ_real[use_ಸೂಚ್ಯಂಕ] = val;
      }
      ಸಾಲೊಳಗೆ ಶೂನ್ಯ ತೆಗೆ_real(ಚಿಹ್ನೆರಹಿತ cnt)
      {
	    ಆಗ (cnt > 0) {
		  ರಾಶಿ_real.pop_back();
		  cnt -= 1;
	    }
      }

	/* Strings are operated on using a forth-like operator
	   set. Items at the top of the stack (back()) are the objects
	   operated on except for special cases. New objects are
	   pushed onto the top (back()) and pulled from the top
	   (back()) only. */
    ಖಾಸಗಿ:
      vector<string> ರಾಶಿ_str;
    ಸಾರ್ವಜನಿಕ:
      ಸಾಲೊಳಗೆ string ತೆಗೆ_str(ಶೂನ್ಯ)
      {
	    ಪ್ರತಿಜ್ಞಾ(! ರಾಶಿ_str.empty());
	    string val = ರಾಶಿ_str.back();
	    ರಾಶಿ_str.pop_back();
	    ಹಿಂತಿರುಗಿ val;
      }
      ಸಾಲೊಳಗೆ ಶೂನ್ಯ ತಳ್ಳು_str(ಸ್ಥಿರಾಂಕ string&val)
      {
	    ರಾಶಿ_str.push_back(val);
      }
      ಸಾಲೊಳಗೆ string&peek_str(ಚಿಹ್ನೆರಹಿತ ಆಳ)
      {
	    ಪ್ರತಿಜ್ಞಾ(ಆಳ<ರಾಶಿ_str.size());
	    ಚಿಹ್ನೆರಹಿತ use_ಸೂಚ್ಯಂಕ = ರಾಶಿ_str.size()-1-ಆಳ;
	    ಹಿಂತಿರುಗಿ ರಾಶಿ_str[use_ಸೂಚ್ಯಂಕ];
      }
      ಸಾಲೊಳಗೆ ಶೂನ್ಯ poke_str(ಚಿಹ್ನೆರಹಿತ ಆಳ, ಸ್ಥಿರಾಂಕ string&val)
      {
	    ಪ್ರತಿಜ್ಞಾ(ಆಳ < ರಾಶಿ_str.size());
	    ಚಿಹ್ನೆರಹಿತ use_ಸೂಚ್ಯಂಕ = ರಾಶಿ_str.size()-1-ಆಳ;
	    ರಾಶಿ_str[use_ಸೂಚ್ಯಂಕ] = val;
      }
      ಸಾಲೊಳಗೆ ಶೂನ್ಯ ತೆಗೆ_str(ಚಿಹ್ನೆರಹಿತ cnt)
      {
	    ಆಗ (cnt > 0) {
		  ರಾಶಿ_str.pop_back();
		  cnt -= 1;
	    }
      }

	/* Objects are also operated on in a stack. */
    ಖಾಸಗಿ:
      ಎಣಿಕೆ { ರಾಶಿ_OBJ_MAX_ಗಾತ್ರ = 32 };
      vvp_object_t ರಾಶಿ_obj[ರಾಶಿ_OBJ_MAX_ಗಾತ್ರ];
      ಚಿಹ್ನೆರಹಿತ ರಾಶಿ_obj_ಗಾತ್ರ;
    ಸಾರ್ವಜನಿಕ:
      ಸಾಲೊಳಗೆ vvp_object_t& peek_object(ಶೂನ್ಯ)
      {
	    ಪ್ರತಿಜ್ಞಾ(ರಾಶಿ_obj_ಗಾತ್ರ > 0);
	    ಹಿಂತಿರುಗಿ ರಾಶಿ_obj[ರಾಶಿ_obj_ಗಾತ್ರ-1];
      }
      ಸಾಲೊಳಗೆ ಶೂನ್ಯ ತೆಗೆ_object(vvp_object_t&obj)
      {
	    ಪ್ರತಿಜ್ಞಾ(ರಾಶಿ_obj_ಗಾತ್ರ > 0);
	    ರಾಶಿ_obj_ಗಾತ್ರ -= 1;
	    obj = ರಾಶಿ_obj[ರಾಶಿ_obj_ಗಾತ್ರ];
	    ರಾಶಿ_obj[ರಾಶಿ_obj_ಗಾತ್ರ].reset(0);
      }
      ಸಾಲೊಳಗೆ ಶೂನ್ಯ ತೆಗೆ_object(ಚಿಹ್ನೆರಹಿತ cnt, ಚಿಹ್ನೆರಹಿತ skip =0)
      {
	    ಪ್ರತಿಜ್ಞಾ((cnt+skip) <= ರಾಶಿ_obj_ಗಾತ್ರ);
	    ಗಾಗಿ (size_t idx = ರಾಶಿ_obj_ಗಾತ್ರ-skip-cnt ; idx < ರಾಶಿ_obj_ಗಾತ್ರ-skip ; idx += 1)
		  ರಾಶಿ_obj[idx].reset(0);
	    ರಾಶಿ_obj_ಗಾತ್ರ -= cnt;
	    ಗಾಗಿ (size_t idx = ರಾಶಿ_obj_ಗಾತ್ರ-skip ; idx < ರಾಶಿ_obj_ಗಾತ್ರ ; idx += 1)
		  ರಾಶಿ_obj[idx] = ರಾಶಿ_obj[idx+skip];
	    ಗಾಗಿ (size_t idx = ರಾಶಿ_obj_ಗಾತ್ರ ; idx < ರಾಶಿ_obj_ಗಾತ್ರ+skip ; idx += 1)
		  ರಾಶಿ_obj[idx].reset(0);
      }
      ಸಾಲೊಳಗೆ ಶೂನ್ಯ ತಳ್ಳು_object(ಸ್ಥಿರಾಂಕ vvp_object_t&obj)
      {
	    ಪ್ರತಿಜ್ಞಾ(ರಾಶಿ_obj_ಗಾತ್ರ < ರಾಶಿ_OBJ_MAX_ಗಾತ್ರ);
	    ರಾಶಿ_obj[ರಾಶಿ_obj_ಗಾತ್ರ] = obj;
	    ರಾಶಿ_obj_ಗಾತ್ರ += 1;
      }

	/* My parent sets this when it wants me to wake it up. */
      ಚಿಹ್ನೆರಹಿತ i_am_joining      :1;
      ಚಿಹ್ನೆರಹಿತ i_am_detached     :1;
      ಚಿಹ್ನೆರಹಿತ i_am_ಕಾಯುವಿಕೆ      :1;
      ಚಿಹ್ನೆರಹಿತ i_am_ಒಳ_function  :1; // True if running function code
      ಚಿಹ್ನೆರಹಿತ i_have_ended      :1;
      ಚಿಹ್ನೆರಹಿತ i_was_disabled    :1;
      ಚಿಹ್ನೆರಹಿತ ಕಾಯುವಿಕೆ_for_event :1;
      ಚಿಹ್ನೆರಹಿತ is_scheduled      :1;
      ಚಿಹ್ನೆರಹಿತ delay_delete      :1;
	/* This points to the children of the thread. */
      ಗಣ<ರಚನೆ vthread_s*>children;
	/* This points to the detached children of the thread. */
      ಗಣ<ರಚನೆ vthread_s*>detached_children;
	/* This points to my parent, if I have one. */
      ರಚನೆ vthread_s*ಪೋಷಕ;
	/* This points to the containing scope. */
      __vpiScope*ಪೋಷಕ_scope;
	/* This is used for keeping wait queues. */
      ರಚನೆ vthread_s*wait_ಮುಂದಿನ;
	/* These are used to access automatically allocated items. */
      vvp_context_t wt_context, rd_context;
	/* These are used to pass non-blocking event control information. */
      vvp_net_t*event;
      uint64_t ecount;
	/* Save the file/line information when available. */
    ಖಾಸಗಿ:
      ಅಕ್ಷರ *filenm_;
      ಚಿಹ್ನೆರಹಿತ lineno_;
    ಸಾರ್ವಜನಿಕ:
      ಶೂನ್ಯ ಗಣ_fileline(ಅಕ್ಷರ *filenm, ಚಿಹ್ನೆರಹಿತ lineno);
      string get_fileline();

      ಸಾಲೊಳಗೆ ಶೂನ್ಯ cleanup()
      {
	    ಆದರೆ (i_was_disabled) {
		  ರಾಶಿ_vec4.clear();
		  ರಾಶಿ_real.clear();
		  ರಾಶಿ_str.clear();
		  ತೆಗೆ_object(ರಾಶಿ_obj_ಗಾತ್ರ);
	    }
	    ಬಿಡುಗಡೆ(filenm_);
	    filenm_ = 0;
	    ಪ್ರತಿಜ್ಞಾ(ರಾಶಿ_vec4.empty());
	    ಪ್ರತಿಜ್ಞಾ(ರಾಶಿ_real.empty());
	    ಪ್ರತಿಜ್ಞಾ(ರಾಶಿ_str.empty());
	    ಪ್ರತಿಜ್ಞಾ(ರಾಶಿ_obj_ಗಾತ್ರ == 0);
      }
};

ಸಾಲೊಳಗೆ vthread_s::vthread_s()
{
      ರಾಶಿ_obj_ಗಾತ್ರ = 0;
      filenm_ = 0;
      lineno_ = 0;
}

ಶೂನ್ಯ vthread_s::ಗಣ_fileline(ಅಕ್ಷರ *filenm, ಚಿಹ್ನೆರಹಿತ lineno)
{
      ಪ್ರತಿಜ್ಞಾ(filenm);
      ಆದರೆ (!filenm_ || (strcmp(filenm_, filenm) != 0)) {
	    ಬಿಡುಗಡೆ(filenm_);
	    filenm_ = strdup(filenm);
      }
      lineno_ = lineno;
}

ಸಾಲೊಳಗೆ string vthread_s::get_fileline()
{
      ostringstream buf;
      ಆದರೆ (filenm_) {
	    buf << filenm_ << ":" << lineno_ << ": ";
      }
      string res = buf.str();
      ಹಿಂತಿರುಗಿ res;
}

ಶೂನ್ಯ vthread_s::debug_dump(ostream&fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*label)
{
      fd << "**** " << label << ಸಾಲು_ಅಂತ್ಯ;
      fd << "**** ThreadId: " << ಇದು << ", parent id: " << ಪೋಷಕ << ಸಾಲು_ಅಂತ್ಯ;

      fd << "**** Flags: ";
      ಗಾಗಿ (ಪೂರ್ಣಾಂಕ idx = 0 ; idx < FLAGS_ಎಣಿಕೆ ; idx += 1)
	    fd << flags[idx];
      fd << ಸಾಲು_ಅಂತ್ಯ;
      fd << "**** vec4 stack..." << ಸಾಲು_ಅಂತ್ಯ;
      ಗಾಗಿ (size_t idx = ರಾಶಿ_vec4.size() ; idx > 0 ; idx -= 1)
	    fd << "    " << (ರಾಶಿ_vec4.size()-idx) << ": " << ರಾಶಿ_vec4[idx-1] << ಸಾಲು_ಅಂತ್ಯ;
      fd << "**** str stack (" << ರಾಶಿ_str.size() << ")..." << ಸಾಲು_ಅಂತ್ಯ;
      fd << "**** obj stack (" << ರಾಶಿ_obj_ಗಾತ್ರ << ")..." << ಸಾಲು_ಅಂತ್ಯ;
      fd << "**** args_vec4 array (" << ವಾದಗಳು_vec4.size() << ")..." << ಸಾಲು_ಅಂತ್ಯ;
      ಗಾಗಿ (size_t idx = 0 ; idx < ವಾದಗಳು_vec4.size() ; idx += 1)
	    fd << "    " << idx << ": " << ವಾದಗಳು_vec4[idx] << ಸಾಲು_ಅಂತ್ಯ;
      fd << "**** file/line (";
      ಆದರೆ (filenm_) fd << filenm_;
      ಇಲ್ಲದಿದ್ದರೆ fd << "<no file name>";
      fd << ":" << lineno_ << ")" << ಸಾಲು_ಅಂತ್ಯ;
      fd << "**** Done ****" << ಸಾಲು_ಅಂತ್ಯ;
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
ಸ್ಥಿರ string filter_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*ಪಠ್ಯ)
{
      vector<ಅಕ್ಷರ> tmp (ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಪಠ್ಯ)+1);
      size_t dst = 0;
      ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*ptr = ಪಠ್ಯ ; *ptr ; ptr += 1) {
	    // Not an escape? Move on.
	    ಆದರೆ (*ptr != '\\') {
		  tmp[dst++] = *ptr;
		  ಮುಂದುವರಿ;
	    }

	    // Now we know that *ptr is pointing to a \ character and we
	    // have an octal sequence coming up. Advance the ptr and start
	    // processing octal digits.
	    ptr += 1;
	    ಆದರೆ (*ptr == 0)
		  ನಿಲ್ಲಿಸು;

	    ಅಕ್ಷರ byte = 0;
	    ಪೂರ್ಣಾಂಕ cnt = 3;
	    ಆಗ (*ptr && cnt > 0 && *ptr >= '0' && *ptr <= '7') {
		  byte *= 8;
		  byte += *ptr - '0';
		  cnt -= 1;
		  ptr += 1;
	    }

	    // null-bytes are supposed to be removed when assigning a string
	    // literal to a string.
	    ಆದರೆ (byte != '\0')
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
      ಹಿಂತಿರುಗಿ res;
}

ಸ್ಥಿರ ಶೂನ್ಯ do_join(vthread_t thr, vthread_t ಮಗು);

__vpiScope* vthread_scope(ರಚನೆ vthread_s*thr)
{
      ಹಿಂತಿರುಗಿ thr->ಪೋಷಕ_scope;
}

ರಚನೆ vthread_s*running_thread = 0;

string get_fileline()
{
      ಹಿಂತಿರುಗಿ running_thread->get_fileline();
}

ಶೂನ್ಯ vthread_ತಳ್ಳು(ರಚನೆ vthread_s*thr, ದ್ವಿಗುಣ val)
{
      thr->ತಳ್ಳು_real(val);
}

ಶೂನ್ಯ vthread_ತಳ್ಳು(ರಚನೆ vthread_s*thr, ಸ್ಥಿರಾಂಕ string&val)
{
      thr->ತಳ್ಳು_str(val);
}

ಶೂನ್ಯ vthread_ತಳ್ಳು(ರಚನೆ vthread_s*thr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&val)
{
      thr->ತಳ್ಳು_vec4(val);
}

ಶೂನ್ಯ vthread_ತೆಗೆ_real(ರಚನೆ vthread_s*thr, ಚಿಹ್ನೆರಹಿತ ಆಳ)
{
      thr->ತೆಗೆ_real(ಆಳ);
}

ಶೂನ್ಯ vthread_ತೆಗೆ_str(ರಚನೆ vthread_s*thr, ಚಿಹ್ನೆರಹಿತ ಆಳ)
{
      thr->ತೆಗೆ_str(ಆಳ);
}

ಶೂನ್ಯ vthread_ತೆಗೆ_vec4(ರಚನೆ vthread_s*thr, ಚಿಹ್ನೆರಹಿತ ಆಳ)
{
      thr->ತೆಗೆ_vec4(ಆಳ);
}

ದ್ವಿಗುಣ vthread_get_real_ರಾಶಿ(ರಚನೆ vthread_s*thr, ಚಿಹ್ನೆರಹಿತ ಆಳ)
{
      ಹಿಂತಿರುಗಿ thr->peek_real(ಆಳ);
}

ಸ್ಥಿರಾಂಕ string&vthread_get_str_ರಾಶಿ(ರಚನೆ vthread_s*thr, ಚಿಹ್ನೆರಹಿತ ಆಳ)
{
      ಹಿಂತಿರುಗಿ thr->peek_str(ಆಳ);
}

ಸ್ಥಿರಾಂಕ vvp_vector4_t& vthread_get_vec4_ರಾಶಿ(ರಚನೆ vthread_s*thr, ಚಿಹ್ನೆರಹಿತ ಆಳ)
{
      ಹಿಂತಿರುಗಿ thr->peek_vec4(ಆಳ);
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
ಮಾದರಿ <ವರ್ಗ VVP_ಸರತಿ> ಸ್ಥಿರ vvp_ಸರತಿ*get_ಸರತಿ_object(vthread_t thr, vvp_net_t*net)
{
      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      ಪ್ರತಿಜ್ಞಾ(obj);

      vvp_ಸರತಿ*ಸರತಿ = obj->get_object().peek<vvp_ಸರತಿ>();
      ಆದರೆ (ಸರತಿ == 0) {
	    ಪ್ರತಿಜ್ಞಾ(obj->get_object().test_nil());
	    ಸರತಿ = ಹೊಸ VVP_ಸರತಿ;
	    vvp_object_t val (ಸರತಿ);
	    vvp_net_ptr_t ptr (net, 0);
	    vvp_send_object(ptr, val, thr->wt_context);
      }

      ಹಿಂತಿರುಗಿ ಸರತಿ;
}

/*
 * Locator and size/word opcodes use the same VVP net for a queue ($) or a
 * dynamic array ([]). Queues are vvp_queue_vec4; dynamic arrays may be
 * vvp_darray_vec4, vvp_darray_atom<>, etc. (all vvp_darray, but vvp_queue
 * also derives from vvp_darray — test queue vec4 first).
 */
ಸ್ಥಿರ ಶೂನ್ಯ get_ಸರತಿ_or_darray_vec4_from_net(vthread_t thr, vvp_net_t* net,
					      vvp_ಸರತಿ_vec4*& qsrc,
					      vvp_darray*& dsrc)
{
      vvp_fun_signal_object* obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      ಪ್ರತಿಜ್ಞಾ(obj);
      vvp_object_t ತುದಿ = obj->get_object();
      qsrc = ತುದಿ.peek<vvp_ಸರತಿ_vec4>();
      ಆದರೆ (qsrc) {
	    dsrc = 0;
	    ಹಿಂತಿರುಗಿ;
      }
      vvp_darray* dany = ತುದಿ.peek<vvp_darray>();
      ಆದರೆ (dany && dynamic_cast<vvp_ಸರತಿ*>(dany) == 0) {
	    dsrc = dany;
	    ಹಿಂತಿರುಗಿ;
      }
      ಆದರೆ (ತುದಿ.test_nil()) {
	    qsrc = ಹೊಸ vvp_ಸರತಿ_vec4();
	    vvp_object_t val(qsrc);
	    vvp_net_ptr_t ptr(net, 0);
	    vvp_send_object(ptr, val, thr->wt_context);
	    dsrc = 0;
	    ಹಿಂತಿರುಗಿ;
      }
      /* e.g. real/string queue — not vec4 queue or plain darray */
      qsrc = 0;
      dsrc = 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ get_ಸರತಿ_or_darray_real_from_net(vthread_t thr, vvp_net_t* net,
					      vvp_ಸರತಿ_real*& qsrc,
					      vvp_darray*& dsrc)
{
      vvp_fun_signal_object* obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      ಪ್ರತಿಜ್ಞಾ(obj);
      vvp_object_t ತುದಿ = obj->get_object();
      qsrc = ತುದಿ.peek<vvp_ಸರತಿ_real>();
      ಆದರೆ (qsrc) {
	    dsrc = 0;
	    ಹಿಂತಿರುಗಿ;
      }
      vvp_darray* dany = ತುದಿ.peek<vvp_darray>();
      ಆದರೆ (dany && dynamic_cast<vvp_ಸರತಿ*>(dany) == 0) {
	    dsrc = dany;
	    ಹಿಂತಿರುಗಿ;
      }
      ಆದರೆ (ತುದಿ.test_nil()) {
	    qsrc = ಹೊಸ vvp_ಸರತಿ_real();
	    vvp_object_t val(qsrc);
	    vvp_net_ptr_t ptr(net, 0);
	    vvp_send_object(ptr, val, thr->wt_context);
	    dsrc = 0;
	    ಹಿಂತಿರುಗಿ;
      }
      /* e.g. real/string queue — not vec4 queue or plain darray */
      qsrc = 0;
      dsrc = 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ get_ಸರತಿ_or_darray_vec4_from_object(vvp_object_t src_obj,
						 vvp_ಸರತಿ_vec4*& qsrc,
						 vvp_darray*& dsrc)
{
      qsrc = src_obj.peek<vvp_ಸರತಿ_vec4>();
      ಆದರೆ (qsrc) {
	    dsrc = 0;
	    ಹಿಂತಿರುಗಿ;
      }
      vvp_darray* dany = src_obj.peek<vvp_darray>();
      ಆದರೆ (dany && dynamic_cast<vvp_ಸರತಿ*>(dany) == 0) {
	    dsrc = dany;
	    ಹಿಂತಿರುಗಿ;
      }
      qsrc = 0;
      dsrc = 0;
}

/*
 * The following are used to allow a common template to be written for
 * queue real/string/vec4 operations
 */
ಸಾಲೊಳಗೆ ಸ್ಥಿರ ಶೂನ್ಯ ತೆಗೆ_ಮೌಲ್ಯ(vthread_t thr, ದ್ವಿಗುಣ&value, ಚಿಹ್ನೆರಹಿತ)
{
      value = thr->ತೆಗೆ_real();
}

ಸಾಲೊಳಗೆ ಸ್ಥಿರ ಶೂನ್ಯ ತೆಗೆ_ಮೌಲ್ಯ(vthread_t thr, string&value, ಚಿಹ್ನೆರಹಿತ)
{
      value = thr->ತೆಗೆ_str();
}

ಸಾಲೊಳಗೆ ಸ್ಥಿರ ಶೂನ್ಯ ತೆಗೆ_ಮೌಲ್ಯ(vthread_t thr, vvp_vector4_t&value, ಚಿಹ್ನೆರಹಿತ wid)
{
      value = thr->ತೆಗೆ_vec4();
      ಪ್ರತಿಜ್ಞಾ(value.size() == wid);
}

/*
 * The following are used to allow the queue templates to print correctly.
 */
ಸಾಲೊಳಗೆ ಸ್ಥಿರ string get_ಸರತಿ_type(ದ್ವಿಗುಣ&)
{
      ಹಿಂತಿರುಗಿ "queue<real>";
}

ಸಾಲೊಳಗೆ ಸ್ಥಿರ string get_ಸರತಿ_type(string&)
{
      ಹಿಂತಿರುಗಿ "queue<string>";
}

ಸಾಲೊಳಗೆ ಸ್ಥಿರ string get_ಸರತಿ_type(ಸ್ಥಿರಾಂಕ vvp_vector4_t&value)
{
      ostringstream buf;
      buf << "queue<vector[" << value.size() << "]>";
      string res = buf.str();
      ಹಿಂತಿರುಗಿ res;
}

ಸಾಲೊಳಗೆ ಸ್ಥಿರ ಶೂನ್ಯ print_ಸರತಿ_ಮೌಲ್ಯ(ದ್ವಿಗುಣ value)
{
      cerr << value;
}

ಸಾಲೊಳಗೆ ಸ್ಥಿರ ಶೂನ್ಯ print_ಸರತಿ_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ string&value)
{
      cerr << "\"" << value << "\"";
}

ಸಾಲೊಳಗೆ ಸ್ಥಿರ ಶೂನ್ಯ print_ಸರತಿ_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ vvp_vector4_t&value)
{
      cerr << value;
}

/*
 * The following are used to get a darray/queue default value.
 */
ಸಾಲೊಳಗೆ ಸ್ಥಿರ ಶೂನ್ಯ dq_default(ದ್ವಿಗುಣ&value, ಚಿಹ್ನೆರಹಿತ)
{
      value = 0.0;
}

ಸಾಲೊಳಗೆ ಸ್ಥಿರ ಶೂನ್ಯ dq_default(string&value, ಚಿಹ್ನೆರಹಿತ)
{
      value = "";
}

ಸಾಲೊಳಗೆ ಸ್ಥಿರ ಶೂನ್ಯ dq_default(vvp_vector4_t&value, ಚಿಹ್ನೆರಹಿತ wid)
{
      value = vvp_vector4_t(wid);
}


ಮಾದರಿ <ವರ್ಗ T> T coerce_ಗಮ್ಯ_ಅಗಲ(ಸ್ಥಿರಾಂಕ T&that, ಚಿಹ್ನೆರಹಿತ width)
{
      ಆದರೆ (that.size() == width)
	    ಹಿಂತಿರುಗಿ that;

      ಪ್ರತಿಜ್ಞಾ(that.size() > width);
      T res (width);
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < width ;  idx += 1)
	    res.ಗಣ_ಬಿಟ್(idx, that.value(idx));

      ಹಿಂತಿರುಗಿ res;
}

/* Explicitly define the vvp_vector4_t version of coerce_to_width(). */
ಮಾದರಿ vvp_vector4_t coerce_ಗಮ್ಯ_ಅಗಲ(ಸ್ಥಿರಾಂಕ vvp_vector4_t&that,
                                       ಚಿಹ್ನೆರಹಿತ width);


ಸ್ಥಿರ ಶೂನ್ಯ multiply_array_imm(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ*res, ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ*val,
			       ಚಿಹ್ನೆರಹಿತ ಪದಗಳು, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ imm)
{
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < ಪದಗಳು ; idx += 1)
	    res[idx] = 0;

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ mul_idx = 0 ; mul_idx < ಪದಗಳು ; mul_idx += 1) {
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮೊತ್ತ;
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tmp = multiply_with_ವಾಹಕ(val[mul_idx], imm, ಮೊತ್ತ);

	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ವಾಹಕ = 0;
	    res[mul_idx] = ಕೂಡಿಸು_with_ವಾಹಕ(res[mul_idx], tmp, ವಾಹಕ);
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ ಕೂಡಿಸು_idx = mul_idx+1 ; ಕೂಡಿಸು_idx < ಪದಗಳು ; ಕೂಡಿಸು_idx += 1) {
		  res[ಕೂಡಿಸು_idx] = ಕೂಡಿಸು_with_ವಾಹಕ(res[ಕೂಡಿಸು_idx], ಮೊತ್ತ, ವಾಹಕ);
		  ಮೊತ್ತ = 0;
	    }
      }
}

/*
 * Allocate a context for use by a child thread. By preference, use
 * the last freed context. If none available, create a new one. Add
 * it to the list of live contexts in that scope.
 */
ಸ್ಥಿರ vvp_context_t vthread_alloc_context(__vpiScope*scope)
{
      ಪ್ರತಿಜ್ಞಾ(scope->is_automatic());

      vvp_context_t context = scope->free_contexts;
      ಆದರೆ (context) {
            scope->free_contexts = vvp_get_ಮುಂದಿನ_context(context);
            ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < scope->nitem ; idx += 1) {
                  scope->ಅಂಶ[idx]->reset_instance(context);
            }
      } ಇಲ್ಲದಿದ್ದರೆ {
            context = vvp_allocate_context(scope->nitem);
            ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < scope->nitem ; idx += 1) {
                  scope->ಅಂಶ[idx]->alloc_instance(context);
            }
      }

      vvp_ಗಣ_ಮುಂದಿನ_context(context, scope->ಜೀವಂತ_contexts);
      scope->ಜೀವಂತ_contexts = context;

      ಹಿಂತಿರುಗಿ context;
}

/*
 * Free a context previously allocated to a child thread by pushing it
 * onto the freed context stack. Remove it from the list of live contexts
 * in that scope.
 */
ಸ್ಥಿರ ಶೂನ್ಯ vthread_free_context(vvp_context_t context, __vpiScope*scope)
{
      ಪ್ರತಿಜ್ಞಾ(scope->is_automatic());
      ಪ್ರತಿಜ್ಞಾ(context);

      ಆದರೆ (context == scope->ಜೀವಂತ_contexts) {
            scope->ಜೀವಂತ_contexts = vvp_get_ಮುಂದಿನ_context(context);
      } ಇಲ್ಲದಿದ್ದರೆ {
            vvp_context_t tmp = scope->ಜೀವಂತ_contexts;
            ಆಗ (context != vvp_get_ಮುಂದಿನ_context(tmp)) {
                  ಪ್ರತಿಜ್ಞಾ(tmp);
                  tmp = vvp_get_ಮುಂದಿನ_context(tmp);
            }
            vvp_ಗಣ_ಮುಂದಿನ_context(tmp, vvp_get_ಮುಂದಿನ_context(context));
      }

      vvp_ಗಣ_ಮುಂದಿನ_context(context, scope->free_contexts);
      scope->free_contexts = context;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಶೂನ್ಯ contexts_delete(ವರ್ಗ __vpiScope*scope)
{
      vvp_context_t context = scope->free_contexts;

      ಆಗ (context) {
	    scope->free_contexts = vvp_get_ಮುಂದಿನ_context(context);
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0; idx < scope->nitem; idx += 1) {
		  scope->ಅಂಶ[idx]->free_instance(context);
	    }
	    ಬಿಡುಗಡೆ(context);
	    context = scope->free_contexts;
      }
      ಬಿಡುಗಡೆ(scope->ಅಂಶ);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Create a new thread with the given start address.
 */
vthread_t vthread_new(vvp_ಸಂಕೇತ_t pc, __vpiScope*scope)
{
      vthread_t thr = ಹೊಸ ರಚನೆ vthread_s;
      thr->pc     = pc;
	//thr->bits4  = vvp_vector4_t(32);
      thr->ಪೋಷಕ = 0;
      thr->ಪೋಷಕ_scope = scope;
      thr->wait_ಮುಂದಿನ = 0;
      thr->wt_context = 0;
      thr->rd_context = 0;

      thr->i_am_joining  = 0;
      thr->i_am_detached = 0;
      thr->i_am_ಕಾಯುವಿಕೆ  = 0;
      thr->i_am_ಒಳ_function = 0;
      thr->is_scheduled  = 0;
      thr->i_have_ended  = 0;
      thr->i_was_disabled = 0;
      thr->delay_delete  = 0;
      thr->ಕಾಯುವಿಕೆ_for_event = 0;
      thr->event  = 0;
      thr->ecount = 0;

      thr->flags[0] = BIT4_0;
      thr->flags[1] = BIT4_1;
      thr->flags[2] = BIT4_X;
      thr->flags[3] = BIT4_Z;
      ಗಾಗಿ (ಪೂರ್ಣಾಂಕ idx = 4 ; idx < 8 ; idx += 1)
	    thr->flags[idx] = BIT4_X;

      scope->ಎಳೆಗಳು .insert(thr);
      ಹಿಂತಿರುಗಿ thr;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
#ಆದರೆ 0
/*
 * These are not currently correct. If you use them you will get
 * double delete messages. There is still a leak related to a
 * waiting event that needs to be investigated.
 */

ಸ್ಥಿರ ಶೂನ್ಯ wait_ಮುಂದಿನ_delete(vthread_t ತಳ)
{
      ಆಗ (ತಳ) {
	    vthread_t tmp = ತಳ->wait_ಮುಂದಿನ;
	    ಅಳಿಸು ತಳ;
	    ತಳ = tmp;
	    ಆದರೆ (ತಳ->ಕಾಯುವಿಕೆ_for_event == 0) ನಿಲ್ಲಿಸು;
      }
}

ಸ್ಥಿರ ಶೂನ್ಯ ಮಗು_delete(vthread_t ತಳ)
{
      ಆಗ (ತಳ) {
	    vthread_t tmp = ತಳ->ಮಗು;
	    ಅಳಿಸು ತಳ;
	    ತಳ = tmp;
      }
}
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ vthreads_delete(ವರ್ಗ __vpiScope*scope)
{
      ಗಾಗಿ (std::ಗಣ<vthread_t>::iterator cur = scope->ಎಳೆಗಳು.begin()
		 ; cur != scope->ಎಳೆಗಳು.ಕೊನೆ() ; ++ cur ) {
	    ಅಳಿಸು *cur;
      }
      scope->ಎಳೆಗಳು.clear();
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Reaping pulls the thread out of the stack of threads. If I have a
 * child, then hand it over to my parent or fully detach it.
 */
ಸ್ಥಿರ ಶೂನ್ಯ vthread_reap(vthread_t thr)
{
      ಆದರೆ (! thr->children.empty()) {
	    ಗಾಗಿ (ಗಣ<vthread_t>::iterator cur = thr->children.begin()
		       ; cur != thr->children.ಕೊನೆ() ; ++cur) {
		  vthread_t ಮಗು = *cur;
		  ಪ್ರತಿಜ್ಞಾ(ಮಗು);
		  ಪ್ರತಿಜ್ಞಾ(ಮಗು->ಪೋಷಕ == thr);
		  ಮಗು->ಪೋಷಕ = thr->ಪೋಷಕ;
	    }
      }
      ಆದರೆ (! thr->detached_children.empty()) {
	    ಗಾಗಿ (ಗಣ<vthread_t>::iterator cur = thr->detached_children.begin()
		       ; cur != thr->detached_children.ಕೊನೆ() ; ++cur) {
		  vthread_t ಮಗು = *cur;
		  ಪ್ರತಿಜ್ಞಾ(ಮಗು);
		  ಪ್ರತಿಜ್ಞಾ(ಮಗು->ಪೋಷಕ == thr);
		  ಪ್ರತಿಜ್ಞಾ(ಮಗು->i_am_detached);
		  ಮಗು->ಪೋಷಕ = 0;
		  ಮಗು->i_am_detached = 0;
	    }
      }
      ಆದರೆ (thr->ಪೋಷಕ) {
	      /* assert that the given element was removed. */
	    ಆದರೆ (thr->i_am_detached) {
		  size_t res = thr->ಪೋಷಕ->detached_children.erase(thr);
		  ಪ್ರತಿಜ್ಞಾ(res == 1);
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  size_t res = thr->ಪೋಷಕ->children.erase(thr);
		  ಪ್ರತಿಜ್ಞಾ(res == 1);
	    }
      }

      thr->ಪೋಷಕ = 0;

	// Remove myself from the containing scope if needed.
      thr->ಪೋಷಕ_scope->ಎಳೆಗಳು.erase(thr);

      thr->pc = codespace_null();

	/* If this thread is not scheduled, then is it safe to delete
	   it now. Otherwise, let the schedule event (which will
	   execute the thread at of_ZOMBIE) delete the object. */
      ಆದರೆ ((thr->is_scheduled == 0) && (thr->ಕಾಯುವಿಕೆ_for_event == 0)) {
	    ಪ್ರತಿಜ್ಞಾ(thr->children.empty());
	    ಪ್ರತಿಜ್ಞಾ(thr->wait_ಮುಂದಿನ == 0);
	    ಆದರೆ (thr->delay_delete)
		  ವೇಳಾಪಟ್ಟಿ_del_thr(thr);
	    ಇಲ್ಲದಿದ್ದರೆ
		  vthread_delete(thr);
      }
}

ಶೂನ್ಯ vthread_delete(vthread_t thr)
{
      thr->cleanup();
      ಅಳಿಸು thr;
}

ಶೂನ್ಯ vthread_mark_scheduled(vthread_t thr)
{
      ಆಗ (thr != 0) {
	    ಪ್ರತಿಜ್ಞಾ(thr->is_scheduled == 0);
	    thr->is_scheduled = 1;
	    thr = thr->wait_ಮುಂದಿನ;
      }
}

ಶೂನ್ಯ vthread_mark_final(vthread_t thr)
{
      /*
       * The behavior in a final thread is the same as in a function. Any
       * child thread will be executed immediately rather than being
       * scheduled.
       */
      ಆಗ (thr != 0) {
	    thr->i_am_ಒಳ_function = 1;
	    thr = thr->wait_ಮುಂದಿನ;
      }
}

ಶೂನ್ಯ vthread_delay_delete()
{
      ಆದರೆ (running_thread)
	    running_thread->delay_delete = 1;
}

/*
 * This function runs each thread by fetching an instruction,
 * incrementing the PC, and executing the instruction. The thread may
 * be the head of a list, so each thread is run so far as possible.
 */
ಶೂನ್ಯ vthread_ಸತತ(vthread_t thr)
{
      ಆಗ (thr != 0) {
	    vthread_t tmp = thr->wait_ಮುಂದಿನ;
	    thr->wait_ಮುಂದಿನ = 0;

	    ಪ್ರತಿಜ್ಞಾ(thr->is_scheduled);
	    thr->is_scheduled = 0;

            running_thread = thr;

	    ಗಾಗಿ (;;) {
		  vvp_ಸಂಕೇತ_t cp = thr->pc;
		  thr->pc += 1;

		    /* Run the opcode implementation. If the execution of
		       the opcode returns false, then the thread is meant to
		       be paused, so break out of the loop. */
		  ತಾರ್ಕಿಕ rc = (cp->opcode)(thr, cp);
		  ಆದರೆ (rc == ಸುಳ್ಳು)
			ನಿಲ್ಲಿಸು;
	    }

	    thr = tmp;
      }
      running_thread = 0;
}

/*
 * The CHUNK_LINK instruction is a special next pointer for linking
 * chunks of code space. It's like a simplified %jmp.
 */
ತಾರ್ಕಿಕ ದ_CHUNK_LINK(vthread_t thr, vvp_ಸಂಕೇತ_t code)
{
      ಪ್ರತಿಜ್ಞಾ(code->cptr);
      thr->pc = code->cptr;
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * This is called by an event functor to wake up all the threads on
 * its list. I in fact created that list in the %wait instruction, and
 * I also am certain that the waiting_for_event flag is set.
 */
ಶೂನ್ಯ vthread_ವೇಳಾಪಟ್ಟಿ_list(vthread_t thr)
{
      ಗಾಗಿ (vthread_t cur = thr ;  cur ;  cur = cur->wait_ಮುಂದಿನ) {
	    ಪ್ರತಿಜ್ಞಾ(cur->ಕಾಯುವಿಕೆ_for_event);
	    cur->ಕಾಯುವಿಕೆ_for_event = 0;
      }

      ವೇಳಾಪಟ್ಟಿ_vthread(thr, 0);
}

vvp_context_t vthread_get_wt_context()
{
      ಆದರೆ (running_thread)
            ಹಿಂತಿರುಗಿ running_thread->wt_context;
      ಇಲ್ಲದಿದ್ದರೆ
            ಹಿಂತಿರುಗಿ 0;
}

vvp_context_t vthread_get_rd_context()
{
      ಆದರೆ (running_thread)
            ಹಿಂತಿರುಗಿ running_thread->rd_context;
      ಇಲ್ಲದಿದ್ದರೆ
            ಹಿಂತಿರುಗಿ 0;
}

vvp_context_ಅಂಶ_t vthread_get_wt_context_ಅಂಶ(ಚಿಹ್ನೆರಹಿತ context_idx)
{
      ಪ್ರತಿಜ್ಞಾ(running_thread && running_thread->wt_context);
      ಹಿಂತಿರುಗಿ vvp_get_context_ಅಂಶ(running_thread->wt_context, context_idx);
}

vvp_context_ಅಂಶ_t vthread_get_rd_context_ಅಂಶ(ಚಿಹ್ನೆರಹಿತ context_idx)
{
      ಪ್ರತಿಜ್ಞಾ(running_thread && running_thread->rd_context);
      ಹಿಂತಿರುಗಿ vvp_get_context_ಅಂಶ(running_thread->rd_context, context_idx);
}

/*
 * %abs/wr
 */
ತಾರ್ಕಿಕ ದ_ABS_WR(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      thr->ತಳ್ಳು_real( fabs(thr->ತೆಗೆ_real()) );
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_ALLOC(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
        /* Allocate a context. */
      vvp_context_t ಮಗು_context = vthread_alloc_context(cp->scope);

        /* Push the allocated context onto the write context stack. */
      vvp_ಗಣ_stacked_context(ಮಗು_context, thr->wt_context);
      thr->wt_context = ಮಗು_context;

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_AND(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      vvp_vector4_t valb = thr->ತೆಗೆ_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();
      ಪ್ರತಿಜ್ಞಾ(vala.size() == valb.size());
      vala &= valb;
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * This function must ALWAYS be called with the val set to the right
 * size, and initialized with BIT4_0 bits. Certain optimizations rely
 * on that.
 */
ಸ್ಥಿರ ಶೂನ್ಯ get_ತಕ್ಷಣ_rval(vvp_ಸಂಕೇತ_t cp, vvp_vector4_t&val)
{
      uint32_t vala = cp->ಬಿಟ್_idx[0];
      uint32_t valb = cp->ಬಿಟ್_idx[1];
      ಚಿಹ್ನೆರಹಿತ wid  = cp->ಸಂಖ್ಯೆ;

      ಆದರೆ (valb == 0) {
	      // Special case: if the value is zero, we are done
	      // before we start.
	    ಆದರೆ (vala == 0) ಹಿಂತಿರುಗಿ;

	      // Special case: The value has no X/Z bits, so we can
	      // use the setarray method to write the value all at once.
	    ಚಿಹ್ನೆರಹಿತ use_wid = 8*ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ);
	    ಆದರೆ (wid < use_wid)
		  use_wid = wid;
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tmp[1];
	    tmp[0] = vala;
	    val.setarray(0, use_wid, tmp);
	    ಹಿಂತಿರುಗಿ;
      }

	// The immediate value can be values bigger then 32 bits, but
	// only if the high bits are zero. So at most we need to run
	// through the loop below 32 times. Maybe less, if the target
	// width is less. We don't have to do anything special on that
	// because vala/valb bits will shift away so (vala|valb) will
	// turn to zero at or before 32 shifts.

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < wid && (vala|valb) ; idx += 1) {
	    uint32_t ba = 0;
	      // Convert the vala/valb bits to a ba number that
	      // matches the encoding of the vvp_bit4_t enumeration.
	    ba = (valb & 1) << 1;
	    ba |= vala & 1;

	      // Note that the val is already pre-filled with BIT4_0
	      // bits, os we only need to set non-zero bit values.
	    ಆದರೆ (ba) val.ಗಣ_ಬಿಟ್(idx, (vvp_bit4_t)ba);

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
ತಾರ್ಕಿಕ ದ_ಕೂಡಿಸು(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      vvp_vector4_t r = thr->ತೆಗೆ_vec4();
	// Rather then pop l, use it directly from the stack. When we
	// assign to 'l', that will edit the top of the stack, which
	// replaces a pop and a pull.
      vvp_vector4_t&l = thr->peek_vec4();

      l.ಕೂಡಿಸು(r);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %addi <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
ತಾರ್ಕಿಕ ದ_ADDI(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಸಂಖ್ಯೆ;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_ತಕ್ಷಣ_rval (cp, r);

      l.ಕೂಡಿಸು(r);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %add/wr
 */
ತಾರ್ಕಿಕ ದ_ಕೂಡಿಸು_WR(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      ದ್ವಿಗುಣ r = thr->ತೆಗೆ_real();
      ದ್ವಿಗುಣ l = thr->ತೆಗೆ_real();
      thr->ತಳ್ಳು_real(l + r);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/* %assign/ar <array>, <delay>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory. <delay>
 * is the delay in simulation time. <bit> is the index register
 * containing the real value.
 */
ತಾರ್ಕಿಕ ದ_ನಿಯೋಜಿಸು_AR(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ದೀರ್ಘ adr = thr->ಪದಗಳು[3].w_int;
      ಚಿಹ್ನೆರಹಿತ delay = cp->ಬಿಟ್_idx[0];
      ದ್ವಿಗುಣ value = thr->ತೆಗೆ_real();

      ಆದರೆ (adr >= 0) {
	    ವೇಳಾಪಟ್ಟಿ_ನಿಯೋಜಿಸು_array_ಶಬ್ದ(cp->array, adr, value, delay);
      }

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/* %assign/ar/d <array>, <delay_idx>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory.
 * <delay_idx> is the integer register that contains the delay value.
 */
ತಾರ್ಕಿಕ ದ_ನಿಯೋಜಿಸು_ARD(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ದೀರ್ಘ adr = thr->ಪದಗಳು[3].w_int;
      ದ್ವಿಗುಣ value = thr->ತೆಗೆ_real();

      ಆದರೆ (adr >= 0) {
	    vvp_time64_t delay = thr->ಪದಗಳು[cp->ಬಿಟ್_idx[0]].w_uint;
	    ವೇಳಾಪಟ್ಟಿ_ನಿಯೋಜಿಸು_array_ಶಬ್ದ(cp->array, adr, value, delay);
      }

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/* %assign/ar/e <array>
 * Generate an assignment event to a real array. Index register 3
 * contains the canonical address of the word in the memory. <bit>
 * is the index register containing the real value. The event
 * information is contained in the thread event control registers
 * and is set with %evctl.
 */
ತಾರ್ಕಿಕ ದ_ನಿಯೋಜಿಸು_ARE(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ದೀರ್ಘ adr = thr->ಪದಗಳು[3].w_int;
      ದ್ವಿಗುಣ value = thr->ತೆಗೆ_real();

      ಆದರೆ (adr >= 0) {
	    ಆದರೆ (thr->ecount == 0) {
		  ವೇಳಾಪಟ್ಟಿ_ನಿಯೋಜಿಸು_array_ಶಬ್ದ(cp->array, adr, value, 0);
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  ವೇಳಾಪಟ್ಟಿ_evctl(cp->array, adr, value, thr->event,
		                 thr->ecount);
	    }
      }

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %assign/vec4 <var>, <delay>
 */
ತಾರ್ಕಿಕ ದ_ನಿಯೋಜಿಸು_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      ಚಿಹ್ನೆರಹಿತ delay = cp->ಬಿಟ್_idx[0];
      ಸ್ಥಿರಾಂಕ vvp_vector4_t&val = thr->peek_vec4();

      ವೇಳಾಪಟ್ಟಿ_ನಿಯೋಜಿಸು_ಸದಿಶ(ptr, 0, 0, val, delay);
      thr->ತೆಗೆ_vec4(1);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * Resizes a vector value for a partial assignment so that the value is fully
 * in-bounds of the target signal. Both `val` and `off` will be updated if
 * necessary.
 *
 * Returns false if the value is fully out-of-bounds and the assignment should
 * be skipped. Otherwise returns true.
 */
ಸ್ಥಿರ ತಾರ್ಕಿಕ resize_rval_vec(vvp_vector4_t &val, int64_t &off,
			    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ sig_wid)
{
      ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ wid = val.size();

        // Fully in bounds, most likely case
      ಆದರೆ (off >= 0 && (uint64_t)off + wid <= sig_wid)
	    ಹಿಂತಿರುಗಿ ನಿಜ;

      ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ತಳ = 0;
      ಆದರೆ (off >= 0) {
	      // Fully out-of-bounds
	    ಆದರೆ ((uint64_t)off >= sig_wid)
		  ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
      } ಇಲ್ಲದಿದ್ದರೆ {
	      // Fully out-of-bounds */
	    ಆದರೆ ((uint64_t)(-off) >= wid)
		  ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

	      // If the index is below the vector, then only assign the high
	      // bits that overlap with the target
	    ತಳ = -off;
	    wid += off;
		off = 0;
      }

	// If the value is partly above the target, then only assign
	// the bits that overlap
      ಆದರೆ ((uint64_t)off + wid > sig_wid)
	    wid = sig_wid - (uint64_t)off;

      val = val.subvalue(ತಳ, wid);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %assign/vec4/a/d <arr>, <offx>, <delx>
 */
ತಾರ್ಕಿಕ ದ_ನಿಯೋಜಿಸು_VEC4_A_D(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಪೂರ್ಣಾಂಕ off_idx = cp->ಬಿಟ್_idx[0];
      ಪೂರ್ಣಾಂಕ del_idx = cp->ಬಿಟ್_idx[1];
      ಪೂರ್ಣಾಂಕ adr_idx = 3;

      int64_t  off = off_idx ? thr->ಪದಗಳು[off_idx].w_int : 0;
      vvp_time64_t del = del_idx? thr->ಪದಗಳು[del_idx].w_uint : 0;
      ದೀರ್ಘ     adr = thr->ಪದಗಳು[adr_idx].w_int;

      vvp_vector4_t val = thr->ತೆಗೆ_vec4();

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      ಆದರೆ (thr->flags[4] == BIT4_1)
	    ಹಿಂತಿರುಗಿ ನಿಜ;

      ಆದರೆ (!resize_rval_vec(val, off, cp->array->get_ಶಬ್ದ_ಗಾತ್ರ()))
	    ಹಿಂತಿರುಗಿ ನಿಜ;

      ವೇಳಾಪಟ್ಟಿ_ನಿಯೋಜಿಸು_array_ಶಬ್ದ(cp->array, adr, off, val, del);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %assign/vec4/a/e <arr>, <offx>
 */
ತಾರ್ಕಿಕ ದ_ನಿಯೋಜಿಸು_VEC4_A_E(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಪೂರ್ಣಾಂಕ off_idx = cp->ಬಿಟ್_idx[0];
      ಪೂರ್ಣಾಂಕ adr_idx = 3;

      int64_t  off = off_idx ? thr->ಪದಗಳು[off_idx].w_int : 0;
      ದೀರ್ಘ     adr = thr->ಪದಗಳು[adr_idx].w_int;

      vvp_vector4_t val = thr->ತೆಗೆ_vec4();

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      ಆದರೆ (thr->flags[4] == BIT4_1)
	    ಹಿಂತಿರುಗಿ ನಿಜ;

      ಆದರೆ (!resize_rval_vec(val, off, cp->array->get_ಶಬ್ದ_ಗಾತ್ರ()))
	    ಹಿಂತಿರುಗಿ ನಿಜ;

      ಆದರೆ (thr->ecount == 0) {
	    ವೇಳಾಪಟ್ಟಿ_ನಿಯೋಜಿಸು_array_ಶಬ್ದ(cp->array, adr, off, val, 0);
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ವೇಳಾಪಟ್ಟಿ_evctl(cp->array, adr, val, off, thr->event, thr->ecount);
      }

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %assign/vec4/off/d <var>, <off>, <del>
 */
ತಾರ್ಕಿಕ ದ_ನಿಯೋಜಿಸು_VEC4_OFF_D(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      ಚಿಹ್ನೆರಹಿತ off_ಸೂಚ್ಯಂಕ = cp->ಬಿಟ್_idx[0];
      ಚಿಹ್ನೆರಹಿತ del_ಸೂಚ್ಯಂಕ = cp->ಬಿಟ್_idx[1];
      vvp_vector4_t val = thr->ತೆಗೆ_vec4();

      int64_t off = thr->ಪದಗಳು[off_ಸೂಚ್ಯಂಕ].w_int;
      vvp_time64_t del = thr->ಪದಗಳು[del_ಸೂಚ್ಯಂಕ].w_uint;

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      ಆದರೆ (thr->flags[4] == BIT4_1)
	    ಹಿಂತಿರುಗಿ ನಿಜ;

      vvp_signal_ಮೌಲ್ಯ*sig = dynamic_cast<vvp_signal_ಮೌಲ್ಯ*> (cp->net->fil);
      ಪ್ರತಿಜ್ಞಾ(sig);

      ಆದರೆ (!resize_rval_vec(val, off, sig->ಮೌಲ್ಯ_ಗಾತ್ರ()))
	    ಹಿಂತಿರುಗಿ ನಿಜ;

      ವೇಳಾಪಟ್ಟಿ_ನಿಯೋಜಿಸು_ಸದಿಶ(ptr, off, sig->ಮೌಲ್ಯ_ಗಾತ್ರ(), val, del);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %assign/vec4/off/e <var>, <off>
 */
ತಾರ್ಕಿಕ ದ_ನಿಯೋಜಿಸು_VEC4_OFF_E(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      ಚಿಹ್ನೆರಹಿತ off_ಸೂಚ್ಯಂಕ = cp->ಬಿಟ್_idx[0];
      vvp_vector4_t val = thr->ತೆಗೆ_vec4();

      int64_t off = thr->ಪದಗಳು[off_ಸೂಚ್ಯಂಕ].w_int;

	// Abort if flags[4] is set. This can happen if the calculation
	// into an index register failed.
      ಆದರೆ (thr->flags[4] == BIT4_1)
	    ಹಿಂತಿರುಗಿ ನಿಜ;

      vvp_signal_ಮೌಲ್ಯ*sig = dynamic_cast<vvp_signal_ಮೌಲ್ಯ*> (cp->net->fil);
      ಪ್ರತಿಜ್ಞಾ(sig);

      ಆದರೆ (!resize_rval_vec(val, off, sig->ಮೌಲ್ಯ_ಗಾತ್ರ()))
	    ಹಿಂತಿರುಗಿ ನಿಜ;

      ಆದರೆ (thr->ecount == 0) {
	    ವೇಳಾಪಟ್ಟಿ_ನಿಯೋಜಿಸು_ಸದಿಶ(ptr, off, sig->ಮೌಲ್ಯ_ಗಾತ್ರ(), val, 0);
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ವೇಳಾಪಟ್ಟಿ_evctl(ptr, val, off, sig->ಮೌಲ್ಯ_ಗಾತ್ರ(), thr->event, thr->ecount);
      }

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %assign/vec4/d <var-label> <delay>
 */
ತಾರ್ಕಿಕ ದ_ನಿಯೋಜಿಸು_VEC4_D(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      ಚಿಹ್ನೆರಹಿತ del_ಸೂಚ್ಯಂಕ = cp->ಬಿಟ್_idx[0];
      vvp_time64_t del = thr->ಪದಗಳು[del_ಸೂಚ್ಯಂಕ].w_int;

      vvp_vector4_t value = thr->ತೆಗೆ_vec4();

      vvp_signal_ಮೌಲ್ಯ*sig = dynamic_cast<vvp_signal_ಮೌಲ್ಯ*> (cp->net->fil);
      ಪ್ರತಿಜ್ಞಾ(sig);

      ವೇಳಾಪಟ್ಟಿ_ನಿಯೋಜಿಸು_ಸದಿಶ(ptr, 0, sig->ಮೌಲ್ಯ_ಗಾತ್ರ(), value, del);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %assign/vec4/e <var-label>
 */
ತಾರ್ಕಿಕ ದ_ನಿಯೋಜಿಸು_VEC4_E(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_vector4_t value = thr->ತೆಗೆ_vec4();

      vvp_signal_ಮೌಲ್ಯ*sig = dynamic_cast<vvp_signal_ಮೌಲ್ಯ*> (cp->net->fil);
      ಪ್ರತಿಜ್ಞಾ(sig);

      ಆದರೆ (thr->ecount == 0) {
	    ವೇಳಾಪಟ್ಟಿ_ನಿಯೋಜಿಸು_ಸದಿಶ(ptr, 0, sig->ಮೌಲ್ಯ_ಗಾತ್ರ(), value, 0);
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ವೇಳಾಪಟ್ಟಿ_evctl(ptr, value, 0, sig->ಮೌಲ್ಯ_ಗಾತ್ರ(), thr->event, thr->ecount);
      }

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * This is %assign/wr <vpi-label>, <delay>
 *
 * This assigns (after a delay) a value to a real variable. Use the
 * vpi_put_value function to do the assign, with the delay written
 * into the vpiInertialDelay carrying the desired delay.
 */
ತಾರ್ಕಿಕ ದ_ನಿಯೋಜಿಸು_WR(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ delay = cp->ಬಿಟ್_idx[0];
      ದ್ವಿಗುಣ value = thr->ತೆಗೆ_real();
      s_vpi_ಸಮಯ del;

      del.type = vpi_Sim_ಸಮಯ;
      vpip_ಸಮಯ_ಗಮ್ಯ_timestruct(&del, delay);

      __vpiHandle*tmp = cp->handle;

      t_vpi_ಮೌಲ್ಯ val;
      val.format = vpiRealVal;
      val.value.real = value;
      vpi_put_ಮೌಲ್ಯ(tmp, &val, &del, vpiTransportDelay);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_ನಿಯೋಜಿಸು_WRD(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_time64_t delay = thr->ಪದಗಳು[cp->ಬಿಟ್_idx[0]].w_uint;
      ದ್ವಿಗುಣ value = thr->ತೆಗೆ_real();
      s_vpi_ಸಮಯ del;

      del.type = vpi_Sim_ಸಮಯ;
      vpip_ಸಮಯ_ಗಮ್ಯ_timestruct(&del, delay);

      __vpiHandle*tmp = cp->handle;

      t_vpi_ಮೌಲ್ಯ val;
      val.format = vpiRealVal;
      val.value.real = value;
      vpi_put_ಮೌಲ್ಯ(tmp, &val, &del, vpiTransportDelay);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_ನಿಯೋಜಿಸು_WRE(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಪ್ರತಿಜ್ಞಾ(thr->event != 0);
      ದ್ವಿಗುಣ value = thr->ತೆಗೆ_real();
      __vpiHandle*tmp = cp->handle;

	// If the count is zero then just put the value.
      ಆದರೆ (thr->ecount == 0) {
	    t_vpi_ಮೌಲ್ಯ val;

	    val.format = vpiRealVal;
	    val.value.real = value;
	    vpi_put_ಮೌಲ್ಯ(tmp, &val, 0, vpiNoDelay);
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ವೇಳಾಪಟ್ಟಿ_evctl(tmp, value, thr->event, thr->ecount);
      }

      thr->event = 0;
      thr->ecount = 0;

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_BLEND(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      vvp_vector4_t vala = thr->ತೆಗೆ_vec4();
      vvp_vector4_t valb = thr->ತೆಗೆ_vec4();
      ಪ್ರತಿಜ್ಞಾ(vala.size() == valb.size());

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < vala.size() ; idx += 1) {
	    ಆದರೆ (vala.value(idx) == valb.value(idx))
		  ಮುಂದುವರಿ;

	    vala.ಗಣ_ಬಿಟ್(idx, BIT4_X);
      }

      thr->ತಳ್ಳು_vec4(vala);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_BLEND_STR(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      string f = thr->ತೆಗೆ_str();
      string t = thr->ತೆಗೆ_str();
      thr->ತಳ್ಳು_str((t == f) ? t : "");
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_BLEND_WR(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      ದ್ವಿಗುಣ f = thr->ತೆಗೆ_real();
      ದ್ವಿಗುಣ t = thr->ತೆಗೆ_real();
      thr->ತಳ್ಳು_real((t == f) ? t : 0.0);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_BREAKPOINT(vthread_t, vvp_ಸಂಕೇತ_t)
{
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %callf/void <code-label>, <scope-label>
 * Combine the %fork and %join steps for invoking a function.
 */
ಸ್ಥಿರ ತಾರ್ಕಿಕ do_callf_void(vthread_t thr, vthread_t ಮಗು)
{

      ಆದರೆ (ಮಗು->ಪೋಷಕ_scope->is_automatic()) {
	      /* The context allocated for this child is the top entry
		 on the write context stack */
	    ಮಗು->wt_context = thr->wt_context;
	    ಮಗು->rd_context = thr->wt_context;
      }

        // Mark the function thread as a direct child of the current thread.
      ಮಗು->ಪೋಷಕ = thr;
      thr->children.insert(ಮಗು);
        // This should be the only child
      ಪ್ರತಿಜ್ಞಾ(thr->children.size()==1);

        // Execute the function. This SHOULD run the function to completion,
        // but there are some exceptional situations where it won't.
      ಪ್ರತಿಜ್ಞಾ(ಮಗು->ಪೋಷಕ_scope->get_type_ಸಂಕೇತ() == vpiFunction);
      ಮಗು->is_scheduled = 1;
      ಮಗು->i_am_ಒಳ_function = 1;
      vthread_ಸತತ(ಮಗು);
      running_thread = thr;

      ಆದರೆ (ಮಗು->i_have_ended) {
	    do_join(thr, ಮಗು);
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    thr->i_am_joining = 1;
	    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
      }
}

ತಾರ್ಕಿಕ ದ_CALLF_OBJ(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vthread_t ಮಗು = vthread_new(cp->cptr2, cp->scope);
      ಹಿಂತಿರುಗಿ do_callf_void(thr, ಮಗು);

      // XXXX NOT IMPLEMENTED
}

ತಾರ್ಕಿಕ ದ_CALLF_REAL(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vthread_t ಮಗು = vthread_new(cp->cptr2, cp->scope);

	// This is the return value. Push a place-holder value. The function
	// will replace this with the actual value using a %ret/real instruction.
      thr->ತಳ್ಳು_real(0.0);
      ಮಗು->ವಾದಗಳು_real.push_back(0);

      ಹಿಂತಿರುಗಿ do_callf_void(thr, ಮಗು);
}

ತಾರ್ಕಿಕ ದ_CALLF_STR(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vthread_t ಮಗು = vthread_new(cp->cptr2, cp->scope);

      thr->ತಳ್ಳು_str("");
      ಮಗು->ವಾದಗಳು_str.push_back(0);

      ಹಿಂತಿರುಗಿ do_callf_void(thr, ಮಗು);
}

ತಾರ್ಕಿಕ ದ_CALLF_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vthread_t ಮಗು = vthread_new(cp->cptr2, cp->scope);

      vpiScopeFunction*scope_func = dynamic_cast<vpiScopeFunction*>(cp->scope);
      ಪ್ರತಿಜ್ಞಾ(scope_func);

	// This is the return value. Push a place-holder value. The function
	// will replace this with the actual value using a %ret/real instruction.
      thr->ತಳ್ಳು_vec4(vvp_vector4_t(scope_func->get_func_ಅಗಲ(), scope_func->get_func_init_val()));
      ಮಗು->ವಾದಗಳು_vec4.push_back(0);

      ಹಿಂತಿರುಗಿ do_callf_void(thr, ಮಗು);
}

ತಾರ್ಕಿಕ ದ_CALLF_VOID(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vthread_t ಮಗು = vthread_new(cp->cptr2, cp->scope);
      ಹಿಂತಿರುಗಿ do_callf_void(thr, ಮಗು);
}

/*
 * The %cassign/link instruction connects a source node to a
 * destination node. The destination node must be a signal, as it is
 * marked with the source of the cassign so that it may later be
 * unlinked without specifically knowing the source that this
 * instruction used.
 */
ತಾರ್ಕಿಕ ದ_CASSIGN_LINK(vthread_t, vvp_ಸಂಕೇತ_t cp)
{
      vvp_net_t*dst = cp->net;
      vvp_net_t*src = cp->net2;

      vvp_fun_signal_ತಳ*sig
	    = dynamic_cast<vvp_fun_signal_ತಳ*>(dst->fun);
      ಪ್ರತಿಜ್ಞಾ(sig);

	/* Any previous continuous assign should have been removed already. */
      ಪ್ರತಿಜ್ಞಾ(sig->cassign_link == 0);

      sig->cassign_link = src;

	/* Link the output of the src to the port[1] (the cassign
	   port) of the destination. */
      vvp_net_ptr_t dst_ptr (dst, 1);
      src->link(dst_ptr);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * If there is an existing continuous assign linked to the destination
 * node, unlink it. This must be done before applying a new continuous
 * assign, otherwise the initial assigned value will be propagated to
 * any other nodes driven by the old continuous assign source.
 */
ಸ್ಥಿರ ಶೂನ್ಯ cassign_unlink(vvp_net_t*dst)
{
      vvp_fun_signal_ತಳ*sig
	    = dynamic_cast<vvp_fun_signal_ತಳ*>(dst->fun);
      ಪ್ರತಿಜ್ಞಾ(sig);

      ಆದರೆ (sig->cassign_link == 0)
	    ಹಿಂತಿರುಗಿ;

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
ತಾರ್ಕಿಕ ದ_CASSIGN_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_net_t*net = cp->net;
      vvp_vector4_t value = thr->ತೆಗೆ_vec4();

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

	/* Set the value into port 1 of the destination. */
      vvp_net_ptr_t ptr (net, 1);
      vvp_send_vec4(ptr, value, 0);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %cassign/vec4/off <var>, <off>
 */
ತಾರ್ಕಿಕ ದ_CASSIGN_VEC4_OFF(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_net_t*net = cp->net;
      ಚಿಹ್ನೆರಹಿತ ತಳ_idx = cp->ಬಿಟ್_idx[0];
      ದೀರ್ಘ ತಳ = thr->ಪದಗಳು[ತಳ_idx].w_int;
      vvp_vector4_t value = thr->ತೆಗೆ_vec4();
      ಚಿಹ್ನೆರಹಿತ wid = value.size();

      ಆದರೆ (thr->flags[4] == BIT4_1)
	    ಹಿಂತಿರುಗಿ ನಿಜ;

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

      vvp_signal_ಮೌಲ್ಯ*sig = dynamic_cast<vvp_signal_ಮೌಲ್ಯ*> (net->fil);
      ಪ್ರತಿಜ್ಞಾ(sig);

      ಆದರೆ (ತಳ < 0 && (wid <= (ಚಿಹ್ನೆರಹಿತ)-ತಳ))
	    ಹಿಂತಿರುಗಿ ನಿಜ;

      ಆದರೆ (ತಳ >= (ದೀರ್ಘ)sig->ಮೌಲ್ಯ_ಗಾತ್ರ())
	    ಹಿಂತಿರುಗಿ ನಿಜ;

      ಆದರೆ (ತಳ < 0) {
	    wid -= (ಚಿಹ್ನೆರಹಿತ) -ತಳ;
	    ತಳ = 0;
	    value.resize(wid);
      }

      ಆದರೆ (ತಳ+wid > sig->ಮೌಲ್ಯ_ಗಾತ್ರ()) {
	    wid = sig->ಮೌಲ್ಯ_ಗಾತ್ರ() - ತಳ;
	    value.resize(wid);
      }

      vvp_net_ptr_t ptr (net, 1);
      vvp_send_vec4_pv(ptr, value, ತಳ, sig->ಮೌಲ್ಯ_ಗಾತ್ರ(), 0);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_CASSIGN_WR(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_net_t*net  = cp->net;
      ದ್ವಿಗುಣ value = thr->ತೆಗೆ_real();

	/* Remove any previous continuous assign to this net. */
      cassign_unlink(net);

	/* Set the value into port 1 of the destination. */
      vvp_net_ptr_t ptr (net, 1);
      vvp_send_real(ptr, value, 0);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %cast2
 */
ತಾರ್ಕಿಕ ದ_CAST2(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      ಚಿಹ್ನೆರಹಿತ wid = val.size();

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < wid ; idx += 1) {
	    ಆಯ್ಕೆ (val.value(idx)) {
		ಸ್ಥಿತಿ BIT4_0:
		ಸ್ಥಿತಿ BIT4_1:
		  ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
		  val.ಗಣ_ಬಿಟ್(idx, BIT4_0);
		  ನಿಲ್ಲಿಸು;
	    }
      }

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ do_cast_vec_dar(vthread_t thr, vvp_ಸಂಕೇತ_t cp, ತಾರ್ಕಿಕ as_vec4)
{
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಸಂಖ್ಯೆ;

      vvp_object_t obj;
      thr->ತೆಗೆ_object(obj);

      vvp_darray*darray = obj.peek<vvp_darray>();
      ಪ್ರತಿಜ್ಞಾ(darray);

      vvp_vector4_t vec = darray->get_bitstream(as_vec4);
      ಆದರೆ (vec.size() != wid) {
	    cerr << thr->get_fileline()
	         << "VVP error: size mismatch when casting dynamic array to vector." << ಸಾಲು_ಅಂತ್ಯ;
            thr->ತಳ್ಳು_vec4(vvp_vector4_t(wid));
            ವೇಳಾಪಟ್ಟಿ_stop(0);
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
      }
      thr->ತಳ್ಳು_vec4(vec);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %cast/vec2/dar <wid>
 */
ತಾರ್ಕಿಕ ದ_CAST_VEC2_DAR(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ do_cast_vec_dar(thr, cp, ಸುಳ್ಳು);
}

/*
 * %cast/vec4/dar <wid>
 */
ತಾರ್ಕಿಕ ದ_CAST_VEC4_DAR(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ do_cast_vec_dar(thr, cp, ನಿಜ);
}

/*
 * %cast/vec4/str <wid>
 */
ತಾರ್ಕಿಕ ದ_CAST_VEC4_STR(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಸಂಖ್ಯೆ;
      string str = thr->ತೆಗೆ_str();

      vvp_vector4_t vec(wid, BIT4_0);

      ಆದರೆ (wid != 8*str.length()) {
	    cerr << thr->get_fileline()
	         << "VVP error: size mismatch when casting string to vector." << ಸಾಲು_ಅಂತ್ಯ;
            thr->ತಳ್ಳು_vec4(vec);
            ವೇಳಾಪಟ್ಟಿ_stop(0);
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
      }

      ಚಿಹ್ನೆರಹಿತ sdx = 0;
      ಚಿಹ್ನೆರಹಿತ vdx = wid;
      ಆಗ (vdx > 0) {
            ಅಕ್ಷರ ch = str[sdx++];
            vdx -= 8;
            ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ bdx = 0; bdx < 8; bdx += 1) {
                  ಆದರೆ (ch & 1)
                        vec.ಗಣ_ಬಿಟ್(vdx+bdx, BIT4_1);
                  ch >>= 1;
            }
      }

      thr->ತಳ್ಳು_vec4(vec);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಸ್ಥಿರ ಶೂನ್ಯ do_CMPE(vthread_t thr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&lval, ಸ್ಥಿರಾಂಕ vvp_vector4_t&rval)
{
      ಪ್ರತಿಜ್ಞಾ(rval.size() == lval.size());

      ಆದರೆ (lval.has_xz() || rval.has_xz()) {

	    ಚಿಹ್ನೆರಹಿತ wid = lval.size();
	    vvp_bit4_t eq  = BIT4_1;
	    vvp_bit4_t eeq = BIT4_1;

	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < wid ; idx += 1) {
		  vvp_bit4_t lv = lval.value(idx);
		  vvp_bit4_t rv = rval.value(idx);

		  ಆದರೆ (lv != rv)
			eeq = BIT4_0;

		  ಆದರೆ (eq==BIT4_1 && (bit4_is_xz(lv) || bit4_is_xz(rv)))
			eq = BIT4_X;
		  ಆದರೆ ((lv == BIT4_0) && (rv==BIT4_1))
			eq = BIT4_0;
		  ಆದರೆ ((lv == BIT4_1) && (rv==BIT4_0))
			eq = BIT4_0;

		  ಆದರೆ (eq == BIT4_0)
			ನಿಲ್ಲಿಸು;
	    }

	    thr->flags[4] = eq;
	    thr->flags[6] = eeq;

      } ಇಲ್ಲದಿದ್ದರೆ {
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
ತಾರ್ಕಿಕ ದ_CMPE(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ಸ್ಥಿರಾಂಕ vvp_vector4_t&rval = thr->peek_vec4(0);
      ಸ್ಥಿರಾಂಕ vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPE(thr, lval, rval);

      thr->ತೆಗೆ_vec4(2);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_CMPNE(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ಸ್ಥಿರಾಂಕ vvp_vector4_t&rval = thr->peek_vec4(0);
      ಸ್ಥಿರಾಂಕ vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];
      thr->flags[6] =  ~thr->flags[6];

      thr->ತೆಗೆ_vec4(2);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %cmpi/e <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
ತಾರ್ಕಿಕ ದ_CMPIE(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಸಂಖ್ಯೆ;

      ಸ್ಥಿರಾಂಕ vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_ತಕ್ಷಣ_rval (cp, rval);

      do_CMPE(thr, lval, rval);

      thr->ತೆಗೆ_vec4(1);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_CMPINE(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಸಂಖ್ಯೆ;

      ಸ್ಥಿರಾಂಕ vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_ತಕ್ಷಣ_rval (cp, rval);

      do_CMPE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];
      thr->flags[6] =  ~thr->flags[6];

      thr->ತೆಗೆ_vec4(1);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}



ಸ್ಥಿರ ಶೂನ್ಯ do_CMPS(vthread_t thr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&lval, ಸ್ಥಿರಾಂಕ vvp_vector4_t&rval)
{
      ಪ್ರತಿಜ್ಞಾ(rval.size() == lval.size());

	// If either value has XZ bits, then the eq and lt values are
	// known already to be X. Just calculate the eeq result as a
	// special case and short circuit the rest of the compare.
      ಆದರೆ (lval.has_xz() || rval.has_xz()) {
	    thr->flags[4] = BIT4_X; // eq
	    thr->flags[5] = BIT4_X; // lt
	    thr->flags[6] = lval.eeq(rval)? BIT4_1 : BIT4_0;
	    ಹಿಂತಿರುಗಿ;
      }

	// Past this point, we know we are dealing only with fully
	// defined values.
      ಚಿಹ್ನೆರಹಿತ wid = lval.size();

      ಸ್ಥಿರಾಂಕ vvp_bit4_t sig1 = lval.value(wid-1);
      ಸ್ಥಿರಾಂಕ vvp_bit4_t sig2 = rval.value(wid-1);

	// If the lval is <0 and the rval is >=0, then we know the result.
      ಆದರೆ ((sig1 == BIT4_1) && (sig2 == BIT4_0)) {
	    thr->flags[4] = BIT4_0; // eq;
	    thr->flags[5] = BIT4_1; // lt;
	    thr->flags[6] = BIT4_0; // eeq
	    ಹಿಂತಿರುಗಿ;
      }

	// If the lval is >=0 and the rval is <0, then we know the result.
      ಆದರೆ ((sig1 == BIT4_0) && (sig2 == BIT4_1)) {
	    thr->flags[4] = BIT4_0; // eq;
	    thr->flags[5] = BIT4_0; // lt;
	    thr->flags[6] = BIT4_0; // eeq
	    ಹಿಂತಿರುಗಿ;
      }

	// The values have the same sign, so we have to look at the
	// actual value. Scan from the MSB down. As soon as we find a
	// bit that differs, we know the result.

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 1 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t lv = lval.value(wid-1-idx);
	    vvp_bit4_t rv = rval.value(wid-1-idx);

	    ಆದರೆ (lv == rv)
		  ಮುಂದುವರಿ;

	    thr->flags[4] = BIT4_0; // eq
	    thr->flags[6] = BIT4_0; // eeq

	    ಆದರೆ (lv==BIT4_0) {
		  thr->flags[5] = BIT4_1; // lt
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  thr->flags[5] = BIT4_0; // lt
	    }
	    ಹಿಂತಿರುಗಿ;
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
ತಾರ್ಕಿಕ ದ_CMPS(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ಸ್ಥಿರಾಂಕ vvp_vector4_t&rval = thr->peek_vec4(0);
      ಸ್ಥಿರಾಂಕ vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPS(thr, lval, rval);

      thr->ತೆಗೆ_vec4(2);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %cmpi/s <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
ತಾರ್ಕಿಕ ದ_CMPIS(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಸಂಖ್ಯೆ;

      ಸ್ಥಿರಾಂಕ vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_ತಕ್ಷಣ_rval (cp, rval);

      do_CMPS(thr, lval, rval);

      thr->ತೆಗೆ_vec4(1);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_CMPSTR(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      string re = thr->ತೆಗೆ_str();
      string le = thr->ತೆಗೆ_str();

      ಪೂರ್ಣಾಂಕ rc = strcmp(le.c_str(), re.c_str());

      vvp_bit4_t eq;
      vvp_bit4_t lt;

      ಆದರೆ (rc == 0) {
	    eq = BIT4_1;
	    lt = BIT4_0;
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (rc < 0) {
	    eq = BIT4_0;
	    lt = BIT4_1;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    eq = BIT4_0;
	    lt = BIT4_0;
      }

      thr->flags[4] = eq;
      thr->flags[5] = lt;

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಸ್ಥಿರ ಶೂನ್ಯ ದ_CMPU_the_hard_way(vthread_t thr, ಚಿಹ್ನೆರಹಿತ wid,
				 ಸ್ಥಿರಾಂಕ vvp_vector4_t&lval,
				 ಸ್ಥಿರಾಂಕ vvp_vector4_t&rval)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_bit4_t eeq = BIT4_1;

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < wid ;  idx += 1) {
	    vvp_bit4_t lv = lval.value(idx);
	    vvp_bit4_t rv = rval.value(idx);

	    ಆದರೆ (lv != rv)
		  eeq = BIT4_0;

	    ಆದರೆ (eq==BIT4_1 && (bit4_is_xz(lv) || bit4_is_xz(rv)))
		  eq = BIT4_X;
	    ಆದರೆ ((lv == BIT4_0) && (rv==BIT4_1))
		  eq = BIT4_0;
	    ಆದರೆ ((lv == BIT4_1) && (rv==BIT4_0))
		  eq = BIT4_0;

	    ಆದರೆ (eq == BIT4_0)
		  ನಿಲ್ಲಿಸು;

      }

      thr->flags[4] = eq;
      thr->flags[5] = BIT4_X;
      thr->flags[6] = eeq;
}

ಸ್ಥಿರ ಶೂನ್ಯ do_CMPU(vthread_t thr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&lval, ಸ್ಥಿರಾಂಕ vvp_vector4_t&rval)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_bit4_t lt = BIT4_0;

      ಆದರೆ (rval.size() != lval.size()) {
	    cerr << thr->get_fileline()
	         << "VVP ERROR: %cmp/u operand width mismatch: lval=" << lval
		 << ", rval=" << rval << ಸಾಲು_ಅಂತ್ಯ;
      }
      ಪ್ರತಿಜ್ಞಾ(rval.size() == lval.size());
      ಚಿಹ್ನೆರಹಿತ wid = lval.size();

      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ*larray = lval.subarray(0,wid);
      ಆದರೆ (larray == 0) ಹಿಂತಿರುಗಿ ದ_CMPU_the_hard_way(thr, wid, lval, rval);

      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ*rarray = rval.subarray(0,wid);
      ಆದರೆ (rarray == 0) {
	    ಅಳಿಸು[]larray;
	    ಹಿಂತಿರುಗಿ ದ_CMPU_the_hard_way(thr, wid, lval, rval);
      }

      ಚಿಹ್ನೆರಹಿತ ಪದಗಳು = (wid+CPU_ಶಬ್ದ_ಬಿಟ್ಟುಗಳು-1) / CPU_ಶಬ್ದ_ಬಿಟ್ಟುಗಳು;

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ wdx = 0 ; wdx < ಪದಗಳು ; wdx += 1) {
	    ಆದರೆ (larray[wdx] == rarray[wdx])
		  ಮುಂದುವರಿ;

	    eq = BIT4_0;
	    ಆದರೆ (larray[wdx] < rarray[wdx])
		  lt = BIT4_1;
	    ಇಲ್ಲದಿದ್ದರೆ
		  lt = BIT4_0;
      }

      ಅಳಿಸು[]larray;
      ಅಳಿಸು[]rarray;

      thr->flags[4] = eq;
      thr->flags[5] = lt;
      thr->flags[6] = eq;
}

ತಾರ್ಕಿಕ ದ_CMPU(vthread_t thr, vvp_ಸಂಕೇತ_t)
{

      ಸ್ಥಿರಾಂಕ vvp_vector4_t&rval = thr->peek_vec4(0);
      ಸ್ಥಿರಾಂಕ vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPU(thr, lval, rval);

      thr->ತೆಗೆ_vec4(2);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %cmpi/u <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments.
 */
ತಾರ್ಕಿಕ ದ_CMPIU(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಸಂಖ್ಯೆ;

      ಸ್ಥಿರಾಂಕ vvp_vector4_t&lval = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t rval (wid, BIT4_0);
      get_ತಕ್ಷಣ_rval (cp, rval);

      do_CMPU(thr, lval, rval);

      thr->ತೆಗೆ_vec4(1);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}


/*
 * %cmp/x
 */
ತಾರ್ಕಿಕ ದ_CMPX(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_vector4_t rval = thr->ತೆಗೆ_vec4();
      vvp_vector4_t lval = thr->ತೆಗೆ_vec4();

      ಪ್ರತಿಜ್ಞಾ(rval.size() == lval.size());
      ಚಿಹ್ನೆರಹಿತ wid = lval.size();

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < wid ; idx += 1) {
	    vvp_bit4_t lv = lval.value(idx);
	    vvp_bit4_t rv = rval.value(idx);
	    ಆದರೆ ((lv != rv) && !bit4_is_xz(lv) && !bit4_is_xz(rv)) {
		  eq = BIT4_0;
		  ನಿಲ್ಲಿಸು;
	    }
      }

      thr->flags[4] = eq;
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಸ್ಥಿರ ಶೂನ್ಯ do_CMPWE(vthread_t thr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&lval, ಸ್ಥಿರಾಂಕ vvp_vector4_t&rval)
{
      ಪ್ರತಿಜ್ಞಾ(rval.size() == lval.size());

      ಆದರೆ (lval.has_xz() || rval.has_xz()) {

	    ಚಿಹ್ನೆರಹಿತ wid = lval.size();
	    vvp_bit4_t eq  = BIT4_1;

	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < wid ; idx += 1) {
		  vvp_bit4_t lv = lval.value(idx);
		  vvp_bit4_t rv = rval.value(idx);

		  ಆದರೆ (bit4_is_xz(rv))
			ಮುಂದುವರಿ;
		  ಆದರೆ ((eq == BIT4_1) && bit4_is_xz(lv))
			eq = BIT4_X;
		  ಆದರೆ ((lv == BIT4_0) && (rv==BIT4_1))
			eq = BIT4_0;
		  ಆದರೆ ((lv == BIT4_1) && (rv==BIT4_0))
			eq = BIT4_0;

		  ಆದರೆ (eq == BIT4_0)
			ನಿಲ್ಲಿಸು;
	    }

	    thr->flags[4] = eq;

      } ಇಲ್ಲದಿದ್ದರೆ {
	      // If there are no XZ bits anywhere, then the results of
	      // ==? match the === test.
	    thr->flags[4] = (lval.eeq(rval)? BIT4_1 : BIT4_0);
      }
}

ತಾರ್ಕಿಕ ದ_CMPWE(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ಸ್ಥಿರಾಂಕ vvp_vector4_t&rval = thr->peek_vec4(0);
      ಸ್ಥಿರಾಂಕ vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPWE(thr, lval, rval);

      thr->ತೆಗೆ_vec4(2);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_CMPWNE(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
	// We are going to pop these and push nothing in their
	// place, but for now it is more efficient to use a constant
	// reference. When we finish, pop the stack without copies.
      ಸ್ಥಿರಾಂಕ vvp_vector4_t&rval = thr->peek_vec4(0);
      ಸ್ಥಿರಾಂಕ vvp_vector4_t&lval = thr->peek_vec4(1);

      do_CMPWE(thr, lval, rval);

      thr->flags[4] =  ~thr->flags[4];

      thr->ತೆಗೆ_vec4(2);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_CMPWR(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      ದ್ವಿಗುಣ r = thr->ತೆಗೆ_real();
      ದ್ವಿಗುಣ l = thr->ತೆಗೆ_real();

      vvp_bit4_t eq = (l == r)? BIT4_1 : BIT4_0;
      vvp_bit4_t lt = (l <  r)? BIT4_1 : BIT4_0;

      thr->flags[4] = eq;
      thr->flags[5] = lt;

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %cmp/z
 */
ತಾರ್ಕಿಕ ದ_CMPZ(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      vvp_bit4_t eq = BIT4_1;
      vvp_vector4_t rval = thr->ತೆಗೆ_vec4();
      vvp_vector4_t lval = thr->ತೆಗೆ_vec4();

      ಪ್ರತಿಜ್ಞಾ(rval.size() == lval.size());
      ಚಿಹ್ನೆರಹಿತ wid = lval.size();

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < wid ; idx += 1) {
	    vvp_bit4_t lv = lval.value(idx);
	    vvp_bit4_t rv = rval.value(idx);
	    ಆದರೆ ((lv != rv) && (rv != BIT4_Z) && (lv != BIT4_Z)) {
		  eq = BIT4_0;
		  ನಿಲ್ಲಿಸು;
	    }
      }

      thr->flags[4] = eq;
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 *  %concat/str;
 */
ತಾರ್ಕಿಕ ದ_CONCAT_STR(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      string ಪಠ್ಯ = thr->ತೆಗೆ_str();
      thr->peek_str(0).append(ಪಠ್ಯ);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 *  %concati/str <string>;
 */
ತಾರ್ಕಿಕ ದ_CONCATI_STR(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*ಪಠ್ಯ = cp->ಪಠ್ಯ;
      thr->peek_str(0).append(filter_string(ಪಠ್ಯ));
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %concat/vec4
 */
ತಾರ್ಕಿಕ ದ_CONCAT_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      ಸ್ಥಿರಾಂಕ vvp_vector4_t&lsb = thr->peek_vec4(0);
      ಸ್ಥಿರಾಂಕ vvp_vector4_t&msb = thr->peek_vec4(1);

	// The result is the size of the top two vectors in the stack.
      vvp_vector4_t res (msb.size() + lsb.size(), BIT4_X);

	// Build up the result.
      res.ಗಣ_vec(0, lsb);
      res.ಗಣ_vec(lsb.size(), msb);

	// Rearrange the stack to pop the inputs and push the
	// result. Do that by actually popping only 1 stack position
	// and replacing the new top with the new value.
      thr->ತೆಗೆ_vec4(1);
      thr->peek_vec4() = res;

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %concati/vec4 <vala>, <valb>, <wid>
 *
 * Concat the immediate value to the LOW bits of the concatenation.
 * Get the HIGH bits from the top of the vec4 stack.
 */
ತಾರ್ಕಿಕ ದ_CONCATI_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ wid  = cp->ಸಂಖ್ಯೆ;

      vvp_vector4_t&msb = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t lsb (wid, BIT4_0);
      get_ತಕ್ಷಣ_rval (cp, lsb);

      vvp_vector4_t res (msb.size()+lsb.size(), BIT4_X);
      res.ಗಣ_vec(0, lsb);
      res.ಗಣ_vec(lsb.size(), msb);

      msb = res;
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %cvt/rv
 */
ತಾರ್ಕಿಕ ದ_CVT_RV(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      ದ್ವಿಗುಣ val;
      vvp_vector4_t val4 = thr->ತೆಗೆ_vec4();
      vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(val4, val, ಸುಳ್ಳು);
      thr->ತಳ್ಳು_real(val);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %cvt/rv/s
 */
ತಾರ್ಕಿಕ ದ_CVT_RV_S(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      ದ್ವಿಗುಣ val;
      vvp_vector4_t val4 = thr->ತೆಗೆ_vec4();
      vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(val4, val, ನಿಜ);
      thr->ತಳ್ಳು_real(val);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %cvt/sr <idx>
 * Pop the top value from the real stack, convert it to a 64bit signed
 * and save it to the indexed register.
 */
ತಾರ್ಕಿಕ ದ_CVT_SR(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ದ್ವಿಗುಣ r = thr->ತೆಗೆ_real();
      thr->ಪದಗಳು[cp->ಬಿಟ್_idx[0]].w_int = i64round(r);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %cvt/ur <idx>
 */
ತಾರ್ಕಿಕ ದ_CVT_UR(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ದ್ವಿಗುಣ r = thr->ತೆಗೆ_real();
      thr->ಪದಗಳು[cp->ಬಿಟ್_idx[0]].w_uint = vlg_ಸುತ್ತು_ಗಮ್ಯ_u64(r);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %cvt/vr <wid>
 */
ತಾರ್ಕಿಕ ದ_CVT_VR(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ದ್ವಿಗುಣ r = thr->ತೆಗೆ_real();
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಸಂಖ್ಯೆ;

      vvp_vector4_t tmp(wid, r);
      thr->ತಳ್ಳು_vec4(tmp);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * This implements the %deassign instruction. All we do is write a
 * long(1) to port-3 of the addressed net. This turns off an active
 * continuous assign activated by %cassign/v
 */
ತಾರ್ಕಿಕ ದ_DEASSIGN(vthread_t, vvp_ಸಂಕೇತ_t cp)
{
      vvp_net_t*net = cp->net;
      ಚಿಹ್ನೆರಹಿತ ತಳ  = cp->ಬಿಟ್_idx[0];
      ಚಿಹ್ನೆರಹಿತ width = cp->ಬಿಟ್_idx[1];

      vvp_signal_ಮೌಲ್ಯ*fil = dynamic_cast<vvp_signal_ಮೌಲ್ಯ*> (net->fil);
      ಪ್ರತಿಜ್ಞಾ(fil);
      vvp_fun_signal_vec*sig = dynamic_cast<vvp_fun_signal_vec*>(net->fun);
      ಪ್ರತಿಜ್ಞಾ(sig);

      ಆದರೆ (ತಳ >= fil->ಮೌಲ್ಯ_ಗಾತ್ರ()) ಹಿಂತಿರುಗಿ ನಿಜ;
      ಆದರೆ (ತಳ+width > fil->ಮೌಲ್ಯ_ಗಾತ್ರ()) width = fil->ಮೌಲ್ಯ_ಗಾತ್ರ() - ತಳ;

      ತಾರ್ಕಿಕ ತುಂಬಿದ_sig = ತಳ == 0 && width == fil->ಮೌಲ್ಯ_ಗಾತ್ರ();

	// This is the net that is forcing me...
      ಆದರೆ (vvp_net_t*src = sig->cassign_link) {
	    ಆದರೆ (! ತುಂಬಿದ_sig) {
		  fprintf(stderr, "Sorry: when a signal is assigning a "
		          "register, I cannot deassign part of it.\n");
		  ನಿರ್ಗಮಿಸು(1);
	    }
	      // And this is the pointer to be removed.
	    vvp_net_ptr_t dst_ptr (net, 1);
	    src->unlink(dst_ptr);
	    sig->cassign_link = 0;
      }

	/* Do we release all or part of the net? */
      ಆದರೆ (ತುಂಬಿದ_sig) {
	    sig->deassign();
      } ಇಲ್ಲದಿದ್ದರೆ {
	    sig->deassign_pv(ತಳ, width);
      }

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_DEASSIGN_WR(vthread_t, vvp_ಸಂಕೇತ_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_real*sig = dynamic_cast<vvp_fun_signal_real*>(net->fun);
      ಪ್ರತಿಜ್ಞಾ(sig);

	// This is the net that is forcing me...
      ಆದರೆ (vvp_net_t*src = sig->cassign_link) {
	      // And this is the pointer to be removed.
	    vvp_net_ptr_t dst_ptr (net, 1);
	    src->unlink(dst_ptr);
	    sig->cassign_link = 0;
      }

      sig->deassign();

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %debug/thr
 */
ತಾರ್ಕಿಕ ದ_DEBUG_THR(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*ಪಠ್ಯ = cp->ಪಠ್ಯ;
      thr->debug_dump(cerr, ಪಠ್ಯ);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * The delay takes two 32bit numbers to make up a 64bit time.
 *
 *   %delay <low>, <hig>
 */
ತಾರ್ಕಿಕ ದ_DELAY(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_time64_t ಕೆಳಗಿನ = cp->ಬಿಟ್_idx[0];
      vvp_time64_t hig = cp->ಬಿಟ್_idx[1];

      vvp_time64_t delay = (hig << 32) | ಕೆಳಗಿನ;

      ಆದರೆ (delay == 0) ವೇಳಾಪಟ್ಟಿ_inactive(thr);
      ಇಲ್ಲದಿದ್ದರೆ ವೇಳಾಪಟ್ಟಿ_vthread(thr, delay);
      ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ತಾರ್ಕಿಕ ದ_DELAYX(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_time64_t delay;

      ಪ್ರತಿಜ್ಞಾ(cp->ಸಂಖ್ಯೆ < vthread_s::ಪದಗಳು_ಎಣಿಕೆ);
      delay = thr->ಪದಗಳು[cp->ಸಂಖ್ಯೆ].w_uint;
      ಆದರೆ (delay == 0) ವೇಳಾಪಟ್ಟಿ_inactive(thr);
      ಇಲ್ಲದಿದ್ದರೆ ವೇಳಾಪಟ್ಟಿ_vthread(thr, delay);
      ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ತಾರ್ಕಿಕ ದ_DELETE_ELEM(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_net_t*net = cp->net;

      int64_t idx_val = thr->ಪದಗಳು[3].w_int;
      ಆದರೆ (thr->flags[4] == BIT4_1) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with undefined index."
	         << ಸಾಲು_ಅಂತ್ಯ;
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }
      ಆದರೆ (idx_val < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with negative index."
	         << ಸಾಲು_ಅಂತ್ಯ;
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }
      size_t idx = idx_val;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      ಪ್ರತಿಜ್ಞಾ(obj);

      vvp_ಸರತಿ*ಸರತಿ = obj->get_object().peek<vvp_ಸರತಿ>();
      ಆದರೆ (ಸರತಿ == 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping delete(" << idx
	         << ") on empty queue." << ಸಾಲು_ಅಂತ್ಯ;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    size_t size = ಸರತಿ->get_ಗಾತ್ರ();
	    ಆದರೆ (idx >= size) {
		  cerr << thr->get_fileline()
		       << "Warning: skipping out of range delete(" << idx
		       << ") on queue of size " << size << "." << ಸಾಲು_ಅಂತ್ಯ;
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  ಸರತಿ->erase(idx);
	    }
      }

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/* %delete/obj <label>
 *
 * This operator works by assigning a nil to the target object. This
 * causes any value that might be there to be garbage collected, thus
 * deleting the object.
 */
ತಾರ್ಕಿಕ ದ_DELETE_OBJ(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_send_object(ptr, vvp_object_t(), thr->wt_context);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/* %reverse/obj <label>
 * Reverse element order of the queue or dynamic array object at this net.
 */
ತಾರ್ಕಿಕ ದ_ಹಿಮ್ಮುಖ_OBJ(vthread_t /*thr*/, vvp_ಸಂಕೇತ_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      ಪ್ರತಿಜ್ಞಾ(obj);

      vvp_object_t stor = obj->get_object();
      ಆದರೆ (stor.test_nil()) ಹಿಂತಿರುಗಿ ನಿಜ;

	/* Match other queue ops: peek queues before vvp_darray so concrete
	   queue types dispatch reverse_elems() correctly. */
      ಆದರೆ (vvp_ಸರತಿ* ಸರತಿ = stor.peek<vvp_ಸರತಿ>()) {
	    ಸರತಿ->ಹಿಮ್ಮುಖ_elems();
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      ಪ್ರತಿಜ್ಞಾ(darray);
      darray->ಹಿಮ್ಮುಖ_elems();

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/* %reverse/prop/obj <property-index>
 * Reverse queue/darray property storage inside the class object on stack top.
 */
ತಾರ್ಕಿಕ ದ_ಹಿಮ್ಮುಖ_PROP_OBJ(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      size_t pid = cp->ಸಂಖ್ಯೆ;

      vvp_object_t& ತುದಿ = thr->peek_object();
      vvp_cobject*cobj = ತುದಿ.peek<vvp_cobject>();
      ಪ್ರತಿಜ್ಞಾ(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      ಆದರೆ (arr_obj.test_nil()) ಹಿಂತಿರುಗಿ ನಿಜ;

      ಆದರೆ (vvp_ಸರತಿ* ಸರತಿ = arr_obj.peek<vvp_ಸರತಿ>()) {
	    ಸರತಿ->ಹಿಮ್ಮುಖ_elems();
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      ಪ್ರತಿಜ್ಞಾ(darray);
      darray->ಹಿಮ್ಮುಖ_elems();

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/* %sort/obj — ascending sort */
ತಾರ್ಕಿಕ ದ_ವಿಂಗಡಿಸು_OBJ(vthread_t /*thr*/, vvp_ಸಂಕೇತ_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      ಪ್ರತಿಜ್ಞಾ(obj);

      vvp_object_t stor = obj->get_object();
      ಆದರೆ (stor.test_nil()) ಹಿಂತಿರುಗಿ ನಿಜ;

      ಆದರೆ (vvp_ಸರತಿ* ಸರತಿ = stor.peek<vvp_ಸರತಿ>()) {
	    ಸರತಿ->ವಿಂಗಡಿಸು_elems(ನಿಜ);
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      ಪ್ರತಿಜ್ಞಾ(darray);
      darray->ವಿಂಗಡಿಸು_elems(ನಿಜ);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/* %sort/prop/obj */
ತಾರ್ಕಿಕ ದ_ವಿಂಗಡಿಸು_PROP_OBJ(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      size_t pid = cp->ಸಂಖ್ಯೆ;

      vvp_object_t& ತುದಿ = thr->peek_object();
      vvp_cobject*cobj = ತುದಿ.peek<vvp_cobject>();
      ಪ್ರತಿಜ್ಞಾ(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      ಆದರೆ (arr_obj.test_nil()) ಹಿಂತಿರುಗಿ ನಿಜ;

      ಆದರೆ (vvp_ಸರತಿ* ಸರತಿ = arr_obj.peek<vvp_ಸರತಿ>()) {
	    ಸರತಿ->ವಿಂಗಡಿಸು_elems(ನಿಜ);
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      ಪ್ರತಿಜ್ಞಾ(darray);
      darray->ವಿಂಗಡಿಸು_elems(ನಿಜ);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/* %rsort/obj — descending sort */
ತಾರ್ಕಿಕ ದ_RSORT_OBJ(vthread_t /*thr*/, vvp_ಸಂಕೇತ_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      ಪ್ರತಿಜ್ಞಾ(obj);

      vvp_object_t stor = obj->get_object();
      ಆದರೆ (stor.test_nil()) ಹಿಂತಿರುಗಿ ನಿಜ;

      ಆದರೆ (vvp_ಸರತಿ* ಸರತಿ = stor.peek<vvp_ಸರತಿ>()) {
	    ಸರತಿ->ವಿಂಗಡಿಸು_elems(ಸುಳ್ಳು);
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      ಪ್ರತಿಜ್ಞಾ(darray);
      darray->ವಿಂಗಡಿಸು_elems(ಸುಳ್ಳು);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/* %rsort/prop/obj */
ತಾರ್ಕಿಕ ದ_RSORT_PROP_OBJ(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      size_t pid = cp->ಸಂಖ್ಯೆ;

      vvp_object_t& ತುದಿ = thr->peek_object();
      vvp_cobject*cobj = ತುದಿ.peek<vvp_cobject>();
      ಪ್ರತಿಜ್ಞಾ(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      ಆದರೆ (arr_obj.test_nil()) ಹಿಂತಿರುಗಿ ನಿಜ;

      ಆದರೆ (vvp_ಸರತಿ* ಸರತಿ = arr_obj.peek<vvp_ಸರತಿ>()) {
	    ಸರತಿ->ವಿಂಗಡಿಸು_elems(ಸುಳ್ಳು);
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      ಪ್ರತಿಜ್ಞಾ(darray);
      darray->ವಿಂಗಡಿಸು_elems(ಸುಳ್ಳು);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/* %shuffle/obj */
ತಾರ್ಕಿಕ ದ_SHUFFLE_OBJ(vthread_t /*thr*/, vvp_ಸಂಕೇತ_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*>(net->fun);
      ಪ್ರತಿಜ್ಞಾ(obj);

      vvp_object_t stor = obj->get_object();
      ಆದರೆ (stor.test_nil()) ಹಿಂತಿರುಗಿ ನಿಜ;

      ಆದರೆ (vvp_ಸರತಿ* ಸರತಿ = stor.peek<vvp_ಸರತಿ>()) {
	    ಸರತಿ->shuffle_elems();
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

      vvp_darray*darray = stor.peek<vvp_darray>();
      ಪ್ರತಿಜ್ಞಾ(darray);
      darray->shuffle_elems();

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/* %shuffle/prop/obj */
ತಾರ್ಕಿಕ ದ_SHUFFLE_PROP_OBJ(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      size_t pid = cp->ಸಂಖ್ಯೆ;

      vvp_object_t& ತುದಿ = thr->peek_object();
      vvp_cobject*cobj = ತುದಿ.peek<vvp_cobject>();
      ಪ್ರತಿಜ್ಞಾ(cobj);

      vvp_object_t arr_obj;
      cobj->get_object(pid, arr_obj, 0);
      ಆದರೆ (arr_obj.test_nil()) ಹಿಂತಿರುಗಿ ನಿಜ;

      ಆದರೆ (vvp_ಸರತಿ* ಸರತಿ = arr_obj.peek<vvp_ಸರತಿ>()) {
	    ಸರತಿ->shuffle_elems();
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

      vvp_darray*darray = arr_obj.peek<vvp_darray>();
      ಪ್ರತಿಜ್ಞಾ(darray);
      darray->shuffle_elems();

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/* %delete/tail <label>, idx
 *
 * Remove all elements after the one specified.
 */
ತಾರ್ಕಿಕ ದ_DELETE_ಬಾಲ(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      ಪ್ರತಿಜ್ಞಾ(obj);

      vvp_ಸರತಿ*ಸರತಿ = obj->get_object().peek<vvp_ಸರತಿ>();
      ಪ್ರತಿಜ್ಞಾ(ಸರತಿ);

      ಚಿಹ್ನೆರಹಿತ idx = thr->ಪದಗಳು[cp->ಬಿಟ್_idx[0]].w_int;
      ಸರತಿ->erase_ಬಾಲ(idx);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಸ್ಥಿರ ತಾರ್ಕಿಕ do_disable(vthread_t thr, vthread_t ಹೊಂದಿಕೆ)
{
      ತಾರ್ಕಿಕ flag = ಸುಳ್ಳು;

	/* Pull the target thread out of its scope if needed. */
      thr->ಪೋಷಕ_scope->ಎಳೆಗಳು.erase(thr);

	/* Turn the thread off by setting is program counter to
	   zero and setting an OFF bit. */
      thr->pc = codespace_null();
      thr->i_was_disabled = 1;
      thr->i_have_ended = 1;

	/* Turn off all the children of the thread. Simulate a %join
	   for as many times as needed to clear the results of all the
	   %forks that this thread has done. */
      ಆಗ (! thr->children.empty()) {

	    vthread_t tmp = *(thr->children.begin());
	    ಪ್ರತಿಜ್ಞಾ(tmp);
	    ಪ್ರತಿಜ್ಞಾ(tmp->ಪೋಷಕ == thr);
	    thr->i_am_joining = 0;
	    ಆದರೆ (do_disable(tmp, ಹೊಂದಿಕೆ))
		  flag = ನಿಜ;

	    vthread_reap(tmp);
      }

      vthread_t ಪೋಷಕ = thr->ಪೋಷಕ;
      ಆದರೆ (ಪೋಷಕ && ಪೋಷಕ->i_am_joining) {
	      // If a parent is waiting in a %join, wake it up. Note
	      // that it is possible to be waiting in a %join yet
	      // already scheduled if multiple child threads are
	      // ending. So check if the thread is already scheduled
	      // before scheduling it again.
	    ಪೋಷಕ->i_am_joining = 0;
	    ಆದರೆ (! ಪೋಷಕ->i_have_ended)
		  ವೇಳಾಪಟ್ಟಿ_vthread(ಪೋಷಕ, 0, ನಿಜ);

	    do_join(ಪೋಷಕ, thr);

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಪೋಷಕ) {
	      /* If the parent is yet to %join me, let its %join
		 do the reaping. */
	      //assert(tmp->is_scheduled == 0);

      } ಇಲ್ಲದಿದ್ದರೆ {
	      /* No parent at all. Goodbye. */
	    vthread_reap(thr);
      }

      ಹಿಂತಿರುಗಿ flag || (thr == ಹೊಂದಿಕೆ);
}

/*
 * Implement the %disable instruction by scanning the target scope for
 * all the target threads. Kill the target threads and wake up a
 * parent that is attempting a %join.
 */
ತಾರ್ಕಿಕ ದ_DISABLE(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      __vpiScope*scope = static_cast<__vpiScope*>(cp->handle);

      ತಾರ್ಕಿಕ disabled_myself_flag = ಸುಳ್ಳು;

      ಆಗ (! scope->ಎಳೆಗಳು.empty()) {
	    ಗಣ<vthread_t>::iterator cur = scope->ಎಳೆಗಳು.begin();

	    ಆದರೆ (do_disable(*cur, thr))
		  disabled_myself_flag = ನಿಜ;
      }

      ಹಿಂತಿರುಗಿ ! disabled_myself_flag;
}

/*
 * Similar to `of_DISABLE`. But will only disable a single thread of the
 * specified scope. The disabled thread will be the thread closest to the
 * current thread in thread hierarchy. This can either be the current thread,
 * either the thread itself or one of its parents.
 * This is used for SystemVerilog flow control instructions like `return`,
 * `continue` and `break`.
 */

ತಾರ್ಕಿಕ ದ_DISABLE_FLOW(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಸ್ಥಿರಾಂಕ __vpiScope*scope = static_cast<__vpiScope*>(cp->handle);
      vthread_t cur = thr;

      ಆಗ (cur && cur->ಪೋಷಕ_scope != scope)
	    cur = cur->ಪೋಷಕ;

      ಪ್ರತಿಜ್ಞಾ(cur);
      ಆದರೆ (cur)
	    ಹಿಂತಿರುಗಿ !do_disable(cur, thr);

      ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

/*
 * Implement the %disable/fork (SystemVerilog) instruction by disabling
 * all the detached children of the given thread.
 */
ತಾರ್ಕಿಕ ದ_DISABLE_FORK(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
	/* If a %disable/fork is being executed then the parent thread
	 * cannot be waiting in a join. */
      ಪ್ರತಿಜ್ಞಾ(! thr->i_am_joining);

	/* There should be no active children to disable. */
      ಪ್ರತಿಜ್ಞಾ(thr->children.empty());

	/* Disable any detached children. */
      ಆಗ (! thr->detached_children.empty()) {
	    vthread_t ಮಗು = *(thr->detached_children.begin());
	    ಪ್ರತಿಜ್ಞಾ(ಮಗು);
	    ಪ್ರತಿಜ್ಞಾ(ಮಗು->ಪೋಷಕ == thr);
	      /* Disabling the children can never match the parent thread. */
	    ತಾರ್ಕಿಕ res = do_disable(ಮಗು, thr);
	    ಪ್ರತಿಜ್ಞಾ(! res);
	    vthread_reap(ಮಗು);
      }

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * This function divides a 2-word number {high, a} by a 1-word
 * number. Assume that high < b.
 */
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ divide2words(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ a, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ b,
				  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮೇಲಿನ)
{
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಫಲಿತಾಂಶ = 0;
      ಆಗ (ಮೇಲಿನ > 0) {
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tmp_ಫಲಿತಾಂಶ = ULONG_MAX / b;
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ remain = ULONG_MAX % b;

	    remain += 1;
	    ಆದರೆ (remain >= b) {
		  remain -= b;
		  tmp_ಫಲಿತಾಂಶ += 1;
	    }

	      // Now 0x1_0...0 = b*tmp_result + remain
	      // high*0x1_0...0 = high*(b*tmp_result + remain)
	      // high*0x1_0...0 = high*b*tmp_result + high*remain

	      // We know that high*0x1_0...0 >= high*b*tmp_result, and
	      // we know that high*0x1_0...0 > high*remain. Use
	      // high*remain as the remainder for another iteration,
	      // and add tmp_result*high into the current estimate of
	      // the result.
	    ಫಲಿತಾಂಶ += tmp_ಫಲಿತಾಂಶ * ಮೇಲಿನ;

	      // The new iteration starts with high*remain + a.
	    remain = multiply_with_ವಾಹಕ(ಮೇಲಿನ, remain, ಮೇಲಿನ);
	    a += remain;
            ಆದರೆ(a < remain)
              ಮೇಲಿನ += 1;

	      // Now result*b + {high,a} == the input {high,a}. It is
	      // possible that the new high >= 1. If so, it will
	      // certainly be less than high from the previous
	      // iteration. Do another iteration and it will shrink,
	      // eventually to 0.
      }

	// high is now 0, so a is the remaining remainder, so we can
	// finish off the integer divide with a simple a/b.

      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ + a/b;
}

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ* divide_ಬಿಟ್ಟುಗಳು(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ*ap, ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ*bp, ಚಿಹ್ನೆರಹಿತ wid)
{
	// Do all our work a cpu-word at a time. The "words" variable
	// is the number of words of the wid.
      ಚಿಹ್ನೆರಹಿತ ಪದಗಳು = (wid+CPU_ಶಬ್ದ_ಬಿಟ್ಟುಗಳು-1) / CPU_ಶಬ್ದ_ಬಿಟ್ಟುಗಳು;

      ಚಿಹ್ನೆರಹಿತ btop = ಪದಗಳು-1;
      ಆಗ (btop > 0 && bp[btop] == 0)
	    btop -= 1;

	// Detect divide by 0, and exit.
      ಆದರೆ (btop==0 && bp[0]==0)
	    ಹಿಂತಿರುಗಿ 0;

	// The result array will eventually accumulate the result. The
	// diff array is a difference that we use in the intermediate.
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ*diff  = ಹೊಸ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ[ಪದಗಳು];
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ*ಫಲಿತಾಂಶ= ಹೊಸ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ[ಪದಗಳು];
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < ಪದಗಳು ; idx += 1)
	    ಫಲಿತಾಂಶ[idx] = 0;

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ cur = ಪದಗಳು-btop ; cur > 0 ; cur -= 1) {
	    ಚಿಹ್ನೆರಹಿತ cur_ptr = cur-1;
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ cur_res;
	    ಆದರೆ (ap[cur_ptr+btop] >= bp[btop]) {
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮೇಲಿನ = 0;
		  ಆದರೆ (cur_ptr+btop+1 < ಪದಗಳು)
			ಮೇಲಿನ = ap[cur_ptr+btop+1];
		  cur_res = divide2words(ap[cur_ptr+btop], bp[btop], ಮೇಲಿನ);

	    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cur_ptr+btop+1 >= ಪದಗಳು) {
		  ಮುಂದುವರಿ;

	    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ap[cur_ptr+btop+1] == 0) {
		  ಮುಂದುವರಿ;

	    } ಇಲ್ಲದಿದ್ದರೆ {
		  cur_res = divide2words(ap[cur_ptr+btop], bp[btop],
					 ap[cur_ptr+btop+1]);
	    }

	      // cur_res is a guesstimate of the result this far. It
	      // may be 1 too big. (But it will also be >0) Try it,
	      // and if the difference comes out negative, then adjust.

	      // diff = (bp * cur_res)  << cur_ptr;
	    multiply_array_imm(diff+cur_ptr, bp, ಪದಗಳು-cur_ptr, cur_res);
	      // ap -= diff
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ವಾಹಕ = 1;
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = cur_ptr ; idx < ಪದಗಳು ; idx += 1)
		  ap[idx] = ಕೂಡಿಸು_with_ವಾಹಕ(ap[idx], ~diff[idx], ವಾಹಕ);

	      // ap has the diff subtracted out of it. If cur_res was
	      // too large, then ap will turn negative. (We easily
	      // tell that ap turned negative by looking at
	      // carry&1. If it is 0, then it is *negative*.) In that
	      // case, we know that cur_res was too large by 1. Correct by
	      // adding 1b back in and reducing cur_res.
	    ಆದರೆ ((ವಾಹಕ&1) == 0) {
		    // Keep adding b back in until the remainder
		    // becomes positive again.
		  ಮಾಡು {
			cur_res -= 1;
			ವಾಹಕ = 0;
			ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = cur_ptr ; idx < ಪದಗಳು ; idx += 1)
			      ap[idx] = ಕೂಡಿಸು_with_ವಾಹಕ(ap[idx], bp[idx-cur_ptr], ವಾಹಕ);
		  } ಆಗ (ವಾಹಕ == 0);
	    }

	    ಫಲಿತಾಂಶ[cur_ptr] = cur_res;
      }

	// Now ap contains the remainder and result contains the
	// desired result. We should find that:
	//  input-a = bp * result + ap;

      ಅಳಿಸು[]diff;
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * %div
 */
ತಾರ್ಕಿಕ ದ_DIV(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      vvp_vector4_t valb = thr->ತೆಗೆ_vec4();
      vvp_vector4_t vala = thr->ತೆಗೆ_vec4();

      ಪ್ರತಿಜ್ಞಾ(vala.size()== valb.size());
      ಚಿಹ್ನೆರಹಿತ wid = vala.size();

      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ*ap = vala.subarray(0, wid);
      ಆದರೆ (ap == 0) {
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->ತಳ್ಳು_vec4(tmp);
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ*bp = valb.subarray(0, wid);
      ಆದರೆ (bp == 0) {
	    ಅಳಿಸು[]ap;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->ತಳ್ಳು_vec4(tmp);
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

	// If the value fits in a single CPU word, then do it the easy way.
      ಆದರೆ (wid <= CPU_ಶಬ್ದ_ಬಿಟ್ಟುಗಳು) {
	    ಆದರೆ (bp[0] == 0) {
		  vvp_vector4_t tmp(wid, BIT4_X);
		  thr->ತಳ್ಳು_vec4(tmp);
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  ap[0] /= bp[0];
		  vala.setarray(0, wid, ap);
		  thr->ತಳ್ಳು_vec4(vala);
	    }
	    ಅಳಿಸು[]ap;
	    ಅಳಿಸು[]bp;
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ*ಫಲಿತಾಂಶ = divide_ಬಿಟ್ಟುಗಳು(ap, bp, wid);
      ಆದರೆ (ಫಲಿತಾಂಶ == 0) {
	    ಅಳಿಸು[]ap;
	    ಅಳಿಸು[]bp;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    thr->ತಳ್ಳು_vec4(tmp);
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

	// Now ap contains the remainder and result contains the
	// desired result. We should find that:
	//  input-a = bp * result + ap;

      vala.setarray(0, wid, ಫಲಿತಾಂಶ);
      thr->ತಳ್ಳು_vec4(vala);
      ಅಳಿಸು[]ap;
      ಅಳಿಸು[]bp;
      ಅಳಿಸು[]ಫಲಿತಾಂಶ;

      ಹಿಂತಿರುಗಿ ನಿಜ;
}


ಸ್ಥಿರ ಶೂನ್ಯ negate_ಪದಗಳು(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ*val, ಚಿಹ್ನೆರಹಿತ ಪದಗಳು)
{
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ವಾಹಕ = 1;
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < ಪದಗಳು ; idx += 1)
	    val[idx] = ಕೂಡಿಸು_with_ವಾಹಕ(0, ~val[idx], ವಾಹಕ);
}

/*
 * %div/s
 */
ತಾರ್ಕಿಕ ದ_DIV_S(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      vvp_vector4_t valb = thr->ತೆಗೆ_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      ಪ್ರತಿಜ್ಞಾ(vala.size()== valb.size());
      ಚಿಹ್ನೆರಹಿತ wid = vala.size();
      ಚಿಹ್ನೆರಹಿತ ಪದಗಳು = (wid + CPU_ಶಬ್ದ_ಬಿಟ್ಟುಗಳು - 1) / CPU_ಶಬ್ದ_ಬಿಟ್ಟುಗಳು;

	// Get the values, left in right, in binary form. If there is
	// a problem with either (caused by an X or Z bit) then we
	// know right away that the entire result is X.
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ*ap = vala.subarray(0, wid);
      ಆದರೆ (ap == 0) {
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ*bp = valb.subarray(0, wid);
      ಆದರೆ (bp == 0) {
	    ಅಳಿಸು[]ap;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

	// Sign extend the bits in the array to fill out the array.
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sign_ಮುಖವಾಡ = 0;
      ಆದರೆ (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sign_ಬಿಟ್ಟುಗಳು = (ಪದಗಳು*CPU_ಶಬ್ದ_ಬಿಟ್ಟುಗಳು) - wid) {
	    sign_ಮುಖವಾಡ = -1UL << (CPU_ಶಬ್ದ_ಬಿಟ್ಟುಗಳು-sign_ಬಿಟ್ಟುಗಳು);
	    ಆದರೆ (ap[ಪದಗಳು-1] & (sign_ಮುಖವಾಡ>>1))
		  ap[ಪದಗಳು-1] |= sign_ಮುಖವಾಡ;
	    ಆದರೆ (bp[ಪದಗಳು-1] & (sign_ಮುಖವಾಡ>>1))
		  bp[ಪದಗಳು-1] |= sign_ಮುಖವಾಡ;
      }

	// If the value fits in a single word, then use the native divide.
      ಆದರೆ (wid <= CPU_ಶಬ್ದ_ಬಿಟ್ಟುಗಳು) {
	    ಆದರೆ (bp[0] == 0) {
		  vvp_vector4_t tmp(wid, BIT4_X);
		  vala = tmp;
	    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (((ದೀರ್ಘ)ap[0] == LONG_MIN) && ((ದೀರ್ಘ)bp[0] == -1)) {
		  vvp_vector4_t tmp(wid, BIT4_0);
		  tmp.ಗಣ_ಬಿಟ್(wid-1, BIT4_1);
		  vala = tmp;
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  ದೀರ್ಘ tmpa = (ದೀರ್ಘ) ap[0];
		  ದೀರ್ಘ tmpb = (ದೀರ್ಘ) bp[0];
		  ದೀರ್ಘ res = tmpa / tmpb;
		  ap[0] = ((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)res) & ~sign_ಮುಖವಾಡ;
		  vala.setarray(0, wid, ap);
	    }
	    ಅಳಿಸು[]ap;
	    ಅಳಿಸು[]bp;
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

	// We need to the actual division to positive integers. Make
	// them positive here, and remember the negations.
      ತಾರ್ಕಿಕ negate_flag = ಸುಳ್ಳು;
      ಆದರೆ ( ((ದೀರ್ಘ) ap[ಪದಗಳು-1]) < 0 ) {
	    negate_flag = ನಿಜ;
	    negate_ಪದಗಳು(ap, ಪದಗಳು);
      }
      ಆದರೆ ( ((ದೀರ್ಘ) bp[ಪದಗಳು-1]) < 0 ) {
	    negate_flag ^= ನಿಜ;
	    negate_ಪದಗಳು(bp, ಪದಗಳು);
      }

      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ*ಫಲಿತಾಂಶ = divide_ಬಿಟ್ಟುಗಳು(ap, bp, wid);
      ಆದರೆ (ಫಲಿತಾಂಶ == 0) {
	    ಅಳಿಸು[]ap;
	    ಅಳಿಸು[]bp;
	    vvp_vector4_t tmp(wid, BIT4_X);
	    vala = tmp;
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

      ಆದರೆ (negate_flag) {
	    negate_ಪದಗಳು(ಫಲಿತಾಂಶ, ಪದಗಳು);
      }

      ಫಲಿತಾಂಶ[ಪದಗಳು-1] &= ~sign_ಮುಖವಾಡ;

      vala.setarray(0, wid, ಫಲಿತಾಂಶ);
      ಅಳಿಸು[]ap;
      ಅಳಿಸು[]bp;
      ಅಳಿಸು[]ಫಲಿತಾಂಶ;
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_DIV_WR(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      ದ್ವಿಗುಣ r = thr->ತೆಗೆ_real();
      ದ್ವಿಗುಣ l = thr->ತೆಗೆ_real();
      thr->ತಳ್ಳು_real(l / r);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %dup/obj
 * %dup/real
 * %dup/vec4
 *
 * Push a duplicate of the object on the appropriate stack.
 */
ತಾರ್ಕಿಕ ದ_DUP_OBJ(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      vvp_object_t src = thr->peek_object();

        // If it is null push a new null object
      ಆದರೆ (src.test_nil())
	    thr->ತಳ್ಳು_object(vvp_object_t());
      ಇಲ್ಲದಿದ್ದರೆ
	    thr->ತಳ್ಳು_object(src.duplicate());

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_DUP_REAL(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      thr->ತಳ್ಳು_real(thr->peek_real(0));
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_DUP_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      thr->ತಳ್ಳು_vec4(thr->peek_vec4(0));
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * This terminates the current thread. If there is a parent who is
 * waiting for me to die, then I schedule it. At any rate, I mark
 * myself as a zombie by setting my pc to 0.
 */
ತಾರ್ಕಿಕ ದ_ಕೊನೆ(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      ಪ್ರತಿಜ್ಞಾ(! thr->ಕಾಯುವಿಕೆ_for_event);
      thr->i_have_ended = 1;
      thr->pc = codespace_null();

	/* Fully detach any detached children. */
      ಆಗ (! thr->detached_children.empty()) {
	    vthread_t ಮಗು = *(thr->detached_children.begin());
	    ಪ್ರತಿಜ್ಞಾ(ಮಗು);
	    ಪ್ರತಿಜ್ಞಾ(ಮಗು->ಪೋಷಕ == thr);
	    ಪ್ರತಿಜ್ಞಾ(ಮಗು->i_am_detached);
	    ಮಗು->ಪೋಷಕ = 0;
	    ಮಗು->i_am_detached = 0;
	    thr->detached_children.erase(thr->detached_children.begin());
      }

	/* It is an error to still have active children running at this
	 * point in time. They should have all been detached or joined. */
      ಪ್ರತಿಜ್ಞಾ(thr->children.empty());

	/* If I have a parent who is waiting for me, then mark that I
	   have ended, and schedule that parent. Also, finish the
	   %join for the parent. */
      ಆದರೆ (!thr->i_am_detached && thr->ಪೋಷಕ && thr->ಪೋಷಕ->i_am_joining) {
	    vthread_t tmp = thr->ಪೋಷಕ;
	    ಪ್ರತಿಜ್ಞಾ(! thr->i_am_detached);

	    tmp->i_am_joining = 0;
	    ವೇಳಾಪಟ್ಟಿ_vthread(tmp, 0, ನಿಜ);
	    do_join(tmp, thr);
	    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
      }

	/* If this thread is not fully detached then remove it from the
	 * parents detached_children set and reap it. */
      ಆದರೆ (thr->i_am_detached) {
	    vthread_t tmp = thr->ಪೋಷಕ;
	    ಪ್ರತಿಜ್ಞಾ(tmp);
	    size_t res = tmp->detached_children.erase(thr);
	    ಪ್ರತಿಜ್ಞಾ(res == 1);
	      /* If the parent is waiting for the detached children to
	       * finish then the last detached child needs to tell the
	       * parent to wake up when it is finished. */
	    ಆದರೆ (tmp->i_am_ಕಾಯುವಿಕೆ && tmp->detached_children.empty()) {
		  tmp->i_am_ಕಾಯುವಿಕೆ = 0;
		  ವೇಳಾಪಟ್ಟಿ_vthread(tmp, 0, ನಿಜ);
	    }
	      /* Fully detach this thread so it will be reaped below. */
	    thr->i_am_detached = 0;
	    thr->ಪೋಷಕ = 0;
      }

	/* If I have no parent, then no one can %join me and there is
	 * no reason to stick around. This can happen, for example if
	 * I am an ``initial'' thread. */
      ಆದರೆ (thr->ಪೋಷಕ == 0) {
	    vthread_reap(thr);
	    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
      }

	/* If I make it this far, then I have a parent who may wish
	   to %join me. Remain a zombie so that it can. */

      ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

/*
 * %event <var-label>
 */
ತಾರ್ಕಿಕ ದ_EVENT(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_vector4_t tmp (1, BIT4_X);
      vvp_send_vec4(ptr, tmp, thr->wt_context);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %event/nb <var-label>, <delay>
 */
ತಾರ್ಕಿಕ ದ_EVENT_NB(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_time64_t delay;

      delay = thr->ಪದಗಳು[cp->ಬಿಟ್_idx[0]].w_uint;
      ವೇಳಾಪಟ್ಟಿ_propagate_event(cp->net, delay);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_EVCTL(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಪ್ರತಿಜ್ಞಾ(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      thr->ecount = thr->ಪದಗಳು[cp->ಬಿಟ್_idx[0]].w_uint;
      ಹಿಂತಿರುಗಿ ನಿಜ;
}
ತಾರ್ಕಿಕ ದ_EVCTLC(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      thr->event = 0;
      thr->ecount = 0;
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_EVCTLI(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಪ್ರತಿಜ್ಞಾ(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      thr->ecount = cp->ಬಿಟ್_idx[0];
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_EVCTLS(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಪ್ರತಿಜ್ಞಾ(thr->event == 0 && thr->ecount == 0);
      thr->event = cp->net;
      int64_t val = thr->ಪದಗಳು[cp->ಬಿಟ್_idx[0]].w_int;
      ಆದರೆ (val < 0) val = 0;
      thr->ecount = val;
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_FLAG_GET_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಪೂರ್ಣಾಂಕ flag = cp->ಸಂಖ್ಯೆ;
      ಪ್ರತಿಜ್ಞಾ(flag < vthread_s::FLAGS_ಎಣಿಕೆ);

      vvp_vector4_t val (1, thr->flags[flag]);
      thr->ತಳ್ಳು_vec4(val);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %flag_inv <flag1>
 */
ತಾರ್ಕಿಕ ದ_FLAG_INV(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಪೂರ್ಣಾಂಕ flag1 = cp->ಬಿಟ್_idx[0];

      thr->flags[flag1] = ~ thr->flags[flag1];
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %flag_mov <flag1>, <flag2>
 */
ತಾರ್ಕಿಕ ದ_FLAG_MOV(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಪೂರ್ಣಾಂಕ flag1 = cp->ಬಿಟ್_idx[0];
      ಪೂರ್ಣಾಂಕ flag2 = cp->ಬಿಟ್_idx[1];

      thr->flags[flag1] = thr->flags[flag2];
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %flag_or <flag1>, <flag2>
 */
ತಾರ್ಕಿಕ ದ_FLAG_OR(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಪೂರ್ಣಾಂಕ flag1 = cp->ಬಿಟ್_idx[0];
      ಪೂರ್ಣಾಂಕ flag2 = cp->ಬಿಟ್_idx[1];

      thr->flags[flag1] = thr->flags[flag1] | thr->flags[flag2];
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_FLAG_ಗಣ_IMM(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಪೂರ್ಣಾಂಕ flag = cp->ಸಂಖ್ಯೆ;
      ಪೂರ್ಣಾಂಕ vali = cp->ಬಿಟ್_idx[0];

      ಪ್ರತಿಜ್ಞಾ(flag < vthread_s::FLAGS_ಎಣಿಕೆ);
      ಪ್ರತಿಜ್ಞಾ(vali >= 0 && vali < 4);

      ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ vvp_bit4_t map_ಬಿಟ್[4] = {BIT4_0, BIT4_1, BIT4_Z, BIT4_X};
      thr->flags[flag] = map_ಬಿಟ್[vali];
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_FLAG_ಗಣ_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಪೂರ್ಣಾಂಕ flag = cp->ಸಂಖ್ಯೆ;
      ಪ್ರತಿಜ್ಞಾ(flag < vthread_s::FLAGS_ಎಣಿಕೆ);

      ಸ್ಥಿರಾಂಕ vvp_vector4_t&val = thr->peek_vec4();
      thr->flags[flag] = val.value(0);
      thr->ತೆಗೆ_vec4(1);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * the %force/link instruction connects a source node to a
 * destination node. The destination node must be a signal, as it is
 * marked with the source of the force so that it may later be
 * unlinked without specifically knowing the source that this
 * instruction used.
 */
ತಾರ್ಕಿಕ ದ_FORCE_LINK(vthread_t, vvp_ಸಂಕೇತ_t cp)
{
      vvp_net_t*dst = cp->net;
      vvp_net_t*src = cp->net2;

      ಪ್ರತಿಜ್ಞಾ(dst->fil);
      dst->fil->force_link(dst, src);

      ಹಿಂತಿರುಗಿ ನಿಜ;
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
ತಾರ್ಕಿಕ ದ_FORCE_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_net_t*net = cp->net;

      vvp_vector4_t value = thr->ತೆಗೆ_vec4();

	/* Send the force value to the filter on the node. */

      ಪ್ರತಿಜ್ಞಾ(net->fil);
      ಆದರೆ (value.size() != net->fil->filter_ಗಾತ್ರ())
	    value = coerce_ಗಮ್ಯ_ಅಗಲ(value, net->fil->filter_ಗಾತ್ರ());

      net->force_vec4(value, vvp_vector2_t(vvp_vector2_t::FILL1, net->fil->filter_ಗಾತ್ರ()));

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %force/vec4/off <net>, <off>
 */
ತಾರ್ಕಿಕ ದ_FORCE_VEC4_OFF(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_net_t*net = cp->net;
      ಚಿಹ್ನೆರಹಿತ ತಳ_idx = cp->ಬಿಟ್_idx[0];
      ದೀರ್ಘ ತಳ = thr->ಪದಗಳು[ತಳ_idx].w_int;
      vvp_vector4_t value = thr->ತೆಗೆ_vec4();
      ಚಿಹ್ನೆರಹಿತ wid = value.size();

      ಪ್ರತಿಜ್ಞಾ(net->fil);

      ಆದರೆ (thr->flags[4] == BIT4_1)
	    ಹಿಂತಿರುಗಿ ನಿಜ;

	// This is the width of the target vector.
      ಚಿಹ್ನೆರಹಿತ use_ಗಾತ್ರ = net->fil->filter_ಗಾತ್ರ();

      ಆದರೆ (ತಳ >= (ದೀರ್ಘ)use_ಗಾತ್ರ)
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      ಆದರೆ (ತಳ < -(ದೀರ್ಘ)use_ಗಾತ್ರ)
	    ಹಿಂತಿರುಗಿ ನಿಜ;

      ಆದರೆ ((ತಳ + wid) > use_ಗಾತ್ರ)
	    wid = use_ಗಾತ್ರ - ತಳ;

	// Make a mask of which bits are to be forced, 0 for unforced
	// bits and 1 for forced bits.
      vvp_vector2_t ಮುಖವಾಡ (vvp_vector2_t::FILL0, use_ಗಾತ್ರ);
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < wid ; idx += 1)
	    ಮುಖವಾಡ.ಗಣ_ಬಿಟ್(ತಳ+idx, 1);

      vvp_vector4_t tmp (use_ಗಾತ್ರ, BIT4_Z);

	// vvp_net_t::force_vec4 propagates all the bits of the
	// forced vector value, regardless of the mask. This
	// ensures the unforced bits retain their current value.
      vvp_signal_ಮೌಲ್ಯ*sig = dynamic_cast<vvp_signal_ಮೌಲ್ಯ*>(net->fil);
      ಪ್ರತಿಜ್ಞಾ(sig);
      sig->vec4_ಮೌಲ್ಯ(tmp);

      tmp.ಗಣ_vec(ತಳ, value);

      net->force_vec4(tmp, ಮುಖವಾಡ);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %force/vec4/off/d <net>, <off>, <del>
 */
ತಾರ್ಕಿಕ ದ_FORCE_VEC4_OFF_D(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_net_t*net = cp->net;

      ಚಿಹ್ನೆರಹಿತ ತಳ_idx = cp->ಬಿಟ್_idx[0];
      ದೀರ್ಘ ತಳ = thr->ಪದಗಳು[ತಳ_idx].w_int;

      ಚಿಹ್ನೆರಹಿತ delay_idx = cp->ಬಿಟ್_idx[1];
      vvp_time64_t delay = thr->ಪದಗಳು[delay_idx].w_uint;

      vvp_vector4_t value = thr->ತೆಗೆ_vec4();

      ಪ್ರತಿಜ್ಞಾ(net->fil);

      ಆದರೆ (thr->flags[4] == BIT4_1)
	    ಹಿಂತಿರುಗಿ ನಿಜ;

	// This is the width of the target vector.
      ಚಿಹ್ನೆರಹಿತ use_ಗಾತ್ರ = net->fil->filter_ಗಾತ್ರ();

      ಆದರೆ (ತಳ >= (ದೀರ್ಘ)use_ಗಾತ್ರ)
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      ಆದರೆ (ತಳ < -(ದೀರ್ಘ)use_ಗಾತ್ರ)
	    ಹಿಂತಿರುಗಿ ನಿಜ;

      ವೇಳಾಪಟ್ಟಿ_force_ಸದಿಶ(net, ತಳ, use_ಗಾತ್ರ, value, delay);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_FORCE_WR(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_net_t*net  = cp->net;
      ದ್ವಿಗುಣ value = thr->ತೆಗೆ_real();

      net->force_real(value, vvp_vector2_t(vvp_vector2_t::FILL1, 1));

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * The %fork instruction causes a new child to be created and pushed
 * in front of any existing child. This causes the new child to be
 * added to the list of children, and for me to be the parent of the
 * new child.
 */
ತಾರ್ಕಿಕ ದ_FORK(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vthread_t ಮಗು = vthread_new(cp->cptr2, cp->scope);

      ಆದರೆ (cp->scope->is_automatic()) {
              /* The context allocated for this child is the top entry
                 on the write context stack. */
            ಮಗು->wt_context = thr->wt_context;
            ಮಗು->rd_context = thr->wt_context;
      }

      ಮಗು->ಪೋಷಕ = thr;
      thr->children.insert(ಮಗು);

      ಆದರೆ (thr->i_am_ಒಳ_function) {
	    ಮಗು->is_scheduled = 1;
	    ಮಗು->i_am_ಒಳ_function = 1;
	    vthread_ಸತತ(ಮಗು);
	    running_thread = thr;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ವೇಳಾಪಟ್ಟಿ_vthread(ಮಗು, 0, ನಿಜ);
      }
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_FREE(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
        /* Pop the child context from the read context stack. */
      vvp_context_t ಮಗು_context = thr->rd_context;
      thr->rd_context = vvp_get_stacked_context(ಮಗು_context);

        /* Free the context. */
      vthread_free_context(ಮಗು_context, cp->scope);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %inv
 *
 * Logically, this pops a value, inverts is (Verilog style, with Z and
 * X converted to X) and pushes the result. We can more efficiently
 * just to the invert in place.
 */
ತಾರ್ಕಿಕ ದ_INV(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val.invert();
      ಹಿಂತಿರುಗಿ ನಿಜ;
}


/*
 * Index registers, arithmetic.
 */

ಸ್ಥಿರ ಸಾಲೊಳಗೆ int64_t get_as_64_ಬಿಟ್(uint32_t ಕೆಳಗಿನ_32, uint32_t ಮೇಲಿನ_32)
{
      int64_t ಕೆಳಗಿನ = ಕೆಳಗಿನ_32;
      int64_t res = ಮೇಲಿನ_32;

      res <<= 32;
      res |= ಕೆಳಗಿನ;
      ಹಿಂತಿರುಗಿ res;
}

ತಾರ್ಕಿಕ ದ_IX_ಕೂಡಿಸು(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      thr->ಪದಗಳು[cp->ಸಂಖ್ಯೆ].w_int += get_as_64_ಬಿಟ್(cp->ಬಿಟ್_idx[0],
                                                    cp->ಬಿಟ್_idx[1]);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_IX_SUB(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      thr->ಪದಗಳು[cp->ಸಂಖ್ಯೆ].w_int -= get_as_64_ಬಿಟ್(cp->ಬಿಟ್_idx[0],
                                                    cp->ಬಿಟ್_idx[1]);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_IX_MUL(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      thr->ಪದಗಳು[cp->ಸಂಖ್ಯೆ].w_int *= get_as_64_ಬಿಟ್(cp->ಬಿಟ್_idx[0],
                                                    cp->ಬಿಟ್_idx[1]);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_IX_ತುಂಬು(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      thr->ಪದಗಳು[cp->ಸಂಖ್ಯೆ].w_int = get_as_64_ಬಿಟ್(cp->ಬಿಟ್_idx[0],
                                                   cp->ಬಿಟ್_idx[1]);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_IX_MOV(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      thr->ಪದಗಳು[cp->ಬಿಟ್_idx[0]].w_int = thr->ಪದಗಳು[cp->ಬಿಟ್_idx[1]].w_int;
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_IX_GETV(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ ಸೂಚ್ಯಂಕ = cp->ಬಿಟ್_idx[0];
      vvp_net_t*net = cp->net;

      vvp_signal_ಮೌಲ್ಯ*sig = dynamic_cast<vvp_signal_ಮೌಲ್ಯ*>(net->fil);
      ಆದರೆ (sig == 0) {
	    ಪ್ರತಿಜ್ಞಾ(net->fil);
	    cerr << thr->get_fileline()
	         << "%%ix/getv error: Net arg not a vector signal? "
		 << typeid(*net->fil).name() << ಸಾಲು_ಅಂತ್ಯ;
      }
      ಪ್ರತಿಜ್ಞಾ(sig);

      vvp_vector4_t vec;
      sig->vec4_ಮೌಲ್ಯ(vec);
      ತಾರ್ಕಿಕ ಅತಿಪ್ರವಾಹ_flag;
      uint64_t val = 0;
      ತಾರ್ಕಿಕ ತಿಳಿದ_flag = vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(vec, ಅತಿಪ್ರವಾಹ_flag, val);

      ಆದರೆ (ತಿಳಿದ_flag)
	    thr->ಪದಗಳು[ಸೂಚ್ಯಂಕ].w_uint = val;
      ಇಲ್ಲದಿದ್ದರೆ
	    thr->ಪದಗಳು[ಸೂಚ್ಯಂಕ].w_uint = 0;

	/* Set bit 4 as a flag if the input is unknown. */
      thr->flags[4] = ತಿಳಿದ_flag ? (ಅತಿಪ್ರವಾಹ_flag ? BIT4_X : BIT4_0) : BIT4_1;

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_IX_GETV_S(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ ಸೂಚ್ಯಂಕ = cp->ಬಿಟ್_idx[0];
      vvp_net_t*net = cp->net;

      vvp_signal_ಮೌಲ್ಯ*sig = dynamic_cast<vvp_signal_ಮೌಲ್ಯ*>(net->fil);
      ಆದರೆ (sig == 0) {
	    ಪ್ರತಿಜ್ಞಾ(net->fil);
	    cerr << thr->get_fileline()
	         << "%%ix/getv/s error: Net arg not a vector signal? "
		 << "fun=" << typeid(*net->fil).name()
		 << ", fil=" << (net->fil? typeid(*net->fil).name() : "<>")
		 << ಸಾಲು_ಅಂತ್ಯ;
      }
      ಪ್ರತಿಜ್ಞಾ(sig);

      vvp_vector4_t vec;
      sig->vec4_ಮೌಲ್ಯ(vec);
      int64_t val;
      ತಾರ್ಕಿಕ ತಿಳಿದ_flag = vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(vec, val, ನಿಜ, ನಿಜ);

      ಆದರೆ (ತಿಳಿದ_flag)
	    thr->ಪದಗಳು[ಸೂಚ್ಯಂಕ].w_int = val;
      ಇಲ್ಲದಿದ್ದರೆ
	    thr->ಪದಗಳು[ಸೂಚ್ಯಂಕ].w_int = 0;

	/* Set bit 4 as a flag if the input is unknown. */
      thr->flags[4] = ತಿಳಿದ_flag? BIT4_0 : BIT4_1;

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಸ್ಥಿರ uint64_t vec4_ಗಮ್ಯ_ಸೂಚ್ಯಂಕ(vthread_t thr, ತಾರ್ಕಿಕ signed_flag)
{
	// Get all the information we need about the vec4 vector, then
	// pop it away. We only need the bool bits and the length.
      ಸ್ಥಿರಾಂಕ vvp_vector4_t&val = thr->peek_vec4();
      ಚಿಹ್ನೆರಹಿತ val_ಗಾತ್ರ = val.size();
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ*ಬಿಟ್ಟುಗಳು = val.subarray(0, val_ಗಾತ್ರ, ಸುಳ್ಳು);
      thr->ತೆಗೆ_vec4(1);

	// If there are X/Z bits, then the subarray will give us a nil
	// pointer. Set a flag to indicate the error, and give up.
      ಆದರೆ (ಬಿಟ್ಟುಗಳು == 0) {
	    thr->flags[4] = BIT4_1;
	    ಹಿಂತಿರುಗಿ 0;
      }

      uint64_t v = 0;
      thr->flags[4] = BIT4_0;

      ಪ್ರತಿಜ್ಞಾ(ಗಾತ್ರ(ಬಿಟ್ಟುಗಳು[0]) <= ಗಾತ್ರ(v));

      v = 0;
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < val_ಗಾತ್ರ ; idx += 8*ಗಾತ್ರ(ಬಿಟ್ಟುಗಳು[0])) {
	    uint64_t tmp = ಬಿಟ್ಟುಗಳು[idx/8/ಗಾತ್ರ(ಬಿಟ್ಟುಗಳು[0])];
	    ಆದರೆ (idx < 8*ಗಾತ್ರ(v)) {
		  v |= tmp << idx;
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  ತಾರ್ಕಿಕ ಅತಿಪ್ರವಾಹ = signed_flag && (v >> 63) ? ~tmp != 0 : tmp != 0;
		  ಆದರೆ (ಅತಿಪ್ರವಾಹ) {
			thr->flags[4] = BIT4_X;
			ನಿಲ್ಲಿಸು;
		  }
	    }
      }

	// Set the high bits that are not necessarily filled in by the
	// subarray function.
      ಆದರೆ (val_ಗಾತ್ರ < 8*ಗಾತ್ರ(v)) {
	    ಆದರೆ (signed_flag && (v & (static_cast<uint64_t>(1)<<(val_ಗಾತ್ರ-1)))) {
		    // Propagate the sign bit...
		  v |= (~static_cast<uint64_t>(0)) << val_ಗಾತ್ರ;

	    } ಇಲ್ಲದಿದ್ದರೆ {
		    // Fill with zeros.
		  v &= ~((~static_cast<uint64_t>(0)) << val_ಗಾತ್ರ);
	    }

      }

      ಅಳಿಸು[]ಬಿಟ್ಟುಗಳು;
      ಹಿಂತಿರುಗಿ v;
}

/*
 * %ix/vec4 <idx>
 */
ತಾರ್ಕಿಕ ದ_IX_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ use_idx = cp->ಸಂಖ್ಯೆ;
      thr->ಪದಗಳು[use_idx].w_uint = vec4_ಗಮ್ಯ_ಸೂಚ್ಯಂಕ(thr, ಸುಳ್ಳು);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %ix/vec4/s <idx>
 */
ತಾರ್ಕಿಕ ದ_IX_VEC4_S(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ use_idx = cp->ಸಂಖ್ಯೆ;
      thr->ಪದಗಳು[use_idx].w_uint = vec4_ಗಮ್ಯ_ಸೂಚ್ಯಂಕ(thr, ನಿಜ);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * The various JMP instruction work simply by pulling the new program
 * counter from the instruction and resuming. If the jump is
 * conditional, then test the bit for the expected value first.
 */
ತಾರ್ಕಿಕ ದ_JMP(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      ಆದರೆ (ವೇಳಾಪಟ್ಟಿ_stopped()) {
	    ವೇಳಾಪಟ್ಟಿ_vthread(thr, 0, ಸುಳ್ಳು);
	    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
      }

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %jmp/0 <pc>, <flag>
 */
ತಾರ್ಕಿಕ ದ_JMP0(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಆದರೆ (thr->flags[cp->ಬಿಟ್_idx[0]] == BIT4_0)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      ಆದರೆ (ವೇಳಾಪಟ್ಟಿ_stopped()) {
	    ವೇಳಾಪಟ್ಟಿ_vthread(thr, 0, ಸುಳ್ಳು);
	    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
      }

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %jmp/0xz <pc>, <flag>
 */
ತಾರ್ಕಿಕ ದ_JMP0_XZ(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಆದರೆ (thr->flags[cp->ಬಿಟ್_idx[0]] != BIT4_1)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      ಆದರೆ (ವೇಳಾಪಟ್ಟಿ_stopped()) {
	    ವೇಳಾಪಟ್ಟಿ_vthread(thr, 0, ಸುಳ್ಳು);
	    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
      }

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %jmp/1 <pc>, <flag>
 */
ತಾರ್ಕಿಕ ದ_JMP1(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಆದರೆ (thr->flags[cp->ಬಿಟ್_idx[0]] == BIT4_1)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      ಆದರೆ (ವೇಳಾಪಟ್ಟಿ_stopped()) {
	    ವೇಳಾಪಟ್ಟಿ_vthread(thr, 0, ಸುಳ್ಳು);
	    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
      }

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %jmp/1xz <pc>, <flag>
 */
ತಾರ್ಕಿಕ ದ_JMP1_XZ(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಆದರೆ (thr->flags[cp->ಬಿಟ್_idx[0]] != BIT4_0)
	    thr->pc = cp->cptr;

	/* Normally, this returns true so that the processor just
	   keeps going to the next instruction. However, if there was
	   a $stop or vpiStop, returning false here can break the
	   simulation out of a hung loop. */
      ಆದರೆ (ವೇಳಾಪಟ್ಟಿ_stopped()) {
	    ವೇಳಾಪಟ್ಟಿ_vthread(thr, 0, ಸುಳ್ಳು);
	    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
      }

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * The %join instruction causes the thread to wait for one child
 * to die.  If a child is already dead (and a zombie) then I reap
 * it and go on. Otherwise, I mark myself as waiting in a join so that
 * children know to wake me when they finish.
 */

ಸ್ಥಿರ ಶೂನ್ಯ do_join(vthread_t thr, vthread_t ಮಗು)
{
      ಪ್ರತಿಜ್ಞಾ(ಮಗು->ಪೋಷಕ == thr);

        /* If the immediate child thread is in an automatic scope... */
      ಆದರೆ (ಮಗು->wt_context) {
              /* and is the top level task/function thread... */
            ಆದರೆ (thr->wt_context != thr->rd_context) {
                    /* Pop the child context from the write context stack. */
                  vvp_context_t ಮಗು_context = thr->wt_context;
                  thr->wt_context = vvp_get_stacked_context(ಮಗು_context);

                    /* Push the child context onto the read context stack */
                  vvp_ಗಣ_stacked_context(ಮಗು_context, thr->rd_context);
                  thr->rd_context = ಮಗು_context;
            }
      }

      vthread_reap(ಮಗು);
}

ಸ್ಥಿರ ತಾರ್ಕಿಕ do_join_opcode(vthread_t thr)
{
      ಪ್ರತಿಜ್ಞಾ( !thr->i_am_joining );
      ಪ್ರತಿಜ್ಞಾ( !thr->children.empty());

	// Are there any children that have already ended? If so, then
	// join with that one.
      ಗಾಗಿ (ಗಣ<vthread_t>::iterator cur = thr->children.begin()
		 ; cur != thr->children.ಕೊನೆ() ; ++cur) {
	    vthread_t curp = *cur;
	    ಆದರೆ (! curp->i_have_ended)
		  ಮುಂದುವರಿ;

	      // found something!
	    do_join(thr, curp);
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

	// Otherwise, tell my children to awaken me when they end,
	// then pause.
      thr->i_am_joining = 1;
      ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ತಾರ್ಕಿಕ ದ_JOIN(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      ಹಿಂತಿರುಗಿ do_join_opcode(thr);
}

/*
 * This %join/detach <n> instruction causes the thread to detach
 * threads that were created by an earlier %fork.
 */
ತಾರ್ಕಿಕ ದ_JOIN_DETACH(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_2 = cp->ಸಂಖ್ಯೆ;

      ಪ್ರತಿಜ್ಞಾ(ಎಣಿಕೆ_2 == thr->children.size());

      ಆಗ (! thr->children.empty()) {
	    vthread_t ಮಗು = *thr->children.begin();
	    ಪ್ರತಿಜ್ಞಾ(ಮಗು->ಪೋಷಕ == thr);

	      // We cannot detach automatic tasks/functions within an
	      // automatic scope. If we try to do that, we might make
	      // a mess of the allocation of the context. Note that it
	      // is OK if the child context is distinct (See %exec_ufunc.)
	    ಪ್ರತಿಜ್ಞಾ(ಮಗು->wt_context==0 || thr->wt_context!=ಮಗು->wt_context);
	    ಆದರೆ (ಮಗು->i_have_ended) {
		    // If the child has already ended, then reap it.
		  vthread_reap(ಮಗು);

	    } ಇಲ್ಲದಿದ್ದರೆ {
		  size_t res = ಮಗು->ಪೋಷಕ->children.erase(ಮಗು);
		  ಪ್ರತಿಜ್ಞಾ(res == 1);
		  ಮಗು->i_am_detached = 1;
		  thr->detached_children.insert(ಮಗು);
	    }
      }

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %load/ar <array-label>, <index>;
*/
ತಾರ್ಕಿಕ ದ_ತುಂಬು_AR(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ idx = cp->ಬಿಟ್_idx[0];
      ದ್ವಿಗುಣ ಶಬ್ದ;

	/* The result is 0.0 if the address is undefined. */
      ಆದರೆ (thr->flags[4] == BIT4_1) {
	    ಶಬ್ದ = 0.0;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಚಿಹ್ನೆರಹಿತ adr = thr->ಪದಗಳು[idx].w_int;
	    ಶಬ್ದ = cp->array->get_ಶಬ್ದ_r(adr);
      }

      thr->ತಳ್ಳು_real(ಶಬ್ದ);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು ELEM>
ಸ್ಥಿರ ತಾರ್ಕಿಕ ತುಂಬು_dar(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      int64_t adr = thr->ಪದಗಳು[3].w_int;
      vvp_net_t*net = cp->net;
      ಪ್ರತಿಜ್ಞಾ(net);

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      ಪ್ರತಿಜ್ಞಾ(obj);

      vvp_darray*darray = obj->get_object().peek<vvp_darray>();

      ELEM ಶಬ್ದ;
      ಆದರೆ (darray &&
          (adr >= 0) && (thr->flags[4] == BIT4_0)) // A defined address >= 0
	    darray->get_ಶಬ್ದ(adr, ಶಬ್ದ);
      ಇಲ್ಲದಿದ್ದರೆ
	    dq_default(ಶಬ್ದ, obj->size());

      vthread_ತಳ್ಳು(thr, ಶಬ್ದ);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %load/dar/r <array-label>;
 */
ತಾರ್ಕಿಕ ದ_ತುಂಬು_DAR_R(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ ತುಂಬು_dar<ದ್ವಿಗುಣ>(thr, cp);
}

/*
 * %load/dar/str <array-label>;
 */
ತಾರ್ಕಿಕ ದ_ತುಂಬು_DAR_STR(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ ತುಂಬು_dar<string>(thr, cp);
}

/*
 * %load/dar/vec4 <array-label>;
 */
ತಾರ್ಕಿಕ ದ_ತುಂಬು_DAR_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ ತುಂಬು_dar<vvp_vector4_t>(thr, cp);
}

/*
 * %load/prop/dar/vec4 <pid>, <wid>;
 *   Indexed read of queue/dynamic-array class property; object on stack,
 *   index in words[3] (same as %load/dar/vec4).
 */
ತಾರ್ಕಿಕ ದ_ತುಂಬು_PROP_DAR_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      size_t pid = cp->ಸಂಖ್ಯೆ;
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಬಿಟ್_idx[0];
      int64_t adr = thr->ಪದಗಳು[3].w_int;

      vvp_object_t& ತುದಿ = thr->peek_object();
      vvp_cobject*cobj = ತುದಿ.peek<vvp_cobject>();
      ಪ್ರತಿಜ್ಞಾ(cobj);

      vvp_object_t pobj;
      cobj->get_object(pid, pobj, 0);
      vvp_darray*darray = pobj.peek<vvp_darray>();

      vvp_vector4_t ಶಬ್ದ;
      ಆದರೆ (darray && (adr >= 0) && (thr->flags[4] == BIT4_0)) {
	    darray->get_ಶಬ್ದ(adr, ಶಬ್ದ);
      } ಇಲ್ಲದಿದ್ದರೆ {
	    dq_default(ಶಬ್ದ, wid);
      }

      thr->ತಳ್ಳು_vec4(ಶಬ್ದ);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %load/obj <var-label>
 */
ತಾರ್ಕಿಕ ದ_ತುಂಬು_OBJ(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_net_t*net = cp->net;
      vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      ಪ್ರತಿಜ್ಞಾ(fun);

      vvp_object_t val = fun->get_object();
      thr->ತಳ್ಳು_object(val);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %load/obja <index>
 *    Loads the object from array, using index <index> as the index
 *    value. If flags[4] == 1, the calculation of <index> may have
 *    failed, so push nil.
 */
ತಾರ್ಕಿಕ ದ_ತುಂಬು_OBJA(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ idx = cp->ಬಿಟ್_idx[0];
      vvp_object_t ಶಬ್ದ;

	/* The result is 0.0 if the address is undefined. */
      ಆದರೆ (thr->flags[4] == BIT4_1) {
	    ; // Return nil
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಚಿಹ್ನೆರಹಿತ adr = thr->ಪದಗಳು[idx].w_int;
	    cp->array->get_ಶಬ್ದ_obj(adr, ಶಬ್ದ);
      }

      thr->ತಳ್ಳು_object(ಶಬ್ದ);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %load/real <var-label>
 */
ತಾರ್ಕಿಕ ದ_ತುಂಬು_REAL(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      __vpiHandle*tmp = cp->handle;
      t_vpi_ಮೌಲ್ಯ val;

      val.format = vpiRealVal;
      vpi_get_ಮೌಲ್ಯ(tmp, &val);

      thr->ತಳ್ಳು_real(val.value.real);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %load/str <var-label>
 */
ತಾರ್ಕಿಕ ದ_ತುಂಬು_STR(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_net_t*net = cp->net;


      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (net->fun);
      ಪ್ರತಿಜ್ಞಾ(fun);

      ಸ್ಥಿರಾಂಕ string&val = fun->get_string();
      thr->ತಳ್ಳು_str(val);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_ತುಂಬು_STRA(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ idx = cp->ಬಿಟ್_idx[0];
      string ಶಬ್ದ;

      ಆದರೆ (thr->flags[4] == BIT4_1) {
	    ಶಬ್ದ = "";
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಚಿಹ್ನೆರಹಿತ adr = thr->ಪದಗಳು[idx].w_int;
	    ಶಬ್ದ = cp->array->get_ಶಬ್ದ_str(adr);
      }

      thr->ತಳ್ಳು_str(ಶಬ್ದ);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}


/*
 * %load/vec4 <net>
 */
ತಾರ್ಕಿಕ ದ_ತುಂಬು_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
	// Push a placeholder onto the stack in order to reserve the
	// stack space. Use a reference for the stack top as a target
	// for the load.
      thr->ತಳ್ಳು_vec4(vvp_vector4_t());
      vvp_vector4_t&sig_ಮೌಲ್ಯ = thr->peek_vec4();

      vvp_net_t*net = cp->net;

	// For the %load to work, the functor must actually be a
	// signal functor. Only signals save their vector value.
      vvp_signal_ಮೌಲ್ಯ*sig = dynamic_cast<vvp_signal_ಮೌಲ್ಯ*> (net->fil);
      ಆದರೆ (sig == 0) {
	    cerr << thr->get_fileline()
	         << "%load/v error: Net arg not a signal? "
		 << (net->fil ? typeid(*net->fil).name() :
	                        typeid(*net->fun).name())
	         << ಸಾಲು_ಅಂತ್ಯ;
	    ಪ್ರತಿಜ್ಞಾ(sig);
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

	// Extract the value from the signal and directly into the
	// target stack position.
      sig->vec4_ಮೌಲ್ಯ(sig_ಮೌಲ್ಯ);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %load/vec4a <arr>, <adrx>
 */
ತಾರ್ಕಿಕ ದ_ತುಂಬು_VEC4_A(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಪೂರ್ಣಾಂಕ adr_ಸೂಚ್ಯಂಕ = cp->ಬಿಟ್_idx[0];

      ದೀರ್ಘ adr = thr->ಪದಗಳು[adr_ಸೂಚ್ಯಂಕ].w_int;

	// If flag[3] is set, then the calculation of the address
	// failed, and this load should return X instead of the actual
	// value.
      ಆದರೆ (thr->flags[4] == BIT4_1) {
	    vvp_vector4_t tmp (cp->array->get_ಶಬ್ದ_ಗಾತ್ರ(), BIT4_X);
	    thr->ತಳ್ಳು_vec4(tmp);
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

      vvp_vector4_t tmp (cp->array->get_ಶಬ್ದ(adr));
      thr->ತಳ್ಳು_vec4(tmp);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಸ್ಥಿರ ಶೂನ್ಯ do_verylong_mod(vvp_vector4_t&vala, ಸ್ಥಿರಾಂಕ vvp_vector4_t&valb,
			    ತಾರ್ಕಿಕ ಎಡ_is_neg, ತಾರ್ಕಿಕ ಬಲ_is_neg)
{
      ತಾರ್ಕಿಕ ಹೊರಹೋಗು_is_neg = ಎಡ_is_neg;
      ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ len=vala.size();
      ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *a, *z, *t;
      a = ಹೊಸ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ[len+1];
      z = ಹೊಸ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ[len+1];
      t = ಹೊಸ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ[len+1];

      ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ವಾಹಕ;
      ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ತಾತ್ಕಾಲಿಕ;

      ಪೂರ್ಣಾಂಕ mxa = -1, mxz = -1;
      ಪೂರ್ಣಾಂಕ i;
      ಪೂರ್ಣಾಂಕ current, copylen;

      ಚಿಹ್ನೆರಹಿತ lb_ವಾಹಕ = ಎಡ_is_neg? 1 : 0;
      ಚಿಹ್ನೆರಹಿತ rb_ವಾಹಕ = ಬಲ_is_neg? 1 : 0;
      ಗಾಗಿ (ಪೂರ್ಣಾಂಕ idx = 0 ;  idx < len ;  idx += 1) {
	    ಚಿಹ್ನೆರಹಿತ lb = vala.value(idx);
	    ಚಿಹ್ನೆರಹಿತ rb = valb.value(idx);

	    ಆದರೆ ((lb | rb) & 2) {
		  ಅಳಿಸು []t;
		  ಅಳಿಸು []z;
		  ಅಳಿಸು []a;
		  vvp_vector4_t tmp(len, BIT4_X);
		  vala = tmp;
		  ಹಿಂತಿರುಗಿ;
	    }

	    ಆದರೆ (ಎಡ_is_neg) {
		  lb = (1-lb) + lb_ವಾಹಕ;
		  lb_ವಾಹಕ = (lb & ~1)? 1 : 0;
		  lb &= 1;
	    }
	    ಆದರೆ (ಬಲ_is_neg) {
		  rb = (1-rb) + rb_ವಾಹಕ;
		  rb_ವಾಹಕ = (rb & ~1)? 1 : 0;
		  rb &= 1;
	    }

	    z[idx]=lb;
	    a[idx]=1-rb;	// for 2s complement add..
      }

      z[len]=0;
      a[len]=1;

      ಗಾಗಿ(i=len-1;i>=0;i--) {
	    ಆದರೆ(! a[i]) {
		  mxa=i;
		  ನಿಲ್ಲಿಸು;
	    }
      }

      ಗಾಗಿ(i=len-1;i>=0;i--) {
	    ಆದರೆ(z[i]) {
		  mxz=i;
		  ನಿಲ್ಲಿಸು;
	    }
      }

      ಆದರೆ((mxa>mxz)||(mxa==-1)) {
	    ಆದರೆ(mxa==-1) {
		  ಅಳಿಸು []t;
		  ಅಳಿಸು []z;
		  ಅಳಿಸು []a;
		  vvp_vector4_t tmpx (len, BIT4_X);
		  vala = tmpx;
		  ಹಿಂತಿರುಗಿ;
	    }

	    ಹೋಗು tally;
      }

      copylen = mxa + 2;
      current = mxz - mxa;

      ಆಗ(current > -1) {
	    ವಾಹಕ = 1;
	    ಗಾಗಿ(i=0;i<copylen;i++) {
		  ತಾತ್ಕಾಲಿಕ = z[i+current] + a[i] + ವಾಹಕ;
		  t[i] = (ತಾತ್ಕಾಲಿಕ&1);
		  ವಾಹಕ = (ತಾತ್ಕಾಲಿಕ>>1);
	    }

	    ಆದರೆ(ವಾಹಕ) {
		  ಗಾಗಿ(i=0;i<copylen;i++) {
			z[i+current] = t[i];
		  }
	    }

	    current--;
      }

 tally:

      vvp_vector4_t tmp (len, BIT4_X);
      ವಾಹಕ = ಹೊರಹೋಗು_is_neg? 1 : 0;
      ಗಾಗಿ (ಪೂರ್ಣಾಂಕ idx = 0 ;  idx < len ;  idx += 1) {
	    ಚಿಹ್ನೆರಹಿತ ob = z[idx];
	    ಆದರೆ (ಹೊರಹೋಗು_is_neg) {
		  ob = (1-ob) + ವಾಹಕ;
		  ವಾಹಕ = (ob & ~1)? 1 : 0;
		  ob = ob & 1;
	    }
	    tmp.ಗಣ_ಬಿಟ್(idx, ob?BIT4_1:BIT4_0);
      }
      vala = tmp;
      ಅಳಿಸು []t;
      ಅಳಿಸು []z;
      ಅಳಿಸು []a;
}

ತಾರ್ಕಿಕ ದ_MAX_WR(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      ದ್ವಿಗುಣ r = thr->ತೆಗೆ_real();
      ದ್ವಿಗುಣ l = thr->ತೆಗೆ_real();
      ಆದರೆ (r != r)
	    thr->ತಳ್ಳು_real(l);
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (l != l)
	    thr->ತಳ್ಳು_real(r);
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (r < l)
	    thr->ತಳ್ಳು_real(l);
      ಇಲ್ಲದಿದ್ದರೆ
	    thr->ತಳ್ಳು_real(r);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_MIN_WR(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      ದ್ವಿಗುಣ r = thr->ತೆಗೆ_real();
      ದ್ವಿಗುಣ l = thr->ತೆಗೆ_real();
      ಆದರೆ (r != r)
	    thr->ತಳ್ಳು_real(l);
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (l != l)
	    thr->ತಳ್ಳು_real(r);
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (r < l)
	    thr->ತಳ್ಳು_real(r);
      ಇಲ್ಲದಿದ್ದರೆ
	    thr->ತಳ್ಳು_real(l);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_MOD(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      vvp_vector4_t valb = thr->ತೆಗೆ_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      ಪ್ರತಿಜ್ಞಾ(vala.size()==valb.size());
      ಚಿಹ್ನೆರಹಿತ wid = vala.size();

      ಆದರೆ(wid <= 8*ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ)) {
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ lv = 0, rv = 0;

	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < wid ;  idx += 1) {
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ lb = vala.value(idx);
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ rb = valb.value(idx);

		  ಆದರೆ ((lb | rb) & 2)
			ಹೋಗು x_ಹೊರಹೋಗು;

		  lv |= (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ) lb << idx;
		  rv |= (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ) rb << idx;
	    }

	    ಆದರೆ (rv == 0)
		  ಹೋಗು x_ಹೊರಹೋಗು;

	    lv %= rv;

	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < wid ;  idx += 1) {
		  vala.ಗಣ_ಬಿಟ್(idx, (lv&1)?BIT4_1 : BIT4_0);
		  lv >>= 1;
	    }

	    ಹಿಂತಿರುಗಿ ನಿಜ;

      } ಇಲ್ಲದಿದ್ದರೆ {
	    do_verylong_mod(vala, valb, ಸುಳ್ಳು, ಸುಳ್ಳು);
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

 x_ಹೊರಹೋಗು:
      vala = vvp_vector4_t(wid, BIT4_X);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %mod/s
 */
ತಾರ್ಕಿಕ ದ_MOD_S(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      vvp_vector4_t valb = thr->ತೆಗೆ_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();

      ಪ್ರತಿಜ್ಞಾ(vala.size()==valb.size());
      ಚಿಹ್ನೆರಹಿತ wid = vala.size();

	/* Handle the case that we can fit the bits into a long-long
	   variable. We cause use native % to do the work. */
      ಆದರೆ(wid <= 8*ಗಾತ್ರ(ದೀರ್ಘ ದೀರ್ಘ)) {
	    ದೀರ್ಘ ದೀರ್ಘ lv = 0, rv = 0;

	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < wid ;  idx += 1) {
		  ದೀರ್ಘ ದೀರ್ಘ lb = vala.value(idx);
		  ದೀರ್ಘ ದೀರ್ಘ rb = valb.value(idx);

		  ಆದರೆ ((lb | rb) & 2)
			ಹೋಗು x_ಹೊರಹೋಗು;

		  lv |= (ದೀರ್ಘ ದೀರ್ಘ) lb << idx;
		  rv |= (ದೀರ್ಘ ದೀರ್ಘ) rb << idx;
	    }

	    ಆದರೆ (rv == 0)
		  ಹೋಗು x_ಹೊರಹೋಗು;

	    ಆದರೆ ((lv == LLONG_MIN) && (rv == -1))
		  ಹೋಗು zero_ಹೊರಹೋಗು;

	      /* Sign extend the signed operands when needed. */
	    ಆದರೆ (wid < 8*ಗಾತ್ರ(ದೀರ್ಘ ದೀರ್ಘ)) {
		  ಆದರೆ (lv & (1LL << (wid-1)))
			lv |= -1ULL << wid;
		  ಆದರೆ (rv & (1LL << (wid-1)))
			rv |= -1ULL << wid;
	    }

	    lv %= rv;

	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < wid ;  idx += 1) {
		  vala.ಗಣ_ಬಿಟ್(idx, (lv&1)? BIT4_1 : BIT4_0);
		  lv >>= 1;
	    }

	      // vala is the top of the stack, edited in place, so we
	      // do not need to push the result.

	    ಹಿಂತಿರುಗಿ ನಿಜ;

      } ಇಲ್ಲದಿದ್ದರೆ {

	    ತಾರ್ಕಿಕ ಎಡ_is_neg  = vala.value(vala.size()-1) == BIT4_1;
	    ತಾರ್ಕಿಕ ಬಲ_is_neg = valb.value(valb.size()-1) == BIT4_1;
	    do_verylong_mod(vala, valb, ಎಡ_is_neg, ಬಲ_is_neg);
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

 x_ಹೊರಹೋಗು:
      vala = vvp_vector4_t(wid, BIT4_X);
      ಹಿಂತಿರುಗಿ ನಿಜ;
 zero_ಹೊರಹೋಗು:
      vala = vvp_vector4_t(wid, BIT4_0);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %mod/wr
 */
ತಾರ್ಕಿಕ ದ_MOD_WR(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      ದ್ವಿಗುಣ r = thr->ತೆಗೆ_real();
      ದ್ವಿಗುಣ l = thr->ತೆಗೆ_real();
      thr->ತಳ್ಳು_real(fmod(l,r));

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %neg/wr
 */
ತಾರ್ಕಿಕ ದ_NEG_WR(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      thr->poke_real(0, -thr->peek_real(0));
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %pad/s <wid>
 */
ತಾರ್ಕಿಕ ದ_PAD_S(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಸಂಖ್ಯೆ;

      vvp_vector4_t&val = thr->peek_vec4();
      ಚಿಹ್ನೆರಹಿತ old_ಗಾತ್ರ = val.size();

	// Sign-extend.
      ಆದರೆ (old_ಗಾತ್ರ < wid)
	    val.resize(wid, val.value(old_ಗಾತ್ರ-1));
      ಇಲ್ಲದಿದ್ದರೆ
	    val.resize(wid);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %pad/u <wid>
 */
ತಾರ್ಕಿಕ ದ_PAD_U(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಸಂಖ್ಯೆ;

      vvp_vector4_t&val = thr->peek_vec4();
      val.resize(wid, BIT4_0);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %part/s <wid>
 * %part/u <wid>
 * Two values are popped from the stack. First, pop the canonical
 * index of the part select, and second is the value to be
 * selected. The result is pushed back to the stack.
 */
ಸ್ಥಿರ ತಾರ್ಕಿಕ ದ_PART_ತಳ(vthread_t thr, vvp_ಸಂಕೇತ_t cp, ತಾರ್ಕಿಕ signed_flag)
{
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಸಂಖ್ಯೆ;

      vvp_vector4_t base4 = thr->ತೆಗೆ_vec4();
      vvp_vector4_t&value = thr->peek_vec4();

      vvp_vector4_t res (wid, BIT4_X);

	// NOTE: This is treating the vector as signed. Is that correct?
      int32_t ತಳ;
      ತಾರ್ಕಿಕ ಮೌಲ್ಯ_ok = vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(base4, ತಳ, signed_flag);
      ಆದರೆ (! ಮೌಲ್ಯ_ok) {
	    value = res;
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

      ಆದರೆ (ತಳ >= (int32_t)value.size()) {
	    value = res;
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

      ಆದರೆ ((ತಳ+(ಪೂರ್ಣಾಂಕ)wid) <= 0) {
	    value = res;
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

      ದೀರ್ಘ vbase = 0;
      ಆದರೆ (ತಳ < 0) {
	    vbase = -ತಳ;
	    wid -= vbase;
	    ತಳ = 0;
      }

      ಆದರೆ ((ತಳ+wid) > value.size()) {
	    wid = value.size() - ತಳ;
      }

      res .ಗಣ_vec(vbase, value.subvalue(ತಳ, wid));
      value = res;

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_PART_S(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ ದ_PART_ತಳ(thr, cp, ನಿಜ);
}

ತಾರ್ಕಿಕ ದ_PART_U(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ ದ_PART_ತಳ(thr, cp, ಸುಳ್ಳು);
}

/*
 * %parti/s <wid>, <basei>, <base_wid>
 * %parti/u <wid>, <basei>, <base_wid>
 *
 * Pop the value to be selected. The result is pushed back to the stack.
 */
ಸ್ಥಿರ ತಾರ್ಕಿಕ ದ_PARTI_ತಳ(vthread_t thr, vvp_ಸಂಕೇತ_t cp, ತಾರ್ಕಿಕ signed_flag)
{
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಸಂಖ್ಯೆ;
      uint32_t ತಳ = cp->ಬಿಟ್_idx[0];
      uint32_t bwid = cp->ಬಿಟ್_idx[1];

      vvp_vector4_t&value = thr->peek_vec4();

      vvp_vector4_t res (wid, BIT4_X);

	// NOTE: This is treating the vector as signed. Is that correct?
      int32_t use_ತಳ = ತಳ;
      ಆದರೆ (signed_flag && bwid < 32 && (ತಳ&(1<<(bwid-1)))) {
	    use_ತಳ |= -1UL << bwid;
      }

      ಆದರೆ (use_ತಳ >= (int32_t)value.size()) {
	    value = res;
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

      ಆದರೆ ((use_ತಳ+(int32_t)wid) <= 0) {
	    value = res;
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

      ದೀರ್ಘ vbase = 0;
      ಆದರೆ (use_ತಳ < 0) {
	    vbase = -use_ತಳ;
	    wid -= vbase;
	    use_ತಳ = 0;
      }

      ಆದರೆ ((use_ತಳ+wid) > value.size()) {
	    wid = value.size() - use_ತಳ;
      }

      res .ಗಣ_vec(vbase, value.subvalue(use_ತಳ, wid));
      value = res;

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_PARTI_S(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ ದ_PARTI_ತಳ(thr, cp, ನಿಜ);
}

ತಾರ್ಕಿಕ ದ_PARTI_U(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ ದ_PARTI_ತಳ(thr, cp, ಸುಳ್ಳು);
}

/*
 * %mul
 */
ತಾರ್ಕಿಕ ದ_MUL(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      vvp_vector4_t r = thr->ತೆಗೆ_vec4();
	// Rather then pop l, use it directly from the stack. When we
	// assign to 'l', that will edit the top of the stack, which
	// replaces a pop and a pull.
      vvp_vector4_t&l = thr->peek_vec4();

      l.mul(r);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %muli <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
ತಾರ್ಕಿಕ ದ_MULI(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಸಂಖ್ಯೆ;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_ತಕ್ಷಣ_rval (cp, r);

      l.mul(r);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_MUL_WR(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      ದ್ವಿಗುಣ r = thr->ತೆಗೆ_real();
      ದ್ವಿಗುಣ l = thr->ತೆಗೆ_real();
      thr->ತಳ್ಳು_real(l * r);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_NAND(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      vvp_vector4_t valr = thr->ತೆಗೆ_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      ಪ್ರತಿಜ್ಞಾ(vall.size() == valr.size());

      vall &= valr;
      vall.invert();

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %new/cobj <vpi_object>
 * This creates a new cobject (SystemVerilog class object) and pushes
 * it to the stack. The <vpi-object> is a __vpiHandle that is a
 * vpiClassDefn object that defines the item to be created.
 */
ತಾರ್ಕಿಕ ದ_NEW_COBJ(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಸ್ಥಿರಾಂಕ class_type*defn = dynamic_cast<ಸ್ಥಿರಾಂಕ class_type*> (cp->handle);
      ಪ್ರತಿಜ್ಞಾ(defn);

      vvp_object_t tmp (ಹೊಸ vvp_cobject(defn));
      thr->ತಳ್ಳು_object(tmp);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_NEW_DARRAY(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*ಪಠ್ಯ = cp->ಪಠ್ಯ;
      size_t size = thr->ಪದಗಳು[cp->ಬಿಟ್_idx[0]].w_int;
      ಚಿಹ್ನೆರಹಿತ ಶಬ್ದ_wid;
      size_t n;

      vvp_object_t obj;
      ಆದರೆ (strcmp(ಪಠ್ಯ,"b8") == 0) {
	    obj = ಹೊಸ vvp_darray_atom<uint8_t>(size);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(ಪಠ್ಯ,"b16") == 0) {
	    obj = ಹೊಸ vvp_darray_atom<uint16_t>(size);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(ಪಠ್ಯ,"b32") == 0) {
	    obj = ಹೊಸ vvp_darray_atom<uint32_t>(size);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(ಪಠ್ಯ,"b64") == 0) {
	    obj = ಹೊಸ vvp_darray_atom<uint64_t>(size);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(ಪಠ್ಯ,"sb8") == 0) {
	    obj = ಹೊಸ vvp_darray_atom<int8_t>(size);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(ಪಠ್ಯ,"sb16") == 0) {
	    obj = ಹೊಸ vvp_darray_atom<int16_t>(size);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(ಪಠ್ಯ,"sb32") == 0) {
	    obj = ಹೊಸ vvp_darray_atom<int32_t>(size);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(ಪಠ್ಯ,"sb64") == 0) {
	    obj = ಹೊಸ vvp_darray_atom<int64_t>(size);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((1 == ಅಕ್ಷರಸರಣಿ_ಓದು(ಪಠ್ಯ, "b%u%zn", &ಶಬ್ದ_wid, &n)) &&
                 (n == ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಪಠ್ಯ))) {
	    obj = ಹೊಸ vvp_darray_vec2(size, ಶಬ್ದ_wid);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((1 == ಅಕ್ಷರಸರಣಿ_ಓದು(ಪಠ್ಯ, "sb%u%zn", &ಶಬ್ದ_wid, &n)) &&
                 (n == ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಪಠ್ಯ))) {
	    obj = ಹೊಸ vvp_darray_vec2(size, ಶಬ್ದ_wid);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((1 == ಅಕ್ಷರಸರಣಿ_ಓದು(ಪಠ್ಯ, "v%u%zn", &ಶಬ್ದ_wid, &n)) &&
                 (n == ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಪಠ್ಯ))) {
	    obj = ಹೊಸ vvp_darray_vec4(size, ಶಬ್ದ_wid);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((1 == ಅಕ್ಷರಸರಣಿ_ಓದು(ಪಠ್ಯ, "sv%u%zn", &ಶಬ್ದ_wid, &n)) &&
                 (n == ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಪಠ್ಯ))) {
	    obj = ಹೊಸ vvp_darray_vec4(size, ಶಬ್ದ_wid);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(ಪಠ್ಯ,"r") == 0) {
	    obj = ಹೊಸ vvp_darray_real(size);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(ಪಠ್ಯ,"S") == 0) {
	    obj = ಹೊಸ vvp_darray_string(size);
      } ಇಲ್ಲದಿದ್ದರೆ {
	    cerr << get_fileline()
	         << "Internal error: Unsupported dynamic array type: "
	         << ಪಠ್ಯ << "." << ಸಾಲು_ಅಂತ್ಯ;
	    ಪ್ರತಿಜ್ಞಾ(0);
      }

      thr->ತಳ್ಳು_object(obj);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_NOOP(vthread_t, vvp_ಸಂಕೇತ_t)
{
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %nor/r
 */
ತಾರ್ಕಿಕ ದ_NORR(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_or());

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * Push a null to the object stack.
 */
ತಾರ್ಕಿಕ ದ_NULL(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      vvp_object_t tmp;
      thr->ತಳ್ಳು_object(tmp);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %and/r
 */
ತಾರ್ಕಿಕ ದ_ANDR(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_and());

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %nand/r
 */
ತಾರ್ಕಿಕ ದ_NANDR(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_and());

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %or/r
 */
ತಾರ್ಕಿಕ ದ_ORR(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_or());

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %xor/r
 */
ತಾರ್ಕಿಕ ದ_XORR(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, val.reduce_xor());

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %xnor/r
 */
ತಾರ್ಕಿಕ ದ_XNORR(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      vvp_vector4_t&val = thr->peek_vec4();
      val = vvp_vector4_t(1, ~val.reduce_xor());

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %or
 */
ತಾರ್ಕಿಕ ದ_OR(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      vvp_vector4_t valb = thr->ತೆಗೆ_vec4();
      vvp_vector4_t&vala = thr->peek_vec4();
      vala |= valb;
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %nor
 */
ತಾರ್ಕಿಕ ದ_NOR(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      vvp_vector4_t valr = thr->ತೆಗೆ_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      ಪ್ರತಿಜ್ಞಾ(vall.size() == valr.size());

      vall |= valr;
      vall.invert();

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %pop/obj <num>, <skip>
 */
ತಾರ್ಕಿಕ ದ_ತೆಗೆ_OBJ(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ cnt = cp->ಬಿಟ್_idx[0];
      ಚಿಹ್ನೆರಹಿತ skip = cp->ಬಿಟ್_idx[1];

      thr->ತೆಗೆ_object(cnt, skip);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %pop/real <number>
 */
ತಾರ್ಕಿಕ ದ_ತೆಗೆ_REAL(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ cnt = cp->ಸಂಖ್ಯೆ;
      thr->ತೆಗೆ_real(cnt);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 *  %pop/str <number>
 */
ತಾರ್ಕಿಕ ದ_ತೆಗೆ_STR(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ cnt = cp->ಸಂಖ್ಯೆ;
      thr->ತೆಗೆ_str(cnt);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 *  %pop/vec4 <number>
 */
ತಾರ್ಕಿಕ ದ_ತೆಗೆ_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ cnt = cp->ಸಂಖ್ಯೆ;
      thr->ತೆಗೆ_vec4(cnt);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %pow
 * %pow/s
 */
ಸ್ಥಿರ ತಾರ್ಕಿಕ ದ_POW_ತಳ(vthread_t thr, ತಾರ್ಕಿಕ signed_flag)
{
      vvp_vector4_t valb = thr->ತೆಗೆ_vec4();
      vvp_vector4_t vala = thr->ತೆಗೆ_vec4();

      ಚಿಹ್ನೆರಹಿತ wid = vala.size();

      vvp_vector2_t xv2 = vvp_vector2_t(vala, ನಿಜ);
      vvp_vector2_t yv2 = vvp_vector2_t(valb, ನಿಜ);


        /* If we have an X or Z in the arguments return X. */
      ಆದರೆ (xv2.is_NaN() || yv2.is_NaN()) {
	    vvp_vector4_t tmp (wid, BIT4_X);
	    thr->ತಳ್ಳು_vec4(tmp);
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

	// Is the exponent negative? If so, table 5-6 in IEEE1364-2005
	// defines what value is returned.
      ಆದರೆ (signed_flag && yv2.value(yv2.size()-1)) {
	    ಪೂರ್ಣಾಂಕ a_val;
	    vvp_bit4_t pad = BIT4_0, lsb = BIT4_0;
	    ಆದರೆ (vector2_ಗಮ್ಯ_ಮೌಲ್ಯ(xv2, a_val, ನಿಜ)) {
		  ಆದರೆ (a_val == 0) {
			pad = BIT4_X; lsb = BIT4_X;
		  }
		  ಆದರೆ (a_val == 1) {
			pad = BIT4_0; lsb = BIT4_1;
		  }
		  ಆದರೆ (a_val == -1) {
			ಆದರೆ (yv2.value(0)) {
			      pad = BIT4_1; lsb = BIT4_1;
			} ಇಲ್ಲದಿದ್ದರೆ {
			      pad = BIT4_0; lsb = BIT4_1;
			}
		  }
	    }
	    vvp_vector4_t tmp (wid, pad);
	    tmp.ಗಣ_ಬಿಟ್(0, lsb);
	    thr->ತಳ್ಳು_vec4(tmp);
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

      vvp_vector2_t ಫಲಿತಾಂಶ = ಘಾತ(xv2, yv2);

        /* Copy only what we need of the result. If the result is too
	   small, zero-pad it. */
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ jdx = 0;  jdx < wid;  jdx += 1) {
	    ಆದರೆ (jdx >= ಫಲಿತಾಂಶ.size())
		  vala.ಗಣ_ಬಿಟ್(jdx, BIT4_0);
	    ಇಲ್ಲದಿದ್ದರೆ
		  vala.ಗಣ_ಬಿಟ್(jdx, ಫಲಿತಾಂಶ.value(jdx) ? BIT4_1 : BIT4_0);
      }
      thr->ತಳ್ಳು_vec4(vala);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_POW(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      ಹಿಂತಿರುಗಿ ದ_POW_ತಳ(thr, ಸುಳ್ಳು);
}

ತಾರ್ಕಿಕ ದ_POW_S(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      ಹಿಂತಿರುಗಿ ದ_POW_ತಳ(thr, ನಿಜ);
}

ತಾರ್ಕಿಕ ದ_POW_WR(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      ದ್ವಿಗುಣ r = thr->ತೆಗೆ_real();
      ದ್ವಿಗುಣ l = thr->ತೆಗೆ_real();
      thr->ತಳ್ಳು_real(ಘಾತ(l,r));

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %prop/obj <pid>, <idx>
 *
 * Load an object value from the cobject and push it onto the object stack.
 */
ತಾರ್ಕಿಕ ದ_PROP_OBJ(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ pid = cp->ಸಂಖ್ಯೆ;
      ಚಿಹ್ನೆರಹಿತ idx = cp->ಬಿಟ್_idx[0];

      ಆದರೆ (idx != 0) {
	    ಪ್ರತಿಜ್ಞಾ(idx < vthread_s::ಪದಗಳು_ಎಣಿಕೆ);
	    idx = thr->ಪದಗಳು[idx].w_uint;
      }

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();

      vvp_object_t val;
      cobj->get_object(pid, val, idx);

      thr->ತಳ್ಳು_object(val);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಸ್ಥಿರ ಶೂನ್ಯ get_from_obj(ಚಿಹ್ನೆರಹಿತ pid, vvp_cobject*cobj, ದ್ವಿಗುಣ&val)
{
      val = cobj->get_real(pid);
}

ಸ್ಥಿರ ಶೂನ್ಯ get_from_obj(ಚಿಹ್ನೆರಹಿತ pid, vvp_cobject*cobj, string&val)
{
      val = cobj->get_string(pid);
}

ಸ್ಥಿರ ಶೂನ್ಯ get_from_obj(ಚಿಹ್ನೆರಹಿತ pid, vvp_cobject*cobj, vvp_vector4_t&val)
{
      cobj->get_vec4(pid, val);
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು ELEM>
ಸ್ಥಿರ ತಾರ್ಕಿಕ prop(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ pid = cp->ಸಂಖ್ಯೆ;

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      ಪ್ರತಿಜ್ಞಾ(cobj);

      ELEM val;
      get_from_obj(pid, cobj, val);
      vthread_ತಳ್ಳು(thr, val);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %prop/r <pid>
 *
 * Load a real value from the cobject and push it onto the real value
 * stack.
 */
ತಾರ್ಕಿಕ ದ_PROP_R(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ prop<ದ್ವಿಗುಣ>(thr, cp);
}

/*
 * %prop/str <pid>
 *
 * Load a string value from the cobject and push it onto the real value
 * stack.
 */
ತಾರ್ಕಿಕ ದ_PROP_STR(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ prop<string>(thr, cp);
}

/*
 * %prop/v <pid>
 *
 * Load a property <pid> from the cobject on the top of the stack into
 * the vector space at <base>.
 */
ತಾರ್ಕಿಕ ದ_PROP_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ prop<vvp_vector4_t>(thr, cp);
}

ತಾರ್ಕಿಕ ದ_PUSHI_REAL(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ದ್ವಿಗುಣ mant = cp->ಬಿಟ್_idx[0];
      uint32_t imant = cp->ಬಿಟ್_idx[0];
      ಪೂರ್ಣಾಂಕ ಘಾತಾಂಕ = cp->ಬಿಟ್_idx[1];

	// Detect +infinity
      ಆದರೆ (ಘಾತಾಂಕ==0x3fff && imant==0) {
	    thr->ತಳ್ಳು_real(INFINITY);
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }
	// Detect -infinity
      ಆದರೆ (ಘಾತಾಂಕ==0x7fff && imant==0) {
	    thr->ತಳ್ಳು_real(-INFINITY);
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }
	// Detect NaN
      ಆದರೆ (ಘಾತಾಂಕ==0x3fff) {
	    thr->ತಳ್ಳು_real(nan(""));
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

      ದ್ವಿಗುಣ sign = (ಘಾತಾಂಕ & 0x4000)? -1.0 : 1.0;

      ಘಾತಾಂಕ &= 0x1fff;

      mant = sign * ldexp(mant, ಘಾತಾಂಕ - 0x1000);
      thr->ತಳ್ಳು_real(mant);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_PUSHI_STR(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*ಪಠ್ಯ = cp->ಪಠ್ಯ;
      thr->ತಳ್ಳು_str(filter_string(ಪಠ್ಯ));
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %pushi/vec4 <vala>, <valb>, <wid>
 */
ತಾರ್ಕಿಕ ದ_PUSHI_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ wid  = cp->ಸಂಖ್ಯೆ;

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t val (wid, BIT4_0);
      get_ತಕ್ಷಣ_rval (cp, val);

      thr->ತಳ್ಳು_vec4(val);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %pushv/str
 *   Pops a vec4 value, and pushes a string.
 */
ತಾರ್ಕಿಕ ದ_PUSHV_STR(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      vvp_vector4_t vec = thr->ತೆಗೆ_vec4();

      size_t slen = (vec.size() + 7)/8;
      vector<ಅಕ್ಷರ>buf;
      buf.reserve(slen);

      ಗಾಗಿ (size_t idx = 0 ; idx < vec.size() ; idx += 8) {
	    ಅಕ್ಷರ tmp = 0;
	    size_t trans = 8;
	    ಆದರೆ (idx+trans > vec.size())
		  trans = vec.size() - idx;

	    ಗಾಗಿ (size_t bdx = 0 ; bdx < trans ; bdx += 1) {
		  ಆದರೆ (vec.value(idx+bdx) == BIT4_1)
			tmp |= 1 << bdx;
	    }

	    ಆದರೆ (tmp != 0)
		  buf.push_back(tmp);
      }

      string val;
      ಗಾಗಿ (vector<ಅಕ್ಷರ>::reverse_iterator cur = buf.rbegin()
		 ; cur != buf.rend() ; ++cur) {
	    val.push_back(*cur);
      }

      thr->ತಳ್ಳು_str(val);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %putc/str/vec4 <var>, <mux>
 */
ತಾರ್ಕಿಕ ದ_PUTC_STR_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ muxr = cp->ಬಿಟ್_idx[0];
      int32_t mux = muxr? thr->ಪದಗಳು[muxr].w_int : 0;

      vvp_vector4_t val = thr->ತೆಗೆ_vec4();
      ಪ್ರತಿಜ್ಞಾ(val.size() == 8);

      ಆದರೆ (mux < 0)
	    ಹಿಂತಿರುಗಿ ನಿಜ;

	/* Get the existing value of the string. If we find that the
	   index is too big for the string, then give up. */
      vvp_net_t*net = cp->net;
      vvp_fun_signal_string*fun = dynamic_cast<vvp_fun_signal_string*> (net->fun);
      ಪ್ರತಿಜ್ಞಾ(fun);

      string tmp = fun->get_string();
      ಆದರೆ (tmp.size() <= (size_t)mux)
	    ಹಿಂತಿರುಗಿ ನಿಜ;

      ಅಕ್ಷರ val_str = 0;
      ಗಾಗಿ (size_t idx = 0 ; idx < 8 ; idx += 1) {
	    ಆದರೆ (val.value(idx)==BIT4_1)
		  val_str |= 1<<idx;
      }

	// It is a quirk of the Verilog standard that putc(..., 'h00)
	// has no effect. Test for that case here.
      ಆದರೆ (val_str == 0)
	    ಹಿಂತಿರುಗಿ ನಿಜ;

      tmp[mux] = val_str;

      vvp_send_string(vvp_net_ptr_t(cp->net, 0), tmp, thr->wt_context);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು ELEM, ವರ್ಗ QTYPE>
ಸ್ಥಿರ ತಾರ್ಕಿಕ qinsert(vthread_t thr, vvp_ಸಂಕೇತ_t cp, ಚಿಹ್ನೆರಹಿತ wid=0)
{
      int64_t idx = thr->ಪದಗಳು[3].w_int;
      ELEM value;
      vvp_net_t*net = cp->net;
      ತೆಗೆ_ಮೌಲ್ಯ(thr, value, wid); // Pop the value to store.

      vvp_ಸರತಿ*ಸರತಿ = get_ಸರತಿ_object<QTYPE>(thr, net);
      ಪ್ರತಿಜ್ಞಾ(ಸರತಿ);
      ಆದರೆ (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at a negative "
	         << get_ಸರತಿ_type(value)
	         << " index (" << idx << "). ";
	    print_ಸರತಿ_ಮೌಲ್ಯ(value);
	    cerr << " was not added." << ಸಾಲು_ಅಂತ್ಯ;
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at an undefined "
	         << get_ಸರತಿ_type(value) << " index. ";
	    print_ಸರತಿ_ಮೌಲ್ಯ(value);
	    cerr << " was not added." << ಸಾಲು_ಅಂತ್ಯ;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ = thr->ಪದಗಳು[cp->ಬಿಟ್_idx[0]].w_int;
	    ಸರತಿ->insert(idx, value, max_ಗಾತ್ರ);
      }
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %qinsert/real <var-label>
 */
ತಾರ್ಕಿಕ ದ_QINSERT_REAL(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ qinsert<ದ್ವಿಗುಣ, vvp_ಸರತಿ_real>(thr, cp);
}

/*
 * %qinsert/str <var-label>
 */
ತಾರ್ಕಿಕ ದ_QINSERT_STR(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ qinsert<string, vvp_ಸರತಿ_string>(thr, cp);
}

/*
 * %qinsert/v <var-label>
 */
ತಾರ್ಕಿಕ ದ_QINSERT_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ qinsert<vvp_vector4_t, vvp_ಸರತಿ_vec4>(thr, cp, cp->ಬಿಟ್_idx[1]);
}

/*
 * Helper functions used in the queue pop templates
 */
ಸಾಲೊಳಗೆ ಶೂನ್ಯ ತಳ್ಳು_ಮೌಲ್ಯ(vthread_t thr, ದ್ವಿಗುಣ value, ಚಿಹ್ನೆರಹಿತ)
{
      thr->ತಳ್ಳು_real(value);
}

ಸಾಲೊಳಗೆ ಶೂನ್ಯ ತಳ್ಳು_ಮೌಲ್ಯ(vthread_t thr, ಸ್ಥಿರಾಂಕ string&value, ಚಿಹ್ನೆರಹಿತ)
{
      thr->ತಳ್ಳು_str(value);
}

ಸಾಲೊಳಗೆ ಶೂನ್ಯ ತಳ್ಳು_ಮೌಲ್ಯ(vthread_t thr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&value, ಚಿಹ್ನೆರಹಿತ wid)
{
      ಪ್ರತಿಜ್ಞಾ(wid == value.size());
      thr->ತಳ್ಳು_vec4(value);
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು ELEM, ವರ್ಗ QTYPE>
ಸ್ಥಿರ ತಾರ್ಕಿಕ q_ತೆಗೆ(vthread_t thr, vvp_ಸಂಕೇತ_t cp,
                  ಶೂನ್ಯ (*get_val_func)(vvp_ಸರತಿ*, ELEM&),
                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*loc, ಚಿಹ್ನೆರಹಿತ wid)
{
      vvp_net_t*net = cp->net;

      vvp_ಸರತಿ*ಸರತಿ = get_ಸರತಿ_object<QTYPE>(thr, net);
      ಪ್ರತಿಜ್ಞಾ(ಸರತಿ);

      size_t size = ಸರತಿ->get_ಗಾತ್ರ();

      ELEM value;
      ಆದರೆ (size) {
	    get_val_func(ಸರತಿ, value);
      } ಇಲ್ಲದಿದ್ದರೆ {
	    dq_default(value, wid);
	    cerr << thr->get_fileline()
	         << "Warning: pop_" << loc << "() on empty "
	         << get_ಸರತಿ_type(value) << "." << ಸಾಲು_ಅಂತ್ಯ;
      }

      ತಳ್ಳು_ಮೌಲ್ಯ(thr, value, wid);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು ELEM>
ಸ್ಥಿರ ಶೂನ್ಯ get_ಹಿಂಭಾಗ_ಮೌಲ್ಯ(vvp_ಸರತಿ*ಸರತಿ, ELEM&value)
{
      ಸರತಿ->get_ಶಬ್ದ(ಸರತಿ->get_ಗಾತ್ರ()-1, value);
      ಸರತಿ->pop_back();
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು ELEM, ವರ್ಗ QTYPE>
ಸ್ಥಿರ ತಾರ್ಕಿಕ qpop_b(vthread_t thr, vvp_ಸಂಕೇತ_t cp, ಚಿಹ್ನೆರಹಿತ wid=0)
{
      ಹಿಂತಿರುಗಿ q_ತೆಗೆ<ELEM, QTYPE>(thr, cp, get_ಹಿಂಭಾಗ_ಮೌಲ್ಯ<ELEM>, "back", wid);
}

/*
 * %qpop/b/real <var-label>
 */
ತಾರ್ಕಿಕ ದ_QPOP_B_REAL(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ qpop_b<ದ್ವಿಗುಣ, vvp_ಸರತಿ_real>(thr, cp);
}

/*
 * %qpop/b/str <var-label>
 */
ತಾರ್ಕಿಕ ದ_QPOP_B_STR(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ qpop_b<string, vvp_ಸರತಿ_string>(thr, cp);
}

/*
 * %qpop/b/v <var-label>
 */
ತಾರ್ಕಿಕ ದ_QPOP_B_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ qpop_b<vvp_vector4_t, vvp_ಸರತಿ_vec4>(thr, cp, cp->ಬಿಟ್_idx[0]);
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು ELEM>
ಸ್ಥಿರ ಶೂನ್ಯ get_front_ಮೌಲ್ಯ(vvp_ಸರತಿ*ಸರತಿ, ELEM&value)
{
      ಸರತಿ->get_ಶಬ್ದ(0, value);
      ಸರತಿ->pop_front();
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು ELEM, ವರ್ಗ QTYPE>
ಸ್ಥಿರ ತಾರ್ಕಿಕ qpop_f(vthread_t thr, vvp_ಸಂಕೇತ_t cp, ಚಿಹ್ನೆರಹಿತ wid=0)
{
      ಹಿಂತಿರುಗಿ q_ತೆಗೆ<ELEM, QTYPE>(thr, cp, get_front_ಮೌಲ್ಯ<ELEM>, "front", wid);
}


/*
 * %qpop/f/real <var-label>
 */
ತಾರ್ಕಿಕ ದ_QPOP_F_REAL(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ qpop_f<ದ್ವಿಗುಣ, vvp_ಸರತಿ_real>(thr, cp);
}

/*
 * %qpop/f/str <var-label>
 */
ತಾರ್ಕಿಕ ದ_QPOP_F_STR(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ qpop_f<string, vvp_ಸರತಿ_string>(thr, cp);
}

/*
 * %qpop/f/v <var-label>
 */
ತಾರ್ಕಿಕ ದ_QPOP_F_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ qpop_f<vvp_vector4_t, vvp_ಸರತಿ_vec4>(thr, cp, cp->ಬಿಟ್_idx[0]);
}

/*
 * These implement the %release/net and %release/reg instructions. The
 * %release/net instruction applies to a net kind of functor by
 * sending the release/net command to the command port. (See vvp_net.h
 * for details.) The %release/reg instruction is the same, but sends
 * the release/reg command instead. These are very similar to the
 * %deassign instruction.
 */
ಸ್ಥಿರ ತಾರ್ಕಿಕ do_release_vec(vvp_ಸಂಕೇತ_t cp, ತಾರ್ಕಿಕ net_flag)
{
      vvp_net_t*net = cp->net;
      ಚಿಹ್ನೆರಹಿತ ತಳ  = cp->ಬಿಟ್_idx[0];
      ಚಿಹ್ನೆರಹಿತ width = cp->ಬಿಟ್_idx[1];

      ಪ್ರತಿಜ್ಞಾ(net->fil);

      ಆದರೆ (ತಳ >= net->fil->filter_ಗಾತ್ರ()) ಹಿಂತಿರುಗಿ ನಿಜ;
      ಆದರೆ (ತಳ+width > net->fil->filter_ಗಾತ್ರ())
	    width = net->fil->filter_ಗಾತ್ರ() - ತಳ;

      ತಾರ್ಕಿಕ ತುಂಬಿದ_sig = ತಳ == 0 && width == net->fil->filter_ಗಾತ್ರ();

	// XXXX Can't really do this if this is a partial release?
      net->fil->force_unlink();

	/* Do we release all or part of the net? */
      vvp_net_ptr_t ptr (net, 0);
      ಆದರೆ (ತುಂಬಿದ_sig) {
	    net->fil->release(ptr, net_flag);
      } ಇಲ್ಲದಿದ್ದರೆ {
	    net->fil->release_pv(ptr, ತಳ, width, net_flag);
      }
      net->fun->force_flag(ಸುಳ್ಳು);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_RELEASE_NET(vthread_t, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ do_release_vec(cp, ನಿಜ);
}


ತಾರ್ಕಿಕ ದ_RELEASE_REG(vthread_t, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ do_release_vec(cp, ಸುಳ್ಳು);
}

/* The type is 1 for registers and 0 for everything else. */
ತಾರ್ಕಿಕ ದ_RELEASE_WR(vthread_t, vvp_ಸಂಕೇತ_t cp)
{
      vvp_net_t*net = cp->net;
      ಚಿಹ್ನೆರಹಿತ type  = cp->ಬಿಟ್_idx[0];

      ಪ್ರತಿಜ್ಞಾ(net->fil);
      net->fil->force_unlink();

	// Send a command to this signal to unforce itself.
      vvp_net_ptr_t ptr (net, 0);
      net->fil->release(ptr, type==0);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_REPLICATE(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಪೂರ್ಣಾಂಕ rept = cp->ಸಂಖ್ಯೆ;
      vvp_vector4_t val = thr->ತೆಗೆ_vec4();
      vvp_vector4_t res (val.size() * rept, BIT4_X);

      ಗಾಗಿ (ಪೂರ್ಣಾಂಕ idx = 0 ; idx < rept ; idx += 1) {
	    res.ಗಣ_vec(idx * val.size(), val);
      }

      thr->ತಳ್ಳು_vec4(res);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಸ್ಥಿರ ಶೂನ್ಯ poke_val(vthread_t fun_thr, ಚಿಹ್ನೆರಹಿತ ಆಳ, ದ್ವಿಗುಣ val)
{
      fun_thr->ಪೋಷಕ->poke_real(ಆಳ, val);
}

ಸ್ಥಿರ ಶೂನ್ಯ poke_val(vthread_t fun_thr, ಚಿಹ್ನೆರಹಿತ ಆಳ, ಸ್ಥಿರಾಂಕ string&val)
{
      fun_thr->ಪೋಷಕ->poke_str(ಆಳ, val);
}

ಸ್ಥಿರ size_t get_max(vthread_t fun_thr, ದ್ವಿಗುಣ&)
{
      ಹಿಂತಿರುಗಿ fun_thr->ವಾದಗಳು_real.size();
}

ಸ್ಥಿರ size_t get_max(vthread_t fun_thr, string&)
{
      ಹಿಂತಿರುಗಿ fun_thr->ವಾದಗಳು_str.size();
}

ಸ್ಥಿರ size_t get_max(vthread_t fun_thr, vvp_vector4_t&)
{
      ಹಿಂತಿರುಗಿ fun_thr->ವಾದಗಳು_vec4.size();
}

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ get_ಆಳ(vthread_t fun_thr, size_t ಸೂಚ್ಯಂಕ, ದ್ವಿಗುಣ&)
{
      ಹಿಂತಿರುಗಿ fun_thr->ವಾದಗಳು_real[ಸೂಚ್ಯಂಕ];
}

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ get_ಆಳ(vthread_t fun_thr, size_t ಸೂಚ್ಯಂಕ, string&)
{
      ಹಿಂತಿರುಗಿ fun_thr->ವಾದಗಳು_str[ಸೂಚ್ಯಂಕ];
}

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ get_ಆಳ(vthread_t fun_thr, size_t ಸೂಚ್ಯಂಕ, vvp_vector4_t&)
{
      ಹಿಂತಿರುಗಿ fun_thr->ವಾದಗಳು_vec4[ಸೂಚ್ಯಂಕ];
}

ಸ್ಥಿರ vthread_t get_func(vthread_t thr)
{
      vthread_t fun_thr = thr;

      ಆಗ (fun_thr->ಪೋಷಕ_scope->get_type_ಸಂಕೇತ() != vpiFunction) {
	    ಪ್ರತಿಜ್ಞಾ(fun_thr->ಪೋಷಕ);
	    fun_thr = fun_thr->ಪೋಷಕ;
      }

      ಹಿಂತಿರುಗಿ fun_thr;
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು ELEM>
ಸ್ಥಿರ ತಾರ್ಕಿಕ ret(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      size_t ಸೂಚ್ಯಂಕ = cp->ಸಂಖ್ಯೆ;
      ELEM val;
      ತೆಗೆ_ಮೌಲ್ಯ(thr, val, 0);

      vthread_t fun_thr = get_func(thr);
      ಪ್ರತಿಜ್ಞಾ(ಸೂಚ್ಯಂಕ < get_max(fun_thr, val));

      ಚಿಹ್ನೆರಹಿತ ಆಳ = get_ಆಳ(fun_thr, ಸೂಚ್ಯಂಕ, val);
	// Use the depth to put the value into the stack of
	// the parent thread.
      poke_val(fun_thr, ಆಳ, val);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %ret/real <index>
 */
ತಾರ್ಕಿಕ ದ_RET_REAL(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ ret<ದ್ವಿಗುಣ>(thr, cp);
}

/*
 * %ret/str <index>
 */
ತಾರ್ಕಿಕ ದ_RET_STR(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ ret<string>(thr, cp);
}

/*
 * %ret/vec4 <index>, <offset>, <wid>
 */
ತಾರ್ಕಿಕ ದ_RET_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      size_t ಸೂಚ್ಯಂಕ = cp->ಸಂಖ್ಯೆ;
      ಚಿಹ್ನೆರಹಿತ off_ಸೂಚ್ಯಂಕ = cp->ಬಿಟ್_idx[0];
      ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ wid = cp->ಬಿಟ್_idx[1];
      vvp_vector4_t&val = thr->peek_vec4();

      vthread_t fun_thr = get_func(thr);
      ಪ್ರತಿಜ್ಞಾ(ಸೂಚ್ಯಂಕ < get_max(fun_thr, val));
      ಪ್ರತಿಜ್ಞಾ(val.size() == wid);
      ಚಿಹ್ನೆರಹಿತ ಆಳ = get_ಆಳ(fun_thr, ಸೂಚ್ಯಂಕ, val);

      int64_t off = off_ಸೂಚ್ಯಂಕ ? thr->ಪದಗಳು[off_ಸೂಚ್ಯಂಕ].w_int : 0;
      ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ sig_ಮೌಲ್ಯ_ಗಾತ್ರ = fun_thr->ಪೋಷಕ->peek_vec4(ಆಳ).size();

      ಆದರೆ (off_ಸೂಚ್ಯಂಕ!=0 && thr->flags[4] == BIT4_1) {
	    thr->ತೆಗೆ_vec4(1);
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

      ಆದರೆ (!resize_rval_vec(val, off, sig_ಮೌಲ್ಯ_ಗಾತ್ರ)) {
	    thr->ತೆಗೆ_vec4(1);
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

      ಆದರೆ (off == 0 && val.size() == sig_ಮೌಲ್ಯ_ಗಾತ್ರ) {
	    fun_thr->ಪೋಷಕ->poke_vec4(ಆಳ, val);
      } ಇಲ್ಲದಿದ್ದರೆ {
	    vvp_vector4_t tmp_dst = fun_thr->ಪೋಷಕ->peek_vec4(ಆಳ);
	    tmp_dst.ಗಣ_vec(off, val);
	    fun_thr->ಪೋಷಕ->poke_vec4(ಆಳ, tmp_dst);
      }

      thr->ತೆಗೆ_vec4(1);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಸ್ಥಿರ ಶೂನ್ಯ ತಳ್ಳು_from_ಪೋಷಕ(vthread_t thr, vthread_t fun_thr, ಚಿಹ್ನೆರಹಿತ ಆಳ, ದ್ವಿಗುಣ&)
{
      thr->ತಳ್ಳು_real(fun_thr->ಪೋಷಕ->peek_real(ಆಳ));
}

ಸ್ಥಿರ ಶೂನ್ಯ ತಳ್ಳು_from_ಪೋಷಕ(vthread_t thr, vthread_t fun_thr, ಚಿಹ್ನೆರಹಿತ ಆಳ, string&)
{
      thr->ತಳ್ಳು_str(fun_thr->ಪೋಷಕ->peek_str(ಆಳ));
}

ಸ್ಥಿರ ಶೂನ್ಯ ತಳ್ಳು_from_ಪೋಷಕ(vthread_t thr, vthread_t fun_thr, ಚಿಹ್ನೆರಹಿತ ಆಳ, vvp_vector4_t&)
{
      thr->ತಳ್ಳು_vec4(fun_thr->ಪೋಷಕ->peek_vec4(ಆಳ));
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು ELEM>
ಸ್ಥಿರ ತಾರ್ಕಿಕ retload(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      size_t ಸೂಚ್ಯಂಕ = cp->ಸಂಖ್ಯೆ;
      ELEM type;

      vthread_t fun_thr = get_func(thr);
      ಪ್ರತಿಜ್ಞಾ(ಸೂಚ್ಯಂಕ < get_max(fun_thr, type));

      ಚಿಹ್ನೆರಹಿತ ಆಳ = get_ಆಳ(fun_thr, ಸೂಚ್ಯಂಕ, type);
	// Use the depth to extract the values from the stack
	// of the parent thread.
      ತಳ್ಳು_from_ಪೋಷಕ(thr, fun_thr, ಆಳ, type);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %retload/real <index>
 */
ತಾರ್ಕಿಕ ದ_RETLOAD_REAL(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ retload<ದ್ವಿಗುಣ>(thr, cp);
}

/*
 * %retload/str <index>
 */
ತಾರ್ಕಿಕ ದ_RETLOAD_STR(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ retload<string>(thr, cp);
}

/*
 * %retload/vec4 <index>
 */
ತಾರ್ಕಿಕ ದ_RETLOAD_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ retload<vvp_vector4_t>(thr, cp);
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
ತಾರ್ಕಿಕ ದ_SCOPY(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      vvp_object_t tmp;
      thr->ತೆಗೆ_object(tmp);

      vvp_object_t&dest = thr->peek_object();
        // If it is null there is nothing to copy
      ಆದರೆ (!tmp.test_nil())
	    dest.shallow_copy(tmp);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಸ್ಥಿರ ಶೂನ್ಯ thread_peek(vthread_t thr, ದ್ವಿಗುಣ&value)
{
      value = thr->peek_real(0);
}

ಸ್ಥಿರ ಶೂನ್ಯ thread_peek(vthread_t thr, string&value)
{
      value = thr->peek_str(0);
}

ಸ್ಥಿರ ಶೂನ್ಯ thread_peek(vthread_t thr, vvp_vector4_t&value)
{
      value = thr->peek_vec4(0);
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು ELEM>
ಸ್ಥಿರ ತಾರ್ಕಿಕ ಗಣ_dar_obj(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ adr = thr->ಪದಗಳು[cp->ಸಂಖ್ಯೆ].w_int;

      ELEM value;
      thread_peek(thr, value);

      vvp_object_t&ತುದಿ = thr->peek_object();
      vvp_darray*darray = ತುದಿ.peek<vvp_darray>();
      ಪ್ರತಿಜ್ಞಾ(darray);

      darray->ಗಣ_ಶಬ್ದ(adr, value);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %set/dar/obj/real <index>
 */
ತಾರ್ಕಿಕ ದ_ಗಣ_DAR_OBJ_REAL(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ ಗಣ_dar_obj<ದ್ವಿಗುಣ>(thr, cp);
}

/*
 * %set/dar/obj/str <index>
 */
ತಾರ್ಕಿಕ ದ_ಗಣ_DAR_OBJ_STR(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ ಗಣ_dar_obj<string>(thr, cp);
}

/*
 * %set/dar/obj/vec4 <index>
 */
ತಾರ್ಕಿಕ ದ_ಗಣ_DAR_OBJ_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ ಗಣ_dar_obj<vvp_vector4_t>(thr, cp);
}

/*
 * %shiftl <idx>
 *
 * Pop the operand, then push the result.
 */
ತಾರ್ಕಿಕ ದ_SHIFTL(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಪೂರ್ಣಾಂಕ use_ಸೂಚ್ಯಂಕ = cp->ಸಂಖ್ಯೆ;
      uint64_t ಸರಿಸು = thr->ಪದಗಳು[use_ಸೂಚ್ಯಂಕ].w_uint;

      vvp_vector4_t&val = thr->peek_vec4();
      ಚಿಹ್ನೆರಹಿತ wid  = val.size();

      ಆದರೆ (thr->flags[4] == BIT4_1) {
	      // The result is 'bx if the shift amount is undefined
	    val = vvp_vector4_t(wid, BIT4_X);

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (thr->flags[4] == BIT4_X || ಸರಿಸು >= wid) {
	      // Shift is so big that all value is shifted out. Write
	      // a constant 0 result.
	    val = vvp_vector4_t(wid, BIT4_0);

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸರಿಸು > 0) {
	    vvp_vector4_t blk = val.subvalue(0, wid-ಸರಿಸು);
	    vvp_vector4_t tmp (ಸರಿಸು, BIT4_0);
	    val.ಗಣ_vec(0, tmp);
	    val.ಗಣ_vec(ಸರಿಸು, blk);
      }

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %shiftr <idx>
 * This is an unsigned right shift. The <idx> is a number that selects
 * the index register with the amount of the shift. This instruction
 * checks flag bit 4, which will be true if the shift is invalid.
 */
ತಾರ್ಕಿಕ ದ_SHIFTR(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಪೂರ್ಣಾಂಕ use_ಸೂಚ್ಯಂಕ = cp->ಸಂಖ್ಯೆ;
      uint64_t ಸರಿಸು = thr->ಪದಗಳು[use_ಸೂಚ್ಯಂಕ].w_uint;

      vvp_vector4_t val = thr->ತೆಗೆ_vec4();
      ಚಿಹ್ನೆರಹಿತ wid  = val.size();

      ಆದರೆ (thr->flags[4] == BIT4_1) {
	    val = vvp_vector4_t(wid, BIT4_X);

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (thr->flags[4] == BIT4_X || ಸರಿಸು > wid) {
	    val = vvp_vector4_t(wid, BIT4_0);

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸರಿಸು > 0) {
	    vvp_vector4_t blk = val.subvalue(ಸರಿಸು, wid-ಸರಿಸು);
	    vvp_vector4_t tmp (ಸರಿಸು, BIT4_0);
	    val.ಗಣ_vec(0, blk);
	    val.ಗಣ_vec(wid-ಸರಿಸು, tmp);
      }

      thr->ತಳ್ಳು_vec4(val);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 *  %shiftr/s <wid>
 */
ತಾರ್ಕಿಕ ದ_SHIFTR_S(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಪೂರ್ಣಾಂಕ use_ಸೂಚ್ಯಂಕ = cp->ಸಂಖ್ಯೆ;
      uint64_t ಸರಿಸು = thr->ಪದಗಳು[use_ಸೂಚ್ಯಂಕ].w_uint;

      vvp_vector4_t val = thr->ತೆಗೆ_vec4();
      ಚಿಹ್ನೆರಹಿತ wid  = val.size();

      vvp_bit4_t sign_ಬಿಟ್ = val.value(val.size()-1);

      ಆದರೆ (thr->flags[4] == BIT4_1) {
	    val = vvp_vector4_t(wid, BIT4_X);

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (thr->flags[4] == BIT4_X || ಸರಿಸು > wid) {
	    val = vvp_vector4_t(wid, sign_ಬಿಟ್);

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸರಿಸು > 0) {
	    vvp_vector4_t blk = val.subvalue(ಸರಿಸು, wid-ಸರಿಸು);
	    vvp_vector4_t tmp (ಸರಿಸು, sign_ಬಿಟ್);
	    val.ಗಣ_vec(0, blk);
	    val.ಗಣ_vec(wid-ಸರಿಸು, tmp);
      }

      thr->ತಳ್ಳು_vec4(val);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %split/vec4 <wid>
 *   Pop 1 value,
 *   Take <wid> bits from the lsb,
 *   Push the remaining msb,
 *   Push the lsb.
 */
ತಾರ್ಕಿಕ ದ_ವಿಭಜಿಸು_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ lsb_wid = cp->ಸಂಖ್ಯೆ;

      vvp_vector4_t&val = thr->peek_vec4();
      ಪ್ರತಿಜ್ಞಾ(lsb_wid < val.size());

      vvp_vector4_t lsb = val.subvalue(0, lsb_wid);
      val = val.subvalue(lsb_wid, val.size()-lsb_wid);

      thr->ತಳ್ಳು_vec4(lsb);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * The following are used to allow the darray templates to print correctly.
 */
ಸಾಲೊಳಗೆ ಸ್ಥಿರ string get_darray_type(ದ್ವಿಗುಣ&)
{
      ಹಿಂತಿರುಗಿ "darray<real>";
}

ಸಾಲೊಳಗೆ ಸ್ಥಿರ string get_darray_type(string&)
{
      ಹಿಂತಿರುಗಿ "darray<string>";
}

ಸಾಲೊಳಗೆ ಸ್ಥಿರ string get_darray_type(ಸ್ಥಿರಾಂಕ vvp_vector4_t&value)
{
      ostringstream buf;
      buf << "darray<vector[" << value.size() << "]>";
      string res = buf.str();
      ಹಿಂತಿರುಗಿ res;
}

/*
 * The following are used to allow a common template to be written for
 * darray real/string/vec4 operations
 */
ಸಾಲೊಳಗೆ ಸ್ಥಿರ ಶೂನ್ಯ dar_ತೆಗೆ_ಮೌಲ್ಯ(vthread_t thr, ದ್ವಿಗುಣ&value)
{
      value = thr->ತೆಗೆ_real();
}

ಸಾಲೊಳಗೆ ಸ್ಥಿರ ಶೂನ್ಯ dar_ತೆಗೆ_ಮೌಲ್ಯ(vthread_t thr, string&value)
{
      value = thr->ತೆಗೆ_str();
}

ಸಾಲೊಳಗೆ ಸ್ಥಿರ ಶೂನ್ಯ dar_ತೆಗೆ_ಮೌಲ್ಯ(vthread_t thr, vvp_vector4_t&value)
{
      value = thr->ತೆಗೆ_vec4();
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು ELEM>
ಸ್ಥಿರ ತಾರ್ಕಿಕ store_dar(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      int64_t adr = thr->ಪದಗಳು[3].w_int;
      ELEM value;
	// FIXME: Can we get the size of the underlying array element
	//        and then use the normal pop_value?
      dar_ತೆಗೆ_ಮೌಲ್ಯ(thr, value);

      vvp_net_t*net = cp->net;
      ಪ್ರತಿಜ್ಞಾ(net);

      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      ಪ್ರತಿಜ್ಞಾ(obj);

      vvp_darray*darray = obj->get_object().peek<vvp_darray>();

      ಆದರೆ (adr < 0)
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to a negative " << get_darray_type(value)
	         << " index (" << adr << ")." << ಸಾಲು_ಅಂತ್ಯ;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (thr->flags[4] != BIT4_0)
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to an undefined " << get_darray_type(value)
	         << " index." << ಸಾಲು_ಅಂತ್ಯ;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (darray)
	    darray->ಗಣ_ಶಬ್ದ(adr, value);
      ಇಲ್ಲದಿದ್ದರೆ
	    cerr << thr->get_fileline()
	         << "Warning: cannot write to an undefined " << get_darray_type(value)
	         << "." << ಸಾಲು_ಅಂತ್ಯ;

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %store/dar/real <var>
 */
ತಾರ್ಕಿಕ ದ_STORE_DAR_R(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ store_dar<ದ್ವಿಗುಣ>(thr, cp);
}

/*
 * %store/dar/str <var>
 */
ತಾರ್ಕಿಕ ದ_STORE_DAR_STR(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ store_dar<string>(thr, cp);
}

/*
 * %store/dar/vec4 <var>
 */
ತಾರ್ಕಿಕ ದ_STORE_DAR_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ store_dar<vvp_vector4_t>(thr, cp);
}

ತಾರ್ಕಿಕ ದ_STORE_OBJ(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);

      vvp_object_t val;
      thr->ತೆಗೆ_object(val);

      vvp_send_object(ptr, val, thr->wt_context);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %store/obja <array-label> <index>
 */
ತಾರ್ಕಿಕ ದ_STORE_OBJA(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ idx = cp->ಬಿಟ್_idx[0];
      ಚಿಹ್ನೆರಹಿತ adr = thr->ಪದಗಳು[idx].w_int;

      vvp_object_t val;
      thr->ತೆಗೆ_object(val);

      cp->array->ಗಣ_ಶಬ್ದ(adr, val);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}


/*
 * %store/prop/obj <pid>, <idx>
 *
 * Pop an object value from the object stack, and store the value into
 * the property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
ತಾರ್ಕಿಕ ದ_STORE_PROP_OBJ(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      size_t pid = cp->ಸಂಖ್ಯೆ;
      ಚಿಹ್ನೆರಹಿತ idx = cp->ಬಿಟ್_idx[0];

      ಆದರೆ (idx != 0) {
	    ಪ್ರತಿಜ್ಞಾ(idx < vthread_s::ಪದಗಳು_ಎಣಿಕೆ);
	    idx = thr->ಪದಗಳು[idx].w_uint;
      }

      vvp_object_t val;
      thr->ತೆಗೆ_object(val);

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      ಪ್ರತಿಜ್ಞಾ(cobj);

      cobj->ಗಣ_object(pid, val, idx);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಸ್ಥಿರ ಶೂನ್ಯ ತೆಗೆ_prop_val(vthread_t thr, ದ್ವಿಗುಣ&val, ಚಿಹ್ನೆರಹಿತ)
{
      val = thr->ತೆಗೆ_real();
}

ಸ್ಥಿರ ಶೂನ್ಯ ತೆಗೆ_prop_val(vthread_t thr, string&val, ಚಿಹ್ನೆರಹಿತ)
{
      val = thr->ತೆಗೆ_str();
}

ಸ್ಥಿರ ಶೂನ್ಯ ತೆಗೆ_prop_val(vthread_t thr, vvp_vector4_t&val, ಚಿಹ್ನೆರಹಿತ wid)
{
      val = thr->ತೆಗೆ_vec4();
      ಪ್ರತಿಜ್ಞಾ(val.size() >= wid);
      val.resize(wid);
}

ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_val(vvp_cobject*cobj, size_t pid, ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ&val)
{
      cobj->ಗಣ_real(pid, val);
}

ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_val(vvp_cobject*cobj, size_t pid, ಸ್ಥಿರಾಂಕ string&val)
{
      cobj->ಗಣ_string(pid, val);
}

ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_val(vvp_cobject*cobj, size_t pid, ಸ್ಥಿರಾಂಕ vvp_vector4_t&val)
{
      cobj->ಗಣ_vec4(pid, val);
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು ELEM>
ಸ್ಥಿರ ತಾರ್ಕಿಕ store_prop(vthread_t thr, vvp_ಸಂಕೇತ_t cp, ಚಿಹ್ನೆರಹಿತ wid=0)
{
      size_t pid = cp->ಸಂಖ್ಯೆ;
      ELEM val;
      ತೆಗೆ_prop_val(thr, val, wid); // Pop the value to store.

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj = obj.peek<vvp_cobject>();
      ಪ್ರತಿಜ್ಞಾ(cobj);

      ಗಣ_val(cobj, pid, val);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %store/prop/r <id>
 *
 * Pop a real value from the real stack, and store the value into the
 * property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
ತಾರ್ಕಿಕ ದ_STORE_PROP_R(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ store_prop<ದ್ವಿಗುಣ>(thr, cp);
}

/*
 * %store/prop/str <id>
 *
 * Pop a string value from the string stack, and store the value into
 * the property of the object references by the top of the stack. Do NOT
 * pop the object stack.
 */
ತಾರ್ಕಿಕ ದ_STORE_PROP_STR(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ store_prop<string>(thr, cp);
}

/*
 * %store/prop/v <pid>, <wid>
 *
 * Store vector value into property <id> of cobject in the top of the
 * stack. Do NOT pop the object stack.
 */
ತಾರ್ಕಿಕ ದ_STORE_PROP_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ store_prop<vvp_vector4_t>(thr, cp, cp->ಬಿಟ್_idx[0]);
}

ಮಾದರಿ <ವರ್ಗ QTYPE>
ಸ್ಥಿರ QTYPE* get_ಸರತಿ_prop(vvp_cobject*cobj, size_t pid)
{
      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      QTYPE* ಸರತಿ = qobj.peek<QTYPE>();
      ಆದರೆ (ಸರತಿ == 0) {
	    ಸರತಿ = ಹೊಸ QTYPE;
	    vvp_object_t val(ಸರತಿ);
	    cobj->ಗಣ_object(pid, val, 0);
      }
      ಹಿಂತಿರುಗಿ ಸರತಿ;
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು ELEM, ವರ್ಗ QTYPE>
ಸ್ಥಿರ ತಾರ್ಕಿಕ store_prop_qb(vthread_t thr, vvp_ಸಂಕೇತ_t cp, ಚಿಹ್ನೆರಹಿತ wid)
{
      size_t pid = cp->ಸಂಖ್ಯೆ;
      ELEM value;
      ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ = thr->ಪದಗಳು[cp->ಬಿಟ್_idx[0]].w_uint;
      ತೆಗೆ_ಮೌಲ್ಯ(thr, value, wid);

      vvp_object_t& ತುದಿ = thr->peek_object();
      vvp_cobject*cobj = ತುದಿ.peek<vvp_cobject>();
      ಪ್ರತಿಜ್ಞಾ(cobj);

      QTYPE* ಸರತಿ = get_ಸರತಿ_prop<QTYPE>(cobj, pid);
      ಪ್ರತಿಜ್ಞಾ(ಸರತಿ);
      ಸರತಿ->push_back(value, max_ಗಾತ್ರ);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು ELEM, ವರ್ಗ QTYPE>
ಸ್ಥಿರ ತಾರ್ಕಿಕ store_prop_qf(vthread_t thr, vvp_ಸಂಕೇತ_t cp, ಚಿಹ್ನೆರಹಿತ wid)
{
      size_t pid = cp->ಸಂಖ್ಯೆ;
      ELEM value;
      ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ = thr->ಪದಗಳು[cp->ಬಿಟ್_idx[0]].w_uint;
      ತೆಗೆ_ಮೌಲ್ಯ(thr, value, wid);

      vvp_object_t& ತುದಿ = thr->peek_object();
      vvp_cobject*cobj = ತುದಿ.peek<vvp_cobject>();
      ಪ್ರತಿಜ್ಞಾ(cobj);

      QTYPE* ಸರತಿ = get_ಸರತಿ_prop<QTYPE>(cobj, pid);
      ಪ್ರತಿಜ್ಞಾ(ಸರತಿ);
      ಸರತಿ->push_front(value, max_ಗಾತ್ರ);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_STORE_PROP_QB_R(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ store_prop_qb<ದ್ವಿಗುಣ, vvp_ಸರತಿ_real>(thr, cp, 0);
}

ತಾರ್ಕಿಕ ದ_STORE_PROP_QB_STR(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ store_prop_qb<string, vvp_ಸರತಿ_string>(thr, cp, 0);
}

ತಾರ್ಕಿಕ ದ_STORE_PROP_QB_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ store_prop_qb<vvp_vector4_t, vvp_ಸರತಿ_vec4>(thr, cp, cp->ಬಿಟ್_idx[1]);
}

ತಾರ್ಕಿಕ ದ_STORE_PROP_QF_R(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ store_prop_qf<ದ್ವಿಗುಣ, vvp_ಸರತಿ_real>(thr, cp, 0);
}

ತಾರ್ಕಿಕ ದ_STORE_PROP_QF_STR(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ store_prop_qf<string, vvp_ಸರತಿ_string>(thr, cp, 0);
}

ತಾರ್ಕಿಕ ದ_STORE_PROP_QF_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ store_prop_qf<vvp_vector4_t, vvp_ಸರತಿ_vec4>(thr, cp, cp->ಬಿಟ್_idx[1]);
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು ELEM, ವರ್ಗ QTYPE>
ಸ್ಥಿರ ತಾರ್ಕಿಕ qinsert_prop(vthread_t thr, vvp_ಸಂಕೇತ_t cp, ಚಿಹ್ನೆರಹಿತ wid)
{
      int64_t idx = thr->ಪದಗಳು[3].w_int;
      ELEM value;
      size_t pid = cp->ಸಂಖ್ಯೆ;
      ತೆಗೆ_ಮೌಲ್ಯ(thr, value, wid);

      vvp_object_t& ತುದಿ = thr->peek_object();
      vvp_cobject*cobj = ತುದಿ.peek<vvp_cobject>();
      ಪ್ರತಿಜ್ಞಾ(cobj);

      QTYPE* ಸರತಿ = get_ಸರತಿ_prop<QTYPE>(cobj, pid);
      ಪ್ರತಿಜ್ಞಾ(ಸರತಿ);
      ಆದರೆ (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at a negative "
	         << get_ಸರತಿ_type(value)
	         << " index (" << idx << "). ";
	    print_ಸರತಿ_ಮೌಲ್ಯ(value);
	    cerr << " was not added." << ಸಾಲು_ಅಂತ್ಯ;
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot insert at an undefined "
	         << get_ಸರತಿ_type(value) << " index. ";
	    print_ಸರತಿ_ಮೌಲ್ಯ(value);
	    cerr << " was not added." << ಸಾಲು_ಅಂತ್ಯ;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ = thr->ಪದಗಳು[cp->ಬಿಟ್_idx[0]].w_int;
	    ಸರತಿ->insert(idx, value, max_ಗಾತ್ರ);
      }
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_QINSERT_PROP_R(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ qinsert_prop<ದ್ವಿಗುಣ, vvp_ಸರತಿ_real>(thr, cp, 0);
}

ತಾರ್ಕಿಕ ದ_QINSERT_PROP_STR(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ qinsert_prop<string, vvp_ಸರತಿ_string>(thr, cp, 0);
}

ತಾರ್ಕಿಕ ದ_QINSERT_PROP_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ qinsert_prop<vvp_vector4_t, vvp_ಸರತಿ_vec4>(thr, cp, cp->ಬಿಟ್_idx[1]);
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು ELEM, ವರ್ಗ QTYPE>
ಸ್ಥಿರ ತಾರ್ಕಿಕ q_ತೆಗೆ_prop(vthread_t thr, vvp_ಸಂಕೇತ_t cp,
                       ಶೂನ್ಯ (*get_val_func)(vvp_ಸರತಿ*, ELEM&),
                       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*loc, ಚಿಹ್ನೆರಹಿತ wid)
{
      size_t pid = cp->ಸಂಖ್ಯೆ;

      vvp_object_t& ತುದಿ = thr->peek_object();
      vvp_cobject*cobj = ತುದಿ.peek<vvp_cobject>();
      ಪ್ರತಿಜ್ಞಾ(cobj);

      QTYPE* ಸರತಿ = get_ಸರತಿ_prop<QTYPE>(cobj, pid);
      ಪ್ರತಿಜ್ಞಾ(ಸರತಿ);

      size_t size = ಸರತಿ->get_ಗಾತ್ರ();

      ELEM value;
      ಆದರೆ (size) {
	    get_val_func(ಸರತಿ, value);
      } ಇಲ್ಲದಿದ್ದರೆ {
	    dq_default(value, wid);
	    cerr << thr->get_fileline()
	         << "Warning: pop_" << loc << "() on empty "
	         << get_ಸರತಿ_type(value) << "." << ಸಾಲು_ಅಂತ್ಯ;
      }

      ತಳ್ಳು_ಮೌಲ್ಯ(thr, value, wid);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು ELEM, ವರ್ಗ QTYPE>
ಸ್ಥಿರ ತಾರ್ಕಿಕ qpop_b_prop(vthread_t thr, vvp_ಸಂಕೇತ_t cp, ಚಿಹ್ನೆರಹಿತ wid)
{
      ಹಿಂತಿರುಗಿ q_ತೆಗೆ_prop<ELEM, QTYPE>(thr, cp, get_ಹಿಂಭಾಗ_ಮೌಲ್ಯ<ELEM>, "back", wid);
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು ELEM, ವರ್ಗ QTYPE>
ಸ್ಥಿರ ತಾರ್ಕಿಕ qpop_f_prop(vthread_t thr, vvp_ಸಂಕೇತ_t cp, ಚಿಹ್ನೆರಹಿತ wid)
{
      ಹಿಂತಿರುಗಿ q_ತೆಗೆ_prop<ELEM, QTYPE>(thr, cp, get_front_ಮೌಲ್ಯ<ELEM>, "front", wid);
}

ತಾರ್ಕಿಕ ದ_QPOP_PROP_B_REAL(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ qpop_b_prop<ದ್ವಿಗುಣ, vvp_ಸರತಿ_real>(thr, cp, 0);
}

ತಾರ್ಕಿಕ ದ_QPOP_PROP_B_STR(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ qpop_b_prop<string, vvp_ಸರತಿ_string>(thr, cp, 0);
}

ತಾರ್ಕಿಕ ದ_QPOP_PROP_B_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ qpop_b_prop<vvp_vector4_t, vvp_ಸರತಿ_vec4>(thr, cp, cp->ಬಿಟ್_idx[0]);
}

ತಾರ್ಕಿಕ ದ_QPOP_PROP_F_REAL(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ qpop_f_prop<ದ್ವಿಗುಣ, vvp_ಸರತಿ_real>(thr, cp, 0);
}

ತಾರ್ಕಿಕ ದ_QPOP_PROP_F_STR(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ qpop_f_prop<string, vvp_ಸರತಿ_string>(thr, cp, 0);
}

ತಾರ್ಕಿಕ ದ_QPOP_PROP_F_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ qpop_f_prop<vvp_vector4_t, vvp_ಸರತಿ_vec4>(thr, cp, cp->ಬಿಟ್_idx[0]);
}

ಸ್ಥಿರ vvp_vector4_t ಸರತಿ_unique_ulong_ಗಮ್ಯ_vec4(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul, ಚಿಹ್ನೆರಹಿತ ಬಿಟ್ಟುಗಳು)
{
      vvp_vector4_t val(ಬಿಟ್ಟುಗಳು, BIT4_0);
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0; idx < ಬಿಟ್ಟುಗಳು && idx < 8 * ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ); idx += 1) {
	    ಆದರೆ (ul & 1UL) {
		  val.ಗಣ_ಬಿಟ್(idx, BIT4_1);
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  val.ಗಣ_ಬಿಟ್(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      ಹಿಂತಿರುಗಿ val;
}

ಮಾದರಿ <ವರ್ಗ SRC> ಸ್ಥಿರ vvp_ಸರತಿ_vec4* ಸರತಿ_ಸತತ_unique_src(SRC* src,
								 ಚಿಹ್ನೆರಹಿತ wid,
								 ತಾರ್ಕಿಕ as_ಸೂಚ್ಯಂಕ)
{
      vvp_ಸರತಿ_vec4* dst = ಹೊಸ vvp_ಸರತಿ_vec4();
      ಆದರೆ (!src) ಹಿಂತಿರುಗಿ dst;
      size_t n = src->get_ಗಾತ್ರ();
      ಗಾಗಿ (size_t i = 0; i < n; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_ಶಬ್ದ(i, vi);
	    ತಾರ್ಕಿಕ ನೋಡಿದ = ಸುಳ್ಳು;
	    ಗಾಗಿ (size_t j = 0; j < i; j += 1) {
		  vvp_vector4_t vj(wid);
		  src->get_ಶಬ್ದ(j, vj);
		  ಆದರೆ (vi.eeq(vj)) {
			ನೋಡಿದ = ನಿಜ;
			ನಿಲ್ಲಿಸು;
		  }
	    }
	    ಆದರೆ (ನೋಡಿದ) ಮುಂದುವರಿ;
	    ಆದರೆ (as_ಸೂಚ್ಯಂಕ) {
		  dst->push_back(ಸರತಿ_unique_ulong_ಗಮ್ಯ_vec4((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)i, 32), 0);
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  dst->push_back(vi, 0);
	    }
      }
      ಹಿಂತಿರುಗಿ dst;
}

ಮಾದರಿ <ವರ್ಗ SRC>
ಸ್ಥಿರ vvp_ಸರತಿ_vec4* ಸರತಿ_ಸತತ_ಕಂಡುಹಿಡಿ_matches_src(SRC* src, ಚಿಹ್ನೆರಹಿತ wid,
						  ಸ್ಥಿರಾಂಕ vvp_vector4_t& cmp,
						  ತಾರ್ಕಿಕ as_ಸೂಚ್ಯಂಕ)
{
      vvp_ಸರತಿ_vec4* dst = ಹೊಸ vvp_ಸರತಿ_vec4();
      ಆದರೆ (!src) ಹಿಂತಿರುಗಿ dst;
      size_t n = src->get_ಗಾತ್ರ();
      ಗಾಗಿ (size_t i = 0; i < n; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_ಶಬ್ದ(i, vi);
	    ಆದರೆ (vi.eeq(cmp)) {
		  ಆದರೆ (as_ಸೂಚ್ಯಂಕ) {
			dst->push_back(ಸರತಿ_unique_ulong_ಗಮ್ಯ_vec4((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)i, 32), 0);
		  } ಇಲ್ಲದಿದ್ದರೆ {
			dst->push_back(vi, 0);
		  }
	    }
      }
      ಹಿಂತಿರುಗಿ dst;
}

/* Array/queue locator methods return a queue with 0 or 1 elements (LRM). */
ಮಾದರಿ <ವರ್ಗ SRC>
ಸ್ಥಿರ vvp_ಸರತಿ_vec4* ಸರತಿ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_ಕೊನೆಯ_ಗಮ್ಯ_ಸರತಿ_src(SRC* src,
							  ಚಿಹ್ನೆರಹಿತ wid,
							  ಸ್ಥಿರಾಂಕ vvp_vector4_t& cmp,
							  ತಾರ್ಕಿಕ want_ಮೊದಲ,
							  ತಾರ್ಕಿಕ want_ಸೂಚ್ಯಂಕ)
{
      vvp_ಸರತಿ_vec4* dst = ಹೊಸ vvp_ಸರತಿ_vec4();
      ಆದರೆ (!src || src->get_ಗಾತ್ರ() == 0) ಹಿಂತಿರುಗಿ dst;
      size_t n = src->get_ಗಾತ್ರ();
      ಆದರೆ (want_ಮೊದಲ) {
	    ಗಾಗಿ (size_t i = 0; i < n; i += 1) {
		  vvp_vector4_t vi(wid);
		  src->get_ಶಬ್ದ(i, vi);
		  ಆದರೆ (vi.eeq(cmp)) {
			ಆದರೆ (want_ಸೂಚ್ಯಂಕ) {
			      dst->push_back(
				  ಸರತಿ_unique_ulong_ಗಮ್ಯ_vec4((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)i, 32),
				  0);
			} ಇಲ್ಲದಿದ್ದರೆ {
			      dst->push_back(vi, 0);
			}
			ಹಿಂತಿರುಗಿ dst;
		  }
	    }
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಗಾಗಿ (size_t i = n; i > 0; ) {
		  i -= 1;
		  vvp_vector4_t vi(wid);
		  src->get_ಶಬ್ದ(i, vi);
		  ಆದರೆ (vi.eeq(cmp)) {
			ಆದರೆ (want_ಸೂಚ್ಯಂಕ) {
			      dst->push_back(
				  ಸರತಿ_unique_ulong_ಗಮ್ಯ_vec4((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)i, 32),
				  0);
			} ಇಲ್ಲದಿದ್ದರೆ {
			      dst->push_back(vi, 0);
			}
			ಹಿಂತಿರುಗಿ dst;
		  }
	    }
      }
      ಹಿಂತಿರುಗಿ dst;
}

/* min()/max() return one element with the selected extremum. */
ಮಾದರಿ <ವರ್ಗ SRC>
ಸ್ಥಿರ vvp_ಸರತಿ_vec4* ಸರತಿ_ಸತತ_min_max_src(SRC* src, ಚಿಹ್ನೆರಹಿತ wid, ತಾರ್ಕಿಕ want_max)
{
      vvp_ಸರತಿ_vec4* dst = ಹೊಸ vvp_ಸರತಿ_vec4();
      ಆದರೆ (!src || src->get_ಗಾತ್ರ() == 0) ಹಿಂತಿರುಗಿ dst;

      vvp_vector4_t ಉತ್ತಮ(wid);
      src->get_ಶಬ್ದ(0, ಉತ್ತಮ);

      size_t n = src->get_ಗಾತ್ರ();
      ಗಾಗಿ (size_t i = 1 ; i < n ; i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_ಶಬ್ದ(i, vi);

	    vvp_bit4_t ge = compare_gtge(vi, ಉತ್ತಮ, BIT4_1);
	    ತಾರ್ಕಿಕ replace = want_max ? (ge == BIT4_1) : (ge == BIT4_0);
	    ಆದರೆ (replace) ಉತ್ತಮ = vi;
      }

      dst->push_back(ಉತ್ತಮ, 0);
      ಹಿಂತಿರುಗಿ dst;
}

ತಾರ್ಕಿಕ ದ_ಸರತಿ_ಕಂಡುಹಿಡಿ_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_vector4_t cmp = thr->ತೆಗೆ_vec4();
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಬಿಟ್_idx[0];
      ಪ್ರತಿಜ್ಞಾ(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_ಸರತಿ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ಸರತಿ_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_ಸರತಿ_vec4* dst;
      ಆದರೆ (qsrc) dst = ಸರತಿ_ಸತತ_ಕಂಡುಹಿಡಿ_matches_src(qsrc, wid, cmp, ಸುಳ್ಳು);
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dsrc) dst = ಸರತಿ_ಸತತ_ಕಂಡುಹಿಡಿ_matches_src(dsrc, wid, cmp, ಸುಳ್ಳು);
      ಇಲ್ಲದಿದ್ದರೆ {
	    vvp_ಸರತಿ* src_q = get_ಸರತಿ_object<vvp_ಸರತಿ_vec4>(thr, net);
	    vvp_ಸರತಿ_vec4* src = dynamic_cast<vvp_ಸರತಿ_vec4*>(src_q);
	    dst = ಸರತಿ_ಸತತ_ಕಂಡುಹಿಡಿ_matches_src(src, wid, cmp, ಸುಳ್ಳು);
      }
      thr->ತಳ್ಳು_object(vvp_object_t(dst));
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_ಸರತಿ_ಕಂಡುಹಿಡಿ_PROP_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_vector4_t cmp = thr->ತೆಗೆ_vec4();
      size_t pid = cp->ಸಂಖ್ಯೆ;
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಬಿಟ್_idx[0];
      ಪ್ರತಿಜ್ಞಾ(cmp.size() == wid);

      vvp_object_t& ತುದಿ = thr->peek_object();
      vvp_cobject*cobj = ತುದಿ.peek<vvp_cobject>();
      ಪ್ರತಿಜ್ಞಾ(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_ಸರತಿ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ಸರತಿ_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_ಸರತಿ_vec4* dst = qsrc ? ಸರತಿ_ಸತತ_ಕಂಡುಹಿಡಿ_matches_src(qsrc, wid, cmp, ಸುಳ್ಳು)
				 : ಸರತಿ_ಸತತ_ಕಂಡುಹಿಡಿ_matches_src(dsrc, wid, cmp, ಸುಳ್ಳು);
      thr->ತಳ್ಳು_object(vvp_object_t(dst));
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_ಸರತಿ_ಕಂಡುಹಿಡಿ_ಸೂಚ್ಯಂಕ_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_vector4_t cmp = thr->ತೆಗೆ_vec4();
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಬಿಟ್_idx[0];
      ಪ್ರತಿಜ್ಞಾ(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_ಸರತಿ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ಸರತಿ_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_ಸರತಿ_vec4* dst;
      ಆದರೆ (qsrc) dst = ಸರತಿ_ಸತತ_ಕಂಡುಹಿಡಿ_matches_src(qsrc, wid, cmp, ನಿಜ);
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dsrc) dst = ಸರತಿ_ಸತತ_ಕಂಡುಹಿಡಿ_matches_src(dsrc, wid, cmp, ನಿಜ);
      ಇಲ್ಲದಿದ್ದರೆ {
	    vvp_ಸರತಿ* src_q = get_ಸರತಿ_object<vvp_ಸರತಿ_vec4>(thr, net);
	    vvp_ಸರತಿ_vec4* src = dynamic_cast<vvp_ಸರತಿ_vec4*>(src_q);
	    dst = ಸರತಿ_ಸತತ_ಕಂಡುಹಿಡಿ_matches_src(src, wid, cmp, ನಿಜ);
      }
      thr->ತಳ್ಳು_object(vvp_object_t(dst));
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_ಸರತಿ_ಕಂಡುಹಿಡಿ_ಸೂಚ್ಯಂಕ_PROP_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_vector4_t cmp = thr->ತೆಗೆ_vec4();
      size_t pid = cp->ಸಂಖ್ಯೆ;
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಬಿಟ್_idx[0];
      ಪ್ರತಿಜ್ಞಾ(cmp.size() == wid);

      vvp_object_t& ತುದಿ = thr->peek_object();
      vvp_cobject*cobj = ತುದಿ.peek<vvp_cobject>();
      ಪ್ರತಿಜ್ಞಾ(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_ಸರತಿ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ಸರತಿ_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_ಸರತಿ_vec4* dst = qsrc ? ಸರತಿ_ಸತತ_ಕಂಡುಹಿಡಿ_matches_src(qsrc, wid, cmp, ನಿಜ)
				 : ಸರತಿ_ಸತತ_ಕಂಡುಹಿಡಿ_matches_src(dsrc, wid, cmp, ನಿಜ);
      thr->ತಳ್ಳು_object(vvp_object_t(dst));
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_ಸರತಿ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_vector4_t cmp = thr->ತೆಗೆ_vec4();
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಬಿಟ್_idx[0];
      ಪ್ರತಿಜ್ಞಾ(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_ಸರತಿ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ಸರತಿ_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_ಸರತಿ_vec4* dst;
      ಆದರೆ (qsrc) dst = ಸರತಿ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_ಕೊನೆಯ_ಗಮ್ಯ_ಸರತಿ_src(qsrc, wid, cmp, ನಿಜ, ಸುಳ್ಳು);
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dsrc) dst = ಸರತಿ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_ಕೊನೆಯ_ಗಮ್ಯ_ಸರತಿ_src(dsrc, wid, cmp, ನಿಜ, ಸುಳ್ಳು);
      ಇಲ್ಲದಿದ್ದರೆ {
	    vvp_ಸರತಿ* src_q = get_ಸರತಿ_object<vvp_ಸರತಿ_vec4>(thr, net);
	    vvp_ಸರತಿ_vec4* src = dynamic_cast<vvp_ಸರತಿ_vec4*>(src_q);
	    dst = ಸರತಿ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_ಕೊನೆಯ_ಗಮ್ಯ_ಸರತಿ_src(src, wid, cmp, ನಿಜ, ಸುಳ್ಳು);
      }
      thr->ತಳ್ಳು_object(vvp_object_t(dst));
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_ಸರತಿ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_PROP_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_vector4_t cmp = thr->ತೆಗೆ_vec4();
      size_t pid = cp->ಸಂಖ್ಯೆ;
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಬಿಟ್_idx[0];
      ಪ್ರತಿಜ್ಞಾ(cmp.size() == wid);

      vvp_object_t& ತುದಿ = thr->peek_object();
      vvp_cobject*cobj = ತುದಿ.peek<vvp_cobject>();
      ಪ್ರತಿಜ್ಞಾ(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_ಸರತಿ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ಸರತಿ_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_ಸರತಿ_vec4* dst =
	    qsrc ? ಸರತಿ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_ಕೊನೆಯ_ಗಮ್ಯ_ಸರತಿ_src(qsrc, wid, cmp, ನಿಜ, ಸುಳ್ಳು)
		 : ಸರತಿ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_ಕೊನೆಯ_ಗಮ್ಯ_ಸರತಿ_src(dsrc, wid, cmp, ನಿಜ, ಸುಳ್ಳು);
      thr->ತಳ್ಳು_object(vvp_object_t(dst));
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_ಸರತಿ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_ಸೂಚ್ಯಂಕ_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_vector4_t cmp = thr->ತೆಗೆ_vec4();
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಬಿಟ್_idx[0];
      ಪ್ರತಿಜ್ಞಾ(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_ಸರತಿ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ಸರತಿ_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_ಸರತಿ_vec4* dst;
      ಆದರೆ (qsrc) dst = ಸರತಿ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_ಕೊನೆಯ_ಗಮ್ಯ_ಸರತಿ_src(qsrc, wid, cmp, ನಿಜ, ನಿಜ);
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dsrc) dst = ಸರತಿ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_ಕೊನೆಯ_ಗಮ್ಯ_ಸರತಿ_src(dsrc, wid, cmp, ನಿಜ, ನಿಜ);
      ಇಲ್ಲದಿದ್ದರೆ {
	    vvp_ಸರತಿ* src_q = get_ಸರತಿ_object<vvp_ಸರತಿ_vec4>(thr, net);
	    vvp_ಸರತಿ_vec4* src = dynamic_cast<vvp_ಸರತಿ_vec4*>(src_q);
	    dst = ಸರತಿ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_ಕೊನೆಯ_ಗಮ್ಯ_ಸರತಿ_src(src, wid, cmp, ನಿಜ, ನಿಜ);
      }
      thr->ತಳ್ಳು_object(vvp_object_t(dst));
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_ಸರತಿ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_ಸೂಚ್ಯಂಕ_PROP_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_vector4_t cmp = thr->ತೆಗೆ_vec4();
      size_t pid = cp->ಸಂಖ್ಯೆ;
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಬಿಟ್_idx[0];
      ಪ್ರತಿಜ್ಞಾ(cmp.size() == wid);

      vvp_object_t& ತುದಿ = thr->peek_object();
      vvp_cobject*cobj = ತುದಿ.peek<vvp_cobject>();
      ಪ್ರತಿಜ್ಞಾ(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_ಸರತಿ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ಸರತಿ_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_ಸರತಿ_vec4* dst =
	    qsrc ? ಸರತಿ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_ಕೊನೆಯ_ಗಮ್ಯ_ಸರತಿ_src(qsrc, wid, cmp, ನಿಜ, ನಿಜ)
		 : ಸರತಿ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_ಕೊನೆಯ_ಗಮ್ಯ_ಸರತಿ_src(dsrc, wid, cmp, ನಿಜ, ನಿಜ);
      thr->ತಳ್ಳು_object(vvp_object_t(dst));
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_ಸರತಿ_ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_vector4_t cmp = thr->ತೆಗೆ_vec4();
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಬಿಟ್_idx[0];
      ಪ್ರತಿಜ್ಞಾ(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_ಸರತಿ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ಸರತಿ_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_ಸರತಿ_vec4* dst;
      ಆದರೆ (qsrc) dst = ಸರತಿ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_ಕೊನೆಯ_ಗಮ್ಯ_ಸರತಿ_src(qsrc, wid, cmp, ಸುಳ್ಳು, ಸುಳ್ಳು);
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dsrc) dst = ಸರತಿ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_ಕೊನೆಯ_ಗಮ್ಯ_ಸರತಿ_src(dsrc, wid, cmp, ಸುಳ್ಳು, ಸುಳ್ಳು);
      ಇಲ್ಲದಿದ್ದರೆ {
	    vvp_ಸರತಿ* src_q = get_ಸರತಿ_object<vvp_ಸರತಿ_vec4>(thr, net);
	    vvp_ಸರತಿ_vec4* src = dynamic_cast<vvp_ಸರತಿ_vec4*>(src_q);
	    dst = ಸರತಿ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_ಕೊನೆಯ_ಗಮ್ಯ_ಸರತಿ_src(src, wid, cmp, ಸುಳ್ಳು, ಸುಳ್ಳು);
      }
      thr->ತಳ್ಳು_object(vvp_object_t(dst));
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_ಸರತಿ_ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_PROP_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_vector4_t cmp = thr->ತೆಗೆ_vec4();
      size_t pid = cp->ಸಂಖ್ಯೆ;
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಬಿಟ್_idx[0];
      ಪ್ರತಿಜ್ಞಾ(cmp.size() == wid);

      vvp_object_t& ತುದಿ = thr->peek_object();
      vvp_cobject*cobj = ತುದಿ.peek<vvp_cobject>();
      ಪ್ರತಿಜ್ಞಾ(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_ಸರತಿ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ಸರತಿ_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_ಸರತಿ_vec4* dst =
	    qsrc ? ಸರತಿ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_ಕೊನೆಯ_ಗಮ್ಯ_ಸರತಿ_src(qsrc, wid, cmp, ಸುಳ್ಳು, ಸುಳ್ಳು)
		 : ಸರತಿ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_ಕೊನೆಯ_ಗಮ್ಯ_ಸರತಿ_src(dsrc, wid, cmp, ಸುಳ್ಳು, ಸುಳ್ಳು);
      thr->ತಳ್ಳು_object(vvp_object_t(dst));
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_ಸರತಿ_ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_ಸೂಚ್ಯಂಕ_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_vector4_t cmp = thr->ತೆಗೆ_vec4();
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಬಿಟ್_idx[0];
      ಪ್ರತಿಜ್ಞಾ(cmp.size() == wid);
      vvp_net_t* net = cp->net;
      vvp_ಸರತಿ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ಸರತಿ_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_ಸರತಿ_vec4* dst;
      ಆದರೆ (qsrc) dst = ಸರತಿ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_ಕೊನೆಯ_ಗಮ್ಯ_ಸರತಿ_src(qsrc, wid, cmp, ಸುಳ್ಳು, ನಿಜ);
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dsrc) dst = ಸರತಿ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_ಕೊನೆಯ_ಗಮ್ಯ_ಸರತಿ_src(dsrc, wid, cmp, ಸುಳ್ಳು, ನಿಜ);
      ಇಲ್ಲದಿದ್ದರೆ {
	    vvp_ಸರತಿ* src_q = get_ಸರತಿ_object<vvp_ಸರತಿ_vec4>(thr, net);
	    vvp_ಸರತಿ_vec4* src = dynamic_cast<vvp_ಸರತಿ_vec4*>(src_q);
	    dst = ಸರತಿ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_ಕೊನೆಯ_ಗಮ್ಯ_ಸರತಿ_src(src, wid, cmp, ಸುಳ್ಳು, ನಿಜ);
      }
      thr->ತಳ್ಳು_object(vvp_object_t(dst));
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_ಸರತಿ_ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_ಸೂಚ್ಯಂಕ_PROP_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_vector4_t cmp = thr->ತೆಗೆ_vec4();
      size_t pid = cp->ಸಂಖ್ಯೆ;
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಬಿಟ್_idx[0];
      ಪ್ರತಿಜ್ಞಾ(cmp.size() == wid);

      vvp_object_t& ತುದಿ = thr->peek_object();
      vvp_cobject*cobj = ತುದಿ.peek<vvp_cobject>();
      ಪ್ರತಿಜ್ಞಾ(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_ಸರತಿ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ಸರತಿ_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_ಸರತಿ_vec4* dst =
	    qsrc ? ಸರತಿ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_ಕೊನೆಯ_ಗಮ್ಯ_ಸರತಿ_src(qsrc, wid, cmp, ಸುಳ್ಳು, ನಿಜ)
		 : ಸರತಿ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_ಕೊನೆಯ_ಗಮ್ಯ_ಸರತಿ_src(dsrc, wid, cmp, ಸುಳ್ಳು, ನಿಜ);
      thr->ತಳ್ಳು_object(vvp_object_t(dst));
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_ಸರತಿ_MIN_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_net_t* net = cp->net;
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಬಿಟ್_idx[0];
      vvp_ಸರತಿ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ಸರತಿ_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_ಸರತಿ_vec4* dst;
      ಆದರೆ (qsrc) dst = ಸರತಿ_ಸತತ_min_max_src(qsrc, wid, ಸುಳ್ಳು);
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dsrc) dst = ಸರತಿ_ಸತತ_min_max_src(dsrc, wid, ಸುಳ್ಳು);
      ಇಲ್ಲದಿದ್ದರೆ {
	    vvp_ಸರತಿ* src_q = get_ಸರತಿ_object<vvp_ಸರತಿ_vec4>(thr, net);
	    vvp_ಸರತಿ_vec4* src = dynamic_cast<vvp_ಸರತಿ_vec4*>(src_q);
	    dst = ಸರತಿ_ಸತತ_min_max_src(src, wid, ಸುಳ್ಳು);
      }
      thr->ತಳ್ಳು_object(vvp_object_t(dst));
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_ಸರತಿ_MIN_OBJ_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಬಿಟ್_idx[0];
      vvp_object_t src_obj;
      thr->ತೆಗೆ_object(src_obj);

      vvp_ಸರತಿ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ಸರತಿ_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_ಸರತಿ_vec4* dst = qsrc ? ಸರತಿ_ಸತತ_min_max_src(qsrc, wid, ಸುಳ್ಳು)
				 : ಸರತಿ_ಸತತ_min_max_src(dsrc, wid, ಸುಳ್ಳು);
      thr->ತಳ್ಳು_object(vvp_object_t(dst));
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_ಸರತಿ_MIN_PROP_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      size_t pid = cp->ಸಂಖ್ಯೆ;
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಬಿಟ್_idx[0];

      vvp_object_t& ತುದಿ = thr->peek_object();
      vvp_cobject*cobj = ತುದಿ.peek<vvp_cobject>();
      ಪ್ರತಿಜ್ಞಾ(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_ಸರತಿ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ಸರತಿ_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_ಸರತಿ_vec4* dst = qsrc ? ಸರತಿ_ಸತತ_min_max_src(qsrc, wid, ಸುಳ್ಳು)
				 : ಸರತಿ_ಸತತ_min_max_src(dsrc, wid, ಸುಳ್ಳು);
      thr->ತಳ್ಳು_object(vvp_object_t(dst));
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_ಸರತಿ_MAX_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_net_t* net = cp->net;
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಬಿಟ್_idx[0];
      vvp_ಸರತಿ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ಸರತಿ_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_ಸರತಿ_vec4* dst;
      ಆದರೆ (qsrc) dst = ಸರತಿ_ಸತತ_min_max_src(qsrc, wid, ನಿಜ);
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dsrc) dst = ಸರತಿ_ಸತತ_min_max_src(dsrc, wid, ನಿಜ);
      ಇಲ್ಲದಿದ್ದರೆ {
	    vvp_ಸರತಿ* src_q = get_ಸರತಿ_object<vvp_ಸರತಿ_vec4>(thr, net);
	    vvp_ಸರತಿ_vec4* src = dynamic_cast<vvp_ಸರತಿ_vec4*>(src_q);
	    dst = ಸರತಿ_ಸತತ_min_max_src(src, wid, ನಿಜ);
      }
      thr->ತಳ್ಳು_object(vvp_object_t(dst));
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_ಸರತಿ_MAX_OBJ_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಬಿಟ್_idx[0];
      vvp_object_t src_obj;
      thr->ತೆಗೆ_object(src_obj);

      vvp_ಸರತಿ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ಸರತಿ_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_ಸರತಿ_vec4* dst = qsrc ? ಸರತಿ_ಸತತ_min_max_src(qsrc, wid, ನಿಜ)
				 : ಸರತಿ_ಸತತ_min_max_src(dsrc, wid, ನಿಜ);
      thr->ತಳ್ಳು_object(vvp_object_t(dst));
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_ಸರತಿ_MAX_PROP_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      size_t pid = cp->ಸಂಖ್ಯೆ;
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಬಿಟ್_idx[0];

      vvp_object_t& ತುದಿ = thr->peek_object();
      vvp_cobject*cobj = ತುದಿ.peek<vvp_cobject>();
      ಪ್ರತಿಜ್ಞಾ(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_ಸರತಿ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ಸರತಿ_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_ಸರತಿ_vec4* dst = qsrc ? ಸರತಿ_ಸತತ_min_max_src(qsrc, wid, ನಿಜ)
				 : ಸರತಿ_ಸತತ_min_max_src(dsrc, wid, ನಿಜ);
      thr->ತಳ್ಳು_object(vvp_object_t(dst));
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_ಸರತಿ_UNIQUE_OBJ_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಬಿಟ್_idx[0];
      vvp_object_t src_obj;
      thr->ತೆಗೆ_object(src_obj);

      vvp_ಸರತಿ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ಸರತಿ_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_ಸರತಿ_vec4* dst = qsrc ? ಸರತಿ_ಸತತ_unique_src(qsrc, wid, ಸುಳ್ಳು)
				 : ಸರತಿ_ಸತತ_unique_src(dsrc, wid, ಸುಳ್ಳು);
      thr->ತಳ್ಳು_object(vvp_object_t(dst));
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_ಸರತಿ_UNIQUE_ಸೂಚ್ಯಂಕ_OBJ_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಬಿಟ್_idx[0];
      vvp_object_t src_obj;
      thr->ತೆಗೆ_object(src_obj);

      vvp_ಸರತಿ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ಸರತಿ_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_ಸರತಿ_vec4* dst = qsrc ? ಸರತಿ_ಸತತ_unique_src(qsrc, wid, ನಿಜ)
				 : ಸರತಿ_ಸತತ_unique_src(dsrc, wid, ನಿಜ);
      thr->ತಳ್ಳು_object(vvp_object_t(dst));
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_ಸರತಿ_UNIQUE_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_net_t*net = cp->net;
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಬಿಟ್_idx[0];
      vvp_ಸರತಿ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ಸರತಿ_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_ಸರತಿ_vec4* dst;
      ಆದರೆ (qsrc) dst = ಸರತಿ_ಸತತ_unique_src(qsrc, wid, ಸುಳ್ಳು);
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dsrc) dst = ಸರತಿ_ಸತತ_unique_src(dsrc, wid, ಸುಳ್ಳು);
      ಇಲ್ಲದಿದ್ದರೆ {
	    vvp_ಸರತಿ* src_q = get_ಸರತಿ_object<vvp_ಸರತಿ_vec4>(thr, net);
	    vvp_ಸರತಿ_vec4* src = dynamic_cast<vvp_ಸರತಿ_vec4*>(src_q);
	    dst = ಸರತಿ_ಸತತ_unique_src(src, wid, ಸುಳ್ಳು);
      }
      thr->ತಳ್ಳು_object(vvp_object_t(dst));
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_ಸರತಿ_UNIQUE_PROP_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      size_t pid = cp->ಸಂಖ್ಯೆ;
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಬಿಟ್_idx[0];

      vvp_object_t& ತುದಿ = thr->peek_object();
      vvp_cobject*cobj = ತುದಿ.peek<vvp_cobject>();
      ಪ್ರತಿಜ್ಞಾ(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_ಸರತಿ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ಸರತಿ_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_ಸರತಿ_vec4* dst = qsrc ? ಸರತಿ_ಸತತ_unique_src(qsrc, wid, ಸುಳ್ಳು)
				 : ಸರತಿ_ಸತತ_unique_src(dsrc, wid, ಸುಳ್ಳು);
      thr->ತಳ್ಳು_object(vvp_object_t(dst));
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_ಸರತಿ_UNIQUE_ಸೂಚ್ಯಂಕ_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_net_t*net = cp->net;
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಬಿಟ್_idx[0];
      vvp_ಸರತಿ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ಸರತಿ_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_ಸರತಿ_vec4* dst;
      ಆದರೆ (qsrc) dst = ಸರತಿ_ಸತತ_unique_src(qsrc, wid, ನಿಜ);
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dsrc) dst = ಸರತಿ_ಸತತ_unique_src(dsrc, wid, ನಿಜ);
      ಇಲ್ಲದಿದ್ದರೆ {
	    vvp_ಸರತಿ* src_q = get_ಸರತಿ_object<vvp_ಸರತಿ_vec4>(thr, net);
	    vvp_ಸರತಿ_vec4* src = dynamic_cast<vvp_ಸರತಿ_vec4*>(src_q);
	    dst = ಸರತಿ_ಸತತ_unique_src(src, wid, ನಿಜ);
      }
      thr->ತಳ್ಳು_object(vvp_object_t(dst));
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_ಸರತಿ_UNIQUE_ಸೂಚ್ಯಂಕ_PROP_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      size_t pid = cp->ಸಂಖ್ಯೆ;
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಬಿಟ್_idx[0];

      vvp_object_t& ತುದಿ = thr->peek_object();
      vvp_cobject*cobj = ತುದಿ.peek<vvp_cobject>();
      ಪ್ರತಿಜ್ಞಾ(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_ಸರತಿ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ಸರತಿ_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_ಸರತಿ_vec4* dst = qsrc ? ಸರತಿ_ಸತತ_unique_src(qsrc, wid, ನಿಜ)
				 : ಸರತಿ_ಸತತ_unique_src(dsrc, wid, ನಿಜ);
      thr->ತಳ್ಳು_object(vvp_object_t(dst));
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_PROP_ಸರತಿ_ಗಾತ್ರ(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      size_t pid = cp->ಸಂಖ್ಯೆ;

      vvp_object_t& ತುದಿ = thr->peek_object();
      vvp_cobject*cobj = ತುದಿ.peek<vvp_cobject>();
      ಪ್ರತಿಜ್ಞಾ(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_ಸರತಿ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ಸರತಿ_or_darray_vec4_from_object(qobj, qsrc, dsrc);

      size_t sz = qsrc ? qsrc->get_ಗಾತ್ರ() : (dsrc ? dsrc->get_ಗಾತ್ರ() : 0);

      vvp_vector4_t val(32, BIT4_0);
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul = sz;
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0; idx < 32; idx++) {
	    ಆದರೆ (ul & 1UL) {
		  val.ಗಣ_ಬಿಟ್(idx, BIT4_1);
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  val.ಗಣ_ಬಿಟ್(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      thr->ತಳ್ಳು_vec4(val);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_ಸರತಿ_ಗಾತ್ರ_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_ಸರತಿ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ಸರತಿ_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      size_t sz =
	    qsrc ? qsrc->get_ಗಾತ್ರ() : (dsrc ? dsrc->get_ಗಾತ್ರ() : 0);

      vvp_vector4_t val(32, BIT4_0);
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul = sz;
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0; idx < 32; idx++) {
	    ಆದರೆ (ul & 1UL) {
		  val.ಗಣ_ಬಿಟ್(idx, BIT4_1);
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  val.ಗಣ_ಬಿಟ್(idx, BIT4_0);
	    }
	    ul >>= 1;
      }
      thr->ತಳ್ಳು_vec4(val);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_ಸರತಿ_ಶಬ್ದ_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಬಿಟ್_idx[0];
      ಚಿಹ್ನೆರಹಿತ ix_reg = cp->ಬಿಟ್_idx[1];
      int64_t adr = thr->ಪದಗಳು[ix_reg].w_int;
      ಆದರೆ (thr->flags[4] == BIT4_1) {
	    thr->ತಳ್ಳು_vec4(vvp_vector4_t(wid));
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }
      ಆದರೆ (adr < 0) {
	    thr->ತಳ್ಳು_vec4(vvp_vector4_t(wid));
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }
      vvp_net_t* net = cp->net;
      vvp_ಸರತಿ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ಸರತಿ_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t out(wid);
      ಆದರೆ (qsrc) {
	    ಆದರೆ ((size_t)adr >= qsrc->get_ಗಾತ್ರ()) {
		  thr->ತಳ್ಳು_vec4(out);
		  ಹಿಂತಿರುಗಿ ನಿಜ;
	    }
	    qsrc->get_ಶಬ್ದ((ಚಿಹ್ನೆರಹಿತ)adr, out);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dsrc) {
	    ಆದರೆ ((size_t)adr >= dsrc->get_ಗಾತ್ರ()) {
		  thr->ತಳ್ಳು_vec4(out);
		  ಹಿಂತಿರುಗಿ ನಿಜ;
	    }
	    dsrc->get_ಶಬ್ದ((ಚಿಹ್ನೆರಹಿತ)adr, out);
      } ಇಲ್ಲದಿದ್ದರೆ {
	    vvp_ಸರತಿ* src_q = get_ಸರತಿ_object<vvp_ಸರತಿ_vec4>(thr, net);
	    vvp_ಸರತಿ_vec4* src = dynamic_cast<vvp_ಸರತಿ_vec4*>(src_q);
	    ಆದರೆ (!src || (size_t)adr >= src->get_ಗಾತ್ರ()) {
		  thr->ತಳ್ಳು_vec4(out);
		  ಹಿಂತಿರುಗಿ ನಿಜ;
	    }
	    src->get_ಶಬ್ದ((ಚಿಹ್ನೆರಹಿತ)adr, out);
      }
      thr->ತಳ್ಳು_vec4(out);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_ಸರತಿ_ಶಬ್ದ_PROP_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      size_t pid = cp->ಸಂಖ್ಯೆ;
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಬಿಟ್_idx[0];
      ಚಿಹ್ನೆರಹಿತ ix_reg = cp->ಬಿಟ್_idx[1];
      int64_t adr = thr->ಪದಗಳು[ix_reg].w_int;
      ಆದರೆ (thr->flags[4] == BIT4_1) {
	    thr->ತಳ್ಳು_vec4(vvp_vector4_t(wid));
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }
      ಆದರೆ (adr < 0) {
	    thr->ತಳ್ಳು_vec4(vvp_vector4_t(wid));
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

      vvp_object_t saved_ತುದಿ;
      ತಾರ್ಕಿಕ popped_ತುದಿ = ಸುಳ್ಳು;
      vvp_object_t& ತುದಿ = thr->peek_object();
      vvp_cobject*cobj = ತುದಿ.peek<vvp_cobject>();
      ಆದರೆ (cobj == 0) {
	    // Array-locator with(property) keeps an accumulator queue on top.
	    // Temporarily pop it to access the class object beneath.
	    thr->ತೆಗೆ_object(saved_ತುದಿ);
	    popped_ತುದಿ = ನಿಜ;
	    vvp_object_t& ಕೆಳಗೆ = thr->peek_object();
	    cobj = ಕೆಳಗೆ.peek<vvp_cobject>();
      }
      ಆದರೆ (cobj == 0) {
	    ಆದರೆ (popped_ತುದಿ) thr->ತಳ್ಳು_object(saved_ತುದಿ);
	    thr->ತಳ್ಳು_vec4(vvp_vector4_t(wid));
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_ಸರತಿ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ಸರತಿ_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t out(wid);
      ಆದರೆ (qsrc) {
	    ಆದರೆ ((size_t)adr >= qsrc->get_ಗಾತ್ರ()) {
		  thr->ತಳ್ಳು_vec4(out);
		  ಹಿಂತಿರುಗಿ ನಿಜ;
	    }
	    qsrc->get_ಶಬ್ದ((ಚಿಹ್ನೆರಹಿತ)adr, out);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dsrc) {
	    ಆದರೆ ((size_t)adr >= dsrc->get_ಗಾತ್ರ()) {
		  thr->ತಳ್ಳು_vec4(out);
		  ಹಿಂತಿರುಗಿ ನಿಜ;
	    }
	    dsrc->get_ಶಬ್ದ((ಚಿಹ್ನೆರಹಿತ)adr, out);
      } ಇಲ್ಲದಿದ್ದರೆ {
	    thr->ತಳ್ಳು_vec4(out);
	    ಆದರೆ (popped_ತುದಿ) thr->ತಳ್ಳು_object(saved_ತುದಿ);
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }
      ಆದರೆ (popped_ತುದಿ) thr->ತಳ್ಳು_object(saved_ತುದಿ);
      thr->ತಳ್ಳು_vec4(out);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_CMPIX_LTU(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ a = cp->ಸಂಖ್ಯೆ;
      ಚಿಹ್ನೆರಹಿತ b = cp->ಬಿಟ್_idx[0];
      ಆದರೆ (thr->ಪದಗಳು[a].w_uint < thr->ಪದಗಳು[b].w_uint) {
	    thr->flags[4] = BIT4_1;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    thr->flags[4] = BIT4_0;
      }
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_CMPIX_SLT0(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ r = cp->ಸಂಖ್ಯೆ;
      thr->flags[4] = (thr->ಪದಗಳು[r].w_int < 0) ? BIT4_1 : BIT4_0;
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_ತಳ್ಳು_IX_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ ix = cp->ಸಂಖ್ಯೆ;
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಬಿಟ್_idx[0];
      ಚಿಹ್ನೆರಹಿತ sgn = cp->ಬಿಟ್_idx[1];
      uint32_t ubits;
      ಆದರೆ (sgn) {
	    int32_t sv = (int32_t)thr->ಪದಗಳು[ix].w_int;
	    ubits = (uint32_t)sv;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ubits = (uint32_t)thr->ಪದಗಳು[ix].w_uint;
      }
      ಆದರೆ (wid > 32) wid = 32;
      vvp_vector4_t tmp(wid, BIT4_0);
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0; idx < wid; idx++) {
	    ಆದರೆ (ubits & 1u) {
		  tmp.ಗಣ_ಬಿಟ್(idx, BIT4_1);
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  tmp.ಗಣ_ಬಿಟ್(idx, BIT4_0);
	    }
	    ubits >>= 1;
      }
      thr->ತಳ್ಳು_vec4(tmp);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_ಸರತಿ_NEW_ಖಾಲಿಗಳು_V(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      thr->ತಳ್ಳು_object(vvp_object_t(ಹೊಸ vvp_ಸರತಿ_vec4()));
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_ಸರತಿ_APPEND_ಶಬ್ದ_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಬಿಟ್_idx[0];
      vvp_vector4_t elem = thr->ತೆಗೆ_vec4();
      ಪ್ರತಿಜ್ಞಾ(elem.size() == wid);
      vvp_object_t qobj;
      thr->ತೆಗೆ_object(qobj);
      vvp_ಸರತಿ_vec4* q = qobj.peek<vvp_ಸರತಿ_vec4>();
      ಪ್ರತಿಜ್ಞಾ(q);
      q->push_back(elem, 0);
      thr->ತಳ್ಳು_object(qobj);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_DELETE_PROP_ELEM(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      size_t pid = cp->ಸಂಖ್ಯೆ;

      int64_t idx_val = thr->ಪದಗಳು[3].w_int;
      ಆದರೆ (thr->flags[4] == BIT4_1) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with undefined index."
	         << ಸಾಲು_ಅಂತ್ಯ;
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }
      ಆದರೆ (idx_val < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping queue delete() with negative index."
	         << ಸಾಲು_ಅಂತ್ಯ;
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }
      size_t idx = idx_val;

      vvp_object_t& ತುದಿ = thr->peek_object();
      vvp_cobject*cobj = ತುದಿ.peek<vvp_cobject>();
      ಪ್ರತಿಜ್ಞಾ(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_ಸರತಿ* ಸರತಿ = qobj.peek<vvp_ಸರತಿ>();
      ಆದರೆ (ಸರತಿ == 0) {
	    cerr << thr->get_fileline()
	         << "Warning: skipping delete(" << idx
	         << ") on empty queue." << ಸಾಲು_ಅಂತ್ಯ;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    size_t size = ಸರತಿ->get_ಗಾತ್ರ();
	    ಆದರೆ (idx >= size) {
		  cerr << thr->get_fileline()
		       << "Warning: skipping out of range delete(" << idx
		       << ") on queue of size " << size << "." << ಸಾಲು_ಅಂತ್ಯ;
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  ಸರತಿ->erase(idx);
	    }
      }

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_DELETE_PROP_OBJ(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      size_t pid = cp->ಸಂಖ್ಯೆ;

      vvp_object_t& ತುದಿ = thr->peek_object();
      vvp_cobject*cobj = ತುದಿ.peek<vvp_cobject>();
      ಪ್ರತಿಜ್ಞಾ(cobj);

      cobj->ಗಣ_object(pid, vvp_object_t(), 0);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು ELEM, ವರ್ಗ QTYPE>
ಸ್ಥಿರ ತಾರ್ಕಿಕ store_qb(vthread_t thr, vvp_ಸಂಕೇತ_t cp, ಚಿಹ್ನೆರಹಿತ wid=0)
{
      ELEM value;
      vvp_net_t*net = cp->net;
      ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ = thr->ಪದಗಳು[cp->ಬಿಟ್_idx[0]].w_int;
      ತೆಗೆ_ಮೌಲ್ಯ(thr, value, wid); // Pop the value to store.

      vvp_ಸರತಿ*ಸರತಿ = get_ಸರತಿ_object<QTYPE>(thr, net);
      ಪ್ರತಿಜ್ಞಾ(ಸರತಿ);
      ಸರತಿ->push_back(value, max_ಗಾತ್ರ);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %store/qb/r <var-label>, <max-idx>
 */
ತಾರ್ಕಿಕ ದ_STORE_QB_R(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ store_qb<ದ್ವಿಗುಣ, vvp_ಸರತಿ_real>(thr, cp);
}

/*
 * %store/qb/str <var-label>, <max-idx>
 */
ತಾರ್ಕಿಕ ದ_STORE_QB_STR(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ store_qb<string, vvp_ಸರತಿ_string>(thr, cp);
}

/*
 * %store/qb/v <var-label>, <max-idx>, <wid>
 */
ತಾರ್ಕಿಕ ದ_STORE_QB_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ store_qb<vvp_vector4_t, vvp_ಸರತಿ_vec4>(thr, cp, cp->ಬಿಟ್_idx[1]);
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು ELEM, ವರ್ಗ QTYPE>
ಸ್ಥಿರ ತಾರ್ಕಿಕ store_qdar(vthread_t thr, vvp_ಸಂಕೇತ_t cp, ಚಿಹ್ನೆರಹಿತ wid=0)
{
      int64_t idx = thr->ಪದಗಳು[3].w_int;
      ELEM value;
      vvp_net_t*net = cp->net;
      ತೆಗೆ_ಮೌಲ್ಯ(thr, value, wid); // Pop the value to store.

      vvp_ಸರತಿ*ಸರತಿ = get_ಸರತಿ_object<QTYPE>(thr, net);
      ಪ್ರತಿಜ್ಞಾ(ಸರತಿ);
      ಆದರೆ (idx < 0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot assign to a negative "
	         << get_ಸರತಿ_type(value)
	         << " index (" << idx << "). ";
	    print_ಸರತಿ_ಮೌಲ್ಯ(value);
	    cerr << " was not added." << ಸಾಲು_ಅಂತ್ಯ;
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (thr->flags[4] != BIT4_0) {
	    cerr << thr->get_fileline()
	         << "Warning: cannot assign to an undefined "
	         << get_ಸರತಿ_type(value) << " index. ";
	    print_ಸರತಿ_ಮೌಲ್ಯ(value);
	    cerr << " was not added." << ಸಾಲು_ಅಂತ್ಯ;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ = thr->ಪದಗಳು[cp->ಬಿಟ್_idx[0]].w_int;
	    ಸರತಿ->ಗಣ_ಶಬ್ದ_max(idx, value, max_ಗಾತ್ರ);
      }
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %store/qdar/r <var>, idx
 */
ತಾರ್ಕಿಕ ದ_STORE_QDAR_R(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ store_qdar<ದ್ವಿಗುಣ, vvp_ಸರತಿ_real>(thr, cp);
}

/*
 * %store/qdar/str <var>, idx
 */
ತಾರ್ಕಿಕ ದ_STORE_QDAR_STR(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ store_qdar<string, vvp_ಸರತಿ_string>(thr, cp);
}

/*
 * %store/qdar/v <var>, idx
 */
ತಾರ್ಕಿಕ ದ_STORE_QDAR_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ store_qdar<vvp_vector4_t, vvp_ಸರತಿ_vec4>(thr, cp, cp->ಬಿಟ್_idx[1]);
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು ELEM, ವರ್ಗ QTYPE>
ಸ್ಥಿರ ತಾರ್ಕಿಕ store_qf(vthread_t thr, vvp_ಸಂಕೇತ_t cp, ಚಿಹ್ನೆರಹಿತ wid=0)
{
      ELEM value;
      vvp_net_t*net = cp->net;
      ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ = thr->ಪದಗಳು[cp->ಬಿಟ್_idx[0]].w_int;
      ತೆಗೆ_ಮೌಲ್ಯ(thr, value, wid); // Pop the value to store.

      vvp_ಸರತಿ*ಸರತಿ = get_ಸರತಿ_object<QTYPE>(thr, net);
      ಪ್ರತಿಜ್ಞಾ(ಸರತಿ);
      ಸರತಿ->push_front(value, max_ಗಾತ್ರ);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}
/*
 * %store/qf/r <var-label>, <max-idx>
 */
ತಾರ್ಕಿಕ ದ_STORE_QF_R(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ store_qf<ದ್ವಿಗುಣ, vvp_ಸರತಿ_real>(thr, cp);
}

/*
 * %store/qf/str <var-label>, <max-idx>
 */
ತಾರ್ಕಿಕ ದ_STORE_QF_STR(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ store_qf<string, vvp_ಸರತಿ_string>(thr, cp);
}

/*
 * %store/qb/v <var-label>, <max-idx>, <wid>
 */
ತಾರ್ಕಿಕ ದ_STORE_QF_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ store_qf<vvp_vector4_t, vvp_ಸರತಿ_vec4>(thr, cp, cp->ಬಿಟ್_idx[1]);
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು ELEM, ವರ್ಗ QTYPE>
ಸ್ಥಿರ ತಾರ್ಕಿಕ store_qobj(vthread_t thr, vvp_ಸಂಕೇತ_t cp, ಚಿಹ್ನೆರಹಿತ wid=0)
{
// FIXME: Can we actually use wid here?
      (ಶೂನ್ಯ)wid;
      vvp_net_t*net = cp->net;

      vvp_ಸರತಿ*ಸರತಿ = get_ಸರತಿ_object<QTYPE>(thr, net);
      ಪ್ರತಿಜ್ಞಾ(ಸರತಿ);

      vvp_object_t src;
      thr->ತೆಗೆ_object(src);

        // If it is null just clear the queue
      ಆದರೆ (src.test_nil())
	    ಸರತಿ->erase_ಬಾಲ(0);
      ಇಲ್ಲದಿದ್ದರೆ {
	    ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ = thr->ಪದಗಳು[cp->ಬಿಟ್_idx[0]].w_int;
	    ಸರತಿ->copy_elems(src, max_ಗಾತ್ರ);
      }

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_STORE_QOBJ_R(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ store_qobj<ದ್ವಿಗುಣ, vvp_ಸರತಿ_real>(thr, cp);
}

ತಾರ್ಕಿಕ ದ_STORE_QOBJ_STR(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ store_qobj<string, vvp_ಸರತಿ_string>(thr, cp);
}

ತಾರ್ಕಿಕ ದ_STORE_QOBJ_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ store_qobj<vvp_vector4_t, vvp_ಸರತಿ_vec4>(thr, cp, cp->ಬಿಟ್_idx[1]);
}

ಸ್ಥಿರ ಶೂನ್ಯ vvp_send(vthread_t thr, vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ&val)
{
      vvp_send_real(ptr, val, thr->wt_context);
}

ಸ್ಥಿರ ಶೂನ್ಯ vvp_send(vthread_t thr, vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ string&val)
{
      vvp_send_string(ptr, val, thr->wt_context);
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು ELEM>
ಸ್ಥಿರ ತಾರ್ಕಿಕ store(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ELEM val;
      ತೆಗೆ_ಮೌಲ್ಯ(thr, val, 0);
	/* set the value into port 0 of the destination. */
      vvp_net_ptr_t ptr (cp->net, 0);
      vvp_send(thr, ptr, val);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_STORE_REAL(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ store<ದ್ವಿಗುಣ>(thr, cp);
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು ELEM>
ಸ್ಥಿರ ತಾರ್ಕಿಕ storea(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ idx = cp->ಬಿಟ್_idx[0];
      ELEM val;
      ತೆಗೆ_ಮೌಲ್ಯ(thr, val, 0);

      ಆದರೆ (thr->flags[4] != BIT4_1) {
	    ಚಿಹ್ನೆರಹಿತ adr = thr->ಪದಗಳು[idx].w_int;
	    cp->array->ಗಣ_ಶಬ್ದ(adr, val);
      }

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %store/reala <var-label> <index>
 */
ತಾರ್ಕಿಕ ದ_STORE_REALA(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ storea<ದ್ವಿಗುಣ>(thr, cp);
}

ತಾರ್ಕಿಕ ದ_STORE_STR(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ store<string>(thr, cp);
}

/*
 * %store/stra <array-label> <index>
 */
ತಾರ್ಕಿಕ ದ_STORE_STRA(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಹಿಂತಿರುಗಿ storea<string>(thr, cp);
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
ತಾರ್ಕಿಕ ದ_STORE_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_net_ptr_t ptr(cp->net, 0);
      ಸ್ಥಿರಾಂಕ vvp_signal_ಮೌಲ್ಯ*sig = dynamic_cast<vvp_signal_ಮೌಲ್ಯ*> (cp->net->fil);
      ಚಿಹ್ನೆರಹಿತ off_ಸೂಚ್ಯಂಕ = cp->ಬಿಟ್_idx[0];
      ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ wid = cp->ಬಿಟ್_idx[1];

      int64_t off = off_ಸೂಚ್ಯಂಕ ? thr->ಪದಗಳು[off_ಸೂಚ್ಯಂಕ].w_int : 0;
      ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ sig_ಮೌಲ್ಯ_ಗಾತ್ರ = sig->ಮೌಲ್ಯ_ಗಾತ್ರ();

      vvp_vector4_t&val = thr->peek_vec4();
      ಚಿಹ್ನೆರಹಿತ val_ಗಾತ್ರ = val.size();

      ಆದರೆ (val_ಗಾತ್ರ < wid) {
	    cerr << thr->get_fileline()
	         << "XXXX Internal error: val.size()=" << val_ಗಾತ್ರ
		 << ", expecting >= " << wid << ಸಾಲು_ಅಂತ್ಯ;
      }
      ಪ್ರತಿಜ್ಞಾ(val_ಗಾತ್ರ >= wid);
      ಆದರೆ (val_ಗಾತ್ರ > wid) {
	    val.resize(wid);
      }

	// If there is a problem loading the index register, flags-4
	// will be set to 1, and we know here to skip the actual assignment.
      ಆದರೆ (off_ಸೂಚ್ಯಂಕ!=0 && thr->flags[4] == BIT4_1) {
	    thr->ತೆಗೆ_vec4(1);
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

      ಆದರೆ (!resize_rval_vec(val, off, sig_ಮೌಲ್ಯ_ಗಾತ್ರ)) {
	    thr->ತೆಗೆ_vec4(1);
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

      ಆದರೆ (off == 0 && val.size() == sig_ಮೌಲ್ಯ_ಗಾತ್ರ)
	    vvp_send_vec4(ptr, val, thr->wt_context);
      ಇಲ್ಲದಿದ್ದರೆ
	    vvp_send_vec4_pv(ptr, val, off, sig_ಮೌಲ್ಯ_ಗಾತ್ರ, thr->wt_context);

      thr->ತೆಗೆ_vec4(1);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %store/vec4a <var-label>, <addr>, <offset>
 */
ತಾರ್ಕಿಕ ದ_STORE_VEC4_A(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ adr_ಸೂಚ್ಯಂಕ = cp->ಬಿಟ್_idx[0];
      ಚಿಹ್ನೆರಹಿತ off_ಸೂಚ್ಯಂಕ = cp->ಬಿಟ್_idx[1];

      ದೀರ್ಘ adr = adr_ಸೂಚ್ಯಂಕ? thr->ಪದಗಳು[adr_ಸೂಚ್ಯಂಕ].w_int : 0;
      int64_t off = off_ಸೂಚ್ಯಂಕ ? thr->ಪದಗಳು[off_ಸೂಚ್ಯಂಕ].w_int : 0;

	// Suppress action if flags-4 is true.
      ಆದರೆ (thr->flags[4] == BIT4_1) {
	    thr->ತೆಗೆ_vec4(1);
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

      vvp_vector4_t &value = thr->peek_vec4();

      ಆದರೆ (!resize_rval_vec(value, off, cp->array->get_ಶಬ್ದ_ಗಾತ್ರ())) {
	    thr->ತೆಗೆ_vec4(1);
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

      cp->array->ಗಣ_ಶಬ್ದ(adr, off, value);

      thr->ತೆಗೆ_vec4(1);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %sub
 *   pop r;
 *   pop l;
 *   push l-r;
 */
ತಾರ್ಕಿಕ ದ_SUB(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      vvp_vector4_t r = thr->ತೆಗೆ_vec4();
      vvp_vector4_t&l = thr->peek_vec4();

      l.sub(r);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %subi <vala>, <valb>, <wid>
 *
 * Pop1 operand, get the other operand from the arguments, and push
 * the result.
 */
ತಾರ್ಕಿಕ ದ_SUBI(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಸಂಖ್ಯೆ;

      vvp_vector4_t&l = thr->peek_vec4();

	// I expect that most of the bits of an immediate value are
	// going to be zero, so start the result vector with all zero
	// bits. Then we only need to replace the bits that are different.
      vvp_vector4_t r (wid, BIT4_0);
      get_ತಕ್ಷಣ_rval (cp, r);

      l.sub(r);

      ಹಿಂತಿರುಗಿ ನಿಜ;

}

ತಾರ್ಕಿಕ ದ_SUB_WR(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      ದ್ವಿಗುಣ r = thr->ತೆಗೆ_real();
      ದ್ವಿಗುಣ l = thr->ತೆಗೆ_real();
      thr->ತಳ್ಳು_real(l - r);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %substr <first>, <last>
 * Pop a string, take the substring (SystemVerilog style), and return
 * the result to the stack. This opcode actually works by editing the
 * string in place.
 */
ತಾರ್ಕಿಕ ದ_SUBSTR(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      int32_t ಮೊದಲ = thr->ಪದಗಳು[cp->ಬಿಟ್_idx[0]].w_int;
      int32_t ಕೊನೆಯ = thr->ಪದಗಳು[cp->ಬಿಟ್_idx[1]].w_int;
      string&val = thr->peek_str(0);

      ಆದರೆ (ಮೊದಲ < 0 || ಕೊನೆಯ < ಮೊದಲ || ಕೊನೆಯ >= (int32_t)val.size()) {
	    val = string("");
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

      val = val.substr(ಮೊದಲ, ಕೊನೆಯ-ಮೊದಲ+1);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %substr/vec4 <index>, <wid>
 */
ತಾರ್ಕಿಕ ದ_SUBSTR_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ sel_idx = cp->ಬಿಟ್_idx[0];
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಬಿಟ್_idx[1];

      int32_t sel = thr->ಪದಗಳು[sel_idx].w_int;
      ಸ್ಥಿರಾಂಕ string&val = thr->peek_str(0);

      ಪ್ರತಿಜ್ಞಾ(wid%8 == 0);

      ಆದರೆ (sel < 0 || sel >= (int32_t)val.size()) {
	    vvp_vector4_t res (wid, BIT4_0);
	    thr->ತಳ್ಳು_vec4(res);
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

      vvp_vector4_t res (wid, BIT4_0);

      ಪ್ರತಿಜ್ಞಾ(wid==8);
      ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ tmp = val[sel];
      ಗಾಗಿ (ಪೂರ್ಣಾಂಕ idx = 0 ; idx < 8 ; idx += 1) {
	    ಆದರೆ (tmp & (1<<idx))
		  res.ಗಣ_ಬಿಟ್(idx, BIT4_1);
      }

      thr->ತಳ್ಳು_vec4(res);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_FILE_ಸಾಲು(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vpiHandle handle = cp->handle;

	/* When it is available, keep the file/line information in the
	   thread for error/warning messages. */
      thr->ಗಣ_fileline(vpi_get_str(vpiFile, handle),
                        vpi_get(vpi_ಸಾಲು_No, handle));

      ಆದರೆ (show_file_ಸಾಲು)
	    cerr << thr->get_fileline()
	         << vpi_get_str(_vpiDescription, handle) << ಸಾಲು_ಅಂತ್ಯ;

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %test_nul <var-label>;
 * Test if the object at the specified variable is nil. If so, write
 * "1" into flags[4], otherwise write "0" into flags[4].
 */
ತಾರ್ಕಿಕ ದ_TEST_NUL(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_net_t*net = cp->net;

      ಪ್ರತಿಜ್ಞಾ(net);
      vvp_fun_signal_object*obj = dynamic_cast<vvp_fun_signal_object*> (net->fun);
      ಪ್ರತಿಜ್ಞಾ(obj);

      ಆದರೆ (obj->get_object().test_nil())
	    thr->flags[4] = BIT4_1;
      ಇಲ್ಲದಿದ್ದರೆ
	    thr->flags[4] = BIT4_0;

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_TEST_NUL_A(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ idx = cp->ಬಿಟ್_idx[0];
      ಚಿಹ್ನೆರಹಿತ adr = thr->ಪದಗಳು[idx].w_int;
      vvp_object_t ಶಬ್ದ;

	/* If the address is undefined, return true. */
      ಆದರೆ (thr->flags[4] == BIT4_1) {
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

      cp->array->get_ಶಬ್ದ_obj(adr, ಶಬ್ದ);
      ಆದರೆ (ಶಬ್ದ.test_nil())
	    thr->flags[4] = BIT4_1;
      ಇಲ್ಲದಿದ್ದರೆ
	    thr->flags[4] = BIT4_0;

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_TEST_NUL_OBJ(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      ಆದರೆ (thr->peek_object().test_nil())
	    thr->flags[4] = BIT4_1;
      ಇಲ್ಲದಿದ್ದರೆ
	    thr->flags[4] = BIT4_0;
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %test_nul/prop <pid>, <idx>
 */
ತಾರ್ಕಿಕ ದ_TEST_NUL_PROP(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ pid = cp->ಸಂಖ್ಯೆ;
      ಚಿಹ್ನೆರಹಿತ idx = cp->ಬಿಟ್_idx[0];

      ಆದರೆ (idx != 0) {
	    ಪ್ರತಿಜ್ಞಾ(idx < vthread_s::ಪದಗಳು_ಎಣಿಕೆ);
	    idx = thr->ಪದಗಳು[idx].w_uint;
      }

      vvp_object_t&obj = thr->peek_object();
      vvp_cobject*cobj  = obj.peek<vvp_cobject>();

      vvp_object_t val;
      cobj->get_object(pid, val, idx);

      ಆದರೆ (val.test_nil())
	    thr->flags[4] = BIT4_1;
      ಇಲ್ಲದಿದ್ದರೆ
	    thr->flags[4] = BIT4_0;

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/* and() reduction returns the scalar sum (Verilog-style bitwise-AND per word). */
ಮಾದರಿ <ವರ್ಗ SRC>
ಸ್ಥಿರ vvp_vector4_t uparr_and_ಪದಗಳು_src(SRC* src, ಚಿಹ್ನೆರಹಿತ wid)
{
      ಆದರೆ (!src) ಹಿಂತಿರುಗಿ vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_1);
      ಗಾಗಿ (size_t i = 0; i < src->get_ಗಾತ್ರ(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_ಶಬ್ದ(i, vi);
	    res &= vi;
      }
      ಹಿಂತಿರುಗಿ res;
}

ತಾರ್ಕಿಕ ದ_UPARR_AND_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_net_t* net = cp->net;
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಬಿಟ್_idx[0];
      vvp_ಸರತಿ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ಸರತಿ_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      ಆದರೆ (qsrc) res = uparr_and_ಪದಗಳು_src(qsrc, wid);
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dsrc) res = uparr_and_ಪದಗಳು_src(dsrc, wid);
      ಇಲ್ಲದಿದ್ದರೆ {
	    vvp_ಸರತಿ* src_q = get_ಸರತಿ_object<vvp_ಸರತಿ_vec4>(thr, net);
	    vvp_ಸರತಿ_vec4* src = dynamic_cast<vvp_ಸರತಿ_vec4*>(src_q);
	    res = uparr_and_ಪದಗಳು_src(src, wid);
      }
      thr->ತಳ್ಳು_vec4(res);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/* or() reduction returns the scalar sum (Verilog-style bitwise-AND per word). */
ಮಾದರಿ <ವರ್ಗ SRC>
ಸ್ಥಿರ vvp_vector4_t uparr_or_ಪದಗಳು_src(SRC* src, ಚಿಹ್ನೆರಹಿತ wid)
{
      ಆದರೆ (!src) ಹಿಂತಿರುಗಿ vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_0);
      ಗಾಗಿ (size_t i = 0; i < src->get_ಗಾತ್ರ(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_ಶಬ್ದ(i, vi);
	    res |= vi;
      }
      ಹಿಂತಿರುಗಿ res;
}

ತಾರ್ಕಿಕ ದ_UPARR_OR_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_net_t* net = cp->net;
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಬಿಟ್_idx[0];
      vvp_ಸರತಿ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ಸರತಿ_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      ಆದರೆ (qsrc) res = uparr_or_ಪದಗಳು_src(qsrc, wid);
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dsrc) res = uparr_or_ಪದಗಳು_src(dsrc, wid);
      ಇಲ್ಲದಿದ್ದರೆ {
	    vvp_ಸರತಿ* src_q = get_ಸರತಿ_object<vvp_ಸರತಿ_vec4>(thr, net);
	    vvp_ಸರತಿ_vec4* src = dynamic_cast<vvp_ಸರತಿ_vec4*>(src_q);
	    res = uparr_or_ಪದಗಳು_src(src, wid);
      }
      thr->ತಳ್ಳು_vec4(res);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/* real product() reduction returns the scalar sum */
ಮಾದರಿ <ವರ್ಗ SRC>
ಸ್ಥಿರ ದ್ವಿಗುಣ uparr_ಗುಣಲಬ್ಧ_ಪದಗಳು_src_real(SRC* src)
{
      ಆದರೆ (!src) ಹಿಂತಿರುಗಿ 0.0;
      ದ್ವಿಗುಣ acc = 1.0;
      ಗಾಗಿ (size_t i = 0; i < src->get_ಗಾತ್ರ(); i += 1) {
	    ದ್ವಿಗುಣ vi;
	    src->get_ಶಬ್ದ(i, vi);
	    acc *= vi;
      }
      ಹಿಂತಿರುಗಿ acc;
}

ತಾರ್ಕಿಕ ದ_UPARR_ಗುಣಲಬ್ಧ_REAL(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_ಸರತಿ_real* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ಸರತಿ_or_darray_real_from_net(thr, net, qsrc, dsrc);
      ದ್ವಿಗುಣ ಮೊತ್ತ;
      ಆದರೆ (qsrc) ಮೊತ್ತ = uparr_ಗುಣಲಬ್ಧ_ಪದಗಳು_src_real(qsrc);
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dsrc) ಮೊತ್ತ = uparr_ಗುಣಲಬ್ಧ_ಪದಗಳು_src_real(dsrc);
      ಇಲ್ಲದಿದ್ದರೆ {
	    vvp_ಸರತಿ* src_q = get_ಸರತಿ_object<vvp_ಸರತಿ_real>(thr, net);
	    vvp_ಸರತಿ_real* src = dynamic_cast<vvp_ಸರತಿ_real*>(src_q);
	    ಮೊತ್ತ = uparr_ಗುಣಲಬ್ಧ_ಪದಗಳು_src_real(src);
      }
      thr->ತಳ್ಳು_real(ಮೊತ್ತ);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/* product() reduction returns the scalar product (Verilog-style multiplication per word). */
ಮಾದರಿ <ವರ್ಗ SRC>
ಸ್ಥಿರ vvp_vector4_t uparr_ಗುಣಲಬ್ಧ_ಪದಗಳು_src(SRC* src, ಚಿಹ್ನೆರಹಿತ wid)
{
      ಆದರೆ (!src) ಹಿಂತಿರುಗಿ vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t acc = ಸರತಿ_unique_ulong_ಗಮ್ಯ_vec4(1UL, wid);
      ಗಾಗಿ (size_t i = 0; i < src->get_ಗಾತ್ರ(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_ಶಬ್ದ(i, vi);
	    acc.mul(vi);
      }
      ಹಿಂತಿರುಗಿ acc;
}

ತಾರ್ಕಿಕ ದ_UPARR_ಗುಣಲಬ್ಧ_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_net_t* net = cp->net;
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಬಿಟ್_idx[0];
      vvp_ಸರತಿ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ಸರತಿ_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t prod;
      ಆದರೆ (qsrc) prod = uparr_ಗುಣಲಬ್ಧ_ಪದಗಳು_src(qsrc, wid);
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dsrc) prod = uparr_ಗುಣಲಬ್ಧ_ಪದಗಳು_src(dsrc, wid);
      ಇಲ್ಲದಿದ್ದರೆ {
	    vvp_ಸರತಿ* src_q = get_ಸರತಿ_object<vvp_ಸರತಿ_vec4>(thr, net);
	    vvp_ಸರತಿ_vec4* src = dynamic_cast<vvp_ಸರತಿ_vec4*>(src_q);
	    prod = uparr_ಗುಣಲಬ್ಧ_ಪದಗಳು_src(src, wid);
      }
      thr->ತಳ್ಳು_vec4(prod);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_UPARR_ಗುಣಲಬ್ಧ_PROP_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      size_t pid = cp->ಸಂಖ್ಯೆ;
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಬಿಟ್_idx[0];

      vvp_object_t& ತುದಿ = thr->peek_object();
      vvp_cobject*cobj = ತುದಿ.peek<vvp_cobject>();
      ಪ್ರತಿಜ್ಞಾ(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_ಸರತಿ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ಸರತಿ_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t prod =
	    qsrc ? uparr_ಗುಣಲಬ್ಧ_ಪದಗಳು_src(qsrc, wid)
		 : uparr_ಗುಣಲಬ್ಧ_ಪದಗಳು_src(dsrc, wid);
      thr->ತಳ್ಳು_vec4(prod);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_UPARR_ಗುಣಲಬ್ಧ_OBJ_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಬಿಟ್_idx[0];
      vvp_object_t src_obj;
      thr->ತೆಗೆ_object(src_obj);

      vvp_ಸರತಿ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ಸರತಿ_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_vector4_t prod =
	    qsrc ? uparr_ಗುಣಲಬ್ಧ_ಪದಗಳು_src(qsrc, wid)
		 : uparr_ಗುಣಲಬ್ಧ_ಪದಗಳು_src(dsrc, wid);
      thr->ತಳ್ಳು_vec4(prod);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/* real sum() reduction returns the scalar sum */
ಮಾದರಿ <ವರ್ಗ SRC>
ಸ್ಥಿರ ದ್ವಿಗುಣ uparr_ಮೊತ್ತ_ಪದಗಳು_src_real(SRC* src)
{
      ದ್ವಿಗುಣ acc = 0.0;
      ಆದರೆ (!src) ಹಿಂತಿರುಗಿ acc;
      ಗಾಗಿ (size_t i = 0; i < src->get_ಗಾತ್ರ(); i += 1) {
	    ದ್ವಿಗುಣ vi;
	    src->get_ಶಬ್ದ(i, vi);
	    acc += vi;
      }
      ಹಿಂತಿರುಗಿ acc;
}

ತಾರ್ಕಿಕ ದ_UPARR_ಮೊತ್ತ_REAL(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_net_t* net = cp->net;
      vvp_ಸರತಿ_real* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ಸರತಿ_or_darray_real_from_net(thr, net, qsrc, dsrc);
      ದ್ವಿಗುಣ ಮೊತ್ತ;
      ಆದರೆ (qsrc) ಮೊತ್ತ = uparr_ಮೊತ್ತ_ಪದಗಳು_src_real(qsrc);
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dsrc) ಮೊತ್ತ = uparr_ಮೊತ್ತ_ಪದಗಳು_src_real(dsrc);
      ಇಲ್ಲದಿದ್ದರೆ {
	    vvp_ಸರತಿ* src_q = get_ಸರತಿ_object<vvp_ಸರತಿ_real>(thr, net);
	    vvp_ಸರತಿ_real* src = dynamic_cast<vvp_ಸರತಿ_real*>(src_q);
	    ಮೊತ್ತ = uparr_ಮೊತ್ತ_ಪದಗಳು_src_real(src);
      }
      thr->ತಳ್ಳು_real(ಮೊತ್ತ);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/* sum() reduction returns the scalar sum (Verilog-style addition per word). */
ಮಾದರಿ <ವರ್ಗ SRC>
ಸ್ಥಿರ vvp_vector4_t uparr_ಮೊತ್ತ_ಪದಗಳು_src(SRC* src, ಚಿಹ್ನೆರಹಿತ wid)
{
      ಆದರೆ (!src) ಹಿಂತಿರುಗಿ vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t acc(wid, BIT4_0);
      ಗಾಗಿ (size_t i = 0; i < src->get_ಗಾತ್ರ(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_ಶಬ್ದ(i, vi);
	    acc.ಕೂಡಿಸು(vi);
      }
      ಹಿಂತಿರುಗಿ acc;
}

ತಾರ್ಕಿಕ ದ_UPARR_ಮೊತ್ತ_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_net_t* net = cp->net;
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಬಿಟ್_idx[0];
      vvp_ಸರತಿ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ಸರತಿ_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t ಮೊತ್ತ;
      ಆದರೆ (qsrc) ಮೊತ್ತ = uparr_ಮೊತ್ತ_ಪದಗಳು_src(qsrc, wid);
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dsrc) ಮೊತ್ತ = uparr_ಮೊತ್ತ_ಪದಗಳು_src(dsrc, wid);
      ಇಲ್ಲದಿದ್ದರೆ {
	    vvp_ಸರತಿ* src_q = get_ಸರತಿ_object<vvp_ಸರತಿ_vec4>(thr, net);
	    vvp_ಸರತಿ_vec4* src = dynamic_cast<vvp_ಸರತಿ_vec4*>(src_q);
	    ಮೊತ್ತ = uparr_ಮೊತ್ತ_ಪದಗಳು_src(src, wid);
      }
      thr->ತಳ್ಳು_vec4(ಮೊತ್ತ);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_UPARR_ಮೊತ್ತ_PROP_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      size_t pid = cp->ಸಂಖ್ಯೆ;
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಬಿಟ್_idx[0];

      vvp_object_t& ತುದಿ = thr->peek_object();
      vvp_cobject*cobj = ತುದಿ.peek<vvp_cobject>();
      ಪ್ರತಿಜ್ಞಾ(cobj);

      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      vvp_ಸರತಿ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ಸರತಿ_or_darray_vec4_from_object(qobj, qsrc, dsrc);
      vvp_vector4_t ಮೊತ್ತ =
	    qsrc ? uparr_ಮೊತ್ತ_ಪದಗಳು_src(qsrc, wid)
		 : uparr_ಮೊತ್ತ_ಪದಗಳು_src(dsrc, wid);
      thr->ತಳ್ಳು_vec4(ಮೊತ್ತ);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_UPARR_ಮೊತ್ತ_OBJ_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಬಿಟ್_idx[0];
      vvp_object_t src_obj;
      thr->ತೆಗೆ_object(src_obj);

      vvp_ಸರತಿ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ಸರತಿ_or_darray_vec4_from_object(src_obj, qsrc, dsrc);
      vvp_vector4_t ಮೊತ್ತ =
	    qsrc ? uparr_ಮೊತ್ತ_ಪದಗಳು_src(qsrc, wid)
		 : uparr_ಮೊತ್ತ_ಪದಗಳು_src(dsrc, wid);
      thr->ತಳ್ಳು_vec4(ಮೊತ್ತ);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/* xor() reduction returns the scalar sum (Verilog-style bitwise-XOR per word). */
ಮಾದರಿ <ವರ್ಗ SRC>
ಸ್ಥಿರ vvp_vector4_t uparr_xor_ಪದಗಳು_src(SRC* src, ಚಿಹ್ನೆರಹಿತ wid)
{
      ಆದರೆ (!src) ಹಿಂತಿರುಗಿ vvp_vector4_t(wid, BIT4_X);
      vvp_vector4_t res(wid, BIT4_0);
      ಗಾಗಿ (size_t i = 0; i < src->get_ಗಾತ್ರ(); i += 1) {
	    vvp_vector4_t vi(wid);
	    src->get_ಶಬ್ದ(i, vi);
	    res ^= vi;
      }
      ಹಿಂತಿರುಗಿ res;
}

ತಾರ್ಕಿಕ ದ_UPARR_XOR_V(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vvp_net_t* net = cp->net;
      ಚಿಹ್ನೆರಹಿತ wid = cp->ಬಿಟ್_idx[0];
      vvp_ಸರತಿ_vec4* qsrc = 0;
      vvp_darray* dsrc = 0;
      get_ಸರತಿ_or_darray_vec4_from_net(thr, net, qsrc, dsrc);
      vvp_vector4_t res;
      ಆದರೆ (qsrc) res = uparr_xor_ಪದಗಳು_src(qsrc, wid);
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dsrc) res = uparr_xor_ಪದಗಳು_src(dsrc, wid);
      ಇಲ್ಲದಿದ್ದರೆ {
	    vvp_ಸರತಿ* src_q = get_ಸರತಿ_object<vvp_ಸರತಿ_vec4>(thr, net);
	    vvp_ಸರತಿ_vec4* src = dynamic_cast<vvp_ಸರತಿ_vec4*>(src_q);
	    res = uparr_xor_ಪದಗಳು_src(src, wid);
      }
      thr->ತಳ್ಳು_vec4(res);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ದ_VPI_CALL(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      vpip_execute_vpi_call(thr, cp->handle);

      ಆದರೆ (ವೇಳಾಪಟ್ಟಿ_stopped()) {
	    ಆದರೆ (! ವೇಳಾಪಟ್ಟಿ_ಪೂರ್ಣಗೊಂಡ())
		  ವೇಳಾಪಟ್ಟಿ_vthread(thr, 0, ಸುಳ್ಳು);

	    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
      }

      ಹಿಂತಿರುಗಿ ವೇಳಾಪಟ್ಟಿ_ಪೂರ್ಣಗೊಂಡ()? ಸುಳ್ಳು : ನಿಜ;
}

/* %wait <label>;
 * Implement the wait by locating the vvp_net_T for the event, and
 * adding this thread to the threads list for the event. The some
 * argument is the  reference to the functor to wait for. This must be
 * an event object of some sort.
 */
ತಾರ್ಕಿಕ ದ_WAIT(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      ಪ್ರತಿಜ್ಞಾ(! thr->i_am_ಒಳ_function);
      ಪ್ರತಿಜ್ಞಾ(! thr->ಕಾಯುವಿಕೆ_for_event);
      thr->ಕಾಯುವಿಕೆ_for_event = 1;

	/* Add this thread to the list in the event. */
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (cp->net->fun);
      ಪ್ರತಿಜ್ಞಾ(ep);
      thr->wait_ಮುಂದಿನ = ep->ಕೂಡಿಸು_ಕಾಯುವಿಕೆ_thread(thr);

	/* Return false to suspend this thread. */
      ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

/*
 * Implement the %wait/fork (SystemVerilog) instruction by suspending
 * the current thread until all the detached children have finished.
 */
ತಾರ್ಕಿಕ ದ_WAIT_FORK(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
	/* If a %wait/fork is being executed then the parent thread
	 * cannot be waiting in a join or already waiting. */
      ಪ್ರತಿಜ್ಞಾ(! thr->i_am_ಒಳ_function);
      ಪ್ರತಿಜ್ಞಾ(! thr->i_am_joining);
      ಪ್ರತಿಜ್ಞಾ(! thr->i_am_ಕಾಯುವಿಕೆ);

	/* There should be no active children when waiting. */
      ಪ್ರತಿಜ್ಞಾ(thr->children.empty());

	/* If there are no detached children then there is nothing to
	 * wait for. */
      ಆದರೆ (thr->detached_children.empty()) ಹಿಂತಿರುಗಿ ನಿಜ;

	/* Flag that this process is waiting for the detached children
	 * to finish and suspend it. */
      thr->i_am_ಕಾಯುವಿಕೆ = 1;
      ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

/*
 * %xnor
 */
ತಾರ್ಕಿಕ ದ_XNOR(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      vvp_vector4_t valr = thr->ತೆಗೆ_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      ಪ್ರತಿಜ್ಞಾ(vall.size() == valr.size());

      vall ^= valr;
      vall.invert();

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * %xor
 */
ತಾರ್ಕಿಕ ದ_XOR(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      vvp_vector4_t valr = thr->ತೆಗೆ_vec4();
      vvp_vector4_t&vall = thr->peek_vec4();
      ಪ್ರತಿಜ್ಞಾ(vall.size() == valr.size());

      vall ^= valr;

      ಹಿಂತಿರುಗಿ ನಿಜ;
}


ತಾರ್ಕಿಕ ದ_ZOMBIE(vthread_t thr, vvp_ಸಂಕೇತ_t)
{
      thr->pc = codespace_null();
      ಆದರೆ ((thr->ಪೋಷಕ == 0) && (thr->children.empty())) {
	    ಆದರೆ (thr->delay_delete)
		  ವೇಳಾಪಟ್ಟಿ_del_thr(thr);
	    ಇಲ್ಲದಿದ್ದರೆ
		  vthread_delete(thr);
      }
      ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

/*
 * This is a phantom opcode used to call user defined functions. It
 * is used in code generated by the .ufunc statement. It contains a
 * pointer to the executable code of the function and a pointer to
 * a ufunc_core object that has all the port information about the
 * function.
 */
ಸ್ಥಿರ ತಾರ್ಕಿಕ do_exec_ufunc(vthread_t thr, vvp_ಸಂಕೇತ_t cp, vthread_t ಮಗು)
{
      __vpiScope*ಮಗು_scope = cp->ufunc_core_ptr->func_scope();
      ಪ್ರತಿಜ್ಞಾ(ಮಗು_scope);

      ಪ್ರತಿಜ್ಞಾ(ಮಗು_scope->get_type_ಸಂಕೇತ() == vpiFunction);
      ಪ್ರತಿಜ್ಞಾ(thr->children.empty());


        /* We can take a number of shortcuts because we know that a
           continuous assignment can only occur in a static scope. */
      ಪ್ರತಿಜ್ಞಾ(thr->wt_context == 0);
      ಪ್ರತಿಜ್ಞಾ(thr->rd_context == 0);

        /* If an automatic function, allocate a context for this call. */
      vvp_context_t ಮಗು_context = 0;
      ಆದರೆ (ಮಗು_scope->is_automatic()) {
            ಮಗು_context = vthread_alloc_context(ಮಗು_scope);
            thr->wt_context = ಮಗು_context;
            thr->rd_context = ಮಗು_context;
      }

      ಮಗು->wt_context = ಮಗು_context;
      ಮಗು->rd_context = ಮಗು_context;

	/* Copy all the inputs to the ufunc object to the port
	   variables of the function. This copies all the values
	   atomically. */
      cp->ufunc_core_ptr->ನಿಯೋಜಿಸು_ಬಿಟ್ಟುಗಳು_ಗಮ್ಯ_ports(ಮಗು_context);
      ಮಗು->delay_delete = 1;

      ಮಗು->ಪೋಷಕ = thr;
      thr->children.insert(ಮಗು);
	// This should be the only child
      ಪ್ರತಿಜ್ಞಾ(thr->children.size()==1);

      ಮಗು->is_scheduled = 1;
      ಮಗು->i_am_ಒಳ_function = 1;
      vthread_ಸತತ(ಮಗು);
      running_thread = thr;

      ಆದರೆ (ಮಗು->i_have_ended) {
	    do_join(thr, ಮಗು);
            ಹಿಂತಿರುಗಿ ನಿಜ;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    thr->i_am_joining = 1;
	    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
      }
}

ತಾರ್ಕಿಕ ದ_EXEC_UFUNC_REAL(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      __vpiScope*ಮಗು_scope = cp->ufunc_core_ptr->func_scope();
      ಪ್ರತಿಜ್ಞಾ(ಮಗು_scope);

	/* Create a temporary thread and run it immediately. */
      vthread_t ಮಗು = vthread_new(cp->cptr, ಮಗು_scope);
      thr->ತಳ್ಳು_real(0.0);
      ಮಗು->ವಾದಗಳು_real.push_back(0);

      ಹಿಂತಿರುಗಿ do_exec_ufunc(thr, cp, ಮಗು);
}

ತಾರ್ಕಿಕ ದ_EXEC_UFUNC_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      __vpiScope*ಮಗು_scope = cp->ufunc_core_ptr->func_scope();
      ಪ್ರತಿಜ್ಞಾ(ಮಗು_scope);

      vpiScopeFunction*scope_func = dynamic_cast<vpiScopeFunction*>(ಮಗು_scope);
      ಪ್ರತಿಜ್ಞಾ(scope_func);

	/* Create a temporary thread and run it immediately. */
      vthread_t ಮಗು = vthread_new(cp->cptr, ಮಗು_scope);
      thr->ತಳ್ಳು_vec4(vvp_vector4_t(scope_func->get_func_ಅಗಲ(), scope_func->get_func_init_val()));
      ಮಗು->ವಾದಗಳು_vec4.push_back(0);

      ಹಿಂತಿರುಗಿ do_exec_ufunc(thr, cp, ಮಗು);
}

/*
 * This is a phantom opcode used to harvest the result of calling a user
 * defined function. It is used in code generated by the .ufunc statement.
 */
ತಾರ್ಕಿಕ ದ_REAP_UFUNC(vthread_t thr, vvp_ಸಂಕೇತ_t cp)
{
      __vpiScope*ಮಗು_scope = cp->ufunc_core_ptr->func_scope();
      ಪ್ರತಿಜ್ಞಾ(ಮಗು_scope);

	/* Copy the output from the result variable to the output
	   ports of the .ufunc device. */
      cp->ufunc_core_ptr->ಮುಕ್ತಾಯ_thread();

        /* If an automatic function, free the context for this call. */
      ಆದರೆ (ಮಗು_scope->is_automatic()) {
            vthread_free_context(thr->rd_context, ಮಗು_scope);
            thr->wt_context = 0;
            thr->rd_context = 0;
      }

      ಹಿಂತಿರುಗಿ ನಿಜ;
}
