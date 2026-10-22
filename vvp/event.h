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

    பொது:
      explicit evctl(uint64_t ecount);
      bool dec_and_run();
      மெய்நிகர் வெறுமை run_run() = 0;
      மெய்நிகர் ~evctl() {}
      evctl*next;

    தனிப்பட்ட:
      uint64_t ecount_;
};

class evctl_real : பொது evctl {

    பொது:
      explicit evctl_real(class __vpiHandle*handle, இரட்டை மதிப்பு,
                          uint64_t ecount);
      மெய்நிகர் ~evctl_real() override {}
      வெறுமை run_run() override;

    தனிப்பட்ட:
      class __vpiHandle*handle_;
      இரட்டை மதிப்பு_2;
};

class evctl_vector : பொது evctl {

    பொது:
      explicit evctl_vector(vvp_net_ptr_t ptr, மாறிலி vvp_vector4_t&மதிப்பு,
                            unsigned off, unsigned wid, uint64_t ecount);
      மெய்நிகர் ~evctl_vector() override {}
      வெறுமை run_run() override;

    தனிப்பட்ட:
      vvp_net_ptr_t ptr_;
      vvp_vector4_t மதிப்பு_2;
      unsigned off_;
      unsigned wid_;
};

class evctl_array : பொது evctl {

    பொது:
      explicit evctl_array(vvp_array_t memory, unsigned index,
                           மாறிலி vvp_vector4_t&மதிப்பு, unsigned off,
                           uint64_t ecount);
      மெய்நிகர் ~evctl_array() override {}
      மெய்நிகர் வெறுமை run_run() override;

    தனிப்பட்ட:
      vvp_array_t mem_;
      unsigned idx_;
      vvp_vector4_t மதிப்பு_2;
      unsigned off_;
};

class evctl_array_r : பொது evctl {

    பொது:
      explicit evctl_array_r(vvp_array_t memory, unsigned index,
                           இரட்டை மதிப்பு, uint64_t ecount);
      மெய்நிகர் ~evctl_array_r() override {}
      மெய்நிகர் வெறுமை run_run() override;

    தனிப்பட்ட:
      vvp_array_t mem_;
      unsigned idx_;
      இரட்டை மதிப்பு_2;
};

வெளி வெறுமை schedule_evctl(class __vpiHandle*handle, இரட்டை மதிப்பு,
                           vvp_net_t*event, uint64_t ecount);

வெளி வெறுமை schedule_evctl(vvp_net_ptr_t ptr, மாறிலி vvp_vector4_t&மதிப்பு,
                           unsigned offset, unsigned wid,
                           vvp_net_t*event, uint64_t ecount);

வெளி வெறுமை schedule_evctl(vvp_array_t memory, unsigned index,
                           மாறிலி vvp_vector4_t&மதிப்பு, unsigned offset,
                           vvp_net_t*event, uint64_t ecount);

வெளி வெறுமை schedule_evctl(vvp_array_t memory, unsigned index,
                           இரட்டை மதிப்பு,
                           vvp_net_t*event, uint64_t ecount);

/*
 *  Event / edge detection functors
 */

/*
 * A "waitable" functor is one that the %wait instruction can wait
 * on. This includes the infrastructure needed to hold threads.
 */
கட்டமைப்பு waitable_hooks_s {

    பொது:
      waitable_hooks_s() : event_ctls(0) { last = &event_ctls; }
      மெய்நிகர் ~waitable_hooks_s() {}

      மெய்நிகர் vthread_t add_waiting_thread(vthread_t thread) = 0;

      evctl*event_ctls;
      evctl**last;

    பாதுகாக்கப்பட்ட:
      வெறுமை run_waiting_threads_(vthread_t&threads);
};

/*
 * This is the base object for storing state information for each instance
 * of an automatically allocated event. In the general case, all that is
 * needed is the list of threads waiting on that instance.
 */
கட்டமைப்பு waitable_state_s {
      waitable_state_s() : threads(0) {}

      vthread_t threads;
};

