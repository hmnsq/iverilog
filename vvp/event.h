#నిర్వచితం_కాకపోతే IVL_event_H
#నిర్వచించు IVL_event_H
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

    బహిరంగ:
      explicit evctl(uint64_t ecount);
      bool dec_and_నిరంతరం();
      మాయా శూన్య నిరంతరం_నిరంతరం() = 0;
      మాయా ~evctl() {}
      evctl*తదుపరి_2;

    అంతరంగ:
      uint64_t ecount_;
};

class evctl_real : బహిరంగ evctl {

    బహిరంగ:
      explicit evctl_real(class __vpiHandle*handle, ద్విగుణ విలువ,
                          uint64_t ecount);
      మాయా ~evctl_real() override {}
      శూన్య నిరంతరం_నిరంతరం() override;

    అంతరంగ:
      class __vpiHandle*handle_;
      ద్విగుణ విలువ_2;
};

class evctl_సదిశ : బహిరంగ evctl {

    బహిరంగ:
      explicit evctl_సదిశ(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&విలువ,
                            గుర్తులేని off, గుర్తులేని wid, uint64_t ecount);
      మాయా ~evctl_సదిశ() override {}
      శూన్య నిరంతరం_నిరంతరం() override;

    అంతరంగ:
      vvp_net_ptr_t ptr_;
      vvp_vector4_t విలువ_2;
      గుర్తులేని off_;
      గుర్తులేని wid_;
};

class evctl_array : బహిరంగ evctl {

    బహిరంగ:
      explicit evctl_array(vvp_array_t జ్ఞాపకం, గుర్తులేని సూచీ,
                           స్థిరరాశి vvp_vector4_t&విలువ, గుర్తులేని off,
                           uint64_t ecount);
      మాయా ~evctl_array() override {}
      మాయా శూన్య నిరంతరం_నిరంతరం() override;

    అంతరంగ:
      vvp_array_t mem_;
      గుర్తులేని idx_;
      vvp_vector4_t విలువ_2;
      గుర్తులేని off_;
};

class evctl_array_r : బహిరంగ evctl {

    బహిరంగ:
      explicit evctl_array_r(vvp_array_t జ్ఞాపకం, గుర్తులేని సూచీ,
                           ద్విగుణ విలువ, uint64_t ecount);
      మాయా ~evctl_array_r() override {}
      మాయా శూన్య నిరంతరం_నిరంతరం() override;

    అంతరంగ:
      vvp_array_t mem_;
      గుర్తులేని idx_;
      ద్విగుణ విలువ_2;
};

బాహ్య శూన్య కాలపట్టిక_evctl(class __vpiHandle*handle, ద్విగుణ విలువ,
                           vvp_net_t*event, uint64_t ecount);

బాహ్య శూన్య కాలపట్టిక_evctl(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&విలువ,
                           గుర్తులేని స్థానభ్రంశం_2, గుర్తులేని wid,
                           vvp_net_t*event, uint64_t ecount);

బాహ్య శూన్య కాలపట్టిక_evctl(vvp_array_t జ్ఞాపకం, గుర్తులేని సూచీ,
                           స్థిరరాశి vvp_vector4_t&విలువ, గుర్తులేని స్థానభ్రంశం_2,
                           vvp_net_t*event, uint64_t ecount);

బాహ్య శూన్య కాలపట్టిక_evctl(vvp_array_t జ్ఞాపకం, గుర్తులేని సూచీ,
                           ద్విగుణ విలువ,
                           vvp_net_t*event, uint64_t ecount);

/*
 *  Event / edge detection functors
 */

/*
 * A "waitable" functor is one that the %wait instruction can wait
 * on. This includes the infrastructure needed to hold threads.
 */
నిర్మాణం waitable_hooks_s {

    బహిరంగ:
      waitable_hooks_s() : event_ctls(0) { చివరి = &event_ctls; }
      మాయా ~waitable_hooks_s() {}

      మాయా vthread_t కూడు_నిరీక్షణ_thread(vthread_t thread) = 0;

      evctl*event_ctls;
      evctl**చివరి;

    రక్షిత:
      శూన్య నిరంతరం_నిరీక్షణ_దారాలు(vthread_t&దారాలు);
};

/*
 * This is the base object for storing state information for each instance
 * of an automatically allocated event. In the general case, all that is
 * needed is the list of threads waiting on that instance.
 */
నిర్మాణం waitable_state_s {
      waitable_state_s() : దారాలు(0) {}

      vthread_t దారాలు;
};

