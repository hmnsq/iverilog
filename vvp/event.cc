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

فراغ waitable_hooks_s::پرلپسې_انتظار_تارونه(vthread_t&تارونه)
{
	// Run the non-blocking event controls.
      وروستی = &event_ctls;
      لكل (evctl*cur = event_ctls; cur != 0;) {
	    إذا (cur->dec_and_پرلپسې()) {
		  evctl*nxt = cur->بل_2;
		  ړنګول cur;
		  cur = nxt;
		  *وروستی = cur;
	    } وإلا {
		  وروستی = &(cur->بل_2);
		  cur = cur->بل_2;
	    }
      }

      vthread_t tmp = تارونه;
      إذا (tmp == 0) أرجع;
      تارونه = 0;

      vthread_مهالوېش_list(tmp);
}

evctl::evctl(uint64_t ecount)
{
      ecount_ = ecount;
      بل_2 = 0;
}

منطقي evctl::dec_and_پرلپسې()
{
      أكد(ecount_ != 0);

      ecount_ -= 1;
      إذا (ecount_ == 0) پرلپسې_پرلپسې();

      أرجع ecount_ == 0;
}

evctl_real::evctl_real(__vpiHandle*handle, مزدوج value,
                       uint64_t ecount)
:evctl(ecount)
{
      handle_ = handle;
      ارزښت_2 = value;
}

فراغ evctl_real::پرلپسې_پرلپسې()
{
      t_vpi_ارزښت val;

      val.format = vpiRealVal;
      val.value.real = ارزښت_2;
      vpi_put_ارزښت(handle_, &val, 0, vpiNoDelay);
}

فراغ مهالوېش_evctl(__vpiHandle*handle, مزدوج value,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      أكد(ep);
	// Now add this call to the end of the event list.
      *(ep->وروستی) = جديد evctl_real(handle, value, ecount);
      ep->وروستی = &((*(ep->وروستی))->بل_2);
}

evctl_ویکټور::evctl_ویکټور(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&value,
                           بینښه off, بینښه wid, uint64_t ecount)
:evctl(ecount), ptr_(ptr), ارزښت_2(value)
{
      off_ = off;
      wid_ = wid;
}

فراغ evctl_ویکټور::پرلپسې_پرلپسې()
{
      إذا (wid_ != 0) {
	    vvp_send_vec4_pv(ptr_, ارزښت_2, off_, wid_, 0);
      } وإلا {
	    vvp_send_vec4(ptr_, ارزښت_2, 0);
      }
}

فراغ مهالوېش_evctl(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&value,
                    بینښه دننه_واټن_2, بینښه wid,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      أكد(ep);
	// Now add this call to the end of the event list.
      *(ep->وروستی) = جديد evctl_ویکټور(ptr, value, دننه_واټن_2, wid, ecount);
      ep->وروستی = &((*(ep->وروستی))->بل_2);
}

evctl_array::evctl_array(vvp_array_t حافظه, بینښه شاخص,
                         ثابت vvp_vector4_t&value, بینښه off,
                         uint64_t ecount)
:evctl(ecount), ارزښت_2(value)
{
      mem_ = حافظه;
      idx_ = شاخص;
      off_ = off;
}

فراغ evctl_array::پرلپسې_پرلپسې()
{
      mem_->سېټ_کلمه(idx_, off_, ارزښت_2);
}

فراغ مهالوېش_evctl(vvp_array_t حافظه, بینښه شاخص,
                    ثابت vvp_vector4_t&value, بینښه دننه_واټن_2,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      أكد(ep);
	// Now add this call to the end of the event list.
      *(ep->وروستی) = جديد evctl_array(حافظه, شاخص, value, دننه_واټن_2, ecount);
      ep->وروستی = &((*(ep->وروستی))->بل_2);
}

evctl_array_r::evctl_array_r(vvp_array_t حافظه, بینښه شاخص,
                             مزدوج value, uint64_t ecount)
:evctl(ecount)
{
      mem_ = حافظه;
      idx_ = شاخص;
      ارزښت_2 = value;
}

فراغ evctl_array_r::پرلپسې_پرلپسې()
{
      mem_->سېټ_کلمه(idx_, ارزښت_2);
}

فراغ مهالوېش_evctl(vvp_array_t حافظه, بینښه شاخص,
                    مزدوج value,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      أكد(ep);
	// Now add this call to the end of the event list.
      *(ep->وروستی) = جديد evctl_array_r(حافظه, شاخص, value, ecount);
      ep->وروستی = &((*(ep->وروستی))->بل_2);
}

