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

ਖਾਲੀ waitable_hooks_s::ਲੜੀਵਾਰ_ਉਡੀਕ_ਧਾਗੇ(vthread_t&ਧਾਗੇ)
{
	// Run the non-blocking event controls.
      ਆਖਰੀ = &event_ctls;
      ਲਈ (evctl*cur = event_ctls; cur != 0;) {
	    ਜੇ (cur->dec_and_ਲੜੀਵਾਰ()) {
		  evctl*nxt = cur->ਅਗਲਾ_2;
		  ਮਿਟਾਓ cur;
		  cur = nxt;
		  *ਆਖਰੀ = cur;
	    } ਨਹੀਂਤਾਂ {
		  ਆਖਰੀ = &(cur->ਅਗਲਾ_2);
		  cur = cur->ਅਗਲਾ_2;
	    }
      }

      vthread_t tmp = ਧਾਗੇ;
      ਜੇ (tmp == 0) ਵਾਪਸ;
      ਧਾਗੇ = 0;

      vthread_ਸਮਾਂ_ਸਾਰਣੀ_list(tmp);
}

evctl::evctl(uint64_t ecount)
{
      ecount_ = ecount;
      ਅਗਲਾ_2 = 0;
}

ਤਾਰਕਿਕ evctl::dec_and_ਲੜੀਵਾਰ()
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ecount_ != 0);

      ecount_ -= 1;
      ਜੇ (ecount_ == 0) ਲੜੀਵਾਰ_ਲੜੀਵਾਰ();

      ਵਾਪਸ ecount_ == 0;
}

evctl_real::evctl_real(__vpiHandle*handle, ਦੁੱਗਣਾ value,
                       uint64_t ecount)
:evctl(ecount)
{
      handle_ = handle;
      ਮੁੱਲ_2 = value;
}

ਖਾਲੀ evctl_real::ਲੜੀਵਾਰ_ਲੜੀਵਾਰ()
{
      t_vpi_ਮੁੱਲ val;

      val.format = vpiRealVal;
      val.value.real = ਮੁੱਲ_2;
      vpi_put_ਮੁੱਲ(handle_, &val, 0, vpiNoDelay);
}

ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_evctl(__vpiHandle*handle, ਦੁੱਗਣਾ value,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      ਪ੍ਰਤਿਜ੍ਞਾ(ep);
	// Now add this call to the end of the event list.
      *(ep->ਆਖਰੀ) = ਨਵਾਂ evctl_real(handle, value, ecount);
      ep->ਆਖਰੀ = &((*(ep->ਆਖਰੀ))->ਅਗਲਾ_2);
}

evctl_ਸਦਿਸ਼::evctl_ਸਦਿਸ਼(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&value,
                           ਬਿਨਾਨਿਸ਼ਾਨੀ off, ਬਿਨਾਨਿਸ਼ਾਨੀ wid, uint64_t ecount)
:evctl(ecount), ptr_(ptr), ਮੁੱਲ_2(value)
{
      off_ = off;
      wid_ = wid;
}

ਖਾਲੀ evctl_ਸਦਿਸ਼::ਲੜੀਵਾਰ_ਲੜੀਵਾਰ()
{
      ਜੇ (wid_ != 0) {
	    vvp_send_vec4_pv(ptr_, ਮੁੱਲ_2, off_, wid_, 0);
      } ਨਹੀਂਤਾਂ {
	    vvp_send_vec4(ptr_, ਮੁੱਲ_2, 0);
      }
}

ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_evctl(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&value,
                    ਬਿਨਾਨਿਸ਼ਾਨੀ ਵਿੱਥ_2, ਬਿਨਾਨਿਸ਼ਾਨੀ wid,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      ਪ੍ਰਤਿਜ੍ਞਾ(ep);
	// Now add this call to the end of the event list.
      *(ep->ਆਖਰੀ) = ਨਵਾਂ evctl_ਸਦਿਸ਼(ptr, value, ਵਿੱਥ_2, wid, ecount);
      ep->ਆਖਰੀ = &((*(ep->ਆਖਰੀ))->ਅਗਲਾ_2);
}

evctl_array::evctl_array(vvp_array_t ਸਮ੍ਰਿਤੀ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੂਚਕਾਂਕ,
                         ਅਚਲ vvp_vector4_t&value, ਬਿਨਾਨਿਸ਼ਾਨੀ off,
                         uint64_t ecount)
:evctl(ecount), ਮੁੱਲ_2(value)
{
      mem_ = ਸਮ੍ਰਿਤੀ;
      idx_ = ਸੂਚਕਾਂਕ;
      off_ = off;
}

ਖਾਲੀ evctl_array::ਲੜੀਵਾਰ_ਲੜੀਵਾਰ()
{
      mem_->ਸੈੱਟ_ਸ਼ਬਦ(idx_, off_, ਮੁੱਲ_2);
}

ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_evctl(vvp_array_t ਸਮ੍ਰਿਤੀ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੂਚਕਾਂਕ,
                    ਅਚਲ vvp_vector4_t&value, ਬਿਨਾਨਿਸ਼ਾਨੀ ਵਿੱਥ_2,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      ਪ੍ਰਤਿਜ੍ਞਾ(ep);
	// Now add this call to the end of the event list.
      *(ep->ਆਖਰੀ) = ਨਵਾਂ evctl_array(ਸਮ੍ਰਿਤੀ, ਸੂਚਕਾਂਕ, value, ਵਿੱਥ_2, ecount);
      ep->ਆਖਰੀ = &((*(ep->ਆਖਰੀ))->ਅਗਲਾ_2);
}

evctl_array_r::evctl_array_r(vvp_array_t ਸਮ੍ਰਿਤੀ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੂਚਕਾਂਕ,
                             ਦੁੱਗਣਾ value, uint64_t ecount)
:evctl(ecount)
{
      mem_ = ਸਮ੍ਰਿਤੀ;
      idx_ = ਸੂਚਕਾਂਕ;
      ਮੁੱਲ_2 = value;
}

ਖਾਲੀ evctl_array_r::ਲੜੀਵਾਰ_ਲੜੀਵਾਰ()
{
      mem_->ਸੈੱਟ_ਸ਼ਬਦ(idx_, ਮੁੱਲ_2);
}

ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_evctl(vvp_array_t ਸਮ੍ਰਿਤੀ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੂਚਕਾਂਕ,
                    ਦੁੱਗਣਾ value,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      ਪ੍ਰਤਿਜ੍ਞਾ(ep);
	// Now add this call to the end of the event list.
      *(ep->ਆਖਰੀ) = ਨਵਾਂ evctl_array_r(ਸਮ੍ਰਿਤੀ, ਸੂਚਕਾਂਕ, value, ecount);
      ep->ਆਖਰੀ = &((*(ep->ਆਖਰੀ))->ਅਗਲਾ_2);
}

