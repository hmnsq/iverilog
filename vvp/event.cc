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

ಶೂನ್ಯ waitable_hooks_s::ಸತತ_ಕಾಯುವಿಕೆ_ಎಳೆಗಳು(vthread_t&ಎಳೆಗಳು)
{
	// Run the non-blocking event controls.
      ಕೊನೆಯ = &event_ctls;
      ಗಾಗಿ (evctl*cur = event_ctls; cur != 0;) {
	    ಆದರೆ (cur->dec_and_ಸತತ()) {
		  evctl*nxt = cur->ಮುಂದಿನ_2;
		  ಅಳಿಸು cur;
		  cur = nxt;
		  *ಕೊನೆಯ = cur;
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  ಕೊನೆಯ = &(cur->ಮುಂದಿನ_2);
		  cur = cur->ಮುಂದಿನ_2;
	    }
      }

      vthread_t tmp = ಎಳೆಗಳು;
      ಆದರೆ (tmp == 0) ಹಿಂತಿರುಗಿ;
      ಎಳೆಗಳು = 0;

      vthread_ವೇಳಾಪಟ್ಟಿ_list(tmp);
}

evctl::evctl(uint64_t ecount)
{
      ecount_ = ecount;
      ಮುಂದಿನ_2 = 0;
}

ತಾರ್ಕಿಕ evctl::dec_and_ಸತತ()
{
      ಪ್ರತಿಜ್ಞಾ(ecount_ != 0);

      ecount_ -= 1;
      ಆದರೆ (ecount_ == 0) ಸತತ_ಸತತ();

      ಹಿಂತಿರುಗಿ ecount_ == 0;
}

evctl_real::evctl_real(__vpiHandle*handle, ದ್ವಿಗುಣ value,
                       uint64_t ecount)
:evctl(ecount)
{
      handle_ = handle;
      ಮೌಲ್ಯ_2 = value;
}

ಶೂನ್ಯ evctl_real::ಸತತ_ಸತತ()
{
      t_vpi_ಮೌಲ್ಯ val;

      val.format = vpiRealVal;
      val.value.real = ಮೌಲ್ಯ_2;
      vpi_put_ಮೌಲ್ಯ(handle_, &val, 0, vpiNoDelay);
}

ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_evctl(__vpiHandle*handle, ದ್ವಿಗುಣ value,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      ಪ್ರತಿಜ್ಞಾ(ep);
	// Now add this call to the end of the event list.
      *(ep->ಕೊನೆಯ) = ಹೊಸ evctl_real(handle, value, ecount);
      ep->ಕೊನೆಯ = &((*(ep->ಕೊನೆಯ))->ಮುಂದಿನ_2);
}

evctl_ಸದಿಶ::evctl_ಸದಿಶ(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&value,
                           ಚಿಹ್ನೆರಹಿತ off, ಚಿಹ್ನೆರಹಿತ wid, uint64_t ecount)
:evctl(ecount), ptr_(ptr), ಮೌಲ್ಯ_2(value)
{
      off_ = off;
      wid_ = wid;
}

ಶೂನ್ಯ evctl_ಸದಿಶ::ಸತತ_ಸತತ()
{
      ಆದರೆ (wid_ != 0) {
	    vvp_send_vec4_pv(ptr_, ಮೌಲ್ಯ_2, off_, wid_, 0);
      } ಇಲ್ಲದಿದ್ದರೆ {
	    vvp_send_vec4(ptr_, ಮೌಲ್ಯ_2, 0);
      }
}

ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_evctl(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&value,
                    ಚಿಹ್ನೆರಹಿತ ಸ್ಥಾನಾಂತರ_2, ಚಿಹ್ನೆರಹಿತ wid,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      ಪ್ರತಿಜ್ಞಾ(ep);
	// Now add this call to the end of the event list.
      *(ep->ಕೊನೆಯ) = ಹೊಸ evctl_ಸದಿಶ(ptr, value, ಸ್ಥಾನಾಂತರ_2, wid, ecount);
      ep->ಕೊನೆಯ = &((*(ep->ಕೊನೆಯ))->ಮುಂದಿನ_2);
}

evctl_array::evctl_array(vvp_array_t ಸ್ಮರಣೆ, ಚಿಹ್ನೆರಹಿತ ಸೂಚ್ಯಂಕ,
                         ಸ್ಥಿರಾಂಕ vvp_vector4_t&value, ಚಿಹ್ನೆರಹಿತ off,
                         uint64_t ecount)
:evctl(ecount), ಮೌಲ್ಯ_2(value)
{
      mem_ = ಸ್ಮರಣೆ;
      idx_ = ಸೂಚ್ಯಂಕ;
      off_ = off;
}

ಶೂನ್ಯ evctl_array::ಸತತ_ಸತತ()
{
      mem_->ಗಣ_ಶಬ್ದ(idx_, off_, ಮೌಲ್ಯ_2);
}

ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_evctl(vvp_array_t ಸ್ಮರಣೆ, ಚಿಹ್ನೆರಹಿತ ಸೂಚ್ಯಂಕ,
                    ಸ್ಥಿರಾಂಕ vvp_vector4_t&value, ಚಿಹ್ನೆರಹಿತ ಸ್ಥಾನಾಂತರ_2,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      ಪ್ರತಿಜ್ಞಾ(ep);
	// Now add this call to the end of the event list.
      *(ep->ಕೊನೆಯ) = ಹೊಸ evctl_array(ಸ್ಮರಣೆ, ಸೂಚ್ಯಂಕ, value, ಸ್ಥಾನಾಂತರ_2, ecount);
      ep->ಕೊನೆಯ = &((*(ep->ಕೊನೆಯ))->ಮುಂದಿನ_2);
}

evctl_array_r::evctl_array_r(vvp_array_t ಸ್ಮರಣೆ, ಚಿಹ್ನೆರಹಿತ ಸೂಚ್ಯಂಕ,
                             ದ್ವಿಗುಣ value, uint64_t ecount)
:evctl(ecount)
{
      mem_ = ಸ್ಮರಣೆ;
      idx_ = ಸೂಚ್ಯಂಕ;
      ಮೌಲ್ಯ_2 = value;
}