مضمن vvp_fun_څنډه::څنډه_t VVP_څنډه(vvp_bit4_t from, vvp_bit4_t to)
{
      أرجع 1 << ((from << 2) | to);
}

ثابت vvp_fun_څنډه::څنډه_t vvp_څنډه_posedge
      = VVP_څنډه(BIT4_0,BIT4_1)
      | VVP_څنډه(BIT4_0,BIT4_X)
      | VVP_څنډه(BIT4_0,BIT4_Z)
      | VVP_څنډه(BIT4_X,BIT4_1)
      | VVP_څنډه(BIT4_Z,BIT4_1)
      ;

ثابت vvp_fun_څنډه::څنډه_t vvp_څنډه_negedge
      = VVP_څنډه(BIT4_1,BIT4_0)
      | VVP_څنډه(BIT4_1,BIT4_X)
      | VVP_څنډه(BIT4_1,BIT4_Z)
      | VVP_څنډه(BIT4_X,BIT4_0)
      | VVP_څنډه(BIT4_Z,BIT4_0)
      ;

ثابت vvp_fun_څنډه::څنډه_t vvp_څنډه_څنډه
      = VVP_څنډه(BIT4_0,BIT4_1)
      | VVP_څنډه(BIT4_1,BIT4_0)
      | VVP_څنډه(BIT4_0,BIT4_X)
      | VVP_څنډه(BIT4_X,BIT4_0)
      | VVP_څنډه(BIT4_0,BIT4_Z)
      | VVP_څنډه(BIT4_Z,BIT4_0)
      | VVP_څنډه(BIT4_X,BIT4_1)
      | VVP_څنډه(BIT4_1,BIT4_X)
      | VVP_څنډه(BIT4_Z,BIT4_1)
      | VVP_څنډه(BIT4_1,BIT4_Z)
      ;

ثابت vvp_fun_څنډه::څنډه_t vvp_څنډه_none    = 0;

بنية vvp_fun_څنډه_state_s : عام waitable_state_s {
      vvp_fun_څنډه_state_s()
      {
            لكل (بینښه idx = 0 ;  idx < 4 ;  idx += 1)
                  بټونه[idx] = BIT4_X;
      }

      vvp_bit4_t بټونه[4];
};

vvp_fun_څنډه::vvp_fun_څنډه(څنډه_t e)
: څنډه_2(e)
{
      لكل (بینښه idx = 0 ;  idx < 4 ;  idx += 1)
            بټونه_2[idx] = BIT4_X;
}

vvp_fun_څنډه::~vvp_fun_څنډه()
{
}

منطقي vvp_fun_څنډه::recv_vec4_(ثابت vvp_vector4_t&بټ,
                              vvp_bit4_t&old_بټ, vthread_t&تارونه)
{
	/* See what kind of edge this represents. */
      څنډه_t ماسک = VVP_څنډه(old_بټ, بټ.value(0));

	/* Save the current input for the next time around. */
      old_بټ = بټ.value(0);

      إذا ((څنډه_2 == vvp_څنډه_none) || (څنډه_2 & ماسک)) {
	    پرلپسې_انتظار_تارونه(تارونه);
            أرجع ریښتیا;
      }
      أرجع خطأ;
}

vvp_fun_څنډه_sa::vvp_fun_څنډه_sa(څنډه_t e)
: vvp_fun_څنډه(e), تارونه_2(0)
{
}

vvp_fun_څنډه_sa::~vvp_fun_څنډه_sa()
{
}

vthread_t vvp_fun_څنډه_sa::جمع_کړه_انتظار_thread(vthread_t thread)
{
      vthread_t tmp = تارونه_2;
      تارونه_2 = thread;

      أرجع tmp;
}

فراغ vvp_fun_څنډه_sa::recv_vec4(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
                                vvp_context_t)
{
      إذا (recv_vec4_(بټ, بټونه_2[پورټ.پورټ()], تارونه_2)) {
	    vvp_net_t*net = پورټ.ptr();
	    net->send_vec4(بټ, 0);
      }
}

فراغ vvp_fun_څنډه_sa::recv_vec4_pv(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
				   بینښه بنسټ, بینښه vwid, vvp_context_t)
{
      أكد(بنسټ == 0);
      إذا (recv_vec4_(بټ, بټونه_2[پورټ.پورټ()], تارونه_2)) {
	    vvp_net_t*net = پورټ.ptr();
	    net->send_vec4_pv(بټ, بنسټ, vwid, 0);
      }
}

