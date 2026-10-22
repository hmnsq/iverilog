#ifndef IVL_schedule_H
#define IVL_schedule_H
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

# include  "vthread.h"
# include  "vvp_net.h"
# include  "array.h"

/*
 * This causes a thread to be scheduled for execution. The schedule
 * puts the event into the event queue after any existing events for a
 * given time step. The delay is a relative time.
 *
 * If the delay is zero, the push_flag can be used to force the event
 * to the front of the queue. %fork uses this to get the thread
 * execution ahead of non-blocking assignments.
 */
خارجي فراغ schedule_vthread(vthread_t thr, vvp_time64_t delay,
			     bool ادفع_flag =false);

خارجي فراغ schedule_inactive(vthread_t thr);

خارجي فراغ schedule_init_vthread(vthread_t thr);

خارجي فراغ schedule_final_vthread(vthread_t thr);

/*
 * Create an assignment event. The val passed here will be assigned to
 * the specified input when the delay times out. This is scheduled
 * like a non-blocking assignment. This is in fact mostly used to
 * implement the non-blocking assignment.
 */
خارجي فراغ schedule_assign_vector(vvp_net_ptr_t ptr,
				   unsigned base, unsigned vwid,
				   ثابت vvp_vector4_t&val,
				   vvp_time64_t  delay);

خارجي فراغ schedule_assign_array_word(vvp_array_t mem,
				       unsigned word_address,
				       unsigned off,
				       ثابت vvp_vector4_t&val,
				       vvp_time64_t delay);

خارجي فراغ schedule_assign_array_word(vvp_array_t mem,
				       unsigned word_address,
				       مزدوج val,
				       vvp_time64_t delay);

/*
 * Create an event to force the output of a net.
 */
خارجي فراغ schedule_force_vector(vvp_net_t*net,
				  unsigned base, unsigned vwid,
				  ثابت vvp_vector4_t&val,
				  vvp_time64_t delay);

/*
 * Create an event to propagate the output of a net.
 */
خارجي فراغ schedule_propagate_vector(vvp_net_t*ptr,
				      vvp_time64_t delay,
				      ثابت vvp_vector4_t&val);

/*
 * Create an event to propagate the output of an event.
 */
خارجي فراغ schedule_propagate_event(vvp_net_t*ptr,
				     vvp_time64_t delay);

/*
 * This is very similar to schedule_assign_vector, but generates an
 * event in the active queue. It is used at link time to assign a
 * constant value (i.e. C4<...>) to the input of a functor. This
 * creates an event in the active queue.
 */
خارجي فراغ schedule_set_vector(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&val);
خارجي فراغ schedule_set_vector(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&val);
خارجي فراغ schedule_set_vector(vvp_net_ptr_t ptr, مزدوج val);

/*
 * Create a T0 event for always_comb/latch processes. This is the first
 * event in the first inactive region.
 */
خارجي فراغ schedule_t0_trigger(vvp_net_ptr_t ptr);

/*
 * The schedule_init_vector function assigns an initial value to a
 * functor. The assignment is put into a pre-simulation queue that is
 * run before the rest of the simulation runs. This is used to assign
 * initial values to variables and have them propagate through the
 * net. Doing the init before the rest of the scheduler starts
 * prevents threads being triggered by the initialization of the
 * variable, but still allows the initial value to be driven
 * (propagated as events) through the rest of the net.
 */
خارجي فراغ schedule_init_vector(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&val);
خارجي فراغ schedule_init_vector(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&val);
خارجي فراغ schedule_init_vector(vvp_net_ptr_t ptr, مزدوج val);
/*
 * The schedule_init_propagate function is similar to the above but
 * propagates an initial value from a net output (i.e. without passing
 * through the net functor).
 */
خارجي فراغ schedule_init_propagate(vvp_net_t*net, vvp_vector4_t val);
خارجي فراغ schedule_init_propagate(vvp_net_t*net, مزدوج val);

