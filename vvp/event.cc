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

فراغ waitable_hooks_s::پیاپی_انتظار_نخها(vthread_t&نخها)
{
	// Run the non-blocking event controls.
      آخرین = &event_ctls;
      لكل (evctl*cur = event_ctls; cur != 0;) {
	    إذا (cur->dec_and_پیاپی()) {
		  evctl*nxt = cur->بعدی_2;
		  حذف cur;
		  cur = nxt;
		  *آخرین = cur;
	    } وإلا {
		  آخرین = &(cur->بعدی_2);
		  cur = cur->بعدی_2;
	    }
      }

      vthread_t tmp = نخها;
      إذا (tmp == 0) أرجع;
      نخها = 0;

      vthread_زمانبندی_list(tmp);
}

evctl::evctl(uint64_t ecount)
{
      ecount_ = ecount;
      بعدی_2 = 0;
}

منطقي evctl::dec_and_پیاپی()
{
      أكد(ecount_ != 0);

      ecount_ -= 1;
      إذا (ecount_ == 0) پیاپی_پیاپی();

      أرجع ecount_ == 0;
}

evctl_real::evctl_real(__vpiHandle*handle, مزدوج value,
                       uint64_t ecount)
:evctl(ecount)
{
      handle_ = handle;
      مقدار_2 = value;
}

فراغ evctl_real::پیاپی_پیاپی()
{
      t_vpi_مقدار val;

      val.format = vpiRealVal;
      val.value.real = مقدار_2;
      vpi_put_مقدار(handle_, &val, 0, vpiNoDelay);
}

فراغ زمانبندی_evctl(__vpiHandle*handle, مزدوج value,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      أكد(ep);
	// Now add this call to the end of the event list.
      *(ep->آخرین) = جديد evctl_real(handle, value, ecount);
      ep->آخرین = &((*(ep->آخرین))->بعدی_2);
}

evctl_وکتور::evctl_وکتور(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&value,
                           بیعلامت off, بیعلامت wid, uint64_t ecount)
:evctl(ecount), ptr_(ptr), مقدار_2(value)
{
      off_ = off;
      wid_ = wid;
}

فراغ evctl_وکتور::پیاپی_پیاپی()
{
      إذا (wid_ != 0) {
	    vvp_send_vec4_pv(ptr_, مقدار_2, off_, wid_, 0);
      } وإلا {
	    vvp_send_vec4(ptr_, مقدار_2, 0);
      }
}

فراغ زمانبندی_evctl(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&value,
                    بیعلامت جابه_جایی_2, بیعلامت wid,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      أكد(ep);
	// Now add this call to the end of the event list.
      *(ep->آخرین) = جديد evctl_وکتور(ptr, value, جابه_جایی_2, wid, ecount);
      ep->آخرین = &((*(ep->آخرین))->بعدی_2);
}

evctl_array::evctl_array(vvp_array_t حافظه, بیعلامت شاخص,
                         ثابت vvp_vector4_t&value, بیعلامت off,
                         uint64_t ecount)
:evctl(ecount), مقدار_2(value)
{
      mem_ = حافظه;
      idx_ = شاخص;
      off_ = off;
}

فراغ evctl_array::پیاپی_پیاپی()
{
      mem_->مجموعه_کلمه(idx_, off_, مقدار_2);
}

فراغ زمانبندی_evctl(vvp_array_t حافظه, بیعلامت شاخص,
                    ثابت vvp_vector4_t&value, بیعلامت جابه_جایی_2,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      أكد(ep);
	// Now add this call to the end of the event list.
      *(ep->آخرین) = جديد evctl_array(حافظه, شاخص, value, جابه_جایی_2, ecount);
      ep->آخرین = &((*(ep->آخرین))->بعدی_2);
}

evctl_array_r::evctl_array_r(vvp_array_t حافظه, بیعلامت شاخص,
                             مزدوج value, uint64_t ecount)
:evctl(ecount)
{
      mem_ = حافظه;
      idx_ = شاخص;
      مقدار_2 = value;
}

فراغ evctl_array_r::پیاپی_پیاپی()
{
      mem_->مجموعه_کلمه(idx_, مقدار_2);
}

