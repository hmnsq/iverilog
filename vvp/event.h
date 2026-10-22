#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ IVL_event_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IVL_event_H
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

    ਜਨਤਕ:
      explicit evctl(uint64_t ecount);
      bool dec_and_ਲੜੀਵਾਰ();
      ਆਭਾਸੀ ਖਾਲੀ ਲੜੀਵਾਰ_ਲੜੀਵਾਰ() = 0;
      ਆਭਾਸੀ ~evctl() {}
      evctl*ਅਗਲਾ_2;

    ਨਿਜੀ:
      uint64_t ecount_;
};

class evctl_real : ਜਨਤਕ evctl {

    ਜਨਤਕ:
      explicit evctl_real(class __vpiHandle*handle, ਦੁੱਗਣਾ ਮੁੱਲ,
                          uint64_t ecount);
      ਆਭਾਸੀ ~evctl_real() override {}
      ਖਾਲੀ ਲੜੀਵਾਰ_ਲੜੀਵਾਰ() override;

    ਨਿਜੀ:
      class __vpiHandle*handle_;
      ਦੁੱਗਣਾ ਮੁੱਲ_2;
};

class evctl_ਸਦਿਸ਼ : ਜਨਤਕ evctl {

    ਜਨਤਕ:
      explicit evctl_ਸਦਿਸ਼(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਮੁੱਲ,
                            ਬਿਨਾਨਿਸ਼ਾਨੀ off, ਬਿਨਾਨਿਸ਼ਾਨੀ wid, uint64_t ecount);
      ਆਭਾਸੀ ~evctl_ਸਦਿਸ਼() override {}
      ਖਾਲੀ ਲੜੀਵਾਰ_ਲੜੀਵਾਰ() override;

    ਨਿਜੀ:
      vvp_net_ptr_t ptr_;
      vvp_vector4_t ਮੁੱਲ_2;
      ਬਿਨਾਨਿਸ਼ਾਨੀ off_;
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid_;
};

class evctl_array : ਜਨਤਕ evctl {

    ਜਨਤਕ:
      explicit evctl_array(vvp_array_t ਸਮ੍ਰਿਤੀ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੂਚਕਾਂਕ,
                           ਅਚਲ vvp_vector4_t&ਮੁੱਲ, ਬਿਨਾਨਿਸ਼ਾਨੀ off,
                           uint64_t ecount);
      ਆਭਾਸੀ ~evctl_array() override {}
      ਆਭਾਸੀ ਖਾਲੀ ਲੜੀਵਾਰ_ਲੜੀਵਾਰ() override;

    ਨਿਜੀ:
      vvp_array_t mem_;
      ਬਿਨਾਨਿਸ਼ਾਨੀ idx_;
      vvp_vector4_t ਮੁੱਲ_2;
      ਬਿਨਾਨਿਸ਼ਾਨੀ off_;
};

class evctl_array_r : ਜਨਤਕ evctl {

    ਜਨਤਕ:
      explicit evctl_array_r(vvp_array_t ਸਮ੍ਰਿਤੀ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੂਚਕਾਂਕ,
                           ਦੁੱਗਣਾ ਮੁੱਲ, uint64_t ecount);
      ਆਭਾਸੀ ~evctl_array_r() override {}
      ਆਭਾਸੀ ਖਾਲੀ ਲੜੀਵਾਰ_ਲੜੀਵਾਰ() override;

    ਨਿਜੀ:
      vvp_array_t mem_;
      ਬਿਨਾਨਿਸ਼ਾਨੀ idx_;
      ਦੁੱਗਣਾ ਮੁੱਲ_2;
};

ਬਾਹਰੀ ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_evctl(class __vpiHandle*handle, ਦੁੱਗਣਾ ਮੁੱਲ,
                           vvp_net_t*event, uint64_t ecount);

ਬਾਹਰੀ ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_evctl(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਮੁੱਲ,
                           ਬਿਨਾਨਿਸ਼ਾਨੀ ਵਿੱਥ_2, ਬਿਨਾਨਿਸ਼ਾਨੀ wid,
                           vvp_net_t*event, uint64_t ecount);