vvp_fun_څنډه_aa::vvp_fun_څنډه_aa(څنډه_t e)
: vvp_fun_څنډه(e)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_جمع_کړه_توکی_منزل_context(هذا, context_scope_);
}

vvp_fun_څنډه_aa::~vvp_fun_څنډه_aa()
{
}

فراغ vvp_fun_څنډه_aa::alloc_instance(vvp_context_t context)
{
      vvp_سېټ_context_توکی(context, context_idx_, جديد vvp_fun_څنډه_state_s);
      reset_instance(context);
}

فراغ vvp_fun_څنډه_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_څنډه_state_s*state = static_cast<vvp_fun_څنډه_state_s*>
            (vvp_get_context_توکی(context, context_idx_));

      state->تارونه = 0;
      لكل (بینښه idx = 0 ;  idx < 4 ;  idx += 1)
            state->بټونه[idx] = بټونه_2[idx];
}

#که_تعریف_شوی CHECK_WITH_VALGRIND
فراغ vvp_fun_څنډه_aa::free_instance(vvp_context_t context)
{
      vvp_fun_څنډه_state_s*state = static_cast<vvp_fun_څنډه_state_s*>
            (vvp_get_context_توکی(context, context_idx_));
      ړنګول state;
}
#د_که_پای

vthread_t vvp_fun_څنډه_aa::جمع_کړه_انتظار_thread(vthread_t thread)
{
      vvp_fun_څنډه_state_s*state = static_cast<vvp_fun_څنډه_state_s*>
            (vthread_get_wt_context_توکی(context_idx_));

      vthread_t tmp = state->تارونه;
      state->تارونه = thread;

      أرجع tmp;
}

فراغ vvp_fun_څنډه_aa::recv_vec4(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
                                vvp_context_t context)
{
      إذا (context) {
            vvp_fun_څنډه_state_s*state = static_cast<vvp_fun_څنډه_state_s*>
                  (vvp_get_context_توکی(context, context_idx_));

            إذا (recv_vec4_(بټ, state->بټونه[پورټ.پورټ()], state->تارونه)) {
                  vvp_net_t*net = پورټ.ptr();
                  net->send_vec4(بټ, context);
            }
      } وإلا {
            context = context_scope_->ژوندی_contexts;
            طالما (context) {
                  recv_vec4(پورټ, بټ, context);
                  context = vvp_get_بل_context(context);
            }
            بټونه_2[پورټ.پورټ()] = بټ.value(0);
      }
}

صنف anyedge_ارزښت {

    عام:
      anyedge_ارزښت() {};
      مجازي ~anyedge_ارزښت() {};

      مجازي فراغ reset() = 0;

      مجازي فراغ duplicate(anyedge_ارزښت*&dup) = 0;
};

صنف anyedge_vec4_ارزښت : عام anyedge_ارزښت {

    عام:
      anyedge_vec4_ارزښت() {};
      مجازي ~anyedge_vec4_ارزښت() override {};

      فراغ reset() override { old_بټونه.سېټ_منزل_x(); }

      فراغ سېټ(ثابت vvp_vector4_t&بټ) { old_بټونه = بټ; };

      فراغ duplicate(anyedge_ارزښت*&dup) override;

      منطقي recv_vec4(ثابت vvp_vector4_t&بټ);

      منطقي recv_vec4_pv(ثابت vvp_vector4_t&بټ, بینښه بنسټ,
			بینښه vwid);

    خاص:
      vvp_vector4_t old_بټونه;
};

ساكن anyedge_vec4_ارزښت*get_vec4_ارزښت(anyedge_ارزښت*&value)
{
      anyedge_vec4_ارزښت*vec4_ارزښت = dynamic_cast<anyedge_vec4_ارزښت*>(value);
      إذا (!value) {
	    vec4_ارزښت = جديد anyedge_vec4_ارزښت();
	    ړنګول value;
	    value = vec4_ارزښت;
      }
      أرجع vec4_ارزښت;
}

صنف anyedge_real_ارزښت : عام anyedge_ارزښت {

    عام:
      anyedge_real_ارزښت() : old_بټونه(0.0) {};
      مجازي ~anyedge_real_ارزښت() override {};

      فراغ reset() override { old_بټونه = 0.0; }

      فراغ سېټ(مزدوج بټ) { old_بټونه = بټ; };

      فراغ duplicate(anyedge_ارزښت*&dup) override;

      منطقي recv_real(مزدوج بټ);

    خاص:
      مزدوج old_بټونه;
};

