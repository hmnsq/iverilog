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

ಶೂನ್ಯ waitable_hooks_s::run_waiting_threads_(vthread_t&threads)
{
	// Run the non-blocking event controls.
      last = &event_ctls;
      ಪ್ರತಿ (evctl*cur = event_ctls; cur != 0;) {
	    ಯದಿ (cur->dec_and_run()) {
		  evctl*nxt = cur->next;
		  delete cur;
		  cur = nxt;
		  *last = cur;
	    } ಅನ್ಯಥಾ {
		  last = &(cur->next);
		  cur = cur->next;
	    }
      }

      vthread_t tmp = threads;
      ಯದಿ (tmp == 0) ಪ್ರತ್ಯಾಗಮ;
      threads = 0;

      vthread_schedule_list(tmp);
}

evctl::evctl(uint64_t ecount)
{
      ecount_ = ecount;
      next = 0;
}

ತಾರ್ಕಿಕ evctl::dec_and_run()
{
      ಪ್ರತಿಜ್ಞಾ(ecount_ != 0);

      ecount_ -= 1;
      ಯದಿ (ecount_ == 0) run_run();

      ಪ್ರತ್ಯಾಗಮ ecount_ == 0;
}

evctl_real::evctl_real(__vpiHandle*handle, ದ್ವಿಗುಣ value,
                       uint64_t ecount)
:evctl(ecount)
{
      handle_ = handle;
      ಮೌಲ್ಯ_2 = value;
}

ಶೂನ್ಯ evctl_real::run_run()
{
      t_vpi_ಮೌಲ್ಯ val;

      val.format = vpiRealVal;
      val.value.real = ಮೌಲ್ಯ_2;
      vpi_put_ಮೌಲ್ಯ(handle_, &val, 0, vpiNoDelay);
}

ಶೂನ್ಯ schedule_evctl(__vpiHandle*handle, ದ್ವಿಗುಣ value,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      ಪ್ರತಿಜ್ಞಾ(ep);
	// Now add this call to the end of the event list.
      *(ep->last) = ನೂತನ evctl_real(handle, value, ecount);
      ep->last = &((*(ep->last))->next);
}

evctl_vector::evctl_vector(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&value,
                           unsigned off, unsigned wid, uint64_t ecount)
:evctl(ecount), ptr_(ptr), ಮೌಲ್ಯ_2(value)
{
      off_ = off;
      wid_ = wid;
}

ಶೂನ್ಯ evctl_vector::run_run()
{
      ಯದಿ (wid_ != 0) {
	    vvp_send_vec4_pv(ptr_, ಮೌಲ್ಯ_2, off_, wid_, 0);
      } ಅನ್ಯಥಾ {
	    vvp_send_vec4(ptr_, ಮೌಲ್ಯ_2, 0);
      }
}

ಶೂನ್ಯ schedule_evctl(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&value,
                    unsigned offset, unsigned wid,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      ಪ್ರತಿಜ್ಞಾ(ep);
	// Now add this call to the end of the event list.
      *(ep->last) = ನೂತನ evctl_vector(ptr, value, offset, wid, ecount);
      ep->last = &((*(ep->last))->next);
}

evctl_array::evctl_array(vvp_array_t memory, unsigned index,
                         ಧ್ರುವ vvp_vector4_t&value, unsigned off,
                         uint64_t ecount)
:evctl(ecount), ಮೌಲ್ಯ_2(value)
{
      mem_ = memory;
      idx_ = index;
      off_ = off;
}

ಶೂನ್ಯ evctl_array::run_run()
{
      mem_->set_word(idx_, off_, ಮೌಲ್ಯ_2);
}

ಶೂನ್ಯ schedule_evctl(vvp_array_t memory, unsigned index,
                    ಧ್ರುವ vvp_vector4_t&value, unsigned offset,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      ಪ್ರತಿಜ್ಞಾ(ep);
	// Now add this call to the end of the event list.
      *(ep->last) = ನೂತನ evctl_array(memory, index, value, offset, ecount);
      ep->last = &((*(ep->last))->next);
}

evctl_array_r::evctl_array_r(vvp_array_t memory, unsigned index,
                             ದ್ವಿಗುಣ value, uint64_t ecount)
:evctl(ecount)
{
      mem_ = memory;
      idx_ = index;
      ಮೌಲ್ಯ_2 = value;
}

ಶೂನ್ಯ evctl_array_r::run_run()
{
      mem_->set_word(idx_, ಮೌಲ್ಯ_2);
}

ಶೂನ್ಯ schedule_evctl(vvp_array_t memory, unsigned index,
                    ದ್ವಿಗುಣ value,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      ಪ್ರತಿಜ್ಞಾ(ep);
	// Now add this call to the end of the event list.
      *(ep->last) = ನೂತನ evctl_array_r(memory, index, value, ecount);
      ep->last = &((*(ep->last))->next);
}

