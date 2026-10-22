#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ IVL_ਸਮਾਂ_ਸਾਰਣੀ_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IVL_ਸਮਾਂ_ਸਾਰਣੀ_H
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
ਬਾਹਰੀ ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_vthread(vthread_t thr, vvp_time64_t delay,
			     bool ਧੱਕੋ_flag =false);

ਬਾਹਰੀ ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_inactive(vthread_t thr);

ਬਾਹਰੀ ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_init_vthread(vthread_t thr);

ਬਾਹਰੀ ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_final_vthread(vthread_t thr);

/*
 * Create an assignment event. The val passed here will be assigned to
 * the specified input when the delay times out. This is scheduled
 * like a non-blocking assignment. This is in fact mostly used to
 * implement the non-blocking assignment.
 */
ਬਾਹਰੀ ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_ਸੌਂਪੋ_ਸਦਿਸ਼(vvp_net_ptr_t ptr,
				   ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid,
				   ਅਚਲ vvp_vector4_t&val,
				   vvp_time64_t  delay);

ਬਾਹਰੀ ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_ਸੌਂਪੋ_array_ਸ਼ਬਦ(vvp_array_t mem,
				       ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ_ਪਤਾ,
				       ਬਿਨਾਨਿਸ਼ਾਨੀ off,
				       ਅਚਲ vvp_vector4_t&val,
				       vvp_time64_t delay);

ਬਾਹਰੀ ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_ਸੌਂਪੋ_array_ਸ਼ਬਦ(vvp_array_t mem,
				       ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ_ਪਤਾ,
				       ਦੁੱਗਣਾ val,
				       vvp_time64_t delay);

/*
 * Create an event to force the output of a net.
 */
ਬਾਹਰੀ ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_force_ਸਦਿਸ਼(vvp_net_t*net,
				  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid,
				  ਅਚਲ vvp_vector4_t&val,
				  vvp_time64_t delay);

/*
 * Create an event to propagate the output of a net.
 */
ਬਾਹਰੀ ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_propagate_ਸਦਿਸ਼(vvp_net_t*ptr,
				      vvp_time64_t delay,
				      ਅਚਲ vvp_vector4_t&val);

/*
 * Create an event to propagate the output of an event.
 */
ਬਾਹਰੀ ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_propagate_event(vvp_net_t*ptr,
				     vvp_time64_t delay);

/*
 * This is very similar to schedule_assign_vector, but generates an
 * event in the active queue. It is used at link time to assign a
 * constant value (i.e. C4<...>) to the input of a functor. This
 * creates an event in the active queue.
 */
ਬਾਹਰੀ ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_ਸੈੱਟ_ਸਦਿਸ਼(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&val);
ਬਾਹਰੀ ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_ਸੈੱਟ_ਸਦਿਸ਼(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector8_t&val);
ਬਾਹਰੀ ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_ਸੈੱਟ_ਸਦਿਸ਼(vvp_net_ptr_t ptr, ਦੁੱਗਣਾ val);

/*
 * Create a T0 event for always_comb/latch processes. This is the first
 * event in the first inactive region.
 */
ਬਾਹਰੀ ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_t0_trigger(vvp_net_ptr_t ptr);

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
ਬਾਹਰੀ ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_init_ਸਦਿਸ਼(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&val);
ਬਾਹਰੀ ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_init_ਸਦਿਸ਼(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector8_t&val);
ਬਾਹਰੀ ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_init_ਸਦਿਸ਼(vvp_net_ptr_t ptr, ਦੁੱਗਣਾ val);
/*
 * The schedule_init_propagate function is similar to the above but
 * propagates an initial value from a net output (i.e. without passing
 * through the net functor).
 */
ਬਾਹਰੀ ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_init_propagate(vvp_net_t*net, vvp_vector4_t val);
ਬਾਹਰੀ ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_init_propagate(vvp_net_t*net, ਦੁੱਗਣਾ val);

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

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ vvp_gen_event_s *vvp_gen_event_t;

ਬਾਹਰੀ ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_generic(vvp_gen_event_t obj, vvp_time64_t delay,
			     bool sync_flag, bool ro_flag =true,
			     bool delete_obj_when_ਪੂਰਾ =false);

