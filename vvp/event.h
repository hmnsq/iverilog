#اگر_غیر_تعریف_شدہ IVL_event_H
#تعریف_کرو IVL_event_H
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

    عوامی:
      explicit evctl(uint64_t ecount);
      bool dec_and_تسلسل();
      مجازی خالی تسلسل_تسلسل() = 0;
      مجازی ~evctl() {}
      evctl*اگلا_2;

    نجی:
      uint64_t ecount_;
};

class evctl_real : عوامی evctl {

    عوامی:
      explicit evctl_real(class __vpiHandle*handle, دوہرا قدر,
                          uint64_t ecount);
      مجازی ~evctl_real() override {}
      خالی تسلسل_تسلسل() override;

    نجی:
      class __vpiHandle*handle_;
      دوہرا قدر_2;
};

class evctl_سمتیہ : عوامی evctl {

    عوامی:
      explicit evctl_سمتیہ(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&قدر,
                            غیرعلامتی off, غیرعلامتی wid, uint64_t ecount);
      مجازی ~evctl_سمتیہ() override {}
      خالی تسلسل_تسلسل() override;

    نجی:
      vvp_net_ptr_t ptr_;
      vvp_vector4_t قدر_2;
      غیرعلامتی off_;
      غیرعلامتی wid_;
};

class evctl_array : عوامی evctl {

    عوامی:
      explicit evctl_array(vvp_array_t حافظہ, غیرعلامتی اشاریہ,
                           ثابت vvp_vector4_t&قدر, غیرعلامتی off,
                           uint64_t ecount);
      مجازی ~evctl_array() override {}
      مجازی خالی تسلسل_تسلسل() override;

    نجی:
      vvp_array_t mem_;
      غیرعلامتی idx_;
      vvp_vector4_t قدر_2;
      غیرعلامتی off_;
};

class evctl_array_r : عوامی evctl {

    عوامی:
      explicit evctl_array_r(vvp_array_t حافظہ, غیرعلامتی اشاریہ,
                           دوہرا قدر, uint64_t ecount);
      مجازی ~evctl_array_r() override {}
      مجازی خالی تسلسل_تسلسل() override;

    نجی:
      vvp_array_t mem_;
      غیرعلامتی idx_;
      دوہرا قدر_2;
};

خارجی خالی جدول_وقت_evctl(class __vpiHandle*handle, دوہرا قدر,
                           vvp_net_t*event, uint64_t ecount);

خارجی خالی جدول_وقت_evctl(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&قدر,
                           غیرعلامتی انحراف_2, غیرعلامتی wid,
                           vvp_net_t*event, uint64_t ecount);

خارجی خالی جدول_وقت_evctl(vvp_array_t حافظہ, غیرعلامتی اشاریہ,
                           ثابت vvp_vector4_t&قدر, غیرعلامتی انحراف_2,
                           vvp_net_t*event, uint64_t ecount);

خارجی خالی جدول_وقت_evctl(vvp_array_t حافظہ, غیرعلامتی اشاریہ,
                           دوہرا قدر,
                           vvp_net_t*event, uint64_t ecount);

/*
 *  Event / edge detection functors
 */

/*
 * A "waitable" functor is one that the %wait instruction can wait
 * on. This includes the infrastructure needed to hold threads.
 */
ساخت waitable_hooks_s {

    عوامی:
      waitable_hooks_s() : event_ctls(0) { آخری = &event_ctls; }
      مجازی ~waitable_hooks_s() {}

      مجازی vthread_t جوڑو_انتظار_thread(vthread_t thread) = 0;

      evctl*event_ctls;
      evctl**آخری;

    محفوظ:
      خالی تسلسل_انتظار_دھاگے(vthread_t&دھاگے);
};

/*
 * This is the base object for storing state information for each instance
 * of an automatically allocated event. In the general case, all that is
 * needed is the list of threads waiting on that instance.
 */
ساخت waitable_state_s {
      waitable_state_s() : دھاگے(0) {}

      vthread_t دھاگے;
};

