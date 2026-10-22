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

    ସର୍ବସାଧାରଣ:
      explicit evctl(uint64_t ecount);
      bool dec_and_run();
      ଆଭାସୀ ଶୂନ୍ୟ run_run() = 0;
      ଆଭାସୀ ~evctl() {}
      evctl*next;

    ବ୍ୟକ୍ତିଗତ:
      uint64_t ecount_;
};

class evctl_real : ସର୍ବସାଧାରଣ evctl {

    ସର୍ବସାଧାରଣ:
      explicit evctl_real(class __vpiHandle*handle, ଦ୍ୱିଗୁଣ ମୂଲ୍ୟ,
                          uint64_t ecount);
      ଆଭାସୀ ~evctl_real() override {}
      ଶୂନ୍ୟ run_run() override;

    ବ୍ୟକ୍ତିଗତ:
      class __vpiHandle*handle_;
      ଦ୍ୱିଗୁଣ ମୂଲ୍ୟ_2;
};

class evctl_vector : ସର୍ବସାଧାରଣ evctl {

    ସର୍ବସାଧାରଣ:
      explicit evctl_vector(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&ମୂଲ୍ୟ,
                            unsigned off, unsigned wid, uint64_t ecount);
      ଆଭାସୀ ~evctl_vector() override {}
      ଶୂନ୍ୟ run_run() override;

    ବ୍ୟକ୍ତିଗତ:
      vvp_net_ptr_t ptr_;
      vvp_vector4_t ମୂଲ୍ୟ_2;
      unsigned off_;
      unsigned wid_;
};

class evctl_array : ସର୍ବସାଧାରଣ evctl {

    ସର୍ବସାଧାରଣ:
      explicit evctl_array(vvp_array_t memory, unsigned index,
                           ଅଚଳ vvp_vector4_t&ମୂଲ୍ୟ, unsigned off,
                           uint64_t ecount);
      ଆଭାସୀ ~evctl_array() override {}
      ଆଭାସୀ ଶୂନ୍ୟ run_run() override;

    ବ୍ୟକ୍ତିଗତ:
      vvp_array_t mem_;
      unsigned idx_;
      vvp_vector4_t ମୂଲ୍ୟ_2;
      unsigned off_;
};

class evctl_array_r : ସର୍ବସାଧାରଣ evctl {

    ସର୍ବସାଧାରଣ:
      explicit evctl_array_r(vvp_array_t memory, unsigned index,
                           ଦ୍ୱିଗୁଣ ମୂଲ୍ୟ, uint64_t ecount);
      ଆଭାସୀ ~evctl_array_r() override {}
      ଆଭାସୀ ଶୂନ୍ୟ run_run() override;

    ବ୍ୟକ୍ତିଗତ:
      vvp_array_t mem_;
      unsigned idx_;
      ଦ୍ୱିଗୁଣ ମୂଲ୍ୟ_2;
};

ବାହ୍ୟ ଶୂନ୍ୟ schedule_evctl(class __vpiHandle*handle, ଦ୍ୱିଗୁଣ ମୂଲ୍ୟ,
                           vvp_net_t*event, uint64_t ecount);

ବାହ୍ୟ ଶୂନ୍ୟ schedule_evctl(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&ମୂଲ୍ୟ,
                           unsigned offset, unsigned wid,
                           vvp_net_t*event, uint64_t ecount);

ବାହ୍ୟ ଶୂନ୍ୟ schedule_evctl(vvp_array_t memory, unsigned index,
                           ଅଚଳ vvp_vector4_t&ମୂଲ୍ୟ, unsigned offset,
                           vvp_net_t*event, uint64_t ecount);

ବାହ୍ୟ ଶୂନ୍ୟ schedule_evctl(vvp_array_t memory, unsigned index,
                           ଦ୍ୱିଗୁଣ ମୂଲ୍ୟ,
                           vvp_net_t*event, uint64_t ecount);

/*
 *  Event / edge detection functors
 */

/*
 * A "waitable" functor is one that the %wait instruction can wait
 * on. This includes the infrastructure needed to hold threads.
 */
ସଂରଚନା waitable_hooks_s {

    ସର୍ବସାଧାରଣ:
      waitable_hooks_s() : event_ctls(0) { last = &event_ctls; }
      ଆଭାସୀ ~waitable_hooks_s() {}

      ଆଭାସୀ vthread_t add_waiting_thread(vthread_t thread) = 0;

      evctl*event_ctls;
      evctl**last;

    ସୁରକ୍ଷିତ:
      ଶୂନ୍ୟ run_waiting_threads_(vthread_t&threads);
};

/*
 * This is the base object for storing state information for each instance
 * of an automatically allocated event. In the general case, all that is
 * needed is the list of threads waiting on that instance.
 */
ସଂରଚନା waitable_state_s {
      waitable_state_s() : threads(0) {}

      vthread_t threads;
};

/*
 * The vvp_fun_edge functor detects events that are edges of various
 * types. This should be hooked to a vvp_net_t that is connected to
 * the output of a signal that we wish to watch for edges.
 */
