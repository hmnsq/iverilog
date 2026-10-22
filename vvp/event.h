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

    عمومی:
      explicit evctl(uint64_t ecount);
      bool dec_and_run();
      مجازی تهی run_run() = 0;
      مجازی ~evctl() {}
      evctl*next;

    خصوصی:
      uint64_t ecount_;
};

class evctl_real : عمومی evctl {

    عمومی:
      explicit evctl_real(class __vpiHandle*handle, دوگانه مقدار,
                          uint64_t ecount);
      مجازی ~evctl_real() override {}
      تهی run_run() override;

    خصوصی:
      class __vpiHandle*handle_;
      دوگانه مقدار_2;
};

class evctl_vector : عمومی evctl {

    عمومی:
      explicit evctl_vector(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&مقدار,
                            unsigned off, unsigned wid, uint64_t ecount);
      مجازی ~evctl_vector() override {}
      تهی run_run() override;

    خصوصی:
      vvp_net_ptr_t ptr_;
      vvp_vector4_t مقدار_2;
      unsigned off_;
      unsigned wid_;
};

class evctl_array : عمومی evctl {

    عمومی:
      explicit evctl_array(vvp_array_t memory, unsigned index,
                           ثابت vvp_vector4_t&مقدار, unsigned off,
                           uint64_t ecount);
      مجازی ~evctl_array() override {}
      مجازی تهی run_run() override;

    خصوصی:
      vvp_array_t mem_;
      unsigned idx_;
      vvp_vector4_t مقدار_2;
      unsigned off_;
};

class evctl_array_r : عمومی evctl {

    عمومی:
      explicit evctl_array_r(vvp_array_t memory, unsigned index,
                           دوگانه مقدار, uint64_t ecount);
      مجازی ~evctl_array_r() override {}
      مجازی تهی run_run() override;

    خصوصی:
      vvp_array_t mem_;
      unsigned idx_;
      دوگانه مقدار_2;
};

بیرونی تهی schedule_evctl(class __vpiHandle*handle, دوگانه مقدار,
                           vvp_net_t*event, uint64_t ecount);

بیرونی تهی schedule_evctl(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&مقدار,
                           unsigned offset, unsigned wid,
                           vvp_net_t*event, uint64_t ecount);

بیرونی تهی schedule_evctl(vvp_array_t memory, unsigned index,
                           ثابت vvp_vector4_t&مقدار, unsigned offset,
                           vvp_net_t*event, uint64_t ecount);

بیرونی تهی schedule_evctl(vvp_array_t memory, unsigned index,
                           دوگانه مقدار,
                           vvp_net_t*event, uint64_t ecount);

/*
 *  Event / edge detection functors
 */

/*
 * A "waitable" functor is one that the %wait instruction can wait
 * on. This includes the infrastructure needed to hold threads.
 */
ساختار waitable_hooks_s {

    عمومی:
      waitable_hooks_s() : event_ctls(0) { last = &event_ctls; }
      مجازی ~waitable_hooks_s() {}

      مجازی vthread_t add_waiting_thread(vthread_t thread) = 0;

      evctl*event_ctls;
      evctl**last;

    محافظتشده:
      تهی run_waiting_threads_(vthread_t&threads);
};

/*
 * This is the base object for storing state information for each instance
 * of an automatically allocated event. In the general case, all that is
 * needed is the list of threads waiting on that instance.
 */
ساختار waitable_state_s {
      waitable_state_s() : threads(0) {}

      vthread_t threads;
};

/*
 * The vvp_fun_edge functor detects events that are edges of various
 * types. This should be hooked to a vvp_net_t that is connected to
 * the output of a signal that we wish to watch for edges.
 */
class vvp_fun_edge : عمومی vvp_net_fun_t, عمومی waitable_hooks_s {

    عمومی:
      تعریفنوع unsigned short edge_t;
      explicit vvp_fun_edge(edge_t e);
      مجازی ~vvp_fun_edge() override;

    محافظتشده:
      bool recv_vec4_(ثابت vvp_vector4_t&bit,
                      vvp_bit4_t&old_bit, vthread_t&threads);

      vvp_bit4_t bits_[4];

    خصوصی:
      edge_t edge_;
};

بیرونی ثابت vvp_fun_edge::edge_t vvp_edge_edge;
بیرونی ثابت vvp_fun_edge::edge_t vvp_edge_posedge;
بیرونی ثابت vvp_fun_edge::edge_t vvp_edge_negedge;
بیرونی ثابت vvp_fun_edge::edge_t vvp_edge_none;

/*
 * Statically allocated vvp_fun_edge.
 */
class vvp_fun_edge_sa : عمومی vvp_fun_edge {

    عمومی:
      explicit vvp_fun_edge_sa(edge_t e);
      مجازی ~vvp_fun_edge_sa() override;

      vthread_t add_waiting_thread(vthread_t thread) override;

      تهی recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                     vvp_context_t context) override;
      تهی recv_vec4_pv(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t context) override;

    خصوصی:
      vthread_t threads_;
};

/*
 * Automatically allocated vvp_fun_edge.
 */
class vvp_fun_edge_aa : عمومی vvp_fun_edge, عمومی automatic_hooks_s {

    عمومی:
      explicit vvp_fun_edge_aa(edge_t e);
      مجازی ~vvp_fun_edge_aa() override;

      تهی alloc_instance(vvp_context_t context) override;
      تهی reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      تهی free_instance(vvp_context_t context) override;
#endif

      vthread_t add_waiting_thread(vthread_t thread) override;