ಅನ್ತಃಸ್ಥ vvp_fun_edge::edge_t VVP_EDGE(vvp_bit4_t from, vvp_bit4_t to)
{
      ಪ್ರತ್ಯಾಗಮ 1 << ((from << 2) | to);
}

ಧ್ರುವ vvp_fun_edge::edge_t vvp_edge_posedge
      = VVP_EDGE(BIT4_0,BIT4_1)
      | VVP_EDGE(BIT4_0,BIT4_X)
      | VVP_EDGE(BIT4_0,BIT4_Z)
      | VVP_EDGE(BIT4_X,BIT4_1)
      | VVP_EDGE(BIT4_Z,BIT4_1)
      ;

ಧ್ರುವ vvp_fun_edge::edge_t vvp_edge_negedge
      = VVP_EDGE(BIT4_1,BIT4_0)
      | VVP_EDGE(BIT4_1,BIT4_X)
      | VVP_EDGE(BIT4_1,BIT4_Z)
      | VVP_EDGE(BIT4_X,BIT4_0)
      | VVP_EDGE(BIT4_Z,BIT4_0)
      ;

ಧ್ರುವ vvp_fun_edge::edge_t vvp_edge_edge
      = VVP_EDGE(BIT4_0,BIT4_1)
      | VVP_EDGE(BIT4_1,BIT4_0)
      | VVP_EDGE(BIT4_0,BIT4_X)
      | VVP_EDGE(BIT4_X,BIT4_0)
      | VVP_EDGE(BIT4_0,BIT4_Z)
      | VVP_EDGE(BIT4_Z,BIT4_0)
      | VVP_EDGE(BIT4_X,BIT4_1)
      | VVP_EDGE(BIT4_1,BIT4_X)
      | VVP_EDGE(BIT4_Z,BIT4_1)
      | VVP_EDGE(BIT4_1,BIT4_Z)
      ;

ಧ್ರುವ vvp_fun_edge::edge_t vvp_edge_none    = 0;

ಸಂರಚನಾ vvp_fun_edge_state_s : ಸಾರ್ವಜನಿಕ waitable_state_s {
      vvp_fun_edge_state_s()
      {
            ಪ್ರತಿ (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
                  bits[idx] = BIT4_X;
      }

      vvp_bit4_t bits[4];
};

vvp_fun_edge::vvp_fun_edge(edge_t e)
: edge_(e)
{
      ಪ್ರತಿ (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
            bits_[idx] = BIT4_X;
}

vvp_fun_edge::~vvp_fun_edge()
{
}

ತಾರ್ಕಿಕ vvp_fun_edge::recv_vec4_(ಧ್ರುವ vvp_vector4_t&bit,
                              vvp_bit4_t&old_bit, vthread_t&threads)
{
	/* See what kind of edge this represents. */
      edge_t mask = VVP_EDGE(old_bit, bit.value(0));

	/* Save the current input for the next time around. */
      old_bit = bit.value(0);

      ಯದಿ ((edge_ == vvp_edge_none) || (edge_ & mask)) {
	    run_waiting_threads_(threads);
            ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
      }
      ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

vvp_fun_edge_sa::vvp_fun_edge_sa(edge_t e)
: vvp_fun_edge(e), threads_(0)
{
}

vvp_fun_edge_sa::~vvp_fun_edge_sa()
{
}

vthread_t vvp_fun_edge_sa::add_waiting_thread(vthread_t thread)
{
      vthread_t tmp = threads_;
      threads_ = thread;

      ಪ್ರತ್ಯಾಗಮ tmp;
}

ಶೂನ್ಯ vvp_fun_edge_sa::recv_vec4(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
                                vvp_context_t)
{
      ಯದಿ (recv_vec4_(bit, bits_[port.port()], threads_)) {
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(bit, 0);
      }
}

ಶೂನ್ಯ vvp_fun_edge_sa::recv_vec4_pv(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
				   unsigned base, unsigned vwid, vvp_context_t)
{
      ಪ್ರತಿಜ್ಞಾ(base == 0);
      ಯದಿ (recv_vec4_(bit, bits_[port.port()], threads_)) {
	    vvp_net_t*net = port.ptr();
	    net->send_vec4_pv(bit, base, vwid, 0);
      }
}

vvp_fun_edge_aa::vvp_fun_edge_aa(edge_t e)
: vvp_fun_edge(e)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_add_item_to_context(ಏತತ್, context_scope_);
}

vvp_fun_edge_aa::~vvp_fun_edge_aa()
{
}

ಶೂನ್ಯ vvp_fun_edge_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, ನೂತನ vvp_fun_edge_state_s);
      reset_instance(context);
}

ಶೂನ್ಯ vvp_fun_edge_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_edge_state_s*state = static_cast<vvp_fun_edge_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->threads = 0;
      ಪ್ರತಿ (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
            state->bits[idx] = bits_[idx];
}

