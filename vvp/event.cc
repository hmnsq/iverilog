/*
 * Copyright (c) 2004-2026 Stephen Williams (steve@icarus.com)
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

# include  "event.h"
# include  "compile.h"
# include  "vthread.h"
# include  "schedule.h"
# include  "vpi_priv.h"
# include  "config.h"
# include  <cstring>
# include  <cassert>
# include  <cstdlib>

# include <iostream>

శూన్య waitable_hooks_s::నిరంతరం_నిరీక్షణ_దారాలు(vthread_t&దారాలు)
{
	// Run the non-blocking event controls.
      చివరి = &event_ctls;
      కొరకు (evctl*cur = event_ctls; cur != 0;) {
	    ఒకవేళ (cur->dec_and_నిరంతరం()) {
		  evctl*nxt = cur->తదుపరి_2;
		  తొలగించు cur;
		  cur = nxt;
		  *చివరి = cur;
	    } లేకపోతే {
		  చివరి = &(cur->తదుపరి_2);
		  cur = cur->తదుపరి_2;
	    }
      }

      vthread_t tmp = దారాలు;
      ఒకవేళ (tmp == 0) తిరిగి;
      దారాలు = 0;

      vthread_కాలపట్టిక_list(tmp);
}

evctl::evctl(uint64_t ecount)
{
      ecount_ = ecount;
      తదుపరి_2 = 0;
}

తార్కిక evctl::dec_and_నిరంతరం()
{
      ప్రతిజ్ఞా(ecount_ != 0);

      ecount_ -= 1;
      ఒకవేళ (ecount_ == 0) నిరంతరం_నిరంతరం();

      తిరిగి ecount_ == 0;
}

evctl_real::evctl_real(__vpiHandle*handle, ద్విగుణ value,
                       uint64_t ecount)
:evctl(ecount)
{
      handle_ = handle;
      విలువ_2 = value;
}

శూన్య evctl_real::నిరంతరం_నిరంతరం()
{
      t_vpi_విలువ val;

      val.format = vpiRealVal;
      val.value.real = విలువ_2;
      vpi_put_విలువ(handle_, &val, 0, vpiNoDelay);
}

శూన్య కాలపట్టిక_evctl(__vpiHandle*handle, ద్విగుణ value,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      ప్రతిజ్ఞా(ep);
	// Now add this call to the end of the event list.
      *(ep->చివరి) = కొత్త evctl_real(handle, value, ecount);
      ep->చివరి = &((*(ep->చివరి))->తదుపరి_2);
}

evctl_సదిశ::evctl_సదిశ(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&value,
                           గుర్తులేని off, గుర్తులేని wid, uint64_t ecount)
:evctl(ecount), ptr_(ptr), విలువ_2(value)
{
      off_ = off;
      wid_ = wid;
}

శూన్య evctl_సదిశ::నిరంతరం_నిరంతరం()
{
      ఒకవేళ (wid_ != 0) {
	    vvp_send_vec4_pv(ptr_, విలువ_2, off_, wid_, 0);
      } లేకపోతే {
	    vvp_send_vec4(ptr_, విలువ_2, 0);
      }
}

శూన్య కాలపట్టిక_evctl(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&value,
                    గుర్తులేని స్థానభ్రంశం_2, గుర్తులేని wid,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      ప్రతిజ్ఞా(ep);
	// Now add this call to the end of the event list.
      *(ep->చివరి) = కొత్త evctl_సదిశ(ptr, value, స్థానభ్రంశం_2, wid, ecount);
      ep->చివరి = &((*(ep->చివరి))->తదుపరి_2);
}

evctl_array::evctl_array(vvp_array_t జ్ఞాపకం, గుర్తులేని సూచీ,
                         స్థిరరాశి vvp_vector4_t&value, గుర్తులేని off,
                         uint64_t ecount)
:evctl(ecount), విలువ_2(value)
{
      mem_ = జ్ఞాపకం;
      idx_ = సూచీ;
      off_ = off;
}

శూన్య evctl_array::నిరంతరం_నిరంతరం()
{
      mem_->సమితి_మాట(idx_, off_, విలువ_2);
}

శూన్య కాలపట్టిక_evctl(vvp_array_t జ్ఞాపకం, గుర్తులేని సూచీ,
                    స్థిరరాశి vvp_vector4_t&value, గుర్తులేని స్థానభ్రంశం_2,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      ప్రతిజ్ఞా(ep);
	// Now add this call to the end of the event list.
      *(ep->చివరి) = కొత్త evctl_array(జ్ఞాపకం, సూచీ, value, స్థానభ్రంశం_2, ecount);
      ep->చివరి = &((*(ep->చివరి))->తదుపరి_2);
}

evctl_array_r::evctl_array_r(vvp_array_t జ్ఞాపకం, గుర్తులేని సూచీ,
                             ద్విగుణ value, uint64_t ecount)
:evctl(ecount)
{
      mem_ = జ్ఞాపకం;
      idx_ = సూచీ;
      విలువ_2 = value;
}

శూన్య evctl_array_r::నిరంతరం_నిరంతరం()
{
      mem_->సమితి_మాట(idx_, విలువ_2);
}

శూన్య కాలపట్టిక_evctl(vvp_array_t జ్ఞాపకం, గుర్తులేని సూచీ,
                    ద్విగుణ value,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      ప్రతిజ్ఞా(ep);
	// Now add this call to the end of the event list.
      *(ep->చివరి) = కొత్త evctl_array_r(జ్ఞాపకం, సూచీ, value, ecount);
      ep->చివరి = &((*(ep->చివరి))->తదుపరి_2);
}

అంతఃవరుస vvp_fun_అంచు::అంచు_t VVP_అంచు(vvp_bit4_t from, vvp_bit4_t to)
{
      తిరిగి 1 << ((from << 2) | to);
}

స్థిరరాశి vvp_fun_అంచు::అంచు_t vvp_అంచు_posedge
      = VVP_అంచు(BIT4_0,BIT4_1)
      | VVP_అంచు(BIT4_0,BIT4_X)
      | VVP_అంచు(BIT4_0,BIT4_Z)
      | VVP_అంచు(BIT4_X,BIT4_1)
      | VVP_అంచు(BIT4_Z,BIT4_1)
      ;

స్థిరరాశి vvp_fun_అంచు::అంచు_t vvp_అంచు_negedge
      = VVP_అంచు(BIT4_1,BIT4_0)
      | VVP_అంచు(BIT4_1,BIT4_X)
      | VVP_అంచు(BIT4_1,BIT4_Z)
      | VVP_అంచు(BIT4_X,BIT4_0)
      | VVP_అంచు(BIT4_Z,BIT4_0)
      ;

స్థిరరాశి vvp_fun_అంచు::అంచు_t vvp_అంచు_అంచు
      = VVP_అంచు(BIT4_0,BIT4_1)
      | VVP_అంచు(BIT4_1,BIT4_0)
      | VVP_అంచు(BIT4_0,BIT4_X)
      | VVP_అంచు(BIT4_X,BIT4_0)
      | VVP_అంచు(BIT4_0,BIT4_Z)
      | VVP_అంచు(BIT4_Z,BIT4_0)
      | VVP_అంచు(BIT4_X,BIT4_1)
      | VVP_అంచు(BIT4_1,BIT4_X)
      | VVP_అంచు(BIT4_Z,BIT4_1)
      | VVP_అంచు(BIT4_1,BIT4_Z)
      ;

స్థిరరాశి vvp_fun_అంచు::అంచు_t vvp_అంచు_none    = 0;

నిర్మాణం vvp_fun_అంచు_state_s : బహిరంగ waitable_state_s {
      vvp_fun_అంచు_state_s()
      {
            కొరకు (గుర్తులేని idx = 0 ;  idx < 4 ;  idx += 1)
                  బిట్లు[idx] = BIT4_X;
      }

      vvp_bit4_t బిట్లు[4];
};

vvp_fun_అంచు::vvp_fun_అంచు(అంచు_t e)
: అంచు_2(e)
{
      కొరకు (గుర్తులేని idx = 0 ;  idx < 4 ;  idx += 1)
            బిట్లు_2[idx] = BIT4_X;
}

vvp_fun_అంచు::~vvp_fun_అంచు()
{
}

తార్కిక vvp_fun_అంచు::recv_vec4_(స్థిరరాశి vvp_vector4_t&బిట్టు,
                              vvp_bit4_t&old_బిట్టు, vthread_t&దారాలు)
{
	/* See what kind of edge this represents. */
      అంచు_t ముసుగు = VVP_అంచు(old_బిట్టు, బిట్టు.value(0));

	/* Save the current input for the next time around. */
      old_బిట్టు = బిట్టు.value(0);

      ఒకవేళ ((అంచు_2 == vvp_అంచు_none) || (అంచు_2 & ముసుగు)) {
	    నిరంతరం_నిరీక్షణ_దారాలు(దారాలు);
            తిరిగి నిజం;
      }
      తిరిగి అబద్ధం;
}

