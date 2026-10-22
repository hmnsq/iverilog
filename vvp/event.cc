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

فراغ waitable_hooks_s::لگاتار_اڈیک_دھاگے(vthread_t&دھاگے)
{
	// Run the non-blocking event controls.
      آخری = &event_ctls;
      لكل (evctl*cur = event_ctls; cur != 0;) {
	    إذا (cur->dec_and_لگاتار()) {
		  evctl*nxt = cur->اگلا_2;
		  مٹاؤ cur;
		  cur = nxt;
		  *آخری = cur;
	    } وإلا {
		  آخری = &(cur->اگلا_2);
		  cur = cur->اگلا_2;
	    }
      }

      vthread_t tmp = دھاگے;
      إذا (tmp == 0) أرجع;
      دھاگے = 0;

      vthread_ویلا_سارنی_list(tmp);
}

evctl::evctl(uint64_t ecount)
{
      ecount_ = ecount;
      اگلا_2 = 0;
}

منطقي evctl::dec_and_لگاتار()
{
      أكد(ecount_ != 0);

      ecount_ -= 1;
      إذا (ecount_ == 0) لگاتار_لگاتار();

      أرجع ecount_ == 0;
}

evctl_real::evctl_real(__vpiHandle*handle, مزدوج value,
                       uint64_t ecount)
:evctl(ecount)
{
      handle_ = handle;
      قدر_2 = value;
}

فراغ evctl_real::لگاتار_لگاتار()
{
      t_vpi_قدر val;

      val.format = vpiRealVal;
      val.value.real = قدر_2;
      vpi_put_قدر(handle_, &val, 0, vpiNoDelay);
}

فراغ ویلا_سارنی_evctl(__vpiHandle*handle, مزدوج value,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      أكد(ep);
	// Now add this call to the end of the event list.
      *(ep->آخری) = جديد evctl_real(handle, value, ecount);
      ep->آخری = &((*(ep->آخری))->اگلا_2);
}

evctl_سمتیہ::evctl_سمتیہ(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&value,
                           بنانشانی off, بنانشانی wid, uint64_t ecount)
:evctl(ecount), ptr_(ptr), قدر_2(value)
{
      off_ = off;
      wid_ = wid;
}

فراغ evctl_سمتیہ::لگاتار_لگاتار()
{
      إذا (wid_ != 0) {
	    vvp_send_vec4_pv(ptr_, قدر_2, off_, wid_, 0);
      } وإلا {
	    vvp_send_vec4(ptr_, قدر_2, 0);
      }
}

فراغ ویلا_سارنی_evctl(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&value,
                    بنانشانی وتھ_2, بنانشانی wid,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      أكد(ep);
	// Now add this call to the end of the event list.
      *(ep->آخری) = جديد evctl_سمتیہ(ptr, value, وتھ_2, wid, ecount);
      ep->آخری = &((*(ep->آخری))->اگلا_2);
}

evctl_array::evctl_array(vvp_array_t حافظہ, بنانشانی اشاریہ,
                         ثابت vvp_vector4_t&value, بنانشانی off,
                         uint64_t ecount)
:evctl(ecount), قدر_2(value)
{
      mem_ = حافظہ;
      idx_ = اشاریہ;
      off_ = off;
}

فراغ evctl_array::لگاتار_لگاتار()
{
      mem_->سیٹ_لفظ(idx_, off_, قدر_2);
}

فراغ ویلا_سارنی_evctl(vvp_array_t حافظہ, بنانشانی اشاریہ,
                    ثابت vvp_vector4_t&value, بنانشانی وتھ_2,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      أكد(ep);
	// Now add this call to the end of the event list.
      *(ep->آخری) = جديد evctl_array(حافظہ, اشاریہ, value, وتھ_2, ecount);
      ep->آخری = &((*(ep->آخری))->اگلا_2);
}

evctl_array_r::evctl_array_r(vvp_array_t حافظہ, بنانشانی اشاریہ,
                             مزدوج value, uint64_t ecount)
:evctl(ecount)
{
      mem_ = حافظہ;
      idx_ = اشاریہ;
      قدر_2 = value;
}

فراغ evctl_array_r::لگاتار_لگاتار()
{
      mem_->سیٹ_لفظ(idx_, قدر_2);
}

فراغ ویلا_سارنی_evctl(vvp_array_t حافظہ, بنانشانی اشاریہ,
                    مزدوج value,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      أكد(ep);
	// Now add this call to the end of the event list.
      *(ep->آخری) = جديد evctl_array_r(حافظہ, اشاریہ, value, ecount);
      ep->آخری = &((*(ep->آخری))->اگلا_2);
}