/*
 * The vvp_fun_edge functor detects events that are edges of various
 * types. This should be hooked to a vvp_net_t that is connected to
 * the output of a signal that we wish to watch for edges.
 */
class vvp_fun_edge : பொது vvp_net_fun_t, பொது waitable_hooks_s {

    பொது:
      வகைவரை unsigned short edge_t;
      explicit vvp_fun_edge(edge_t e);
      மெய்நிகர் ~vvp_fun_edge() override;

    பாதுகாக்கப்பட்ட:
      bool recv_vec4_(மாறிலி vvp_vector4_t&bit,
                      vvp_bit4_t&old_bit, vthread_t&threads);

      vvp_bit4_t bits_[4];

    தனிப்பட்ட:
      edge_t edge_;
};

வெளி மாறிலி vvp_fun_edge::edge_t vvp_edge_edge;
வெளி மாறிலி vvp_fun_edge::edge_t vvp_edge_posedge;
வெளி மாறிலி vvp_fun_edge::edge_t vvp_edge_negedge;
வெளி மாறிலி vvp_fun_edge::edge_t vvp_edge_none;

/*
 * Statically allocated vvp_fun_edge.
 */
class vvp_fun_edge_sa : பொது vvp_fun_edge {

    பொது:
      explicit vvp_fun_edge_sa(edge_t e);
      மெய்நிகர் ~vvp_fun_edge_sa() override;

      vthread_t add_waiting_thread(vthread_t thread) override;

      வெறுமை recv_vec4(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
                     vvp_context_t context) override;
      வெறுமை recv_vec4_pv(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t context) override;

    தனிப்பட்ட:
      vthread_t threads_;
};

/*
 * Automatically allocated vvp_fun_edge.
 */
class vvp_fun_edge_aa : பொது vvp_fun_edge, பொது automatic_hooks_s {

    பொது:
      explicit vvp_fun_edge_aa(edge_t e);
      மெய்நிகர் ~vvp_fun_edge_aa() override;

      வெறுமை alloc_instance(vvp_context_t context) override;
      வெறுமை reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      வெறுமை free_instance(vvp_context_t context) override;
#endif

      vthread_t add_waiting_thread(vthread_t thread) override;

      வெறுமை recv_vec4(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
                     vvp_context_t context) override;

    தனிப்பட்ட:
      __vpiScope*context_scope_;
      unsigned context_idx_;
};

/*
 * This is the base object for storing the last received values for a
 * vvp_fun_anyedge functor.
 */
class anyedge_மதிப்பு;

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
class vvp_fun_anyedge : பொது vvp_net_fun_t, பொது waitable_hooks_s {

    பொது:
      explicit vvp_fun_anyedge();
      மெய்நிகர் ~vvp_fun_anyedge() override;

    பாதுகாக்கப்பட்ட:
      anyedge_மதிப்பு*last_மதிப்பு[4];
};

/*
 * Statically allocated vvp_fun_anyedge.
 */
class vvp_fun_anyedge_sa : பொது vvp_fun_anyedge {

    பொது:
      explicit vvp_fun_anyedge_sa();
      மெய்நிகர் ~vvp_fun_anyedge_sa() override;

      vthread_t add_waiting_thread(vthread_t thread) override;

      வெறுமை recv_vec4(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
                     vvp_context_t context) override;
      வெறுமை recv_vec4_pv(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t context) override;

      வெறுமை recv_real(vvp_net_ptr_t port, இரட்டை bit,
                     vvp_context_t context) override;

      வெறுமை recv_string(vvp_net_ptr_t port, மாறிலி std::string&bit,
		       vvp_context_t context) override;
      வெறுமை recv_object(vvp_net_ptr_t port, vvp_object_t bit,
		       vvp_context_t context) override;

    தனிப்பட்ட:
      vthread_t threads_;
};

/*
 * Automatically allocated vvp_fun_anyedge.
 */
class vvp_fun_anyedge_aa : பொது vvp_fun_anyedge, பொது automatic_hooks_s {

    பொது:
      explicit vvp_fun_anyedge_aa();
      மெய்நிகர் ~vvp_fun_anyedge_aa() override;