#ifdef CHECK_WITH_VALGRIND
ಶೂನ್ಯ vvp_fun_edge_aa::free_instance(vvp_context_t context)
{
      vvp_fun_edge_state_s*state = static_cast<vvp_fun_edge_state_s*>
            (vvp_get_context_item(context, context_idx_));
      delete state;
}
#endif

vthread_t vvp_fun_edge_aa::add_waiting_thread(vthread_t thread)
{
      vvp_fun_edge_state_s*state = static_cast<vvp_fun_edge_state_s*>
            (vthread_get_wt_context_item(context_idx_));

      vthread_t tmp = state->threads;
      state->threads = thread;

      ಪ್ರತ್ಯಾಗಮ tmp;
}

ಶೂನ್ಯ vvp_fun_edge_aa::recv_vec4(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
                                vvp_context_t context)
{
      ಯದಿ (context) {
            vvp_fun_edge_state_s*state = static_cast<vvp_fun_edge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            ಯದಿ (recv_vec4_(bit, state->bits[port.port()], state->threads)) {
                  vvp_net_t*net = port.ptr();
                  net->send_vec4(bit, context);
            }
      } ಅನ್ಯಥಾ {
            context = context_scope_->live_contexts;
            ಯಾವತ್ (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
            bits_[port.port()] = bit.value(0);
      }
}

ವರ್ಗ anyedge_ಮೌಲ್ಯ {

    ಸಾರ್ವಜನಿಕ:
      anyedge_ಮೌಲ್ಯ() {};
      ಆಭಾಸ ~anyedge_ಮೌಲ್ಯ() {};

      ಆಭಾಸ ಶೂನ್ಯ reset() = 0;

      ಆಭಾಸ ಶೂನ್ಯ duplicate(anyedge_ಮೌಲ್ಯ*&dup) = 0;
};

ವರ್ಗ anyedge_vec4_ಮೌಲ್ಯ : ಸಾರ್ವಜನಿಕ anyedge_ಮೌಲ್ಯ {

    ಸಾರ್ವಜನಿಕ:
      anyedge_vec4_ಮೌಲ್ಯ() {};
      ಆಭಾಸ ~anyedge_vec4_ಮೌಲ್ಯ() override {};

      ಶೂನ್ಯ reset() override { old_bits.set_to_x(); }

      ಶೂನ್ಯ set(ಧ್ರುವ vvp_vector4_t&bit) { old_bits = bit; };

      ಶೂನ್ಯ duplicate(anyedge_ಮೌಲ್ಯ*&dup) override;

      ತಾರ್ಕಿಕ recv_vec4(ಧ್ರುವ vvp_vector4_t&bit);

      ತಾರ್ಕಿಕ recv_vec4_pv(ಧ್ರುವ vvp_vector4_t&bit, unsigned base,
			unsigned vwid);

    ಗುಪ್ತ:
      vvp_vector4_t old_bits;
};

ಸ್ಥಿರ anyedge_vec4_ಮೌಲ್ಯ*get_vec4_ಮೌಲ್ಯ(anyedge_ಮೌಲ್ಯ*&value)
{
      anyedge_vec4_ಮೌಲ್ಯ*vec4_ಮೌಲ್ಯ = dynamic_cast<anyedge_vec4_ಮೌಲ್ಯ*>(value);
      ಯದಿ (!value) {
	    vec4_ಮೌಲ್ಯ = ನೂತನ anyedge_vec4_ಮೌಲ್ಯ();
	    delete value;
	    value = vec4_ಮೌಲ್ಯ;
      }
      ಪ್ರತ್ಯಾಗಮ vec4_ಮೌಲ್ಯ;
}

ವರ್ಗ anyedge_real_ಮೌಲ್ಯ : ಸಾರ್ವಜನಿಕ anyedge_ಮೌಲ್ಯ {

    ಸಾರ್ವಜನಿಕ:
      anyedge_real_ಮೌಲ್ಯ() : old_bits(0.0) {};
      ಆಭಾಸ ~anyedge_real_ಮೌಲ್ಯ() override {};

      ಶೂನ್ಯ reset() override { old_bits = 0.0; }

      ಶೂನ್ಯ set(ದ್ವಿಗುಣ bit) { old_bits = bit; };

      ಶೂನ್ಯ duplicate(anyedge_ಮೌಲ್ಯ*&dup) override;

      ತಾರ್ಕಿಕ recv_real(ದ್ವಿಗುಣ bit);

    ಗುಪ್ತ:
      ದ್ವಿಗುಣ old_bits;
};

ಸ್ಥಿರ anyedge_real_ಮೌಲ್ಯ*get_real_ಮೌಲ್ಯ(anyedge_ಮೌಲ್ಯ*&value)
{
      anyedge_real_ಮೌಲ್ಯ*real_ಮೌಲ್ಯ = dynamic_cast<anyedge_real_ಮೌಲ್ಯ*>(value);
      ಯದಿ (!value) {
	    real_ಮೌಲ್ಯ = ನೂತನ anyedge_real_ಮೌಲ್ಯ();
	    delete value;
	    value = real_ಮೌಲ್ಯ;
      }
      ಪ್ರತ್ಯಾಗಮ real_ಮೌಲ್ಯ;
}

ವರ್ಗ anyedge_string_ಮೌಲ್ಯ : ಸಾರ್ವಜನಿಕ anyedge_ಮೌಲ್ಯ {

    ಸಾರ್ವಜನಿಕ:
      anyedge_string_ಮೌಲ್ಯ() {};
      ಆಭಾಸ ~anyedge_string_ಮೌಲ್ಯ() override {};

      ಶೂನ್ಯ reset() override { old_bits.clear(); }

      ಶೂನ್ಯ set(ಧ್ರುವ std::string&bit) { old_bits = bit; };

      ಶೂನ್ಯ duplicate(anyedge_ಮೌಲ್ಯ*&dup) override;

      ತಾರ್ಕಿಕ recv_string(ಧ್ರುವ std::string&bit);

    ಗುಪ್ತ:
      std::string old_bits;
};

ಸ್ಥಿರ anyedge_string_ಮೌಲ್ಯ*get_string_ಮೌಲ್ಯ(anyedge_ಮೌಲ್ಯ*&value)
{
      anyedge_string_ಮೌಲ್ಯ*string_ಮೌಲ್ಯ = dynamic_cast<anyedge_string_ಮೌಲ್ಯ*>(value);
      ಯದಿ (!value) {
	    string_ಮೌಲ್ಯ = ನೂತನ anyedge_string_ಮೌಲ್ಯ();
	    delete value;
	    value = string_ಮೌಲ್ಯ;
      }
      ಪ್ರತ್ಯಾಗಮ string_ಮೌಲ್ಯ;
}

ಸಂರಚನಾ vvp_fun_anyedge_state_s : ಸಾರ್ವಜನಿಕ waitable_state_s {
      vvp_fun_anyedge_state_s()
      {
            ಪ್ರತಿ (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
                  last_ಮೌಲ್ಯ[idx] = 0;
      }

      ~vvp_fun_anyedge_state_s()
      {
            ಪ್ರತಿ (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
                  delete last_ಮೌಲ್ಯ[idx];
      }

      anyedge_ಮೌಲ್ಯ *last_ಮೌಲ್ಯ[4];
      vvp_fun_anyedge_state_s(ಧ್ರುವ vvp_fun_anyedge_state_s&) = delete;
      vvp_fun_anyedge_state_s& operator=(ಧ್ರುವ vvp_fun_anyedge_state_s&) = delete;
};

vvp_fun_anyedge::vvp_fun_anyedge()
{
      ಪ್ರತಿ (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
	    last_ಮೌಲ್ಯ[idx] = 0;
}

vvp_fun_anyedge::~vvp_fun_anyedge()
{
      ಪ್ರತಿ (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
	    delete last_ಮೌಲ್ಯ[idx];
}

ಶೂನ್ಯ anyedge_vec4_ಮೌಲ್ಯ::duplicate(anyedge_ಮೌಲ್ಯ*&dup)
{
      anyedge_vec4_ಮೌಲ್ಯ*dup_vec4 = get_vec4_ಮೌಲ್ಯ(dup);
      ಪ್ರತಿಜ್ಞಾ(dup_vec4);
      dup_vec4->set(old_bits);
}

ತಾರ್ಕಿಕ anyedge_vec4_ಮೌಲ್ಯ::recv_vec4(ಧ್ರುವ vvp_vector4_t&bit)
{
      ತಾರ್ಕಿಕ flag = ಅಸತ್ಯ;

      ಯದಿ (old_bits.size() != bit.size()) {
	    ಯದಿ (old_bits.size() == 0) {
		    // Special case: If we've not seen any input yet
		    // (old_bits.size()==0) then replace it will a reference
		    // vector that is 'bx. Then compare that with the input
		    // to see if we are processing a change from 'bx.
		  old_bits = vvp_vector4_t(bit.size(), BIT4_X);
		  ಯದಿ (old_bits.eeq(bit))
			flag = ಅಸತ್ಯ;
		  ಅನ್ಯಥಾ
			flag = ಸತ್ಯ;

	    } ಅನ್ಯಥಾ {
		  flag = ಸತ್ಯ;
	    }

      } ಅನ್ಯಥಾ {
	    ಪ್ರತಿ (unsigned idx = 0 ;  idx < bit.size() ;  idx += 1) {
		  ಯದಿ (old_bits.value(idx) != bit.value(idx)) {
			flag = ಸತ್ಯ;
			ಭಙ್ಗ;
		  }
	    }
      }

      ಯದಿ (flag) {
	    old_bits = bit;
      }

      ಪ್ರತ್ಯಾಗಮ flag;
}

ತಾರ್ಕಿಕ anyedge_vec4_ಮೌಲ್ಯ::recv_vec4_pv(ಧ್ರುವ vvp_vector4_t&bit, unsigned base,
				      unsigned vwid)
{
      vvp_vector4_t tmp = old_bits;
      ಯದಿ (tmp.size() == 0)
	    tmp = vvp_vector4_t(vwid, BIT4_Z);
      ಪ್ರತಿಜ್ಞಾ(base + bit.size()<= vwid);
      ಪ್ರತಿಜ್ಞಾ(tmp.size() == vwid);
      tmp.set_vec(base, bit);

      ಪ್ರತ್ಯಾಗಮ recv_vec4(tmp);
}

ಶೂನ್ಯ anyedge_real_ಮೌಲ್ಯ::duplicate(anyedge_ಮೌಲ್ಯ*&dup)
{
      anyedge_real_ಮೌಲ್ಯ*dup_real = get_real_ಮೌಲ್ಯ(dup);
      ಪ್ರತಿಜ್ಞಾ(dup_real);
      dup_real->set(old_bits);
}

ತಾರ್ಕಿಕ anyedge_real_ಮೌಲ್ಯ::recv_real(ದ್ವಿಗುಣ bit)
{
      ಯದಿ (old_bits != bit) {
	    old_bits = bit;
            ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
      }
      ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

ಶೂನ್ಯ anyedge_string_ಮೌಲ್ಯ::duplicate(anyedge_ಮೌಲ್ಯ*&dup)
{
      anyedge_string_ಮೌಲ್ಯ*dup_string = get_string_ಮೌಲ್ಯ(dup);
      ಪ್ರತಿಜ್ಞಾ(dup_string);
      dup_string->set(old_bits);
}

ತಾರ್ಕಿಕ anyedge_string_ಮೌಲ್ಯ::recv_string(ಧ್ರುವ std::string&bit)
{
      ಯದಿ (old_bits != bit) {
	    old_bits = bit;
            ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
      }
      ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

vvp_fun_anyedge_sa::vvp_fun_anyedge_sa()
: threads_(0)
{
}

vvp_fun_anyedge_sa::~vvp_fun_anyedge_sa()
{
}

vthread_t vvp_fun_anyedge_sa::add_waiting_thread(vthread_t thread)
{
      vthread_t tmp = threads_;
      threads_ = thread;

      ಪ್ರತ್ಯಾಗಮ tmp;
}

ಶೂನ್ಯ vvp_fun_anyedge_sa::recv_vec4(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
                                   vvp_context_t)
{
      anyedge_vec4_ಮೌಲ್ಯ*value = get_vec4_ಮೌಲ್ಯ(last_ಮೌಲ್ಯ[port.port()]);
      ಪ್ರತಿಜ್ಞಾ(value);
      ಯದಿ (value->recv_vec4(bit)) {
	    run_waiting_threads_(threads_);
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(bit, 0);
      }
}

ಶೂನ್ಯ vvp_fun_anyedge_sa::recv_vec4_pv(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
				      unsigned base, unsigned vwid, vvp_context_t)
{
      anyedge_vec4_ಮೌಲ್ಯ*value = get_vec4_ಮೌಲ್ಯ(last_ಮೌಲ್ಯ[port.port()]);
      ಪ್ರತಿಜ್ಞಾ(value);
      ಯದಿ (value->recv_vec4_pv(bit, base, vwid)) {
	    run_waiting_threads_(threads_);
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(bit, 0);
      }
}

ಶೂನ್ಯ vvp_fun_anyedge_sa::recv_real(vvp_net_ptr_t port, ದ್ವಿಗುಣ bit,
                                   vvp_context_t)
{
      anyedge_real_ಮೌಲ್ಯ*value = get_real_ಮೌಲ್ಯ(last_ಮೌಲ್ಯ[port.port()]);
      ಪ್ರತಿಜ್ಞಾ(value);
      ಯದಿ (value->recv_real(bit)) {
	    run_waiting_threads_(threads_);
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(vvp_vector4_t(), 0);
      }
}

ಶೂನ್ಯ vvp_fun_anyedge_sa::recv_string(vvp_net_ptr_t port, ಧ್ರುವ std::string&bit,
				     vvp_context_t)
{
      anyedge_string_ಮೌಲ್ಯ*value = get_string_ಮೌಲ್ಯ(last_ಮೌಲ್ಯ[port.port()]);
      ಪ್ರತಿಜ್ಞಾ(value);
      ಯದಿ (value->recv_string(bit)) {
	    run_waiting_threads_(threads_);
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(vvp_vector4_t(), 0);
      }
}

/*
 * An anyedge receiving an object should do nothing with it, but should
 * trigger waiting threads.
 */
ಶೂನ್ಯ vvp_fun_anyedge_sa::recv_object(vvp_net_ptr_t port, vvp_object_t,
				     vvp_context_t)
{
      run_waiting_threads_(threads_);
      vvp_net_t*net = port.ptr();
      net->send_vec4(vvp_vector4_t(), 0);
}

vvp_fun_anyedge_aa::vvp_fun_anyedge_aa()
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_add_item_to_context(ಏತತ್, context_scope_);
}

vvp_fun_anyedge_aa::~vvp_fun_anyedge_aa()
{
}

ಶೂನ್ಯ vvp_fun_anyedge_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, ನೂತನ vvp_fun_anyedge_state_s);
      reset_instance(context);
}

ಶೂನ್ಯ vvp_fun_anyedge_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->threads = 0;
      ಪ್ರತಿ (unsigned idx = 0 ;  idx < 4 ;  idx += 1) {
	    ಯದಿ (last_ಮೌಲ್ಯ[idx])
	          last_ಮೌಲ್ಯ[idx]->duplicate(state->last_ಮೌಲ್ಯ[idx]);
	    ಅನ್ಯಥಾ ಯದಿ (state->last_ಮೌಲ್ಯ[idx])
		  state->last_ಮೌಲ್ಯ[idx]->reset();
      }
}

#ifdef CHECK_WITH_VALGRIND
ಶೂನ್ಯ vvp_fun_anyedge_aa::free_instance(vvp_context_t context)
{
      vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
            (vvp_get_context_item(context, context_idx_));
      delete state;
}
#endif

vthread_t vvp_fun_anyedge_aa::add_waiting_thread(vthread_t thread)
{
      vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
            (vthread_get_wt_context_item(context_idx_));

      vthread_t tmp = state->threads;
      state->threads = thread;

      ಪ್ರತ್ಯಾಗಮ tmp;
}

ಶೂನ್ಯ vvp_fun_anyedge_aa::recv_vec4(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
                                   vvp_context_t context)
{
      ಯದಿ (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            anyedge_vec4_ಮೌಲ್ಯ*value = get_vec4_ಮೌಲ್ಯ(state->last_ಮೌಲ್ಯ[port.port()]);
            ಪ್ರತಿಜ್ಞಾ(value);
            ಯದಿ (value->recv_vec4(bit)) {
                  run_waiting_threads_(state->threads);
                  vvp_net_t*net = port.ptr();
                  net->send_vec4(bit, context);
            }
      } ಅನ್ಯಥಾ {
            context = context_scope_->live_contexts;
            ಯಾವತ್ (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
            anyedge_vec4_ಮೌಲ್ಯ*value = get_vec4_ಮೌಲ್ಯ(last_ಮೌಲ್ಯ[port.port()]);
            ಪ್ರತಿಜ್ಞಾ(value);
            value->set(bit);
      }
}

ಶೂನ್ಯ vvp_fun_anyedge_aa::recv_real(vvp_net_ptr_t port, ದ್ವಿಗುಣ bit,
                                   vvp_context_t context)
{
      ಯದಿ (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            anyedge_real_ಮೌಲ್ಯ*value = get_real_ಮೌಲ್ಯ(state->last_ಮೌಲ್ಯ[port.port()]);
            ಪ್ರತಿಜ್ಞಾ(value);
            ಯದಿ (value->recv_real(bit)) {
                  run_waiting_threads_(state->threads);
                  vvp_net_t*net = port.ptr();
                  net->send_vec4(vvp_vector4_t(), context);
            }
      } ಅನ್ಯಥಾ {
            context = context_scope_->live_contexts;
            ಯಾವತ್ (context) {
                  recv_real(port, bit, context);
                  context = vvp_get_next_context(context);
            }
            anyedge_real_ಮೌಲ್ಯ*value = get_real_ಮೌಲ್ಯ(last_ಮೌಲ್ಯ[port.port()]);
            ಪ್ರತಿಜ್ಞಾ(value);
            value->set(bit);
      }
}

ಶೂನ್ಯ vvp_fun_anyedge_aa::recv_string(vvp_net_ptr_t port, ಧ್ರುವ std::string&bit,
				     vvp_context_t context)
{
      ಯದಿ (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            anyedge_string_ಮೌಲ್ಯ*value = get_string_ಮೌಲ್ಯ(state->last_ಮೌಲ್ಯ[port.port()]);
            ಪ್ರತಿಜ್ಞಾ(value);
            ಯದಿ (value->recv_string(bit)) {
                  run_waiting_threads_(state->threads);
                  vvp_net_t*net = port.ptr();
                  net->send_vec4(vvp_vector4_t(), context);
            }
      } ಅನ್ಯಥಾ {
            context = context_scope_->live_contexts;
            ಯಾವತ್ (context) {
                  recv_string(port, bit, context);
                  context = vvp_get_next_context(context);
            }
            anyedge_string_ಮೌಲ್ಯ*value = get_string_ಮೌಲ್ಯ(last_ಮೌಲ್ಯ[port.port()]);
            ಪ್ರತಿಜ್ಞಾ(value);
            value->set(bit);
      }
}

vvp_fun_event_or::vvp_fun_event_or(vvp_net_t*base_net)
: base_net_(base_net)
{
}

vvp_fun_event_or::~vvp_fun_event_or()
{
}

vvp_fun_event_or_sa::vvp_fun_event_or_sa(vvp_net_t*base_net)
: vvp_fun_event_or(base_net), threads_(0)
{
}

vvp_fun_event_or_sa::~vvp_fun_event_or_sa()
{
}

vthread_t vvp_fun_event_or_sa::add_waiting_thread(vthread_t thread)
{
      vthread_t tmp = threads_;
      threads_ = thread;

      ಪ್ರತ್ಯಾಗಮ tmp;
}

ಶೂನ್ಯ vvp_fun_event_or_sa::recv_vec4(vvp_net_ptr_t, ಧ್ರುವ vvp_vector4_t&bit,
                                    vvp_context_t)
{
      run_waiting_threads_(threads_);
      base_net_->send_vec4(bit, 0);
}

vvp_fun_event_or_aa::vvp_fun_event_or_aa(vvp_net_t*base_net)
: vvp_fun_event_or(base_net)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_add_item_to_context(ಏತತ್, context_scope_);
}

vvp_fun_event_or_aa::~vvp_fun_event_or_aa()
{
}

ಶೂನ್ಯ vvp_fun_event_or_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, ನೂತನ waitable_state_s);
}

ಶೂನ್ಯ vvp_fun_event_or_aa::reset_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->threads = 0;
}