ਸਤਰਅੰਦਰ vvp_fun_ਕਿਨਾਰਾ::ਕਿਨਾਰਾ_t VVP_ਕਿਨਾਰਾ(vvp_bit4_t from, vvp_bit4_t to)
{
      ਵਾਪਸ 1 << ((from << 2) | to);
}

ਅਚਲ vvp_fun_ਕਿਨਾਰਾ::ਕਿਨਾਰਾ_t vvp_ਕਿਨਾਰਾ_posedge
      = VVP_ਕਿਨਾਰਾ(BIT4_0,BIT4_1)
      | VVP_ਕਿਨਾਰਾ(BIT4_0,BIT4_X)
      | VVP_ਕਿਨਾਰਾ(BIT4_0,BIT4_Z)
      | VVP_ਕਿਨਾਰਾ(BIT4_X,BIT4_1)
      | VVP_ਕਿਨਾਰਾ(BIT4_Z,BIT4_1)
      ;

ਅਚਲ vvp_fun_ਕਿਨਾਰਾ::ਕਿਨਾਰਾ_t vvp_ਕਿਨਾਰਾ_negedge
      = VVP_ਕਿਨਾਰਾ(BIT4_1,BIT4_0)
      | VVP_ਕਿਨਾਰਾ(BIT4_1,BIT4_X)
      | VVP_ਕਿਨਾਰਾ(BIT4_1,BIT4_Z)
      | VVP_ਕਿਨਾਰਾ(BIT4_X,BIT4_0)
      | VVP_ਕਿਨਾਰਾ(BIT4_Z,BIT4_0)
      ;

ਅਚਲ vvp_fun_ਕਿਨਾਰਾ::ਕਿਨਾਰਾ_t vvp_ਕਿਨਾਰਾ_ਕਿਨਾਰਾ
      = VVP_ਕਿਨਾਰਾ(BIT4_0,BIT4_1)
      | VVP_ਕਿਨਾਰਾ(BIT4_1,BIT4_0)
      | VVP_ਕਿਨਾਰਾ(BIT4_0,BIT4_X)
      | VVP_ਕਿਨਾਰਾ(BIT4_X,BIT4_0)
      | VVP_ਕਿਨਾਰਾ(BIT4_0,BIT4_Z)
      | VVP_ਕਿਨਾਰਾ(BIT4_Z,BIT4_0)
      | VVP_ਕਿਨਾਰਾ(BIT4_X,BIT4_1)
      | VVP_ਕਿਨਾਰਾ(BIT4_1,BIT4_X)
      | VVP_ਕਿਨਾਰਾ(BIT4_Z,BIT4_1)
      | VVP_ਕਿਨਾਰਾ(BIT4_1,BIT4_Z)
      ;

ਅਚਲ vvp_fun_ਕਿਨਾਰਾ::ਕਿਨਾਰਾ_t vvp_ਕਿਨਾਰਾ_none    = 0;

ਬਣਤਰ vvp_fun_ਕਿਨਾਰਾ_state_s : ਜਨਤਕ waitable_state_s {
      vvp_fun_ਕਿਨਾਰਾ_state_s()
      {
            ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < 4 ;  idx += 1)
                  ਬਿੱਟ[idx] = BIT4_X;
      }

      vvp_bit4_t ਬਿੱਟ[4];
};

vvp_fun_ਕਿਨਾਰਾ::vvp_fun_ਕਿਨਾਰਾ(ਕਿਨਾਰਾ_t e)
: ਕਿਨਾਰਾ_2(e)
{
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < 4 ;  idx += 1)
            ਬਿੱਟ_2[idx] = BIT4_X;
}

vvp_fun_ਕਿਨਾਰਾ::~vvp_fun_ਕਿਨਾਰਾ()
{
}

ਤਾਰਕਿਕ vvp_fun_ਕਿਨਾਰਾ::recv_vec4_(ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                              vvp_bit4_t&old_ਦੋਆਧਾਰੀ_ਅੰਕ, vthread_t&ਧਾਗੇ)
{
	/* See what kind of edge this represents. */
      ਕਿਨਾਰਾ_t ਨਕਾਬ = VVP_ਕਿਨਾਰਾ(old_ਦੋਆਧਾਰੀ_ਅੰਕ, ਦੋਆਧਾਰੀ_ਅੰਕ.value(0));

	/* Save the current input for the next time around. */
      old_ਦੋਆਧਾਰੀ_ਅੰਕ = ਦੋਆਧਾਰੀ_ਅੰਕ.value(0);

      ਜੇ ((ਕਿਨਾਰਾ_2 == vvp_ਕਿਨਾਰਾ_none) || (ਕਿਨਾਰਾ_2 & ਨਕਾਬ)) {
	    ਲੜੀਵਾਰ_ਉਡੀਕ_ਧਾਗੇ(ਧਾਗੇ);
            ਵਾਪਸ ਸੱਚ;
      }
      ਵਾਪਸ ਝੂਠ;
}

vvp_fun_ਕਿਨਾਰਾ_sa::vvp_fun_ਕਿਨਾਰਾ_sa(ਕਿਨਾਰਾ_t e)
: vvp_fun_ਕਿਨਾਰਾ(e), ਧਾਗੇ_2(0)
{
}

vvp_fun_ਕਿਨਾਰਾ_sa::~vvp_fun_ਕਿਨਾਰਾ_sa()
{
}

vthread_t vvp_fun_ਕਿਨਾਰਾ_sa::ਜੋੜੋ_ਉਡੀਕ_thread(vthread_t thread)
{
      vthread_t tmp = ਧਾਗੇ_2;
      ਧਾਗੇ_2 = thread;

      ਵਾਪਸ tmp;
}

ਖਾਲੀ vvp_fun_ਕਿਨਾਰਾ_sa::recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                                vvp_context_t)
{
      ਜੇ (recv_vec4_(ਦੋਆਧਾਰੀ_ਅੰਕ, ਬਿੱਟ_2[ਪੋਰਟ.ਪੋਰਟ()], ਧਾਗੇ_2)) {
	    vvp_net_t*net = ਪੋਰਟ.ptr();
	    net->send_vec4(ਦੋਆਧਾਰੀ_ਅੰਕ, 0);
      }
}