ਬਾਹਰੀ ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_evctl(vvp_array_t ਸਮ੍ਰਿਤੀ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੂਚਕਾਂਕ,
                           ਅਚਲ vvp_vector4_t&ਮੁੱਲ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਵਿੱਥ_2,
                           vvp_net_t*event, uint64_t ecount);

ਬਾਹਰੀ ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_evctl(vvp_array_t ਸਮ੍ਰਿਤੀ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੂਚਕਾਂਕ,
                           ਦੁੱਗਣਾ ਮੁੱਲ,
                           vvp_net_t*event, uint64_t ecount);

/*
 *  Event / edge detection functors
 */

/*
 * A "waitable" functor is one that the %wait instruction can wait
 * on. This includes the infrastructure needed to hold threads.
 */
ਬਣਤਰ waitable_hooks_s {

    ਜਨਤਕ:
      waitable_hooks_s() : event_ctls(0) { ਆਖਰੀ = &event_ctls; }
      ਆਭਾਸੀ ~waitable_hooks_s() {}

      ਆਭਾਸੀ vthread_t ਜੋੜੋ_ਉਡੀਕ_thread(vthread_t thread) = 0;

      evctl*event_ctls;
      evctl**ਆਖਰੀ;

    ਸੁਰੱਖਿਅਤ:
      ਖਾਲੀ ਲੜੀਵਾਰ_ਉਡੀਕ_ਧਾਗੇ(vthread_t&ਧਾਗੇ);
};

/*
 * This is the base object for storing state information for each instance
 * of an automatically allocated event. In the general case, all that is
 * needed is the list of threads waiting on that instance.
 */
ਬਣਤਰ waitable_state_s {
      waitable_state_s() : ਧਾਗੇ(0) {}

      vthread_t ਧਾਗੇ;
};

/*
 * The vvp_fun_edge functor detects events that are edges of various
 * types. This should be hooked to a vvp_net_t that is connected to
 * the output of a signal that we wish to watch for edges.
 */
class vvp_fun_ਕਿਨਾਰਾ : ਜਨਤਕ vvp_net_fun_t, ਜਨਤਕ waitable_hooks_s {

    ਜਨਤਕ:
      ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ ਕਿਨਾਰਾ_t;
      explicit vvp_fun_ਕਿਨਾਰਾ(ਕਿਨਾਰਾ_t e);
      ਆਭਾਸੀ ~vvp_fun_ਕਿਨਾਰਾ() override;

    ਸੁਰੱਖਿਅਤ:
      bool recv_vec4_(ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                      vvp_bit4_t&old_ਦੋਆਧਾਰੀ_ਅੰਕ, vthread_t&ਧਾਗੇ);

      vvp_bit4_t ਬਿੱਟ_2[4];

    ਨਿਜੀ:
      ਕਿਨਾਰਾ_t ਕਿਨਾਰਾ_2;
};

ਬਾਹਰੀ ਅਚਲ vvp_fun_ਕਿਨਾਰਾ::ਕਿਨਾਰਾ_t vvp_ਕਿਨਾਰਾ_ਕਿਨਾਰਾ;
ਬਾਹਰੀ ਅਚਲ vvp_fun_ਕਿਨਾਰਾ::ਕਿਨਾਰਾ_t vvp_ਕਿਨਾਰਾ_posedge;
ਬਾਹਰੀ ਅਚਲ vvp_fun_ਕਿਨਾਰਾ::ਕਿਨਾਰਾ_t vvp_ਕਿਨਾਰਾ_negedge;
ਬਾਹਰੀ ਅਚਲ vvp_fun_ਕਿਨਾਰਾ::ਕਿਨਾਰਾ_t vvp_ਕਿਨਾਰਾ_none;

/*
 * Statically allocated vvp_fun_edge.
 */
class vvp_fun_ਕਿਨਾਰਾ_sa : ਜਨਤਕ vvp_fun_ਕਿਨਾਰਾ {

    ਜਨਤਕ:
      explicit vvp_fun_ਕਿਨਾਰਾ_sa(ਕਿਨਾਰਾ_t e);
      ਆਭਾਸੀ ~vvp_fun_ਕਿਨਾਰਾ_sa() override;

