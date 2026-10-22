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

فراغ waitable_hooks_s::لاڳيتو_انتظار_ڌاڳا(vthread_t&ڌاڳا)
{
	// Run the non-blocking event controls.
      آخري = &event_ctls;
      لكل (evctl*cur = event_ctls; cur != 0;) {
	    إذا (cur->dec_and_لاڳيتو()) {
		  evctl*nxt = cur->ايندڙ_2;
		  ميٽيو cur;
		  cur = nxt;
		  *آخري = cur;
	    } وإلا {
		  آخري = &(cur->ايندڙ_2);
		  cur = cur->ايندڙ_2;
	    }
      }

      vthread_t tmp = ڌاڳا;
      إذا (tmp == 0) أرجع;
      ڌاڳا = 0;

      vthread_وقت_جدول_list(tmp);
}

evctl::evctl(uint64_t ecount)
{
      ecount_ = ecount;
      ايندڙ_2 = 0;
}

منطقي evctl::dec_and_لاڳيتو()
{
      أكد(ecount_ != 0);

      ecount_ -= 1;
      إذا (ecount_ == 0) لاڳيتو_لاڳيتو();

      أرجع ecount_ == 0;
}

evctl_real::evctl_real(__vpiHandle*handle, مزدوج value,
                       uint64_t ecount)
:evctl(ecount)
{
      handle_ = handle;
      قدر_2 = value;
}

فراغ evctl_real::لاڳيتو_لاڳيتو()
{
      t_vpi_قدر val;

      val.format = vpiRealVal;
      val.value.real = قدر_2;
      vpi_put_قدر(handle_, &val, 0, vpiNoDelay);
}

فراغ وقت_جدول_evctl(__vpiHandle*handle, مزدوج value,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      أكد(ep);
	// Now add this call to the end of the event list.
      *(ep->آخري) = جديد evctl_real(handle, value, ecount);
      ep->آخري = &((*(ep->آخري))->ايندڙ_2);
}

evctl_ويڪٽر::evctl_ويڪٽر(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&value,
                           بنانشان off, بنانشان wid, uint64_t ecount)
:evctl(ecount), ptr_(ptr), قدر_2(value)
{
      off_ = off;
      wid_ = wid;
}

فراغ evctl_ويڪٽر::لاڳيتو_لاڳيتو()
{
      إذا (wid_ != 0) {
	    vvp_send_vec4_pv(ptr_, قدر_2, off_, wid_, 0);
      } وإلا {
	    vvp_send_vec4(ptr_, قدر_2, 0);
      }
}

فراغ وقت_جدول_evctl(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&value,
                    بنانشان وٿي_2, بنانشان wid,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      أكد(ep);
	// Now add this call to the end of the event list.
      *(ep->آخري) = جديد evctl_ويڪٽر(ptr, value, وٿي_2, wid, ecount);
      ep->آخري = &((*(ep->آخري))->ايندڙ_2);
}

evctl_array::evctl_array(vvp_array_t يادگيري, بنانشان اشاريو,
                         ثابت vvp_vector4_t&value, بنانشان off,
                         uint64_t ecount)
:evctl(ecount), قدر_2(value)
{
      mem_ = يادگيري;
      idx_ = اشاريو;
      off_ = off;
}

فراغ evctl_array::لاڳيتو_لاڳيتو()
{
      mem_->سيٽ_لفظ(idx_, off_, قدر_2);
}

فراغ وقت_جدول_evctl(vvp_array_t يادگيري, بنانشان اشاريو,
                    ثابت vvp_vector4_t&value, بنانشان وٿي_2,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      أكد(ep);
	// Now add this call to the end of the event list.
      *(ep->آخري) = جديد evctl_array(يادگيري, اشاريو, value, وٿي_2, ecount);
      ep->آخري = &((*(ep->آخري))->ايندڙ_2);
}

evctl_array_r::evctl_array_r(vvp_array_t يادگيري, بنانشان اشاريو,
                             مزدوج value, uint64_t ecount)
:evctl(ecount)
{
      mem_ = يادگيري;
      idx_ = اشاريو;
      قدر_2 = value;
}

فراغ evctl_array_r::لاڳيتو_لاڳيتو()
{
      mem_->سيٽ_لفظ(idx_, قدر_2);
}

فراغ وقت_جدول_evctl(vvp_array_t يادگيري, بنانشان اشاريو,
                    مزدوج value,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      أكد(ep);
	// Now add this call to the end of the event list.
      *(ep->آخري) = جديد evctl_array_r(يادگيري, اشاريو, value, ecount);
      ep->آخري = &((*(ep->آخري))->ايندڙ_2);
}