مضمن vvp_fun_کنارا::کنارا_t VVP_کنارا(vvp_bit4_t from, vvp_bit4_t to)
{
      أرجع 1 << ((from << 2) | to);
}

ثابت vvp_fun_کنارا::کنارا_t vvp_کنارا_posedge
      = VVP_کنارا(BIT4_0,BIT4_1)
      | VVP_کنارا(BIT4_0,BIT4_X)
      | VVP_کنارا(BIT4_0,BIT4_Z)
      | VVP_کنارا(BIT4_X,BIT4_1)
      | VVP_کنارا(BIT4_Z,BIT4_1)
      ;

ثابت vvp_fun_کنارا::کنارا_t vvp_کنارا_negedge
      = VVP_کنارا(BIT4_1,BIT4_0)
      | VVP_کنارا(BIT4_1,BIT4_X)
      | VVP_کنارا(BIT4_1,BIT4_Z)
      | VVP_کنارا(BIT4_X,BIT4_0)
      | VVP_کنارا(BIT4_Z,BIT4_0)
      ;

ثابت vvp_fun_کنارا::کنارا_t vvp_کنارا_کنارا
      = VVP_کنارا(BIT4_0,BIT4_1)
      | VVP_کنارا(BIT4_1,BIT4_0)
      | VVP_کنارا(BIT4_0,BIT4_X)
      | VVP_کنارا(BIT4_X,BIT4_0)
      | VVP_کنارا(BIT4_0,BIT4_Z)
      | VVP_کنارا(BIT4_Z,BIT4_0)
      | VVP_کنارا(BIT4_X,BIT4_1)
      | VVP_کنارا(BIT4_1,BIT4_X)
      | VVP_کنارا(BIT4_Z,BIT4_1)
      | VVP_کنارا(BIT4_1,BIT4_Z)
      ;

ثابت vvp_fun_کنارا::کنارا_t vvp_کنارا_none    = 0;

بنية vvp_fun_کنارا_state_s : عام waitable_state_s {
      vvp_fun_کنارا_state_s()
      {
            لكل (بنانشانی idx = 0 ;  idx < 4 ;  idx += 1)
                  بٹ[idx] = BIT4_X;
      }

      vvp_bit4_t بٹ[4];
};

vvp_fun_کنارا::vvp_fun_کنارا(کنارا_t e)
: کنارا_2(e)
{
      لكل (بنانشانی idx = 0 ;  idx < 4 ;  idx += 1)
            بٹ_2[idx] = BIT4_X;
}

vvp_fun_کنارا::~vvp_fun_کنارا()
{
}

منطقي vvp_fun_کنارا::recv_vec4_(ثابت vvp_vector4_t&ثنائی_ہندسہ,
                              vvp_bit4_t&old_ثنائی_ہندسہ, vthread_t&دھاگے)
{
	/* See what kind of edge this represents. */
      کنارا_t نقاب = VVP_کنارا(old_ثنائی_ہندسہ, ثنائی_ہندسہ.value(0));

	/* Save the current input for the next time around. */
      old_ثنائی_ہندسہ = ثنائی_ہندسہ.value(0);

      إذا ((کنارا_2 == vvp_کنارا_none) || (کنارا_2 & نقاب)) {
	    لگاتار_اڈیک_دھاگے(دھاگے);
            أرجع سچ;
      }
      أرجع خطأ;
}

vvp_fun_کنارا_sa::vvp_fun_کنارا_sa(کنارا_t e)
: vvp_fun_کنارا(e), دھاگے_2(0)
{
}

vvp_fun_کنارا_sa::~vvp_fun_کنارا_sa()
{
}

vthread_t vvp_fun_کنارا_sa::جوڑو_اڈیک_thread(vthread_t thread)
{
      vthread_t tmp = دھاگے_2;
      دھاگے_2 = thread;

      أرجع tmp;
}

فراغ vvp_fun_کنارا_sa::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                vvp_context_t)
{
      إذا (recv_vec4_(ثنائی_ہندسہ, بٹ_2[پورٹ.پورٹ()], دھاگے_2)) {
	    vvp_net_t*net = پورٹ.ptr();
	    net->send_vec4(ثنائی_ہندسہ, 0);
      }
}

