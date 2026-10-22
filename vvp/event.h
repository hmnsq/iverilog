#ifndef IVL_event_H
#define IVL_event_H
/*
 * Copyright (c) 2004-2025 Stephen Williams (steve@icarus.com)
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
# include  "array.h"
# include  "vthread.h"
# include  "config.h"

class evctl {

    ಸಾರ್ವಜನಿಕ:
      explicit evctl(uint64_t ecount);
      bool dec_and_run();
      ಆಭಾಸ ಶೂನ್ಯ run_run() = 0;
      ಆಭಾಸ ~evctl() {}
      evctl*next;

    ಗುಪ್ತ:
      uint64_t ecount_;
};

class evctl_real : ಸಾರ್ವಜನಿಕ evctl {

    ಸಾರ್ವಜನಿಕ:
      explicit evctl_real(class __vpiHandle*handle, ದ್ವಿಗುಣ ಮೌಲ್ಯ,
                          uint64_t ecount);
      ಆಭಾಸ ~evctl_real() override {}
      ಶೂನ್ಯ run_run() override;

    ಗುಪ್ತ:
      class __vpiHandle*handle_;
      ದ್ವಿಗುಣ ಮೌಲ್ಯ_2;
};

class evctl_vector : ಸಾರ್ವಜನಿಕ evctl {

    ಸಾರ್ವಜನಿಕ:
      explicit evctl_vector(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&ಮೌಲ್ಯ,
                            unsigned off, unsigned wid, uint64_t ecount);
      ಆಭಾಸ ~evctl_vector() override {}
      ಶೂನ್ಯ run_run() override;

    ಗುಪ್ತ:
      vvp_net_ptr_t ptr_;
      vvp_vector4_t ಮೌಲ್ಯ_2;
      unsigned off_;
      unsigned wid_;
};

class evctl_array : ಸಾರ್ವಜನಿಕ evctl {

    ಸಾರ್ವಜನಿಕ:
      explicit evctl_array(vvp_array_t memory, unsigned index,
                           ಧ್ರುವ vvp_vector4_t&ಮೌಲ್ಯ, unsigned off,
                           uint64_t ecount);
      ಆಭಾಸ ~evctl_array() override {}
      ಆಭಾಸ ಶೂನ್ಯ run_run() override;

    ಗುಪ್ತ:
      vvp_array_t mem_;
      unsigned idx_;
      vvp_vector4_t ಮೌಲ್ಯ_2;
      unsigned off_;
};

class evctl_array_r : ಸಾರ್ವಜನಿಕ evctl {

    ಸಾರ್ವಜನಿಕ:
      explicit evctl_array_r(vvp_array_t memory, unsigned index,
                           ದ್ವಿಗುಣ ಮೌಲ್ಯ, uint64_t ecount);
      ಆಭಾಸ ~evctl_array_r() override {}
      ಆಭಾಸ ಶೂನ್ಯ run_run() override;

    ಗುಪ್ತ:
      vvp_array_t mem_;
      unsigned idx_;
      ದ್ವಿಗುಣ ಮೌಲ್ಯ_2;
};

ಬಾಹ್ಯ ಶೂನ್ಯ schedule_evctl(class __vpiHandle*handle, ದ್ವಿಗುಣ ಮೌಲ್ಯ,
                           vvp_net_t*event, uint64_t ecount);

ಬಾಹ್ಯ ಶೂನ್ಯ schedule_evctl(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&ಮೌಲ್ಯ,
                           unsigned offset, unsigned wid,
                           vvp_net_t*event, uint64_t ecount);

ಬಾಹ್ಯ ಶೂನ್ಯ schedule_evctl(vvp_array_t memory, unsigned index,
                           ಧ್ರುವ vvp_vector4_t&ಮೌಲ್ಯ, unsigned offset,
                           vvp_net_t*event, uint64_t ecount);

ಬಾಹ್ಯ ಶೂನ್ಯ schedule_evctl(vvp_array_t memory, unsigned index,
                           ದ್ವಿಗುಣ ಮೌಲ್ಯ,
                           vvp_net_t*event, uint64_t ecount);

/*
 *  Event / edge detection functors
 */

/*
 * A "waitable" functor is one that the %wait instruction can wait
 * on. This includes the infrastructure needed to hold threads.
 */
ಸಂರಚನಾ waitable_hooks_s {

    ಸಾರ್ವಜನಿಕ:
      waitable_hooks_s() : event_ctls(0) { last = &event_ctls; }
      ಆಭಾಸ ~waitable_hooks_s() {}

      ಆಭಾಸ vthread_t add_waiting_thread(vthread_t thread) = 0;

      evctl*event_ctls;
      evctl**last;

    ರಕ್ಷಿತ:
      ಶೂನ್ಯ run_waiting_threads_(vthread_t&threads);
};