ಶೂನ್ಯ evctl_array_r::ಸತತ_ಸತತ()
{
      mem_->ಗಣ_ಶಬ್ದ(idx_, ಮೌಲ್ಯ_2);
}

ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_evctl(vvp_array_t ಸ್ಮರಣೆ, ಚಿಹ್ನೆರಹಿತ ಸೂಚ್ಯಂಕ,
                    ದ್ವಿಗುಣ value,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      ಪ್ರತಿಜ್ಞಾ(ep);
	// Now add this call to the end of the event list.
      *(ep->ಕೊನೆಯ) = ಹೊಸ evctl_array_r(ಸ್ಮರಣೆ, ಸೂಚ್ಯಂಕ, value, ecount);
      ep->ಕೊನೆಯ = &((*(ep->ಕೊನೆಯ))->ಮುಂದಿನ_2);
}

ಸಾಲೊಳಗೆ vvp_fun_ಅಂಚು::ಅಂಚು_t VVP_ಅಂಚು(vvp_bit4_t from, vvp_bit4_t to)
{
      ಹಿಂತಿರುಗಿ 1 << ((from << 2) | to);
}

ಸ್ಥಿರಾಂಕ vvp_fun_ಅಂಚು::ಅಂಚು_t vvp_ಅಂಚು_posedge
      = VVP_ಅಂಚು(BIT4_0,BIT4_1)
      | VVP_ಅಂಚು(BIT4_0,BIT4_X)
      | VVP_ಅಂಚು(BIT4_0,BIT4_Z)
      | VVP_ಅಂಚು(BIT4_X,BIT4_1)
      | VVP_ಅಂಚು(BIT4_Z,BIT4_1)
      ;

ಸ್ಥಿರಾಂಕ vvp_fun_ಅಂಚು::ಅಂಚು_t vvp_ಅಂಚು_negedge
      = VVP_ಅಂಚು(BIT4_1,BIT4_0)
      | VVP_ಅಂಚು(BIT4_1,BIT4_X)
      | VVP_ಅಂಚು(BIT4_1,BIT4_Z)
      | VVP_ಅಂಚು(BIT4_X,BIT4_0)
      | VVP_ಅಂಚು(BIT4_Z,BIT4_0)
      ;

ಸ್ಥಿರಾಂಕ vvp_fun_ಅಂಚು::ಅಂಚು_t vvp_ಅಂಚು_ಅಂಚು
      = VVP_ಅಂಚು(BIT4_0,BIT4_1)
      | VVP_ಅಂಚು(BIT4_1,BIT4_0)
      | VVP_ಅಂಚು(BIT4_0,BIT4_X)
      | VVP_ಅಂಚು(BIT4_X,BIT4_0)
      | VVP_ಅಂಚು(BIT4_0,BIT4_Z)
      | VVP_ಅಂಚು(BIT4_Z,BIT4_0)
      | VVP_ಅಂಚು(BIT4_X,BIT4_1)
      | VVP_ಅಂಚು(BIT4_1,BIT4_X)
      | VVP_ಅಂಚು(BIT4_Z,BIT4_1)
      | VVP_ಅಂಚು(BIT4_1,BIT4_Z)
      ;

ಸ್ಥಿರಾಂಕ vvp_fun_ಅಂಚು::ಅಂಚು_t vvp_ಅಂಚು_none    = 0;

ರಚನೆ vvp_fun_ಅಂಚು_state_s : ಸಾರ್ವಜನಿಕ waitable_state_s {
      vvp_fun_ಅಂಚು_state_s()
      {
            ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < 4 ;  idx += 1)
                  ಬಿಟ್ಟುಗಳು[idx] = BIT4_X;
      }

      vvp_bit4_t ಬಿಟ್ಟುಗಳು[4];
};

vvp_fun_ಅಂಚು::vvp_fun_ಅಂಚು(ಅಂಚು_t e)
: ಅಂಚು_2(e)
{
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < 4 ;  idx += 1)
            ಬಿಟ್ಟುಗಳು_2[idx] = BIT4_X;
}

vvp_fun_ಅಂಚು::~vvp_fun_ಅಂಚು()
{
}

ತಾರ್ಕಿಕ vvp_fun_ಅಂಚು::recv_vec4_(ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                              vvp_bit4_t&old_ಬಿಟ್, vthread_t&ಎಳೆಗಳು)
{
	/* See what kind of edge this represents. */
      ಅಂಚು_t ಮುಖವಾಡ = VVP_ಅಂಚು(old_ಬಿಟ್, ಬಿಟ್.value(0));

	/* Save the current input for the next time around. */
      old_ಬಿಟ್ = ಬಿಟ್.value(0);

      ಆದರೆ ((ಅಂಚು_2 == vvp_ಅಂಚು_none) || (ಅಂಚು_2 & ಮುಖವಾಡ)) {
	    ಸತತ_ಕಾಯುವಿಕೆ_ಎಳೆಗಳು(ಎಳೆಗಳು);
            ಹಿಂತಿರುಗಿ ನಿಜ;
      }
      ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

vvp_fun_ಅಂಚು_sa::vvp_fun_ಅಂಚು_sa(ಅಂಚು_t e)
: vvp_fun_ಅಂಚು(e), ಎಳೆಗಳು_2(0)
{
}

vvp_fun_ಅಂಚು_sa::~vvp_fun_ಅಂಚು_sa()
{
}

vthread_t vvp_fun_ಅಂಚು_sa::ಕೂಡಿಸು_ಕಾಯುವಿಕೆ_thread(vthread_t thread)
{
      vthread_t tmp = ಎಳೆಗಳು_2;
      ಎಳೆಗಳು_2 = thread;

      ಹಿಂತಿರುಗಿ tmp;
}

ಶೂನ್ಯ vvp_fun_ಅಂಚು_sa::recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                                vvp_context_t)
{
      ಆದರೆ (recv_vec4_(ಬಿಟ್, ಬಿಟ್ಟುಗಳು_2[ಪೋರ್ಟ್.ಪೋರ್ಟ್()], ಎಳೆಗಳು_2)) {
	    vvp_net_t*net = ಪೋರ್ಟ್.ptr();
	    net->send_vec4(ಬಿಟ್, 0);
      }
}

ಶೂನ್ಯ vvp_fun_ಅಂಚು_sa::recv_vec4_pv(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
				   ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t)
{
      ಪ್ರತಿಜ್ಞಾ(ತಳ == 0);
      ಆದರೆ (recv_vec4_(ಬಿಟ್, ಬಿಟ್ಟುಗಳು_2[ಪೋರ್ಟ್.ಪೋರ್ಟ್()], ಎಳೆಗಳು_2)) {
	    vvp_net_t*net = ಪೋರ್ಟ್.ptr();
	    net->send_vec4_pv(ಬಿಟ್, ತಳ, vwid, 0);
      }
}

