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

خالی waitable_hooks_s::تسلسل_انتظار_دھاگے(vthread_t&دھاگے)
{
	// Run the non-blocking event controls.
      آخری = &event_ctls;
      برائے (evctl*cur = event_ctls; cur != 0;) {
	    اگر (cur->dec_and_تسلسل()) {
		  evctl*nxt = cur->اگلا_2;
		  حذف cur;
		  cur = nxt;
		  *آخری = cur;
	    } ورنہ {
		  آخری = &(cur->اگلا_2);
		  cur = cur->اگلا_2;
	    }
      }

      vthread_t tmp = دھاگے;
      اگر (tmp == 0) واپس;
      دھاگے = 0;

      vthread_جدول_وقت_list(tmp);
}

evctl::evctl(uint64_t ecount)
{
      ecount_ = ecount;
      اگلا_2 = 0;
}

منطقی evctl::dec_and_تسلسل()
{
      أكد(ecount_ != 0);

      ecount_ -= 1;
      اگر (ecount_ == 0) تسلسل_تسلسل();

      واپس ecount_ == 0;
}

evctl_real::evctl_real(__vpiHandle*handle, دوہرا value,
                       uint64_t ecount)
:evctl(ecount)
{
      handle_ = handle;
      قدر_2 = value;
}

خالی evctl_real::تسلسل_تسلسل()
{
      t_vpi_قدر val;

      val.format = vpiRealVal;
      val.value.real = قدر_2;
      vpi_put_قدر(handle_, &val, 0, vpiNoDelay);
}

خالی جدول_وقت_evctl(__vpiHandle*handle, دوہرا value,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      أكد(ep);
	// Now add this call to the end of the event list.
      *(ep->آخری) = نیا evctl_real(handle, value, ecount);
      ep->آخری = &((*(ep->آخری))->اگلا_2);
}

evctl_سمتیہ::evctl_سمتیہ(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&value,
                           غیرعلامتی off, غیرعلامتی wid, uint64_t ecount)
:evctl(ecount), ptr_(ptr), قدر_2(value)
{
      off_ = off;
      wid_ = wid;
}

خالی evctl_سمتیہ::تسلسل_تسلسل()
{
      اگر (wid_ != 0) {
	    vvp_send_vec4_pv(ptr_, قدر_2, off_, wid_, 0);
      } ورنہ {
	    vvp_send_vec4(ptr_, قدر_2, 0);
      }
}

خالی جدول_وقت_evctl(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&value,
                    غیرعلامتی انحراف_2, غیرعلامتی wid,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      أكد(ep);
	// Now add this call to the end of the event list.
      *(ep->آخری) = نیا evctl_سمتیہ(ptr, value, انحراف_2, wid, ecount);
      ep->آخری = &((*(ep->آخری))->اگلا_2);
}

evctl_array::evctl_array(vvp_array_t حافظہ, غیرعلامتی اشاریہ,
                         ثابت vvp_vector4_t&value, غیرعلامتی off,
                         uint64_t ecount)
:evctl(ecount), قدر_2(value)
{
      mem_ = حافظہ;
      idx_ = اشاریہ;
      off_ = off;
}

خالی evctl_array::تسلسل_تسلسل()
{
      mem_->سیٹ_لفظ(idx_, off_, قدر_2);
}

خالی جدول_وقت_evctl(vvp_array_t حافظہ, غیرعلامتی اشاریہ,
                    ثابت vvp_vector4_t&value, غیرعلامتی انحراف_2,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      أكد(ep);
	// Now add this call to the end of the event list.
      *(ep->آخری) = نیا evctl_array(حافظہ, اشاریہ, value, انحراف_2, ecount);
      ep->آخری = &((*(ep->آخری))->اگلا_2);
}

evctl_array_r::evctl_array_r(vvp_array_t حافظہ, غیرعلامتی اشاریہ,
                             دوہرا value, uint64_t ecount)
:evctl(ecount)
{
      mem_ = حافظہ;
      idx_ = اشاریہ;
      قدر_2 = value;
}

خالی evctl_array_r::تسلسل_تسلسل()
{
      mem_->سیٹ_لفظ(idx_, قدر_2);
}

خالی جدول_وقت_evctl(vvp_array_t حافظہ, غیرعلامتی اشاریہ,
                    دوہرا value,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      أكد(ep);
	// Now add this call to the end of the event list.
      *(ep->آخری) = نیا evctl_array_r(حافظہ, اشاریہ, value, ecount);
      ep->آخری = &((*(ep->آخری))->اگلا_2);
}