ਖਾਲੀ vvp_fun_ਕਿਨਾਰਾ_sa::recv_vec4_pv(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
				   ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ਅਧਾਰ == 0);
      ਜੇ (recv_vec4_(ਦੋਆਧਾਰੀ_ਅੰਕ, ਬਿੱਟ_2[ਪੋਰਟ.ਪੋਰਟ()], ਧਾਗੇ_2)) {
	    vvp_net_t*net = ਪੋਰਟ.ptr();
	    net->send_vec4_pv(ਦੋਆਧਾਰੀ_ਅੰਕ, ਅਧਾਰ, vwid, 0);
      }
}

vvp_fun_ਕਿਨਾਰਾ_aa::vvp_fun_ਕਿਨਾਰਾ_aa(ਕਿਨਾਰਾ_t e)
: vvp_fun_ਕਿਨਾਰਾ(e)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_ਜੋੜੋ_ਵਸਤੂ_ਮੰਜ਼ਿਲ_context(ਇਹ, context_scope_);
}

vvp_fun_ਕਿਨਾਰਾ_aa::~vvp_fun_ਕਿਨਾਰਾ_aa()
{
}

ਖਾਲੀ vvp_fun_ਕਿਨਾਰਾ_aa::alloc_instance(vvp_context_t context)
{
      vvp_ਸੈੱਟ_context_ਵਸਤੂ(context, context_idx_, ਨਵਾਂ vvp_fun_ਕਿਨਾਰਾ_state_s);
      reset_instance(context);
}

ਖਾਲੀ vvp_fun_ਕਿਨਾਰਾ_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_ਕਿਨਾਰਾ_state_s*state = static_cast<vvp_fun_ਕਿਨਾਰਾ_state_s*>
            (vvp_get_context_ਵਸਤੂ(context, context_idx_));

      state->ਧਾਗੇ = 0;
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < 4 ;  idx += 1)
            state->ਬਿੱਟ[idx] = ਬਿੱਟ_2[idx];
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਖਾਲੀ vvp_fun_ਕਿਨਾਰਾ_aa::free_instance(vvp_context_t context)
{
      vvp_fun_ਕਿਨਾਰਾ_state_s*state = static_cast<vvp_fun_ਕਿਨਾਰਾ_state_s*>
            (vvp_get_context_ਵਸਤੂ(context, context_idx_));
      ਮਿਟਾਓ state;
}
#ਜੇ_ਅੰਤ

vthread_t vvp_fun_ਕਿਨਾਰਾ_aa::ਜੋੜੋ_ਉਡੀਕ_thread(vthread_t thread)
{
      vvp_fun_ਕਿਨਾਰਾ_state_s*state = static_cast<vvp_fun_ਕਿਨਾਰਾ_state_s*>
            (vthread_get_wt_context_ਵਸਤੂ(context_idx_));

      vthread_t tmp = state->ਧਾਗੇ;
      state->ਧਾਗੇ = thread;

      ਵਾਪਸ tmp;
}

ਖਾਲੀ vvp_fun_ਕਿਨਾਰਾ_aa::recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                                vvp_context_t context)
{
      ਜੇ (context) {
            vvp_fun_ਕਿਨਾਰਾ_state_s*state = static_cast<vvp_fun_ਕਿਨਾਰਾ_state_s*>
                  (vvp_get_context_ਵਸਤੂ(context, context_idx_));

            ਜੇ (recv_vec4_(ਦੋਆਧਾਰੀ_ਅੰਕ, state->ਬਿੱਟ[ਪੋਰਟ.ਪੋਰਟ()], state->ਧਾਗੇ)) {
                  vvp_net_t*net = ਪੋਰਟ.ptr();
                  net->send_vec4(ਦੋਆਧਾਰੀ_ਅੰਕ, context);
            }
      } ਨਹੀਂਤਾਂ {
            context = context_scope_->ਜੀਵਤ_contexts;
            ਜਦੋਂ (context) {
                  recv_vec4(ਪੋਰਟ, ਦੋਆਧਾਰੀ_ਅੰਕ, context);
                  context = vvp_get_ਅਗਲਾ_context(context);
            }
            ਬਿੱਟ_2[ਪੋਰਟ.ਪੋਰਟ()] = ਦੋਆਧਾਰੀ_ਅੰਕ.value(0);
      }
}

ਵਰਗ anyedge_ਮੁੱਲ {

    ਜਨਤਕ:
      anyedge_ਮੁੱਲ() {};
      ਆਭਾਸੀ ~anyedge_ਮੁੱਲ() {};

      ਆਭਾਸੀ ਖਾਲੀ reset() = 0;

      ਆਭਾਸੀ ਖਾਲੀ duplicate(anyedge_ਮੁੱਲ*&dup) = 0;
};

ਵਰਗ anyedge_vec4_ਮੁੱਲ : ਜਨਤਕ anyedge_ਮੁੱਲ {

    ਜਨਤਕ:
      anyedge_vec4_ਮੁੱਲ() {};
      ਆਭਾਸੀ ~anyedge_vec4_ਮੁੱਲ() override {};

      ਖਾਲੀ reset() override { old_ਬਿੱਟ.ਸੈੱਟ_ਮੰਜ਼ਿਲ_x(); }

      ਖਾਲੀ ਸੈੱਟ(ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ) { old_ਬਿੱਟ = ਦੋਆਧਾਰੀ_ਅੰਕ; };

      ਖਾਲੀ duplicate(anyedge_ਮੁੱਲ*&dup) override;

      ਤਾਰਕਿਕ recv_vec4(ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ);

      ਤਾਰਕਿਕ recv_vec4_pv(ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ,
			ਬਿਨਾਨਿਸ਼ਾਨੀ vwid);

    ਨਿਜੀ:
      vvp_vector4_t old_ਬਿੱਟ;
};

ਸਥਿਰ anyedge_vec4_ਮੁੱਲ*get_vec4_ਮੁੱਲ(anyedge_ਮੁੱਲ*&value)
{
      anyedge_vec4_ਮੁੱਲ*vec4_ਮੁੱਲ = dynamic_cast<anyedge_vec4_ਮੁੱਲ*>(value);
      ਜੇ (!value) {
	    vec4_ਮੁੱਲ = ਨਵਾਂ anyedge_vec4_ਮੁੱਲ();
	    ਮਿਟਾਓ value;
	    value = vec4_ਮੁੱਲ;
      }
      ਵਾਪਸ vec4_ਮੁੱਲ;
}