/*
 * The vvp_fun_edge functor detects events that are edges of various
 * types. This should be hooked to a vvp_net_t that is connected to
 * the output of a signal that we wish to watch for edges.
 */
class vvp_fun_అంచు : బహిరంగ vvp_net_fun_t, బహిరంగ waitable_hooks_s {

    బహిరంగ:
      రకనిర్వచనం గుర్తులేని పొట్టి అంచు_t;
      explicit vvp_fun_అంచు(అంచు_t e);
      మాయా ~vvp_fun_అంచు() override;

    రక్షిత:
      bool recv_vec4_(స్థిరరాశి vvp_vector4_t&బిట్టు,
                      vvp_bit4_t&old_బిట్టు, vthread_t&దారాలు);

      vvp_bit4_t బిట్లు_2[4];

    అంతరంగ:
      అంచు_t అంచు_2;
};

బాహ్య స్థిరరాశి vvp_fun_అంచు::అంచు_t vvp_అంచు_అంచు;
బాహ్య స్థిరరాశి vvp_fun_అంచు::అంచు_t vvp_అంచు_posedge;
బాహ్య స్థిరరాశి vvp_fun_అంచు::అంచు_t vvp_అంచు_negedge;
బాహ్య స్థిరరాశి vvp_fun_అంచు::అంచు_t vvp_అంచు_none;

/*
 * Statically allocated vvp_fun_edge.
 */
class vvp_fun_అంచు_sa : బహిరంగ vvp_fun_అంచు {

    బహిరంగ:
      explicit vvp_fun_అంచు_sa(అంచు_t e);
      మాయా ~vvp_fun_అంచు_sa() override;

      vthread_t కూడు_నిరీక్షణ_thread(vthread_t thread) override;

      శూన్య recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t context) override;
      శూన్య recv_vec4_pv(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
			గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t context) override;

    అంతరంగ:
      vthread_t దారాలు_2;
};

/*
 * Automatically allocated vvp_fun_edge.
 */
class vvp_fun_అంచు_aa : బహిరంగ vvp_fun_అంచు, బహిరంగ automatic_hooks_s {

    బహిరంగ:
      explicit vvp_fun_అంచు_aa(అంచు_t e);
      మాయా ~vvp_fun_అంచు_aa() override;

      శూన్య alloc_instance(vvp_context_t context) override;
      శూన్య reset_instance(vvp_context_t context) override;
#నిర్వచితమైతే CHECK_WITH_VALGRIND
      శూన్య free_instance(vvp_context_t context) override;
#అయితే_ముగింపు

      vthread_t కూడు_నిరీక్షణ_thread(vthread_t thread) override;

      శూన్య recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t context) override;

    అంతరంగ:
      __vpiScope*context_scope_;
      గుర్తులేని context_idx_;
};

/*
 * This is the base object for storing the last received values for a
 * vvp_fun_anyedge functor.
 */
class anyedge_విలువ;

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
class vvp_fun_anyedge : బహిరంగ vvp_net_fun_t, బహిరంగ waitable_hooks_s {

    బహిరంగ:
      explicit vvp_fun_anyedge();
      మాయా ~vvp_fun_anyedge() override;

    రక్షిత:
      anyedge_విలువ*చివరి_విలువ[4];
};

/*
 * Statically allocated vvp_fun_anyedge.
 */
class vvp_fun_anyedge_sa : బహిరంగ vvp_fun_anyedge {

    బహిరంగ:
      explicit vvp_fun_anyedge_sa();
      మాయా ~vvp_fun_anyedge_sa() override;

      vthread_t కూడు_నిరీక్షణ_thread(vthread_t thread) override;

      శూన్య recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t context) override;
      శూన్య recv_vec4_pv(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
			గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t context) override;

      శూన్య recv_real(vvp_net_ptr_t పోర్టు, ద్విగుణ బిట్టు,
                     vvp_context_t context) override;

      శూన్య recv_string(vvp_net_ptr_t పోర్టు, స్థిరరాశి std::string&బిట్టు,
		       vvp_context_t context) override;
      శూన్య recv_object(vvp_net_ptr_t పోర్టు, vvp_object_t బిట్టు,
		       vvp_context_t context) override;

    అంతరంగ:
      vthread_t దారాలు_2;
};

/*
 * Automatically allocated vvp_fun_anyedge.
 */
class vvp_fun_anyedge_aa : బహిరంగ vvp_fun_anyedge, బహిరంగ automatic_hooks_s {

    బహిరంగ:
      explicit vvp_fun_anyedge_aa();
      మాయా ~vvp_fun_anyedge_aa() override;