/*
 * The vvp_fun_edge functor detects events that are edges of various
 * types. This should be hooked to a vvp_net_t that is connected to
 * the output of a signal that we wish to watch for edges.
 */
class vvp_fun_کنارہ : عوامی vvp_net_fun_t, عوامی waitable_hooks_s {

    عوامی:
      قسمتعریف غیرعلامتی مختصر کنارہ_t;
      explicit vvp_fun_کنارہ(کنارہ_t e);
      مجازی ~vvp_fun_کنارہ() override;

    محفوظ:
      bool recv_vec4_(ثابت vvp_vector4_t&ثنائی_ہندسہ,
                      vvp_bit4_t&old_ثنائی_ہندسہ, vthread_t&دھاگے);

      vvp_bit4_t بٹ_2[4];

    نجی:
      کنارہ_t کنارہ_2;
};

خارجی ثابت vvp_fun_کنارہ::کنارہ_t vvp_کنارہ_کنارہ;
خارجی ثابت vvp_fun_کنارہ::کنارہ_t vvp_کنارہ_posedge;
خارجی ثابت vvp_fun_کنارہ::کنارہ_t vvp_کنارہ_negedge;
خارجی ثابت vvp_fun_کنارہ::کنارہ_t vvp_کنارہ_none;

/*
 * Statically allocated vvp_fun_edge.
 */
class vvp_fun_کنارہ_sa : عوامی vvp_fun_کنارہ {

    عوامی:
      explicit vvp_fun_کنارہ_sa(کنارہ_t e);
      مجازی ~vvp_fun_کنارہ_sa() override;

      vthread_t جوڑو_انتظار_thread(vthread_t thread) override;

      خالی recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t context) override;
      خالی recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t context) override;

    نجی:
      vthread_t دھاگے_2;
};

/*
 * Automatically allocated vvp_fun_edge.
 */
class vvp_fun_کنارہ_aa : عوامی vvp_fun_کنارہ, عوامی automatic_hooks_s {

    عوامی:
      explicit vvp_fun_کنارہ_aa(کنارہ_t e);
      مجازی ~vvp_fun_کنارہ_aa() override;

      خالی alloc_instance(vvp_context_t context) override;
      خالی reset_instance(vvp_context_t context) override;
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
      خالی free_instance(vvp_context_t context) override;
#اگر_ختم

      vthread_t جوڑو_انتظار_thread(vthread_t thread) override;

      خالی recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t context) override;

    نجی:
      __vpiScope*context_scope_;
      غیرعلامتی context_idx_;
};

/*
 * This is the base object for storing the last received values for a
 * vvp_fun_anyedge functor.
 */
class anyedge_قدر;

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
class vvp_fun_anyedge : عوامی vvp_net_fun_t, عوامی waitable_hooks_s {

    عوامی:
      explicit vvp_fun_anyedge();
      مجازی ~vvp_fun_anyedge() override;

    محفوظ:
      anyedge_قدر*آخری_قدر[4];
};

/*
 * Statically allocated vvp_fun_anyedge.
 */
class vvp_fun_anyedge_sa : عوامی vvp_fun_anyedge {

    عوامی:
      explicit vvp_fun_anyedge_sa();
      مجازی ~vvp_fun_anyedge_sa() override;

      vthread_t جوڑو_انتظار_thread(vthread_t thread) override;

      خالی recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t context) override;
      خالی recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t context) override;

      خالی recv_real(vvp_net_ptr_t پورٹ, دوہرا ثنائی_ہندسہ,
                     vvp_context_t context) override;

      خالی recv_string(vvp_net_ptr_t پورٹ, ثابت std::string&ثنائی_ہندسہ,
		       vvp_context_t context) override;
      خالی recv_object(vvp_net_ptr_t پورٹ, vvp_object_t ثنائی_ہندسہ,
		       vvp_context_t context) override;

    نجی:
      vthread_t دھاگے_2;
};

/*
 * Automatically allocated vvp_fun_anyedge.
 */
class vvp_fun_anyedge_aa : عوامی vvp_fun_anyedge, عوامی automatic_hooks_s {

    عوامی:
      explicit vvp_fun_anyedge_aa();
      مجازی ~vvp_fun_anyedge_aa() override;