فراغ زمانبندی_evctl(vvp_array_t حافظه, بیعلامت شاخص,
                    مزدوج value,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      أكد(ep);
	// Now add this call to the end of the event list.
      *(ep->آخرین) = جديد evctl_array_r(حافظه, شاخص, value, ecount);
      ep->آخرین = &((*(ep->آخرین))->بعدی_2);
}

مضمن vvp_fun_یال::یال_t VVP_یال(vvp_bit4_t from, vvp_bit4_t to)
{
      أرجع 1 << ((from << 2) | to);
}

ثابت vvp_fun_یال::یال_t vvp_یال_posedge
      = VVP_یال(BIT4_0,BIT4_1)
      | VVP_یال(BIT4_0,BIT4_X)
      | VVP_یال(BIT4_0,BIT4_Z)
      | VVP_یال(BIT4_X,BIT4_1)
      | VVP_یال(BIT4_Z,BIT4_1)
      ;

ثابت vvp_fun_یال::یال_t vvp_یال_negedge
      = VVP_یال(BIT4_1,BIT4_0)
      | VVP_یال(BIT4_1,BIT4_X)
      | VVP_یال(BIT4_1,BIT4_Z)
      | VVP_یال(BIT4_X,BIT4_0)
      | VVP_یال(BIT4_Z,BIT4_0)
      ;

ثابت vvp_fun_یال::یال_t vvp_یال_یال
      = VVP_یال(BIT4_0,BIT4_1)
      | VVP_یال(BIT4_1,BIT4_0)
      | VVP_یال(BIT4_0,BIT4_X)
      | VVP_یال(BIT4_X,BIT4_0)
      | VVP_یال(BIT4_0,BIT4_Z)
      | VVP_یال(BIT4_Z,BIT4_0)
      | VVP_یال(BIT4_X,BIT4_1)
      | VVP_یال(BIT4_1,BIT4_X)
      | VVP_یال(BIT4_Z,BIT4_1)
      | VVP_یال(BIT4_1,BIT4_Z)
      ;

ثابت vvp_fun_یال::یال_t vvp_یال_none    = 0;

بنية vvp_fun_یال_state_s : عام waitable_state_s {
      vvp_fun_یال_state_s()
      {
            لكل (بیعلامت idx = 0 ;  idx < 4 ;  idx += 1)
                  بیتها[idx] = BIT4_X;
      }

      vvp_bit4_t بیتها[4];
};

vvp_fun_یال::vvp_fun_یال(یال_t e)
: یال_2(e)
{
      لكل (بیعلامت idx = 0 ;  idx < 4 ;  idx += 1)
            بیتها_2[idx] = BIT4_X;
}

vvp_fun_یال::~vvp_fun_یال()
{
}

منطقي vvp_fun_یال::recv_vec4_(ثابت vvp_vector4_t&بیت,
                              vvp_bit4_t&old_بیت, vthread_t&نخها)
{
	/* See what kind of edge this represents. */
      یال_t ماسک = VVP_یال(old_بیت, بیت.value(0));

	/* Save the current input for the next time around. */
      old_بیت = بیت.value(0);

      إذا ((یال_2 == vvp_یال_none) || (یال_2 & ماسک)) {
	    پیاپی_انتظار_نخها(نخها);
            أرجع درست;
      }
      أرجع خطأ;
}

vvp_fun_یال_sa::vvp_fun_یال_sa(یال_t e)
: vvp_fun_یال(e), نخها_2(0)
{
}

vvp_fun_یال_sa::~vvp_fun_یال_sa()
{
}

vthread_t vvp_fun_یال_sa::جمع_کن_انتظار_thread(vthread_t thread)
{
      vthread_t tmp = نخها_2;
      نخها_2 = thread;

      أرجع tmp;
}

فراغ vvp_fun_یال_sa::recv_vec4(vvp_net_ptr_t پورت, ثابت vvp_vector4_t&بیت,
                                vvp_context_t)
{
      إذا (recv_vec4_(بیت, بیتها_2[پورت.پورت()], نخها_2)) {
	    vvp_net_t*net = پورت.ptr();
	    net->send_vec4(بیت, 0);
      }
}

