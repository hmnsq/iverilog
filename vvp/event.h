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

    সার্বজনিক:
      explicit evctl(uint64_t ecount);
      bool dec_and_run();
      আভাস শূন্য run_run() = 0;
      আভাস ~evctl() {}
      evctl*next;

    গুপ্ত:
      uint64_t ecount_;
};

class evctl_real : সার্বজনিক evctl {

    সার্বজনিক:
      explicit evctl_real(class __vpiHandle*handle, দ্বিগুণ মান,
                          uint64_t ecount);
      আভাস ~evctl_real() override {}
      শূন্য run_run() override;

    গুপ্ত:
      class __vpiHandle*handle_;
      দ্বিগুণ মান_2;
};

class evctl_vector : সার্বজনিক evctl {

    সার্বজনিক:
      explicit evctl_vector(vvp_net_ptr_t ptr, ধ্রুব vvp_vector4_t&মান,
                            unsigned off, unsigned wid, uint64_t ecount);
      আভাস ~evctl_vector() override {}
      শূন্য run_run() override;

    গুপ্ত:
      vvp_net_ptr_t ptr_;
      vvp_vector4_t মান_2;
      unsigned off_;
      unsigned wid_;
};

class evctl_array : সার্বজনিক evctl {

    সার্বজনিক:
      explicit evctl_array(vvp_array_t memory, unsigned index,
                           ধ্রুব vvp_vector4_t&মান, unsigned off,
                           uint64_t ecount);
      আভাস ~evctl_array() override {}
      আভাস শূন্য run_run() override;

    গুপ্ত:
      vvp_array_t mem_;
      unsigned idx_;
      vvp_vector4_t মান_2;
      unsigned off_;
};

class evctl_array_r : সার্বজনিক evctl {

    সার্বজনিক:
      explicit evctl_array_r(vvp_array_t memory, unsigned index,
                           দ্বিগুণ মান, uint64_t ecount);
      আভাস ~evctl_array_r() override {}
      আভাস শূন্য run_run() override;

    গুপ্ত:
      vvp_array_t mem_;
      unsigned idx_;
      দ্বিগুণ মান_2;
};

বাহ্য শূন্য schedule_evctl(class __vpiHandle*handle, দ্বিগুণ মান,
                           vvp_net_t*event, uint64_t ecount);

বাহ্য শূন্য schedule_evctl(vvp_net_ptr_t ptr, ধ্রুব vvp_vector4_t&মান,
                           unsigned offset, unsigned wid,
                           vvp_net_t*event, uint64_t ecount);

বাহ্য শূন্য schedule_evctl(vvp_array_t memory, unsigned index,
                           ধ্রুব vvp_vector4_t&মান, unsigned offset,
                           vvp_net_t*event, uint64_t ecount);

বাহ্য শূন্য schedule_evctl(vvp_array_t memory, unsigned index,
                           দ্বিগুণ মান,
                           vvp_net_t*event, uint64_t ecount);

/*
 *  Event / edge detection functors
 */

/*
 * A "waitable" functor is one that the %wait instruction can wait
 * on. This includes the infrastructure needed to hold threads.
 */
সংরচনা waitable_hooks_s {

    সার্বজনিক:
      waitable_hooks_s() : event_ctls(0) { last = &event_ctls; }
      আভাস ~waitable_hooks_s() {}

      আভাস vthread_t add_waiting_thread(vthread_t thread) = 0;

      evctl*event_ctls;
      evctl**last;

    রক্ষিত:
      শূন্য run_waiting_threads_(vthread_t&threads);
};

/*
 * This is the base object for storing state information for each instance
 * of an automatically allocated event. In the general case, all that is
 * needed is the list of threads waiting on that instance.
 */
সংরচনা waitable_state_s {
      waitable_state_s() : threads(0) {}

      vthread_t threads;
};

/*
 * The vvp_fun_edge functor detects events that are edges of various
 * types. This should be hooked to a vvp_net_t that is connected to
 * the output of a signal that we wish to watch for edges.
 */
class vvp_fun_edge : সার্বজনিক vvp_net_fun_t, সার্বজনিক waitable_hooks_s {

    সার্বজনিক:
      প্রকারনির্দেশ unsigned short edge_t;
      explicit vvp_fun_edge(edge_t e);
      আভাস ~vvp_fun_edge() override;