اندرونی vvp_fun_کنارہ::کنارہ_t VVP_کنارہ(vvp_bit4_t from, vvp_bit4_t to)
{
      واپس 1 << ((from << 2) | to);
}

ثابت vvp_fun_کنارہ::کنارہ_t vvp_کنارہ_posedge
      = VVP_کنارہ(BIT4_0,BIT4_1)
      | VVP_کنارہ(BIT4_0,BIT4_X)
      | VVP_کنارہ(BIT4_0,BIT4_Z)
      | VVP_کنارہ(BIT4_X,BIT4_1)
      | VVP_کنارہ(BIT4_Z,BIT4_1)
      ;

ثابت vvp_fun_کنارہ::کنارہ_t vvp_کنارہ_negedge
      = VVP_کنارہ(BIT4_1,BIT4_0)
      | VVP_کنارہ(BIT4_1,BIT4_X)
      | VVP_کنارہ(BIT4_1,BIT4_Z)
      | VVP_کنارہ(BIT4_X,BIT4_0)
      | VVP_کنارہ(BIT4_Z,BIT4_0)
      ;

ثابت vvp_fun_کنارہ::کنارہ_t vvp_کنارہ_کنارہ
      = VVP_کنارہ(BIT4_0,BIT4_1)
      | VVP_کنارہ(BIT4_1,BIT4_0)
      | VVP_کنارہ(BIT4_0,BIT4_X)
      | VVP_کنارہ(BIT4_X,BIT4_0)
      | VVP_کنارہ(BIT4_0,BIT4_Z)
      | VVP_کنارہ(BIT4_Z,BIT4_0)
      | VVP_کنارہ(BIT4_X,BIT4_1)
      | VVP_کنارہ(BIT4_1,BIT4_X)
      | VVP_کنارہ(BIT4_Z,BIT4_1)
      | VVP_کنارہ(BIT4_1,BIT4_Z)
      ;

ثابت vvp_fun_کنارہ::کنارہ_t vvp_کنارہ_none    = 0;

ساخت vvp_fun_کنارہ_state_s : عوامی waitable_state_s {
      vvp_fun_کنارہ_state_s()
      {
            برائے (غیرعلامتی idx = 0 ;  idx < 4 ;  idx += 1)
                  بٹ[idx] = BIT4_X;
      }

      vvp_bit4_t بٹ[4];
};

vvp_fun_کنارہ::vvp_fun_کنارہ(کنارہ_t e)
: کنارہ_2(e)
{
      برائے (غیرعلامتی idx = 0 ;  idx < 4 ;  idx += 1)
            بٹ_2[idx] = BIT4_X;
}

vvp_fun_کنارہ::~vvp_fun_کنارہ()
{
}

منطقی vvp_fun_کنارہ::recv_vec4_(ثابت vvp_vector4_t&ثنائی_ہندسہ,
                              vvp_bit4_t&old_ثنائی_ہندسہ, vthread_t&دھاگے)
{
	/* See what kind of edge this represents. */
      کنارہ_t نقاب = VVP_کنارہ(old_ثنائی_ہندسہ, ثنائی_ہندسہ.value(0));

	/* Save the current input for the next time around. */
      old_ثنائی_ہندسہ = ثنائی_ہندسہ.value(0);

      اگر ((کنارہ_2 == vvp_کنارہ_none) || (کنارہ_2 & نقاب)) {
	    تسلسل_انتظار_دھاگے(دھاگے);
            واپس سچ;
      }
      واپس جھوٹ;
}

vvp_fun_کنارہ_sa::vvp_fun_کنارہ_sa(کنارہ_t e)
: vvp_fun_کنارہ(e), دھاگے_2(0)
{
}

vvp_fun_کنارہ_sa::~vvp_fun_کنارہ_sa()
{
}

vthread_t vvp_fun_کنارہ_sa::جوڑو_انتظار_thread(vthread_t thread)
{
      vthread_t tmp = دھاگے_2;
      دھاگے_2 = thread;

      واپس tmp;
}

خالی vvp_fun_کنارہ_sa::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                vvp_context_t)
{
      اگر (recv_vec4_(ثنائی_ہندسہ, بٹ_2[پورٹ.پورٹ()], دھاگے_2)) {
	    vvp_net_t*net = پورٹ.ptr();
	    net->send_vec4(ثنائی_ہندسہ, 0);
      }
}