فراغ vvp_fun_یال_sa::recv_vec4_pv(vvp_net_ptr_t پورت, ثابت vvp_vector4_t&بیت,
				   بیعلامت پایه, بیعلامت vwid, vvp_context_t)
{
      أكد(پایه == 0);
      إذا (recv_vec4_(بیت, بیتها_2[پورت.پورت()], نخها_2)) {
	    vvp_net_t*net = پورت.ptr();
	    net->send_vec4_pv(بیت, پایه, vwid, 0);
      }
}

vvp_fun_یال_aa::vvp_fun_یال_aa(یال_t e)
: vvp_fun_یال(e)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_جمع_کن_قلم_مقصد_context(هذا, context_scope_);
}

vvp_fun_یال_aa::~vvp_fun_یال_aa()
{
}

فراغ vvp_fun_یال_aa::alloc_instance(vvp_context_t context)
{
      vvp_مجموعه_context_قلم(context, context_idx_, جديد vvp_fun_یال_state_s);
      reset_instance(context);
}

فراغ vvp_fun_یال_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_یال_state_s*state = static_cast<vvp_fun_یال_state_s*>
            (vvp_get_context_قلم(context, context_idx_));

      state->نخها = 0;
      لكل (بیعلامت idx = 0 ;  idx < 4 ;  idx += 1)
            state->بیتها[idx] = بیتها_2[idx];
}

#اگر_تعریف_شده CHECK_WITH_VALGRIND
فراغ vvp_fun_یال_aa::free_instance(vvp_context_t context)
{
      vvp_fun_یال_state_s*state = static_cast<vvp_fun_یال_state_s*>
            (vvp_get_context_قلم(context, context_idx_));
      حذف state;
}
#پایان_اگر

vthread_t vvp_fun_یال_aa::جمع_کن_انتظار_thread(vthread_t thread)
{
      vvp_fun_یال_state_s*state = static_cast<vvp_fun_یال_state_s*>
            (vthread_get_wt_context_قلم(context_idx_));

      vthread_t tmp = state->نخها;
      state->نخها = thread;

      أرجع tmp;
}

فراغ vvp_fun_یال_aa::recv_vec4(vvp_net_ptr_t پورت, ثابت vvp_vector4_t&بیت,
                                vvp_context_t context)
{
      إذا (context) {
            vvp_fun_یال_state_s*state = static_cast<vvp_fun_یال_state_s*>
                  (vvp_get_context_قلم(context, context_idx_));

            إذا (recv_vec4_(بیت, state->بیتها[پورت.پورت()], state->نخها)) {
                  vvp_net_t*net = پورت.ptr();
                  net->send_vec4(بیت, context);
            }
      } وإلا {
            context = context_scope_->زنده_contexts;
            طالما (context) {
                  recv_vec4(پورت, بیت, context);
                  context = vvp_get_بعدی_context(context);
            }
            بیتها_2[پورت.پورت()] = بیت.value(0);
      }
}

صنف anyedge_مقدار {

    عام:
      anyedge_مقدار() {};
      مجازی ~anyedge_مقدار() {};

      مجازی فراغ reset() = 0;

      مجازی فراغ duplicate(anyedge_مقدار*&dup) = 0;
};

صنف anyedge_vec4_مقدار : عام anyedge_مقدار {

    عام:
      anyedge_vec4_مقدار() {};
      مجازی ~anyedge_vec4_مقدار() override {};

      فراغ reset() override { old_بیتها.مجموعه_مقصد_x(); }

      فراغ مجموعه(ثابت vvp_vector4_t&بیت) { old_بیتها = بیت; };

      فراغ duplicate(anyedge_مقدار*&dup) override;

      منطقي recv_vec4(ثابت vvp_vector4_t&بیت);

      منطقي recv_vec4_pv(ثابت vvp_vector4_t&بیت, بیعلامت پایه,
			بیعلامت vwid);

    خاص:
      vvp_vector4_t old_بیتها;
};

ساكن anyedge_vec4_مقدار*get_vec4_مقدار(anyedge_مقدار*&value)
{
      anyedge_vec4_مقدار*vec4_مقدار = dynamic_cast<anyedge_vec4_مقدار*>(value);
      إذا (!value) {
	    vec4_مقدار = جديد anyedge_vec4_مقدار();
	    حذف value;
	    value = vec4_مقدار;
      }
      أرجع vec4_مقدار;
}

