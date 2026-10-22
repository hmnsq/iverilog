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

    सार्वजनिक:
      explicit evctl(uint64_t ecount);
      bool dec_and_run();
      आभासी शून्य run_run() = 0;
      आभासी ~evctl() {}
      evctl*next;

    खाजगी:
      uint64_t ecount_;
};

class evctl_real : सार्वजनिक evctl {

    सार्वजनिक:
      explicit evctl_real(class __vpiHandle*handle, द्विगुण मूल्य,
                          uint64_t ecount);
      आभासी ~evctl_real() override {}
      शून्य run_run() override;

    खाजगी:
      class __vpiHandle*handle_;
      द्विगुण मूल्य_2;
};

class evctl_vector : सार्वजनिक evctl {

    सार्वजनिक:
      explicit evctl_vector(vvp_net_ptr_t ptr, स्थिरांक vvp_vector4_t&मूल्य,
                            अचिन्हित off, अचिन्हित wid, uint64_t ecount);
      आभासी ~evctl_vector() override {}
      शून्य run_run() override;

    खाजगी:
      vvp_net_ptr_t ptr_;
      vvp_vector4_t मूल्य_2;
      अचिन्हित off_;
      अचिन्हित wid_;
};

class evctl_array : सार्वजनिक evctl {

    सार्वजनिक:
      explicit evctl_array(vvp_array_t memory, अचिन्हित index,
                           स्थिरांक vvp_vector4_t&मूल्य, अचिन्हित off,
                           uint64_t ecount);
      आभासी ~evctl_array() override {}
      आभासी शून्य run_run() override;

    खाजगी:
      vvp_array_t mem_;
      अचिन्हित idx_;
      vvp_vector4_t मूल्य_2;
      अचिन्हित off_;
};

class evctl_array_r : सार्वजनिक evctl {

    सार्वजनिक:
      explicit evctl_array_r(vvp_array_t memory, अचिन्हित index,
                           द्विगुण मूल्य, uint64_t ecount);
      आभासी ~evctl_array_r() override {}
      आभासी शून्य run_run() override;

    खाजगी:
      vvp_array_t mem_;
      अचिन्हित idx_;
      द्विगुण मूल्य_2;
};

बाह्य शून्य schedule_evctl(class __vpiHandle*handle, द्विगुण मूल्य,
                           vvp_net_t*event, uint64_t ecount);

बाह्य शून्य schedule_evctl(vvp_net_ptr_t ptr, स्थिरांक vvp_vector4_t&मूल्य,
                           अचिन्हित offset, अचिन्हित wid,
                           vvp_net_t*event, uint64_t ecount);

बाह्य शून्य schedule_evctl(vvp_array_t memory, अचिन्हित index,
                           स्थिरांक vvp_vector4_t&मूल्य, अचिन्हित offset,
                           vvp_net_t*event, uint64_t ecount);

बाह्य शून्य schedule_evctl(vvp_array_t memory, अचिन्हित index,
                           द्विगुण मूल्य,
                           vvp_net_t*event, uint64_t ecount);

/*
 *  Event / edge detection functors
 */

/*
 * A "waitable" functor is one that the %wait instruction can wait
 * on. This includes the infrastructure needed to hold threads.
 */
संरचना waitable_hooks_s {

    सार्वजनिक:
      waitable_hooks_s() : event_ctls(0) { last = &event_ctls; }
      आभासी ~waitable_hooks_s() {}

      आभासी vthread_t add_waiting_thread(vthread_t thread) = 0;

      evctl*event_ctls;
      evctl**last;

    संरक्षित:
      शून्य run_waiting_threads_(vthread_t&threads);
};

/*
 * This is the base object for storing state information for each instance
 * of an automatically allocated event. In the general case, all that is
 * needed is the list of threads waiting on that instance.
 */
संरचना waitable_state_s {
      waitable_state_s() : threads(0) {}

      vthread_t threads;
};

/*
 * The vvp_fun_edge functor detects events that are edges of various
 * types. This should be hooked to a vvp_net_t that is connected to
 * the output of a signal that we wish to watch for edges.
 */
class vvp_fun_edge : सार्वजनिक vvp_net_fun_t, सार्वजनिक waitable_hooks_s {

