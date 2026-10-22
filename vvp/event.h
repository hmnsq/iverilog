#अगर_अघोषित IVL_event_H
#घोषणा IVL_event_H
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

    खुला:
      explicit evctl(uint64_t ecount);
      bool dec_and_run();
      भव व्योम run_run() = 0;
      भव ~evctl() {}
      evctl*अग्र_2;

    निजी:
      uint64_t ecount_;
};

class evctl_real : खुला evctl {

    खुला:
      explicit evctl_real(class __vpiHandle*handle, द्विगुन मूल्य,
                          uint64_t ecount);
      भव ~evctl_real() override {}
      व्योम run_run() override;

    निजी:
      class __vpiHandle*handle_;
      द्विगुन मूल्य_2;
};

class evctl_vector : खुला evctl {

    खुला:
      explicit evctl_vector(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&मूल्य,
                            अचिन्हित off, अचिन्हित wid, uint64_t ecount);
      भव ~evctl_vector() override {}
      व्योम run_run() override;

    निजी:
      vvp_net_ptr_t ptr_;
      vvp_vector4_t मूल्य_2;
      अचिन्हित off_;
      अचिन्हित wid_;
};

class evctl_array : खुला evctl {

    खुला:
      explicit evctl_array(vvp_array_t memory, अचिन्हित सूचकांक,
                           स्थिर vvp_vector4_t&मूल्य, अचिन्हित off,
                           uint64_t ecount);
      भव ~evctl_array() override {}
      भव व्योम run_run() override;

    निजी:
      vvp_array_t mem_;
      अचिन्हित idx_;
      vvp_vector4_t मूल्य_2;
      अचिन्हित off_;
};

class evctl_array_r : खुला evctl {

    खुला:
      explicit evctl_array_r(vvp_array_t memory, अचिन्हित सूचकांक,
                           द्विगुन मूल्य, uint64_t ecount);
      भव ~evctl_array_r() override {}
      भव व्योम run_run() override;

    निजी:
      vvp_array_t mem_;
      अचिन्हित idx_;
      द्विगुन मूल्य_2;
};

बाह्य व्योम schedule_evctl(class __vpiHandle*handle, द्विगुन मूल्य,
                           vvp_net_t*event, uint64_t ecount);

बाह्य व्योम schedule_evctl(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&मूल्य,
                           अचिन्हित offset, अचिन्हित wid,
                           vvp_net_t*event, uint64_t ecount);

बाह्य व्योम schedule_evctl(vvp_array_t memory, अचिन्हित सूचकांक,
                           स्थिर vvp_vector4_t&मूल्य, अचिन्हित offset,
                           vvp_net_t*event, uint64_t ecount);

बाह्य व्योम schedule_evctl(vvp_array_t memory, अचिन्हित सूचकांक,
                           द्विगुन मूल्य,
                           vvp_net_t*event, uint64_t ecount);

/*
 *  Event / edge detection functors
 */

/*
 * A "waitable" functor is one that the %wait instruction can wait
 * on. This includes the infrastructure needed to hold threads.
 */
काष्ठा waitable_hooks_s {

    खुला:
      waitable_hooks_s() : event_ctls(0) { last = &event_ctls; }
      भव ~waitable_hooks_s() {}

      भव vthread_t add_waiting_thread(vthread_t thread) = 0;

      evctl*event_ctls;
      evctl**last;

    रक्षित:
      व्योम run_waiting_threads_(vthread_t&threads);
};

/*
 * This is the base object for storing state information for each instance
 * of an automatically allocated event. In the general case, all that is
 * needed is the list of threads waiting on that instance.
 */
काष्ठा waitable_state_s {
      waitable_state_s() : threads(0) {}

      vthread_t threads;
};

/*
 * The vvp_fun_edge functor detects events that are edges of various
 * types. This should be hooked to a vvp_net_t that is connected to
 * the output of a signal that we wish to watch for edges.
 */
class vvp_fun_edge : खुला vvp_net_fun_t, खुला waitable_hooks_s {