vvp_fun_ಅಂಚು_aa::vvp_fun_ಅಂಚು_aa(ಅಂಚು_t e)
: vvp_fun_ಅಂಚು(e)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_ಕೂಡಿಸು_ಅಂಶ_ಗಮ್ಯ_context(ಇದು, context_scope_);
}

vvp_fun_ಅಂಚು_aa::~vvp_fun_ಅಂಚು_aa()
{
}

ಶೂನ್ಯ vvp_fun_ಅಂಚು_aa::alloc_instance(vvp_context_t context)
{
      vvp_ಗಣ_context_ಅಂಶ(context, context_idx_, ಹೊಸ vvp_fun_ಅಂಚು_state_s);
      reset_instance(context);
}

ಶೂನ್ಯ vvp_fun_ಅಂಚು_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_ಅಂಚು_state_s*state = static_cast<vvp_fun_ಅಂಚು_state_s*>
            (vvp_get_context_ಅಂಶ(context, context_idx_));

      state->ಎಳೆಗಳು = 0;
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < 4 ;  idx += 1)
            state->ಬಿಟ್ಟುಗಳು[idx] = ಬಿಟ್ಟುಗಳು_2[idx];
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಶೂನ್ಯ vvp_fun_ಅಂಚು_aa::free_instance(vvp_context_t context)
{
      vvp_fun_ಅಂಚು_state_s*state = static_cast<vvp_fun_ಅಂಚು_state_s*>
            (vvp_get_context_ಅಂಶ(context, context_idx_));
      ಅಳಿಸು state;
}
#ಆದರೆ_ಅಂತ್ಯ

vthread_t vvp_fun_ಅಂಚು_aa::ಕೂಡಿಸು_ಕಾಯುವಿಕೆ_thread(vthread_t thread)
{
      vvp_fun_ಅಂಚು_state_s*state = static_cast<vvp_fun_ಅಂಚು_state_s*>
            (vthread_get_wt_context_ಅಂಶ(context_idx_));

      vthread_t tmp = state->ಎಳೆಗಳು;
      state->ಎಳೆಗಳು = thread;

      ಹಿಂತಿರುಗಿ tmp;
}

ಶೂನ್ಯ vvp_fun_ಅಂಚು_aa::recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                                vvp_context_t context)
{
      ಆದರೆ (context) {
            vvp_fun_ಅಂಚು_state_s*state = static_cast<vvp_fun_ಅಂಚು_state_s*>
                  (vvp_get_context_ಅಂಶ(context, context_idx_));

            ಆದರೆ (recv_vec4_(ಬಿಟ್, state->ಬಿಟ್ಟುಗಳು[ಪೋರ್ಟ್.ಪೋರ್ಟ್()], state->ಎಳೆಗಳು)) {
                  vvp_net_t*net = ಪೋರ್ಟ್.ptr();
                  net->send_vec4(ಬಿಟ್, context);
            }
      } ಇಲ್ಲದಿದ್ದರೆ {
            context = context_scope_->ಜೀವಂತ_contexts;
            ಆಗ (context) {
                  recv_vec4(ಪೋರ್ಟ್, ಬಿಟ್, context);
                  context = vvp_get_ಮುಂದಿನ_context(context);
            }
            ಬಿಟ್ಟುಗಳು_2[ಪೋರ್ಟ್.ಪೋರ್ಟ್()] = ಬಿಟ್.value(0);
      }
}

ವರ್ಗ anyedge_ಮೌಲ್ಯ {

    ಸಾರ್ವಜನಿಕ:
      anyedge_ಮೌಲ್ಯ() {};
      ವಾಸ್ತವಿಕ ~anyedge_ಮೌಲ್ಯ() {};

      ವಾಸ್ತವಿಕ ಶೂನ್ಯ reset() = 0;

      ವಾಸ್ತವಿಕ ಶೂನ್ಯ duplicate(anyedge_ಮೌಲ್ಯ*&dup) = 0;
};

ವರ್ಗ anyedge_vec4_ಮೌಲ್ಯ : ಸಾರ್ವಜನಿಕ anyedge_ಮೌಲ್ಯ {

    ಸಾರ್ವಜನಿಕ:
      anyedge_vec4_ಮೌಲ್ಯ() {};
      ವಾಸ್ತವಿಕ ~anyedge_vec4_ಮೌಲ್ಯ() override {};

      ಶೂನ್ಯ reset() override { old_ಬಿಟ್ಟುಗಳು.ಗಣ_ಗಮ್ಯ_x(); }

      ಶೂನ್ಯ ಗಣ(ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್) { old_ಬಿಟ್ಟುಗಳು = ಬಿಟ್; };

      ಶೂನ್ಯ duplicate(anyedge_ಮೌಲ್ಯ*&dup) override;

      ತಾರ್ಕಿಕ recv_vec4(ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್);

      ತಾರ್ಕಿಕ recv_vec4_pv(ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್, ಚಿಹ್ನೆರಹಿತ ತಳ,
			ಚಿಹ್ನೆರಹಿತ vwid);

    ಖಾಸಗಿ:
      vvp_vector4_t old_ಬಿಟ್ಟುಗಳು;
};

ಸ್ಥಿರ anyedge_vec4_ಮೌಲ್ಯ*get_vec4_ಮೌಲ್ಯ(anyedge_ಮೌಲ್ಯ*&value)
{
      anyedge_vec4_ಮೌಲ್ಯ*vec4_ಮೌಲ್ಯ = dynamic_cast<anyedge_vec4_ಮೌಲ್ಯ*>(value);
      ಆದರೆ (!value) {
	    vec4_ಮೌಲ್ಯ = ಹೊಸ anyedge_vec4_ಮೌಲ್ಯ();
	    ಅಳಿಸು value;
	    value = vec4_ಮೌಲ್ಯ;
      }
      ಹಿಂತಿರುಗಿ vec4_ಮೌಲ್ಯ;
}