صنف anyedge_real_مقدار : عام anyedge_مقدار {

    عام:
      anyedge_real_مقدار() : old_بیتها(0.0) {};
      مجازی ~anyedge_real_مقدار() override {};

      فراغ reset() override { old_بیتها = 0.0; }

      فراغ مجموعه(مزدوج بیت) { old_بیتها = بیت; };

      فراغ duplicate(anyedge_مقدار*&dup) override;

      منطقي recv_real(مزدوج بیت);

    خاص:
      مزدوج old_بیتها;
};

ساكن anyedge_real_مقدار*get_real_مقدار(anyedge_مقدار*&value)
{
      anyedge_real_مقدار*real_مقدار = dynamic_cast<anyedge_real_مقدار*>(value);
      إذا (!value) {
	    real_مقدار = جديد anyedge_real_مقدار();
	    حذف value;
	    value = real_مقدار;
      }
      أرجع real_مقدار;
}

صنف anyedge_string_مقدار : عام anyedge_مقدار {

    عام:
      anyedge_string_مقدار() {};
      مجازی ~anyedge_string_مقدار() override {};

      فراغ reset() override { old_بیتها.clear(); }

      فراغ مجموعه(ثابت std::string&بیت) { old_بیتها = بیت; };

      فراغ duplicate(anyedge_مقدار*&dup) override;

      منطقي recv_string(ثابت std::string&بیت);

    خاص:
      std::string old_بیتها;
};

ساكن anyedge_string_مقدار*get_string_مقدار(anyedge_مقدار*&value)
{
      anyedge_string_مقدار*string_مقدار = dynamic_cast<anyedge_string_مقدار*>(value);
      إذا (!value) {
	    string_مقدار = جديد anyedge_string_مقدار();
	    حذف value;
	    value = string_مقدار;
      }
      أرجع string_مقدار;
}

بنية vvp_fun_anyedge_state_s : عام waitable_state_s {
      vvp_fun_anyedge_state_s()
      {
            لكل (بیعلامت idx = 0 ;  idx < 4 ;  idx += 1)
                  آخرین_مقدار[idx] = 0;
      }

      ~vvp_fun_anyedge_state_s()
      {
            لكل (بیعلامت idx = 0 ;  idx < 4 ;  idx += 1)
                  حذف آخرین_مقدار[idx];
      }

      anyedge_مقدار *آخرین_مقدار[4];
      vvp_fun_anyedge_state_s(ثابت vvp_fun_anyedge_state_s&) = حذف;
      vvp_fun_anyedge_state_s& عملگر=(ثابت vvp_fun_anyedge_state_s&) = حذف;
};

vvp_fun_anyedge::vvp_fun_anyedge()
{
      لكل (بیعلامت idx = 0 ;  idx < 4 ;  idx += 1)
	    آخرین_مقدار[idx] = 0;
}

vvp_fun_anyedge::~vvp_fun_anyedge()
{
      لكل (بیعلامت idx = 0 ;  idx < 4 ;  idx += 1)
	    حذف آخرین_مقدار[idx];
}

فراغ anyedge_vec4_مقدار::duplicate(anyedge_مقدار*&dup)
{
      anyedge_vec4_مقدار*dup_vec4 = get_vec4_مقدار(dup);
      أكد(dup_vec4);
      dup_vec4->مجموعه(old_بیتها);
}

منطقي anyedge_vec4_مقدار::recv_vec4(ثابت vvp_vector4_t&بیت)
{
      منطقي flag = خطأ;

      إذا (old_بیتها.size() != بیت.size()) {
	    إذا (old_بیتها.size() == 0) {
		    // Special case: If we've not seen any input yet
		    // (old_bits.size()==0) then replace it will a reference
		    // vector that is 'bx. Then compare that with the input
		    // to see if we are processing a change from 'bx.
		  old_بیتها = vvp_vector4_t(بیت.size(), BIT4_X);
		  إذا (old_بیتها.eeq(بیت))
			flag = خطأ;
		  وإلا
			flag = درست;

	    } وإلا {
		  flag = درست;
	    }

      } وإلا {
	    لكل (بیعلامت idx = 0 ;  idx < بیت.size() ;  idx += 1) {
		  إذا (old_بیتها.value(idx) != بیت.value(idx)) {
			flag = درست;
			اكسر;
		  }
	    }
      }

      إذا (flag) {
	    old_بیتها = بیت;
      }

      أرجع flag;
}