/* Create a functor output event. This is placed in the pre-simulation
 * event queue if the scheduler is still processing pre-simulation
 * events, otherwise it is placed in the stratified event queue as an
 * ACTIVE event with a delay of 0. It is up to the user to allocate/free
 * the vvp_get_event_s object. The object is never referenced by the
 * scheduler after the run method is called.
*/
ਬਾਹਰੀ ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_functor(vvp_gen_event_t obj);

ਬਾਹਰੀ ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_at_ਸ਼ੁਰੂ_ਦਾ_simtime(vvp_gen_event_t obj, vvp_time64_t delay);
ਬਾਹਰੀ ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_at_ਸਿਰਾ_ਦਾ_simtime(vvp_gen_event_t obj, vvp_time64_t delay);

/* Use this is schedule thread deletion (after rosync). */
ਬਾਹਰੀ ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_del_thr(vthread_t thr);

ਬਣਤਰ vvp_gen_event_s
{
      ਆਭਾਸੀ ~vvp_gen_event_s() =0;
      ਆਭਾਸੀ ਖਾਲੀ ਲੜੀਵਾਰ_ਲੜੀਵਾਰ() =0;
      ਆਭਾਸੀ ਖਾਲੀ single_ਕਦਮ_display(ਖਾਲੀ);
};

/*
 * This runs the simulator. It runs until all the functors run out or
 * the simulation is otherwise finished.
 */
ਬਾਹਰੀ ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_simulate(ਖਾਲੀ);

/*
 * Get the current absolute simulation time. This is not used
 * internally by the scheduler (which uses time differences instead)
 * but is used for printouts and stuff.
 */
ਬਾਹਰੀ vvp_time64_t ਸਮਾਂ_ਸਾਰਣੀ_simtime(ਖਾਲੀ);

/*
 * Indicate that the simulator is running the rosync callbacks. This is
 * used to prevent the callbacks from performing any write operations
 * in the current simulation time slot.
 */
ਬਾਹਰੀ bool ਸਮਾਂ_ਸਾਰਣੀ_at_rosync(ਖਾਲੀ);

/*
 * This function is the equivalent of the $finish system task. It
 * tells the simulator that simulation is done, the current thread
 * should be stopped, all remaining events abandoned and the
 * schedule_simulate() function will return.
 *
 * The schedule_finished() function will return true if the
 * schedule_finish() function has been called.
 */
ਬਾਹਰੀ ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_ਅੰਤ(ਪੂਰਨ rc);
ਬਾਹਰੀ ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_stop(ਪੂਰਨ rc);
ਬਾਹਰੀ ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_single_ਕਦਮ(ਪੂਰਨ flag);
ਬਾਹਰੀ bool ਸਮਾਂ_ਸਾਰਣੀ_ਮੁਕੰਮਲ(ਖਾਲੀ);
ਬਾਹਰੀ bool ਸਮਾਂ_ਸਾਰਣੀ_stopped(ਖਾਲੀ);

/*
 * The scheduler calls this function to process stop events. When this
 * function returns, the simulation resumes.
 */
ਬਾਹਰੀ ਖਾਲੀ stop_handler(ਪੂਰਨ rc);

/*
 * These are event counters for the sake of performance measurements.
 */
ਬਾਹਰੀ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਗਿਣਤੀ_ਸੌਂਪੋ_events;
ਬਾਹਰੀ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਗਿਣਤੀ_gen_events;
ਬਾਹਰੀ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਗਿਣਤੀ_prop_events;
ਬਾਹਰੀ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਗਿਣਤੀ_thread_events;
ਬਾਹਰੀ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਗਿਣਤੀ_event_pool;

// Trigger the simulation callbacks.
ਬਾਹਰੀ ਖਾਲੀ vpi_ਸਿਰਾ_ਦਾ_Compile(ਖਾਲੀ);
ਬਾਹਰੀ ਖਾਲੀ vpi_ਸ਼ੁਰੂ_ਦਾ_Sim(ਖਾਲੀ);
ਬਾਹਰੀ ਖਾਲੀ vpiPostsim(ਖਾਲੀ);
ਬਾਹਰੀ ਖਾਲੀ vpi_ਅਗਲਾ_Sim_ਸਮਾਂ(ਖਾਲੀ);

#ਜੇ_ਅੰਤ /* IVL_schedule_H */
