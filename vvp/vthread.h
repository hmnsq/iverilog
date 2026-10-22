#ifndef IVL_vthread_H
#define IVL_vthread_H
/*
 * Copyright (c) 2001-2020 Stephen Williams (steve@icarus.com)
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

# include  "vvp_net.h"

# include  <string>

/*
 * A vthread is a simulation thread that executes instructions when
 * they are scheduled. This structure contains all the thread specific
 * context needed to run an instruction.
 *
 * Included in a thread are its program counter, local bits and
 * members needed for tracking the thread. The thread runs by fetching
 * instructions from code space, interpreting the instruction, then
 * fetching the next instruction.
 */

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା vthread_s* vthread_t;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା vvp_code_s*vvp_code_t;
class __vpiScope;

/*
 * This creates a new simulation thread, with the given start
 * address. The generated thread is ready to run, but is not yet
 * scheduled.
 */
ବାହ୍ୟ vthread_t vthread_new(vvp_code_t sa, __vpiScope*scope);

/*
 * This function marks the thread as scheduled. It is used only by the
 * schedule_vthread function.
 */
ବାହ୍ୟ ଶୂନ୍ୟ vthread_mark_scheduled(vthread_t thr);

/*
 * This function marks the thread as being a final procedure.
 */
ବାହ୍ୟ ଶୂନ୍ୟ vthread_mark_final(vthread_t thr);

/*
 * This function causes deletion of the currently running thread to
 * be delayed until after all sync events have been processed for the
 * time step in which the thread terminates. It is only used by the
 * schedule_generic function.
 */
ବାହ୍ୟ ଶୂନ୍ୟ vthread_delay_delete();

/*
 * Cause this thread to execute instructions until in is put to sleep
 * by executing some sort of delay or wait instruction.
 */
ବାହ୍ୟ ଶୂନ୍ୟ vthread_run(vthread_t thr);

/*
 * This function schedules all the threads in the list to be scheduled
 * for execution with delay 0. The thr pointer is taken to be the head
 * of a list, and all the threads in the list are presumably placed in
 * the list by the %wait instruction.
 */
ବାହ୍ୟ ଶୂନ୍ୟ vthread_schedule_list(vthread_t thr);

ବାହ୍ୟ __vpiScope*vthread_scope(vthread_t thr);

/*
 * This function returns a handle to the writable context of the currently
 * running thread. Normally the writable context is the context allocated
 * to the scope associated with that thread. However, between executing a
 * %alloc instruction and executing the associated %fork instruction, the
 * writable context changes to the newly allocated context, thus allowing
 * the input parameters of an automatic task or function to be written to
 * the task/function local variables.
 */
ବାହ୍ୟ vvp_context_t vthread_get_wt_context();

/*
 * This function returns a handle to the readable context of the currently
 * running thread. Normally the readable context is the context allocated
 * to the scope associated with that thread. However, between executing a
 * %join instruction and executing the associated %free instruction, the
 * readable context changes to the context allocated to the newly joined
 * thread, thus allowing the output parameters of an automatic task or
 * function to be read from the task/function local variables.
 */
ବାହ୍ୟ vvp_context_t vthread_get_rd_context();

/*
 * This function returns a handle to an item in the writable context
 * of the currently running thread.
 */
ବାହ୍ୟ vvp_context_item_t vthread_get_wt_context_item(unsigned context_idx);

/*
 * This function returns a handle to an item in the readable context
 * of the currently running thread.
 */
ବାହ୍ୟ vvp_context_item_t vthread_get_rd_context_item(unsigned context_idx);

/*
 * Access value stacks from thread space.
 */
ବାହ୍ୟ ଶୂନ୍ୟ vthread_ଠେଲ(ସଂରଚନା vthread_s*thr, ଅଚଳ vvp_vector4_t&val);
ବାହ୍ୟ ଶୂନ୍ୟ vthread_ଠେଲ(ସଂରଚନା vthread_s*thr, ଅଚଳ std::string&val);
ବାହ୍ୟ ଶୂନ୍ୟ vthread_ଠେଲ(ସଂରଚନା vthread_s*thr, ଦ୍ୱିଗୁଣ val);

ବାହ୍ୟ ଶୂନ୍ୟ vthread_କାଢ଼_vec4(ସଂରଚନା vthread_s*thr, unsigned ଗଣନା_2);
ବାହ୍ୟ ଶୂନ୍ୟ vthread_କାଢ଼_str(ସଂରଚନା vthread_s*thr, unsigned ଗଣନା_2);
ବାହ୍ୟ ଶୂନ୍ୟ vthread_କାଢ଼_real(ସଂରଚନା vthread_s*thr, unsigned ଗଣନା_2);


/* Get the string from the requested position in the vthread string
   stack. The top of the stack is depth==0, and items below are
   depth==1, etc. */
ବାହ୍ୟ ଅଚଳ std::string&vthread_get_str_ଗଦା(ସଂରଚନା vthread_s*thr, unsigned depth);
ବାହ୍ୟ ଦ୍ୱିଗୁଣ vthread_get_real_ଗଦା(ସଂରଚନା vthread_s*thr, unsigned depth);
ବାହ୍ୟ ଅଚଳ vvp_vector4_t& vthread_get_vec4_ଗଦା(ସଂରଚନା vthread_s*thr, unsigned depth);

/* This is used to actually delete a thread once we are done with it. */
ବାହ୍ୟ ଶୂନ୍ୟ vthread_delete(vthread_t thr);

#endif /* IVL_vthread_H */