vvp_fun_అంచు_sa::vvp_fun_అంచు_sa(అంచు_t e)
: vvp_fun_అంచు(e), దారాలు_2(0)
{
}

vvp_fun_అంచు_sa::~vvp_fun_అంచు_sa()
{
}

vthread_t vvp_fun_అంచు_sa::కూడు_నిరీక్షణ_thread(vthread_t thread)
{
      vthread_t tmp = దారాలు_2;
      దారాలు_2 = thread;

      తిరిగి tmp;
}

శూన్య vvp_fun_అంచు_sa::recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                                vvp_context_t)
{
      ఒకవేళ (recv_vec4_(బిట్టు, బిట్లు_2[పోర్టు.పోర్టు()], దారాలు_2)) {
	    vvp_net_t*net = పోర్టు.ptr();
	    net->send_vec4(బిట్టు, 0);
      }
}

శూన్య vvp_fun_అంచు_sa::recv_vec4_pv(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
				   గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t)
{
      ప్రతిజ్ఞా(పునాది == 0);
      ఒకవేళ (recv_vec4_(బిట్టు, బిట్లు_2[పోర్టు.పోర్టు()], దారాలు_2)) {
	    vvp_net_t*net = పోర్టు.ptr();
	    net->send_vec4_pv(బిట్టు, పునాది, vwid, 0);
      }
}