      శూన్య alloc_instance(vvp_context_t context) override;
      శూన్య reset_instance(vvp_context_t context) override;
#నిర్వచితమైతే CHECK_WITH_VALGRIND
      శూన్య free_instance(vvp_context_t context) override;
#అయితే_ముగింపు

      vthread_t కూడు_నిరీక్షణ_thread(vthread_t thread) override;

      శూన్య recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t context) override;

      శూన్య recv_real(vvp_net_ptr_t పోర్టు, ద్విగుణ బిట్టు,
                     vvp_context_t context) override;

      శూన్య recv_string(vvp_net_ptr_t పోర్టు, స్థిరరాశి std::string&బిట్టు,
		       vvp_context_t context) override;

    అంతరంగ:
      __vpiScope*context_scope_;
      గుర్తులేని context_idx_;
};

/*
 * This functor triggers anytime any input is set, no matter what the
 * value. This is similar to a named event, but it has no handle. It
 * supports wide (more than 4) inputs, so needs a pointer to the base
 * net that is used for output.
 */
class vvp_fun_event_or : బహిరంగ vvp_net_fun_t, బహిరంగ waitable_hooks_s {

    బహిరంగ:
      explicit vvp_fun_event_or(vvp_net_t*పునాది_net);
      ~vvp_fun_event_or() override;

    రక్షిత:
      vvp_net_t*పునాది_net_2;
};

/*
 * Statically allocated vvp_fun_event_or.
 */
class vvp_fun_event_or_sa : బహిరంగ vvp_fun_event_or {

    బహిరంగ:
      explicit vvp_fun_event_or_sa(vvp_net_t*పునాది_net);
      ~vvp_fun_event_or_sa() override;

      vthread_t కూడు_నిరీక్షణ_thread(vthread_t thread) override;

      శూన్య recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t context) override;

    అంతరంగ:
      vthread_t దారాలు_2;
};

/*
 * Automatically allocated vvp_fun_event_or.
 */
class vvp_fun_event_or_aa : బహిరంగ vvp_fun_event_or, బహిరంగ automatic_hooks_s {

    బహిరంగ:
      explicit vvp_fun_event_or_aa(vvp_net_t*పునాది_net);
      ~vvp_fun_event_or_aa() override;

      శూన్య alloc_instance(vvp_context_t context) override;
      శూన్య reset_instance(vvp_context_t context) override;
#నిర్వచితమైతే CHECK_WITH_VALGRIND
      శూన్య free_instance(vvp_context_t context) override;
#అయితే_ముగింపు

      vthread_t కూడు_నిరీక్షణ_thread(vthread_t thread) override;

      శూన్య recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t context) override;

    అంతరంగ:
      __vpiScope*context_scope_;
      గుర్తులేని context_idx_;
};

/*
 * A named event is simpler than a vvp_fun_edge in that it triggers on
 * any input at all to port-0. The idea here is that behavioral code
 * can use a %set/v instruction to trigger the event.
 */
class vvp_named_event : బహిరంగ vvp_net_fun_t, బహిరంగ waitable_hooks_s {

    బహిరంగ:
      explicit vvp_named_event(class __vpiHandle*eh);
      ~vvp_named_event() override;

    రక్షిత:
      class __vpiHandle*handle_;
};

/*
 * Statically allocated vvp_named_event.
 */
class vvp_named_event_sa : బహిరంగ vvp_named_event {

    బహిరంగ:
      explicit vvp_named_event_sa(class __vpiHandle*eh);
      ~vvp_named_event_sa() override;

      vthread_t కూడు_నిరీక్షణ_thread(vthread_t thread) override;

      శూన్య recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t) override;

    అంతరంగ:
      vthread_t దారాలు_2;
};

/*
 * Automatically allocated vvp_named_event.
 */
class vvp_named_event_aa : బహిరంగ vvp_named_event, బహిరంగ automatic_hooks_s {

    బహిరంగ:
      explicit vvp_named_event_aa(class __vpiHandle*eh);
      ~vvp_named_event_aa() override;

      శూన్య alloc_instance(vvp_context_t context) override;
      శూన్య reset_instance(vvp_context_t context) override;
#నిర్వచితమైతే CHECK_WITH_VALGRIND
      శూన్య free_instance(vvp_context_t context) override;
#అయితే_ముగింపు

      vthread_t కూడు_నిరీక్షణ_thread(vthread_t thread) override;

      శూన్య recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t context) override;

    అంతరంగ:
      గుర్తులేని context_idx_;
};

#అయితే_ముగింపు /* IVL_event_H */