ਵਰਗ anyedge_real_ਮੁੱਲ : ਜਨਤਕ anyedge_ਮੁੱਲ {

    ਜਨਤਕ:
      anyedge_real_ਮੁੱਲ() : old_ਬਿੱਟ(0.0) {};
      ਆਭਾਸੀ ~anyedge_real_ਮੁੱਲ() override {};

      ਖਾਲੀ reset() override { old_ਬਿੱਟ = 0.0; }

      ਖਾਲੀ ਸੈੱਟ(ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ) { old_ਬਿੱਟ = ਦੋਆਧਾਰੀ_ਅੰਕ; };

      ਖਾਲੀ duplicate(anyedge_ਮੁੱਲ*&dup) override;

      ਤਾਰਕਿਕ recv_real(ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ);

    ਨਿਜੀ:
      ਦੁੱਗਣਾ old_ਬਿੱਟ;
};

ਸਥਿਰ anyedge_real_ਮੁੱਲ*get_real_ਮੁੱਲ(anyedge_ਮੁੱਲ*&value)
{
      anyedge_real_ਮੁੱਲ*real_ਮੁੱਲ = dynamic_cast<anyedge_real_ਮੁੱਲ*>(value);
      ਜੇ (!value) {
	    real_ਮੁੱਲ = ਨਵਾਂ anyedge_real_ਮੁੱਲ();
	    ਮਿਟਾਓ value;
	    value = real_ਮੁੱਲ;
      }
      ਵਾਪਸ real_ਮੁੱਲ;
}

ਵਰਗ anyedge_string_ਮੁੱਲ : ਜਨਤਕ anyedge_ਮੁੱਲ {

    ਜਨਤਕ:
      anyedge_string_ਮੁੱਲ() {};
      ਆਭਾਸੀ ~anyedge_string_ਮੁੱਲ() override {};

      ਖਾਲੀ reset() override { old_ਬਿੱਟ.clear(); }

      ਖਾਲੀ ਸੈੱਟ(ਅਚਲ std::string&ਦੋਆਧਾਰੀ_ਅੰਕ) { old_ਬਿੱਟ = ਦੋਆਧਾਰੀ_ਅੰਕ; };

      ਖਾਲੀ duplicate(anyedge_ਮੁੱਲ*&dup) override;

      ਤਾਰਕਿਕ recv_string(ਅਚਲ std::string&ਦੋਆਧਾਰੀ_ਅੰਕ);

    ਨਿਜੀ:
      std::string old_ਬਿੱਟ;
};

ਸਥਿਰ anyedge_string_ਮੁੱਲ*get_string_ਮੁੱਲ(anyedge_ਮੁੱਲ*&value)
{
      anyedge_string_ਮੁੱਲ*string_ਮੁੱਲ = dynamic_cast<anyedge_string_ਮੁੱਲ*>(value);
      ਜੇ (!value) {
	    string_ਮੁੱਲ = ਨਵਾਂ anyedge_string_ਮੁੱਲ();
	    ਮਿਟਾਓ value;
	    value = string_ਮੁੱਲ;
      }
      ਵਾਪਸ string_ਮੁੱਲ;
}

ਬਣਤਰ vvp_fun_anyedge_state_s : ਜਨਤਕ waitable_state_s {
      vvp_fun_anyedge_state_s()
      {
            ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < 4 ;  idx += 1)
                  ਆਖਰੀ_ਮੁੱਲ[idx] = 0;
      }

      ~vvp_fun_anyedge_state_s()
      {
            ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < 4 ;  idx += 1)
                  ਮਿਟਾਓ ਆਖਰੀ_ਮੁੱਲ[idx];
      }

      anyedge_ਮੁੱਲ *ਆਖਰੀ_ਮੁੱਲ[4];
      vvp_fun_anyedge_state_s(ਅਚਲ vvp_fun_anyedge_state_s&) = ਮਿਟਾਓ;
      vvp_fun_anyedge_state_s& ਸੰਚਾਲਕ=(ਅਚਲ vvp_fun_anyedge_state_s&) = ਮਿਟਾਓ;
};

vvp_fun_anyedge::vvp_fun_anyedge()
{
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < 4 ;  idx += 1)
	    ਆਖਰੀ_ਮੁੱਲ[idx] = 0;
}

vvp_fun_anyedge::~vvp_fun_anyedge()
{
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < 4 ;  idx += 1)
	    ਮਿਟਾਓ ਆਖਰੀ_ਮੁੱਲ[idx];
}

ਖਾਲੀ anyedge_vec4_ਮੁੱਲ::duplicate(anyedge_ਮੁੱਲ*&dup)
{
      anyedge_vec4_ਮੁੱਲ*dup_vec4 = get_vec4_ਮੁੱਲ(dup);
      ਪ੍ਰਤਿਜ੍ਞਾ(dup_vec4);
      dup_vec4->ਸੈੱਟ(old_ਬਿੱਟ);
}

ਤਾਰਕਿਕ anyedge_vec4_ਮੁੱਲ::recv_vec4(ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ)
{
      ਤਾਰਕਿਕ flag = ਝੂਠ;

      ਜੇ (old_ਬਿੱਟ.size() != ਦੋਆਧਾਰੀ_ਅੰਕ.size()) {
	    ਜੇ (old_ਬਿੱਟ.size() == 0) {
		    // Special case: If we've not seen any input yet
		    // (old_bits.size()==0) then replace it will a reference
		    // vector that is 'bx. Then compare that with the input
		    // to see if we are processing a change from 'bx.
		  old_ਬਿੱਟ = vvp_vector4_t(ਦੋਆਧਾਰੀ_ਅੰਕ.size(), BIT4_X);
		  ਜੇ (old_ਬਿੱਟ.eeq(ਦੋਆਧਾਰੀ_ਅੰਕ))
			flag = ਝੂਠ;
		  ਨਹੀਂਤਾਂ
			flag = ਸੱਚ;

	    } ਨਹੀਂਤਾਂ {
		  flag = ਸੱਚ;
	    }

      } ਨਹੀਂਤਾਂ {
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < ਦੋਆਧਾਰੀ_ਅੰਕ.size() ;  idx += 1) {
		  ਜੇ (old_ਬਿੱਟ.value(idx) != ਦੋਆਧਾਰੀ_ਅੰਕ.value(idx)) {
			flag = ਸੱਚ;
			ਰੋਕੋ;
		  }
	    }
      }

      ਜੇ (flag) {
	    old_ਬਿੱਟ = ਦੋਆਧਾਰੀ_ਅੰਕ;
      }

      ਵਾਪਸ flag;
}