مضمن vvp_fun_ڪنارو::ڪنارو_t VVP_ڪنارو(vvp_bit4_t from, vvp_bit4_t to)
{
      أرجع 1 << ((from << 2) | to);
}

ثابت vvp_fun_ڪنارو::ڪنارو_t vvp_ڪنارو_posedge
      = VVP_ڪنارو(BIT4_0,BIT4_1)
      | VVP_ڪنارو(BIT4_0,BIT4_X)
      | VVP_ڪنارو(BIT4_0,BIT4_Z)
      | VVP_ڪنارو(BIT4_X,BIT4_1)
      | VVP_ڪنارو(BIT4_Z,BIT4_1)
      ;

ثابت vvp_fun_ڪنارو::ڪنارو_t vvp_ڪنارو_negedge
      = VVP_ڪنارو(BIT4_1,BIT4_0)
      | VVP_ڪنارو(BIT4_1,BIT4_X)
      | VVP_ڪنارو(BIT4_1,BIT4_Z)
      | VVP_ڪنارو(BIT4_X,BIT4_0)
      | VVP_ڪنارو(BIT4_Z,BIT4_0)
      ;

ثابت vvp_fun_ڪنارو::ڪنارو_t vvp_ڪنارو_ڪنارو
      = VVP_ڪنارو(BIT4_0,BIT4_1)
      | VVP_ڪنارو(BIT4_1,BIT4_0)
      | VVP_ڪنارو(BIT4_0,BIT4_X)
      | VVP_ڪنارو(BIT4_X,BIT4_0)
      | VVP_ڪنارو(BIT4_0,BIT4_Z)
      | VVP_ڪنارو(BIT4_Z,BIT4_0)
      | VVP_ڪنارو(BIT4_X,BIT4_1)
      | VVP_ڪنارو(BIT4_1,BIT4_X)
      | VVP_ڪنارو(BIT4_Z,BIT4_1)
      | VVP_ڪنارو(BIT4_1,BIT4_Z)
      ;

ثابت vvp_fun_ڪنارو::ڪنارو_t vvp_ڪنارو_none    = 0;

بنية vvp_fun_ڪنارو_state_s : عام waitable_state_s {
      vvp_fun_ڪنارو_state_s()
      {
            لكل (بنانشان idx = 0 ;  idx < 4 ;  idx += 1)
                  بٽ[idx] = BIT4_X;
      }

      vvp_bit4_t بٽ[4];
};

vvp_fun_ڪنارو::vvp_fun_ڪنارو(ڪنارو_t e)
: ڪنارو_2(e)
{
      لكل (بنانشان idx = 0 ;  idx < 4 ;  idx += 1)
            بٽ_2[idx] = BIT4_X;
}

vvp_fun_ڪنارو::~vvp_fun_ڪنارو()
{
}

منطقي vvp_fun_ڪنارو::recv_vec4_(ثابت vvp_vector4_t&ٻٽو_انگ,
                              vvp_bit4_t&old_ٻٽو_انگ, vthread_t&ڌاڳا)
{
	/* See what kind of edge this represents. */
      ڪنارو_t نقاب = VVP_ڪنارو(old_ٻٽو_انگ, ٻٽو_انگ.value(0));

	/* Save the current input for the next time around. */
      old_ٻٽو_انگ = ٻٽو_انگ.value(0);

      إذا ((ڪنارو_2 == vvp_ڪنارو_none) || (ڪنارو_2 & نقاب)) {
	    لاڳيتو_انتظار_ڌاڳا(ڌاڳا);
            أرجع سچ;
      }
      أرجع خطأ;
}

vvp_fun_ڪنارو_sa::vvp_fun_ڪنارو_sa(ڪنارو_t e)
: vvp_fun_ڪنارو(e), ڌاڳا_2(0)
{
}

vvp_fun_ڪنارو_sa::~vvp_fun_ڪنارو_sa()
{
}

vthread_t vvp_fun_ڪنارو_sa::جوڙيو_انتظار_thread(vthread_t thread)
{
      vthread_t tmp = ڌاڳا_2;
      ڌاڳا_2 = thread;

      أرجع tmp;
}

فراغ vvp_fun_ڪنارو_sa::recv_vec4(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
                                vvp_context_t)
{
      إذا (recv_vec4_(ٻٽو_انگ, بٽ_2[پورٽ.پورٽ()], ڌاڳا_2)) {
	    vvp_net_t*net = پورٽ.ptr();
	    net->send_vec4(ٻٽو_انگ, 0);
      }
}