#ifdef CHECK_WITH_VALGRIND
ಶೂನ್ಯ vvp_fun_event_or_aa::free_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_item(context, context_idx_));
      delete state;
}
#endif

vthread_t vvp_fun_event_or_aa::add_waiting_thread(vthread_t thread)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vthread_get_wt_context_item(context_idx_));

      vthread_t tmp = state->threads;
      state->threads = thread;

      ಪ್ರತ್ಯಾಗಮ tmp;
}

ಶೂನ್ಯ vvp_fun_event_or_aa::recv_vec4(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
                                    vvp_context_t context)
{
      ಯದಿ (context) {
            waitable_state_s*state = static_cast<waitable_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            run_waiting_threads_(state->threads);
            base_net_->send_vec4(bit, context);
      } ಅನ್ಯಥಾ {
            context = context_scope_->live_contexts;
            ಯಾವತ್ (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
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
: vvp_named_event(h), threads_(0)
{
}

vvp_named_event_sa::~vvp_named_event_sa()
{
}

vthread_t vvp_named_event_sa::add_waiting_thread(vthread_t thread)
{
      vthread_t tmp = threads_;
      threads_ = thread;

      ಪ್ರತ್ಯಾಗಮ tmp;
}

ಶೂನ್ಯ vvp_named_event_sa::recv_vec4(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
                                   vvp_context_t)
{
      run_waiting_threads_(threads_);
      vvp_net_t*net = port.ptr();
      net->send_vec4(bit, 0);

      __vpiNamedEvent*obj = dynamic_cast<__vpiNamedEvent*>(handle_);
      ಪ್ರತಿಜ್ಞಾ(obj);
      obj->run_vpi_callbacks();
}

vvp_named_event_aa::vvp_named_event_aa(__vpiHandle*h)
: vvp_named_event(h)
{
      context_idx_ = vpip_add_item_to_context(ಏತತ್, vpip_peek_context_scope());
}

vvp_named_event_aa::~vvp_named_event_aa()
{
}

ಶೂನ್ಯ vvp_named_event_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, ನೂತನ waitable_state_s);
}

ಶೂನ್ಯ vvp_named_event_aa::reset_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->threads = 0;
}