      வெறுமை alloc_instance(vvp_context_t context) override;
      வெறுமை reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      வெறுமை free_instance(vvp_context_t context) override;
#endif

      vthread_t add_waiting_thread(vthread_t thread) override;

      வெறுமை recv_vec4(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
                     vvp_context_t context) override;

      வெறுமை recv_real(vvp_net_ptr_t port, இரட்டை bit,
                     vvp_context_t context) override;

      வெறுமை recv_string(vvp_net_ptr_t port, மாறிலி std::string&bit,
		       vvp_context_t context) override;

    தனிப்பட்ட:
      __vpiScope*context_scope_;
      unsigned context_idx_;
};

/*
 * This functor triggers anytime any input is set, no matter what the
 * value. This is similar to a named event, but it has no handle. It
 * supports wide (more than 4) inputs, so needs a pointer to the base
 * net that is used for output.
 */
class vvp_fun_event_or : பொது vvp_net_fun_t, பொது waitable_hooks_s {

    பொது:
      explicit vvp_fun_event_or(vvp_net_t*base_net);
      ~vvp_fun_event_or() override;

    பாதுகாக்கப்பட்ட:
      vvp_net_t*base_net_;
};

/*
 * Statically allocated vvp_fun_event_or.
 */
class vvp_fun_event_or_sa : பொது vvp_fun_event_or {

    பொது:
      explicit vvp_fun_event_or_sa(vvp_net_t*base_net);
      ~vvp_fun_event_or_sa() override;

      vthread_t add_waiting_thread(vthread_t thread) override;

      வெறுமை recv_vec4(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
                     vvp_context_t context) override;

    தனிப்பட்ட:
      vthread_t threads_;
};

/*
 * Automatically allocated vvp_fun_event_or.
 */
class vvp_fun_event_or_aa : பொது vvp_fun_event_or, பொது automatic_hooks_s {

    பொது:
      explicit vvp_fun_event_or_aa(vvp_net_t*base_net);
      ~vvp_fun_event_or_aa() override;

      வெறுமை alloc_instance(vvp_context_t context) override;
      வெறுமை reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      வெறுமை free_instance(vvp_context_t context) override;
#endif

      vthread_t add_waiting_thread(vthread_t thread) override;

      வெறுமை recv_vec4(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
                     vvp_context_t context) override;

    தனிப்பட்ட:
      __vpiScope*context_scope_;
      unsigned context_idx_;
};

/*
 * A named event is simpler than a vvp_fun_edge in that it triggers on
 * any input at all to port-0. The idea here is that behavioral code
 * can use a %set/v instruction to trigger the event.
 */
class vvp_named_event : பொது vvp_net_fun_t, பொது waitable_hooks_s {

    பொது:
      explicit vvp_named_event(class __vpiHandle*eh);
      ~vvp_named_event() override;

    பாதுகாக்கப்பட்ட:
      class __vpiHandle*handle_;
};

/*
 * Statically allocated vvp_named_event.
 */
class vvp_named_event_sa : பொது vvp_named_event {

    பொது:
      explicit vvp_named_event_sa(class __vpiHandle*eh);
      ~vvp_named_event_sa() override;

      vthread_t add_waiting_thread(vthread_t thread) override;

      வெறுமை recv_vec4(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
                     vvp_context_t) override;

    தனிப்பட்ட:
      vthread_t threads_;
};

/*
 * Automatically allocated vvp_named_event.
 */
class vvp_named_event_aa : பொது vvp_named_event, பொது automatic_hooks_s {

    பொது:
      explicit vvp_named_event_aa(class __vpiHandle*eh);
      ~vvp_named_event_aa() override;

      வெறுமை alloc_instance(vvp_context_t context) override;
      வெறுமை reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      வெறுமை free_instance(vvp_context_t context) override;
#endif

      vthread_t add_waiting_thread(vthread_t thread) override;

      வெறுமை recv_vec4(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
                     vvp_context_t context) override;

    தனிப்பட்ட:
      unsigned context_idx_;
};

#endif /* IVL_event_H */
