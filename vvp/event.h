#यदि_अपरिभाषितम् IVL_event_H
#परिभाषय IVL_event_H
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
      bool dec_and_सततम्();
      आभास शून्य सततम्_सततम्() = 0;
      आभास ~evctl() {}
      evctl*अग्र_2;

    गुप्त:
      uint64_t ecount_;
};

class evctl_real : सार्वजनिक evctl {

    सार्वजनिक:
      explicit evctl_real(class __vpiHandle*handle, द्विगुण मानम्,
                          uint64_t ecount);
      आभास ~evctl_real() override {}
      शून्य सततम्_सततम्() override;

    गुप्त:
      class __vpiHandle*handle_;
      द्विगुण मानम्_2;
};

class evctl_सदिशम् : सार्वजनिक evctl {

    सार्वजनिक:
      explicit evctl_सदिशम्(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&मानम्,
                            अचिह्नित off, अचिह्नित wid, uint64_t ecount);
      आभास ~evctl_सदिशम्() override {}
      शून्य सततम्_सततम्() override;

    गुप्त:
      vvp_net_ptr_t ptr_;
      vvp_vector4_t मानम्_2;
      अचिह्नित off_;
      अचिह्नित wid_;
};

class evctl_array : सार्वजनिक evctl {

    सार्वजनिक:
      explicit evctl_array(vvp_array_t स्मृतिः, अचिह्नित सूचकांक,
                           ध्रुव vvp_vector4_t&मानम्, अचिह्नित off,
                           uint64_t ecount);
      आभास ~evctl_array() override {}
      आभास शून्य सततम्_सततम्() override;

    गुप्त:
      vvp_array_t mem_;
      अचिह्नित idx_;
      vvp_vector4_t मानम्_2;
      अचिह्नित off_;
};

class evctl_array_r : सार्वजनिक evctl {

    सार्वजनिक:
      explicit evctl_array_r(vvp_array_t स्मृतिः, अचिह्नित सूचकांक,
                           द्विगुण मानम्, uint64_t ecount);
      आभास ~evctl_array_r() override {}
      आभास शून्य सततम्_सततम्() override;

    गुप्त:
      vvp_array_t mem_;
      अचिह्नित idx_;
      द्विगुण मानम्_2;
};

बाह्य शून्य कालसारणी_evctl(class __vpiHandle*handle, द्विगुण मानम्,
                           vvp_net_t*event, uint64_t ecount);

बाह्य शून्य कालसारणी_evctl(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&मानम्,
                           अचिह्नित विस्थापनम्_2, अचिह्नित wid,
                           vvp_net_t*event, uint64_t ecount);

बाह्य शून्य कालसारणी_evctl(vvp_array_t स्मृतिः, अचिह्नित सूचकांक,
                           ध्रुव vvp_vector4_t&मानम्, अचिह्नित विस्थापनम्_2,
                           vvp_net_t*event, uint64_t ecount);

बाह्य शून्य कालसारणी_evctl(vvp_array_t स्मृतिः, अचिह्नित सूचकांक,
                           द्विगुण मानम्,
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
      waitable_hooks_s() : event_ctls(0) { चरमः = &event_ctls; }
      आभास ~waitable_hooks_s() {}

      आभास vthread_t सङ्कलय_प्रतीक्षा_thread(vthread_t thread) = 0;

      evctl*event_ctls;
      evctl**चरमः;

    रक्षित:
      शून्य सततम्_प्रतीक्षा_तन्तवः(vthread_t&तन्तवः);
};

/*
 * This is the base object for storing state information for each instance
 * of an automatically allocated event. In the general case, all that is
 * needed is the list of threads waiting on that instance.
 */
संरचना waitable_state_s {
      waitable_state_s() : तन्तवः(0) {}

      vthread_t तन्तवः;
};

/*
 * The vvp_fun_edge functor detects events that are edges of various
 * types. This should be hooked to a vvp_net_t that is connected to
 * the output of a signal that we wish to watch for edges.
 */