ਤਾਰਕਿਕ anyedge_vec4_ਮੁੱਲ::recv_vec4_pv(ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ,
				      ਬਿਨਾਨਿਸ਼ਾਨੀ vwid)
{
      vvp_vector4_t tmp = old_ਬਿੱਟ;
      ਜੇ (tmp.size() == 0)
	    tmp = vvp_vector4_t(vwid, BIT4_Z);
      ਪ੍ਰਤਿਜ੍ਞਾ(ਅਧਾਰ + ਦੋਆਧਾਰੀ_ਅੰਕ.size()<= vwid);
      ਪ੍ਰਤਿਜ੍ਞਾ(tmp.size() == vwid);
      tmp.ਸੈੱਟ_vec(ਅਧਾਰ, ਦੋਆਧਾਰੀ_ਅੰਕ);

      ਵਾਪਸ recv_vec4(tmp);
}

ਖਾਲੀ anyedge_real_ਮੁੱਲ::duplicate(anyedge_ਮੁੱਲ*&dup)
{
      anyedge_real_ਮੁੱਲ*dup_real = get_real_ਮੁੱਲ(dup);
      ਪ੍ਰਤਿਜ੍ਞਾ(dup_real);
      dup_real->ਸੈੱਟ(old_ਬਿੱਟ);
}

ਤਾਰਕਿਕ anyedge_real_ਮੁੱਲ::recv_real(ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ)
{
      ਜੇ (old_ਬਿੱਟ != ਦੋਆਧਾਰੀ_ਅੰਕ) {
	    old_ਬਿੱਟ = ਦੋਆਧਾਰੀ_ਅੰਕ;
            ਵਾਪਸ ਸੱਚ;
      }
      ਵਾਪਸ ਝੂਠ;
}

ਖਾਲੀ anyedge_string_ਮੁੱਲ::duplicate(anyedge_ਮੁੱਲ*&dup)
{
      anyedge_string_ਮੁੱਲ*dup_string = get_string_ਮੁੱਲ(dup);
      ਪ੍ਰਤਿਜ੍ਞਾ(dup_string);
      dup_string->ਸੈੱਟ(old_ਬਿੱਟ);
}

ਤਾਰਕਿਕ anyedge_string_ਮੁੱਲ::recv_string(ਅਚਲ std::string&ਦੋਆਧਾਰੀ_ਅੰਕ)
{
      ਜੇ (old_ਬਿੱਟ != ਦੋਆਧਾਰੀ_ਅੰਕ) {
	    old_ਬਿੱਟ = ਦੋਆਧਾਰੀ_ਅੰਕ;
            ਵਾਪਸ ਸੱਚ;
      }
      ਵਾਪਸ ਝੂਠ;
}

vvp_fun_anyedge_sa::vvp_fun_anyedge_sa()
: ਧਾਗੇ_2(0)
{
}

vvp_fun_anyedge_sa::~vvp_fun_anyedge_sa()
{
}

vthread_t vvp_fun_anyedge_sa::ਜੋੜੋ_ਉਡੀਕ_thread(vthread_t thread)
{
      vthread_t tmp = ਧਾਗੇ_2;
      ਧਾਗੇ_2 = thread;

      ਵਾਪਸ tmp;
}

ਖਾਲੀ vvp_fun_anyedge_sa::recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                                   vvp_context_t)
{
      anyedge_vec4_ਮੁੱਲ*value = get_vec4_ਮੁੱਲ(ਆਖਰੀ_ਮੁੱਲ[ਪੋਰਟ.ਪੋਰਟ()]);
      ਪ੍ਰਤਿਜ੍ਞਾ(value);
      ਜੇ (value->recv_vec4(ਦੋਆਧਾਰੀ_ਅੰਕ)) {
	    ਲੜੀਵਾਰ_ਉਡੀਕ_ਧਾਗੇ(ਧਾਗੇ_2);
	    vvp_net_t*net = ਪੋਰਟ.ptr();
	    net->send_vec4(ਦੋਆਧਾਰੀ_ਅੰਕ, 0);
      }
}

ਖਾਲੀ vvp_fun_anyedge_sa::recv_vec4_pv(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
				      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t)
{
      anyedge_vec4_ਮੁੱਲ*value = get_vec4_ਮੁੱਲ(ਆਖਰੀ_ਮੁੱਲ[ਪੋਰਟ.ਪੋਰਟ()]);
      ਪ੍ਰਤਿਜ੍ਞਾ(value);
      ਜੇ (value->recv_vec4_pv(ਦੋਆਧਾਰੀ_ਅੰਕ, ਅਧਾਰ, vwid)) {
	    ਲੜੀਵਾਰ_ਉਡੀਕ_ਧਾਗੇ(ਧਾਗੇ_2);
	    vvp_net_t*net = ਪੋਰਟ.ptr();
	    net->send_vec4(ਦੋਆਧਾਰੀ_ਅੰਕ, 0);
      }
}

ਖਾਲੀ vvp_fun_anyedge_sa::recv_real(vvp_net_ptr_t ਪੋਰਟ, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                                   vvp_context_t)
{
      anyedge_real_ਮੁੱਲ*value = get_real_ਮੁੱਲ(ਆਖਰੀ_ਮੁੱਲ[ਪੋਰਟ.ਪੋਰਟ()]);
      ਪ੍ਰਤਿਜ੍ਞਾ(value);
      ਜੇ (value->recv_real(ਦੋਆਧਾਰੀ_ਅੰਕ)) {
	    ਲੜੀਵਾਰ_ਉਡੀਕ_ਧਾਗੇ(ਧਾਗੇ_2);
	    vvp_net_t*net = ਪੋਰਟ.ptr();
	    net->send_vec4(vvp_vector4_t(), 0);
      }
}

ਖਾਲੀ vvp_fun_anyedge_sa::recv_string(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ std::string&ਦੋਆਧਾਰੀ_ਅੰਕ,
				     vvp_context_t)
{
      anyedge_string_ਮੁੱਲ*value = get_string_ਮੁੱਲ(ਆਖਰੀ_ਮੁੱਲ[ਪੋਰਟ.ਪੋਰਟ()]);
      ਪ੍ਰਤਿਜ੍ਞਾ(value);
      ਜੇ (value->recv_string(ਦੋਆਧਾਰੀ_ਅੰਕ)) {
	    ਲੜੀਵਾਰ_ਉਡੀਕ_ਧਾਗੇ(ਧਾਗੇ_2);
	    vvp_net_t*net = ਪੋਰਟ.ptr();
	    net->send_vec4(vvp_vector4_t(), 0);
      }
}