فراغ vvp_fun_کنارا_sa::recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
				   بنانشانی ادھار, بنانشانی vwid, vvp_context_t)
{
      أكد(ادھار == 0);
      إذا (recv_vec4_(ثنائی_ہندسہ, بٹ_2[پورٹ.پورٹ()], دھاگے_2)) {
	    vvp_net_t*net = پورٹ.ptr();
	    net->send_vec4_pv(ثنائی_ہندسہ, ادھار, vwid, 0);
      }
}

vvp_fun_کنارا_aa::vvp_fun_کنارا_aa(کنارا_t e)
: vvp_fun_کنارا(e)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_جوڑو_شے_منزل_context(هذا, context_scope_);
}

vvp_fun_کنارا_aa::~vvp_fun_کنارا_aa()
{
}

فراغ vvp_fun_کنارا_aa::alloc_instance(vvp_context_t context)
{
      vvp_سیٹ_context_شے(context, context_idx_, جديد vvp_fun_کنارا_state_s);
      reset_instance(context);
}

فراغ vvp_fun_کنارا_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_کنارا_state_s*state = static_cast<vvp_fun_کنارا_state_s*>
            (vvp_get_context_شے(context, context_idx_));

      state->دھاگے = 0;
      لكل (بنانشانی idx = 0 ;  idx < 4 ;  idx += 1)
            state->بٹ[idx] = بٹ_2[idx];
}

#جے_تعریف_شدہ CHECK_WITH_VALGRIND
فراغ vvp_fun_کنارا_aa::free_instance(vvp_context_t context)
{
      vvp_fun_کنارا_state_s*state = static_cast<vvp_fun_کنارا_state_s*>
            (vvp_get_context_شے(context, context_idx_));
      مٹاؤ state;
}
#جے_انت

vthread_t vvp_fun_کنارا_aa::جوڑو_اڈیک_thread(vthread_t thread)
{
      vvp_fun_کنارا_state_s*state = static_cast<vvp_fun_کنارا_state_s*>
            (vthread_get_wt_context_شے(context_idx_));

      vthread_t tmp = state->دھاگے;
      state->دھاگے = thread;

      أرجع tmp;
}

فراغ vvp_fun_کنارا_aa::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                vvp_context_t context)
{
      إذا (context) {
            vvp_fun_کنارا_state_s*state = static_cast<vvp_fun_کنارا_state_s*>
                  (vvp_get_context_شے(context, context_idx_));

            إذا (recv_vec4_(ثنائی_ہندسہ, state->بٹ[پورٹ.پورٹ()], state->دھاگے)) {
                  vvp_net_t*net = پورٹ.ptr();
                  net->send_vec4(ثنائی_ہندسہ, context);
            }
      } وإلا {
            context = context_scope_->جیوندا_contexts;
            طالما (context) {
                  recv_vec4(پورٹ, ثنائی_ہندسہ, context);
                  context = vvp_get_اگلا_context(context);
            }
            بٹ_2[پورٹ.پورٹ()] = ثنائی_ہندسہ.value(0);
      }
}

صنف anyedge_قدر {

    عام:
      anyedge_قدر() {};
      مجازی ~anyedge_قدر() {};

      مجازی فراغ reset() = 0;

      مجازی فراغ duplicate(anyedge_قدر*&dup) = 0;
};

صنف anyedge_vec4_قدر : عام anyedge_قدر {

    عام:
      anyedge_vec4_قدر() {};
      مجازی ~anyedge_vec4_قدر() override {};

      فراغ reset() override { old_بٹ.سیٹ_منزل_x(); }

      فراغ سیٹ(ثابت vvp_vector4_t&ثنائی_ہندسہ) { old_بٹ = ثنائی_ہندسہ; };

      فراغ duplicate(anyedge_قدر*&dup) override;

      منطقي recv_vec4(ثابت vvp_vector4_t&ثنائی_ہندسہ);

      منطقي recv_vec4_pv(ثابت vvp_vector4_t&ثنائی_ہندسہ, بنانشانی ادھار,
			بنانشانی vwid);

    خاص:
      vvp_vector4_t old_بٹ;
};

ساكن anyedge_vec4_قدر*get_vec4_قدر(anyedge_قدر*&value)
{
      anyedge_vec4_قدر*vec4_قدر = dynamic_cast<anyedge_vec4_قدر*>(value);
      إذا (!value) {
	    vec4_قدر = جديد anyedge_vec4_قدر();
	    مٹاؤ value;
	    value = vec4_قدر;
      }
      أرجع vec4_قدر;
}