ವರ್ಗ anyedge_real_ಮೌಲ್ಯ : ಸಾರ್ವಜನಿಕ anyedge_ಮೌಲ್ಯ {

    ಸಾರ್ವಜನಿಕ:
      anyedge_real_ಮೌಲ್ಯ() : old_ಬಿಟ್ಟುಗಳು(0.0) {};
      ವಾಸ್ತವಿಕ ~anyedge_real_ಮೌಲ್ಯ() override {};

      ಶೂನ್ಯ reset() override { old_ಬಿಟ್ಟುಗಳು = 0.0; }

      ಶೂನ್ಯ ಗಣ(ದ್ವಿಗುಣ ಬಿಟ್) { old_ಬಿಟ್ಟುಗಳು = ಬಿಟ್; };

      ಶೂನ್ಯ duplicate(anyedge_ಮೌಲ್ಯ*&dup) override;

      ತಾರ್ಕಿಕ recv_real(ದ್ವಿಗುಣ ಬಿಟ್);

    ಖಾಸಗಿ:
      ದ್ವಿಗುಣ old_ಬಿಟ್ಟುಗಳು;
};

ಸ್ಥಿರ anyedge_real_ಮೌಲ್ಯ*get_real_ಮೌಲ್ಯ(anyedge_ಮೌಲ್ಯ*&value)
{
      anyedge_real_ಮೌಲ್ಯ*real_ಮೌಲ್ಯ = dynamic_cast<anyedge_real_ಮೌಲ್ಯ*>(value);
      ಆದರೆ (!value) {
	    real_ಮೌಲ್ಯ = ಹೊಸ anyedge_real_ಮೌಲ್ಯ();
	    ಅಳಿಸು value;
	    value = real_ಮೌಲ್ಯ;
      }
      ಹಿಂತಿರುಗಿ real_ಮೌಲ್ಯ;
}

ವರ್ಗ anyedge_string_ಮೌಲ್ಯ : ಸಾರ್ವಜನಿಕ anyedge_ಮೌಲ್ಯ {

    ಸಾರ್ವಜನಿಕ:
      anyedge_string_ಮೌಲ್ಯ() {};
      ವಾಸ್ತವಿಕ ~anyedge_string_ಮೌಲ್ಯ() override {};

      ಶೂನ್ಯ reset() override { old_ಬಿಟ್ಟುಗಳು.clear(); }

      ಶೂನ್ಯ ಗಣ(ಸ್ಥಿರಾಂಕ std::string&ಬಿಟ್) { old_ಬಿಟ್ಟುಗಳು = ಬಿಟ್; };

      ಶೂನ್ಯ duplicate(anyedge_ಮೌಲ್ಯ*&dup) override;

      ತಾರ್ಕಿಕ recv_string(ಸ್ಥಿರಾಂಕ std::string&ಬಿಟ್);

    ಖಾಸಗಿ:
      std::string old_ಬಿಟ್ಟುಗಳು;
};

ಸ್ಥಿರ anyedge_string_ಮೌಲ್ಯ*get_string_ಮೌಲ್ಯ(anyedge_ಮೌಲ್ಯ*&value)
{
      anyedge_string_ಮೌಲ್ಯ*string_ಮೌಲ್ಯ = dynamic_cast<anyedge_string_ಮೌಲ್ಯ*>(value);
      ಆದರೆ (!value) {
	    string_ಮೌಲ್ಯ = ಹೊಸ anyedge_string_ಮೌಲ್ಯ();
	    ಅಳಿಸು value;
	    value = string_ಮೌಲ್ಯ;
      }
      ಹಿಂತಿರುಗಿ string_ಮೌಲ್ಯ;
}

ರಚನೆ vvp_fun_anyedge_state_s : ಸಾರ್ವಜನಿಕ waitable_state_s {
      vvp_fun_anyedge_state_s()
      {
            ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < 4 ;  idx += 1)
                  ಕೊನೆಯ_ಮೌಲ್ಯ[idx] = 0;
      }

      ~vvp_fun_anyedge_state_s()
      {
            ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < 4 ;  idx += 1)
                  ಅಳಿಸು ಕೊನೆಯ_ಮೌಲ್ಯ[idx];
      }

      anyedge_ಮೌಲ್ಯ *ಕೊನೆಯ_ಮೌಲ್ಯ[4];
      vvp_fun_anyedge_state_s(ಸ್ಥಿರಾಂಕ vvp_fun_anyedge_state_s&) = ಅಳಿಸು;
      vvp_fun_anyedge_state_s& ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ vvp_fun_anyedge_state_s&) = ಅಳಿಸು;
};

vvp_fun_anyedge::vvp_fun_anyedge()
{
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < 4 ;  idx += 1)
	    ಕೊನೆಯ_ಮೌಲ್ಯ[idx] = 0;
}

vvp_fun_anyedge::~vvp_fun_anyedge()
{
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < 4 ;  idx += 1)
	    ಅಳಿಸು ಕೊನೆಯ_ಮೌಲ್ಯ[idx];
}

ಶೂನ್ಯ anyedge_vec4_ಮೌಲ್ಯ::duplicate(anyedge_ಮೌಲ್ಯ*&dup)
{
      anyedge_vec4_ಮೌಲ್ಯ*dup_vec4 = get_vec4_ಮೌಲ್ಯ(dup);
      ಪ್ರತಿಜ್ಞಾ(dup_vec4);
      dup_vec4->ಗಣ(old_ಬಿಟ್ಟುಗಳು);
}

ತಾರ್ಕಿಕ anyedge_vec4_ಮೌಲ್ಯ::recv_vec4(ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್)
{
      ತಾರ್ಕಿಕ flag = ಸುಳ್ಳು;

      ಆದರೆ (old_ಬಿಟ್ಟುಗಳು.size() != ಬಿಟ್.size()) {
	    ಆದರೆ (old_ಬಿಟ್ಟುಗಳು.size() == 0) {
		    // Special case: If we've not seen any input yet
		    // (old_bits.size()==0) then replace it will a reference
		    // vector that is 'bx. Then compare that with the input
		    // to see if we are processing a change from 'bx.
		  old_ಬಿಟ್ಟುಗಳು = vvp_vector4_t(ಬಿಟ್.size(), BIT4_X);
		  ಆದರೆ (old_ಬಿಟ್ಟುಗಳು.eeq(ಬಿಟ್))
			flag = ಸುಳ್ಳು;
		  ಇಲ್ಲದಿದ್ದರೆ
			flag = ನಿಜ;

	    } ಇಲ್ಲದಿದ್ದರೆ {
		  flag = ನಿಜ;
	    }

      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < ಬಿಟ್.size() ;  idx += 1) {
		  ಆದರೆ (old_ಬಿಟ್ಟುಗಳು.value(idx) != ಬಿಟ್.value(idx)) {
			flag = ನಿಜ;
			ನಿಲ್ಲಿಸು;
		  }
	    }
      }

      ಆದರೆ (flag) {
	    old_ಬಿಟ್ಟುಗಳು = ಬಿಟ್;
      }

      ಹಿಂತಿರುಗಿ flag;
}