vvp_fun_అంచు_aa::vvp_fun_అంచు_aa(అంచు_t e)
: vvp_fun_అంచు(e)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_కూడు_అంశం_గమ్యం_context(ఇది, context_scope_);
}

vvp_fun_అంచు_aa::~vvp_fun_అంచు_aa()
{
}

శూన్య vvp_fun_అంచు_aa::alloc_instance(vvp_context_t context)
{
      vvp_సమితి_context_అంశం(context, context_idx_, కొత్త vvp_fun_అంచు_state_s);
      reset_instance(context);
}

శూన్య vvp_fun_అంచు_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_అంచు_state_s*state = static_cast<vvp_fun_అంచు_state_s*>
            (vvp_get_context_అంశం(context, context_idx_));

      state->దారాలు = 0;
      కొరకు (గుర్తులేని idx = 0 ;  idx < 4 ;  idx += 1)
            state->బిట్లు[idx] = బిట్లు_2[idx];
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
శూన్య vvp_fun_అంచు_aa::free_instance(vvp_context_t context)
{
      vvp_fun_అంచు_state_s*state = static_cast<vvp_fun_అంచు_state_s*>
            (vvp_get_context_అంశం(context, context_idx_));
      తొలగించు state;
}
#అయితే_ముగింపు

vthread_t vvp_fun_అంచు_aa::కూడు_నిరీక్షణ_thread(vthread_t thread)
{
      vvp_fun_అంచు_state_s*state = static_cast<vvp_fun_అంచు_state_s*>
            (vthread_get_wt_context_అంశం(context_idx_));

      vthread_t tmp = state->దారాలు;
      state->దారాలు = thread;

      తిరిగి tmp;
}

శూన్య vvp_fun_అంచు_aa::recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                                vvp_context_t context)
{
      ఒకవేళ (context) {
            vvp_fun_అంచు_state_s*state = static_cast<vvp_fun_అంచు_state_s*>
                  (vvp_get_context_అంశం(context, context_idx_));

            ఒకవేళ (recv_vec4_(బిట్టు, state->బిట్లు[పోర్టు.పోర్టు()], state->దారాలు)) {
                  vvp_net_t*net = పోర్టు.ptr();
                  net->send_vec4(బిట్టు, context);
            }
      } లేకపోతే {
            context = context_scope_->సజీవ_contexts;
            అయితే (context) {
                  recv_vec4(పోర్టు, బిట్టు, context);
                  context = vvp_get_తదుపరి_context(context);
            }
            బిట్లు_2[పోర్టు.పోర్టు()] = బిట్టు.value(0);
      }
}

తరగతి anyedge_విలువ {

    బహిరంగ:
      anyedge_విలువ() {};
      మాయా ~anyedge_విలువ() {};

      మాయా శూన్య reset() = 0;

      మాయా శూన్య duplicate(anyedge_విలువ*&dup) = 0;
};

తరగతి anyedge_vec4_విలువ : బహిరంగ anyedge_విలువ {

    బహిరంగ:
      anyedge_vec4_విలువ() {};
      మాయా ~anyedge_vec4_విలువ() override {};

      శూన్య reset() override { old_బిట్లు.సమితి_గమ్యం_x(); }

      శూన్య సమితి(స్థిరరాశి vvp_vector4_t&బిట్టు) { old_బిట్లు = బిట్టు; };

      శూన్య duplicate(anyedge_విలువ*&dup) override;

      తార్కిక recv_vec4(స్థిరరాశి vvp_vector4_t&బిట్టు);

      తార్కిక recv_vec4_pv(స్థిరరాశి vvp_vector4_t&బిట్టు, గుర్తులేని పునాది,
			గుర్తులేని vwid);

    అంతరంగ:
      vvp_vector4_t old_బిట్లు;
};

స్థిర anyedge_vec4_విలువ*get_vec4_విలువ(anyedge_విలువ*&value)
{
      anyedge_vec4_విలువ*vec4_విలువ = dynamic_cast<anyedge_vec4_విలువ*>(value);
      ఒకవేళ (!value) {
	    vec4_విలువ = కొత్త anyedge_vec4_విలువ();
	    తొలగించు value;
	    value = vec4_విలువ;
      }
      తిరిగి vec4_విలువ;
}