فراغ vvp_fun_ڪنارو_sa::recv_vec4_pv(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
				   بنانشان بنياد, بنانشان vwid, vvp_context_t)
{
      أكد(بنياد == 0);
      إذا (recv_vec4_(ٻٽو_انگ, بٽ_2[پورٽ.پورٽ()], ڌاڳا_2)) {
	    vvp_net_t*net = پورٽ.ptr();
	    net->send_vec4_pv(ٻٽو_انگ, بنياد, vwid, 0);
      }
}

vvp_fun_ڪنارو_aa::vvp_fun_ڪنارو_aa(ڪنارو_t e)
: vvp_fun_ڪنارو(e)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_جوڙيو_شي_منزل_context(هذا, context_scope_);
}

vvp_fun_ڪنارو_aa::~vvp_fun_ڪنارو_aa()
{
}

فراغ vvp_fun_ڪنارو_aa::alloc_instance(vvp_context_t context)
{
      vvp_سيٽ_context_شي(context, context_idx_, جديد vvp_fun_ڪنارو_state_s);
      reset_instance(context);
}

فراغ vvp_fun_ڪنارو_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_ڪنارو_state_s*state = static_cast<vvp_fun_ڪنارو_state_s*>
            (vvp_get_context_شي(context, context_idx_));

      state->ڌاڳا = 0;
      لكل (بنانشان idx = 0 ;  idx < 4 ;  idx += 1)
            state->بٽ[idx] = بٽ_2[idx];
}

#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
فراغ vvp_fun_ڪنارو_aa::free_instance(vvp_context_t context)
{
      vvp_fun_ڪنارو_state_s*state = static_cast<vvp_fun_ڪنارو_state_s*>
            (vvp_get_context_شي(context, context_idx_));
      ميٽيو state;
}
#جيڪڏهن_پورو

vthread_t vvp_fun_ڪنارو_aa::جوڙيو_انتظار_thread(vthread_t thread)
{
      vvp_fun_ڪنارو_state_s*state = static_cast<vvp_fun_ڪنارو_state_s*>
            (vthread_get_wt_context_شي(context_idx_));

      vthread_t tmp = state->ڌاڳا;
      state->ڌاڳا = thread;

      أرجع tmp;
}

فراغ vvp_fun_ڪنارو_aa::recv_vec4(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
                                vvp_context_t context)
{
      إذا (context) {
            vvp_fun_ڪنارو_state_s*state = static_cast<vvp_fun_ڪنارو_state_s*>
                  (vvp_get_context_شي(context, context_idx_));

            إذا (recv_vec4_(ٻٽو_انگ, state->بٽ[پورٽ.پورٽ()], state->ڌاڳا)) {
                  vvp_net_t*net = پورٽ.ptr();
                  net->send_vec4(ٻٽو_انگ, context);
            }
      } وإلا {
            context = context_scope_->جيئرو_contexts;
            طالما (context) {
                  recv_vec4(پورٽ, ٻٽو_انگ, context);
                  context = vvp_get_ايندڙ_context(context);
            }
            بٽ_2[پورٽ.پورٽ()] = ٻٽو_انگ.value(0);
      }
}

صنف anyedge_قدر {

    عام:
      anyedge_قدر() {};
      مجازي ~anyedge_قدر() {};

      مجازي فراغ reset() = 0;

      مجازي فراغ duplicate(anyedge_قدر*&dup) = 0;
};

صنف anyedge_vec4_قدر : عام anyedge_قدر {

    عام:
      anyedge_vec4_قدر() {};
      مجازي ~anyedge_vec4_قدر() override {};

      فراغ reset() override { old_بٽ.سيٽ_منزل_x(); }

      فراغ سيٽ(ثابت vvp_vector4_t&ٻٽو_انگ) { old_بٽ = ٻٽو_انگ; };

      فراغ duplicate(anyedge_قدر*&dup) override;

      منطقي recv_vec4(ثابت vvp_vector4_t&ٻٽو_انگ);

      منطقي recv_vec4_pv(ثابت vvp_vector4_t&ٻٽو_انگ, بنانشان بنياد,
			بنانشان vwid);

    خاص:
      vvp_vector4_t old_بٽ;
};