/*
 * Create a generic event. This supports scheduled events that are not
 * any of the specific types above. The vvp_get_event_t points to a
 * function to be executed when the scheduler gets to the event. It is
 * up to the user to allocate/free the vvp_get_event_s object. The
 * object is never referenced by the scheduler after the run method is
 * called.
 *
 * The sync_flag is true if this is intended to be a sync event. These
 * are placed in the stratified event queue after nb assigns. If the
 * ro_flag is true as well, then it is a Read-only sync event, with
 * all that means.
 *
 * If the sync_flag is false, then the event is ACTIVE, and the
 * ro_flag is ignored.
 */

تعريفنوع بنية vvp_gen_event_s *vvp_gen_event_t;

خارجي فراغ schedule_generic(vvp_gen_event_t obj, vvp_time64_t delay,
			     bool sync_flag, bool ro_flag =true,
			     bool delete_obj_when_done =false);

/* Create a functor output event. This is placed in the pre-simulation
 * event queue if the scheduler is still processing pre-simulation
 * events, otherwise it is placed in the stratified event queue as an
 * ACTIVE event with a delay of 0. It is up to the user to allocate/free
 * the vvp_get_event_s object. The object is never referenced by the
 * scheduler after the run method is called.
*/
خارجي فراغ schedule_functor(vvp_gen_event_t obj);

خارجي فراغ schedule_at_start_of_simtime(vvp_gen_event_t obj, vvp_time64_t delay);
خارجي فراغ schedule_at_end_of_simtime(vvp_gen_event_t obj, vvp_time64_t delay);

/* Use this is schedule thread deletion (after rosync). */
خارجي فراغ schedule_del_thr(vthread_t thr);

بنية vvp_gen_event_s
{
      virtual ~vvp_gen_event_s() =0;
      virtual فراغ run_run() =0;
      virtual فراغ single_step_display(فراغ);
};

/*
 * This runs the simulator. It runs until all the functors run out or
 * the simulation is otherwise finished.
 */
خارجي فراغ schedule_simulate(فراغ);

/*
 * Get the current absolute simulation time. This is not used
 * internally by the scheduler (which uses time differences instead)
 * but is used for printouts and stuff.
 */
خارجي vvp_time64_t schedule_simtime(فراغ);

/*
 * Indicate that the simulator is running the rosync callbacks. This is
 * used to prevent the callbacks from performing any write operations
 * in the current simulation time slot.
 */
خارجي bool schedule_at_rosync(فراغ);

/*
 * This function is the equivalent of the $finish system task. It
 * tells the simulator that simulation is done, the current thread
 * should be stopped, all remaining events abandoned and the
 * schedule_simulate() function will return.
 *
 * The schedule_finished() function will return true if the
 * schedule_finish() function has been called.
 */
خارجي فراغ schedule_finish(صحيح rc);
خارجي فراغ schedule_stop(صحيح rc);
خارجي فراغ schedule_single_step(صحيح flag);
خارجي bool schedule_finished(فراغ);
خارجي bool schedule_stopped(فراغ);

/*
 * The scheduler calls this function to process stop events. When this
 * function returns, the simulation resumes.
 */
خارجي فراغ stop_handler(صحيح rc);

/*
 * These are event counters for the sake of performance measurements.
 */
خارجي unsigned long تعداد_assign_events;
خارجي unsigned long تعداد_gen_events;
خارجي unsigned long تعداد_prop_events;
خارجي unsigned long تعداد_thread_events;
خارجي unsigned long تعداد_event_pool;

// Trigger the simulation callbacks.
خارجي فراغ vpiEndOfCompile(فراغ);
خارجي فراغ vpiStartOfSim(فراغ);
خارجي فراغ vpiPostsim(فراغ);
خارجي فراغ vpiNextSimTime(فراغ);

#endif /* IVL_schedule_H */