خالی vvp_fun_کنارہ_sa::recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
				   غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t)
{
      أكد(بنیاد == 0);
      اگر (recv_vec4_(ثنائی_ہندسہ, بٹ_2[پورٹ.پورٹ()], دھاگے_2)) {
	    vvp_net_t*net = پورٹ.ptr();
	    net->send_vec4_pv(ثنائی_ہندسہ, بنیاد, vwid, 0);
      }
}

vvp_fun_کنارہ_aa::vvp_fun_کنارہ_aa(کنارہ_t e)
: vvp_fun_کنارہ(e)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_جوڑو_شے_مقصد_context(یہ, context_scope_);
}

vvp_fun_کنارہ_aa::~vvp_fun_کنارہ_aa()
{
}

خالی vvp_fun_کنارہ_aa::alloc_instance(vvp_context_t context)
{
      vvp_سیٹ_context_شے(context, context_idx_, نیا vvp_fun_کنارہ_state_s);
      reset_instance(context);
}

خالی vvp_fun_کنارہ_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_کنارہ_state_s*state = static_cast<vvp_fun_کنارہ_state_s*>
            (vvp_get_context_شے(context, context_idx_));

      state->دھاگے = 0;
      برائے (غیرعلامتی idx = 0 ;  idx < 4 ;  idx += 1)
            state->بٹ[idx] = بٹ_2[idx];
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
خالی vvp_fun_کنارہ_aa::free_instance(vvp_context_t context)
{
      vvp_fun_کنارہ_state_s*state = static_cast<vvp_fun_کنارہ_state_s*>
            (vvp_get_context_شے(context, context_idx_));
      حذف state;
}
#اگر_ختم

vthread_t vvp_fun_کنارہ_aa::جوڑو_انتظار_thread(vthread_t thread)
{
      vvp_fun_کنارہ_state_s*state = static_cast<vvp_fun_کنارہ_state_s*>
            (vthread_get_wt_context_شے(context_idx_));

      vthread_t tmp = state->دھاگے;
      state->دھاگے = thread;

      واپس tmp;
}

خالی vvp_fun_کنارہ_aa::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                vvp_context_t context)
{
      اگر (context) {
            vvp_fun_کنارہ_state_s*state = static_cast<vvp_fun_کنارہ_state_s*>
                  (vvp_get_context_شے(context, context_idx_));

            اگر (recv_vec4_(ثنائی_ہندسہ, state->بٹ[پورٹ.پورٹ()], state->دھاگے)) {
                  vvp_net_t*net = پورٹ.ptr();
                  net->send_vec4(ثنائی_ہندسہ, context);
            }
      } ورنہ {
            context = context_scope_->زندہ_contexts;
            جب (context) {
                  recv_vec4(پورٹ, ثنائی_ہندسہ, context);
                  context = vvp_get_اگلا_context(context);
            }
            بٹ_2[پورٹ.پورٹ()] = ثنائی_ہندسہ.value(0);
      }
}

جماعت anyedge_قدر {

    عوامی:
      anyedge_قدر() {};
      مجازی ~anyedge_قدر() {};

      مجازی خالی reset() = 0;

      مجازی خالی duplicate(anyedge_قدر*&dup) = 0;
};

جماعت anyedge_vec4_قدر : عوامی anyedge_قدر {

    عوامی:
      anyedge_vec4_قدر() {};
      مجازی ~anyedge_vec4_قدر() override {};

      خالی reset() override { old_بٹ.سیٹ_مقصد_x(); }

      خالی سیٹ(ثابت vvp_vector4_t&ثنائی_ہندسہ) { old_بٹ = ثنائی_ہندسہ; };

      خالی duplicate(anyedge_قدر*&dup) override;

      منطقی recv_vec4(ثابت vvp_vector4_t&ثنائی_ہندسہ);

      منطقی recv_vec4_pv(ثابت vvp_vector4_t&ثنائی_ہندسہ, غیرعلامتی بنیاد,
			غیرعلامتی vwid);

    نجی:
      vvp_vector4_t old_بٹ;
};

ساکن anyedge_vec4_قدر*get_vec4_قدر(anyedge_قدر*&value)
{
      anyedge_vec4_قدر*vec4_قدر = dynamic_cast<anyedge_vec4_قدر*>(value);
      اگر (!value) {
	    vec4_قدر = نیا anyedge_vec4_قدر();
	    حذف value;
	    value = vec4_قدر;
      }
      واپس vec4_قدر;
}