صنف anyedge_real_قدر : عام anyedge_قدر {

    عام:
      anyedge_real_قدر() : old_بٹ(0.0) {};
      مجازی ~anyedge_real_قدر() override {};

      فراغ reset() override { old_بٹ = 0.0; }

      فراغ سیٹ(مزدوج ثنائی_ہندسہ) { old_بٹ = ثنائی_ہندسہ; };

      فراغ duplicate(anyedge_قدر*&dup) override;

      منطقي recv_real(مزدوج ثنائی_ہندسہ);

    خاص:
      مزدوج old_بٹ;
};

ساكن anyedge_real_قدر*get_real_قدر(anyedge_قدر*&value)
{
      anyedge_real_قدر*real_قدر = dynamic_cast<anyedge_real_قدر*>(value);
      إذا (!value) {
	    real_قدر = جديد anyedge_real_قدر();
	    مٹاؤ value;
	    value = real_قدر;
      }
      أرجع real_قدر;
}

صنف anyedge_string_قدر : عام anyedge_قدر {

    عام:
      anyedge_string_قدر() {};
      مجازی ~anyedge_string_قدر() override {};

      فراغ reset() override { old_بٹ.clear(); }

      فراغ سیٹ(ثابت std::string&ثنائی_ہندسہ) { old_بٹ = ثنائی_ہندسہ; };

      فراغ duplicate(anyedge_قدر*&dup) override;

      منطقي recv_string(ثابت std::string&ثنائی_ہندسہ);

    خاص:
      std::string old_بٹ;
};

ساكن anyedge_string_قدر*get_string_قدر(anyedge_قدر*&value)
{
      anyedge_string_قدر*string_قدر = dynamic_cast<anyedge_string_قدر*>(value);
      إذا (!value) {
	    string_قدر = جديد anyedge_string_قدر();
	    مٹاؤ value;
	    value = string_قدر;
      }
      أرجع string_قدر;
}

بنية vvp_fun_anyedge_state_s : عام waitable_state_s {
      vvp_fun_anyedge_state_s()
      {
            لكل (بنانشانی idx = 0 ;  idx < 4 ;  idx += 1)
                  آخری_قدر[idx] = 0;
      }

      ~vvp_fun_anyedge_state_s()
      {
            لكل (بنانشانی idx = 0 ;  idx < 4 ;  idx += 1)
                  مٹاؤ آخری_قدر[idx];
      }

      anyedge_قدر *آخری_قدر[4];
      vvp_fun_anyedge_state_s(ثابت vvp_fun_anyedge_state_s&) = مٹاؤ;
      vvp_fun_anyedge_state_s& عامل=(ثابت vvp_fun_anyedge_state_s&) = مٹاؤ;
};

vvp_fun_anyedge::vvp_fun_anyedge()
{
      لكل (بنانشانی idx = 0 ;  idx < 4 ;  idx += 1)
	    آخری_قدر[idx] = 0;
}

vvp_fun_anyedge::~vvp_fun_anyedge()
{
      لكل (بنانشانی idx = 0 ;  idx < 4 ;  idx += 1)
	    مٹاؤ آخری_قدر[idx];
}

فراغ anyedge_vec4_قدر::duplicate(anyedge_قدر*&dup)
{
      anyedge_vec4_قدر*dup_vec4 = get_vec4_قدر(dup);
      أكد(dup_vec4);
      dup_vec4->سیٹ(old_بٹ);
}

منطقي anyedge_vec4_قدر::recv_vec4(ثابت vvp_vector4_t&ثنائی_ہندسہ)
{
      منطقي flag = خطأ;

      إذا (old_بٹ.size() != ثنائی_ہندسہ.size()) {
	    إذا (old_بٹ.size() == 0) {
		    // Special case: If we've not seen any input yet
		    // (old_bits.size()==0) then replace it will a reference
		    // vector that is 'bx. Then compare that with the input
		    // to see if we are processing a change from 'bx.
		  old_بٹ = vvp_vector4_t(ثنائی_ہندسہ.size(), BIT4_X);
		  إذا (old_بٹ.eeq(ثنائی_ہندسہ))
			flag = خطأ;
		  وإلا
			flag = سچ;

	    } وإلا {
		  flag = سچ;
	    }

      } وإلا {
	    لكل (بنانشانی idx = 0 ;  idx < ثنائی_ہندسہ.size() ;  idx += 1) {
		  إذا (old_بٹ.value(idx) != ثنائی_ہندسہ.value(idx)) {
			flag = سچ;
			اكسر;
		  }
	    }
      }

      إذا (flag) {
	    old_بٹ = ثنائی_ہندسہ;
      }

      أرجع flag;
}