#ifdef CHECK_WITH_VALGRIND
ಶೂನ್ಯ vvp_named_event_aa::free_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_item(context, context_idx_));
      delete state;
}
#endif

vthread_t vvp_named_event_aa::add_waiting_thread(vthread_t thread)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vthread_get_wt_context_item(context_idx_));

      vthread_t tmp = state->threads;
      state->threads = thread;

      ಪ್ರತ್ಯಾಗಮ tmp;
}

ಶೂನ್ಯ vvp_named_event_aa::recv_vec4(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
                                   vvp_context_t context)
{
      ಪ್ರತಿಜ್ಞಾ(context);

      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_item(context, context_idx_));

      run_waiting_threads_(state->threads);
      vvp_net_t*net = port.ptr();
      net->send_vec4(bit, context);
}

/*
**  Create an event functor
**  edge:  compile_event(label, type, argc, argv, debug_flag)
**  or:    compile_event(label, NULL, argc, argv, debug_flag)
**
**  Named events are handled elsewhere.
*/

ಸ್ಥಿರ ಶೂನ್ಯ compile_event_or(ವರ್ಣ*label, unsigned argc, ಸಂರಚನಾ symb_s*argv);

ಶೂನ್ಯ compile_event(ವರ್ಣ*label, ವರ್ಣ*type, unsigned argc, ಸಂರಚನಾ symb_s*argv)
{
      vvp_net_fun_t*fun = 0;

      ಯದಿ (type == 0) {
	    compile_event_or(label, argc, argv);
	    ಪ್ರತ್ಯಾಗಮ;
      }

      ಯದಿ (strcmp(type,"anyedge") == 0) {

	    ಮುಕ್ತ(type);

            ಯದಿ (vpip_peek_current_scope()->is_automatic()) {
                  fun = ನೂತನ vvp_fun_anyedge_aa;
            } ಅನ್ಯಥಾ {
                  fun = ನೂತನ vvp_fun_anyedge_sa;
            }

      } ಅನ್ಯಥಾ {

	    vvp_fun_edge::edge_t edge_type = vvp_edge_none;

	    ಯದಿ (strcmp(type,"posedge") == 0)
		  edge_type = vvp_edge_posedge;
	    ಅನ್ಯಥಾ ಯದಿ (strcmp(type,"negedge") == 0)
		  edge_type = vvp_edge_negedge;
	    ಅನ್ಯಥಾ ಯದಿ (strcmp(type,"edge") == 0)
		  edge_type = vvp_edge_edge;

	    ಪ್ರತಿಜ್ಞಾ(argc <= 4);
	    ಮುಕ್ತ(type);

            ಯದಿ (vpip_peek_current_scope()->is_automatic()) {
                  fun = ನೂತನ vvp_fun_edge_aa(edge_type);
            } ಅನ್ಯಥಾ {
                  fun = ನೂತನ vvp_fun_edge_sa(edge_type);
            }

      }

      vvp_net_t* ptr = ನೂತನ vvp_net_t;
      ptr->fun = fun;

      define_functor_ಚಿಹ್ನೆ(label, ptr);
      ಮುಕ್ತ(label);

      inputs_connect(ptr, argc, argv);
      ಮುಕ್ತ(argv);
}