جماعت anyedge_real_قدر : عوامی anyedge_قدر {

    عوامی:
      anyedge_real_قدر() : old_بٹ(0.0) {};
      مجازی ~anyedge_real_قدر() override {};

      خالی reset() override { old_بٹ = 0.0; }

      خالی سیٹ(دوہرا ثنائی_ہندسہ) { old_بٹ = ثنائی_ہندسہ; };

      خالی duplicate(anyedge_قدر*&dup) override;

      منطقی recv_real(دوہرا ثنائی_ہندسہ);

    نجی:
      دوہرا old_بٹ;
};

ساکن anyedge_real_قدر*get_real_قدر(anyedge_قدر*&value)
{
      anyedge_real_قدر*real_قدر = dynamic_cast<anyedge_real_قدر*>(value);
      اگر (!value) {
	    real_قدر = نیا anyedge_real_قدر();
	    حذف value;
	    value = real_قدر;
      }
      واپس real_قدر;
}

جماعت anyedge_string_قدر : عوامی anyedge_قدر {

    عوامی:
      anyedge_string_قدر() {};
      مجازی ~anyedge_string_قدر() override {};

      خالی reset() override { old_بٹ.clear(); }

      خالی سیٹ(ثابت std::string&ثنائی_ہندسہ) { old_بٹ = ثنائی_ہندسہ; };

      خالی duplicate(anyedge_قدر*&dup) override;

      منطقی recv_string(ثابت std::string&ثنائی_ہندسہ);

    نجی:
      std::string old_بٹ;
};

ساکن anyedge_string_قدر*get_string_قدر(anyedge_قدر*&value)
{
      anyedge_string_قدر*string_قدر = dynamic_cast<anyedge_string_قدر*>(value);
      اگر (!value) {
	    string_قدر = نیا anyedge_string_قدر();
	    حذف value;
	    value = string_قدر;
      }
      واپس string_قدر;
}

ساخت vvp_fun_anyedge_state_s : عوامی waitable_state_s {
      vvp_fun_anyedge_state_s()
      {
            برائے (غیرعلامتی idx = 0 ;  idx < 4 ;  idx += 1)
                  آخری_قدر[idx] = 0;
      }

      ~vvp_fun_anyedge_state_s()
      {
            برائے (غیرعلامتی idx = 0 ;  idx < 4 ;  idx += 1)
                  حذف آخری_قدر[idx];
      }

      anyedge_قدر *آخری_قدر[4];
      vvp_fun_anyedge_state_s(ثابت vvp_fun_anyedge_state_s&) = حذف;
      vvp_fun_anyedge_state_s& عامل=(ثابت vvp_fun_anyedge_state_s&) = حذف;
};

vvp_fun_anyedge::vvp_fun_anyedge()
{
      برائے (غیرعلامتی idx = 0 ;  idx < 4 ;  idx += 1)
	    آخری_قدر[idx] = 0;
}

vvp_fun_anyedge::~vvp_fun_anyedge()
{
      برائے (غیرعلامتی idx = 0 ;  idx < 4 ;  idx += 1)
	    حذف آخری_قدر[idx];
}

خالی anyedge_vec4_قدر::duplicate(anyedge_قدر*&dup)
{
      anyedge_vec4_قدر*dup_vec4 = get_vec4_قدر(dup);
      أكد(dup_vec4);
      dup_vec4->سیٹ(old_بٹ);
}

منطقی anyedge_vec4_قدر::recv_vec4(ثابت vvp_vector4_t&ثنائی_ہندسہ)
{
      منطقی flag = جھوٹ;

      اگر (old_بٹ.size() != ثنائی_ہندسہ.size()) {
	    اگر (old_بٹ.size() == 0) {
		    // Special case: If we've not seen any input yet
		    // (old_bits.size()==0) then replace it will a reference
		    // vector that is 'bx. Then compare that with the input
		    // to see if we are processing a change from 'bx.
		  old_بٹ = vvp_vector4_t(ثنائی_ہندسہ.size(), BIT4_X);
		  اگر (old_بٹ.eeq(ثنائی_ہندسہ))
			flag = جھوٹ;
		  ورنہ
			flag = سچ;

	    } ورنہ {
		  flag = سچ;
	    }

      } ورنہ {
	    برائے (غیرعلامتی idx = 0 ;  idx < ثنائی_ہندسہ.size() ;  idx += 1) {
		  اگر (old_بٹ.value(idx) != ثنائی_ہندسہ.value(idx)) {
			flag = سچ;
			توڑو;
		  }
	    }
      }

      اگر (flag) {
	    old_بٹ = ثنائی_ہندسہ;
      }

      واپس flag;
}