తరగతి anyedge_real_విలువ : బహిరంగ anyedge_విలువ {

    బహిరంగ:
      anyedge_real_విలువ() : old_బిట్లు(0.0) {};
      మాయా ~anyedge_real_విలువ() override {};

      శూన్య reset() override { old_బిట్లు = 0.0; }

      శూన్య సమితి(ద్విగుణ బిట్టు) { old_బిట్లు = బిట్టు; };

      శూన్య duplicate(anyedge_విలువ*&dup) override;

      తార్కిక recv_real(ద్విగుణ బిట్టు);

    అంతరంగ:
      ద్విగుణ old_బిట్లు;
};

స్థిర anyedge_real_విలువ*get_real_విలువ(anyedge_విలువ*&value)
{
      anyedge_real_విలువ*real_విలువ = dynamic_cast<anyedge_real_విలువ*>(value);
      ఒకవేళ (!value) {
	    real_విలువ = కొత్త anyedge_real_విలువ();
	    తొలగించు value;
	    value = real_విలువ;
      }
      తిరిగి real_విలువ;
}

తరగతి anyedge_string_విలువ : బహిరంగ anyedge_విలువ {

    బహిరంగ:
      anyedge_string_విలువ() {};
      మాయా ~anyedge_string_విలువ() override {};

      శూన్య reset() override { old_బిట్లు.clear(); }

      శూన్య సమితి(స్థిరరాశి std::string&బిట్టు) { old_బిట్లు = బిట్టు; };

      శూన్య duplicate(anyedge_విలువ*&dup) override;

      తార్కిక recv_string(స్థిరరాశి std::string&బిట్టు);

    అంతరంగ:
      std::string old_బిట్లు;
};

స్థిర anyedge_string_విలువ*get_string_విలువ(anyedge_విలువ*&value)
{
      anyedge_string_విలువ*string_విలువ = dynamic_cast<anyedge_string_విలువ*>(value);
      ఒకవేళ (!value) {
	    string_విలువ = కొత్త anyedge_string_విలువ();
	    తొలగించు value;
	    value = string_విలువ;
      }
      తిరిగి string_విలువ;
}

నిర్మాణం vvp_fun_anyedge_state_s : బహిరంగ waitable_state_s {
      vvp_fun_anyedge_state_s()
      {
            కొరకు (గుర్తులేని idx = 0 ;  idx < 4 ;  idx += 1)
                  చివరి_విలువ[idx] = 0;
      }

      ~vvp_fun_anyedge_state_s()
      {
            కొరకు (గుర్తులేని idx = 0 ;  idx < 4 ;  idx += 1)
                  తొలగించు చివరి_విలువ[idx];
      }

      anyedge_విలువ *చివరి_విలువ[4];
      vvp_fun_anyedge_state_s(స్థిరరాశి vvp_fun_anyedge_state_s&) = తొలగించు;
      vvp_fun_anyedge_state_s& క్రియాకారకం=(స్థిరరాశి vvp_fun_anyedge_state_s&) = తొలగించు;
};

vvp_fun_anyedge::vvp_fun_anyedge()
{
      కొరకు (గుర్తులేని idx = 0 ;  idx < 4 ;  idx += 1)
	    చివరి_విలువ[idx] = 0;
}

vvp_fun_anyedge::~vvp_fun_anyedge()
{
      కొరకు (గుర్తులేని idx = 0 ;  idx < 4 ;  idx += 1)
	    తొలగించు చివరి_విలువ[idx];
}

శూన్య anyedge_vec4_విలువ::duplicate(anyedge_విలువ*&dup)
{
      anyedge_vec4_విలువ*dup_vec4 = get_vec4_విలువ(dup);
      ప్రతిజ్ఞా(dup_vec4);
      dup_vec4->సమితి(old_బిట్లు);
}

తార్కిక anyedge_vec4_విలువ::recv_vec4(స్థిరరాశి vvp_vector4_t&బిట్టు)
{
      తార్కిక flag = అబద్ధం;

      ఒకవేళ (old_బిట్లు.size() != బిట్టు.size()) {
	    ఒకవేళ (old_బిట్లు.size() == 0) {
		    // Special case: If we've not seen any input yet
		    // (old_bits.size()==0) then replace it will a reference
		    // vector that is 'bx. Then compare that with the input
		    // to see if we are processing a change from 'bx.
		  old_బిట్లు = vvp_vector4_t(బిట్టు.size(), BIT4_X);
		  ఒకవేళ (old_బిట్లు.eeq(బిట్టు))
			flag = అబద్ధం;
		  లేకపోతే
			flag = నిజం;

	    } లేకపోతే {
		  flag = నిజం;
	    }

      } లేకపోతే {
	    కొరకు (గుర్తులేని idx = 0 ;  idx < బిట్టు.size() ;  idx += 1) {
		  ఒకవేళ (old_బిట్లు.value(idx) != బిట్టు.value(idx)) {
			flag = నిజం;
			ఆపు;
		  }
	    }
      }

      ఒకవేళ (flag) {
	    old_బిట్లు = బిట్టు;
      }

      తిరిగి flag;
}