/*
 * This is the base object for storing state information for each instance
 * of an automatically allocated event. In the general case, all that is
 * needed is the list of threads waiting on that instance.
 */
ಸಂರಚನಾ waitable_state_s {
      waitable_state_s() : threads(0) {}

      vthread_t threads;
};

/*
 * The vvp_fun_edge functor detects events that are edges of various
 * types. This should be hooked to a vvp_net_t that is connected to
 * the output of a signal that we wish to watch for edges.
 */
class vvp_fun_edge : ಸಾರ್ವಜನಿಕ vvp_net_fun_t, ಸಾರ್ವಜನಿಕ waitable_hooks_s {

    ಸಾರ್ವಜನಿಕ:
      ಪ್ರಕಾರನಿರ್ದೇಶ unsigned short edge_t;
      explicit vvp_fun_edge(edge_t e);
      ಆಭಾಸ ~vvp_fun_edge() override;

    ರಕ್ಷಿತ:
      bool recv_vec4_(ಧ್ರುವ vvp_vector4_t&bit,
                      vvp_bit4_t&old_bit, vthread_t&threads);

      vvp_bit4_t bits_[4];

    ಗುಪ್ತ:
      edge_t edge_;
};

ಬಾಹ್ಯ ಧ್ರುವ vvp_fun_edge::edge_t vvp_edge_edge;
ಬಾಹ್ಯ ಧ್ರುವ vvp_fun_edge::edge_t vvp_edge_posedge;
ಬಾಹ್ಯ ಧ್ರುವ vvp_fun_edge::edge_t vvp_edge_negedge;
ಬಾಹ್ಯ ಧ್ರುವ vvp_fun_edge::edge_t vvp_edge_none;

/*
 * Statically allocated vvp_fun_edge.
 */
class vvp_fun_edge_sa : ಸಾರ್ವಜನಿಕ vvp_fun_edge {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_edge_sa(edge_t e);
      ಆಭಾಸ ~vvp_fun_edge_sa() override;

      vthread_t add_waiting_thread(vthread_t thread) override;

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
                     vvp_context_t context) override;
      ಶೂನ್ಯ recv_vec4_pv(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t context) override;

    ಗುಪ್ತ:
      vthread_t threads_;
};

/*
 * Automatically allocated vvp_fun_edge.
 */
class vvp_fun_edge_aa : ಸಾರ್ವಜನಿಕ vvp_fun_edge, ಸಾರ್ವಜನಿಕ automatic_hooks_s {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_edge_aa(edge_t e);
      ಆಭಾಸ ~vvp_fun_edge_aa() override;

      ಶೂನ್ಯ alloc_instance(vvp_context_t context) override;
      ಶೂನ್ಯ reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      ಶೂನ್ಯ free_instance(vvp_context_t context) override;
#endif

      vthread_t add_waiting_thread(vthread_t thread) override;

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
                     vvp_context_t context) override;

    ಗುಪ್ತ:
      __vpiScope*context_scope_;
      unsigned context_idx_;
};

/*
 * This is the base object for storing the last received values for a
 * vvp_fun_anyedge functor.
 */
class anyedge_ಮೌಲ್ಯ;

/*
 * The vvp_fun_anyedge functor checks to see if any value in an input
 * vector changes. Unlike the vvp_fun_edge, which watches for the LSB
 * of its inputs to change in a particular direction, the anyedge
 * functor looks at the entire input vector for any change.
 *
 * The anyedge is also different in that it can receive real
 * values. In this case, any detectable change in the real value leads
 * to an event trigger.
 */
class vvp_fun_anyedge : ಸಾರ್ವಜನಿಕ vvp_net_fun_t, ಸಾರ್ವಜನಿಕ waitable_hooks_s {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_anyedge();
      ಆಭಾಸ ~vvp_fun_anyedge() override;

    ರಕ್ಷಿತ:
      anyedge_ಮೌಲ್ಯ*last_ಮೌಲ್ಯ[4];
};

/*
 * Statically allocated vvp_fun_anyedge.
 */
class vvp_fun_anyedge_sa : ಸಾರ್ವಜನಿಕ vvp_fun_anyedge {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_anyedge_sa();
      ಆಭಾಸ ~vvp_fun_anyedge_sa() override;

      vthread_t add_waiting_thread(vthread_t thread) override;

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
                     vvp_context_t context) override;
      ಶೂನ್ಯ recv_vec4_pv(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t context) override;

      ಶೂನ್ಯ recv_real(vvp_net_ptr_t port, ದ್ವಿಗುಣ bit,
                     vvp_context_t context) override;

      ಶೂನ್ಯ recv_string(vvp_net_ptr_t port, ಧ್ರುವ std::string&bit,
		       vvp_context_t context) override;
      ಶೂನ್ಯ recv_object(vvp_net_ptr_t port, vvp_object_t bit,
		       vvp_context_t context) override;

    ಗುಪ್ತ:
      vthread_t threads_;
};