ساكن anyedge_real_ارزښت*get_real_ارزښت(anyedge_ارزښت*&value)
{
      anyedge_real_ارزښت*real_ارزښت = dynamic_cast<anyedge_real_ارزښت*>(value);
      إذا (!value) {
	    real_ارزښت = جديد anyedge_real_ارزښت();
	    ړنګول value;
	    value = real_ارزښت;
      }
      أرجع real_ارزښت;
}

صنف anyedge_string_ارزښت : عام anyedge_ارزښت {

    عام:
      anyedge_string_ارزښت() {};
      مجازي ~anyedge_string_ارزښت() override {};

      فراغ reset() override { old_بټونه.clear(); }

      فراغ سېټ(ثابت std::string&بټ) { old_بټونه = بټ; };

      فراغ duplicate(anyedge_ارزښت*&dup) override;

      منطقي recv_string(ثابت std::string&بټ);

    خاص:
      std::string old_بټونه;
};

ساكن anyedge_string_ارزښت*get_string_ارزښت(anyedge_ارزښت*&value)
{
      anyedge_string_ارزښت*string_ارزښت = dynamic_cast<anyedge_string_ارزښت*>(value);
      إذا (!value) {
	    string_ارزښت = جديد anyedge_string_ارزښت();
	    ړنګول value;
	    value = string_ارزښت;
      }
      أرجع string_ارزښت;
}

بنية vvp_fun_anyedge_state_s : عام waitable_state_s {
      vvp_fun_anyedge_state_s()
      {
            لكل (بینښه idx = 0 ;  idx < 4 ;  idx += 1)
                  وروستی_ارزښت[idx] = 0;
      }

      ~vvp_fun_anyedge_state_s()
      {
            لكل (بینښه idx = 0 ;  idx < 4 ;  idx += 1)
                  ړنګول وروستی_ارزښت[idx];
      }

      anyedge_ارزښت *وروستی_ارزښت[4];
      vvp_fun_anyedge_state_s(ثابت vvp_fun_anyedge_state_s&) = ړنګول;
      vvp_fun_anyedge_state_s& چلوونکی=(ثابت vvp_fun_anyedge_state_s&) = ړنګول;
};

vvp_fun_anyedge::vvp_fun_anyedge()
{
      لكل (بینښه idx = 0 ;  idx < 4 ;  idx += 1)
	    وروستی_ارزښت[idx] = 0;
}

vvp_fun_anyedge::~vvp_fun_anyedge()
{
      لكل (بینښه idx = 0 ;  idx < 4 ;  idx += 1)
	    ړنګول وروستی_ارزښت[idx];
}

فراغ anyedge_vec4_ارزښت::duplicate(anyedge_ارزښت*&dup)
{
      anyedge_vec4_ارزښت*dup_vec4 = get_vec4_ارزښت(dup);
      أكد(dup_vec4);
      dup_vec4->سېټ(old_بټونه);
}

منطقي anyedge_vec4_ارزښت::recv_vec4(ثابت vvp_vector4_t&بټ)
{
      منطقي flag = خطأ;

      إذا (old_بټونه.size() != بټ.size()) {
	    إذا (old_بټونه.size() == 0) {
		    // Special case: If we've not seen any input yet
		    // (old_bits.size()==0) then replace it will a reference
		    // vector that is 'bx. Then compare that with the input
		    // to see if we are processing a change from 'bx.
		  old_بټونه = vvp_vector4_t(بټ.size(), BIT4_X);
		  إذا (old_بټونه.eeq(بټ))
			flag = خطأ;
		  وإلا
			flag = ریښتیا;

	    } وإلا {
		  flag = ریښتیا;
	    }

      } وإلا {
	    لكل (بینښه idx = 0 ;  idx < بټ.size() ;  idx += 1) {
		  إذا (old_بټونه.value(idx) != بټ.value(idx)) {
			flag = ریښتیا;
			اكسر;
		  }
	    }
      }

      إذا (flag) {
	    old_بټونه = بټ;
      }

      أرجع flag;
}

منطقي anyedge_vec4_ارزښت::recv_vec4_pv(ثابت vvp_vector4_t&بټ, بینښه بنسټ,
				      بینښه vwid)
{
      vvp_vector4_t tmp = old_بټونه;
      إذا (tmp.size() == 0)
	    tmp = vvp_vector4_t(vwid, BIT4_Z);
      أكد(بنسټ + بټ.size()<= vwid);
      أكد(tmp.size() == vwid);
      tmp.سېټ_vec(بنسټ, بټ);

      أرجع recv_vec4(tmp);
}