class vvp_fun_edge : ସର୍ବସାଧାରଣ vvp_net_fun_t, ସର୍ବସାଧାରଣ waitable_hooks_s {

    ସର୍ବସାଧାରଣ:
      ପ୍ରକାରବ୍ୟାଖ୍ୟା unsigned short edge_t;
      explicit vvp_fun_edge(edge_t e);
      ଆଭାସୀ ~vvp_fun_edge() override;

    ସୁରକ୍ଷିତ:
      bool recv_vec4_(ଅଚଳ vvp_vector4_t&bit,
                      vvp_bit4_t&old_bit, vthread_t&threads);

      vvp_bit4_t bits_[4];

    ବ୍ୟକ୍ତିଗତ:
      edge_t edge_;
};

ବାହ୍ୟ ଅଚଳ vvp_fun_edge::edge_t vvp_edge_edge;
ବାହ୍ୟ ଅଚଳ vvp_fun_edge::edge_t vvp_edge_posedge;
ବାହ୍ୟ ଅଚଳ vvp_fun_edge::edge_t vvp_edge_negedge;
ବାହ୍ୟ ଅଚଳ vvp_fun_edge::edge_t vvp_edge_none;

/*
 * Statically allocated vvp_fun_edge.
 */
class vvp_fun_edge_sa : ସର୍ବସାଧାରଣ vvp_fun_edge {

    ସର୍ବସାଧାରଣ:
      explicit vvp_fun_edge_sa(edge_t e);
      ଆଭାସୀ ~vvp_fun_edge_sa() override;

      vthread_t add_waiting_thread(vthread_t thread) override;

      ଶୂନ୍ୟ recv_vec4(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
                     vvp_context_t context) override;
      ଶୂନ୍ୟ recv_vec4_pv(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t context) override;

    ବ୍ୟକ୍ତିଗତ:
      vthread_t threads_;
};

/*
 * Automatically allocated vvp_fun_edge.
 */
class vvp_fun_edge_aa : ସର୍ବସାଧାରଣ vvp_fun_edge, ସର୍ବସାଧାରଣ automatic_hooks_s {

    ସର୍ବସାଧାରଣ:
      explicit vvp_fun_edge_aa(edge_t e);
      ଆଭାସୀ ~vvp_fun_edge_aa() override;

      ଶୂନ୍ୟ alloc_instance(vvp_context_t context) override;
      ଶୂନ୍ୟ reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      ଶୂନ୍ୟ free_instance(vvp_context_t context) override;
#endif

      vthread_t add_waiting_thread(vthread_t thread) override;

      ଶୂନ୍ୟ recv_vec4(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
                     vvp_context_t context) override;

    ବ୍ୟକ୍ତିଗତ:
      __vpiScope*context_scope_;
      unsigned context_idx_;
};

/*
 * This is the base object for storing the last received values for a
 * vvp_fun_anyedge functor.
 */
class anyedge_ମୂଲ୍ୟ;

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
class vvp_fun_anyedge : ସର୍ବସାଧାରଣ vvp_net_fun_t, ସର୍ବସାଧାରଣ waitable_hooks_s {

    ସର୍ବସାଧାରଣ:
      explicit vvp_fun_anyedge();
      ଆଭାସୀ ~vvp_fun_anyedge() override;

    ସୁରକ୍ଷିତ:
      anyedge_ମୂଲ୍ୟ*last_ମୂଲ୍ୟ[4];
};

/*
 * Statically allocated vvp_fun_anyedge.
 */
class vvp_fun_anyedge_sa : ସର୍ବସାଧାରଣ vvp_fun_anyedge {

    ସର୍ବସାଧାରଣ:
      explicit vvp_fun_anyedge_sa();
      ଆଭାସୀ ~vvp_fun_anyedge_sa() override;

      vthread_t add_waiting_thread(vthread_t thread) override;

      ଶୂନ୍ୟ recv_vec4(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
                     vvp_context_t context) override;
      ଶୂନ୍ୟ recv_vec4_pv(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t context) override;

      ଶୂନ୍ୟ recv_real(vvp_net_ptr_t port, ଦ୍ୱିଗୁଣ bit,
                     vvp_context_t context) override;

      ଶୂନ୍ୟ recv_string(vvp_net_ptr_t port, ଅଚଳ std::string&bit,
		       vvp_context_t context) override;
      ଶୂନ୍ୟ recv_object(vvp_net_ptr_t port, vvp_object_t bit,
		       vvp_context_t context) override;

    ବ୍ୟକ୍ତିଗତ:
      vthread_t threads_;
};

/*
 * Automatically allocated vvp_fun_anyedge.
 */
class vvp_fun_anyedge_aa : ସର୍ବସାଧାରଣ vvp_fun_anyedge, ସର୍ବସାଧାରଣ automatic_hooks_s {

    ସର୍ବସାଧାରଣ:
      explicit vvp_fun_anyedge_aa();
      ଆଭାସୀ ~vvp_fun_anyedge_aa() override;