      vthread_t ਜੋੜੋ_ਉਡੀਕ_thread(vthread_t thread) override;

      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t context) override;
      ਖਾਲੀ recv_vec4_pv(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t context) override;

    ਨਿਜੀ:
      vthread_t ਧਾਗੇ_2;
};

/*
 * Automatically allocated vvp_fun_edge.
 */
class vvp_fun_ਕਿਨਾਰਾ_aa : ਜਨਤਕ vvp_fun_ਕਿਨਾਰਾ, ਜਨਤਕ automatic_hooks_s {

    ਜਨਤਕ:
      explicit vvp_fun_ਕਿਨਾਰਾ_aa(ਕਿਨਾਰਾ_t e);
      ਆਭਾਸੀ ~vvp_fun_ਕਿਨਾਰਾ_aa() override;

      ਖਾਲੀ alloc_instance(vvp_context_t context) override;
      ਖਾਲੀ reset_instance(vvp_context_t context) override;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
      ਖਾਲੀ free_instance(vvp_context_t context) override;
#ਜੇ_ਅੰਤ

      vthread_t ਜੋੜੋ_ਉਡੀਕ_thread(vthread_t thread) override;

      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t context) override;

    ਨਿਜੀ:
      __vpiScope*context_scope_;
      ਬਿਨਾਨਿਸ਼ਾਨੀ context_idx_;
};

/*
 * This is the base object for storing the last received values for a
 * vvp_fun_anyedge functor.
 */
class anyedge_ਮੁੱਲ;

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
class vvp_fun_anyedge : ਜਨਤਕ vvp_net_fun_t, ਜਨਤਕ waitable_hooks_s {

    ਜਨਤਕ:
      explicit vvp_fun_anyedge();
      ਆਭਾਸੀ ~vvp_fun_anyedge() override;

    ਸੁਰੱਖਿਅਤ:
      anyedge_ਮੁੱਲ*ਆਖਰੀ_ਮੁੱਲ[4];
};

/*
 * Statically allocated vvp_fun_anyedge.
 */
class vvp_fun_anyedge_sa : ਜਨਤਕ vvp_fun_anyedge {

    ਜਨਤਕ:
      explicit vvp_fun_anyedge_sa();
      ਆਭਾਸੀ ~vvp_fun_anyedge_sa() override;

      vthread_t ਜੋੜੋ_ਉਡੀਕ_thread(vthread_t thread) override;

      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t context) override;
      ਖਾਲੀ recv_vec4_pv(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t context) override;

      ਖਾਲੀ recv_real(vvp_net_ptr_t ਪੋਰਟ, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t context) override;

      ਖਾਲੀ recv_string(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ std::string&ਦੋਆਧਾਰੀ_ਅੰਕ,
		       vvp_context_t context) override;
      ਖਾਲੀ recv_object(vvp_net_ptr_t ਪੋਰਟ, vvp_object_t ਦੋਆਧਾਰੀ_ਅੰਕ,
		       vvp_context_t context) override;

    ਨਿਜੀ:
      vthread_t ਧਾਗੇ_2;
};

/*
 * Automatically allocated vvp_fun_anyedge.
 */
class vvp_fun_anyedge_aa : ਜਨਤਕ vvp_fun_anyedge, ਜਨਤਕ automatic_hooks_s {

    ਜਨਤਕ:
      explicit vvp_fun_anyedge_aa();
      ਆਭਾਸੀ ~vvp_fun_anyedge_aa() override;

      ਖਾਲੀ alloc_instance(vvp_context_t context) override;
      ਖਾਲੀ reset_instance(vvp_context_t context) override;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
      ਖਾਲੀ free_instance(vvp_context_t context) override;
#ਜੇ_ਅੰਤ

      vthread_t ਜੋੜੋ_ਉਡੀਕ_thread(vthread_t thread) override;

      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t context) override;

      ਖਾਲੀ recv_real(vvp_net_ptr_t ਪੋਰਟ, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t context) override;

      ਖਾਲੀ recv_string(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ std::string&ਦੋਆਧਾਰੀ_ਅੰਕ,
		       vvp_context_t context) override;

    ਨਿਜੀ:
      __vpiScope*context_scope_;
      ਬਿਨਾਨਿਸ਼ਾਨੀ context_idx_;
};