    खुला:
      प्रकार अचिन्हित लघु edge_t;
      explicit vvp_fun_edge(edge_t e);
      भव ~vvp_fun_edge() override;

    रक्षित:
      bool recv_vec4_(स्थिर vvp_vector4_t&bit,
                      vvp_bit4_t&old_bit, vthread_t&threads);

      vvp_bit4_t bits_[4];

    निजी:
      edge_t edge_;
};

बाह्य स्थिर vvp_fun_edge::edge_t vvp_edge_edge;
बाह्य स्थिर vvp_fun_edge::edge_t vvp_edge_posedge;
बाह्य स्थिर vvp_fun_edge::edge_t vvp_edge_negedge;
बाह्य स्थिर vvp_fun_edge::edge_t vvp_edge_none;

/*
 * Statically allocated vvp_fun_edge.
 */
class vvp_fun_edge_sa : खुला vvp_fun_edge {

    खुला:
      explicit vvp_fun_edge_sa(edge_t e);
      भव ~vvp_fun_edge_sa() override;

      vthread_t add_waiting_thread(vthread_t thread) override;

      व्योम recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                     vvp_context_t context) override;
      व्योम recv_vec4_pv(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
			अचिन्हित base, अचिन्हित vwid, vvp_context_t context) override;

    निजी:
      vthread_t threads_;
};

/*
 * Automatically allocated vvp_fun_edge.
 */
class vvp_fun_edge_aa : खुला vvp_fun_edge, खुला automatic_hooks_s {

    खुला:
      explicit vvp_fun_edge_aa(edge_t e);
      भव ~vvp_fun_edge_aa() override;

      व्योम alloc_instance(vvp_context_t context) override;
      व्योम reset_instance(vvp_context_t context) override;
#अगर_घोषित CHECK_WITH_VALGRIND
      व्योम free_instance(vvp_context_t context) override;
#पूर्ण_अगर

      vthread_t add_waiting_thread(vthread_t thread) override;

      व्योम recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                     vvp_context_t context) override;

    निजी:
      __vpiScope*context_scope_;
      अचिन्हित context_idx_;
};

/*
 * This is the base object for storing the last received values for a
 * vvp_fun_anyedge functor.
 */
class anyedge_मूल्य;

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
class vvp_fun_anyedge : खुला vvp_net_fun_t, खुला waitable_hooks_s {

    खुला:
      explicit vvp_fun_anyedge();
      भव ~vvp_fun_anyedge() override;

    रक्षित:
      anyedge_मूल्य*last_मूल्य[4];
};

/*
 * Statically allocated vvp_fun_anyedge.
 */
class vvp_fun_anyedge_sa : खुला vvp_fun_anyedge {

    खुला:
      explicit vvp_fun_anyedge_sa();
      भव ~vvp_fun_anyedge_sa() override;

      vthread_t add_waiting_thread(vthread_t thread) override;

      व्योम recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                     vvp_context_t context) override;
      व्योम recv_vec4_pv(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
			अचिन्हित base, अचिन्हित vwid, vvp_context_t context) override;

      व्योम recv_real(vvp_net_ptr_t port, द्विगुन bit,
                     vvp_context_t context) override;

      व्योम recv_string(vvp_net_ptr_t port, स्थिर std::string&bit,
		       vvp_context_t context) override;
      व्योम recv_object(vvp_net_ptr_t port, vvp_object_t bit,
		       vvp_context_t context) override;

    निजी:
      vthread_t threads_;
};

/*
 * Automatically allocated vvp_fun_anyedge.
 */
class vvp_fun_anyedge_aa : खुला vvp_fun_anyedge, खुला automatic_hooks_s {

    खुला:
      explicit vvp_fun_anyedge_aa();
      भव ~vvp_fun_anyedge_aa() override;

      व्योम alloc_instance(vvp_context_t context) override;
      व्योम reset_instance(vvp_context_t context) override;
#अगर_घोषित CHECK_WITH_VALGRIND
      व्योम free_instance(vvp_context_t context) override;
#पूर्ण_अगर