తార్కిక anyedge_vec4_విలువ::recv_vec4_pv(స్థిరరాశి vvp_vector4_t&బిట్టు, గుర్తులేని పునాది,
				      గుర్తులేని vwid)
{
      vvp_vector4_t tmp = old_బిట్లు;
      ఒకవేళ (tmp.size() == 0)
	    tmp = vvp_vector4_t(vwid, BIT4_Z);
      ప్రతిజ్ఞా(పునాది + బిట్టు.size()<= vwid);
      ప్రతిజ్ఞా(tmp.size() == vwid);
      tmp.సమితి_vec(పునాది, బిట్టు);

      తిరిగి recv_vec4(tmp);
}

శూన్య anyedge_real_విలువ::duplicate(anyedge_విలువ*&dup)
{
      anyedge_real_విలువ*dup_real = get_real_విలువ(dup);
      ప్రతిజ్ఞా(dup_real);
      dup_real->సమితి(old_బిట్లు);
}

తార్కిక anyedge_real_విలువ::recv_real(ద్విగుణ బిట్టు)
{
      ఒకవేళ (old_బిట్లు != బిట్టు) {
	    old_బిట్లు = బిట్టు;
            తిరిగి నిజం;
      }
      తిరిగి అబద్ధం;
}

శూన్య anyedge_string_విలువ::duplicate(anyedge_విలువ*&dup)
{
      anyedge_string_విలువ*dup_string = get_string_విలువ(dup);
      ప్రతిజ్ఞా(dup_string);
      dup_string->సమితి(old_బిట్లు);
}

తార్కిక anyedge_string_విలువ::recv_string(స్థిరరాశి std::string&బిట్టు)
{
      ఒకవేళ (old_బిట్లు != బిట్టు) {
	    old_బిట్లు = బిట్టు;
            తిరిగి నిజం;
      }
      తిరిగి అబద్ధం;
}

vvp_fun_anyedge_sa::vvp_fun_anyedge_sa()
: దారాలు_2(0)
{
}

vvp_fun_anyedge_sa::~vvp_fun_anyedge_sa()
{
}

vthread_t vvp_fun_anyedge_sa::కూడు_నిరీక్షణ_thread(vthread_t thread)
{
      vthread_t tmp = దారాలు_2;
      దారాలు_2 = thread;

      తిరిగి tmp;
}

శూన్య vvp_fun_anyedge_sa::recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                                   vvp_context_t)
{
      anyedge_vec4_విలువ*value = get_vec4_విలువ(చివరి_విలువ[పోర్టు.పోర్టు()]);
      ప్రతిజ్ఞా(value);
      ఒకవేళ (value->recv_vec4(బిట్టు)) {
	    నిరంతరం_నిరీక్షణ_దారాలు(దారాలు_2);
	    vvp_net_t*net = పోర్టు.ptr();
	    net->send_vec4(బిట్టు, 0);
      }
}

శూన్య vvp_fun_anyedge_sa::recv_vec4_pv(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
				      గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t)
{
      anyedge_vec4_విలువ*value = get_vec4_విలువ(చివరి_విలువ[పోర్టు.పోర్టు()]);
      ప్రతిజ్ఞా(value);
      ఒకవేళ (value->recv_vec4_pv(బిట్టు, పునాది, vwid)) {
	    నిరంతరం_నిరీక్షణ_దారాలు(దారాలు_2);
	    vvp_net_t*net = పోర్టు.ptr();
	    net->send_vec4(బిట్టు, 0);
      }
}

శూన్య vvp_fun_anyedge_sa::recv_real(vvp_net_ptr_t పోర్టు, ద్విగుణ బిట్టు,
                                   vvp_context_t)
{
      anyedge_real_విలువ*value = get_real_విలువ(చివరి_విలువ[పోర్టు.పోర్టు()]);
      ప్రతిజ్ఞా(value);
      ఒకవేళ (value->recv_real(బిట్టు)) {
	    నిరంతరం_నిరీక్షణ_దారాలు(దారాలు_2);
	    vvp_net_t*net = పోర్టు.ptr();
	    net->send_vec4(vvp_vector4_t(), 0);
      }
}

శూన్య vvp_fun_anyedge_sa::recv_string(vvp_net_ptr_t పోర్టు, స్థిరరాశి std::string&బిట్టు,
				     vvp_context_t)
{
      anyedge_string_విలువ*value = get_string_విలువ(చివరి_విలువ[పోర్టు.పోర్టు()]);
      ప్రతిజ్ఞా(value);
      ఒకవేళ (value->recv_string(బిట్టు)) {
	    నిరంతరం_నిరీక్షణ_దారాలు(దారాలు_2);
	    vvp_net_t*net = పోర్టు.ptr();
	    net->send_vec4(vvp_vector4_t(), 0);
      }
}

/*
 * An anyedge receiving an object should do nothing with it, but should
 * trigger waiting threads.
 */
శూన్య vvp_fun_anyedge_sa::recv_object(vvp_net_ptr_t పోర్టు, vvp_object_t,
				     vvp_context_t)
{
      నిరంతరం_నిరీక్షణ_దారాలు(దారాలు_2);
      vvp_net_t*net = పోర్టు.ptr();
      net->send_vec4(vvp_vector4_t(), 0);
}