ساكن anyedge_vec4_قدر*get_vec4_قدر(anyedge_قدر*&value)
{
      anyedge_vec4_قدر*vec4_قدر = dynamic_cast<anyedge_vec4_قدر*>(value);
      إذا (!value) {
	    vec4_قدر = جديد anyedge_vec4_قدر();
	    ميٽيو value;
	    value = vec4_قدر;
      }
      أرجع vec4_قدر;
}

صنف anyedge_real_قدر : عام anyedge_قدر {

    عام:
      anyedge_real_قدر() : old_بٽ(0.0) {};
      مجازي ~anyedge_real_قدر() override {};

      فراغ reset() override { old_بٽ = 0.0; }

      فراغ سيٽ(مزدوج ٻٽو_انگ) { old_بٽ = ٻٽو_انگ; };

      فراغ duplicate(anyedge_قدر*&dup) override;

      منطقي recv_real(مزدوج ٻٽو_انگ);

    خاص:
      مزدوج old_بٽ;
};

ساكن anyedge_real_قدر*get_real_قدر(anyedge_قدر*&value)
{
      anyedge_real_قدر*real_قدر = dynamic_cast<anyedge_real_قدر*>(value);
      إذا (!value) {
	    real_قدر = جديد anyedge_real_قدر();
	    ميٽيو value;
	    value = real_قدر;
      }
      أرجع real_قدر;
}

صنف anyedge_string_قدر : عام anyedge_قدر {

    عام:
      anyedge_string_قدر() {};
      مجازي ~anyedge_string_قدر() override {};

      فراغ reset() override { old_بٽ.clear(); }

      فراغ سيٽ(ثابت std::string&ٻٽو_انگ) { old_بٽ = ٻٽو_انگ; };

      فراغ duplicate(anyedge_قدر*&dup) override;

      منطقي recv_string(ثابت std::string&ٻٽو_انگ);

    خاص:
      std::string old_بٽ;
};

ساكن anyedge_string_قدر*get_string_قدر(anyedge_قدر*&value)
{
      anyedge_string_قدر*string_قدر = dynamic_cast<anyedge_string_قدر*>(value);
      إذا (!value) {
	    string_قدر = جديد anyedge_string_قدر();
	    ميٽيو value;
	    value = string_قدر;
      }
      أرجع string_قدر;
}

بنية vvp_fun_anyedge_state_s : عام waitable_state_s {
      vvp_fun_anyedge_state_s()
      {
            لكل (بنانشان idx = 0 ;  idx < 4 ;  idx += 1)
                  آخري_قدر[idx] = 0;
      }

      ~vvp_fun_anyedge_state_s()
      {
            لكل (بنانشان idx = 0 ;  idx < 4 ;  idx += 1)
                  ميٽيو آخري_قدر[idx];
      }

      anyedge_قدر *آخري_قدر[4];
      vvp_fun_anyedge_state_s(ثابت vvp_fun_anyedge_state_s&) = ميٽيو;
      vvp_fun_anyedge_state_s& عامل=(ثابت vvp_fun_anyedge_state_s&) = ميٽيو;
};

vvp_fun_anyedge::vvp_fun_anyedge()
{
      لكل (بنانشان idx = 0 ;  idx < 4 ;  idx += 1)
	    آخري_قدر[idx] = 0;
}

vvp_fun_anyedge::~vvp_fun_anyedge()
{
      لكل (بنانشان idx = 0 ;  idx < 4 ;  idx += 1)
	    ميٽيو آخري_قدر[idx];
}

فراغ anyedge_vec4_قدر::duplicate(anyedge_قدر*&dup)
{
      anyedge_vec4_قدر*dup_vec4 = get_vec4_قدر(dup);
      أكد(dup_vec4);
      dup_vec4->سيٽ(old_بٽ);
}

منطقي anyedge_vec4_قدر::recv_vec4(ثابت vvp_vector4_t&ٻٽو_انگ)
{
      منطقي flag = خطأ;

      إذا (old_بٽ.size() != ٻٽو_انگ.size()) {
	    إذا (old_بٽ.size() == 0) {
		    // Special case: If we've not seen any input yet
		    // (old_bits.size()==0) then replace it will a reference
		    // vector that is 'bx. Then compare that with the input
		    // to see if we are processing a change from 'bx.
		  old_بٽ = vvp_vector4_t(ٻٽو_انگ.size(), BIT4_X);
		  إذا (old_بٽ.eeq(ٻٽو_انگ))
			flag = خطأ;
		  وإلا
			flag = سچ;

	    } وإلا {
		  flag = سچ;
	    }

      } وإلا {
	    لكل (بنانشان idx = 0 ;  idx < ٻٽو_انگ.size() ;  idx += 1) {
		  إذا (old_بٽ.value(idx) != ٻٽو_انگ.value(idx)) {
			flag = سچ;
			اكسر;
		  }
	    }
      }

      إذا (flag) {
	    old_بٽ = ٻٽو_انگ;
      }

      أرجع flag;
}