    सार्वजनिक:
      प्रकारव्याख्या अचिन्हित लघु edge_t;
      explicit vvp_fun_edge(edge_t e);
      आभासी ~vvp_fun_edge() override;

    संरक्षित:
      bool recv_vec4_(स्थिरांक vvp_vector4_t&bit,
                      vvp_bit4_t&old_bit, vthread_t&threads);

      vvp_bit4_t bits_[4];

    खाजगी:
      edge_t edge_;
};

बाह्य स्थिरांक vvp_fun_edge::edge_t vvp_edge_edge;
बाह्य स्थिरांक vvp_fun_edge::edge_t vvp_edge_posedge;
बाह्य स्थिरांक vvp_fun_edge::edge_t vvp_edge_negedge;
बाह्य स्थिरांक vvp_fun_edge::edge_t vvp_edge_none;

/*
 * Statically allocated vvp_fun_edge.
 */
class vvp_fun_edge_sa : सार्वजनिक vvp_fun_edge {

    सार्वजनिक:
      explicit vvp_fun_edge_sa(edge_t e);
      आभासी ~vvp_fun_edge_sa() override;

      vthread_t add_waiting_thread(vthread_t thread) override;

      शून्य recv_vec4(vvp_net_ptr_t port, स्थिरांक vvp_vector4_t&bit,
                     vvp_context_t context) override;
      शून्य recv_vec4_pv(vvp_net_ptr_t port, स्थिरांक vvp_vector4_t&bit,
			अचिन्हित base, अचिन्हित vwid, vvp_context_t context) override;

    खाजगी:
      vthread_t threads_;
};

/*
 * Automatically allocated vvp_fun_edge.
 */
class vvp_fun_edge_aa : सार्वजनिक vvp_fun_edge, सार्वजनिक automatic_hooks_s {

    सार्वजनिक:
      explicit vvp_fun_edge_aa(edge_t e);
      आभासी ~vvp_fun_edge_aa() override;

      शून्य alloc_instance(vvp_context_t context) override;
      शून्य reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      शून्य free_instance(vvp_context_t context) override;
#endif

      vthread_t add_waiting_thread(vthread_t thread) override;

      शून्य recv_vec4(vvp_net_ptr_t port, स्थिरांक vvp_vector4_t&bit,
                     vvp_context_t context) override;

    खाजगी:
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
class vvp_fun_anyedge : सार्वजनिक vvp_net_fun_t, सार्वजनिक waitable_hooks_s {

    सार्वजनिक:
      explicit vvp_fun_anyedge();
      आभासी ~vvp_fun_anyedge() override;

    संरक्षित:
      anyedge_मूल्य*last_मूल्य[4];
};

/*
 * Statically allocated vvp_fun_anyedge.
 */
class vvp_fun_anyedge_sa : सार्वजनिक vvp_fun_anyedge {

    सार्वजनिक:
      explicit vvp_fun_anyedge_sa();
      आभासी ~vvp_fun_anyedge_sa() override;

      vthread_t add_waiting_thread(vthread_t thread) override;

      शून्य recv_vec4(vvp_net_ptr_t port, स्थिरांक vvp_vector4_t&bit,
                     vvp_context_t context) override;
      शून्य recv_vec4_pv(vvp_net_ptr_t port, स्थिरांक vvp_vector4_t&bit,
			अचिन्हित base, अचिन्हित vwid, vvp_context_t context) override;

      शून्य recv_real(vvp_net_ptr_t port, द्विगुण bit,
                     vvp_context_t context) override;

      शून्य recv_string(vvp_net_ptr_t port, स्थिरांक std::string&bit,
		       vvp_context_t context) override;
      शून्य recv_object(vvp_net_ptr_t port, vvp_object_t bit,
		       vvp_context_t context) override;

    खाजगी:
      vthread_t threads_;
};

/*
 * Automatically allocated vvp_fun_anyedge.
 */
class vvp_fun_anyedge_aa : सार्वजनिक vvp_fun_anyedge, सार्वजनिक automatic_hooks_s {

    सार्वजनिक:
      explicit vvp_fun_anyedge_aa();
      आभासी ~vvp_fun_anyedge_aa() override;

      शून्य alloc_instance(vvp_context_t context) override;
      शून्य reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      शून्य free_instance(vvp_context_t context) override;
#endif

      vthread_t add_waiting_thread(vthread_t thread) override;