/*
 * An anyedge receiving an object should do nothing with it, but should
 * trigger waiting threads.
 */
ਖਾਲੀ vvp_fun_anyedge_sa::recv_object(vvp_net_ptr_t ਪੋਰਟ, vvp_object_t,
				     vvp_context_t)
{
      ਲੜੀਵਾਰ_ਉਡੀਕ_ਧਾਗੇ(ਧਾਗੇ_2);
      vvp_net_t*net = ਪੋਰਟ.ptr();
      net->send_vec4(vvp_vector4_t(), 0);
}

vvp_fun_anyedge_aa::vvp_fun_anyedge_aa()
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_ਜੋੜੋ_ਵਸਤੂ_ਮੰਜ਼ਿਲ_context(ਇਹ, context_scope_);
}

vvp_fun_anyedge_aa::~vvp_fun_anyedge_aa()
{
}

ਖਾਲੀ vvp_fun_anyedge_aa::alloc_instance(vvp_context_t context)
{
      vvp_ਸੈੱਟ_context_ਵਸਤੂ(context, context_idx_, ਨਵਾਂ vvp_fun_anyedge_state_s);
      reset_instance(context);
}

ਖਾਲੀ vvp_fun_anyedge_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
            (vvp_get_context_ਵਸਤੂ(context, context_idx_));

      state->ਧਾਗੇ = 0;
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < 4 ;  idx += 1) {
	    ਜੇ (ਆਖਰੀ_ਮੁੱਲ[idx])
	          ਆਖਰੀ_ਮੁੱਲ[idx]->duplicate(state->ਆਖਰੀ_ਮੁੱਲ[idx]);
	    ਨਹੀਂਤਾਂ ਜੇ (state->ਆਖਰੀ_ਮੁੱਲ[idx])
		  state->ਆਖਰੀ_ਮੁੱਲ[idx]->reset();
      }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਖਾਲੀ vvp_fun_anyedge_aa::free_instance(vvp_context_t context)
{
      vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
            (vvp_get_context_ਵਸਤੂ(context, context_idx_));
      ਮਿਟਾਓ state;
}
#ਜੇ_ਅੰਤ

vthread_t vvp_fun_anyedge_aa::ਜੋੜੋ_ਉਡੀਕ_thread(vthread_t thread)
{
      vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
            (vthread_get_wt_context_ਵਸਤੂ(context_idx_));

      vthread_t tmp = state->ਧਾਗੇ;
      state->ਧਾਗੇ = thread;

      ਵਾਪਸ tmp;
}

ਖਾਲੀ vvp_fun_anyedge_aa::recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                                   vvp_context_t context)
{
      ਜੇ (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_ਵਸਤੂ(context, context_idx_));

            anyedge_vec4_ਮੁੱਲ*value = get_vec4_ਮੁੱਲ(state->ਆਖਰੀ_ਮੁੱਲ[ਪੋਰਟ.ਪੋਰਟ()]);
            ਪ੍ਰਤਿਜ੍ਞਾ(value);
            ਜੇ (value->recv_vec4(ਦੋਆਧਾਰੀ_ਅੰਕ)) {
                  ਲੜੀਵਾਰ_ਉਡੀਕ_ਧਾਗੇ(state->ਧਾਗੇ);
                  vvp_net_t*net = ਪੋਰਟ.ptr();
                  net->send_vec4(ਦੋਆਧਾਰੀ_ਅੰਕ, context);
            }
      } ਨਹੀਂਤਾਂ {
            context = context_scope_->ਜੀਵਤ_contexts;
            ਜਦੋਂ (context) {
                  recv_vec4(ਪੋਰਟ, ਦੋਆਧਾਰੀ_ਅੰਕ, context);
                  context = vvp_get_ਅਗਲਾ_context(context);
            }
            anyedge_vec4_ਮੁੱਲ*value = get_vec4_ਮੁੱਲ(ਆਖਰੀ_ਮੁੱਲ[ਪੋਰਟ.ਪੋਰਟ()]);
            ਪ੍ਰਤਿਜ੍ਞਾ(value);
            value->ਸੈੱਟ(ਦੋਆਧਾਰੀ_ਅੰਕ);
      }
}

ਖਾਲੀ vvp_fun_anyedge_aa::recv_real(vvp_net_ptr_t ਪੋਰਟ, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                                   vvp_context_t context)
{
      ਜੇ (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_ਵਸਤੂ(context, context_idx_));

            anyedge_real_ਮੁੱਲ*value = get_real_ਮੁੱਲ(state->ਆਖਰੀ_ਮੁੱਲ[ਪੋਰਟ.ਪੋਰਟ()]);
            ਪ੍ਰਤਿਜ੍ਞਾ(value);
            ਜੇ (value->recv_real(ਦੋਆਧਾਰੀ_ਅੰਕ)) {
                  ਲੜੀਵਾਰ_ਉਡੀਕ_ਧਾਗੇ(state->ਧਾਗੇ);
                  vvp_net_t*net = ਪੋਰਟ.ptr();
                  net->send_vec4(vvp_vector4_t(), context);
            }
      } ਨਹੀਂਤਾਂ {
            context = context_scope_->ਜੀਵਤ_contexts;
            ਜਦੋਂ (context) {
                  recv_real(ਪੋਰਟ, ਦੋਆਧਾਰੀ_ਅੰਕ, context);
                  context = vvp_get_ਅਗਲਾ_context(context);
            }
            anyedge_real_ਮੁੱਲ*value = get_real_ਮੁੱਲ(ਆਖਰੀ_ਮੁੱਲ[ਪੋਰਟ.ਪੋਰਟ()]);
            ਪ੍ਰਤਿਜ੍ਞਾ(value);
            value->ਸੈੱਟ(ਦੋਆਧਾਰੀ_ਅੰਕ);
      }
}