منطقی anyedge_vec4_قدر::recv_vec4_pv(ثابت vvp_vector4_t&ثنائی_ہندسہ, غیرعلامتی بنیاد,
				      غیرعلامتی vwid)
{
      vvp_vector4_t tmp = old_بٹ;
      اگر (tmp.size() == 0)
	    tmp = vvp_vector4_t(vwid, BIT4_Z);
      أكد(بنیاد + ثنائی_ہندسہ.size()<= vwid);
      أكد(tmp.size() == vwid);
      tmp.سیٹ_vec(بنیاد, ثنائی_ہندسہ);

      واپس recv_vec4(tmp);
}

خالی anyedge_real_قدر::duplicate(anyedge_قدر*&dup)
{
      anyedge_real_قدر*dup_real = get_real_قدر(dup);
      أكد(dup_real);
      dup_real->سیٹ(old_بٹ);
}

منطقی anyedge_real_قدر::recv_real(دوہرا ثنائی_ہندسہ)
{
      اگر (old_بٹ != ثنائی_ہندسہ) {
	    old_بٹ = ثنائی_ہندسہ;
            واپس سچ;
      }
      واپس جھوٹ;
}

خالی anyedge_string_قدر::duplicate(anyedge_قدر*&dup)
{
      anyedge_string_قدر*dup_string = get_string_قدر(dup);
      أكد(dup_string);
      dup_string->سیٹ(old_بٹ);
}

منطقی anyedge_string_قدر::recv_string(ثابت std::string&ثنائی_ہندسہ)
{
      اگر (old_بٹ != ثنائی_ہندسہ) {
	    old_بٹ = ثنائی_ہندسہ;
            واپس سچ;
      }
      واپس جھوٹ;
}

vvp_fun_anyedge_sa::vvp_fun_anyedge_sa()
: دھاگے_2(0)
{
}

vvp_fun_anyedge_sa::~vvp_fun_anyedge_sa()
{
}

vthread_t vvp_fun_anyedge_sa::جوڑو_انتظار_thread(vthread_t thread)
{
      vthread_t tmp = دھاگے_2;
      دھاگے_2 = thread;

      واپس tmp;
}

خالی vvp_fun_anyedge_sa::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                   vvp_context_t)
{
      anyedge_vec4_قدر*value = get_vec4_قدر(آخری_قدر[پورٹ.پورٹ()]);
      أكد(value);
      اگر (value->recv_vec4(ثنائی_ہندسہ)) {
	    تسلسل_انتظار_دھاگے(دھاگے_2);
	    vvp_net_t*net = پورٹ.ptr();
	    net->send_vec4(ثنائی_ہندسہ, 0);
      }
}

خالی vvp_fun_anyedge_sa::recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
				      غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t)
{
      anyedge_vec4_قدر*value = get_vec4_قدر(آخری_قدر[پورٹ.پورٹ()]);
      أكد(value);
      اگر (value->recv_vec4_pv(ثنائی_ہندسہ, بنیاد, vwid)) {
	    تسلسل_انتظار_دھاگے(دھاگے_2);
	    vvp_net_t*net = پورٹ.ptr();
	    net->send_vec4(ثنائی_ہندسہ, 0);
      }
}

خالی vvp_fun_anyedge_sa::recv_real(vvp_net_ptr_t پورٹ, دوہرا ثنائی_ہندسہ,
                                   vvp_context_t)
{
      anyedge_real_قدر*value = get_real_قدر(آخری_قدر[پورٹ.پورٹ()]);
      أكد(value);
      اگر (value->recv_real(ثنائی_ہندسہ)) {
	    تسلسل_انتظار_دھاگے(دھاگے_2);
	    vvp_net_t*net = پورٹ.ptr();
	    net->send_vec4(vvp_vector4_t(), 0);
      }
}

خالی vvp_fun_anyedge_sa::recv_string(vvp_net_ptr_t پورٹ, ثابت std::string&ثنائی_ہندسہ,
				     vvp_context_t)
{
      anyedge_string_قدر*value = get_string_قدر(آخری_قدر[پورٹ.پورٹ()]);
      أكد(value);
      اگر (value->recv_string(ثنائی_ہندسہ)) {
	    تسلسل_انتظار_دھاگے(دھاگے_2);
	    vvp_net_t*net = پورٹ.ptr();
	    net->send_vec4(vvp_vector4_t(), 0);
      }
}