فراغ anyedge_real_ارزښت::duplicate(anyedge_ارزښت*&dup)
{
      anyedge_real_ارزښت*dup_real = get_real_ارزښت(dup);
      أكد(dup_real);
      dup_real->سېټ(old_بټونه);
}

منطقي anyedge_real_ارزښت::recv_real(مزدوج بټ)
{
      إذا (old_بټونه != بټ) {
	    old_بټونه = بټ;
            أرجع ریښتیا;
      }
      أرجع خطأ;
}

فراغ anyedge_string_ارزښت::duplicate(anyedge_ارزښت*&dup)
{
      anyedge_string_ارزښت*dup_string = get_string_ارزښت(dup);
      أكد(dup_string);
      dup_string->سېټ(old_بټونه);
}

منطقي anyedge_string_ارزښت::recv_string(ثابت std::string&بټ)
{
      إذا (old_بټونه != بټ) {
	    old_بټونه = بټ;
            أرجع ریښتیا;
      }
      أرجع خطأ;
}

vvp_fun_anyedge_sa::vvp_fun_anyedge_sa()
: تارونه_2(0)
{
}

vvp_fun_anyedge_sa::~vvp_fun_anyedge_sa()
{
}

vthread_t vvp_fun_anyedge_sa::جمع_کړه_انتظار_thread(vthread_t thread)
{
      vthread_t tmp = تارونه_2;
      تارونه_2 = thread;

      أرجع tmp;
}

فراغ vvp_fun_anyedge_sa::recv_vec4(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
                                   vvp_context_t)
{
      anyedge_vec4_ارزښت*value = get_vec4_ارزښت(وروستی_ارزښت[پورټ.پورټ()]);
      أكد(value);
      إذا (value->recv_vec4(بټ)) {
	    پرلپسې_انتظار_تارونه(تارونه_2);
	    vvp_net_t*net = پورټ.ptr();
	    net->send_vec4(بټ, 0);
      }
}

فراغ vvp_fun_anyedge_sa::recv_vec4_pv(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
				      بینښه بنسټ, بینښه vwid, vvp_context_t)
{
      anyedge_vec4_ارزښت*value = get_vec4_ارزښت(وروستی_ارزښت[پورټ.پورټ()]);
      أكد(value);
      إذا (value->recv_vec4_pv(بټ, بنسټ, vwid)) {
	    پرلپسې_انتظار_تارونه(تارونه_2);
	    vvp_net_t*net = پورټ.ptr();
	    net->send_vec4(بټ, 0);
      }
}

فراغ vvp_fun_anyedge_sa::recv_real(vvp_net_ptr_t پورټ, مزدوج بټ,
                                   vvp_context_t)
{
      anyedge_real_ارزښت*value = get_real_ارزښت(وروستی_ارزښت[پورټ.پورټ()]);
      أكد(value);
      إذا (value->recv_real(بټ)) {
	    پرلپسې_انتظار_تارونه(تارونه_2);
	    vvp_net_t*net = پورټ.ptr();
	    net->send_vec4(vvp_vector4_t(), 0);
      }
}

فراغ vvp_fun_anyedge_sa::recv_string(vvp_net_ptr_t پورټ, ثابت std::string&بټ,
				     vvp_context_t)
{
      anyedge_string_ارزښت*value = get_string_ارزښت(وروستی_ارزښت[پورټ.پورټ()]);
      أكد(value);
      إذا (value->recv_string(بټ)) {
	    پرلپسې_انتظار_تارونه(تارونه_2);
	    vvp_net_t*net = پورټ.ptr();
	    net->send_vec4(vvp_vector4_t(), 0);
      }
}

/*
 * An anyedge receiving an object should do nothing with it, but should
 * trigger waiting threads.
 */
فراغ vvp_fun_anyedge_sa::recv_object(vvp_net_ptr_t پورټ, vvp_object_t,
				     vvp_context_t)
{
      پرلپسې_انتظار_تارونه(تارونه_2);
      vvp_net_t*net = پورټ.ptr();
      net->send_vec4(vvp_vector4_t(), 0);
}

vvp_fun_anyedge_aa::vvp_fun_anyedge_aa()
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_جمع_کړه_توکی_منزل_context(هذا, context_scope_);
}