/*
 * This functor triggers anytime any input is set, no matter what the
 * value. This is similar to a named event, but it has no handle. It
 * supports wide (more than 4) inputs, so needs a pointer to the base
 * net that is used for output.
 */
class vvp_fun_event_or : ਜਨਤਕ vvp_net_fun_t, ਜਨਤਕ waitable_hooks_s {

    ਜਨਤਕ:
      explicit vvp_fun_event_or(vvp_net_t*ਅਧਾਰ_net);
      ~vvp_fun_event_or() override;

    ਸੁਰੱਖਿਅਤ:
      vvp_net_t*ਅਧਾਰ_net_2;
};

/*
 * Statically allocated vvp_fun_event_or.
 */
class vvp_fun_event_or_sa : ਜਨਤਕ vvp_fun_event_or {

    ਜਨਤਕ:
      explicit vvp_fun_event_or_sa(vvp_net_t*ਅਧਾਰ_net);
      ~vvp_fun_event_or_sa() override;

      vthread_t ਜੋੜੋ_ਉਡੀਕ_thread(vthread_t thread) override;

      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t context) override;

    ਨਿਜੀ:
      vthread_t ਧਾਗੇ_2;
};

/*
 * Automatically allocated vvp_fun_event_or.
 */
class vvp_fun_event_or_aa : ਜਨਤਕ vvp_fun_event_or, ਜਨਤਕ automatic_hooks_s {

    ਜਨਤਕ:
      explicit vvp_fun_event_or_aa(vvp_net_t*ਅਧਾਰ_net);
      ~vvp_fun_event_or_aa() override;

      ਖਾਲੀ alloc_instance(vvp_context_t context) override;
      ਖਾਲੀ reset_instance(vvp_context_t context) override;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
      ਖਾਲੀ free_instance(vvp_context_t context) override;
#ਜੇ_ਅੰਤ

      vthread_t ਜੋੜੋ_ਉਡੀਕ_thread(vthread_t thread) override;

      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t context) override;

    ਨਿਜੀ:
      __vpiScope*context_scope_;
      ਬਿਨਾਨਿਸ਼ਾਨੀ context_idx_;
};

/*
 * A named event is simpler than a vvp_fun_edge in that it triggers on
 * any input at all to port-0. The idea here is that behavioral code
 * can use a %set/v instruction to trigger the event.
 */
class vvp_named_event : ਜਨਤਕ vvp_net_fun_t, ਜਨਤਕ waitable_hooks_s {

    ਜਨਤਕ:
      explicit vvp_named_event(class __vpiHandle*eh);
      ~vvp_named_event() override;

    ਸੁਰੱਖਿਅਤ:
      class __vpiHandle*handle_;
};

/*
 * Statically allocated vvp_named_event.
 */
class vvp_named_event_sa : ਜਨਤਕ vvp_named_event {

    ਜਨਤਕ:
      explicit vvp_named_event_sa(class __vpiHandle*eh);
      ~vvp_named_event_sa() override;

      vthread_t ਜੋੜੋ_ਉਡੀਕ_thread(vthread_t thread) override;

      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;

    ਨਿਜੀ:
      vthread_t ਧਾਗੇ_2;
};

/*
 * Automatically allocated vvp_named_event.
 */
class vvp_named_event_aa : ਜਨਤਕ vvp_named_event, ਜਨਤਕ automatic_hooks_s {

    ਜਨਤਕ:
      explicit vvp_named_event_aa(class __vpiHandle*eh);
      ~vvp_named_event_aa() override;

      ਖਾਲੀ alloc_instance(vvp_context_t context) override;
      ਖਾਲੀ reset_instance(vvp_context_t context) override;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
      ਖਾਲੀ free_instance(vvp_context_t context) override;
#ਜੇ_ਅੰਤ

      vthread_t ਜੋੜੋ_ਉਡੀਕ_thread(vthread_t thread) override;

      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t context) override;

    ਨਿਜੀ:
      ਬਿਨਾਨਿਸ਼ਾਨੀ context_idx_;
};

#ਜੇ_ਅੰਤ /* IVL_event_H */