/*
 * An anyedge receiving an object should do nothing with it, but should
 * trigger waiting threads.
 */
خالی vvp_fun_anyedge_sa::recv_object(vvp_net_ptr_t پورٹ, vvp_object_t,
				     vvp_context_t)
{
      تسلسل_انتظار_دھاگے(دھاگے_2);
      vvp_net_t*net = پورٹ.ptr();
      net->send_vec4(vvp_vector4_t(), 0);
}

vvp_fun_anyedge_aa::vvp_fun_anyedge_aa()
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_جوڑو_شے_مقصد_context(یہ, context_scope_);
}

vvp_fun_anyedge_aa::~vvp_fun_anyedge_aa()
{
}

خالی vvp_fun_anyedge_aa::alloc_instance(vvp_context_t context)
{
      vvp_سیٹ_context_شے(context, context_idx_, نیا vvp_fun_anyedge_state_s);
      reset_instance(context);
}

خالی vvp_fun_anyedge_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
            (vvp_get_context_شے(context, context_idx_));

      state->دھاگے = 0;
      برائے (غیرعلامتی idx = 0 ;  idx < 4 ;  idx += 1) {
	    اگر (آخری_قدر[idx])
	          آخری_قدر[idx]->duplicate(state->آخری_قدر[idx]);
	    ورنہ اگر (state->آخری_قدر[idx])
		  state->آخری_قدر[idx]->reset();
      }
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
خالی vvp_fun_anyedge_aa::free_instance(vvp_context_t context)
{
      vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
            (vvp_get_context_شے(context, context_idx_));
      حذف state;
}
#اگر_ختم

vthread_t vvp_fun_anyedge_aa::جوڑو_انتظار_thread(vthread_t thread)
{
      vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
            (vthread_get_wt_context_شے(context_idx_));

      vthread_t tmp = state->دھاگے;
      state->دھاگے = thread;

      واپس tmp;
}

خالی vvp_fun_anyedge_aa::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                   vvp_context_t context)
{
      اگر (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_شے(context, context_idx_));

            anyedge_vec4_قدر*value = get_vec4_قدر(state->آخری_قدر[پورٹ.پورٹ()]);
            أكد(value);
            اگر (value->recv_vec4(ثنائی_ہندسہ)) {
                  تسلسل_انتظار_دھاگے(state->دھاگے);
                  vvp_net_t*net = پورٹ.ptr();
                  net->send_vec4(ثنائی_ہندسہ, context);
            }
      } ورنہ {
            context = context_scope_->زندہ_contexts;
            جب (context) {
                  recv_vec4(پورٹ, ثنائی_ہندسہ, context);
                  context = vvp_get_اگلا_context(context);
            }
            anyedge_vec4_قدر*value = get_vec4_قدر(آخری_قدر[پورٹ.پورٹ()]);
            أكد(value);
            value->سیٹ(ثنائی_ہندسہ);
      }
}

خالی vvp_fun_anyedge_aa::recv_real(vvp_net_ptr_t پورٹ, دوہرا ثنائی_ہندسہ,
                                   vvp_context_t context)
{
      اگر (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_شے(context, context_idx_));

            anyedge_real_قدر*value = get_real_قدر(state->آخری_قدر[پورٹ.پورٹ()]);
            أكد(value);
            اگر (value->recv_real(ثنائی_ہندسہ)) {
                  تسلسل_انتظار_دھاگے(state->دھاگے);
                  vvp_net_t*net = پورٹ.ptr();
                  net->send_vec4(vvp_vector4_t(), context);
            }
      } ورنہ {
            context = context_scope_->زندہ_contexts;
            جب (context) {
                  recv_real(پورٹ, ثنائی_ہندسہ, context);
                  context = vvp_get_اگلا_context(context);
            }
            anyedge_real_قدر*value = get_real_قدر(آخری_قدر[پورٹ.پورٹ()]);
            أكد(value);
            value->سیٹ(ثنائی_ہندسہ);
      }
}