/*
 * Automatically allocated vvp_fun_anyedge.
 */
class vvp_fun_anyedge_aa : ಸಾರ್ವಜನಿಕ vvp_fun_anyedge, ಸಾರ್ವಜನಿಕ automatic_hooks_s {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_anyedge_aa();
      ಆಭಾಸ ~vvp_fun_anyedge_aa() override;

      ಶೂನ್ಯ alloc_instance(vvp_context_t context) override;
      ಶೂನ್ಯ reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      ಶೂನ್ಯ free_instance(vvp_context_t context) override;
#endif

      vthread_t add_waiting_thread(vthread_t thread) override;

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
                     vvp_context_t context) override;

      ಶೂನ್ಯ recv_real(vvp_net_ptr_t port, ದ್ವಿಗುಣ bit,
                     vvp_context_t context) override;

      ಶೂನ್ಯ recv_string(vvp_net_ptr_t port, ಧ್ರುವ std::string&bit,
		       vvp_context_t context) override;

    ಗುಪ್ತ:
      __vpiScope*context_scope_;
      unsigned context_idx_;
};

/*
 * This functor triggers anytime any input is set, no matter what the
 * value. This is similar to a named event, but it has no handle. It
 * supports wide (more than 4) inputs, so needs a pointer to the base
 * net that is used for output.
 */
class vvp_fun_event_or : ಸಾರ್ವಜನಿಕ vvp_net_fun_t, ಸಾರ್ವಜನಿಕ waitable_hooks_s {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_event_or(vvp_net_t*base_net);
      ~vvp_fun_event_or() override;

    ರಕ್ಷಿತ:
      vvp_net_t*base_net_;
};

/*
 * Statically allocated vvp_fun_event_or.
 */
class vvp_fun_event_or_sa : ಸಾರ್ವಜನಿಕ vvp_fun_event_or {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_event_or_sa(vvp_net_t*base_net);
      ~vvp_fun_event_or_sa() override;

      vthread_t add_waiting_thread(vthread_t thread) override;

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
                     vvp_context_t context) override;

    ಗುಪ್ತ:
      vthread_t threads_;
};

/*
 * Automatically allocated vvp_fun_event_or.
 */
class vvp_fun_event_or_aa : ಸಾರ್ವಜನಿಕ vvp_fun_event_or, ಸಾರ್ವಜನಿಕ automatic_hooks_s {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_event_or_aa(vvp_net_t*base_net);
      ~vvp_fun_event_or_aa() override;

      ಶೂನ್ಯ alloc_instance(vvp_context_t context) override;
      ಶೂನ್ಯ reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      ಶೂನ್ಯ free_instance(vvp_context_t context) override;
#endif

      vthread_t add_waiting_thread(vthread_t thread) override;

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
                     vvp_context_t context) override;

    ಗುಪ್ತ:
      __vpiScope*context_scope_;
      unsigned context_idx_;
};

/*
 * A named event is simpler than a vvp_fun_edge in that it triggers on
 * any input at all to port-0. The idea here is that behavioral code
 * can use a %set/v instruction to trigger the event.
 */
class vvp_named_event : ಸಾರ್ವಜನಿಕ vvp_net_fun_t, ಸಾರ್ವಜನಿಕ waitable_hooks_s {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_named_event(class __vpiHandle*eh);
      ~vvp_named_event() override;

    ರಕ್ಷಿತ:
      class __vpiHandle*handle_;
};

/*
 * Statically allocated vvp_named_event.
 */
class vvp_named_event_sa : ಸಾರ್ವಜನಿಕ vvp_named_event {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_named_event_sa(class __vpiHandle*eh);
      ~vvp_named_event_sa() override;

      vthread_t add_waiting_thread(vthread_t thread) override;

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
                     vvp_context_t) override;

    ಗುಪ್ತ:
      vthread_t threads_;
};

/*
 * Automatically allocated vvp_named_event.
 */
class vvp_named_event_aa : ಸಾರ್ವಜನಿಕ vvp_named_event, ಸಾರ್ವಜನಿಕ automatic_hooks_s {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_named_event_aa(class __vpiHandle*eh);
      ~vvp_named_event_aa() override;

      ಶೂನ್ಯ alloc_instance(vvp_context_t context) override;
      ಶೂನ್ಯ reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      ಶೂನ್ಯ free_instance(vvp_context_t context) override;
#endif

      vthread_t add_waiting_thread(vthread_t thread) override;

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
                     vvp_context_t context) override;

    ಗುಪ್ತ:
      unsigned context_idx_;
};

#endif /* IVL_event_H */