ತಾರ್ಕಿಕ anyedge_vec4_ಮೌಲ್ಯ::recv_vec4_pv(ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್, ಚಿಹ್ನೆರಹಿತ ತಳ,
				      ಚಿಹ್ನೆರಹಿತ vwid)
{
      vvp_vector4_t tmp = old_ಬಿಟ್ಟುಗಳು;
      ಆದರೆ (tmp.size() == 0)
	    tmp = vvp_vector4_t(vwid, BIT4_Z);
      ಪ್ರತಿಜ್ಞಾ(ತಳ + ಬಿಟ್.size()<= vwid);
      ಪ್ರತಿಜ್ಞಾ(tmp.size() == vwid);
      tmp.ಗಣ_vec(ತಳ, ಬಿಟ್);

      ಹಿಂತಿರುಗಿ recv_vec4(tmp);
}

ಶೂನ್ಯ anyedge_real_ಮೌಲ್ಯ::duplicate(anyedge_ಮೌಲ್ಯ*&dup)
{
      anyedge_real_ಮೌಲ್ಯ*dup_real = get_real_ಮೌಲ್ಯ(dup);
      ಪ್ರತಿಜ್ಞಾ(dup_real);
      dup_real->ಗಣ(old_ಬಿಟ್ಟುಗಳು);
}

ತಾರ್ಕಿಕ anyedge_real_ಮೌಲ್ಯ::recv_real(ದ್ವಿಗುಣ ಬಿಟ್)
{
      ಆದರೆ (old_ಬಿಟ್ಟುಗಳು != ಬಿಟ್) {
	    old_ಬಿಟ್ಟುಗಳು = ಬಿಟ್;
            ಹಿಂತಿರುಗಿ ನಿಜ;
      }
      ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ಶೂನ್ಯ anyedge_string_ಮೌಲ್ಯ::duplicate(anyedge_ಮೌಲ್ಯ*&dup)
{
      anyedge_string_ಮೌಲ್ಯ*dup_string = get_string_ಮೌಲ್ಯ(dup);
      ಪ್ರತಿಜ್ಞಾ(dup_string);
      dup_string->ಗಣ(old_ಬಿಟ್ಟುಗಳು);
}

ತಾರ್ಕಿಕ anyedge_string_ಮೌಲ್ಯ::recv_string(ಸ್ಥಿರಾಂಕ std::string&ಬಿಟ್)
{
      ಆದರೆ (old_ಬಿಟ್ಟುಗಳು != ಬಿಟ್) {
	    old_ಬಿಟ್ಟುಗಳು = ಬಿಟ್;
            ಹಿಂತಿರುಗಿ ನಿಜ;
      }
      ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

vvp_fun_anyedge_sa::vvp_fun_anyedge_sa()
: ಎಳೆಗಳು_2(0)
{
}

vvp_fun_anyedge_sa::~vvp_fun_anyedge_sa()
{
}

vthread_t vvp_fun_anyedge_sa::ಕೂಡಿಸು_ಕಾಯುವಿಕೆ_thread(vthread_t thread)
{
      vthread_t tmp = ಎಳೆಗಳು_2;
      ಎಳೆಗಳು_2 = thread;

      ಹಿಂತಿರುಗಿ tmp;
}

ಶೂನ್ಯ vvp_fun_anyedge_sa::recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                                   vvp_context_t)
{
      anyedge_vec4_ಮೌಲ್ಯ*value = get_vec4_ಮೌಲ್ಯ(ಕೊನೆಯ_ಮೌಲ್ಯ[ಪೋರ್ಟ್.ಪೋರ್ಟ್()]);
      ಪ್ರತಿಜ್ಞಾ(value);
      ಆದರೆ (value->recv_vec4(ಬಿಟ್)) {
	    ಸತತ_ಕಾಯುವಿಕೆ_ಎಳೆಗಳು(ಎಳೆಗಳು_2);
	    vvp_net_t*net = ಪೋರ್ಟ್.ptr();
	    net->send_vec4(ಬಿಟ್, 0);
      }
}

ಶೂನ್ಯ vvp_fun_anyedge_sa::recv_vec4_pv(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
				      ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t)
{
      anyedge_vec4_ಮೌಲ್ಯ*value = get_vec4_ಮೌಲ್ಯ(ಕೊನೆಯ_ಮೌಲ್ಯ[ಪೋರ್ಟ್.ಪೋರ್ಟ್()]);
      ಪ್ರತಿಜ್ಞಾ(value);
      ಆದರೆ (value->recv_vec4_pv(ಬಿಟ್, ತಳ, vwid)) {
	    ಸತತ_ಕಾಯುವಿಕೆ_ಎಳೆಗಳು(ಎಳೆಗಳು_2);
	    vvp_net_t*net = ಪೋರ್ಟ್.ptr();
	    net->send_vec4(ಬಿಟ್, 0);
      }
}

ಶೂನ್ಯ vvp_fun_anyedge_sa::recv_real(vvp_net_ptr_t ಪೋರ್ಟ್, ದ್ವಿಗುಣ ಬಿಟ್,
                                   vvp_context_t)
{
      anyedge_real_ಮೌಲ್ಯ*value = get_real_ಮೌಲ್ಯ(ಕೊನೆಯ_ಮೌಲ್ಯ[ಪೋರ್ಟ್.ಪೋರ್ಟ್()]);
      ಪ್ರತಿಜ್ಞಾ(value);
      ಆದರೆ (value->recv_real(ಬಿಟ್)) {
	    ಸತತ_ಕಾಯುವಿಕೆ_ಎಳೆಗಳು(ಎಳೆಗಳು_2);
	    vvp_net_t*net = ಪೋರ್ಟ್.ptr();
	    net->send_vec4(vvp_vector4_t(), 0);
      }
}

ಶೂನ್ಯ vvp_fun_anyedge_sa::recv_string(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ std::string&ಬಿಟ್,
				     vvp_context_t)
{
      anyedge_string_ಮೌಲ್ಯ*value = get_string_ಮೌಲ್ಯ(ಕೊನೆಯ_ಮೌಲ್ಯ[ಪೋರ್ಟ್.ಪೋರ್ಟ್()]);
      ಪ್ರತಿಜ್ಞಾ(value);
      ಆದರೆ (value->recv_string(ಬಿಟ್)) {
	    ಸತತ_ಕಾಯುವಿಕೆ_ಎಳೆಗಳು(ಎಳೆಗಳು_2);
	    vvp_net_t*net = ಪೋರ್ಟ್.ptr();
	    net->send_vec4(vvp_vector4_t(), 0);
      }
}

/*
 * An anyedge receiving an object should do nothing with it, but should
 * trigger waiting threads.
 */
ಶೂನ್ಯ vvp_fun_anyedge_sa::recv_object(vvp_net_ptr_t ಪೋರ್ಟ್, vvp_object_t,
				     vvp_context_t)
{
      ಸತತ_ಕಾಯುವಿಕೆ_ಎಳೆಗಳು(ಎಳೆಗಳು_2);
      vvp_net_t*net = ಪೋರ್ಟ್.ptr();
      net->send_vec4(vvp_vector4_t(), 0);
}

vvp_fun_anyedge_aa::vvp_fun_anyedge_aa()
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_ಕೂಡಿಸು_ಅಂಶ_ಗಮ್ಯ_context(ಇದು, context_scope_);
}