    রক্ষিত:
      bool recv_vec4_(ধ্রুব vvp_vector4_t&bit,
                      vvp_bit4_t&old_bit, vthread_t&threads);

      vvp_bit4_t bits_[4];

    গুপ্ত:
      edge_t edge_;
};

বাহ্য ধ্রুব vvp_fun_edge::edge_t vvp_edge_edge;
বাহ্য ধ্রুব vvp_fun_edge::edge_t vvp_edge_posedge;
বাহ্য ধ্রুব vvp_fun_edge::edge_t vvp_edge_negedge;
বাহ্য ধ্রুব vvp_fun_edge::edge_t vvp_edge_none;

/*
 * Statically allocated vvp_fun_edge.
 */
class vvp_fun_edge_sa : সার্বজনিক vvp_fun_edge {

    সার্বজনিক:
      explicit vvp_fun_edge_sa(edge_t e);
      আভাস ~vvp_fun_edge_sa() override;

      vthread_t add_waiting_thread(vthread_t thread) override;

      শূন্য recv_vec4(vvp_net_ptr_t port, ধ্রুব vvp_vector4_t&bit,
                     vvp_context_t context) override;
      শূন্য recv_vec4_pv(vvp_net_ptr_t port, ধ্রুব vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t context) override;

    গুপ্ত:
      vthread_t threads_;
};

/*
 * Automatically allocated vvp_fun_edge.
 */
class vvp_fun_edge_aa : সার্বজনিক vvp_fun_edge, সার্বজনিক automatic_hooks_s {

    সার্বজনিক:
      explicit vvp_fun_edge_aa(edge_t e);
      আভাস ~vvp_fun_edge_aa() override;

      শূন্য alloc_instance(vvp_context_t context) override;
      শূন্য reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      শূন্য free_instance(vvp_context_t context) override;
#endif

      vthread_t add_waiting_thread(vthread_t thread) override;

      শূন্য recv_vec4(vvp_net_ptr_t port, ধ্রুব vvp_vector4_t&bit,
                     vvp_context_t context) override;

    গুপ্ত:
      __vpiScope*context_scope_;
      unsigned context_idx_;
};

/*
 * This is the base object for storing the last received values for a
 * vvp_fun_anyedge functor.
 */
class anyedge_মান;

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
class vvp_fun_anyedge : সার্বজনিক vvp_net_fun_t, সার্বজনিক waitable_hooks_s {

    সার্বজনিক:
      explicit vvp_fun_anyedge();
      আভাস ~vvp_fun_anyedge() override;

    রক্ষিত:
      anyedge_মান*last_মান[4];
};

/*
 * Statically allocated vvp_fun_anyedge.
 */
class vvp_fun_anyedge_sa : সার্বজনিক vvp_fun_anyedge {

    সার্বজনিক:
      explicit vvp_fun_anyedge_sa();
      আভাস ~vvp_fun_anyedge_sa() override;

      vthread_t add_waiting_thread(vthread_t thread) override;

      শূন্য recv_vec4(vvp_net_ptr_t port, ধ্রুব vvp_vector4_t&bit,
                     vvp_context_t context) override;
      শূন্য recv_vec4_pv(vvp_net_ptr_t port, ধ্রুব vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t context) override;

      শূন্য recv_real(vvp_net_ptr_t port, দ্বিগুণ bit,
                     vvp_context_t context) override;

      শূন্য recv_string(vvp_net_ptr_t port, ধ্রুব std::string&bit,
		       vvp_context_t context) override;
      শূন্য recv_object(vvp_net_ptr_t port, vvp_object_t bit,
		       vvp_context_t context) override;

    গুপ্ত:
      vthread_t threads_;
};

/*
 * Automatically allocated vvp_fun_anyedge.
 */
class vvp_fun_anyedge_aa : সার্বজনিক vvp_fun_anyedge, সার্বজনিক automatic_hooks_s {

    সার্বজনিক:
      explicit vvp_fun_anyedge_aa();
      আভাস ~vvp_fun_anyedge_aa() override;

      শূন্য alloc_instance(vvp_context_t context) override;
      শূন্য reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      শূন্য free_instance(vvp_context_t context) override;
#endif

      vthread_t add_waiting_thread(vthread_t thread) override;