vvp_fun_anyedge_aa::~vvp_fun_anyedge_aa()
{
}

فراغ vvp_fun_anyedge_aa::alloc_instance(vvp_context_t context)
{
      vvp_سېټ_context_توکی(context, context_idx_, جديد vvp_fun_anyedge_state_s);
      reset_instance(context);
}

فراغ vvp_fun_anyedge_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
            (vvp_get_context_توکی(context, context_idx_));

      state->تارونه = 0;
      لكل (بینښه idx = 0 ;  idx < 4 ;  idx += 1) {
	    إذا (وروستی_ارزښت[idx])
	          وروستی_ارزښت[idx]->duplicate(state->وروستی_ارزښت[idx]);
	    وإلا إذا (state->وروستی_ارزښت[idx])
		  state->وروستی_ارزښت[idx]->reset();
      }
}

#که_تعریف_شوی CHECK_WITH_VALGRIND
فراغ vvp_fun_anyedge_aa::free_instance(vvp_context_t context)
{
      vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
            (vvp_get_context_توکی(context, context_idx_));
      ړنګول state;
}
#د_که_پای

vthread_t vvp_fun_anyedge_aa::جمع_کړه_انتظار_thread(vthread_t thread)
{
      vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
            (vthread_get_wt_context_توکی(context_idx_));

      vthread_t tmp = state->تارونه;
      state->تارونه = thread;

      أرجع tmp;
}

فراغ vvp_fun_anyedge_aa::recv_vec4(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
                                   vvp_context_t context)
{
      إذا (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_توکی(context, context_idx_));

            anyedge_vec4_ارزښت*value = get_vec4_ارزښت(state->وروستی_ارزښت[پورټ.پورټ()]);
            أكد(value);
            إذا (value->recv_vec4(بټ)) {
                  پرلپسې_انتظار_تارونه(state->تارونه);
                  vvp_net_t*net = پورټ.ptr();
                  net->send_vec4(بټ, context);
            }
      } وإلا {
            context = context_scope_->ژوندی_contexts;
            طالما (context) {
                  recv_vec4(پورټ, بټ, context);
                  context = vvp_get_بل_context(context);
            }
            anyedge_vec4_ارزښت*value = get_vec4_ارزښت(وروستی_ارزښت[پورټ.پورټ()]);
            أكد(value);
            value->سېټ(بټ);
      }
}

فراغ vvp_fun_anyedge_aa::recv_real(vvp_net_ptr_t پورټ, مزدوج بټ,
                                   vvp_context_t context)
{
      إذا (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_توکی(context, context_idx_));

            anyedge_real_ارزښت*value = get_real_ارزښت(state->وروستی_ارزښت[پورټ.پورټ()]);
            أكد(value);
            إذا (value->recv_real(بټ)) {
                  پرلپسې_انتظار_تارونه(state->تارونه);
                  vvp_net_t*net = پورټ.ptr();
                  net->send_vec4(vvp_vector4_t(), context);
            }
      } وإلا {
            context = context_scope_->ژوندی_contexts;
            طالما (context) {
                  recv_real(پورټ, بټ, context);
                  context = vvp_get_بل_context(context);
            }
            anyedge_real_ارزښت*value = get_real_ارزښت(وروستی_ارزښت[پورټ.پورټ()]);
            أكد(value);
            value->سېټ(بټ);
      }
}

فراغ vvp_fun_anyedge_aa::recv_string(vvp_net_ptr_t پورټ, ثابت std::string&بټ,
				     vvp_context_t context)
{
      إذا (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_توکی(context, context_idx_));

            anyedge_string_ارزښت*value = get_string_ارزښت(state->وروستی_ارزښت[پورټ.پورټ()]);
            أكد(value);
            إذا (value->recv_string(بټ)) {
                  پرلپسې_انتظار_تارونه(state->تارونه);
                  vvp_net_t*net = پورټ.ptr();
                  net->send_vec4(vvp_vector4_t(), context);
            }
      } وإلا {
            context = context_scope_->ژوندی_contexts;
            طالما (context) {
                  recv_string(پورټ, بټ, context);
                  context = vvp_get_بل_context(context);
            }
            anyedge_string_ارزښت*value = get_string_ارزښت(وروستی_ارزښت[پورټ.پورټ()]);
            أكد(value);
            value->سېټ(بټ);
      }
}

vvp_fun_event_or::vvp_fun_event_or(vvp_net_t*بنسټ_net)
: بنسټ_net_2(بنسټ_net)
{
}

