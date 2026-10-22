#که_نه_تعریف_شوی IVL_event_H
#تعریف_کړه IVL_event_H
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

    عام:
      explicit evctl(uint64_t ecount);
      bool dec_and_پرلپسې();
      مجازي فراغ پرلپسې_پرلپسې() = 0;
      مجازي ~evctl() {}
      evctl*بل_2;

    خاص:
      uint64_t ecount_;
};

class evctl_real : عام evctl {

    عام:
      explicit evctl_real(class __vpiHandle*handle, مزدوج ارزښت,
                          uint64_t ecount);
      مجازي ~evctl_real() override {}
      فراغ پرلپسې_پرلپسې() override;

    خاص:
      class __vpiHandle*handle_;
      مزدوج ارزښت_2;
};

class evctl_ویکټور : عام evctl {

    عام:
      explicit evctl_ویکټور(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ارزښت,
                            بینښه off, بینښه wid, uint64_t ecount);
      مجازي ~evctl_ویکټور() override {}
      فراغ پرلپسې_پرلپسې() override;

    خاص:
      vvp_net_ptr_t ptr_;
      vvp_vector4_t ارزښت_2;
      بینښه off_;
      بینښه wid_;
};

class evctl_array : عام evctl {

    عام:
      explicit evctl_array(vvp_array_t حافظه, بینښه شاخص,
                           ثابت vvp_vector4_t&ارزښت, بینښه off,
                           uint64_t ecount);
      مجازي ~evctl_array() override {}
      مجازي فراغ پرلپسې_پرلپسې() override;

    خاص:
      vvp_array_t mem_;
      بینښه idx_;
      vvp_vector4_t ارزښت_2;
      بینښه off_;
};

class evctl_array_r : عام evctl {

    عام:
      explicit evctl_array_r(vvp_array_t حافظه, بینښه شاخص,
                           مزدوج ارزښت, uint64_t ecount);
      مجازي ~evctl_array_r() override {}
      مجازي فراغ پرلپسې_پرلپسې() override;

    خاص:
      vvp_array_t mem_;
      بینښه idx_;
      مزدوج ارزښت_2;
};

خارجي فراغ مهالوېش_evctl(class __vpiHandle*handle, مزدوج ارزښت,
                           vvp_net_t*event, uint64_t ecount);

خارجي فراغ مهالوېش_evctl(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ارزښت,
                           بینښه دننه_واټن_2, بینښه wid,
                           vvp_net_t*event, uint64_t ecount);

خارجي فراغ مهالوېش_evctl(vvp_array_t حافظه, بینښه شاخص,
                           ثابت vvp_vector4_t&ارزښت, بینښه دننه_واټن_2,
                           vvp_net_t*event, uint64_t ecount);

خارجي فراغ مهالوېش_evctl(vvp_array_t حافظه, بینښه شاخص,
                           مزدوج ارزښت,
                           vvp_net_t*event, uint64_t ecount);

/*
 *  Event / edge detection functors
 */

/*
 * A "waitable" functor is one that the %wait instruction can wait
 * on. This includes the infrastructure needed to hold threads.
 */
بنية waitable_hooks_s {

    عام:
      waitable_hooks_s() : event_ctls(0) { وروستی = &event_ctls; }
      مجازي ~waitable_hooks_s() {}

      مجازي vthread_t جمع_کړه_انتظار_thread(vthread_t thread) = 0;

      evctl*event_ctls;
      evctl**وروستی;

    محمي:
      فراغ پرلپسې_انتظار_تارونه(vthread_t&تارونه);
};

/*
 * This is the base object for storing state information for each instance
 * of an automatically allocated event. In the general case, all that is
 * needed is the list of threads waiting on that instance.
 */
بنية waitable_state_s {
      waitable_state_s() : تارونه(0) {}

      vthread_t تارونه;
};

/*
 * The vvp_fun_edge functor detects events that are edges of various
 * types. This should be hooked to a vvp_net_t that is connected to
 * the output of a signal that we wish to watch for edges.
 */
class vvp_fun_څنډه : عام vvp_net_fun_t, عام waitable_hooks_s {

