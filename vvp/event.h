#جے_بنا_تعریف IVL_event_H
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

    عام:
      explicit evctl(uint64_t ecount);
      bool dec_and_لگاتار();
      مجازی فراغ لگاتار_لگاتار() = 0;
      مجازی ~evctl() {}
      evctl*اگلا_2;

    خاص:
      uint64_t ecount_;
};

class evctl_real : عام evctl {

    عام:
      explicit evctl_real(class __vpiHandle*handle, مزدوج قدر,
                          uint64_t ecount);
      مجازی ~evctl_real() override {}
      فراغ لگاتار_لگاتار() override;

    خاص:
      class __vpiHandle*handle_;
      مزدوج قدر_2;
};

class evctl_سمتیہ : عام evctl {

    عام:
      explicit evctl_سمتیہ(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&قدر,
                            بنانشانی off, بنانشانی wid, uint64_t ecount);
      مجازی ~evctl_سمتیہ() override {}
      فراغ لگاتار_لگاتار() override;

    خاص:
      vvp_net_ptr_t ptr_;
      vvp_vector4_t قدر_2;
      بنانشانی off_;
      بنانشانی wid_;
};

class evctl_array : عام evctl {

    عام:
      explicit evctl_array(vvp_array_t حافظہ, بنانشانی اشاریہ,
                           ثابت vvp_vector4_t&قدر, بنانشانی off,
                           uint64_t ecount);
      مجازی ~evctl_array() override {}
      مجازی فراغ لگاتار_لگاتار() override;

    خاص:
      vvp_array_t mem_;
      بنانشانی idx_;
      vvp_vector4_t قدر_2;
      بنانشانی off_;
};

class evctl_array_r : عام evctl {

    عام:
      explicit evctl_array_r(vvp_array_t حافظہ, بنانشانی اشاریہ,
                           مزدوج قدر, uint64_t ecount);
      مجازی ~evctl_array_r() override {}
      مجازی فراغ لگاتار_لگاتار() override;

    خاص:
      vvp_array_t mem_;
      بنانشانی idx_;
      مزدوج قدر_2;
};

خارجي فراغ ویلا_سارنی_evctl(class __vpiHandle*handle, مزدوج قدر,
                           vvp_net_t*event, uint64_t ecount);

خارجي فراغ ویلا_سارنی_evctl(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&قدر,
                           بنانشانی وتھ_2, بنانشانی wid,
                           vvp_net_t*event, uint64_t ecount);

خارجي فراغ ویلا_سارنی_evctl(vvp_array_t حافظہ, بنانشانی اشاریہ,
                           ثابت vvp_vector4_t&قدر, بنانشانی وتھ_2,
                           vvp_net_t*event, uint64_t ecount);

خارجي فراغ ویلا_سارنی_evctl(vvp_array_t حافظہ, بنانشانی اشاریہ,
                           مزدوج قدر,
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
      waitable_hooks_s() : event_ctls(0) { آخری = &event_ctls; }
      مجازی ~waitable_hooks_s() {}

      مجازی vthread_t جوڑو_اڈیک_thread(vthread_t thread) = 0;

      evctl*event_ctls;
      evctl**آخری;

    محمي:
      فراغ لگاتار_اڈیک_دھاگے(vthread_t&دھاگے);
};

/*
 * This is the base object for storing state information for each instance
 * of an automatically allocated event. In the general case, all that is
 * needed is the list of threads waiting on that instance.
 */
بنية waitable_state_s {
      waitable_state_s() : دھاگے(0) {}

      vthread_t دھاگے;
};

/*
 * The vvp_fun_edge functor detects events that are edges of various
 * types. This should be hooked to a vvp_net_t that is connected to
 * the output of a signal that we wish to watch for edges.
 */
class vvp_fun_کنارا : عام vvp_net_fun_t, عام waitable_hooks_s {

    عام:
      تعريفنوع بنانشانی چھوٹا کنارا_t;
      explicit vvp_fun_کنارا(کنارا_t e);
      مجازی ~vvp_fun_کنارا() override;

    محمي:
      bool recv_vec4_(ثابت vvp_vector4_t&ثنائی_ہندسہ,
                      vvp_bit4_t&old_ثنائی_ہندسہ, vthread_t&دھاگے);

      vvp_bit4_t بٹ_2[4];

    خاص:
      کنارا_t کنارا_2;
};

خارجي ثابت vvp_fun_کنارا::کنارا_t vvp_کنارا_کنارا;
خارجي ثابت vvp_fun_کنارا::کنارا_t vvp_کنارا_posedge;
خارجي ثابت vvp_fun_کنارا::کنارا_t vvp_کنارا_negedge;
خارجي ثابت vvp_fun_کنارا::کنارا_t vvp_کنارا_none;

/*
 * Statically allocated vvp_fun_edge.
 */
class vvp_fun_کنارا_sa : عام vvp_fun_کنارا {

    عام:
      explicit vvp_fun_کنارا_sa(کنارا_t e);
      مجازی ~vvp_fun_کنارا_sa() override;

      vthread_t جوڑو_اڈیک_thread(vthread_t thread) override;

      فراغ recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t context) override;
      فراغ recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			بنانشانی ادھار, بنانشانی vwid, vvp_context_t context) override;

    خاص:
      vthread_t دھاگے_2;
};

/*
 * Automatically allocated vvp_fun_edge.
 */
