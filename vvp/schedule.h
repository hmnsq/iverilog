#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ IVL_ವೇಳಾಪಟ್ಟಿ_H
#ವ್ಯಾಖ್ಯಾನಿಸು IVL_ವೇಳಾಪಟ್ಟಿ_H
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
ಬಾಹ್ಯ ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_vthread(vthread_t thr, vvp_time64_t delay,
			     bool ತಳ್ಳು_flag =false);

ಬಾಹ್ಯ ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_inactive(vthread_t thr);

ಬಾಹ್ಯ ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_init_vthread(vthread_t thr);

ಬಾಹ್ಯ ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_final_vthread(vthread_t thr);

/*
 * Create an assignment event. The val passed here will be assigned to
 * the specified input when the delay times out. This is scheduled
 * like a non-blocking assignment. This is in fact mostly used to
 * implement the non-blocking assignment.
 */
ಬಾಹ್ಯ ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_ನಿಯೋಜಿಸು_ಸದಿಶ(vvp_net_ptr_t ptr,
				   ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid,
				   ಸ್ಥಿರಾಂಕ vvp_vector4_t&val,
				   vvp_time64_t  delay);

ಬಾಹ್ಯ ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_ನಿಯೋಜಿಸು_array_ಶಬ್ದ(vvp_array_t mem,
				       ಚಿಹ್ನೆರಹಿತ ಶಬ್ದ_ವಿಳಾಸ,
				       ಚಿಹ್ನೆರಹಿತ off,
				       ಸ್ಥಿರಾಂಕ vvp_vector4_t&val,
				       vvp_time64_t delay);

ಬಾಹ್ಯ ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_ನಿಯೋಜಿಸು_array_ಶಬ್ದ(vvp_array_t mem,
				       ಚಿಹ್ನೆರಹಿತ ಶಬ್ದ_ವಿಳಾಸ,
				       ದ್ವಿಗುಣ val,
				       vvp_time64_t delay);

/*
 * Create an event to force the output of a net.
 */
ಬಾಹ್ಯ ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_force_ಸದಿಶ(vvp_net_t*net,
				  ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid,
				  ಸ್ಥಿರಾಂಕ vvp_vector4_t&val,
				  vvp_time64_t delay);

/*
 * Create an event to propagate the output of a net.
 */
ಬಾಹ್ಯ ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_propagate_ಸದಿಶ(vvp_net_t*ptr,
				      vvp_time64_t delay,
				      ಸ್ಥಿರಾಂಕ vvp_vector4_t&val);

/*
 * Create an event to propagate the output of an event.
 */
ಬಾಹ್ಯ ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_propagate_event(vvp_net_t*ptr,
				     vvp_time64_t delay);

/*
 * This is very similar to schedule_assign_vector, but generates an
 * event in the active queue. It is used at link time to assign a
 * constant value (i.e. C4<...>) to the input of a functor. This
 * creates an event in the active queue.
 */
ಬಾಹ್ಯ ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_ಗಣ_ಸದಿಶ(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&val);
ಬಾಹ್ಯ ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_ಗಣ_ಸದಿಶ(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector8_t&val);
ಬಾಹ್ಯ ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_ಗಣ_ಸದಿಶ(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ val);

/*
 * Create a T0 event for always_comb/latch processes. This is the first
 * event in the first inactive region.
 */
ಬಾಹ್ಯ ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_t0_trigger(vvp_net_ptr_t ptr);

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
ಬಾಹ್ಯ ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_init_ಸದಿಶ(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&val);
ಬಾಹ್ಯ ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_init_ಸದಿಶ(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector8_t&val);
ಬಾಹ್ಯ ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_init_ಸದಿಶ(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ val);
/*
 * The schedule_init_propagate function is similar to the above but
 * propagates an initial value from a net output (i.e. without passing
 * through the net functor).
 */
ಬಾಹ್ಯ ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_init_propagate(vvp_net_t*net, vvp_vector4_t val);
ಬಾಹ್ಯ ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_init_propagate(vvp_net_t*net, ದ್ವಿಗುಣ val);

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

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ vvp_gen_event_s *vvp_gen_event_t;

ಬಾಹ್ಯ ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_generic(vvp_gen_event_t obj, vvp_time64_t delay,
			     bool sync_flag, bool ro_flag =true,
			     bool delete_obj_when_ಮುಗಿದ =false);

