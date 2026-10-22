#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ IVL_event_H
#ವ್ಯಾಖ್ಯಾನಿಸು IVL_event_H
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
      bool dec_and_ಸತತ();
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಸತತ_ಸತತ() = 0;
      ವಾಸ್ತವಿಕ ~evctl() {}
      evctl*ಮುಂದಿನ_2;

    ಖಾಸಗಿ:
      uint64_t ecount_;
};

class evctl_real : ಸಾರ್ವಜನಿಕ evctl {

    ಸಾರ್ವಜನಿಕ:
      explicit evctl_real(class __vpiHandle*handle, ದ್ವಿಗುಣ ಮೌಲ್ಯ,
                          uint64_t ecount);
      ವಾಸ್ತವಿಕ ~evctl_real() override {}
      ಶೂನ್ಯ ಸತತ_ಸತತ() override;

    ಖಾಸಗಿ:
      class __vpiHandle*handle_;
      ದ್ವಿಗುಣ ಮೌಲ್ಯ_2;
};

class evctl_ಸದಿಶ : ಸಾರ್ವಜನಿಕ evctl {

    ಸಾರ್ವಜನಿಕ:
      explicit evctl_ಸದಿಶ(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಮೌಲ್ಯ,
                            ಚಿಹ್ನೆರಹಿತ off, ಚಿಹ್ನೆರಹಿತ wid, uint64_t ecount);
      ವಾಸ್ತವಿಕ ~evctl_ಸದಿಶ() override {}
      ಶೂನ್ಯ ಸತತ_ಸತತ() override;

    ಖಾಸಗಿ:
      vvp_net_ptr_t ptr_;
      vvp_vector4_t ಮೌಲ್ಯ_2;
      ಚಿಹ್ನೆರಹಿತ off_;
      ಚಿಹ್ನೆರಹಿತ wid_;
};

class evctl_array : ಸಾರ್ವಜನಿಕ evctl {

    ಸಾರ್ವಜನಿಕ:
      explicit evctl_array(vvp_array_t ಸ್ಮರಣೆ, ಚಿಹ್ನೆರಹಿತ ಸೂಚ್ಯಂಕ,
                           ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಮೌಲ್ಯ, ಚಿಹ್ನೆರಹಿತ off,
                           uint64_t ecount);
      ವಾಸ್ತವಿಕ ~evctl_array() override {}
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಸತತ_ಸತತ() override;

    ಖಾಸಗಿ:
      vvp_array_t mem_;
      ಚಿಹ್ನೆರಹಿತ idx_;
      vvp_vector4_t ಮೌಲ್ಯ_2;
      ಚಿಹ್ನೆರಹಿತ off_;
};

class evctl_array_r : ಸಾರ್ವಜನಿಕ evctl {

    ಸಾರ್ವಜನಿಕ:
      explicit evctl_array_r(vvp_array_t ಸ್ಮರಣೆ, ಚಿಹ್ನೆರಹಿತ ಸೂಚ್ಯಂಕ,
                           ದ್ವಿಗುಣ ಮೌಲ್ಯ, uint64_t ecount);
      ವಾಸ್ತವಿಕ ~evctl_array_r() override {}
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಸತತ_ಸತತ() override;

    ಖಾಸಗಿ:
      vvp_array_t mem_;
      ಚಿಹ್ನೆರಹಿತ idx_;
      ದ್ವಿಗುಣ ಮೌಲ್ಯ_2;
};

ಬಾಹ್ಯ ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_evctl(class __vpiHandle*handle, ದ್ವಿಗುಣ ಮೌಲ್ಯ,
                           vvp_net_t*event, uint64_t ecount);

ಬಾಹ್ಯ ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_evctl(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಮೌಲ್ಯ,
                           ಚಿಹ್ನೆರಹಿತ ಸ್ಥಾನಾಂತರ_2, ಚಿಹ್ನೆರಹಿತ wid,
                           vvp_net_t*event, uint64_t ecount);

ಬಾಹ್ಯ ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_evctl(vvp_array_t ಸ್ಮರಣೆ, ಚಿಹ್ನೆರಹಿತ ಸೂಚ್ಯಂಕ,
                           ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಮೌಲ್ಯ, ಚಿಹ್ನೆರಹಿತ ಸ್ಥಾನಾಂತರ_2,
                           vvp_net_t*event, uint64_t ecount);

ಬಾಹ್ಯ ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_evctl(vvp_array_t ಸ್ಮರಣೆ, ಚಿಹ್ನೆರಹಿತ ಸೂಚ್ಯಂಕ,
                           ದ್ವಿಗುಣ ಮೌಲ್ಯ,
                           vvp_net_t*event, uint64_t ecount);

/*
 *  Event / edge detection functors
 */

/*
 * A "waitable" functor is one that the %wait instruction can wait
 * on. This includes the infrastructure needed to hold threads.
 */