vvp_fun_anyedge_aa::vvp_fun_anyedge_aa()
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_కూడు_అంశం_గమ్యం_context(ఇది, context_scope_);
}

vvp_fun_anyedge_aa::~vvp_fun_anyedge_aa()
{
}

శూన్య vvp_fun_anyedge_aa::alloc_instance(vvp_context_t context)
{
      vvp_సమితి_context_అంశం(context, context_idx_, కొత్త vvp_fun_anyedge_state_s);
      reset_instance(context);
}

శూన్య vvp_fun_anyedge_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
            (vvp_get_context_అంశం(context, context_idx_));

      state->దారాలు = 0;
      కొరకు (గుర్తులేని idx = 0 ;  idx < 4 ;  idx += 1) {
	    ఒకవేళ (చివరి_విలువ[idx])
	          చివరి_విలువ[idx]->duplicate(state->చివరి_విలువ[idx]);
	    లేకపోతే ఒకవేళ (state->చివరి_విలువ[idx])
		  state->చివరి_విలువ[idx]->reset();
      }
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
శూన్య vvp_fun_anyedge_aa::free_instance(vvp_context_t context)
{
      vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
            (vvp_get_context_అంశం(context, context_idx_));
      తొలగించు state;
}
#అయితే_ముగింపు

vthread_t vvp_fun_anyedge_aa::కూడు_నిరీక్షణ_thread(vthread_t thread)
{
      vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
            (vthread_get_wt_context_అంశం(context_idx_));

      vthread_t tmp = state->దారాలు;
      state->దారాలు = thread;

      తిరిగి tmp;
}

శూన్య vvp_fun_anyedge_aa::recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                                   vvp_context_t context)
{
      ఒకవేళ (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_అంశం(context, context_idx_));

            anyedge_vec4_విలువ*value = get_vec4_విలువ(state->చివరి_విలువ[పోర్టు.పోర్టు()]);
            ప్రతిజ్ఞా(value);
            ఒకవేళ (value->recv_vec4(బిట్టు)) {
                  నిరంతరం_నిరీక్షణ_దారాలు(state->దారాలు);
                  vvp_net_t*net = పోర్టు.ptr();
                  net->send_vec4(బిట్టు, context);
            }
      } లేకపోతే {
            context = context_scope_->సజీవ_contexts;
            అయితే (context) {
                  recv_vec4(పోర్టు, బిట్టు, context);
                  context = vvp_get_తదుపరి_context(context);
            }
            anyedge_vec4_విలువ*value = get_vec4_విలువ(చివరి_విలువ[పోర్టు.పోర్టు()]);
            ప్రతిజ్ఞా(value);
            value->సమితి(బిట్టు);
      }
}

శూన్య vvp_fun_anyedge_aa::recv_real(vvp_net_ptr_t పోర్టు, ద్విగుణ బిట్టు,
                                   vvp_context_t context)
{
      ఒకవేళ (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_అంశం(context, context_idx_));

            anyedge_real_విలువ*value = get_real_విలువ(state->చివరి_విలువ[పోర్టు.పోర్టు()]);
            ప్రతిజ్ఞా(value);
            ఒకవేళ (value->recv_real(బిట్టు)) {
                  నిరంతరం_నిరీక్షణ_దారాలు(state->దారాలు);
                  vvp_net_t*net = పోర్టు.ptr();
                  net->send_vec4(vvp_vector4_t(), context);
            }
      } లేకపోతే {
            context = context_scope_->సజీవ_contexts;
            అయితే (context) {
                  recv_real(పోర్టు, బిట్టు, context);
                  context = vvp_get_తదుపరి_context(context);
            }
            anyedge_real_విలువ*value = get_real_విలువ(చివరి_విలువ[పోర్టు.పోర్టు()]);
            ప్రతిజ్ఞా(value);
            value->సమితి(బిట్టు);
      }
}

శూన్య vvp_fun_anyedge_aa::recv_string(vvp_net_ptr_t పోర్టు, స్థిరరాశి std::string&బిట్టు,
				     vvp_context_t context)
{
      ఒకవేళ (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_అంశం(context, context_idx_));

            anyedge_string_విలువ*value = get_string_విలువ(state->చివరి_విలువ[పోర్టు.పోర్టు()]);
            ప్రతిజ్ఞా(value);
            ఒకవేళ (value->recv_string(బిట్టు)) {
                  నిరంతరం_నిరీక్షణ_దారాలు(state->దారాలు);
                  vvp_net_t*net = పోర్టు.ptr();
                  net->send_vec4(vvp_vector4_t(), context);
            }
      } లేకపోతే {
            context = context_scope_->సజీవ_contexts;
            అయితే (context) {
                  recv_string(పోర్టు, బిట్టు, context);
                  context = vvp_get_తదుపరి_context(context);
            }
            anyedge_string_విలువ*value = get_string_విలువ(చివరి_విలువ[పోర్టు.పోర్టు()]);
            ప్రతిజ్ఞా(value);
            value->సమితి(బిట్టు);
      }
}