ಸ್ಥಿರ ಶೂನ್ಯ compile_event_or(ವರ್ಣ*label, unsigned argc, ಸಂರಚನಾ symb_s*argv)
{
      vvp_net_t*base_net = ನೂತನ vvp_net_t;
      ಯದಿ (vpip_peek_current_scope()->is_automatic()) {
            base_net->fun = ನೂತನ vvp_fun_event_or_aa(base_net);
      } ಅನ್ಯಥಾ {
            base_net->fun = ನೂತನ vvp_fun_event_or_sa(base_net);
      }
      define_functor_ಚಿಹ್ನೆ(label, base_net);
      ಮುಕ್ತ(label);

	/* This is a simplified version of a wide functor. We don't
	   care about the data values or what port they arrived on,
	   so we can use a single shared functor. */
      vvp_net_t*curr_net = base_net;
      ಪ್ರತಿ (unsigned idx = 0 ;  idx < argc ;  idx += 1) {
	    ಯದಿ (idx > 0 && (idx % 4) == 0) {
		  curr_net = ನೂತನ vvp_net_t;
		  curr_net->fun = base_net->fun;
	    }
	    ಒಳಹರಿವು_connect(curr_net, idx % 4, argv[idx].text);
      }
      ಮುಕ್ತ(argv);
}

/*
 * This handles the compile of named events. This functor has no
 * inputs, it is only accessed by behavioral trigger statements, which
 * in vvp are %set instructions.
 */
ಶೂನ್ಯ compile_named_event(ವರ್ಣ*label, ವರ್ಣ*name, ತಾರ್ಕಿಕ local_flag)
{
      vvp_net_t*ptr = ನೂತನ vvp_net_t;

      vpiHandle obj = vpip_make_named_event(name, ptr);

      ಯದಿ (vpip_peek_current_scope()->is_automatic()) {
            ptr->fun = ನೂತನ vvp_named_event_aa(obj);
      } ಅನ್ಯಥಾ {
            ptr->fun = ನೂತನ vvp_named_event_sa(obj);
      }
      define_functor_ಚಿಹ್ನೆ(label, ptr);
      compile_vpi_ಚಿಹ್ನೆ(label, obj);
      ಯದಿ (! local_flag) vpip_attach_to_current_scope(obj);

      ಮುಕ್ತ(label);
      delete[] name;
}

#ifdef CHECK_WITH_VALGRIND
ಶೂನ್ಯ named_event_delete(__vpiHandle*handle)
{
      delete dynamic_cast<__vpiNamedEvent *>(handle);
}
#endif