vvp_fun_event_or::~vvp_fun_event_or()
{
}

vvp_fun_event_or_sa::vvp_fun_event_or_sa(vvp_net_t*بنسټ_net)
: vvp_fun_event_or(بنسټ_net), تارونه_2(0)
{
}

vvp_fun_event_or_sa::~vvp_fun_event_or_sa()
{
}

vthread_t vvp_fun_event_or_sa::جمع_کړه_انتظار_thread(vthread_t thread)
{
      vthread_t tmp = تارونه_2;
      تارونه_2 = thread;

      أرجع tmp;
}

فراغ vvp_fun_event_or_sa::recv_vec4(vvp_net_ptr_t, ثابت vvp_vector4_t&بټ,
                                    vvp_context_t)
{
      پرلپسې_انتظار_تارونه(تارونه_2);
      بنسټ_net_2->send_vec4(بټ, 0);
}

vvp_fun_event_or_aa::vvp_fun_event_or_aa(vvp_net_t*بنسټ_net)
: vvp_fun_event_or(بنسټ_net)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_جمع_کړه_توکی_منزل_context(هذا, context_scope_);
}

vvp_fun_event_or_aa::~vvp_fun_event_or_aa()
{
}

فراغ vvp_fun_event_or_aa::alloc_instance(vvp_context_t context)
{
      vvp_سېټ_context_توکی(context, context_idx_, جديد waitable_state_s);
}

فراغ vvp_fun_event_or_aa::reset_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_توکی(context, context_idx_));

      state->تارونه = 0;
}

#که_تعریف_شوی CHECK_WITH_VALGRIND
فراغ vvp_fun_event_or_aa::free_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_توکی(context, context_idx_));
      ړنګول state;
}
#د_که_پای

vthread_t vvp_fun_event_or_aa::جمع_کړه_انتظار_thread(vthread_t thread)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vthread_get_wt_context_توکی(context_idx_));

      vthread_t tmp = state->تارونه;
      state->تارونه = thread;

      أرجع tmp;
}

فراغ vvp_fun_event_or_aa::recv_vec4(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
                                    vvp_context_t context)
{
      إذا (context) {
            waitable_state_s*state = static_cast<waitable_state_s*>
                  (vvp_get_context_توکی(context, context_idx_));

            پرلپسې_انتظار_تارونه(state->تارونه);
            بنسټ_net_2->send_vec4(بټ, context);
      } وإلا {
            context = context_scope_->ژوندی_contexts;
            طالما (context) {
                  recv_vec4(پورټ, بټ, context);
                  context = vvp_get_بل_context(context);
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
: vvp_named_event(h), تارونه_2(0)
{
}

vvp_named_event_sa::~vvp_named_event_sa()
{
}

vthread_t vvp_named_event_sa::جمع_کړه_انتظار_thread(vthread_t thread)
{
      vthread_t tmp = تارونه_2;
      تارونه_2 = thread;

      أرجع tmp;
}

فراغ vvp_named_event_sa::recv_vec4(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
                                   vvp_context_t)
{
      پرلپسې_انتظار_تارونه(تارونه_2);
      vvp_net_t*net = پورټ.ptr();
      net->send_vec4(بټ, 0);

      __vpiNamedEvent*obj = dynamic_cast<__vpiNamedEvent*>(handle_);
      أكد(obj);
      obj->پرلپسې_vpi_callbacks();
}

vvp_named_event_aa::vvp_named_event_aa(__vpiHandle*h)
: vvp_named_event(h)
{
      context_idx_ = vpip_جمع_کړه_توکی_منزل_context(هذا, vpip_peek_context_scope());
}

vvp_named_event_aa::~vvp_named_event_aa()
{
}

فراغ vvp_named_event_aa::alloc_instance(vvp_context_t context)
{
      vvp_سېټ_context_توکی(context, context_idx_, جديد waitable_state_s);
}

فراغ vvp_named_event_aa::reset_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_توکی(context, context_idx_));

      state->تارونه = 0;
}

#که_تعریف_شوی CHECK_WITH_VALGRIND
فراغ vvp_named_event_aa::free_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_توکی(context, context_idx_));
      ړنګول state;
}
#د_که_پای

vthread_t vvp_named_event_aa::جمع_کړه_انتظار_thread(vthread_t thread)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vthread_get_wt_context_توکی(context_idx_));

      vthread_t tmp = state->تارونه;
      state->تارونه = thread;

      أرجع tmp;
}