class vvp_fun_کنارا_aa : عام vvp_fun_کنارا, عام automatic_hooks_s {

    عام:
      explicit vvp_fun_کنارا_aa(کنارا_t e);
      مجازی ~vvp_fun_کنارا_aa() override;

      فراغ alloc_instance(vvp_context_t context) override;
      فراغ reset_instance(vvp_context_t context) override;
#جے_تعریف_شدہ CHECK_WITH_VALGRIND
      فراغ free_instance(vvp_context_t context) override;
#جے_انت

      vthread_t جوڑو_اڈیک_thread(vthread_t thread) override;

      فراغ recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t context) override;

    خاص:
      __vpiScope*context_scope_;
      بنانشانی context_idx_;
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
class vvp_fun_anyedge : عام vvp_net_fun_t, عام waitable_hooks_s {

    عام:
      explicit vvp_fun_anyedge();
      مجازی ~vvp_fun_anyedge() override;

    محمي:
      anyedge_قدر*آخری_قدر[4];
};

/*
 * Statically allocated vvp_fun_anyedge.
 */
class vvp_fun_anyedge_sa : عام vvp_fun_anyedge {

    عام:
      explicit vvp_fun_anyedge_sa();
      مجازی ~vvp_fun_anyedge_sa() override;

      vthread_t جوڑو_اڈیک_thread(vthread_t thread) override;

      فراغ recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t context) override;
      فراغ recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			بنانشانی ادھار, بنانشانی vwid, vvp_context_t context) override;

      فراغ recv_real(vvp_net_ptr_t پورٹ, مزدوج ثنائی_ہندسہ,
                     vvp_context_t context) override;

      فراغ recv_string(vvp_net_ptr_t پورٹ, ثابت std::string&ثنائی_ہندسہ,
		       vvp_context_t context) override;
      فراغ recv_object(vvp_net_ptr_t پورٹ, vvp_object_t ثنائی_ہندسہ,
		       vvp_context_t context) override;

    خاص:
      vthread_t دھاگے_2;
};

/*
 * Automatically allocated vvp_fun_anyedge.
 */
class vvp_fun_anyedge_aa : عام vvp_fun_anyedge, عام automatic_hooks_s {

    عام:
      explicit vvp_fun_anyedge_aa();
      مجازی ~vvp_fun_anyedge_aa() override;

      فراغ alloc_instance(vvp_context_t context) override;
      فراغ reset_instance(vvp_context_t context) override;
#جے_تعریف_شدہ CHECK_WITH_VALGRIND
      فراغ free_instance(vvp_context_t context) override;
#جے_انت

      vthread_t جوڑو_اڈیک_thread(vthread_t thread) override;

      فراغ recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t context) override;

      فراغ recv_real(vvp_net_ptr_t پورٹ, مزدوج ثنائی_ہندسہ,
                     vvp_context_t context) override;

      فراغ recv_string(vvp_net_ptr_t پورٹ, ثابت std::string&ثنائی_ہندسہ,
		       vvp_context_t context) override;

    خاص:
      __vpiScope*context_scope_;
      بنانشانی context_idx_;
};

/*
 * This functor triggers anytime any input is set, no matter what the
 * value. This is similar to a named event, but it has no handle. It
 * supports wide (more than 4) inputs, so needs a pointer to the base
 * net that is used for output.
 */
class vvp_fun_event_or : عام vvp_net_fun_t, عام waitable_hooks_s {

    عام:
      explicit vvp_fun_event_or(vvp_net_t*ادھار_net);
      ~vvp_fun_event_or() override;

    محمي:
      vvp_net_t*ادھار_net_2;
};

/*
 * Statically allocated vvp_fun_event_or.
 */
class vvp_fun_event_or_sa : عام vvp_fun_event_or {

    عام:
      explicit vvp_fun_event_or_sa(vvp_net_t*ادھار_net);
      ~vvp_fun_event_or_sa() override;

      vthread_t جوڑو_اڈیک_thread(vthread_t thread) override;

      فراغ recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t context) override;

    خاص:
      vthread_t دھاگے_2;
};

/*
 * Automatically allocated vvp_fun_event_or.
 */
class vvp_fun_event_or_aa : عام vvp_fun_event_or, عام automatic_hooks_s {

    عام:
      explicit vvp_fun_event_or_aa(vvp_net_t*ادھار_net);
      ~vvp_fun_event_or_aa() override;

      فراغ alloc_instance(vvp_context_t context) override;
      فراغ reset_instance(vvp_context_t context) override;
#جے_تعریف_شدہ CHECK_WITH_VALGRIND
      فراغ free_instance(vvp_context_t context) override;
#جے_انت

      vthread_t جوڑو_اڈیک_thread(vthread_t thread) override;

      فراغ recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t context) override;

    خاص:
      __vpiScope*context_scope_;
      بنانشانی context_idx_;
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

      vthread_t جوڑو_اڈیک_thread(vthread_t thread) override;

      فراغ recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override;

    خاص:
      vthread_t دھاگے_2;
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
#جے_تعریف_شدہ CHECK_WITH_VALGRIND
      فراغ free_instance(vvp_context_t context) override;
#جے_انت

      vthread_t جوڑو_اڈیک_thread(vthread_t thread) override;

      فراغ recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t context) override;

    خاص:
      بنانشانی context_idx_;
};

#جے_انت /* IVL_event_H */