    عام:
      تعريفنوع بینښه لنډ څنډه_t;
      explicit vvp_fun_څنډه(څنډه_t e);
      مجازي ~vvp_fun_څنډه() override;

    محمي:
      bool recv_vec4_(ثابت vvp_vector4_t&بټ,
                      vvp_bit4_t&old_بټ, vthread_t&تارونه);

      vvp_bit4_t بټونه_2[4];

    خاص:
      څنډه_t څنډه_2;
};

خارجي ثابت vvp_fun_څنډه::څنډه_t vvp_څنډه_څنډه;
خارجي ثابت vvp_fun_څنډه::څنډه_t vvp_څنډه_posedge;
خارجي ثابت vvp_fun_څنډه::څنډه_t vvp_څنډه_negedge;
خارجي ثابت vvp_fun_څنډه::څنډه_t vvp_څنډه_none;

/*
 * Statically allocated vvp_fun_edge.
 */
class vvp_fun_څنډه_sa : عام vvp_fun_څنډه {

    عام:
      explicit vvp_fun_څنډه_sa(څنډه_t e);
      مجازي ~vvp_fun_څنډه_sa() override;

      vthread_t جمع_کړه_انتظار_thread(vthread_t thread) override;

      فراغ recv_vec4(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
                     vvp_context_t context) override;
      فراغ recv_vec4_pv(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
			بینښه بنسټ, بینښه vwid, vvp_context_t context) override;

    خاص:
      vthread_t تارونه_2;
};

/*
 * Automatically allocated vvp_fun_edge.
 */
class vvp_fun_څنډه_aa : عام vvp_fun_څنډه, عام automatic_hooks_s {

    عام:
      explicit vvp_fun_څنډه_aa(څنډه_t e);
      مجازي ~vvp_fun_څنډه_aa() override;

      فراغ alloc_instance(vvp_context_t context) override;
      فراغ reset_instance(vvp_context_t context) override;
#که_تعریف_شوی CHECK_WITH_VALGRIND
      فراغ free_instance(vvp_context_t context) override;
#د_که_پای

      vthread_t جمع_کړه_انتظار_thread(vthread_t thread) override;

      فراغ recv_vec4(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
                     vvp_context_t context) override;

    خاص:
      __vpiScope*context_scope_;
      بینښه context_idx_;
};

/*
 * This is the base object for storing the last received values for a
 * vvp_fun_anyedge functor.
 */
class anyedge_ارزښت;

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
class vvp_fun_anyedge : عام vvp_net_fun_t, عام waitable_hooks_s {

    عام:
      explicit vvp_fun_anyedge();
      مجازي ~vvp_fun_anyedge() override;

    محمي:
      anyedge_ارزښت*وروستی_ارزښت[4];
};

/*
 * Statically allocated vvp_fun_anyedge.
 */
class vvp_fun_anyedge_sa : عام vvp_fun_anyedge {

    عام:
      explicit vvp_fun_anyedge_sa();
      مجازي ~vvp_fun_anyedge_sa() override;

      vthread_t جمع_کړه_انتظار_thread(vthread_t thread) override;

      فراغ recv_vec4(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
                     vvp_context_t context) override;
      فراغ recv_vec4_pv(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
			بینښه بنسټ, بینښه vwid, vvp_context_t context) override;

      فراغ recv_real(vvp_net_ptr_t پورټ, مزدوج بټ,
                     vvp_context_t context) override;

      فراغ recv_string(vvp_net_ptr_t پورټ, ثابت std::string&بټ,
		       vvp_context_t context) override;
      فراغ recv_object(vvp_net_ptr_t پورټ, vvp_object_t بټ,
		       vvp_context_t context) override;

    خاص:
      vthread_t تارونه_2;
};

/*
 * Automatically allocated vvp_fun_anyedge.
 */
class vvp_fun_anyedge_aa : عام vvp_fun_anyedge, عام automatic_hooks_s {

    عام:
      explicit vvp_fun_anyedge_aa();
      مجازي ~vvp_fun_anyedge_aa() override;

      فراغ alloc_instance(vvp_context_t context) override;
      فراغ reset_instance(vvp_context_t context) override;
#که_تعریف_شوی CHECK_WITH_VALGRIND
      فراغ free_instance(vvp_context_t context) override;
#د_که_پای