vvp_fun_event_or::vvp_fun_event_or(vvp_net_t*పునాది_net)
: పునాది_net_2(పునాది_net)
{
}

vvp_fun_event_or::~vvp_fun_event_or()
{
}

vvp_fun_event_or_sa::vvp_fun_event_or_sa(vvp_net_t*పునాది_net)
: vvp_fun_event_or(పునాది_net), దారాలు_2(0)
{
}

vvp_fun_event_or_sa::~vvp_fun_event_or_sa()
{
}

vthread_t vvp_fun_event_or_sa::కూడు_నిరీక్షణ_thread(vthread_t thread)
{
      vthread_t tmp = దారాలు_2;
      దారాలు_2 = thread;

      తిరిగి tmp;
}

శూన్య vvp_fun_event_or_sa::recv_vec4(vvp_net_ptr_t, స్థిరరాశి vvp_vector4_t&బిట్టు,
                                    vvp_context_t)
{
      నిరంతరం_నిరీక్షణ_దారాలు(దారాలు_2);
      పునాది_net_2->send_vec4(బిట్టు, 0);
}

vvp_fun_event_or_aa::vvp_fun_event_or_aa(vvp_net_t*పునాది_net)
: vvp_fun_event_or(పునాది_net)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_కూడు_అంశం_గమ్యం_context(ఇది, context_scope_);
}

vvp_fun_event_or_aa::~vvp_fun_event_or_aa()
{
}

శూన్య vvp_fun_event_or_aa::alloc_instance(vvp_context_t context)
{
      vvp_సమితి_context_అంశం(context, context_idx_, కొత్త waitable_state_s);
}

శూన్య vvp_fun_event_or_aa::reset_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_అంశం(context, context_idx_));

      state->దారాలు = 0;
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
శూన్య vvp_fun_event_or_aa::free_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_అంశం(context, context_idx_));
      తొలగించు state;
}
#అయితే_ముగింపు

vthread_t vvp_fun_event_or_aa::కూడు_నిరీక్షణ_thread(vthread_t thread)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vthread_get_wt_context_అంశం(context_idx_));

      vthread_t tmp = state->దారాలు;
      state->దారాలు = thread;

      తిరిగి tmp;
}

శూన్య vvp_fun_event_or_aa::recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                                    vvp_context_t context)
{
      ఒకవేళ (context) {
            waitable_state_s*state = static_cast<waitable_state_s*>
                  (vvp_get_context_అంశం(context, context_idx_));

            నిరంతరం_నిరీక్షణ_దారాలు(state->దారాలు);
            పునాది_net_2->send_vec4(బిట్టు, context);
      } లేకపోతే {
            context = context_scope_->సజీవ_contexts;
            అయితే (context) {
                  recv_vec4(పోర్టు, బిట్టు, context);
                  context = vvp_get_తదుపరి_context(context);
            }
      }
}

vvp_named_event::vvp_named_event(__vpiHandle*h)
{
      handle_ = h;
}

vvp_named_event::~vvp_named_event()
{
}

vvp_named_event_sa::vvp_named_event_sa(__vpiHandle*h)
: vvp_named_event(h), దారాలు_2(0)
{
}

vvp_named_event_sa::~vvp_named_event_sa()
{
}

vthread_t vvp_named_event_sa::కూడు_నిరీక్షణ_thread(vthread_t thread)
{
      vthread_t tmp = దారాలు_2;
      దారాలు_2 = thread;

      తిరిగి tmp;
}

శూన్య vvp_named_event_sa::recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                                   vvp_context_t)
{
      నిరంతరం_నిరీక్షణ_దారాలు(దారాలు_2);
      vvp_net_t*net = పోర్టు.ptr();
      net->send_vec4(బిట్టు, 0);

      __vpiNamedEvent*obj = dynamic_cast<__vpiNamedEvent*>(handle_);
      ప్రతిజ్ఞా(obj);
      obj->నిరంతరం_vpi_callbacks();
}

vvp_named_event_aa::vvp_named_event_aa(__vpiHandle*h)
: vvp_named_event(h)
{
      context_idx_ = vpip_కూడు_అంశం_గమ్యం_context(ఇది, vpip_peek_context_scope());
}

vvp_named_event_aa::~vvp_named_event_aa()
{
}

శూన్య vvp_named_event_aa::alloc_instance(vvp_context_t context)
{
      vvp_సమితి_context_అంశం(context, context_idx_, కొత్త waitable_state_s);
}

శూన్య vvp_named_event_aa::reset_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_అంశం(context, context_idx_));

      state->దారాలు = 0;
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
శూన్య vvp_named_event_aa::free_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_అంశం(context, context_idx_));
      తొలగించు state;
}
#అయితే_ముగింపు