      ଶୂନ୍ୟ alloc_instance(vvp_context_t context) override;
      ଶୂନ୍ୟ reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      ଶୂନ୍ୟ free_instance(vvp_context_t context) override;
#endif

      vthread_t add_waiting_thread(vthread_t thread) override;

      ଶୂନ୍ୟ recv_vec4(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
                     vvp_context_t context) override;

      ଶୂନ୍ୟ recv_real(vvp_net_ptr_t port, ଦ୍ୱିଗୁଣ bit,
                     vvp_context_t context) override;

      ଶୂନ୍ୟ recv_string(vvp_net_ptr_t port, ଅଚଳ std::string&bit,
		       vvp_context_t context) override;

    ବ୍ୟକ୍ତିଗତ:
      __vpiScope*context_scope_;
      unsigned context_idx_;
};

/*
 * This functor triggers anytime any input is set, no matter what the
 * value. This is similar to a named event, but it has no handle. It
 * supports wide (more than 4) inputs, so needs a pointer to the base
 * net that is used for output.
 */
class vvp_fun_event_or : ସର୍ବସାଧାରଣ vvp_net_fun_t, ସର୍ବସାଧାରଣ waitable_hooks_s {

    ସର୍ବସାଧାରଣ:
      explicit vvp_fun_event_or(vvp_net_t*base_net);
      ~vvp_fun_event_or() override;

    ସୁରକ୍ଷିତ:
      vvp_net_t*base_net_;
};

/*
 * Statically allocated vvp_fun_event_or.
 */
class vvp_fun_event_or_sa : ସର୍ବସାଧାରଣ vvp_fun_event_or {

    ସର୍ବସାଧାରଣ:
      explicit vvp_fun_event_or_sa(vvp_net_t*base_net);
      ~vvp_fun_event_or_sa() override;

      vthread_t add_waiting_thread(vthread_t thread) override;

      ଶୂନ୍ୟ recv_vec4(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
                     vvp_context_t context) override;

    ବ୍ୟକ୍ତିଗତ:
      vthread_t threads_;
};

/*
 * Automatically allocated vvp_fun_event_or.
 */
class vvp_fun_event_or_aa : ସର୍ବସାଧାରଣ vvp_fun_event_or, ସର୍ବସାଧାରଣ automatic_hooks_s {

    ସର୍ବସାଧାରଣ:
      explicit vvp_fun_event_or_aa(vvp_net_t*base_net);
      ~vvp_fun_event_or_aa() override;

      ଶୂନ୍ୟ alloc_instance(vvp_context_t context) override;
      ଶୂନ୍ୟ reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      ଶୂନ୍ୟ free_instance(vvp_context_t context) override;
#endif

      vthread_t add_waiting_thread(vthread_t thread) override;

      ଶୂନ୍ୟ recv_vec4(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
                     vvp_context_t context) override;

    ବ୍ୟକ୍ତିଗତ:
      __vpiScope*context_scope_;
      unsigned context_idx_;
};

/*
 * A named event is simpler than a vvp_fun_edge in that it triggers on
 * any input at all to port-0. The idea here is that behavioral code
 * can use a %set/v instruction to trigger the event.
 */
class vvp_named_event : ସର୍ବସାଧାରଣ vvp_net_fun_t, ସର୍ବସାଧାରଣ waitable_hooks_s {

    ସର୍ବସାଧାରଣ:
      explicit vvp_named_event(class __vpiHandle*eh);
      ~vvp_named_event() override;

    ସୁରକ୍ଷିତ:
      class __vpiHandle*handle_;
};

/*
 * Statically allocated vvp_named_event.
 */
class vvp_named_event_sa : ସର୍ବସାଧାରଣ vvp_named_event {

    ସର୍ବସାଧାରଣ:
      explicit vvp_named_event_sa(class __vpiHandle*eh);
      ~vvp_named_event_sa() override;

      vthread_t add_waiting_thread(vthread_t thread) override;

      ଶୂନ୍ୟ recv_vec4(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
                     vvp_context_t) override;

    ବ୍ୟକ୍ତିଗତ:
      vthread_t threads_;
};

/*
 * Automatically allocated vvp_named_event.
 */
class vvp_named_event_aa : ସର୍ବସାଧାରଣ vvp_named_event, ସର୍ବସାଧାରଣ automatic_hooks_s {

    ସର୍ବସାଧାରଣ:
      explicit vvp_named_event_aa(class __vpiHandle*eh);
      ~vvp_named_event_aa() override;

      ଶୂନ୍ୟ alloc_instance(vvp_context_t context) override;
      ଶୂନ୍ୟ reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      ଶୂନ୍ୟ free_instance(vvp_context_t context) override;
#endif

      vthread_t add_waiting_thread(vthread_t thread) override;

      ଶୂନ୍ୟ recv_vec4(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
                     vvp_context_t context) override;

    ବ୍ୟକ୍ତିଗତ:
      unsigned context_idx_;
};

#endif /* IVL_event_H */