vvp_fun_anyedge_aa::~vvp_fun_anyedge_aa()
{
}

ಶೂನ್ಯ vvp_fun_anyedge_aa::alloc_instance(vvp_context_t context)
{
      vvp_ಗಣ_context_ಅಂಶ(context, context_idx_, ಹೊಸ vvp_fun_anyedge_state_s);
      reset_instance(context);
}

ಶೂನ್ಯ vvp_fun_anyedge_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
            (vvp_get_context_ಅಂಶ(context, context_idx_));

      state->ಎಳೆಗಳು = 0;
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < 4 ;  idx += 1) {
	    ಆದರೆ (ಕೊನೆಯ_ಮೌಲ್ಯ[idx])
	          ಕೊನೆಯ_ಮೌಲ್ಯ[idx]->duplicate(state->ಕೊನೆಯ_ಮೌಲ್ಯ[idx]);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (state->ಕೊನೆಯ_ಮೌಲ್ಯ[idx])
		  state->ಕೊನೆಯ_ಮೌಲ್ಯ[idx]->reset();
      }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಶೂನ್ಯ vvp_fun_anyedge_aa::free_instance(vvp_context_t context)
{
      vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
            (vvp_get_context_ಅಂಶ(context, context_idx_));
      ಅಳಿಸು state;
}
#ಆದರೆ_ಅಂತ್ಯ

vthread_t vvp_fun_anyedge_aa::ಕೂಡಿಸು_ಕಾಯುವಿಕೆ_thread(vthread_t thread)
{
      vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
            (vthread_get_wt_context_ಅಂಶ(context_idx_));

      vthread_t tmp = state->ಎಳೆಗಳು;
      state->ಎಳೆಗಳು = thread;

      ಹಿಂತಿರುಗಿ tmp;
}

ಶೂನ್ಯ vvp_fun_anyedge_aa::recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                                   vvp_context_t context)
{
      ಆದರೆ (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_ಅಂಶ(context, context_idx_));

            anyedge_vec4_ಮೌಲ್ಯ*value = get_vec4_ಮೌಲ್ಯ(state->ಕೊನೆಯ_ಮೌಲ್ಯ[ಪೋರ್ಟ್.ಪೋರ್ಟ್()]);
            ಪ್ರತಿಜ್ಞಾ(value);
            ಆದರೆ (value->recv_vec4(ಬಿಟ್)) {
                  ಸತತ_ಕಾಯುವಿಕೆ_ಎಳೆಗಳು(state->ಎಳೆಗಳು);
                  vvp_net_t*net = ಪೋರ್ಟ್.ptr();
                  net->send_vec4(ಬಿಟ್, context);
            }
      } ಇಲ್ಲದಿದ್ದರೆ {
            context = context_scope_->ಜೀವಂತ_contexts;
            ಆಗ (context) {
                  recv_vec4(ಪೋರ್ಟ್, ಬಿಟ್, context);
                  context = vvp_get_ಮುಂದಿನ_context(context);
            }
            anyedge_vec4_ಮೌಲ್ಯ*value = get_vec4_ಮೌಲ್ಯ(ಕೊನೆಯ_ಮೌಲ್ಯ[ಪೋರ್ಟ್.ಪೋರ್ಟ್()]);
            ಪ್ರತಿಜ್ಞಾ(value);
            value->ಗಣ(ಬಿಟ್);
      }
}

ಶೂನ್ಯ vvp_fun_anyedge_aa::recv_real(vvp_net_ptr_t ಪೋರ್ಟ್, ದ್ವಿಗುಣ ಬಿಟ್,
                                   vvp_context_t context)
{
      ಆದರೆ (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_ಅಂಶ(context, context_idx_));

            anyedge_real_ಮೌಲ್ಯ*value = get_real_ಮೌಲ್ಯ(state->ಕೊನೆಯ_ಮೌಲ್ಯ[ಪೋರ್ಟ್.ಪೋರ್ಟ್()]);
            ಪ್ರತಿಜ್ಞಾ(value);
            ಆದರೆ (value->recv_real(ಬಿಟ್)) {
                  ಸತತ_ಕಾಯುವಿಕೆ_ಎಳೆಗಳು(state->ಎಳೆಗಳು);
                  vvp_net_t*net = ಪೋರ್ಟ್.ptr();
                  net->send_vec4(vvp_vector4_t(), context);
            }
      } ಇಲ್ಲದಿದ್ದರೆ {
            context = context_scope_->ಜೀವಂತ_contexts;
            ಆಗ (context) {
                  recv_real(ಪೋರ್ಟ್, ಬಿಟ್, context);
                  context = vvp_get_ಮುಂದಿನ_context(context);
            }
            anyedge_real_ಮೌಲ್ಯ*value = get_real_ಮೌಲ್ಯ(ಕೊನೆಯ_ಮೌಲ್ಯ[ಪೋರ್ಟ್.ಪೋರ್ಟ್()]);
            ಪ್ರತಿಜ್ಞಾ(value);
            value->ಗಣ(ಬಿಟ್);
      }
}