منطقي anyedge_vec4_قدر::recv_vec4_pv(ثابت vvp_vector4_t&ثنائی_ہندسہ, بنانشانی ادھار,
				      بنانشانی vwid)
{
      vvp_vector4_t tmp = old_بٹ;
      إذا (tmp.size() == 0)
	    tmp = vvp_vector4_t(vwid, BIT4_Z);
      أكد(ادھار + ثنائی_ہندسہ.size()<= vwid);
      أكد(tmp.size() == vwid);
      tmp.سیٹ_vec(ادھار, ثنائی_ہندسہ);

      أرجع recv_vec4(tmp);
}

فراغ anyedge_real_قدر::duplicate(anyedge_قدر*&dup)
{
      anyedge_real_قدر*dup_real = get_real_قدر(dup);
      أكد(dup_real);
      dup_real->سیٹ(old_بٹ);
}

منطقي anyedge_real_قدر::recv_real(مزدوج ثنائی_ہندسہ)
{
      إذا (old_بٹ != ثنائی_ہندسہ) {
	    old_بٹ = ثنائی_ہندسہ;
            أرجع سچ;
      }
      أرجع خطأ;
}

فراغ anyedge_string_قدر::duplicate(anyedge_قدر*&dup)
{
      anyedge_string_قدر*dup_string = get_string_قدر(dup);
      أكد(dup_string);
      dup_string->سیٹ(old_بٹ);
}

منطقي anyedge_string_قدر::recv_string(ثابت std::string&ثنائی_ہندسہ)
{
      إذا (old_بٹ != ثنائی_ہندسہ) {
	    old_بٹ = ثنائی_ہندسہ;
            أرجع سچ;
      }
      أرجع خطأ;
}

vvp_fun_anyedge_sa::vvp_fun_anyedge_sa()
: دھاگے_2(0)
{
}

vvp_fun_anyedge_sa::~vvp_fun_anyedge_sa()
{
}

vthread_t vvp_fun_anyedge_sa::جوڑو_اڈیک_thread(vthread_t thread)
{
      vthread_t tmp = دھاگے_2;
      دھاگے_2 = thread;

      أرجع tmp;
}

فراغ vvp_fun_anyedge_sa::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                   vvp_context_t)
{
      anyedge_vec4_قدر*value = get_vec4_قدر(آخری_قدر[پورٹ.پورٹ()]);
      أكد(value);
      إذا (value->recv_vec4(ثنائی_ہندسہ)) {
	    لگاتار_اڈیک_دھاگے(دھاگے_2);
	    vvp_net_t*net = پورٹ.ptr();
	    net->send_vec4(ثنائی_ہندسہ, 0);
      }
}

فراغ vvp_fun_anyedge_sa::recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
				      بنانشانی ادھار, بنانشانی vwid, vvp_context_t)
{
      anyedge_vec4_قدر*value = get_vec4_قدر(آخری_قدر[پورٹ.پورٹ()]);
      أكد(value);
      إذا (value->recv_vec4_pv(ثنائی_ہندسہ, ادھار, vwid)) {
	    لگاتار_اڈیک_دھاگے(دھاگے_2);
	    vvp_net_t*net = پورٹ.ptr();
	    net->send_vec4(ثنائی_ہندسہ, 0);
      }
}

فراغ vvp_fun_anyedge_sa::recv_real(vvp_net_ptr_t پورٹ, مزدوج ثنائی_ہندسہ,
                                   vvp_context_t)
{
      anyedge_real_قدر*value = get_real_قدر(آخری_قدر[پورٹ.پورٹ()]);
      أكد(value);
      إذا (value->recv_real(ثنائی_ہندسہ)) {
	    لگاتار_اڈیک_دھاگے(دھاگے_2);
	    vvp_net_t*net = پورٹ.ptr();
	    net->send_vec4(vvp_vector4_t(), 0);
      }
}

فراغ vvp_fun_anyedge_sa::recv_string(vvp_net_ptr_t پورٹ, ثابت std::string&ثنائی_ہندسہ,
				     vvp_context_t)
{
      anyedge_string_قدر*value = get_string_قدر(آخری_قدر[پورٹ.پورٹ()]);
      أكد(value);
      إذا (value->recv_string(ثنائی_ہندسہ)) {
	    لگاتار_اڈیک_دھاگے(دھاگے_2);
	    vvp_net_t*net = پورٹ.ptr();
	    net->send_vec4(vvp_vector4_t(), 0);
      }
}