منطقي anyedge_vec4_قدر::recv_vec4_pv(ثابت vvp_vector4_t&ٻٽو_انگ, بنانشان بنياد,
				      بنانشان vwid)
{
      vvp_vector4_t tmp = old_بٽ;
      إذا (tmp.size() == 0)
	    tmp = vvp_vector4_t(vwid, BIT4_Z);
      أكد(بنياد + ٻٽو_انگ.size()<= vwid);
      أكد(tmp.size() == vwid);
      tmp.سيٽ_vec(بنياد, ٻٽو_انگ);

      أرجع recv_vec4(tmp);
}

فراغ anyedge_real_قدر::duplicate(anyedge_قدر*&dup)
{
      anyedge_real_قدر*dup_real = get_real_قدر(dup);
      أكد(dup_real);
      dup_real->سيٽ(old_بٽ);
}

منطقي anyedge_real_قدر::recv_real(مزدوج ٻٽو_انگ)
{
      إذا (old_بٽ != ٻٽو_انگ) {
	    old_بٽ = ٻٽو_انگ;
            أرجع سچ;
      }
      أرجع خطأ;
}

فراغ anyedge_string_قدر::duplicate(anyedge_قدر*&dup)
{
      anyedge_string_قدر*dup_string = get_string_قدر(dup);
      أكد(dup_string);
      dup_string->سيٽ(old_بٽ);
}

منطقي anyedge_string_قدر::recv_string(ثابت std::string&ٻٽو_انگ)
{
      إذا (old_بٽ != ٻٽو_انگ) {
	    old_بٽ = ٻٽو_انگ;
            أرجع سچ;
      }
      أرجع خطأ;
}

vvp_fun_anyedge_sa::vvp_fun_anyedge_sa()
: ڌاڳا_2(0)
{
}

vvp_fun_anyedge_sa::~vvp_fun_anyedge_sa()
{
}

vthread_t vvp_fun_anyedge_sa::جوڙيو_انتظار_thread(vthread_t thread)
{
      vthread_t tmp = ڌاڳا_2;
      ڌاڳا_2 = thread;

      أرجع tmp;
}

فراغ vvp_fun_anyedge_sa::recv_vec4(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
                                   vvp_context_t)
{
      anyedge_vec4_قدر*value = get_vec4_قدر(آخري_قدر[پورٽ.پورٽ()]);
      أكد(value);
      إذا (value->recv_vec4(ٻٽو_انگ)) {
	    لاڳيتو_انتظار_ڌاڳا(ڌاڳا_2);
	    vvp_net_t*net = پورٽ.ptr();
	    net->send_vec4(ٻٽو_انگ, 0);
      }
}

فراغ vvp_fun_anyedge_sa::recv_vec4_pv(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
				      بنانشان بنياد, بنانشان vwid, vvp_context_t)
{
      anyedge_vec4_قدر*value = get_vec4_قدر(آخري_قدر[پورٽ.پورٽ()]);
      أكد(value);
      إذا (value->recv_vec4_pv(ٻٽو_انگ, بنياد, vwid)) {
	    لاڳيتو_انتظار_ڌاڳا(ڌاڳا_2);
	    vvp_net_t*net = پورٽ.ptr();
	    net->send_vec4(ٻٽو_انگ, 0);
      }
}

فراغ vvp_fun_anyedge_sa::recv_real(vvp_net_ptr_t پورٽ, مزدوج ٻٽو_انگ,
                                   vvp_context_t)
{
      anyedge_real_قدر*value = get_real_قدر(آخري_قدر[پورٽ.پورٽ()]);
      أكد(value);
      إذا (value->recv_real(ٻٽو_انگ)) {
	    لاڳيتو_انتظار_ڌاڳا(ڌاڳا_2);
	    vvp_net_t*net = پورٽ.ptr();
	    net->send_vec4(vvp_vector4_t(), 0);
      }
}