منطقي anyedge_vec4_مقدار::recv_vec4_pv(ثابت vvp_vector4_t&بیت, بیعلامت پایه,
				      بیعلامت vwid)
{
      vvp_vector4_t tmp = old_بیتها;
      إذا (tmp.size() == 0)
	    tmp = vvp_vector4_t(vwid, BIT4_Z);
      أكد(پایه + بیت.size()<= vwid);
      أكد(tmp.size() == vwid);
      tmp.مجموعه_vec(پایه, بیت);

      أرجع recv_vec4(tmp);
}

فراغ anyedge_real_مقدار::duplicate(anyedge_مقدار*&dup)
{
      anyedge_real_مقدار*dup_real = get_real_مقدار(dup);
      أكد(dup_real);
      dup_real->مجموعه(old_بیتها);
}

منطقي anyedge_real_مقدار::recv_real(مزدوج بیت)
{
      إذا (old_بیتها != بیت) {
	    old_بیتها = بیت;
            أرجع درست;
      }
      أرجع خطأ;
}

فراغ anyedge_string_مقدار::duplicate(anyedge_مقدار*&dup)
{
      anyedge_string_مقدار*dup_string = get_string_مقدار(dup);
      أكد(dup_string);
      dup_string->مجموعه(old_بیتها);
}

منطقي anyedge_string_مقدار::recv_string(ثابت std::string&بیت)
{
      إذا (old_بیتها != بیت) {
	    old_بیتها = بیت;
            أرجع درست;
      }
      أرجع خطأ;
}

vvp_fun_anyedge_sa::vvp_fun_anyedge_sa()
: نخها_2(0)
{
}

vvp_fun_anyedge_sa::~vvp_fun_anyedge_sa()
{
}

vthread_t vvp_fun_anyedge_sa::جمع_کن_انتظار_thread(vthread_t thread)
{
      vthread_t tmp = نخها_2;
      نخها_2 = thread;

      أرجع tmp;
}

فراغ vvp_fun_anyedge_sa::recv_vec4(vvp_net_ptr_t پورت, ثابت vvp_vector4_t&بیت,
                                   vvp_context_t)
{
      anyedge_vec4_مقدار*value = get_vec4_مقدار(آخرین_مقدار[پورت.پورت()]);
      أكد(value);
      إذا (value->recv_vec4(بیت)) {
	    پیاپی_انتظار_نخها(نخها_2);
	    vvp_net_t*net = پورت.ptr();
	    net->send_vec4(بیت, 0);
      }
}

فراغ vvp_fun_anyedge_sa::recv_vec4_pv(vvp_net_ptr_t پورت, ثابت vvp_vector4_t&بیت,
				      بیعلامت پایه, بیعلامت vwid, vvp_context_t)
{
      anyedge_vec4_مقدار*value = get_vec4_مقدار(آخرین_مقدار[پورت.پورت()]);
      أكد(value);
      إذا (value->recv_vec4_pv(بیت, پایه, vwid)) {
	    پیاپی_انتظار_نخها(نخها_2);
	    vvp_net_t*net = پورت.ptr();
	    net->send_vec4(بیت, 0);
      }
}

فراغ vvp_fun_anyedge_sa::recv_real(vvp_net_ptr_t پورت, مزدوج بیت,
                                   vvp_context_t)
{
      anyedge_real_مقدار*value = get_real_مقدار(آخرین_مقدار[پورت.پورت()]);
      أكد(value);
      إذا (value->recv_real(بیت)) {
	    پیاپی_انتظار_نخها(نخها_2);
	    vvp_net_t*net = پورت.ptr();
	    net->send_vec4(vvp_vector4_t(), 0);
      }
}

فراغ vvp_fun_anyedge_sa::recv_string(vvp_net_ptr_t پورت, ثابت std::string&بیت,
				     vvp_context_t)
{
      anyedge_string_مقدار*value = get_string_مقدار(آخرین_مقدار[پورت.پورت()]);
      أكد(value);
      إذا (value->recv_string(بیت)) {
	    پیاپی_انتظار_نخها(نخها_2);
	    vvp_net_t*net = پورت.ptr();
	    net->send_vec4(vvp_vector4_t(), 0);
      }
}