خالی vvp_fun_anyedge_aa::recv_string(vvp_net_ptr_t پورٹ, ثابت std::string&ثنائی_ہندسہ,
				     vvp_context_t context)
{
      اگر (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_شے(context, context_idx_));

            anyedge_string_قدر*value = get_string_قدر(state->آخری_قدر[پورٹ.پورٹ()]);
            أكد(value);
            اگر (value->recv_string(ثنائی_ہندسہ)) {
                  تسلسل_انتظار_دھاگے(state->دھاگے);
                  vvp_net_t*net = پورٹ.ptr();
                  net->send_vec4(vvp_vector4_t(), context);
            }
      } ورنہ {
            context = context_scope_->زندہ_contexts;
            جب (context) {
                  recv_string(پورٹ, ثنائی_ہندسہ, context);
                  context = vvp_get_اگلا_context(context);
            }
            anyedge_string_قدر*value = get_string_قدر(آخری_قدر[پورٹ.پورٹ()]);
            أكد(value);
            value->سیٹ(ثنائی_ہندسہ);
      }
}

vvp_fun_event_or::vvp_fun_event_or(vvp_net_t*بنیاد_net)
: بنیاد_net_2(بنیاد_net)
{
}

vvp_fun_event_or::~vvp_fun_event_or()
{
}

vvp_fun_event_or_sa::vvp_fun_event_or_sa(vvp_net_t*بنیاد_net)
: vvp_fun_event_or(بنیاد_net), دھاگے_2(0)
{
}

vvp_fun_event_or_sa::~vvp_fun_event_or_sa()
{
}

vthread_t vvp_fun_event_or_sa::جوڑو_انتظار_thread(vthread_t thread)
{
      vthread_t tmp = دھاگے_2;
      دھاگے_2 = thread;

      واپس tmp;
}

خالی vvp_fun_event_or_sa::recv_vec4(vvp_net_ptr_t, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                    vvp_context_t)
{
      تسلسل_انتظار_دھاگے(دھاگے_2);
      بنیاد_net_2->send_vec4(ثنائی_ہندسہ, 0);
}

vvp_fun_event_or_aa::vvp_fun_event_or_aa(vvp_net_t*بنیاد_net)
: vvp_fun_event_or(بنیاد_net)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_جوڑو_شے_مقصد_context(یہ, context_scope_);
}

vvp_fun_event_or_aa::~vvp_fun_event_or_aa()
{
}

خالی vvp_fun_event_or_aa::alloc_instance(vvp_context_t context)
{
      vvp_سیٹ_context_شے(context, context_idx_, نیا waitable_state_s);
}

خالی vvp_fun_event_or_aa::reset_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_شے(context, context_idx_));

      state->دھاگے = 0;
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
خالی vvp_fun_event_or_aa::free_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_شے(context, context_idx_));
      حذف state;
}
#اگر_ختم

vthread_t vvp_fun_event_or_aa::جوڑو_انتظار_thread(vthread_t thread)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vthread_get_wt_context_شے(context_idx_));

      vthread_t tmp = state->دھاگے;
      state->دھاگے = thread;

      واپس tmp;
}

خالی vvp_fun_event_or_aa::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                    vvp_context_t context)
{
      اگر (context) {
            waitable_state_s*state = static_cast<waitable_state_s*>
                  (vvp_get_context_شے(context, context_idx_));

            تسلسل_انتظار_دھاگے(state->دھاگے);
            بنیاد_net_2->send_vec4(ثنائی_ہندسہ, context);
      } ورنہ {
            context = context_scope_->زندہ_contexts;
            جب (context) {
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

vthread_t vvp_named_event_sa::جوڑو_انتظار_thread(vthread_t thread)
{
      vthread_t tmp = دھاگے_2;
      دھاگے_2 = thread;

      واپس tmp;
}

خالی vvp_named_event_sa::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                   vvp_context_t)
{
      تسلسل_انتظار_دھاگے(دھاگے_2);
      vvp_net_t*net = پورٹ.ptr();
      net->send_vec4(ثنائی_ہندسہ, 0);

      __vpiNamedEvent*obj = dynamic_cast<__vpiNamedEvent*>(handle_);
      أكد(obj);
      obj->تسلسل_vpi_callbacks();
}

vvp_named_event_aa::vvp_named_event_aa(__vpiHandle*h)
: vvp_named_event(h)
{
      context_idx_ = vpip_جوڑو_شے_مقصد_context(یہ, vpip_peek_context_scope());
}

vvp_named_event_aa::~vvp_named_event_aa()
{
}

خالی vvp_named_event_aa::alloc_instance(vvp_context_t context)
{
      vvp_سیٹ_context_شے(context, context_idx_, نیا waitable_state_s);
}

خالی vvp_named_event_aa::reset_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_شے(context, context_idx_));

      state->دھاگے = 0;
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
خالی vvp_named_event_aa::free_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_شے(context, context_idx_));
      حذف state;
}
#اگر_ختم