ಶೂನ್ಯ vvp_fun_anyedge_aa::recv_string(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ std::string&ಬಿಟ್,
				     vvp_context_t context)
{
      ಆದರೆ (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_ಅಂಶ(context, context_idx_));

            anyedge_string_ಮೌಲ್ಯ*value = get_string_ಮೌಲ್ಯ(state->ಕೊನೆಯ_ಮೌಲ್ಯ[ಪೋರ್ಟ್.ಪೋರ್ಟ್()]);
            ಪ್ರತಿಜ್ಞಾ(value);
            ಆದರೆ (value->recv_string(ಬಿಟ್)) {
                  ಸತತ_ಕಾಯುವಿಕೆ_ಎಳೆಗಳು(state->ಎಳೆಗಳು);
                  vvp_net_t*net = ಪೋರ್ಟ್.ptr();
                  net->send_vec4(vvp_vector4_t(), context);
            }
      } ಇಲ್ಲದಿದ್ದರೆ {
            context = context_scope_->ಜೀವಂತ_contexts;
            ಆಗ (context) {
                  recv_string(ಪೋರ್ಟ್, ಬಿಟ್, context);
                  context = vvp_get_ಮುಂದಿನ_context(context);
            }
            anyedge_string_ಮೌಲ್ಯ*value = get_string_ಮೌಲ್ಯ(ಕೊನೆಯ_ಮೌಲ್ಯ[ಪೋರ್ಟ್.ಪೋರ್ಟ್()]);
            ಪ್ರತಿಜ್ಞಾ(value);
            value->ಗಣ(ಬಿಟ್);
      }
}

vvp_fun_event_or::vvp_fun_event_or(vvp_net_t*ತಳ_net)
: ತಳ_net_2(ತಳ_net)
{
}

vvp_fun_event_or::~vvp_fun_event_or()
{
}

vvp_fun_event_or_sa::vvp_fun_event_or_sa(vvp_net_t*ತಳ_net)
: vvp_fun_event_or(ತಳ_net), ಎಳೆಗಳು_2(0)
{
}

vvp_fun_event_or_sa::~vvp_fun_event_or_sa()
{
}

vthread_t vvp_fun_event_or_sa::ಕೂಡಿಸು_ಕಾಯುವಿಕೆ_thread(vthread_t thread)
{
      vthread_t tmp = ಎಳೆಗಳು_2;
      ಎಳೆಗಳು_2 = thread;

      ಹಿಂತಿರುಗಿ tmp;
}

ಶೂನ್ಯ vvp_fun_event_or_sa::recv_vec4(vvp_net_ptr_t, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                                    vvp_context_t)
{
      ಸತತ_ಕಾಯುವಿಕೆ_ಎಳೆಗಳು(ಎಳೆಗಳು_2);
      ತಳ_net_2->send_vec4(ಬಿಟ್, 0);
}

vvp_fun_event_or_aa::vvp_fun_event_or_aa(vvp_net_t*ತಳ_net)
: vvp_fun_event_or(ತಳ_net)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_ಕೂಡಿಸು_ಅಂಶ_ಗಮ್ಯ_context(ಇದು, context_scope_);
}

vvp_fun_event_or_aa::~vvp_fun_event_or_aa()
{
}

ಶೂನ್ಯ vvp_fun_event_or_aa::alloc_instance(vvp_context_t context)
{
      vvp_ಗಣ_context_ಅಂಶ(context, context_idx_, ಹೊಸ waitable_state_s);
}

ಶೂನ್ಯ vvp_fun_event_or_aa::reset_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_ಅಂಶ(context, context_idx_));

      state->ಎಳೆಗಳು = 0;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಶೂನ್ಯ vvp_fun_event_or_aa::free_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_ಅಂಶ(context, context_idx_));
      ಅಳಿಸು state;
}
#ಆದರೆ_ಅಂತ್ಯ

vthread_t vvp_fun_event_or_aa::ಕೂಡಿಸು_ಕಾಯುವಿಕೆ_thread(vthread_t thread)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vthread_get_wt_context_ಅಂಶ(context_idx_));

      vthread_t tmp = state->ಎಳೆಗಳು;
      state->ಎಳೆಗಳು = thread;

      ಹಿಂತಿರುಗಿ tmp;
}

ಶೂನ್ಯ vvp_fun_event_or_aa::recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                                    vvp_context_t context)
{
      ಆದರೆ (context) {
            waitable_state_s*state = static_cast<waitable_state_s*>
                  (vvp_get_context_ಅಂಶ(context, context_idx_));

            ಸತತ_ಕಾಯುವಿಕೆ_ಎಳೆಗಳು(state->ಎಳೆಗಳು);
            ತಳ_net_2->send_vec4(ಬಿಟ್, context);
      } ಇಲ್ಲದಿದ್ದರೆ {
            context = context_scope_->ಜೀವಂತ_contexts;
            ಆಗ (context) {
                  recv_vec4(ಪೋರ್ಟ್, ಬಿಟ್, context);
                  context = vvp_get_ಮುಂದಿನ_context(context);
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
: vvp_named_event(h), ಎಳೆಗಳು_2(0)
{
}

vvp_named_event_sa::~vvp_named_event_sa()
{
}

vthread_t vvp_named_event_sa::ಕೂಡಿಸು_ಕಾಯುವಿಕೆ_thread(vthread_t thread)
{
      vthread_t tmp = ಎಳೆಗಳು_2;
      ಎಳೆಗಳು_2 = thread;

      ಹಿಂತಿರುಗಿ tmp;
}

ಶೂನ್ಯ vvp_named_event_sa::recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                                   vvp_context_t)
{
      ಸತತ_ಕಾಯುವಿಕೆ_ಎಳೆಗಳು(ಎಳೆಗಳು_2);
      vvp_net_t*net = ಪೋರ್ಟ್.ptr();
      net->send_vec4(ಬಿಟ್, 0);

      __vpiNamedEvent*obj = dynamic_cast<__vpiNamedEvent*>(handle_);
      ಪ್ರತಿಜ್ಞಾ(obj);
      obj->ಸತತ_vpi_callbacks();
}

vvp_named_event_aa::vvp_named_event_aa(__vpiHandle*h)
: vvp_named_event(h)
{
      context_idx_ = vpip_ಕೂಡಿಸು_ಅಂಶ_ಗಮ್ಯ_context(ಇದು, vpip_peek_context_scope());
}

vvp_named_event_aa::~vvp_named_event_aa()
{
}

ಶೂನ್ಯ vvp_named_event_aa::alloc_instance(vvp_context_t context)
{
      vvp_ಗಣ_context_ಅಂಶ(context, context_idx_, ಹೊಸ waitable_state_s);
}

ಶೂನ್ಯ vvp_named_event_aa::reset_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_ಅಂಶ(context, context_idx_));

      state->ಎಳೆಗಳು = 0;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಶೂನ್ಯ vvp_named_event_aa::free_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_ಅಂಶ(context, context_idx_));
      ಅಳಿಸು state;
}
#ಆದರೆ_ಅಂತ್ಯ