      শূন্য recv_vec4(vvp_net_ptr_t port, ধ্রুব vvp_vector4_t&bit,
                     vvp_context_t context) override;

      শূন্য recv_real(vvp_net_ptr_t port, দ্বিগুণ bit,
                     vvp_context_t context) override;

      শূন্য recv_string(vvp_net_ptr_t port, ধ্রুব std::string&bit,
		       vvp_context_t context) override;

    গুপ্ত:
      __vpiScope*context_scope_;
      unsigned context_idx_;
};

/*
 * This functor triggers anytime any input is set, no matter what the
 * value. This is similar to a named event, but it has no handle. It
 * supports wide (more than 4) inputs, so needs a pointer to the base
 * net that is used for output.
 */
class vvp_fun_event_or : সার্বজনিক vvp_net_fun_t, সার্বজনিক waitable_hooks_s {

    সার্বজনিক:
      explicit vvp_fun_event_or(vvp_net_t*base_net);
      ~vvp_fun_event_or() override;

    রক্ষিত:
      vvp_net_t*base_net_;
};

/*
 * Statically allocated vvp_fun_event_or.
 */
class vvp_fun_event_or_sa : সার্বজনিক vvp_fun_event_or {

    সার্বজনিক:
      explicit vvp_fun_event_or_sa(vvp_net_t*base_net);
      ~vvp_fun_event_or_sa() override;

      vthread_t add_waiting_thread(vthread_t thread) override;

      শূন্য recv_vec4(vvp_net_ptr_t port, ধ্রুব vvp_vector4_t&bit,
                     vvp_context_t context) override;

    গুপ্ত:
      vthread_t threads_;
};

/*
 * Automatically allocated vvp_fun_event_or.
 */
class vvp_fun_event_or_aa : সার্বজনিক vvp_fun_event_or, সার্বজনিক automatic_hooks_s {

    সার্বজনিক:
      explicit vvp_fun_event_or_aa(vvp_net_t*base_net);
      ~vvp_fun_event_or_aa() override;

      শূন্য alloc_instance(vvp_context_t context) override;
      শূন্য reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      শূন্য free_instance(vvp_context_t context) override;
#endif

      vthread_t add_waiting_thread(vthread_t thread) override;

      শূন্য recv_vec4(vvp_net_ptr_t port, ধ্রুব vvp_vector4_t&bit,
                     vvp_context_t context) override;

    গুপ্ত:
      __vpiScope*context_scope_;
      unsigned context_idx_;
};

/*
 * A named event is simpler than a vvp_fun_edge in that it triggers on
 * any input at all to port-0. The idea here is that behavioral code
 * can use a %set/v instruction to trigger the event.
 */
class vvp_named_event : সার্বজনিক vvp_net_fun_t, সার্বজনিক waitable_hooks_s {

    সার্বজনিক:
      explicit vvp_named_event(class __vpiHandle*eh);
      ~vvp_named_event() override;

    রক্ষিত:
      class __vpiHandle*handle_;
};

/*
 * Statically allocated vvp_named_event.
 */
class vvp_named_event_sa : সার্বজনিক vvp_named_event {

    সার্বজনিক:
      explicit vvp_named_event_sa(class __vpiHandle*eh);
      ~vvp_named_event_sa() override;

      vthread_t add_waiting_thread(vthread_t thread) override;

      শূন্য recv_vec4(vvp_net_ptr_t port, ধ্রুব vvp_vector4_t&bit,
                     vvp_context_t) override;

    গুপ্ত:
      vthread_t threads_;
};

/*
 * Automatically allocated vvp_named_event.
 */
class vvp_named_event_aa : সার্বজনিক vvp_named_event, সার্বজনিক automatic_hooks_s {

    সার্বজনিক:
      explicit vvp_named_event_aa(class __vpiHandle*eh);
      ~vvp_named_event_aa() override;

      শূন্য alloc_instance(vvp_context_t context) override;
      শূন্য reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      শূন্য free_instance(vvp_context_t context) override;
#endif

      vthread_t add_waiting_thread(vthread_t thread) override;

      শূন্য recv_vec4(vvp_net_ptr_t port, ধ্রুব vvp_vector4_t&bit,
                     vvp_context_t context) override;

    গুপ্ত:
      unsigned context_idx_;
};

#endif /* IVL_event_H */