/* Create a functor output event. This is placed in the pre-simulation
 * event queue if the scheduler is still processing pre-simulation
 * events, otherwise it is placed in the stratified event queue as an
 * ACTIVE event with a delay of 0. It is up to the user to allocate/free
 * the vvp_get_event_s object. The object is never referenced by the
 * scheduler after the run method is called.
*/
ಬಾಹ್ಯ ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_functor(vvp_gen_event_t obj);

ಬಾಹ್ಯ ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_at_ಆರಂಭ_ದ_simtime(vvp_gen_event_t obj, vvp_time64_t delay);
ಬಾಹ್ಯ ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_at_ಕೊನೆ_ದ_simtime(vvp_gen_event_t obj, vvp_time64_t delay);

/* Use this is schedule thread deletion (after rosync). */
ಬಾಹ್ಯ ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_del_thr(vthread_t thr);

ರಚನೆ vvp_gen_event_s
{
      ವಾಸ್ತವಿಕ ~vvp_gen_event_s() =0;
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಸತತ_ಸತತ() =0;
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ single_ಹೆಜ್ಜೆ_display(ಶೂನ್ಯ);
};

/*
 * This runs the simulator. It runs until all the functors run out or
 * the simulation is otherwise finished.
 */
ಬಾಹ್ಯ ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_simulate(ಶೂನ್ಯ);

/*
 * Get the current absolute simulation time. This is not used
 * internally by the scheduler (which uses time differences instead)
 * but is used for printouts and stuff.
 */
ಬಾಹ್ಯ vvp_time64_t ವೇಳಾಪಟ್ಟಿ_simtime(ಶೂನ್ಯ);

/*
 * Indicate that the simulator is running the rosync callbacks. This is
 * used to prevent the callbacks from performing any write operations
 * in the current simulation time slot.
 */
ಬಾಹ್ಯ bool ವೇಳಾಪಟ್ಟಿ_at_rosync(ಶೂನ್ಯ);

/*
 * This function is the equivalent of the $finish system task. It
 * tells the simulator that simulation is done, the current thread
 * should be stopped, all remaining events abandoned and the
 * schedule_simulate() function will return.
 *
 * The schedule_finished() function will return true if the
 * schedule_finish() function has been called.
 */
ಬಾಹ್ಯ ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_ಮುಕ್ತಾಯ(ಪೂರ್ಣಾಂಕ rc);
ಬಾಹ್ಯ ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_stop(ಪೂರ್ಣಾಂಕ rc);
ಬಾಹ್ಯ ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_single_ಹೆಜ್ಜೆ(ಪೂರ್ಣಾಂಕ flag);
ಬಾಹ್ಯ bool ವೇಳಾಪಟ್ಟಿ_ಪೂರ್ಣಗೊಂಡ(ಶೂನ್ಯ);
ಬಾಹ್ಯ bool ವೇಳಾಪಟ್ಟಿ_stopped(ಶೂನ್ಯ);

/*
 * The scheduler calls this function to process stop events. When this
 * function returns, the simulation resumes.
 */
ಬಾಹ್ಯ ಶೂನ್ಯ stop_handler(ಪೂರ್ಣಾಂಕ rc);

/*
 * These are event counters for the sake of performance measurements.
 */
ಬಾಹ್ಯ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_ನಿಯೋಜಿಸು_events;
ಬಾಹ್ಯ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_gen_events;
ಬಾಹ್ಯ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_prop_events;
ಬಾಹ್ಯ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_thread_events;
ಬಾಹ್ಯ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_event_pool;

// Trigger the simulation callbacks.
ಬಾಹ್ಯ ಶೂನ್ಯ vpi_ಕೊನೆ_ದ_Compile(ಶೂನ್ಯ);
ಬಾಹ್ಯ ಶೂನ್ಯ vpi_ಆರಂಭ_ದ_Sim(ಶೂನ್ಯ);
ಬಾಹ್ಯ ಶೂನ್ಯ vpiPostsim(ಶೂನ್ಯ);
ಬಾಹ್ಯ ಶೂನ್ಯ vpi_ಮುಂದಿನ_Sim_ಸಮಯ(ಶೂನ್ಯ);

#ಆದರೆ_ಅಂತ್ಯ /* IVL_schedule_H */