فراغ vvp_fun_anyedge_sa::recv_string(vvp_net_ptr_t پورٽ, ثابت std::string&ٻٽو_انگ,
				     vvp_context_t)
{
      anyedge_string_قدر*value = get_string_قدر(آخري_قدر[پورٽ.پورٽ()]);
      أكد(value);
      إذا (value->recv_string(ٻٽو_انگ)) {
	    لاڳيتو_انتظار_ڌاڳا(ڌاڳا_2);
	    vvp_net_t*net = پورٽ.ptr();
	    net->send_vec4(vvp_vector4_t(), 0);
      }
}

/*
 * An anyedge receiving an object should do nothing with it, but should
 * trigger waiting threads.
 */
فراغ vvp_fun_anyedge_sa::recv_object(vvp_net_ptr_t پورٽ, vvp_object_t,
				     vvp_context_t)
{
      لاڳيتو_انتظار_ڌاڳا(ڌاڳا_2);
      vvp_net_t*net = پورٽ.ptr();
      net->send_vec4(vvp_vector4_t(), 0);
}

vvp_fun_anyedge_aa::vvp_fun_anyedge_aa()
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_جوڙيو_شي_منزل_context(هذا, context_scope_);
}

vvp_fun_anyedge_aa::~vvp_fun_anyedge_aa()
{
}

فراغ vvp_fun_anyedge_aa::alloc_instance(vvp_context_t context)
{
      vvp_سيٽ_context_شي(context, context_idx_, جديد vvp_fun_anyedge_state_s);
      reset_instance(context);
}

فراغ vvp_fun_anyedge_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
            (vvp_get_context_شي(context, context_idx_));

      state->ڌاڳا = 0;
      لكل (بنانشان idx = 0 ;  idx < 4 ;  idx += 1) {
	    إذا (آخري_قدر[idx])
	          آخري_قدر[idx]->duplicate(state->آخري_قدر[idx]);
	    وإلا إذا (state->آخري_قدر[idx])
		  state->آخري_قدر[idx]->reset();
      }
}

#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
فراغ vvp_fun_anyedge_aa::free_instance(vvp_context_t context)
{
      vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
            (vvp_get_context_شي(context, context_idx_));
      ميٽيو state;
}
#جيڪڏهن_پورو

vthread_t vvp_fun_anyedge_aa::جوڙيو_انتظار_thread(vthread_t thread)
{
      vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
            (vthread_get_wt_context_شي(context_idx_));

      vthread_t tmp = state->ڌاڳا;
      state->ڌاڳا = thread;

      أرجع tmp;
}

فراغ vvp_fun_anyedge_aa::recv_vec4(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
                                   vvp_context_t context)
{
      إذا (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_شي(context, context_idx_));

            anyedge_vec4_قدر*value = get_vec4_قدر(state->آخري_قدر[پورٽ.پورٽ()]);
            أكد(value);
            إذا (value->recv_vec4(ٻٽو_انگ)) {
                  لاڳيتو_انتظار_ڌاڳا(state->ڌاڳا);
                  vvp_net_t*net = پورٽ.ptr();
                  net->send_vec4(ٻٽو_انگ, context);
            }
      } وإلا {
            context = context_scope_->جيئرو_contexts;
            طالما (context) {
                  recv_vec4(پورٽ, ٻٽو_انگ, context);
                  context = vvp_get_ايندڙ_context(context);
            }
            anyedge_vec4_قدر*value = get_vec4_قدر(آخري_قدر[پورٽ.پورٽ()]);
            أكد(value);
            value->سيٽ(ٻٽو_انگ);
      }
}

فراغ vvp_fun_anyedge_aa::recv_real(vvp_net_ptr_t پورٽ, مزدوج ٻٽو_انگ,
                                   vvp_context_t context)
{
      إذا (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_شي(context, context_idx_));

            anyedge_real_قدر*value = get_real_قدر(state->آخري_قدر[پورٽ.پورٽ()]);
            أكد(value);
            إذا (value->recv_real(ٻٽو_انگ)) {
                  لاڳيتو_انتظار_ڌاڳا(state->ڌاڳا);
                  vvp_net_t*net = پورٽ.ptr();
                  net->send_vec4(vvp_vector4_t(), context);
            }
      } وإلا {
            context = context_scope_->جيئرو_contexts;
            طالما (context) {
                  recv_real(پورٽ, ٻٽو_انگ, context);
                  context = vvp_get_ايندڙ_context(context);
            }
            anyedge_real_قدر*value = get_real_قدر(آخري_قدر[پورٽ.پورٽ()]);
            أكد(value);
            value->سيٽ(ٻٽو_انگ);
      }
}