/*
 * An anyedge receiving an object should do nothing with it, but should
 * trigger waiting threads.
 */
فراغ vvp_fun_anyedge_sa::recv_object(vvp_net_ptr_t پورت, vvp_object_t,
				     vvp_context_t)
{
      پیاپی_انتظار_نخها(نخها_2);
      vvp_net_t*net = پورت.ptr();
      net->send_vec4(vvp_vector4_t(), 0);
}

vvp_fun_anyedge_aa::vvp_fun_anyedge_aa()
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_جمع_کن_قلم_مقصد_context(هذا, context_scope_);
}

vvp_fun_anyedge_aa::~vvp_fun_anyedge_aa()
{
}

فراغ vvp_fun_anyedge_aa::alloc_instance(vvp_context_t context)
{
      vvp_مجموعه_context_قلم(context, context_idx_, جديد vvp_fun_anyedge_state_s);
      reset_instance(context);
}

فراغ vvp_fun_anyedge_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
            (vvp_get_context_قلم(context, context_idx_));

      state->نخها = 0;
      لكل (بیعلامت idx = 0 ;  idx < 4 ;  idx += 1) {
	    إذا (آخرین_مقدار[idx])
	          آخرین_مقدار[idx]->duplicate(state->آخرین_مقدار[idx]);
	    وإلا إذا (state->آخرین_مقدار[idx])
		  state->آخرین_مقدار[idx]->reset();
      }
}

#اگر_تعریف_شده CHECK_WITH_VALGRIND
فراغ vvp_fun_anyedge_aa::free_instance(vvp_context_t context)
{
      vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
            (vvp_get_context_قلم(context, context_idx_));
      حذف state;
}
#پایان_اگر

vthread_t vvp_fun_anyedge_aa::جمع_کن_انتظار_thread(vthread_t thread)
{
      vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
            (vthread_get_wt_context_قلم(context_idx_));

      vthread_t tmp = state->نخها;
      state->نخها = thread;

      أرجع tmp;
}

فراغ vvp_fun_anyedge_aa::recv_vec4(vvp_net_ptr_t پورت, ثابت vvp_vector4_t&بیت,
                                   vvp_context_t context)
{
      إذا (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_قلم(context, context_idx_));

            anyedge_vec4_مقدار*value = get_vec4_مقدار(state->آخرین_مقدار[پورت.پورت()]);
            أكد(value);
            إذا (value->recv_vec4(بیت)) {
                  پیاپی_انتظار_نخها(state->نخها);
                  vvp_net_t*net = پورت.ptr();
                  net->send_vec4(بیت, context);
            }
      } وإلا {
            context = context_scope_->زنده_contexts;
            طالما (context) {
                  recv_vec4(پورت, بیت, context);
                  context = vvp_get_بعدی_context(context);
            }
            anyedge_vec4_مقدار*value = get_vec4_مقدار(آخرین_مقدار[پورت.پورت()]);
            أكد(value);
            value->مجموعه(بیت);
      }
}

فراغ vvp_fun_anyedge_aa::recv_real(vvp_net_ptr_t پورت, مزدوج بیت,
                                   vvp_context_t context)
{
      إذا (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_قلم(context, context_idx_));

            anyedge_real_مقدار*value = get_real_مقدار(state->آخرین_مقدار[پورت.پورت()]);
            أكد(value);
            إذا (value->recv_real(بیت)) {
                  پیاپی_انتظار_نخها(state->نخها);
                  vvp_net_t*net = پورت.ptr();
                  net->send_vec4(vvp_vector4_t(), context);
            }
      } وإلا {
            context = context_scope_->زنده_contexts;
            طالما (context) {
                  recv_real(پورت, بیت, context);
                  context = vvp_get_بعدی_context(context);
            }
            anyedge_real_مقدار*value = get_real_مقدار(آخرین_مقدار[پورت.پورت()]);
            أكد(value);
            value->مجموعه(بیت);
      }
}