      vthread_t جمع_کړه_انتظار_thread(vthread_t thread) override;

      فراغ recv_vec4(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
                     vvp_context_t context) override;

      فراغ recv_real(vvp_net_ptr_t پورټ, مزدوج بټ,
                     vvp_context_t context) override;

      فراغ recv_string(vvp_net_ptr_t پورټ, ثابت std::string&بټ,
		       vvp_context_t context) override;

    خاص:
      __vpiScope*context_scope_;
      بینښه context_idx_;
};

/*
 * This functor triggers anytime any input is set, no matter what the
 * value. This is similar to a named event, but it has no handle. It
 * supports wide (more than 4) inputs, so needs a pointer to the base
 * net that is used for output.
 */
class vvp_fun_event_or : عام vvp_net_fun_t, عام waitable_hooks_s {

    عام:
      explicit vvp_fun_event_or(vvp_net_t*بنسټ_net);
      ~vvp_fun_event_or() override;

    محمي:
      vvp_net_t*بنسټ_net_2;
};

/*
 * Statically allocated vvp_fun_event_or.
 */
class vvp_fun_event_or_sa : عام vvp_fun_event_or {

    عام:
      explicit vvp_fun_event_or_sa(vvp_net_t*بنسټ_net);
      ~vvp_fun_event_or_sa() override;

      vthread_t جمع_کړه_انتظار_thread(vthread_t thread) override;

      فراغ recv_vec4(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
                     vvp_context_t context) override;

    خاص:
      vthread_t تارونه_2;
};

/*
 * Automatically allocated vvp_fun_event_or.
 */
class vvp_fun_event_or_aa : عام vvp_fun_event_or, عام automatic_hooks_s {

    عام:
      explicit vvp_fun_event_or_aa(vvp_net_t*بنسټ_net);
      ~vvp_fun_event_or_aa() override;

      فراغ alloc_instance(vvp_context_t context) override;
      فراغ reset_instance(vvp_context_t context) override;
#که_تعریف_شوی CHECK_WITH_VALGRIND
      فراغ free_instance(vvp_context_t context) override;
#د_که_پای

      vthread_t جمع_کړه_انتظار_thread(vthread_t thread) override;

      فراغ recv_vec4(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
                     vvp_context_t context) override;

    خاص:
      __vpiScope*context_scope_;
      بینښه context_idx_;
};

/*
 * A named event is simpler than a vvp_fun_edge in that it triggers on
 * any input at all to port-0. The idea here is that behavioral code
 * can use a %set/v instruction to trigger the event.
 */
class vvp_named_event : عام vvp_net_fun_t, عام waitable_hooks_s {

    عام:
      explicit vvp_named_event(class __vpiHandle*eh);
      ~vvp_named_event() override;

    محمي:
      class __vpiHandle*handle_;
};

/*
 * Statically allocated vvp_named_event.
 */
class vvp_named_event_sa : عام vvp_named_event {

    عام:
      explicit vvp_named_event_sa(class __vpiHandle*eh);
      ~vvp_named_event_sa() override;

      vthread_t جمع_کړه_انتظار_thread(vthread_t thread) override;

      فراغ recv_vec4(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
                     vvp_context_t) override;

    خاص:
      vthread_t تارونه_2;
};

/*
 * Automatically allocated vvp_named_event.
 */
class vvp_named_event_aa : عام vvp_named_event, عام automatic_hooks_s {

    عام:
      explicit vvp_named_event_aa(class __vpiHandle*eh);
      ~vvp_named_event_aa() override;

      فراغ alloc_instance(vvp_context_t context) override;
      فراغ reset_instance(vvp_context_t context) override;
#که_تعریف_شوی CHECK_WITH_VALGRIND
      فراغ free_instance(vvp_context_t context) override;
#د_که_پای

      vthread_t جمع_کړه_انتظار_thread(vthread_t thread) override;

      فراغ recv_vec4(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
                     vvp_context_t context) override;

    خاص:
      بینښه context_idx_;
};

#د_که_پای /* IVL_event_H */