class vvp_fun_edge : सार्वजनिक vvp_net_fun_t, सार्वजनिक waitable_hooks_s {

    सार्वजनिक:
      प्रकारनिर्देश अचिह्नित ह्रस्व edge_t;
      explicit vvp_fun_edge(edge_t e);
      आभास ~vvp_fun_edge() override;

    रक्षित:
      bool recv_vec4_(ध्रुव vvp_vector4_t&bit,
                      vvp_bit4_t&old_bit, vthread_t&तन्तवः);

      vvp_bit4_t द्वयङ्काः_2[4];

    गुप्त:
      edge_t edge_;
};

बाह्य ध्रुव vvp_fun_edge::edge_t vvp_edge_edge;
बाह्य ध्रुव vvp_fun_edge::edge_t vvp_edge_posedge;
बाह्य ध्रुव vvp_fun_edge::edge_t vvp_edge_negedge;
बाह्य ध्रुव vvp_fun_edge::edge_t vvp_edge_none;

/*
 * Statically allocated vvp_fun_edge.
 */
class vvp_fun_edge_sa : सार्वजनिक vvp_fun_edge {

    सार्वजनिक:
      explicit vvp_fun_edge_sa(edge_t e);
      आभास ~vvp_fun_edge_sa() override;

      vthread_t सङ्कलय_प्रतीक्षा_thread(vthread_t thread) override;

      शून्य recv_vec4(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t context) override;
      शून्य recv_vec4_pv(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
			अचिह्नित आधारः, अचिह्नित vwid, vvp_context_t context) override;

    गुप्त:
      vthread_t तन्तवः_2;
};

/*
 * Automatically allocated vvp_fun_edge.
 */
class vvp_fun_edge_aa : सार्वजनिक vvp_fun_edge, सार्वजनिक automatic_hooks_s {

    सार्वजनिक:
      explicit vvp_fun_edge_aa(edge_t e);
      आभास ~vvp_fun_edge_aa() override;

      शून्य alloc_instance(vvp_context_t context) override;
      शून्य reset_instance(vvp_context_t context) override;
#यदि_परिभाषितम् CHECK_WITH_VALGRIND
      शून्य free_instance(vvp_context_t context) override;
#यदि_अन्तः

      vthread_t सङ्कलय_प्रतीक्षा_thread(vthread_t thread) override;

      शून्य recv_vec4(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t context) override;

    गुप्त:
      __vpiScope*context_scope_;
      अचिह्नित context_idx_;
};

/*
 * This is the base object for storing the last received values for a
 * vvp_fun_anyedge functor.
 */
class anyedge_मानम्;

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
      आभास ~vvp_fun_anyedge() override;

    रक्षित:
      anyedge_मानम्*चरमः_मानम्[4];
};

/*
 * Statically allocated vvp_fun_anyedge.
 */
class vvp_fun_anyedge_sa : सार्वजनिक vvp_fun_anyedge {

    सार्वजनिक:
      explicit vvp_fun_anyedge_sa();
      आभास ~vvp_fun_anyedge_sa() override;

      vthread_t सङ्कलय_प्रतीक्षा_thread(vthread_t thread) override;

      शून्य recv_vec4(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t context) override;
      शून्य recv_vec4_pv(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
			अचिह्नित आधारः, अचिह्नित vwid, vvp_context_t context) override;

      शून्य recv_real(vvp_net_ptr_t पत्तनम्, द्विगुण bit,
                     vvp_context_t context) override;

      शून्य recv_string(vvp_net_ptr_t पत्तनम्, ध्रुव std::string&bit,
		       vvp_context_t context) override;
      शून्य recv_object(vvp_net_ptr_t पत्तनम्, vvp_object_t bit,
		       vvp_context_t context) override;

    गुप्त:
      vthread_t तन्तवः_2;
};

/*
 * Automatically allocated vvp_fun_anyedge.
 */