      vthread_t add_waiting_thread(vthread_t thread) override;

      व्योम recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                     vvp_context_t context) override;

      व्योम recv_real(vvp_net_ptr_t port, द्विगुन bit,
                     vvp_context_t context) override;

      व्योम recv_string(vvp_net_ptr_t port, स्थिर std::string&bit,
		       vvp_context_t context) override;

    निजी:
      __vpiScope*context_scope_;
      अचिन्हित context_idx_;
};

/*
 * This functor triggers anytime any input is set, no matter what the
 * value. This is similar to a named event, but it has no handle. It
 * supports wide (more than 4) inputs, so needs a pointer to the base
 * net that is used for output.
 */
class vvp_fun_event_or : खुला vvp_net_fun_t, खुला waitable_hooks_s {

    खुला:
      explicit vvp_fun_event_or(vvp_net_t*base_net);
      ~vvp_fun_event_or() override;

    रक्षित:
      vvp_net_t*base_net_;
};

/*
 * Statically allocated vvp_fun_event_or.
 */
class vvp_fun_event_or_sa : खुला vvp_fun_event_or {

    खुला:
      explicit vvp_fun_event_or_sa(vvp_net_t*base_net);
      ~vvp_fun_event_or_sa() override;

      vthread_t add_waiting_thread(vthread_t thread) override;

      व्योम recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                     vvp_context_t context) override;

    निजी:
      vthread_t threads_;
};

/*
 * Automatically allocated vvp_fun_event_or.
 */
class vvp_fun_event_or_aa : खुला vvp_fun_event_or, खुला automatic_hooks_s {

    खुला:
      explicit vvp_fun_event_or_aa(vvp_net_t*base_net);
      ~vvp_fun_event_or_aa() override;

      व्योम alloc_instance(vvp_context_t context) override;
      व्योम reset_instance(vvp_context_t context) override;
#अगर_घोषित CHECK_WITH_VALGRIND
      व्योम free_instance(vvp_context_t context) override;
#पूर्ण_अगर

      vthread_t add_waiting_thread(vthread_t thread) override;

      व्योम recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                     vvp_context_t context) override;

    निजी:
      __vpiScope*context_scope_;
      अचिन्हित context_idx_;
};

/*
 * A named event is simpler than a vvp_fun_edge in that it triggers on
 * any input at all to port-0. The idea here is that behavioral code
 * can use a %set/v instruction to trigger the event.
 */
class vvp_named_event : खुला vvp_net_fun_t, खुला waitable_hooks_s {

    खुला:
      explicit vvp_named_event(class __vpiHandle*eh);
      ~vvp_named_event() override;

    रक्षित:
      class __vpiHandle*handle_;
};

/*
 * Statically allocated vvp_named_event.
 */
class vvp_named_event_sa : खुला vvp_named_event {

    खुला:
      explicit vvp_named_event_sa(class __vpiHandle*eh);
      ~vvp_named_event_sa() override;

      vthread_t add_waiting_thread(vthread_t thread) override;

      व्योम recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                     vvp_context_t) override;

    निजी:
      vthread_t threads_;
};

/*
 * Automatically allocated vvp_named_event.
 */
class vvp_named_event_aa : खुला vvp_named_event, खुला automatic_hooks_s {

    खुला:
      explicit vvp_named_event_aa(class __vpiHandle*eh);
      ~vvp_named_event_aa() override;

      व्योम alloc_instance(vvp_context_t context) override;
      व्योम reset_instance(vvp_context_t context) override;
#अगर_घोषित CHECK_WITH_VALGRIND
      व्योम free_instance(vvp_context_t context) override;
#पूर्ण_अगर

      vthread_t add_waiting_thread(vthread_t thread) override;

      व्योम recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                     vvp_context_t context) override;

    निजी:
      अचिन्हित context_idx_;
};

#पूर्ण_अगर /* IVL_event_H */