ರಚನೆ waitable_hooks_s {

    ಸಾರ್ವಜನಿಕ:
      waitable_hooks_s() : event_ctls(0) { ಕೊನೆಯ = &event_ctls; }
      ವಾಸ್ತವಿಕ ~waitable_hooks_s() {}

      ವಾಸ್ತವಿಕ vthread_t ಕೂಡಿಸು_ಕಾಯುವಿಕೆ_thread(vthread_t thread) = 0;

      evctl*event_ctls;
      evctl**ಕೊನೆಯ;

    ಸಂರಕ್ಷಿತ:
      ಶೂನ್ಯ ಸತತ_ಕಾಯುವಿಕೆ_ಎಳೆಗಳು(vthread_t&ಎಳೆಗಳು);
};

/*
 * This is the base object for storing state information for each instance
 * of an automatically allocated event. In the general case, all that is
 * needed is the list of threads waiting on that instance.
 */
ರಚನೆ waitable_state_s {
      waitable_state_s() : ಎಳೆಗಳು(0) {}

      vthread_t ಎಳೆಗಳು;
};

/*
 * The vvp_fun_edge functor detects events that are edges of various
 * types. This should be hooked to a vvp_net_t that is connected to
 * the output of a signal that we wish to watch for edges.
 */
class vvp_fun_ಅಂಚು : ಸಾರ್ವಜನಿಕ vvp_net_fun_t, ಸಾರ್ವಜನಿಕ waitable_hooks_s {

    ಸಾರ್ವಜನಿಕ:
      ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ ಅಂಚು_t;
      explicit vvp_fun_ಅಂಚು(ಅಂಚು_t e);
      ವಾಸ್ತವಿಕ ~vvp_fun_ಅಂಚು() override;

    ಸಂರಕ್ಷಿತ:
      bool recv_vec4_(ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                      vvp_bit4_t&old_ಬಿಟ್, vthread_t&ಎಳೆಗಳು);

      vvp_bit4_t ಬಿಟ್ಟುಗಳು_2[4];

    ಖಾಸಗಿ:
      ಅಂಚು_t ಅಂಚು_2;
};

ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ vvp_fun_ಅಂಚು::ಅಂಚು_t vvp_ಅಂಚು_ಅಂಚು;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ vvp_fun_ಅಂಚು::ಅಂಚು_t vvp_ಅಂಚು_posedge;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ vvp_fun_ಅಂಚು::ಅಂಚು_t vvp_ಅಂಚು_negedge;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ vvp_fun_ಅಂಚು::ಅಂಚು_t vvp_ಅಂಚು_none;

/*
 * Statically allocated vvp_fun_edge.
 */
class vvp_fun_ಅಂಚು_sa : ಸಾರ್ವಜನಿಕ vvp_fun_ಅಂಚು {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_ಅಂಚು_sa(ಅಂಚು_t e);
      ವಾಸ್ತವಿಕ ~vvp_fun_ಅಂಚು_sa() override;

      vthread_t ಕೂಡಿಸು_ಕಾಯುವಿಕೆ_thread(vthread_t thread) override;

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                     vvp_context_t context) override;
      ಶೂನ್ಯ recv_vec4_pv(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
			ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t context) override;

    ಖಾಸಗಿ:
      vthread_t ಎಳೆಗಳು_2;
};

/*
 * Automatically allocated vvp_fun_edge.
 */
class vvp_fun_ಅಂಚು_aa : ಸಾರ್ವಜನಿಕ vvp_fun_ಅಂಚು, ಸಾರ್ವಜನಿಕ automatic_hooks_s {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_ಅಂಚು_aa(ಅಂಚು_t e);
      ವಾಸ್ತವಿಕ ~vvp_fun_ಅಂಚು_aa() override;

      ಶೂನ್ಯ alloc_instance(vvp_context_t context) override;
      ಶೂನ್ಯ reset_instance(vvp_context_t context) override;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
      ಶೂನ್ಯ free_instance(vvp_context_t context) override;
#ಆದರೆ_ಅಂತ್ಯ

      vthread_t ಕೂಡಿಸು_ಕಾಯುವಿಕೆ_thread(vthread_t thread) override;

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                     vvp_context_t context) override;

    ಖಾಸಗಿ:
      __vpiScope*context_scope_;
      ಚಿಹ್ನೆರಹಿತ context_idx_;
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
      ವಾಸ್ತವಿಕ ~vvp_fun_anyedge() override;

    ಸಂರಕ್ಷಿತ:
      anyedge_ಮೌಲ್ಯ*ಕೊನೆಯ_ಮೌಲ್ಯ[4];
};

/*
 * Statically allocated vvp_fun_anyedge.
 */
class vvp_fun_anyedge_sa : ಸಾರ್ವಜನಿಕ vvp_fun_anyedge {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_anyedge_sa();
      ವಾಸ್ತವಿಕ ~vvp_fun_anyedge_sa() override;