فراغ vvp_fun_anyedge_aa::recv_string(vvp_net_ptr_t پورٽ, ثابت std::string&ٻٽو_انگ,
				     vvp_context_t context)
{
      إذا (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_شي(context, context_idx_));

            anyedge_string_قدر*value = get_string_قدر(state->آخري_قدر[پورٽ.پورٽ()]);
            أكد(value);
            إذا (value->recv_string(ٻٽو_انگ)) {
                  لاڳيتو_انتظار_ڌاڳا(state->ڌاڳا);
                  vvp_net_t*net = پورٽ.ptr();
                  net->send_vec4(vvp_vector4_t(), context);
            }
      } وإلا {
            context = context_scope_->جيئرو_contexts;
            طالما (context) {
                  recv_string(پورٽ, ٻٽو_انگ, context);
                  context = vvp_get_ايندڙ_context(context);
            }
            anyedge_string_قدر*value = get_string_قدر(آخري_قدر[پورٽ.پورٽ()]);
            أكد(value);
            value->سيٽ(ٻٽو_انگ);
      }
}

vvp_fun_event_or::vvp_fun_event_or(vvp_net_t*بنياد_net)
: بنياد_net_2(بنياد_net)
{
}

vvp_fun_event_or::~vvp_fun_event_or()
{
}

vvp_fun_event_or_sa::vvp_fun_event_or_sa(vvp_net_t*بنياد_net)
: vvp_fun_event_or(بنياد_net), ڌاڳا_2(0)
{
}

vvp_fun_event_or_sa::~vvp_fun_event_or_sa()
{
}

vthread_t vvp_fun_event_or_sa::جوڙيو_انتظار_thread(vthread_t thread)
{
      vthread_t tmp = ڌاڳا_2;
      ڌاڳا_2 = thread;

      أرجع tmp;
}

فراغ vvp_fun_event_or_sa::recv_vec4(vvp_net_ptr_t, ثابت vvp_vector4_t&ٻٽو_انگ,
                                    vvp_context_t)
{
      لاڳيتو_انتظار_ڌاڳا(ڌاڳا_2);
      بنياد_net_2->send_vec4(ٻٽو_انگ, 0);
}

vvp_fun_event_or_aa::vvp_fun_event_or_aa(vvp_net_t*بنياد_net)
: vvp_fun_event_or(بنياد_net)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_جوڙيو_شي_منزل_context(هذا, context_scope_);
}

vvp_fun_event_or_aa::~vvp_fun_event_or_aa()
{
}

فراغ vvp_fun_event_or_aa::alloc_instance(vvp_context_t context)
{
      vvp_سيٽ_context_شي(context, context_idx_, جديد waitable_state_s);
}

فراغ vvp_fun_event_or_aa::reset_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_شي(context, context_idx_));

      state->ڌاڳا = 0;
}

#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
فراغ vvp_fun_event_or_aa::free_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_شي(context, context_idx_));
      ميٽيو state;
}
#جيڪڏهن_پورو

vthread_t vvp_fun_event_or_aa::جوڙيو_انتظار_thread(vthread_t thread)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vthread_get_wt_context_شي(context_idx_));

      vthread_t tmp = state->ڌاڳا;
      state->ڌاڳا = thread;

      أرجع tmp;
}

فراغ vvp_fun_event_or_aa::recv_vec4(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
                                    vvp_context_t context)
{
      إذا (context) {
            waitable_state_s*state = static_cast<waitable_state_s*>
                  (vvp_get_context_شي(context, context_idx_));

            لاڳيتو_انتظار_ڌاڳا(state->ڌاڳا);
            بنياد_net_2->send_vec4(ٻٽو_انگ, context);
      } وإلا {
            context = context_scope_->جيئرو_contexts;
            طالما (context) {
                  recv_vec4(پورٽ, ٻٽو_انگ, context);
                  context = vvp_get_ايندڙ_context(context);
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
: vvp_named_event(h), ڌاڳا_2(0)
{
}

vvp_named_event_sa::~vvp_named_event_sa()
{
}

vthread_t vvp_named_event_sa::جوڙيو_انتظار_thread(vthread_t thread)
{
      vthread_t tmp = ڌاڳا_2;
      ڌاڳا_2 = thread;

      أرجع tmp;
}

فراغ vvp_named_event_sa::recv_vec4(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
                                   vvp_context_t)
{
      لاڳيتو_انتظار_ڌاڳا(ڌاڳا_2);
      vvp_net_t*net = پورٽ.ptr();
      net->send_vec4(ٻٽو_انگ, 0);

      __vpiNamedEvent*obj = dynamic_cast<__vpiNamedEvent*>(handle_);
      أكد(obj);
      obj->لاڳيتو_vpi_callbacks();
}

vvp_named_event_aa::vvp_named_event_aa(__vpiHandle*h)
: vvp_named_event(h)
{
      context_idx_ = vpip_جوڙيو_شي_منزل_context(هذا, vpip_peek_context_scope());
}

vvp_named_event_aa::~vvp_named_event_aa()
{
}

فراغ vvp_named_event_aa::alloc_instance(vvp_context_t context)
{
      vvp_سيٽ_context_شي(context, context_idx_, جديد waitable_state_s);
}

فراغ vvp_named_event_aa::reset_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_شي(context, context_idx_));

      state->ڌاڳا = 0;
}

#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
فراغ vvp_named_event_aa::free_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_شي(context, context_idx_));
      ميٽيو state;
}
#جيڪڏهن_پورو