      शून्य recv_vec4(vvp_net_ptr_t port, स्थिरांक vvp_vector4_t&bit,
                     vvp_context_t context) override;

      शून्य recv_real(vvp_net_ptr_t port, द्विगुण bit,
                     vvp_context_t context) override;

      शून्य recv_string(vvp_net_ptr_t port, स्थिरांक std::string&bit,
		       vvp_context_t context) override;

    खाजगी:
      __vpiScope*context_scope_;
      अचिन्हित context_idx_;
};

/*
 * This functor triggers anytime any input is set, no matter what the
 * value. This is similar to a named event, but it has no handle. It
 * supports wide (more than 4) inputs, so needs a pointer to the base
 * net that is used for output.
 */
class vvp_fun_event_or : सार्वजनिक vvp_net_fun_t, सार्वजनिक waitable_hooks_s {

    सार्वजनिक:
      explicit vvp_fun_event_or(vvp_net_t*base_net);
      ~vvp_fun_event_or() override;

    संरक्षित:
      vvp_net_t*base_net_;
};

/*
 * Statically allocated vvp_fun_event_or.
 */
class vvp_fun_event_or_sa : सार्वजनिक vvp_fun_event_or {

    सार्वजनिक:
      explicit vvp_fun_event_or_sa(vvp_net_t*base_net);
      ~vvp_fun_event_or_sa() override;

      vthread_t add_waiting_thread(vthread_t thread) override;

      शून्य recv_vec4(vvp_net_ptr_t port, स्थिरांक vvp_vector4_t&bit,
                     vvp_context_t context) override;

    खाजगी:
      vthread_t threads_;
};

/*
 * Automatically allocated vvp_fun_event_or.
 */
class vvp_fun_event_or_aa : सार्वजनिक vvp_fun_event_or, सार्वजनिक automatic_hooks_s {

    सार्वजनिक:
      explicit vvp_fun_event_or_aa(vvp_net_t*base_net);
      ~vvp_fun_event_or_aa() override;

      शून्य alloc_instance(vvp_context_t context) override;
      शून्य reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      शून्य free_instance(vvp_context_t context) override;
#endif

      vthread_t add_waiting_thread(vthread_t thread) override;

      शून्य recv_vec4(vvp_net_ptr_t port, स्थिरांक vvp_vector4_t&bit,
                     vvp_context_t context) override;

    खाजगी:
      __vpiScope*context_scope_;
      अचिन्हित context_idx_;
};

/*
 * A named event is simpler than a vvp_fun_edge in that it triggers on
 * any input at all to port-0. The idea here is that behavioral code
 * can use a %set/v instruction to trigger the event.
 */
class vvp_named_event : सार्वजनिक vvp_net_fun_t, सार्वजनिक waitable_hooks_s {

    सार्वजनिक:
      explicit vvp_named_event(class __vpiHandle*eh);
      ~vvp_named_event() override;

    संरक्षित:
      class __vpiHandle*handle_;
};

/*
 * Statically allocated vvp_named_event.
 */
class vvp_named_event_sa : सार्वजनिक vvp_named_event {

    सार्वजनिक:
      explicit vvp_named_event_sa(class __vpiHandle*eh);
      ~vvp_named_event_sa() override;

      vthread_t add_waiting_thread(vthread_t thread) override;

      शून्य recv_vec4(vvp_net_ptr_t port, स्थिरांक vvp_vector4_t&bit,
                     vvp_context_t) override;

    खाजगी:
      vthread_t threads_;
};

/*
 * Automatically allocated vvp_named_event.
 */
class vvp_named_event_aa : सार्वजनिक vvp_named_event, सार्वजनिक automatic_hooks_s {

    सार्वजनिक:
      explicit vvp_named_event_aa(class __vpiHandle*eh);
      ~vvp_named_event_aa() override;

      शून्य alloc_instance(vvp_context_t context) override;
      शून्य reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      शून्य free_instance(vvp_context_t context) override;
#endif

      vthread_t add_waiting_thread(vthread_t thread) override;

      शून्य recv_vec4(vvp_net_ptr_t port, स्थिरांक vvp_vector4_t&bit,
                     vvp_context_t context) override;

    खाजगी:
      अचिन्हित context_idx_;
};

#endif /* IVL_event_H */