/*
 * An anyedge receiving an object should do nothing with it, but should
 * trigger waiting threads.
 */
فراغ vvp_fun_anyedge_sa::recv_object(vvp_net_ptr_t پورٹ, vvp_object_t,
				     vvp_context_t)
{
      لگاتار_اڈیک_دھاگے(دھاگے_2);
      vvp_net_t*net = پورٹ.ptr();
      net->send_vec4(vvp_vector4_t(), 0);
}

vvp_fun_anyedge_aa::vvp_fun_anyedge_aa()
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_جوڑو_شے_منزل_context(هذا, context_scope_);
}

vvp_fun_anyedge_aa::~vvp_fun_anyedge_aa()
{
}

فراغ vvp_fun_anyedge_aa::alloc_instance(vvp_context_t context)
{
      vvp_سیٹ_context_شے(context, context_idx_, جديد vvp_fun_anyedge_state_s);
      reset_instance(context);
}

فراغ vvp_fun_anyedge_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
            (vvp_get_context_شے(context, context_idx_));

      state->دھاگے = 0;
      لكل (بنانشانی idx = 0 ;  idx < 4 ;  idx += 1) {
	    إذا (آخری_قدر[idx])
	          آخری_قدر[idx]->duplicate(state->آخری_قدر[idx]);
	    وإلا إذا (state->آخری_قدر[idx])
		  state->آخری_قدر[idx]->reset();
      }
}

#جے_تعریف_شدہ CHECK_WITH_VALGRIND
فراغ vvp_fun_anyedge_aa::free_instance(vvp_context_t context)
{
      vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
            (vvp_get_context_شے(context, context_idx_));
      مٹاؤ state;
}
#جے_انت

vthread_t vvp_fun_anyedge_aa::جوڑو_اڈیک_thread(vthread_t thread)
{
      vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
            (vthread_get_wt_context_شے(context_idx_));

      vthread_t tmp = state->دھاگے;
      state->دھاگے = thread;

      أرجع tmp;
}

فراغ vvp_fun_anyedge_aa::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                   vvp_context_t context)
{
      إذا (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_شے(context, context_idx_));

            anyedge_vec4_قدر*value = get_vec4_قدر(state->آخری_قدر[پورٹ.پورٹ()]);
            أكد(value);
            إذا (value->recv_vec4(ثنائی_ہندسہ)) {
                  لگاتار_اڈیک_دھاگے(state->دھاگے);
                  vvp_net_t*net = پورٹ.ptr();
                  net->send_vec4(ثنائی_ہندسہ, context);
            }
      } وإلا {
            context = context_scope_->جیوندا_contexts;
            طالما (context) {
                  recv_vec4(پورٹ, ثنائی_ہندسہ, context);
                  context = vvp_get_اگلا_context(context);
            }
            anyedge_vec4_قدر*value = get_vec4_قدر(آخری_قدر[پورٹ.پورٹ()]);
            أكد(value);
            value->سیٹ(ثنائی_ہندسہ);
      }
}

فراغ vvp_fun_anyedge_aa::recv_real(vvp_net_ptr_t پورٹ, مزدوج ثنائی_ہندسہ,
                                   vvp_context_t context)
{
      إذا (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_شے(context, context_idx_));

            anyedge_real_قدر*value = get_real_قدر(state->آخری_قدر[پورٹ.پورٹ()]);
            أكد(value);
            إذا (value->recv_real(ثنائی_ہندسہ)) {
                  لگاتار_اڈیک_دھاگے(state->دھاگے);
                  vvp_net_t*net = پورٹ.ptr();
                  net->send_vec4(vvp_vector4_t(), context);
            }
      } وإلا {
            context = context_scope_->جیوندا_contexts;
            طالما (context) {
                  recv_real(پورٹ, ثنائی_ہندسہ, context);
                  context = vvp_get_اگلا_context(context);
            }
            anyedge_real_قدر*value = get_real_قدر(آخری_قدر[پورٹ.پورٹ()]);
            أكد(value);
            value->سیٹ(ثنائی_ہندسہ);
      }
}