فراغ vvp_fun_anyedge_aa::recv_string(vvp_net_ptr_t پورت, ثابت std::string&بیت,
				     vvp_context_t context)
{
      إذا (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_قلم(context, context_idx_));

            anyedge_string_مقدار*value = get_string_مقدار(state->آخرین_مقدار[پورت.پورت()]);
            أكد(value);
            إذا (value->recv_string(بیت)) {
                  پیاپی_انتظار_نخها(state->نخها);
                  vvp_net_t*net = پورت.ptr();
                  net->send_vec4(vvp_vector4_t(), context);
            }
      } وإلا {
            context = context_scope_->زنده_contexts;
            طالما (context) {
                  recv_string(پورت, بیت, context);
                  context = vvp_get_بعدی_context(context);
            }
            anyedge_string_مقدار*value = get_string_مقدار(آخرین_مقدار[پورت.پورت()]);
            أكد(value);
            value->مجموعه(بیت);
      }
}

vvp_fun_event_or::vvp_fun_event_or(vvp_net_t*پایه_net)
: پایه_net_2(پایه_net)
{
}

vvp_fun_event_or::~vvp_fun_event_or()
{
}

vvp_fun_event_or_sa::vvp_fun_event_or_sa(vvp_net_t*پایه_net)
: vvp_fun_event_or(پایه_net), نخها_2(0)
{
}

vvp_fun_event_or_sa::~vvp_fun_event_or_sa()
{
}

vthread_t vvp_fun_event_or_sa::جمع_کن_انتظار_thread(vthread_t thread)
{
      vthread_t tmp = نخها_2;
      نخها_2 = thread;

      أرجع tmp;
}

فراغ vvp_fun_event_or_sa::recv_vec4(vvp_net_ptr_t, ثابت vvp_vector4_t&بیت,
                                    vvp_context_t)
{
      پیاپی_انتظار_نخها(نخها_2);
      پایه_net_2->send_vec4(بیت, 0);
}

vvp_fun_event_or_aa::vvp_fun_event_or_aa(vvp_net_t*پایه_net)
: vvp_fun_event_or(پایه_net)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_جمع_کن_قلم_مقصد_context(هذا, context_scope_);
}

vvp_fun_event_or_aa::~vvp_fun_event_or_aa()
{
}

فراغ vvp_fun_event_or_aa::alloc_instance(vvp_context_t context)
{
      vvp_مجموعه_context_قلم(context, context_idx_, جديد waitable_state_s);
}

فراغ vvp_fun_event_or_aa::reset_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_قلم(context, context_idx_));

      state->نخها = 0;
}

#اگر_تعریف_شده CHECK_WITH_VALGRIND
فراغ vvp_fun_event_or_aa::free_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_قلم(context, context_idx_));
      حذف state;
}
#پایان_اگر

vthread_t vvp_fun_event_or_aa::جمع_کن_انتظار_thread(vthread_t thread)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vthread_get_wt_context_قلم(context_idx_));

      vthread_t tmp = state->نخها;
      state->نخها = thread;

      أرجع tmp;
}

فراغ vvp_fun_event_or_aa::recv_vec4(vvp_net_ptr_t پورت, ثابت vvp_vector4_t&بیت,
                                    vvp_context_t context)
{
      إذا (context) {
            waitable_state_s*state = static_cast<waitable_state_s*>
                  (vvp_get_context_قلم(context, context_idx_));

            پیاپی_انتظار_نخها(state->نخها);
            پایه_net_2->send_vec4(بیت, context);
      } وإلا {
            context = context_scope_->زنده_contexts;
            طالما (context) {
                  recv_vec4(پورت, بیت, context);
                  context = vvp_get_بعدی_context(context);
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
: vvp_named_event(h), نخها_2(0)
{
}

vvp_named_event_sa::~vvp_named_event_sa()
{
}

vthread_t vvp_named_event_sa::جمع_کن_انتظار_thread(vthread_t thread)
{
      vthread_t tmp = نخها_2;
      نخها_2 = thread;

      أرجع tmp;
}

فراغ vvp_named_event_sa::recv_vec4(vvp_net_ptr_t پورت, ثابت vvp_vector4_t&بیت,
                                   vvp_context_t)
{
      پیاپی_انتظار_نخها(نخها_2);
      vvp_net_t*net = پورت.ptr();
      net->send_vec4(بیت, 0);

      __vpiNamedEvent*obj = dynamic_cast<__vpiNamedEvent*>(handle_);
      أكد(obj);
      obj->پیاپی_vpi_callbacks();
}

vvp_named_event_aa::vvp_named_event_aa(__vpiHandle*h)
: vvp_named_event(h)
{
      context_idx_ = vpip_جمع_کن_قلم_مقصد_context(هذا, vpip_peek_context_scope());
}

vvp_named_event_aa::~vvp_named_event_aa()
{
}

فراغ vvp_named_event_aa::alloc_instance(vvp_context_t context)
{
      vvp_مجموعه_context_قلم(context, context_idx_, جديد waitable_state_s);
}

فراغ vvp_named_event_aa::reset_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_قلم(context, context_idx_));

      state->نخها = 0;
}

#اگر_تعریف_شده CHECK_WITH_VALGRIND
فراغ vvp_named_event_aa::free_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_قلم(context, context_idx_));
      حذف state;
}
#پایان_اگر