vthread_t vvp_named_event_aa::ಕೂಡಿಸು_ಕಾಯುವಿಕೆ_thread(vthread_t thread)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vthread_get_wt_context_ಅಂಶ(context_idx_));

      vthread_t tmp = state->ಎಳೆಗಳು;
      state->ಎಳೆಗಳು = thread;

      ಹಿಂತಿರುಗಿ tmp;
}

ಶೂನ್ಯ vvp_named_event_aa::recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                                   vvp_context_t context)
{
      ಪ್ರತಿಜ್ಞಾ(context);

      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_ಅಂಶ(context, context_idx_));

      ಸತತ_ಕಾಯುವಿಕೆ_ಎಳೆಗಳು(state->ಎಳೆಗಳು);
      vvp_net_t*net = ಪೋರ್ಟ್.ptr();
      net->send_vec4(ಬಿಟ್, context);
}

/*
**  Create an event functor
**  edge:  compile_event(label, type, argc, argv, debug_flag)
**  or:    compile_event(label, NULL, argc, argv, debug_flag)
**
**  Named events are handled elsewhere.
*/

ಸ್ಥಿರ ಶೂನ್ಯ compile_event_or(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);

ಶೂನ್ಯ compile_event(ಅಕ್ಷರ*label, ಅಕ್ಷರ*type, ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      vvp_net_fun_t*fun = 0;

      ಆದರೆ (type == 0) {
	    compile_event_or(label, argc, argv);
	    ಹಿಂತಿರುಗಿ;
      }

      ಆದರೆ (strcmp(type,"anyedge") == 0) {

	    ಬಿಡುಗಡೆ(type);

            ಆದರೆ (vpip_peek_current_scope()->is_automatic()) {
                  fun = ಹೊಸ vvp_fun_anyedge_aa;
            } ಇಲ್ಲದಿದ್ದರೆ {
                  fun = ಹೊಸ vvp_fun_anyedge_sa;
            }

      } ಇಲ್ಲದಿದ್ದರೆ {

	    vvp_fun_ಅಂಚು::ಅಂಚು_t ಅಂಚು_type = vvp_ಅಂಚು_none;

	    ಆದರೆ (strcmp(type,"posedge") == 0)
		  ಅಂಚು_type = vvp_ಅಂಚು_posedge;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type,"negedge") == 0)
		  ಅಂಚು_type = vvp_ಅಂಚು_negedge;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type,"edge") == 0)
		  ಅಂಚು_type = vvp_ಅಂಚು_ಅಂಚು;

	    ಪ್ರತಿಜ್ಞಾ(argc <= 4);
	    ಬಿಡುಗಡೆ(type);

            ಆದರೆ (vpip_peek_current_scope()->is_automatic()) {
                  fun = ಹೊಸ vvp_fun_ಅಂಚು_aa(ಅಂಚು_type);
            } ಇಲ್ಲದಿದ್ದರೆ {
                  fun = ಹೊಸ vvp_fun_ಅಂಚು_sa(ಅಂಚು_type);
            }

      }

      vvp_net_t* ptr = ಹೊಸ vvp_net_t;
      ptr->fun = fun;

      define_functor_ಚಿಹ್ನೆ(label, ptr);
      ಬಿಡುಗಡೆ(label);

      inputs_connect(ptr, argc, argv);
      ಬಿಡುಗಡೆ(argv);
}

ಸ್ಥಿರ ಶೂನ್ಯ compile_event_or(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      vvp_net_t*ತಳ_net = ಹೊಸ vvp_net_t;
      ಆದರೆ (vpip_peek_current_scope()->is_automatic()) {
            ತಳ_net->fun = ಹೊಸ vvp_fun_event_or_aa(ತಳ_net);
      } ಇಲ್ಲದಿದ್ದರೆ {
            ತಳ_net->fun = ಹೊಸ vvp_fun_event_or_sa(ತಳ_net);
      }
      define_functor_ಚಿಹ್ನೆ(label, ತಳ_net);
      ಬಿಡುಗಡೆ(label);

	/* This is a simplified version of a wide functor. We don't
	   care about the data values or what port they arrived on,
	   so we can use a single shared functor. */
      vvp_net_t*curr_net = ತಳ_net;
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < argc ;  idx += 1) {
	    ಆದರೆ (idx > 0 && (idx % 4) == 0) {
		  curr_net = ಹೊಸ vvp_net_t;
		  curr_net->fun = ತಳ_net->fun;
	    }
	    ಒಳಹರಿವು_connect(curr_net, idx % 4, argv[idx].ಪಠ್ಯ);
      }
      ಬಿಡುಗಡೆ(argv);
}

/*
 * This handles the compile of named events. This functor has no
 * inputs, it is only accessed by behavioral trigger statements, which
 * in vvp are %set instructions.
 */
ಶೂನ್ಯ compile_named_event(ಅಕ್ಷರ*label, ಅಕ್ಷರ*name, ತಾರ್ಕಿಕ local_flag)
{
      vvp_net_t*ptr = ಹೊಸ vvp_net_t;

      vpiHandle obj = vpip_make_named_event(name, ptr);

      ಆದರೆ (vpip_peek_current_scope()->is_automatic()) {
            ptr->fun = ಹೊಸ vvp_named_event_aa(obj);
      } ಇಲ್ಲದಿದ್ದರೆ {
            ptr->fun = ಹೊಸ vvp_named_event_sa(obj);
      }
      define_functor_ಚಿಹ್ನೆ(label, ptr);
      compile_vpi_ಚಿಹ್ನೆ(label, obj);
      ಆದರೆ (! local_flag) vpip_attach_ಗಮ್ಯ_current_scope(obj);

      ಬಿಡುಗಡೆ(label);
      ಅಳಿಸು[] name;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಶೂನ್ಯ named_event_delete(__vpiHandle*handle)
{
      ಅಳಿಸು dynamic_cast<__vpiNamedEvent *>(handle);
}
#ಆದರೆ_ಅಂತ್ಯ