ਖਾਲੀ vvp_fun_anyedge_aa::recv_string(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ std::string&ਦੋਆਧਾਰੀ_ਅੰਕ,
				     vvp_context_t context)
{
      ਜੇ (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_ਵਸਤੂ(context, context_idx_));

            anyedge_string_ਮੁੱਲ*value = get_string_ਮੁੱਲ(state->ਆਖਰੀ_ਮੁੱਲ[ਪੋਰਟ.ਪੋਰਟ()]);
            ਪ੍ਰਤਿਜ੍ਞਾ(value);
            ਜੇ (value->recv_string(ਦੋਆਧਾਰੀ_ਅੰਕ)) {
                  ਲੜੀਵਾਰ_ਉਡੀਕ_ਧਾਗੇ(state->ਧਾਗੇ);
                  vvp_net_t*net = ਪੋਰਟ.ptr();
                  net->send_vec4(vvp_vector4_t(), context);
            }
      } ਨਹੀਂਤਾਂ {
            context = context_scope_->ਜੀਵਤ_contexts;
            ਜਦੋਂ (context) {
                  recv_string(ਪੋਰਟ, ਦੋਆਧਾਰੀ_ਅੰਕ, context);
                  context = vvp_get_ਅਗਲਾ_context(context);
            }
            anyedge_string_ਮੁੱਲ*value = get_string_ਮੁੱਲ(ਆਖਰੀ_ਮੁੱਲ[ਪੋਰਟ.ਪੋਰਟ()]);
            ਪ੍ਰਤਿਜ੍ਞਾ(value);
            value->ਸੈੱਟ(ਦੋਆਧਾਰੀ_ਅੰਕ);
      }
}

vvp_fun_event_or::vvp_fun_event_or(vvp_net_t*ਅਧਾਰ_net)
: ਅਧਾਰ_net_2(ਅਧਾਰ_net)
{
}

vvp_fun_event_or::~vvp_fun_event_or()
{
}

vvp_fun_event_or_sa::vvp_fun_event_or_sa(vvp_net_t*ਅਧਾਰ_net)
: vvp_fun_event_or(ਅਧਾਰ_net), ਧਾਗੇ_2(0)
{
}

vvp_fun_event_or_sa::~vvp_fun_event_or_sa()
{
}

vthread_t vvp_fun_event_or_sa::ਜੋੜੋ_ਉਡੀਕ_thread(vthread_t thread)
{
      vthread_t tmp = ਧਾਗੇ_2;
      ਧਾਗੇ_2 = thread;

      ਵਾਪਸ tmp;
}

ਖਾਲੀ vvp_fun_event_or_sa::recv_vec4(vvp_net_ptr_t, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                                    vvp_context_t)
{
      ਲੜੀਵਾਰ_ਉਡੀਕ_ਧਾਗੇ(ਧਾਗੇ_2);
      ਅਧਾਰ_net_2->send_vec4(ਦੋਆਧਾਰੀ_ਅੰਕ, 0);
}

vvp_fun_event_or_aa::vvp_fun_event_or_aa(vvp_net_t*ਅਧਾਰ_net)
: vvp_fun_event_or(ਅਧਾਰ_net)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_ਜੋੜੋ_ਵਸਤੂ_ਮੰਜ਼ਿਲ_context(ਇਹ, context_scope_);
}

vvp_fun_event_or_aa::~vvp_fun_event_or_aa()
{
}

ਖਾਲੀ vvp_fun_event_or_aa::alloc_instance(vvp_context_t context)
{
      vvp_ਸੈੱਟ_context_ਵਸਤੂ(context, context_idx_, ਨਵਾਂ waitable_state_s);
}

ਖਾਲੀ vvp_fun_event_or_aa::reset_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_ਵਸਤੂ(context, context_idx_));

      state->ਧਾਗੇ = 0;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਖਾਲੀ vvp_fun_event_or_aa::free_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_ਵਸਤੂ(context, context_idx_));
      ਮਿਟਾਓ state;
}
#ਜੇ_ਅੰਤ

vthread_t vvp_fun_event_or_aa::ਜੋੜੋ_ਉਡੀਕ_thread(vthread_t thread)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vthread_get_wt_context_ਵਸਤੂ(context_idx_));

      vthread_t tmp = state->ਧਾਗੇ;
      state->ਧਾਗੇ = thread;

      ਵਾਪਸ tmp;
}

ਖਾਲੀ vvp_fun_event_or_aa::recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                                    vvp_context_t context)
{
      ਜੇ (context) {
            waitable_state_s*state = static_cast<waitable_state_s*>
                  (vvp_get_context_ਵਸਤੂ(context, context_idx_));

            ਲੜੀਵਾਰ_ਉਡੀਕ_ਧਾਗੇ(state->ਧਾਗੇ);
            ਅਧਾਰ_net_2->send_vec4(ਦੋਆਧਾਰੀ_ਅੰਕ, context);
      } ਨਹੀਂਤਾਂ {
            context = context_scope_->ਜੀਵਤ_contexts;
            ਜਦੋਂ (context) {
                  recv_vec4(ਪੋਰਟ, ਦੋਆਧਾਰੀ_ਅੰਕ, context);
                  context = vvp_get_ਅਗਲਾ_context(context);
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
: vvp_named_event(h), ਧਾਗੇ_2(0)
{
}

vvp_named_event_sa::~vvp_named_event_sa()
{
}

vthread_t vvp_named_event_sa::ਜੋੜੋ_ਉਡੀਕ_thread(vthread_t thread)
{
      vthread_t tmp = ਧਾਗੇ_2;
      ਧਾਗੇ_2 = thread;

      ਵਾਪਸ tmp;
}

ਖਾਲੀ vvp_named_event_sa::recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                                   vvp_context_t)
{
      ਲੜੀਵਾਰ_ਉਡੀਕ_ਧਾਗੇ(ਧਾਗੇ_2);
      vvp_net_t*net = ਪੋਰਟ.ptr();
      net->send_vec4(ਦੋਆਧਾਰੀ_ਅੰਕ, 0);

      __vpiNamedEvent*obj = dynamic_cast<__vpiNamedEvent*>(handle_);
      ਪ੍ਰਤਿਜ੍ਞਾ(obj);
      obj->ਲੜੀਵਾਰ_vpi_callbacks();
}

vvp_named_event_aa::vvp_named_event_aa(__vpiHandle*h)
: vvp_named_event(h)
{
      context_idx_ = vpip_ਜੋੜੋ_ਵਸਤੂ_ਮੰਜ਼ਿਲ_context(ਇਹ, vpip_peek_context_scope());
}

vvp_named_event_aa::~vvp_named_event_aa()
{
}

ਖਾਲੀ vvp_named_event_aa::alloc_instance(vvp_context_t context)
{
      vvp_ਸੈੱਟ_context_ਵਸਤੂ(context, context_idx_, ਨਵਾਂ waitable_state_s);
}

ਖਾਲੀ vvp_named_event_aa::reset_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_ਵਸਤੂ(context, context_idx_));

      state->ਧਾਗੇ = 0;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਖਾਲੀ vvp_named_event_aa::free_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_ਵਸਤੂ(context, context_idx_));
      ਮਿਟਾਓ state;
}
#ਜੇ_ਅੰਤ