فراغ vvp_fun_anyedge_aa::recv_string(vvp_net_ptr_t پورٹ, ثابت std::string&ثنائی_ہندسہ,
				     vvp_context_t context)
{
      إذا (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_شے(context, context_idx_));

            anyedge_string_قدر*value = get_string_قدر(state->آخری_قدر[پورٹ.پورٹ()]);
            أكد(value);
            إذا (value->recv_string(ثنائی_ہندسہ)) {
                  لگاتار_اڈیک_دھاگے(state->دھاگے);
                  vvp_net_t*net = پورٹ.ptr();
                  net->send_vec4(vvp_vector4_t(), context);
            }
      } وإلا {
            context = context_scope_->جیوندا_contexts;
            طالما (context) {
                  recv_string(پورٹ, ثنائی_ہندسہ, context);
                  context = vvp_get_اگلا_context(context);
            }
            anyedge_string_قدر*value = get_string_قدر(آخری_قدر[پورٹ.پورٹ()]);
            أكد(value);
            value->سیٹ(ثنائی_ہندسہ);
      }
}

vvp_fun_event_or::vvp_fun_event_or(vvp_net_t*ادھار_net)
: ادھار_net_2(ادھار_net)
{
}

vvp_fun_event_or::~vvp_fun_event_or()
{
}

vvp_fun_event_or_sa::vvp_fun_event_or_sa(vvp_net_t*ادھار_net)
: vvp_fun_event_or(ادھار_net), دھاگے_2(0)
{
}

vvp_fun_event_or_sa::~vvp_fun_event_or_sa()
{
}

vthread_t vvp_fun_event_or_sa::جوڑو_اڈیک_thread(vthread_t thread)
{
      vthread_t tmp = دھاگے_2;
      دھاگے_2 = thread;

      أرجع tmp;
}

فراغ vvp_fun_event_or_sa::recv_vec4(vvp_net_ptr_t, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                    vvp_context_t)
{
      لگاتار_اڈیک_دھاگے(دھاگے_2);
      ادھار_net_2->send_vec4(ثنائی_ہندسہ, 0);
}

vvp_fun_event_or_aa::vvp_fun_event_or_aa(vvp_net_t*ادھار_net)
: vvp_fun_event_or(ادھار_net)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_جوڑو_شے_منزل_context(هذا, context_scope_);
}

vvp_fun_event_or_aa::~vvp_fun_event_or_aa()
{
}

فراغ vvp_fun_event_or_aa::alloc_instance(vvp_context_t context)
{
      vvp_سیٹ_context_شے(context, context_idx_, جديد waitable_state_s);
}

فراغ vvp_fun_event_or_aa::reset_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_شے(context, context_idx_));

      state->دھاگے = 0;
}

#جے_تعریف_شدہ CHECK_WITH_VALGRIND
فراغ vvp_fun_event_or_aa::free_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_شے(context, context_idx_));
      مٹاؤ state;
}
#جے_انت

vthread_t vvp_fun_event_or_aa::جوڑو_اڈیک_thread(vthread_t thread)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vthread_get_wt_context_شے(context_idx_));

      vthread_t tmp = state->دھاگے;
      state->دھاگے = thread;

      أرجع tmp;
}

فراغ vvp_fun_event_or_aa::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                    vvp_context_t context)
{
      إذا (context) {
            waitable_state_s*state = static_cast<waitable_state_s*>
                  (vvp_get_context_شے(context, context_idx_));

            لگاتار_اڈیک_دھاگے(state->دھاگے);
            ادھار_net_2->send_vec4(ثنائی_ہندسہ, context);
      } وإلا {
            context = context_scope_->جیوندا_contexts;
            طالما (context) {
                  recv_vec4(پورٹ, ثنائی_ہندسہ, context);
                  context = vvp_get_اگلا_context(context);
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
: vvp_named_event(h), دھاگے_2(0)
{
}

vvp_named_event_sa::~vvp_named_event_sa()
{
}

vthread_t vvp_named_event_sa::جوڑو_اڈیک_thread(vthread_t thread)
{
      vthread_t tmp = دھاگے_2;
      دھاگے_2 = thread;

      أرجع tmp;
}

فراغ vvp_named_event_sa::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                   vvp_context_t)
{
      لگاتار_اڈیک_دھاگے(دھاگے_2);
      vvp_net_t*net = پورٹ.ptr();
      net->send_vec4(ثنائی_ہندسہ, 0);

      __vpiNamedEvent*obj = dynamic_cast<__vpiNamedEvent*>(handle_);
      أكد(obj);
      obj->لگاتار_vpi_callbacks();
}

vvp_named_event_aa::vvp_named_event_aa(__vpiHandle*h)
: vvp_named_event(h)
{
      context_idx_ = vpip_جوڑو_شے_منزل_context(هذا, vpip_peek_context_scope());
}

vvp_named_event_aa::~vvp_named_event_aa()
{
}

فراغ vvp_named_event_aa::alloc_instance(vvp_context_t context)
{
      vvp_سیٹ_context_شے(context, context_idx_, جديد waitable_state_s);
}

فراغ vvp_named_event_aa::reset_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_شے(context, context_idx_));

      state->دھاگے = 0;
}