      vthread_t ಕೂಡಿಸು_ಕಾಯುವಿಕೆ_thread(vthread_t thread) override;

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                     vvp_context_t context) override;
      ಶೂನ್ಯ recv_vec4_pv(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
			ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t context) override;

      ಶೂನ್ಯ recv_real(vvp_net_ptr_t ಪೋರ್ಟ್, ದ್ವಿಗುಣ ಬಿಟ್,
                     vvp_context_t context) override;

      ಶೂನ್ಯ recv_string(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ std::string&ಬಿಟ್,
		       vvp_context_t context) override;
      ಶೂನ್ಯ recv_object(vvp_net_ptr_t ಪೋರ್ಟ್, vvp_object_t ಬಿಟ್,
		       vvp_context_t context) override;

    ಖಾಸಗಿ:
      vthread_t ಎಳೆಗಳು_2;
};

/*
 * Automatically allocated vvp_fun_anyedge.
 */
class vvp_fun_anyedge_aa : ಸಾರ್ವಜನಿಕ vvp_fun_anyedge, ಸಾರ್ವಜನಿಕ automatic_hooks_s {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_anyedge_aa();
      ವಾಸ್ತವಿಕ ~vvp_fun_anyedge_aa() override;

      ಶೂನ್ಯ alloc_instance(vvp_context_t context) override;
      ಶೂನ್ಯ reset_instance(vvp_context_t context) override;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
      ಶೂನ್ಯ free_instance(vvp_context_t context) override;
#ಆದರೆ_ಅಂತ್ಯ

      vthread_t ಕೂಡಿಸು_ಕಾಯುವಿಕೆ_thread(vthread_t thread) override;

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                     vvp_context_t context) override;

      ಶೂನ್ಯ recv_real(vvp_net_ptr_t ಪೋರ್ಟ್, ದ್ವಿಗುಣ ಬಿಟ್,
                     vvp_context_t context) override;

      ಶೂನ್ಯ recv_string(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ std::string&ಬಿಟ್,
		       vvp_context_t context) override;

    ಖಾಸಗಿ:
      __vpiScope*context_scope_;
      ಚಿಹ್ನೆರಹಿತ context_idx_;
};

/*
 * This functor triggers anytime any input is set, no matter what the
 * value. This is similar to a named event, but it has no handle. It
 * supports wide (more than 4) inputs, so needs a pointer to the base
 * net that is used for output.
 */
class vvp_fun_event_or : ಸಾರ್ವಜನಿಕ vvp_net_fun_t, ಸಾರ್ವಜನಿಕ waitable_hooks_s {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_event_or(vvp_net_t*ತಳ_net);
      ~vvp_fun_event_or() override;

    ಸಂರಕ್ಷಿತ:
      vvp_net_t*ತಳ_net_2;
};

/*
 * Statically allocated vvp_fun_event_or.
 */
class vvp_fun_event_or_sa : ಸಾರ್ವಜನಿಕ vvp_fun_event_or {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_event_or_sa(vvp_net_t*ತಳ_net);
      ~vvp_fun_event_or_sa() override;

      vthread_t ಕೂಡಿಸು_ಕಾಯುವಿಕೆ_thread(vthread_t thread) override;

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                     vvp_context_t context) override;

    ಖಾಸಗಿ:
      vthread_t ಎಳೆಗಳು_2;
};

/*
 * Automatically allocated vvp_fun_event_or.
 */
class vvp_fun_event_or_aa : ಸಾರ್ವಜನಿಕ vvp_fun_event_or, ಸಾರ್ವಜನಿಕ automatic_hooks_s {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_event_or_aa(vvp_net_t*ತಳ_net);
      ~vvp_fun_event_or_aa() override;

      ಶೂನ್ಯ alloc_instance(vvp_context_t context) override;
      ಶೂನ್ಯ reset_instance(vvp_context_t context) override;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
      ಶೂನ್ಯ free_instance(vvp_context_t context) override;
#ಆದರೆ_ಅಂತ್ಯ

      vthread_t ಕೂಡಿಸು_ಕಾಯುವಿಕೆ_thread(vthread_t thread) override;

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                     vvp_context_t context) override;

    ಖಾಸಗಿ:
      __vpiScope*context_scope_;
      ಚಿಹ್ನೆರಹಿತ context_idx_;
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

    ಸಂರಕ್ಷಿತ:
      class __vpiHandle*handle_;
};

/*
 * Statically allocated vvp_named_event.
 */
class vvp_named_event_sa : ಸಾರ್ವಜನಿಕ vvp_named_event {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_named_event_sa(class __vpiHandle*eh);
      ~vvp_named_event_sa() override;

      vthread_t ಕೂಡಿಸು_ಕಾಯುವಿಕೆ_thread(vthread_t thread) override;

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                     vvp_context_t) override;

    ಖಾಸಗಿ:
      vthread_t ಎಳೆಗಳು_2;
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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
      ಶೂನ್ಯ free_instance(vvp_context_t context) override;
#ಆದರೆ_ಅಂತ್ಯ

      vthread_t ಕೂಡಿಸು_ಕಾಯುವಿಕೆ_thread(vthread_t thread) override;

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                     vvp_context_t context) override;

    ಖಾಸಗಿ:
      ಚಿಹ್ನೆರಹಿತ context_idx_;
};

#ಆದರೆ_ಅಂತ್ಯ /* IVL_event_H */