vthread_t vvp_named_event_aa::ਜੋੜੋ_ਉਡੀਕ_thread(vthread_t thread)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vthread_get_wt_context_ਵਸਤੂ(context_idx_));

      vthread_t tmp = state->ਧਾਗੇ;
      state->ਧਾਗੇ = thread;

      ਵਾਪਸ tmp;
}

ਖਾਲੀ vvp_named_event_aa::recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                                   vvp_context_t context)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(context);

      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_ਵਸਤੂ(context, context_idx_));

      ਲੜੀਵਾਰ_ਉਡੀਕ_ਧਾਗੇ(state->ਧਾਗੇ);
      vvp_net_t*net = ਪੋਰਟ.ptr();
      net->send_vec4(ਦੋਆਧਾਰੀ_ਅੰਕ, context);
}

/*
**  Create an event functor
**  edge:  compile_event(label, type, argc, argv, debug_flag)
**  or:    compile_event(label, NULL, argc, argv, debug_flag)
**
**  Named events are handled elsewhere.
*/

ਸਥਿਰ ਖਾਲੀ compile_event_or(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);

ਖਾਲੀ compile_event(ਅੱਖਰ*label, ਅੱਖਰ*type, ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      vvp_net_fun_t*fun = 0;

      ਜੇ (type == 0) {
	    compile_event_or(label, argc, argv);
	    ਵਾਪਸ;
      }

      ਜੇ (strcmp(type,"anyedge") == 0) {

	    ਮੁਕਤ(type);

            ਜੇ (vpip_peek_current_scope()->is_automatic()) {
                  fun = ਨਵਾਂ vvp_fun_anyedge_aa;
            } ਨਹੀਂਤਾਂ {
                  fun = ਨਵਾਂ vvp_fun_anyedge_sa;
            }

      } ਨਹੀਂਤਾਂ {

	    vvp_fun_ਕਿਨਾਰਾ::ਕਿਨਾਰਾ_t ਕਿਨਾਰਾ_type = vvp_ਕਿਨਾਰਾ_none;

	    ਜੇ (strcmp(type,"posedge") == 0)
		  ਕਿਨਾਰਾ_type = vvp_ਕਿਨਾਰਾ_posedge;
	    ਨਹੀਂਤਾਂ ਜੇ (strcmp(type,"negedge") == 0)
		  ਕਿਨਾਰਾ_type = vvp_ਕਿਨਾਰਾ_negedge;
	    ਨਹੀਂਤਾਂ ਜੇ (strcmp(type,"edge") == 0)
		  ਕਿਨਾਰਾ_type = vvp_ਕਿਨਾਰਾ_ਕਿਨਾਰਾ;

	    ਪ੍ਰਤਿਜ੍ਞਾ(argc <= 4);
	    ਮੁਕਤ(type);

            ਜੇ (vpip_peek_current_scope()->is_automatic()) {
                  fun = ਨਵਾਂ vvp_fun_ਕਿਨਾਰਾ_aa(ਕਿਨਾਰਾ_type);
            } ਨਹੀਂਤਾਂ {
                  fun = ਨਵਾਂ vvp_fun_ਕਿਨਾਰਾ_sa(ਕਿਨਾਰਾ_type);
            }

      }

      vvp_net_t* ptr = ਨਵਾਂ vvp_net_t;
      ptr->fun = fun;

      define_functor_ਚਿੰਨ੍ਹ(label, ptr);
      ਮੁਕਤ(label);

      inputs_connect(ptr, argc, argv);
      ਮੁਕਤ(argv);
}

ਸਥਿਰ ਖਾਲੀ compile_event_or(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      vvp_net_t*ਅਧਾਰ_net = ਨਵਾਂ vvp_net_t;
      ਜੇ (vpip_peek_current_scope()->is_automatic()) {
            ਅਧਾਰ_net->fun = ਨਵਾਂ vvp_fun_event_or_aa(ਅਧਾਰ_net);
      } ਨਹੀਂਤਾਂ {
            ਅਧਾਰ_net->fun = ਨਵਾਂ vvp_fun_event_or_sa(ਅਧਾਰ_net);
      }
      define_functor_ਚਿੰਨ੍ਹ(label, ਅਧਾਰ_net);
      ਮੁਕਤ(label);

	/* This is a simplified version of a wide functor. We don't
	   care about the data values or what port they arrived on,
	   so we can use a single shared functor. */
      vvp_net_t*curr_net = ਅਧਾਰ_net;
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < argc ;  idx += 1) {
	    ਜੇ (idx > 0 && (idx % 4) == 0) {
		  curr_net = ਨਵਾਂ vvp_net_t;
		  curr_net->fun = ਅਧਾਰ_net->fun;
	    }
	    ਨਿਵੇਸ਼_connect(curr_net, idx % 4, argv[idx].ਪਾਠ);
      }
      ਮੁਕਤ(argv);
}

/*
 * This handles the compile of named events. This functor has no
 * inputs, it is only accessed by behavioral trigger statements, which
 * in vvp are %set instructions.
 */
ਖਾਲੀ compile_named_event(ਅੱਖਰ*label, ਅੱਖਰ*name, ਤਾਰਕਿਕ local_flag)
{
      vvp_net_t*ptr = ਨਵਾਂ vvp_net_t;

      vpiHandle obj = vpip_make_named_event(name, ptr);

      ਜੇ (vpip_peek_current_scope()->is_automatic()) {
            ptr->fun = ਨਵਾਂ vvp_named_event_aa(obj);
      } ਨਹੀਂਤਾਂ {
            ptr->fun = ਨਵਾਂ vvp_named_event_sa(obj);
      }
      define_functor_ਚਿੰਨ੍ਹ(label, ptr);
      compile_vpi_ਚਿੰਨ੍ਹ(label, obj);
      ਜੇ (! local_flag) vpip_attach_ਮੰਜ਼ਿਲ_current_scope(obj);

      ਮੁਕਤ(label);
      ਮਿਟਾਓ[] name;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਖਾਲੀ named_event_delete(__vpiHandle*handle)
{
      ਮਿਟਾਓ dynamic_cast<__vpiNamedEvent *>(handle);
}
#ਜੇ_ਅੰਤ