#جے_تعریف_شدہ CHECK_WITH_VALGRIND
فراغ vvp_named_event_aa::free_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_شے(context, context_idx_));
      مٹاؤ state;
}
#جے_انت

vthread_t vvp_named_event_aa::جوڑو_اڈیک_thread(vthread_t thread)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vthread_get_wt_context_شے(context_idx_));

      vthread_t tmp = state->دھاگے;
      state->دھاگے = thread;

      أرجع tmp;
}

فراغ vvp_named_event_aa::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                   vvp_context_t context)
{
      أكد(context);

      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_شے(context, context_idx_));

      لگاتار_اڈیک_دھاگے(state->دھاگے);
      vvp_net_t*net = پورٹ.ptr();
      net->send_vec4(ثنائی_ہندسہ, context);
}

/*
**  Create an event functor
**  edge:  compile_event(label, type, argc, argv, debug_flag)
**  or:    compile_event(label, NULL, argc, argv, debug_flag)
**
**  Named events are handled elsewhere.
*/

ساكن فراغ compile_event_or(حرف*label, بنانشانی argc, بنية symb_s*argv);

فراغ compile_event(حرف*label, حرف*type, بنانشانی argc, بنية symb_s*argv)
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

	    vvp_fun_کنارا::کنارا_t کنارا_type = vvp_کنارا_none;

	    إذا (strcmp(type,"posedge") == 0)
		  کنارا_type = vvp_کنارا_posedge;
	    وإلا إذا (strcmp(type,"negedge") == 0)
		  کنارا_type = vvp_کنارا_negedge;
	    وإلا إذا (strcmp(type,"edge") == 0)
		  کنارا_type = vvp_کنارا_کنارا;

	    أكد(argc <= 4);
	    حرر(type);

            إذا (vpip_peek_current_scope()->is_automatic()) {
                  fun = جديد vvp_fun_کنارا_aa(کنارا_type);
            } وإلا {
                  fun = جديد vvp_fun_کنارا_sa(کنارا_type);
            }

      }

      vvp_net_t* ptr = جديد vvp_net_t;
      ptr->fun = fun;

      define_functor_علامت(label, ptr);
      حرر(label);

      inputs_connect(ptr, argc, argv);
      حرر(argv);
}

ساكن فراغ compile_event_or(حرف*label, بنانشانی argc, بنية symb_s*argv)
{
      vvp_net_t*ادھار_net = جديد vvp_net_t;
      إذا (vpip_peek_current_scope()->is_automatic()) {
            ادھار_net->fun = جديد vvp_fun_event_or_aa(ادھار_net);
      } وإلا {
            ادھار_net->fun = جديد vvp_fun_event_or_sa(ادھار_net);
      }
      define_functor_علامت(label, ادھار_net);
      حرر(label);

	/* This is a simplified version of a wide functor. We don't
	   care about the data values or what port they arrived on,
	   so we can use a single shared functor. */
      vvp_net_t*curr_net = ادھار_net;
      لكل (بنانشانی idx = 0 ;  idx < argc ;  idx += 1) {
	    إذا (idx > 0 && (idx % 4) == 0) {
		  curr_net = جديد vvp_net_t;
		  curr_net->fun = ادھار_net->fun;
	    }
	    داخلہ_connect(curr_net, idx % 4, argv[idx].متن);
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
      define_functor_علامت(label, ptr);
      compile_vpi_علامت(label, obj);
      إذا (! local_flag) vpip_attach_منزل_current_scope(obj);

      حرر(label);
      مٹاؤ[] name;
}

#جے_تعریف_شدہ CHECK_WITH_VALGRIND
فراغ named_event_delete(__vpiHandle*handle)
{
      مٹاؤ dynamic_cast<__vpiNamedEvent *>(handle);
}
#جے_انت