فراغ vvp_named_event_aa::recv_vec4(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
                                   vvp_context_t context)
{
      أكد(context);

      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_توکی(context, context_idx_));

      پرلپسې_انتظار_تارونه(state->تارونه);
      vvp_net_t*net = پورټ.ptr();
      net->send_vec4(بټ, context);
}

/*
**  Create an event functor
**  edge:  compile_event(label, type, argc, argv, debug_flag)
**  or:    compile_event(label, NULL, argc, argv, debug_flag)
**
**  Named events are handled elsewhere.
*/

ساكن فراغ compile_event_or(حرف*label, بینښه argc, بنية symb_s*argv);

فراغ compile_event(حرف*label, حرف*type, بینښه argc, بنية symb_s*argv)
{
      vvp_net_fun_t*fun = 0;

      إذا (type == 0) {
	    compile_event_or(label, argc, argv);
	    أرجع;
      }

      إذا (strcmp(type,"anyedge") == 0) {

	    حرر(type);

            إذا (vpip_peek_current_scope()->is_automatic()) {
                  fun = جديد vvp_fun_anyedge_aa;
            } وإلا {
                  fun = جديد vvp_fun_anyedge_sa;
            }

      } وإلا {

	    vvp_fun_څنډه::څنډه_t څنډه_type = vvp_څنډه_none;

	    إذا (strcmp(type,"posedge") == 0)
		  څنډه_type = vvp_څنډه_posedge;
	    وإلا إذا (strcmp(type,"negedge") == 0)
		  څنډه_type = vvp_څنډه_negedge;
	    وإلا إذا (strcmp(type,"edge") == 0)
		  څنډه_type = vvp_څنډه_څنډه;

	    أكد(argc <= 4);
	    حرر(type);

            إذا (vpip_peek_current_scope()->is_automatic()) {
                  fun = جديد vvp_fun_څنډه_aa(څنډه_type);
            } وإلا {
                  fun = جديد vvp_fun_څنډه_sa(څنډه_type);
            }

      }

      vvp_net_t* ptr = جديد vvp_net_t;
      ptr->fun = fun;

      define_functor_نښه(label, ptr);
      حرر(label);

      inputs_connect(ptr, argc, argv);
      حرر(argv);
}

ساكن فراغ compile_event_or(حرف*label, بینښه argc, بنية symb_s*argv)
{
      vvp_net_t*بنسټ_net = جديد vvp_net_t;
      إذا (vpip_peek_current_scope()->is_automatic()) {
            بنسټ_net->fun = جديد vvp_fun_event_or_aa(بنسټ_net);
      } وإلا {
            بنسټ_net->fun = جديد vvp_fun_event_or_sa(بنسټ_net);
      }
      define_functor_نښه(label, بنسټ_net);
      حرر(label);

	/* This is a simplified version of a wide functor. We don't
	   care about the data values or what port they arrived on,
	   so we can use a single shared functor. */
      vvp_net_t*curr_net = بنسټ_net;
      لكل (بینښه idx = 0 ;  idx < argc ;  idx += 1) {
	    إذا (idx > 0 && (idx % 4) == 0) {
		  curr_net = جديد vvp_net_t;
		  curr_net->fun = بنسټ_net->fun;
	    }
	    ننوت_connect(curr_net, idx % 4, argv[idx].متن);
      }
      حرر(argv);
}

/*
 * This handles the compile of named events. This functor has no
 * inputs, it is only accessed by behavioral trigger statements, which
 * in vvp are %set instructions.
 */
فراغ compile_named_event(حرف*label, حرف*name, منطقي local_flag)
{
      vvp_net_t*ptr = جديد vvp_net_t;

      vpiHandle obj = vpip_make_named_event(name, ptr);

      إذا (vpip_peek_current_scope()->is_automatic()) {
            ptr->fun = جديد vvp_named_event_aa(obj);
      } وإلا {
            ptr->fun = جديد vvp_named_event_sa(obj);
      }
      define_functor_نښه(label, ptr);
      compile_vpi_نښه(label, obj);
      إذا (! local_flag) vpip_attach_منزل_current_scope(obj);

      حرر(label);
      ړنګول[] name;
}

#که_تعریف_شوی CHECK_WITH_VALGRIND
فراغ named_event_delete(__vpiHandle*handle)
{
      ړنګول dynamic_cast<__vpiNamedEvent *>(handle);
}
#د_که_پای