vthread_t vvp_named_event_aa::جمع_کن_انتظار_thread(vthread_t thread)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vthread_get_wt_context_قلم(context_idx_));

      vthread_t tmp = state->نخها;
      state->نخها = thread;

      أرجع tmp;
}

فراغ vvp_named_event_aa::recv_vec4(vvp_net_ptr_t پورت, ثابت vvp_vector4_t&بیت,
                                   vvp_context_t context)
{
      أكد(context);

      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_قلم(context, context_idx_));

      پیاپی_انتظار_نخها(state->نخها);
      vvp_net_t*net = پورت.ptr();
      net->send_vec4(بیت, context);
}

/*
**  Create an event functor
**  edge:  compile_event(label, type, argc, argv, debug_flag)
**  or:    compile_event(label, NULL, argc, argv, debug_flag)
**
**  Named events are handled elsewhere.
*/

ساكن فراغ compile_event_or(حرف*label, بیعلامت argc, بنية symb_s*argv);

فراغ compile_event(حرف*label, حرف*type, بیعلامت argc, بنية symb_s*argv)
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

	    vvp_fun_یال::یال_t یال_type = vvp_یال_none;

	    إذا (strcmp(type,"posedge") == 0)
		  یال_type = vvp_یال_posedge;
	    وإلا إذا (strcmp(type,"negedge") == 0)
		  یال_type = vvp_یال_negedge;
	    وإلا إذا (strcmp(type,"edge") == 0)
		  یال_type = vvp_یال_یال;

	    أكد(argc <= 4);
	    حرر(type);

            إذا (vpip_peek_current_scope()->is_automatic()) {
                  fun = جديد vvp_fun_یال_aa(یال_type);
            } وإلا {
                  fun = جديد vvp_fun_یال_sa(یال_type);
            }

      }

      vvp_net_t* ptr = جديد vvp_net_t;
      ptr->fun = fun;

      define_functor_نماد(label, ptr);
      حرر(label);

      inputs_connect(ptr, argc, argv);
      حرر(argv);
}

ساكن فراغ compile_event_or(حرف*label, بیعلامت argc, بنية symb_s*argv)
{
      vvp_net_t*پایه_net = جديد vvp_net_t;
      إذا (vpip_peek_current_scope()->is_automatic()) {
            پایه_net->fun = جديد vvp_fun_event_or_aa(پایه_net);
      } وإلا {
            پایه_net->fun = جديد vvp_fun_event_or_sa(پایه_net);
      }
      define_functor_نماد(label, پایه_net);
      حرر(label);

	/* This is a simplified version of a wide functor. We don't
	   care about the data values or what port they arrived on,
	   so we can use a single shared functor. */
      vvp_net_t*curr_net = پایه_net;
      لكل (بیعلامت idx = 0 ;  idx < argc ;  idx += 1) {
	    إذا (idx > 0 && (idx % 4) == 0) {
		  curr_net = جديد vvp_net_t;
		  curr_net->fun = پایه_net->fun;
	    }
	    درون_داد_connect(curr_net, idx % 4, argv[idx].متن);
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
      define_functor_نماد(label, ptr);
      compile_vpi_نماد(label, obj);
      إذا (! local_flag) vpip_attach_مقصد_current_scope(obj);

      حرر(label);
      حذف[] name;
}

#اگر_تعریف_شده CHECK_WITH_VALGRIND
فراغ named_event_delete(__vpiHandle*handle)
{
      حذف dynamic_cast<__vpiNamedEvent *>(handle);
}
#پایان_اگر