class vvp_fun_anyedge_aa : सार्वजनिक vvp_fun_anyedge, सार्वजनिक automatic_hooks_s {

    सार्वजनिक:
      explicit vvp_fun_anyedge_aa();
      आभास ~vvp_fun_anyedge_aa() override;

      शून्य alloc_instance(vvp_context_t context) override;
      शून्य reset_instance(vvp_context_t context) override;
#यदि_परिभाषितम् CHECK_WITH_VALGRIND
      शून्य free_instance(vvp_context_t context) override;
#यदि_अन्तः

      vthread_t सङ्कलय_प्रतीक्षा_thread(vthread_t thread) override;

      शून्य recv_vec4(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t context) override;

      शून्य recv_real(vvp_net_ptr_t पत्तनम्, द्विगुण bit,
                     vvp_context_t context) override;

      शून्य recv_string(vvp_net_ptr_t पत्तनम्, ध्रुव std::string&bit,
		       vvp_context_t context) override;

    गुप्त:
      __vpiScope*context_scope_;
      अचिह्नित context_idx_;
};

/*
 * This functor triggers anytime any input is set, no matter what the
 * value. This is similar to a named event, but it has no handle. It
 * supports wide (more than 4) inputs, so needs a pointer to the base
 * net that is used for output.
 */
class vvp_fun_event_or : सार्वजनिक vvp_net_fun_t, सार्वजनिक waitable_hooks_s {

    सार्वजनिक:
      explicit vvp_fun_event_or(vvp_net_t*आधारः_net);
      ~vvp_fun_event_or() override;

    रक्षित:
      vvp_net_t*आधारः_net_2;
};

/*
 * Statically allocated vvp_fun_event_or.
 */
class vvp_fun_event_or_sa : सार्वजनिक vvp_fun_event_or {

    सार्वजनिक:
      explicit vvp_fun_event_or_sa(vvp_net_t*आधारः_net);
      ~vvp_fun_event_or_sa() override;

      vthread_t सङ्कलय_प्रतीक्षा_thread(vthread_t thread) override;

      शून्य recv_vec4(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t context) override;

    गुप्त:
      vthread_t तन्तवः_2;
};

/*
 * Automatically allocated vvp_fun_event_or.
 */
class vvp_fun_event_or_aa : सार्वजनिक vvp_fun_event_or, सार्वजनिक automatic_hooks_s {

    सार्वजनिक:
      explicit vvp_fun_event_or_aa(vvp_net_t*आधारः_net);
      ~vvp_fun_event_or_aa() override;

      शून्य alloc_instance(vvp_context_t context) override;
      शून्य reset_instance(vvp_context_t context) override;
#यदि_परिभाषितम् CHECK_WITH_VALGRIND
      शून्य free_instance(vvp_context_t context) override;
#यदि_अन्तः

      vthread_t सङ्कलय_प्रतीक्षा_thread(vthread_t thread) override;

      शून्य recv_vec4(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t context) override;

    गुप्त:
      __vpiScope*context_scope_;
      अचिह्नित context_idx_;
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

    रक्षित:
      class __vpiHandle*handle_;
};

/*
 * Statically allocated vvp_named_event.
 */
class vvp_named_event_sa : सार्वजनिक vvp_named_event {

    सार्वजनिक:
      explicit vvp_named_event_sa(class __vpiHandle*eh);
      ~vvp_named_event_sa() override;

      vthread_t सङ्कलय_प्रतीक्षा_thread(vthread_t thread) override;

      शून्य recv_vec4(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t) override;

    गुप्त:
      vthread_t तन्तवः_2;
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
#यदि_परिभाषितम् CHECK_WITH_VALGRIND
      शून्य free_instance(vvp_context_t context) override;
#यदि_अन्तः

      vthread_t सङ्कलय_प्रतीक्षा_thread(vthread_t thread) override;

      शून्य recv_vec4(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t context) override;

    गुप्त:
      अचिह्नित context_idx_;
};

#यदि_अन्तः /* IVL_event_H */