      تهی recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                     vvp_context_t context) override;

    خصوصی:
      __vpiScope*context_scope_;
      unsigned context_idx_;
};

/*
 * This is the base object for storing the last received values for a
 * vvp_fun_anyedge functor.
 */
class anyedge_مقدار;

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
class vvp_fun_anyedge : عمومی vvp_net_fun_t, عمومی waitable_hooks_s {

    عمومی:
      explicit vvp_fun_anyedge();
      مجازی ~vvp_fun_anyedge() override;

    محافظتشده:
      anyedge_مقدار*last_مقدار[4];
};

/*
 * Statically allocated vvp_fun_anyedge.
 */
class vvp_fun_anyedge_sa : عمومی vvp_fun_anyedge {

    عمومی:
      explicit vvp_fun_anyedge_sa();
      مجازی ~vvp_fun_anyedge_sa() override;

      vthread_t add_waiting_thread(vthread_t thread) override;

      تهی recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                     vvp_context_t context) override;
      تهی recv_vec4_pv(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t context) override;

      تهی recv_real(vvp_net_ptr_t port, دوگانه bit,
                     vvp_context_t context) override;

      تهی recv_string(vvp_net_ptr_t port, ثابت std::string&bit,
		       vvp_context_t context) override;
      تهی recv_object(vvp_net_ptr_t port, vvp_object_t bit,
		       vvp_context_t context) override;

    خصوصی:
      vthread_t threads_;
};

/*
 * Automatically allocated vvp_fun_anyedge.
 */
class vvp_fun_anyedge_aa : عمومی vvp_fun_anyedge, عمومی automatic_hooks_s {

    عمومی:
      explicit vvp_fun_anyedge_aa();
      مجازی ~vvp_fun_anyedge_aa() override;

      تهی alloc_instance(vvp_context_t context) override;
      تهی reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      تهی free_instance(vvp_context_t context) override;
#endif

      vthread_t add_waiting_thread(vthread_t thread) override;

      تهی recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                     vvp_context_t context) override;

      تهی recv_real(vvp_net_ptr_t port, دوگانه bit,
                     vvp_context_t context) override;

      تهی recv_string(vvp_net_ptr_t port, ثابت std::string&bit,
		       vvp_context_t context) override;

    خصوصی:
      __vpiScope*context_scope_;
      unsigned context_idx_;
};

/*
 * This functor triggers anytime any input is set, no matter what the
 * value. This is similar to a named event, but it has no handle. It
 * supports wide (more than 4) inputs, so needs a pointer to the base
 * net that is used for output.
 */
class vvp_fun_event_or : عمومی vvp_net_fun_t, عمومی waitable_hooks_s {

    عمومی:
      explicit vvp_fun_event_or(vvp_net_t*base_net);
      ~vvp_fun_event_or() override;

    محافظتشده:
      vvp_net_t*base_net_;
};

/*
 * Statically allocated vvp_fun_event_or.
 */
class vvp_fun_event_or_sa : عمومی vvp_fun_event_or {

    عمومی:
      explicit vvp_fun_event_or_sa(vvp_net_t*base_net);
      ~vvp_fun_event_or_sa() override;

      vthread_t add_waiting_thread(vthread_t thread) override;

      تهی recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                     vvp_context_t context) override;

    خصوصی:
      vthread_t threads_;
};

/*
 * Automatically allocated vvp_fun_event_or.
 */
class vvp_fun_event_or_aa : عمومی vvp_fun_event_or, عمومی automatic_hooks_s {

    عمومی:
      explicit vvp_fun_event_or_aa(vvp_net_t*base_net);
      ~vvp_fun_event_or_aa() override;

      تهی alloc_instance(vvp_context_t context) override;
      تهی reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      تهی free_instance(vvp_context_t context) override;
#endif

      vthread_t add_waiting_thread(vthread_t thread) override;

      تهی recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                     vvp_context_t context) override;

    خصوصی:
      __vpiScope*context_scope_;
      unsigned context_idx_;
};

/*
 * A named event is simpler than a vvp_fun_edge in that it triggers on
 * any input at all to port-0. The idea here is that behavioral code
 * can use a %set/v instruction to trigger the event.
 */
class vvp_named_event : عمومی vvp_net_fun_t, عمومی waitable_hooks_s {

    عمومی:
      explicit vvp_named_event(class __vpiHandle*eh);
      ~vvp_named_event() override;

    محافظتشده:
      class __vpiHandle*handle_;
};

/*
 * Statically allocated vvp_named_event.
 */
class vvp_named_event_sa : عمومی vvp_named_event {

    عمومی:
      explicit vvp_named_event_sa(class __vpiHandle*eh);
      ~vvp_named_event_sa() override;

      vthread_t add_waiting_thread(vthread_t thread) override;

      تهی recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                     vvp_context_t) override;

    خصوصی:
      vthread_t threads_;
};

/*
 * Automatically allocated vvp_named_event.
 */
class vvp_named_event_aa : عمومی vvp_named_event, عمومی automatic_hooks_s {

    عمومی:
      explicit vvp_named_event_aa(class __vpiHandle*eh);
      ~vvp_named_event_aa() override;

      تهی alloc_instance(vvp_context_t context) override;
      تهی reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      تهی free_instance(vvp_context_t context) override;
#endif

      vthread_t add_waiting_thread(vthread_t thread) override;

      تهی recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                     vvp_context_t context) override;

    خصوصی:
      unsigned context_idx_;
};

#endif /* IVL_event_H */