      خالی alloc_instance(vvp_context_t context) override;
      خالی reset_instance(vvp_context_t context) override;
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
      خالی free_instance(vvp_context_t context) override;
#اگر_ختم

      vthread_t جوڑو_انتظار_thread(vthread_t thread) override;

      خالی recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t context) override;

      خالی recv_real(vvp_net_ptr_t پورٹ, دوہرا ثنائی_ہندسہ,
                     vvp_context_t context) override;

      خالی recv_string(vvp_net_ptr_t پورٹ, ثابت std::string&ثنائی_ہندسہ,
		       vvp_context_t context) override;

    نجی:
      __vpiScope*context_scope_;
      غیرعلامتی context_idx_;
};

/*
 * This functor triggers anytime any input is set, no matter what the
 * value. This is similar to a named event, but it has no handle. It
 * supports wide (more than 4) inputs, so needs a pointer to the base
 * net that is used for output.
 */
class vvp_fun_event_or : عوامی vvp_net_fun_t, عوامی waitable_hooks_s {

    عوامی:
      explicit vvp_fun_event_or(vvp_net_t*بنیاد_net);
      ~vvp_fun_event_or() override;

    محفوظ:
      vvp_net_t*بنیاد_net_2;
};

/*
 * Statically allocated vvp_fun_event_or.
 */
class vvp_fun_event_or_sa : عوامی vvp_fun_event_or {

    عوامی:
      explicit vvp_fun_event_or_sa(vvp_net_t*بنیاد_net);
      ~vvp_fun_event_or_sa() override;

      vthread_t جوڑو_انتظار_thread(vthread_t thread) override;

      خالی recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t context) override;

    نجی:
      vthread_t دھاگے_2;
};

/*
 * Automatically allocated vvp_fun_event_or.
 */
class vvp_fun_event_or_aa : عوامی vvp_fun_event_or, عوامی automatic_hooks_s {

    عوامی:
      explicit vvp_fun_event_or_aa(vvp_net_t*بنیاد_net);
      ~vvp_fun_event_or_aa() override;

      خالی alloc_instance(vvp_context_t context) override;
      خالی reset_instance(vvp_context_t context) override;
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
      خالی free_instance(vvp_context_t context) override;
#اگر_ختم

      vthread_t جوڑو_انتظار_thread(vthread_t thread) override;

      خالی recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t context) override;

    نجی:
      __vpiScope*context_scope_;
      غیرعلامتی context_idx_;
};

/*
 * A named event is simpler than a vvp_fun_edge in that it triggers on
 * any input at all to port-0. The idea here is that behavioral code
 * can use a %set/v instruction to trigger the event.
 */
class vvp_named_event : عوامی vvp_net_fun_t, عوامی waitable_hooks_s {

    عوامی:
      explicit vvp_named_event(class __vpiHandle*eh);
      ~vvp_named_event() override;

    محفوظ:
      class __vpiHandle*handle_;
};

/*
 * Statically allocated vvp_named_event.
 */
class vvp_named_event_sa : عوامی vvp_named_event {

    عوامی:
      explicit vvp_named_event_sa(class __vpiHandle*eh);
      ~vvp_named_event_sa() override;

      vthread_t جوڑو_انتظار_thread(vthread_t thread) override;

      خالی recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override;

    نجی:
      vthread_t دھاگے_2;
};

/*
 * Automatically allocated vvp_named_event.
 */
class vvp_named_event_aa : عوامی vvp_named_event, عوامی automatic_hooks_s {

    عوامی:
      explicit vvp_named_event_aa(class __vpiHandle*eh);
      ~vvp_named_event_aa() override;

      خالی alloc_instance(vvp_context_t context) override;
      خالی reset_instance(vvp_context_t context) override;
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
      خالی free_instance(vvp_context_t context) override;
#اگر_ختم

      vthread_t جوڑو_انتظار_thread(vthread_t thread) override;

      خالی recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t context) override;

    نجی:
      غیرعلامتی context_idx_;
};

#اگر_ختم /* IVL_event_H */