vthread_t vvp_named_event_aa::కూడు_నిరీక్షణ_thread(vthread_t thread)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vthread_get_wt_context_అంశం(context_idx_));

      vthread_t tmp = state->దారాలు;
      state->దారాలు = thread;

      తిరిగి tmp;
}

శూన్య vvp_named_event_aa::recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                                   vvp_context_t context)
{
      ప్రతిజ్ఞా(context);

      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_అంశం(context, context_idx_));

      నిరంతరం_నిరీక్షణ_దారాలు(state->దారాలు);
      vvp_net_t*net = పోర్టు.ptr();
      net->send_vec4(బిట్టు, context);
}

/*
**  Create an event functor
**  edge:  compile_event(label, type, argc, argv, debug_flag)
**  or:    compile_event(label, NULL, argc, argv, debug_flag)
**
**  Named events are handled elsewhere.
*/

స్థిర శూన్య compile_event_or(అక్షరం*label, గుర్తులేని argc, నిర్మాణం symb_s*argv);

శూన్య compile_event(అక్షరం*label, అక్షరం*type, గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      vvp_net_fun_t*fun = 0;

      ఒకవేళ (type == 0) {
	    compile_event_or(label, argc, argv);
	    తిరిగి;
      }

      ఒకవేళ (strcmp(type,"anyedge") == 0) {

	    విడుదల(type);

            ఒకవేళ (vpip_peek_current_scope()->is_automatic()) {
                  fun = కొత్త vvp_fun_anyedge_aa;
            } లేకపోతే {
                  fun = కొత్త vvp_fun_anyedge_sa;
            }

      } లేకపోతే {

	    vvp_fun_అంచు::అంచు_t అంచు_type = vvp_అంచు_none;

	    ఒకవేళ (strcmp(type,"posedge") == 0)
		  అంచు_type = vvp_అంచు_posedge;
	    లేకపోతే ఒకవేళ (strcmp(type,"negedge") == 0)
		  అంచు_type = vvp_అంచు_negedge;
	    లేకపోతే ఒకవేళ (strcmp(type,"edge") == 0)
		  అంచు_type = vvp_అంచు_అంచు;

	    ప్రతిజ్ఞా(argc <= 4);
	    విడుదల(type);

            ఒకవేళ (vpip_peek_current_scope()->is_automatic()) {
                  fun = కొత్త vvp_fun_అంచు_aa(అంచు_type);
            } లేకపోతే {
                  fun = కొత్త vvp_fun_అంచు_sa(అంచు_type);
            }

      }

      vvp_net_t* ptr = కొత్త vvp_net_t;
      ptr->fun = fun;

      define_functor_చిహ్నం(label, ptr);
      విడుదల(label);

      inputs_connect(ptr, argc, argv);
      విడుదల(argv);
}

స్థిర శూన్య compile_event_or(అక్షరం*label, గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      vvp_net_t*పునాది_net = కొత్త vvp_net_t;
      ఒకవేళ (vpip_peek_current_scope()->is_automatic()) {
            పునాది_net->fun = కొత్త vvp_fun_event_or_aa(పునాది_net);
      } లేకపోతే {
            పునాది_net->fun = కొత్త vvp_fun_event_or_sa(పునాది_net);
      }
      define_functor_చిహ్నం(label, పునాది_net);
      విడుదల(label);

	/* This is a simplified version of a wide functor. We don't
	   care about the data values or what port they arrived on,
	   so we can use a single shared functor. */
      vvp_net_t*curr_net = పునాది_net;
      కొరకు (గుర్తులేని idx = 0 ;  idx < argc ;  idx += 1) {
	    ఒకవేళ (idx > 0 && (idx % 4) == 0) {
		  curr_net = కొత్త vvp_net_t;
		  curr_net->fun = పునాది_net->fun;
	    }
	    ఆదానం_connect(curr_net, idx % 4, argv[idx].పాఠ్యం);
      }
      విడుదల(argv);
}

/*
 * This handles the compile of named events. This functor has no
 * inputs, it is only accessed by behavioral trigger statements, which
 * in vvp are %set instructions.
 */
శూన్య compile_named_event(అక్షరం*label, అక్షరం*name, తార్కిక local_flag)
{
      vvp_net_t*ptr = కొత్త vvp_net_t;

      vpiHandle obj = vpip_make_named_event(name, ptr);

      ఒకవేళ (vpip_peek_current_scope()->is_automatic()) {
            ptr->fun = కొత్త vvp_named_event_aa(obj);
      } లేకపోతే {
            ptr->fun = కొత్త vvp_named_event_sa(obj);
      }
      define_functor_చిహ్నం(label, ptr);
      compile_vpi_చిహ్నం(label, obj);
      ఒకవేళ (! local_flag) vpip_attach_గమ్యం_current_scope(obj);

      విడుదల(label);
      తొలగించు[] name;
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
శూన్య named_event_delete(__vpiHandle*handle)
{
      తొలగించు dynamic_cast<__vpiNamedEvent *>(handle);
}
#అయితే_ముగింపు