vthread_t vvp_named_event_aa::جوڙيو_انتظار_thread(vthread_t thread)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vthread_get_wt_context_شي(context_idx_));

      vthread_t tmp = state->ڌاڳا;
      state->ڌاڳا = thread;

      أرجع tmp;
}

فراغ vvp_named_event_aa::recv_vec4(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
                                   vvp_context_t context)
{
      أكد(context);

      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_شي(context, context_idx_));

      لاڳيتو_انتظار_ڌاڳا(state->ڌاڳا);
      vvp_net_t*net = پورٽ.ptr();
      net->send_vec4(ٻٽو_انگ, context);
}

/*
**  Create an event functor
**  edge:  compile_event(label, type, argc, argv, debug_flag)
**  or:    compile_event(label, NULL, argc, argv, debug_flag)
**
**  Named events are handled elsewhere.
*/

ساكن فراغ compile_event_or(حرف*label, بنانشان argc, بنية symb_s*argv);

فراغ compile_event(حرف*label, حرف*type, بنانشان argc, بنية symb_s*argv)
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

	    vvp_fun_ڪنارو::ڪنارو_t ڪنارو_type = vvp_ڪنارو_none;

	    إذا (strcmp(type,"posedge") == 0)
		  ڪنارو_type = vvp_ڪنارو_posedge;
	    وإلا إذا (strcmp(type,"negedge") == 0)
		  ڪنارو_type = vvp_ڪنارو_negedge;
	    وإلا إذا (strcmp(type,"edge") == 0)
		  ڪنارو_type = vvp_ڪنارو_ڪنارو;

	    أكد(argc <= 4);
	    حرر(type);

            إذا (vpip_peek_current_scope()->is_automatic()) {
                  fun = جديد vvp_fun_ڪنارو_aa(ڪنارو_type);
            } وإلا {
                  fun = جديد vvp_fun_ڪنارو_sa(ڪنارو_type);
            }

      }

      vvp_net_t* ptr = جديد vvp_net_t;
      ptr->fun = fun;

      define_functor_نشان(label, ptr);
      حرر(label);

      inputs_connect(ptr, argc, argv);
      حرر(argv);
}

ساكن فراغ compile_event_or(حرف*label, بنانشان argc, بنية symb_s*argv)
{
      vvp_net_t*بنياد_net = جديد vvp_net_t;
      إذا (vpip_peek_current_scope()->is_automatic()) {
            بنياد_net->fun = جديد vvp_fun_event_or_aa(بنياد_net);
      } وإلا {
            بنياد_net->fun = جديد vvp_fun_event_or_sa(بنياد_net);
      }
      define_functor_نشان(label, بنياد_net);
      حرر(label);

	/* This is a simplified version of a wide functor. We don't
	   care about the data values or what port they arrived on,
	   so we can use a single shared functor. */
      vvp_net_t*curr_net = بنياد_net;
      لكل (بنانشان idx = 0 ;  idx < argc ;  idx += 1) {
	    إذا (idx > 0 && (idx % 4) == 0) {
		  curr_net = جديد vvp_net_t;
		  curr_net->fun = بنياد_net->fun;
	    }
	    داخلا_connect(curr_net, idx % 4, argv[idx].متن);
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
      define_functor_نشان(label, ptr);
      compile_vpi_نشان(label, obj);
      إذا (! local_flag) vpip_attach_منزل_current_scope(obj);

      حرر(label);
      ميٽيو[] name;
}

#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
فراغ named_event_delete(__vpiHandle*handle)
{
      ميٽيو dynamic_cast<__vpiNamedEvent *>(handle);
}
#جيڪڏهن_پورو