vthread_t vvp_named_event_aa::جوڑو_انتظار_thread(vthread_t thread)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vthread_get_wt_context_شے(context_idx_));

      vthread_t tmp = state->دھاگے;
      state->دھاگے = thread;

      واپس tmp;
}

خالی vvp_named_event_aa::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                   vvp_context_t context)
{
      أكد(context);

      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_شے(context, context_idx_));

      تسلسل_انتظار_دھاگے(state->دھاگے);
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

ساکن خالی compile_event_or(حرف*label, غیرعلامتی argc, ساخت symb_s*argv);

خالی compile_event(حرف*label, حرف*type, غیرعلامتی argc, ساخت symb_s*argv)
{
      vvp_net_fun_t*fun = 0;

      اگر (type == 0) {
	    compile_event_or(label, argc, argv);
	    واپس;
      }

      اگر (strcmp(type,"anyedge") == 0) {

	    آزاد(type);

            اگر (vpip_peek_current_scope()->is_automatic()) {
                  fun = نیا vvp_fun_anyedge_aa;
            } ورنہ {
                  fun = نیا vvp_fun_anyedge_sa;
            }

      } ورنہ {

	    vvp_fun_کنارہ::کنارہ_t کنارہ_type = vvp_کنارہ_none;

	    اگر (strcmp(type,"posedge") == 0)
		  کنارہ_type = vvp_کنارہ_posedge;
	    ورنہ اگر (strcmp(type,"negedge") == 0)
		  کنارہ_type = vvp_کنارہ_negedge;
	    ورنہ اگر (strcmp(type,"edge") == 0)
		  کنارہ_type = vvp_کنارہ_کنارہ;

	    أكد(argc <= 4);
	    آزاد(type);

            اگر (vpip_peek_current_scope()->is_automatic()) {
                  fun = نیا vvp_fun_کنارہ_aa(کنارہ_type);
            } ورنہ {
                  fun = نیا vvp_fun_کنارہ_sa(کنارہ_type);
            }

      }

      vvp_net_t* ptr = نیا vvp_net_t;
      ptr->fun = fun;

      define_functor_علامت(label, ptr);
      آزاد(label);

      inputs_connect(ptr, argc, argv);
      آزاد(argv);
}

ساکن خالی compile_event_or(حرف*label, غیرعلامتی argc, ساخت symb_s*argv)
{
      vvp_net_t*بنیاد_net = نیا vvp_net_t;
      اگر (vpip_peek_current_scope()->is_automatic()) {
            بنیاد_net->fun = نیا vvp_fun_event_or_aa(بنیاد_net);
      } ورنہ {
            بنیاد_net->fun = نیا vvp_fun_event_or_sa(بنیاد_net);
      }
      define_functor_علامت(label, بنیاد_net);
      آزاد(label);

	/* This is a simplified version of a wide functor. We don't
	   care about the data values or what port they arrived on,
	   so we can use a single shared functor. */
      vvp_net_t*curr_net = بنیاد_net;
      برائے (غیرعلامتی idx = 0 ;  idx < argc ;  idx += 1) {
	    اگر (idx > 0 && (idx % 4) == 0) {
		  curr_net = نیا vvp_net_t;
		  curr_net->fun = بنیاد_net->fun;
	    }
	    داخلہ_connect(curr_net, idx % 4, argv[idx].متن);
      }
      آزاد(argv);
}

/*
 * This handles the compile of named events. This functor has no
 * inputs, it is only accessed by behavioral trigger statements, which
 * in vvp are %set instructions.
 */
خالی compile_named_event(حرف*label, حرف*name, منطقی local_flag)
{
      vvp_net_t*ptr = نیا vvp_net_t;

      vpiHandle obj = vpip_make_named_event(name, ptr);

      اگر (vpip_peek_current_scope()->is_automatic()) {
            ptr->fun = نیا vvp_named_event_aa(obj);
      } ورنہ {
            ptr->fun = نیا vvp_named_event_sa(obj);
      }
      define_functor_علامت(label, ptr);
      compile_vpi_علامت(label, obj);
      اگر (! local_flag) vpip_attach_مقصد_current_scope(obj);

      آزاد(label);
      حذف[] name;
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
خالی named_event_delete(__vpiHandle*handle)
{
      حذف dynamic_cast<__vpiNamedEvent *>(handle);
}
#اگر_ختم
